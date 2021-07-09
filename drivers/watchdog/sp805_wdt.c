/*
 * drivers/char/watchdog/sp805-wdt.c
 *
 * Watchdog driver for ARM SP805 watchdog module
 *
 * Copyright (C) 2010 ST Microelectronics
 * Viresh Kumar <vireshk@kernel.org>
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2 or later. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#include <linux/acpi.h>
#include <linux/device.h>
#include <linux/resource.h>
#include <linux/amba/bus.h>
#include <linux/bitops.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/kernel.h>
#include <linux/math64.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/of.h>
#include <linux/pm.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/types.h>
#include <linux/watchdog.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/of_irq.h>

#include <linux/reboot.h>
#include "watchdog_core.h"

#if defined(CONFIG_UBNT_REBOOT_REASON)
#include <linux/resvmem_reboot.h>
#endif

/* default timeout in seconds */
#define DEFAULT_TIMEOUT		60

#define MODULE_NAME		"sp805-wdt"

/* watchdog register offsets and masks */
#define WDTLOAD			0x000
	#define LOAD_MIN	0x00000001
	#define LOAD_MAX	0xFFFFFFFF
#define WDTVALUE		0x004
#define WDTCONTROL		0x008
	/* control register masks */
	#define	INT_ENABLE	(1 << 0)
	#define	RESET_ENABLE	(1 << 1)
	#define	ENABLE_MASK	(INT_ENABLE | RESET_ENABLE)
#define WDTINTCLR		0x00C
#define WDTRIS			0x010
#define WDTMIS			0x014
	#define INT_MASK	(1 << 0)
#define WDTLOCK			0xC00
	#define	UNLOCK		0x1ACCE551
	#define	LOCK		0x00000001

/**
 * struct sp805_wdt: sp805 wdt device structure
 * @wdd: instance of struct watchdog_device
 * @lock: spin lock protecting dev structure and io access
 * @base: base address of wdt
 * @clk: clock structure of wdt
 * @adev: amba device structure of wdt
 * @status: current status of wdt
 * @load_val: load value to be set for current timeout
 */
struct sp805_wdt {
	struct watchdog_device		wdd;
	spinlock_t			lock;
	void __iomem			*base;
	struct clk			*clk;
	u64				rate;
	struct amba_device		*adev;
	unsigned int			load_val;
	int				irq;
	bool				ris_status;
};

static bool nowayout = WATCHDOG_NOWAYOUT;
module_param(nowayout, bool, 0);
MODULE_PARM_DESC(nowayout,
		"Set to 1 to keep watchdog running after device release");

/* returns true if wdt is running; otherwise returns false */
static bool wdt_is_running(struct watchdog_device *wdd)
{
	struct sp805_wdt *wdt = watchdog_get_drvdata(wdd);
	u32 wdtcontrol = readl_relaxed(wdt->base + WDTCONTROL);

	return (wdtcontrol & ENABLE_MASK) == ENABLE_MASK;
}

/* This routine finds load value that will reset system in required timout */
static int wdt_setload(struct watchdog_device *wdd, unsigned int timeout)
{
	struct sp805_wdt *wdt = watchdog_get_drvdata(wdd);
	u64 load, rate;

	rate = wdt->rate;

	/*
	 * sp805 runs counter with given value twice, after the end of first
	 * counter it gives an interrupt and then starts counter again. If
	 * interrupt already occurred then it resets the system. This is why
	 * load is half of what should be required.
	 */
	load = div_u64(rate, 2) * timeout - 1;

	load = (load > LOAD_MAX) ? LOAD_MAX : load;
	load = (load < LOAD_MIN) ? LOAD_MIN : load;

	spin_lock(&wdt->lock);
	wdt->load_val = load;
	/* roundup timeout to closest positive integer value */
	wdd->timeout = div_u64((load + 1) * 2 + (rate / 2), rate);
	spin_unlock(&wdt->lock);

	return 0;
}

/* returns number of seconds left for reset to occur */
static unsigned int wdt_timeleft(struct watchdog_device *wdd)
{
	struct sp805_wdt *wdt = watchdog_get_drvdata(wdd);
	u64 load;

	spin_lock(&wdt->lock);
	load = readl_relaxed(wdt->base + WDTVALUE);

	/*If the interrupt is inactive then time left is WDTValue + WDTLoad. */
	if (!(readl_relaxed(wdt->base + WDTRIS) & INT_MASK))
		load += wdt->load_val + 1;
	spin_unlock(&wdt->lock);

	return div_u64(load, wdt->rate);
}

static int
wdt_restart(struct watchdog_device *wdd, unsigned long mode, void *cmd)
{
	struct sp805_wdt *wdt = watchdog_get_drvdata(wdd);

	writel_relaxed(UNLOCK, wdt->base + WDTLOCK);
	writel_relaxed(0, wdt->base + WDTCONTROL);
	writel_relaxed(0, wdt->base + WDTLOAD);
	writel_relaxed(INT_ENABLE | RESET_ENABLE, wdt->base + WDTCONTROL);

	/* Flush posted writes. */
	readl_relaxed(wdt->base + WDTLOCK);

	return 0;
}

