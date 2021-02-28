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

#ifndef __AL_HAL_PCIE_REGS_H__
#define __AL_HAL_PCIE_REGS_H__

/* Note: Definitions before the includes so axi/wrapper regs sees them */

/** Maximum physical functions supported */
#define REV1_2_MAX_NUM_OF_PFS	1
#define REV3_MAX_NUM_OF_PFS	4
#define REV4_MAX_NUM_OF_PFS	4
#define REV5_MAX_NUM_OF_PFS	4
#define AL_MAX_NUM_OF_PFS	4 /* the maximum between all Revisions */

#include "al_hal_pcie_axi_reg.h"
#ifndef AL_PCIE_EX
#include "al_hal_pcie_w_reg.h"
#else
#include "al_hal_pcie_w_reg_ex.h"
#endif

#define AL_PCIE_AXI_REGS_OFFSET			0x0
#define AL_PCIE_REV_1_2_APP_REGS_OFFSET		0x1000
#define AL_PCIE_REV_3_APP_REGS_OFFSET		0x2000
#define AL_PCIE_REV_4_APP_REGS_OFFSET		0x2000
#define AL_PCIE_REV_5_APP_REGS_OFFSET		0x2000
#define AL_PCIE_REV_1_2_CORE_CONF_BASE_OFFSET	0x2000
#define AL_PCIE_REV_3_CORE_CONF_BASE_OFFSET	0x10000
#define AL_PCIE_REV_4_CORE_CONF_BASE_OFFSET	0x10000
#define AL_PCIE_REV_5_CORE_CONF_BASE_OFFSET	0x10000

/** Maximum number of lanes supported */
#define AL_MAX_NUM_OF_LANES	16

/** Number of outbound atu regions - rev 1/2 */
#define AL_PCIE_REV_1_2_ATU_NUM_OUTBOUND_REGIONS 12
/** Number of outbound atu regions - rev 3 */
#define AL_PCIE_REV_3_4_ATU_NUM_OUTBOUND_REGIONS 16

struct al_pcie_core_iatu_regs {
	uint32_t index;
	uint32_t cr1;
	uint32_t cr2;
	uint32_t lower_base_addr;
	uint32_t upper_base_addr;
	uint32_t limit_addr;
	uint32_t lower_target_addr;
	uint32_t upper_target_addr;
	uint32_t cr3;
	uint32_t rsrvd[(0x270 - 0x224) >> 2];
};

struct al_pcie_core_port_regs {
	uint32_t ack_lat_rply_timer;
	uint32_t reserved1[(0xc - 0x4) >> 2];
	uint32_t ack_f_aspm_ctrl;
	uint32_t port_link_ctrl;
	uint32_t reserved2[(0x18 - 0x14) >> 2];
	uint32_t timer_ctrl_max_func_num;
	uint32_t filter_mask_reg_1;
	uint32_t reserved3[(0x48 - 0x20) >> 2];
	uint32_t vc0_posted_rcv_q_ctrl;
	uint32_t vc0_non_posted_rcv_q_ctrl;
	uint32_t vc0_comp_rcv_q_ctrl;
	uint32_t reserved4[(0x10C - 0x54) >> 2];
	uint32_t gen2_ctrl;
	uint32_t reserved5[(0x190 - 0x110) >> 2];
	uint32_t gen3_ctrl;
	uint32_t gen3_eq_fs_lf;
	uint32_t gen3_eq_preset_to_coef_map;
	uint32_t gen3_eq_preset_idx;
	uint32_t reserved6;
	uint32_t gen3_eq_status;
	uint32_t gen3_eq_ctrl;
	uint32_t gen3_eq_fb_mode_dir_chg;
	uint32_t reserved7[(0x1B8 - 0x1B0) >> 2];
	uint32_t pipe_loopback_ctrl;
	uint32_t rd_only_wr_en;
	uint32_t reserved8[(0x1D0 - 0x1C0) >> 2];
	uint32_t axi_slave_err_resp;
	uint32_t reserved9[(0x200 - 0x1D4) >> 2];
	struct al_pcie_core_iatu_regs iatu;
	uint32_t reserved10[(0x448 - 0x270) >> 2];
};

