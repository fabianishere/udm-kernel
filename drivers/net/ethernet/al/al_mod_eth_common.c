/*
 * linux/drivers/soc/alpine/alpine_eth_common.c
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
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/of_platform.h>

#include <linux/netdevice.h>

#include "al_mod_eth_common.h"

#define AL_ETH_COMMON_REGS_OFFSET					0x0
#define AL_ETH_COMMON_SCACHE_OFFSET					0x1000
#define AL_ETH_COMMON_SHARED_MAC_OFFSET				0x1800
#define AL_ETH_COMMON_SHARED_RESOURCES_OFFSET		0x2000

static void __iomem *eth_common_base;

int al_mod_eth_common_resource_init(void)
{
	struct device_node *np;
	char *compatible;

	compatible = "annapurna-labs,al-eth-common";
	np = of_find_compatible_node(NULL, NULL, compatible);

	/**
	 * Notice! kernel expects an enabled entry to hold status property of "ok" or "okay"
	 * (and not "enabled")
	 * in case the status property is absent - we treat it as it was "ok".
	 * if status is set to "enabled" - it will not be parsed and treated as disabled.
	 *
	 * a disabled entry should have the status property set to "disabled"
	 */
	if (np && of_device_is_available(np)) {
		eth_common_base = of_iomap(np, 0);
		BUG_ON(!eth_common_base);
	} else {
		eth_common_base = NULL;
	}

	return 0;
}

void __iomem *al_mod_eth_common_resource_get(enum al_mod_eth_common_resource_type type)
{
	void __iomem *res_base = NULL;

	if (eth_common_base == NULL) {
		pr_err("%s: Error, eth-common is not supported on this platform "
			"(check your device tree)\n", __func__);
		BUG_ON(!0);
	}

	switch (type) {
	case AL_ETH_COMMON_RESOURCE_TYPE_COMMON_REGS:
		res_base = eth_common_base + AL_ETH_COMMON_REGS_OFFSET;
		break;
	case AL_ETH_COMMON_RESOURCE_TYPE_SCACHE:
		res_base = eth_common_base + AL_ETH_COMMON_SCACHE_OFFSET;
		break;
	case AL_ETH_COMMON_RESOURCE_TYPE_SHARED_MAC:
		res_base = eth_common_base + AL_ETH_COMMON_SHARED_MAC_OFFSET;
		break;
	case AL_ETH_COMMON_RESOURCE_TYPE_SHARED_RESOURCE:
		res_base = eth_common_base + AL_ETH_COMMON_SHARED_RESOURCES_OFFSET;
		break;
	default:
		pr_err("%s: Error, invalid type (%d)\n", __func__, type);
		BUG_ON(!0);
	}

	return res_base;
}
