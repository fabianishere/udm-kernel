#ifndef __AL_HAL_SYS_FABRIC_UTILS_H__
#define __AL_HAL_SYS_FABRIC_UTILS_H__

#include "al_hal_common.h"
#include "al_hal_nb_regs.h"

/* Normal power mode state */
#define NB_CPUN_CONFIG_STATUS_POWER_CTRL_PM_REQ_NORMAL		0
/* Dormant power mode state */
#define NB_CPUN_CONFIG_STATUS_POWER_CTRL_PM_REQ_DEEP_IDLE	2
/* Powered-off power mode */
#define NB_CPUN_CONFIG_STATUS_POWER_CTRL_PM_REQ_POWEREDOFF	3

/*
 * L2 RAMs Power Down
 * Power down the L2 RAMs. L2 caches must be flushed prior to entering this state.
 */
#ifndef NB_GLOBAL_CPUS_POWER_CTRL_L2RAMS_PWRDN_EN
#define NB_GLOBAL_CPUS_POWER_CTRL_L2RAMS_PWRDN_EN		AL_BIT(2)
#endif
/*
 * Enable external debugger over power-down.
 * Provides support for external debug over power down. If any or all of the processors are powered
 * down, the SoC can still use the debug facilities if the debug PCLKDBG domain is powered up.
 */
#ifndef NB_GLOBAL_CPUS_POWER_CTRL_EXT_DEBUGGER_OVER_PD_EN
#define NB_GLOBAL_CPUS_POWER_CTRL_EXT_DEBUGGER_OVER_PD_EN	AL_BIT(8)
#endif

/**
 * NMI & Error iofics are relevant for Alpine v3 only
 */
/** Error SFIC */
#define AL_SYS_FABRIC_SFIC_ERR_GROUP_A_CPU_AXIERRIRQ_N_0                                 AL_BIT(0)
#define AL_SYS_FABRIC_SFIC_ERR_GROUP_A_CPU_AXIERRIRQ_N_1                                 AL_BIT(1)
#define AL_SYS_FABRIC_SFIC_ERR_GROUP_A_CPU_AXIERRIRQ_N_2                                 AL_BIT(2)
#define AL_SYS_FABRIC_SFIC_ERR_GROUP_A_CPU_AXIERRIRQ_N_3                                 AL_BIT(3)
#define AL_SYS_FABRIC_SFIC_ERR_GROUP_A_CPU_RD_DATA_PAR_ERR_0                             AL_BIT(4)
#define AL_SYS_FABRIC_SFIC_ERR_GROUP_A_CPU_RD_DATA_PAR_ERR_1                             AL_BIT(5)
#define AL_SYS_FABRIC_SFIC_ERR_GROUP_A_CPU_RD_DATA_PAR_ERR_2                             AL_BIT(6)
#define AL_SYS_FABRIC_SFIC_ERR_GROUP_A_CPU_RD_DATA_PAR_ERR_3                             AL_BIT(7)
#define AL_SYS_FABRIC_SFIC_ERR_GROUP_A_CCI_ERRO_IRQ_N                                    AL_BIT(8)
#define AL_SYS_FABRIC_SFIC_ERR_GROUP_A_CORESIGHT_TSCNT_OVFL_INTR                         AL_BIT(10)
#define AL_SYS_FABRIC_SFIC_ERR_GROUP_A_GIC_ECC_FATAL                                     AL_BIT(11)
#define AL_SYS_FABRIC_SFIC_ERR_GROUP_A_FABRIC_MSIX_ERR_INTR                              AL_BIT(12)
#define AL_SYS_FABRIC_SFIC_ERR_GROUP_A_FABRIC_POS_ERR_IRQ                                AL_BIT(13)
#define AL_SYS_FABRIC_SFIC_ERR_GROUP_A_FABRIC_PP_SRAM_PARITY_ERR                         AL_BIT(14)
#define AL_SYS_FABRIC_SFIC_ERR_GROUP_A_FABRIC_C2SWB_PARITY_ERR                           AL_BIT(15)
#define AL_SYS_FABRIC_SFIC_ERR_GROUP_A_FABRIC_SB_ARREQ_PAR_ERR_0                         AL_BIT(16)
#define AL_SYS_FABRIC_SFIC_ERR_GROUP_A_FABRIC_SB_ARREQ_PAR_ERR_1                         AL_BIT(17)
#define AL_SYS_FABRIC_SFIC_ERR_GROUP_A_FABRIC_SB_ARREQ_PAR_ERR_2                         AL_BIT(18)
#define AL_SYS_FABRIC_SFIC_ERR_GROUP_A_FABRIC_SB_ARREQ_PAR_ERR_3                         AL_BIT(19)
#define AL_SYS_FABRIC_SFIC_ERR_GROUP_A_DDR_CAUSE_SUM_0                                   AL_BIT(20)
#define AL_SYS_FABRIC_SFIC_ERR_GROUP_A_DDR_CAUSE_SUM_1                                   AL_BIT(21)
#define AL_SYS_FABRIC_SFIC_ERR_GROUP_A_IO_MMU_PTW_RD_DATA_PAR_ERR                        AL_BIT(22)
#define AL_SYS_FABRIC_SFIC_ERR_GROUP_A_IO_CACHE_MEMORY_UNCORR_ECC_0                      AL_BIT(24)
#define AL_SYS_FABRIC_SFIC_ERR_GROUP_A_IO_CACHE_MEMORY_UNCORR_ECC_1                      AL_BIT(25)
#define AL_SYS_FABRIC_SFIC_ERR_GROUP_A_IO_CACHE_MEMORY_CORR_ECC_0                        AL_BIT(26)
#define AL_SYS_FABRIC_SFIC_ERR_GROUP_A_IO_CACHE_MEMORY_CORR_ECC_1                        AL_BIT(27)
#define AL_SYS_FABRIC_SFIC_ERR_GROUP_A_IO_CACHE_PARITY_ERROR_0                           AL_BIT(28)
#define AL_SYS_FABRIC_SFIC_ERR_GROUP_A_IO_CACHE_PARITY_ERROR_1                           AL_BIT(29)

