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
 * @defgroup group_eth_api API
 * Ethernet MAC HAL driver API
 * @ingroup group_eth
 * @{
 * @file   al_mod_hal_eth_mac.h
 *
 * @brief Annpurna Labs Ethernet MAC API header file
 *
 */

#ifndef __AL_HAL_ETH_MAC_H__
#define __AL_HAL_ETH_MAC_H__

#include "al_mod_hal_common.h"
#include "al_mod_hal_eth_defs.h"
#include "al_mod_hal_eth_mac_v4_regs.h"
#include <linux/spinlock.h>

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

#define AL_ETH_IS_1G_MAC(mac_mode) (((mac_mode) == AL_ETH_MAC_MODE_RGMII) ||			\
					((mac_mode) == AL_ETH_MAC_MODE_SGMII))
#define AL_ETH_IS_10G_MAC(mac_mode)	(((mac_mode) == AL_ETH_MAC_MODE_10GbE_Serial) ||	\
					((mac_mode) == AL_ETH_MAC_MODE_10G_SGMII) ||		\
					((mac_mode) == AL_ETH_MAC_MODE_SGMII_2_5G))
#define AL_ETH_IS_25G_MAC(mac_mode) ((mac_mode) == AL_ETH_MAC_MODE_KR_LL_25G)
#define AL_ETH_IS_40G_MAC(mac_mode)	(((mac_mode) == AL_ETH_MAC_MODE_XLG_LL_40G) ||		\
					((mac_mode) == AL_ETH_MAC_MODE_XLG_LL_50G))


/*********************************** IOFIC *****************************************/

/* MAC IOFIC group A */
#define AL_ETH_MAC_IOFIC_GROUP_A_SERDES_3_TX_FIFO_ERR		AL_BIT(0)
#define AL_ETH_MAC_IOFIC_GROUP_A_SERDES_2_TX_FIFO_ERR		AL_BIT(1)
#define AL_ETH_MAC_IOFIC_GROUP_A_SERDES_1_TX_FIFO_ERR		AL_BIT(2)
#define AL_ETH_MAC_IOFIC_GROUP_A_SERDES_0_TX_FIFO_ERR		AL_BIT(3)
#define AL_ETH_MAC_IOFIC_GROUP_A_SERDES_3_TX_FIFO_FULL		AL_BIT(4)
#define AL_ETH_MAC_IOFIC_GROUP_A_SERDES_2_TX_FIFO_FULL		AL_BIT(5)
#define AL_ETH_MAC_IOFIC_GROUP_A_SERDES_1_TX_FIFO_FULL		AL_BIT(6)
#define AL_ETH_MAC_IOFIC_GROUP_A_SERDES_0_TX_FIFO_FULL		AL_BIT(7)
#define AL_ETH_V1_V2_MAC_IOFIC_GROUP_A_XGMII_DFIFO_64_32_ERR	AL_BIT(8)
#define AL_ETH_V1_V2_MAC_IOFIC_GROUP_A_XGMII_DFIFO_64_32_FULL	AL_BIT(9)
#define AL_ETH_V1_V2_MAC_IOFIC_GROUP_A_XGMII_DFIFO_32_64_ERR	AL_BIT(10)
#define AL_ETH_V1_V2_MAC_IOFIC_GROUP_A_XGMII_DFIFO_32_64_FULL	AL_BIT(11)

/* MAC IOFIC group B */
#define AL_ETH_V2_V3_MAC_IOFIC_GROUP_B_1G_TX_FIFO_PARITY	AL_BIT(24)
#define AL_ETH_MAC_IOFIC_GROUP_B_1G_TX_RTSM_PARITY		AL_BIT(25)
#define AL_ETH_MAC_IOFIC_GROUP_B_1G_RX_FIFO_PARITY		AL_BIT(26)
#define AL_ETH_V1_V2_MAC_IOFIC_GROUP_B_10G_RX_DATA_FIFO_PARITY	AL_BIT(27)
#define AL_ETH_V1_V2_MAC_IOFIC_GROUP_B_10G_RX_CTRL_FIFO_PARITY	AL_BIT(28)
#define AL_ETH_V2_MAC_IOFIC_GROUP_B_10G_TX_DATA_FIFO_PARITY	AL_BIT(29)
#define AL_ETH_V2_MAC_IOFIC_GROUP_B_10G_TX_CTRL_FIFO_PARITY	AL_BIT(30)
#define AL_ETH_V3_MAC_IOFIC_GROUP_B_10G_LL_STATS_TX_PARITY	AL_BIT(27)
#define AL_ETH_V3_MAC_IOFIC_GROUP_B_10G_LL_STATS_RX_PARITY	AL_BIT(28)
#define AL_ETH_V3_MAC_IOFIC_GROUP_B_40G_LL_STATS_TX_PARITY	AL_BIT(29)
#define AL_ETH_V3_MAC_IOFIC_GROUP_B_40G_LL_STATS_RX_PARITY	AL_BIT(30)

/* MAC IOFIC group C */
#define AL_ETH_MAC_IOFIC_GROUP_C_10G_PCS_MEM_PARITY		AL_BIT(29)
#define AL_ETH_MAC_IOFIC_GROUP_C_10G_PCS_LOOPBACK_PARITY	AL_BIT(30)
#define AL_ETH_MAC_IOFIC_GROUP_C_10G_PCS_DELAY_RAM_PARITY	AL_BIT(31)

/* MAC IOFIC group D */
#define AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_DELAY_1_PARITY		AL_BIT(15)
#define AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_DELAY_0_PARITY		AL_BIT(16)
#define AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_FEC91_RAM_PARITY	AL_BIT(17)
#define AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_DELAY_RAM_3_PARITY	AL_BIT(18)
#define AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_DELAY_RAM_2_PARITY	AL_BIT(19)
#define AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_DELAY_RAM_1_PARITY	AL_BIT(20)
#define AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_DELAY_RAM_0_PARITY	AL_BIT(21)
#define AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_MEM_3_PARITY		AL_BIT(22)
#define AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_MEM_2_PARITY		AL_BIT(23)
#define AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_MEM_1_PARITY		AL_BIT(24)
#define AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_MEM_0_PARITY		AL_BIT(25)
#define AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_DESK_3_PARITY		AL_BIT(26)
#define AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_DESK_2_PARITY		AL_BIT(27)
#define AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_DESK_1_PARITY		AL_BIT(28)
#define AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_DESK_0_PARITY		AL_BIT(29)

