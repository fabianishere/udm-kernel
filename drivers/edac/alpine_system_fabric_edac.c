/*
 * Copyright 2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 */

#include <linux/ctype.h>
#include <linux/dma-mapping.h>
#include <linux/edac.h>
#include <linux/interrupt.h>
#include <linux/kdebug.h>
#include <linux/kernel.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/of_platform.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/workqueue.h>

#include <al_hal_pbs_regs.h>
#include <alpine_pbs.h>

#include "edac_module.h"
#include "al_err_events.h"
#include "al_err_events_sys_fabric.h"
#include "al_err_events_io_fabric.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Talel Shenhar");
MODULE_DESCRIPTION("EDAC Driver for Alpine System Fabric");

#define ALPINE_EDAC_CPU_INT_MASK 0x1

#define ALPINE_EDAC_CE_CHECK_POLL_MSEC 3000

#define BIT_START_DELAY_MSEC 20000
#define BIT_CHECK_DELAY_MSEC 5000
#define BIT_CE_TIMEOUT_CYCLES 7
#define BIT_UE_TIMEOUT_MSEC (BIT_CHECK_DELAY_MSEC + 4000)

#define POS_RO_WRITE_MAGIC 0x83
#define DDR_INJECT_VALUE 0x57
#define ALPINE_EDAC_ERR_MASKING_THREASHOLD 500
#define ALPINE_EDAC_MAX_ERR_MSG_LEN 256

#define ALPINE_EDAC_DEBUGFS_BUF_SIZE 32

#define AL_DDR_NUM 2

struct alpine_system_fabric_edac_device {
	struct platform_device *pdev;
	struct edac_device_ctl_info *edac_dev;
	struct al_err_events_handle handle;
	struct al_err_events_sys_fabric_data err_events_data;
	u64 *err_cached;
	struct notifier_block die_notifier;
	struct al_ddr_cfg ddr_cfg[AL_DDR_NUM];
	struct dentry *debugfs_dir;
	spinlock_t lock;
};

static void alpine_edac_system_fabric_handle_ce(
		struct alpine_system_fabric_edac_device *sf_edac,
		int err_index,
		struct al_err_events_field *err)
{
	int count = err->counter - sf_edac->err_cached[err_index];
	int i;

	for (i = 0; i < count; i++) {
		err->print(err, true);
		sf_edac->err_cached[err_index] = err->counter;
		edac_device_handle_ce(sf_edac->edac_dev, 0, 0, err->name);
	}
}

static void alpine_edac_system_fabric_collect_ce(
	struct edac_device_ctl_info *edac_dev)
{
	unsigned long flags;
	struct al_err_events_module *module;
	struct alpine_system_fabric_edac_device *sf_edac = edac_dev->pvt_info;
	int ret;
	int i;

	spin_lock_irqsave(&sf_edac->lock, flags);

	module = &sf_edac->err_events_data.module;

	ret = al_err_events_module_collect(module,
			AL_ERR_EVENTS_COLLECT_POLLING);
	if (ret != 0) {
		dev_err(edac_dev->dev,
			"fail to collect ce events for %s\n",
			module->name);
		return;
	}

	for (i = 0; i < module->fields_size; i++) {
		if ((module->fields[i].valid) &&
			(module->fields[i].sev == AL_ERR_EVENTS_CORRECTABLE) &&
			(module->fields[i].counter != sf_edac->err_cached[i])) {

			alpine_edac_system_fabric_handle_ce(sf_edac, i,
							    &module->fields[i]);

		}
	}

	spin_unlock_irqrestore(&sf_edac->lock, flags);
}

static void alpine_edac_system_fabric_handle_ue(
		struct alpine_system_fabric_edac_device *sf_edac,
		int err_index,
		struct al_err_events_field *err)
{
	int count = err->counter - sf_edac->err_cached[err_index];
	int i;

	for (i = 0; i < count; i++) {
		sf_edac->err_cached[err_index] = err->counter;
		err->print(err, true);
		edac_device_handle_ue(sf_edac->edac_dev, 0, 0, err->name);
	}
}