struct al_pcie_core_aer_regs {
	/* 0x0 - PCI Express Extended Capability Header */
	uint32_t header;
	/* 0x4 - Uncorrectable Error Status Register */
	uint32_t uncorr_err_stat;
	/* 0x8 - Uncorrectable Error Mask Register */
	uint32_t uncorr_err_mask;
	/* 0xc - Uncorrectable Error Severity Register */
	uint32_t uncorr_err_severity;
	/* 0x10 - Correctable Error Status Register */
	uint32_t corr_err_stat;
	/* 0x14 - Correctable Error Mask Register */
	uint32_t corr_err_mask;
	/* 0x18 - Advanced Error Capabilities and Control Register */
	uint32_t cap_and_ctrl;
	/* 0x1c - Header Log Registers */
	uint32_t header_log[4];
	/* 0x2c - Root Error Command Register */
	uint32_t root_err_cmd;
	/* 0x30 - Root Error Status Register */
	uint32_t root_err_stat;
	/* 0x34 - Error Source Identification Register */
	uint32_t err_src_id;
};

struct al_pcie_core_reg_space_rev_1_2 {
	uint32_t			config_header[0x40 >> 2];
	uint32_t			pcie_pm_cap_base;
	uint32_t			reserved1[(0x70 - 0x44) >> 2];
	uint32_t			pcie_cap_base;
	uint32_t			pcie_dev_cap_base;
	uint32_t			pcie_dev_ctrl_status;
	uint32_t			pcie_link_cap_base;
	uint32_t			pcie_link_ctrl_status;
	uint32_t			reserved2[(0xB0 - 0x84) >> 2];
	uint32_t			msix_cap_base;
	uint32_t			reserved3[(0x100 - 0xB4) >> 2];
	struct al_pcie_core_aer_regs	aer;
	uint32_t			reserved4[(0x150 -
						   (0x100 +
						    sizeof(struct al_pcie_core_aer_regs))) >> 2];
	uint32_t			pcie_sec_ext_cap_base;
	uint32_t			reserved5[(0x700 - 0x154) >> 2];
	struct al_pcie_core_port_regs	port_regs;
	uint32_t			reserved6[(0x1000 -
						   (0x700 +
						    sizeof(struct al_pcie_core_port_regs))) >> 2];
};

struct al_pcie_core_reg_space_rev_3_4 {
	uint32_t			config_header[0x40 >> 2];
	uint32_t			pcie_pm_cap_base;
	uint32_t			reserved1[(0x70 - 0x44) >> 2];
	uint32_t			pcie_cap_base;
	uint32_t			pcie_dev_cap_base;
	uint32_t			pcie_dev_ctrl_status;
	uint32_t			pcie_link_cap_base;
	uint32_t			pcie_link_ctrl_status;
	uint32_t			reserved2[(0xB0 - 0x84) >> 2];
	uint32_t			msix_cap_base;
	uint32_t			reserved3[(0x100 - 0xB4) >> 2];
	struct al_pcie_core_aer_regs	aer;
	uint32_t			reserved4[(0x158 -
						   (0x100 +
						    sizeof(struct al_pcie_core_aer_regs))) >> 2];
	/* pcie_sec_cap is only applicable for function 0 */
	uint32_t			pcie_sec_ext_cap_base;
	uint32_t			reserved5[(0x178 - 0x15C) >> 2];
	/* tph capability is only applicable for rev3 */
	uint32_t			tph_cap_base;
	uint32_t			reserved6[3];
	uint32_t			tph_cap_base_x16;
	uint32_t			reserved7[(0x700 - 0x18C) >> 2];
	/* port_regs is only applicable for function 0 */
	struct al_pcie_core_port_regs	port_regs;
	uint32_t			reserved8[(0x1000 -
						   (0x700 +
						    sizeof(struct al_pcie_core_port_regs))) >> 2];
};

struct al_pcie_rev3_core_reg_space {
	struct al_pcie_core_reg_space_rev_3_4 func[REV3_MAX_NUM_OF_PFS];
};

struct al_pcie_rev4_core_reg_space {
	struct al_pcie_core_reg_space_rev_3_4 func[REV4_MAX_NUM_OF_PFS];
};

