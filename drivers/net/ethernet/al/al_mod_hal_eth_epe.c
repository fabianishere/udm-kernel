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

#include "al_mod_hal_common.h"
#include "al_mod_hal_eth.h"
#include "al_mod_hal_eth_ec_regs.h"
#include "al_mod_hal_eth_rmp_regs.h"
#include "al_mod_hal_eth_epe_regs.h"
#include "al_mod_hal_eth_epe.h"
#include "al_mod_hal_udma_iofic.h"

/* Ethernet revision <= 3 */
#define EC_EPE_ACT_NUM								2

/* Ethernet revision >= 4 */
#define EC_RMP_EPE_ACT_TABLE_NUM					2
#define EC_RMP_EPE_P_COMP_NUM						2

/* Default protocol index */
#define EPE_DEFAULT_PROTOCOL_IDX			0
/* Default header offset */
#define EPE_DEFAULT_HDR_OFFSET				0
/* Default action table index */
#define EPE_DEFAULT_ACT_IDX				1
/* Max beats per packet parsing */
#define EPE_MAX_BEATS_PER_PACKET_PARSING		0x25
/* Max iterations per packet parsing */
#define EPE_MAX_ITERS_PER_PACKET_PARSING		0x20
/* Location of the control bits2 in the parser result vector */
#define EPE_PARSE_RES_CTRL_BITS_2_LOCATION		0x42

/* Number of bits in 1 byte */
#define BITS_TO_BYTE					8

/**
 * EPE <=3 extra masks and shifts
 * Inside al_mod_hal_eth_epe_regs.h
 */


/**
 * EPE >=4 extra masks and shifts
 */
/**** res_def register ****/
/* Protocol index */
#define EC_RMP_EPE_RES_DEF_PROTOCOL_INDEX_MASK 0x0000001F
#define EC_RMP_EPE_RES_DEF_PROTOCOL_INDEX_SHIFT 0
/* Parsing enable */
#define EC_RMP_EPE_RES_DEF_P_SOP_DEF_PARSE_EN (1 << 7)
/* Header offset */
#define EC_RMP_EPE_RES_DEF_P_SOP_DEF_HDR_OFFSET_MASK 0x00FF8000
#define EC_RMP_EPE_RES_DEF_P_SOP_DEF_HDR_OFFSET_SHIFT 15
/* Action table address */
#define EC_RMP_EPE_RES_DEF_DEF_SOP_ACTION_TABLE_ADDR_MASK 0xFC000000
#define EC_RMP_EPE_RES_DEF_DEF_SOP_ACTION_TABLE_ADDR_SHIFT 26
/* Parse cfg register */
#define EC_RMP_EPE_PARSE_CFG_MAX_BEATS_PER_PKT_MASK	AL_FIELD_MASK(7, 0)
#define EC_RMP_EPE_PARSE_CFG_MAX_BEATS_PER_PKT_SHIFT	0
#define EC_RMP_EPE_PARSE_CFG_MAX_ITERS_PER_PKT_MASK	AL_FIELD_MASK(15, 8)
#define EC_RMP_EPE_PARSE_CFG_MAX_ITERS_PER_PKT_SHIFT	8
/**
 * EPE default entries values for Ethernet revision <= 3
 */

/* GRE specific */
#define AL_ETH_V3_EPE_H_GRE_IDX			(8)
#define AL_ETH_V3_EPE_H_GRE_HDR_LEN		((4 << 16) | 4)

static void al_mod_eth_epe_error_ints_unmask(struct al_mod_eth_epe_handle *handle);

static struct al_mod_eth_epe_p_reg_entry eth_v3_default_comp_table[AL_ETH_EPE_TABLE_SIZE] = {
	/* 0 - Not used */
	AL_ETH_EPE_V3_P_COMP_ENTRY(AL_ETH_V3_P_COMP_DATA(AL_ETH_V3_P_COMP_DATA_DATA_2(0x00, 0x0), 0x0000),
				AL_ETH_V3_P_COMP_MASK(AL_ETH_V3_P_COMP_MASK_DATA_2(0x00, 0x0), 0x0000),
				AL_ETH_V3_P_COMP_CTRL(0, EC_EPE_P_COMP_CTRL_CMD_EQ, EC_EPE_P_COMP_CTRL_CMD_EQ, 0x0)),
	/* 1 - Not used */
	AL_ETH_EPE_V3_P_COMP_ENTRY(AL_ETH_V3_P_COMP_DATA(AL_ETH_V3_P_COMP_DATA_DATA_2(0x00, 0x0), 0x0000),
				AL_ETH_V3_P_COMP_MASK(AL_ETH_V3_P_COMP_MASK_DATA_2(0x00, 0x0), 0x0000),
				AL_ETH_V3_P_COMP_CTRL(0, EC_EPE_P_COMP_CTRL_CMD_EQ, EC_EPE_P_COMP_CTRL_CMD_EQ, 0x1)),
	/* 2 - Not used */
	AL_ETH_EPE_V3_P_COMP_ENTRY(AL_ETH_V3_P_COMP_DATA(AL_ETH_V3_P_COMP_DATA_DATA_2(0x00, 0x0), 0x0000),
				AL_ETH_V3_P_COMP_MASK(AL_ETH_V3_P_COMP_MASK_DATA_2(0x00, 0x0), 0x0000),
				AL_ETH_V3_P_COMP_CTRL(0, EC_EPE_P_COMP_CTRL_CMD_EQ, EC_EPE_P_COMP_CTRL_CMD_EQ, 0x2)),
	/* 3 - Not used */
	AL_ETH_EPE_V3_P_COMP_ENTRY(AL_ETH_V3_P_COMP_DATA(AL_ETH_V3_P_COMP_DATA_DATA_2(0x00, 0x0), 0x0000),
				AL_ETH_V3_P_COMP_MASK(AL_ETH_V3_P_COMP_MASK_DATA_2(0x00, 0x0), 0x0000),
				AL_ETH_V3_P_COMP_CTRL(0, EC_EPE_P_COMP_CTRL_CMD_EQ, EC_EPE_P_COMP_CTRL_CMD_EQ, 0x3)),
	/* 4 - Standard 801.1q vlan */
	AL_ETH_EPE_V3_P_COMP_ENTRY(AL_ETH_V3_P_COMP_DATA(AL_ETH_V3_P_COMP_DATA_DATA_2(0x00, 0x1), 0x8100),
				AL_ETH_V3_P_COMP_MASK(AL_ETH_V3_P_COMP_MASK_DATA_2(0x01, 0x7), 0xFFFF),
				AL_ETH_V3_P_COMP_CTRL(1, EC_EPE_P_COMP_CTRL_CMD_EQ, EC_EPE_P_COMP_CTRL_CMD_EQ, 0x4)),
	/* 5 - Standard 801.1ad QinQ vlan - Outer tag*/
	AL_ETH_EPE_V3_P_COMP_ENTRY(AL_ETH_V3_P_COMP_DATA(AL_ETH_V3_P_COMP_DATA_DATA_2(0x00, 0x1), 0x88A8),
				AL_ETH_V3_P_COMP_MASK(AL_ETH_V3_P_COMP_MASK_DATA_2(0x01, 0x7), 0xFFFF),
				AL_ETH_V3_P_COMP_CTRL(1, EC_EPE_P_COMP_CTRL_CMD_EQ, EC_EPE_P_COMP_CTRL_CMD_EQ, 0x5)),
	/* 6 - Non standard QinQ vlan tag - Inner tag*/
	AL_ETH_EPE_V3_P_COMP_ENTRY(AL_ETH_V3_P_COMP_DATA(AL_ETH_V3_P_COMP_DATA_DATA_2(0x01, 0x1), 0x9100),
				AL_ETH_V3_P_COMP_MASK(AL_ETH_V3_P_COMP_MASK_DATA_2(0x01, 0x7), 0xFFFF),
				AL_ETH_V3_P_COMP_CTRL(1, EC_EPE_P_COMP_CTRL_CMD_EQ, EC_EPE_P_COMP_CTRL_CMD_EQ, 0x6)),
	/* 7 - Standard 801.1q vlan tag - Inner tag*/
	AL_ETH_EPE_V3_P_COMP_ENTRY(AL_ETH_V3_P_COMP_DATA(AL_ETH_V3_P_COMP_DATA_DATA_2(0x01, 0x1), 0x8100),
				AL_ETH_V3_P_COMP_MASK(AL_ETH_V3_P_COMP_MASK_DATA_2(0x01, 0x7), 0xFFFF),
				AL_ETH_V3_P_COMP_CTRL(1, EC_EPE_P_COMP_CTRL_CMD_EQ, EC_EPE_P_COMP_CTRL_CMD_EQ, 0x7)),
	/* 8 - IPv4 - Check if fragmented */
	AL_ETH_EPE_V3_P_COMP_ENTRY(AL_ETH_V3_P_COMP_DATA(AL_ETH_V3_P_COMP_DATA_DATA_2(0x00, 0x1), 0x0800),
				AL_ETH_V3_P_COMP_MASK(AL_ETH_V3_P_COMP_MASK_DATA_2(0x00, 0x7), 0xFFFF),
				AL_ETH_V3_P_COMP_CTRL(1, EC_EPE_P_COMP_CTRL_CMD_EQ, EC_EPE_P_COMP_CTRL_CMD_EQ, 0x8)),
	/* 9 - IPv4 - Non Fragmented (Both MF and Frag Offset fields are 0) */
	AL_ETH_EPE_V3_P_COMP_ENTRY(AL_ETH_V3_P_COMP_DATA(AL_ETH_V3_P_COMP_DATA_DATA_2(0x00, 0x2), 0x0000),
				AL_ETH_V3_P_COMP_MASK(AL_ETH_V3_P_COMP_MASK_DATA_2(0x00, 0x7), 0x3FFF),
				AL_ETH_V3_P_COMP_CTRL(1, EC_EPE_P_COMP_CTRL_CMD_EQ, EC_EPE_P_COMP_CTRL_CMD_EQ, 0x9)),
	/* 10 - IPv4 - Fragmented */
	AL_ETH_EPE_V3_P_COMP_ENTRY(AL_ETH_V3_P_COMP_DATA(AL_ETH_V3_P_COMP_DATA_DATA_2(0x00, 0x2), 0x0000),
				AL_ETH_V3_P_COMP_MASK(AL_ETH_V3_P_COMP_MASK_DATA_2(0x00, 0x7), 0x0000),
				AL_ETH_V3_P_COMP_CTRL(1, EC_EPE_P_COMP_CTRL_CMD_EQ, EC_EPE_P_COMP_CTRL_CMD_EQ, 0x0A)),
	/* 11 - IPv6 */
	AL_ETH_EPE_V3_P_COMP_ENTRY(AL_ETH_V3_P_COMP_DATA(AL_ETH_V3_P_COMP_DATA_DATA_2(0x00, 0x1), 0x86DD),
				AL_ETH_V3_P_COMP_MASK(AL_ETH_V3_P_COMP_MASK_DATA_2(0x00, 0x7), 0xFFFF),
				AL_ETH_V3_P_COMP_CTRL(1, EC_EPE_P_COMP_CTRL_CMD_EQ, EC_EPE_P_COMP_CTRL_CMD_EQ, 0x0B)),
	/* 12 - TCP */
	AL_ETH_EPE_V3_P_COMP_ENTRY(AL_ETH_V3_P_COMP_DATA(AL_ETH_V3_P_COMP_DATA_DATA_2(0x00, 0x3), 0x0600),
				AL_ETH_V3_P_COMP_MASK(AL_ETH_V3_P_COMP_MASK_DATA_2(0x00, 0x7), 0xFF00),
				AL_ETH_V3_P_COMP_CTRL(1, EC_EPE_P_COMP_CTRL_CMD_EQ, EC_EPE_P_COMP_CTRL_CMD_EQ, 0x0C)),
	/* 13 - UDP */
	AL_ETH_EPE_V3_P_COMP_ENTRY(AL_ETH_V3_P_COMP_DATA(AL_ETH_V3_P_COMP_DATA_DATA_2(0x00, 0x3), 0x1100),
				AL_ETH_V3_P_COMP_MASK(AL_ETH_V3_P_COMP_MASK_DATA_2(0x00, 0x7), 0xFF00),
				AL_ETH_V3_P_COMP_CTRL(1, EC_EPE_P_COMP_CTRL_CMD_EQ, EC_EPE_P_COMP_CTRL_CMD_EQ, 0x0D)),
	/* 31 - Default Last Entry - Undetected Protocol */
	[AL_ETH_PROTO_ID_DEFAULT] =
	AL_ETH_EPE_V3_P_COMP_ENTRY(AL_ETH_V3_P_COMP_DATA(AL_ETH_V3_P_COMP_DATA_DATA_2(0x00, 0x0), 0x0000),
				AL_ETH_V3_P_COMP_MASK(AL_ETH_V3_P_COMP_MASK_DATA_2(0x00, 0x0), 0x0000),
				AL_ETH_V3_P_COMP_CTRL(1, EC_EPE_P_COMP_CTRL_CMD_EQ, EC_EPE_P_COMP_CTRL_CMD_EQ, 0x1F)),
};

