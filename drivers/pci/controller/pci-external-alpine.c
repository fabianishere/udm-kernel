/*
 *  Annapurna Labs PCI host bridge device tree driver
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
 * - This driver for external PCIe ports (in Root-Complex mode).
 * - The driver requires PCI_DOMAINS as each port registered as a pci domain
 * - for the external PCIe ports, the following applies:
 *	- Configuration access to bus 0 device 0 are routed to the configuration
 *	  space header register that found in the host bridge.
 *	- The driver assumes the controller link is initialized by the
 *	  bootloader.
 */

#include <linux/kernel.h>
#include <linux/export.h>
#include <linux/pci.h>
#include <linux/slab.h>

#include <linux/of_address.h>
#include <linux/of_pci.h>
#include <linux/of_irq.h>
#include <linux/of_platform.h>
#include <linux/io.h>
#include <al_hal_pcie.h>

#include "../pci.h"

#define AL_SB_PCIE_NUM (3)

/* PCI bridge private data */
struct al_pcie_pd {
	struct device *dev;
	struct list_head resources;

	struct resource busn;
	void __iomem *ecam_base;
	void __iomem *regs_base;

	resource_size_t io_base;

	struct al_pbs_regs __iomem *pbs_reg_base;
	void __iomem *local_bridge_config_space;
	unsigned int index;

	spinlock_t conf_lock;

	struct al_pcie_port pcie_port;
	struct al_pcie_link_status status;
	u8 target_bus;
};


static inline struct al_pcie_pd *sys_to_pcie(void *sys)
{
	return sys;
}

static int al_pcie_enable_controller(struct al_pcie_pd *pcie)
{
	int rc;
	enum al_pcie_operating_mode op_mode;

	rc = al_pcie_port_handle_init(&pcie->pcie_port,
				pcie->regs_base,
				pcie->pbs_reg_base,
				pcie->index);

	if(rc) {
		dev_err(pcie->dev, "controller could not be initialized\n");
		return -ENOSYS;
	}

	op_mode = al_pcie_operating_mode_get(&pcie->pcie_port);

	if (op_mode != AL_PCIE_OPERATING_MODE_RC) {
		dev_err(pcie->dev, "controller is not configured to Root-Complex mode\n");
		return -ENOSYS;
	}

	return 0;
}

static bool al_pcie_port_check_link(struct al_pcie_pd *pcie)
{
	struct al_pcie_link_status *status = &pcie->status;
	int rc;

	rc = al_pcie_link_status(&pcie->pcie_port, status);
	if (rc < 0) {
		dev_err(pcie->dev, "failed to get pcie link status\n");
		return false;
	}
	if (status->link_up == AL_FALSE) {
		dev_info(pcie->dev, "link %u down\n", pcie->index);
		return false;
	}
	dev_info(pcie->dev, "link up: speed Gen %d width x%x\n",
		 status->speed, status->lanes);

	return true;
}

/* prepare controller for issueing IO transactions */
static int al_pcie_io_prepare(struct al_pcie_pd *pcie)
{
	struct resource_entry *window;
	struct al_pcie_port *pcie_port = &pcie->pcie_port;

	struct al_pcie_atu_region io_atu_region = {
		.enable = AL_TRUE,
		.direction = AL_PCIE_ATU_DIR_OUTBOUND,
		.index = 0,
		.base_addr = 0ull,
		.limit = 0ull,
		/* the address that matches will be translated to this address + offset */
		.target_addr = 0ull,
		.invert_matching = AL_FALSE,
		.tlp_type = AL_PCIE_TLP_TYPE_IO, /* pcie tlp type*/
		.attr = 0, /* pcie frame header attr field*/
		/* outbound specific params */
		.msg_code = 0, /* pcie message code */
		.cfg_shift_mode = AL_FALSE,
		/* inbound specific params*/
		.enforce_ob_atu_region_set = AL_TRUE,
	};

	resource_list_for_each_entry(window, &pcie->resources) {
		struct resource *res = window->res;
		u64 restype = resource_type(res);

		switch (restype) {
		case IORESOURCE_IO:
			io_atu_region.base_addr = pcie->io_base;
			io_atu_region.limit = pcie->io_base + resource_size(res) - 1;
			io_atu_region.target_addr = res->start;
			break;
		}
	}

	BUG_ON(io_atu_region.base_addr == 0);
	dev_dbg(pcie->dev, "%s: base %llx, limit %llx, target %llx\n",
			__func__, io_atu_region.base_addr,
			io_atu_region.limit, io_atu_region.target_addr);
	al_pcie_atu_region_set(pcie_port, &io_atu_region);
	return 0;
}

