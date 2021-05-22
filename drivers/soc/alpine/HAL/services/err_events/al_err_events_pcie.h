/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#ifndef __AL_ERR_EVENTS_PCIE_H__
#define __AL_ERR_EVENTS_PCIE_H__

#include "al_err_events.h"
#include "al_hal_pcie.h"

/*******************************************************************************
 ** Error ID's
 ******************************************************************************/
/*
 * PCIe Application errors ID's
 */
enum al_err_events_pcie_app {
	AL_ERR_EVENTS_PCIE_APP_CORE_INTERNAL_DECOMPOSE_LOGIC_ERROR = 0,
	AL_ERR_EVENTS_PCIE_APP_PARITY_ERROR,
	AL_ERR_EVENTS_PCIE_APP_WRITE_OVERFLOW_ERROR,
	AL_ERR_EVENTS_PCIE_APP_INTERNAL_OVERFLOW_ERROR,
	AL_ERR_EVENTS_PCIE_APP_SYS_ERR_FUNC_0,
	AL_ERR_EVENTS_PCIE_APP_SYS_ERR_FUNC_1,
	AL_ERR_EVENTS_PCIE_APP_SYS_ERR_FUNC_2,
	AL_ERR_EVENTS_PCIE_APP_SYS_ERR_FUNC_3,
	AL_ERR_EVENTS_PCIE_APP_AER_ERR_INT_0,
	AL_ERR_EVENTS_PCIE_APP_AER_ERR_INT_1,
	AL_ERR_EVENTS_PCIE_APP_AER_ERR_INT_2,
	AL_ERR_EVENTS_PCIE_APP_AER_ERR_INT_3,
	AL_ERR_EVENTS_PCIE_APP_AER_ERR_MSI_0,
	AL_ERR_EVENTS_PCIE_APP_AER_ERR_MSI_1,
	AL_ERR_EVENTS_PCIE_APP_AER_ERR_MSI_2,
	AL_ERR_EVENTS_PCIE_APP_AER_ERR_MSI_3,
	AL_ERR_EVENTS_PCIE_APP_AP_CRCTBL_STAT_0,
	AL_ERR_EVENTS_PCIE_APP_AP_CRCTBL_STAT_1,
	AL_ERR_EVENTS_PCIE_APP_AP_CRCTBL_STAT_2,
	AL_ERR_EVENTS_PCIE_APP_AP_CRCTBL_STAT_3,
	AL_ERR_EVENTS_PCIE_APP_AP_UNCRCTBL_STAT_0,
	AL_ERR_EVENTS_PCIE_APP_AP_UNCRCTBL_STAT_1,
	AL_ERR_EVENTS_PCIE_APP_AP_UNCRCTBL_STAT_2,
	AL_ERR_EVENTS_PCIE_APP_AP_UNCRCTBL_STAT_3,
	AL_ERR_EVENTS_PCIE_APP_MAX_ERRORS,
};

/*
 * PCIe AXI errors ID's
 */
enum al_err_events_pcie_axi {
	AL_ERR_EVENTS_PCIE_AXI_PARITY_ERROR = 0,
	AL_ERR_EVENTS_PCIE_AXI_CORE_INTERNAL_DECOMPOSE_LOGIC_ERROR,
	AL_ERR_EVENTS_PCIE_AXI_MASTER_READ_DATA_PARITY_ERROR,
	AL_ERR_EVENTS_PCIE_AXI_SLAVE_READ_ADDR_PARITY_ERROR,
	AL_ERR_EVENTS_PCIE_AXI_SLAVE_WRITE_ADDR_PARITY_ERROR,
	AL_ERR_EVENTS_PCIE_AXI_SLAVE_WRITE_DATA_PARITY_ERROR,
	AL_ERR_EVENTS_PCIE_AXI_READ_COMPLETION_ERROR,
	AL_ERR_EVENTS_PCIE_AXI_WRITE_COMPLETION_ERROR,
	AL_ERR_EVENTS_PCIE_AXI_READ_COMPLETION_TIMEOUT,
	AL_ERR_EVENTS_PCIE_AXI_WRITE_COMPLETION_TIMEOUT,
	AL_ERR_EVENTS_PCIE_AXI_POS_ERROR,
	AL_ERR_EVENTS_PCIE_AXI_WRITE_RESPONED_ERROR,
	AL_ERR_EVENTS_PCIE_AXI_MAX_ERRORS,
};

/*******************************************************************************
 ** Init params
 ******************************************************************************/
struct al_err_events_pcie_init_params {
	/** Initialized PCie port object */
	struct al_pcie_port *pcie_port;