static struct al_mod_eth_epe_control_entry eth_v3_default_ctrl_table[AL_ETH_EPE_TABLE_SIZE] = {
	{/* Entry 0 */
	 .data[0] = (EC_EPE_ACT_DATA_1_HDR_OFFSET_WR_SEL_CUR |
		     EC_EPE_ACT_DATA_1_HDR_LEN_WR_SEL_NEXT |
		     EC_EPE_ACT_DATA_1_PROTO_WR_SEL_CUR),
	 .data[1] = EC_EPE_ACT_DATA_2_DATA_SIZE(0x000),
	 .data[2] = EC_EPE_ACT_DATA_3_DATA_SIZE(0x000),
	 .data[3] = (EC_EPE_ACT_DATA_4_DEFAULT_HDR_LEN(0x00) |
		     EC_EPE_ACT_DATA_4_HDR_LEN_COMMAND_USE_DEFAULT_HDR_LEN),
	 .data[4] = (EC_EPE_ACT_DATA_5_NEXT_CTRL_BIT_VAL(0x00) |
		     EC_EPE_ACT_DATA_5_NEXT_CTRL_BIT_SET(0x00) |
		     EC_EPE_ACT_DATA_5_NEXT_BRANCH_ID_VAL(0x00) |
		     EC_EPE_ACT_DATA_5_NEXT_BRANCH_ID_SET(0x00) |
		     EC_EPE_ACT_DATA_5_NEXT_STAGE(0x0) |
		     EC_EPE_ACT_DATA_5_END_OF_PARSING),
	 .data[5] = EC_EPE_ACT_DATA_6_NEXT_PARSE_DIS,
	},
	{/* Entry 1 */
	 .data[0] = (EC_EPE_ACT_DATA_1_HDR_OFFSET_WR_SEL_CUR |
		     EC_EPE_ACT_DATA_1_HDR_LEN_WR_SEL_NEXT |
		     EC_EPE_ACT_DATA_1_PROTO_WR_SEL_CUR |
		     EC_EPE_ACT_DATA_1_NEXT_PROTO_AVAIL | EC_EPE_ACT_DATA_1_NEXT_PROTO_OFFSET(0x0C)),
	 .data[1] = (EC_EPE_ACT_DATA_2_DATA_PTR(0x00E) |
		     EC_EPE_ACT_DATA_2_DATA_WR |
		     EC_EPE_ACT_DATA_2_DATA_SIZE(0x030) |
		     EC_EPE_ACT_DATA_2_DATA_OFFSET(0x000)),
	 .data[2] = (EC_EPE_ACT_DATA_3_DATA_PTR(0x014) |
		     EC_EPE_ACT_DATA_3_DATA_WR |
		     EC_EPE_ACT_DATA_3_DATA_SIZE(0x030) |
		     EC_EPE_ACT_DATA_3_DATA_OFFSET(0x030)),
	 .data[3] = (EC_EPE_ACT_DATA_4_DEFAULT_HDR_LEN(0x0E) |
		     EC_EPE_ACT_DATA_4_HDR_LEN_COMMAND_USE_DEFAULT_HDR_LEN),
	 .data[4] = (EC_EPE_ACT_DATA_5_NEXT_CTRL_BIT_VAL(0x00) |
		     EC_EPE_ACT_DATA_5_NEXT_CTRL_BIT_SET(0x00) |
		     EC_EPE_ACT_DATA_5_NEXT_BRANCH_ID_VAL(0x00) |
		     EC_EPE_ACT_DATA_5_NEXT_BRANCH_ID_SET(0x00) |
		     EC_EPE_ACT_DATA_5_NEXT_STAGE(0x1)),
	 .data[5] = EC_EPE_ACT_DATA_6_NEXT_PARSE_DIS,
	},
	{/* Entry 2 */
	 .data[0] = (EC_EPE_ACT_DATA_1_HDR_OFFSET_WR_SEL_CUR |
		     EC_EPE_ACT_DATA_1_HDR_LEN_WR_SEL_NEXT |
		     EC_EPE_ACT_DATA_1_PROTO_WR_SEL_CUR |
		     EC_EPE_ACT_DATA_1_NEXT_PROTO_AVAIL | EC_EPE_ACT_DATA_1_NEXT_PROTO_OFFSET(0x14)),
	 .data[1] = (EC_EPE_ACT_DATA_2_DATA_PTR(0x00E) |
		     EC_EPE_ACT_DATA_2_DATA_WR |
		     EC_EPE_ACT_DATA_2_DATA_SIZE(0x030) |
		     EC_EPE_ACT_DATA_2_DATA_OFFSET(0x000)),
	 .data[2] = (EC_EPE_ACT_DATA_3_DATA_PTR(0x014) |
		     EC_EPE_ACT_DATA_3_DATA_WR |
		     EC_EPE_ACT_DATA_3_DATA_SIZE(0x030) |
		     EC_EPE_ACT_DATA_3_DATA_OFFSET(0x030)),
	 .data[3] = (EC_EPE_ACT_DATA_4_DEFAULT_HDR_LEN(0x16) |
		     EC_EPE_ACT_DATA_4_HDR_LEN_COMMAND_USE_DEFAULT_HDR_LEN),
	 .data[4] = (EC_EPE_ACT_DATA_5_NEXT_CTRL_BIT_VAL(0x00) |
		     EC_EPE_ACT_DATA_5_NEXT_CTRL_BIT_SET(0x00) |
		     EC_EPE_ACT_DATA_5_NEXT_BRANCH_ID_VAL(0x00) |
		     EC_EPE_ACT_DATA_5_NEXT_BRANCH_ID_SET(0x00) |
		     EC_EPE_ACT_DATA_5_NEXT_STAGE(0x1)),
	 .data[5] = EC_EPE_ACT_DATA_6_NEXT_PARSE_DIS,
	},
	{/* Entry 3 */
	 .data[0] = (EC_EPE_ACT_DATA_1_HDR_OFFSET_WR_SEL_CUR |
		     EC_EPE_ACT_DATA_1_HDR_LEN_WR_SEL_NEXT |
		     EC_EPE_ACT_DATA_1_PROTO_WR_SEL_CUR |
		     EC_EPE_ACT_DATA_1_NEXT_PROTO_AVAIL | EC_EPE_ACT_DATA_1_NEXT_PROTO_OFFSET(0x1C)),
	 .data[1] = (EC_EPE_ACT_DATA_2_DATA_PTR(0x00E) |
		     EC_EPE_ACT_DATA_2_DATA_WR |
		     EC_EPE_ACT_DATA_2_DATA_SIZE(0x030) |
		     EC_EPE_ACT_DATA_2_DATA_OFFSET(0x000)),
	 .data[2] = (EC_EPE_ACT_DATA_3_DATA_PTR(0x014) |
		     EC_EPE_ACT_DATA_3_DATA_WR |
		     EC_EPE_ACT_DATA_3_DATA_SIZE(0x030) |
		     EC_EPE_ACT_DATA_3_DATA_OFFSET(0x030)),
	 .data[3] = (EC_EPE_ACT_DATA_4_DEFAULT_HDR_LEN(0x1E) |
		     EC_EPE_ACT_DATA_4_HDR_LEN_COMMAND_USE_DEFAULT_HDR_LEN),
	 .data[4] = (EC_EPE_ACT_DATA_5_NEXT_CTRL_BIT_VAL(0x00) |
		     EC_EPE_ACT_DATA_5_NEXT_CTRL_BIT_SET(0x00) |
		     EC_EPE_ACT_DATA_5_NEXT_BRANCH_ID_VAL(0x00) |
		     EC_EPE_ACT_DATA_5_NEXT_BRANCH_ID_SET(0x00) |
		     EC_EPE_ACT_DATA_5_NEXT_STAGE(0x1)),
	 .data[5] = EC_EPE_ACT_DATA_6_NEXT_PARSE_DIS,
	},
	{/* Entry 4 - save VLAN data in parser result vector */
	 .data[0] = (EC_EPE_ACT_DATA_1_HDR_OFFSET_WR_SEL_CUR |
		     EC_EPE_ACT_DATA_1_HDR_LEN_WR_SEL_NEXT |
		     EC_EPE_ACT_DATA_1_PROTO_WR_SEL_CUR |
		     EC_EPE_ACT_DATA_1_NEXT_PROTO_AVAIL | EC_EPE_ACT_DATA_1_NEXT_PROTO_OFFSET(0x02)),
	 .data[1] = (EC_EPE_ACT_DATA_2_DATA_PTR(0x01A) |
		     EC_EPE_ACT_DATA_2_DATA_WR |
		     EC_EPE_ACT_DATA_2_DATA_SIZE(0x010) |
		     EC_EPE_ACT_DATA_2_DATA_OFFSET(0x000)),
	 .data[2] =  EC_EPE_ACT_DATA_3_DATA_SIZE(0x000),
	 .data[3] = (EC_EPE_ACT_DATA_4_DEFAULT_HDR_LEN(0x04) |
		     EC_EPE_ACT_DATA_4_HDR_LEN_COMMAND_USE_DEFAULT_HDR_LEN),
	 .data[4] = (EC_EPE_ACT_DATA_5_NEXT_CTRL_BIT_VAL(0x01) |
		     EC_EPE_ACT_DATA_5_NEXT_CTRL_BIT_SET(0x01) |
		     EC_EPE_ACT_DATA_5_NEXT_BRANCH_ID_VAL(0x01) |
		     EC_EPE_ACT_DATA_5_NEXT_BRANCH_ID_SET(0x01) |
		     EC_EPE_ACT_DATA_5_NEXT_STAGE(0x1)),
	 .data[5] = EC_EPE_ACT_DATA_6_NEXT_PARSE_DIS,
	},
	{/* Entry 5 - save VLAN data in parser result vector */
	 .data[0] = (EC_EPE_ACT_DATA_1_HDR_OFFSET_WR_SEL_CUR |
		     EC_EPE_ACT_DATA_1_HDR_LEN_WR_SEL_NEXT |
		     EC_EPE_ACT_DATA_1_PROTO_WR_SEL_CUR |
		     EC_EPE_ACT_DATA_1_NEXT_PROTO_AVAIL | EC_EPE_ACT_DATA_1_NEXT_PROTO_OFFSET(0x02)),
	 .data[1] = (EC_EPE_ACT_DATA_2_DATA_PTR(0x01A) |
		     EC_EPE_ACT_DATA_2_DATA_WR |
		     EC_EPE_ACT_DATA_2_DATA_SIZE(0x010) |
		     EC_EPE_ACT_DATA_2_DATA_OFFSET(0x000)),
	 .data[2] =  EC_EPE_ACT_DATA_3_DATA_SIZE(0x000),
	 .data[3] = (EC_EPE_ACT_DATA_4_DEFAULT_HDR_LEN(0x04) |
		     EC_EPE_ACT_DATA_4_HDR_LEN_COMMAND_USE_DEFAULT_HDR_LEN),
	 .data[4] = (EC_EPE_ACT_DATA_5_NEXT_CTRL_BIT_VAL(0x01) |
		     EC_EPE_ACT_DATA_5_NEXT_CTRL_BIT_SET(0x01) |
		     EC_EPE_ACT_DATA_5_NEXT_BRANCH_ID_VAL(0x01) |
		     EC_EPE_ACT_DATA_5_NEXT_BRANCH_ID_SET(0x01) |
		     EC_EPE_ACT_DATA_5_NEXT_STAGE(0x1)),
	 .data[5] = EC_EPE_ACT_DATA_6_NEXT_PARSE_DIS,
	},
	{/* Entry 6 - save VLAN data in parser result vector (and that 2 vlans exist) */
	 .data[0] = (EC_EPE_ACT_DATA_1_HDR_OFFSET_WR_SEL_CUR |
		     EC_EPE_ACT_DATA_1_HDR_LEN_WR_SEL_NEXT |
		     EC_EPE_ACT_DATA_1_PROTO_WR_SEL_CUR |
		     EC_EPE_ACT_DATA_1_NEXT_PROTO_AVAIL | EC_EPE_ACT_DATA_1_NEXT_PROTO_OFFSET(0x02)),
	 .data[1] = (EC_EPE_ACT_DATA_2_DATA_PTR(0x01C) |
		     EC_EPE_ACT_DATA_2_DATA_WR |
		     EC_EPE_ACT_DATA_2_DATA_SIZE(0x010) |
		     EC_EPE_ACT_DATA_2_DATA_OFFSET(0x000)),
	 .data[2] =  EC_EPE_ACT_DATA_3_DATA_SIZE(0x000),
	 .data[3] = (EC_EPE_ACT_DATA_4_DEFAULT_HDR_LEN(0x04) |
		     EC_EPE_ACT_DATA_4_HDR_LEN_COMMAND_USE_DEFAULT_HDR_LEN),
	 .data[4] = (EC_EPE_ACT_DATA_5_NEXT_CTRL_BIT_VAL(0x02) |
		     EC_EPE_ACT_DATA_5_NEXT_CTRL_BIT_SET(0x02) |
		     EC_EPE_ACT_DATA_5_NEXT_BRANCH_ID_VAL(0x00) |
		     EC_EPE_ACT_DATA_5_NEXT_BRANCH_ID_SET(0x00) |
		     EC_EPE_ACT_DATA_5_NEXT_STAGE(0x1)),
	 .data[5] = EC_EPE_ACT_DATA_6_NEXT_PARSE_DIS,
	},
	{/* Entry 7 - save VLAN data in parser result vector (and that 2 vlans exist) */
	 .data[0] = (EC_EPE_ACT_DATA_1_HDR_OFFSET_WR_SEL_CUR |
		     EC_EPE_ACT_DATA_1_HDR_LEN_WR_SEL_NEXT |
		     EC_EPE_ACT_DATA_1_PROTO_WR_SEL_CUR |
		     EC_EPE_ACT_DATA_1_NEXT_PROTO_AVAIL | EC_EPE_ACT_DATA_1_NEXT_PROTO_OFFSET(0x02)),
	 .data[1] = (EC_EPE_ACT_DATA_2_DATA_PTR(0x01C) |
		     EC_EPE_ACT_DATA_2_DATA_WR |
		     EC_EPE_ACT_DATA_2_DATA_SIZE(0x010) |
		     EC_EPE_ACT_DATA_2_DATA_OFFSET(0x000)),
	 .data[2] =  EC_EPE_ACT_DATA_3_DATA_SIZE(0x000),
	 .data[3] = (EC_EPE_ACT_DATA_4_DEFAULT_HDR_LEN(0x04) |
		     EC_EPE_ACT_DATA_4_HDR_LEN_COMMAND_USE_DEFAULT_HDR_LEN),
	 .data[4] = (EC_EPE_ACT_DATA_5_NEXT_CTRL_BIT_VAL(0x02) |
		     EC_EPE_ACT_DATA_5_NEXT_CTRL_BIT_SET(0x02) |
		     EC_EPE_ACT_DATA_5_NEXT_BRANCH_ID_VAL(0x00) |
		     EC_EPE_ACT_DATA_5_NEXT_BRANCH_ID_SET(0x00) |
		     EC_EPE_ACT_DATA_5_NEXT_STAGE(0x1)),
	 .data[5] = EC_EPE_ACT_DATA_6_NEXT_PARSE_DIS,
	},
	{/* Entry 8 - Save protocol_index for IPv4, DSCP/ToS Field, L3 Header offset and set offset
	  *           for matching in the next iteration to 6 (IPv4 Flags and Frag offset fields) */
	 .data[0] = (EC_EPE_ACT_DATA_1_HDR_OFFSET_WR_SEL_CUR |
		     EC_EPE_ACT_DATA_1_HDR_LEN_WR_SEL_NEXT |
		     EC_EPE_ACT_DATA_1_PROTO_WR_SEL_CUR |
		     EC_EPE_ACT_DATA_1_PROTO_WR_PTR(0x005) |
		     EC_EPE_ACT_DATA_1_PROTO_WR |
		     EC_EPE_ACT_DATA_1_NEXT_PROTO_AVAIL | EC_EPE_ACT_DATA_1_NEXT_PROTO_OFFSET(0x06)),
	 .data[1] =  EC_EPE_ACT_DATA_2_DATA_SIZE(0x000),
	 .data[2] = (EC_EPE_ACT_DATA_3_DATA_PTR(0x00D) |
		     EC_EPE_ACT_DATA_3_DATA_WR |
		     EC_EPE_ACT_DATA_3_DATA_SIZE(0x008) |
		     EC_EPE_ACT_DATA_3_DATA_OFFSET(0x008)),
	 .data[3] = (EC_EPE_ACT_DATA_4_DEFAULT_HDR_LEN(0x00) |
		     EC_EPE_ACT_DATA_4_HDR_LEN_COMMAND_USE_DEFAULT_HDR_LEN),
	 .data[4] = (EC_EPE_ACT_DATA_5_NEXT_CTRL_BIT_VAL(0x00) |
		     EC_EPE_ACT_DATA_5_NEXT_CTRL_BIT_SET(0x00) |
		     EC_EPE_ACT_DATA_5_NEXT_BRANCH_ID_VAL(0x00) |
		     EC_EPE_ACT_DATA_5_NEXT_BRANCH_ID_SET(0x00) |
		     EC_EPE_ACT_DATA_5_NEXT_STAGE(0x2)),
	 .data[5] = (EC_EPE_ACT_DATA_6_NEXT_PARSE_DIS |
		     EC_EPE_ACT_DATA_6_HDR_OFFSET_WR_PTR(0x006) |
		     EC_EPE_ACT_DATA_6_HDR_OFFSET_WR)
	},
	{/* Entry 9 - Save SIP and DIP, L3 Header length (IHL field shifted left by 2) and
	  *           set offset for matching in the next iteration to 9 (IPv4 Protocol Field)*/
	 .data[0] = (EC_EPE_ACT_DATA_1_HDR_OFFSET_WR_SEL_CUR |
		     EC_EPE_ACT_DATA_1_HDR_LEN_WR_SEL_NEXT |
		     EC_EPE_ACT_DATA_1_PROTO_WR_SEL_CUR |
		     EC_EPE_ACT_DATA_1_NEXT_PROTO_AVAIL | EC_EPE_ACT_DATA_1_NEXT_PROTO_OFFSET(0x09)),
	 .data[1] = (EC_EPE_ACT_DATA_2_DATA_PTR(0x01E) |
		     EC_EPE_ACT_DATA_2_DATA_WR |
		     EC_EPE_ACT_DATA_2_DATA_SIZE(0x020) |
		     EC_EPE_ACT_DATA_2_DATA_OFFSET(0x060)),
	 .data[2] = (EC_EPE_ACT_DATA_3_DATA_PTR(0x02E) |
		     EC_EPE_ACT_DATA_3_DATA_WR |
		     EC_EPE_ACT_DATA_3_DATA_SIZE(0x020) |
		     EC_EPE_ACT_DATA_3_DATA_OFFSET(0x080)),
	 .data[3] = (EC_EPE_ACT_DATA_4_DEFAULT_HDR_LEN(0x00) |
		     EC_EPE_ACT_DATA_4_HDR_LEN_COMMAND_DEFAULT_HDR_LEN_ADD | EC_EPE_ACT_DATA_4_HDR_LEN_COMMAND_FROM_PKT_DATA |
		     EC_EPE_ACT_DATA_4_HDR_LEN_UNITS(0x2) |
		     EC_EPE_ACT_DATA_4_HDR_LEN_SIZE(0x04) |
		     EC_EPE_ACT_DATA_4_HDR_LEN_OFFSET(0x04)),
	 .data[4] = (EC_EPE_ACT_DATA_5_NEXT_CTRL_BIT_VAL(0x00) |
		     EC_EPE_ACT_DATA_5_NEXT_CTRL_BIT_SET(0x00) |
		     EC_EPE_ACT_DATA_5_NEXT_BRANCH_ID_VAL(0x00) |
		     EC_EPE_ACT_DATA_5_NEXT_BRANCH_ID_SET(0x00) |
		     EC_EPE_ACT_DATA_5_NEXT_STAGE(0x3)),
	 .data[5] = (EC_EPE_ACT_DATA_6_NEXT_PARSE_DIS |
		     EC_EPE_ACT_DATA_6_HDR_LEN_WR_PTR(0x008) |
		     EC_EPE_ACT_DATA_6_HDR_LEN_WR),
	},
	{/* Entry 10 - Save SIP and DIP, L3 Header length (IHL field shifted left by 2) and
	  *            set offset for matching in the next iteration to 9 (IPv4 Protocol Field) */
	 .data[0] = (EC_EPE_ACT_DATA_1_HDR_OFFSET_WR_SEL_CUR |
		     EC_EPE_ACT_DATA_1_HDR_LEN_WR_SEL_NEXT |
		     EC_EPE_ACT_DATA_1_PROTO_WR_SEL_NEXT |
		     EC_EPE_ACT_DATA_1_PROTO_WR_PTR(0x00A) |
		     EC_EPE_ACT_DATA_1_PROTO_WR |
		     EC_EPE_ACT_DATA_1_DEFAULT_NEXT_PROTO(0x00) |
		     EC_EPE_ACT_DATA_1_NEXT_PROTO_AVAIL | EC_EPE_ACT_DATA_1_NEXT_PROTO_OFFSET(0x09)),
	 .data[1] = (EC_EPE_ACT_DATA_2_DATA_PTR(0x01E) |
		     EC_EPE_ACT_DATA_2_DATA_WR |
		     EC_EPE_ACT_DATA_2_DATA_SIZE(0x020) |
		     EC_EPE_ACT_DATA_2_DATA_OFFSET(0x060)),
	 .data[2] = (EC_EPE_ACT_DATA_3_DATA_PTR(0x02E) |
		     EC_EPE_ACT_DATA_3_DATA_WR |
		     EC_EPE_ACT_DATA_3_DATA_SIZE(0x020) |
		     EC_EPE_ACT_DATA_3_DATA_OFFSET(0x080)),
	 .data[3] = (EC_EPE_ACT_DATA_4_DEFAULT_HDR_LEN(0x00) |
		     EC_EPE_ACT_DATA_4_HDR_LEN_COMMAND_DEFAULT_HDR_LEN_ADD | EC_EPE_ACT_DATA_4_HDR_LEN_COMMAND_FROM_PKT_DATA |
		     EC_EPE_ACT_DATA_4_HDR_LEN_UNITS(0x2) |
		     EC_EPE_ACT_DATA_4_HDR_LEN_SIZE(0x04) |
		     EC_EPE_ACT_DATA_4_HDR_LEN_OFFSET(0x04)),
	 .data[4] = (EC_EPE_ACT_DATA_5_NEXT_CTRL_BIT_VAL(0x08) |
		     EC_EPE_ACT_DATA_5_NEXT_CTRL_BIT_SET(0x08) |
		     EC_EPE_ACT_DATA_5_NEXT_BRANCH_ID_VAL(0x00) |
		     EC_EPE_ACT_DATA_5_NEXT_BRANCH_ID_SET(0x00) |
		     EC_EPE_ACT_DATA_5_NEXT_STAGE(0x3) |
		     EC_EPE_ACT_DATA_5_END_OF_PARSING),
	 .data[5] = (EC_EPE_ACT_DATA_6_NEXT_PARSE_DIS |
		     EC_EPE_ACT_DATA_6_HDR_LEN_WR_PTR(0x008) |
		     EC_EPE_ACT_DATA_6_HDR_LEN_WR),
	},
	{/* Entry 11 - Save protocol_index for IPv6, SIP and DIP, Traffic Class, L3 Header offset,
	  *            L3 Header length and set offset for matching in the next iteration
	  *            to 6 (IPv6 Next Header field) */
	 .data[0] = (EC_EPE_ACT_DATA_1_HDR_OFFSET_WR_SEL_CUR |
		     EC_EPE_ACT_DATA_1_HDR_LEN_WR_SEL_NEXT |
		     EC_EPE_ACT_DATA_1_PROTO_WR_SEL_CUR |
		     EC_EPE_ACT_DATA_1_PROTO_WR_PTR(0x005) |
		     EC_EPE_ACT_DATA_1_PROTO_WR |
		     EC_EPE_ACT_DATA_1_NEXT_PROTO_AVAIL | EC_EPE_ACT_DATA_1_NEXT_PROTO_OFFSET(0x06)),
	 .data[1] = (EC_EPE_ACT_DATA_2_DATA_PTR(0x01E) |
		     EC_EPE_ACT_DATA_2_DATA_WR |
		     EC_EPE_ACT_DATA_2_DATA_SIZE(0x100) |
		     EC_EPE_ACT_DATA_2_DATA_OFFSET(0x040)),
	 .data[2] = (EC_EPE_ACT_DATA_3_DATA_PTR(0x00D) |
		     EC_EPE_ACT_DATA_3_DATA_WR |
		     EC_EPE_ACT_DATA_3_DATA_SIZE(0x008) |
		     EC_EPE_ACT_DATA_3_DATA_OFFSET(0x004)),
	 .data[3] = (EC_EPE_ACT_DATA_4_DEFAULT_HDR_LEN(0x28) |
		     EC_EPE_ACT_DATA_4_HDR_LEN_COMMAND_USE_DEFAULT_HDR_LEN),
	 .data[4] = (EC_EPE_ACT_DATA_5_NEXT_CTRL_BIT_VAL(0x00) |
		     EC_EPE_ACT_DATA_5_NEXT_CTRL_BIT_SET(0x00) |
		     EC_EPE_ACT_DATA_5_NEXT_BRANCH_ID_VAL(0x00) |
		     EC_EPE_ACT_DATA_5_NEXT_BRANCH_ID_SET(0x00) |
		     EC_EPE_ACT_DATA_5_NEXT_STAGE(0x3)),
	 .data[5] = (EC_EPE_ACT_DATA_6_NEXT_PARSE_DIS |
		     EC_EPE_ACT_DATA_6_HDR_OFFSET_WR_PTR(0x006) |
		     EC_EPE_ACT_DATA_6_HDR_OFFSET_WR |
		     EC_EPE_ACT_DATA_6_HDR_LEN_WR_PTR(0x008) |
		     EC_EPE_ACT_DATA_6_HDR_LEN_WR),
	},
	{/* Entry 12 - Save protocol_index for TCP, Src Port and Dst Port, L4 Header offset,
	  *            L4 Header length(?) and set offset for matching in the next iteration
	  *            to 2 (TCP Dst Port field) */
	 .data[0] = (EC_EPE_ACT_DATA_1_HDR_OFFSET_WR_SEL_CUR |
		     EC_EPE_ACT_DATA_1_HDR_LEN_WR_SEL_NEXT |
		     EC_EPE_ACT_DATA_1_PROTO_WR_SEL_CUR |
		     EC_EPE_ACT_DATA_1_PROTO_WR_PTR(0x00A) |
		     EC_EPE_ACT_DATA_1_PROTO_WR |
		     EC_EPE_ACT_DATA_1_NEXT_PROTO_AVAIL | EC_EPE_ACT_DATA_1_NEXT_PROTO_OFFSET(0x02)),
	 .data[1] = (EC_EPE_ACT_DATA_2_DATA_PTR(0x03E) |
		     EC_EPE_ACT_DATA_2_DATA_WR |
		     EC_EPE_ACT_DATA_2_DATA_SIZE(0x010) |
		     EC_EPE_ACT_DATA_2_DATA_OFFSET(0x000)),
	 .data[2] = (EC_EPE_ACT_DATA_3_DATA_PTR(0x040) |
		     EC_EPE_ACT_DATA_3_DATA_WR |
		     EC_EPE_ACT_DATA_3_DATA_SIZE(0x010) |
		     EC_EPE_ACT_DATA_3_DATA_OFFSET(0x010)),
	 .data[3] = (EC_EPE_ACT_DATA_4_DEFAULT_HDR_LEN(0x14) |
		     EC_EPE_ACT_DATA_4_HDR_LEN_COMMAND_DEFAULT_HDR_LEN_ADD | EC_EPE_ACT_DATA_4_HDR_LEN_COMMAND_FROM_PKT_DATA |
		     EC_EPE_ACT_DATA_4_HDR_LEN_UNITS(0x2) |
		     EC_EPE_ACT_DATA_4_HDR_LEN_SIZE(0x04) |
		     EC_EPE_ACT_DATA_4_HDR_LEN_OFFSET(0x60)),
	 .data[4] = (EC_EPE_ACT_DATA_5_NEXT_CTRL_BIT_VAL(0x10) |
		     EC_EPE_ACT_DATA_5_NEXT_CTRL_BIT_SET(0x10) |
		     EC_EPE_ACT_DATA_5_NEXT_BRANCH_ID_VAL(0x00) |
		     EC_EPE_ACT_DATA_5_NEXT_BRANCH_ID_SET(0x00) |
		     EC_EPE_ACT_DATA_5_NEXT_STAGE(0x4) |
		     EC_EPE_ACT_DATA_5_END_OF_PARSING),
	 .data[5] = (EC_EPE_ACT_DATA_6_NEXT_PARSE_DIS |
		     EC_EPE_ACT_DATA_6_HDR_OFFSET_WR_PTR(0x00B) |
		     EC_EPE_ACT_DATA_6_HDR_OFFSET_WR),
	},
	{/* Entry 13 - Save protocol_index for UDP, Src Port and Dst Port, L4 Header offset,
	  *            L4 Header length(?) and set offset for matching in the next iteration
	  *            to 2 (UDP Dst Port field) */
	 .data[0] = (EC_EPE_ACT_DATA_1_HDR_OFFSET_WR_SEL_CUR |
		     EC_EPE_ACT_DATA_1_HDR_LEN_WR_SEL_NEXT |
		     EC_EPE_ACT_DATA_1_PROTO_WR_SEL_CUR |
		     EC_EPE_ACT_DATA_1_PROTO_WR_PTR(0x00A) |
		     EC_EPE_ACT_DATA_1_PROTO_WR |
		     EC_EPE_ACT_DATA_1_NEXT_PROTO_AVAIL | EC_EPE_ACT_DATA_1_NEXT_PROTO_OFFSET(0x02)),
	 .data[1] = (EC_EPE_ACT_DATA_2_DATA_PTR(0x03E) |
		     EC_EPE_ACT_DATA_2_DATA_WR |
		     EC_EPE_ACT_DATA_2_DATA_SIZE(0x010) |
		     EC_EPE_ACT_DATA_2_DATA_OFFSET(0x000)),
	 .data[2] = (EC_EPE_ACT_DATA_3_DATA_PTR(0x040) |
		     EC_EPE_ACT_DATA_3_DATA_WR |
		     EC_EPE_ACT_DATA_3_DATA_SIZE(0x010) |
		     EC_EPE_ACT_DATA_3_DATA_OFFSET(0x010)),
	 .data[3] = (EC_EPE_ACT_DATA_4_DEFAULT_HDR_LEN(0x08) |
		     EC_EPE_ACT_DATA_4_HDR_LEN_COMMAND_USE_DEFAULT_HDR_LEN),
	 .data[4] = (EC_EPE_ACT_DATA_5_NEXT_CTRL_BIT_VAL(0x10) |
		     EC_EPE_ACT_DATA_5_NEXT_CTRL_BIT_SET(0x10) |
		     EC_EPE_ACT_DATA_5_NEXT_BRANCH_ID_VAL(0x00) |
		     EC_EPE_ACT_DATA_5_NEXT_BRANCH_ID_SET(0x00) |
		     EC_EPE_ACT_DATA_5_NEXT_STAGE(0x4) |
		     EC_EPE_ACT_DATA_5_END_OF_PARSING),
	 .data[5] = (EC_EPE_ACT_DATA_6_NEXT_PARSE_DIS |
		     EC_EPE_ACT_DATA_6_HDR_OFFSET_WR_PTR(0x00B) |
		     EC_EPE_ACT_DATA_6_HDR_OFFSET_WR),
	},
	[AL_ETH_PROTO_ID_DEFAULT] = {
	 /* Entry 31 - Default last entry for undetected protocols - basically just end parsing */
	 .data[0] = (EC_EPE_ACT_DATA_1_HDR_OFFSET_WR_SEL_CUR |
		     EC_EPE_ACT_DATA_1_HDR_LEN_WR_SEL_NEXT |
		     EC_EPE_ACT_DATA_1_PROTO_WR_SEL_CUR),
	 .data[1] =  EC_EPE_ACT_DATA_2_DATA_SIZE(0x000),
	 .data[2] =  EC_EPE_ACT_DATA_3_DATA_SIZE(0x000),
	 .data[3] = (EC_EPE_ACT_DATA_4_DEFAULT_HDR_LEN(0x00) |
		     EC_EPE_ACT_DATA_4_HDR_LEN_COMMAND_USE_DEFAULT_HDR_LEN),
	 .data[4] = (EC_EPE_ACT_DATA_5_NEXT_CTRL_BIT_VAL(0x00) |
		     EC_EPE_ACT_DATA_5_NEXT_CTRL_BIT_SET(0x00) |
		     EC_EPE_ACT_DATA_5_NEXT_BRANCH_ID_VAL(0x00) |
		     EC_EPE_ACT_DATA_5_NEXT_BRANCH_ID_SET(0x00) |
		     EC_EPE_ACT_DATA_5_NEXT_STAGE(0x0) |
		     EC_EPE_ACT_DATA_5_END_OF_PARSING),
	 .data[5] = EC_EPE_ACT_DATA_6_NEXT_PARSE_DIS,
	}
};

