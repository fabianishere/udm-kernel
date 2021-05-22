// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright 2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 */

#include <linux/debugfs.h>
#include <linux/module.h>
#include <linux/of_address.h>
#include <linux/platform_device.h>

#include <al_err_events_pcie.h>
#include <al_hal_pcie.h>

/* PCIe error message max length */
#define AMAZON_PCIE_ERR_MSG_LEN		256
/* PCIe workqueue delay time */
#define AMAZON_PCIE_ERR_DELAY		400

struct amazon_pcie_err {
	spinlock_t lock;
	struct device *dev;
	void __iomem *regs_base;
	u32 port_num;
	bool ue_found;
	struct al_pcie_port pcie_port;
	struct al_err_events_handle err_handle;
	struct al_err_events_pcie_app_data pcie_err_app_data;
	struct al_err_events_pcie_axi_data pcie_err_axi_data;
	bool panic_on_ue;
	struct dentry *dir;
	struct delayed_work workqueue;
};

static int amazon_pcie_err_print_cb(const char *fmt, ...)
{
	int ret;
	char str[AMAZON_PCIE_ERR_MSG_LEN] = {0};
	char *prefix = "PCIe: ";
	va_list args;

	va_start(args, fmt);

	ret = vsnprintf(str, AMAZON_PCIE_ERR_MSG_LEN, fmt, args);

	va_end(args);

	pr_err("%s %s", prefix, str);

	return ret;
}

static void amazon_pcie_err_error_cb(struct al_err_events_field *field)
{
	struct amazon_pcie_err *pcie_err =
			container_of(field->parent_module->handle,
				     struct amazon_pcie_err, err_handle);
	field->print(field, true);

	if (field->sev == AL_ERR_EVENTS_UNCORRECTABLE)
		pcie_err->ue_found = true;
}

static int amazon_pcie_err_handler_init(struct amazon_pcie_err *pcie_err,
					void __iomem *pbs_base)
{
	int ret;
	struct al_err_events_handle_init_params handle_init = {
		.print_cb = amazon_pcie_err_print_cb,
		.error_cb = amazon_pcie_err_error_cb,
		.error_threshold = 0,
	};

	handle_init.pbs_regs_base = pbs_base;
	ret = al_err_events_handle_init(&pcie_err->err_handle, &handle_init);

	return ret;
}

static void amazon_pcie_err_handler(struct work_struct *work)
{
	struct amazon_pcie_err *pcie_err;
	struct al_err_events_module *module;
	unsigned long flags;
	int ret;

	pcie_err = container_of(work, struct amazon_pcie_err, workqueue.work);

	spin_lock_irqsave(&pcie_err->lock, flags);

	/* Collect APP errors */
	module = &pcie_err->pcie_err_app_data.module;
	ret = al_err_events_module_collect(module,
					   AL_ERR_EVENTS_COLLECT_POLLING);
	if (ret) {
		dev_err(pcie_err->dev, "Failed to collect APP events for %s\n",
			module->name);
		spin_unlock(&pcie_err->lock);
		return;
	}

	/* Collect AXI errors */
	module = &pcie_err->pcie_err_axi_data.module;
	ret = al_err_events_module_collect(module,
					   AL_ERR_EVENTS_COLLECT_POLLING);
	if (ret) {
		dev_err(pcie_err->dev, "Failed to collect AXI events for %s\n",
			module->name);
		spin_unlock(&pcie_err->lock);
		return;
	}

	spin_unlock(&pcie_err->lock);

	if (pcie_err->ue_found && pcie_err->panic_on_ue)
		panic("Uncorrectable Error!\n");

	pcie_err->ue_found = false;

	schedule_delayed_work(&pcie_err->workqueue, AMAZON_PCIE_ERR_DELAY);
}

static ssize_t amazon_pcie_err_panic_on_ue_show(struct device *dev,
						struct device_attribute *attr,
						char *buf)
{
	struct amazon_pcie_err *pcie_err = dev_get_drvdata(dev);

	return sprintf(buf, "%u\n", pcie_err->panic_on_ue);
}

static ssize_t amazon_pcie_err_panic_on_ue_store(struct device *dev,
						 struct device_attribute *attr,
						 const char *buf, size_t len)
{
	struct amazon_pcie_err *pcie_err = dev_get_drvdata(dev);
	unsigned long res;

	if (kstrtoul(buf, 0, &res))
		dev_err(pcie_err->dev, "Failed to modify panic_on_ue\n");

	pcie_err->panic_on_ue = (res != 0);

	return len;
}

static struct device_attribute amazon_pcie_err_dev_attr = {
	.attr = {
		.name = "panic_on_ue",
		.mode = (0644)
	},
	.show = amazon_pcie_err_panic_on_ue_show,
	.store = amazon_pcie_err_panic_on_ue_store,
};

static ssize_t amazon_pcie_err_app_read(struct file *filep, char __user *ubuf,
					size_t size, loff_t *offp)
{
	struct amazon_pcie_err *pcie_err = filep->private_data;
	struct al_err_events_module *module;

	module = &pcie_err->pcie_err_app_data.module;
	pr_info("APP PCIe error events for PCIe port %d\n", pcie_err->port_num);
	al_err_events_module_print(module);

	return 0;
}

