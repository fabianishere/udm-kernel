/*
 * drivers/crypto/al_crypto_main.c
 *
 * Annapurna Labs Crypto driver - pci enumeration and init invocation
 *
 * Copyright (C) 2012 Annapurna Labs Ltd.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * The full GNU General Public License is included in this distribution in
 * the file called "COPYING".
 *
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/dma-mapping.h>

#include <linux/delay.h>

#include "al_crypto.h"
#include "al_crypto_module_params.h"

#include <al_hal_unit_adapter_regs.h>
#include <al_hal_unit_adapter.h>
#include <al_hal_ssm_crypto.h>

MODULE_VERSION(AL_CRYPTO_VERSION);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Annapurna Labs");

#define DRV_NAME "al_crypto"
#define MAX_HW_DESCS_PER_SW_DECS 4

enum {
	/* BAR's are enumerated in terms of pci_resource_start() terms */
	AL_CRYPTO_UDMA_BAR		= 0,
	AL_CRYPTO_APP_BAR		= 4
};

static inline bool is_physfn(struct pci_dev *pdev)
{
#ifdef CONFIG_PCI_IOV
	return pdev->is_physfn;
#else
	return AL_TRUE;
#endif
}

static int al_crypto_pci_probe(
	struct pci_dev			*pdev,
	const struct pci_device_id	*id);

static void al_crypto_pci_remove(
	struct pci_dev	*pdev);

static void al_crypto_flr(struct pci_dev *pdev);

/* indexed by board_t */
static struct {
	/* needed for HOST modes */
        unsigned int bar;
	unsigned int udma_num;
} board_info[] = {
        {
	},
	/* HOST_PF */
        {
                .bar = 5,
		.udma_num = 0,
        },
	/* HOST_VF */
        {
                .bar = 5,
		.udma_num = 1,
        },
};

#define PCI_DEVICE_ID_AL_CRYPTO_HOST_PF_V2	0x1040
#define PCI_DEVICE_ID_AL_CRYPTO_HOST_VF_V2	0x9040

static DEFINE_PCI_DEVICE_TABLE(al_crypto_pci_tbl) = {
	{ .vendor = PCI_VENDOR_ID_ANNAPURNA_LABS,
	  .device = PCI_DEVICE_ID_AL_SSM,
	  .subvendor = PCI_ANY_ID,
	  .subdevice = PCI_ANY_ID,
	  .class = PCI_CLASS_CRYPT_NETWORK << 8,
	  .class_mask = PCI_ANY_ID,
	},
	{ .vendor = PCI_VENDOR_ID_ANNAPURNA_LABS,
	  .device = PCI_DEVICE_ID_AL_SSM_VF,
	  .subvendor = PCI_ANY_ID,
	  .subdevice = PCI_ANY_ID,
	  .class = PCI_CLASS_CRYPT_NETWORK << 8,
	  .class_mask = PCI_ANY_ID,
	},
	{ PCI_VDEVICE(ANNAPURNA_LABS, PCIE_DEVICE_ID_AL_CRYPTO) },
	{ PCI_VDEVICE(ANNAPURNA_LABS, PCIE_DEVICE_ID_AL_CRYPTO_VF) },
	{ PCI_VENDOR_ID_ANNAPURNA_LABS, PCI_DEVICE_ID_AL_CRYPTO_HOST_PF_V2,
	  PCI_ANY_ID, PCI_ANY_ID, 0, 0, ALPINE_HOST_PF},
	{ PCI_VENDOR_ID_ANNAPURNA_LABS, PCI_DEVICE_ID_AL_CRYPTO_HOST_VF_V2,
	  PCI_ANY_ID, PCI_ANY_ID, 0, 0, ALPINE_HOST_VF},

	{ 0, }
};

MODULE_DEVICE_TABLE(pci, al_crypto_pci_tbl);

static struct pci_driver al_crypto_pci_driver = {
	.name		= DRV_NAME,
	.id_table	= al_crypto_pci_tbl,
	.probe		= al_crypto_pci_probe,
	.remove		= al_crypto_pci_remove,
};

