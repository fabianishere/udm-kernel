// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright 2019 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 */

#include <linux/module.h>
#include <linux/of_address.h>
#include <linux/pci.h>
#include <linux/ctype.h>

#include <al_err_events_udma.h>
#include <al_err_events_ssm.h>
#include <al_hal_ssm.h>
#include <al_hal_udma.h>
#include <al_hal_udma_iofic.h>

#define DRV_NAME			"al-ssm-pcie"

#define AL_UDMA_BAR			0
#define AL_APP_BAR			4

/* SSM error message max length */
#define AL_SSM_ERR_MSG_LEN		256
/* SSM workqueue delay time */
#define AL_SSM_ERR_DELAY		400

#define AL_SSM_CRC_NUM_V2		2
#define AL_SSM_CRC_NUM_V3		4
#define AL_SSM_CRC_NUM(rev)		((rev) == AL_SSM_REV_ID_REV2 ? \
					 AL_SSM_CRC_NUM_V2 : AL_SSM_CRC_NUM_V3)

struct al_ssm_pcie {
	struct al_err_events_udma_data err_udma_data;
	struct attribute_group al_ssm_udma_grp;
	char **al_ssm_udma_field_name;
	struct al_err_events_ssm_crypto_data err_crypto_data;
	struct attribute_group al_ssm_crypto_grp;
	char **al_ssm_crypto_field_name;
	struct al_err_events_ssm_raid_data err_raid_data;
	struct attribute_group al_ssm_raid_grp;
	char **al_ssm_raid_field_name;
	struct al_err_events_ssm_crc_data err_crc_data[AL_SSM_CRC_NUM_V3];
	struct attribute_group al_ssm_crc_grp[AL_SSM_CRC_NUM_V3];
	char **al_ssm_crc_field_name;
	struct al_err_events_handle err_handle;
	struct al_udma udma;
	struct dentry *dir;
	struct pci_dev *pdev;
	void __iomem *pbs_base;
	struct msix_entry *ssm_msix_entry;
	int ssm_id;
	u16 dev_id;
	u8 rev_id;
	u8 panic_on_ue;
};

static int al_ssm_pcie_sysfs_name2index(char **arr_str, const char *name,
					int count)
{
	int i;

	for (i = 0; i < count; i++)
		if (!strcmp(arr_str[i], name))
			return i;

	return -1;
}

static ssize_t al_ssm_err_udma_read(struct device *dev,
				    struct device_attribute *attr, char *buf)
{
	struct al_ssm_pcie *ssm_pcie = dev_get_drvdata(dev);
	struct al_err_events_field *field;
	int index;

	index = al_ssm_pcie_sysfs_name2index(ssm_pcie->al_ssm_udma_field_name,
					     attr->attr.name,
					     AL_ERR_EVENTS_UDMA_MAX_ERRORS);
	if (index < 0)
		return -EINVAL;

	field = &ssm_pcie->err_udma_data.fields[index];

	return scnprintf(buf, PAGE_SIZE, "%d\n", field->counter);
}

static ssize_t al_ssm_err_udma_write(struct device *dev,
				     struct device_attribute *attr,
				     const char *buf, size_t len)
{
	struct al_ssm_pcie *ssm_pcie = dev_get_drvdata(dev);
	struct al_err_events_field *field;
	int index;

	index = al_ssm_pcie_sysfs_name2index(ssm_pcie->al_ssm_udma_field_name,
					     attr->attr.name,
					     AL_ERR_EVENTS_UDMA_MAX_ERRORS);
	if (index < 0)
		return -EINVAL;

	field = &ssm_pcie->err_udma_data.fields[index];

	field->counter = 0;

	return len;
}

static ssize_t al_ssm_err_crypto_read(struct device *dev,
				      struct device_attribute *attr, char *buf)
{
	struct al_ssm_pcie *ssm_pcie = dev_get_drvdata(dev);
	struct al_err_events_field *field;
	int index;

	index = al_ssm_pcie_sysfs_name2index(ssm_pcie->al_ssm_crypto_field_name,
					     attr->attr.name,
					     AL_ERR_EVENTS_SSM_CRYPTO_MAX_ERRORS);
	if (index < 0)
		return -EINVAL;

	field = &ssm_pcie->err_crypto_data.fields[index];

	return scnprintf(buf, PAGE_SIZE, "%d\n", field->counter);
}

