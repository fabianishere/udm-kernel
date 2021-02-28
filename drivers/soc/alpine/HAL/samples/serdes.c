/*******************************************************************************
Copyright (C) 2015 Annapurna Labs Ltd.

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
#include "stdlib.h"

/**
 * @defgroup group_serdes_samples Code Samples
 * @ingroup group_serdes
 * @{
 * serdes.c: this file can be found in samples directory.
 *
 * @code */
#include "al_hal_serdes.h"

struct al_serdes_cfg cfg = {
	.grp_cfg = {
	// Group A
	{
		.mode = AL_SRDS_CFG_PCIE_A_G2X2_USB_A_USB_B,
		.clk_src_r2l = AL_SRDS_CLK_SRC_R2L,
		.clk_src_l2r = AL_SRDS_CLK_SRC_L2R,
		.clk_src_core = AL_SRDS_CLK_SRC_R2L,
		.ref_clk_freq = AL_SRDS_CLK_FREQ_NA,
		.r2l_clk_freq = AL_SRDS_CLK_FREQ_100_MHZ,
		.l2r_clk_freq = AL_SRDS_CLK_FREQ_NA,
	},
       // Group B
	{
		.mode = AL_SRDS_CFG_SATA_A,
		.clk_src_r2l = AL_SRDS_CLK_SRC_R2L,
		.clk_src_l2r = AL_SRDS_CLK_SRC_L2R,
		.clk_src_core = AL_SRDS_CLK_SRC_R2L,
		.ref_clk_freq = AL_SRDS_CLK_FREQ_NA,
		.r2l_clk_freq = AL_SRDS_CLK_FREQ_NA,
		.l2r_clk_freq = AL_SRDS_CLK_FREQ_NA,
	},
       // Group C
	{
		.mode = AL_SRDS_CFG_SATA_B,
		.clk_src_r2l = AL_SRDS_CLK_SRC_R2L,
		.clk_src_l2r = AL_SRDS_CLK_SRC_L2R,
		.clk_src_core = AL_SRDS_CLK_SRC_R2L,
		.ref_clk_freq = AL_SRDS_CLK_FREQ_NA,
		.r2l_clk_freq = AL_SRDS_CLK_FREQ_NA,
		.l2r_clk_freq = AL_SRDS_CLK_FREQ_NA,
	},
       // Group D
	{
		.mode = AL_SRDS_CFG_SGMII,
		.clk_src_r2l = AL_SRDS_CLK_SRC_R2L,
		.clk_src_l2r = AL_SRDS_CLK_SRC_L2R,
		.clk_src_core = AL_SRDS_CLK_SRC_L2R,
		.ref_clk_freq = AL_SRDS_CLK_FREQ_NA,
		.r2l_clk_freq = AL_SRDS_CLK_FREQ_NA,
		.l2r_clk_freq = AL_SRDS_CLK_FREQ_125_MHZ,
	}},
	.skip_core_init = 0
};

struct al_serdes_obj obj;

int serdes_init()
{
	// Initialization
	return al_serdes_init(
			(void __iomem *)0xFD880000,
			(void __iomem *)0xFD8C0000,
			"SERDES", &cfg, &obj);
}

void serdes_loopback_prbs()
{
	///////////////////////////////////////
	// BIST (group A, lane 0):
	// 1. Loopback enable
	// 2. BIST pattern selection
	// 3. TX BIST enable
	// 4. RX BIST enable
	// 5. Wait
	// 6. RX BIST check status
	// 7. TX BIST inject error
	// 8. RX BIST check status
	// 9. RX BIST disable
	// 10. TX BIST disable
	// 11. Loopback disable
	///////////////////////////////////////
	al_bool			*is_locked,
	al_bool			*err_cnt_overflow,
	uint16_t		*err_cnt);

	al_serdes_loopback_control(
		obj, AL_SRDS_GRP_A, AL_SRDS_LANE_0,
		AL_SRDS_LB_MODE_PMA_PARALLEL_RX_TO_TX);

	al_serdes_bist_pattern_select(
		obj, AL_SRDS_GRP_A, AL_SRDS_BIST_PATTERN_PRBS7, NULL);

	al_serdes_bist_tx_enable(obj, AL_SRDS_GRP_A, AL_SRDS_LANE_0, AL_TRUE);

	al_serdes_bist_rx_enable(obj, AL_SRDS_GRP_A, AL_SRDS_LANE_0, AL_TRUE);

	al_udelay(1000000);

	al_serdes_bist_rx_status(
		obj, AL_SRDS_GRP_A, AL_SRDS_LANE_0, &is_locked,
		&err_cnt_overflow, &err_cnt);

	al_serdes_bist_tx_err_inject(obj, AL_SRDS_GRP_A);

	al_serdes_bist_rx_status(
		obj, AL_SRDS_GRP_A, AL_SRDS_LANE_0, &is_locked,
		&err_cnt_overflow, &err_cnt);

	al_serdes_bist_rx_enable(obj, AL_SRDS_GRP_A, AL_SRDS_LANE_0, AL_FALSE);

	al_serdes_bist_tx_enable(obj, AL_SRDS_GRP_A, AL_SRDS_LANE_0, AL_FALSE);

	al_serdes_loopback_control(
		obj, AL_SRDS_GRP_A, AL_SRDS_LANE_0, AL_SRDS_LB_MODE_OFF);
}

/** @endcode */

/** @} */