struct al_pcie_rev5_core_reg_space {
	struct al_pcie_core_reg_space_rev_3_4 func[REV4_MAX_NUM_OF_PFS];
};

struct al_pcie_core_reg_space {
	uint32_t			*config_header;
	uint32_t			*pcie_pm_cap_base;
	uint32_t			*pcie_cap_base;
	uint32_t			*pcie_dev_cap_base;
	uint32_t			*pcie_dev_ctrl_status;
	uint32_t			*pcie_link_cap_base;
	uint32_t			*pcie_link_ctrl_status;
	uint32_t			*msix_cap_base;
	struct al_pcie_core_aer_regs	*aer;
	uint32_t			*pcie_sec_ext_cap_base;
	uint32_t			*tph_cap_base;
};

struct al_pcie_revx_regs {
	struct al_pcie_revx_axi_regs __iomem	axi;
};

struct al_pcie_rev1_regs {
	struct al_pcie_rev1_axi_regs __iomem	axi;
	uint32_t reserved1[(AL_PCIE_REV_1_2_APP_REGS_OFFSET -
				(AL_PCIE_AXI_REGS_OFFSET +
				sizeof(struct al_pcie_rev1_axi_regs))) >> 2];
	struct al_pcie_rev1_w_regs __iomem	app;
	uint32_t reserved2[(AL_PCIE_REV_1_2_CORE_CONF_BASE_OFFSET -
				(AL_PCIE_REV_1_2_APP_REGS_OFFSET +
				sizeof(struct al_pcie_rev1_w_regs))) >> 2];
	struct al_pcie_core_reg_space_rev_1_2	core_space;
};

struct al_pcie_rev2_regs {
	struct al_pcie_rev2_axi_regs __iomem	axi;
	uint32_t reserved1[(AL_PCIE_REV_1_2_APP_REGS_OFFSET -
				(AL_PCIE_AXI_REGS_OFFSET +
				sizeof(struct al_pcie_rev2_axi_regs))) >> 2];
	struct al_pcie_rev2_w_regs __iomem	app;
	uint32_t reserved2[(AL_PCIE_REV_1_2_CORE_CONF_BASE_OFFSET -
				(AL_PCIE_REV_1_2_APP_REGS_OFFSET +
				sizeof(struct al_pcie_rev2_w_regs))) >> 2];
	struct al_pcie_core_reg_space_rev_1_2	core_space;
};

struct al_pcie_rev3_regs {
	struct al_pcie_rev3_axi_regs __iomem	axi;
	uint32_t reserved1[(AL_PCIE_REV_3_APP_REGS_OFFSET -
				(AL_PCIE_AXI_REGS_OFFSET +
				sizeof(struct al_pcie_rev3_axi_regs))) >> 2];
	struct al_pcie_rev3_w_regs __iomem	app;
	uint32_t reserved2[(AL_PCIE_REV_3_CORE_CONF_BASE_OFFSET -
				(AL_PCIE_REV_3_APP_REGS_OFFSET +
				sizeof(struct al_pcie_rev3_w_regs))) >> 2];
	struct al_pcie_rev3_core_reg_space	core_space;
};

struct al_pcie_rev4_regs {
	struct al_pcie_rev4_axi_regs __iomem	axi;
	uint32_t reserved1[(AL_PCIE_REV_4_APP_REGS_OFFSET -
				(AL_PCIE_AXI_REGS_OFFSET +
				sizeof(struct al_pcie_rev4_axi_regs))) >> 2];
	struct al_pcie_rev4_w_regs __iomem	app;
	uint32_t reserved2[(AL_PCIE_REV_4_CORE_CONF_BASE_OFFSET -
				(AL_PCIE_REV_4_APP_REGS_OFFSET +
				sizeof(struct al_pcie_rev4_w_regs))) >> 2];
	struct al_pcie_rev4_core_reg_space	core_space;
};