static void al_crypto_ext_info_set(struct al_crypto_device *device, void __iomem * const *iomap)
{
	struct al_crypto_ext_info *ext_info = &device->ext_info;

	ext_info->udma_num = board_info[device->device_type].udma_num;
	if (PCI_FUNC(device->pdev->devfn) == 1) {
		ext_info->device_num = 4;
		ext_info->tgtid = 0x8001;
	} else {
		ext_info->device_num = 5;
		ext_info->tgtid = 0x8002;
	}

	dev_dbg(&device->pdev->dev,
		"ssm host mode device num %d udma %d\n",
		ext_info->device_num, ext_info->udma_num);
	/*
	 * pci adapter configuration space: 0-4K
	 * BAR0-UDMA: 128K-256K
	 * BAR4-CRYPTO: 9K
	 */

	ext_info->internal_pcie_base = iomap[5] + ext_info->device_num * 0x100000;
	device->udma_regs_base = iomap[5] + ext_info->device_num * 0x100000 +
				(128 * 0x400)*(1 + ext_info->udma_num);
	device->crypto_regs_base = iomap[5] + ext_info->device_num * 0x100000 +
				0x9000;

	dev_dbg(&device->pdev->dev,
		"Mapped addresses - internal pcie base %p udma %p crypto %p\n",
		ext_info->internal_pcie_base, device->udma_regs_base,
		device->crypto_regs_base);
}

/******************************************************************************
 *****************************************************************************/
