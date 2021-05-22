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
 * @file   al_hal_udma_regs_gen.h
 *
 * @brief UDMA_GEN registers
 *
 */

#ifndef __AL_HAL_UDMA_GEN_REGS_H__
#define __AL_HAL_UDMA_GEN_REGS_H__

#include "al_hal_udma_iofic_regs.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Unit Registers
 */

struct udma_gen_dma_misc {
	/* [0x0] Reserved register for the interrupt controller */
	uint32_t int_cfg;
	/* [0x4] Revision register */
	uint32_t revision;
	/* [0x8] Reserved for future use */
	uint32_t general_cfg_1;
	/* [0xc] Reserved for future use */
	uint32_t general_cfg_2;
	/* [0x10] Reserved for future use */
	uint32_t general_cfg_3;
	/* [0x14] Reserved for future use */
	uint32_t general_cfg_4;
	/*
	 * [0x18] General timer configuration
	 * Timer value
	 * 0 - not active
	 * 1-2^32 - SB clock cycles
	 */
	uint32_t general_cfg_5;
	/*
	 * [0x1c] from toprock UDMA_abort goes also for the applicaiton, this register should
	 * control if abort should go only to UDMA or only to application or both. Default goes to
	 * both
	 */
	uint32_t abort_ctl;
	/* [0x20] */
	uint32_t bw_bug_fixes;
	uint32_t rsrvd[55];
};

struct udma_gen_mailbox {
	/*
	 * [0x0] Mailbox interrupt generator.
	 * Generates interrupt to neighbor DMA
	 */
	uint32_t interrupt;
	/* [0x4] Mailbox message data out */
	uint32_t msg_out;
	/* [0x8] Mailbox message data in */
	uint32_t msg_in;
	uint32_t rsrvd[13];
};

struct udma_gen_axi {
	/*
	 * [0x0] Configuration of the AXI masters
	 * Timeout value for all transactions on the AXI
	 */
	uint32_t cfg_1;
	/* [0x4] Configuration of the AXI masters */
	uint32_t cfg_2;
	/* [0x8] Configuration of the AXI masters. Endianess configuration */
	uint32_t endian_cfg;
	uint32_t rsrvd[61];
};

struct udma_gen_sram_ctrl {
	/* [0x0] Timing configuration */
	uint32_t timing;
};

struct udma_gen_tgtid {
	/* [0x0] Target-ID control - V3 only */
	uint32_t cfg_tgtid_0;
	/* [0x4] TX queue 0/1 Target-ID - V3 only */
	uint32_t cfg_tgtid_1;
	/* [0x8] TX queue 2/3 Target-ID - V3 only */
	uint32_t cfg_tgtid_2;
	/* [0xc] RX queue 0/1 Target-ID - V3 only */
	uint32_t cfg_tgtid_3;
	/* [0x10] RX queue 2/3 Target-ID - V3 only */
	uint32_t cfg_tgtid_4;
};
struct udma_gen_tgtaddr_v3 {
	/* [0x0] TX queue 0/1 TGTADDR */
	uint32_t cfg_tgtaddr_0;
	/* [0x4] TX queue 2/3 TGTADDR */
	uint32_t cfg_tgtaddr_1;
	/* [0x8] RX queue 0/1 TGTADDR */
	uint32_t cfg_tgtaddr_2;
	/* [0xc] RX queue 2/3 TGTADDR */
	uint32_t cfg_tgtaddr_3;
};

struct udma_gen_tgtaddr_v4 {
	/* [0x0] Target-ID control */
	uint32_t cfg_vm_ctrl;
	uint32_t rsrvd_0;
	/* [0x8] TX queue TGTADDR/TGTID */
	uint32_t cfg_tx_vm;
	/* [0xc] RX queue TGTADDR/TGTID */
	uint32_t cfg_rx_vm;
};

struct udma_gen_axi_outstanding_cnt {
	/* [0x0] How many outstanding desc pref */
	uint32_t tx_desc_read;
	/* [0x4] How many outstanding data read */
	uint32_t tx_data_read;
	/* [0x8] How many outstanding cmpl write */
	uint32_t tx_cmpl_write;
	/* [0xc] How many outstanding desc pref */
	uint32_t rx_desc_read;
	/* [0x10] How many outstanding data write */
	uint32_t rx_data_write;
	/* [0x14] How many outstanding cmpl write */
	uint32_t rx_cmpl_write;
	/* [0x18] Write to this register will reset Outstnading cnt to zero */
	uint32_t rst_counters;
};

struct udma_gen_axi_error_detection_table {
	/* [0x0] Addr to table is {axi_parity_error,axi_timeout_error,axi_response_error} */
	uint32_t addr0;
	/* [0x4] */
	uint32_t addr1;
	/* [0x8] */
	uint32_t addr2;
	/* [0xc] */
	uint32_t addr3;
	/* [0x10] */
	uint32_t addr4;
	/* [0x14] */
	uint32_t addr5;
	/* [0x18] */
	uint32_t addr6;
	/* [0x1c] */
	uint32_t addr7;
	/* [0x20] */
	uint32_t addr8;
	/* [0x24] */
	uint32_t addr9;
	/* [0x28] */
	uint32_t addr10;
	/* [0x2c] */
	uint32_t addr11;
	/* [0x30] */
	uint32_t addr12;
	/* [0x34] */
	uint32_t addr13;
	/* [0x38] */
	uint32_t addr14;
	/* [0x3c] */
	uint32_t addr15;
};

struct udma_gen_axi_error_control {
	/* [0x0] */
	uint32_t table_addr;
	/* [0x4] */
	uint32_t table_data;
};

struct udma_gen_axi_queue {
	/* [0x0] this register can change axi queue state ACTIVE/NON_ACTIVE */
	uint32_t state_request;
	/* [0x4] This register is read on clear on read */
	uint32_t error_status;
	/* [0x8] */
	uint32_t cfg;
};

