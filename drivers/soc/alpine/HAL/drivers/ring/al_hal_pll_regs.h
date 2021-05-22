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

/**
 *  @{
 * @file   al_hal_pll_regs.h
 *
 * @brief PLL registers
 *
 */

#ifndef __AL_HAL_PLL_REGS_H__
#define __AL_HAL_PLL_REGS_H__

#include "al_hal_plat_types.h"

#ifdef __cplusplus
extern "C" {
#endif
/*
* Unit Registers
*/



struct al_pll_gen {
	/* [0x0] PLL IP coeficients */
	uint32_t setup_0_v1;
	/* [0x4] PLL IP coeficients and configuration */
	uint32_t setup_1;
	/* [0x8] PLL wrapper configuration */
	uint32_t setup_2;
	/* [0xc] PLL wrapper configuration */
	uint32_t setup_3;
	/* [0x10] Anpa Test mode control */
	uint32_t pll_test_setup_4;
	/* [0x14] Anpa Test mode result */
	uint32_t test_result_5;
	/* [0x18] Rev1 test mode and special controls */
	uint32_t setup_6_v1;
	/* [0x1c] PLL Wrapper status */
	uint32_t status_7;
};
struct al_pll_ch_pair {
	/* [0x0] Clock Channel dividers setup */
	uint32_t cfg;
};
struct al_pll_v2 {
	/* [0x0] PLL IP coeficients */
	uint32_t setup_0;
	uint32_t rsrvd_0[5];
	/* [0x18] Rev2 test mode and special controls */
	uint32_t setup_6;
	uint32_t rsrvd;
};
struct al_pll_v3 {
	/* [0x0] PLL IP coeficients */
	uint32_t setup_0;
	/* [0x4] PLL IP coeficients and configuration */
	uint32_t setup_3;
	uint32_t rsrvd_0[4];
	/* [0x18] Rev3 test mode and special controls */
	uint32_t setup_6;
	uint32_t rsrvd;
};

struct al_pll_regs {
	struct al_pll_gen gen;                                  /* [0x0] */
	struct al_pll_ch_pair ch_pair[8];                       /* [0x20] */
	struct al_pll_v2 v2;                                    /* [0x40] */
	uint32_t rsrvd_0[8];
	struct al_pll_v3 v3;                                    /* [0x80] */
};


/*
* Registers Fields
*/


/**** setup_0_v1 register ****/
/*
 * PLL NF encoding style requires value minus one. Default is set per PLL per
 * strapping and is different than zero.
 */
#define PLL_GEN_SETUP_0_V1_NFM1_MASK     0x00001FFF
#define PLL_GEN_SETUP_0_V1_NFM1_SHIFT    0
/* Reserved */
#define PLL_GEN_SETUP_0_V1_RSRVD_15_13_MASK 0x0000E000
#define PLL_GEN_SETUP_0_V1_RSRVD_15_13_SHIFT 13
/*
 * PLL NR encoding style requires value minus one. Default is set per PLL per
 * strapping and is different than zero.
 */
#define PLL_GEN_SETUP_0_V1_NRM1_MASK     0x003F0000
#define PLL_GEN_SETUP_0_V1_NRM1_SHIFT    16
/* Reserved */
#define PLL_GEN_SETUP_0_V1_RSRVD_23_22_MASK 0x00C00000
#define PLL_GEN_SETUP_0_V1_RSRVD_23_22_SHIFT 22
/*
 * PLL OD encoding style requires value minus one. Default is set per PLL per
 * strapping and is different than zero.
 */
#define PLL_GEN_SETUP_0_V1_ODM1_MASK     0x0F000000
#define PLL_GEN_SETUP_0_V1_ODM1_SHIFT    24
/* Reserved */
#define PLL_GEN_SETUP_0_V1_RSRVD_30_28_MASK 0x70000000
#define PLL_GEN_SETUP_0_V1_RSRVD_30_28_SHIFT 28
/* Re-Lock request (detect by re-assert). */
#define PLL_GEN_SETUP_0_V1_RELOCK        (1 << 31)

/**** setup_1 register ****/
/* PLL BWADJ encoding style requires value minus one. */
#define PLL_GEN_SETUP_1_BWADJM1_V1_MASK 0x00000FFF
#define PLL_GEN_SETUP_1_BWADJM1_V1_SHIFT 0
/*
 * Core Reset Extend (power of two), times 16 cycles (gives the range of 16 to
 * 2048 clock cycles). Default is for 256 cycles.
 */
#define PLL_GEN_SETUP_1_CORE_RESET_EXTEND_MASK 0x00007000
#define PLL_GEN_SETUP_1_CORE_RESET_EXTEND_SHIFT 12
/* PLL Lock Stage use NR scaling or special count value. */
#define PLL_GEN_SETUP_1_LOCK_USE_NR_V1 (1 << 15)
/* PLL Lock Stage value (if not NR scaling). Taken to match max NR expected. */
#define PLL_GEN_SETUP_1_LOCK_COUNT_MAX_V1_MASK 0xFFFF0000
#define PLL_GEN_SETUP_1_LOCK_COUNT_MAX_V1_SHIFT 16

/**** setup_2 register ****/
/*
 * PLL reset and bandgap-power-down RefClk count period. For Rev2 the default
 * value is 0x80. For Rev3 the default is 0x100
 */
#define PLL_GEN_SETUP_2_PLL_RST_COUNT_MASK 0x0000FFFF
#define PLL_GEN_SETUP_2_PLL_RST_COUNT_SHIFT 0
/* Dividers settle RefClk count period. (default is for 2.5MHz case). */
#define PLL_GEN_SETUP_2_DIV_SETTLE_COUNT_MASK 0xFFFF0000
#define PLL_GEN_SETUP_2_DIV_SETTLE_COUNT_SHIFT 16

/**** setup_3 register ****/
/* Dividers Set/Sample RefClk count period. */
#define PLL_GEN_SETUP_3_DIV_SET_COUNT_MASK 0x0000FFFF
#define PLL_GEN_SETUP_3_DIV_SET_COUNT_SHIFT 0
/*
 * Dividers Keep towards Re-Lock, RefClk count. (for Bresp of endpoint prior to
 * shutting of clocks).
 */
#define PLL_GEN_SETUP_3_DIV_KEEP_COUNT_MASK 0xFFFF0000
#define PLL_GEN_SETUP_3_DIV_KEEP_COUNT_SHIFT 16

/**** pll_test_setup_4 register ****/
/*
 * Test Count Period (default is set to 1mSec in RefClk count terms). (1250
 * cycles)
 */
#define PLL_GEN_PLL_TEST_SETUP_4_TEST_PERIOD_MASK 0x00FFFFFF
#define PLL_GEN_PLL_TEST_SETUP_4_TEST_PERIOD_SHIFT 0
/* PLL Clock Test Count Ratio, power of two. (default = ratio of four). */
#define PLL_GEN_PLL_TEST_SETUP_4_TEST_RATIO_MASK 0x07000000
#define PLL_GEN_PLL_TEST_SETUP_4_TEST_RATIO_SHIFT 24
/* Test Enable */
#define PLL_GEN_PLL_TEST_SETUP_4_TEST_ENABLE (1 << 27)
/* Reserved */
#define PLL_GEN_PLL_TEST_SETUP_4_RSRVD_30_28_MASK 0x70000000
#define PLL_GEN_PLL_TEST_SETUP_4_RSRVD_30_28_SHIFT 28
/* Test Enable */
#define PLL_GEN_PLL_TEST_SETUP_4_TEST_TRIGGER (1 << 31)

/**** test_result_5 register ****/
/* Test Counter as measured in PLL-Ratioed clock (based on Reg4). */
#define PLL_GEN_TEST_RESULT_5_TEST_COUNT_RESULT_MASK 0xFFFFFFFF
#define PLL_GEN_TEST_RESULT_5_TEST_COUNT_RESULT_SHIFT 0

/**** setup_6_v1 register ****/
/* select external regulator input when high */
#define PLL_GEN_SETUP_6_V1_REG_EXT_SEL   (1 << 0)
/* selects regulator test path when high */
#define PLL_GEN_SETUP_6_V1_REG_TEST_SEL  (1 << 1)
/* selects bandgap replica output when high (if REG_TEST_SEL=1) */
#define PLL_GEN_SETUP_6_V1_REG_TEST_REP  (1 << 2)
/* selects regulator output when high (if REG_TEST_SEL=1) */
#define PLL_GEN_SETUP_6_V1_REG_TEST_OUT  (1 << 3)
/* drives regulator input when high (if REG_TEST_SEL=1) */
#define PLL_GEN_SETUP_6_V1_REG_TEST_DRV  (1 << 4)
/* Reserved */
#define PLL_GEN_SETUP_6_V1_RSRVD_7_5_MASK 0x000000E0
#define PLL_GEN_SETUP_6_V1_RSRVD_7_5_SHIFT 5
/* Enable Fast Relock flow */
#define PLL_GEN_SETUP_6_V1_FASTEN        (1 << 8)
/* Enable Saturate mode */
#define PLL_GEN_SETUP_6_V1_ENSAT         (1 << 9)
/* Selects PLL Test mode (TCI) */
#define PLL_GEN_SETUP_6_V1_TEST          (1 << 10)
/* Reserved */
#define PLL_GEN_SETUP_6_V1_RSRVD_15_11_MASK 0x0000F800
#define PLL_GEN_SETUP_6_V1_RSRVD_15_11_SHIFT 11
/* BG_RESET security key. (for usage model - contact the Annapurna Labs) */
#define PLL_GEN_SETUP_6_V1_BG_RST_KEY_MASK 0x00FF0000
#define PLL_GEN_SETUP_6_V1_BG_RST_KEY_SHIFT 16
/* Band Gap Power Down */
#define PLL_GEN_SETUP_6_V1_BG_PWR_DOWN   (1 << 24)
/* Reserved */
#define PLL_GEN_SETUP_6_V1_RSRVD_29_25_MASK 0x3E000000
#define PLL_GEN_SETUP_6_V1_RSRVD_29_25_SHIFT 25
/* PLL Power Down */
#define PLL_GEN_SETUP_6_V1_POWER_DOWN    (1 << 30)
/* PLL Bypass, set per PLL depends on strapping. */
#define PLL_GEN_SETUP_6_V1_PLL_BYPASS    (1 << 31)

/**** status_7 register ****/
/* Reserved */
#define PLL_GEN_STATUS_7_RSRVD_7_0_MASK  0x000000FF
#define PLL_GEN_STATUS_7_RSRVD_7_0_SHIFT 0
/*
 * PLL Control State Machine states. For detailed encoding contact Annapurna
 * Labs.
 */
#define PLL_GEN_STATUS_7_PLL_FSM_STATE_MASK 0x00000F00
#define PLL_GEN_STATUS_7_PLL_FSM_STATE_SHIFT 8
/* Reserved */
#define PLL_GEN_STATUS_7_RSRVD_15_12_MASK 0x0000F000
#define PLL_GEN_STATUS_7_RSRVD_15_12_SHIFT 12
/* PLL Revision ID (0-Rev1, 1=Rev2, 2=Rev3) */
#define PLL_GEN_STATUS_7_REVISION_ID_MASK 0x000F0000
#define PLL_GEN_STATUS_7_REVISION_ID_SHIFT 16
/* Reserved */
#define PLL_GEN_STATUS_7_RSRVD_28_20_MASK 0x1FF00000
#define PLL_GEN_STATUS_7_RSRVD_28_20_SHIFT 20
/* Anpa Test Mode – count busy */
#define PLL_GEN_STATUS_7_ATM_BUSY        (1 << 29)
/* Post PLL (channels) Dividers Locked */
#define PLL_GEN_STATUS_7_DIV_LOCK        (1 << 30)
/* PLL Locked (FSM based Counter’s indication). */
#define PLL_GEN_STATUS_7_PLL_LOCK        (1 << 31)

/**** cfg register ****/
/*
 * Divide value, according to strap per divider (zero == disable this channel).
 */
#define PLL_CH_PAIR_CFG_DIV0_VALUE_MASK  0x000007FF
#define PLL_CH_PAIR_CFG_DIV0_VALUE_SHIFT 0
/* Divide value half indication, for 2:N ratios */
#define PLL_CH_PAIR_CFG_DIV0_HALF        (1 << 11)
/* Request to Relock this divider */
#define PLL_CH_PAIR_CFG_DIV0_RELOCK_REQUEST (1 << 12)
/* This is the Last relock request for this round. */
#define PLL_CH_PAIR_CFG_DIV0_LAST_IN_SERIES (1 << 13)
/* Reset Mask. If set – suppress reset for that channel */
#define PLL_CH_PAIR_CFG_DIV0_RESET_MASK  (1 << 14)
/* RefClk bypass request for this channel */
#define PLL_CH_PAIR_CFG_DIV0_REF_CLK_BYPASS (1 << 15)
/*
 * Divide value, according to strap per divider (zero == disable this channel).
 */
#define PLL_CH_PAIR_CFG_DIV1_VALUE_MASK  0x07FF0000
#define PLL_CH_PAIR_CFG_DIV1_VALUE_SHIFT 16
/* Divide value half indication, for 2:N ratios */
#define PLL_CH_PAIR_CFG_DIV1_HALF        (1 << 27)
/* Request to Relock this divider */
#define PLL_CH_PAIR_CFG_DIV1_RELOCK_REQUEST (1 << 28)
/* This is the Last relock request for this round. */
#define PLL_CH_PAIR_CFG_DIV1_LAST_IN_SERIES (1 << 29)
/* Reset Mask. If set – suppress reset for that channel */
#define PLL_CH_PAIR_CFG_DIV1_RESET_MASK  (1 << 30)
/* RefClk bypass request for this channel */
#define PLL_CH_PAIR_CFG_DIV1_REF_CLK_BYPASS (1 << 31)

/**** Setup_0_v2 register ****/
/*
 * PLL Feedback Post Divider value. Default is set per PLL per strapping and is
 * different than zero.
 */
#define PLL_V2_SETUP_0_FB_DIV255_MASK 0x000000FF
#define PLL_V2_SETUP_0_FB_DIV255_SHIFT 0
/*
 * PLL Feedback Pre Divider value. Default is set per PLL per strapping. Zero =
 * divide by 2 ; One = divide by 3.
 */
#define PLL_V2_SETUP_0_FB_PRE23       (1 << 8)
/* Reserved */
#define PLL_V2_SETUP_0_RSRVD_15_9_MASK 0x0000FE00
#define PLL_V2_SETUP_0_RSRVD_15_9_SHIFT 9
/*
 * PLL Output Divider value. Default is set per PLL per strapping and is
 * different than zero.
 */
#define PLL_V2_SETUP_0_OUT_DIV_MASK   0x003F0000
#define PLL_V2_SETUP_0_OUT_DIV_SHIFT  16
/* Reserved */
#define PLL_V2_SETUP_0_RSRVD_23_22_MASK 0x00C00000
#define PLL_V2_SETUP_0_RSRVD_23_22_SHIFT 22
/*
 * PLL Incoming Reference clock divider. Default is set per PLL per strapping
 * and is different than zero.
 */
#define PLL_V2_SETUP_0_REF_IN_DIV_MASK 0x3F000000
#define PLL_V2_SETUP_0_REF_IN_DIV_SHIFT 24
/* Reserved */
#define PLL_V2_SETUP_0_RSRVD_30       (1 << 30)
/* Re-Lock request (detect by re-assert). */
#define PLL_V2_SETUP_0_RELOCK         (1 << 31)

/**** Setup_6_v2 register ****/
/*
 * Disable PLL output channels of fd8,fd4,fd2,fd1 respectively. Default is
 * enabling only fd1, as all other channels are not in use
 */
#define PLL_V2_SETUP_6_CLK_OUT_DISABLE_MASK 0x0000000F
#define PLL_V2_SETUP_6_CLK_OUT_DISABLE_SHIFT 0
/* Enable Slow claibration mode */
#define PLL_V2_SETUP_6_SLOW_CALIBRATION_ENABLE (1 << 4)
/* Reserved */
#define PLL_V2_SETUP_6_RSRVD_7_5_MASK 0x000000E0
#define PLL_V2_SETUP_6_RSRVD_7_5_SHIFT 5
/* Disable RefClock propagation while in Pre-Lock states. */
#define PLL_V2_SETUP_6_REF_DISABLE    (1 << 8)
/* Reserved */
#define PLL_V2_SETUP_6_RSRVD_11_9_MASK 0x00000E00
#define PLL_V2_SETUP_6_RSRVD_11_9_SHIFT 9
/* PLL FeedBack Loop Enable */
#define PLL_V2_SETUP_6_PLL_ENABLE     (1 << 12)
/* Reserved */
#define PLL_V2_SETUP_6_RSRVD_15_13_MASK 0x0000E000
#define PLL_V2_SETUP_6_RSRVD_15_13_SHIFT 13
/* Force reload of PLL Output Divider */
#define PLL_V2_SETUP_6_OUT_DIV_LOAD_ENABLE (1 << 16)
/* Reserved */
#define PLL_V2_SETUP_6_RSRVD_27_17_MASK 0x0FFE0000
#define PLL_V2_SETUP_6_RSRVD_27_17_SHIFT 17
/* PLL Power Down */
#define PLL_V2_SETUP_6_POWER_DOWN     (1 << 28)
/* Reserved */
#define PLL_V2_SETUP_6_RSRVD_30_29_MASK 0x60000000
#define PLL_V2_SETUP_6_RSRVD_30_29_SHIFT 29
/* PLL Bypass, set per PLL depends on strapping. */
#define PLL_V2_SETUP_6_PLL_BYPASS     (1 << 31)

/**** setup_0 register ****/
/*
 * PLL Feedback Divider value. Default is set per PLL per strapping and is
 * different than zero.
 */
#define PLL_V3_SETUP_0_FB_DIV_MASK    0x00000FFF
#define PLL_V3_SETUP_0_FB_DIV_SHIFT   0
/* Reserved */
#define PLL_V3_SETUP_0_RSRVD_14_12_MASK 0x00007000
#define PLL_V3_SETUP_0_RSRVD_14_12_SHIFT 12
/*
 * PLL Fractional Division enable. If enabled, the value is located in the next
 * register, otherwise - ignore.
 */
#define PLL_V3_SETUP_0_FRAC_ENABLE    (1 << 15)
/*
 * PLL Output Divider One value. Default is set per PLL per strapping and is
 * different than zero.
 */
#define PLL_V3_SETUP_0_POST_DIV_1_MASK 0x00070000
#define PLL_V3_SETUP_0_POST_DIV_1_SHIFT 16
/* Reserved */
#define PLL_V3_SETUP_0_RSRVD_19       (1 << 19)
/*
 * PLL Output Divider Tow value. Default is set per PLL per strapping and is
 * different than zero.
 */
#define PLL_V3_SETUP_0_POST_DIV_2_MASK 0x00700000
#define PLL_V3_SETUP_0_POST_DIV_2_SHIFT 20
/* Reserved */
#define PLL_V3_SETUP_0_RSRVD_23       (1 << 23)
/*
 * PLL Incoming Reference clock divider. Default is set per PLL per strapping
 * and is different than zero.
 */
#define PLL_V3_SETUP_0_REF_DIV_MASK   0x3F000000
#define PLL_V3_SETUP_0_REF_DIV_SHIFT  24
/* Reserved */
#define PLL_V3_SETUP_0_RSRVD_30       (1 << 30)
/* Re-Lock request (detect by re-assert). */
#define PLL_V3_SETUP_0_RELOCK         (1 << 31)

/**** setup_3 register ****/
/* PLL Fractional division. */
#define PLL_V3_SETUP_3_FRAC_MASK      0x00FFFFFF
#define PLL_V3_SETUP_3_FRAC_SHIFT     0
/* Reserved */
#define PLL_V3_SETUP_3_RSRVD_27_24_MASK 0x0F000000
#define PLL_V3_SETUP_3_RSRVD_27_24_SHIFT 24
/*
 * Core Reset Extend (power of two), times 16 cycles (gives the range of 16 to
 * 2048 clock cycles). Default is for 256 cycles.
 */
#define PLL_V3_SETUP_3_CORE_RESET_EXTEND_MASK 0x70000000
#define PLL_V3_SETUP_3_CORE_RESET_EXTEND_SHIFT 28
/* Reserved */
#define PLL_V3_SETUP_3_RSRVD_31       (1 << 31)

/**** setup_6 register ****/
/* Power Down clock out channel */
#define PLL_V3_SETUP_6_CLK_OUT_DISABLE (1 << 0)
/* Power Down DSM */
#define PLL_V3_SETUP_6_DSM_POWER_DOWN (1 << 1)
/* Power Down D/A converter */
#define PLL_V3_SETUP_6_DAC_POWER_DOWN (1 << 2)
/* Reserved */
#define PLL_V3_SETUP_6_RSRVD_27_3_MASK 0x0FFFFFF8
#define PLL_V3_SETUP_6_RSRVD_27_3_SHIFT 3
/* PLL Power Down */
#define PLL_V3_SETUP_6_POWER_DOWN     (1 << 28)
/* Reserved */
#define PLL_V3_SETUP_6_RSRVD_30_29_MASK 0x60000000
#define PLL_V3_SETUP_6_RSRVD_30_29_SHIFT 29
/* PLL Bypass, set per PLL depends on strapping. */
#define PLL_V3_SETUP_6_PLL_BYPASS     (1 << 31)

#ifdef __cplusplus
}
#endif

#endif

/** @} */