static int al_crypto_pci_probe(
	struct pci_dev			*pdev,
	const struct pci_device_id	*id)
{
	int status = 0;
	int sriov_crc_channels = al_crypto_get_crc_channels();
	void __iomem * const *iomap;
	struct device *dev = &pdev->dev;
	struct al_crypto_device *device;
	int bar_reg;
	u16 dev_id;
	u8 rev_id;

	dev_dbg(dev, "%s(%p, %p)\n", __func__, pdev, id);

	if (min(al_crypto_get_rx_descs_order(),al_crypto_get_tx_descs_order()) <
			(MAX_HW_DESCS_PER_SW_DECS + al_crypto_get_ring_alloc_order())) {
		dev_err(dev, "%s: Too small HW Q can lead to unexpected behavior "
				"upon queue overflow\n",__func__);
	}

	device = devm_kzalloc(dev, sizeof(struct al_crypto_device), GFP_KERNEL);
	if (!device) {
		status = -ENOMEM;
		goto done;
	}

	device->device_type = id->driver_data;
	status = pcim_enable_device(pdev);
	if (status) {
		pr_err("%s: pcim_enable_device failed!\n", __func__);
		goto done;
	}

	if (device->device_type == ALPINE_ONCHIP) {
		bar_reg = is_physfn(pdev) ?
			(1 << AL_CRYPTO_UDMA_BAR) | (1 << AL_CRYPTO_APP_BAR ) :
			(1 << AL_CRYPTO_UDMA_BAR);
	} else {
		bar_reg = 1 << 5;
	}

	status = pcim_iomap_regions(
		pdev,
		bar_reg,
		DRV_NAME);
	if (status) {
		pr_err("%s: pcim_iomap_regions failed!\n", __func__);
		goto done;
	}

	iomap = pcim_iomap_table(pdev);
	if (!iomap) {
		status = -ENOMEM;
		goto done;
	}

	status = pci_set_dma_mask(pdev, DMA_BIT_MASK(40));
	if (status)
		goto done;

	status = pci_set_consistent_dma_mask(pdev, DMA_BIT_MASK(40));
	if (status)
		goto done;

	device->pdev = pdev;
	pci_set_master(pdev);
	pci_set_drvdata(pdev, device);
	dev_set_drvdata(dev, device);

	if (device->device_type == ALPINE_ONCHIP) {
		pci_read_config_word(pdev, PCI_DEVICE_ID, &dev_id);
		pci_read_config_byte(pdev, PCI_REVISION_ID, &rev_id);
		device->udma_regs_base = iomap[AL_CRYPTO_UDMA_BAR],
		device->crypto_regs_base = is_physfn(pdev) ? iomap[AL_CRYPTO_APP_BAR] : NULL;
	} else {
		struct al_crypto_ext_info *ext_info = &device->ext_info;
		u16 adapter_pci_cmd;

		al_crypto_ext_info_set(device, iomap);

		dev_id = readw(ext_info->internal_pcie_base + PCI_DEVICE_ID);
		rev_id = readb(ext_info->internal_pcie_base + PCI_REVISION_ID);

		dev_dbg(dev, "ssm rev_id %d\n", rev_id);

		if (ext_info->udma_num > 0) {
			/* enable VF */
			writel(1, ext_info->internal_pcie_base + 0x300 + PCI_SRIOV_NUM_VF);
			writel(PCI_SRIOV_CTRL_VFE | PCI_SRIOV_CTRL_MSE, ext_info->internal_pcie_base + 0x300 + PCI_SRIOV_CTRL);

			/* enable master/slave in the adapter conf */
			adapter_pci_cmd = readw(ext_info->internal_pcie_base + 0x1000 * (ext_info->udma_num) + PCI_COMMAND);
			adapter_pci_cmd |= PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER;
			writew(adapter_pci_cmd, ext_info->internal_pcie_base + 0x1000 * (ext_info->udma_num) + PCI_COMMAND);
		} else {
			/* enable master/slave in the adapter conf */
			adapter_pci_cmd = readw(ext_info->internal_pcie_base + PCI_COMMAND);
			adapter_pci_cmd |= PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER;
			adapter_pci_cmd &= ~PCI_COMMAND_INTX_DISABLE;
			writew(adapter_pci_cmd, ext_info->internal_pcie_base + PCI_COMMAND);
		}
	}

	device->dev_id = dev_id;
	device->rev_id = rev_id;

	al_crypto_flr(pdev);

	/*
	 * When VF is used the PF is dedicated to crc and the VF is dedicated
	 * to crypto
	 */
	if (al_crypto_get_use_virtual_function()) {
		if (is_physfn(pdev) && (pci_sriov_get_totalvfs(pdev) > 0))
			sriov_crc_channels = 0;
		else if (pdev->is_virtfn)
			sriov_crc_channels = al_crypto_get_max_channels();
	}

	device->max_channels = al_crypto_get_max_channels();
	device->crc_channels = sriov_crc_channels;

	if (al_crypto_get_use_virtual_function() && is_physfn(pdev) &&
			(pci_sriov_get_totalvfs(pdev) > 0)) {
		status = pci_enable_sriov(pdev, 1);
		if (status) {
			dev_err(dev, "%s: pci_enable_sriov failed, status %d\n",
					__func__, status);
			goto done;
		}
	}

	status = al_crypto_core_init(
		device,
		device->udma_regs_base,
		device->crypto_regs_base);
	if (status) {
		dev_err(dev, "%s: al_crypto_core_init failed\n", __func__);
		goto done;
	}

	status = al_crypto_sysfs_init(device);
	if (status) {
		dev_err(dev, "%s: al_dma_sysfs_init failed\n", __func__);
		goto err_sysfs_init;
	}

	if (device->crc_channels < device->max_channels) {
		status = al_crypto_alg_init(device);
		if (status) {
			dev_err(dev, "%s: al_crypto_alg_init failed\n",
				__func__);
			goto err_alg_init;
		}

		status = al_crypto_hash_init(device);
		if (status) {
			dev_err(dev, "%s: al_crypto_hash_init failed\n",
				__func__);
			goto err_hash_init;
		}
	} else
		dev_info(dev, "%s: Skipping alg/hash initialization, " \
				"no allocated channels\n", __func__);

	if (device->crc_channels > 0) {
		status = al_crypto_crc_init(device);
		if (status) {
			dev_err(dev, "%s: al_crypto_hash_init failed\n",
				__func__);
			goto err_crc_init;
		}
	} else
		dev_info(dev, "%s: Skipping crc initialization, " \
				"no allocated channels\n", __func__);

	goto done;


err_crc_init:
	al_crypto_hash_terminate(device);
err_hash_init:
	al_crypto_alg_terminate(device);
err_alg_init:
	al_crypto_sysfs_terminate(device);
err_sysfs_init:
	al_crypto_core_terminate(device);
done:
	return status;
}