static ssize_t al_ssm_err_crypto_write(struct device *dev,
				       struct device_attribute *attr,
				       const char *buf, size_t len)
{
	struct al_ssm_pcie *ssm_pcie = dev_get_drvdata(dev);
	struct al_err_events_field *field;
	int index;

	index = al_ssm_pcie_sysfs_name2index(ssm_pcie->al_ssm_crypto_field_name,
					     attr->attr.name,
					     AL_ERR_EVENTS_SSM_CRYPTO_MAX_ERRORS);
	if (index < 0)
		return -EINVAL;

	field = &ssm_pcie->err_crypto_data.fields[index];

	field->counter = 0;

	return len;
}

static ssize_t al_ssm_err_raid_read(struct device *dev,
				    struct device_attribute *attr, char *buf)
{
	struct al_ssm_pcie *ssm_pcie = dev_get_drvdata(dev);
	struct al_err_events_field *field;
	int index;

	index = al_ssm_pcie_sysfs_name2index(ssm_pcie->al_ssm_raid_field_name,
					     attr->attr.name,
					     AL_ERR_EVENTS_SSM_RAID_MAX_ERRORS);
	if (index < 0)
		return -EINVAL;

	field = &ssm_pcie->err_raid_data.fields[index];

	return scnprintf(buf, PAGE_SIZE, "%d\n", field->counter);
}

static ssize_t al_ssm_err_raid_write(struct device *dev,
				     struct device_attribute *attr,
				     const char *buf, size_t len)
{
	struct al_ssm_pcie *ssm_pcie = dev_get_drvdata(dev);
	struct al_err_events_field *field;
	int index;

	index = al_ssm_pcie_sysfs_name2index(ssm_pcie->al_ssm_raid_field_name,
					     attr->attr.name,
					     AL_ERR_EVENTS_SSM_RAID_MAX_ERRORS);
	if (index < 0)
		return -EINVAL;

	field = &ssm_pcie->err_raid_data.fields[index];

	field->counter = 0;

	return len;
}

static ssize_t al_ssm_err_crc_read(struct device *dev,
				   struct device_attribute *attr, char *buf)
{
	struct al_ssm_pcie *ssm_pcie = dev_get_drvdata(dev);
	struct al_err_events_field *field;
	struct attribute_group *crc_grp;
	int index, i;

	index = al_ssm_pcie_sysfs_name2index(ssm_pcie->al_ssm_crc_field_name,
					     attr->attr.name,
					     AL_ERR_EVENTS_SSM_CRC_MAX_ERRORS);
	if (index < 0)
		return -EINVAL;


	for (i = 0; i < AL_SSM_CRC_NUM(ssm_pcie->rev_id); i++) {
		struct device_attribute *temp;

		crc_grp = &ssm_pcie->al_ssm_crc_grp[i];
		temp = (struct device_attribute *)crc_grp->attrs[index];

		if (temp == attr)
			break;
	}

	field = &ssm_pcie->err_crc_data[i].fields[index];

	return scnprintf(buf, PAGE_SIZE, "%d\n", field->counter);
}

static ssize_t al_ssm_err_crc_write(struct device *dev,
				    struct device_attribute *attr,
				    const char *buf, size_t len)
{
	struct al_ssm_pcie *ssm_pcie = dev_get_drvdata(dev);
	struct al_err_events_field *field;
	struct attribute_group *crc_grp;
	int index, i;

	index = al_ssm_pcie_sysfs_name2index(ssm_pcie->al_ssm_crc_field_name,
					     attr->attr.name,
					     AL_ERR_EVENTS_SSM_CRC_MAX_ERRORS);
	if (index < 0)
		return -EINVAL;

	for (i = 0; i < AL_SSM_CRC_NUM(ssm_pcie->rev_id); i++) {
		struct device_attribute *temp;

		crc_grp = &ssm_pcie->al_ssm_crc_grp[i];
		temp = (struct device_attribute *)crc_grp->attrs[index];

		if (temp == attr)
			break;
	}

	field = &ssm_pcie->err_crc_data[i].fields[index];
	field->counter = 0;

	return len;
}

static ssize_t al_ssm_err_panic_on_ue_show(struct device *dev,
					   struct device_attribute *attr,
					   char *buf)
{
	struct al_ssm_pcie *ssm_pcie = dev_get_drvdata(dev);

	return sprintf(buf, "%u\n", ssm_pcie->panic_on_ue);
}

