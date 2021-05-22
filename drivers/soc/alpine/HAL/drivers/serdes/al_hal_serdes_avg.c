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

#include "al_hal_serdes_avg.h"
#include "al_hal_serdes_avg_internal.h"
#include "al_hal_serdes_avg_utils.h"
#include "al_hal_serdes_avg_lane_regs.h"
#include "al_hal_iofic.h"
#include "al_hal_iofic_regs.h"

/* The minimal delay for Making sure 'progress' bit is asserted [uSec] */
#define CORE_INT_PROG_ASSERTION_DELAY		1

#define CORE_INT_TIMEOUT			100000	/* [uSec] */

#ifndef AL_SERDES_ERR_ONLY
#define AL_SERDES_ERR_ONLY	0
#endif

#if (AL_SERDES_ERR_ONLY)
#define AL_INFO(...)			do { } while (0)
#else
#define AL_INFO(...)			al_info(__VA_ARGS__)
#endif

#ifndef AL_SERDES_FW_CRC_CHECK_SKIP
#define AL_SERDES_FW_CRC_CHECK_SKIP	0
#endif

/* iCal total delay = MDELAY * TRIES */
#define ICAL_MDELAY				10
#define ICAL_TRIES				1000

/* pCal total delay = MDELAY * TRIES */
#define PCAL_MDELAY				10
#define PCAL_TRIES				6000

/* pCal with fixed DC total delay = MDELAY * TRIES */
#define PCAL_FIXED_DC_MDELAY			10
#define PCAL_FIXED_DC_TRIES			1000

/* pCal adaptive enablement delay = MDELAY * TRIES */
#define PCAL_ADAPTIVE_MDELAY			1
#define PCAL_ADAPTIVE_TRIES			5

/* KR training timeout */
#define AL_SERDES_KR_TRAINING_TIMEOUT		500000	/* [uSec] */

/* Signal detection num status reads */
#define SIG_DETECT_NUM_READS			5
/* Signal detection num status reads */
#define SIG_DETECT_READ_DELAY			2	/* [uSec] */
/* Signal detection threshold of number of good reads to be considered as signal detected */
#define SIG_DETECT_CNT_THR			2

/******************************************************************************/
/******************************************************************************/
static const char *complex_type_str_get(enum al_serdes_complex_type type)
{
	switch (type) {
	case AL_SRDS_COMPLEX_TYPE_TNK_MAIN:
		return "TNK_MAIN";
	case AL_SRDS_COMPLEX_TYPE_TR_TC_MAIN:
		return "TR_TC_MAIN";
	case AL_SRDS_COMPLEX_TYPE_TR_MAIN:
		return "TR_MAIN";
	case AL_SRDS_COMPLEX_TYPE_TR_SEC:
		return "TR_SEC";
	case AL_SRDS_COMPLEX_TYPE_TR_HS:
		return "TR_HS";
	default:
		break;
	}

	return "N/A";
}

/******************************************************************************/
/******************************************************************************/
static uint32_t reg_read32_default(uint32_t *p)
{
	return al_reg_read32(p);
}

/******************************************************************************/
/******************************************************************************/
static void reg_write32_default(uint32_t *p, uint32_t v)
{
	al_reg_write32(p, v);
}

/******************************************************************************/
/******************************************************************************/
unsigned int al_serdes_avg_core_interrupt_res_wait_and_get(
	struct al_serdes_grp_obj	*obj)
{
	struct al_srd_1lane_regs __iomem *lane_regs = obj->regs_base;
	uint32_t reg_val;
	unsigned int result;
	int time_left = CORE_INT_TIMEOUT;

	/* Wait for 'active' and 'progress' bit de-assertion */
	do {
		/* Allow enough time for 'progress' bit is asserted */
		al_udelay(CORE_INT_PROG_ASSERTION_DELAY);

		time_left -= CORE_INT_PROG_ASSERTION_DELAY;
		if (time_left <= 0) {
			unsigned int code;
			unsigned int data;

			reg_val = obj->complex_obj->reg_read32(&lane_regs->gen.i_core_interrupt);
			code = AL_REG_FIELD_GET(reg_val, SRD_1LANE_GEN_I_CORE_INTERRUPT_CODE_MASK,
					SRD_1LANE_GEN_I_CORE_INTERRUPT_CODE_SHIFT);
			data = AL_REG_FIELD_GET(reg_val, SRD_1LANE_GEN_I_CORE_INTERRUPT_DATA_MASK,
				SRD_1LANE_GEN_I_CORE_INTERRUPT_DATA_SHIFT);

			al_err("%s: timed out! (complex %s lane %d code %04x data %04x)\n",
				__func__, complex_type_str_get(obj->complex_obj->complex_type),
				obj->lane_in_complex, code, data);

			al_assert(0);
			return 0;
		}

		reg_val = obj->complex_obj->reg_read32(&lane_regs->gen.i_core_interrupt_res);
	} while (reg_val & (SRD_1LANE_GEN_I_CORE_INTERRUPT_RES_PROGRESS |
			SRD_1LANE_GEN_I_CORE_INTERRUPT_RES_ACTIVE));

	result = (reg_val & SRD_1LANE_GEN_I_CORE_INTERRUPT_RES_DATA_MASK) >>
		SRD_1LANE_GEN_I_CORE_INTERRUPT_RES_DATA_SHIFT;

	if (obj->core_int_dbg_en)
		al_print("serdes core int result %04x\n", result);

	return result;
}

/******************************************************************************/
/******************************************************************************/
void al_serdes_avg_core_interrupt_no_wait(
	struct al_serdes_grp_obj	*obj,
	unsigned int			code,
	unsigned int			data)
{
	struct al_srd_1lane_regs __iomem *lane_regs = obj->regs_base;

	if (obj->core_int_dbg_en)
		al_print("serdes core int complex %s lane %d code %04x data %04x\n",
			complex_type_str_get(obj->complex_obj->complex_type), obj->lane_in_complex,
			code, data);

	/* Issue interrupt */
	obj->complex_obj->reg_write32(&lane_regs->gen.i_core_interrupt,
		(code << SRD_1LANE_GEN_I_CORE_INTERRUPT_CODE_SHIFT) |
		(data << SRD_1LANE_GEN_I_CORE_INTERRUPT_DATA_SHIFT));
}

/******************************************************************************/
/******************************************************************************/
unsigned int al_serdes_avg_core_interrupt(
	struct al_serdes_grp_obj	*obj,
	unsigned int			code,
	unsigned int			data)
{
	al_serdes_avg_core_interrupt_no_wait(obj, code, data);

	return al_serdes_avg_core_interrupt_res_wait_and_get(obj);
}

/******************************************************************************/
/******************************************************************************/
void al_serdes_avg_core_interrupt_dbg_en_set(
	struct al_serdes_grp_obj	*obj,
	al_bool				en)
{
	al_assert(obj);

	obj->core_int_dbg_en = en;
}

/******************************************************************************/
/******************************************************************************/
void al_serdes_avg_sata_speed_force(
	struct al_serdes_grp_obj	*obj,
	enum al_serdes_avg_sata_speed	speed)
{
	unsigned int rate =
		(speed == AL_SRDS_AVG_SATA_SPEED_GEN_1) ? CORE_INT_RX_BIT_RATE_RATE_VAL_1_4 :
		((speed == AL_SRDS_AVG_SATA_SPEED_GEN_2) ? CORE_INT_RX_BIT_RATE_RATE_VAL_1_2 :
		CORE_INT_RX_BIT_RATE_RATE_VAL_1_1);

