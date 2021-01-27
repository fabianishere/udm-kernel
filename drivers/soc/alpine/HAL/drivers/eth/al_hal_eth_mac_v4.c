/*******************************************************************************
Copyright (C) 2016 Annapurna Labs Ltd.

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
 * @file   al_hal_eth_mac_v4.c
 *
 * @brief  Ethernet MAC v4 HAL
 *
 */

#include "al_hal_eth.h"
#include "al_hal_eth_common.h"
#include "al_hal_eth_mac.h"
#include "al_hal_eth_mac_internal.h"
#include "al_hal_eth_mac_regs.h"
#include "al_hal_eth_mac_v4_regs.h"
#include "al_hal_eth_mac_v4_common_regs.h"
#include "al_hal_iofic.h"

#define ETH_MAC_GEN_LED_CFG_BLINK_TIMER_VAL 5
#define ETH_MAC_GEN_LED_CFG_ACT_TIMER_VAL 7

/*** AFIFO defines ***/
#define ETH_MAC_V4_GEN_V4_MIN_PKT_SIZE 0x0020
#define ETH_MAC_V4_GEN_V4_MAX_PKT_SIZE 0x2710

#define ETH_MAC_V4_GEN_V4_BUS_W_64 0x0040
#define ETH_MAC_V4_GEN_V4_BUS_W_256 0x0100

#define ETH_MAC_V4_GEN_V4_BUS_W_F_1 0x0000
#define ETH_MAC_V4_GEN_V4_BUS_W_F_2 0x0001
#define ETH_MAC_V4_GEN_V4_BUS_W_F_3 0x0002
#define ETH_MAC_V4_GEN_V4_BUS_W_F_4 0x0003

#define ETH_MAC_V4_GEN_V4_AFULL_ABSORB_ENTRIES_4 0x4

#define ETH_MAC_V4_GEN_V4_SAF_THRESHOLD_8 0x8

/*** 50G MAC Core registers addresses ***/
/* revision */
#define ETH_MAC_GEN_V4_MAC_50G_REVISION_ADDR	0x00000000

/* command config */
#define ETH_MAC_GEN_V4_MAC_50G_COMMAND_CONFIG_ADDR		0x00000008
#define ETH_MAC_GEN_V4_MAC_50G_COMMAND_CONFIG_TX_ENA		AL_BIT(0)
#define ETH_MAC_GEN_V4_MAC_50G_COMMAND_CONFIG_RX_ENA		AL_BIT(1)
#define ETH_MAC_GEN_V4_MAC_50G_COMMAND_CONFIG_CRC_FWD		AL_BIT(6)
#define ETH_MAC_GEN_V4_MAC_50G_COMMAND_CONFIG_LOOP_ENA		AL_BIT(10)
#define ETH_MAC_GEN_V4_MAC_50G_COMMAND_CONFIG_TX_PAD_EN		AL_BIT(11)
#define ETH_MAC_GEN_V4_MAC_50G_COMMAND_CONFIG_CNTL_FRM_ENA	AL_BIT(13)
#define ETH_MAC_GEN_V4_MAC_50G_COMMAND_CONFIG_PFC_MODE		AL_BIT(19)
#define ETH_MAC_GEN_V4_MAC_50G_COMMAND_CONFIG_TX_FLUSH		AL_BIT(22)

/* mac address */
#define ETH_MAC_GEN_V4_MAC_50G_MAC_ADDR_0_ADDR		0x0000000C
#define ETH_MAC_GEN_V4_MAC_50G_MAC_ADDR_1_ADDR		0x00000010

/* frame length */
#define ETH_MAC_GEN_V4_MAC_50G_FRM_LENGTH_ADDR		0x00000014

/* pause quanta / threshold */
#define ETH_MAC_GEN_V4_MAC_50G_CL01_PAUSE_QUANTA_ADDR	0x00000054
#define ETH_MAC_GEN_V4_MAC_50G_CL23_PAUSE_QUANTA_ADDR	0x00000058
#define ETH_MAC_GEN_V4_MAC_50G_CL45_PAUSE_QUANTA_ADDR	0x0000005C
#define ETH_MAC_GEN_V4_MAC_50G_CL67_PAUSE_QUANTA_ADDR	0x00000060
#define ETH_MAC_GEN_V4_MAC_50G_CL01_QUANTA_THRESH_ADDR	0x00000064
#define ETH_MAC_GEN_V4_MAC_50G_CL23_QUANTA_THRESH_ADDR	0x00000068
#define ETH_MAC_GEN_V4_MAC_50G_CL45_QUANTA_THRESH_ADDR	0x0000006C
#define ETH_MAC_GEN_V4_MAC_50G_CL67_QUANTA_THRESH_ADDR	0x00000070

#define ETH_MAC_GEN_V4_MAC_50G_XGMII_MODE_ADDR		0x00000080
#define ETH_MAC_GEN_V4_MAC_50G_XGMII_MODE_ENABLE	AL_BIT(0)

/*** 100G MAC Core registers addresses ***/
/* revision */
#define ETH_MAC_GEN_V4_MAC_100G_REVISION_ADDR	0x00000000

/* command config */
#define ETH_MAC_GEN_V4_MAC_100G_COMMAND_CONFIG_ADDR		0x00000008
#define ETH_MAC_GEN_V4_MAC_100G_COMMAND_CONFIG_TX_ENA		AL_BIT(0)
#define ETH_MAC_GEN_V4_MAC_100G_COMMAND_CONFIG_RX_ENA		AL_BIT(1)
#define ETH_MAC_GEN_V4_MAC_100G_COMMAND_CONFIG_PROMIS_EN	AL_BIT(4)
#define ETH_MAC_GEN_V4_MAC_100G_COMMAND_CONFIG_CRC_FWD		AL_BIT(6)
#define ETH_MAC_GEN_V4_MAC_100G_COMMAND_CONFIG_LOOP_ENA		AL_BIT(10)
#define ETH_MAC_GEN_V4_MAC_100G_COMMAND_CONFIG_TX_PAD_EN	AL_BIT(11)
#define ETH_MAC_GEN_V4_MAC_100G_COMMAND_CONFIG_CNTL_FRM_ENA	AL_BIT(13)
#define ETH_MAC_GEN_V4_MAC_100G_COMMAND_CONFIG_NO_LGTH_CHECK	AL_BIT(17)
#define ETH_MAC_GEN_V4_MAC_100G_COMMAND_CONFIG_PFC_MODE		AL_BIT(19)
#define ETH_MAC_GEN_V4_MAC_100G_COMMAND_CONFIG_TX_FLUSH		AL_BIT(22)

/* mac address */
#define ETH_MAC_GEN_V4_MAC_100G_MAC_ADDR_0_ADDR		0x0000000C
#define ETH_MAC_GEN_V4_MAC_100G_MAC_ADDR_1_ADDR		0x00000010

/* frame length */
#define ETH_MAC_GEN_V4_MAC_100G_FRM_LENGTH_ADDR		0x00000014

/* pause quanta / threshold */
#define ETH_MAC_GEN_V4_MAC_100G_CL01_PAUSE_QUANTA_ADDR	0x00000054
#define ETH_MAC_GEN_V4_MAC_100G_CL23_PAUSE_QUANTA_ADDR	0x00000058
#define ETH_MAC_GEN_V4_MAC_100G_CL45_PAUSE_QUANTA_ADDR	0x0000005C
#define ETH_MAC_GEN_V4_MAC_100G_CL67_PAUSE_QUANTA_ADDR	0x00000060
#define ETH_MAC_GEN_V4_MAC_100G_CL01_QUANTA_THRESH_ADDR	0x00000064
#define ETH_MAC_GEN_V4_MAC_100G_CL23_QUANTA_THRESH_ADDR	0x00000068
#define ETH_MAC_GEN_V4_MAC_100G_CL45_QUANTA_THRESH_ADDR	0x0000006C
#define ETH_MAC_GEN_V4_MAC_100G_CL67_QUANTA_THRESH_ADDR	0x00000070

#define ETH_MAC_GEN_V4_MAC_100G_XGMII_MODE_ADDR 0x00000080

/*** 50G PCS Core registers addresses ***/
#define PCS_ADDR(w) ((w) << 1)

#define ETH_MAC_GEN_V4_PCS_50G_CONTROL1_ADDR		PCS_ADDR(0x00000000)
#define ETH_MAC_GEN_V4_PCS_50G_CONTROL1_RESET		AL_BIT(15)
#define ETH_MAC_GEN_V4_PCS_50G_CONTROL1_LOOPBACK	AL_BIT(14)

#define ETH_MAC_GEN_V4_PCS_50G_CONTROL2_ADDR		PCS_ADDR(0x00000007)
#define ETH_MAC_GEN_V4_PCS_50G_CONTROL2_40GBASER	0x00000005

#define ETH_MAC_GEN_V4_PCS_50G_VL_INTVL_ADDR		PCS_ADDR(0x00008002)
#define ETH_MAC_GEN_V4_PCS_50G_VL_INTVL_1023		0x000003FF
#define ETH_MAC_GEN_V4_PCS_50G_VL_INTVL_1279		0x000004FF
#define ETH_MAC_GEN_V4_PCS_50G_VL0_0_ADDR		PCS_ADDR(0x00008008)
#define ETH_MAC_GEN_V4_PCS_50G_VL0_1_ADDR		PCS_ADDR(0x00008009)
#define ETH_MAC_GEN_V4_PCS_50G_VL0_0_MARKER_50G		0x7690
#define ETH_MAC_GEN_V4_PCS_50G_VL0_1_MARKER_50G		0x47
#define ETH_MAC_GEN_V4_PCS_50G_VL0_0_MARKER_25G		0x68C1
#define ETH_MAC_GEN_V4_PCS_50G_VL0_1_MARKER_25G		0x21
#define ETH_MAC_GEN_V4_PCS_50G_RXLAUI_CONFIG_ADDR	PCS_ADDR(0x0008004)
#define ETH_MAC_GEN_V4_PCS_50G_RXLAUI_CONFIG_USE_2LANE	AL_BIT(0)

#define ETH_MAC_GEN_V4_PCS_50G_PCS_MODE_ADDR		PCS_ADDR(0x00008010)
#define ETH_MAC_GEN_V4_PCS_50G_PCS_MODE_ENA_CLAUSE49	AL_BIT(0)
#define ETH_MAC_GEN_V4_PCS_50G_PCS_MODE_DISABLE_MLD	AL_BIT(1)

/*** 100G PCS Core registers addresses ***/
#define ETH_MAC_GEN_V4_PCS_100G_CONTROL1_ADDR		PCS_ADDR(0x00000000)
#define ETH_MAC_GEN_V4_PCS_100G_CONTROL1_RESET		AL_BIT(15)
#define ETH_MAC_GEN_V4_PCS_100G_CONTROL1_LOOPBACK	AL_BIT(14)

#define ETH_MAC_GEN_V4_PCS_100G_VL_INTVL_ADDR		PCS_ADDR(0x00008002)
#define ETH_MAC_GEN_V4_PCS_100G_VL_INTVL_1023		0x000003FF
#define ETH_MAC_GEN_V4_PCS_100G_VL_INTVL_1279		0x000004FF
/*** RS-FEC Core registers addresses ***/
#define ETH_MAC_GEN_V4_RS_FEC_CONTROL_CHAN0_ADDR	PCS_ADDR(0x00000000)
#define ETH_MAC_GEN_V4_RS_FEC_CONTROL_CHAN1_ADDR	PCS_ADDR(0x00000040)
#define ETH_MAC_GEN_V4_RS_FEC_CONTROL_CHAN2_ADDR	PCS_ADDR(0x00000048)
#define ETH_MAC_GEN_V4_RS_FEC_CONTROL_CHAN3_ADDR	PCS_ADDR(0x00000050)
#define ETH_MAC_GEN_V4_RS_FEC_BYPASS_CORRECTION		AL_BIT(0)
#define ETH_MAC_GEN_V4_RS_FEC_BYPASS_ERROR		AL_BIT(1)
#define ETH_MAC_GEN_V4_RS_FEC_ENABLE			AL_BIT(2)
#define ETH_MAC_GEN_V4_RS_FEC_VENDOR_CONTROL_ADDR	PCS_ADDR(0x00000080)
#define ETH_MAC_GEN_V4_RS_FEC_CH0_1LANE_MODE		AL_BIT(4)
#define ETH_MAC_GEN_V4_RS_FEC_CH2_1LANE_MODE		AL_BIT(5)

/**
 * MDIO
 */

/* mdio cfg status */
#define ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_CFG_STATUS_ADDR	0x00000030
#define ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_CFG_STATUS_BUSY	AL_BIT(0)
#define ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_CFG_STATUS_READ_ERR	AL_BIT(1)
#define ETH_MAC_GEN_V4_MAC_50G_MDIO_CFG_STATUS_HOLD_TIME_MASK	0x0000001c
#define ETH_MAC_GEN_V4_MAC_50G_MDIO_CFG_STATUS_HOLD_TIME_SHIFT	2
#define ETH_MAC_GEN_V4_MAC_100G_MDIO_CFG_STATUS_HOLD_TIME_MASK	0x0000000c
#define ETH_MAC_GEN_V4_MAC_100G_MDIO_CFG_STATUS_HOLD_TIME_SHIFT	2

/** mdio cmd */
#define ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_COMMAND_ADDR				0x00000034
#define ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_COMMAND_CLAUSE22_REG_ADDR_MASK		0x0000001f
#define ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_COMMAND_CLAUSE22_REG_ADDR_SHIFT	0
#define ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_COMMAND_CLAUSE22_PHY_ADDR_MASK		0x000003e0
#define ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_COMMAND_CLAUSE22_PHY_ADDR_SHIFT	5
#define ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_COMMAND_CLAUSE45_DEV_ADDR_MASK		0x0000001f
#define ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_COMMAND_CLAUSE45_DEV_ADDR_SHIFT	0
#define ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_COMMAND_CLAUSE45_PORT_ADDR_MASK	0x000003e0
#define ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_COMMAND_CLAUSE45_PORT_ADDR_SHIFT	5
#define ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_COMMAND_CLAUSE45_POST_INC		AL_BIT(14)
#define ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_COMMAND_READ				AL_BIT(15)
/* mdio data */
#define ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_DATA_ADDR				0x00000038
/* mdio regaddr */
#define ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_REGADDR_ADDR				0x0000003C
/* mdio clause 22 (0) or 45 (1) */
#define AL_ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_C22_OR_C45				AL_BIT(6)

/*** CMAC MDIO ***/
#define ETH_MAC_GEN_V4_MAC_100G_MDIO_CFG_STATUS_HOLD_TIME_1_CLK	0
#define ETH_MAC_GEN_V4_MAC_100G_MDIO_CFG_STATUS_HOLD_TIME_2_CLK	1
#define ETH_MAC_GEN_V4_MAC_100G_MDIO_CFG_STATUS_HOLD_TIME_3_CLK	2
#define ETH_MAC_GEN_V4_MAC_100G_MDIO_CFG_STATUS_HOLD_TIME_4_CLK	3

#define ETH_MAC_GEN_V4_MAC_100G_MDIO_CFG_STATUS_CLK_DIV_MASK	0x0000ff00
#define ETH_MAC_GEN_V4_MAC_100G_MDIO_CFG_STATUS_CLK_DIV_SHIFT	8

/*** LMAC MDIO ***/
#define ETH_MAC_GEN_V4_MAC_50G_MDIO_CFG_STATUS_HOLD_TIME_1_CLK	0
#define ETH_MAC_GEN_V4_MAC_50G_MDIO_CFG_STATUS_HOLD_TIME_3_CLK	1
#define ETH_MAC_GEN_V4_MAC_50G_MDIO_CFG_STATUS_HOLD_TIME_5_CLK	2
#define ETH_MAC_GEN_V4_MAC_50G_MDIO_CFG_STATUS_HOLD_TIME_7_CLK	3
#define ETH_MAC_GEN_V4_MAC_50G_MDIO_CFG_STATUS_HOLD_TIME_9_CLK	4
#define ETH_MAC_GEN_V4_MAC_50G_MDIO_CFG_STATUS_HOLD_TIME_11_CLK	5
#define ETH_MAC_GEN_V4_MAC_50G_MDIO_CFG_STATUS_HOLD_TIME_13_CLK	6
#define ETH_MAC_GEN_V4_MAC_50G_MDIO_CFG_STATUS_HOLD_TIME_15_CLK	7

#define ETH_MAC_GEN_V4_MAC_50G_MDIO_CFG_STATUS_DIS_PREAMBLE	AL_BIT(5)

#define ETH_MAC_GEN_V4_MAC_50G_MDIO_CFG_STATUS_CLK_DIV_MASK	0x0000ff80
#define ETH_MAC_GEN_V4_MAC_50G_MDIO_CFG_STATUS_CLK_DIV_SHIFT	7

/* mdio constants */
#define AL_ETH_MDIO_DELAY_PERIOD	1 /* micro seconds to wait when polling mdio status */
#define AL_ETH_MDIO_DELAY_COUNT		150 /* number of times to poll */

#define ETH_MAC_GEN_V4_MAC_MODE_MASK	0x000000ff
#define ETH_MAC_GEN_V4_MAC_MODE_SHIFT	0

#define ETH_MAC_GEN_V4_FEC_ENABLE_MASK	0x00000f00
#define ETH_MAC_GEN_V4_FEC_ENABLE_SHIFT	8