int al_crypto_read_pcie_config(void *handle, int where, uint32_t *val)
{
	/* handle is a pointer to the pci_dev */
	pci_read_config_dword((struct pci_dev *)handle, where, val);
	return 0;
}

int al_crypto_write_pcie_config(void *handle, int where, uint32_t val)
{
	/* handle is a pointer to the pci_dev */
	pci_write_config_dword((struct pci_dev *)handle, where, val);
	return 0;
}

int al_crypto_write_pcie_flr(void *handle)
{
	/* handle is a pointer to the pci_dev */
	__pci_reset_function_locked((struct pci_dev *)handle);
	udelay(1000);
	return 0;
}

int al_crypto_ext_read_pcie_config(void *handle, int where, uint32_t *val)
{
	/* handle is the base address of the adapter */
	*val = readl(handle + where);
	return 0;
}

int al_crypto_ext_write_pcie_config(void *handle, int where, uint32_t val)
{
	/* handle is the base address of the adapter */
	writel(val, handle + where);
	return 0;
}

int al_crypto_ext_write_pcie_flr(void *handle)
{
	/* handle is the base address of the adapter */
	al_crypto_ext_write_pcie_config(handle, AL_PCI_EXP_CAP_BASE + AL_PCI_EXP_DEVCTL,
					AL_PCI_EXP_DEVCTL_BCR_FLR);
	al_udelay(1000);
	return 0;
}

/******************************************************************************
 *****************************************************************************/
static void inline al_crypto_int_flr(struct pci_dev *pdev)
{
	struct al_unit_adapter unit_adapter;
	int status;

	status = al_unit_adapter_handle_init(&unit_adapter,
					AL_UNIT_ADAPTER_TYPE_SSM, 0,
					al_crypto_read_pcie_config,
					al_crypto_write_pcie_config,
					al_crypto_write_pcie_flr,
					pdev);
	if(status) {
		BUG();
		return;
	}

	al_pcie_perform_flr(&unit_adapter);
}

static void inline al_crypto_ext_flr(struct pci_dev *pdev)
{
	struct al_crypto_device *device = pci_get_drvdata(pdev);
	struct al_unit_adapter unit_adapter;
	int status;

	status = al_unit_adapter_handle_init(&unit_adapter,
					AL_UNIT_ADAPTER_TYPE_SSM, 0,
					al_crypto_ext_read_pcie_config,
					al_crypto_ext_write_pcie_config,
					al_crypto_ext_write_pcie_flr,
					device->ext_info.internal_pcie_base);
	if(status) {
		BUG();
		return;
	}

	al_pcie_perform_flr(&unit_adapter);
}

static void inline al_crypto_flr(struct pci_dev *pdev)
{
	struct al_crypto_device *device = pci_get_drvdata(pdev);

	if (device->device_type == ALPINE_ONCHIP)
		al_crypto_int_flr(pdev);
	else
		al_crypto_ext_flr(pdev);
}

/******************************************************************************
 *****************************************************************************/
static void al_crypto_pci_remove(struct pci_dev *pdev)
{
	struct al_crypto_device *device = pci_get_drvdata(pdev);

	if (!device)
		return;

	dev_dbg(&pdev->dev, "Removing dma\n");

	if (is_physfn(device->pdev))
		pci_disable_sriov(device->pdev);

	al_crypto_crc_terminate(device);

	al_crypto_hash_terminate(device);

	al_crypto_sysfs_terminate(device);

	al_crypto_alg_terminate(device);

	al_crypto_core_terminate(device);

	pci_disable_device(pdev);
}

/******************************************************************************
 *****************************************************************************/
static int __init al_crypto_init_module(void)
{
	int err;

	pr_info(
		"%s: Annapurna Labs Crypto Driver %s\n",
		DRV_NAME,
		AL_CRYPTO_VERSION);

	err = pci_register_driver(&al_crypto_pci_driver);

	return err;
}
module_init(al_crypto_init_module);

/******************************************************************************
 *****************************************************************************/
static void __exit al_crypto_exit_module(void)
{
	pci_unregister_driver(&al_crypto_pci_driver);
}
module_exit(al_crypto_exit_module);