static ssize_t al_ssm_err_panic_on_ue_store(struct device *dev,
					    struct device_attribute *attr,
					    const char *buf, size_t len)
{
	struct al_ssm_pcie *ssm_pcie = dev_get_drvdata(dev);
	unsigned long res;

	if (kstrtoul(buf, 0, &res)) {
		dev_err(dev, "Failed to modify panic_on_ue\n");
		return -EINVAL;
	}

	ssm_pcie->panic_on_ue = (res != 0);

	return len;
}

static struct device_attribute al_ssm_err_panic_sysfs = {
	.attr = {
		.name = "panic_on_ue",
		.mode = (0644)
	},
	.show = al_ssm_err_panic_on_ue_show,
	.store = al_ssm_err_panic_on_ue_store,
};

static void al_ssm_pcie_field_name_update(char *str)
{
	if (!str)
		return;

	for (; *str; str++)
		if (*str == ' ' || *str == '/')
			*str = '_';
		else
			*str = tolower(*str);
}

static int al_ssm_pcie_sysfs_group_create(struct al_ssm_pcie *ssm_pcie,
					  struct attribute_group *attr_grp,
					  char *grp_name, int field_count,
					  char ***field_names,
					  struct al_err_events_field fields[],
					  ssize_t (*show)(struct device *dev,
							  struct device_attribute *attr,
							  char *buf),
					  ssize_t (*store)(struct device *dev,
							   struct device_attribute *attr,
							   const char *buf,
							   size_t count))
{
	struct device *dev = &ssm_pcie->pdev->dev;
	struct device_attribute *al_ssm_sysfs;
	struct attribute **al_ssm_attr;
	int i, ret;

	al_ssm_sysfs = devm_kcalloc(dev, field_count, sizeof(*al_ssm_sysfs),
				    GFP_KERNEL);
	if (!al_ssm_sysfs)
		return -ENOMEM;

	al_ssm_attr = devm_kcalloc(dev, field_count + 1, sizeof(*al_ssm_attr),
				   GFP_KERNEL);
	if (!al_ssm_attr)
		return -ENOMEM;

	*field_names = devm_kcalloc(dev, field_count, sizeof(**field_names),
				    GFP_KERNEL);
	if (!*field_names)
		return -ENOMEM;

	for (i = 0; i < field_count; i++) {
		struct al_err_events_field *field = &fields[i];
		struct device_attribute *da = &al_ssm_sysfs[i];
		char *str;

		str = devm_kstrdup(dev, field->name, GFP_KERNEL);
		if (!str)
			return -ENOMEM;

		al_ssm_pcie_field_name_update(str);

		*(*field_names + i) = str;
		sysfs_attr_init(&da->attr);
		da->attr.name = str;
		da->attr.mode = 0644;
		da->show = show;
		da->store = store;
		al_ssm_attr[i] = &da->attr;
	}

	attr_grp->name = grp_name;
	attr_grp->attrs = al_ssm_attr;
	attr_grp->is_visible = NULL;
	attr_grp->bin_attrs = NULL;

	ret = sysfs_create_group(&dev->kobj, attr_grp);
	if (ret) {
		pci_err(ssm_pcie->pdev,
			"failed to create sysfs device attributes\n");
		return -EINVAL;
	}

	return 0;
}