/** NMI SFIC bits */
#define AL_SYS_FABRIC_SFIC_NMI_GROUP_A_DDR_CHANNEL_0_UNCORRECTABLE_ECC                   AL_BIT(0)
#define AL_SYS_FABRIC_SFIC_NMI_GROUP_A_DDR_CHANNEL_0_CORRECTABLE_ECC                     AL_BIT(1)
#define AL_SYS_FABRIC_SFIC_NMI_GROUP_A_DDR_CHANNEL_0_DFI_ALERT_N_CRC                     AL_BIT(2)
#define AL_SYS_FABRIC_SFIC_NMI_GROUP_A_DDR_CHANNEL_0_MAX_ALERT_N_REACHED_CRC             AL_BIT(3)
#define AL_SYS_FABRIC_SFIC_NMI_GROUP_A_DDR_CHANNEL_1_UNCORRECTABLE_ECC                   AL_BIT(4)
#define AL_SYS_FABRIC_SFIC_NMI_GROUP_A_DDR_CHANNEL_1_CORRECTABLE_ECC                     AL_BIT(5)
#define AL_SYS_FABRIC_SFIC_NMI_GROUP_A_DDR_CHANNEL_1_DFI_ALERT_N_CRC                     AL_BIT(6)
#define AL_SYS_FABRIC_SFIC_NMI_GROUP_A_DDR_CHANNEL_1_MAX_ALERT_N_REACHED_CRC             AL_BIT(7)
#define AL_SYS_FABRIC_SFIC_NMI_GROUP_A_CPU_INTR_ERR_N0                                   AL_BIT(8)
#define AL_SYS_FABRIC_SFIC_NMI_GROUP_A_CPU_INTR_ERR_N1                                   AL_BIT(9)
#define AL_SYS_FABRIC_SFIC_NMI_GROUP_A_CPU_INTR_ERR_N2                                   AL_BIT(10)
#define AL_SYS_FABRIC_SFIC_NMI_GROUP_A_CPU_INTR_ERR_N3                                   AL_BIT(11)
#define AL_SYS_FABRIC_SFIC_NMI_GROUP_A_ERROR_SFIC_SUMMARY                                AL_BIT(12)
#define AL_SYS_FABRIC_SFIC_NMI_GROUP_A_CCI_SF_PARERR                                     AL_BIT(13)
#define AL_SYS_FABRIC_SFIC_NMI_GROUP_A_DDR_CHANNEL_0_PAR_WDATA_OUT_ERR                   AL_BIT(14)
#define AL_SYS_FABRIC_SFIC_NMI_GROUP_A_DDR_CHANNEL_1_PAR_WDATA_OUT_ERR                   AL_BIT(15)
#define AL_SYS_FABRIC_SFIC_NMI_GROUP_A_PCIE_DEV_0_FATAL_0                                AL_BIT(16)
#define AL_SYS_FABRIC_SFIC_NMI_GROUP_A_PCIE_DEV_0_FATAL_1                                AL_BIT(17)
#define AL_SYS_FABRIC_SFIC_NMI_GROUP_A_PCIE_DEV_0_FATAL_2                                AL_BIT(18)
#define AL_SYS_FABRIC_SFIC_NMI_GROUP_A_PCIE_DEV_0_FATAL_3                                AL_BIT(19)
#define AL_SYS_FABRIC_SFIC_NMI_GROUP_A_PCIE_DEV_0_FATAL_4                                AL_BIT(20)
#define AL_SYS_FABRIC_SFIC_NMI_GROUP_A_PCIE_DEV_0_FATAL_5                                AL_BIT(21)
#define AL_SYS_FABRIC_SFIC_NMI_GROUP_A_PCIE_DEV_0_FATAL_6                                AL_BIT(22)
#define AL_SYS_FABRIC_SFIC_NMI_GROUP_A_PCIE_DEV_0_FATAL_7                                AL_BIT(23)
#define AL_SYS_FABRIC_SFIC_NMI_GROUP_A_WD_TIMER_0                                        AL_BIT(24)
#define AL_SYS_FABRIC_SFIC_NMI_GROUP_A_WD_TIMER_1                                        AL_BIT(25)
#define AL_SYS_FABRIC_SFIC_NMI_GROUP_A_WD_TIMER_2                                        AL_BIT(26)
#define AL_SYS_FABRIC_SFIC_NMI_GROUP_A_WD_TIMER_3                                        AL_BIT(27)
#define AL_SYS_FABRIC_SFIC_NMI_GROUP_A_WD_TIMER_4                                        AL_BIT(28)
#define AL_SYS_FABRIC_SFIC_NMI_GROUP_A_WD_TIMER_5                                        AL_BIT(29)
#define AL_SYS_FABRIC_SFIC_NMI_GROUP_A_WD_TIMER_6                                        AL_BIT(30)
#define AL_SYS_FABRIC_SFIC_NMI_GROUP_A_WD_TIMER_7                                        AL_BIT(31)