static int alpine_edac_system_fabric_collect_ue(
			struct alpine_system_fabric_edac_device	*sf_edac)
{
	unsigned long flags;
	struct al_err_events_module *module;
	int ret;
	int i;
	bool ue_found = false;

	spin_lock_irqsave(&sf_edac->lock, flags);

	module = &sf_edac->err_events_data.module;

	ret = al_err_events_module_collect(module,
		AL_ERR_EVENTS_COLLECT_INTERRUPT);
	if (ret != 0) {
		dev_err(sf_edac->edac_dev->dev,
			"fail to collect ue events for %s\n", module->name);
		spin_unlock_irqrestore(&sf_edac->lock, flags);
		return ret;
	}

	for (i = 0; i < module->fields_size; i++) {
		if (module->fields[i].valid &&
			(module->fields[i].sev == AL_ERR_EVENTS_UNCORRECTABLE) &&
			(module->fields[i].counter != sf_edac->err_cached[i])) {
			ue_found = true;
			alpine_edac_system_fabric_handle_ue(sf_edac, i,
							    &module->fields[i]);

		}
	}

	if (!ue_found)
		dev_warn(sf_edac->edac_dev->dev,
			"irq was fired however ue was not found\n");

	spin_unlock_irqrestore(&sf_edac->lock, flags);

	return 0;
}

static irqreturn_t alpine_edac_sf_irq_handler(
	int irq,
	void *private)
{
	int ret;
	struct platform_device *pdev = private;
	struct alpine_system_fabric_edac_device *sf_edac;

	sf_edac = platform_get_drvdata(pdev);

	ret = alpine_edac_system_fabric_collect_ue(sf_edac);
	if (ret != 0) {
		dev_err(&sf_edac->pdev->dev,
			"no ue found for system fabric\n");
		return IRQ_NONE;
	}

	return IRQ_HANDLED;
}

static int alpine_edac_system_fabric_print(const char *fmt, ...)
{
	int ret;
	char str[ALPINE_EDAC_MAX_ERR_MSG_LEN] = {0};
	char *prefix = "system fabric: ";
	va_list args;

	va_start(args, fmt);

	strcpy(str, prefix);
	ret = vsnprintf(str + strlen(prefix),
		ALPINE_EDAC_MAX_ERR_MSG_LEN - strlen(prefix), fmt, args);

	va_end(args);

	pr_err("%s %s", prefix, str);

	return ret;
}

static int alpine_edac_system_fabric_init(
	struct platform_device *pdev,
	struct alpine_system_fabric_edac_device *sf_edac)
{
	int ret;
	struct resource *resource;
	void __iomem *system_fabric_base;
	size_t size;
	int ioresource_cnt = 0;
	int ch;
	struct al_err_events_sys_fabric_init_params system_fabric_init_params = {
		.collect_mode = AL_ERR_EVENTS_COLLECT_DEFAULT,
		.local_int_mask = ALPINE_EDAC_CPU_INT_MASK,
	};

	/* Sytstem Fabric MMIO */
	resource = platform_get_resource(pdev, IORESOURCE_MEM,
					 ioresource_cnt++);
	if (!resource) {
		dev_err(&pdev->dev, "system sabric dt get fail\n");
		return -EINVAL;
	}

	system_fabric_base = devm_ioremap(&pdev->dev, resource->start,
			       resource_size(resource));
	if (!system_fabric_base) {
		dev_err(&pdev->dev, "system Fabric ioremap\n");
		return -ENOMEM;
	}
	system_fabric_init_params.nb_regs_base = system_fabric_base;