static int al_ssm_pcie_sysfs_create(struct al_ssm_pcie *ssm_pcie)
{
	struct device *dev = &ssm_pcie->pdev->dev;
	int ret, i;

	ret = device_create_file(dev, &al_ssm_err_panic_sysfs);
	if (ret) {
		pci_err(ssm_pcie->pdev,
			"Failed to create sysfs to control panic\n");
		return ret;
	}

	ret = al_ssm_pcie_sysfs_group_create(ssm_pcie,
					     &ssm_pcie->al_ssm_udma_grp,
					     "udma",
					     AL_ERR_EVENTS_UDMA_MAX_ERRORS,
					     &ssm_pcie->al_ssm_udma_field_name,
					     ssm_pcie->err_udma_data.fields,
					     al_ssm_err_udma_read,
					     al_ssm_err_udma_write);
	if (ret) {
		pci_err(ssm_pcie->pdev,
			"Failed to create sysfs group for SSM-UDMA\n");
		goto udma_fail;
	}

	ret = al_ssm_pcie_sysfs_group_create(ssm_pcie,
					     &ssm_pcie->al_ssm_crypto_grp,
					     "crypto",
					     AL_ERR_EVENTS_SSM_CRYPTO_MAX_ERRORS,
					     &ssm_pcie->al_ssm_crypto_field_name,
					     ssm_pcie->err_crypto_data.fields,
					     al_ssm_err_crypto_read,
					     al_ssm_err_crypto_write);
	if (ret) {
		pci_err(ssm_pcie->pdev,
		       "Failed to create sysfs group for SSM-Crypto\n");
		goto crypto_fail;
	}

	ret = al_ssm_pcie_sysfs_group_create(ssm_pcie,
					     &ssm_pcie->al_ssm_raid_grp,
					     "raid",
					     AL_ERR_EVENTS_SSM_RAID_MAX_ERRORS,
					     &ssm_pcie->al_ssm_raid_field_name,
					     ssm_pcie->err_raid_data.fields,
					     al_ssm_err_raid_read,
					     al_ssm_err_raid_write);
	if (ret) {
		pci_err(ssm_pcie->pdev,
			"Failed to create sysfs group for SSM-Raid\n");
		goto raid_fail;
	}

	for (i = 0; i < AL_SSM_CRC_NUM(ssm_pcie->rev_id); i++) {
		char *grp_name;

		grp_name = devm_kasprintf(dev, GFP_KERNEL, "crc%d", i);
		if (!grp_name) {
			ret = -ENOMEM;
			goto crc_fail;
		}

		ret = al_ssm_pcie_sysfs_group_create(ssm_pcie,
						     &ssm_pcie->al_ssm_crc_grp[i],
						     grp_name,
						     AL_ERR_EVENTS_SSM_CRC_MAX_ERRORS,
						     &ssm_pcie->al_ssm_crc_field_name,
						     ssm_pcie->err_crc_data[i].fields,
						     al_ssm_err_crc_read,
						     al_ssm_err_crc_write);
		if (ret) {
			pci_err(ssm_pcie->pdev,
				"Failed to create sysfs group for SSM-CRC%d\n",
				i);
			goto crc_fail;
		}
	}

	return ret;

crc_fail:
	while (i--)
		sysfs_remove_group(&ssm_pcie->pdev->dev.kobj,
				   &ssm_pcie->al_ssm_crc_grp[i]);
	sysfs_remove_group(&ssm_pcie->pdev->dev.kobj,
			   &ssm_pcie->al_ssm_raid_grp);
raid_fail:
	sysfs_remove_group(&ssm_pcie->pdev->dev.kobj,
			   &ssm_pcie->al_ssm_crypto_grp);
crypto_fail:
	sysfs_remove_group(&ssm_pcie->pdev->dev.kobj,
			   &ssm_pcie->al_ssm_udma_grp);
udma_fail:
	device_remove_file(&ssm_pcie->pdev->dev, &al_ssm_err_panic_sysfs);

	return ret;
}

static void al_ssm_pcie_sysfs_remove(struct pci_dev *pdev)
{
	struct al_ssm_pcie *ssm_pcie = pci_get_drvdata(pdev);
	int i;

	sysfs_remove_group(&pdev->dev.kobj, &ssm_pcie->al_ssm_udma_grp);
	sysfs_remove_group(&pdev->dev.kobj, &ssm_pcie->al_ssm_crypto_grp);
	sysfs_remove_group(&pdev->dev.kobj, &ssm_pcie->al_ssm_raid_grp);
	for (i = 0; i < AL_SSM_CRC_NUM(ssm_pcie->rev_id); i++)
		sysfs_remove_group(&pdev->dev.kobj,
				   &ssm_pcie->al_ssm_crc_grp[i]);
	device_remove_file(&pdev->dev, &al_ssm_err_panic_sysfs);
}

static int al_ssm_err_print_cb(const char *fmt, ...)
{
	return 0;
}

static void al_ssm_err_error_cb(struct al_err_events_field *field)
{
	struct al_ssm_pcie *ssm_pcie = container_of(field->parent_module->handle,
						    struct al_ssm_pcie,
						    err_handle);

	pci_err(ssm_pcie->pdev, "%s %s: %d\n", field->parent_module->name,
		field->name, field->counter);

	if (ssm_pcie->panic_on_ue && field->sev == AL_ERR_EVENTS_UNCORRECTABLE)
		panic("Uncorrectable error!\n");
}

