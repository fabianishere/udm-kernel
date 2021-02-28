/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#include "al_err_events_internal.h"
#include "al_err_events_ssm.h"
#include "al_hal_udma_iofic.h"
#include "al_hal_ssm_crypto.h"
#include "al_hal_ssm_raid.h"
#include "al_hal_ssm_crc_memcpy.h"

/*******************************************************************************
 ** Static error fields
 ******************************************************************************/
static const struct al_err_events_field
	al_err_events_ssm_crypto_fields[AL_ERR_EVENTS_SSM_CRYPTO_MAX_ERRORS] = {
	{
		.name = "S2M Timeout",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_S2M_TIMEOUT,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_CRYPTO_APP_INT_A_S2M_TIMEOUT,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "M2S Timeout",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_M2S_TIMEOUT,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_CRYPTO_APP_INT_A_M2S_TIMEOUT,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "EOP Without SOP Error",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_EOP_WITHOUT_SOP_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_CRYPTO_APP_INT_A_EOP_WITHOUT_SOP,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "SOP Without EOP Error",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_SOP_WITHOUT_EOP_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_CRYPTO_APP_INT_A_SOP_WITHOUT_EOP,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "SOP With EOP Togther Error",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_SOP_WITH_EOP_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_CRYPTO_APP_INT_A_SOP_WITH_EOP_TOGETHER,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Packet Protocol Violation",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_UNMAP_PROTOCOL_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_CRYPTO_APP_INT_A_UNMAP_PROTOCOL,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "FIFO Overrun SA Error",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_FIFO_OVERRUN_SA_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_SA,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "FIFO Overrun IV Error",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_FIFO_OVERRUN_IV_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_IV,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "FIFO Overrun ICV Error",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_FIFO_OVERRUN_ICV_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_ICV,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "FIFO Overrun DMB Function 0 Error",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_FIFO_OVERRUN_DMB_FUNC_0_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_DMB_FUNC_0,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "FIFO Overrun DMB Function 1 Error",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_FIFO_OVERRUN_DMB_FUNC_1_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_DMB_FUNC_1,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "FIFO Overrun DMB Function 2 Error",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_FIFO_OVERRUN_DMB_FUNC_2_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_DMB_FUNC_2,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "FIFO Overrun DMB Function 3 Error",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_FIFO_OVERRUN_DMB_FUNC_3_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_DMB_FUNC_3,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "FIFO Overrun E2M Payload Error",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_FIFO_OVERRUN_E2M_PAYLOAD_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_E2M_PAYLOAD,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "FIFO Overrun E2M Signature Error",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_FIFO_OVERRUN_E2M_SIGNATURE_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_E2M_SIGNATURE,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "FIFO Overrun D2E Data Error",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_FIFO_OVERRUN_D2E_DATA_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_D2E_DATA,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "FIFO Overrun D2E Control Error",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_FIFO_OVERRUN_D2E_CONTROL_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_D2E_CONTROL,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "FIFO Overrun Ingress Error",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_FIFO_OVERRUN_INGRESS_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_INGRESS,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "FIFO Overrun Aligner Error",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_FIFO_OVERRUN_ALIGNER_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_ALIGNER,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "FIFO Overrun D2E Context Error",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_FIFO_OVERRUN_D2E_CONTEXT_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_D2E_CONTEXT,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "FIFO Overrun D2E Hash State Error",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_FIFO_OVERRUN_D2E_HASH_STATE_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_D2E_HASH_ST,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "FIFO Overrun D2E OPAD Error",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_FIFO_OVERRUN_D2E_OPAD_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_D2E_OPAD,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "FIFO Overrun E2D WB Signature Error",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_FIFO_OVERRUN_E2D_WB_SIGNATURE_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_E2D_WRITE_B,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "FIFO Overrun E2M INTR Error",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_FIFO_OVERRUN_E2M_INTR_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_E2M_INTR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "FIFO Overrun D2E GMAC state Error",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_FIFO_OVERRUN_D2E_GMAC_STATE_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_D2E_GMAC_STATE,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "FIFO Overrun D2E compression history Error",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_FIFO_OVERRUN_D2E_CMPRS_HIST_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_D2E_COMP_HIST,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "FIFO Overrun D2E Huffman dynamic table Error",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_FIFO_OVERRUN_D2E_HUFFMAN_D_TBL_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_D2E_HUFFMAN_D_TBL,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "SA/IV FIFO Out Empty Error",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_SA_IV_FIFO_OUT_EMPTY_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_CRYPTO_APP_INT_B_SA_IV_FIFO_OUT_EMPTY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "M2S error",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_M2S_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_CRYPTO_APP_INT_B_M2S_ERROR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "SAD Memories Parity Error",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_SAD_MEM_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_CRYPTO_APP_INT_B_PAR_ERR_SAD_MEMORIES,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "SAD authentication IV in Parity Error",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_SAD_AUTH_IV_IN_PAR_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_CRYPTO_APP_INT_B_PAR_ERR_SAD_AUTH_IV_IN,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "SAD authentication IV out Parity Error",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_SAD_ENC_KEY_AUTH_IV_OUT_PAR_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_CRYPTO_APP_INT_B_PAR_ERR_SAD_ENC_KEY_AUTH_IV_OUT,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "SAD encryption parameters Parity Error",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_SAD_ENC_PARAMS_PAR_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_CRYPTO_APP_INT_B_PAR_ERR_SAD_ENC_PARAMS,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Decompression Parity Error - history 0",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_DECMPRS_PAR_ERROR_HIST0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_CRYPTO_APP_INT_C_PAR_ERR_DECOMP_HIST(0),
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Decompression Parity Error - history 1",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_DECMPRS_PAR_ERROR_HIST1,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_CRYPTO_APP_INT_C_PAR_ERR_DECOMP_HIST(1),
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Decompression Parity Error - history 2",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_DECMPRS_PAR_ERROR_HIST2,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_CRYPTO_APP_INT_C_PAR_ERR_DECOMP_HIST(2),
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Decompression Parity Error - history 3",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_DECMPRS_PAR_ERROR_HIST3,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_CRYPTO_APP_INT_C_PAR_ERR_DECOMP_HIST(3),
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Compression output controller compressed Parity Error",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_OUT_CONT_COMPRESSED_CMPRS_PAR_ERROR,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_CRYPTO_APP_INT_C_PAR_ERR_COMP_OUT_CONT_COMP,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Compression output controller original Parity Error",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_OUT_CONT_ORIG_CMPRS_PAR_ERROR,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_CRYPTO_APP_INT_C_PAR_ERR_COMP_OUT_CONT_ORIG,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Compression Parity Error - history 0",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_CMPRS_PAR_ERR_HIST0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_CRYPTO_APP_INT_C_PAR_ERR_COMP_HIST(0),
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Compression Parity Error - history 1",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_CMPRS_PAR_ERR_HIST1,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_CRYPTO_APP_INT_C_PAR_ERR_COMP_HIST(1),
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Compression Parity Error - history 2",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_CMPRS_PAR_ERR_HIST2,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_CRYPTO_APP_INT_C_PAR_ERR_COMP_HIST(2),
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Compression Parity Error - history 3",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_CMPRS_PAR_ERR_HIST3,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_CRYPTO_APP_INT_C_PAR_ERR_COMP_HIST(3),
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Compression Parity Error - history 4",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_CMPRS_PAR_ERR_HIST4,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_CRYPTO_APP_INT_C_PAR_ERR_COMP_HIST(4),
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Compression Parity Error - history 5",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_CMPRS_PAR_ERR_HIST5,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_CRYPTO_APP_INT_C_PAR_ERR_COMP_HIST(5),
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Compression Parity Error - history 6",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_CMPRS_PAR_ERR_HIST6,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_CRYPTO_APP_INT_C_PAR_ERR_COMP_HIST(6),
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Compression Parity Error - history 7",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_CMPRS_PAR_ERR_HIST7,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_CRYPTO_APP_INT_C_PAR_ERR_COMP_HIST(7),
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Compression Parity Error - history 8",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_CMPRS_PAR_ERR_HIST8,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_CRYPTO_APP_INT_C_PAR_ERR_COMP_HIST(8),
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Compression Parity Error - history 9",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_CMPRS_PAR_ERR_HIST9,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_CRYPTO_APP_INT_C_PAR_ERR_COMP_HIST(9),
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Compression Parity Error - history 10",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_CMPRS_PAR_ERR_HIST10,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_CRYPTO_APP_INT_C_PAR_ERR_COMP_HIST(10),
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Compression Parity Error - history 11",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_CMPRS_PAR_ERR_HIST11,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_CRYPTO_APP_INT_C_PAR_ERR_COMP_HIST(11),
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Compression Parity Error - history 12",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_CMPRS_PAR_ERR_HIST12,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_CRYPTO_APP_INT_C_PAR_ERR_COMP_HIST(12),
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Compression Parity Error - history 13",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_CMPRS_PAR_ERR_HIST13,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_CRYPTO_APP_INT_C_PAR_ERR_COMP_HIST(13),
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Compression Parity Error - history 14",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_CMPRS_PAR_ERR_HIST14,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_CRYPTO_APP_INT_C_PAR_ERR_COMP_HIST(14),
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Compression Parity Error - history 15",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_CMPRS_PAR_ERR_HIST15,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_CRYPTO_APP_INT_C_PAR_ERR_COMP_HIST(15),
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "LZ77/LZSS compression CAM entry not found",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_LZ77_LZSS_CAM_ENT_NOT_FOUND_CMPRS_ERROR,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_CRYPTO_APP_INT_C_LZ77_LZSS_COMP_CAM_ENT_NOT_FOUND,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "LZ77/LZSS compression invalid ALU command",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_LZ77_LZSS_INVALID_ALU_CMD_CMPRS_ERROR,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_CRYPTO_APP_INT_C_LZ77_LZSS_COMP_INVALID_ALU_CMD,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Compression history exceeded",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_HIST_EXCEEDED_CMPRS_ERROR,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_CRYPTO_APP_INT_C_COMP_HIST_EXCEEDED,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Decompression invalid field composition",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_INVALID_FIELD_COMP_DECMPRS_ERROR,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_CRYPTO_APP_INT_C_DECOMP_INVALID_FIELD_COMPOS,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Decompression invalid field size",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_INVALID_FIELD_SIZE_DECMPRS_ERROR,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_CRYPTO_APP_INT_C_DECOMP_INVALID_FIELD_SIZE,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Decompression offset not resolved",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_OFFSET_NOT_RESOLVED_DECMPRS_ERROR,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_CRYPTO_APP_INT_C_DECOMP_OFFSET_NOT_RESOLVED,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Decompression zero code word count",
		.id = AL_ERR_EVENTS_SSM_CRYPTO_ZERO_CODE_WORD_CNT_DECMPRS_ERROR,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_CRYPTO_APP_INT_C_DECOMP_ZERO_CODE_WORD_CNT,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
};

static const struct al_err_events_field
	al_err_events_ssm_raid_fields[AL_ERR_EVENTS_SSM_RAID_MAX_ERRORS] = {
	{
		.name = "S2M Timeout",
		.id = AL_ERR_EVENTS_SSM_RAID_S2M_TIMEOUT,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_RAID_APP_INT_A_S2M_TIMEOUT,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "M2S Timeout",
		.id = AL_ERR_EVENTS_SSM_RAID_M2S_TIMEOUT,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_RAID_APP_INT_A_M2S_TIMEOUT,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Command Decode Error",
		.id = AL_ERR_EVENTS_SSM_RAID_CMD_DECODE_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_RAID_APP_INT_A_CMD_DECODE_ERROR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Input Length Error",
		.id = AL_ERR_EVENTS_SSM_RAID_INPUT_LENGTH_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_RAID_APP_INT_A_INPUT_LENGTH_ERROR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Memory Compare/Set Error",
		.id = AL_ERR_EVENTS_SSM_RAID_MEMORY_COMPARE_SET_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_RAID_APP_INT_A_MEM_SW_ERROR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Max Length Error",
		.id = AL_ERR_EVENTS_SSM_RAID_MAX_LENGTH_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_RAID_APP_INT_A_LENGTH_TOO_BIG,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Q_FIFO/Memory Stage Error",
		.id = AL_ERR_EVENTS_SSM_RAID_MEMORY_STAGE_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_RAID_APP_INT_A_MEM_ERROR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "M2S Error",
		.id = AL_ERR_EVENTS_SSM_RAID_M2S_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_RAID_APP_INT_A_M2S_ERROR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "FIFO ACK Error 0",
		.id = AL_ERR_EVENTS_SSM_RAID_FIFO_ACK_ERROR_0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_RAID_APP_INT_A_FIFO_ACK_ERROR_0,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "FIFO ACK Error 1",
		.id = AL_ERR_EVENTS_SSM_RAID_FIFO_ACK_ERROR_1,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_RAID_APP_INT_A_FIFO_ACK_ERROR_1,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "FIFO ACK Error 2",
		.id = AL_ERR_EVENTS_SSM_RAID_FIFO_ACK_ERROR_2,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_RAID_APP_INT_A_FIFO_ACK_ERROR_2,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "FIFO ACK Error 3",
		.id = AL_ERR_EVENTS_SSM_RAID_FIFO_ACK_ERROR_3,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_RAID_APP_INT_A_FIFO_ACK_ERROR_3,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "FIFO Queue Overrun Error",
		.id = AL_ERR_EVENTS_SSM_RAID_FIFO_QUEUE_OVERRUN_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_RAID_APP_INT_A_FIFO_Q_OVERRUN,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "SOP After EOP Error",
		.id = AL_ERR_EVENTS_SSM_RAID_SOP_AFTER_EOP_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_RAID_APP_INT_A_SOP_AFTER_SOP,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "No SOP Error",
		.id = AL_ERR_EVENTS_SSM_RAID_NO_SOP_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_RAID_APP_INT_A_NO_SOP,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "SOP With EOP Error",
		.id = AL_ERR_EVENTS_SSM_RAID_SOP_WITH_EOP_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_RAID_APP_INT_A_SOP_WITH_EOP,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "EOP After EOP Error",
		.id = AL_ERR_EVENTS_SSM_RAID_EOP_AFTER_EOP_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_RAID_APP_INT_A_EOP_AFTER_EOP,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Memory 1 odd Parity Error",
		.id = AL_ERR_EVENTS_SSM_RAID_MEM1_ODD_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_RAID_APP_INT_A_PAR_ERR_MEM1_ODD,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Memory 1 even Parity Error",
		.id = AL_ERR_EVENTS_SSM_RAID_MEM1_EVEN_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_RAID_APP_INT_A_PAR_ERR_MEM1_EVEN,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Memory 2 odd Parity Error",
		.id = AL_ERR_EVENTS_SSM_RAID_MEM2_ODD_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_RAID_APP_INT_A_PAR_ERR_MEM2_ODD,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Memory 2 even Parity Error",
		.id = AL_ERR_EVENTS_SSM_RAID_MEM2_EVEN_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_RAID_APP_INT_A_PAR_ERR_MEM2_EVEN,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Memory 3 odd Parity Error",
		.id = AL_ERR_EVENTS_SSM_RAID_MEM3_ODD_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_RAID_APP_INT_A_PAR_ERR_MEM3_ODD,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Memory 3 even Parity Error",
		.id = AL_ERR_EVENTS_SSM_RAID_MEM3_EVEN_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_RAID_APP_INT_A_PAR_ERR_MEM3_EVEN,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Memory 4 odd Parity Error",
		.id = AL_ERR_EVENTS_SSM_RAID_MEM4_ODD_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_RAID_APP_INT_A_PAR_ERR_MEM4_ODD,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Memory 4 even Parity Error",
		.id = AL_ERR_EVENTS_SSM_RAID_MEM4_EVEN_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_RAID_APP_INT_A_PAR_ERR_MEM4_EVEN,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Erasure code FIFO Q Overrun Error",
		.id = AL_ERR_EVENTS_SSM_RAID_ERASURE_CODE_FIFO_Q_OVERRUN_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_RAID_APP_INT_A_ERASURE_CODE_FIFO_Q_OVERRUN,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Erasure code FIFO R Overrun Error",
		.id = AL_ERR_EVENTS_SSM_RAID_ERASURE_CODE_FIFO_R_OVERRUN_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_RAID_APP_INT_A_ERASURE_CODE_FIFO_R_OVERRUN,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Erasure code FIFO S Overrun Error",
		.id = AL_ERR_EVENTS_SSM_RAID_ERASURE_CODE_FIFO_S_OVERRUN_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_RAID_APP_INT_A_ERASURE_CODE_FIFO_S_OVERRUN,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
};

static const struct al_err_events_field
	al_err_events_ssm_crc_fields[AL_ERR_EVENTS_SSM_CRC_MAX_ERRORS] = {
	{
		.name = "S2M Timeout",
		.id = AL_ERR_EVENTS_SSM_CRC_S2M_TIMEOUT,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_CRC_APP_INT_A_S2M_TIMEOUT,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "M2S Timeout",
		.id = AL_ERR_EVENTS_SSM_CRC_M2S_TIMEOUT,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_CRC_APP_INT_A_M2S_TIMEOUT,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Command Decode Error",
		.id = AL_ERR_EVENTS_SSM_CRC_CMD_DECODE_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_CRC_APP_INT_A_CMD_DECODE_ERROR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Input Length Error",
		.id = AL_ERR_EVENTS_SSM_CRC_INPUT_LENGTH_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_CRC_APP_INT_A_INPUT_LENGTH_ERROR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Memory Compare/Set Error",
		.id = AL_ERR_EVENTS_SSM_CRC_MEMORY_COMPARE_SET_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_CRC_APP_INT_A_MEM_SW_ERROR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Max Length Error",
		.id = AL_ERR_EVENTS_SSM_CRC_MAX_LENGTH_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_CRC_APP_INT_A_LENGTH_TOO_BIG,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "M2S Error",
		.id = AL_ERR_EVENTS_SSM_CRC_M2S_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_CRC_APP_INT_A_M2S_ERROR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "FIFO ACK Error 0",
		.id = AL_ERR_EVENTS_SSM_CRC_FIFO_ACK_ERROR_0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_CRC_APP_INT_A_FIFO_ACK_ERROR_0,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "FIFO ACK Error 1",
		.id = AL_ERR_EVENTS_SSM_CRC_FIFO_ACK_ERROR_1,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_CRC_APP_INT_A_FIFO_ACK_ERROR_1,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "FIFO ACK Error 2",
		.id = AL_ERR_EVENTS_SSM_CRC_FIFO_ACK_ERROR_2,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_CRC_APP_INT_A_FIFO_ACK_ERROR_2,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "FIFO ACK Error 3",
		.id = AL_ERR_EVENTS_SSM_CRC_FIFO_ACK_ERROR_3,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_CRC_APP_INT_A_FIFO_ACK_ERROR_3,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "FIFO Queue Overrun",
		.id = AL_ERR_EVENTS_SSM_CRC_FIFO_QUEUE_OVERRUN_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_CRC_APP_INT_A_FIFO_Q_OVERRUN,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "SOP After EOP Error",
		.id = AL_ERR_EVENTS_SSM_CRC_SOP_AFTER_EOP_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_CRC_APP_INT_A_SOP_AFTER_SOP,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "No SOP Error",
		.id = AL_ERR_EVENTS_SSM_CRC_NO_SOP_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_CRC_APP_INT_A_NO_SOP,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "SOP With EOP Error",
		.id = AL_ERR_EVENTS_SSM_CRC_SOP_WITH_EOP_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_CRC_APP_INT_A_SOP_WITH_EOP,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "EOP After EOP Error",
		.id = AL_ERR_EVENTS_SSM_CRC_EOP_AFTER_EOP_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_CRC_APP_INT_A_EOP_AFTER_EOP,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
};

/*******************************************************************************
 ** Crypto
 ******************************************************************************/
static int err_events_ssm_crypto_collect(struct al_err_events_module *module,
					 enum al_err_events_collect collect)
{
	int ret;

	ret = al_err_events_common_module_iofic_collect(module, collect, 0, AL_INT_GROUP_A);
	if (ret)
		return ret;

	ret = al_err_events_common_module_iofic_collect(module, collect, 0, AL_INT_GROUP_B);
	if (ret)
		return ret;

	ret = al_err_events_common_module_iofic_collect(module, collect, 0, AL_INT_GROUP_C);
	if (ret)
		return ret;

	return 0;
}

int al_err_events_ssm_crypto_init(struct al_err_events_handle *handle,
				  struct al_err_events_ssm_crypto_data *data,
				  struct al_err_events_ssm_app_init_params *params)
{
	uint32_t a_mask, b_mask;
	uint32_t c_mask, d_mask;
	struct al_err_events_module *module;
	struct al_err_events_common_module_params module_params;

	al_assert(handle);
	al_assert(data);
	al_assert(params);
	al_assert(sizeof(al_err_events_ssm_crypto_fields) == sizeof(data->fields));
	al_assert(sizeof(al_err_events_ssm_crypto_fields) ==
		 (AL_ERR_EVENTS_SSM_CRYPTO_MAX_ERRORS * sizeof(struct al_err_events_field)));

	al_memset(&module_params, 0, sizeof(module_params));

	module = &data->module;

	/* init module */
	al_sprintf(module_params.name, "SSM(%d)-CRYPTO", params->ssm_idx);
	module_params.primary_module = AL_ERR_EVENTS_MODULE_SSM;
	module_params.secondary_module = AL_ERR_EVENTS_SEC_MODULE_SSM_CRYPTO;
	module_params.primary_index = params->ssm_idx;
	module_params.secondary_index = 0;
	module_params.collect_mode = params->collect_mode;
	module_params.ic_regs_bases[0] = (uint8_t *)params->app_regs_base +
							AL_CRYPTO_APP_IOFIC_OFFSET;
	module_params.ic_size = 1;
	module_params.fields_size = AL_ERR_EVENTS_SSM_CRYPTO_MAX_ERRORS;
	module_params.fields = data->fields;
	module_params.fields_template = al_err_events_ssm_crypto_fields;
	module_params.private_data = data;
	module_params.collect = err_events_ssm_crypto_collect;
	module_params.print = al_err_events_common_module_print;
	module_params.clear = al_err_events_common_module_clear;
	module_params.test = al_err_events_common_module_test;

	al_err_events_common_module_init(handle, module, &module_params);

	al_crypto_error_ints_mask_get(params->rev_id,
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
 ** Raid
 ******************************************************************************/
static int err_events_ssm_raid_collect(struct al_err_events_module *module,
				       enum al_err_events_collect collect)
{
	return al_err_events_common_module_iofic_collect(module, collect, 0, AL_INT_GROUP_A);
}

int al_err_events_ssm_raid_init(struct al_err_events_handle *handle,
				struct al_err_events_ssm_raid_data *data,
				struct al_err_events_ssm_app_init_params *params)
{
	uint32_t a_mask, b_mask;
	uint32_t c_mask, d_mask;
	struct al_err_events_module *module;
	struct al_err_events_common_module_params module_params;

	al_assert(handle);
	al_assert(data);
	al_assert(params);
	al_assert(sizeof(al_err_events_ssm_raid_fields) == sizeof(data->fields));
	al_assert(sizeof(al_err_events_ssm_raid_fields) ==
		 (AL_ERR_EVENTS_SSM_RAID_MAX_ERRORS * sizeof(struct al_err_events_field)));

	module = &data->module;

	/* init module */
	al_memset(&module_params, 0, sizeof(module_params));
	al_sprintf(module_params.name, "SSM(%d)-RAID", params->ssm_idx);
	module_params.primary_module = AL_ERR_EVENTS_MODULE_SSM;
	module_params.secondary_module = AL_ERR_EVENTS_SEC_MODULE_SSM_RAID;
	module_params.primary_index = params->ssm_idx;
	module_params.secondary_index = 0;
	module_params.collect_mode = params->collect_mode;
	module_params.ic_regs_bases[0] = (uint8_t *)params->app_regs_base +
							AL_RAID_APP_IOFIC_OFFSET;
	module_params.ic_size = 1;
	module_params.fields_size = AL_ERR_EVENTS_SSM_RAID_MAX_ERRORS;
	module_params.fields = data->fields;
	module_params.fields_template = al_err_events_ssm_raid_fields;
	module_params.private_data = data;
	module_params.collect = err_events_ssm_raid_collect;
	module_params.print = al_err_events_common_module_print;
	module_params.clear = al_err_events_common_module_clear;
	module_params.test = al_err_events_common_module_test;

	al_err_events_common_module_init(handle, module, &module_params);

	al_raid_error_ints_mask_get(params->rev_id,
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
 ** CRC
 ******************************************************************************/
static int err_events_ssm_crc_collect(struct al_err_events_module *module,
				      enum al_err_events_collect collect)
{
	return al_err_events_common_module_iofic_collect(module, collect, 0, AL_INT_GROUP_A);
}

int al_err_events_ssm_crc_init(struct al_err_events_handle *handle,
			       struct al_err_events_ssm_crc_data *data,
			       struct al_err_events_ssm_app_init_params *params)
{
	uint32_t a_mask, b_mask;
	uint32_t c_mask, d_mask;
	struct al_err_events_module *module;
	struct al_err_events_common_module_params module_params;

	al_assert(handle);
	al_assert(data);
	al_assert(params);
	al_assert(sizeof(al_err_events_ssm_crc_fields) == sizeof(data->fields));
	al_assert(sizeof(al_err_events_ssm_crc_fields) ==
		 (AL_ERR_EVENTS_SSM_CRC_MAX_ERRORS * sizeof(struct al_err_events_field)));

	module = &data->module;

	/* init module */
	al_memset(&module_params, 0, sizeof(module_params));
	al_sprintf(module_params.name, "SSM(%d)-CRC(%d)", params->ssm_idx, params->secondary_index);
	module_params.primary_module = AL_ERR_EVENTS_MODULE_SSM;
	module_params.secondary_module = AL_ERR_EVENTS_SEC_MODULE_SSM_CRC;
	module_params.primary_index = params->ssm_idx;
	module_params.secondary_index = params->secondary_index;
	module_params.collect_mode = params->collect_mode;
	module_params.ic_regs_bases[0] = (uint8_t *)params->app_regs_base + AL_CRC_APP_IOFIC_OFFSET;
	module_params.ic_size = 1;
	module_params.fields_size = AL_ERR_EVENTS_SSM_CRC_MAX_ERRORS;
	module_params.fields = data->fields;
	module_params.fields_template = al_err_events_ssm_crc_fields;
	module_params.private_data = data;
	module_params.collect = err_events_ssm_crc_collect;
	module_params.print = al_err_events_common_module_print;
	module_params.clear = al_err_events_common_module_clear;
	module_params.test = al_err_events_common_module_test;

	al_err_events_common_module_init(handle, module, &module_params);

	al_crc_error_ints_mask_get(params->rev_id,
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