	al_assert(obj);

	al_serdes_avg_core_interrupt(obj, CORE_INT_RX_BIT_RATE_REG_NUM,
		(60 << CORE_INT_RX_BIT_RATE_RATIO_SHIFT) |
		(rate << CORE_INT_RX_BIT_RATE_RATE_SHIFT));
}

/******************************************************************************/
/******************************************************************************/
static enum al_serdes_type al_serdes_avg_type_get(void)
{
	return AL_SRDS_TYPE_AVG;
}

static void loopback_control(
	struct al_serdes_grp_obj	*obj,
	enum al_serdes_lane		lane,
	enum al_serdes_lb_mode		mode)
{
	unsigned int val;
	uint32_t reg_val;
	struct al_srd_1lane_regs __iomem *lane_regs;

	al_assert(obj);
	al_assert(lane == AL_SRDS_LANE_0);

	lane_regs = obj->regs_base;

	/* Core loopback */
	switch (mode) {
	case AL_SRDS_LB_MODE_OFF:
		val = 0x300;
		break;
	case AL_SRDS_LB_MODE_PMA_SERIAL_TX_IO_TO_RX_IO:
		val = 0x301;
		break;
	case AL_SRDS_LB_MODE_PMA_PARALLEL_RX_TO_TX:
		val = 0x310;
		break;
	case AL_SRDS_LB_MODE_WRAPPER_RX_TO_TX_FIFO:
		/* Disable PMA loopback */
		val = 0x300;
		break;
	default:
		al_err("%s: invalid lb mode (%d)\n", __func__, mode);
		return;
	}

	/* Programming loopback mode */
	al_serdes_avg_core_interrupt(obj, 0x0008, val);

	/* Wrapper loopback */
	switch (mode) {
		break;
	case AL_SRDS_LB_MODE_WRAPPER_RX_TO_TX_FIFO:
		/* Enable FIFO logic in SerDes wrapper */
		reg_val = obj->complex_obj->reg_read32(&lane_regs->gen.cntl);
		reg_val &= ~SRD_1LANE_GEN_CNTL_LOOPBACK_FIFO_BYPASS;
		reg_val |= SRD_1LANE_GEN_CNTL_LOOPBACK_FIFO_RESET;
		obj->complex_obj->reg_write32(&lane_regs->gen.cntl, reg_val);
		reg_val &= ~SRD_1LANE_GEN_CNTL_LOOPBACK_FIFO_RESET;
		obj->complex_obj->reg_write32(&lane_regs->gen.cntl, reg_val);
		reg_val |= SRD_1LANE_GEN_CNTL_LINK_LOOPBACK_SET;
		reg_val |= SRD_1LANE_GEN_CNTL_LINK_LOOPBACK_MASK;
		obj->complex_obj->reg_write32(&lane_regs->gen.cntl, reg_val);
		/* Assert o_link_loopback_en */
		al_serdes_avg_core_interrupt(obj, CORE_INT_BIG_REG_SEL_REG_NUM,
			BIG_REG_TBD1_REG_NUM);
		reg_val = al_serdes_avg_core_interrupt(obj, CORE_INT_BIG_REG_READ_REG_NUM, 0);
		reg_val |= BIG_REG_TBD1_O_LINK_LOOPBACK_EN;
		al_serdes_avg_core_interrupt(obj, CORE_INT_BIG_REG_SEL_REG_NUM,
			BIG_REG_TBD1_REG_NUM);
		al_serdes_avg_core_interrupt(obj, CORE_INT_BIG_REG_WRITE_REG_NUM, reg_val);
		break;
	case AL_SRDS_LB_MODE_OFF:
	case AL_SRDS_LB_MODE_PMA_SERIAL_TX_IO_TO_RX_IO:
	case AL_SRDS_LB_MODE_PMA_PARALLEL_RX_TO_TX:
	default:
		break;
	}
}

static al_bool pcal_adaptive_is_enabled(
	struct al_serdes_grp_obj	*obj,
	enum al_serdes_lane		lane);

static int pcal_adaptive_stop(
	struct al_serdes_grp_obj	*obj,
	enum al_serdes_lane		lane);

static int pcal_adaptive_start(
	struct al_serdes_grp_obj	*obj,
	enum al_serdes_lane		lane);

static void al_serdes_avg_bist_aux_err_enable(
	struct al_serdes_grp_obj	*obj)
{
	unsigned int val;

	/* Set TC_COUNTER_EN_SEL  = 1 */
	/* Set TC_COUNTER_CLK_SEL = 5 (rx) */
	/* Set TC_COUNTER_VAL_SEL = 2 (error_count_val) */
	al_serdes_avg_core_interrupt(
		obj, CORE_INT_ESB_REG_WR(ESB_REG_BIST_AUX_ERR_CTRL_3),
		ESB_REG_BIST_AUX_ERR_CTRL_3_CFG_VAL_1);

	/* Set TC_COUNTER_SYNC_EN = 1 */
	/* Set TC_TIMER_CLK_SEL   = 5 (rx) */
	al_serdes_avg_core_interrupt(
		obj, CORE_INT_ESB_REG_WR(ESB_REG_BIST_AUX_ERR_CTRL_2),
		ESB_REG_BIST_AUX_ERR_CTRL_2_CFG_VAL);

	/* Set CMP_CLK_SEL = 0 (rx_clk) */
	/* Set CMP_SIMPLE_EN_ERROR_OCCURRED = 1 */
	val = al_serdes_avg_core_interrupt(
		obj, CORE_INT_ESB_REG_RD(ESB_REG_BIST_AUX_ERR_CTRL_1), 0);
	al_serdes_avg_core_interrupt(
		obj, CORE_INT_ESB_REG_WR(ESB_REG_BIST_AUX_ERR_CTRL_1),
		(val & ~ESB_REG_BIST_AUX_ERR_CTRL_1_CFG_MASK) |
		ESB_REG_BIST_AUX_ERR_CTRL_1_CFG_VAL);

	/* Set CAL_CLK_EN = 1 */
	/* Set CMP_CLK_EN = 1 */
	/* Set TC_COUNTER_CLK_EN = 1 */
	val = al_serdes_avg_core_interrupt(
		obj, CORE_INT_ESB_REG_RD(ESB_REG_BIST_AUX_ERR_CTRL_0), 0);
	al_serdes_avg_core_interrupt(
		obj, CORE_INT_ESB_REG_WR(ESB_REG_BIST_AUX_ERR_CTRL_0),
		(val & ~ESB_REG_BIST_AUX_ERR_CTRL_0_CFG_MASK) |
		ESB_REG_BIST_AUX_ERR_CTRL_0_CFG_VAL);

	/* Set and clear TC_COUNTER_RESET */
	al_serdes_avg_core_interrupt(
		obj, CORE_INT_ESB_REG_WR(ESB_REG_BIST_AUX_ERR_CTRL_3),
		ESB_REG_BIST_AUX_ERR_CTRL_3_CFG_VAL_2);
	al_serdes_avg_core_interrupt(
		obj, CORE_INT_ESB_REG_WR(ESB_REG_BIST_AUX_ERR_CTRL_3),
		ESB_REG_BIST_AUX_ERR_CTRL_3_CFG_VAL_1);
}

