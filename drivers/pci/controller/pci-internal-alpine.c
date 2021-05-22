/*
 *  Annapurna Labs internal PCI host bridge driver
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
#include <linux/kernel.h>
#include <linux/export.h>
#include <linux/pci.h>
#include <linux/slab.h>

#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_pci.h>
#include <linux/of_irq.h>
#include <linux/of_platform.h>

#include "../pci.h"

/* PCI bridge private data */
struct al_pcie_pd {
	struct device *dev;

	void __iomem *ecam_base;
};


/* Get ECAM address according to bus, device, function, and offset */
static void __iomem *al_pcie_cfg_addr(struct al_pcie_pd *al_pcie,
				      struct pci_bus *bus,
				      unsigned int devfn, int offset)
{
	void __iomem *ecam_base = al_pcie->ecam_base;
	unsigned int busnr = bus->number;

	/* Trap out illegal values */
	if (busnr > 255)
		BUG();
	if (devfn > 255)
		BUG();

	return (ecam_base + ((PCI_SLOT(devfn) << 15) | (PCI_FUNC(devfn) << 12) | offset));
}

/* PCI config space read */
static int al_internal_read_config(struct pci_bus *bus, unsigned int devfn,
				   int where, int size, u32 *val)
{
	struct al_pcie_pd *al_pcie = bus->sysdata;
	void __iomem *addr = al_pcie_cfg_addr(al_pcie, bus, devfn, where & ~3);
	u32 v;

	pr_debug("read_config from %d size %d dev %d:%d:%d\n", where, size,
		 bus->number, PCI_SLOT(devfn), PCI_FUNC(devfn));

	v = readl(addr);
	switch (size) {
	case 1:
		v = (v >> ((where&0x3)*8)) & 0xff;
		break;
	case 2:
		v = (v >> ((where&0x3)*8)) & 0xffff;
		break;
	default:
		break;
	}

	*val = v;
	pr_debug("read_config_byte read %#x\n", *val);
	return PCIBIOS_SUCCESSFUL;
}

/* PCI config space write */
static int al_internal_write_config(struct pci_bus *bus, unsigned int devfn,
				    int where, int size, u32 val)
{
	struct al_pcie_pd *al_pcie = bus->sysdata;
	void __iomem *addr = al_pcie_cfg_addr(al_pcie, bus, devfn, where);

	pr_debug("write_config %#x to %d size %d dev %d:%d:%d\n", val,
		where, size, bus->number, PCI_SLOT(devfn), PCI_FUNC(devfn));

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
	}

	return PCIBIOS_SUCCESSFUL;
}

/* PCI bridge config space read/write operations */
static struct pci_ops al_internal_pcie_ops = {
	.read	= al_internal_read_config,
	.write	= al_internal_write_config,
};

static const struct of_device_id al_pcie_of_match[] = {
	{ .compatible = "annapurna-labs,alpine-internal-pcie"},
	{ },
};

static int al_pci_internal_device_notifier(struct notifier_block *nb,
					   unsigned long event, void *__dev)
{
	struct device *dev = __dev;
	struct pci_dev *pdev = to_pci_dev(dev);
	u32 temp;

	if (event != BUS_NOTIFY_BIND_DRIVER)
		return NOTIFY_DONE;

	arch_setup_dma_ops(dev, 0 /*dma_base*/, 0 /*size*/, 0 /*iommu*/, 1 /* coherent */);

	/* Force the PCIE adapter to set AXI attr to match CC*/
	if (pdev->bus->number == 0) {
		if (PCI_FUNC(pdev->devfn) != 0) {
			dev_dbg(&pdev->dev, "this is a VF, no need to initialize\n");
			return NOTIFY_DONE;
		}

		pci_read_config_dword(pdev, 0x110 ,&temp);
		temp |= 0x3;
		pci_write_config_dword(pdev, 0x110 ,temp);
		/* Enable cache coherency for VF's (except USB and SATA) */
		if (PCI_SLOT(pdev->devfn) < 6) {
			dev_err(&pdev->dev, "writing to VF config space\n");
			pci_write_config_dword(pdev, 0x130 ,temp);
			pci_write_config_dword(pdev, 0x150 ,temp);
			pci_write_config_dword(pdev, 0x170 ,temp);
		}

		pci_read_config_dword(pdev, 0x220 ,&temp);
		temp &= ~0xffff;
		temp |= 0x3ff;
		pci_write_config_dword(pdev, 0x220 ,temp);
	} else {
		dev_dbg(&pdev->dev, "no need to update config space for bus[%d]\n",
			pdev->bus->number);
	}

	return NOTIFY_OK;
}

static struct notifier_block al_pci_internal_device_nb = {
	.notifier_call = al_pci_internal_device_notifier,
};

static int al_pcie_probe(struct platform_device *pdev)
{
	struct al_pcie_pd *al_pcie;
	int err;
	struct pci_bus *bus;
	struct device_node *np = pdev->dev.of_node;
	struct resource *res;
	bool coherent;
	LIST_HEAD(pci_resources);

	al_pcie = devm_kzalloc(&pdev->dev, sizeof(*al_pcie), GFP_KERNEL);
	if (!al_pcie)
		return -ENOMEM;

	al_pcie->dev = &pdev->dev;

	/* Get the ECAM from DT */
	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "ecam");
	al_pcie->ecam_base = devm_ioremap_resource(al_pcie->dev, res);
	if (IS_ERR(al_pcie->ecam_base))
		return -EADDRNOTAVAIL;

	err = devm_of_pci_get_host_bridge_resources(&pdev->dev, 0, 0xff,
						    &pci_resources, NULL);
	if (err)
		return err;

	/* add notifier to handle IOCC */
	coherent = of_dma_is_coherent(np);
	dev_dbg(al_pcie->dev,"IOCC %s!\n", coherent ? "enabled" : "disabled");
	if (coherent) {
		bus_register_notifier(&pci_bus_type, &al_pci_internal_device_nb);
	}

	bus = pci_scan_root_bus(&pdev->dev, 0, &al_internal_pcie_ops,
				 al_pcie, &pci_resources);
	if (!bus)
		return -ENOMEM;

	if (!pci_has_flag(PCI_PROBE_ONLY)) {
		/*
		 * Size the bridge windows.
		 */
		pci_bus_size_bridges(bus);

		/*
		 * Assign resources.
		 */
		pci_bus_assign_resources(bus);
	}

	/*
	 * Tell drivers about devices found.
	 */
	pci_bus_add_devices(bus);

	platform_set_drvdata(pdev, al_pcie);

	return 0;
}

static struct platform_driver al_pcie_driver = {
	.driver = {
		.name = "al-internal-pcie",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(al_pcie_of_match),
	},
	.probe = al_pcie_probe,
};
builtin_platform_driver(al_pcie_driver);