#define AL_ETH_MAC_GROUP_D_ERROR_INTS \
	(AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_DELAY_1_PARITY | \
	 AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_DELAY_0_PARITY | \
	 AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_FEC91_RAM_PARITY | \
	 AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_DELAY_RAM_3_PARITY | \
	 AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_DELAY_RAM_2_PARITY | \
	 AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_DELAY_RAM_1_PARITY | \
	 AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_DELAY_RAM_0_PARITY | \
	 AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_MEM_3_PARITY | \
	 AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_MEM_2_PARITY | \
	 AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_MEM_1_PARITY | \
	 AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_MEM_0_PARITY | \
	 AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_DESK_3_PARITY | \
	 AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_DESK_2_PARITY | \
	 AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_DESK_1_PARITY | \
	 AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_DESK_0_PARITY)

/* MAC V4 IOFIC group A */
#define AL_ETH_V4_MAC_IOFIC_GROUP_A_LMAC_FF_TX_ERR_SB_SYNC_EDGE				AL_BIT(0)
#define AL_ETH_V4_MAC_IOFIC_GROUP_A_LMAC_FF_RX_ERR_SB_SYNC_EDGE				AL_BIT(1)
#define AL_ETH_V4_MAC_IOFIC_GROUP_A_LMAC_TX_UNDERFLOW_SB_SYNC_EDGE			AL_BIT(5)
#define AL_ETH_V4_MAC_IOFIC_GROUP_A_LMAC_RX_OVERFLOW_SB_SYNC_EDGE			AL_BIT(6)
#define AL_ETH_V4_MAC_IOFIC_GROUP_A_LMAC_STAT_PAR_ERR_SB_SYNC_EDGE			AL_BIT(31)

/* MAC V4 IOFIC group B */
#define AL_ETH_V4_MAC_IOFIC_GROUP_B_CMAC_FF_TX_ERR_SB_SYNC_EDGE				AL_BIT(0)
#define AL_ETH_V4_MAC_IOFIC_GROUP_B_CMAC_FF_RX_ERR_SB_SYNC_EDGE				AL_BIT(1)
#define AL_ETH_V4_MAC_IOFIC_GROUP_B_CMAC_TX_UNDERFLOW_SB_SYNC_EDGE			AL_BIT(5)
#define AL_ETH_V4_MAC_IOFIC_GROUP_B_CMAC_RX_OVERFLOW_SB_SYNC_EDGE			AL_BIT(6)

/* MAC V4 IOFIC group C */
#define AL_ETH_V4_MAC_IOFIC_GROUP_C_PCS_FEC91_PAR_ERR_SB_SYNC_EDGE			AL_BIT(29)
#define AL_ETH_V4_MAC_IOFIC_GROUP_C_PCS_FEC74_PAR_ERR_SB_SYNC_EDGE			AL_BIT(30)
#define AL_ETH_V4_MAC_IOFIC_GROUP_C_PCS_DESKEW_PAR_ERR_SB_SYNC_EDGE			AL_BIT(31)

/* MAC V4 IOFIC group D */
#define AL_ETH_V4_MAC_IOFIC_GROUP_D_PCS_FEC_NCERR_SB_SYNC_EDGE_0			AL_BIT(0)
#define AL_ETH_V4_MAC_IOFIC_GROUP_D_PCS_FEC_NCERR_SB_SYNC_EDGE_1			AL_BIT(1)
#define AL_ETH_V4_MAC_IOFIC_GROUP_D_PCS_FEC_NCERR_SB_SYNC_EDGE_2			AL_BIT(2)
#define AL_ETH_V4_MAC_IOFIC_GROUP_D_PCS_FEC_NCERR_SB_SYNC_EDGE_3			AL_BIT(3)
#define AL_ETH_V4_MAC_IOFIC_GROUP_D_PCS_FEC_NCERR_SB_SYNC_EDGE_4			AL_BIT(4)
#define AL_ETH_V4_MAC_IOFIC_GROUP_D_PCS_FEC_NCERR_SB_SYNC_EDGE_5			AL_BIT(5)
#define AL_ETH_V4_MAC_IOFIC_GROUP_D_PCS_FEC_NCERR_SB_SYNC_EDGE_6			AL_BIT(6)
#define AL_ETH_V4_MAC_IOFIC_GROUP_D_PCS_FEC_NCERR_SB_SYNC_EDGE_7			AL_BIT(7)
#define AL_ETH_V4_MAC_IOFIC_GROUP_D_PCS_FEC_CERR_SB_SYNC_EDGE_0				AL_BIT(8)
#define AL_ETH_V4_MAC_IOFIC_GROUP_D_PCS_FEC_CERR_SB_SYNC_EDGE_1				AL_BIT(9)
#define AL_ETH_V4_MAC_IOFIC_GROUP_D_PCS_FEC_CERR_SB_SYNC_EDGE_2				AL_BIT(10)
#define AL_ETH_V4_MAC_IOFIC_GROUP_D_PCS_FEC_CERR_SB_SYNC_EDGE_3				AL_BIT(11)
#define AL_ETH_V4_MAC_IOFIC_GROUP_D_PCS_FEC_CERR_SB_SYNC_EDGE_4				AL_BIT(12)
#define AL_ETH_V4_MAC_IOFIC_GROUP_D_PCS_FEC_CERR_SB_SYNC_EDGE_5				AL_BIT(13)
#define AL_ETH_V4_MAC_IOFIC_GROUP_D_PCS_FEC_CERR_SB_SYNC_EDGE_6				AL_BIT(14)
#define AL_ETH_V4_MAC_IOFIC_GROUP_D_PCS_FEC_CERR_SB_SYNC_EDGE_7				AL_BIT(15)

/**
 * MAC media mode
 *
 * +---------------------+----+----+----+----+
 * | Mode \ MAC version  | v1 | v2 | v3 | v4 |
 * +---------------------+----+----+----+----+
 * | RGMII               | v  | v  |    |    |
 * +---------------------+----+----+----+----+
 * | SGMII               | v  | v  | v  |    |
 * +---------------------+----+----+----+----+
 * | SGMII_2_5G          | v  | v  | v  |    |
 * +---------------------+----+----+----+----+
 * | 10GbE_Serial        | v  | v  | v  | v  |
 * +---------------------+----+----+----+----+
 * | 10G_SGMII           | v  | v  |    |    |
 * +---------------------+----+----+----+----+
 * | XLG_LL_40G          |    |    | v  | v  |
 * +---------------------+----+----+----+----+
 * | KR_LL_25G           |    |    | v  |    |
 * +---------------------+----+----+----+----+
 * | XLG_LL_50G          |    |    | v  | v  |
 * +---------------------+----+----+----+----+
 * | XLG_LL_25G          |    |    | v  | v  |
 * +---------------------+----+----+----+----+
 * | CG_100G             |    |    |    | v  |
 * +---------------------+----+----+----+----+
 */