static void al_serdes_avg_bist_aux_err_disable(
	struct al_serdes_grp_obj	*obj)
{
	unsigned int val;

	val = al_serdes_avg_core_interrupt(
		obj, CORE_INT_ESB_REG_RD(ESB_REG_BIST_AUX_ERR_CTRL_0), 0);
	al_serdes_avg_core_interrupt(
		obj, CORE_INT_ESB_REG_WR(ESB_REG_BIST_AUX_ERR_CTRL_0),
		(val & ~ESB_REG_BIST_AUX_ERR_CTRL_0_DIS_MASK) |
		ESB_REG_BIST_AUX_ERR_CTRL_0_DIS_VAL);
	val = al_serdes_avg_core_interrupt(
		obj, CORE_INT_ESB_REG_RD(ESB_REG_BIST_AUX_ERR_CTRL_1), 0);
	al_serdes_avg_core_interrupt(
		obj, CORE_INT_ESB_REG_WR(ESB_REG_BIST_AUX_ERR_CTRL_1),
		(val & ~ESB_REG_BIST_AUX_ERR_CTRL_1_DIS_MASK) |
		ESB_REG_BIST_AUX_ERR_CTRL_1_DIS_VAL);
	al_serdes_avg_core_interrupt(
		obj, CORE_INT_ESB_REG_WR(ESB_REG_BIST_AUX_ERR_CTRL_2),
		ESB_REG_BIST_AUX_ERR_CTRL_2_DIS_VAL);
	al_serdes_avg_core_interrupt(
		obj, CORE_INT_ESB_REG_WR(ESB_REG_BIST_AUX_ERR_CTRL_3),
		ESB_REG_BIST_AUX_ERR_CTRL_3_DIS_VAL);
}

static uint32_t al_serdes_avg_bist_aux_err_get(
	struct al_serdes_grp_obj	*obj)
{
	unsigned int lo;
	unsigned int hi0;
	unsigned int hi1;

	do {
		hi0 = al_serdes_avg_core_interrupt(
			obj, CORE_INT_ESB_REG_RD(ESB_REG_BIST_AUX_ERR_CNT_HI), 0x0);
		lo = al_serdes_avg_core_interrupt(
			obj, CORE_INT_ESB_REG_RD(ESB_REG_BIST_AUX_ERR_CNT_LO), 0x0);
		hi1 = al_serdes_avg_core_interrupt(
			obj, CORE_INT_ESB_REG_RD(ESB_REG_BIST_AUX_ERR_CNT_HI), 0x0);
	} while (hi0 != hi1);

	return (hi1 << 16) | lo;
}

static void bist_ctrl(
	struct al_serdes_grp_obj	*obj,
	enum al_serdes_bist_pattern	pattern,
	al_bool				tx_en,
	al_bool				rx_en,
	al_bool				tx_dis,
	al_bool				rx_dis)
{
	unsigned int val;
	al_bool pcal_adaptive_is_en;

	if (rx_dis)
		al_serdes_avg_bist_aux_err_disable(obj);

	/* Disable adaptive pCal if was enabled */
	pcal_adaptive_is_en = pcal_adaptive_is_enabled(obj, AL_SRDS_LANE_0);
	if (pcal_adaptive_is_en) {
		al_dbg("%s: adaptive pCal is enabled\n", __func__);
		/* Disable adaptive pCal */
		if (pcal_adaptive_stop(obj, AL_SRDS_LANE_0))
			return;
	}

	/* Enable Tx electrical idle gate (SATA) */
	if (tx_en) {
		val = al_serdes_avg_core_interrupt(obj, CORE_INT_ESB_REG_RD(0x21), 0x0);
		if (!(val & AL_BIT(7)))
			al_serdes_avg_core_interrupt(obj,
				CORE_INT_ESB_REG_WR(0x21), val | AL_BIT(7));
	}

	if (tx_dis && rx_dis) {
		val = CORE_INT_PRBS_CTRL_TX_RX_DIS_VAL;
	} else if (tx_dis) {
		val = CORE_INT_PRBS_CTRL_TX_DIS_VAL;
	} else if (rx_dis) {
		val = CORE_INT_PRBS_CTRL_RX_DIS_VAL;
	} else {
		switch (pattern) {
		case AL_SRDS_BIST_PATTERN_PRBS7:
			val = 0;
			break;
		case AL_SRDS_BIST_PATTERN_PRBS9:
			val = 1;
			break;
		case AL_SRDS_BIST_PATTERN_PRBS11:
			val = 2;
			break;
		case AL_SRDS_BIST_PATTERN_PRBS15:
			val = 3;
			break;
		case AL_SRDS_BIST_PATTERN_PRBS23:
			val = 4;
			break;
		case AL_SRDS_BIST_PATTERN_PRBS31:
			val = 5;
			break;
		default:
			al_err("%s: non supported pattern (%d)\n", __func__, pattern);
			return;
		}

		val <<= CORE_INT_PRBS_CTRL_PATTERN_SHIFT;

		if (tx_en) {
			val |=  CORE_INT_PRBS_CTRL_AUTO_SEED;
			val |= CORE_INT_PRBS_CTRL_TX_EN;
		}
		if (rx_en) {
			val |=  CORE_INT_PRBS_CTRL_RESEED_ON_ERR;
			val |= CORE_INT_PRBS_CTRL_RX_EN;
		}

	}

	al_serdes_avg_core_interrupt(obj, CORE_INT_PRBS_CTRL_REG_NUM, val);

	if (rx_en) {
		val &= ~CORE_INT_PRBS_CTRL_RESEED_ON_ERR;
		al_serdes_avg_core_interrupt(obj, CORE_INT_PRBS_CTRL_REG_NUM, val);
	}

	if (rx_en) {
		al_serdes_avg_core_interrupt(obj, CORE_INT_CMP_CTRL_TMR_LO_REG_NUM, 0);
		al_serdes_avg_core_interrupt(obj, CORE_INT_CMP_CTRL_TMR_HI_REG_NUM, 0);
		al_serdes_avg_core_interrupt(obj, CORE_INT_CMP_CTRL_REG_NUM,
			CORE_INT_CMP_CTRL_CMP_EN |
			CORE_INT_CMP_CTRL_CMP_SUM |
			CORE_INT_CMP_CTRL_SRC_DATA_VAL_RX |
			CORE_INT_CMP_CTRL_CMP_DATA_VAL_USR_PRBS);
		al_serdes_avg_signal_loss_sticky_get(obj, AL_TRUE);
	}

	/* Initiate adaptive pCal if was enabled */
	if (pcal_adaptive_is_en && (!rx_dis))
		pcal_adaptive_start(obj, AL_SRDS_LANE_0);

	if (rx_en)
		al_serdes_avg_bist_aux_err_enable(obj);
}