/** PMU */
#define AL_SYS_FABRIC_SFIC_PMU_INT_GROUP_A_CPU_NPMUIRQ_0                                 AL_BIT(0)
#define AL_SYS_FABRIC_SFIC_PMU_INT_GROUP_A_CPU_NPMUIRQ_1                                 AL_BIT(1)
#define AL_SYS_FABRIC_SFIC_PMU_INT_GROUP_A_CPU_NPMUIRQ_2                                 AL_BIT(2)
#define AL_SYS_FABRIC_SFIC_PMU_INT_GROUP_A_CPU_NPMUIRQ_3                                 AL_BIT(3)
#define AL_SYS_FABRIC_SFIC_PMU_INT_GROUP_A_CPU_NPMUIRQ_4                                 AL_BIT(4)
#define AL_SYS_FABRIC_SFIC_PMU_INT_GROUP_A_CPU_NPMUIRQ_5                                 AL_BIT(5)
#define AL_SYS_FABRIC_SFIC_PMU_INT_GROUP_A_CPU_NPMUIRQ_6                                 AL_BIT(6)
#define AL_SYS_FABRIC_SFIC_PMU_INT_GROUP_A_CPU_NPMUIRQ_7                                 AL_BIT(7)
#define AL_SYS_FABRIC_SFIC_PMU_INT_GROUP_A_CPU_NPMUIRQ_8                                 AL_BIT(8)
#define AL_SYS_FABRIC_SFIC_PMU_INT_GROUP_A_CPU_NPMUIRQ_9                                 AL_BIT(9)
#define AL_SYS_FABRIC_SFIC_PMU_INT_GROUP_A_CPU_NPMUIRQ_10                                AL_BIT(10)
#define AL_SYS_FABRIC_SFIC_PMU_INT_GROUP_A_CPU_NPMUIRQ_11                                AL_BIT(11)
#define AL_SYS_FABRIC_SFIC_PMU_INT_GROUP_A_CPU_NPMUIRQ_12                                AL_BIT(12)
#define AL_SYS_FABRIC_SFIC_PMU_INT_GROUP_A_CPU_NPMUIRQ_13                                AL_BIT(13)
#define AL_SYS_FABRIC_SFIC_PMU_INT_GROUP_A_CPU_NPMUIRQ_14                                AL_BIT(14)
#define AL_SYS_FABRIC_SFIC_PMU_INT_GROUP_A_CPU_NPMUIRQ_15                                AL_BIT(15)
#define AL_SYS_FABRIC_SFIC_PMU_INT_GROUP_A_IO_MMU_PERF_IRPT_0                            AL_BIT(16)
#define AL_SYS_FABRIC_SFIC_PMU_INT_GROUP_A_IO_MMU_PERF_IRPT_1                            AL_BIT(17)
#define AL_SYS_FABRIC_SFIC_PMU_INT_GROUP_A_IO_MMU_PERF_IRPT_2                            AL_BIT(18)
#define AL_SYS_FABRIC_SFIC_PMU_INT_GROUP_A_IO_MMU_PERF_IRPT_3                            AL_BIT(19)
#define AL_SYS_FABRIC_SFIC_PMU_INT_GROUP_A_CCI_PMU_OVERFLOW_0                            AL_BIT(20)
#define AL_SYS_FABRIC_SFIC_PMU_INT_GROUP_A_CCI_PMU_OVERFLOW_1                            AL_BIT(21)
#define AL_SYS_FABRIC_SFIC_PMU_INT_GROUP_A_CCI_PMU_OVERFLOW_2                            AL_BIT(22)
#define AL_SYS_FABRIC_SFIC_PMU_INT_GROUP_A_CCI_PMU_OVERFLOW_3                            AL_BIT(23)
#define AL_SYS_FABRIC_SFIC_PMU_INT_GROUP_A_CCI_PMU_OVERFLOW_4                            AL_BIT(24)
#define AL_SYS_FABRIC_SFIC_PMU_INT_GROUP_A_CCI_PMU_OVERFLOW_5                            AL_BIT(25)
#define AL_SYS_FABRIC_SFIC_PMU_INT_GROUP_A_CCI_PMU_OVERFLOW_6                            AL_BIT(26)
#define AL_SYS_FABRIC_SFIC_PMU_INT_GROUP_A_CCI_PMU_OVERFLOW_7                            AL_BIT(27)

/** IOMMU */
/* Group A holds the LSB of the context interrupt (31:0) */
#define AL_SYS_FABRIC_SFIC_IOMMU_GROUP_A_CTX_INT(x)	AL_BIT(0)
#define AL_SYS_FABRIC_SFIC_IOMMU_GROUP_A_CTX_INT_MAX	31
/* Group A holdsBthe MSB of the context interrupt (63:32) */
#define AL_SYS_FABRIC_SFIC_IOMMU_GROUP_B_CTX_INT(x)	AL_BIT(0)
#define AL_SYS_FABRIC_SFIC_IOMMU_GROUP_B_CTX_INT_MAX	31

/** Coresight */
#define AL_SYS_FABRIC_SFIC_CORESIGHT_GROUP_A_COMM_RX_INT(x)	AL_BIT(x)
#define AL_SYS_FABRIC_SFIC_CORESIGHT_GROUP_A_COMM_RX_MAX	15
#define AL_SYS_FABRIC_SFIC_CORESIGHT_GROUP_A_COMM_TX_INT(x)	AL_BIT(16 + (x))
#define AL_SYS_FABRIC_SFIC_CORESIGHT_GROUP_A_COMM_TX_MAX	31

/** External interrupt */
/*
 * There are 2 ext interrupts SFICS
 * both share the same interrupt bits layout
 *
 * Group A - SB interrupts 0 - 31
 * Group B - SB interrupts 32 - 63
 * Group C - SB interrupts 64 - 95
 * Group D - MSIx interrupts 928 - 959
*/
#define AL_SYS_FABRIC_SFIC_EXT_INT_GROUP_A_SB_INT(x)		AL_BIT(x)
#define AL_SYS_FABRIC_SFIC_EXT_INT_GROUP_B_SB_INT(x)		AL_BIT(x)
#define AL_SYS_FABRIC_SFIC_EXT_INT_GROUP_C_SB_INT(x)		AL_BIT(x)
#define AL_SYS_FABRIC_SFIC_EXT_INT_SB_INT_MAX			31

/*
 * SB PoS error info
 */
struct al_sys_fabric_sb_pos_error_info {
	/* Set on SB PoS error occurrence */
	al_bool valid;

	/* Write address that caused the error */
	uint64_t address;

	/* Transaction AXI ID */
	uint8_t request_id;

	/*
	 * Response
	 * 0x0: OK
	 * 0x1: Exclusive OK
	 * 0x2: Slave error
	 * 0x3: Decode error
	 */
	uint8_t bresp;
};

/** System fabric quality of service configuration */
struct al_sys_fabric_qos_cfg {
	/** CPU Low priority Read QoS (0-15) - Relevant to Alpine V1/V2 only */
	unsigned int cpu_lp_arqos;
	/** CPU High priority Read QoS (0-15) - Relevant to Alpine V1/V2 only */
	unsigned int cpu_hp_arqos;
	/** CPU Low priority Write QoS (0-15) - Relevant to Alpine V1/V2 only */
	unsigned int cpu_lp_awqos;
	/** CPU High priority Write QoS (0-15) - Relevant to Alpine V1/V2 only */
	unsigned int cpu_hp_awqos;
	/** SB Low priority Read QoS (0-15) - Relevant to Alpine V1/V2 only */
	unsigned int sb_lp_arqos;
	/** SB Low-priority Write QoS (0-15) - Relevant to Alpine V1/V2 only */
	unsigned int sb_lp_awqos;
};

