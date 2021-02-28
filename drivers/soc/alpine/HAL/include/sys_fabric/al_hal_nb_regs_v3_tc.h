/*******************************************************************************
Copyright (C) 2016 Annapurna Labs Ltd.

This file may be licensed under the terms of the Annapurna Labs Commercial
License Agreement.

Alternatively, this file can be distributed under the terms of the GNU General
Public License V2 as published by the Free Software Foundation and can be
found at http://www.gnu.org/licenses/gpl-2.0.html

Alternatively, redistribution and use in source and binary forms, with or
without modification, are permitted provided that the following conditions are
met:

    *     Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

    *     Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in
the documentation and/or other materials provided with the
distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

/**
 *  @{
 * @file   al_hal_nb_regs.h
 *
 * @brief NB registers
 *
 */

#ifndef __AL_HAL_NB_REGS_H__
#define __AL_HAL_NB_REGS_H__

#include "al_hal_plat_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Unit Registers
 */

struct al_nb_global {
	/*
	 * [0x0] Allow non-secure access to secure registers.
	 * Each bit disables security for one register
	 */
	uint32_t sec_ovrd_low;
	/*
	 * [0x4] Allow non-secure access to secure registers.
	 * Each bit disables security for one register
	 */
	uint32_t sec_ovrd_high;
	/* [0x8] */
	uint32_t cpus_secure;
	uint32_t rsrvd_0;
	/* [0x10] */
	uint32_t nb_int_cause;
	/* [0x14] */
	uint32_t sev_int_cause;
	/* [0x18] */
	uint32_t pmus_int_cause;
	/*
	 * [0x1c] Send Event Mask Bit
	 * If set to 1, Send Event is generated to CPU Cluster if corresponding cause bit is set in
	 * Sfabric Interrupt Cause
	 */
	uint32_t sev_mask;
	uint32_t rsrvd_1[7];
	/* [0x3c] */
	uint32_t fabric_software_reset;
	uint32_t rsrvd_2[8];
	/* [0x60] */
	uint32_t acf_base_high;
	/* [0x64] */
	uint32_t acf_base_low;
	/* [0x68] */
	uint32_t acf_control_override;
	/* [0x6c] Read-only that reflects CPU Cluster Local GIC base high address  */
	uint32_t lgic_base_high;
	/* [0x70] Read-only that reflects CPU Cluster Local GIC base low address   */
	uint32_t lgic_base_low;
	uint32_t rsrvd_3[4];
	/* [0x84] */
	uint32_t sb_pos_error_log_1;
	/*
	 * [0x88] Error Log 0
	 * [31:0] address_low
	 */
	uint32_t sb_pos_error_log_0;
	/* [0x8c] */
	uint32_t c2swb_config;
	/* [0x90] */
	uint32_t msix_error_log;
	/* [0x94] */
	uint32_t error_cause;
	/*
	 * [0x98] Error Cause Mask Bit
	 * If set to 1,interrupt is delivered to nb_cause if corresponding cause bit is set
	 */
	uint32_t error_mask;
	uint32_t rsrvd_4;
	/* [0xa0] */
	uint32_t qos_peak_control;
	/* [0xa4] */
	uint32_t qos_set_control;
	/* [0xa8] */
	uint32_t ddr_qos;
	uint32_t rsrvd_5[9];
	/* [0xd0] */
	uint32_t acf_misc;
	/* [0xd4] */
	uint32_t config_bus_control;
	uint32_t rsrvd_6[2];
	/* [0xe0] */
	uint32_t pos_id_match;
	uint32_t rsrvd_7[3];
	/* [0xf0] */
	uint32_t sb_sel_override_awuser;
	/* [0xf4] */
	uint32_t sb_override_awuser;
	/* [0xf8] */
	uint32_t sb_sel_override_aruser;
	/* [0xfc] */
	uint32_t sb_override_aruser;
	uint32_t rsrvd_8[4];
	/* [0x110] */
	uint32_t wr_mixer;
	/* [0x114] */
	uint32_t rd_mixer;
	uint32_t rsrvd[58];
};

struct al_nb_coresight {
	/* [0x0] */
	uint32_t pd;
	/* [0x4] */
	uint32_t ctm;
	/* [0x8] */
	uint32_t dbgromaddr;
	uint32_t rsrvd_0;
	/*
	 * [0x10] [31:28] Peripheral ID2 [7:4]
	 * [27:24] Peripheral ID1 [3:0]
	 * [23:16] Peripheral ID0 [7:0]
	 * [15:12] Drive LOW
	 * [11:8] Peripheral ID4 [3:0]
	 * [7:5] Peripheral ID2 [2:0]
	 * [4:1] Peripheral ID1 [7:4]
	 * [0] Drive HIGH
	 */
	uint32_t targetid;
	/* [0x14] Internal  */
	uint32_t targetid0;
	/* [0x18] */
	uint32_t cti_0;
	/* [0x1c] */
	uint32_t cti_1;
	/* [0x20] */
	uint32_t stm;
	/* [0x24] */
	uint32_t rom;
	uint32_t rsrvd[54];
};

struct al_nb_sb2nb_port {
	/* [0x0] */
	uint32_t wr_split_control;
	uint32_t rsrvd_0;
	/* [0x8] */
	uint32_t rd_rob_control;
	uint32_t rsrvd_1[5];
	/* [0x20] */
	uint32_t force_same_id_cfg;
	/*
	 * [0x24] Selects whether to force same ID on each master.
	 * Each bit when set indicate that the writes ID from the corresponding master (which have
	 * the same ID in SB_NIC) should be forced to same ID.
	 */
	uint32_t mstr_force_same_id_sel;
	uint32_t rsrvd[6];
};

struct al_nb_system_counter {
	/* [0x0] */
	uint32_t cnt_control;
	/*
	 * [0x4] Counter base frequency
	 * Programs this register to match the counter frequency in ticks per second.
	 * Software information only register
	 */
	uint32_t cnt_base_freq;
	/* [0x8] Counter count low */
	uint32_t cnt_low;
	/* [0xc] Counter count high */
	uint32_t cnt_high;
	/* [0x10] Counter init low */
	uint32_t cnt_init_low;
	/* [0x14] Counter init high */
	uint32_t cnt_init_high;
	uint32_t rsrvd[58];
};

struct al_nb_rams_control_misc {
	uint32_t rsrvd_0[2];
	/* [0x8] */
	uint32_t nb_rf_misc;
	uint32_t rsrvd[61];
};

struct al_nb_semaphores {
	/* [0x0] This configuration is only sampled during reset of the processor */
	uint32_t lockn;
};

struct al_nb_debug {
	uint32_t rsrvd_0[8];
	/* [0x20] For ECO */
	uint32_t rsv0_config;
	/* [0x24] For ECO */
	uint32_t rsv1_config;
	uint32_t rsrvd_1[2];
	/* [0x30] For ECO */
	uint32_t rsv0_status;
	/* [0x34] For ECO */
	uint32_t rsv1_status;
	uint32_t rsrvd_2[10];
	/* [0x60] */
	uint32_t pmc;
	uint32_t rsrvd_3[7];
	/* [0x80] */
	uint32_t cpus_int_out;
	uint32_t rsrvd_4[31];
	/* [0x100] */
	uint32_t track_dump_ctrl;
	/* [0x104] */
	uint32_t track_dump_rdata_0;
	/* [0x108] High data */
	uint32_t track_dump_rdata_1;
	uint32_t rsrvd_5[5];
	/*
	 * [0x120] [21:0] Selective events from CCI
	 * [31:22] PoS
	 */
	uint32_t track_events;
	uint32_t rsrvd_6[3];
	/* [0x130] */
	uint32_t pos_track_dump_ctrl;
	/* [0x134] */
	uint32_t pos_track_dump_rdata_0;
	/* [0x138] High data */
	uint32_t pos_track_dump_rdata_1;
	uint32_t rsrvd_7;
	/* [0x140] */
	uint32_t c2swb_track_dump_ctrl;
	/* [0x144] */
	uint32_t c2swb_track_dump_rdata_0;
	/* [0x148] High data */
	uint32_t c2swb_track_dump_rdata_1;
	uint32_t rsrvd[45];
};

