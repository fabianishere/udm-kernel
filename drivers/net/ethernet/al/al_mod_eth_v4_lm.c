/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#include "al_mod_hal_common.h"
#include "al_mod_eth_v4_lm.h"

/** AVG SerDes used for Ethernet v4 which have only lane 0 per group  */
#define SERDES_LANE				0

#define SFP_PRESENT				0
#define SFP_NOT_PRESENT				1
#define SFP_PRESENCE_DEBOUNCE_COUNT		9
#define SFP_PRESENCE_DEBOUNCE_DELAY		1 /* uSec */

#define IS_ETH_ADV(handle)			((handle)->dev_id == AL_ETH_DEV_ID_ADVANCED && \
						 (handle)->rev_id == AL_ETH_REV_ID_4)
#define IS_ETH_STD(handle)			((handle)->dev_id == AL_ETH_DEV_ID_STANDARD && \
						 (handle)->rev_id == AL_ETH_REV_ID_2)

#define NUM_OF_LINK_DOWN_RETRY			5
#define LINK_DOWN_TIMEOUT			2000
/* SerDes RX/TX reset is done will take up to 1sec */
#define SERDES_RX_TX_RESET_IS_DONE_TIMEOUT	1000
/* Retimer RX_EQ_DONE usually takes ~2 sec */
#define RETIMER_RX_EQ_DONE_TIMEOUT		4000
/* Signal reset timeout - wait for rx/tx ready status */
#define SERDES_RESET_TIMEOUT			1000
/* Signal detection timeout - suppose to be immediately */
#define SIGNAL_DETECTION_TIMEOUT		100
/* CDR lock timeout - suppose to be immediately (once there is signal) */
#define CDR_LOCK_TIMEOUT			100
/* SERDES ICAL START - If timeout ended, apperentaly PCAL disable failed */
#define ICAL_START_TIMEOUT			500
/* SERDES ICAL DONE - By spec should take 500ms max */
#define ICAL_DONE_TIMEOUT			500

/**
 * Generic LM prints
 */
#define LM_PRI(format, ...)	\
	"LM [Port %u] " format, handle->port_id, ##__VA_ARGS__

