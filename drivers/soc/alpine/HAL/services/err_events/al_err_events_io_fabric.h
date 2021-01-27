/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#ifndef __AL_ERR_EVENTS_IO_FABRIC_H__
#define __AL_ERR_EVENTS_IO_FABRIC_H__

#include "al_hal_unit_adapter.h"
#include "al_err_events.h"
#include "al_hal_serdes_interface.h"
#include "al_hal_serdes_avg.h"

/*******************************************************************************
 ** Error ID's
 ******************************************************************************/
/*
 * PBS errors ID's
 */
enum al_err_events_pbs {
	AL_ERR_EVENTS_PBS_SW_SRAM_PARITY = 0,
	AL_ERR_EVENTS_PBS_UFC_PARITY_ERROR,
	AL_ERR_EVENTS_PBS_APB_PREADY_NO_PSEL_ERROR,
	AL_ERR_EVENTS_PBS_APB_DOUBLE_PREADY_ERROR,
	AL_ERR_EVENTS_PBS_TDM_TX_PARITY_ERROR,
	AL_ERR_EVENTS_PBS_TDM_RX_PARITY_ERROR,
	AL_ERR_EVENTS_PBS_MAX_ERRORS
};

/*
 * Serdes-25G errors ID's
 */
enum al_err_events_serdes_25g {
	AL_ERR_EVENTS_SERDES_INTERNAL_SRAM_PARITY = 0,
	AL_ERR_EVENTS_SERDES_DATA_MEMORY_PARITY,
	AL_ERR_EVENTS_SERDES_DATA_PROGRAM_PARITY,
	AL_ERR_EVENTS_SERDES_MAX_ERRORS
};

/*
 * AVG serdes errors ID's
 */
enum al_err_events_serdes_avg {
	AL_ERR_EVENTS_SERDES_AVG_SINGLE_BIT_ERR = 0,
	AL_ERR_EVENTS_SERDES_AVG_DOUBLE_BIT_ERR,
	AL_ERR_EVENTS_SERDES_AVG_MAX_ERRORS
};

/*
 * Unit-Adapter errors ID's
 */
enum al_err_events_unit_adapter {
	AL_ERR_EVENTS_UNIT_ADAPTER_AXI_READ_PARITY_ERROR = 0,
	AL_ERR_EVENTS_UNIT_ADAPTER_AXI_READ_ERROR_BLOCKED,
	AL_ERR_EVENTS_UNIT_ADAPTER_AXI_READ_COMPLETION_TIMEOUT,
	AL_ERR_EVENTS_UNIT_ADAPTER_AXI_READ_COMPLETION_ERROR,
	AL_ERR_EVENTS_UNIT_ADAPTER_AXI_READ_ADDRESS_TIMEOUT,
	AL_ERR_EVENTS_UNIT_ADAPTER_AXI_WRITE_ERROR_BLOCKED,
	AL_ERR_EVENTS_UNIT_ADAPTER_AXI_WRITE_COMPLETION_TIMEOUT,
	AL_ERR_EVENTS_UNIT_ADAPTER_AXI_WRITE_COMPLETION_ERROR,
	AL_ERR_EVENTS_UNIT_ADAPTER_AXI_WRITE_ADDRESS_TIMEOUT,
	AL_ERR_EVENTS_UNIT_ADAPTER_AXI_ROB_OUT_SRAM_PARITY,
	AL_ERR_EVENTS_UNIT_ADAPTER_AXI_ROB_IN_SRAM_PARITY,
	AL_ERR_EVENTS_UNIT_ADAPTER_SATA_PORT_0_TX_RAM_PARITY,
	AL_ERR_EVENTS_UNIT_ADAPTER_SATA_PORT_1_TX_RAM_PARITY,
	AL_ERR_EVENTS_UNIT_ADAPTER_SATA_PORT_2_TX_RAM_PARITY,
	AL_ERR_EVENTS_UNIT_ADAPTER_SATA_PORT_3_TX_RAM_PARITY,
	AL_ERR_EVENTS_UNIT_ADAPTER_SATA_PORT_0_RX_RAM_PARITY,
	AL_ERR_EVENTS_UNIT_ADAPTER_SATA_PORT_1_RX_RAM_PARITY,
	AL_ERR_EVENTS_UNIT_ADAPTER_SATA_PORT_2_RX_RAM_PARITY,
	AL_ERR_EVENTS_UNIT_ADAPTER_SATA_PORT_3_RX_RAM_PARITY,
	AL_ERR_EVENTS_UNIT_ADAPTER_SATA_PORT_0_FB_LOW_RAM_PARITY,
	AL_ERR_EVENTS_UNIT_ADAPTER_SATA_PORT_1_FB_LOW_RAM_PARITY,
	AL_ERR_EVENTS_UNIT_ADAPTER_SATA_PORT_2_FB_LOW_RAM_PARITY,
	AL_ERR_EVENTS_UNIT_ADAPTER_SATA_PORT_3_FB_LOW_RAM_PARITY,
	AL_ERR_EVENTS_UNIT_ADAPTER_SATA_PORT_0_FB_HIGH_RAM_PARITY,
	AL_ERR_EVENTS_UNIT_ADAPTER_SATA_PORT_1_FB_HIGH_RAM_PARITY,
	AL_ERR_EVENTS_UNIT_ADAPTER_SATA_PORT_2_FB_HIGH_RAM_PARITY,
	AL_ERR_EVENTS_UNIT_ADAPTER_SATA_PORT_3_FB_HIGH_RAM_PARITY,
	AL_ERR_EVENTS_UNIT_ADAPTER_MAX_ERRORS
};