struct udma_gen_pmu {
	/*
	 * [0x0] RX HDR fifo
	 * PMUhas 3 counters
	 * 1.Fifo size lower then low th
	 * 2.FIfo size higher then high th
	 * 3.FIfo size is between th
	 */
	uint32_t rx_hdr;
	/*
	 * [0x4] RX DATA fifo
	 * PMUhas 3 counters
	 * 1.Fifo size lower then low th
	 * 2.FIfo size higher then high th
	 * 3.FIfo size is between th
	 */
	uint32_t rx_data;
	/*
	 * [0x8] TX DATA fifo
	 * PMUhas 3 counters
	 * 1.Fifo size lower then low th
	 * 2.FIfo size higher then high th
	 * 3.FIfo size is between th
	 */
	uint32_t tx_data;
	/* [0xc] PMU can count drop packet on selected queues, (4 queues to count on them) */
	uint32_t rx_drop;
};

struct udma_gen_init_memory {
	/* [0x0] */
	uint32_t rx_init_trigger;
	/* [0x4] */
	uint32_t rx_init_in_progress;
	/* [0x8] */
	uint32_t tx_init_trigger;
	/* [0xc] */
	uint32_t tx_init_in_progress;
	uint32_t rsrvd[4];
};

struct udma_gen_spare_reg {
	/* [0x0] */
	uint32_t zeroes0;
	/* [0x4] */
	uint32_t zeroes1;
	/* [0x8] */
	uint32_t ones0;
	/* [0xc] */
	uint32_t ones1;
};

struct udma_gen_regs_v3 {
	struct udma_iofic_regs interrupt_regs;		     /* [0x0000] */
	struct udma_gen_dma_misc dma_misc;                   /* [0x2080] */
	struct udma_gen_mailbox mailbox[4];                  /* [0x2180] */
	struct udma_gen_axi axi;                             /* [0x2280] */
	struct udma_gen_sram_ctrl sram_ctrl[25];             /* [0x2380] */
	uint32_t rsrvd_1[2];
	struct udma_gen_tgtid tgtid;                           /* [0x23ec] */
	struct udma_gen_tgtaddr_v3 tgtaddr;                    /* [0x2400] */
	uint32_t rsrvd_2[252];
};

struct udma_gen_regs_v4 {
	struct udma_iofic_regs interrupt_regs;		     /* [0x0000] */
	uint32_t rsrvd_0[32];
	struct udma_gen_dma_misc dma_misc;                   /* [0x2100] */
	struct udma_gen_mailbox mailbox[4];                  /* [0x2200] */
	struct udma_gen_axi axi;                             /* [0x2300] */
	struct udma_gen_sram_ctrl sram_ctrl[25];             /* [0x2400] */
	uint32_t rsrvd_1[39];
	struct udma_gen_tgtid tgtid;                           /* [0x2500] */
	uint32_t rsrvd_2[59];
	struct udma_gen_tgtaddr_v4 tgtaddr[16];                   /* [0x2600] */
	struct udma_gen_axi_outstanding_cnt axi_outstanding_cnt[16]; /* [0x2700] */
	uint32_t rsrvd_3[16];
	struct udma_gen_axi_error_detection_table axi_error_detection_table[7]; /* [0x2900] */
	uint32_t rsrvd_4[16];
	struct udma_gen_axi_error_control axi_error_control[7]; /* [0x2b00] */
	uint32_t rsrvd_5[50];
	struct udma_gen_axi_queue axi_queue[16];             /* [0x2c00] */
	uint32_t rsrvd_6[16];
	uint32_t iofic_base_m2s_desc_rd;			/* [0x2d00] */
	uint32_t rsrvd_m7[63];
	uint32_t iofic_base_m2s_data_rd;			/* [0x2e00] */
	uint32_t rsrvd_m8[63];
	uint32_t iofic_base_m2s_cmpl_wr;			/* [0x2f00] */
	uint32_t rsrvd_m9[63];
	uint32_t iofic_base_s2m_desc_rd;			/* [0x3000] */
	uint32_t rsrvd_m10[63];
	uint32_t iofic_base_s2m_data_wr;			/* [0x3100] */
	uint32_t rsrvd_m11[63];
	uint32_t iofic_base_s2m_cmpl_wr;			/* [0x3200] */
	uint32_t rsrvd_m12[63];
	uint32_t iofic_base_msix;				/* [0x3300] */
	uint32_t rsrvd_m13[63];
	struct udma_gen_pmu pmu;                             /* [0x3400] */
	uint32_t rsrvd_7[4];
	struct udma_gen_init_memory init_memory;             /* [0x3420] */
	uint32_t rsrvd_8[8];
	struct udma_gen_spare_reg spare_reg;                 /* [0x3460] */
};

#ifndef AL_UDMA_EX
struct udma_gen_ex_vmpr_v4 {
	uint32_t tx_sel;
	uint32_t rx_sel[3];
	uint32_t rsrvd[12];
};

struct udma_gen_ex_regs {
	uint32_t rsrvd[0x100];
	struct udma_gen_ex_vmpr_v4 vmpr_v4[16];              /* [0x400] */
};
#endif

/*
 * Registers Fields
 */

/**** int_cfg register ****/
/*
 * MSIX data width
 * 1 - 64 bit
 * 0 - 32 bit
 */
#define UDMA_GEN_DMA_MISC_INT_CFG_MSIX_64 (1 << 0)
/* General configuration */
#define UDMA_GEN_DMA_MISC_INT_CFG_RESERVED_3_1_MASK 0x0000000E
#define UDMA_GEN_DMA_MISC_INT_CFG_RESERVED_3_1_SHIFT 1
/* MSIx AXI QoS */
#define UDMA_GEN_DMA_MISC_INT_CFG_MSIX_AXI_QOS_MASK 0x00000070
#define UDMA_GEN_DMA_MISC_INT_CFG_MSIX_AXI_QOS_SHIFT 4