#define AL_ETH_MAC_HAS_MAC_MODE_ETH_V4
enum al_mod_eth_mac_mode {
	/** RGMII */
	AL_ETH_MAC_MODE_RGMII,
	/** SGMII */
	AL_ETH_MAC_MODE_SGMII,
	/** SGMII 2.5G */
	AL_ETH_MAC_MODE_SGMII_2_5G,
	/** XFI and KR modes */
	AL_ETH_MAC_MODE_10GbE_Serial,
	/** SGMII using the 10G MAC, don't use */
	AL_ETH_MAC_MODE_10G_SGMII,
	/** 40G mode using the 40G low latency (LL) MAC */
	AL_ETH_MAC_MODE_XLG_LL_40G,
	/** 25G mode using the 10/25G low latency (LL) MAC */
	AL_ETH_MAC_MODE_KR_LL_25G,
	/** 50G mode using the 40/50G low latency (LL) MAC */
	AL_ETH_MAC_MODE_XLG_LL_50G,
	/** 25G mode using the 40/50G low latency (LL) MAC */
	AL_ETH_MAC_MODE_XLG_LL_25G,
	/** 100G mode using the 100G MAC */
	AL_ETH_MAC_MODE_CG_100G,
	AL_ETH_MAC_MODE_NUM,
};

/** Available SerDes lanes for MAC instances */
enum al_mod_eth_mac_common_mac_v4_lane {
	AL_ETH_MAC_V4_LANE_0,
	AL_ETH_MAC_V4_LANE_1,
	AL_ETH_MAC_V4_LANE_2,
	AL_ETH_MAC_V4_LANE_3,
	AL_ETH_MAC_V4_LANE_MAX
};

/**
 * FEC (Forward Error Correction) types
 *
 * +---------------------+----+----+----+----+
 * | Type \ MAC version  | v1 | v2 | v3 | v4 |
 * +---------------------+----+----+----+----+
 * | CLAUSE_74           |    |    | v  |    |
 * +---------------------+----+----+----+----+
 * | CLAUSE_91           |    |    | v  |    |
 * +---------------------+----+----+----+----+
 */
enum al_mod_eth_fec_type {
	/** None (invalid value) */
	AL_ETH_FEC_TYPE_NONE,
	/** Clause 74 */
	AL_ETH_FEC_TYPE_CLAUSE_74,
	/** Clause 91 */
	AL_ETH_FEC_TYPE_CLAUSE_91
};

/** interface type used for MDIO */
enum al_mod_eth_mdio_if {
	AL_ETH_MDIO_IF_1G_MAC = 0,
	AL_ETH_MDIO_IF_10G_MAC = 1,
	AL_ETH_MDIO_IF_MAX,
};

/** MDIO protocol type */
enum al_mod_eth_mdio_type {
	AL_ETH_MDIO_TYPE_CLAUSE_22 = 0,
	AL_ETH_MDIO_TYPE_CLAUSE_45 = 1,
	AL_ETH_MDIO_TYPE_MAX,
};

/** MDC MDIO polarity */
enum al_mod_eth_mdc_mdio_pol {
	AL_ETH_MDC_MDIO_POL_NORMAL = 0,
	AL_ETH_MDC_MDIO_POL_FLIPPED = 1
};

#define aOctetsReceivedOK			octets_rxed_ok
#define aOctetsTransmittedOK			octets_txed_ok
#define etherStatsPkts				pkts
#define ifInUcastPkts				if_in_ucast_pkts
#define ifInMulticastPkts			if_in_mcast_pkts
#define ifInBroadcastPkts			if_in_bcast_pkts
#define ifInErrors				if_in_errors
#define ifOutUcastPkts				if_out_ucast_pkts
#define ifOutMulticastPkts			if_out_mcast_pkts
#define ifOutBroadcastPkts			if_out_bcast_pkts
#define ifOutErrors				if_out_errors
#define aFramesReceivedOK			frames_rxed_ok
#define aFramesTransmittedOK			frames_txed_ok
#define etherStatsUndersizePkts			undersize_pkts
#define etherStatsFragments			fragments
#define etherStatsJabbers			jabbers
#define etherStatsOversizePkts			oversize_pkts
#define aFrameCheckSequenceErrors		fcs_errors
#define aAlignmentErrors			alignment_errors
#define etherStatsDropEvents			drop_events
#define aPAUSEMACCtrlFramesTransmitted		pause_mac_ctrl_frames_txed
#define aPAUSEMACCtrlFramesReceived		pause_mac_ctrl_frames_rxed
#define etherStatsOctets			octets
#define etherStatsPkts64Octets			pkts_64_octets
#define etherStatsPkts65to127Octets		pkts_65_to_127_octets
#define etherStatsPkts128to255Octets		pkts_128_to_255_octets
#define etherStatsPkts256to511Octets		pkts_256_to_511_octets
#define etherStatsPkts512to1023Octets		pkts_512_to_1023_octets
#define etherStatsPkts1024to1518Octets		pkts_1024_to_1518_octets
#define etherStatsPkts1519toX			pkts_1519_to_x
#define aFrameTooLongErrors			frame_too_long_errs
#define aInRangeLengthErrors			in_range_length_errs
#define VLANTransmittedOK			vlan_txed_ok
#define VLANReceivedOK				vlan_rxed_ok

/** MAC statistics */
struct al_mod_eth_mac_stats {
	/** Sum the data and padding octets (i.e. without header and FCS)
	 *  received with a valid frame */
	uint64_t octets_rxed_ok;
	/** Sum of Payload and padding octets of frames transmitted without error */
	uint64_t octets_txed_ok;
	/** Total number of packets received. Good and bad packets */
	uint32_t pkts;
	/** Number of received unicast packets */
	uint32_t if_in_ucast_pkts;
	/** Number of received multicast packets */
	uint32_t if_in_mcast_pkts;
	/** Number of received broadcast packets */
	uint32_t if_in_bcast_pkts;
	/** Number of frames received with FIFO Overflow, CRC, Payload Length, Jabber and
	 *  Oversized, Alignment or PHY/PCS error indication */
	uint32_t if_in_errors;

