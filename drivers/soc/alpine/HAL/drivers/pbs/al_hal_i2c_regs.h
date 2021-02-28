/*******************************************************************************
Copyright (C) 2015 Annapurna Labs Ltd.

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

#ifndef __AL_HAL_I2C_REGS_H__
#define __AL_HAL_I2C_REGS_H__

#include "al_hal_plat_types.h"

struct al_i2c_regs {
	/* 0x00 */
	uint16_t con;		/* i2c control register */
	uint16_t reserved0;
	uint16_t tar;		/* target device for current xfer */
	uint16_t reserved1;
	uint16_t sar;		/* device's Slave mode address */
	uint16_t reserved2;
	uint16_t hs_maddr;		/* device's Master mode address */
	uint16_t reserved3;
	/* 0x10 */
	uint16_t data_cmd;		/* data buffer and command register
					   write-> write byte to TX FIFO
					   read-> retrieve byte from RX FIFO */
	uint16_t reserved4;
	uint16_t ss_scl_hcnt;
	uint16_t reserved5;
	uint16_t ss_scl_lcnt;
	uint16_t reserved6;
	uint16_t fs_scl_hcnt;
	uint16_t reserved7;
	/* 0x20 */
	uint16_t fs_scl_lcnt;
	uint16_t reserved8;
	uint16_t hs_scl_hcnt;
	uint16_t reserved9;
	uint16_t hs_scl_lcnt;
	uint16_t reserved10;
	uint16_t intr_stat;		/* interrupt values after masking */
	uint16_t reserved11;
	/* 0x30 */
	uint16_t intr_mask;		/* interrupt masking */
	uint16_t reserved12;
	uint16_t raw_intr_stat;	/* interrupt values before masking */
	uint16_t reserved13;
	uint16_t rx_tl;		/* receive FIFO threshold */
	uint16_t reserved14;
	uint16_t tx_tl;		/* transmit FIFO threshold */
	uint16_t reserved15;
	/* 0x40 */
	uint16_t clr_intr;		/* read this register in order to
					   clear all SW controlled intrrupts.

					   read one of the following registers
					   to clear the corresponding intr  */
	uint16_t reserved16;
	uint16_t clr_rx_under;
	uint16_t reserved17;
	uint16_t clr_rx_over;
	uint16_t reserved18;
	uint16_t clr_tx_over;
	uint16_t reserved19;
	/* 0x50 */
	uint16_t clr_rd_req;
	uint16_t reserved20;
	uint16_t clr_tx_abrt;
	uint16_t reserved21;
	uint16_t clr_rx_done;
	uint16_t reserved22;
	uint16_t clr_activity;
	uint16_t reserved23;
	/* 0x60 */
	uint16_t clr_stop_det;
	uint16_t reserved24;
	uint16_t clr_start_det;
	uint16_t reserved25;
	uint16_t clr_gen_call;
	uint16_t reserved26;
	uint16_t enable;		/* enable the device / abort xfer */
	uint16_t reserved27;
	/* 0x70 */
	uint32_t status;		/* xfer & fifos status */
	uint32_t txflr;		/* transmit fifo level */
	uint32_t rxflr;		/* receive fifo level */
	uint32_t sda_hold;
	/* 0x80 */
	uint32_t tx_abrt_source;
	uint32_t slv_data_nack_only;
	uint32_t dma_cr;
	uint32_t dma_tdlr;
	/* 0x90 */
	uint32_t dma_rdlr;
	uint32_t sda_setup;
	uint32_t ack_general_call;
	uint32_t enable_status;
	/* 0xA0 */
	uint32_t fs_spklen;
	uint32_t hs_spklen;
	uint32_t reserved28[19]; /* 0xA8 + 19*4 = 0xF4 */
	/* 0xF4 */
	uint32_t comp_param_1;
	uint32_t comp_version;
	uint32_t comp_type;
};

#define AL_I2C_CON_MASTER_MODE_SHIFT		0x0
#define AL_I2C_CON_MASTER_MODE_MASK		(0x1 << \
		AL_I2C_CON_MASTER_MODE_SHIFT)
#define AL_I2C_CON_MASTER_MODE_SLAVE		(0x0 << \
		AL_I2C_CON_MASTER_MODE_SHIFT)
#define AL_I2C_CON_MASTER_MODE_MASTER		(0x1 << \
		AL_I2C_CON_MASTER_MODE_SHIFT)
#define AL_I2C_CON_MAX_SPEED_MODE_SHIFT		0x1
#define AL_I2C_CON_MAX_SPEED_MODE_MASK		(0x3 << \
		AL_I2C_CON_MAX_SPEED_MODE_SHIFT)
#define AL_I2C_CON_MAX_SPEED_MODE_STANDARD	(0x1 << \
		AL_I2C_CON_MAX_SPEED_MODE_SHIFT)
#define AL_I2C_CON_MAX_SPEED_MODE_FAST		(0x2 << \
		AL_I2C_CON_MAX_SPEED_MODE_SHIFT)
