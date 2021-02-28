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

#include "al_mod_hal_serdes_25g.h"
#include "al_mod_hal_serdes_25g_regs.h"
#include "al_mod_hal_serdes_25g_internal_regs.h"

#define AL_SERDES_MB_MAX_DATA_LEN		8

#define AL_SERDES_25G_WAIT_FOR_READY_TO		200
#define AL_SERDES_25G_RESET_TO			100
#define AL_SERDES_25G_RESET_NUM_RETRIES		5

#define AL_SERDES_25G_SERDES_INIT_NUM_RETRIES	3

#if (!defined(AL_SERDES_BASIC_SERVICES_ONLY)) || (AL_SERDES_BASIC_SERVICES_ONLY == 0)
#define AL_SRDS_ADV_SRVC(func)			func
#else
static void al_mod_serdes_hssp_stub_func(void)
{
	al_mod_err("%s: not implemented service called!\n", __func__);
}

#define AL_SRDS_ADV_SRVC(func)			((typeof(func) *)al_mod_serdes_hssp_stub_func)
#endif

#define AL_SERDES_25G_UDELAY(usec) (obj->udelay_cb(obj->udelay_cb_context, (usec)))
#define AL_SERDES_25G_MSLEEP(msec) (obj->msleep_cb(obj->msleep_cb_context, (msec)))

/******************************************************************************/
/******************************************************************************/
static void al_mod_serdes_25g_udelay(
		void *context  __attribute__((unused)),
		unsigned int delay)
{
	al_mod_udelay(delay);
}

/******************************************************************************/
/******************************************************************************/
static void al_mod_serdes_25g_msleep(
		void *context  __attribute__((unused)),
		unsigned int delay)
{
	al_mod_msleep(delay);
}

/******************************************************************************/
/******************************************************************************/
static enum al_mod_serdes_type al_mod_serdes_25g_type_get(void)
{
	return AL_SRDS_TYPE_25G;
}

/******************************************************************************/
/******************************************************************************/
static int al_mod_serdes_25g_reg_read(
	struct al_mod_serdes_grp_obj	*obj,
	enum al_mod_serdes_reg_page		page,
	enum al_mod_serdes_reg_type		type,
	uint16_t			offset,
	uint8_t				*data)
{
	struct al_mod_serdes_c_regs __iomem *regs_base =
		(struct al_mod_serdes_c_regs __iomem *)obj->regs_base;
	uint32_t addr = 0;

	al_mod_dbg("%s(%p, %d, %d, %u)\n", __func__, obj, page, type, offset);

	al_mod_assert(obj);
	al_mod_assert(data);

	switch (page) {
	case AL_SRDS_REG_PAGE_TOP:
		addr = (SERDES_25G_TOP_BASE + offset);
		break;
	case AL_SRDS_REG_PAGE_4_COMMON:
		addr = (SERDES_25G_CM_BASE + offset);
		break;
	case AL_SRDS_REG_PAGE_0_LANE_0:
	case AL_SRDS_REG_PAGE_1_LANE_1:
		addr = (SERDES_25G_LANE_BASE + (page * SERDES_25G_LANE_SIZE) + offset);
		break;
	default:
		*data = 0;
		al_mod_err("%s: wrong serdes type %d\n", __func__, type);
		return -1;
	}

	al_mod_reg_write32(&regs_base->gen.reg_addr, addr);
	*data = al_mod_reg_read32(&regs_base->gen.reg_data);

	al_mod_dbg("%s: return(%u)\n", __func__, *data);

	return 0;
}

static int al_mod_serdes_25g_reg_write(
	struct al_mod_serdes_grp_obj	*obj,
	enum al_mod_serdes_reg_page		page,
	enum al_mod_serdes_reg_type		type,
	uint16_t			offset,
	uint8_t				data)
{
	struct al_mod_serdes_c_regs __iomem *regs_base =
		(struct al_mod_serdes_c_regs __iomem *)obj->regs_base;
	uint32_t addr = 0;

	al_mod_dbg("%s(%p, %d, %d, %u)\n", __func__, obj, page, type, offset);

	al_mod_assert(obj);

	switch (page) {
	case AL_SRDS_REG_PAGE_TOP:
		addr = (SERDES_25G_TOP_BASE + offset);
		break;
	case AL_SRDS_REG_PAGE_4_COMMON:
		addr = (SERDES_25G_CM_BASE + offset);
		break;
	case AL_SRDS_REG_PAGE_0_LANE_0:
	case AL_SRDS_REG_PAGE_1_LANE_1:
		addr = (SERDES_25G_LANE_BASE + (page * SERDES_25G_LANE_SIZE) + offset);
		break;
	default:
		al_mod_err("%s: wrong serdes type %d\n", __func__, type);
		return -1;
	}

	al_mod_reg_write32(&regs_base->gen.reg_addr, addr);
	al_mod_reg_write32(&regs_base->gen.reg_data, (data | SERDES_C_GEN_REG_DATA_STRB_MASK));

	al_mod_dbg("%s: write(%u)\n", __func__, data);

	return 0;
}

/******************************************************************************/
/******************************************************************************/
static int al_mod_serdes_25g_reg_masked_read(
	struct al_mod_serdes_grp_obj	*obj,
	enum al_mod_serdes_reg_page		page,
	uint16_t			offset,
	uint8_t				mask,
	uint8_t				shift,
	uint8_t				*data)
{
	uint8_t val;
	int status = 0;

	status = al_mod_serdes_25g_reg_read(obj, page, 0, offset, &val);
	if (status)
		return status;

	*data = AL_REG_FIELD_GET(val, mask, shift);

	return 0;
}

static int al_mod_serdes_25g_reg_masked_write(
	struct al_mod_serdes_grp_obj	*obj,
	enum al_mod_serdes_reg_page		page,
	uint16_t			offset,
	uint8_t				mask,
	uint8_t				shift,
	uint8_t				data)
{
	uint8_t val;
	int status = 0;

	status = al_mod_serdes_25g_reg_read(obj, page, 0, offset, &val);
	if (status)
		return status;

	val &= (~mask);
	val |= (data << shift);
	return al_mod_serdes_25g_reg_write(obj, page, 0, offset, val);
}

/******************************************************************************/
/******************************************************************************/
static int al_mod_serdes_25g_poll(
	struct al_mod_serdes_grp_obj *obj,
	uint32_t			*addr,
	uint8_t				mask,
	uint8_t				value,
	uint32_t			timeout)
{
	uint32_t reg_val = 0;

	while (1) {
		reg_val = al_mod_reg_read32(addr);
		if ((reg_val & mask) == value)
			break;

		AL_SERDES_25G_UDELAY(1);
		timeout--;

		if (timeout == 0)
			return -ETIME;
	}

	return 0;
}

static void al_mod_serdes_set_adaptation_on_sd(
	struct al_mod_serdes_grp_obj	*obj,
	enum al_mod_serdes_reg_page		lane)
{
	al_mod_serdes_25g_reg_masked_write(
				obj,
				lane,
				SERDES_25G_LANE_FEATURE_CTLE_ADAPT_CFG_ADDR,
				SERDES_25G_LANE_FEATURE_CTLE_ADAPT_CFG_REPEAT_COUNT_EIE0_MASK,
				SERDES_25G_LANE_FEATURE_CTLE_ADAPT_CFG_REPEAT_COUNT_EIE0_SHIFT,
				1);

	al_mod_serdes_25g_reg_masked_write(
				obj,
				lane,
				SERDES_25G_LANE_FEATURE_CTLE_ADAPT_AGC_CFG_ADDR,
				SERDES_25G_LANE_FEATURE_CTLE_ADAPT_AGC_CFG_EIE0_EN_MASK,
				SERDES_25G_LANE_FEATURE_CTLE_ADAPT_AGC_CFG_EIE0_EN_SHIFT,
				1);

	al_mod_serdes_25g_reg_masked_write(
				obj,
				lane,
				SERDES_25G_LANE_FEATURE_CTLE_ADAPT_APG_MAP_CFG_ADDR,
				SERDES_25G_LANE_FEATURE_CTLE_ADAPT_APG_MAP_CFG_EIE0_EN_MASK,
				SERDES_25G_LANE_FEATURE_CTLE_ADAPT_APG_MAP_CFG_EIE0_EN_SHIFT,
				1);

	al_mod_serdes_25g_reg_masked_write(
				obj,
				lane,
				SERDES_25G_LANE_FEATURE_CTLE_ADAPT_LFG_CFG_ADDR,
				SERDES_25G_LANE_FEATURE_CTLE_ADAPT_LFG_CFG_EIE0_SEL_MASK,
				SERDES_25G_LANE_FEATURE_CTLE_ADAPT_LFG_CFG_EIE0_SEL_SHIFT,
				1);

	al_mod_serdes_25g_reg_masked_write(
				obj,
				lane,
				SERDES_25G_LANE_FEATURE_CTLE_ADAPT_LFG_CFG_ADDR,
				SERDES_25G_LANE_FEATURE_CTLE_ADAPT_LFG_CFG_EIE1_SEL_MASK,
				SERDES_25G_LANE_FEATURE_CTLE_ADAPT_LFG_CFG_EIE1_SEL_SHIFT,
				1);

	al_mod_serdes_25g_reg_masked_write(
				obj,
				lane,
				SERDES_25G_LANE_FEATURE_CTLE_ADAPT_HFG_CFG0_ADDR,
				SERDES_25G_LANE_FEATURE_CTLE_ADAPT_HFG_CFG0_EIE0_EDGE_EN_MASK,
				SERDES_25G_LANE_FEATURE_CTLE_ADAPT_HFG_CFG0_EIE0_EDGE_EN_SHIFT,
				1);

	al_mod_serdes_25g_reg_masked_write(
				obj,
				lane,
				SERDES_25G_LANE_FEATURE_CTLE_ADAPT_HFG_CFG0_ADDR,
				SERDES_25G_LANE_FEATURE_CTLE_ADAPT_HFG_CFG0_EIE0_DATA_EN_MASK,
				SERDES_25G_LANE_FEATURE_CTLE_ADAPT_HFG_CFG0_EIE0_DATA_EN_SHIFT,
				1);

	al_mod_serdes_25g_reg_masked_write(
				obj,
				lane,
				SERDES_25G_LANE_FEATURE_CTLE_ADAPT_HFG_CFG0_ADDR,
				SERDES_25G_LANE_FEATURE_CTLE_ADAPT_HFG_CFG0_EIE1_EDGE_EN_MASK,
				SERDES_25G_LANE_FEATURE_CTLE_ADAPT_HFG_CFG0_EIE1_EDGE_EN_SHIFT,
				1);

	al_mod_serdes_25g_reg_masked_write(
				obj,
				lane,
				SERDES_25G_LANE_FEATURE_CTLE_ADAPT_HFG_CFG0_ADDR,
				SERDES_25G_LANE_FEATURE_CTLE_ADAPT_HFG_CFG0_EIE1_DATA_EN_MASK,
				SERDES_25G_LANE_FEATURE_CTLE_ADAPT_HFG_CFG0_EIE1_DATA_EN_SHIFT,
				1);

	al_mod_serdes_25g_reg_masked_write(
				obj,
				lane,
				SERDES_25G_LANE_FEATURE_CTLE_ADAPT_HFG_CFG1_ADDR,
				SERDES_25G_LANE_FEATURE_CTLE_ADAPT_HFG_CFG1_EIE0_RESULT_SEL_MASK,
				SERDES_25G_LANE_FEATURE_CTLE_ADAPT_HFG_CFG1_EIE0_RESULT_SEL_SHIFT,
				2);

	al_mod_serdes_25g_reg_masked_write(
				obj,
				lane,
				SERDES_25G_LANE_FEATURE_CTLE_ADAPT_HFG_CFG1_ADDR,
				SERDES_25G_LANE_FEATURE_CTLE_ADAPT_HFG_CFG1_EIE1_RESULT_SEL_MASK,
				SERDES_25G_LANE_FEATURE_CTLE_ADAPT_HFG_CFG1_EIE1_RESULT_SEL_SHIFT,
				2);

	al_mod_serdes_25g_reg_masked_write(
				obj,
				lane,
				SERDES_25G_LANE_FEATURE_DFE_ADAPT_TAP1_CFG_ADDR,
				SERDES_25G_LANE_FEATURE_DFE_ADAPT_TAP1_CFG_TAP1_EIE_EN_MASK,
				SERDES_25G_LANE_FEATURE_DFE_ADAPT_TAP1_CFG_TAP1_EIE_EN_SHIFT,
				1);

	al_mod_serdes_25g_reg_masked_write(
				obj,
				lane,
				SERDES_25G_LANE_FEATURE_DFE_ADAPT_TAP2_CFG_ADDR,
				SERDES_25G_LANE_FEATURE_DFE_ADAPT_TAP2_CFG_TAP2_EIE_EN_MASK,
				SERDES_25G_LANE_FEATURE_DFE_ADAPT_TAP2_CFG_TAP2_EIE_EN_SHIFT,
				1);

	al_mod_serdes_25g_reg_masked_write(
				obj,
				lane,
				SERDES_25G_LANE_FEATURE_DFE_ADAPT_TAP3_CFG_ADDR,
				SERDES_25G_LANE_FEATURE_DFE_ADAPT_TAP3_CFG_TAP3_EIE_EN_MASK,
				SERDES_25G_LANE_FEATURE_DFE_ADAPT_TAP3_CFG_TAP3_EIE_EN_SHIFT,
				1);

	al_mod_serdes_25g_reg_masked_write(
				obj,
				lane,
				SERDES_25G_LANE_FEATURE_DFE_ADAPT_TAP4_CFG_ADDR,
				SERDES_25G_LANE_FEATURE_DFE_ADAPT_TAP4_CFG_TAP4_EIE_EN_MASK,
				SERDES_25G_LANE_FEATURE_DFE_ADAPT_TAP4_CFG_TAP4_EIE_EN_SHIFT,
				1);

	al_mod_serdes_25g_reg_masked_write(
				obj,
				lane,
				SERDES_25G_LANE_FEATURE_DFE_ADAPT_TAP5_CFG_ADDR,
				SERDES_25G_LANE_FEATURE_DFE_ADAPT_TAP5_CFG_TAP5_EIE_EN_MASK,
				SERDES_25G_LANE_FEATURE_DFE_ADAPT_TAP5_CFG_TAP5_EIE_EN_SHIFT,
				1);
}

/******************************************************************************/
/******************************************************************************/
#define SERDES_25G_MB_RESP_BYTES	16
#define SERDES_25G_MB_TIMEOUT		5000000 /* uSec */

static int al_mod_serdes_25g_mailbox_send_cmd(
	struct al_mod_serdes_grp_obj	*obj,
	uint8_t				cmd,
	uint8_t				*data,
	uint8_t				data_len)
{
	uint8_t val;
	int i;
	uint32_t timeout = SERDES_25G_MB_TIMEOUT;

	if (data_len > AL_SERDES_MB_MAX_DATA_LEN) {
		al_mod_err("Cannot send command, data too long\n");
		return -1;
	}

	/* Wait for CMD_FLAG to clear */
	while(1) {
		al_mod_serdes_25g_reg_read(obj, AL_SRDS_REG_PAGE_TOP, 0,
				       SERDES_25G_TOP_CMD_FLAG_ADDR, &val);
		if (val == 0)
			break;

		if (timeout == 0) {
			al_mod_err("%s: timeout occurred waiting to CMD_FLAG\n", __func__);
			return -1;
		}

		timeout--;
		AL_SERDES_25G_UDELAY(1);
	}

	for (i = 0; i < data_len; i++) {
		al_mod_serdes_25g_reg_write(obj, AL_SRDS_REG_PAGE_TOP, 0,
					(SERDES_25G_TOP_CMD_DATA0_ADDR + i), data[i]);
	}

	/* this write will set CMD_FLAG automatically */
	al_mod_serdes_25g_reg_write(obj, AL_SRDS_REG_PAGE_TOP, 0, SERDES_25G_TOP_CMD_ADDR, cmd);

	return 0;
}

static int al_mod_serdes_25g_mailbox_recv_rsp(
	struct al_mod_serdes_grp_obj	*obj,
	uint8_t				*rsp_code,
	uint8_t				*data,
	uint8_t				*data_len)
{
	uint8_t val;
	int i;
	uint32_t timeout = SERDES_25G_MB_TIMEOUT;

	/* wait for RSP_FLAG to set */
	while(1) {
		al_mod_serdes_25g_reg_read(obj, AL_SRDS_REG_PAGE_TOP, 0,
				       SERDES_25G_TOP_RSP_FLAG_ADDR, &val);
		if (val == 0x1)
			break;

		if (timeout == 0) {
			al_mod_err("%s: timeout occurred waiting to RSP_FLAG\n", __func__);
			*data_len = 0;
			return -1;
		}

		timeout--;
		AL_SERDES_25G_UDELAY(1);
	}

	/* Grab the response code and data */
	al_mod_serdes_25g_reg_read(obj, AL_SRDS_REG_PAGE_TOP, 0,
				SERDES_25G_TOP_RSP_ADDR, rsp_code);

	for (i = 0; i < SERDES_25G_MB_RESP_BYTES; i++) {
		al_mod_serdes_25g_reg_read(obj, AL_SRDS_REG_PAGE_TOP, 0,
				(SERDES_25G_TOP_RSP_DATA0_ADDR + i), &data[i]);
	}

	/* clear the RSP_FLAG (write 1 to clear) */
	al_mod_serdes_25g_reg_write(obj, AL_SRDS_REG_PAGE_TOP, 0,
				SERDES_25G_TOP_RSP_FLAG_ADDR, 0x1);

	*data_len = SERDES_25G_MB_RESP_BYTES;

	return 0;
}