#ifdef CONFIG_AL_PCIE_RMN_1010
/* prepare controller for issuing mem transactions */
static int al_pcie_mem_prepare(struct al_pcie_pd *pcie)
{
	struct al_pcie_port *pcie_port = &pcie->pcie_port;
	struct al_pcie_atu_region mem_atu_region;

	/*
	 * This region is meant to insure all accesses to this section
	 * will be always with type memory (accessing from DMA may
	 * change the type to IO).
	 */
	mem_atu_region.enable = AL_TRUE;
	mem_atu_region.direction = al_pcie_atu_dir_outbound;
	mem_atu_region.index = 1;
	mem_atu_region.base_addr = pcie->mem.start;
	mem_atu_region.limit = pcie->mem.end;
	mem_atu_region.target_addr = pcie->mem.start;
	mem_atu_region.invert_matching = AL_FALSE;
	mem_atu_region.tlp_type = AL_PCIE_TLP_TYPE_MEM; /* pcie tlp type*/
	mem_atu_region.attr = 0; /* pcie frame header attr field*/
	mem_atu_region.msg_code = 0; /* pcie message code */
	mem_atu_region.cfg_shift_mode = AL_FALSE;
	mem_atu_region.bar_number = 0; /* not used */
	mem_atu_region.match_mode = 0; /* address match mode */
	mem_atu_region.enable_attr_match_mode = AL_FALSE;
	mem_atu_region.enable_msg_match_mode = AL_FALSE;

	dev_dbg(pcie->dev, "%s: base %llx, limit %llx, target %llx\n",
			__func__, mem_atu_region.base_addr,
			mem_atu_region.limit, mem_atu_region.target_addr);

	al_pcie_atu_region_set(pcie_port, &mem_atu_region);

	return 0;
}
#endif

/* prepare controller for issueing CFG transactions*/
static int al_pcie_cfg_prepare(struct al_pcie_pd *pcie)
{
	struct al_pcie_port *pcie_port = &pcie->pcie_port;
	uint8_t secondary_busn = pcie->busn.start + 1;

	spin_lock_init(&pcie->conf_lock);
	pcie->target_bus = 1;
	/*
	 * force the controller to set the pci bus in the TLP to
	 * pcie->target_bus no matter what is the bus portion of the ECAM
	 * address is.
	 */
	al_pcie_target_bus_set(pcie_port, pcie->target_bus, 0xFF);

	/* the secondary bus connected to the controller always enumerate from DT 'bus-range' start value + 1 */
	al_pcie_secondary_bus_set(pcie_port, secondary_busn);
	/* set subordinary to max value */
	al_pcie_subordinary_bus_set(pcie_port, 0xff);

	return 0;
}

/* Get ECAM address according to bus, device, function, and offset */
static void __iomem *al_pcie_cfg_addr(struct al_pcie_pd *pcie,
					struct pci_bus *bus,
					unsigned int devfn, int offset)
{
	uint8_t primary_busn = pcie->busn.start;
	uint8_t max_busn = pcie->busn.end;
	void __iomem *ecam_base = pcie->ecam_base;
	unsigned int busnr = bus->number;
	int slot = PCI_SLOT(devfn);
	void __iomem *ret_val;

	/* Trap out illegal values */
	if (busnr > 255)
		BUG();
	if (devfn > 255)
		BUG();

	/* bus number should apply with bus range limitation */
	if (busnr >  max_busn) {
		dev_err(pcie->dev, "bus number exceed mas allowed bus number (%d > %d)\n",
				busnr, max_busn);
		return NULL;
	}

	ret_val = (ecam_base + ((PCI_SLOT(devfn) << 15) |
				(PCI_FUNC(devfn) << 12) |
				offset));

	/* If there is no link, just show the PCI bridge. */
	if ((pcie->status.link_up == AL_FALSE) && (busnr > 0 || slot > 0))
		return NULL;

	/* the bus connected to the controller always enumerate from DT 'bus-range' start value */
	if (busnr == primary_busn) {
		if (slot > 0)
			return NULL;

		ret_val = pcie->local_bridge_config_space;
		ret_val += offset;
	} else {
		if (busnr != pcie->target_bus) {
			dev_dbg(pcie->dev, "change target bus number from %d to %d\n",
				pcie->target_bus, busnr);
			pcie->target_bus = busnr;
			al_pcie_target_bus_set(&pcie->pcie_port,
						pcie->target_bus,
						0xFF);
		}
	}
	return ret_val;
}