/* Helper Functions */
static int al_mod_eth_epe_v3_values_init(struct al_mod_eth_epe_handle *handle,
	struct al_mod_eth_epe_init_params *params)
{
	struct al_mod_ec_regs *ec_regs_base = handle->regs_base;
	unsigned int i;

	al_mod_assert(handle->eth_rev_id <= AL_ETH_REV_ID_3);

	if (!params->enable || !params->init_v3_default_entries)
		return 0;

	for (i = 0; i < AL_ETH_EPE_TABLE_SIZE; i++) {
		if (handle->type == EPE_TYPE_OUTER)
			al_mod_eth_epe_comp_entry_raw_set(handle, i, &eth_v3_default_comp_table[i]);

		al_mod_eth_epe_ctrl_entry_raw_set(handle, i, &eth_v3_default_ctrl_table[i]);
	}

	/* header length as function of 4 bits value, for GRE, when C bit is set,
	 * the header len should be increase by 4 */
	al_mod_reg_write32(&ec_regs_base->epe_h[AL_ETH_V3_EPE_H_GRE_IDX].hdr_len,
		AL_ETH_V3_EPE_H_GRE_HDR_LEN);

	return 0;
}

/* API */
int al_mod_eth_epe_handle_init(struct al_mod_eth_epe_handle *handle,
	struct al_mod_eth_epe_handle_init_params *params)
{
	al_mod_assert(handle);
	al_mod_assert(params);
	al_mod_assert(params->regs_base);

