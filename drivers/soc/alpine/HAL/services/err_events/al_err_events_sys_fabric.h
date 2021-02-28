/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#ifndef __AL_ERR_EVENTS_SYS_FABRIC_H__
#define __AL_ERR_EVENTS_SYS_FABRIC_H__

#include "al_err_events.h"
#include "al_init_sys_fabric.h"
#include "al_hal_sys_fabric_utils.h"
#include "al_hal_ddr.h"

#define AL_ERR_EVENTS_PCIE_NUM 8

/** Indicates that current API assumes DDR controller has multiple channels */
#define AL_ERR_EVENTS_HAS_MULTI_DDR_CHANNELS
/** Indicates that current API assumes CPU complex has multiple clusters */
#define AL_ERR_EVENTS_HAS_MULTI_CPU_CLUSTERS

/*******************************************************************************
 ** Error ID's
 ******************************************************************************/
/*
 * L1 errors ID's
 */
enum al_err_events_l1 {
	AL_ERR_EVENTS_L1_CORRECTABLE_ERROR = 0,
	AL_ERR_EVENTS_L1_UNCORRECTABLE_ERROR,
	AL_ERR_EVENTS_L1_ITAG_CORRECTABLE_ERROR,
	AL_ERR_EVENTS_L1_IDATA_CORRECTABLE_ERROR,
	AL_ERR_EVENTS_L1_DTAG_CORRECTABLE_ERROR,
	AL_ERR_EVENTS_L1_DDATA_CORRECTABLE_ERROR,
	AL_ERR_EVENTS_L1_L2_TLB_CORRECTABLE_ERROR,
	AL_ERR_EVENTS_L1_ITAG_UNCORRECTABLE_ERROR,
	AL_ERR_EVENTS_L1_IDATA_UNCORRECTABLE_ERROR,
	AL_ERR_EVENTS_L1_DTAG_UNCORRECTABLE_ERROR,
	AL_ERR_EVENTS_L1_DDATA_UNCORRECTABLE_ERROR,
	AL_ERR_EVENTS_L1_L2_TLB_UNCORRECTABLE_ERROR,
	AL_ERR_EVENTS_L1_UNKNOWN_ERROR,
	AL_ERR_EVENTS_L1_MAX_ERRORS,
};

/*
 * L2 errors ID's
 */
enum al_err_events_l2 {
	AL_ERR_EVENTS_L2_CORRECTABLE_ERROR = 0,
	AL_ERR_EVENTS_L2_UNCORRECTABLE_ERROR,
	AL_ERR_EVENTS_L2_TAG_CORRECTABLE_ERROR,
	AL_ERR_EVENTS_L2_DATA_CORRECTABLE_ERROR,
	AL_ERR_EVENTS_L2_SNOOP_FILTER_CORRECTABLE_ERROR,
	AL_ERR_EVENTS_L2_TAG_UNCORRECTABLE_ERROR,
	AL_ERR_EVENTS_L2_DATA_UNCORRECTABLE_ERROR,
	AL_ERR_EVENTS_L2_SNOOP_FILTER_UNORRECTABLE_ERROR,
	AL_ERR_EVENTS_L2_UNKNOWN_ERROR,
	AL_ERR_EVENTS_L2_MAX_ERRORS,
};

/*
 * System Fabric errors ID's
 */
enum al_err_events_sys_fabric {
	AL_ERR_EVENTS_SYS_FABRIC_DDR_CORRECTABLE_ERROR = 0,
	AL_ERR_EVENTS_SYS_FABRIC_DDR_UNCORRECTABLE_ERROR,
	AL_ERR_EVENTS_SYS_FABRIC_DDR_PARITY_ERROR,
	AL_ERR_EVENTS_SYS_FABRIC_DDR_ON_CHIP_PARITY_ERROR,
	AL_ERR_EVENTS_SYS_FABRIC_AXI_ERROR,
	AL_ERR_EVENTS_SYS_FABRIC_LOCAL_GIC_ERROR,
	AL_ERR_EVENTS_SYS_FABRIC_MSIX_ERROR,
	AL_ERR_EVENTS_SYS_FABRIC_SB_POS_ERROR,
	AL_ERR_EVENTS_SYS_FABRIC_CCI_ERROR,
	AL_ERR_EVENTS_SYS_FABRIC_MAX_ERRORS,
};

