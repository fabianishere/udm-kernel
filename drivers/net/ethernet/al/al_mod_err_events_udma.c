/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#include "al_mod_err_events_internal.h"
#include "al_mod_err_events_udma.h"
#include "al_mod_hal_udma_iofic.h"

/*******************************************************************************
 ** Static error fields
 ******************************************************************************/
static const struct al_mod_err_events_field al_mod_err_events_udma_fields[AL_ERR_EVENTS_UDMA_MAX_ERRORS] = {
	{
		.name = "M2S MSIX Response Error",
		.id = AL_ERR_EVENTS_UDMA_M2S_MSIX_RESP_ERR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_INT_2ND_GROUP_A_M2S_MSIX_RESP,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "M2S MSIX Timeout Error",
		.id = AL_ERR_EVENTS_UDMA_M2S_MSIX_TIMEOUT,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_INT_2ND_GROUP_A_M2S_MSIX_TO,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "M2S Pre-fetch Header Parity",
		.id = AL_ERR_EVENTS_UDMA_M2S_PREFETCH_HEADER_PARITY,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_INT_2ND_GROUP_A_M2S_PREFETCH_HDR_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "M2S Pre-fetch Descriptor Parity",
		.id = AL_ERR_EVENTS_UDMA_M2S_PREFETCH_DESC_PARITY,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_INT_2ND_GROUP_A_M2S_PREFETCH_DESC_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "M2S Data buffer Parity",
		.id = AL_ERR_EVENTS_UDMA_M2S_DATA_BUF_PARITY,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_INT_2ND_GROUP_A_M2S_DATA_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "M2S Data header Parity",
		.id = AL_ERR_EVENTS_UDMA_M2S_DATA_HEADER_PARITY,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_INT_2ND_GROUP_A_M2S_HDR_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "M2S Completion Coalescing Parity",
		.id = AL_ERR_EVENTS_UDMA_M2S_COMPL_COAL_PARITY,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_INT_2ND_GROUP_A_M2S_COMPL_COAL_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "M2S Unack Packets Parity",
		.id = AL_ERR_EVENTS_UDMA_M2S_UNACK_PACKETS_PARITY,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_INT_2ND_GROUP_A_M2S_UNACK_PKT_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "M2S Ack Packets Parity",
		.id = AL_ERR_EVENTS_UDMA_M2S_ACK_PACKETS_PARITY,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_INT_2ND_GROUP_A_M2S_ACK_PKT_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "M2S AXI data Parity",
		.id = AL_ERR_EVENTS_UDMA_M2S_AXI_DATA_PARITY,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_INT_2ND_GROUP_A_M2S_AXI_DATA_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "M2S Pre-fetch Ring ID Error",
		.id = AL_ERR_EVENTS_UDMA_M2S_PREFETCH_RING_ID_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_INT_2ND_GROUP_A_M2S_PREFETCH_RING_ID,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "M2S Pre-fetch Last Error",
		.id = AL_ERR_EVENTS_UDMA_M2S_PREFETCH_LAST_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_INT_2ND_GROUP_A_M2S_PREFETCH_LAST,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "M2S Pre-fetch First Error",
		.id = AL_ERR_EVENTS_UDMA_M2S_PREFETCH_FIRST_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_INT_2ND_GROUP_A_M2S_PREFETCH_FIRST,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "M2S Pre-fetch Max Descriptors Error",
		.id = AL_ERR_EVENTS_UDMA_M2S_PREFETCH_MAX_DESC_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_INT_2ND_GROUP_A_M2S_PREFETCH_MAX_DESC,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "M2S Max Packet Length Error",
		.id = AL_ERR_EVENTS_UDMA_M2S_MAX_PACKET_LENGTH_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_INT_2ND_GROUP_A_M2S_PKT_LEN,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "M2S Pre-fetch AXI Timeout",
		.id = AL_ERR_EVENTS_UDMA_M2S_PREFETCH_AXI_TIMEOUT,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_INT_2ND_GROUP_A_M2S_PREFETCH_AXI_TO,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "M2S Pre-fetch AXI Response Error",
		.id = AL_ERR_EVENTS_UDMA_M2S_PREFETCH_AXI_RESP_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_INT_2ND_GROUP_A_M2S_PREFETCH_AXI_RESP,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "M2S Pre-fetch AXI Parity",
		.id = AL_ERR_EVENTS_UDMA_M2S_PREFETCH_AXI_PARITY,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_INT_2ND_GROUP_A_M2S_PREFETCH_AXI_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "M2S Data AXI Timeout",
		.id = AL_ERR_EVENTS_UDMA_M2S_DATA_AXI_TIMEOUT,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_INT_2ND_GROUP_A_M2S_DATA_AXI_TO,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "M2S Data AXI Parity",
		.id = AL_ERR_EVENTS_UDMA_M2S_DATA_AXI_PARITY,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_INT_2ND_GROUP_A_M2S_DATA_AXI_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "M2S Data AXI Response Error",
		.id = AL_ERR_EVENTS_UDMA_M2S_DATA_AXI_RESP_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_INT_2ND_GROUP_A_M2S_DATA_AXI_RESP,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "M2S Completion AXI Timeout",
		.id = AL_ERR_EVENTS_UDMA_M2S_COMPLETION_AXI_TIMEOUT,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_INT_2ND_GROUP_A_M2S_COMPL_AXI_TO,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "M2S Completion AXI Response Error",
		.id = AL_ERR_EVENTS_UDMA_M2S_COMPLETION_AXI_RESP_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_INT_2ND_GROUP_A_M2S_COMPL_AXI_RESP,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "M2S AXI completion Parity",
		.id = AL_ERR_EVENTS_UDMA_M2S_COMPLETION_AXI_PARITY,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_INT_2ND_GROUP_A_M2S_COMP_AXI_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "M2S Stream Timeout",
		.id = AL_ERR_EVENTS_UDMA_M2S_STREAM_TIMEOUT,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_INT_2ND_GROUP_A_M2S_STRM_TO,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "M2S Stream Response Error",
		.id = AL_ERR_EVENTS_UDMA_M2S_STREAM_RESP_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_INT_2ND_GROUP_A_M2S_STRM_RESP,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "M2S Stream Parity",
		.id = AL_ERR_EVENTS_UDMA_M2S_STREAM_PARITY,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_INT_2ND_GROUP_A_M2S_STRM_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "M2S Stream Completion Mismatch Error",
		.id = AL_ERR_EVENTS_UDMA_M2S_STREAM_COMPL_MISMATCH_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_INT_2ND_GROUP_A_M2S_STRM_COMPL_MISMATCH,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "S2M Transaction table info Parity",
		.id = AL_ERR_EVENTS_UDMA_S2M_TRNSCTN_TBL_INFO_PARITY,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_INT_2ND_GROUP_B_S2M_TRNSCTN_TBL_INFO_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "S2M Prefetch Descriptor Parity",
		.id = AL_ERR_EVENTS_UDMA_S2M_PREFETCH_DESC_PARITY,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_INT_2ND_GROUP_B_S2M_PREFETCH_DESC_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "S2M Completion Coalescing Parity",
		.id = AL_ERR_EVENTS_UDMA_S2M_COMPLETION_COAL_PARITY,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_INT_2ND_GROUP_B_S2M_COMPL_COAL_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "S2M Pre Unack Packets Parity",
		.id = AL_ERR_EVENTS_UDMA_S2M_PRE_UNACK_PACKETS_PARITY,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_INT_2ND_GROUP_B_S2M_PRE_UNACK_PKT_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "S2M Unack Packets Parity",
		.id = AL_ERR_EVENTS_UDMA_S2M_UNACK_PACKETS_PARITY,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_INT_2ND_GROUP_B_S2M_UNACK_PKT_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "S2M Data buffer Parity",
		.id = AL_ERR_EVENTS_UDMA_S2M_DATA_BUF_PARITY,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_INT_2ND_GROUP_B_S2M_DATA_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "S2M Data header Parity",
		.id = AL_ERR_EVENTS_UDMA_S2M_DATA_HEADER_PARITY,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_INT_2ND_GROUP_B_S2M_DATA_HDR_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "S2M Packet Length Error",
		.id = AL_ERR_EVENTS_UDMA_S2M_PACKET_LENGTH_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_INT_2ND_GROUP_B_S2M_PKT_LEN,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "S2M Stream Last Error",
		.id = AL_ERR_EVENTS_UDMA_S2M_STREAM_LAST_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_INT_2ND_GROUP_B_S2M_STRM_LAST,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "S2M Stream First Error",
		.id = AL_ERR_EVENTS_UDMA_S2M_STREAM_FIRST_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_INT_2ND_GROUP_B_S2M_STRM_FIRST,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "S2M Stream Data Error",
		.id = AL_ERR_EVENTS_UDMA_S2M_STREAM_DATA_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_INT_2ND_GROUP_B_S2M_STRM_DATA,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "S2M Stream Data Parity",
		.id = AL_ERR_EVENTS_UDMA_S2M_STREAM_DATA_PARITY,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_INT_2ND_GROUP_B_S2M_STRM_DATA_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "S2M Stream Header Error",
		.id = AL_ERR_EVENTS_UDMA_S2M_STREAM_HEADER_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_INT_2ND_GROUP_B_S2M_STRM_HDR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "S2M Stream Header Parity",
		.id = AL_ERR_EVENTS_UDMA_S2M_STREAM_HEADER_PARITY,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_INT_2ND_GROUP_B_S2M_STRM_HDR_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "S2M Completion Unack Timeout",
		.id = AL_ERR_EVENTS_UDMA_S2M_COMPLETION_UNACK_TIMEOUT,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_INT_2ND_GROUP_B_S2M_COMPL_UNACK,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "S2M Compleation Stream Timeout",
		.id = AL_ERR_EVENTS_UDMA_S2M_COMPLETION_STREAM_TIMEOUT,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_INT_2ND_GROUP_B_S2M_COMPL_STRM,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "S2M Completion AXI Timeout",
		.id = AL_ERR_EVENTS_UDMA_S2M_COMPLETION_AXI_TIMEOUT,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_INT_2ND_GROUP_B_S2M_COMPL_AXI_TO,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "S2M Completion AXI Response Error",
		.id = AL_ERR_EVENTS_UDMA_S2M_COMPLETION_AXI_RESP_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_INT_2ND_GROUP_B_S2M_COMPL_AXI_RESP,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "S2M Completion AXI Parity",
		.id = AL_ERR_EVENTS_UDMA_S2M_COMPLETION_AXI_PARITY,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_INT_2ND_GROUP_B_S2M_COMPL_AXI_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "S2M Pre-fetch Ring ID error",
		.id = AL_ERR_EVENTS_UDMA_S2M_PREFETCH_RING_ID_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_INT_2ND_GROUP_B_S2M_PREFETCH_RING_ID,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "S2M Pre-fetch AXI Timeout",
		.id = AL_ERR_EVENTS_UDMA_S2M_PREFETCH_AXI_TIMEOUT,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_INT_2ND_GROUP_B_S2M_PREFETCH_AXI_TO,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "S2M Pre-fetch AXI Response Error",
		.id = AL_ERR_EVENTS_UDMA_S2M_PREFETCH_AXI_RESP_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_INT_2ND_GROUP_B_S2M_PREFETCH_AXI_RESP,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "S2M Pre-fetch AXI Parity",
		.id = AL_ERR_EVENTS_UDMA_S2M_PREFETCH_AXI_PARITY,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_INT_2ND_GROUP_B_S2M_PREFETCH_AXI_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "S2M No Descriptors Timeout",
		.id = AL_ERR_EVENTS_UDMA_S2M_NO_DESCRIPTORS_TIMEOUT,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_INT_2ND_GROUP_B_S2M_NO_DESC_TO,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "S2M Data AXI Timeout",
		.id = AL_ERR_EVENTS_UDMA_S2M_DATA_AXI_TIMEOUT,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_INT_2ND_GROUP_B_S2M_DATA_AXI_TO,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "S2M Data AXI Response Error",
		.id = AL_ERR_EVENTS_UDMA_S2M_DATA_AXI_RESP_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_INT_2ND_GROUP_B_S2M_DATA_AXI_RESP,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "S2M Data AXI Parity",
		.id = AL_ERR_EVENTS_UDMA_S2M_DATA_AXI_PARITY,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_INT_2ND_GROUP_B_S2M_DATA_AXI_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "S2M AXI request fifo descriptor prefetch Parity",
		.id = AL_ERR_EVENTS_UDMA_S2M_PRE_AXI_DESC_REQ_PARITY,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_INT_2ND_GROUP_C_S2M_DESC_PRE_AXI_REQ_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "S2M AXI command fifo descriptor prefetch Parity",
		.id = AL_ERR_EVENTS_UDMA_S2M_PRE_AXI_DESC_CMD_PARITY,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_INT_2ND_GROUP_C_S2M_DESC_PRE_AXI_CMD_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "S2M AXI write completion request fifo Parity",
		.id = AL_ERR_EVENTS_UDMA_S2M_COMPLETION_AXI_WR_REQ_PARITY,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_INT_2ND_GROUP_C_S2M_CMP_WR_AXI_REQ_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "S2M AXI write completion data fifo Parity",
		.id = AL_ERR_EVENTS_UDMA_S2M_COMPLETION_AXI_WR_DATA_PARITY,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_INT_2ND_GROUP_C_S2M_CMP_WR_AXI_DATA_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "S2M AXI data write request fifo Parity",
		.id = AL_ERR_EVENTS_UDMA_S2M_DATA_AXI_WR_REQ_PARITY,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_INT_2ND_GROUP_C_S2M_DATA_WR_AXI_REQ_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "S2M AXI data write data fifo Parity",
		.id = AL_ERR_EVENTS_UDMA_S2M_DATA_AXI_WR_DATA_PARITY,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_INT_2ND_GROUP_C_S2M_DATA_WR_AXI_DATA_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "M2S AXI write completion request fifo Parity",
		.id = AL_ERR_EVENTS_UDMA_M2S_COMPLETION_AXI_WR_REQ_PARITY,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_INT_2ND_GROUP_C_M2S_CMP_WR_AXI_REQ_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "M2S AXI write completion command fifo Parity",
		.id = AL_ERR_EVENTS_UDMA_M2S_COMPLETION_AXI_WR_CMD_PARITY,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_INT_2ND_GROUP_C_M2S_CMP_WR_AXI_CMD_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "M2S AXI descriptor prefetch request fifo Parity",
		.id = AL_ERR_EVENTS_UDMA_M2S_PREFETCH_AXI_DESC_REQ_PARITY,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_INT_2ND_GROUP_C_M2S_DESC_PRE_AXI_REQ_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "M2S AXI descriptor prefetch command fifo Parity",
		.id = AL_ERR_EVENTS_UDMA_M2S_PREFETCH_AXI_DESC_CMD_PARITY,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_INT_2ND_GROUP_C_M2S_DESC_PRE_AXI_CMD_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "M2S AXI data fetch request fifo Parity",
		.id = AL_ERR_EVENTS_UDMA_M2S_FETCH_AXI_DATA_REQ_PARITY,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_INT_2ND_GROUP_C_M2S_DATA_FTCH_AXI_REQ_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "M2S AXI data fetch command fifo Parity",
		.id = AL_ERR_EVENTS_UDMA_M2S_FETCH_AXI_DATA_CMD_PARITY,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_INT_2ND_GROUP_C_M2S_DATA_FTCH_AXI_CMD_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
};

/*******************************************************************************
 ** UDMA
 ******************************************************************************/
static int err_events_udma_collect(struct al_mod_err_events_module *module,
				   enum al_mod_err_events_collect collect)
{
	uint32_t cause;
	uint32_t bit;
	unsigned int group;
	uint32_t cause_clr = 0;
	struct al_mod_err_events_udma_data *data =
			(struct al_mod_err_events_udma_data *)module->private_data;
	al_mod_bool should_collect = AL_FALSE;

	cause =  al_mod_iofic_read_cause(data->iofic_primary_regs_base, AL_INT_GROUP_D);

	if (collect == AL_ERR_EVENTS_COLLECT_POLLING)
		should_collect = AL_TRUE;


	for (group = AL_INT_GROUP_A; group <= AL_INT_GROUP_C; group++) {
		bit = data->sec_level_int[group];

		if (collect == AL_ERR_EVENTS_COLLECT_INTERRUPT)
			should_collect = !!(cause & bit);

		if (!should_collect)
			continue;

		al_mod_err_events_common_module_iofic_collect(module, collect, 0, group);
		cause_clr |= bit;
	}

	if (collect == AL_ERR_EVENTS_COLLECT_INTERRUPT)
		al_mod_iofic_clear_cause(data->iofic_primary_regs_base, AL_INT_GROUP_D, cause_clr);

	return 0;
}

int al_mod_err_events_udma_init(struct al_mod_err_events_handle *handle,
			    struct al_mod_err_events_udma_data *data,
			    struct al_mod_err_events_udma_init_params *params)
{
	struct al_mod_err_events_common_module_params module_params;
	struct al_mod_udma udma;
	struct al_mod_udma_params udma_params;

	al_mod_assert(handle);
	al_mod_assert(data);
	al_mod_assert(params);
	al_mod_assert(sizeof(al_mod_err_events_udma_fields) == sizeof(data->fields));
	al_mod_assert(sizeof(al_mod_err_events_udma_fields) ==
		 (AL_ERR_EVENTS_UDMA_MAX_ERRORS * sizeof(struct al_mod_err_events_field)));

	al_mod_memset(&module_params, 0, sizeof(module_params));

	switch (params->primary_module) {
	case AL_ERR_EVENTS_MODULE_SSM:
		al_mod_sprintf(module_params.name,
			"SSM(%d)-UDMA(%d)",
			params->module_index,
			params->udma_index);
		break;
	case AL_ERR_EVENTS_MODULE_ETH:
		al_mod_sprintf(module_params.name,
			"ETH(%d)-UDMA(%d)",
			params->module_index,
			params->udma_index);
		break;
	default:
		return -EINVAL;
	}

	/* init UDMA handle */
	udma_params.name = module_params.name;
	udma_params.udma_regs_base = params->regs_base;
	/* Not interesting parameters for our use */
	udma_params.type = UDMA_TX;
	udma_params.num_of_queues = 1;

	al_mod_udma_handle_init(&udma, &udma_params);

	/* init data */
	data->iofic_primary_regs_base = al_mod_udma_iofic_reg_base_get_adv(&udma,
								   AL_UDMA_IOFIC_LEVEL_PRIMARY);
	data->ext_app_int = al_mod_udma_iofic_get_ext_app_bit(&udma);
	data->sec_level_int[AL_INT_GROUP_A] =
		al_mod_udma_iofic_sec_level_int_get(&udma, AL_INT_GROUP_A);
	data->sec_level_int[AL_INT_GROUP_B] =
		al_mod_udma_iofic_sec_level_int_get(&udma, AL_INT_GROUP_B);
	if (al_mod_udma_rev_id_get(&udma) >= AL_UDMA_REV_ID_4)
		data->sec_level_int[AL_INT_GROUP_C] =
			al_mod_udma_iofic_sec_level_int_get(&udma, AL_INT_GROUP_C);
	else
		data->sec_level_int[AL_INT_GROUP_C] = 0;
	data->sec_level_int[AL_INT_GROUP_D] = 0;

	/* init module */
	module_params.primary_module = params->primary_module;
	module_params.secondary_module = AL_ERR_EVENTS_SEC_MODULE_UDMA;
	module_params.primary_index = params->module_index;
	module_params.secondary_index = params->udma_index;
	module_params.collect_mode = params->collect_mode;
	module_params.ic_regs_bases[0] = al_mod_udma_iofic_reg_base_get_adv(&udma,
								AL_UDMA_IOFIC_LEVEL_SECONDARY);
	module_params.ic_size = 1;
	module_params.fields_size = AL_ERR_EVENTS_UDMA_MAX_ERRORS;
	module_params.fields = data->fields;
	module_params.fields_template = al_mod_err_events_udma_fields;
	module_params.private_data = data;
	module_params.collect = err_events_udma_collect;
	module_params.print = al_mod_err_events_common_module_print;
	module_params.clear = al_mod_err_events_common_module_clear;
	module_params.test = al_mod_err_events_common_module_test;
	module_params.set_cause = al_mod_err_events_common_field_set_cause;
	module_params.get_attrs = al_mod_err_events_common_field_get_attrs;

	al_mod_err_events_common_module_init(handle, &data->module, &module_params);

	if (params->primary_module == AL_ERR_EVENTS_MODULE_ETH)
		data->module.fields[AL_ERR_EVENTS_UDMA_S2M_NO_DESCRIPTORS_TIMEOUT].valid = AL_FALSE;

	/** Invalidate bits of Eth UDMA rev 4 */
	if ((params->primary_module == AL_ERR_EVENTS_MODULE_SSM) ||
		(al_mod_udma_rev_id_get(&udma) < AL_UDMA_REV_ID_4)) {

		unsigned int i;
		enum al_mod_err_events_udma  ids[] = {
			AL_ERR_EVENTS_UDMA_S2M_TRNSCTN_TBL_INFO_PARITY,
			AL_ERR_EVENTS_UDMA_S2M_PRE_AXI_DESC_REQ_PARITY,
			AL_ERR_EVENTS_UDMA_S2M_PRE_AXI_DESC_CMD_PARITY,
			AL_ERR_EVENTS_UDMA_S2M_COMPLETION_AXI_WR_REQ_PARITY,
			AL_ERR_EVENTS_UDMA_S2M_COMPLETION_AXI_WR_DATA_PARITY,
			AL_ERR_EVENTS_UDMA_S2M_DATA_AXI_WR_REQ_PARITY,
			AL_ERR_EVENTS_UDMA_S2M_DATA_AXI_WR_DATA_PARITY,
			AL_ERR_EVENTS_UDMA_M2S_COMPLETION_AXI_WR_REQ_PARITY,
			AL_ERR_EVENTS_UDMA_M2S_COMPLETION_AXI_WR_CMD_PARITY,
			AL_ERR_EVENTS_UDMA_M2S_PREFETCH_AXI_DESC_REQ_PARITY,
			AL_ERR_EVENTS_UDMA_M2S_PREFETCH_AXI_DESC_CMD_PARITY,
			AL_ERR_EVENTS_UDMA_M2S_FETCH_AXI_DATA_REQ_PARITY,
			AL_ERR_EVENTS_UDMA_M2S_FETCH_AXI_DATA_CMD_PARITY
		};

		for (i = 0; i < AL_ARR_SIZE(ids); i++)
				data->module.fields[ids[i]].valid = AL_FALSE;
	}

	/*
	 * Addressing RMN: 12057
	 *
	 * RMN description:
	 * Compression false interrupt assertion aborts SSMAE
	 * During AL7 Compression Device driver development and test with Alpine Linux, DMA hang was found.
	 * Specific compression/decompression falsely triggers an interrupt.The interrupt propagates to
	 * application status bits in the S2M completion, and from there to the DMA secondary interrupt
	 * controller, issuing an abort.
	 * Software flow:
	 * Mask the relevant interrupts:
	 * AL_INT_2ND_GROUP_A_M2S_STRM_RESP
	 * AL_INT_2ND_GROUP_B_S2M_STRM_DATA
	 */
	if ((params->primary_module == AL_ERR_EVENTS_MODULE_SSM) &&
		(al_mod_udma_rev_id_get(&udma) == AL_UDMA_REV_ID_4)) {
		unsigned int i;
		enum al_mod_err_events_udma ids[] = {
			AL_ERR_EVENTS_UDMA_M2S_STREAM_RESP_ERROR,
			AL_ERR_EVENTS_UDMA_S2M_STREAM_DATA_ERROR
		};

		for (i = 0; i < AL_ARR_SIZE(ids); i++)
			data->module.fields[ids[i]].valid = AL_FALSE;
	}

	al_mod_err_events_common_module_post_init(&data->module);

	return 0;
}

al_mod_bool al_mod_err_events_udma_ext_app_int_read_and_clear(struct al_mod_err_events_udma_data *data)
{
	uint32_t cause;

	cause = al_mod_iofic_read_and_clear_cause(data->iofic_primary_regs_base,
					      AL_INT_GROUP_D,
					      data->ext_app_int);

	if (cause & data->ext_app_int)
		return AL_TRUE;

	return AL_FALSE;
}
