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
 * @file   al_hal_pll.c
 *
 * @brief  PLL HAL driver
 *
 */

#include "al_hal_pll.h"
#include "al_hal_pll_regs.h"
#include "al_hal_pll_map.h"

#define AL_PLL_NUM_CHNLS					16
#define AL_PLL_NUM_CHNL_PAIRS					((AL_PLL_NUM_CHNLS) / 2)
#define AL_PLL_DIV_LOCK_START_DELAY				3

#define PLL_CH_PAIR_CFG_DIV0_MASK				0x0000FFFF
#define PLL_CH_PAIR_CFG_DIV0_SHIFT				0
#define PLL_CH_PAIR_CFG_DIV1_MASK				0xFFFF0000
#define PLL_CH_PAIR_CFG_DIV1_SHIFT				16

#define PLL_GEN_STATUS_7_PLL_FSM_STATE_VAL_DIV_STABLE		6
#define PLL_GEN_STATUS_7_PLL_FSM_STATE_VAL_DIV_ACTIVE		7

#define REV1							0
#define REV2							1
#define REV3							2

/**
 * Determine whether a PLL is locked
 */
static inline int al_pll_is_locked_s(
	struct al_pll_obj	*obj);

/**
 * Determine whether a PLL is bypassed
 */
static inline int al_pll_is_bypassed_s(
	struct al_pll_obj	*obj);

/**
 * Get current PLL frequency, according to its configuration and lock state
 */
static unsigned int al_pll_freq_get_s(
	struct al_pll_obj	*obj);

/**
 * Determine whether all PLL's dividers are locked
 */
static inline int al_pll_channel_div_are_all_locked_s(
	struct al_pll_obj	*obj);

/**
 * Get PLL specific channel divider parameters
 */
static inline uint16_t al_pll_channel_params_get_s(
	struct al_pll_obj	*obj,
	unsigned int		chan_idx);

/**
 * Set PLL specific channel divider parameters
 */
static inline void al_pll_channel_params_set_s(
	struct al_pll_obj	*obj,
	unsigned int		chan_idx,
	uint16_t		params);

/**
 * Determine whether a PLL test is done
 */
static inline int al_pll_test_is_done_s(
	struct al_pll_obj	*obj);

/******************************************************************************/
/******************************************************************************/
int al_pll_init(
	void __iomem			*regs_base,
	const char				*name,
	enum al_pll_ref_clk_freq	ref_clk_freq,
	struct al_pll_obj		*obj)
{
	struct al_pll_regs *regs;
	int status = 0;

	/* al_dbg(
		"%s(%p, %s, %d, %p)\n",
		__func__,
		regs_base,
		name,
		ref_clk_freq,
		obj); */

	al_assert(regs_base);
	al_assert(name);
	al_assert(obj);

	obj->regs_base = regs_base;

	obj->name = name;

	regs = (struct al_pll_regs *)obj->regs_base;
	obj->rev = (al_reg_read32(&regs->gen.status_7) & PLL_GEN_STATUS_7_REVISION_ID_MASK) >>
		PLL_GEN_STATUS_7_REVISION_ID_SHIFT;