struct al_nb_cpun_config_status {
	uint32_t rsrvd_0[2];
	/* [0x8] NB Local Cause Interrupt Mask Register */
	uint32_t local_cause_mask;
	uint32_t rsrvd_1;
	/*
	 * [0x10] Mask per PMU overflow event
	 * Generates summary interrupt bit CPUn_PMUs_OVFL_IRQ.
	 */
	uint32_t pmus_cause_mask;
	/*
	 * [0x14] Error Cause Mask
	 *
	 * Summary set the nSEI (System Error Interrupt) line to the CPU
	 *
	 * Individual processor System Error Interrupt request. Active-LOW, SEI request:
	 * 0 Activate SEI request.
	 * 1 Do not activate SEI request.
	 * The processor treats nSEI as edge-sensitive. The nSEI signal must be sent as a pulse to
	 * the
	 * processor
	 */
	uint32_t sei_cause_mask;
	uint32_t rsrvd[58];
};

struct al_nb_nb_version {
	/* [0x0] Northbridge Revision */
	uint32_t version;
	uint32_t rsrvd;
};

struct al_nb_push_packet {
	/* [0x0] */
	uint32_t pp_config;
	uint32_t rsrvd_0[3];
	/* [0x10] */
	uint32_t pp_ext_attr;
	uint32_t rsrvd_1[3];
	/* [0x20] PP window base low address bits */
	uint32_t pp_base_low;
	/* [0x24] PP window base high address bits */
	uint32_t pp_base_high;
	uint32_t rsrvd_2[2];
	/* [0x30] */
	uint32_t pp_sel_attr;
	/* [0x34] */
	uint32_t pp_err_log;
	uint32_t rsrvd[50];
};

struct al_nb_regs {
	struct al_nb_global global;                             /* [0x0] */
	uint32_t rsrvd_0[128];
	struct al_nb_coresight coresight;                       /* [0x400] */
	uint32_t rsrvd_1[192];
	struct al_nb_sb2nb_port sb2nb_port[4];                  /* [0x800] */
	uint32_t rsrvd_2[448];
	struct al_nb_system_counter system_counter;             /* [0x1000] */
	struct al_nb_rams_control_misc rams_control_misc;       /* [0x1100] */
	uint32_t rsrvd_3[128];
	struct al_nb_semaphores semaphores[64];                 /* [0x1400] */
	uint32_t rsrvd_4[320];
	struct al_nb_debug debug;                               /* [0x1a00] */
	uint32_t rsrvd_5[256];
	struct al_nb_cpun_config_status cpun_config_status[4];  /* [0x2000] */
	uint32_t rsrvd_6[2048];
	struct al_nb_nb_version nb_version;                     /* [0x4400] */
	uint32_t rsrvd_7[1790];
	struct al_nb_push_packet push_packet;                   /* [0x6000] */
};


/*
 * Registers Fields
 */

/**** CPUs_Secure register ****/
/*
 * DBGEN
 */
#define NB_GLOBAL_CPUS_SECURE_DBGEN      (1 << 0)
/*
 * NIDEN
 */
#define NB_GLOBAL_CPUS_SECURE_NIDEN      (1 << 1)
/*
 * SPIDEN
 */
#define NB_GLOBAL_CPUS_SECURE_SPIDEN     (1 << 2)
/*
 * SPNIDEN
 */
#define NB_GLOBAL_CPUS_SECURE_SPNIDEN    (1 << 3)
/*
 * Disable write access to some secure GIC registers.
 * .
 */
#define NB_GLOBAL_CPUS_SECURE_CFGSDISABLE (1 << 4)
/*
 * Disable write access to some secure IOGIC registers.
 * .
 */
#define NB_GLOBAL_CPUS_SECURE_IOGIC_CFGSDISABLE (1 << 5)

/**** NB_Int_Cause register ****/
/* Cross trigger interrupt  */
#define NB_GLOBAL_NB_INT_CAUSE_NCTIIRQ_MASK 0x0000000F
#define NB_GLOBAL_NB_INT_CAUSE_NCTIIRQ_SHIFT 0
/* Communications channel receive. Receive portion of Data Transfer Register full flag */
#define NB_GLOBAL_NB_INT_CAUSE_COMMRX_MASK 0x000000F0
#define NB_GLOBAL_NB_INT_CAUSE_COMMRX_SHIFT 4
/* Communication channel transmit. Transmit portion of Data Transfer Register empty flag. */
#define NB_GLOBAL_NB_INT_CAUSE_COMMTX_MASK 0x00000F00
#define NB_GLOBAL_NB_INT_CAUSE_COMMTX_SHIFT 8
/* Emulation write FIFO log has valid entry */
#define NB_GLOBAL_NB_INT_CAUSE_PCIE_LOG_FIFO_VALID_0 (1 << 12)
/* Emulation write FIFO log has valid entry */
#define NB_GLOBAL_NB_INT_CAUSE_PCIE_LOG_FIFO_VALID_1 (1 << 13)
/* Reserved */
#define NB_GLOBAL_NB_INT_CAUSE_RESERVED_15_14_MASK 0x0000C000
#define NB_GLOBAL_NB_INT_CAUSE_RESERVED_15_14_SHIFT 14
/*
 * Error indicator for AXI write transactions with a BRESP error condition. Writing 0 to bit[29] of
 * the L2ECTLR clears the error indicator connected to CA15 nAXIERRIRQ.
 */
#define NB_GLOBAL_NB_INT_CAUSE_CPU_AXIERRIRQ (1 << 16)
/* Error indicator for: L2 RAM double-bit ECC error, illegal writes to the GIC memory-map region. */
#define NB_GLOBAL_NB_INT_CAUSE_CPU_INTERRIRQ (1 << 17)
/* Coherent fabric error summary interrupt */
#define NB_GLOBAL_NB_INT_CAUSE_ACF_ERRORIRQ (1 << 18)
/* DDR Controller ECC Correctable error summary interrupt */
#define NB_GLOBAL_NB_INT_CAUSE_MCTL_ECC_CORR_ERR (1 << 19)
/* DDR Controller ECC Uncorrectable error summary interrupt */
#define NB_GLOBAL_NB_INT_CAUSE_MCTL_ECC_UNCORR_ERR (1 << 20)
/* DRAM parity error interrupt */
#define NB_GLOBAL_NB_INT_CAUSE_MCTL_PARITY_ERR (1 << 21)
/* Reserved, not functional */
#define NB_GLOBAL_NB_INT_CAUSE_MCTL_WDATARAM_PAR (1 << 22)
/* Error cause summary interrupt */
#define NB_GLOBAL_NB_INT_CAUSE_ERR_CAUSE_SUM (1 << 23)
/* SB PoS error */
#define NB_GLOBAL_NB_INT_CAUSE_SB_POS_ERR (1 << 24)
/* Reserved */
#define NB_GLOBAL_NB_INT_CAUSE_RESERVED_25 (1 << 25)
/* Emulation write FIFO log is wrapped */
#define NB_GLOBAL_NB_INT_CAUSE_WR_LOG_FIFO_WRAP (1 << 26)
/* Emulation write FIFO log is full (new pushes might corrupt data) */
#define NB_GLOBAL_NB_INT_CAUSE_WR_LOG_FIFO_FULL (1 << 27)
/* Emulation write FIFO log is wrapped */
#define NB_GLOBAL_NB_INT_CAUSE_WR_LOG_FIFO_WRAP_1 (1 << 28)
/* Emulation write FIFO log is full (new pushes might corrupt data) */
#define NB_GLOBAL_NB_INT_CAUSE_WR_LOG_FIFO_FULL_1 (1 << 29)
/* PCIe read latched */
#define NB_GLOBAL_NB_INT_CAUSE_PCIE_RD_LOG_SET_0 (1 << 30)
/* PCIe read latched */
#define NB_GLOBAL_NB_INT_CAUSE_PCIE_RD_LOG_SET_1 (1 << 31)