#define UDMA_GEN_DMA_MISC_INT_CFG_RESERVED_31_7_MASK 0xFFFFFF80
#define UDMA_GEN_DMA_MISC_INT_CFG_RESERVED_31_7_SHIFT 7

/**** revision register ****/
/* Design programming interface  revision ID */
#define UDMA_GEN_DMA_MISC_REVISION_PROGRAMMING_ID_MASK 0x00000FFF
#define UDMA_GEN_DMA_MISC_REVISION_PROGRAMMING_ID_SHIFT 0
/* Design minor revision ID */
#define UDMA_GEN_DMA_MISC_REVISION_MINOR_ID_MASK 0x00FFF000
#define UDMA_GEN_DMA_MISC_REVISION_MINOR_ID_SHIFT 12
/* Design major revision ID */
#define UDMA_GEN_DMA_MISC_REVISION_MAJOR_ID_MASK 0xFF000000
#define UDMA_GEN_DMA_MISC_REVISION_MAJOR_ID_SHIFT 24

/**** abort_ctl register ****/
/*
 * When set mask the abort indicaiton coming from INT controller (after local int Controller mask) -
 * otherwise pass abort indicaiton (same as GDMA)
 */
#define UDMA_GEN_DMA_MISC_ABORT_CTL_MASK_LOCAL_ABORT (1 << 0)
/*
 * When set mask the abort indicaiton Going toward application/Adapter from INT controller (after
 * local int Controller mask)
 */
#define UDMA_GEN_DMA_MISC_ABORT_CTL_MASK_APP_ABORT (1 << 1)

/**** bw_bug_fixes register ****/
/* When '1' BUG fix enabled When '0' work as PKR mode */
#define UDMA_GEN_DMA_MISC_BW_BUG_FIXES_RX_DATA_BUILD_FSM (1 << 0)
/* When '1' BUG fix enabled When '0' work as PKR mode */
#define UDMA_GEN_DMA_MISC_BW_BUG_FIXES_RX_PREF_FIFO (1 << 1)
/*
 * When '1' BUG fix enabled
 * When '0' work as PKR mode
 */
#define UDMA_GEN_DMA_MISC_BW_BUG_FIXES_TX_STREAMER_FSM (1 << 2)

/**** Interrupt register ****/
/* Generate interrupt to another DMA */
#define UDMA_GEN_MAILBOX_INTERRUPT_SET   (1 << 0)

/**** cfg_2 register ****/
/*
 * Enable arbitration promotion.
 * Increment master priority after configured number of arbitration cycles
 */
#define UDMA_GEN_AXI_CFG_2_ARB_PROMOTION_MASK 0x0000000F
#define UDMA_GEN_AXI_CFG_2_ARB_PROMOTION_SHIFT 0

/**** endian_cfg register ****/
/* Swap M2S descriptor read and completion descriptor write.  */
#define UDMA_GEN_AXI_ENDIAN_CFG_SWAP_M2S_DESC (1 << 0)
/* Swap M2S data read. */
#define UDMA_GEN_AXI_ENDIAN_CFG_SWAP_M2S_DATA (1 << 1)
/* Swap S2M descriptor read and completion descriptor write.  */
#define UDMA_GEN_AXI_ENDIAN_CFG_SWAP_S2M_DESC (1 << 2)
/* Swap S2M data write. */
#define UDMA_GEN_AXI_ENDIAN_CFG_SWAP_S2M_DATA (1 << 3)
/*
 * Swap 32 or 64 bit mode:
 * 0 - Swap groups of 4 bytes
 * 1 - Swap groups of 8 bytes
 */
#define UDMA_GEN_AXI_ENDIAN_CFG_SWAP_64B_EN (1 << 4)

/**** timing register ****/
/* Write margin */
#define UDMA_GEN_SRAM_CTRL_TIMING_RMA_MASK 0x0000000F
#define UDMA_GEN_SRAM_CTRL_TIMING_RMA_SHIFT 0
/* Write margin enable */
#define UDMA_GEN_SRAM_CTRL_TIMING_RMEA   (1 << 8)
/* Read margin */
#define UDMA_GEN_SRAM_CTRL_TIMING_RMB_MASK 0x000F0000
#define UDMA_GEN_SRAM_CTRL_TIMING_RMB_SHIFT 16
/* Read margin enable */
#define UDMA_GEN_SRAM_CTRL_TIMING_RMEB   (1 << 24)

/**** cfg_tgtid_0 register ****/
/* For M2S queues 3:0, enable usage of the Target-ID from the buffer address 63:56 */
/* V3 only */
#define UDMA_GEN_TGTID_CFG_TGTID_0_TX_Q_TGTID_DESC_EN_MASK 0x0000000F
#define UDMA_GEN_TGTID_CFG_TGTID_0_TX_Q_TGTID_DESC_EN_SHIFT 0
#define UDMA_GEN_TGTID_CFG_TX_DESC_EN(qid) (AL_BIT((qid) + 0))

/*
 * For M2S queues 3:0, enable usage of the Target-ID from the configuration register
 * (cfg_tgtid_1/2 used for M2S queue_x)
 */
/* V3 only */
#define UDMA_GEN_TGTID_CFG_TGTID_0_TX_Q_TGTID_QUEUE_EN_MASK 0x000000F0
#define UDMA_GEN_TGTID_CFG_TGTID_0_TX_Q_TGTID_QUEUE_EN_SHIFT 4
#define UDMA_GEN_TGTID_CFG_TX_QUEUE_EN(qid) (AL_BIT((qid) + 4))

