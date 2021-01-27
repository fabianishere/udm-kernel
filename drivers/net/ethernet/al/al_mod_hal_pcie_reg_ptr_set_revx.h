/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */
#ifndef __AL_HAL_PCIE_REG_PTR_SET_REVX_H__
#define __AL_HAL_PCIE_REG_PTR_SET_REVX_H__

#include "al_mod_hal_pcie_regs.h"

void FUNC_NAME(
	struct al_mod_pcie_port	*pcie_port,
	void __iomem		*pcie_reg_base)
{
	struct REGS_REVX __iomem *regs = (struct REGS_REVX __iomem *)pcie_reg_base;
	struct al_mod_pcie_regs *reg_ptrs = pcie_port->regs;
#if (REV_ID >= AL_PCIE_REV_ID_3)
	unsigned int i;
#endif

	reg_ptrs->axi.ctrl.global = &regs->axi.ctrl.global;
	reg_ptrs->axi.ctrl.master_rctl = &regs->axi.ctrl.master_rctl;
	reg_ptrs->axi.ctrl.master_ctl = &regs->axi.ctrl.master_ctl;
	reg_ptrs->axi.ctrl.master_arctl = &regs->axi.ctrl.master_arctl;
	reg_ptrs->axi.ctrl.master_awctl = &regs->axi.ctrl.master_awctl;
	reg_ptrs->axi.ctrl.slv_ctl = &regs->axi.ctrl.slv_ctl;
	reg_ptrs->axi.ctrl.slv_wctl = &regs->axi.ctrl.slv_wctl;
	reg_ptrs->axi.ctrl.tgtid_mask = &regs->axi.ctrl.tgtid_mask;
	reg_ptrs->axi.ob_ctrl.cfg_target_bus = &regs->axi.ob_ctrl.cfg_target_bus;
	reg_ptrs->axi.ob_ctrl.cfg_control = &regs->axi.ob_ctrl.cfg_control;
	reg_ptrs->axi.ob_ctrl.io_start_l = &regs->axi.ob_ctrl.io_start_l;
	reg_ptrs->axi.ob_ctrl.io_start_h = &regs->axi.ob_ctrl.io_start_h;
	reg_ptrs->axi.ob_ctrl.io_limit_l = &regs->axi.ob_ctrl.io_limit_l;
	reg_ptrs->axi.ob_ctrl.io_limit_h = &regs->axi.ob_ctrl.io_limit_h;
#if (REV_ID >= AL_PCIE_REV_ID_3)
	reg_ptrs->axi.ob_ctrl.io_addr_mask_h = &regs->axi.ob_ctrl.io_addr_mask_h;
	reg_ptrs->axi.ob_ctrl.ar_msg_addr_mask_h = &regs->axi.ob_ctrl.ar_msg_addr_mask_h;
	reg_ptrs->axi.ob_ctrl.aw_msg_addr_mask_h = &regs->axi.ob_ctrl.aw_msg_addr_mask_h;
#endif
#if (REV_ID >= AL_PCIE_REV_ID_2)
	reg_ptrs->axi.ob_ctrl.tgtid_reg_ovrd = &regs->axi.ob_ctrl.tgtid_reg_ovrd;
	reg_ptrs->axi.ob_ctrl.addr_high_reg_ovrd_sel = &regs->axi.ob_ctrl.addr_high_reg_ovrd_sel;
	reg_ptrs->axi.ob_ctrl.addr_high_reg_ovrd_value =
		&regs->axi.ob_ctrl.addr_high_reg_ovrd_value;
	reg_ptrs->axi.ob_ctrl.addr_size_replace = &regs->axi.ob_ctrl.addr_size_replace;
#endif
	reg_ptrs->axi.rd_parity.log_high = &regs->axi.rd_parity.log_high;
	reg_ptrs->axi.rd_parity.log_low = &regs->axi.rd_parity.log_low;
	reg_ptrs->axi.rd_cmpl.cmpl_log_high = &regs->axi.rd_cmpl.cmpl_log_high;
	reg_ptrs->axi.rd_cmpl.cmpl_log_low = &regs->axi.rd_cmpl.cmpl_log_low;
	reg_ptrs->axi.rd_to.to_log_high = &regs->axi.rd_to.to_log_high;
	reg_ptrs->axi.rd_to.to_log_low = &regs->axi.rd_to.to_log_low;
	reg_ptrs->axi.wr_cmpl.wr_cmpl_log_high = &regs->axi.wr_cmpl.wr_cmpl_log_high;
	reg_ptrs->axi.wr_cmpl.wr_cmpl_log_low = &regs->axi.wr_cmpl.wr_cmpl_log_low;
	reg_ptrs->axi.wr_to.wr_to_log_high = &regs->axi.wr_to.wr_to_log_high;
	reg_ptrs->axi.wr_to.wr_to_log_low = &regs->axi.wr_to.wr_to_log_low;
	reg_ptrs->axi.pcie_global.conf = &regs->axi.pcie_global.conf;
	reg_ptrs->axi.conf.zero_lane[0] = &regs->axi.conf.zero_lane0;
	reg_ptrs->axi.conf.zero_lane[1] = &regs->axi.conf.zero_lane1;
	reg_ptrs->axi.conf.zero_lane[2] = &regs->axi.conf.zero_lane2;
	reg_ptrs->axi.conf.zero_lane[3] = &regs->axi.conf.zero_lane3;
#if (REV_ID >= AL_PCIE_REV_ID_3)
	reg_ptrs->axi.conf.zero_lane[4] = &regs->axi.conf.zero_lane4;
	reg_ptrs->axi.conf.zero_lane[5] = &regs->axi.conf.zero_lane5;
	reg_ptrs->axi.conf.zero_lane[6] = &regs->axi.conf.zero_lane6;
	reg_ptrs->axi.conf.zero_lane[7] = &regs->axi.conf.zero_lane7;
#if (REV_ID >= AL_PCIE_REV_ID_4)
	reg_ptrs->axi.conf.zero_lane[8] = &regs->axi.conf.zero_lane8;
	reg_ptrs->axi.conf.zero_lane[9] = &regs->axi.conf.zero_lane9;
	reg_ptrs->axi.conf.zero_lane[10] = &regs->axi.conf.zero_lane10;
	reg_ptrs->axi.conf.zero_lane[11] = &regs->axi.conf.zero_lane11;
	reg_ptrs->axi.conf.zero_lane[12] = &regs->axi.conf.zero_lane12;
	reg_ptrs->axi.conf.zero_lane[13] = &regs->axi.conf.zero_lane13;
	reg_ptrs->axi.conf.zero_lane[14] = &regs->axi.conf.zero_lane14;
	reg_ptrs->axi.conf.zero_lane[15] = &regs->axi.conf.zero_lane15;
	reg_ptrs->axi.conf_high.zero_lane[0] = &regs->axi.conf.zero_lane0;
	reg_ptrs->axi.conf_high.zero_lane[1] = &regs->axi.conf.zero_lane1;
	reg_ptrs->axi.conf_high.zero_lane[2] = &regs->axi.conf.zero_lane2;
	reg_ptrs->axi.conf_high.zero_lane[3] = &regs->axi.conf.zero_lane3;
	reg_ptrs->axi.conf_high.zero_lane[4] = &regs->axi.conf.zero_lane4;
	reg_ptrs->axi.conf_high.zero_lane[5] = &regs->axi.conf.zero_lane5;
	reg_ptrs->axi.conf_high.zero_lane[6] = &regs->axi.conf.zero_lane6;
	reg_ptrs->axi.conf_high.zero_lane[7] = &regs->axi.conf.zero_lane7;
	reg_ptrs->axi.conf_high.zero_lane[8] = &regs->axi.conf.zero_lane8;
	reg_ptrs->axi.conf_high.zero_lane[9] = &regs->axi.conf.zero_lane9;
	reg_ptrs->axi.conf_high.zero_lane[10] = &regs->axi.conf.zero_lane10;
	reg_ptrs->axi.conf_high.zero_lane[11] = &regs->axi.conf.zero_lane11;
	reg_ptrs->axi.conf_high.zero_lane[12] = &regs->axi.conf.zero_lane12;
	reg_ptrs->axi.conf_high.zero_lane[13] = &regs->axi.conf.zero_lane13;
	reg_ptrs->axi.conf_high.zero_lane[14] = &regs->axi.conf.zero_lane14;
	reg_ptrs->axi.conf_high.zero_lane[15] = &regs->axi.conf.zero_lane15;
#endif
#endif
	reg_ptrs->axi.status.lane[0] = &regs->axi.status.lane0;
	reg_ptrs->axi.status.lane[1] = &regs->axi.status.lane1;
	reg_ptrs->axi.status.lane[2] = &regs->axi.status.lane2;
	reg_ptrs->axi.status.lane[3] = &regs->axi.status.lane3;
#if (REV_ID >= AL_PCIE_REV_ID_3)
	reg_ptrs->axi.status.lane[4] = &regs->axi.status.lane4;
	reg_ptrs->axi.status.lane[5] = &regs->axi.status.lane5;
	reg_ptrs->axi.status.lane[6] = &regs->axi.status.lane6;
	reg_ptrs->axi.status.lane[7] = &regs->axi.status.lane7;
#if (REV_ID >= AL_PCIE_REV_ID_4)
	reg_ptrs->axi.status.lane[8] = &regs->axi.status.lane8;
	reg_ptrs->axi.status.lane[9] = &regs->axi.status.lane9;
	reg_ptrs->axi.status.lane[10] = &regs->axi.status.lane10;
	reg_ptrs->axi.status.lane[11] = &regs->axi.status.lane11;
	reg_ptrs->axi.status.lane[12] = &regs->axi.status.lane12;
	reg_ptrs->axi.status.lane[13] = &regs->axi.status.lane13;
	reg_ptrs->axi.status.lane[14] = &regs->axi.status.lane14;
	reg_ptrs->axi.status.lane[15] = &regs->axi.status.lane15;
	reg_ptrs->axi.status_high.lane[0] = &regs->axi.status.lane0;
	reg_ptrs->axi.status_high.lane[1] = &regs->axi.status.lane1;
	reg_ptrs->axi.status_high.lane[2] = &regs->axi.status.lane2;
	reg_ptrs->axi.status_high.lane[3] = &regs->axi.status.lane3;
	reg_ptrs->axi.status_high.lane[4] = &regs->axi.status.lane4;
	reg_ptrs->axi.status_high.lane[5] = &regs->axi.status.lane5;
	reg_ptrs->axi.status_high.lane[6] = &regs->axi.status.lane6;
	reg_ptrs->axi.status_high.lane[7] = &regs->axi.status.lane7;
	reg_ptrs->axi.status_high.lane[8] = &regs->axi.status.lane8;
	reg_ptrs->axi.status_high.lane[9] = &regs->axi.status.lane9;
	reg_ptrs->axi.status_high.lane[10] = &regs->axi.status.lane10;
	reg_ptrs->axi.status_high.lane[11] = &regs->axi.status.lane11;
	reg_ptrs->axi.status_high.lane[12] = &regs->axi.status.lane12;
	reg_ptrs->axi.status_high.lane[13] = &regs->axi.status.lane13;
	reg_ptrs->axi.status_high.lane[14] = &regs->axi.status.lane14;
	reg_ptrs->axi.status_high.lane[15] = &regs->axi.status.lane15;
#endif
#endif
	reg_ptrs->axi.parity.en_axi = &regs->axi.parity.en_axi;
	reg_ptrs->axi.parity.status_axi = &regs->axi.parity.status_axi;
	reg_ptrs->axi.pos_logged.error_low = &regs->axi.pos_logged.error_low;
	reg_ptrs->axi.pos_logged.error_high = &regs->axi.pos_logged.error_high;
	reg_ptrs->axi.ordering.pos_cntl = &regs->axi.ordering.pos_cntl;
	reg_ptrs->axi.link_down.reset_extend = &regs->axi.link_down.reset_extend;
	reg_ptrs->axi.pre_configuration.pcie_core_setup =
		&regs->axi.pre_configuration.pcie_core_setup;
	reg_ptrs->axi.init_fc.cfg = &regs->axi.init_fc.cfg;
	reg_ptrs->axi.int_grp_a = &regs->axi.int_grp_a;

#if (REV_ID >= AL_PCIE_REV_ID_4)
	reg_ptrs->axi.int_grp_b = &regs->axi.int_grp_b;
#endif
#if (REV_ID >= AL_PCIE_REV_ID_3)
	reg_ptrs->axi.axi_attr_ovrd.write_msg_ctrl_0 = &regs->axi.axi_attr_ovrd.write_msg_ctrl_0;
	reg_ptrs->axi.axi_attr_ovrd.write_msg_ctrl_1 = &regs->axi.axi_attr_ovrd.write_msg_ctrl_1;
	reg_ptrs->axi.axi_attr_ovrd.pf_sel = &regs->axi.axi_attr_ovrd.pf_sel;

#if (REV_ID >= AL_PCIE_REV_ID_4)
	reg_ptrs->axi.axi_attr_ovrd.mapsel = &regs->axi.axi_attr_ovrd.mapsel;
#endif
	for (i = 0; i < AL_MAX_NUM_OF_PFS; i++) {
		reg_ptrs->axi.pf_axi_attr_ovrd[i].func_ctrl_0 =
			&regs->axi.pf_axi_attr_ovrd[i].func_ctrl_0;
		reg_ptrs->axi.pf_axi_attr_ovrd[i].func_ctrl_1 =
			&regs->axi.pf_axi_attr_ovrd[i].func_ctrl_1;
		reg_ptrs->axi.pf_axi_attr_ovrd[i].func_ctrl_2 =
			&regs->axi.pf_axi_attr_ovrd[i].func_ctrl_2;
		reg_ptrs->axi.pf_axi_attr_ovrd[i].func_ctrl_3 =
			&regs->axi.pf_axi_attr_ovrd[i].func_ctrl_3;
		reg_ptrs->axi.pf_axi_attr_ovrd[i].func_ctrl_4 =
			&regs->axi.pf_axi_attr_ovrd[i].func_ctrl_4;
		reg_ptrs->axi.pf_axi_attr_ovrd[i].func_ctrl_5 =
			&regs->axi.pf_axi_attr_ovrd[i].func_ctrl_5;
		reg_ptrs->axi.pf_axi_attr_ovrd[i].func_ctrl_6 =
			&regs->axi.pf_axi_attr_ovrd[i].func_ctrl_6;
		reg_ptrs->axi.pf_axi_attr_ovrd[i].func_ctrl_7 =
			&regs->axi.pf_axi_attr_ovrd[i].func_ctrl_7;
		reg_ptrs->axi.pf_axi_attr_ovrd[i].func_ctrl_8 =
			&regs->axi.pf_axi_attr_ovrd[i].func_ctrl_8;
		reg_ptrs->axi.pf_axi_attr_ovrd[i].func_ctrl_9 =
			&regs->axi.pf_axi_attr_ovrd[i].func_ctrl_9;
#if (REV_ID >= AL_PCIE_REV_ID_4)
		reg_ptrs->axi.pf_axi_attr_ovrd[i].func_ctrl_10 =
			&regs->axi.pf_axi_attr_ovrd[i].func_ctrl_10;
#endif
	}

	reg_ptrs->axi.msg_attr_axuser_table.entry_vec = &regs->axi.msg_attr_axuser_table.entry_vec;
#endif
#if (REV_ID >= AL_PCIE_REV_ID_4)
	reg_ptrs->axi.msg_attr_axuser_table.code_entry_vec_0 =
		&regs->axi.msg_attr_axuser_table.code_entry_vec_0;
	reg_ptrs->axi.msg_attr_axuser_table.code_entry_vec_1 =
		&regs->axi.msg_attr_axuser_table.code_entry_vec_1;
	reg_ptrs->axi.cfg_outbound_rd_len.len_shaper = &regs->axi.cfg_outbound_rd_len.len_shaper;
	reg_ptrs->axi.mstr_pos.flush = &regs->axi.mstr_pos.flush;
	reg_ptrs->axi.mstr_attr_map.ctrl0 = &regs->axi.mstr_attr_map.ctrl0;
	reg_ptrs->axi.mstr_attr_map.ctrl1 = &regs->axi.mstr_attr_map.ctrl1;

	al_mod_assert(AL_ARR_SIZE(reg_ptrs->axi.mstr_cfgtable) == AL_ARR_SIZE(regs->axi.mstr_cfgtable));
	for (i = 0; i < AL_ARR_SIZE(reg_ptrs->axi.mstr_cfgtable); i++)
		reg_ptrs->axi.mstr_cfgtable[i].entry = &regs->axi.mstr_cfgtable[i].entry;

	reg_ptrs->axi.mstr_pos.flush = &regs->axi.mstr_pos.flush;
	reg_ptrs->axi.cfg_pclk_generate.cntr_low = &regs->axi.cfg_pclk_generate.cntr_low;
	reg_ptrs->axi.cfg_pclk_generate.cntr_high = &regs->axi.cfg_pclk_generate.cntr_high;
	reg_ptrs->axi.cfg_pclk_generate_status.status_low =
		&regs->axi.cfg_pclk_generate_status.status_low;
#endif

	reg_ptrs->app.global_ctrl.port_init = &regs->app.global_ctrl.port_init;
	reg_ptrs->app.global_ctrl.pm_control = &regs->app.global_ctrl.pm_control;

#if (REV_ID >= AL_PCIE_REV_ID_3)
	for (i = 0; i < AL_MAX_NUM_OF_PFS; i++) {
		reg_ptrs->app.global_ctrl.events_gen[i] =
			&regs->app.events_gen_per_func[i].events_gen;
	}
#else
	reg_ptrs->app.global_ctrl.events_gen[0] = &regs->app.global_ctrl.events_gen;
#endif

#if (REV_ID >= AL_PCIE_REV_ID_2)
	reg_ptrs->app.global_ctrl.corr_err_sts_int = &regs->app.global_ctrl.pended_corr_err_sts_int;
	reg_ptrs->app.global_ctrl.uncorr_err_sts_int =
		&regs->app.global_ctrl.pended_uncorr_err_sts_int;
	reg_ptrs->app.global_ctrl.sris_kp_counter = &regs->app.global_ctrl.sris_kp_counter_value;
#endif
	reg_ptrs->app.debug.info_0 = &regs->app.debug.info_0;
	reg_ptrs->app.debug.info_1 = &regs->app.debug.info_1;
	reg_ptrs->app.debug.info_2 = &regs->app.debug.info_2;
	reg_ptrs->app.debug.info_3 = &regs->app.debug.info_3;
#if (REV_ID >= AL_PCIE_REV_ID_2)
	reg_ptrs->app.ap_user_send_msg.req_info = &regs->app.ap_user_send_msg.req_info;
	reg_ptrs->app.ap_user_send_msg.ack_info = &regs->app.ap_user_send_msg.ack_info;
#endif

#if (REV_ID >= AL_PCIE_REV_ID_3)
	for (i = 0; i < AL_MAX_NUM_OF_PFS; i++) {
		reg_ptrs->app.soc_int[i].mask_inta_leg_0 =
			&regs->app.soc_int_per_func[i].mask_inta_leg_0;
		reg_ptrs->app.soc_int[i].mask_inta_leg_1 =
			&regs->app.soc_int_per_func[i].mask_inta_leg_1;
		reg_ptrs->app.soc_int[i].mask_inta_leg_2 =
			&regs->app.soc_int_per_func[i].mask_inta_leg_2;
		reg_ptrs->app.soc_int[i].mask_inta_leg_3 =
			&regs->app.soc_int_per_func[i].mask_inta_leg_3;
#if (REV_ID >= AL_PCIE_REV_ID_4)
		reg_ptrs->app.soc_int[i].mask_inta_leg_6 =
			&regs->app.soc_int_per_func[i].mask_inta_leg_6;
#endif
		reg_ptrs->app.soc_int[i].mask_msi_leg_0 =
			&regs->app.soc_int_per_func[i].mask_msi_leg_0;
		reg_ptrs->app.soc_int[i].mask_msi_leg_3 =
			&regs->app.soc_int_per_func[i].mask_msi_leg_3;
#if (REV_ID >= AL_PCIE_REV_ID_4)
		reg_ptrs->app.soc_int[i].mask_msi_leg_6 =
			&regs->app.soc_int_per_func[i].mask_msi_leg_6;
#endif
	}
#else
	reg_ptrs->app.soc_int[0].mask_inta_leg_0 = &regs->app.soc_int.mask_inta_leg_0;
	reg_ptrs->app.soc_int[0].mask_inta_leg_1 = &regs->app.soc_int.mask_inta_leg_1;
	reg_ptrs->app.soc_int[0].mask_inta_leg_2 = &regs->app.soc_int.mask_inta_leg_2;
#if (REV_ID >= AL_PCIE_REV_ID_2)
		reg_ptrs->app.soc_int[0].mask_inta_leg_3 = &regs->app.soc_int.mask_inta_leg_3;
#endif
	reg_ptrs->app.soc_int[0].mask_msi_leg_0 = &regs->app.soc_int.mask_msi_leg_0;
#if (REV_ID >= AL_PCIE_REV_ID_2)
		reg_ptrs->app.soc_int[0].mask_msi_leg_3 = &regs->app.soc_int.mask_msi_leg_3;
#endif
#endif

	reg_ptrs->app.ctrl_gen.features = &regs->app.ctrl_gen.features;
	reg_ptrs->app.parity.en_core = &regs->app.parity.en_core;
	reg_ptrs->app.parity.status_core = &regs->app.parity.status_core;
	reg_ptrs->app.link_down.reset_delay = &regs->app.link_down.reset_delay;
	reg_ptrs->app.link_down.reset_extend_rsrvd = &regs->app.link_down.reset_extend_rsrvd;
	reg_ptrs->app.atu.in_mask_pair = regs->app.atu.in_mask_pair;
	reg_ptrs->app.atu.out_mask_pair = regs->app.atu.out_mask_pair;

#if (REV_ID >= AL_PCIE_REV_ID_3)
	reg_ptrs->app.atu.reg_out_mask = &regs->app.atu.reg_out_mask;
	reg_ptrs->app.cfg_func_ext.cfg = &regs->app.cfg_func_ext.cfg;

	for (i = 0; i < AL_MAX_NUM_OF_PFS; i++) {
		reg_ptrs->app.status_per_func[i].status_per_func = &regs->app.status_per_func[i].status_per_func;
		reg_ptrs->app.pm_state_per_func[i].pm_state_per_func = &regs->app.pm_state_per_func[i].pm_state_per_func;
	}
#elif (REV_ID >= AL_PCIE_REV_ID_2)
	reg_ptrs->app.status_per_func[0].status_per_func = &regs->app.status_per_func.status_per_func;
#endif

#if (REV_ID >= AL_PCIE_REV_ID_4)
	reg_ptrs->app.cfg_blockalign.cfg_ext_cntl_1 = &regs->app.cfg_blockalign.cfg_ext_cntl_1;
#endif

	reg_ptrs->app.int_grp_a = &regs->app.int_grp_a;
	reg_ptrs->app.int_grp_b = &regs->app.int_grp_b;
#if (REV_ID >= AL_PCIE_REV_ID_3)
	reg_ptrs->app.int_grp_c = &regs->app.int_grp_c;
	reg_ptrs->app.int_grp_d = &regs->app.int_grp_d;

	for (i = 0; i < AL_MAX_NUM_OF_PFS; i++) {
		reg_ptrs->core_space[i].config_header = regs->core_space.func[i].config_header;
		reg_ptrs->core_space[i].pcie_pm_cap_base =
			&regs->core_space.func[i].pcie_pm_cap_base;
		reg_ptrs->core_space[i].pcie_cap_base = &regs->core_space.func[i].pcie_cap_base;
		reg_ptrs->core_space[i].pcie_dev_cap_base =
			&regs->core_space.func[i].pcie_dev_cap_base;
		reg_ptrs->core_space[i].pcie_dev_ctrl_status =
			&regs->core_space.func[i].pcie_dev_ctrl_status;
		reg_ptrs->core_space[i].pcie_link_cap_base =
			&regs->core_space.func[i].pcie_link_cap_base;
		reg_ptrs->core_space[i].pcie_link_ctrl_status =
			&regs->core_space.func[i].pcie_link_ctrl_status;
		reg_ptrs->core_space[i].msix_cap_base = &regs->core_space.func[i].msix_cap_base;
		reg_ptrs->core_space[i].aer = &regs->core_space.func[i].aer;
		/*
		 * Addressing RMN: 11448
		 *
		 * RMN description:
		 * TPH cap is assigned on top of PCIe secondary capability,
		 * the size of secondary capability is changed according to
		 * max link width (due to per lane eq params). this puts the TPH cap
		 * in different offset on the three version we have (X4 , X8 : 0x178 , X16 : 0x188).
		 *
		 * Software flow:
		 * change TPH pointer location to proper value on X16 ports
		 */
		if (pcie_port->subrev_id == PCIE_AXI_DEVICE_ID_REG_REV_ID_X16)
			reg_ptrs->core_space[i].tph_cap_base =
					&regs->core_space.func[i].tph_cap_base_x16;
		else
			reg_ptrs->core_space[i].tph_cap_base =
					&regs->core_space.func[i].tph_cap_base;
	}

	/* secondary extension capability only for PF0 */
	reg_ptrs->core_space[0].pcie_sec_ext_cap_base =
		&regs->core_space.func[0].pcie_sec_ext_cap_base;

	reg_ptrs->port_regs = &regs->core_space.func[0].port_regs;
#else
	reg_ptrs->core_space[0].config_header = regs->core_space.config_header;
	reg_ptrs->core_space[0].pcie_pm_cap_base = &regs->core_space.pcie_pm_cap_base;
	reg_ptrs->core_space[0].pcie_cap_base = &regs->core_space.pcie_cap_base;
	reg_ptrs->core_space[0].pcie_dev_cap_base = &regs->core_space.pcie_dev_cap_base;
	reg_ptrs->core_space[0].pcie_dev_ctrl_status = &regs->core_space.pcie_dev_ctrl_status;
	reg_ptrs->core_space[0].pcie_link_cap_base = &regs->core_space.pcie_link_cap_base;
	reg_ptrs->core_space[0].pcie_link_ctrl_status = &regs->core_space.pcie_link_ctrl_status;
	reg_ptrs->core_space[0].msix_cap_base = &regs->core_space.msix_cap_base;
	reg_ptrs->core_space[0].aer = &regs->core_space.aer;
	reg_ptrs->core_space[0].pcie_sec_ext_cap_base = &regs->core_space.pcie_sec_ext_cap_base;

	reg_ptrs->port_regs = &regs->core_space.port_regs;
#endif
}

#endif