	/** Number of transmitted unicast packets */
	uint32_t if_out_ucast_pkts;
	/** Number of transmitted multicast packets */
	uint32_t if_out_mcast_pkts;
	/** Number of transmitted broadcast packets */
	uint32_t if_out_bcast_pkts;
	/** Number of frames transmitted with FIFO Overflow, FIFO Underflow or Controller
	 *  indicated error */
	uint32_t if_out_errors;

	/** Number of Frame received without error (Including Pause Frames) */
	uint32_t frames_rxed_ok;
	/** Number of Frames transmitter without error (Including Pause Frames) */
	uint32_t frames_txed_ok;
	/** Number of packets received with less than 64 octets */
	uint32_t undersize_pkts;
	/** Too short frames with CRC error, available only for RGMII and 1G Serial modes */
	uint32_t fragments;
	/** Too long frames with CRC error */
	uint32_t jabbers;
	/** Packet that exceeds the valid maximum programmed frame length */
	uint32_t oversize_pkts;
	/** Number of frames received with a CRC error */
	uint32_t fcs_errors;
	/** Number of frames received with alignment error */
	uint32_t alignment_errors;
	/** Number of dropped packets due to FIFO overflow */
	uint32_t drop_events;
	/** Number of transmitted pause frames. */
	uint32_t pause_mac_ctrl_frames_txed;
	/** Number of received pause frames. */
	uint32_t pause_mac_ctrl_frames_rxed;
	/** Frame received exceeded the maximum length programmed with register FRM_LGTH,
	 *  available only for 10G modes */
	uint32_t frame_too_long_errs;
	/** Received frame with bad length/type (between 46 and 0x600 or less
	 *  than 46 for packets longer than 64), available only for 10G modes */
	uint32_t in_range_length_errs;
	/** Valid VLAN tagged frames transmitted */
	uint32_t vlan_txed_ok;
	/** Valid VLAN tagged frames received */
	uint32_t vlan_rxed_ok;
	/** Total number of octets received. Good and bad packets */
	uint32_t octets;

	/** Packets of 64 octets length is received (good and bad frames are counted) */
	uint32_t pkts_64_octets;
	/** Frames (good and bad) with 65 to 127 octets */
	uint32_t pkts_65_to_127_octets;
	/** Frames (good and bad) with 128 to 255 octets */
	uint32_t pkts_128_to_255_octets;
	/** Frames (good and bad) with 256 to 511 octets */
	uint32_t pkts_256_to_511_octets;
	/** Frames (good and bad) with 512 to 1023 octets */
	uint32_t pkts_512_to_1023_octets;
	/** Frames (good and bad) with 1024 to 1518 octets */
	uint32_t pkts_1024_to_1518_octets;
	/** Frames with 1519 bytes to the maximum length programmed in the register FRAME_LENGTH */
	uint32_t pkts_1519_to_x;

	uint32_t eee_in;
	uint32_t eee_out;

	/** Statistics for MAV V4, Valid only if mac_v4_stats_valid is true */
	al_mod_bool mac_v4_stats_valid;
	struct al_mod_eth_mac_v4_stat mac_v4_stats;
};

/* link */
struct al_mod_eth_link_status {
	al_mod_bool		link_up;
	al_mod_bool		local_fault;
	al_mod_bool		remote_fault;
};

/* rate counter configuration */
enum al_mod_eth_mac_rate_counter_mode {
	/** One shot */
	AL_ETH_MAC_RATE_COUNTER_MODE_ONE_SHOT = 0,
	/** Count bytes/frames in time window */
	AL_ETH_MAC_RATE_COUNTER_MODE_WINDOW_COUNT,
	/** Average bytes/frames in time window */
	AL_ETH_MAC_RATE_COUNTER_MODE_WINDOW_AVERAGE,
	/** Number of modes */
	AL_ETH_MAC_RATE_COUNTER_MODE_NUM,
};

enum al_mod_eth_mac_rate_counter_direction {
	/** RX rate counter */
	AL_ETH_MAC_RATE_COUNTER_TYPE_RX,
	/** TX rate counter */
	AL_ETH_MAC_RATE_COUNTER_TYPE_TX
};

/** Forward Declarations */
struct al_mod_hal_eth_adapter;
struct al_mod_eth_eee_params;
struct al_mod_eth_flow_control_params;
struct al_mod_hal_eth_an_lt_dme_downsample_conf;

struct al_mod_eth_mac_rate_counter_handle_init_params {
	struct al_mod_eth_mac_obj *mac_obj; /** pointer to private object */
	enum al_mod_eth_mac_rate_counter_direction direction; /* RX or TX */
	enum al_mod_eth_mac_rate_counter_mode mode; /* mode of counter */
	/**
	 * windows length
	 * must be a product of units of reference clock duration
	 * (typicaly 500Mhz clock - 2 ns cycle)
	 */
	uint64_t window_len;
	enum al_mod_eth_ref_clk_freq ref_clk; /** I/O FABRIC (sb) reference clk  */
};
struct al_mod_eth_mdio_config_params {
	enum al_mod_eth_mdio_type mdio_type;
	al_mod_bool	shared_mdio_if;
	enum al_mod_eth_ref_clk_freq ref_clk_freq;
	unsigned int mdio_clk_freq_khz;
	al_mod_bool ignore_read_status;
};
struct al_mod_eth_mac_rate_counter_handle {
	struct al_mod_eth_mac_obj *mac_obj; /** pointer to private object */
	enum al_mod_eth_mac_rate_counter_direction direction; /* RX or TX */
	enum al_mod_eth_mac_rate_counter_mode mode; /* mode of counter */
	uint64_t window_len; /** window length in units of [ns] will be written as units of 2ns */
};

/** Ethernet MAC object */
#define AL_ETH_HAS_MAC_OBJ
struct al_mod_eth_mac_obj {
	/** MAC registers base address */
	void __iomem *mac_regs_base;
	/** MAC iofic regs base address */
	void __iomem *mac_iofic_regs_base;
	/** MAC common registers base address */
	void __iomem *mac_common_regs;
	/** MAC common registers lock/unlock callback */
	int (*mac_common_regs_lock)(void *handle, al_mod_bool lock);
	/** MAC v4 common registers lock/unlock handle */
	void __iomem *mac_common_regs_lock_handle;

