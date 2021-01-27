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

#include "al_eth_group_lm.h"

#define GROUP_LM_SERDES_25G_MASTER_LANE 0

enum al_eth_group_lm_step_stage {
	GROUP_LM_STEP_STAGE_INIT,
	GROUP_LM_STEP_STAGE_LINK_STATE_CHECK,
	GROUP_LM_STEP_STAGE_TRY_LOCK_DETECTION,
	GROUP_LM_STEP_STAGE_DETECTION_DONE,
	GROUP_LM_STEP_STAGE_TRY_LOCK_SPEED_CHANGE,
	GROUP_LM_STEP_STAGE_PRE_ESTABLISH,
	GROUP_LM_STEP_STAGE_TRY_LOCK_ESTABLISH,
	GROUP_LM_STEP_STAGE_ESTABLISH_DONE,
	GROUP_LM_STEP_STAGE_GROUP_LM_FLOW,
};

/* this macro assumes that link_id is unsigned */
#define GROUP_LM_LINK_ID_CHECK(group_lm_context, link_id)					\
	do {											\
		al_assert(((link_id) < MAX_NUM_OF_LINKS));					\
		al_assert(!((group_lm_context)->common_params.serdes_grp_obj->type_get() ==	\
				AL_SRDS_TYPE_25G) ||						\
			((link_id) < AL_SRDS_25G_NUM_LANES));					\
	  } while (0)

static const char *al_eth_group_lm_step_stage_convert_to_str(enum al_eth_group_lm_step_stage val)
{
	switch (val) {
	case GROUP_LM_STEP_STAGE_INIT:
		return "GROUP_LM_STEP_STAGE_INIT";
	case GROUP_LM_STEP_STAGE_LINK_STATE_CHECK:
		return "GROUP_LM_STEP_STAGE_LINK_STATE_CHECK";
	case GROUP_LM_STEP_STAGE_TRY_LOCK_DETECTION:
		return "GROUP_LM_STEP_STAGE_TRY_LOCK_DETECTION";
	case GROUP_LM_STEP_STAGE_DETECTION_DONE:
		return "GROUP_LM_STEP_STAGE_DETECTION_DONE";
	case GROUP_LM_STEP_STAGE_TRY_LOCK_SPEED_CHANGE:
		return "GROUP_LM_STEP_STAGE_TRY_LOCK_SPEED_CHANGE";
	case GROUP_LM_STEP_STAGE_PRE_ESTABLISH:
		return "GROUP_LM_STEP_STAGE_PRE_ESTABLISH";
	case GROUP_LM_STEP_STAGE_TRY_LOCK_ESTABLISH:
		return "GROUP_LM_STEP_STAGE_TRY_LOCK_ESTABLISH";
	case GROUP_LM_STEP_STAGE_ESTABLISH_DONE:
		return "GROUP_LM_STEP_STAGE_ESTABLISH_DONE";
	case GROUP_LM_STEP_STAGE_GROUP_LM_FLOW:
		return "GROUP_LM_STEP_STAGE_GROUP_LM_FLOW";
	}

	return "N/A";
}

int al_eth_group_lm_port_register(struct al_eth_group_lm_context *group_lm_context,
				  unsigned int link_id,
				  struct al_eth_group_lm_link_params *link_params)
{

	GROUP_LM_LINK_ID_CHECK(group_lm_context, link_id);

