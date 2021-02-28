/*
 * Machine declaration for Alpine platforms.
 *
 * Copyright (C) 2015 Annapurna Labs Ltd.
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
 */

#include <linux/of_platform.h>
#include <linux/of_address.h>
#include <asm/mach/arch.h>

#include "al_init_sys_fabric.h"

static const char * const al_match[] __initconst = {
	"al,alpine",
	NULL,
};

static struct of_device_id of_ccu_table[] = {
	{.compatible = "al,alpine-ccu"},
	{ /* end of list */ },
};

int al_fabric_hwcc __read_mostly;

int al_fabric_hwcc_enabled(void)
{
	return al_fabric_hwcc;
}
EXPORT_SYMBOL(al_fabric_hwcc_enabled);


static void __init al_init(void)
{
	struct device_node *ccu_node;
	void __iomem *ccu_address;
	u32 prop;

	ccu_node = of_find_matching_node(NULL, of_ccu_table);

	if (ccu_node && of_device_is_available(ccu_node)) {
		al_fabric_hwcc = !of_property_read_u32(ccu_node, "io_coherency", &prop)
			&& prop;
		ccu_address = of_iomap(ccu_node, 0);
		BUG_ON(!ccu_address);
		if (al_fabric_hwcc)
			printk("Enabling IO Cache Coherency.\n");
		al_ccu_init(ccu_address, al_fabric_hwcc);
	}
	of_platform_populate(NULL, of_default_bus_match_table, NULL, NULL);
}


unsigned int al_spin_lock_wfe_enable __read_mostly = 0;
EXPORT_SYMBOL(al_spin_lock_wfe_enable);

static int __init spin_lock_wfe_enable(char *str)
{
	get_option(&str, &al_spin_lock_wfe_enable);
	if (al_spin_lock_wfe_enable)
		al_spin_lock_wfe_enable = 1;
	return 0;
}

early_param("spin_lock_wfe_enable", spin_lock_wfe_enable);

DT_MACHINE_START(AL_DT, "Annapurna Labs Alpine")
	.init_machine	= al_init,
	.dt_compat	= al_match,
MACHINE_END
