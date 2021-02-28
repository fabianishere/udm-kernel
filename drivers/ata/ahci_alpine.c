/*
 *  ahci.c - AHCI SATA support
 *
 *  Maintained by:  Tejun Heo <tj@kernel.org>
 *    		    Please ALWAYS copy linux-ide@vger.kernel.org
 *		    on emails.
 *
 *  Copyright 2004-2005 Red Hat, Inc.
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 * libata documentation is available via 'make {ps|pdf}docs',
 * as Documentation/DocBook/libata.*
 *
 * AHCI hardware documentation:
 * http://www.intel.com/technology/serialata/pdf/rev1_0.pdf
 * http://www.intel.com/technology/serialata/pdf/rev1_1.pdf
 *
 */
#define DEBUG 1
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/blkdev.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/dma-mapping.h>
#include <linux/device.h>
#include <linux/dmi.h>
#include <linux/gfp.h>
#include <scsi/scsi_host.h>
#include <scsi/scsi_cmnd.h>
#include <linux/libata.h>
#include "ahci.h"
#include "ahci_alpine.h"

#include <al_hal_unit_adapter_regs.h>
#include <al_hal_unit_adapter.h>
#include <al_hal_iofic.h>
#include <al_hal_iofic_regs.h>

#define al_ahci_iofic_base(base)	((base) + 0x2000)
#define AL_AHCI_SPEED_AN_TRIES			(16)

struct alpine_host_priv {
	/* for msix interrupts */
	unsigned int msix_vecs;
	struct msix_entry msix_entries[];
};

void ahci_port_intr(struct ata_port *ap);

int al_init_msix_interrupts(struct pci_dev *pdev, unsigned int n_ports,
				struct ahci_host_priv *hpriv)
{
	int i, rc;
	void __iomem *iofic_base = al_ahci_iofic_base(hpriv->mmio);
	struct alpine_host_priv *al_data;

	hpriv->plat_data = NULL;

	al_data = kzalloc(
		sizeof(unsigned int) + n_ports * sizeof(struct msix_entry), GFP_KERNEL);

	if (!al_data)
		return -ENOMEM;

	al_data->msix_vecs = n_ports;

	for (i = 0; i < n_ports; i++) {
		/* entries 0-2 are in group A */
		al_data->msix_entries[i].entry = 3 + i;
		al_data->msix_entries[i].vector = 0;
	}

	rc = pci_enable_msix_exact(pdev, al_data->msix_entries, n_ports);

	if (rc) {
		dev_info(&pdev->dev,"failed to enable MSIX, vectors %d rc %d\n",
				n_ports, rc);
		kfree(al_data);
		return -EPERM;
	}

	/* we use only group B */
	al_iofic_config(iofic_base, 1 /*GROUP_B*/,
			INT_CONTROL_GRP_SET_ON_POSEDGE |
			INT_CONTROL_GRP_AUTO_CLEAR |
			INT_CONTROL_GRP_AUTO_MASK |
			INT_CONTROL_GRP_CLEAR_ON_READ);

	al_iofic_moder_res_config(iofic_base, 1, 15);

        al_iofic_unmask(iofic_base, 1, (1 << n_ports) - 1);

	hpriv->plat_data = al_data;

	return n_ports;
}
EXPORT_SYMBOL(al_init_msix_interrupts);

void alpine_clean_cause(struct ata_port *ap_this)
{
	struct ata_host *host = ap_this->host;
	struct ahci_host_priv *hpriv = host->private_data;
	void __iomem *iofic_base = al_ahci_iofic_base(hpriv->mmio);

	/* clean host cause */
	writel(1 << ap_this->port_no, hpriv->mmio + HOST_IRQ_STAT);

	/* unmask the interrupt in the iofic (auto-masked) */
	al_iofic_unmask(iofic_base, 1, 1 << ap_this->port_no);
}
EXPORT_SYMBOL(alpine_clean_cause);