	if (group_lm_context->link[link_id].registered == AL_TRUE) {
		al_err("%s: link_id %d of serdes_group %d is already registered\n",
			__func__, link_id, group_lm_context->common_params.serdes_grp);
		return -EINVAL;
	} else {

		al_assert(link_params->lm_mode_change_cb);
		al_assert(link_params->update_link_status_cb);

		group_lm_context->link_params[link_id] = *link_params;

		group_lm_context->link[link_id].last_link_state = AL_ETH_LM_LINK_DOWN;
		group_lm_context->link[link_id].lm_step_stage = GROUP_LM_STEP_STAGE_INIT;
		group_lm_context->link[link_id].desired_speed = AL_SERDES_MODE_UNKNOWN;
		group_lm_context->link[link_id].old_mode = AL_ETH_LM_MODE_DISCONNECTED;
		group_lm_context->link[link_id].new_mode = AL_ETH_LM_MODE_DISCONNECTED;
		group_lm_context->link[link_id].wa_data.need_pll_wa = AL_FALSE;
		group_lm_context->link[link_id].wa_data.need_watchdog_wa = AL_FALSE;
		group_lm_context->link[link_id].wa_data.watchdog_wa_reset_lane_done = AL_FALSE;
		group_lm_context->link[link_id].registered = AL_TRUE;
		group_lm_context->link[link_id].wa_data.watchdog_last_timestamp =
			group_lm_context->common_params.get_msec();
	}

	return 0;
}

int al_eth_group_lm_port_unregister(struct al_eth_group_lm_context *group_lm_context,
				    unsigned int link_id)
{

	GROUP_LM_LINK_ID_CHECK(group_lm_context, link_id);

	if (group_lm_context->link[link_id].registered == AL_FALSE) {
		al_warn("%s: link_id %d of serdesgroup %d is already unregistered\n",
			__func__, link_id, group_lm_context->common_params.serdes_grp);
	} else {
		group_lm_context->link[link_id].registered = AL_FALSE;
	}

	return 0;
}

static int al_eth_group_lm_check_workarounds(struct al_eth_group_lm_context *group_lm_context,
					     unsigned int link_id)
{
	struct al_eth_lm_context *lm_context;
	enum al_eth_lm_link_state link_state;

	unsigned int current_time;

	GROUP_LM_LINK_ID_CHECK(group_lm_context, link_id);

	lm_context = group_lm_context->link_params[link_id].lm_context;
	group_lm_context->link[link_id].wa_data.need_pll_wa = AL_FALSE;
	group_lm_context->link[link_id].wa_data.need_watchdog_wa = AL_FALSE;

	link_state = al_eth_lm_link_state_get(lm_context);

	current_time = group_lm_context->common_params.get_msec();

	if ((group_lm_context->link[link_id].new_mode != AL_ETH_LM_MODE_DISCONNECTED) &&
		(link_state != AL_ETH_LM_LINK_UP)) {
		unsigned int elapsed_time;
		al_bool tx_cdr_lock;

		/**** PLL workaround check ****/

		/* accessing shared resource */
		tx_cdr_lock = al_eth_lm_retimer_tx_cdr_lock_get(lm_context);

		if (tx_cdr_lock == AL_FALSE)
			group_lm_context->link[link_id].wa_data.need_pll_wa = AL_TRUE;

		/**** General Watchdog workaround check ****/
		elapsed_time = current_time -
			group_lm_context->link[link_id].wa_data.watchdog_last_timestamp;

		if (elapsed_time >=
			group_lm_context->common_params.watchdog_timeout[link_id])
			group_lm_context->link[link_id].wa_data.need_watchdog_wa = AL_TRUE;

		al_dbg("%s: link %d on serdes_grp %d - watchdog_wa last_timestamp: %dms, elapsed_time: %dms, timeout val: %dms\n",
			__func__, link_id, group_lm_context->common_params.serdes_grp,
			group_lm_context->link[link_id].wa_data.watchdog_last_timestamp,
			elapsed_time,
			group_lm_context->common_params.watchdog_timeout[link_id]);
	} else {
		group_lm_context->link[link_id].wa_data.watchdog_wa_reset_lane_done =
			AL_FALSE;
		group_lm_context->link[link_id].wa_data.watchdog_last_timestamp =
			current_time;
	}


	al_dbg("%s:end of check_workarounds. link %d of serdes %d - pll_flag:%d, watchdog_flag;%d\n",
		__func__, link_id, group_lm_context->common_params.serdes_grp,
		group_lm_context->link[link_id].wa_data.need_pll_wa,
		group_lm_context->link[link_id].wa_data.need_watchdog_wa);

	return 0;
}

