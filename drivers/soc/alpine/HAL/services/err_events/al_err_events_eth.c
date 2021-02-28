/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#include "al_err_events_internal.h"
#include "al_err_events_eth.h"
#include "al_hal_eth.h"
#include "al_hal_udma_iofic.h"

/*******************************************************************************
 ** Static error fields
 ******************************************************************************/
static const struct al_err_events_field
	al_err_events_eth_ec_fields[AL_ERR_EVENTS_ETH_EC_MAX_ERRORS] = {
	{
		.name = "Multi Stream Start In Packet Error",
		.id = AL_ERR_EVENTS_ETH_EC_MS_START_IN_PACKET_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_A_ERR_MSW_IN_SOP_IN_PKT,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Multi Stream End On Non Packet Error",
		.id = AL_ERR_EVENTS_ETH_EC_MS_PACKET_START_OF_PACKET_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_A_ERR_MSW_IN_EOP_IN_IDLE,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RFW No Data On End Of Packet (Meta) Error",
		.id = AL_ERR_EVENTS_ETH_EC_RFW_NO_DATA_ON_END_OF_PACKET_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_A_ERR_FWD_OUT_EOP_META_EMPTY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RFW Start In Packet Error",
		.id = AL_ERR_EVENTS_ETH_EC_RFW_START_IN_PACKET_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_A_ERR_FWD_IN_SOP_IN_PKT,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RFW End On Non Packet Error",
		.id = AL_ERR_EVENTS_ETH_EC_RFW_END_ON_NON_PACKET_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_A_ERR_FWD_IN_EOP_IN_IDLE,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RPE3 No Data On End Of Packet Error",
		.id = AL_ERR_EVENTS_ETH_EC_RPE3_NO_DATA_ON_END_OF_PACKET_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_A_ERR_RPE_3_OUT_EOP_HDR_EMPTY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RPE3 Start In Packet Error",
		.id = AL_ERR_EVENTS_ETH_EC_RPE3_START_IN_PACKET_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_A_ERR_RPE_3_IN_SOP_IN_PKT,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RPE3 End On Non Packet Error",
		.id = AL_ERR_EVENTS_ETH_EC_RPE3_END_ON_NON_PACKET_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_A_ERR_RPE_3_IN_EOP_IN_IDLE,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RPE2 No Data On End Of Packet Error",
		.id = AL_ERR_EVENTS_ETH_EC_RPE2_NO_DATA_ON_END_OF_PACKET_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_A_ERR_RPE_2_OUT_EOP_HDR_EMPTY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RPE2 Start In Packet Error",
		.id = AL_ERR_EVENTS_ETH_EC_RPE2_START_IN_PACKET_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_A_ERR_RPE_2_IN_SOP_IN_PKT,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RPE2 End On Non Packet Error",
		.id = AL_ERR_EVENTS_ETH_EC_RPE2_END_ON_NON_PACKET_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_A_ERR_RPE_2_IN_EOP_IN_IDLE,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RPE1 No Data On End Of Packet Error",
		.id = AL_ERR_EVENTS_ETH_EC_RPE1_NO_DATA_ON_END_OF_PACKET_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_A_ERR_RPE_1_OUT_EOP_HDR_EMPTY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RPE1 Start In Packet Error",
		.id = AL_ERR_EVENTS_ETH_EC_RPE1_START_IN_PACKET_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_A_ERR_RPE_1_IN_SOP_IN_PKT,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RPE1 End On Non Packet Error",
		.id = AL_ERR_EVENTS_ETH_EC_RPE1_END_ON_NON_PACKET_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_A_ERR_RPE_1_IN_EOP_IN_IDLE,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RXF Wire Out No Data On End Of Packet Error",
		.id = AL_ERR_EVENTS_ETH_EC_RXF_NO_DATA_ON_END_OF_PACKET_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_A_ERR_RXF_OUT_EOP_HDR_EMPTY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RXF LB Out Start In Packet Error",
		.id = AL_ERR_EVENTS_ETH_EC_RXF_LB_START_IN_PACKET_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_A_ERR_LBF_IN_SOP_IN_PKT,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RXF LB Out Packet Error End On Non Packet",
		.id = AL_ERR_EVENTS_ETH_EC_RXF_LB_PACKT_END_ON_NON_PACKET_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_A_ERR_LBF_IN_EOP_IN_IDLE,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RXF Wire Out Start In Packet Error",
		.id = AL_ERR_EVENTS_ETH_EC_RXF_START_IN_PACKET_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_A_ERR_RXF_IN_SOP_IN_PKT,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RXF Wire Out End On Non Packet Error",
		.id = AL_ERR_EVENTS_ETH_EC_RXF_END_ON_NON_PACKET_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_A_ERR_RXF_IN_EOP_IN_IDLE,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Wire No Data On End Of Packet Error",
		.id = AL_ERR_EVENTS_ETH_EC_WIRE_NON_DATA_ON_END_OF_PACKET_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_A_ERR_FAF_2_OUT_EOP_HDR_EMPTY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Wire Start In Packet Error",
		.id = AL_ERR_EVENTS_ETH_EC_WIRE_START_IN_PACKET_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_A_ERR_FAF_2_IN_SOP_IN_PKT,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Wire End On Non Packet Error",
		.id = AL_ERR_EVENTS_ETH_EC_WIRE_END_ON_NON_PACKET_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_A_ERR_FAF_2_IN_EOP_IN_IDLE,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "LB No Data On End Of Packet Error",
		.id = AL_ERR_EVENTS_ETH_EC_LB_NO_DATA_ON_END_OF_PACKET_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_A_ERR_FAF_1_OUT_EOP_HDR_EMPTY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "LB Start In Packet Error",
		.id = AL_ERR_EVENTS_ETH_EC_LB_START_IN_PACKET_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_A_ERR_FAF_1_IN_SOP_IN_PKT,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "LB End On Non Packet Error",
		.id = AL_ERR_EVENTS_ETH_EC_LB_END_ON_NON_PACKET_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_A_ERR_FAF_1_IN_EOP_IN_IDLE,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TMI Start In Packet Error",
		.id = AL_ERR_EVENTS_ETH_EC_TMI_START_IN_PACKET_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_B_ERR_TMI_IN_SOP_IN_PKT,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TMI End On Non Packet Error",
		.id = AL_ERR_EVENTS_ETH_EC_TMI_END_ON_NON_PACKET_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_B_ERR_TMI_IN_EOP_IN_IDLE,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TFW No Data On End Of Packet Error",
		.id = AL_ERR_EVENTS_ETH_EC_TFW_NO_DATA_ON_END_OF_PACKET_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_B_ERR_TFW_OUT_EOP_HDR_EMPTY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TFW Start In Packet Error",
		.id = AL_ERR_EVENTS_ETH_EC_TFW_START_IN_PACKET_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_B_ERR_TFW_IN_SOP_IN_PKT,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TFW End On Non Packet Error",
		.id = AL_ERR_EVENTS_ETH_EC_TFW_END_ON_NON_PACKET_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_B_ERR_TFW_IN_EOP_IN_IDLE,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TPM No SOP Error",
		.id = AL_ERR_EVENTS_ETH_EC_TPM_NO_SOP_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_B_ERR_TPM_NO_SOP,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TPM Spoof 0 Error",
		.id = AL_ERR_EVENTS_ETH_EC_TPM_SPOOF_0_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_B_ERR_TPM_SPOOF_0,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TPM Spoof 1 Error",
		.id = AL_ERR_EVENTS_ETH_EC_TPM_SPOOF_1_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_B_ERR_TPM_SPOOF_1,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TPM Spoof 2 Error",
		.id = AL_ERR_EVENTS_ETH_EC_TPM_SPOOF_2_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_B_ERR_TPM_SPOOF_2,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TPM Spoof 3 Error",
		.id = AL_ERR_EVENTS_ETH_EC_TPM_SPOOF_3_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_B_ERR_TPM_SPOOF_3,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TPE No Data On End Of Packet Error",
		.id = AL_ERR_EVENTS_ETH_EC_TPE_NO_DATA_ON_END_OF_PACKET_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_B_ERR_TPE_OUT_EOP_HDR_EMPTY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TPE Start In Packet Error",
		.id = AL_ERR_EVENTS_ETH_EC_TPE_START_IN_PACKET_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_B_ERR_TPE_IN_SOP_IN_PKT,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TPE End On Non Packet Error",
		.id = AL_ERR_EVENTS_ETH_EC_TPE_END_ON_NON_PACKET_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_B_ERR_TPE_IN_EOP_IN_IDLE,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TSO Count End of Sequence Error",
		.id = AL_ERR_EVENTS_ETH_EC_TSO_COUNT_END_OF_SEQ_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_B_ERR_TSO_CNT_END_OF_SEQ,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TSO Last In SHDR Error",
		.id = AL_ERR_EVENTS_ETH_EC_TSO_LAST_IN_SHDR_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_B_ERR_TSO_LAST_IN_SHDR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TSO Count End Of Packet 4 Error",
		.id = AL_ERR_EVENTS_ETH_EC_TSO_COUNT_END_OF_PACKET_4_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_B_ERR_TSO_CNT_END_OF_PKT_4,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TSO Count End Of Packet 3 Error",
		.id = AL_ERR_EVENTS_ETH_EC_TSO_COUNT_END_OF_PACKET_3_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_B_ERR_TSO_CNT_END_OF_PKT_3,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TSO Count End Of Packet 2 Error",
		.id = AL_ERR_EVENTS_ETH_EC_TSO_COUNT_END_OF_PACKET_2_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_B_ERR_TSO_CNT_END_OF_PKT_2,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TSO Count End Of Packet 1 Error",
		.id = AL_ERR_EVENTS_ETH_EC_TSO_COUNT_END_OF_PACKET_1_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_B_ERR_TSO_CNT_END_OF_PKT_1,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TSO No SOP Error",
		.id = AL_ERR_EVENTS_ETH_EC_TSO_NO_SOP_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_B_ERR_TSO_NO_SOP,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX SAD 3 Parity Error",
		.id = AL_ERR_EVENTS_ETH_EC_RX_SAD_3_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_B_ERR_RX_SAD_3_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX SAD 2 Parity Error",
		.id = AL_ERR_EVENTS_ETH_EC_RX_SAD_2_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_B_ERR_RX_SAD_2_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX SAD 1 Parity Error",
		.id = AL_ERR_EVENTS_ETH_EC_RX_SAD_1_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_B_ERR_RX_SAD_1_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX SAD 0 Parity Error",
		.id = AL_ERR_EVENTS_ETH_EC_RX_SAD_0_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_B_ERR_RX_SAD_0_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TX SAD 3 Parity Error",
		.id = AL_ERR_EVENTS_ETH_EC_TX_SAD_3_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_B_ERR_TX_SAD_3_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TX SAD 2 Parity Error",
		.id = AL_ERR_EVENTS_ETH_EC_TX_SAD_2_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_B_ERR_TX_SAD_2_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TX SAD 1 Parity Error",
		.id = AL_ERR_EVENTS_ETH_EC_TX_SAD_1_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_B_ERR_TX_SAD_1_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TX SAD 0 Parity Error",
		.id = AL_ERR_EVENTS_ETH_EC_TX_SAD_0_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_B_ERR_TX_SAD_0_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TFW FIFO 1 EOP SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_EC_TFW_FIFO_1_EOP_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_C_TFW_EOP_FIFO_1_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TFW FIFO 1 SOP SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_EC_TFW_FIFO_1_SOP_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_C_TFW_SOP_FIFO_1_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX FIFO 2 EOP SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_EC_RX_FIFO_2_EOP_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_C_RXF_EOP_FIFO_2_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX FIFO 2 SOP SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_EC_RX_FIFO_2_SOP_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_C_RXF_SOP_FIFO_2_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX FIFO 1 EOP SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_EC_RX_FIFO_1_EOP_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_C_RXF_EOP_FIFO_1_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX FIFO 1 SOP SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_EC_RX_FIFO_1_SOP_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_C_RXF_SOP_FIFO_1_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TXF VLAN Table 1 SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_EC_TXF_VLAN_TABLE_1_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_C_TFW_VLAN_TABLE_1_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RFW Ctrl Table 1 SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_EC_RFW_CTRL_TABLE_1_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_C_RFW_CTRL_TABLE_1_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RFW VLAN Table 1 SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_EC_RFW_VLAN_TABLE_1_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_C_RFW_VLAN_TABLE_1_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RFW Tuple hash Table 1 SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_EC_RFW_THASH_TABLE_1_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_C_RFW_THASH_TABLE_1_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RFW MAC hash Table 1 SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_EC_RFW_MHASH_TABLE_1_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_C_RFW_MHASH_TABLE_1_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TFW GCP Table 2 SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_EC_TFW_GCP_TABLE_2_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_V3_EC_IOFIC_GROUP_C_TFW_GCP_TABLE_2_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TFW GCP Table 1 SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_EC_TFW_GCP_TABLE_1_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_V3_EC_IOFIC_GROUP_C_TFW_GCP_TABLE_1_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TFW GCP Table 0 SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_EC_TFW_GCP_TABLE_0_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_V3_EC_IOFIC_GROUP_C_TFW_GCP_TABLE_0_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RFW GCP Table 2 SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_EC_RFW_GCP_TABLE_2_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_V3_EC_IOFIC_GROUP_C_RFW_GCP_TABLE_2_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RFW GCP Table 1 SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_EC_RFW_GCP_TABLE_1_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_V3_EC_IOFIC_GROUP_C_RFW_GCP_TABLE_1_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RFW GCP Table 0 SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_EC_RFW_GCP_TABLE_0_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_V3_EC_IOFIC_GROUP_C_RFW_GCP_TABLE_0_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RXF Flow FSM Table SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_EC_RFW_FLOW_FSM_TABLE_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_C_RFW_FLOW_TABLE_1_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TX Data FIFO 2 SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_EC_TX_DATA_FIFO_2_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_C_TFW_DATA_FIFO_2_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TX Data FIFO 1 SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_EC_TX_DATA_FIFO_1_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_C_TFW_DATA_FIFO_1_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX Data FIFO 2_2 SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_EC_RX_DATA_FIFO_2_2_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_C_RXF_DATA_FIFO_2_2_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX Data FIFO 2_1 SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_EC_RX_DATA_FIFO_2_1_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_C_RXF_DATA_FIFO_2_1_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX Data FIFO 1_2 SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_EC_RX_DATA_FIFO_1_2_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_C_RXF_DATA_FIFO_1_2_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX Data FIFO 1_1 SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_EC_RX_DATA_FIFO_1_1_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_C_RXF_DATA_FIFO_1_1_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TSO Meta Cache Table 1_4 SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_EC_TSO_META_CACHE_1_4_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_C_TSO_META_CACHE_1_4_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TSO Meta Cache Table 1_3 SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_EC_TSO_META_CACHE_1_3_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_C_TSO_META_CACHE_1_3_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TSO Meta Cache Table 1_2 SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_EC_TSO_META_CACHE_1_2_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_C_TSO_META_CACHE_1_2_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TSO Meta Cache Table 1_1 SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_EC_TSO_META_CACHE_1_1_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_C_TSO_META_CACHE_1_1_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TimeStamp FIFO Overrun Error",
		.id = AL_ERR_EVENTS_ETH_EC_TIMESTAMP_FIFO_OVERRUN_ERROR,
		.iofic_group = AL_INT_GROUP_D,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_D_ERR_TIMESTAMP_FIFO_OVERRUN,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "ROCE Write Error",
		.id = AL_ERR_EVENTS_ETH_EC_ROCE_WRITE_ERROR,
		.iofic_group = AL_INT_GROUP_D,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_D_ERR_ROCE_WR_INT,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "ROCE Read Error",
		.id = AL_ERR_EVENTS_ETH_EC_ROCE_READ_ERROR,
		.iofic_group = AL_INT_GROUP_D,
		.iofic_bit = AL_ETH_EC_IOFIC_GROUP_D_ERR_ROCE_RR_INT,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_err_events_common_field_iofic_collect,
		.print = al_err_events_common_field_print,
		.mask = al_err_events_common_field_iofic_mask,
	},
};