/* use TGTID_n [7:0] from MSI-X Controller for MSI-X message  */
#define UDMA_GEN_TGTID_CFG_TGTID_0_MSIX_TGTID_SEL (1 << 8)
/* Enable write to all TGTID_n registers in the MSI-X Controller */
#define UDMA_GEN_TGTID_CFG_TGTID_0_MSIX_TGTID_ACCESS_EN (1 << 9)
/* For S2M queues 3:0, enable usage of the Target-ID from the buffer address 63:56 */
/* V3 only */
#define UDMA_GEN_TGTID_CFG_TGTID_0_RX_Q_TGTID_DESC_EN_MASK 0x000F0000
#define UDMA_GEN_TGTID_CFG_TGTID_0_RX_Q_TGTID_DESC_EN_SHIFT 16
#define UDMA_GEN_TGTID_CFG_RX_DESC_EN(qid) (AL_BIT((qid) + 16))

/*
 * For S2M queues 3:0, enable usage of the Target-ID from the configuration register
 * (cfg_tgtid_3/4 used for M2S queue_x)
 */
/* V3 only */
#define UDMA_GEN_TGTID_CFG_TGTID_0_RX_Q_TGTID_QUEUE_EN_MASK 0x00F00000
#define UDMA_GEN_TGTID_CFG_TGTID_0_RX_Q_TGTID_QUEUE_EN_SHIFT 20
#define UDMA_GEN_TGTID_CFG_RX_QUEUE_EN(qid) (AL_BIT((qid) + 20))

#define UDMA_GEN_TGTID_CFG_TGTID_SHIFT(qid)	(((qid) & 0x1) ? 16 : 0)
#define UDMA_GEN_TGTID_CFG_TGTID_MASK(qid)	(((qid) & 0x1) ? 0xFFFF0000 : 0x0000FFFF)

/**** cfg_tgtid_1 register ****/
/* V3 only */
/* TX queue 0 Target-ID value */
#define UDMA_GEN_TGTID_CFG_TGTID_1_TX_Q_0_TGTID_MASK 0x0000FFFF
#define UDMA_GEN_TGTID_CFG_TGTID_1_TX_Q_0_TGTID_SHIFT 0
/* TX queue 1 Target-ID value */
#define UDMA_GEN_TGTID_CFG_TGTID_1_TX_Q_1_TGTID_MASK 0xFFFF0000
#define UDMA_GEN_TGTID_CFG_TGTID_1_TX_Q_1_TGTID_SHIFT 16

/**** cfg_tgtid_2 register ****/
/* V3 only */
/* TX queue 2 Target-ID value */
#define UDMA_GEN_TGTID_CFG_TGTID_2_TX_Q_2_TGTID_MASK 0x0000FFFF
#define UDMA_GEN_TGTID_CFG_TGTID_2_TX_Q_2_TGTID_SHIFT 0
/* TX queue 3 Target-ID value */
#define UDMA_GEN_TGTID_CFG_TGTID_2_TX_Q_3_TGTID_MASK 0xFFFF0000
#define UDMA_GEN_TGTID_CFG_TGTID_2_TX_Q_3_TGTID_SHIFT 16

/**** cfg_tgtid_3 register ****/
/* V3 only */
/* RX queue 0 Target-ID value */
#define UDMA_GEN_TGTID_CFG_TGTID_3_RX_Q_0_TGTID_MASK 0x0000FFFF
#define UDMA_GEN_TGTID_CFG_TGTID_3_RX_Q_0_TGTID_SHIFT 0
/* RX queue 1 Target-ID value */
#define UDMA_GEN_TGTID_CFG_TGTID_3_RX_Q_1_TGTID_MASK 0xFFFF0000
#define UDMA_GEN_TGTID_CFG_TGTID_3_RX_Q_1_TGTID_SHIFT 16

/**** cfg_tgtid_4 register ****/
/* V3 only */
/* RX queue 2 Target-ID value */
#define UDMA_GEN_TGTID_CFG_TGTID_4_RX_Q_2_TGTID_MASK 0x0000FFFF
#define UDMA_GEN_TGTID_CFG_TGTID_4_RX_Q_2_TGTID_SHIFT 0
/* RX queue 3 Target-ID value */
#define UDMA_GEN_TGTID_CFG_TGTID_4_RX_Q_3_TGTID_MASK 0xFFFF0000
#define UDMA_GEN_TGTID_CFG_TGTID_4_RX_Q_3_TGTID_SHIFT 16

#define UDMA_GEN_TGTADDR_CFG_SHIFT(qid)	(((qid) & 0x1) ? 16 : 0)
#define UDMA_GEN_TGTADDR_CFG_MASK(qid)	(((qid) & 0x1) ? 0xFFFF0000 : 0x0000FFFF)

/**** cfg_tgtaddr_0 register ****/
/* V3 only */
/* TX queue 0 TGTADDR value */
#define UDMA_GEN_TGTADDR_CFG_TGTADDR_0_TX_Q_0_TGTADDR_MASK 0x0000FFFF
#define UDMA_GEN_TGTADDR_CFG_TGTADDR_0_TX_Q_0_TGTADDR_SHIFT 0
/* TX queue 1 TGTADDR value */
#define UDMA_GEN_TGTADDR_CFG_TGTADDR_0_TX_Q_1_TGTADDR_MASK 0xFFFF0000
#define UDMA_GEN_TGTADDR_CFG_TGTADDR_0_TX_Q_1_TGTADDR_SHIFT 16

