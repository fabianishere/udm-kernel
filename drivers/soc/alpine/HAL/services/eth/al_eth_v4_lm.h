/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#ifndef __AL_ETH_V4_LM_H__
#define __AL_ETH_V4_LM_H__

#include "al_hal_common.h"
#include "al_hal_eth.h"
#include "al_hal_serdes_interface.h"
#include "al_eth_lm_retimer.h"

/**
 * Ethernet v4 Link Management
 *
 * Suitable for both Standard and Advanced Ethernet v4.
 *
 * Standard Ethernet v4:
 * Each port is standalone with not interferences between the ports.
 * Supported MAC modes:
 * - 10G (KR)
 *
 * Advanced Ethernet v4:
 * Considered as a complex of 4 Ethernet ports.
 * Ports mode detection (1x100G or 2x25G) is done by port 0,
 * while in this time other ports are idle.
 * Port 0 will be initialized with pointers to all 4 SerDes lanes and to all 4 MAC objects.
 * Supported MAC modes:
 * - 25G
 * - 100G
 */

#define AL_ETH_V4_LM_PORT_MAX				4
#define AL_ETH_V4_LM_MAC_MAX				4
#define AL_ETH_V4_LM_LANE_MAX				4
#define AL_ETH_V4_LM_RETIMER_MAX			2

#define AL_ETH_V4_LM_ADV_MAIN_PORT			0

/*******************************************************************************
 ** Initialization Structures
 ******************************************************************************/

/**
 * Lane Retimer parameters
 */
struct al_eth_v4_lm_lane_retimer_params {
	/** Retimer index (referring to retimers in LM handle) */
	unsigned int			idx;
	/** Retimer channel */
	unsigned int			channel;
};

/**
 * Lane parameters
 */
struct al_eth_v4_lm_lane_params {
	/**
	 * SerDes lane index
	 *
	 * Note: This does not refer to lane index inside SerDes group object,
	 *	 which will be always 0, as each SerDes group has only single lane
	 *	 in this SerDes type.
	 *	 We can see this as SerDes group index, but with respect that each SerDes
	 *	 group has a single lane.
	 */
	unsigned int			lane_idx;

	/**
	 * SerDes object
	 *
	 * Note: SerDes lane parameter is not needed as for SerDeses used with Ethernet v4
	 *	 we have only a single lane per SerDes group.
	 */
	struct al_serdes_grp_obj	*serdes_obj;

	/** RX Retimer */
	struct al_eth_v4_lm_lane_retimer_params		rx_retimer;
	/** TX Retimer */
	struct al_eth_v4_lm_lane_retimer_params		tx_retimer;

	/**
	 * Is a dedicated SFP presence signal exists for this lane?
	 *
	 * If a single SFP exists for multiple lanes then it's advised to
	 * set this SFP only to a single lane, to avoid redundant SFP presence reads.
	 * But if SFP presence detect is needed than at least a single lane should be set
	 * with SFP presence parameters, even if this SFP presence GPIO is already set for
	 * other port.
	 */
	al_bool				sfp_presence_exists;
	/** SFP presence GPIO */
	unsigned int			sfp_presence_gpio;
};

/**
 * Ethernet v4 Link Management time units
 */
enum al_eth_v4_lm_time_units {
	/** Milliseconds */
	AL_ETH_V4_LM_TIME_UNITS_MSEC,
	/** Microseconds */
	AL_ETH_V4_LM_TIME_UNITS_USEC,
};

/**
 * Ethernet v4 Link Management handle parameters
 */
struct al_eth_v4_lm_params {
	/** Ethernet device ID */
	unsigned int			dev_id;
	/** Ethernet revision ID */
	unsigned int			rev_id;

	/**
	 * Port ID
	 *
	 * Advanced Ethernet v4 ports are enumerated from 0 to 3.
	 * Not applicable for Standard Ethernet v4 ports.
	 */
	unsigned int			port_id;

	/**
	 * MAC objects
	 *
	 * Multiple MAC objects are relevant only for port 0 and only during mode detection stage.
	 * All MAC object should be provided for port 0 of Advanced Ethernet.
	 */
	union {
		struct al_eth_mac_obj		*mac_obj;
		struct al_eth_mac_obj		*mac_objs[AL_ETH_V4_LM_MAC_MAX];
	};