/* RS-FEC Registers Addresses */
#define ETH_MAC_GEN_V4_PCS_RS_FEC_REG_CCW	4
#define ETH_MAC_GEN_V4_PCS_RS_FEC_REG_NCCW	8

enum mac_v4_mdio_cmd_type {
	MAC_V4_MDIO_CMD_TYPE_READ = 0,
	MAC_V4_MDIO_CMD_TYPE_WRITE = 1,
};

#define ETH_MAC_GEN_RATE_COUNTER_WIN_LEN_UNITS_NS	2
#define ETH_MAC_GEN_RATE_COUNTER_MAX_WIN		(ETH_MAC_GEN_RATE_COUNTER_WIN_LEN_UNITS_NS \
							* (~0ULL))

struct al_eth_mac_100g_stats_v4 {
	uint32_t reserved53[32];
	uint32_t frames_txed_ok;				/* 0x80 */
	uint32_t reserved54;
	uint32_t frames_rxed_ok;				/* 0x88 */
	uint32_t reserved55;
	uint32_t fcs_errors;					/* 0x90 */
	uint32_t reserved56;
	uint32_t alignment_errors;				/* 0x98 */
	uint32_t reserved57;
	uint32_t pause_mac_ctrl_frames_txed;			/* 0xA0 */
	uint32_t reserved58;
	uint32_t pause_mac_ctrl_frames_rxed;			/* 0xA8 */
	uint32_t reserved59;
	uint32_t frame_too_long_errs;				/* 0xB0 */
	uint32_t reserved60;
	uint32_t in_range_length_errs;				/* 0xB8 */
	uint32_t reserved61;
	uint32_t vlan_txed_ok;					/* 0xC0 */
	uint32_t reserved62;
	uint32_t vlan_rxed_ok;					/* 0xC8 */
	uint32_t reserved63;
	uint32_t if_out_octets_l;				/* 0xD0 */
	uint32_t if_out_octets_h;				/* 0xD4 */
	uint32_t if_in_octets_l;				/* 0xD8 */
	uint32_t if_in_octets_h;				/* 0xDC */
	uint32_t if_in_ucast_pkts;				/* 0xE0 */
	uint32_t reserved68;
	uint32_t if_in_mcast_pkts;				/* 0xE8 */
	uint32_t reserved69;
	uint32_t if_in_bcast_pkts;				/* 0xF0 */
	uint32_t reserved70;
	uint32_t if_out_errors;					/* 0xF8 */
	uint32_t reserved71[3];
	uint32_t if_out_ucast_pkts;				/* 0x108 */
	uint32_t reserved72;
	uint32_t if_out_mcast_pkts;				/* 0x110 */
	uint32_t reserved73;
	uint32_t if_out_bcast_pkts;				/* 0x118 */
	uint32_t reserved74;
	uint32_t drop_events;					/* 0x120 */
	uint32_t reserved75;
	uint32_t octets_l;					/* 0x128 */
	uint32_t octets_h;					/* 0x12C */
	uint32_t pkts;						/* 0x130 */
	uint32_t reserved78;
	uint32_t undersize_pkts;				/* 0x138 */
	uint32_t reserved79;
	uint32_t pkts_64_octets;				/* 0x140 */
	uint32_t reserved80;
	uint32_t pkts_65_to_127_octets;				/* 0x148 */
	uint32_t reserved81;
	uint32_t pkts_128_to_255_octets;			/* 0x150 */
	uint32_t reserved82;
	uint32_t pkts_256_to_511_octets;			/* 0x158 */
	uint32_t reserved83;
	uint32_t pkts_512_to_1023_octets;			/* 0x160 */
	uint32_t reserved84;
	uint32_t pkts_1024_to_1518_octets;			/* 0x168 */
	uint32_t reserved85;
	uint32_t pkts_1519_to_max_octets;			/* 0x170 */
	uint32_t reserved86;
	uint32_t oversize_pkts;					/* 0x178 */
	uint32_t reserved88;
	uint32_t jabbers;					/* 0x180 */
	uint32_t reserved89;
	uint32_t fragments;					/* 0x188 */
	uint32_t reserved90;
	uint32_t if_in_errors;					/* 0x190 */
	uint32_t reserved91;
	uint32_t cbfc_pause_frames_txed_0;			/* 0x198 */
	uint32_t reserved92;
	uint32_t cbfc_pause_frames_txed_1;			/* 0x1A0 */
	uint32_t reserved93;
	uint32_t cbfc_pause_frames_txed_2;			/* 0x1A8 */
	uint32_t reserved94;
	uint32_t cbfc_pause_frames_txed_3;			/* 0x1B0 */
	uint32_t reserved95;
	uint32_t cbfc_pause_frames_txed_4;			/* 0x1B8 */
	uint32_t reserved96;
	uint32_t cbfc_pause_frames_txed_5;			/* 0x1C0 */
	uint32_t reserved97;
	uint32_t cbfc_pause_frames_txed_6;			/* 0x1C8 */
	uint32_t reserved98;
	uint32_t cbfc_pause_frames_txed_7;			/* 0x1D0 */
	uint32_t reserved99;
	uint32_t cbfc_pause_frames_rxed_0;			/* 0x1D8 */
	uint32_t reserved100;
	uint32_t cbfc_pause_frames_rxed_1;			/* 0x1E0 */
	uint32_t reserved101;
	uint32_t cbfc_pause_frames_rxed_2;			/* 0x1E8 */
	uint32_t reserved102;
	uint32_t cbfc_pause_frames_rxed_3;			/* 0x1F0 */
	uint32_t reserved103;
	uint32_t cbfc_pause_frames_rxed_4;			/* 0x1F8 */
	uint32_t reserved104;
	uint32_t cbfc_pause_frames_rxed_5;			/* 0x200 */
	uint32_t reserved105;
	uint32_t cbfc_pause_frames_rxed_6;			/* 0x208 */
	uint32_t reserved106;
	uint32_t cbfc_pause_frames_rxed_7;			/* 0x210 */
	uint32_t reserved107;
	uint32_t mac_ctrl_frames_txed;				/* 0x218 */
	uint32_t reserved108;
	uint32_t mac_ctrl_frames_rxed;				/* 0x220 */
	uint32_t reserved109;
	uint32_t tx_octets;					/* 0x228 */
	uint32_t reserved110;
	uint32_t tx_pkts;					/* 0x230 */
	uint32_t reserved111;
	uint32_t tx_pkts_64_octets;				/* 0x238 */
	uint32_t reserved112;
	uint32_t tx_pkts_65_to_127_octets;			/* 0x240 */
	uint32_t reserved113;
	uint32_t tx_pkts_128_to_255_octets;			/* 0x248 */
	uint32_t reserved114;
	uint32_t tx_pkts_256_to_511_octets;			/* 0x250 */
	uint32_t reserved115;
	uint32_t tx_pkts_512_to_1023_octets;			/* 0x258 */
	uint32_t reserved116;
	uint32_t tx_pkts_1024_to_1518_octets;			/* 0x260 */
	uint32_t reserved117;
	uint32_t tx_pkts_1519_to_max_octets;			/* 0x268 */
};

struct al_eth_mac_50g_stats_v4 {
	uint32_t reserved1[64];
	/* 0x100 */
	struct al_eth_mac_10g_stats_v3_rx	rx;
	/* 0x200 */
	struct al_eth_mac_10g_stats_v3_tx	tx;
};

/**
 * read 50G MAC registers (indirect access)
 *
 * @param mac_regs_base MAC register base
 * @param reg_addr address in the an registers
 *
 * @return the register value
 */

static uint32_t al_eth_50g_mac_reg_read(
			struct al_eth_mac_v4_regs *mac_regs_base,
			uint32_t reg_addr)
{
	uint32_t val;

	/* indirect access */
	al_reg_write32(&mac_regs_base->mtip.mac_50g_addr, reg_addr);
	val = al_reg_read32(&mac_regs_base->mtip.mac_50g_data);

	al_dbg("%s: reg %d. val 0x%x\n", __func__, reg_addr, val);

	return val;
}

static uint64_t al_eth_50g_mac_reg_read64(
	struct al_eth_mac_v4_regs *mac_regs_base,
	uint32_t reg_addr)
{
	uint64_t val;

	/* indirect access */
	al_reg_write32(&mac_regs_base->mtip.mac_50g_addr, reg_addr);
	val = al_reg_read32(&mac_regs_base->mtip.mac_50g_data);

	al_reg_write32(&mac_regs_base->mtip.mac_50g_addr, reg_addr+4);
	val |= (uint64_t) (al_reg_read32(&mac_regs_base->mtip.mac_50g_data)) << 32;

	al_dbg("%s: reg %d. val 0x%"PRIx64"\n", __func__, reg_addr, val);

	return val;
}

/**
 * write 50G MAC registers (indirect access)
 *
 * @param mac_regs_base MAC register base
 * @param reg_addr address in the an registers
 * @param reg_data value to write to the register
 *
 */
static void al_eth_50g_mac_reg_write(
			struct al_eth_mac_v4_regs *mac_regs_base,
			uint32_t reg_addr,
			uint32_t reg_data)
{
	/* indirect access */
	al_reg_write32(&mac_regs_base->mtip.mac_50g_addr, reg_addr);
	al_reg_write32(&mac_regs_base->mtip.mac_50g_data, reg_data);

	al_dbg("%s: reg %d. val 0x%x\n", __func__, reg_addr, reg_data);
}

/**
 * read 100G MAC registers (indirect access)
 *
 * @param mac_regs_base MAC register base
 * @param reg_addr address in the an registers
 *
 * @return the register value
 */
static uint32_t al_eth_100g_mac_reg_read(
			struct al_eth_mac_v4_regs *mac_regs_base,
			uint32_t reg_addr)
{
	uint32_t val;

	/* indirect access */
	al_reg_write32(&mac_regs_base->mtip.mac_100g_addr, reg_addr);
	val = al_reg_read32(&mac_regs_base->mtip.mac_100g_data);

	al_dbg("%s: reg %d. val 0x%x\n", __func__, reg_addr, val);

	return val;
}

static uint64_t al_eth_100g_mac_reg_read64(
			struct al_eth_mac_v4_regs *mac_regs_base,
			uint32_t reg_addr)
{
	uint64_t val;

	/* indirect access */
	al_reg_write32(&mac_regs_base->mtip.mac_100g_addr, reg_addr);
	val = al_reg_read32(&mac_regs_base->mtip.mac_100g_data);

	al_reg_write32(&mac_regs_base->mtip.mac_100g_addr, reg_addr+4);
	val |= (uint64_t) (al_reg_read32(&mac_regs_base->mtip.mac_100g_data)) << 32;

	al_dbg("%s: reg %d. val 0x%"PRIx64"\n", __func__, reg_addr, val);

	return val;
}

/**
 * write 100G MAC registers (indirect access)
 *
 * @param mac_regs_base MAC register base
 * @param reg_addr address in the an registers
 * @param reg_data value to write to the register
 *
 */
static void al_eth_100g_mac_reg_write(
			struct al_eth_mac_v4_regs *mac_regs_base,
			uint32_t reg_addr,
			uint32_t reg_data)
{
	/* indirect access */
	al_reg_write32(&mac_regs_base->mtip.mac_100g_addr, reg_addr);
	al_reg_write32(&mac_regs_base->mtip.mac_100g_data, reg_data);

	al_dbg("%s: reg %d. val 0x%x\n", __func__, reg_addr, reg_data);
}

/**
 * read 100G MAC common registers (indirect access)
 *
 * @param mac_common_regs MAC common register base
 * @param reg_addr address in the an registers
 *
 * @return the register value
 */
static uint32_t al_eth_100g_mac_common_reg_read32(
			struct al_eth_mac_v4_common_regs *mac_common_regs,
			uint32_t reg_addr)
{
	uint32_t val;

	/* indirect access */
	al_reg_write32(&mac_common_regs->mtip.pcs_fec91_addr, reg_addr);
	val = al_reg_read32(&mac_common_regs->mtip.pcs_fec91_data);

	al_reg_write32(&mac_common_regs->mtip.pcs_fec91_addr, reg_addr + 2);
	val |= (al_reg_read32(&mac_common_regs->mtip.pcs_fec91_data) << 16);

	al_dbg("%s: reg %d. val 0x%x\n", __func__, reg_addr, val);

	return val;
}

/**
 * read 50G PCS registers (indirect access)
 *
 * @param mac_regs_base MAC register base
 * @param reg_addr address in the an registers
 *
 * @return the register value
 */
static uint32_t al_eth_50g_pcs_reg_read(
			struct al_eth_mac_v4_regs *mac_regs_base,
			uint32_t reg_addr)
{
	uint32_t val;

	/* indirect access */
	al_reg_write32(&mac_regs_base->mtip.pcs_50g_addr, reg_addr);
	val = al_reg_read32(&mac_regs_base->mtip.pcs_50g_data);

	al_dbg("%s: reg %d. val 0x%x\n", __func__, reg_addr, val);

	return val;
}

/**
 * write 50G PCS registers (indirect access)
 *
 * @param mac_regs_base MAC register base
 * @param reg_addr address in the an registers
 * @param reg_data value to write to the register
 *
 */
static void al_eth_50g_pcs_reg_write(
			struct al_eth_mac_v4_regs *mac_regs_base,
			uint32_t reg_addr,
			uint32_t reg_data)
{
	/* indirect access */
	al_reg_write32(&mac_regs_base->mtip.pcs_50g_addr, reg_addr);
	al_reg_write32(&mac_regs_base->mtip.pcs_50g_data, reg_data);

	al_dbg("%s: reg %d. val 0x%x\n", __func__, reg_addr, reg_data);
}

/**
 * read 100G PCS registers (indirect access)
 *
 * @param mac_regs_base MAC register base
 * @param reg_addr address in the an registers
 *
 * @return the register value
 */
static uint32_t al_eth_100g_pcs_reg_read(
			struct al_eth_mac_v4_regs *mac_regs_base,
			uint32_t reg_addr)
{
	uint32_t val;

	/* indirect access */
	al_reg_write32(&mac_regs_base->mtip.pcs_100g_addr, reg_addr);
	val = al_reg_read32(&mac_regs_base->mtip.pcs_100g_data);

	al_dbg("%s: reg %d. val 0x%x\n", __func__, reg_addr, val);

	return val;
}

/**
 * write 100G PCS registers (indirect access)
 *
 * @param mac_regs_base MAC register base
 * @param reg_addr address in the an registers
 * @param reg_data value to write to the register
 *
 */
static void al_eth_100g_pcs_reg_write(
			struct al_eth_mac_v4_regs *mac_regs_base,
			uint32_t reg_addr,
			uint32_t reg_data)
{
	/* indirect access */
	al_reg_write32(&mac_regs_base->mtip.pcs_100g_addr, reg_addr);
	al_reg_write32(&mac_regs_base->mtip.pcs_100g_data, reg_data);

	al_dbg("%s: reg %d. val 0x%x\n",  __func__, reg_addr, reg_data);
}

static int mac_mode_set(struct al_eth_mac_obj *obj, enum al_eth_mac_mode mode)
{
	struct al_eth_mac_v4_regs *mac_regs_base = obj->mac_regs_base;
	uint32_t reg = 0;

	al_assert(obj);

	obj->mac_mode = mode;

	AL_REG_FIELD_SET(reg,
		ETH_MAC_GEN_V4_MAC_MODE_MASK,
		ETH_MAC_GEN_V4_MAC_MODE_SHIFT,
		mode);

	al_reg_write32_masked(&mac_regs_base->gen.scratch0, ETH_MAC_GEN_V4_MAC_MODE_MASK, reg);

	return 0;
}

static int mac_mode_get(struct al_eth_mac_obj *obj, enum al_eth_mac_mode *mode)
{
	struct al_eth_mac_v4_regs *mac_regs_base = obj->mac_regs_base;
	uint32_t reg;

	al_assert(obj);

	reg = al_reg_read32(&mac_regs_base->gen.scratch0);

	*mode = ((reg & ETH_MAC_GEN_V4_MAC_MODE_MASK) >> ETH_MAC_GEN_V4_MAC_MODE_SHIFT);

	return 0;
}

