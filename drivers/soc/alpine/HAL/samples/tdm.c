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
 * @defgroup group_tdm_samples Code Samples
 * @ingroup group_tdm
 * @{
 * tdm.c: this file can be found in samples directory.
 *
 * @code */
#include "al_hal_tdm.h"
#include "al_hal_reg_utils.h"
#include "al_hal_common.h"
#include "al_hal_iomap.h"

#define TX_BUFF_SIZE	8192
#define RX_BUFF_SIZE	8192

static uint8_t tx_buff[TX_BUFF_SIZE];
static uint8_t rx_buff[RX_BUFF_SIZE];

static void cs_enter_cb(void *cs_ctx __attribute__((unused)))
{
}

static void cs_leave_cb(void *cs_ctx __attribute__((unused)))
{
}

int main(void)
{
	int err;
	al_bool tdm_enabled = AL_FALSE;
	unsigned int tx_buff_idx = 0;
	unsigned int rx_buff_idx = 0;
	struct al_tdm tdm;
	struct al_tdm_tx_fifo_status tx_fifo_status;
	struct al_tdm_rx_fifo_status rx_fifo_status;
	struct al_tdm_handle_init_params handle_init_params = {
		.cfg_regs = (void __iomem *)AL_TDM_AXI_CFG_BASE,
		.tx_fifo_regs = (void __iomem *)AL_TDM_AXI_TX_FIFO_BASE,
		.rx_fifo_regs = (void __iomem *)AL_TDM_AXI_RX_FIFO_BASE,
		.cs_enter_cb = cs_enter_cb,
		.cs_leave_cb = cs_leave_cb,
	};
	struct al_tdm_cfg cfg = {
		.zsi_en = AL_FALSE,
		.num_slots_log2 = 1,	/* 2 slots */
		.slot_size = AL_TDM_SLOT_SIZE_8_BITS,
		.pcm_clk_src = AL_TDM_PCM_CLK_SRC_INTERNAL,
		.pcm_tune_dynamic = AL_TRUE,
		.pcm_tune_period = 38,
		.pcm_clk_ratio = (500000000 / 8192000),
		.pcm_tune_delta = 1,
		.pcm_tune_rnd_up = AL_FALSE,
		.pcm_tune_keep = AL_FALSE,
		.window_frame_ratio = (25000000 / 8000),
		.window_hyst_start = AL_TDM_PCM_REF_WINDOW_HYST_START_SLOWER,
		.window_hyst_level = 8,
		.fsync_edge = AL_TDM_EDGE_RISING,
		.fsync_inv = AL_FALSE,
		.fsync_len = 1,
		.tx_ctrl = {
			.auto_track = AL_FALSE,
			.en_special = AL_TDM_CHAN_EN_SPECIAL_00,
			.data_is_padded = AL_FALSE,
			.data_alignment = AL_TDM_DATA_ALIGNMENT_LSB,
			.sample_alignment = AL_TDM_SAMPLE_ALIGNMENT_END,
			.sample_size = AL_TDM_SAMPLE_SIZE_8_BITS,
			.en_edge = AL_TDM_EDGE_RISING,
			.en_inv = AL_FALSE,
			.en_len = 8,
			.en_delay = 0,
			.data_edge = AL_TDM_EDGE_RISING,
			.data_inv = AL_FALSE,
			.data_delay = 0,
			.active_slots_127_96 = 0,
			.active_slots_95_64 = 0,
			.active_slots_63_32 = AL_BIT(5), /* Slot 37 */
			.active_slots_31_0 = AL_BIT(3), /* Slot 3 */
			.fifo_empty_threshold = 32,
		},
		.tx_specific_ctrl = {
			.tx_padding_data = AL_TDM_TX_PADDING_DATA_0,
			.tx_inactive_data = AL_TDM_TX_INACTIVE_DATA_TRI_STATE,
			.tx_inactive_data_user_val = 0,
			.tx_underflow_data_user_val = 0,
		},
		.rx_ctrl = {
			.auto_track = AL_FALSE,
			.en_special = AL_TDM_CHAN_EN_SPECIAL_00,
			.data_is_padded = AL_FALSE,
			.data_alignment = AL_TDM_DATA_ALIGNMENT_LSB,
			.sample_alignment = AL_TDM_SAMPLE_ALIGNMENT_END,
			.sample_size = AL_TDM_SAMPLE_SIZE_8_BITS,
			.en_edge = AL_TDM_EDGE_RISING,
			.en_inv = AL_FALSE,
			.en_len = 8,
			.en_delay = 0,
			.data_edge = AL_TDM_EDGE_RISING,
			.data_inv = AL_FALSE,
			.data_delay = 0,
			.active_slots_127_96 = 0,
			.active_slots_95_64 = 0,
			.active_slots_63_32 = AL_BIT(5), /* Slot 37 */
			.active_slots_31_0 = AL_BIT(3), /* Slot 3 */
			.fifo_empty_threshold = 32,
		},
		.slverr_en = AL_TRUE,
		.parity_en = AL_TRUE,
	};

	/* Handle initialization */
	al_tdm_handle_init(&tdm, &handle_init_params);

	/* TDM configuration */
	err = al_tdm_config(&tdm, &cfg);
	if (err) {
		al_err("al_tdm_config failed!\n");
		return err;
	}

	 /* TX & RX loop */
	do {
		/* TX FIFO underrun handling */
		al_tdm_tx_fifo_status_get(&tdm, &tx_fifo_status);
		if (tx_fifo_status.underrun) {
			al_err("TX FIFO underrun!\n");
			al_tdm_tx_fifo_clear(&tdm);
			continue;
		}

		/* RX FIFO overrun handling */
		al_tdm_rx_fifo_status_get(&tdm, &rx_fifo_status);
		if (rx_fifo_status.overrun) {
			al_err("RX FIFO overrun!\n");
			al_tdm_rx_fifo_clear(&tdm);
			continue;
		}

		/* TX FIFO sample addition */
		if ((tx_buff_idx < TX_BUFF_SIZE) && tx_fifo_status.num_samples_vacant) {
			unsigned int num_bytes_to_send = al_min_t(
				unsigned int,
				TX_BUFF_SIZE - tx_buff_idx,
				tx_fifo_status.num_samples_vacant);
			al_tdm_tx_fifo_samples_add(
				&tdm,
				tx_buff + tx_buff_idx,
				num_bytes_to_send,
				AL_TRUE);
			tx_buff_idx += num_bytes_to_send;
		}

		/* TDM enabling once some data in TX FIFO */
		if (!tdm_enabled) {
			tdm_enabled = AL_TRUE;
			al_tdm_set_enables(
				&tdm,
				AL_TRUE, AL_TRUE, AL_TRUE, AL_TRUE);
		}

		/* RX FIFO sample fetching */
		if ((rx_buff_idx < RX_BUFF_SIZE) && rx_fifo_status.num_samples_avail) {
			unsigned int num_bytes_to_receive = al_min_t(
				unsigned int,
				RX_BUFF_SIZE - rx_buff_idx,
				rx_fifo_status.num_samples_avail);
			al_tdm_rx_fifo_samples_fetch(
				&tdm,
				rx_buff + rx_buff_idx,
				num_bytes_to_receive,
				AL_TRUE);
			rx_buff_idx += num_bytes_to_receive;
		}
	} while (rx_buff_idx < RX_BUFF_SIZE);

	return 0;
}

/** @endcode */

/** @} */