static int al_ssm_err_handler_init(struct al_ssm_pcie *ssm_pcie)
{
	struct al_err_events_handle_init_params handle_init = {
		.pbs_regs_base = ssm_pcie->pbs_base,
		.print_cb = al_ssm_err_print_cb,
		.error_cb = al_ssm_err_error_cb,
		.error_threshold = 0,
	};
	int ret;

	ret = al_err_events_handle_init(&ssm_pcie->err_handle, &handle_init);

	return ret;
}

static irqreturn_t al_ssm_pcie_irq_handler(int irq, void *data)
{
	struct al_ssm_pcie *ssm_pcie = data;
	struct al_err_events_module *module;
	int ret, i;

	pci_dbg(ssm_pcie->pdev, "%s entered\n", __func__);

	module = &ssm_pcie->err_udma_data.module;
	ret = al_err_events_module_collect(module,
					   AL_ERR_EVENTS_COLLECT_INTERRUPT);
	if (ret) {
		pci_err(ssm_pcie->pdev,
			"Failed to collect error events for %s\n",
			module->name);
		goto done;
	}

	if (al_err_events_udma_ext_app_int_read_and_clear(&ssm_pcie->err_udma_data)) {
		pci_dbg(ssm_pcie->pdev, "Collect Crypto/Raid/CRC errors\n");

		module = &ssm_pcie->err_crypto_data.module;
		ret = al_err_events_module_collect(module,
						   AL_ERR_EVENTS_COLLECT_INTERRUPT);
		if (ret) {
			pci_err(ssm_pcie->pdev,
				"Failed to collect error events for %s\n",
				module->name);
			goto done;
		}

		module = &ssm_pcie->err_raid_data.module;
		ret = al_err_events_module_collect(module,
						   AL_ERR_EVENTS_COLLECT_INTERRUPT);
		if (ret) {
			pci_err(ssm_pcie->pdev,
				"Failed to collect error events for %s\n",
				module->name);
			goto done;
		}

		for (i = 0; i < AL_SSM_CRC_NUM(ssm_pcie->rev_id); i++) {
			module = &ssm_pcie->err_crc_data[i].module;
			if (!module->enabled)
				continue;

			ret = al_err_events_module_collect(module,
							   AL_ERR_EVENTS_COLLECT_INTERRUPT);
			if (ret) {
				pci_err(ssm_pcie->pdev,
					"Failed to collect error events for %s\n",
					module->name);
				goto done;
			}
		}
	}

	/*
	 * We only listen on AL_INT_GROUP_A_GROUP_D_SUM,
	 * and group A auto clears, so just need to unmask again
	 */
	al_udma_iofic_unmask_adv(&ssm_pcie->udma,
				 AL_UDMA_IOFIC_LEVEL_PRIMARY, AL_INT_GROUP_A,
				 AL_INT_GROUP_A_GROUP_D_SUM);
done:
	return IRQ_HANDLED;
}

static int al_ssm_err_events_init(struct al_ssm_pcie *ssm_pcie,
				  struct al_ssm_unit_regs_info *unit_info)
{
	struct al_err_events_ssm_app_init_params ssm_param;
	struct al_err_events_udma_init_params udma_init_param;
	int i, ret;

	/* Initialize handler */
	ret = al_ssm_err_handler_init(ssm_pcie);
	if (ret) {
		pci_err(ssm_pcie->pdev,
			"Failed to init handle for error service\n");
		return ret;
	}

	/* Init UDMA errors */
	udma_init_param.regs_base = unit_info->udma_regs_base;
	udma_init_param.udma_index = 0;
	udma_init_param.module_index = ssm_pcie->ssm_id;
	udma_init_param.primary_module = AL_ERR_EVENTS_MODULE_SSM;
	udma_init_param.collect_mode = AL_ERR_EVENTS_COLLECT_DEFAULT;

	ret = al_err_events_udma_init(&ssm_pcie->err_handle,
				      &ssm_pcie->err_udma_data,
				      &udma_init_param);
	if (ret) {
		pci_err(ssm_pcie->pdev,
			"Failed to initialize SSM(%d) UDMA(0) events\n",
			ssm_pcie->ssm_id);
		return ret;
	}

	ssm_param.ssm_idx = ssm_pcie->ssm_id;
	ssm_param.rev_id = ssm_pcie->rev_id;
	ssm_param.collect_mode = AL_ERR_EVENTS_COLLECT_DEFAULT;