/* PCI config space read */
static int al_read_config(struct pci_bus *bus, unsigned int devfn, int where,
				 int size, u32 *val)
{
	struct al_pcie_pd *pcie = sys_to_pcie(bus->sysdata);
	int rc = PCIBIOS_SUCCESSFUL;
	unsigned long flags;
	void __iomem *addr;
	u32 v = 0xffffffff;

	dev_dbg(pcie->dev, "read_config from %d size %d dev (domain %d) %d:%d:%d\n",
		where, size, pcie->index,
		bus->number, PCI_SLOT(devfn), PCI_FUNC(devfn));

	spin_lock_irqsave(&pcie->conf_lock, flags);

	addr = al_pcie_cfg_addr(pcie, bus, devfn, where);
	dev_dbg(pcie->dev, " read address %p\n", addr);

	if (addr) {
		switch (size) {
		case 1:
			v = readb(addr);
			break;
		case 2:
			v = readw(addr);
			break;
		case 4:
			v = readl(addr);
			break;
		default:
			rc = PCIBIOS_BAD_REGISTER_NUMBER;
		}
	} else {
		rc = PCIBIOS_DEVICE_NOT_FOUND;
	}

	spin_unlock_irqrestore(&pcie->conf_lock, flags);
	*val = v;

	dev_dbg(pcie->dev, " al_read_config %#x\n", *val);
	return rc;
}

/* PCI config space write */
static int al_write_config(struct pci_bus *bus, unsigned int devfn, int where,
				  int size, u32 val)
{
	struct al_pcie_pd *pcie = sys_to_pcie(bus->sysdata);
	int rc = PCIBIOS_SUCCESSFUL;
	unsigned long flags;
	void __iomem *addr;

	dev_dbg(pcie->dev, "write_config_byte %#x to %d size %d dev (domain %d) %d:%d:%d\n", val,
		where, size, pcie->index,
		bus->number, PCI_SLOT(devfn), PCI_FUNC(devfn));

	spin_lock_irqsave(&pcie->conf_lock, flags);

	addr = al_pcie_cfg_addr(pcie, bus, devfn, where);
	dev_dbg(pcie->dev, " write address %p\n", addr);
	if (addr) {
		switch (size) {
		case 1:
			writeb((u8)val, addr);
			break;

		case 2:
			writew((u16)val, addr);
			break;

		case 4:
			writel(val, addr);
			break;
		default:
			rc = PCIBIOS_BAD_REGISTER_NUMBER;
		}
	} else {
		rc = PCIBIOS_DEVICE_NOT_FOUND;
	}
	spin_unlock_irqrestore(&pcie->conf_lock, flags);
	return rc;
}

/* PCI bridge config space read/write operations */
static struct pci_ops al_pcie_ops = {
	.read	= al_read_config,
	.write	= al_write_config,
};

#define range_fill_resource(range, res) \
	do { \
		(res)->flags = (range).flags; \
		(res)->start = (range).cpu_addr; \
		(res)->end = (range).cpu_addr + (range).size - 1; \
		(res)->parent = (res)->child = (res)->sibling = NULL; \
	} while (0)