/******************************************************************************/
/******************************************************************************/
static void al_mod_serdes_25g_bist_rx_enable(
	struct al_mod_serdes_grp_obj	*obj,
	enum al_mod_serdes_lane		lane,
	al_mod_bool				enable)
{
	if (enable) {
		switch (lane) {
		case 0:
			al_mod_serdes_25g_reg_masked_write(
					obj,
					AL_SRDS_REG_PAGE_TOP,
					SERDES_25G_TOP_CLOCK_LN0_CLK_RX_ADDR,
					SERDES_25G_TOP_CLOCK_LN0_CLK_RX_CTRL_CG_EN_MASK,
					SERDES_25G_TOP_CLOCK_LN0_CLK_RX_CTRL_CG_EN_SHIFT,
					0x1);
			al_mod_serdes_25g_reg_masked_write(
					obj,
					AL_SRDS_REG_PAGE_TOP,
					SERDES_25G_TOP_CLOCK_LN0_CLK_RX_ADDR,
					SERDES_25G_TOP_CLOCK_LN0_CLK_RX_CTRL_BIST_CG_EN_MASK,
					SERDES_25G_TOP_CLOCK_LN0_CLK_RX_CTRL_BIST_CG_EN_SHIFT,
					0x1);
			break;
		case 1:
			al_mod_serdes_25g_reg_masked_write(
					obj,
					AL_SRDS_REG_PAGE_TOP,
					SERDES_25G_TOP_CLOCK_LN1_CLK_RX_ADDR,
					SERDES_25G_TOP_CLOCK_LN1_CLK_RX_CTRL_CG_EN_MASK,
					SERDES_25G_TOP_CLOCK_LN1_CLK_RX_CTRL_CG_EN_SHIFT,
					0x1);

			al_mod_serdes_25g_reg_masked_write(
					obj,
					AL_SRDS_REG_PAGE_TOP,
					SERDES_25G_TOP_CLOCK_LN1_CLK_RX_ADDR,
					SERDES_25G_TOP_CLOCK_LN1_CLK_RX_CTRL_BIST_CG_EN_MASK,
					SERDES_25G_TOP_CLOCK_LN1_CLK_RX_CTRL_BIST_CG_EN_SHIFT,
					0x1);
			break;
		default:
			al_mod_err("%s: Wrong serdes lane %d\n", __func__, lane);
			return;
		}

		al_mod_serdes_25g_reg_masked_write(
				obj,
				(enum al_mod_serdes_reg_page)lane,
				SERDES_25G_LANE_RX_BIST_LOSS_LOCK_CTRL4_ADDR,
				SERDES_25G_LANE_RX_BIST_LOSS_LOCK_CTRL4_STOP_ON_LOSS_LOCK_MASK,
				SERDES_25G_LANE_RX_BIST_LOSS_LOCK_CTRL4_STOP_ON_LOSS_LOCK_SHIFT,
				0);
		al_mod_serdes_25g_reg_masked_write(
				obj,
				(enum al_mod_serdes_reg_page)lane,
				SERDES_25G_LANE_RX_BIST_CTRL_ADDR,
				SERDES_25G_LANE_RX_BIST_CTRL_EN_MASK,
				SERDES_25G_LANE_RX_BIST_CTRL_EN_SHIFT,
				1);
		al_mod_serdes_25g_reg_masked_write(
				obj,
				(enum al_mod_serdes_reg_page)lane,
				SERDES_25G_LANE_RX_BIST_CTRL_ADDR,
				SERDES_25G_LANE_RX_BIST_CTRL_PATTERN_SEL_MASK,
				SERDES_25G_LANE_RX_BIST_CTRL_PATTERN_SEL_SHIFT,
				6);
	} else {
		/* clear counters */
		al_mod_serdes_25g_reg_masked_write(
					obj,
					(enum al_mod_serdes_reg_page)lane,
					SERDES_25G_LANE_RX_BIST_CTRL_ADDR,
					SERDES_25G_LANE_RX_BIST_CTRL_CLEAR_BER_MASK,
					SERDES_25G_LANE_RX_BIST_CTRL_CLEAR_BER_SHIFT,
					1);

		al_mod_serdes_25g_reg_masked_write(
					obj,
					(enum al_mod_serdes_reg_page)lane,
					SERDES_25G_LANE_RX_BIST_CTRL_ADDR,
					SERDES_25G_LANE_RX_BIST_CTRL_CLEAR_BER_MASK,
					SERDES_25G_LANE_RX_BIST_CTRL_CLEAR_BER_SHIFT,
					0);

		AL_SERDES_25G_MSLEEP(AL_SERDES_25G_WAIT_FOR_READY_TO);

		/* disable */
		al_mod_serdes_25g_reg_masked_write(
				obj,
				(enum al_mod_serdes_reg_page)lane,
				SERDES_25G_LANE_RX_BIST_CTRL_ADDR,
				SERDES_25G_LANE_RX_BIST_CTRL_EN_MASK,
				SERDES_25G_LANE_RX_BIST_CTRL_EN_SHIFT,
				0);
	}
}

// TODO: [Guy] change API to be per lane.
static void al_mod_serdes_25g_bist_pattern_select(
	struct al_mod_serdes_grp_obj	*obj,
	enum al_mod_serdes_bist_pattern	pattern,
	uint8_t				*user_data)
{
	enum al_mod_serdes_lane lane;
	uint8_t val = 0;

	switch (pattern) {
	case AL_SRDS_BIST_PATTERN_USER:
		al_mod_assert(user_data);
		val = SERDES_25G_LANE_TX_BIST_CTRL_PATTERN_PRBS_USER;
		break;
	case AL_SRDS_BIST_PATTERN_PRBS7:
		val = SERDES_25G_LANE_TX_BIST_CTRL_PATTERN_PRBS7;
		break;
	case AL_SRDS_BIST_PATTERN_PRBS9:
		val = SERDES_25G_LANE_TX_BIST_CTRL_PATTERN_PRBS9;
		break;
	case AL_SRDS_BIST_PATTERN_PRBS23:
		val = SERDES_25G_LANE_TX_BIST_CTRL_PATTERN_PRBS23;
		break;
	case AL_SRDS_BIST_PATTERN_PRBS31:
		val = SERDES_25G_LANE_TX_BIST_CTRL_PATTERN_PRBS31;
		break;
	case AL_SRDS_BIST_PATTERN_CLK1010:
	default:
		al_mod_err("%s: invalid pattern (%d)\n", __func__, pattern);
		al_mod_assert(0);
	}

	for (lane = AL_SRDS_LANE_0; lane <= AL_SRDS_LANE_1; lane++) {
		if (pattern == AL_SRDS_BIST_PATTERN_USER) {
			int i;

			for (i = 0; i < SERDES_25G_LANE_TX_BIST_UDP_NUM_BYTES; i++)
				al_mod_serdes_25g_reg_write(
						obj,
						(enum al_mod_serdes_reg_page)lane,
						0,
						SERDES_25G_LANE_TX_BIST_UDP_ADDR(i),
						user_data[i]);
		}

		al_mod_serdes_25g_reg_masked_write(
					obj,
					(enum al_mod_serdes_reg_page)lane,
					SERDES_25G_LANE_TX_BIST_CTRL_ADDR,
					SERDES_25G_LANE_TX_BIST_CTRL_PATTERN_SEL_MASK,
					SERDES_25G_LANE_TX_BIST_CTRL_PATTERN_SEL_SHIFT,
					val);
	}
}

static void al_mod_serdes_25g_bist_tx_enable(
	struct al_mod_serdes_grp_obj	*obj,
	enum al_mod_serdes_lane		lane,
	al_mod_bool				enable)
{
	if (enable) {
		al_mod_serdes_25g_reg_masked_write(
					obj,
					(enum al_mod_serdes_reg_page)lane,
					SERDES_25G_LANE_TX_BIST_CTRL_ADDR,
					SERDES_25G_LANE_TX_BIST_CTRL_EN_MASK,
					SERDES_25G_LANE_TX_BIST_CTRL_EN_SHIFT,
					0x1);
		al_mod_serdes_25g_reg_masked_write(
					obj,
					(enum al_mod_serdes_reg_page)lane,
					SERDES_25G_LANE_TOP_DPL_TXDP_CTRL1_ADDR,
					SERDES_25G_LANE_TOP_DPL_TXDP_CTRL1_DMUX_TXA_SEL_MASK,
					SERDES_25G_LANE_TOP_DPL_TXDP_CTRL1_DMUX_TXA_SEL_SHIFT,
					0x2);

		switch (lane) {
		case AL_SRDS_LANE_0:
			al_mod_serdes_25g_reg_masked_write(
					obj,
					AL_SRDS_REG_PAGE_TOP,
					SERDES_25G_TOP_CLOCK_LN0_CLK_TX_ADDR,
					SERDES_25G_TOP_CLOCK_LN0_CLK_TX_CTRL_BIST_CG_EN_MASK,
					SERDES_25G_TOP_CLOCK_LN0_CLK_TX_CTRL_BIST_CG_EN_SHIFT,
					0x1);
			break;
		case AL_SRDS_LANE_1:
			al_mod_serdes_25g_reg_masked_write(
					obj,
					AL_SRDS_REG_PAGE_TOP,
					SERDES_25G_TOP_CLOCK_LN1_CLK_TX_ADDR,
					SERDES_25G_TOP_CLOCK_LN1_CLK_TX_CTRL_BIST_CG_EN_MASK,
					SERDES_25G_TOP_CLOCK_LN1_CLK_TX_CTRL_BIST_CG_EN_SHIFT,
					0x1);
			break;
		default:
			al_mod_err("%s: Wrong serdes lane %d\n", __func__, lane);
				return;
		}
	} else {
		al_mod_serdes_25g_reg_masked_write(
					obj,
					(enum al_mod_serdes_reg_page)lane,
					SERDES_25G_LANE_TX_BIST_CTRL_ADDR,
					SERDES_25G_LANE_TX_BIST_CTRL_EN_MASK,
					SERDES_25G_LANE_TX_BIST_CTRL_EN_SHIFT,
					0);
	}

}

static void al_mod_serdes_25g_bist_rx_status(
	struct al_mod_serdes_grp_obj	*obj,
	enum al_mod_serdes_lane		lane,
	al_mod_bool				*is_locked,
	al_mod_bool				*err_cnt_overflow,
	uint32_t			*err_cnt)
{
	uint8_t status;
	uint8_t err1;
	uint8_t err2;
	uint8_t err3;
	int ret;

	ret = al_mod_serdes_25g_reg_masked_read(
		obj,
		(enum al_mod_serdes_reg_page)lane,
		SERDES_25G_LANE_RX_BIST_STATUS_ADDR,
		SERDES_25G_LANE_RX_BIST_STATUS_STATE_MASK,
		SERDES_25G_LANE_RX_BIST_STATUS_STATE_SHIFT,
		&status);
	if (ret) {
		al_mod_err("%s: Error on reading status state\n", __func__);
		return;
	}

	if (status != 3) {
		*is_locked = AL_FALSE;
		return;
	}

	*is_locked = AL_TRUE;
	*err_cnt_overflow = AL_FALSE;

	ret = al_mod_serdes_25g_reg_masked_read(
		obj,
		(enum al_mod_serdes_reg_page)lane,
		SERDES_25G_LANE_RX_BIST_BER_STATUS0_ADDR,
		SERDES_25G_LANE_RX_BIST_BER_STATUS0_BIT_ERROR_COUNT_7_0_MASK,
		SERDES_25G_LANE_RX_BIST_BER_STATUS0_BIT_ERROR_COUNT_7_0_SHIFT,
		&err1);
	if (ret) {
		al_mod_err("%s: Error on reading error 1\n", __func__);
		return;
	}

	ret = al_mod_serdes_25g_reg_masked_read(
		obj,
		(enum al_mod_serdes_reg_page)lane,
		SERDES_25G_LANE_RX_BIST_BER_STATUS1_ADDR,
		SERDES_25G_LANE_RX_BIST_BER_STATUS1_BIT_ERROR_COUNT_15_8_MASK,
		SERDES_25G_LANE_RX_BIST_BER_STATUS1_BIT_ERROR_COUNT_15_8_SHIFT,
		&err2);
	if (ret) {
		al_mod_err("%s: Error on reading error 2\n", __func__);
		return;
	}

	ret = al_mod_serdes_25g_reg_masked_read(
		obj,
		(enum al_mod_serdes_reg_page)lane,
		SERDES_25G_LANE_RX_BIST_BER_STATUS2_ADDR,
		SERDES_25G_LANE_RX_BIST_BER_STATUS2_BIT_ERROR_COUNT_23_16_MASK,
		SERDES_25G_LANE_RX_BIST_BER_STATUS2_BIT_ERROR_COUNT_23_16_SHIFT,
		&err3);
	if (ret) {
		al_mod_err("%s: Error on reading error 3\n", __func__);
		return;
	}

	*err_cnt = (err1 + (err2 << 8) + (err3 << 16));
}

#define SERDES_MB_CMD_SWING_CFG		0x83
#define SERDES_MB_CMD_SAMPLES_COUNT	0x84
#define SERDES_MB_CMD_START_MEASURE	0x82

#define SERDES_MB_RSP_CODE_0		0
#define SERDES_MB_RSP_CODE_1		1
#define SERDES_MB_RSP_CODE_2		2

static int al_mod_serdes_25g_eye_diag_run(
	struct al_mod_serdes_grp_obj	*obj,
	enum al_mod_serdes_lane		lane,
	int				x_start,
	int				x_stop,
	unsigned int			x_step,
	int				y_start,
	int				y_stop,
	unsigned int			y_step,
	uint64_t			ber_target,
	uint64_t			*buf,
	uint32_t			buf_size)
{
	int rc;
	uint8_t rsp_code;
	uint8_t data[16];
	uint8_t data_len;
	uint32_t total_bits;
	uint8_t bits_left_curr_sample;
	uint8_t bits_left_curr_byte;
	uint32_t byte = 0;
	uint32_t x = 0;
	uint32_t x_samples = (((x_stop - x_start) / x_step) + 1);
	uint32_t y = 0;
	uint32_t y_samples = (((y_stop - y_start) / y_step) + 1);
	uint8_t sample_width = (64 - __builtin_clzll(ber_target));
	uint8_t msb;
	uint8_t lsb;
	uint32_t samples_left = ((x_samples * y_samples));
	uint8_t sign = 0;

	al_mod_assert(buf_size == (samples_left * sizeof(uint64_t)));

	al_mod_memset(buf, 0, buf_size);

	if (y_start < 0) {
		y_start *= -1;
		sign |= 0x1;
	}

	if (y_stop < 0) {
		y_stop *= -1;
		sign |= 0x2;
	}

	data[0] = lane;
	data[1] = x_start;
	data[2] = x_stop;
	data[3] = x_step;
	data[4] = y_start;
	data[5] = y_stop;
	data[6] = sign;
	data[7] = y_step;

	rc = al_mod_serdes_25g_mailbox_send_cmd(
				obj,
				SERDES_MB_CMD_SWING_CFG,
				data,
				8);

	if (rc) {
		al_mod_err("%s: Failed to send command %d to mailbox.\n",
			__func__, SERDES_MB_CMD_SWING_CFG);
		return rc;
	}

	rc = al_mod_serdes_25g_mailbox_recv_rsp(
				obj,
				&rsp_code,
				data,
				&data_len);

	if ((rc) || (rsp_code != SERDES_MB_RSP_CODE_0)) {
		al_mod_err("%s: Failed to send command %d to mailbox. rsp_code %d\n",
			__func__, SERDES_MB_CMD_SWING_CFG, rsp_code);

		return -ETIME;
	}

	al_mod_assert(sample_width <= 40);

	data[0] = lane;
	data[1] = ((ber_target >> 32) & 0xFF);
	data[2] = ((ber_target >> 24) & 0xFF);
	data[3] = ((ber_target >> 16) & 0xFF);
	data[4] = ((ber_target >> 8) & 0xFF);
	data[5] = (ber_target & 0xFF);

	rc = al_mod_serdes_25g_mailbox_send_cmd(
				obj,
				SERDES_MB_CMD_SAMPLES_COUNT,
				data,
				6);

	if (rc) {
		al_mod_err("%s: Failed to send command %d to mailbox.\n",
			__func__, SERDES_MB_CMD_SAMPLES_COUNT);
		return rc;
	}

	rc = al_mod_serdes_25g_mailbox_recv_rsp(
				obj,
				&rsp_code,
				data,
				&data_len);

	if ((rc) || (rsp_code != SERDES_MB_RSP_CODE_0)) {
		al_mod_err("%s: Failed to send command %d to mailbox. rsp_code %d\n",
			__func__, SERDES_MB_CMD_SAMPLES_COUNT, rsp_code);

		return -ETIME;
	}

	rc = al_mod_serdes_25g_mailbox_send_cmd(
				obj,
				SERDES_MB_CMD_START_MEASURE,
				data,
				0);

	bits_left_curr_sample = sample_width;

	while (rsp_code != SERDES_MB_RSP_CODE_1) {
		uint8_t num_bits = 0;

		rc = al_mod_serdes_25g_mailbox_recv_rsp(
				obj,
				&rsp_code,
				data,
				&data_len);

		if ((rc != 0) || (rsp_code > SERDES_MB_RSP_CODE_2)) {
			al_mod_err("%s: command %d return failure. rsp_code %d\n",
			__func__, SERDES_MB_CMD_START_MEASURE, rsp_code);

			return -ETIME;
		}
		byte = 0;
		total_bits = data_len * 8;
		bits_left_curr_byte = 8;
		while (total_bits > 0) {
			num_bits = al_mod_min_t(uint8_t, bits_left_curr_sample, bits_left_curr_byte);

			buf[(y * x_samples) + x] <<= num_bits;
			msb = bits_left_curr_byte - 1;
			lsb = msb - num_bits + 1;
			buf[(y * x_samples) + x] |= ((data[byte] & AL_FIELD_MASK(msb, lsb)) >> lsb);

			total_bits -= num_bits;

			bits_left_curr_byte -= num_bits;
			if (!bits_left_curr_byte) {
				bits_left_curr_byte = 8;
				byte++;
			}

			bits_left_curr_sample -= num_bits;
			if (!bits_left_curr_sample) {
				y++;
				if (y == y_samples) {
					y = 0;
					x++;
				}

				samples_left--;
				bits_left_curr_sample = sample_width;
			}

			if (samples_left == 0)
				break;
		}

		if ((samples_left == 0) && (rsp_code != SERDES_MB_RSP_CODE_1)) {
			rc = al_mod_serdes_25g_mailbox_recv_rsp(
						obj,
						&rsp_code,
						data,
						&data_len);
			if ((rc) || (rsp_code == SERDES_MB_RSP_CODE_0)) {
				al_mod_err("%s: Parsed enough samples but f/w is still sending more\n",
					__func__);

				return -EIO;
			}
			break;
		}
	}

	if (samples_left > 0) {
		al_mod_err("%s: Still need more samples but f/w has stopped sending them!?!?!?\n",
			__func__);

		return -EIO;
	}

	return 0;
}

#define SERDES_25G_EYE_X_MIN		1
#define SERDES_25G_EYE_X_MAX		127
#define SERDES_25G_EYE_Y_MIN		-200
#define SERDES_25G_EYE_Y_MAX		200
#define SERDES_25G_EYE_SIZE_MAX_SAMPLES	401
#define SERDES_25G_EYE_SIZE_BER_TARGET	0xffff
#define SERDES_25G_EYE_SIZE_ERR_TH	10

static int al_mod_serdes_25g_calc_eye_size(
		struct al_mod_serdes_grp_obj	*obj,
		enum al_mod_serdes_lane		lane,
		int				*width,
		int				*height)
{
	uint64_t samples[SERDES_25G_EYE_SIZE_MAX_SAMPLES];
	int i;
	int _width = 0;
	int _height = 0;
	int rc;
	int mid_x = ((SERDES_25G_EYE_X_MIN + SERDES_25G_EYE_X_MAX) / 2);
	int mid_y = ((SERDES_25G_EYE_Y_MIN + SERDES_25G_EYE_Y_MAX) / 2);

	*height = 0;
	*width = 0;

	rc = al_mod_serdes_25g_eye_diag_run(obj,
					lane,
					mid_x,
					mid_x,
					1,
					SERDES_25G_EYE_Y_MIN,
					SERDES_25G_EYE_Y_MAX,
					1,
					SERDES_25G_EYE_SIZE_BER_TARGET,
					samples,
					((SERDES_25G_EYE_Y_MAX - SERDES_25G_EYE_Y_MIN + 1) *
					  sizeof(uint64_t)));

	if (rc) {
		al_mod_err("%s: failed to run eye_diag\n", __func__);
		return rc;
	}

	for (i = (mid_y - SERDES_25G_EYE_Y_MIN);
		((i < (SERDES_25G_EYE_Y_MAX - SERDES_25G_EYE_Y_MIN + 1)) &&
				(samples[i] < SERDES_25G_EYE_SIZE_ERR_TH));
		i++, (_height)++)
		;
	for (i = (mid_y - SERDES_25G_EYE_Y_MIN);
		((i >= 0) && (samples[i] < SERDES_25G_EYE_SIZE_ERR_TH));
		i--, (_height)++)
		;

	rc = al_mod_serdes_25g_eye_diag_run(obj,
					lane,
					SERDES_25G_EYE_X_MIN,
					SERDES_25G_EYE_X_MAX,
					1,
					mid_y,
					mid_y,
					1,
					SERDES_25G_EYE_SIZE_BER_TARGET,
					samples,
					((SERDES_25G_EYE_X_MAX - SERDES_25G_EYE_X_MIN + 1) *
					  sizeof(uint64_t)));

	if (rc) {
		al_mod_err("%s: failed to run eye_diag\n", __func__);
		return rc;
	}

	for (i = (mid_x - SERDES_25G_EYE_X_MIN);
		((samples[i] < SERDES_25G_EYE_SIZE_ERR_TH) &&
			(i < (SERDES_25G_EYE_X_MAX - SERDES_25G_EYE_X_MIN + 1)));
		i++, (_width)++)
		;
	for (i = (mid_x - SERDES_25G_EYE_X_MIN);
		((samples[i] < SERDES_25G_EYE_SIZE_ERR_TH) && (i >= 0));
		i--, (_width)++)
		;

	*height = _height;
	*width = _width;

	return 0;
}