static ssize_t amazon_pcie_err_app_write(struct file *filep,
					 const char __user *ubuf, size_t size,
					 loff_t *offp)
{
	struct amazon_pcie_err *pcie_err = filep->private_data;
	struct al_err_events_module *module;
	unsigned long flags;

	spin_lock_irqsave(&pcie_err->lock, flags);
	module = &pcie_err->pcie_err_app_data.module;
	al_err_events_module_clear(module);
	spin_unlock(&pcie_err->lock);

	return size;
}

static const struct file_operations amazon_pcie_err_app_debugfs = {
	.owner = THIS_MODULE,
	.open = simple_open,
	.read = amazon_pcie_err_app_read,
	.write = amazon_pcie_err_app_write,
};

static ssize_t amazon_pcie_err_axi_read(struct file *filep, char __user *ubuf,
					size_t size, loff_t *offp)
{
	struct amazon_pcie_err *pcie_err = filep->private_data;
	struct al_err_events_module *module;

	module = &pcie_err->pcie_err_axi_data.module;
	pr_info("AXI PCIe error events for PCIe port %d\n", pcie_err->port_num);
	al_err_events_module_print(module);

	return 0;
}

static ssize_t amazon_pcie_err_axi_write(struct file *filep,
					 const char __user *ubuf, size_t size,
					 loff_t *offp)
{
	struct amazon_pcie_err *pcie_err = filep->private_data;
	struct al_err_events_module *module;
	unsigned long flags;

	spin_lock_irqsave(&pcie_err->lock, flags);
	module = &pcie_err->pcie_err_axi_data.module;
	al_err_events_module_clear(module);
	spin_unlock(&pcie_err->lock);

	return size;
}

static const struct file_operations amazon_pcie_err_axi_debugfs = {
	.owner = THIS_MODULE,
	.open = simple_open,
	.read = amazon_pcie_err_axi_read,
	.write = amazon_pcie_err_axi_write,
};

static int amazon_pcie_err_debugfs_create(struct amazon_pcie_err *pcie_err)
{
	pcie_err->dir = debugfs_create_dir("pcie-err-events", NULL);
	if (!pcie_err->dir) {
		dev_err(pcie_err->dev, "Failed to create debugfs dir\n");
		return -EINVAL;
	}

	if (!debugfs_create_file("pcie_err_app", 0600, pcie_err->dir, pcie_err,
				 &amazon_pcie_err_app_debugfs)) {
		dev_err(pcie_err->dev, "Failed to create PCIe app debugfs\n");
		return -EINVAL;
	}

	if (!debugfs_create_file("pcie_err_axi", 0600, pcie_err->dir, pcie_err,
				 &amazon_pcie_err_axi_debugfs)) {
		dev_err(pcie_err->dev, "Failed to create PCIe axi debugfs\n");
		return -EINVAL;
	}

	return 0;
}

static ssize_t currrent_link_width_show(struct device *dev,
					struct device_attribute *attr,
					char *buf)
{
	struct al_pcie_link_status link_status;
	struct amazon_pcie_err *pcie_err = dev_get_drvdata(dev);

	al_pcie_link_status(&pcie_err->pcie_port, &link_status);

	return sprintf(buf, "%u\n", link_status.lanes);
}

static DEVICE_ATTR_RO(currrent_link_width);

static ssize_t currrent_link_speed_show(struct device *dev,
					struct device_attribute *attr,
					char *buf)
{
	const char *speed;
	struct al_pcie_link_status link_status;
	struct amazon_pcie_err *pcie_err = dev_get_drvdata(dev);

	al_pcie_link_status(&pcie_err->pcie_port, &link_status);

	switch (link_status.speed) {
	case AL_PCIE_LINK_SPEED_GEN3:
		speed = "8 GT/s";
		break;
	case AL_PCIE_LINK_SPEED_GEN2:
		speed = "5 GT/s";
		break;
	case AL_PCIE_LINK_SPEED_GEN1:
		speed = "2.5 GT/s";
		break;
	default:
		speed = "Unknown speed";
	}

	return sprintf(buf, "%s\n", speed);
}

static DEVICE_ATTR_RO(currrent_link_speed);