static int wdt_config(struct watchdog_device *wdd, bool ping)
{
	struct sp805_wdt *wdt = watchdog_get_drvdata(wdd);
	int ret;
	bool status;

	if (!ping) {

		ret = clk_prepare_enable(wdt->clk);
		if (ret) {
			dev_err(&wdt->adev->dev, "clock enable fail");
			return ret;
		}
	}

	spin_lock(&wdt->lock);

	writel_relaxed(UNLOCK, wdt->base + WDTLOCK);
	writel_relaxed(wdt->load_val, wdt->base + WDTLOAD);
	writel_relaxed(INT_MASK, wdt->base + WDTINTCLR);

	if (!ping)
		writel_relaxed(INT_ENABLE | RESET_ENABLE, wdt->base +
				WDTCONTROL);

	writel_relaxed(LOCK, wdt->base + WDTLOCK);

	/* Flush posted writes. */
	readl_relaxed(wdt->base + WDTLOCK);

	status = wdt->ris_status;
	wdt->ris_status = false;
	spin_unlock(&wdt->lock);

	if (status) {
		pr_crit("Watchdog reboot averted\n");
	}

	return 0;
}

static int wdt_ping(struct watchdog_device *wdd)
{
	return wdt_config(wdd, true);
}

/* enables watchdog timers reset */
static int wdt_enable(struct watchdog_device *wdd)
{
	return wdt_config(wdd, false);
}

/* disables watchdog timers reset */
static int wdt_disable(struct watchdog_device *wdd)
{
	struct sp805_wdt *wdt = watchdog_get_drvdata(wdd);

	spin_lock(&wdt->lock);

	writel_relaxed(UNLOCK, wdt->base + WDTLOCK);
	writel_relaxed(0, wdt->base + WDTCONTROL);
	writel_relaxed(LOCK, wdt->base + WDTLOCK);

	/* Flush posted writes. */
	readl_relaxed(wdt->base + WDTLOCK);
	spin_unlock(&wdt->lock);

	clk_disable_unprepare(wdt->clk);

	return 0;
}

static const struct watchdog_info wdt_info = {
	.options = WDIOF_MAGICCLOSE | WDIOF_SETTIMEOUT | WDIOF_KEEPALIVEPING,
	.identity = MODULE_NAME,
};

static const struct watchdog_ops wdt_ops = {
	.owner		= THIS_MODULE,
	.start		= wdt_enable,
	.stop		= wdt_disable,
	.ping		= wdt_ping,
	.set_timeout	= wdt_setload,
	.get_timeleft	= wdt_timeleft,
	.restart	= wdt_restart,
};

static irqreturn_t sp805_wdt_interrupt(int irq, void *dev_id)
{
	struct sp805_wdt *wdt = (struct sp805_wdt *)dev_id;
	bool status = readl_relaxed(wdt->base + WDTRIS) & INT_MASK;
	if(status && (wdt->ris_status != status)) {
		pr_crit("Watchdog is about to reboot system\n");
		wdt->ris_status = true;
#if defined(CONFIG_UBNT_REBOOT_REASON)
		resvmem_set_reboot_reason(RESVMEM_REBOOT_WATCHDOG);
#endif
	} else if(!status) {
		pr_crit("Watchdog reboot averted\n");
		wdt->ris_status = false;
	}

	return IRQ_HANDLED;
}

static int sp805_wdt_reboot_notifier(struct notifier_block *nb,
				    unsigned long code, void *data)
{
	struct watchdog_device *wdd;
	wdd = container_of(nb, struct watchdog_device, reboot_nb);
	if (code == SYS_DOWN || code == SYS_HALT || code == SYS_POWER_OFF) {
		if (watchdog_active(wdd)) {
			if (wdd->ops->stop(wdd))
				return NOTIFY_BAD;
		}
	}

	return NOTIFY_DONE;
}