static int mac_config(struct al_eth_mac_obj *obj, enum al_eth_mac_mode mode)
{
	struct al_eth_mac_v4_regs *mac_regs_base = obj->mac_regs_base;
	uint32_t reg = 0;

	/* Input Validation */
	if (((mode == AL_ETH_MAC_MODE_XLG_LL_40G) || (mode == AL_ETH_MAC_MODE_CG_100G)) &
		(obj->serdes_lane != 0)) {
		al_err("%s: MAC mode %s is invalid for channel %d\n",
			__func__, al_eth_mac_mode_str(mode),
			obj->serdes_lane);
		return -EINVAL;
	}

	reg = al_reg_read32(&mac_regs_base->gen.ch_num);
	if (reg != obj->serdes_lane) {
		al_err("%s: Unexpected channel id: expected %d, saw %d\n",
			 __func__, obj->serdes_lane, reg);
		return -EINVAL;
	}

	/* configure and enable the ASYNC FIFO between the MACs and the EC */
	/* TX min packet size */
	al_reg_write32(&mac_regs_base->gen_v4.tx_afifo_cfg_1,
		ETH_MAC_V4_GEN_V4_MIN_PKT_SIZE);

	/* TX max packet size */
	al_reg_write32(&mac_regs_base->gen_v4.tx_afifo_cfg_2,
		ETH_MAC_V4_GEN_V4_MAX_PKT_SIZE);

	/* TX input bus configuration */
	reg = 0;
	AL_REG_FIELD_SET(reg,
		ETH_MAC_V4_GEN_V4_TX_AFIFO_CFG_3_INPUT_BUS_W_MASK,
		ETH_MAC_V4_GEN_V4_TX_AFIFO_CFG_3_INPUT_BUS_W_SHIFT,
		ETH_MAC_V4_GEN_V4_BUS_W_256);
	AL_REG_FIELD_SET(reg,
		ETH_MAC_V4_GEN_V4_TX_AFIFO_CFG_3_INPUT_BUS_W_F_MASK,
		ETH_MAC_V4_GEN_V4_TX_AFIFO_CFG_3_INPUT_BUS_W_F_SHIFT,
		ETH_MAC_V4_GEN_V4_BUS_W_F_1);
	al_reg_write32(&mac_regs_base->gen_v4.tx_afifo_cfg_3, reg);

	/* TX output bus configuration */
	reg = 0;
	switch (mode) {
	case AL_ETH_MAC_MODE_10GbE_Serial:
	case AL_ETH_MAC_MODE_XLG_LL_25G:
	case AL_ETH_MAC_MODE_XLG_LL_40G:
	case AL_ETH_MAC_MODE_XLG_LL_50G:
		/* Check for LMAC availability */
		reg = al_reg_read32(&mac_regs_base->gen.avail);
		if (!(reg & ETH_MAC_V4_GEN_AVAIL_MAC_50G)) {
			al_err("%s: 50G MAC is not available\n", __func__);
			return -EINVAL;
		}

		/* TX Valid/ready configuration for LMAC*/
		/**
		 * Addressing RMN 10821
		 * RMN description:
		 *  Async FIFO RTL fix for masking the MTIP MAC non-zero padding
		 *  issue not functional for RX AFIFO.
		 * Software flow:
		 *	There is a HW fix for the MAC RTL, need to enable zero padding
		 *	on TX direction
		 */
		reg = ETH_MAC_V4_GEN_V4_TX_AFIFO_CFG_5_OUTPUT_BUS_SWAP_BYTES |
			ETH_MAC_V4_GEN_V4_TX_AFIFO_CFG_5_OUTPUT_BUS_VALID_RDY |
			ETH_MAC_V4_GEN_V4_TX_AFIFO_CFG_5_INPUT_BUS_VALID_RDY |
			ETH_MAC_V4_GEN_V4_TX_AFIFO_CFG_5_INPUT_BUS_ZERO_PAD_EN |
			ETH_MAC_V4_GEN_V4_TX_AFIFO_CFG_5_DATA_FIFO_SAF_EN;
		/**
		 * Addressing RMN 11074
		 * RMN description:
		 *  add configurable threshold for SAF in async_fifo_bus_adpt_top
		 * Software flow:
		 *	There is a HW fix for the MAC RTL, need to enable
		 *      store-and-forward (SAF) mode and configure threshold to 8.
		 */
		AL_REG_FIELD_SET(reg,
			ETH_MAC_V4_GEN_V4_TX_AFIFO_CFG_5_DATA_FIFO_SAF_THRESHOLD_MASK,
			ETH_MAC_V4_GEN_V4_TX_AFIFO_CFG_5_DATA_FIFO_SAF_THRESHOLD_SHIFT,
			ETH_MAC_V4_GEN_V4_SAF_THRESHOLD_8);
		al_reg_write32(&mac_regs_base->gen_v4.tx_afifo_cfg_5, reg);

		reg = 0;
		AL_REG_FIELD_SET(reg,
			ETH_MAC_V4_GEN_V4_TX_AFIFO_CFG_4_OUTPUT_BUS_W_MASK,
			ETH_MAC_V4_GEN_V4_TX_AFIFO_CFG_4_OUTPUT_BUS_W_SHIFT,
			ETH_MAC_V4_GEN_V4_BUS_W_64);
		AL_REG_FIELD_SET(reg,
			ETH_MAC_V4_GEN_V4_TX_AFIFO_CFG_4_OUTPUT_BUS_W_F_MASK,
			ETH_MAC_V4_GEN_V4_TX_AFIFO_CFG_4_OUTPUT_BUS_W_F_SHIFT,
			ETH_MAC_V4_GEN_V4_BUS_W_F_4);
		break;
	case AL_ETH_MAC_MODE_CG_100G:
		/* Check for CMAC availability */
		reg = al_reg_read32(&mac_regs_base->gen.avail);
		if (!(reg & ETH_MAC_V4_GEN_AVAIL_MAC_100G)) {
			al_err("%s: 100G MAC is not available\n", __func__);
			return -EINVAL;
		}

		/* TX Valid/ready configuration for CMAC */
		reg = ETH_MAC_V4_GEN_V4_TX_AFIFO_CFG_5_OUTPUT_BUS_SWAP_BYTES |
			ETH_MAC_V4_GEN_V4_TX_AFIFO_CFG_5_INPUT_BUS_VALID_RDY |
			ETH_MAC_V4_GEN_V4_TX_AFIFO_CFG_5_INPUT_BUS_ZERO_PAD_EN |
			ETH_MAC_V4_GEN_V4_TX_AFIFO_CFG_5_DATA_FIFO_SAF_EN;
		/**
		 * Addressing RMN 11074
		 * RMN description:
		 *  add configurable threshold for SAF in async_fifo_bus_adpt_top
		 * Software flow:
		 *	There is a HW fix for the MAC RTL, need to enable
		 *      store-and-forward (SAF) mode and configure threshold to 8.
		 */
		AL_REG_FIELD_SET(reg,
			ETH_MAC_V4_GEN_V4_TX_AFIFO_CFG_5_DATA_FIFO_SAF_THRESHOLD_MASK,
			ETH_MAC_V4_GEN_V4_TX_AFIFO_CFG_5_DATA_FIFO_SAF_THRESHOLD_SHIFT,
			ETH_MAC_V4_GEN_V4_SAF_THRESHOLD_8);
		al_reg_write32(&mac_regs_base->gen_v4.tx_afifo_cfg_5, reg);

		reg = 0;
		AL_REG_FIELD_SET(reg,
			ETH_MAC_V4_GEN_V4_TX_AFIFO_CFG_4_OUTPUT_BUS_W_MASK,
			ETH_MAC_V4_GEN_V4_TX_AFIFO_CFG_4_OUTPUT_BUS_W_SHIFT,
			ETH_MAC_V4_GEN_V4_BUS_W_256);
		AL_REG_FIELD_SET(reg,
			ETH_MAC_V4_GEN_V4_TX_AFIFO_CFG_4_OUTPUT_BUS_W_F_MASK,
			ETH_MAC_V4_GEN_V4_TX_AFIFO_CFG_4_OUTPUT_BUS_W_F_SHIFT,
			ETH_MAC_V4_GEN_V4_BUS_W_F_1);
		break;
	default:
		return -EPERM;
	}
	al_reg_write32(&mac_regs_base->gen_v4.tx_afifo_cfg_4, reg);

	/* RX min packet size */
	al_reg_write32(&mac_regs_base->gen_v4.rx_afifo_cfg_1,
		ETH_MAC_V4_GEN_V4_MIN_PKT_SIZE);

	/* RX max packet size */
	al_reg_write32(&mac_regs_base->gen_v4.rx_afifo_cfg_2,
		ETH_MAC_V4_GEN_V4_MAX_PKT_SIZE);

	/* RX input bus configuration */
	reg = 0;
	switch (mode) {
	case AL_ETH_MAC_MODE_CG_100G:
		AL_REG_FIELD_SET(reg,
			ETH_MAC_V4_GEN_V4_RX_AFIFO_CFG_3_INPUT_BUS_W_MASK,
			ETH_MAC_V4_GEN_V4_RX_AFIFO_CFG_3_INPUT_BUS_W_SHIFT,
			ETH_MAC_V4_GEN_V4_BUS_W_256);
		AL_REG_FIELD_SET(reg,
			ETH_MAC_V4_GEN_V4_RX_AFIFO_CFG_3_INPUT_BUS_W_F_MASK,
			ETH_MAC_V4_GEN_V4_RX_AFIFO_CFG_3_INPUT_BUS_W_F_SHIFT,
			ETH_MAC_V4_GEN_V4_BUS_W_F_1);
		break;
	default:
		AL_REG_FIELD_SET(reg,
			ETH_MAC_V4_GEN_V4_RX_AFIFO_CFG_3_INPUT_BUS_W_MASK,
			ETH_MAC_V4_GEN_V4_RX_AFIFO_CFG_3_INPUT_BUS_W_SHIFT,
			ETH_MAC_V4_GEN_V4_BUS_W_64);
		AL_REG_FIELD_SET(reg,
			ETH_MAC_V4_GEN_V4_RX_AFIFO_CFG_3_INPUT_BUS_W_F_MASK,
			ETH_MAC_V4_GEN_V4_RX_AFIFO_CFG_3_INPUT_BUS_W_F_SHIFT,
			ETH_MAC_V4_GEN_V4_BUS_W_F_4);
	}
	al_reg_write32(&mac_regs_base->gen_v4.rx_afifo_cfg_3, reg);

	/* RX output bus configuration */
	reg = 0;
	AL_REG_FIELD_SET(reg,
		ETH_MAC_V4_GEN_V4_RX_AFIFO_CFG_4_OUTPUT_BUS_W_MASK,
		ETH_MAC_V4_GEN_V4_RX_AFIFO_CFG_4_OUTPUT_BUS_W_SHIFT,
		ETH_MAC_V4_GEN_V4_BUS_W_256);
	AL_REG_FIELD_SET(reg,
		ETH_MAC_V4_GEN_V4_RX_AFIFO_CFG_4_OUTPUT_BUS_W_F_MASK,
		ETH_MAC_V4_GEN_V4_RX_AFIFO_CFG_4_OUTPUT_BUS_W_F_SHIFT,
		ETH_MAC_V4_GEN_V4_BUS_W_F_1);
	al_reg_write32(&mac_regs_base->gen_v4.rx_afifo_cfg_4, reg);

	/* RX Valid/ready configuration */
	reg = ETH_MAC_V4_GEN_V4_RX_AFIFO_CFG_5_INPUT_BUS_SWAP_BYTES |
		ETH_MAC_V4_GEN_V4_RX_AFIFO_CFG_5_OUTPUT_BUS_VALID_RDY |
		ETH_MAC_V4_GEN_V4_RX_AFIFO_CFG_5_INPUT_BUS_EOP_READY;

	AL_REG_FIELD_SET(reg,
		ETH_MAC_V4_GEN_V4_RX_AFIFO_CFG_5_INPUT_BUS_AFULL_ABSORB_VAL_MASK,
		ETH_MAC_V4_GEN_V4_RX_AFIFO_CFG_5_INPUT_BUS_AFULL_ABSORB_VAL_SHIFT,
		ETH_MAC_V4_GEN_V4_AFULL_ABSORB_ENTRIES_4);

	al_reg_write32(&mac_regs_base->gen_v4.rx_afifo_cfg_5, reg);

	/** Select LMAC/CMAC */
	switch (mode) {
	case AL_ETH_MAC_MODE_CG_100G:
		/* Select CMAC */
		al_reg_write32(&mac_regs_base->gen_v4.mac,
			ETH_MAC_V4_GEN_V4_MAC_EN_100G);
		break;
	default:
		/* Select LMAC */
		al_reg_write32(&mac_regs_base->gen_v4.mac,
			ETH_MAC_V4_GEN_V4_MAC_EN_50G);
	}

	/* ASYNC FIFO ENABLE */
	al_reg_write32(&mac_regs_base->gen_v4.afifo_ctrl,
		ETH_MAC_V4_GEN_V4_AFIFO_CTRL_EN_TX_IN |
		ETH_MAC_V4_GEN_V4_AFIFO_CTRL_EN_TX_OUT |
		ETH_MAC_V4_GEN_V4_AFIFO_CTRL_EN_RX_IN |
		ETH_MAC_V4_GEN_V4_AFIFO_CTRL_EN_RX_OUT |
		ETH_MAC_V4_GEN_V4_AFIFO_CTRL_EN_TX_FIFO_IN |
		ETH_MAC_V4_GEN_V4_AFIFO_CTRL_EN_TX_FIFO_OUT |
		ETH_MAC_V4_GEN_V4_AFIFO_CTRL_EN_RX_FIFO_IN |
		ETH_MAC_V4_GEN_V4_AFIFO_CTRL_EN_RX_FIFO_OUT);


	/* cmd_cfg: writes to MAC Core registers */
	switch (mode) {
	case AL_ETH_MAC_MODE_CG_100G:
		/**
		 * Read current config, as we don't want to change TX/RX enable
		 */
		reg = al_eth_100g_mac_reg_read(mac_regs_base,
			ETH_MAC_GEN_V4_MAC_100G_COMMAND_CONFIG_ADDR);

		reg |= ETH_MAC_GEN_V4_MAC_100G_COMMAND_CONFIG_PROMIS_EN |
			ETH_MAC_GEN_V4_MAC_100G_COMMAND_CONFIG_CNTL_FRM_ENA |
			ETH_MAC_GEN_V4_MAC_100G_COMMAND_CONFIG_TX_PAD_EN;

		al_eth_100g_mac_reg_write(mac_regs_base,
			ETH_MAC_GEN_V4_MAC_100G_COMMAND_CONFIG_ADDR, reg);
		break;
	default:
		/* Enable XGMII mode for 10G/25G */
		if ((mode == AL_ETH_MAC_MODE_10GbE_Serial) ||
			(mode == AL_ETH_MAC_MODE_XLG_LL_25G)) {
			al_eth_50g_mac_reg_write(mac_regs_base,
				ETH_MAC_GEN_V4_MAC_50G_XGMII_MODE_ADDR,
				ETH_MAC_GEN_V4_MAC_50G_XGMII_MODE_ENABLE);
		}

		/**
		 * Read current config, as we don't want to change TX/RX enable
		 */
		reg = al_eth_50g_mac_reg_read(mac_regs_base,
			ETH_MAC_GEN_V4_MAC_50G_COMMAND_CONFIG_ADDR);

		reg |= ETH_MAC_GEN_V4_MAC_50G_COMMAND_CONFIG_CNTL_FRM_ENA |
			ETH_MAC_GEN_V4_MAC_50G_COMMAND_CONFIG_TX_PAD_EN;
		al_eth_50g_mac_reg_write(mac_regs_base,
			ETH_MAC_GEN_V4_MAC_50G_COMMAND_CONFIG_ADDR, reg);
	}

	/* Complete remaining PCS channel programming */
	switch (mode) {
	case AL_ETH_MAC_MODE_CG_100G:

#if (defined(AL_ETH_FAST_AM_DONE)) && (AL_ETH_FAST_AM_DONE == 1)
		/* Shorten VL_INTVL in PCS to 1023 - for simulation only! */
		al_eth_100g_pcs_reg_write(mac_regs_base, ETH_MAC_GEN_V4_PCS_100G_VL_INTVL_ADDR,
			ETH_MAC_GEN_V4_PCS_100G_VL_INTVL_1023);
#endif
		/* Reset 100G PCS to make sure all settings take effect */
		reg = al_eth_100g_pcs_reg_read(mac_regs_base,
			ETH_MAC_GEN_V4_PCS_100G_CONTROL1_ADDR);
		al_eth_100g_pcs_reg_write(mac_regs_base,
			ETH_MAC_GEN_V4_PCS_100G_CONTROL1_ADDR,
			reg | ETH_MAC_GEN_V4_PCS_100G_CONTROL1_RESET);
		break;
	default:
		if ((mode == AL_ETH_MAC_MODE_XLG_LL_50G) || (mode == AL_ETH_MAC_MODE_XLG_LL_40G)) {
#if (defined(AL_ETH_FAST_AM_DONE)) && (AL_ETH_FAST_AM_DONE == 1)
			/* Shorten VL_INTVL in PCS to 1023 - for simulation only! */
			al_eth_50g_pcs_reg_write(mac_regs_base,
				ETH_MAC_GEN_V4_PCS_50G_VL_INTVL_ADDR,
				ETH_MAC_GEN_V4_PCS_50G_VL_INTVL_1023);
#endif
			/* Set PCS to 40GBASE-R via CONTROL2 register
			   Also covers enablement of MLD (See PCS Vendor PCS_MODE register) */
			al_eth_50g_pcs_reg_write(mac_regs_base,
				ETH_MAC_GEN_V4_PCS_50G_CONTROL2_ADDR,
				ETH_MAC_GEN_V4_PCS_50G_CONTROL2_40GBASER);

			/* Clear Vendor Register PCS_MODE to change MLD in CONTROL2 register */
			al_eth_50g_pcs_reg_write(mac_regs_base,
				ETH_MAC_GEN_V4_PCS_50G_PCS_MODE_ADDR,
				0);
		}

		/* Reset single 50G PCS to make sure all settings take effect */
		reg = al_eth_50g_pcs_reg_read(mac_regs_base,
			ETH_MAC_GEN_V4_PCS_50G_CONTROL1_ADDR);

		al_eth_50g_pcs_reg_write(mac_regs_base,
			ETH_MAC_GEN_V4_PCS_50G_CONTROL1_ADDR,
			reg | ETH_MAC_GEN_V4_PCS_50G_CONTROL1_RESET);

		break;
	}

	mac_mode_set(obj, mode);
	return 0;
}

