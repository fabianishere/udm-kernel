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
 * @defgroup group_pmdt_cctm_api API
 * Performance monitoring debug and trace- Central Cross Triggering Matrix(CCTM)
 * @ingroup group_pmdt_cctm
 * @{
 * The CCTM is a single central unit in the PMDT. All PMU complexes are
 * connected to CCTM such that event triggers, interrupts and gpio triggers
 * that are fired by PMU device are routed into CCTM. The CCTM can route it
 * externally via single GIC interrupt and/or single GPIO as well as using it
 * internally to pause other PMUs according to a predefined enablement matrix.
 * The CCTM has the ability to enable, disable, clear, pause (sw enforced) all
 * PMUs in the system. In addition, it has a timestamp generation engine that
 * is propagated to all PMU devices enabling measurements synchronization.
 *
 * @file   al_hal_pmdt_cctm.h
 */

#ifndef __AL_HAL_PMDT_CCTM_H__
#define __AL_HAL_PMDT_CCTM_H__

#include "al_hal_common.h"

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

/** CCTM matrix line size (in words, 4x32 = 128 PMU devices) */
#define AL_PMDT_CCTM_MATRIX_LINE_SIZE 4

/** CCTM pause delay configuration */
struct al_cctm_pause_config {
	unsigned int pause_start_delay; /** start delay of x cycles to all PMDTs*/
	unsigned int reset_delay_exp; /** delay of 2^x cycles after reset x=[0-15]*/
	al_bool trig_delay_en; /** enable delay for pause caused by event triggers*/
	al_bool sw_delay_en; /** enable delay for pause caused by sw pause */
};

/** CCTM PMU configuration info */
struct al_cctm_config {
	unsigned int imp_pmu; /** number of implemented PMUs in the system */
	unsigned int active_pmu; /** number of active tracked PMUs */
};

/** CCTM context */
struct al_pmdt_cctm {
	void __iomem *regs;
};

/**
 * Initialize the CCTM handle
 *
 * @param pmdt_cctm
 *		An allocated, non-initialized instance
 * @param pmdt_cctm_regs_base
 *		The base address of the CCTM registers
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_pmdt_cctm_handle_init(
		struct al_pmdt_cctm *pmdt_cctm,
		void __iomem *pmdt_cctm_regs_base);

/**
 * Retrieve PMDT revision
 * (stored in CCTM as it is a single central PMDT unit)
 *
 * @param pmdt_cctm
 *		CCTM handle
 *
 * @return PMDT revision
 */
uint32_t al_pmdt_cctm_rev_get(struct al_pmdt_cctm *pmdt_cctm);

/**
 * Retrieve CCTM PMU configuration info
 *
 * @param pmdt_cctm
 *		CCTM handle
 * @param info
 *		PMU configuration info
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_pmdt_cctm_pmu_conf_get(
		struct al_pmdt_cctm *pmdt_cctm,
		struct al_cctm_config *info);

/**
 * Retrieve CCTM iofic register base address
 *
 * All PMU units IC are routed into CCTM IC, hence irq n represents PMU n
 *
 * @param pmdt_cctm
 *		CCTM handle
 *
 * @return	iofic base address
 */
void __iomem *al_pmdt_cctm_iofic_base_get(struct al_pmdt_cctm *pmdt_cctm);

/**
 * Configure CCTM pause delays
 *
 * @param pmdt_cctm
 *		CCTM handle
 * @param pause_config
 *		pause configuration parameters
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_pmdt_cctm_pause_config(
		struct al_pmdt_cctm *pmdt_cctm,
		const struct al_cctm_pause_config *pause_config);

/**
 * Set central timestamp generator ratio (to all PMU units)
 *
 * @param pmdt_cctm
 *		CCTM handle
 * @param ratio
 *		timestamp window ratio vs. local CCTM clock
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_pmdt_cctm_timestamp_ratio_set(
		struct al_pmdt_cctm *pmdt_cctm,
		uint32_t ratio);

/**
 * Set timestamp initial value
 *
 * Note. every PMU device can set its timestamp counter with a unique
 * initial value as part of PMU configuration.
 * This function clear all time stamp counters and set them to their configured
 * initial value.
 * In addition an initial value to the local CCTM time stamp
 * counter should be provided.
 * From now on, every call to al_pmdt_cctm_timestamp_clear will set all
 * timestamp counters to their initial value.
 *
 * @param pmdt_cctm
 *		CCTM handle
 * @param cctm_ts_init_val
 *		CCTM timestamp counter initial value
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_pmdt_cctm_initial_timestamp_set(
		struct al_pmdt_cctm *pmdt_cctm,
		uint64_t cctm_ts_init_val);

/**
 * Enable/disable timestamp generation
 *
 * @param pmdt_cctm
 *		CCTM handle
 * @param is_enable
 *		0/1 - disable/enable
 */
void al_pmdt_cctm_timestamp_enable(
		struct al_pmdt_cctm *pmdt_cctm,
		al_bool is_enable);

/**
 * Pause/resume timestamp generation to all PMU units
 *
 * @param pmdt_cctm
 *		CCTM handle
 * @param is_pause
 *		0/1 - resume/pause
 */
void al_pmdt_cctm_timestamp_pause(
		struct al_pmdt_cctm *pmdt_cctm,
		al_bool is_pause);

/**
 * Clear timestamp in all PMU units
 *
 * @param pmdt_cctm
 *		CCTM handle
 */
void al_pmdt_cctm_timestamp_clear(struct al_pmdt_cctm *pmdt_cctm);

/**
 * Retrieve current CCTM time stamp
 *
 * - this time stamp should be reflected at all PMU units +/-1 cycle
 * difference caused by signal skew
 *
 * @param pmdt_cctm
 *		ELA-CONF handle
 *
 * @return current time stamp
 */
uint64_t al_pmdt_cctm_timestamp_get(struct al_pmdt_cctm *pmdt_cctm);

/**
 * Enable/disable underlying PMU devices
 *
 * @param pmdt_cctm
 *		CCTM handle
 * @param is_enable
 *		0/1 - disable/enable
 * @param pmu_mask
 *		bit mask of underlying PMU devices (1 - enable/disable, 0 - untouched).
 *		The current enablement state of all PMUs is returned via this parameter.
 */
void al_pmdt_cctm_pmu_enable(
		struct al_pmdt_cctm *pmdt_cctm,
		al_bool is_enable,
		uint32_t (*pmu_mask)[AL_PMDT_CCTM_MATRIX_LINE_SIZE]);

/**
 * Pause/resume underlying PMU devices
 *
 * @param pmdt_cctm
 *		CCTM handle
 * @param is_pause
 *		0/1 - resume/pause
 * @param pmu_mask
 *		bit mask of underlying PMU devices (1 - pause/resume, 0 - untouched).
 *		The current state of all *SW paused* PMUs is returned via this parameter
 */
void al_pmdt_cctm_pmu_pause(
		struct al_pmdt_cctm *pmdt_cctm,
		al_bool is_pause,
		uint32_t (*pmu_mask)[AL_PMDT_CCTM_MATRIX_LINE_SIZE]);

/**
 * Clear underlying PMU devices
 *
 * @param pmdt_cctm
 *		CCTM handle
 * @param pmu_mask
 *		bit mask of underlying PMU devices to be cleared
 */
void al_pmdt_cctm_pmu_clear(
		struct al_pmdt_cctm *pmdt_cctm,
		uint32_t (*pmu_mask)[AL_PMDT_CCTM_MATRIX_LINE_SIZE]);

/**
 * Enable/disable underlying PMU GPIO trigger
 *
 * @param pmdt_cctm
 *		CCTM handle
 * @param is_enable
 *		0/1 - disable/enable
 * @param pmu_mask
 *		bit mask of underlying PMU devices (1 - enable/disable, 0 - untouched).
 *		The current gpio enablement of all PMUs is returned via this parameter.
 */
void al_pmdt_cctm_gpio_enable(
		struct al_pmdt_cctm *pmdt_cctm,
		al_bool is_enable,
		uint32_t (*pmu_mask)[AL_PMDT_CCTM_MATRIX_LINE_SIZE]);

/**
 * Force/clear SW trigger to emulate incoming trigger from specific PMU
 * Note. trigger will pause other PMU only if it enabled via cross_pause matrix
 *
 * @param pmdt_cctm
 *		CCTM handle
 * @param is_force
 *		0/1 - clear/force
 * @param pmu_mask
 *		bit mask of underlying PMU devices (1 - force/clear, 0 - untouched).
 *		The current *SW forced* triggers of all PMUs is returned via this param.
 */
void al_pmdt_cctm_trig_force(
		struct al_pmdt_cctm *pmdt_cctm,
		al_bool is_force,
		uint32_t (*pmu_mask)[AL_PMDT_CCTM_MATRIX_LINE_SIZE]);

/**
 * Get active PMU triggers snapshot
 *
 * @param pmdt_cctm
 *		CCTM handle
 * @param pmu_mask
 *		bit mask of underlying PMU devices
 */
void al_pmdt_cctm_trig_get(
		struct al_pmdt_cctm *pmdt_cctm,
		uint32_t (*pmu_mask)[AL_PMDT_CCTM_MATRIX_LINE_SIZE]);

/**
 * Get active PMU GPIO trigger snapshot
 *
 * @param pmdt_cctm
 *		CCTM handle
 * @param pmu_mask
 *		bit mask of underlying PMU devices
 */
void al_pmdt_cctm_gpio_get(
		struct al_pmdt_cctm *pmdt_cctm,
		uint32_t (*pmu_mask)[AL_PMDT_CCTM_MATRIX_LINE_SIZE]);

/**
 * Get paused PMU snapshot
 *
 * @param pmdt_cctm
 *		CCTM handle
 * @param pmu_mask
 *		bit mask of underlying PMU devices
 */
void al_pmdt_cctm_paused_get(
		struct al_pmdt_cctm *pmdt_cctm,
		uint32_t (*pmu_mask)[AL_PMDT_CCTM_MATRIX_LINE_SIZE]);

/**
 * Enable/disable dst-PMU to be paused by src-PMUs by firing an event trigger
 *
 * @param pmdt_cctm
 *		CCTM handle
 * @param is_enable
 *		0/1 - disable/enable
 * @param dst_pmu
 *		the destination PMU to be paused
 * @param src_pmu_mask
 *		bit mask of underlying PMU devices that can trigger pause to dst-PMU
 *		(1 - enable/disable, 0 - untouched)
 *		The current pause enablement mask of all PMUs is returned via this param
 */
void al_pmdt_cctm_cross_pause_enable(
		struct al_pmdt_cctm *pmdt_cctm,
		al_bool is_enable,
		uint32_t dst_pmu,
		uint32_t (*src_pmu_mask)[AL_PMDT_CCTM_MATRIX_LINE_SIZE]);

/* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */

#endif /* __AL_HAL_PMDT_CCTM_H__ */
/** @} end of group_pmdt_cctm */

