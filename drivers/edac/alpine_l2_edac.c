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

#include "edac_device.h"
#include "edac_module.h"
#include "al_err_events.h"
#include "al_err_events_sys_fabric.h"
#include "al_err_events_io_fabric.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Talel Shenhar");
MODULE_DESCRIPTION("EDAC Driver for Alpine L2 Caches");

#define ALPINE_EDAC_CE_CHECK_POLL_MSEC 3000
#define ALPINE_EDAC_MAX_ERR_MSG_LEN 256
#define ALPINE_EDAC_ERR_MASKING_THREASHOLD 500

#define ALPINE_CLUSTERS_COUNT	4
#define ALPINE_CPU_PER_CLUSTER	4
#define ALPINE_EDAC_CPU_INT_MASK 0x1

struct alpine_l2_edac_device {
	struct platform_device *pdev;
	struct edac_device_ctl_info *edac_dev;
	struct al_err_events_handle handle;
	struct al_err_events_l2_data err_events_data;
	u64 *err_cached;
	struct notifier_block die_notifier;
	spinlock_t lock;
	void __iomem *system_fabric_regs_base;
	int cluster_cpu_owner;
	int cluster_id;
};

static void alpine_edac_l2_handle_ce(
		struct alpine_l2_edac_device *l2_edac,
		int err_index,
		struct al_err_events_field *err)
{
	int count = err->counter - l2_edac->err_cached[err_index];
	int i;

	for (i = 0; i < count; i++) {
		err->print(err, true);
		l2_edac->err_cached[err_index] = err->counter;
		edac_device_handle_ce(l2_edac->edac_dev, 0, 0, err->name);
	}
}

static void alpine_edac_l2_cluster_collect_ce(void *info)
{
	unsigned long flags;
	struct al_err_events_module *module;
	struct alpine_l2_edac_device *l2_edac = info;
	int ret;
	int i;

	spin_lock_irqsave(&l2_edac->lock, flags);

	module = &l2_edac->err_events_data.module;

	ret = al_err_events_module_collect(module,
			AL_ERR_EVENTS_COLLECT_POLLING);
	if (ret != 0) {
		dev_err(l2_edac->edac_dev->dev,
			"fail to collect ce events for %s\n",
			module->name);
		spin_unlock_irqrestore(&l2_edac->lock, flags);
		return;
	}

	for (i = 0; i < module->fields_size; i++) {
		if ((module->fields[i].valid) &&
			(module->fields[i].sev == AL_ERR_EVENTS_CORRECTABLE) &&
			(module->fields[i].counter != l2_edac->err_cached[i])) {

			alpine_edac_l2_handle_ce(l2_edac, i,
						 &module->fields[i]);

		}
	}

	spin_unlock_irqrestore(&l2_edac->lock, flags);
}

static void alpine_edac_l2_collect_ce( struct edac_device_ctl_info *edac_dev)
{
	int ret;
	struct alpine_l2_edac_device *l2_edac = edac_dev->pvt_info;

	ret = smp_call_function_single(l2_edac->cluster_cpu_owner,
				       alpine_edac_l2_cluster_collect_ce,
				       l2_edac,
				       true);
	if (ret) {
		dev_err(&l2_edac->pdev->dev,
			"fail to run collect function on cpu%d\n",
			l2_edac->cluster_cpu_owner);
	}
}

static void alpine_edac_l2_handle_ue(
		struct alpine_l2_edac_device *l2_edac,
		int err_index,
		struct al_err_events_field *err)
{
	int count = err->counter - l2_edac->err_cached[err_index];
	int i;

	for (i = 0; i < count; i++) {
		l2_edac->err_cached[err_index] = err->counter;
		err->print(err, true);
		edac_device_handle_ue(l2_edac->edac_dev, 0, 0, err->name);
	}
}

static int alpine_edac_l2_collect_ue(
			struct alpine_l2_edac_device *l2_edac)
{
	unsigned long flags;
	struct al_err_events_module *module;
	int ret;
	int i;


	module = &l2_edac->err_events_data.module;

	spin_lock_irqsave(&l2_edac->lock, flags);