static int mac_start_stop_adv(struct al_eth_mac_obj *obj,
			      al_bool tx,
			      al_bool rx,
			      al_bool tx_start,
			      al_bool rx_start)
{
	struct al_eth_mac_v4_regs *mac_regs_base = obj->mac_regs_base;
	uint32_t cmd_cfg = 0;
	uint32_t mask = 0;
	uint32_t data = 0;

	al_dbg("%s: mac_mode:%s, tx_modify:%d, tx_start:%d, rx_modify:%d, rx_start:%d\n",
	       __func__,
	       al_eth_mac_mode_str(obj->mac_mode),
	       tx,
	       tx_start,
	       rx,
	       rx_start);

	switch (obj->mac_mode) {
	case AL_ETH_MAC_MODE_10GbE_Serial:
	case AL_ETH_MAC_MODE_XLG_LL_25G:
	case AL_ETH_MAC_MODE_XLG_LL_40G:
	case AL_ETH_MAC_MODE_XLG_LL_50G:
		cmd_cfg = al_eth_50g_mac_reg_read(mac_regs_base,
						ETH_MAC_GEN_V4_MAC_50G_COMMAND_CONFIG_ADDR);

		if (tx) {
			mask |= ETH_MAC_GEN_V4_MAC_50G_COMMAND_CONFIG_TX_ENA;
			if (tx_start)
				data |= ETH_MAC_GEN_V4_MAC_50G_COMMAND_CONFIG_TX_ENA;
		}

		if (rx) {
			mask |= ETH_MAC_GEN_V4_MAC_50G_COMMAND_CONFIG_RX_ENA;
			if (rx_start)
				data |= ETH_MAC_GEN_V4_MAC_50G_COMMAND_CONFIG_RX_ENA;
		}

		cmd_cfg = AL_MASK_VAL(mask, data, cmd_cfg);

		al_eth_50g_mac_reg_write(mac_regs_base,
					ETH_MAC_GEN_V4_MAC_50G_COMMAND_CONFIG_ADDR, cmd_cfg);
		break;
	case AL_ETH_MAC_MODE_CG_100G:
		/* RMW! */
		cmd_cfg = al_eth_100g_mac_reg_read(mac_regs_base,
						ETH_MAC_GEN_V4_MAC_100G_COMMAND_CONFIG_ADDR);

		if (tx) {
			mask |= ETH_MAC_GEN_V4_MAC_100G_COMMAND_CONFIG_TX_ENA;
			if (tx_start)
				data |= ETH_MAC_GEN_V4_MAC_100G_COMMAND_CONFIG_TX_ENA;
		}

		if (rx) {
			mask |= ETH_MAC_GEN_V4_MAC_100G_COMMAND_CONFIG_RX_ENA;
			if (rx_start)
				data |= ETH_MAC_GEN_V4_MAC_100G_COMMAND_CONFIG_RX_ENA;
		}

		cmd_cfg = AL_MASK_VAL(mask, data, cmd_cfg);

		al_eth_100g_mac_reg_write(mac_regs_base,
					ETH_MAC_GEN_V4_MAC_100G_COMMAND_CONFIG_ADDR, cmd_cfg);
		break;
	default:
		al_err("%s@%s: Unsupported mac_mode: %s\n",
		       __func__, __FILE__,
		       al_eth_mac_mode_str(obj->mac_mode));
		return -EINVAL;
	}

	return 0;
}

static int mac_loopback_config(struct al_eth_mac_obj *obj, al_bool enable)
{
	struct al_eth_mac_v4_regs *mac_regs_base = obj->mac_regs_base;
	uint32_t reg = 0;

	switch (obj->mac_mode) {
	case AL_ETH_MAC_MODE_10GbE_Serial:
	case AL_ETH_MAC_MODE_XLG_LL_25G:
	case AL_ETH_MAC_MODE_XLG_LL_40G:
	case AL_ETH_MAC_MODE_XLG_LL_50G:
		reg = al_eth_50g_pcs_reg_read(mac_regs_base,
			ETH_MAC_GEN_V4_PCS_50G_CONTROL1_ADDR);
		if (enable)
			reg |= ETH_MAC_GEN_V4_PCS_50G_CONTROL1_LOOPBACK;
		else
			reg &= ~ETH_MAC_GEN_V4_PCS_50G_CONTROL1_LOOPBACK;
		al_eth_50g_pcs_reg_write(mac_regs_base,
			ETH_MAC_GEN_V4_PCS_50G_CONTROL1_ADDR, reg);
	break;

	case AL_ETH_MAC_MODE_CG_100G:
		reg = al_eth_100g_pcs_reg_read(mac_regs_base,
			ETH_MAC_GEN_V4_PCS_100G_CONTROL1_ADDR);
		if (enable)
			reg |= ETH_MAC_GEN_V4_PCS_100G_CONTROL1_LOOPBACK;
		else
			reg &= ~ETH_MAC_GEN_V4_PCS_100G_CONTROL1_LOOPBACK;
		al_eth_100g_pcs_reg_write(mac_regs_base,
			ETH_MAC_GEN_V4_PCS_100G_CONTROL1_ADDR, reg);
	break;
	default:
		return -EPERM;
	}
	return 0;
}

static int flow_control_config_v4(struct al_eth_mac_obj *obj,
	struct al_eth_flow_control_params *params)
{
	struct al_eth_mac_v4_regs *mac_regs_base = obj->mac_regs_base;
	uint32_t reg;

	/* Valid params pointer */
	al_assert(params);

	switch (params->type) {
	case AL_ETH_FLOW_CONTROL_TYPE_LINK_PAUSE:
		al_dbg("[%s]: config flow control to link pause mode.\n", obj->name);
		/* Config the MAC */
		switch (obj->mac_mode) {
		case AL_ETH_MAC_MODE_10GbE_Serial:
		case AL_ETH_MAC_MODE_XLG_LL_25G:
		case AL_ETH_MAC_MODE_XLG_LL_40G:
		case AL_ETH_MAC_MODE_XLG_LL_50G:
			/* Set quanta value */
			al_eth_50g_mac_reg_write(mac_regs_base,
				ETH_MAC_GEN_V4_MAC_50G_CL01_PAUSE_QUANTA_ADDR,
				params->quanta);
			/* Set quanta threshold value */
			al_eth_50g_mac_reg_write(mac_regs_base,
				ETH_MAC_GEN_V4_MAC_50G_CL01_QUANTA_THRESH_ADDR,
				params->quanta_th);
			break;
		case AL_ETH_MAC_MODE_CG_100G:
			/* set quanta value */
			al_eth_100g_mac_reg_write(mac_regs_base,
				ETH_MAC_GEN_V4_MAC_100G_CL01_PAUSE_QUANTA_ADDR,
				params->quanta);
			/* Set quanta threshold value */
			al_eth_100g_mac_reg_write(mac_regs_base,
				ETH_MAC_GEN_V4_MAC_100G_CL01_QUANTA_THRESH_ADDR,
				params->quanta_th);
			break;
		default:
			al_err("[%s]: unsupported flow control type %d (mac mode %d)\n",
				obj->name, params->type, obj->mac_mode);
			return -EINVAL;
			break;
		}
			break;
	case AL_ETH_FLOW_CONTROL_TYPE_PFC:
			al_dbg("[%s]: config flow control to PFC mode.\n", obj->name);
			switch (obj->mac_mode) {
			case AL_ETH_MAC_MODE_10GbE_Serial:
			case AL_ETH_MAC_MODE_XLG_LL_25G:
			case AL_ETH_MAC_MODE_XLG_LL_40G:
			case AL_ETH_MAC_MODE_XLG_LL_50G:
				/* Set quanta value (same value for all prios) */
				reg = params->quanta | (params->quanta << 16);
				al_eth_50g_mac_reg_write(mac_regs_base,
					ETH_MAC_GEN_V4_MAC_50G_CL01_PAUSE_QUANTA_ADDR, reg);
				al_eth_50g_mac_reg_write(mac_regs_base,
					ETH_MAC_GEN_V4_MAC_50G_CL23_PAUSE_QUANTA_ADDR, reg);
				al_eth_50g_mac_reg_write(mac_regs_base,
					ETH_MAC_GEN_V4_MAC_50G_CL45_PAUSE_QUANTA_ADDR, reg);
				al_eth_50g_mac_reg_write(mac_regs_base,
					ETH_MAC_GEN_V4_MAC_50G_CL67_PAUSE_QUANTA_ADDR, reg);

				/* Set quanta threshold value (same value for all prios) */
				reg = params->quanta_th | (params->quanta_th << 16);
				al_eth_50g_mac_reg_write(mac_regs_base,
					ETH_MAC_GEN_V4_MAC_50G_CL01_QUANTA_THRESH_ADDR, reg);
				al_eth_50g_mac_reg_write(mac_regs_base,
					ETH_MAC_GEN_V4_MAC_50G_CL23_QUANTA_THRESH_ADDR, reg);
				al_eth_50g_mac_reg_write(mac_regs_base,
					ETH_MAC_GEN_V4_MAC_50G_CL45_QUANTA_THRESH_ADDR, reg);
				al_eth_50g_mac_reg_write(mac_regs_base,
					ETH_MAC_GEN_V4_MAC_50G_CL67_QUANTA_THRESH_ADDR, reg);

				/* Enable PFC in the 50g_MAC */
				reg = al_eth_50g_mac_reg_read(mac_regs_base,
					ETH_MAC_GEN_V4_MAC_50G_COMMAND_CONFIG_ADDR);
				reg |= ETH_MAC_GEN_V4_MAC_50G_COMMAND_CONFIG_PFC_MODE;
				al_eth_50g_mac_reg_write(mac_regs_base,
					ETH_MAC_GEN_V4_MAC_50G_COMMAND_CONFIG_ADDR, reg);
				break;
			case AL_ETH_MAC_MODE_CG_100G:
				al_dbg("Configure flow control mac_mode : %d\n", obj->mac_mode);
				/* Set quanta value (same value for all prios) */
				reg = params->quanta | (params->quanta << 16);
				al_eth_100g_mac_reg_write(mac_regs_base,
					ETH_MAC_GEN_V4_MAC_100G_CL01_PAUSE_QUANTA_ADDR, reg);
				al_eth_100g_mac_reg_write(mac_regs_base,
					ETH_MAC_GEN_V4_MAC_100G_CL23_PAUSE_QUANTA_ADDR, reg);
				al_eth_100g_mac_reg_write(mac_regs_base,
					ETH_MAC_GEN_V4_MAC_100G_CL45_PAUSE_QUANTA_ADDR, reg);
				al_eth_100g_mac_reg_write(mac_regs_base,
					ETH_MAC_GEN_V4_MAC_100G_CL67_PAUSE_QUANTA_ADDR, reg);

				/* set quanta threshold value (same value for all prios) */
				reg = params->quanta_th | (params->quanta_th << 16);
				al_eth_100g_mac_reg_write(mac_regs_base,
					ETH_MAC_GEN_V4_MAC_100G_CL01_QUANTA_THRESH_ADDR, reg);
				al_eth_100g_mac_reg_write(mac_regs_base,
					ETH_MAC_GEN_V4_MAC_100G_CL23_QUANTA_THRESH_ADDR, reg);
				al_eth_100g_mac_reg_write(mac_regs_base,
					ETH_MAC_GEN_V4_MAC_100G_CL45_QUANTA_THRESH_ADDR, reg);
				al_eth_100g_mac_reg_write(mac_regs_base,
					ETH_MAC_GEN_V4_MAC_100G_CL67_QUANTA_THRESH_ADDR, reg);

				/* Enable PFC in the 100g_MAC */
				reg = al_eth_100g_mac_reg_read(mac_regs_base,
					ETH_MAC_GEN_V4_MAC_100G_COMMAND_CONFIG_ADDR);
				reg |= ETH_MAC_GEN_V4_MAC_100G_COMMAND_CONFIG_PFC_MODE;
				al_eth_100g_mac_reg_write(mac_regs_base,
					ETH_MAC_GEN_V4_MAC_100G_COMMAND_CONFIG_ADDR, reg);
				break;
			default:
				break;
			}
			break;

	default:
		al_err("[%s]: unsupported flow control type %d\n", obj->name, params->type);
		return -EINVAL;
	}
	return 0;
}

static int fec_enable_v4_mac(struct al_eth_mac_obj *obj, enum al_eth_fec_type fec_type)
{
	/** For Ethernet V4, fec enable has 2 stages - first one is calling fec_enable_common from
	 *  mac_common_init and then call the following function
	 */
	uint32_t reg;

	/** Get pointer to mac_v4_regs_base inside mac_obj */
	struct al_eth_mac_v4_regs *mac_regs_base = obj->mac_regs_base;

	if (fec_type == AL_ETH_FEC_TYPE_CLAUSE_91) {
		switch (obj->mac_mode) {
		case AL_ETH_MAC_MODE_XLG_LL_25G:
#if (defined(AL_ETH_FAST_AM_DONE)) && (AL_ETH_FAST_AM_DONE == 1)
			/* Shorten VL_INTVL in PCS to 1279 - for simulation only! */
			al_eth_50g_pcs_reg_write(mac_regs_base,
				ETH_MAC_GEN_V4_PCS_50G_VL_INTVL_ADDR,
				ETH_MAC_GEN_V4_PCS_50G_VL_INTVL_1279);
#endif
			/* In 25G mode, markers must be changes to match 50G for all lanes! */
			al_eth_50g_pcs_reg_write(mac_regs_base,
				ETH_MAC_GEN_V4_PCS_50G_VL0_0_ADDR,
				ETH_MAC_GEN_V4_PCS_50G_VL0_0_MARKER_50G);
			al_eth_50g_pcs_reg_write(mac_regs_base,
				ETH_MAC_GEN_V4_PCS_50G_VL0_1_ADDR,
				ETH_MAC_GEN_V4_PCS_50G_VL0_1_MARKER_50G);

			/** Note: DISABLE_MLD must be CLEARED for FEC91 over 25G */
			reg = al_eth_50g_pcs_reg_read(mac_regs_base,
				ETH_MAC_GEN_V4_PCS_50G_PCS_MODE_ADDR);

			reg &= ~(ETH_MAC_GEN_V4_PCS_50G_PCS_MODE_DISABLE_MLD);

			al_eth_50g_pcs_reg_write(mac_regs_base,
				ETH_MAC_GEN_V4_PCS_50G_PCS_MODE_ADDR,
				reg);

			/* Reset PCS for changes to immediately take effect */
			reg = al_eth_50g_pcs_reg_read(mac_regs_base,
				ETH_MAC_GEN_V4_PCS_50G_CONTROL1_ADDR);
			al_eth_50g_pcs_reg_write(mac_regs_base,
				ETH_MAC_GEN_V4_PCS_50G_CONTROL1_ADDR,
				reg | ETH_MAC_GEN_V4_PCS_50G_CONTROL1_RESET);
		break;
		case AL_ETH_MAC_MODE_XLG_LL_50G:
#if (defined(AL_ETH_FAST_AM_DONE)) && (AL_ETH_FAST_AM_DONE == 1)
			/* Shorten VL_INTVL in PCS to 1279 - for simulation only! */
			al_eth_50g_pcs_reg_write(mac_regs_base,
				ETH_MAC_GEN_V4_PCS_50G_VL_INTVL_ADDR,
				ETH_MAC_GEN_V4_PCS_50G_VL_INTVL_1279);

			/**
			 * Addressing RMN (Austin) 2729
			 *
			 * RMN description:
			 *	When shortening VL_INTVL for a 50G channel,
			 *	both the current lane *and* the adjacent lane must be modified.
			 *	MAC mode of 50G is using 2 lanes - pairs (0,1) & (2,3)
			 *
			 * Software flow:
			 *	Try setting VL_INTVL for adjacent channel!
			 *	watch the offset change with RTL changes.
			 *
			 */
			al_eth_50g_pcs_reg_write(
				(mac_regs_base + sizeof(struct al_eth_mac_v4_regs)),
				ETH_MAC_GEN_V4_PCS_50G_VL_INTVL_ADDR,
				ETH_MAC_GEN_V4_PCS_50G_VL_INTVL_1279);
#endif
		break;
		case AL_ETH_MAC_MODE_CG_100G:
#if (defined(AL_ETH_FAST_AM_DONE)) && (AL_ETH_FAST_AM_DONE == 1)
			/* Shorten VL_INTVL in PCS to 1023 - for simulation only! */
			al_eth_100g_pcs_reg_write(mac_regs_base,
				ETH_MAC_GEN_V4_PCS_100G_VL_INTVL_ADDR,
				ETH_MAC_GEN_V4_PCS_100G_VL_INTVL_1023);
#endif
			/* Reset PCS for changes to immediately take effect */
			reg = al_eth_100g_pcs_reg_read(mac_regs_base,
				ETH_MAC_GEN_V4_PCS_100G_CONTROL1_ADDR);
			al_eth_100g_pcs_reg_write(mac_regs_base,
				ETH_MAC_GEN_V4_PCS_100G_CONTROL1_ADDR,
				reg | ETH_MAC_GEN_V4_PCS_100G_CONTROL1_RESET);
		break;
		default:
			al_err("%s: FEC type %s (%d) is not supported on this MAC mode %s (%d)\n",
				__func__, al_eth_fec_type_str(fec_type), fec_type,
				al_eth_mac_mode_str(obj->mac_mode), obj->mac_mode);
			return -EPERM;
		}
	} else if (fec_type == AL_ETH_FEC_TYPE_CLAUSE_74) {
		/**
		 * This FEC is supported, but nothing to be done here.
		 * Configuration already done by FEC control function in ETH Common.
		 */
	} else {
		al_err("%s: FEC type %s (%d) is not supported\n",
			__func__, al_eth_fec_type_str(fec_type), fec_type);
		return -EPERM;
	}
	return 0;
}

