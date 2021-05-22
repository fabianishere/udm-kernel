/*
 * Annapurna Labs reboot service
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

#include <linux/module.h>
#include <linux/of_address.h>
#include <linux/platform_device.h>
#include <asm/system_misc.h>
#include <asm/io.h>

#define WDTLOAD		0x000
#define LOAD_MIN	0x00000001
#define LOAD_MAX	0xFFFFFFFF
#define WDTVALUE	0x004
#define WDTCONTROL	0x008

/* control register masks */
#define	INT_ENABLE	(1 << 0)
#define	RESET_ENABLE	(1 << 1)
#define WDTLOCK		0xC00
#define	UNLOCK		0x1ACCE551
#define	LOCK		0x00000001

static void __iomem *wdt_base;

static void alpine_wdt_restart(enum reboot_mode mode, const char *cmd)
{
	if (!wdt_base) {
		pr_err("%s: Not supported!\n", __func__);
	} else {
		writel(UNLOCK, wdt_base + WDTLOCK);
		writel(LOAD_MIN, wdt_base + WDTLOAD);
		writel(INT_ENABLE | RESET_ENABLE, wdt_base + WDTCONTROL);
	}

	while (1)
		;
}

static int alpine_reboot_probe(struct platform_device *pdev)
{
	struct device_node *wdt_node =
			of_parse_phandle(pdev->dev.of_node, "wdt-parent", 0);

	if (!wdt_node) {
		WARN(1, "failed to get wdt-parent entry");
		return -ENODEV;
	}

	wdt_base = of_iomap(wdt_node, 0);
	if (!wdt_base) {
		WARN(1, "failed to map watchdog base address");
		return -ENODEV;
	}

	arm_pm_restart = alpine_wdt_restart;

	return 0;
}

static const struct of_device_id alpine_reboot_of_match[] = {
	{ .compatible = "annapurna-labs,alpine-reboot"},
	{}
};

static struct platform_driver alpine_reboot_driver = {
	.probe = alpine_reboot_probe,
	.driver = {
		.name = "alpine-reboot",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(alpine_reboot_of_match),
	},
};
module_platform_driver(alpine_reboot_driver);

MODULE_DESCRIPTION("Annapurna Labs Reboot driver");
MODULE_AUTHOR("Hani Ayoub <hani@annapurnaLabs.com>");
MODULE_LICENSE("GPL");