static int al_eth_group_lm_set_desired_speed(struct al_eth_group_lm_context *group_lm_context,
						   unsigned int link_id)
{
	GROUP_LM_LINK_ID_CHECK(group_lm_context, link_id);

	switch (group_lm_context->link[link_id].new_mode) {
	case AL_ETH_LM_MODE_25G:
		group_lm_context->link[link_id].desired_speed = AL_SERDES_MODE_25G;
		break;
	case AL_ETH_LM_MODE_10G_DA:
	case AL_ETH_LM_MODE_10G_OPTIC:
		group_lm_context->link[link_id].desired_speed = AL_SERDES_MODE_KR;
		break;
	case AL_ETH_LM_MODE_1G:
		group_lm_context->link[link_id].desired_speed = AL_SERDES_MODE_SGMII;
		break;
	default:
		al_err("%s: new LM mode is not supported by bootapp\n", __func__);
		return -EINVAL;
	};

	return 0;
}

static void al_eth_group_lm_serdes_speed_change(struct al_eth_group_lm_context *group_lm_context,
					 unsigned int link_id)
{
	struct al_serdes_grp_obj	*serdes_grp_obj =
		group_lm_context->common_params.serdes_grp_obj;
	enum al_serdes_type serdes_type = serdes_grp_obj->type_get();

	al_dbg("%s:serdes_grp: %d ,serdes_type:%d, serdes_speed:%d, serdes_desired_speed:%d\n",
		__func__, group_lm_context->common_params.serdes_grp, serdes_type,
		group_lm_context->serdes_mode,
		group_lm_context->link[link_id].desired_speed);

	if (group_lm_context->link[link_id].desired_speed != group_lm_context->serdes_mode) {
		switch (serdes_type) {
		case AL_SRDS_TYPE_25G:
			if ((group_lm_context->serdes_mode == AL_SERDES_MODE_UNKNOWN) ||
				((group_lm_context->link_params[link_id].auto_speed) &&
					(link_id == GROUP_LM_SERDES_25G_MASTER_LANE))) {
				/* accessing a shared resource */
				if ((group_lm_context->link[link_id].desired_speed ==
						AL_SERDES_MODE_25G)){
					serdes_grp_obj->mode_set_25g(serdes_grp_obj);
					group_lm_context->serdes_mode = AL_SERDES_MODE_25G;
				} else if (group_lm_context->link[link_id].desired_speed ==
					AL_SERDES_MODE_KR) {
					serdes_grp_obj->mode_set_kr(serdes_grp_obj);
					group_lm_context->serdes_mode = AL_SERDES_MODE_KR;
				} else {
					al_err("%s: Invalid desired_speed(%d) for link%d on serdes %d\n",
						__func__, group_lm_context->link[link_id].
						desired_speed, link_id,
						group_lm_context->common_params.
						serdes_grp);
				}
			}

			break;
		case AL_SRDS_TYPE_HSSP:
			if ((group_lm_context->link_params[link_id].auto_speed) ||
				(group_lm_context->serdes_mode == AL_SERDES_MODE_UNKNOWN)) {
				/* accessing a shared resource */
				if (group_lm_context->link[link_id].desired_speed ==
						AL_SERDES_MODE_KR) {
					serdes_grp_obj->mode_set_kr(serdes_grp_obj);
					group_lm_context->serdes_mode = AL_SERDES_MODE_KR;
				} else if (group_lm_context->link[link_id].desired_speed ==
					AL_SERDES_MODE_SGMII) {
					serdes_grp_obj->mode_set_sgmii(serdes_grp_obj);
					group_lm_context->serdes_mode = AL_SERDES_MODE_SGMII;
				} else {
					al_err("%s: Invalid desired_speed(%d) for link%d on serdes %d\n",
						__func__, group_lm_context->link[link_id].
						desired_speed, link_id,
						group_lm_context->common_params.
						serdes_grp);
				}
			}

			break;
		default:
			al_err("%s:Undefined serdes type\n", __func__);
		}
	}
}