static void al_mod_serdes_25g_tx_advanced_params_set(struct al_mod_serdes_grp_obj	*obj,
					enum al_mod_serdes_lane			lane,
					void					*tx_params)
{
	struct al_mod_serdes_adv_tx_params *params = (struct al_mod_serdes_adv_tx_params *)tx_params;
	uint32_t timeout = 5000;
	uint8_t val = 0;

	al_mod_serdes_25g_reg_masked_write(obj,
					(enum al_mod_serdes_reg_page)lane,
					SERDES_25G_LANE_DRV_TXEQ_CTRL3_ADDR,
					SERDES_25G_LANE_DRV_TXEQ_CTRL3_TXEQ_CM1_MASK,
					SERDES_25G_LANE_DRV_TXEQ_CTRL3_TXEQ_CM1_SHIFT,
					params->c_minus_1);

	al_mod_serdes_25g_reg_masked_write(obj,
					(enum al_mod_serdes_reg_page)lane,
					SERDES_25G_LANE_DRV_TXEQ_CTRL1_ADDR,
					SERDES_25G_LANE_DRV_TXEQ_CTRL1_TXEQ_C1_MASK,
					SERDES_25G_LANE_DRV_TXEQ_CTRL1_TXEQ_C1_SHIFT,
					params->c_plus_1);

	al_mod_serdes_25g_reg_masked_write(obj,
					(enum al_mod_serdes_reg_page)lane,
					SERDES_25G_LANE_DRV_TXEQ_CTRL5_ADDR,
					SERDES_25G_LANE_DRV_TXEQ_CTRL5_DRV_SWING_MASK,
					SERDES_25G_LANE_DRV_TXEQ_CTRL5_DRV_SWING_SHIFT,
					params->total_driver_units);

	al_mod_serdes_25g_reg_masked_write(obj,
					(enum al_mod_serdes_reg_page)lane,
					SERDES_25G_LANE_DRV_TXEQ_CTRL0_ADDR,
					SERDES_25G_LANE_DRV_TXEQ_CTRL0_REQ_MASK,
					SERDES_25G_LANE_DRV_TXEQ_CTRL0_REQ_SHIFT,
					1);


	/* wait for acknowledge */
	while (1) {
		al_mod_serdes_25g_reg_masked_read(obj,
					(enum al_mod_serdes_reg_page)lane,
					SERDES_25G_LANE_DRV_TXEQ_STATUS0_ADDR,
					SERDES_25G_LANE_DRV_TXEQ_STATUS0_ACK_MASK,
					SERDES_25G_LANE_DRV_TXEQ_STATUS0_ACK_SHIFT,
					&val);
		if (val == 1)
			break;

		if (timeout == 0) {
			al_mod_err("%s: timeout occurred waiting to FW ack\n", __func__);
			break;
		}

		timeout--;
		AL_SERDES_25G_UDELAY(1);
	}

	al_mod_serdes_25g_reg_masked_write(obj,
					(enum al_mod_serdes_reg_page)lane,
					SERDES_25G_LANE_DRV_TXEQ_CTRL0_ADDR,
					SERDES_25G_LANE_DRV_TXEQ_CTRL0_REQ_MASK,
					SERDES_25G_LANE_DRV_TXEQ_CTRL0_REQ_SHIFT,
					0);
}

static void al_mod_serdes_25g_tx_advanced_params_get(struct al_mod_serdes_grp_obj	*obj,
		enum al_mod_serdes_lane			lane,
		void					*tx_params)
{
	struct al_mod_serdes_adv_tx_params *params = (struct al_mod_serdes_adv_tx_params *)tx_params;

	al_mod_serdes_25g_reg_masked_read(obj,
					(enum al_mod_serdes_reg_page)lane,
					SERDES_25G_LANE_DRV_TXEQ_CTRL3_ADDR,
					SERDES_25G_LANE_DRV_TXEQ_CTRL3_TXEQ_CM1_MASK,
					SERDES_25G_LANE_DRV_TXEQ_CTRL3_TXEQ_CM1_SHIFT,
					&params->c_minus_1);

	al_mod_serdes_25g_reg_masked_read(obj,
					(enum al_mod_serdes_reg_page)lane,
					SERDES_25G_LANE_DRV_TXEQ_CTRL1_ADDR,
					SERDES_25G_LANE_DRV_TXEQ_CTRL1_TXEQ_C1_MASK,
					SERDES_25G_LANE_DRV_TXEQ_CTRL1_TXEQ_C1_SHIFT,
					&params->c_plus_1);

	al_mod_serdes_25g_reg_masked_read(obj,
					(enum al_mod_serdes_reg_page)lane,
					SERDES_25G_LANE_DRV_TXEQ_CTRL5_ADDR,
					SERDES_25G_LANE_DRV_TXEQ_CTRL5_DRV_SWING_MASK,
					SERDES_25G_LANE_DRV_TXEQ_CTRL5_DRV_SWING_SHIFT,
					&params->total_driver_units);
}

static al_mod_bool al_mod_serdes_25g_cdr_is_locked(
		struct al_mod_serdes_grp_obj	*obj,
		enum al_mod_serdes_lane		lane)
{
	uint8_t reg;
	int ret;

	ret = al_mod_serdes_25g_reg_masked_read(obj,
				(enum al_mod_serdes_reg_page)lane,
				SERDES_25G_LANE_CDR_RXCLK_DLPF_STATUS5_ADDR,
				SERDES_25G_LANE_CDR_RXCLK_DLPF_STATUS5_LOCKED_MASK,
				SERDES_25G_LANE_CDR_RXCLK_DLPF_STATUS5_LOCKED_SHIFT,
				&reg);
	if (ret) {
		al_mod_err("%s: Error on reading CDR lock\n", __func__);
		return AL_FALSE;
	}

	return !!reg;

}

static al_mod_bool al_mod_serdes_25g_rx_valid(
		struct al_mod_serdes_grp_obj	*obj,
		enum al_mod_serdes_lane		lane)
{
	uint8_t reg;
	int ret;

	ret = al_mod_serdes_25g_reg_masked_read(obj,
				(enum al_mod_serdes_reg_page)lane,
				SERDES_25G_LANE_TOP_LN_STAT_CTRL0_ADDR,
				SERDES_25G_LANE_TOP_LN_STAT_CTRL0_RXVALID_MASK,
				SERDES_25G_LANE_TOP_LN_STAT_CTRL0_RXVALID_SHIFT,
				&reg);
	if (ret) {
		al_mod_err("%s: Error on reading RX valid\n", __func__);
		return AL_FALSE;
	}

	return !!reg;

}

static al_mod_bool al_mod_serdes_25g_signal_is_detected(
		struct al_mod_serdes_grp_obj	*obj,
		enum al_mod_serdes_lane		lane)
{
	struct al_mod_serdes_c_regs __iomem *regs_base =
		(struct al_mod_serdes_c_regs __iomem *)obj->regs_base;
	uint32_t reg;
	al_mod_bool signal_detect = AL_FALSE;

	reg = al_mod_reg_read32(&regs_base->lane[lane].stat);

	signal_detect = ((reg & (SERDES_C_LANE_STAT_LN_STAT_LOS |
				 SERDES_C_LANE_STAT_LN_STAT_LOS_DEGLITCH)) ?
					AL_FALSE : AL_TRUE);

	return signal_detect;

}

static int al_mod_serdes_25g_rx_equalization(
		struct al_mod_serdes_grp_obj	*obj,
		enum al_mod_serdes_lane		lane)
{
	struct al_mod_serdes_c_regs __iomem *regs_base =
		(struct al_mod_serdes_c_regs __iomem *)obj->regs_base;
	uint32_t ready_mask = (SERDES_C_GEN_STATUS_CM0_RST_PD_READY | SERDES_C_GEN_STATUS_CM0_OK_O);
	uint32_t reset_mask;
	uint32_t timeout;
	uint32_t reg_val;
	uint32_t retries = AL_SERDES_25G_RESET_NUM_RETRIES;
	int status = 0;

	if (lane == 0) {
		ready_mask |= SERDES_C_GEN_STATUS_LN0_RST_PD_READY;
		reset_mask = SERDES_C_GEN_RST_LN0_RST_N;
	} else {
		ready_mask |= SERDES_C_GEN_STATUS_LN1_RST_PD_READY;
		reset_mask = SERDES_C_GEN_RST_LN1_RST_N;
	}

	while (retries > 0) {
		timeout = AL_SERDES_25G_WAIT_FOR_READY_TO;
		status = 0;

		al_mod_reg_write32_masked(&regs_base->gen.rst, reset_mask, 0);

		AL_SERDES_25G_MSLEEP(AL_SERDES_25G_RESET_TO);

		al_mod_serdes_25g_reg_masked_write(obj,
					(enum al_mod_serdes_reg_page)lane,
					SERDES_25G_LANE_FEATURE_CTLE_ADAPT_MBS_CFG_ADDR,
					SERDES_25G_LANE_FEATURE_CTLE_ADAPT_MBS_CFG_INIT0_EN_MASK,
					SERDES_25G_LANE_FEATURE_CTLE_ADAPT_MBS_CFG_INIT0_EN_SHIFT,
					0);

		al_mod_serdes_25g_reg_masked_write(obj,
					(enum al_mod_serdes_reg_page)lane,
					SERDES_25G_LANE_LEQ_REFCLK_EQ_MB_CTRL1_ADDR,
					SERDES_25G_LANE_LEQ_REFCLK_EQ_MB_CTRL1_EQ_MBF_START_MASK,
					SERDES_25G_LANE_LEQ_REFCLK_EQ_MB_CTRL1_EQ_MBF_START_SHIFT,
					7);

		al_mod_serdes_25g_reg_masked_write(obj,
					(enum al_mod_serdes_reg_page)lane,
					SERDES_25G_LANE_LEQ_REFCLK_EQ_MB_CTRL1_ADDR,
					SERDES_25G_LANE_LEQ_REFCLK_EQ_MB_CTRL1_EQ_MBG_START_MASK,
					SERDES_25G_LANE_LEQ_REFCLK_EQ_MB_CTRL1_EQ_MBG_START_SHIFT,
					15);

		AL_SERDES_25G_MSLEEP(AL_SERDES_25G_RESET_TO);

		al_mod_reg_write32_masked(&regs_base->gen.rst, reset_mask, reset_mask);

		while (1) {
			reg_val = al_mod_reg_read32(&regs_base->gen.status);
			if ((reg_val & ready_mask) == ready_mask)
				break;

			AL_SERDES_25G_UDELAY(1);
			timeout--;

			if (timeout == 0) {
				al_mod_err("%s: Timeout waiting for serdes ready\n", __func__);
				status = -ETIME;
				retries--;
				break;
			}
		}

		if (status)
			continue;

		while (1) {
			reg_val = al_mod_reg_read32(&regs_base->lane[lane].stat);
			reg_val &= (SERDES_C_LANE_STAT_LNX_STAT_OK |
				    SERDES_C_LANE_STAT_LN_STAT_RXVALID);
			if (reg_val == (SERDES_C_LANE_STAT_LNX_STAT_OK |
					SERDES_C_LANE_STAT_LN_STAT_RXVALID))
				break;

			AL_SERDES_25G_UDELAY(1);
			timeout--;

			if (timeout == 0) {
				al_mod_err("%s: TO waiting for lane ready (%x)\n", __func__, reg_val);
				status = -ETIME;
				retries--;
				break;
			}
		}

		if (status)
			continue;

		break;
	}

	if (retries == 0) {
		al_mod_err("%s: Failed to run equalization\n", __func__);
		status = -ETIME;
	}

	return status;

}

#define AL_SERDES_25G_GCFSM2_READ_TIMEOUT		2000000 /* uSec */

static int al_mod_serdes_25g_gcfsm2_read(
		struct al_mod_serdes_grp_obj	*obj,
		enum al_mod_serdes_lane		lane,
		uint8_t			offset,
		uint16_t		*data)
{
	int status = 0;
	uint32_t timeout = AL_SERDES_25G_GCFSM2_READ_TIMEOUT;
	uint8_t ack = 0;
	uint8_t data_low, data_high;
	int ret;

	al_mod_assert(data);

	/* Make sure GCFSM2 REQuest is off */
	al_mod_serdes_25g_reg_masked_write(
			obj,
			(enum al_mod_serdes_reg_page)lane,
			SERDES_25G_LANE_GCFSM2_CMD_CTRL0_ADDR,
			SERDES_25G_LANE_GCFSM2_CMD_CTRL0_REQ_MASK,
			SERDES_25G_LANE_GCFSM2_CMD_CTRL0_REQ_SHIFT,
			0);
	/* Write GCFSM2 CMD; CMD=0 for Read Request */
	al_mod_serdes_25g_reg_masked_write(
			obj,
			(enum al_mod_serdes_reg_page)lane,
			SERDES_25G_LANE_GCFSM2_CMD_CTRL1_ADDR,
			SERDES_25G_LANE_GCFSM2_CMD_CTRL1_CMD_MASK,
			SERDES_25G_LANE_GCFSM2_CMD_CTRL1_CMD_SHIFT,
			0);
	/* Write GCFSM2 the Address we wish to read */
	al_mod_serdes_25g_reg_write(
			obj,
			(enum al_mod_serdes_reg_page)lane,
			0,
			SERDES_25G_LANE_GCFSM2_CMD_CTRL2_ADDR,
			offset);
	/* Issue a command REQuest */
	al_mod_serdes_25g_reg_masked_write(
			obj,
			(enum al_mod_serdes_reg_page)lane,
			SERDES_25G_LANE_GCFSM2_CMD_CTRL0_ADDR,
			SERDES_25G_LANE_GCFSM2_CMD_CTRL0_REQ_MASK,
			SERDES_25G_LANE_GCFSM2_CMD_CTRL0_REQ_SHIFT,
			1);
	/* Poll on GCFSM2 ACK */
	while (1) {
		al_mod_serdes_25g_reg_masked_read(
				obj,
				(enum al_mod_serdes_reg_page)lane,
				SERDES_25G_LANE_GCFSM2_CMD_STATUS_ADDR,
				SERDES_25G_LANE_GCFSM2_CMD_STATUS_ACK_MASK,
				SERDES_25G_LANE_GCFSM2_CMD_STATUS_ACK_SHIFT,
				&ack);

		if (ack || (timeout == 0))
			break;

		timeout--;
		AL_SERDES_25G_UDELAY(1);
	}

	if (ack) {
		/* Read 12bit of register value */
		ret = al_mod_serdes_25g_reg_read(
				obj,
				(enum al_mod_serdes_reg_page)lane,
				0,
				SERDES_25G_LANE_GCFSM2_READ_SHADOW_DATA_STATUS0_ADDR,
				&data_low);
		if (ret) {
			al_mod_err("%s: Error on reading data low\n", __func__);
			return ret;
		}

		ret = al_mod_serdes_25g_reg_masked_read(
				obj,
				(enum al_mod_serdes_reg_page)lane,
				SERDES_25G_LANE_GCFSM2_READ_SHADOW_DATA_STATUS1_ADDR,
				SERDES_25G_LANE_GCFSM2_READ_SHADOW_DATA_STATUS1_11_8_MASK,
				SERDES_25G_LANE_GCFSM2_READ_SHADOW_DATA_STATUS1_11_8_SHIFT,
				&data_high);
		if (ret) {
			al_mod_err("%s: Error on reading data high\n", __func__);
			return ret;
		}

		*data = (data_high << 8) | data_low;
	} else {
		al_mod_err("%s: TO waiting for GCFSM2 req to complete (%x)\n", __func__, offset);
		status = -ETIME;
	}

	/* Deassert the GCFSM2 REQuest */
	al_mod_serdes_25g_reg_masked_write(
			obj,
			(enum al_mod_serdes_reg_page)lane,
			SERDES_25G_LANE_GCFSM2_CMD_CTRL0_ADDR,
			SERDES_25G_LANE_GCFSM2_CMD_CTRL0_REQ_MASK,
			SERDES_25G_LANE_GCFSM2_CMD_CTRL0_REQ_SHIFT,
			0);

	return status;
}

enum al_mod_serdes_25g_rx_leq_fsm_opcode {
	AL_SERDES_25G_RX_LEQ_FSM_OPCODE_READ		= 0x1,
	AL_SERDES_25G_RX_LEQ_FSM_OPCODE_WRITE		= 0x2,
};

enum al_mod_serdes_25g_rx_leq_fsm_target {
	AL_SERDES_25G_RX_LEQ_FSM_TARGET_AGC_SOURCE		= 0x1,
	AL_SERDES_25G_RX_LEQ_FSM_TARGET_PLE_ATT			= 0x2,
	AL_SERDES_25G_RX_LEQ_FSM_TARGET_EQ_LFG			= 0x3,
	AL_SERDES_25G_RX_LEQ_FSM_TARGET_GN_APG			= 0x4,
	AL_SERDES_25G_RX_LEQ_FSM_TARGET_GNEQ_CCL_LFG	= 0x5,
	AL_SERDES_25G_RX_LEQ_FSM_TARGET_HFG_SQL			= 0x6,
	AL_SERDES_25G_RX_LEQ_FSM_TARGET_EQ_MBF			= 0x8,
	AL_SERDES_25G_RX_LEQ_FSM_TARGET_EQ_MBG			= 0x9,
	AL_SERDES_25G_RX_LEQ_FSM_TARGET_VSCAN			= 0xA,
	AL_SERDES_25G_RX_LEQ_FSM_TARGET_HSCAN			= 0xB,
	AL_SERDES_25G_RX_LEQ_FSM_TARGET_EYE_INTF		= 0xC,
};

#define AL_SERDES_25G_RX_LEQ_FSM_TIMEOUT		2000000 /* uSec */