	switch (obj->rev) {
	case REV1:
		switch (ref_clk_freq) {
		case AL_PLL_REF_CLK_FREQ_125_MHZ:
			obj->ref_clk_freq_val = 125000;
			obj->freq_map = (const void *)al_pll_freq_map_v1_125;
			obj->freq_map_size = sizeof(al_pll_freq_map_v1_125) /
				sizeof(al_pll_freq_map_v1_125[0]);
			break;

		case AL_PLL_REF_CLK_FREQ_100_MHZ:
			obj->ref_clk_freq_val = 100000;
			obj->freq_map = (const void *)al_pll_freq_map_v1_100;
			obj->freq_map_size = sizeof(al_pll_freq_map_v1_100) /
				sizeof(al_pll_freq_map_v1_100[0]);
			break;

		case AL_PLL_REF_CLK_FREQ_25_MHZ:
			obj->ref_clk_freq_val = 25000;
			obj->freq_map = (const void *)al_pll_freq_map_v1_25;
			obj->freq_map_size = sizeof(al_pll_freq_map_v1_25) /
				sizeof(al_pll_freq_map_v1_25[0]);
			break;

		default:
			al_err(
				"%s: invalid ref clk freq enum (%d)!\n",
				__func__,
				ref_clk_freq);
			status = -EINVAL;
		}
		break;
#if (!defined(AL_DEV_ID) || (AL_DEV_ID > AL_DEV_ID_ALPINE_V2))
	case REV2:
		switch (ref_clk_freq) {
		case AL_PLL_REF_CLK_FREQ_100_MHZ:
			obj->ref_clk_freq_val = 100000;
			obj->freq_map = (const void *)al_pll_freq_map_v2_100;
			obj->freq_map_size = sizeof(al_pll_freq_map_v2_100) /
				sizeof(al_pll_freq_map_v2_100[0]);
			break;

		case AL_PLL_REF_CLK_FREQ_50_MHZ:
			obj->ref_clk_freq_val = 50000;
			obj->freq_map = (const void *)al_pll_freq_map_v2_50;
			obj->freq_map_size = sizeof(al_pll_freq_map_v2_50) /
				sizeof(al_pll_freq_map_v2_50[0]);
			break;

		default:
			al_err(
				"%s: invalid ref clk freq enum (%d)!\n",
				__func__,
				ref_clk_freq);
			status = -EINVAL;
		}
		break;

	case REV3:
		switch (ref_clk_freq) {
		case AL_PLL_REF_CLK_FREQ_100_MHZ:
			obj->ref_clk_freq_val = 100000;
			obj->freq_map = (const void *)al_pll_freq_map_v3_100;
			obj->freq_map_size = sizeof(al_pll_freq_map_v3_100) /
				sizeof(al_pll_freq_map_v3_100[0]);
			break;

		case AL_PLL_REF_CLK_FREQ_50_MHZ:
			obj->ref_clk_freq_val = 50000;
			obj->freq_map = (const void *)al_pll_freq_map_v3_50;
			obj->freq_map_size = sizeof(al_pll_freq_map_v3_50) /
				sizeof(al_pll_freq_map_v3_50[0]);
			break;

		case AL_PLL_REF_CLK_FREQ_25_MHZ:
			obj->ref_clk_freq_val = 25000;
			obj->freq_map = (const void *)al_pll_freq_map_v3_25;
			obj->freq_map_size = sizeof(al_pll_freq_map_v3_25) /
				sizeof(al_pll_freq_map_v3_25[0]);
			break;

		default:
			al_err(
				"%s: invalid ref clk freq enum (%d)!\n",
				__func__,
				ref_clk_freq);
			status = -EINVAL;
		}
		break;
#endif
	default:
		al_err("%s: invalid rev (%d)!\n", __func__, obj->rev);
		status = -EINVAL;
	}

	return status;
}

/******************************************************************************/
/******************************************************************************/
int al_pll_terminate(
	struct al_pll_obj	*obj)
{
	int status = 0;

	al_dbg("%s(%p)\n", __func__, obj);

	return status;
}

/******************************************************************************/
/******************************************************************************/
int al_pll_freq_get(
	struct al_pll_obj	*obj,
	enum al_pll_freq	*freq,
	unsigned int		*freq_val)
{
	int status = 0;

	int i;

	const struct al_pll_freq_map_ent *ent;

	/* al_dbg(
		"%s(%p)\n",
		__func__,
		obj); */

	al_assert(obj);

	ent = (const struct al_pll_freq_map_ent *)obj->freq_map;

	*freq = AL_PLL_FREQ_NA;

	*freq_val = al_pll_freq_get_s(obj);

	for (i = 0; i < obj->freq_map_size; i++, ent++) {
		if (*freq_val == ent->freq_val) {
			*freq = ent->freq;
			break;
		}
	}

	/* al_dbg(
		"%s: return (%d, %u)\n",
		__func__,
		*freq,
		*freq_val); */

	return status;
}

/******************************************************************************/
/******************************************************************************/
static void al_pll_freq_set_v1(
	struct al_pll_regs			*regs,
	const struct al_pll_freq_map_ent	*ent)
{
	uint32_t setup_0;
	uint32_t setup_1;
	uint32_t setup_6;

	setup_6 = al_reg_read32(&regs->gen.setup_6_v1);
	setup_6 &= ~PLL_GEN_SETUP_6_V1_PLL_BYPASS;
	al_reg_write32(&regs->gen.setup_6_v1, setup_6);

	setup_1 = al_reg_read32(&regs->gen.setup_1);

	setup_0 =
		(ent->params.v1.nf << PLL_GEN_SETUP_0_V1_NFM1_SHIFT) |
		(ent->params.v1.nr << PLL_GEN_SETUP_0_V1_NRM1_SHIFT) |
		(ent->params.v1.od << PLL_GEN_SETUP_0_V1_ODM1_SHIFT);

	setup_1 =
		(setup_1 & ~PLL_GEN_SETUP_1_BWADJM1_V1_MASK) |
		(ent->params.v1.bwadj << PLL_GEN_SETUP_1_BWADJM1_V1_SHIFT);

	al_reg_write32(&regs->gen.setup_0_v1, setup_0);
	al_reg_write32(&regs->gen.setup_1, setup_1);

	setup_0 |= PLL_GEN_SETUP_0_V1_RELOCK;
	al_reg_write32(&regs->gen.setup_0_v1, setup_0);
}