	handle->regs_base = params->regs_base;
	if (params->eth_rev_id >= AL_ETH_REV_ID_4) {
		struct al_mod_ec_rmp_regs *rmp_base = (struct al_mod_ec_rmp_regs *)(handle->regs_base);
		handle->ints_base = &rmp_base->int_ctrl_mem[0];
	}
	handle->type = params->type;
	handle->eth_rev_id = params->eth_rev_id;
	handle->fields = params->fields;

	return 0;
}

int al_mod_eth_epe_init(struct al_mod_eth_epe_handle *handle, struct al_mod_eth_epe_init_params *params)
{
	uint32_t res_in_val, res_def_val = 0;
	int ret;

	/* Parser input selection - all values from res_def */
	res_in_val = EC_EPE_RES_IN_SEL_PARSE_EN |
		EC_EPE_RES_IN_SEL_PROT_INDEX |
		EC_EPE_RES_IN_SEL_HDR_OFFSET;

	/* If inner and enabled - take from outer parser result */
	if (params->enable && (handle->type == EPE_TYPE_INNER))
		res_in_val = 0;

	if (handle->eth_rev_id <= AL_ETH_REV_ID_3) {
		struct al_mod_ec_regs *ec_regs_base = handle->regs_base;

		/* Parser input from register value */
		if (params->enable)
			res_def_val |= EC_EPE_RES_DEF_P_SOP_DEF_PARSE_EN;

		AL_REG_FIELD_SET(res_def_val,
			EC_EPE_RES_DEF_P_SOP_DEF_HDR_OFFSET_MASK,
			EC_EPE_RES_DEF_P_SOP_DEF_HDR_OFFSET_SHIFT,
			EPE_DEFAULT_HDR_OFFSET);

		AL_REG_FIELD_SET(res_def_val,
			EC_EPE_RES_DEF_DEF_SOP_ACTION_TABLE_ADDR_MASK,
			EC_EPE_RES_DEF_DEF_SOP_ACTION_TABLE_ADDR_SHIFT,
			EPE_DEFAULT_ACT_IDX);

		al_mod_reg_write32(&ec_regs_base->epe[handle->type].res_def, res_def_val);
		al_mod_reg_write32(&ec_regs_base->epe[handle->type].res_in, res_in_val);
	} else {
		struct al_mod_ec_rmp_regs *rmp_regs_base = handle->regs_base;
		uint32_t parse_cfg = 0;

		/* Initialize memories */
		if (handle->type == EPE_TYPE_OUTER) {
			uint32_t mem_init = EC_RMP_GEN_MEM_INIT_TRIG_RMP_HDR_INPUT_FIFO |
				EC_RMP_GEN_MEM_INIT_TRIG_RMP_DATA_FIFO;

			/* Reset initialization */
			al_mod_reg_write32_masked(&rmp_regs_base->gen.mem_init, mem_init, 0);
			al_mod_local_data_memory_barrier();
			al_mod_udelay(1);

			/* Set initialization and wait enough for it to finish */
			al_mod_reg_write32_masked(&rmp_regs_base->gen.mem_init, mem_init, mem_init);
			al_mod_local_data_memory_barrier();
			al_mod_udelay(10);

			/** Need to initialize only once */
			al_mod_eth_epe_error_ints_unmask(handle);
		}

		/* Parser input from register value */
		AL_REG_FIELD_SET(res_def_val,
			EC_RMP_EPE_RES_DEF_PROTOCOL_INDEX_MASK,
			EC_RMP_EPE_RES_DEF_PROTOCOL_INDEX_SHIFT,
			EPE_DEFAULT_PROTOCOL_IDX);

		if (params->enable)
			res_def_val |= EC_RMP_EPE_RES_DEF_P_SOP_DEF_PARSE_EN;

		AL_REG_FIELD_SET(res_def_val,
			EC_RMP_EPE_RES_DEF_P_SOP_DEF_HDR_OFFSET_MASK,
			EC_RMP_EPE_RES_DEF_P_SOP_DEF_HDR_OFFSET_SHIFT,
			EPE_DEFAULT_HDR_OFFSET);

		AL_REG_FIELD_SET(res_def_val,
			EC_RMP_EPE_RES_DEF_DEF_SOP_ACTION_TABLE_ADDR_MASK,
			EC_RMP_EPE_RES_DEF_DEF_SOP_ACTION_TABLE_ADDR_SHIFT,
			EPE_DEFAULT_ACT_IDX);

		/** Parse cfg */
		AL_REG_FIELD_SET(parse_cfg,
			EC_RMP_EPE_PARSE_CFG_MAX_BEATS_PER_PKT_MASK,
			EC_RMP_EPE_PARSE_CFG_MAX_BEATS_PER_PKT_SHIFT,
			EPE_MAX_BEATS_PER_PACKET_PARSING);

		AL_REG_FIELD_SET(parse_cfg,
			EC_RMP_EPE_PARSE_CFG_MAX_ITERS_PER_PKT_MASK,
			EC_RMP_EPE_PARSE_CFG_MAX_ITERS_PER_PKT_SHIFT,
			EPE_MAX_ITERS_PER_PACKET_PARSING);

		al_mod_reg_write32(&rmp_regs_base->epe[handle->type].res_def, res_def_val);
		al_mod_reg_write32(&rmp_regs_base->epe[handle->type].res_in, res_in_val);
		al_mod_reg_write32(&rmp_regs_base->epe[handle->type].parse_cfg, parse_cfg);
		al_mod_reg_write32_masked(&rmp_regs_base->epe_res.p21,
			EC_RMP_EPE_RES_P21_CTRL_BITS2_MASK,
			EPE_PARSE_RES_CTRL_BITS_2_LOCATION << EC_RMP_EPE_RES_P21_CTRL_BITS2_SHIFT);
	}

	if (handle->eth_rev_id <= AL_ETH_REV_ID_3) {
		ret = al_mod_eth_epe_v3_values_init(handle, params);
		if (ret)
			return ret;
	}

	return 0;
}

