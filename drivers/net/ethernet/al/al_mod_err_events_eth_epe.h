/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#ifndef __AL_ERR_EVENTS_ETH_EPE_H__
#define __AL_ERR_EVENTS_ETH_EPE_H__

#include "al_mod_hal_eth.h"
#include "al_mod_err_events.h"
#include "al_mod_err_events_eth_epe.h"

/*******************************************************************************
 ** Error ID's
 ******************************************************************************/
/*
 * EPE errors ID's
 */
enum al_mod_err_events_eth_epe {
	AL_ERR_EVENTS_ETH_EPE_RMP_HDR_INPUT_FIFO_MEM_PAR_ERR_0 = 0,
	AL_ERR_EVENTS_ETH_EPE_RMP_HDR_INPUT_FIFO_MEM_PAR_ERR_1,
	AL_ERR_EVENTS_ETH_EPE_RMP_HDR_INPUT_FIFO_MEM_PAR_ERR_2,
	AL_ERR_EVENTS_ETH_EPE_RMP_HDR_INPUT_FIFO_MEM_PAR_ERR_3,
	AL_ERR_EVENTS_ETH_EPE_RMP_HDR_INPUT_FIFO_MEM_PAR_ERR_4,
	AL_ERR_EVENTS_ETH_EPE_RMP_HDR_INPUT_FIFO_MEM_PAR_ERR_5,
	AL_ERR_EVENTS_ETH_EPE_RMP_HDR_INPUT_FIFO_MEM_PAR_ERR_6,
	AL_ERR_EVENTS_ETH_EPE_RMP_HDR_INPUT_FIFO_MEM_PAR_ERR_7,
	AL_ERR_EVENTS_ETH_EPE_RMP_DATA_FIFO_LO_MEM_PAR_ERR,
	AL_ERR_EVENTS_ETH_EPE_RMP_DATA_FIFO_HI_MEM_PAR_ERR,
	AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_IN_EOP_IN_IDLE_0,
	AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_IN_EOP_IN_IDLE_1,
	AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_IN_EOP_IN_IDLE_2,
	AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_IN_EOP_IN_IDLE_3,
	AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_IN_EOP_IN_IDLE_4,
	AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_IN_EOP_IN_IDLE_5,
	AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_IN_EOP_IN_IDLE_6,
	AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_IN_EOP_IN_IDLE_7,
	AL_ERR_EVENTS_ETH_EPE_HDR_INPUT_FIFO_ERR_IN_EOP_IN_IDLE_0,
	AL_ERR_EVENTS_ETH_EPE_HDR_INPUT_FIFO_ERR_IN_EOP_IN_IDLE_1,
	AL_ERR_EVENTS_ETH_EPE_HDR_INPUT_FIFO_ERR_IN_EOP_IN_IDLE_2,
	AL_ERR_EVENTS_ETH_EPE_HDR_INPUT_FIFO_ERR_IN_EOP_IN_IDLE_3,
	AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_IN_SOP_IN_PKT_0,
	AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_IN_SOP_IN_PKT_1,
	AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_IN_SOP_IN_PKT_2,
	AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_IN_SOP_IN_PKT_3,
	AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_IN_SOP_IN_PKT_4,
	AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_IN_SOP_IN_PKT_5,
	AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_IN_SOP_IN_PKT_6,
	AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_IN_SOP_IN_PKT_7,
	AL_ERR_EVENTS_ETH_EPE_HDR_INPUT_FIFO_ERR_IN_SOP_IN_PKT_0,
	AL_ERR_EVENTS_ETH_EPE_HDR_INPUT_FIFO_ERR_IN_SOP_IN_PKT_1,
	AL_ERR_EVENTS_ETH_EPE_HDR_INPUT_FIFO_ERR_IN_SOP_IN_PKT_2,
	AL_ERR_EVENTS_ETH_EPE_HDR_INPUT_FIFO_ERR_IN_SOP_IN_PKT_3,
	AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_OUT_EOP_HDR_EMPTY_0,
	AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_OUT_EOP_HDR_EMPTY_1,
	AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_OUT_EOP_HDR_EMPTY_2,
	AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_OUT_EOP_HDR_EMPTY_3,
	AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_OUT_EOP_HDR_EMPTY_4,
	AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_OUT_EOP_HDR_EMPTY_5,
	AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_OUT_EOP_HDR_EMPTY_6,
	AL_ERR_EVENTS_ETH_EPE_EC_ERR_RPE_OUT_EOP_HDR_EMPTY_7,
	AL_ERR_EVENTS_ETH_EPE_HDR_INPUT_FIFO_ERR_OUT_EOP_HDR_EMPTY_0,
	AL_ERR_EVENTS_ETH_EPE_HDR_INPUT_FIFO_ERR_OUT_EOP_HDR_EMPTY_1,
	AL_ERR_EVENTS_ETH_EPE_HDR_INPUT_FIFO_ERR_OUT_EOP_HDR_EMPTY_2,
	AL_ERR_EVENTS_ETH_EPE_HDR_INPUT_FIFO_ERR_OUT_EOP_HDR_EMPTY_3,
	AL_ERR_EVENTS_ETH_EPE_MAX_ERRORS,
};

/*******************************************************************************
 ** Init params
 ******************************************************************************/
struct al_mod_err_events_eth_epe_init_params {
	/** EPE handle */
	struct al_mod_eth_epe_handle_init_params *handle_params;

	/** EC regs base address */
	void __iomem *ec_intc_regs_base;

	/** collection mode */
	enum al_mod_err_events_collect collect_mode;
};

/*******************************************************************************
 ** Error Data structures
 ******************************************************************************/
struct al_mod_err_events_eth_epe_data {
	struct al_mod_err_events_module module;

	struct al_mod_err_events_field fields[AL_ERR_EVENTS_ETH_EPE_MAX_ERRORS];

	/** EPE handle */
	struct al_mod_eth_epe_handle handle;

	/** EC regs base address */
	void __iomem *ec_intc_regs_base;
};

/*******************************************************************************
 ** API
 ******************************************************************************/
/*
 * Initialize EPE error events
 *
 * @param handle Error events handle
 * @param data EPE error events object
 * @param params EPE initialization parameters
 *
 * @return 0 on success, errno otherwise
 */
int al_mod_err_events_eth_epe_init(struct al_mod_err_events_handle *handle,
			      struct al_mod_err_events_eth_epe_data *data,
			      struct al_mod_err_events_eth_epe_init_params *params);

#endif /* __AL_ERR_EVENTS_ETH_EPE_H__ */