/**** SEV_Int_Cause register ****/
/* SMMU 0/1 global non-secure fault interrupt */
#define NB_GLOBAL_SEV_INT_CAUSE_SMMU_GBL_FLT_IRPT_NS_MASK 0x00000003
#define NB_GLOBAL_SEV_INT_CAUSE_SMMU_GBL_FLT_IRPT_NS_SHIFT 0
/* SMMU 0/1 non-secure context interrupt */
#define NB_GLOBAL_SEV_INT_CAUSE_SMMU_CXT_IRPT_NS_MASK 0x0000000C
#define NB_GLOBAL_SEV_INT_CAUSE_SMMU_CXT_IRPT_NS_SHIFT 2
/* SMMU0/1 Non-secure configuration access fault interrupt */
#define NB_GLOBAL_SEV_INT_CAUSE_SMMU_CFG_FLT_IRPT_S_MASK 0x00000030
#define NB_GLOBAL_SEV_INT_CAUSE_SMMU_CFG_FLT_IRPT_S_SHIFT 4
/* Reserved. Read undefined; must write as zeros. */
#define NB_GLOBAL_SEV_INT_CAUSE_RESERVED_11_6_MASK 0x00000FC0
#define NB_GLOBAL_SEV_INT_CAUSE_RESERVED_11_6_SHIFT 6
/* PCIe emulation: inbound writes FIFO has valid entry */
#define NB_GLOBAL_SEV_INT_CAUSE_PCIE_LOG_FIFO_VALID_0 (1 << 12)
/* PCIe emulation: inbound writes FIFO has being wrapped (tail pointer) */
#define NB_GLOBAL_SEV_INT_CAUSE_PCIE_LOG_FIFO_WRAP_0 (1 << 13)
/* PCIe emulation: inbound writes FIFO is full */
#define NB_GLOBAL_SEV_INT_CAUSE_PCIE_LOG_FIFO_FULL_0 (1 << 14)
/* PCIe emulation: inbound writes FIFO has valid entry */
#define NB_GLOBAL_SEV_INT_CAUSE_PCIE_LOG_FIFO_VALID_1 (1 << 15)
/* PCIe emulation: inbound writes FIFO has being wrapped (tail pointer) */
#define NB_GLOBAL_SEV_INT_CAUSE_PCIE_LOG_FIFO_WRAP_1 (1 << 16)
/* PCIe emulation: inbound writes FIFO is full */
#define NB_GLOBAL_SEV_INT_CAUSE_PCIE_LOG_FIFO_FULL_1 (1 << 17)
/* PCIe emulation: inbound PCIe read is latched */
#define NB_GLOBAL_SEV_INT_CAUSE_PCIE_RD_LOG_SET_0 (1 << 18)
/* PCIe emulation: inbound PCIe read is latched */
#define NB_GLOBAL_SEV_INT_CAUSE_PCIE_RD_LOG_SET_1 (1 << 19)
/* Reserved. Read undefined; must write as zeros. */
#define NB_GLOBAL_SEV_INT_CAUSE_RESERVED_31_20_MASK 0xFFF00000
#define NB_GLOBAL_SEV_INT_CAUSE_RESERVED_31_20_SHIFT 20

/**** PMUs_Int_Cause register ****/
/* CPUs PMU Overflow interrupt */
#define NB_GLOBAL_PMUS_INT_CAUSE_CPUS_OVFL_MASK 0x0000000F
#define NB_GLOBAL_PMUS_INT_CAUSE_CPUS_OVFL_SHIFT 0
/* Northbridge PMU overflow */
#define NB_GLOBAL_PMUS_INT_CAUSE_NB_OVFL (1 << 4)
/* Memory Controller PMU overflow */
#define NB_GLOBAL_PMUS_INT_CAUSE_MCTL_OVFL (1 << 5)
/* Coherency Interconnect PMU overflow */
#define NB_GLOBAL_PMUS_INT_CAUSE_CCI_OVFL_MASK 0x000007C0
#define NB_GLOBAL_PMUS_INT_CAUSE_CCI_OVFL_SHIFT 6
/* Coherency Interconnect PMU overflow */
#define NB_GLOBAL_PMUS_INT_CAUSE_SMMU_OVFL_MASK 0x00001800
#define NB_GLOBAL_PMUS_INT_CAUSE_SMMU_OVFL_SHIFT 11
/* Reserved. Read undefined; must write as zeros. */
#define NB_GLOBAL_PMUS_INT_CAUSE_RESERVED_23_13_MASK 0x00FFE000
#define NB_GLOBAL_PMUS_INT_CAUSE_RESERVED_23_13_SHIFT 13
/* Southbridge PMUs overflow */
#define NB_GLOBAL_PMUS_INT_CAUSE_SB_PMUS_OVFL_MASK 0xFF000000
#define NB_GLOBAL_PMUS_INT_CAUSE_SB_PMUS_OVFL_SHIFT 24

/**** Fabric_Software_Reset register ****/
/* Write 1 apply the software reset. */
#define NB_GLOBAL_FABRIC_SOFTWARE_RESET_SWRESET_REQ (1 << 0)
/*
 * Defines the level of software reset:
 * 0x0 -  fabric: Fabric reset
 * 0x1 - gic: GIC reset
 * 0x2 - smmu: SMMU reset
 */
#define NB_GLOBAL_FABRIC_SOFTWARE_RESET_LEVEL_MASK 0x0000000E
#define NB_GLOBAL_FABRIC_SOFTWARE_RESET_LEVEL_SHIFT 1
/*
 * CPUn waiting for interrupt enable.
 * Defines which CPU WFI indication to wait before applying the software reset.
 */
#define NB_GLOBAL_FABRIC_SOFTWARE_RESET_WFI_MASK_MASK 0x000F0000
#define NB_GLOBAL_FABRIC_SOFTWARE_RESET_WFI_MASK_SHIFT 16

/**** ACF_Base_High register ****/
/* Coherency Fabric registers base [39:32]. */
#define NB_GLOBAL_ACF_BASE_HIGH_BASE_39_32_MASK 0x000000FF
#define NB_GLOBAL_ACF_BASE_HIGH_BASE_39_32_SHIFT 0
/* Coherency Fabric registers base [31:15] */
#define NB_GLOBAL_ACF_BASE_LOW_BASED_31_15_MASK 0xFFFF8000
#define NB_GLOBAL_ACF_BASE_LOW_BASED_31_15_SHIFT 15

/**** ACF_Control_Override register ****/
/*
 * Overrides the ARQOS and AWQOS input signals. One bit exists for each slave
 * interface.
 * Connected to QOSOVERRIDE[6:0]
 */
#define NB_GLOBAL_ACF_CONTROL_OVERRIDE_QOSOVRD_MASK 0x0000007F
#define NB_GLOBAL_ACF_CONTROL_OVERRIDE_QOSOVRD_SHIFT 0
/*
 * If LOW, then AC requests are never issued on the corresponding slave
 * interface. One bit exists for each slave interface.
 * Connected to ACCHANNELEN[6:0].
 */
#define NB_GLOBAL_ACF_CONTROL_OVERRIDE_ACE_CH_EN_MASK 0x00007F00
#define NB_GLOBAL_ACF_CONTROL_OVERRIDE_ACE_CH_EN_SHIFT 8
/*
 * If HIGH, then the corresponding slave interface will support the ordered write observation
 * property. Required for ports connected to PCIe.
 */
#define NB_GLOBAL_ACF_CONTROL_OVERRIDE_ORDERED_WRITE_OBSERVATION_MASK 0x007F0000
#define NB_GLOBAL_ACF_CONTROL_OVERRIDE_ORDERED_WRITE_OBSERVATION_SHIFT 16
/*
 * If HIGH, indicates that this master interface is connected to a component with both slave and
 * master interfaces, where there is a dependency between them.
 */