void al_mod_eth_epe_comp_entry_set(struct al_mod_eth_epe_handle *handle,
		struct al_mod_eth_epe_comp_entry *comp_entry)
{
	struct al_mod_eth_epe_p_reg_entry comp_entry_raw;

	al_mod_assert(handle);
	al_mod_assert(comp_entry);
	al_mod_assert((handle->eth_rev_id >= AL_ETH_REV_ID_4) || (handle->type == EPE_TYPE_OUTER));

	al_mod_memset(&comp_entry_raw, 0, sizeof(comp_entry_raw));

	/* Data */
	AL_REG_FIELD_SET(comp_entry_raw.data,
		EC_EPE_P_COMP_DATA_DATA_1_MASK,
		EC_EPE_P_COMP_DATA_DATA_1_SHIFT,
		comp_entry->header_val);
	AL_REG_FIELD_SET(comp_entry_raw.data,
		EC_EPE_P_COMP_DATA_DATA_2_STAGE_MASK,
		EC_EPE_P_COMP_DATA_DATA_2_STAGE_SHIFT,
		comp_entry->stage_val);
	AL_REG_FIELD_SET(comp_entry_raw.data,
		EC_EPE_P_COMP_DATA_DATA_2_BRANCH_ID_MASK,
		EC_EPE_P_COMP_DATA_DATA_2_BRANCH_ID_SHIFT,
		comp_entry->branch_val);

	/* Mask */
	AL_REG_FIELD_SET(comp_entry_raw.mask,
		EC_EPE_P_COMP_MASK_DATA_1_MASK,
		EC_EPE_P_COMP_MASK_DATA_1_SHIFT,
		comp_entry->header_mask);
	AL_REG_FIELD_SET(comp_entry_raw.mask,
		EC_EPE_P_COMP_MASK_DATA_2_STAGE_MASK,
		EC_EPE_P_COMP_MASK_DATA_2_STAGE_SHIFT,
		comp_entry->stage_mask);
	AL_REG_FIELD_SET(comp_entry_raw.mask,
		EC_EPE_P_COMP_MASK_DATA_2_BRANCH_ID_MASK,
		EC_EPE_P_COMP_MASK_DATA_2_BRANCH_ID_SHIFT,
		comp_entry->branch_mask);

	/* Control */
	AL_REG_FIELD_SET(comp_entry_raw.ctrl,
		EC_EPE_P_COMP_CTRL_RES_MASK,
		EC_EPE_P_COMP_CTRL_RES_SHIFT,
		comp_entry->output_proto_idx);