/******************************************************************************/
/******************************************************************************/
#if (!defined(AL_DEV_ID) || (AL_DEV_ID > AL_DEV_ID_ALPINE_V2))
static void al_pll_freq_set_v2(
	struct al_pll_regs			*regs,
	const struct al_pll_freq_map_ent	*ent)
{
	uint32_t setup_0;
	uint32_t setup_6;

	setup_6 = al_reg_read32(&regs->v2.setup_6);
	setup_6 &= ~PLL_V2_SETUP_6_PLL_BYPASS;
	al_reg_write32(&regs->v2.setup_6, setup_6);

	setup_0 =
		(ent->params.v2.ref_clk_div << PLL_V2_SETUP_0_REF_IN_DIV_SHIFT) |
		(ent->params.v2.out_div << PLL_V2_SETUP_0_OUT_DIV_SHIFT) |
		((ent->params.v2.fb_pre_scalar == 3) ? PLL_V2_SETUP_0_FB_PRE23 : 0) |
		(ent->params.v2.fb_post_scalar << PLL_V2_SETUP_0_FB_DIV255_SHIFT);
	al_reg_write32(&regs->v2.setup_0, setup_0);

	setup_0 |= PLL_V2_SETUP_0_RELOCK;
	al_reg_write32(&regs->v2.setup_0, setup_0);
}

/******************************************************************************/
/******************************************************************************/
static void al_pll_freq_set_v3(
	struct al_pll_regs			*regs,
	const struct al_pll_freq_map_ent	*ent)
{
	uint32_t setup_0;
	uint32_t setup_3;
	uint32_t setup_6;

	setup_6 = al_reg_read32(&regs->v3.setup_6);
	setup_6 &= ~PLL_V3_SETUP_6_PLL_BYPASS;
	al_reg_write32(&regs->v3.setup_6, setup_6);

	setup_0 =
		(ent->params.v3.fb_div << PLL_V3_SETUP_0_FB_DIV_SHIFT) |
		(ent->params.v3.frac ? PLL_V3_SETUP_0_FRAC_ENABLE : 0) |
		(ent->params.v3.post_div_1 << PLL_V3_SETUP_0_POST_DIV_1_SHIFT) |
		(ent->params.v3.post_div_2 << PLL_V3_SETUP_0_POST_DIV_2_SHIFT) |
		(ent->params.v3.ref_div << PLL_V3_SETUP_0_REF_DIV_SHIFT);
	al_reg_write32(&regs->v3.setup_0, setup_0);

	setup_3 = al_reg_read32(&regs->v3.setup_3);
	setup_3 &= ~PLL_V3_SETUP_3_FRAC_MASK;
	setup_3 |= ent->params.v3.frac << PLL_V3_SETUP_3_FRAC_SHIFT;
	al_reg_write32(&regs->v3.setup_3, setup_3);

	setup_0 |= PLL_V3_SETUP_0_RELOCK;
	al_reg_write32(&regs->v3.setup_0, setup_0);
}
#endif

/******************************************************************************/
/******************************************************************************/
int al_pll_freq_set(
	struct al_pll_obj	*obj,
	enum al_pll_freq	freq,
	unsigned int		timeout)
{
	const struct al_pll_freq_map_ent *ent;
	struct al_pll_regs *regs;
	int status = 0;
	int i;

	al_dbg(
		"%s(%p, %d, %u)\n",
		__func__,
		obj,
		freq,
		timeout);

	al_assert(obj);

	ent = (const struct al_pll_freq_map_ent *)obj->freq_map;
	regs = (struct al_pll_regs *)obj->regs_base;

	for (i = 0; i < obj->freq_map_size; i++, ent++) {
		if (freq == ent->freq)
			break;
	}

	if (i == obj->freq_map_size) {
		al_err("%s: invalid freq (%d)\n", __func__, freq);
		status = -EINVAL;
		goto done;
	}

	switch (obj->rev) {
	case REV1:
		al_pll_freq_set_v1(regs, ent);
		break;
#if (!defined(AL_DEV_ID) || (AL_DEV_ID > AL_DEV_ID_ALPINE_V2))
	case REV2:
		al_pll_freq_set_v2(regs, ent);
		break;
	case REV3:
		al_pll_freq_set_v3(regs, ent);
		break;
#endif
	default:
		al_err("%s: invalid rev (%d)!\n", __func__, obj->rev);
		status = -EINVAL;
		goto done;
	}

	while (!(al_pll_is_locked_s(obj)) && (timeout)) {
		al_udelay(1);
		timeout--;
	}

	if (!al_pll_is_locked_s(obj)) {
		al_err("%s: timed out!\n", __func__);
		status = -ETIMEDOUT;
	}

done:
	return status;
}