/** Interrupt controller ID */
enum al_sys_fabric_iofic_id {
	/** Error interrupt controller */
	AL_SYS_FABRIC_IOFIC_ID_ERR,
	/** PMU interrupt controller */
	AL_SYS_FABRIC_IOFIC_ID_PMU,
	/** Coresight interrupt controller */
	AL_SYS_FABRIC_IOFIC_ID_CORESIGHT,
	/** I/O MMU interrupt controller */
	AL_SYS_FABRIC_IOFIC_ID_IOMMU,
	/** External interrupt controller */
	AL_SYS_FABRIC_IOFIC_ID_EXTERNAL,
	/** NMI interrupt controller */
	AL_SYS_FABRIC_IOFIC_ID_NMI,
};

/** Write splitter mode. Relevant only for Alpine V2 */
enum al_sys_fabric_wr_split_mode {
	SYS_FABRIC_WR_SPLIT_MODE_UNKNOWN,
	SYS_FABRIC_WR_SPLIT_MODE_ALPINE_V1,
	SYS_FABRIC_WR_SPLIT_MODE_ALPINE_V2
};

struct al_sys_fabric_cluster_handle;
struct al_sys_fabric_handle;
struct al_sys_fabric_iocache_usage_cfg;

#if (defined(AL_DEV_ID))
#define al_sys_fabric_handle_init(...)				\
	_al_sys_fabric_handle_init(__VA_ARGS__)
#define al_sys_fabric_cluster_handle_init(...)			\
	_al_sys_fabric_cluster_handle_init(__VA_ARGS__)
#define al_sys_fabric_int_local_mask_read(...)			\
	_al_sys_fabric_int_local_mask_read(__VA_ARGS__)
#define al_sys_fabric_int_local_unmask(...)			\
	_al_sys_fabric_int_local_unmask(__VA_ARGS__)
#define al_sys_fabric_int_local_mask(...)			\
	_al_sys_fabric_int_local_mask(__VA_ARGS__)
#define al_sys_fabric_int_clear(...)				\
	_al_sys_fabric_int_clear(__VA_ARGS__)
#define al_sys_fabric_int_cause_read(...)			\
	_al_sys_fabric_int_cause_read(__VA_ARGS__)
#define al_sys_fabric_error_cause_read(...)			\
	_al_sys_fabric_error_cause_read(__VA_ARGS__)
#define al_sys_fabric_error_int_mask_read(...)			\
	_al_sys_fabric_error_int_mask_read(__VA_ARGS__)
#define al_sys_fabric_error_int_unmask(...)			\
	_al_sys_fabric_error_int_unmask(__VA_ARGS__)
#define al_sys_fabric_error_int_mask(...)			\
	_al_sys_fabric_error_int_mask(__VA_ARGS__)
#define al_sys_fabric_error_int_clear(...)			\
	_al_sys_fabric_error_int_clear(__VA_ARGS__)
#define al_sys_fabric_sb_pos_info_get_and_clear(...)		\
	_al_sys_fabric_sb_pos_info_get_and_clear(__VA_ARGS__)
#define al_sys_fabric_core_msg_set(...)				\
	_al_sys_fabric_core_msg_set(__VA_ARGS__)
#define al_sys_fabric_core_msg_get(...)				\
	_al_sys_fabric_core_msg_get(__VA_ARGS__)
#define al_sys_fabric_sys_counter_freq_get(...)			\
	_al_sys_fabric_sys_counter_freq_get(__VA_ARGS__)
#define al_sys_fabric_sys_counter_get_64(...)			\
	_al_sys_fabric_sys_counter_get_64(__VA_ARGS__)
#define al_sys_fabric_sys_counter_get_32(...)			\
	_al_sys_fabric_sys_counter_get_32(__VA_ARGS__)
#define al_sys_fabric_hw_semaphore_lock(...)			\
	_al_sys_fabric_hw_semaphore_lock(__VA_ARGS__)
#define al_sys_fabric_hw_semaphore_unlock(...)			\
	_al_sys_fabric_hw_semaphore_unlock(__VA_ARGS__)
#define al_sys_fabric_cluster_pd_pu_timer_set(...)		\
	_al_sys_fabric_cluster_pd_pu_timer_set(__VA_ARGS__)
#define al_sys_fabric_cluster_power_ctrl(...)			\
	_al_sys_fabric_cluster_power_ctrl(__VA_ARGS__)
#define al_sys_fabric_core_power_ctrl(...)			\
	_al_sys_fabric_core_power_ctrl(__VA_ARGS__)
#define al_sys_fabric_cluster_sw_reset(...)			\
	_al_sys_fabric_cluster_sw_reset(__VA_ARGS__)
#define al_sys_fabric_core_sw_reset(...)			\
	_al_sys_fabric_core_sw_reset(__VA_ARGS__)
#define al_sys_fabric_core_power_on_reset(...)			\
	_al_sys_fabric_core_power_on_reset(__VA_ARGS__)
#define al_sys_fabric_core_reset_deassert(...)			\
	_al_sys_fabric_core_reset_deassert(__VA_ARGS__)
#define al_sys_fabric_core_aarch64_setup(...)			\
	_al_sys_fabric_core_aarch64_setup(__VA_ARGS__)
#define al_sys_fabric_core_aarch32_setup(...)			\
	_al_sys_fabric_core_aarch32_setup(__VA_ARGS__)
#define al_sys_fabric_core_aarch32_setup_get(...)		\
	_al_sys_fabric_core_aarch32_setup_get(__VA_ARGS__)
#define al_sys_fabric_iofic_regs_get(...)			\
	_al_sys_fabric_iofic_regs_get(__VA_ARGS__)
#define al_sys_fabric_iocache_usage_cfg_set(...)		\
	_al_sys_fabric_iocache_usage_cfg_set(__VA_ARGS__)
#define al_sys_fabric_wr_split_mode_set(...)			\
	_al_sys_fabric_wr_split_mode_set(__VA_ARGS__)
#define al_sys_fabric_wr_split_mode_get(...)			\
	_al_sys_fabric_wr_split_mode_get(__VA_ARGS__)
#define al_sys_fabric_error_ints_mask_get(...)			\
	_al_sys_fabric_error_ints_mask_get(__VA_ARGS__)
#define al_sys_fabric_error_unmask(...)				\
	_al_sys_fabric_error_unmask(__VA_ARGS__)
#define al_sys_fabric_nmi_ints_mask_get(...)			\
	_al_sys_fabric_nmi_ints_mask_get(__VA_ARGS__)
#define al_sys_fabric_nmi_unmask(...)				\
	_al_sys_fabric_nmi_unmask(__VA_ARGS__)
