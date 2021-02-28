/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#ifndef __AL_ETH_LM_RETIMER_INTERNAL_H__
#define __AL_ETH_LM_RETIMER_INTERNAL_H__

#include "al_mod_hal_common.h"
#include "al_mod_eth_lm_retimer.h"

/*******************************************************************************
 ** Internal Macros
 ******************************************************************************/
#define retimer_debug(...)			\
	do {					\
		if (handle->debug)		\
			al_mod_warn(__VA_ARGS__);	\
		else				\
			al_mod_dbg(__VA_ARGS__);	\
	} while (AL_FALSE)

/*******************************************************************************
 ** Internal DA cable configuration
 ******************************************************************************/
/**
 * Internal parameters used for DA cable configuration
 */
struct al_mod_eth_lm_retimer_da_config {
	uint32_t eq_boost;
	uint32_t de_emphasis;
	uint32_t vod;
};

#define AL_ETH_RETIMER_DA_LENS_MAX		5

/*******************************************************************************
 ** Internal API Functions
 ******************************************************************************/

/**
 * Ethernet Link Management BR210 Retimer handle initialization
 *
 * @param handle
 *	Ethernet Link Management Retimer handle
 * @param params
 *	Ethernet Link Management Retimer parameters
 *
 * @return
 *	0 on success, errno otherwise
 */
int al_mod_eth_lm_retimer_br210_handle_init(
	struct al_mod_eth_lm_retimer *handle,
	const struct al_mod_eth_lm_retimer_params *params);

/**
 * Ethernet Link Management BR410 Retimer handle initialization
 *
 * @param handle
 *	Ethernet Link Management Retimer handle
 * @param params
 *	Ethernet Link Management Retimer parameters
 *
 * @return
 *	0 on success, errno otherwise
 */
int al_mod_eth_lm_retimer_br410_handle_init(
	struct al_mod_eth_lm_retimer *handle,
	const struct al_mod_eth_lm_retimer_params *params);

/**
 * Ethernet Link Management DS25 Retimer handle initialization
 *
 * @param handle
 *	Ethernet Link Management Retimer handle
 * @param params
 *	Ethernet Link Management Retimer parameters
 *
 * @return
 *	0 on success, errno otherwise
 */
int al_mod_eth_lm_retimer_ds25_handle_init(
	struct al_mod_eth_lm_retimer *handle,
	const struct al_mod_eth_lm_retimer_params *params);

#endif /* __AL_ETH_LM_RETIMER_INTERNAL_H__ */