static int amazon_pcie_err_probe(struct platform_device *pdev)
{
	struct amazon_pcie_err *pcie_err;
	struct resource res;
	struct device *dev = &pdev->dev;
	struct device_node *pbs_node;
	struct al_err_events_pcie_init_params pcie_init;
	void __iomem *pbs_base;
	int ret;

	pcie_err = devm_kzalloc(dev, sizeof(*pcie_err), GFP_KERNEL);
	if (!pcie_err)
		return -ENOMEM;

	platform_set_drvdata(pdev, pcie_err);
	spin_lock_init(&pcie_err->lock);
	pcie_err->dev = dev;

	if (of_property_read_u32(dev->of_node, "pcie-port-num",
				 &pcie_err->port_num)) {
		dev_err(dev, "Missing PCIe port number in dt\n");
		return -EINVAL;
	}

	pcie_err->ue_found = false;

	/* Get registers resources */
	ret = of_address_to_resource(dev->of_node, 0, &res);
	if (ret < 0) {
		dev_err(dev, "of_address_to_resource(): %d\n", ret);
		return ret;
	}

	pcie_err->regs_base = devm_ioremap(dev, res.start, resource_size(&res));
	if (!pcie_err->regs_base) {
		dev_err(dev, "ioremap failed for PCIe error\n");
		return -ENOMEM;
	}

	pbs_node = of_find_compatible_node(NULL, NULL, "annapurna-labs,al-pbs");

	pbs_base = of_iomap(pbs_node, 0);
	if (!pbs_base) {
		dev_err(dev, "pbs_base map failed\n");
		return -ENOMEM;
	}

	/* Initialize port handle */
	ret = al_pcie_port_handle_init(&pcie_err->pcie_port,
				       pcie_err->regs_base, pbs_base,
				       pcie_err->port_num);
	if (ret) {
		dev_err(dev, "Failed to initiate pcie_port handle\n");
		return -EINVAL;
	}

	/* Init the sysfs for PCIe status speed/width */
	if (device_create_file(dev, &dev_attr_currrent_link_speed)) {
		dev_notice(dev,
			   "Failed to create sysfs for PCIe speed status\n");
	}

	if (device_create_file(dev, &dev_attr_currrent_link_width)) {
		dev_notice(dev,
			   "Failed to create sysfs for PCIe width status\n");
	}

	/*
	 * The Bootloader disabled the unused PCIe port, in the device-tree,
	 * but PCIe error nodes are not modified.
	 * Check if the port enabled, to prevent access to disabled PCIe port.
	 */
	ret = al_pcie_port_memory_is_shutdown(&pcie_err->pcie_port);
	if (ret) {
		dev_notice(dev, "Skipping PCIe port (%d)\n",
			   pcie_err->port_num);
		return 0;
	}

	/* Initialize handler */
	ret = amazon_pcie_err_handler_init(pcie_err, pbs_base);
	if (ret) {
		dev_err(dev, "Failed to init handle for error service\n");
		return ret;
	}

	/* Initialize init params */
	pcie_init.collect_mode = AL_ERR_EVENTS_COLLECT_POLLING;
	pcie_init.pcie_port = &pcie_err->pcie_port;

	/* Init PCIe APP errors */
	ret = al_err_events_pcie_app_init(&pcie_err->err_handle,
					  &pcie_err->pcie_err_app_data,
					  &pcie_init);
	if (ret) {
		dev_err(dev, "Failed to initiate PCIe APP events\n");
		return -EINVAL;
	}

	/* Init PCIe AXI errors */
	ret = al_err_events_pcie_axi_init(&pcie_err->err_handle,
					  &pcie_err->pcie_err_axi_data,
					  &pcie_init);
	if (ret) {
		dev_err(dev, "Failed to initiate PCIe AXI events\n");
		return -EINVAL;
	}

	pcie_err->panic_on_ue = of_property_read_bool(dev->of_node,
						      "panic-on-ue");
	/*
	 * In addition to device-tree configuration initialize sysfs
	 * to enable/disable panic on uncorrectable errors.
	 */
	if (device_create_file(dev, &amazon_pcie_err_dev_attr))
		dev_info(dev, "Failed to create %s\n",
			 amazon_pcie_err_dev_attr.attr.name);

	/* Initialize debugfs to read/set zero for error counters */
	if (amazon_pcie_err_debugfs_create(pcie_err)) {
		debugfs_remove_recursive(pcie_err->dir);
		dev_info(dev, "Failed to create debugfs\n");
	}

	/* Initialize work queue (Polling mode) */
	INIT_DELAYED_WORK(&pcie_err->workqueue, amazon_pcie_err_handler);
	schedule_delayed_work(&pcie_err->workqueue, AMAZON_PCIE_ERR_DELAY);

	dev_info(&pdev->dev, "Successfully loaded for PCIe port %d\n",
		 pcie_err->port_num);

	return 0;
}

static int amazon_pcie_err_remove(struct platform_device *pdev)
{
	struct amazon_pcie_err *pcie_err = platform_get_drvdata(pdev);

	cancel_delayed_work_sync(&pcie_err->workqueue);
	debugfs_remove_recursive(pcie_err->dir);
	device_remove_file(pcie_err->dev, &amazon_pcie_err_dev_attr);
	device_remove_file(pcie_err->dev, &dev_attr_currrent_link_speed);
	device_remove_file(pcie_err->dev, &dev_attr_currrent_link_width);

	return 0;
}

static const struct of_device_id amazon_pcie_err_of_match[] = {
	{ .compatible = "amazon,al-pcie-external-err", },
	{},
};

MODULE_DEVICE_TABLE(of, amazon_pcie_err_of_match);

static struct platform_driver amazon_pcie_err_driver = {
	.driver = {
		.name = "amazon,al-pcie-external-err",
		.of_match_table = amazon_pcie_err_of_match,
	},
	.probe = amazon_pcie_err_probe,
	.remove = amazon_pcie_err_remove,
};

module_platform_driver(amazon_pcie_err_driver);
