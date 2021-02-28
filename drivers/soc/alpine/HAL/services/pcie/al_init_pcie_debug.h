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

*******************************************************************************//**
 *  PCIe init debug
 *  @{
 * @file   al_init_pcie_debug.h
 *
 * @brief  PCIe debug functions
 */

#ifndef AL_INIT_PCIE_DEBUG_H__
#define AL_INIT_PCIE_DEBUG_H__

#include "al_init_pcie.h"

/* This set of functions can be used to print each and every one of the PCIe config parameters */

void al_init_pcie_print_reads_config(struct al_pcie_ib_hcrd_os_ob_reads_config *reads_config);
void al_init_pcie_print_link_params(struct al_pcie_link_params *link_params);
void al_init_pcie_print_latency_params(struct al_pcie_latency_replay_timers *lat_rply_timers);
void al_init_pcie_print_gen2_params(struct al_pcie_gen2_params *gen2_params);
void al_init_pcie_print_gen3_lane_eq_params(
	struct al_pcie_gen3_lane_eq_params *eq_params,
	int eq_params_elements);
void al_init_pcie_print_gen3_params(struct al_pcie_gen3_params *gen3_params);
void al_init_pcie_print_relaxed_ordering_params(
	struct al_pcie_relaxed_ordering_params *relaxed_ordering_params);
void al_init_pcie_print_port_params(struct al_pcie_port_config_params *port_params);
void al_init_pcie_print_bar_params(struct al_pcie_ep_bar_params *bar_params, int bar_num);
void al_init_pcie_print_pf_params(struct al_pcie_pf_config_params *pf_params, int pf_num);

/**
 * print all pcie init params
 * @param init_pcie_params		pcie init Port and PFs params
 */
void al_init_pcie_print_params(struct al_init_pcie_params *init_pcie_params);

/**
 * print all pcie init params
 * @param init_pcie_params		pcie init Port and PFs params
 */
void al_init_pcie_print_params_ex(struct al_init_pcie_params *init_pcie_params);

#endif /* AL_INIT_PCIE_DEBUG_H__ */
