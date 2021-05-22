/*
 * Copyright 2016, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

/**
 *  @{
 * @file   al_mod_hal_serdes_avg_utils.c
 *
 * @brief Avago SerDes utilities
 *
 */

#include "al_mod_hal_serdes_avg_utils.h"
#include "al_mod_hal_serdes_avg_internal.h"
#include "al_mod_hal_serdes_avg_lane_regs.h"


#define COMPLEX_TYPE_IS_TR(type) (((type) == AL_SRDS_COMPLEX_TYPE_TR_MAIN) || \
	((type) == AL_SRDS_COMPLEX_TYPE_TR_SEC) || \
	((type) == AL_SRDS_COMPLEX_TYPE_TR_HS))

#define SERDES_WAIT_FOR_READY_TIMEOUT		1000 /* 1m Sec */

unsigned int al_mod_serdes_avg_sbus_addr_serdes(
	enum al_mod_serdes_complex_type	complex_type,
	unsigned int			lane_idx)
{
	unsigned int receiver_addr;

	switch (complex_type) {
	case AL_SRDS_COMPLEX_TYPE_TNK_MAIN:
		receiver_addr = AL_SBUS_ADDR_SERDES_TNK_MAIN(lane_idx);
		break;
	case AL_SRDS_COMPLEX_TYPE_TR_TC_MAIN:
		receiver_addr = AL_SBUS_ADDR_SERDES_TR_TC_MAIN(lane_idx);
		break;
	case AL_SRDS_COMPLEX_TYPE_TR_MAIN:
		receiver_addr = AL_SBUS_ADDR_SERDES_TR_MAIN(lane_idx);
		break;
	case AL_SRDS_COMPLEX_TYPE_TR_SEC:
		receiver_addr = AL_SBUS_ADDR_SERDES_TR_SEC(lane_idx);
		break;
	case AL_SRDS_COMPLEX_TYPE_TR_HS:
		receiver_addr = AL_SBUS_ADDR_SERDES_TR_HS(lane_idx);
		break;
	default:
		al_mod_err("%s: invalid complex type (%d)\n", __func__, complex_type);
		receiver_addr = 0;
		al_mod_assert(0);
	}

	return receiver_addr;
}

unsigned int al_mod_ratio_field_val_get(
	enum al_mod_serdes_clk_freq	ref_clk_freq,
	enum al_mod_serdes_bit_rate	bit_rate)
{
	uint32_t input_rate;
	uint32_t output_rate;

	switch (ref_clk_freq) {
	case AL_SRDS_CLK_FREQ_100_MHZ:
		input_rate = 100000;
		switch (bit_rate) {
		case AL_SRDS_BIT_RATE_6_0G:
			output_rate = 6000000;
			break;
		default:
			al_mod_err("%s: invalid combination of ref clk freq (%d) and bit rate (%d)\n",
				__func__, ref_clk_freq, bit_rate);
			return 0;
		}
		break;

	case AL_SRDS_CLK_FREQ_156_MHZ:
		input_rate = 156250;
		switch (bit_rate) {
		case AL_SRDS_BIT_RATE_1_25G:
			output_rate = 1250000;
			break;
		case AL_SRDS_BIT_RATE_10_3125G:
			output_rate = 10312500;
			break;
		case AL_SRDS_BIT_RATE_25_78G:
			output_rate = 25781250;
			break;
		default:
			al_mod_err("%s: invalid combination of ref clk freq (%d) and bit rate (%d)\n",
				__func__, ref_clk_freq, bit_rate);
			return 0;
		}
		break;

	default:
		al_mod_err("%s: invalid ref clk freq (%d)\n", __func__, ref_clk_freq);
		return 0;
	}

	return output_rate / input_rate;
}

unsigned int al_mod_width_field_val_get(
	enum al_mod_serdes_bit_width	bit_width)
{
	switch (bit_width) {
	case AL_SRDS_BIT_WIDTH_10:
		return (CORE_INT_WIDTH_MODE_TX_WIDTH_VAL_10 << CORE_INT_WIDTH_MODE_TX_WIDTH_SHIFT) |
			(CORE_INT_WIDTH_MODE_RX_WIDTH_VAL_10 << CORE_INT_WIDTH_MODE_RX_WIDTH_SHIFT);
	case AL_SRDS_BIT_WIDTH_20:
		return (CORE_INT_WIDTH_MODE_TX_WIDTH_VAL_20 << CORE_INT_WIDTH_MODE_TX_WIDTH_SHIFT) |
			(CORE_INT_WIDTH_MODE_RX_WIDTH_VAL_20 << CORE_INT_WIDTH_MODE_RX_WIDTH_SHIFT);
	case AL_SRDS_BIT_WIDTH_32:
		return (CORE_INT_WIDTH_MODE_TX_WIDTH_VAL_32 << CORE_INT_WIDTH_MODE_TX_WIDTH_SHIFT) |
			(CORE_INT_WIDTH_MODE_RX_WIDTH_VAL_32 << CORE_INT_WIDTH_MODE_RX_WIDTH_SHIFT);
	case AL_SRDS_BIT_WIDTH_40:
		return (CORE_INT_WIDTH_MODE_TX_WIDTH_VAL_40 << CORE_INT_WIDTH_MODE_TX_WIDTH_SHIFT) |
			(CORE_INT_WIDTH_MODE_RX_WIDTH_VAL_40 << CORE_INT_WIDTH_MODE_RX_WIDTH_SHIFT);
	default:
		al_mod_err("%s: invalid bit width (%d)\n", __func__, bit_width);
	}

	return 0;
}

