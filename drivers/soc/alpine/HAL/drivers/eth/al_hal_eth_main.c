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
 * @file   al_hal_eth_main.c
 *
 * @brief  XG Ethernet unit HAL driver for main functions (initialization, data path)
 *
 */

#include "al_hal_eth.h"
#include "al_hal_eth_desc.h"
#include "al_hal_udma_iofic.h"
#include "al_hal_udma_config.h"
#include "al_hal_eth_ec_regs.h"
#include "al_hal_eth_mac_regs.h"
#include "al_hal_eth_rmp_regs.h"
#include "al_hal_unit_adapter_regs.h"
#include "al_hal_eth_crc_tables.h"
#if defined(AL_ETH_EX) && defined(AL_DEV_ID)
#if (AL_DEV_ID == AL_DEV_ID_ALPINE_V2)
#include "al_hal_eth_ex_internal_v3.h"
#endif
#if (AL_DEV_ID == AL_DEV_ID_ALPINE_V3)
#include "al_hal_eth_ex_internal_v4.h"
#endif
#endif
#include "al_hal_unit_adapter.h"

#define AL_ETH_V4_IS_STD(rev, dev) \
	(((rev) == AL_ETH_REV_ID_4) && ((dev) == AL_ETH_DEV_ID_STANDARD))

#define AL_ETH_V4_IS_ADV(rev, dev) \
	(((rev) == AL_ETH_REV_ID_4) && ((dev) == AL_ETH_DEV_ID_ADVANCED))

/* Used to check if common mode is relevant for a specific adapter set of rev, dev ID */
#define AL_ETH_ADAPTER_NEED_COMMON_MODE(dev, rev) \
	(((rev) >= AL_ETH_REV_ID_4) && ((dev) == AL_ETH_DEV_ID_ADVANCED))

/* EC error interrupts */
#define AL_ETH_EC_IOFIC_GROUP_A_ERROR_INTS \
	(AL_ETH_EC_IOFIC_GROUP_A_ERR_MSW_IN_SOP_IN_PKT | \
	 AL_ETH_EC_IOFIC_GROUP_A_ERR_MSW_IN_EOP_IN_IDLE | \
	 AL_ETH_EC_IOFIC_GROUP_A_ERR_FWD_OUT_EOP_META_EMPTY | \
	 AL_ETH_EC_IOFIC_GROUP_A_ERR_FWD_IN_SOP_IN_PKT | \
	 AL_ETH_EC_IOFIC_GROUP_A_ERR_FWD_IN_EOP_IN_IDLE | \
	 AL_ETH_EC_IOFIC_GROUP_A_ERR_RPE_3_OUT_EOP_HDR_EMPTY | \
	 AL_ETH_EC_IOFIC_GROUP_A_ERR_RPE_3_IN_SOP_IN_PKT | \
	 AL_ETH_EC_IOFIC_GROUP_A_ERR_RPE_3_IN_EOP_IN_IDLE | \
	 AL_ETH_EC_IOFIC_GROUP_A_ERR_RPE_2_OUT_EOP_HDR_EMPTY | \
	 AL_ETH_EC_IOFIC_GROUP_A_ERR_RPE_2_IN_SOP_IN_PKT | \
	 AL_ETH_EC_IOFIC_GROUP_A_ERR_RPE_2_IN_EOP_IN_IDLE | \
	 AL_ETH_EC_IOFIC_GROUP_A_ERR_RPE_1_OUT_EOP_HDR_EMPTY | \
	 AL_ETH_EC_IOFIC_GROUP_A_ERR_RPE_1_IN_SOP_IN_PKT | \
	 AL_ETH_EC_IOFIC_GROUP_A_ERR_RPE_1_IN_EOP_IN_IDLE | \
	 AL_ETH_EC_IOFIC_GROUP_A_ERR_RXF_OUT_EOP_HDR_EMPTY | \
	 AL_ETH_EC_IOFIC_GROUP_A_ERR_LBF_IN_SOP_IN_PKT | \
	 AL_ETH_EC_IOFIC_GROUP_A_ERR_LBF_IN_EOP_IN_IDLE | \
	 AL_ETH_EC_IOFIC_GROUP_A_ERR_RXF_IN_SOP_IN_PKT | \
	 AL_ETH_EC_IOFIC_GROUP_A_ERR_RXF_IN_EOP_IN_IDLE | \
	 AL_ETH_EC_IOFIC_GROUP_A_ERR_FAF_2_OUT_EOP_HDR_EMPTY | \
	 AL_ETH_EC_IOFIC_GROUP_A_ERR_FAF_2_IN_SOP_IN_PKT | \
	 AL_ETH_EC_IOFIC_GROUP_A_ERR_FAF_2_IN_EOP_IN_IDLE | \
	 AL_ETH_EC_IOFIC_GROUP_A_ERR_FAF_1_OUT_EOP_HDR_EMPTY | \
	 AL_ETH_EC_IOFIC_GROUP_A_ERR_FAF_1_IN_SOP_IN_PKT | \
	 AL_ETH_EC_IOFIC_GROUP_A_ERR_FAF_1_IN_EOP_IN_IDLE)

#define AL_ETH_EC_IOFIC_GROUP_B_ERROR_INTS \
	(AL_ETH_EC_IOFIC_GROUP_B_ERR_TMI_IN_SOP_IN_PKT | \
	 AL_ETH_EC_IOFIC_GROUP_B_ERR_TMI_IN_EOP_IN_IDLE | \
	 AL_ETH_EC_IOFIC_GROUP_B_ERR_TFW_OUT_EOP_HDR_EMPTY | \
	 AL_ETH_EC_IOFIC_GROUP_B_ERR_TFW_IN_SOP_IN_PKT | \
	 AL_ETH_EC_IOFIC_GROUP_B_ERR_TFW_IN_EOP_IN_IDLE | \
	 AL_ETH_EC_IOFIC_GROUP_B_ERR_TPM_NO_SOP | \
	 AL_ETH_EC_IOFIC_GROUP_B_ERR_TPM_SPOOF_0 | \
	 AL_ETH_EC_IOFIC_GROUP_B_ERR_TPM_SPOOF_1 | \
	 AL_ETH_EC_IOFIC_GROUP_B_ERR_TPM_SPOOF_2 | \
	 AL_ETH_EC_IOFIC_GROUP_B_ERR_TPM_SPOOF_3 | \
	 AL_ETH_EC_IOFIC_GROUP_B_ERR_TPE_OUT_EOP_HDR_EMPTY | \
	 AL_ETH_EC_IOFIC_GROUP_B_ERR_TPE_IN_SOP_IN_PKT | \
	 AL_ETH_EC_IOFIC_GROUP_B_ERR_TPE_IN_EOP_IN_IDLE | \
	 AL_ETH_EC_IOFIC_GROUP_B_ERR_TSO_CNT_END_OF_SEQ | \
	 AL_ETH_EC_IOFIC_GROUP_B_ERR_TSO_LAST_IN_SHDR | \
	 AL_ETH_EC_IOFIC_GROUP_B_ERR_TSO_CNT_END_OF_PKT_4 | \
	 AL_ETH_EC_IOFIC_GROUP_B_ERR_TSO_CNT_END_OF_PKT_3 | \
	 AL_ETH_EC_IOFIC_GROUP_B_ERR_TSO_CNT_END_OF_PKT_2 | \
	 AL_ETH_EC_IOFIC_GROUP_B_ERR_TSO_CNT_END_OF_PKT_1 | \
	 AL_ETH_EC_IOFIC_GROUP_B_ERR_TSO_NO_SOP)

#define AL_ETH_V1_EC_IOFIC_GROUP_B_ERROR_INTS \
	(AL_ETH_EC_IOFIC_GROUP_B_ERROR_INTS | \
	 AL_ETH_EC_IOFIC_GROUP_B_ERR_RX_SAD_3_PARITY | \
	 AL_ETH_EC_IOFIC_GROUP_B_ERR_RX_SAD_2_PARITY | \
	 AL_ETH_EC_IOFIC_GROUP_B_ERR_RX_SAD_1_PARITY | \
	 AL_ETH_EC_IOFIC_GROUP_B_ERR_RX_SAD_0_PARITY | \
	 AL_ETH_EC_IOFIC_GROUP_B_ERR_TX_SAD_3_PARITY | \
	 AL_ETH_EC_IOFIC_GROUP_B_ERR_TX_SAD_2_PARITY | \
	 AL_ETH_EC_IOFIC_GROUP_B_ERR_TX_SAD_1_PARITY | \
	 AL_ETH_EC_IOFIC_GROUP_B_ERR_TX_SAD_0_PARITY)

#define AL_ETH_V2_V3_EC_IOFIC_GROUP_B_ERROR_INTS \
	(AL_ETH_EC_IOFIC_GROUP_B_ERROR_INTS)

#define AL_ETH_EC_IOFIC_GROUP_C_ERROR_INTS \
	(AL_ETH_EC_IOFIC_GROUP_C_TFW_EOP_FIFO_1_PARITY | \
	 AL_ETH_EC_IOFIC_GROUP_C_TFW_SOP_FIFO_1_PARITY | \
	 AL_ETH_EC_IOFIC_GROUP_C_RXF_EOP_FIFO_2_PARITY | \
	 AL_ETH_EC_IOFIC_GROUP_C_RXF_SOP_FIFO_2_PARITY | \
	 AL_ETH_EC_IOFIC_GROUP_C_RXF_EOP_FIFO_1_PARITY | \
	 AL_ETH_EC_IOFIC_GROUP_C_RXF_SOP_FIFO_1_PARITY | \
	 AL_ETH_EC_IOFIC_GROUP_C_TFW_VLAN_TABLE_1_PARITY | \
	 AL_ETH_EC_IOFIC_GROUP_C_RFW_CTRL_TABLE_1_PARITY | \
	 AL_ETH_EC_IOFIC_GROUP_C_RFW_VLAN_TABLE_1_PARITY | \
	 AL_ETH_EC_IOFIC_GROUP_C_RFW_THASH_TABLE_1_PARITY | \
	 AL_ETH_EC_IOFIC_GROUP_C_RFW_MHASH_TABLE_1_PARITY | \
	 AL_ETH_EC_IOFIC_GROUP_C_TFW_DATA_FIFO_2_PARITY | \
	 AL_ETH_EC_IOFIC_GROUP_C_TFW_DATA_FIFO_1_PARITY | \
	 AL_ETH_EC_IOFIC_GROUP_C_RXF_DATA_FIFO_2_2_PARITY | \
	 AL_ETH_EC_IOFIC_GROUP_C_RXF_DATA_FIFO_2_1_PARITY | \
	 AL_ETH_EC_IOFIC_GROUP_C_RXF_DATA_FIFO_1_2_PARITY | \
	 AL_ETH_EC_IOFIC_GROUP_C_RXF_DATA_FIFO_1_1_PARITY | \
	 AL_ETH_EC_IOFIC_GROUP_C_TSO_META_CACHE_1_4_PARITY | \
	 AL_ETH_EC_IOFIC_GROUP_C_TSO_META_CACHE_1_3_PARITY | \
	 AL_ETH_EC_IOFIC_GROUP_C_TSO_META_CACHE_1_2_PARITY | \
	 AL_ETH_EC_IOFIC_GROUP_C_TSO_META_CACHE_1_1_PARITY)

#define AL_ETH_V1_V2_EC_IOFIC_GROUP_C_ERROR_INTS \
	(AL_ETH_EC_IOFIC_GROUP_C_ERROR_INTS | \
	 AL_ETH_EC_IOFIC_GROUP_C_RFW_FLOW_TABLE_1_PARITY)

#define AL_ETH_V3_EC_IOFIC_GROUP_C_ERROR_INTS \
	(AL_ETH_EC_IOFIC_GROUP_C_ERROR_INTS | \
	 AL_ETH_V3_EC_IOFIC_GROUP_C_TFW_GCP_TABLE_2_PARITY | \
	 AL_ETH_V3_EC_IOFIC_GROUP_C_TFW_GCP_TABLE_1_PARITY | \
	 AL_ETH_V3_EC_IOFIC_GROUP_C_TFW_GCP_TABLE_0_PARITY | \
	 AL_ETH_V3_EC_IOFIC_GROUP_C_RFW_GCP_TABLE_2_PARITY | \
	 AL_ETH_V3_EC_IOFIC_GROUP_C_RFW_GCP_TABLE_1_PARITY | \
	 AL_ETH_V3_EC_IOFIC_GROUP_C_RFW_GCP_TABLE_0_PARITY)

#define AL_ETH_V1_EC_IOFIC_GROUP_D_ERROR_INTS \
	(AL_ETH_EC_IOFIC_GROUP_D_ERR_TIMESTAMP_FIFO_OVERRUN | \
	 AL_ETH_EC_IOFIC_GROUP_D_ERR_ROCE_WR_INT | \
	 AL_ETH_EC_IOFIC_GROUP_D_ERR_ROCE_RR_INT)

#define AL_ETH_V2_V3_EC_IOFIC_GROUP_D_ERROR_INTS \
	(AL_ETH_EC_IOFIC_GROUP_D_ERR_TIMESTAMP_FIFO_OVERRUN)

/** ETH v4 IOFICS */
#define AL_ETH_V4_EC_IOFIC_GROUP_A_SUM \
	(AL_ETH_V4_EC_IOFIC_GROUP_A_EC_ERR_MSW_IN_SOP_IN_PKT | \
	AL_ETH_V4_EC_IOFIC_GROUP_A_EC_ERR_MSW_IN_EOP_IN_IDLE | \
	AL_ETH_V4_EC_IOFIC_GROUP_A_EC_ERR_FWD_OUT_EOP_META_EMPTY | \
	AL_ETH_V4_EC_IOFIC_GROUP_A_EC_ERR_FWD_IN_SOP_IN_PKT | \
	AL_ETH_V4_EC_IOFIC_GROUP_A_EC_ERR_FWD_IN_EOP_IN_IDLE | \
	AL_ETH_V4_EC_IOFIC_GROUP_A_RMP_INT | \
	AL_ETH_V4_EC_IOFIC_GROUP_A_EC_RFW_PKT_HDR_FIFO_MEM_PAR_ERR_0 | \
	AL_ETH_V4_EC_IOFIC_GROUP_A_EC_RFW_PKT_HDR_FIFO_MEM_PAR_ERR_1 | \
	AL_ETH_V4_EC_IOFIC_GROUP_A_EC_RFW_META_FIFO_MEM_PAR_ERR_0 | \
	AL_ETH_V4_EC_IOFIC_GROUP_A_EC_RFW_META_FIFO_MEM_PAR_ERR_1 | \
	AL_ETH_V4_EC_IOFIC_GROUP_A_EC_RFW_META_FIFO_MEM_PAR_ERR_2 | \
	AL_ETH_V4_EC_IOFIC_GROUP_A_EC_RFW_DATA_FIFO_MEM_PAR_ERR_0 | \
	AL_ETH_V4_EC_IOFIC_GROUP_A_EC_RFW_DATA_FIFO_MEM_PAR_ERR_1 | \
	AL_ETH_V4_EC_IOFIC_GROUP_A_EC_ERR_RXF_OUT_EOP_HDR_EMPTY | \
	AL_ETH_V4_EC_IOFIC_GROUP_A_EC_ERR_LBF_IN_SOP_IN_PKT | \
	AL_ETH_V4_EC_IOFIC_GROUP_A_EC_ERR_LBF_IN_EOP_IN_IDLE | \
	AL_ETH_V4_EC_IOFIC_GROUP_A_EC_ERR_RXF_IN_SOP_IN_PKT | \
	AL_ETH_V4_EC_IOFIC_GROUP_A_EC_ERR_RXF_IN_EOP_IN_IDLE | \
	AL_ETH_V4_EC_IOFIC_GROUP_A_EC_ERR_FAF_2_OUT_EOP_HDR_EMPTY | \
	AL_ETH_V4_EC_IOFIC_GROUP_A_EC_ERR_FAF_2_IN_SOP_IN_PKT | \
	AL_ETH_V4_EC_IOFIC_GROUP_A_EC_ERR_FAF_2_IN_EOP_IN_IDLE | \
	AL_ETH_V4_EC_IOFIC_GROUP_A_EC_ERR_FAF_1_OUT_EOP_HDR_EMPTY | \
	AL_ETH_V4_EC_IOFIC_GROUP_A_EC_ERR_FAF_1_IN_SOP_IN_PKT | \
	AL_ETH_V4_EC_IOFIC_GROUP_A_EC_ERR_FAF_1_IN_EOP_IN_IDLE)

#define AL_ETH_V4_EC_IOFIC_GROUP_B_SUM \
	(AL_ETH_V4_EC_IOFIC_GROUP_B_EC_ERR_TMI_IN_SOP_IN_PKT | \
	AL_ETH_V4_EC_IOFIC_GROUP_B_EC_ERR_TMI_IN_EOP_IN_IDLE | \
	AL_ETH_V4_EC_IOFIC_GROUP_B_EC_ERR_TFW_OUT_EOP_HDR_EMPTY | \
	AL_ETH_V4_EC_IOFIC_GROUP_B_EC_ERR_TFW_IN_SOP_IN_PKT | \
	AL_ETH_V4_EC_IOFIC_GROUP_B_EC_ERR_TFW_IN_EOP_IN_IDLE | \
	AL_ETH_V4_EC_IOFIC_GROUP_B_TFW_OUT_S_ERR | \
	AL_ETH_V4_EC_IOFIC_GROUP_B_EC_ERR_TPM_NO_SOP_ERR | \
	AL_ETH_V4_EC_IOFIC_GROUP_B_EC_ERR_TPM_SPOOF_ERR_0 | \
	AL_ETH_V4_EC_IOFIC_GROUP_B_EC_ERR_TPM_SPOOF_ERR_1 | \
	AL_ETH_V4_EC_IOFIC_GROUP_B_EC_ERR_TPM_SPOOF_ERR_2 | \
	AL_ETH_V4_EC_IOFIC_GROUP_B_EC_ERR_TPM_SPOOF_ERR_3 | \
	AL_ETH_V4_EC_IOFIC_GROUP_B_EC_ERR_TPE_OUT_EOP_HDR_EMPTY | \
	AL_ETH_V4_EC_IOFIC_GROUP_B_EC_ERR_TPE_IN_SOP_IN_PKT | \
	AL_ETH_V4_EC_IOFIC_GROUP_B_EC_ERR_TPE_IN_EOP_IN_IDLE | \
	AL_ETH_V4_EC_IOFIC_GROUP_B_EC_ERR_TSO_CNT_END_OF_SEG | \
	AL_ETH_V4_EC_IOFIC_GROUP_B_EC_ERR_TSO_LAST_IN_SHDR | \
	AL_ETH_V4_EC_IOFIC_GROUP_B_EC_ERR_TSO_CNT_END_OF_PKT_4 | \
	AL_ETH_V4_EC_IOFIC_GROUP_B_EC_ERR_TSO_CNT_END_OF_PKT_3 | \
	AL_ETH_V4_EC_IOFIC_GROUP_B_EC_ERR_TSO_CNT_END_OF_PKT_2 | \
	AL_ETH_V4_EC_IOFIC_GROUP_B_EC_ERR_TSO_CNT_END_OF_PKT_1 | \
	AL_ETH_V4_EC_IOFIC_GROUP_B_EC_ERR_TSO_NO_SOP_ERR | \
	AL_ETH_V4_EC_IOFIC_GROUP_B_EC_HS_CTRL_TABLE_PAR_ERR_3 | \
	AL_ETH_V4_EC_IOFIC_GROUP_B_EC_HS_CTRL_TABLE_PAR_ERR_2 | \
	AL_ETH_V4_EC_IOFIC_GROUP_B_EC_HS_CTRL_TABLE_PAR_ERR_1 | \
	AL_ETH_V4_EC_IOFIC_GROUP_B_EC_HS_CTRL_TABLE_PAR_ERR_0 | \
	AL_ETH_V4_EC_IOFIC_GROUP_B_EC_RFW_IN_PIPE_DATA_FIFO_PAR_ERR_0 | \
	AL_ETH_V4_EC_IOFIC_GROUP_B_EC_RFW_IN_PIPE_DATA_FIFO_PAR_ERR_1 | \
	AL_ETH_V4_EC_IOFIC_GROUP_B_EC_ERR_TFW_OUT_SOP_INFO_EMPTY)

#define AL_ETH_V4_EC_IOFIC_GROUP_C_SUM \
	(AL_ETH_V4_EC_IOFIC_GROUP_C_EC_SEC_INT_OUTPUT_0 | \
	AL_ETH_V4_EC_IOFIC_GROUP_C_EC_SEC_INT_OUTPUT_1 | \
	AL_ETH_V4_EC_IOFIC_GROUP_C_EC_TFW_EOP_FIFO_1_PAR_ERR | \
	AL_ETH_V4_EC_IOFIC_GROUP_C_EC_TFW_SOP_FIFO_1_PAR_ERR | \
	AL_ETH_V4_EC_IOFIC_GROUP_C_EC_TFW_GCP_TABLE_LO_PAR_ERR_3 | \
	AL_ETH_V4_EC_IOFIC_GROUP_C_EC_RFW_GCP_TABLE_LO_PAR_ERR_3 | \
	AL_ETH_V4_EC_IOFIC_GROUP_C_EC_TFW_VLAN_TABLE_1_PAR_ERR | \
	AL_ETH_V4_EC_IOFIC_GROUP_C_EC_RFW_CTRL_TABLE_1_PAR_ERR | \
	AL_ETH_V4_EC_IOFIC_GROUP_C_EC_RFW_VLAN_TABLE_1_PAR_ERR | \
	AL_ETH_V4_EC_IOFIC_GROUP_C_EC_RFW_FLOW_TABLE_1_PAR_ERR | \
	AL_ETH_V4_EC_IOFIC_GROUP_C_EC_RFW_THASH_TABLE_1_PAR_ERR | \
	AL_ETH_V4_EC_IOFIC_GROUP_C_EC_RFW_MHASH_TABLE_1_PAR_ERR | \
	AL_ETH_V4_EC_IOFIC_GROUP_C_EC_TFW_GCP_TABLE_LO_PAR_ERR_2 | \
	AL_ETH_V4_EC_IOFIC_GROUP_C_EC_TFW_GCP_TABLE_LO_PAR_ERR_1 | \
	AL_ETH_V4_EC_IOFIC_GROUP_C_EC_TFW_GCP_TABLE_LO_PAR_ERR_0 | \
	AL_ETH_V4_EC_IOFIC_GROUP_C_EC_RFW_GCP_TABLE_LO_PAR_ERR_2 | \
	AL_ETH_V4_EC_IOFIC_GROUP_C_EC_RFW_GCP_TABLE_LO_PAR_ERR_1 | \
	AL_ETH_V4_EC_IOFIC_GROUP_C_EC_RFW_GCP_TABLE_LO_PAR_ERR_0 | \
	AL_ETH_V4_EC_IOFIC_GROUP_C_EC_TFW_DATA_FIFO_2_PAR_ERR_0 | \
	AL_ETH_V4_EC_IOFIC_GROUP_C_EC_TFW_DATA_FIFO_1_PAR_ERR_0 | \
	AL_ETH_V4_EC_IOFIC_GROUP_C_EC_TFW_DATA_FIFO_2_PAR_ERR_1 | \
	AL_ETH_V4_EC_IOFIC_GROUP_C_EC_TFW_DATA_FIFO_1_PAR_ERR_1 | \
	AL_ETH_V4_EC_IOFIC_GROUP_C_EC_TSO_META_CACHE_1_4_PAR_ERR | \
	AL_ETH_V4_EC_IOFIC_GROUP_C_EC_TSO_META_CACHE_1_3_PAR_ERR | \
	AL_ETH_V4_EC_IOFIC_GROUP_C_EC_TSO_META_CACHE_1_2_PAR_ERR | \
	AL_ETH_V4_EC_IOFIC_GROUP_C_EC_TSO_META_CACHE_1_1_PAR_ERR)

#define AL_ETH_V4_EC_IOFIC_GROUP_D_SUM \
	(AL_ETH_V4_EC_IOFIC_GROUP_D_ERR_TIMESTAMP_FIFO_OVERRUN)

/** RMN 10760 */
#define AL_ETH_EC_IOFIC_GROUP_A_RMN_10760_INTS \
	(AL_ETH_V4_EC_IOFIC_GROUP_A_EC_ERR_RXF_OUT_EOP_HDR_EMPTY | \
	AL_ETH_V4_EC_IOFIC_GROUP_A_EC_ERR_FWD_OUT_EOP_META_EMPTY | \
	AL_ETH_V4_EC_IOFIC_GROUP_A_EC_ERR_MSW_IN_EOP_IN_IDLE | \
	AL_ETH_V4_EC_IOFIC_GROUP_A_EC_ERR_MSW_IN_SOP_IN_PKT)

#define AL_ETH_EC_IOFIC_GROUP_B_RMN_10760_INTS \
	AL_ETH_V4_EC_IOFIC_GROUP_B_EC_ERR_TFW_OUT_EOP_HDR_EMPTY

/** ETH v4 secondary EC */
#define AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_SUM_TX \
	(AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_TFW_GCP_TABLE_HI_PAR_ERR_0 | \
	AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_TFW_GCP_TABLE_HI_PAR_ERR_1 | \
	AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_TFW_GCP_TABLE_HI_PAR_ERR_2 | \
	AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_TFW_GCP_TABLE_HI_PAR_ERR_3 | \
	AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_TFW_GCP_TABLE_HI_PAR_ERR_4 | \
	AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_TFW_GCP_TABLE_HI_PAR_ERR_5 | \
	AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_TFW_GCP_TABLE_HI_PAR_ERR_6 | \
	AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_TFW_GCP_TABLE_HI_PAR_ERR_7)

#define AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_SUM_RX \
	(AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_RFW_GCP_TABLE_HI_PAR_ERR_0 | \
	AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_RFW_GCP_TABLE_HI_PAR_ERR_1 | \
	AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_RFW_GCP_TABLE_HI_PAR_ERR_2 | \
	AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_RFW_GCP_TABLE_HI_PAR_ERR_3 | \
	AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_RFW_GCP_TABLE_HI_PAR_ERR_4 | \
	AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_RFW_GCP_TABLE_HI_PAR_ERR_5 | \
	AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_RFW_GCP_TABLE_HI_PAR_ERR_6 | \
	AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_RFW_GCP_TABLE_HI_PAR_ERR_7 | \
	AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_RFF_DATA_FIFO_LO_MEM_PAR_ERR | \
	AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_RFF_DATA_FIFO_HI_MEM_PAR_ERR | \
	AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_RFF_RFW_SR_RESULT_FIFO_MEM_PAR_ERR_0 | \
	AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_RFF_RFW_SR_RESULT_FIFO_MEM_PAR_ERR_1 | \
	AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_RFF_RFW_SR_RESULT_FIFO_MEM_PAR_ERR_2 | \
	AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_RFF_RFW_SR_RESULT_FIFO_MEM_PAR_ERR_3 | \
	AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_RFF_PARSE_RES_FIFO_MEM_PAR_ERR_0 | \
	AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_RFF_PARSE_RES_FIFO_MEM_PAR_ERR_1 | \
	AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_RFF_PARSE_RES_FIFO_MEM_PAR_ERR_2 | \
	AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_RFF_PARSE_RES_FIFO_MEM_PAR_ERR_3 | \
	AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_RFF_PARSE_RES_FIFO_MEM_PAR_ERR_4 | \
	AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_RFF_PARSE_RES_FIFO_MEM_PAR_ERR_5 | \
	AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_RFF_PARSE_RES_FIFO_MEM_PAR_ERR_6)

#define AL_ETH_TX_PKT_UDMA_FLAGS	(AL_ETH_TX_FLAGS_NO_SNOOP | \
					 AL_ETH_TX_FLAGS_INT)

#define AL_ETH_TX_PKT_META_FLAGS	(AL_ETH_TX_FLAGS_IPV4_L3_CSUM | \
					AL_ETH_TX_FLAGS_L4_CSUM |	\
					AL_ETH_TX_FLAGS_L4_PARTIAL_CSUM |\
					AL_ETH_TX_FLAGS_L2_MACSEC_PKT | \
					AL_ETH_TX_FLAGS_L2_DIS_FCS |\
					AL_ETH_TX_FLAGS_TSO |\
					AL_ETH_TX_FLAGS_TS |\
					AL_ETH_TX_FLAGS_PACKET_DROP)

#define AL_ETH_TX_SRC_VLAN_CNT_MASK	3
#define AL_ETH_TX_SRC_VLAN_CNT_SHIFT	5
#define AL_ETH_TX_L4_PROTO_IDX_MASK	0x1F
#define AL_ETH_TX_L4_PROTO_IDX_SHIFT	8
#define AL_ETH_TX_TUNNEL_MODE_SHIFT		18
#define AL_ETH_TX_OUTER_L3_PROTO_SHIFT		20
#define AL_ETH_TX_VLAN_MOD_ADD_SHIFT		22
#define AL_ETH_TX_VLAN_MOD_DEL_SHIFT		24
#define AL_ETH_TX_VLAN_MOD_E_SEL_SHIFT		26
#define AL_ETH_TX_VLAN_MOD_VID_SEL_SHIFT	28
#define AL_ETH_TX_VLAN_MOD_PBIT_SEL_SHIFT	30

#define AL_ETH_TX_GEN_CRC_CHECKSUM_EN_MASK	AL_FIELD_MASK(31, 28)
#define AL_ETH_TX_GEN_CRC_CHECKSUM_EN_SHIFT	28
#define AL_ETH_TX_PROTOCOL_NUM_MASK		AL_FIELD_MASK(27, 22)
#define AL_ETH_TX_PROTOCOL_NUM_SHIFT		22
#define AL_ETH_TX_PACKET_DROP			AL_BIT(16)

/* tx Meta Descriptor defines */
#define AL_ETH_TX_META_MSS_LSB_VAL_SHIFT	22
#define AL_ETH_TX_META_MSS_MSB_TS_VAL_SHIFT	16
#define AL_ETH_TX_META_OUTER_L3_LEN_MASK	0x1f
#define AL_ETH_TX_META_OUTER_L3_LEN_SHIFT	24
#define AL_ETH_TX_META_OUTER_L3_OFF_HIGH_MASK	0x18
#define AL_ETH_TX_META_OUTER_L3_OFF_HIGH_SHIFT	10
#define AL_ETH_TX_META_OUTER_L3_OFF_LOW_MASK	0x07
#define AL_ETH_TX_META_OUTER_L3_OFF_LOW_SHIFT	29
#define AL_ETH_TX_CHECKSUM_ENGINE_L3_OFFSET_MAX AL_FIELD_MASK(7, 0)

/* tx Meta Descriptor defines - MacSec */
#define AL_ETH_TX_MACSEC_SIGN_SHIFT			  0		/* Sign TX pkt */
#define AL_ETH_TX_MACSEC_ENCRYPT_SHIFT			  1		/* Encrypt TX pkt */
#define AL_ETH_TX_MACSEC_AN_LSB_SHIFT			  2		/* Association Number */
#define AL_ETH_TX_MACSEC_AN_MSB_SHIFT			  3
#define AL_ETH_TX_MACSEC_SC_LSB_SHIFT			  4		/* Secured Channel */
#define AL_ETH_TX_MACSEC_SC_MSB_SHIFT			  9
#define AL_ETH_TX_MACSEC_SECURED_PYLD_LEN_LSB_SHIFT	 10		/* Secure Payload Length (0x3FFF for non-SL packets) */
#define AL_ETH_TX_MACSEC_SECURED_PYLD_LEN_MSB_SHIFT	 23

/* Rx Descriptor defines */
#define AL_ETH_RX_L3_PROTO_IDX_MASK	0x1F
#define AL_ETH_RX_SRC_VLAN_CNT_MASK	3
#define AL_ETH_RX_SRC_VLAN_CNT_SHIFT	5
#define AL_ETH_RX_L4_PROTO_IDX_MASK	0x1F
#define AL_ETH_RX_L4_PROTO_IDX_SHIFT	8
#define AL_ETH_V4_RX_PROTO_NUM_SHIFT	0
#define AL_ETH_V4_RX_PROTO_NUM_MASK	0x3F

#define AL_ETH_RX_L3_OFFSET_SHIFT	9
#define AL_ETH_RX_L3_OFFSET_MASK	(0x7f << AL_ETH_RX_L3_OFFSET_SHIFT)
#define AL_ETH_RX_HASH_SHIFT		16
#define AL_ETH_RX_HASH_MASK		(0xffff 	<< AL_ETH_RX_HASH_SHIFT)

#define ETH_MAC_GEN_LED_CFG_BLINK_TIMER_VAL 5
#define ETH_MAC_GEN_LED_CFG_ACT_TIMER_VAL 7

#define AL_ETH_RX_W0_EC_INFO_LB_INDICATION	AL_BIT(12)
#define AL_ETH_RX_W0_ETH_ERR_1				AL_BIT(16)
#define AL_ETH_RX_W0_ETH_ERR_2				AL_BIT(17)

/* Tx VID Table*/
#define AL_ETH_TX_VLAN_TABLE_UDMA_MASK		0xF
#define AL_ETH_TX_VLAN_TABLE_FWD_TO_MAC		(1 << 4)

/* tx gpd defines */
#define AL_ETH_TX_GPD_L3_PROTO_MASK		0x1f
#define AL_ETH_TX_GPD_L3_PROTO_SHIFT		0
#define AL_ETH_TX_GPD_L4_PROTO_MASK		0x1f
#define AL_ETH_TX_GPD_L4_PROTO_SHIFT		5
#define AL_ETH_TX_GPD_TUNNEL_CTRL_MASK		0x7
#define AL_ETH_TX_GPD_TUNNEL_CTRL_SHIFT		10
#define AL_ETH_TX_GPD_SRC_VLAN_CNT_MASK		0x3
#define AL_ETH_TX_GPD_SRC_VLAN_CNT_SHIFT	13
#define AL_ETH_TX_GPD_CAM_DATA_2_SHIFT		32
#define AL_ETH_TX_GPD_CAM_MASK_2_SHIFT		32
#define AL_ETH_TX_GPD_CAM_CTRL_VALID_SHIFT	31

/* tx gcp defines */
#define AL_ETH_TX_GCP_POLY_SEL_MASK		0x1
#define AL_ETH_TX_GCP_POLY_SEL_SHIFT		0
#define AL_ETH_TX_GCP_CRC32_BIT_COMP_MASK	0x1
#define AL_ETH_TX_GCP_CRC32_BIT_COMP_SHIFT	1
#define AL_ETH_TX_GCP_CRC32_BIT_SWAP_MASK	0x1
#define AL_ETH_TX_GCP_CRC32_BIT_SWAP_SHIFT	2
#define AL_ETH_TX_GCP_CRC32_BYTE_SWAP_MASK	0x1
#define AL_ETH_TX_GCP_CRC32_BYTE_SWAP_SHIFT	3
#define AL_ETH_TX_GCP_DATA_BIT_SWAP_MASK	0x1
#define AL_ETH_TX_GCP_DATA_BIT_SWAP_SHIFT	4
#define AL_ETH_TX_GCP_DATA_BYTE_SWAP_MASK	0x1
#define AL_ETH_TX_GCP_DATA_BYTE_SWAP_SHIFT	5
#define AL_ETH_TX_GCP_TRAIL_SIZE_MASK		0xFF
#define AL_ETH_TX_GCP_TRAIL_SIZE_SHIFT		6
#define AL_ETH_TX_GCP_HEAD_SIZE_MASK		0xFF
#define AL_ETH_TX_GCP_HEAD_SIZE_SHIFT		16
#define AL_ETH_TX_GCP_HEAD_CALC_MASK		0x1
#define AL_ETH_TX_GCP_HEAD_CALC_SHIFT		24
#define AL_ETH_TX_GCP_MASK_POLARITY_MASK	0x1
#define AL_ETH_TX_GCP_MASK_POLARITY_SHIFT	25

#define AL_ETH_TX_GCP_OPCODE_1_MASK		0x3F
#define AL_ETH_TX_GCP_OPCODE_1_SHIFT		0
#define AL_ETH_TX_GCP_OPCODE_2_MASK		0x3F
#define AL_ETH_TX_GCP_OPCODE_2_SHIFT		6
#define AL_ETH_TX_GCP_OPCODE_3_MASK		0x3F
#define AL_ETH_TX_GCP_OPCODE_3_SHIFT		12
#define AL_ETH_TX_GCP_OPSEL_1_MASK		0xF
#define AL_ETH_TX_GCP_OPSEL_1_SHIFT		0
#define AL_ETH_TX_GCP_OPSEL_2_MASK		0xF
#define AL_ETH_TX_GCP_OPSEL_2_SHIFT		4
#define AL_ETH_TX_GCP_OPSEL_3_MASK		0xF
#define AL_ETH_TX_GCP_OPSEL_3_SHIFT		8
#define AL_ETH_TX_GCP_OPSEL_4_MASK		0xF
#define AL_ETH_TX_GCP_OPSEL_4_SHIFT		12

/*  Tx crc_chksum_replace defines (This table is used only for ETH v3) */
#define L4_CHECKSUM_DIS_AND_L3_CHECKSUM_DIS	0x00
#define L4_CHECKSUM_DIS_AND_L3_CHECKSUM_EN	0x20
#define L4_CHECKSUM_EN_AND_L3_CHECKSUM_DIS	0x40
#define L4_CHECKSUM_EN_AND_L3_CHECKSUM_EN	0x60
#define AL_ETH_TX_CRC_CHKSUM_REPLACE_CMD_L3_CSUM_EN_MASK	0x1
#define AL_ETH_TX_CRC_CHKSUM_REPLACE_CMD_L3_CSUM_EN_SHIFT	0
#define AL_ETH_TX_CRC_CHKSUM_REPLACE_CMD_L4_CSUM_EN_MASK	0x1
#define AL_ETH_TX_CRC_CHKSUM_REPLACE_CMD_L4_CSUM_EN_SHIFT	1
#define AL_ETH_TX_CRC_CHKSUM_REPLACE_CMD_CRC_EN_MASK		0x1
#define AL_ETH_TX_CRC_CHKSUM_REPLACE_CMD_CRC_EN_SHIFT		2

/* rx gpd defines */
#define AL_ETH_RX_GPD_OUTER_L3_PROTO_MASK		0x1f
#define AL_ETH_RX_GPD_OUTER_L3_PROTO_SHIFT		(3 + 0)
#define AL_ETH_RX_GPD_OUTER_L4_PROTO_MASK		0x1f
#define AL_ETH_RX_GPD_OUTER_L4_PROTO_SHIFT		(3 + 8)
#define AL_ETH_RX_GPD_INNER_L3_PROTO_MASK		0x1f
#define AL_ETH_RX_GPD_INNER_L3_PROTO_SHIFT		(3 + 16)
#define AL_ETH_RX_GPD_INNER_L4_PROTO_MASK		0x1f
#define AL_ETH_RX_GPD_INNER_L4_PROTO_SHIFT		(3 + 24)
#define AL_ETH_RX_GPD_OUTER_PARSE_CTRL_MASK		0xFF
#define AL_ETH_RX_GPD_OUTER_PARSE_CTRL_SHIFT		32
#define AL_ETH_RX_GPD_INNER_PARSE_CTRL_MASK		0xFF
#define AL_ETH_RX_GPD_INNER_PARSE_CTRL_SHIFT		40
#define AL_ETH_RX_GPD_L3_PRIORITY_MASK			0xFF
#define AL_ETH_RX_GPD_L3_PRIORITY_SHIFT			48
#define AL_ETH_RX_GPD_L4_DST_PORT_LSB_MASK		0xFF
#define AL_ETH_RX_GPD_L4_DST_PORT_LSB_SHIFT		56
#define AL_ETH_RX_GPD_CAM_DATA_2_SHIFT			32
#define AL_ETH_RX_GPD_CAM_MASK_2_SHIFT			32
#define AL_ETH_RX_GPD_CAM_CTRL_VALID_SHIFT		31

#define AL_ETH_RX_GPD_PARSE_RESULT_OUTER_START_OFFSET(rev_id) \
		(((rev_id) == AL_ETH_REV_ID_4) ? (8 + 106 + 88) : 106)

#define AL_ETH_RX_GPD_PARSE_RESULT_INNER_START_OFFSET(rev_id) \
		(((rev_id) == AL_ETH_REV_ID_4) ? 8 : 0)

#define AL_ETH_RX_GPD_PARSE_RESULT_OUTER_L3_PROTO_IDX_OFFSET(rev_id) \
		(AL_ETH_RX_GPD_PARSE_RESULT_OUTER_START_OFFSET(rev_id) + 5)
#define AL_ETH_RX_GPD_PARSE_RESULT_OUTER_L4_PROTO_IDX_OFFSET(rev_id) \
		(AL_ETH_RX_GPD_PARSE_RESULT_OUTER_START_OFFSET(rev_id) + 10)
#define AL_ETH_RX_GPD_PARSE_RESULT_OUTER_PARSE_CTRL(rev_id) \
		(AL_ETH_RX_GPD_PARSE_RESULT_OUTER_START_OFFSET(rev_id) + 4)
#define AL_ETH_RX_GPD_PARSE_RESULT_L3_PRIORITY(rev_id) \
		(AL_ETH_RX_GPD_PARSE_RESULT_OUTER_START_OFFSET(rev_id) + 13)
#define AL_ETH_RX_GPD_PARSE_RESULT_OUTER_L4_DST_PORT_LSB(rev_id) \
		(AL_ETH_RX_GPD_PARSE_RESULT_OUTER_START_OFFSET(rev_id) + 65)
#define AL_ETH_RX_GPD_PARSE_RESULT_INNER_PARSE_CTRL(rev_id) \
		(AL_ETH_RX_GPD_PARSE_RESULT_INNER_START_OFFSET(rev_id) + 4)
#define AL_ETH_RX_GPD_PARSE_RESULT_INNER_L3_PROTO_IDX_OFFSET(rev_id) \
		(AL_ETH_RX_GPD_PARSE_RESULT_INNER_START_OFFSET(rev_id) + 5)
#define AL_ETH_RX_GPD_PARSE_RESULT_INNER_L4_PROTO_IDX_OFFSET(rev_id) \
		(AL_ETH_RX_GPD_PARSE_RESULT_INNER_START_OFFSET(rev_id) + 10)

/* rx gcp defines */
#define AL_ETH_RX_GCP_POLY_SEL_MASK		0x1
#define AL_ETH_RX_GCP_POLY_SEL_SHIFT		0
#define AL_ETH_RX_GCP_CRC32_BIT_COMP_MASK	0x1
#define AL_ETH_RX_GCP_CRC32_BIT_COMP_SHIFT	1
#define AL_ETH_RX_GCP_CRC32_BIT_SWAP_MASK	0x1
#define AL_ETH_RX_GCP_CRC32_BIT_SWAP_SHIFT	2
#define AL_ETH_RX_GCP_CRC32_BYTE_SWAP_MASK      0x1
#define AL_ETH_RX_GCP_CRC32_BYTE_SWAP_SHIFT	3
#define AL_ETH_RX_GCP_DATA_BIT_SWAP_MASK	0x1
#define AL_ETH_RX_GCP_DATA_BIT_SWAP_SHIFT	4
#define AL_ETH_RX_GCP_DATA_BYTE_SWAP_MASK       0x1
#define AL_ETH_RX_GCP_DATA_BYTE_SWAP_SHIFT	5
#define AL_ETH_RX_GCP_TRAIL_SIZE_MASK		0xFF
#define AL_ETH_RX_GCP_TRAIL_SIZE_SHIFT		6
#define AL_ETH_RX_GCP_HEAD_SIZE_MASK		0xFF
#define AL_ETH_RX_GCP_HEAD_SIZE_SHIFT		16
#define AL_ETH_RX_GCP_HEAD_CALC_MASK		0x1
#define AL_ETH_RX_GCP_HEAD_CALC_SHIFT		24
#define AL_ETH_RX_GCP_MASK_POLARITY_MASK	0x1
#define AL_ETH_RX_GCP_MASK_POLARITY_SHIFT	25

#define AL_ETH_RX_GCP_OPCODE_1_MASK		0x3F
#define AL_ETH_RX_GCP_OPCODE_1_SHIFT		0
#define AL_ETH_RX_GCP_OPCODE_2_MASK		0x3F
#define AL_ETH_RX_GCP_OPCODE_2_SHIFT		6
#define AL_ETH_RX_GCP_OPCODE_3_MASK		0x3F
#define AL_ETH_RX_GCP_OPCODE_3_SHIFT		12
#define AL_ETH_RX_GCP_OPSEL_1_MASK		0xF
#define AL_ETH_RX_GCP_OPSEL_1_SHIFT		0
#define AL_ETH_RX_GCP_OPSEL_2_MASK		0xF
#define AL_ETH_RX_GCP_OPSEL_2_SHIFT		4
#define AL_ETH_RX_GCP_OPSEL_3_MASK		0xF
#define AL_ETH_RX_GCP_OPSEL_3_SHIFT		8
#define AL_ETH_RX_GCP_OPSEL_4_MASK		0xF
#define AL_ETH_RX_GCP_OPSEL_4_SHIFT		12

/* ETHv4 extension */
#define AL_ETH_RX_GCP_POLY_SEL_EXT_MASK		0x3
#define AL_ETH_RX_GCP_POLY_SEL_EXT_SHIFT	0
#define AL_ETH_RX_GCP_CRC_CKS_SEL_MASK		0x1
#define AL_ETH_RX_GCP_CRC_CKS_SEL_SHIFT	        4
#define AL_ETH_RX_GCP_TRAIL_CALC_MASK		0x1
#define AL_ETH_RX_GCP_TRAIL_CALC_SHIFT	        7
#define AL_ETH_RX_GCP_TRAIL_OPSEL_1_MASK	0xF
#define AL_ETH_RX_GCP_TRAIL_OPSEL_1_SHIFT	8
#define AL_ETH_RX_GCP_TRAIL_OPSEL_2_MASK	0xF
#define AL_ETH_RX_GCP_TRAIL_OPSEL_2_SHIFT	12
#define AL_ETH_RX_GCP_TRAIL_OPSEL_3_MASK	0xF
#define AL_ETH_RX_GCP_TRAIL_OPSEL_3_SHIFT	16
#define AL_ETH_RX_GCP_TRAIL_OPSEL_4_MASK	0xF
#define AL_ETH_RX_GCP_TRAIL_OPSEL_4_SHIFT	20
#define AL_ETH_RX_GCP_HEAD_SIZE_EXT_MASK	0xFF
#define AL_ETH_RX_GCP_HEAD_SIZE_EXT_SHIFT	24

#define AL_ETH_RX_GCP_CRC_FIELD_SIZE_MASK	0x3F
#define AL_ETH_RX_GCP_CRC_FIELD_SIZE_SHIFT	0
#define AL_ETH_RX_GCP_CRC_FIELD_CALC_MASK	0x1
#define AL_ETH_RX_GCP_CRC_FIELD_CALC_SHIFT	8
#define AL_ETH_RX_GCP_CRC_FIELD_OFFSET_MASK	0xFFFF
#define AL_ETH_RX_GCP_CRC_FIELD_OFFSET_SHIFT	16

#define AL_ETH_RX_GCP_TRAIL_OPCODE_1_MASK	0x3F
#define AL_ETH_RX_GCP_TRAIL_OPCODE_1_SHIFT	0
#define AL_ETH_RX_GCP_TRAIL_OPCODE_2_MASK	0x3F
#define AL_ETH_RX_GCP_TRAIL_OPCODE_2_SHIFT	6
#define AL_ETH_RX_GCP_TRAIL_OPCODE_3_MASK	0x3F
#define AL_ETH_RX_GCP_TRAIL_OPCODE_3_SHIFT	12
#define AL_ETH_RX_GCP_CRC_REPLACE_SEL_MASK	0x1
#define AL_ETH_RX_GCP_CRC_REPLACE_SEL_SHIFT	30
#define AL_ETH_RX_GCP_CRC_DETECT_SEL_MASK	0x1
#define AL_ETH_RX_GCP_CRC_DETECT_SEL_SHIFT	31

#define AL_ETH_S2M_UDMA_COMP_COAL_TIMEOUT	200 /* Rx descriptors coalescing timeout in SB clocks */

/* EC fwd mac ctrl reg  - drop indication */
#define EC_FWD_MAC_CTRL_RX_VAL_DROP            (1 << 0)

/* EC fwd mac ctrl reg - control table command input */
#define EC_FWD_MAC_CTRL_RX_VAL_CTRL_CMD_MASK   0x00000006
#define EC_FWD_MAC_CTRL_RX_VAL_CTRL_CMD_SHIFT  1

/* EC fwd mac ctrl reg - UDMA selection */
#define EC_FWD_MAC_CTRL_RX_VAL_UDMA_MASK       0x000000078
#define EC_FWD_MAC_CTRL_RX_VAL_UDMA_SHIFT      3

/* EC fwd mac ctrl reg - queue number */
#define EC_FWD_MAC_CTRL_RX_VAL_QID_MASK                0x00000780
#define EC_FWD_MAC_CTRL_RX_VAL_QID_SHIFT       7

/**
 * TX forwarding table - offset of the bits to override in TX completion descriptor
 *
 * We actually want to disable the override, but the only way to do is to configure offset
 * to some invalid value.
 * Width of the completion metadata is 32 bits, so setting to 40 is high enough
 * as invalid value.
 */
#define AL_ETH_TX_TMI_FWD_TABLE_CMPL_METADATA_FIELD_OFFSET	40

static int al_eth_mac_scratch_regs_get(void __iomem *regs_base, int eth_rev_id,
	uint32_t **scratch_regs)
{
	struct al_eth_mac_regs __iomem *mac_regs_base;
	struct al_ec_regs __iomem *ec_regs_base;
	struct al_ec_gen __iomem *ec_gen_base;

	switch (eth_rev_id) {
	case AL_ETH_REV_ID_1:
	case AL_ETH_REV_ID_2:
	case AL_ETH_REV_ID_3:
		mac_regs_base =	(struct	al_eth_mac_regs __iomem *)regs_base;
		scratch_regs[0] = &mac_regs_base->mac_1g.scratch;
		scratch_regs[1] = &mac_regs_base->mac_10g.scratch;
		scratch_regs[2] = &mac_regs_base->mac_1g.mac_0;
		break;
	case AL_ETH_REV_ID_4:
		ec_regs_base = (struct al_ec_regs __iomem *)regs_base;
		ec_gen_base = (struct al_ec_gen __iomem *)&ec_regs_base->gen;
		scratch_regs[0] = &ec_gen_base->scratch_pad_1;
		scratch_regs[1] = &ec_gen_base->scratch_pad_2;
		scratch_regs[2] = &ec_gen_base->scratch_pad_3;
		break;
	default:
		al_err("%s : Error, unsupported eth rev %d", __func__, eth_rev_id);
		return -EPERM;
	}

	return 0;
}

/**
 * EPE handle initialization from ETH adapter
 */
static int epe_handle_init(struct al_hal_eth_adapter *adapter,
	struct al_eth_epe_handle *epe_handle, enum al_eth_epe_type epe_type)
{
	struct al_eth_epe_handle_init_params init_params;

	init_params.regs_base = al_eth_epe_regs_get(adapter);
	init_params.eth_rev_id = adapter->rev_id;
	init_params.type = epe_type;

	return al_eth_epe_handle_init(epe_handle, &init_params);
}

/**
 * read 40G MAC registers (indirect access)
 *
 * @param adapter pointer to the private structure
 * @param reg_addr address in the an registers
 *
 * @return the register value
 */
static uint32_t al_eth_40g_mac_reg_read(
			struct al_hal_eth_adapter *adapter,
			uint32_t reg_addr)
{
	uint32_t val;

	/* indirect access */
	al_reg_write32(&adapter->mac_regs_base->gen_v3.mac_40g_ll_addr, reg_addr);
	val = al_reg_read32(&adapter->mac_regs_base->gen_v3.mac_40g_ll_data);

	al_dbg("[%s]: %s - reg %d. val 0x%x\n",
	       adapter->name, __func__, reg_addr, val);

	return val;
}

/**
 * write 40G MAC registers (indirect access)
 *
 * @param adapter pointer to the private structure
 * @param reg_addr address in the an registers
 * @param reg_data value to write to the register
 *
 */
static void al_eth_40g_mac_reg_write(
			struct al_hal_eth_adapter *adapter,
			uint32_t reg_addr,
			uint32_t reg_data)
{
	/* indirect access */
	al_reg_write32(&adapter->mac_regs_base->gen_v3.mac_40g_ll_addr, reg_addr);
	al_reg_write32(&adapter->mac_regs_base->gen_v3.mac_40g_ll_data, reg_data);

	al_dbg("[%s]: %s - reg %d. val 0x%x\n",
	       adapter->name, __func__, reg_addr, reg_data);
}

/* list of supported init cases */
enum al_eth_ctrl_mem_init_type {
	AL_ETH_CTRL_MEM_INIT_TX_AND_RX	= 0,
	AL_ETH_CTRL_MEM_INIT_TX_ONLY	= 1,
};

/**
 * This register is only available in eth v4 (std & adv)
 * Eth v4 adv register file should be fully backward compatible with eth v4 std
 * however several mem init bits has different meaning in eth v4 adv
 * and therefore are not documented correctly for eth v4 std
 */
#define EC_V4_STD_GEN_MEM_INIT_TRIG_RXF_DATA_FIFO	EC_GEN_MEM_INIT_TRIG_TFW_CSP_PN_MAP_TABLE
#define EC_V4_STD_GEN_MEM_INIT_TRIG_RXF_SOP_FIFO EC_GEN_MEM_INIT_TRIG_TFW_CRC_PROTOCOL_CONTROL_TABLE
#define EC_V4_STD_GEN_MEM_INIT_TRIG_RXF_EOP_FIFO	AL_BIT(5)
#define EC_V4_STD_GEN_MEM_INIT_TRIG_RFW_FLOW_TABLE	AL_BIT(6)

/* Trigger Automatic Sram memories initialization for the eth_ctrl memories (initialize with 0's) */
static void al_eth_ctrl_mem_init(
	struct al_hal_eth_adapter *adapter,
	enum al_eth_ctrl_mem_init_type init_type)
{
	unsigned int rev_id = adapter->rev_id;
	uint32_t val =
		EC_GEN_MEM_INIT_TRIG_TFW_DATA_FIFO |
		EC_GEN_MEM_INIT_TRIG_TFW_SOP_FIFO |
		EC_GEN_MEM_INIT_TRIG_TFW_EOP_FIFO |
		EC_GEN_MEM_INIT_TRIG_TSO_META_DATA_CACHE;

	/**
	 * Only available in eth v4 std / adv
	 * We treat eth v4 std as eth rev 2
	 */
	if (adapter->original_rev_id < AL_ETH_REV_ID_4)
		return;

	switch (rev_id) {
	case AL_ETH_REV_ID_2:
		/** This is actually eth v4 std */
		if (init_type == AL_ETH_CTRL_MEM_INIT_TX_AND_RX) {
			val |=
				EC_V4_STD_GEN_MEM_INIT_TRIG_RXF_DATA_FIFO |
				EC_V4_STD_GEN_MEM_INIT_TRIG_RXF_SOP_FIFO |
				EC_V4_STD_GEN_MEM_INIT_TRIG_RXF_EOP_FIFO |
				EC_V4_STD_GEN_MEM_INIT_TRIG_RFW_FLOW_TABLE;
		}

		break;
	case AL_ETH_REV_ID_4:
		/* Add TX */
		val |=
			EC_GEN_MEM_INIT_TRIG_TFW_VLAN_TABLE |
			EC_GEN_MEM_INIT_TRIG_TFW_GCP_TABLE |
			EC_GEN_MEM_INIT_TRIG_TFW_CSP_PN_MAP_TABLE |
			EC_GEN_MEM_INIT_TRIG_TFW_CRC_PROTOCOL_CONTROL_TABLE;
		/* Add RX */
		if (init_type == AL_ETH_CTRL_MEM_INIT_TX_AND_RX) {
			val |=
				EC_GEN_MEM_INIT_TRIG_RFW_THASH_TABLE |
				EC_GEN_MEM_INIT_TRIG_RFW_MHASH_TABLE |
				EC_GEN_MEM_INIT_TRIG_RFW_VLAN_TABLE |
				EC_GEN_MEM_INIT_TRIG_RFW_CTRL_TABLE |
				EC_GEN_MEM_INIT_TRIG_RFW_HS_CTRL_TABLE |
				EC_GEN_MEM_INIT_TRIG_RFW_GCP_TABLE |
				EC_GEN_MEM_INIT_TRIG_RFW_PKT_HDR_FIFO |
				EC_GEN_MEM_INIT_TRIG_RFW_META_FIFO |
				EC_GEN_MEM_INIT_TRIG_RFW_DATA_FIFO |
				EC_GEN_MEM_INIT_TRIG_RFF_RFW_SR_RESULT_FIFO |
				EC_GEN_MEM_INIT_TRIG_RFF_PARSE_RES_FIFO |
				EC_GEN_MEM_INIT_TRIG_RFF_DATA_FIFO |
				EC_GEN_MEM_INIT_TRIG_RFW_IN_PIPE_DATA_FIFO |
				EC_GEN_MEM_INIT_TRIG_RFW_CRC_PROTOCOL_CONTROL_TABLE;
		}
		break;
	default:
		al_assert(0);
		break;
	}

	al_reg_write32(&adapter->ec_regs_base->gen.mem_init, val);

	/* wait enough time for the init to end */
	al_udelay(10);
}

/**
 * Ethernet EC iofic pointer
 *
 * @param adapter pointer to the private structure
 *
 * @return pointer to iofic regs
 */
void __iomem *al_eth_ec_iofic_regs_get(
	struct al_hal_eth_adapter *adapter)
{
	al_assert(adapter);
	al_assert(adapter->ec_ints_base);
	return adapter->ec_ints_base;
}

#define AL_ETH_EC_IOFIC_GROUP_C_DATA_FIFO_ERRORS \
	(AL_ETH_EC_IOFIC_GROUP_C_TFW_DATA_FIFO_2_PARITY | \
	 AL_ETH_EC_IOFIC_GROUP_C_TFW_DATA_FIFO_1_PARITY | \
	 AL_ETH_EC_IOFIC_GROUP_C_RXF_DATA_FIFO_1_2_PARITY | \
	 AL_ETH_EC_IOFIC_GROUP_C_RXF_DATA_FIFO_1_1_PARITY)

#define AL_ETH_EC_IOFIC_GROUP_C_INFO_FIFO_ERRORS \
	(AL_ETH_EC_IOFIC_GROUP_C_RXF_EOP_FIFO_1_PARITY | \
	 AL_ETH_EC_IOFIC_GROUP_C_RXF_SOP_FIFO_1_PARITY | \
	 AL_ETH_EC_IOFIC_GROUP_C_TFW_SOP_FIFO_1_PARITY | \
	 AL_ETH_EC_IOFIC_GROUP_C_TFW_EOP_FIFO_1_PARITY)

#define AL_ETH_EC_IOFIC_GROUP_C_LB_DATA_FIFO_ERRORS \
	(AL_ETH_EC_IOFIC_GROUP_C_RXF_DATA_FIFO_2_2_PARITY | \
	 AL_ETH_EC_IOFIC_GROUP_C_RXF_DATA_FIFO_2_1_PARITY)

#define AL_ETH_EC_IOFIC_GROUP_C_LB_INFO_FIFO_ERRORS \
	(AL_ETH_EC_IOFIC_GROUP_C_RXF_EOP_FIFO_2_PARITY | \
	 AL_ETH_EC_IOFIC_GROUP_C_RXF_SOP_FIFO_2_PARITY)

static void al_eth_ec_error_ints_setup(struct al_hal_eth_adapter *adapter,
				       al_bool clear_only)
{
	uint32_t ec_a_mask;
	uint32_t ec_b_mask;
	uint32_t ec_c_mask;
	uint32_t ec_d_mask;

	al_assert(adapter->ec_ints_base);

	al_eth_ec_iofic_attrs_get(adapter, AL_ETH_IOFIC_LEVEL_PRIMARY,
				  AL_ETH_IOFIC_ATTRS_RUNTIME_CONFIG |
				  AL_ETH_IOFIC_ATTRS_FILTER_TRANSIENT |
				  AL_ETH_IOFIC_ATTRS_FATAL,
				  &ec_a_mask, &ec_b_mask,
				  &ec_c_mask, &ec_d_mask);

	if (!clear_only) {
		/* EC IOFIC config */
		al_iofic_config(adapter->ec_ints_base,
				AL_INT_GROUP_A,
				INT_CONTROL_GRP_MASK_MSI_X);
		al_iofic_config(adapter->ec_ints_base,
				AL_INT_GROUP_B,
				INT_CONTROL_GRP_MASK_MSI_X);
		al_iofic_config(adapter->ec_ints_base,
				AL_INT_GROUP_C,
				INT_CONTROL_GRP_MASK_MSI_X);
		al_iofic_config(adapter->ec_ints_base,
				AL_INT_GROUP_D,
				INT_CONTROL_GRP_MASK_MSI_X);

		/* UDMA IOFIC config */
		al_iofic_config(al_udma_iofic_reg_base_get_adv(&adapter->rx_udma,
							       AL_UDMA_IOFIC_LEVEL_PRIMARY),
				AL_INT_GROUP_D,
				INT_CONTROL_GRP_MASK_MSI_X);
	}

	/* clear old unvalid parity interrupts */
	al_iofic_clear_cause(adapter->ec_ints_base,
			     AL_INT_GROUP_A,
			     ec_a_mask);
	al_iofic_clear_cause(adapter->ec_ints_base,
			     AL_INT_GROUP_B,
			     ec_b_mask);
	al_iofic_clear_cause(adapter->ec_ints_base,
			     AL_INT_GROUP_C,
			     ec_c_mask);
	al_iofic_clear_cause(adapter->ec_ints_base,
			     AL_INT_GROUP_D,
			     ec_d_mask);

	if (adapter->rev_id >= AL_ETH_REV_ID_4) {
		/**
		 * Clear and mask unvalid interrupts bits as this may rise
		 * during initial traffic
		 */
		al_iofic_clear_cause(adapter->ec_ints_base, AL_INT_GROUP_D, ~ec_d_mask);
		al_iofic_mask(adapter->ec_ints_base, AL_INT_GROUP_D, ~ec_d_mask);

		al_udma_iofic_clear_cause_adv(
			&adapter->rx_udma, AL_UDMA_IOFIC_LEVEL_PRIMARY,
			AL_INT_GROUP_D, AL_INT_UDMA_V4_GROUP_D_APP_EXT_INT);

	}

	if (!clear_only) {
		/* unmask interrupts */
		al_iofic_unmask(adapter->ec_ints_base,
				AL_INT_GROUP_A,
				ec_a_mask);
		al_iofic_unmask(adapter->ec_ints_base,
				AL_INT_GROUP_B,
				ec_b_mask);
		al_iofic_unmask(adapter->ec_ints_base,
				AL_INT_GROUP_C,
				ec_c_mask);
		al_iofic_unmask(adapter->ec_ints_base,
				AL_INT_GROUP_D,
				ec_d_mask);

		/* UDMA IOFIC primary unmask */
		al_udma_iofic_unmask_ext_app(&adapter->rx_udma, AL_FALSE);
	}

	if ((adapter->rev_id >= AL_ETH_REV_ID_4) && (adapter->dev_id == AL_ETH_DEV_ID_ADVANCED)) {
		uint32_t sec_ec_a_mask;
		uint32_t sec_ec_b_mask;
		uint32_t sec_ec_c_mask;
		uint32_t sec_ec_d_mask;

		/**
		 * EC secondary lines are connected to EC primary iofic lines and are unmasked
		 * with the reset of primary interrupts
		 */

		al_eth_ec_iofic_attrs_get(adapter, AL_ETH_IOFIC_LEVEL_SECONDARY,
					  AL_ETH_IOFIC_ATTRS_RUNTIME_CONFIG |
					  AL_ETH_IOFIC_ATTRS_FILTER_TRANSIENT |
					  AL_ETH_IOFIC_ATTRS_FATAL,
					  &sec_ec_a_mask, &sec_ec_b_mask,
					  &sec_ec_c_mask, &sec_ec_d_mask);

		if (!clear_only) {
			/* EC IOFIC config */
			al_iofic_config(adapter->sec_ec_ints_base,
					AL_INT_GROUP_A,
					INT_CONTROL_GRP_MASK_MSI_X);
		}

		al_iofic_clear_cause(adapter->sec_ec_ints_base,
				AL_INT_GROUP_A,
				sec_ec_a_mask);

		if (!clear_only) {
			/* unmask interrupts */
			al_iofic_unmask(adapter->sec_ec_ints_base,
					AL_INT_GROUP_A,
					sec_ec_a_mask);
		}

		if (adapter->serdes_lane == 0) {
			al_iofic_clear_cause(adapter->ec_ints_base,
				AL_INT_GROUP_C, AL_ETH_V4_EC_IOFIC_GROUP_C_EC_SEC_INT_OUTPUT_2);
			if (!clear_only) {
				al_iofic_unmask(adapter->ec_ints_base,
						AL_INT_GROUP_C,
						AL_ETH_V4_EC_IOFIC_GROUP_C_EC_SEC_INT_OUTPUT_2);
			}
		}
	}
}

static void al_eth_mac_error_ints_setup_v1_v3(struct al_hal_eth_adapter *adapter,
					      al_bool clear_only)
{
	uint32_t mac_a_mask;
	uint32_t mac_b_mask;
	uint32_t mac_c_mask;
	uint32_t mac_d_mask;

	al_assert(adapter->ec_ints_base);
	al_assert(adapter->mac_ints_base);

	/* called when all transient bits are stable, so no need to filter them
	 * out */
	al_eth_mac_iofic_attrs_get(&adapter->mac_obj,
				   AL_ETH_IOFIC_ATTRS_RUNTIME_CONFIG |
				   AL_ETH_IOFIC_ATTRS_FATAL,
				   &mac_a_mask, &mac_b_mask,
				   &mac_c_mask, &mac_d_mask);

	if (!clear_only) {
		/* MAC IOFIC config */
		al_iofic_config(adapter->mac_ints_base,
				AL_INT_GROUP_A,
				INT_CONTROL_GRP_MASK_MSI_X);
		al_iofic_config(adapter->mac_ints_base,
				AL_INT_GROUP_B,
				INT_CONTROL_GRP_MASK_MSI_X);
		al_iofic_config(adapter->mac_ints_base,
				AL_INT_GROUP_C,
				INT_CONTROL_GRP_MASK_MSI_X);
		al_iofic_config(adapter->mac_ints_base,
				AL_INT_GROUP_D,
				INT_CONTROL_GRP_MASK_MSI_X);
	}

	/* clear old non valid parity interrupts */
	al_iofic_clear_cause(adapter->mac_ints_base,
			     AL_INT_GROUP_A,
			     mac_a_mask);
	al_iofic_clear_cause(adapter->mac_ints_base,
			     AL_INT_GROUP_B,
			     mac_b_mask);
	al_iofic_clear_cause(adapter->mac_ints_base,
			     AL_INT_GROUP_C,
			     mac_c_mask);
	al_iofic_clear_cause(adapter->mac_ints_base,
			     AL_INT_GROUP_D,
			     mac_d_mask);
	al_iofic_clear_cause(adapter->ec_ints_base,
			     AL_INT_GROUP_A,
			     AL_ETH_EC_IOFIC_GROUP_A_MAC_INT_IN);

	if (!clear_only) {
		/* MAC IOFIC unmask */
		al_iofic_unmask(adapter->mac_ints_base,
				AL_INT_GROUP_A,
				mac_a_mask);
		al_iofic_unmask(adapter->mac_ints_base,
				AL_INT_GROUP_B,
				mac_b_mask);
		al_iofic_unmask(adapter->mac_ints_base,
				AL_INT_GROUP_C,
				mac_c_mask);
		al_iofic_unmask(adapter->mac_ints_base,
				AL_INT_GROUP_D,
				mac_d_mask);
		al_iofic_unmask(adapter->ec_ints_base,
				AL_INT_GROUP_A,
				AL_ETH_EC_IOFIC_GROUP_A_MAC_INT_IN);
	}
}

static void al_eth_mac_error_ints_setup_ex(struct al_hal_eth_adapter *adapter,
					   al_bool clear_only)
{
	uint32_t mac_bits_in_ec = 0;
	/** internal MAC implementation configures the MAC IOFICS & unmask the relevant bits */
	adapter->mac_obj.error_ints_unmask(&adapter->mac_obj);

	switch (adapter->common_mode) {
	case AL_ETH_COMMON_MODE_1X100G:
		mac_bits_in_ec =
			AL_ETH_V4_EC_IOFIC_GROUP_A_MAC_INT_IN_0 |
			AL_ETH_V4_EC_IOFIC_GROUP_A_MAC_INT_IN_1 |
			AL_ETH_V4_EC_IOFIC_GROUP_A_MAC_INT_IN_2 |
			AL_ETH_V4_EC_IOFIC_GROUP_A_MAC_INT_IN_3;
		break;
	case AL_ETH_COMMON_MODE_4X25G:
		switch (adapter->serdes_lane) {
		case 0:
			mac_bits_in_ec = AL_ETH_V4_EC_IOFIC_GROUP_A_MAC_INT_IN_0;
			break;
		case 1:
			mac_bits_in_ec = AL_ETH_V4_EC_IOFIC_GROUP_A_MAC_INT_IN_1;
			break;
		case 2:
			mac_bits_in_ec = AL_ETH_V4_EC_IOFIC_GROUP_A_MAC_INT_IN_2;
			break;
		case 3:
			mac_bits_in_ec = AL_ETH_V4_EC_IOFIC_GROUP_A_MAC_INT_IN_3;
			break;
		default:
			al_err("%s: Invalid serdes lane\n", __func__);
			al_assert(0);
		}
		break;
	default:
		al_err("%s: Invalid common mode\n", __func__);
		al_assert(0);
	}

	/** Open EC bits that are connected to MAC iofic */
	al_iofic_clear_cause(adapter->ec_ints_base,
			AL_INT_GROUP_A,
			mac_bits_in_ec);

	if (!clear_only) {
		al_iofic_unmask(adapter->ec_ints_base,
				AL_INT_GROUP_A,
				mac_bits_in_ec);
	}
}

static void al_eth_mac_error_ints_setup(struct al_hal_eth_adapter *adapter,
					al_bool clear_only)
{

	if (adapter->rev_id <= AL_ETH_REV_ID_3)
		al_eth_mac_error_ints_setup_v1_v3(adapter, clear_only);
	else
		al_eth_mac_error_ints_setup_ex(adapter, clear_only);

}

/*****************************API Functions  **********************************/

void al_eth_adapter_handle_init(struct al_hal_eth_adapter *adapter,
	struct al_eth_adapter_handle_init_params *params)
{
	struct al_eth_mac_obj_init_params mac_obj_init_params;
	struct al_udma_params udma_params;
	int rc;

	al_assert(adapter);
	al_assert(params);

	al_memset(&mac_obj_init_params, 0, sizeof(struct al_eth_mac_obj_init_params));
	al_memset(adapter, 0, sizeof(struct al_hal_eth_adapter));

	adapter->name = params->name;

	adapter->original_rev_id = params->rev_id;
	/* Temporarily treat STD ETH V4 as STD ETH V2 */
	if (AL_ETH_V4_IS_STD(params->rev_id, params->dev_id))
		adapter->rev_id = AL_ETH_REV_ID_2;
	else
		adapter->rev_id = params->rev_id;

	adapter->dev_id = params->dev_id;
	adapter->udma_id = params->udma_id;
	adapter->common_mode = params->common_mode;
	adapter->internal_switching = params->internal_switching;
	adapter->udma_regs_base = params->udma_regs_base;
	adapter->serdes_lane = params->serdes_lane;

	if (params->vf_handle) {
		/*
		 * a VF handle should not hold regs base other than UDMA
		 */
		al_assert(params->ec_regs_base == NULL);
		al_assert(params->mac_regs_base == NULL);
		al_assert(params->mac_common_regs == NULL);
		al_assert(params->eth_common_regs_base == NULL);
	} else {
		adapter->ec_regs_base = (struct al_ec_regs __iomem *)params->ec_regs_base;
		adapter->ec_ints_base = &adapter->ec_regs_base->int_ctrl_mem[0];
		if ((params->rev_id >= AL_ETH_REV_ID_4) &&
			(params->dev_id == AL_ETH_DEV_ID_ADVANCED))
			adapter->sec_ec_ints_base = &adapter->ec_regs_base->sec_int_ctrl_mem[0];

		adapter->mac_regs_base = (struct al_eth_mac_regs __iomem *)params->mac_regs_base;
		adapter->eth_common_regs_base = (struct al_eth_common_regs __iomem *)params->eth_common_regs_base;

		/* initialize MAC handle */
		mac_obj_init_params.mac_regs_base = params->mac_regs_base;
		mac_obj_init_params.eth_rev_id = adapter->rev_id;
		mac_obj_init_params.eth_original_rev_id = adapter->original_rev_id;
		mac_obj_init_params.serdes_lane = params->serdes_lane;
		mac_obj_init_params.name = params->name;
		mac_obj_init_params.mac_common_regs = params->mac_common_regs;
		mac_obj_init_params.mac_common_regs_lock = params->mac_common_regs_lock;
		mac_obj_init_params.mac_common_regs_lock_handle =
			params->mac_common_regs_lock_handle;
		mac_obj_init_params.dme_conf = params->dme_conf;

		rc = al_eth_mac_handle_init(&adapter->mac_obj, &mac_obj_init_params);
		if (rc != 0)
			al_assert(0);

		adapter->mac_ints_base = al_eth_mac_iofic_regs_get(&adapter->mac_obj);
	}

	if (params->udma_regs_base) {
		/* initialize Tx udma */
		udma_params.udma_regs_base = params->udma_regs_base;
		udma_params.type = UDMA_TX;
		udma_params.num_of_queues = AL_ETH_UDMA_HW_QUEUES_BY_REV(adapter->rev_id);
		udma_params.name = "eth tx";
		rc = al_udma_handle_init(&adapter->tx_udma, &udma_params);
		if (rc != 0)
			al_warn("failed to initialize %s udma handle, error %d\n",
				 udma_params.name, rc);

		/* initialize Rx udma */
		udma_params.udma_regs_base = params->udma_regs_base;
		udma_params.type = UDMA_RX;
		udma_params.num_of_queues = AL_ETH_UDMA_HW_QUEUES_BY_REV(adapter->rev_id);
		udma_params.name = "eth rx";
		rc = al_udma_handle_init(&adapter->rx_udma, &udma_params);
		if (rc != 0)
			al_warn("failed to initialize %s udma handle, error %d\n",
				 udma_params.name, rc);
	} else {
		al_warn("%s: udma_regs_base is NULL, skipping UDMA handle initialization\n",
			__func__);
	}

	return;
}

void __iomem *al_eth_iofic_regs_base_get(struct al_hal_eth_adapter *adapter,
		enum al_eth_iofic_level level)
{
	void __iomem *iofic_regs;

	al_assert(adapter);
	al_assert(level <= AL_ETH_IOFIC_LEVEL_SECONDARY);

	iofic_regs = (level == AL_ETH_IOFIC_LEVEL_PRIMARY) ?
		(void __iomem *)adapter->ec_ints_base :
		(void __iomem *)adapter->sec_ec_ints_base;

	al_assert(iofic_regs);

	return iofic_regs;
}

void __iomem *al_eth_ec_regs_base_get(struct al_hal_eth_adapter *adapter)
{
	al_assert(adapter);

	return adapter->ec_regs_base;
}

enum al_eth_common_mode al_eth_common_mode_get(struct al_hal_eth_adapter *adapter)
{
	al_assert(adapter);

	return adapter->common_mode;
}

/*adapter management */

/**
 * EC RX FIFO thresholds
 * for Alpine V3 this needs to be aligned with the shared FIFO partitioning
 */
#define AL_ETH_EC_DATA_RXF_PART_SIZE	2048 /** Granularity of partitioning */
#define AL_ETH_EC_INFO_RXF_PART_SIZE	1024 /** Granularity of partitioning */
#define AL_ETH_EC_DATA_RXF_SIZE		20480
#define AL_ETH_EC_INFO_RXF_SIZE		10240
#define AL_ETH_EC_DATA_INFO_RXF_PARTS	(AL_ETH_EC_DATA_RXF_SIZE/AL_ETH_EC_DATA_RXF_PART_SIZE)
#define AL_ETH_EC_DATA_BUS_WIDTH	32

static void al_eth_ec_rxf_threshold_config(
	struct al_hal_eth_adapter *adapter)
{
	if (adapter->rev_id >= AL_ETH_REV_ID_4) {
		struct al_ec_regs __iomem *regs_base = adapter->ec_regs_base;
		al_bool internal_switching = adapter->internal_switching;
		uint32_t reg = 0;
		static const struct {
			unsigned int th_parts;
			unsigned int th_parts_lb;
			unsigned int num_adapters;
		} thresholds[] = {
				[AL_ETH_COMMON_MODE_4X25G] = {
					.th_parts = AL_ETH_EC_DATA_INFO_RXF_PARTS/4,
					.th_parts_lb = AL_ETH_EC_DATA_INFO_RXF_PARTS/(2*4),
					.num_adapters = 4,
				},
				[AL_ETH_COMMON_MODE_4X10G] = {
					.th_parts = AL_ETH_EC_DATA_INFO_RXF_PARTS/4,
					.th_parts_lb = AL_ETH_EC_DATA_INFO_RXF_PARTS/(2*4),
					.num_adapters = 4,
				},
				[AL_ETH_COMMON_MODE_2X50G] = {
					.th_parts = AL_ETH_EC_DATA_INFO_RXF_PARTS/2,
					.th_parts_lb = AL_ETH_EC_DATA_INFO_RXF_PARTS/(2*2),
					.num_adapters = 2,
				},
				[AL_ETH_COMMON_MODE_1X100G] = {
					.th_parts = AL_ETH_EC_DATA_INFO_RXF_PARTS/1,
					.th_parts_lb = AL_ETH_EC_DATA_INFO_RXF_PARTS/(2*1),
					.num_adapters = 1,
				},
				[AL_ETH_COMMON_MODE_1X100G_AGG] = {
					.th_parts = AL_ETH_EC_DATA_INFO_RXF_PARTS/4,
					.th_parts_lb = AL_ETH_EC_DATA_INFO_RXF_PARTS/(2*4),
					.num_adapters = 4,
				},
				[AL_ETH_COMMON_MODE_1X40G] = {
					.th_parts = AL_ETH_EC_DATA_INFO_RXF_PARTS/1,
					.th_parts_lb = AL_ETH_EC_DATA_INFO_RXF_PARTS/(2*1),
					.num_adapters = 1,
				},
		};
		uint32_t rxf_th_parts = (internal_switching) ?
				thresholds[adapter->common_mode].th_parts_lb :
				thresholds[adapter->common_mode].th_parts;
		uint32_t data_rxf_th = rxf_th_parts*AL_ETH_EC_DATA_RXF_PART_SIZE;
		uint32_t info_rxf_th = rxf_th_parts*AL_ETH_EC_INFO_RXF_PART_SIZE;

		/** assert if no part allocated */
		al_assert(rxf_th_parts);
		/** assert if allocation exceeds number of parts available */
		al_assert(rxf_th_parts*thresholds[adapter->common_mode].num_adapters <=
				AL_ETH_EC_DATA_INFO_RXF_PARTS);

		/* configure the logical size of the RX FIFO (both data and info FIFOs) */
		/* Set high threshold at -1 of fifo size to correctly signal when fifo is full */
		AL_REG_FIELD_SET(reg,
			EC_RXF_WR_FIFO_TH_INFO_MASK,
			EC_RXF_WR_FIFO_TH_INFO_SHIFT, info_rxf_th - 1);
		AL_REG_FIELD_SET(reg,
			EC_RXF_WR_FIFO_TH_DATA_MASK,
			EC_RXF_WR_FIFO_TH_DATA_SHIFT, data_rxf_th - 1);
		al_reg_write32(&regs_base->rxf.wr_fifo, reg);

		/**
		 *
		 * Set high & low sop threshold to -MAX_PACKET/BUS_WIDTH of fifo size
		 * to prevent accepting of partial packet.
		 */
		reg = 0;
		AL_REG_FIELD_SET(reg,
			EC_RXF_CFG_2_FIFO_USED_TH_L_MASK,
			EC_RXF_CFG_2_FIFO_USED_TH_L_SHIFT,
			data_rxf_th - (AL_ETH_MAX_FRAME_LEN/AL_ETH_EC_DATA_BUS_WIDTH));
		AL_REG_FIELD_SET(reg,
			EC_RXF_CFG_2_FIFO_USED_TH_H_MASK,
			EC_RXF_CFG_2_FIFO_USED_TH_H_SHIFT,
			data_rxf_th - (AL_ETH_MAX_FRAME_LEN/AL_ETH_EC_DATA_BUS_WIDTH));
		al_reg_write32(&regs_base->rxf.cfg_2, reg);

		if (internal_switching) {
			reg = 0;
			AL_REG_FIELD_SET(reg,
				EC_RXF_LB_FIFO_TH_DATA_MASK,
				EC_RXF_LB_FIFO_TH_DATA_SHIFT, data_rxf_th - 1);
			AL_REG_FIELD_SET(reg,
				EC_RXF_LB_FIFO_TH_INFO_MASK,
				EC_RXF_LB_FIFO_TH_INFO_SHIFT, info_rxf_th - 1);
			al_reg_write32(&regs_base->rxf.lb_fifo, reg);
		}
	} else if (adapter->rev_id == AL_ETH_REV_ID_3) {
		uint32_t reg = 0;
		/* configure high/low threshold values for accepting new packets */
		AL_REG_FIELD_SET(reg, EC_RXF_CFG_2_FIFO_USED_TH_H_MASK,
				EC_RXF_CFG_2_FIFO_USED_TH_H_SHIFT, 0x09C4);
		AL_REG_FIELD_SET(reg, EC_RXF_CFG_2_FIFO_USED_TH_L_MASK,
				EC_RXF_CFG_2_FIFO_USED_TH_L_SHIFT, 0x0960);
		al_reg_write32(&adapter->ec_regs_base->rxf.cfg_2, reg);

		reg = 0;
		/* configure the logical size of the RX FIFO (both data and info FIFOs) */
		AL_REG_FIELD_SET(reg, EC_RXF_WR_FIFO_TH_INFO_MASK,
				EC_RXF_WR_FIFO_TH_INFO_SHIFT, 0x01F4);
		AL_REG_FIELD_SET(reg, EC_RXF_WR_FIFO_TH_DATA_MASK,
				EC_RXF_WR_FIFO_TH_DATA_SHIFT, 0x09C4);
		al_reg_write32(&adapter->ec_regs_base->rxf.wr_fifo, reg);
	}
}

void al_eth_adpater_tables_reset(struct al_hal_eth_adapter *adapter)
{
	unsigned int i;

	if (adapter->rev_id <= AL_ETH_REV_ID_3) {
		/* init MHASH table */
		for (i = 0; i < AL_ETH_FWD_MAC_HASH_NUM; i++)
			al_eth_fwd_mhash_table_set(adapter, i, 0x0, 0);

		/* init THASH table */
		for (i = 0; i < AL_ETH_RX_THASH_TABLE_SIZE; i++)
			al_eth_thash_table_set(adapter, i, 0, 0);

		/* init vlan table */
		for (i = 0; i < AL_ETH_FWD_VID_TABLE_NUM; i++) {
			struct al_eth_fwd_vid_table_entry entry;

			al_memset(&entry, 0, sizeof(entry));
			al_eth_fwd_vid_table_set(adapter, i, &entry);
		}

		/* init control table */
		for (i = 0; i < AL_ETH_RX_CTRL_TABLE_SIZE; i++)
			al_eth_ctrl_table_raw_set(adapter, i, 0);

		/* init tx fwd vlan table */
		for (i = 0; i < AL_ETH_FWD_VID_TABLE_NUM; i++)
			al_eth_tx_fwd_vid_table_set(adapter, i, 0, AL_FALSE);
	}

	if (adapter->rev_id == AL_ETH_REV_ID_3) {
		for (i = 0; i < AL_ETH_CRC_ENGINE_V3_MAX; i++) {
			/* init TFW GCP table */
			al_eth_tx_generic_crc_table_init(adapter, i);

			/* init RFW GCP table */
			al_eth_rx_generic_crc_table_init(adapter, i);
		}

		/* init FSM table */
		for (i = 0; i < AL_ETH_RX_FSM_TABLE_SIZE; i++)
			al_eth_fsm_table_set(adapter, i, 0);
	}
}

/**
 * initialize the ethernet adapter's DMA
 */
int al_eth_adapter_init(struct al_hal_eth_adapter *adapter, struct al_eth_adapter_params *params)
{
	struct al_udma_params udma_params;
	struct al_udma_m2s_pkt_len_conf conf;
	struct al_eth_epe_handle epe_handle;
	struct al_eth_epe_init_params epe_init_params;
	int i;
	uint32_t reg;
	int rc;
	struct al_eth_adapter_handle_init_params handle_init_params = {
		.rev_id = params->rev_id, .dev_id = params->dev_id, .udma_id = params->udma_id,
		.common_mode = params->common_mode,
		.internal_switching = params->internal_switching,
		.udma_regs_base = params->udma_regs_base,
		.ec_regs_base = params->ec_regs_base,
		.mac_regs_base = params->mac_regs_base, .mac_common_regs = params->mac_common_regs,
		.mac_common_regs_lock = params->mac_common_regs_lock,
		.mac_common_regs_lock_handle = params->mac_common_regs_lock_handle,
		.name = params->name, .serdes_lane = params->serdes_lane,
		.eth_common_regs_base = params->eth_common_regs_base};

	/**
	 * Usually UDMA ID > 0 means a VF and thus no need to configure HW besides UDMA.
	 * The exception is host-driver which uses UDMA_1 but configures HW (EC)
	 */
	if ((params->udma_id > 0) && (params->force_ec_init == AL_FALSE))
		handle_init_params.vf_handle = AL_TRUE;
	else
		handle_init_params.vf_handle = AL_FALSE;

	al_dbg("eth [%s]: initialize controller's UDMA. id = %d\n", params->name, params->udma_id);
	al_dbg("eth [%s]: UDMA base regs: %p\n", params->name, params->udma_regs_base);
	al_dbg("eth [%s]: EC base regs: %p\n", params->name, params->ec_regs_base);
	al_dbg("eth [%s]: MAC base regs: %p\n", params->name, params->mac_regs_base);
	al_dbg("eth [%s]: enable_rx_parser: %x\n", params->name, params->enable_rx_parser);
	al_dbg("eth [%s]: Eth Common base regs: %p\n", params->name, params->eth_common_regs_base);

	al_eth_adapter_handle_init(adapter, &handle_init_params);
	adapter->enable_rx_parser = params->enable_rx_parser;

	al_assert(!(params->skip_adapter_init && params->force_adapter_init));

	/* if adapter handler is provided configure the adapter for PF only */
	if (params->unit_adapter && ((!params->func_num) || params->force_adapter_init)
	    && (!params->skip_adapter_init))
		al_unit_adapter_init(params->unit_adapter);
	else if (!params->unit_adapter)
		al_warn("%s: non optimal adapter configuration\n", __func__);

	/* initialize Tx udma */
	udma_params.udma_regs_base = adapter->udma_regs_base;
	udma_params.type = UDMA_TX;
	udma_params.num_of_queues = AL_ETH_UDMA_TX_QUEUES_BY_REV(adapter->rev_id);
	udma_params.name = "eth tx";
	rc = al_udma_init(&adapter->tx_udma, &udma_params);
	if (rc != 0) {
		al_err("failed to initialize %s, error %d\n",
			 udma_params.name, rc);
		return rc;
	}

	rc = al_udma_state_set_wait(&adapter->tx_udma, UDMA_NORMAL, AL_FALSE);
	if (rc != 0) {
		al_err("[%s]: failed to change state, error %d\n",
			 udma_params.name, rc);
		return rc;
	}
	/* initialize Rx udma */
	udma_params.udma_regs_base = adapter->udma_regs_base;
	udma_params.type = UDMA_RX;
	udma_params.num_of_queues = AL_ETH_UDMA_RX_QUEUES_BY_REV(adapter->rev_id);
	udma_params.name = "eth rx";
	rc = al_udma_init(&adapter->rx_udma, &udma_params);
	if (rc != 0) {
		al_err("failed to initialize %s, error %d\n",
			 udma_params.name, rc);
		return rc;
	}

	rc = al_udma_state_set_wait(&adapter->rx_udma, UDMA_NORMAL, AL_TRUE);
	if (rc != 0) {
		al_err("[%s]: failed to change state, error %d\n",
			 udma_params.name, rc);
		return rc;
	}
	al_dbg("eth [%s]: controller's UDMA successfully initialized\n",
		 params->name);

	/* set max packet size to 1M (for TSO) */
	conf.encode_64k_as_zero = AL_TRUE;
	conf.max_pkt_size = 0xfffff;
	al_udma_m2s_packet_size_cfg_set(&adapter->tx_udma, &conf);

	/* set m2s (tx) max descriptors to max data buffers number and one for
	 * meta descriptor
	 */
	al_udma_m2s_max_descs_set(&adapter->tx_udma, AL_ETH_PKT_MAX_BUFS + 1);

	/* set s2m (rx) max descriptors to max data buffers */
	al_udma_s2m_max_descs_set(&adapter->rx_udma, AL_ETH_PKT_MAX_BUFS);

	/* set s2m burst lenght when writing completion descriptors to 64 bytes
	 */
	al_udma_s2m_compl_desc_burst_config(&adapter->rx_udma, 64);

	/* Drop if no descs - min timeout to drop immediately to prevent backpressure towards MAC */
	if ((adapter->rev_id >= AL_ETH_REV_ID_4) && (adapter->dev_id == AL_ETH_DEV_ID_ADVANCED))
		al_udma_s2m_no_desc_cfg_set(&adapter->rx_udma, AL_TRUE, AL_FALSE, 1);

	/* if pointer to ec regs provided, then init the tx meta cache of this udma*/
	if (adapter->ec_regs_base != NULL) {
		/*
		 * INIT TX CACHE TABLE
		 *
		 * For a TX transaction consists of multiple TX buffer desc + metadata desc
		 * by enabling this cache we can prepare to the UDMA only 1 metadata desc
		 * (instead of preparing a meta desc for each buffer desc)
		 * This is under the assumption that all TX buffer descs share the same meta desc.
		 */

		for (i = 0; i < AL_ETH_UDMA_RX_QUEUES_BY_REV(adapter->rev_id); i++) {
			al_reg_write32(&adapter->ec_regs_base->tso.cache_table_addr,
				i + (adapter->udma_id *
				AL_ETH_UDMA_RX_QUEUES_BY_REV(adapter->rev_id)));
			al_reg_write32(&adapter->ec_regs_base->tso.cache_table_data_1, 0x00000000);
			al_reg_write32(&adapter->ec_regs_base->tso.cache_table_data_2, 0x00000000);
			al_reg_write32(&adapter->ec_regs_base->tso.cache_table_data_3, 0x00000000);
			al_reg_write32(&adapter->ec_regs_base->tso.cache_table_data_4, 0x00000000);
		}
	}

	/* VF should not init HW other than UDMA */
	if (handle_init_params.vf_handle)
		return 0;

	/* enable Ethernet controller: */
	/* enable internal machines*/
	al_reg_write32(&adapter->ec_regs_base->gen.en, 0xffffffff);
	al_reg_write32(&adapter->ec_regs_base->gen.fifo_en, 0xffffffff);

	/* enable A0 descriptor structure */
	al_reg_write32_masked(&adapter->ec_regs_base->gen.en_ext,
			      EC_GEN_EN_EXT_CACHE_WORD_SPLIT,
			      EC_GEN_EN_EXT_CACHE_WORD_SPLIT);

	/* use mss value in the descriptor */
	al_reg_write32(&adapter->ec_regs_base->tso.cfg_add_0,
					EC_TSO_CFG_ADD_0_MSS_SEL);

	/* enable tunnel TSO */
	al_reg_write32(&adapter->ec_regs_base->tso.cfg_tunnel,
					(EC_TSO_CFG_TUNNEL_EN_TUNNEL_TSO |
					 EC_TSO_CFG_TUNNEL_EN_UDP_CHKSUM |
					 EC_TSO_CFG_TUNNEL_EN_UDP_LEN |
					 EC_TSO_CFG_TUNNEL_EN_IPV6_PLEN |
					 EC_TSO_CFG_TUNNEL_EN_IPV4_CHKSUM |
					 EC_TSO_CFG_TUNNEL_EN_IPV4_IDEN |
					 EC_TSO_CFG_TUNNEL_EN_IPV4_TLEN));

	/* swap input byts from MAC RX */
	al_reg_write32(&adapter->ec_regs_base->mac.gen, 0x00000001);
	/* swap output bytes to MAC TX*/
	al_reg_write32(&adapter->ec_regs_base->tmi.tx_cfg, EC_TMI_TX_CFG_EN_FWD_TO_RX|EC_TMI_TX_CFG_SWAP_BYTES);

	/* TODO: check if we need this line*/
	al_reg_write32(&adapter->ec_regs_base->tfw_udma[0].fwd_dec, 0x000003fb);

	/* RFW configuration: default 0 */
	al_reg_write32(&adapter->ec_regs_base->rfw_default[0].opt_1, 0x00000001);

	/* VLAN table address*/
	al_reg_write32(&adapter->ec_regs_base->rfw.vid_table_addr, 0x00000000);
	/* VLAN table data*/
	al_reg_write32(&adapter->ec_regs_base->rfw.vid_table_data, 0x00000000);
	/* HASH config (select toeplitz and bits 7:0 of the thash result, enable
	 * symmetric hash) */
	al_reg_write32(&adapter->ec_regs_base->rfw.thash_cfg_1,
			EC_RFW_THASH_CFG_1_ENABLE_IP_SWAP |
			EC_RFW_THASH_CFG_1_ENABLE_PORT_SWAP);

	/* EPE initialization */
	epe_init_params.enable = params->enable_rx_parser;
	epe_init_params.init_v3_default_entries = AL_TRUE;

	/* Outer parser */
	epe_handle_init(adapter, &epe_handle, EPE_TYPE_OUTER);
	al_eth_epe_init(&epe_handle, &epe_init_params);

	/* Inner parser */
	epe_handle_init(adapter, &epe_handle, EPE_TYPE_INNER);
	al_eth_epe_init(&epe_handle, &epe_init_params);

	/* Default RX descriptor */
	al_eth_rx_desc_config(adapter,
		AL_ETH_LRO_CONTEXT_VALUE,
		AL_ETH_L4_OFFSET_OUTER,
		AL_ETH_L3_OFFSET_OUTER,
		AL_ETH_L3_L4_HEADER_INNER,
		AL_ETH_L4_DEFAULT_CHK,
		AL_ETH_L3_CHK_TYPE_0,
		AL_ETH_L3_PROTO_IDX_OUTER,
		AL_ETH_L4_PROTO_IDX_OUTER,
		AL_ETH_FRAG_OUTER);

	/* disable TSO padding and use mac padding instead */
	reg = al_reg_read32(&adapter->ec_regs_base->tso.in_cfg);
	reg &= ~0x7F00; /*clear bits 14:8 */
	al_reg_write32(&adapter->ec_regs_base->tso.in_cfg, reg);

	/* initialize eth_ctrl memories */
	al_eth_ctrl_mem_init(adapter, AL_ETH_CTRL_MEM_INIT_TX_AND_RX);

	al_eth_ec_rxf_threshold_config(adapter);

	/* Set Tx packet modification protocol number selection according to table
	 * The following table options for each bit are:
	 *	0 – from Tx buffer descriptor
	 *	1 – from protocol detect cam
	 */
	if (adapter->rev_id >= AL_ETH_REV_ID_4) {
		reg = 0;
		AL_REG_FIELD_SET(reg, EC_GEN_V3_TPM_PROTOCOL_NUM_SEL_MASK,
				EC_GEN_V3_TPM_PROTOCOL_NUM_SEL_SHIFT , 0x2);
		AL_REG_FIELD_SET(reg, EC_GEN_V3_TPM_FF_INIT_CHICKEN_MASK,
				EC_GEN_V3_TPM_FF_INIT_CHICKEN_SHIFT , 0x4);
		al_reg_write32(&adapter->ec_regs_base->gen_v3.tpm, reg);
		/* select -all- protocol from TX descriptor */
		al_reg_write32(&adapter->ec_regs_base->tfw_v3.tx_protocol_num_sel_table_1,
			0x00000000);
		al_reg_write32(&adapter->ec_regs_base->tfw_v3.tx_protocol_num_sel_table_2,
			0x00000000);
	}

	/**
	 * Addressing RMN: 11841
	 *
	 * RMN description:
	 * When TMI forwarding table is enabled, there are 2 bits that are taken from table
	 * and override completion info.
	 * There is NO way to disable the override, thus we can override
	 * valid bits in the completion.
	 *
	 * Software flow:
	 * Simple workaround is to configure offset to override above completion size
	 * (e.g. offset 40), these bits will be ignored and no harm will occur.
	 */
	if (adapter->rev_id == AL_ETH_REV_ID_4) {
		reg = 0;

		AL_REG_FIELD_SET(reg, EC_TMI_FWD_TABLE_CMPL_METADATA_FIELDS_OFFSET0_MASK,
			EC_TMI_FWD_TABLE_CMPL_METADATA_FIELDS_OFFSET0_SHIFT,
			AL_ETH_TX_TMI_FWD_TABLE_CMPL_METADATA_FIELD_OFFSET);
		AL_REG_FIELD_SET(reg, EC_TMI_FWD_TABLE_CMPL_METADATA_FIELDS_OFFSET1_MASK,
			EC_TMI_FWD_TABLE_CMPL_METADATA_FIELDS_OFFSET1_SHIFT,
			AL_ETH_TX_TMI_FWD_TABLE_CMPL_METADATA_FIELD_OFFSET);

		al_reg_write32(&adapter->ec_regs_base->tmi_fwd_table.cmpl_metadata_fields, reg);
	}

	/*
	 * init un-initialized SRAMS first
	 * before we unmask the parity error interrupts
	 */
	al_eth_adpater_tables_reset(adapter);

	if (adapter->rev_id > AL_ETH_REV_ID_3)
		al_eth_mac_error_ints_setup(adapter, params->app_int_unmask_bypass);

	al_eth_ec_error_ints_setup(adapter, params->app_int_unmask_bypass);
#ifdef AL_ETH_EX
	if (adapter->rev_id >= AL_ETH_REV_ID_4)
		al_eth_ec_ex_error_ints_setup(adapter, params->app_int_unmask_bypass);
#endif

	if (adapter->rev_id >= AL_ETH_REV_ID_4)
		al_eth_perf_mode_config(adapter, AL_ETH_PERF_MODE_DEFAULT);

	return 0;
}

void al_eth_perf_mode_config(struct al_hal_eth_adapter *adapter,
	enum al_eth_perf_mode mode)
{
	uint32_t fifo_th; /* FIFO reading threshold */
	al_bool fifo_sf_en; /* Enable FIFO store and forward */
	uint32_t reg;

	al_assert(adapter->rev_id >= AL_ETH_REV_ID_4);

	switch (mode) {
	case AL_ETH_PERF_MODE_HW_RESET_VALUE:
		fifo_th = 0x1;
		fifo_sf_en = AL_FALSE;
		break;
	case AL_ETH_PERF_MODE_A:
		fifo_th = 0x1;
		fifo_sf_en = AL_TRUE;
		break;
	default:
		al_assert_msg(AL_FALSE, "Unknown Ethernet performance mode: %d\n", mode);
		return;
	}

	/**
	 * Dirty fix to make Fortify pass.
	 * Fortify claims that this variable is written but never read, which is false claim,
	 * as it's used for a comparison operation.
	 */
	(void)fifo_sf_en;

	reg = al_reg_read32(&adapter->ec_regs_base->rxf.rd_fifo);
	AL_REG_FIELD_SET(reg,
		EC_RXF_RD_FIFO_TH_DATA_MASK,
		EC_RXF_RD_FIFO_TH_DATA_SHIFT,
		fifo_th);
	AL_REG_FIELD_SET(reg,
		EC_RXF_RD_FIFO_EN_CUT_TH_MASK,
		EC_RXF_RD_FIFO_EN_CUT_TH_SHIFT,
		(fifo_sf_en == AL_TRUE ? 0x0 : 0x3));
	al_reg_write32(&adapter->ec_regs_base->rxf.rd_fifo, reg);
}

struct al_eth_mac_obj *al_eth_mac_obj_get(struct al_hal_eth_adapter *adapter)
{
	al_assert(adapter);

	return &adapter->mac_obj;
}

unsigned int al_eth_rev_id_get(struct al_hal_eth_adapter *adapter)
{
	al_assert(adapter);

	return adapter->rev_id;
}

unsigned int al_eth_dev_id_get(struct al_hal_eth_adapter *adapter)
{
	al_assert(adapter);

	return adapter->dev_id;
}

unsigned int al_eth_get_serdes_lane(struct al_hal_eth_adapter *adapter)
{
	al_assert(adapter);

	return adapter->serdes_lane;
}

void __iomem *al_eth_epe_regs_get(struct al_hal_eth_adapter *adapter)
{
	return (adapter->rev_id <= AL_ETH_REV_ID_3 ?
		adapter->ec_regs_base :
		(void __iomem *)&adapter->ec_regs_base->rmp_cfg[0]);
}

/*****************************API Functions  **********************************/
/*adapter management */
/**
 * enable the ec and mac interrupts
 */
int al_eth_ec_mac_ints_config(struct al_hal_eth_adapter *adapter)
{

	al_dbg("eth [%s]: enable ethernet and mac interrupts\n", adapter->name);

	// only udma 0 allowed to init ec
	if (adapter->udma_id != 0)
		return -EPERM;

	/* enable mac ints */
	al_iofic_config(adapter->ec_ints_base, AL_INT_GROUP_A, 0);
	al_iofic_config(adapter->ec_ints_base, AL_INT_GROUP_B, 0);
	al_iofic_config(adapter->ec_ints_base, AL_INT_GROUP_C, 0);
	al_iofic_config(adapter->ec_ints_base, AL_INT_GROUP_D, 0);

	/* unmask MAC int */
	al_iofic_unmask(adapter->ec_ints_base, AL_INT_GROUP_A, 8);

	/* enable ec interrupts */
	al_iofic_config(adapter->mac_ints_base, AL_INT_GROUP_A, 0);
	al_iofic_config(adapter->mac_ints_base, AL_INT_GROUP_B, 0);
	al_iofic_config(adapter->mac_ints_base, AL_INT_GROUP_C, 0);
	al_iofic_config(adapter->mac_ints_base, AL_INT_GROUP_D, 0);

	/* eee active */
	al_iofic_unmask(adapter->mac_ints_base, AL_INT_GROUP_B, AL_BIT(14));

	al_iofic_unmask(adapter->udma_regs_base, AL_INT_GROUP_D, AL_BIT(11));
	return 0;
}

/**
 * ec and mac interrupt service routine
 * read and print asserted interrupts
 *
 * @param adapter pointer to the private structure
 *
 * @return 0 on success. otherwise on failure.
 */
int al_eth_ec_mac_isr(struct al_hal_eth_adapter *adapter)
{
	uint32_t cause;
	al_dbg("[%s]: ethernet interrupts handler\n", adapter->name);

	// only udma 0 allowed to init ec
	if (adapter->udma_id != 0)
		return -EPERM;

	/* read ec cause */
	cause = al_iofic_read_cause(adapter->ec_ints_base, AL_INT_GROUP_A);
	al_dbg("[%s]: ethernet group A cause 0x%08x\n", adapter->name, cause);
	if (cause & 1)
	{
		cause = al_iofic_read_cause(adapter->mac_ints_base, AL_INT_GROUP_A);
		al_dbg("[%s]: mac group A cause 0x%08x\n", adapter->name, cause);

		cause = al_iofic_read_cause(adapter->mac_ints_base, AL_INT_GROUP_B);
		al_dbg("[%s]: mac group B cause 0x%08x\n", adapter->name, cause);

		cause = al_iofic_read_cause(adapter->mac_ints_base, AL_INT_GROUP_C);
		al_dbg("[%s]: mac group C cause 0x%08x\n", adapter->name, cause);

		cause = al_iofic_read_cause(adapter->mac_ints_base, AL_INT_GROUP_D);
		al_dbg("[%s]: mac group D cause 0x%08x\n", adapter->name, cause);
	}
	cause = al_iofic_read_cause(adapter->ec_ints_base, AL_INT_GROUP_B);
	al_dbg("[%s]: ethernet group B cause 0x%08x\n", adapter->name, cause);
	cause = al_iofic_read_cause(adapter->ec_ints_base, AL_INT_GROUP_C);
	al_dbg("[%s]: ethernet group C cause 0x%08x\n", adapter->name, cause);
	cause = al_iofic_read_cause(adapter->ec_ints_base, AL_INT_GROUP_D);
	al_dbg("[%s]: ethernet group D cause 0x%08x\n", adapter->name, cause);

	return 0;
}

/**
 * stop the DMA of the ethernet adapter
 */
int al_eth_adapter_stop(struct al_hal_eth_adapter *adapter)
{
	int rc;

	al_dbg("eth [%s]: stop controller's UDMA\n", adapter->name);

	/* disable Tx dma*/
	rc = al_udma_state_set_wait(&adapter->tx_udma, UDMA_DISABLE, AL_FALSE);
	if (rc != 0) {
		al_warn("[%s] warn: failed to change state, error %d\n",
			 adapter->tx_udma.name, rc);
		return rc;
	}

	al_dbg("eth [%s]: controller's TX UDMA stopped\n",
		 adapter->name);
	/* disable Rx dma*/
	rc = al_udma_state_set_wait(&adapter->rx_udma, UDMA_DISABLE, AL_TRUE);
	if (rc != 0) {
		al_warn("[%s] warn: failed to change state, error %d\n",
			 adapter->rx_udma.name, rc);
		return rc;
	}

	al_dbg("eth [%s]: controller's RX UDMA stopped\n",
		 adapter->name);
	return 0;
}

int al_eth_adapter_reset(struct al_hal_eth_adapter *adapter)
{
	al_dbg("eth [%s]: reset controller's UDMA\n", adapter->name);

	return -EPERM;
}

/* Q management */
/**
 * Configure and enable a queue ring
 */
int al_eth_queue_config(struct al_hal_eth_adapter *adapter, enum al_udma_type type, uint32_t qid,
			     struct al_udma_q_params *q_params)
{
	struct al_udma *udma;
	int rc;

	al_dbg("eth [%s]: config UDMA %s queue %d\n", adapter->name,
		 type == UDMA_TX ? "Tx" : "Rx\n", qid);

	if (type == UDMA_TX) {
		udma = &adapter->tx_udma;
	} else {
		udma = &adapter->rx_udma;
	}

	q_params->adapter_rev_id = adapter->rev_id;

	rc = al_udma_q_init(udma, qid, q_params);

	if (rc)
		return rc;

	if (type == UDMA_RX) {
		rc = al_udma_s2m_q_compl_coal_config(&udma->udma_q[qid],
				AL_TRUE, AL_ETH_S2M_UDMA_COMP_COAL_TIMEOUT);

		al_assert(q_params->cdesc_size <= 32);

		if (q_params->cdesc_size > 16)
			al_reg_write32_masked(&adapter->ec_regs_base->rfw.out_cfg,
					EC_RFW_OUT_CFG_META_CNT_MASK, 2);
	}
	return rc;
}

int al_eth_queue_enable(struct al_hal_eth_adapter *adapter __attribute__((__unused__)),
			enum al_udma_type type __attribute__((__unused__)),
			uint32_t qid __attribute__((__unused__)))
{
	return -EPERM;
}
int al_eth_queue_disable(struct al_hal_eth_adapter *adapter __attribute__((__unused__)),
			 enum al_udma_type type __attribute__((__unused__)),
			 uint32_t qid __attribute__((__unused__)))
{
	return -EPERM;
}

/* MAC layer */
int al_eth_rx_pkt_limit_config(struct al_hal_eth_adapter *adapter, uint32_t min_rx_len, uint32_t max_rx_len)
{
	int ret;

	al_assert(max_rx_len <= AL_ETH_MAX_FRAME_LEN);

	/* EC minimum packet length [bytes] in RX */
	al_reg_write32(&adapter->ec_regs_base->mac.min_pkt, min_rx_len);
	/* EC maximum packet length [bytes] in RX */
	al_reg_write32(&adapter->ec_regs_base->mac.max_pkt, max_rx_len);

	/** Perform MAC config section */
	ret = adapter->mac_obj.rx_pkt_limit_config(&adapter->mac_obj, min_rx_len, max_rx_len);

	return ret;
}

int al_eth_capabilities_get(struct al_hal_eth_adapter *adapter, struct al_eth_capabilities *caps)
{
	al_assert(caps);

	caps->speed_10_HD = AL_FALSE;
	caps->speed_10_FD = AL_FALSE;
	caps->speed_100_HD = AL_FALSE;
	caps->speed_100_FD = AL_FALSE;
	caps->speed_1000_HD = AL_FALSE;
	caps->speed_1000_FD = AL_FALSE;
	caps->speed_10000_HD = AL_FALSE;
	caps->speed_10000_FD = AL_FALSE;
	caps->pfc = AL_FALSE;
	caps->eee = AL_FALSE;

	switch (adapter->mac_mode) {
		case AL_ETH_MAC_MODE_RGMII:
		case AL_ETH_MAC_MODE_SGMII:
			caps->speed_10_HD = AL_TRUE;
			caps->speed_10_FD = AL_TRUE;
			caps->speed_100_HD = AL_TRUE;
			caps->speed_100_FD = AL_TRUE;
			caps->speed_1000_FD = AL_TRUE;
			caps->eee = AL_TRUE;
			break;
		case AL_ETH_MAC_MODE_10GbE_Serial:
			caps->speed_10000_FD = AL_TRUE;
			caps->pfc = AL_TRUE;
			break;
		default:
		al_err("Eth: unsupported MAC mode %d\n", adapter->mac_mode);
		return -EPERM;
	}
	return 0;
}

static void al_dump_tx_desc(union al_udma_desc *tx_desc)
{
	uint32_t *ptr = (uint32_t *)tx_desc;
	al_dbg("eth tx desc:\n");
	al_dbg("0x%08x\n", *(ptr++));
	al_dbg("0x%08x\n", *(ptr++));
	al_dbg("0x%08x\n", *(ptr++));
	al_dbg("0x%08x\n", *(ptr++));
}

void al_eth_tso_metacahe_line_dump(struct al_hal_eth_adapter *adapter,
	unsigned int idx, uint32_t *arr)
{
	uint32_t data[AL_ETH_TSO_METACACHE_LINE_SIZE];
	unsigned int i;

	al_assert(adapter);
	al_assert(idx < AL_ETH_UDMA_RX_QUEUES_BY_REV(adapter->rev_id));

	if (al_udma_state_get(&adapter->tx_udma) != UDMA_IDLE)
		al_warn("%s: %s TX udma is not idle, incorrect data maybe read\n",
			__func__, adapter->name);

	al_dbg("%s: Entry idx: %d\n", __func__, idx);
	al_reg_write32(&adapter->ec_regs_base->tso.cache_table_addr, (uint32_t)idx);

	for (i = 0; i < AL_ARR_SIZE(data); i++) {
		data[i] = al_reg_read32(&adapter->ec_regs_base->tso.cache_table_data_1 + i);
		al_dbg("%s: cache_table_data_%d:0x%08x\n", __func__, i, data[i]);

		if (arr)
			arr[i] = data[i];
	}
}

static void
al_dump_tx_pkt(struct al_udma_q *tx_dma_q, struct al_eth_pkt *pkt)
{
	const char *tso = (pkt->flags & AL_ETH_TX_FLAGS_TSO) ? "TSO" : "";
	const char *l3_csum = (pkt->flags & AL_ETH_TX_FLAGS_IPV4_L3_CSUM) ? "L3 CSUM" : "";
	const char *l4_csum = (pkt->flags & AL_ETH_TX_FLAGS_L4_CSUM) ?
	  ((pkt->flags & AL_ETH_TX_FLAGS_L4_PARTIAL_CSUM) ? "L4 PARTIAL CSUM" : "L4 FULL CSUM") : "";
	const char *packet_drop = (pkt->flags & AL_ETH_TX_FLAGS_PACKET_DROP) ? "PACKET DROP" : "";
	const char *fcs = (pkt->flags & AL_ETH_TX_FLAGS_L2_DIS_FCS) ? "Disable FCS" : "";
	const char *ptp = (pkt->flags & AL_ETH_TX_FLAGS_TS) ? "TX_PTP" : "";
	const char *l3_proto_name = "unknown";
	const char *l4_proto_name = "unknown";
	const char *outer_l3_proto_name = "N/A";
	const char *tunnel_mode = (pkt->tunnel_mode == AL_ETH_TUNNEL_WITH_UDP) ?
				  "TUNNEL_WITH_UDP" :
				  (pkt->tunnel_mode == AL_ETH_TUNNEL_NO_UDP) ?
				  "TUNNEL_NO_UDP" :
				  "NO_TUNNELING";
	uint32_t total_len = 0;
	int i;

	al_dbg("[%s %d]: flags: %s %s %s %s %s %s %s\n", tx_dma_q->udma->name, tx_dma_q->qid,
			tso, l3_csum, l4_csum, packet_drop, fcs, ptp, tunnel_mode);

	switch (pkt->l3_proto_idx) {
	case AL_ETH_PROTO_ID_IPv4:
		l3_proto_name = "IPv4";
		break;
	case AL_ETH_PROTO_ID_IPv6:
		l3_proto_name = "IPv6";
		break;
	default:
		l3_proto_name = "unknown";
		break;
	}

	switch (pkt->l4_proto_idx) {
	case AL_ETH_PROTO_ID_TCP:
		l4_proto_name = "TCP";
		break;
	case AL_ETH_PROTO_ID_UDP:
		l4_proto_name = "UDP";
		break;
	default:
		l4_proto_name = "unknown";
		break;
	}

	switch (pkt->outer_l3_proto_idx) {
	case AL_ETH_PROTO_ID_IPv4:
		outer_l3_proto_name = "IPv4";
		break;
	case AL_ETH_PROTO_ID_IPv6:
		outer_l3_proto_name = "IPv6";
		break;
	default:
		outer_l3_proto_name = "N/A";
		break;
	}

	al_dbg("[%s %d]: L3 proto: %d (%s). L4 proto: %d (%s). Outer_L3 proto: %d (%s). vlan source count %d. mod add %d. mod del %d\n",
			tx_dma_q->udma->name, tx_dma_q->qid, pkt->l3_proto_idx,
			l3_proto_name, pkt->l4_proto_idx, l4_proto_name,
			pkt->outer_l3_proto_idx, outer_l3_proto_name,
			pkt->source_vlan_count, pkt->vlan_mod_add_count,
			pkt->vlan_mod_del_count);

	if (pkt->meta) {
		const char * store = pkt->meta->store ? "Yes" : "No";
		const char *ptp_val = (pkt->flags & AL_ETH_TX_FLAGS_TS) ? "Yes" : "No";

		al_dbg("[%s %d]: tx pkt with meta data. words valid %x\n",
			tx_dma_q->udma->name, tx_dma_q->qid,
			pkt->meta->words_valid);
		al_dbg("[%s %d]: meta: store to cache %s. l3 hdr len %d. l3 hdr offset %d. "
			"l4 hdr len %d. mss val %d ts_index %d ts_val:%s\n"
			, tx_dma_q->udma->name, tx_dma_q->qid, store,
			pkt->meta->l3_header_len, pkt->meta->l3_header_offset,
			pkt->meta->l4_header_len, pkt->meta->mss_val,
			pkt->meta->ts_index, ptp_val);
		al_dbg("outer_l3_hdr_offset %d. outer_l3_len %d.\n",
			pkt->meta->outer_l3_offset, pkt->meta->outer_l3_len);
	}

	al_dbg("[%s %d]: num of bufs: %d\n", tx_dma_q->udma->name, tx_dma_q->qid,
		pkt->num_of_bufs);
	for (i = 0; i < pkt->num_of_bufs; i++) {
		al_dbg("eth [%s %d]: buf[%d]: len 0x%08x."
				" address 0x%016" PRIx64 "\n",
				tx_dma_q->udma->name, tx_dma_q->qid,
				i, pkt->bufs[i].len, pkt->bufs[i].addr);
		total_len += pkt->bufs[i].len;
	}
	al_dbg("[%s %d]: total len: 0x%08x\n", tx_dma_q->udma->name, tx_dma_q->qid, total_len);

}

/* TX */
/**
 * add packet to transmission queue
 */
int al_eth_tx_pkt_prepare(struct al_udma_q *tx_dma_q, struct al_eth_pkt *pkt)
{
	union al_udma_desc *tx_desc;
	uint32_t tx_descs;
	uint32_t flags = AL_M2S_DESC_FIRST |
			AL_M2S_DESC_CONCAT |
			(pkt->flags & AL_ETH_TX_FLAGS_INT);
	uint64_t tgtid = ((uint64_t)pkt->tgtid) << AL_UDMA_DESC_TGTID_SHIFT;
	uint32_t meta_ctrl;
	uint32_t ring_id;
	int buf_idx;

	al_dbg("[%s %d]: new tx pkt\n", tx_dma_q->udma->name, tx_dma_q->qid);

	al_dump_tx_pkt(tx_dma_q, pkt);

	tx_descs = pkt->num_of_bufs;
	if (pkt->meta) {
		tx_descs += 1;
	}
#ifdef AL_ETH_EX_HAS_EXT_METADATA
	al_assert((pkt->ext_meta_data == NULL) || (tx_dma_q->adapter_rev_id > AL_ETH_REV_ID_2));

	tx_descs += al_eth_ext_metadata_needed_descs(pkt->ext_meta_data);
	al_dbg("[%s %d]: %d Descriptors: ext_meta (%d). meta (%d). buffer (%d)\n",
			tx_dma_q->udma->name, tx_dma_q->qid, tx_descs,
			al_eth_ext_metadata_needed_descs(pkt->ext_meta_data),
			(pkt->meta != NULL), pkt->num_of_bufs);
#endif

	if (unlikely(al_udma_available_get(tx_dma_q) < tx_descs)) {
		al_dbg("[%s %d]: failed to allocate (%d) descriptors\n",
			 tx_dma_q->udma->name, tx_dma_q->qid, tx_descs);
		return 0;
	}

#ifdef AL_ETH_EX_HAS_EXT_METADATA
	if (pkt->ext_meta_data != NULL) {
		al_eth_ext_metadata_create(tx_dma_q, &flags, pkt->ext_meta_data);
		flags &= ~(AL_M2S_DESC_FIRST | AL_ETH_TX_FLAGS_INT);
	}
#endif

	if (pkt->meta) {
		uint32_t meta_words[4] = { 0, 0, 0, 0 };

		meta_words[0] |= flags | AL_M2S_DESC_META_DATA;
		meta_words[0] &=  ~AL_M2S_DESC_CONCAT;
		flags &= ~(AL_M2S_DESC_FIRST | AL_ETH_TX_FLAGS_INT);

		tx_desc = al_udma_desc_get(tx_dma_q);
		/* get ring id, and clear FIRST and Int flags */
		ring_id = al_udma_ring_id_get(tx_dma_q) <<
			AL_M2S_DESC_RING_ID_SHIFT;

		meta_words[0] |= ring_id;
		meta_words[0] |= pkt->meta->words_valid << 12;

		if (pkt->meta->store)
			meta_words[0] |= AL_ETH_TX_META_STORE;

		if (tx_dma_q->adapter_rev_id < AL_ETH_REV_ID_4) {
			if (pkt->meta->words_valid & 1) {
				meta_words[0] |= pkt->meta->vlan1_cfi_sel;
				meta_words[0] |= pkt->meta->vlan2_vid_sel << 2;
				meta_words[0] |= pkt->meta->vlan2_cfi_sel << 4;
				meta_words[0] |= pkt->meta->vlan2_pbits_sel << 6;
				meta_words[0] |= pkt->meta->vlan2_ether_sel << 8;
			}

			if (pkt->meta->words_valid & 2) {
				meta_words[1] = pkt->meta->vlan1_new_vid;
				meta_words[1] |= pkt->meta->vlan1_new_cfi << 12;
				meta_words[1] |= pkt->meta->vlan1_new_pbits << 13;
				meta_words[1] |= pkt->meta->vlan2_new_vid << 16;
				meta_words[1] |= pkt->meta->vlan2_new_cfi << 28;
				meta_words[1] |= pkt->meta->vlan2_new_pbits << 29;
			}
		}

		if (pkt->meta->words_valid & 4) {
			uint32_t l3_offset;

			/**
			 * Addressing RMN: 11279
			 *
			 * RMN description:
			 * L3_offset field size in the TX_meta_descriptor is only 8 bits
			 * The L3_offset that the L3/L4 checksum engine receiving is:
			 * L3_offset field + source_vlan_count field * 4 (calculated in rtl)
			 * and it's also limited by 8 bits
			 * As a result, we can't use the L3/L4 checksum engine for
			 * L3/L4 checksum offload if the sum expression from above is
             * larger than 255.
			 *
			 * Software flow:
			 * in case of checksum_offload - Sum l3_offset and source_vlan size and assert if needed
			 */
			if (unlikely((((pkt->meta->l3_header_offset) + (uint32_t)(pkt->source_vlan_count << 2)) >
				AL_ETH_TX_CHECKSUM_ENGINE_L3_OFFSET_MAX) &&
				((pkt->flags & AL_ETH_TX_FLAGS_L4_CSUM) || (pkt->flags & AL_ETH_TX_FLAGS_IPV4_L3_CSUM)))) {
				al_err("%s : L3 header offset max size is %lu", __func__,
					AL_ETH_TX_CHECKSUM_ENGINE_L3_OFFSET_MAX);
				al_assert(0);
			}

			meta_words[2] |= ((pkt->meta->l3_header_len & AL_ETH_TX_META_L3_LEN_MASK) <<
					  AL_ETH_TX_META_L3_LEN_SHIFT);
			meta_words[2] |= ((pkt->meta->l3_header_offset &
					   AL_ETH_TX_META_L3_OFF_MASK) <<
					  AL_ETH_TX_META_L3_OFF_SHIFT);
			meta_words[2] |= ((pkt->meta->l4_header_len & AL_ETH_TX_META_L4_LEN_MASK) <<
					  AL_ETH_TX_META_L4_LEN_SHIFT);

			/**
			 * Addressing RMN: 11632
			 *
			 * RMN description:
			 * TSO operation is limited to use MSS value of DATA_WIDTH and greater
			 * For Alpine v2 the limitation is 16B, and for Alpine v3 it's 32B.
			 *
			 * Software flow:
			 * Don't support MSS values smaller than 64B for TSO (we harden the limit
			 * to 64B)
			 */
			al_assert_msg(likely((pkt->meta->mss_val == 0) ||
				      ((pkt->meta->mss_val >= AL_ETH_TSO_MSS_MIN_VAL) &&
				       (pkt->meta->mss_val <= AL_ETH_TSO_MSS_MAX_VAL))),
				      "[%s %d]: Valid MSS range is [%dB-%dB] for TSO packet! requested mss_val:%u\n",
				      tx_dma_q->udma->name, tx_dma_q->qid, AL_ETH_TSO_MSS_MIN_VAL,
				      AL_ETH_TSO_MSS_MAX_VAL, pkt->meta->mss_val);

			if (unlikely(pkt->flags & AL_ETH_TX_FLAGS_TS))
				meta_words[0] |= pkt->meta->ts_index <<
					AL_ETH_TX_META_MSS_MSB_TS_VAL_SHIFT;
			else
				meta_words[0] |= (((pkt->meta->mss_val & 0x3c00) >> 10)
						<< AL_ETH_TX_META_MSS_MSB_TS_VAL_SHIFT);
			meta_words[2] |= ((pkt->meta->mss_val & 0x03ff)
					<< AL_ETH_TX_META_MSS_LSB_VAL_SHIFT);

			/*
			 * move from bytes to multiplication of 2 as the HW
			 * expect to get it
			 */
			l3_offset = (pkt->meta->outer_l3_offset >> 1);

			meta_words[0] |=
				(((l3_offset &
				   AL_ETH_TX_META_OUTER_L3_OFF_HIGH_MASK) >> 3)
				   << AL_ETH_TX_META_OUTER_L3_OFF_HIGH_SHIFT);

			meta_words[3] |=
				((l3_offset &
				   AL_ETH_TX_META_OUTER_L3_OFF_LOW_MASK)
				   << AL_ETH_TX_META_OUTER_L3_OFF_LOW_SHIFT);

			/*
			 * shift right 2 bits to work in multiplication of 4
			 * as the HW expect to get it
			 */
			meta_words[3] |=
				(((pkt->meta->outer_l3_len >> 2) &
				   AL_ETH_TX_META_OUTER_L3_LEN_MASK)
				   << AL_ETH_TX_META_OUTER_L3_LEN_SHIFT);
		}

#if defined(AL_ETH_EX_HAS_EXT_METADATA) && (AL_DEV_ID >= AL_DEV_ID_ALPINE_V3)
		if (pkt->tx_meta_proprietary)
			al_eth_tx_metadata_create_proprietary(pkt->tx_meta_proprietary, meta_words);
#endif

		tx_desc->tx_meta.len_ctrl = swap32_to_le(meta_words[0]);
		tx_desc->tx_meta.meta_ctrl = swap32_to_le(meta_words[1]);
		tx_desc->tx_meta.meta1 = swap32_to_le(meta_words[2]);
		tx_desc->tx_meta.meta2 = swap32_to_le(meta_words[3]);
		al_dump_tx_desc(tx_desc);
	}

	meta_ctrl = pkt->flags & AL_ETH_TX_PKT_META_FLAGS;

	/* L4_PARTIAL_CSUM without L4_CSUM is invalid option  */
	al_assert((pkt->flags & (AL_ETH_TX_FLAGS_L4_CSUM|AL_ETH_TX_FLAGS_L4_PARTIAL_CSUM)) !=
		  AL_ETH_TX_FLAGS_L4_PARTIAL_CSUM);

	/* TSO packets can't have Timestamp enabled */
	al_assert((pkt->flags & (AL_ETH_TX_FLAGS_TSO|AL_ETH_TX_FLAGS_TS)) !=
		  (AL_ETH_TX_FLAGS_TSO|AL_ETH_TX_FLAGS_TS));

	meta_ctrl |= pkt->l3_proto_idx;
	meta_ctrl |= pkt->l4_proto_idx << AL_ETH_TX_L4_PROTO_IDX_SHIFT;
	meta_ctrl |= pkt->source_vlan_count << AL_ETH_TX_SRC_VLAN_CNT_SHIFT;

	if (tx_dma_q->adapter_rev_id < AL_ETH_REV_ID_4) {
		meta_ctrl |= pkt->vlan_mod_add_count << AL_ETH_TX_VLAN_MOD_ADD_SHIFT;
		meta_ctrl |= pkt->vlan_mod_del_count << AL_ETH_TX_VLAN_MOD_DEL_SHIFT;
		meta_ctrl |= pkt->vlan_mod_v1_ether_sel << AL_ETH_TX_VLAN_MOD_E_SEL_SHIFT;
		meta_ctrl |= pkt->vlan_mod_v1_vid_sel << AL_ETH_TX_VLAN_MOD_VID_SEL_SHIFT;
		meta_ctrl |= pkt->vlan_mod_v1_pbits_sel << AL_ETH_TX_VLAN_MOD_PBIT_SEL_SHIFT;
	} else {
		meta_ctrl |= pkt->gen_crc_checksum_en_mask << AL_ETH_TX_GEN_CRC_CHECKSUM_EN_SHIFT;
		meta_ctrl |= pkt->protocol_number << AL_ETH_TX_PROTOCOL_NUM_SHIFT;
	}

#if defined(AL_ETH_EX_HAS_EXT_METADATA) && (AL_DEV_ID == AL_DEV_ID_ALPINE_V2)
	if ((pkt->ext_meta_data != NULL) && (pkt->ext_meta_data->tx_crypto_data != NULL))
		meta_ctrl |= AL_ETH_TX_FLAGS_ENCRYPT;
#endif

	meta_ctrl |= pkt->tunnel_mode << AL_ETH_TX_TUNNEL_MODE_SHIFT;
	if (pkt->outer_l3_proto_idx == AL_ETH_PROTO_ID_IPv4)
		meta_ctrl |= 1 << AL_ETH_TX_OUTER_L3_PROTO_SHIFT;

	flags |= pkt->flags & AL_ETH_TX_PKT_UDMA_FLAGS;
	for(buf_idx = 0; buf_idx < pkt->num_of_bufs; buf_idx++ ) {
		uint32_t flags_len = flags;

		tx_desc = al_udma_desc_get(tx_dma_q);
		/* get ring id, and clear FIRST and Int flags */
		ring_id = al_udma_ring_id_get(tx_dma_q) <<
			AL_M2S_DESC_RING_ID_SHIFT;

		flags_len |= ring_id;

		if (buf_idx == (pkt->num_of_bufs - 1))
			flags_len |= AL_M2S_DESC_LAST;

		/* clear First and Int flags */
		flags &= AL_ETH_TX_FLAGS_NO_SNOOP;
		flags |= AL_M2S_DESC_CONCAT;

		flags_len |= pkt->bufs[buf_idx].len & AL_M2S_DESC_LEN_MASK;
		tx_desc->tx.len_ctrl = swap32_to_le(flags_len);
		if (buf_idx == 0)
			tx_desc->tx.meta_ctrl = swap32_to_le(meta_ctrl);
		tx_desc->tx.buf_ptr = swap64_to_le(
			pkt->bufs[buf_idx].addr | tgtid);
		al_dump_tx_desc(tx_desc);
	}

	al_dbg("[%s %d]: pkt descriptors written into the tx queue. descs num (%d)\n",
		tx_dma_q->udma->name, tx_dma_q->qid, tx_descs);

	return tx_descs;
}


void al_eth_tx_dma_action(struct al_udma_q *tx_dma_q, uint32_t tx_descs)
{
	/* add tx descriptors */
	al_udma_desc_action_add(tx_dma_q, tx_descs);
}

/**
 * get number of completed tx descriptors, upper layer should derive from
 */
int al_eth_comp_tx_get(struct al_udma_q *tx_dma_q)
{
	int rc;

	rc = al_udma_cdesc_get_all(tx_dma_q, NULL);
	if (rc != 0) {
		al_udma_cdesc_ack(tx_dma_q, rc);
		al_dbg("[%s %d]: tx completion: descs (%d)\n",
			 tx_dma_q->udma->name, tx_dma_q->qid, rc);
	}

	return rc;
}

/* RX */
/**
 * config the rx descriptor fields
 */
void al_eth_rx_desc_config(
			struct al_hal_eth_adapter *adapter,
			enum al_eth_rx_desc_lro_context_val_res lro_sel,
			enum al_eth_rx_desc_l4_offset_sel l4_offset_sel,
			enum al_eth_rx_desc_l3_offset_sel l3_offset_sel,
			enum al_eth_rx_desc_l3_l4_default_chk l3_l4_default_sel,
			enum al_eth_rx_desc_l4_chk_res_sel l4_sel,
			enum al_eth_rx_desc_l3_chk_res_sel l3_sel,
			enum al_eth_rx_desc_l3_proto_idx_sel l3_proto_sel,
			enum al_eth_rx_desc_l4_proto_idx_sel l4_proto_sel,
			enum al_eth_rx_desc_frag_sel frag_sel)
{
	uint32_t reg_val = 0;

	reg_val |= (lro_sel == AL_ETH_L4_OFFSET) ?
			EC_RFW_CFG_A_0_LRO_CONTEXT_SEL : 0;

	reg_val |= (l4_sel == AL_ETH_L4_INNER_OUTER_CHK) ?
			EC_RFW_CFG_A_0_META_L4_CHK_RES_SEL : 0;

	reg_val |= l3_sel << EC_RFW_CFG_A_0_META_L3_CHK_RES_SEL_SHIFT;

	al_reg_write32(&adapter->ec_regs_base->rfw.cfg_a_0, reg_val);

	reg_val = al_reg_read32(&adapter->ec_regs_base->rfw.checksum);

	if (l3_l4_default_sel == AL_ETH_L3_L4_HEADER_INNER)
		reg_val |= EC_RFW_CHECKSUM_HDR_SEL;
	else
		reg_val &= ~EC_RFW_CHECKSUM_HDR_SEL;

	al_reg_write32(&adapter->ec_regs_base->rfw.checksum, reg_val);

	reg_val = al_reg_read32(&adapter->ec_regs_base->rfw.meta);
	if (l3_proto_sel == AL_ETH_L3_PROTO_IDX_INNER)
		reg_val |= EC_RFW_META_L3_PROT_SEL;
	else
		reg_val &= ~EC_RFW_META_L3_PROT_SEL;

	if (l4_proto_sel == AL_ETH_L4_PROTO_IDX_INNER)
		reg_val |= EC_RFW_META_L4_PROT_SEL;
	else
		reg_val &= ~EC_RFW_META_L4_PROT_SEL;

	if (l4_offset_sel == AL_ETH_L4_OFFSET_INNER)
		reg_val |= EC_RFW_META_L4_OFFSET_SEL;
	else
		reg_val &= ~EC_RFW_META_L4_OFFSET_SEL;

	if (l3_offset_sel == AL_ETH_L3_OFFSET_INNER)
		reg_val |= EC_RFW_META_L3_OFFSET_SEL;
	else
		reg_val &= ~EC_RFW_META_L3_OFFSET_SEL;

	if (frag_sel == AL_ETH_FRAG_INNER)
		reg_val |= EC_RFW_META_FRAG_SEL;
	else
		reg_val &= ~EC_RFW_META_FRAG_SEL;

	/* Use parser output to calculate the L3 header length when L3 is IPv6 (#40) */
	reg_val |= EC_RFW_META_L3_LEN_CALC;

	al_reg_write32(&adapter->ec_regs_base->rfw.meta, reg_val);
}

/**
 * Configure RX header split
 */
int al_eth_rx_header_split_config(struct al_hal_eth_adapter *adapter, al_bool enable, uint32_t header_len)
{
	uint32_t	reg;

	reg = al_reg_read32(&adapter->ec_regs_base->rfw.hdr_split);
	if (enable == AL_TRUE)
		reg |= EC_RFW_HDR_SPLIT_EN;
	else
		reg &= ~EC_RFW_HDR_SPLIT_EN;

	AL_REG_FIELD_SET(reg, EC_RFW_HDR_SPLIT_DEF_LEN_MASK, EC_RFW_HDR_SPLIT_DEF_LEN_SHIFT, header_len);
	al_reg_write32(&adapter->ec_regs_base->rfw.hdr_split, reg);
	return 0;
}


/**
 * enable / disable header split in the udma queue.
 * length will be taken from the udma configuration to enable different length per queue.
 */
int al_eth_rx_header_split_force_len_config(struct al_hal_eth_adapter *adapter,
					al_bool enable,
					uint32_t qid,
					uint32_t header_len)
{
	al_udma_s2m_q_compl_hdr_split_config(&(adapter->rx_udma.udma_q[qid]), enable,
					     AL_TRUE, header_len);

	return 0;
}


/**
 * add buffer to receive queue
 */
int al_eth_rx_buffer_add(struct al_udma_q *rx_dma_q,
			      struct al_buf *buf, uint32_t flags,
			      struct al_buf *header_buf)
{
	uint64_t tgtid = ((uint64_t)flags & AL_ETH_RX_FLAGS_TGTID_MASK) <<
		AL_UDMA_DESC_TGTID_SHIFT;
	uint32_t flags_len = flags & ~AL_ETH_RX_FLAGS_TGTID_MASK;
	union al_udma_desc *rx_desc;

	al_dbg("[%s %d]: add rx buffer.\n", rx_dma_q->udma->name, rx_dma_q->qid);

#if 1
	if (unlikely(al_udma_available_get(rx_dma_q) < 1)) {
		al_dbg("[%s]: rx q (%d) has no enough free descriptor\n",
			 rx_dma_q->udma->name, rx_dma_q->qid);
		return -ENOSPC;
	}
#endif
	rx_desc = al_udma_desc_get(rx_dma_q);

	flags_len |= al_udma_ring_id_get(rx_dma_q) << AL_S2M_DESC_RING_ID_SHIFT;
	flags_len |= buf->len & AL_S2M_DESC_LEN_MASK;

	if (flags & AL_S2M_DESC_DUAL_BUF) {
		al_assert(header_buf != NULL); /*header valid in dual buf */
		al_assert((rx_dma_q->udma->rev_id >= AL_UDMA_REV_ID_2) ||
			(AL_ADDR_HIGH(buf->addr) == AL_ADDR_HIGH(header_buf->addr)));

		flags_len |= ((header_buf->len >> AL_S2M_DESC_LEN2_GRANULARITY_SHIFT)
			<< AL_S2M_DESC_LEN2_SHIFT) & AL_S2M_DESC_LEN2_MASK;
		rx_desc->rx.buf2_ptr_lo = swap32_to_le(AL_ADDR_LOW(header_buf->addr));
	}
	rx_desc->rx.len_ctrl = swap32_to_le(flags_len);
	rx_desc->rx.buf1_ptr = swap64_to_le(buf->addr | tgtid);

	return 0;
}

/**
 * notify the hw engine about rx descriptors that were added to the receive queue
 */
void al_eth_rx_buffer_action(struct al_udma_q *rx_dma_q, uint32_t descs_num)
{
	al_dbg("[%s]: update the rx engine tail pointer: queue %d. descs %d\n",
		 rx_dma_q->udma->name, rx_dma_q->qid, descs_num);

	/* add rx descriptor */
	al_udma_desc_action_add(rx_dma_q, descs_num);
}

/**
 * get packet from RX completion ring
 */
uint32_t al_eth_pkt_rx(struct al_udma_q *rx_dma_q,
			      struct al_eth_pkt *pkt)
{
	volatile union al_udma_cdesc *cdesc;
	volatile al_eth_rx_cdesc *rx_desc;
	uint32_t i;
	uint32_t rc;

	rc = al_udma_cdesc_packet_get(rx_dma_q, &cdesc);
	if (rc == 0)
		return 0;

	al_assert(rc <= AL_ETH_PKT_MAX_BUFS);

	al_dbg("[%s]: fetch rx packet: queue %d.\n",
		 rx_dma_q->udma->name, rx_dma_q->qid);

	pkt->rx_header_len = 0;
	for (i = 0; i < rc; i++) {
		uint32_t buf1_len, buf2_len;

		/* get next descriptor */
		rx_desc = (volatile al_eth_rx_cdesc *)al_cdesc_next(rx_dma_q, cdesc, i);

		buf1_len = swap32_from_le(rx_desc->len);

		if ((i == 0) && (swap32_from_le(rx_desc->word2) &
			AL_UDMA_CDESC_BUF2_USED)) {
			buf2_len = swap32_from_le(rx_desc->word2);
			pkt->rx_header_len = (buf2_len & AL_S2M_DESC_LEN2_MASK) >>
			AL_S2M_DESC_LEN2_SHIFT;
			}
		if (swap32_from_le(rx_desc->ctrl_meta) & AL_UDMA_CDESC_BUF1_USED)
			pkt->bufs[i].len = buf1_len & AL_S2M_DESC_LEN_MASK;
		else
			pkt->bufs[i].len = 0;
	}
	/* get flags from last desc */
	pkt->flags = swap32_from_le(rx_desc->ctrl_meta);
	pkt->flags_ex = swap32_from_le(rx_desc->word3);

	/**
	 * Addressing RMN: 11287, 11492
	 *
	 * RMN description:
	 * ETHv4- lb_packets (internal switching) may carry wrong
	 * pkt_l2_error / pkt_fifo_error indication
	 * Software flow:
	 * for lb_pkt (which can be identified by one of the 2 fields in the rx_comp_desc-
	 * EC_number or switch_sorce_port)- ignore the mac_error bit (which may carry l2_error).
	 */
	if (rx_dma_q->adapter_rev_id == AL_ETH_REV_ID_4) {
		/**
		 * LB indication is valid only if Word0[12:8] carry the EC info
		 * to make sure - look for :
		 * "w_0_b_8_12_select = AL_ETH_RX_CMPL_V4_W_0_B_8_12_EC_INFO"
		 *
		 * By default ETH_ERR_1 carries MAC errors, the register that determines what
		 * errors are collected at this bit is "rfw.meta_err.mask_1"
		 */
		if (unlikely(pkt->flags & AL_ETH_RX_W0_EC_INFO_LB_INDICATION))
			pkt->flags &= ~AL_ETH_RX_W0_ETH_ERR_1;
	}

#ifdef AL_ETH_RX_DESC_RAW_GET
	pkt->rx_desc_raw[0] = pkt->flags;
	pkt->rx_desc_raw[1] = swap32_from_le(rx_desc->len);
	pkt->rx_desc_raw[2] = swap32_from_le(rx_desc->word2);
	pkt->rx_desc_raw[3] = swap32_from_le(rx_desc->word3);
	if (rx_dma_q->cdesc_size > 16) {
		pkt->rx_desc_raw[4] = swap32_from_le(rx_desc->word4);
		pkt->rx_desc_raw[5] = swap32_from_le(rx_desc->word5);
		pkt->rx_desc_raw[6] = swap32_from_le(rx_desc->word6);
		pkt->rx_desc_raw[7] = swap32_from_le(rx_desc->word7);
	}
#endif

	if (rx_dma_q->adapter_rev_id >= AL_ETH_REV_ID_4) {
		pkt->rxhash = AL_REG_FIELD_GET(swap32_from_le(rx_desc->word6),
			AL_ETH_RX_HASH_MASK, AL_ETH_RX_HASH_SHIFT);
		pkt->protocol_number = AL_REG_FIELD_GET(pkt->flags,
			AL_ETH_V4_RX_PROTO_NUM_MASK, AL_ETH_V4_RX_PROTO_NUM_SHIFT);
		pkt->l3_proto_idx = 0;
		pkt->l4_proto_idx = 0;
	} else {
		pkt->protocol_number = 0;
		/* update L3/L4 proto index */
		pkt->l3_proto_idx = pkt->flags & AL_ETH_RX_L3_PROTO_IDX_MASK;
		pkt->l4_proto_idx = (pkt->flags >> AL_ETH_RX_L4_PROTO_IDX_SHIFT) &
			AL_ETH_RX_L4_PROTO_IDX_MASK;
		pkt->rxhash = (swap32_from_le(rx_desc->len) & AL_ETH_RX_HASH_MASK) >>
			AL_ETH_RX_HASH_SHIFT;
	}
	pkt->l3_offset = (swap32_from_le(rx_desc->word2) & AL_ETH_RX_L3_OFFSET_MASK) >> AL_ETH_RX_L3_OFFSET_SHIFT;

	al_udma_cdesc_ack(rx_dma_q, rc);
	return rc;
}

/* The Ethernet Parser Engine database includes 2 tables:
 * - Compare logic array: An array of values, masks, and commands, with each line used to detect a
 *   pre‐configured protocol.
 * - Parser Control table: A table that includes commands for processing the detected protocol
 *   and generating inputs for the next iteration.
 */
int al_eth_rx_parser_entry_update(struct al_hal_eth_adapter *adapter, uint32_t idx,
		struct al_eth_epe_p_reg_entry *reg_entry,
		struct al_eth_epe_control_entry *control_entry)
{
	struct al_eth_epe_handle epe_handle;

	/* Outer parser */
	epe_handle_init(adapter, &epe_handle, EPE_TYPE_OUTER);
	al_eth_epe_comp_entry_raw_set(&epe_handle, idx, reg_entry);
	al_eth_epe_ctrl_entry_raw_set(&epe_handle, idx, control_entry);

	/* Inner Parser */
	epe_handle_init(adapter, &epe_handle, EPE_TYPE_INNER);

	if (adapter->rev_id > AL_ETH_REV_ID_3)
		al_eth_epe_comp_entry_raw_set(&epe_handle, idx, reg_entry);

	al_eth_epe_ctrl_entry_raw_set(&epe_handle, idx, control_entry);

	return 0;
}

void al_eth_epe_entry_set(struct al_hal_eth_adapter *adapter, uint32_t idx,
			  struct al_eth_epe_p_reg_entry *reg_entry,
			  struct al_eth_epe_control_entry *control_entry)
{
	int rc = 0;
	rc = al_eth_rx_parser_entry_update(adapter, idx, reg_entry, control_entry);
	if (rc != 0)
		al_err("%s: Failed, rc = %d\n", __func__, rc);
}

void al_eth_epe_entry_get(struct al_hal_eth_adapter *adapter, uint32_t idx,
			struct al_eth_epe_p_reg_entry *reg_entry,
			struct al_eth_epe_control_entry *control_entry,
			unsigned int control_table_idx)
{
	struct al_eth_epe_handle epe_handle;
	epe_handle_init(adapter, &epe_handle, EPE_TYPE_OUTER);

	al_assert(adapter->rev_id <= AL_ETH_REV_ID_3);

	al_eth_epe_v3_entry_get(&epe_handle, idx, reg_entry, control_entry, control_table_idx);
}

#define AL_ETH_THASH_UDMA_SHIFT		0
#define AL_ETH_THASH_UDMA_MASK		(0xF << AL_ETH_THASH_UDMA_SHIFT)

#define AL_ETH_THASH_Q_SHIFT		4
#define AL_ETH_THASH_Q_MASK		(0xF << AL_ETH_THASH_Q_SHIFT)

int al_eth_thash_table_set(struct al_hal_eth_adapter *adapter, uint32_t idx, uint8_t udma, uint32_t queue)
{
	uint32_t entry;
	al_assert(idx < AL_ETH_RX_THASH_TABLE_SIZE); /*valid THASH index*/

	entry = (udma << AL_ETH_THASH_UDMA_SHIFT) & AL_ETH_THASH_UDMA_MASK;
	entry |= (queue << AL_ETH_THASH_Q_SHIFT) & AL_ETH_THASH_Q_MASK;

	al_reg_write32(&adapter->ec_regs_base->rfw.thash_table_addr, idx);
	al_reg_write32(&adapter->ec_regs_base->rfw.thash_table_data, entry);
	return 0;
}

int al_eth_fsm_table_set(struct al_hal_eth_adapter *adapter, uint32_t idx, uint32_t entry)
{

	al_assert(idx < AL_ETH_RX_FSM_TABLE_SIZE); /*valid FSM index*/


	al_reg_write32(&adapter->ec_regs_base->rfw.fsm_table_addr, idx);
	al_reg_write32(&adapter->ec_regs_base->rfw.fsm_table_data, entry);
	return 0;
}

static uint32_t	al_eth_fwd_ctrl_entry_to_val(struct al_eth_fwd_ctrl_table_entry *entry)
{
	uint32_t val = 0;
	AL_REG_FIELD_SET(val,  AL_FIELD_MASK(3,0), 0, entry->prio_sel);
	AL_REG_FIELD_SET(val,  AL_FIELD_MASK(7,4), 4, entry->queue_sel_1);
	AL_REG_FIELD_SET(val,  AL_FIELD_MASK(9,8), 8, entry->queue_sel_2);
	AL_REG_FIELD_SET(val,  AL_FIELD_MASK(13,10), 10, entry->udma_sel);
	AL_REG_FIELD_SET(val,  AL_FIELD_MASK(17,15), 15, entry->hdr_split_len_sel);
	if (entry->hdr_split_len_sel != AL_ETH_CTRL_TABLE_HDR_SPLIT_LEN_SEL_0)
		val |= AL_BIT(18);
	AL_REG_BIT_VAL_SET(val, 19, !!(entry->filter == AL_TRUE));

	return val;
}

static int al_eth_ctrl_index_match(struct al_eth_fwd_ctrl_table_index *index, uint32_t i) {
	if ((index->vlan_table_out != AL_ETH_FWD_CTRL_IDX_VLAN_TABLE_OUT_ANY)
		&& (index->vlan_table_out != AL_REG_BIT_GET(i, 0)))
		return 0;
	if ((index->tunnel_exist != AL_ETH_FWD_CTRL_IDX_TUNNEL_ANY)
		&& (index->tunnel_exist != AL_REG_BIT_GET(i, 1)))
		return 0;
	if ((index->vlan_exist != AL_ETH_FWD_CTRL_IDX_VLAN_ANY)
		&& (index->vlan_exist != AL_REG_BIT_GET(i, 2)))
		return 0;
	if ((index->mac_table_match != AL_ETH_FWD_CTRL_IDX_MAC_TABLE_ANY)
		&& (index->mac_table_match != AL_REG_BIT_GET(i, 3)))
		return 0;
	if ((index->protocol_id != AL_ETH_PROTO_ID_ANY)
		&& (index->protocol_id != AL_REG_FIELD_GET(i, AL_FIELD_MASK(8,4),4)))
		return 0;
	if ((index->mac_type != AL_ETH_FWD_CTRL_IDX_MAC_DA_TYPE_ANY)
		&& (index->mac_type != AL_REG_FIELD_GET(i, AL_FIELD_MASK(10,9),9)))
		return 0;
	return 1;
}

int al_eth_ctrl_table_set(struct al_hal_eth_adapter *adapter,
			  struct al_eth_fwd_ctrl_table_index *index,
			  struct al_eth_fwd_ctrl_table_entry *entry)
{
	uint32_t val = al_eth_fwd_ctrl_entry_to_val(entry);
	uint32_t i;

	for (i = 0; i < AL_ETH_RX_CTRL_TABLE_SIZE; i++) {
		if (al_eth_ctrl_index_match(index, i)) {
			al_reg_write32(&adapter->ec_regs_base->rfw.ctrl_table_addr, i);
			al_reg_write32(&adapter->ec_regs_base->rfw.ctrl_table_data, val);
		}
	}
	return 0;
}

int al_eth_ctrl_table_def_set(struct al_hal_eth_adapter *adapter,
			      al_bool use_table,
			      struct al_eth_fwd_ctrl_table_entry *entry)
{
	uint32_t val = al_eth_fwd_ctrl_entry_to_val(entry);

	if (use_table)
		val |= EC_RFW_CTRL_TABLE_DEF_SEL;

	al_reg_write32(&adapter->ec_regs_base->rfw.ctrl_table_def, val);

	return 0;
}

int al_eth_ctrl_table_raw_set(struct al_hal_eth_adapter *adapter, uint32_t idx, uint32_t entry)
{

	al_assert(idx < AL_ETH_RX_CTRL_TABLE_SIZE); /* valid CTRL index */


	al_reg_write32(&adapter->ec_regs_base->rfw.ctrl_table_addr, idx);
	al_reg_write32(&adapter->ec_regs_base->rfw.ctrl_table_data, entry);
	return 0;
}

int al_eth_ctrl_table_raw_get(struct al_hal_eth_adapter *adapter, uint32_t idx, uint32_t *entry)
{
	al_assert(adapter);
	al_assert(entry);
	al_assert(idx < AL_ETH_RX_CTRL_TABLE_SIZE); /* valid CTRL index */

	al_reg_write32(&adapter->ec_regs_base->rfw.ctrl_table_addr, idx);
	al_local_data_memory_barrier();
	*entry = al_reg_read32(&adapter->ec_regs_base->rfw.ctrl_table_data);

	return 0;
}

int al_eth_ctrl_table_def_raw_set(struct al_hal_eth_adapter *adapter, uint32_t val)
{
	al_reg_write32(&adapter->ec_regs_base->rfw.ctrl_table_def, val);

	return 0;
}

int al_eth_ctrl_table_def_raw_get(struct al_hal_eth_adapter *adapter, uint32_t *val)
{
	al_assert(adapter);
	al_assert(val);

	*val = al_reg_read32(&adapter->ec_regs_base->rfw.ctrl_table_def);

	return 0;
}

int al_eth_hash_key_set(struct al_hal_eth_adapter *adapter, uint32_t idx, uint32_t val)
{

	al_assert(idx < AL_ETH_RX_HASH_KEY_NUM); /*valid CTRL index*/

	al_reg_write32(&adapter->ec_regs_base->rfw_hash[idx].key, val);

	return 0;
}

static uint32_t	al_eth_fwd_mac_table_entry_to_val(struct al_eth_fwd_mac_table_entry *entry)
{
	uint32_t val = 0;

	val |= (entry->filter == AL_TRUE) ? EC_FWD_MAC_CTRL_RX_VAL_DROP : 0;
	val |= ((entry->udma_mask << EC_FWD_MAC_CTRL_RX_VAL_UDMA_SHIFT) &
					EC_FWD_MAC_CTRL_RX_VAL_UDMA_MASK);

	val |= ((entry->qid << EC_FWD_MAC_CTRL_RX_VAL_QID_SHIFT) &
					EC_FWD_MAC_CTRL_RX_VAL_QID_MASK);

	val |= (entry->rx_valid == AL_TRUE) ? EC_FWD_MAC_CTRL_RX_VALID : 0;

	val |= ((entry->tx_target << EC_FWD_MAC_CTRL_TX_VAL_SHIFT) &
					EC_FWD_MAC_CTRL_TX_VAL_MASK);

	val |= (entry->tx_valid == AL_TRUE) ? EC_FWD_MAC_CTRL_TX_VALID : 0;

	return val;
}

int al_eth_fwd_mac_table_set(struct al_hal_eth_adapter *adapter, uint32_t idx,
			     struct al_eth_fwd_mac_table_entry *entry)
{
	uint32_t val;

	al_assert(idx < AL_ETH_FWD_MAC_NUM); /*valid FWD MAC index */

	val = (entry->addr[2] << 24) | (entry->addr[3] << 16) |
	      (entry->addr[4] << 8) | entry->addr[5];
	al_reg_write32(&adapter->ec_regs_base->fwd_mac[idx].data_l, val);
	val = (entry->addr[0] << 8) | entry->addr[1];
	al_reg_write32(&adapter->ec_regs_base->fwd_mac[idx].data_h, val);
	val = (entry->mask[2] << 24) | (entry->mask[3] << 16) |
	      (entry->mask[4] << 8) | entry->mask[5];
	al_reg_write32(&adapter->ec_regs_base->fwd_mac[idx].mask_l, val);
	val = (entry->mask[0] << 8) | entry->mask[1];
	al_reg_write32(&adapter->ec_regs_base->fwd_mac[idx].mask_h, val);

	val = al_eth_fwd_mac_table_entry_to_val(entry);
	al_reg_write32(&adapter->ec_regs_base->fwd_mac[idx].ctrl, val);
	return 0;
}



int al_eth_fwd_mac_addr_raw_set(struct al_hal_eth_adapter *adapter, uint32_t idx, uint32_t addr_lo, uint32_t addr_hi, uint32_t mask_lo, uint32_t mask_hi)
{
	al_assert(idx < AL_ETH_FWD_MAC_NUM); /*valid FWD MAC index */

	al_reg_write32(&adapter->ec_regs_base->fwd_mac[idx].data_l, addr_lo);
	al_reg_write32(&adapter->ec_regs_base->fwd_mac[idx].data_h, addr_hi);
	al_reg_write32(&adapter->ec_regs_base->fwd_mac[idx].mask_l, mask_lo);
	al_reg_write32(&adapter->ec_regs_base->fwd_mac[idx].mask_h, mask_hi);

	return 0;
}

int al_eth_fwd_mac_ctrl_raw_set(struct al_hal_eth_adapter *adapter, uint32_t idx, uint32_t ctrl)
{
	al_assert(idx < AL_ETH_FWD_MAC_NUM); /*valid FWD MAC index */

	al_reg_write32(&adapter->ec_regs_base->fwd_mac[idx].ctrl, ctrl);

	return 0;
}

int al_eth_mac_addr_store(void * __iomem ec_base, uint32_t idx, uint8_t *addr)
{
	struct al_ec_regs __iomem *ec_regs_base = (struct al_ec_regs __iomem*)ec_base;
	uint32_t val;

	al_assert(idx < AL_ETH_FWD_MAC_NUM); /*valid FWD MAC index */

	val = (addr[2] << 24) | (addr[3] << 16) | (addr[4] << 8) | addr[5];
	al_reg_write32(&ec_regs_base->fwd_mac[idx].data_l, val);
	val = (addr[0] << 8) | addr[1];
	al_reg_write32(&ec_regs_base->fwd_mac[idx].data_h, val);
	return 0;
}

int al_eth_mac_addr_read(void * __iomem ec_base, uint32_t idx, uint8_t *addr)
{
	struct al_ec_regs __iomem *ec_regs_base = (struct al_ec_regs __iomem*)ec_base;
	uint32_t addr_lo = al_reg_read32(&ec_regs_base->fwd_mac[idx].data_l);
	uint16_t addr_hi = al_reg_read32(&ec_regs_base->fwd_mac[idx].data_h);

	addr[5] = addr_lo & 0xff;
	addr[4] = (addr_lo >> 8) & 0xff;
	addr[3] = (addr_lo >> 16) & 0xff;
	addr[2] = (addr_lo >> 24) & 0xff;
	addr[1] = addr_hi & 0xff;
	addr[0] = (addr_hi >> 8) & 0xff;
	return 0;
}

int al_eth_fwd_mhash_table_set(struct al_hal_eth_adapter *adapter, uint32_t idx, uint8_t udma_mask, uint8_t qid)
{
	uint32_t val = 0;
	al_assert(idx < AL_ETH_FWD_MAC_HASH_NUM); /* valid MHASH index */

	AL_REG_FIELD_SET(val,  AL_FIELD_MASK(3, 0), 0, udma_mask);
	AL_REG_FIELD_SET(val,  AL_FIELD_MASK(7, 4), 4, qid);

	al_reg_write32(&adapter->ec_regs_base->rfw.mhash_table_addr, idx);
	al_reg_write32(&adapter->ec_regs_base->rfw.mhash_table_data, val);
	return 0;
}
static uint32_t	al_eth_fwd_vid_entry_to_val(struct al_eth_fwd_vid_table_entry *entry)
{
	uint32_t val = 0;
	AL_REG_BIT_VAL_SET(val, 0, entry->control);
	AL_REG_BIT_VAL_SET(val, 1, entry->filter);
	AL_REG_FIELD_SET(val, AL_FIELD_MASK(5,2), 2, entry->udma_mask);

	return val;
}

int al_eth_fwd_vid_config_set(struct al_hal_eth_adapter *adapter, al_bool use_table,
			      struct al_eth_fwd_vid_table_entry *default_entry,
			      uint32_t default_vlan)
{
	uint32_t reg;

	reg = al_eth_fwd_vid_entry_to_val(default_entry);
	if (use_table)
		reg |= EC_RFW_VID_TABLE_DEF_SEL;
	else
		reg &= ~EC_RFW_VID_TABLE_DEF_SEL;
	al_reg_write32(&adapter->ec_regs_base->rfw.vid_table_def, reg);
	al_reg_write32(&adapter->ec_regs_base->rfw.default_vlan, default_vlan);

	return 0;
}

int al_eth_fwd_vid_table_set(struct al_hal_eth_adapter *adapter, uint32_t idx,
			     struct al_eth_fwd_vid_table_entry *entry)
{
	uint32_t val;
	al_assert(idx < AL_ETH_FWD_VID_TABLE_NUM); /* valid VID index */

	val = al_eth_fwd_vid_entry_to_val(entry);
	al_reg_write32(&adapter->ec_regs_base->rfw.vid_table_addr, idx);
	al_reg_write32(&adapter->ec_regs_base->rfw.vid_table_data, val);
	return 0;
}

int al_eth_fwd_pbits_table_set(struct al_hal_eth_adapter *adapter, uint32_t idx, uint8_t prio)
{

	al_assert(idx < AL_ETH_FWD_PBITS_TABLE_NUM); /* valid PBIT index */
	al_assert(prio < AL_ETH_FWD_PRIO_TABLE_NUM); /* valid PRIO index */
	al_reg_write32(&adapter->ec_regs_base->rfw.pbits_table_addr, idx);
	al_reg_write32(&adapter->ec_regs_base->rfw.pbits_table_data, prio);
	return 0;
}

int al_eth_fwd_priority_table_set(struct al_hal_eth_adapter *adapter, uint8_t prio, uint8_t qid)
{
	al_assert(prio < AL_ETH_FWD_PRIO_TABLE_NUM); /* valid PRIO index */

	al_reg_write32(&adapter->ec_regs_base->rfw_priority[prio].queue, qid);
	return 0;
}


int al_eth_fwd_dscp_table_set(struct al_hal_eth_adapter *adapter, uint32_t idx, uint8_t prio)
{

	al_assert(idx < AL_ETH_FWD_DSCP_TABLE_NUM); /* valid DSCP index */


	al_reg_write32(&adapter->ec_regs_base->rfw.dscp_table_addr, idx);
	al_reg_write32(&adapter->ec_regs_base->rfw.dscp_table_data, prio);
	return 0;
}

int al_eth_fwd_tc_table_set(struct al_hal_eth_adapter *adapter, uint32_t idx, uint8_t prio)
{

	al_assert(idx < AL_ETH_FWD_TC_TABLE_NUM); /* valid TC index */


	al_reg_write32(&adapter->ec_regs_base->rfw.tc_table_addr, idx);
	al_reg_write32(&adapter->ec_regs_base->rfw.tc_table_data, prio);
	return 0;
}

/** Configure default UDMA register */
int al_eth_fwd_default_udma_config(struct al_hal_eth_adapter *adapter, uint32_t idx,
				   uint8_t udma_mask)
{
	al_reg_write32_masked(&adapter->ec_regs_base->rfw_default[idx].opt_1,
			       EC_RFW_DEFAULT_OPT_1_UDMA_MASK,
			       udma_mask << EC_RFW_DEFAULT_OPT_1_UDMA_SHIFT);
	return 0;
}

/** Configure default queue register */
int al_eth_fwd_default_queue_config(struct al_hal_eth_adapter *adapter, uint32_t idx,
				   uint8_t qid)
{
	al_reg_write32_masked(&adapter->ec_regs_base->rfw_default[idx].opt_1,
			       EC_RFW_DEFAULT_OPT_1_QUEUE_MASK,
			       qid << EC_RFW_DEFAULT_OPT_1_QUEUE_SHIFT);
	return 0;
}

/** Configure default priority register */
int al_eth_fwd_default_priority_config(struct al_hal_eth_adapter *adapter, uint32_t idx,
				   uint8_t prio)
{
	al_reg_write32_masked(&adapter->ec_regs_base->rfw_default[idx].opt_1,
			       EC_RFW_DEFAULT_OPT_1_DEFAULT_PRIORITY_MASK,
			       prio << EC_RFW_DEFAULT_OPT_1_DEFAULT_PRIORITY_SHIFT);
	return 0;
}

int al_eth_switching_config_set(struct al_hal_eth_adapter *adapter, uint8_t udma_id, uint8_t forward_all_to_mac, uint8_t enable_int_switching,
					enum al_eth_tx_switch_vid_sel_type vid_sel_type,
					enum al_eth_tx_switch_dec_type uc_dec,
					enum al_eth_tx_switch_dec_type mc_dec,
					enum al_eth_tx_switch_dec_type bc_dec)
{
	uint32_t reg;

	if (udma_id == 0) {
		reg = al_reg_read32(&adapter->ec_regs_base->tfw.tx_gen);
		if (forward_all_to_mac)
			reg |= EC_TFW_TX_GEN_FWD_ALL_TO_MAC;
		else
			reg &= ~EC_TFW_TX_GEN_FWD_ALL_TO_MAC;
		al_reg_write32(&adapter->ec_regs_base->tfw.tx_gen, reg);
	}

	reg = enable_int_switching;
	reg |= (vid_sel_type & 7) << 1;
	reg |= (bc_dec & 3) << 4;
	reg |= (mc_dec & 3) << 6;
	reg |= (uc_dec & 3) << 8;
	al_reg_write32(&adapter->ec_regs_base->tfw_udma[udma_id].fwd_dec, reg);

	return 0;
}

#define AL_ETH_RFW_FILTER_SUPPORTED(rev_id)	\
	(AL_ETH_RFW_FILTER_UNDET_MAC | \
	AL_ETH_RFW_FILTER_DET_MAC | \
	AL_ETH_RFW_FILTER_TAGGED | \
	AL_ETH_RFW_FILTER_UNTAGGED | \
	AL_ETH_RFW_FILTER_BC | \
	AL_ETH_RFW_FILTER_MC | \
	AL_ETH_RFW_FILTER_VLAN_VID | \
	AL_ETH_RFW_FILTER_CTRL_TABLE | \
	AL_ETH_RFW_FILTER_PROT_INDEX | \
	AL_ETH_RFW_FILTER_WOL | \
	AL_ETH_RFW_FILTER_PARSE)

/* Configure the receive filters */
int al_eth_filter_config(struct al_hal_eth_adapter *adapter, struct al_eth_filter_params *params)
{
	uint32_t reg;

	al_assert(params); /* valid params pointer */

	if (params->filters & ~(AL_ETH_RFW_FILTER_SUPPORTED(adapter->rev_id))) {
		al_err("[%s]: unsupported filter options (0x%08x)\n", adapter->name, params->filters);
		return -EINVAL;
	}

	reg = al_reg_read32(&adapter->ec_regs_base->rfw.out_cfg);
	if (params->enable == AL_TRUE)
		AL_REG_MASK_SET(reg, EC_RFW_OUT_CFG_DROP_EN);
	else
		AL_REG_MASK_CLEAR(reg, EC_RFW_OUT_CFG_DROP_EN);
	al_reg_write32(&adapter->ec_regs_base->rfw.out_cfg, reg);

	al_reg_write32_masked(
		&adapter->ec_regs_base->rfw.filter,
		AL_ETH_RFW_FILTER_SUPPORTED(adapter->rev_id),
		params->filters);
	if (params->filters & AL_ETH_RFW_FILTER_PROT_INDEX) {
		int i;
		for (i = 0; i < AL_ETH_PROTOCOLS_NUM; i++) {
			reg = al_reg_read32(&adapter->ec_regs_base->epe_a[i].prot_act);
			if (params->filter_proto[i] == AL_TRUE)
				AL_REG_MASK_SET(reg, EC_EPE_A_PROT_ACT_DROP);
			else
				AL_REG_MASK_CLEAR(reg, EC_EPE_A_PROT_ACT_DROP);
			al_reg_write32(&adapter->ec_regs_base->epe_a[i].prot_act, reg);
		}
	}
	return 0;
}

/* Configure the receive override filters */
int al_eth_filter_override_config(struct al_hal_eth_adapter *adapter,
				  struct al_eth_filter_override_params *params)
{
	uint32_t reg;

	al_assert(params); /* valid params pointer */

	if (params->filters & ~(AL_ETH_RFW_FILTER_SUPPORTED(adapter->rev_id))) {
		al_err("[%s]: unsupported override filter options (0x%08x)\n", adapter->name, params->filters);
		return -EINVAL;
	}

	al_reg_write32_masked(
		&adapter->ec_regs_base->rfw.filter,
		AL_ETH_RFW_FILTER_SUPPORTED(adapter->rev_id) << 16,
		params->filters << 16);

	reg = al_reg_read32(&adapter->ec_regs_base->rfw.default_or);
	AL_REG_FIELD_SET(reg, EC_RFW_DEFAULT_OR_UDMA_MASK, EC_RFW_DEFAULT_OR_UDMA_SHIFT, params->udma);
	AL_REG_FIELD_SET(reg, EC_RFW_DEFAULT_OR_QUEUE_MASK, EC_RFW_DEFAULT_OR_QUEUE_SHIFT, params->qid);
	al_reg_write32(&adapter->ec_regs_base->rfw.default_or, reg);
	return 0;
}



int al_eth_switching_default_bitmap_set(struct al_hal_eth_adapter *adapter, uint8_t udma_id, uint8_t udma_uc_bitmask,
						uint8_t udma_mc_bitmask,uint8_t udma_bc_bitmask)
{
	al_reg_write32(&adapter->ec_regs_base->tfw_udma[udma_id].uc_udma, udma_uc_bitmask);
	al_reg_write32(&adapter->ec_regs_base->tfw_udma[udma_id].mc_udma, udma_mc_bitmask);
	al_reg_write32(&adapter->ec_regs_base->tfw_udma[udma_id].bc_udma, udma_bc_bitmask);

	return 0;
}

static int flow_control_config_v1_v3(struct al_hal_eth_adapter *adapter,
	struct al_eth_flow_control_params *params)
{
	uint32_t reg;
	al_assert(params); /* valid params pointer */

	switch(params->type){
	case AL_ETH_FLOW_CONTROL_TYPE_LINK_PAUSE:
		al_dbg("[%s]: config flow control to link pause mode.\n", adapter->name);

		/* config the mac */
		if (AL_ETH_IS_1G_MAC(adapter->mac_mode)) {
			/* set quanta value */
			al_reg_write32(
				&adapter->mac_regs_base->mac_1g.pause_quant,
				params->quanta);
			al_reg_write32(
				&adapter->ec_regs_base->efc.xoff_timer_1g,
				params->quanta_th);

		} else if (AL_ETH_IS_10G_MAC(adapter->mac_mode) || AL_ETH_IS_25G_MAC(adapter->mac_mode)) {
			/* set quanta value */
			al_reg_write32(
				&adapter->mac_regs_base->mac_10g.cl01_pause_quanta,
				params->quanta);
			/* set quanta threshold value */
			al_reg_write32(
				&adapter->mac_regs_base->mac_10g.cl01_quanta_thresh,
				params->quanta_th);
		} else {
			/* set quanta value */
			al_eth_40g_mac_reg_write(adapter,
				ETH_MAC_GEN_V3_MAC_40G_CL01_PAUSE_QUANTA_ADDR,
				params->quanta);
			/* set quanta threshold value */
			al_eth_40g_mac_reg_write(adapter,
				ETH_MAC_GEN_V3_MAC_40G_CL01_QUANTA_THRESH_ADDR,
				params->quanta_th);
		}


	break;
	case AL_ETH_FLOW_CONTROL_TYPE_PFC:
		al_dbg("[%s]: config flow control to PFC mode.\n", adapter->name);
		/* pfc not available for RGMII mode */;
		al_assert(!AL_ETH_IS_1G_MAC(adapter->mac_mode));



		if (AL_ETH_IS_10G_MAC(adapter->mac_mode) || AL_ETH_IS_25G_MAC(adapter->mac_mode)) {
			/* config the 10g_mac */
			/* set quanta value (same value for all prios) */
			reg = params->quanta | (params->quanta << 16);
			al_reg_write32(
				&adapter->mac_regs_base->mac_10g.cl01_pause_quanta, reg);
			al_reg_write32(
				&adapter->mac_regs_base->mac_10g.cl23_pause_quanta, reg);
			al_reg_write32(
				&adapter->mac_regs_base->mac_10g.cl45_pause_quanta, reg);
			al_reg_write32(
				&adapter->mac_regs_base->mac_10g.cl67_pause_quanta, reg);
			/* set quanta threshold value (same value for all prios) */
			reg = params->quanta_th | (params->quanta_th << 16);
			al_reg_write32(
				&adapter->mac_regs_base->mac_10g.cl01_quanta_thresh, reg);
			al_reg_write32(
				&adapter->mac_regs_base->mac_10g.cl23_quanta_thresh, reg);
			al_reg_write32(
				&adapter->mac_regs_base->mac_10g.cl45_quanta_thresh, reg);
			al_reg_write32(
				&adapter->mac_regs_base->mac_10g.cl67_quanta_thresh, reg);

			/* enable PFC in the 10g_MAC */
			reg = al_reg_read32(&adapter->mac_regs_base->mac_10g.cmd_cfg);
			reg |= 1 << 19;
			al_reg_write32(&adapter->mac_regs_base->mac_10g.cmd_cfg, reg);
		} else {
			/* config the 40g_mac */
			/* set quanta value (same value for all prios) */
			reg = params->quanta | (params->quanta << 16);
			al_eth_40g_mac_reg_write(adapter,
				ETH_MAC_GEN_V3_MAC_40G_CL01_PAUSE_QUANTA_ADDR, reg);
			al_eth_40g_mac_reg_write(adapter,
				ETH_MAC_GEN_V3_MAC_40G_CL23_PAUSE_QUANTA_ADDR, reg);
			al_eth_40g_mac_reg_write(adapter,
				ETH_MAC_GEN_V3_MAC_40G_CL45_PAUSE_QUANTA_ADDR, reg);
			al_eth_40g_mac_reg_write(adapter,
				ETH_MAC_GEN_V3_MAC_40G_CL67_PAUSE_QUANTA_ADDR, reg);
			/* set quanta threshold value (same value for all prios) */
			reg = params->quanta_th | (params->quanta_th << 16);
			al_eth_40g_mac_reg_write(adapter,
				ETH_MAC_GEN_V3_MAC_40G_CL01_QUANTA_THRESH_ADDR, reg);
			al_eth_40g_mac_reg_write(adapter,
				ETH_MAC_GEN_V3_MAC_40G_CL23_QUANTA_THRESH_ADDR, reg);
			al_eth_40g_mac_reg_write(adapter,
				ETH_MAC_GEN_V3_MAC_40G_CL45_QUANTA_THRESH_ADDR, reg);
			al_eth_40g_mac_reg_write(adapter,
				ETH_MAC_GEN_V3_MAC_40G_CL67_QUANTA_THRESH_ADDR, reg);

			/* enable PFC in the 40g_MAC */
			reg = al_reg_read32(&adapter->mac_regs_base->mac_10g.cmd_cfg);
			reg |= 1 << 19;
			al_reg_write32(&adapter->mac_regs_base->mac_10g.cmd_cfg, reg);
			reg = al_eth_40g_mac_reg_read(adapter, ETH_MAC_GEN_V3_MAC_40G_COMMAND_CONFIG_ADDR);

			reg |= ETH_MAC_GEN_V3_MAC_40G_COMMAND_CONFIG_PFC_MODE;

			al_eth_40g_mac_reg_write(adapter, ETH_MAC_GEN_V3_MAC_40G_COMMAND_CONFIG_ADDR, reg);
		}

	break;
	default:
		al_err("[%s]: unsupported flow control type %d\n", adapter->name, params->type);
		return -EINVAL;

	}
	return 0;
}

static int al_eth_flow_control_config_efc(struct al_hal_eth_adapter *adapter,
	struct al_eth_flow_control_params *params)
{
	int i;
	al_assert(params); /* valid params pointer */

	switch (adapter->rev_id) {
	case AL_ETH_REV_ID_4:
		/** Not all common mode are supported for now */
		if (adapter->dev_id == AL_ETH_DEV_ID_STANDARD)
			break;
		else if (adapter->common_mode == AL_ETH_COMMON_MODE_1X100G)
			break;
		else if (adapter->common_mode == AL_ETH_COMMON_MODE_4X25G)
			break;
		else
			al_assert(0);

		break;
	case AL_ETH_REV_ID_1:
	case AL_ETH_REV_ID_2:
	case AL_ETH_REV_ID_3:
		break;
	default:
		al_err("%s: Unknown rev id %d\n", __func__, adapter->rev_id);
		al_assert(0);
	}

	switch (params->type) {
	case AL_ETH_FLOW_CONTROL_TYPE_LINK_PAUSE:
		al_dbg("[%s]: config EFC flow control to link pause mode.\n", adapter->name);

		/**
		 * RX path - Generating Xoff (gen_enable)
		 */

		/* enable generating xoff from ec fifo almost full indication in hysteresis mode.
		 * (as its link-pause mode, only the first bit that relates to priority 0 (class 0)
		 * is set
		 * This is the RX FIFO (between MAC & EC)
		 *
		 * NOTICE - Below we are also generating Xoff from UDMA queue almost full indication
		 * The final Xoff generation signal is an OR of those two decisions
		 */
		al_reg_write32(&adapter->ec_regs_base->efc.ec_xoff,
			params->gen_enable ? 1 << EC_EFC_EC_XOFF_MASK_2_SHIFT : 0);

		/* set hysteresis mode thresholds - active only if the above wrote a value
		 * different than 0 */
		al_reg_write32(&adapter->ec_regs_base->efc.rx_fifo_hyst,
			params->rx_fifo_th_low |
			(params->rx_fifo_th_high << EC_EFC_RX_FIFO_HYST_TH_HIGH_SHIFT));

		/**
		 * TX path - Obeying to Xoff (obey_enable)
		 */

		/* Tx path FIFO, unmask pause_on from MAC when PAUSE packet received */
		al_reg_write32(&adapter->ec_regs_base->efc.ec_pause, params->obay_enable ? 1 : 0);

		/**
		 * RX path - Generating Xon
		 */

		/* in 1G mode, enable generating xon from ec fifo in hysteresis mode */
		if (AL_ETH_IS_1G_MAC(adapter->mac_mode))
			al_reg_write32(&adapter->ec_regs_base->efc.xon,
				EC_EFC_XON_MASK_2 | EC_EFC_XON_MASK_1);

		/**
		 * This loop configures the Xoff packets effect on UDMAs.
		 * The EC receives a signal from the MAC that Xoff packets has been received and
		 * can set the EC / UDMA / UDMA queues response.
		 *
		 * One should know that the MAC stops transmitting as it receives an Xoff,
		 * So even if we don't configure the EC / UDMA to act the outgoing traffic
		 * rate will still be lowered.
		 *
		 * As we are in link-pause mode we will write to the priority 0 (class 0) registers
		 * only
		 */
		for (i = 0; i < 4; i++) {
			uint32_t gen_enable = params->gen_enable ? params->prio_q_map[i][0] : 0;
			uint32_t obey_enable = params->obay_enable ? params->prio_q_map[i][0] : 0;
			int udma_num = adapter->serdes_lane;

			if (AL_ETH_ADAPTER_NEED_COMMON_MODE(adapter->dev_id, adapter->rev_id)) {
				/**
				 * RX side (generate Xoff) -
				 *  we need to configure one time according to absolute udma number
				 *  (config val is taken from prio_q_map[0][0] as the adapter
				 *  "sees" its UDMA as UDMA 0)
				 *  We have a 1 to 1 match between serdes lanes and udma mapping
				 *
				 * TX (accept Xoff) side -
				 *  we need to configure all UDMA's
				 *
				 * TODO other common port modes needs their own handling
				 */
				if (adapter->common_mode == AL_ETH_COMMON_MODE_4X25G) {
					if (udma_num == i)
						gen_enable = params->gen_enable ?
							params->prio_q_map[0][0] : 0;
					else
						gen_enable = 0;

				}
			}

			/*
			 * Tx path UDMA, unmask pause_on for selected queues (hot-one bitmap)
			 * A one-hot queues bitmap
			 */
			al_reg_write32(&adapter->ec_regs_base->fc_udma[i].q_pause_0,
				obey_enable);
			/*
			 * Rx path UDMA, enable generating xoff from UDMA queue almost full
			 * indication for selected queues
			 * A one-hot queues bitmap
			 */
			al_reg_write32(&adapter->ec_regs_base->fc_udma[i].q_xoff_0,
				gen_enable);
		}

	break;
	case AL_ETH_FLOW_CONTROL_TYPE_PFC:
		al_dbg("[%s]: config EFC flow control to PFC mode.\n", adapter->name);
		/* pfc not available for RGMII mode */;
		al_assert(!AL_ETH_IS_1G_MAC(adapter->mac_mode));

		/* set hysteresis mode thresholds */
		al_reg_write32(&adapter->ec_regs_base->efc.rx_fifo_hyst,
			params->rx_fifo_th_low |
			(params->rx_fifo_th_high << EC_EFC_RX_FIFO_HYST_TH_HIGH_SHIFT));

		for (i = 0; i < 4; i++) {
			int prio;
			int udma_num = adapter->serdes_lane;

			for (prio = 0; prio < 8; prio++) {
				uint32_t gen_enable = params->gen_enable ?
					params->prio_q_map[i][prio] : 0;
				uint32_t obey_enable = params->obay_enable ?
					params->prio_q_map[i][prio] : 0;

				if (adapter->common_mode == AL_ETH_COMMON_MODE_4X25G) {
					al_assert(adapter->rev_id >= AL_ETH_REV_ID_4);

					if (udma_num == i)
						gen_enable =  params->gen_enable ?
							params->prio_q_map[0][prio] : 0;
					else
						gen_enable = 0;
				}

				al_reg_write32(
					&adapter->ec_regs_base->fc_udma[i].q_pause_0 + prio,
					obey_enable);
				al_reg_write32(
					&adapter->ec_regs_base->fc_udma[i].q_xoff_0 + prio,
					gen_enable);
			}
		}

		/* Rx path */
		/* enable generating xoff from ec fifo almost full indication in hysteresis mode */
		al_reg_write32(&adapter->ec_regs_base->efc.ec_xoff,
			params->gen_enable ? 0xFF << EC_EFC_EC_XOFF_MASK_2_SHIFT : 0);
	break;
	default:
		al_err("[%s]: unsupported flow control type %d\n", adapter->name, params->type);
		return -EINVAL;

	}
	return 0;
}

int al_eth_flow_control_config(struct al_hal_eth_adapter *adapter,
	struct al_eth_flow_control_params *params)
{
	int err;
	if (adapter->mac_obj.flow_control_config_full)
		err = adapter->mac_obj.flow_control_config_full(&adapter->mac_obj, params);
	else
		err = flow_control_config_v1_v3(adapter, params);
	if (err)
		return err;

	if (!params->skip_flow_ctrl_efc)
		err = al_eth_flow_control_config_efc(adapter, params);

	return err;
}

int al_eth_vlan_mod_config(struct al_hal_eth_adapter *adapter, uint8_t udma_id, uint16_t udma_etype, uint16_t vlan1_data, uint16_t vlan2_data)
{
	al_dbg("[%s]: config vlan modification registers. udma id %d.\n", adapter->name, udma_id);

	al_reg_write32(&adapter->ec_regs_base->tpm_sel[udma_id].etype, udma_etype);
	al_reg_write32(&adapter->ec_regs_base->tpm_udma[udma_id].vlan_data, vlan1_data | (vlan2_data << 16));

	return 0;
}

int al_eth_eee_get(struct al_hal_eth_adapter *adapter, struct al_eth_eee_params *params)
{
	uint32_t reg;

	al_dbg("[%s]: getting eee.\n", adapter->name);

	reg = al_reg_read32(&adapter->ec_regs_base->eee.cfg_e);
	params->enable = (reg & EC_EEE_CFG_E_ENABLE) ? AL_TRUE : AL_FALSE;

	params->tx_eee_timer = al_reg_read32(&adapter->ec_regs_base->eee.pre_cnt);
	params->min_interval = al_reg_read32(&adapter->ec_regs_base->eee.post_cnt);
	params->stop_cnt = al_reg_read32(&adapter->ec_regs_base->eee.stop_cnt);

	return 0;
}


int al_eth_eee_config(struct al_hal_eth_adapter *adapter, struct al_eth_eee_params *params)
{
	int ret;
	uint32_t reg;
	al_dbg("[%s]: config eee.\n", adapter->name);

	if (params->enable == 0) {
		al_dbg("[%s]: disable eee.\n", adapter->name);
		al_reg_write32(&adapter->ec_regs_base->eee.cfg_e, 0);
		return 0;
	}

	ret = adapter->mac_obj.eee_config(&adapter->mac_obj, params);
	if (ret != 0)
		return ret;

	al_reg_write32(&adapter->ec_regs_base->eee.pre_cnt, params->tx_eee_timer);
	al_reg_write32(&adapter->ec_regs_base->eee.post_cnt, params->min_interval);
	al_reg_write32(&adapter->ec_regs_base->eee.stop_cnt, params->stop_cnt);

	reg = EC_EEE_CFG_E_MASK_EC_TMI_STOP | EC_EEE_CFG_E_MASK_MAC_EEE |
	       EC_EEE_CFG_E_ENABLE |
	       EC_EEE_CFG_E_USE_EC_TX_FIFO | EC_EEE_CFG_E_USE_EC_RX_FIFO;

	/*
	 * Addressing RMN: 3732
	 *
	 * RMN description:
	 * When the HW get into eee mode, it can't transmit any pause packet
	 * (when flow control policy is enabled).
	 * In such case, the HW has no way to handle extreme pushback from
	 * the Rx_path fifos.
	 *
	 * Software flow:
	 * Configure RX_FIFO empty as eee mode term.
	 * That way, nothing will prevent pause packet transmittion in
	 * case of extreme pushback from the Rx_path fifos.
	 *
	 */

	al_reg_write32(&adapter->ec_regs_base->eee.cfg_e, reg);

	return ret;
}

/* Timestamp */
/* prepare the adapter for doing Timestamps for Rx packets. */
int al_eth_ts_init(struct al_hal_eth_adapter *adapter)
{
	uint32_t reg;

	/*TODO:
	 * return error when:
	 * - working in 1G mode and MACSEC enabled
	 * - RX completion descriptor is not 8 words
	 */
	reg = al_reg_read32(&adapter->ec_regs_base->gen.en_ext);
	if (AL_ETH_IS_1G_MAC(adapter->mac_mode))
		reg &= ~EC_GEN_EN_EXT_PTH_1_10_SEL;
	else
		reg |= EC_GEN_EN_EXT_PTH_1_10_SEL;
	/*
	 * set completion bypass so tx timestamps won't be inserted to tx cmpl
	 * (in order to disable unverified flow)
	 */
	reg |= EC_GEN_EN_EXT_PTH_COMPLETION_BYPASS;
	al_reg_write32(&adapter->ec_regs_base->gen.en_ext, reg);

	/*TODO: add the following when we have updated regs file:
	 * reg_rfw_out_cfg_timestamp_sample_out
		0 (default) – use the timestamp from the SOP info (10G MAC)
		1 – use the timestamp from the EOP (1G MAC) (noly when MACSEC is disabled)
	 */
	return 0;
}

/* read Timestamp sample value of previously transmitted packet. */
int al_eth_tx_ts_val_get(struct al_hal_eth_adapter *adapter, uint8_t ts_index,
			 uint32_t *timestamp)
{
	al_assert(ts_index < AL_ETH_PTH_TX_SAMPLES_NUM);

	/* in 1G mode, only indexes 1-7 are allowed*/
	if (AL_ETH_IS_1G_MAC(adapter->mac_mode)) {
		al_assert(ts_index <= 7);
		al_assert(ts_index >= 1);
	}

	/*TODO: check if sample is valid */
	*timestamp = al_reg_read32(&adapter->ec_regs_base->pth_db[ts_index].ts);
	return 0;
}

/* Read the systime value */
int al_eth_pth_systime_read(struct al_hal_eth_adapter *adapter,
			    struct al_eth_pth_time *systime)
{
	uint32_t reg;

	/* first we must read the subseconds MSB so the seconds register will be
	 * shadowed
	 */
	reg = al_reg_read32(&adapter->ec_regs_base->pth.system_time_subseconds_msb);
	systime->femto = (uint64_t)reg << 18;
	reg = al_reg_read32(&adapter->ec_regs_base->pth.system_time_seconds);
	systime->seconds = reg;

	return 0;
}

/* Set the clock period to a given value. */
int al_eth_pth_clk_period_write(struct al_hal_eth_adapter *adapter,
				uint64_t clk_period)
{
	uint32_t reg;
	/* first write the LSB so it will be shadowed */
	/* bits 31:14 of the clock period lsb register contains bits 17:0 of the
	 * period.
	 */
	reg = (clk_period & AL_BIT_MASK(18)) << EC_PTH_CLOCK_PERIOD_LSB_VAL_SHIFT;
	al_reg_write32(&adapter->ec_regs_base->pth.clock_period_lsb, reg);
	reg = clk_period >> 18;
	al_reg_write32(&adapter->ec_regs_base->pth.clock_period_msb, reg);

	return 0;
}

/* Configure the systime internal update */
int al_eth_pth_int_update_config(struct al_hal_eth_adapter *adapter,
				 struct al_eth_pth_int_update_params const *params)
{
	uint32_t reg;

	reg = al_reg_read32(&adapter->ec_regs_base->pth.int_update_ctrl);
	if (params->enable == AL_FALSE) {
		reg &= ~EC_PTH_INT_UPDATE_CTRL_INT_TRIG_EN;
	} else {
		reg |= EC_PTH_INT_UPDATE_CTRL_INT_TRIG_EN;
		AL_REG_FIELD_SET(reg, EC_PTH_INT_UPDATE_CTRL_UPDATE_METHOD_MASK,
				 EC_PTH_INT_UPDATE_CTRL_UPDATE_METHOD_SHIFT,
				 params->method);
		if (params->trigger == AL_ETH_PTH_INT_TRIG_REG_WRITE)
			reg |= EC_PTH_INT_UPDATE_CTRL_UPDATE_TRIG;
		else
			reg &= ~EC_PTH_INT_UPDATE_CTRL_UPDATE_TRIG;
	}
	al_reg_write32(&adapter->ec_regs_base->pth.int_update_ctrl, reg);
	return 0;
}
/* set internal update time */
int al_eth_pth_int_update_time_set(struct al_hal_eth_adapter *adapter,
				   struct al_eth_pth_time const *time)
{
	uint32_t reg;

	al_reg_write32(&adapter->ec_regs_base->pth.int_update_seconds,
		       time->seconds);
	reg = time->femto & AL_BIT_MASK(18);
	reg = reg << EC_PTH_INT_UPDATE_SUBSECONDS_LSB_VAL_SHIFT;
	al_reg_write32(&adapter->ec_regs_base->pth.int_update_subseconds_lsb,
		       reg);
	reg = time->femto >> 18;
	al_reg_write32(&adapter->ec_regs_base->pth.int_update_subseconds_msb,
		       reg);

	return 0;
}

/* Configure the systime external update */
int al_eth_pth_ext_update_config(struct al_hal_eth_adapter *adapter,
				 struct al_eth_pth_ext_update_params const *params)
{
	uint32_t reg;

	reg = al_reg_read32(&adapter->ec_regs_base->pth.ext_update_ctrl);
	AL_REG_FIELD_SET(reg, EC_PTH_EXT_UPDATE_CTRL_UPDATE_METHOD_MASK,
			 EC_PTH_EXT_UPDATE_CTRL_UPDATE_METHOD_SHIFT,
			 params->method);

	AL_REG_FIELD_SET(reg, EC_PTH_EXT_UPDATE_CTRL_EXT_TRIG_EN_MASK,
			 EC_PTH_EXT_UPDATE_CTRL_EXT_TRIG_EN_SHIFT,
			 params->triggers);
	al_reg_write32(&adapter->ec_regs_base->pth.ext_update_ctrl, reg);
	return 0;
}

/* set external update time */
int al_eth_pth_ext_update_time_set(struct al_hal_eth_adapter *adapter,
				   struct al_eth_pth_time const *time)
{
	uint32_t reg;

	al_reg_write32(&adapter->ec_regs_base->pth.ext_update_seconds,
		       time->seconds);
	reg = time->femto & AL_BIT_MASK(18);
	reg = reg << EC_PTH_EXT_UPDATE_SUBSECONDS_LSB_VAL_SHIFT;
	al_reg_write32(&adapter->ec_regs_base->pth.ext_update_subseconds_lsb,
		       reg);
	reg = time->femto >> 18;
	al_reg_write32(&adapter->ec_regs_base->pth.ext_update_subseconds_msb,
		       reg);

	return 0;
};

/* set the read compensation delay */
int al_eth_pth_read_compensation_set(struct al_hal_eth_adapter *adapter,
				     uint64_t subseconds)
{
	uint32_t reg;

	/* first write to lsb to ensure atomicity */
	reg = (subseconds & AL_BIT_MASK(18)) << EC_PTH_READ_COMPENSATION_SUBSECONDS_LSB_VAL_SHIFT;
	al_reg_write32(&adapter->ec_regs_base->pth.read_compensation_subseconds_lsb, reg);

	reg = subseconds >> 18;
	al_reg_write32(&adapter->ec_regs_base->pth.read_compensation_subseconds_msb, reg);
	return 0;
}

/* set the internal write compensation delay */
int al_eth_pth_int_write_compensation_set(struct al_hal_eth_adapter *adapter,
					  uint64_t subseconds)
{
	uint32_t reg;

	/* first write to lsb to ensure atomicity */
	reg = (subseconds & AL_BIT_MASK(18)) << EC_PTH_INT_WRITE_COMPENSATION_SUBSECONDS_LSB_VAL_SHIFT;
	al_reg_write32(&adapter->ec_regs_base->pth.int_write_compensation_subseconds_lsb, reg);

	reg = subseconds >> 18;
	al_reg_write32(&adapter->ec_regs_base->pth.int_write_compensation_subseconds_msb, reg);
	return 0;
}

/* set the external write compensation delay */
int al_eth_pth_ext_write_compensation_set(struct al_hal_eth_adapter *adapter,
					  uint64_t subseconds)
{
	uint32_t reg;

	/* first write to lsb to ensure atomicity */
	reg = (subseconds & AL_BIT_MASK(18)) << EC_PTH_EXT_WRITE_COMPENSATION_SUBSECONDS_LSB_VAL_SHIFT;
	al_reg_write32(&adapter->ec_regs_base->pth.ext_write_compensation_subseconds_lsb, reg);

	reg = subseconds >> 18;
	al_reg_write32(&adapter->ec_regs_base->pth.ext_write_compensation_subseconds_msb, reg);
	return 0;
}

/* set the sync compensation delay */
int al_eth_pth_sync_compensation_set(struct al_hal_eth_adapter *adapter,
				     uint64_t subseconds)
{
	uint32_t reg;

	/* first write to lsb to ensure atomicity */
	reg = (subseconds & AL_BIT_MASK(18)) << EC_PTH_SYNC_COMPENSATION_SUBSECONDS_LSB_VAL_SHIFT;
	al_reg_write32(&adapter->ec_regs_base->pth.sync_compensation_subseconds_lsb, reg);

	reg = subseconds >> 18;
	al_reg_write32(&adapter->ec_regs_base->pth.sync_compensation_subseconds_msb, reg);
	return 0;
}

/* Configure an output pulse */
int al_eth_pth_pulse_out_config(struct al_hal_eth_adapter *adapter,
				struct al_eth_pth_pulse_out_params const *params)
{
	uint32_t reg;

	if (params->index >= AL_ETH_PTH_PULSE_OUT_NUM) {
		al_err("eth [%s] PTH out pulse index out of range\n",
				 adapter->name);
		return -EINVAL;
	}
	reg = al_reg_read32(&adapter->ec_regs_base->pth_egress[params->index].trigger_ctrl);
	if (params->enable == AL_FALSE) {
		reg &= ~EC_PTH_EGRESS_TRIGGER_CTRL_EN;
	} else {
		reg |= EC_PTH_EGRESS_TRIGGER_CTRL_EN;
		if (params->periodic == AL_FALSE)
			reg &= ~EC_PTH_EGRESS_TRIGGER_CTRL_PERIODIC;
		else
			reg |= EC_PTH_EGRESS_TRIGGER_CTRL_PERIODIC;

		AL_REG_FIELD_SET(reg, EC_PTH_EGRESS_TRIGGER_CTRL_PERIOD_SUBSEC_MASK,
				 EC_PTH_EGRESS_TRIGGER_CTRL_PERIOD_SUBSEC_SHIFT,
				 params->period_us);
		AL_REG_FIELD_SET(reg, EC_PTH_EGRESS_TRIGGER_CTRL_PERIOD_SEC_MASK,
				 EC_PTH_EGRESS_TRIGGER_CTRL_PERIOD_SEC_SHIFT,
				 params->period_sec);
	}
	al_reg_write32(&adapter->ec_regs_base->pth_egress[params->index].trigger_ctrl, reg);

	/* set trigger time */
	al_reg_write32(&adapter->ec_regs_base->pth_egress[params->index].trigger_seconds,
		       params->start_time.seconds);
	reg = params->start_time.femto & AL_BIT_MASK(18);
	reg = reg << EC_PTH_EGRESS_TRIGGER_SUBSECONDS_LSB_VAL_SHIFT;
	al_reg_write32(&adapter->ec_regs_base->pth_egress[params->index].trigger_subseconds_lsb,
		       reg);
	reg = params->start_time.femto >> 18;
	al_reg_write32(&adapter->ec_regs_base->pth_egress[params->index].trigger_subseconds_msb,
		       reg);

	/* set pulse width */
	reg = params->pulse_width & AL_BIT_MASK(18);
	reg = reg << EC_PTH_EGRESS_PULSE_WIDTH_SUBSECONDS_LSB_VAL_SHIFT;
	al_reg_write32(&adapter->ec_regs_base->pth_egress[params->index].pulse_width_subseconds_lsb, reg);

	reg = params->pulse_width  >> 18;
	al_reg_write32(&adapter->ec_regs_base->pth_egress[params->index].pulse_width_subseconds_msb, reg);

	return 0;
}

/**
* read ec_stat_counters
*/
int al_eth_ec_stats_get(struct al_hal_eth_adapter *adapter, struct al_eth_ec_stats *stats)
{
	al_assert(stats);
	stats->faf_in_rx_pkt = al_reg_read32(&adapter->ec_regs_base->stat.faf_in_rx_pkt);
	stats->faf_in_rx_short = al_reg_read32(&adapter->ec_regs_base->stat.faf_in_rx_short);
	stats->faf_in_rx_long = al_reg_read32(&adapter->ec_regs_base->stat.faf_in_rx_long);
	stats->faf_out_rx_pkt = al_reg_read32(&adapter->ec_regs_base->stat.faf_out_rx_pkt);
	stats->faf_out_rx_short = al_reg_read32(&adapter->ec_regs_base->stat.faf_out_rx_short);
	stats->faf_out_rx_long = al_reg_read32(&adapter->ec_regs_base->stat.faf_out_rx_long);
	stats->faf_out_drop = al_reg_read32(&adapter->ec_regs_base->stat.faf_out_drop);
	stats->rxf_in_rx_pkt = al_reg_read32(&adapter->ec_regs_base->stat.rxf_in_rx_pkt);
	stats->rxf_in_fifo_err = al_reg_read32(&adapter->ec_regs_base->stat.rxf_in_fifo_err);
	stats->lbf_in_rx_pkt = al_reg_read32(&adapter->ec_regs_base->stat.lbf_in_rx_pkt);
	stats->lbf_in_fifo_err = al_reg_read32(&adapter->ec_regs_base->stat.lbf_in_fifo_err);
	stats->rxf_out_rx_1_pkt = al_reg_read32(&adapter->ec_regs_base->stat.rxf_out_rx_1_pkt);
	stats->rxf_out_rx_2_pkt = al_reg_read32(&adapter->ec_regs_base->stat.rxf_out_rx_2_pkt);
	stats->rxf_out_drop_1_pkt = al_reg_read32(&adapter->ec_regs_base->stat.rxf_out_drop_1_pkt);
	stats->rxf_out_drop_2_pkt = al_reg_read32(&adapter->ec_regs_base->stat.rxf_out_drop_2_pkt);
	stats->rpe_1_in_rx_pkt = al_reg_read32(&adapter->ec_regs_base->stat.rpe_1_in_rx_pkt);
	stats->rpe_1_out_rx_pkt = al_reg_read32(&adapter->ec_regs_base->stat.rpe_1_out_rx_pkt);
	stats->rpe_2_in_rx_pkt = al_reg_read32(&adapter->ec_regs_base->stat.rpe_2_in_rx_pkt);
	stats->rpe_2_out_rx_pkt = al_reg_read32(&adapter->ec_regs_base->stat.rpe_2_out_rx_pkt);
	stats->rpe_3_in_rx_pkt = al_reg_read32(&adapter->ec_regs_base->stat.rpe_3_in_rx_pkt);
	stats->rpe_3_out_rx_pkt = al_reg_read32(&adapter->ec_regs_base->stat.rpe_3_out_rx_pkt);
	stats->tpe_in_tx_pkt = al_reg_read32(&adapter->ec_regs_base->stat.tpe_in_tx_pkt);
	stats->tpe_out_tx_pkt = al_reg_read32(&adapter->ec_regs_base->stat.tpe_out_tx_pkt);
	stats->tpm_tx_pkt = al_reg_read32(&adapter->ec_regs_base->stat.tpm_tx_pkt);
	stats->tfw_in_tx_pkt = al_reg_read32(&adapter->ec_regs_base->stat.tfw_in_tx_pkt);
	stats->tfw_out_tx_pkt = al_reg_read32(&adapter->ec_regs_base->stat.tfw_out_tx_pkt);
	stats->rfw_in_rx_pkt = al_reg_read32(&adapter->ec_regs_base->stat.rfw_in_rx_pkt);
	stats->rfw_in_vlan_drop = al_reg_read32(&adapter->ec_regs_base->stat.rfw_in_vlan_drop);
	stats->rfw_in_parse_drop = al_reg_read32(&adapter->ec_regs_base->stat.rfw_in_parse_drop);
	stats->rfw_in_mc = al_reg_read32(&adapter->ec_regs_base->stat.rfw_in_mc);
	stats->rfw_in_bc = al_reg_read32(&adapter->ec_regs_base->stat.rfw_in_bc);
	stats->rfw_in_vlan_exist = al_reg_read32(&adapter->ec_regs_base->stat.rfw_in_vlan_exist);
	stats->rfw_in_vlan_nexist = al_reg_read32(&adapter->ec_regs_base->stat.rfw_in_vlan_nexist);
	stats->rfw_in_mac_drop = al_reg_read32(&adapter->ec_regs_base->stat.rfw_in_mac_drop);
	stats->rfw_in_mac_ndet_drop = al_reg_read32(&adapter->ec_regs_base->stat.rfw_in_mac_ndet_drop);
	stats->rfw_in_ctrl_drop = al_reg_read32(&adapter->ec_regs_base->stat.rfw_in_ctrl_drop);
	stats->rfw_in_prot_i_drop = al_reg_read32(&adapter->ec_regs_base->stat.rfw_in_prot_i_drop);
	stats->eee_in = al_reg_read32(&adapter->ec_regs_base->stat.eee_in);
	return 0;
}

/**
 * read per_udma_counters
 */
int al_eth_ec_stat_udma_get(struct al_hal_eth_adapter *adapter, uint8_t idx,
	struct al_eth_ec_stat_udma *stats)
{
	struct al_ec_stat_udma __iomem *stat_udma;

	al_assert(idx <= 3); /*valid udma_id*/
	al_assert(stats);

	stat_udma = &adapter->ec_regs_base->stat_udma[idx];

	stats->rfw_out_rx_pkt = al_reg_read32(&stat_udma->rfw_out_rx_pkt);
	stats->rfw_out_drop = al_reg_read32(&stat_udma->rfw_out_drop);
	stats->msw_in_rx_pkt = al_reg_read32(&stat_udma->msw_in_rx_pkt);
	stats->msw_drop_q_full = al_reg_read32(&stat_udma->msw_drop_q_full);
	stats->msw_drop_sop = al_reg_read32(&stat_udma->msw_drop_sop);
	stats->msw_drop_eop = al_reg_read32(&stat_udma->msw_drop_eop);
	stats->msw_wr_eop = al_reg_read32(&stat_udma->msw_wr_eop);
	stats->msw_out_rx_pkt = al_reg_read32(&stat_udma->msw_out_rx_pkt);
	stats->tso_no_tso_pkt = al_reg_read32(&stat_udma->tso_no_tso_pkt);
	stats->tso_tso_pkt = al_reg_read32(&stat_udma->tso_tso_pkt);
	stats->tso_seg_pkt = al_reg_read32(&stat_udma->tso_seg_pkt);
	stats->tso_pad_pkt = al_reg_read32(&stat_udma->tso_pad_pkt);
	stats->tpm_tx_spoof = al_reg_read32(&stat_udma->tpm_tx_spoof);
	stats->tmi_in_tx_pkt = al_reg_read32(&stat_udma->tmi_in_tx_pkt);
	stats->tmi_out_to_mac = al_reg_read32(&stat_udma->tmi_out_to_mac);
	stats->tmi_out_to_rx = al_reg_read32(&stat_udma->tmi_out_to_rx);
	stats->tx_q0_bytes = al_reg_read32(&stat_udma->tx_q0_bytes);
	stats->tx_q1_bytes = al_reg_read32(&stat_udma->tx_q1_bytes);
	stats->tx_q2_bytes = al_reg_read32(&stat_udma->tx_q2_bytes);
	stats->tx_q3_bytes = al_reg_read32(&stat_udma->tx_q3_bytes);
	stats->tx_q0_pkts = al_reg_read32(&stat_udma->tx_q0_pkts);
	stats->tx_q1_pkts = al_reg_read32(&stat_udma->tx_q1_pkts);
	stats->tx_q2_pkts = al_reg_read32(&stat_udma->tx_q2_pkts);
	stats->tx_q3_pkts = al_reg_read32(&stat_udma->tx_q3_pkts);

	stats->tx_q_pkts[0] = stats->tx_q0_pkts;
	stats->tx_q_pkts[1] = stats->tx_q1_pkts;
	stats->tx_q_pkts[2] = stats->tx_q2_pkts;
	stats->tx_q_pkts[3] = stats->tx_q3_pkts;
	stats->tx_q_bytes[0] = stats->tx_q0_bytes;
	stats->tx_q_bytes[1] = stats->tx_q1_bytes;
	stats->tx_q_bytes[2] = stats->tx_q2_bytes;
	stats->tx_q_bytes[3] = stats->tx_q3_bytes;

	if (adapter->rev_id >= AL_ETH_REV_ID_4) {

		stats->tx_q_pkts[4] = al_reg_read32(&stat_udma->tx_q4_pkts);
		stats->tx_q_pkts[5] = al_reg_read32(&stat_udma->tx_q5_pkts);
		stats->tx_q_pkts[6] = al_reg_read32(&stat_udma->tx_q6_pkts);
		stats->tx_q_pkts[7] = al_reg_read32(&stat_udma->tx_q7_pkts);
		stats->tx_q_pkts[8] = al_reg_read32(&stat_udma->tx_q8_pkts);
		stats->tx_q_pkts[9] = al_reg_read32(&stat_udma->tx_q9_pkts);
		stats->tx_q_pkts[10] = al_reg_read32(&stat_udma->tx_q10_pkts);
		stats->tx_q_pkts[11] = al_reg_read32(&stat_udma->tx_q11_pkts);
		stats->tx_q_pkts[12] = al_reg_read32(&stat_udma->tx_q12_pkts);
		stats->tx_q_pkts[13] = al_reg_read32(&stat_udma->tx_q13_pkts);
		stats->tx_q_pkts[14] = al_reg_read32(&stat_udma->tx_q14_pkts);
		stats->tx_q_pkts[15] = al_reg_read32(&stat_udma->tx_q15_pkts);
		stats->tx_q_bytes[4] = al_reg_read32(&stat_udma->tx_q4_bytes);
		stats->tx_q_bytes[5] = al_reg_read32(&stat_udma->tx_q5_bytes);
		stats->tx_q_bytes[6] = al_reg_read32(&stat_udma->tx_q6_bytes);
		stats->tx_q_bytes[7] = al_reg_read32(&stat_udma->tx_q7_bytes);
		stats->tx_q_bytes[8] = al_reg_read32(&stat_udma->tx_q8_bytes);
		stats->tx_q_bytes[9] = al_reg_read32(&stat_udma->tx_q9_bytes);
		stats->tx_q_bytes[10] = al_reg_read32(&stat_udma->tx_q10_bytes);
		stats->tx_q_bytes[11] = al_reg_read32(&stat_udma->tx_q11_bytes);
		stats->tx_q_bytes[12] = al_reg_read32(&stat_udma->tx_q12_bytes);
		stats->tx_q_bytes[13] = al_reg_read32(&stat_udma->tx_q13_bytes);
		stats->tx_q_bytes[14] = al_reg_read32(&stat_udma->tx_q14_bytes);
		stats->tx_q_bytes[15] = al_reg_read32(&stat_udma->tx_q15_bytes);
	}

	return 0;
}

/* Traffic control */


int al_eth_flr_rmn(int (* pci_read_config_u32)(void *handle, int where, uint32_t *val),
		   int (* pci_write_config_u32)(void *handle, int where, uint32_t val),
		   void *handle,
		   void __iomem	*mac_base)
{
	struct al_eth_mac_regs __iomem *mac_regs_base =
		(struct	al_eth_mac_regs __iomem *)mac_base;
	uint32_t cfg_reg_store[10];
	uint32_t reg;
	uint32_t mux_sel;
	int i = 0;
	int j;

	(*pci_read_config_u32)(handle, AL_ADAPTER_GENERIC_CONTROL_0, &reg);

	/* reset 1G mac */
	AL_REG_MASK_SET(reg, AL_ADAPTER_GENERIC_CONTROL_0_ETH_RESET_1GMAC);
	(*pci_write_config_u32)(handle, AL_ADAPTER_GENERIC_CONTROL_0, reg);
#if (!defined(AL_ETH_FAST_FLR)) || (AL_ETH_FAST_FLR == 0)
	al_udelay(1000);
#else
	al_udelay(1);
#endif
	/* don't reset 1G mac */
	AL_REG_MASK_CLEAR(reg, AL_ADAPTER_GENERIC_CONTROL_0_ETH_RESET_1GMAC);
	/* prevent 1G mac reset on FLR */
	AL_REG_MASK_CLEAR(reg, AL_ADAPTER_GENERIC_CONTROL_0_ETH_RESET_1GMAC_ON_FLR);
	/* prevent adapter reset */
	(*pci_write_config_u32)(handle, AL_ADAPTER_GENERIC_CONTROL_0, reg);

	mux_sel = al_reg_read32(&mac_regs_base->gen.mux_sel);

	/* save pci register that get reset due to flr*/
	(*pci_read_config_u32)(handle, AL_PCI_COMMAND, &cfg_reg_store[i++]);
	(*pci_read_config_u32)(handle, 0xC, &cfg_reg_store[i++]);
	(*pci_read_config_u32)(handle, 0x10, &cfg_reg_store[i++]);
	(*pci_read_config_u32)(handle, 0x18, &cfg_reg_store[i++]);
	(*pci_read_config_u32)(handle, 0x20, &cfg_reg_store[i++]);
	(*pci_read_config_u32)(handle, 0x110, &cfg_reg_store[i++]);
	(*pci_read_config_u32)(handle, AL_ADAPTER_SMCC_CONF_2, &cfg_reg_store[i++]);
	for (j = 1; j < AL_ADAPTER_SMCC_BUNDLE_NUM; j++)
		(*pci_read_config_u32)(handle, (AL_ADAPTER_SMCC + j * AL_ADAPTER_SMCC_BUNDLE_SIZE),
			       &cfg_reg_store[i++]);

	/* do flr */
	(*pci_write_config_u32)(handle, AL_PCI_EXP_CAP_BASE + AL_PCI_EXP_DEVCTL, AL_PCI_EXP_DEVCTL_BCR_FLR);
#if (!defined(AL_ETH_FAST_FLR)) || (AL_ETH_FAST_FLR == 0)
	al_udelay(1000);
#else
	al_udelay(1);
#endif
	/* restore command */
	i = 0;
	(*pci_write_config_u32)(handle, AL_PCI_COMMAND, cfg_reg_store[i++]);
	(*pci_write_config_u32)(handle, 0xC, cfg_reg_store[i++]);
	(*pci_write_config_u32)(handle, 0x10, cfg_reg_store[i++]);
	(*pci_write_config_u32)(handle, 0x18, cfg_reg_store[i++]);
	(*pci_write_config_u32)(handle, 0x20, cfg_reg_store[i++]);
	(*pci_write_config_u32)(handle, 0x110, cfg_reg_store[i++]);
	(*pci_write_config_u32)(handle, AL_ADAPTER_SMCC_CONF_2, cfg_reg_store[i++]);
	for (j = 1; j < AL_ADAPTER_SMCC_BUNDLE_NUM; j++)
		(*pci_write_config_u32)(handle, (AL_ADAPTER_SMCC + j * AL_ADAPTER_SMCC_BUNDLE_SIZE),
				cfg_reg_store[i++]);

	al_reg_write32_masked(&mac_regs_base->gen.mux_sel, ETH_MAC_GEN_MUX_SEL_KR_IN_MASK, mux_sel);

	/* set SGMII clock to 125MHz */
	al_reg_write32(&mac_regs_base->sgmii.clk_div, 0x03320501);

	/* reset 1G mac */
	AL_REG_MASK_SET(reg, AL_ADAPTER_GENERIC_CONTROL_0_ETH_RESET_1GMAC);
	(*pci_write_config_u32)(handle, AL_ADAPTER_GENERIC_CONTROL_0, reg);

#if (!defined(AL_ETH_FAST_FLR)) || (AL_ETH_FAST_FLR == 0)
	al_udelay(1000);
#else
	al_udelay(1);
#endif

	/* clear 1G mac reset */
	AL_REG_MASK_CLEAR(reg, AL_ADAPTER_GENERIC_CONTROL_0_ETH_RESET_1GMAC);
	(*pci_write_config_u32)(handle, AL_ADAPTER_GENERIC_CONTROL_0, reg);

	/* reset SGMII mac clock to default */
	al_reg_write32(&mac_regs_base->sgmii.clk_div, 0x00320501);
#if (!defined(AL_ETH_FAST_FLR)) || (AL_ETH_FAST_FLR == 0)
	al_udelay(1000);
#else
	al_udelay(1);
#endif
	/* reset async fifo */
	reg = al_reg_read32(&mac_regs_base->gen.sd_fifo_ctrl);
	AL_REG_MASK_SET(reg, 0xF0);
	al_reg_write32(&mac_regs_base->gen.sd_fifo_ctrl, reg);
	reg = al_reg_read32(&mac_regs_base->gen.sd_fifo_ctrl);
	AL_REG_MASK_CLEAR(reg, 0xF0);
	al_reg_write32(&mac_regs_base->gen.sd_fifo_ctrl, reg);

	return 0;
}

int al_eth_flr_rmn_restore_params(int (* pci_read_config_u32)(void *handle, int where, uint32_t *val),
		int (* pci_write_config_u32)(void *handle, int where, uint32_t val),
		void *handle,
		void __iomem    *mac_base,
		void __iomem    *ec_base,
		int     mac_addresses_num
		)
{
	struct al_eth_board_params params = { .media_type = 0 };
	uint8_t mac_addr[6];
	int rc;

	/* not implemented yet */
	if (mac_addresses_num > 1)
		return -EPERM;

	/* save board params so we restore it after reset */
	al_eth_board_params_get(mac_base, &params);
	al_eth_mac_addr_read(ec_base, 0, mac_addr);

	rc = al_eth_flr_rmn(pci_read_config_u32, pci_write_config_u32, handle, mac_base);
	al_eth_board_params_set(mac_base, &params);
	al_eth_mac_addr_store(ec_base, 0, mac_addr);

	return rc;
}

static void al_eth_flr_v4(
	struct al_hal_eth_adapter	*adapter,
	struct al_eth_flr_params	*params)
{
	struct al_eth_board_params board_params = { .media_type = 0 };
	struct al_eth_common_udma_select_params udma_select_params;
	void *handle = params->handle;
	uint32_t cfg_reg_store[11];
	uint8_t mac_addr[6];
	uint32_t reg_val;
	uint32_t reg_clr;
	uint32_t reg_set;
	uint32_t reg_set_default;
	uint32_t reg_clear_default;
	uint32_t pulse_reg;
	unsigned int i;
	unsigned int j;
	unsigned int smcc_bundle_num;
	al_bool vf_flr = AL_FALSE;
	unsigned int vf_num;
	uint32_t gen_control_0_default_val;
	struct al_eth_common_handle eth_common_handle = {NULL};
	struct al_eth_common_handle_init_params eth_common_handle_init_params = {NULL};
	struct al_eth_common_adp2func_flr_regs adp2func_flr_regs;
	struct al_eth_common_adp2func_flr_udma_attr udma_attr = {0};

	al_memset(&udma_select_params, 0, sizeof(udma_select_params));

	/* Save adapter regs which are reset upon FLR, but should be restored */
	al_eth_common_udma_select_config_get(
			adapter->ec_regs_base, &udma_select_params);
	al_eth_board_params_get_ex(adapter, &board_params);
	al_eth_mac_addr_read(adapter->ec_regs_base, 0, mac_addr);
	i = 0;
	params->pci_read_config_u32(handle, AL_PCI_COMMAND, &cfg_reg_store[i++]);
	params->pci_read_config_u32(handle, PCI_CLS_LT_HT_BIST, &cfg_reg_store[i++]);
	params->pci_read_config_u32(handle, AL_PCI_BASE_ADDRESS_0, &cfg_reg_store[i++]);
	params->pci_read_config_u32(handle, AL_PCI_BASE_ADDRESS_2, &cfg_reg_store[i++]);
	params->pci_read_config_u32(handle, AL_PCI_BASE_ADDRESS_4, &cfg_reg_store[i++]);
	params->pci_read_config_u32(handle, AL_ADAPTER_GENERIC_CONTROL_19, &cfg_reg_store[i++]);
	/**
	 * PF FLR - save & restore the VF's SMCC as well.
	 * VF FLR - SMCC not effected as they are not part of VF bundle and exists only in PF space
	 */
	smcc_bundle_num = vf_flr ? 0 : AL_ADAPTER_SMCC_BUNDLE_NUM;
	for (j = 0; j < smcc_bundle_num; j++)
		params->pci_read_config_u32(
			handle, (AL_ADAPTER_SMCC + j * AL_ADAPTER_SMCC_BUNDLE_SIZE),
			&cfg_reg_store[i++]);
	if (!vf_flr)
		params->pci_read_config_u32(handle, AL_ADAPTER_SMCC_CONF_2, &cfg_reg_store[i++]);

	/** default bit map of set bits - mask all resets besides PF PCIE CFG */
	reg_set_default =
			AL_ADAPTER_GENERIC_CONTROL_0_VF_UDMA_RST_MASK(0) |
			AL_ADAPTER_GENERIC_CONTROL_0_VF_UDMA_RST_MASK(1) |
			AL_ADAPTER_GENERIC_CONTROL_0_VF_UDMA_RST_MASK(2) |
			AL_ADAPTER_GENERIC_CONTROL_0_PF_RST_MASK |
			AL_ADAPTER_GENERIC_CONTROL_0_VF_RST_MASK(0) |
			AL_ADAPTER_GENERIC_CONTROL_0_VF_RST_MASK(1) |
			AL_ADAPTER_GENERIC_CONTROL_0_VF_RST_MASK(2) |
			AL_ADAPTER_GENERIC_CONTROL_0_AXI_ARB_RESET_MASK |
			AL_ADAPTER_GENERIC_CONTROL_0_VF_PCIE_CFG_RST_MASK(0) |
			AL_ADAPTER_GENERIC_CONTROL_0_VF_PCIE_CFG_RST_MASK(1) |
			AL_ADAPTER_GENERIC_CONTROL_0_VF_PCIE_CFG_RST_MASK(2);
	/** default bit map of cleared bits - mask all reset of applications */
	reg_clear_default =
			AL_ADAPTER_GENERIC_CONTROL_0_CFG_REGS_RESET_MASK |
			AL_ADAPTER_GENERIC_CONTROL_0_ETH_V4_EC_RST_EN |
			AL_ADAPTER_GENERIC_CONTROL_0_ETH_V4_EC_REG_RST_EN |
			AL_ADAPTER_GENERIC_CONTROL_0_ETH_V4_MAC_RST_EN |
			AL_ADAPTER_GENERIC_CONTROL_0_ETH_V4_SCACHE_RST_EN |
			AL_ADAPTER_GENERIC_CONTROL_0_ETH_V4_SRESOURCE_RST_EN |
			AL_ADAPTER_GENERIC_CONTROL_0_ETH_V4_EC_RST_EN_SELF |
			AL_ADAPTER_GENERIC_CONTROL_0_ETH_V4_EC_REG_RST_EN_SELF |
			AL_ADAPTER_GENERIC_CONTROL_0_ETH_V4_MAC_RST_EN_SELF |
			AL_ADAPTER_GENERIC_CONTROL_0_ETH_V4_SCACHE_RST_EN_SELF |
			AL_ADAPTER_GENERIC_CONTROL_0_ETH_V4_SRESOURCE_RST_EN;

	switch (params->type) {
	case AL_ETH_FLR_TYPE_FULL:

		/*
		 * Currently, Don't enable FULL FLR at all - should have a really good reason to use it
		 * and should consult Ethernet & Verification team.
		 */
		al_assert(0);
		al_assert(adapter->udma_id == 0); /* FULL FLR only from port 0 */

		reg_clr =
			AL_ADAPTER_GENERIC_CONTROL_0_PF_RST_MASK;

		reg_set =
			AL_ADAPTER_GENERIC_CONTROL_0_ETH_V4_EC_RST_EN |
			AL_ADAPTER_GENERIC_CONTROL_0_ETH_V4_EC_REG_RST_EN |
			AL_ADAPTER_GENERIC_CONTROL_0_ETH_V4_MAC_RST_EN |
			AL_ADAPTER_GENERIC_CONTROL_0_ETH_V4_SCACHE_RST_EN |
			AL_ADAPTER_GENERIC_CONTROL_0_ETH_V4_SRESOURCE_RST_EN;

		/**
		 * Addressing RMN 12445
		 *
		 * ETH Advanced: EC_0 (application) soft reset (flr) is forbidden (affects the whole chip)
		 * SW W/A : Do not enable this reset
		 */
		reg_set &= ~AL_ADAPTER_GENERIC_CONTROL_0_ETH_V4_EC_RST_EN;
		break;
	case AL_ETH_FLR_TYPE_PORT:
		/** Only UDMA 0 is allowed to FLR a port */
		al_assert(params->udma_num == 0);
		/**
		 * Reset the MACm UDMA PF - EC should not be reset as it will lose
		 * proprietary configuration set in earlier boot stages
		 */
		reg_clr =
			AL_ADAPTER_GENERIC_CONTROL_0_PF_RST_MASK;
		reg_set =
			AL_ADAPTER_GENERIC_CONTROL_0_ETH_V4_MAC_RST_EN;

		break;
	case AL_ETH_FLR_TYPE_UDMA:
		/**
		 * Addressing RMN 11527
		 * ETH advanced: UDMA FLR in port 0 may affect other ports
		 * SW W/A: Use UDMA_QUEUES_FLR instead
		 */
		if (AL_ETH_V4_IS_ADV(adapter->rev_id, adapter->dev_id)) {
			al_err("%s: eth v%d:dev id %d UDMA FLR not allowed, use UDMA_QUEUES_FLR\n",
				__func__, adapter->rev_id, adapter->dev_id);
			al_assert(0);
			return;
		}

		if (!params->udma_num) {
			/* unmask PF reset */
			reg_clr =
				AL_ADAPTER_GENERIC_CONTROL_0_PF_RST_MASK;
			reg_set = 0;
		} else {
			/* UDMA 1,2,3 FLR utilizes the per VF FLR
			 * if PF FLR is issued
			 *
			 */
			/* unmask VF resets */
			reg_clr =
				AL_ADAPTER_GENERIC_CONTROL_0_VF_UDMA_RST_MASK(0) |
				AL_ADAPTER_GENERIC_CONTROL_0_VF_UDMA_RST_MASK(1) |
				AL_ADAPTER_GENERIC_CONTROL_0_VF_UDMA_RST_MASK(2) |
				AL_ADAPTER_GENERIC_CONTROL_0_VF_PCIE_CFG_RST_MASK(0) |
				AL_ADAPTER_GENERIC_CONTROL_0_VF_PCIE_CFG_RST_MASK(1) |
				AL_ADAPTER_GENERIC_CONTROL_0_VF_PCIE_CFG_RST_MASK(2);
			reg_set = 0;

			vf_flr = AL_TRUE;
			vf_num = params->udma_num - 1; /** not to be confused with udma mask */
			/**
			 * Notice - VF FLR is performed if PF FLR is done and
			 * bits :
			 *   AL_ADAPTER_GENERIC_CONTROL_0_VF_RST_MASK(i)
			 * are set to 0
			 */
		}
		break;
	case AL_ETH_FLR_TYPE_ALL_UDMAS:
		al_assert(adapter->eth_common_regs_base);
		al_assert(AL_ETH_V4_IS_ADV(adapter->rev_id, adapter->dev_id));

		eth_common_handle_init_params.eth_common_regs_base = adapter->eth_common_regs_base;
		al_eth_common_handle_init(&eth_common_handle, &eth_common_handle_init_params);

		udma_attr.adapter_num = params->port_idx;
		udma_attr.flr_bit = 0;

		/* Route the flr line from this adapter to all UDMAs */
		al_eth_common_adp2func_flr_read_raw(&eth_common_handle,
						    &adp2func_flr_regs);

		for (i = 0; i < AL_ETH_UDMA_MAX_ADV_REV_ID_4; i++) {
			udma_attr.udma_num = i;
			al_eth_common_adp2func_flr_udma_config(&eth_common_handle,
							       &udma_attr);
		}

		/* unmask PF reset */
		reg_clr = AL_ADAPTER_GENERIC_CONTROL_0_PF_RST_MASK;
		/* mask PCIe config space reset */
		reg_set = AL_ADAPTER_GENERIC_CONTROL_0_CFG_REGS_RESET_MASK;

		break;
	default:
		al_assert(0);
		return;
	}

	/* Configure FLR behavior register according to FLR type */
	params->pci_read_config_u32(handle, AL_ADAPTER_GENERIC_CONTROL_0, &reg_val);
	reg_val &= ~reg_clear_default;
	reg_val |= reg_set_default;

	reg_val &= ~reg_clr;
	reg_val |= reg_set;
	params->pci_write_config_u32(handle, AL_ADAPTER_GENERIC_CONTROL_0, reg_val);

	/* Actual FLR */
	if (vf_flr)
		params->pci_write_config_u32(params->handle, AL_ADPTR_VF_CTRL_STATUS(vf_num),
			AL_ADPTR_VF_CTRL_STATUS_FLR);
	else
		params->pci_write_config_u32(handle, AL_PCI_EXP_CAP_BASE + AL_PCI_EXP_DEVCTL,
			AL_PCI_EXP_DEVCTL_BCR_FLR);
	/** delay should be enough for effected units (EC/MAC/SCACHE etc..) to finish restarting */
	al_udelay(100);

	/** make sure FLR bit is cleared */
	if (vf_flr) {
		params->pci_read_config_u32(params->handle, AL_PCI_EXP_CAP_BASE + AL_PCI_EXP_DEVCTL,
			&pulse_reg);
		params->pci_write_config_u32(params->handle, AL_ADPTR_VF_CTRL_STATUS(vf_num),
			pulse_reg & ~AL_ADPTR_VF_CTRL_STATUS_FLR);
	} else {
		params->pci_read_config_u32(handle, AL_PCI_EXP_CAP_BASE + AL_PCI_EXP_DEVCTL,
			&pulse_reg);
		params->pci_write_config_u32(handle, AL_PCI_EXP_CAP_BASE + AL_PCI_EXP_DEVCTL,
			pulse_reg & ~AL_PCI_EXP_DEVCTL_BCR_FLR);
	}

	/* Restore adapter saved registers */
	i = 0;
	params->pci_write_config_u32(handle, AL_PCI_COMMAND, cfg_reg_store[i++]);
	params->pci_write_config_u32(handle, PCI_CLS_LT_HT_BIST, cfg_reg_store[i++]);
	params->pci_write_config_u32(handle, AL_PCI_BASE_ADDRESS_0, cfg_reg_store[i++]);
	params->pci_write_config_u32(handle, AL_PCI_BASE_ADDRESS_2, cfg_reg_store[i++]);
	params->pci_write_config_u32(handle, AL_PCI_BASE_ADDRESS_4, cfg_reg_store[i++]);
	params->pci_write_config_u32(handle, AL_ADAPTER_GENERIC_CONTROL_19, cfg_reg_store[i++]);
	for (j = 0; j < smcc_bundle_num; j++)
		params->pci_write_config_u32(
			handle, (AL_ADAPTER_SMCC + j * AL_ADAPTER_SMCC_BUNDLE_SIZE),
			cfg_reg_store[i++]);
	if (!vf_flr)
		params->pci_write_config_u32(handle, AL_ADAPTER_SMCC_CONF_2, cfg_reg_store[i++]);

	al_eth_board_params_set_ex(adapter, &board_params);
	al_eth_common_udma_select_config_set(
			adapter->ec_regs_base, &udma_select_params);
	al_eth_mac_addr_store(adapter->ec_regs_base, 0, mac_addr);

	if (params->type == AL_ETH_FLR_TYPE_ALL_UDMAS)
		/* Restore original values */
		al_eth_common_adp2func_flr_write_raw(&eth_common_handle,
						     &adp2func_flr_regs);

	/* After performing the FLR, we set the AL_ADAPTER_GENERIC_CONTROL_0 reg
	 * to our default value. This leaves us in a "clean" state and is
	 * also required for supporting AL_ETH_FLR_TYPE_ALL_UDMAS, mainly to ensure
	 * that the the following bits are cleared in all adapters:
	 *   - AL_ADAPTER_GENERIC_CONTROL_0_ETH_V4_EC_RST_EN
	 *   - AL_ADAPTER_GENERIC_CONTROL_0_ETH_V4_EC_REG_RST_EN
	 *   - AL_ADAPTER_GENERIC_CONTROL_0_ETH_V4_MAC_RST_EN
	 */

	gen_control_0_default_val = ~reg_clear_default &
		AL_ADAPTER_GENERIC_CONTROL_0_ETH_V4_HW_RESET_VAL;
	gen_control_0_default_val |= reg_set_default;

	params->pci_write_config_u32(handle,
				     AL_ADAPTER_GENERIC_CONTROL_0,
				     gen_control_0_default_val);
}

static int al_eth_flr_udma_queues(struct al_hal_eth_adapter *adapter)
{
	int err;
	struct al_udma *udma;

	al_assert(adapter);

	udma = &adapter->tx_udma;
	err = al_udma_q_reset_all(udma);
	if (err) {
		al_err("%s: al_udma_q_reset_all failed(%s)!\n", __func__, udma->name);
		return err;
	}

	udma = &adapter->rx_udma;
	err = al_udma_q_reset_all(udma);
	if (err) {
		al_err("%s: al_udma_q_reset_all failed(%s)!\n", __func__, udma->name);
		return err;
	}

	return 0;
}

void al_eth_flr(
	struct al_hal_eth_adapter	*adapter,
	struct al_eth_flr_params	*params)
{
	if (params->type == AL_ETH_FLR_TYPE_UDMA_QUEUES) {
		int err;
		err = al_eth_flr_udma_queues(adapter);
		if (err)
			al_err("%s: Failed al_eth_flr_udma_queues, err = %d\n", __func__, err);
	} else if (adapter->rev_id <= AL_ETH_REV_ID_3) {
		al_assert(params->type == AL_ETH_FLR_TYPE_FULL);
		al_eth_flr_rmn_restore_params(
			params->pci_read_config_u32,
			params->pci_write_config_u32,
			params->handle,
			adapter->mac_regs_base,
			adapter->ec_regs_base,
			1);
	} else {
		al_eth_flr_v4(adapter, params);
	}
}

/* board params register 1 */
#define AL_HAL_ETH_MEDIA_TYPE_MASK	(AL_FIELD_MASK(3, 0))
#define AL_HAL_ETH_MEDIA_TYPE_SHIFT	0
#define AL_HAL_ETH_EXT_PHY_SHIFT	4
#define AL_HAL_ETH_PHY_ADDR_MASK	(AL_FIELD_MASK(9, 5))
#define AL_HAL_ETH_PHY_ADDR_SHIFT	5
#define AL_HAL_ETH_SFP_EXIST_SHIFT	10
#define AL_HAL_ETH_AN_ENABLE_SHIFT	11
#define AL_HAL_ETH_KR_LT_ENABLE_SHIFT	12
#define AL_HAL_ETH_KR_FEC_ENABLE_SHIFT	13
#define AL_HAL_ETH_MDIO_FREQ_MASK	(AL_FIELD_MASK(15, 14))
#define AL_HAL_ETH_MDIO_FREQ_SHIFT	14
#define AL_HAL_ETH_I2C_ADAPTER_ID_MASK	(AL_FIELD_MASK(19, 16))
#define AL_HAL_ETH_I2C_ADAPTER_ID_SHIFT	16
#define AL_HAL_ETH_EXT_PHY_IF_MASK	(AL_FIELD_MASK(21, 20))
#define AL_HAL_ETH_EXT_PHY_IF_SHIFT	20
#define AL_HAL_ETH_AUTO_NEG_MODE_SHIFT	22
#define AL_HAL_ETH_SERDES_GRP_2_SHIFT	23
#define AL_HAL_ETH_SERDES_GRP_MASK	(AL_FIELD_MASK(26, 25))
#define AL_HAL_ETH_SERDES_GRP_SHIFT	25
#define AL_HAL_ETH_SERDES_LANE_LO_MASK	(AL_FIELD_MASK(28, 27))
#define AL_HAL_ETH_SERDES_LANE_LO_SHIFT 27
#define AL_HAL_ETH_REF_CLK_FREQ_MASK	(AL_FIELD_MASK(31, 29))
#define AL_HAL_ETH_REF_CLK_FREQ_SHIFT	29

/* board params register 2 */
#define AL_HAL_ETH_DONT_OVERRIDE_SERDES_SHIFT	0
#define AL_HAL_ETH_1000_BASE_X_SHIFT		1
#define AL_HAL_ETH_1G_AN_DISABLE_SHIFT		2
#define AL_HAL_ETH_1G_SPEED_MASK		(AL_FIELD_MASK(4, 3))
#define AL_HAL_ETH_1G_SPEED_SHIFT		3
#define AL_HAL_ETH_1G_HALF_DUPLEX_SHIFT		5
#define AL_HAL_ETH_1G_FC_DISABLE_SHIFT		6
#define AL_HAL_ETH_RETIMER_EXIST_SHIFT		7
#define AL_HAL_ETH_RETIMER_BUS_ID_MASK		(AL_FIELD_MASK(11, 8))
#define AL_HAL_ETH_RETIMER_BUS_ID_SHIFT		8
#define AL_HAL_ETH_RETIMER_I2C_ADDR_MASK	(AL_FIELD_MASK(18, 12))
#define AL_HAL_ETH_RETIMER_I2C_ADDR_SHIFT	12
#define AL_HAL_ETH_RETIMER_CHANNEL_SHIFT	19
#define AL_HAL_ETH_DAC_LENGTH_MASK		(AL_FIELD_MASK(23, 20))
#define AL_HAL_ETH_DAC_LENGTH_SHIFT		20
#define AL_HAL_ETH_DAC_SHIFT			24
#define AL_HAL_ETH_RETIMER_TYPE_MASK		(AL_FIELD_MASK(26, 25))
#define AL_HAL_ETH_RETIMER_TYPE_SHIFT		25
#define AL_HAL_ETH_RETIMER_CHANNEL_2_MASK	(AL_FIELD_MASK(28, 27))
#define AL_HAL_ETH_RETIMER_CHANNEL_2_SHIFT	27
#define AL_HAL_ETH_RETIMER_TX_CHANNEL_MASK	(AL_FIELD_MASK(31, 29))
#define AL_HAL_ETH_RETIMER_TX_CHANNEL_SHIFT	29

/* board params register 3 */
#define AL_HAL_ETH_GPIO_SFP_PRESENT_MASK	(AL_FIELD_MASK(5, 0))
#define AL_HAL_ETH_GPIO_SFP_PRESENT_SHIFT	0
#define AL_HAL_ETH_AUTO_FEC_ENABLE_SHIFT	6
#define AL_HAL_ETH_GPIO_SPD_1G_10G_MASK		(AL_FIELD_MASK(12, 7))
#define AL_HAL_ETH_GPIO_SPD_1G_10G_SHIFT	7
#define AL_HAL_ETH_GPIO_SPD_25G_MASK		(AL_FIELD_MASK(18, 13))
#define AL_HAL_ETH_GPIO_SPD_25G_SHIFT		13
#define AL_HAL_ETH_COMMON_MODE_MASK			(AL_FIELD_MASK(22, 19))
#define AL_HAL_ETH_COMMON_MODE_SHIFT		19
#define AL_HAL_ETH_SERDES_LANE_HI_MASK		(AL_FIELD_MASK(24, 23))
#define AL_HAL_ETH_SERDES_LANE_HI_SHIFT	23

#define AL_HAL_ETH_SERDES_LANE_LO(lane) ((lane) & 0x3)
#define AL_HAL_ETH_SERDES_LANE_HI(lane) (((lane >> 2)) & 0x3)
#define AL_HAL_ETH_SERDES_LANE(lane_hi, lane_lo) (((lane_hi) << 2) | (lane_lo))

static void al_eth_board_params_set_scratch_regs(uint32_t **scratch_regs,
	struct al_eth_board_params *params)
{
	uint32_t reg;

	/* ************* Setting Board params register 1 **************** */
	reg = 0;
	AL_REG_FIELD_SET(reg, AL_HAL_ETH_MEDIA_TYPE_MASK,
			 AL_HAL_ETH_MEDIA_TYPE_SHIFT, params->media_type);
	AL_REG_BIT_VAL_SET(reg, AL_HAL_ETH_EXT_PHY_SHIFT, params->phy_exist == AL_TRUE);
	AL_REG_FIELD_SET(reg, AL_HAL_ETH_PHY_ADDR_MASK,
			 AL_HAL_ETH_PHY_ADDR_SHIFT, params->phy_mdio_addr);

	AL_REG_BIT_VAL_SET(reg, AL_HAL_ETH_SFP_EXIST_SHIFT, params->sfp_plus_module_exist == AL_TRUE);

	AL_REG_BIT_VAL_SET(reg, AL_HAL_ETH_AN_ENABLE_SHIFT, params->autoneg_enable == AL_TRUE);
	AL_REG_BIT_VAL_SET(reg, AL_HAL_ETH_KR_LT_ENABLE_SHIFT, params->kr_lt_enable == AL_TRUE);
	AL_REG_BIT_VAL_SET(reg, AL_HAL_ETH_KR_FEC_ENABLE_SHIFT, params->kr_fec_enable == AL_TRUE);
	AL_REG_FIELD_SET(reg, AL_HAL_ETH_MDIO_FREQ_MASK,
			 AL_HAL_ETH_MDIO_FREQ_SHIFT, params->mdio_freq);
	AL_REG_FIELD_SET(reg, AL_HAL_ETH_I2C_ADAPTER_ID_MASK,
			 AL_HAL_ETH_I2C_ADAPTER_ID_SHIFT, params->i2c_adapter_id);
	AL_REG_FIELD_SET(reg, AL_HAL_ETH_EXT_PHY_IF_MASK,
			 AL_HAL_ETH_EXT_PHY_IF_SHIFT, params->phy_if);

	AL_REG_BIT_VAL_SET(reg, AL_HAL_ETH_AUTO_NEG_MODE_SHIFT,
			   params->an_mode == AL_ETH_BOARD_AUTONEG_IN_BAND);

	AL_REG_FIELD_SET(reg, AL_HAL_ETH_SERDES_GRP_MASK,
			 AL_HAL_ETH_SERDES_GRP_SHIFT, params->serdes_grp);

	AL_REG_BIT_VAL_SET(reg, AL_HAL_ETH_SERDES_GRP_2_SHIFT,
			(params->serdes_grp & AL_BIT(2)) ? 1 : 0);

	AL_REG_FIELD_SET(reg, AL_HAL_ETH_SERDES_LANE_LO_MASK,
			 AL_HAL_ETH_SERDES_LANE_LO_SHIFT,
			 AL_HAL_ETH_SERDES_LANE_LO(params->serdes_lane));

	AL_REG_FIELD_SET(reg, AL_HAL_ETH_REF_CLK_FREQ_MASK,
			 AL_HAL_ETH_REF_CLK_FREQ_SHIFT, params->ref_clk_freq);

	al_assert(reg != 0);

	al_reg_write32(scratch_regs[0], reg);

	/* ************* Setting Board params register 2 **************** */
	reg = 0;
	AL_REG_BIT_VAL_SET(reg, AL_HAL_ETH_DONT_OVERRIDE_SERDES_SHIFT,
			   params->dont_override_serdes == AL_TRUE);

	AL_REG_BIT_VAL_SET(reg, AL_HAL_ETH_1000_BASE_X_SHIFT,
			   params->force_1000_base_x == AL_TRUE);

	AL_REG_BIT_VAL_SET(reg, AL_HAL_ETH_1G_AN_DISABLE_SHIFT,
			   params->an_disable == AL_TRUE);

	AL_REG_FIELD_SET(reg, AL_HAL_ETH_1G_SPEED_MASK,
			 AL_HAL_ETH_1G_SPEED_SHIFT, params->speed);

	AL_REG_BIT_VAL_SET(reg, AL_HAL_ETH_1G_HALF_DUPLEX_SHIFT,
			   params->half_duplex == AL_TRUE);

	AL_REG_BIT_VAL_SET(reg, AL_HAL_ETH_1G_FC_DISABLE_SHIFT,
			   params->fc_disable == AL_TRUE);

	AL_REG_BIT_VAL_SET(reg, AL_HAL_ETH_RETIMER_EXIST_SHIFT, params->retimer_exist == AL_TRUE);
	AL_REG_FIELD_SET(reg, AL_HAL_ETH_RETIMER_BUS_ID_MASK,
			 AL_HAL_ETH_RETIMER_BUS_ID_SHIFT, params->retimer_bus_id);
	AL_REG_FIELD_SET(reg, AL_HAL_ETH_RETIMER_I2C_ADDR_MASK,
			 AL_HAL_ETH_RETIMER_I2C_ADDR_SHIFT, params->retimer_i2c_addr);

	AL_REG_BIT_VAL_SET(reg, AL_HAL_ETH_RETIMER_CHANNEL_SHIFT,
				(params->retimer_channel & AL_BIT(0)));

	AL_REG_FIELD_SET(reg, AL_HAL_ETH_RETIMER_CHANNEL_2_MASK,
			 AL_HAL_ETH_RETIMER_CHANNEL_2_SHIFT,
			 (AL_REG_FIELD_GET(params->retimer_channel, 0x6, 1)));

	AL_REG_FIELD_SET(reg, AL_HAL_ETH_DAC_LENGTH_MASK,
			 AL_HAL_ETH_DAC_LENGTH_SHIFT, params->dac_len);
	AL_REG_BIT_VAL_SET(reg, AL_HAL_ETH_DAC_SHIFT, params->dac);

	AL_REG_FIELD_SET(reg, AL_HAL_ETH_RETIMER_TYPE_MASK,
			 AL_HAL_ETH_RETIMER_TYPE_SHIFT, params->retimer_type);

	AL_REG_FIELD_SET(reg, AL_HAL_ETH_RETIMER_TX_CHANNEL_MASK,
			 AL_HAL_ETH_RETIMER_TX_CHANNEL_SHIFT,
			 params->retimer_tx_channel);

	al_reg_write32(scratch_regs[1], reg);

	/* ************* Setting Board params register 3 **************** */
	reg = 0;

	AL_REG_FIELD_SET(reg, AL_HAL_ETH_GPIO_SFP_PRESENT_MASK,
			 AL_HAL_ETH_GPIO_SFP_PRESENT_SHIFT,
			 params->gpio_sfp_present);

	AL_REG_BIT_VAL_SET(reg, AL_HAL_ETH_AUTO_FEC_ENABLE_SHIFT, params->auto_fec_enable);

	AL_REG_FIELD_SET(reg, AL_HAL_ETH_GPIO_SPD_1G_10G_MASK,
			 AL_HAL_ETH_GPIO_SPD_1G_10G_SHIFT,
			 params->gpio_spd_1g_10g);

	AL_REG_FIELD_SET(reg, AL_HAL_ETH_GPIO_SPD_25G_MASK,
			 AL_HAL_ETH_GPIO_SPD_25G_SHIFT,
			 params->gpio_spd_25g);
	AL_REG_FIELD_SET(reg, AL_HAL_ETH_COMMON_MODE_MASK,
			AL_HAL_ETH_COMMON_MODE_SHIFT,
			params->common_mode);

	AL_REG_FIELD_SET(reg, AL_HAL_ETH_SERDES_LANE_HI_MASK,
			 AL_HAL_ETH_SERDES_LANE_HI_SHIFT,
			 AL_HAL_ETH_SERDES_LANE_HI(params->serdes_lane));

	al_reg_write32(scratch_regs[2], reg);

	return;
}

/** Applicible for ETH V1 - V3 only */
int al_eth_board_params_set(void * __iomem mac_base, struct al_eth_board_params *params)
{
	uint32_t *scratch_regs[3];
	int rc;

	rc = al_eth_mac_scratch_regs_get(mac_base, AL_ETH_REV_ID_3, scratch_regs);
	if (rc != 0)
		return rc;

	al_eth_board_params_set_scratch_regs(scratch_regs, params);

	return 0;
}

int al_eth_board_params_set_ex(struct al_hal_eth_adapter *adapter,
	struct al_eth_board_params *params)
{
	uint32_t *scratch_regs[3];
	void __iomem *regs_base;
	int rc;

	if (adapter->rev_id <= AL_ETH_REV_ID_3)
		regs_base = adapter->mac_regs_base;
	else
		regs_base = adapter->ec_regs_base;

	rc = al_eth_mac_scratch_regs_get(regs_base, adapter->rev_id, scratch_regs);
	if (rc != 0)
		return rc;

	al_eth_board_params_set_scratch_regs(scratch_regs, params);

	return 0;
}

static int al_eth_board_params_get_scratch_regs(uint32_t **scratch_regs,
	struct al_eth_board_params *params)
{
	uint32_t	reg = al_reg_read32(scratch_regs[0]);
	uint32_t serdes_lane_hi;
	uint32_t serdes_lane_lo;

	/* check if the register was initialized, 0 is not a valid value */
	if (reg == 0)
		return -ENOENT;

	/* ************* Getting Board params register 1 **************** */
	params->media_type = AL_REG_FIELD_GET(reg, AL_HAL_ETH_MEDIA_TYPE_MASK,
					      AL_HAL_ETH_MEDIA_TYPE_SHIFT);
	if (AL_REG_BIT_GET(reg, AL_HAL_ETH_EXT_PHY_SHIFT))
		params->phy_exist = AL_TRUE;
	else
		params->phy_exist = AL_FALSE;

	params->phy_mdio_addr = AL_REG_FIELD_GET(reg, AL_HAL_ETH_PHY_ADDR_MASK,
						 AL_HAL_ETH_PHY_ADDR_SHIFT);

	if (AL_REG_BIT_GET(reg, AL_HAL_ETH_SFP_EXIST_SHIFT))
		params->sfp_plus_module_exist = AL_TRUE;
	else
		params->sfp_plus_module_exist = AL_FALSE;

	if (AL_REG_BIT_GET(reg, AL_HAL_ETH_AN_ENABLE_SHIFT))
		params->autoneg_enable = AL_TRUE;
	else
		params->autoneg_enable = AL_FALSE;

	if (AL_REG_BIT_GET(reg, AL_HAL_ETH_KR_LT_ENABLE_SHIFT))
		params->kr_lt_enable = AL_TRUE;
	else
		params->kr_lt_enable = AL_FALSE;

	if (AL_REG_BIT_GET(reg, AL_HAL_ETH_KR_FEC_ENABLE_SHIFT))
		params->kr_fec_enable = AL_TRUE;
	else
		params->kr_fec_enable = AL_FALSE;

	params->mdio_freq = AL_REG_FIELD_GET(reg,
					     AL_HAL_ETH_MDIO_FREQ_MASK,
					     AL_HAL_ETH_MDIO_FREQ_SHIFT);

	params->i2c_adapter_id = AL_REG_FIELD_GET(reg,
						  AL_HAL_ETH_I2C_ADAPTER_ID_MASK,
						  AL_HAL_ETH_I2C_ADAPTER_ID_SHIFT);

	params->phy_if = AL_REG_FIELD_GET(reg,
					  AL_HAL_ETH_EXT_PHY_IF_MASK,
					  AL_HAL_ETH_EXT_PHY_IF_SHIFT);

	if (AL_REG_BIT_GET(reg, AL_HAL_ETH_AUTO_NEG_MODE_SHIFT))
		params->an_mode = AL_TRUE;
	else
		params->an_mode = AL_FALSE;

	params->serdes_grp = AL_REG_FIELD_GET(reg,
					      AL_HAL_ETH_SERDES_GRP_MASK,
					      AL_HAL_ETH_SERDES_GRP_SHIFT);

	params->serdes_grp |= (AL_REG_BIT_GET(reg, AL_HAL_ETH_SERDES_GRP_2_SHIFT) ? AL_BIT(2) : 0);

	serdes_lane_lo = AL_REG_FIELD_GET(reg,
					  AL_HAL_ETH_SERDES_LANE_LO_MASK,
					  AL_HAL_ETH_SERDES_LANE_LO_SHIFT);

	params->ref_clk_freq = AL_REG_FIELD_GET(reg,
						AL_HAL_ETH_REF_CLK_FREQ_MASK,
						AL_HAL_ETH_REF_CLK_FREQ_SHIFT);

	/* ************* Getting Board params register 2 **************** */
	reg = al_reg_read32(scratch_regs[1]);
	if (AL_REG_BIT_GET(reg, AL_HAL_ETH_DONT_OVERRIDE_SERDES_SHIFT))
		params->dont_override_serdes = AL_TRUE;
	else
		params->dont_override_serdes = AL_FALSE;

	if (AL_REG_BIT_GET(reg, AL_HAL_ETH_1000_BASE_X_SHIFT))
		params->force_1000_base_x = AL_TRUE;
	else
		params->force_1000_base_x = AL_FALSE;

	if (AL_REG_BIT_GET(reg, AL_HAL_ETH_1G_AN_DISABLE_SHIFT))
		params->an_disable = AL_TRUE;
	else
		params->an_disable = AL_FALSE;

	params->speed = AL_REG_FIELD_GET(reg,
					 AL_HAL_ETH_1G_SPEED_MASK,
					 AL_HAL_ETH_1G_SPEED_SHIFT);

	if (AL_REG_BIT_GET(reg, AL_HAL_ETH_1G_HALF_DUPLEX_SHIFT))
		params->half_duplex = AL_TRUE;
	else
		params->half_duplex = AL_FALSE;

	if (AL_REG_BIT_GET(reg, AL_HAL_ETH_1G_FC_DISABLE_SHIFT))
		params->fc_disable = AL_TRUE;
	else
		params->fc_disable = AL_FALSE;

	if (AL_REG_BIT_GET(reg, AL_HAL_ETH_RETIMER_EXIST_SHIFT))
		params->retimer_exist = AL_TRUE;
	else
		params->retimer_exist = AL_FALSE;

	params->retimer_bus_id = AL_REG_FIELD_GET(reg,
					       AL_HAL_ETH_RETIMER_BUS_ID_MASK,
					       AL_HAL_ETH_RETIMER_BUS_ID_SHIFT);
	params->retimer_i2c_addr = AL_REG_FIELD_GET(reg,
					       AL_HAL_ETH_RETIMER_I2C_ADDR_MASK,
					       AL_HAL_ETH_RETIMER_I2C_ADDR_SHIFT);

	params->retimer_channel =
		((AL_REG_BIT_GET(reg, AL_HAL_ETH_RETIMER_CHANNEL_SHIFT)) |
		 (AL_REG_FIELD_GET(reg, AL_HAL_ETH_RETIMER_CHANNEL_2_MASK,
				   AL_HAL_ETH_RETIMER_CHANNEL_2_SHIFT) << 1));

	params->dac_len = AL_REG_FIELD_GET(reg,
					   AL_HAL_ETH_DAC_LENGTH_MASK,
					   AL_HAL_ETH_DAC_LENGTH_SHIFT);

	if (AL_REG_BIT_GET(reg, AL_HAL_ETH_DAC_SHIFT))
		params->dac = AL_TRUE;
	else
		params->dac = AL_FALSE;

	params->retimer_type = AL_REG_FIELD_GET(reg,
					   AL_HAL_ETH_RETIMER_TYPE_MASK,
					   AL_HAL_ETH_RETIMER_TYPE_SHIFT);

	params->retimer_tx_channel = AL_REG_FIELD_GET(reg,
					   AL_HAL_ETH_RETIMER_TX_CHANNEL_MASK,
					   AL_HAL_ETH_RETIMER_TX_CHANNEL_SHIFT);

	/* ************* Getting Board params register 3 **************** */
	reg = al_reg_read32(scratch_regs[2]);

	params->gpio_sfp_present = AL_REG_FIELD_GET(reg,
					AL_HAL_ETH_GPIO_SFP_PRESENT_MASK,
					AL_HAL_ETH_GPIO_SFP_PRESENT_SHIFT);

	if (AL_REG_BIT_GET(reg, AL_HAL_ETH_AUTO_FEC_ENABLE_SHIFT))
		params->auto_fec_enable = AL_TRUE;
	else
		params->auto_fec_enable = AL_FALSE;

	params->gpio_spd_1g_10g = AL_REG_FIELD_GET(reg,
					AL_HAL_ETH_GPIO_SPD_1G_10G_MASK,
					AL_HAL_ETH_GPIO_SPD_1G_10G_SHIFT);

	params->gpio_spd_25g = AL_REG_FIELD_GET(reg,
					AL_HAL_ETH_GPIO_SPD_25G_MASK,
					AL_HAL_ETH_GPIO_SPD_25G_SHIFT);
	params->common_mode = AL_REG_FIELD_GET(reg,
					AL_HAL_ETH_COMMON_MODE_MASK,
					AL_HAL_ETH_COMMON_MODE_SHIFT);

	serdes_lane_hi = AL_REG_FIELD_GET(reg,
					  AL_HAL_ETH_SERDES_LANE_HI_MASK,
					  AL_HAL_ETH_SERDES_LANE_HI_SHIFT);

	params->serdes_lane = AL_HAL_ETH_SERDES_LANE(serdes_lane_hi,
						     serdes_lane_lo);

	return 0;
}

/** Applicable to V1 - V3 only */
int al_eth_board_params_get(void * __iomem mac_base, struct al_eth_board_params *params)
{
	uint32_t *scratch_regs[3];
	int rc;

	rc = al_eth_mac_scratch_regs_get(mac_base, AL_ETH_REV_ID_3, scratch_regs);
	if (rc != 0)
		return rc;

	rc = al_eth_board_params_get_scratch_regs(scratch_regs, params);

	return rc;
}

int al_eth_board_params_get_ex(struct al_hal_eth_adapter *adapter,
	struct al_eth_board_params *params)
{
	uint32_t *scratch_regs[3];
	void __iomem *regs_base;
	int rc;

	if (adapter->rev_id <= AL_ETH_REV_ID_3)
		regs_base = adapter->mac_regs_base;
	else
		regs_base = adapter->ec_regs_base;

	rc = al_eth_mac_scratch_regs_get(regs_base, adapter->rev_id, scratch_regs);
	if (rc != 0)
		return rc;

	rc = al_eth_board_params_get_scratch_regs(scratch_regs, params);

	return rc;
}

/* Wake-On-Lan (WoL) */
static inline void al_eth_byte_arr_to_reg(
		uint32_t *reg, uint8_t *arr, unsigned int num_bytes)
{
	uint32_t mask = 0xff;
	unsigned int i;

	al_assert(num_bytes <= 4);

	*reg = 0;

	for (i = 0 ; i < num_bytes ; i++) {
		AL_REG_FIELD_SET(*reg, mask, (sizeof(uint8_t) * i), arr[i]);
		mask = mask << sizeof(uint8_t);
	}
}

int al_eth_wol_enable(
		struct al_hal_eth_adapter *adapter,
		struct al_eth_wol_params *wol)
{
	uint32_t reg = 0;

	if (wol->int_mask & AL_ETH_WOL_INT_MAGIC_PSWD) {
		al_assert(wol->pswd != NULL);

		al_eth_byte_arr_to_reg(&reg, &wol->pswd[0], 4);
		al_reg_write32(&adapter->ec_regs_base->wol.magic_pswd_l, reg);

		al_eth_byte_arr_to_reg(&reg, &wol->pswd[4], 2);
		al_reg_write32(&adapter->ec_regs_base->wol.magic_pswd_h, reg);
	}

	if (wol->int_mask & AL_ETH_WOL_INT_IPV4) {
		al_assert(wol->ipv4 != NULL);

		al_eth_byte_arr_to_reg(&reg, &wol->ipv4[0], 4);
		al_reg_write32(&adapter->ec_regs_base->wol.ipv4_dip, reg);
	}

	if (wol->int_mask & AL_ETH_WOL_INT_IPV6) {
		al_assert(wol->ipv6 != NULL);

		al_eth_byte_arr_to_reg(&reg, &wol->ipv6[0], 4);
		al_reg_write32(&adapter->ec_regs_base->wol.ipv6_dip_word0, reg);

		al_eth_byte_arr_to_reg(&reg, &wol->ipv6[4], 4);
		al_reg_write32(&adapter->ec_regs_base->wol.ipv6_dip_word1, reg);

		al_eth_byte_arr_to_reg(&reg, &wol->ipv6[8], 4);
		al_reg_write32(&adapter->ec_regs_base->wol.ipv6_dip_word2, reg);

		al_eth_byte_arr_to_reg(&reg, &wol->ipv6[12], 4);
		al_reg_write32(&adapter->ec_regs_base->wol.ipv6_dip_word3, reg);
	}

	if (wol->int_mask &
		(AL_ETH_WOL_INT_ETHERTYPE_BC | AL_ETH_WOL_INT_ETHERTYPE_DA)) {

		reg = ((uint32_t)wol->ethr_type2 << 16);
		reg |= wol->ethr_type1;

		al_reg_write32(&adapter->ec_regs_base->wol.ethertype, reg);
	}

	/* make sure we dont forwarding packets without interrupt */
	al_assert((wol->forward_mask | wol->int_mask) == wol->int_mask);

	reg = ((uint32_t)wol->forward_mask << 16);
	reg |= wol->int_mask;
	al_reg_write32(&adapter->ec_regs_base->wol.wol_en, reg);

	return 0;
}

int al_eth_wol_disable(
		struct al_hal_eth_adapter *adapter)
{
	al_reg_write32(&adapter->ec_regs_base->wol.wol_en, 0);

	return 0;
}

int al_eth_tx_fwd_vid_table_set(struct al_hal_eth_adapter *adapter, uint32_t idx,
				uint8_t udma_mask, al_bool fwd_to_mac)
{
	uint32_t	val = 0;
	al_assert(idx < AL_ETH_FWD_VID_TABLE_NUM); /* valid VID index */
	AL_REG_FIELD_SET(val,  AL_ETH_TX_VLAN_TABLE_UDMA_MASK, 0, udma_mask);
	AL_REG_FIELD_SET(val,  AL_ETH_TX_VLAN_TABLE_FWD_TO_MAC, 4, fwd_to_mac);

	al_reg_write32(&adapter->ec_regs_base->tfw.tx_vid_table_addr, idx);
	al_reg_write32(&adapter->ec_regs_base->tfw.tx_vid_table_data, val);
	return 0;
}

int al_eth_tx_protocol_detect_table_entry_set(struct al_hal_eth_adapter *adapter, uint32_t idx,
		struct al_eth_tx_gpd_cam_entry *tx_gpd_entry)
{
	uint64_t gpd_data;
	uint64_t gpd_mask;

	gpd_data = ((uint64_t)tx_gpd_entry->l3_proto_idx & AL_ETH_TX_GPD_L3_PROTO_MASK) <<
		AL_ETH_TX_GPD_L3_PROTO_SHIFT;
	gpd_data |= ((uint64_t)tx_gpd_entry->l4_proto_idx & AL_ETH_TX_GPD_L4_PROTO_MASK) <<
		AL_ETH_TX_GPD_L4_PROTO_SHIFT;
	gpd_data |= ((uint64_t)tx_gpd_entry->tunnel_control & AL_ETH_TX_GPD_TUNNEL_CTRL_MASK) <<
		AL_ETH_TX_GPD_TUNNEL_CTRL_SHIFT;
	gpd_data |= ((uint64_t)tx_gpd_entry->source_vlan_count & AL_ETH_TX_GPD_SRC_VLAN_CNT_MASK) <<
		AL_ETH_TX_GPD_SRC_VLAN_CNT_SHIFT;
	gpd_mask  = ((uint64_t)tx_gpd_entry->l3_proto_idx_mask & AL_ETH_TX_GPD_L3_PROTO_MASK) <<
		AL_ETH_TX_GPD_L3_PROTO_SHIFT;
	gpd_mask |= ((uint64_t)tx_gpd_entry->l4_proto_idx_mask & AL_ETH_TX_GPD_L4_PROTO_MASK) <<
		AL_ETH_TX_GPD_L4_PROTO_SHIFT;
	gpd_mask |= ((uint64_t)tx_gpd_entry->tunnel_control_mask & AL_ETH_TX_GPD_TUNNEL_CTRL_MASK) <<
		AL_ETH_TX_GPD_TUNNEL_CTRL_SHIFT;
	gpd_mask |= ((uint64_t)tx_gpd_entry->source_vlan_count_mask & AL_ETH_TX_GPD_SRC_VLAN_CNT_MASK) <<
		AL_ETH_TX_GPD_SRC_VLAN_CNT_SHIFT;

	/* Tx Generic protocol detect Cam compare table */
	al_reg_write32(&adapter->ec_regs_base->tfw_v3.tx_gpd_cam_addr, idx);
	al_reg_write32(&adapter->ec_regs_base->tfw_v3.tx_gpd_cam_ctrl,
			(uint32_t)((tx_gpd_entry->tx_gpd_cam_ctrl) << AL_ETH_TX_GPD_CAM_CTRL_VALID_SHIFT));
	al_dbg("al_eth_tx_generic_crc_entry_set, line [%d], tx_gpd_cam_ctrl: %#x\n",
		idx, tx_gpd_entry->tx_gpd_cam_ctrl);
	al_reg_write32(&adapter->ec_regs_base->tfw_v3.tx_gpd_cam_mask_2,
			(uint32_t)(gpd_mask >> AL_ETH_TX_GPD_CAM_MASK_2_SHIFT));
	al_dbg("al_eth_tx_generic_crc_entry_set, line [%d], tx_gpd_cam_mask_2: %#x\n",
		idx, (uint32_t)(gpd_mask >> AL_ETH_TX_GPD_CAM_MASK_2_SHIFT));
	al_reg_write32(&adapter->ec_regs_base->tfw_v3.tx_gpd_cam_mask_1,
			(uint32_t)(gpd_mask));
	al_dbg("al_eth_tx_generic_crc_entry_set, line [%d], tx_gpd_cam_mask_1: %#x\n",
		idx, (uint32_t)(gpd_mask));
	al_reg_write32(&adapter->ec_regs_base->tfw_v3.tx_gpd_cam_data_2,
			(uint32_t)(gpd_data >> AL_ETH_TX_GPD_CAM_DATA_2_SHIFT));
	al_dbg("al_eth_tx_generic_crc_entry_set, line [%d], tx_gpd_cam_data_2: %#x\n",
		idx, (uint32_t)(gpd_data >> AL_ETH_TX_GPD_CAM_DATA_2_SHIFT));
	al_reg_write32(&adapter->ec_regs_base->tfw_v3.tx_gpd_cam_data_1,
			(uint32_t)(gpd_data));
	al_dbg("al_eth_tx_generic_crc_entry_set, line [%d], tx_gpd_cam_data_1: %#x\n",
		idx, (uint32_t)(gpd_data));
	return 0;
}

void al_eth_tx_protocol_detect_table_entry_get(struct al_hal_eth_adapter *adapter, uint32_t idx,
		struct al_eth_tx_gpd_cam_entry *tx_gpd_entry)
{
	uint32_t reg = 0;

	al_assert(adapter);
	al_assert(tx_gpd_entry);
	al_assert(idx < AL_ETH_TX_GPD_TABLE_SIZE);
	al_assert(adapter->rev_id == AL_ETH_REV_ID_3);

	/* Tx Generic protocol detect Cam compare table */
	al_reg_write32(&adapter->ec_regs_base->tfw_v3.tx_gpd_cam_addr, idx);
	al_local_data_memory_barrier();
	/* tx_gpd_cam_data_1 is the read trigger for the enture line */
	reg = al_reg_read32(&adapter->ec_regs_base->tfw_v3.tx_gpd_cam_data_1);
	al_dbg("%s, line [%d], tx_gpd_cam_data_1: %#x\n",
	       __func__, idx, reg);
	tx_gpd_entry->l3_proto_idx = (reg >> AL_ETH_TX_GPD_L3_PROTO_SHIFT) &
		AL_ETH_TX_GPD_L3_PROTO_MASK;
	tx_gpd_entry->l4_proto_idx = (reg >> AL_ETH_TX_GPD_L4_PROTO_SHIFT) &
		AL_ETH_TX_GPD_L4_PROTO_MASK;
	tx_gpd_entry->tunnel_control = (reg >> AL_ETH_TX_GPD_TUNNEL_CTRL_SHIFT) &
		AL_ETH_TX_GPD_TUNNEL_CTRL_MASK;
	tx_gpd_entry->source_vlan_count = (reg >> AL_ETH_TX_GPD_SRC_VLAN_CNT_SHIFT) &
		AL_ETH_TX_GPD_SRC_VLAN_CNT_MASK;

	reg = al_reg_read32(&adapter->ec_regs_base->tfw_v3.tx_gpd_cam_data_2);
	al_dbg("%s, line [%d], tx_gpd_cam_data_2: %#x\n",
	       __func__, idx, reg);

	reg = al_reg_read32(&adapter->ec_regs_base->tfw_v3.tx_gpd_cam_mask_1);
	al_dbg("%s, line [%d], tx_gpd_cam_mask_1: %#x\n",
	       __func__, idx, reg);
	tx_gpd_entry->l3_proto_idx_mask = (reg >> AL_ETH_TX_GPD_L3_PROTO_SHIFT) &
		AL_ETH_TX_GPD_L3_PROTO_MASK;
	tx_gpd_entry->l4_proto_idx_mask = (reg >> AL_ETH_TX_GPD_L4_PROTO_SHIFT) &
		AL_ETH_TX_GPD_L4_PROTO_MASK;
	tx_gpd_entry->tunnel_control_mask = (reg >> AL_ETH_TX_GPD_TUNNEL_CTRL_SHIFT) &
		AL_ETH_TX_GPD_TUNNEL_CTRL_MASK;
	tx_gpd_entry->source_vlan_count_mask = (reg >> AL_ETH_TX_GPD_SRC_VLAN_CNT_SHIFT) &
		AL_ETH_TX_GPD_SRC_VLAN_CNT_MASK;

	reg = al_reg_read32(&adapter->ec_regs_base->tfw_v3.tx_gpd_cam_mask_2);
	al_dbg("%s, line [%d], tx_gpd_cam_mask_2: %#x\n",
	       __func__, idx, reg);

	reg = al_reg_read32(&adapter->ec_regs_base->tfw_v3.tx_gpd_cam_ctrl);
	al_dbg("%s, line [%d], tx_gpd_cam_ctrl: %#x\n",
	       __func__, idx, reg);
	tx_gpd_entry->tx_gpd_cam_ctrl = (reg >> AL_ETH_TX_GPD_CAM_CTRL_VALID_SHIFT) & 0x1;
}

int al_eth_tx_generic_crc_table_entry_set(struct al_hal_eth_adapter *adapter,
		unsigned int engine_idx, uint32_t idx,
		struct al_eth_tx_gcp_table_entry *tx_gcp_entry)
{
	uint32_t gcp_table_gen;
	uint32_t tx_alu_opcode;
	uint32_t tx_alu_opsel;

	al_assert((adapter->rev_id > AL_ETH_REV_ID_2));
	al_assert((engine_idx < AL_ETH_CRC_ENGINE_V3_MAX) ||
		(engine_idx < AL_ETH_CRC_ENGINE_V4_MAX && adapter->rev_id >= AL_ETH_REV_ID_4));

	gcp_table_gen  = (tx_gcp_entry->poly_sel & AL_ETH_TX_GCP_POLY_SEL_MASK) <<
		AL_ETH_TX_GCP_POLY_SEL_SHIFT;
	gcp_table_gen |= (tx_gcp_entry->crc32_bit_comp & AL_ETH_TX_GCP_CRC32_BIT_COMP_MASK) <<
		AL_ETH_TX_GCP_CRC32_BIT_COMP_SHIFT;
	gcp_table_gen |= (tx_gcp_entry->crc32_bit_swap & AL_ETH_TX_GCP_CRC32_BIT_SWAP_MASK) <<
		AL_ETH_TX_GCP_CRC32_BIT_SWAP_SHIFT;
	gcp_table_gen |= (tx_gcp_entry->crc32_byte_swap & AL_ETH_TX_GCP_CRC32_BYTE_SWAP_MASK) <<
		AL_ETH_TX_GCP_CRC32_BYTE_SWAP_SHIFT;
	gcp_table_gen |= (tx_gcp_entry->data_bit_swap & AL_ETH_TX_GCP_DATA_BIT_SWAP_MASK) <<
		AL_ETH_TX_GCP_DATA_BIT_SWAP_SHIFT;
	gcp_table_gen |= (tx_gcp_entry->data_byte_swap & AL_ETH_TX_GCP_DATA_BYTE_SWAP_MASK) <<
		AL_ETH_TX_GCP_DATA_BYTE_SWAP_SHIFT;
	gcp_table_gen |= (tx_gcp_entry->trail_size & AL_ETH_TX_GCP_TRAIL_SIZE_MASK) <<
		AL_ETH_TX_GCP_TRAIL_SIZE_SHIFT;
	gcp_table_gen |= (tx_gcp_entry->head_size & AL_ETH_TX_GCP_HEAD_SIZE_MASK) <<
		AL_ETH_TX_GCP_HEAD_SIZE_SHIFT;
	gcp_table_gen |= (tx_gcp_entry->head_calc & AL_ETH_TX_GCP_HEAD_CALC_MASK) <<
		AL_ETH_TX_GCP_HEAD_CALC_SHIFT;
	gcp_table_gen |= (tx_gcp_entry->mask_polarity & AL_ETH_TX_GCP_MASK_POLARITY_MASK) <<
		AL_ETH_TX_GCP_MASK_POLARITY_SHIFT;
	al_dbg("al_eth_tx_generic_crc_entry_set, line [%d], gcp_table_gen: %#x\n",
		idx, gcp_table_gen);

	tx_alu_opcode  = (tx_gcp_entry->tx_alu_opcode_1 & AL_ETH_TX_GCP_OPCODE_1_MASK) <<
		AL_ETH_TX_GCP_OPCODE_1_SHIFT;
	tx_alu_opcode |= (tx_gcp_entry->tx_alu_opcode_2 & AL_ETH_TX_GCP_OPCODE_2_MASK) <<
		AL_ETH_TX_GCP_OPCODE_2_SHIFT;
	tx_alu_opcode |= (tx_gcp_entry->tx_alu_opcode_3 & AL_ETH_TX_GCP_OPCODE_3_MASK) <<
		AL_ETH_TX_GCP_OPCODE_3_SHIFT;
	tx_alu_opsel  = (tx_gcp_entry->tx_alu_opsel_1 & AL_ETH_TX_GCP_OPSEL_1_MASK) <<
		AL_ETH_TX_GCP_OPSEL_1_SHIFT;
	tx_alu_opsel |= (tx_gcp_entry->tx_alu_opsel_2 & AL_ETH_TX_GCP_OPSEL_2_MASK) <<
		AL_ETH_TX_GCP_OPSEL_2_SHIFT;
	tx_alu_opsel |= (tx_gcp_entry->tx_alu_opsel_3 & AL_ETH_TX_GCP_OPSEL_3_MASK) <<
		AL_ETH_TX_GCP_OPSEL_3_SHIFT;
	tx_alu_opsel |= (tx_gcp_entry->tx_alu_opsel_4 & AL_ETH_TX_GCP_OPSEL_4_MASK) <<
		AL_ETH_TX_GCP_OPSEL_4_SHIFT;

	/*  Tx Generic crc prameters table general */
	if (adapter->rev_id > AL_ETH_REV_ID_3) {
		struct al_ec_crc __iomem *crc_regs_base = &adapter->ec_regs_base->crc[engine_idx];

		al_reg_write32(&crc_regs_base->v4_tx_gcp_table_addr, idx);
		al_reg_write32(&crc_regs_base->v4_tx_gcp_table_gen, gcp_table_gen);
		al_reg_write32(&crc_regs_base->v4_tx_gcp_table_mask_1, tx_gcp_entry->gcp_mask[0]);
		al_reg_write32(&crc_regs_base->v4_tx_gcp_table_mask_2, tx_gcp_entry->gcp_mask[1]);
		al_reg_write32(&crc_regs_base->v4_tx_gcp_table_mask_3, tx_gcp_entry->gcp_mask[2]);
		al_reg_write32(&crc_regs_base->v4_tx_gcp_table_mask_4, tx_gcp_entry->gcp_mask[3]);
		al_reg_write32(&crc_regs_base->v4_tx_gcp_table_mask_5, tx_gcp_entry->gcp_mask[4]);
		al_reg_write32(&crc_regs_base->v4_tx_gcp_table_mask_6, tx_gcp_entry->gcp_mask[5]);
		al_reg_write32(&crc_regs_base->v4_tx_gcp_table_crc_init, tx_gcp_entry->crc_init);
		al_reg_write32(&crc_regs_base->v4_tx_gcp_table_res, tx_gcp_entry->gcp_table_res);
		al_reg_write32(&crc_regs_base->v4_tx_gcp_table_alu_opcode, tx_alu_opcode);
		al_reg_write32(&crc_regs_base->v4_tx_gcp_table_alu_opsel, tx_alu_opsel);
		al_reg_write32(&crc_regs_base->v4_tx_gcp_table_alu_val, tx_gcp_entry->alu_val);
		al_reg_write32(&crc_regs_base->v4_tx_gcp_table_ext_1, tx_gcp_entry->ext[0]);
		al_reg_write32(&crc_regs_base->v4_tx_gcp_table_ext_2, tx_gcp_entry->ext[1]);
		al_reg_write32(&crc_regs_base->v4_tx_gcp_table_ext_3, tx_gcp_entry->ext[2]);
		al_reg_write32(&crc_regs_base->v4_tx_gcp_table_ext_4, tx_gcp_entry->ext[3]);
		al_reg_write32(&crc_regs_base->v4_tx_gcp_table_ext_5, tx_gcp_entry->ext[4]);
		al_reg_write32(&crc_regs_base->v4_tx_gcp_table_ext_6, tx_gcp_entry->ext[5]);
	} else {
		al_reg_write32(&adapter->ec_regs_base->tfw_v3.tx_gcp_table_addr, idx);
		al_reg_write32(&adapter->ec_regs_base->tfw_v3.tx_gcp_table_gen,
				gcp_table_gen);
		al_reg_write32(&adapter->ec_regs_base->tfw_v3.tx_gcp_table_mask_1,
				tx_gcp_entry->gcp_mask[0]);
		al_reg_write32(&adapter->ec_regs_base->tfw_v3.tx_gcp_table_mask_2,
				tx_gcp_entry->gcp_mask[1]);
		al_reg_write32(&adapter->ec_regs_base->tfw_v3.tx_gcp_table_mask_3,
				tx_gcp_entry->gcp_mask[2]);
		al_reg_write32(&adapter->ec_regs_base->tfw_v3.tx_gcp_table_mask_4,
				tx_gcp_entry->gcp_mask[3]);
		al_reg_write32(&adapter->ec_regs_base->tfw_v3.tx_gcp_table_mask_5,
				tx_gcp_entry->gcp_mask[4]);
		al_reg_write32(&adapter->ec_regs_base->tfw_v3.tx_gcp_table_mask_6,
				tx_gcp_entry->gcp_mask[5]);
		al_reg_write32(&adapter->ec_regs_base->tfw_v3.tx_gcp_table_crc_init,
				tx_gcp_entry->crc_init);
		al_reg_write32(&adapter->ec_regs_base->tfw_v3.tx_gcp_table_res,
				tx_gcp_entry->gcp_table_res);
		al_reg_write32(&adapter->ec_regs_base->tfw_v3.tx_gcp_table_alu_opcode,
				tx_alu_opcode);
		al_reg_write32(&adapter->ec_regs_base->tfw_v3.tx_gcp_table_alu_opsel,
				tx_alu_opsel);
		al_reg_write32(&adapter->ec_regs_base->tfw_v3.tx_gcp_table_alu_val,
				tx_gcp_entry->alu_val);
	};
	return 0;
}

void al_eth_tx_generic_crc_table_entry_get(struct al_hal_eth_adapter *adapter,
		unsigned int engine_idx, uint32_t idx,
		struct al_eth_tx_gcp_table_entry *tx_gcp_entry)
{
	uint32_t reg = 0;

	al_assert(adapter);
	al_assert(tx_gcp_entry);
	al_assert(idx < AL_ETH_TX_GCP_TABLE_SIZE);
	/** Supports only ETHv3 - engine IDX 0 */
	al_assert(adapter->rev_id == AL_ETH_REV_ID_3);
	al_assert(engine_idx == 0);

	/*  Tx Generic crc prameters table general */
	al_reg_write32(&adapter->ec_regs_base->tfw_v3.tx_gcp_table_addr, idx);
	al_local_data_memory_barrier();
	/* tx_gcp_table_gen is the read trigger for the enture line */
	reg = al_reg_read32(&adapter->ec_regs_base->tfw_v3.tx_gcp_table_gen);
	al_dbg("%s, line [%d], tx_gcp_table_gen: %#x\n",
	       __func__, idx, reg);
	tx_gcp_entry->poly_sel = (reg >> AL_ETH_TX_GCP_POLY_SEL_SHIFT) &
		AL_ETH_TX_GCP_POLY_SEL_MASK;
	tx_gcp_entry->crc32_bit_comp = (reg >> AL_ETH_TX_GCP_CRC32_BIT_COMP_SHIFT) &
		AL_ETH_TX_GCP_CRC32_BIT_COMP_MASK;
	tx_gcp_entry->crc32_bit_swap = (reg >> AL_ETH_TX_GCP_CRC32_BIT_SWAP_SHIFT) &
		AL_ETH_TX_GCP_CRC32_BIT_SWAP_MASK;
	tx_gcp_entry->crc32_byte_swap = (reg >> AL_ETH_TX_GCP_CRC32_BYTE_SWAP_SHIFT) &
		AL_ETH_TX_GCP_CRC32_BYTE_SWAP_MASK;
	tx_gcp_entry->data_bit_swap = (reg >> AL_ETH_TX_GCP_DATA_BIT_SWAP_SHIFT) &
		AL_ETH_TX_GCP_DATA_BIT_SWAP_MASK;
	tx_gcp_entry->data_byte_swap = (reg >> AL_ETH_TX_GCP_DATA_BYTE_SWAP_SHIFT) &
		AL_ETH_TX_GCP_DATA_BYTE_SWAP_MASK;
	tx_gcp_entry->trail_size = (reg >> AL_ETH_TX_GCP_TRAIL_SIZE_SHIFT) &
		AL_ETH_TX_GCP_TRAIL_SIZE_MASK;
	tx_gcp_entry->head_size = (reg >> AL_ETH_TX_GCP_HEAD_SIZE_SHIFT) &
		AL_ETH_TX_GCP_HEAD_SIZE_MASK;
	tx_gcp_entry->head_calc = (reg >> AL_ETH_TX_GCP_HEAD_CALC_SHIFT) &
		AL_ETH_TX_GCP_HEAD_CALC_MASK;
	tx_gcp_entry->mask_polarity = (reg >> AL_ETH_TX_GCP_MASK_POLARITY_SHIFT) &
		AL_ETH_TX_GCP_MASK_POLARITY_MASK;

	tx_gcp_entry->gcp_mask[0] =
		al_reg_read32(&adapter->ec_regs_base->tfw_v3.tx_gcp_table_mask_1);
	al_dbg("%s, line [%d], tx_gcp_table_mask_1: %#x\n",
	       __func__, idx, tx_gcp_entry->gcp_mask[0]);
	tx_gcp_entry->gcp_mask[1] =
		al_reg_read32(&adapter->ec_regs_base->tfw_v3.tx_gcp_table_mask_2);
	al_dbg("%s, line [%d], tx_gcp_table_mask_2: %#x\n",
	       __func__, idx, tx_gcp_entry->gcp_mask[1]);
	tx_gcp_entry->gcp_mask[2] =
		al_reg_read32(&adapter->ec_regs_base->tfw_v3.tx_gcp_table_mask_3);
	al_dbg("%s, line [%d], tx_gcp_table_mask_3: %#x\n",
	       __func__, idx, tx_gcp_entry->gcp_mask[2]);
	tx_gcp_entry->gcp_mask[3] =
		al_reg_read32(&adapter->ec_regs_base->tfw_v3.tx_gcp_table_mask_4);
	al_dbg("%s, line [%d], tx_gcp_table_mask_4: %#x\n",
	       __func__, idx, tx_gcp_entry->gcp_mask[3]);
	tx_gcp_entry->gcp_mask[4] =
		al_reg_read32(&adapter->ec_regs_base->tfw_v3.tx_gcp_table_mask_5);
	al_dbg("%s, line [%d], tx_gcp_table_mask_5: %#x\n",
	       __func__, idx, tx_gcp_entry->gcp_mask[4]);
	tx_gcp_entry->gcp_mask[5] =
		al_reg_read32(&adapter->ec_regs_base->tfw_v3.tx_gcp_table_mask_6);
	al_dbg("%s, line [%d], tx_gcp_table_mask_6: %#x\n",
	       __func__, idx, tx_gcp_entry->gcp_mask[5]);

	tx_gcp_entry->crc_init =
		al_reg_read32(&adapter->ec_regs_base->tfw_v3.tx_gcp_table_crc_init);
	al_dbg("%s, line [%d], tx_gcp_table_crc_init: %#x\n",
	       __func__, idx, tx_gcp_entry->crc_init);

	tx_gcp_entry->gcp_table_res =
		al_reg_read32(&adapter->ec_regs_base->tfw_v3.tx_gcp_table_res);
	al_dbg("%s, line [%d], tx_gcp_table_res: %#x\n",
	       __func__, idx, tx_gcp_entry->gcp_table_res);

	reg = al_reg_read32(&adapter->ec_regs_base->tfw_v3.tx_gcp_table_alu_opcode);
	al_dbg("%s, line [%d], tx_gcp_table_alu_opcode: %#x\n",
	       __func__, idx, reg);
	tx_gcp_entry->tx_alu_opcode_1 = (reg >> AL_ETH_TX_GCP_OPCODE_1_SHIFT) &
		AL_ETH_TX_GCP_OPCODE_1_MASK;
	tx_gcp_entry->tx_alu_opcode_2 = (reg >> AL_ETH_TX_GCP_OPCODE_2_SHIFT) &
		AL_ETH_TX_GCP_OPCODE_2_MASK;
	tx_gcp_entry->tx_alu_opcode_3 = (reg >> AL_ETH_TX_GCP_OPCODE_3_SHIFT) &
		AL_ETH_TX_GCP_OPCODE_3_MASK;

	reg = al_reg_read32(&adapter->ec_regs_base->tfw_v3.tx_gcp_table_alu_opsel);
	al_dbg("%s, line [%d], tx_gcp_table_alu_opsel: %#x\n",
	       __func__, idx, reg);
	tx_gcp_entry->tx_alu_opsel_1 = (reg >> AL_ETH_TX_GCP_OPSEL_1_SHIFT) &
		AL_ETH_TX_GCP_OPSEL_1_MASK;
	tx_gcp_entry->tx_alu_opsel_2 = (reg >> AL_ETH_TX_GCP_OPSEL_2_SHIFT) &
		AL_ETH_TX_GCP_OPSEL_2_MASK;
	tx_gcp_entry->tx_alu_opsel_3 = (reg >> AL_ETH_TX_GCP_OPSEL_3_SHIFT) &
		AL_ETH_TX_GCP_OPSEL_3_MASK;
	tx_gcp_entry->tx_alu_opsel_4 = (reg >> AL_ETH_TX_GCP_OPSEL_4_SHIFT) &
		AL_ETH_TX_GCP_OPSEL_4_MASK;

	tx_gcp_entry->alu_val = al_reg_read32(&adapter->ec_regs_base->tfw_v3.tx_gcp_table_alu_val);
	al_dbg("%s, line [%d], tx_gcp_table_alu_val: %#x\n",
	       __func__, idx, tx_gcp_entry->alu_val);
}

int al_eth_tx_crc_chksum_replace_cmd_entry_set(struct al_hal_eth_adapter *adapter, uint32_t idx,
		struct al_eth_tx_crc_chksum_replace_cmd_for_protocol_num_entry *tx_replace_entry)
{
	uint32_t replace_table_address;
	uint32_t tx_replace_cmd;

	al_assert(adapter->rev_id == AL_ETH_REV_ID_3);

	/*  Tx crc_chksum_replace_cmd */
	replace_table_address = (L4_CHECKSUM_DIS_AND_L3_CHECKSUM_DIS) | idx;
	tx_replace_cmd  = (uint32_t)(tx_replace_entry->l3_csum_en_00) << 0;
	tx_replace_cmd |= (uint32_t)(tx_replace_entry->l4_csum_en_00) << 1;
	tx_replace_cmd |= (uint32_t)(tx_replace_entry->crc_en_00)     << 2;
	al_reg_write32(&adapter->ec_regs_base->tfw_v3.crc_csum_replace_table_addr, replace_table_address);
	al_reg_write32(&adapter->ec_regs_base->tfw_v3.crc_csum_replace_table,
			tx_replace_cmd);
	replace_table_address = (L4_CHECKSUM_DIS_AND_L3_CHECKSUM_EN) | idx;
	tx_replace_cmd  = (uint32_t)(tx_replace_entry->l3_csum_en_01) << 0;
	tx_replace_cmd |= (uint32_t)(tx_replace_entry->l4_csum_en_01) << 1;
	tx_replace_cmd |= (uint32_t)(tx_replace_entry->crc_en_01)     << 2;
	al_reg_write32(&adapter->ec_regs_base->tfw_v3.crc_csum_replace_table_addr, replace_table_address);
	al_reg_write32(&adapter->ec_regs_base->tfw_v3.crc_csum_replace_table,
			tx_replace_cmd);
	replace_table_address = (L4_CHECKSUM_EN_AND_L3_CHECKSUM_DIS) | idx;
	tx_replace_cmd  = (uint32_t)(tx_replace_entry->l3_csum_en_10) << 0;
	tx_replace_cmd |= (uint32_t)(tx_replace_entry->l4_csum_en_10) << 1;
	tx_replace_cmd |= (uint32_t)(tx_replace_entry->crc_en_10)     << 2;
	al_reg_write32(&adapter->ec_regs_base->tfw_v3.crc_csum_replace_table_addr, replace_table_address);
	al_reg_write32(&adapter->ec_regs_base->tfw_v3.crc_csum_replace_table,
			tx_replace_cmd);
	replace_table_address = (L4_CHECKSUM_EN_AND_L3_CHECKSUM_EN) | idx;
	tx_replace_cmd  = (uint32_t)(tx_replace_entry->l3_csum_en_11) << 0;
	tx_replace_cmd |= (uint32_t)(tx_replace_entry->l4_csum_en_11) << 1;
	tx_replace_cmd |= (uint32_t)(tx_replace_entry->crc_en_11)     << 2;
	al_reg_write32(&adapter->ec_regs_base->tfw_v3.crc_csum_replace_table_addr, replace_table_address);
	al_reg_write32(&adapter->ec_regs_base->tfw_v3.crc_csum_replace_table,
			tx_replace_cmd);

	return 0;
}

void al_eth_tx_crc_chksum_replace_cmd_entry_get(struct al_hal_eth_adapter *adapter, uint32_t idx,
		struct al_eth_tx_crc_chksum_replace_cmd_for_protocol_num_entry *tx_replace_entry)
{
	uint32_t replace_table_address;
	uint32_t tx_replace_cmd;

	al_assert(adapter);
	al_assert(tx_replace_entry);
	al_assert(idx < AL_ETH_TX_CRC_CSUM_REPLACE_SW_TABLE_SIZE);
	al_assert(adapter->rev_id == AL_ETH_REV_ID_3);

	/*  Tx crc_chksum_replace_cmd */
	replace_table_address = L4_CHECKSUM_DIS_AND_L3_CHECKSUM_DIS | idx;
	al_reg_write32(&adapter->ec_regs_base->tfw_v3.crc_csum_replace_table_addr,
		replace_table_address);
	al_local_data_memory_barrier();
	tx_replace_cmd = al_reg_read32(&adapter->ec_regs_base->tfw_v3.crc_csum_replace_table);
	tx_replace_entry->l3_csum_en_00 =
		(tx_replace_cmd >> AL_ETH_TX_CRC_CHKSUM_REPLACE_CMD_L3_CSUM_EN_SHIFT) &
		AL_ETH_TX_CRC_CHKSUM_REPLACE_CMD_L3_CSUM_EN_MASK;
	tx_replace_entry->l4_csum_en_00 =
		(tx_replace_cmd >> AL_ETH_TX_CRC_CHKSUM_REPLACE_CMD_L4_CSUM_EN_SHIFT) &
		AL_ETH_TX_CRC_CHKSUM_REPLACE_CMD_L4_CSUM_EN_MASK;
	tx_replace_entry->crc_en_00 =
		(tx_replace_cmd >> AL_ETH_TX_CRC_CHKSUM_REPLACE_CMD_CRC_EN_SHIFT) &
		AL_ETH_TX_CRC_CHKSUM_REPLACE_CMD_CRC_EN_MASK;

	replace_table_address = L4_CHECKSUM_DIS_AND_L3_CHECKSUM_EN | idx;
	al_reg_write32(&adapter->ec_regs_base->tfw_v3.crc_csum_replace_table_addr,
		replace_table_address);
	al_local_data_memory_barrier();
	tx_replace_cmd = al_reg_read32(&adapter->ec_regs_base->tfw_v3.crc_csum_replace_table);
	tx_replace_entry->l3_csum_en_01 =
		(tx_replace_cmd >> AL_ETH_TX_CRC_CHKSUM_REPLACE_CMD_L3_CSUM_EN_SHIFT) &
		AL_ETH_TX_CRC_CHKSUM_REPLACE_CMD_L3_CSUM_EN_MASK;
	tx_replace_entry->l4_csum_en_01 =
		(tx_replace_cmd >> AL_ETH_TX_CRC_CHKSUM_REPLACE_CMD_L4_CSUM_EN_SHIFT) &
		AL_ETH_TX_CRC_CHKSUM_REPLACE_CMD_L4_CSUM_EN_MASK;
	tx_replace_entry->crc_en_01 =
		(tx_replace_cmd >> AL_ETH_TX_CRC_CHKSUM_REPLACE_CMD_CRC_EN_SHIFT) &
		AL_ETH_TX_CRC_CHKSUM_REPLACE_CMD_CRC_EN_MASK;

	replace_table_address = L4_CHECKSUM_EN_AND_L3_CHECKSUM_DIS | idx;
	al_reg_write32(&adapter->ec_regs_base->tfw_v3.crc_csum_replace_table_addr,
		replace_table_address);
	al_local_data_memory_barrier();
	tx_replace_cmd = al_reg_read32(&adapter->ec_regs_base->tfw_v3.crc_csum_replace_table);
	tx_replace_entry->l3_csum_en_10 =
		(tx_replace_cmd >> AL_ETH_TX_CRC_CHKSUM_REPLACE_CMD_L3_CSUM_EN_SHIFT) &
		AL_ETH_TX_CRC_CHKSUM_REPLACE_CMD_L3_CSUM_EN_MASK;
	tx_replace_entry->l4_csum_en_10 =
		(tx_replace_cmd >> AL_ETH_TX_CRC_CHKSUM_REPLACE_CMD_L4_CSUM_EN_SHIFT) &
		AL_ETH_TX_CRC_CHKSUM_REPLACE_CMD_L4_CSUM_EN_MASK;
	tx_replace_entry->crc_en_10 =
		(tx_replace_cmd >> AL_ETH_TX_CRC_CHKSUM_REPLACE_CMD_CRC_EN_SHIFT) &
		AL_ETH_TX_CRC_CHKSUM_REPLACE_CMD_CRC_EN_MASK;

	replace_table_address = L4_CHECKSUM_EN_AND_L3_CHECKSUM_EN | idx;
	al_reg_write32(&adapter->ec_regs_base->tfw_v3.crc_csum_replace_table_addr,
		replace_table_address);
	al_local_data_memory_barrier();
	tx_replace_cmd = al_reg_read32(&adapter->ec_regs_base->tfw_v3.crc_csum_replace_table);
	tx_replace_entry->l3_csum_en_11 =
		(tx_replace_cmd >> AL_ETH_TX_CRC_CHKSUM_REPLACE_CMD_L3_CSUM_EN_SHIFT) &
		AL_ETH_TX_CRC_CHKSUM_REPLACE_CMD_L3_CSUM_EN_MASK;
	tx_replace_entry->l4_csum_en_11 =
		(tx_replace_cmd >> AL_ETH_TX_CRC_CHKSUM_REPLACE_CMD_L4_CSUM_EN_SHIFT) &
		AL_ETH_TX_CRC_CHKSUM_REPLACE_CMD_L4_CSUM_EN_MASK;
	tx_replace_entry->crc_en_11 =
		(tx_replace_cmd >> AL_ETH_TX_CRC_CHKSUM_REPLACE_CMD_CRC_EN_SHIFT) &
		AL_ETH_TX_CRC_CHKSUM_REPLACE_CMD_CRC_EN_MASK;
}

int al_eth_rx_protocol_detect_table_entry_set(struct al_hal_eth_adapter *adapter, uint32_t idx,
		struct al_eth_rx_gpd_cam_entry *rx_gpd_entry)
{
	uint64_t gpd_data;
	uint64_t gpd_mask;

	gpd_data  = ((uint64_t)rx_gpd_entry->outer_l3_proto_idx & AL_ETH_RX_GPD_OUTER_L3_PROTO_MASK) <<
		AL_ETH_RX_GPD_OUTER_L3_PROTO_SHIFT;
	gpd_data |= ((uint64_t)rx_gpd_entry->outer_l4_proto_idx & AL_ETH_RX_GPD_OUTER_L4_PROTO_MASK) <<
		AL_ETH_RX_GPD_OUTER_L4_PROTO_SHIFT;
	gpd_data |= ((uint64_t)rx_gpd_entry->inner_l3_proto_idx & AL_ETH_RX_GPD_INNER_L3_PROTO_MASK) <<
		AL_ETH_RX_GPD_INNER_L3_PROTO_SHIFT;
	gpd_data |= ((uint64_t)rx_gpd_entry->inner_l4_proto_idx & AL_ETH_RX_GPD_INNER_L4_PROTO_MASK) <<
		AL_ETH_RX_GPD_INNER_L4_PROTO_SHIFT;
	gpd_data |= ((uint64_t)rx_gpd_entry->parse_ctrl & AL_ETH_RX_GPD_OUTER_PARSE_CTRL_MASK) <<
		AL_ETH_RX_GPD_OUTER_PARSE_CTRL_SHIFT;
	gpd_data |= ((uint64_t)rx_gpd_entry->outer_l3_len & AL_ETH_RX_GPD_INNER_PARSE_CTRL_MASK) <<
		AL_ETH_RX_GPD_INNER_PARSE_CTRL_SHIFT;
	gpd_data |= ((uint64_t)rx_gpd_entry->l3_priority & AL_ETH_RX_GPD_L3_PRIORITY_MASK) <<
		AL_ETH_RX_GPD_L3_PRIORITY_SHIFT;
	gpd_data |= ((uint64_t)rx_gpd_entry->l4_dst_port_lsb & AL_ETH_RX_GPD_L4_DST_PORT_LSB_MASK) <<
		AL_ETH_RX_GPD_L4_DST_PORT_LSB_SHIFT;

	gpd_mask  = ((uint64_t)rx_gpd_entry->outer_l3_proto_idx_mask & AL_ETH_RX_GPD_OUTER_L3_PROTO_MASK) <<
		AL_ETH_RX_GPD_OUTER_L3_PROTO_SHIFT;
	gpd_mask |= ((uint64_t)rx_gpd_entry->outer_l4_proto_idx_mask & AL_ETH_RX_GPD_OUTER_L4_PROTO_MASK) <<
		AL_ETH_RX_GPD_OUTER_L4_PROTO_SHIFT;
	gpd_mask |= ((uint64_t)rx_gpd_entry->inner_l3_proto_idx_mask & AL_ETH_RX_GPD_INNER_L3_PROTO_MASK) <<
		AL_ETH_RX_GPD_INNER_L3_PROTO_SHIFT;
	gpd_mask |= ((uint64_t)rx_gpd_entry->inner_l4_proto_idx_mask & AL_ETH_RX_GPD_INNER_L4_PROTO_MASK) <<
		AL_ETH_RX_GPD_INNER_L4_PROTO_SHIFT;
	gpd_mask |= ((uint64_t)rx_gpd_entry->parse_ctrl_mask & AL_ETH_RX_GPD_OUTER_PARSE_CTRL_MASK) <<
		AL_ETH_RX_GPD_OUTER_PARSE_CTRL_SHIFT;
	gpd_mask |= ((uint64_t)rx_gpd_entry->outer_l3_len_mask & AL_ETH_RX_GPD_INNER_PARSE_CTRL_MASK) <<
		AL_ETH_RX_GPD_INNER_PARSE_CTRL_SHIFT;
	gpd_mask |= ((uint64_t)rx_gpd_entry->l3_priority_mask & AL_ETH_RX_GPD_L3_PRIORITY_MASK) <<
		AL_ETH_RX_GPD_L3_PRIORITY_SHIFT;
	gpd_mask |= ((uint64_t)rx_gpd_entry->l4_dst_port_lsb_mask & AL_ETH_RX_GPD_L4_DST_PORT_LSB_MASK) <<
		AL_ETH_RX_GPD_L4_DST_PORT_LSB_SHIFT;

	/* Rx Generic protocol detect Cam compare table */
	al_reg_write32(&adapter->ec_regs_base->rfw_v3.rx_gpd_cam_addr, idx);
	al_reg_write32(&adapter->ec_regs_base->rfw_v3.rx_gpd_cam_ctrl,
			(uint32_t)((rx_gpd_entry->rx_gpd_cam_ctrl) << AL_ETH_RX_GPD_CAM_CTRL_VALID_SHIFT));
	al_reg_write32(&adapter->ec_regs_base->rfw_v3.rx_gpd_cam_mask_2,
			(uint32_t)(gpd_mask >> AL_ETH_RX_GPD_CAM_MASK_2_SHIFT));
	al_reg_write32(&adapter->ec_regs_base->rfw_v3.rx_gpd_cam_mask_1,
			(uint32_t)(gpd_mask));
	al_reg_write32(&adapter->ec_regs_base->rfw_v3.rx_gpd_cam_data_2,
			(uint32_t)(gpd_data >> AL_ETH_RX_GPD_CAM_DATA_2_SHIFT));
	al_reg_write32(&adapter->ec_regs_base->rfw_v3.rx_gpd_cam_data_1,
			(uint32_t)(gpd_data));
	return 0;
}

void al_eth_rx_protocol_detect_table_entry_get(struct al_hal_eth_adapter *adapter, uint32_t idx,
		struct al_eth_rx_gpd_cam_entry *rx_gpd_entry)
{
	uint32_t gpd_data_1;
	uint32_t gpd_data_2;
	uint32_t gpd_mask_1;
	uint32_t gpd_mask_2;
	uint32_t gpd_ctrl;
	uint64_t gpd_data;
	uint64_t gpd_mask;

	al_assert(adapter);
	al_assert(rx_gpd_entry);
	al_assert(idx < AL_ETH_RX_GPD_TABLE_SIZE);
	al_assert(adapter->rev_id == AL_ETH_REV_ID_3);

	/* Rx Generic protocol detect Cam compare table */
	al_reg_write32(&adapter->ec_regs_base->rfw_v3.rx_gpd_cam_addr, idx);
	al_local_data_memory_barrier();
	/* rx_gpd_cam_data_1 is the read/write trigger for the entire entry */
	gpd_data_1 = al_reg_read32(&adapter->ec_regs_base->rfw_v3.rx_gpd_cam_data_1);
	gpd_data_2 = al_reg_read32(&adapter->ec_regs_base->rfw_v3.rx_gpd_cam_data_2);
	gpd_mask_1 = al_reg_read32(&adapter->ec_regs_base->rfw_v3.rx_gpd_cam_mask_1);
	gpd_mask_2 = al_reg_read32(&adapter->ec_regs_base->rfw_v3.rx_gpd_cam_mask_2);
	gpd_ctrl = al_reg_read32(&adapter->ec_regs_base->rfw_v3.rx_gpd_cam_ctrl);

	gpd_data = ((uint64_t)gpd_data_2 << AL_ETH_RX_GPD_CAM_DATA_2_SHIFT) | gpd_data_1;

	/* data_1 fields */
	rx_gpd_entry->outer_l3_proto_idx = (gpd_data >> AL_ETH_RX_GPD_OUTER_L3_PROTO_SHIFT) &
		AL_ETH_RX_GPD_OUTER_L3_PROTO_MASK;
	rx_gpd_entry->outer_l4_proto_idx = (gpd_data >> AL_ETH_RX_GPD_OUTER_L4_PROTO_SHIFT) &
		AL_ETH_RX_GPD_OUTER_L4_PROTO_MASK;
	rx_gpd_entry->inner_l3_proto_idx = (gpd_data >> AL_ETH_RX_GPD_INNER_L3_PROTO_SHIFT) &
		AL_ETH_RX_GPD_INNER_L3_PROTO_MASK;
	rx_gpd_entry->inner_l4_proto_idx = (gpd_data >> AL_ETH_RX_GPD_INNER_L4_PROTO_SHIFT) &
		AL_ETH_RX_GPD_INNER_L4_PROTO_MASK;

	/* data_2 fields */
	rx_gpd_entry->parse_ctrl = (gpd_data >> AL_ETH_RX_GPD_OUTER_PARSE_CTRL_SHIFT) &
		AL_ETH_RX_GPD_OUTER_PARSE_CTRL_MASK;
	rx_gpd_entry->outer_l3_len = (gpd_data >> AL_ETH_RX_GPD_INNER_PARSE_CTRL_SHIFT) &
		AL_ETH_RX_GPD_INNER_PARSE_CTRL_MASK;
	rx_gpd_entry->l3_priority = (gpd_data >> AL_ETH_RX_GPD_L3_PRIORITY_SHIFT) &
		AL_ETH_RX_GPD_L3_PRIORITY_MASK;
	rx_gpd_entry->l4_dst_port_lsb = (gpd_data >> AL_ETH_RX_GPD_L4_DST_PORT_LSB_SHIFT) &
		AL_ETH_RX_GPD_L4_DST_PORT_LSB_MASK;

	gpd_mask = ((uint64_t)gpd_mask_2 << AL_ETH_RX_GPD_CAM_MASK_2_SHIFT) | gpd_mask_1;

	/* mask_1 fields */
	rx_gpd_entry->outer_l3_proto_idx_mask = (gpd_mask >> AL_ETH_RX_GPD_OUTER_L3_PROTO_SHIFT) &
		AL_ETH_RX_GPD_OUTER_L3_PROTO_MASK;
	rx_gpd_entry->outer_l4_proto_idx_mask = (gpd_mask >> AL_ETH_RX_GPD_OUTER_L4_PROTO_SHIFT) &
		AL_ETH_RX_GPD_OUTER_L4_PROTO_MASK;
	rx_gpd_entry->inner_l3_proto_idx_mask = (gpd_mask >> AL_ETH_RX_GPD_INNER_L3_PROTO_SHIFT) &
		AL_ETH_RX_GPD_INNER_L3_PROTO_MASK;
	rx_gpd_entry->inner_l4_proto_idx_mask = (gpd_mask >> AL_ETH_RX_GPD_INNER_L4_PROTO_SHIFT) &
		AL_ETH_RX_GPD_INNER_L4_PROTO_MASK;

	/* mask_2 fields */
	rx_gpd_entry->parse_ctrl_mask = (gpd_mask >> AL_ETH_RX_GPD_OUTER_PARSE_CTRL_SHIFT) &
		AL_ETH_RX_GPD_OUTER_PARSE_CTRL_MASK;
	rx_gpd_entry->outer_l3_len_mask = (gpd_mask >> AL_ETH_RX_GPD_INNER_PARSE_CTRL_SHIFT) &
		AL_ETH_RX_GPD_INNER_PARSE_CTRL_MASK;
	rx_gpd_entry->l3_priority_mask = (gpd_mask >> AL_ETH_RX_GPD_L3_PRIORITY_SHIFT) &
		AL_ETH_RX_GPD_L3_PRIORITY_MASK;
	rx_gpd_entry->l4_dst_port_lsb_mask = (gpd_mask >> AL_ETH_RX_GPD_L4_DST_PORT_LSB_SHIFT) &
		AL_ETH_RX_GPD_L4_DST_PORT_LSB_MASK;

	rx_gpd_entry->rx_gpd_cam_ctrl = (gpd_ctrl >> AL_ETH_RX_GPD_CAM_CTRL_VALID_SHIFT);
}

int al_eth_rx_generic_crc_table_entry_set(struct al_hal_eth_adapter *adapter,
	unsigned int engine_idx, uint32_t idx, struct al_eth_rx_gcp_table_entry *rx_gcp_entry)
{
	uint32_t gcp_table_gen;
	uint32_t rx_alu_opcode;
	uint32_t rx_alu_opsel;
	uint32_t rx_ext_1;
	uint32_t rx_ext_2;
	uint32_t rx_ext_6;

	al_assert((adapter->rev_id > AL_ETH_REV_ID_2));
	al_assert((engine_idx < AL_ETH_CRC_ENGINE_V3_MAX) ||
		(engine_idx < AL_ETH_CRC_ENGINE_V4_MAX && adapter->rev_id >= AL_ETH_REV_ID_4));

	gcp_table_gen  = (rx_gcp_entry->poly_sel & AL_ETH_RX_GCP_POLY_SEL_MASK) <<
		AL_ETH_RX_GCP_POLY_SEL_SHIFT;
	gcp_table_gen |= (rx_gcp_entry->crc32_bit_comp & AL_ETH_RX_GCP_CRC32_BIT_COMP_MASK) <<
		AL_ETH_RX_GCP_CRC32_BIT_COMP_SHIFT;
	gcp_table_gen |= (rx_gcp_entry->crc32_bit_swap & AL_ETH_RX_GCP_CRC32_BIT_SWAP_MASK) <<
		AL_ETH_RX_GCP_CRC32_BIT_SWAP_SHIFT;
	gcp_table_gen |= (rx_gcp_entry->crc32_byte_swap & AL_ETH_RX_GCP_CRC32_BYTE_SWAP_MASK) <<
		AL_ETH_RX_GCP_CRC32_BYTE_SWAP_SHIFT;
	gcp_table_gen |= (rx_gcp_entry->data_bit_swap & AL_ETH_RX_GCP_DATA_BIT_SWAP_MASK) <<
		AL_ETH_RX_GCP_DATA_BIT_SWAP_SHIFT;
	gcp_table_gen |= (rx_gcp_entry->data_byte_swap & AL_ETH_RX_GCP_DATA_BYTE_SWAP_MASK) <<
		AL_ETH_RX_GCP_DATA_BYTE_SWAP_SHIFT;
	gcp_table_gen |= (rx_gcp_entry->trail_size & AL_ETH_RX_GCP_TRAIL_SIZE_MASK) <<
		AL_ETH_RX_GCP_TRAIL_SIZE_SHIFT;
	gcp_table_gen |= (rx_gcp_entry->head_size & AL_ETH_RX_GCP_HEAD_SIZE_MASK) <<
		AL_ETH_RX_GCP_HEAD_SIZE_SHIFT;
	gcp_table_gen |= (rx_gcp_entry->head_calc & AL_ETH_RX_GCP_HEAD_CALC_MASK) <<
		AL_ETH_RX_GCP_HEAD_CALC_SHIFT;
	gcp_table_gen |= (rx_gcp_entry->mask_polarity & AL_ETH_RX_GCP_MASK_POLARITY_MASK) <<
		AL_ETH_RX_GCP_MASK_POLARITY_SHIFT;

	rx_alu_opcode  = (rx_gcp_entry->rx_alu_opcode_1 & AL_ETH_RX_GCP_OPCODE_1_MASK) <<
		AL_ETH_RX_GCP_OPCODE_1_SHIFT;
	rx_alu_opcode |= (rx_gcp_entry->rx_alu_opcode_2 & AL_ETH_RX_GCP_OPCODE_2_MASK) <<
		AL_ETH_RX_GCP_OPCODE_2_SHIFT;
	rx_alu_opcode |= (rx_gcp_entry->rx_alu_opcode_3 & AL_ETH_RX_GCP_OPCODE_3_MASK) <<
		AL_ETH_RX_GCP_OPCODE_3_SHIFT;
	rx_alu_opsel  = (rx_gcp_entry->rx_alu_opsel_1 & AL_ETH_RX_GCP_OPSEL_1_MASK) <<
		AL_ETH_RX_GCP_OPSEL_1_SHIFT;
	rx_alu_opsel |= (rx_gcp_entry->rx_alu_opsel_2 & AL_ETH_RX_GCP_OPSEL_2_MASK) <<
		AL_ETH_RX_GCP_OPSEL_2_SHIFT;
	rx_alu_opsel |= (rx_gcp_entry->rx_alu_opsel_3 & AL_ETH_RX_GCP_OPSEL_3_MASK) <<
		AL_ETH_RX_GCP_OPSEL_3_SHIFT;
	rx_alu_opsel |= (rx_gcp_entry->rx_alu_opsel_4 & AL_ETH_RX_GCP_OPSEL_4_MASK) <<
		AL_ETH_RX_GCP_OPSEL_4_SHIFT;

	/*  Rx Generic crc prameters table general */
	if (adapter->rev_id > AL_ETH_REV_ID_3) {
		struct al_ec_crc __iomem *crc_regs_base = &adapter->ec_regs_base->crc[engine_idx];

		rx_ext_1  = (rx_gcp_entry->poly_sel_ext &
			AL_ETH_RX_GCP_POLY_SEL_EXT_MASK) <<
			AL_ETH_RX_GCP_POLY_SEL_EXT_SHIFT;
		rx_ext_1 |= (rx_gcp_entry->crc_cks_sel &
			AL_ETH_RX_GCP_CRC_CKS_SEL_MASK) <<
			AL_ETH_RX_GCP_CRC_CKS_SEL_SHIFT;
		rx_ext_1 |= (rx_gcp_entry->trail_calc &
			AL_ETH_RX_GCP_TRAIL_CALC_MASK) <<
			AL_ETH_RX_GCP_TRAIL_CALC_SHIFT;
		rx_ext_1 |= (rx_gcp_entry->rx_trail_alu_opsel_1 &
			AL_ETH_RX_GCP_TRAIL_OPSEL_1_MASK) <<
			AL_ETH_RX_GCP_TRAIL_OPSEL_1_SHIFT;
		rx_ext_1 |= (rx_gcp_entry->rx_trail_alu_opsel_2 &
			AL_ETH_RX_GCP_TRAIL_OPSEL_2_MASK) <<
			AL_ETH_RX_GCP_TRAIL_OPSEL_2_SHIFT;
		rx_ext_1 |= (rx_gcp_entry->rx_trail_alu_opsel_3 &
			AL_ETH_RX_GCP_TRAIL_OPSEL_3_MASK) <<
			AL_ETH_RX_GCP_TRAIL_OPSEL_3_SHIFT;
		rx_ext_1 |= (rx_gcp_entry->rx_trail_alu_opsel_4 &
			AL_ETH_RX_GCP_TRAIL_OPSEL_4_MASK) <<
			AL_ETH_RX_GCP_TRAIL_OPSEL_4_SHIFT;
		rx_ext_1 |= (rx_gcp_entry->head_size_ext &
			AL_ETH_RX_GCP_HEAD_SIZE_EXT_MASK) <<
			AL_ETH_RX_GCP_HEAD_SIZE_EXT_SHIFT;

		rx_ext_2  = (rx_gcp_entry->crc_field_size &
			AL_ETH_RX_GCP_CRC_FIELD_SIZE_MASK) <<
			AL_ETH_RX_GCP_CRC_FIELD_SIZE_SHIFT;
		rx_ext_2 |= (rx_gcp_entry->crc_field_offset_calc &
			AL_ETH_RX_GCP_CRC_FIELD_CALC_MASK) <<
			AL_ETH_RX_GCP_CRC_FIELD_CALC_SHIFT;
		rx_ext_2 |= (rx_gcp_entry->crc_field_offset &
			AL_ETH_RX_GCP_CRC_FIELD_OFFSET_MASK) <<
			AL_ETH_RX_GCP_CRC_FIELD_OFFSET_SHIFT;

		rx_ext_6  = (rx_gcp_entry->rx_trail_alu_opcode_1 &
			AL_ETH_RX_GCP_TRAIL_OPCODE_1_MASK) <<
			AL_ETH_RX_GCP_TRAIL_OPCODE_1_SHIFT;
		rx_ext_6 |= (rx_gcp_entry->rx_trail_alu_opcode_2 &
			AL_ETH_RX_GCP_TRAIL_OPCODE_2_MASK) <<
			AL_ETH_RX_GCP_TRAIL_OPCODE_2_SHIFT;
		rx_ext_6 |= (rx_gcp_entry->rx_trail_alu_opcode_3 &
			AL_ETH_RX_GCP_TRAIL_OPCODE_3_MASK) <<
			AL_ETH_RX_GCP_TRAIL_OPCODE_3_SHIFT;
		rx_ext_6 |= (rx_gcp_entry->crc_replace_sel &
			AL_ETH_RX_GCP_CRC_REPLACE_SEL_MASK) <<
			AL_ETH_RX_GCP_CRC_REPLACE_SEL_SHIFT;
		rx_ext_6 |= (rx_gcp_entry->crc_detect_sel &
			AL_ETH_RX_GCP_CRC_DETECT_SEL_MASK) <<
			AL_ETH_RX_GCP_CRC_DETECT_SEL_SHIFT;

		al_reg_write32(&crc_regs_base->v4_rx_gcp_table_addr, idx);
		al_reg_write32(&crc_regs_base->v4_rx_gcp_table_gen, gcp_table_gen);
		al_reg_write32(&crc_regs_base->v4_rx_gcp_table_mask_1, rx_gcp_entry->gcp_mask[0]);
		al_reg_write32(&crc_regs_base->v4_rx_gcp_table_mask_2, rx_gcp_entry->gcp_mask[1]);
		al_reg_write32(&crc_regs_base->v4_rx_gcp_table_mask_3, rx_gcp_entry->gcp_mask[2]);
		al_reg_write32(&crc_regs_base->v4_rx_gcp_table_mask_4, rx_gcp_entry->gcp_mask[3]);
		al_reg_write32(&crc_regs_base->v4_rx_gcp_table_mask_5, rx_gcp_entry->gcp_mask[4]);
		al_reg_write32(&crc_regs_base->v4_rx_gcp_table_mask_6, rx_gcp_entry->gcp_mask[5]);
		al_reg_write32(&crc_regs_base->v4_rx_gcp_table_crc_init, rx_gcp_entry->crc_init);
		al_reg_write32(&crc_regs_base->v4_rx_gcp_table_res, rx_gcp_entry->gcp_table_res);
		al_reg_write32(&crc_regs_base->v4_rx_gcp_table_alu_opcode, rx_alu_opcode);
		al_reg_write32(&crc_regs_base->v4_rx_gcp_table_alu_opsel, rx_alu_opsel);
		al_reg_write32(&crc_regs_base->v4_rx_gcp_table_alu_val, rx_gcp_entry->alu_val);
		al_reg_write32(&crc_regs_base->v4_rx_gcp_table_ext_1, rx_ext_1);
		al_reg_write32(&crc_regs_base->v4_rx_gcp_table_ext_2, rx_ext_2);
		al_reg_write32(&crc_regs_base->v4_rx_gcp_table_ext_3,
			rx_gcp_entry->crc_check_value);
		al_reg_write32(&crc_regs_base->v4_rx_gcp_table_ext_4,
			rx_gcp_entry->crc_replace_value);
		al_reg_write32(&crc_regs_base->v4_rx_gcp_table_ext_5,
			rx_gcp_entry->crc_detect_value);
		al_reg_write32(&crc_regs_base->v4_rx_gcp_table_ext_6, rx_ext_6);
	} else {
		al_reg_write32(&adapter->ec_regs_base->rfw_v3.rx_gcp_table_addr, idx);
		al_reg_write32(&adapter->ec_regs_base->rfw_v3.rx_gcp_table_gen,
				gcp_table_gen);
		al_reg_write32(&adapter->ec_regs_base->rfw_v3.rx_gcp_table_mask_1,
				rx_gcp_entry->gcp_mask[0]);
		al_reg_write32(&adapter->ec_regs_base->rfw_v3.rx_gcp_table_mask_2,
				rx_gcp_entry->gcp_mask[1]);
		al_reg_write32(&adapter->ec_regs_base->rfw_v3.rx_gcp_table_mask_3,
				rx_gcp_entry->gcp_mask[2]);
		al_reg_write32(&adapter->ec_regs_base->rfw_v3.rx_gcp_table_mask_4,
				rx_gcp_entry->gcp_mask[3]);
		al_reg_write32(&adapter->ec_regs_base->rfw_v3.rx_gcp_table_mask_5,
				rx_gcp_entry->gcp_mask[4]);
		al_reg_write32(&adapter->ec_regs_base->rfw_v3.rx_gcp_table_mask_6,
				rx_gcp_entry->gcp_mask[5]);
		al_reg_write32(&adapter->ec_regs_base->rfw_v3.rx_gcp_table_crc_init,
				rx_gcp_entry->crc_init);
		al_reg_write32(&adapter->ec_regs_base->rfw_v3.rx_gcp_table_res,
				rx_gcp_entry->gcp_table_res);
		al_reg_write32(&adapter->ec_regs_base->rfw_v3.rx_gcp_table_alu_opcode,
				rx_alu_opcode);
		al_reg_write32(&adapter->ec_regs_base->rfw_v3.rx_gcp_table_alu_opsel,
				rx_alu_opsel);
		al_reg_write32(&adapter->ec_regs_base->rfw_v3.rx_gcp_table_alu_val,
				rx_gcp_entry->alu_val);
	};
	return 0;
}

void al_eth_rx_generic_crc_table_entry_get(struct al_hal_eth_adapter *adapter, uint32_t idx,
		struct al_eth_rx_gcp_table_entry *rx_gcp_entry)
{
	uint32_t gcp_table_gen;
	uint32_t rx_alu_opcode;
	uint32_t rx_alu_opsel;

	al_assert(adapter);
	al_assert(rx_gcp_entry);
	al_assert(idx < AL_ETH_RX_GCP_TABLE_SIZE);
	al_assert(adapter->rev_id == AL_ETH_REV_ID_3);

	/*  Rx Generic crc prameters table general */
	al_reg_write32(&adapter->ec_regs_base->rfw_v3.rx_gcp_table_addr, idx);
	al_local_data_memory_barrier();
	/* rx_gcp_table_gen is the read trigger for the entire entry */
	gcp_table_gen = al_reg_read32(&adapter->ec_regs_base->rfw_v3.rx_gcp_table_gen);
	rx_gcp_entry->gcp_mask[0] =
		al_reg_read32(&adapter->ec_regs_base->rfw_v3.rx_gcp_table_mask_1);
	rx_gcp_entry->gcp_mask[1] =
		al_reg_read32(&adapter->ec_regs_base->rfw_v3.rx_gcp_table_mask_2);
	rx_gcp_entry->gcp_mask[2] =
		al_reg_read32(&adapter->ec_regs_base->rfw_v3.rx_gcp_table_mask_3);
	rx_gcp_entry->gcp_mask[3] =
		al_reg_read32(&adapter->ec_regs_base->rfw_v3.rx_gcp_table_mask_4);
	rx_gcp_entry->gcp_mask[4] =
		al_reg_read32(&adapter->ec_regs_base->rfw_v3.rx_gcp_table_mask_5);
	rx_gcp_entry->gcp_mask[5] =
		al_reg_read32(&adapter->ec_regs_base->rfw_v3.rx_gcp_table_mask_6);
	rx_gcp_entry->crc_init =
		al_reg_read32(&adapter->ec_regs_base->rfw_v3.rx_gcp_table_crc_init);
	rx_gcp_entry->gcp_table_res =
		al_reg_read32(&adapter->ec_regs_base->rfw_v3.rx_gcp_table_res);
	rx_alu_opcode = al_reg_read32(&adapter->ec_regs_base->rfw_v3.rx_gcp_table_alu_opcode);
	rx_alu_opsel = al_reg_read32(&adapter->ec_regs_base->rfw_v3.rx_gcp_table_alu_opsel);
	rx_gcp_entry->alu_val = al_reg_read32(&adapter->ec_regs_base->rfw_v3.rx_gcp_table_alu_val);

	rx_gcp_entry->poly_sel = (gcp_table_gen >> AL_ETH_RX_GCP_POLY_SEL_SHIFT) &
		AL_ETH_RX_GCP_POLY_SEL_MASK;
	rx_gcp_entry->crc32_bit_comp = (gcp_table_gen >> AL_ETH_RX_GCP_CRC32_BIT_COMP_SHIFT) &
		AL_ETH_RX_GCP_CRC32_BIT_COMP_MASK;
	rx_gcp_entry->crc32_bit_swap = (gcp_table_gen >> AL_ETH_RX_GCP_CRC32_BIT_SWAP_SHIFT) &
		AL_ETH_RX_GCP_CRC32_BIT_SWAP_MASK;
	rx_gcp_entry->crc32_byte_swap = (gcp_table_gen >> AL_ETH_RX_GCP_CRC32_BYTE_SWAP_SHIFT) &
		AL_ETH_RX_GCP_CRC32_BYTE_SWAP_MASK;
	rx_gcp_entry->data_bit_swap = (gcp_table_gen >> AL_ETH_RX_GCP_DATA_BIT_SWAP_SHIFT) &
		AL_ETH_RX_GCP_DATA_BIT_SWAP_MASK;
	rx_gcp_entry->data_byte_swap = (gcp_table_gen >> AL_ETH_RX_GCP_DATA_BYTE_SWAP_SHIFT) &
		AL_ETH_RX_GCP_DATA_BYTE_SWAP_MASK;
	rx_gcp_entry->trail_size = (gcp_table_gen >> AL_ETH_RX_GCP_TRAIL_SIZE_SHIFT) &
		AL_ETH_RX_GCP_TRAIL_SIZE_MASK;
	rx_gcp_entry->head_size = (gcp_table_gen >> AL_ETH_RX_GCP_HEAD_SIZE_SHIFT) &
		AL_ETH_RX_GCP_HEAD_SIZE_MASK;
	rx_gcp_entry->head_calc = (gcp_table_gen >> AL_ETH_RX_GCP_HEAD_CALC_SHIFT) &
		AL_ETH_RX_GCP_HEAD_CALC_MASK;
	rx_gcp_entry->mask_polarity = (gcp_table_gen >> AL_ETH_RX_GCP_MASK_POLARITY_SHIFT) &
		AL_ETH_RX_GCP_MASK_POLARITY_MASK;

	rx_gcp_entry->rx_alu_opcode_1 = (rx_alu_opcode >> AL_ETH_RX_GCP_OPCODE_1_SHIFT) &
		AL_ETH_RX_GCP_OPCODE_1_MASK;
	rx_gcp_entry->rx_alu_opcode_2 = (rx_alu_opcode >> AL_ETH_RX_GCP_OPCODE_2_SHIFT) &
		AL_ETH_RX_GCP_OPCODE_2_MASK;
	rx_gcp_entry->rx_alu_opcode_3 = (rx_alu_opcode >> AL_ETH_RX_GCP_OPCODE_3_SHIFT) &
		AL_ETH_RX_GCP_OPCODE_3_MASK;
	rx_gcp_entry->rx_alu_opsel_1 = (rx_alu_opsel >> AL_ETH_RX_GCP_OPSEL_1_SHIFT) &
		AL_ETH_RX_GCP_OPSEL_1_MASK;
	rx_gcp_entry->rx_alu_opsel_2 = (rx_alu_opsel >> AL_ETH_RX_GCP_OPSEL_2_SHIFT) &
		AL_ETH_RX_GCP_OPSEL_2_MASK;
	rx_gcp_entry->rx_alu_opsel_3 = (rx_alu_opsel >> AL_ETH_RX_GCP_OPSEL_3_SHIFT) &
		AL_ETH_RX_GCP_OPSEL_3_MASK;
	rx_gcp_entry->rx_alu_opsel_4 = (rx_alu_opsel >> AL_ETH_RX_GCP_OPSEL_4_SHIFT) &
		AL_ETH_RX_GCP_OPSEL_4_MASK;
}

int al_eth_tx_protocol_detect_table_init_ex(struct al_hal_eth_adapter *adapter,
						struct al_eth_tx_gpd_cam_entry *entries,
						unsigned int num_entries)
{
	unsigned int idx;

	al_assert(adapter->rev_id == AL_ETH_REV_ID_3);
	al_assert(num_entries <= AL_ETH_TX_GPD_TABLE_SIZE);

	for (idx = 0; idx < num_entries; idx++)
			al_eth_tx_protocol_detect_table_entry_set(adapter, idx, &entries[idx]);

	return 0;
}

int al_eth_tx_protocol_detect_table_init(struct al_hal_eth_adapter *adapter)
{
	al_assert((adapter->rev_id > AL_ETH_REV_ID_2));

	if (adapter->rev_id == AL_ETH_REV_ID_3)
		return al_eth_tx_protocol_detect_table_init_ex(adapter,
			al_eth_generic_tx_crc_gpd_v3,
			AL_ETH_TX_GPD_TABLE_SIZE);

	return 0;
}

int al_eth_tx_generic_crc_table_init_ex(struct al_hal_eth_adapter *adapter,
					struct al_eth_tx_gcp_table_entry *entries,
					unsigned int num_entries)
{
	unsigned int idx;

	al_assert((adapter->rev_id == AL_ETH_REV_ID_3));
	al_assert(num_entries <= AL_ETH_TX_GCP_TABLE_SIZE);

	al_dbg("eth [%s]: enable tx_generic_crc\n", adapter->name);
	al_reg_write32(&adapter->ec_regs_base->tfw_v3.tx_gcp_legacy, 0x0);
	al_reg_write32(&adapter->ec_regs_base->tfw_v3.crc_csum_replace, 0x0);
	for (idx = 0; idx < num_entries; idx++)
		al_eth_tx_generic_crc_table_entry_set(adapter, 0, idx, &entries[idx]);

	return 0;
}

int al_eth_tx_generic_crc_table_init(struct al_hal_eth_adapter *adapter, unsigned int engine_idx)
{
	int reg;

	al_assert((adapter->rev_id > AL_ETH_REV_ID_2));
	al_assert((engine_idx < AL_ETH_CRC_ENGINE_V3_MAX) ||
		(engine_idx < AL_ETH_CRC_ENGINE_V4_MAX && adapter->rev_id >= AL_ETH_REV_ID_4));

	al_dbg("eth [%s]: enable tx_generic_crc\n", adapter->name);
	al_reg_write32(&adapter->ec_regs_base->tfw_v3.tx_gcp_legacy, 0x0);

	if (adapter->rev_id == AL_ETH_REV_ID_3) {
		return al_eth_tx_generic_crc_table_init_ex(adapter,
				al_eth_generic_tx_crc_gcp_v3,
				AL_ETH_TX_GPD_TABLE_SIZE);
	} else {
		struct al_ec_crc __iomem *crc_regs_base = &adapter->ec_regs_base->crc[engine_idx];

		al_reg_write32(&adapter->ec_regs_base->tfw_v3.crc_csum_replace, 0xb);
		reg = 0;
		AL_REG_FIELD_SET(reg, EC_CRC_V4_TX_GCP_ALU_P1_OFFSET_MASK,
				EC_CRC_V4_TX_GCP_ALU_P1_OFFSET_SHIFT, 0xc8);
		AL_REG_FIELD_SET(reg, EC_CRC_V4_TX_GCP_ALU_P1_SHIFT_MASK,
				EC_CRC_V4_TX_GCP_ALU_P1_SHIFT_SHIFT, 0x1);
		AL_REG_FIELD_SET(reg, EC_CRC_V4_TX_GCP_ALU_P1_SIZE_MASK,
				EC_CRC_V4_TX_GCP_ALU_P1_SIZE_SHIFT, 0x8);
		al_reg_write32(&crc_regs_base->v4_tx_gcp_alu_p1, reg);

		reg = 0;
		AL_REG_FIELD_SET(reg, EC_CRC_V4_TX_GCP_ALU_P2_OFFSET_MASK,
				EC_CRC_V4_TX_GCP_ALU_P2_OFFSET_SHIFT, 0x6A);
		AL_REG_FIELD_SET(reg, EC_CRC_V4_TX_GCP_ALU_P2_SHIFT_MASK,
				EC_CRC_V4_TX_GCP_ALU_P2_SHIFT_SHIFT, 0x1);
		AL_REG_FIELD_SET(reg, EC_CRC_V4_TX_GCP_ALU_P2_SIZE_MASK,
				EC_CRC_V4_TX_GCP_ALU_P2_SIZE_SHIFT, 0x6);
		al_reg_write32(&crc_regs_base->v4_tx_gcp_alu_p2, reg);
		/* substrare encryption exists */
		al_reg_write32(&crc_regs_base->v4_tx_gcp_alu_p3, 0xc6);
		/* VPC encryption exists */
		al_reg_write32(&crc_regs_base->v4_tx_gcp_alu_p4, 0xc7);
		/* reserved */
		al_reg_write32(&crc_regs_base->v4_tx_gcp_alu_p5, 0xc7);
		/* reserved */
		al_reg_write32(&crc_regs_base->v4_tx_gcp_alu_p6, 0xc7);

		/* No sub or vpc encryption */
		al_reg_write32(&crc_regs_base->v4_tx_gcp_alu_const_table_addr, 0x0);
		/* Encryption headers size + UDP - this is the selected alu input */
		al_reg_write32(&crc_regs_base->v4_tx_gcp_alu_const_table_1, 0);
		/* Encryption headers size + UDP - not used */
		al_reg_write32(&crc_regs_base->v4_tx_gcp_alu_const_table_2, 0);
		/* reserved */
		al_reg_write32(&crc_regs_base->v4_tx_gcp_alu_const_table_3, 0);

		/* sub encryption only*/
		al_reg_write32(&crc_regs_base->v4_tx_gcp_alu_const_table_addr, 0x1);
		/* Encryption headers size + UDP - this is the selected alu input */
		al_reg_write32(&crc_regs_base->v4_tx_gcp_alu_const_table_1, 28);
		/* Encryption headers size + UDP - not used */
		al_reg_write32(&crc_regs_base->v4_tx_gcp_alu_const_table_2, 28);
		/* reserved */
		al_reg_write32(&crc_regs_base->v4_tx_gcp_alu_const_table_3, 0);

		/* vpc encryption only*/
		al_reg_write32(&crc_regs_base->v4_tx_gcp_alu_const_table_addr, 0xe);
		/* Encryption headers size + UDP - this is the selected alu input */
		al_reg_write32(&crc_regs_base->v4_tx_gcp_alu_const_table_1, 28);
		/* Encryption headers size + UDP - not used */
		al_reg_write32(&crc_regs_base->v4_tx_gcp_alu_const_table_2, 28);
		/* reserved */
		al_reg_write32(&crc_regs_base->v4_tx_gcp_alu_const_table_3, 0);

		/* sub and vpc encryption*/
		al_reg_write32(&crc_regs_base->v4_tx_gcp_alu_const_table_addr, 0xf);
		/* Encryption headers size + UDP - this is the selected alu input */
		al_reg_write32(&crc_regs_base->v4_tx_gcp_alu_const_table_1, 48);
		/* Encryption headers size + UDP - not used */
		al_reg_write32(&crc_regs_base->v4_tx_gcp_alu_const_table_2, 48);
		/* reserved */
		al_reg_write32(&crc_regs_base->v4_tx_gcp_alu_const_table_3, 0);
	}
	return 0;
}

int al_eth_tx_crc_chksum_replace_cmd_init_ex(struct al_hal_eth_adapter *adapter,
			struct al_eth_tx_crc_chksum_replace_cmd_for_protocol_num_entry *entries,
			unsigned int num_entries)
{
	unsigned int idx;
	al_assert((adapter->rev_id == AL_ETH_REV_ID_3));
	al_assert(num_entries <= AL_ETH_TX_CRC_CSUM_REPLACE_SW_TABLE_SIZE);

	for (idx = 0; idx < num_entries; idx++)
		al_eth_tx_crc_chksum_replace_cmd_entry_set(adapter, idx, &entries[idx]);

	return 0;
}

int al_eth_tx_crc_chksum_replace_cmd_init(struct al_hal_eth_adapter *adapter)
{
	al_assert((adapter->rev_id > AL_ETH_REV_ID_2));

	if (adapter->rev_id == AL_ETH_REV_ID_3) {
		return al_eth_tx_crc_chksum_replace_cmd_init_ex(adapter,
							al_eth_tx_crc_chksum_replace_cmd_v3,
							AL_ETH_TX_GPD_TABLE_SIZE);
	}

	return 0;
}

int al_eth_rx_protocol_detect_table_init_ex(struct al_hal_eth_adapter *adapter,
					    struct al_eth_rx_gpd_cam_entry *entries,
					    unsigned int num_entries)
{
	unsigned int idx;

	al_assert((adapter->rev_id == AL_ETH_REV_ID_3));
	al_assert(num_entries <= AL_ETH_RX_GPD_TABLE_SIZE);

	al_reg_write32(&adapter->ec_regs_base->rfw_v3.gpd_p1,
			AL_ETH_RX_GPD_PARSE_RESULT_OUTER_L3_PROTO_IDX_OFFSET(adapter->rev_id));
	al_reg_write32(&adapter->ec_regs_base->rfw_v3.gpd_p2,
			AL_ETH_RX_GPD_PARSE_RESULT_OUTER_L4_PROTO_IDX_OFFSET(adapter->rev_id));
	al_reg_write32(&adapter->ec_regs_base->rfw_v3.gpd_p3,
			AL_ETH_RX_GPD_PARSE_RESULT_INNER_L3_PROTO_IDX_OFFSET(adapter->rev_id));
	al_reg_write32(&adapter->ec_regs_base->rfw_v3.gpd_p4,
			AL_ETH_RX_GPD_PARSE_RESULT_INNER_L4_PROTO_IDX_OFFSET(adapter->rev_id));
	al_reg_write32(&adapter->ec_regs_base->rfw_v3.gpd_p5,
			AL_ETH_RX_GPD_PARSE_RESULT_OUTER_PARSE_CTRL(adapter->rev_id));
	al_reg_write32(&adapter->ec_regs_base->rfw_v3.gpd_p6,
			AL_ETH_RX_GPD_PARSE_RESULT_INNER_PARSE_CTRL(adapter->rev_id));
	al_reg_write32(&adapter->ec_regs_base->rfw_v3.gpd_p7,
			AL_ETH_RX_GPD_PARSE_RESULT_L3_PRIORITY(adapter->rev_id));
	al_reg_write32(&adapter->ec_regs_base->rfw_v3.gpd_p8,
			AL_ETH_RX_GPD_PARSE_RESULT_OUTER_L4_DST_PORT_LSB(adapter->rev_id));

	if (adapter->rev_id == AL_ETH_REV_ID_3) {
		for (idx = 0; idx < num_entries; idx++)
			al_eth_rx_protocol_detect_table_entry_set(adapter, idx, &entries[idx]);
	}

	return 0;
}

int al_eth_rx_protocol_detect_table_init(struct al_hal_eth_adapter *adapter)
{
	if (adapter->rev_id == AL_ETH_REV_ID_3) {
		return al_eth_rx_protocol_detect_table_init_ex(adapter,
							al_eth_generic_rx_crc_gpd_v3,
							AL_ETH_RX_GCP_TABLE_SIZE);

	}

	return 0;
}

int al_eth_tx_generic_crc_shared_table_init(struct al_hal_eth_adapter *adapter)
{
	struct al_ec_crc_extended __iomem *crc_ex;
	int i;

	al_assert(adapter);

	crc_ex = &adapter->ec_regs_base->crc_extended;

	/* Insert novice crc8n and rrd crc8n after FIFO 8bit (ENA, EFA and EBS packets)*/
	for (i = 31; i < 43; i++) {
		al_reg_write32(&crc_ex->tx_insert_en_table_addr, i);
		al_reg_write32(&crc_ex->tx_insert_en_table_1, 0x00002500);
	}

	/* Insert novice crc8n after FIFO 8bit (VXLAN packets)*/
	for (i = 49; i < 57; i++) {
		al_reg_write32(&crc_ex->tx_insert_en_table_addr, i);
		al_reg_write32(&crc_ex->tx_insert_en_table_1, 0x00002100);
	}

	/* ENA - IPv4 fragment*/
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x01f);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x00001f0);
	/* substrare encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x05f);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x00001f0);
	/* VPC encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x39f);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x00001f0);
	/* substrare encryption exists and VPC encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x3df);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x00001f0);

	/*  Novice v2 / IPv4   (over IPV4 and udp) */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x020);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000200);
	/* substrare encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x060);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000200);
	/* VPC encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x3a0);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000200);
	/* substrare encryption exists and VPC encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x3e0);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000200);

	/*   Novice v2 / IPv4 / UDP   (over IPV4 and udp) */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x021);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000210);
	/* substrare encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x061);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000210);
	/* VPC encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x3a1);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000210);
	/* substrare encryption exists and VPC encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x3e1);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000210);


	/*   Novice v2 / IPv4 / TCP   (over IPV4 and udp) */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x022);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000220);
	/* substrare encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x062);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000220);
	/* VPC encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x3a2);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000220);
	/* substrare encryption exists and VPC encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x3e2);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000220);

	/*    Novice v2 / IPv6   (over IPV4 and udp) */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x023);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000230);
	/* substrare encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x063);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000230);
	/* VPC encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x3a3);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000230);
	/* substrare encryption exists and VPC encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x3e3);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000230);

	/*    Novice v2 / IPv6 / UDP   (over IPV4 and udp) */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x024);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000240);
	/* substrare encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x064);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000240);
	/* VPC encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x3a4);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000240);
	/* substrare encryption exists and VPC encryption exists*/
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x3e4);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000240);

	/*    Novice v2 / IPv6 / TCP   (over IPV4 and udp) */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x025);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000250);
	/* substrare encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x065);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000250);
	/* VPC encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x3a5);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000250);
	/* substrare encryption exists and VPC encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x3e5);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000250);

	/*     Novice v2 / IPv4 / UDP / RRD   (over IPV4 and udp) */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x026);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000260);
	/* substrare encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x066);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000260);
	/* VPC encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x3a6);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000260);
	/* substrare encryption exists and VPC encryption exists*/
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x3e6);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000260);

	/*     Novice v2 / IPv6 / UDP / RRD   (over IPV4 and udp) */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x027);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000270);
	/* substrare encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x067);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000270);
	/* VPC encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x3a7);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000270);
	/* substrare encryption exists and VPC encryption exists*/
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x3e7);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000270);

	/*      Novice v2  no customer packets   (over IPV4 and udp) */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x028);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000280);
	/* substrare encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x068);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000280);
	/* VPC encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x3a8);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000280);
	/* substrare encryption exists and VPC encryption exists*/
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x3e8);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000280);

	/*      Novice v2  no customer packets   (over IPV4 and udp) */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x028);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000280);
	/* substrare encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x068);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000280);
	/* VPC encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x3a8);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000280);
	/* substrare encryption exists and VPC encryption exists*/
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x3e8);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000280);

	/*      IPv4 / UDP / RRD */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x029);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000290);
	/* substrare encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x069);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000290);
	/* VPC encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x3a9);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000290);
	/* substrare encryption exists and VPC encryption exists*/
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x3e9);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000290);

	/*      IPv6 / UDP / RRD */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x02a);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x00002a0);
	/* substrare encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x06a);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x00002a0);
	/* VPC encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x3aa);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x00002a0);
	/* substrare encryption exists and VPC encryption exists*/
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x3ea);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x00002a0);

	/*      Double tunnel with VxLAN L2 packet */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x031);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000310);
	/* substrare encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x071);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000310);
	/* VPC encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x3b1);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000310);
	/* substrare encryption exists and VPC encryption exists*/
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x3f1);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000310);

	/*      Double tunnel with VxLAN IPv4 fragmented */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x032);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000320);
	/* substrare encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x072);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000320);
	/* VPC encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x3b2);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000320);
	/* substrare encryption exists and VPC encryption exists*/
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x3f2);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000320);

	/*      Double tunnel with VxLAN IPv4 */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x033);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000330);
	/* substrare encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x073);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000330);
	/* VPC encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x3b3);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000330);
	/* substrare encryption exists and VPC encryption exists*/
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x3f3);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000330);

	/*      Double tunnel with VxLAN IPv4 UDP */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x034);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000340);
	/* substrare encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x074);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000340);
	/* VPC encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x3b4);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000340);
	/* substrare encryption exists and VPC encryption exists*/
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x3f4);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000340);

	/*      Double tunnel with VxLAN IPv4 TCP */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x035);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000350);
	/* substrare encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x075);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000350);
	/* VPC encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x3b5);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000350);
	/* substrare encryption exists and VPC encryption exists*/
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x3f5);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000350);

	/*      Double tunnel with VxLAN IPv6 */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x036);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000360);
	/* substrare encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x076);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000360);
	/* VPC encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x3b6);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000360);
	/* substrare encryption exists and VPC encryption exists*/
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x3f6);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000360);

	/*      Double tunnel with VxLAN IPv6 UDP */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x037);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000370);
	/* substrare encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x077);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000370);
	/* VPC encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x3b7);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000370);
	/* substrare encryption exists and VPC encryption exists*/
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x3f7);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000370);

	/*      Double tunnel with VxLAN IPv6 TCP */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x038);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000380);
	/* substrare encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x078);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000380);
	/* VPC encryption exists */
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x3b8);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000380);
	/* substrare encryption exists and VPC encryption exists*/
	al_reg_write32(&crc_ex->tx_protocol_control_table_addr, 0x3f8);
	al_reg_write32(&crc_ex->tx_protocol_control_table_1, 0x0000380);

	return 0;
}

int al_eth_rx_generic_crc_shared_table_init(struct al_hal_eth_adapter *adapter)
{
	struct al_ec_crc_extended __iomem *crc_ex;

	al_assert(adapter);

	crc_ex = &adapter->ec_regs_base->crc_extended;

	/* ENA - IPv4 fragment*/
	/* no footer icrc no encryption- disable engine2 & engine3 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x01f);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK |
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_3_MASK);
		/* no footer icrc- disable engine2 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x05f);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
		/* no footer icrc- disable engine2 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x09f);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
		/* no footer icrc- disable engine2 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x0df);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	/* no encryption- disable engine3 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x31f);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_3_MASK);

	/*  Novice v2 / IPv4   (over IPV4 and udp) */
	/* no footer icrc no encryption- disable engine2 & engine3 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x020);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK |
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_3_MASK);
	/* no footer icrc- disable engine2 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x060);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	/* no footer icrc- disable engine2 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x0a0);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	/* no footer icrc- disable engine2 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x0e0);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	/* no encryption- disable engine3 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x320);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_3_MASK);

	/*   Novice v2 / IPv4 / UDP   (over IPV4 and udp) */
	/* no footer icrc no encryption- disable engine2 & engine3 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x021);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK |
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_3_MASK);
	/* no footer icrc- disable engine2 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x061);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	/* no footer icrc- disable engine2 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x0a1);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	/* no footer icrc- disable engine2 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x0e1);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	/* no encryption- disable engine3 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x321);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_3_MASK);

	/*   Novice v2 / IPv4 / TCP   (over IPV4 and udp) */
	/* no footer icrc no encryption- disable engine2 & engine3 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x022);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK |
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_3_MASK);
	/* no footer icrc- disable engine2 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x062);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	/* no footer icrc- disable engine2 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x0a2);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	/* no footer icrc- disable engine2 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x0e2);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	/* no encryption- disable engine3 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x322);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_3_MASK);

	/*    Novice v2 / IPv6   (over IPV4 and udp) */
	/* no footer icrc no encryption- disable engine2 & engine3 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x023);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK |
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_3_MASK);
	/* no footer icrc- disable engine2 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x063);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	/* no footer icrc- disable engine2 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x0a3);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	/* no footer icrc- disable engine2 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x0e3);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	/* no encryption- disable engine3 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x323);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_3_MASK);

	/*    Novice v2 / IPv6 / UDP   (over IPV4 and udp) */
	/* no footer icrc no encryption- disable engine2 & engine3 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x024);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK |
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_3_MASK);
	/* no footer icrc- disable engine2 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x064);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	/* no footer icrc- disable engine2 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x0a4);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	/* no footer icrc- disable engine2 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x0e4);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	/* no encryption- disable engine3 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x324);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_3_MASK);

	/*    Novice v2 / IPv6 / TCP   (over IPV4 and udp) */
	/* no footer icrc no encryption- disable engine2 & engine3 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x025);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK |
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_3_MASK);
	/* no footer icrc- disable engine2 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x065);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	/* no footer icrc- disable engine2 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x0a5);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	/* no footer icrc- disable engine2 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x0e5);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	/* no encryption- disable engine3 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x325);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_3_MASK);

	/*     Novice v2 / IPv4 / UDP / RRD   (over IPV4 and udp) */
	/* no footer icrc no encryption- disable engine2 & engine3 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x026);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK |
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_3_MASK);
	/* no footer icrc- disable engine2 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x066);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	/* no footer icrc- disable engine2 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x0a6);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	/* no footer icrc- disable engine2 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x0e6);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	/* no encryption- disable engine3 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x326);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_3_MASK);

	/*     Novice v2 / IPv6 / UDP / RRD   (over IPV4 and udp) */
	/* no footer icrc no encryption- disable engine2 & engine3 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x027);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK |
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_3_MASK);
	/* no footer icrc- disable engine2 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x067);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	/* no footer icrc- disable engine2 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x0a7);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	/* no footer icrc- disable engine2 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x0e7);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	/* no encryption- disable engine3 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x327);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_3_MASK);

	/*      Novice v2  no customer packets   (over IPV4 and udp) */
	/* no footer icrc no encryption- disable engine2 & engine3 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x028);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK |
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_3_MASK);
	/* no footer icrc- disable engine2 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x068);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	/* no footer icrc- disable engine2 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x0a8);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	/* no footer icrc- disable engine2 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x0e8);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	/* no encryption- disable engine3 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x328);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_3_MASK);

	/*      Double tunnel with VxLAN L2 packet */
	/* no footer icrc- disable engine2 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x031);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x071);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x0b1);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x0f1);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);

	/*      Double tunnel with VxLAN IPv4 fragmented */
	/* no footer icrc- disable engine2 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x032);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x072);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x0b2);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x0f2);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);

	/*      Double tunnel with VxLAN IPv4 */
	/* no footer icrc- disable engine2 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x033);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x073);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x0b3);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x0f3);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);

	/*      Double tunnel with VxLAN IPv4 UDP */
	/* no footer icrc- disable engine2 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x034);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x074);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x0b4);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x0f4);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);

	/*      Double tunnel with VxLAN IPv4 TCP */
	/* no footer icrc- disable engine2 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x035);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x075);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x0b5);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x0f5);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	/*      Double tunnel with VxLAN IPv6 */
	/* no footer icrc- disable engine2 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x036);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x076);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x0b6);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x0f6);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);

	/*      Double tunnel with VxLAN IPv6 UDP */
	/* no footer icrc- disable engine2 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x037);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x077);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x0b7);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x0f7);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);

	/*      Double tunnel with VxLAN IPv6 TCP */
	/* no footer icrc- disable engine2 */
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x038);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x078);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x0b8);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);
	al_reg_write32(&crc_ex->rx_protocol_control_table_addr, 0x0f8);
	al_reg_write32(&crc_ex->rx_protocol_control_table_1,
	EC_CRC_EXTENDED_RX_PROTOCOL_CONTROL_TABLE_1_CRC_FORCE_GCE_2_MASK);

	return 0;
}

int al_eth_rx_generic_crc_table_init_ex(struct al_hal_eth_adapter *adapter,
					struct al_eth_rx_gcp_table_entry *entries,
					unsigned int num_entries)
{
	unsigned int idx;

	al_assert((adapter->rev_id == AL_ETH_REV_ID_3));
	al_assert(num_entries <= AL_ETH_RX_GCP_TABLE_SIZE);

	al_dbg("eth [%s]: enable rx_generic_crc\n", adapter->name);

	for (idx = 0; idx < num_entries; idx++)
		al_eth_rx_generic_crc_table_entry_set(adapter, 0, idx, &entries[idx]);

	al_eth_rx_generic_crc_table_enable(adapter, AL_TRUE);
	return 0;
}

int al_eth_rx_generic_crc_table_init(struct al_hal_eth_adapter *adapter, unsigned int engine_idx)
	{
	al_assert((adapter->rev_id > AL_ETH_REV_ID_2));
	al_assert((engine_idx < AL_ETH_CRC_ENGINE_V3_MAX) ||
		(engine_idx < AL_ETH_CRC_ENGINE_V4_MAX && adapter->rev_id >= AL_ETH_REV_ID_4));

	al_dbg("eth [%s]: enable rx_generic_crc\n", adapter->name);

	if (adapter->rev_id == AL_ETH_REV_ID_3) {
		return al_eth_rx_generic_crc_table_init_ex(adapter,
					al_eth_generic_rx_crc_gcp_v3,
					AL_ETH_RX_GCP_TABLE_SIZE);
	} else if (engine_idx == 1) {
		struct al_ec_crc __iomem *crc_regs_base = &adapter->ec_regs_base->crc[engine_idx];

		/* v4_rx_gcp_alu_const.val_0 should be zero for end to end checksum offset */
		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const, 0x04020100);
		/* REQUEST(0)/RESPONSE(1) - EFA */
		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_p3, 0x3da);
		/* TS exists - EFA */
		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_p4, 0x3d9);
		/* REQUEST(0)/RESPONSE(1) - EBS */
		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_p5, 0x9ea);
		/* TS exists - EBS */
		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_p6, 0x9e9);

		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_addr, 0);
		/* EFA :  TS exists - 0, REQUEST(0)/RESPONSE(1) - 0 */
		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_1, 0);
		/* EBS :  TS exists - 0, REQUEST(0)/RESPONSE(1) - 0 */
		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_2, 0);

		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_addr, 1);
		/* EFA :  TS exists - 0, REQUEST(0)/RESPONSE(1) - 1 */
		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_1, 0);
		/* EBS :  TS exists - 0, REQUEST(0)/RESPONSE(1) - 0 */
		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_2, 0);

		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_addr, 2);
		/* EFA :  TS exists - 1, REQUEST(0)/RESPONSE(1) - 0 */
		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_1, 16);
		/* EBS : TS exists - 0, REQUEST(0)/RESPONSE(1) - 0 */
		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_2, 0);

		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_addr, 3);
		/* EFA :  TS exists - 1, REQUEST(0)/RESPONSE(1) - 1 */
		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_1, 16);
		/* EBS : TS exists - 0, REQUEST(0)/RESPONSE(1) - 0 */
		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_2, 0);

		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_addr, 4);
		/* EFA :  TS exists - 0, REQUEST(0)/RESPONSE(1) - 0 */
		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_1, 0);
		/* EBS : TS exists - 0, REQUEST(0)/RESPONSE(1) - 1 */
		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_2, 0);

		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_addr, 5);
		/* EFA :  TS exists - 0, REQUEST(0)/RESPONSE(1) - 1 */
		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_1, 0);
		/* EBS : TS exists - 0, REQUEST(0)/RESPONSE(1) - 1 */
		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_2, 0);

		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_addr, 6);
		/* EFA :  TS exists - 1, REQUEST(0)/RESPONSE(1) - 0 */
		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_1, 16);
		/* EBS : TS exists - 0, REQUEST(0)/RESPONSE(1) - 1 */
		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_2, 0);

		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_addr, 7);
		/* EFA :  TS exists - 1, REQUEST(0)/RESPONSE(1) - 1 */
		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_1, 16);
		/* EBS : TS exists - 0, REQUEST(0)/RESPONSE(1) - 1  */
		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_2, 0);

		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_addr, 8);
		/* EFA :  TS exists - 0, REQUEST(0)/RESPONSE(1) - 0 */
		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_1, 0);
		/* EBS : TS exists - 1, REQUEST(0)/RESPONSE(1) - 0 */
		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_2, 16);

		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_addr, 9);
		/* EFA :  TS exists - 0, REQUEST(0)/RESPONSE(1) - 1 */
		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_1, 0);
		/* EBS : TS exists - 1, REQUEST(0)/RESPONSE(1) - 0 */
		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_2, 16);

		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_addr, 10);
		/* EFA :  TS exists - 1, REQUEST(0)/RESPONSE(1) - 0 */
		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_1, 16);
		/* EBS : TS exists - 1, REQUEST(0)/RESPONSE(1) - 0 */
		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_2, 16);

		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_addr, 11);
		/* EFA :  TS exists - 1, REQUEST(0)/RESPONSE(1) - 1 */
		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_1, 16);
		/* EBS : TS exists - 1, REQUEST(0)/RESPONSE(1) - 0 */
		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_2, 16);

		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_addr, 12);
		/* EFA :  TS exists - 0, REQUEST(0)/RESPONSE(1) - 0 */
		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_1, 0);
		/* EBS : TS exists - 1, REQUEST(0)/RESPONSE(1) - 1 */
		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_2, 16);

		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_addr, 13);
		/* EFA :  TS exists - 0, REQUEST(0)/RESPONSE(1) - 1 */
		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_1, 0);
		/* EBS : TS exists - 1, REQUEST(0)/RESPONSE(1) - 1 */
		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_2, 16);

		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_addr, 14);
		/* EFA :  TS exists - 1, REQUEST(0)/RESPONSE(1) - 0 */
		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_1, 16);
		/* EBS : TS exists - 1, REQUEST(0)/RESPONSE(1) - 1 */
		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_2, 16);

		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_addr, 15);
		/* EFA :  TS exists - 1, REQUEST(0)/RESPONSE(1) - 1 */
		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_1, 16);
		/* EBS : TS exists - 1, REQUEST(0)/RESPONSE(1) - 1 */
		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_2, 16);
	} else {
		struct al_ec_crc __iomem *crc_regs_base = &adapter->ec_regs_base->crc[engine_idx];
		int i;
		/* v4_rx_gcp_alu_const.val_0 should be zero for end to end checksum offset */
		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const, 0x04020100);
		/* substrare encryption exists */
		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_p3, 0x86c);
		/* VPC encryption exists */
		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_p4, 0x86d);
		/* novice footer CRC */
		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_p5, 0x86e);
		/* reserved */
		al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_p6, 0x86e);
		for (i = 0; i < 4; i++) {
			al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_addr, 4*i);
			/* substrare exists - 0, VPC exists - 0 */
			al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_1, 0);
			/* encryption exists */
			al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_2, 0x0);
		}
		for (i = 0; i < 4; i++) {
			al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_addr, 4*i + 1);
			/* substrare exists - 1, VPC exists - 0 */
			al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_1, 28);
			/* encryption exists */
			al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_2, 0x1ff);
		}
		for (i = 0; i < 4; i++) {
			al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_addr, 4*i + 2);
			/* substrare exists - 0, VPC exists - 1 */
			al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_1, 28);
			/* encryption exists */
			al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_2, 0x1ff);
		}
		for (i = 0; i < 4; i++) {
			al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_addr, 4*i + 3);
			/* substrare exists - 0, VPC exists - 1 */
			al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_1, 48);
			/* encryption exists */
			al_reg_write32(&crc_regs_base->v4_rx_gcp_alu_const_table_2, 0x1ff);
		}
	}

	al_eth_rx_generic_crc_table_enable(adapter, AL_TRUE);

	return 0;
}

void al_eth_rx_generic_crc_table_enable(struct al_hal_eth_adapter *adapter, al_bool enable)
{
	uint32_t rx_gcp_legacy_val;
	uint32_t rx_comp_desc_mask;
	uint32_t rx_comp_desc_val;

	al_assert(adapter->rev_id > AL_ETH_REV_ID_2);

	if (adapter->rev_id > AL_ETH_REV_ID_3)
		rx_comp_desc_mask = EC_GEN_V3_RX_COMP_DESC_W3_DEC_STAT_15_CRC_RES_SEL |
			EC_GEN_V3_RX_COMP_DESC_W3_DEC_STAT_14_L3_CKS_RES_SEL |
			EC_GEN_V3_RX_COMP_DESC_W3_DEC_STAT_13_L4_CKS_RES_SEL |
			EC_GEN_V3_RX_COMP_DESC_W0_L3_CKS_RES_SEL;
	else
		rx_comp_desc_mask = EC_GEN_V3_RX_COMP_DESC_W3_DEC_STAT_15_CRC_RES_SEL |
			EC_GEN_V3_RX_COMP_DESC_W3_DEC_STAT_14_L3_CKS_RES_SEL |
			EC_GEN_V3_RX_COMP_DESC_W3_DEC_STAT_13_L4_CKS_RES_SEL |
			EC_GEN_V3_RX_COMP_DESC_W0_L4_CKS_RES_SEL |
			EC_GEN_V3_RX_COMP_DESC_W0_L3_CKS_RES_SEL;

	if (enable) {
		rx_gcp_legacy_val = 0x0;
		rx_comp_desc_val = rx_comp_desc_mask;
	} else {
		rx_gcp_legacy_val = EC_RFW_V3_RX_GCP_LEGACY_PARAM_SEL;
		rx_comp_desc_val = 0x0;
	}

	al_reg_write32(&adapter->ec_regs_base->rfw_v3.rx_gcp_legacy, rx_gcp_legacy_val);
	al_reg_write32_masked(&adapter->ec_regs_base->gen_v3.rx_comp_desc,
			rx_comp_desc_mask,
			rx_comp_desc_val);
}

void al_eth_tx_crce_pub_crc_cks_ins_set(struct al_hal_eth_adapter *adapter,
					   al_bool crc_ins_en,
					   al_bool cks_ins_en)
{
	uint32_t reg = 0;

	al_assert(adapter);
	al_assert(adapter->rev_id < AL_ETH_REV_ID_4);

	if (crc_ins_en)
		reg |= EC_CRCE_GEN_CFG_EN_TX_CRC_INS;
	if (cks_ins_en == AL_FALSE)
		reg |= EC_CRCE_GEN_CFG_EN_TX_CKS_DIS;

	al_reg_write32_masked(&adapter->ec_regs_base->crce.gen_cfg,
			EC_CRCE_GEN_CFG_EN_TX_CRC_INS | EC_CRCE_GEN_CFG_EN_TX_CKS_DIS,
			reg);
}

void al_eth_unit_regs_info_get(void *bars[6],
			       unsigned int dev_id __attribute__((__unused__)),
			       unsigned int rev_id __attribute__((__unused__)),
			       struct al_eth_unit_regs_info *unit_info)
{
	al_assert(unit_info);
	al_assert(bars);
	al_assert(bars[0]);

	al_memset(unit_info, 0, sizeof(*unit_info));

	/* UDMA regs base is always BAR 0 address */
	unit_info->udma_regs_base = bars[0];

	/* MAC regs base is always BAR 2 address */
	if (bars[2])
		unit_info->mac_regs_base = bars[2];

	/* EC regs base is always BAR 4 address */
	if (bars[4])
		unit_info->ec_regs_base = bars[4];
}

static void al_eth_ec_iofic_attrs_primary_get_hw(struct al_hal_eth_adapter *adapter,
						 uint32_t *ec_a_mask,
						 uint32_t *ec_b_mask,
						 uint32_t *ec_c_mask,
						 uint32_t *ec_d_mask)
{
	switch (adapter->rev_id) {
	case AL_ETH_REV_ID_1:
		*ec_a_mask = AL_ETH_EC_IOFIC_GROUP_A_ERROR_INTS;
		*ec_b_mask = AL_ETH_V1_EC_IOFIC_GROUP_B_ERROR_INTS;
		*ec_c_mask = AL_ETH_V1_V2_EC_IOFIC_GROUP_C_ERROR_INTS;
		*ec_d_mask = AL_ETH_V1_EC_IOFIC_GROUP_D_ERROR_INTS;
		break;
	case AL_ETH_REV_ID_2:
		*ec_a_mask = AL_ETH_EC_IOFIC_GROUP_A_ERROR_INTS;
		*ec_b_mask = AL_ETH_V2_V3_EC_IOFIC_GROUP_B_ERROR_INTS;
		*ec_c_mask = AL_ETH_V1_V2_EC_IOFIC_GROUP_C_ERROR_INTS;
		*ec_d_mask = AL_ETH_V2_V3_EC_IOFIC_GROUP_D_ERROR_INTS;
		break;
	case AL_ETH_REV_ID_3:
		*ec_a_mask = AL_ETH_EC_IOFIC_GROUP_A_ERROR_INTS;
		*ec_b_mask = AL_ETH_V2_V3_EC_IOFIC_GROUP_B_ERROR_INTS;
		*ec_c_mask = AL_ETH_V3_EC_IOFIC_GROUP_C_ERROR_INTS;
		*ec_d_mask = AL_ETH_V2_V3_EC_IOFIC_GROUP_D_ERROR_INTS;
		break;
	case AL_ETH_REV_ID_4:
		*ec_a_mask = AL_ETH_V4_EC_IOFIC_GROUP_A_SUM;
		*ec_b_mask = AL_ETH_V4_EC_IOFIC_GROUP_B_SUM;
		*ec_c_mask = AL_ETH_V4_EC_IOFIC_GROUP_C_SUM;
		*ec_d_mask = AL_ETH_V4_EC_IOFIC_GROUP_D_SUM;
		break;
	default:
		*ec_a_mask = 0;
		*ec_b_mask = 0;
		*ec_c_mask = 0;
		*ec_d_mask = 0;

		al_err("%s: unknown eth rev ID: %d\n", __func__, adapter->rev_id);
		al_assert(0);
	}

	if (adapter->rev_id <= AL_ETH_REV_ID_3) {
		uint32_t mask;

		/**
		 * Addressing RMN: 5715, 12004
		 *
		 * RMN description:
		 * HW looks at wrong signals, so we get false error indications
		 *
		 * Software flow:
		 * mask and ignore this interrupts
		 */
		mask = (AL_ETH_EC_IOFIC_GROUP_A_ERR_MSW_IN_SOP_IN_PKT |
			AL_ETH_EC_IOFIC_GROUP_A_ERR_MSW_IN_EOP_IN_IDLE |
			AL_ETH_EC_IOFIC_GROUP_A_ERR_FWD_OUT_EOP_META_EMPTY |
			AL_ETH_EC_IOFIC_GROUP_A_ERR_RXF_OUT_EOP_HDR_EMPTY);
		*ec_a_mask &= ~mask;

		mask = (AL_ETH_EC_IOFIC_GROUP_B_ERR_TMI_IN_SOP_IN_PKT |
			AL_ETH_EC_IOFIC_GROUP_B_ERR_TFW_OUT_EOP_HDR_EMPTY);
		*ec_b_mask &= ~mask;

		/**
		 * Addressing RMN: 11018
		 *
		 * RMN description:
		 * un-initialized TSO SRAM gives false parity interrupt
		 *
		 * Software flow:
		 * Dont unmask TSO interrupt
		 */
		mask = AL_ETH_EC_IOFIC_GROUP_C_TSO_META_CACHE_1_4_PARITY;
		*ec_c_mask &= ~mask;
	}

	if (adapter->rev_id == AL_ETH_REV_ID_2) {
		/**
		 * Addressing RMN: 12004
		 *
		 * RMN description:
		 * ERR_RXF_OUT_EOP_HDR_EMPTY - HW checks wrong combination of
		 *                             signals
		 * ERR_FAF_1_IN_EOP_IN_IDLE  - The actual signals checked refer
		 *                             to a long RX packet, and not EOP
		 *                             while in Idle.
		 *
		 * Software flow:
		 * Mask and ignore these interrupts
		 */
		*ec_a_mask &= ~(AL_ETH_EC_IOFIC_GROUP_A_ERR_RXF_OUT_EOP_HDR_EMPTY |
				AL_ETH_EC_IOFIC_GROUP_A_ERR_FAF_1_IN_EOP_IN_IDLE);
	}

	if (adapter->rev_id == AL_ETH_REV_ID_4) {
		/**
		 * Addressing RMN 10760
		 *
		 * RMN description :
		 * Several issues were discovered when eth error_causes were unmasked.
		 * Detailed description in the RMN
		 *
		 * SW W/A :
		 * Don't unmask these errors.
		 */
		*ec_a_mask &= ~AL_ETH_EC_IOFIC_GROUP_A_RMN_10760_INTS;
		*ec_b_mask &= ~AL_ETH_EC_IOFIC_GROUP_B_RMN_10760_INTS;
	}
}

static void al_eth_ec_iofic_attrs_primary_get_runtime_inval(struct al_hal_eth_adapter *adapter,
							    uint32_t *ec_a_mask,
							    uint32_t *ec_b_mask,
							    uint32_t *ec_c_mask,
							    uint32_t *ec_d_mask)
{
	*ec_a_mask = 0;
	*ec_b_mask = 0;
	*ec_c_mask = 0;
	*ec_d_mask = 0;

	if (adapter->rev_id >= AL_ETH_REV_ID_4) {
		/** Parser is subunit of port 0 */
		if (adapter->serdes_lane != 0)
			*ec_a_mask |= AL_ETH_V4_EC_IOFIC_GROUP_A_RMP_INT;
	}

	/**
	 * In case internal switching is disabled, invalidate loopback
	 * FIFO errors.
	 * No need to check revision - RMN 5714 and 11499 refer to false
	 * positive parity errors triggered during FIFOs initialization,
	 * and are revision dependent; here we want to ignore these
	 * bits altogether
	 */
	if (!adapter->internal_switching) {
		*ec_c_mask |= (AL_ETH_EC_IOFIC_GROUP_C_LB_INFO_FIFO_ERRORS |
			       AL_ETH_EC_IOFIC_GROUP_C_LB_DATA_FIFO_ERRORS);
	}
}

static void al_eth_ec_iofic_attrs_primary_get_transient(struct al_hal_eth_adapter *adapter,
							uint32_t *ec_a_mask,
							uint32_t *ec_b_mask,
							uint32_t *ec_c_mask,
							uint32_t *ec_d_mask)
{
	*ec_a_mask = 0;
	*ec_b_mask = 0;
	*ec_c_mask = 0;
	*ec_d_mask = 0;

	if (adapter->original_rev_id <= AL_ETH_REV_ID_3) {
		/**
		 * Addressing RMN: 5714, 12004
		 *
		 * RMN description:
		 * un-initialized FIFO memories give parity errors.
		 *
		 * Software flow:
		 * Unmask the interrupts only after X packets are received
		 */
		*ec_c_mask |= (AL_ETH_EC_IOFIC_GROUP_C_DATA_FIFO_ERRORS |
			       AL_ETH_EC_IOFIC_GROUP_C_INFO_FIFO_ERRORS |
			       AL_ETH_EC_IOFIC_GROUP_C_LB_DATA_FIFO_ERRORS |
			       AL_ETH_EC_IOFIC_GROUP_C_LB_INFO_FIFO_ERRORS);
	} else if ((adapter->original_rev_id == AL_ETH_REV_ID_4) &&
		(adapter->dev_id == AL_ETH_DEV_ID_STANDARD)) {
		/**
		 * Addressing RMN: 11499
		 *
		 * RMN description:
		 * tx/rx_data_fifo memory in eth1x_top_small give parity errors.
		 *
		 * Software flow:
		 * Unmask the interrupts only after X packets are received
		 */
		*ec_c_mask |= (AL_ETH_EC_IOFIC_GROUP_C_DATA_FIFO_ERRORS |
			       AL_ETH_EC_IOFIC_GROUP_C_LB_DATA_FIFO_ERRORS);
	}
}

static void al_eth_ec_iofic_attrs_primary_get_non_fatal(struct al_hal_eth_adapter *adapter
							__attribute__ ((__unused__)),
							uint32_t *ec_a_mask,
							uint32_t *ec_b_mask,
							uint32_t *ec_c_mask,
							uint32_t *ec_d_mask)
{
	*ec_a_mask = 0;
	*ec_b_mask = 0;
	*ec_c_mask = 0;
	*ec_d_mask = 0;
}

static void al_eth_ec_iofic_attrs_primary_get(struct al_hal_eth_adapter *adapter,
					      int flags,
					      uint32_t *ec_a_mask,
					      uint32_t *ec_b_mask,
					      uint32_t *ec_c_mask,
					      uint32_t *ec_d_mask)
{
	uint32_t a_mask, b_mask, c_mask, d_mask;

	al_eth_ec_iofic_attrs_primary_get_hw(adapter, ec_a_mask, ec_b_mask,
					     ec_c_mask, ec_d_mask);

	if (flags & AL_ETH_IOFIC_ATTRS_RUNTIME_CONFIG) {
		al_eth_ec_iofic_attrs_primary_get_runtime_inval(adapter, &a_mask,
								&b_mask, &c_mask,
								&d_mask);
		*ec_a_mask &= ~a_mask;
		*ec_b_mask &= ~b_mask;
		*ec_c_mask &= ~c_mask;
		*ec_d_mask &= ~d_mask;
	}

	if (flags & AL_ETH_IOFIC_ATTRS_FILTER_TRANSIENT) {
		al_eth_ec_iofic_attrs_primary_get_transient(adapter, &a_mask,
							    &b_mask, &c_mask,
							    &d_mask);
		*ec_a_mask &= ~a_mask;
		*ec_b_mask &= ~b_mask;
		*ec_c_mask &= ~c_mask;
		*ec_d_mask &= ~d_mask;
	}

	if (flags & AL_ETH_IOFIC_ATTRS_FATAL) {
		al_eth_ec_iofic_attrs_primary_get_non_fatal(adapter, &a_mask,
							    &b_mask, &c_mask,
							    &d_mask);
		*ec_a_mask &= ~a_mask;
		*ec_b_mask &= ~b_mask;
		*ec_c_mask &= ~c_mask;
		*ec_d_mask &= ~d_mask;
	}
}

static void al_eth_ec_iofic_attrs_secondary_get(struct al_hal_eth_adapter *adapter,
						int flags __attribute__ ((__unused__)),
						uint32_t *ec_a_mask,
						uint32_t *ec_b_mask,
						uint32_t *ec_c_mask,
						uint32_t *ec_d_mask)
{
	al_assert(adapter->rev_id >= AL_ETH_REV_ID_4);

	if (adapter->serdes_lane == 0) {
		*ec_a_mask =
			AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_SUM_RX |
			AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_SUM_TX;
	} else {
		*ec_a_mask = AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_SUM_TX;
	}

	*ec_b_mask = 0;
	*ec_c_mask = 0;
	*ec_d_mask = 0;
}

int al_eth_ec_iofic_attrs_get(struct al_hal_eth_adapter *adapter,
			      enum al_eth_iofic_level level, int flags,
			      uint32_t *ec_a_mask, uint32_t *ec_b_mask,
			      uint32_t *ec_c_mask, uint32_t *ec_d_mask)
{
	al_assert(ec_a_mask);
	al_assert(ec_b_mask);
	al_assert(ec_c_mask);
	al_assert(ec_d_mask);

	if (level == AL_ETH_IOFIC_LEVEL_PRIMARY)
		al_eth_ec_iofic_attrs_primary_get(adapter, flags,
						  ec_a_mask, ec_b_mask,
						  ec_c_mask, ec_d_mask);
	else
		al_eth_ec_iofic_attrs_secondary_get(adapter, flags,
						    ec_a_mask, ec_b_mask,
						    ec_c_mask, ec_d_mask);

	return 0;
}

/*
 * Data FIFO depth is 640 entries (16B each)
 * - Minimal packet size (64B) will use (64/16) == 4 entries
 * - FIFO will be filled (worst case) after (640/4) == 160 frames
 * Info FIFO depth is 128 entries (1 per frame)
 * 160 frames will guarantee all FIFOs are initialized
 */
#define AL_ETH_V1_V2_FIFO_MIN_FRAMES		160
/*
 * Data FIFO depth is 2560 entries (16B each)
 * - Minimal packet size (64B) will use (64/16) == 4 entries
 * - FIFO will be filled (worst case) after (2560/4) == 640 frames
 * Info FIFO depth is 640 entries (1 per frame)
 */
#define AL_ETH_V3_FIFO_MIN_FRAMES		640

#define AL_ETH_FIFO_MIN_FRAMES(rev_id) \
	(((rev_id) == AL_ETH_REV_ID_3) ? \
	AL_ETH_V3_FIFO_MIN_FRAMES : AL_ETH_V1_V2_FIFO_MIN_FRAMES)

/*
 * Check if FIFOs are fully initialized (i.e. don't raise parity errors due to
 * uninitialized data), clear cause bits and (if requested) configure and unmask
 * IOFICs
 */
static int al_eth_error_ints_setup(struct al_hal_eth_adapter *adapter,
				   al_bool check_only)
{
	struct al_eth_mac_stats stats;
	uint32_t ec_c_mask;

	al_assert(adapter);
	al_assert(adapter->mac_regs_base);

	if (adapter->original_rev_id <= AL_ETH_REV_ID_3) {
		ec_c_mask = (AL_ETH_EC_IOFIC_GROUP_C_DATA_FIFO_ERRORS |
			     AL_ETH_EC_IOFIC_GROUP_C_INFO_FIFO_ERRORS);
	} else if ((adapter->original_rev_id == AL_ETH_REV_ID_4) &&
		(adapter->dev_id == AL_ETH_DEV_ID_STANDARD)) {
		ec_c_mask = (AL_ETH_EC_IOFIC_GROUP_C_DATA_FIFO_ERRORS);
	} else {
		return 0;
	}

	if (!(al_iofic_read_mask(adapter->ec_ints_base, AL_INT_GROUP_C) & ec_c_mask))
		return 0;

	al_eth_mac_stats_get(adapter, &stats);

	/*
	 * Check if Tx and Rx data FIFO's were fully initialized.
	 */
	if ((stats.frames_rxed_ok < AL_ETH_FIFO_MIN_FRAMES(adapter->rev_id)) ||
	    (stats.frames_txed_ok < AL_ETH_FIFO_MIN_FRAMES(adapter->rev_id)))
		return -EAGAIN;

	/* clear old non valid parity interrupts */
	al_iofic_clear_cause(adapter->ec_ints_base,
			     AL_INT_GROUP_C,
			     ec_c_mask);

	if (!check_only) {
		/* unmask FIFO parity interrupts */
		al_iofic_unmask(adapter->ec_ints_base,
				AL_INT_GROUP_C,
				ec_c_mask);
	}

	al_eth_mac_error_ints_setup(adapter, check_only);

	al_dbg("%s: unmasked eth error interrupts\n", adapter->name);

	return 0;
}

al_bool al_eth_err_polling_is_avail(struct al_hal_eth_adapter *adapter)
{
	return al_eth_error_ints_setup(adapter, AL_TRUE) == 0;
}

int al_eth_error_ints_unmask(struct al_hal_eth_adapter *adapter)
{
	return al_eth_error_ints_setup(adapter, AL_FALSE);
}

int al_eth_mailbox_link_status_get(struct al_hal_eth_adapter *adapter)
{
	uint32_t msg_in;
	struct al_udma *udma = &adapter->tx_udma;

	al_udma_mailbox_read(udma, AL_ETH_LINK_STATUS_SOURCE_UDMA, &msg_in);

	/* Check for valid */
	if ((msg_in & AL_ETH_LINK_STATUS_MESSAGE_VALID_BIT) == 0)
		return -EAGAIN;

	return (msg_in & AL_ETH_LINK_STATUS_MESSAGE_DATA_BIT)
			>> AL_ETH_LINK_STATUS_MESSAGE_DATA_SHIFT;
}

void al_eth_mailbox_link_status_set(struct al_hal_eth_adapter *adapter,
				    uint8_t dst_udma_id, al_bool status)
{
	uint32_t msg = AL_ETH_LINK_STATUS_MESSAGE_VALID_BIT;
	struct al_udma *udma = &adapter->tx_udma;

	msg |= (status << AL_ETH_LINK_STATUS_MESSAGE_DATA_SHIFT);

	al_udma_mailbox_write(udma, dst_udma_id, msg);
}

/** @} end of Ethernet group */