static int al_mod_serdes_25g_rx_leq_fsm_op(
		struct al_mod_serdes_grp_obj	*obj,
		enum al_mod_serdes_lane	lane,
		enum al_mod_serdes_25g_rx_leq_fsm_opcode	opcode,
		enum al_mod_serdes_25g_rx_leq_fsm_target	target,
		uint8_t	val,
		uint8_t	*data,
		uint8_t	*err)
{
	uint32_t reg;
	uint32_t timeout = AL_SERDES_25G_RX_LEQ_FSM_TIMEOUT;
	uint8_t ack = 0;
	int status = 0;

	al_mod_assert(data);
	al_mod_assert(err);

	/* Write the OpCode & Target to LEQ FSM */
	reg = (target << 4) | opcode;
	al_mod_serdes_25g_reg_write(
			obj,
			(enum al_mod_serdes_reg_page)lane,
			0,
			SERDES_25G_LANE_LEQ_REFCLK_LEQ_FSM_CMD0_ADDR,
			reg);

	/* Write 0 as MiscOption value to LEQ FSM */
	al_mod_serdes_25g_reg_write(
			obj,
			(enum al_mod_serdes_reg_page)lane,
			0,
			SERDES_25G_LANE_LEQ_REFCLK_LEQ_FSM_CMD2_ADDR,
			0);

	/* Write the ArgumentValue to LEQ FSM if needed*/
	if (opcode == AL_SERDES_25G_RX_LEQ_FSM_OPCODE_WRITE) {
		al_mod_serdes_25g_reg_write(
				obj,
				(enum al_mod_serdes_reg_page)lane,
				0,
				SERDES_25G_LANE_LEQ_REFCLK_LEQ_FSM_CMD1_ADDR,
				val);
	}

	/* Issue an LEQ FSM Command Request */
	al_mod_serdes_25g_reg_masked_write(
			obj,
			(enum al_mod_serdes_reg_page)lane,
			SERDES_25G_LANE_LEQ_REFCLK_LEQ_FSM_CTRL0_ADDR,
			SERDES_25G_LANE_LEQ_REFCLK_LEQ_FSM_CTRL0_LEQ_FSM_CMD_REQ_MASK,
			SERDES_25G_LANE_LEQ_REFCLK_LEQ_FSM_CTRL0_LEQ_FSM_CMD_REQ_SHIFT,
			1);

	/* Poll on LEQ FSM Command acknowledge */
	while (1) {
		al_mod_serdes_25g_reg_masked_read(
				obj,
				(enum al_mod_serdes_reg_page)lane,
				SERDES_25G_LANE_LEQ_REFCLK_LEQ_FSM_STATUS5_ADDR,
				SERDES_25G_LANE_LEQ_REFCLK_LEQ_FSM_STATUS5_LEQ_FSM_CMD_ACK_MASK,
				SERDES_25G_LANE_LEQ_REFCLK_LEQ_FSM_STATUS5_LEQ_FSM_CMD_ACK_SHIFT,
				&ack);

		if (ack || (timeout == 0))
			break;

		timeout--;
		AL_SERDES_25G_UDELAY(1);
	}

	if (ack) {
		uint8_t err1, err2;
		al_mod_serdes_25g_reg_read(
				obj,
				(enum al_mod_serdes_reg_page)lane,
				0,
				SERDES_25G_LANE_LEQ_REFCLK_LEQ_FSM_STATUS0_ADDR,
				err);

		err1 = (*err &
			SERDES_25G_LANE_LEQ_REFCLK_LEQ_FSM_STATUS0_LEQ_FSM_STATUS_ERROR1_MASK) >>
			SERDES_25G_LANE_LEQ_REFCLK_LEQ_FSM_STATUS0_LEQ_FSM_STATUS_ERROR1_SHIFT;
		err2 = (*err &
			SERDES_25G_LANE_LEQ_REFCLK_LEQ_FSM_STATUS0_LEQ_FSM_STATUS_ERROR2_MASK) >>
			SERDES_25G_LANE_LEQ_REFCLK_LEQ_FSM_STATUS0_LEQ_FSM_STATUS_ERROR2_SHIFT;

		if (err1 || err2) {
			al_mod_err("%s: error in RX LEQ FSM req, err status 1=0x%x, err status 2=0x%x\n",
					__func__, err1, err2);
			status = -EIO;
		}

		/* Read LEQ FSM Command return Value */
		al_mod_serdes_25g_reg_read(
				obj,
				(enum al_mod_serdes_reg_page)lane,
				0,
				SERDES_25G_LANE_LEQ_REFCLK_LEQ_FSM_STATUS3_ADDR,
				data);

		/* Clear an LEQ FSM Command Request */
		al_mod_serdes_25g_reg_masked_write(
				obj,
				(enum al_mod_serdes_reg_page)lane,
				SERDES_25G_LANE_LEQ_REFCLK_LEQ_FSM_CTRL0_ADDR,
				SERDES_25G_LANE_LEQ_REFCLK_LEQ_FSM_CTRL0_LEQ_FSM_CMD_REQ_MASK,
				SERDES_25G_LANE_LEQ_REFCLK_LEQ_FSM_CTRL0_LEQ_FSM_CMD_REQ_SHIFT,
				0);
	} else {
		al_mod_err("%s: TO waiting for RX LEQ FSM req to complete (opcode %x, target %x, val %x)\n",
				__func__, opcode, target, val);
		status = -ETIME;
	}

	return status;
}

/* enum values correspond to HW values, don't change! */
enum al_mod_serdes_25g_tbus_obj {
	AL_SERDES_25G_TBUS_OBJ_TOP	= 0,
	AL_SERDES_25G_TBUS_OBJ_CMU	= 1,
	AL_SERDES_25G_TBUS_OBJ_LANE	= 2,
};

#define AL_SERDES_25G_TBUS_DELAY	1000 /* uSec */
#define AL_SERDES_25G_TBUS_ADDR_HIGH_SHIFT	5

static int al_mod_serdes_25g_tbus_read(
		struct al_mod_serdes_grp_obj	*obj,
		enum al_mod_serdes_lane	lane,
		enum al_mod_serdes_25g_tbus_obj	tbus_obj,
		uint8_t	offset,
		uint16_t	*data)
{
	uint8_t addr_high, val_high, val_low;
	int ret;

	al_mod_assert(lane < AL_SRDS_NUM_LANES);

	if (tbus_obj == AL_SERDES_25G_TBUS_OBJ_TOP)
		addr_high = AL_SERDES_25G_TBUS_OBJ_TOP;
	else if (tbus_obj == AL_SERDES_25G_TBUS_OBJ_CMU)
		addr_high = AL_SERDES_25G_TBUS_OBJ_CMU;
	else
		addr_high = AL_SERDES_25G_TBUS_OBJ_LANE + lane;

	addr_high <<= AL_SERDES_25G_TBUS_ADDR_HIGH_SHIFT;

	al_mod_serdes_25g_reg_write(
			obj,
			AL_SRDS_REG_PAGE_TOP,
			0,
			SERDES_25G_TOP_TBUS_ADDR_7_0_ADDR,
			offset);

	al_mod_serdes_25g_reg_write(
			obj,
			AL_SRDS_REG_PAGE_TOP,
			0,
			SERDES_25G_TOP_TBUS_ADDR_15_8_ADDR,
			addr_high);

	AL_SERDES_25G_UDELAY(AL_SERDES_25G_TBUS_DELAY);

	ret = al_mod_serdes_25g_reg_read(
			obj,
			AL_SRDS_REG_PAGE_TOP,
			0,
			SERDES_25G_TOP_TBUS_DATA_7_0_ADDR,
			&val_low);
	if (ret) {
		al_mod_err("%s: Error on reading low value, ret = %d\n",
		       __func__, ret);
		return ret;
	}

	ret = al_mod_serdes_25g_reg_masked_read(
			obj,
			AL_SRDS_REG_PAGE_TOP,
			SERDES_25G_TOP_TBUS_DATA_11_8_ADDR,
			SERDES_25G_TOP_TBUS_DATA_11_8_MASK,
			SERDES_25G_TOP_TBUS_DATA_11_8_SHIFT,
			&val_high);
	if (ret) {
		al_mod_err("%s: Error on reading high value, ret = %d\n",
		       __func__, ret);
		return ret;
	}

	*data = (val_high << 8) | val_low;

	return 0;
}

#define AL_SERDES_25G_RX_ADV_PARAMS_ATT_MASK	0x07
#define AL_SERDES_25G_RX_ADV_PARAMS_APG_MASK	0x03
#define AL_SERDES_25G_RX_ADV_PARAMS_LFG_MASK	0x1F
#define AL_SERDES_25G_RX_ADV_PARAMS_HFG_MASK	0x1F
#define AL_SERDES_25G_RX_ADV_PARAMS_MBG_MASK	0x0F
#define AL_SERDES_25G_RX_ADV_PARAMS_MBF_MASK	0x0F
#define AL_SERDES_25G_RX_ADV_PARAMS_DFE_TAP_CNT			8
#define AL_SERDES_25G_RX_ADV_PARAMS_DFE_TAP_MASK		0x1F
#define AL_SERDES_25G_RX_ADV_PARAMS_DFE_TAP_SIGN_SHIFT	7

static void al_mod_serdes_25g_rx_advanced_params_get(
		struct al_mod_serdes_grp_obj	*obj,
		enum al_mod_serdes_lane			lane,
		void					*rx_params)
{
	struct al_mod_serdes_25g_adv_rx_params *params =
		(struct al_mod_serdes_25g_adv_rx_params *)rx_params;
	uint8_t value, err;
	int8_t tap_weight;
	uint8_t tap_sign;
	int8_t *tap_ptr_arr[AL_SERDES_25G_RX_ADV_PARAMS_DFE_TAP_CNT];
	int rc;
	int i;

	rc = al_mod_serdes_25g_rx_leq_fsm_op(obj, lane, AL_SERDES_25G_RX_LEQ_FSM_OPCODE_READ,
			AL_SERDES_25G_RX_LEQ_FSM_TARGET_PLE_ATT, 0, &value, &err);
	if (rc || err) {
		al_mod_err("%s: al_mod_serdes_25g_rx_leq_fsm_op failed to read att, rc %d, err %d\n",
				__func__, rc, err);
		return;
	}
	params->att = value & AL_SERDES_25G_RX_ADV_PARAMS_ATT_MASK;

	rc = al_mod_serdes_25g_rx_leq_fsm_op(obj, lane, AL_SERDES_25G_RX_LEQ_FSM_OPCODE_READ,
			AL_SERDES_25G_RX_LEQ_FSM_TARGET_GN_APG, 0, &value, &err);
	if (rc || err) {
		al_mod_err("%s: al_mod_serdes_25g_rx_leq_fsm_op failed to read apg, rc %d, err %d\n",
				__func__, rc, err);
		return;
	}
	params->apg = value & AL_SERDES_25G_RX_ADV_PARAMS_APG_MASK;

	rc = al_mod_serdes_25g_rx_leq_fsm_op(obj, lane, AL_SERDES_25G_RX_LEQ_FSM_OPCODE_READ,
			AL_SERDES_25G_RX_LEQ_FSM_TARGET_EQ_LFG, 0, &value, &err);
	if (rc || err) {
		al_mod_err("%s: al_mod_serdes_25g_rx_leq_fsm_op failed to read lfg, rc %d, err %d\n",
				__func__, rc, err);
		return;
	}
	params->lfg = value & AL_SERDES_25G_RX_ADV_PARAMS_LFG_MASK;

	rc = al_mod_serdes_25g_rx_leq_fsm_op(obj, lane, AL_SERDES_25G_RX_LEQ_FSM_OPCODE_READ,
			AL_SERDES_25G_RX_LEQ_FSM_TARGET_HFG_SQL, 0, &value, &err);
	if (rc || err) {
		al_mod_err("%s: al_mod_serdes_25g_rx_leq_fsm_op failed to read hfg, rc %d, err %d\n",
				__func__, rc, err);
		return;
	}
	params->hfg = value & AL_SERDES_25G_RX_ADV_PARAMS_HFG_MASK;

	rc = al_mod_serdes_25g_rx_leq_fsm_op(obj, lane, AL_SERDES_25G_RX_LEQ_FSM_OPCODE_READ,
			AL_SERDES_25G_RX_LEQ_FSM_TARGET_EQ_MBG, 0, &value, &err);
	if (rc || err) {
		al_mod_err("%s: al_mod_serdes_25g_rx_leq_fsm_op failed to read mbg, rc %d, err %d\n",
				__func__, rc, err);
		return;
	}
	params->mbg = value & AL_SERDES_25G_RX_ADV_PARAMS_MBG_MASK;

	rc = al_mod_serdes_25g_rx_leq_fsm_op(obj, lane, AL_SERDES_25G_RX_LEQ_FSM_OPCODE_READ,
			AL_SERDES_25G_RX_LEQ_FSM_TARGET_EQ_MBF, 0, &value, &err);
	if (rc || err) {
		al_mod_err("%s: al_mod_serdes_25g_rx_leq_fsm_op failed to read mbf, rc %d, err %d\n",
				__func__, rc, err);
		return;
	}
	params->mbf = value & AL_SERDES_25G_RX_ADV_PARAMS_MBF_MASK;

	tap_ptr_arr[0] = &params->dfe_first_tap_even0_ctrl;
	tap_ptr_arr[1] = &params->dfe_first_tap_even1_ctrl;
	tap_ptr_arr[2] = &params->dfe_first_tap_odd0_ctrl;
	tap_ptr_arr[3] = &params->dfe_first_tap_odd1_ctrl;
	tap_ptr_arr[4] = &params->dfe_second_tap_ctrl;
	tap_ptr_arr[5] = &params->dfe_third_tap_ctrl;
	tap_ptr_arr[6] = &params->dfe_fourth_tap_ctrl;
	tap_ptr_arr[7] = &params->dfe_fifth_tap_ctrl;

	for (i = 0; i < AL_SERDES_25G_RX_ADV_PARAMS_DFE_TAP_CNT; i++) {
		al_mod_serdes_25g_reg_read(
				obj,
				(enum al_mod_serdes_reg_page)lane,
				0,
				SERDES_25G_LANE_DFE_REFCLK_TAP_VAL_STATUS0_ADDR + i,
				&value);

		tap_weight = value & AL_SERDES_25G_RX_ADV_PARAMS_DFE_TAP_MASK;
		tap_sign = (value & AL_BIT(AL_SERDES_25G_RX_ADV_PARAMS_DFE_TAP_SIGN_SHIFT)) >>
				AL_SERDES_25G_RX_ADV_PARAMS_DFE_TAP_SIGN_SHIFT;
		if (tap_sign == 0)
			tap_weight = 0 - tap_weight;

		*tap_ptr_arr[i] = tap_weight;
	}
}

#define AL_SERDES_25G_TX_DIAG_GCFSM2_DCD_TRIM_ADDR		0x0B
#define AL_SERDES_25G_TX_DIAG_GCFSM2_DCD_TRIM_MASK		0x3F
#define AL_SERDES_25G_TX_DIAG_GCFSM2_DCD_TRIM_SIGN_SHIFT	7
#define AL_SERDES_25G_TX_DIAG_GCFSM2_CLK_DELAY_ADDR		0x0C
#define AL_SERDES_25G_TX_DIAG_GCFSM2_CLK_DELAY_MASK		0xFFF

static void al_mod_serdes_25g_tx_diag_info_get(
		struct al_mod_serdes_grp_obj	*obj,
		enum al_mod_serdes_lane		lane,
		void *tx_info)
{
	struct al_mod_serdes_25g_tx_diag_info *info =
		(struct al_mod_serdes_25g_tx_diag_info *)tx_info;
	uint8_t cal_x1, cal_x1_fixed, cal_x2, cal_xp5_fixed;
	uint16_t val16, sign;
	uint8_t val8, abs;
	int rc;

	rc = al_mod_serdes_25g_reg_read(
			obj,
			(enum al_mod_serdes_reg_page)lane,
			0,
			SERDES_25G_LANE_TOP_AFE_TXCP_CTRL0_ADDR,
			&val8);
	if (rc) {
		al_mod_err("%s: al_mod_serdes_25g_reg_read failed to read txcp_ctrl0, rc %d\n",
				__func__, rc);
		return;
	}

	info->regulated_supply = val8 & SERDES_25G_LANE_TOP_AFE_TXCP_CTRL0_REG_TXCP_TRIM_MASK;

	rc = al_mod_serdes_25g_gcfsm2_read(
			obj,
			lane,
			AL_SERDES_25G_TX_DIAG_GCFSM2_DCD_TRIM_ADDR,
			&val16);
	if (rc) {
		al_mod_err("%s: al_mod_serdes_25g_gcfsm2_read failed to read dcd_trim, rc %d\n",
				__func__, rc);
		return;
	}

	abs = val16 & AL_SERDES_25G_TX_DIAG_GCFSM2_DCD_TRIM_MASK;
	sign = (val16 & AL_BIT(AL_SERDES_25G_TX_DIAG_GCFSM2_DCD_TRIM_SIGN_SHIFT)) >>
			AL_SERDES_25G_TX_DIAG_GCFSM2_DCD_TRIM_SIGN_SHIFT;
	if (sign)
		info->dcd_trim = abs;
	else
		info->dcd_trim = 0 - abs;

	rc = al_mod_serdes_25g_gcfsm2_read(
			obj,
			lane,
			AL_SERDES_25G_TX_DIAG_GCFSM2_CLK_DELAY_ADDR,
			&val16);
	if (rc) {
		al_mod_err("%s: al_mod_serdes_25g_gcfsm2_read failed to read clk_delay, rc %d\n",
				__func__, rc);
		return;
	}
	info->clk_delay = val16 & AL_SERDES_25G_TX_DIAG_GCFSM2_CLK_DELAY_MASK;

	al_mod_serdes_25g_reg_read(
			obj,
			AL_SRDS_REG_PAGE_4_COMMON,
			0,
			SERDES_25G_CM_TOP_AFE_TXTC_CTRL2_ADDR,
			&val8);
	cal_x1 = (val8 & SERDES_25G_CMU_TOP_AFE_TXTC_CTRL2_TXTC_CALP_X1_MASK) >>
			SERDES_25G_CMU_TOP_AFE_TXTC_CTRL2_TXTC_CALP_X1_SHIFT;
	cal_x1_fixed = (val8 & SERDES_25G_CMU_TOP_AFE_TXTC_CTRL2_TXTC_CALP_X1_FIXED_MASK) >>
			SERDES_25G_CMU_TOP_AFE_TXTC_CTRL2_TXTC_CALP_X1_FIXED_SHIFT;
	al_mod_serdes_25g_reg_read(
			obj,
			AL_SRDS_REG_PAGE_4_COMMON,
			0,
			SERDES_25G_CM_TOP_AFE_TXTC_CTRL3_ADDR,
			&val8);
	cal_x2 = (val8 & SERDES_25G_CMU_TOP_AFE_TXTC_CTRL3_TXTC_CALP_X2_MASK) >>
			SERDES_25G_CMU_TOP_AFE_TXTC_CTRL3_TXTC_CALP_X2_SHIFT;
	cal_xp5_fixed = (val8 &
			SERDES_25G_CMU_TOP_AFE_TXTC_CTRL3_TXTC_CALP_XP5_FIXED_MASK) >>
					SERDES_25G_CMU_TOP_AFE_TXTC_CTRL3_TXTC_CALP_XP5_FIXED_SHIFT;
	info->calp_multiplied_by_2 = 4 * cal_x2 + 2 * cal_x1 + 2 * cal_x1_fixed + cal_xp5_fixed;

	al_mod_serdes_25g_reg_read(
			obj,
			AL_SRDS_REG_PAGE_4_COMMON,
			0,
			SERDES_25G_CM_TOP_AFE_TXTC_CTRL0_ADDR,
			&val8);
	cal_x1 = (val8 & SERDES_25G_CMU_TOP_AFE_TXTC_CTRL0_TXTC_CALN_X1_MASK) >>
			SERDES_25G_CMU_TOP_AFE_TXTC_CTRL0_TXTC_CALN_X1_SHIFT;
	cal_x1_fixed = (val8 & SERDES_25G_CMU_TOP_AFE_TXTC_CTRL0_TXTC_CALN_X1_FIXED_MASK) >>
			SERDES_25G_CMU_TOP_AFE_TXTC_CTRL0_TXTC_CALN_X1_FIXED_SHIFT;
	al_mod_serdes_25g_reg_read(
			obj,
			AL_SRDS_REG_PAGE_4_COMMON,
			0,
			SERDES_25G_CM_TOP_AFE_TXTC_CTRL1_ADDR,
			&val8);
	cal_x2 = (val8 & SERDES_25G_CMU_TOP_AFE_TXTC_CTRL1_TXTC_CALN_X2_MASK) >>
			SERDES_25G_CMU_TOP_AFE_TXTC_CTRL1_TXTC_CALN_X2_SHIFT;
	cal_xp5_fixed = (val8 &
			SERDES_25G_CMU_TOP_AFE_TXTC_CTRL1_TXTC_CALN_XP5_FIXED_MASK) >>
					SERDES_25G_CMU_TOP_AFE_TXTC_CTRL1_TXTC_CALN_XP5_FIXED_SHIFT;
	info->caln_multiplied_by_2 = 4 * cal_x2 + 2 * cal_x1 + 2 * cal_x1_fixed + cal_xp5_fixed;
}

