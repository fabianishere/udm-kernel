/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#include "al_mod_err_events_internal.h"
#include "al_mod_err_events_eth.h"
#include "al_mod_err_events_eth_epe.h"
#include "al_mod_hal_eth.h"
#include "al_mod_hal_udma_iofic.h"

/*******************************************************************************
 ** Static error fields
 ******************************************************************************/
static const struct al_mod_err_events_field
	al_mod_err_events_eth_epe_fields[AL_ERR_EVENTS_ETH_EPE_MAX_ERRORS] = {
	{
		.name = "RX multi parser header input FIFO memory 0 parity error",
		.id = AL_ERR_EVENTS_ETH_EPE_RMP_HDR_INPUT_FIFO_MEM_PAR_ERR_0,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_EPE_GROUP_A_RMP_HDR_INPUT_FIFO_MEM_PAR_ERR_0,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX multi parser header input FIFO memory 1 parity error",
		.id = AL_ERR_EVENTS_ETH_EPE_RMP_HDR_INPUT_FIFO_MEM_PAR_ERR_1,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_EPE_GROUP_A_RMP_HDR_INPUT_FIFO_MEM_PAR_ERR_1,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX multi parser header input FIFO memory 2 parity error",
		.id = AL_ERR_EVENTS_ETH_EPE_RMP_HDR_INPUT_FIFO_MEM_PAR_ERR_2,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_EPE_GROUP_A_RMP_HDR_INPUT_FIFO_MEM_PAR_ERR_2,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX multi parser header input FIFO memory 3 parity error",
		.id = AL_ERR_EVENTS_ETH_EPE_RMP_HDR_INPUT_FIFO_MEM_PAR_ERR_3,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_EPE_GROUP_A_RMP_HDR_INPUT_FIFO_MEM_PAR_ERR_3,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX multi parser header input FIFO memory 4 parity error",
		.id = AL_ERR_EVENTS_ETH_EPE_RMP_HDR_INPUT_FIFO_MEM_PAR_ERR_4,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_EPE_GROUP_A_RMP_HDR_INPUT_FIFO_MEM_PAR_ERR_4,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX multi parser header input FIFO memory 5 parity error",
		.id = AL_ERR_EVENTS_ETH_EPE_RMP_HDR_INPUT_FIFO_MEM_PAR_ERR_5,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_EPE_GROUP_A_RMP_HDR_INPUT_FIFO_MEM_PAR_ERR_5,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX multi parser header input FIFO memory 6 parity error",
		.id = AL_ERR_EVENTS_ETH_EPE_RMP_HDR_INPUT_FIFO_MEM_PAR_ERR_6,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_EPE_GROUP_A_RMP_HDR_INPUT_FIFO_MEM_PAR_ERR_6,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX multi parser header input FIFO memory 7 parity error",
		.id = AL_ERR_EVENTS_ETH_EPE_RMP_HDR_INPUT_FIFO_MEM_PAR_ERR_7,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_EPE_GROUP_A_RMP_HDR_INPUT_FIFO_MEM_PAR_ERR_7,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX multi parser data FIFO low memory parity error",
		.id = AL_ERR_EVENTS_ETH_EPE_RMP_DATA_FIFO_LO_MEM_PAR_ERR,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_EPE_GROUP_A_RMP_DATA_FIFO_LO_MEM_PAR_ERR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX multi parser data FIFO high memory parity error",
		.id = AL_ERR_EVENTS_ETH_EPE_RMP_DATA_FIFO_HI_MEM_PAR_ERR,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_ETH_EPE_GROUP_A_RMP_DATA_FIFO_HI_MEM_PAR_ERR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},

	{
		.name = "EC RX parser engine 0 error - end of packet in idle",
		.id = AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_IN_EOP_IN_IDLE_0,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_EPE_GROUP_B_EC_ERR_RPE_IN_EOP_IN_IDLE_0,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "EC RX parser engine 1 error - end of packet in idle",
		.id = AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_IN_EOP_IN_IDLE_1,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_EPE_GROUP_B_EC_ERR_RPE_IN_EOP_IN_IDLE_1,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "EC RX parser engine 2 error - end of packet in idle",
		.id = AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_IN_EOP_IN_IDLE_2,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_EPE_GROUP_B_EC_ERR_RPE_IN_EOP_IN_IDLE_2,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "EC RX parser engine 3 error - end of packet in idle",
		.id = AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_IN_EOP_IN_IDLE_3,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_EPE_GROUP_B_EC_ERR_RPE_IN_EOP_IN_IDLE_3,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "EC RX parser engine 4 error - end of packet in idle",
		.id = AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_IN_EOP_IN_IDLE_4,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_EPE_GROUP_B_EC_ERR_RPE_IN_EOP_IN_IDLE_4,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "EC RX parser engine 5 error - end of packet in idle",
		.id = AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_IN_EOP_IN_IDLE_5,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_EPE_GROUP_B_EC_ERR_RPE_IN_EOP_IN_IDLE_5,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "EC RX parser engine 6 error - end of packet in idle",
		.id = AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_IN_EOP_IN_IDLE_6,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_EPE_GROUP_B_EC_ERR_RPE_IN_EOP_IN_IDLE_6,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "EC RX parser engine 7 error - end of packet in idle",
		.id = AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_IN_EOP_IN_IDLE_7,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_EPE_GROUP_B_EC_ERR_RPE_IN_EOP_IN_IDLE_7,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX multi parser - FIFO 0 error in end of packet",
		.id = AL_ERR_EVENTS_ETH_EPE_HDR_INPUT_FIFO_ERR_IN_EOP_IN_IDLE_0,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_EPE_GROUP_B_HDR_INPUT_FIFO_ERR_IN_EOP_IN_IDLE_0,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX multi parser - FIFO 1 error in end of packet",
		.id = AL_ERR_EVENTS_ETH_EPE_HDR_INPUT_FIFO_ERR_IN_EOP_IN_IDLE_1,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_EPE_GROUP_B_HDR_INPUT_FIFO_ERR_IN_EOP_IN_IDLE_1,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX multi parser - FIFO 2 error in end of packet",
		.id = AL_ERR_EVENTS_ETH_EPE_HDR_INPUT_FIFO_ERR_IN_EOP_IN_IDLE_2,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_EPE_GROUP_B_HDR_INPUT_FIFO_ERR_IN_EOP_IN_IDLE_2,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX multi parser - FIFO 3 error in end of packet",
		.id = AL_ERR_EVENTS_ETH_EPE_HDR_INPUT_FIFO_ERR_IN_EOP_IN_IDLE_3,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_ETH_EPE_GROUP_B_HDR_INPUT_FIFO_ERR_IN_EOP_IN_IDLE_3,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},

	{
		.name = "EC RX parser engine 0 error start of packet",
		.id = AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_IN_SOP_IN_PKT_0,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_EPE_GROUP_C_EC_ERR_RPE_IN_SOP_IN_PKT_0,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "EC RX parser engine 1 error start of packet",
		.id = AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_IN_SOP_IN_PKT_1,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_EPE_GROUP_C_EC_ERR_RPE_IN_SOP_IN_PKT_1,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "EC RX parser engine 2 error start of packet",
		.id = AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_IN_SOP_IN_PKT_2,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_EPE_GROUP_C_EC_ERR_RPE_IN_SOP_IN_PKT_2,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "EC RX parser engine 3 error start of packet",
		.id = AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_IN_SOP_IN_PKT_3,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_EPE_GROUP_C_EC_ERR_RPE_IN_SOP_IN_PKT_3,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "EC RX parser engine 4 error start of packet",
		.id = AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_IN_SOP_IN_PKT_4,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_EPE_GROUP_C_EC_ERR_RPE_IN_SOP_IN_PKT_4,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "EC RX parser engine 5 error start of packet",
		.id = AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_IN_SOP_IN_PKT_5,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_EPE_GROUP_C_EC_ERR_RPE_IN_SOP_IN_PKT_5,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "EC RX parser engine 6 error start of packet",
		.id = AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_IN_SOP_IN_PKT_6,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_EPE_GROUP_C_EC_ERR_RPE_IN_SOP_IN_PKT_6,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "EC RX parser engine 7 error start of packet",
		.id = AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_IN_SOP_IN_PKT_7,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_EPE_GROUP_C_EC_ERR_RPE_IN_SOP_IN_PKT_7,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "RMP Header input FIFO 0 error start of packet",
		.id = AL_ERR_EVENTS_ETH_EPE_HDR_INPUT_FIFO_ERR_IN_SOP_IN_PKT_0,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_EPE_GROUP_C_HDR_INPUT_FIFO_ERR_IN_SOP_IN_PKT_0,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "RMP Header input FIFO 1 error start of packet",
		.id = AL_ERR_EVENTS_ETH_EPE_HDR_INPUT_FIFO_ERR_IN_SOP_IN_PKT_1,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_EPE_GROUP_C_HDR_INPUT_FIFO_ERR_IN_SOP_IN_PKT_1,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "RMP Header input FIFO 2 error start of packet",
		.id = AL_ERR_EVENTS_ETH_EPE_HDR_INPUT_FIFO_ERR_IN_SOP_IN_PKT_2,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_EPE_GROUP_C_HDR_INPUT_FIFO_ERR_IN_SOP_IN_PKT_2,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "RMP Header input FIFO 3 error start of packet",
		.id = AL_ERR_EVENTS_ETH_EPE_HDR_INPUT_FIFO_ERR_IN_SOP_IN_PKT_3,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_ETH_EPE_GROUP_C_HDR_INPUT_FIFO_ERR_IN_SOP_IN_PKT_3,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},

	{
		.name = "RX parser engine 0 error end of packet header empty",
		.id = AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_OUT_EOP_HDR_EMPTY_0,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_D,
		.iofic_bit = AL_ETH_EPE_GROUP_D_EC_ERR_RPE_OUT_EOP_HDR_EMPTY_0,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX parser engine 1 error end of packet header empty",
		.id = AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_OUT_EOP_HDR_EMPTY_1,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_D,
		.iofic_bit = AL_ETH_EPE_GROUP_D_EC_ERR_RPE_OUT_EOP_HDR_EMPTY_1,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX parser engine 2 error end of packet header empty",
		.id = AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_OUT_EOP_HDR_EMPTY_2,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_D,
		.iofic_bit = AL_ETH_EPE_GROUP_D_EC_ERR_RPE_OUT_EOP_HDR_EMPTY_2,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX parser engine 3 error end of packet header empty",
		.id = AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_OUT_EOP_HDR_EMPTY_3,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_D,
		.iofic_bit = AL_ETH_EPE_GROUP_D_EC_ERR_RPE_OUT_EOP_HDR_EMPTY_3,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX parser engine 4 error end of packet header empty",
		.id = AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_OUT_EOP_HDR_EMPTY_4,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_D,
		.iofic_bit = AL_ETH_EPE_GROUP_D_EC_ERR_RPE_OUT_EOP_HDR_EMPTY_4,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX parser engine 5 error end of packet header empty",
		.id = AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_OUT_EOP_HDR_EMPTY_5,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_D,
		.iofic_bit = AL_ETH_EPE_GROUP_D_EC_ERR_RPE_OUT_EOP_HDR_EMPTY_5,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX parser engine 6 error end of packet header empty",
		.id = AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_OUT_EOP_HDR_EMPTY_6,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_D,
		.iofic_bit = AL_ETH_EPE_GROUP_D_EC_ERR_RPE_OUT_EOP_HDR_EMPTY_6,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "RX parser engine 7 error end of packet header empty",
		.id = AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_OUT_EOP_HDR_EMPTY_7,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_D,
		.iofic_bit = AL_ETH_EPE_GROUP_D_EC_ERR_RPE_OUT_EOP_HDR_EMPTY_7,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "RMP header input FIFO 0 error end of packet heade empty",
		.id = AL_ERR_EVENTS_ETH_EPE_HDR_INPUT_FIFO_ERR_OUT_EOP_HDR_EMPTY_0,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_D,
		.iofic_bit = AL_ETH_EPE_GROUP_D_HDR_INPUT_FIFO_ERR_OUT_EOP_HDR_EMPTY_0,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "RMP header input FIFO 1 error end of packet heade empty",
		.id = AL_ERR_EVENTS_ETH_EPE_HDR_INPUT_FIFO_ERR_OUT_EOP_HDR_EMPTY_1,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_D,
		.iofic_bit = AL_ETH_EPE_GROUP_D_HDR_INPUT_FIFO_ERR_OUT_EOP_HDR_EMPTY_1,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "RMP header input FIFO 2 error end of packet heade empty",
		.id = AL_ERR_EVENTS_ETH_EPE_HDR_INPUT_FIFO_ERR_OUT_EOP_HDR_EMPTY_2,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_D,
		.iofic_bit = AL_ETH_EPE_GROUP_D_HDR_INPUT_FIFO_ERR_OUT_EOP_HDR_EMPTY_2,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "RMP header input FIFO 3 error end of packet heade empty",
		.id = AL_ERR_EVENTS_ETH_EPE_HDR_INPUT_FIFO_ERR_OUT_EOP_HDR_EMPTY_3,
		.iofic_id = 0,
		.iofic_group = AL_INT_GROUP_D,
		.iofic_bit = AL_ETH_EPE_GROUP_D_HDR_INPUT_FIFO_ERR_OUT_EOP_HDR_EMPTY_3,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
};

/*******************************************************************************
 ** EC
 ******************************************************************************/
static int err_events_eth_epe_collect(struct al_mod_err_events_module *module,
				     enum al_mod_err_events_collect collect)
{
	int ret;
	unsigned int iofic_group;
	al_mod_bool should_collect = AL_FALSE;
	struct al_mod_err_events_eth_epe_data *epe_data = module->private_data;

	if (collect == AL_ERR_EVENTS_COLLECT_POLLING) {
		should_collect = AL_TRUE;
	} else if (collect == AL_ERR_EVENTS_COLLECT_INTERRUPT) {
		uint32_t cause = al_mod_iofic_read_cause(epe_data->ec_intc_regs_base, AL_INT_GROUP_A);
		should_collect = !!(cause & AL_ETH_V4_EC_IOFIC_GROUP_A_RMP_INT);
	}

	if (!should_collect)
		return 0;

	for (iofic_group = AL_INT_GROUP_A; iofic_group <= AL_INT_GROUP_D; iofic_group++) {
		ret = al_mod_err_events_common_module_iofic_collect(module, collect, 0, iofic_group);
		if (ret)
			return ret;
	}

	if (collect == AL_ERR_EVENTS_COLLECT_INTERRUPT)
		al_mod_iofic_clear_cause(epe_data->ec_intc_regs_base,
				AL_INT_GROUP_A,
				AL_ETH_V4_EC_IOFIC_GROUP_A_RMP_INT);

	return 0;
}

int al_mod_err_events_eth_epe_init(struct al_mod_err_events_handle *handle,
			      struct al_mod_err_events_eth_epe_data *data,
			      struct al_mod_err_events_eth_epe_init_params *params)
{
	uint32_t a_mask, b_mask;
	uint32_t c_mask, d_mask;
	struct al_mod_err_events_module *module;
	struct al_mod_err_events_common_module_params module_params;

	al_mod_assert(handle);
	al_mod_assert(data);
	al_mod_assert(params);
	al_mod_assert(sizeof(al_mod_err_events_eth_epe_fields) == sizeof(data->fields));
	al_mod_assert(sizeof(al_mod_err_events_eth_epe_fields) ==
		 (AL_ERR_EVENTS_ETH_EPE_MAX_ERRORS * sizeof(struct al_mod_err_events_field)));
	al_mod_assert(params->handle_params);
	al_mod_assert(params->ec_intc_regs_base);

	al_mod_memset(&module_params, 0, sizeof(module_params));

	module = &data->module;
	data->ec_intc_regs_base = params->ec_intc_regs_base;

	al_mod_eth_epe_handle_init(&data->handle, params->handle_params);

	/* init module */
	al_mod_sprintf(module_params.name, "ETH-EPE");
	module_params.primary_module = AL_ERR_EVENTS_MODULE_ETH;
	module_params.secondary_module = AL_ERR_EVENTS_SEC_MODULE_ETH_EPE;
	module_params.primary_index = 0;
	module_params.secondary_index = 0;
	module_params.collect_mode = params->collect_mode;
	module_params.ic_regs_bases[0] = data->handle.ints_base;
	module_params.ic_size = 1;
	module_params.fields_size = AL_ERR_EVENTS_ETH_EPE_MAX_ERRORS;
	module_params.fields = data->fields;
	module_params.fields_template = al_mod_err_events_eth_epe_fields;
	module_params.private_data = data;
	module_params.collect = err_events_eth_epe_collect;
	module_params.print = al_mod_err_events_common_module_print;
	module_params.clear = al_mod_err_events_common_module_clear;
	module_params.test = al_mod_err_events_common_module_test;
	module_params.set_cause = al_mod_err_events_common_field_set_cause;
	module_params.get_attrs = al_mod_err_events_common_field_get_attrs;

	al_mod_err_events_common_module_init(handle, module, &module_params);

	al_mod_eth_epe_error_ints_masks_get(&data->handle,
				       &a_mask,
				       &b_mask,
				       &c_mask,
				       &d_mask);

	al_mod_err_event_common_update_valid_fields(module, 0,
						a_mask,
						b_mask,
						c_mask,
						d_mask);

	al_mod_err_events_common_module_post_init(module);

	return 0;
}