	for (ch = 0; ch < AL_DDR_NUM; ch++) {
		void __iomem *ddr_ctrl_regs_base;
		void __iomem *ddr_phy_regs_base;
		void __iomem *ddrc_regs_base;
		struct al_ddr_cfg *ddr_cfg = &sf_edac->ddr_cfg[ch];

		/* DDR Core Ctrl MMIO */
		resource = platform_get_resource(pdev, IORESOURCE_MEM,
						 ioresource_cnt++);
		if (!resource) {
			dev_err(&pdev->dev, "DDR Core DT get fail\n");
			return -EINVAL;
		}

		ddr_ctrl_regs_base = devm_ioremap(&pdev->dev, resource->start,
						  resource_size(resource));
		if (!ddr_ctrl_regs_base) {
			dev_err(&pdev->dev, "DDR Core ioremap fail\n");
			return -ENOMEM;
		}

		/* DDR PHY MMIO */
		resource = platform_get_resource(pdev, IORESOURCE_MEM,
						 ioresource_cnt++);
		if (!resource) {
			dev_err(&pdev->dev, "ddr phy dt get fail\n");
			return -EINVAL;
		}

		ddr_phy_regs_base = devm_ioremap(&pdev->dev, resource->start,
						 resource_size(resource));
		if (!ddr_phy_regs_base) {
			dev_err(&pdev->dev, "ddr phy ioremap fail\n");
			return -ENOMEM;
		}

		/* DDRC MMIO */
		resource = platform_get_resource(pdev, IORESOURCE_MEM,
						 ioresource_cnt++);
		if (!resource) {
			dev_err(&pdev->dev, "ddr phy dt get fail\n");
			return -EINVAL;
		}

		ddrc_regs_base = devm_ioremap(&pdev->dev, resource->start,
						 resource_size(resource));
		if (!ddrc_regs_base) {
			dev_err(&pdev->dev, "ddr phy ioremap fail\n");
			return -ENOMEM;
		}

		ret = al_ddr_cfg_init_v3(system_fabric_base ,
					 ddr_ctrl_regs_base,
					 ddr_phy_regs_base,
					 ddrc_regs_base,
					 ch,
					 ddr_cfg);
		if (ret) {
			al_err("fail to initialize alpine DDR configuration\n");
			return ret;
		}

		system_fabric_init_params.ddr_cfg[ch] = ddr_cfg;
	}

	ret = al_err_events_sys_fabric_init(&sf_edac->handle,
		&sf_edac->err_events_data,
		&system_fabric_init_params);
	if (ret) {
		dev_err(&pdev->dev,
			"failed to initiate al_err_events_sys_fabric_init\n");
		return ret;
	}

	size = sizeof(u64)*sf_edac->err_events_data.module.fields_size;
	sf_edac->err_cached = devm_kmalloc(&pdev->dev,
		size, GFP_KERNEL);
	if (!sf_edac->err_cached) {
		dev_err(&pdev->dev,
			"fail to allocate memory for system fabric ce prev\n");
		return -ENOMEM;
	}
	memset(sf_edac->err_cached, 0, size);

	return 0;

}

static int alpine_system_fabric_edac_irq_setup(
	struct platform_device *pdev,
	struct alpine_system_fabric_edac_device *sf_edac)
{
	int ret;
	int irq;

	irq = irq_of_parse_and_map(pdev->dev.of_node, 0);
	ret = devm_request_irq(&pdev->dev, irq,
					alpine_edac_sf_irq_handler,
					IRQF_SHARED, pdev->name, pdev);
	if (ret != 0) {
		dev_err(&pdev->dev,
			"failed to register to system fabric irq %d\n", irq);
		return ret;
	}

	ret = al_err_events_sys_fabric_ints_enable(&sf_edac->err_events_data);
	if (ret) {
		dev_err(&pdev->dev,
			"fail to enable system fabric interrupts\n");
		goto fail;
	}

	return 0;

fail:
	devm_free_irq(&pdev->dev, irq, pdev);

	return ret;
}


struct edac_device_ctl_info *alpine_edac_system_fabric_register(
				struct platform_device *pdev)
{
	struct edac_device_ctl_info *edac_dev;
	int device_index;
	int ret;

	device_index = edac_device_alloc_index();
	edac_dev = edac_device_alloc_ctl_info(0, pdev->name, 1, "", 1, 0,
					NULL, 0, device_index);
	if (!edac_dev) {
		dev_err(&pdev->dev, "fail to allocate edac device\n");
		return NULL;
	}
	edac_dev->dev = &pdev->dev;
	edac_dev->ctl_name = pdev->name;
	edac_dev->mod_name = pdev->name;
	edac_dev->dev_name = pdev->name;
	edac_dev->panic_on_ue = true;
	edac_dev->edac_check = alpine_edac_system_fabric_collect_ce;
	ret = edac_device_add_device(edac_dev);
	if (ret) {
		dev_err(&pdev->dev, "fail to load alpine edac device\n");
		return NULL;
	}

