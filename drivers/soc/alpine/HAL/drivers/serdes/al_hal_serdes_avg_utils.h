/*
 * Copyright 2016, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

/**
 *  @{
 * @file   al_hal_serdes_avg_utils.h
 *
 * @brief Avago SerDes utilities
 *
 */

#ifndef __AL_HAL_SERDES_AVG_UTIS_H__
#define __AL_HAL_SERDES_AVG_UTIS_H__

#include "al_hal_serdes_avg.h"

#ifdef __cplusplus
extern "C" {
#endif

unsigned int al_serdes_avg_sbus_addr_serdes(
	enum al_serdes_complex_type	complex_type,
	unsigned int			lane_idx);

unsigned int ratio_field_val_get(
	enum al_serdes_clk_freq	ref_clk_freq,
	enum al_serdes_bit_rate	bit_rate);

unsigned int width_field_val_get(
	enum al_serdes_bit_width	bit_width);

al_bool al_serdes_avg_signal_loss_sticky_get(
	struct al_serdes_grp_obj	*obj,
	al_bool				clear);

al_bool al_serdes_avg_bist_err_sticky_get(
	struct al_serdes_grp_obj	*obj,
	al_bool				clear);

int al_serdes_avg_wait_for_o_tx_ready(
	struct al_serdes_grp_obj	*obj);

int al_serdes_avg_wait_for_o_rx_ready(
	struct al_serdes_grp_obj	*obj);

int al_serdes_avg_rx_tx_reset(
	struct al_serdes_grp_obj	*obj,
	al_bool is_eth);

al_bool al_serdes_avg_rx_tx_reset_is_done(
	struct al_serdes_grp_obj	*obj);
#ifdef __cplusplus
}
#endif

#endif

/** @} */