#define al_sys_fabric_nmi_sfic_regs_base_get(...)		\
	_al_sys_fabric_nmi_sfic_regs_base_get(__VA_ARGS__)
#define al_sys_fabric_err_sfic_regs_base_get(...)		\
	_al_sys_fabric_err_sfic_regs_base_get(__VA_ARGS__)
#define al_sys_fabric_qos_cfg_set(...)				\
	_al_sys_fabric_qos_cfg_set(__VA_ARGS__)
#define al_sys_fabric_qos_cfg_get(...)				\
	_al_sys_fabric_qos_cfg_get(__VA_ARGS__)
#define al_sys_fabric_iof_2_iof_path_en(...)				\
	_al_sys_fabric_iof_2_iof_path_en(__VA_ARGS__)
#else
struct al_sys_fabric_imp {
	void (*al_sys_fabric_handle_init)(
		struct al_sys_fabric_handle		*handle,
		void __iomem				*nb_regs_base);

	void (*al_sys_fabric_cluster_handle_init)(
		struct al_sys_fabric_cluster_handle	*handle,
		const struct al_sys_fabric_handle	*fabric_handle,
		void __iomem				*anpa_regs_base);

	uint32_t (*al_sys_fabric_int_local_mask_read)(
		const struct al_sys_fabric_handle	*handle,
		unsigned int				idx);

	void (*al_sys_fabric_int_local_unmask)(
		const struct al_sys_fabric_handle	*handle,
		unsigned int				idx,
		unsigned int				mask);

	void (*al_sys_fabric_int_local_mask)(
		const struct al_sys_fabric_handle	*handle,
		unsigned int				idx,
		unsigned int				mask);

	void (*al_sys_fabric_int_clear)(
		const struct al_sys_fabric_handle	*handle,
		unsigned int				mask);

	void (*al_sys_fabric_int_cause_read)(
		const struct al_sys_fabric_handle	*handle,
		uint32_t				*cause);

	void (*al_sys_fabric_error_cause_read)(
		const struct al_sys_fabric_handle	*handle,
		uint32_t				*cause);

	uint32_t (*al_sys_fabric_error_int_mask_read)(
		const struct al_sys_fabric_handle	*handle);

	void (*al_sys_fabric_error_int_unmask)(
		const struct al_sys_fabric_handle	*handle,
		unsigned int				mask);

	void (*al_sys_fabric_error_int_mask)(
		const struct al_sys_fabric_handle	*handle,
		unsigned int				mask);

	void (*al_sys_fabric_error_int_clear)(
		const struct al_sys_fabric_handle	*handle,
		unsigned int				mask);

	void (*al_sys_fabric_sb_pos_info_get_and_clear)(
		const struct al_sys_fabric_handle	*handle,
		struct al_sys_fabric_sb_pos_error_info	*pos_info);

	void (*al_sys_fabric_core_msg_set)(
		const struct al_sys_fabric_cluster_handle	*handle,
		unsigned int					core,
		unsigned int					msg,
		al_bool						concat_reg_addr);

	void (*al_sys_fabric_core_msg_get)(
		const struct al_sys_fabric_cluster_handle	*handle,
		unsigned int					core,
		unsigned int					*msg_valid,
		unsigned int					*msg);

	unsigned int (*al_sys_fabric_sys_counter_freq_get)(
		const struct al_sys_fabric_handle	*handle);

	uint64_t (*al_sys_fabric_sys_counter_get_64)(
		const struct al_sys_fabric_handle	*handle);

	uint32_t (*al_sys_fabric_sys_counter_get_32)(
		const struct al_sys_fabric_handle	*handle);

	int (*al_sys_fabric_hw_semaphore_lock)(
		const struct al_sys_fabric_handle	*handle,
		unsigned int				id);

	void (*al_sys_fabric_hw_semaphore_unlock)(
		const struct al_sys_fabric_handle	*handle,
		unsigned int				id);

	void (*al_sys_fabric_cluster_pd_pu_timer_set)(
		const struct al_sys_fabric_cluster_handle	*handle,
		unsigned int					pd,
		unsigned int					pu);

	void (*al_sys_fabric_cluster_power_ctrl)(
		const struct al_sys_fabric_cluster_handle	*handle,
		unsigned int					ctrl);

	void (*al_sys_fabric_core_power_ctrl)(
		const struct al_sys_fabric_cluster_handle	*handle,
		unsigned int					core,
		unsigned int					ctrl);

	void (*al_sys_fabric_cluster_sw_reset)(
		const struct al_sys_fabric_cluster_handle	*handle);

	void (*al_sys_fabric_core_sw_reset)(
		const struct al_sys_fabric_cluster_handle	*handle,
		unsigned int					core);

	void (*al_sys_fabric_core_power_on_reset)(
		const struct al_sys_fabric_cluster_handle	*handle,
		unsigned int					core);

	void (*al_sys_fabric_core_reset_deassert)(
		const struct al_sys_fabric_cluster_handle	*handle,
		unsigned int					core);

	void (*al_sys_fabric_core_aarch64_setup)(
		const struct al_sys_fabric_cluster_handle	*handle,
		unsigned int					core,
		uint32_t					entry_high,
		uint32_t					entry_low);

	void (*al_sys_fabric_core_aarch32_setup)(
		const struct al_sys_fabric_cluster_handle	*handle,
		unsigned int					core,
		uint32_t					entry_high,
		uint32_t					entry_low);

	void (*al_sys_fabric_core_aarch32_setup_get)(
		const struct al_sys_fabric_cluster_handle	*handle,
		unsigned int					core,
		uint32_t					*entry_high,
		uint32_t					*entry_low);

	struct al_iofic_regs __iomem * (*al_sys_fabric_iofic_regs_get)(
		const struct al_sys_fabric_handle		*handle,
		enum al_sys_fabric_iofic_id			id);

	void (*al_sys_fabric_iocache_usage_cfg_set)(
		const struct al_sys_fabric_handle		*handle,
		const struct al_sys_fabric_iocache_usage_cfg	*cfg);

	void (*al_sys_fabric_wr_split_mode_set)(
			const struct al_sys_fabric_handle	*handle,
			enum al_sys_fabric_wr_split_mode	mode);

	void (*al_sys_fabric_wr_split_mode_get)(
			const struct al_sys_fabric_handle	*handle,
			enum al_sys_fabric_wr_split_mode	*mode);

