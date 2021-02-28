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
 *  @{
 * @file   al_hal_pmdt_pmu.c
 *
 * @brief  PMDT Performance Monitor Unit HAL functions
 *
 */

#include "al_hal_pmdt_pmu_complex_regs.h"
#include "al_hal_pmdt_pmu.h"
#include "al_hal_pmdt_pmu_regs.h"
#include "al_hal_pmdt_common.h"

/**
 * Calculate timer ratio to achieve the desired window time unit
 *
 * @param clock_freq_khz
 *		Local core clock frequency in KHz
 * @param window_unit_exp
 *		desired window time unit exponent e.g. for 1usec set to -6
 *
 * @return	ratio to be programmed into timer_ratio in pmu_cfg register
 */
static uint32_t calc_timer_ratio(
		uint32_t clock_freq_khz,
		int window_unit_exp)
{
	int i;
	int exp = window_unit_exp+3; /* +3 for Hz-->KHz */
	uint32_t ratio = clock_freq_khz;

	/* divide or multiply according to exp sign */
	if (exp < 0)
		for (i = 0; i < -exp; i++)
			ratio /= 10;
	else
		for (i = 0; i < exp; i++)
			ratio *= 10;

	al_dbg("pmu calc timer ratio: freq_khz %d, window_unit %d, ratio %d\n",
			clock_freq_khz,
			window_unit_exp,
			ratio);

	return ratio;
}

/**
 * Initialize the PMU handle
 */
int al_pmdt_pmu_handle_init(
		struct al_pmdt_pmu *pmdt_pmu,
		const char *core_name,
		void __iomem *pmu_cmplx_base,
		uint32_t core_clock_freq)
{
	struct al_pmdt_pmu_complex_regs *cmplx_regs;
	struct al_pmdt_pmu_cmplx_info info;
	int ret;

	al_assert(pmdt_pmu);
	al_assert(pmu_cmplx_base);
	al_assert(core_clock_freq);

	if (core_name)
		pmdt_pmu->name = core_name;
	else
		pmdt_pmu->name = "";

	cmplx_regs =  (struct al_pmdt_pmu_complex_regs *)pmu_cmplx_base;
	pmdt_pmu->regs = (struct al_pmdt_pmu_regs *)&cmplx_regs->pmu;
	pmdt_pmu->core_clock_freq = core_clock_freq;

	ret = al_pmdt_pmu_cmplx_info_get(pmdt_pmu, &info);

	if (ret < 0)
		return ret;

	pmdt_pmu->axi_mon_num = info.axi_mon_num;
	pmdt_pmu->ela_num = info.ela_num;
	pmdt_pmu->events_num = info.events_num;

	al_dbg("pmdt pmu [%s]: init\n", pmdt_pmu->name);

	return 0;
}

/**
 * Retrieve PMU iofic register base address
 */
void __iomem *al_pmdt_pmu_iofic_base_get(
		struct al_pmdt_pmu *pmdt_pmu)
{
	struct al_pmdt_pmu_regs *regs;

	al_assert(pmdt_pmu);

	regs = (struct al_pmdt_pmu_regs *)pmdt_pmu->regs;
	return (void __iomem *)regs->interrupt_ctl;
}

/**
 * Retrieve PMU complex information
 */
int al_pmdt_pmu_cmplx_info_get(
		struct al_pmdt_pmu *pmdt_pmu,
		struct al_pmdt_pmu_cmplx_info *info)
{
	struct al_pmdt_pmu_regs *regs;
	uint32_t val;

	al_assert(pmdt_pmu);

	regs = (struct al_pmdt_pmu_regs *)pmdt_pmu->regs;
	val = al_reg_read32(&regs->pmu_cfg.pmu_status);

	/* check that event number field is valid */
	if (!(val & PMDT_PMU_PMU_CFG_PMU_STATUS_EVENTS_VALID))
		return -EBUSY;

	info->rev = AL_REG_FIELD_GET(val,
			PMDT_PMU_PMU_CFG_PMU_STATUS_REVISION_MASK,
			PMDT_PMU_PMU_CFG_PMU_STATUS_REVISION_SHIFT);
	info->ela_num = AL_REG_FIELD_GET(val,
			PMDT_PMU_PMU_CFG_PMU_STATUS_ELA_NUM_MASK,
			PMDT_PMU_PMU_CFG_PMU_STATUS_ELA_NUM_SHIFT);
	info->axi_mon_num = AL_REG_FIELD_GET(val,
			PMDT_PMU_PMU_CFG_PMU_STATUS_MON_NUM_MASK,
			PMDT_PMU_PMU_CFG_PMU_STATUS_MON_NUM_SHIFT);
	info->trk_num = AL_REG_FIELD_GET(val,
			PMDT_PMU_PMU_CFG_PMU_STATUS_TRK_NUM_MASK,
			PMDT_PMU_PMU_CFG_PMU_STATUS_TRK_NUM_SHIFT);
	info->trc_num = AL_REG_FIELD_GET(val,
			PMDT_PMU_PMU_CFG_PMU_STATUS_TRC_NUM_MASK,
			PMDT_PMU_PMU_CFG_PMU_STATUS_TRC_NUM_SHIFT);
	info->events_num = AL_REG_FIELD_GET(val,
			PMDT_PMU_PMU_CFG_PMU_STATUS_EVENTS_IMPL_MASK,
			PMDT_PMU_PMU_CFG_PMU_STATUS_EVENTS_IMPL_SHIFT);
	info->active_events_num = AL_REG_FIELD_GET(val,
			PMDT_PMU_PMU_CFG_PMU_STATUS_EVENT_SUM_MASK,
			PMDT_PMU_PMU_CFG_PMU_STATUS_EVENT_SUM_SHIFT);
	info->active_events = al_reg_read32(&regs->events.event);

	return 0;
}

/**
 * Configure the PMU
 */
int al_pmdt_pmu_config(
		struct al_pmdt_pmu *pmdt_pmu,
		const struct al_pmdt_pmu_config *config)
{
	struct al_pmdt_pmu_regs *regs;
	uint32_t val = 0;
	uint32_t timer_ratio = 0;

	al_assert(pmdt_pmu);
	al_assert(config);
	al_assert(AL_REG_FIELD_RANGE_VALID(config->reset_delay_exp,
			PMDT_PMU_PMU_CFG_GENERAL_PAUSE_EXTEND_MASK,
			PMDT_PMU_PMU_CFG_GENERAL_PAUSE_EXTEND_SHIFT));
	al_assert(AL_REG_FIELD_RANGE_VALID(config->aux_control,
			PMDT_PMU_SPECIALS_TRACE_SELECT_AUXILIARY_MASK,
			PMDT_PMU_SPECIALS_TRACE_SELECT_AUXILIARY_SHIFT));

	regs = (struct al_pmdt_pmu_regs *)pmdt_pmu->regs;

	if (config->windows_en) {
		timer_ratio = calc_timer_ratio(pmdt_pmu->core_clock_freq,
				config->window_unit_exp);
		/* check that desired window_unit_exp can be satisfied */
		al_assert(timer_ratio);
		al_assert(AL_REG_FIELD_RANGE_VALID(timer_ratio,
				PMDT_PMU_PMU_CFG_GENERAL_TIMER_RATIO_MASK,
				PMDT_PMU_PMU_CFG_GENERAL_TIMER_RATIO_SHIFT));
	}

	AL_REG_FIELD_SET(val,
			PMDT_PMU_PMU_CFG_GENERAL_TIMER_RATIO_MASK,
			PMDT_PMU_PMU_CFG_GENERAL_TIMER_RATIO_SHIFT,
			timer_ratio);
	AL_REG_MASK_SET_VAL(val,
			PMDT_PMU_PMU_CFG_GENERAL_ENABLE_OVERRIDE,
			config->detach_cctm);
	AL_REG_MASK_SET_VAL(val,
			PMDT_PMU_PMU_CFG_GENERAL_START_BOUNDARY,
			config->start_on_window);
	AL_REG_MASK_SET_VAL(val,
			PMDT_PMU_PMU_CFG_GENERAL_GPIO_EN,
			config->gpio_en);
	AL_REG_MASK_SET_VAL(val,
			PMDT_PMU_PMU_CFG_GENERAL_PULSE_TRIG,
			config->pulse_trig);
	AL_REG_MASK_SET_VAL(val,
			PMDT_PMU_PMU_CFG_GENERAL_UNF_TRIG_EN,
			config->unf_trig_en);
	AL_REG_MASK_SET_VAL(val,
			PMDT_PMU_PMU_CFG_GENERAL_OVF_TRIG_EN,
			config->ovf_trig_en);
	AL_REG_MASK_SET_VAL(val,
			PMDT_PMU_PMU_CFG_GENERAL_UNF_INTR_EN,
			config->unf_int_en);
	AL_REG_MASK_SET_VAL(val,
			PMDT_PMU_PMU_CFG_GENERAL_ERROR_GPIO_EN,
			config->error_gpio_en);
	AL_REG_MASK_SET_VAL(val,
			PMDT_PMU_PMU_CFG_GENERAL_ERROR_TRIG_EN,
			config->error_trig_en);
	AL_REG_MASK_SET_VAL(val,
			PMDT_PMU_PMU_CFG_GENERAL_OVF_INTR_EN,
			config->ovf_int_en);
	AL_REG_FIELD_SET(val,
			PMDT_PMU_PMU_CFG_GENERAL_PAUSE_EXTEND_MASK,
			PMDT_PMU_PMU_CFG_GENERAL_PAUSE_EXTEND_SHIFT,
			config->reset_delay_exp);
	AL_REG_MASK_SET_VAL(val,
			PMDT_PMU_PMU_CFG_GENERAL_INC_VAL_EN,
			config->inc_val_en);
	AL_REG_MASK_SET_VAL(val,
			PMDT_PMU_PMU_CFG_GENERAL_SATURATE_EN,
			config->saturate_en);
	AL_REG_MASK_SET_VAL(val,
			PMDT_PMU_PMU_CFG_GENERAL_WINDOWS_EN,
			config->windows_en);

	al_reg_write32(&regs->pmu_cfg.general, val);

	/* set delay values */
	al_reg_write32(&regs->pmu_cfg.delayed_enable, config->enable_delay);
	al_reg_write32(&regs->pmu_cfg.delayed_pause, config->pause_delay);

	/* set auxiliary control */
	val = al_reg_read32(&regs->specials.trace_select);
	AL_REG_FIELD_SET(val,
			PMDT_PMU_SPECIALS_TRACE_SELECT_AUXILIARY_MASK,
			PMDT_PMU_SPECIALS_TRACE_SELECT_AUXILIARY_SHIFT,
			config->aux_control);
	al_reg_write32(&regs->specials.trace_select, val);

	/* set time stamp counter initial value */
	al_hal_pmdt_write64(config->tstamp_init,
			&regs->specials.tstamp_offset_low,
			&regs->specials.tstamp_offset_high);

	return 0;
}

/**
 * Set sampling window size
 */
void al_pmdt_pmu_window_size_set(
		struct al_pmdt_pmu *pmdt_pmu,
		unsigned int size)
{
	struct al_pmdt_pmu_regs *regs;

	al_assert(pmdt_pmu);

	regs = (struct al_pmdt_pmu_regs *)pmdt_pmu->regs;
	al_reg_write32(&regs->pmu_cfg.window, size);
}

/**
 * Enable/disable PMU
 */
void al_pmdt_pmu_enable(
		struct al_pmdt_pmu *pmdt_pmu,
		al_bool is_enable)
{
	struct al_pmdt_pmu_regs *regs;
	uint32_t val;

	al_assert(pmdt_pmu);

	regs = (struct al_pmdt_pmu_regs *)pmdt_pmu->regs;

	val = al_reg_read32(&regs->pmu_cfg.general);
	AL_REG_MASK_SET_VAL(val,
			PMDT_PMU_PMU_CFG_GENERAL_PMU_ENABLE,
			is_enable);
	al_reg_write32(&regs->pmu_cfg.general, val);
}

/**
 * Clear PMU state and event counters
 */
void al_pmdt_pmu_clear(struct al_pmdt_pmu *pmdt_pmu)
{
	struct al_pmdt_pmu_regs *regs;
	uint32_t val;

	al_assert(pmdt_pmu);

	regs = (struct al_pmdt_pmu_regs *)pmdt_pmu->regs;

	val = al_reg_read32(&regs->pmu_cfg.general);
	AL_REG_MASK_SET_VAL(val, PMDT_PMU_PMU_CFG_GENERAL_CLEAR, 1);
	al_reg_write32(&regs->pmu_cfg.general, val);
}

/**
 * Retrieve current time stamp
 */
uint64_t al_pmdt_pmu_timestamp_get(struct al_pmdt_pmu *pmdt_pmu)
{
	struct al_pmdt_pmu_regs *regs;
	uint64_t ts;

	al_assert(pmdt_pmu);

	regs = (struct al_pmdt_pmu_regs *)pmdt_pmu->regs;
	ts = al_hal_pmdt_read64_counter(&regs->pmu_cfg.tstamp_low,
			&regs->pmu_cfg.tstamp_high);

	return ts;
}

/**
 * Retrieve current window count
 */
uint64_t al_pmdt_pmu_window_count_get(struct al_pmdt_pmu *pmdt_pmu)
{
	struct al_pmdt_pmu_regs *regs;
	uint64_t wc;

	al_assert(pmdt_pmu);

	regs = (struct al_pmdt_pmu_regs *)pmdt_pmu->regs;
	wc = al_hal_pmdt_read64_counter(&regs->pmu_cfg.window_low,
			&regs->pmu_cfg.window_high);

	return wc;
}

/**
 * Retrieve PMU report of the last violation trigger
 */
int al_pmdt_pmu_report_get(
		struct al_pmdt_pmu *pmdt_pmu,
		struct al_pmdt_pmu_report *report)
{
	struct al_pmdt_pmu_regs *regs;
	uint32_t report_status;
	uint64_t wc;

	al_assert(pmdt_pmu);
	al_assert(report);

	regs = (struct al_pmdt_pmu_regs *)pmdt_pmu->regs;
	/* check that we're not on window boundary - 1st sample */
	wc = al_pmdt_pmu_window_count_get(pmdt_pmu);
	al_data_memory_barrier();

	report_status = al_reg_read32(&regs->pmu_cfg.report_status);
	report->ovf_violation = AL_REG_MASK_IS_SET(report_status,
			PMDT_PMU_PMU_CFG_REPORT_STATUS_OVF);
	report->unf_violation = AL_REG_MASK_IS_SET(report_status,
				PMDT_PMU_PMU_CFG_REPORT_STATUS_UNF);
	report->event_num = AL_REG_FIELD_GET(report_status,
			PMDT_PMU_PMU_CFG_REPORT_STATUS_EVENT_NUM_MASK,
			PMDT_PMU_PMU_CFG_REPORT_STATUS_EVENT_NUM_SHIFT);
	report->violation_cnt = al_reg_read32(&regs->pmu_cfg.report_viol_cnt);
	report->report_timestamp = al_hal_pmdt_read64_counter(
			&regs->pmu_cfg.report_tstamp_low,
			&regs->pmu_cfg.report_tstamp_high);
	report->window_cnt = al_hal_pmdt_read64_counter(
			&regs->pmu_cfg.report_window_low,
			&regs->pmu_cfg.report_window_high);

	/* check that we're not on window boundary - 2nd sample */
	al_data_memory_barrier();
	if (wc < al_pmdt_pmu_window_count_get(pmdt_pmu))
		return -EBUSY;

	return 0;
}

/**
 * Configure a single event
 */
int al_pmdt_pmu_event_config(
		struct al_pmdt_pmu *pmdt_pmu,
		unsigned int event_num,
		const struct al_pmdt_pmu_event_config *config)
{
	struct al_pmdt_pmu_regs *regs;
	uint32_t val;

	al_assert(pmdt_pmu);
	al_assert(event_num < pmdt_pmu->events_num);
	al_assert(config);
	al_assert(AL_REG_FIELD_RANGE_VALID(config->trig_num,
			PMDT_PMU_CROSS_MATRIX_EVENT_MAP_TRIG_NUM_MASK,
			PMDT_PMU_CROSS_MATRIX_EVENT_MAP_TRIG_NUM_SHIFT));

	regs = (struct al_pmdt_pmu_regs *)pmdt_pmu->regs;

	/* set event's enable bits */
	val = al_reg_read32(&regs->events.window_disable);
	if (config->window_en)
		val &= ~AL_BIT(event_num);
	else
		val |= AL_BIT(event_num);
	al_reg_write32(&regs->events.window_disable, val);

	val = al_reg_read32(&regs->events.gpio);
	if (config->gpion_en)
		val |= AL_BIT(event_num);
	else
		val &= ~AL_BIT(event_num);
	al_reg_write32(&regs->events.gpio, val);

	val = al_reg_read32(&regs->events.pause_group);
	if (config->pause_en)
		val |= AL_BIT(event_num);
	else
		val &= ~AL_BIT(event_num);
	al_reg_write32(&regs->events.pause_group, val);

	/* associate event counter to internal/external trigger */
	val = 0;
	AL_REG_MASK_SET_VAL(val,
			PMDT_PMU_CROSS_MATRIX_EVENT_MAP_TRIG_REROUTE,
			config->reroute_en);
	AL_REG_FIELD_SET(val,
			PMDT_PMU_CROSS_MATRIX_EVENT_MAP_TRIG_NUM_MASK,
			PMDT_PMU_CROSS_MATRIX_EVENT_MAP_TRIG_NUM_SHIFT,
			config->trig_num);
	al_reg_write32(&regs->cross_matrix[event_num].event_map, val);

	/* enable internal pause triggers */
	al_reg_write32(&regs->cross_matrix[event_num].cross_trig,
			config->cross_trig);

	/* set violation thresholds */
	al_hal_pmdt_write64(config->thr_low,
			&regs->cross_matrix[event_num].threshold_low_lo,
			&regs->cross_matrix[event_num].threshold_low_hi);
	al_hal_pmdt_write64(config->thr_high,
				&regs->cross_matrix[event_num].threshold_high_lo,
				&regs->cross_matrix[event_num].threshold_high_hi);
	return 0;
}

/**
 * Enable/disable event/s
 */
void al_pmdt_pmu_events_enable(
		struct al_pmdt_pmu *pmdt_pmu,
		al_bool is_enable,
		uint32_t *events_mask)
{
	struct al_pmdt_pmu_regs *regs;
	uint32_t val;

	al_assert(pmdt_pmu);
	al_assert(events_mask);

	regs = (struct al_pmdt_pmu_regs *)pmdt_pmu->regs;

	val = al_reg_read32(&regs->events.event);
	if (is_enable)
		val |= *events_mask;
	else
		val &= ~(*events_mask);
	al_reg_write32(&regs->events.event, val);

	/* return the current enabled events */
	*events_mask = val;
}

/**
 * Set clear events enablement mask for clear command coming from CCTM
 */
void al_pmdt_pmu_events_clear_enable(
		struct al_pmdt_pmu *pmdt_pmu,
		al_bool is_enable,
		uint32_t *events_mask)
{
	struct al_pmdt_pmu_regs *regs;
	uint32_t val;

	al_assert(pmdt_pmu);
	al_assert(events_mask);

	regs = (struct al_pmdt_pmu_regs *)pmdt_pmu->regs;

	val = al_reg_read32(&regs->events.clear_group);
	if (is_enable)
		val |= *events_mask;
	else
		val &= ~(*events_mask);
	al_reg_write32(&regs->events.clear_group, val);

	/* return the current enabled events */
	*events_mask = val;
}

/**
 * Retrieve a single event status
 */
int al_pmdt_pmu_event_status_get(
		struct al_pmdt_pmu *pmdt_pmu,
		unsigned int event_num,
		struct al_pmdt_pmu_event_status *status)
{
	struct al_pmdt_pmu_regs *regs;
	uint32_t event_status;
	uint64_t wc;

	al_assert(pmdt_pmu);
	al_assert(event_num < pmdt_pmu->events_num);
	al_assert(status);

	regs = (struct al_pmdt_pmu_regs *)pmdt_pmu->regs;
	/* check that we're not on window boundary - 1st sample */
	wc = al_pmdt_pmu_window_count_get(pmdt_pmu);
	al_data_memory_barrier();

	event_status = al_reg_read32(&regs->cross_matrix[event_num].event_status);

	/* check that event status is valid */
	if (!(event_status & PMDT_PMU_CROSS_MATRIX_EVENT_STATUS_VALID))
		return -EBUSY;

	status->any_violation = AL_REG_MASK_IS_SET(event_status,
			PMDT_PMU_CROSS_MATRIX_EVENT_STATUS_VIOL);
	status->ovf_violation = AL_REG_MASK_IS_SET(event_status,
			PMDT_PMU_CROSS_MATRIX_EVENT_STATUS_OVF);
	status->unf_violation = AL_REG_MASK_IS_SET(event_status,
			PMDT_PMU_CROSS_MATRIX_EVENT_STATUS_UNF);
	status->paused = AL_REG_MASK_IS_SET(event_status,
			PMDT_PMU_CROSS_MATRIX_EVENT_STATUS_PAUSED);
	status->active = (event_status &
			PMDT_PMU_CROSS_MATRIX_EVENT_STATUS_STATUS_MASK) ?
					AL_TRUE : AL_FALSE;
	status->trig_num = AL_REG_FIELD_GET(event_status,
			PMDT_PMU_CROSS_MATRIX_EVENT_STATUS_TRIG_NUM_MASK,
			PMDT_PMU_CROSS_MATRIX_EVENT_STATUS_TRIG_NUM_SHIFT);

	status->counters.violation_cnt =
			al_reg_read32(&regs->cross_matrix[event_num].violation_count);
	status->counters.cur_event_cnt = al_hal_pmdt_read64_counter(
			&regs->cross_matrix[event_num].current_cnt_lo,
			&regs->cross_matrix[event_num].current_cnt_hi);
	status->counters.last_event_cnt = al_hal_pmdt_read64_counter(
			&regs->cross_matrix[event_num].previous_cnt_lo,
			&regs->cross_matrix[event_num].previous_cnt_hi);

	/* check that we're not on window boundary - 2nd sample */
	al_data_memory_barrier();

	if (wc < al_pmdt_pmu_window_count_get(pmdt_pmu))
		return -EBUSY;

	return 0;
}

/**
 * Enable/disable source-event to pause dest-event by firing an events trigger
 */
void al_pmdt_pmu_cross_pause_enable(
		struct al_pmdt_pmu *pmdt_pmu,
		al_bool is_enable,
		unsigned int dst_event,
		uint32_t src_events_mask)
{
	struct al_pmdt_pmu_regs *regs;

	al_assert(pmdt_pmu);
	al_assert(dst_event < pmdt_pmu->events_num);
	al_assert(src_events_mask < AL_BIT(pmdt_pmu->events_num));

	regs = (struct al_pmdt_pmu_regs *)pmdt_pmu->regs;
	if (is_enable)
		al_reg_write32_masked(&regs->cross_matrix[dst_event].cross_trig,
				src_events_mask, src_events_mask);
	else
		al_reg_write32_masked(&regs->cross_matrix[dst_event].cross_trig,
				src_events_mask, ~src_events_mask);
}

int al_pmdt_pmu_event_counters_get(
		struct al_pmdt_pmu *pmdt_pmu,
		unsigned int event_num,
		struct al_pmdt_pmu_event_counters *counters)
{
	struct al_pmdt_pmu_regs *regs;
	uint32_t val;
	uint64_t wc1 = 0, wc2 = 0;
	int window_enabled, i;

	al_assert(pmdt_pmu);
	al_assert(event_num < pmdt_pmu->events_num);
	al_assert(counters);

	regs = (struct al_pmdt_pmu_regs *)pmdt_pmu->regs;

	val = al_reg_read32(&regs->pmu_cfg.general);
	window_enabled = AL_REG_FIELD_BIT_GET(val, PMDT_PMU_PMU_CFG_GENERAL_WINDOWS_EN);

	if (window_enabled) {
		/* wait for window crossing so we have enough time to sample metrics */
		for (i = 0; i < 100; i++) {
			wc2 = al_pmdt_pmu_window_count_get(pmdt_pmu);
			wc1 = al_pmdt_pmu_window_count_get(pmdt_pmu);
			if (wc1 != wc2)
				break;
		}
		al_data_memory_barrier();
	}

	counters->violation_cnt =
			al_reg_read32(&regs->cross_matrix[event_num].violation_count);
	counters->cur_event_cnt = al_hal_pmdt_read64_counter(
			&regs->cross_matrix[event_num].current_cnt_lo,
			&regs->cross_matrix[event_num].current_cnt_hi);
	counters->last_event_cnt = al_hal_pmdt_read64_counter(
			&regs->cross_matrix[event_num].previous_cnt_lo,
			&regs->cross_matrix[event_num].previous_cnt_hi);

	if (window_enabled) {
		/* check that we didn't cross window boundary - 2nd sample */
		al_data_memory_barrier();
		wc2 = al_pmdt_pmu_window_count_get(pmdt_pmu);
		if (wc1 < wc2)
			return -EBUSY;
	}

	return 0;
}

int al_pmdt_pmu_event_viol_counter_get(
		struct al_pmdt_pmu *pmdt_pmu,
		unsigned int event_num,
		unsigned int *violation_cnt)
{
	struct al_pmdt_pmu_regs *regs;
	uint64_t wc;

	al_assert(pmdt_pmu);
	al_assert(event_num < pmdt_pmu->events_num);
	al_assert(violation_cnt);

	regs = (struct al_pmdt_pmu_regs *)pmdt_pmu->regs;
	/* check that we're not on window boundary - 1st sample */
	wc = al_pmdt_pmu_window_count_get(pmdt_pmu);
	al_data_memory_barrier();

	*violation_cnt = al_reg_read32(&regs->cross_matrix[event_num].violation_count);

	/* check that we're not on window boundary - 2nd sample */
	al_data_memory_barrier();
	if (wc < al_pmdt_pmu_window_count_get(pmdt_pmu))
			return -EBUSY;

	return 0;
}

int al_pmdt_pmu_event_cur_counter_get(
		struct al_pmdt_pmu *pmdt_pmu,
		unsigned int event_num,
		uint64_t *cur_event_cnt)
{
	struct al_pmdt_pmu_regs *regs;
	uint64_t wc;

	al_assert(pmdt_pmu);
	al_assert(event_num < pmdt_pmu->events_num);
	al_assert(cur_event_cnt);

	regs = (struct al_pmdt_pmu_regs *)pmdt_pmu->regs;
	/* check that we're not on window boundary - 1st sample */
	wc = al_pmdt_pmu_window_count_get(pmdt_pmu);
	al_data_memory_barrier();

	*cur_event_cnt = al_hal_pmdt_read64_counter(
			&regs->cross_matrix[event_num].current_cnt_lo,
			&regs->cross_matrix[event_num].current_cnt_hi);

	/* check that we're not on window boundary - 2nd sample */
	al_data_memory_barrier();
	if (wc < al_pmdt_pmu_window_count_get(pmdt_pmu))
			return -EBUSY;

	return 0;
}

int al_pmdt_pmu_event_last_counter_get(
		struct al_pmdt_pmu *pmdt_pmu,
		unsigned int event_num,
		uint64_t *last_event_cnt)
{
	struct al_pmdt_pmu_regs *regs;
	uint64_t wc;

	al_assert(pmdt_pmu);
	al_assert(event_num < pmdt_pmu->events_num);
	al_assert(last_event_cnt);

	regs = (struct al_pmdt_pmu_regs *)pmdt_pmu->regs;
	/* check that we're not on window boundary - 1st sample */
	wc = al_pmdt_pmu_window_count_get(pmdt_pmu);
	al_data_memory_barrier();

	*last_event_cnt = al_hal_pmdt_read64_counter(
			&regs->cross_matrix[event_num].previous_cnt_lo,
			&regs->cross_matrix[event_num].previous_cnt_hi);

	/* check that we're not on window boundary - 2nd sample */
	al_data_memory_barrier();
	if (wc < al_pmdt_pmu_window_count_get(pmdt_pmu))
			return -EBUSY;

	return 0;
}

/**
 * Set local pulse trigger to specific event/s
 */
void al_pmdt_pmu_local_trigger_set(
		struct al_pmdt_pmu *pmdt_pmu,
		uint32_t events_mask)
{
	struct al_pmdt_pmu_regs *regs;

	al_assert(pmdt_pmu);

	regs = (struct al_pmdt_pmu_regs *)pmdt_pmu->regs;
	al_reg_write32(&regs->events.sw_set_trig, events_mask);
}

/**
 * Enable/disable event counter trace via ELA
 */
void al_pmdt_pmu_trace_enable(
		struct al_pmdt_pmu *pmdt_pmu,
		al_bool is_enable,
		unsigned int event_num)
{
	struct al_pmdt_pmu_regs *regs;
	uint32_t val = 0;

	al_assert(pmdt_pmu);
	al_assert(event_num < pmdt_pmu->events_num);

	regs = (struct al_pmdt_pmu_regs *)pmdt_pmu->regs;

	val = al_reg_read32(&regs->specials.trace_select);
	AL_REG_MASK_SET_VAL(val,
			PMDT_PMU_SPECIALS_TRACE_SELECT_TRACE_EN,
			is_enable);
	AL_REG_FIELD_SET(val,
			PMDT_PMU_SPECIALS_TRACE_SELECT_SEL_MASK,
			PMDT_PMU_SPECIALS_TRACE_SELECT_SEL_SHIFT,
			event_num);
	al_reg_write32(&regs->specials.trace_select, val);
}

void al_pmdt_pmu_simple_init(
	struct al_pmdt_pmu *pmdt_pmu)
{
	const struct al_pmdt_pmu_config pmu_config = {
		.window_unit_exp = -6,
		.start_on_window = AL_FALSE,
		.pulse_trig = AL_FALSE,
		.gpio_en = AL_FALSE,
		.unf_trig_en = AL_FALSE,
		.ovf_trig_en = AL_FALSE,
		.unf_int_en = AL_FALSE,
		.ovf_int_en = AL_FALSE,
		.reset_delay_exp = 0,
		.enable_delay = 0,
		.pause_delay = 0,
		.inc_val_en = AL_TRUE,
		.saturate_en = AL_FALSE,
		.windows_en = AL_FALSE,
		.detach_cctm = AL_TRUE,
	};
	int err;

	err = al_pmdt_pmu_config(pmdt_pmu, &pmu_config);
	al_assert(!err);
}

unsigned int al_pmdt_pmu_simple_cnt_num_get(
	struct al_pmdt_pmu *pmdt_pmu)
{
	struct al_pmdt_pmu_cmplx_info info;
	int err;

	err = al_pmdt_pmu_cmplx_info_get(pmdt_pmu, &info);
	al_assert(!err);

	return info.events_num;
}

void al_pmdt_pmu_simple_cnt_config(
	struct al_pmdt_pmu	*pmdt_pmu,
	unsigned int		cnt,
	unsigned int		event_id)
{
	const struct al_pmdt_pmu_event_config event_config = {
		.window_en = AL_FALSE,
		.gpion_en = AL_FALSE,
		.pause_en = AL_FALSE,
		.reroute_en = AL_FALSE,
		.cross_trig = 0,
		.thr_high = 0,
		.thr_low = 0,
		.trig_num = event_id,
	};
	uint32_t events_to_enable_mask = AL_BIT(cnt);
	int err;

	err = al_pmdt_pmu_event_config(pmdt_pmu, cnt, &event_config);
	al_assert(!err);

	al_pmdt_pmu_events_enable(pmdt_pmu, AL_TRUE, &events_to_enable_mask);
}

void al_pmdt_pmu_simple_cnt_start(
	struct al_pmdt_pmu	*pmdt_pmu)
{
	al_pmdt_pmu_enable(pmdt_pmu, AL_TRUE);
}

void al_pmdt_pmu_simple_cnt_stop(
	struct al_pmdt_pmu	*pmdt_pmu)
{
	al_pmdt_pmu_enable(pmdt_pmu, AL_FALSE);
}

void al_pmdt_pmu_simple_cnt_clear(
	struct al_pmdt_pmu	*pmdt_pmu)
{
	al_pmdt_pmu_clear(pmdt_pmu);
}

uint64_t al_pmdt_pmu_simple_cnt_get(
	struct al_pmdt_pmu	*pmdt_pmu,
	unsigned int		cnt)
{
	uint64_t cnt_val = 0;
	int err;

	err = al_pmdt_pmu_event_cur_counter_get(pmdt_pmu, cnt, &cnt_val);
	al_assert(!err);

	return cnt_val;
}

/** @} end of PMDT PMU group */