static int
sp805_wdt_probe(struct amba_device *adev, const struct amba_id *id)
{
	struct sp805_wdt *wdt;
	int irq, ret = 0;

	wdt = devm_kzalloc(&adev->dev, sizeof(*wdt), GFP_KERNEL);
	if (!wdt) {
		ret = -ENOMEM;
		goto err;
	}

	wdt->base = devm_ioremap_resource(&adev->dev, &adev->res);
	if (IS_ERR(wdt->base))
		return PTR_ERR(wdt->base);

	if (adev->dev.of_node) {
		wdt->clk = devm_clk_get(&adev->dev, NULL);
		if (IS_ERR(wdt->clk)) {
			dev_err(&adev->dev, "Clock not found\n");
			return PTR_ERR(wdt->clk);
		}
		wdt->rate = clk_get_rate(wdt->clk);
	} else if (has_acpi_companion(&adev->dev)) {
		/*
		 * When Driver probe with ACPI device, clock devices
		 * are not available, so watchdog rate get from
		 * clock-frequency property given in _DSD object.
		 */
		device_property_read_u64(&adev->dev, "clock-frequency",
					 &wdt->rate);
		if (!wdt->rate) {
			dev_err(&adev->dev, "no clock-frequency property\n");
			return -ENODEV;
		}
	}

	wdt->adev = adev;
	wdt->wdd.info = &wdt_info;
	wdt->wdd.ops = &wdt_ops;
	wdt->wdd.parent = &adev->dev;

	spin_lock_init(&wdt->lock);
	watchdog_set_nowayout(&wdt->wdd, nowayout);
	watchdog_set_drvdata(&wdt->wdd, wdt);
	watchdog_set_restart_priority(&wdt->wdd, 128);

	wdt->wdd.reboot_nb.notifier_call = sp805_wdt_reboot_notifier;
	ret = register_reboot_notifier(&wdt->wdd.reboot_nb);
	if (ret) {
		pr_err("sp805 watchdog%d: Cannot register reboot notifier (%d)\n",
		       wdt->wdd.id, ret);
		watchdog_dev_unregister(&wdt->wdd);
		return ret;
	}

	/*
	 * If 'timeout-sec' devicetree property is specified, use that.
	 * Otherwise, use DEFAULT_TIMEOUT
	 */
	wdt->wdd.timeout = DEFAULT_TIMEOUT;
	watchdog_init_timeout(&wdt->wdd, 0, &adev->dev);
	wdt_setload(&wdt->wdd, wdt->wdd.timeout);

	/*
	 * If HW is already running, enable/reset the wdt and set the running
	 * bit to tell the wdt subsystem
	 */
	if (wdt_is_running(&wdt->wdd)) {
		wdt_enable(&wdt->wdd);
		set_bit(WDOG_HW_RUNNING, &wdt->wdd.status);
	}

	ret = watchdog_register_device(&wdt->wdd);
	if (ret) {
		dev_err(&adev->dev, "watchdog_register_device() failed: %d\n",
				ret);
		goto err;
	}
	amba_set_drvdata(adev, wdt);

	irq = irq_of_parse_and_map(adev->dev.of_node, 0);
	if (irq <= 0) {
		dev_err(&adev->dev, "sp805 failed to get IRQ\n");
		goto err;
	}
	wdt->irq = irq;

	wdt->ris_status = false;
	ret = request_irq(irq, sp805_wdt_interrupt, 0, "sp805_wis", wdt);
	if (ret) {
		dev_err(&adev->dev, "sp805 IRQ %d request fail\n", irq);
		goto err;
	}

	dev_info(&adev->dev, "registration successful\n");
	return 0;

err:
	dev_err(&adev->dev, "Probe Failed!!!\n");
	return ret;
}

static int sp805_wdt_remove(struct amba_device *adev)
{
	struct sp805_wdt *wdt = amba_get_drvdata(adev);

	watchdog_unregister_device(&wdt->wdd);
	watchdog_set_drvdata(&wdt->wdd, NULL);
	free_irq(wdt->irq, wdt);

	return 0;
}

static int __maybe_unused sp805_wdt_suspend(struct device *dev)
{
	struct sp805_wdt *wdt = dev_get_drvdata(dev);

	if (watchdog_active(&wdt->wdd))
		return wdt_disable(&wdt->wdd);

	return 0;
}

static int __maybe_unused sp805_wdt_resume(struct device *dev)
{
	struct sp805_wdt *wdt = dev_get_drvdata(dev);

	if (watchdog_active(&wdt->wdd))
		return wdt_enable(&wdt->wdd);

	return 0;
}

static SIMPLE_DEV_PM_OPS(sp805_wdt_dev_pm_ops, sp805_wdt_suspend,
		sp805_wdt_resume);

static const struct amba_id sp805_wdt_ids[] = {
	{
		.id	= 0x00141805,
		.mask	= 0x00ffffff,
	},
	{ 0, 0 },
};

MODULE_DEVICE_TABLE(amba, sp805_wdt_ids);

static struct amba_driver sp805_wdt_driver = {
	.drv = {
		.name	= MODULE_NAME,
		.pm	= &sp805_wdt_dev_pm_ops,
	},
	.id_table	= sp805_wdt_ids,
	.probe		= sp805_wdt_probe,
	.remove = sp805_wdt_remove,
};

module_amba_driver(sp805_wdt_driver);

MODULE_AUTHOR("Viresh Kumar <vireshk@kernel.org>");
MODULE_DESCRIPTION("ARM SP805 Watchdog Driver");
MODULE_LICENSE("GPL");
