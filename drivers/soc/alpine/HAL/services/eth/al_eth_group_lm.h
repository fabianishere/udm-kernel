/*******************************************************************************
Copyright (C) 2016 Annapurna Labs Ltd.

This file may be licensed under the terms of the Annapurna Labs Commercial
License Agreement.

Alternatively, this file can be distributed under the terms of the GNU General
Public License V2 as published by the Free Software Foundation and can be
found at http://www.gnu.org/licenses/gpl-2.0.html

Alternatively, redistribution and use in source and binary forms, with or
without modification, are permitted provided that the following conditions are
met:

*     Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

*     Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in
the documentation and/or other materials provided with the
distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#ifndef __AL_ETH_GROUP_LM__
#define __AL_ETH_GROUP_LM__

#include "al_serdes.h"
#include "al_hal_eth.h"
#include "al_init_eth_lm.h"

#define MAX_NUM_OF_LANES AL_SRDS_NUM_LANES
#define MAX_NUM_OF_LINKS MAX_NUM_OF_LANES

enum al_serdes_mode {
	AL_SERDES_MODE_UNKNOWN,
	AL_SERDES_MODE_25G,
	AL_SERDES_MODE_KR,
	AL_SERDES_MODE_SGMII,
};

struct al_eth_group_lm_workaround_data {
	/* save if link detected the need to initiate a workaround*/
	al_bool need_pll_wa;
	al_bool need_watchdog_wa;

	/* save if we already reset the lane due to watchdog wa (and link didn't go up yet
	 * or we haven't performed a serdes reset)
	 */
	al_bool watchdog_wa_reset_lane_done;

	/* last time watchdog was updated for this link, in msec */
	unsigned int watchdog_last_timestamp;
};

struct al_eth_group_lm_link {
	/* holds if link registered to the group_lm manager */
	al_bool registered;

	/* desired speed of the link */
	enum al_serdes_mode desired_speed;

	/* holds the old_mode and new_mode for link. necessary to preserve
	 * through multiple calls to link_detection_step
	 */
	enum al_eth_lm_link_mode old_mode;
	enum al_eth_lm_link_mode new_mode;

	/* saves the link state from the last time we performed a whole link manage flow */
	enum al_eth_lm_link_state last_link_state;

	/* saves the the lm_step_stage of the link*/
	int lm_step_stage;

	/* workaround data for the link*/
	struct al_eth_group_lm_workaround_data wa_data;
};

struct al_eth_group_lm_common_params {
	/* serdes group number */
	enum al_serdes_group serdes_grp;
	/* pointer to serdes object */
	struct al_serdes_grp_obj *serdes_grp_obj;

	/* watchdog timeout per link, in msec*/
	unsigned int watchdog_timeout[MAX_NUM_OF_LINKS];

	/* callbacks to acquire/release the lock - MUST be set by the user
	 * EXAMPLE:
	 *
	 * static struct alpine_serdes_eth_group_mutex {
	 *	struct mutex			lock;
	 * } alpine_serdes_eth_group_mutex[SERDES_NUM_GROUPS] = {
	 *	{
	 *		.lock = __MUTEX_INITIALIZER(alpine_serdes_eth_group_mutex[0].lock),
	 *	},
	 *	{
	 *		.lock = __MUTEX_INITIALIZER(alpine_serdes_eth_group_mutex[1].lock),
	 *	},
	 *	{
	 *		.lock = __MUTEX_INITIALIZER(alpine_serdes_eth_group_mutex[2].lock),
	 *	},
	 *	{
	 *		.lock = __MUTEX_INITIALIZER(alpine_serdes_eth_group_mutex[3].lock),
	 *	},
	 *	{
	 *		.lock = __MUTEX_INITIALIZER(alpine_serdes_eth_group_mutex[4].lock),
	 *	}};
	 *
	 * static int alpine_group_lm_trylock(u32 group)
	 * {
	 *	struct alpine_serdes_eth_group_mutex *group_mutex =
	 *		&alpine_serdes_eth_group_mutex[group];
	 *
	 *	return mutex_trylock(&group_mutex->lock);
	 * }
	 *
	 * static int alpine_group_lm_unlock(u32 group)
	 * {
	 *	struct alpine_serdes_eth_group_mutex *group_mutex =
	 *		&alpine_serdes_eth_group_mutex[group];
	 *
	 *	mutex_unlock(&group_mutex->lock);
	 * }
	 */
	int (*try_lock_cb)(uint32_t group);
	void (*unlock_cb)(uint32_t group);