/**** cfg_tgtaddr_1 register ****/
/* V3 only */
/* TX queue 2 TGTADDR value */
#define UDMA_GEN_TGTADDR_CFG_TGTADDR_1_TX_Q_2_TGTADDR_MASK 0x0000FFFF
#define UDMA_GEN_TGTADDR_CFG_TGTADDR_1_TX_Q_2_TGTADDR_SHIFT 0
/* TX queue 3 TGTADDR value */
#define UDMA_GEN_TGTADDR_CFG_TGTADDR_1_TX_Q_3_TGTADDR_MASK 0xFFFF0000
#define UDMA_GEN_TGTADDR_CFG_TGTADDR_1_TX_Q_3_TGTADDR_SHIFT 16

/**** cfg_tgtaddr_2 register ****/
/* V3 only */
/* RX queue 0 TGTADDR value */
#define UDMA_GEN_TGTADDR_CFG_TGTADDR_2_RX_Q_0_TGTADDR_MASK 0x0000FFFF
#define UDMA_GEN_TGTADDR_CFG_TGTADDR_2_RX_Q_0_TGTADDR_SHIFT 0
/* RX queue 1 TGTADDR value */
#define UDMA_GEN_TGTADDR_CFG_TGTADDR_2_RX_Q_1_TGTADDR_MASK 0xFFFF0000
#define UDMA_GEN_TGTADDR_CFG_TGTADDR_2_RX_Q_1_TGTADDR_SHIFT 16

/**** cfg_tgtaddr_3 register ****/
/* V3 only */
/* RX queue 2 TGTADDR value */
#define UDMA_GEN_TGTADDR_CFG_TGTADDR_3_RX_Q_2_TGTADDR_MASK 0x0000FFFF
#define UDMA_GEN_TGTADDR_CFG_TGTADDR_3_RX_Q_2_TGTADDR_SHIFT 0
/* RX queue 3 TGTADDR value */
#define UDMA_GEN_TGTADDR_CFG_TGTADDR_3_RX_Q_3_TGTADDR_MASK 0xFFFF0000
#define UDMA_GEN_TGTADDR_CFG_TGTADDR_3_RX_Q_3_TGTADDR_SHIFT 16

/**** cfg_vm_ctrl register ****/
/* For S2M queues, enable usage of the Target-ID from the buffer address 63:56 */
#define UDMA_GEN_TGTADDR_CFG_VM_CTRL_RX_Q_TGTID_DESC_EN (1 << 0)
/*
 * For S2M queues , enable usage of the Target-ID from the configuration register
 * (cfg_tgtid_3/4 used for M2S queue_x)
 */
#define UDMA_GEN_TGTADDR_CFG_VM_CTRL_RX_Q_TGTID_QUEUE_EN (1 << 1)
/* For M2S queues, enable usage of the Target-ID from the buffer address 63:56 */
#define UDMA_GEN_TGTADDR_CFG_VM_CTRL_TX_Q_TGTID_DESC_EN (1 << 2)
/*
 * For M2S queues, enable usage of the Target-ID from the configuration register
 * (cfg_tgtid_1/2 used for M2S queue_x)
 */
#define UDMA_GEN_TGTADDR_CFG_VM_CTRL_TX_Q_TGTID_QUEUE_EN (1 << 3)

/**** cfg_tx_vm register ****/
/* TX queue Target-ID value */
#define UDMA_GEN_TGTADDR_CFG_TX_VM_TX_Q_TGTID_MASK 0x0000FFFF
#define UDMA_GEN_TGTADDR_CFG_TX_VM_TX_Q_TGTID_SHIFT 0
/* TX queue TGTADDR value */
#define UDMA_GEN_TGTADDR_CFG_TX_VM_TX_Q_TGTADDR_MASK 0xFFFF0000
#define UDMA_GEN_TGTADDR_CFG_TX_VM_TX_Q_TGTADDR_SHIFT 16

/**** cfg_rx_vm register ****/
/* RX queue Target-ID value */
#define UDMA_GEN_TGTADDR_CFG_RX_VM_RX_Q_TGTID_MASK 0x0000FFFF
#define UDMA_GEN_TGTADDR_CFG_RX_VM_RX_Q_TGTID_SHIFT 0
/* RX queue TGTADDR value */
#define UDMA_GEN_TGTADDR_CFG_RX_VM_RX_Q_TGTADDR_MASK 0xFFFF0000
#define UDMA_GEN_TGTADDR_CFG_RX_VM_RX_Q_TGTADDR_SHIFT 16

#define UDMA_GEN_AXI_OUTSTANDING_CNT_TX_DESC_READ_VALUE_MASK 0xFF000000
#define UDMA_GEN_AXI_OUTSTANDING_CNT_TX_DESC_READ_VALUE_SHIFT 24

#define UDMA_GEN_AXI_OUTSTANDING_CNT_TX_DATA_READ_VALUE_MASK 0xFF000000
#define UDMA_GEN_AXI_OUTSTANDING_CNT_TX_DATA_READ_VALUE_SHIFT 24

#define UDMA_GEN_AXI_OUTSTANDING_CNT_TX_CMPL_WRITE_VALUE_MASK 0xFF000000
#define UDMA_GEN_AXI_OUTSTANDING_CNT_TX_CMPL_WRITE_VALUE_SHIFT 24

#define UDMA_GEN_AXI_OUTSTANDING_CNT_RX_DESC_READ_VALUE_MASK 0xFF000000
#define UDMA_GEN_AXI_OUTSTANDING_CNT_RX_DESC_READ_VALUE_SHIFT 24

#define UDMA_GEN_AXI_OUTSTANDING_CNT_RX_DATA_WRITE_VALUE_MASK 0xFF000000
#define UDMA_GEN_AXI_OUTSTANDING_CNT_RX_DATA_WRITE_VALUE_SHIFT 24

#define UDMA_GEN_AXI_OUTSTANDING_CNT_RX_CMPL_WRITE_VALUE_MASK 0xFF000000
#define UDMA_GEN_AXI_OUTSTANDING_CNT_RX_CMPL_WRITE_VALUE_SHIFT 24