#define AL_I2C_CON_MAX_SPEED_MODE_HIGH		(0x3 << \
		AL_I2C_CON_MAX_SPEED_MODE_SHIFT)
#define AL_I2C_CON_10BITADDR_SLAVE_SHIFT	0x3
#define AL_I2C_CON_10BITADDR_SLAVE_MASK		(0x1 <<	\
		AL_I2C_CON_10BITADDR_SLAVE_SHIFT)
#define AL_I2C_CON_10BITADDR_MASTER_SHIFT	0x4
#define AL_I2C_CON_10BITADDR_MASTER_MASK	(0x1 <<	\
		AL_I2C_CON_10BITADDR_MASTER_SHIFT)
#define AL_I2C_CON_10BITADDR_MASTER_ENABLE	(0x1 <<	\
		AL_I2C_CON_10BITADDR_MASTER_SHIFT)
#define AL_I2C_CON_RESTART_EN_SHIFT		0x5
#define AL_I2C_CON_RESTART_EN_MASK		(0x1 <<	\
		AL_I2C_CON_RESTART_EN_SHIFT)
#define AL_I2C_CON_SLAVE_DISABLE_SHIFT		0x6
#define AL_I2C_CON_SLAVE_DISABLE_MASK		(0x1 <<	\
		AL_I2C_CON_SLAVE_DISABLE_SHIFT)
#define AL_I2C_CON_SLAVE_DISABLE_DISABLE	(0x1 << \
		AL_I2C_CON_SLAVE_DISABLE_SHIFT)


#define AL_I2C_TAR_TARGET_SHIFT			0x0
#define AL_I2C_TAR_TARGET_MASK			(0x03FF << \
		AL_I2C_TAR_TARGET_SHIFT)
#define AL_I2C_TAR_10BIT_ADDR_SHIFT		0xC
#define AL_I2C_TAR_10BIT_ADDR_MASK		(0x1 << \
		AL_I2C_TAR_10BIT_ADDR_SHIFT)
#define AL_I2C_TAR_10BIT_ADDR_ENABLE		(0x1 << \
		AL_I2C_TAR_10BIT_ADDR_SHIFT)

#define AL_I2C_DATA_CMD_CMD_SHIFT		0x8
#define AL_I2C_DATA_CMD_CMD_MASK		(0x1 <<	\
	AL_I2C_DATA_CMD_CMD_SHIFT)
#define AL_I2C_DATA_CMD_READ			(0x1 <<	\
	AL_I2C_DATA_CMD_CMD_SHIFT)
#define AL_I2C_DATA_CMD_STOP_SHIFT		0x9
#define AL_I2C_DATA_CMD_STOP_MASK		(0x1 <<	\
	AL_I2C_DATA_CMD_STOP_SHIFT)
#define AL_I2C_DATA_CMD_STOP			(0x1 << \
	AL_I2C_DATA_CMD_STOP_SHIFT)

#define AL_I2C_ENABLE_SHIFT		0
#define AL_I2C_ENABLE_MASK		(0x1 << \
	AL_I2C_ENABLE_SHIFT)
#define AL_I2C_ENABLE_DISABLE		0
#define AL_I2C_ENABLE_ENABLE		(0x1 << \
	AL_I2C_ENABLE_SHIFT)

#define AL_I2C_STATUS_ACTIVITY_SHIFT		0x0
#define AL_I2C_STATUS_ACTIVITY_MASK		(0x1<< \
		AL_I2C_STATUS_ACTIVITY_SHIFT)
#define AL_I2C_STATUS_ACTIVITY_INACTIVE		(0x0<< \
		AL_I2C_STATUS_ACTIVITY_SHIFT)
#define AL_I2C_STATUS_ACTIVITY_ACTIVE		(0x1<< \
		AL_I2C_STATUS_ACTIVITY_SHIFT)
#define AL_I2C_STATUS_TFNF_SHIFT		0x1
#define AL_I2C_STATUS_TFNF_MASK			(0x1<< \
		AL_I2C_STATUS_TFNF_SHIFT)
#define AL_I2C_STATUS_TFNF_FULL			(0x0<< \
		AL_I2C_STATUS_TFNF_SHIFT)
#define AL_I2C_STATUS_TFNF_NOT_FULL		(0x1<< \
		AL_I2C_STATUS_TFNF_SHIFT)
#define AL_I2C_STATUS_TFE_SHIFT			0x2
#define AL_I2C_STATUS_TFE_MASK			(0x1<< \
		AL_I2C_STATUS_TFE_SHIFT)
#define AL_I2C_STATUS_TFE_NOT_EMPTY		(0x0<< \
		AL_I2C_STATUS_TFE_SHIFT)
#define AL_I2C_STATUS_TFE_EMPTY			(0x1<< \
		AL_I2C_STATUS_TFE_SHIFT)
#define AL_I2C_STATUS_RFNE_SHIFT		0x3
#define AL_I2C_STATUS_RFNE_MASK			(0x1<< \
		AL_I2C_STATUS_RFNE_SHIFT)