	/** SerDes lane */
	unsigned int serdes_lane;
	/** MAC mode */
	enum al_mod_eth_mac_mode mac_mode;
	/** FEC type */
	enum al_mod_eth_fec_type fec_type;

	/** the upper layer must keep the string area */
	char *name;

	/** Which MAC MDIO interface to use */
	enum al_mod_eth_mdio_if	mdio_if;
	/** MDIO protocol type */
	enum al_mod_eth_mdio_type mdio_type;
	/** When AL_TRUE, the MDIO interface is shared with other controllers */
	al_mod_bool	shared_mdio_if;
	/** When AL_TRUE, the MDIO interface accept READ replies despite errors */
	al_mod_bool ignore_read_status;
	/** DME (Differential manchester encoding) downsample */
	struct al_mod_hal_eth_an_lt_dme_downsample_conf *dme_conf;

	/** Ethernet original revision ID */
	uint8_t eth_original_rev_id;

	/** Additional data to overcome rx octets 32bit HW overflow */
	uint64_t octets_rxed_ok_prev32;
	uint64_t octets_rxed_ok_reported64;
	/** Additional data to overcome tx octets 32bit HW overflow */
	uint64_t octets_txed_ok_prev32;
	uint64_t octets_txed_ok_reported64;
	al_mod_bool octets_overflow_fix_adapter_started;
	/** Overflow fix is disabled ~30 seconds after mac start */
	al_mod_bool octets_overflow_fix_active;
	/** jiffies to activate overflow fix - 30s after mac start */
	u64 octets_overflow_fix_activate_jiffies;
	spinlock_t stats_get_lock;

	/**
	 * configure the mac media type.
	 * this function only sets the mode, but not the speed as certain mac modes
	 * support multiple speeds as will be negotiated by the link layer.
	 * @param adapter pointer to the private structure.
	 * @param mode media mode
	 *
	 * @return 0 on success. negative errno on failure.
	 */
	int (*config)(struct al_mod_eth_mac_obj *, enum al_mod_eth_mac_mode);

	/**
	 * Set the mac mode variable of the mac obj
	 * this function only sets the mode variabe and does not configure any thing on
	 * hardware - to be called after mac config or from application if the firmware
	 * does mac config
	 * @return 0 on success. negative errno on failure.
	 */
	int (*mode_set)(struct al_mod_eth_mac_obj *, enum al_mod_eth_mac_mode);

	/**
	 * Get the mac mode variable of the mac obj
	 * this function gets the mac_mode from a scratch register that was updated
	 * by the config function above
	 * This scratch pad register is inside the MAC and not EC, thus this function
	 * is working only for mac v4
	 * @return 0 on success. negative errno on failure.
	 */
	int (*mode_get)(struct al_mod_eth_mac_obj *, enum al_mod_eth_mac_mode *);

	/**
	 * update link auto negotiation speed and duplex mode
	 * this function assumes the mac mode already set using the al_mod_eth_mac_config()
	 * function.
	 *
	 * @param obj pointer to the private structure
	 * @param force_1000_base_x set to AL_TRUE to force the mac to work on 1000baseX
	 *	  (not relevant to RGMII)
	 * @param an_enable set to AL_TRUE to enable auto negotiation
	 *	  (not relevant to RGMII)
	 * @param speed in mega bits, e.g 1000 stands for 1Gbps (relevant only in case
	 *	  an_enable is AL_FALSE)
	 * @param full_duplex set to AL_TRUE to enable full duplex mode (relevant only
	 *	  in case an_enable is AL_FALSE)
	 *
	 * @return 0 on success. otherwise on failure.
	 */
	int (*link_config)(struct al_mod_eth_mac_obj *,
				   al_mod_bool force_1000_base_x, al_mod_bool an_enable,
				   uint32_t speed, al_mod_bool full_duplex);

	/**
	 * get link status
	 *
	 * this function should be used when no external phy is used to get
	 * information about the link
	 *
	 * @param obj pointer to the private structure.
	 * @param status pointer to struct where to set link information
	 *
	 * @return return 0 on success. otherwise on failure.
	 */
	int (*link_status_get)(struct al_mod_eth_mac_obj *obj,
				   struct al_mod_eth_link_status *status);

	/**
	 * clear link status
	 *
	 * this function clear latched status of the link.
	 *
	 * @param adapter pointer to the private structure.
	 *
	 * @return return 0 if supported.
	 */
	int (*link_status_clear)(struct al_mod_eth_mac_obj *);

	/**
	 * Set LEDs to represent link status.
	 *
	 * @param obj pointer to the private structure.
	 * @param link_is_up boolean indicating current link status.
	 *	  In case link is down the leds will be turned off.
	 *	  In case link is up the leds will be turned on, that means
	 *	  leds will be blinking on traffic and will be constantly lighting
	 *	  on inactive link
	 * @return return 0 on success. otherwise on failure.
	 */
	int (*led_set)(struct al_mod_eth_mac_obj *obj, al_mod_bool link_is_up);

	/**
	 * Enable/Disable Loopback mode
	 *
	 * @param adapter pointer to the private structure
	 * @param enable set to AL_TRUE to enable full duplex mode
	 *
	 * @return 0 on success. otherwise on failure.
	 */
	int (*loopback_config)(struct al_mod_eth_mac_obj *, al_mod_bool enable);

	/**
	 * Get Loopback mode
	 *
	 * @param adapter pointer to the private structure
	 *
	 * @return AL_TRUE if loopback is enabled and AL_FALSE otherwise
	 */
	al_mod_bool (*loopback_get)(struct al_mod_eth_mac_obj *);

	/**
	 * get mac statistics
	 * @param adapter pointer to the private structure.
	 * @param stats pointer to structure that will be filled with statistics.
	 *
	 * @return return 0 on success. otherwise on failure.
	 */
	int (*stats_get)(struct al_mod_eth_mac_obj *, struct al_mod_eth_mac_stats *);

