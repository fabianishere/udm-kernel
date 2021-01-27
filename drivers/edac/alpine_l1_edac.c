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

/*#include "edac_device.h"*/
#include "edac_module.h"
#include "al_err_events.h"
#include "al_err_events_sys_fabric.h"
#include "al_err_events_io_fabric.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Talel Shenhar");
MODULE_DESCRIPTION("EDAC Driver for Alpine L1 cache");

#define ALPINE_EDAC_ERR_MASKING_THREASHOLD 500
#define ALPINE_EDAC_MAX_ERR_MSG_LEN 256
#define ALPINE_EDAC_CE_CHECK_POLL_MSEC 3000

struct alpine_edac_l1_device {
	struct platform_device *pdev;
	struct edac_device_ctl_info *edac_dev;
	struct al_err_events_handle handle;
	struct al_err_events_l1_data *err_events_data;
	u64 **err_cached;
	spinlock_t lock;
	struct notifier_block die_notifier;
};

static void alpine_edac_l1_handle_ce(
		struct alpine_edac_l1_device *l1_edac,
		int err_index,
		struct al_err_events_field *err)
{
	int cpu = smp_processor_id();
	int i;
	u64 *err_cached;
	int count;

	err_cached = l1_edac->err_cached[cpu];
	count = err->counter - err_cached[err_index];

	for (i = 0; i < count; i++) {
		err->print(err, true);
		err_cached[err_index] = err->counter;
		edac_device_handle_ce(l1_edac->edac_dev, 0, 0, err->name);
	}
}

static void alpine_edac_l1_handle_ue(
		struct alpine_edac_l1_device *l1_edac,
		int err_index,
		struct al_err_events_field *err)
{
	int cpu = smp_processor_id();
	int i;
	u64 *err_cached;
	int count;

	err_cached = l1_edac->err_cached[cpu];
	count = err->counter - err_cached[err_index];

	for (i = 0; i < count; i++) {
		err_cached[err_index] = err->counter;
		err->print(err, true);
		edac_device_handle_ue(l1_edac->edac_dev, 0, 0, err->name);
	}
}

static int alpine_edac_l1_collect_ue(
			struct alpine_edac_l1_device *l1_edac)
{
	struct al_err_events_module *module;
	int cpu = smp_processor_id();
	int i;
	u64 *err_cached = l1_edac->err_cached[cpu];
	int ret;
	unsigned long flags;

	spin_lock_irqsave(&l1_edac->lock, flags);

	module = &l1_edac->err_events_data[cpu].module;

	ret = al_err_events_module_collect(module,
		AL_ERR_EVENTS_COLLECT_INTERRUPT);
	if (ret != 0) {
		dev_err(l1_edac->edac_dev->dev,
			"fail to collect ue events for %s\n", module->name);
		spin_unlock_irqrestore(&l1_edac->lock, flags);
		return ret;
	}

	for (i = 0; i < module->fields_size; i++) {
		if (module->fields[i].valid &&
			(module->fields[i].sev == AL_ERR_EVENTS_UNCORRECTABLE) &&
			(module->fields[i].counter != err_cached[i])) {

			alpine_edac_l1_handle_ue(l1_edac, i,
				   &module->fields[i]);
		}
	}

	spin_unlock_irqrestore(&l1_edac->lock, flags);

	return 0;
}

static void alpine_edac_l1_per_cpu_collect_ce(void *info)
{
	unsigned long flags;
	struct al_err_events_module *module;
	struct alpine_edac_l1_device *l1_edac = info;
	int ret;
	int i;
	int cpu = smp_processor_id();

	spin_lock_irqsave(&l1_edac->lock, flags);

	module = &l1_edac->err_events_data[cpu].module;

	ret = al_err_events_module_collect(module,
			AL_ERR_EVENTS_COLLECT_POLLING);
	if (ret != 0) {
		dev_err(l1_edac->edac_dev->dev,
			"fail to collect ce events for %s\n",
			module->name);
		spin_unlock_irqrestore(&l1_edac->lock, flags);
		return;
	}

	for (i = 0; i < module->fields_size; i++) {
		if ((module->fields[i].valid) &&
			(module->fields[i].sev == AL_ERR_EVENTS_CORRECTABLE) &&
			(module->fields[i].counter != l1_edac->err_cached[cpu][i])){

			alpine_edac_l1_handle_ce(l1_edac, i,
						 &module->fields[i]);

		}
	}

	spin_unlock_irqrestore(&l1_edac->lock, flags);
}

void alpine_edac_l1_collect_ce(
	struct edac_device_ctl_info *edac_dev)
{
	struct alpine_edac_l1_device *l1_edac = edac_dev->pvt_info;
	on_each_cpu(alpine_edac_l1_per_cpu_collect_ce, l1_edac, 0);
}

static int alpine_l1_die_notifier(
	struct notifier_block *nb,
	unsigned long event,
	void *_die_args)
{
	struct alpine_edac_l1_device *l1_edac;

	l1_edac = container_of(nb, struct alpine_edac_l1_device, die_notifier);

	alpine_edac_l1_collect_ue(l1_edac);

	return NOTIFY_DONE;
}

static int alpine_edac_l1_print(const char *fmt, ...)
{
	int ret;
	char str[ALPINE_EDAC_MAX_ERR_MSG_LEN] = {0};
	char *prefix = "l1: ";
	va_list args;

	va_start(args, fmt);

	strcpy(str, prefix);
	ret = vsnprintf(str + strlen(prefix),
		ALPINE_EDAC_MAX_ERR_MSG_LEN - strlen(prefix), fmt, args);

	va_end(args);

	pr_err("%s %s", prefix, str);

	return ret;
}