	if (handle->eth_rev_id >= AL_ETH_REV_ID_4) {
		AL_REG_FIELD_SET(comp_entry_raw.ctrl,
			EC_RMP_RMP_EPE_P_COMP_CTRL_ACT_TABLE_ADDR_RES_MASK,
			EC_RMP_RMP_EPE_P_COMP_CTRL_ACT_TABLE_ADDR_RES_SHIFT,
			comp_entry->output_ctrl_idx);
	}

	AL_REG_FIELD_SET(comp_entry_raw.ctrl,
		EC_EPE_P_COMP_CTRL_CMD_1_MASK,
		EC_EPE_P_COMP_CTRL_CMD_1_SHIFT,
		comp_entry->header_cmd);
	AL_REG_FIELD_SET(comp_entry_raw.ctrl,
		EC_EPE_P_COMP_CTRL_CMD_2_MASK,
		EC_EPE_P_COMP_CTRL_CMD_2_SHIFT,
		comp_entry->stage_cmd);
	if (comp_entry->valid)
		comp_entry_raw.ctrl |= EC_EPE_P_COMP_CTRL_VALID;

	al_mod_eth_epe_comp_entry_raw_set(handle, comp_entry->idx, &comp_entry_raw);
}

void al_mod_eth_epe_comp_entry_raw_set(struct al_mod_eth_epe_handle *handle,
	unsigned int idx, struct al_mod_eth_epe_p_reg_entry *comp_entry_raw)
{
	al_mod_assert(handle);
	al_mod_assert(comp_entry_raw);
	al_mod_assert(handle->eth_rev_id >= AL_ETH_REV_ID_4 || handle->type == EPE_TYPE_OUTER);

	if (handle->eth_rev_id <= AL_ETH_REV_ID_3) {
		struct al_mod_ec_regs *ec_regs_base = handle->regs_base;
		struct al_mod_ec_epe_p *epe_p_regs = &ec_regs_base->epe_p[idx];

		al_mod_reg_write32(&epe_p_regs->comp_data, comp_entry_raw->data);
		al_mod_reg_write32(&epe_p_regs->comp_mask, comp_entry_raw->mask);
		al_mod_reg_write32(&epe_p_regs->comp_ctrl, comp_entry_raw->ctrl);
	} else {
		struct al_mod_ec_rmp_regs *rmp_regs_base = handle->regs_base;
		uint32_t reg = 0;

		AL_REG_FIELD_SET(reg,
			EC_RMP_RMP_EPE_P_COMP_TABLE_ADDR_MASK,
			EC_RMP_RMP_EPE_P_COMP_TABLE_ADDR_SHIFT,
			idx);

		if (handle->type == EPE_TYPE_INNER)
			reg |= EC_RMP_RMP_EPE_P_COMP_TABLE_PARSER_NUM;

		al_mod_reg_write32(&rmp_regs_base->rmp.epe_p_comp_table, reg);
		al_mod_reg_write32(&rmp_regs_base->rmp.epe_p_comp_data, comp_entry_raw->data);
		al_mod_reg_write32(&rmp_regs_base->rmp.epe_p_comp_mask, comp_entry_raw->mask);
		al_mod_reg_write32(&rmp_regs_base->rmp.epe_p_comp_ctrl, comp_entry_raw->ctrl);
	}
}

static void epe_ctrl_data_cmd_set(struct al_mod_eth_epe_handle *handle,
	struct al_mod_eth_epe_ctrl_entry *ctrl_entry, unsigned int cmd_idx, uint32_t *data)
{
	struct al_mod_eth_epe_ctrl_data_cmd *data_cmd = &ctrl_entry->data_cmds[cmd_idx];
	unsigned int first_bit, last_bit, size;
	uint32_t *data_cmd_data;

	/* We assume data is zeroed at bits we intend to write */
	if (!data_cmd->field)
		return;

	first_bit = data_cmd->field->offset;
	size = data_cmd->field->size;
	last_bit = first_bit + size - 1;

	al_mod_assert_msg(
		(data_cmd->field->offset % BITS_TO_BYTE == 0),
		"Data command %u - offset must be byte aligned\n", cmd_idx);
	al_mod_assert_msg(
		(first_bit >= EPE_CTRL_DATA_CMD_OFFSET_MIN_BY_REV(handle->eth_rev_id, cmd_idx)),
		"Data command %u - invalid field start offset %u (< %d)\n",
		cmd_idx, first_bit,
		EPE_CTRL_DATA_CMD_OFFSET_MIN_BY_REV(handle->eth_rev_id, cmd_idx));

	al_mod_assert_msg(
		(size < EPE_CTRL_DATA_CMD_SIZE_MAX_BY_REV(handle->eth_rev_id, cmd_idx)),
		"Data command %u - invalid field size %u (>= %d)\n",
		cmd_idx, size, EPE_CTRL_DATA_CMD_SIZE_MAX_BY_REV(handle->eth_rev_id, cmd_idx));

	al_mod_assert_msg(
		(last_bit <= EPE_CTRL_DATA_CMD_OFFSET_MAX_BY_REV(handle->eth_rev_id, cmd_idx)),
		"Data command %u - invalid field end offset %u (> %d)\n",
		cmd_idx, last_bit,
		EPE_CTRL_DATA_CMD_OFFSET_MAX_BY_REV(handle->eth_rev_id, cmd_idx));

	/* Data commands 1-2 to data[1-2] */
	/* Data commands 3-4 to data[6-7] */
	if (cmd_idx < 2)
		data_cmd_data = &data[1 + cmd_idx];
	else
		data_cmd_data = &data[6 + (cmd_idx - 2)];

	AL_REG_FIELD_SET(*data_cmd_data,
		EC_EPE_ACT_DATA_2_DATA_OFFSET_MASK,
		EC_EPE_ACT_DATA_2_DATA_OFFSET_SHIFT,
		data_cmd->offset);

	AL_REG_FIELD_SET(*data_cmd_data,
		EC_EPE_ACT_DATA_2_DATA_SIZE_MASK,
		EC_EPE_ACT_DATA_2_DATA_SIZE_SHIFT,
		data_cmd->field->size);

	*data_cmd_data |= EC_EPE_ACT_DATA_2_DATA_WR;

	AL_REG_FIELD_SET(*data_cmd_data,
		EC_EPE_ACT_DATA_2_DATA_PTR_MASK,
		EC_EPE_ACT_DATA_2_DATA_PTR_SHIFT,
		data_cmd->field->offset / BITS_TO_BYTE);
}

void al_mod_eth_epe_comp_entry_valid_set(struct al_mod_eth_epe_handle *handle,
				     unsigned int idx, al_mod_bool state)
{
	struct al_mod_ec_regs *ec_regs_base;

	al_mod_assert(handle);
	al_mod_assert(idx < AL_ETH_EPE_ENTRIES_NUM);

	ec_regs_base = handle->regs_base;

	al_mod_reg_write32_masked(&ec_regs_base->epe_p[idx].comp_ctrl,
			      EC_EPE_P_COMP_CTRL_VALID,
			      state ? EC_EPE_P_COMP_CTRL_VALID : 0);
}

void al_mod_eth_epe_ctrl_entry_set(struct al_mod_eth_epe_handle *handle,
		struct al_mod_eth_epe_ctrl_entry *ctrl_entry)
{
	struct al_mod_eth_epe_control_entry ctrl_entry_raw = { {0} };
	uint32_t *data = ctrl_entry_raw.data;
	unsigned int i;

	al_mod_assert(handle);
	al_mod_assert_msg(handle->fields, "No fields provided\n");
	al_mod_assert(ctrl_entry);

	/* Next protocol available */
	if (ctrl_entry->next_proto_avaliable)
		data[0] |= EC_EPE_ACT_DATA_1_NEXT_PROTO_AVAIL;

	/* Offset to next protocol */
	AL_REG_FIELD_SET(data[0],
		EC_EPE_ACT_DATA_1_NEXT_PROTO_OFFSET_MASK,
		EC_EPE_ACT_DATA_1_NEXT_PROTO_OFFSET_SHIFT,
		ctrl_entry->offset_to_next_proto);

	/* Next stage */
	AL_REG_FIELD_SET(data[4],
		EC_EPE_ACT_DATA_5_NEXT_STAGE_MASK,
		EC_EPE_ACT_DATA_5_NEXT_STAGE_SHIFT,
		ctrl_entry->next_stage);

	/* Next branch */
	AL_REG_FIELD_SET(data[4],
		EC_EPE_ACT_DATA_5_NEXT_BRANCH_ID_SET_MASK,
		EC_EPE_ACT_DATA_5_NEXT_BRANCH_ID_SET_SHIFT,
		ctrl_entry->next_branch_mask);
	AL_REG_FIELD_SET(data[4],
		EC_EPE_ACT_DATA_5_NEXT_BRANCH_ID_VAL_MASK,
		EC_EPE_ACT_DATA_5_NEXT_BRANCH_ID_VAL_SHIFT,
		ctrl_entry->next_branch_val);

	/* Default next protocol index */
	AL_REG_FIELD_SET(data[0],
		EC_EPE_ACT_DATA_1_DEFAULT_NEXT_PROTO_MASK,
		EC_EPE_ACT_DATA_1_DEFAULT_NEXT_PROTO_SHIFT,
		ctrl_entry->default_next_proto_idx);

	/* Default next protocol index */
	if (handle->eth_rev_id >= AL_ETH_REV_ID_4) {
		AL_REG_FIELD_SET(data[0],
			EC_RMP_RMP_EPE_ACT_TABLE_DATA_1_DEFAULT_NEXT_ACT_TABLE_ADDR_MASK,
			EC_RMP_RMP_EPE_ACT_TABLE_DATA_1_DEFAULT_NEXT_ACT_TABLE_ADDR_SHIFT,
			ctrl_entry->default_next_ctrl_idx);
	}

	/* End parsing control */
	if (ctrl_entry->end_ctrl == EPE_CTRL_END_NEXT_PARSER ||
		ctrl_entry->end_ctrl == EPE_CTRL_END_FINISH)
		data[4] |= EC_EPE_ACT_DATA_5_END_OF_PARSING;

	if (ctrl_entry->end_ctrl == EPE_CTRL_END_CONTINUE ||
		ctrl_entry->end_ctrl == EPE_CTRL_END_FINISH)
		data[5] |= EC_EPE_ACT_DATA_6_NEXT_PARSE_DIS;

	/* Header length calculation */
	AL_REG_FIELD_SET(data[3],
		EC_EPE_ACT_DATA_4_HDR_LEN_OFFSET_MASK,
		EC_EPE_ACT_DATA_4_HDR_LEN_OFFSET_SHIFT,
		ctrl_entry->hdr_len_calc.offset);
	AL_REG_FIELD_SET(data[3],
		EC_EPE_ACT_DATA_4_HDR_LEN_SIZE_MASK,
		EC_EPE_ACT_DATA_4_HDR_LEN_SIZE_SHIFT,
		ctrl_entry->hdr_len_calc.size);