static void bist_pattern_select(
	struct al_serdes_grp_obj	*obj,
	enum al_serdes_bist_pattern	pattern,
	uint8_t				*user_data)
{
	al_assert(obj);
	al_assert(!user_data);

	obj->bist_pattern = pattern;
	obj->bist_pattern_valid = AL_TRUE;

	bist_ctrl(obj, pattern, AL_FALSE, AL_FALSE, AL_FALSE, AL_FALSE);
}

static void bist_tx_enable(
	struct al_serdes_grp_obj	*obj,
	enum al_serdes_lane		lane,
	al_bool				en)
{
	al_assert(obj);
	al_assert(lane == AL_SRDS_LANE_0);
	al_assert(obj->bist_pattern_valid);

	bist_ctrl(obj, obj->bist_pattern, en, AL_FALSE, !en, AL_FALSE);
}

static void bist_tx_err_inject(
	struct al_serdes_grp_obj	*obj)
{
	al_assert(obj);

	al_serdes_avg_core_interrupt(obj, CORE_INT_ERR_INJECT_REG_NUM, 1);
}

static void bist_rx_enable(
	struct al_serdes_grp_obj	*obj,
	enum al_serdes_lane		lane,
	al_bool				en)
{
	al_assert(obj);
	al_assert(lane == AL_SRDS_LANE_0);
	al_assert(obj->bist_pattern_valid);

	bist_ctrl(obj, obj->bist_pattern, AL_FALSE, en, AL_FALSE, !en);
}

static void bist_rx_status(
	struct al_serdes_grp_obj	*obj,
	enum al_serdes_lane		lane,
	al_bool				*is_locked,
	al_bool				*err_cnt_overflow,
	uint32_t			*err_cnt)
{
	al_assert(obj);
	al_assert(lane == AL_SRDS_LANE_0);
	al_assert(is_locked);
	al_assert(err_cnt_overflow);
	al_assert(err_cnt);

	*is_locked = !al_serdes_avg_signal_loss_sticky_get(obj, AL_TRUE);
	*err_cnt = al_serdes_avg_bist_aux_err_get(obj);
	*err_cnt_overflow = ((*err_cnt) == 0xffffffff);
}

static al_bool rx_valid(
	struct al_serdes_grp_obj	*obj,
	enum al_serdes_lane		lane)
{
	struct al_srd_1lane_regs __iomem *lane_regs;

	al_assert(obj);
	al_assert(lane == AL_SRDS_LANE_0);

	lane_regs = obj->regs_base;

	return !!(obj->complex_obj->reg_read32(&lane_regs->gen.status) &
		SRD_1LANE_GEN_STATUS_RX_RDY);
}

static int ical_start(
	struct al_serdes_grp_obj	*obj,
	enum al_serdes_lane		lane)
{
	al_bool pcal_adaptive_is_en;
	int err;

	al_assert(obj);
	al_assert(lane == AL_SRDS_LANE_0);

	/* Stop pCal adaptive */
	pcal_adaptive_is_en = pcal_adaptive_is_enabled(obj, AL_SRDS_LANE_0);

	if (pcal_adaptive_is_en) {
		err = pcal_adaptive_stop(obj, lane);
		if (err)
			return err;
	}

	/* DWELL bit setting (influence on calibration time) */
	al_serdes_avg_core_interrupt(obj, CORE_INT_BIG_REG_SEL_REG_NUM,
		BIG_REG_DFE_TUNING_DWELL_TIME_REG_NUM);
	al_serdes_avg_core_interrupt(obj, CORE_INT_BIG_REG_WRITE_REG_NUM, 0x1999);
	al_serdes_avg_core_interrupt(obj, CORE_INT_BIG_REG_WRITE_REG_NUM, 0x0);

	/* Disable auto pCal */
	al_serdes_avg_core_interrupt(obj, CORE_INT_SET_RX_EQ_REG_NUM,
		CORE_INT_SET_RX_EQ_AUTO_PCAL_DIS);

	/* Run iCal with fixed DC */
	al_dbg("%s: Running iCal with fixed DC...\n", __func__);
	al_serdes_avg_core_interrupt(obj, CORE_INT_DFE_CTRL_REG_NUM,
		CORE_INT_DFE_CTRL_RUN_ICAL | CORE_INT_DFE_CTRL_FIXED_DC);

	return 0;
}

static void ical_restore_dwell(
	struct al_serdes_grp_obj	*obj)
{
	al_serdes_avg_core_interrupt(obj, CORE_INT_BIG_REG_SEL_REG_NUM,
		BIG_REG_DFE_TUNING_DWELL_TIME_REG_NUM);
	al_serdes_avg_core_interrupt(obj, CORE_INT_BIG_REG_WRITE_REG_NUM, 0x1);
	al_serdes_avg_core_interrupt(obj, CORE_INT_BIG_REG_WRITE_REG_NUM, 0x1);
}

static al_bool ical_is_done(
	struct al_serdes_grp_obj	*obj,
	enum al_serdes_lane		lane)
{
	unsigned int val;

	al_assert(obj);
	al_assert(lane == AL_SRDS_LANE_0);

	val = al_serdes_avg_core_interrupt(obj, CORE_INT_GET_DFE_REG_NUM,
		CORE_INT_GET_DFE_VAL_DFE_STATUS);

	if (val != CORE_INT_GET_DFE_VAL_DFE_STATUS_IOC_COMPLETE)
		return AL_FALSE;

	/* Restore DWELL bit setting */
	ical_restore_dwell(obj);

	al_dbg("%s: iCal done\n", __func__);

	/* Clear the signal loss sticky indication */
	al_serdes_avg_signal_loss_sticky_get(obj, AL_TRUE);

	return AL_TRUE;
}

static int ical_wait_for_completion(
	struct al_serdes_grp_obj	*obj,
	enum al_serdes_lane		lane)
{
	unsigned int i;

	al_assert(obj);
	al_assert(lane == AL_SRDS_LANE_0);

	/* Poll on iCal completion */
	for (i = 0; i < ICAL_TRIES; ++i) {
		if (ical_is_done(obj, lane))
			break;

		al_msleep(ICAL_MDELAY);
	}

	/* Exit if iCal failed */
	if (i >= ICAL_TRIES) {
		/* Restore DWELL bit setting */
		ical_restore_dwell(obj);

		al_err("%s: iCal Timeout!\n", __func__);
		return -ETIMEDOUT;
	}

	return 0;
}

static int pcal_start(
	struct al_serdes_grp_obj	*obj,
	enum al_serdes_lane		lane)
{
	al_assert(obj);
	al_assert(lane == AL_SRDS_LANE_0);

	AL_INFO("%s: Running single pCal...\n", __func__);
	al_serdes_avg_core_interrupt(obj, CORE_INT_DFE_CTRL_REG_NUM,
		CORE_INT_DFE_CTRL_RUN_PCAL_SINGLE);

	return 0;
}

static al_bool pcal_is_done(
	struct al_serdes_grp_obj	*obj,
	enum al_serdes_lane		lane)
{
	unsigned int val;

	al_assert(obj);
	al_assert(lane == AL_SRDS_LANE_0);

	val = al_serdes_avg_core_interrupt(obj, CORE_INT_GET_DFE_REG_NUM,
		CORE_INT_GET_DFE_VAL_DFE_STATUS);

	if (val != CORE_INT_GET_DFE_VAL_DFE_STATUS_IOC_COMPLETE)
		return AL_FALSE;

