/*******************************************************************************
Copyright (C) 2013 Annapurna Labs Ltd.

This file is licensed under the terms of the Annapurna Labs' Commercial License
Agreement distributed with the file or available on the software download site.
Recipient shall use the content of this file only on semiconductor devices or
systems developed by or for Annapurna Labs.

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

/**
 * @defgroup group_serdes_init SerDes Initialization
 * @ingroup group_serdes SerDes
 * @{
 *
 * @file   al_hal_serdes_hssp_init.h
 *
 */

#ifndef __AL_HAL_SERDES_INIT_H__
#define __AL_HAL_SERDES_INIT_H__

#include "al_hal_serdes_hssp.h"

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

/**
 * SERDES ICTL_PMA register value setting
 */
int al_serdes_hssp_group_ictl_pma_val_set(
	struct al_serdes_grp_obj	*grp_obj,
	uint32_t			*ictl_pma_val,
	enum al_serdes_clk_src		clk_src_r2l,
	enum al_serdes_clk_src		clk_src_l2r,
	enum al_serdes_clk_src		clk_src_core);

/**
 * SERDES ICTL_PMA register value setting for flipped SERDES (L2R <--> R2L)
 */
int al_serdes_hssp_group_ictl_pma_val_set_flp(
	struct al_serdes_grp_obj	*grp_obj,
	uint32_t			*ictl_pma_val,
	enum al_serdes_clk_src		clk_src_r2l,
	enum al_serdes_clk_src		clk_src_l2r,
	enum al_serdes_clk_src		clk_src_core);

/******************************************************************************/
/**
 * SERDES basic power up to allow taking clock from right/left
 */
void al_serdes_hssp_basic_power_up(
	struct al_serdes_grp_obj	*grp_obj,
	al_bool				r2l_pll);

/******************************************************************************/
/**
 * SERDES core configuration for inverting Tx/Rx lanes polarity
 */
void al_serdes_hssp_group_lanes_tx_rx_pol_inv(
	struct al_serdes_grp_obj	*grp_obj,
	al_bool				*inv_tx_lanes,
	al_bool				*inv_rx_lanes);

/**
 * SERDES core configuration for turning off a group non active lanes
 */
int al_serdes_hssp_group_non_active_lanes_pd(
	struct al_serdes_grp_obj	*grp_obj,
	al_bool				*active_lanes);

/**
 * SERDES core configuration for turning off a group
 */
int al_serdes_hssp_group_cfg_off(
	struct al_serdes_grp_obj	*grp_obj);

/**
 * SERDES core configuration for turning off a group, but allow it to bypass
 * L2R/R2L clocks
 */
int al_serdes_hssp_group_cfg_off_bp(
	struct al_serdes_grp_obj	*grp_obj);

/**
 * SERDES core configuration for SATA
 */
int al_serdes_hssp_group_cfg_sata_mode(
	struct al_serdes_grp_obj	*grp_obj,
	int				skip_core_init,
	uint32_t			ictl_pma_val,
	enum al_serdes_clk_freq		core_clk_freq,
	al_bool				ssc_en);

/**
 * SERDES core configuration for PCIE3
 */
int al_serdes_hssp_group_cfg_pcie3_mode(
	struct al_serdes_grp_obj	*grp_obj,
	int				skip_core_init,
	uint32_t			ictl_pma_val,
	int				inhibit_gen3,
	int				x8_lanes_4_to_7,
	al_bool				override_agc_ac_boost,
	uint8_t				override_agc_ac_boost_val);

/**
 * SERDES core configuration for KR
 */
int al_serdes_hssp_group_cfg_eth_kr_mode(
	struct al_serdes_grp_obj	*grp_obj,
	int				skip_core_init,
	uint32_t			ictl_pma_val,
	enum al_serdes_clk_freq		core_clk_freq,
	al_bool				is_16_bit_interface);

/**
 * SERDES core configuration for SGMII
 */
int al_serdes_hssp_group_cfg_eth_sgmii_mode(
	struct al_serdes_grp_obj	*grp_obj,
	int				skip_core_init,
	enum al_serdes_clk_freq		core_clk_freq,
	uint32_t			ictl_pma_val);

int al_serdes_hssp_group_cfg_eth_sgmii_2_5g_mode(
	struct al_serdes_grp_obj	*grp_obj,
	int				skip_core_init,
	enum al_serdes_clk_freq		core_clk_freq,
	uint32_t			ictl_pma_val);


/* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif

/* *INDENT-ON* */
#endif

/** @} end of SERDES group */

