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
 *  PCIe init params
 *  @{
 * @file   al_init_pcie_params.h
 *
 * @brief  PCIe initialization params
 */
#ifndef AL_INIT_PCIE_PARAMS_H__
#define AL_INIT_PCIE_PARAMS_H__

#include "al_hal_pcie.h"
#include "al_init_pcie.h"
#include "al_hal_serdes_interface.h"

#define MAX_NUM_OF_LANES	(16)

static struct al_pcie_gen3_lane_eq_params gen3_eq_params[MAX_NUM_OF_LANES] = {
	[0 ... (MAX_NUM_OF_LANES - 1)] = {
		.downstream_port_transmitter_preset = 5,
		.downstream_port_receiver_preset_hint = 0,
#if (AL_DEV_ID <= AL_DEV_ID_ALPINE_V2)
		.upstream_port_transmitter_preset = 9,
#else
		.upstream_port_transmitter_preset = 5,
#endif
		.upstream_port_receiver_preset_hint = 0,
		},
	};


static struct al_pcie_gen3_params gen3_params_set = {
	.perform_eq = AL_TRUE,
	.interrupt_enable_on_link_eq_request = AL_FALSE,
	.eq_params = gen3_eq_params,
	.eq_params_elements = MAX_NUM_OF_LANES,
	.eq_disable = AL_FALSE,
	.eq_phase2_3_disable = AL_FALSE,
	.eq_redo_bypass = AL_FALSE,
	.local_lf = 6,
	.local_fs = 27,
#if (AL_DEV_ID <= AL_DEV_ID_ALPINE_V2)
	.pset_req_vec_rc = AL_BIT(9),
#else
	.pset_req_vec_rc = AL_BIT(4),
#endif
#if (AL_DEV_ID <= AL_DEV_ID_ALPINE_V2)
	.pset_req_vec_ep = AL_BIT(9),
#else
	.pset_req_vec_ep = AL_BIT(4),
#endif
	.fb_mode = AL_PCIE_GEN3_FB_MODE_FOM,
};

static struct al_pcie_link_params pcie_link_params = {
	.max_speed = AL_PCIE_LINK_SPEED_GEN3,
};

static struct al_pcie_port_config_params pcie_port_conf_params = {
	.link_params = &pcie_link_params,
	.enable_axi_snoop = AL_TRUE,
	.enable_ram_parity_int = AL_FALSE,
	.enable_axi_parity_int = AL_FALSE,
	.lat_rply_timers = NULL,
	.gen2_params = NULL,
	.gen3_params = &gen3_params_set,
	.fast_link_mode = AL_FALSE,
	.enable_axi_slave_err_resp = AL_TRUE,
	.relaxed_ordering_params = NULL,
};

static struct al_pcie_pf_config_params pcie_pf_conf_params[AL_MAX_NUM_OF_PFS] = {
	/* PF0 */
	{
		.cap_d1_d3hot_dis = AL_TRUE,
		.cap_flr_dis = AL_TRUE,
		.cap_aspm_dis = AL_TRUE,
		.bar_params_valid = AL_TRUE,
		.bar_params = {
			{
				.enable = AL_TRUE,
				.memory_64_bit = AL_TRUE,
				.memory_is_prefetchable = AL_FALSE,
				.memory_space = AL_TRUE,
				.size = 0x8000000000000000ULL
			}, {
				.enable = AL_FALSE
			}, {
				.enable = AL_TRUE,
				.memory_64_bit = AL_TRUE,
				.memory_is_prefetchable = AL_FALSE,
				.memory_space = AL_TRUE,
				.size = 0x8000000000000000ULL
			}, {
				.enable = AL_FALSE
			}, {
			.enable = AL_FALSE
			}, {
			.enable = AL_FALSE
			}
		}
	},
	/* PF1 */
	{
		.cap_d1_d3hot_dis = AL_TRUE,
		.cap_flr_dis = AL_TRUE,
		.cap_aspm_dis = AL_TRUE,
		.bar_params_valid = AL_TRUE,
		.bar_params = {
			{
				.enable = AL_TRUE,
				.memory_64_bit = AL_TRUE,
				.memory_is_prefetchable = AL_FALSE,
				.memory_space = AL_TRUE,
				.size = 0x8000000000000000ULL
			}, {
				.enable = AL_FALSE
			}, {
				.enable = AL_TRUE,
				.memory_64_bit = AL_TRUE,
				.memory_is_prefetchable = AL_FALSE,
				.memory_space = AL_TRUE,
				.size = 0x8000000000000000ULL
			}, {
				.enable = AL_FALSE
			}, {
			.enable = AL_FALSE
			}, {
			.enable = AL_FALSE
			}
		}
	},
	/* PF2 */
	{
		.cap_d1_d3hot_dis = AL_TRUE,
		.cap_flr_dis = AL_TRUE,
		.cap_aspm_dis = AL_TRUE,
		.bar_params_valid = AL_TRUE,
		.bar_params = {
			{
				.enable = AL_TRUE,
				.memory_64_bit = AL_TRUE,
				.memory_is_prefetchable = AL_FALSE,
				.memory_space = AL_TRUE,
				.size = 0x8000000000000000ULL
			}, {
				.enable = AL_FALSE
			}, {
				.enable = AL_TRUE,
				.memory_64_bit = AL_TRUE,
				.memory_is_prefetchable = AL_FALSE,
				.memory_space = AL_TRUE,
				.size = 0x8000000000000000ULL
			}, {
				.enable = AL_FALSE
			}, {
			.enable = AL_FALSE
			}, {
			.enable = AL_FALSE
			}
		}
	},
	/* PF3 */
	{
		.cap_d1_d3hot_dis = AL_TRUE,
		.cap_flr_dis = AL_TRUE,
		.cap_aspm_dis = AL_TRUE,
		.bar_params_valid = AL_TRUE,
		.bar_params = {
			{
				.enable = AL_TRUE,
				.memory_64_bit = AL_TRUE,
				.memory_is_prefetchable = AL_FALSE,
				.memory_space = AL_TRUE,
				.size = 0x8000000000000000ULL
			}, {
				.enable = AL_FALSE
			}, {
				.enable = AL_TRUE,
				.memory_64_bit = AL_TRUE,
				.memory_is_prefetchable = AL_FALSE,
				.memory_space = AL_TRUE,
				.size = 0x8000000000000000ULL
			}, {
				.enable = AL_FALSE
			}, {
			.enable = AL_FALSE
			}, {
			.enable = AL_FALSE
			}
		}
	},
};

struct al_init_pcie_params default_init_pcie_params = {
	.port_id = 0,
	.pcie_reg_base = NULL,
	.pbs_reg_base = NULL,
	.mode = AL_PCIE_OPERATING_MODE_EP,
	.max_lanes = 8,
	.max_num_of_pfs = 1,
	.reads_config = NULL,
	.port_params = &pcie_port_conf_params,
	.pf_params = {
		&(pcie_pf_conf_params[0]),
		&(pcie_pf_conf_params[1]),
		&(pcie_pf_conf_params[2]),
		&(pcie_pf_conf_params[3]),
	},
	.ex_params = NULL,
	.start_link = AL_TRUE,
	.wait_for_link_timeout_ms = 1,
	.wait_for_link_silent_fail = AL_FALSE,
	.crrs_en = AL_FALSE,
	.reinit_link = AL_FALSE,
	.skip_teardown = AL_FALSE,
	.serdes_grp_objs = NULL,
	.serdes_grp_objs_num = 0,
	.skip_serdes_ical = AL_FALSE,
	.retrain_if_not_full_width = AL_FALSE,
	.iof_regs_access_en = AL_FALSE
};

#endif /* AL_INIT_PCIE_PARAMS_H__ */