	ret = al_err_events_module_collect(module,
		AL_ERR_EVENTS_COLLECT_INTERRUPT);
	if (ret != 0) {
		dev_err(l2_edac->edac_dev->dev,
			"fail to collect ue events for %s\n", module->name);

		spin_unlock_irqrestore(&l2_edac->lock, flags);
		return ret;
	}

	for (i = 0; i < module->fields_size; i++) {
		if (module->fields[i].valid &&
			(module->fields[i].sev == AL_ERR_EVENTS_UNCORRECTABLE) &&
			(module->fields[i].counter != l2_edac->err_cached[i])) {

			alpine_edac_l2_handle_ue(l2_edac, i,
							    &module->fields[i]);

		}
	}

	spin_unlock_irqrestore(&l2_edac->lock, flags);

	return 0;
}

static int alpine_l2_die_notifier(
	struct notifier_block *nb,
	unsigned long event,
	void *_die_args)
{
	struct alpine_l2_edac_device *l2_edac;

	l2_edac = container_of(nb, struct alpine_l2_edac_device, die_notifier);
	alpine_edac_l2_collect_ue(l2_edac);

	return NOTIFY_DONE;
}

static int alpine_l2_dt(struct platform_device *pdev)
{
	struct alpine_l2_edac_device *l2_edac = platform_get_drvdata(pdev);
	struct resource *resource;
	int ret;

	resource = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!resource) {
		dev_err(&pdev->dev, "fail to get platfom memory resource\n");
		return -EINVAL;
	}

	l2_edac->system_fabric_regs_base =
		devm_ioremap(&pdev->dev,
			     resource->start,
			     resource_size(resource));
	if (!l2_edac->system_fabric_regs_base) {
		dev_err(&pdev->dev, "failed to ioremap memory\n");
		return -ENOMEM;
	}

	ret = of_property_read_u32(pdev->dev.of_node,
				   "cluster-id",
				   &l2_edac->cluster_id);
	if (ret) {
		dev_err(&pdev->dev, "failed to get cluster id\n");
		return -EINVAL;
	}


	ret = of_property_read_u32(pdev->dev.of_node,
				   "cluster-cpu-owner",
				   &l2_edac->cluster_cpu_owner);
	if (ret) {
		dev_err(&pdev->dev, "failed to get cpu owner\n");
		return -EINVAL;
	}

	return 0;
}

static struct edac_device_ctl_info *alpine_edac_l2_register(
	struct platform_device *pdev)
{
	int ret;
	int device_index = 0;
	bool panic_on_ue = true;
	struct edac_device_ctl_info *edac_dev;

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
	edac_dev->panic_on_ue = panic_on_ue;
	edac_dev->edac_check = alpine_edac_l2_collect_ce;
	ret = edac_device_add_device(edac_dev);
	if (ret) {
		dev_err(&pdev->dev, "fail to load alpine edac device\n");
		return NULL;
	}

	edac_device_reset_delay_period(edac_dev,
			ALPINE_EDAC_CE_CHECK_POLL_MSEC);

	return edac_dev;
}

static void  alpine_edac_l2_nop(struct al_err_events_field *field)
{

}

static int alpine_edac_l2_print(const char *fmt, ...)
{
	int ret;
	char str[ALPINE_EDAC_MAX_ERR_MSG_LEN] = {0};
	char *prefix = "l2: ";
	va_list args;

	va_start(args, fmt);

	strcpy(str, prefix);
	ret = vsnprintf(str + strlen(prefix),
		ALPINE_EDAC_MAX_ERR_MSG_LEN - strlen(prefix), fmt, args);

	va_end(args);

	pr_err("%s %s", prefix, str);

	return ret;
}