struct al_pcie_rev5_regs {
	struct al_pcie_rev5_axi_regs __iomem axi;
	uint32_t reserved1[(AL_PCIE_REV_5_APP_REGS_OFFSET -
				(AL_PCIE_AXI_REGS_OFFSET +
				sizeof(struct al_pcie_rev5_axi_regs))) >> 2];
	struct al_pcie_rev5_w_regs __iomem app;
	uint32_t reserved2[(AL_PCIE_REV_5_CORE_CONF_BASE_OFFSET -
				(AL_PCIE_REV_5_APP_REGS_OFFSET +
				sizeof(struct al_pcie_rev5_w_regs))) >> 2];
	struct al_pcie_rev5_core_reg_space	core_space;
};

struct al_pcie_reg_ptrs_axi_ctrl {
	uint32_t *global;
	uint32_t *master_rctl;
	uint32_t *master_arctl;
	uint32_t *master_awctl;
	uint32_t *master_ctl;
	uint32_t *slv_ctl;
	uint32_t *slv_wctl;
	uint32_t *tgtid_mask;
};

struct al_pcie_reg_ptrs_axi_ob_ctrl {
	uint32_t *cfg_target_bus;
	uint32_t *cfg_control;
	uint32_t *io_start_l;
	uint32_t *io_start_h;
	uint32_t *io_limit_l;
	uint32_t *io_limit_h;
	uint32_t *io_addr_mask_h; /* Rev3+ only */
	uint32_t *ar_msg_addr_mask_h; /* Rev3+ only */
	uint32_t *aw_msg_addr_mask_h; /* Rev3+ only */
	uint32_t *tgtid_reg_ovrd; /* Rev2+ only */
	uint32_t *addr_high_reg_ovrd_value; /* Rev2+ only */
	uint32_t *addr_high_reg_ovrd_sel; /* Rev2+ only */
	uint32_t *addr_size_replace; /* Rev2+ only */
};

struct al_pcie_reg_ptrs_axi_rd_parity {
	uint32_t *log_high;
	uint32_t *log_low;
};

struct al_pcie_reg_ptrs_axi_rd_cmpl {
	uint32_t *cmpl_log_high;
	uint32_t *cmpl_log_low;
};

struct al_pcie_reg_ptrs_axi_rd_to {
	uint32_t *to_log_high;
	uint32_t *to_log_low;
};

struct al_pcie_reg_ptrs_axi_wr_cmpl {
	uint32_t *wr_cmpl_log_high;
	uint32_t *wr_cmpl_log_low;
};

struct al_pcie_reg_ptrs_axi_wr_to {
	uint32_t *wr_to_log_high;
	uint32_t *wr_to_log_low;
};

struct al_pcie_reg_ptrs_axi_pcie_global {
	uint32_t *conf;
};

struct al_pcie_reg_ptrs_axi_conf {
	uint32_t *zero_lane[AL_MAX_NUM_OF_LANES];
};

struct al_pcie_reg_ptrs_axi_conf_high {
	uint32_t *zero_lane[AL_MAX_NUM_OF_LANES];
};

struct al_pcie_reg_ptrs_axi_status {
	uint32_t *lane[AL_MAX_NUM_OF_LANES];
};

struct al_pcie_reg_ptrs_axi_status_high {
	uint32_t *lane[AL_MAX_NUM_OF_LANES];
};

struct al_pcie_reg_ptrs_axi_parity {
	uint32_t *en_axi;
	uint32_t *status_axi;
};

struct al_pcie_reg_ptrs_axi_pos_logged {
	uint32_t *error_low;
	uint32_t *error_high;
};

struct al_pcie_reg_ptrs_axi_ordering {
	uint32_t *pos_cntl;
};

struct al_pcie_reg_ptrs_axi_link_down {
	uint32_t *reset_extend;
};

struct al_pcie_reg_ptrs_axi_pre_configuration {
	uint32_t *pcie_core_setup;
};

struct al_pcie_reg_ptrs_axi_init_fc {
	uint32_t *cfg;
};

struct al_pcie_reg_ptrs_axi_attr_ovrd {
	uint32_t *write_msg_ctrl_0;
	uint32_t *write_msg_ctrl_1;
	uint32_t *pf_sel;
	uint32_t *mapsel; /* Rev4+ only */
};