#define AL_I2C_STATUS_RFNE_EMPTY		(0x0<< \
		AL_I2C_STATUS_RFNE_SHIFT)
#define AL_I2C_STATUS_RFNE_NOT_EMPTY		(0x1<< \
		AL_I2C_STATUS_RFNE_SHIFT)
#define AL_I2C_STATUS_RFF_SHIFT			0x4
#define AL_I2C_STATUS_RFF_MASK			(0x1<< \
		AL_I2C_STATUS_RFF_SHIFT)
#define AL_I2C_STATUS_RFF_NOT_FULL		(0x0<< \
		AL_I2C_STATUS_RFF_SHIFT)
#define AL_I2C_STATUS_RFF_FULL			(0x1<< \
		AL_I2C_STATUS_RFF_SHIFT)
#define AL_I2C_STATUS_MST_ACTIVITY_SHIFT	0x5
#define AL_I2C_STATUS_MST_ACTIVITY_MASK		(0x1<< \
		AL_I2C_STATUS_MST_ACTIVITY_SHIFT)
#define AL_I2C_STATUS_MST_ACTIVITY_NOT_ACTIVE	(0x0<< \
		AL_I2C_STATUS_MST_ACTIVITY_SHIFT)
#define AL_I2C_STATUS_MST_ACTIVITY_ACTIVE	(0x1<< \
		AL_I2C_STATUS_MST_ACTIVITY_SHIFT)
#define AL_I2C_STATUS_SLV_ACTIVITY_SHIFT	0x6
#define AL_I2C_STATUS_SLV_ACTIVITY_MASK		(0x1<< \
		AL_I2C_STATUS_SLV_ACTIVITY_SHIFT)
#define AL_I2C_STATUS_SLV_ACTIVITY_NOT_ACTIVE	(0x0<< \
		AL_I2C_STATUS_SLV_ACTIVITY_SHIFT)
#define AL_I2C_STATUS_SLV_ACTIVITY_ACTIVE	(0x1<< \
		AL_I2C_STATUS_SLV_ACTIVITY_SHIFT)

#define AL_I2C_INTR_MASK_RX_UNDER_SHIFT		0x0
#define AL_I2C_INTR_MASK_RX_UNDER_MASK		(0x1<< \
		AL_I2C_INTR_MASK_RX_UNDER_SHIFT)
#define AL_I2C_INTR_MASK_RX_OVER_SHIFT		0x1
#define AL_I2C_INTR_MASK_RX_OVER_MASK		(0x1<< \
		AL_I2C_INTR_MASK_RX_OVER_SHIFT)
#define AL_I2C_INTR_MASK_RX_FULL_SHIFT		0x2
#define AL_I2C_INTR_MASK_RX_FULL_MASK		(0x1<< \
		AL_I2C_INTR_MASK_RX_FULL_SHIFT)
#define AL_I2C_INTR_MASK_TX_OVER_SHIFT		0x3
#define AL_I2C_INTR_MASK_TX_OVER_MASK		(0x1<< \
		AL_I2C_INTR_MASK_TX_OVER_SHIFT)
#define AL_I2C_INTR_MASK_TX_EMPTY_SHIFT		0x4
#define AL_I2C_INTR_MASK_TX_EMPTY_MASK		(0x1<< \
		AL_I2C_INTR_MASK_TX_EMPTY_SHIFT)
#define AL_I2C_INTR_MASK_RD_REQ_SHIFT		0x5
#define AL_I2C_INTR_MASK_RD_REQ_MASK		(0x1<< \
		AL_I2C_INTR_MASK_RD_REQ_SHIFT)
#define AL_I2C_INTR_MASK_TX_ABRT_SHIFT		0x6
#define AL_I2C_INTR_MASK_TX_ABRT_MASK		(0x1<< \
		AL_I2C_INTR_MASK_TX_ABRT_SHIFT)
#define AL_I2C_INTR_MASK_RX_DONE_SHIFT		0x7
#define AL_I2C_INTR_MASK_RX_DONE_MASK		(0x1<< \
		AL_I2C_INTR_MASK_RX_DONE_SHIFT)
#define AL_I2C_INTR_MASK_ACTIVITY_SHIFT		0x8
#define AL_I2C_INTR_MASK_ACTIVITY_MASK		(0x1<< \
		AL_I2C_INTR_MASK_ACTIVITY_SHIFT)
#define AL_I2C_INTR_MASK_STOP_DET_SHIFT		0x9
#define AL_I2C_INTR_MASK_STOP_DET_MASK		(0x1<< \
		AL_I2C_INTR_MASK_STOP_DET_SHIFT)
#define AL_I2C_INTR_MASK_START_DET_SHIFT	0xA
#define AL_I2C_INTR_MASK_START_DET_MASK		(0x1<< \
		AL_I2C_INTR_MASK_START_DET_SHIFT)
#define AL_I2C_INTR_MASK_GEN_CALL_SHIFT		0xB
#define AL_I2C_INTR_MASK_GEN_CALL_MASK		(0x1<< \
		AL_I2C_INTR_MASK_GEN_CALL_SHIFT)

#endif