#define AL_SERDES_25G_RX_DIAG_SIGNED_5BIT_ABS_MASK			0x1F
#define AL_SERDES_25G_RX_DIAG_SIGNED_5BIT_MASK				0x3F
#define AL_SERDES_25G_RX_DIAG_SIGNED_5BIT_SIGN_SHIFT		5
#define AL_SERDES_25G_RX_DIAG_SIGNED_5BIT_HIGH_MASK			0xFC0
#define AL_SERDES_25G_RX_DIAG_SIGNED_5BIT_HIGH_SHIFT		6
#define AL_SERDES_25G_RX_DIAG_LEQ_EQ_COUNT					5
#define AL_SERDES_25G_RX_DIAG_GCFSM2_LEQ_EQ_ADDR			0
#define AL_SERDES_25G_RX_DIAG_GCFSM2_LEQ_GAINSTAGE_ADDR		0x5
#define AL_SERDES_25G_RX_DIAG_GCFSM2_SUMMER_EVEN_ADDR		0x6
#define AL_SERDES_25G_RX_DIAG_GCFSM2_SUMMER_ODD_ADDR		0x7
#define AL_SERDES_25G_RX_DIAG_GCFSM2_VSCAN_EVEN_ADDR		0x8
#define AL_SERDES_25G_RX_DIAG_GCFSM2_VSCAN_ODD_ADDR			0x9
#define AL_SERDES_25G_RX_DIAG_GCFSM2_CDR_VCO_FR_ADDR		0xF
#define AL_SERDES_25G_RX_DIAG_GCFSM2_CDR_VCO_FR_MASK		0xFFF
#define AL_SERDES_25G_TBUS_LANE_CDR_VCO_FR_ADDR			0x7
#define AL_SERDES_25G_TBUS_LANE_CDR_VCO_FR_MASK			0x3E0
#define AL_SERDES_25G_TBUS_LANE_CDR_VCO_FR_SHIFT		5
#define AL_SERDES_25G_TBUS_DOSC_ADDR				0x10
#define AL_SERDES_25G_RX_DIAG_TBUS_DATA_SLICER_EVEN_ADDR	0x11
#define AL_SERDES_25G_RX_DIAG_TBUS_DATA_SLICER_ODD_ADDR		0x12
#define AL_SERDES_25G_RX_DIAG_TBUS_EDGE_SLICER_ADDR			0x13
#define AL_SERDES_25G_TBUS_CMU_VCO_SWING_ADDR			0x14
#define AL_SERDES_25G_RX_DIAG_TBUS_EYE_SLICER_ADDR			0x23
#define AL_SERDES_25G_RX_DIAG_TBUS_CDR_CLK_Q_ADDR			0x2
#define AL_SERDES_25G_RX_DIAG_TBUS_CDR_CLK_I_ADDR			0x1
#define AL_SERDES_25G_RX_DIAG_CDR_RXCLK_DLPF_L_ADDR			0x26
#define AL_SERDES_25G_RX_DIAG_CDR_RXCLK_DLPF_H_ADDR			0x27

static inline void al_mod_serdes_25g_rx_diag_5bit_signed_set(uint8_t packed_val, int8_t *ptr)
{
	uint8_t abs, sign;

	abs = packed_val & AL_SERDES_25G_RX_DIAG_SIGNED_5BIT_ABS_MASK;
	sign = (packed_val & AL_BIT(AL_SERDES_25G_RX_DIAG_SIGNED_5BIT_SIGN_SHIFT)) >>
			AL_SERDES_25G_RX_DIAG_SIGNED_5BIT_SIGN_SHIFT;
	if (sign)
		*ptr = abs;
	else
		*ptr = 0 - abs;
}

static void al_mod_serdes_25g_rx_diag_info_get(
		struct al_mod_serdes_grp_obj	*obj,
		enum al_mod_serdes_lane		lane,
		void *rx_info)
{
	struct al_mod_serdes_25g_rx_diag_info *info =
		(struct al_mod_serdes_25g_rx_diag_info *)rx_info;
	uint16_t val16;
	uint8_t val8, val8_2;
	int rc;
	int i;

	al_mod_serdes_25g_reg_read(
			obj,
			(enum al_mod_serdes_reg_page)lane,
			0,
			SERDES_25G_LANE_LOS_REFCLK_CALIBRATION_STATUS0_ADDR,
			&val8);
	al_mod_serdes_25g_rx_diag_5bit_signed_set(val8, &info->los_offset);

	al_mod_serdes_25g_reg_read(
			obj,
			(enum al_mod_serdes_reg_page)lane,
			0,
			SERDES_25G_LANE_LOS_REFCLK_CALIBRATION_STATUS1_ADDR,
			&val8);
	al_mod_serdes_25g_rx_diag_5bit_signed_set(val8, &info->agc_offset);

	rc = al_mod_serdes_25g_gcfsm2_read(
			obj,
			lane,
			AL_SERDES_25G_RX_DIAG_GCFSM2_LEQ_GAINSTAGE_ADDR,
			&val16);
	if (rc) {
		al_mod_err("%s: al_mod_serdes_25g_gcfsm2_read failed to read leq_gainstage, rc %d\n",
				__func__, rc);
		return;
	}
	val8 = (uint8_t)val16;
	al_mod_serdes_25g_rx_diag_5bit_signed_set(val8, &info->leq_gainstage_offset);

	for (i = 0; i < AL_SERDES_25G_RX_DIAG_LEQ_EQ_COUNT; i++) {
		rc = al_mod_serdes_25g_gcfsm2_read(
				obj,
				lane,
				AL_SERDES_25G_RX_DIAG_GCFSM2_LEQ_EQ_ADDR + i,
				&val16);
		if (rc) {
			al_mod_err("%s: al_mod_serdes_25g_gcfsm2_read failed to read leq_eq %d, rc %d\n",
					__func__, i, rc);
			return;
		}
		val8 = (uint8_t)val16;

		switch (i) {
		case 0:
			al_mod_serdes_25g_rx_diag_5bit_signed_set(val8, &info->leq_eq1_offset);
			break;
		case 1:
			al_mod_serdes_25g_rx_diag_5bit_signed_set(val8, &info->leq_eq2_offset);
			break;
		case 2:
			al_mod_serdes_25g_rx_diag_5bit_signed_set(val8, &info->leq_eq3_offset);
			break;
		case 3:
			al_mod_serdes_25g_rx_diag_5bit_signed_set(val8, &info->leq_eq4_offset);
			break;
		case 4:
			al_mod_serdes_25g_rx_diag_5bit_signed_set(val8, &info->leq_eq5_offset);
			break;
		default:
			break;
		}
	}

	rc = al_mod_serdes_25g_gcfsm2_read(
			obj,
			lane,
			AL_SERDES_25G_RX_DIAG_GCFSM2_SUMMER_EVEN_ADDR,
			&val16);
	if (rc) {
		al_mod_err("%s: al_mod_serdes_25g_gcfsm2_read failed to read summer_even_offset, rc %d\n",
				__func__, rc);
		return;
	}
	val8 = (uint8_t)val16;
	al_mod_serdes_25g_rx_diag_5bit_signed_set(val8, &info->summer_even_offset);

	rc = al_mod_serdes_25g_gcfsm2_read(
			obj,
			lane,
			AL_SERDES_25G_RX_DIAG_GCFSM2_SUMMER_ODD_ADDR,
			&val16);
	if (rc) {
		al_mod_err("%s: al_mod_serdes_25g_gcfsm2_read failed to read summer_odd_offset, rc %d\n",
				__func__, rc);
		return;
	}
	val8 = (uint8_t)val16;
	al_mod_serdes_25g_rx_diag_5bit_signed_set(val8, &info->summer_odd_offset);

	rc = al_mod_serdes_25g_gcfsm2_read(
			obj,
			lane,
			AL_SERDES_25G_RX_DIAG_GCFSM2_VSCAN_EVEN_ADDR,
			&val16);
	if (rc) {
		al_mod_err("%s: al_mod_serdes_25g_gcfsm2_read failed to read vscan_even_offset, rc %d\n",
				__func__, rc);
		return;
	}
	val8 = (uint8_t)val16;
	al_mod_serdes_25g_rx_diag_5bit_signed_set(val8, &info->vscan_even_offset);

	rc = al_mod_serdes_25g_gcfsm2_read(
			obj,
			lane,
			AL_SERDES_25G_RX_DIAG_GCFSM2_VSCAN_ODD_ADDR,
			&val16);
	if (rc) {
		al_mod_err("%s: al_mod_serdes_25g_gcfsm2_read failed to read vscan_odd_offset, rc %d\n",
				__func__, rc);
		return;
	}
	val8 = (uint8_t)val16;
	al_mod_serdes_25g_rx_diag_5bit_signed_set(val8, &info->vscan_odd_offset);

	al_mod_serdes_25g_tbus_read(
			obj,
			lane,
			AL_SERDES_25G_TBUS_OBJ_LANE,
			AL_SERDES_25G_RX_DIAG_TBUS_DATA_SLICER_EVEN_ADDR,
			&val16);
	val8 = (uint8_t)(val16 & AL_SERDES_25G_RX_DIAG_SIGNED_5BIT_MASK);
	al_mod_serdes_25g_rx_diag_5bit_signed_set(val8, &info->data_slicer_even0_offset);
	val8 = (uint8_t)((val16 & AL_SERDES_25G_RX_DIAG_SIGNED_5BIT_HIGH_MASK) >>
			AL_SERDES_25G_RX_DIAG_SIGNED_5BIT_HIGH_SHIFT);
	al_mod_serdes_25g_rx_diag_5bit_signed_set(val8, &info->data_slicer_even1_offset);

	al_mod_serdes_25g_tbus_read(
			obj,
			lane,
			AL_SERDES_25G_TBUS_OBJ_LANE,
			AL_SERDES_25G_RX_DIAG_TBUS_DATA_SLICER_ODD_ADDR,
			&val16);
	val8 = (uint8_t)(val16 & AL_SERDES_25G_RX_DIAG_SIGNED_5BIT_MASK);
	al_mod_serdes_25g_rx_diag_5bit_signed_set(val8, &info->data_slicer_odd0_offset);
	val8 = (uint8_t)((val16 & AL_SERDES_25G_RX_DIAG_SIGNED_5BIT_HIGH_MASK) >>
			AL_SERDES_25G_RX_DIAG_SIGNED_5BIT_HIGH_SHIFT);
	al_mod_serdes_25g_rx_diag_5bit_signed_set(val8, &info->data_slicer_odd1_offset);

	al_mod_serdes_25g_tbus_read(
			obj,
			lane,
			AL_SERDES_25G_TBUS_OBJ_LANE,
			AL_SERDES_25G_RX_DIAG_TBUS_EDGE_SLICER_ADDR,
			&val16);
	val8 = (uint8_t)(val16 & AL_SERDES_25G_RX_DIAG_SIGNED_5BIT_MASK);
	al_mod_serdes_25g_rx_diag_5bit_signed_set(val8, &info->edge_slicer_even_offset);
	val8 = (uint8_t)((val16 & AL_SERDES_25G_RX_DIAG_SIGNED_5BIT_HIGH_MASK) >>
			AL_SERDES_25G_RX_DIAG_SIGNED_5BIT_HIGH_SHIFT);
	al_mod_serdes_25g_rx_diag_5bit_signed_set(val8, &info->edge_slicer_odd_offset);

	al_mod_serdes_25g_tbus_read(
			obj,
			lane,
			AL_SERDES_25G_TBUS_OBJ_LANE,
			AL_SERDES_25G_RX_DIAG_TBUS_EYE_SLICER_ADDR,
			&val16);
	val8 = (uint8_t)(val16 & AL_SERDES_25G_RX_DIAG_SIGNED_5BIT_MASK);
	al_mod_serdes_25g_rx_diag_5bit_signed_set(val8, &info->eye_slicer_even_offset);
	val8 = (uint8_t)((val16 & AL_SERDES_25G_RX_DIAG_SIGNED_5BIT_HIGH_MASK) >>
			AL_SERDES_25G_RX_DIAG_SIGNED_5BIT_HIGH_SHIFT);
	al_mod_serdes_25g_rx_diag_5bit_signed_set(val8, &info->eye_slicer_odd_offset);

	al_mod_serdes_25g_reg_masked_read(
			obj,
			(enum al_mod_serdes_reg_page)lane,
			SERDES_25G_LANE_CDR_REFCLK_AFE_PI_CTRL0_ADDR,
			SERDES_25G_LANE_CDR_REFCLK_AFE_PI_CTRL0_RXCDR_HSCAN_CLKQ_MASK,
			SERDES_25G_LANE_CDR_REFCLK_AFE_PI_CTRL0_RXCDR_HSCAN_CLKQ_SHIFT,
			&info->cdr_clk_q);

	al_mod_serdes_25g_reg_masked_read(
			obj,
			(enum al_mod_serdes_reg_page)lane,
			SERDES_25G_LANE_CDR_REFCLK_AFE_PI_CTRL1_ADDR,
			SERDES_25G_LANE_CDR_REFCLK_AFE_PI_CTRL1_RXCDR_HSCAN_CLKI_MASK,
			SERDES_25G_LANE_CDR_REFCLK_AFE_PI_CTRL1_RXCDR_HSCAN_CLKI_SHIFT,
			&info->cdr_clk_i);

	al_mod_serdes_25g_reg_masked_read(
			obj,
			(enum al_mod_serdes_reg_page)lane,
			SERDES_25G_LANE_CDR_REFCLK_AFE_PI_CTRL2_ADDR,
			SERDES_25G_LANE_CDR_REFCLK_AFE_PI_CTRL2_RXCDR_HSCAN_EYE_MASK,
			SERDES_25G_LANE_CDR_REFCLK_AFE_PI_CTRL2_RXCDR_HSCAN_EYE_SHIFT,
			&info->cdr_dll);

	al_mod_serdes_25g_reg_masked_read(
			obj,
			(enum al_mod_serdes_reg_page)lane,
			SERDES_25G_LANE_CDR_REFCLK_AFE_VCO_CTRL2_ADDR,
			SERDES_25G_LANE_CDR_REFCLK_AFE_VCO_CTRL2_RXCDR_DOSC_MASK,
			SERDES_25G_LANE_CDR_REFCLK_AFE_VCO_CTRL2_RXCDR_DOSC_SHIFT,
			&info->cdr_vco_dosc);

	al_mod_serdes_25g_reg_read(
			obj,
			(enum al_mod_serdes_reg_page)lane,
			0,
			SERDES_25G_LANE_CDR_RXCLK_LOAD_MODE_CTRL1_ADDR,
			&val8_2);
	al_mod_serdes_25g_reg_read(
			obj,
			(enum al_mod_serdes_reg_page)lane,
			0,
			SERDES_25G_LANE_CDR_RXCLK_LOAD_MODE_CTRL0_ADDR,
			&val8);
	val8_2 &= SERDES_25G_LANE_CDR_RXCLK_LOAD_MODE_CTRL1_DLPF_VAL_8_MASK;
	info->cdr_dlpf = (uint16_t)val8_2 << 8 | val8;

	rc = al_mod_serdes_25g_gcfsm2_read(
			obj,
			lane,
			AL_SERDES_25G_RX_DIAG_GCFSM2_CDR_VCO_FR_ADDR,
			&val16);
	if (rc) {
		al_mod_err("%s: al_mod_serdes_25g_gcfsm2_read failed to read cdr_vco_fr, rc %d\n",
				__func__, rc);
		return;
	}
	info->cdr_vco_fr = val16 & AL_SERDES_25G_RX_DIAG_GCFSM2_CDR_VCO_FR_MASK;

	al_mod_serdes_25g_reg_masked_read(
			obj,
			(enum al_mod_serdes_reg_page)lane,
			SERDES_25G_LANE_LEQ_REFCLK_AFE_PLE_CTRL0_ADDR,
			SERDES_25G_LANE_LEQ_REFCLK_AFE_PLE_CTRL0_RXLEQ_PLE_BLW_ZERO_MASK,
			SERDES_25G_LANE_LEQ_REFCLK_AFE_PLE_CTRL0_RXLEQ_PLE_BLW_ZERO_SHIFT,
			&info->ple_resistance);

	al_mod_serdes_25g_reg_read(
			obj,
			(enum al_mod_serdes_reg_page)lane,
			0,
			SERDES_25G_LANE_TOP_AFE_RXTERM_CTRL0_ADDR,
			&val8);

	info->rx_term_mode = (val8 & SERDES_25G_LANE_TOP_AFE_RXTERM_CTRL0_RXTERM_HIZ_MASK) >>
			SERDES_25G_LANE_TOP_AFE_RXTERM_CTRL0_RXTERM_HIZ_SHIFT;

	info->rx_coupling = (val8 & SERDES_25G_LANE_TOP_AFE_RXTERM_CTRL0_RXTERM_VCM_GND_MASK) >>
			SERDES_25G_LANE_TOP_AFE_RXTERM_CTRL0_RXTERM_VCM_GND_SHIFT;

	al_mod_serdes_25g_reg_masked_read(
			obj,
			(enum al_mod_serdes_reg_page)lane,
			SERDES_25G_LANE_TOP_AFE_RXTERM_CTRL1_ADDR,
			SERDES_25G_LANE_TOP_AFE_RXTERM_CTRL1_RXTERM_VAL_MASK,
			SERDES_25G_LANE_TOP_AFE_RXTERM_CTRL1_RXTERM_VAL_SHIFT,
			&info->rx_term_cal_code);

	al_mod_serdes_25g_reg_masked_read(
			obj,
			(enum al_mod_serdes_reg_page)lane,
			SERDES_25G_LANE_LEQ_REFCLK_AFE_BIAS_CTRL1_ADDR,
			SERDES_25G_LANE_LEQ_REFCLK_AFE_BIAS_CTRL1_RXLEQ_BIASI_TRIM_MASK,
			SERDES_25G_LANE_LEQ_REFCLK_AFE_BIAS_CTRL1_RXLEQ_BIASI_TRIM_SHIFT,
			&info->rx_sheet_res_cal_code);
}

static al_mod_bool al_mod_serdes_25g_pll_lock_get(struct al_mod_serdes_grp_obj *obj)
{
	uint8_t reg;

	al_mod_serdes_25g_reg_read(
			obj,
			AL_SRDS_REG_PAGE_4_COMMON,
			0,
			SERDES_25G_CMU_PLL_LOCKDET_STATUS_ADDR,
			&reg);

	return !!reg;
}