	void (*al_sys_fabric_error_ints_mask_get)(
		const struct al_sys_fabric_handle	*handle,
		uint32_t				*a,
		uint32_t				*b,
		uint32_t				*c,
		uint32_t				*d);

	void (*al_sys_fabric_error_unmask)(
		const struct al_sys_fabric_handle	*handle);

	void (*al_sys_fabric_nmi_ints_mask_get)(
		const struct al_sys_fabric_handle	*handle,
		uint32_t				*a,
		uint32_t				*b,
		uint32_t				*c,
		uint32_t				*d);

	void (*al_sys_fabric_nmi_unmask)(
		const struct al_sys_fabric_handle	*handle);

	void __iomem * (*al_sys_fabric_nmi_sfic_regs_base_get)(
		const struct al_sys_fabric_handle *handle);

	void __iomem * (*al_sys_fabric_err_sfic_regs_base_get)(
		const struct al_sys_fabric_handle *handle);

	void (*al_sys_fabric_qos_cfg_set)(
		const struct al_sys_fabric_handle	*handle,
		const struct al_sys_fabric_qos_cfg	*cfg);

	void (*al_sys_fabric_qos_cfg_get)(
		const struct al_sys_fabric_handle	*handle,
		struct al_sys_fabric_qos_cfg		*cfg);

	void (*al_sys_fabric_iof_2_iof_path_en)(
		const struct al_sys_fabric_handle	*handle,
		al_bool					en);
};

#endif

/**
 * System fabric handle
 */
struct al_sys_fabric_handle {
	unsigned int			ver;
	void __iomem			*nb_regs_base;
	void __iomem			*error_sfic_base;
	void __iomem			*nmi_sfic_base;

#if (!defined(AL_DEV_ID))
	struct al_sys_fabric_imp	imp;
#endif
};

/**
 * System fabric cluster handle
 */
struct al_sys_fabric_cluster_handle {
	const struct al_sys_fabric_handle	*fabric_handle;
	void __iomem			*anpa_regs_base;
};

/**
 * System fabric handle initialization
 *
 * @param	handle
 *		System fabric clear handle
 * @param	nb_regs_base
 *		NB service registers base address
 */
void al_sys_fabric_handle_init(
	struct al_sys_fabric_handle	*handle,
	void __iomem			*nb_regs_base);

/**
 * System fabric cluster handle initialization
 *
 * @param	handle
 *		System fabric cluster clear handle
 * @param	fabric_handle
 *		System fabric initialized handle
 * @param	anpa_regs_base
 *		ANPA registers base address
 */
void al_sys_fabric_cluster_handle_init(
	struct al_sys_fabric_cluster_handle	*handle,
	const struct al_sys_fabric_handle	*fabric_handle,
	void __iomem				*anpa_regs_base);

/**
 * System fabric local interrupt mask read
 *
 * @param	handle
 *		System fabric initialized handle
 * @param	idx
 *		Local interrupt index
 * @returns	System fabric interrupt mask value
 */
uint32_t al_sys_fabric_int_local_mask_read(
	const struct al_sys_fabric_handle	*handle,
	unsigned int			idx);

/**
 * System fabric interrupt local unmasking
 *
 * @param	handle
 *		System fabric initialized handle
 * @param	idx
 *		Local interrupt index
 * @param	mask
 *		Interrupts to unmask (see NB_GLOBAL_NB_INT_CAUSE_*)
 */
void al_sys_fabric_int_local_unmask(
	const struct al_sys_fabric_handle	*handle,
	unsigned int			idx,
	unsigned int			mask);

/**
 * System fabric interrupt local masking
 *
 * @param	handle
 *		System fabric initialized handle
 * @param	idx
 *		Local interrupt index
 * @param	mask
 *		Interrupts to mask (see NB_GLOBAL_NB_INT_CAUSE_*)
 */
void al_sys_fabric_int_local_mask(
	const struct al_sys_fabric_handle	*handle,
	unsigned int			idx,
	unsigned int			mask);

/**
 * System fabric interrupt clearing
 *
 * @param	handle
 *		System fabric initialized handle
 * @param	mask
 *		Interrupts to clear (see NB_GLOBAL_NB_INT_CAUSE_*)
 */
void al_sys_fabric_int_clear(
	const struct al_sys_fabric_handle	*handle,
	unsigned int			mask);

/**
 * System fabric interrupt cause read
 *
 * @param	handle
 *		System fabric initialized handle
 * @param	cause
 *		pointer for returned value
 */
void al_sys_fabric_int_cause_read(const struct al_sys_fabric_handle	*handle, uint32_t *cause);

/**
 * System fabric interrupt error cause read
 *
 * @param	handle
 *		System fabric initialized handle
 * @param	cause
 *		pointer for returned value
 */
void al_sys_fabric_error_cause_read(const struct al_sys_fabric_handle	*handle, uint32_t *cause);

/**
 * System fabric error interrupt read mask
 *
 * @param	handle
 *		System fabric initialized handle
 * @returns	System fabric error interrupt mask
 */
uint32_t al_sys_fabric_error_int_mask_read(
	const struct al_sys_fabric_handle	*handle);

/**
 * System fabric error interrupt unmasking
 *
 * @param	handle
 *		System fabric initialized handle
 * @param	mask
 *		Interrupts to unmask (see NB_GLOBAL_NB_ERROR_CAUSE_*)
 */
void al_sys_fabric_error_int_unmask(
	const struct al_sys_fabric_handle	*handle,
	unsigned int			mask);

/**
 * System fabric error interrupt masking
 *
 * @param	handle
 *		System fabric initialized handle
 * @param	mask
 *		Interrupts to mask (see NB_GLOBAL_NB_ERROR_CAUSE_*)
 */
void al_sys_fabric_error_int_mask(
	const struct al_sys_fabric_handle	*handle,
	unsigned int			mask);

/**
 * System fabric error interrupt clearing
 *
 * @param	handle
 *		System fabric initialized handle
 * @param	mask
 *		Interrupts to clear (see NB_GLOBAL_NB_ERROR_CAUSE_*)
 */
void al_sys_fabric_error_int_clear(
	const struct al_sys_fabric_handle	*handle,
	unsigned int			mask);

/**
 * System fabric SB PoS error interrupt clearing and info retrieve
 *
 * @param	handle
 *		System fabric initialized handle
 * @param	pos_info
 *		pos_info instance
 */