enum al_err_events_sys_fabric_v3_error {
	AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_CPU_AXIERRIRQ_N_0,
	AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_CPU_AXIERRIRQ_N_1,
	AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_CPU_AXIERRIRQ_N_2,
	AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_CPU_AXIERRIRQ_N_3,
	AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_CPU_RD_DATA_PAR_ERR_0,
	AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_CPU_RD_DATA_PAR_ERR_1,
	AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_CPU_RD_DATA_PAR_ERR_2,
	AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_CPU_RD_DATA_PAR_ERR_3,
	AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_CCI_ERRO_IRQ_N,
	AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_CORESIGHT_TSCNT_OVFL_INTR,
	AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_GIC_ECC_FATAL,
	AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_FABRIC_MSIX_ERR_INTR,
	AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_FABRIC_POS_ERR_IRQ,
	AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_FABRIC_PP_SRAM_PARITY_ERR,
	AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_FABRIC_C2SWB_PARITY_ERR,
	AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_FABRIC_SB_ARREQ_PAR_ERR_0,
	AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_FABRIC_SB_ARREQ_PAR_ERR_1,
	AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_FABRIC_SB_ARREQ_PAR_ERR_2,
	AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_FABRIC_SB_ARREQ_PAR_ERR_3,
	AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_IO_MMU_PTW_RD_DATA_PAR_ERR,
	AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_IO_CACHE_MEMORY_UNCORR_ECC_0,
	AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_IO_CACHE_MEMORY_UNCORR_ECC_1,
	AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_IO_CACHE_MEMORY_CORR_ECC_0,
	AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_IO_CACHE_MEMORY_CORR_ECC_1,
	AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_IO_CACHE_PARITY_ERROR_0,
	AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_IO_CACHE_PARITY_ERROR_1,
	AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_DDR_ONCHIP_PARITY_0,
	AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_DDR_ONCHIP_PARITY_1,
	AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_DDR_CHANNEL_0_UNCORRECTABLE_ECC,
	AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_DDR_CHANNEL_0_CORRECTABLE_ECC,
	AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_DDR_CHANNEL_0_DFI_ALERT_N_CRC,
	AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_DDR_CHANNEL_0_MAX_ALERT_N_REACHED_CRC,
	AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_DDR_CHANNEL_1_UNCORRECTABLE_ECC,
	AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_DDR_CHANNEL_1_CORRECTABLE_ECC,
	AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_DDR_CHANNEL_1_DFI_ALERT_N_CRC,
	AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_DDR_CHANNEL_1_MAX_ALERT_N_REACHED_CRC,
	AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_CCI_SF_PARERR,
	AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_DDR_CHANNEL_0_PAR_WDATA_OUT_ERR,
	AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_DDR_CHANNEL_1_PAR_WDATA_OUT_ERR,
	AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_PCIE_DEV_0_FATAL_0,
	AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_PCIE_DEV_0_FATAL_1,
	AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_PCIE_DEV_0_FATAL_2,
	AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_PCIE_DEV_0_FATAL_3,
	AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_PCIE_DEV_0_FATAL_4,
	AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_PCIE_DEV_0_FATAL_5,
	AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_PCIE_DEV_0_FATAL_6,
	AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_PCIE_DEV_0_FATAL_7,
	AL_ERR_EVENTS_SYS_FABRIC_V3_SFIC_MAX_ERRORS,
};

#define AL_ERR_EVENTS_SYS_FABRIC_MAX_FIELD \
	(((unsigned int)AL_ERR_EVENTS_SYS_FABRIC_V3_SFIC_MAX_ERRORS > \
		(unsigned int)AL_ERR_EVENTS_SYS_FABRIC_MAX_ERRORS) ? \
		AL_ERR_EVENTS_SYS_FABRIC_V3_SFIC_MAX_ERRORS : \
		AL_ERR_EVENTS_SYS_FABRIC_MAX_ERRORS)

/*******************************************************************************
 ** Init params
 ******************************************************************************/