/******************************************************************************/
/******************************************************************************/
int al_pll_freq_switch_to_bypass(
	struct al_pll_obj	*obj,
	unsigned int		timeout)
{
	int status = 0;
	struct al_pll_regs *regs;
	uint32_t setup_0;
	uint32_t setup_6;

	al_dbg(
		"%s(%p, %u)\n",
		__func__,
		obj,
		timeout);

	al_assert(obj);

	regs = (struct al_pll_regs *)obj->regs_base;

	if (obj->rev == REV1) {
		setup_6 = al_reg_read32(&regs->gen.setup_6_v1);
		setup_6 |= PLL_GEN_SETUP_6_V1_PLL_BYPASS;
		al_reg_write32(&regs->gen.setup_6_v1, setup_6);

		setup_0 = al_reg_read32(&regs->gen.setup_0_v1);
		setup_0 |= PLL_GEN_SETUP_0_V1_RELOCK;
		al_reg_write32(&regs->gen.setup_0_v1, setup_0);
	} else if (obj->rev == REV2) {
		setup_6 = al_reg_read32(&regs->v2.setup_6);
		setup_6 |= PLL_V2_SETUP_6_PLL_BYPASS;
		al_reg_write32(&regs->v2.setup_6, setup_6);

		setup_0 = al_reg_read32(&regs->v2.setup_0);
		setup_0 |= PLL_V2_SETUP_0_RELOCK;
		al_reg_write32(&regs->v2.setup_0, setup_0);
	} else {
		setup_6 = al_reg_read32(&regs->v3.setup_6);
		setup_6 |= PLL_V3_SETUP_6_PLL_BYPASS;
		al_reg_write32(&regs->v3.setup_6, setup_6);

		setup_0 = al_reg_read32(&regs->v3.setup_0);
		setup_0 |= PLL_V3_SETUP_0_RELOCK;
		al_reg_write32(&regs->v3.setup_0, setup_0);
	}

	while (!(al_pll_is_locked_s(obj)) && (timeout)) {
		al_udelay(1);
		timeout--;
	}

	if (!al_pll_is_locked_s(obj)) {
		al_err("%s: timed out!\n", __func__);
		status = -ETIMEDOUT;
	}

	return status;
}

/******************************************************************************/
/******************************************************************************/
int al_pll_is_locked(
	struct al_pll_obj	*obj,
	int			*is_locked)
{
	int status = 0;

	al_dbg("%s(%p)\n", __func__, obj);

	al_assert(obj);

	*is_locked = al_pll_is_locked_s(obj);

	al_dbg(
		"%s: return (%d)\n",
		__func__,
		*is_locked);

	return status;
}

/******************************************************************************/
/******************************************************************************/
int al_pll_channel_freq_get(
	struct al_pll_obj	*obj,
	unsigned int		chan_idx,
	unsigned int		*freq)
{
	int status = 0;

	/* al_dbg("%s(%p, %d)\n", __func__, obj, chan_idx); */

	al_assert(obj);
	al_assert(freq);
	al_assert(chan_idx < AL_PLL_NUM_CHNLS);

	*freq = 0;

	if (al_pll_channel_div_are_all_locked_s(obj)) {
		unsigned int pll_freq = al_pll_freq_get_s(obj);
		uint16_t chan_params =
			al_pll_channel_params_get_s(obj, chan_idx);
		uint16_t div_val =
			((chan_params & PLL_CH_PAIR_CFG_DIV0_VALUE_MASK) >>
			 PLL_CH_PAIR_CFG_DIV0_VALUE_SHIFT);
		uint16_t div_half = !!(chan_params & PLL_CH_PAIR_CFG_DIV0_HALF);
		uint16_t ref_clk_bypass =
			!!(chan_params & PLL_CH_PAIR_CFG_DIV0_REF_CLK_BYPASS);

		if ((!div_val) && ref_clk_bypass)
			*freq = pll_freq;
		else if ((!div_val) && (!ref_clk_bypass))
			*freq = 0;
		else if (div_half && div_val && (!ref_clk_bypass))
			*freq = (2 * pll_freq) / (unsigned int)div_val;
		else if (div_val && (!ref_clk_bypass))
			*freq = pll_freq / (unsigned int)div_val;
		else { /* div_val && ref_clk_bypass */
			al_err(
				"%s: div_val && ref_clk_bypass!\n",
				__func__);
			status = -EIO;
		}
	}

	/* al_dbg(
		"%s: return (%u)\n",
		__func__,
		*freq); */

	return status;
}