#define NB_GLOBAL_ACF_CONTROL_OVERRIDE_MI_DEPENDENT_ON_SI_MASK 0x07000000
#define NB_GLOBAL_ACF_CONTROL_OVERRIDE_MI_DEPENDENT_ON_SI_SHIFT 24
/*
 * If HIGH, all incoming requests are split into 64-byte transfers, rather than shareable requests
 * only.
 * This signal has no effect currently.
 */
#define NB_GLOBAL_ACF_CONTROL_OVERRIDE_BURST_SPLIT_MASK 0x70000000
#define NB_GLOBAL_ACF_CONTROL_OVERRIDE_BURST_SPLIT_SHIFT 28

/**** LGIC_Base_High register ****/
/*
 * GIC registers base [39:32].
 * This value is sampled into the CP15 Configuration Base Address Register (CBAR) at reset.
 */
#define NB_GLOBAL_LGIC_BASE_HIGH_BASE_43_32_MASK 0x00000FFF
#define NB_GLOBAL_LGIC_BASE_HIGH_BASE_43_32_SHIFT 0
/*
 * GIC registers base [31:15].
 * This value is sampled into the CP15 Configuration Base Address Register (CBAR) at reset
 */
#define NB_GLOBAL_LGIC_BASE_LOW_BASED_31_15_MASK 0xFFFF8000
#define NB_GLOBAL_LGIC_BASE_LOW_BASED_31_15_SHIFT 15

/**** SB_PoS_Error_Log_1 register ****/
/*
 * Error Log 1
 * [7:0] address_high
 * [16:8] request id
 * [18:17] bresp
 */
#define NB_GLOBAL_SB_POS_ERROR_LOG_1_ERR_LOG_MASK 0x7FFFFFFF
#define NB_GLOBAL_SB_POS_ERROR_LOG_1_ERR_LOG_SHIFT 0
/*
 * Valid logged error
 * Set on SB PoS error occurrence on capturing the error information. Subsequent errors will not be
 * captured until the valid bit is cleared.
 * The SB PoS reports on write errors.
 * When valid, an interrupt is set in the NB Cause Register.
 */
#define NB_GLOBAL_SB_POS_ERROR_LOG_1_VALID (1 << 31)
/* Write buffer bypass */
#define NB_GLOBAL_C2SWB_CONFIG_BYPASS    (1 << 4)
/* Write buffer enable */
#define NB_GLOBAL_C2SWB_CONFIG_ENABLE    (1 << 5)
/* Disable counter (wait 1000 NB cycles) before applying Write buffer enable/bypass configuration */
#define NB_GLOBAL_C2SWB_CONFIG_CFG_CNT_DIS (1 << 6)

/**** MSIx_Error_Log register ****/
/*
 * Error Log
 * Corresponds to MSIx address message [30:0].
 */
#define NB_GLOBAL_MSIX_ERROR_LOG_ERR_LOG_MASK 0x7FFFFFFF
#define NB_GLOBAL_MSIX_ERROR_LOG_ERR_LOG_SHIFT 0
/* Valid logged error */
#define NB_GLOBAL_MSIX_ERROR_LOG_VALID   (1 << 31)

/**** Error_Cause register ****/
/* PCIe emulation: inbound PCIe read latch timeout */
#define NB_GLOBAL_ERROR_CAUSE_PCIE_RD_LOG_0_TIMEOUT (1 << 0)
/* PCIe emulation: inbound PCIe read latch timeout */
#define NB_GLOBAL_ERROR_CAUSE_PCIE_RD_LOG_1_TIMEOUT (1 << 1)
/* Received msix is not mapped to local GIC or IO-GIC spin */
#define NB_GLOBAL_ERROR_CAUSE_MSIX_ERR_INT (1 << 2)
/* Coresight timestamp overflow */
#define NB_GLOBAL_ERROR_CAUSE_CORESIGHT_TS_OVERFLOW (1 << 3)
/* Write data parity error from SB channel 0. */
#define NB_GLOBAL_ERROR_CAUSE_SB0_WRDATA_PERR (1 << 4)
/* Write data parity error from SB channel 1. */
#define NB_GLOBAL_ERROR_CAUSE_SB1_WRDATA_PERR (1 << 5)
/* Read data parity error from SB slaves. */
#define NB_GLOBAL_ERROR_CAUSE_SB_SLV_RDATA_PERR (1 << 6)
/* Local GIC uncorrectable ECC error */
#define NB_GLOBAL_ERROR_CAUSE_LOCAL_GIC_ECC_FATAL (1 << 7)
/* SB PoS error */
#define NB_GLOBAL_ERROR_CAUSE_SB_POS_ERR (1 << 8)
/* Coherent fabric error summary interrupt */
#define NB_GLOBAL_ERROR_CAUSE_ACF_ERRORIRQ (1 << 9)
/*
 * Error indicator for AXI write transactions with a BRESP error condition. Writing 0 to bit[29] of
 * the L2ECTLR clears the error indicator connected to CA15 nAXIERRIRQ.
 */
#define NB_GLOBAL_ERROR_CAUSE_CPU_AXIERRIRQ (1 << 10)
/* Error indicator for: L2 RAM double-bit ECC error, illegal writes to the GIC memory-map region. */
#define NB_GLOBAL_ERROR_CAUSE_CPU_INTERRIRQ (1 << 12)
/* DDR cause summery interrupt */
#define NB_GLOBAL_ERROR_CAUSE_DDR_CAUSE_SUM (1 << 14)
/*
 * Emulation write FIFO log is full (new pushes might corrupt data).
 * Summery bit for all SB channel 0 FIFO's
 */
#define NB_GLOBAL_ERROR_CAUSE_WR_LOG_FIFO_FULL_SUM_0 (1 << 16)
/*
 * Emulation write FIFO log is full (new pushes might corrupt data).
 * Summery bit for all SB channel 1 FIFO's
 */
#define NB_GLOBAL_ERROR_CAUSE_WR_LOG_FIFO_FULL_SUM_1 (1 << 17)

/**** QoS_Peak_Control register ****/
/*
 * Peak Read Low Threshold
 * When the number of outstanding read transactions from SB masters is below this value, the CPU is
 * assigned high-priority QoS.
 */
#define NB_GLOBAL_QOS_PEAK_CONTROL_RD_L_THRESHOLD_MASK 0x0000007F
#define NB_GLOBAL_QOS_PEAK_CONTROL_RD_L_THRESHOLD_SHIFT 0
/*
 * Peak Read High Threshold
 * When the number of outstanding read transactions from SB masters exceeds this value, the CPU is
 * assigned high-priority QoS.
 */
#define NB_GLOBAL_QOS_PEAK_CONTROL_RD_H_THRESHOLD_MASK 0x00007F00
#define NB_GLOBAL_QOS_PEAK_CONTROL_RD_H_THRESHOLD_SHIFT 8
/*
 * Peak Write Low Threshold
 * When the number of outstanding write transactions from SB masters is below this value, the CPU is
 * assigned high-priority QoS
 */
#define NB_GLOBAL_QOS_PEAK_CONTROL_WR_L_THRESHOLD_MASK 0x007F0000
#define NB_GLOBAL_QOS_PEAK_CONTROL_WR_L_THRESHOLD_SHIFT 16
/*
 * Peak Write High Threshold
 * When the number of outstanding write transactions from SB masters exceeds this value, the CPU is
 * assigned high-priority QoS.
 */
#define NB_GLOBAL_QOS_PEAK_CONTROL_WR_H_THRESHOLD_MASK 0x7F000000
#define NB_GLOBAL_QOS_PEAK_CONTROL_WR_H_THRESHOLD_SHIFT 24