static int fec_enable(struct al_eth_mac_obj *obj, enum al_eth_fec_type fec_type,
	al_bool enable)
{
	int ret;
	uint32_t reg = 0;

	struct al_eth_mac_v4_regs *mac_regs_base = obj->mac_regs_base;

	al_assert(obj->mac_common_regs);

	/** Check if valid MAC object before changing common registers */
	switch (obj->mac_mode) {
	case AL_ETH_MAC_MODE_XLG_LL_50G:
		if (obj->serdes_lane % 2 != 0) {
			al_err("%s: %s (%d) must be on even SerDes lanes, this lase is %d\n",
				__func__, al_eth_mac_mode_str(obj->mac_mode),
				obj->mac_mode, obj->serdes_lane);
			return -EPERM;
		}
		break;
	case AL_ETH_MAC_MODE_XLG_LL_40G:
		if (obj->serdes_lane != 0) {
			al_err("%s: %s (%d) must be on SerDes lane 0, this lase is %d\n",
				__func__, al_eth_mac_mode_str(obj->mac_mode),
				obj->mac_mode, obj->serdes_lane);
			return -EPERM;
		}
		break;
	case AL_ETH_MAC_MODE_CG_100G:
		if (obj->serdes_lane != 0) {
			al_err("%s: %s (%d) must be on SerDes lane 0, this lase is %d\n",
				__func__, al_eth_mac_mode_str(obj->mac_mode),
				obj->mac_mode, obj->serdes_lane);
			return -EPERM;
		}
		break;
	default:
		/** All other MAC modes are valid for each lane */
		break;
	}

	/** Lock MAC common registers if needed */
	if (obj->mac_common_regs_lock)
		if (obj->mac_common_regs_lock(obj->mac_common_regs_lock_handle, AL_TRUE)) {
			al_err("%s: Cannot lock MAC common registers\n", __func__);
			return -EPERM;
		}

	ret = al_eth_mac_v4_fec_ctrl(obj, obj->serdes_lane, fec_type, enable);

	/** Unlock MAC common registers if needed */
	if (obj->mac_common_regs_lock)
		if (obj->mac_common_regs_lock(obj->mac_common_regs_lock_handle, AL_FALSE)) {
			al_err("%s: Cannot unlock MAC common registers\n", __func__);
			return -EPERM;
		}

	/** If failed, don't perform 2nd phase */
	if (ret)
		return ret;

	/** If FEC enable then configure the MAC registers */
	if (enable)
		ret = fec_enable_v4_mac(obj, fec_type);

	/* Write fec_enable into a scratch register */
	AL_REG_FIELD_SET(reg,
		ETH_MAC_GEN_V4_FEC_ENABLE_MASK,
		ETH_MAC_GEN_V4_FEC_ENABLE_SHIFT,
		(enable) && (!ret));

	al_reg_write32_masked(&mac_regs_base->gen.scratch0, ETH_MAC_GEN_V4_FEC_ENABLE_MASK, reg);

	return ret;
}

static al_bool fec_is_enabled(struct al_eth_mac_obj *obj)
{
	struct al_eth_mac_v4_regs *mac_regs_base = obj->mac_regs_base;
	uint32_t reg;

	reg = al_reg_read32(&mac_regs_base->gen.scratch0);

	if (((reg & ETH_MAC_GEN_V4_FEC_ENABLE_MASK) >> ETH_MAC_GEN_V4_FEC_ENABLE_SHIFT))
		return AL_TRUE;
	else
		return AL_FALSE;
}

static int fec_stats_get(struct al_eth_mac_obj *obj, uint32_t *fec_ce,
			 uint32_t *fec_ue)
{
	struct al_eth_mac_v4_regs *mac_regs_base = obj->mac_regs_base;
	struct al_eth_mac_v4_common_regs *mac_common_regs_base = obj->mac_common_regs;

	switch (obj->mac_mode) {
	case AL_ETH_MAC_MODE_XLG_LL_25G:
		switch (obj->serdes_lane) {
		case 0:
			*fec_ce = al_reg_read32(&mac_regs_base->stat_pcs.fec_cerr_0);
			*fec_ue = al_reg_read32(&mac_regs_base->stat_pcs.fec_ncerr_0);
			break;
		case 1:
			*fec_ce = al_reg_read32(&mac_regs_base->stat_pcs.fec_cerr_2);
			*fec_ue = al_reg_read32(&mac_regs_base->stat_pcs.fec_ncerr_2);
			break;
		case 2:
			*fec_ce = al_reg_read32(&mac_regs_base->stat_pcs.fec_cerr_4);
			*fec_ue = al_reg_read32(&mac_regs_base->stat_pcs.fec_ncerr_4);
			break;
		case 3:
			*fec_ce = al_reg_read32(&mac_regs_base->stat_pcs.fec_cerr_6);
			*fec_ue = al_reg_read32(&mac_regs_base->stat_pcs.fec_ncerr_6);
			break;
		default:
			al_assert_msg(0, "Serdes lane %d not supported\n",
				      obj->serdes_lane);
			break;
		}
		break;
	case AL_ETH_MAC_MODE_CG_100G:
		*fec_ce = al_eth_100g_mac_common_reg_read32(mac_common_regs_base,
							    (uintptr_t)
							    ETH_MAC_GEN_V4_PCS_RS_FEC_REG_CCW);
		*fec_ue = al_eth_100g_mac_common_reg_read32(mac_common_regs_base,
							    (uintptr_t)
							    ETH_MAC_GEN_V4_PCS_RS_FEC_REG_NCCW);
		break;
	default:
		return -EPERM;
	}

	return 0;
}

static unsigned int link_status_link_flaps_get_and_clear(
	struct al_eth_mac_obj *obj)
{
	struct al_eth_mac_v4_regs *mac_regs_base = obj->mac_regs_base;
	unsigned int link_flaps;

	switch (obj->mac_mode) {
	case AL_ETH_MAC_MODE_CG_100G:
		link_flaps = al_reg_read32(&mac_regs_base->stat_pcs.link_status);
		break;
	case AL_ETH_MAC_MODE_10GbE_Serial:
	case AL_ETH_MAC_MODE_XLG_LL_25G:
	case AL_ETH_MAC_MODE_XLG_LL_40G:
	case AL_ETH_MAC_MODE_XLG_LL_50G:
		switch (obj->serdes_lane) {
		case 0:
			link_flaps = al_reg_read32(&mac_regs_base->stat_pcs.xl_link_status0);
			break;
		case 1:
			link_flaps = al_reg_read32(&mac_regs_base->stat_pcs.xl_link_status1);
			break;
		case 2:
			link_flaps = al_reg_read32(&mac_regs_base->stat_pcs.xl_link_status2);
			break;
		case 3:
			link_flaps = al_reg_read32(&mac_regs_base->stat_pcs.xl_link_status3);
			break;
		default:
			al_assert_msg(0, "Serdes lane %d not supported\n", obj->serdes_lane);
			return 0;
		}
		break;
	default:
		al_assert_msg(0, "MAC mode %d not supported\n", obj->mac_mode);
		return 0;
	}

	return link_flaps;
}

static int link_status_get(struct al_eth_mac_obj *obj,
	struct al_eth_link_status *status)
{
	struct al_eth_mac_v4_regs *mac_regs_base = obj->mac_regs_base;
	uint32_t reg;

	al_assert(obj && status);

	status->link_up = AL_FALSE;
	status->local_fault = AL_FALSE;
	status->remote_fault = AL_FALSE;

	switch (obj->mac_mode) {
	case AL_ETH_MAC_MODE_CG_100G:
		/* 1) Check PCS for block lock across all 20 virtual lanes (VLs) */
		/* 2) Check for align_lock = 1 (infers block lock) */
		/* 3) Check for link_status = 1 (infers align done) */
		reg = al_reg_read32(&mac_regs_base->gen_v4_sr.pcs_100g_pin_status);

		if (reg & ETH_MAC_V4_COMMON_GEN_V4_PCS_100G_PIN_STATUS_LINK_STATUS)
			status->link_up = AL_TRUE;


		/* 4) Check for faults on LMAC side */
		reg = al_reg_read32(&mac_regs_base->gen_v4.mac_100g_pin_status);

		if (reg & ETH_MAC_V4_GEN_V4_MAC_100G_PIN_STATUS_LOC_FAULT)
			status->local_fault = AL_TRUE;

		if (reg & ETH_MAC_V4_GEN_V4_MAC_100G_PIN_STATUS_REM_FAULT)
			status->remote_fault = AL_TRUE;
		break;
	case AL_ETH_MAC_MODE_10GbE_Serial:
	case AL_ETH_MAC_MODE_XLG_LL_25G:
	case AL_ETH_MAC_MODE_XLG_LL_40G:
	case AL_ETH_MAC_MODE_XLG_LL_50G:
		/* Check for link_status = 1 (infers align done) */
		reg = al_reg_read32(&mac_regs_base->gen_v4.pcs_50g_pin_status);

		if (reg & ETH_MAC_V4_GEN_V4_PCS_50G_PIN_STATUS_XL_LINK_STATUS)
			status->link_up = AL_TRUE;

		/* Then check for LOC/REM Fault on LMAC */
		reg = al_reg_read32(&mac_regs_base->gen_v4.mac_50g_pin_status);

		if (reg & ETH_MAC_V4_GEN_V4_MAC_50G_PIN_STATUS_LOC_FAULT)
			status->local_fault = AL_TRUE;

		if (reg & ETH_MAC_V4_GEN_V4_MAC_50G_PIN_STATUS_REM_FAULT)
			status->remote_fault = AL_TRUE;
		break;
	default:
		return -EINVAL;
	}

	if (link_status_link_flaps_get_and_clear(obj))
		status->link_up = AL_FALSE;

	if (status->local_fault || status->remote_fault)
		status->link_up = AL_FALSE;

	return 0;
}

static int link_status_clear(struct al_eth_mac_obj *obj)
{
	al_assert(obj);

	link_status_link_flaps_get_and_clear(obj);

	return 0;
}