	/** SerDes lanes */
	struct al_eth_v4_lm_lane_params	lanes[AL_ETH_V4_LM_LANE_MAX];
	unsigned int			lanes_num;

	/** Retimers */
	struct al_eth_lm_retimer_params retimers[AL_ETH_V4_LM_RETIMER_MAX];
	unsigned int			retimers_num;

	/** GPIO context for GPIO functions */
	void				*gpio_context;

	/**
	 * GPIO read function
	 *
	 * @param gpio_context
	 *	GPIO context
	 * @param gpio
	 *	GPIO number
	 * @param val
	 *	Value returned from GPIO on success
	 *
	 * @return
	 *	0 on success, errno otherwise
	 */
	int				(*gpio_get)(void *gpio_context,
						unsigned int gpio, al_bool *val);

	/** Time context for time functions */
	void				*time_context;

	/**
	 * Time get function
	 *
	 * @param time_context
	 *	Time context
	 *
	 * @return
	 *	Time in selected time units
	 */
	uint64_t			(*time_get)(void *time_context);

	/**
	 * Time units
	 *
	 * Time measuring function should return in this units.
	 * Also all references to time are in this units (unless state otherwise).
	 */
	enum al_eth_v4_lm_time_units	time_units;

	/** Step maximum execution time */
	unsigned int			step_max_time;

	/** Debug prints will be printed in higher level */
	al_bool				debug;
};

struct al_eth_v4_lm_detected_mode {
	/** Is mode detected? */
	al_bool				detected;
	/** Detected MAC mode */
	enum al_eth_mac_mode		mac_mode;
};

struct al_eth_v4_lm_status {
	/**
	 * Mode detection process is ongoing
	 *
	 * Applicable only for Advanced Ethernet port 0 or any Standard Ethernet port.
	 */
	al_bool				mode_detection_ongoing;

	/** Detected mode */
	struct al_eth_v4_lm_detected_mode	detected_mode;

	/** Link status - Up */
	al_bool				link_up;
	/** Link status - Local fault */
	al_bool				link_local_fault;
	/** Link status - Remote fault */
	al_bool				link_remote_fault;
};

/*******************************************************************************
 ** Internal Usage Structures
 ******************************************************************************/

struct al_eth_v4_lm_lane_retimer {
	/** Retimer index (referring to retimers in LM handle) */
	unsigned int			idx;
	/** Retimer channel */
	unsigned int			channel;
};

struct al_eth_v4_lm_lane {
	/** SerDes object */
	struct al_serdes_grp_obj	*serdes_obj;

	/** RX Retimer */
	struct al_eth_v4_lm_lane_retimer		rx_retimer;
	/** TX Retimer */
	struct al_eth_v4_lm_lane_retimer		tx_retimer;

	/**
	 * Is a dedicated SFP presence signal exists for this lane?
	 */
	al_bool				sfp_presence_exists;
	/** SFP presence GPIO */
	unsigned int			sfp_presence_gpio;

	/** SFP present */
	al_bool				sfp_present;
	/** SFP presence debounce counter */
	unsigned int			sfp_debounce_count;
};

struct al_eth_v4_lm {
	/** Ethernet device ID */
	unsigned int			dev_id;
	/** Ethernet revision ID */
	unsigned int			rev_id;

	/**
	 * Port ID
	 *
	 * Advanced Ethernet v4 ports are enumerated from 0 to 3.
	 * Not applicable for Standard Ethernet v4 ports.
	 */
	unsigned int			port_id;

	/**
	 * MAC objects
	 *
	 * Multiple MAC objects are relevant only for port 0 and only during mode detection stage.
	 * All MAC object should be provided for port 0 of Advanced Ethernet.
	 */
	union {
		struct al_eth_mac_obj		*mac_obj;
		struct al_eth_mac_obj		*mac_objs[AL_ETH_V4_LM_MAC_MAX];
	};

	/** SerDes lanes */
	struct al_eth_v4_lm_lane	lanes[AL_ETH_V4_LM_LANE_MAX];
	unsigned int			lanes_num;

	/** Retimers */
	struct al_eth_lm_retimer	retimers[AL_ETH_V4_LM_RETIMER_MAX];
	unsigned int			retimers_num;