	/**
	 * configure the MDIO hardware interface
	 * @param obj pointer to the private structure
	 * @param mdio_type clause type
	 * @param shared_mdio_if set to AL_TRUE if multiple controllers using the same
	 * @param ref_clk_freq reference clock frequency
	 * @param mdio_clk_freq_khz the required MDC/MDIO clock frequency [Khz]
	 * @param ignore_read_status if set to AL_TRUE, will accept READ response despite errors
	 * MDIO pins of the chip.
	 *
	 * @return 0 on success, otherwise on failure.
	 */
	int (*mdio_config)(struct al_mod_eth_mac_obj *, enum al_mod_eth_mdio_type mdio_type,
		al_mod_bool	shared_mdio_if, enum al_mod_eth_ref_clk_freq ref_clk_freq,
		unsigned int mdio_clk_freq_khz, al_mod_bool ignore_read_status);

	/**
	 * configure the clock polarity of the MDIO hardware
	 * @param obj pointer to the private structure
	 * @param mdio_pol polarity of the MDIO clock
	 *
	 * @return 0 on success, otherwise on failure.
	 */
	int (*mdio_pol_config)(struct al_mod_eth_mac_obj *, enum al_mod_eth_mdc_mdio_pol mdio_pol);

	/**
	 * read mdio register
	 * this function uses polling mode, and as the mdio is slow interface, it might
	 * block the cpu for long time (milliseconds).
	 * @param adapter pointer to the private structure
	 * @param phy_addr address of mdio phy
	 * @param device address of mdio device (used only in CLAUSE 45)
	 * @param reg index of the register
	 * @param val pointer for read value of the register
	 *
	 * @return 0 on success, negative errno on failure
	 */
	int (*mdio_read)(struct al_mod_eth_mac_obj *, uint32_t phy_addr, uint32_t device,
		uint32_t reg, uint16_t *val);

	/**
	 * write mdio register
	 * this function uses polling mode, and as the mdio is slow interface, it might
	 * block the cpu for long time (milliseconds).
	 * @param adapter pointer to the private structure
	 * @param phy_addr address of mdio phy
	 * @param device address of mdio device (used only in CLAUSE 45)
	 * @param reg index of the register
	 * @param val value to write
	 *
	 * @return 0 on success, negative errno on failure
	 */
	int (*mdio_write)(struct al_mod_eth_mac_obj *, uint32_t phy_addr, uint32_t device,
		uint32_t reg, uint16_t val);

	/**
	 * Enable / Disable forward error correction (FEC)
	 *
	 * @param adapter pointer to the private structure.
	 * @param type FEC type
	 * @param enable true to enable FEC. false to disable FEC.
	 *
	 * @return 0 on success. negative error on failure.
	 */
	int (*fec_enable)(struct al_mod_eth_mac_obj *, enum al_mod_eth_fec_type, al_mod_bool enable);

	/*
	 * Check if forward error correction (FEC) is enabled
	 *
	 * @param adapter pointer to the private structure.
	 *
	 * @return AL_TRUE if FEC is enabled and AL_FALSE otherwise
	 */
	al_mod_bool (*fec_is_enabled)(struct al_mod_eth_mac_obj *);

	/**
	 * Get forward error correction (FEC) statistics
	 *
	 * @param adapter pointer to the private structure.
	 * @param corrected number of bits been corrected by the FEC
	 * @param uncorrectable number of bits that FEC couldn't correct.
	 *
	 * @return 0 on success. negative error on failure.
	 */
	int (*fec_stats_get)(struct al_mod_eth_mac_obj *,
		uint32_t *corrected, uint32_t *uncorrectable);

	/**
	 * Perform gearbox reset for tx lanes And/Or Rx lanes.
	 * applicable only when the controller is connected to srds25G.
	 * This reset should be performed after each operation that changes the clocks
	 * (such as serdes reset, mac stop, etc.)
	 *
	 * @param adapter pointer to the private structure.
	 * @param tx_reset assert and de-assert reset for tx lanes
	 * @param rx_reset assert and de-assert reset for rx lanes
	 *
	 * @return 0 on success. negative error on failure.
	 */
	int (*gearbox_reset)(struct al_mod_eth_mac_obj *,
		al_mod_bool tx_reset, al_mod_bool rx_reset);

	/**
	 * configure mac related parts of
	 * minimum and maximum rx packet length
	 *
	 * @param obj pointer to the private structure
	 * @param min_rx_len minimum rx packet length
	 * @param max_rx_len maximum rx packet length
	 * both length limits in bytes and it includes the MAC Layer header and FCS.
	 * @return 0 on success, otherwise on failure.
	 */
	int (*rx_pkt_limit_config)(struct al_mod_eth_mac_obj *obj, unsigned int min_rx_len,
		unsigned int max_rx_len);

	/**
	 * configure mac related parts of EEE mode
	 * @param obj pointer to the private structure.
	 * @param params pointer to the eee input parameters.
	 *
	 * @return return 0 on success. otherwise on failure.
	 */
	int (*eee_config)(struct al_mod_eth_mac_obj *obj,
		struct al_mod_eth_eee_params *params);

	/**
	 * read from mac scratch register inside mac register file
	 * @param obj pointer to the private structure.
	 * @param idx scratch register index.
	 * @param val pointer to read into.
	 *
	 * @return return 0 on success. otherwise on failure.
	 */
	int (*scratch_read)(struct al_mod_eth_mac_obj *obj, unsigned int idx,
		uint32_t *val);

	/**
	 * write to mac scratch register inside mac register file
	 * @param obj pointer to the private structure.
	 * @param idx scratch register index.
	 * @param val value to write.
	 *
	 * @return return 0 on success. otherwise on failure.
	 */
	int (*scratch_write)(struct al_mod_eth_mac_obj *obj, unsigned int idx,
		uint32_t val);

	/**
	 * Configure flow control for MAC
	 * for (Ethernet Rev < 4) it is implemented in al_mod_hal_eth_main.c and
	 * this struct should not be initialized.
	 *
	 * @param obj pointer to the private structure.
	 * @param params pointer to the flow control configuration parameters
	 *
	 * @return return 0 on success. otherwise on failure.
	 */
	int (*flow_control_config_full)(struct al_mod_eth_mac_obj *obj,
		struct al_mod_eth_flow_control_params *params);

	/**
	 * Enable/Disable Tx flush
	 *
	 * @param obj pointer to the private structure.
	 * @param enable set to AL_TRUE to enable tx_flush.
	 *		 In this mode the MAC will pull packets from the FIFO and drop them.
	 *
	 * @return 0 on success. otherwise on failure.
	 */
	int (*tx_flush_config)(struct al_mod_eth_mac_obj *obj, al_mod_bool enable);

