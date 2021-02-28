/*
 * linux/drivers/soc/alpine/alpine_group_lm.c
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

#include <linux/time.h>
#include <linux/timer.h>
#include "al_mod_serdes.h"
#include "alpine_serdes.h"

#include "al_mod_eth_group_lm.h"

#define GROUP_LM_WORKAROUND_WATCHDOG_TIMEOUT 30000 /* [msec] */

static struct al_mod_eth_group_lm_context group_lm_context[AL_SRDS_NUM_GROUPS];

static unsigned int alpine_systime_msec_get(void)
{
	struct timespec ts;
	getnstimeofday(&ts);

	return (unsigned int)((ts.tv_sec * 1000) + (ts.tv_nsec / 1000000));
}

static int __init alpine_group_lm_init(void)
{
	enum al_mod_serdes_group serdes_grp;
	enum al_mod_serdes_lane serdes_lane;

	struct al_mod_eth_group_lm_common_params common_params;

	for (serdes_grp = AL_SRDS_GRP_C; serdes_grp < AL_SRDS_NUM_GROUPS; serdes_grp++) {

		common_params.serdes_grp = serdes_grp;
		common_params.serdes_grp_obj = alpine_serdes_grp_obj_get(serdes_grp);
		if (common_params.serdes_grp_obj == NULL)
			return -EINVAL;

		common_params.get_msec = &alpine_systime_msec_get;
		common_params.try_lock_cb = &alpine_serdes_eth_group_trylock;
		common_params.unlock_cb = &alpine_serdes_eth_group_unlock;

		for (serdes_lane = 0; serdes_lane < MAX_NUM_OF_LANES; serdes_lane++)
			common_params.watchdog_timeout[serdes_lane] =
				GROUP_LM_WORKAROUND_WATCHDOG_TIMEOUT;

		al_mod_eth_group_lm_init(&group_lm_context[serdes_grp],
			&common_params);
	}

	return 0;
}
/* using fs_initcall so this function is invoked after alpine_serdes_grp_objs_init() */
fs_initcall(alpine_group_lm_init);

struct al_mod_eth_group_lm_context *alpine_group_lm_get(u32 group)
{
	if (group >= AL_SRDS_NUM_GROUPS)
		return NULL;

	return &group_lm_context[group];
}
EXPORT_SYMBOL(alpine_group_lm_get);
