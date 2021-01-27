/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#include "al_hal_serdes_snps.h"

/******************************************************************************/
/******************************************************************************/
static enum al_serdes_type al_serdes_snps_type_get(void)
{
	return AL_SRDS_TYPE_SNPS;
}

/******************************************************************************/
/******************************************************************************/
void al_serdes_snps_complex_handle_init(
	void __iomem			*regs_base,
	enum al_serdes_complex_type	complex_type,
	struct al_serdes_complex_obj	*obj)
{
	al_dbg(
		"%s(%p, %d, %p)\n",
		__func__,
		regs_base,
		complex_type,
		obj);

	al_assert(regs_base);
	al_assert(obj);

	al_memset(obj, 0, sizeof(struct al_serdes_complex_obj));

	obj->regs_base = regs_base;
	obj->complex_type = complex_type;
}

/******************************************************************************/
/******************************************************************************/
void al_serdes_snps_quad_handle_init(
	void __iomem			*regs_base,
	struct al_serdes_complex_obj	*complex_obj,
	unsigned int			quad_in_complex,
	struct al_serdes_grp_obj	*obj)
{
	al_dbg(
		"%s(%p, %p, %d, %p)\n",
		__func__,
		regs_base,
		complex_obj,
		quad_in_complex,
		obj);

	al_assert(regs_base);
	al_assert(complex_obj);
	al_assert(!complex_obj->quads[quad_in_complex]);
	al_assert(obj);

	al_memset(obj, 0, sizeof(struct al_serdes_grp_obj));

	obj->regs_base = regs_base;
	obj->complex_obj = complex_obj;
	obj->quad_in_complex = quad_in_complex;

	obj->type_get = al_serdes_snps_type_get;
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
	/* obj->loopback_control = loopback_control; */
	/* obj->bist_pattern_select = bist_pattern_select; */
	/* obj->bist_tx_enable = bist_tx_enable; */
	/* obj->bist_tx_err_inject = bist_tx_err_inject; */
	/* obj->bist_rx_enable = bist_rx_enable; */
	/* obj->bist_rx_status = bist_rx_status; */
	/*  obj->tx_deemph_preset = tx_deemph_preset; */
	/*  obj->tx_deemph_inc = tx_deemph_inc; */
	/*  obj->tx_deemph_dec = tx_deemph_dec; */
	/*  obj->eye_measure_run = eye_measure_run; */
	/*  obj->eye_diag_sample = eye_diag_sample; */
	/*  obj->eye_diag_run = eye_diag_run; */
	/* obj->signal_is_detected = signal_is_detected; */
	/* obj->cdr_is_locked = cdr_is_locked; */
	/* obj->rx_valid = rx_valid; */
	/*  obj->tx_advanced_params_set = tx_advanced_params_set; */
	/*  obj->tx_advanced_params_get = tx_advanced_params_get; */
	/*  obj->rx_advanced_params_set = rx_advanced_params_set; */
	/*  obj->rx_advanced_params_get = rx_advanced_params_get; */
	/*  obj->mode_set_sgmii = mode_set_sgmii; */
	/*  obj->mode_set_kr = mode_set_kr; */
	/* obj->rx_equalization = rx_equalization; */
	/*  obj->calc_eye_size = calc_eye_size; */
	/*  obj->sris_config = sris_config; */
	/*  obj->dcgain_set = dcgain_set; */
	/* obj->pcs_interrupt_enable_set = pcs_interrupt_enable_set; */
	/* obj->pcie_run_ical_config = pcie_run_ical_config; */
	/* obj->fw_init_status_get = fw_init_status_get; */
	/* obj->ical_start = ical_start; */
	/* obj->ical_wait_for_completion = ical_wait_for_completion; */
	/* obj->pcal_start = pcal_start; */
	/* obj->pcal_wait_for_completion = pcal_wait_for_completion; */
	/* obj->pcal_adaptive_start = pcal_adaptive_start; */
	/* obj->pcal_adaptive_stop = pcal_adaptive_stop; */

	obj->complex_obj->quads[quad_in_complex] = obj;
}