void al_sys_fabric_sb_pos_info_get_and_clear(
	const struct al_sys_fabric_handle	*handle,
	struct al_sys_fabric_sb_pos_error_info	*pos_info);

/**
 * System fabric core message setting
 *
 * @param	handle
 *		System fabric cluster initialized handle
 * @param	core
 *		Core index (within cluster)
 * @param	msg
 *		Message to set
 *		Note: message meaning depends on implementation
 * @param	concat_reg_addr
 *		Concatenate the low address halfword to the data, because it simplifies matching
 *		data to address in waves/logs
 */
void al_sys_fabric_core_msg_set(
	const struct al_sys_fabric_cluster_handle	*handle,
	unsigned int				core,
	unsigned int				msg,
	al_bool					concat_reg_addr);

/**
 * System fabric core message getting
 *
 * @param	handle
 *		System fabric cluster initialized handle
 * @param	core
 *		Core index (within cluster)
 * @param	msg_valid
 *		An indication whether the message is valid
 * @param	msg
 *		Message received
 *		Note: message meaning depends on implementation
 */
void al_sys_fabric_core_msg_get(
	const struct al_sys_fabric_cluster_handle	*handle,
	unsigned int				core,
	unsigned int				*msg_valid,
	unsigned int				*msg);

/**
 * System fabric system counter get frequency
 *
 * @param	handle
 *		System fabric initialized handle
 *
 * @returns	System fabric system counter frequency [Hz]
 */
unsigned int al_sys_fabric_sys_counter_freq_get(
	const struct al_sys_fabric_handle	*handle);

/**
 * System fabric system counter get 64 bits value
 *
 * @param	handle
 *		System fabric initialized handle
 *
 * @returns	System fabric system counter value (64 bits)
 */
uint64_t al_sys_fabric_sys_counter_get_64(
	const struct al_sys_fabric_handle	*handle);

/**
 * System fabric system counter get 32 bits value
 *
 * @param	handle
 *		System fabric initialized handle
 *
 * @returns	System fabric system counter value (32 bits)
 */
uint32_t al_sys_fabric_sys_counter_get_32(
	const struct al_sys_fabric_handle	*handle);

/**
 * System fabric semaphore lock
 *
 * @param	handle
 *		System fabric initialized handle
  * @param	id
  *		Semaphore ID
 *
 * @returns	0 upon success
 */
int al_sys_fabric_hw_semaphore_lock(
	const struct al_sys_fabric_handle	*handle,
	unsigned int			id);

/**
 * System fabric semaphore unlock
 *
 * @param	handle
 *		System fabric initialized handle
  * @param	id
  *		Semaphore ID
 */
void al_sys_fabric_hw_semaphore_unlock(
	const struct al_sys_fabric_handle	*handle,
	unsigned int			id);

/**
 * System fabric cluster powerdown/powerup timer setting
 *
 * @param	handle
 *		System fabric cluster initialized handle
 * @param	pd
 *		Powerdown time [system fabric clocks]
 * @param	pu
 *		Powerup time [system fabric clocks]
 */
void al_sys_fabric_cluster_pd_pu_timer_set(
	const struct al_sys_fabric_cluster_handle	*handle,
	unsigned int				pd,
	unsigned int				pu);

/**
 * System fabric cluster power control
 *
 * @param	handle
 *		System fabric cluster initialized handle
 * @param	ctrl
 *		Control value (see NB_GLOBAL_CPUS_POWER_CTRL_*)
 */
void al_sys_fabric_cluster_power_ctrl(
	const struct al_sys_fabric_cluster_handle	*handle,
	unsigned int				ctrl);

/**
 * System fabric core power control
 *
 * @param	handle
 *		System fabric cluster initialized handle
 * @param	core
 *		Core index (within cluster)
 * @param	ctrl
 *		Control value (see NB_CPUN_CONFIG_STATUS_POWER_CTRL_*)
 */
void al_sys_fabric_core_power_ctrl(
	const struct al_sys_fabric_cluster_handle	*handle,
	unsigned int				core,
	unsigned int				ctrl);

/**
 * System fabric cluster SW reset
 *
 * @param	handle
 *		System fabric cluster initialized handle
 */
void al_sys_fabric_cluster_sw_reset(
	const struct al_sys_fabric_cluster_handle	*handle);

/**
 * System fabric core SW reset
 *
 * @param	handle
 *		System fabric cluster initialized handle
 * @param	core
 *		Core index (within cluster)
 */
void al_sys_fabric_core_sw_reset(
	const struct al_sys_fabric_cluster_handle	*handle,
	unsigned int				core);

/**
 * System fabric core power-on reset (POR)
 *
 * @param	handle
 *		System fabric cluster initialized handle
 * @param	core
 *		Core index (within cluster)
 */
void al_sys_fabric_core_power_on_reset(
	const struct al_sys_fabric_cluster_handle	*handle,
	unsigned int				core);

/**
 * System fabric core reset de-assertion
 *
 * @param	handle
 *		System fabric cluster initialized handle
 * @param	core
 *		Core index (within cluster)
 */
void al_sys_fabric_core_reset_deassert(
	const struct al_sys_fabric_cluster_handle	*handle,
	unsigned int				core);

/**
 * System fabric core setup for aarch64
 *
 * @param	handle
 *		System fabric cluster initialized handle
 * @param	core
 *		Core index (within cluster)
 * @param	entry_high
 *		aarch64 entry point high 32 bits
 * @param	entry_low
 *		aarch64 entry point low 32 bits
 */
void al_sys_fabric_core_aarch64_setup(
	const struct al_sys_fabric_cluster_handle	*handle,
	unsigned int				core,
	uint32_t				entry_high,
	uint32_t				entry_low);

/**
 * System fabric core setup for aarch32
 *
 * @param	handle
 *		System fabric cluster initialized handle
 * @param	core
 *		Core index (within cluster)
 * @param	entry_high
 *		aarch32 entry point high 32 bits
 * @param	entry_low
 *		aarch32 entry point low 32 bits
 */
void al_sys_fabric_core_aarch32_setup(
	const struct al_sys_fabric_cluster_handle	*handle,
	unsigned int				core,
	uint32_t				entry_high,
	uint32_t				entry_low);

/**
 * System fabric core aarch32 setup get
 *
 * @param	handle
 *		System fabric cluster initialized handle
 * @param	core
 *		Core index (within cluster)
 * @param	entry_high
 *		aarch32 entry point high 32 bits
 * @param	entry_low
 *		aarch32 entry point low 32 bits
 */