/******************************************************************************/
/******************************************************************************/
int al_pll_channel_div_set(
	struct al_pll_obj	*obj,
	unsigned int		chan_idx,
	unsigned int		divider,
	al_bool			divider_mul_half,
	int			reset,
	int			apply,
	unsigned int		timeout)
{
	int status = 0;
	uint16_t chan_params;

	al_dbg(
		"%s(%p, %u, %u, %d, %d, %u)\n",
		__func__,
		obj,
		chan_idx,
		divider,
		reset,
		apply,
		timeout);

	al_assert(obj);
	al_assert(chan_idx < AL_PLL_NUM_CHNLS);

	if (divider & ~PLL_CH_PAIR_CFG_DIV0_VALUE_MASK) {
		al_err(
			"%s: requested divider too big (%u > %u)!\n",
			__func__,
			divider,
			PLL_CH_PAIR_CFG_DIV0_VALUE_MASK >>
			PLL_CH_PAIR_CFG_DIV0_VALUE_SHIFT);
		status = -EINVAL;
		goto done;
	}

	chan_params = 0;

	chan_params |= (divider << PLL_CH_PAIR_CFG_DIV0_VALUE_SHIFT);

	if (divider_mul_half)
		chan_params |= PLL_CH_PAIR_CFG_DIV0_HALF;

	if (!reset)
		chan_params |= PLL_CH_PAIR_CFG_DIV0_RESET_MASK;

	chan_params |= PLL_CH_PAIR_CFG_DIV0_RELOCK_REQUEST;

	al_pll_channel_params_set_s(obj, chan_idx, chan_params);

	if (apply) {
		/* Last in series must be written separately */
		chan_params |= PLL_CH_PAIR_CFG_DIV0_LAST_IN_SERIES;
		al_pll_channel_params_set_s(obj, chan_idx, chan_params);

		/*
		* Addressing RMN: 5593
		*
		* RMN description:
		* PLL divider relock status does not cover pre-relock count
		*
		* Software flow:
		* Add delay before checking the lock status
		*/
		al_udelay(AL_PLL_DIV_LOCK_START_DELAY);

		while ((!al_pll_channel_div_are_all_locked_s(obj)) && timeout) {
			al_udelay(1);
			timeout--;
		}

		if (!al_pll_channel_div_are_all_locked_s(obj)) {
			al_err("%s: timed out!\n", __func__);
			status = -ETIMEDOUT;
		}
	}

done:
	return status;
}

/******************************************************************************/
/******************************************************************************/
int al_pll_channel_switch_to_bypass(
	struct al_pll_obj	*obj,
	unsigned int		chan_idx,
	int			reset,
	int			apply,
	unsigned int		timeout)
{
	int status = 0;
	uint16_t chan_params;

	al_dbg(
		"%s(%p, %u, %d, %d, %u)\n",
		__func__,
		obj,
		chan_idx,
		reset,
		apply,
		timeout);

	al_assert(obj);
	al_assert(chan_idx < AL_PLL_NUM_CHNLS);

	chan_params = 0;

	chan_params |= PLL_CH_PAIR_CFG_DIV0_REF_CLK_BYPASS;

	if (!reset)
		chan_params |= PLL_CH_PAIR_CFG_DIV0_RESET_MASK;

	chan_params |= PLL_CH_PAIR_CFG_DIV0_RELOCK_REQUEST;

	al_pll_channel_params_set_s(obj, chan_idx, chan_params);

	if (apply) {
		/* Last in series must be written separately */
		chan_params |= PLL_CH_PAIR_CFG_DIV0_LAST_IN_SERIES;
		al_pll_channel_params_set_s(obj, chan_idx, chan_params);

		while ((!al_pll_channel_div_are_all_locked_s(obj)) && timeout) {
			al_udelay(1);
			timeout--;
		}

		if (!al_pll_channel_div_are_all_locked_s(obj)) {
			al_err("%s: timed out!\n", __func__);
			status = -ETIMEDOUT;
		}
	}

	return status;
}