/**** rst_counters register ****/

#define UDMA_GEN_AXI_OUTSTANDING_CNT_RST_COUNTERS_RESERVED_25_0_MASK 0x03FFFFFF
#define UDMA_GEN_AXI_OUTSTANDING_CNT_RST_COUNTERS_RESERVED_25_0_SHIFT 0

#define UDMA_GEN_AXI_OUTSTANDING_CNT_RST_COUNTERS_RX_CMPL_WRITE (1 << 26)

#define UDMA_GEN_AXI_OUTSTANDING_CNT_RST_COUNTERS_RX_DATA_WRITE (1 << 27)

#define UDMA_GEN_AXI_OUTSTANDING_CNT_RST_COUNTERS_RX_DESC_READ (1 << 28)

#define UDMA_GEN_AXI_OUTSTANDING_CNT_RST_COUNTERS_TX_CMPL_WRITE (1 << 29)

#define UDMA_GEN_AXI_OUTSTANDING_CNT_RST_COUNTERS_TX_DATA_READ (1 << 30)

#define UDMA_GEN_AXI_OUTSTANDING_CNT_RST_COUNTERS_TX_DESC_READ (1 << 31)

/**** addr0 register ****/
/* Coding of error type */
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR0_ERROR_TYPE_MASK 0x00000007
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR0_ERROR_TYPE_SHIFT 0
/* decide if action required */
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR0_ERROR_INDICATION (1 << 3)

/**** addr1 register ****/
/* Coding of error type */
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR1_ERROR_TYPE_MASK 0x00000007
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR1_ERROR_TYPE_SHIFT 0
/* decide if action required */
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR1_ERROR_INDICATION (1 << 3)

/**** addr2 register ****/
/* Coding of error type */
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR2_ERROR_TYPE_MASK 0x00000007
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR2_ERROR_TYPE_SHIFT 0
/* decide if action required */
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR2_ERROR_INDICATION (1 << 3)

/**** addr3 register ****/
/* Coding of error type */
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR3_ERROR_TYPE_MASK 0x00000007
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR3_ERROR_TYPE_SHIFT 0
/* decide if action required */
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR3_ERROR_INDICATION (1 << 3)

/**** addr4 register ****/
/* Coding of error type */
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR4_ERROR_TYPE_MASK 0x00000007
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR4_ERROR_TYPE_SHIFT 0
/* decide if action required */
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR4_ERROR_INDICATION (1 << 3)

/**** addr5 register ****/
/* Coding of error type */
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR5_ERROR_TYPE_MASK 0x00000007
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR5_ERROR_TYPE_SHIFT 0
/* decide if action required */
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR5_ERROR_INDICATION (1 << 3)

/**** addr6 register ****/
/* Coding of error type */
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR6_ERROR_TYPE_MASK 0x00000007
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR6_ERROR_TYPE_SHIFT 0
/* decide if action required */
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR6_ERROR_INDICATION (1 << 3)

/**** addr7 register ****/
/* Coding of error type */
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR7_ERROR_TYPE_MASK 0x00000007
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR7_ERROR_TYPE_SHIFT 0
/* decide if action required */
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR7_ERROR_INDICATION (1 << 3)

/**** addr8 register ****/
/* Coding of error type */
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR8_ERROR_TYPE_MASK 0x00000007
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR8_ERROR_TYPE_SHIFT 0
/* decide if action required */
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR8_ERROR_INDICATION (1 << 3)

/**** addr9 register ****/
/* Coding of error type */
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR9_ERROR_TYPE_MASK 0x00000007
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR9_ERROR_TYPE_SHIFT 0
/* decide if action required */
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR9_ERROR_INDICATION (1 << 3)

/**** addr10 register ****/
/* Coding of error type */
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR10_ERROR_TYPE_MASK 0x00000007
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR10_ERROR_TYPE_SHIFT 0
/* decide if action required */
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR10_ERROR_INDICATION (1 << 3)

/**** addr11 register ****/
/* Coding of error type */
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR11_ERROR_TYPE_MASK 0x00000007
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR11_ERROR_TYPE_SHIFT 0
/* decide if action required */
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR11_ERROR_INDICATION (1 << 3)

/**** addr12 register ****/
/* Coding of error type */
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR12_ERROR_TYPE_MASK 0x00000007
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR12_ERROR_TYPE_SHIFT 0
/* decide if action required */
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR12_ERROR_INDICATION (1 << 3)

/**** addr13 register ****/
/* Coding of error type */
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR13_ERROR_TYPE_MASK 0x00000007
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR13_ERROR_TYPE_SHIFT 0
/* decide if action required */
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR13_ERROR_INDICATION (1 << 3)

/**** addr14 register ****/
/* Coding of error type */
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR14_ERROR_TYPE_MASK 0x00000007
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR14_ERROR_TYPE_SHIFT 0
/* decide if action required */
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR14_ERROR_INDICATION (1 << 3)

/**** addr15 register ****/
/* Coding of error type */
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR15_ERROR_TYPE_MASK 0x00000007
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR15_ERROR_TYPE_SHIFT 0
/* decide if action required */
#define UDMA_GEN_AXI_ERROR_DETECTION_TABLE_ADDR15_ERROR_INDICATION (1 << 3)

/**** table_addr register ****/

#define UDMA_GEN_AXI_ERROR_CONTROL_TABLE_ADDR_VAL_MASK 0x0000007F
#define UDMA_GEN_AXI_ERROR_CONTROL_TABLE_ADDR_VAL_SHIFT 0

