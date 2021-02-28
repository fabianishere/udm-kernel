/*
 * linux/drivers/soc/alpine/alpine_serdes.c
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
#include <linux/init.h>
#include <linux/thermal.h>

#include <al_mod_serdes.h>
#include "alpine_serdes.h"

#include <linux/netdevice.h>

static void __iomem *serdes_base;

static uint32_t serdes_grp_offset[] = {0, 0x400, 0x800, 0xc00, 0x2000};

static struct al_mod_serdes_grp_obj serdes_grp_obj[AL_SRDS_NUM_GROUPS];

static bool alpine_serdes_of_device_is_available(const struct device_node *device)
{
	const char *status;
	int statlen;

	if (!device)
		return false;

	if (of_device_is_available(device))
		return true;

	status = of_get_property(device, "status", &statlen);
	if (status == NULL)
		return true;

	if (statlen > 0 && !strcmp(status, "enabled"))
			return true;

	return false;
}

static int __init al_mod_serdes_resource_init(void)
{
	struct device_node *np;

	/* Find the serdes node and make sure it is not disabled */
	np = of_find_compatible_node(NULL, NULL, "annapurna-labs,al-serdes");

	if (np && alpine_serdes_of_device_is_available(np)) {
		serdes_base = of_iomap(np, 0);
		BUG_ON(!serdes_base);
	} else {
		pr_debug("%s: Didnt find DT node!\n", __func__);
		serdes_base = NULL;
	}
	return 0;
}
arch_initcall(al_mod_serdes_resource_init);

static int alpine_serdes_thermal_get(void)
{
	struct thermal_zone_device *thermal;
	int temp;

	thermal = thermal_zone_get_zone_by_name("cpu-thermal");

	if (!IS_ERR(thermal)) {
		thermal_zone_get_temp(thermal, &temp);
		temp /= 1000;
		return (int)temp;
	}

	pr_info("%s: Falling back to al_thermal-thermal-zone\n", __func__);

	thermal = thermal_zone_get_zone_by_name("al_thermal");

	if (IS_ERR(thermal)) {
		pr_err("%s: thermal sensor driver must be available!\n", __func__);
		BUG();
	}

	thermal_zone_get_temp(thermal, &temp);

	return (int)temp;
}

void __iomem *alpine_serdes_resource_get(u32 group)
{
	void __iomem *base = NULL;

	if (group >= AL_SRDS_NUM_GROUPS)
		return NULL;

	if (serdes_base)
		base = serdes_base + serdes_grp_offset[group];

	return base;
}
EXPORT_SYMBOL(alpine_serdes_resource_get);

static struct alpine_serdes_eth_group_mode {
	struct mutex			lock;
	enum alpine_serdes_eth_mode	mode;
	bool				mode_set;
} alpine_serdes_eth_group_mode[AL_SRDS_NUM_GROUPS] = {
	{
		.lock = __MUTEX_INITIALIZER(alpine_serdes_eth_group_mode[0].lock),
		.mode_set = false,
	},
	{
		.lock = __MUTEX_INITIALIZER(alpine_serdes_eth_group_mode[1].lock),
		.mode_set = false,
	},
	{
		.lock = __MUTEX_INITIALIZER(alpine_serdes_eth_group_mode[2].lock),
		.mode_set = false,
	},
	{
		.lock = __MUTEX_INITIALIZER(alpine_serdes_eth_group_mode[3].lock),
		.mode_set = false,
	},
	{
		.lock = __MUTEX_INITIALIZER(alpine_serdes_eth_group_mode[4].lock),
		.mode_set = false,
	}
};

int alpine_serdes_eth_mode_set(
	u32				group,
	enum alpine_serdes_eth_mode	mode)
{
	struct alpine_serdes_eth_group_mode *group_mode =
		&alpine_serdes_eth_group_mode[group];

	if (!serdes_base)
		return -EINVAL;

	if (group >= AL_SRDS_NUM_GROUPS)
		return -EINVAL;

	mutex_lock(&group_mode->lock);

	if (!group_mode->mode_set || (group_mode->mode != mode)) {
		struct al_mod_serdes_grp_obj *obj = &serdes_grp_obj[group];

		if (mode == ALPINE_SERDES_ETH_MODE_SGMII)
			obj->mode_set_sgmii(obj);
		else if (mode == ALPINE_SERDES_ETH_MODE_25G)
			obj->mode_set_25g(obj);
		else
			obj->mode_set_kr(obj);

		group_mode->mode = mode;
		group_mode->mode_set = true;
	}

	mutex_unlock(&group_mode->lock);

	return 0;
}
EXPORT_SYMBOL(alpine_serdes_eth_mode_set);

void alpine_serdes_eth_group_lock(u32 group)
{
	struct alpine_serdes_eth_group_mode *group_mode =
		&alpine_serdes_eth_group_mode[group];

	mutex_lock(&group_mode->lock);
}
EXPORT_SYMBOL(alpine_serdes_eth_group_lock);

int alpine_serdes_eth_group_trylock(u32 group)
{
	struct alpine_serdes_eth_group_mode *group_mode =
		&alpine_serdes_eth_group_mode[group];

	return mutex_trylock(&group_mode->lock);
}
EXPORT_SYMBOL(alpine_serdes_eth_group_trylock);

void alpine_serdes_eth_group_unlock(u32 group)
{
	struct alpine_serdes_eth_group_mode *group_mode =
		&alpine_serdes_eth_group_mode[group];

	mutex_unlock(&group_mode->lock);
}
EXPORT_SYMBOL(alpine_serdes_eth_group_unlock);

struct al_mod_serdes_grp_obj *alpine_serdes_grp_obj_get(u32 group)
{
	if (group >= AL_SRDS_NUM_GROUPS)
		return NULL;

	return &serdes_grp_obj[group];
}
EXPORT_SYMBOL(alpine_serdes_grp_obj_get);

static int __init alpine_serdes_grp_objs_init(void)
{
	u32 group;

	for (group = 0; group < AL_SRDS_NUM_GROUPS; group++) {
		al_mod_serdes_handle_grp_init(
			alpine_serdes_resource_get(group), group, &serdes_grp_obj[group]);

		al_mod_serdes_temperature_get_cb_set(&serdes_grp_obj[group],
						 &alpine_serdes_thermal_get);
	}

	return 0;
}
subsys_initcall(alpine_serdes_grp_objs_init);