int al_port_irq(struct ata_host *host, int port)
{
	struct ahci_host_priv *hpriv = host->private_data;
	struct alpine_host_priv *al_priv = hpriv->plat_data;

	return al_priv->msix_entries[port].vector;
}
EXPORT_SYMBOL(al_port_irq);

static int al_ahci_read_pcie_config(void *handle, int where, uint32_t *val)
{
	/* handle is a pointer to the pci_dev */
	pci_read_config_dword((struct pci_dev *)handle, where, val);
	return 0;
}

static int al_ahci_write_pcie_config(void *handle, int where, uint32_t val)
{
	/* handle is a pointer to the pci_dev */
	pci_write_config_dword((struct pci_dev *)handle, where, val);
	return 0;
}

static int al_ahci_write_pcie_flr(void *handle)
{
	/* handle is a pointer to the pci_dev */
	__pci_reset_function_locked((struct pci_dev *)handle);
	udelay(1000);
	return 0;
}

void al_ahci_flr(struct pci_dev *pdev)
{
	struct al_unit_adapter unit_adapter;
	int status;

	status = al_unit_adapter_handle_init(&unit_adapter,
					AL_UNIT_ADAPTER_TYPE_SATA, 0,
					al_ahci_read_pcie_config,
					al_ahci_write_pcie_config,
					al_ahci_write_pcie_flr,
					pdev);
	if (status) {
		BUG();
		return;
	}

	al_pcie_perform_flr(&unit_adapter);
}
EXPORT_SYMBOL(al_ahci_flr);

bool al_ahci_sss_wa_needed(struct device *dev)
{
	struct pci_dev *pdev = to_pci_dev(dev);

	return (pdev->vendor == PCI_VENDOR_ID_ANNAPURNA_LABS) &&
		al_ahci_sss_enabled;
}
EXPORT_SYMBOL(al_ahci_sss_wa_needed);

static inline int al_sata_phy_reset(struct ata_link *link)
{
	int rc;
	u32 scontrol;

	rc = sata_scr_read(link, SCR_CONTROL, &scontrol);

	if (rc)
		return rc;
	/*Speed negotiation is not finished, reset SATA link once again*/
	scontrol = (scontrol & 0x0f0) | 0x301;

	rc = sata_scr_write_flush(link, SCR_CONTROL, scontrol);
	if (rc)
		return rc;

	ata_msleep(link->ap, 1);
	scontrol = (scontrol & 0x0f0) | 0x300;
	return sata_scr_write_flush(link, SCR_CONTROL, scontrol);
}


int al_sata_link_hardreset(struct ata_link *link,
		const unsigned long *timing, unsigned long deadline)
{
	u32 sstatus;
	int i, rc = 0;

	rc = sata_scr_read(link, SCR_STATUS, &sstatus);
	if (rc)
		return rc;

	/*reset to determine link state*/
	rc = al_sata_phy_reset(link);
	if (rc)
		return rc;

	if (!(sstatus & 0xF))
		goto out;

	ahci_power_up(link->ap);

	for (i = 0; i < AL_AHCI_SPEED_AN_TRIES; i++) {
		udelay(200);
		rc = sata_scr_read(link, SCR_STATUS, &sstatus);
		if (rc)
			goto out;
		/*Check if device is present*/
		if (!(sstatus & 0x3)) {
			rc = 0;
			goto out;
		}
		/*Check negotiated speed*/
		if (sstatus & 0xF0)
			break;
		ata_dev_warn(&link->device[0], "WARNING: repeat PHY hardreset for port\n");

		rc = al_sata_phy_reset(link);
		if (rc)
			return rc;
	}
	if (sstatus & 0x3) {
		u32 serror;

		rc = sata_link_debounce(link, timing, deadline);
		if (rc)
			goto out;

		rc = sata_scr_read(link, SCR_ERROR, &serror);
		if (!rc)
			sata_scr_write(link, SCR_ERROR, serror);

		al_ahci_port_start(link->ap);
		return 0;
	}
out:
	al_ahci_port_stop(link->ap);
	return rc;
}
EXPORT_SYMBOL(al_sata_link_hardreset);