/******************************************************************************/
/******************************************************************************/
int al_pll_channel_div_are_all_locked(
	struct al_pll_obj	*obj,
	int			*are_locked)
{
	int status = 0;

	al_dbg(
		"%s(%p)\n",
		__func__,
		obj);

	al_assert(obj);

	*are_locked = al_pll_channel_div_are_all_locked_s(obj);

	al_dbg(
		"%s: return (%d)\n",
		__func__,
		*are_locked);

	return status;
}

/******************************************************************************/
/******************************************************************************/
int al_pll_test(
	struct al_pll_obj	*obj,
	unsigned int		ref_clk_num_cycles,
	unsigned int		ref_clk_ratio,
	int			*test_passed,
	unsigned int		*res_pll_num_cycles,
	unsigned int		timeout)
{
	int status = 0;

	struct al_pll_regs __iomem *regs;
	unsigned int pll_freq;
	unsigned int pll_num_cycles_expected;
	uint32_t tst_ctl;

	al_dbg(
		"%s(%p, %u, %u, %u)\n",
		__func__,
		obj,
		ref_clk_num_cycles,
		ref_clk_ratio,
		timeout);

	al_assert(obj);
	al_assert(!(ref_clk_num_cycles & ~(PLL_GEN_PLL_TEST_SETUP_4_TEST_PERIOD_MASK >>
					PLL_GEN_PLL_TEST_SETUP_4_TEST_PERIOD_SHIFT)));
	al_assert(!(ref_clk_ratio & ~(PLL_GEN_PLL_TEST_SETUP_4_TEST_RATIO_MASK >>
					PLL_GEN_PLL_TEST_SETUP_4_TEST_RATIO_SHIFT)));

	regs = (struct al_pll_regs __iomem *)obj->regs_base;

	tst_ctl =
		(ref_clk_num_cycles << PLL_GEN_PLL_TEST_SETUP_4_TEST_PERIOD_SHIFT) |
		(ref_clk_ratio << PLL_GEN_PLL_TEST_SETUP_4_TEST_RATIO_SHIFT) |
		PLL_GEN_PLL_TEST_SETUP_4_TEST_ENABLE;

	al_reg_write32(&regs->gen.pll_test_setup_4, tst_ctl);

	tst_ctl |= PLL_GEN_PLL_TEST_SETUP_4_TEST_TRIGGER;

	al_reg_write32(&regs->gen.pll_test_setup_4, tst_ctl);

	while (!(al_pll_test_is_done_s(obj)) && (timeout)) {
		al_udelay(1);
		timeout--;
	}

	if (!al_pll_test_is_done_s(obj)) {
		al_err("%s: timed out!\n", __func__);
		status = -ETIMEDOUT;
		goto done;
	}

	*res_pll_num_cycles =
		(al_reg_read32(&regs->gen.test_result_5) &
		 PLL_GEN_TEST_RESULT_5_TEST_COUNT_RESULT_MASK) >>
		PLL_GEN_TEST_RESULT_5_TEST_COUNT_RESULT_SHIFT;

	pll_freq = al_pll_freq_get_s(obj);

	pll_num_cycles_expected =
		(ref_clk_num_cycles * pll_freq) /
		((1 << ref_clk_ratio) * obj->ref_clk_freq_val);

	*test_passed =
		((*res_pll_num_cycles) == pll_num_cycles_expected);

	al_dbg(
		"%s: return (%d, %u)\n",
		__func__,
		*test_passed,
		*res_pll_num_cycles);

done:

	al_reg_write32(&regs->gen.pll_test_setup_4, 0);

	return status;
}

/******************************************************************************/
/******************************************************************************/
static inline int al_pll_is_locked_s(
	struct al_pll_obj	*obj)
{
	struct al_pll_regs *regs =
		(struct al_pll_regs *)obj->regs_base;

	uint32_t status = al_reg_read32(&regs->gen.status_7);

	return !!(status & PLL_GEN_STATUS_7_PLL_LOCK);
}

/******************************************************************************/
/******************************************************************************/
static inline int al_pll_is_bypassed_s(
	struct al_pll_obj	*obj)
{
	struct al_pll_regs *regs =
		(struct al_pll_regs *)obj->regs_base;
	uint32_t reg_val;
	int is_bypassed;

