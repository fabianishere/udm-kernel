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
 * @defgroup group_pmdt_pmu_api API
 * Performance Monitoring Debug and Trace - PMU, Performance Monitoring Unit
 * @ingroup group_pmdt_pmu
 * @{
 * The PMU is part of a PMU complex instance, embedded in a specific HW core.
 * The PMU is used to count events such as AXI Monitor events. It contains
 * up to 32 counters that can accumulate the events in a predefined window
 * slice or infinitely. Each counter can be programmed with underflow and
 * overflow thresholds that are sampled at window boundary and can produce a
 * violation trigger. This trigger can be used to fire an interrupt,
 * changing a gpio state (via CCTM) or producing a cross-trigger for pausing a
 * different PMU complex in the system.
 * The counter accumulation can also be paused by other PMU cross-trigger for
 * in-depth debugging. All PMU counters can be logged periodically through the
 * ELA trace.
 *
 * @file   al_hal_pmdt_pmu.h
 */

#ifndef __AL_HAL_PMDT_PMU_H__
#define __AL_HAL_PMDT_PMU_H__

#include "al_hal_common.h"

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

/** Number of event counters per PMU */
#define AL_PMDT_PMU_MAX_EVENT_NUM 32

/** Number of available triggers (external and internal) per PMU */
#define AL_PMDT_PMU_MAX_TRIGER_NUM 128

/** PMU configuration parameters */
struct al_pmdt_pmu_config {
	int window_unit_exp; /** window time unit e.g. for 1usec set to -6 */
	al_bool start_on_window; /** start counters on window boundary */
	al_bool pulse_trig; /** trigger signal type. use 0 - level trigger */
	al_bool gpio_en; /** enable gpio routing */
	al_bool error_trig_en; /** enable outgoing trigger signal in case of error */
	al_bool error_gpio_en; /** enable outgoing gpio routing in case of error */
	al_bool unf_trig_en; /** enable counter underflow trigger */
	al_bool ovf_trig_en; /** enable counter overflow trigger */
	al_bool unf_int_en; /** enable counter underflow as violation interrupt */
	al_bool ovf_int_en; /** enable counter overflow as violation interrupt */
	unsigned int reset_delay_exp; /** pause delay of 2^x after reset x=[0-15] */
	unsigned int enable_delay; /** pause delay after enable */
	unsigned int pause_delay; /** pause delay after CCTM pause */
	al_bool inc_val_en; /** enable counter inc value by ext unit (0-inc by 1)*/
	al_bool saturate_en; /** enable window counter saturation (0-wrap around) */
	al_bool windows_en; /** enable window framing (0-infinite window) */
	al_bool detach_cctm; /** pmu can be enabled locally regardless of CCTM */
	unsigned int aux_control; /** bit mask of outgoing auxiliary signals */
	uint64_t tstamp_init; /** initial value to start time stamp counting from */
};

/** PMU complex information */
struct al_pmdt_pmu_cmplx_info {
	unsigned int rev;
	unsigned int ela_num; /** number of available ELAs in CMPLX */
	unsigned int axi_mon_num; /** number of available AXI MONITORs in CMPLX */
	unsigned int trk_num; /** number of tracked event channels per AXI MON */
	unsigned int trc_num; /** number of trace channels per ELA */
	unsigned int events_num; /** number of available event counters in PMU */
	unsigned int active_events_num; /** number of active tracked events */
	unsigned int active_events; /** bitwise presentation of active tracked events */
};

/** PMU report */
struct al_pmdt_pmu_report {
	al_bool ovf_violation; /* last violation is ovf */
	al_bool unf_violation; /* last violation is unf */
	unsigned int event_num; /** last event that triggered this violation */
	unsigned int violation_cnt; /**accumulated violations of this event so far*/
	uint64_t report_timestamp;
	uint64_t window_cnt; /** window count at the report moment */
};

/** Event configuration parameters */
struct al_pmdt_pmu_event_config {
	al_bool window_en; /** enable window frame, otherwise accumulate forever */
	al_bool gpion_en; /** enable gpio routing for this event */
	al_bool pause_en; /** enable incoming pause commands */
	al_bool reroute_en; /** enable other event counter violation as a trigger */
	unsigned int trig_num; /** trigger number associated to this event counter.
	0-63 are external triggers,
	64-127 are pmdt internal triggers,
	in case reroute_en=1, this number represents the rerouted event counter */
	uint32_t cross_trig; /**local-trigger-pause enablement mask (not via CCTM)*/
	uint64_t thr_low; /** low threshold for underflow event */
	uint64_t thr_high; /** high threshold for overflow event */
};

struct al_pmdt_pmu_event_counters {
	unsigned int violation_cnt;
	uint64_t last_event_cnt; /** from the last window frame */
	uint64_t cur_event_cnt;
};

/** Event status */
struct al_pmdt_pmu_event_status {
	al_bool any_violation;
	al_bool ovf_violation;
	al_bool unf_violation;
	al_bool paused;
	al_bool active;
	unsigned int trig_num; /** attached trigger number */
	struct al_pmdt_pmu_event_counters counters;
};

/** PMU context */
struct al_pmdt_pmu {
	const char *name;
	void __iomem *regs;
	uint32_t core_clock_freq; /** in KHz */
	unsigned int events_num;
	unsigned int axi_mon_num;
	unsigned int ela_num;
};

/**
 * Initialize the PMU handle
 *
 * @param pmdt_pmu
 *		An allocated, non-initialized instance
 * @param core_name
 *		The name of the core PMU is integrated into
 *		Upper layer must keep string area
 * @param pmu_cmplx_base
 *		The base address of the PMU complex registers
 * @param core_clock_freq
 *		The local core clock frequency in KHz
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_pmdt_pmu_handle_init(
		struct al_pmdt_pmu *pmdt_pmu,
		const char *core_name,
		void __iomem *pmu_cmplx_base,
		unsigned int core_clock_freq);

/**
 * Retrieve PMU iofic register base address
 *
 * There are two iofic PMU groups:
 * Group A - represents violation interrupts of events 0-31 respectively
 * Group B - represents generic PMU status interrupts (al_pmdt_pmu_grp_b_irq)
 *
 * @param pmdt_pmu
 *		PMU handle
 *
 * @return	iofic base address
 */
void __iomem *al_pmdt_pmu_iofic_base_get(
		struct al_pmdt_pmu *pmdt_pmu);

/**
 * Retrieve PMU complex information
 *
 * @param pmdt_pmu
 *		PMU handle
 * @param info
 *		Allocated info instance (out)
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_pmdt_pmu_cmplx_info_get(
		struct al_pmdt_pmu *pmdt_pmu,
		struct al_pmdt_pmu_cmplx_info *info);

/**
 * Configure the PMU
 *
 * Note. changing enabled-PMU configuration will automatically disable it but
 * won't clear it (for clearing call al_pmdt_pmu_clear explicitly)
 *
 * @param pmdt_pmu
 *		PMU handle
 * @param config
 *		PMU configuration parameters
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_pmdt_pmu_config(
		struct al_pmdt_pmu *pmdt_pmu,
		const struct al_pmdt_pmu_config *config);

/**
 * Set sampling window size
 *
 * - relevant only if al_pmdt_pmu_config->windows_en = 1
 * - per event, relevant only if al_pmdt_pmu_event_config->window_en=1
 * - window unit = 10^(al_pmdt_pmu_config->window_unit_exp) seconds
 *
 * @param pmdt_pmu
 *		PMU handle
 * @param size
 *		Window size in window units
 */
void al_pmdt_pmu_window_size_set(
		struct al_pmdt_pmu *pmdt_pmu,
		unsigned int size);

/**
 * Enable/disable PMU
 *
 * @param pmdt_pmu
 *		PMU handle
 * @param is_enable
 *		0/1 - disable/enable
 */
void al_pmdt_pmu_enable(
		struct al_pmdt_pmu *pmdt_pmu,
		al_bool is_enable);

/**
 * Clear PMU state and event counters
 *
 * @param pmdt_pmu
 *		PMU handle
 */
void al_pmdt_pmu_clear(struct al_pmdt_pmu *pmdt_pmu);

/**
 * Retrieve current time stamp
 *
 * - time stamp configuration is done via CCTM to all units
 *
 * @param pmdt_pmu
 *		PMU handle
 *
 * @return current time stamp
 */
uint64_t al_pmdt_pmu_timestamp_get(struct al_pmdt_pmu *pmdt_pmu);

/**
 * Retrieve current window count
 *
 * - time stamp configuration is done via CCTM to all units
 *
 * @param pmdt_pmu
 *		PMU handle
 *
 * @return current window count
 */
uint64_t al_pmdt_pmu_window_count_get(struct al_pmdt_pmu *pmdt_pmu);

/**
 * Retrieve PMU report of the last violation trigger
 *
 * @param pmdt_pmu
 *		PMU handle
 * @param report
 *		Allocated report instance (out)
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_pmdt_pmu_report_get(
		struct al_pmdt_pmu *pmdt_pmu,
		struct al_pmdt_pmu_report *report);

/**
 * Configure a single event
 *
 * @param pmdt_pmu
 *		PMU handle
 * @param event_num
 *		Number of the event to be configured (0-31)
 * @param config
 *		Event configuration parameters
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_pmdt_pmu_event_config(
		struct al_pmdt_pmu *pmdt_pmu,
		unsigned int event_num,
		const struct al_pmdt_pmu_event_config *config);

/**
 * Enable/disable event/s
 *
 * @param pmdt_pmu
 *		PMU handle
 * @param is_enable
 *		0/1 - disabled/enabled
 * @param events_mask
 *		Mask of the events to be enabled/disabled (1-en/dis, 0-untouched).
 *		The current enablement mask is returned via this parameter.
 */
void al_pmdt_pmu_events_enable(
		struct al_pmdt_pmu *pmdt_pmu,
		al_bool is_enable,
		uint32_t *events_mask);

/**
 * Set clear events enablement mask for clear command coming form CCTM
 *
 * @param pmdt_pmu
 *		PMU handle
 * @param is_enable
 *		0/1 - disabled/enabled
 * @param events_mask
 *		Mask of the events to be enabled/disabled (1-en/dis, 0-untouched).
 *		The current enablement mask is returned via this parameter.
 */
void al_pmdt_pmu_events_clear_enable(
		struct al_pmdt_pmu *pmdt_pmu,
		al_bool is_enable,
		uint32_t *events_mask);

/**
 * Retrieve a single event status
 *
 * @param pmdt_pmu
 *		PMU handle
 * @param event_num
 *		Number of the inquired event (0-31)
 * @param status
 *		Allocated status instance (out)
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_pmdt_pmu_event_status_get(
		struct al_pmdt_pmu *pmdt_pmu,
		unsigned int event_num,
		struct al_pmdt_pmu_event_status *status);

/**
 * Enable/disable dst-event to be paused
 * by source-event by firing a local event trigger
 *
 * @param pmdt_pmu
 *		PMU handle
 * @param is_enable
 *		AL_TRUE/AL_FALSE - disable/enable
 * @param dst_event
 *		the destination PMU event to be paused
 * @param src_events_mask
 *		bit mask of source events that can trigger pause to dst-event
 *		(1 - enable/disable, 0 - untouched)
 */
void al_pmdt_pmu_cross_pause_enable(
		struct al_pmdt_pmu *pmdt_pmu,
		al_bool is_enable,
		unsigned int dst_event,
		uint32_t src_events_mask);

/**
 * Retrieve a single event counters
 *
 * @param pmdt_pmu
 *		PMU handle
 * @param event_num
 *		Number of the inquired event (0-31)
 * @param counters
 *		Allocated counters instance (out)
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_pmdt_pmu_event_counters_get(
		struct al_pmdt_pmu *pmdt_pmu,
		unsigned int event_num,
		struct al_pmdt_pmu_event_counters *counters);

/**
 * Retrieve a single event violation counter
 *
 * @param pmdt_pmu
 *		PMU handle
 * @param event_num
 *		Number of the inquired event (0-31)
 * @param violation_cnt
 *		Allocated counter instance (out)
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_pmdt_pmu_event_viol_counter_get(
		struct al_pmdt_pmu *pmdt_pmu,
		unsigned int event_num,
		unsigned int *violation_cnt);

/**
 * Retrieve a single event current counter
 *
 * @param pmdt_pmu
 *		PMU handle
 * @param event_num
 *		Number of the inquired event (0-31)
 * @param cur_event_cnt
 *		Allocated counter instance (out)
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_pmdt_pmu_event_cur_counter_get(
		struct al_pmdt_pmu *pmdt_pmu,
		unsigned int event_num,
		uint64_t *cur_event_cnt);

/**
 * Retrieve a single event last counter
 *
 * @param pmdt_pmu
 *		PMU handle
 * @param event_num
 *		Number of the inquired event (0-31)
 * @param last_event_cnt
 *		Allocated counter instance (out)
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_pmdt_pmu_event_last_counter_get(
		struct al_pmdt_pmu *pmdt_pmu,
		unsigned int event_num,
		uint64_t *last_event_cnt);

/**
 * Set local pulse trigger to specific event/s
 *
 * @param pmdt_pmu
 *		PMU handle
 * @param events_mask
 *		Mask of the events to be triggered
 */
void al_pmdt_pmu_local_trigger_set(
		struct al_pmdt_pmu *pmdt_pmu,
		uint32_t events_mask);

/**
 * Enable/disable event counter trace via ELA for specific event number
 *
 * Note. only one event number can be traced, enabling event x will disable
 * event y that was previously enabled.
 * Trace vector structure:
 *
 *   127                   68            64 63                                0
 * ----------------------------------------------------------------------------
 * |valid|    rsv         |selected counter|             event count          |
 * ----------------------------------------------------------------------------
 *
 * @param pmdt_pmu
 *		PMU handle
 * @param is_enable
 *		0/1 - enable/disable (if disabled the event_num is ignored)
 * @param event_num
 *		Event number to be traced
 */
void al_pmdt_pmu_trace_enable(
		struct al_pmdt_pmu *pmdt_pmu,
		al_bool is_enable,
		unsigned int event_num);

/**
 * Simple PMU init (simple event counting)
 *
 * @param	pmdt_pmu
 *		Initialized PMU handle
 */
void al_pmdt_pmu_simple_init(
	struct al_pmdt_pmu *pmdt_pmu);

/**
 * Simple PMU get number of available counters
 *
 * @param	pmdt_pmu
 *		Initialized PMU handle
 *
 * @returns	Number of available counters
 */
unsigned int al_pmdt_pmu_simple_cnt_num_get(
	struct al_pmdt_pmu *pmdt_pmu);

/**
 * Simple PMU counter configuration
 *
 * @param	pmdt_pmu
 *		Initialized PMU handle
 * @param	cnt
 *		Index of counter to configure
 * @param	event_id
 *		Index of event to count
 */
void al_pmdt_pmu_simple_cnt_config(
	struct al_pmdt_pmu	*pmdt_pmu,
	unsigned int		cnt,
	unsigned int		event_id);

/**
 * Simple PMU start counting
 *
 * @param	pmdt_pmu
 *		Initialized PMU handle
 */
void al_pmdt_pmu_simple_cnt_start(
	struct al_pmdt_pmu	*pmdt_pmu);

/**
 * Simple PMU stop counting
 *
 * @param	pmdt_pmu
 *		Initialized PMU handle
 */
void al_pmdt_pmu_simple_cnt_stop(
	struct al_pmdt_pmu	*pmdt_pmu);

/**
 * Simple PMU clear counters
 *
 * @param	pmdt_pmu
 *		Initialized PMU handle
 */
void al_pmdt_pmu_simple_cnt_clear(
	struct al_pmdt_pmu	*pmdt_pmu);

/**
 * Simple PMU get counter current count
 *
 * @param	pmdt_pmu
 *		Initialized PMU handle
 * @param	cnt
 *		Counter index
 *
 * @returns	Counter current count
 */
uint64_t al_pmdt_pmu_simple_cnt_get(
	struct al_pmdt_pmu	*pmdt_pmu,
	unsigned int		cnt);

/* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */

#endif /* __AL_HAL_PMDT_PMU_H__ */
/** @} end of group_pmdt_pmu */