struct al_pcie_reg_ptrs_axi_pf_axi_attr_ovrd {
	uint32_t *func_ctrl_0;
	uint32_t *func_ctrl_1;
	uint32_t *func_ctrl_2;
	uint32_t *func_ctrl_3;
	uint32_t *func_ctrl_4;
	uint32_t *func_ctrl_5;
	uint32_t *func_ctrl_6;
	uint32_t *func_ctrl_7;
	uint32_t *func_ctrl_8;
	uint32_t *func_ctrl_9;
	uint32_t *func_ctrl_10; /* Rev4+ only */
};

struct al_pcie_reg_ptrs_axi_msg_attr_axuser_table {
	uint32_t *entry_vec;
	uint32_t *code_entry_vec_0; /* Rev4+ only */
	uint32_t *code_entry_vec_1; /* Rev4+ only */
};

struct al_pcie_reg_ptrs_axi_cfg_pclk_generate {
	uint32_t *cntr_low;
	uint32_t *cntr_high;
};
struct al_pcie_reg_ptrs_axi_cfg_pclk_generate_status {
	uint32_t *status_low;
};

struct al_pcie_reg_ptrs_axi_cfg_outbound_rd_len {
	uint32_t *len_shaper;
};

struct al_pcie_reg_ptrs_axi_mstr_pos {
	uint32_t *flush;
};

struct al_pcie_reg_ptrs_axi_mstr_attr_map {
	uint32_t *ctrl0;
	uint32_t *ctrl1;
};

struct al_pcie_reg_ptrs_axi_mstr_cfgtable {
	uint32_t *entry;
};

struct al_pcie_reg_ptrs_axi {
	struct al_pcie_reg_ptrs_axi_ctrl ctrl;
	struct al_pcie_reg_ptrs_axi_ob_ctrl ob_ctrl;
	struct al_pcie_reg_ptrs_axi_rd_parity rd_parity;
	struct al_pcie_reg_ptrs_axi_rd_cmpl rd_cmpl;
	struct al_pcie_reg_ptrs_axi_rd_to rd_to;
	struct al_pcie_reg_ptrs_axi_wr_cmpl wr_cmpl;
	struct al_pcie_reg_ptrs_axi_wr_to wr_to;
	struct al_pcie_reg_ptrs_axi_pcie_global pcie_global;
	struct al_pcie_reg_ptrs_axi_conf conf;
	struct al_pcie_reg_ptrs_axi_conf_high conf_high; /* Rev4+ only */
	struct al_pcie_reg_ptrs_axi_status status;
	struct al_pcie_reg_ptrs_axi_status_high status_high; /* Rev4+ only */
	struct al_pcie_reg_ptrs_axi_parity parity;
	struct al_pcie_reg_ptrs_axi_pos_logged pos_logged;
	struct al_pcie_reg_ptrs_axi_ordering ordering;
	struct al_pcie_reg_ptrs_axi_link_down link_down;
	struct al_pcie_reg_ptrs_axi_pre_configuration pre_configuration;
	struct al_pcie_reg_ptrs_axi_init_fc init_fc;
	struct al_pcie_revx_axi_int_grp_x_axi *int_grp_a;
	/* Rev3+ only */
	struct al_pcie_reg_ptrs_axi_attr_ovrd axi_attr_ovrd;
	struct al_pcie_reg_ptrs_axi_pf_axi_attr_ovrd pf_axi_attr_ovrd[REV3_MAX_NUM_OF_PFS];
	struct al_pcie_reg_ptrs_axi_msg_attr_axuser_table msg_attr_axuser_table;
	/* Rev4+ only */
	struct al_pcie_reg_ptrs_axi_cfg_pclk_generate cfg_pclk_generate;
	struct al_pcie_reg_ptrs_axi_cfg_pclk_generate_status cfg_pclk_generate_status;
	struct al_pcie_revx_axi_int_grp_x_axi *int_grp_b;
	struct al_pcie_reg_ptrs_axi_cfg_outbound_rd_len cfg_outbound_rd_len;
	struct al_pcie_reg_ptrs_axi_mstr_pos mstr_pos;
	struct al_pcie_reg_ptrs_axi_mstr_attr_map mstr_attr_map;
	struct al_pcie_reg_ptrs_axi_mstr_cfgtable mstr_cfgtable[32];
};