	/* Init SSM crypto errors */
	ssm_param.app_regs_base = unit_info->crypto_regs_base;
	ssm_param.secondary_index = 0;
	ret = al_err_events_ssm_crypto_init(&ssm_pcie->err_handle,
					    &ssm_pcie->err_crypto_data,
					    &ssm_param);
	if (ret) {
		pci_err(ssm_pcie->pdev,
			"Failed to initialize SSM(%d) Crypto events\n",
			ssm_pcie->ssm_id);
		return ret;
	}

	/* Init SSM raid errors */
	ssm_param.app_regs_base = unit_info->raid_regs_base;
	ssm_param.secondary_index = 0;
	ret = al_err_events_ssm_raid_init(&ssm_pcie->err_handle,
					  &ssm_pcie->err_raid_data,
					  &ssm_param);
	if (ret) {
		pci_err(ssm_pcie->pdev,
			"Failed to initialize SSM(%d) Raid events\n",
			ssm_pcie->ssm_id);
		return ret;
	}

	/* Init SSM CRC errors */
	for (i = 0; i < AL_SSM_CRC_NUM(ssm_pcie->rev_id); i++) {
		ssm_param.app_regs_base = unit_info->crc_regs_base[i];
		ssm_param.secondary_index = i;
		ret = al_err_events_ssm_crc_init(&ssm_pcie->err_handle,
						 &ssm_pcie->err_crc_data[i],
						 &ssm_param);
		if (ret) {
			pci_err(ssm_pcie->pdev,
				"Failed to initialize SSM(%d) CRC(%d) events\n",
				ssm_pcie->ssm_id, i);
			return ret;
		}
	}

	return 0;
}

static int al_ssm_pcie_probe(struct pci_dev *pdev,
			     const struct pci_device_id *id)
{
	struct al_ssm_pcie *ssm_pcie;
	struct al_ssm_unit_regs_info unit_info;
	struct al_udma_params udma_param = {0};
	struct al_udma_iofic_cfg iofic_cfg = {0};
	struct msix_entry *msix_entries;
	void __iomem * const *iomap;
	void *bars[6];
	int msix_vecs;
	int ret;

	pci_info(pdev, "claimed by %s\n", DRV_NAME);

	/*
	 * Initialize and enable the errors of physical functions only.
	 * Errors or virtual functions are the responsibility of the owner
	 * of the VF.
	 */
	if (!pdev->is_physfn)
		return 0;

	ssm_pcie = devm_kzalloc(&pdev->dev, sizeof(*ssm_pcie), GFP_KERNEL);
	if (!ssm_pcie)
		return -ENOMEM;

	ret = pcim_enable_device(pdev);
	if (ret) {
		pci_err(pdev, "Failed to enable PCIm device!\n");
		return ret;
	}

	ret = pcim_iomap_regions(pdev, (1 << AL_UDMA_BAR) | (1 << AL_APP_BAR),
				 DRV_NAME);
	if (ret) {
		pci_err(pdev, "Failed to IOMAP PCIm regions!\n");
		goto pci_disable;
	}

	iomap = pcim_iomap_table(pdev);
	if (!iomap) {
		ret = -ENOMEM;
		goto pci_disable;
	}

	bars[0] = iomap[AL_UDMA_BAR];
	bars[4] = iomap[AL_APP_BAR];

	pci_set_master(pdev);
	pci_set_drvdata(pdev, ssm_pcie);
	ssm_pcie->pdev = pdev;
	ssm_pcie->ssm_id = PCI_SLOT(pdev->devfn);
	ssm_pcie->panic_on_ue = 0;

	ssm_pcie->pbs_base = of_iomap(of_find_compatible_node(NULL, NULL,
							      "annapurna-labs,al-pbs"),
				     0);
	if (!ssm_pcie->pbs_base) {
		pci_err(ssm_pcie->pdev, "Failed to map pbs_base\n");
		goto pci_disable;
	}

	pci_read_config_word(pdev, PCI_DEVICE_ID, &ssm_pcie->dev_id);
	pci_read_config_byte(pdev, PCI_REVISION_ID, &ssm_pcie->rev_id);

	msix_vecs = 1;
	msix_entries = devm_kcalloc(&pdev->dev, msix_vecs,
				    sizeof(*msix_entries), GFP_KERNEL);
	if (!msix_entries) {
		ret = -ENOMEM;
		goto pbs_iomap;
	}
	ssm_pcie->ssm_msix_entry = msix_entries;

	msix_entries[0].entry = AL_INT_MSIX_GROUP_A_SUM_D_IDX;
	msix_entries[0].vector = 0;

	ret = pci_enable_msix_range(pdev, msix_entries, msix_vecs, msix_vecs);
	if (ret != msix_vecs) {
		pci_err(pdev,
			"Failed to enable MSIx interrupt for SSM%d, ret = %d\n",
			ssm_pcie->ssm_id, ret);
		goto pbs_iomap;
	}

	al_ssm_unit_regs_info_get(bars, ssm_pcie->dev_id, ssm_pcie->rev_id,
				  &unit_info);

	/* Init UDMA handle */
	udma_param.udma_regs_base = bars[0];
	al_udma_handle_init(&ssm_pcie->udma, &udma_param);

	/* Config UDMA IOFIC */
	iofic_cfg.mode = AL_IOFIC_MODE_MSIX_PER_Q;
	ret = al_udma_iofic_config_ex(&ssm_pcie->udma, &iofic_cfg);
	if (ret) {
		pci_err(pdev, "Failed to config iofic ex\n");
		goto pbs_iomap;
	}

	ret = al_ssm_err_events_init(ssm_pcie, &unit_info);
	if (ret) {
		pci_err(pdev, "Failed to initialize SSM(%d) error events\n",
			ssm_pcie->ssm_id);
		goto pbs_iomap;
	}

	/* Initialize sysfs to read/set zero for error counters */
	ret = al_ssm_pcie_sysfs_create(ssm_pcie);
	if (ret) {
		pci_info(pdev, "Failed to create sysfs entries\n");
		goto pbs_iomap;
	}

	ret = devm_request_irq(&pdev->dev, msix_entries[0].vector,
			       al_ssm_pcie_irq_handler, 0, DRV_NAME, ssm_pcie);
	if (ret) {
		pci_err(pdev, "Failed to request irq");
		goto sysfs_remove;
	}

	al_ssm_error_ints_unmask(&unit_info, ssm_pcie->rev_id);

	pci_info(pdev, "Successfully loaded Error events for SSM(%d)\n",
		 ssm_pcie->ssm_id);

	return 0;

sysfs_remove:
	al_ssm_pcie_sysfs_remove(pdev);

pbs_iomap:
	iounmap(ssm_pcie->pbs_base);

pci_disable:
	pci_disable_device(pdev);

	return ret;
}