static void alpine_edac_per_cpu_l1_init(void *info)
{
	int ret;
	int size;
	int cpu = smp_processor_id();
	struct al_err_events_l1_init_params l1_init_params = {
		.collect_mode = AL_ERR_EVENTS_COLLECT_DEFAULT,
	};
	struct alpine_edac_l1_device *l1_edac = info;

	l1_init_params.cpu_id = cpu;

	ret = al_err_events_l1_init(&l1_edac->handle,
		&l1_edac->err_events_data[cpu],
		&l1_init_params);
	if (ret) {
		dev_err(&l1_edac->pdev->dev, "failed to init l1 errors\n");
	}

	size = sizeof(u64)*l1_edac->err_events_data[cpu].module.fields_size;
	l1_edac->err_cached[cpu] = devm_kmalloc(&l1_edac->pdev->dev,
		size, GFP_KERNEL);
	BUG_ON(!l1_edac->err_cached[cpu]); /* find genteler approch */
	memset(l1_edac->err_cached[cpu], 0, size);
}

static int alpine_l1_edac_init(struct platform_device *pdev)
{
	int size;
	struct alpine_edac_l1_device *l1_edac = platform_get_drvdata(pdev);

	/* allocate memory for per cpu error events data object */
	size = sizeof(struct al_err_events_l1_data)*nr_cpu_ids;
	l1_edac->err_events_data = devm_kmalloc(&pdev->dev, size, GFP_KERNEL);
	if (!l1_edac->err_events_data) {
		dev_err(&pdev->dev, "fail to allocate memory for l1 data\n");
		return -ENOMEM;
	}
	memset(l1_edac->err_events_data, 0, size);

	/* allocate memory for per cpu error cached/prev errors */
	size = sizeof(u64 *)*nr_cpu_ids;
	l1_edac->err_cached = devm_kmalloc(&pdev->dev, size, GFP_KERNEL);
	if (!l1_edac->err_cached) {
		dev_err(&pdev->dev,
			"fail to allocate memory for l1 prev ce array\n");
		return -ENOMEM;
	}
	memset(l1_edac->err_cached, 0, size);

	on_each_cpu(alpine_edac_per_cpu_l1_init, l1_edac, true);

	return 0;
}

static void alpine_edac_l1_nop(struct al_err_events_field *field)
{

}

int alpine_edac_l1_error_handler_init(
	struct al_err_events_handle *handle,
	struct platform_device *pdev)
{
	int ret;
	struct al_err_events_handle_init_params handle_init = {
		.print_cb = alpine_edac_l1_print,
		.error_cb = alpine_edac_l1_nop,
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

struct edac_device_ctl_info *alpine_edac_l1_register(struct platform_device *pdev)
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
	edac_dev->edac_check = alpine_edac_l1_collect_ce;
	ret = edac_device_add_device(edac_dev);
	if (ret) {
		dev_err(&pdev->dev, "fail to load alpine edac device\n");
		return NULL;
	}

	edac_device_reset_delay_period(edac_dev,
			ALPINE_EDAC_CE_CHECK_POLL_MSEC);

	return edac_dev;
}

static int alpine_l1_edac_probe(struct platform_device *pdev)
{
	int ret;
	int size;
	struct alpine_edac_l1_device *l1_edac;

	size = sizeof(struct alpine_edac_l1_device);
	l1_edac = devm_kmalloc(&pdev->dev, size, GFP_KERNEL);
	if (!l1_edac) {
		dev_err(&pdev->dev, "fail to allocate memory for alpine edac\n");
		return -ENOMEM;
	}
	memset(l1_edac, 0, size);

	l1_edac->pdev = pdev;
	platform_set_drvdata(pdev, l1_edac);
	l1_edac->die_notifier.notifier_call = alpine_l1_die_notifier;
	spin_lock_init(&l1_edac->lock);

	ret = alpine_edac_l1_error_handler_init(&l1_edac->handle, pdev);
	if (ret != 0) {
		dev_err(&pdev->dev, "failed to init error service handler\n");
		return ret;
	}

	ret = alpine_l1_edac_init(pdev);
	if (ret != 0) {
		dev_err(&pdev->dev, "failed to init l1 error service\n");
		return ret;
	}

	l1_edac->edac_dev = alpine_edac_l1_register(pdev);
	if (!l1_edac->edac_dev) {
		dev_err(&pdev->dev, "fail to register to edac framework\n");
	}

	ret = register_die_notifier(&l1_edac->die_notifier);
	if (ret) {
		dev_err(&pdev->dev, "fail to register l1 die notification\n");
		goto fail_register_die_notifier;
	}

	l1_edac->edac_dev->pvt_info = l1_edac;

	dev_info(&pdev->dev, "successfully loaded\n");

	return 0;

fail_register_die_notifier:
	edac_device_del_device(&pdev->dev);

	return ret;
}

static int alpine_l1_edac_remove(struct platform_device *pdev)
{
	struct alpine_edac_l1_device *l1_edac = platform_get_drvdata(pdev);
	unregister_die_notifier(&l1_edac->die_notifier);
	edac_device_del_device(&pdev->dev);

	return 0;
}

static const struct of_device_id alpine_l1_edac_of_match[] = {
	{ .compatible = "annapurna-labs,alpine-v3-l1-edac", },
	{},
};

MODULE_DEVICE_TABLE(of, alpine_l1_edac_of_match);

static struct platform_driver alpine_l1_edac_driver = {
	.probe = alpine_l1_edac_probe,
	.remove = alpine_l1_edac_remove,
	.driver = {
		.name = "alpine-l1-edac-driver",
		.of_match_table = alpine_l1_edac_of_match,
	},
};

module_platform_driver(alpine_l1_edac_driver);