struct al_pcie_reg_ptrs_app_global_ctrl {
	uint32_t *port_init;
	uint32_t *pm_control;
	uint32_t *events_gen[REV3_MAX_NUM_OF_PFS];
	uint32_t *corr_err_sts_int;
	uint32_t *uncorr_err_sts_int;
	uint32_t *sris_kp_counter;
};

struct al_pcie_reg_ptrs_app_soc_int {
	uint32_t *mask_inta_leg_0;
	uint32_t *mask_inta_leg_1;
	uint32_t *mask_inta_leg_2;
	uint32_t *mask_inta_leg_3; /* Rev2+ only */
	uint32_t *mask_inta_leg_6; /* Rev4+ only */
	uint32_t *mask_msi_leg_0;
	uint32_t *mask_msi_leg_3; /* Rev2+ only */
	uint32_t *mask_msi_leg_6; /* Rev4+ only */
};
struct al_pcie_reg_ptrs_app_atu {
	uint32_t *in_mask_pair;
	uint32_t *out_mask_pair;
	uint32_t *reg_out_mask; /* Rev3+ only */
};

struct al_pcie_reg_ptrs_app_debug {
	uint32_t *info_0;
	uint32_t *info_1;
	uint32_t *info_2;
	uint32_t *info_3;
};
struct al_pcie_reg_ptrs_app_ap_user_send_msg {
	uint32_t *req_info;
	uint32_t *ack_info;
};
struct al_pcie_reg_ptrs_app_cntl_gen {
	uint32_t *features;
};
struct al_pcie_reg_ptrs_app_parity {
	uint32_t *en_core;
	uint32_t *status_core;
};
struct al_pcie_reg_ptrs_app_link_down {
	uint32_t *reset_delay;
	uint32_t *reset_extend_rsrvd;
};
struct al_pcie_reg_ptrs_app_status_per_func {
	uint32_t *status_per_func;
};
struct al_pcie_reg_ptrs_app_cfg_func_ext {
	uint32_t *cfg;
};
struct al_pcie_reg_ptrs_app_pm_state_per_func {
	uint32_t *pm_state_per_func;
};
struct al_pcie_reg_ptrs_app_cfg_blockalign {
	uint32_t *cfg_ext_cntl_1;
};

struct al_pcie_reg_ptrs_app {
	struct al_pcie_reg_ptrs_app_global_ctrl		global_ctrl;
	struct al_pcie_reg_ptrs_app_debug		debug;
	struct al_pcie_reg_ptrs_app_ap_user_send_msg	ap_user_send_msg;
	struct al_pcie_reg_ptrs_app_soc_int		soc_int[REV3_MAX_NUM_OF_PFS];
	struct al_pcie_reg_ptrs_app_cntl_gen		ctrl_gen;
	struct al_pcie_reg_ptrs_app_parity		parity;
	struct al_pcie_reg_ptrs_app_link_down		link_down;
	struct al_pcie_reg_ptrs_app_atu			atu;
	struct al_pcie_reg_ptrs_app_status_per_func	status_per_func[REV3_MAX_NUM_OF_PFS];
	struct al_pcie_revx_w_int_grp		*int_grp_a;
	struct al_pcie_revx_w_int_grp		*int_grp_b;
	struct al_pcie_revx_w_int_grp		*int_grp_c;
	struct al_pcie_revx_w_int_grp		*int_grp_d;
	/* Rev3+ only */
	struct al_pcie_reg_ptrs_app_cfg_func_ext	cfg_func_ext;
	struct al_pcie_reg_ptrs_app_pm_state_per_func pm_state_per_func[REV3_MAX_NUM_OF_PFS];
	/* Rev4+ only */
	struct al_pcie_reg_ptrs_app_cfg_blockalign	cfg_blockalign;
};

struct al_pcie_regs {
	struct al_pcie_reg_ptrs_axi	axi;
	struct al_pcie_reg_ptrs_app	app;
	struct al_pcie_core_port_regs	*port_regs;
	struct al_pcie_core_reg_space	core_space[REV3_MAX_NUM_OF_PFS];
};

/* Ack Frequency and L0-L1 ASPM Control Register */
#define PCIE_PORT_ACK_F_ASPM_CTRL_ENTER_ASPM		AL_BIT(30)

#endif
