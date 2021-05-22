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
 *  PCIe debug
 *  @{
 * @file   al_init_pcie_debug.c
 *
 * @brief  PCIe debug functions
 */

#include "al_hal_pcie.h"
#include "al_init_pcie_debug.h"
#ifdef AL_HAL_EX
#include "../../proprietary/services/pcie/al_init_pcie_debug_ex.h"
#endif
#include "al_hal_reg_utils.h"

__attribute__((weak)) void al_init_pcie_print_params_ex(
	struct al_init_pcie_params *init_pcie_params __attribute__ ((unused)))
{
}

void al_init_pcie_print_reads_config(struct al_pcie_ib_hcrd_os_ob_reads_config *reads_config)
{
	if (!reads_config) {
		al_dbg("	Reads Config: N/A\n");
		return;
	}

	al_dbg("	Reads Config:\n");
	al_dbg("		NOF OS OB RD: %d\n", reads_config->nof_outstanding_ob_reads);
	al_dbg("		NOF CPL HDR: %d\n", reads_config->nof_cpl_hdr);
	al_dbg("		NOF NP HDR: %d\n", reads_config->nof_np_hdr);
	al_dbg("		NOF P HDR: %d\n", reads_config->nof_p_hdr);
}

void al_init_pcie_print_link_params(struct al_pcie_link_params *link_params)
{
	if (!link_params) {
		al_dbg("		Link Parameters: N/A\n");
		return;
	}

	al_dbg("		Link Parameters:\n");
	al_dbg("			Max Speed: %d\n", link_params->max_speed);
	al_dbg("			Max Payload size: %d\n", link_params->max_payload_size);
}

void al_init_pcie_print_latency_params(struct al_pcie_latency_replay_timers *lat_rply_timers)
{
	if (!lat_rply_timers) {
		al_dbg("		Link Parameters: N/A\n");
		return;
	}

	al_dbg("		Latency replay timers:\n");
	al_dbg("			RT latency limit: %d\n",
						lat_rply_timers->round_trip_lat_limit);
	al_dbg("			Reply timer limit: %d\n",
						lat_rply_timers->replay_timer_limit);
}

void al_init_pcie_print_gen2_params(struct al_pcie_gen2_params *gen2_params)
{
	if (!gen2_params) {
		al_dbg("		Gen2 Parameters: N/A\n");
		return;
	}
	al_dbg("		Gen2 Parameters:\n");
	al_dbg("			TX swing low: %d\n",
						gen2_params->tx_swing_low);
	al_dbg("			TX compliance receive: %d\n",
						gen2_params->tx_compliance_receive_enable);
	al_dbg("			Deemphasis: %d\n",
						gen2_params->set_deemphasis);
}

void al_init_pcie_print_gen3_lane_eq_params(
	struct al_pcie_gen3_lane_eq_params *eq_params,
	int eq_params_elements)
{
	int i;

	if (!eq_params) {
		al_dbg("			EQ: N/A\n");
		return;
	}

	for (i = 0; i < eq_params_elements; i++) {
		al_dbg("			EQ[%d]: [%d][%d][%d][%d]\n", i,
						eq_params[i].downstream_port_transmitter_preset,
						eq_params[i].downstream_port_receiver_preset_hint,
						eq_params[i].upstream_port_transmitter_preset,
						eq_params[i].upstream_port_receiver_preset_hint);
	}
}

void al_init_pcie_print_gen3_params(struct al_pcie_gen3_params *gen3_params)
{
	if (!gen3_params) {
		al_dbg("		Gen3 Parameters: N/A\n");
		return;
	}
	al_dbg("		Gen3 Parameters:\n");
	al_dbg("			Perform EQ: %d\n", gen3_params->perform_eq);
	al_dbg("			INT on link EQ: %d\n",
						gen3_params->interrupt_enable_on_link_eq_request);
	al_init_pcie_print_gen3_lane_eq_params(gen3_params->eq_params,
						gen3_params->eq_params_elements);
	al_dbg("			EQ disable: %d\n", gen3_params->eq_disable);
	al_dbg("			EQ phases 2/3 disable: %d\n",
						gen3_params->eq_phase2_3_disable);
	al_dbg("			EQ redo bypass: %d\n", gen3_params->eq_redo_bypass);
	al_dbg("			Local full swing: %d\n", gen3_params->local_lf);
	al_dbg("			Local low frequency: %d\n", gen3_params->local_fs);
}

void al_init_pcie_print_relaxed_ordering_params(
	struct al_pcie_relaxed_ordering_params *relaxed_ordering_params)
{
	if (!relaxed_ordering_params) {
		al_dbg("		Relaxed-Ordering Parameters: N/A\n");
		return;
	}
	al_dbg("		Relaxed-Ordering Parameters:\n");
	al_dbg("			TX: %d\n",
					relaxed_ordering_params->enable_tx_relaxed_ordering);
	al_dbg("			RX: %d\n",
					relaxed_ordering_params->enable_tx_relaxed_ordering);
}

void al_init_pcie_print_port_params(struct al_pcie_port_config_params *port_params)
{
	if (!port_params) {
		al_dbg("	Port parameters: N/A\n");
		return;
	}

	al_dbg("	Port parameters:\n");
	al_init_pcie_print_link_params(port_params->link_params);
	al_dbg("		AXI snoop: %d\n", port_params->enable_axi_snoop);
	al_dbg("		RAM parity: %d\n", port_params->enable_ram_parity_int);
	al_dbg("		AXI parity: %d\n", port_params->enable_axi_parity_int);
	al_init_pcie_print_latency_params(port_params->lat_rply_timers);
	al_init_pcie_print_gen2_params(port_params->gen2_params);
	al_init_pcie_print_gen3_params(port_params->gen3_params);
	al_dbg("		Fast link mode: %d\n", port_params->fast_link_mode);
	al_dbg("		AXI slave err response: %d\n",
					port_params->enable_axi_slave_err_resp);
	al_init_pcie_print_relaxed_ordering_params(port_params->relaxed_ordering_params);
}

void al_init_pcie_print_bar_params(struct al_pcie_ep_bar_params *bar_params, int bar_num)
{
	uint32_t size_high;
	uint32_t size_low;

	if (!bar_params) {
		al_dbg("		BAR[%d]: N/A\n", bar_num);
		return;
	}

	size_low = (bar_params->size & AL_FIELD_MASK(31, 0));
	size_high = ((bar_params->size >> 32) & AL_FIELD_MASK(31, 0));

	al_dbg("		BAR[%d]: En[%d] Mem[%d] 64[%d] Pref[%d] SZ[%x][%x]\n",
					bar_num,
					bar_params->enable,
					bar_params->memory_space,
					bar_params->memory_64_bit,
					bar_params->memory_is_prefetchable,
					size_high, size_low);
}

void al_init_pcie_print_pf_params(struct al_pcie_pf_config_params *pf_params, int pf_num)
{
	int i;

	if (!pf_params) {
		al_dbg("	PF[%d]: N/A\n", pf_num);
		return;
	}

	al_dbg("	PF[%d]:\n", pf_num);
	al_dbg("		D1/D3 hot disable: %d\n", pf_params->cap_d1_d3hot_dis);
	al_dbg("		FLR disable: %d\n", pf_params->cap_flr_dis);
	al_dbg("		ASPM disable: %d\n", pf_params->cap_aspm_dis);
	al_dbg("		Bar params valid: %d\n", pf_params->bar_params_valid);
	for (i = 0; i < 6; i++)
		al_init_pcie_print_bar_params(&pf_params->bar_params[i], i);
	al_init_pcie_print_bar_params(&pf_params->exp_bar_params, 7);

}

void al_init_pcie_print_params(struct al_init_pcie_params *init_pcie_params)
{
	unsigned int i;

	al_dbg("========== PCIe initialization parameters ==========\n");
	al_dbg("	Port ID: %d\n", init_pcie_params->port_id);
	al_dbg("	PCIe base reg: %p\n", init_pcie_params->pcie_reg_base);
	al_dbg("	Port pbs reg: %p\n", init_pcie_params->pbs_reg_base);
	al_dbg("	Operating Mode: %d\n", init_pcie_params->mode);
	al_dbg("	Max Lanes: %d\n", init_pcie_params->max_lanes);
	al_dbg("	Max PFs: %d\n", init_pcie_params->max_num_of_pfs);
	al_init_pcie_print_reads_config(init_pcie_params->reads_config);
	al_init_pcie_print_port_params(init_pcie_params->port_params);
	for (i = 0; i < init_pcie_params->max_num_of_pfs; i++) {
		al_init_pcie_print_pf_params(init_pcie_params->pf_params[i], i);
	}
	al_dbg("	Start link: %d\n", init_pcie_params->start_link);
	al_dbg("	Link timeout (ms): %d\n", init_pcie_params->wait_for_link_timeout_ms);
	al_dbg("	Wait for link silent fail: %s\n",
			init_pcie_params->wait_for_link_silent_fail ? "enabled" : "disabled");
	al_dbg("	CRRS: %s\n", init_pcie_params->crrs_en ? "enabled" : "disabled");
	al_dbg("	Skip teardown: %s\n",
			init_pcie_params->skip_teardown ? "enabled" : "disabled");
	al_dbg("	Retrain if not full width: %s\n",
			init_pcie_params->retrain_if_not_full_width ? "enabled" : "disabled");
	al_dbg("	Sys Fabric regs base: %p\n", init_pcie_params->nb_regs_base);
	al_dbg("	IO Fabric regs access enable: %s\n",
			init_pcie_params->iof_regs_access_en ? "enabled" : "disabled");
	al_init_pcie_print_params_ex(init_pcie_params);
	al_dbg("====================================================\n");
}