static void al_eth_group_lm_group_flow(struct al_eth_group_lm_context *group_lm_context)
{
	al_bool initiate_pll_workaround = AL_FALSE;
	al_bool initiate_watchdog_wa_serdes_init = AL_FALSE;
	int i;

	al_assert(group_lm_context);

	al_dbg("%s: Performing group_lm_flow for serdes %d\n", __func__,
		group_lm_context->common_params.serdes_grp);

	/* check if pll workaround is necessary */
	for (i = 0; i < MAX_NUM_OF_LINKS; i++) {
		if (group_lm_context->link[i].registered == AL_FALSE)
			continue;

		if (group_lm_context->link[i].new_mode !=
			AL_ETH_LM_MODE_DISCONNECTED) {
			if (group_lm_context->link[i].wa_data.need_pll_wa == AL_TRUE)
				initiate_pll_workaround = AL_TRUE;
			else {
				initiate_pll_workaround = AL_FALSE;
				break;
			}
		}
	}

	/* check if init of serdes is necessary due to general watchdog workaround */
	for (i = 0; i < MAX_NUM_OF_LINKS; i++) {
		if (group_lm_context->link[i].registered == AL_FALSE)
			continue;

		if (group_lm_context->link[i].new_mode !=
			AL_ETH_LM_MODE_DISCONNECTED) {
			if (group_lm_context->link[i].wa_data.need_watchdog_wa == AL_TRUE)
				initiate_watchdog_wa_serdes_init = AL_TRUE;
			else {
				initiate_watchdog_wa_serdes_init = AL_FALSE;
				break;
			}
		}
	}

	if (initiate_pll_workaround || initiate_watchdog_wa_serdes_init) {
		if (group_lm_context->serdes_mode == AL_SERDES_MODE_25G) {
			group_lm_context->common_params.serdes_grp_obj->mode_set_25g(
				group_lm_context->common_params.serdes_grp_obj);
			al_info("%s: Initiated workaround for serdes %d. Initialized serdes to 25G mode\n",
				__func__, group_lm_context->common_params.serdes_grp);
		} else if (group_lm_context->serdes_mode == AL_SERDES_MODE_KR) {
			group_lm_context->common_params.serdes_grp_obj->mode_set_kr(
				group_lm_context->common_params.serdes_grp_obj);
			al_info("%s: Initiated workaround for serdes %d. Initialized serdes to 10G mode\n",
				__func__, group_lm_context->common_params.serdes_grp);
		} else {
			al_info("%s: Initiated workaround when previous state of serdes was unknown\n",
				__func__);
			group_lm_context->common_params.serdes_grp_obj->mode_set_25g(
				group_lm_context->common_params.serdes_grp_obj);
			group_lm_context->serdes_mode = AL_SERDES_MODE_25G;
		}

		for (i = 0; i < MAX_NUM_OF_LINKS; i++) {
			if (group_lm_context->link[i].registered == AL_FALSE)
				continue;
			group_lm_context->link[i].wa_data.watchdog_last_timestamp =
				group_lm_context->common_params.get_msec();
			group_lm_context->link[i].wa_data.need_watchdog_wa = AL_FALSE;
			group_lm_context->link[i].wa_data.watchdog_wa_reset_lane_done = AL_FALSE;
		}

	} else {
		for (i = 0; i < MAX_NUM_OF_LINKS; i++) {
			if (group_lm_context->link[i].registered == AL_FALSE)
				continue;
			if (group_lm_context->link[i].wa_data.need_watchdog_wa &&
				!group_lm_context->link[i].wa_data.watchdog_wa_reset_lane_done) {
				/* Once the corrext sequence for reset_lane is known we need to add
				 * the call here:
				 * group_lm_context->common_params.serdes_grp_obj->reset_lane(
				 * group_lm_context->common_params.serdes_grp_obj, i);
				 */

				group_lm_context->link[i].wa_data.watchdog_wa_reset_lane_done =
					AL_TRUE;
				/* The only way to exit watchdog_wa_reset_lane_done is by either
				 * resetting the serdes or the link going up
				 */
				al_info("%s: General Watchdog workaround of link%d in serdes group %d Timed Out!\n",
					__func__, i, group_lm_context->common_params.serdes_grp);
			}
		}
	}
}

