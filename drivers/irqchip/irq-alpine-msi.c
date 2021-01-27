/*
 * Annapurna Labs MSIX support services
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
 /*
  * reference: irq-gic-v2m.c
  */
#define DEBUG
#include <linux/slab.h>
#include <linux/pci.h>
#include <linux/msi.h>
#include <asm/irq.h>
#include <linux/irqchip/arm-gic.h>

#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/of_pci.h>

#include <asm-generic/msi.h>

#include "irqchip.h"

#define ALPINE_MSIX_DEFAULT_INTERRUPT_RANGE_FIRST	161
#define ALPINE_MSIX_DEFAULT_INTERRUPT_RANGE_NUM_IRQS	158

/*
 * generic msi-x level: generic chip/msi domain
 */
//default doesn't go to parent
static void alpine_mask_msi_irq(struct irq_data *d)
{
	pci_msi_mask_irq(d);
	irq_chip_mask_parent(d);
}

//default doesn't go to parent
static void alpine_unmask_msi_irq(struct irq_data *d)
{
	pci_msi_unmask_irq(d);
	irq_chip_unmask_parent(d);
}

//default can't handle if not DONE
static int alpine_set_affinity(struct irq_data *irq_data,
			       const struct cpumask *mask, bool force)
{
	int ret;

	ret = irq_chip_set_affinity_parent(irq_data, mask, force);
	if (ret == IRQ_SET_MASK_OK)
		ret = IRQ_SET_MASK_OK_DONE;

	return ret;
}

static struct irq_chip alpine_msix_irq_chip = {
	.name			= "MSIx",
	.irq_mask		= alpine_mask_msi_irq,
	.irq_unmask		= alpine_unmask_msi_irq, //default doesn't go to parent
	.irq_eoi		= irq_chip_eoi_parent, //no default
	.irq_set_affinity	= alpine_set_affinity,
//	.irq_write_msi_msg	= pci_msi_domain_write_msg, //default
};

/*
 * middle level:
 * allocates/releases avialable msi-x interrupts
 * alloc ignore agrs
 * hwirq is sgi# (32 less then irq#!)
 * gets called from msi-domain child
 * calls gic-domain parent
 */

struct alpine_msix_data {
	struct msi_controller controller;
	struct irq_domain *middle_domain;
	spinlock_t alloc_lock;

	u32 al_msix_addr_high;
	u32 al_msix_addr_low;

	u32 al_msix_sgi_first;		/* The SPI number that MSIs start */
	u32 al_msix_num_irqs;		/* The number of SPIs for MSIs */

	char *al_msix_use_map;

	struct resource res;	/* GICv2m resource */
};

static int al_msix_allocate_sgi(struct alpine_msix_data *mdata, int num_req)
{
	int pos, first, avail;

	spin_lock(&mdata->alloc_lock);
	avail=pos=first=0;
	while (pos < mdata->al_msix_num_irqs) {
		if (!mdata->al_msix_use_map[pos]) {
			if (!avail)
				first = pos;
			avail++;
			if (avail == num_req)
				break;
		} else {
			avail = 0;
		}
		pos++;
	}
	if (avail == num_req) {
		for (pos=first; pos < first+avail; pos++) {
			mdata->al_msix_use_map[pos] = 1;
		}
	}
	spin_unlock(&mdata->alloc_lock);
	if (avail != num_req)
		return -ENOSPC;
	return mdata->al_msix_sgi_first + first;
}

void al_msix_free_sgi(struct alpine_msix_data *mdata, unsigned int sgi, int num_req)
{
	int pos;
	int first = sgi - mdata->al_msix_sgi_first;
	spin_lock(&mdata->alloc_lock);
	for (pos=first; pos < first+num_req; pos++) {
		mdata->al_msix_use_map[pos] = 0;
	}
	spin_unlock(&mdata->alloc_lock);
}

static void alpine_compose_msi_msg(struct irq_data *data, struct msi_msg *msg)
{
	struct alpine_msix_data *mdata = irq_data_get_irq_chip_data(data);

	msg->address_hi = mdata->al_msix_addr_high;
	msg->address_lo = mdata->al_msix_addr_low + (data->hwirq << 3);
	msg->data = 0;
}

static struct msi_domain_info alpine_msix_domain_info = {
	.flags	= (MSI_FLAG_USE_DEF_DOM_OPS | MSI_FLAG_USE_DEF_CHIP_OPS
		   | MSI_FLAG_PCI_MSIX),
	.chip	= &alpine_msix_irq_chip,
};


static struct irq_chip middle_irq_chip = {
	.name			= "alpine_middle",
	.irq_mask		= irq_chip_mask_parent,
	.irq_unmask		= irq_chip_unmask_parent,
	.irq_eoi		= irq_chip_eoi_parent,
	.irq_set_affinity	= irq_chip_set_affinity_parent,
	.irq_compose_msi_msg	= alpine_compose_msi_msg,
};