#define lm_debug(format, ...)					\
	do {							\
		if (handle->debug)				\
			al_mod_warn(LM_PRI(format, ##__VA_ARGS__));	\
		else						\
			al_mod_dbg(LM_PRI(format, ##__VA_ARGS__));	\
	} while (0)

#define lm_err(format, ...)					\
	al_mod_err(LM_PRI(format, ##__VA_ARGS__));

/**
 * Retimer prints
 */
#define LM_RETIMER_PRI(idx, format, ...)			\
	"[Retimer %u] " format, (idx), ##__VA_ARGS__

#define lm_retimer_debug(idx, format, ...)			\
	lm_debug(LM_RETIMER_PRI(idx, format, ##__VA_ARGS__))

#define lm_retimer_err(idx, format, ...)				\
	lm_err(LM_RETIMER_PRI(idx, format, ##__VA_ARGS__))

/**
 * SerDes lane prints
 */
#define LM_SERDES_PRI(lane, format, ...)			\
	"[SerDes %u] " format, (lane), ##__VA_ARGS__

#define lm_serdes_debug(lane, format, ...)			\
	lm_debug(LM_SERDES_PRI(lane, format, ##__VA_ARGS__))

#define lm_serdes_err(lane, format, ...)			\
	lm_err(LM_SERDES_PRI(lane, format, ##__VA_ARGS__))

/**
 * MAC prints
 */
#define LM_MAC_PRI(mac, format, ...)				\
	"[MAC %u] " format, (mac), ##__VA_ARGS__

#define lm_mac_debug(mac, format, ...)				\
	lm_debug(LM_MAC_PRI(mac, format, ##__VA_ARGS__))

#define lm_mac_err(mac, format, ...)				\
	lm_err(LM_MAC_PRI(mac, format, ##__VA_ARGS__))

/**
 * Useful Macros
 */
#define VISIT_REQ_SET(state)					\
		(handle->state_visit_req |= AL_BIT_64(state))	\

#define VISIT_REQ_CLR(state)					\
		(handle->state_visit_req &= ~AL_BIT_64(state))	\

#define VISIT_REQ_IS_EN(state)	(!!(handle->state_visit_req & AL_BIT_64(state)))

#define VISIT_REQ_INIT()	(handle->state_visit_req = ~0)

/*******************************************************************************
 ** States Handling Functions - Forward declaration
 ******************************************************************************/
static int state_func_wait_mode_detection(struct al_mod_eth_v4_lm *handle);
static int state_func_sfp_presence(struct al_mod_eth_v4_lm *handle);
static int state_func_mac_pcs_config(struct al_mod_eth_v4_lm *handle);
static int state_func_retimer_rx_config(struct al_mod_eth_v4_lm *handle);
static int state_func_retimer_tx_config(struct al_mod_eth_v4_lm *handle);
static int state_func_retimer_rx_cdr_reset(struct al_mod_eth_v4_lm *handle);
static int state_func_retimer_tx_cdr_reset(struct al_mod_eth_v4_lm *handle);
static int state_func_retimer_signal_detect(struct al_mod_eth_v4_lm *handle);
static int state_func_retimer_cdr_lock(struct al_mod_eth_v4_lm *handle);
static int state_func_serdes_rx_tx_reset(struct al_mod_eth_v4_lm *handle);
static int state_func_serdes_rx_tx_reset_is_done(struct al_mod_eth_v4_lm *handle);
static int state_func_retimer_rx_eq_done(struct al_mod_eth_v4_lm *handle);
static int state_func_serdes_signal_detect(struct al_mod_eth_v4_lm *handle);
static int state_func_serdes_ical_start(struct al_mod_eth_v4_lm *handle);
static int state_func_serdes_ical_is_done(struct al_mod_eth_v4_lm *handle);
static int state_func_serdes_cdr_lock(struct al_mod_eth_v4_lm *handle);
static int state_func_serdes_pcal_adaptive_start(struct al_mod_eth_v4_lm *handle);
static int state_func_link_check(struct al_mod_eth_v4_lm *handle);
static int state_func_link_is_down(struct al_mod_eth_v4_lm *handle);

/*******************************************************************************
 ** States
 ******************************************************************************/
enum al_mod_eth_v4_lm_state {
	AL_ETH_V4_LM_STATE_INVALID,
	/** Helper for initial state */
	AL_ETH_V4_LM_STATE_INITIAL,
	/**
	 * Wait mode detection
	 *
	 * Move to SFP presence check when mode is valid.
	 */
	AL_ETH_V4_LM_STATE_WAIT_MODE_DETECTION = AL_ETH_V4_LM_STATE_INITIAL,
	/**
	 * SFP presence
	 *
	 * Waits until SFP (or SFPs) are present with debounce awareness.
	 * When done moves to MAC & PCS configuration.
	 */
	AL_ETH_V4_LM_STATE_SFP_PRESENCE,
	/**
	 * MAC & PCS configuration
	 *
	 * Sets MAC mode and FEC type
	 *
	 * When done move to Retimer RX channels configuration
	 */
	AL_ETH_V4_LM_STATE_MAC_PCS_CONFIG,
	/**
	 * Retimer RX channels configuration
	 *
	 * When done move to Retimer TX channels configuration
	 */
	AL_ETH_V4_LM_STATE_RETIMER_RX_CONFIG,
	/**
	 * Retimer RX channels configuration
	 *
	 * When done move to Retimer RX CDR reset
	 */
	AL_ETH_V4_LM_STATE_RETIMER_TX_CONFIG,
	/**
	 * Retimer RX CDR reset
	 *
	 * When done move to Retimer TX CDR reset
	 */
	AL_ETH_V4_LM_STATE_RETIMER_RX_CDR_RESET,
	/**
	 * Retimer TX CDR reset
	 *
	 * When done move to Retimer signal detect
	 */
	AL_ETH_V4_LM_STATE_RETIMER_TX_CDR_RESET,
	/**
	 * Retimer signal detect
	 *
	 * When done move to Retimer CDR lock check
	 */
	AL_ETH_V4_LM_STATE_RETIMER_SIGNAL_DETECT,
	/**
	 * Retimer CDR lock check
	 *
	 * When done move to SerDes RX/TX reset
	 */
	AL_ETH_V4_LM_STATE_RETIMER_CDR_LOCK,
	/**
	 * SerDes RX/TX reset
	 *
	 * When done move to wait serdes reset for completion.
	 */
	AL_ETH_V4_LM_STATE_SERDES_RX_TX_RESET,
	/**
	 * SerDes RX/TX reset is done
	 *
	 * When done move to retimer RX_EQ wait for completion.
	 */
	AL_ETH_V4_LM_STATE_SERDES_RX_TX_RESET_IS_DONE,
	/**
	 * Retimer RX EQ adaption complete check
	 *
	 * When done move to serdes dignal detect.
	 */
	AL_ETH_V4_LM_STATE_RETIMER_RX_EQ_DONE,
	/**
	 * SerDes signal detect
	 *
	 * When done move to SerDes initial calibration start
	 */
	AL_ETH_V4_LM_STATE_SERDES_SIGNAL_DETECT,
	/**
	 * SerDes initial calibration start
	 *
	 * When done move to waiting for this calibration to be done.
	 */
	AL_ETH_V4_LM_STATE_SERDES_ICAL_START,
	/**
	 * Is SerDes initial calibration done?
	 *
	 * When done move to SerDes CDR lock check
	 */
	AL_ETH_V4_LM_STATE_SERDES_ICAL_IS_DONE,
	/**
	 * SerDes CDR lock check
	 *
	 * When done move to SerDes periodic calibration start.
	 */
	AL_ETH_V4_LM_STATE_SERDES_CDR_LOCK,
	/**
	 * SerDes periodic calibration adaptive start
	 *
	 * No need to wait for this calibration to finish.
	 *
	 * When done move to link check
	 */
	AL_ETH_V4_LM_STATE_SERDES_PCAL_ADAPTIVE_START,
	/**
	 * Link check
	 *
	 * Stay in this state while link is up.
	 * On down after was up check SPF presence.
	 * If link is down, move to link is down state
	 */
	AL_ETH_V4_LM_STATE_LINK_CHECK,
	/**
	 * Link is down
	 *
	 * Check link on loop for LINK_DOWN_TIMEOUT.
	 * On local fault, reset retimer RX+TX CDR.
	 * On remote fault, reset retimer TX CDR.
	 * If link down for more than LINK_DOWN_TIMEOUT*NUM_OF_LINK_DOWN_RETRY
	 * move to re-initial port (including retimer RX/TX CDR reset)
	 */
	AL_ETH_V4_LM_STATE_LINK_IS_DOWN,
	AL_ETH_V4_LM_STATE_MAX,
};

/**
 * ETHv4 Link Management state handling function
 *
 * @param handle
 *	ETHv4 Link Management handle
 *
 * @return 0 on success, errno otherwise
 */
typedef int (*al_mod_eth_v4_lm_state_func)(
	struct al_mod_eth_v4_lm *);

/**
 * ETHv4 Link Management state parameters
 */
struct al_mod_eth_v4_lm_state_params {
	/** State handling function */
	al_mod_eth_v4_lm_state_func		func;

	/**
	 * Failure timeout in milliseconds
	 *
	 * If 0 - not timeout
	 */
	unsigned int			timeout;

	/**
	 * State to auto-move after timeout
	 *
	 * If set to INVALID then no auto-move will happen
	 * and state's handling function expected to detect the timeout and act accordingly.
	 */
	enum al_mod_eth_v4_lm_state		timeout_auto_move_state;

	uint64_t			timeout_visit_req;
};

static const struct al_mod_eth_v4_lm_state_params lm_states[AL_ETH_V4_LM_STATE_MAX] = {
	[AL_ETH_V4_LM_STATE_WAIT_MODE_DETECTION] = {
		.func = state_func_wait_mode_detection,
	},
	[AL_ETH_V4_LM_STATE_SFP_PRESENCE] = {
		.func = state_func_sfp_presence,
	},
	[AL_ETH_V4_LM_STATE_MAC_PCS_CONFIG] = {
		.func = state_func_mac_pcs_config,
	},
	[AL_ETH_V4_LM_STATE_RETIMER_RX_CONFIG] = {
		.func = state_func_retimer_rx_config,
	},
	[AL_ETH_V4_LM_STATE_RETIMER_TX_CONFIG] = {
		.func = state_func_retimer_tx_config,
	},
	[AL_ETH_V4_LM_STATE_RETIMER_RX_CDR_RESET] = {
		.func = state_func_retimer_rx_cdr_reset,
	},
	[AL_ETH_V4_LM_STATE_RETIMER_TX_CDR_RESET] = {
		.func = state_func_retimer_tx_cdr_reset,
	},
	[AL_ETH_V4_LM_STATE_RETIMER_SIGNAL_DETECT] = {
		.func = state_func_retimer_signal_detect,
		/** TODO: define right timeout */
		.timeout = SIGNAL_DETECTION_TIMEOUT,
	},
	[AL_ETH_V4_LM_STATE_RETIMER_CDR_LOCK] = {
		.func = state_func_retimer_cdr_lock,
		/** TODO: define right timeout */
		.timeout = CDR_LOCK_TIMEOUT,
		.timeout_auto_move_state = AL_ETH_V4_LM_STATE_WAIT_MODE_DETECTION,
		.timeout_visit_req =
			AL_BIT_64(AL_ETH_V4_LM_STATE_RETIMER_RX_CDR_RESET) |
			AL_BIT_64(AL_ETH_V4_LM_STATE_RETIMER_TX_CDR_RESET),
	},
	[AL_ETH_V4_LM_STATE_SERDES_RX_TX_RESET] = {
		.func = state_func_serdes_rx_tx_reset,
		.timeout = SERDES_RESET_TIMEOUT,
		.timeout_auto_move_state = AL_ETH_V4_LM_STATE_WAIT_MODE_DETECTION,
		.timeout_visit_req =
			AL_BIT_64(AL_ETH_V4_LM_STATE_RETIMER_RX_CDR_RESET) |
			AL_BIT_64(AL_ETH_V4_LM_STATE_RETIMER_TX_CDR_RESET),
	},
	[AL_ETH_V4_LM_STATE_SERDES_RX_TX_RESET_IS_DONE] = {
		.func = state_func_serdes_rx_tx_reset_is_done,
		.timeout = SERDES_RX_TX_RESET_IS_DONE_TIMEOUT,
		.timeout_auto_move_state = AL_ETH_V4_LM_STATE_WAIT_MODE_DETECTION,
	},
	[AL_ETH_V4_LM_STATE_RETIMER_RX_EQ_DONE] = {
		.func = state_func_retimer_rx_eq_done,
		.timeout = RETIMER_RX_EQ_DONE_TIMEOUT,
		.timeout_auto_move_state = AL_ETH_V4_LM_STATE_WAIT_MODE_DETECTION,
		.timeout_visit_req =
			AL_BIT_64(AL_ETH_V4_LM_STATE_RETIMER_RX_CDR_RESET) |
			AL_BIT_64(AL_ETH_V4_LM_STATE_RETIMER_TX_CDR_RESET),
	},
	[AL_ETH_V4_LM_STATE_SERDES_SIGNAL_DETECT] = {
		.func = state_func_serdes_signal_detect,
		/** TODO: define right timeout */
		.timeout = SIGNAL_DETECTION_TIMEOUT,
		.timeout_auto_move_state = AL_ETH_V4_LM_STATE_WAIT_MODE_DETECTION,
		.timeout_visit_req =
			AL_BIT_64(AL_ETH_V4_LM_STATE_RETIMER_RX_CDR_RESET) |
			AL_BIT_64(AL_ETH_V4_LM_STATE_RETIMER_TX_CDR_RESET) |
			AL_BIT_64(AL_ETH_V4_LM_STATE_SERDES_RX_TX_RESET),
	},
	[AL_ETH_V4_LM_STATE_SERDES_ICAL_START] = {
		.func = state_func_serdes_ical_start,
		.timeout = ICAL_START_TIMEOUT,
		.timeout_auto_move_state = AL_ETH_V4_LM_STATE_WAIT_MODE_DETECTION,
		.timeout_visit_req = AL_BIT_64(AL_ETH_V4_LM_STATE_SERDES_RX_TX_RESET),
	},
	[AL_ETH_V4_LM_STATE_SERDES_ICAL_IS_DONE] = {
		.func = state_func_serdes_ical_is_done,
		/** TODO: define right timeout */
		.timeout = ICAL_DONE_TIMEOUT,
		.timeout_auto_move_state = AL_ETH_V4_LM_STATE_WAIT_MODE_DETECTION,
		.timeout_visit_req = AL_BIT_64(AL_ETH_V4_LM_STATE_SERDES_RX_TX_RESET),
	},
	[AL_ETH_V4_LM_STATE_SERDES_CDR_LOCK] = {
		.func = state_func_serdes_cdr_lock,
		/** TODO: define right timeout */
		.timeout = CDR_LOCK_TIMEOUT,
		.timeout_auto_move_state = AL_ETH_V4_LM_STATE_WAIT_MODE_DETECTION,
		.timeout_visit_req =
			AL_BIT_64(AL_ETH_V4_LM_STATE_RETIMER_RX_CDR_RESET) |
			AL_BIT_64(AL_ETH_V4_LM_STATE_RETIMER_TX_CDR_RESET) |
			AL_BIT_64(AL_ETH_V4_LM_STATE_SERDES_RX_TX_RESET),
	},
	[AL_ETH_V4_LM_STATE_SERDES_PCAL_ADAPTIVE_START] = {
		.func = state_func_serdes_pcal_adaptive_start,
	},
	[AL_ETH_V4_LM_STATE_LINK_CHECK] = {
		.func = state_func_link_check,
	},
	[AL_ETH_V4_LM_STATE_LINK_IS_DOWN] = {
		.func = state_func_link_is_down,
		.timeout = LINK_DOWN_TIMEOUT,
	},
};

static const char *state_to_str(enum al_mod_eth_v4_lm_state state)
{
	switch (state) {
	case AL_ETH_V4_LM_STATE_INVALID:
		return "Invalid";
	case AL_ETH_V4_LM_STATE_WAIT_MODE_DETECTION:
		return "Wait mode detection";
	case AL_ETH_V4_LM_STATE_SFP_PRESENCE:
		return "SFP presence";
	case AL_ETH_V4_LM_STATE_MAC_PCS_CONFIG:
		return "MAC & PCS config";
	case AL_ETH_V4_LM_STATE_RETIMER_RX_CONFIG:
		return "Retimer RX config";
	case AL_ETH_V4_LM_STATE_RETIMER_TX_CONFIG:
		return "Retimer TX config";
	case AL_ETH_V4_LM_STATE_RETIMER_RX_CDR_RESET:
		return "Retimer RX CDR reset";
	case AL_ETH_V4_LM_STATE_RETIMER_TX_CDR_RESET:
		return "Retimer TX CDR reset";
	case AL_ETH_V4_LM_STATE_RETIMER_SIGNAL_DETECT:
		return "Retimer signal detect";
	case AL_ETH_V4_LM_STATE_RETIMER_CDR_LOCK:
		return "Retimer CDR lock";
	case AL_ETH_V4_LM_STATE_SERDES_RX_TX_RESET:
		return "SerDes RX/TX reset start";
	case AL_ETH_V4_LM_STATE_SERDES_RX_TX_RESET_IS_DONE:
		return "SerDes RX/TX reset start is done";
	case AL_ETH_V4_LM_STATE_RETIMER_RX_EQ_DONE:
		return "Retimer rx_eq is done?";
	case AL_ETH_V4_LM_STATE_SERDES_SIGNAL_DETECT:
		return "SerDes signal detect";
	case AL_ETH_V4_LM_STATE_SERDES_ICAL_START:
		return "SerDes iCal start";
	case AL_ETH_V4_LM_STATE_SERDES_ICAL_IS_DONE:
		return "SerDes iCal is done?";
	case AL_ETH_V4_LM_STATE_SERDES_CDR_LOCK:
		return "SerDes CDR lock";
	case AL_ETH_V4_LM_STATE_SERDES_PCAL_ADAPTIVE_START:
		return "SerDes adaptive pCal start";
	case AL_ETH_V4_LM_STATE_LINK_CHECK:
		return "Link check";
	case AL_ETH_V4_LM_STATE_LINK_IS_DOWN:
		return "Link is down";
	default:
		break;
	}

	return "N/A";
}

static void state_set(struct al_mod_eth_v4_lm *handle, enum al_mod_eth_v4_lm_state state)
{
	if ((enum al_mod_eth_v4_lm_state)handle->state != state)
		lm_debug("States change '%s' -> '%s'\n",
			state_to_str(handle->state),
			state_to_str(state));
	else
		lm_debug("Re-entering state '%s'\n",
			state_to_str(handle->state));

	VISIT_REQ_CLR(handle->state);
	handle->state = state;
	handle->state_start_time_set = AL_FALSE;
	handle->state_counter = 0;
}

static uint64_t msec_to_time(struct al_mod_eth_v4_lm *handle, uint64_t msec)
{
	switch (handle->time_units) {
	case AL_ETH_V4_LM_TIME_UNITS_MSEC:
		return msec;
	case AL_ETH_V4_LM_TIME_UNITS_USEC:
		return msec * 1000;
	default:
		al_mod_assert_msg(AL_FALSE, "Unknown time unit\n");
		break;
	}

	return 0;
}

static al_mod_bool state_is_timeout(struct al_mod_eth_v4_lm *handle)
{
	uint64_t current_time = handle->time_get(handle->time_context);
	uint64_t timeout = msec_to_time(handle, lm_states[handle->state].timeout);

	if ((handle->state_start_time_set) &&
		(timeout > 0) &&
		(timeout < (current_time - handle->state_start_time)))
		return AL_TRUE;

	return AL_FALSE;
}

/*******************************************************************************
 ** Helper functions
 ******************************************************************************/

static unsigned int macs_num_get(struct al_mod_eth_v4_lm *handle)
{
	enum al_mod_eth_mac_mode mac_mode = handle->status.detected_mode.mac_mode;
	unsigned int mac_num = 1;

	switch (mac_mode) {
	case AL_ETH_MAC_MODE_10GbE_Serial:
		break;
	case AL_ETH_MAC_MODE_XLG_LL_25G:
		if (handle->status.mode_detection_ongoing)
			mac_num = 4;
		break;
	case AL_ETH_MAC_MODE_CG_100G:
		break;
	default:
		al_mod_assert_msg(AL_FALSE, "%s: Unsupported Ethernet MAC Mode: %s\n",
			__func__, al_mod_eth_mac_mode_str(mac_mode));
		return 0;
	}

	return mac_num;
}

static unsigned int lanes_num_get(struct al_mod_eth_v4_lm *handle)
{
	enum al_mod_eth_mac_mode mac_mode = handle->status.detected_mode.mac_mode;
	unsigned int lanes_num = 1;

	switch (mac_mode) {
	case AL_ETH_MAC_MODE_10GbE_Serial:
		break;
	case AL_ETH_MAC_MODE_XLG_LL_25G:
		if (handle->status.mode_detection_ongoing)
			lanes_num = 4;
		break;
	case AL_ETH_MAC_MODE_CG_100G:
		lanes_num = 4;
		break;
	default:
		al_mod_assert_msg(AL_FALSE, "%s: Unsupported Ethernet MAC Mode: %s\n",
			__func__, al_mod_eth_mac_mode_str(mac_mode));
		return 0;
	}

	return lanes_num;
}

/**
 * SFP presence check *without* debounce awareness
 * Moves to SFP presence state if SFP is not detected
 *
 * This functions intended for a quick and dirty SFP check to make sure we are not
 * stuck in states that fail due to not connected SFP (signal detect, CDR lock, etc).
 *
 * @param handle
 *	ETHv4 Link Management handle
 */
static al_mod_bool sfp_presence_check(struct al_mod_eth_v4_lm *handle)
{
	unsigned int lanes_num = lanes_num_get(handle);
	unsigned int i;
	int ret = 0;

	for (i = 0; i < lanes_num; i++) {
		struct al_mod_eth_v4_lm_lane *lane = &handle->lanes[i];
		al_mod_bool single_present;

		if (!lane->sfp_presence_exists)
			continue;

		ret = handle->gpio_get(handle->gpio_context,
			lane->sfp_presence_gpio,
			&single_present);

		/* Normalize result */
		single_present = (single_present == SFP_PRESENT);

		if (ret || !single_present) {
			state_set(handle, AL_ETH_V4_LM_STATE_SFP_PRESENCE);

			return AL_FALSE;
		}
	}

	return AL_TRUE;
}

static void led_set(struct al_mod_eth_v4_lm *handle, al_mod_bool link_up)
{
	unsigned int macs_num = macs_num_get(handle);
	unsigned int i;

	for (i = 0; i < macs_num; i++) {
		struct al_mod_eth_mac_obj *mac_obj = handle->mac_objs[i];
		int err;

		err = mac_obj->led_set(mac_obj, link_up);
		if (err)
			lm_mac_err(i, "led_set(%u, %d) failed!\n", i, link_up);
	}
}

static int link_check(struct al_mod_eth_v4_lm *handle)
{
	unsigned int macs_num = macs_num_get(handle);
	al_mod_bool link_up_prev = handle->status.link_up;
	unsigned int i;
	int ret;

	handle->status.link_up = AL_FALSE;
	handle->status.link_remote_fault = AL_FALSE;
	handle->status.link_local_fault = AL_FALSE;

	/**
	 * This is our last state of state machine and once we achieve here
	 * we would like to return control to caller regardless of link up or down.
	 * If link is up - great, no need to stall in checking for link up over and
	 * over until time is up.
	 * If link is down - don't waste caller time by running NOW all the sequence again,
	 * wait for next round.
	 */
	handle->state_return = AL_TRUE;

	for (i = 0; i < macs_num; i++) {
		struct al_mod_eth_mac_obj *mac_obj = handle->mac_objs[i];
		struct al_mod_eth_link_status link_status;

		/*
		 * While there's no link, need to clear the status before reading it because it is
		 * sticky
		 */
		if (!link_up_prev) {
			ret = mac_obj->link_status_clear(mac_obj);
			if (ret)
				lm_mac_err(i, "Failed to clear MAC link status");
		}

		ret = mac_obj->link_status_get(mac_obj, &link_status);
		if (ret)
			lm_mac_err(i, "Failed to read MAC link status");

		lm_mac_debug(i, "link_up = %u, local_fault = %u, remote_fault = %u\n",
			link_status.link_up,
			link_status.local_fault,
			link_status.remote_fault);

		/**
		 * Link up is if at least one of the MACs shows link up - this indicates
		 * that the overall mode of operation is right, but some MACs indeed has
		 * a link down.
		 * This is due to the fact that a single port has only single MAC.
		 * If we had a port with multiple MACs then we will need to check that all the MACs
		 * has a link up to make sure that a single port is indeed up.
		 *
		 * This is can to a situation where there is a link up but also link fault
		 * indication that comes from other MACs.
		 */
		handle->status.link_up |= link_status.link_up;
		handle->status.link_local_fault |= link_status.local_fault;
		handle->status.link_remote_fault |= link_status.remote_fault;
	}

	if (handle->status.link_up) {
		if (!link_up_prev)
			led_set(handle, AL_TRUE);
	} else {
		led_set(handle, AL_FALSE);
	}

	lm_debug("[MAC Total] link_up = %u, local_fault = %u, remote_fault = %u\n",
		handle->status.link_up,
		handle->status.link_local_fault,
		handle->status.link_remote_fault);

	/**
	 * Re-enter this state (while reseting all timeout statuses) as long as link is up
	 */
	if (handle->status.link_up) {
		if (handle->status.mode_detection_ongoing) {
			lm_debug("Detected mode is %s\n",
				al_mod_eth_mac_mode_str(handle->status.detected_mode.mac_mode));

			handle->status.mode_detection_ongoing = AL_FALSE;
			handle->status.detected_mode.detected = AL_TRUE;
		}

		state_set(handle, AL_ETH_V4_LM_STATE_LINK_CHECK);
		handle->link_down_counter = 0;

		/**
		 * TODO:
		 * After link is up we should go into state that will check that pCal has finished.
		 * After it finished we should initiate adaptive pCal.
		 * While doing all that we should keep checking link at every stage,
		 * as waiting for pCal to finish or starting adaptive pCal when link is actually
		 * down is useless.
		 */

		return AL_TRUE;
	}

	return AL_FALSE;

}

/*******************************************************************************
 ** States Handling Functions
 ******************************************************************************/

static int state_func_wait_mode_detection(struct al_mod_eth_v4_lm *handle)
{
	static const enum al_mod_eth_mac_mode std_mac_modes[] = {
		AL_ETH_MAC_MODE_10GbE_Serial
	};
	static const enum al_mod_eth_mac_mode adv_mac_modes[] = {
		AL_ETH_MAC_MODE_XLG_LL_25G,
		AL_ETH_MAC_MODE_CG_100G
	};

	if (IS_ETH_ADV(handle) &&
		handle->port_id != AL_ETH_V4_LM_ADV_MAIN_PORT &&
		!handle->status.detected_mode.detected)
		return 0;

	/**
	 * If not detected - choose MAC mode and try it.
	 * Advance MAC mode detect stage, so if we won't succeed next time we will
	 * try other MAC mode.
	 * This block shouldn't be reached by Advanced Ethernet port 1-3.
	 */
	if (handle->status.mode_detection_ongoing && !handle->status.detected_mode.detected) {
		enum al_mod_eth_mac_mode mac_mode;
		unsigned int modes_count;

		if (IS_ETH_STD(handle)) {
			mac_mode = std_mac_modes[handle->mac_mode_detect_stage];
			modes_count = AL_ARR_SIZE(std_mac_modes);
		} else {
			mac_mode = adv_mac_modes[handle->mac_mode_detect_stage];
			modes_count = AL_ARR_SIZE(adv_mac_modes);
		}

		handle->mac_mode_detect_stage++;
		handle->mac_mode_detect_stage %= modes_count;

		handle->status.detected_mode.mac_mode = mac_mode;

		/* Enable all LM state due to mode change */
		VISIT_REQ_INIT();

		lm_debug("Trying mode %s\n",
			al_mod_eth_mac_mode_str(handle->status.detected_mode.mac_mode));
	}

	if (IS_ETH_ADV(handle)) {
		switch (handle->status.detected_mode.mac_mode) {
		case AL_ETH_MAC_MODE_XLG_LL_25G:
			break;
		case AL_ETH_MAC_MODE_CG_100G:
			/* Only port 0 is relevant, all others will wait forever */
			if (handle->port_id > 0)
				return 0;
			break;
		default:
			lm_err("Unsupported Ethernet MAC Mode: %s\n",
				al_mod_eth_mac_mode_str(handle->status.detected_mode.mac_mode));
			al_mod_assert(AL_FALSE);
			return 0;
		}
	} else {
		switch (handle->status.detected_mode.mac_mode) {
		case AL_ETH_MAC_MODE_10GbE_Serial:
			break;
		default:
			lm_err("Unsupported Ethernet MAC Mode: %s\n",
				al_mod_eth_mac_mode_str(handle->status.detected_mode.mac_mode));
			al_mod_assert(AL_FALSE);
			return 0;
		}
	}

	led_set(handle, AL_FALSE);

	state_set(handle, AL_ETH_V4_LM_STATE_SFP_PRESENCE);

	return 0;
}

static int state_func_sfp_presence(struct al_mod_eth_v4_lm *handle)
{
	al_mod_bool total_present_ready = AL_TRUE;
	al_mod_bool total_present = AL_TRUE;
	al_mod_bool bounce = AL_FALSE;
	unsigned int lanes_num = lanes_num_get(handle);
	unsigned int i;
	int ret = 0;

	for (i = 0; i < lanes_num; i++) {
		struct al_mod_eth_v4_lm_lane *lane = &handle->lanes[i];
		al_mod_bool single_present;

		if (!lane->sfp_presence_exists)
			continue;

		ret = handle->gpio_get(handle->gpio_context,
			lane->sfp_presence_gpio,
			&single_present);
		if (ret) {
			lm_err("Failed to read SFP presence GPIO %u for lane %u",
				lane->sfp_presence_gpio, i);
			bounce = AL_TRUE;
			break;
		}

		/* Normalize result */
		single_present = (single_present == SFP_PRESENT);

		if (lane->sfp_debounce_count == 0)
			lane->sfp_present = single_present;
		else if (lane->sfp_present != single_present) {
			bounce = AL_TRUE;
			break;
		}

		if (lane->sfp_debounce_count < SFP_PRESENCE_DEBOUNCE_COUNT)
			total_present_ready = AL_FALSE;
		else
			total_present &= lane->sfp_present;

		lane->sfp_debounce_count++;
	}

	if (total_present_ready && total_present)
		state_set(handle, AL_ETH_V4_LM_STATE_MAC_PCS_CONFIG);
	else
		handle->state_sleep_request = SFP_PRESENCE_DEBOUNCE_DELAY;

	/* Clear debounce counters when done debouncing process or bounce detected */
	if (total_present_ready || bounce)
		for (i = 0; i < lanes_num; i++)
			handle->lanes[i].sfp_debounce_count = 0;

	/**
	 * If the SFP is not present or bounce has been detected -
	 * return to caller and try again on next round
	 */
	if ((total_present_ready && !total_present) || bounce)
		handle->state_return = AL_TRUE;

	return ret;
}

static int state_func_mac_pcs_config(struct al_mod_eth_v4_lm *handle)
{
	enum al_mod_eth_mac_mode mac_mode = handle->status.detected_mode.mac_mode;
	al_mod_bool fec_enable;
	enum al_mod_eth_fec_type fec_type = 0;
	unsigned int macs_num = macs_num_get(handle);
	unsigned int i;
	int ret;

	if (!VISIT_REQ_IS_EN(handle->state))
		goto done;

	switch (mac_mode) {
	case AL_ETH_MAC_MODE_10GbE_Serial:
		fec_enable = AL_FALSE;
		break;
	case AL_ETH_MAC_MODE_XLG_LL_25G:
		fec_enable = AL_TRUE;
		fec_type = AL_ETH_FEC_TYPE_CLAUSE_74;
		break;
	case AL_ETH_MAC_MODE_CG_100G:
		fec_enable = AL_TRUE;
		fec_type = AL_ETH_FEC_TYPE_CLAUSE_91;
		break;
	default:
		al_mod_assert_msg(AL_FALSE, "%s: Unsupported Ethernet MAC Mode: %s\n",
			__func__, al_mod_eth_mac_mode_str(mac_mode));
		return -EINVAL;
	}

	for (i = 0; i < macs_num; i++) {
		struct al_mod_eth_mac_obj *mac_obj = handle->mac_objs[i];

		lm_mac_debug(i, "Setting MAC mode to '%s'\n",
			al_mod_eth_mac_mode_str(mac_mode));

		/**
		 * MAC config is the first MAC function to be executed,
		 * any farther functions uses the MAC mode this function sets.
		 */
		ret = mac_obj->config(mac_obj, mac_mode);
		if (ret) {
			lm_mac_err(i, "Config failed!\n");
			return ret;
		}

		if (handle->status.mode_detection_ongoing) {
			if (fec_enable)
				lm_mac_debug(i, "Enabling FEC type '%s'\n",
					al_mod_eth_fec_type_str(fec_type));

			ret = mac_obj->fec_enable(mac_obj, fec_type, fec_enable);
			if (ret) {
				lm_mac_err(i, "FEC enable failed!\n");
				return ret;
			}
		}
	}

done:
	state_set(handle, AL_ETH_V4_LM_STATE_RETIMER_RX_CONFIG);

	return 0;
}

static int state_func_retimer_rx_config(struct al_mod_eth_v4_lm *handle)
{
	unsigned int lanes_num = lanes_num_get(handle);
	unsigned int i;
	int ret;

	if (!VISIT_REQ_IS_EN(handle->state))
		goto done;

	for (i = 0; i < lanes_num; i++) {
		struct al_mod_eth_v4_lm_lane *lane = &handle->lanes[i];
		struct al_mod_eth_lm_retimer *retimer = &handle->retimers[lane->rx_retimer.idx];
		struct al_mod_eth_lm_retimer_config_params config_params = {
			.dir = AL_ETH_LM_RETIMER_CH_DIR_RX,
			.speed = AL_ETH_LM_RETIMER_SPEED_25G,
			.da_len = 0
		};

		if (retimer->type == AL_ETH_LM_RETIMER_TYPE_NONE)
			continue;

		ret = retimer->config(retimer, lane->rx_retimer.channel, &config_params);
		if (ret) {
			lm_retimer_err(i, "Retimer RX channel config failed!\n");
			return ret;
		}
	}

done:
	state_set(handle, AL_ETH_V4_LM_STATE_RETIMER_TX_CONFIG);

	return 0;
}

static int state_func_retimer_tx_config(struct al_mod_eth_v4_lm *handle)
{
	unsigned int lanes_num = lanes_num_get(handle);
	unsigned int i;
	int ret;

	if (!VISIT_REQ_IS_EN(handle->state))
		goto done;

	for (i = 0; i < lanes_num; i++) {
		struct al_mod_eth_v4_lm_lane *lane = &handle->lanes[i];
		struct al_mod_eth_lm_retimer *retimer = &handle->retimers[lane->tx_retimer.idx];
		struct al_mod_eth_lm_retimer_config_params config_params = {
			.dir = AL_ETH_LM_RETIMER_CH_DIR_TX,
			.speed = AL_ETH_LM_RETIMER_SPEED_25G,
			.da_len = 0
		};

		if (retimer->type == AL_ETH_LM_RETIMER_TYPE_NONE)
			continue;

		ret = retimer->config(retimer, lane->tx_retimer.channel, &config_params);
		if (ret) {
			lm_retimer_err(i, "Retimer TX channel config failed!\n");
			return ret;
		}
	}

done:
	state_set(handle, AL_ETH_V4_LM_STATE_RETIMER_RX_CDR_RESET);

	return 0;
}

static int state_func_retimer_rx_cdr_reset(struct al_mod_eth_v4_lm *handle)
{
	unsigned int lanes_num = lanes_num_get(handle);
	unsigned int i;
	int ret;

	if (!VISIT_REQ_IS_EN(handle->state))
		goto done;

	for (i = 0; i < lanes_num; i++) {
		struct al_mod_eth_v4_lm_lane *lane = &handle->lanes[i];
		struct al_mod_eth_lm_retimer *retimer = &handle->retimers[lane->rx_retimer.idx];

		if (retimer->type == AL_ETH_LM_RETIMER_TYPE_NONE)
			continue;

		ret = retimer->reset(retimer, lane->rx_retimer.channel);
		if (ret) {
			lm_retimer_err(i, "Retimer RX CDR reset failed!\n");
			return ret;
		}
	}

done:
	state_set(handle, AL_ETH_V4_LM_STATE_RETIMER_TX_CDR_RESET);

	return 0;
}

static int state_func_retimer_tx_cdr_reset(struct al_mod_eth_v4_lm *handle)
{
	unsigned int lanes_num = lanes_num_get(handle);
	unsigned int i;
	int ret;

	if (!VISIT_REQ_IS_EN(handle->state))
		goto done;

	for (i = 0; i < lanes_num; i++) {
		struct al_mod_eth_v4_lm_lane *lane = &handle->lanes[i];
		struct al_mod_eth_lm_retimer *retimer = &handle->retimers[lane->tx_retimer.idx];

		if (retimer->type == AL_ETH_LM_RETIMER_TYPE_NONE)
			continue;

		ret = retimer->reset(retimer, lane->tx_retimer.channel);
		if (ret) {
			lm_retimer_err(i, "Retimer TX CDR reset failed!\n");
			return ret;
		}
	}

done:
	state_set(handle, AL_ETH_V4_LM_STATE_RETIMER_SIGNAL_DETECT);

	return 0;
}

static int state_func_retimer_signal_detect(struct al_mod_eth_v4_lm *handle)
{
	unsigned int lanes_num = lanes_num_get(handle);
	unsigned int i;
	int ret;

	for (i = 0; i < lanes_num; i++) {
		struct al_mod_eth_v4_lm_lane *lane = &handle->lanes[i];
		struct al_mod_eth_lm_retimer *retimer = &handle->retimers[lane->rx_retimer.idx];
		al_mod_bool detected;

		if (retimer->type == AL_ETH_LM_RETIMER_TYPE_NONE)
			continue;

		ret = retimer->signal_detect(retimer, lane->rx_retimer.channel, &detected);
		if (ret) {
			lm_retimer_err(i, "Retimer CDR signal detect check failed!\n");
			return ret;
		}

		/**
		 * We encountered a retimer lane without a signal,
		 * make sure SFP is connected if yes stay in this state.
		 */
		if (!detected) {
			sfp_presence_check(handle);
			return 0;
		}
	}

	state_set(handle, AL_ETH_V4_LM_STATE_RETIMER_CDR_LOCK);

	return 0;
}

static int state_func_retimer_cdr_lock(struct al_mod_eth_v4_lm *handle)
{
	unsigned int lanes_num = lanes_num_get(handle);
	unsigned int i;
	int ret;

	for (i = 0; i < lanes_num; i++) {
		struct al_mod_eth_v4_lm_lane *lane = &handle->lanes[i];
		struct al_mod_eth_lm_retimer *retimer = &handle->retimers[lane->rx_retimer.idx];
		al_mod_bool locked;

		if (retimer->type == AL_ETH_LM_RETIMER_TYPE_NONE)
			continue;

		ret = retimer->cdr_lock(retimer, lane->rx_retimer.channel, &locked);
		if (ret) {
			lm_retimer_err(i, "Retimer CDR lock check failed!\n");
			return ret;
		}

		/**
		 * We encountered a retimer lane without a CDR lock,
		 * make sure SFP is connected if yes stay in this state.
		 */
		if (!locked) {
			sfp_presence_check(handle);
			return 0;
		}
	}

	state_set(handle, AL_ETH_V4_LM_STATE_SERDES_RX_TX_RESET);

	return 0;
}

static int state_func_serdes_rx_tx_reset(struct al_mod_eth_v4_lm *handle)
{
	unsigned int lanes_num = lanes_num_get(handle);
	unsigned int i;
	int ret;

	if (!VISIT_REQ_IS_EN(handle->state))
		goto done;

	VISIT_REQ_SET(AL_ETH_V4_LM_STATE_SERDES_RX_TX_RESET_IS_DONE);

	for (i = 0; i < lanes_num; i++) {
		struct al_mod_eth_v4_lm_lane *lane = &handle->lanes[i];

		ret = lane->serdes_obj->rx_tx_reset(lane->serdes_obj, AL_TRUE);
		if (ret) {
			if (ret == -EAGAIN) {
				/* Serdes reset rdy is not ready yet */
				lm_serdes_debug(lane->serdes_obj->lane_in_complex,
					"SerDes RX/TX reset is not ready yet.\n");
				continue;
			}
			lm_serdes_err(lane->serdes_obj->lane_in_complex,
				"SerDes RX/TX reset failed!\n");
			return ret;
		}
	}

done:
	state_set(handle, AL_ETH_V4_LM_STATE_SERDES_RX_TX_RESET_IS_DONE);

	return 0;
}

static int state_func_serdes_rx_tx_reset_is_done(struct al_mod_eth_v4_lm *handle)
{
	unsigned int lanes_num = lanes_num_get(handle);
	unsigned int i;
	al_mod_bool ret;

	if (!VISIT_REQ_IS_EN(handle->state))
		goto done;

	for (i = 0; i < lanes_num; i++) {
		struct al_mod_eth_v4_lm_lane *lane = &handle->lanes[i];

		ret = lane->serdes_obj->rx_tx_reset_is_done(lane->serdes_obj);
		if (!ret) {
			lm_serdes_debug(lane->serdes_obj->lane_in_complex,
				"SerDes RX/TX reset is not done yet.\n");
			return ret;
		}
	}

done:
	state_set(handle, AL_ETH_V4_LM_STATE_RETIMER_RX_EQ_DONE);

	return 0;
}

static int state_func_retimer_rx_eq_done(struct al_mod_eth_v4_lm *handle)
{
	unsigned int lanes_num = lanes_num_get(handle);
	unsigned int i;
	int ret;

	for (i = 0; i < lanes_num; i++) {
		struct al_mod_eth_v4_lm_lane *lane = &handle->lanes[i];
		struct al_mod_eth_lm_retimer *retimer = &handle->retimers[lane->rx_retimer.idx];
		al_mod_bool eq_done;

		if (retimer->type == AL_ETH_LM_RETIMER_TYPE_NONE)
			continue;

		ret = retimer->rx_eq_done(retimer, lane->rx_retimer.channel, &eq_done);
		if (ret) {
			lm_retimer_err(i, "Retimer RX_EQ check failed!\n");
			return ret;
		}

		if (!eq_done) {
			sfp_presence_check(handle);
			return 0;
		}
	}

	state_set(handle, AL_ETH_V4_LM_STATE_SERDES_SIGNAL_DETECT);

	return 0;
}

static int state_func_serdes_signal_detect(struct al_mod_eth_v4_lm *handle)
{
	unsigned int lanes_num = lanes_num_get(handle);
	unsigned int i;

	for (i = 0; i < lanes_num; i++) {
		struct al_mod_eth_v4_lm_lane *lane = &handle->lanes[i];
		al_mod_bool detected;

		detected = lane->serdes_obj->signal_is_detected(lane->serdes_obj, SERDES_LANE);

		/**
		 * We encountered a SerDes lane without a signal,
		 * make sure SFP is connected if yes stay in this state.
		 */
		if (!detected) {
			sfp_presence_check(handle);
			return 0;
		}
	}

	state_set(handle, AL_ETH_V4_LM_STATE_SERDES_ICAL_START);

	return 0;
}
static int state_func_serdes_ical_start(struct al_mod_eth_v4_lm *handle)
{
	unsigned int lanes_num = lanes_num_get(handle);
	unsigned int i;
	int ret;

	for (i = 0; i < lanes_num; i++) {
		struct al_mod_eth_v4_lm_lane *lane = &handle->lanes[i];

		ret = lane->serdes_obj->ical_start(lane->serdes_obj, SERDES_LANE);
		if (ret) {
			lm_serdes_err(lane->serdes_obj->lane_in_complex,
				"Initial calibration start failed!\n");
			return ret;
		}
	}

	state_set(handle, AL_ETH_V4_LM_STATE_SERDES_ICAL_IS_DONE);

	return 0;
}

static int state_func_serdes_ical_is_done(struct al_mod_eth_v4_lm *handle)
{
	unsigned int lanes_num = lanes_num_get(handle);
	unsigned int i;

	for (i = 0; i < lanes_num; i++) {
		struct al_mod_eth_v4_lm_lane *lane = &handle->lanes[i];
		al_mod_bool done;

		done = lane->serdes_obj->ical_is_done(lane->serdes_obj, SERDES_LANE);

		/**
		 * We encountered a lane which didn't finish initial calibration,
		 * make sure SFP is connected if yes stay in this state.
		 */
		if (!done) {
			sfp_presence_check(handle);
			return 0;
		}
	}

	state_set(handle, AL_ETH_V4_LM_STATE_SERDES_CDR_LOCK);

	return 0;
}

static int state_func_serdes_cdr_lock(struct al_mod_eth_v4_lm *handle)
{
	unsigned int lanes_num = lanes_num_get(handle);
	unsigned int i;

	for (i = 0; i < lanes_num; i++) {
		struct al_mod_eth_v4_lm_lane *lane = &handle->lanes[i];
		al_mod_bool locked;

		locked = lane->serdes_obj->cdr_is_locked(lane->serdes_obj, SERDES_LANE);

		/**
		 * We encountered a SerDes lane without a CDR lock,
		 * make sure SFP is connected if yes stay in this state.
		 */
		if (!locked) {
			sfp_presence_check(handle);
			return 0;
		}
	}

	state_set(handle, AL_ETH_V4_LM_STATE_SERDES_PCAL_ADAPTIVE_START);

	return 0;
}

static int state_func_serdes_pcal_adaptive_start(struct al_mod_eth_v4_lm *handle)
{
	unsigned int lanes_num = lanes_num_get(handle);
	unsigned int i;
	int ret;

	for (i = 0; i < lanes_num; i++) {
		struct al_mod_eth_v4_lm_lane *lane = &handle->lanes[i];

		ret = lane->serdes_obj->pcal_adaptive_start(lane->serdes_obj, SERDES_LANE);
		if (ret) {
			lm_serdes_err(lane->serdes_obj->lane_in_complex,
				"Periodic calibration start failed!\n");
			return ret;
		}
	}

	state_set(handle, AL_ETH_V4_LM_STATE_LINK_CHECK);

	return 0;
}

static int state_func_link_check(struct al_mod_eth_v4_lm *handle)
{

	if (link_check(handle))
		return 0;

	state_set(handle, AL_ETH_V4_LM_STATE_LINK_IS_DOWN);

	return 0;

}

static int state_func_link_is_down(struct al_mod_eth_v4_lm *handle)
{

	if (link_check(handle))
		return 0;

	/**
	 * If SFP isn't connected this is the cause of failure.
	 */
	if (!sfp_presence_check(handle))
		return 0;

	/**
	 * Keep re-checking while not timeout
	 */
	if (!state_is_timeout(handle))
		return 0;

	handle->link_down_counter++;

	if ((handle->link_down_counter % NUM_OF_LINK_DOWN_RETRY) == 0) {
		VISIT_REQ_INIT();
		state_set(handle, AL_ETH_V4_LM_STATE_WAIT_MODE_DETECTION);
		return 0;
	}

	/**
	 * Re-initialize port on local fault
	 */
	if (handle->status.link_local_fault) {
		VISIT_REQ_SET(AL_ETH_V4_LM_STATE_RETIMER_TX_CDR_RESET);
		VISIT_REQ_SET(AL_ETH_V4_LM_STATE_RETIMER_RX_CDR_RESET);
		goto done;
	}

	/**
	 * Re-initialize port on remote fault (or any other reason)
	 */
	VISIT_REQ_SET(AL_ETH_V4_LM_STATE_RETIMER_TX_CDR_RESET);

done:
	state_set(handle, AL_ETH_V4_LM_STATE_MAC_PCS_CONFIG);

	return 0;
}

/*******************************************************************************
 ** API Functions
 ******************************************************************************/
int al_mod_eth_v4_lm_handle_init(
	struct al_mod_eth_v4_lm *handle,
	const struct al_mod_eth_v4_lm_params *params)
{
	unsigned int i;

	al_mod_assert(handle);
	al_mod_assert(params);
	al_mod_assert(IS_ETH_ADV(params) || IS_ETH_STD(params));
	al_mod_assert(params->mac_obj);
	al_mod_assert(params->time_get);
	al_mod_assert(params->step_max_time > 0);
	al_mod_assert(params->lanes_num > 0 &&
		params->lanes_num <= AL_ETH_V4_LM_LANE_MAX);
	al_mod_assert(params->retimers_num <= AL_ETH_V4_LM_RETIMER_MAX);

	handle->dev_id = params->dev_id;
	handle->rev_id = params->rev_id;
	handle->port_id = params->port_id;
	handle->mac_obj = params->mac_obj;
	handle->lanes_num = params->lanes_num;
	handle->gpio_context = params->gpio_context;
	handle->gpio_get = params->gpio_get;
	handle->time_context = params->time_context;
	handle->time_get = params->time_get;
	handle->time_units = params->time_units;
	handle->step_max_time = params->step_max_time;
	handle->debug = params->debug;
	handle->mac_mode_detect_stage = 0;
	handle->status.mode_detection_ongoing = (IS_ETH_STD(params) ||
		params->port_id == AL_ETH_V4_LM_ADV_MAIN_PORT);
	handle->status.detected_mode.detected = AL_FALSE;
	handle->status.link_up = AL_FALSE;
	handle->status.link_local_fault = AL_FALSE;
	handle->status.link_remote_fault = AL_FALSE;

	if (IS_ETH_ADV(params) && params->port_id == AL_ETH_V4_LM_ADV_MAIN_PORT) {
		al_mod_assert(params->lanes_num == AL_ETH_V4_LM_LANE_MAX);

		for (i = 0; i < AL_ETH_V4_LM_MAC_MAX; i++) {
			al_mod_assert(params->mac_objs[i]);

			handle->mac_objs[i] = params->mac_objs[i];
		}
	} else {
		al_mod_assert(params->lanes_num == 1);
	}

	/**
	 * If not Retimers exist initialize 1st Retimer as "NONE" type
	 */
	if (params->retimers_num) {
		handle->retimers_num = params->retimers_num;

		for (i = 0; i < handle->retimers_num; i++) {
			const struct al_mod_eth_lm_retimer_params *retimer_params =
				&params->retimers[i];
			struct al_mod_eth_lm_retimer *retimer = &handle->retimers[i];

			al_mod_eth_lm_retimer_handle_init(retimer, retimer_params);
		}
	} else {
		struct al_mod_eth_lm_retimer_params retimer_params = {
			.type = AL_ETH_LM_RETIMER_TYPE_NONE
		};

		handle->retimers_num = 1;

		al_mod_eth_lm_retimer_handle_init(&handle->retimers[0], &retimer_params);
	}

	for (i = 0; i < handle->lanes_num; i++) {
		const struct al_mod_eth_v4_lm_lane_params *lane_params = &params->lanes[i];
		struct al_mod_eth_v4_lm_lane *lane = &handle->lanes[i];

		al_mod_assert(lane_params->serdes_obj);
		al_mod_assert(lane_params->rx_retimer.idx < handle->retimers_num);
		al_mod_assert(lane_params->tx_retimer.idx < handle->retimers_num);
		al_mod_assert(!lane_params->sfp_presence_exists || params->gpio_get);

		lane->serdes_obj = lane_params->serdes_obj;
		lane->rx_retimer.idx = lane_params->rx_retimer.idx;
		lane->rx_retimer.channel = lane_params->rx_retimer.channel;
		lane->tx_retimer.idx = lane_params->tx_retimer.idx;
		lane->tx_retimer.channel = lane_params->tx_retimer.channel;
		lane->sfp_presence_exists = lane_params->sfp_presence_exists;
		lane->sfp_presence_gpio = lane_params->sfp_presence_gpio;
		lane->sfp_debounce_count = 0;
	}

	VISIT_REQ_INIT();

	handle->link_down_counter = 0;

	state_set(handle, AL_ETH_V4_LM_STATE_INITIAL);

	/* Check that state enum size is less than 64 */
	al_mod_assert(AL_ETH_V4_LM_STATE_MAX < 64);

	return 0;
}

void al_mod_eth_v4_lm_debug_mode_set(struct al_mod_eth_v4_lm *handle, al_mod_bool debug_mode)
{
	al_mod_assert(handle);

	handle->debug = debug_mode;
}

int al_mod_eth_v4_lm_mode_detection_trigger(
	struct al_mod_eth_v4_lm *handle)
{
	al_mod_assert(handle);
	al_mod_assert(IS_ETH_STD(handle) || handle->port_id == AL_ETH_V4_LM_ADV_MAIN_PORT);

	handle->mac_mode_detect_stage = 0;
	handle->status.mode_detection_ongoing = AL_TRUE;
	handle->status.detected_mode.detected = AL_FALSE;

	state_set(handle, AL_ETH_V4_LM_STATE_WAIT_MODE_DETECTION);

	return 0;
}

int al_mod_eth_v4_lm_detected_mode_set(
	struct al_mod_eth_v4_lm *handle,
	struct al_mod_eth_v4_lm_detected_mode *detected_mode)
{
	al_mod_assert(handle);
	al_mod_assert(detected_mode);

	if (detected_mode->detected == handle->status.detected_mode.detected &&
		detected_mode->mac_mode == handle->status.detected_mode.mac_mode)
		return 0;

	/**
	 * We set as if mode detection is on-going for Standard Ethernet or
	 * Advanced Ethernet port 0 so any configurations that are done during mode detection
	 * will be executed now as well.
	 * For Advanced Ethernet it is also indicates for ports 1-3 that port 0 is still not ready,
	 * and the mode for them is not yet applicable.
	 */
	handle->status.mode_detection_ongoing = (IS_ETH_STD(handle) ||
		handle->port_id == AL_ETH_V4_LM_ADV_MAIN_PORT);

	handle->status.detected_mode.detected = detected_mode->detected;
	handle->status.detected_mode.mac_mode = detected_mode->mac_mode;

	lm_debug("Set detected mode to %s\n",
		al_mod_eth_mac_mode_str(handle->status.detected_mode.mac_mode));

	state_set(handle, AL_ETH_V4_LM_STATE_WAIT_MODE_DETECTION);

	return 0;
}

static void al_mod_eth_v4_timeout_state_req_set(struct al_mod_eth_v4_lm *handle)
{
	al_mod_dbg("States request after timeout for %s\n", state_to_str(handle->state));

	handle->state_visit_req |= lm_states[handle->state].timeout_visit_req;
}

int al_mod_eth_v4_lm_step(
	struct al_mod_eth_v4_lm *handle)
{
	uint64_t current_time, end_time;
	int ret;

	al_mod_assert(handle);

	current_time = handle->time_get(handle->time_context);
	end_time = current_time + handle->step_max_time;

	do {
		enum al_mod_eth_v4_lm_state orig_state = handle->state;
		al_mod_eth_v4_lm_state_func func = lm_states[orig_state].func;

		al_mod_assert_msg(func, "Empty function for state '%s'\n", state_to_str(orig_state));

		handle->state_sleep_request = 0;
		handle->state_return = AL_FALSE;

		if (!handle->state_start_time_set) {
			handle->state_start_time = handle->time_get(handle->time_context);
			handle->state_start_time_set = AL_TRUE;
		}

		ret = func(handle);
		if (ret)
			return ret;

		if (handle->state_return)
			return 0;

		if (handle->state_sleep_request)
			al_mod_udelay(handle->state_sleep_request);

		handle->state_counter++;

		current_time = handle->time_get(handle->time_context);

		/**
		 * Auto-move to a new state on timeout on the same state
		 */
		if (((enum al_mod_eth_v4_lm_state)handle->state == orig_state) &&
			(lm_states[orig_state].timeout_auto_move_state !=
				AL_ETH_V4_LM_STATE_INVALID) &&
			state_is_timeout(handle)) {
			lm_debug("Timeout on state '%s'\n", state_to_str(handle->state));
			al_mod_eth_v4_timeout_state_req_set(handle);
			state_set(handle, lm_states[orig_state].timeout_auto_move_state);
		}
	} while (current_time < end_time);

	return 0;
}

void al_mod_eth_v4_lm_status_get(
	struct al_mod_eth_v4_lm *handle,
	struct al_mod_eth_v4_lm_status *status)
{
	al_mod_assert(handle);
	al_mod_assert(status);

	*status = handle->status;
}