#define GROUP_LM_STEP_STAGE_CHANGE_STATE(state, new_state)					\
	do {											\
		al_dbg("%s: CHANGING GROUP_LM_STEP_STAGE from %s to %s\n", __func__,		\
			al_eth_group_lm_step_stage_convert_to_str(state), (#new_state));	\
		(state) = (new_state);								\
	} while (0)

/* need to think about the 40G case. we should register the port on lane 0 only,
 * to make it compatible with this code */
int al_eth_group_lm_link_manage(struct al_eth_group_lm_context *group_lm_context,
				unsigned int link_id, void *handle)
{
	int rc = 0, lock;
	al_bool fault;
	enum al_eth_lm_link_mode *new_mode = &group_lm_context->link[link_id].new_mode;
	enum al_eth_lm_link_mode *old_mode = &group_lm_context->link[link_id].old_mode;
	enum al_eth_group_lm_step_stage *lm_step_stage =
		(enum al_eth_group_lm_step_stage *)&group_lm_context->link[link_id].lm_step_stage;
	struct al_eth_lm_context *lm_context = group_lm_context->link_params[link_id].lm_context;
	enum al_serdes_group serdes_grp = group_lm_context->common_params.serdes_grp;
	al_bool link_up;
	enum al_eth_lm_link_state link_state;
	al_bool lm_pause_status;

	GROUP_LM_LINK_ID_CHECK(group_lm_context, link_id);

	if (!group_lm_context->link[link_id].registered) {
		al_err("%s: Trying to perform link_status for an unregistered link! link:%d, serdes_grp:%d\n",
			__func__, link_id, serdes_grp);
		return -EINVAL;
	}

	al_dbg("%s: entering link_manage for link:%d, serdes_grp:%d\n", __func__,
		link_id, serdes_grp);

	switch (*lm_step_stage) {
	case GROUP_LM_STEP_STAGE_INIT:
		lm_pause_status = al_eth_lm_pause_check(lm_context);
		if (lm_pause_status == AL_TRUE) {
			rc = 0;
			goto out;
		}

		if (group_lm_context->link_params[link_id].init_cb) {
			rc = group_lm_context->link_params[link_id].init_cb(handle);

			if (rc != 0) {
				GROUP_LM_STEP_STAGE_CHANGE_STATE(*lm_step_stage,
								GROUP_LM_STEP_STAGE_GROUP_LM_FLOW);
				break;
			}
		}

		GROUP_LM_STEP_STAGE_CHANGE_STATE(*lm_step_stage,
						GROUP_LM_STEP_STAGE_LINK_STATE_CHECK);
		/* fall through */
	case GROUP_LM_STEP_STAGE_LINK_STATE_CHECK:
		/* This stage is helpful for the case that the current port's previous link_state
		 * was up and its current state is also up. If another other port holds the
		 * lock and is currently configuring the retimer, this could take a long time and
		 * the current port would be waiting for no reason
		 */
		link_state = al_eth_lm_link_state_get(lm_context);
		if (link_state == AL_ETH_LM_LINK_UP) {
			rc = al_eth_lm_link_check(lm_context, &link_state);

			if ((rc == 0) && (link_state == AL_ETH_LM_LINK_UP)) {
				GROUP_LM_STEP_STAGE_CHANGE_STATE(*lm_step_stage,
								GROUP_LM_STEP_STAGE_GROUP_LM_FLOW);
				break;
			}
		}

		GROUP_LM_STEP_STAGE_CHANGE_STATE(*lm_step_stage,
						GROUP_LM_STEP_STAGE_TRY_LOCK_DETECTION);
		/* fall through */
	case GROUP_LM_STEP_STAGE_TRY_LOCK_DETECTION:
		/* we need to lock link_detection only in 25g serdes, since it accesses the retimer,
		 * which is a shared resource
		 */
		if (group_lm_context->common_params.serdes_grp_obj->type_get() ==
			AL_SRDS_TYPE_25G) {
			lock = group_lm_context->common_params.try_lock_cb(
				serdes_grp);

			al_dbg("%s:link %d of serdes %d after try_lock. lock: %d - step_stage: %s\n",
				__func__, link_id, serdes_grp, lock,
				al_eth_group_lm_step_stage_convert_to_str(*lm_step_stage));

			if (lock == 0)
				break;
		}

		rc = al_eth_lm_link_detection_step(lm_context, &fault, old_mode, new_mode);
		if (group_lm_context->common_params.serdes_grp_obj->type_get() ==
			AL_SRDS_TYPE_25G) {
			group_lm_context->common_params.unlock_cb(
				serdes_grp);

			al_dbg("%s:link %d of serdes %d released group lock - step_stage: %s\n",
				__func__, link_id, serdes_grp,
				al_eth_group_lm_step_stage_convert_to_str(*lm_step_stage));
		}

		if (rc == -EINPROGRESS)
			break;

		if (rc != 0) {
			al_err("Link detection failed (%d)\n", rc);
			GROUP_LM_STEP_STAGE_CHANGE_STATE(*lm_step_stage,
							GROUP_LM_STEP_STAGE_GROUP_LM_FLOW);
			break;
		}

		GROUP_LM_STEP_STAGE_CHANGE_STATE(*lm_step_stage,
						GROUP_LM_STEP_STAGE_DETECTION_DONE);
		/* fall through */
	case GROUP_LM_STEP_STAGE_DETECTION_DONE:
		if (fault == AL_FALSE) {
			/* The link is still up so we can skip the rest of the flow */
			GROUP_LM_STEP_STAGE_CHANGE_STATE(*lm_step_stage,
							GROUP_LM_STEP_STAGE_GROUP_LM_FLOW);
			break;
		}

		al_dbg("%s: link %d of serdes %d - after link_detection -\nold_mode:%s, new_mode:%s\n",
			__func__, link_id, serdes_grp,
			al_eth_lm_mode_convert_to_str(*old_mode),
			al_eth_lm_mode_convert_to_str(*new_mode));

		if (*new_mode == *old_mode) {
			if (*new_mode == AL_ETH_LM_MODE_DISCONNECTED) {
				/* still disconnected so no need to attempt establish */
				GROUP_LM_STEP_STAGE_CHANGE_STATE(*lm_step_stage,
								GROUP_LM_STEP_STAGE_GROUP_LM_FLOW);
				break;
			}
		} else {
			/* run System specific operations in the case of a mode change */
			rc = group_lm_context->link_params[link_id].lm_mode_change_cb(handle,
										*old_mode,
										*new_mode);

			if (rc || (*new_mode == AL_ETH_LM_MODE_DISCONNECTED)) {
				GROUP_LM_STEP_STAGE_CHANGE_STATE(*lm_step_stage,
								GROUP_LM_STEP_STAGE_GROUP_LM_FLOW);
				break;
			}
		}

		GROUP_LM_STEP_STAGE_CHANGE_STATE(*lm_step_stage,
						GROUP_LM_STEP_STAGE_TRY_LOCK_SPEED_CHANGE);

	/* fall through */
	case GROUP_LM_STEP_STAGE_TRY_LOCK_SPEED_CHANGE:
		lock = group_lm_context->common_params.try_lock_cb(
			serdes_grp);

		al_dbg("%s:link %d of serdes %d after try_lock. lock: %d - step_stage: %s\n",
			__func__, link_id, serdes_grp,
			lock, al_eth_group_lm_step_stage_convert_to_str(*lm_step_stage));

		if (lock == 0)
			break;

		al_eth_group_lm_set_desired_speed(group_lm_context, link_id);

		al_eth_group_lm_serdes_speed_change(group_lm_context, link_id);

		group_lm_context->common_params.unlock_cb(
			serdes_grp);

		al_dbg("%s:link %d of serdes %d released group lock - step_stage: %s\n", __func__,
			link_id, serdes_grp,
			al_eth_group_lm_step_stage_convert_to_str(*lm_step_stage));

		GROUP_LM_STEP_STAGE_CHANGE_STATE(*lm_step_stage, GROUP_LM_STEP_STAGE_PRE_ESTABLISH);
		/* fall through */
	case GROUP_LM_STEP_STAGE_PRE_ESTABLISH:
		if (group_lm_context->link_params[link_id].pre_establish_cb) {
			rc = group_lm_context->link_params[link_id].pre_establish_cb(handle,
										*old_mode,
										*new_mode);
			if (rc == -EINPROGRESS)
				break;

			if (rc != 0) {
				GROUP_LM_STEP_STAGE_CHANGE_STATE(*lm_step_stage,
								GROUP_LM_STEP_STAGE_GROUP_LM_FLOW);
				break;
			}
		}
		GROUP_LM_STEP_STAGE_CHANGE_STATE(*lm_step_stage,
						GROUP_LM_STEP_STAGE_TRY_LOCK_ESTABLISH);
		/* fall through */
	case GROUP_LM_STEP_STAGE_TRY_LOCK_ESTABLISH:
		lock = group_lm_context->common_params.try_lock_cb(
			serdes_grp);

		al_dbg("%s:link %d of serdes %d after try_lock. lock: %d - step_stage: %s\n",
			__func__, link_id, serdes_grp, lock,
			al_eth_group_lm_step_stage_convert_to_str(*lm_step_stage));

		if (lock == 0)
			break;

		rc = al_eth_lm_link_establish_step(lm_context, &link_up);

		group_lm_context->common_params.unlock_cb(
			serdes_grp);

		al_dbg("%s:link %d of serdes %d released group lock - step_stage: %s\n", __func__,
			link_id, serdes_grp,
			al_eth_group_lm_step_stage_convert_to_str(*lm_step_stage));

		if (rc == -EINPROGRESS)
			break;

		GROUP_LM_STEP_STAGE_CHANGE_STATE(*lm_step_stage,
						GROUP_LM_STEP_STAGE_ESTABLISH_DONE);
		/* fall through */
	case GROUP_LM_STEP_STAGE_ESTABLISH_DONE:
		if (rc == 0) {
			if (link_up == AL_TRUE) {
				al_info("%s: eth%d - link established successfully\n",
					__func__,
					group_lm_context->link_params[link_id].eth_port_num);
				group_lm_context->link_params[link_id].update_link_status_cb(handle,
											AL_TRUE);
				group_lm_context->link[link_id].last_link_state = AL_ETH_LM_LINK_UP;
			} else {
				/* DOWN_RF */
				if (group_lm_context->link[link_id].last_link_state ==
					AL_ETH_LM_LINK_UP)
					group_lm_context->link_params[link_id].update_link_status_cb
						(handle, AL_FALSE);

				if (group_lm_context->link[link_id].last_link_state !=
					AL_ETH_LM_LINK_DOWN_RF) {
					al_info("%s: eth%d - link wasn't established (remote side might have a problem)\n",
						__func__,
						group_lm_context->link_params[link_id].
						eth_port_num);

					group_lm_context->link[link_id].last_link_state =
					AL_ETH_LM_LINK_DOWN_RF;
				}
			}
		} else {
			if (group_lm_context->link[link_id].last_link_state == AL_ETH_LM_LINK_UP) {
				al_info("%s: eth%d - link wasn't established, link went down\n",
					__func__,
					group_lm_context->link_params[link_id].eth_port_num);

				group_lm_context->link_params[link_id].update_link_status_cb(handle,
											AL_FALSE);
			}

			group_lm_context->link[link_id].last_link_state = AL_ETH_LM_LINK_DOWN;
		}

		GROUP_LM_STEP_STAGE_CHANGE_STATE(*lm_step_stage, GROUP_LM_STEP_STAGE_GROUP_LM_FLOW);
		/* fall through */
	case GROUP_LM_STEP_STAGE_GROUP_LM_FLOW:
		al_dbg("%s: entered GROUP_LM_STEP_STAGE_GROUP_LM_FLOW. serdes group:%d\n",
			__func__, serdes_grp);

		/* we check if lm_pause is true, in order to not check workarounds
		 * for this link in GROUP_LM_STEP_STAGE_GROUP_LM_FLOW. It's ok to skip group_flow
		 * in this case, since lm_pause will be TRUE for all links (it is a shared value)
		 */
		lm_pause_status = al_eth_lm_pause_check(lm_context);
		if (lm_pause_status == AL_TRUE) {
			rc = 0;
			goto out;
		}

		if (group_lm_context->common_params.serdes_grp_obj->type_get() ==
			AL_SRDS_TYPE_25G) {
			lock = group_lm_context->common_params.try_lock_cb(
				serdes_grp);

			al_dbg("%s:link %d of serdes %d after try_lock. lock: %d - step_stage: %s\n",
				__func__, link_id, serdes_grp, lock,
				al_eth_group_lm_step_stage_convert_to_str(*lm_step_stage));

			if (lock == 0)
				break;

			al_eth_group_lm_check_workarounds(group_lm_context,
							  link_id);

			if (group_lm_context->link_params[link_id].skip_group_flow == AL_FALSE)
				al_eth_group_lm_group_flow(group_lm_context);

			group_lm_context->common_params.unlock_cb(
				serdes_grp);

			al_dbg("%s:link %d of serdes %d released group lock - step_stage: %s\n",
				__func__, link_id, serdes_grp,
				al_eth_group_lm_step_stage_convert_to_str(*lm_step_stage));
		}

		rc = 0;
		goto out;

	default:
		al_err("%s: Undefined step stage! (%d)\n", __func__,
				*lm_step_stage);
		goto out;
	}

	return -EINPROGRESS;

out:
	GROUP_LM_STEP_STAGE_CHANGE_STATE(*lm_step_stage, GROUP_LM_STEP_STAGE_INIT);
	al_dbg("%s: reached out with rc %d\n", __func__, rc);
	return rc;
}

void al_eth_group_lm_init(struct al_eth_group_lm_context *group_lm_context,
			  struct al_eth_group_lm_common_params *group_lm_common_params)
{
	al_assert(group_lm_common_params->serdes_grp_obj);
	al_assert(group_lm_common_params->try_lock_cb);
	al_assert(group_lm_common_params->unlock_cb);
	al_assert(group_lm_common_params->get_msec);

	group_lm_context->common_params = *group_lm_common_params;

	group_lm_context->serdes_mode = AL_SERDES_MODE_UNKNOWN;

	al_memset(&group_lm_context->link, 0,
		sizeof(struct al_eth_group_lm_link[MAX_NUM_OF_LINKS]));
}

int al_eth_group_lm_skip_group_flow_enable(
	struct al_eth_group_lm_context *group_lm_context,
	unsigned int link_id, al_bool skip_group_flow_en)
{
	GROUP_LM_LINK_ID_CHECK(group_lm_context, link_id);

	if (group_lm_context->link[link_id].registered != AL_TRUE) {
		al_err("%s: Trying to disable group_flow for unregistered link %d\n", __func__,
		       link_id);
		return -EINVAL;
	}

	group_lm_context->link_params[link_id].skip_group_flow = skip_group_flow_en;

	return 0;
}

int al_eth_group_lm_link_manage_is_done(struct al_eth_group_lm_context *group_lm_context,
				unsigned int link_id, al_bool *is_done)
{
	GROUP_LM_LINK_ID_CHECK(group_lm_context, link_id);
	al_assert(is_done);

	if (group_lm_context->link[link_id].registered != AL_TRUE) {
		al_err("%s: Trying to check if link is stable for unregistered link %d\n", __func__,
		       link_id);
		return -EINVAL;
	}

	*is_done = (group_lm_context->link[link_id].lm_step_stage == GROUP_LM_STEP_STAGE_INIT);

	return 0;
}