	/** collection mode */
	enum al_err_events_collect collect_mode;
};

/*******************************************************************************
 ** Error Data structures
 ******************************************************************************/
struct al_err_pcie_axi_parity {
	unsigned int u5_ram2p;
	unsigned int u4_ram2p;
	unsigned int u3_ram2p;
	unsigned int u2_ram2p;
	unsigned int u12_ram2p;
	unsigned int u10_ram2p;
};

struct al_err_pcie_app_parity_v1_v2 {
	unsigned int u_ram_1p_sotbuf;
	unsigned int u0_ram_radm_qbuffer;
	unsigned int u3_qbuffer_0;
	unsigned int u3_qbuffer_1;
	unsigned int u9_decomp;
	unsigned int u8_ram2p;
	unsigned int u7_ram2p;
	unsigned int u6_ram2p;
	unsigned int u11_ram2p;
	unsigned int u1_ram2p;
	unsigned int u0_ram2p;
	unsigned int u0_rbuf;
	unsigned int u3_qbuffer_2;
};

struct al_err_pcie_app_parity_v3 {
	unsigned int ram_1p_rbuf;
	unsigned int ram_2p_sotbuf;
	unsigned int u0_ram_radm_qbuffer_hdr;
	unsigned int u3_ram_radm_qbuffer_data_0;
	unsigned int u3_ram_radm_qbuffer_data_1;
	unsigned int u10_ram2p_0;
	unsigned int u10_ram2p_1;
	unsigned int u8_ram2p;
	unsigned int u7_ram2p;
	unsigned int u6_ram;
	unsigned int u11_ram2p;
	unsigned int u1_ram2p;
	unsigned int u0_ram2p;
};

struct al_err_events_pcie_app_data {
	/** PCie error event module */
	struct al_err_events_module module;

	/** PCIe error events fields */
	struct al_err_events_field fields[AL_ERR_EVENTS_PCIE_APP_MAX_ERRORS];

	struct al_pcie_port *pcie_port;

	struct al_err_pcie_app_parity_v1_v2 app_v1_v2_parity;

	struct al_err_pcie_app_parity_v3 app_v3_parity;
};

struct al_err_events_pcie_axi_data {
	/** PCie error event module */
	struct al_err_events_module module;

	/** PCIe error events fields */
	struct al_err_events_field fields[AL_ERR_EVENTS_PCIE_AXI_MAX_ERRORS];

	struct al_pcie_port *pcie_port;

	/** AXI read data parity error latched address */
	uint64_t axi_read_data_parity_address;

	/** AXI read completion error latched address */
	uint64_t axi_read_completion_error_address;

	/** AXI write completion error latched address */
	uint64_t axi_write_completion_error_address;

	/** AXI read completion timeout latched address */
	uint64_t axi_read_compl_timeout_address;

	/** AXI write completion timeout error latched address */
	uint64_t axi_write_cmpl_timeout_address;

	/** AXI POS error latched address */
	uint64_t axi_pos_error_addr;

	struct al_err_pcie_axi_parity axi_parity;
};

/*******************************************************************************
 ** API
 ******************************************************************************/
/*
 * Initialize PCIe Application error events
 *
 * @param handle Error events handle
 * @param data PCIe Application error events object
 * @param params PCIe initialization parameters
 *
 * @return 0 on success, errno otherwise
 */
int al_err_events_pcie_app_init(struct al_err_events_handle *handle,
				struct al_err_events_pcie_app_data *data,
				struct al_err_events_pcie_init_params *params);

/*
 * Enable PCIe Application error interrupts
 *
 * @param data PCIe Application error events initialized object
 *
 * @return 0 on success, errno otherwise
 */
int al_err_events_pcie_app_int_enable(struct al_err_events_pcie_app_data *data);

/*
 * Initialize PCIe AXI error events
 *
 * @param handle Error events handle
 * @param data PCIe AXI error events object
 * @param params PCIe AXI parameters
 *
 * @return 0 on success, errno otherwise
 */
int al_err_events_pcie_axi_init(struct al_err_events_handle *handle,
				struct al_err_events_pcie_axi_data *data,
				struct al_err_events_pcie_init_params *params);

/*
 * Enable PCIe AXI error interrupts
 *
 * @param data PCIe AXI error events initialized object
 *
 * @return 0 on success, errno otherwise
 */
int al_err_events_pcie_axi_int_enable(struct al_err_events_pcie_axi_data *data);

#endif /* __AL_ERR_EVENTS_PCIE_H__ */