	edac_device_reset_delay_period(edac_dev,
			ALPINE_EDAC_CE_CHECK_POLL_MSEC);

	return edac_dev;
}

static void alpine_edac_system_fabric_nop(struct al_err_events_field *field)
{

}

int alpine_edac_system_fabric_error_handler_init(
	struct al_err_events_handle *handle,
	struct platform_device *pdev)
{
	int ret;
	struct al_err_events_handle_init_params handle_init = {
		.print_cb = alpine_edac_system_fabric_print,
		.error_cb = alpine_edac_system_fabric_nop,
		.error_threshold = ALPINE_EDAC_ERR_MASKING_THREASHOLD,
	};

	handle_init.pbs_regs_base = alpine_pbs_regs_base_get();
	ret = al_err_events_handle_init(handle, &handle_init);
	if (ret) {
		dev_err(&pdev->dev, "fail to init handle for error service\n");
	return ret;
	}

	return 0;
}

static int alpine_edac_configure_ddr_ecc_channel(
				struct al_ddr_cfg *ddr_cfg,
				bool correctable)
{
	struct device_node *node;
	struct resource resource;
	dma_addr_t ddr_phy_addr;
	int ret;
	unsigned int rank = 0;
	unsigned int  bank = 0;
	unsigned int  bank_group = 0;
	unsigned int  col = 0;
	unsigned int  row = 0;
	char node_name[ALPINE_EDAC_DEBUGFS_BUF_SIZE];


	snprintf(node_name,
		 sizeof(node_name),
		 "ecc_poison_reserved_ch%d",
		 ddr_cfg->ch);
	node = of_find_node_by_name(NULL, node_name);
	if (!node) {
		pr_err("system-fabric fail to get %s DT\n", node_name);
		return -EINVAL;
	}

	ret = of_address_to_resource(node, 0, &resource);
	ddr_phy_addr = (dma_addr_t)resource.start;

	ret = al_ddr_address_translate_sys2dram(ddr_cfg,
						(al_phys_addr_t)ddr_phy_addr,
						&rank,
						&bank,
						&bank_group,
						&col,
						&row);
	if (ret) {
		pr_err("system-fabric fail to translate sys2dram address\n");
		return ret;
	}

	ret = al_ddr_ecc_data_poison_enable(ddr_cfg,
					    rank,
					    bank,
					    bank_group,
					    col,
					    row,
					    correctable);
	if (ret) {
		pr_err("system-fabric fail to enable ddr poisoning\n");
		return ret;
	}

	return 0;
}

static int sf_edac_poison_debugfs_write(void *data, u64 val)
{
	struct al_ddr_cfg *ddr = data;
	int ret;
	bool correctable = (bool)val;

	ret = alpine_edac_configure_ddr_ecc_channel(ddr, correctable);
	if (ret) {
		pr_err("system-fabric fail to configure ecc error\n");
		return ret;
	}

	pr_err("system-fabric ddr channel %d correctable = %s\n",
	       ddr->ch,
	       correctable ? "correctable" : "uncorrectable");

	return 0;
}

DEFINE_SIMPLE_ATTRIBUTE(sf_edac_poison_debugfs_fops,
			NULL,
			sf_edac_poison_debugfs_write,
			 "%llu\n");

