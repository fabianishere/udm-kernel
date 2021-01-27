/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#ifndef __AL_ERR_EVENTS_UDMA_H__
#define __AL_ERR_EVENTS_UDMA_H__

#include "al_mod_err_events.h"
#include "al_mod_hal_iofic.h"

/*******************************************************************************
 ** Error ID's
 ******************************************************************************/
/*
 * UDMA errors ID's
 */
enum al_mod_err_events_udma {
	AL_ERR_EVENTS_UDMA_M2S_MSIX_RESP_ERR = 0,
	AL_ERR_EVENTS_UDMA_M2S_MSIX_TIMEOUT,
	AL_ERR_EVENTS_UDMA_M2S_PREFETCH_HEADER_PARITY,
	AL_ERR_EVENTS_UDMA_M2S_PREFETCH_DESC_PARITY,
	AL_ERR_EVENTS_UDMA_M2S_DATA_BUF_PARITY,
	AL_ERR_EVENTS_UDMA_M2S_DATA_HEADER_PARITY,
	AL_ERR_EVENTS_UDMA_M2S_COMPL_COAL_PARITY,
	AL_ERR_EVENTS_UDMA_M2S_UNACK_PACKETS_PARITY,
	AL_ERR_EVENTS_UDMA_M2S_ACK_PACKETS_PARITY,
	AL_ERR_EVENTS_UDMA_M2S_AXI_DATA_PARITY,
	AL_ERR_EVENTS_UDMA_M2S_PREFETCH_RING_ID_ERROR,
	AL_ERR_EVENTS_UDMA_M2S_PREFETCH_LAST_ERROR,
	AL_ERR_EVENTS_UDMA_M2S_PREFETCH_FIRST_ERROR,
	AL_ERR_EVENTS_UDMA_M2S_PREFETCH_MAX_DESC_ERROR,
	AL_ERR_EVENTS_UDMA_M2S_MAX_PACKET_LENGTH_ERROR,
	AL_ERR_EVENTS_UDMA_M2S_PREFETCH_AXI_TIMEOUT,
	AL_ERR_EVENTS_UDMA_M2S_PREFETCH_AXI_RESP_ERROR,
	AL_ERR_EVENTS_UDMA_M2S_PREFETCH_AXI_PARITY,
	AL_ERR_EVENTS_UDMA_M2S_DATA_AXI_TIMEOUT,
	AL_ERR_EVENTS_UDMA_M2S_DATA_AXI_PARITY,
	AL_ERR_EVENTS_UDMA_M2S_DATA_AXI_RESP_ERROR,
	AL_ERR_EVENTS_UDMA_M2S_COMPLETION_AXI_TIMEOUT,
	AL_ERR_EVENTS_UDMA_M2S_COMPLETION_AXI_RESP_ERROR,
	AL_ERR_EVENTS_UDMA_M2S_COMPLETION_AXI_PARITY,
	AL_ERR_EVENTS_UDMA_M2S_STREAM_TIMEOUT,
	AL_ERR_EVENTS_UDMA_M2S_STREAM_RESP_ERROR,
	AL_ERR_EVENTS_UDMA_M2S_STREAM_PARITY,
	AL_ERR_EVENTS_UDMA_M2S_STREAM_COMPL_MISMATCH_ERROR,
	AL_ERR_EVENTS_UDMA_S2M_TRNSCTN_TBL_INFO_PARITY,
	AL_ERR_EVENTS_UDMA_S2M_PREFETCH_DESC_PARITY,
	AL_ERR_EVENTS_UDMA_S2M_COMPLETION_COAL_PARITY,
	AL_ERR_EVENTS_UDMA_S2M_PRE_UNACK_PACKETS_PARITY,
	AL_ERR_EVENTS_UDMA_S2M_UNACK_PACKETS_PARITY,
	AL_ERR_EVENTS_UDMA_S2M_DATA_BUF_PARITY,
	AL_ERR_EVENTS_UDMA_S2M_DATA_HEADER_PARITY,
	AL_ERR_EVENTS_UDMA_S2M_PACKET_LENGTH_ERROR,
	AL_ERR_EVENTS_UDMA_S2M_STREAM_LAST_ERROR,
	AL_ERR_EVENTS_UDMA_S2M_STREAM_FIRST_ERROR,
	AL_ERR_EVENTS_UDMA_S2M_STREAM_DATA_ERROR,
	AL_ERR_EVENTS_UDMA_S2M_STREAM_DATA_PARITY,
	AL_ERR_EVENTS_UDMA_S2M_STREAM_HEADER_ERROR,
	AL_ERR_EVENTS_UDMA_S2M_STREAM_HEADER_PARITY,
	AL_ERR_EVENTS_UDMA_S2M_COMPLETION_UNACK_TIMEOUT,
	AL_ERR_EVENTS_UDMA_S2M_COMPLETION_STREAM_TIMEOUT,
	AL_ERR_EVENTS_UDMA_S2M_COMPLETION_AXI_TIMEOUT,
	AL_ERR_EVENTS_UDMA_S2M_COMPLETION_AXI_RESP_ERROR,
	AL_ERR_EVENTS_UDMA_S2M_COMPLETION_AXI_PARITY,
	AL_ERR_EVENTS_UDMA_S2M_PREFETCH_RING_ID_ERROR,
	AL_ERR_EVENTS_UDMA_S2M_PREFETCH_AXI_TIMEOUT,
	AL_ERR_EVENTS_UDMA_S2M_PREFETCH_AXI_RESP_ERROR,
	AL_ERR_EVENTS_UDMA_S2M_PREFETCH_AXI_PARITY,
	AL_ERR_EVENTS_UDMA_S2M_NO_DESCRIPTORS_TIMEOUT,
	AL_ERR_EVENTS_UDMA_S2M_DATA_AXI_TIMEOUT,
	AL_ERR_EVENTS_UDMA_S2M_DATA_AXI_RESP_ERROR,
	AL_ERR_EVENTS_UDMA_S2M_DATA_AXI_PARITY,
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
	AL_ERR_EVENTS_UDMA_M2S_FETCH_AXI_DATA_CMD_PARITY,
	AL_ERR_EVENTS_UDMA_MAX_ERRORS,
};