/**** QoS_Set_Control register ****/
/* CPU Low priority Read QoS */
#define NB_GLOBAL_QOS_SET_CONTROL_CPU_LP_ARQOS_MASK 0x0000000F
#define NB_GLOBAL_QOS_SET_CONTROL_CPU_LP_ARQOS_SHIFT 0
/* CPU High priority Read QoS */
#define NB_GLOBAL_QOS_SET_CONTROL_CPU_HP_ARQOS_MASK 0x000000F0
#define NB_GLOBAL_QOS_SET_CONTROL_CPU_HP_ARQOS_SHIFT 4
/* CPU Low priority Write QoS */
#define NB_GLOBAL_QOS_SET_CONTROL_CPU_LP_AWQOS_MASK 0x00000F00
#define NB_GLOBAL_QOS_SET_CONTROL_CPU_LP_AWQOS_SHIFT 8
/* CPU High priority Write QoS */
#define NB_GLOBAL_QOS_SET_CONTROL_CPU_HP_AWQOS_MASK 0x0000F000
#define NB_GLOBAL_QOS_SET_CONTROL_CPU_HP_AWQOS_SHIFT 12
/* SB Low priority Read QoS */
#define NB_GLOBAL_QOS_SET_CONTROL_SB_LP_ARQOS_MASK 0x000F0000
#define NB_GLOBAL_QOS_SET_CONTROL_SB_LP_ARQOS_SHIFT 16
/* SB Low-priority Write QoS */
#define NB_GLOBAL_QOS_SET_CONTROL_SB_LP_AWQOS_MASK 0x00F00000
#define NB_GLOBAL_QOS_SET_CONTROL_SB_LP_AWQOS_SHIFT 20

/**** DDR_QoS register ****/
/*
 * High Priority Read Threshold
 * Limits the number of outstanding high priority reads in the system through the memory controller.
 * This parameter is programmed in conjunction with number of outstanding high priority reads
 * supported by the DDR controller.
 */
#define NB_GLOBAL_DDR_QOS_HIGH_PRIO_THRESHOLD_MASK 0x0000007F
#define NB_GLOBAL_DDR_QOS_HIGH_PRIO_THRESHOLD_SHIFT 0
/*
 * DDR Low Priority QoS
 * Fabric priority below this value is mapped to DDR low priority queue.
 */
#define NB_GLOBAL_DDR_QOS_LP_QOS_MASK    0x00000F00
#define NB_GLOBAL_DDR_QOS_LP_QOS_SHIFT   8

/**** ACF_Misc register ****/
/*
 * Disable DDR Write Chop
 * Performance optimization feature to chop non-active data beats to the DDR.
 */
#define NB_GLOBAL_ACF_MISC_DDR_WR_CHOP_DIS (1 << 0)
/* Disable SB-2-SB path through NB fabric. */
#define NB_GLOBAL_ACF_MISC_SB2SB_PATH_DIS (1 << 1)
/* Disable ETR tracing to non-DDR. */
#define NB_GLOBAL_ACF_MISC_ETR2SB_PATH_DIS (1 << 2)
/*
 * Disable CPU generation of MSIx
 * By default, the CPU can set any MSIx message results by setting any SPIn bit in the local and
 * IO-GIC.
 */
#define NB_GLOBAL_ACF_MISC_CPU2MSIX_DIS  (1 << 3)
/*
 * Disable MSIx termination.
 * By Default, an MSIx transaction is terminated and not written to memory
 */
#define NB_GLOBAL_ACF_MISC_MSIX_TERMINATE_DIS (1 << 4)
/*
 * Disable snoop override for MSIx
 * By default, an MSIx transaction is downgraded to non-coherent.
 */
#define NB_GLOBAL_ACF_MISC_MSIX_SNOOPOVRD_DIS (1 << 5)
/* POS bypass */
#define NB_GLOBAL_ACF_MISC_POS_BYPASS    (1 << 6)
/*
 * PoS ReadStronglyOrdered enable
 * SO read forces flushing of all prior writes
 */
#define NB_GLOBAL_ACF_MISC_POS_RSO_EN    (1 << 7)
/* WRAP to INC transfer enable */
#define NB_GLOBAL_ACF_MISC_POS_WRAP2INC  (1 << 8)
/*
 * PoS DSB flush Disable
 * On DSB from CPU, PoS blocks the progress of post-barrier reads and writes until all pre-barrier
 * writes have been completed.
 */
#define NB_GLOBAL_ACF_MISC_POS_DSB_FLUSH_DIS (1 << 9)
/*
 * PoS DMB Flush Disable
 * On DMB from CPU, the PoS blocks the progress of post-barrier non-buffereable reads or writes when
 * there are outstanding non-bufferable writes that have not yet been completed.
 * Other access types are  hazard check against the pre-barrier requests.
 */
#define NB_GLOBAL_ACF_MISC_POS_DMB_FLUSH_DIS (1 << 10)
/* change DMB functionality to DSB (block and drain) */
#define NB_GLOBAL_ACF_MISC_POS_DMB_TO_DSB_EN (1 << 11)
/* Disable counter (wait 1000 NB cycles) before applying PoS enable/disable configuration */
#define NB_GLOBAL_ACF_MISC_POS_CONFIG_CNT_DIS (1 << 14)
/* Disable IO MMU page table walk to non-DDR. */
#define NB_GLOBAL_ACF_MISC_IOMMU_PTW2SB_PATH_DIS (1 << 15)
/* Override the address parity calucation for write transactions going to IO-fabric */
#define NB_GLOBAL_ACF_MISC_NB_NIC_AWADDR_PAR_OVRD (1 << 18)
/* Override the data parity calucation for write transactions going to IO-fabric */
#define NB_GLOBAL_ACF_MISC_NB_NIC_WDATA_PAR_OVRD (1 << 19)
/* Override the address parity calucation for read transactions going to IO-fabric */
#define NB_GLOBAL_ACF_MISC_NB_NIC_ARADDR_PAR_OVRD (1 << 20)
/* Halts CPU AXI interface (Ar/Aw channels), not allowing the CPU to send additional transactions */
#define NB_GLOBAL_ACF_MISC_CPU_AXI_HALT  (1 << 23)
/*
 * Enable DMB flush request to NB to SB PoS when barrier is terminted inside the processor cluster
 */
#define NB_GLOBAL_ACF_MISC_CPU_DSB_FLUSH_DIS (1 << 26)
/*
 * Enable DMB flush request to NB to SB PoS when barrier is terminted inside the processor cluster
 */
#define NB_GLOBAL_ACF_MISC_CPU_DMB_FLUSH_DIS (1 << 27)
/* Enable CPU WriteUnique to WriteNoSnoop trasform */
#define NB_GLOBAL_ACF_MISC_CPU_WU2WNS_EN (1 << 29)
/* Disable device after device check */
#define NB_GLOBAL_ACF_MISC_WR_POS_DEV_AFTER_DEV_DIS (1 << 30)
/* Disable wrap to inc on write */
#define NB_GLOBAL_ACF_MISC_WR_INC2WRAP_EN (1 << 31)

/**** Config_Bus_Control register ****/
/* Write slave error enable */
#define NB_GLOBAL_CONFIG_BUS_CONTROL_WR_SLV_ERR_EN (1 << 0)
/* Write decode error enable */
#define NB_GLOBAL_CONFIG_BUS_CONTROL_WR_DEC_ERR_EN (1 << 1)
/* Read slave error enable */
#define NB_GLOBAL_CONFIG_BUS_CONTROL_RD_SLV_ERR_EN (1 << 2)
/* Read decode error enable */
#define NB_GLOBAL_CONFIG_BUS_CONTROL_RD_DEC_ERR_EN (1 << 3)
/* Ignore Write ID */
#define NB_GLOBAL_CONFIG_BUS_CONTROL_IGNORE_WR_ID (1 << 4)
/* Weight number of reads per one write */
#define NB_GLOBAL_CONFIG_BUS_CONTROL_WRR_CNT_MASK 0x000000E0
#define NB_GLOBAL_CONFIG_BUS_CONTROL_WRR_CNT_SHIFT 5
/* Timeout limit before terminating configuration bus access with slave error */
#define NB_GLOBAL_CONFIG_BUS_CONTROL_TIMEOUT_LIMIT_MASK 0xFFFFFF00
#define NB_GLOBAL_CONFIG_BUS_CONTROL_TIMEOUT_LIMIT_SHIFT 8