static const struct al_err_events_field
	al_err_events_eth_mac_fields[AL_ERR_EVENTS_ETH_MAC_MAX_ERRORS] = {
	{
		.name = "SERDES 3 Tx FIFO Error",
		.id = AL_ERR_EVENTS_ETH_MAC_SERDES_3_TX_FIFO_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_MAC_IOFIC_GROUP_A_SERDES_3_TX_FIFO_ERR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "SERDES 2 Tx FIFO Error",
		.id = AL_ERR_EVENTS_ETH_MAC_SERDES_2_TX_FIFO_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_MAC_IOFIC_GROUP_A_SERDES_2_TX_FIFO_ERR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "SERDES 1 Tx FIFO Error",
		.id = AL_ERR_EVENTS_ETH_MAC_SERDES_1_TX_FIFO_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_MAC_IOFIC_GROUP_A_SERDES_1_TX_FIFO_ERR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "SERDES 0 Tx FIFO Error",
		.id = AL_ERR_EVENTS_ETH_MAC_SERDES_0_TX_FIFO_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_MAC_IOFIC_GROUP_A_SERDES_0_TX_FIFO_ERR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "SERDES 3 Tx FIFO Full Error",
		.id = AL_ERR_EVENTS_ETH_MAC_SERDES_3_TX_FIFO_FULL_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_MAC_IOFIC_GROUP_A_SERDES_3_TX_FIFO_FULL,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "SERDES 2 Tx FIFO Full Error",
		.id = AL_ERR_EVENTS_ETH_MAC_SERDES_2_TX_FIFO_FULL_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_MAC_IOFIC_GROUP_A_SERDES_2_TX_FIFO_FULL,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "SERDES 1 Tx FIFO Full Error",
		.id = AL_ERR_EVENTS_ETH_MAC_SERDES_1_TX_FIFO_FULL_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_MAC_IOFIC_GROUP_A_SERDES_1_TX_FIFO_FULL,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "SERDES 0 Tx FIFO Full Error",
		.id = AL_ERR_EVENTS_ETH_MAC_SERDES_0_TX_FIFO_FULL_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_MAC_IOFIC_GROUP_A_SERDES_0_TX_FIFO_FULL,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "XGMII Data FIFO 64_32 Error",
		.id = AL_ERR_EVENTS_ETH_MAC_XGMII_DATA_FIFO_64_32_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V1_V2_MAC_IOFIC_GROUP_A_XGMII_DFIFO_64_32_ERR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "XGMII Data FIFO 64_32 Full",
		.id = AL_ERR_EVENTS_ETH_MAC_XGMII_DATA_FIFO_64_32_FULL_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V1_V2_MAC_IOFIC_GROUP_A_XGMII_DFIFO_64_32_FULL,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "XGMII Data FIFO 32_64 Error",
		.id = AL_ERR_EVENTS_ETH_MAC_XGMII_DATA_FIFO_32_64_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V1_V2_MAC_IOFIC_GROUP_A_XGMII_DFIFO_32_64_ERR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "XGMII Data FIFO 32_64 Full Error",
		.id = AL_ERR_EVENTS_ETH_MAC_XGMII_DATA_FIFO_32_64_FULL_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V1_V2_MAC_IOFIC_GROUP_A_XGMII_DFIFO_32_64_FULL,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "1G TX RTSM data FIFO SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_MAC_1G_TX_RTSM_DATA_FIFO_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_MAC_IOFIC_GROUP_B_1G_TX_RTSM_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "1G RX Data FIFO SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_MAC_1G_RX_DATA_FIFO_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_MAC_IOFIC_GROUP_B_1G_RX_FIFO_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "1G TX Data FIFO SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_MAC_1G_TX_DATA_FIFO_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_V2_V3_MAC_IOFIC_GROUP_B_1G_TX_FIFO_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "10G TX Stats SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_MAC_10G_TX_STATS_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_V3_MAC_IOFIC_GROUP_B_10G_LL_STATS_TX_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "10G RX Stats SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_MAC_10G_RX_STATS_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_V3_MAC_IOFIC_GROUP_B_10G_LL_STATS_RX_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "40G TX Stats SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_MAC_40G_TX_STATS_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_V3_MAC_IOFIC_GROUP_B_40G_LL_STATS_TX_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "40G RX Stats SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_MAC_40G_RX_STATS_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_V3_MAC_IOFIC_GROUP_B_40G_LL_STATS_RX_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "10G Rx Data FIFO SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_MAC_10G_RX_DATA_FIFO_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_V1_V2_MAC_IOFIC_GROUP_B_10G_RX_DATA_FIFO_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "10G Rx Ctrl FIFO SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_MAC_10G_RX_CTRL_FIFO_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_V1_V2_MAC_IOFIC_GROUP_B_10G_RX_CTRL_FIFO_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "10G Tx Data FIFO SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_MAC_10G_TX_DATA_FIFO_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_V2_MAC_IOFIC_GROUP_B_10G_TX_DATA_FIFO_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "10G Tx Ctrl FIFO SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_MAC_10G_TX_CTRL_FIFO_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_V2_MAC_IOFIC_GROUP_B_10G_TX_CTRL_FIFO_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "10G PCS Memory SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_MAC_10G_PCS_MEM_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_MAC_IOFIC_GROUP_C_10G_PCS_MEM_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "10G PCS LoopBack SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_MAC_10G_PCS_LB_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_MAC_IOFIC_GROUP_C_10G_PCS_LOOPBACK_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "10G PCS Delay RAM SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_MAC_10G_PCS_DELAY_RAM_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_MAC_IOFIC_GROUP_C_10G_PCS_DELAY_RAM_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "40G PCS FEC91 Delay RAM 1 Parity Error",
		.id = AL_ERR_EVENTS_ETH_MAC_40G_PCS_FEC91_DELAY_RAM_1_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_D,
		.iofic_bit = AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_DELAY_1_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "40G PCS FEC91 Delay RAM 0 Parity Error",
		.id = AL_ERR_EVENTS_ETH_MAC_40G_PCS_FEC91_DELAY_RAM_0_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_D,
		.iofic_bit = AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_DELAY_0_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "40G PCS FEC91 RAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_MAC_40G_PCS_FEC91_RAM_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_D,
		.iofic_bit = AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_FEC91_RAM_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "40G PCS Delay RAM 3 Parity Error",
		.id = AL_ERR_EVENTS_ETH_MAC_40G_PCS_DELAY_RAM_3_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_D,
		.iofic_bit = AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_DELAY_RAM_3_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "40G PCS Delay RAM 2 Parity Error",
		.id = AL_ERR_EVENTS_ETH_MAC_40G_PCS_DELAY_RAM_2_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_D,
		.iofic_bit = AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_DELAY_RAM_2_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "40G PCS Delay RAM 1 Parity Error",
		.id = AL_ERR_EVENTS_ETH_MAC_40G_PCS_DELAY_RAM_1_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_D,
		.iofic_bit = AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_DELAY_RAM_1_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "40G PCS Delay RAM 0 Parity Error",
		.id = AL_ERR_EVENTS_ETH_MAC_40G_PCS_DELAY_RAM_0_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_D,
		.iofic_bit = AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_DELAY_RAM_0_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "40G PCS Memory 3 SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_MAC_40G_PCS_MEM_3_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_D,
		.iofic_bit = AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_MEM_3_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "40G PCS Memory 2 SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_MAC_40G_PCS_MEM_2_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_D,
		.iofic_bit = AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_MEM_2_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "40G PCS Memory 1 SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_MAC_40G_PCS_MEM_1_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_D,
		.iofic_bit = AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_MEM_1_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "40G PCS Memory 0 SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_MAC_40G_PCS_MEM_0_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_D,
		.iofic_bit = AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_MEM_0_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "40G PCS Desk 3 SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_MAC_40G_PVS_DESK_3_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_D,
		.iofic_bit = AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_DESK_3_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "40G PCS Desk 2 SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_MAC_40G_PVS_DESK_2_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_D,
		.iofic_bit = AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_DESK_2_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "40G PCS Desk 1 SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_MAC_40G_PVS_DESK_1_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_D,
		.iofic_bit = AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_DESK_1_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "40G PCS Desk 0 SRAM Parity Error",
		.id = AL_ERR_EVENTS_ETH_MAC_40G_PVS_DESK_0_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_D,
		.iofic_bit = AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_DESK_0_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
};

static const struct al_err_events_field
	al_err_events_eth_v4_ec_fields[AL_ERR_EVENTS_ETH_V4_EC_MAX_ERRORS] = {
	{
		.name = "Multi Stream Start In Packet Error",
		.id = AL_ERR_EVENTS_ETH_V4_EC_ERR_MSW_IN_SOP_IN_PKT,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_A_EC_ERR_MSW_IN_SOP_IN_PKT,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Multi Stream End On Non Packet Error",
		.id = AL_ERR_EVENTS_ETH_V4_EC_ERR_MSW_IN_EOP_IN_IDLE,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_A_EC_ERR_MSW_IN_EOP_IN_IDLE,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RFW No Data On End Of Packet (Meta) Error",
		.id = AL_ERR_EVENTS_ETH_V4_EC_ERR_FWD_OUT_EOP_META_EMPTY,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_A_EC_ERR_FWD_OUT_EOP_META_EMPTY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RFW Start In Packet Error",
		.id = AL_ERR_EVENTS_ETH_V4_EC_ERR_FWD_IN_SOP_IN_PKT,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_A_EC_ERR_FWD_IN_SOP_IN_PKT,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RFW End On Non Packet Error",
		.id = AL_ERR_EVENTS_ETH_V4_EC_ERR_FWD_IN_EOP_IN_IDLE,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_A_EC_ERR_FWD_IN_EOP_IN_IDLE,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX forwarding packet header FIFO memory 0 parity error",
		.id = AL_ERR_EVENTS_ETH_V4_EC_RFW_PKT_HDR_FIFO_MEM_PAR_ERR_0,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_A_EC_RFW_PKT_HDR_FIFO_MEM_PAR_ERR_0,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX forwarding packet header FIFO memory 1 parity error",
		.id = AL_ERR_EVENTS_ETH_V4_EC_RFW_PKT_HDR_FIFO_MEM_PAR_ERR_1,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_A_EC_RFW_PKT_HDR_FIFO_MEM_PAR_ERR_1,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX forwarding metadata FIFO memory 0 parity error",
		.id = AL_ERR_EVENTS_ETH_V4_EC_RFW_META_FIFO_MEM_PAR_ERR_0,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_A_EC_RFW_META_FIFO_MEM_PAR_ERR_0,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX forwarding metadata FIFO memory 1 parity error",
		.id = AL_ERR_EVENTS_ETH_V4_EC_RFW_META_FIFO_MEM_PAR_ERR_1,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_A_EC_RFW_META_FIFO_MEM_PAR_ERR_1,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX forwarding metadata FIFO memory 2 parity error",
		.id = AL_ERR_EVENTS_ETH_V4_EC_RFW_META_FIFO_MEM_PAR_ERR_2,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_A_EC_RFW_META_FIFO_MEM_PAR_ERR_2,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX forwarding data FIFO memory 0 parity error",
		.id = AL_ERR_EVENTS_ETH_V4_EC_RFW_DATA_FIFO_MEM_PAR_ERR_0,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_A_EC_RFW_DATA_FIFO_MEM_PAR_ERR_0,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX forwarding data FIFO memory 1 parity error",
		.id = AL_ERR_EVENTS_ETH_V4_EC_RFW_DATA_FIFO_MEM_PAR_ERR_1,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_A_EC_RFW_DATA_FIFO_MEM_PAR_ERR_1,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RXF Wire Out No Data On End Of Packet Error",
		.id = AL_ERR_EVENTS_ETH_V4_EC_ERR_RXF_OUT_EOP_HDR_EMPTY,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_A_EC_ERR_RXF_OUT_EOP_HDR_EMPTY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "LB FIFO in start of packet error",
		.id = AL_ERR_EVENTS_ETH_V4_EC_ERR_LBF_IN_SOP_IN_PKT,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_A_EC_ERR_LBF_IN_SOP_IN_PKT,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "LB FIFO in end of packet idle error",
		.id = AL_ERR_EVENTS_ETH_V4_EC_ERR_LBF_IN_EOP_IN_IDLE,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_A_EC_ERR_LBF_IN_EOP_IN_IDLE,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX FIFO in start of packet error",
		.id = AL_ERR_EVENTS_ETH_V4_EC_ERR_RXF_IN_SOP_IN_PKT,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_A_EC_ERR_RXF_IN_SOP_IN_PKT,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX FIFO in end of packet idle error",
		.id = AL_ERR_EVENTS_ETH_V4_EC_ERR_RXF_IN_EOP_IN_IDLE,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_A_EC_ERR_RXF_IN_EOP_IN_IDLE,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Wire No Data On End Of Packet Error",
		.id = AL_ERR_EVENTS_ETH_V4_EC_ERR_FAF_2_OUT_EOP_HDR_EMPTY,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_A_EC_ERR_FAF_2_OUT_EOP_HDR_EMPTY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Wire Start In Packet Error",
		.id = AL_ERR_EVENTS_ETH_V4_EC_ERR_FAF_2_IN_SOP_IN_PKT,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_A_EC_ERR_FAF_2_IN_SOP_IN_PKT,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Wire End On Non Packet Error",
		.id = AL_ERR_EVENTS_ETH_V4_EC_ERR_FAF_2_IN_EOP_IN_IDLE,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_A_EC_ERR_FAF_2_IN_EOP_IN_IDLE,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "LB No Data On End Of Packet Error",
		.id = AL_ERR_EVENTS_ETH_V4_EC_ERR_FAF_1_OUT_EOP_HDR_EMPTY,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_A_EC_ERR_FAF_1_OUT_EOP_HDR_EMPTY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "LB Start In Packet Error",
		.id = AL_ERR_EVENTS_ETH_V4_EC_ERR_FAF_1_IN_SOP_IN_PKT,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_A_EC_ERR_FAF_1_IN_SOP_IN_PKT,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "LB End On Non Packet Error",
		.id = AL_ERR_EVENTS_ETH_V4_EC_ERR_FAF_1_IN_EOP_IN_IDLE,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_A_EC_ERR_FAF_1_IN_EOP_IN_IDLE,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TMI in start of packet in error",
		.id = AL_ERR_EVENTS_ETH_V4_EC_ERR_TMI_IN_SOP_IN_PKT,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_B_EC_ERR_TMI_IN_SOP_IN_PKT,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TMI in end of packet in error",
		.id = AL_ERR_EVENTS_ETH_V4_EC_ERR_TMI_IN_EOP_IN_IDLE,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_B_EC_ERR_TMI_IN_EOP_IN_IDLE,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TX forwarding out  end of packet header empty",
		.id = AL_ERR_EVENTS_ETH_V4_EC_ERR_TFW_OUT_EOP_HDR_EMPTY,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_B_EC_ERR_TFW_OUT_EOP_HDR_EMPTY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TX forwarding in start of packet in error",
		.id = AL_ERR_EVENTS_ETH_V4_EC_ERR_TFW_IN_SOP_IN_PKT,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_B_EC_ERR_TFW_IN_SOP_IN_PKT,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TX forwarding in end of packet in idle error",
		.id = AL_ERR_EVENTS_ETH_V4_EC_ERR_TFW_IN_EOP_IN_IDLE,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_B_EC_ERR_TFW_IN_EOP_IN_IDLE,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TFW out s error",
		.id = AL_ERR_EVENTS_ETH_V4_EC_TFW_OUT_S_ERR,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_B_TFW_OUT_S_ERR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TPR no start of packet",
		.id = AL_ERR_EVENTS_ETH_V4_EC_ERR_TPM_NO_SOP_ERR,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_B_EC_ERR_TPM_NO_SOP_ERR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TPR spoof 0 error",
		.id = AL_ERR_EVENTS_ETH_V4_EC_ERR_TPM_SPOOF_ERR_0,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_B_EC_ERR_TPM_SPOOF_ERR_0,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TPR spoof 1 error",
		.id = AL_ERR_EVENTS_ETH_V4_EC_ERR_TPM_SPOOF_ERR_1,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_B_EC_ERR_TPM_SPOOF_ERR_1,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TPR spoof 2 error",
		.id = AL_ERR_EVENTS_ETH_V4_EC_ERR_TPM_SPOOF_ERR_2,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_B_EC_ERR_TPM_SPOOF_ERR_2,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TPR spoof 3 error",
		.id = AL_ERR_EVENTS_ETH_V4_EC_ERR_TPM_SPOOF_ERR_3,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_B_EC_ERR_TPM_SPOOF_ERR_3,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TPE out end of packet header empty",
		.id = AL_ERR_EVENTS_ETH_V4_EC_ERR_TPE_OUT_EOP_HDR_EMPTY,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_B_EC_ERR_TPE_OUT_EOP_HDR_EMPTY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TPE in start of packet in packet error",
		.id = AL_ERR_EVENTS_ETH_V4_EC_ERR_TPE_IN_SOP_IN_PKT,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_B_EC_ERR_TPE_IN_SOP_IN_PKT,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TPE in end of packet in idle error",
		.id = AL_ERR_EVENTS_ETH_V4_EC_ERR_TPE_IN_EOP_IN_IDLE,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_B_EC_ERR_TPE_IN_EOP_IN_IDLE,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TSO count end of segment error",
		.id = AL_ERR_EVENTS_ETH_V4_EC_ERR_TSO_CNT_END_OF_SEG,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_B_EC_ERR_TSO_CNT_END_OF_SEG,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TSO last in SHDR error",
		.id = AL_ERR_EVENTS_ETH_V4_EC_ERR_TSO_LAST_IN_SHDR,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_B_EC_ERR_TSO_LAST_IN_SHDR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TSO count end of packet 4",
		.id = AL_ERR_EVENTS_ETH_V4_EC_ERR_TSO_CNT_END_OF_PKT_4,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_B_EC_ERR_TSO_CNT_END_OF_PKT_4,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TSO count end of packet 3",
		.id = AL_ERR_EVENTS_ETH_V4_EC_ERR_TSO_CNT_END_OF_PKT_3,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_B_EC_ERR_TSO_CNT_END_OF_PKT_3,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TSO count end of packet 2",
		.id = AL_ERR_EVENTS_ETH_V4_EC_ERR_TSO_CNT_END_OF_PKT_2,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_B_EC_ERR_TSO_CNT_END_OF_PKT_2,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TSO count end of packet 1",
		.id = AL_ERR_EVENTS_ETH_V4_EC_ERR_TSO_CNT_END_OF_PKT_1,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_B_EC_ERR_TSO_CNT_END_OF_PKT_1,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TSP no start of packet error",
		.id = AL_ERR_EVENTS_ETH_V4_EC_ERR_TSO_NO_SOP_ERR,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_B_EC_ERR_TSO_NO_SOP_ERR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "HS control table parity 3",
		.id = AL_ERR_EVENTS_ETH_V4_EC_HS_CTRL_TABLE_PAR_ERR_3,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_B_EC_HS_CTRL_TABLE_PAR_ERR_3,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "HS control table parity 2",
		.id = AL_ERR_EVENTS_ETH_V4_EC_HS_CTRL_TABLE_PAR_ERR_2,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_B_EC_HS_CTRL_TABLE_PAR_ERR_2,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "HS control table parity 1",
		.id = AL_ERR_EVENTS_ETH_V4_EC_HS_CTRL_TABLE_PAR_ERR_1,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_B_EC_HS_CTRL_TABLE_PAR_ERR_1,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "HS control table parity 0",
		.id = AL_ERR_EVENTS_ETH_V4_EC_HS_CTRL_TABLE_PAR_ERR_0,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_B_EC_HS_CTRL_TABLE_PAR_ERR_0,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX forwarding in pipe data FIFO parity 0",
		.id = AL_ERR_EVENTS_ETH_V4_EC_RFW_IN_PIPE_DATA_FIFO_PAR_ERR_0,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_B_EC_RFW_IN_PIPE_DATA_FIFO_PAR_ERR_0,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX forwarding in pipe data FIFO parity 1",
		.id = AL_ERR_EVENTS_ETH_V4_EC_RFW_IN_PIPE_DATA_FIFO_PAR_ERR_1,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_B_EC_RFW_IN_PIPE_DATA_FIFO_PAR_ERR_1,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TX forwarding out start of packet info empty error",
		.id = AL_ERR_EVENTS_ETH_V4_EC_ERR_TFW_OUT_SOP_INFO_EMPTY,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_B_EC_ERR_TFW_OUT_SOP_INFO_EMPTY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TX forwarding end of packet FIFO 1 parity",
		.id = AL_ERR_EVENTS_ETH_V4_EC_TFW_EOP_FIFO_1_PAR_ERR,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_C_EC_TFW_EOP_FIFO_1_PAR_ERR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TX forwarding start of packet FIFO 1 parity",
		.id = AL_ERR_EVENTS_ETH_V4_EC_TFW_SOP_FIFO_1_PAR_ERR,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_C_EC_TFW_SOP_FIFO_1_PAR_ERR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TX forwarding GCP gable low parity 3",
		.id = AL_ERR_EVENTS_ETH_V4_EC_TFW_GCP_TABLE_LO_PAR_ERR_3,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_C_EC_TFW_GCP_TABLE_LO_PAR_ERR_3,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX forwarding GCP gable low parity 3",
		.id = AL_ERR_EVENTS_ETH_V4_EC_RFW_GCP_TABLE_LO_PAR_ERR_3,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_C_EC_RFW_GCP_TABLE_LO_PAR_ERR_3,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TX forwarding VLAN table 1 parity",
		.id = AL_ERR_EVENTS_ETH_V4_EC_TFW_VLAN_TABLE_1_PAR_ERR,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_C_EC_TFW_VLAN_TABLE_1_PAR_ERR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TX forwarding control table 1 parity",
		.id = AL_ERR_EVENTS_ETH_V4_EC_RFW_CTRL_TABLE_1_PAR_ERR,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_C_EC_RFW_CTRL_TABLE_1_PAR_ERR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX forwarding VLAN table 1 parity",
		.id = AL_ERR_EVENTS_ETH_V4_EC_RFW_VLAN_TABLE_1_PAR_ERR,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_C_EC_RFW_VLAN_TABLE_1_PAR_ERR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX forwarding flow steering table 1 parity",
		.id = AL_ERR_EVENTS_ETH_V4_EC_RFW_FLOW_TABLE_1_PAR_ERR,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_C_EC_RFW_FLOW_TABLE_1_PAR_ERR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX forwarding THASH table 1 parity",
		.id = AL_ERR_EVENTS_ETH_V4_EC_RFW_THASH_TABLE_1_PAR_ERR,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_C_EC_RFW_THASH_TABLE_1_PAR_ERR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX forwarding MHASH table 1 parity",
		.id = AL_ERR_EVENTS_ETH_V4_EC_RFW_MHASH_TABLE_1_PAR_ERR,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_C_EC_RFW_MHASH_TABLE_1_PAR_ERR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TX forwarding GCP table low parity 2",
		.id = AL_ERR_EVENTS_ETH_V4_EC_TFW_GCP_TABLE_LO_PAR_ERR_2,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_C_EC_TFW_GCP_TABLE_LO_PAR_ERR_2,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TX forwarding GCP table low parity 1",
		.id = AL_ERR_EVENTS_ETH_V4_EC_TFW_GCP_TABLE_LO_PAR_ERR_1,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_C_EC_TFW_GCP_TABLE_LO_PAR_ERR_1,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TX forwarding GCP table low parity 0",
		.id = AL_ERR_EVENTS_ETH_V4_EC_TFW_GCP_TABLE_LO_PAR_ERR_0,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_C_EC_TFW_GCP_TABLE_LO_PAR_ERR_0,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX forwarding GCP table low parity 2",
		.id = AL_ERR_EVENTS_ETH_V4_EC_RFW_GCP_TABLE_LO_PAR_ERR_2,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_C_EC_RFW_GCP_TABLE_LO_PAR_ERR_2,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX forwarding GCP table low parity 1",
		.id = AL_ERR_EVENTS_ETH_V4_EC_RFW_GCP_TABLE_LO_PAR_ERR_1,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_C_EC_RFW_GCP_TABLE_LO_PAR_ERR_1,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX forwarding GCP table low parity 0",
		.id = AL_ERR_EVENTS_ETH_V4_EC_RFW_GCP_TABLE_LO_PAR_ERR_0,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_C_EC_RFW_GCP_TABLE_LO_PAR_ERR_0,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TX forwarding data FIFO 2 parity 0",
		.id = AL_ERR_EVENTS_ETH_V4_EC_TFW_DATA_FIFO_2_PAR_ERR_0,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_C_EC_TFW_DATA_FIFO_2_PAR_ERR_0,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TX forwarding data FIFO 1 parity 0",
		.id = AL_ERR_EVENTS_ETH_V4_EC_TFW_DATA_FIFO_1_PAR_ERR_0,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_C_EC_TFW_DATA_FIFO_1_PAR_ERR_0,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TX forwarding data FIFO 2 parity 1",
		.id = AL_ERR_EVENTS_ETH_V4_EC_TFW_DATA_FIFO_2_PAR_ERR_1,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_C_EC_TFW_DATA_FIFO_2_PAR_ERR_1,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TX forwarding data FIFO 1 parity 1",
		.id = AL_ERR_EVENTS_ETH_V4_EC_TFW_DATA_FIFO_1_PAR_ERR_1,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_C_EC_TFW_DATA_FIFO_1_PAR_ERR_1,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TSO meta cache 1 4 parity",
		.id = AL_ERR_EVENTS_ETH_V4_EC_TSO_META_CACHE_1_4_PAR_ERR,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_C_EC_TSO_META_CACHE_1_4_PAR_ERR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TSO meta cache 1 3 parity",
		.id = AL_ERR_EVENTS_ETH_V4_EC_TSO_META_CACHE_1_3_PAR_ERR,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_C_EC_TSO_META_CACHE_1_3_PAR_ERR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TSO meta cache 1 2 parity",
		.id = AL_ERR_EVENTS_ETH_V4_EC_TSO_META_CACHE_1_2_PAR_ERR,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_C_EC_TSO_META_CACHE_1_2_PAR_ERR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TSO meta cache 1 1 parity",
		.id = AL_ERR_EVENTS_ETH_V4_EC_TSO_META_CACHE_1_1_PAR_ERR,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_C_EC_TSO_META_CACHE_1_1_PAR_ERR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Timestamp FIFO overrun",
		.id = AL_ERR_EVENTS_ETH_V4_EC_ERR_TIMESTAMP_FIFO_OVERRUN,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_D,
		.iofic_bit = AL_ETH_V4_EC_IOFIC_GROUP_D_ERR_TIMESTAMP_FIFO_OVERRUN,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
};

/** EC secondary iofic error fields */
static const struct al_err_events_field
	al_err_events_eth_ec_sec_fields[AL_ERR_EVENTS_ETH_V4_EC_SEC_MAX_ERRORS] = {
	{
		.name = "TX forwarding table high memory parity 0",
		.id = AL_ERR_EVENTS_ETH_V4_EC_SEC_TFW_GCP_TABLE_HI_PAR_ERR_0,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_TFW_GCP_TABLE_HI_PAR_ERR_0,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TX forwarding table high memory parity 1",
		.id = AL_ERR_EVENTS_ETH_V4_EC_SEC_TFW_GCP_TABLE_HI_PAR_ERR_1,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_TFW_GCP_TABLE_HI_PAR_ERR_1,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TX forwarding table high memory parity 2",
		.id = AL_ERR_EVENTS_ETH_V4_EC_SEC_TFW_GCP_TABLE_HI_PAR_ERR_2,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_TFW_GCP_TABLE_HI_PAR_ERR_2,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TX forwarding table high memory parity 3",
		.id = AL_ERR_EVENTS_ETH_V4_EC_SEC_TFW_GCP_TABLE_HI_PAR_ERR_3,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_TFW_GCP_TABLE_HI_PAR_ERR_3,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TX forwarding table high memory parity 4",
		.id = AL_ERR_EVENTS_ETH_V4_EC_SEC_TFW_GCP_TABLE_HI_PAR_ERR_4,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_TFW_GCP_TABLE_HI_PAR_ERR_4,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TX forwarding table high memory parity 5",
		.id = AL_ERR_EVENTS_ETH_V4_EC_SEC_TFW_GCP_TABLE_HI_PAR_ERR_5,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_TFW_GCP_TABLE_HI_PAR_ERR_5,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TX forwarding table high memory parity 6",
		.id = AL_ERR_EVENTS_ETH_V4_EC_SEC_TFW_GCP_TABLE_HI_PAR_ERR_6,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_TFW_GCP_TABLE_HI_PAR_ERR_6,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "TX forwarding table high memory parity 7",
		.id = AL_ERR_EVENTS_ETH_V4_EC_SEC_TFW_GCP_TABLE_HI_PAR_ERR_7,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_TFW_GCP_TABLE_HI_PAR_ERR_7,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX forwarding table high memory parity 0",
		.id = AL_ERR_EVENTS_ETH_V4_EC_SEC_RFW_GCP_TABLE_HI_PAR_ERR_0,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_RFW_GCP_TABLE_HI_PAR_ERR_0,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX forwarding table high memory parity 1",
		.id = AL_ERR_EVENTS_ETH_V4_EC_SEC_RFW_GCP_TABLE_HI_PAR_ERR_1,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_RFW_GCP_TABLE_HI_PAR_ERR_1,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX forwarding table high memory parity 2",
		.id = AL_ERR_EVENTS_ETH_V4_EC_SEC_RFW_GCP_TABLE_HI_PAR_ERR_2,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_RFW_GCP_TABLE_HI_PAR_ERR_2,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX forwarding table high memory parity 3",
		.id = AL_ERR_EVENTS_ETH_V4_EC_SEC_RFW_GCP_TABLE_HI_PAR_ERR_3,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_RFW_GCP_TABLE_HI_PAR_ERR_3,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX forwarding table high memory parity 4",
		.id = AL_ERR_EVENTS_ETH_V4_EC_SEC_RFW_GCP_TABLE_HI_PAR_ERR_4,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_RFW_GCP_TABLE_HI_PAR_ERR_4,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX forwarding table high memory parity 5",
		.id = AL_ERR_EVENTS_ETH_V4_EC_SEC_RFW_GCP_TABLE_HI_PAR_ERR_5,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_RFW_GCP_TABLE_HI_PAR_ERR_5,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX forwarding table high memory parity 6",
		.id = AL_ERR_EVENTS_ETH_V4_EC_SEC_RFW_GCP_TABLE_HI_PAR_ERR_6,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_RFW_GCP_TABLE_HI_PAR_ERR_6,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX forwarding table high memory parity 7",
		.id = AL_ERR_EVENTS_ETH_V4_EC_SEC_RFW_GCP_TABLE_HI_PAR_ERR_7,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_RFW_GCP_TABLE_HI_PAR_ERR_7,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RFF data fifo low memory parity",
		.id = AL_ERR_EVENTS_ETH_V4_EC_SEC_RFF_DATA_FIFO_LO_MEM_PAR_ERR,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_RFF_DATA_FIFO_LO_MEM_PAR_ERR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RFF data fifo high memory parity",
		.id = AL_ERR_EVENTS_ETH_V4_EC_SEC_RFF_DATA_FIFO_HI_MEM_PAR_ERR,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_RFF_DATA_FIFO_HI_MEM_PAR_ERR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX RFW FIFO to RFW shared resource result parity 0",
		.id = AL_ERR_EVENTS_ETH_V4_EC_SEC_RFF_RFW_SR_RESULT_FIFO_MEM_PAR_ERR_0,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_RFF_RFW_SR_RESULT_FIFO_MEM_PAR_ERR_0,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX RFW FIFO to RFW shared resource result parity 1",
		.id = AL_ERR_EVENTS_ETH_V4_EC_SEC_RFF_RFW_SR_RESULT_FIFO_MEM_PAR_ERR_1,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_RFF_RFW_SR_RESULT_FIFO_MEM_PAR_ERR_1,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX RFW FIFO to RFW shared resource result parity 2",
		.id = AL_ERR_EVENTS_ETH_V4_EC_SEC_RFF_RFW_SR_RESULT_FIFO_MEM_PAR_ERR_2,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_RFF_RFW_SR_RESULT_FIFO_MEM_PAR_ERR_2,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX RFW FIFO to RFW shared resource result parity 3",
		.id = AL_ERR_EVENTS_ETH_V4_EC_SEC_RFF_RFW_SR_RESULT_FIFO_MEM_PAR_ERR_3,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_RFF_RFW_SR_RESULT_FIFO_MEM_PAR_ERR_3,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX RFW parse result FIFO memory parity 0",
		.id = AL_ERR_EVENTS_ETH_V4_EC_SEC_RFF_PARSE_RES_FIFO_MEM_PAR_ERR_0,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_RFF_PARSE_RES_FIFO_MEM_PAR_ERR_0,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX RFW parse result FIFO memory parity 1",
		.id = AL_ERR_EVENTS_ETH_V4_EC_SEC_RFF_PARSE_RES_FIFO_MEM_PAR_ERR_1,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_RFF_PARSE_RES_FIFO_MEM_PAR_ERR_1,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX RFW parse result FIFO memory parity 2",
		.id = AL_ERR_EVENTS_ETH_V4_EC_SEC_RFF_PARSE_RES_FIFO_MEM_PAR_ERR_2,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_RFF_PARSE_RES_FIFO_MEM_PAR_ERR_2,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX RFW parse result FIFO memory parity 3",
		.id = AL_ERR_EVENTS_ETH_V4_EC_SEC_RFF_PARSE_RES_FIFO_MEM_PAR_ERR_3,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_RFF_PARSE_RES_FIFO_MEM_PAR_ERR_3,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX RFW parse result FIFO memory parity 4",
		.id = AL_ERR_EVENTS_ETH_V4_EC_SEC_RFF_PARSE_RES_FIFO_MEM_PAR_ERR_4,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_RFF_PARSE_RES_FIFO_MEM_PAR_ERR_4,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX RFW parse result FIFO memory parity 5",
		.id = AL_ERR_EVENTS_ETH_V4_EC_SEC_RFF_PARSE_RES_FIFO_MEM_PAR_ERR_5,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_RFF_PARSE_RES_FIFO_MEM_PAR_ERR_5,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX RFW parse result FIFO memory parity 6",
		.id = AL_ERR_EVENTS_ETH_V4_EC_SEC_RFF_PARSE_RES_FIFO_MEM_PAR_ERR_6,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_EC_SEC_IOFIC_GROUP_A_EC_RFF_PARSE_RES_FIFO_MEM_PAR_ERR_6,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
};

static const struct al_err_events_field
	al_err_events_eth_v4_mac_fields[AL_ERR_EVENTS_ETH_V4_MAC_MAX_ERRORS] = {
{
		.name = "25G/50G MAC TX FIFO error",
		.id = AL_ERR_EVENTS_ETH_V4_MAC_LMAC_FF_TX_ERR_SB_SYNC_EDGE,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_MAC_IOFIC_GROUP_A_LMAC_FF_TX_ERR_SB_SYNC_EDGE,
		.sev = AL_ERR_EVENTS_CORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_POLLING,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "25G/50G MAC RX FIFO error",
		.id = AL_ERR_EVENTS_ETH_V4_MAC_LMAC_FF_RX_ERR_SB_SYNC_EDGE,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_MAC_IOFIC_GROUP_A_LMAC_FF_RX_ERR_SB_SYNC_EDGE,
		.sev = AL_ERR_EVENTS_CORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_POLLING,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "25G/50G MAC TX underflow",
		.id = AL_ERR_EVENTS_ETH_V4_MAC_LMAC_TX_UNDERFLOW_SB_SYNC_EDGE,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_MAC_IOFIC_GROUP_A_LMAC_TX_UNDERFLOW_SB_SYNC_EDGE,
		.sev = AL_ERR_EVENTS_CORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_POLLING,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "25G/50G MAC statistics parity error",
		.id = AL_ERR_EVENTS_ETH_V4_MAC_LMAC_STAT_PAR_ERR_SB_SYNC_EDGE,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_V4_MAC_IOFIC_GROUP_A_LMAC_STAT_PAR_ERR_SB_SYNC_EDGE,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "100G MAC underflow error",
		.id = AL_ERR_EVENTS_ETH_V4_MAC_CMAC_TX_UNDERFLOW_SB_SYNC_EDGE,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_V4_MAC_IOFIC_GROUP_B_CMAC_TX_UNDERFLOW_SB_SYNC_EDGE,
		.sev = AL_ERR_EVENTS_CORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_POLLING,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "PCS FEC91 parity error",
		.id = AL_ERR_EVENTS_ETH_V4_MAC_PCS_FEC91_PAR_ERR_SB_SYNC_EDGE,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_V4_MAC_IOFIC_GROUP_C_PCS_FEC91_PAR_ERR_SB_SYNC_EDGE,
		.sev = AL_ERR_EVENTS_CORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_POLLING,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "PCS FEC74 parity error",
		.id = AL_ERR_EVENTS_ETH_V4_MAC_PCS_FEC74_PAR_ERR_SB_SYNC_EDGE,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_V4_MAC_IOFIC_GROUP_C_PCS_FEC74_PAR_ERR_SB_SYNC_EDGE,
		.sev = AL_ERR_EVENTS_CORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_POLLING,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "PCS deskew parity error",
		.id = AL_ERR_EVENTS_ETH_V4_MAC_PCS_DESKEW_PAR_ERR_SB_SYNC_EDGE,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_V4_MAC_IOFIC_GROUP_C_PCS_DESKEW_PAR_ERR_SB_SYNC_EDGE,
		.sev = AL_ERR_EVENTS_CORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_POLLING,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
};

/*******************************************************************************
 ** EC
 ******************************************************************************/
static int err_events_eth_ec_collect(struct al_err_events_module *module,
				     enum al_err_events_collect collect)
{
	int ret;
	unsigned int iofic_group;
	unsigned int iofic_id;
	struct al_err_events_eth_ec_data *data = module->private_data;
	struct al_err_events_eth_mac_data *mac_data = &data->mac_data;
	struct al_err_events_eth_ec_sec_data *sec_data = &data->sec_data;
	struct al_err_events_eth_epe_data *epe_data = &data->epe_data;

	/**
	 * Sub units are listed as their own modules so no need to collect from inside parent unit
	 * while on interrupt context
	 */
	if (collect != AL_ERR_EVENTS_COLLECT_POLLING) {
		/* Collect from MAC */
		ret = al_err_events_module_collect(&mac_data->module, collect);
		if (ret)
			return ret;

		if (sec_data->module.enabled) {
			/* Collect from Secondary level EC */
			ret = al_err_events_module_collect(&sec_data->module, collect);
			if (ret)
				return ret;
		}

		if ((module->primary_index == 0) && (epe_data->module.enabled)) {
			/* Collect from EPE */
			ret = al_err_events_module_collect(&epe_data->module, collect);
			if (ret)
				return ret;
		}
	}

	for (iofic_id = 0; iofic_id < module->ic_size; iofic_id++)
		for (iofic_group = AL_INT_GROUP_A; iofic_group <= AL_INT_GROUP_D; iofic_group++) {
			ret = al_err_events_common_module_iofic_collect(module, collect,
									iofic_id, iofic_group);
			if (ret)
				return ret;
		}

	return 0;
}

static int err_events_mac_init(struct al_err_events_handle *handle,
			      struct al_err_events_eth_ec_data *data,
			      struct al_err_events_eth_ec_init_params *params);

static int al_err_events_eth_ec_sec_init(struct al_err_events_handle *handle,
			      struct al_err_events_eth_ec_sec_data *data,
			      struct al_err_events_eth_ec_init_params *params);

static void err_events_epe_init(struct al_err_events_handle *handle,
			struct al_err_events_eth_ec_data *data,
			struct al_err_events_eth_ec_init_params *params)
{
	struct al_err_events_eth_epe_init_params epe_params;
	struct al_eth_epe_handle_init_params epe_handle_params;

	/* Initialize EPE module */
	al_memset(&epe_params, 0, sizeof(struct al_err_events_eth_epe_init_params));
	al_memset(&epe_handle_params, 0, sizeof(struct al_eth_epe_handle_init_params));

	epe_handle_params.eth_rev_id = al_eth_rev_id_get(params->adapter);
	epe_handle_params.regs_base = al_eth_epe_regs_get(params->adapter);
	epe_handle_params.type = EPE_TYPE_OUTER;

	epe_params.ec_intc_regs_base =
		al_eth_iofic_regs_base_get(params->adapter, AL_ETH_IOFIC_LEVEL_PRIMARY);
	epe_params.handle_params = &epe_handle_params;
	epe_params.collect_mode = params->collect_mode;
	al_err_events_eth_epe_init(handle, &data->epe_data, &epe_params);

	al_info("Registered %s:%s errors\n", data->module.name, data->epe_data.module.name);
}

/** EC init */
int al_err_events_eth_ec_init(struct al_err_events_handle *handle,
			      struct al_err_events_eth_ec_data *data,
			      struct al_err_events_eth_ec_init_params *params)
{
	uint32_t a_mask, b_mask;
	uint32_t c_mask, d_mask;
	struct al_err_events_module *module;
	struct al_err_events_common_module_params module_params;
	unsigned int rev_id;

	al_assert(handle);
	al_assert(data);
	al_assert(params);

	rev_id = al_eth_rev_id_get(params->adapter);

	if (rev_id < AL_ETH_REV_ID_4) {
		al_assert(sizeof(al_err_events_eth_ec_fields) <= sizeof(data->fields));
		al_assert(sizeof(al_err_events_eth_ec_fields) ==
			(AL_ERR_EVENTS_ETH_EC_MAX_ERRORS * sizeof(struct al_err_events_field)));
	} else {
		al_assert(sizeof(al_err_events_eth_v4_ec_fields) <= sizeof(data->fields));
		al_assert(sizeof(al_err_events_eth_v4_ec_fields) ==
			(AL_ERR_EVENTS_ETH_V4_EC_MAX_ERRORS * sizeof(struct al_err_events_field)));
	}
	al_assert(params->adapter);

	al_memset(&module_params, 0, sizeof(module_params));

	module = &data->module;
	data->adapter = params->adapter;

	/* init module */
	al_sprintf(module_params.name, "ETH(%d)-EC", params->eth_index);

	if (rev_id >= AL_ETH_REV_ID_4) {
		module_params.fields_size = AL_ERR_EVENTS_ETH_V4_EC_MAX_ERRORS;
		module_params.fields_template = al_err_events_eth_v4_ec_fields;
	} else {
		module_params.fields_size = AL_ERR_EVENTS_ETH_EC_MAX_ERRORS;
		module_params.fields_template = al_err_events_eth_ec_fields;
	}
	module_params.primary_module = AL_ERR_EVENTS_MODULE_ETH;
	module_params.secondary_module = AL_ERR_EVENTS_SEC_MODULE_ETH_EC;
	module_params.primary_index = params->eth_index;
	module_params.secondary_index = 0;
	module_params.collect_mode = params->collect_mode;
	module_params.ic_regs_bases[0] =
		al_eth_iofic_regs_base_get(params->adapter, AL_ETH_IOFIC_LEVEL_PRIMARY);
	module_params.ic_size = 1;
	module_params.fields = data->fields;
	module_params.private_data = data;
	module_params.collect = err_events_eth_ec_collect;
	module_params.print = al_err_events_common_module_print;
	module_params.clear = al_err_events_common_module_clear;
	module_params.test = al_err_events_common_module_test;

	al_err_events_common_module_init(handle, module, &module_params);

	al_eth_ec_iofic_attrs_get(params->adapter, AL_ETH_IOFIC_LEVEL_PRIMARY,
				  AL_ETH_IOFIC_ATTRS_RUNTIME_CONFIG,
				  &a_mask,
				  &b_mask,
				  &c_mask,
				  &d_mask);

	al_err_event_common_update_valid_fields(module, 0,
						a_mask,
						b_mask,
						c_mask,
						d_mask);

	al_err_events_common_module_post_init(module);

	/* Initialize sub modules */
	err_events_mac_init(handle, data, params);

	/** No submodules in these eth revisions */
	if (rev_id < AL_ETH_REV_ID_4)
		return 0;

	/* Initialize EC secondary level iofic module */
	al_err_events_eth_ec_sec_init(handle, &data->sec_data, params);

	if (params->eth_index == 0)
		err_events_epe_init(handle, data, params);

	return 0;
}


static int err_events_eth_ec_sec_collect(struct al_err_events_module *module,
				enum al_err_events_collect collect)
{
	int ret;
	unsigned int iofic_group;
	uint32_t cause;
	al_bool should_collect = AL_FALSE;
	uint32_t cause_clr = 0;
	unsigned int group_bits_in_primary[] = {
		AL_ETH_V4_EC_IOFIC_GROUP_C_EC_SEC_INT_OUTPUT_0,
		AL_ETH_V4_EC_IOFIC_GROUP_C_EC_SEC_INT_OUTPUT_1,
		AL_ETH_V4_EC_IOFIC_GROUP_C_EC_SEC_INT_OUTPUT_2,
		AL_ETH_V4_EC_IOFIC_GROUP_C_EC_SEC_INT_OUTPUT_3,
	};
	struct al_err_events_eth_ec_sec_data *ec_sec_data =
		(struct al_err_events_eth_ec_sec_data *)module->private_data;
	void __iomem *primary_ints_base =
		al_eth_iofic_regs_base_get(ec_sec_data->adapter, AL_ETH_IOFIC_LEVEL_PRIMARY);

	if (collect == AL_ERR_EVENTS_COLLECT_POLLING)
		should_collect = AL_TRUE;
	else if (collect == AL_ERR_EVENTS_COLLECT_INTERRUPT)
		cause = al_iofic_read_cause(primary_ints_base, AL_INT_GROUP_C);

	for (iofic_group = AL_INT_GROUP_A; iofic_group <= AL_INT_GROUP_D; iofic_group++) {

		if (collect == AL_ERR_EVENTS_COLLECT_INTERRUPT)
			should_collect = !!(cause & group_bits_in_primary[iofic_group]);

		if (!should_collect)
			continue;

		ret = al_err_events_common_module_iofic_collect(module, collect, 0, iofic_group);
		if (ret)
			return ret;

		if (collect == AL_ERR_EVENTS_COLLECT_INTERRUPT) {
			should_collect = AL_FALSE;
			cause_clr |= group_bits_in_primary[iofic_group];
		}
	}

	/** Clean bit in primary iofic */
	if (cause_clr)
		al_iofic_clear_cause(primary_ints_base,
			AL_INT_GROUP_C,
			cause_clr);

	return 0;
}

static int al_err_events_eth_ec_sec_init(struct al_err_events_handle *handle,
			      struct al_err_events_eth_ec_sec_data *data,
			      struct al_err_events_eth_ec_init_params *params)
{
	uint32_t a_mask, b_mask;
	uint32_t c_mask, d_mask;
	struct al_err_events_module *module;
	struct al_err_events_common_module_params module_params;

	al_assert(handle);
	al_assert(data);
	al_assert(params);
	al_assert(sizeof(al_err_events_eth_ec_sec_fields) == sizeof(data->fields));
	al_assert(sizeof(al_err_events_eth_ec_sec_fields) ==
		 (AL_ERR_EVENTS_ETH_V4_EC_SEC_MAX_ERRORS * sizeof(struct al_err_events_field)));
	al_assert(params->adapter);
	al_assert(al_eth_rev_id_get(params->adapter) >= AL_ETH_REV_ID_4);

	module = &data->module;

	/** init data */
	data->adapter = params->adapter;

	/* init module */
	al_memset(&module_params, 0, sizeof(module_params));
	al_sprintf(module_params.name, "ETH(%d)-EC secondary", params->eth_index);
	module_params.primary_module = AL_ERR_EVENTS_MODULE_ETH;
	module_params.secondary_module = AL_ERR_EVENTS_SEC_MODULE_ETH_EC_SEC;
	module_params.primary_index = params->eth_index;
	module_params.secondary_index = 0;
	module_params.collect_mode = params->collect_mode;
	module_params.ic_regs_bases[0] =
		al_eth_iofic_regs_base_get(params->adapter, AL_ETH_IOFIC_LEVEL_SECONDARY);
	module_params.ic_size = 1;
	module_params.fields_size = AL_ERR_EVENTS_ETH_V4_EC_SEC_MAX_ERRORS;
	module_params.fields = data->fields;
	module_params.fields_template = al_err_events_eth_ec_sec_fields;
	module_params.private_data = data;
	module_params.collect = err_events_eth_ec_sec_collect;
	module_params.print = al_err_events_common_module_print;
	module_params.clear = al_err_events_common_module_clear;
	module_params.test = al_err_events_common_module_test;

	al_err_events_common_module_init(handle, module, &module_params);

	al_eth_ec_iofic_attrs_get(params->adapter, AL_ETH_IOFIC_LEVEL_SECONDARY,
				  AL_ETH_IOFIC_ATTRS_RUNTIME_CONFIG,
				  &a_mask,
				  &b_mask,
				  &c_mask,
				  &d_mask);

	al_err_event_common_update_valid_fields(module, 0,
						a_mask,
						b_mask,
						c_mask,
						d_mask);

	al_err_events_common_module_post_init(module);

	return 0;
}

/*******************************************************************************
 ** MAC
 ******************************************************************************/
static int err_events_eth_mac_collect(struct al_err_events_module *module,
				      enum al_err_events_collect collect)
{

	uint32_t cause;
	unsigned int iofic_group;
	al_bool should_collect = AL_FALSE;
	struct al_err_events_eth_mac_data *data =
			(struct al_err_events_eth_mac_data *)module->private_data;
	unsigned int iofic_id;

	if (collect == AL_ERR_EVENTS_COLLECT_POLLING) {
		should_collect = AL_TRUE;
	} else if (collect == AL_ERR_EVENTS_COLLECT_INTERRUPT) {
		cause = al_iofic_read_cause(data->ec_iofic_regs_base, AL_INT_GROUP_A);
		should_collect = !!(cause & data->mac_int_bit);
	}

	if (!should_collect)
		return 0;

	for (iofic_id = 0; iofic_id < module->ic_size; iofic_id++)
		for (iofic_group = AL_INT_GROUP_A; iofic_group <= AL_INT_GROUP_D; iofic_group++)
			al_err_events_common_module_iofic_collect(
				module, collect, iofic_id, iofic_group);

	if (collect == AL_ERR_EVENTS_COLLECT_INTERRUPT)
		al_iofic_clear_cause(data->ec_iofic_regs_base,
			     AL_INT_GROUP_A,
			     data->mac_int_bit);

	return 0;
}

/** MAC init params */
struct al_err_events_eth_mac_init_params {
	/** Ethernet adapter handle */
	struct al_hal_eth_adapter *adapter;

	/* ETH index */
	unsigned int eth_index;

	/** collection mode */
	enum al_err_events_collect collect_mode;
};

static int al_err_events_eth_mac_init(struct al_err_events_handle *handle,
			       struct al_err_events_eth_mac_data *data,
			       struct al_err_events_eth_mac_init_params *params)
{
	uint32_t a_mask, b_mask;
	uint32_t c_mask, d_mask;
	struct al_err_events_module *module;
	struct al_err_events_common_module_params module_params;
	unsigned int rev_id;

	al_assert(handle);
	al_assert(data);
	al_assert(params);
	al_assert(params->adapter);
	rev_id = al_eth_rev_id_get(params->adapter);
	al_assert(rev_id <= AL_ETH_REV_ID_4);

	if (rev_id >= AL_ETH_REV_ID_4) {
		al_assert(sizeof(al_err_events_eth_v4_mac_fields) <= sizeof(data->fields));
		al_assert(sizeof(al_err_events_eth_v4_mac_fields) ==
			(AL_ERR_EVENTS_ETH_V4_MAC_MAX_ERRORS * sizeof(struct al_err_events_field)));
	} else {
		al_assert(sizeof(al_err_events_eth_mac_fields) <= sizeof(data->fields));
		al_assert(sizeof(al_err_events_eth_mac_fields) ==
			(AL_ERR_EVENTS_ETH_MAC_MAX_ERRORS * sizeof(struct al_err_events_field)));
	}

	module = &data->module;

	/* init data */
	data->ec_iofic_regs_base =
		al_eth_iofic_regs_base_get(params->adapter, AL_ETH_IOFIC_LEVEL_PRIMARY);
	data->mac_obj = al_eth_mac_obj_get(params->adapter);

	al_memset(&module_params, 0, sizeof(module_params));

	/* init module */
	if (rev_id >= AL_ETH_REV_ID_4) {
		uint32_t mac_ints[AL_ETH_V4_EC_IOFIC_GROUP_A_MAC_INT_NUM] = {
			AL_ETH_V4_EC_IOFIC_GROUP_A_MAC_INT_IN_0,
			AL_ETH_V4_EC_IOFIC_GROUP_A_MAC_INT_IN_1,
			AL_ETH_V4_EC_IOFIC_GROUP_A_MAC_INT_IN_2,
			AL_ETH_V4_EC_IOFIC_GROUP_A_MAC_INT_IN_3,
		};

		data->mac_idx = al_eth_get_serdes_lane(params->adapter);
		al_sprintf(module_params.name, "ETH v4(%d)-MAC(%d)",
			params->eth_index, data->mac_idx);
		module_params.fields_size = AL_ERR_EVENTS_ETH_V4_MAC_MAX_ERRORS;
		module_params.fields_template = al_err_events_eth_v4_mac_fields;

		data->mac_int_bit = mac_ints[data->mac_idx];
	} else {
		data->mac_idx = 0;
		al_sprintf(module_params.name, "ETH(%d)-MAC", params->eth_index);
		module_params.fields_size = AL_ERR_EVENTS_ETH_MAC_MAX_ERRORS;
		module_params.fields_template = al_err_events_eth_mac_fields;

		data->mac_int_bit = AL_ETH_EC_IOFIC_GROUP_A_MAC_INT_IN;
	}

	al_eth_mac_iofic_attrs_get(data->mac_obj,
				   AL_ETH_IOFIC_ATTRS_RUNTIME_CONFIG,
				   &a_mask, &b_mask, &c_mask, &d_mask);

	module_params.primary_module = AL_ERR_EVENTS_MODULE_ETH;
	module_params.secondary_module = AL_ERR_EVENTS_SEC_MODULE_ETH_MAC;
	module_params.primary_index = params->eth_index;
	module_params.secondary_index = data->mac_idx;
	module_params.collect_mode = params->collect_mode;
	module_params.ic_regs_bases[0] = al_eth_mac_iofic_regs_get(data->mac_obj);
	module_params.ic_size = 1;
	module_params.fields = data->fields;
	module_params.private_data = data;
	module_params.collect = err_events_eth_mac_collect;
	module_params.print = al_err_events_common_module_print;
	module_params.clear = al_err_events_common_module_clear;
	module_params.test = al_err_events_common_module_test;

	al_err_events_common_module_init(handle, module, &module_params);

	al_err_event_common_update_valid_fields(module, 0,
						a_mask,
						b_mask,
						c_mask,
						d_mask);

	al_err_events_common_module_post_init(module);

	return 0;
}

static int err_events_mac_init(struct al_err_events_handle *handle,
			      struct al_err_events_eth_ec_data *data,
			      struct al_err_events_eth_ec_init_params *params)
{
	struct al_err_events_eth_mac_init_params mac_params;

	al_memset(&mac_params, 0, sizeof(struct al_err_events_eth_mac_init_params));
	mac_params.adapter = params->adapter;
	mac_params.eth_index = params->eth_index;
	mac_params.collect_mode = params->collect_mode;

	al_err_events_eth_mac_init(handle, &data->mac_data, &mac_params);

	return 0;
}