	AL_INFO("%s: pCal done\n", __func__);

	return AL_TRUE;
}

static int pcal_wait_for_completion(
	struct al_serdes_grp_obj	*obj,
	enum al_serdes_lane		lane)
{
	unsigned int i;

	al_assert(obj);
	al_assert(lane == AL_SRDS_LANE_0);

	/* Poll on pCal completion */
	for (i = 0; i < PCAL_TRIES; ++i) {
		if (pcal_is_done(obj, lane))
			break;

		al_msleep(PCAL_MDELAY);
	}

	/* Exit if pCal failed */
	if (i >= PCAL_TRIES) {
		al_err("%s: pCal Timeout!\n", __func__);
		return -ETIMEDOUT;
	}

	return 0;
}

static al_bool pcal_adaptive_is_enabled(
	struct al_serdes_grp_obj	*obj,
	enum al_serdes_lane		lane)
{
	unsigned int val;
	al_bool pcal_adaptive_is_en;

	al_assert(obj);
	al_assert(lane == AL_SRDS_LANE_0);

	val = al_serdes_avg_core_interrupt(obj, CORE_INT_GET_DFE_REG_NUM,
			CORE_INT_GET_DFE_VAL_DFE_STATUS);
	pcal_adaptive_is_en = (val == CORE_INT_GET_DFE_VAL_DFE_STATUS_PCAL_ADAPTIVE_EN);

	return pcal_adaptive_is_en;
}

static int pcal_adaptive_start(
	struct al_serdes_grp_obj	*obj,
	enum al_serdes_lane		lane)
{
	int i;

	al_assert(obj);
	al_assert(lane == AL_SRDS_LANE_0);

	al_dbg("%s: Initiating adaptive pCal...\n", __func__);
	al_serdes_avg_core_interrupt(obj, CORE_INT_DFE_CTRL_REG_NUM,
		CORE_INT_DFE_CTRL_RUN_PCAL_ADAPTIVE | CORE_INT_DFE_CTRL_FIXED_DC);
	/* Poll on adaptive pCal enablement */
	for (i = 0; i < PCAL_ADAPTIVE_TRIES; ++i) {
		if (pcal_adaptive_is_enabled(obj, lane))
			break;
		al_msleep(PCAL_ADAPTIVE_MDELAY);
	}
	/* Exit if pCal failed */
	if (i >= PCAL_ADAPTIVE_TRIES) {
		al_err("%s: adaptive pCal enablement timeout!\n", __func__);
		return -ETIMEDOUT;
	}
	al_dbg("%s: adaptive pCal initiated\n", __func__);

	return 0;
}

static int pcal_adaptive_stop(
	struct al_serdes_grp_obj	*obj,
	enum al_serdes_lane		lane)
{
	int i;

	al_assert(obj);
	al_assert(lane == AL_SRDS_LANE_0);

	/* Disable adaptive pCal by running single pCal with fixed DC */
	al_serdes_avg_core_interrupt(obj, CORE_INT_DFE_CTRL_REG_NUM,
		CORE_INT_DFE_CTRL_RUN_PCAL_SINGLE | CORE_INT_DFE_CTRL_FIXED_DC);
	/* Poll on pCal completion */
	for (i = 0; i < PCAL_FIXED_DC_TRIES; ++i) {
		unsigned int val = al_serdes_avg_core_interrupt(obj,
			CORE_INT_GET_DFE_REG_NUM, CORE_INT_GET_DFE_VAL_DFE_STATUS);

		if (val == CORE_INT_GET_DFE_VAL_DFE_STATUS_IOC_COMPLETE)
			break;
		al_msleep(PCAL_FIXED_DC_MDELAY);
	}

	/* Exit if pCal failed */
	if (i >= PCAL_FIXED_DC_TRIES) {
		al_err("%s: pCal Timeout!\n", __func__);
		return -ETIMEDOUT;
	}

	return 0;
}

static int rx_equalization(
	struct al_serdes_grp_obj	*obj,
	enum al_serdes_lane		lane)
{
	int err;

	al_assert(obj);
	al_assert(lane == AL_SRDS_LANE_0);

	/* iCal with fixed DC */
	err = ical_start(obj, lane);
	if (err)
		return err;

	err = ical_wait_for_completion(obj, lane);
	if (err)
		return err;

	/* Initiate adaptive pCal */
	err = pcal_adaptive_start(obj, lane);
	if (err)
		return err;

	return 0;
}

static al_bool signal_is_detected(
	struct al_serdes_grp_obj	*obj,
	enum al_serdes_lane		lane)
{
	al_bool sig_detected_final = AL_FALSE;
	unsigned int sig_detected_cnt = 0;
	unsigned int i;

	al_assert(obj);
	al_assert(lane == AL_SRDS_LANE_0);

	/* Enable idle detector with specific threshold */
	al_serdes_avg_core_interrupt(obj, CORE_INT_IDLE_DETECTOR_REG_NUM,
		CORE_INT_IDLE_DETECTOR_RX_IDLE_DETECTOR_EN);

	/* Check signal several times and consider detected if passed threshold */
	for (i = 0; i < SIG_DETECT_NUM_READS; i++) {
		if (!(al_serdes_avg_core_interrupt(obj,
			CORE_INT_ESB_REG_RD(ESB_REG_SIG_STAT_REG_NUM), 0) &
			ESB_REG_SIG_STAT_SIG_DETECTED_N)) {
			sig_detected_cnt++;
			if (sig_detected_cnt >= SIG_DETECT_CNT_THR) {
				sig_detected_final = AL_TRUE;
				break;
			}
		}

		al_udelay(SIG_DETECT_READ_DELAY);
	}

	/* Disable idle detector */
	al_serdes_avg_core_interrupt(obj, CORE_INT_IDLE_DETECTOR_REG_NUM, 0);

	return sig_detected_final;
}

static al_bool cdr_is_locked(
	struct al_serdes_grp_obj	*obj,
	enum al_serdes_lane		lane)
{
	al_assert(obj);
	al_assert(lane == AL_SRDS_LANE_0);

	return !al_serdes_avg_signal_loss_sticky_get(obj, AL_TRUE);
}

/*
 * Masks of interrupts groups
 */
/* group A */
#define AL_SRDS_AVG_INT_GROUP_A_SUM \
		AL_SRDS_AVG_INT_GROUP_A_DOUBLE_BIT_ERROR_INDICATION_IN_SERDES_MEMORY

static void error_masks_get(struct al_serdes_grp_obj *obj,
		uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d)
{
	al_assert(obj);
	al_assert(a);
	al_assert(b);
	al_assert(c);
	al_assert(d);

	*a = AL_SRDS_AVG_INT_GROUP_A_SUM;
	*b = 0;
	*c = 0;
	*d = 0;
}

static void error_ints_unmask(struct al_serdes_grp_obj	*obj)
{
	uint32_t a, b, c, d;

	obj->error_masks_get(obj, &a, &b, &c, &d);

	al_iofic_config(obj->int_mem_base,
		AL_INT_GROUP_A,
		INT_CONTROL_GRP_MASK_MSI_X);

	al_iofic_clear_cause(obj->int_mem_base,
		AL_INT_GROUP_A,
		a);

	al_iofic_unmask(obj->int_mem_base,
		AL_INT_GROUP_A,
		a);
}