	/*
	 * callback to get system time in msec - MUST be set by the user
	 * NOTICE: the time is REQUIRED to wraparound over the full range of uint, or else the
	 * behavior is undefined.
	 */
	unsigned int (*get_msec)(void);
};

struct al_eth_group_lm_link_params {
	/* pointer to group_lm_context */
	struct al_eth_lm_context *lm_context;

	/* holds the eth_port_num as referenced by the system. */
	unsigned int eth_port_num;

	/* saves if this link enables group_lm to change the link speed */
	al_bool auto_speed;

	/* decides if to run group_lm_flow at the end of the group_lm_link_manage for
	 * this link. this is helpful when we run all of the group_lm_link_manage in
	 * the same thread, so we can choose to run group_lm_flow only once
	 */
	al_bool skip_group_flow;

	/*
	 * callback which performs client-specific operations
	 * at init stage of al_eth_group_lm_link_manage
	 * EXAMPLE:
	 *
	 * int eth_link_manage_init_stage(void *handle)
	 * {
	 *	struct eth_link *link = (struct eth_link*)handle;
	 *	struct al_eth_board_params	params = {0};
	 *	int rc;
	 *
	 *	rc = al_eth_board_params_get(link->mac_base, &params);
	 *	if (rc != 0) {
	 *		al_err("Failed to get mac params\n");
	 *		return rc;
	 *	}
	 *
	 *	if (memcmp(&params, &link->board_params, sizeof(struct al_eth_board_params))) {
	 *		al_info("new board params found, reconfigure link management\n");
	 *		link->board_params = params;
	 *		eth_lm_config(link);
	 *	}
	 *
	 *	return 0;
	 * }
	 */
	int (*init_cb)(void *handle);

	/* callback which is called when lm_mode changed after
	 * link_detection to perform client-specific operations - MUST be set by the user
	 * EXAMPLE:
	 *
	 * static int al_eth_lm_mode_change(void *handle, enum al_eth_lm_link_mode old_mode,
	 *			  enum al_eth_lm_link_mode new_mode)
	 * {
	 *	struct al_eth_adapter		*adapter = (struct al_eth_adapter*)handle;
	 *	int rc;
	 *
	 *	if (old_mode != AL_ETH_LM_MODE_DISCONNECTED) {
	 *		netdev_info(adapter->netdev, "%s link down\n", __func__);
	 *		adapter->last_link = false;
	 *		al_eth_down(adapter);
	 *	}
	 *
	 *	al_eth_lm_mode_apply(adapter, new_mode);
	 *
	 *	return 0;
	 * }
	 */
	int (*lm_mode_change_cb)(void *handle,
			enum al_eth_lm_link_mode old_mode,
			enum al_eth_lm_link_mode new_mode);

	/* callback which updates the system-specific link status - - MUST be set by the user
	 * EXAMPLE:
	 *
	 * static void alpine_group_lm_update_link_status(void *handle, al_bool link_up)
	 * {
	 *	struct al_eth_adapter		*adapter = (struct al_eth_adapter*)handle;
	 *
	 *	if (link_up)
	 *		netif_carrier_on(adapter->netdev);
	 *	else
	 *		netif_carrier_off(adapter->netdev);
	 * }
	 */
	void (*update_link_status_cb)(void *handle, int up);