	if (obj->rev == REV1) {
		reg_val	= al_reg_read32(&regs->gen.setup_6_v1);
		is_bypassed = !!(reg_val & PLL_GEN_SETUP_6_V1_PLL_BYPASS);
	} else if (obj->rev == REV2) {
		reg_val	= al_reg_read32(&regs->v2.setup_6);
		is_bypassed = !!(reg_val & PLL_V2_SETUP_6_PLL_BYPASS);
	} else {
		reg_val	= al_reg_read32(&regs->v3.setup_6);
		is_bypassed = !!(reg_val & PLL_V3_SETUP_6_PLL_BYPASS);
	}

	return is_bypassed;
}

/******************************************************************************/
/******************************************************************************/
static unsigned int al_pll_freq_get_v1(
	struct al_pll_regs	*regs,
	unsigned int		ref_clk_freq_val)
{
	unsigned int freq_val = 0;

	uint32_t setup_0 = al_reg_read32(&regs->gen.setup_0_v1);

	unsigned int nf =
		1 + ((setup_0 & PLL_GEN_SETUP_0_V1_NFM1_MASK) >>
		PLL_GEN_SETUP_0_V1_NFM1_SHIFT);
	unsigned int nr =
		1 + ((setup_0 & PLL_GEN_SETUP_0_V1_NRM1_MASK) >>
		PLL_GEN_SETUP_0_V1_NRM1_SHIFT);
	unsigned int od =
		1 + ((setup_0 & PLL_GEN_SETUP_0_V1_ODM1_MASK) >>
		PLL_GEN_SETUP_0_V1_ODM1_SHIFT);

	freq_val = (ref_clk_freq_val * nf) / (nr * od);

	return freq_val;
}

/******************************************************************************/
/******************************************************************************/
static unsigned int al_pll_freq_get_v2(
	struct al_pll_regs	*regs,
	unsigned int		ref_clk_freq_val)
{
	unsigned int freq_val = 0;

	uint32_t setup_0 = al_reg_read32(&regs->v2.setup_0);

	unsigned int ref_clk_div =
		((setup_0 & PLL_V2_SETUP_0_REF_IN_DIV_MASK) >> PLL_V2_SETUP_0_REF_IN_DIV_SHIFT);
	unsigned int fb_pre_scalar = (setup_0 & PLL_V2_SETUP_0_FB_PRE23) ? 3 : 2;
	unsigned int fb_post_scalar =
		((setup_0 & PLL_V2_SETUP_0_FB_DIV255_MASK) >> PLL_V2_SETUP_0_FB_DIV255_SHIFT);
	unsigned int out_div =
		((setup_0 & PLL_V2_SETUP_0_OUT_DIV_MASK) >> PLL_V2_SETUP_0_OUT_DIV_SHIFT);

	freq_val = (fb_pre_scalar * fb_post_scalar * ref_clk_freq_val) / (ref_clk_div * out_div);

	return freq_val;
}

/******************************************************************************/
/******************************************************************************/
static unsigned int al_pll_freq_get_v3(
	struct al_pll_regs	*regs,
	unsigned int		ref_clk_freq_val)
{
	uint32_t setup_0 = al_reg_read32(&regs->v3.setup_0);
	uint32_t setup_3 = al_reg_read32(&regs->v3.setup_3);
	uint64_t fb_div = (setup_0 & PLL_V3_SETUP_0_FB_DIV_MASK) >>
		PLL_V3_SETUP_0_FB_DIV_SHIFT;
	uint64_t frac = (setup_0 & PLL_V3_SETUP_0_FRAC_ENABLE) ?
		((setup_3 & PLL_V3_SETUP_3_FRAC_MASK) >> PLL_V3_SETUP_3_FRAC_SHIFT) : 0;
	unsigned int post_div_1 = (setup_0 & PLL_V3_SETUP_0_POST_DIV_1_MASK) >>
		PLL_V3_SETUP_0_POST_DIV_1_SHIFT;
	unsigned int post_div_2 = (setup_0 & PLL_V3_SETUP_0_POST_DIV_2_MASK) >>
		PLL_V3_SETUP_0_POST_DIV_2_SHIFT;
	unsigned int ref_div = (setup_0 & PLL_V3_SETUP_0_REF_DIV_MASK) >>
		PLL_V3_SETUP_0_REF_DIV_SHIFT;
	unsigned int freq_val =
		((unsigned int)((((fb_div << 24) + frac) * (uint64_t)ref_clk_freq_val) >> 24)) /
		(ref_div * post_div_1 * post_div_2);

	return freq_val;
}