al_mod_bool al_mod_serdes_avg_signal_loss_sticky_get(
	struct al_mod_serdes_grp_obj	*obj,
	al_mod_bool				clear)
{
	unsigned int reg_val = al_mod_serdes_avg_core_interrupt(obj,
		CORE_INT_ESB_REG_RD(ESB_REG_SIG_LOSS_STAT_REG_NUM), 0);
	al_mod_bool is_set = !!(reg_val & ESB_REG_SIG_LOSS_STAT_SIG_LOST);

	if (clear && is_set) {
		reg_val &= ~ESB_REG_SIG_LOSS_STAT_SIG_LOST;
		al_mod_serdes_avg_core_interrupt(obj,
			CORE_INT_ESB_REG_WR(ESB_REG_SIG_LOSS_STAT_REG_NUM), reg_val);
	}

	return is_set;
}

int al_mod_serdes_avg_wait_for_o_tx_ready(
	struct al_mod_serdes_grp_obj	*obj)
{
	struct al_mod_srd_1lane_regs __iomem *lane_regs = obj->regs_base;
	uint32_t timeout = SERDES_WAIT_FOR_READY_TIMEOUT;

	while ((!(obj->complex_obj->reg_read32(&lane_regs->gen.status) &
		SRD_1LANE_GEN_STATUS_TX_RDY)) && (timeout > 0)) {
		al_mod_udelay(1);
		timeout--;
	}

	if (!timeout) {
		al_mod_err("%s failed due timeout\n", __func__);
		return -EINVAL;
	}

	return 0;
}

int al_mod_serdes_avg_wait_for_o_rx_ready(
	struct al_mod_serdes_grp_obj	*obj)
{
	struct al_mod_srd_1lane_regs __iomem *lane_regs = obj->regs_base;
	uint32_t timeout = SERDES_WAIT_FOR_READY_TIMEOUT;

	while ((!(obj->complex_obj->reg_read32(&lane_regs->gen.status) &
		SRD_1LANE_GEN_STATUS_RX_RDY)) && (timeout > 0)) {
		al_mod_udelay(1);
		timeout--;
	}

	if (!timeout) {
		al_mod_err("%s failed due timeout\n", __func__);
		return -EINVAL;
	}

	return 0;
}

al_mod_bool al_mod_serdes_avg_rx_tx_reset_is_done(
	struct al_mod_serdes_grp_obj	*obj)
{
	int err;

	/* For HAD case, wait for o_tx_rdy & o_rx_rdy to assert before calling init complete */
	err = al_mod_serdes_avg_wait_for_o_tx_ready(obj);
	if (err)
		return AL_FALSE;

	err = al_mod_serdes_avg_wait_for_o_rx_ready(obj);
	if (err)
		return AL_FALSE;

	return AL_TRUE;
}

int al_mod_serdes_avg_rx_tx_reset(
	struct al_mod_serdes_grp_obj	*obj,
	al_mod_bool is_eth)
{
	if (is_eth) {
		/* Disabled Tx/rx serdes */
		al_mod_serdes_avg_core_interrupt(obj,
			CORE_INT_SRDS_EN_REG_NUM, CORE_INT_SRDS_EN_DIS);

		/* Calibrate the electrical idle detector to 89.3mV */
		al_mod_serdes_avg_core_interrupt(obj,
			CORE_INT_EI_THR_REG_NUM, CORE_INT_EI_THR_ETH_DEFAULT);
	}

	/* TX phase calibration enable */
	al_mod_serdes_avg_core_interrupt(obj,
		CORE_INT_TX_PHASE_CAL_REG_NUM,
		CORE_INT_TX_PHASE_CAL_EN);

	if (is_eth) {
		if (COMPLEX_TYPE_IS_TR(obj->complex_obj->complex_type))
			al_mod_serdes_avg_core_interrupt(obj, CORE_INT_IDLE_DETECTOR_REG_NUM,
				CORE_INT_IDLE_DETECTOR_RX_IDLE_DETECTOR_EN);

		/* Set cal flag to recalibrate PLL when TX/RX_en transaction from 0->1 */
		if (COMPLEX_TYPE_IS_TR(obj->complex_obj->complex_type))
			al_mod_serdes_avg_core_interrupt(obj,
				CORE_INT_SRDS_PLL_RECAL,
				CORE_INT_SRDS_PLL_RECAL_TX_PLL_RECAL
				| CORE_INT_SRDS_PLL_RECAL_RX_PLL_RECAL);
	}

	/* Set Tx/rx_en to one */
	al_mod_serdes_avg_core_interrupt_no_wait(obj,
		CORE_INT_SRDS_EN_REG_NUM, CORE_INT_SRDS_EN_TX_EN |
		CORE_INT_SRDS_EN_RX_EN | CORE_INT_SRDS_EN_OUT_EN);

	/* Wait for Tx/rx_en interrupt completion */
	al_mod_serdes_avg_core_interrupt_res_wait_and_get(obj);

	if (is_eth) {
		/* Configure Tx/Rx termination (Ethernet) */
		al_mod_serdes_avg_core_interrupt(obj, CORE_INT_TX_RX_TERM_REG_NUM,
			(obj->complex_obj->complex_type == AL_SRDS_COMPLEX_TYPE_TR_TC_MAIN ?
			 CORE_INT_TX_RX_TERM_VAL_DEFAULT_TC :
			 CORE_INT_TX_RX_TERM_VAL_DEFAULT));

		/* Disable electrical idle detection */
		if (COMPLEX_TYPE_IS_TR(obj->complex_obj->complex_type))
			al_mod_serdes_avg_core_interrupt(obj, CORE_INT_IDLE_DETECTOR_REG_NUM, 0);
	}

	/* For HAD case, wait for o_tx_rdy & o_rx_rdy to assert before calling init
	   complete */
	if (!al_mod_serdes_avg_rx_tx_reset_is_done(obj))
		return -EAGAIN;

	return 0;
}

/** @} */