static void al_mod_serdes_25g_fw_init_status_get(struct al_mod_serdes_grp_obj *obj,
		void *fw_init_status)
{
	struct al_mod_serdes_25g_fw_init_status *status =
		(struct al_mod_serdes_25g_fw_init_status *)fw_init_status;
	enum al_mod_serdes_reg_page lane;
	uint8_t reg;

	al_mod_memset(status, 0, sizeof(struct al_mod_serdes_25g_fw_init_status));

	obj->reg_read(obj, AL_SRDS_REG_PAGE_TOP, 0, SERDES_25G_TOP_ERR_CTRL0_ADDR, &reg);
	if (reg != 0) {
		obj->reg_read(obj, AL_SRDS_REG_PAGE_TOP, 0, SERDES_25G_TOP_ERR_CTRL1_ADDR, &reg);
		status->top_error = reg;
		obj->reg_read(obj, AL_SRDS_REG_PAGE_TOP, 0, SERDES_25G_TOP_ERR_CTRL2_ADDR, &reg);
		status->top_error |= (reg << 8);

		obj->reg_read(obj, AL_SRDS_REG_PAGE_4_COMMON, 0,
				SERDES_25G_CM_TOP_ERR_CTRL1_ADDR, &reg);
		status->cm_error = reg;
		obj->reg_read(obj, AL_SRDS_REG_PAGE_4_COMMON, 0,
				SERDES_25G_CM_TOP_ERR_CTRL2_ADDR, &reg);
		status->cm_error |= (reg << 8);
		obj->reg_read(obj, AL_SRDS_REG_PAGE_4_COMMON, 0,
				SERDES_25G_CM_TOP_ERR_CTRL3_ADDR, &reg);
		status->cm_critical_error = reg;

		for (lane = AL_SRDS_REG_PAGE_0_LANE_0; lane <= AL_SRDS_REG_PAGE_1_LANE_1; lane++) {
			obj->reg_read(obj, lane, 0, SERDES_25G_LANE_TOP_ERR_CTRL1_ADDR, &reg);
			status->lane_error[lane] = reg;
			obj->reg_read(obj, lane, 0, SERDES_25G_LANE_TOP_ERR_CTRL2_ADDR, &reg);
			status->lane_error[lane] |= (reg << 8);
			obj->reg_read(obj, lane, 0, SERDES_25G_LANE_TOP_ERR_CTRL3_ADDR, &reg);
			status->lane_critical_error[lane] = reg;
		}
	}
}

/******************************************************************************/
/********************************* serdes init ********************************/
/******************************************************************************/
#define AL_SERDES_25G_INIT_TIMEOUT		10000 /* uSec */
#define AL_SERDES_25G_RESET_DELAY		10 /* uSec */
#define AL_SERDES_BB_STEP_VALUE_25G		0x12
#define AL_SERDES_BB_STEP_VALUE_10G		0x1C
#define AL_SERDES_25G_QSAMPLE_COUNT		100
#define AL_SERDES_25G_PLL_LOCK_DELAY		100
#define AL_SERDES_25G_PLL_CMD_ACK_TIMEOUT	500

static void al_mod_serdes_25g_pll_loop(struct al_mod_serdes_grp_obj *obj, al_mod_bool open)
{
	uint8_t value;

	value = (open) ? 1 : 0;

	al_mod_serdes_25g_reg_masked_write(
				obj,
				AL_SRDS_REG_PAGE_4_COMMON,
				SERDES_25G_CMU_PLL_AFE_VCO_CTRL1_ADDR,
				SERDES_25G_CMU_PLL_AFE_VCO_CTRL1_CMPLL_INTVAR_ZERO_MASK,
				SERDES_25G_CMU_PLL_AFE_VCO_CTRL1_CMPLL_INTVAR_ZERO_SHIFT,
				value);

	al_mod_serdes_25g_reg_masked_write(
				obj,
				AL_SRDS_REG_PAGE_4_COMMON,
				SERDES_25G_CMU_PLL_AFE_INT_CTRL2_ADDR,
				SERDES_25G_CMU_PLL_AFE_INT_CTRL2_CMPLL_IZERO_MASK,
				SERDES_25G_CMU_PLL_AFE_INT_CTRL2_CMPLL_IZERO_SHIFT,
				value);

	al_mod_serdes_25g_reg_masked_write(
				obj,
				AL_SRDS_REG_PAGE_4_COMMON,
				SERDES_25G_CMU_PLL_AFE_PROP_CTRL1_ADDR,
				SERDES_25G_CMU_PLL_AFE_PROP_CTRL1_CMPLL_PZERO_MASK,
				SERDES_25G_CMU_PLL_AFE_PROP_CTRL1_CMPLL_PZERO_SHIFT,
				value);

	al_mod_serdes_25g_reg_masked_write(
				obj,
				AL_SRDS_REG_PAGE_4_COMMON,
				SERDES_25G_CMU_PLL_AFE_PFD_CTRL1_ADDR,
				SERDES_25G_CMU_PLL_AFE_PFD_CTRL1_CMPLL_MUTE_MASK,
				SERDES_25G_CMU_PLL_AFE_PFD_CTRL1_CMPLL_MUTE_SHIFT,
				value);
}

#define AL_SERDES_DOSC_MIN_VAL		1
#define AL_SERDES_DOSC_MAX_VAL		132
#define AL_SERDES_UPDATE_DOSC_MAX_CYCLES (AL_SERDES_DOSC_MAX_VAL - AL_SERDES_DOSC_MIN_VAL)

static int dosc_convert(int dosc)
{
	if ((dosc % 4) == 0)
		return dosc - 1;
	else if ((dosc % 4) == 3)
		return dosc + 1;
	return dosc;
}

#define AL_SERDES_DOSC_CMD_RECAL	0xC
#define AL_SERDES_DOSC_CMD_SET		0x3

static void al_mod_serdes_25g_single_iteration_dosc_set(struct al_mod_serdes_grp_obj *obj, uint8_t dosc,
		al_mod_bool do_recal)
{
	int32_t rc;
	uint8_t dosc_orig;
	uint8_t ack;
	int timeout = AL_SERDES_25G_PLL_CMD_ACK_TIMEOUT;
	uint8_t cmd = AL_SERDES_DOSC_CMD_SET;

	al_mod_serdes_25g_pll_loop(obj, AL_TRUE);

	rc = al_mod_serdes_25g_reg_masked_read(
			obj,
			AL_SRDS_REG_PAGE_4_COMMON,
			SERDES_25G_CMU_PLL_FCAL_DOSC_START_ADDR,
			SERDES_25G_CMU_PLL_FCAL_DOSC_START_MASK,
			SERDES_25G_CMU_PLL_FCAL_DOSC_START_SHIFT,
			&dosc_orig);

	if (rc) {
		al_mod_assert(0);
		return;
	}

	al_mod_serdes_25g_reg_masked_write(
			obj,
			AL_SRDS_REG_PAGE_4_COMMON,
			SERDES_25G_CMU_PLL_FCAL_DOSC_START_ADDR,
			SERDES_25G_CMU_PLL_FCAL_DOSC_START_MASK,
			SERDES_25G_CMU_PLL_FCAL_DOSC_START_SHIFT,
			dosc);

	if (do_recal)
		cmd = AL_SERDES_DOSC_CMD_RECAL;

	al_mod_serdes_25g_reg_masked_write(
			obj,
			AL_SRDS_REG_PAGE_4_COMMON,
			SERDES_25G_CMU_PLL_FCAL_CMD_ADDR,
			SERDES_25G_CMU_PLL_FCAL_CMD_CMD_I_MASK,
			SERDES_25G_CMU_PLL_FCAL_CMD_CMD_I_SHIFT,
			cmd);

	al_mod_serdes_25g_reg_masked_write(
			obj,
			AL_SRDS_REG_PAGE_4_COMMON,
			SERDES_25G_CMU_PLL_FCAL_CMD_ADDR,
			SERDES_25G_CMU_PLL_FCAL_CMD_REQ_MASK,
			SERDES_25G_CMU_PLL_FCAL_CMD_REQ_SHIFT,
			0x1);

	while (timeout > 0) {
		al_mod_serdes_25g_reg_masked_read(
				obj,
				AL_SRDS_REG_PAGE_4_COMMON,
				SERDES_25G_CMU_PLL_FCAL_CMD_ADDR,
				SERDES_25G_CMU_PLL_FCAL_CMD_ACK_MASK,
				SERDES_25G_CMU_PLL_FCAL_CMD_ACK_SHIFT,
				&ack);

		if (ack)
			break;

		AL_SERDES_25G_UDELAY(1);
		timeout--;
	}

	if (timeout > 0) {
		al_mod_serdes_25g_reg_masked_write(
				obj,
				AL_SRDS_REG_PAGE_4_COMMON,
				SERDES_25G_CMU_PLL_FCAL_CMD_ADDR,
				SERDES_25G_CMU_PLL_FCAL_CMD_REQ_MASK,
				SERDES_25G_CMU_PLL_FCAL_CMD_REQ_SHIFT,
				0);
	} else {
		al_mod_err("%s: timeout waiting for pll fcal cmd ack\n", __func__);
	}

	al_mod_serdes_25g_pll_loop(obj, AL_FALSE);

	al_mod_serdes_25g_reg_masked_write(
			obj,
			AL_SRDS_REG_PAGE_4_COMMON,
			SERDES_25G_CMU_PLL_FCAL_DOSC_START_ADDR,
			SERDES_25G_CMU_PLL_FCAL_DOSC_START_MASK,
			SERDES_25G_CMU_PLL_FCAL_DOSC_START_SHIFT,
			dosc_orig);
}

/* check for PLL lock, it will read qsample iterations number of times to determine
 * if qsample has toggled from initial value.
 * It will return true if pll is locked (no toggles)
 */
static al_mod_bool al_mod_serdes_25g_qsample_pll_lock_check(struct al_mod_serdes_grp_obj *obj)
{
	uint8_t init_val;
	uint8_t new_val;
	uint8_t iterations = AL_SERDES_25G_QSAMPLE_COUNT;
	int ret;

	AL_SERDES_25G_UDELAY(AL_SERDES_25G_PLL_LOCK_DELAY);

	ret = al_mod_serdes_25g_reg_masked_read(
			obj,
			AL_SRDS_REG_PAGE_4_COMMON,
			SERDES_25G_CMU_PLL_AFE_CLKDIV_STATUS_ADDR,
			SERDES_25G_CMU_PLL_AFE_CLKDIV_STATUS_CMPLL_QSAMPLE_MASK,
			SERDES_25G_CMU_PLL_AFE_CLKDIV_STATUS_CMPLL_QSAMPLE_SHIFT,
			&init_val);
	al_mod_assert(ret == 0);

	while (iterations > 0) {
		ret = al_mod_serdes_25g_reg_masked_read(
			obj,
			AL_SRDS_REG_PAGE_4_COMMON,
			SERDES_25G_CMU_PLL_AFE_CLKDIV_STATUS_ADDR,
			SERDES_25G_CMU_PLL_AFE_CLKDIV_STATUS_CMPLL_QSAMPLE_MASK,
			SERDES_25G_CMU_PLL_AFE_CLKDIV_STATUS_CMPLL_QSAMPLE_SHIFT,
			&new_val);
		al_mod_assert(ret == 0);

		if (new_val != init_val)
			return AL_FALSE;

		AL_SERDES_25G_UDELAY(AL_SERDES_25G_PLL_LOCK_DELAY);
		iterations--;
	}

	return AL_TRUE;
}

/* set dosc value (with or without conversion) */
static al_mod_bool al_mod_serdes_25g_set_dosc(struct al_mod_serdes_grp_obj *obj, int val, al_mod_bool do_convert)
{
	uint16_t dosc = val;

	if (do_convert)
		dosc = dosc_convert(val);

	al_mod_dbg("%s val = %d dosc = %d\n", __func__, val, dosc);

	al_mod_serdes_25g_pll_loop(obj, AL_TRUE);

	al_mod_serdes_25g_single_iteration_dosc_set(obj, dosc, AL_FALSE);

	al_mod_serdes_25g_pll_loop(obj, AL_FALSE);

	return al_mod_serdes_25g_qsample_pll_lock_check(obj);
}

/* get dosc value (with or without conversion) */
static int al_mod_serdes_25g_get_dosc(struct al_mod_serdes_grp_obj *obj, al_mod_bool do_convert)
{
	uint16_t dosc;

	al_mod_serdes_25g_tbus_read(obj,
				0,
				AL_SERDES_25G_TBUS_OBJ_CMU,
				AL_SERDES_25G_TBUS_DOSC_ADDR,
				&dosc);

	if (do_convert)
		dosc = dosc_convert(dosc);

	return dosc;
}

/* set dosc to given value, check for lock and true if the pll is locked with the new val */
static al_mod_bool al_mod_serdes_25g_recalibrate_dosc(struct al_mod_serdes_grp_obj *obj, int val)
{
	al_mod_serdes_25g_pll_loop(obj, AL_TRUE);

	al_mod_serdes_25g_single_iteration_dosc_set(obj, val, AL_TRUE);

	al_mod_serdes_25g_pll_loop(obj, AL_FALSE);

	return al_mod_serdes_25g_qsample_pll_lock_check(obj);
}

struct al_mod_serdes_25g_tx_wa_data {
	int temp;
	int direction;
	int step;
};

static struct al_mod_serdes_25g_tx_wa_data tx_pll_wa[] = {
	{.temp = 15, .direction = 1, .step = 0},
	{.temp = 33, .direction = 1, .step = 1},
	{.temp = 51, .direction = 1, .step = 2},
	{.temp = 69, .direction = -1, .step = 3},
	{.temp = 87, .direction = -1, .step = 2},
	{.temp = 110, .direction = -1, .step = 1},
	{.temp = 140, .direction = -1, .step = 0},
};

/* Convert swing value from gray code to binary */
static uint16_t al_mod_serdes_25g_swing_g2b_convert(uint16_t swing)
{
	swing = swing ^ (swing >> 8);
	swing = swing ^ (swing >> 4);
	swing = swing ^ (swing >> 2);
	swing = swing ^ (swing >> 1);

	return swing;
}

static uint16_t al_mod_serdes_25g_swing_b2g_convert(uint16_t swing)
{
	return swing ^ (swing >> 1);
}

#define AL_SERDES_25G_CMU_VCO_SWING_MIN_VALUE	10
#define AL_SERDES_25G_CMU_VCO_SWING_MAX_VALUE	13
#define AL_SERDES_25G_DOSC_RECAL_START_VALUE	40

static al_mod_bool al_mod_serdes_25g_tx_pll_wa_swing_adjust(struct al_mod_serdes_grp_obj *obj,
		uint16_t *swing_init, uint16_t *swing_adjusted)
{
	uint16_t cmu_vco_swing;
	al_mod_bool is_adjusted = AL_FALSE;

	/*
	 * make sure vco swing is no less than 10 and not greater than 13
	 */
	al_mod_serdes_25g_tbus_read(obj,
				0,
				AL_SERDES_25G_TBUS_OBJ_CMU,
				AL_SERDES_25G_TBUS_CMU_VCO_SWING_ADDR,
				&cmu_vco_swing);
	cmu_vco_swing = al_mod_serdes_25g_swing_g2b_convert(cmu_vco_swing);
	*swing_init = cmu_vco_swing;

	if (cmu_vco_swing < AL_SERDES_25G_CMU_VCO_SWING_MIN_VALUE) {
		cmu_vco_swing = AL_SERDES_25G_CMU_VCO_SWING_MIN_VALUE;
		is_adjusted = AL_TRUE;
	} else if (cmu_vco_swing > AL_SERDES_25G_CMU_VCO_SWING_MAX_VALUE) {
		cmu_vco_swing = AL_SERDES_25G_CMU_VCO_SWING_MAX_VALUE;
		is_adjusted = AL_TRUE;
	}

	*swing_adjusted = cmu_vco_swing;

	al_mod_serdes_25g_reg_masked_write(
			obj,
			AL_SRDS_REG_PAGE_TOP,
			SERDES_25G_TOP_OVR_CMU_CMPLL_VCO_SWING_OVR_VAL_ADDR,
			SERDES_25G_TOP_OVR_CMU_CMPLL_VCO_SWING_OVR_VAL_MASK,
			SERDES_25G_TOP_OVR_CMU_CMPLL_VCO_SWING_OVR_VAL_SHIFT,
			al_mod_serdes_25g_swing_b2g_convert(cmu_vco_swing));

	al_mod_serdes_25g_reg_masked_write(
			obj,
			AL_SRDS_REG_PAGE_TOP,
			SERDES_25G_TOP_OVR_CMU_CMPLL_VCO_SWING_OVR_EN_ADDR,
			SERDES_25G_TOP_OVR_CMU_CMPLL_VCO_SWING_OVR_EN_MASK,
			SERDES_25G_TOP_OVR_CMU_CMPLL_VCO_SWING_OVR_EN_SHIFT,
			1);

	return is_adjusted;
}

#define AL_SERDES_25G_WINDOW_SEARCH_STEP	5
#define AL_SERDES_25G_WINDOW_EDGE_THRESH	2

static void al_mod_serdes_25g_tx_pll_wa_find_window_edge(struct al_mod_serdes_grp_obj *obj,
		int dosc_init, int direction, int *window_edge)
{
	int dosc = dosc_init;
	int edge_count = 0;

	do {
		dosc += direction;
		if (!al_mod_serdes_25g_set_dosc(obj, dosc, AL_TRUE)) {
			edge_count++;
			al_mod_dbg("%s: dosc [l]%d [r]%d: unlocked\n",
					__func__, dosc, dosc_convert(dosc));
		} else {
			if (edge_count > 0)
				al_mod_warn("%s: dosc [l]%d [r]%d: hole\n",
						__func__, dosc - direction,
						dosc_convert(dosc - direction));
			edge_count = 0;
			al_mod_dbg("%s: dosc [l]%d [r]%d: locked\n",
					__func__, dosc, dosc_convert(dosc));
		}
	} while ((dosc <= AL_SERDES_DOSC_MAX_VAL) &&
			(dosc >= AL_SERDES_DOSC_MIN_VAL) &&
			(edge_count < AL_SERDES_25G_WINDOW_EDGE_THRESH));

	al_mod_dbg("%s: edge detected, [l]%d [r]%d\n",
			__func__, dosc - (direction * edge_count),
			dosc_convert(dosc - (direction * edge_count)));
	*window_edge = dosc - (direction * edge_count);
}

static al_mod_bool al_mod_serdes_25g_tx_pll_wa_find_window(struct al_mod_serdes_grp_obj *obj,
		int *dosc_init_adj, int *window_start, int *window_end,
		struct al_mod_serdes_tx_pll_wa_status *status)
{
	al_mod_bool is_inside;
	int dosc_init;

	dosc_init = al_mod_serdes_25g_get_dosc(obj, AL_TRUE);
	is_inside = al_mod_serdes_25g_qsample_pll_lock_check(obj);

	if (status)
		status->pll_inside_window_on_start = is_inside;

	/* handle the case of init inside a hole */
	if (!is_inside && (dosc_init < AL_SERDES_DOSC_MAX_VAL) &&
			al_mod_serdes_25g_set_dosc(obj, dosc_init + 1, AL_TRUE)) {
		dosc_init++;
		is_inside = AL_TRUE;
	}

	if (!is_inside && (dosc_init > AL_SERDES_DOSC_MIN_VAL) &&
			al_mod_serdes_25g_set_dosc(obj, dosc_init - 1, AL_TRUE)) {
		dosc_init--;
		is_inside = AL_TRUE;
	}