	/**
	 * Initialize rate counter handle
	 *
	 * @param handle pointer to the private structure handle to initialize
	 * @param handle_init_params initialization parameters
	 */
	int (*rate_counter_handle_init)(struct al_mod_eth_mac_rate_counter_handle *handle,
			struct al_mod_eth_mac_rate_counter_handle_init_params *handle_init_params);

	/**
	 * Configure rate counter
	 *
	 * @param obj pointer to the private structure.
	 * @param conf the rate counter configuration
	 */
	int (*rate_counter_config)(const struct al_mod_eth_mac_rate_counter_handle *);

	/**
	 * Start rate counter
	 *
	 * @param obj pointer to the private structure.
	 * @param conf the rate counter configuration
	 */
	int (*rate_counter_start)(const struct al_mod_eth_mac_rate_counter_handle *);

	/**
	 * Stop rate counter
	 *
	 * @param obj pointer to the private structure.
	 * @param conf the rate counter configuration
	 */
	int (*rate_counter_stop)(const struct al_mod_eth_mac_rate_counter_handle *);

	/**
	 * Get rate counter values
	 *
	 * @param obj pointer to the private structure.
	 * @param conf the rate counter configuration
	 * @param bytes pointer to store value
	 * @param frames pointer to store value
	 */
	int (*rate_counter_get)(const struct al_mod_eth_mac_rate_counter_handle *handle,
		uint32_t *bytes, uint32_t *frames);

	/**
	 * Print rate counter values
	 *
	 * @param obj pointer to the private structure.
	 * @param conf the rate counter configuration
	 */
	int (*rate_counter_print)(const struct al_mod_eth_mac_rate_counter_handle *);

	/**
	 * Get IOFIC hardware compatible bits
	 *
	 * @param obj Ethernet MAC object
	 * @param mac_a_mask pointer to save group A mask to
	 * @param mac_b_mask pointer to save group B mask to
	 * @param mac_d_mask pointer to save group C mask to
	 * @param mac_d_mask pointer to save group D mask to
	 */
	void (*iofic_attrs_get_hw)(struct al_mod_eth_mac_obj *obj,
				   uint32_t *mac_a_mask,
				   uint32_t *mac_b_mask,
				   uint32_t *mac_c_mask,
				   uint32_t *mac_d_mask);

	/**
	 * Get IOFIC bits which are incompatible due to runtime configuration
	 *
	 * @param obj Ethernet MAC object
	 * @param mac_a_mask pointer to save group A mask to
	 * @param mac_b_mask pointer to save group B mask to
	 * @param mac_d_mask pointer to save group C mask to
	 * @param mac_d_mask pointer to save group D mask to
	 */
	void (*iofic_attrs_get_runtime_inval)(struct al_mod_eth_mac_obj *obj,
					      uint32_t *mac_a_mask,
					      uint32_t *mac_b_mask,
					      uint32_t *mac_c_mask,
					      uint32_t *mac_d_mask);

	/**
	 * Get IOFIC bits with unstable values during startup
	 *
	 * @param obj Ethernet MAC object
	 * @param mac_a_mask pointer to save group A mask to
	 * @param mac_b_mask pointer to save group B mask to
	 * @param mac_d_mask pointer to save group C mask to
	 * @param mac_d_mask pointer to save group D mask to
	 */
	void (*iofic_attrs_get_transient)(struct al_mod_eth_mac_obj *obj,
					  uint32_t *mac_a_mask,
					  uint32_t *mac_b_mask,
					  uint32_t *mac_c_mask,
					  uint32_t *mac_d_mask);

	/**
	 * Get IOFIC bits which are intended to be polled and not unmasked by
	 * default (non-fatal errors)
	 *
	 * @param obj Ethernet MAC object
	 * @param mac_a_mask pointer to save group A mask to
	 * @param mac_b_mask pointer to save group B mask to
	 * @param mac_d_mask pointer to save group C mask to
	 * @param mac_d_mask pointer to save group D mask to
	 */
	void (*iofic_attrs_get_non_fatal)(struct al_mod_eth_mac_obj *obj,
					  uint32_t *mac_a_mask,
					  uint32_t *mac_b_mask,
					  uint32_t *mac_c_mask,
					  uint32_t *mac_d_mask);

	/**
	 * Unmask MAC error interrupts
	 *
	 * @param obj pointer to the private structure
	 */
	void (*error_ints_unmask)(struct al_mod_eth_mac_obj *obj);

	/**
	 * start/stop the mac tx and/or rx paths. Enables configuring either TX or RX (or both),
	 * and for each of them, if to start or stop.
	 * @param adapter pointer to the private structure.
	 * @param tx modify tx path
	 * @param rx modify rx path
	 * @param tx_start true=start, false=stop
	 * @param rx_start true=start, false=stop
	 *
	 * @return 0 on success. negative error on failure.
	 *
	 * @note tx_start is valid only if tx is set to true. same goes for rx_start
	 */
	int (*mac_start_stop_adv)(struct al_mod_eth_mac_obj *obj,
					 al_mod_bool tx,
					 al_mod_bool rx,
					 al_mod_bool tx_start,
					 al_mod_bool rx_start);
};

/** Ethernet MAC handle initialization parameters */
struct al_mod_eth_mac_obj_init_params {
	/** MAC registers base address */
	void __iomem *mac_regs_base;
	/** MAC common registers base address */
	void __iomem *mac_common_regs;
	/** MAC common registers lock/unlock callback */
	int (*mac_common_regs_lock)(void *handler, al_mod_bool lock);
	/** MAC common registers lock/unlock handle */
	void __iomem *mac_common_regs_lock_handle;

	/** Ethernet revision ID */
	uint8_t eth_rev_id;

	/** Ethernet original revision ID */
	uint8_t eth_original_rev_id;

	/** SerDes lane */
	unsigned int serdes_lane;

	/**< the upper layer must keep the string area */
	char *name;

	/** DME (Differential manchester encoding) downsample */
	struct al_mod_hal_eth_an_lt_dme_downsample_conf *dme_conf;
};

/**
 * Ethernet MAC object handle initialization
 *
 * @param obj
 *	Ethernet MAC object
 * @param params
 *	Ethernet MAC object initialization parameters
 *
 * @return 0 on success, errno otherwise.
 */