	/* callback which performs client-specific operations between link_detection
	 * and link_establish (e.g. mac configuration after serdes spede change, etc.)
	 * EXAMPLE:
	 *
	 * static int al_eth_pre_establish(void *handle, enum al_eth_lm_link_mode old_mode,
	 *				enum al_eth_lm_link_mode new_mode)
	 * {
	 *	if (new_mode != old_mode) {
	 *		if (new_mode != AL_ETH_LM_MODE_DISCONNECTED) {
	 *			rc = al_eth_up(adapter);
	 *			if (rc)
	 *				return rc;
	 *		} else {
	 *			return -ENETDOWN;
	 *		}
	 *	}
	 *
	 *	return 0;
	 * }
	 */
	int (*pre_establish_cb)(void *handle, enum al_eth_lm_link_mode old_mode,
				 enum al_eth_lm_link_mode new_mode);
};

struct al_eth_group_lm_context {
	/* group common parameters which are passed by the user in al_eth_group_lm_init() */
	struct al_eth_group_lm_common_params common_params;

	/* current speed the serdes is configured to */
	enum al_serdes_mode serdes_mode;

	/* link parameters which are passed by the user in al_eth_group_lm_port_register() */
	struct al_eth_group_lm_link_params link_params[MAX_NUM_OF_LINKS];

	/* link internal data */
	struct al_eth_group_lm_link link[MAX_NUM_OF_LINKS];
};


/**
 * initialize the group_lm_context instance.
 *
 * @param group_lm_context pointer to group_lm_context
 * @param group_lm_common_params pointer to group_lm_common_params
 *
 * @return  0 in case of success. otherwise on failure.
 */
void al_eth_group_lm_init(struct al_eth_group_lm_context *group_lm_context,
			struct al_eth_group_lm_common_params *group_lm_common_params);

/**
 * register an ethernet port to a group_lm manager
 * NOTICE: this function must be called using the lock supplied to the group_lm_context
 *
 * @param group_lm_context pointer to group_lm_context
 * @param link_id link number on the serdes group
 * @param link_params pointer to link_params for the link
 *
 * @return  0 in case of success. otherwise on failure.
 */
int al_eth_group_lm_port_register(struct al_eth_group_lm_context *group_lm_context,
				  unsigned int link_id,
				  struct al_eth_group_lm_link_params *link_params);

/**
 * unregister an ethernet port from a group_lm_manager
 * NOTICE: this function must be called using the lock supplied to the group_lm_context
 *
 * @param group_lm_context pointer to group_lm_context
 * @param link_id link number on the serdes group
 *
 * @return  0 in case of success. otherwise on failure.
 */
int al_eth_group_lm_port_unregister(struct al_eth_group_lm_context *group_lm_context,
				    unsigned int link_id);

/**
 * perform the flow of link_management for a specific link
 *
 * @param group_lm_context pointer to group_lm_context
 * @param link_id link number on the serdes group
 * @param handle pointer to system specific structure
 *	  which should be passed to callbacks
 *
 * @return  0 in case of finishing a full iteration, -EINPROGRESS if still in the middle of
 *	    an iteration. otherwise on failure.
 */
int al_eth_group_lm_link_manage(struct al_eth_group_lm_context *group_lm_context,
				unsigned int link_id, void *handle);

/**
 * enable/disable skipping the group_lm_flow for this link (at the end of group_lm_link_manage)
 * NOTICE: this function must be called using the lock supplied to the group_lm_context
 *
 * @param group_lm_context pointer to group_lm_context
 * @param link_id link number on the serdes group
 * @param skip_group_flow_en TRUE to skip group_flow for this link. FALSE to run group_flow
 *
 * @return  0 in case of success. otherwise on failure.
 */
int al_eth_group_lm_skip_group_flow_enable(struct al_eth_group_lm_context *group_lm_context,
					unsigned int link_id, al_bool skip_group_flow_en);

/**
 * check if we are in the course of a group_link_manage flow iteration for a link
 *
 * @param group_lm_context pointer to group_lm_context
 * @param link_id link number on the serdes group
 * @param is_done pointer to return if we are in the course of group_link_manage or not
 *
 * @return 0 in case of success, otherwise on failure
 * @note If @ref al_eth_group_lm_link_manage is running in different threads for each link,
 * this function needs to be called using the lock supplied to the group_lm_context
 */
int al_eth_group_lm_link_manage_is_done(struct al_eth_group_lm_context *group_lm_context,
				unsigned int link_id, al_bool *is_done);

#endif