static void al_ssm_pcie_remove(struct pci_dev *pdev)
{
	struct al_ssm_pcie *ssm_pcie = pci_get_drvdata(pdev);

        /* mask all interrupts */
	al_udma_iofic_mask_adv(&ssm_pcie->udma, AL_UDMA_IOFIC_LEVEL_PRIMARY,
			       AL_INT_GROUP_A, AL_INT_GROUP_A_GROUP_D_SUM);
	al_udma_iofic_mask_adv(&ssm_pcie->udma, AL_UDMA_IOFIC_LEVEL_PRIMARY,
			       AL_INT_GROUP_D, 0xFFFFFFFF);

	synchronize_irq(ssm_pcie->ssm_msix_entry[0].vector);

	al_ssm_pcie_sysfs_remove(pdev);
	iounmap(ssm_pcie->pbs_base);
	pci_set_drvdata(pdev, NULL);
	pci_disable_device(pdev);
}

static const struct pci_device_id al_ssm_pcie_id_table[] = {
	{ PCI_VDEVICE(ANNAPURNA_LABS, PCI_DEVICE_ID_AL_SSM) },
	{ PCI_VDEVICE(ANNAPURNA_LABS, PCI_DEVICE_ID_AL_SSM_VF) },
	/* required last entry */
	{ 0 }
};
MODULE_DEVICE_TABLE(pci, al_ssm_pcie_id_table);

static struct pci_driver al_ssm_pcie_driver = {
	.name			= DRV_NAME,
	.id_table		= al_ssm_pcie_id_table,
	.probe			= al_ssm_pcie_probe,
	.remove			= al_ssm_pcie_remove,
	.sriov_configure	= pci_sriov_configure_simple,
};

static int __init al_ssm_pcie_init(void)
{
	return pci_register_driver(&al_ssm_pcie_driver);
}

static void __exit al_ssm_pcie_exit(void)
{
	pci_unregister_driver(&al_ssm_pcie_driver);
}

module_init(al_ssm_pcie_init);
module_exit(al_ssm_pcie_exit);

MODULE_LICENSE("GPL");