void al_sys_fabric_core_aarch32_setup_get(
	const struct al_sys_fabric_cluster_handle	*handle,
	unsigned int				core,
	uint32_t				*entry_high,
	uint32_t				*entry_low);

/**
 * Get system fabric interrupt controller registers
 *
 * @param	handle
 *		System fabric initialized handle
 * @param	id
 *		Interrupt controller ID
 *
 * @returns	Interrupt controller registers
 *		(NULL if specific interrupt controller isn't supported)
 */
struct al_iofic_regs __iomem *al_sys_fabric_iofic_regs_get(
	const struct al_sys_fabric_handle	*handle,
	enum al_sys_fabric_iofic_id	id);

/** I/O cache usage configuration */
struct al_sys_fabric_iocache_usage_cfg {
	/**
	 * Determines if address decoder will override the SRAM access attribute
	 * If not set, allow propagation of attribute from incoming transaction
	 * For SRAM usage this flag should be set
	 * For cache usage this flag should be cleared
	 */
	al_bool		addr_map_sram_ovrd;
	/** Allow CPU to generate PAD transactions to I/O Cache */
	al_bool		pad_en;
	/** Allow CPU to generate Allocate transactions to I/O Cache */
	al_bool		alloc_en;
	/** Mask on RDMEMATTR of CPU to determine if access I/O cache (0-255) */
	unsigned int	rd_mask;
	/**
	 * Value on RDMEMATTR of CPU to determine if access I/O cache (0-255).
	 * To disable CPU access to I/O cache, set the mask to 0 and the value to non-zero.
	 * To send all transactions to I/O cache, set the mask to 0 and the value to 0.
	 */
	unsigned int	rd_value;
	/** Mask on WRMEMATTR of CPU to determine if access I/O cache (0-255) */
	unsigned int	wr_mask;
	/**
	 * Value on WRMEMATTR of CPU to determine if access I/O cache (0-255).
	 * To disable CPU access to I/O cache, set the mask to 0 and the value to non-zero.
	 * To send all transactions to I/O cache, set the mask to 0 and the value to 0.
	 */
	unsigned int	wr_value;
};

/**
 * Configure usage of I/O Cache
 *
 * @param	handle
 *		System fabric initialized handle
 * @param	cfg
 *		I/O cache usage configuration
 */
void al_sys_fabric_iocache_usage_cfg_set(
	const struct al_sys_fabric_handle		*handle,
	const struct al_sys_fabric_iocache_usage_cfg	*cfg);

/**
 * System fabric write splitter mode set
 *
 * Supported only for Alpine V2. Allows setting write splitter
 * in Alpine V1 mode.
 *
 * @param	handle
 *		System fabric initialized handle
 * @param	mode
 *		Write splitter mode
 */
void al_sys_fabric_wr_split_mode_set(
		const struct al_sys_fabric_handle		*handle,
		enum al_sys_fabric_wr_split_mode	mode);

/**
 * System fabric write splitter mode get
 *
 * Supported only for Alpine V2
 *
 * @param	handle
 *		System fabric initialized handle
 * @param	mode
 *		Write splitter mode
 */
void al_sys_fabric_wr_split_mode_get(
		const struct al_sys_fabric_handle	*handle,
		enum al_sys_fabric_wr_split_mode	*mode);


/**
 * System fabric error sfic masks get
 *
 * Supported only for Alpine v3
 *
 * @param	handle
 *		System fabric initialized handle
 * @param	a
 *		Group A mask
 * @param	b
 *		Group B mask
 * @param	c
 *		Group C mask
 * @param	d
 *		Group D mask
 */
void al_sys_fabric_error_ints_mask_get(
	const struct al_sys_fabric_handle		*handle,
	uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d);

/**
 * System fabric error sfic unmask
 *
 * Supported only for Alpine v3
 *
 * @param	handle
 *		System fabric initialized handle
 */
void al_sys_fabric_error_unmask(const struct al_sys_fabric_handle *handle);

/**
 * System fabric nmi sfic masks get
 *
 * Supported only for Alpine v3
 *
 * @param	handle
 *		System fabric initialized handle
 * @param	a
 *		Group A mask
 * @param	b
 *		Group B mask
 * @param	c
 *		Group C mask
 * @param	d
 *		Group D mask
 */void al_sys_fabric_nmi_ints_mask_get(
	const struct al_sys_fabric_handle		*handle,
	uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d);

/**
 * System fabric nmi sfic unmask
 *
 * Supported only for Alpine v3
 *
 * @param	handle
 *		System fabric initialized handle
 */
void al_sys_fabric_nmi_unmask(const struct al_sys_fabric_handle *handle);

/**
 * Get NMI (none maskable interrutps) interrupt controller register base address
 * Not Suuported on Alpine V1 & V2
 *
 * @param	handle
 *		System fabric initialized handle
 * @return
 *		base address
 */
void __iomem *al_sys_fabric_nmi_sfic_regs_base_get(
	const struct al_sys_fabric_handle *handle);

/**
 * Get error interrupt controller register base address
 * Not Suuported on Alpine V1 & V2
 *
 * @param	handle
 *		System fabric initialized handle
 * @return
 *		base address
 */
void __iomem *al_sys_fabric_err_sfic_regs_base_get(
	const struct al_sys_fabric_handle *handle);

/**
 * System fabric quality of service configuration setting
 *
 * @param	handle
 *		System fabric initialized handle
 * @param	cfg
 *		The required configuration
 */
void al_sys_fabric_qos_cfg_set(
	const struct al_sys_fabric_handle	*handle,
	const struct al_sys_fabric_qos_cfg	*cfg);

/**
 * System fabric quality of service configuration getting
 *
 * @param	handle
 *		System fabric initialized handle
 * @param	cfg
 *		The retrieved configuration
 */
void al_sys_fabric_qos_cfg_get(
	const struct al_sys_fabric_handle	*handle,
	struct al_sys_fabric_qos_cfg		*cfg);

/**
 * System fabric IO fabric to IO fabric path disable/enable
 *
 * @param	handle
 *		System fabric initialized handle
 * @param	en
 *		Enable/Disable this path
 */
void al_sys_fabric_iof_2_iof_path_en(
	const struct al_sys_fabric_handle	*handle,
	al_bool					en);

#endif

