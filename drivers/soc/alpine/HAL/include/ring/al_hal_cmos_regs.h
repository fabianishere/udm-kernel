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
#ifndef __AL_HAL_CMOS_REGS_H__
#define __AL_HAL_CMOS_REGS_H__

#include <al_hal_plat_types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct al_cmos_regs_m0 {
	/* 0x00 - RGMII delay control */
	uint32_t rgmii_dly_ctl;
	/* 0x04 - DLL update rate */
	uint32_t dll_upd_rate;
	/* 0x08 - DDRS mode */
	uint32_t ddrs_mode;
	/* 0x0c - Pull up enable */
	uint32_t pullup_en;
	/* 0x10 - Data Mask */
	uint32_t data_mask;
	/* 0x14 - RGMII status */
	uint32_t rgmii_status;
};

#define AL_CMOS_NUM_GROUPS			10

#if defined(AL_DEV_ID)
#if (AL_DEV_ID <= AL_DEV_ID_ALPINE_V2)
#define AL_CMOS_GROUP_RGMII_A			2
#define AL_CMOS_GROUP_RGMII_B			3
#else
#define AL_CMOS_GROUP_RGMII_A			1
#define AL_CMOS_GROUP_RGMII_B			2
#endif
#endif

#define AL_CMOS_GROUP_NB_CPU_CLK_OUT_EN		9

struct al_cmos_group_regs {
	/* 0x00 - General setup */
	uint32_t setup;
	/* 0x04 - DDR setup - relevant to RGMII groups only (2, 3) */
	uint32_t ddr_setup;
	/* 0x08 - I/O BIST flow - relevant to RGMII groups only (2, 3) */
	uint32_t io_bist_flow;
	/* 0x0c - I/O BIST status - relevant to RGMII groups only (2, 3) */
	uint32_t io_bist_status;
	/* 0x10 - 0x1c - Test pattern - relevant to RGMII groups only (2, 3) */
	uint32_t test_pattern[4];
	/* 0x20- CMOS buffer drive select - relevant to RGMII groups only (2, 3) */
	uint32_t cmos_drv;
	/* 0x24 - 0xfc - Reserved */
	uint32_t reserved[(0x100 - 0x24) / sizeof(uint32_t)];
};

struct al_cmos_regs {
	struct al_cmos_group_regs groups[AL_CMOS_NUM_GROUPS];
};

/*
* Registers Fields
*/

/**** cmos_drv register ****/
/* Buffer drive strength. Two bits per pad. (00=4mA,01=6mA,10=8mA,11=12mA) */
#define AL_CVOS_REGS_CMOS_DRV_TYPE_MASK 0x00FFFFFF
#define AL_CMOS_REGS_CMOS_DRV_TYPE_SHIFT 0
#define AL_CMOS_REGS_CMOS_DRV_TYPE_VAL(val)					\
	(((val) << 22) | ((val) << 20) | ((val) << 18) | ((val) << 16) |	\
	((val) << 14) | ((val) << 12) | ((val) << 10) | ((val) << 8) |		\
	((val) << 6) | ((val) << 4) | ((val) << 2) | ((val) << 0))

/* Register 'setup' field NB_CPU_CLK_OUT_EN */
/**
 * Group 9 only – used for enabling Clock Outs of the NB and CPU PLL's to the
 * DTO ports. This is selected as the default.
 * This field is for internal characterizations and must not be changed.
 */
#define AL_CMOS_REGS_SETUP_NB_CPU_CLK_OUT_EN		AL_BIT(31)

/* Register 'setup' field DATA_MASK */
/**
 * Group Data Masks for bits [14:0]
 */
#define AL_CMOS_REGS_SETUP_DATA_MASK_MASK		AL_FIELD_MASK(30, 16)
#define AL_CMOS_REGS_SETUP_DATA_MASK_SHIFT		16

/* Register 'setup' field DATA_IN_DIS_TST */
/**
 * Data in disable while in test mode
 */
#define AL_CMOS_REGS_SETUP_DATA_IN_DIS_TST		AL_BIT(15)

/* Register 'setup' field PULL_UP_EN */
/**
 * Group buffer pull-up enable for bits [14:0]
 */
#define AL_CMOS_REGS_SETUP_PULLUP_EN_MASK		AL_FIELD_MASK(14, 0)
#define AL_CMOS_REGS_SETUP_PULLUP_EN_SHIFT		0

/* Register 'ddr_setup' field DDR_EN */
/**
 * Relevant only for groups 2, 3:
 * RGMII Double Data Rate Enable
 */
#define AL_CMOS_REGS_DDR_SETUP_DDR_EN			AL_BIT(31)

/* Register 'ddr_setup' field DLL_EN */
/**
 * Relevant only for groups 2, 3:
 * RGMII Dynamic DLL Enable
 */