static int gic_domain_alloc(struct irq_domain *gic_domain,
				       unsigned int virq, int sgi,
				       unsigned int count)
{
	struct of_phandle_args args;
	struct irq_data *d;
	int err, i;

	args.np = gic_domain->of_node;
	args.args_count = 3;
	args.args[0] = 0;
	args.args[2] = IRQ_TYPE_EDGE_RISING;

	for (i=0; i<count; i++) {
		args.args[1] = sgi + i;
		err = gic_domain->ops->alloc(gic_domain, virq + i, 1, &args);
		BUG_ON(err);
		d = irq_domain_get_irq_data(gic_domain, virq + i);
		d->chip->irq_set_type(d, IRQ_TYPE_EDGE_RISING);
	}
	return 0;
}

static int middle_domain_alloc(struct irq_domain *domain, unsigned int virq,
				   unsigned int nr_irqs, void *args)
{
	struct alpine_msix_data *mdata = domain->host_data;
	int sgi, err,i;

	sgi = al_msix_allocate_sgi(mdata, nr_irqs);

	if (sgi < 0)
		return sgi;

	err = gic_domain_alloc(domain->parent, virq, sgi, nr_irqs);
	if (err) {
		al_msix_free_sgi(mdata, sgi, nr_irqs);
		return err;
	}

	for (i=0; i<nr_irqs; i++) {
		irq_domain_set_hwirq_and_chip(domain, virq + i, sgi + i, &middle_irq_chip, mdata);
	}

	return 0;
}

static void middle_domain_free(struct irq_domain *domain,
				   unsigned int virq, unsigned int nr_irqs)
{
	struct irq_data *d = irq_domain_get_irq_data(domain, virq);
	struct alpine_msix_data *mdata = irq_data_get_irq_chip_data(d);

	al_msix_free_sgi(mdata, d->hwirq, nr_irqs);
	irq_domain_free_irqs_parent(domain, virq, nr_irqs);
}

static const struct irq_domain_ops middle_domain_ops = {
	.alloc			= middle_domain_alloc,
	.free			= middle_domain_free,
};

int alpine_msix_init(struct device_node *node, struct device_node *parent)
{
	struct alpine_msix_data *mdata;
	struct irq_domain *gic_domain;
	struct device_node *gic_node;
	int status = 0;
	struct resource res;
	u32 first, num_irqs;

	mdata = kzalloc(sizeof(struct alpine_msix_data), GFP_KERNEL);

	if (of_address_to_resource(node, 0, &res))
		BUG_ON(1);

	mdata->al_msix_addr_high = ((u64)res.start) >> 32;
	mdata->al_msix_addr_low = (res.start & 0xffffffff) + (1<<16);

	if (of_property_read_u32(node, "al,msi-base-spi", &first)) {
		pr_warn("Unable to parse MSI base, using default: %d\n",
				ALPINE_MSIX_DEFAULT_INTERRUPT_RANGE_FIRST);
		first = ALPINE_MSIX_DEFAULT_INTERRUPT_RANGE_FIRST;
	}
	if (of_property_read_u32(node, "al,msi-num-spis", &num_irqs)) {
		pr_warn("Unable to parse MSI count, using default: %d\n",
				ALPINE_MSIX_DEFAULT_INTERRUPT_RANGE_NUM_IRQS);
		num_irqs = ALPINE_MSIX_DEFAULT_INTERRUPT_RANGE_NUM_IRQS;
	}

	mdata->al_msix_num_irqs = num_irqs;
	mdata->al_msix_sgi_first = first;
	pr_debug("al-msix: registering %d msixs, starting with %d\n", mdata->al_msix_num_irqs, mdata->al_msix_sgi_first);

	mdata->al_msix_use_map = kzalloc(mdata->al_msix_num_irqs, GFP_KERNEL);
	BUG_ON(!mdata->al_msix_use_map);

	gic_node = of_irq_find_parent(node); // of_parse_phandle(np, "interrupt-parent", 0);
	BUG_ON(!gic_node);
	gic_domain = irq_find_host(gic_node);

	mdata->middle_domain = irq_domain_add_tree(NULL, &middle_domain_ops, mdata);
	BUG_ON(!mdata->middle_domain);

	mdata->middle_domain->parent = gic_domain;

	mdata->controller.of_node = node;
	mdata->controller.domain = pci_msi_create_irq_domain(node,
						      &alpine_msix_domain_info,
						      mdata->middle_domain);
	BUG_ON(!mdata->controller.domain);
	spin_lock_init(&mdata->alloc_lock);

	status = of_pci_msi_chip_add(&mdata->controller);
	if (status < 0) {
		kfree(mdata);
		return status;
	}

	return 0;
}
IRQCHIP_DECLARE(alpine_msix, "annapurna-labs,alpine-msix", alpine_msix_init);