static int alpine_system_fabric_debugfs_init(struct platform_device *pdev)
{
	struct dentry *sf_op;
	struct alpine_system_fabric_edac_device *sf_edac;
	int i;
	int ret;

	sf_edac = platform_get_drvdata(pdev);

	sf_edac->debugfs_dir = debugfs_create_dir("system-fabric-edac", NULL);
	if (!sf_edac->debugfs_dir) {
		dev_err(&pdev->dev, "fail to create debugfs directory\n");
		debugfs_remove_recursive(sf_edac->debugfs_dir);
		return -ENOMEM;
	}

	for (i = 0; i < AL_DDR_NUM; i++) {
		char name[ALPINE_EDAC_DEBUGFS_BUF_SIZE];
		snprintf(name, sizeof(name), "ch%d_is_correctable_poison", i);
		sf_op = debugfs_create_file(name,
					    0444,
					    sf_edac->debugfs_dir,
					    &sf_edac->ddr_cfg[i],
					    &sf_edac_poison_debugfs_fops);
		if (!sf_op) {
			dev_err(&pdev->dev, "fail to create debugfs directory\n");
			ret = -ENOMEM;
			goto fail;
		}
	}

	return 0;
fail:
	debugfs_remove_recursive(sf_edac->debugfs_dir);

	return ret;
}

static int alpine_system_fabric_edac_probe(
	struct platform_device *pdev)
{
	int ret;
	int size;
	struct alpine_system_fabric_edac_device *sf_edac;

	size = sizeof(struct alpine_system_fabric_edac_device);
	sf_edac = devm_kmalloc(&pdev->dev, size, GFP_KERNEL);
	if (!sf_edac) {
		dev_err(&pdev->dev,
			"fail to allocate memory for alpine edac\n");
		return -ENOMEM;
	}
	platform_set_drvdata(pdev, sf_edac);
	memset(sf_edac, 0, size);
	sf_edac->pdev = pdev;
	spin_lock_init(&sf_edac->lock);

	ret = alpine_edac_system_fabric_error_handler_init(&sf_edac->handle,
							   pdev);
	if (ret != 0) {
		dev_err(&pdev->dev, "failed to init error service handler\n");
		return ret;
	}

	ret = alpine_edac_system_fabric_init(pdev, sf_edac);
	if (ret != 0) {
		dev_err(&pdev->dev,
			"failed to init system fabric error service\n");
		return ret;
	}

	ret = alpine_system_fabric_debugfs_init(pdev);
	if (ret) {
		dev_err(&pdev->dev, "fail to create debugfs entry\n");
		return ret;
	}

	ret = alpine_system_fabric_edac_irq_setup(pdev, sf_edac);
	if (ret) {
		dev_err(&pdev->dev, "fail to enable alpine edac\n");
		goto fail_irq_setup;
	}

	sf_edac->edac_dev = alpine_edac_system_fabric_register(pdev);
	if (!sf_edac->edac_dev) {
		dev_err(&pdev->dev, "fail to register to edac framework\n");
		goto fail_edac_register;
	}

	sf_edac->edac_dev->pvt_info = sf_edac;

	dev_info(&pdev->dev, "successfully loaded\n");

	return 0;

fail_edac_register:
	al_err_events_module_ints_mask(&sf_edac->err_events_data.module);
fail_irq_setup:
	debugfs_remove_recursive(sf_edac->debugfs_dir);
	return ret;
}

static int alpine_system_fabric_edac_remove(
	struct platform_device *pdev)
{
	struct alpine_system_fabric_edac_device *sf_edac;
	int irq = irq_of_parse_and_map(pdev->dev.of_node, 0);

	sf_edac = platform_get_drvdata(pdev);

	al_err_events_module_ints_mask(&sf_edac->err_events_data.module);
	debugfs_remove_recursive(sf_edac->debugfs_dir);
	devm_free_irq(&pdev->dev, irq, pdev);
	edac_device_del_device(&pdev->dev);

	return 0;
}

static const struct of_device_id alpine_system_fabric_edac_of_match[] = {
	{ .compatible = "annapurna-labs,alpine-v3-system-fabric-edac", },
	{},
};

MODULE_DEVICE_TABLE(of, alpine_system_fabric_edac_of_match);

static struct platform_driver alpine_system_fabric_edac_driver = {
	.probe = alpine_system_fabric_edac_probe,
	.remove = alpine_system_fabric_edac_remove,
	.driver = {
		.name = "alpine-system-fabric-edac-driver",
		.of_match_table = alpine_system_fabric_edac_of_match,
	},
};

module_platform_driver(alpine_system_fabric_edac_driver);