static int rx_pkt_limit_config(struct al_eth_mac_obj *obj, unsigned int min_rx_len,
	unsigned int max_rx_len)
{
	struct al_eth_mac_v4_regs *mac_regs_base = obj->mac_regs_base;

	al_reg_write32(&mac_regs_base->gen_v4.rx_afifo_cfg_1, min_rx_len);
	al_reg_write32(&mac_regs_base->gen_v4.rx_afifo_cfg_2, max_rx_len);

	/* Configure the MAC's max rx length, add 16 bytes so the packet get
	 * trimmed by the EC/Async_fifo rather by the MAC
	 */
	switch (obj->mac_mode) {
	case AL_ETH_MAC_MODE_CG_100G:
		al_eth_100g_mac_reg_write(mac_regs_base,
			ETH_MAC_GEN_V4_MAC_100G_FRM_LENGTH_ADDR, max_rx_len + 16);
		break;
	case AL_ETH_MAC_MODE_10GbE_Serial:
	case AL_ETH_MAC_MODE_XLG_LL_25G:
	case AL_ETH_MAC_MODE_XLG_LL_40G:
	case AL_ETH_MAC_MODE_XLG_LL_50G:
		al_eth_50g_mac_reg_write(mac_regs_base,
			ETH_MAC_GEN_V4_MAC_100G_FRM_LENGTH_ADDR, max_rx_len + 16);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static int tx_flush_config(struct al_eth_mac_obj *obj, al_bool enable)
{
	struct al_eth_mac_v4_regs *mac_regs_base = obj->mac_regs_base;
	uint32_t reg;

	switch (obj->mac_mode) {
	case AL_ETH_MAC_MODE_CG_100G:
		reg = al_eth_100g_mac_reg_read(mac_regs_base,
			ETH_MAC_GEN_V4_MAC_100G_COMMAND_CONFIG_ADDR);

		if (enable)
			reg |= ETH_MAC_GEN_V4_MAC_100G_COMMAND_CONFIG_TX_FLUSH;
		else
			reg &= ~ETH_MAC_GEN_V4_MAC_100G_COMMAND_CONFIG_TX_FLUSH;

		al_eth_100g_mac_reg_write(mac_regs_base,
			ETH_MAC_GEN_V4_MAC_100G_COMMAND_CONFIG_ADDR, reg);
		break;
	case AL_ETH_MAC_MODE_10GbE_Serial:
	case AL_ETH_MAC_MODE_XLG_LL_25G:
	case AL_ETH_MAC_MODE_XLG_LL_40G:
	case AL_ETH_MAC_MODE_XLG_LL_50G:
		reg = al_eth_50g_mac_reg_read(mac_regs_base,
			ETH_MAC_GEN_V4_MAC_50G_COMMAND_CONFIG_ADDR);

		if (enable)
			reg |= ETH_MAC_GEN_V4_MAC_50G_COMMAND_CONFIG_TX_FLUSH;
		else
			reg &= ~ETH_MAC_GEN_V4_MAC_50G_COMMAND_CONFIG_TX_FLUSH;

		al_eth_50g_mac_reg_write(mac_regs_base,
			ETH_MAC_GEN_V4_MAC_50G_COMMAND_CONFIG_ADDR, reg);
		break;
	default:
		return -EPERM;
	}

	return 0;
}

/** Calculate OK rx/tx octets - octets_ok = octets - 18 * frames_ok- 4 * vlan_ok */
static inline void mac_stats_net_rx_tx_ok(struct al_eth_mac_v4_regs *mac_regs_base,
	struct al_eth_mac_stats *stats,
	uint64_t *rx_ok, uint64_t *tx_ok)
{
	struct al_eth_mac_50g_stats_v4 *stat_regs = NULL;
	uint64_t reg;

	reg = al_eth_50g_mac_reg_read(mac_regs_base,
		(uintptr_t) &stat_regs->rx.if_octets_l);
	reg |= ((uint64_t)(al_eth_50g_mac_reg_read(mac_regs_base,
		(uintptr_t) &stat_regs->rx.if_octets_h)) << 32);
	reg -= 18 * stats->frames_rxed_ok;
	reg -= 4 * stats->vlan_rxed_ok;
	*rx_ok = reg;

	reg = al_eth_50g_mac_reg_read(mac_regs_base,
		(uintptr_t) &stat_regs->tx.if_octets_l);
	reg |= ((uint64_t)(al_eth_50g_mac_reg_read(mac_regs_base,
		(uintptr_t) &stat_regs->tx.if_octets_h)) << 32);
	reg -= 18 * stats->frames_txed_ok;
	reg -= 4 * stats->vlan_txed_ok;
	*tx_ok = reg;
}

static inline void mac_stats_net_rx_tx_ok_100g(struct al_eth_mac_v4_regs *mac_regs_base,
	struct al_eth_mac_stats *stats,
	uint64_t *rx_ok, uint64_t *tx_ok)
{
	struct al_eth_mac_100g_stats_v4 *stat_regs_100G = NULL;
	uint64_t reg;

	reg = al_eth_100g_mac_reg_read64(mac_regs_base,
		(uintptr_t) &stat_regs_100G->if_in_octets_l);
	reg -= 18 * stats->frames_rxed_ok;
	reg -= 4 * stats->vlan_rxed_ok;
	*rx_ok = reg;

	reg = al_eth_100g_mac_reg_read64(mac_regs_base,
		(uintptr_t) &stat_regs_100G->if_out_octets_l);
	reg -= 18 * stats->frames_txed_ok;
	reg -= 4 * stats->vlan_txed_ok;
	*tx_ok = reg;
}

static int mac_stats_afifo_get(struct al_eth_mac_obj *obj, struct al_eth_mac_v4_stat *afifo_stats)
{
	struct al_eth_mac_v4_regs __iomem *mac_regs_base = obj->mac_regs_base;

	al_memset(afifo_stats, 0, sizeof(struct al_eth_mac_v4_stat));

	afifo_stats->tx_afifo_in_pkt =
		al_reg_read32(&mac_regs_base->stat.tx_afifo_in_pkt);
	afifo_stats->tx_afifo_in_short_pkt =
		al_reg_read32(&mac_regs_base->stat.tx_afifo_in_short_pkt);
	afifo_stats->tx_afifo_in_long_pkt =
		al_reg_read32(&mac_regs_base->stat.tx_afifo_in_long_pkt);
	afifo_stats->tx_afifo_out_pkt =
		al_reg_read32(&mac_regs_base->stat.tx_afifo_out_pkt);
	afifo_stats->tx_afifo_out_short_pkt =
		al_reg_read32(&mac_regs_base->stat.tx_afifo_out_short_pkt);
	afifo_stats->tx_afifo_out_long_pkt  =
		al_reg_read32(&mac_regs_base->stat.tx_afifo_out_long_pkt);
	afifo_stats->tx_afifo_out_drop_pkt  =
		al_reg_read32(&mac_regs_base->stat.tx_afifo_out_drop_pkt);
	afifo_stats->rx_afifo_in_pkt =
		al_reg_read32(&mac_regs_base->stat.rx_afifo_in_pkt);
	afifo_stats->rx_afifo_in_short_pkt =
		al_reg_read32(&mac_regs_base->stat.rx_afifo_in_short_pkt);
	afifo_stats->rx_afifo_in_long_pkt =
		al_reg_read32(&mac_regs_base->stat.rx_afifo_in_long_pkt);
	afifo_stats->rx_afifo_out_pkt =
		al_reg_read32(&mac_regs_base->stat.rx_afifo_out_pkt);
	afifo_stats->rx_afifo_out_short_pkt =
		al_reg_read32(&mac_regs_base->stat.rx_afifo_out_short_pkt);
	afifo_stats->rx_afifo_out_long_pkt =
		al_reg_read32(&mac_regs_base->stat.rx_afifo_out_long_pkt);
	afifo_stats->rx_afifo_out_drop_pkt =
		al_reg_read32(&mac_regs_base->stat.rx_afifo_out_drop_pkt);
	afifo_stats->tx_afifo_in_eop_err =
		al_reg_read32(&mac_regs_base->stat.tx_afifo_in_eop_err);
	afifo_stats->tx_afifo_in_sop_err =
		al_reg_read32(&mac_regs_base->stat.tx_afifo_in_sop_err);
	afifo_stats->tx_afifo_out_eop_err =
		al_reg_read32(&mac_regs_base->stat.tx_afifo_out_eop_err);
	afifo_stats->rx_afifo_in_eop_err =
		al_reg_read32(&mac_regs_base->stat.rx_afifo_in_eop_err);
	afifo_stats->rx_afifo_in_sop_err =
		al_reg_read32(&mac_regs_base->stat.rx_afifo_in_sop_err);
	afifo_stats->rx_afifo_out_eop_err =
		al_reg_read32(&mac_regs_base->stat.rx_afifo_out_eop_err);
	return 0;
}

static int mac_stats_get(struct al_eth_mac_obj *obj, struct al_eth_mac_stats *stats)
{
	struct al_eth_mac_v4_regs *mac_regs_base = obj->mac_regs_base;
	struct al_eth_mac_50g_stats_v4 *stat_regs = NULL;
	struct al_eth_mac_100g_stats_v4 *stat_regs_100G = NULL;

	al_assert(stats);

	al_memset(stats, 0, sizeof(struct al_eth_mac_stats));

	switch (obj->mac_mode) {
	case AL_ETH_MAC_MODE_10GbE_Serial:
	case AL_ETH_MAC_MODE_KR_LL_25G:
	case AL_ETH_MAC_MODE_XLG_LL_25G:
	case AL_ETH_MAC_MODE_XLG_LL_40G:
	case AL_ETH_MAC_MODE_XLG_LL_50G:
		stats->if_in_ucast_pkts =
			al_eth_50g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs->rx.if_in_ucast_pkts);
		stats->if_in_mcast_pkts =
			al_eth_50g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs->rx.if_in_mcast_pkts);
		stats->if_in_bcast_pkts =
			al_eth_50g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs->rx.if_in_bcast_pkts);
		stats->pkts =
			al_eth_50g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs->rx.pkts);
		stats->if_out_ucast_pkts =
			al_eth_50g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs->tx.if_ucast_pkts);
		stats->if_out_mcast_pkts =
			al_eth_50g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs->tx.if_multicast_pkts);
		stats->if_out_bcast_pkts =
			al_eth_50g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs->tx.if_broadcast_pkts);
		stats->if_in_errors =
			al_eth_50g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs->rx.if_in_errors);
		stats->if_out_errors =
			al_eth_50g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs->tx.if_out_errors);
		stats->frames_rxed_ok =
			al_eth_50g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs->rx.frames_ok);
		stats->frames_txed_ok =
			al_eth_50g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs->tx.frames_ok);
		stats->vlan_txed_ok =
			al_eth_50g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs->tx.vlan_ok);
		stats->vlan_rxed_ok =
			al_eth_50g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs->rx.vlan_ok);

		/** octets_rxed_ok = if_in_octets - 18 * frames_rxed_ok - 4 * vlan_rxed_ok */
		mac_stats_net_rx_tx_ok(mac_regs_base, stats,
			&stats->octets_rxed_ok, &stats->octets_txed_ok);

		stats->undersize_pkts =
			al_eth_50g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs->rx.undersize_pkts);
		stats->fragments =
			al_eth_50g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs->rx.fragments);
		stats->jabbers =
			al_eth_50g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs->rx.jabbers);
		stats->oversize_pkts =
			al_eth_50g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs->rx.oversize_pkts);
		stats->fcs_errors =
			al_eth_50g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs->rx.crc_errs);
		stats->alignment_errors =
			al_eth_50g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs->rx.alignment_errors);
		stats->drop_events =
			al_eth_50g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs->rx.drop_events);
		stats->pause_mac_ctrl_frames_txed =
			al_eth_50g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs->tx.pause_mac_ctrl_frames);
		stats->pause_mac_ctrl_frames_rxed =
			al_eth_50g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs->rx.pause_mac_ctrl_frames);
		stats->frame_too_long_errs =
			al_eth_50g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs->rx.aFrameTooLong);
		stats->in_range_length_errs =
			al_eth_50g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs->rx.in_range_length_errs);
		stats->octets =
			al_eth_50g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs->rx.octets);
		stats->pkts_64_octets =
			al_eth_50g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs->rx.pkts_64_octets);
		stats->pkts_65_to_127_octets =
			al_eth_50g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs->rx.pkts_65_to_127_octets);
		stats->pkts_128_to_255_octets =
			al_eth_50g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs->rx.pkts_128_to_255_octets);
		stats->pkts_256_to_511_octets =
			al_eth_50g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs->rx.pkts_256_to_511_octets);
		stats->pkts_512_to_1023_octets =
			al_eth_50g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs->rx.pkts_512_to_1023_octets);
		stats->pkts_1024_to_1518_octets =
			al_eth_50g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs->rx.pkts_1024_to_1518_octets);
		stats->pkts_1519_to_x =
			al_eth_50g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs->rx.pkts1519toMax);

		break;
	case AL_ETH_MAC_MODE_CG_100G:
		stats->if_in_ucast_pkts =
			al_eth_100g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs_100G->if_in_ucast_pkts);
		stats->if_in_mcast_pkts =
			al_eth_100g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs_100G->if_in_mcast_pkts);
		stats->if_in_bcast_pkts =
			al_eth_100g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs_100G->if_in_bcast_pkts);
		stats->pkts =
			al_eth_100g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs_100G->pkts);
		stats->if_out_ucast_pkts =
			al_eth_100g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs_100G->if_out_ucast_pkts);
		stats->if_out_mcast_pkts =
			al_eth_100g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs_100G->if_out_mcast_pkts);
		stats->if_out_bcast_pkts =
			al_eth_100g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs_100G->if_out_bcast_pkts);
		stats->if_in_errors =
			al_eth_100g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs_100G->if_in_errors);
		stats->if_out_errors =
			al_eth_100g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs_100G->if_out_errors);
		stats->frames_rxed_ok =
			al_eth_100g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs_100G->frames_rxed_ok);
		stats->frames_txed_ok =
			al_eth_100g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs_100G->frames_txed_ok);
		stats->vlan_txed_ok =
			al_eth_100g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs_100G->vlan_txed_ok);
		stats->vlan_rxed_ok =
			al_eth_100g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs_100G->vlan_rxed_ok);

		/**
		 * octets_ok = octets - 18 * frames_ok - 4 * vlan_ok
		 **/
		mac_stats_net_rx_tx_ok_100g(mac_regs_base, stats,
			&stats->octets_rxed_ok, &stats->octets_txed_ok);

		stats->undersize_pkts =
			al_eth_100g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs_100G->undersize_pkts);
		stats->fragments =
			al_eth_100g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs_100G->fragments);
		stats->jabbers =
			al_eth_100g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs_100G->jabbers);
		stats->oversize_pkts =
			al_eth_100g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs_100G->oversize_pkts);
		stats->fcs_errors =
			al_eth_100g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs_100G->fcs_errors);
		stats->alignment_errors =
			al_eth_100g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs_100G->alignment_errors);
		stats->drop_events =
			al_eth_100g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs_100G->drop_events);
		stats->pause_mac_ctrl_frames_txed =
			al_eth_100g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs_100G->pause_mac_ctrl_frames_txed);
		stats->pause_mac_ctrl_frames_rxed =
			al_eth_100g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs_100G->pause_mac_ctrl_frames_rxed);
		stats->frame_too_long_errs =
			al_eth_100g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs_100G->frame_too_long_errs);
		stats->in_range_length_errs =
			al_eth_100g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs_100G->in_range_length_errs);
		stats->octets =
			al_eth_100g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs_100G->octets_l);
		stats->pkts_64_octets =
			al_eth_100g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs_100G->pkts_64_octets);
		stats->pkts_65_to_127_octets =
			al_eth_100g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs_100G->pkts_65_to_127_octets);
		stats->pkts_128_to_255_octets =
			al_eth_100g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs_100G->pkts_128_to_255_octets);
		stats->pkts_256_to_511_octets =
			al_eth_100g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs_100G->pkts_256_to_511_octets);
		stats->pkts_512_to_1023_octets =
			al_eth_100g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs_100G->pkts_512_to_1023_octets);
		stats->pkts_1024_to_1518_octets =
			al_eth_100g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs_100G->pkts_1024_to_1518_octets);
		stats->pkts_1519_to_x =
			al_eth_100g_mac_reg_read64(mac_regs_base,
				(uintptr_t) &stat_regs_100G->pkts_1519_to_max_octets);

		break;
	default:
		return -EPERM;
	}

	/**
	 * Placeholder for eee statistics
	 */

	/** Fill the MAC V4 stats & set flag as valid */
	mac_stats_afifo_get(obj, &stats->mac_v4_stats);
	stats->mac_v4_stats_valid = AL_TRUE;

	return 0;
}

static int mdio_config(
	struct al_eth_mac_obj		*obj,
	enum al_eth_mdio_type		mdio_type,
	al_bool				shared_mdio_if,
	enum al_eth_ref_clk_freq	ref_clk_freq,
	unsigned int			mdio_clk_freq_khz,
	al_bool ignore_read_status)
{

	enum al_eth_mdio_if mdio_if = AL_ETH_MDIO_IF_10G_MAC;
	const char *if_name = (mdio_if == AL_ETH_MDIO_IF_1G_MAC) ? "10/100/1G MAC" : "10G MAC";
	const char *type_name = al_eth_mdio_type_str(mdio_type);
	const char *shared_name = (shared_mdio_if == AL_TRUE) ? "Yes" : "No";
	struct al_eth_mac_v4_regs *mac_regs_base = obj->mac_regs_base;

	int ref_clk_freq_khz;
	uint32_t val;
	uint32_t c22_or_c45 = 0;

	al_dbg("eth [%s]: mdio config: interface %s. type %s. shared: %s\n", obj->name, if_name,
		type_name, shared_name);
	obj->shared_mdio_if = shared_mdio_if;
	obj->mdio_if = mdio_if;
	obj->ignore_read_status = ignore_read_status;

	/* set clock div to get 'mdio_clk_freq_khz' */
	ref_clk_freq_khz = al_eth_ref_clk_freq_val(ref_clk_freq);
	if (ref_clk_freq_khz < 0) {
		al_err("eth [%s]: %s: invalid reference clock frequency (%d)\n",
			obj->name, __func__, ref_clk_freq);
		return -EPERM;
	}

	/** MDIO C22 or C45 */
	if (mdio_type == AL_ETH_MDIO_TYPE_CLAUSE_22)
		c22_or_c45 = 0;
	else if (mdio_type == AL_ETH_MDIO_TYPE_CLAUSE_45)
		c22_or_c45 |= AL_ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_C22_OR_C45;
	else
		return -EINVAL;

	if (mdio_if == AL_ETH_MDIO_IF_10G_MAC) {
		switch (obj->mac_mode) {
		case AL_ETH_MAC_MODE_10GbE_Serial:
		case AL_ETH_MAC_MODE_XLG_LL_25G:
		case AL_ETH_MAC_MODE_XLG_LL_40G:
		case AL_ETH_MAC_MODE_XLG_LL_50G:
			/* Configure to use LMAC MDIO! */
			val = al_reg_read32(&mac_regs_base->gen.cfg);
			val &= ~ETH_MAC_V4_GEN_CFG_MDIO_100_50B_SEL;
			al_reg_write32(&mac_regs_base->gen.cfg, val);

			val = al_eth_50g_mac_reg_read(mac_regs_base,
				ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_CFG_STATUS_ADDR);

			val &= ~AL_ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_C22_OR_C45;
			val |= c22_or_c45;

			/* CLK_DIV: Encoding per formula in MTIP LMAC spec */
			AL_REG_FIELD_SET(val,
				ETH_MAC_GEN_V4_MAC_50G_MDIO_CFG_STATUS_CLK_DIV_MASK,
				ETH_MAC_GEN_V4_MAC_50G_MDIO_CFG_STATUS_CLK_DIV_SHIFT,
				(ref_clk_freq_khz / (2 * mdio_clk_freq_khz)));

			AL_REG_FIELD_SET(val,
				ETH_MAC_GEN_V4_MAC_50G_MDIO_CFG_STATUS_HOLD_TIME_MASK,
				ETH_MAC_GEN_V4_MAC_50G_MDIO_CFG_STATUS_HOLD_TIME_SHIFT,
				ETH_MAC_GEN_V4_MAC_50G_MDIO_CFG_STATUS_HOLD_TIME_7_CLK);
			al_eth_50g_mac_reg_write(mac_regs_base,
				ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_CFG_STATUS_ADDR, val);
			break;

		case AL_ETH_MAC_MODE_CG_100G:
			/* Configure to use CMAC MDIO! */
			val = al_reg_read32(&mac_regs_base->gen.cfg);
			val |= ETH_MAC_V4_GEN_CFG_MDIO_100_50B_SEL;
			al_reg_write32(&mac_regs_base->gen.cfg, val);

			val = al_eth_100g_mac_reg_read(mac_regs_base,
				ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_CFG_STATUS_ADDR);

			val &= ~AL_ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_C22_OR_C45;
			val |= c22_or_c45;

			/* CLK_DIV: Encoding per formula in MTIP CMAC spec */
			AL_REG_FIELD_SET(val,
				ETH_MAC_GEN_V4_MAC_100G_MDIO_CFG_STATUS_CLK_DIV_MASK,
				ETH_MAC_GEN_V4_MAC_100G_MDIO_CFG_STATUS_CLK_DIV_SHIFT,
				(ref_clk_freq_khz / mdio_clk_freq_khz));

			AL_REG_FIELD_SET(val,
				ETH_MAC_GEN_V4_MAC_100G_MDIO_CFG_STATUS_HOLD_TIME_MASK,
				ETH_MAC_GEN_V4_MAC_100G_MDIO_CFG_STATUS_HOLD_TIME_SHIFT,
				ETH_MAC_GEN_V4_MAC_100G_MDIO_CFG_STATUS_HOLD_TIME_4_CLK);
			al_eth_100g_mac_reg_write(mac_regs_base,
				ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_CFG_STATUS_ADDR, val);
			break;
		default:
			return -EPERM;
		}
	} else {
		if (mdio_type != AL_ETH_MDIO_TYPE_CLAUSE_22) {
			al_err("eth [%s] mdio type not supported for this interface\n",
				 obj->name);
			return -EINVAL;
		}
	}
	obj->mdio_type = mdio_type;

	return 0;
}

static int mdio_pol_config(
	struct al_eth_mac_obj		*obj,
	enum al_eth_mdc_mdio_pol	mdio_pol)
{

	struct al_eth_mac_v4_regs *mac_regs_base = obj->mac_regs_base;
	uint32_t val;

	val = al_reg_read32(&mac_regs_base->gen.cfg);

	if (mdio_pol == AL_ETH_MDC_MDIO_POL_FLIPPED)
		val |= ETH_MAC_V4_GEN_CFG_MDIO_POL;
	else
		val &= ~(ETH_MAC_V4_GEN_CFG_MDIO_POL);

	al_reg_write32(&mac_regs_base->gen.cfg, val);

	return 0;
}

static int mdio_lock(struct al_eth_mac_obj *obj)
{
	int	count = 0;
	uint32_t mdio_ctrl_1;
	struct al_eth_mac_v4_regs *mac_regs_base = obj->mac_regs_base;

	if (obj->shared_mdio_if == AL_FALSE)
		return 0; /* nothing to do when interface is not shared */

	do {
		mdio_ctrl_1 = al_reg_read32(&mac_regs_base->gen.mdio_ctrl_1);

		if (mdio_ctrl_1 & ETH_MAC_V4_GEN_MDIO_CTRL_1_AVAIL) {
			if (count > 0)
				al_dbg("eth %s mdio interface still busy!\n", obj->name);
		} else {
			return 0;
		}
		al_udelay(AL_ETH_MDIO_DELAY_PERIOD);
	} while (count++ < (AL_ETH_MDIO_DELAY_COUNT * 4));
	al_err(" %s mdio failed to take ownership. MDIO info reg: 0x%08x\n",
		obj->name, al_reg_read32(&mac_regs_base->gen.mdio_1));

	return -ETIMEDOUT;
}