/******************************************************************************/
/******************************************************************************/
static unsigned int al_pll_freq_get_s(
	struct al_pll_obj	*obj)
{
	unsigned int freq_val = 0;

	if (al_pll_is_locked_s(obj)) {
		if (al_pll_is_bypassed_s(obj)) {
			freq_val = obj->ref_clk_freq_val;
		} else {
			struct al_pll_regs *regs =
				(struct al_pll_regs *)obj->regs_base;

			if (obj->rev == REV1)
				freq_val = al_pll_freq_get_v1(regs, obj->ref_clk_freq_val);
			else if (obj->rev == REV2)
				freq_val = al_pll_freq_get_v2(regs, obj->ref_clk_freq_val);
			else
				freq_val = al_pll_freq_get_v3(regs, obj->ref_clk_freq_val);
		}
	}

	return freq_val;
}

/******************************************************************************/
/******************************************************************************/
static inline int al_pll_channel_div_are_all_locked_s(
	struct al_pll_obj	*obj)
{
	int pll_div_locked = 0;
	int pll_div_sm_current_state = 0;

	struct al_pll_regs *regs =
		(struct al_pll_regs *)obj->regs_base;

	uint32_t status = al_reg_read32(&regs->gen.status_7);

	pll_div_sm_current_state = ((status & (PLL_GEN_STATUS_7_PLL_FSM_STATE_MASK)) >>
			PLL_GEN_STATUS_7_PLL_FSM_STATE_SHIFT);

	/*
	* Addressing RMN: 5633
	*
	* RMN description:
	* In the PLL status register there is a dedicated field for the divider state-machine
	* current state. Because of PLL/apb clock synchronization issues, the value in the status
	* register might not reflect the correct state-machine current state.
	*
	* Software flow:
	* For the divider relock completion checking, both state 6 (DIV_ACTIVE) and state 7
	* (DIV_STABLE) can serve as relock completion indication.
	*/
	pll_div_locked =
		(pll_div_sm_current_state == PLL_GEN_STATUS_7_PLL_FSM_STATE_VAL_DIV_STABLE) ||
		(pll_div_sm_current_state == PLL_GEN_STATUS_7_PLL_FSM_STATE_VAL_DIV_ACTIVE);

	return pll_div_locked;
}

/******************************************************************************/
/******************************************************************************/
static inline uint16_t al_pll_channel_params_get_s(
	struct al_pll_obj	*obj,
	unsigned int		chan_idx)
{
	unsigned int pair_idx = chan_idx / 2;
	struct al_pll_regs __iomem *regs =
		(struct al_pll_regs __iomem *)obj->regs_base;
	uint32_t pair = al_reg_read32(&regs->ch_pair[pair_idx].cfg);
	uint16_t params;

	if (!(chan_idx & 1)) {
		params = (pair & PLL_CH_PAIR_CFG_DIV0_MASK) >>
			PLL_CH_PAIR_CFG_DIV0_SHIFT;
	} else {
		params = (pair & PLL_CH_PAIR_CFG_DIV1_MASK) >>
			PLL_CH_PAIR_CFG_DIV1_SHIFT;
	}

	return params;
}

/******************************************************************************/
/******************************************************************************/
static inline void al_pll_channel_params_set_s(
	struct al_pll_obj	*obj,
	unsigned int		chan_idx,
	uint16_t		params)
{
	unsigned int pair_idx = chan_idx / 2;
	struct al_pll_regs __iomem *regs =
		(struct al_pll_regs __iomem *)obj->regs_base;
	uint32_t pair = al_reg_read32(&regs->ch_pair[pair_idx].cfg);

	if (!(chan_idx & 1)) {
		pair &= ~PLL_CH_PAIR_CFG_DIV0_MASK;
		pair |= ((uint32_t)params) <<
			PLL_CH_PAIR_CFG_DIV0_SHIFT;
	} else {
		pair &= ~PLL_CH_PAIR_CFG_DIV1_MASK;
		pair |= ((uint32_t)params) <<
			PLL_CH_PAIR_CFG_DIV1_SHIFT;
	}

	al_reg_write32(&regs->ch_pair[pair_idx].cfg, pair);
}

/******************************************************************************/
/******************************************************************************/
static inline int al_pll_test_is_done_s(
	struct al_pll_obj	*obj)
{
	struct al_pll_regs __iomem *regs =
		(struct al_pll_regs __iomem *)obj->regs_base;

	uint32_t status = al_reg_read32(&regs->gen.status_7);

	return !(status & PLL_GEN_STATUS_7_ATM_BUSY);
}

/** @} end of PLL group */