/**** table_data register ****/
/* This bit stop rx for relevantg queue */
#define UDMA_GEN_AXI_ERROR_CONTROL_TABLE_DATA_ENABLE_RX_STOP (1 << 0)
/* This bit stop tx for relevantg queue */
#define UDMA_GEN_AXI_ERROR_CONTROL_TABLE_DATA_ENABLE_TX_STOP (1 << 1)
/* this bit sample the error that occured to  the status register (for relevant queue) */
#define UDMA_GEN_AXI_ERROR_CONTROL_TABLE_DATA_SET_DETECTION_STATUS (1 << 2)

#define UDMA_GEN_AXI_ERROR_CONTROL_TABLE_DATA_SPARE_BIT (1 << 3)
/* this bit decide if to set INT A/B/C/D for relevant queue */
#define UDMA_GEN_AXI_ERROR_CONTROL_TABLE_DATA_ENABLE_INT_MASK 0x000000F0
#define UDMA_GEN_AXI_ERROR_CONTROL_TABLE_DATA_ENABLE_INT_SHIFT 4

/**** state_request register ****/
/* Change rx queue to active state */
#define UDMA_GEN_AXI_QUEUE_STATE_REQUEST_RX_SET_QUEUE_STATE_ACTIVE (1 << 0)
/* Change rx queue to non-active state */
#define UDMA_GEN_AXI_QUEUE_STATE_REQUEST_RX_SET_QUEUE_STATE_NON_ACTIVE (1 << 1)
/* Change tx queue to active state */
#define UDMA_GEN_AXI_QUEUE_STATE_REQUEST_TX_SET_QUEUE_STATE_ACTIVE (1 << 2)
/* Change tx queue to non-active state */
#define UDMA_GEN_AXI_QUEUE_STATE_REQUEST_TX_SET_QUEUE_STATE_NON_ACTIVE (1 << 3)

/**** error_status register ****/
/* axi_parity_error,axi_timeout_error,axi_response_error} */
#define UDMA_GEN_AXI_QUEUE_ERROR_STATUS_M2S_DESC_READ_MASK 0x0000000F
#define UDMA_GEN_AXI_QUEUE_ERROR_STATUS_M2S_DESC_READ_SHIFT 0
/* axi_parity_error,axi_timeout_error,axi_response_error} */
#define UDMA_GEN_AXI_QUEUE_ERROR_STATUS_M2S_DATA_READ_MASK 0x000000F0
#define UDMA_GEN_AXI_QUEUE_ERROR_STATUS_M2S_DATA_READ_SHIFT 4
/* axi_parity_error,axi_timeout_error,axi_response_error} */
#define UDMA_GEN_AXI_QUEUE_ERROR_STATUS_M2S_CMPL_WRITE_MASK 0x00000F00
#define UDMA_GEN_AXI_QUEUE_ERROR_STATUS_M2S_CMPL_WRITE_SHIFT 8
/* axi_parity_error,axi_timeout_error,axi_response_error} */
#define UDMA_GEN_AXI_QUEUE_ERROR_STATUS_S2M_DESC_READ_MASK 0x0000F000
#define UDMA_GEN_AXI_QUEUE_ERROR_STATUS_S2M_DESC_READ_SHIFT 12
/* axi_parity_error,axi_timeout_error,axi_response_error} */
#define UDMA_GEN_AXI_QUEUE_ERROR_STATUS_S2M_DATA_WRITE_MASK 0x000F0000
#define UDMA_GEN_AXI_QUEUE_ERROR_STATUS_S2M_DATA_WRITE_SHIFT 16
/* axi_parity_error,axi_timeout_error,axi_response_error} */
#define UDMA_GEN_AXI_QUEUE_ERROR_STATUS_S2M_CMPL_WRITE_MASK 0x00F00000
#define UDMA_GEN_AXI_QUEUE_ERROR_STATUS_S2M_CMPL_WRITE_SHIFT 20
/* axi_parity_error,axi_timeout_error,axi_response_error} */
#define UDMA_GEN_AXI_QUEUE_ERROR_STATUS_MSIX_MASK 0x0F000000
#define UDMA_GEN_AXI_QUEUE_ERROR_STATUS_MSIX_SHIFT 24
/*
 * indicates if error occured - when value is bigger then 0 error occured, and value means how many
 * times, stuck at max_error_occured (below register) until cleared
 */
#define UDMA_GEN_AXI_QUEUE_ERROR_STATUS_ERROR_OCCURED_MASK 0xF0000000
#define UDMA_GEN_AXI_QUEUE_ERROR_STATUS_ERROR_OCCURED_SHIFT 28

/**** cfg register ****/
/*
 * State of axi error per queue
 * 0 -ACTIVE
 * 1 - NON_ACTIVE
 */
#define UDMA_GEN_AXI_QUEUE_CFG_RX_STATE  (1 << 0)
/*
 * indicates to “mark” rx pkt (CMPL)
 * 0-  dont mark pkt with error
 * 1 - mark pkt with error if queue in NON_ACTIVE state
 * 2 -  mark any pkt with error
 *
 * Marking will cause
 * 1.Stopping Queue prefetch
 * 2.Drop pkt from stream
 * 3.Set Error bit in S2M CMPL
 * Each one of the three behaviors can be masked independently
 */
#define UDMA_GEN_AXI_QUEUE_CFG_RX_MARK_PKT_MASK 0x00000030
#define UDMA_GEN_AXI_QUEUE_CFG_RX_MARK_PKT_SHIFT 4
/* When set- masked the rx stop prefetch (Prefetch will occur) */
#define UDMA_GEN_AXI_QUEUE_CFG_RX_MASK_STOP_DESC_PREFETCH (1 << 6)
/* When set- masked the rx drop stream (Pkt will NOT! Be dropped) */
#define UDMA_GEN_AXI_QUEUE_CFG_RX_MASK_DROP_STREAM (1 << 7)
/*
 * When set- masked the cmpl error indication (No Error will be reported in S2M CMPL due to queue in
 * non active state)
 */