int al_mod_eth_mac_handle_init(struct al_mod_eth_mac_obj *obj, struct al_mod_eth_mac_obj_init_params *params);

/**
 * Ethernet MAC mode string representation
 *
 * @param mode
 *	Ethernet MAC mode
 *
 * @return string representation of the mode on success, "N/A" otherwise.
 */
const char *al_mod_eth_mac_mode_str(enum al_mod_eth_mac_mode mode);

/**
 * Ethernet MAC FEC type string representation
 *
 * @param type
 *	Ethernet MAC FEC type
 *
 * @return string representation of the mode on success, "N/A" otherwise.
 */
const char *al_mod_eth_fec_type_str(enum al_mod_eth_fec_type type);

/**
 * Ethernet MDIO type string representation
 *
 * @param type_name
 *	Ethernet MDIO type
 *
 * @return string representation of the mode on success, "N/A" otherwise.
 */
const char *al_mod_eth_mdio_type_str(enum al_mod_eth_mdio_type type_name);

/**
 * Ethernet MAC object get iofic pointer
 *
 * @param obj
 *	Ethernet MAC object
 *
 * @return pointer to iofic regs
 */
void __iomem *al_mod_eth_mac_iofic_regs_get(
	struct al_mod_eth_mac_obj *obj);

/** Helper functions to call above object API */
int al_mod_eth_mac_mode_set(struct al_mod_hal_eth_adapter *adapter, enum al_mod_eth_mac_mode mode);
int al_mod_eth_mac_config(struct al_mod_hal_eth_adapter *adapter, enum al_mod_eth_mac_mode mode);
int al_mod_eth_mac_start(struct al_mod_hal_eth_adapter *adapter);
int al_mod_eth_mac_stop(struct al_mod_hal_eth_adapter *adapter);
int al_mod_eth_mac_start_stop_adv(struct al_mod_hal_eth_adapter *adapter,
			      al_mod_bool tx,
			      al_mod_bool rx,
			      al_mod_bool tx_start,
			      al_mod_bool rx_start);
int al_mod_eth_mac_link_config(struct al_mod_hal_eth_adapter *adapter,
	al_mod_bool force_1000_base_x, al_mod_bool an_enable,
	uint32_t speed, al_mod_bool full_duplex);
int al_mod_eth_link_status_get(struct al_mod_hal_eth_adapter *adapter,
	struct al_mod_eth_link_status *status);
int al_mod_eth_link_status_clear(struct al_mod_hal_eth_adapter *adapter);
int al_mod_eth_led_set(struct al_mod_hal_eth_adapter *adapter, al_mod_bool link_is_up);
int al_mod_eth_mac_loopback_config(struct al_mod_hal_eth_adapter *adapter, al_mod_bool enable);
int al_mod_eth_mac_stats_get(struct al_mod_hal_eth_adapter *adapter, struct al_mod_eth_mac_stats *stats);

int al_mod_eth_mdio_config(struct al_mod_hal_eth_adapter *adapter, enum al_mod_eth_mdio_type mdio_type,
	al_mod_bool	shared_mdio_if, enum al_mod_eth_ref_clk_freq ref_clk_freq,
	unsigned int mdio_clk_freq_khz);
#define AL_ETH_MAC_HAS_MDIO_CONFIG_EX
int al_mod_eth_mdio_config_ex(struct al_mod_hal_eth_adapter *adapter,
	struct al_mod_eth_mdio_config_params *params);
int al_mod_eth_mdio_pol_config(struct al_mod_hal_eth_adapter *adapter, enum al_mod_eth_mdc_mdio_pol mdio_pol);
int al_mod_eth_mdio_read(struct al_mod_hal_eth_adapter *adapter, uint32_t phy_addr, uint32_t device,
	uint32_t reg, uint16_t *val);
int al_mod_eth_mdio_write(struct al_mod_hal_eth_adapter *adapter, uint32_t phy_addr, uint32_t device,
	uint32_t reg, uint16_t val);

int al_mod_eth_fec_enable(struct al_mod_hal_eth_adapter *adapter,
	enum al_mod_eth_fec_type type, al_mod_bool enable);
al_mod_bool al_mod_eth_fec_is_enabled(struct al_mod_hal_eth_adapter *adapter);
int al_mod_eth_fec_stats_get(struct al_mod_hal_eth_adapter *adapter,
	uint32_t *corrected, uint32_t *uncorrectable);
int al_mod_eth_gearbox_reset(struct al_mod_hal_eth_adapter *adapter,
	al_mod_bool tx_reset, al_mod_bool rx_reset);
int al_mod_eth_mac_tx_flush_config(struct al_mod_hal_eth_adapter *adapter, al_mod_bool enable);

/** MAC rate counters */
int al_mod_eth_mac_rate_counter_handle_init(struct al_mod_hal_eth_adapter *adapter,
	struct al_mod_eth_mac_rate_counter_handle *handle,
	struct al_mod_eth_mac_rate_counter_handle_init_params *handle_init_params);
int al_mod_eth_mac_rate_counter_config(struct al_mod_eth_mac_rate_counter_handle *handle);
int al_mod_eth_mac_rate_counter_start(struct al_mod_eth_mac_rate_counter_handle *handle);
int al_mod_eth_mac_rate_counter_stop(struct al_mod_eth_mac_rate_counter_handle *handle);
int al_mod_eth_mac_rate_counter_get(struct al_mod_eth_mac_rate_counter_handle *handle,
	uint32_t *bytes, uint32_t *frames);
int al_mod_eth_mac_rate_counter_print(struct al_mod_eth_mac_rate_counter_handle *handle);

/**
 * Get MAC iofic attributes
 * Function will return iofic masks according to given flags
 *
 * @param handle Ethernet MAC object
 * @param flags bit field of AL_ETH_IOFIC_ATTRS_ values
 * @param mac_a_mask pointer to MAC group A mask
 * @param mac_b_mask pointer to MAC group B mask
 * @param mac_c_mask pointer to MAC group C mask
 * @param mac_d_mask pointer to MAC group D mask
 *
 */
int al_mod_eth_mac_iofic_attrs_get(struct al_mod_eth_mac_obj *handle, int flags,
			       uint32_t *mac_a_mask, uint32_t *mac_b_mask,
			       uint32_t *mac_c_mask, uint32_t *mac_d_mask);

#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */

#endif /* __AL_HAL_ETH_MAC_H__ */
/** @} end of Ethernet group */