/*******************************************************************************
 ** Init params
 ******************************************************************************/
struct al_mod_err_events_udma_init_params {
	/** UDMA regs base address */
	void __iomem *regs_base;

	/** UDMA index */
	unsigned int udma_index;

	/** Primary Module index */
	unsigned int module_index;

	/** Primary Module ID */
	enum al_mod_err_events_primary_module primary_module;

	/** collection mode */
	enum al_mod_err_events_collect collect_mode;
};

/*******************************************************************************
 ** Error Data structures
 ******************************************************************************/
struct al_mod_err_events_udma_data {
	struct al_mod_err_events_module module;

	struct al_mod_err_events_field fields[AL_ERR_EVENTS_UDMA_MAX_ERRORS];

	void __iomem *iofic_primary_regs_base;
	uint32_t ext_app_int;
	uint32_t sec_level_int[AL_IOFIC_MAX_GROUPS];
};

/*******************************************************************************
 ** API
 ******************************************************************************/
/*
 * Initialize UDMA error events
 *
 * @param handle Error events handle
 * @param data UDMA error events object
 * @param params UDMA initialization parameters
 *
 * @return 0 on success, errno otherwise
 */
int al_mod_err_events_udma_init(struct al_mod_err_events_handle *handle,
			    struct al_mod_err_events_udma_data *data,
			    struct al_mod_err_events_udma_init_params *params);

/*
 * Reads and clears UDMA external application interrupt indication
 *
 * @param data UDMA error events object
 *
 * @return AL_TRUE if interrupt occurred on external application, AL_FALSE otherwise
 */
#define AL_ERR_EVENTS_UDMA_HAS_EXT_APP_INT_READ_AND_CLEAR
al_mod_bool al_mod_err_events_udma_ext_app_int_read_and_clear(struct al_mod_err_events_udma_data *data);

#endif /* __AL_ERR_EVENTS_UDMA_H__ */