static int al_pcie_parse_dt(struct al_pcie_pd *pcie, struct platform_device *pdev)
{
	struct device_node *np = pcie->dev->of_node;
	struct resource_entry *window;
	struct resource regs;
	struct device_node *pbs_np;
	int cfg_space_offset;
	int err;
	int index;

	if (strncmp(np->name, "pcie-external", 13) != 0)
		index = -1;
	else
		index = np->name[13] - '0';

	/* Get configuration space offset */
	err = of_property_read_u32(np, "cfg-space-offset", &cfg_space_offset);
	if (err < 0) {
		dev_err(pcie->dev, "of_property_read_u32(): %d\n", err);
		return err;
	}

	/* Get registers resources */
	err = of_address_to_resource(np, 0, &regs);
	if (err < 0) {
		dev_dbg(pcie->dev, "of_address_to_resource(): %d\n",
			err);
		return err;
	}

	pcie->regs_base = devm_pci_remap_cfg_resource(pcie->dev, &regs);
	if (IS_ERR(pcie->regs_base))
		return PTR_ERR(pcie->regs_base);

	pbs_np = of_find_compatible_node(NULL, NULL, "annapurna-labs,al-pbs");

	pcie->pbs_reg_base = of_iomap(pbs_np, 0);
	if (!(pcie->pbs_reg_base)) {
		dev_err(pcie->dev, "%s: pbs_reg_base map failed\n", __func__);
		return -ENOMEM;
	}

	/* set the base address of the configuration space of the local
	 * bridge
	 */
	pcie->local_bridge_config_space = pcie->regs_base + cfg_space_offset;

	err = devm_of_pci_get_host_bridge_resources(&pdev->dev, 0, 0xff,
						    &pcie->resources,
						    &pcie->io_base);
	if (err) {
		dev_err(pcie->dev, "Getting bridge resources failed\n");
		return err;
	}

	err = devm_request_pci_bus_resources(pcie->dev, &pcie->resources);

	resource_list_for_each_entry(window, &pcie->resources) {
		struct resource *res = window->res;
		u64 restype = resource_type(res);

		switch (restype) {
		case 0:
			res->flags = IORESOURCE_MEM;
			pcie->ecam_base = devm_ioremap_resource(pcie->dev, res);
			res->flags = 0;
			break;
		}
	}
	if (IS_ERR(pcie->ecam_base)) {
		dev_err(pcie->dev, "Unable to find ecam base, failing\n");
		return PTR_ERR(pcie->ecam_base);
	}

	err = of_pci_parse_bus_range(np, &pcie->busn);
	if (err < 0) {
		dev_err(pcie->dev, "failed to parse ranges property: %d\n",
			err);
		pcie->busn.name = np->name;
		pcie->busn.start = 1;
		pcie->busn.end = 0xff;
		pcie->busn.flags = IORESOURCE_BUS;
	}
	pcie->index = index;
	return 0;
}

static const struct of_device_id al_pcie_of_match[] = {
	{ .compatible = "annapurna-labs,alpine-external-pcie"},
	{ },
};


static int al_pcie_probe(struct platform_device *pdev)
{
	struct al_pcie_pd *pcie;
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	struct pci_host_bridge *bridge;
	struct pci_bus *bus, *child;
	int err;

	bridge = devm_pci_alloc_host_bridge(dev, sizeof(*pcie));
	if (!bridge)
		return -ENOMEM;

	pcie = pci_host_bridge_priv(bridge);
	pcie->dev = dev;

	INIT_LIST_HEAD(&pcie->resources);

	err = al_pcie_parse_dt(pcie, pdev);
	if (err < 0)
		goto free_resources;

	err = al_pcie_enable_controller(pcie);
	if (err)
		goto free_resources;

	al_pcie_port_check_link(pcie);

	al_pcie_port_check_link(pcie);
	al_pcie_cfg_prepare(pcie);

	al_pcie_io_prepare(pcie);

	if (of_dma_is_coherent(np)) {
		dev_dbg(pcie->dev, "Enabling pcie snoop\n");
		al_pcie_port_snoop_config(&pcie->pcie_port, 1);
	}

	list_splice_init(&pcie->resources, &bridge->windows);
	bridge->dev.parent = dev;
	bridge->sysdata = pcie;
	bridge->busnr = pcie->busn.start;
	bridge->ops = &al_pcie_ops;
	bridge->map_irq = of_irq_parse_and_map_pci;
	bridge->swizzle_irq = pci_common_swizzle;

	err = pci_scan_root_bus_bridge(bridge);
	if (err < 0)
		goto free_resources;

	bus = bridge->bus;
	pci_assign_unassigned_bus_resources(bus);
	list_for_each_entry(child, &bus->children, node)
		pcie_bus_configure_settings(child);
	pci_bus_add_devices(bus);

	platform_set_drvdata(pdev, pcie);
	return 0;

free_resources:
	pci_free_resource_list(&pcie->resources);
	return err;
}

static struct platform_driver al_pcie_driver = {
	.driver = {
		.name = "al-external-pcie",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(al_pcie_of_match),
	},
	.probe = al_pcie_probe,
};
builtin_platform_driver(al_pcie_driver);
