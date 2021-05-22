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
 *  Ethernet
 *  @{
 * @file   al_mod_hal_eth_kr.c
 *
 * @brief  KR HAL driver for main functions (auto-neg, Link Training)
 *
 */

#include "al_mod_hal_iofic.h"
#include "al_mod_hal_eth_kr.h"
#include "al_mod_hal_eth_mac_regs.h"
#include "al_mod_hal_an_lt_wrapper_regs.h"
#include "al_mod_hal_eth_mac_v4_regs.h"
#include "al_mod_hal_eth_mac_v4_common_regs.h"

/** Offset to iofic regs in an_lt_wrapper regs */
#define AL_HAL_ETH_AN_LT_INT_MEM_OFFSET 0

enum al_mod_eth_lt_unit_rev {
	AL_ETH_LT_UNIT_REV_1 = 0,
	AL_ETH_LT_UNIT_REV_2,

	AL_ETH_LT_UNIT_REV_MAX
};

/** AN/LT units for Ethernet V4 */
enum al_mod_eth_v4_an_lt_unit {
	AL_ETH_V4_AN_LT_UNIT_40_BIT	= 0,
	AL_ETH_V4_AN_LT_UNIT_32_BIT	= 1,
	AL_ETH_V4_AN_LT_UNIT_20_BIT	= 2,
	AL_ETH_V4_AN_LT_UNIT_40_BIT_DME_DS_EN	= 3,
};

enum al_mod_eth_an_lt_unit {
	AL_ETH_AN_LT_UNIT_32_BIT	= 0,
	AL_ETH_AN_LT_UNIT_20_BIT	= 1,
	AL_ETH_AN_LT_UNIT_16_BIT	= 2,
};

enum al_mod_eth_an_lt_regs_ids {
	AL_ETH_KR_AN_CONTROL = 0,
	AL_ETH_KR_AN_STATUS,
	AL_ETH_KR_AN_ADV0,
	AL_ETH_KR_AN_ADV1,
	AL_ETH_KR_AN_ADV2,
	AL_ETH_KR_AN_REM_ADV0,
	AL_ETH_KR_AN_REM_ADV1,
	AL_ETH_KR_AN_REM_ADV2,
	AL_ETH_KR_AN_MS_CNT,
	AL_ETH_KR_PMD_CONTROL,
	AL_ETH_KR_PMD_STATUS,
	AL_ETH_KR_PMD_LP_COEF_UP,
	AL_ETH_KR_PMD_LP_STATUS_REPORT,
	AL_ETH_KR_PMD_LD_COEF_UP,
	AL_ETH_KR_PMD_LD_STATUS_REPORT,
	AL_ETH_KR_AN_XNP_ADV0,
	AL_ETH_KR_AN_XNP_ADV1,
	AL_ETH_KR_AN_XNP_ADV2,
	AL_ETH_KR_AN_REM_XNP_ADV0,
	AL_ETH_KR_AN_REM_XNP_ADV1,
	AL_ETH_KR_AN_REM_XNP_ADV2,
};

static uint32_t al_mod_eth_an_lt_regs_addr[][AL_ETH_LT_UNIT_REV_MAX] = {
	[AL_ETH_KR_AN_CONTROL]			=	{0  ,	0x0},
	[AL_ETH_KR_AN_STATUS]			=	{1  ,	0x4},
	[AL_ETH_KR_AN_ADV0]			=	{16 ,	0x8},
	[AL_ETH_KR_AN_ADV1]			=	{17 ,	0xc},
	[AL_ETH_KR_AN_ADV2]			=	{18 ,	0x10},
	[AL_ETH_KR_AN_REM_ADV0]			=	{19 ,	0x14},
	[AL_ETH_KR_AN_REM_ADV1]			=	{20 ,	0x18},
	[AL_ETH_KR_AN_REM_ADV2]			=	{21 ,	0x1c},
	[AL_ETH_KR_AN_MS_CNT]			=	{22 ,	0x20},
	[AL_ETH_KR_PMD_CONTROL]			=	{150,	0x400},
	[AL_ETH_KR_PMD_STATUS]			=	{151,	0x404},
	[AL_ETH_KR_PMD_LP_COEF_UP]		=	{152,	0x408},
	[AL_ETH_KR_PMD_LP_STATUS_REPORT]	=	{153,	0x40c},
	[AL_ETH_KR_PMD_LD_COEF_UP]		=	{154,	0x410},
	[AL_ETH_KR_PMD_LD_STATUS_REPORT]	=	{155,	0x414},
	[AL_ETH_KR_AN_XNP_ADV0]			=	{22 ,   0x24},
	[AL_ETH_KR_AN_XNP_ADV1]			=	{23 ,   0x28},
	[AL_ETH_KR_AN_XNP_ADV2]			=	{24 ,   0x2c},
	[AL_ETH_KR_AN_REM_XNP_ADV0]		=	{25 ,   0x30},
	[AL_ETH_KR_AN_REM_XNP_ADV1]		=	{26 ,   0x34},
	[AL_ETH_KR_AN_REM_XNP_ADV2]		=	{27 ,   0x38},
};


/*
 * AN(Auto Negotiation) registers
 * (read / write indirect with al_mod_eth_an_reg_read/write)
 */
#define AL_ETH_KR_AN_CONTROL_RESTART				AL_BIT(9)
#define AL_ETH_KR_AN_CONTROL_ENABLE				AL_BIT(12)
#define AL_ETH_KR_AN_CONTROL_NP_ENABLE				AL_BIT(13)

#define AL_ETH_KR_AN_STATUS_COMPLETED				AL_BIT(5)
#define AL_ETH_KR_AN_STATUS_BASE_PAGE_RECEIVED			AL_BIT(6)
#define AL_ETH_KR_AN_STATUS_CHECK_MASK				0xFF0A
#define AL_ETH_KR_AN_STATUS_CHECK_NO_ERROR			0x0008

/* AN advertising registers parsing */
/* register 1 */
#define AL_ETH_KR_AN_ADV1_SEL_FIELD_MASK			0x001f
#define AL_ETH_KR_AN_ADV1_SEL_FIELD_SHIFT			0
#define AL_ETH_KR_AN_ADV1_ECHOED_NONCE_MASK			0x03e0
#define AL_ETH_KR_AN_ADV1_ECHOED_NONCE_SHIFT			5
#define AL_ETH_KR_AN_ADV1_CAPABILITY_MASK			0x1c00
#define AL_ETH_KR_AN_ADV1_CAPABILITY_SHIFT			10
#define AL_ETH_KR_AN_ADV1_REM_FAULT_MASK			0x2000
#define AL_ETH_KR_AN_ADV1_REM_FAULT_SHIFT			13
#define AL_ETH_KR_AN_ADV1_ACK_MASK				0x4000
#define AL_ETH_KR_AN_ADV1_ACK_SHIFT				14
#define AL_ETH_KR_AN_ADV1_NEXT_PAGE_MASK			0x8000
#define AL_ETH_KR_AN_ADV1_NEXT_PAGE_SHIFT			15
/* register 2 */
#define AL_ETH_KR_AN_ADV2_TX_NONCE_MASK				0x001f
#define AL_ETH_KR_AN_ADV2_TX_NONCE_SHIFT			0
#define AL_ETH_KR_AN_ADV2_TECH_MASK				0xffe0
#define AL_ETH_KR_AN_ADV2_TECH_SHIFT				5
/* register 3 */
/* TECH field in the third register is extended to the field in the second
 * register and it is currently reserved (should be always 0) */
#define AL_ETH_KR_AN_ADV3_TECH_MASK				0x1fff
#define AL_ETH_KR_AN_ADV3_TECH_SHIFT				0
#define AL_ETH_KR_AN_ADV3_FEC_MASK				0xc000
#define AL_ETH_KR_AN_ADV3_FEC_SHIFT				14

/* Next Page Fields */
/* register 1 */
#define AL_ETH_KR_AN_NP_ADV1_DATA1_MASK				0x07ff
#define AL_ETH_KR_AN_NP_ADV1_DATA1_SHIFT			0
#define AL_ETH_KR_AN_NP_ADV1_TOGGLE_MASK			0x0800
#define AL_ETH_KR_AN_NP_ADV1_TOGGLE_SHIFT			11
#define AL_ETH_KR_AN_NP_ADV1_ACK2_MASK				0x1000
#define AL_ETH_KR_AN_NP_ADV1_ACK2_SHIFT				12
#define AL_ETH_KR_AN_NP_ADV1_MSG_PAGE_MASK			0x2000
#define AL_ETH_KR_AN_NP_ADV1_MSG_PAGE_SHIFT			13
#define AL_ETH_KR_AN_NP_ADV1_ACK_MASK				0x4000
#define AL_ETH_KR_AN_NP_ADV1_ACK_SHIFT				14
#define AL_ETH_KR_AN_NP_ADV1_NP_MASK				0x8000
#define AL_ETH_KR_AN_NP_ADV1_NP_SHIFT				15

/*
 * LT(Link Training) registers
 * (read / write indirect with al_mod_eth_pma_reg_read/write)
 */
#define AL_ETH_KR_PMD_CONTROL_RESTART				0
#define AL_ETH_KR_PMD_CONTROL_ENABLE				1

#define AL_ETH_KR_PMD_STATUS_RECEIVER_COMPLETED_SHIFT		0
#define AL_ETH_KR_PMD_STATUS_RECEIVER_FRAME_LOCK_SHIFT		1
#define AL_ETH_KR_PMD_STATUS_RECEIVER_START_UP_PROTO_PROG_SHIFT	2
#define AL_ETH_KR_PMD_STATUS_FAILURE_SHIFT			3

#define AL_ETH_KR_PMD_LP_COEF_UP_MINUS_MASK			0x0003
#define AL_ETH_KR_PMD_LP_COEF_UP_MINUS_SHIFT			0
#define AL_ETH_KR_PMD_LP_COEF_UP_ZERO_MASK			0x000C
#define AL_ETH_KR_PMD_LP_COEF_UP_ZERO_SHIFT			2
#define AL_ETH_KR_PMD_LP_COEF_UP_PLUS_MASK			0x0030
#define AL_ETH_KR_PMD_LP_COEF_UP_PLUS_SHIFT			4
#define AL_ETH_KR_PMD_LP_COEF_UP_INITIALIZE_SHIFT		12
#define AL_ETH_KR_PMD_LP_COEF_UP_PRESET_SHIFT			13

#define AL_ETH_KR_PMD_LP_STATUS_REPORT_MINUS_MASK		0x0003
#define AL_ETH_KR_PMD_LP_STATUS_REPORT_MINUS_SHIFT		0
#define AL_ETH_KR_PMD_LP_STATUS_REPORT_ZERO_MASK		0x000C
#define AL_ETH_KR_PMD_LP_STATUS_REPORT_ZERO_SHIFT		2
#define AL_ETH_KR_PMD_LP_STATUS_REPORT_PLUS_MASK		0x0030
#define AL_ETH_KR_PMD_LP_STATUS_REPORT_PLUS_SHIFT		4
#define AL_ETH_KR_PMD_LP_STATUS_RECEIVER_READY_SHIFT		15

#define AL_ETH_KR_PMD_LD_COEF_UP_MINUS_MASK			0x0003
#define AL_ETH_KR_PMD_LD_COEF_UP_MINUS_SHIFT			0
#define AL_ETH_KR_PMD_LD_COEF_UP_ZERO_MASK			0x000C
#define AL_ETH_KR_PMD_LD_COEF_UP_ZERO_SHIFT			2
#define AL_ETH_KR_PMD_LD_COEF_UP_PLUS_MASK			0x0030
#define AL_ETH_KR_PMD_LD_COEF_UP_PLUS_SHIFT			4
#define AL_ETH_KR_PMD_LD_COEF_UP_INITIALIZE_SHIFT		12
#define AL_ETH_KR_PMD_LD_COEF_UP_PRESET_SHIFT			13

#define AL_ETH_KR_PMD_LD_STATUS_REPORT_MINUS_MASK		0x0003
#define AL_ETH_KR_PMD_LD_STATUS_REPORT_MINUS_SHIFT		0
#define AL_ETH_KR_PMD_LD_STATUS_REPORT_ZERO_MASK		0x000C
#define AL_ETH_KR_PMD_LD_STATUS_REPORT_ZERO_SHIFT		2
#define AL_ETH_KR_PMD_LD_STATUS_REPORT_PLUS_MASK		0x0030
#define AL_ETH_KR_PMD_LD_STATUS_REPORT_PLUS_SHIFT		4
#define AL_ETH_KR_PMD_LD_STATUS_REPORT_RECEIVER_READY_SHIFT	15

#define AL_ETH_KR_AN_MPS_CHECK_AN_DONE 500

#define AL_ETH_AL_LT_SEL_CH0_ALL_LANES 0x03020100

#define AL_ETH_AN_LT_DME_DS_MAX_CURR_SAMPLE_POINT_MASK		AL_FIELD_MASK(7, 0)
#define AL_ETH_AN_LT_DME_DS_MAX_CURR_SAMPLE_POINT_SHIFT		0
#define AL_ETH_AN_LT_DME_DS_MASK_FIND_FIRST_VEC_MASK		AL_FIELD_MASK(14, 8)
#define AL_ETH_AN_LT_DME_DS_MASK_FIND_FIRST_VEC_SHIFT		8
#define AL_ETH_AN_LT_DME_DS_ENABLE_OVERRIDE_VAL			AL_BIT(15)

enum al_mod_eth_an_lt_regs {
	AL_ETH_AN_REGS,
	AL_ETH_LT_REGS,
};

static void al_mod_eth_kr_an_get_indir_addr_by_lane(struct al_mod_eth_mac_obj *obj, unsigned int lane,
	uint32_t **an_lt_addr, uint32_t **an_lt_data)
{
	struct al_mod_eth_mac_v4_regs *mac_regs_base = obj->mac_regs_base;

	switch (lane) {
	case AL_ETH_AN__LT_LANE_0:
		*an_lt_addr = &mac_regs_base->mtip.an_lt_0_addr;
		*an_lt_data = &mac_regs_base->mtip.an_lt_0_data;
		break;
	case AL_ETH_AN__LT_LANE_1:
		*an_lt_addr = &mac_regs_base->mtip.an_lt_1_addr;
		*an_lt_data = &mac_regs_base->mtip.an_lt_1_data;
		break;
	case AL_ETH_AN__LT_LANE_2:
		*an_lt_addr = &mac_regs_base->mtip.an_lt_2_addr;
		*an_lt_data = &mac_regs_base->mtip.an_lt_2_data;
		break;
	case AL_ETH_AN__LT_LANE_3:
		*an_lt_addr = &mac_regs_base->mtip.an_lt_3_addr;
		*an_lt_data = &mac_regs_base->mtip.an_lt_3_data;
		break;
	default:
		al_mod_err("%s: Unknown Lane %d\n", __func__, lane);
		*an_lt_addr = NULL;
		*an_lt_data = NULL;
		al_mod_assert(0);
		return;
	}

	return;
}

static uint16_t al_mod_eth_an_lt_reg_read(
			struct al_mod_hal_eth_adapter *adapter,
			enum al_mod_eth_an_lt_regs_ids reg_id,
			enum al_mod_eth_an_lt_regs an_lt,
			enum al_mod_eth_an_lt_lane lane)
{
	uint32_t val;
	uint16_t reg_addr;

	if (adapter->rev_id < AL_ETH_REV_ID_3) {
		al_mod_assert(lane == AL_ETH_AN__LT_LANE_0);

		reg_addr = al_mod_eth_an_lt_regs_addr[reg_id][AL_ETH_LT_UNIT_REV_1];
		if (an_lt == AL_ETH_AN_REGS) {
			al_mod_reg_write32(&adapter->mac_regs_base->kr.an_addr, reg_addr);
			val = al_mod_reg_read32(&adapter->mac_regs_base->kr.an_data);
		} else {
			al_mod_reg_write32(&adapter->mac_regs_base->kr.pma_addr, reg_addr);
			val = al_mod_reg_read32(&adapter->mac_regs_base->kr.pma_data);
		}
	} else if (adapter->rev_id == AL_ETH_REV_ID_3) {
		struct al_mod_an_lt_wrapper_regs *regs = NULL;

		reg_addr = al_mod_eth_an_lt_regs_addr[reg_id][AL_ETH_LT_UNIT_REV_2];

		switch (lane) {
		case AL_ETH_AN__LT_LANE_0:
			al_mod_reg_write32(&adapter->mac_regs_base->gen_v3.an_lt_0_addr,
				       (uintptr_t)&regs->an_lt[adapter->curr_lt_unit].addr);
			al_mod_reg_write32(&adapter->mac_regs_base->gen_v3.an_lt_0_data,
				       reg_addr);

			al_mod_reg_write32(&adapter->mac_regs_base->gen_v3.an_lt_0_addr,
				       (uintptr_t)&regs->an_lt[adapter->curr_lt_unit].data);
			val = al_mod_reg_read32(&adapter->mac_regs_base->gen_v3.an_lt_0_data);
			break;
		case AL_ETH_AN__LT_LANE_1:
			al_mod_reg_write32(&adapter->mac_regs_base->gen_v3.an_lt_1_addr,
				       (uintptr_t)&regs->an_lt[adapter->curr_lt_unit].addr);
			al_mod_reg_write32(&adapter->mac_regs_base->gen_v3.an_lt_1_data,
				       reg_addr);

			al_mod_reg_write32(&adapter->mac_regs_base->gen_v3.an_lt_1_addr,
				       (uintptr_t)&regs->an_lt[adapter->curr_lt_unit].data);
			val = al_mod_reg_read32(&adapter->mac_regs_base->gen_v3.an_lt_1_data);
			break;
		case AL_ETH_AN__LT_LANE_2:
			al_mod_reg_write32(&adapter->mac_regs_base->gen_v3.an_lt_2_addr,
				       (uintptr_t)&regs->an_lt[adapter->curr_lt_unit].addr);
			al_mod_reg_write32(&adapter->mac_regs_base->gen_v3.an_lt_2_data,
				       reg_addr);

			al_mod_reg_write32(&adapter->mac_regs_base->gen_v3.an_lt_2_addr,
				       (uintptr_t)&regs->an_lt[adapter->curr_lt_unit].data);
			val = al_mod_reg_read32(&adapter->mac_regs_base->gen_v3.an_lt_2_data);
			break;
		case AL_ETH_AN__LT_LANE_3:
			al_mod_reg_write32(&adapter->mac_regs_base->gen_v3.an_lt_3_addr,
				       (uintptr_t)&regs->an_lt[adapter->curr_lt_unit].addr);
			al_mod_reg_write32(&adapter->mac_regs_base->gen_v3.an_lt_3_data,
				       reg_addr);

			al_mod_reg_write32(&adapter->mac_regs_base->gen_v3.an_lt_3_addr,
				       (uintptr_t)&regs->an_lt[adapter->curr_lt_unit].data);
			val = al_mod_reg_read32(&adapter->mac_regs_base->gen_v3.an_lt_3_data);
			break;
		default:
			al_mod_err("%s: Unknown Lane %d\n", __func__, lane);
			return 0;
		}
	} else if (adapter->rev_id >= AL_ETH_REV_ID_4) {
		/* Get pointer to mac_v4_regs_base inside mac_obj */
		struct al_mod_eth_mac_obj *obj = &adapter->mac_obj;

		uint32_t *an_lt_addr = NULL;
		uint32_t *an_lt_data = NULL;

		al_mod_eth_kr_an_get_indir_addr_by_lane(obj, obj->serdes_lane,
			&an_lt_addr, &an_lt_data);

		reg_addr = al_mod_eth_an_lt_regs_addr[reg_id][AL_ETH_LT_UNIT_REV_2];

		/** AN / LT uses two layers of indirection */
		al_mod_reg_write32(an_lt_addr,
			(uintptr_t) al_mod_offsetof(struct al_mod_an_lt_wrapper_regs,
			an_lt[adapter->curr_lt_unit].addr));
		al_mod_reg_write32(an_lt_data, reg_addr);

		al_mod_reg_write32(an_lt_addr,
			(uintptr_t) al_mod_offsetof(struct al_mod_an_lt_wrapper_regs,
			an_lt[adapter->curr_lt_unit].data));
		val = al_mod_reg_read32(an_lt_data);
	} else {
		al_mod_err("%s: No support for Ethernet Rev id %d\n", __func__, adapter->rev_id);
		return -EPERM;
	}

	al_mod_dbg("[%s]: %s - (%s) lane %d, reg %d, val 0x%x\n", adapter->name, __func__,
		(an_lt == AL_ETH_AN_REGS) ? "AN" : "LT", lane, reg_addr, val);

	return (uint16_t)val;
}

static void al_mod_eth_an_lt_reg_write(
			struct al_mod_hal_eth_adapter *adapter,
			enum al_mod_eth_an_lt_regs_ids reg_id,
			enum al_mod_eth_an_lt_regs an_lt,
			enum al_mod_eth_an_lt_lane lane,
			uint16_t val)
{
	uint16_t reg_addr;

	if (adapter->rev_id < AL_ETH_REV_ID_3) {
		reg_addr = al_mod_eth_an_lt_regs_addr[reg_id][AL_ETH_LT_UNIT_REV_1];
		if (an_lt == AL_ETH_AN_REGS) {
			al_mod_reg_write32(&adapter->mac_regs_base->kr.an_addr, reg_addr);
			al_mod_reg_write32(&adapter->mac_regs_base->kr.an_data, val);
		} else {
			al_mod_reg_write32(&adapter->mac_regs_base->kr.pma_addr, reg_addr);
			al_mod_reg_write32(&adapter->mac_regs_base->kr.pma_data, val);
		}
	} else if (adapter->rev_id == AL_ETH_REV_ID_3) {
		struct al_mod_an_lt_wrapper_regs *regs = NULL;

		reg_addr = al_mod_eth_an_lt_regs_addr[reg_id][AL_ETH_LT_UNIT_REV_2];

		switch (lane) {
		case AL_ETH_AN__LT_LANE_0:
			al_mod_reg_write32(&adapter->mac_regs_base->gen_v3.an_lt_0_addr,
				       (uintptr_t)&regs->an_lt[adapter->curr_lt_unit].addr);
			al_mod_reg_write32(&adapter->mac_regs_base->gen_v3.an_lt_0_data,
				       reg_addr);

			al_mod_reg_write32(&adapter->mac_regs_base->gen_v3.an_lt_0_addr,
				       (uintptr_t)&regs->an_lt[adapter->curr_lt_unit].data);
			al_mod_reg_write32(&adapter->mac_regs_base->gen_v3.an_lt_0_data,
				       val);
			break;
		case AL_ETH_AN__LT_LANE_1:
			al_mod_reg_write32(&adapter->mac_regs_base->gen_v3.an_lt_1_addr,
				       (uintptr_t)&regs->an_lt[adapter->curr_lt_unit].addr);
			al_mod_reg_write32(&adapter->mac_regs_base->gen_v3.an_lt_1_data,
				       reg_addr);

			al_mod_reg_write32(&adapter->mac_regs_base->gen_v3.an_lt_1_addr,
				       (uintptr_t)&regs->an_lt[adapter->curr_lt_unit].data);
			al_mod_reg_write32(&adapter->mac_regs_base->gen_v3.an_lt_1_data,
				       val);
			break;
		case AL_ETH_AN__LT_LANE_2:
			al_mod_reg_write32(&adapter->mac_regs_base->gen_v3.an_lt_2_addr,
				       (uintptr_t)&regs->an_lt[adapter->curr_lt_unit].addr);
			al_mod_reg_write32(&adapter->mac_regs_base->gen_v3.an_lt_2_data,
				       reg_addr);

			al_mod_reg_write32(&adapter->mac_regs_base->gen_v3.an_lt_2_addr,
				       (uintptr_t)&regs->an_lt[adapter->curr_lt_unit].data);
			al_mod_reg_write32(&adapter->mac_regs_base->gen_v3.an_lt_2_data,
				       val);
			break;
		case AL_ETH_AN__LT_LANE_3:
			al_mod_reg_write32(&adapter->mac_regs_base->gen_v3.an_lt_3_addr,
				       (uintptr_t)&regs->an_lt[adapter->curr_lt_unit].addr);
			al_mod_reg_write32(&adapter->mac_regs_base->gen_v3.an_lt_3_data,
				       reg_addr);

			al_mod_reg_write32(&adapter->mac_regs_base->gen_v3.an_lt_3_addr,
				       (uintptr_t)&regs->an_lt[adapter->curr_lt_unit].data);
			al_mod_reg_write32(&adapter->mac_regs_base->gen_v3.an_lt_3_data,
				       val);
			break;
		default:
			al_mod_err("%s: Unknown Lane %d\n", __func__, lane);
			return;
		}
	} else if (adapter->rev_id >= AL_ETH_REV_ID_4) {
		/* Get pointer to mac_v4_regs_base inside mac_obj */
		struct al_mod_eth_mac_obj *obj = &adapter->mac_obj;
		uint32_t *an_lt_addr = NULL;
		uint32_t *an_lt_data = NULL;

		al_mod_eth_kr_an_get_indir_addr_by_lane(obj, obj->serdes_lane,
			&an_lt_addr, &an_lt_data);

		reg_addr = al_mod_eth_an_lt_regs_addr[reg_id][AL_ETH_LT_UNIT_REV_2];

		/** AN / LT uses two layers of indirection */
		al_mod_reg_write32(an_lt_addr,
			(uintptr_t) al_mod_offsetof(struct al_mod_an_lt_wrapper_regs,
			an_lt[adapter->curr_lt_unit].addr));
		al_mod_reg_write32(an_lt_data, reg_addr);

		al_mod_reg_write32(an_lt_addr,
			(uintptr_t) al_mod_offsetof(struct al_mod_an_lt_wrapper_regs,
			an_lt[adapter->curr_lt_unit].data));
		al_mod_reg_write32(an_lt_data, val);
	} else
		al_mod_err("%s: No support for Ethernet Rev id %d\n", __func__, adapter->rev_id);

	al_mod_dbg("[%s]: %s - (%s) lane %d, reg %d, val 0x%x\n", adapter->name, __func__,
		(an_lt == AL_ETH_AN_REGS) ? "AN" : "LT", lane, reg_addr, val);
}

static void al_mod_eth_an_lt_unit_config(struct al_mod_hal_eth_adapter *adapter)
{
	uint32_t *an_lt_data, *an_lt_addr;
	struct al_mod_an_lt_wrapper_regs *regs = NULL;
	struct al_mod_eth_mac_obj *obj = &adapter->mac_obj;
	struct al_mod_eth_mac_v4_regs *mac_regs_base = obj->mac_regs_base;

	const int eth_rev_id = adapter->rev_id;
	int an_lt_unit;
	al_mod_bool dme_ds_en =
		adapter->mac_obj.dme_conf ? adapter->mac_obj.dme_conf->en_override : AL_FALSE;

	/* Default is to bypass AN_LT for all lanes, both directions */
	uint32_t cfg_lane_0 = (AN_LT_WRAPPER_GEN_CFG_BYPASS_RX | AN_LT_WRAPPER_GEN_CFG_BYPASS_TX);
	uint32_t cfg_lane_1 = (AN_LT_WRAPPER_GEN_CFG_BYPASS_RX | AN_LT_WRAPPER_GEN_CFG_BYPASS_TX);
	uint32_t cfg_lane_2 = (AN_LT_WRAPPER_GEN_CFG_BYPASS_RX | AN_LT_WRAPPER_GEN_CFG_BYPASS_TX);
	uint32_t cfg_lane_3 = (AN_LT_WRAPPER_GEN_CFG_BYPASS_RX | AN_LT_WRAPPER_GEN_CFG_BYPASS_TX);

	/* Based on mode, determine config reg values */
	switch (adapter->mac_mode) {
	case AL_ETH_MAC_MODE_10GbE_Serial:
		/* Disables bypass from above */
		cfg_lane_0 = 0;
		/* Configure TX & RX for 20bit parallel */
		an_lt_unit = (eth_rev_id >= AL_ETH_REV_ID_4) ?
			AL_ETH_V4_AN_LT_UNIT_20_BIT : AL_ETH_AN_LT_UNIT_20_BIT;
		AL_REG_FIELD_SET(cfg_lane_0,
			AN_LT_WRAPPER_GEN_CFG_AN_LT_SEL_RX_MASK,
			AN_LT_WRAPPER_GEN_CFG_AN_LT_SEL_RX_SHIFT,
			an_lt_unit);
		AL_REG_FIELD_SET(cfg_lane_0,
			AN_LT_WRAPPER_GEN_CFG_AN_LT_SEL_TX_MASK,
			AN_LT_WRAPPER_GEN_CFG_AN_LT_SEL_TX_SHIFT,
			an_lt_unit);
		adapter->curr_lt_unit = an_lt_unit;
		break;
	case AL_ETH_MAC_MODE_KR_LL_25G:
	case AL_ETH_MAC_MODE_XLG_LL_25G:
		cfg_lane_0 = 0;
		if (eth_rev_id < AL_ETH_REV_ID_4)
			an_lt_unit = AL_ETH_AN_LT_UNIT_32_BIT;
		else {
			/* Use Instance 3 if DME_DS is enabled */
			an_lt_unit = (dme_ds_en) ?
				AL_ETH_V4_AN_LT_UNIT_40_BIT_DME_DS_EN :
				AL_ETH_V4_AN_LT_UNIT_40_BIT;
		}
		AL_REG_FIELD_SET(cfg_lane_0,
			AN_LT_WRAPPER_GEN_CFG_AN_LT_SEL_RX_MASK,
			AN_LT_WRAPPER_GEN_CFG_AN_LT_SEL_RX_SHIFT,
			an_lt_unit);
		AL_REG_FIELD_SET(cfg_lane_0,
			AN_LT_WRAPPER_GEN_CFG_AN_LT_SEL_TX_MASK,
			AN_LT_WRAPPER_GEN_CFG_AN_LT_SEL_TX_SHIFT,
			an_lt_unit);

		adapter->curr_lt_unit = AL_ETH_V4_AN_LT_UNIT_40_BIT;
		break;
	case AL_ETH_MAC_MODE_XLG_LL_40G:
		cfg_lane_0 = 0;
		an_lt_unit = (eth_rev_id >= AL_ETH_REV_ID_4) ?
			AL_ETH_V4_AN_LT_UNIT_20_BIT : AL_ETH_AN_LT_UNIT_16_BIT;
		AL_REG_FIELD_SET(cfg_lane_0,
			AN_LT_WRAPPER_GEN_CFG_AN_LT_SEL_RX_MASK,
			AN_LT_WRAPPER_GEN_CFG_AN_LT_SEL_RX_SHIFT,
			an_lt_unit);
		AL_REG_FIELD_SET(cfg_lane_0,
			AN_LT_WRAPPER_GEN_CFG_AN_LT_SEL_TX_MASK,
			AN_LT_WRAPPER_GEN_CFG_AN_LT_SEL_TX_SHIFT,
			an_lt_unit);

		cfg_lane_1 = 0;
		AL_REG_FIELD_SET(cfg_lane_1,
			AN_LT_WRAPPER_GEN_CFG_AN_LT_SEL_RX_MASK,
			AN_LT_WRAPPER_GEN_CFG_AN_LT_SEL_RX_SHIFT,
			an_lt_unit);
		AL_REG_FIELD_SET(cfg_lane_1,
			AN_LT_WRAPPER_GEN_CFG_AN_LT_SEL_TX_MASK,
			AN_LT_WRAPPER_GEN_CFG_AN_LT_SEL_TX_SHIFT,
			an_lt_unit);

		cfg_lane_2 = 0;
		AL_REG_FIELD_SET(cfg_lane_2,
			AN_LT_WRAPPER_GEN_CFG_AN_LT_SEL_RX_MASK,
			AN_LT_WRAPPER_GEN_CFG_AN_LT_SEL_RX_SHIFT,
			an_lt_unit);
		AL_REG_FIELD_SET(cfg_lane_2,
			AN_LT_WRAPPER_GEN_CFG_AN_LT_SEL_TX_MASK,
			AN_LT_WRAPPER_GEN_CFG_AN_LT_SEL_TX_SHIFT,
			an_lt_unit);

		cfg_lane_3 = 0;
		AL_REG_FIELD_SET(cfg_lane_3,
			AN_LT_WRAPPER_GEN_CFG_AN_LT_SEL_RX_MASK,
			AN_LT_WRAPPER_GEN_CFG_AN_LT_SEL_RX_SHIFT,
			an_lt_unit);
		AL_REG_FIELD_SET(cfg_lane_3,
			AN_LT_WRAPPER_GEN_CFG_AN_LT_SEL_TX_MASK,
			AN_LT_WRAPPER_GEN_CFG_AN_LT_SEL_TX_SHIFT,
			an_lt_unit);

		adapter->curr_lt_unit = an_lt_unit;
		break;
	case AL_ETH_MAC_MODE_XLG_LL_50G:
		cfg_lane_0 = 0;
		if (eth_rev_id < AL_ETH_REV_ID_4)
			an_lt_unit = AL_ETH_AN_LT_UNIT_32_BIT;
		else {
			/* Use Instance 3 if DME_DS is enabled */
			an_lt_unit = (dme_ds_en) ?
				AL_ETH_V4_AN_LT_UNIT_40_BIT_DME_DS_EN :
				AL_ETH_V4_AN_LT_UNIT_40_BIT;
		}
		AL_REG_FIELD_SET(cfg_lane_0,
			AN_LT_WRAPPER_GEN_CFG_AN_LT_SEL_RX_MASK,
			AN_LT_WRAPPER_GEN_CFG_AN_LT_SEL_RX_SHIFT,
			an_lt_unit);
		AL_REG_FIELD_SET(cfg_lane_0,
			AN_LT_WRAPPER_GEN_CFG_AN_LT_SEL_TX_MASK,
			AN_LT_WRAPPER_GEN_CFG_AN_LT_SEL_TX_SHIFT,
			an_lt_unit);

		cfg_lane_1 = 0;
		AL_REG_FIELD_SET(cfg_lane_1,
			AN_LT_WRAPPER_GEN_CFG_AN_LT_SEL_RX_MASK,
			AN_LT_WRAPPER_GEN_CFG_AN_LT_SEL_RX_SHIFT,
			an_lt_unit);
		AL_REG_FIELD_SET(cfg_lane_1,
			AN_LT_WRAPPER_GEN_CFG_AN_LT_SEL_TX_MASK,
			AN_LT_WRAPPER_GEN_CFG_AN_LT_SEL_TX_SHIFT,
			an_lt_unit);

		adapter->curr_lt_unit = AL_ETH_V4_AN_LT_UNIT_40_BIT;
		break;
	case AL_ETH_MAC_MODE_CG_100G:
		cfg_lane_0 = 0;
		/* Use Instance 3 if DME_DS is enabled */
		an_lt_unit = (dme_ds_en) ?
			AL_ETH_V4_AN_LT_UNIT_40_BIT_DME_DS_EN :
			AL_ETH_V4_AN_LT_UNIT_40_BIT;

		AL_REG_FIELD_SET(cfg_lane_0,
			AN_LT_WRAPPER_GEN_CFG_AN_LT_SEL_RX_MASK,
			AN_LT_WRAPPER_GEN_CFG_AN_LT_SEL_RX_SHIFT,
			an_lt_unit);
		AL_REG_FIELD_SET(cfg_lane_0,
			AN_LT_WRAPPER_GEN_CFG_AN_LT_SEL_TX_MASK,
			AN_LT_WRAPPER_GEN_CFG_AN_LT_SEL_TX_SHIFT,
			an_lt_unit);

		cfg_lane_1 = cfg_lane_0;
		cfg_lane_2 = cfg_lane_0;
		cfg_lane_3 = cfg_lane_0;

		adapter->curr_lt_unit = AL_ETH_V4_AN_LT_UNIT_40_BIT;
		break;
	default:
		al_mod_err("%s: Unknown mac_mode\n", __func__);
		return;
	}

	if (eth_rev_id >= AL_ETH_REV_ID_4) {
		/* Write to cfg reg of all an_lt wrappers to complete init */

		/*
		BOZO: In the new scheme, each channel will "see" all 4 an_lt reg sets,
		but only those configured in macs_common_init (via an_lt_ipregs_sel)
		will be connected.
		The current adapter needs to understand which mode it is in, and then
		write to the an_lt_x_addr/data reg pairs of all participating lanes.
		Because the mac_mode is a factor, it may make sense to  leverage the
		case statement above for performing these writes
		*/

		al_mod_eth_kr_an_get_indir_addr_by_lane(obj, adapter->serdes_lane, &an_lt_addr,
			&an_lt_data);
		switch (adapter->mac_mode) {
		case AL_ETH_MAC_MODE_10GbE_Serial:
			al_mod_reg_write32(an_lt_addr, (uintptr_t)&regs->gen.cfg);
			al_mod_reg_write32(an_lt_data, cfg_lane_0);
			break;
		case AL_ETH_MAC_MODE_XLG_LL_25G:
			al_mod_reg_write32(an_lt_addr, (uintptr_t)&regs->gen.cfg);
			al_mod_reg_write32(an_lt_data, cfg_lane_0);
			break;
		case AL_ETH_MAC_MODE_XLG_LL_40G:
			if (adapter->serdes_lane == 0) {
				al_mod_reg_write32(an_lt_addr, (uintptr_t)&regs->gen.cfg);
				al_mod_reg_write32(an_lt_data, cfg_lane_0);
			} else
				al_mod_err("%s: Illegal lane (%d) for AN config w/ mac mode %s (%d)\n",
					__func__, adapter->serdes_lane,
					al_mod_eth_mac_mode_str(adapter->mac_mode), adapter->mac_mode);
			break;
		case AL_ETH_MAC_MODE_XLG_LL_50G:
			if (adapter->serdes_lane == 0 || adapter->serdes_lane == 2) {
				al_mod_reg_write32(an_lt_addr, (uintptr_t)&regs->gen.cfg);
				al_mod_reg_write32(an_lt_data, cfg_lane_0);
			} else
				al_mod_err("%s: Illegal lane (%d) for AN config w/ mac mode %s (%d)\n",
					__func__, adapter->serdes_lane,
					al_mod_eth_mac_mode_str(adapter->mac_mode), adapter->mac_mode);
			break;
		case AL_ETH_MAC_MODE_CG_100G:
			if (adapter->serdes_lane == 0) {
				al_mod_reg_write32(&mac_regs_base->mtip.an_lt_0_addr,
					(uintptr_t)&regs->gen.cfg);
				al_mod_reg_write32(&mac_regs_base->mtip.an_lt_0_data, cfg_lane_0);
			} else
				al_mod_err("%s: Illegal lane (%d) for AN config w/ mac mode %s (%d)\n",
					__func__, adapter->serdes_lane,
					al_mod_eth_mac_mode_str(adapter->mac_mode), adapter->mac_mode);
			break;

		default:
			al_mod_err("%s: Unknown mac_mode %d\n", __func__, adapter->mac_mode);
			return;
		}
	} else {
		al_mod_reg_write32(&adapter->mac_regs_base->gen_v3.an_lt_0_addr,
			(uintptr_t)&regs->gen.cfg);
		al_mod_reg_write32(&adapter->mac_regs_base->gen_v3.an_lt_0_data,
			cfg_lane_0);
		al_mod_reg_write32(&adapter->mac_regs_base->gen_v3.an_lt_1_addr,
			(uintptr_t)&regs->gen.cfg);
		al_mod_reg_write32(&adapter->mac_regs_base->gen_v3.an_lt_1_data,
			cfg_lane_1);
		al_mod_reg_write32(&adapter->mac_regs_base->gen_v3.an_lt_2_addr,
			(uintptr_t)&regs->gen.cfg);
		al_mod_reg_write32(&adapter->mac_regs_base->gen_v3.an_lt_2_data,
			cfg_lane_2);
		al_mod_reg_write32(&adapter->mac_regs_base->gen_v3.an_lt_3_addr,
			(uintptr_t)&regs->gen.cfg);
		al_mod_reg_write32(&adapter->mac_regs_base->gen_v3.an_lt_3_data,
			cfg_lane_3);
	}
}

void al_mod_eth_lp_coeff_up_get(
			struct al_mod_hal_eth_adapter *adapter,
			enum al_mod_eth_an_lt_lane lane,
			struct al_mod_eth_kr_coef_up_data *lpcoeff)
{
	uint16_t reg;

	reg = al_mod_eth_an_lt_reg_read(adapter, AL_ETH_KR_PMD_LP_COEF_UP, AL_ETH_LT_REGS, lane);

	lpcoeff->preset =
		(AL_REG_BIT_GET(
			reg, AL_ETH_KR_PMD_LP_COEF_UP_PRESET_SHIFT) != 0);

	lpcoeff->initialize =
		(AL_REG_BIT_GET(
			reg, AL_ETH_KR_PMD_LP_COEF_UP_INITIALIZE_SHIFT) != 0);

	lpcoeff->c_minus = AL_REG_FIELD_GET(reg,
					AL_ETH_KR_PMD_LP_COEF_UP_MINUS_MASK,
					AL_ETH_KR_PMD_LP_COEF_UP_MINUS_SHIFT);

	lpcoeff->c_zero = AL_REG_FIELD_GET(reg,
					AL_ETH_KR_PMD_LP_COEF_UP_ZERO_MASK,
					AL_ETH_KR_PMD_LP_COEF_UP_ZERO_SHIFT);

	lpcoeff->c_plus = AL_REG_FIELD_GET(reg,
					AL_ETH_KR_PMD_LP_COEF_UP_PLUS_MASK,
					AL_ETH_KR_PMD_LP_COEF_UP_PLUS_SHIFT);
}

void al_mod_eth_lp_status_report_get(
			struct al_mod_hal_eth_adapter *adapter,
			enum al_mod_eth_an_lt_lane lane,
			struct al_mod_eth_kr_status_report_data *status)
{
	uint16_t reg;

	reg = al_mod_eth_an_lt_reg_read(adapter, AL_ETH_KR_PMD_LP_STATUS_REPORT, AL_ETH_LT_REGS, lane);

	status->c_minus = AL_REG_FIELD_GET(reg,
				    AL_ETH_KR_PMD_LP_STATUS_REPORT_MINUS_MASK,
				    AL_ETH_KR_PMD_LP_STATUS_REPORT_MINUS_SHIFT);

	status->c_zero = AL_REG_FIELD_GET(reg,
				   AL_ETH_KR_PMD_LP_STATUS_REPORT_ZERO_MASK,
				   AL_ETH_KR_PMD_LP_STATUS_REPORT_ZERO_SHIFT);

	status->c_plus = AL_REG_FIELD_GET(reg,
				   AL_ETH_KR_PMD_LP_STATUS_REPORT_PLUS_MASK,
				   AL_ETH_KR_PMD_LP_STATUS_REPORT_PLUS_SHIFT);

	status->receiver_ready =
		(AL_REG_BIT_GET(
			reg, AL_ETH_KR_PMD_LP_STATUS_RECEIVER_READY_SHIFT) != 0);

}

void al_mod_eth_ld_coeff_up_set(
			struct al_mod_hal_eth_adapter *adapter,
			enum al_mod_eth_an_lt_lane lane,
			struct al_mod_eth_kr_coef_up_data *ldcoeff)
{
	uint16_t reg = 0;

	if (ldcoeff->preset)
		AL_REG_BIT_SET(reg, AL_ETH_KR_PMD_LD_COEF_UP_PRESET_SHIFT);

	if (ldcoeff->initialize)
		AL_REG_BIT_SET(reg, AL_ETH_KR_PMD_LD_COEF_UP_INITIALIZE_SHIFT);

	AL_REG_FIELD_SET(reg,
			 AL_ETH_KR_PMD_LD_COEF_UP_MINUS_MASK,
			 AL_ETH_KR_PMD_LD_COEF_UP_MINUS_SHIFT,
			 ldcoeff->c_minus);

	AL_REG_FIELD_SET(reg,
			 AL_ETH_KR_PMD_LD_COEF_UP_ZERO_MASK,
			 AL_ETH_KR_PMD_LD_COEF_UP_ZERO_SHIFT,
			 ldcoeff->c_zero);

	AL_REG_FIELD_SET(reg,
			 AL_ETH_KR_PMD_LD_COEF_UP_PLUS_MASK,
			 AL_ETH_KR_PMD_LD_COEF_UP_PLUS_SHIFT,
			 ldcoeff->c_plus);

	al_mod_eth_an_lt_reg_write(adapter, AL_ETH_KR_PMD_LD_COEF_UP, AL_ETH_LT_REGS, lane, reg);
}

void al_mod_eth_ld_status_report_set(
			struct al_mod_hal_eth_adapter *adapter,
			enum al_mod_eth_an_lt_lane lane,
			struct al_mod_eth_kr_status_report_data *status)
{
	uint16_t reg = 0;

	AL_REG_FIELD_SET(reg,
			 AL_ETH_KR_PMD_LD_STATUS_REPORT_MINUS_MASK,
			 AL_ETH_KR_PMD_LD_STATUS_REPORT_MINUS_SHIFT,
			 status->c_minus);

	AL_REG_FIELD_SET(reg,
			 AL_ETH_KR_PMD_LD_STATUS_REPORT_ZERO_MASK,
			 AL_ETH_KR_PMD_LD_STATUS_REPORT_ZERO_SHIFT,
			 status->c_zero);

	AL_REG_FIELD_SET(reg,
			 AL_ETH_KR_PMD_LD_STATUS_REPORT_PLUS_MASK,
			 AL_ETH_KR_PMD_LD_STATUS_REPORT_PLUS_SHIFT,
			 status->c_plus);

	if (status->receiver_ready)
		AL_REG_BIT_SET(reg,
			AL_ETH_KR_PMD_LD_STATUS_REPORT_RECEIVER_READY_SHIFT);

	al_mod_eth_an_lt_reg_write(adapter, AL_ETH_KR_PMD_LD_STATUS_REPORT, AL_ETH_LT_REGS, lane, reg);
}

al_mod_bool al_mod_eth_kr_receiver_frame_lock_get(struct al_mod_hal_eth_adapter *adapter,
					  enum al_mod_eth_an_lt_lane lane)
{
	uint16_t reg;

	reg = al_mod_eth_an_lt_reg_read(adapter, AL_ETH_KR_PMD_STATUS, AL_ETH_LT_REGS, lane);

	return (AL_REG_BIT_GET(reg,
			AL_ETH_KR_PMD_STATUS_RECEIVER_FRAME_LOCK_SHIFT) != 0);
}

al_mod_bool al_mod_eth_kr_startup_proto_prog_get(struct al_mod_hal_eth_adapter *adapter,
					 enum al_mod_eth_an_lt_lane lane)
{
	uint16_t reg;

	reg = al_mod_eth_an_lt_reg_read(adapter, AL_ETH_KR_PMD_STATUS, AL_ETH_LT_REGS, lane);

	return (AL_REG_BIT_GET(
		reg, AL_ETH_KR_PMD_STATUS_RECEIVER_START_UP_PROTO_PROG_SHIFT) != 0);
}

al_mod_bool al_mod_eth_kr_training_status_fail_get(struct al_mod_hal_eth_adapter *adapter,
					   enum al_mod_eth_an_lt_lane lane)
{
	uint16_t reg;

	reg = al_mod_eth_an_lt_reg_read(adapter, AL_ETH_KR_PMD_STATUS, AL_ETH_LT_REGS, lane);

	return (AL_REG_BIT_GET(reg, AL_ETH_KR_PMD_STATUS_FAILURE_SHIFT) != 0);
}

void al_mod_eth_receiver_ready_set(struct al_mod_hal_eth_adapter *adapter,
			       enum al_mod_eth_an_lt_lane lane)
{
	al_mod_eth_an_lt_reg_write(adapter, AL_ETH_KR_PMD_STATUS, AL_ETH_LT_REGS, lane, 1);
}

/*************************** auto negotiation *********************************/
static int al_mod_eth_kr_an_validate_adv(struct al_mod_hal_eth_adapter *adapter,
				     struct al_mod_eth_an_adv *an_adv)
{
	al_mod_assert(adapter);

	if (an_adv == NULL)
		return 0;

	if (an_adv->selector_field != 1) {
		al_mod_err("[%s]: %s failed on selector_field (%d)\n",
			adapter->name, __func__, an_adv->selector_field);
		return -EINVAL;
	}

	if (an_adv->capability & AL_BIT(2)) {
		al_mod_err("[%s]: %s failed on capability bit 2 (%d)\n",
			adapter->name, __func__, an_adv->capability);
		return -EINVAL;
	}

	if (an_adv->remote_fault) {
		al_mod_err("[%s]: %s failed on remote_fault (%d)\n",
			adapter->name, __func__, an_adv->remote_fault);
		return -EINVAL;
	}

	if (an_adv->acknowledge) {
		al_mod_err("[%s]: %s failed on acknowledge (%d)\n",
			adapter->name, __func__, an_adv->acknowledge);
		return -EINVAL;
	}

	return 0;
}

static int al_mod_eth_kr_an_write_adv(struct al_mod_hal_eth_adapter *adapter,
			       struct al_mod_eth_an_adv *an_adv)
{
	uint16_t	reg;

	if(an_adv == NULL)
		return 0;

	reg = 0;
	AL_REG_FIELD_SET(reg, AL_ETH_KR_AN_ADV1_SEL_FIELD_MASK,
			 AL_ETH_KR_AN_ADV1_SEL_FIELD_SHIFT,
			 an_adv->selector_field);

	AL_REG_FIELD_SET(reg, AL_ETH_KR_AN_ADV1_ECHOED_NONCE_MASK,
			 AL_ETH_KR_AN_ADV1_ECHOED_NONCE_SHIFT,
			 an_adv->echoed_nonce);

	AL_REG_FIELD_SET(reg, AL_ETH_KR_AN_ADV1_CAPABILITY_MASK,
			 AL_ETH_KR_AN_ADV1_CAPABILITY_SHIFT,
			 an_adv->capability);

	AL_REG_BIT_VAL_SET(reg, AL_ETH_KR_AN_ADV1_REM_FAULT_SHIFT,
			   an_adv->remote_fault);

	AL_REG_BIT_VAL_SET(reg, AL_ETH_KR_AN_ADV1_ACK_SHIFT,
			   an_adv->acknowledge);

	AL_REG_BIT_VAL_SET(reg, AL_ETH_KR_AN_ADV1_NEXT_PAGE_SHIFT,
			   an_adv->next_page);

	al_mod_eth_an_lt_reg_write(adapter, AL_ETH_KR_AN_ADV0, AL_ETH_AN_REGS,
			       adapter->serdes_lane, reg);

	reg = 0;
	AL_REG_FIELD_SET(reg, AL_ETH_KR_AN_ADV2_TX_NONCE_MASK,
			 AL_ETH_KR_AN_ADV2_TX_NONCE_SHIFT,
			 an_adv->transmitted_nonce);

	AL_REG_FIELD_SET(reg, AL_ETH_KR_AN_ADV2_TECH_MASK,
			 AL_ETH_KR_AN_ADV2_TECH_SHIFT,
			 an_adv->technology);

	al_mod_eth_an_lt_reg_write(adapter, AL_ETH_KR_AN_ADV1, AL_ETH_AN_REGS,
			       adapter->serdes_lane, reg);

	reg = 0;
	AL_REG_FIELD_SET(reg, AL_ETH_KR_AN_ADV3_TECH_MASK,
			 AL_ETH_KR_AN_ADV3_TECH_SHIFT,
			 an_adv->technology >> 11);

	AL_REG_FIELD_SET(reg, AL_ETH_KR_AN_ADV3_FEC_MASK,
			 AL_ETH_KR_AN_ADV3_FEC_SHIFT,
			 an_adv->fec_capability);

	al_mod_eth_an_lt_reg_write(adapter, AL_ETH_KR_AN_ADV2, AL_ETH_AN_REGS,
			       adapter->serdes_lane, reg);

	return 0;
}

void al_mod_eth_kr_an_read_adv(struct al_mod_hal_eth_adapter *adapter,
			   struct al_mod_eth_an_adv *an_adv)
{
	int16_t	reg;

	al_mod_assert(an_adv != NULL);


	reg = al_mod_eth_an_lt_reg_read(adapter, AL_ETH_KR_AN_REM_ADV0,
				    AL_ETH_AN_REGS, adapter->serdes_lane);

	an_adv->selector_field = AL_REG_FIELD_GET(reg,
					AL_ETH_KR_AN_ADV1_SEL_FIELD_MASK,
					AL_ETH_KR_AN_ADV1_SEL_FIELD_SHIFT);

	an_adv->echoed_nonce = AL_REG_FIELD_GET(reg,
					AL_ETH_KR_AN_ADV1_ECHOED_NONCE_MASK,
					AL_ETH_KR_AN_ADV1_ECHOED_NONCE_SHIFT);

	an_adv->capability = AL_REG_FIELD_GET(reg,
					AL_ETH_KR_AN_ADV1_CAPABILITY_MASK,
					AL_ETH_KR_AN_ADV1_CAPABILITY_SHIFT);

	an_adv->remote_fault = AL_REG_BIT_GET(reg,
					AL_ETH_KR_AN_ADV1_REM_FAULT_SHIFT);

	an_adv->acknowledge = AL_REG_BIT_GET(reg,
					AL_ETH_KR_AN_ADV1_ACK_SHIFT);

	an_adv->next_page = AL_REG_BIT_GET(reg,
					AL_ETH_KR_AN_ADV1_NEXT_PAGE_SHIFT);


	reg = al_mod_eth_an_lt_reg_read(adapter, AL_ETH_KR_AN_REM_ADV1,
				    AL_ETH_AN_REGS, adapter->serdes_lane);

	an_adv->transmitted_nonce = AL_REG_FIELD_GET(reg,
					 AL_ETH_KR_AN_ADV2_TX_NONCE_MASK,
					 AL_ETH_KR_AN_ADV2_TX_NONCE_SHIFT);

	an_adv->technology = AL_REG_FIELD_GET(reg,
					 AL_ETH_KR_AN_ADV2_TECH_MASK,
					 AL_ETH_KR_AN_ADV2_TECH_SHIFT);


	reg = al_mod_eth_an_lt_reg_read(adapter, AL_ETH_KR_AN_REM_ADV2,
				    AL_ETH_AN_REGS, adapter->serdes_lane);

	an_adv->technology |= (AL_REG_FIELD_GET(reg,
					AL_ETH_KR_AN_ADV3_TECH_MASK,
					AL_ETH_KR_AN_ADV3_TECH_SHIFT)  << 11);

	an_adv->fec_capability = AL_REG_FIELD_GET(reg,
					 AL_ETH_KR_AN_ADV3_FEC_MASK,
					 AL_ETH_KR_AN_ADV3_FEC_SHIFT);
}

int al_mod_eth_kr_next_page_read(struct al_mod_hal_eth_adapter *adapter,
			     struct al_mod_eth_an_np *np)
{
	uint16_t reg;

	reg = al_mod_eth_an_lt_reg_read(adapter,
		AL_ETH_KR_AN_REM_XNP_ADV0,
		AL_ETH_AN_REGS,
		adapter->serdes_lane);

	np->unformatted_code_field = AL_REG_FIELD_GET(reg, AL_ETH_KR_AN_NP_ADV1_DATA1_MASK,
		AL_ETH_KR_AN_NP_ADV1_DATA1_SHIFT);

	np->toggle = AL_REG_FIELD_GET(reg, AL_ETH_KR_AN_NP_ADV1_TOGGLE_MASK,
		AL_ETH_KR_AN_NP_ADV1_TOGGLE_SHIFT);

	np->ack2 = AL_REG_FIELD_GET(reg, AL_ETH_KR_AN_NP_ADV1_ACK2_MASK,
		AL_ETH_KR_AN_NP_ADV1_ACK2_SHIFT);

	np->msg_page = AL_REG_FIELD_GET(reg, AL_ETH_KR_AN_NP_ADV1_MSG_PAGE_MASK,
		AL_ETH_KR_AN_NP_ADV1_MSG_PAGE_SHIFT);

	np->next_page = AL_REG_FIELD_GET(reg, AL_ETH_KR_AN_NP_ADV1_NP_MASK,
		AL_ETH_KR_AN_NP_ADV1_NP_SHIFT);

	/** In Ethernet V4 we can signal when there are no more pages */
	if (adapter->rev_id >= AL_ETH_REV_ID_4) {
		np->ack_no_more_pages = AL_REG_FIELD_GET(reg, AL_ETH_KR_AN_NP_ADV1_ACK_MASK,
			AL_ETH_KR_AN_NP_ADV1_ACK_SHIFT);
	}

	np->unformatted_code_field1 = al_mod_eth_an_lt_reg_read(adapter,
		AL_ETH_KR_AN_REM_XNP_ADV1,
		AL_ETH_AN_REGS,
		adapter->serdes_lane);
	np->unformatted_code_field2 = al_mod_eth_an_lt_reg_read(adapter,
		AL_ETH_KR_AN_REM_XNP_ADV2,
		AL_ETH_AN_REGS,
		adapter->serdes_lane);

	return 0;
}

int al_mod_eth_kr_next_page_write(struct al_mod_hal_eth_adapter *adapter, struct al_mod_eth_an_np *np)
{
	uint16_t reg = 0;

	AL_REG_FIELD_SET(reg, AL_ETH_KR_AN_NP_ADV1_DATA1_MASK,
		AL_ETH_KR_AN_NP_ADV1_DATA1_SHIFT,
		np->unformatted_code_field);
	AL_REG_FIELD_SET(reg, AL_ETH_KR_AN_NP_ADV1_TOGGLE_MASK,
		AL_ETH_KR_AN_NP_ADV1_TOGGLE_SHIFT,
		np->toggle);
	AL_REG_FIELD_SET(reg, AL_ETH_KR_AN_NP_ADV1_ACK2_MASK,
		AL_ETH_KR_AN_NP_ADV1_ACK2_SHIFT,
		np->ack2);
	AL_REG_FIELD_SET(reg, AL_ETH_KR_AN_NP_ADV1_MSG_PAGE_MASK,
		AL_ETH_KR_AN_NP_ADV1_MSG_PAGE_SHIFT,
		np->msg_page);
	AL_REG_FIELD_SET(reg, AL_ETH_KR_AN_NP_ADV1_NP_MASK,
		AL_ETH_KR_AN_NP_ADV1_NP_SHIFT,
		np->next_page);
	/** In Ethernet V4 we can signal when there are no more pages */
	if (adapter->rev_id >= AL_ETH_REV_ID_4)
		AL_REG_FIELD_SET(reg, AL_ETH_KR_AN_NP_ADV1_ACK_MASK,
			AL_ETH_KR_AN_NP_ADV1_ACK_SHIFT, np->ack_no_more_pages);

	al_mod_eth_an_lt_reg_write(adapter, AL_ETH_KR_AN_XNP_ADV0, AL_ETH_AN_REGS,
		adapter->serdes_lane, reg);
	al_mod_eth_an_lt_reg_write(adapter, AL_ETH_KR_AN_XNP_ADV1, AL_ETH_AN_REGS,
		adapter->serdes_lane, np->unformatted_code_field1);
	al_mod_eth_an_lt_reg_write(adapter, AL_ETH_KR_AN_XNP_ADV2, AL_ETH_AN_REGS,
		adapter->serdes_lane, np->unformatted_code_field2);

	return 0;
}

int al_mod_eth_kr_an_init(struct al_mod_hal_eth_adapter *adapter,
		      struct al_mod_eth_an_adv *an_adv)
{
	int rc;

	if (adapter->mac_obj.dme_conf)
		al_mod_eth_an_lt_dme_downsample_override(adapter, adapter->mac_obj.dme_conf);

	if (adapter->rev_id > AL_ETH_REV_ID_2)
		al_mod_eth_an_lt_unit_config(adapter);
	else {
		al_mod_err("%s: Ethernet adapter rev %d not supported\n", __func__, adapter->rev_id);
		return -EPERM;
	}

	rc = al_mod_eth_kr_an_validate_adv(adapter, an_adv);
	if (rc)
		return rc;

	rc = al_mod_eth_kr_an_write_adv(adapter, an_adv);
	if (rc)
		return rc;

	/* clear status */
	al_mod_eth_an_lt_reg_read(adapter, AL_ETH_KR_AN_STATUS, AL_ETH_AN_REGS, adapter->serdes_lane);

#if (defined(AL_ETH_FAST_AN)) && (AL_ETH_FAST_AN == 1)
	al_mod_eth_an_lt_reg_write(adapter, AL_ETH_KR_AN_MS_CNT, AL_ETH_AN_REGS, adapter->serdes_lane,
		0x0001);
	al_mod_dbg("[%s]: Reduced an_ms_cnt!\n", adapter->name);
#endif

	al_mod_dbg("[%s]: autonegotiation initialized successfully\n", adapter->name);
	return 0;
}

int al_mod_eth_kr_an_start(struct al_mod_hal_eth_adapter *adapter,
		       enum al_mod_eth_an_lt_lane lane,
		       al_mod_bool next_page_enable,
		       al_mod_bool lt_enable)
{
	uint16_t control = AL_ETH_KR_AN_CONTROL_ENABLE | AL_ETH_KR_AN_CONTROL_RESTART;

	al_mod_dbg("Eth [%s]: enable autonegotiation. lt_en %s\n",
		adapter->name, (lt_enable == AL_TRUE) ? "yes" : "no");

	al_mod_eth_an_lt_reg_write(adapter, AL_ETH_KR_PMD_CONTROL, AL_ETH_LT_REGS,
			       lane, AL_BIT(AL_ETH_KR_PMD_CONTROL_RESTART));

	if (next_page_enable == AL_TRUE)
		control |= AL_ETH_KR_AN_CONTROL_NP_ENABLE;

	al_mod_eth_an_lt_reg_write(adapter, AL_ETH_KR_AN_CONTROL, AL_ETH_AN_REGS,
			       lane, control);

	if (lt_enable == AL_TRUE) {
		al_mod_eth_kr_lt_initialize(adapter, lane);
	}

	return 0;
}

void al_mod_eth_kr_an_stop(struct al_mod_hal_eth_adapter *adapter)
{
	al_mod_eth_an_lt_reg_write(adapter, AL_ETH_KR_AN_CONTROL, AL_ETH_AN_REGS,
			       adapter->serdes_lane, 0);
}

void al_mod_eth_kr_an_status_check(struct al_mod_hal_eth_adapter *adapter,
			      al_mod_bool *page_received,
			      al_mod_bool *an_completed,
			      al_mod_bool *error)
{
	uint16_t	reg;

	reg = al_mod_eth_an_lt_reg_read(adapter, AL_ETH_KR_AN_STATUS,
				    AL_ETH_AN_REGS, adapter->serdes_lane);

	if ((reg & AL_ETH_KR_AN_STATUS_CHECK_MASK) !=
				AL_ETH_KR_AN_STATUS_CHECK_NO_ERROR) {
		al_mod_err("[%s]: %s AN_STATUS (0x%x) indicated error\n",
			adapter->name, __func__, reg);

		*error = AL_TRUE;
	}

	if (reg & AL_ETH_KR_AN_STATUS_BASE_PAGE_RECEIVED)
		*page_received = AL_TRUE;
	else
		*page_received = AL_FALSE;

	if (reg & AL_ETH_KR_AN_STATUS_COMPLETED)
		*an_completed = AL_TRUE;
	else
		*an_completed = AL_FALSE;
}

int al_mod_eth_kr_an_wait_done(struct al_mod_hal_eth_adapter *adapter,
	int *an_select)
{
	uint32_t *an_lt_data, *an_lt_addr;
	int reg;
	int timeout = AL_ETH_KR_AN_MPS_CHECK_AN_DONE*1000;

	struct al_mod_eth_mac_obj *obj;
	obj = &adapter->mac_obj;

	al_mod_eth_kr_an_get_indir_addr_by_lane(obj, adapter->serdes_lane, &an_lt_addr, &an_lt_data);

	if (adapter->rev_id < AL_ETH_REV_ID_4) {
		al_mod_dbg("[%s]: For v4 only!", adapter->name);
		return -EINVAL;
	} else {
		do {
			/* Read out status reg for the currently selected instance */
			al_mod_reg_write32(an_lt_addr,
				(uintptr_t) al_mod_offsetof(struct al_mod_an_lt_wrapper_regs,
				an_lt[adapter->curr_lt_unit].status));
			reg = al_mod_reg_read32(an_lt_data);

			/* This sounds like it will be a while.... */
			al_mod_udelay(1);
			timeout -= 1;
			if (timeout <= 0) {
				al_mod_info("%s %s an_done poll failed on timeout\n",
				adapter->name, __func__);
				return -ETIMEDOUT;
			}
		} while (reg & AN_LT_WRAPPER_AN_LT_STATUS_AN_DONE);

		/*  If an_done == 1, double check the an_val / an_status */
		if (!(reg & (AN_LT_WRAPPER_AN_LT_STATUS_AN_VAL |
			AN_LT_WRAPPER_AN_LT_STATUS_AN_STATUS))) {
			al_mod_info("%s %s an_done == 1, but an_val / an_status aren't!\n",
			adapter->name, __func__);
			return -EINVAL;
		}

	/*  If an_done == 1, double check the an_val / an_status */
	if (!(reg & (AN_LT_WRAPPER_AN_LT_STATUS_AN_VAL |
		AN_LT_WRAPPER_AN_LT_STATUS_AN_STATUS))) {
		al_mod_err("%s %s an_done == 1, but an_val / an_status aren't!\n",
			adapter->name, __func__);
		return -EINVAL;
	}

	/* If those are good, an_select is valid - extract it. */
	*an_select = AL_REG_FIELD_GET(reg,
		AN_LT_WRAPPER_AN_LT_CTRL_PHY_LOS_OUT_SEL_MASK,
		AN_LT_WRAPPER_AN_LT_CTRL_PHY_LOS_OUT_SEL_SHIFT);

	al_mod_info("%s %s an_done == 1, saw an_select = %0d\n", adapter->name, __func__, *an_select);
			al_mod_info("%s %s an_done == 1, saw an_select = %0d\n",
			adapter->name, __func__, *an_select);

#if (defined(AL_ETH_FAST_AN)) && (AL_ETH_FAST_AN == 1)
	/** For simulations only */
		al_mod_eth_an_lt_reg_write(adapter, AL_ETH_KR_AN_MS_CNT, AL_ETH_AN_REGS,
			adapter->serdes_lane,
			0x061a);
		al_mod_dbg("[%s]: Restored an_ms_cnt to default value!", adapter->name);
#endif
	}
	return 0;
}

void al_mod_eth_kr_an_link_status_set(struct al_mod_hal_eth_adapter *adapter,
		int link_status)
{
	uint32_t reg, lane;
	struct al_mod_eth_mac_obj *obj = &adapter->mac_obj;
	uintptr_t an_lt_ctrl;
	uint32_t *an_lt_addr, *an_lt_data;

	/* Adapter's serdes lane determines the lane to be accessed from channel */
	lane = obj->serdes_lane;

	/* Read ctrl reg from AN_LT RBA in wrapper*/
	an_lt_ctrl = al_mod_offsetof(struct al_mod_an_lt_wrapper_regs, an_lt[adapter->curr_lt_unit].ctrl);

	al_mod_eth_kr_an_get_indir_addr_by_lane(obj, lane, &an_lt_addr, &an_lt_data);

	al_mod_reg_write32(an_lt_addr, an_lt_ctrl);
	reg = al_mod_reg_read32(an_lt_data);
	if (link_status == 1)
		reg |= AN_LT_WRAPPER_AN_LT_CTRL_LINK_STATUS;
	else
		reg &= ~(AN_LT_WRAPPER_AN_LT_CTRL_LINK_STATUS);

	/* Write back to ctrl reg */
	al_mod_reg_write32(an_lt_addr, an_lt_ctrl);
	al_mod_reg_write32(an_lt_data, reg);
}

/****************************** KR Link Training *****************************/
void al_mod_eth_kr_lt_restart(struct al_mod_hal_eth_adapter *adapter,
			  enum al_mod_eth_an_lt_lane lane)
{
	al_mod_dbg("[%s]: KR LT Restart Link Training.\n", adapter->name);

	al_mod_eth_an_lt_reg_write(adapter, AL_ETH_KR_PMD_CONTROL, AL_ETH_LT_REGS,
			       lane, (AL_BIT(AL_ETH_KR_PMD_CONTROL_ENABLE) |
				      AL_BIT(AL_ETH_KR_PMD_CONTROL_RESTART)));
}

void al_mod_eth_kr_lt_stop(struct al_mod_hal_eth_adapter *adapter,
		       enum al_mod_eth_an_lt_lane lane)
{
	al_mod_dbg("[%s]: KR LT Stop Link Training.\n", adapter->name);

	al_mod_eth_an_lt_reg_write(adapter, AL_ETH_KR_PMD_CONTROL, AL_ETH_LT_REGS,
			       lane, AL_BIT(AL_ETH_KR_PMD_CONTROL_RESTART));
}

void al_mod_eth_kr_lt_initialize(struct al_mod_hal_eth_adapter *adapter,
			     enum al_mod_eth_an_lt_lane lane)
{
	al_mod_dbg("[%s]: KR LT Initialize.\n", adapter->name);

	/* Reset LT state machine */
	al_mod_eth_kr_lt_stop(adapter, lane);

	/* clear receiver status */
	al_mod_eth_an_lt_reg_write(adapter, AL_ETH_KR_PMD_STATUS, AL_ETH_LT_REGS, lane, 0);

	/* Coefficient Update to all zero (no command, hold) */
	al_mod_eth_an_lt_reg_write(adapter, AL_ETH_KR_PMD_LD_COEF_UP, AL_ETH_LT_REGS, lane, 0);
	/* Coefficient Status to all zero (not_updated) */
	al_mod_eth_an_lt_reg_write(adapter, AL_ETH_KR_PMD_LD_STATUS_REPORT, AL_ETH_LT_REGS, lane, 0);

	/* start */
	al_mod_eth_kr_lt_restart(adapter, lane);
}

al_mod_bool al_mod_eth_kr_lt_frame_lock_wait(struct al_mod_hal_eth_adapter *adapter,
				     enum al_mod_eth_an_lt_lane lane,
				     uint32_t timeout)
{
	uint32_t loop;
	uint16_t reg = 0;

	for (loop = 0; loop < timeout; loop++) {
		reg = al_mod_eth_an_lt_reg_read(adapter, AL_ETH_KR_PMD_STATUS, AL_ETH_LT_REGS, lane);

		if (AL_REG_BIT_GET(reg, AL_ETH_KR_PMD_STATUS_FAILURE_SHIFT)) {
			al_mod_info("[%s]: Failed on Training Failure."
			       " loops %d PMD STATUS 0x%04x\n",
			       adapter->name, loop, reg);

			return AL_FALSE;
		}
		if (AL_REG_BIT_GET(reg,
			AL_ETH_KR_PMD_STATUS_RECEIVER_FRAME_LOCK_SHIFT)) {
			al_mod_dbg("[%s]: Frame lock received."
			       " loops %d PMD STATUS 0x%04x\n",
			       adapter->name, loop, reg);

			return AL_TRUE;
		}
		al_mod_udelay(1);
	}
	al_mod_info("[%s]: Failed on timeout. PMD STATUS 0x%04x\n",
			adapter->name, reg);

	return AL_FALSE;
}


/**
 * Indirect access to AN/LT iofic registers
 */

/*
 * configure the interrupt registers, interrupts will are kept masked
 */
int al_mod_eth_an_lt_iofic_config(struct al_mod_hal_eth_adapter *adapter, int group, int lane,
	uint32_t flags)
{
	uint64_t intc_addr;
	uint64_t write_addr;
	uint32_t *an_lt_addr;
	uint32_t *an_lt_data;
	struct al_mod_eth_mac_obj *obj;

	obj = &adapter->mac_obj;

	al_mod_assert(group < AL_IOFIC_MAX_GROUPS);
	al_mod_assert(lane < AL_ETH_AN__LT_LANE_NUM);

	al_mod_eth_kr_an_get_indir_addr_by_lane(obj, lane,
		&an_lt_addr, &an_lt_data);

	intc_addr = (uintptr_t)al_mod_offsetof(struct al_mod_iofic_regs, ctrl[group].int_control_grp);
	write_addr = intc_addr + AL_HAL_ETH_AN_LT_INT_MEM_OFFSET;

	al_mod_reg_write32(an_lt_addr, write_addr);
	al_mod_reg_write32(an_lt_data, flags);

	return 0;
}

/*
 * mask specific interrupts for a given group
 */
void al_mod_eth_an_lt_iofic_mask(struct al_mod_hal_eth_adapter *adapter, int group, int lane,
	uint32_t mask)
{
	uint64_t intc_addr;
	uint64_t write_addr;
	uint32_t *an_lt_addr;
	uint32_t *an_lt_data;
	struct al_mod_eth_mac_obj *obj;
	uint32_t reg;

	obj = &adapter->mac_obj;

	al_mod_eth_kr_an_get_indir_addr_by_lane(obj, lane,
		&an_lt_addr, &an_lt_data);

	al_mod_assert(group < AL_IOFIC_MAX_GROUPS);

	intc_addr = (uintptr_t)al_mod_offsetof(struct al_mod_iofic_regs, ctrl[group].int_mask_grp);
	write_addr = intc_addr + AL_HAL_ETH_AN_LT_INT_MEM_OFFSET;

	al_mod_reg_write32(an_lt_addr, write_addr);

	reg = al_mod_reg_read32(an_lt_data);
	al_mod_reg_write32_relaxed(an_lt_data, reg | mask);
}

/*
 * unmask specific interrupts for a given group
 */
void al_mod_eth_an_lt_iofic_unmask(struct al_mod_hal_eth_adapter *adapter, int group, int lane,
	uint32_t mask)
{
	uint64_t intc_addr;
	uint64_t write_addr;
	uint32_t *an_lt_addr;
	uint32_t *an_lt_data;
	struct al_mod_eth_mac_obj *obj;

	obj = &adapter->mac_obj;

	al_mod_assert(group < AL_IOFIC_MAX_GROUPS);
	al_mod_assert(lane < AL_ETH_AN__LT_LANE_NUM);

	al_mod_eth_kr_an_get_indir_addr_by_lane(obj, lane,
		&an_lt_addr, &an_lt_data);

	intc_addr = (uintptr_t)al_mod_offsetof(struct al_mod_iofic_regs, ctrl[group].int_mask_clear_grp);
	write_addr = intc_addr + AL_HAL_ETH_AN_LT_INT_MEM_OFFSET;

	/*
	 * use the mask clear register, no need to read the mask register
	 * itself. write 0 to unmask, 1 has no effect
	 */
	al_mod_reg_write32_relaxed(an_lt_addr, write_addr);
	al_mod_reg_write32_relaxed(an_lt_data, ~mask);
}

/*
 * read interrupt cause register for a given group
 */
uint32_t al_mod_eth_an_lt_iofic_read_cause(struct al_mod_hal_eth_adapter *adapter, int group, int lane)
{
	uint64_t intc_addr;
	uint64_t write_addr;
	uint32_t *an_lt_addr;
	uint32_t *an_lt_data;
	struct al_mod_eth_mac_obj *obj;
	/** for internal offsetting */

	obj = &adapter->mac_obj;

	al_mod_assert(group < AL_IOFIC_MAX_GROUPS);
	al_mod_assert(lane < AL_ETH_AN__LT_LANE_NUM);

	al_mod_eth_kr_an_get_indir_addr_by_lane(obj, lane,
		&an_lt_addr, &an_lt_data);

	intc_addr = (uintptr_t)al_mod_offsetof(struct al_mod_iofic_regs, ctrl[group].int_cause_grp);
	write_addr = intc_addr + AL_HAL_ETH_AN_LT_INT_MEM_OFFSET;

	al_mod_reg_write32_relaxed(an_lt_addr, write_addr);
	return al_mod_reg_read32(an_lt_data);
}

/*
 * set the interrupt cause register for a given group
 */
void al_mod_eth_an_lt_iofic_set_cause(struct al_mod_hal_eth_adapter *adapter, int group, int lane,
	uint32_t mask)
{
	uint64_t intc_addr;
	uint64_t write_addr;
	uint32_t *an_lt_addr;
	uint32_t *an_lt_data;
	struct al_mod_eth_mac_obj *obj;

	obj = &adapter->mac_obj;

	al_mod_eth_kr_an_get_indir_addr_by_lane(obj, lane,
		&an_lt_addr, &an_lt_data);

	al_mod_assert(group < AL_IOFIC_MAX_GROUPS);

	intc_addr = (uintptr_t)al_mod_offsetof(struct al_mod_iofic_regs, ctrl[group].int_cause_set_grp);
	write_addr = intc_addr + AL_HAL_ETH_AN_LT_INT_MEM_OFFSET;

	al_mod_reg_write32(an_lt_addr, write_addr);
	al_mod_reg_write32(an_lt_data, mask);
}

/*
 * Force the DME downsample configuration to do auto negotiation in 25G
 */
void al_mod_eth_an_lt_dme_downsample_override(struct al_mod_hal_eth_adapter *adapter,
	struct al_mod_hal_eth_an_lt_dme_downsample_conf *conf)
{
	uint32_t reg = 0;
	uint32_t *an_lt_data, *an_lt_addr;
	struct al_mod_eth_mac_obj *obj;

	al_mod_assert(adapter);
	al_mod_assert(adapter->rev_id >= AL_ETH_REV_ID_4);
	al_mod_assert(conf);

	obj = &adapter->mac_obj;

	al_mod_eth_kr_an_get_indir_addr_by_lane(obj, adapter->serdes_lane, &an_lt_addr, &an_lt_data);

	AL_REG_FIELD_SET(reg,
		AL_ETH_AN_LT_DME_DS_MASK_FIND_FIRST_VEC_MASK,
		AL_ETH_AN_LT_DME_DS_MASK_FIND_FIRST_VEC_SHIFT,
		conf->mask_find_first_vec);
	AL_REG_FIELD_SET(reg,
		AL_ETH_AN_LT_DME_DS_MAX_CURR_SAMPLE_POINT_MASK,
		AL_ETH_AN_LT_DME_DS_MAX_CURR_SAMPLE_POINT_SHIFT,
		conf->max_curr_sample_point);
	if (conf->en_override)
		reg |= AL_ETH_AN_LT_DME_DS_ENABLE_OVERRIDE_VAL;
	/* Read out status reg for the currently selected instance */
	al_mod_reg_write32(an_lt_addr,
		(uintptr_t) al_mod_offsetof(struct al_mod_an_lt_wrapper_regs,
		an_lt[adapter->curr_lt_unit].dme_ds));
	al_mod_reg_write32(an_lt_data, reg);
}
