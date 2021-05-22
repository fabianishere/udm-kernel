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
 * @defgroup group_tdm	TDM
 *  @{
 *
 * @file   al_hal_tdm.h
 * @brief Header file for the TDM HAL driver
 *
 */

#include "al_hal_common.h"
#include "al_hal_tdm_regs.h"
#include "al_hal_tdm.h"
#include "al_hal_reg_utils.h"

#define FIFO_SIZE			AL_TDM_FIFO_SIZE
#define CYCLE_CONSUMPTION_TIMEOUT	1000	/* [uSec] */
#define CYCLE_PRODUCTION_TIMEOUT	1000	/* [uSec] */
#define TX_FIFO_WA_SIZE			(FIFO_SIZE - 128)
#define RX_FIFO_WA_THRESHOLD		2048	/* [Bytes] */

/******************************************************************************
 ******************************************************************************/
struct al_tdm_fifo_ptr_info {
	unsigned int byte_cnt_tail;
	unsigned int byte_cnt_head;
	unsigned int sample_cnt_tail;
	unsigned int frame_cnt_tail;
	unsigned int sample_cnt_head;
	unsigned int frame_cnt_head;
};

/******************************************************************************
 ******************************************************************************/
void al_tdm_handle_init(
	struct al_tdm				*tdm,
	const struct al_tdm_handle_init_params	*params)
{
	al_assert(tdm);
	al_assert(params);
	al_assert(params->cfg_regs);
	al_assert(params->tx_fifo_regs);
	al_assert(params->rx_fifo_regs);
	al_assert(params->cs_enter_cb);
	al_assert(params->cs_leave_cb);

	al_dbg(
		"%s(%p, %p, %p, %p)\n", __func__,
		tdm, params->cfg_regs, params->tx_fifo_regs, params->rx_fifo_regs);

	tdm->cfg_regs = (struct al_tdm_tdm_cfg *)params->cfg_regs;
	tdm->tx_fifo_regs = params->tx_fifo_regs;
	tdm->rx_fifo_regs = params->rx_fifo_regs;
	tdm->cs_enter_cb = params->cs_enter_cb;
	tdm->cs_leave_cb = params->cs_leave_cb;
	tdm->cs_ctx = params->cs_ctx;

	tdm->tx_fifo_head = 0;
	tdm->tx_fifo_head_pending_samples = 0;
	tdm->rx_fifo_tail = 0;
	tdm->rx_fifo_tail_pending_samples = 0;
	tdm->sw_rx_fifo_head = 0;
	tdm->sw_rx_fifo_tail = 0;
	tdm->sw_rx_fifo_pending = 0;
}

/******************************************************************************
 ******************************************************************************/
static int al_tdm_config_xmit_ctrl(
	struct al_tdm_cfg_xmit_ctrl	*cfg,
	uint32_t			*ctl_reg,
	uint32_t			*sa0_reg,
	uint32_t			*sa1_reg,
	uint32_t			*sa2_reg,
	uint32_t			*sa3_reg,
	uint32_t			*limits_reg,
	unsigned int			*sample_num_bytes,
	unsigned int			*num_slots_active)
{
	if (cfg->data_is_padded) {
		*sample_num_bytes = 4;
	} else {
		switch (cfg->sample_size) {
		case AL_TDM_SAMPLE_SIZE_8_BITS:
			*sample_num_bytes = 1;
			break;
		case AL_TDM_SAMPLE_SIZE_16_BITS:
			*sample_num_bytes = 2;
			break;
		case AL_TDM_SAMPLE_SIZE_24_BITS:
			*sample_num_bytes = 3;
			break;
		case AL_TDM_SAMPLE_SIZE_32_BITS:
			*sample_num_bytes = 4;
			break;
		default:
			al_err("%s: invalid sample size!\n", __func__);
			return -EINVAL;
		}
	}

	*num_slots_active = al_popcount(cfg->active_slots_31_0) +
		al_popcount(cfg->active_slots_63_32) +
		al_popcount(cfg->active_slots_95_64) +
		al_popcount(cfg->active_slots_127_96);

	al_assert(cfg->en_len > 0);

	al_reg_write32(
		ctl_reg,
		(cfg->data_delay << TDM_TDM_CFG_TX_CTL_TXD_DELAY_SHIFT) |
		((cfg->data_inv) ? TDM_TDM_CFG_TX_CTL_TXD_INV : 0) |
		((cfg->data_edge == AL_TDM_EDGE_RISING) ? TDM_TDM_CFG_TX_CTL_TXD_EDGE : 0) |
		(cfg->en_delay << TDM_TDM_CFG_TX_CTL_TX_EN_DELAY_SHIFT) |
		((cfg->en_len - 1) << TDM_TDM_CFG_TX_CTL_TX_EN_LEN_SHIFT) |
		((cfg->en_inv) ? TDM_TDM_CFG_TX_CTL_TX_EN_INV : 0) |
		((cfg->en_edge == AL_TDM_EDGE_RISING) ? TDM_TDM_CFG_TX_CTL_TX_EN_EDGE : 0) |
		(cfg->sample_size << TDM_TDM_CFG_TX_CTL_TX_SMPL_SIZE_SHIFT) |
		((cfg->sample_alignment == AL_TDM_SAMPLE_ALIGNMENT_START) ?
		 TDM_TDM_CFG_TX_CTL_TX_SMPL_ALIGN : 0) |
		((cfg->data_is_padded) ? TDM_TDM_CFG_TX_CTL_TX_HOST_PADDING : 0) |
		((cfg->data_alignment == AL_TDM_DATA_ALIGNMENT_MSB) ?
		 TDM_TDM_CFG_TX_CTL_TX_HOST_ALIGNMENT : 0) |
		((cfg->en_special == AL_TDM_CHAN_EN_SPECIAL_11) ?
		 TDM_TDM_CFG_TX_CTL_TX_EN_SPECIAL : 0) |
		((cfg->auto_track) ? TDM_TDM_CFG_TX_CTL_TX_AUTO_TRACK : 0));
	al_reg_write32(
		sa0_reg,
		cfg->active_slots_31_0);
	al_reg_write32(
		sa1_reg,
		cfg->active_slots_63_32);
	al_reg_write32(
		sa2_reg,
		cfg->active_slots_95_64);
	al_reg_write32(
		sa3_reg,
		cfg->active_slots_127_96);
	al_reg_write32(
		limits_reg,
		((FIFO_SIZE / (*sample_num_bytes)) << TDM_TDM_CFG_TX_LIMITS_MAX_SHIFT) |
		(cfg->fifo_empty_threshold << TDM_TDM_CFG_TX_LIMITS_ALMOST_EMPTY_SHIFT));

	return 0;
}

/******************************************************************************
 ******************************************************************************/
static void tdm_memcpy_to_hw(uint8_t *tgt, uint8_t *src, unsigned int size)
{
	for (; size && (((uintptr_t)tgt & 7) || ((uintptr_t)src & 7)); size--, tgt++, src++)
		al_reg_write8(tgt, *src);
	for (; (size >= 8) && (!((uintptr_t)tgt & 7)) && (!((uintptr_t)src & 7));
			size -= 8, tgt  += 8, src += 8)
		al_reg_write64((uint64_t *)tgt, *(uint64_t *)src);
	for (; size && (((uintptr_t)tgt & 3) || ((uintptr_t)src & 3)); size--, tgt++, src++)
		al_reg_write8(tgt, *src);
	for (; (size >= 4) && (!((uintptr_t)tgt & 3)) && (!((uintptr_t)src & 3));
			size -= 4, tgt += 4, src += 4)
		al_reg_write32((uint32_t *)tgt, *(uint32_t *)src);
	for (; size; size--, tgt++, src++)
		al_reg_write8(tgt, *src);
}

/******************************************************************************
 ******************************************************************************/
static void tdm_memcpy_from_hw(uint8_t *tgt, uint8_t *src, unsigned int size)
{
	for (; size && (((uintptr_t)tgt & 7) || ((uintptr_t)src & 7)); size--, tgt++, src++)
		*tgt = al_reg_read8(src);
	for (; (size >= 8) && (!((uintptr_t)tgt & 7)) && (!((uintptr_t)src & 7));
			size -= 8, tgt += 8, src += 8)
		*(uint64_t *)tgt = al_reg_read64((uint64_t *)src);
	for (; size && (((uintptr_t)tgt & 3) || ((uintptr_t)src & 3)); size--, tgt++, src++)
		*tgt = al_reg_read8(src);
	for (; (size >= 4) && (!((uintptr_t)tgt & 3)) && (!((uintptr_t)src & 3));
			size -= 4, tgt += 4, src += 4)
		*(uint32_t *)tgt = al_reg_read32((uint32_t *)src);
	for (; size; size--, tgt++, src++)
		*tgt = al_reg_read8(src);
}

/******************************************************************************
 ******************************************************************************/
static void tdm_tx_fifo_add(struct al_tdm *tdm, uint8_t *buff, unsigned int num_bytes)
{
	unsigned int num_bytes_till_wrap_around = FIFO_SIZE - tdm->tx_fifo_head;
	unsigned int num_bytes_current = (num_bytes <= num_bytes_till_wrap_around) ?
		num_bytes : num_bytes_till_wrap_around;

	al_assert(num_bytes <= FIFO_SIZE);

	tdm_memcpy_to_hw((uint8_t *)tdm->tx_fifo_regs + tdm->tx_fifo_head, buff, num_bytes_current);
	tdm->tx_fifo_head = (tdm->tx_fifo_head + num_bytes_current) % FIFO_SIZE;
	num_bytes -= num_bytes_current;

	if (num_bytes) {
		tdm_memcpy_to_hw(tdm->tx_fifo_regs, buff + num_bytes_current, num_bytes);
		tdm->tx_fifo_head = num_bytes;
	}
}

/******************************************************************************
 ******************************************************************************/
static void tdm_rx_fifo_fetch(struct al_tdm *tdm, uint8_t *buff, unsigned int num_bytes)
{
	unsigned int num_bytes_till_wrap_around = FIFO_SIZE - tdm->rx_fifo_tail;
	unsigned int num_bytes_current = (num_bytes <= num_bytes_till_wrap_around) ?
		num_bytes : num_bytes_till_wrap_around;

	al_assert(num_bytes <= FIFO_SIZE);

	tdm_memcpy_from_hw(buff, (uint8_t *)tdm->rx_fifo_regs + tdm->rx_fifo_tail,
		num_bytes_current);
	tdm->rx_fifo_tail = (tdm->rx_fifo_tail + num_bytes_current) % FIFO_SIZE;
	num_bytes -= num_bytes_current;

	if (num_bytes) {
		tdm_memcpy_from_hw(buff + num_bytes_current, tdm->rx_fifo_regs, num_bytes);
		tdm->rx_fifo_tail = num_bytes;
	}
}

/******************************************************************************
 ******************************************************************************/
static void al_tdm_tx_fifo_ptr_info_get(
	struct al_tdm			*tdm,
	struct al_tdm_fifo_ptr_info	*info)
{
	unsigned int num_slots = tdm->tx_num_slots_active;
	unsigned int byte_cnt_tail = AL_REG_FIELD_GET(al_reg_read32(&tdm->cfg_regs->txbyteptr),
		TDM_TDM_CFG_TXBYTEPTR_TAIL_MASK, TDM_TDM_CFG_TXBYTEPTR_TAIL_SHIFT);
	unsigned int byte_cnt_head = AL_REG_FIELD_GET(al_reg_read32(&tdm->cfg_regs->txbyteptr),
		TDM_TDM_CFG_TXBYTEPTR_HEAD_MASK, TDM_TDM_CFG_TXBYTEPTR_HEAD_SHIFT);
	unsigned int sample_cnt_tail = AL_REG_FIELD_GET(al_reg_read32(&tdm->cfg_regs->txsmplptr),
		TDM_TDM_CFG_TXSMPLPTR_TAIL_MASK, TDM_TDM_CFG_TXSMPLPTR_TAIL_SHIFT);
	unsigned int frame_cnt_tail = sample_cnt_tail / num_slots;
	unsigned int sample_cnt_head = AL_REG_FIELD_GET(al_reg_read32(&tdm->cfg_regs->txsmplptr),
		TDM_TDM_CFG_TXSMPLPTR_HEAD_MASK, TDM_TDM_CFG_TXSMPLPTR_HEAD_SHIFT);
	unsigned int frame_cnt_head = sample_cnt_head / num_slots;

	info->byte_cnt_tail = byte_cnt_tail;
	info->byte_cnt_head = byte_cnt_head;
	info->sample_cnt_tail = sample_cnt_tail;
	info->frame_cnt_tail = frame_cnt_tail;
	info->sample_cnt_head = sample_cnt_head;
	info->frame_cnt_head = frame_cnt_head;
}

/******************************************************************************
 ******************************************************************************/
static void al_tdm_rx_fifo_ptr_info_get(
	struct al_tdm			*tdm,
	struct al_tdm_fifo_ptr_info	*info)
{
	unsigned int num_slots = tdm->rx_num_slots_active;
	unsigned int byte_cnt_tail = AL_REG_FIELD_GET(al_reg_read32(&tdm->cfg_regs->rxbyteptr),
		TDM_TDM_CFG_RXBYTEPTR_TAIL_MASK, TDM_TDM_CFG_RXBYTEPTR_TAIL_SHIFT);
	unsigned int byte_cnt_head = AL_REG_FIELD_GET(al_reg_read32(&tdm->cfg_regs->rxbyteptr),
		TDM_TDM_CFG_RXBYTEPTR_HEAD_MASK, TDM_TDM_CFG_RXBYTEPTR_HEAD_SHIFT);
	unsigned int sample_cnt_tail = AL_REG_FIELD_GET(al_reg_read32(&tdm->cfg_regs->rxsmplptr),
		TDM_TDM_CFG_RXSMPLPTR_TAIL_MASK, TDM_TDM_CFG_RXSMPLPTR_TAIL_SHIFT);
	unsigned int frame_cnt_tail = sample_cnt_tail / num_slots;
	unsigned int sample_cnt_head = AL_REG_FIELD_GET(al_reg_read32(&tdm->cfg_regs->rxsmplptr),
		TDM_TDM_CFG_RXSMPLPTR_HEAD_MASK, TDM_TDM_CFG_RXSMPLPTR_HEAD_SHIFT);
	unsigned int frame_cnt_head = sample_cnt_head / num_slots;

	info->byte_cnt_tail = byte_cnt_tail;
	info->byte_cnt_head = byte_cnt_head;
	info->sample_cnt_tail = sample_cnt_tail;
	info->frame_cnt_tail = frame_cnt_tail;
	info->sample_cnt_head = sample_cnt_head;
	info->frame_cnt_head = frame_cnt_head;
}

/******************************************************************************
 ******************************************************************************/
static int al_tdm_tx_poll(
	struct al_tdm	*tdm,
	unsigned int	addition_size)
{
	unsigned int time_left;
	unsigned int frame_cnt_tail_first;
	al_bool frame_cnt_tail_first_valid = AL_FALSE;
	struct al_tdm_fifo_ptr_info ptr_info;
	struct al_tdm_tx_fifo_status	tx_fifo_status;
	unsigned int head_orig;
	unsigned int pending_orig;

	al_dbg("%s(%p)\n", __func__, tdm);

	al_assert(tdm);

	/* Quit if enough frames are pending */
	al_tdm_tx_fifo_ptr_info_get(tdm, &ptr_info);
	if (((ptr_info.sample_cnt_head * tdm->tx_sample_num_bytes) + addition_size)
			< (FIFO_SIZE / 4)) {
		al_dbg("%s: No action required, enough room\n", __func__);
		return 0;
	}

	/* Start of critical section */
	tdm->cs_enter_cb(tdm->cs_ctx);

	/* Busy wait for sample to be consumed */
	al_dbg("%s: Waiting for frame to be consumed...\n", __func__);

	for (time_left = CYCLE_CONSUMPTION_TIMEOUT; time_left; time_left--) {
		al_tdm_tx_fifo_ptr_info_get(tdm, &ptr_info);

		if (!frame_cnt_tail_first_valid) {
			frame_cnt_tail_first = ptr_info.frame_cnt_tail;
			frame_cnt_tail_first_valid = AL_TRUE;
		}

		if (ptr_info.frame_cnt_tail > frame_cnt_tail_first)
			break;

		al_udelay(1);
	}

	if (!time_left) {
		al_err("%s: Timed out waiting for frame consumtion\n", __func__);
		tdm->cs_leave_cb(tdm->cs_ctx);
		return -ETIMEDOUT;
	}

	/* Stop the FIFO and current tail */
	al_tdm_tx_enable_set(tdm, AL_FALSE);
	al_udelay(1);
	al_tdm_tx_fifo_ptr_info_get(tdm, &ptr_info);

	/* Copy samples which are currently pending in FIFO to SW FIFO 1 */
	head_orig = tdm->tx_fifo_head;
	pending_orig = (head_orig - (ptr_info.sample_cnt_tail * tdm->tx_sample_num_bytes));
	al_memcpy(tdm->sw_tx_fifo_1,
		tdm->sw_tx_fifo_0 + (tdm->tx_sample_num_bytes * ptr_info.sample_cnt_tail),
		pending_orig);
	al_memcpy(tdm->sw_tx_fifo_0, tdm->sw_tx_fifo_1, pending_orig);

	/* Reset the HW FIFO */
	al_tdm_tx_fifo_clear(tdm);

	/* Copy from SW FIFO 0 to HW FIFO according to its vacancy */
	tdm_tx_fifo_add(tdm, tdm->sw_tx_fifo_0, pending_orig);
	tdm->tx_fifo_head = pending_orig;
	al_tdm_tx_enable_set(tdm, AL_TRUE);

	/* End of critical section */
	tdm->cs_leave_cb(tdm->cs_ctx);

	al_tdm_tx_fifo_status_get(tdm, &tx_fifo_status);
	al_assert(!tx_fifo_status.underrun);

	al_assert(tdm->tx_fifo_auto_track);

	return 0;
}

/******************************************************************************
 ******************************************************************************/
static void tdm_rx_fifo_fetch_to_sw_ring(struct al_tdm *tdm, unsigned int num_bytes)
{
	unsigned int tail = tdm->rx_fifo_tail;
	unsigned int num_bytes_till_wrap_around = FIFO_SIZE - tdm->sw_rx_fifo_head;
	unsigned int num_bytes_current = (num_bytes < num_bytes_till_wrap_around) ?
		num_bytes : num_bytes_till_wrap_around;

	al_assert((num_bytes + tdm->sw_rx_fifo_pending) < FIFO_SIZE);

	tdm_memcpy_from_hw(tdm->sw_rx_fifo + tdm->sw_rx_fifo_head,
		(uint8_t *)tdm->rx_fifo_regs + tail, num_bytes_current);
	tdm->sw_rx_fifo_head = (tdm->sw_rx_fifo_head + num_bytes_current) % FIFO_SIZE;
	tdm->sw_rx_fifo_pending += num_bytes_current;
	num_bytes -= num_bytes_current;

	if (num_bytes) {
		tdm_memcpy_from_hw(tdm->sw_rx_fifo + tdm->sw_rx_fifo_head,
			(uint8_t *)tdm->rx_fifo_regs + tail + num_bytes_current, num_bytes);
		tdm->sw_rx_fifo_head = num_bytes;
		tdm->sw_rx_fifo_pending += num_bytes;
	}
}

/******************************************************************************
 ******************************************************************************/
static int al_tdm_rx_poll(
	struct al_tdm	*tdm)
{
	unsigned int time_left;
	unsigned int frame_cnt_head_first;
	al_bool frame_cnt_head_first_valid = AL_FALSE;
	struct al_tdm_fifo_ptr_info ptr_info;

	al_dbg("%s(%p)\n", __func__, tdm);

	/* Quit if enough frames are vacant */
	al_tdm_rx_fifo_ptr_info_get(tdm, &ptr_info);
	if (((ptr_info.sample_cnt_head * tdm->rx_sample_num_bytes) + RX_FIFO_WA_THRESHOLD) <
			FIFO_SIZE) {
		al_dbg("%s: No action required, enough room\n", __func__);
		return 0;
	}

	/* Start of critical section */
	tdm->cs_enter_cb(tdm->cs_ctx);

	/* Busy wait for sample to be produced */
	al_dbg("%s: Waiting for frame to be produced...\n", __func__);

	for (time_left = CYCLE_PRODUCTION_TIMEOUT; time_left; time_left--) {
		al_tdm_rx_fifo_ptr_info_get(tdm, &ptr_info);

		if (!frame_cnt_head_first_valid) {
			frame_cnt_head_first = ptr_info.frame_cnt_head;
			frame_cnt_head_first_valid = AL_TRUE;
		}

		if (ptr_info.frame_cnt_head > frame_cnt_head_first)
			break;

		al_udelay(1);
	}

	if (!time_left) {
		tdm->cs_leave_cb(tdm->cs_ctx);
		al_err("%s: Timed out waiting for frame production\n", __func__);
		return -ETIMEDOUT;
	}

	/* Copy samples which are currently pending in FIFO to SW FIFO 1 */
	tdm_rx_fifo_fetch_to_sw_ring(
		tdm, (ptr_info.sample_cnt_head * tdm->rx_sample_num_bytes) - tdm->rx_fifo_tail);

	/* Reset the HW FIFO */
	al_tdm_rx_fifo_clear(tdm);

	/* End of critical section */
	tdm->cs_leave_cb(tdm->cs_ctx);

	al_assert(tdm->rx_fifo_auto_track);

	return 0;
}

/******************************************************************************
 ******************************************************************************/
int al_tdm_config(
	struct al_tdm		*tdm,
	struct al_tdm_cfg	*cfg)
{
	int err;

	al_dbg("%s(%p, %p)\n", __func__, tdm, cfg);

	al_assert(tdm);
	al_assert(cfg);

	al_reg_write32(
		&tdm->cfg_regs->slot,
		(cfg->num_slots_log2 << TDM_TDM_CFG_SLOT_NUM_SHIFT) |
		(cfg->slot_size << TDM_TDM_CFG_SLOT_SIZE_SHIFT) |
		((cfg->zsi_en) ? TDM_TDM_CFG_SLOT_ZSI_EN : 0));

	al_reg_write32(
		&tdm->cfg_regs->pcm_clk_gen,
		((cfg->pcm_clk_src == AL_TDM_PCM_CLK_SRC_INTERNAL) ? TDM_TDM_CFG_PCM_CLK_GEN_PCM_CLK_SOURCE : 0) |
		((cfg->pcm_tune_dynamic) ? TDM_TDM_CFG_PCM_CLK_GEN_PCM_TUNE_DYNAMIC : 0) |
		(cfg->pcm_tune_period << TDM_TDM_CFG_PCM_CLK_GEN_PCM_TUNE_PERIOD_SHIFT) |
		(cfg->pcm_clk_ratio << TDM_TDM_CFG_PCM_CLK_GEN_PCM_CLK_RATIO_SHIFT) |
		(cfg->pcm_tune_delta << TDM_TDM_CFG_PCM_CLK_GEN_PCM_TUNE_DELTA_SHIFT) |
		(cfg->pcm_tune_rnd_up ? TDM_TDM_CFG_PCM_CLK_GEN_PCM_TUNE_RND_DIR : 0) |
		(cfg->pcm_tune_keep ? TDM_TDM_CFG_PCM_CLK_GEN_PCM_TUNE_KEEP : 0));

	al_reg_write32(
		&tdm->cfg_regs->pcm_window,
		(cfg->window_frame_ratio << TDM_TDM_CFG_PCM_WINDOW_WINDOW_FRAME_RATIO_SHIFT) |
		(cfg->window_hyst_start << TDM_TDM_CFG_PCM_WINDOW_WINDOW_HYST_START_SHIFT) |
		(cfg->window_hyst_level << TDM_TDM_CFG_PCM_WINDOW_WINDOW_HYST_LEVEL_SHIFT));

	al_assert(cfg->fsync_len > 0);
	al_reg_write32(
		&tdm->cfg_regs->fsync_ctl,
		((cfg->fsync_edge == AL_TDM_EDGE_RISING) ? TDM_TDM_CFG_FSYNC_CTL_FSYNC_EDGE : 0) |
		((cfg->fsync_inv) ? TDM_TDM_CFG_FSYNC_CTL_INV : 0) |
		((cfg->fsync_len - 1) << TDM_TDM_CFG_FSYNC_CTL_LEN_SHIFT));

	err = al_tdm_config_xmit_ctrl(
		&cfg->tx_ctrl,
		&tdm->cfg_regs->tx_ctl,
		&tdm->cfg_regs->tx_sa_0,
		&tdm->cfg_regs->tx_sa_1,
		&tdm->cfg_regs->tx_sa_2,
		&tdm->cfg_regs->tx_sa_3,
		&tdm->cfg_regs->tx_limits,
		&tdm->tx_sample_num_bytes,
		&tdm->tx_num_slots_active);
	if (err) {
		al_err("%s: al_tdm_config_xmit_ctrl failed!\n", __func__);
		return err;
	}

	al_reg_write32_masked(
		&tdm->cfg_regs->tx_ctl,
		TDM_TDM_CFG_TX_CTL_TX_PADDING_DATA |
		TDM_TDM_CFG_TX_CTL_TXD_DRV_INACTIVE,
		((cfg->tx_specific_ctrl.tx_padding_data ==
		  AL_TDM_TX_PADDING_DATA_0) ?
		 0 : TDM_TDM_CFG_TX_CTL_TX_PADDING_DATA) |
		((cfg->tx_specific_ctrl.tx_inactive_data ==
		  AL_TDM_TX_INACTIVE_DATA_USER) ?
		 TDM_TDM_CFG_TX_CTL_TXD_DRV_INACTIVE : 0));

	al_reg_write32(
		&tdm->cfg_regs->tx_uf_data,
		cfg->tx_specific_ctrl.tx_underflow_data_user_val);

	al_reg_write32(
		&tdm->cfg_regs->tx_na_data,
		cfg->tx_specific_ctrl.tx_inactive_data_user_val);

	err = al_tdm_config_xmit_ctrl(
		&cfg->rx_ctrl,
		&tdm->cfg_regs->rx_ctl,
		&tdm->cfg_regs->rx_sa_0,
		&tdm->cfg_regs->rx_sa_1,
		&tdm->cfg_regs->rx_sa_2,
		&tdm->cfg_regs->rx_sa_3,
		&tdm->cfg_regs->rx_limits,
		&tdm->rx_sample_num_bytes,
		&tdm->rx_num_slots_active);
	if (err) {
		al_err("%s: al_tdm_config_xmit_ctrl failed!\n", __func__);
		return err;
	}

	al_reg_write32(
		&tdm->cfg_regs->misc,
		((cfg->parity_en) ? TDM_TDM_CFG_MISC_PARITY_EN : 0) |
		((cfg->slverr_en) ? TDM_TDM_CFG_MISC_SLVERR_EN : 0));

	tdm->tx_fifo_auto_track = cfg->tx_ctrl.auto_track;
	tdm->rx_fifo_auto_track = cfg->rx_ctrl.auto_track;

	return 0;
}

/******************************************************************************
 ******************************************************************************/
void al_tdm_set_enables(
	struct al_tdm	*tdm,
	al_bool		enable,
	al_bool		stream_enable,
	al_bool		rx_stream_enable,
	al_bool		tx_stream_enable)
{
	al_dbg(
		"%s(%p, %d, %d, %d)\n", __func__,
		tdm, stream_enable, rx_stream_enable, tx_stream_enable);

	al_assert(tdm);

	al_reg_write32(
		&tdm->cfg_regs->tdmenable,
		((enable) ? TDM_TDM_CFG_TDMENABLE_ENABLE : 0) |
		((stream_enable) ? TDM_TDM_CFG_TDMENABLE_STREAM_ENABLE : 0) |
		((rx_stream_enable) ? TDM_TDM_CFG_TDMENABLE_RX_STREAM_EN : 0) |
		((tx_stream_enable) ? TDM_TDM_CFG_TDMENABLE_TX_STREAM_EN : 0));
}

/******************************************************************************
 ******************************************************************************/
void al_tdm_tx_enable_set(
	struct al_tdm	*tdm,
	al_bool		tx_stream_enable)
{
	al_dbg("%s(%p, %d)\n", __func__, tdm, tx_stream_enable);

	al_assert(tdm);

	al_reg_write32_masked(
		&tdm->cfg_regs->tdmenable,
		TDM_TDM_CFG_TDMENABLE_TX_STREAM_EN,
		(tx_stream_enable ? TDM_TDM_CFG_TDMENABLE_TX_STREAM_EN : 0));
}

/******************************************************************************
 ******************************************************************************/
void al_tdm_rx_enable_set(
	struct al_tdm	*tdm,
	al_bool		rx_stream_enable)
{
	al_dbg("%s(%p, %d)\n", __func__, tdm, rx_stream_enable);

	al_assert(tdm);

	al_reg_write32_masked(
		&tdm->cfg_regs->tdmenable,
		TDM_TDM_CFG_TDMENABLE_RX_STREAM_EN,
		(rx_stream_enable ? TDM_TDM_CFG_TDMENABLE_RX_STREAM_EN : 0));
}

/******************************************************************************
 ******************************************************************************/
void al_tdm_tx_fifo_status_get(
	struct al_tdm			*tdm,
	struct al_tdm_tx_fifo_status	*status)
{
	uint32_t reg_val;

	al_dbg("%s(%p, %p)\n", __func__, tdm, status);

	al_assert(tdm);
	al_assert(status);

	reg_val = al_reg_read32(&tdm->cfg_regs->txstatus);
	status->underrun = (reg_val & TDM_TDM_CFG_TXSTATUS_TX_UNDERRUN) ?
		AL_TRUE : AL_FALSE;
	status->almost_empty = (reg_val & TDM_TDM_CFG_TXSTATUS_TX_AEMPTY) ?
		AL_TRUE : AL_FALSE;
	status->num_samples_vacant = (TX_FIFO_WA_SIZE / tdm->tx_sample_num_bytes) -
		((reg_val & TDM_TDM_CFG_TXSTATUS_TX_INUSE_MASK) >>
		 TDM_TDM_CFG_TXSTATUS_TX_INUSE_SHIFT);

	al_dbg(
		"%s(%p, %p) --> underrun = %d, almost_empty = %d, num_samples_vacant = %d\n",
		__func__, tdm, status, status->underrun, status->almost_empty,
		status->num_samples_vacant);
}

/******************************************************************************
 ******************************************************************************/
void al_tdm_tx_fifo_clear(
	struct al_tdm *tdm)
{
	al_dbg("%s(%p)\n", __func__, tdm);

	al_assert(tdm);

	al_reg_write32(&tdm->cfg_regs->tdmclear, TDM_TDM_CFG_TDMCLEAR_TX_CLEAR);
	tdm->tx_fifo_head = 0;
	tdm->tx_fifo_head_pending_samples = 0;
}

/******************************************************************************
 ******************************************************************************/
void al_tdm_rx_fifo_status_get(
	struct al_tdm			*tdm,
	struct al_tdm_rx_fifo_status	*status)
{
	uint32_t reg_val;

	al_dbg("%s(%p, %p)\n", __func__, tdm, status);

	al_assert(tdm);
	al_assert(status);

	reg_val = al_reg_read32(&tdm->cfg_regs->rxstatus);
	status->overrun = (reg_val & TDM_TDM_CFG_RXSTATUS_RX_OVERRUN) ?
		AL_TRUE : AL_FALSE;
	status->enough_avail = (reg_val & TDM_TDM_CFG_RXSTATUS_RX_AFULL) ?
		AL_TRUE : AL_FALSE;
	status->num_samples_avail =
		(tdm->sw_rx_fifo_pending / tdm->rx_sample_num_bytes) +
		((reg_val & TDM_TDM_CFG_RXSTATUS_TX_INUSE_MASK) >>
		TDM_TDM_CFG_RXSTATUS_TX_INUSE_SHIFT);

	al_dbg(
		"%s(%p, %p) --> overrun = %d, enough_avail = %d, num_samples_avail = %d\n",
		__func__, tdm, status, status->overrun, status->enough_avail,
		status->num_samples_avail);
}

/******************************************************************************
 ******************************************************************************/
void al_tdm_rx_fifo_clear(
	struct al_tdm *tdm)
{
	al_dbg("%s(%p)\n", __func__, tdm);

	al_assert(tdm);

	al_reg_write32(&tdm->cfg_regs->tdmclear, TDM_TDM_CFG_TDMCLEAR_RX_CLEAR);
	tdm->rx_fifo_tail = 0;
	tdm->rx_fifo_tail_pending_samples = 0;
}

/******************************************************************************
 ******************************************************************************/
void al_tdm_tx_fifo_samples_add(
	struct al_tdm	*tdm,
	void		*sample_data,
	unsigned int	num_samples,
	al_bool		head_inc)
{
	unsigned int size_remaining;

	al_dbg("%s(%p, %p, %u, %d)\n", __func__,
		tdm, sample_data, num_samples, head_inc);

	al_assert(tdm);
	al_assert(sample_data);
	al_assert(num_samples > 0);

	size_remaining = num_samples * tdm->tx_sample_num_bytes;

	al_assert(size_remaining <= FIFO_SIZE);

	al_tdm_tx_poll(tdm, size_remaining);

	al_memcpy(tdm->sw_tx_fifo_0 + tdm->tx_fifo_head, sample_data, size_remaining);

	tdm_tx_fifo_add(tdm, sample_data, size_remaining);

	if (!tdm->tx_fifo_auto_track) {
		tdm->tx_fifo_head_pending_samples += num_samples;
		if (head_inc) {
			al_reg_write32(&tdm->cfg_regs->tx_inc,
				tdm->tx_fifo_head_pending_samples);
			tdm->tx_fifo_head_pending_samples = 0;
		}
	}
}

/******************************************************************************
 ******************************************************************************/
void al_tdm_rx_fifo_samples_fetch(
	struct al_tdm	*tdm,
	void		*sample_data,
	unsigned int	num_samples,
	al_bool		tail_inc)
{
	unsigned int size_remaining;
	unsigned int size_till_wrap_around;
	unsigned int size_current;

	al_dbg("%s(%p, %p, %u, %d)\n", __func__,
		tdm, sample_data, num_samples, tail_inc);

	al_assert(tdm);
	al_assert(sample_data);
	al_assert(num_samples > 0);

	al_tdm_rx_poll(tdm);

	size_remaining = num_samples * tdm->rx_sample_num_bytes;

	if (tdm->sw_rx_fifo_pending) {
		size_till_wrap_around = FIFO_SIZE - tdm->sw_rx_fifo_tail;
		size_current = (size_remaining <= tdm->sw_rx_fifo_pending) ?
			size_remaining : tdm->sw_rx_fifo_pending;
		size_current = (size_current <= size_till_wrap_around) ?
			size_current : size_till_wrap_around;

		al_memcpy(sample_data, (uint8_t *)tdm->sw_rx_fifo + tdm->sw_rx_fifo_tail,
			size_current);
		tdm->sw_rx_fifo_tail = (tdm->sw_rx_fifo_tail + size_current) % FIFO_SIZE;
		size_remaining -= size_current;
		tdm->sw_rx_fifo_pending -= size_current;
		sample_data = (void *)((uint8_t *)sample_data + size_current);
	}

	if (tdm->sw_rx_fifo_pending && size_remaining) {
		size_current = (size_remaining <= tdm->sw_rx_fifo_pending) ?
			size_remaining : tdm->sw_rx_fifo_pending;
		al_memcpy((uint8_t *)sample_data, tdm->sw_rx_fifo, size_current);
		tdm->sw_rx_fifo_tail = size_current;
		size_remaining -= size_current;
		tdm->sw_rx_fifo_pending -= size_current;
		sample_data = (void *)((uint8_t *)sample_data + size_current);
	}

	if (!size_remaining)
		return;

	tdm_rx_fifo_fetch(tdm, sample_data, size_remaining);

	if (!tdm->tx_fifo_auto_track) {
		tdm->rx_fifo_tail_pending_samples += num_samples;
		if (tail_inc) {
			al_reg_write32(&tdm->cfg_regs->rx_inc,
				tdm->rx_fifo_tail_pending_samples);
			tdm->rx_fifo_tail_pending_samples = 0;
		}
	}
}

/******************************************************************************
 ******************************************************************************/
void al_tdm_status_print(
	struct al_tdm	*tdm)
{
	struct al_tdm_fifo_ptr_info ptr_info;

	al_print("TDM info:\n");
	al_print("\tTx:\n");
	al_tdm_tx_fifo_ptr_info_get(tdm, &ptr_info);
	al_print("\t\tbyte_cnt_tail = %08x\n", ptr_info.byte_cnt_tail);
	al_print("\t\tbyte_cnt_head = %08x\n", ptr_info.byte_cnt_head);
	al_print("\t\tsample_cnt_tail = %08x\n", ptr_info.sample_cnt_tail);
	al_print("\t\tframe_cnt_tail = %08x\n", ptr_info.frame_cnt_tail);
	al_print("\t\tsample_cnt_head = %08x\n", ptr_info.sample_cnt_head);
	al_print("\t\tframe_cnt_head = %08x\n", ptr_info.frame_cnt_head);

	al_print("\tRx:\n");
	al_tdm_rx_fifo_ptr_info_get(tdm, &ptr_info);
	al_print("\t\tbyte_cnt_tail = %08x\n", ptr_info.byte_cnt_tail);
	al_print("\t\tbyte_cnt_head = %08x\n", ptr_info.byte_cnt_head);
	al_print("\t\tsample_cnt_tail = %08x\n", ptr_info.sample_cnt_tail);
	al_print("\t\tframe_cnt_tail = %08x\n", ptr_info.frame_cnt_tail);
	al_print("\t\tsample_cnt_head = %08x\n", ptr_info.sample_cnt_head);
	al_print("\t\tframe_cnt_head = %08x\n", ptr_info.frame_cnt_head);
	al_print("\t\tsw_rx_fifo_head = %08x\n", tdm->sw_rx_fifo_head);
	al_print("\t\tsw_rx_fifo_tail = %08x\n", tdm->sw_rx_fifo_tail);
}

/** @} end of TDM group */
