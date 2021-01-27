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
 * @defgroup group_tdm TDM
 * @ingroup group_pbs
 *  @{
 *
 * Pulse Code Modulation (PCM) is a method used to digitally represent sampled
 * analog signals. It is the standard form of digital audio in computers,
 * compact discs, digital telephony, and other digital audio applications.
 * The PCM controller is used to interface with an external SLIC/SLAC for VoIP
 * applications. It implements a flexible Time Division Multiplexing (TDM)
 * interface with up to 128 channels and a sample width of 8, 16, 24, or 32
 * bits per channel, while using a fixed sample rate of 8 KHz, generating
 * frames of a fixed length of 125uS.
 *
 * @file   al_hal_tdm.h
 * @brief Header file for the TDM HAL driver
 *
 */

#ifndef _AL_HAL_TDM_H_
#define _AL_HAL_TDM_H_

#include "al_hal_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * Data structures
 ******************************************************************************/
#define AL_TDM_FIFO_SIZE		4096	/* Bytes */

/**
 * TDM handle
 * The fields in this structure are used internally and should not be accessed
 * externally
 */
struct al_tdm {
	struct al_tdm_tdm_cfg __iomem	*cfg_regs;
	void __iomem			*tx_fifo_regs;
	void __iomem			*rx_fifo_regs;

	unsigned int			tx_num_slots_active;
	unsigned int			tx_sample_num_bytes;
	unsigned int			tx_fifo_head;
	unsigned int			tx_fifo_head_pending_samples;
	al_bool				tx_fifo_auto_track;

	unsigned int			rx_num_slots_active;
	unsigned int			rx_sample_num_bytes;
	unsigned int			rx_fifo_tail;
	unsigned int			rx_fifo_tail_pending_samples;
	al_bool				rx_fifo_auto_track;

	uint8_t				__cache_aligned sw_tx_fifo_0[AL_TDM_FIFO_SIZE];
	uint8_t				__cache_aligned sw_tx_fifo_1[AL_TDM_FIFO_SIZE];
	uint8_t				__cache_aligned sw_rx_fifo[AL_TDM_FIFO_SIZE];
	unsigned int			sw_rx_fifo_head;
	unsigned int			sw_rx_fifo_tail;
	unsigned int			sw_rx_fifo_pending;

	void				(*cs_enter_cb)(void *cs_ctx);
	void				(*cs_leave_cb)(void *cs_ctx);
	void				*cs_ctx;
};

/** Slot size */
enum al_tdm_slot_size {
	AL_TDM_SLOT_SIZE_8_BITS = 0,
	AL_TDM_SLOT_SIZE_16_BITS = 1,
	AL_TDM_SLOT_SIZE_24_BITS = 2,
	AL_TDM_SLOT_SIZE_32_BITS = 3,
};

/** PCM clock source */
enum al_tdm_pcm_clk_src {
	AL_TDM_PCM_CLK_SRC_EXTERNAL = 0,
	AL_TDM_PCM_CLK_SRC_INTERNAL = 1,
};

/** Channel enable encoding of the 3rd channel and on (00 or 11) */
enum al_tdm_chan_en_special {
	AL_TDM_CHAN_EN_SPECIAL_00 = 0,
	AL_TDM_CHAN_EN_SPECIAL_11 = 1,
};

/** Host data alignment within a word (if padded) */
enum al_tdm_data_alignment {
	AL_TDM_DATA_ALIGNMENT_LSB = 0,
	AL_TDM_DATA_ALIGNMENT_MSB = 1,
};

/** Padding data */
enum al_tdm_tx_padding_data {
	AL_TDM_TX_PADDING_DATA_0,
	AL_TDM_TX_PADDING_DATA_1,
};

/** Sample alignment (Start of slot or end of slot) */
enum al_tdm_sample_alignment {
	/** End of slot */
	AL_TDM_SAMPLE_ALIGNMENT_END = 0,
	/** Start of slot */
	AL_TDM_SAMPLE_ALIGNMENT_START = 1,
};

/** Sample size */
enum al_tdm_sample_size {
	AL_TDM_SAMPLE_SIZE_8_BITS = 0,
	AL_TDM_SAMPLE_SIZE_16_BITS = 1,
	AL_TDM_SAMPLE_SIZE_24_BITS = 2,
	AL_TDM_SAMPLE_SIZE_32_BITS = 3,
};

/** Edge */
enum al_tdm_edge {
	AL_TDM_EDGE_FALLING = 0,
	AL_TDM_EDGE_RISING = 1,
};

/** Inactive data format */
enum al_tdm_tx_inactive_data {
	AL_TDM_TX_INACTIVE_DATA_USER,
	AL_TDM_TX_INACTIVE_DATA_TRI_STATE,
};

/** TX/RX control */
struct al_tdm_cfg_xmit_ctrl {
	/*
	 * Channel pointers automatic management. When set - data pointers
	 * increment automatically once data beat is sent/received.
	 * If not set, 'head_inc' / 'tail_inc' must be set when calling
	 * 'al_tdm_tx_fifo_samples_add' and 'al_tdm_rx_fifo_samples_fetch'.
	 */
	al_bool	auto_track;

	/* Channel enable encoding of the 3rd channel and on */
	enum al_tdm_chan_en_special en_special;

	/*
	 * Host data format - padded to word size, or consecutive sample bytes
	 * stream
	 */
	al_bool data_is_padded;

	/* Host data alignment within a word (if padded) */
	enum al_tdm_data_alignment data_alignment;

	/* Sample alignment */
	enum al_tdm_sample_alignment sample_alignment;

	/* Sample size */
	enum al_tdm_sample_size sample_size;

	/* Data Channel Enable edge alignment */
	enum al_tdm_edge en_edge;

	/* Data Channel Enable polarity inverted */
	al_bool en_inv;

	/* Data Channel Enable duration */
	unsigned int en_len;

	/* Data Channel Enable Delay 0-7 */
	unsigned int en_delay;

	/* Data edge alignment */
	enum al_tdm_edge data_edge;

	/* Data polarity inverted */
	al_bool data_inv;

	/* Data Delay 0-7 */
	unsigned int data_delay;

	/* Active slots 127 - 96 */
	uint32_t active_slots_127_96;

	/* Active slots 95 - 64 */
	uint32_t active_slots_95_64;

	/* Active slots 63 - 32 */
	uint32_t active_slots_63_32;

	/* Active slots 31 - 0 */
	uint32_t active_slots_31_0;

	/*
	 * FIFO level (in sample count) in which an interrupt is generated:
	 * - TX - The FIFO is almost empty
	 * - RX - There is enough data piled up to be fetched
	 */
	unsigned int fifo_empty_threshold;
};

/** TX specific control */
struct al_tdm_cfg_tx_specific_ctrl {
	/* Tx data during inactive slot cycles of an active channel (padding) */
	enum al_tdm_tx_padding_data tx_padding_data;

	/*
	 * Tx data during inactive slots - user value or tristate
	 * If user value, set 'tx_inactive_data_user_val' accordingly
	 */
	enum al_tdm_tx_inactive_data tx_inactive_data;

	/*
	 * Tx data during inactive slots
	 * Relevant only when 'tx_inactive_data' is
	 * 'AL_TDM_TX_INACTIVE_DATA_USER'
	 */
	uint32_t tx_inactive_data_user_val;

	/* Tx data during underflow conditions */
	uint32_t tx_underflow_data_user_val;
};

/*
 * In order to save convergence period, start already at first
 * iteration with a tuning value as expected to be used later on.
 */
enum al_tdm_pcm_ref_window_hyst_start {
	/* start from the exact configuration */
	AL_TDM_PCM_REF_WINDOW_HYST_START_EXACT = 0,
	/* start from base+1 (slower by one integer) */
	AL_TDM_PCM_REF_WINDOW_HYST_START_SLOWER = 1,
	/* start from base-1 (faster by one integer) */
	AL_TDM_PCM_REF_WINDOW_HYST_START_FASTER = 2,
};

/** TDM handle initialization params */
struct al_tdm_handle_init_params {
	/** TDM configuration registers base address */
	void __iomem	*cfg_regs;
	/** TDM TX FIFO registers base address */
	void __iomem	*tx_fifo_regs;
	/** TDM RX FIFO registers base address */
	void __iomem	*rx_fifo_regs;

	/** Critical section enter callback function */
	void		(*cs_enter_cb)(void *cs_ctx);
	/** Critical section leave callback function */
	void		(*cs_leave_cb)(void *cs_ctx);
	/** Critical section context */
	void		*cs_ctx;
};

/** TDM configuration */
struct al_tdm_cfg {
	/* Slot configuration */

	/* zsi protocol mixer (provision) */
	al_bool					zsi_en;
	/* Number of slots. Up to 128. [log2] */
	unsigned int				num_slots_log2;
	/* Slot size */
	enum al_tdm_slot_size			slot_size;

	/* PCM clock configuration */

	enum al_tdm_pcm_clk_src			pcm_clk_src;
	/*
	 * Using an internal source may require a dynamic (adaptive) tuning of
	 * the PCM clock
	 */
	al_bool					pcm_tune_dynamic;
	/*
	 * In case a deviation is detected, how many pcmclk cycles shall be
	 * using the corrective value
	 */
	unsigned int				pcm_tune_period;
	/* Divide ratio of PCM CLK from it source */
	unsigned int				pcm_clk_ratio;
	/*
	 * In case a deviation is detected, how many pcmclk cycles (extra or
	 * less) shall be using the corrective value vs. the base period (to
	 * speed up convergence)
	 */
	unsigned int				pcm_tune_delta;
	/* Divide ration is rounded down (0) or up (1) as base ratio */
	al_bool					pcm_tune_rnd_up;
	/*
	 * When correcting a deviation, does it keep the deviated value till
	 * the opposite deviation is detected, OR does it return to default
	 * after one tuning attempt (Default is returning to base value)
	 */
	al_bool					pcm_tune_keep;

	/* PCM 8KHz reference window */

	/* 8KHz generation of a 25MHz reference clock */
	unsigned int				window_frame_ratio;
	/*
	 * In order to save convergence period, start already at first
	 * iteration with a tuning value as expected to be used later on. 00-
	 * start from the exact configuration. 01 - start from base+1 (slower
	 * by one integer), 10 - start from base-1 (faster by one integer)
	 */
	enum al_tdm_pcm_ref_window_hyst_start	window_hyst_start;
	/*
	 * Using internal clock as reference may require rational division,
	 * hence exact match will never happen, but as close as possible to
	 * averaging the target. The hystheresis window is the count deviation
	 * for the exact match for which tuning will be activated
	 */
	unsigned int				window_hyst_level;

	/* Frame Sync control */

	/* Frame sync edge alignment */
	enum al_tdm_edge			fsync_edge;
	/* Frame sync polarity */
	al_bool					fsync_inv;
	/* Length period of frame sync */
	unsigned int				fsync_len;

	/* TX Control */

	struct al_tdm_cfg_xmit_ctrl		tx_ctrl;
	struct al_tdm_cfg_tx_specific_ctrl	tx_specific_ctrl;

	/* RX Control */

	struct al_tdm_cfg_xmit_ctrl		rx_ctrl;

	/* Misc control */

	/*
	 * Enable parity error propagation through AXI/APB reads.
	 * Requires that parity will be enabled
	 */
	al_bool					slverr_en;

	/* Enable parity calculation of the FIFOs read */
	al_bool					parity_en;
};

/** TX FIFO status */
struct al_tdm_tx_fifo_status {
	al_bool		underrun;
	al_bool		almost_empty;
	unsigned int	num_samples_vacant;
};

/** RX FIFO status */
struct al_tdm_rx_fifo_status {
	al_bool		overrun;
	al_bool		enough_avail;
	unsigned int	num_samples_avail;
};

/******************************************************************************
 * API Functions
 ******************************************************************************/

/**
 * Initialize a TDM handle
 *
 * @param	tdm
 *		TDM instance
 * @param	params
 *		Handle initialization params
 */
void al_tdm_handle_init(
	struct al_tdm				*tdm,
	const struct al_tdm_handle_init_params	*params);

/**
 * TDM configuration
 *
 * @param	tdm
 *		TDM instance
 * @param	cfg
 *		TDM required configuration
 *
 * @returns	0 if no failure
 */
int al_tdm_config(
	struct al_tdm		*tdm,
	struct al_tdm_cfg	*cfg);

/**
 * TDM enabling control

 *
 * @param	tdm
 *		TDM instance
 * @param	enable
 *		This is the main enable. Once set, PCMCLK generation starts,
 *		fifo's can start accumulating data from the host for Tx to
 *		start.
 * @param	stream_enable
 *		This is the stream shared enable. Once set (and the enable is
 *		active), Frame sync and PCMCLK become active on the serial bus
 * @param	rx_stream_enable
 *		Rx stream enable. Once set (simultanously with stream enable or
 *		later), the serial Rx side starts accumulating serial data into
 *		valid samples for the host to fetch. If overrun condition
 *		exists, the host can restart the Rx fifo without shutting off
 *		the serial stream. However, samples will be fecthed only from
 *		the next start of PCM frame.
 * @param	tx_stream_enable
 *		Tx stream enable. Once set (simultanously with stream enable or
 *		later), the serial Tx side starts submision of serial data out.
 *		If data is not available, UnderRun value will be published out,
 *		and the HOST may restart the Tx fifos, without shutting off the
 *		stream.
 */
void al_tdm_set_enables(
	struct al_tdm	*tdm,
	al_bool		enable,
	al_bool		stream_enable,
	al_bool		rx_stream_enable,
	al_bool		tx_stream_enable);

/**
 * TDM Tx enabling control
 *
 * @param	tdm
 *		TDM instance
 * @param	tx_stream_enable
 *		Tx stream enable. Once set (simultanously with stream enable or later), the serial
 *		Tx side starts submision of serial data out.
 *		If data is not available, UnderRun value will be published out, and the HOST may
 *		restart the Tx fifos, without shutting off the stream.
 */
void al_tdm_tx_enable_set(
	struct al_tdm	*tdm,
	al_bool		tx_stream_enable);

/**
 * TDM Rx enabling control
 *
 * @param	tdm
 *		TDM instance
 * @param	rx_stream_enable
 *		Rx stream enable. Once set (simultanously with stream enable or later), the serial
 *		Rx side starts accumulating serial data into valid samples for the host to fetch.
 *		If overrun condition exists, the host can restart the Rx fifo without shutting off
 *		the serial stream. However, samples will be fecthed only from the next start of PCM
 *		frame.
 */
void al_tdm_rx_enable_set(
	struct al_tdm	*tdm,
	al_bool		rx_stream_enable);
/**
 * TDM TX FIFO status get
 *
 * @param	tdm
 *		TDM instance
 * @param	status
 *		TX FIFO status
 */
void al_tdm_tx_fifo_status_get(
	struct al_tdm			*tdm,
	struct al_tdm_tx_fifo_status	*status);

/**
 * TDM inflight TX FIFO clear (in case of underrun)
 * Stream can still be left active
 *
 * @param	tdm
 *		TDM instance
 */
void al_tdm_tx_fifo_clear(
	struct al_tdm *tdm);

/**
 * TDM RX FIFO status get
 *
 * @param	tdm
 *		TDM instance
 * @param	status
 *		RX FIFO status
 */
void al_tdm_rx_fifo_status_get(
	struct al_tdm			*tdm,
	struct al_tdm_rx_fifo_status	*status);

/**
 * TDM inflight RX FIFO clear (in case of overrun)
 * Stream can still be left active
 *
 * @param	tdm
 *		TDM instance
 */
void al_tdm_rx_fifo_clear(
	struct al_tdm *tdm);

/**
 * TDM add samples to the TX FIFO with optional update of the head pointer
 *
 * User should first make sure there's enough vacant samples using
 * 'al_tdm_tx_fifo_status_get' and checking the 'num_samples_vacant' field.
 *
 * @param	tdm
 *		TDM instance
 * @param	sample_data
 *		Sample data to add, including alignment and padding
 * @param	num_samples
 *		Number of samples to add
 * @param	head_inc
 *		An indication whether or not to update the head pointer and thus
 *		enable the HW to actually transmit the data
 */
void al_tdm_tx_fifo_samples_add(
	struct al_tdm	*tdm,
	void		*sample_data,
	unsigned int	num_samples,
	al_bool		head_inc);

/**
 * TDM fetch samples from the RX FIFO with optional update of the tail pointer
 *
 * User should first make sure there's enough available samples using
 * 'al_tdm_rx_fifo_status_get' and checking the 'num_samples_avail' field.
 *
 * @param	tdm
 *		TDM instance
 * @param	sample_data
 *		A buffer to hold the fetched samples, including alignment and
 *		padding
 * @param	num_samples
 *		Number of samples to fetch
 * @param	tail_inc
 *		An indication whether or not to update the tail pointer and thus
 *		enable the HW to overrun these samples with newly received
 *		samples
 */
void al_tdm_rx_fifo_samples_fetch(
	struct al_tdm	*tdm,
	void		*sample_data,
	unsigned int	num_samples,
	al_bool		tail_inc);

/**
 * TDM status print
 *
 * @param	tdm
 *		TDM instance
 */
void al_tdm_status_print(
	struct al_tdm	*tdm);

#ifdef __cplusplus
}
#endif

#endif

/** @} end of TDM group */