/**** Pos_ID_Match register ****/
/* Enable Device (GRE and nGRE) after Device ID hazard */
#define NB_GLOBAL_POS_ID_MATCH_ENABLE    (1 << 0)
/*
 * ID Field Mask
 * If set, corresonpding ID bits are not used for ID match
 */
#define NB_GLOBAL_POS_ID_MATCH_MASK_MASK 0x0FFF0000
#define NB_GLOBAL_POS_ID_MATCH_MASK_SHIFT 16

/**** sb_sel_override_awuser register ****/
/*
 * Select whether to use transaction awuser or sb_override_awuser value for awuser field on outgoing
 * write transactions to SB.
 * Each bit if set to 1 selects the corresponding sb_override_awuser bit. Otherwise, selects the
 * corersponding transaction awuser bit.
 */
#define NB_GLOBAL_SB_SEL_OVERRIDE_AWUSER_SEL_MASK 0x03FFFFFF
#define NB_GLOBAL_SB_SEL_OVERRIDE_AWUSER_SEL_SHIFT 0

/**** sb_override_awuser register ****/
/*
 * Awuser to use on overriden transactions
 * Only applicable if sel_override_awuser.sel is set to 1'b1 for the coressponding bit
 */
#define NB_GLOBAL_SB_OVERRIDE_AWUSER_AWUSER_MASK 0x03FFFFFF
#define NB_GLOBAL_SB_OVERRIDE_AWUSER_AWUSER_SHIFT 0

/**** sb_sel_override_aruser register ****/
/*
 * Select whether to use transaction aruser or sb_override_aruser value for aruser field on outgoing
 * read transactions to SB.
 * Each bit if set to 1 selects the corresponding sb_override_aruser bit. Otherwise, selects the
 * corersponding transaction aruser bit.
 */
#define NB_GLOBAL_SB_SEL_OVERRIDE_ARUSER_SEL_MASK 0x03FFFFFF
#define NB_GLOBAL_SB_SEL_OVERRIDE_ARUSER_SEL_SHIFT 0

/**** sb_override_aruser register ****/
/*
 * Aruser to use on overriden transactions
 * Only applicable if sb_sel_override_aruser.sel is set to 1'b1 for the coressponding bit
 */
#define NB_GLOBAL_SB_OVERRIDE_ARUSER_ARUSER_MASK 0x03FFFFFF
#define NB_GLOBAL_SB_OVERRIDE_ARUSER_ARUSER_SHIFT 0

/**** wr_mixer register ****/
/*
 * Write mixer request Qos.
 * Each 4 bits provide the channel quality of service when <use_agent_qos> is set.
 * Channel 0 - SB port 2
 * Channel 1 - SB port 3
 * Channel 2 - ETR
 */
#define NB_GLOBAL_WR_MIXER_REQ_QOS_MASK  0x00000FFF
#define NB_GLOBAL_WR_MIXER_REQ_QOS_SHIFT 0
/*
 * Write mixer promotion rate.
 * Controls after how many denials of service to increase the channel Qos by one.
 */
#define NB_GLOBAL_WR_MIXER_PROMO_RATE_MASK 0x0F000000
#define NB_GLOBAL_WR_MIXER_PROMO_RATE_SHIFT 24
/*
 * Write mixer use request Qos.
 * If set, the mixer arbiter uses the <req_qos> values for quality of service arbitration.
 * Else, it uses the AXI Qos value.
 */
#define NB_GLOBAL_WR_MIXER_USE_AGENT_QOS (1 << 31)

/**** rd_mixer register ****/
/*
 * Read mixer request Qos.
 * Each 4 bits provide the channel quality of service when <use_agent_qos> is set.
 * Channel 0 - SB port 2
 * Channel 1 - SB port 3
 * Channel 2 - ETR
 * Channel 3 - IO MMU page table walk
 */
#define NB_GLOBAL_RD_MIXER_REQ_QOS_MASK  0x0000FFFF
#define NB_GLOBAL_RD_MIXER_REQ_QOS_SHIFT 0
/*
 * Read mixer promotion rate.
 * Controls after how many denials of service to increase the channel Qos by one.
 */
#define NB_GLOBAL_RD_MIXER_PROMO_RATE_MASK 0x0F000000
#define NB_GLOBAL_RD_MIXER_PROMO_RATE_SHIFT 24
/*
 * Read mixer use request Qos.
 * If set, the mixer arbiter uses the <req_qos> values for quality of service arbitration.
 * Else, it uses the AXI Qos value.
 */
#define NB_GLOBAL_RD_MIXER_USE_AGENT_QOS (1 << 31)
/* APBIC clock enable */
#define NB_CORESIGHT_PD_APBICLKEN        (1 << 4)
/* DAP system clock enable */
#define NB_CORESIGHT_PD_DAP_SYS_CLKEN    (1 << 5)

/**** CTM register ****/
/*
 * TOP CTM and Coresight CTI operate according to the same clock.
 * Bypass modes can be enabled
 */
#define NB_CORESIGHT_CTM_CTIAPBSBYPASS   (1 << 0)
/* CPU CTM and TOP CTM operate at different clock, bypass modes cannot be enabled */
#define NB_CORESIGHT_CTM_CPU_CISBYPASS   (1 << 1)
/* CPU CTM and TOP CTM operate at different clock, bypass modes cannot be enabled */
#define NB_CORESIGHT_CTM_CPU_CIHSBYPASS_MASK 0x0000003C
#define NB_CORESIGHT_CTM_CPU_CIHSBYPASS_SHIFT 2
/* TOP CTM and Coresight CTI operate at same clock, bypass modes can be enabled */
#define NB_CORESIGHT_CTM_TOP_CISBYPASS   (1 << 6)
/*
 * TOP CTM and Coresight CTI operate according to the same clock.
 * Bypass modes can be enabled
 */
#define NB_CORESIGHT_CTM_TOP_CIHSBYPASS_MASK 0x00000780
#define NB_CORESIGHT_CTM_TOP_CIHSBYPASS_SHIFT 7

/**** DBGROMADDR register ****/
/*
 * Valid signal for DBGROMADDR.
 * Connected to DBGROMADDRV
 */
#define NB_CORESIGHT_DBGROMADDR_VALID    (1 << 0)
/* Specifies bits [39:12] of the ROM table physical address. */
#define NB_CORESIGHT_DBGROMADDR_ADDR_39_12_MASK 0x3FFFFFFC
#define NB_CORESIGHT_DBGROMADDR_ADDR_39_12_SHIFT 2

/**** CTI_0 register ****/
/* Trigger interface handshake bypass */
#define NB_CORESIGHT_CTI_0_TIHSBYPASS_MASK 0x000000FF
#define NB_CORESIGHT_CTI_0_TIHSBYPASS_SHIFT 0
/* Trigger out acknowledge sync bypass */
#define NB_CORESIGHT_CTI_0_TISBYPASSACK_MASK 0x00FF0000
#define NB_CORESIGHT_CTI_0_TISBYPASSACK_SHIFT 16
/* Trigger in sync bypass */
#define NB_CORESIGHT_CTI_0_TISBYPASSIN_MASK 0xFF000000
#define NB_CORESIGHT_CTI_0_TISBYPASSIN_SHIFT 24

/**** CTI_1 register ****/
/* Masks when dbgen is LOW */
#define NB_CORESIGHT_CTI_1_TODBGENSEL_MASK 0x000000FF
#define NB_CORESIGHT_CTI_1_TODBGENSEL_SHIFT 0
/* Masks when NIDEN is LOW */
#define NB_CORESIGHT_CTI_1_TINIDENSEL_MASK 0x0000FF00
#define NB_CORESIGHT_CTI_1_TINIDENSEL_SHIFT 8