static int mdio_free(struct al_eth_mac_obj *obj)
{
	struct al_eth_mac_v4_regs *mac_regs_base = obj->mac_regs_base;

	if (obj->shared_mdio_if == AL_FALSE)
		return 0; /* nothing to do when interface is not shared */

	al_reg_write32(&mac_regs_base->gen.mdio_ctrl_1, 0);

	/*
	 * Addressing RMN: 2917
	 *
	 * RMN description:
	 * The HW spin-lock is stateless and doesn't maintain any scheduling
	 * policy.
	 *
	 * Software flow:
	 * After getting the lock wait 2 times the delay period in order to give
	 * the other port chance to take the lock and prevent starvation.
	 * This is not scalable to more than two ports.
	 */
	al_udelay(2 * AL_ETH_MDIO_DELAY_PERIOD);

	return 0;
}

static int mdio_10g_mac_wait_busy(struct al_eth_mac_obj *obj)
{
	int	count = 0;
	uint32_t mdio_cfg_status;
	struct al_eth_mac_v4_regs *mac_regs_base = obj->mac_regs_base;

	switch (obj->mac_mode) {
	case AL_ETH_MAC_MODE_10GbE_Serial:
	case AL_ETH_MAC_MODE_XLG_LL_25G:
	case AL_ETH_MAC_MODE_XLG_LL_40G:
	case AL_ETH_MAC_MODE_XLG_LL_50G:
		for (count = 0; count < AL_ETH_MDIO_DELAY_COUNT; count++) {
			mdio_cfg_status = al_eth_50g_mac_reg_read(mac_regs_base,
				ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_CFG_STATUS_ADDR);

			if (mdio_cfg_status & ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_CFG_STATUS_BUSY) {
				if (count > 0)
					al_dbg("eth [%s] mdio: still busy!\n", obj->name);
			} else {
				return 0;
			}

			al_udelay(AL_ETH_MDIO_DELAY_PERIOD);
		}

	break;
	case AL_ETH_MAC_MODE_CG_100G:
		for (count = 0; count < AL_ETH_MDIO_DELAY_COUNT; count++) {
			mdio_cfg_status = al_eth_100g_mac_reg_read(mac_regs_base,
				ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_CFG_STATUS_ADDR);

			if (mdio_cfg_status & ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_CFG_STATUS_BUSY) {
				if (count > 0)
					al_dbg("eth [%s] mdio: still busy!\n", obj->name);
			} else {
				return 0;
			}

			al_udelay(AL_ETH_MDIO_DELAY_PERIOD);
		}

	break;
	default:
		return -EPERM;
	}
	return -ETIMEDOUT;
}

static int mdio_10g_mac_type22(struct al_eth_mac_obj *obj,
	enum mac_v4_mdio_cmd_type cmd_type, uint32_t phy_addr, uint32_t reg, uint16_t *val)
{
	int rc;
	const char *op = (cmd_type == MAC_V4_MDIO_CMD_TYPE_READ) ? "read" : "write";
	uint32_t mdio_cfg_status;
	uint16_t mdio_cmd = 0;
	struct al_eth_mac_v4_regs *mac_regs_base = obj->mac_regs_base;

	switch (obj->mac_mode) {
	case AL_ETH_MAC_MODE_10GbE_Serial:
	case AL_ETH_MAC_MODE_XLG_LL_25G:
	case AL_ETH_MAC_MODE_XLG_LL_40G:
	case AL_ETH_MAC_MODE_XLG_LL_50G:
		/* wait if the HW is busy */
		rc = mdio_10g_mac_wait_busy(obj);
		if (rc) {
			al_err(" %s mdio %s failed. HW is busy\n", obj->name, op);
			return rc;
		}
		/* set command register */
		AL_REG_FIELD_SET(mdio_cmd,
			ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_COMMAND_CLAUSE22_REG_ADDR_MASK,
			ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_COMMAND_CLAUSE22_REG_ADDR_SHIFT,
			reg);

		AL_REG_FIELD_SET(mdio_cmd,
			ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_COMMAND_CLAUSE22_PHY_ADDR_MASK,
			ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_COMMAND_CLAUSE22_PHY_ADDR_SHIFT,
			phy_addr);

		/** READ command */
		if (cmd_type == MAC_V4_MDIO_CMD_TYPE_READ)
			mdio_cmd |= ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_COMMAND_READ;

		al_eth_50g_mac_reg_write(mac_regs_base,
			ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_COMMAND_ADDR, mdio_cmd);

		if (cmd_type == MAC_V4_MDIO_CMD_TYPE_WRITE)
			al_eth_50g_mac_reg_write(mac_regs_base,
				ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_DATA_ADDR, *val);

		/* wait for busy bit to clear */
		rc = mdio_10g_mac_wait_busy(obj);
		if (rc) {
			al_err(" %s mdio %s failed on timeout\n", obj->name, op);
			return rc;
		}

		mdio_cfg_status = al_eth_50g_mac_reg_read(mac_regs_base,
			ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_CFG_STATUS_ADDR);

		if (mdio_cfg_status & ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_CFG_STATUS_READ_ERR) {
			/* MDIO access reported error */
			al_err(" %s mdio %s failed on error. phy_addr 0x%x reg 0x%x\n",
				obj->name, op, phy_addr, reg);
				return -EIO;
		}
		if (cmd_type == MAC_V4_MDIO_CMD_TYPE_READ)
			*val = al_eth_50g_mac_reg_read(mac_regs_base,
				ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_DATA_ADDR);
		return 0;
	break;
	case AL_ETH_MAC_MODE_CG_100G:
		/* wait if the HW is busy */
		rc = mdio_10g_mac_wait_busy(obj);
		if (rc) {
			al_err(" %s mdio %s failed. HW is busy\n", obj->name, op);
			return rc;
		}
		/* set command register */
		AL_REG_FIELD_SET(mdio_cmd,
			ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_COMMAND_CLAUSE22_REG_ADDR_MASK,
			ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_COMMAND_CLAUSE22_REG_ADDR_SHIFT,
			reg);

		AL_REG_FIELD_SET(mdio_cmd,
			ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_COMMAND_CLAUSE22_PHY_ADDR_MASK,
			ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_COMMAND_CLAUSE22_PHY_ADDR_SHIFT,
			phy_addr);

		/** READ command */
		if (cmd_type == MAC_V4_MDIO_CMD_TYPE_READ)
			mdio_cmd |= ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_COMMAND_READ;

		al_eth_100g_mac_reg_write(mac_regs_base,
			ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_COMMAND_ADDR, mdio_cmd);

		if (cmd_type == MAC_V4_MDIO_CMD_TYPE_WRITE)
			al_eth_100g_mac_reg_write(mac_regs_base,
				ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_DATA_ADDR, *val);

		/* wait for busy bit to clear */
		rc = mdio_10g_mac_wait_busy(obj);
		if (rc) {
			al_err(" %s mdio %s failed on timeout\n", obj->name, op);
			return rc;
		}

		mdio_cfg_status = al_eth_100g_mac_reg_read(mac_regs_base,
			ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_CFG_STATUS_ADDR);

		if (mdio_cfg_status & ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_CFG_STATUS_READ_ERR) {
			/* MDIO access reported error */
			al_err(" %s mdio %s failed on error. phy_addr 0x%x reg 0x%x\n",
				obj->name, op, phy_addr, reg);
				return -EIO;
		}
		if (cmd_type == MAC_V4_MDIO_CMD_TYPE_READ)
			*val = al_eth_100g_mac_reg_read(mac_regs_base,
				ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_DATA_ADDR);
		return 0;

	break;
	default:
		return -EINVAL;
	}

}

static int mdio_10g_mac_type45(struct al_eth_mac_obj *obj,
	enum mac_v4_mdio_cmd_type cmd_type, uint32_t port_addr, uint32_t device, uint32_t reg,
	uint16_t *val)
{
	int rc;
	const char *op = (cmd_type == MAC_V4_MDIO_CMD_TYPE_READ) ? "read" : "write";
	uint32_t mdio_cfg_status;
	uint16_t mdio_cmd = 0;
	struct al_eth_mac_v4_regs *mac_regs_base = obj->mac_regs_base;

	switch (obj->mac_mode) {
	case AL_ETH_MAC_MODE_10GbE_Serial:
	case AL_ETH_MAC_MODE_XLG_LL_25G:
	case AL_ETH_MAC_MODE_XLG_LL_40G:
	case AL_ETH_MAC_MODE_XLG_LL_50G:
		/* wait if the HW is busy */
		rc = mdio_10g_mac_wait_busy(obj);
		if (rc) {
			al_err(" %s mdio %s failed. HW is busy\n", obj->name, op);
			return rc;
		}
		/* set command register */
		AL_REG_FIELD_SET(mdio_cmd,
			ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_COMMAND_CLAUSE45_DEV_ADDR_MASK,
			ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_COMMAND_CLAUSE45_DEV_ADDR_SHIFT,
			device);

		AL_REG_FIELD_SET(mdio_cmd,
			ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_COMMAND_CLAUSE45_PORT_ADDR_MASK,
			ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_COMMAND_CLAUSE45_PORT_ADDR_SHIFT,
			port_addr);
		al_eth_50g_mac_reg_write(mac_regs_base,
			ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_COMMAND_ADDR, mdio_cmd);

		/* send address frame */
		al_eth_50g_mac_reg_write(mac_regs_base,
			ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_REGADDR_ADDR, reg);

		/* wait for busy bit to clear */
		rc = mdio_10g_mac_wait_busy(obj);
		if (rc) {
			al_err(" %s mdio %s (address frame) failed on timeout\n", obj->name, op);
			return rc;
		}

		/* if read, write again to the command register with READ bit set */
		if (cmd_type == MAC_V4_MDIO_CMD_TYPE_READ) {
			mdio_cmd |= ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_COMMAND_READ;
			al_eth_50g_mac_reg_write(mac_regs_base,
				ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_COMMAND_ADDR, mdio_cmd);
		} else {
			al_eth_50g_mac_reg_write(mac_regs_base,
				ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_DATA_ADDR, *val);
		}
		/* wait for busy bit to clear */
		rc = mdio_10g_mac_wait_busy(obj);
		if (rc) {
			al_err(" %s mdio %s failed on timeout\n", obj->name, op);
			return rc;
		}

		mdio_cfg_status = al_eth_50g_mac_reg_read(mac_regs_base,
			ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_CFG_STATUS_ADDR);

		if (mdio_cfg_status & ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_CFG_STATUS_READ_ERR) {
			al_err(" %s mdio %s failed on error. port 0x%x, device 0x%x reg 0x%x\n",
				obj->name, op, port_addr, device, reg);
				return -EIO;
		}
		if (cmd_type == MAC_V4_MDIO_CMD_TYPE_READ)
			*val = al_eth_50g_mac_reg_read(mac_regs_base,
				ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_DATA_ADDR);
		return 0;
	break;
	case AL_ETH_MAC_MODE_CG_100G:
		/* wait if the HW is busy */
		rc = mdio_10g_mac_wait_busy(obj);
		if (rc) {
			al_err(" %s mdio %s failed. HW is busy\n", obj->name, op);
			return rc;
		}
		/* set command register */
		AL_REG_FIELD_SET(mdio_cmd,
			ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_COMMAND_CLAUSE45_DEV_ADDR_MASK,
			ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_COMMAND_CLAUSE45_DEV_ADDR_SHIFT,
			device);

		AL_REG_FIELD_SET(mdio_cmd,
			ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_COMMAND_CLAUSE45_PORT_ADDR_MASK,
			ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_COMMAND_CLAUSE45_PORT_ADDR_SHIFT,
			port_addr);

		al_eth_100g_mac_reg_write(mac_regs_base,
			ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_COMMAND_ADDR, mdio_cmd);

		/* send address frame */
		al_eth_100g_mac_reg_write(mac_regs_base,
			ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_REGADDR_ADDR, reg);

		/* wait for busy bit to clear */
		rc = mdio_10g_mac_wait_busy(obj);
		if (rc) {
			al_err(" %s mdio %s (address frame) failed on timeout\n", obj->name, op);
			return rc;
		}

		/* if read, write again to the command register with READ bit set */
		if (cmd_type == MAC_V4_MDIO_CMD_TYPE_READ) {
			mdio_cmd |= ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_COMMAND_READ;
			al_eth_100g_mac_reg_write(mac_regs_base,
				ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_COMMAND_ADDR, mdio_cmd);
		} else {
			al_eth_100g_mac_reg_write(mac_regs_base,
				ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_DATA_ADDR, *val);
		}
		/* wait for busy bit to clear */
		rc = mdio_10g_mac_wait_busy(obj);
		if (rc) {
			al_err(" %s mdio %s failed on timeout\n", obj->name, op);
			return rc;
		}

		mdio_cfg_status = al_eth_100g_mac_reg_read(mac_regs_base,
			ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_CFG_STATUS_ADDR);

		if (mdio_cfg_status & ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_CFG_STATUS_READ_ERR) {
			al_err(" %s mdio %s failed on error. port 0x%x, device 0x%x reg 0x%x\n",
				obj->name, op, port_addr, device, reg);
				return -EIO;
		}
		if (cmd_type == MAC_V4_MDIO_CMD_TYPE_READ)
			*val = al_eth_100g_mac_reg_read(mac_regs_base,
				ETH_MAC_GEN_V4_MAC_50G_100G_MDIO_DATA_ADDR);
		return 0;
	break;
	default:
		return -EINVAL;
	}
}

static int mdio_read(struct al_eth_mac_obj *obj, uint32_t phy_addr, uint32_t device,
			uint32_t reg, uint16_t *val)
{
	int rc;

	if (obj->mdio_if == AL_ETH_MDIO_IF_1G_MAC)
		al_assert(0);

	rc = mdio_lock(obj);
	/*"interface ownership taken"*/
	if (rc)
		return rc;

	if (obj->mdio_type == AL_ETH_MDIO_TYPE_CLAUSE_22)
		rc = mdio_10g_mac_type22(obj, MAC_V4_MDIO_CMD_TYPE_READ,
			phy_addr, reg, val);
	else if (obj->mdio_type == AL_ETH_MDIO_TYPE_CLAUSE_45)
		rc = mdio_10g_mac_type45(obj, MAC_V4_MDIO_CMD_TYPE_READ,
			phy_addr, device, reg, val);
	else
		al_assert(0);

	mdio_free(obj);
	al_dbg("eth mdio read: phy_addr %x, device %x, reg %x val %x\n", phy_addr, device,
		reg, *val);
	return rc;
}

static int mdio_write(struct al_eth_mac_obj *obj, uint32_t phy_addr, uint32_t device,
			uint32_t reg, uint16_t val)
{
	int rc;
	al_dbg("eth mdio write: phy_addr %x, device %x, reg %x, val %x\n",
		phy_addr, device, reg, val);

	if (obj->mdio_if == AL_ETH_MDIO_IF_1G_MAC)
		al_assert(0);

	rc = mdio_lock(obj);
	/* interface ownership taken */
	if (rc)
		return rc;

	if (obj->mdio_type == AL_ETH_MDIO_TYPE_CLAUSE_22)
		rc = mdio_10g_mac_type22(obj, MAC_V4_MDIO_CMD_TYPE_WRITE,
			phy_addr, reg, &val);
	else if (obj->mdio_type == AL_ETH_MDIO_TYPE_CLAUSE_45)
		rc = mdio_10g_mac_type45(obj, MAC_V4_MDIO_CMD_TYPE_WRITE,
			phy_addr, device, reg, &val);
	else
		al_assert(0);

	mdio_free(obj);
	return rc;
}

static const char *mac_rate_counter_mode_to_str(enum al_eth_mac_rate_counter_mode mode)
{
	switch (mode) {
	case AL_ETH_MAC_RATE_COUNTER_MODE_ONE_SHOT:
		return "one shot";
	case AL_ETH_MAC_RATE_COUNTER_MODE_WINDOW_AVERAGE:
		return "window average";
	case AL_ETH_MAC_RATE_COUNTER_MODE_WINDOW_COUNT:
		return "window counter";
	default:
		return "unknown mode";
	}
}

static int mac_rate_counter_handle_init(struct al_eth_mac_rate_counter_handle *handle,
		struct al_eth_mac_rate_counter_handle_init_params *handle_init_params)
{
	al_assert(handle);

	al_assert(handle_init_params->mac_obj);

	/** Do input checks */
	if (handle_init_params->ref_clk != AL_ETH_REF_FREQ_500_MHZ) {
		al_err("%s: MAC rate counters are supported only when io fabric clk is 500 Mhz\n",
			__func__);
		return -EINVAL;
	}

	al_assert(handle_init_params->direction == AL_ETH_MAC_RATE_COUNTER_TYPE_RX ||
			handle_init_params->direction == AL_ETH_MAC_RATE_COUNTER_TYPE_TX);

	al_assert(handle_init_params->mode <= AL_ETH_MAC_RATE_COUNTER_MODE_NUM);

	if (handle->mode != AL_ETH_MAC_RATE_COUNTER_MODE_ONE_SHOT) {
		if (handle_init_params->window_len > ETH_MAC_GEN_RATE_COUNTER_MAX_WIN)
			return -EINVAL;
		/** Verify window length is even */
		if (handle_init_params->window_len & AL_BIT(0))
			return -EINVAL;
	}

	handle->mac_obj = handle_init_params->mac_obj;
	handle->direction = handle_init_params->direction;
	handle->window_len = handle_init_params->window_len;
	handle->mode = handle_init_params->mode;

