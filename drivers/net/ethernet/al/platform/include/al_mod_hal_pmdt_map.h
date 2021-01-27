/*******************************************************************************
Copyright (C) 2016 Annapurna Labs Ltd.

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
 * This file contains PMDT device mapping definitions
 */

#ifndef __AL_HAL_PMDT_MAP_H__
#define __AL_HAL_PMDT_MAP_H__

#include "al_mod_hal_reg_utils.h"
#include "al_mod_hal_bootstrap.h"
#include "al_mod_pmdt_config.h"
#include "al_mod_hal_pmdt_map_regs.h"

struct al_mod_pmdt_map_entry {
	enum al_mod_pmdt_unit id;
	const char *name; /* unit name - up to AL_PMDT_UNIT_NAME_LEN chars */
	void __iomem *regs_base; /* unit regiter base address */
	void __iomem *bus_base; /* units bus base address (e.g. NB_BASE/SB_BASE */
	unsigned int core_freq; /* unit clock frequency */
	unsigned int cctm_grp; /* unit's cctm group */
	unsigned int cctm_pmu_bit; /* unit's cctm pmu bit (in the above group) */
	unsigned int cctm_ela_bit; /* unit's cctm ela bit (in the above group) */
	struct al_mod_pmdt_config pmdtu_config; /* default PMDT unit configuration parameters */
};

struct al_mod_pmdt_components_info {
	unsigned int axi_mon_num;
	unsigned int ela_num;
	unsigned int ext_trigger_num;
	unsigned int pmu_counters;
};

/**
 * Retrieve map entry for specific pmdt unit
 *
 * @param region
 *		pmdt unit region on multi-regions systems, ignored in single regions systems
 * @param id
 *		pmdt unit ID
 * @param bs
 *		device bootstrap info
 * @param entry
 *		map entry (out)
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_mod_pmdt_map_entry_get(
		unsigned int region,
		enum al_mod_pmdt_unit id,
		struct al_mod_bootstrap *bs,
		struct al_mod_pmdt_map_entry *entry);

/**
 * Retrieve map default configuration for specific pmdt unit
 *
 * @param id
 *		pmdt unit ID
 * @param config
 *		unit default configuration (out)
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_mod_pmdt_map_unit_default_config_get(
		enum al_mod_pmdt_unit id,
		struct al_mod_pmdt_config *config);

/**
 * Retrieve internal trigger number to be used
 * with al_mod_pmdt_pmu_event_config.trig_num
 *
 * @param axi_mon_num
 *		number of axi_mon in this pmdt
 * @param axi_mon_instance
 *		axi monitor instance from which the trigger is fired
 * @param type
 *		trigger type
 * @param index
 *		trigger index (per type)
 *
 * @return	trigger number if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_mod_pmdt_map_trig_num_get(
		unsigned int axi_mon_num,
		unsigned int axi_mon_instance,
		enum al_mod_pmdt_pmu_int_trig_type type,
		unsigned int index);

/**
 * Retrieve axi_mon trace signal group in the ela.
 * To be used with al_mod_pmdt_ela_trig_config.signal_group
 *
 * @param axi_mon_num
 *		number of axi_mon in this pmdt
 * @param axi_mon_instance
 *		axi monitor instance from which the signal is fired
 * @return	signal_group if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_mod_pmdt_map_trace_sig_get(
		unsigned int axi_mon_num,
		unsigned int axi_mon_instance);

/**
 * Retrieve pmu group B interrupt bit number
 *
 * @param axi_mon_num
 *		number of axi_mon in this pmdt
 * @param axi_mon_instance
 *		axi monitor instance from which the trigger is fired
 * @param type
 *		irq type
 *
 * @return	trigger number if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_mod_pmdt_map_pmu_irq_num_get(
		unsigned int axi_mon_num,
		unsigned int axi_mon_instance,
		enum al_mod_pmdt_axi_mon_irq_type type);

/**
 * Get cpu num for cluster units
 *
 * @param	cmplx
 *		pmdt complex handle
 *
 * @return	cpu number if the unit is cluster, else -1
 */
int al_mod_pmdt_map_cpu_clust_num_get(
		struct al_mod_pmdt_complex_handle *cmplx);

/**
 * get the components info for given unit.
 *
 * @param	id
 *		pmdt unit id
 * @param	info
 *		pmdt components info
 *
 * @return 0 if finished successfully else <0
 */
int al_mod_pmdt_unit_components_info_get(
		enum al_mod_pmdt_unit id,
		struct al_mod_pmdt_components_info *info);

#endif /*__AL_HAL_PMDT_MAP_H__*/