	AL_REG_FIELD_SET(data[3],
		EC_EPE_ACT_DATA_4_HDR_LEN_UNITS_MASK,
		EC_EPE_ACT_DATA_4_HDR_LEN_UNITS_SHIFT,
		ctrl_entry->hdr_len_calc.shift);
	AL_REG_FIELD_SET(data[3],
		EC_EPE_ACT_DATA_4_HDR_LEN_COMMAND_MASK,
		EC_EPE_ACT_DATA_4_HDR_LEN_COMMAND_SHIFT,
		ctrl_entry->hdr_len_calc.op);
	AL_REG_FIELD_SET(data[3],
		EC_EPE_ACT_DATA_4_DEFAULT_HDR_LEN_MASK,
		EC_EPE_ACT_DATA_4_DEFAULT_HDR_LEN_SHIFT,
		ctrl_entry->hdr_len_calc.add_val);

	/* Protocol index write command */
	if (ctrl_entry->proto_idx_cmd.layer != EPE_CTRL_LAYER_NONE) {
		struct al_mod_eth_field *field = NULL;

		if (ctrl_entry->proto_idx_cmd.layer == EPE_CTRL_LAYER_L3)
			field = handle->fields[EPE_FIELD_WRITE_L3_PROTO_IDX];
		else
			field = handle->fields[EPE_FIELD_WRITE_L4_PROTO_IDX];

		al_mod_assert_msg(field, "Missing L3/L4 field for protocol index command\n");
		al_mod_assert_msg((field->offset % BITS_TO_BYTE == 0),
			"L3/L4 field offset for protocol index command must be byte aligned\n");

		data[0] |= EC_EPE_ACT_DATA_1_PROTO_WR;

		AL_REG_FIELD_SET(data[0],
			EC_EPE_ACT_DATA_1_PROTO_WR_PTR_MASK,
			EC_EPE_ACT_DATA_1_PROTO_WR_PTR_SHIFT,
			field->offset / BITS_TO_BYTE);
	}

	if (ctrl_entry->proto_idx_cmd.data_src == EPE_CTRL_DATA_SRC_CURRENT)
		data[0] |= EC_EPE_ACT_DATA_1_PROTO_WR_SEL;

	/* Header offset write command */
	if (ctrl_entry->hdr_offset_cmd.layer != EPE_CTRL_LAYER_NONE) {
		struct al_mod_eth_field *field = NULL;

		if (ctrl_entry->hdr_offset_cmd.layer == EPE_CTRL_LAYER_L3)
			field = handle->fields[EPE_FIELD_WRITE_L3_HDR_OFFSET];
		else
			field = handle->fields[EPE_FIELD_WRITE_L4_HDR_OFFSET];

		al_mod_assert_msg(field, "Missing L3/L4 field for header offset command\n");
		al_mod_assert_msg((field->offset % BITS_TO_BYTE == 0),
			"L3/L4 field offset for header offset command must be byte aligned\n");

		data[5] |= EC_EPE_ACT_DATA_6_HDR_OFFSET_WR;

		AL_REG_FIELD_SET(data[5],
			EC_EPE_ACT_DATA_6_HDR_OFFSET_WR_PTR_MASK,
			EC_EPE_ACT_DATA_6_HDR_OFFSET_WR_PTR_SHIFT,
			field->offset / BITS_TO_BYTE);
	}

	if (ctrl_entry->hdr_offset_cmd.data_src == EPE_CTRL_DATA_SRC_CURRENT)
		data[0] |= EC_EPE_ACT_DATA_1_HDR_OFFSET_WR_SEL;

	/* Header length write command */
	if (ctrl_entry->hdr_len_cmd.layer != EPE_CTRL_LAYER_NONE) {
		struct al_mod_eth_field *field = NULL;

		if (ctrl_entry->hdr_len_cmd.layer == EPE_CTRL_LAYER_L3)
			field = handle->fields[EPE_FIELD_WRITE_L3_HDR_LEN];
		else
			field = handle->fields[EPE_FIELD_WRITE_L4_HDR_LEN];

		al_mod_assert_msg(field, "Missing L3/L4 field for header length command\n");
		al_mod_assert_msg((field->offset % BITS_TO_BYTE == 0),
			"L3/L4 field offset for header length command must be byte aligned\n");

		data[5] |= EC_EPE_ACT_DATA_6_HDR_LEN_WR;

		AL_REG_FIELD_SET(data[5],
				EC_EPE_ACT_DATA_6_HDR_LEN_WR_PTR_MASK,
				EC_EPE_ACT_DATA_6_HDR_LEN_WR_PTR_SHIFT,
			field->offset / BITS_TO_BYTE);
	}

	if (ctrl_entry->hdr_len_cmd.data_src == EPE_CTRL_DATA_SRC_CURRENT)
		data[0] |= EC_EPE_ACT_DATA_1_HDR_LEN_WR_SEL;

	/* Data commands 1-2 (data[1] - data[2]) */
	/* Data commands 3-4 (data[6] - data[7]) */
	for (i = 0; i < EPE_CTRL_DATA_CMD_MAX_BY_REV(handle->eth_rev_id); i++)
		epe_ctrl_data_cmd_set(handle, ctrl_entry, i, data);

	/* Control bit */
	AL_REG_FIELD_SET(data[4],
		EC_EPE_ACT_DATA_5_NEXT_CTRL_BIT_SET_MASK,
		EC_EPE_ACT_DATA_5_NEXT_CTRL_BIT_SET_SHIFT,
		ctrl_entry->ctrl_bits_val);
	AL_REG_FIELD_SET(data[4],
		EC_EPE_ACT_DATA_5_NEXT_CTRL_BIT_VAL_MASK,
		EC_EPE_ACT_DATA_5_NEXT_CTRL_BIT_VAL_SHIFT,
		ctrl_entry->ctrl_bits_mask);

	/* Control bit 2 */
	if (handle->eth_rev_id >= AL_ETH_REV_ID_4) {
		AL_REG_FIELD_SET(data[8],
			EC_RMP_RMP_EPE_ACT_TABLE_DATA_9_CONTROL_BIT2_SET_MASK,
			EC_RMP_RMP_EPE_ACT_TABLE_DATA_9_CONTROL_BIT2_SET_SHIFT,
			ctrl_entry->ctrl_bits2_val);
		AL_REG_FIELD_SET(data[8],
			EC_RMP_RMP_EPE_ACT_TABLE_DATA_9_CONTROL_BIT2_VALUE_MASK,
			EC_RMP_RMP_EPE_ACT_TABLE_DATA_9_CONTROL_BIT2_VALUE_SHIFT,
			ctrl_entry->ctrl_bits2_mask);
	}

	al_mod_eth_epe_ctrl_entry_raw_set(handle, ctrl_entry->idx, &ctrl_entry_raw);
}

void al_mod_eth_epe_ctrl_entry_raw_set(struct al_mod_eth_epe_handle *handle,
	unsigned int idx, struct al_mod_eth_epe_control_entry *ctrl_entry_raw)
{
	uint32_t *data = ctrl_entry_raw->data;

	al_mod_assert(handle);
	al_mod_assert(ctrl_entry_raw);

	if (handle->eth_rev_id <= AL_ETH_REV_ID_3) {
		struct al_mod_ec_regs *ec_regs_base = handle->regs_base;
		struct al_mod_ec_epe *epe_regs = &ec_regs_base->epe[handle->type];

		al_mod_reg_write32(&epe_regs->act_table_addr,
			idx & EC_EPE_ACT_TABLE_ADDR_VAL_MASK);
		al_mod_local_data_memory_barrier();
		al_mod_reg_write32(&epe_regs->act_table_data_2, data[1]);
		al_mod_reg_write32(&epe_regs->act_table_data_3, data[2]);
		al_mod_reg_write32(&epe_regs->act_table_data_4, data[3]);
		al_mod_reg_write32(&epe_regs->act_table_data_5, data[4]);
		al_mod_reg_write32(&epe_regs->act_table_data_6, data[5]);
		/* Trigger write */
		al_mod_reg_write32(&epe_regs->act_table_data_1, data[0]);
	} else {
		struct al_mod_ec_rmp_regs *rmp_regs_base = handle->regs_base;
		uint32_t reg = 0;

		AL_REG_FIELD_SET(reg,
			EC_RMP_RMP_EPE_ACT_TABLE_ADDR_MASK,
			EC_RMP_RMP_EPE_ACT_TABLE_ADDR_SHIFT,
			idx);

		if (handle->type == EPE_TYPE_INNER)
			reg |= EC_RMP_RMP_EPE_ACT_TABLE_PARSER_NUM;

		al_mod_reg_write32(&rmp_regs_base->rmp.epe_act_table, reg);
		al_mod_reg_write32(&rmp_regs_base->rmp.epe_act_table_data_1, data[0]);
		al_mod_reg_write32(&rmp_regs_base->rmp.epe_act_table_data_2, data[1]);
		al_mod_reg_write32(&rmp_regs_base->rmp.epe_act_table_data_3, data[2]);
		al_mod_reg_write32(&rmp_regs_base->rmp.epe_act_table_data_4, data[3]);
		al_mod_reg_write32(&rmp_regs_base->rmp.epe_act_table_data_5, data[4]);
		al_mod_reg_write32(&rmp_regs_base->rmp.epe_act_table_data_6, data[5]);
		al_mod_reg_write32(&rmp_regs_base->rmp.epe_act_table_data_7, data[6]);
		al_mod_reg_write32(&rmp_regs_base->rmp.epe_act_table_data_8, data[7]);
		/* Trigger write */
		al_mod_reg_write32(&rmp_regs_base->rmp.epe_act_table_data_9, data[8]);
	}
}

#define AL_ETH_V3_EPE_TABLE_SIZE	32
#define AL_ETH_V3_EPE_CTRL_TABLE_NUM	2
#define AL_ETH_V3_EPE_ENTRIES_NUM 26

void al_mod_eth_epe_v3_entry_get(struct al_mod_eth_epe_handle *handle, uint32_t idx,
			struct al_mod_eth_epe_p_reg_entry *reg_entry,
			struct al_mod_eth_epe_control_entry *control_entry,
			unsigned int control_table_idx)
{
	struct al_mod_ec_regs *ec_regs_base;

	al_mod_assert(handle);
	al_mod_assert(reg_entry);
	al_mod_assert(control_entry);
	al_mod_assert(idx < AL_ETH_V3_EPE_TABLE_SIZE);
	al_mod_assert(control_table_idx < AL_ETH_V3_EPE_CTRL_TABLE_NUM);

	al_mod_assert(handle->eth_rev_id <= AL_ETH_REV_ID_3);

	ec_regs_base = handle->regs_base;

	reg_entry->data = al_mod_reg_read32(&ec_regs_base->epe_p[idx].comp_data);
	reg_entry->mask = al_mod_reg_read32(&ec_regs_base->epe_p[idx].comp_mask);
	reg_entry->ctrl = al_mod_reg_read32(&ec_regs_base->epe_p[idx].comp_ctrl);

	al_mod_reg_write32(&ec_regs_base->epe[control_table_idx].act_table_addr, idx);
	al_mod_local_data_memory_barrier();
	control_entry->data[0] =
		al_mod_reg_read32(&ec_regs_base->epe[control_table_idx].act_table_data_1);
	control_entry->data[1] =
		al_mod_reg_read32(&ec_regs_base->epe[control_table_idx].act_table_data_2);
	control_entry->data[2] =
		al_mod_reg_read32(&ec_regs_base->epe[control_table_idx].act_table_data_3);
	control_entry->data[3] =
		al_mod_reg_read32(&ec_regs_base->epe[control_table_idx].act_table_data_4);
	control_entry->data[4] =
		al_mod_reg_read32(&ec_regs_base->epe[control_table_idx].act_table_data_5);
	control_entry->data[5] =
		al_mod_reg_read32(&ec_regs_base->epe[control_table_idx].act_table_data_6);
}