	return 0;
}

static int mac_rate_counter_config(const struct al_eth_mac_rate_counter_handle *handle)
{
	struct al_eth_mac_v4_regs *mac_regs_base = handle->mac_obj->mac_regs_base;
	uint32_t reg = 0;
	uint32_t *rate_conf_reg;
	uint32_t *rate_win_reg;

	al_assert(handle);

	if (handle->direction == AL_ETH_MAC_RATE_COUNTER_TYPE_RX) {
		rate_conf_reg = &mac_regs_base->gen.rx_rate_cnt;
		rate_win_reg = &mac_regs_base->gen.rx_rate_cnt_win;
	} else {
		rate_conf_reg = &mac_regs_base->gen.tx_rate_cnt;
		rate_win_reg = &mac_regs_base->gen.tx_rate_cnt_win;
	}

	/** Reset conf reg */
	al_reg_write32(rate_conf_reg, 0);

	AL_REG_FIELD_SET(reg,
		ETH_MAC_V4_GEN_TX_RATE_CNT_MODE_MASK,
		ETH_MAC_V4_GEN_TX_RATE_CNT_MODE_SHIFT, handle->mode);
	al_reg_write32_masked(rate_conf_reg, ETH_MAC_V4_GEN_TX_RATE_CNT_MODE_MASK, reg);

	if ((handle->mode == AL_ETH_MAC_RATE_COUNTER_MODE_WINDOW_AVERAGE) ||
		(handle->mode == AL_ETH_MAC_RATE_COUNTER_MODE_WINDOW_COUNT)) {
			al_reg_write32(rate_win_reg,
			(uint32_t)(handle->window_len / ETH_MAC_GEN_RATE_COUNTER_WIN_LEN_UNITS_NS));
			al_dbg("%s: Configured TX rate counter to %s mode with window of %"
				PRIu64" ns\n",
				__func__,
				mac_rate_counter_mode_to_str(handle->mode), handle->window_len);
	} else {
		al_dbg("%s: Configured TX rate counter to %s mode\n",
			__func__, mac_rate_counter_mode_to_str(handle->mode));
	}
	return 0;
}

static int mac_rate_counter_start(const struct al_eth_mac_rate_counter_handle *handle)
{
	struct al_eth_mac_v4_regs *mac_regs_base = handle->mac_obj->mac_regs_base;
	uint32_t *rate_conf_reg;

	if (handle->direction == AL_ETH_MAC_RATE_COUNTER_TYPE_RX)
		rate_conf_reg = &mac_regs_base->gen.rx_rate_cnt;
	else
		rate_conf_reg = &mac_regs_base->gen.tx_rate_cnt;

	al_reg_write32_masked(rate_conf_reg,
		ETH_MAC_V4_GEN_TX_RATE_CNT_START,
		ETH_MAC_V4_GEN_TX_RATE_CNT_START);

	return 0;
}

static int mac_rate_counter_stop(const struct al_eth_mac_rate_counter_handle *handle)
{
	struct al_eth_mac_v4_regs *mac_regs_base = handle->mac_obj->mac_regs_base;
	uint32_t *rate_conf_reg;

	if (handle->direction == AL_ETH_MAC_RATE_COUNTER_TYPE_RX)
		rate_conf_reg = &mac_regs_base->gen.rx_rate_cnt;
	else
		rate_conf_reg = &mac_regs_base->gen.tx_rate_cnt;

	al_reg_write32_masked(rate_conf_reg,
		ETH_MAC_V4_GEN_TX_RATE_CNT_START,
		0);

	return 0;
}

static int mac_rate_counter_get(const struct al_eth_mac_rate_counter_handle *handle,
	uint32_t *bytes, uint32_t *frames)
{
	struct al_eth_mac_v4_regs *mac_regs_base = handle->mac_obj->mac_regs_base;
	uint32_t *reg_bytes;
	uint32_t *reg_frames;

	if (handle->direction == AL_ETH_MAC_RATE_COUNTER_TYPE_RX) {
		reg_bytes = &mac_regs_base->gen.rx_rate_cnt_byte;
		reg_frames = &mac_regs_base->gen.rx_rate_cnt_frame;
	} else {
		reg_bytes = &mac_regs_base->gen.tx_rate_cnt_byte;
		reg_frames = &mac_regs_base->gen.tx_rate_cnt_frame;
	}

	if (bytes)
		*bytes = al_reg_read32(reg_bytes);

	if (frames)
		*frames = al_reg_read32(reg_frames);

	return 0;
}

static int mac_rate_counter_print(const struct al_eth_mac_rate_counter_handle *handle)
{
	uint32_t bytes, frames;
	int rc;

	rc = mac_rate_counter_get(handle, &bytes, &frames);
	if (rc != 0)
		return rc;

	al_print("MAC rate counter mode : %s\n", mac_rate_counter_mode_to_str(handle->mode));

	al_print("[%s] rate : %d bytes per %"PRIu64" ns window\n",
		(handle->direction == AL_ETH_MAC_RATE_COUNTER_TYPE_TX) ? "TX" : "RX",
		bytes, handle->window_len);
	al_print("[%s] rate : %d frames per %"PRIu64" ns window\n",
		(handle->direction == AL_ETH_MAC_RATE_COUNTER_TYPE_TX) ? "TX" : "RX",
		frames, handle->window_len);

	return 0;
}

#define AL_ETH_V4_MAC_IOFIC_GROUP_A_FATAL \
	AL_ETH_V4_MAC_IOFIC_GROUP_A_LMAC_STAT_PAR_ERR_SB_SYNC_EDGE

#define AL_ETH_V4_MAC_IOFIC_GROUP_B_FATAL 0

#define AL_ETH_V4_MAC_IOFIC_GROUP_C_FATAL 0

#define AL_ETH_V4_MAC_IOFIC_GROUP_D_FATAL 0

#define AL_ETH_V4_MAC_IOFIC_GROUP_A_NON_FATAL \
	(AL_ETH_V4_MAC_IOFIC_GROUP_A_LMAC_FF_TX_ERR_SB_SYNC_EDGE | \
	 AL_ETH_V4_MAC_IOFIC_GROUP_A_LMAC_FF_RX_ERR_SB_SYNC_EDGE | \
	 AL_ETH_V4_MAC_IOFIC_GROUP_A_LMAC_TX_UNDERFLOW_SB_SYNC_EDGE)

#define AL_ETH_V4_MAC_IOFIC_GROUP_B_NON_FATAL \
	AL_ETH_V4_MAC_IOFIC_GROUP_B_CMAC_TX_UNDERFLOW_SB_SYNC_EDGE

#define AL_ETH_V4_MAC_IOFIC_GROUP_C_NON_FATAL \
	(AL_ETH_V4_MAC_IOFIC_GROUP_C_PCS_FEC91_PAR_ERR_SB_SYNC_EDGE | \
	 AL_ETH_V4_MAC_IOFIC_GROUP_C_PCS_FEC74_PAR_ERR_SB_SYNC_EDGE | \
	 AL_ETH_V4_MAC_IOFIC_GROUP_C_PCS_DESKEW_PAR_ERR_SB_SYNC_EDGE)

#define AL_ETH_V4_MAC_IOFIC_GROUP_D_NON_FATAL 0

#define AL_ETH_V4_MAC_IOFIC_GROUP_A_SUM \
	(AL_ETH_V4_MAC_IOFIC_GROUP_A_FATAL | \
	 AL_ETH_V4_MAC_IOFIC_GROUP_A_NON_FATAL)

#define AL_ETH_V4_MAC_IOFIC_GROUP_B_SUM \
	(AL_ETH_V4_MAC_IOFIC_GROUP_B_FATAL | \
	 AL_ETH_V4_MAC_IOFIC_GROUP_B_NON_FATAL)

#define AL_ETH_V4_MAC_IOFIC_GROUP_C_SUM \
	(AL_ETH_V4_MAC_IOFIC_GROUP_C_FATAL | \
	 AL_ETH_V4_MAC_IOFIC_GROUP_C_NON_FATAL)

#define AL_ETH_V4_MAC_IOFIC_GROUP_D_SUM \
	(AL_ETH_V4_MAC_IOFIC_GROUP_D_FATAL | \
	 AL_ETH_V4_MAC_IOFIC_GROUP_D_NON_FATAL)

static void error_ints_unmask(struct al_eth_mac_obj *obj)
{
	uint32_t a, b, c, d;

	al_assert(obj);

	al_eth_mac_iofic_attrs_get(obj,
				   AL_ETH_IOFIC_ATTRS_RUNTIME_CONFIG |
				   AL_ETH_IOFIC_ATTRS_FILTER_TRANSIENT |
				   AL_ETH_IOFIC_ATTRS_FATAL,
				   &a, &b, &c, &d);

	/* MAC IOFIC config */
	al_iofic_config(obj->mac_iofic_regs_base,
			AL_INT_GROUP_A,
			INT_CONTROL_GRP_MASK_MSI_X);
	al_iofic_config(obj->mac_iofic_regs_base,
			AL_INT_GROUP_B,
			INT_CONTROL_GRP_MASK_MSI_X);
	al_iofic_config(obj->mac_iofic_regs_base,
			AL_INT_GROUP_C,
			INT_CONTROL_GRP_MASK_MSI_X);
	al_iofic_config(obj->mac_iofic_regs_base,
			AL_INT_GROUP_D,
			INT_CONTROL_GRP_MASK_MSI_X);

	/* Clear cause before unmasking */
	al_iofic_clear_cause(obj->mac_iofic_regs_base,
			AL_INT_GROUP_A,
			a);
	al_iofic_clear_cause(obj->mac_iofic_regs_base,
			AL_INT_GROUP_B,
			b);
	al_iofic_clear_cause(obj->mac_iofic_regs_base,
			AL_INT_GROUP_C,
			c);
	al_iofic_clear_cause(obj->mac_iofic_regs_base,
			AL_INT_GROUP_D,
			d);

	/* MAC IOFIC unmask */
	al_iofic_unmask(obj->mac_iofic_regs_base,
			AL_INT_GROUP_A,
			a);
	al_iofic_unmask(obj->mac_iofic_regs_base,
			AL_INT_GROUP_B,
			b);
	al_iofic_unmask(obj->mac_iofic_regs_base,
			AL_INT_GROUP_C,
			c);
	al_iofic_unmask(obj->mac_iofic_regs_base,
			AL_INT_GROUP_D,
			d);
}

static void iofic_attrs_get_hw(struct al_eth_mac_obj *obj __attribute__((unused)),
			       uint32_t *mac_a_mask,
			       uint32_t *mac_b_mask,
			       uint32_t *mac_c_mask,
			       uint32_t *mac_d_mask)
{
	al_assert(mac_a_mask);
	al_assert(mac_b_mask);
	al_assert(mac_c_mask);
	al_assert(mac_d_mask);

	*mac_a_mask = AL_ETH_V4_MAC_IOFIC_GROUP_A_SUM;
	*mac_b_mask = AL_ETH_V4_MAC_IOFIC_GROUP_B_SUM;
	*mac_c_mask = AL_ETH_V4_MAC_IOFIC_GROUP_C_SUM;
	*mac_d_mask = AL_ETH_V4_MAC_IOFIC_GROUP_D_SUM;
}

static void iofic_attrs_get_runtime_inval(struct al_eth_mac_obj *obj,
					  uint32_t *mac_a_mask,
					  uint32_t *mac_b_mask,
					  uint32_t *mac_c_mask,
					  uint32_t *mac_d_mask)
{
	struct al_hal_eth_adapter *adapter = al_container_of(obj,
							     struct al_hal_eth_adapter,
							     mac_obj);
	al_assert(mac_a_mask);
	al_assert(mac_b_mask);
	al_assert(mac_c_mask);
	al_assert(mac_d_mask);

	*mac_a_mask = 0;
	*mac_b_mask = 0;
	*mac_c_mask = 0;
	*mac_d_mask = 0;

	switch (adapter->common_mode) {
	case AL_ETH_COMMON_MODE_1X100G:
		*mac_c_mask |= AL_ETH_V4_MAC_IOFIC_GROUP_C_PCS_FEC74_PAR_ERR_SB_SYNC_EDGE;
		break;
	default:
		*mac_c_mask |= AL_ETH_V4_MAC_IOFIC_GROUP_C_PCS_FEC91_PAR_ERR_SB_SYNC_EDGE;
		break;
	}
}

static void iofic_attrs_get_transient(struct al_eth_mac_obj *obj __attribute__((unused)),
				      uint32_t *mac_a_mask,
				      uint32_t *mac_b_mask,
				      uint32_t *mac_c_mask,
				      uint32_t *mac_d_mask)
{
	al_assert(mac_a_mask);
	al_assert(mac_b_mask);
	al_assert(mac_c_mask);
	al_assert(mac_d_mask);

	*mac_a_mask = AL_ETH_V4_MAC_IOFIC_GROUP_A_NON_FATAL;
	*mac_b_mask = AL_ETH_V4_MAC_IOFIC_GROUP_B_NON_FATAL;
	*mac_c_mask = AL_ETH_V4_MAC_IOFIC_GROUP_C_NON_FATAL;
	*mac_d_mask = AL_ETH_V4_MAC_IOFIC_GROUP_D_NON_FATAL;
}

static void iofic_attrs_get_non_fatal(struct al_eth_mac_obj *obj __attribute__((unused)),
				    uint32_t *mac_a_mask,
				    uint32_t *mac_b_mask,
				    uint32_t *mac_c_mask,
				    uint32_t *mac_d_mask)
{
	al_assert(mac_a_mask);
	al_assert(mac_b_mask);
	al_assert(mac_c_mask);
	al_assert(mac_d_mask);

	*mac_a_mask = AL_ETH_V4_MAC_IOFIC_GROUP_A_NON_FATAL;
	*mac_b_mask = AL_ETH_V4_MAC_IOFIC_GROUP_B_NON_FATAL;
	*mac_c_mask = AL_ETH_V4_MAC_IOFIC_GROUP_C_NON_FATAL;
	*mac_d_mask = AL_ETH_V4_MAC_IOFIC_GROUP_D_NON_FATAL;
}

static int al_eth_mac_v4_led_set(struct al_eth_mac_obj *obj, al_bool link_is_up)
{
	struct al_eth_mac_v4_regs *mac_regs_base = obj->mac_regs_base;
	uint32_t reg = 0;
	uint32_t mode  = ETH_MAC_GEN_LED_CFG_SEL_DEFAULT_REG;

	if (link_is_up)
		mode = ETH_MAC_GEN_LED_CFG_SEL_LINK_ACTIVITY;

	AL_REG_FIELD_SET(reg,  ETH_MAC_GEN_LED_CFG_SEL_MASK,
			 ETH_MAC_GEN_LED_CFG_SEL_SHIFT, mode);

	AL_REG_FIELD_SET(reg, ETH_MAC_GEN_LED_CFG_BLINK_TIMER_MASK,
			 ETH_MAC_GEN_LED_CFG_BLINK_TIMER_SHIFT,
			 ETH_MAC_GEN_LED_CFG_BLINK_TIMER_VAL);

	AL_REG_FIELD_SET(reg, ETH_MAC_GEN_LED_CFG_ACT_TIMER_MASK,
			 ETH_MAC_GEN_LED_CFG_ACT_TIMER_SHIFT,
			 ETH_MAC_GEN_LED_CFG_ACT_TIMER_VAL);

	al_reg_write32(&mac_regs_base->gen.led_cfg, reg);

	return 0;
}

int al_eth_mac_v4_handle_init(struct al_eth_mac_obj *obj,
	__attribute__((unused)) struct al_eth_mac_obj_init_params *params)
{
	obj->mode_set = mac_mode_set;
	obj->mode_get = mac_mode_get;
	obj->config = mac_config;
	obj->loopback_config = mac_loopback_config;
	obj->flow_control_config_full = flow_control_config_v4;
	obj->fec_enable = fec_enable;
	obj->link_status_get = link_status_get;
	obj->link_status_clear = link_status_clear;
	obj->rx_pkt_limit_config = rx_pkt_limit_config;
	obj->tx_flush_config = tx_flush_config;
	obj->stats_get = mac_stats_get;
	obj->mdio_config = mdio_config;
	obj->mdio_pol_config = mdio_pol_config;
	obj->mdio_read = mdio_read;
	obj->mdio_write = mdio_write;
	obj->led_set = al_eth_mac_v4_led_set;
	obj->rate_counter_handle_init = mac_rate_counter_handle_init;
	obj->rate_counter_config = mac_rate_counter_config;
	obj->rate_counter_start = mac_rate_counter_start;
	obj->rate_counter_stop =  mac_rate_counter_stop;
	obj->rate_counter_get = mac_rate_counter_get;
	obj->rate_counter_print = mac_rate_counter_print;
	obj->error_ints_unmask = error_ints_unmask;
	obj->fec_is_enabled = fec_is_enabled;
	obj->fec_stats_get = fec_stats_get;
	obj->iofic_attrs_get_hw = iofic_attrs_get_hw;
	obj->iofic_attrs_get_runtime_inval = iofic_attrs_get_runtime_inval;
	obj->iofic_attrs_get_transient = iofic_attrs_get_transient;
	obj->iofic_attrs_get_non_fatal = iofic_attrs_get_non_fatal;
	obj->mac_start_stop_adv = mac_start_stop_adv;

	return 0;
}

/** @} end of Ethernet group */