/******************************************************************************/
/******************************************************************************/
void al_serdes_avg_complex_handle_init(
	void __iomem			*regs_base,
	void __iomem			*sbus_regs_base,
	enum al_serdes_complex_type	complex_type,
	uint32_t			(*reg_read32)(uint32_t *p),
	void				(*reg_write32)(uint32_t *p, uint32_t v),
	struct al_serdes_complex_obj	*obj)
{
	al_dbg(
		"%s(%p, %d, %p, %p, %p)\n",
		__func__,
		regs_base,
		complex_type,
		reg_read32,
		reg_write32,
		obj);

	al_assert(regs_base);
	al_assert(obj);

	al_memset(obj, 0, sizeof(struct al_serdes_complex_obj));

	obj->regs_base = regs_base;
	obj->complex_type = complex_type;
	if (obj->reg_write32)
		obj->reg_write32 = reg_write32;
	else
		obj->reg_write32 = reg_write32_default;
	if (obj->reg_read32)
		obj->reg_read32 = reg_read32;
	else
		obj->reg_read32 = reg_read32_default;

	al_sbus_handle_init(&obj->sbus_obj, sbus_regs_base);
}

static void pcs_interrupt_enable_set(
	struct al_serdes_grp_obj *obj,
	al_bool en)
{
	struct al_srd_1lane_regs __iomem *lane_regs;
	uint32_t reg_val;

	al_assert(obj);
	al_assert(obj->regs_base);

	lane_regs = obj->regs_base;
	reg_val = obj->complex_obj->reg_read32(&lane_regs->pcie.cfg);
	obj->complex_obj->reg_write32(&lane_regs->pcie.cfg,
		(reg_val & ~SRD_1LANE_PCIE_CFG_PCS_INTERRUPT_DISABLE) |
		(en ? 0 : SRD_1LANE_PCIE_CFG_PCS_INTERRUPT_DISABLE));
}

static void pcie_ical_config_begin(
	struct al_serdes_grp_obj *obj)
{
	al_assert(obj);

	obj->pcie_ical_config_seq_state = 0;
}

static al_bool pcie_ical_config_iterate(
	struct al_serdes_grp_obj *obj)
{
	al_assert(obj);

	switch (obj->pcie_ical_config_seq_state) {
	case 0:
		/* limit the cf max to 125 decimal */
		al_serdes_avg_core_interrupt_res_wait_and_get(obj);
		al_serdes_avg_core_interrupt_no_wait(obj,
			CORE_INT_PLL_CAL_CF_MAX0_PCIE_REG_NUM,
			CORE_INT_PLL_CAL_CF_MAX0_PCIE_REG_VAL);
		break;
	case 1:
		/* limit the cf min to 40 decimal 8 */
		al_serdes_avg_core_interrupt_res_wait_and_get(obj);
		al_serdes_avg_core_interrupt_no_wait(obj,
			CORE_INT_PLL_CAL_CF_MIN1_PCIE_REG_NUM,
			CORE_INT_PLL_CAL_CF_MIN1_PCIE_REG_VAL);
		break;
	case 2:
		/* limit the cf max to 125 decimal */
		al_serdes_avg_core_interrupt_res_wait_and_get(obj);
		al_serdes_avg_core_interrupt_no_wait(obj,
			CORE_INT_PLL_CAL_CF_MAX2_PCIE_REG_NUM,
			CORE_INT_PLL_CAL_CF_MAX2_PCIE_REG_VAL);
		break;
	case 3:
		/* limit the cf min to 40 decimal 8 */
		al_serdes_avg_core_interrupt_res_wait_and_get(obj);
		al_serdes_avg_core_interrupt_no_wait(obj,
			CORE_INT_PLL_CAL_CF_MIN3_PCIE_REG_NUM,
			CORE_INT_PLL_CAL_CF_MIN3_PCIE_REG_VAL);
		break;
	case 4:
		al_serdes_avg_core_interrupt_res_wait_and_get(obj);
		al_serdes_avg_core_interrupt_no_wait(obj,
			CORE_INT_SET_RX_EQ_REG_NUM, CORE_INT_SET_RX_EQ_PCIE_CTLE_HF_SEED);
		break;
	case 5:
		al_serdes_avg_core_interrupt_res_wait_and_get(obj);
		al_serdes_avg_core_interrupt_no_wait(obj,
			CORE_INT_SET_RX_EQ_REG_NUM, CORE_INT_SET_RX_EQ_PCIE_CTLE_LF_SEED);
		break;
	case 6:
		al_serdes_avg_core_interrupt_res_wait_and_get(obj);
		al_serdes_avg_core_interrupt_no_wait(obj,
			CORE_INT_SET_RX_EQ_REG_NUM, CORE_INT_SET_RX_EQ_CTLE_BW(0x7));
		break;
	case 7:
		/* Addressing RMN: 11442
		 *
		 * RMN description:
		 * When doing equal should do full ical on the 2nd RX_EQ_EVAL
		 * Software flow:
		 * In order to do instruct the serdes to do extensive tuning algorithm (iCal).
		 * Should set the Int_run_ical_on_eq_eval as following :
		 * Write 0x5202 to modify flag run_ical_on_eq_eval to 2.
		 */
		al_serdes_avg_core_interrupt_res_wait_and_get(obj);
		al_serdes_avg_core_interrupt_no_wait(obj,
			CORE_INT_SET_RX_EQ_REG_NUM, CORE_INT_SET_RX_EQ_PCIE_RUN_ICAL_DFLT);
		break;
	case 8:
		al_serdes_avg_core_interrupt_res_wait_and_get(obj);
		obj->pcie_ical_config_seq_state++;
		/* Fall through */
	default:
		return AL_FALSE;
	}

	obj->pcie_ical_config_seq_state++;

	return AL_TRUE;
}

static void pcie_run_ical_config(
	struct al_serdes_grp_obj *obj)
{
	pcie_ical_config_begin(obj);
	while (pcie_ical_config_iterate(obj))
		;
}

static void fw_init_status_get(
	struct al_serdes_grp_obj	*lane_obj,
	void				*status)
{
	unsigned int crc_status = CORE_INT_CRC_CHECK_VAL_VALID;
	unsigned int timeout = CORE_INT_CRC_CHECK_TIMEOUT_US;
	struct al_serdes_avg_fw_init_status *fw_init_status =
		(struct al_serdes_avg_fw_init_status *)status;

#if (AL_SERDES_FW_CRC_CHECK_SKIP == 0)
	for (; timeout; timeout--) {
		crc_status = al_serdes_avg_core_interrupt(lane_obj, CORE_INT_CRC_CHECK_REG_NUM, 0);
		if (crc_status != CORE_INT_CRC_CHECK_VAL_IN_PROGRESS)
			break;
		al_udelay(1);
	}
#endif

	if (crc_status == CORE_INT_CRC_CHECK_VAL_VALID) {
		unsigned int fw_rev =
			al_serdes_avg_core_interrupt(lane_obj, CORE_INT_REVISION_GET_REG_NUM, 0);
		unsigned int fw_rev_eng =
			al_serdes_avg_core_interrupt(lane_obj, CORE_INT_REVISION_GET_REG_NUM, 1);
		unsigned int fw_build =
			al_serdes_avg_core_interrupt(lane_obj, CORE_INT_BUILD_ID_GET_REG_NUM, 0);

		AL_INFO("%s: complex %d, lane %d fw rev %02x build %02x rev_eng %02x\n",
			__func__, lane_obj->complex_obj->complex_type, lane_obj->lane_in_complex,
			fw_rev, fw_build, fw_rev_eng);

		if (fw_init_status) {
			fw_init_status->crc_err = AL_FALSE;
			fw_init_status->fw_rev = fw_rev;
			fw_init_status->fw_rev_eng = fw_rev_eng;
			fw_init_status->fw_build = fw_build;
		}
	} else {
		al_err("%s: complex %d, lane %d fw crc check failed (%08x)!\n",
			__func__, lane_obj->complex_obj->complex_type, lane_obj->lane_in_complex,
			crc_status);

		if (fw_init_status) {
			fw_init_status->crc_err = AL_TRUE;
			fw_init_status->fw_rev = 0;
			fw_init_status->fw_rev_eng = 0;
			fw_init_status->fw_build = 0;
		}
	}
}