enum al_err_events_axi_err_type {
	AL_ERR_EVENTS_AXI_ERR_TYPE_NONE,
	AL_ERR_EVENTS_AXI_ERR_TYPE_READ,
	AL_ERR_EVENTS_AXI_ERR_TYPE_WRITE,
};

/*******************************************************************************
 ** Init params
 ******************************************************************************/
struct al_err_events_pbs_init_params {
	/** collection mode */
	enum al_err_events_collect collect_mode;
};

struct al_err_events_serdes_25g_init_params {
	/** Serdes registers base address */
	void __iomem *unit_regs_base;

	/** collection mode */
	enum al_err_events_collect collect_mode;
};

struct al_err_events_serdes_avg_init_params {
	/** collection mode */
	enum al_err_events_collect collect_mode;

	/** Lane register base address */
	void __iomem			*regs_base;
	/** Initialized complex object */
	struct al_serdes_complex_obj	*complex_obj;
	/** Lane number in complex */
	unsigned int			lane_in_complex;
};

struct al_err_events_ua_init_params {
	/** Unit Adapter type */
	enum al_unit_adapter_type type;

	/** Unit Adapter index */
	unsigned int index;

	/** Unit Adapter registers base address */
	void __iomem *ua_regs_base;

	/** collection mode */
	enum al_err_events_collect collect_mode;
};

/*******************************************************************************
 ** Error Data structures
 ******************************************************************************/
struct al_err_events_pbs_data {
	struct al_err_events_module module;

	struct al_err_events_field fields[AL_ERR_EVENTS_PBS_MAX_ERRORS];
};

struct al_err_events_serdes_25g_data {
	struct al_err_events_module module;

	struct al_err_events_field fields[AL_ERR_EVENTS_SERDES_MAX_ERRORS];
};

struct al_err_events_serdes_avg_data {
	struct al_err_events_module module;

	struct al_err_events_field fields[AL_ERR_EVENTS_SERDES_AVG_MAX_ERRORS];

	struct al_serdes_grp_obj	obj;
};

struct al_err_events_unit_adapter_data {
	struct al_err_events_module module;

	struct al_err_events_field fields[AL_ERR_EVENTS_UNIT_ADAPTER_MAX_ERRORS];

	enum al_unit_adapter_type type;

	struct al_unit_adapter unit_adapter;

	struct al_unit_adapter_err_attr axi_err;

	enum al_err_events_axi_err_type axi_err_type;

	struct al_unit_adapter_int_fields int_fields;
};

/*******************************************************************************
 ** API
 ******************************************************************************/
/*
 * Initialize PBS error events
 *
 * @param handle Error events handle
 * @param data PBS error events object
 * @param params PBS initialization parameters
 *
 * @return 0 on success, errno otherwise
 */
int al_err_events_pbs_init(struct al_err_events_handle *handle,
			   struct al_err_events_pbs_data *data,
			   struct al_err_events_pbs_init_params *params);

/*
 * Enable PBS error interrupts
 *
 * @param data PBS error events initialized object
 *
 * @return 0 on success, errno otherwise
 */
int al_err_events_pbs_ints_enable(struct al_err_events_pbs_data *data);

/*
 * Initialize Serdes 25G error events
 *
 * @param handle Error events handle
 * @param data Serdes 25G error events object
 * @param params Serdes 25G initialization parameters
 *
 * @return 0 on success, errno otherwise
 */
int al_err_events_serdes_25g_init(struct al_err_events_handle *handle,
				  struct al_err_events_serdes_25g_data *data,
				  struct al_err_events_serdes_25g_init_params *params);

/*
 * Enable Serdes 25G error interrupts
 *
 * @param data Serdes 25G error events initialized object
 *
 * @return 0 on success, errno otherwise
 */
int al_err_events_serdes_25g_ints_enable(struct al_err_events_serdes_25g_data *data);

/*
 * Initialize AVG Serdes error events
 *
 * @param handle Error events handle
 * @param data AVG Serdes error events object
 * @param params AVG Serdes initialization parameters
 *
 * @return 0 on success, errno otherwise
 */
int al_err_events_serdes_avg_init(struct al_err_events_handle *handle,
				  struct al_err_events_serdes_avg_data *data,
				  struct al_err_events_serdes_avg_init_params *params);

/*
 * Enable AVG Serdes error interrupts
 *
 * @param data AVG Serdes error events initialized object
 *
 * @return 0 on success, errno otherwise
 */
int al_err_events_serdes_avg_ints_enable(struct al_err_events_serdes_avg_data *data);

/*
 * Initialize Unit Adapter error events
 *
 * @param handle Error events handle
 * @param data Unit Adapter error events object
 * @param params Unit Adapter initialization parameters
 *
 * @return 0 on success, errno otherwise
 */
int al_err_events_unit_adapter_init(struct al_err_events_handle *handle,
				    struct al_err_events_unit_adapter_data *data,
				    struct al_err_events_ua_init_params *params);

/*
 * Enable Unit Adapter error interrupts
 *
 * @param data Unit Adapter error events initialized object
 *
 * @return 0 on success, errno otherwise
 */
int al_err_events_ua_ints_enable(struct al_err_events_unit_adapter_data *data);

/*
 * Disable Unit Adapter error interrupts
 *
 * @param data Unit Adapter error events initialized object
 */
void al_err_events_ua_ints_disable(struct al_err_events_unit_adapter_data *data);

#endif /* __AL_ERR_EVENTS_IO_FABRIC_H__ */