#define UDMA_GEN_AXI_QUEUE_CFG_RX_MASK_CMPL_ERROR (1 << 8)
/*
 * State of axi error per queue
 * 0 -ACTIVE
 * 1 - NON_ACTIVE
 */
#define UDMA_GEN_AXI_QUEUE_CFG_TX_STATE  (1 << 12)
/*
 * indicates to “mark” tx pkt (STREAM)
 * 0-  dont mark pkt with error
 * 1 - mark pkt with error if queue in NON_ACTIVE state
 * 2 -  mark any pkt with error
 * Marking will cause
 * 1.Stopping Queue prefetch
 * 2. Stopping Queue scheduling
 * 3. Set Error bit in M2S CMPL
 * 4. Set Error bit in AXI_STREAM i/f with APP
 * Each one of the four  behaviors can be masked independently
 */
#define UDMA_GEN_AXI_QUEUE_CFG_TX_MARK_PKT_MASK 0x00030000
#define UDMA_GEN_AXI_QUEUE_CFG_TX_MARK_PKT_SHIFT 16
/* When set- masked the tx stop prefetch (Prefetch will occur) */
#define UDMA_GEN_AXI_QUEUE_CFG_TX_MASK_STOP_DESC_PREFETCH (1 << 18)
/* When set- masked the tx pkt sched (Pkt will BE! Sched) */
#define UDMA_GEN_AXI_QUEUE_CFG_TX_MASK_STOP_SCHED (1 << 19)
/*
 * When set- masked the cmpl error indication (No Error will be reported in M2S CMPL due to queue in
 * non active state)
 */
#define UDMA_GEN_AXI_QUEUE_CFG_TX_MASK_CMPL_ERROR (1 << 20)
/* When set Mask the error bit indication to APP on axi stream */
#define UDMA_GEN_AXI_QUEUE_CFG_TX_MASK_STREAM_ERROR (1 << 21)
/*
 * Max value that error occurred cnt will cnt for (from error status register), when reaching to
 * this value, cnt will stuck until reset (clear on read)
 */
#define UDMA_GEN_AXI_QUEUE_CFG_MAX_ERROR_OCCURRED_MASK 0x0F000000
#define UDMA_GEN_AXI_QUEUE_CFG_MAX_ERROR_OCCURRED_SHIFT 24

/**** rx_hdr register ****/

#define UDMA_GEN_PMU_RX_HDR_INUSE_LOW_TH_MASK 0x0000003F
#define UDMA_GEN_PMU_RX_HDR_INUSE_LOW_TH_SHIFT 0

#define UDMA_GEN_PMU_RX_HDR_INUSE_HIGH_TH_MASK 0x003F0000
#define UDMA_GEN_PMU_RX_HDR_INUSE_HIGH_TH_SHIFT 16

/**** rx_data register ****/

#define UDMA_GEN_PMU_RX_DATA_INUSE_LOW_TH_MASK 0x0000007F
#define UDMA_GEN_PMU_RX_DATA_INUSE_LOW_TH_SHIFT 0

#define UDMA_GEN_PMU_RX_DATA_INUSE_HIGH_TH_MASK 0x007F0000
#define UDMA_GEN_PMU_RX_DATA_INUSE_HIGH_TH_SHIFT 16

/**** tx_data register ****/

#define UDMA_GEN_PMU_TX_DATA_INUSE_LOW_TH_MASK 0x000007FF
#define UDMA_GEN_PMU_TX_DATA_INUSE_LOW_TH_SHIFT 0

#define UDMA_GEN_PMU_TX_DATA_INUSE_HIGH_TH_MASK 0x07FF0000
#define UDMA_GEN_PMU_TX_DATA_INUSE_HIGH_TH_SHIFT 16

/**** rx_drop register ****/

#define UDMA_GEN_PMU_RX_DROP_Q0_CNT_MASK 0x0000000F
#define UDMA_GEN_PMU_RX_DROP_Q0_CNT_SHIFT 0

#define UDMA_GEN_PMU_RX_DROP_Q1_CNT_MASK 0x00000F00
#define UDMA_GEN_PMU_RX_DROP_Q1_CNT_SHIFT 8

#define UDMA_GEN_PMU_RX_DROP_Q2_CNT_MASK 0x000F0000
#define UDMA_GEN_PMU_RX_DROP_Q2_CNT_SHIFT 16

#define UDMA_GEN_PMU_RX_DROP_Q3_CNT_MASK 0x0F000000
#define UDMA_GEN_PMU_RX_DROP_Q3_CNT_SHIFT 24

/**** rx_init_trigger register ****/

#define UDMA_GEN_INIT_MEMORY_RX_INIT_TRIGGER_CMPL_MEM0 (1 << 0)

#define UDMA_GEN_INIT_MEMORY_RX_INIT_TRIGGER_CMPL_MEM1 (1 << 1)

/**** rx_init_in_progress register ****/

#define UDMA_GEN_INIT_MEMORY_RX_INIT_IN_PROGRESS_CMPL_MEM1 (1 << 0)

#define UDMA_GEN_INIT_MEMORY_RX_INIT_IN_PROGRESS_CMPL_MEM0 (1 << 1)

/**** tx_init_trigger register ****/

#define UDMA_GEN_INIT_MEMORY_TX_INIT_TRIGGER_CMPL_MEM0 (1 << 0)

#define UDMA_GEN_INIT_MEMORY_TX_INIT_TRIGGER_CMPL_MEM1 (1 << 1)

/**** tx_init_in_progress register ****/

#define UDMA_GEN_INIT_MEMORY_TX_INIT_IN_PROGRESS_CMPL_MEM1 (1 << 0)

#define UDMA_GEN_INIT_MEMORY_TX_INIT_IN_PROGRESS_CMPL_MEM0 (1 << 1)

#ifdef __cplusplus
}
#endif

#endif

/** @} */