int al_mod_eth_epe_entry_range_update(struct al_mod_hal_eth_adapter *adapter,
				  struct al_mod_eth_epe_p_reg_entry epe_p_reg_entries[],
				  struct al_mod_eth_epe_control_entry epe_control_entries[],
				  unsigned int num_entries,
				  unsigned int first_idx)
{
	unsigned int idx;

	al_mod_assert(adapter);
	al_mod_assert(epe_p_reg_entries);
	al_mod_assert(epe_control_entries);

	if (num_entries == 0) {
		al_mod_err("[%s] %s: num_of_entries is 0\n", adapter->name, __func__);
		return -EINVAL;
	}

	if ((first_idx + num_entries) > AL_ETH_EPE_TABLE_SIZE) {
		al_mod_err("[%s] %s: the requested range exceeds the EPE tables' size\n",
		       adapter->name, __func__);
		return -EINVAL;
	}

	if (first_idx <= AL_ETH_EPE_LAST_USED_ENTRY) {
		al_mod_warn("[%s] %s: overwriting an entry which is part of default configuration in EPE comp table. next free entry is %d\n",
			adapter->name, __func__, (AL_ETH_EPE_LAST_USED_ENTRY + 1));
	}

	if ((first_idx + num_entries) == AL_ETH_PROTO_ID_DEFAULT) {
		al_mod_warn("[%s] %s: overwriting default entry for undetected protocols\n",
			adapter->name, __func__);
	}

	for (idx = 0; idx < num_entries; idx++)
		al_mod_eth_epe_entry_set(adapter,
				     (first_idx + idx),
				     &epe_p_reg_entries[idx],
				     &epe_control_entries[idx]);

	return 0;
}

#define AL_ETH_EPE_GROUP_A_ERROR_INTS \
	(AL_ETH_EPE_GROUP_A_RMP_HDR_INPUT_FIFO_MEM_PAR_ERR_0 | \
	AL_ETH_EPE_GROUP_A_RMP_HDR_INPUT_FIFO_MEM_PAR_ERR_1 | \
	AL_ETH_EPE_GROUP_A_RMP_HDR_INPUT_FIFO_MEM_PAR_ERR_2 | \
	AL_ETH_EPE_GROUP_A_RMP_HDR_INPUT_FIFO_MEM_PAR_ERR_3 | \
	AL_ETH_EPE_GROUP_A_RMP_HDR_INPUT_FIFO_MEM_PAR_ERR_4 | \
	AL_ETH_EPE_GROUP_A_RMP_HDR_INPUT_FIFO_MEM_PAR_ERR_5 | \
	AL_ETH_EPE_GROUP_A_RMP_HDR_INPUT_FIFO_MEM_PAR_ERR_6 | \
	AL_ETH_EPE_GROUP_A_RMP_HDR_INPUT_FIFO_MEM_PAR_ERR_7 | \
	AL_ETH_EPE_GROUP_A_RMP_DATA_FIFO_LO_MEM_PAR_ERR | \
	AL_ETH_EPE_GROUP_A_RMP_DATA_FIFO_HI_MEM_PAR_ERR)

#define AL_ETH_EPE_GROUP_B_ERROR_INTS \
	(AL_ETH_EPE_GROUP_B_EC_ERR_RPE_IN_EOP_IN_IDLE_0 | \
	AL_ETH_EPE_GROUP_B_EC_ERR_RPE_IN_EOP_IN_IDLE_1 | \
	AL_ETH_EPE_GROUP_B_EC_ERR_RPE_IN_EOP_IN_IDLE_2 | \
	AL_ETH_EPE_GROUP_B_EC_ERR_RPE_IN_EOP_IN_IDLE_3 | \
	AL_ETH_EPE_GROUP_B_EC_ERR_RPE_IN_EOP_IN_IDLE_4 | \
	AL_ETH_EPE_GROUP_B_EC_ERR_RPE_IN_EOP_IN_IDLE_5 | \
	AL_ETH_EPE_GROUP_B_EC_ERR_RPE_IN_EOP_IN_IDLE_6 | \
	AL_ETH_EPE_GROUP_B_EC_ERR_RPE_IN_EOP_IN_IDLE_7 | \
	AL_ETH_EPE_GROUP_B_HDR_INPUT_FIFO_ERR_IN_EOP_IN_IDLE_0 | \
	AL_ETH_EPE_GROUP_B_HDR_INPUT_FIFO_ERR_IN_EOP_IN_IDLE_1 | \
	AL_ETH_EPE_GROUP_B_HDR_INPUT_FIFO_ERR_IN_EOP_IN_IDLE_2 | \
	AL_ETH_EPE_GROUP_B_HDR_INPUT_FIFO_ERR_IN_EOP_IN_IDLE_3)

#define AL_ETH_EPE_GROUP_C_ERROR_INTS \
	(AL_ETH_EPE_GROUP_C_EC_ERR_RPE_IN_SOP_IN_PKT_0 | \
	AL_ETH_EPE_GROUP_C_EC_ERR_RPE_IN_SOP_IN_PKT_1 | \
	AL_ETH_EPE_GROUP_C_EC_ERR_RPE_IN_SOP_IN_PKT_2 | \
	AL_ETH_EPE_GROUP_C_EC_ERR_RPE_IN_SOP_IN_PKT_3 | \
	AL_ETH_EPE_GROUP_C_EC_ERR_RPE_IN_SOP_IN_PKT_4 | \
	AL_ETH_EPE_GROUP_C_EC_ERR_RPE_IN_SOP_IN_PKT_5 | \
	AL_ETH_EPE_GROUP_C_EC_ERR_RPE_IN_SOP_IN_PKT_6 | \
	AL_ETH_EPE_GROUP_C_EC_ERR_RPE_IN_SOP_IN_PKT_7 | \
	AL_ETH_EPE_GROUP_C_HDR_INPUT_FIFO_ERR_IN_SOP_IN_PKT_0 | \
	AL_ETH_EPE_GROUP_C_HDR_INPUT_FIFO_ERR_IN_SOP_IN_PKT_1 | \
	AL_ETH_EPE_GROUP_C_HDR_INPUT_FIFO_ERR_IN_SOP_IN_PKT_2 | \
	AL_ETH_EPE_GROUP_C_HDR_INPUT_FIFO_ERR_IN_SOP_IN_PKT_3)

#define AL_ETH_EPE_GROUP_D_ERROR_INTS \
	(AL_ETH_EPE_GROUP_D_EC_ERR_RPE_OUT_EOP_HDR_EMPTY_0 | \
	AL_ETH_EPE_GROUP_D_EC_ERR_RPE_OUT_EOP_HDR_EMPTY_1 | \
	AL_ETH_EPE_GROUP_D_EC_ERR_RPE_OUT_EOP_HDR_EMPTY_2 | \
	AL_ETH_EPE_GROUP_D_EC_ERR_RPE_OUT_EOP_HDR_EMPTY_3 | \
	AL_ETH_EPE_GROUP_D_EC_ERR_RPE_OUT_EOP_HDR_EMPTY_4 | \
	AL_ETH_EPE_GROUP_D_EC_ERR_RPE_OUT_EOP_HDR_EMPTY_5 | \
	AL_ETH_EPE_GROUP_D_EC_ERR_RPE_OUT_EOP_HDR_EMPTY_6 | \
	AL_ETH_EPE_GROUP_D_EC_ERR_RPE_OUT_EOP_HDR_EMPTY_7 | \
	AL_ETH_EPE_GROUP_D_HDR_INPUT_FIFO_ERR_OUT_EOP_HDR_EMPTY_0 | \
	AL_ETH_EPE_GROUP_D_HDR_INPUT_FIFO_ERR_OUT_EOP_HDR_EMPTY_1 | \
	AL_ETH_EPE_GROUP_D_HDR_INPUT_FIFO_ERR_OUT_EOP_HDR_EMPTY_2 | \
	AL_ETH_EPE_GROUP_D_HDR_INPUT_FIFO_ERR_OUT_EOP_HDR_EMPTY_3)

void al_mod_eth_epe_error_ints_masks_get(struct al_mod_eth_epe_handle *handle,
					uint32_t *epe_a_mask,
					uint32_t *epe_b_mask,
					uint32_t *epe_c_mask,
					uint32_t *epe_d_mask)
{
	al_mod_assert(epe_a_mask);
	al_mod_assert(epe_b_mask);
	al_mod_assert(epe_c_mask);
	al_mod_assert(epe_d_mask);

	al_mod_assert(handle);
	al_mod_assert(handle->eth_rev_id >= AL_ETH_REV_ID_4);

	*epe_a_mask = AL_ETH_EPE_GROUP_A_ERROR_INTS;
	*epe_b_mask = AL_ETH_EPE_GROUP_B_ERROR_INTS;
	*epe_c_mask = AL_ETH_EPE_GROUP_C_ERROR_INTS;
	*epe_d_mask = AL_ETH_EPE_GROUP_D_ERROR_INTS;
}

static void al_mod_eth_epe_error_ints_unmask(struct al_mod_eth_epe_handle *handle)
{
	uint32_t epe_a_mask;
	uint32_t epe_b_mask;
	uint32_t epe_c_mask;
	uint32_t epe_d_mask;

	al_mod_assert(handle);
	al_mod_assert(handle->ints_base);

	al_mod_eth_epe_error_ints_masks_get(handle,
					&epe_a_mask,
					&epe_b_mask,
					&epe_c_mask,
					&epe_d_mask);

	/* EPE IOFIC config */
	al_mod_iofic_config(handle->ints_base,
			AL_INT_GROUP_A,
			INT_CONTROL_GRP_MASK_MSI_X);
	al_mod_iofic_config(handle->ints_base,
			AL_INT_GROUP_B,
			INT_CONTROL_GRP_MASK_MSI_X);
	al_mod_iofic_config(handle->ints_base,
			AL_INT_GROUP_C,
			INT_CONTROL_GRP_MASK_MSI_X);
	al_mod_iofic_config(handle->ints_base,
			AL_INT_GROUP_D,
			INT_CONTROL_GRP_MASK_MSI_X);

	/* clear old unvalid parity interrupts */
	al_mod_iofic_clear_cause(handle->ints_base,
				AL_INT_GROUP_A,
				epe_a_mask);
	al_mod_iofic_clear_cause(handle->ints_base,
				AL_INT_GROUP_B,
				epe_b_mask);
	al_mod_iofic_clear_cause(handle->ints_base,
				AL_INT_GROUP_C,
				epe_c_mask);
	al_mod_iofic_clear_cause(handle->ints_base,
				AL_INT_GROUP_D,
				epe_d_mask);

	/* unmask interrupts */
	al_mod_iofic_unmask(handle->ints_base,
			AL_INT_GROUP_A,
			epe_a_mask);
	al_mod_iofic_unmask(handle->ints_base,
			AL_INT_GROUP_B,
			epe_b_mask);
	al_mod_iofic_unmask(handle->ints_base,
			AL_INT_GROUP_C,
			epe_c_mask);
	al_mod_iofic_unmask(handle->ints_base,
			AL_INT_GROUP_D,
			epe_d_mask);
}