/**** STM register ****/
/*
 * When the NSGUAREN signal is LOW, Non-secure guaranteed accesses behave like
 * invariant timing accesses, that is, the AXI does not stall.
 * When the NSGUAREN signal is HIGH, Non-secure guaranteed accesses are enabled,
 * that is, the AXI can stall and the trace output is guaranteed.
 */
#define NB_CORESIGHT_STM_NSGUAREN        (1 << 0)

/**** ROM register ****/
/* Override ROM entries */
#define NB_CORESIGHT_ROM_OVRD            (1 << 0)

/**** Wr_split_Control register ****/
/* Write splitters bypass. */
#define NB_SB2NB_PORT_WR_SPLIT_CONTROL_BYPASS (1 << 0)
/*
 * Write splitters store and forward.
 * If store and forward is disabled, splitter does not check non-active BE in the middle of a
 * transaction.
 */
#define NB_SB2NB_PORT_WR_SPLIT_CONTROL_ST_FW (1 << 1)
/*
 * Write splitters unmodify snoop type.
 * Disables modifying snoop type from Clean & Invalidate to Invalidate when conditions enable it.
 * Only split operation to 64B is applied.
 */
#define NB_SB2NB_PORT_WR_SPLIT_CONTROL_UNMODIFY_SNP (1 << 2)
/*
 * Write splitters unsplit non-coherent access.
 * Disables splitting of non-coherent access to cache-line chunks.
 */
#define NB_SB2NB_PORT_WR_SPLIT_CONTROL_UNSPLIT_NOSNP (1 << 3)
/* In default mode, AWADDR waits for WDATA. */
#define NB_SB2NB_PORT_WR_SPLIT_CONTROL_PACK_DIS (1 << 4)
/*
 * Write splitters 64bit remap enable
 * Enables remapping of 64bit transactions
 */
#define NB_SB2NB_PORT_WR_SPLIT_CONTROL_REMAP_64BIT_EN (1 << 5)
/* Write splitters track sub 64bit transactions enable */
#define NB_SB2NB_PORT_WR_SPLIT_CONTROL_TRACK_64BIT (1 << 6)
/* Write splitter rate limit. */
#define NB_SB2NB_PORT_WR_SPLIT_CONTROL_RATE_LIMIT_MASK 0x00001F00
#define NB_SB2NB_PORT_WR_SPLIT_CONTROL_RATE_LIMIT_SHIFT 8
/* Disable wr spliter alpine V1 bug fixes */
#define NB_SB2NB_PORT_WR_SPLIT_CONTROL_ALPINE_V1_MODE_DIS (1 << 13)
/* Disable wr spliter alpine V2 bug fixes */
#define NB_SB2NB_PORT_WR_SPLIT_CONTROL_ALPINE_V2_MODE_DIS (1 << 14)
/* Disable AxUser remap support */
#define NB_SB2NB_PORT_WR_SPLIT_CONTROL_REMAP_AXUSER_DIS (1 << 15)

/**** Rd_ROB_Control register ****/
/* Read ROB Bypass */
#define NB_SB2NB_PORT_RD_ROB_CONTROL_BYPASS (1 << 0)
/*
 * Read ROB in order.
 * Return data in the order of request acceptance.
 */
#define NB_SB2NB_PORT_RD_ROB_CONTROL_INORDER (1 << 1)
/*
 * Read ROB response rate
 * When enabled drops one cycle from back to back read responses
 */
#define NB_SB2NB_PORT_RD_ROB_CONTROL_RSP_RATE (1 << 2)
/* Read splitter rate limit */
#define NB_SB2NB_PORT_RD_ROB_CONTROL_RATE_LIMIT_MASK 0x00001F00
#define NB_SB2NB_PORT_RD_ROB_CONTROL_RATE_LIMIT_SHIFT 8

/**** force_same_id_cfg register ****/
/* Enables force same id mechanism for SB port 0 */
#define NB_SB2NB_PORT_FORCE_SAME_ID_CFG_EN (1 << 0)
/*
 * Enables MSIx stall when write transactions from same ID mechanism are in progress for SB port 0
 */
#define NB_SB2NB_PORT_FORCE_SAME_ID_CFG_MSIX_STALL_EN (1 << 1)
/* Mask for choosing which ID bits to match for indicating the originating master */
#define NB_SB2NB_PORT_FORCE_SAME_ID_CFG_SB_MSTR_ID_MASK 0x000000F8
#define NB_SB2NB_PORT_FORCE_SAME_ID_CFG_SB_MSTR_ID_SHIFT 3

/**** Cnt_Control register ****/
/*
 * System counter enable
 * Counter is enabled after reset.
 */
#define NB_SYSTEM_COUNTER_CNT_CONTROL_EN (1 << 0)
/*
 * System counter restart
 * Initial value is reloaded from Counter_Init_L and Counter_Init_H registers.
 * Transition from 0 to 1 reloads the register.
 */
#define NB_SYSTEM_COUNTER_CNT_CONTROL_RESTART (1 << 1)
/* Disable CTI trigger out that halt the counter progress */
#define NB_SYSTEM_COUNTER_CNT_CONTROL_CTI_TRIGOUT_HALT_DIS (1 << 2)
/*
 * System counter tick
 * Specifies the counter tick rate relative to the Northbridge clock, e.g., the counter is
 * incremented every 16 NB cycles if programmed to 0x0f.
 */
#define NB_SYSTEM_COUNTER_CNT_CONTROL_SCALE_MASK 0x0000FF00
#define NB_SYSTEM_COUNTER_CNT_CONTROL_SCALE_SHIFT 8

/**** NB_RF_Misc register ****/
/* SMMU TLB RAMs force power down */
#define NB_RAMS_CONTROL_MISC_NB_RF_MISC_SMMU_RAM_FORCE_PD (1 << 0)

/**** Lockn register ****/
/*
 * Semaphore Lock
 * CPU reads it:
 * If current value ==0,  return 0 to CPU but set bit to 1. (CPU knows it captured the semaphore.)
 * If current value ==1, return 1 to CPU. (CPU knows it is already used and waits.)
 * CPU writes 0 to it to release the semaphore.
 */
#define NB_SEMAPHORES_LOCKN_LOCK         (1 << 0)

/**** pmc register ****/
/* Enable system control on NB DRO */
#define NB_DEBUG_PMC_SYS_EN              (1 << 0)
/* NB PMC HVT35 counter value */
#define NB_DEBUG_PMC_HVT35_VAL_14_0_MASK 0x0000FFFE
#define NB_DEBUG_PMC_HVT35_VAL_14_0_SHIFT 1
/* NB PMC SVT31 counter value */
#define NB_DEBUG_PMC_SVT31_VAL_14_0_MASK 0x7FFF0000
#define NB_DEBUG_PMC_SVT31_VAL_14_0_SHIFT 16

/**** cpus_int_out register ****/
/* Defines which CPUs' FIQ will be triggered out through the cpus_int_out[1] pinout. */
#define NB_DEBUG_CPUS_INT_OUT_FIQ_EN_MASK 0x0000000F
#define NB_DEBUG_CPUS_INT_OUT_FIQ_EN_SHIFT 0
/* Defines which CPUs' IRQ will be triggered out through the cpus_int_out[0] pinout. */
#define NB_DEBUG_CPUS_INT_OUT_IRQ_EN_MASK 0x000000F0
#define NB_DEBUG_CPUS_INT_OUT_IRQ_EN_SHIFT 4
/* Defines which CPUs' SEI will be triggered out through the cpus_int_out[0] pinout. */
#define NB_DEBUG_CPUS_INT_OUT_IRQ_SEI_EN_MASK 0x00000F00
#define NB_DEBUG_CPUS_INT_OUT_IRQ_SEI_EN_SHIFT 8