#define AL_CMOS_REGS_DDR_SETUP_DLL_EN			AL_BIT(30)

/* Register 'ddr_setup' field DLL_TX_RELOCK_EN */
/**
 * Relevant only for groups 2, 3:
 * RGMII dynamic auto-update tx
 */
#define AL_CMOS_REGS_DDR_SETUP_DLL_TX_RELOCK_EN		AL_BIT(29)

/* Register 'ddr_setup' field DLL_RX_RELOCK_EN */
/**
 * Relevant only for groups 2, 3:
 * RGMII dynamic auto-update rx
 */
#define AL_CMOS_REGS_DDR_SETUP_DLL_RX_RELOCK_EN		AL_BIT(28)

/* Register 'ddr_setup' field DLL_CLK_SEL */
/**
 * Relevant only for groups 2, 3:
 * RGMII dynamic clock select
 */
#define AL_CMOS_REGS_DDR_SETUP_DLL_CLK_SEL		AL_BIT(27)

/* Register 'ddr_setup' field DLL_ERR */
/**
 * Relevant only for groups 2, 3:
 * RGMII DLL error
 */
#define AL_CMOS_REGS_DDR_SETUP_DLL_ERR			AL_BIT(26)

/* Register 'ddr_setup' field DLL_DONE */
/**
 * Relevant only for groups 2, 3:
 * RGMII DLL sweep done
 */
#define AL_CMOS_REGS_DDR_SETUP_DLL_DONE			AL_BIT(25)

/* Register 'ddr_setup' field DLL_BUSY */
/**
 * Relevant only for groups 2, 3:
 * RO: RGMII DLL sweep in progress / WO: Sweep trigger
 */
#define AL_CMOS_REGS_DDR_SETUP_DLL_BUSY			AL_BIT(24)

/* Register 'ddr_setup' field DLL_CLEAR*/
/**
 * Relevant only for groups 2, 3:
 * RGMII DLL clear (break sweep)
 */
#define AL_CMOS_REGS_DDR_SETUP_DLL_CLEAR		AL_BIT(23)

/* Register 'ddr_setup' field DLL_OFFSET_EN */
/**
 * Relevant only for groups 2, 3:
 * RGMII DLL offset load enable
 */
#define AL_CMOS_REGS_DDR_SETUP_DLL_OFFSET_EN		AL_BIT(22)

/* Register 'ddr_setup' field DLL_OFFSET */
/**
 * Relevant only for groups 2, 3:
 * RO: dll sweep result / WO: RGMII dll offset value
 */
#define AL_CMOS_REGS_DDR_SETUP_DLL_OFFSET_MASK		AL_FIELD_MASK(21, 16)
#define AL_CMOS_REGS_DDR_SETUP_DLL_OFFSET_SHIFT		16

/* Register 'ddr_setup' field TX_DLY */
/**
 * Relevant only for groups 2, 3:
 * RGMII TX Delay configuration
 */
#define AL_CMOS_REGS_DDR_SETUP_TX_DLY_MASK		AL_FIELD_MASK(13, 8)
#define AL_CMOS_REGS_DDR_SETUP_TX_DLY_SHIFT		8

/* Register 'ddr_setup' field RX_DLY */
/**
 * Relevant only for groups 2, 3:
 * RGMII RX Delay configuration
 */
#define AL_CMOS_REGS_DDR_SETUP_RX_DLY_MASK		AL_FIELD_MASK(5, 0)
#define AL_CMOS_REGS_DDR_SETUP_RX_DLY_SHIFT		0

/*
 * Register 'io_bist_flow'
 * This register is for internal characterizations and must not be changed.
 */

/* Register 'io_bist_flow' field RX_START */
/**
 * Relevant only for groups 2, 3:
 * RX test flow start
 * RX and TX together activates a loopback test
 */
#define AL_CMOS_REGS_IO_BIST_FLOW_RX_START		AL_BIT(31)

/* Register 'io_bist_flow' field TX_START */
/**
 * Relevant only for groups 2, 3:
 * TX test flow start
 * RX and TX together activates a loopback test
 */
#define AL_CMOS_REGS_IO_BIST_FLOW_TX_START		AL_BIT(30)

/* Register 'io_bist_flow' field RXH_MASK */
/**
 * Relevant only for groups 2, 3:
 * RXH bit masking {CTL,RXD[3:0]}
 */
#define AL_CMOS_REGS_IO_BIST_FLOW_RXH_MASK_MASK		AL_FIELD_MASK(12, 8)
#define AL_CMOS_REGS_IO_BIST_FLOW_RXH_MASK_SHIFT	8

/* Register 'io_bist_flow' field RXL_MASK */
/**
 * Relevant only for groups 2, 3:
 * RXL bit masking {CTL,RXD[3:0]}
 */