int alpine_edac_l2_error_handler_init(
	struct al_err_events_handle *handle,
	struct platform_device *pdev)
{
	int ret;
	struct al_err_events_handle_init_params handle_init = {
		.print_cb = alpine_edac_l2_print,
		.error_cb = alpine_edac_l2_nop,
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

static int alpine_l2_edac_probe(struct platform_device *pdev)
{
	int ret;
	int size;
	struct alpine_l2_edac_device *l2_edac;
	struct al_err_events_l2_init_params l2_init_params = {
		.collect_mode = AL_ERR_EVENTS_COLLECT_DEFAULT,
	};

	l2_edac = devm_kmalloc(&pdev->dev,
			       sizeof(struct alpine_l2_edac_device),
			       GFP_KERNEL);
	if (!l2_edac) {
		dev_err(&pdev->dev,
			"fail to allocate memory for alpine edac\n");
		return -ENOMEM;
	}
	platform_set_drvdata(pdev, l2_edac);
	memset(l2_edac, 0, sizeof(struct alpine_l2_edac_device));

	ret = alpine_l2_dt(pdev);
	if (ret) {
		dev_err(&pdev->dev, "fail to parse dt\n");
	}

	l2_edac->die_notifier.notifier_call = alpine_l2_die_notifier;
	spin_lock_init(&l2_edac->lock);

	ret = alpine_edac_l2_error_handler_init(&l2_edac->handle, pdev);
	if (ret != 0) {
		dev_err(&pdev->dev, "failed to init error service handler\n");
		return ret;
	}

	l2_init_params.cluster_id = l2_edac->cluster_id;
	l2_init_params.nb_regs_base = l2_edac->system_fabric_regs_base;
	ret = al_err_events_l2_init(&l2_edac->handle,
				    &l2_edac->err_events_data,
				    &l2_init_params);
	if (ret) {
		dev_err(&pdev->dev, "failed to init l2 error service\n");
		return ret;
	}

	size = sizeof(u64)*l2_edac->err_events_data.module.fields_size;
	l2_edac->err_cached = devm_kmalloc(&pdev->dev, size, GFP_KERNEL);
	if (!l2_edac->err_cached) {
		dev_err(&pdev->dev, "fail to allocate memory for l2 ce prev\n");
		return -ENOMEM;
	}
	memset(l2_edac->err_cached, 0, size);

	l2_edac->edac_dev = alpine_edac_l2_register(pdev);
	if (!l2_edac->edac_dev) {
		dev_err(&pdev->dev, "fail to register to edac framework\n");
	}
	l2_edac->edac_dev->pvt_info = l2_edac;

	ret = register_die_notifier(&l2_edac->die_notifier);
	if (ret) {
		dev_err(&pdev->dev, "fail to egister to die notification\n");
		goto fail_register_die_notifier;
	}

	ret = al_err_events_l2_ints_enable(&l2_edac->err_events_data,
					   1 << l2_edac->cluster_cpu_owner);
	if (ret) {;
		dev_err(&pdev->dev, "fail to enable l2 interrupts\n");
		goto fail_irq_setup;
	}

	dev_info(&pdev->dev, "successfully loaded\n");
	return 0;

fail_irq_setup:
	unregister_die_notifier(&l2_edac->die_notifier);
fail_register_die_notifier:
	edac_device_del_device(&pdev->dev);

	return ret;
}

static int alpine_l2_edac_remove(struct platform_device *pdev)
{
	int ret;
	struct alpine_l2_edac_device *l2_edac = platform_get_drvdata(pdev);

	ret = al_err_events_l2_ints_enable(&l2_edac->err_events_data, 0);
	if (ret) {
		dev_err(&pdev->dev, "fail to disable interrupts (%d)\n", ret);
		return ret;
	}
	edac_device_del_device(&l2_edac->pdev->dev);
	unregister_die_notifier(&l2_edac->die_notifier);

	return 0;
}

static const struct of_device_id alpine_l2_edac_of_match[] = {
	{ .compatible = "annapurna-labs,alpine-v3-l2-edac", },
	{},
};

MODULE_DEVICE_TABLE(of, alpine_l2_edac_of_match);

static struct platform_driver alpine_l2_edac_driver = {
	.probe = alpine_l2_edac_probe,
	.remove = alpine_l2_edac_remove,
	.driver = {
		.name = "alpine-l2-edac-driver",
		.of_match_table = alpine_l2_edac_of_match,
	},
};

module_platform_driver(alpine_l2_edac_driver);