/**** track_dump_ctrl register ****/
/*
 * [24:16]: Queue entry pointer
 * [2] Target queue:  1'b0: HazardTrack or 1'b1: AmiRMI queues
 * [1:0]: CCI target master: 2'b00: M0, 2'b01: M1, 2'b10: M2
 */
#define NB_DEBUG_TRACK_DUMP_CTRL_PTR_MASK 0x7FFFFFFF
#define NB_DEBUG_TRACK_DUMP_CTRL_PTR_SHIFT 0
/*
 * Track Dump Request
 * If set, queue entry info is latched on track_dump_rdata register.
 * Program the pointer and target queue.
 * This is a full handshake register.
 * Read <valid> bit from track_dump_rdata register. If set, clear the request field before
 * triggering a new request.
 */
#define NB_DEBUG_TRACK_DUMP_CTRL_REQ     (1 << 31)

/**** track_dump_rdata_0 register ****/
/* Valid */
#define NB_DEBUG_TRACK_DUMP_RDATA_0_VALID (1 << 0)
/* Low data */
#define NB_DEBUG_TRACK_DUMP_RDATA_0_DATA_MASK 0xFFFFFFFE
#define NB_DEBUG_TRACK_DUMP_RDATA_0_DATA_SHIFT 1

/**** pos_track_dump_ctrl register ****/
/* [24:16]: queue entry pointer */
#define NB_DEBUG_POS_TRACK_DUMP_CTRL_PTR_MASK 0x7FFFFFFF
#define NB_DEBUG_POS_TRACK_DUMP_CTRL_PTR_SHIFT 0
/*
 * Track Dump Request
 * If set, queue entry info is latched on track_dump_rdata register.
 * Program the pointer and target queue.
 * This is a  full handshake register
 * Read <valid> bit from track_dump_rdata register. If set, clear the request field before
 * triggering a new request.
 */
#define NB_DEBUG_POS_TRACK_DUMP_CTRL_REQ (1 << 31)

/**** pos_track_dump_rdata_0 register ****/
/* Valid */
#define NB_DEBUG_POS_TRACK_DUMP_RDATA_0_VALID (1 << 0)
/* Low data */
#define NB_DEBUG_POS_TRACK_DUMP_RDATA_0_DATA_MASK 0xFFFFFFFE
#define NB_DEBUG_POS_TRACK_DUMP_RDATA_0_DATA_SHIFT 1

/**** c2swb_track_dump_ctrl register ****/
/* [24:16]: Queue entry pointer */
#define NB_DEBUG_C2SWB_TRACK_DUMP_CTRL_PTR_MASK 0x7FFFFFFF
#define NB_DEBUG_C2SWB_TRACK_DUMP_CTRL_PTR_SHIFT 0
/*
 * Track Dump Request
 * If set, queue entry info is latched on track_dump_rdata register.
 * Program the pointer and target queue.
 * This is a full handshake register
 * Read <valid> bit from track_dump_rdata register. If set, clear the request field before
 * triggering a new request.
 */
#define NB_DEBUG_C2SWB_TRACK_DUMP_CTRL_REQ (1 << 31)

/**** c2swb_track_dump_rdata_0 register ****/
/* Valid */
#define NB_DEBUG_C2SWB_TRACK_DUMP_RDATA_0_VALID (1 << 0)
/* Low data */
#define NB_DEBUG_C2SWB_TRACK_DUMP_RDATA_0_DATA_MASK 0xFFFFFFFE
#define NB_DEBUG_C2SWB_TRACK_DUMP_RDATA_0_DATA_SHIFT 1

/**** version register ****/
/*  Revision number (Minor) */
#define NB_NB_VERSION_VERSION_RELEASE_NUM_MINOR_MASK 0x000000FF
#define NB_NB_VERSION_VERSION_RELEASE_NUM_MINOR_SHIFT 0
/*  Revision number (Major) */
#define NB_NB_VERSION_VERSION_RELEASE_NUM_MAJOR_MASK 0x0000FF00
#define NB_NB_VERSION_VERSION_RELEASE_NUM_MAJOR_SHIFT 8
/*  Date of release */
#define NB_NB_VERSION_VERSION_DATE_DAY_MASK 0x001F0000
#define NB_NB_VERSION_VERSION_DATE_DAY_SHIFT 16
/*  Month of release */
#define NB_NB_VERSION_VERSION_DATA_MONTH_MASK 0x01E00000
#define NB_NB_VERSION_VERSION_DATA_MONTH_SHIFT 21
/*  Year of release (starting from 2000) */
#define NB_NB_VERSION_VERSION_DATE_YEAR_MASK 0x3E000000
#define NB_NB_VERSION_VERSION_DATE_YEAR_SHIFT 25
/*  Reserved */
#define NB_NB_VERSION_VERSION_RESERVED_MASK 0xC0000000
#define NB_NB_VERSION_VERSION_RESERVED_SHIFT 30

/**** pp_config register ****/
/* Bypass PP module (formality equivalent) */
#define NB_PUSH_PACKET_PP_CONFIG_FM_BYPASS (1 << 0)
/* Bypass PP module */
#define NB_PUSH_PACKET_PP_CONFIG_BYPASS  (1 << 1)
/* Force Cleanup of entries */
#define NB_PUSH_PACKET_PP_CONFIG_CLEAR   (1 << 2)
/* Enable forwarding DECERR response */
#define NB_PUSH_PACKET_PP_CONFIG_DECERR_EN (1 << 3)
/* Enable forwarding SLVERR response */
#define NB_PUSH_PACKET_PP_CONFIG_SLVERR_EN (1 << 4)
/* Enable forwarding of data parity generation */
#define NB_PUSH_PACKET_PP_CONFIG_PAR_GEN_EN (1 << 5)
/* Select channel on 8K boundaries ([15:13]) instead of 64k boundaries ([18:16]). */
#define NB_PUSH_PACKET_PP_CONFIG_SEL_8K  (1 << 6)
/*
 * Forces aattribute to be as configured in ext_attr register.
 * Not functional
 */
#define NB_PUSH_PACKET_PP_CONFIG_SEL_EXT_ATTR (1 << 7)
/* parity reporting of the sram output */
#define NB_PUSH_PACKET_PP_CONFIG_PAR_ERR_EN (1 << 8)
/* data parity invert if parity error was detected on the data beat coming from the sram */
#define NB_PUSH_PACKET_PP_CONFIG_PAR_ERR_FWD_EN (1 << 9)
/*
 * Enables PP channel.
 * 1 bit per channel
 */
#define NB_PUSH_PACKET_PP_CONFIG_CHANNEL_ENABLE_MASK 0x00FF0000
#define NB_PUSH_PACKET_PP_CONFIG_CHANNEL_ENABLE_SHIFT 16

/**** pp_ext_attr register ****/
/*
 * Attribute to use on PP transactions
 * Only applicable if <sel_ext_attr> is set to 1'b1
 * Parity bits are still generated per transaction
 */
#define NB_PUSH_PACKET_PP_EXT_ATTR_VAL_MASK 0x03FFFFFF
#define NB_PUSH_PACKET_PP_EXT_ATTR_VAL_SHIFT 0

/**** pp_sel_attr register ****/
/*
 * Select whether to use addr[63:48] or PP awmisc as tgtid.
 * Each bit if set to 1 selects the corresponding address bit. Otherwise, selects the corersponding
 * awmisc bit.
 */
#define NB_PUSH_PACKET_PP_SEL_ATTR_VAL_MASK 0x0000FFFF
#define NB_PUSH_PACKET_PP_SEL_ATTR_VAL_SHIFT 0

/**** pp_err_log register ****/
/* Indicate parity error in push packet SRAM */
#define NB_PUSH_PACKET_PP_ERR_LOG_VALID  (1 << 0)
/* SRAM address that caused the parity error */
#define NB_PUSH_PACKET_PP_ERR_LOG_PTR_MASK 0x003F0000
#define NB_PUSH_PACKET_PP_ERR_LOG_PTR_SHIFT 16

#ifdef __cplusplus
}
#endif

#endif

/** @} */


