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

/**
 *  @{
 * @file   al_hal_tdm_regs.h
 *
 * @brief ... registers
 *
 */

#ifndef __AL_HAL_TDM_REGS_H__
#define __AL_HAL_TDM_REGS_H__

#include "al_hal_plat_types.h"

#ifdef __cplusplus
extern "C" {
#endif
/*
* Unit Registers
*/



struct al_tdm_tdm_cfg {
	/* [0x0] Slots configuration - number of slots and size of slot. */
	uint32_t slot;
	uint32_t rsrvd_0;
	/* [0x8] PCM Clock Configuration */
	uint32_t pcm_clk_gen;
	uint32_t rsrvd_1;
	/* [0x10] PCM 8KHz reference window */
	uint32_t pcm_window;
	uint32_t rsrvd_2;
	/* [0x18] Frame Sync control */
	uint32_t fsync_ctl;
	uint32_t rsrvd_3;
	/* [0x20] Tx channel control */
	uint32_t tx_ctl;
	uint32_t rsrvd_4;
	/* [0x28] Rx channel control */
	uint32_t rx_ctl;
	uint32_t rsrvd_5;
	/* [0x30] Tx Underflow data */
	uint32_t tx_uf_data;
	uint32_t rsrvd_6;
	/* [0x38] Tx Non Active data */
	uint32_t tx_na_data;
	uint32_t rsrvd_7;
	/* [0x40] Tx active slots 31:0 */
	uint32_t tx_sa_0;
	uint32_t rsrvd_8;
	/* [0x48] Tx active slots 63:32 */
	uint32_t tx_sa_1;
	uint32_t rsrvd_9;
	/* [0x50] Tx active slots 95:64 */
	uint32_t tx_sa_2;
	uint32_t rsrvd_10;
	/* [0x58] Tx active slots 127:96 */
	uint32_t tx_sa_3;
	uint32_t rsrvd_11;
	/* [0x60] Rx active slots 31:0 */
	uint32_t rx_sa_0;
	uint32_t rsrvd_12;
	/* [0x68] Rx active slots 63:32 */
	uint32_t rx_sa_1;
	uint32_t rsrvd_13;
	/* [0x70] Rx active slots 95:64 */
	uint32_t rx_sa_2;
	uint32_t rsrvd_14;
	/* [0x78] Rx active slots 127:96 */
	uint32_t rx_sa_3;
	uint32_t rsrvd_15;
	/* [0x80] Tx limits */
	uint32_t tx_limits;
	uint32_t rsrvd_16;
	/* [0x88] Rx limits */
	uint32_t rx_limits;
	uint32_t rsrvd_17;
	/* [0x90] Rx limits */
	uint32_t misc;
	uint32_t rsrvd_18;
	/* [0x98]  */
	uint32_t reserved;
	uint32_t rsrvd_19;
	/* [0xa0] Tx Status */
	uint32_t txstatus;
	uint32_t rsrvd_20;
	/* [0xa8] Tx pointers in sample count */
	uint32_t txsmplptr;
	uint32_t rsrvd_21;
	/* [0xb0] Tx pointers in byte count */
	uint32_t txbyteptr;
	uint32_t rsrvd_22;
	/* [0xb8] Rx Status */
	uint32_t rxstatus;
	uint32_t rsrvd_23;
	/* [0xc0] Rx pointers in sample count */
	uint32_t rxsmplptr;
	uint32_t rsrvd_24;
	/* [0xc8] Rx pointers in byte count */
	uint32_t rxbyteptr;
	uint32_t rsrvd_25;
	/* [0xd0] PCM Clk Gen loop tracking */
	uint32_t pcm_genstatus;
	uint32_t rsrvd_26;
	/* [0xd8] Emergency clear of the AXI tracking mechanisms */
	uint32_t axi_special;
	uint32_t rsrvd_27;
	/* [0xe0] TX increment register */
	uint32_t tx_inc;
	uint32_t rsrvd_28;
	/* [0xe8] RX increment register */
	uint32_t rx_inc;
	uint32_t rsrvd_29;
	/* [0xf0] TDM inflight fifos clear */
	uint32_t tdmclear;
	uint32_t rsrvd_30;
	/* [0xf8] TDM Main Enables */
	uint32_t tdmenable;
	uint32_t rsrvd;
};

struct al_tdm_regs {
	uint32_t rsrvd_0[2048];
	struct al_tdm_tdm_cfg tdm_cfg;                          /* [0x2000] */
};


/*
* Registers Fields
*/


/**** slot register ****/
/* Number of slots. Up to 128. Encoded power-of-two */
#define TDM_TDM_CFG_SLOT_NUM_MASK        0x00000007
#define TDM_TDM_CFG_SLOT_NUM_SHIFT       0
/* Slot size. 00=8bits, 01=16bits, 10=24bits, 11=32bits. */
#define TDM_TDM_CFG_SLOT_SIZE_MASK       0x00000030
#define TDM_TDM_CFG_SLOT_SIZE_SHIFT      4
/* zsi protocol mixer (provision) */
#define TDM_TDM_CFG_SLOT_ZSI_EN          (1 << 31)

/**** PCM_CLK_GEN register ****/
/*
 * divide ratio of PCM CLK from it source. Default is Internal 500MHz and target
 * 8.192MHz, so divide value is 61.035
 */
#define TDM_TDM_CFG_PCM_CLK_GEN_PCM_CLK_RATIO_MASK 0x000007FF
#define TDM_TDM_CFG_PCM_CLK_GEN_PCM_CLK_RATIO_SHIFT 0
/*
 * In case a deviation is detected, how many pcmclk cycles shall be using the
 * corrective value
 */
#define TDM_TDM_CFG_PCM_CLK_GEN_PCM_TUNE_PERIOD_MASK 0x007FF000
#define TDM_TDM_CFG_PCM_CLK_GEN_PCM_TUNE_PERIOD_SHIFT 12
/*
 * In case a deviation is detected, how many pcmclk cycles (extra or less) shall
 * be using the corrective value vs. the base period (to speed up convergence)
 */
#define TDM_TDM_CFG_PCM_CLK_GEN_PCM_TUNE_DELTA_MASK 0x0F000000
#define TDM_TDM_CFG_PCM_CLK_GEN_PCM_TUNE_DELTA_SHIFT 24
/* Divide ration is rounded down (0) or up (1) as base ratio. */
#define TDM_TDM_CFG_PCM_CLK_GEN_PCM_TUNE_RND_DIR (1 << 28)
/*
 * When correcting a deviation, does it keep the deviated value till the
 * opposite deviation is detected, OR does it return to default after one tuning
 * attempt (Default is returning to base value).
 */
#define TDM_TDM_CFG_PCM_CLK_GEN_PCM_TUNE_KEEP (1 << 29)
/*
 * Using an internal source may require a dynamic (adaptive) tuning of the
 * pcm-clk. Default: Dynamic
 */
#define TDM_TDM_CFG_PCM_CLK_GEN_PCM_TUNE_DYNAMIC (1 << 30)
/* Internal or External reference clock source. Default is internal */
#define TDM_TDM_CFG_PCM_CLK_GEN_PCM_CLK_SOURCE (1 << 31)

/**** PCM_WINDOW register ****/
/* 8KHz generation of a 25MHz reference clock. */
#define TDM_TDM_CFG_PCM_WINDOW_WINDOW_FRAME_RATIO_MASK 0x00000FFF
#define TDM_TDM_CFG_PCM_WINDOW_WINDOW_FRAME_RATIO_SHIFT 0
/*
 * Using internal clock as reference may require rational division, hence exact
 * match will never happen, but as close as possible to averaging the target.
 * The hystheresis window is the count deviation for the exact match for which
 * tuning will be activated.
 */
#define TDM_TDM_CFG_PCM_WINDOW_WINDOW_HYST_LEVEL_MASK 0x3F000000
#define TDM_TDM_CFG_PCM_WINDOW_WINDOW_HYST_LEVEL_SHIFT 24
/*
 * In order to save convergence period, start already at first iteration with a
 * tuning value as expected to be used later on. 00- start from the exact
 * configuration. 01 - start from base+1 (slower by one integer), 10 - start
 * from base-1 (faster by one integer)
 */
#define TDM_TDM_CFG_PCM_WINDOW_WINDOW_HYST_START_MASK 0xC0000000
#define TDM_TDM_CFG_PCM_WINDOW_WINDOW_HYST_START_SHIFT 30

/**** FSYNC_CTL register ****/
/* Length period of Fsync (N-1) */
#define TDM_TDM_CFG_FSYNC_CTL_LEN_MASK   0x000003FF
#define TDM_TDM_CFG_FSYNC_CTL_LEN_SHIFT  0
/* Fsync polarity. 1=invert, 0= as is. */
#define TDM_TDM_CFG_FSYNC_CTL_INV        (1 << 14)
/* Fsync edge alignment. 1= rising edge 0 = falling edge */
#define TDM_TDM_CFG_FSYNC_CTL_FSYNC_EDGE (1 << 15)

/**** TX_CTL register ****/
/* Data Delay 0-7 */
#define TDM_TDM_CFG_TX_CTL_TXD_DELAY_MASK 0x00000007
#define TDM_TDM_CFG_TX_CTL_TXD_DELAY_SHIFT 0
/*
 * At inactive slots, drive default value, or keep tristated. Default is keep
 * tristated
 */
#define TDM_TDM_CFG_TX_CTL_TXD_DRV_INACTIVE (1 << 5)
/* Data polarity. 1=invert, 0= as is. */
#define TDM_TDM_CFG_TX_CTL_TXD_INV       (1 << 6)
/* Data edge alignment. 1= rising edge 0 = falling edge */
#define TDM_TDM_CFG_TX_CTL_TXD_EDGE      (1 << 7)
/* Data Channel Enable Delay 0-7 */
#define TDM_TDM_CFG_TX_CTL_TX_EN_DELAY_MASK 0x00000700
#define TDM_TDM_CFG_TX_CTL_TX_EN_DELAY_SHIFT 8
/* Data Channel Enable duration. Encoded as N-1 */
#define TDM_TDM_CFG_TX_CTL_TX_EN_LEN_MASK 0x003FF000
#define TDM_TDM_CFG_TX_CTL_TX_EN_LEN_SHIFT 12
/* Data Channel Enable polarity. 1=invert, 0= as is. */
#define TDM_TDM_CFG_TX_CTL_TX_EN_INV     (1 << 22)
/* Data Channel Enable edge alignment. 1= rising edge 0 = falling edge */
#define TDM_TDM_CFG_TX_CTL_TX_EN_EDGE    (1 << 23)
/* Tx sample size. 00=8bit, 01=16bit, 10=24bit, 11=32bit */
#define TDM_TDM_CFG_TX_CTL_TX_SMPL_SIZE_MASK 0x03000000
#define TDM_TDM_CFG_TX_CTL_TX_SMPL_SIZE_SHIFT 24
/* Tx sample alignment. 1=Start of Slot, 0=End of slot (padding zone). */
#define TDM_TDM_CFG_TX_CTL_TX_SMPL_ALIGN (1 << 26)
/* Tx data during inactive slot cycles of an active channel */
#define TDM_TDM_CFG_TX_CTL_TX_PADDING_DATA (1 << 27)
/*
 * Tx Host data format - padded to word size, or consecutive sample bytes stream
 */
#define TDM_TDM_CFG_TX_CTL_TX_HOST_PADDING (1 << 28)
/* Tx Host data alignment within a word (if padded). 1=MSB, 0=LSB */
#define TDM_TDM_CFG_TX_CTL_TX_HOST_ALIGNMENT (1 << 29)
/*
 * Tx channel enable encoding of the 3rd channel and on. 1= encode as 11. 0=
 * encode as 00
 */
#define TDM_TDM_CFG_TX_CTL_TX_EN_SPECIAL (1 << 30)
/*
 * Tx channel pointers automatic management. When set - data pointers increment
 * automatically once data beat is written in. If zero, SW must provide pointers
 * increment information for streaming to begin.
 */
#define TDM_TDM_CFG_TX_CTL_TX_AUTO_TRACK (1 << 31)

/**** RX_CTL register ****/
/* Data Delay 0-7 */
#define TDM_TDM_CFG_RX_CTL_RXD_DELAY_MASK 0x00000007
#define TDM_TDM_CFG_RX_CTL_RXD_DELAY_SHIFT 0
/* Data polarity. 1=invert, 0= as is. */
#define TDM_TDM_CFG_RX_CTL_RXD_INV       (1 << 6)
/* Data edge alignment. 1= rising edge 0 = falling edge */
#define TDM_TDM_CFG_RX_CTL_RXD_EDGE      (1 << 7)
/* Data Channel Enable Delay 0-7 */
#define TDM_TDM_CFG_RX_CTL_RX_EN_DELAY_MASK 0x00000700
#define TDM_TDM_CFG_RX_CTL_RX_EN_DELAY_SHIFT 8
/* Data Channel Enable duration. Encoded as N-1 */
#define TDM_TDM_CFG_RX_CTL_RX_EN_LEN_MASK 0x003FF000
#define TDM_TDM_CFG_RX_CTL_RX_EN_LEN_SHIFT 12
/* Data Channel Enable polarity. 1=invert, 0= as is. */
#define TDM_TDM_CFG_RX_CTL_RX_EN_INV     (1 << 22)
/* Data Channel Enable edge alignment. 1= rising edge 0 = falling edge */
#define TDM_TDM_CFG_RX_CTL_RX_EN_EDGE    (1 << 23)
/* Rx sample size. 00=8bit, 01=16bit, 10=24bit, 11=32bit */
#define TDM_TDM_CFG_RX_CTL_RX_SMPL_SIZE_MASK 0x03000000
#define TDM_TDM_CFG_RX_CTL_RX_SMPL_SIZE_SHIFT 24
/* Rx sample alignment. 1=Start of Slot, 0=End of slot. */
#define TDM_TDM_CFG_RX_CTL_RX_SMPL_ALIGN (1 << 26)
/*
 * Rx Host data format - padded to word size, or consecutive sample bytes stream
 */
#define TDM_TDM_CFG_RX_CTL_RX_HOST_PADDING (1 << 28)
/* Rx Host data alignment within a word (if padded). 1=MSB, 0=LSB */
#define TDM_TDM_CFG_RX_CTL_RX_HOST_ALIGNMENT (1 << 29)
/*
 * Rx channel enable encoding of the 3rd channel and on. 1= encode as 11. 0=
 * encode as 00
 */
#define TDM_TDM_CFG_RX_CTL_RX_EN_SPECIAL (1 << 30)
/*
 * Rx channel pointers automatic management. When set - data pointers increment
 * automatically once data beat is read out. If zero, SW must provide pointers
 * increment information for streaming to begin.
 */
#define TDM_TDM_CFG_RX_CTL_RX_AUTO_TRACK (1 << 31)

/**** TX_LIMITS register ****/
/* Data Base max limit in sample-count */
#define TDM_TDM_CFG_TX_LIMITS_MAX_MASK   0x00001FFF
#define TDM_TDM_CFG_TX_LIMITS_MAX_SHIFT  0
/* Data Base almost-empty limit in sample-count */
#define TDM_TDM_CFG_TX_LIMITS_ALMOST_EMPTY_MASK 0x1FFF0000
#define TDM_TDM_CFG_TX_LIMITS_ALMOST_EMPTY_SHIFT 16

/**** RX_LIMITS register ****/
/* Data Base max limit in sample-count */
#define TDM_TDM_CFG_RX_LIMITS_MAX_MASK   0x00001FFF
#define TDM_TDM_CFG_RX_LIMITS_MAX_SHIFT  0
/*
 * Data Base level in which SW is being informed that there is enough data piled
 * up to be fetched. In sample-count
 */
#define TDM_TDM_CFG_RX_LIMITS_AVAIL_MASK 0x1FFF0000
#define TDM_TDM_CFG_RX_LIMITS_AVAIL_SHIFT 16

/**** MISC register ****/
/* enable parity calculation of the data bases read */
#define TDM_TDM_CFG_MISC_PARITY_EN       (1 << 0)
/*
 * enable parity error propagation through AXI/APB reads. Requires that parity
 * will be enabled.
 */
#define TDM_TDM_CFG_MISC_SLVERR_EN       (1 << 1)
/*
 * (internal) Associated with Tx AutoTrack - enables access to be in whatever
 * size, as long as it is consecutive. The Accounting takes into consideration
 * the exact data access size. Setting to 1'b1 binds the access to be 32bits
 * aligned and sized. Default is not binding.
 */
#define TDM_TDM_CFG_MISC_TX_AUTO_DELTA32 (1 << 24)
/*
 * (internal) Associated with Rx AutoTrack - enables access to be in whatever
 * size, as long as it is consecutive. The Accounting takes into consideration
 * the exact data access size. Setting to 1'b1 binds the access to be 32bits
 * aligned and sized. Default is not binding.
 */
#define TDM_TDM_CFG_MISC_RX_AUTO_DELTA32 (1 << 25)
/*
 * (internal) Test mode assist. If no device and external RXD connected to TXD -
 * enable alignment of Rx data such that it will start sampling in only when Tx
 * submits out. This simplifies loopback data comparison tests.
 */
#define TDM_TDM_CFG_MISC_LOOPBACK_ALIGN  (1 << 28)
/*
 * (internal) Drive TXD and Channel Enables at slots even if data is not yet
 * valid at start of day. Data will be the inactive value, but channel enable
 * will be asserted to the device (internal). Default is that TX drives out only
 * from real-value data in hand at start of day.
 */
#define TDM_TDM_CFG_MISC_TX_DRIVE_ALWAYS (1 << 29)

/**** TXStatus register ****/
/* Tx in use (sample count) */
#define TDM_TDM_CFG_TXSTATUS_TX_INUSE_MASK 0x00001FFF
#define TDM_TDM_CFG_TXSTATUS_TX_INUSE_SHIFT 0

#define TDM_TDM_CFG_TXSTATUS_RSVD_19_13_MASK 0x000FE000
#define TDM_TDM_CFG_TXSTATUS_RSVD_19_13_SHIFT 13
/* Tx full */
#define TDM_TDM_CFG_TXSTATUS_TX_FULL     (1 << 20)

#define TDM_TDM_CFG_TXSTATUS_RSVD_23_21_MASK 0x00E00000
#define TDM_TDM_CFG_TXSTATUS_RSVD_23_21_SHIFT 21
/* Tx Almost Empty */
#define TDM_TDM_CFG_TXSTATUS_TX_AEMPTY   (1 << 24)

#define TDM_TDM_CFG_TXSTATUS_RSVD_27_25_MASK 0x0E000000
#define TDM_TDM_CFG_TXSTATUS_RSVD_27_25_SHIFT 25
/* Tx Underrun condition */
#define TDM_TDM_CFG_TXSTATUS_TX_UNDERRUN (1 << 28)

#define TDM_TDM_CFG_TXSTATUS_RSVD_31_29_MASK 0xE0000000
#define TDM_TDM_CFG_TXSTATUS_RSVD_31_29_SHIFT 29

/**** TXsmplptr register ****/
/* head pointer (samples) */
#define TDM_TDM_CFG_TXSMPLPTR_HEAD_MASK  0x00001FFF
#define TDM_TDM_CFG_TXSMPLPTR_HEAD_SHIFT 0

#define TDM_TDM_CFG_TXSMPLPTR_RSVD_15_13_MASK 0x0000E000
#define TDM_TDM_CFG_TXSMPLPTR_RSVD_15_13_SHIFT 13
/* tail pointer (samples) */
#define TDM_TDM_CFG_TXSMPLPTR_TAIL_MASK  0x1FFF0000
#define TDM_TDM_CFG_TXSMPLPTR_TAIL_SHIFT 16

#define TDM_TDM_CFG_TXSMPLPTR_RSVD_31_29_MASK 0xE0000000
#define TDM_TDM_CFG_TXSMPLPTR_RSVD_31_29_SHIFT 29

/**** TXbyteptr register ****/
/* head pointer (bytes) */
#define TDM_TDM_CFG_TXBYTEPTR_HEAD_MASK  0x00001FFF
#define TDM_TDM_CFG_TXBYTEPTR_HEAD_SHIFT 0
/* tail pointer (bytes) */
#define TDM_TDM_CFG_TXBYTEPTR_TAIL_MASK  0x1FFF0000
#define TDM_TDM_CFG_TXBYTEPTR_TAIL_SHIFT 16

/**** RXStatus register ****/
/* Rx in use (sample count) */
#define TDM_TDM_CFG_RXSTATUS_TX_INUSE_MASK 0x00001FFF
#define TDM_TDM_CFG_RXSTATUS_TX_INUSE_SHIFT 0
/* Rx full */
#define TDM_TDM_CFG_RXSTATUS_RX_FULL     (1 << 16)
/* Rx empty */
#define TDM_TDM_CFG_RXSTATUS_RX_EMPTY    (1 << 20)
/* Rx Almost Full */
#define TDM_TDM_CFG_RXSTATUS_RX_AFULL    (1 << 24)
/* Rx Overrun condition */
#define TDM_TDM_CFG_RXSTATUS_RX_OVERRUN  (1 << 28)

/**** RXsmplptr register ****/
/* head pointer (samples) */
#define TDM_TDM_CFG_RXSMPLPTR_HEAD_MASK  0x00001FFF
#define TDM_TDM_CFG_RXSMPLPTR_HEAD_SHIFT 0

#define TDM_TDM_CFG_RXSMPLPTR_RSVD_15_13_MASK 0x0000E000
#define TDM_TDM_CFG_RXSMPLPTR_RSVD_15_13_SHIFT 13
/* tail pointer (samples) */
#define TDM_TDM_CFG_RXSMPLPTR_TAIL_MASK  0x1FFF0000
#define TDM_TDM_CFG_RXSMPLPTR_TAIL_SHIFT 16

#define TDM_TDM_CFG_RXSMPLPTR_RSVD_31_29_MASK 0xE0000000
#define TDM_TDM_CFG_RXSMPLPTR_RSVD_31_29_SHIFT 29

/**** RXbyteptr register ****/
/* head pointer (bytes) */
#define TDM_TDM_CFG_RXBYTEPTR_HEAD_MASK  0x00001FFF
#define TDM_TDM_CFG_RXBYTEPTR_HEAD_SHIFT 0
/* tail pointer (bytes) */
#define TDM_TDM_CFG_RXBYTEPTR_TAIL_MASK  0x1FFF0000
#define TDM_TDM_CFG_RXBYTEPTR_TAIL_SHIFT 16

/**** PCM_GENStatus register ****/
/* Count value. How many PCM CLOCK cycles counted during the 8KHz frame. */
#define TDM_TDM_CFG_PCM_GENSTATUS_ACTUAL_CNT_MASK 0x000007FF
#define TDM_TDM_CFG_PCM_GENSTATUS_ACTUAL_CNT_SHIFT 0
/* Count is too low, need to speed up. */
#define TDM_TDM_CFG_PCM_GENSTATUS_SLOW   (1 << 16)
/* Count is too high, need to slow down */
#define TDM_TDM_CFG_PCM_GENSTATUS_FAST   (1 << 17)
/* zsi rev id (provision for) */
#define TDM_TDM_CFG_PCM_GENSTATUS_ZSI_REV_ID_MASK 0xF0000000
#define TDM_TDM_CFG_PCM_GENSTATUS_ZSI_REV_ID_SHIFT 28

/**** AXI_SPECIAL register ****/
/* Internal */
#define TDM_TDM_CFG_AXI_SPECIAL_CLEAR_WR_FSM (1 << 0)
/* Internal */
#define TDM_TDM_CFG_AXI_SPECIAL_CLEAR_WR_FIFO (1 << 1)
/* Internal */
#define TDM_TDM_CFG_AXI_SPECIAL_CLEAR_RD_FSM (1 << 2)
/* Internal */
#define TDM_TDM_CFG_AXI_SPECIAL_CLEAR_RD_FIFO (1 << 3)

/**** TX_INC register ****/
/* Increment head (write) value in samples */
#define TDM_TDM_CFG_TX_INC_VALUE_MASK    0x00001FFF
#define TDM_TDM_CFG_TX_INC_VALUE_SHIFT   0

#define TDM_TDM_CFG_TX_INC_RSVD_31_13_MASK 0xFFFFE000
#define TDM_TDM_CFG_TX_INC_RSVD_31_13_SHIFT 13

/**** RX_INC register ****/
/* Increment tail (read) value in samples */
#define TDM_TDM_CFG_RX_INC_VALUE_MASK    0x00001FFF
#define TDM_TDM_CFG_RX_INC_VALUE_SHIFT   0

/**** TDMClear register ****/
/* clear the tx fifo, in case of underrun. Stream can still be left active. */
#define TDM_TDM_CFG_TDMCLEAR_TX_CLEAR    (1 << 0)
/* clear the rx fifo, in case of overrun. Stream can still be left active. */
#define TDM_TDM_CFG_TDMCLEAR_RX_CLEAR    (1 << 1)

/**** TDMEnable register ****/
/*
 * Tx stream enable. Once set (simultanously with stream enable or later), the
 * serial Tx side starts submision of serial data out. If data is not available,
 * UnderRun value will be published out, and the HOST may restart the Tx fifos,
 * without shutting off the stream.
 */
#define TDM_TDM_CFG_TDMENABLE_TX_STREAM_EN (1 << 0)
/*
 * Rx stream enable. Once set (simultanously with stream enable or later), the
 * serial Rx side starts accumulating serial data into valid samples for the
 * host to fetch. If overrun condition exists, the host can restart the Rx fifo
 * without shutting off the serial stream. However, samples will be fecthed only
 * from the next start of PCM frame.
 */
#define TDM_TDM_CFG_TDMENABLE_RX_STREAM_EN (1 << 1)
/*
 * This is the stream shared enable. Once set (and the enable is active), Frame
 * sync and PCMCLK become active on the serial bus
 */
#define TDM_TDM_CFG_TDMENABLE_STREAM_ENABLE (1 << 30)
/*
 * This is the main enable. Once set, PCMCLK generation starts, fifo's can start
 * accumulating data from the host for Tx to start.
 */
#define TDM_TDM_CFG_TDMENABLE_ENABLE     (1 << 31)

#ifdef __cplusplus
}
#endif

#endif /* __AL_HAL_TDM_REGS_H__ */

/** @} end of ... group */