/******************************************************************************/
/******************************************************************************/
void al_serdes_avg_handle_init(
	void __iomem			*regs_base,
	struct al_serdes_complex_obj	*complex_obj,
	unsigned int			lane_in_complex,
	struct al_serdes_grp_obj	*obj)
{
	al_dbg(
		"%s(%p, %p, %d, %p)\n",
		__func__,
		regs_base,
		complex_obj,
		lane_in_complex,
		obj);

	al_assert(regs_base);
	al_assert(complex_obj);
	al_assert(!complex_obj->lanes[lane_in_complex]);
	al_assert(obj);

	al_memset(obj, 0, sizeof(struct al_serdes_grp_obj));

	obj->regs_base = regs_base;
	obj->int_mem_base = &((struct al_srd_1lane_regs __iomem *)regs_base)->int_ctrl_mem[0];
	obj->complex_obj = complex_obj;
	obj->lane_in_complex = lane_in_complex;

	obj->type_get = al_serdes_avg_type_get;
	/*  obj->reg_read = reg_read; */
	/*  obj->reg_write = reg_write; */
	/*  obj->bist_overrides_enable = bist_overrides_enable; */
	/*  obj->bist_overrides_disable = bist_overrides_disable; */
	/*  obj->rx_rate_change = rx_rate_change; */
	/*  obj->rx_rate_change_sw_flow_en = rx_rate_change_sw_flow_en; */
	/*  obj->rx_rate_change_sw_flow_dis = rx_rate_change_sw_flow_dis; */
	/*  obj->pcie_rate_override_is_enabled = pcie_rate_override_is_enabled; */
	/*  obj->pcie_rate_override_enable_set = pcie_rate_override_enable_set; */
	/*  obj->pcie_rate_get = pcie_rate_get; */
	/*  obj->pcie_rate_set = pcie_rate_set; */
	/*  obj->group_pm_set = group_pm_set; */
	/*  obj->lane_pm_set = lane_pm_set; */
	/*  obj->pma_hard_reset_group = pma_hard_reset_group; */
	/*  obj->pma_hard_reset_lane = pma_hard_reset_lane; */
	obj->loopback_control = loopback_control;
	obj->bist_pattern_select = bist_pattern_select;
	obj->bist_tx_enable = bist_tx_enable;
	obj->bist_tx_err_inject = bist_tx_err_inject;
	obj->bist_rx_enable = bist_rx_enable;
	obj->bist_rx_status = bist_rx_status;
	/*  obj->tx_deemph_preset = tx_deemph_preset; */
	/*  obj->tx_deemph_inc = tx_deemph_inc; */
	/*  obj->tx_deemph_dec = tx_deemph_dec; */
	/*  obj->eye_measure_run = eye_measure_run; */
	/*  obj->eye_diag_sample = eye_diag_sample; */
	/*  obj->eye_diag_run = eye_diag_run; */
	obj->signal_is_detected = signal_is_detected;
	obj->cdr_is_locked = cdr_is_locked;
	obj->rx_valid = rx_valid;
	/*  obj->tx_advanced_params_set = tx_advanced_params_set; */
	/*  obj->tx_advanced_params_get = tx_advanced_params_get; */
	/*  obj->rx_advanced_params_set = rx_advanced_params_set; */
	/*  obj->rx_advanced_params_get = rx_advanced_params_get; */
	/*  obj->mode_set_sgmii = mode_set_sgmii; */
	/*  obj->mode_set_kr = mode_set_kr; */
	obj->rx_equalization = rx_equalization;
	/*  obj->calc_eye_size = calc_eye_size; */
	/*  obj->sris_config = sris_config; */
	/*  obj->dcgain_set = dcgain_set; */
	obj->pcs_interrupt_enable_set = pcs_interrupt_enable_set;
	obj->pcie_run_ical_config = pcie_run_ical_config;
	obj->pcie_ical_config_begin = pcie_ical_config_begin;
	obj->pcie_ical_config_iterate = pcie_ical_config_iterate;
	obj->fw_init_status_get = fw_init_status_get;
	obj->ical_start = ical_start;
	obj->rx_tx_reset = al_serdes_avg_rx_tx_reset;
	obj->rx_tx_reset_is_done = al_serdes_avg_rx_tx_reset_is_done;
	obj->ical_is_done = ical_is_done;
	obj->ical_wait_for_completion = ical_wait_for_completion;
	obj->pcal_start = pcal_start;
	obj->pcal_is_done = pcal_is_done;
	obj->pcal_wait_for_completion = pcal_wait_for_completion;
	obj->pcal_adaptive_start = pcal_adaptive_start;
	obj->pcal_adaptive_stop = pcal_adaptive_stop;
	obj->error_masks_get = error_masks_get;
	obj->errors_unmask = error_ints_unmask;

	obj->complex_obj->lanes[lane_in_complex] = obj;
}

/******************************************************************************/
/******************************************************************************/
int al_serdes_avg_eth_lt(
	struct al_serdes_grp_obj	*obj,
	enum al_serdes_avg_eth_lt_mode	mode)
{
	struct al_srd_1lane_regs __iomem *lane_regs;
	unsigned int rval;
	unsigned int time;
	unsigned int lane;
	unsigned int lane_in_port;
	al_bool pmd_cfg_required = AL_TRUE;
	uint32_t reg;

	al_assert(obj);

	lane = obj->lane_in_complex;
	lane_regs = obj->regs_base;

	switch (mode) {
	case AL_SRDS_AVG_ETH_LT_MODE_10G:
		pmd_cfg_required = AL_FALSE;
		lane_in_port = 0;
		break;
	case AL_SRDS_AVG_ETH_LT_MODE_25G:
		pmd_cfg_required = AL_TRUE;
		lane_in_port = 0;
		break;
	case AL_SRDS_AVG_ETH_LT_MODE_40G:
		pmd_cfg_required = AL_TRUE;
		lane_in_port = lane;
		break;
	case AL_SRDS_AVG_ETH_LT_MODE_50G:
		pmd_cfg_required = AL_TRUE;
		lane_in_port = lane % 2;
		break;
	case AL_SRDS_AVG_ETH_LT_MODE_100G:
		pmd_cfg_required = AL_TRUE;
		lane_in_port = lane;
		break;
	default:
		al_err("%s: non supported mode (%d)!\n", __func__, mode);
		return -EINVAL;
	}

