/**
 * Copyright 2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 */

#include <linux/export.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/io.h>
#include <linux/irq.h>
#include <linux/irqchip.h>
#include <linux/irqchip/chained_irq.h>
#include <linux/irqdomain.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/syscore_ops.h>
#include <linux/device.h>
#include <linux/slab.h>

#include <asm/exception.h>
#include <asm/irq.h>

#include "al_hal_pbs_iofic.h"
#include <linux/of.h>
#include <linux/of_address.h>
#include "al_hal_iofic_regs.h"

struct alpine_iofic {
	void __iomem *base;
	bool trigger_posedge;
	u32 interrupts_in;
	int parent_irq;
	int irq_base;
	struct irq_chip chip;
	struct irq_domain *domain;
};

static int alpine_iofic_irqdomain_map(struct irq_domain *domain,
				      unsigned int irq,
				      irq_hw_number_t hwirq)
{
	struct alpine_iofic *iofic = domain->host_data;
	irq_flow_handler_t handle;

	handle = iofic->trigger_posedge ?
		handle_edge_irq : handle_level_irq;

	irq_set_chip_data(irq, iofic);
	irq_set_chip_and_handler(irq, &iofic->chip, handle);
	irq_set_probe(irq);

	return 0;
}

static const struct irq_domain_ops alpine_iofic_irqdomain_ops = {
	.map = alpine_iofic_irqdomain_map,
	.xlate = irq_domain_xlate_onecell,
};

void alpine_iofic_irq_handler(struct irq_desc *desc)
{
	struct alpine_iofic *iofic = irq_desc_get_handler_data(desc);
	unsigned long pending;
	unsigned long mask;
	int hwirq;
	struct irq_chip *irqchip = irq_desc_get_chip(desc);
	unsigned int virtq;

	chained_irq_enter(irqchip, desc);

	mask = al_iofic_read_mask(iofic->base, AL_INT_GROUP_A);
	pending = al_iofic_read_cause(iofic->base, AL_INT_GROUP_A);
	pending &= ~mask;

	/* deassert pending edge-triggered irqs */
	if (iofic->trigger_posedge)
		al_iofic_clear_cause(iofic->base, AL_INT_GROUP_A, pending);

	/* handle pending irqs */
	if (likely(pending)) {
		for_each_set_bit(hwirq, &pending, iofic->interrupts_in) {
			virtq = irq_find_mapping(iofic->domain, hwirq);
			generic_handle_irq(virtq);
		}
	}

	/* deassert pending level-triggered irqs */
	if (!iofic->trigger_posedge)
		al_iofic_clear_cause(iofic->base, AL_INT_GROUP_A, pending);

	chained_irq_exit(irqchip, desc);
}

static void alpine_iofic_irq_mask(struct irq_data *data)
{
	struct alpine_iofic *iofic = irq_data_get_irq_chip_data(data);

	al_iofic_mask(iofic->base, AL_INT_GROUP_A, BIT(data->hwirq));
}

static void alpine_iofic_irq_unmask(struct irq_data *data)
{
	struct alpine_iofic *iofic = irq_data_get_irq_chip_data(data);

	al_iofic_unmask(iofic->base, AL_INT_GROUP_A, BIT(data->hwirq));
}

static int alpine_iofic_irqcip_init(struct device_node *node,
				    struct alpine_iofic *iofic)
{
	unsigned long hwirq;
	unsigned long virtq;

	iofic->chip.name = node->name;
	iofic->chip.irq_mask = alpine_iofic_irq_mask;
	iofic->chip.irq_unmask = alpine_iofic_irq_unmask;

	irq_set_chained_handler_and_data(iofic->parent_irq,
					 alpine_iofic_irq_handler,
					 iofic);

	iofic->domain = irq_domain_add_simple(node,
					      iofic->interrupts_in,
					      0,
					      &alpine_iofic_irqdomain_ops,
					      iofic);
	if (!iofic->domain) {
		pr_err("%s(): fail to add irq domain\n", __func__);
		return -EINVAL;
	}

	for (hwirq = 0; hwirq < iofic->interrupts_in; hwirq++) {
		virtq = irq_create_mapping(iofic->domain, hwirq);
		if (!virtq) {
			pr_err("%s(): fail to map irq %lu\n", __func__, hwirq);
			return -EINVAL;
		}
		pr_debug("%s(): hwirq %lu mapped to virtq %lu\n",
		       __func__, hwirq, virtq);
	}

	return 0;
}

static int __init alpine_iofic_init(struct device_node *node,
				    struct device_node *parent)
{
	struct alpine_iofic *iofic;
	int ret;
	bool trigger_posedge;

	if (!parent) {
		pr_err("%s(): iofic doesn't support parentless mode\n",
		       __func__);
		return -EINVAL;
	}

	iofic = kzalloc(sizeof(struct alpine_iofic), GFP_KERNEL);
	if (!iofic) {
		pr_err("%s(): fail allocate memory for iofic\n", __func__);
		return -ENOMEM;
	}

	iofic->base = of_iomap(node, 0);
	if (!iofic->base) {
		pr_err("%s(): fail to map memory\n", __func__);
		return -ENOMEM;
	}

	trigger_posedge = of_property_read_bool(node, "trigger-posedge");
	ret = al_iofic_config(iofic->base,
			      AL_INT_GROUP_A,
			      trigger_posedge ?
			      INT_CONTROL_GRP_SET_ON_POSEDGE : 0);
	if (ret) {
		pr_err("%s(): fail to configure iofic\n",
		       __func__);
		return -EINVAL;
	}

	ret = of_property_read_u32(node,
				   "interrupts-in",
				   &iofic->interrupts_in);
	if (ret) {
		pr_err("%s(): fail to read number of interrupts for iofic\n",
		       __func__);
		return -EINVAL;
	}

	iofic->parent_irq =  irq_of_parse_and_map(node, 0);
	if (!iofic->parent_irq) {
		pr_err("%s(): fail to map irq\n", __func__);
		return -EINVAL;
	}

	ret = alpine_iofic_irqcip_init(node, iofic);
	if (ret) {
		pr_err("%s(): fail to read number of interrupts for iofic\n",
		       __func__);
		return ret;
	}

	al_iofic_clear_cause(iofic->base, AL_INT_GROUP_A, ~0);

	pr_info("%s() succesfuly initialized %s  base_irq=%d (parent: %s)\n",
		__func__,
		of_node_full_name(node),
		iofic->irq_base,
		of_node_full_name(parent));

	return 0;
}

IRQCHIP_DECLARE(al_iofic, "al,alpine-iofic", alpine_iofic_init);