	if (is_inside) {
		al_mod_dbg("%s: init [l]%d [r]%d inside window\n", __func__, dosc_init,
				dosc_convert(dosc_init));

		al_mod_serdes_25g_tx_pll_wa_find_window_edge(obj, dosc_init, 1, window_end);
		al_mod_serdes_25g_tx_pll_wa_find_window_edge(obj, dosc_init, -1, window_start);

		*dosc_init_adj = dosc_init;

		return AL_TRUE;
	} else {
		/* search for window in increasing steps till the whole range is covered */
		int start = 0;
		int end = 0;
		int iteration = 0;
		int dosc = dosc_init;

		al_mod_warn("%s: init [l]%d [r]%d outside window, searching for valid window\n",
				__func__, dosc, dosc_convert(dosc_init));

		while (1) {
			int dosc_up = dosc_init + iteration * AL_SERDES_25G_WINDOW_SEARCH_STEP;
			int dosc_down = dosc_init - iteration * AL_SERDES_25G_WINDOW_SEARCH_STEP;

			if ((dosc_up > AL_SERDES_DOSC_MAX_VAL) &&
					(dosc_down < AL_SERDES_DOSC_MIN_VAL))
				break;

			for (dosc = dosc_up;
					dosc < (dosc_up + AL_SERDES_25G_WINDOW_SEARCH_STEP);
					dosc++) {
				if (dosc > AL_SERDES_DOSC_MAX_VAL)
					break;
				if (al_mod_serdes_25g_set_dosc(obj, dosc, AL_TRUE)) {
					start = dosc;
					break;
				}
			}

			if (start)
				break;

			for (dosc = dosc_down;
					dosc > (dosc_down - AL_SERDES_25G_WINDOW_SEARCH_STEP);
					dosc--) {
				if (dosc < AL_SERDES_DOSC_MIN_VAL)
					break;
				if (al_mod_serdes_25g_set_dosc(obj, dosc, AL_TRUE)) {
					end = dosc;
					break;
				}
			}

			if (end)
				break;

			iteration++;
		}

		if (start) {
			*window_start = start;
			al_mod_serdes_25g_tx_pll_wa_find_window_edge(obj, start, 1, window_end);
		} else if (end) {
			*window_end = end;
			al_mod_serdes_25g_tx_pll_wa_find_window_edge(obj, end, -1, window_start);
		}

		if (start || end) {
			*dosc_init_adj = (*window_start + *window_end) / 2;
			return AL_TRUE;
		} else
			return AL_FALSE;
	}
}

/*
 * this WA required to improve TX PLL lock range over temperature ramps.
 * without it the pll may lose lock in case of big change in the system temperature.
 */
static int al_mod_serdes_25g_tx_pll_wa(struct al_mod_serdes_grp_obj *obj,
	struct al_mod_serdes_tx_pll_wa_status *status)
{
	uint16_t swing_init, swing_adjusted;
	unsigned int range_idx;
	int temperature;
	int dosc_init, dosc_target;
	int dosc_window_start = 0;
	int dosc_window_end = 0;
	int dosc_window_center;
	al_mod_bool window_avail;
	al_mod_bool locked;

	if (status) {
		status->valid = AL_FALSE;
		status->pll_inside_window_on_start = AL_FALSE;
		status->dosc_window_available = AL_FALSE;
		status->locked_final = AL_FALSE;
	}

	if (!obj->temperature_get_cb) {
		al_mod_err("can't apply tx pll WA without temp read\n");
		return -EIO;
	}

	if (al_mod_serdes_25g_tx_pll_wa_swing_adjust(obj, &swing_init, &swing_adjusted))
		al_mod_serdes_25g_recalibrate_dosc(obj, AL_SERDES_25G_DOSC_RECAL_START_VALUE);

	if (status) {
		status->valid = AL_TRUE;
		status->swing_init = swing_init;
		status->swing_adjusted = swing_adjusted;
	}

	temperature = obj->temperature_get_cb();
	al_mod_dbg("%s: temperature for tx WA %d\n", __func__, temperature);

	if (status)
		status->temperature = temperature;

	for (range_idx = 0; range_idx < AL_ARR_SIZE(tx_pll_wa); range_idx++) {
		if (temperature <= tx_pll_wa[range_idx].temp)
			break;
	}

	if (range_idx >= AL_ARR_SIZE(tx_pll_wa)) {
		al_mod_err("%s: error: invalid temperature range, temperature %d!\n",
				__func__, temperature);
		return -EIO;
	}

	window_avail = al_mod_serdes_25g_tx_pll_wa_find_window(obj, &dosc_init,
			&dosc_window_start, &dosc_window_end, status);

	if (status)
		status->dosc_window_available = window_avail;

	if (!window_avail) {
		al_mod_err("%s: error: no valid dosc window available!\n", __func__);
		return -EIO;
	}

	if (status) {
		status->dosc_init = dosc_convert(dosc_init);
		status->dosc_window_start = dosc_convert(dosc_window_start);
		status->dosc_window_end = dosc_convert(dosc_window_end);
	}

	dosc_window_center = (dosc_window_start + dosc_window_end) / 2;

	if (tx_pll_wa[range_idx].direction == 1) {
		dosc_target = dosc_window_end - tx_pll_wa[range_idx].step;
		if (dosc_target < AL_SERDES_DOSC_MIN_VAL) {
			al_mod_warn("%s: dosc_target [l]%d [r]%d lower than min, setting to min %d\n",
					__func__, dosc_target, dosc_convert(dosc_target),
					AL_SERDES_DOSC_MIN_VAL);
			dosc_target = AL_SERDES_DOSC_MIN_VAL;
		}
	} else {
		dosc_target = dosc_window_start + tx_pll_wa[range_idx].step;
		if (dosc_target > AL_SERDES_DOSC_MAX_VAL) {
			al_mod_warn("%s: dosc_target [l]%d [r]%d is higher than max, setting to max %d\n",
					__func__, dosc_target, dosc_convert(dosc_target),
					AL_SERDES_DOSC_MAX_VAL);
			dosc_target = AL_SERDES_DOSC_MAX_VAL;
		}
	}

	if ((dosc_target < dosc_window_start) || (dosc_target > dosc_window_end)) {
		/* windows is small - go to center of the window */
		al_mod_warn("%s: stepped outside of window go to center [l]%d [r]%d->[l]%d [r]%d\n",
				__func__, dosc_target, dosc_convert(dosc_target),
				dosc_window_center, dosc_convert(dosc_window_center));
		dosc_target = dosc_window_center;
	}

	locked = al_mod_serdes_25g_set_dosc(obj, dosc_target, AL_TRUE);
	while (!locked && (dosc_target != dosc_window_center)) {
		al_mod_warn("%s: dosc_target inside the window, but pll is unlocked! stepping towards center of window\n",
				__func__);

		if (dosc_target < dosc_window_center)
			dosc_target++;
		else
			dosc_target--;
		locked = al_mod_serdes_25g_set_dosc(obj, dosc_target, AL_TRUE);
	}

	if (!locked) {
		dosc_target = dosc_init;
		al_mod_warn("%s: pll is unlocked, returning to dosc_init value!\n", __func__);
		locked = al_mod_serdes_25g_set_dosc(obj, dosc_target, AL_TRUE);
	}

	if (status) {
		status->dosc_target = dosc_convert(dosc_target);
		status->locked_final = locked;
	}

	if (!locked) {
		al_mod_err("%s: failed to lock PLL\n", __func__);
		return -EIO;
	}

	al_mod_info("%s: temperature %d, swing_init %d, swing_adjusted %d\n",
			__func__, temperature, swing_init, swing_adjusted);
	al_mod_info("%s: dosc_init %d, dosc_target %d, window_start %d, window_end %d\n",
			__func__, dosc_convert(dosc_init), dosc_convert(dosc_target),
			dosc_convert(dosc_window_start), dosc_convert(dosc_window_end));

	return 0;
}

static void al_mod_serdes_25g_lane_adapt_hfg_config(struct al_mod_serdes_grp_obj *obj,
						enum al_mod_serdes_reg_page lane)
{
	al_mod_serdes_25g_reg_masked_write(
		obj,
		lane,
		SERDES_25G_LANE_FEATURE_CTLE_ADAPT_HFG_CFG0_ADDR,
		SERDES_25G_LANE_FEATURE_CTLE_ADAPT_HFG_CFG0_INIT0_EDGE_EN_MASK,
		SERDES_25G_LANE_FEATURE_CTLE_ADAPT_HFG_CFG0_INIT0_EDGE_EN_SHIFT,
		1);

	al_mod_serdes_25g_reg_masked_write(
		obj,
		lane,
		SERDES_25G_LANE_FEATURE_CTLE_ADAPT_HFG_CFG0_ADDR,
		SERDES_25G_LANE_FEATURE_CTLE_ADAPT_HFG_CFG0_INIT0_DATA_EN_MASK,
		SERDES_25G_LANE_FEATURE_CTLE_ADAPT_HFG_CFG0_INIT0_DATA_EN_SHIFT,
		0);
}

static void al_mod_serdes_25g_lane_adapt_mbs_leq_refclk_config(struct al_mod_serdes_grp_obj *obj,
							   enum al_mod_serdes_reg_page lane,
							   al_mod_bool mode_25g)
{
	uint8_t mbf, mbg;

	al_mod_serdes_25g_reg_masked_write(
		obj,
		lane,
		SERDES_25G_LANE_FEATURE_CTLE_ADAPT_MBS_CFG_ADDR,
		SERDES_25G_LANE_FEATURE_CTLE_ADAPT_MBS_CFG_INIT0_EN_MASK,
		SERDES_25G_LANE_FEATURE_CTLE_ADAPT_MBS_CFG_INIT0_EN_SHIFT,
		0);

	if (mode_25g) {
		mbf = 0x7;
		mbg = 0xf;
	} else {
		mbf = 0x0;
		mbg = 0x7;
	}

	al_mod_serdes_25g_reg_masked_write(
		obj,
		lane,
		SERDES_25G_LANE_LEQ_REFCLK_EQ_MB_CTRL1_ADDR,
		SERDES_25G_LANE_LEQ_REFCLK_EQ_MB_CTRL1_EQ_MBF_START_MASK,
		SERDES_25G_LANE_LEQ_REFCLK_EQ_MB_CTRL1_EQ_MBF_START_SHIFT,
		mbf);

	al_mod_serdes_25g_reg_masked_write(
		obj,
		lane,
		SERDES_25G_LANE_LEQ_REFCLK_EQ_MB_CTRL1_ADDR,
		SERDES_25G_LANE_LEQ_REFCLK_EQ_MB_CTRL1_EQ_MBG_START_MASK,
		SERDES_25G_LANE_LEQ_REFCLK_EQ_MB_CTRL1_EQ_MBG_START_SHIFT,
		mbg);
}

static void al_mod_serdes_25g_lane_cdr_leq_config(struct al_mod_serdes_grp_obj *obj,
					      enum al_mod_serdes_reg_page lane)
{
	uint8_t reg;

	al_mod_serdes_25g_reg_masked_write(
		obj,
		lane,
		SERDES_25G_LANE_CDR_REFCLK_AFE_VCO_CTRL2_ADDR,
		SERDES_25G_LANE_CDR_REFCLK_AFE_VCO_CTRL2_RXCDR_DOSC_MASK,
		SERDES_25G_LANE_CDR_REFCLK_AFE_VCO_CTRL2_RXCDR_DOSC_SHIFT,
		0x4);
	al_mod_serdes_25g_reg_masked_write(
		obj,
		lane,
		SERDES_25G_LANE_CDR_RXCLK_DLPF_CTRL0_ADDR,
		SERDES_25G_LANE_CDR_RXCLK_DLPF_CTRL0_NUM_DITHER_BITS_MASK,
		SERDES_25G_LANE_CDR_RXCLK_DLPF_CTRL0_NUM_DITHER_BITS_SHIFT,
		0x6);

	al_mod_serdes_25g_reg_read(
		obj,
		lane,
		0,
		SERDES_25G_LANE_FEATURE_CDR_CAL_CFG0_ADDR,
		&reg);
	AL_REG_FIELD_SET(
		reg,
		SERDES_25G_LANE_FEATURE_CDR_CAL_CFG0_CDR_IQ_CAL1_EN_MASK,
		SERDES_25G_LANE_FEATURE_CDR_CAL_CFG0_CDR_IQ_CAL1_EN_SHIFT,
		0);
	AL_REG_FIELD_SET(
		reg,
		SERDES_25G_LANE_FEATURE_CDR_CAL_CFG0_CDR_IQ_CAL3_EN_MASK,
		SERDES_25G_LANE_FEATURE_CDR_CAL_CFG0_CDR_IQ_CAL3_EN_SHIFT,
		0x1);
	AL_REG_FIELD_SET(
		reg,
		SERDES_25G_LANE_FEATURE_CDR_CAL_CFG0_CDR_IQ_CAL_RESULT_SEL_MASK,
		SERDES_25G_LANE_FEATURE_CDR_CAL_CFG0_CDR_IQ_CAL_RESULT_SEL_SHIFT,
		0x2);
	al_mod_serdes_25g_reg_write(
		obj,
		lane,
		0,
		SERDES_25G_LANE_FEATURE_CDR_CAL_CFG0_ADDR,
		reg);

	al_mod_serdes_25g_reg_masked_write(
		obj,
		lane,
		SERDES_25G_LANE_LEQ_REFCLK_AFE_BIAS_CTRL1_ADDR,
		SERDES_25G_LANE_LEQ_REFCLK_AFE_BIAS_CTRL1_RXLEQ_BIASI_TRIM_MASK,
		SERDES_25G_LANE_LEQ_REFCLK_AFE_BIAS_CTRL1_RXLEQ_BIASI_TRIM_SHIFT,
		0x3);
}

static void al_mod_serdes_25g_lane_bbstep_comparator_config(struct al_mod_serdes_grp_obj *obj,
							enum al_mod_serdes_reg_page lane,
							al_mod_bool mode_25g)
{
	uint8_t reg;
	uint32_t bb_step_val = mode_25g ? AL_SERDES_BB_STEP_VALUE_25G :
		AL_SERDES_BB_STEP_VALUE_10G;

	obj->reg_read(obj, lane, 0, SERDES_25G_LANE_CDR_REFCLK_AFE_CTRL0_ADDR, &reg);
	AL_REG_FIELD_SET(reg, SERDES_25G_LANE_CDR_REFCLK_AFE_CTRL0_RXCDR_BBSTEP_MASK,
			SERDES_25G_LANE_CDR_REFCLK_AFE_CTRL0_RXCDR_BBSTEP_SHIFT,
			bb_step_val);
	obj->reg_write(obj, lane, 0, SERDES_25G_LANE_CDR_REFCLK_AFE_CTRL0_ADDR, reg);
	al_mod_serdes_25g_reg_masked_write(
		obj,
		lane,
		SERDES_25G_LANE_LOS_REFCLK_COMPARATOR_CTRL1_ADDR,
		SERDES_25G_LANE_LOS_REFCLK_COMPARATOR_CTRL1_THRESHOLD_MASK,
		SERDES_25G_LANE_LOS_REFCLK_COMPARATOR_CTRL1_THRESHOLD_SHIFT,
		0x4);
}

static void al_mod_serdes_25g_lane_afe_txcp_cal_config(struct al_mod_serdes_grp_obj *obj,
						   enum al_mod_serdes_reg_page lane)
{
	al_mod_serdes_25g_reg_masked_write(
		obj,
		lane,
		SERDES_25G_LANE_TOP_AFE_TXCP_CTRL4_ADDR,
		SERDES_25G_LANE_TOP_AFE_TXCP_CTRL4_TXCP_CLKDIV_SWING_MASK,
		SERDES_25G_LANE_TOP_AFE_TXCP_CTRL4_TXCP_CLKDIV_SWING_SHIFT,
		0x3);

	al_mod_serdes_25g_reg_masked_write(
		obj,
		lane,
		SERDES_25G_LANE_TOP_AFE_TXCP_CTRL0_ADDR,
		SERDES_25G_LANE_TOP_AFE_TXCP_CTRL0_REG_TXCP_TRIM_MASK,
		SERDES_25G_LANE_TOP_AFE_TXCP_CTRL0_REG_TXCP_TRIM_SHIFT,
		0x8);

	al_mod_serdes_25g_reg_masked_write(
		obj,
		lane,
		SERDES_25G_LANE_FEATURE_TX_CAL_CFG0_ADDR,
		SERDES_25G_LANE_FEATURE_TX_CAL_CFG0_TX_REG_EN_MASK,
		SERDES_25G_LANE_FEATURE_TX_CAL_CFG0_TX_REG_EN_SHIFT,
		0);
}

#define SERDES_25G_10G_CDR_VCO_FR_ADD		8
#define SERDES_25G_10G_CDR_VCO_FR_MAX		31

static void al_mod_serdes_25g_lane_cdr_vco_fr_config(struct al_mod_serdes_grp_obj *obj,
						 enum al_mod_serdes_reg_page lane)
{
	uint16_t cdr_vco_fr;

	/* add 8 to cdr_vco_fr value and override, maximum allowed value is 31 */
	al_mod_serdes_25g_tbus_read(
		obj,
		(enum al_mod_serdes_lane)lane,
		AL_SERDES_25G_TBUS_OBJ_LANE,
		AL_SERDES_25G_TBUS_LANE_CDR_VCO_FR_ADDR,
		&cdr_vco_fr);

	cdr_vco_fr = (cdr_vco_fr & AL_SERDES_25G_TBUS_LANE_CDR_VCO_FR_MASK) >>
		AL_SERDES_25G_TBUS_LANE_CDR_VCO_FR_SHIFT;

	cdr_vco_fr += SERDES_25G_10G_CDR_VCO_FR_ADD;

	if (cdr_vco_fr > SERDES_25G_10G_CDR_VCO_FR_MAX)
		cdr_vco_fr = SERDES_25G_10G_CDR_VCO_FR_MAX;

	al_mod_serdes_25g_reg_masked_write(
		obj,
		AL_SRDS_REG_PAGE_TOP,
		SERDES_25G_TOP_OVR_LANE_RXCDR_VCOFR_OVR_VAL_ADDR(lane),
		SERDES_25G_TOP_OVR_LANE_RXCDR_VCOFR_OVR_VAL_MASK,
		SERDES_25G_TOP_OVR_LANE_RXCDR_VCOFR_OVR_VAL_SHIFT,
		cdr_vco_fr);
	al_mod_serdes_25g_reg_masked_write(
		obj,
		AL_SRDS_REG_PAGE_TOP,
		SERDES_25G_TOP_OVR_LANE_RXCDR_VCOFR_OVR_EN_ADDR(lane),
		SERDES_25G_TOP_OVR_LANE_RXCDR_VCOFR_OVR_EN_MASK,
		SERDES_25G_TOP_OVR_LANE_RXCDR_VCOFR_OVR_EN_SHIFT,
		0x1);
}

static int al_mod_serdes_25g_fw_init_status_group_common_check(struct al_mod_serdes_25g_fw_init_status
							   *fw_init_status)
{

	if (fw_init_status->top_error != 0)
		al_mod_warn("%s: Got FW error! TOP error code %x\n", __func__,
		       fw_init_status->top_error);

	if ((fw_init_status->cm_error != 0) || (fw_init_status->cm_critical_error != 0)) {
		al_mod_err("%s: Got FW error! CM error code %x\n", __func__, fw_init_status->cm_error);

		if (fw_init_status->cm_critical_error != 0) {
			al_mod_err("%s: FW CM error is critical (%d)!!\n",
				__func__, fw_init_status->cm_critical_error);

			return -EIO;
		}
	}

	return 0;
}

static int al_mod_serdes_25g_fw_init_status_lane_check(struct al_mod_serdes_25g_fw_init_status
						   *fw_init_status, enum al_mod_serdes_lane lane)
{
	if ((fw_init_status->lane_error[lane] != 0) ||
				(fw_init_status->lane_critical_error[lane] != 0)) {
		al_mod_err("%s: Got FW error! lane %d error code %x\n",
		       __func__, lane, fw_init_status->lane_error[lane]);

		if (fw_init_status->lane_critical_error[lane] != 0) {
			al_mod_err("%s: FW lane %d error is critical (%d)!!\n",
			       __func__, lane, fw_init_status->lane_critical_error[lane]);

			return -EIO;
		}
	}

	return 0;
}