	/* PMD configuration */
	if (pmd_cfg_required) {
		unsigned int bhv;
		unsigned int prbs_seq;
		unsigned int prbs_seed;

		if (mode == AL_SRDS_AVG_ETH_LT_MODE_40G) {
			static const unsigned int prbs_seeds[4] = { 0x001, 0x040, 0x0ff, 0x033 };

			al_assert(lane_in_port < AL_ARR_SIZE(prbs_seeds));

			bhv = CORE_INT_PMD_CFG_TRAINING_BHV_OVERLAY;
			prbs_seq = CORE_INT_PMD_CFG_PRBS_SEQ_CLAUSE_72;
			prbs_seed = CORE_INT_PMD_CFG_PRBS_SEED(prbs_seeds[lane_in_port]);
		} else {
			static const unsigned int prbs_seeds[4] = { 0x57e, 0x645, 0x72d, 0x7b6 };

			al_assert(lane_in_port < CORE_INT_PMD_CFG_PRBS_SEQ_CLAUSE_92_NUM_LANES);
			al_assert(lane_in_port < AL_ARR_SIZE(prbs_seeds));

			bhv = CORE_INT_PMD_CFG_TRAINING_BHV_FIXED;
			prbs_seq = CORE_INT_PMD_CFG_PRBS_SEQ_CLAUSE_92(lane_in_port);
			prbs_seed = CORE_INT_PMD_CFG_PRBS_SEED(prbs_seeds[lane_in_port]);
		}

		rval = al_serdes_avg_core_interrupt(obj, CORE_INT_PMD_CFG_REG_NUM, bhv);
		if (rval != CORE_INT_PMD_CFG_REG_NUM) {
			al_err("%s: [%d]: PMD training behavior config failed (%x)\n",
				__func__, lane, rval);
			return -EIO;
		}
		rval = al_serdes_avg_core_interrupt(obj, CORE_INT_PMD_CFG_REG_NUM, prbs_seq);
		if (rval != CORE_INT_PMD_CFG_REG_NUM) {
			al_err("%s: [%d]: PMD PRBS sequence config failed (%x)\n",
				__func__, lane, rval);
			return -EIO;
		}
		rval = al_serdes_avg_core_interrupt(obj, CORE_INT_PMD_CFG_REG_NUM, prbs_seed);
		if (rval != CORE_INT_PMD_CFG_REG_NUM) {
			al_err("%s: [%d]: PMD PRBS seed config failed (%x)\n",
				__func__, lane, rval);
			return -EIO;
		}
	}

	/* Restart training */
	rval = al_serdes_avg_core_interrupt(
		obj, CORE_INT_PMD_CTRL_REG_NUM, CORE_INT_PMD_CTRL_RESTART_TRAINING);
	if (rval != CORE_INT_PMD_CTRL_REG_NUM) {
		al_err("%s: [%d]: restart link training failed (%x)\n", __func__, lane, rval);
		return -EIO;
	}

	rval = al_serdes_avg_core_interrupt(
		obj, CORE_INT_PMD_CTRL_REG_NUM, CORE_INT_PMD_CTRL_LAUNCH_PMD_TRAIN);
	if (rval != CORE_INT_PMD_CTRL_REG_NUM) {
		al_err("%s: [%d]: launch PMD with training failed (%x)\n", __func__, lane, rval);
		return -EIO;
	}

	/*
	 * Poll o_core_status[1] to see if training completes within 500ms
	 * Once the training window has expired, check o_core_status[2] to see if rx_trained = 1
	 * AN_LT should later confirm that PHY_LOS is 0
	 */

	/* Wait before 1st poll, otherwise, o_core_status[1] may not yet be asserted... */
	al_udelay(1);

	for (time = 0; time < AL_SERDES_KR_TRAINING_TIMEOUT; time++) {
		reg = obj->complex_obj->reg_read32(&lane_regs->gen.status);

		if (!(reg & O_CORE_STATUS_KR_TRAINING_IN_PROGRESS)) {
			/*
			 * Training has completed.  Check:
			 * o_core_status[0] = 0 -> training_failure
			 * o_core_status[2] = 1 -> rx_trained
			 */
			if (reg & O_CORE_STATUS_KR_TRAINING_FAILURE) {
				al_err("%s: [%d]: training_failure! time %d STATUS = %x\n",
					__func__, lane, time, reg);
				return -EIO;
			}
			if (!(reg & O_CORE_STATUS_RX_TRAINED)) {
				al_err("%s: [%dx: rx is not trained! time %d STATUS = %x\n",
					__func__, lane, time, reg);
				return -EIO;
			}

			break;
		}

		al_dbg("%s: [%d]: Link Training still In Progress (time %d STATUS = %x)\n",
			__func__, lane, time, reg);

		al_udelay(1);
	}

	if (time == AL_SERDES_KR_TRAINING_TIMEOUT) {
		al_err("%s: [%d]: Timeout! STATUS = %x\n", __func__, lane, reg);
		return -ETIME;
	}

	return 0;
}

/******************************************************************************/
/******************************************************************************/
void al_serdes_avg_eth_lane_speed_change(
	struct al_serdes_grp_obj		*obj,
	enum al_serdes_avg_eth_lane_speed	speed)
{
	enum al_serdes_bit_rate bit_rate;
	enum al_serdes_bit_width bit_width;

	al_assert(obj);

	/* Obtain required bit-rate & bit-width */
	switch (speed) {
	case AL_SRDS_AVG_ETH_LANE_SPEED_10G:
		bit_rate = AL_SRDS_BIT_RATE_10_3125G;
		bit_width = AL_SRDS_BIT_WIDTH_20;
		break;
	case AL_SRDS_AVG_ETH_LANE_SPEED_10G_40BIT:
		bit_rate = AL_SRDS_BIT_RATE_10_3125G;
		bit_width = AL_SRDS_BIT_WIDTH_40;
		break;
	case AL_SRDS_AVG_ETH_LANE_SPEED_25G:
		bit_rate = AL_SRDS_BIT_RATE_25_78G;
		bit_width = AL_SRDS_BIT_WIDTH_40;
		break;

	default:
		al_err("%s: invalid speed (%d)\n", __func__, speed);
		al_assert(0);
		return;
	};

	/* Program bit-rate (assuming 156.25 MHz reference clock) */
	al_serdes_avg_core_interrupt(obj,
		CORE_INT_TX_BIT_RATE_REG_NUM,
		(ratio_field_val_get(AL_SRDS_CLK_FREQ_156_MHZ, bit_rate) <<
		CORE_INT_TX_BIT_RATE_RATIO_SHIFT) |
		(CORE_INT_TX_BIT_RATE_RATE_VAL_1_1 << CORE_INT_TX_BIT_RATE_RATE_SHIFT) |
		CORE_INT_TX_BIT_RATE_APPLY_BOTH_TX_RX);

	/* Program bit-width */
	al_serdes_avg_core_interrupt(obj,
		CORE_INT_WIDTH_MODE_REG_NUM, width_field_val_get(bit_width));
}