struct al_err_events_l1_init_params {
	/** L1 CPU ID */
	unsigned int cpu_id;

	/** collection mode */
	enum al_err_events_collect collect_mode;
};

struct al_err_events_l2_init_params {
	/** NB register base address */
	void __iomem *nb_regs_base;
	/** CPU cluster ID */
	unsigned int cluster_id;
	/** collection mode */
	enum al_err_events_collect collect_mode;
};

struct al_err_events_sys_fabric_init_params {
	/** NB register base address */
	void __iomem *nb_regs_base;

	/** Pointer to initialized DDR cfg object */
	struct al_ddr_cfg *ddr_cfg[2];

	/** collection mode */
	enum al_err_events_collect collect_mode;

	/** system fabric interrupt mask */
	unsigned int local_int_mask;

	/** PCIE enabled ports */
#define AL_ERR_EVENTS_SYS_FABRIC_INIT_PARAMS_HAS_PCIE_ENABLE_MAP
	uint32_t pcie_enable_map;
};

/*******************************************************************************
 ** Error Data structures
 ******************************************************************************/
struct al_err_events_l1_data {
	struct al_err_events_module module;

	struct al_err_events_field fields[AL_ERR_EVENTS_L1_MAX_ERRORS];

	struct al_cpu_mem_err_syndrome syndrome;
};

struct al_err_events_l2_data {
	struct al_err_events_module module;

	struct al_err_events_field fields[AL_ERR_EVENTS_L2_MAX_ERRORS];

	struct al_l2_mem_err_syndrome syndrome;

	struct al_sys_fabric_handle handle;

	unsigned int cluster_id;
	uint32_t nmi_sfic_bit;

	uint32_t local_int_mask;
};

struct al_err_events_sys_fabric_data {
	struct al_err_events_module module;

	struct al_err_events_field fields[AL_ERR_EVENTS_SYS_FABRIC_MAX_FIELD];

	struct al_sys_fabric_handle handle;

	struct al_ddr_cfg *ddr_cfg[2];
	unsigned int supported_ddr_ch;

	uint32_t local_int_mask;
	uint32_t pcie_enable_map;

	struct al_ddr_ecc_status ddr_corr_status;
	struct al_ddr_ecc_status ddr_uncorr_status;

	struct al_sys_fabric_sb_pos_error_info sb_pos_status;

	struct al_ddr_onchip_par_status	ddr_on_chip_status;
};

/*******************************************************************************
 ** API
 ******************************************************************************/
/*
 * Initialize PBS error events
 *
 * @param handle Error events handle
 * @param data L1 error events object
 * @param params L1 initialization parameters
 *
 * @return 0 on success, errno otherwise
 */
int al_err_events_l1_init(struct al_err_events_handle *handle,
			  struct al_err_events_l1_data *data,
			  struct al_err_events_l1_init_params *params);

/*
 * Initialize L2 error events
 *
 * @param handle Error events handle
 * @param data L2 error events object
 * @param params L2 initialization parameters
 *
 * @return 0 on success, errno otherwise
 */
int al_err_events_l2_init(struct al_err_events_handle *handle,
			  struct al_err_events_l2_data *data,
			  struct al_err_events_l2_init_params *params);

/*
 * Enable L2 error interrupts
 *
 * @param data L2 error events initialized object
 *
 * @return 0 on success, errno otherwise
 */
int al_err_events_l2_ints_enable(struct al_err_events_l2_data *data,
				 unsigned int local_int_mask);

/*
 * Initialize System Fabric error events
 *
 * @param handle Error events handle
 * @param data System Fabric error events object
 * @param params System Fabric initialization parameters
 *
 * @return 0 on success, errno otherwise
 */
int al_err_events_sys_fabric_init(struct al_err_events_handle *handle,
				  struct al_err_events_sys_fabric_data *data,
				  struct al_err_events_sys_fabric_init_params *params);

/*
 * Enable System Fabric error interrupts
 *
 * @param data System Fabric error events initialized object
 *
 * @return 0 on success, errno otherwise
 */
int al_err_events_sys_fabric_ints_enable(struct al_err_events_sys_fabric_data *data);

#endif /* __AL_ERR_EVENTS_SYS_FABRIC_H__ */