static int al_mod_serdes_25g_group_cfg(struct al_mod_serdes_grp_obj *obj,
	struct al_mod_serdes_mode_set_adv_params *params)
{
	struct al_mod_serdes_c_regs __iomem *regs_base =
		(struct al_mod_serdes_c_regs __iomem *)obj->regs_base;
	al_mod_bool mode_25g = (params->group_mode == AL_SRDS_CFG_ETH_25G);
	enum al_mod_serdes_reg_page lane;
	struct al_mod_serdes_25g_fw_init_status fw_init_status;
	int rc = 0;

	if (mode_25g)
		al_mod_reg_write32(&regs_base->gen.ctrl, 0x201100);
	else
		al_mod_reg_write32(&regs_base->gen.ctrl, 0x901100);

	/* Reset POR, CM0, and Lanes */
	al_mod_reg_write32(&regs_base->gen.rst, 0x00);
	AL_SERDES_25G_UDELAY(AL_SERDES_25G_RESET_DELAY);

	/* Release POR from reset */
	al_mod_reg_write32_masked(&regs_base->gen.rst, SERDES_C_GEN_RST_POR_N, SERDES_C_GEN_RST_POR_N);
	AL_SERDES_25G_UDELAY(AL_SERDES_25G_RESET_DELAY);
	/* Polling the Serdes power up */
	rc = al_mod_serdes_25g_poll(obj, &regs_base->gen.status,
				(SERDES_C_GEN_STATUS_CM0_RST_PD_READY |
				 SERDES_C_GEN_STATUS_LN0_RST_PD_READY |
				 SERDES_C_GEN_STATUS_LN1_RST_PD_READY),
				(SERDES_C_GEN_STATUS_CM0_RST_PD_READY |
				 SERDES_C_GEN_STATUS_LN0_RST_PD_READY |
				 SERDES_C_GEN_STATUS_LN1_RST_PD_READY),
				AL_SERDES_25G_INIT_TIMEOUT);

	if (rc) {
		al_mod_err("%s: Got timeout waiting for serdes CM0 and LN0/LN1 to enter PD_READY\n",
		       __func__);
		return rc;
	}


	for (lane = AL_SRDS_REG_PAGE_0_LANE_0; lane <= AL_SRDS_REG_PAGE_1_LANE_1; lane++) {
		al_mod_serdes_25g_lane_adapt_hfg_config(obj, lane);
	}

	for (lane = AL_SRDS_REG_PAGE_0_LANE_0; lane <= AL_SRDS_REG_PAGE_1_LANE_1; lane++) {
		al_mod_serdes_25g_lane_adapt_mbs_leq_refclk_config(obj, lane, mode_25g);
	}

	if (mode_25g) {
		/* fix tx clk jitter peak appears at 150Khz */
		al_mod_serdes_25g_reg_masked_write(
					obj,
					AL_SRDS_REG_PAGE_4_COMMON,
					SERDES_25G_CMU_PLL_AFE_PROP_CTRL3_ADDR,
					SERDES_25G_CMU_PLL_AFE_PROP_CTRL3_CMPLL_PKVCO_BIAS_UP_MASK,
					SERDES_25G_CMU_PLL_AFE_PROP_CTRL3_CMPLL_PKVCO_BIAS_UP_SHIFT,
					6);

		al_mod_serdes_25g_reg_masked_write(
					obj,
					AL_SRDS_REG_PAGE_4_COMMON,
					SERDES_25G_CMU_PLL_AFE_PROP_CTRL3_ADDR,
					SERDES_25G_CMU_PLL_AFE_PROP_CTRL3_CMPLL_PKVCO_BIAS_DN_MASK,
					SERDES_25G_CMU_PLL_AFE_PROP_CTRL3_CMPLL_PKVCO_BIAS_DN_SHIFT,
					6);
	} else {
		al_mod_serdes_25g_reg_masked_write(
					obj,
					AL_SRDS_REG_PAGE_4_COMMON,
					SERDES_25G_CM_TOP_AFE_BIAS_CTRL0_ADDR,
					SERDES_25G_CMU_TOP_AFE_BIAS_CTRL0_BIAS_ICV_TRIM_MASK,
					SERDES_25G_CMU_TOP_AFE_BIAS_CTRL0_BIAS_ICV_TRIM_SHIFT,
					0x9);
	}

	AL_SERDES_25G_UDELAY(1);
	/* Release CM0 from reset */
	al_mod_reg_write32_masked(&regs_base->gen.rst,
				SERDES_C_GEN_RST_CM0_RST_N,
				SERDES_C_GEN_RST_CM0_RST_N);

	/* Polling the Serdes power up */
	rc = al_mod_serdes_25g_poll(obj, &regs_base->gen.status,
			(SERDES_C_GEN_STATUS_CM0_RST_PD_READY | SERDES_C_GEN_STATUS_CM0_OK_O),
			(SERDES_C_GEN_STATUS_CM0_RST_PD_READY | SERDES_C_GEN_STATUS_CM0_OK_O),
			AL_SERDES_25G_INIT_TIMEOUT);

	if (rc) {
		al_mod_err("%s: Got timeout waiting for serdes CM0 Status to become ok\n", __func__);
		return rc;
	}

	if (!mode_25g) {
		for (lane = AL_SRDS_REG_PAGE_0_LANE_0; lane <= AL_SRDS_REG_PAGE_1_LANE_1; lane++) {
			al_mod_serdes_25g_lane_cdr_leq_config(obj, lane);
		}
	}

	for (lane = AL_SRDS_REG_PAGE_0_LANE_0; lane <= AL_SRDS_REG_PAGE_1_LANE_1; lane++) {
		uint8_t reg_val = mode_25g ? 0x71 : 0x44;

		al_mod_serdes_25g_reg_masked_write(
			obj,
			lane,
			SERDES_25G_LANE_LEQ_REFCLK_AFE_BIAS_CTRL0_ADDR,
			SERDES_25G_LANE_LEQ_REFCLK_AFE_BIAS_CTRL0_RXLEQ_BIAS_MASK,
			SERDES_25G_LANE_LEQ_REFCLK_AFE_BIAS_CTRL0_RXLEQ_BIAS_SHIFT,
			reg_val);
	}

	/* Use DIV4 */
	for (lane = AL_SRDS_REG_PAGE_0_LANE_0; lane <= AL_SRDS_REG_PAGE_1_LANE_1; lane++) {
		al_mod_serdes_25g_reg_masked_write(
			obj,
			lane,
			SERDES_25G_LANE_CDR_REFCLK_AFE_CAL_CTRL0_ADDR,
			SERDES_25G_LANE_CDR_REFCLK_AFE_CAL_CTRL0_RXCDR_VCOCAL_DIV4_MASK,
			SERDES_25G_LANE_CDR_REFCLK_AFE_CAL_CTRL0_RXCDR_VCOCAL_DIV4_SHIFT,
			0x1);
	}

	for (lane = AL_SRDS_REG_PAGE_0_LANE_0; lane <= AL_SRDS_REG_PAGE_1_LANE_1; lane++) {
		al_mod_serdes_25g_lane_bbstep_comparator_config(obj, lane, mode_25g);
	}

	for (lane = AL_SRDS_REG_PAGE_0_LANE_0; lane <= AL_SRDS_REG_PAGE_1_LANE_1; lane++) {
		al_mod_serdes_25g_lane_afe_txcp_cal_config(obj, lane);
	}

	rc = al_mod_serdes_25g_tx_pll_wa(obj, params->tx_pll_wa_status);
	if (rc) {
		al_mod_err("%s: failed to apply tx pll wa\n", __func__);
		return rc;
	}

	/* Release Lanes from reset */
	al_mod_reg_write32_masked(&regs_base->gen.rst,
				(SERDES_C_GEN_RST_LN0_RST_N | SERDES_C_GEN_RST_LN1_RST_N),
				(SERDES_C_GEN_RST_LN0_RST_N | SERDES_C_GEN_RST_LN1_RST_N));

	al_mod_reg_write32(&regs_base->lane[AL_SRDS_REG_PAGE_0_LANE_0].cfg, 0x1110);
	al_mod_reg_write32(&regs_base->lane[AL_SRDS_REG_PAGE_1_LANE_1].cfg, 0x1410);

	for (lane = AL_SRDS_REG_PAGE_0_LANE_0; lane <= AL_SRDS_REG_PAGE_1_LANE_1; lane++) {
		rc = al_mod_serdes_25g_poll(obj, &regs_base->lane[lane].stat,
					SERDES_C_LANE_STAT_LNX_STAT_OK,
					SERDES_C_LANE_STAT_LNX_STAT_OK,
					AL_SERDES_25G_INIT_TIMEOUT);

		if (rc) {
			al_mod_err("%s: Got timeout waiting for LN%d status to become ok\n",
			       __func__, lane);
			return rc;
		}
	}

	if (!mode_25g) {
		for (lane = AL_SRDS_REG_PAGE_0_LANE_0; lane <= AL_SRDS_REG_PAGE_1_LANE_1; lane++) {
			al_mod_serdes_25g_lane_cdr_vco_fr_config(obj, lane);
		}
	}

	for (lane = AL_SRDS_REG_PAGE_0_LANE_0; lane <= AL_SRDS_REG_PAGE_1_LANE_1; lane++) {
		al_mod_serdes_set_adaptation_on_sd(obj, lane);

		/* Enable continuous adaptation */
		al_mod_serdes_25g_reg_masked_write(
				obj,
				lane,
				SERDES_25G_LANE_FEATURE_SPARE_CFG0_ADDR,
				SERDES_25G_LANE_FEATURE_SPARE_CFG0_CONTINUOUS_ADAPTATION_EN_MASK,
				SERDES_25G_LANE_FEATURE_SPARE_CFG0_CONTINUOUS_ADAPTATION_EN_SHIFT,
				0x1);
	}

	/* check if the FW got error during the initialization */
	obj->fw_init_status_get(obj, &fw_init_status);

	rc = al_mod_serdes_25g_fw_init_status_group_common_check(&fw_init_status);

	if (rc)
		return rc;

	for (lane = AL_SRDS_REG_PAGE_0_LANE_0; lane <= AL_SRDS_REG_PAGE_1_LANE_1; lane++) {
		rc = al_mod_serdes_25g_fw_init_status_lane_check(&fw_init_status,
							(enum al_mod_serdes_lane)lane);
		if (rc)
			return rc;
	}

	al_mod_info("Serdes25G power up OK (%s mode)\n", (mode_25g) ? "25G" : "10G");

	return 0;
}

static int al_mod_serdes_25g_group_cfg_with_retry(struct al_mod_serdes_grp_obj *obj,
	int retry_count, struct al_mod_serdes_mode_set_adv_params *params)
{
	int rc;

	do {
		retry_count--;
		rc = al_mod_serdes_25g_group_cfg(obj, params);
	} while ((rc != 0) && retry_count > 0);

	return rc;
}

static al_mod_bool al_mod_serdes_25g_is_fw_alive(struct al_mod_serdes_grp_obj *obj)
{
	struct al_mod_serdes_c_regs __iomem	*regs_base;
	uint32_t ctrl;
	uint8_t rsp_data_5;
	uint8_t rsp_data_6;
	uint8_t rsp_data_7;
	uint8_t rsp_data_8;

	al_mod_assert(obj);

	regs_base = obj->regs_base;

	/* check if serdes ctrl register
	 * was written to by SW. this will serve
	 * as indication that the serdes is alive
	 * and its register can be accessed
	 */
	ctrl = al_mod_reg_read32(&regs_base->gen.ctrl);
	if (!(ctrl & SERDES_C_GEN_CTRL_PHY_CTRL_CFG_MASK))
		return AL_FALSE;

	/* check that FW is alive
	 * The first thing the firmware does after boot
	 * is write it’s version information to the mailbox
	 * rsp_data registers which are 0 by default
	 */
	al_mod_serdes_25g_reg_read(obj, AL_SRDS_REG_PAGE_TOP, 0,
			SERDES_25G_TOP_RSP_DATA5_ADDR, &rsp_data_5);
	al_mod_serdes_25g_reg_read(obj, AL_SRDS_REG_PAGE_TOP, 0,
			SERDES_25G_TOP_RSP_DATA6_ADDR, &rsp_data_6);
	al_mod_serdes_25g_reg_read(obj, AL_SRDS_REG_PAGE_TOP, 0,
			SERDES_25G_TOP_RSP_DATA7_ADDR, &rsp_data_7);
	al_mod_serdes_25g_reg_read(obj, AL_SRDS_REG_PAGE_TOP, 0,
			SERDES_25G_TOP_RSP_DATA8_ADDR, &rsp_data_8);
	if (rsp_data_5 == 0 ||
	    rsp_data_6 == 0 ||
	    rsp_data_7 == 0 ||
	    rsp_data_8 == 0)
		return AL_FALSE;

	return AL_TRUE;
}

/******************************************************************************/
/******************************************************************************/
static int al_mod_serdes_25g_group_cfg_mode_adv(struct al_mod_serdes_grp_obj	*obj,
	struct al_mod_serdes_mode_set_adv_params *params)
{
	if ((params->group_mode != AL_SRDS_CFG_KR) &&
		(params->group_mode != AL_SRDS_CFG_ETH_25G)) {
		al_mod_err("%s: Unsupported group mode %u\n", __func__, params->group_mode);
		return -EINVAL;
	}

	return al_mod_serdes_25g_group_cfg_with_retry(obj,
		AL_SERDES_25G_SERDES_INIT_NUM_RETRIES, params);
}

static int al_mod_serdes_25g_group_cfg_25g_mode(struct al_mod_serdes_grp_obj	*obj)
{
	struct al_mod_serdes_mode_set_adv_params params = {
		.group_mode = AL_SRDS_CFG_ETH_25G
	};

	return al_mod_serdes_25g_group_cfg_mode_adv(obj, &params);
}

static int al_mod_serdes_25g_group_cfg_10g_mode(struct al_mod_serdes_grp_obj	*obj)
{
	struct al_mod_serdes_mode_set_adv_params params = {
		.group_mode = AL_SRDS_CFG_KR
	};

	return al_mod_serdes_25g_group_cfg_mode_adv(obj, &params);
}

/******************************************************************************/
/******************************************************************************/
#define SERDES_25G_GEN_CTRL_SPEED_10G	9
#define SERDES_25G_GEN_CTRL_SPEED_25G	2

static void al_mod_serdes_25g_mode_class_get(__attribute__ ((unused)) struct al_mod_serdes_grp_obj *obj,
				enum al_mod_serdes_group_mode_class *serdes_group_mode_class)
{
	/* Currently Serdes Group E is used only for ETH mode_class*/
	*serdes_group_mode_class = AL_SRDS_MODE_CLASS_ETH;
}

static int al_mod_serdes_25g_mode_get(struct al_mod_serdes_grp_obj *obj,
				enum al_mod_serdes_group_mode *serdes_group_mode)
{
	struct al_mod_serdes_c_regs __iomem *regs_base = obj->regs_base;
	uint32_t reg;
	unsigned int serdes_25g_speed;

	reg = al_mod_reg_read32(&regs_base->gen.ctrl);
	serdes_25g_speed = AL_REG_FIELD_GET(reg,
					SERDES_C_GEN_CTRL_PHY_CTRL_CFG_MASK,
					SERDES_C_GEN_CTRL_PHY_CTRL_CFG_SHIFT);

	switch (serdes_25g_speed) {
	case SERDES_25G_GEN_CTRL_SPEED_25G:
		*serdes_group_mode = AL_SRDS_CFG_ETH_25G;
		break;
	case SERDES_25G_GEN_CTRL_SPEED_10G:
		*serdes_group_mode = AL_SRDS_CFG_KR;
		break;
	default:
		al_mod_err("%s: Unable to determine the serdes_group_mode (read 0x%x)\n", __func__,
			serdes_25g_speed);
		return -EIO;
	}

	al_mod_dbg("%s: serdes_group_mode is %d\n", __func__, *serdes_group_mode);

	return 0;
}

/******************************************************************************/
/******************************************************************************/
int al_mod_serdes_25g_handle_init(
	void __iomem			*serdes_regs_base,
	struct al_mod_serdes_grp_obj	*obj)
{
	al_mod_dbg(
		"%s(%p, %p)\n",
		__func__,
		serdes_regs_base,
		obj);

	al_mod_memset(obj, 0, sizeof(struct al_mod_serdes_grp_obj));

	obj->regs_base = (struct al_mod_serdes_regs *)serdes_regs_base;
	obj->type_get = al_mod_serdes_25g_type_get;
	obj->reg_read = al_mod_serdes_25g_reg_read;
	obj->reg_write = al_mod_serdes_25g_reg_write;
	obj->bist_pattern_select = AL_SRDS_ADV_SRVC(al_mod_serdes_25g_bist_pattern_select);
	obj->bist_tx_enable = AL_SRDS_ADV_SRVC(al_mod_serdes_25g_bist_tx_enable);
	obj->bist_rx_enable = AL_SRDS_ADV_SRVC(al_mod_serdes_25g_bist_rx_enable);
	obj->bist_rx_status = AL_SRDS_ADV_SRVC(al_mod_serdes_25g_bist_rx_status);
	obj->eye_diag_run = AL_SRDS_ADV_SRVC(al_mod_serdes_25g_eye_diag_run);
	obj->cdr_is_locked = AL_SRDS_ADV_SRVC(al_mod_serdes_25g_cdr_is_locked);
	obj->rx_valid = AL_SRDS_ADV_SRVC(al_mod_serdes_25g_rx_valid);
	obj->signal_is_detected = AL_SRDS_ADV_SRVC(al_mod_serdes_25g_signal_is_detected);
	obj->tx_advanced_params_set = AL_SRDS_ADV_SRVC(al_mod_serdes_25g_tx_advanced_params_set);
	obj->tx_advanced_params_get = AL_SRDS_ADV_SRVC(al_mod_serdes_25g_tx_advanced_params_get);
	obj->rx_advanced_params_get = AL_SRDS_ADV_SRVC(al_mod_serdes_25g_rx_advanced_params_get);
	obj->tx_diag_info_get = AL_SRDS_ADV_SRVC(al_mod_serdes_25g_tx_diag_info_get);
	obj->rx_diag_info_get = AL_SRDS_ADV_SRVC(al_mod_serdes_25g_rx_diag_info_get);
	obj->mode_set_kr = AL_SRDS_ADV_SRVC(al_mod_serdes_25g_group_cfg_10g_mode);
	obj->mode_set_25g = AL_SRDS_ADV_SRVC(al_mod_serdes_25g_group_cfg_25g_mode);
	obj->mode_set_adv = AL_SRDS_ADV_SRVC(al_mod_serdes_25g_group_cfg_mode_adv);
	obj->rx_equalization = AL_SRDS_ADV_SRVC(al_mod_serdes_25g_rx_equalization);
	obj->calc_eye_size = AL_SRDS_ADV_SRVC(al_mod_serdes_25g_calc_eye_size);
	obj->pll_lock_get = AL_SRDS_ADV_SRVC(al_mod_serdes_25g_pll_lock_get);
	obj->fw_init_status_get = AL_SRDS_ADV_SRVC(al_mod_serdes_25g_fw_init_status_get);
	obj->fw_is_alive = AL_SRDS_ADV_SRVC(al_mod_serdes_25g_is_fw_alive);
	obj->udelay_cb = al_mod_serdes_25g_udelay;
	obj->msleep_cb = al_mod_serdes_25g_msleep;
	obj->serdes_mode_class_get = AL_SRDS_ADV_SRVC(al_mod_serdes_25g_mode_class_get);
	obj->serdes_mode_get = AL_SRDS_ADV_SRVC(al_mod_serdes_25g_mode_get);

	return 0;
}