	/** GPIO context for GPIO functions */
	void				*gpio_context;

	/**
	 * GPIO read function
	 *
	 * @param gpio_context
	 *	GPIO context
	 * @param gpio
	 *	GPIO number
	 * @param val
	 *	Value returned from GPIO on success
	 *
	 * @return
	 *	0 on success, errno otherwise
	 */
	int				(*gpio_get)(void *gpio_context,
						unsigned int gpio, al_bool *val);

	/** Time context for time functions */
	void				*time_context;

	/**
	 * Time get function
	 *
	 * @param time_context
	 *	Time context
	 *
	 * @return
	 *	Time in selected time units
	 */
	uint64_t			(*time_get)(void *time_context);

	/**
	 * Time units
	 *
	 * Time measuring function should return in this units.
	 * Also all references to time are in this units (unless state otherwise).
	 */
	enum al_eth_v4_lm_time_units	time_units;

	/** Step maximum execution time */
	unsigned int			step_max_time;

	/** Debug prints will be printed in higher level */
	al_bool				debug;

	/** Internal state machine - state */
	int				state;
	/** Internal state machine - sleep request (in uSec) */
	unsigned int			state_sleep_request;
	/** Internal state machine - indicates state machine should return control to caller */
	al_bool				state_return;
	/** Internal state machine - state start time set */
	al_bool				state_start_time_set;
	/** Internal state machine - state start time */
	uint64_t			state_start_time;
	/** Internal state machine - counts number successive entries to same state */
	unsigned int			state_counter;
	/** Internal state machine - Bitmap trigger for state execution */
	uint64_t			state_visit_req;

	/** MAC mode detection internal stage */
	int				mac_mode_detect_stage;

	/** Counter of continues link down */
	uint64_t			link_down_counter;

	/** Status to be published */
	struct al_eth_v4_lm_status	status;
};


/*******************************************************************************
 ** API Functions
 ******************************************************************************/

/**
 * Ethernet v4 Link Management handle initialization
 *
 * @param handle
 *	Ethernet v4 Link Management handle
 * @param params
 *	Ethernet v4 Link Management handle parameters
 *
 * @return
 *	0 on success, errno otherwise
 */
int al_eth_v4_lm_handle_init(
	struct al_eth_v4_lm *handle,
	const struct al_eth_v4_lm_params *params);

/**
 * Turn on / off debug prints
 *
 * @param handle
 *	Ethernet v4 Link Management handle
 * @param debug_mode
 *	desired debug mode
 *
 */
void al_eth_v4_lm_debug_mode_set(struct al_eth_v4_lm *handle, al_bool debug_mode);

/**
 * Ethernet v4 Link Management triggers mode detection process
 *
 * Applicable only for Advanced Ethernet port 0 or any Standard Ethernet port.
 *
 * @param handle
 *	Ethernet v4 Link Management handle
 *
 * @return
 *	0 on success, errno otherwise
 */
int al_eth_v4_lm_mode_detection_trigger(
	struct al_eth_v4_lm *handle);

/**
 * Ethernet v4 Link Management detected mode setter
 *
 * Setting mode to current mode will be ignored.
 *
 * @param handle
 *	Ethernet v4 Link Management handle
 * @param detected_mode
 *	Ethernet v4 detected mode
 *
 * @return
 *	0 on success, errno otherwise
 */
int al_eth_v4_lm_detected_mode_set(
	struct al_eth_v4_lm *handle,
	struct al_eth_v4_lm_detected_mode *detected_mode);

/**
 * Ethernet v4 Link Management step
 *
 * @param handle
 *	Ethernet v4 Link Management handle
 *
 * @return
 *	0 on success, errno otherwise
 */
int al_eth_v4_lm_step(
	struct al_eth_v4_lm *handle);


/**
 * Ethernet v4 Link Management status getter
 *
 * @param handle
 *	Ethernet v4 Link Management handle
 * @param status
 *	Ethernet v4 Link Management status
 */
void al_eth_v4_lm_status_get(
	struct al_eth_v4_lm *handle,
	struct al_eth_v4_lm_status *status);


#endif /* __AL_ETH_V4_LM_H__ */