#define AL_CMOS_REGS_IO_BIST_FLOW_RXL_MASK_MASK		AL_FIELD_MASK(4, 0)
#define AL_CMOS_REGS_IO_BIST_FLOW_RXL_MASK_SHIFT	0

/* Register 'io_bist_status' field RX_TEST_VALID */
/**
 * Relevant only for groups 2, 3:
 * RX test result valid
 */
#define AL_CMOS_REGS_IO_BIST_STATUS_RX_TEST_VALID	AL_BIT(31)

/* Register 'io_bist_status' field RX_TEST_ERR */
/**
 * Relevant only for groups 2, 3:
 * RX test error detected
 */
#define AL_CMOS_REGS_IO_BIST_STATUS_RX_TEST_ERR		AL_BIT(30)

/* Register 'io_bist_status' field RX_TEST_STAGE */
/**
 * Relevant only for groups 2, 3:
 * RX test stage (0 - preamble, 1 - run)
 */
#define AL_CMOS_REGS_IO_BIST_STATUS_RX_TEST_STAGE	AL_BIT(27)

/* Register 'io_bist_status' field TEST_VEC_CNT */
/**
 * Relevant only for groups 2, 3:
 * Test vector count at the falling stage
 */
#define AL_CMOS_REGS_IO_BIST_STATUS_TEST_VEC_CNT_MASK	AL_FIELD_MASK(26, 24)
#define AL_CMOS_REGS_IO_BIST_STATUS_TEST_VEC_CNT_SHIFT	24

/* Register 'io_bist_status' field RXH_REF */
/**
 * Relevant only for groups 2, 3:
 * RXH reference pattern {CTL,RXD[3:0]}
 */
#define AL_CMOS_REGS_IO_BIST_STATUS_RXH_REF_MASK	AL_FIELD_MASK(21, 17)
#define AL_CMOS_REGS_IO_BIST_STATUS_RXH_REF_SHIFT	17

/* Register 'io_bist_status' field RXL_REF */
/**
 * Relevant only for groups 2, 3:
 * RXL reference pattern {CTL,RXD[3:0]}
 */
#define AL_CMOS_REGS_IO_BIST_STATUS_RXL_REF_MASK	AL_FIELD_MASK(16, 12)
#define AL_CMOS_REGS_IO_BIST_STATUS_RXL_REF_SHIFT	12

/* Register 'io_bist_status' field RXH_IN */
/**
 * Relevant only for groups 2, 3:
 * RXH incoming pattern {CTL,RXD[3:0]}
 */
#define AL_CMOS_REGS_IO_BIST_STATUS_RXH_IN_MASK		AL_FIELD_MASK(9, 5)
#define AL_CMOS_REGS_IO_BIST_STATUS_RXH_IN_SHIFT	5

/* Register 'io_bist_status' field RXL_IN */
/**
 * Relevant only for groups 2, 3:
 * RXL incoming pattern {CTL,RXD[3:0]}
 */
#define AL_CMOS_REGS_IO_BIST_STATUS_RXL_IN_MASK		AL_FIELD_MASK(4, 0)
#define AL_CMOS_REGS_IO_BIST_STATUS_RXL_IN_SHIFT	0

/* Register 'test_pattern' field DAT_HIGH_1 */
/**
 * Relevant only for groups 2, 3:
 * Data high pattern #1 {CTL,RXD[3:0]}
 */
#define AL_CMOS_REGS_TST_PAT_DAT_HIGH_1_MASK		AL_FIELD_MASK(28, 24)
#define AL_CMOS_REGS_TST_PAT_DAT_HIGH_1_SHIFT		24

/* Register 'test_pattern' field DAT_LOW_1 */
/**
 * Relevant only for groups 2, 3:
 * Data low pattern #1 {CTL,RXD[3:0]}
 */
#define AL_CMOS_REGS_TST_PAT_DAT_LOW_1_MASK		AL_FIELD_MASK(20, 16)
#define AL_CMOS_REGS_TST_PAT_DAT_LOW_1_SHIFT		16

/* Register 'test_pattern' field DAT_HIGH_0 */
/**
 * Relevant only for groups 2, 3:
 * Data high pattern #0 {CTL,RXD[3:0]}
 */
#define AL_CMOS_REGS_TST_PAT_DAT_HIGH_0_MASK		AL_FIELD_MASK(12, 8)
#define AL_CMOS_REGS_TST_PAT_DAT_HIGH_0_SHIFT		8

/* Register 'test_pattern' field DAT_LOW_0 */
/**
 * Relevant only for groups 2, 3:
 * Data low pattern #0 {CTL,RXD[3:0]}
 */
#define AL_CMOS_REGS_TST_PAT_DAT_LOW_0_MASK		AL_FIELD_MASK(4, 0)
#define AL_CMOS_REGS_TST_PAT_DAT_LOW_0_SHIFT		0

#ifdef __cplusplus
}
#endif

#endif

