/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#include "stdlib.h"

/**
 * @defgroup group_eth_v4_lm_samples Code Samples
 * @ingroup group_eth_v4_lm
 * @{
 * eth_v4_lm.c: this file can be found in samples directory.
 *
 * Example of ETHv4 Link Management service operation
 *
 * @code */
#include "samples.h"
#include "al_eth_v4_lm.h"
#include "al_hal_iomap.h"
#include "al_hal_serdes_avg.h"

/* Maximum allowed time for LM machine to work */
#define STEP_MAX_TIME		200

enum sample_lm_mode {
	SAMPLE_LM_MODE_AUTO,
	SAMPLE_LM_MODE_25G,
	SAMPLE_LM_MODE_100G,
};

static enum sample_lm_mode sample_lm_mode = SAMPLE_LM_MODE_AUTO;

static struct al_eth_v4_lm lm_handles[AL_ETH_V4_LM_PORT_MAX];
static struct al_eth_v4_lm_status last_lm_status[AL_ETH_V4_LM_PORT_MAX];

/**
 * Common resources handles
 *
 * Same MAC and SerDes handles might be needed by LM of main port and the non-main ports.
 */
static struct al_eth_mac_obj mac_objs[AL_ETH_V4_LM_MAC_MAX];
static struct al_serdes_complex_obj serdes_complex;
static struct al_serdes_grp_obj serdes_objs[AL_ETH_V4_LM_LANE_MAX];

static uint64_t time_get(void *context __attribute__((unused)))
{
	static uint64_t time = 1;

	/** Return real time! */

	return time++;
}

/**
 * Common resources handles initialization
 */
static void common_resources_handle_init(void)
{
	struct al_eth_mac_obj_init_params mac_init_params;
	unsigned int i;

	al_memset(&mac_init_params, 0, sizeof(mac_init_params));

	/* MAC objects */
	mac_init_params.mac_common_regs = (void *)(uintptr_t)AL_ETH_COMMON_MAC_COMMON_BASE;
	mac_init_params.eth_rev_id = AL_ETH_REV_ID_4;

	for (i = 0; i < AL_ETH_V4_LM_MAC_MAX; i++) {
		mac_init_params.mac_regs_base = (void *)(uintptr_t)AL_ETH_MAC_BASE(i);
		mac_init_params.serdes_lane = i;

		al_eth_mac_handle_init(&mac_objs[i], &mac_init_params);
	}

	/* SerDes objects */
	al_serdes_avg_complex_handle_init(
		(void __iomem *)(uintptr_t)AL_SB_SRD_CMPLX_HS_BASE,
		(void __iomem *)(uintptr_t)AL_SRD_CMPLX_HS_SBUS_MASTER_BASE,
		AL_SRDS_COMPLEX_TYPE_TR_HS, NULL, NULL, &serdes_complex);

	for (i = 0; i < AL_ETH_V4_LM_LANE_MAX; i++)
		al_serdes_avg_handle_init(
			(void __iomem *)(uintptr_t)AL_SRD_CMPLX_HS_LANE_BASE(i),
			&serdes_complex, i, &serdes_objs[i]);
}

/**
 * Common resources handles initialization
 */
static void port_lm_handle_init(unsigned int port_id)
{
	struct al_eth_v4_lm_params lm_params;
	int ret;

	al_memset(&lm_params, 0, sizeof(lm_params));
	lm_params.dev_id = AL_ETH_DEV_ID_ADVANCED;
	lm_params.rev_id = AL_ETH_REV_ID_4;
	lm_params.port_id = port_id;
	lm_params.time_get = time_get;
	lm_params.step_max_time = STEP_MAX_TIME;
	lm_params.debug = AL_TRUE;

	if (port_id == AL_ETH_V4_LM_ADV_MAIN_PORT) {
		unsigned int i;

		/**
		 * Advanced Ethernet main port needs to have access to resources
		 * of non-main ports.
		 */

		for (i = 0; i < AL_ETH_V4_LM_MAC_MAX; i++)
			lm_params.mac_objs[i] = &mac_objs[i];

		for (i = 0; i < AL_ETH_V4_LM_LANE_MAX; i++)
			lm_params.lanes[i].serdes_obj = &serdes_objs[i];

		lm_params.lanes_num = AL_ETH_V4_LM_LANE_MAX;
	} else {
		/**
		 * Advanced Ethernet non-main port holds only its own MAC and SerDes lanes
		 */

		lm_params.mac_obj = &mac_objs[port_id];
		lm_params.lanes[0].serdes_obj = &serdes_objs[port_id];
		lm_params.lanes_num = 1;
	}

	ret = al_eth_v4_lm_handle_init(&lm_handles[port_id], &lm_params);
	al_assert(ret == 0);
}


/**
 * A thread-like function for a single port
 */
static void port_lm_thread(unsigned int port_id)
{
	struct al_eth_v4_lm *lm_handle = &lm_handles[port_id];
	struct al_eth_v4_lm_status status;
	unsigned int i;
	int ret;

	ret = al_eth_v4_lm_step(lm_handle);
	al_assert(ret == 0);

	al_eth_v4_lm_status_get(lm_handle, &status);

	if (last_lm_status[port_id].link_up != status.link_up)
		al_info("Port %u link changed to %s\n", port_id, (status.link_up ? "UP" : "DOWN"));


	if (port_id == AL_ETH_V4_LM_ADV_MAIN_PORT &&
		last_lm_status[port_id].mode_detection_ongoing != status.mode_detection_ongoing &&
		!status.mode_detection_ongoing) {

		al_info("Main port %u detected MAC mode is %s, "
			"setting this mode to all other ports\n",
			port_id, al_eth_mac_mode_str(status.detected_mode.mac_mode));

		for (i = 0; i < AL_ETH_V4_LM_PORT_MAX; i++) {
			if (port_id == AL_ETH_V4_LM_ADV_MAIN_PORT)
				continue;

			al_eth_v4_lm_detected_mode_set(lm_handle, &status.detected_mode);
		}
	}

	last_lm_status[port_id] = status;
}

int main(void)
{
	struct al_eth_v4_lm_detected_mode detected_mode;
	unsigned int i;
	int ret;

	al_memset(&detected_mode, 0, sizeof(detected_mode));

	common_resources_handle_init();

	for (i = 0; i < AL_ETH_V4_LM_PORT_MAX; i++)
		port_lm_handle_init(i);

	/**
	 * Setting static mode if needed.
	 * The mode is set for the main port and then propagates by main port thread
	 * to the non-main ports after the main port indicates it's ready.
	 */
	if (sample_lm_mode != SAMPLE_LM_MODE_AUTO) {
		detected_mode.detected = AL_TRUE;

		switch (sample_lm_mode) {
		case SAMPLE_LM_MODE_25G:
			detected_mode.mac_mode = AL_ETH_MAC_MODE_XLG_LL_25G;
			break;
		case SAMPLE_LM_MODE_100G:
			detected_mode.mac_mode = AL_ETH_MAC_MODE_CG_100G;
			break;
		default:
			al_assert(AL_FALSE);
			break;
		}

		ret = al_eth_v4_lm_detected_mode_set(&lm_handles[AL_ETH_V4_LM_ADV_MAIN_PORT],
			&detected_mode);
		al_assert(ret == 0);
	}

	do {
		for (i = 0; i < AL_ETH_V4_LM_PORT_MAX; i++)
			port_lm_thread(i);
	} while (AL_TRUE);

	return 0;
}

/** @endcode */

/** @} */

