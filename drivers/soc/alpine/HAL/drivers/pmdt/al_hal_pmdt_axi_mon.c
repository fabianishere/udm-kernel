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
 * @file   al_hal_pmdt_axi_mon.c
 *
 * @brief  PMDT AXI Monitor HAL functions
 *
 */

#include "al_hal_pmdt_pmu_complex_regs.h"
#include "al_hal_pmdt_axi_mon.h"
#include "al_hal_pmdt_axi_mon_regs.h"
#include "al_hal_pmdt_common.h"

/**
 * Verify that trace configuration does not exceed trace line width
 *
 * (TBD- in the future we may need to move it to platform specific file)
 *
 * @param trace
 *		Trace configuration parameters
 *
 * @return	trace configuration validity
 */
static int check_trace_config(const struct al_pmdt_axi_mon_trace *trace)
{
	int sum = 0;

	/* check that trace configuration does not exceed 128bit */
	sum += 5; /* valid bit + track hit bits */
	sum += trace->user_width;
	sum += trace->id_width;
	sum += trace->tgtid_width * 8;
	sum += trace->addr_width * 8;
	sum += trace->incl_busy ? 8 : 0;
	sum += trace->incl_bcount ? 10 : 0;
	sum += trace->incl_data ? 32 : 0;
	sum += trace->incl_attr ? 26 : 0;
	if (sum > 128)
		return -1;

	return 0;
}

/**
 * Initialize the AXI Monitor handle
 */
int al_pmdt_axi_mon_handle_init(
		struct al_pmdt_axi_mon *pmdt_axi_mon,
		const char *core_name,
		unsigned int id,
		void __iomem *pmu_cmplx_base)
{
	struct al_pmdt_pmu_complex_regs *cmplx_regs;

	al_assert(pmdt_axi_mon);
	al_assert(pmu_cmplx_base);
	al_assert(id < AL_PMDT_PMU_CMPLX_AXI_MON_NUM);

	if (core_name)
		pmdt_axi_mon->name = core_name;
	else
		pmdt_axi_mon->name = "";
	pmdt_axi_mon->id = id;

	cmplx_regs =  (struct al_pmdt_pmu_complex_regs *)pmu_cmplx_base;
	pmdt_axi_mon->regs = ((id == 0) ?
			(struct al_pmdt_axi_mon_regs *)&cmplx_regs->axi_monitor_0 :
			(struct al_pmdt_axi_mon_regs *)&cmplx_regs->axi_monitor_1);

	al_dbg("pmdt axi monitor [%s:%d]: init\n",
			pmdt_axi_mon->name, pmdt_axi_mon->id);

	return 0;
}

/**
 * Configure AXI Monitor mode of operation
 */
int al_pmdt_axi_mon_mode_config(
		struct al_pmdt_axi_mon *pmdt_axi_mon,
		const struct al_pmdt_axi_mon_mode *mode)
{
	struct al_pmdt_axi_mon_regs *regs;
	uint32_t val = 0;
	uint32_t outstand_mon = 0, outstand_cont = 0;

	al_assert(pmdt_axi_mon);
	al_assert(mode);
	al_assert(mode->latency_tracker_sel < AL_BIT(AL_PMDT_AXI_MON_TRACKER_NUM));

	regs = (struct al_pmdt_axi_mon_regs *)pmdt_axi_mon->regs;

	AL_REG_MASK_SET_VAL(val,
			PMDT_AXI_MON_AXI_MON_MONITORENABLE_READ,
			mode->is_read);
	AL_REG_MASK_SET_VAL(val,
			PMDT_AXI_MON_AXI_MON_MONITORENABLE_LOG_FRST,
			mode->log_first);
	AL_REG_MASK_SET_VAL(val,
			PMDT_AXI_MON_AXI_MON_MONITORENABLE_USE_CYC_CNT,
			mode->cyc_cnt);
	AL_REG_FIELD_SET(val,
			PMDT_AXI_MON_AXI_MON_MONITORENABLE_LOG_MODE_MASK,
			PMDT_AXI_MON_AXI_MON_MONITORENABLE_LOG_MODE_SHIFT,
			mode->log_mode);
	AL_REG_FIELD_SET(val,
			PMDT_AXI_MON_AXI_MON_MONITORENABLE_LOG_COND_MASK,
			PMDT_AXI_MON_AXI_MON_MONITORENABLE_LOG_COND_SHIFT,
			mode->latency_tracker_sel);

	switch (mode->special_op_mode) {
	case AL_AXI_MON_OP_MODE_LOAD_CONT:
		outstand_cont = 1;
		/* fall through */
	case AL_AXI_MON_OP_MODE_LOAD:
		outstand_mon = 1;
		break;
	default: /* do nothing */
		break;
	}
	AL_REG_MASK_SET_VAL(val,
			PMDT_AXI_MON_AXI_MON_MONITORENABLE_OUTSTAND_MON,
			outstand_mon);
	AL_REG_MASK_SET_VAL(val,
			PMDT_AXI_MON_AXI_MON_MONITORENABLE_OUTSTAND_CONT,
			outstand_cont);

	al_reg_write32(&regs->axi_mon.monitorenable, val);

	return 0;
}

/**
 * Retrieve if monitor configured to monitoring read transaction or write
 */
al_bool al_pmdt_axi_mon_is_read(
		struct al_pmdt_axi_mon *pmdt_axi_mon)
{
	struct al_pmdt_axi_mon_regs *regs;
	uint32_t val = 0;

	al_assert(pmdt_axi_mon);

	regs = (struct al_pmdt_axi_mon_regs *)pmdt_axi_mon->regs;

	val = al_reg_read32(&regs->axi_mon.monitorenable);

	return !!(val & PMDT_AXI_MON_AXI_MON_MONITORENABLE_READ);
}

/**
 * Configure AXI Monitor latency bars
 */
int al_pmdt_axi_mon_bars_config(
		struct al_pmdt_axi_mon *pmdt_axi_mon,
		const struct al_pmdt_axi_mon_bars *bars)
{
	struct al_pmdt_axi_mon_regs *regs;
	int i;
	uint32_t *thr_reg_base;

	al_assert(pmdt_axi_mon);
	al_assert(bars);
	/** verify ascending bar values */
	for (i = 0; i < AL_PMDT_AXI_MON_BAR_NUM-2; i++)
		al_assert(bars->bar_thr[i] <= bars->bar_thr[i+1]);
	al_assert(bars->bar_thr[AL_PMDT_AXI_MON_BAR_NUM-2] <= bars->timeout);

	regs = (struct al_pmdt_axi_mon_regs *)pmdt_axi_mon->regs;
	thr_reg_base = &regs->axi_mon.threshold_0_low;

	/* set bar thresholds */
	for (i = 0; i < AL_PMDT_AXI_MON_BAR_NUM-1; i++)
		al_hal_pmdt_write64(bars->bar_thr[i],
				thr_reg_base + 2*i, thr_reg_base + 2*i+1);

	/* set timeout value */
	al_hal_pmdt_write64(bars->timeout,
			&regs->axi_mon.timeout_low,
			&regs->axi_mon.timeout_high);

	return 0;
}

/**
 * Configure AXI Monitor tracker
 */
int al_pmdt_axi_mon_tracker_config(
		struct al_pmdt_axi_mon *pmdt_axi_mon,
		int tracker_id,
		const struct al_pmdt_axi_mon_tracker *tracker)
{
	struct al_pmdt_axi_mon_regs *regs;
	int i;
	uint32_t *src_sel_reg_base, *src_val_reg_base;

	al_assert(pmdt_axi_mon);
	al_assert(tracker_id < AL_PMDT_AXI_MON_TRACKER_NUM);
	al_assert(tracker);

	regs = (struct al_pmdt_axi_mon_regs *)pmdt_axi_mon->regs;

	src_sel_reg_base = &regs->screen_matrix[tracker_id].scr_sel_0;
	src_val_reg_base = &regs->screen_matrix[tracker_id].scr_val_0;

	for (i = 0; i < AL_PMDT_AXI_MON_TRACKER_WIDTH; i++) {
		al_reg_write32(src_sel_reg_base + i, tracker->mask[i]);
		al_reg_write32(src_val_reg_base + i, tracker->comp[i]);
	}

	return 0;
}

/**
 * Enable/disable AXI Monitor
 */
void al_pmdt_axi_mon_enable(
		struct al_pmdt_axi_mon *pmdt_axi_mon,
		al_bool is_enable)
{
	struct al_pmdt_axi_mon_regs *regs;
	uint32_t val;

	al_assert(pmdt_axi_mon);

	regs = (struct al_pmdt_axi_mon_regs *)pmdt_axi_mon->regs;

	val = al_reg_read32(&regs->axi_mon.monitorenable);
	AL_REG_MASK_SET_VAL(val,
			PMDT_AXI_MON_AXI_MON_MONITORENABLE_ENABLE,
			is_enable);
	al_reg_write32(&regs->axi_mon.monitorenable, val);
}

/**
 * Clear AXI MON results and database
 */
void al_pmdt_axi_mon_clear(
		struct al_pmdt_axi_mon *pmdt_axi_mon,
		al_bool tracker_res,
		al_bool tracker_db,
		al_bool timeout_log)
{
	struct al_pmdt_axi_mon_regs *regs;
	uint32_t val;

	al_assert(pmdt_axi_mon);

	regs = (struct al_pmdt_axi_mon_regs *)pmdt_axi_mon->regs;

	val = al_reg_read32(&regs->axi_mon.monitorenable);
	AL_REG_MASK_SET_VAL(val,
			PMDT_AXI_MON_AXI_MON_MONITORENABLE_CLEAR_MON,
			tracker_res);
	AL_REG_MASK_SET_VAL(val,
			PMDT_AXI_MON_AXI_MON_MONITORENABLE_CLEAR_DBASE,
			tracker_db);
	AL_REG_MASK_SET_VAL(val,
			PMDT_AXI_MON_AXI_MON_MONITORENABLE_CLEAR_TIMEOUT,
			timeout_log);
	al_reg_write32(&regs->axi_mon.monitorenable, val);
}

/**
 * Retrieve current time stamp
 */
uint64_t al_pmdt_axi_mon_timestamp_get(struct al_pmdt_axi_mon *pmdt_axi_mon)
{
	struct al_pmdt_axi_mon_regs *regs;
	uint64_t ts;

	al_assert(pmdt_axi_mon);

	regs = (struct al_pmdt_axi_mon_regs *)pmdt_axi_mon->regs;
	ts = al_hal_pmdt_read64_counter(&regs->axi_mon.tstamp_low,
			&regs->axi_mon.tstamp_high);

	return ts;
}

/**
 * Retrieve AXI MON log
 */
void al_pmdt_axi_mon_log_get(
		struct al_pmdt_axi_mon *pmdt_axi_mon,
		struct al_pmdt_axi_mon_log_entry (*last)[AL_PMDT_AXI_MON_BAR_NUM],
		struct al_pmdt_axi_mon_log_entry *max,
		struct al_pmdt_axi_mon_log_entry *timeout)
{
	struct al_pmdt_axi_mon_regs *regs;
	struct al_pmdt_axi_mon_log_entry *last_event;
	uint32_t *zone_addr_base, *zone_attr_base, *zone_latency_base;
	int i;

	al_assert(pmdt_axi_mon);

	regs = (struct al_pmdt_axi_mon_regs *)pmdt_axi_mon->regs;

	zone_addr_base = &regs->axi_mon.zone_0_addr_low;
	zone_attr_base = &regs->axi_mon.zone_0_attr_low;
	zone_latency_base = &regs->axi_mon.zone_0_period_low;

	if (last) {
		last_event = *last;
		for (i = 0; i < AL_PMDT_AXI_MON_BAR_NUM; i++) {
			last_event[i].address =
					al_hal_pmdt_read64_latched(zone_addr_base + 2*i,
					zone_addr_base + 2*i+1);
			/* attr & latency are latched on address reading */
			al_data_memory_barrier();
			last_event[i].attr = al_hal_pmdt_read64(zone_attr_base + 2*i,
							zone_attr_base + 2*i+1);
			last_event[i].latency = al_hal_pmdt_read64(zone_latency_base + 2*i,
							zone_latency_base + 2*i+1);
		}
	} else {
		/* dummy read, just for latching */
		al_hal_pmdt_read64_latched(zone_addr_base, zone_addr_base+1);
		/* attr & latency are latched on address reading */
		al_data_memory_barrier();
	}

	/* all following registers are also latched on address reading */
	if (max) {
		max->address = al_hal_pmdt_read64(
				&regs->axi_mon.max_addr_low,
				&regs->axi_mon.max_addr_high);
		max->attr = al_hal_pmdt_read64(
				&regs->axi_mon.max_attr_low,
				&regs->axi_mon.max_attr_high);
		max->latency = al_hal_pmdt_read64(
				&regs->axi_mon.max_period_low,
				&regs->axi_mon.max_period_high);
	}
	if (timeout) {
		timeout->address = al_hal_pmdt_read64(
				&regs->axi_mon.timeout_addr_low,
				&regs->axi_mon.timeout_addr_high);
		timeout->attr = al_hal_pmdt_read64(
				&regs->axi_mon.timeout_attr_low,
				&regs->axi_mon.timeout_attr_high);
		timeout->latency = al_hal_pmdt_read64(
				&regs->axi_mon.timeout_period_low,
				&regs->axi_mon.timeout_period_high);
	}
}

/**
 * Retrieve AXI MON busy level
 */
uint32_t al_pmdt_axi_mon_busy_level_get(struct al_pmdt_axi_mon *pmdt_axi_mon)
{
	struct al_pmdt_axi_mon_regs *regs;
	uint32_t bl;

	al_assert(pmdt_axi_mon);

	regs = (struct al_pmdt_axi_mon_regs *)pmdt_axi_mon->regs;
	bl = al_reg_read32(&regs->axi_mon.busy_cnt);

	return bl;
}

/**
 * Configure AXI Monitor trace via ELA
 */
int al_pmdt_axi_mon_trace_config(
		struct al_pmdt_axi_mon *pmdt_axi_mon,
		int trace_id,
		const struct al_pmdt_axi_mon_trace *trace)
{
	struct al_pmdt_axi_mon_regs *regs;
	uint32_t val = 0;

	al_assert(pmdt_axi_mon);
	al_assert(trace_id < AL_PMDT_AXI_MON_TRACE_NUM);
	al_assert(trace);
	al_assert(trace->tracker_sel < AL_BIT(AL_PMDT_AXI_MON_TRACKER_NUM));
	al_assert(AL_REG_FIELD_RANGE_VALID(trace->user_width,
			PMDT_AXI_MON_TRACE_MATRIX_TRC_SEL0_USER_WIDTH_MASK,
			PMDT_AXI_MON_TRACE_MATRIX_TRC_SEL0_USER_WIDTH_SHIFT));
	al_assert(AL_REG_FIELD_RANGE_VALID(trace->id_width,
			PMDT_AXI_MON_TRACE_MATRIX_TRC_SEL0_ID_WIDTH_MASK,
			PMDT_AXI_MON_TRACE_MATRIX_TRC_SEL0_ID_WIDTH_SHIFT));
	al_assert(AL_REG_FIELD_RANGE_VALID(trace->tgtid_width,
			PMDT_AXI_MON_TRACE_MATRIX_TRC_SEL0_TGTID_WIDTH_MASK,
			PMDT_AXI_MON_TRACE_MATRIX_TRC_SEL0_TGTID_WIDTH_SHIFT));
	al_assert(AL_REG_FIELD_RANGE_VALID(trace->addr_width,
			PMDT_AXI_MON_TRACE_MATRIX_TRC_SEL0_ADDR_WIDTH_MASK,
			PMDT_AXI_MON_TRACE_MATRIX_TRC_SEL0_ADDR_WIDTH_SHIFT));

	regs = (struct al_pmdt_axi_mon_regs *)pmdt_axi_mon->regs;

	if (check_trace_config(trace))
		/* we just warn as HW cut the extra bits */
		al_warn("pmdt axi monitor [%s:%d]: trace config exceeds max len\n",
			pmdt_axi_mon->name, pmdt_axi_mon->id);

	AL_REG_FIELD_SET(val,
			PMDT_AXI_MON_TRACE_MATRIX_TRC_SEL0_TRK_COND_MASK,
			PMDT_AXI_MON_TRACE_MATRIX_TRC_SEL0_TRK_COND_SHIFT,
			trace->tracker_sel);
	AL_REG_FIELD_SET(val,
			PMDT_AXI_MON_TRACE_MATRIX_TRC_SEL0_USER_WIDTH_MASK,
			PMDT_AXI_MON_TRACE_MATRIX_TRC_SEL0_USER_WIDTH_SHIFT,
			trace->user_width);
	AL_REG_MASK_SET_VAL(val,
			PMDT_AXI_MON_TRACE_MATRIX_TRC_SEL0_INCL_BUSY,
			trace->incl_busy);
	AL_REG_MASK_SET_VAL(val,
			PMDT_AXI_MON_TRACE_MATRIX_TRC_SEL0_INCL_BCNT,
			trace->incl_bcount);
	AL_REG_FIELD_SET(val,
			PMDT_AXI_MON_TRACE_MATRIX_TRC_SEL0_ID_WIDTH_MASK,
			PMDT_AXI_MON_TRACE_MATRIX_TRC_SEL0_ID_WIDTH_SHIFT,
			trace->id_width);
	AL_REG_MASK_SET_VAL(val,
			PMDT_AXI_MON_TRACE_MATRIX_TRC_SEL0_INCL_DATA,
			trace->incl_data);
	AL_REG_MASK_SET_VAL(val,
			PMDT_AXI_MON_TRACE_MATRIX_TRC_SEL0_INCL_ATTR,
			trace->incl_attr);
	AL_REG_FIELD_SET(val,
			PMDT_AXI_MON_TRACE_MATRIX_TRC_SEL0_TGTID_WIDTH_MASK,
			PMDT_AXI_MON_TRACE_MATRIX_TRC_SEL0_TGTID_WIDTH_SHIFT,
			trace->tgtid_width);
	AL_REG_FIELD_SET(val,
			PMDT_AXI_MON_TRACE_MATRIX_TRC_SEL0_ADDR_WIDTH_MASK,
			PMDT_AXI_MON_TRACE_MATRIX_TRC_SEL0_ADDR_WIDTH_SHIFT,
			trace->addr_width);
	al_reg_write32(&regs->trace_matrix[trace_id].trc_sel0, val);

	return 0;
}

/* Get current AXI Monitor trace configuration*/
void al_pmdt_axi_mon_trace_config_get(
		struct al_pmdt_axi_mon *pmdt_axi_mon,
		int trace_id,
		struct al_pmdt_axi_mon_trace *trace)
{
	struct al_pmdt_axi_mon_regs *regs;
	uint32_t val = 0;

	al_assert(pmdt_axi_mon);
	al_assert(trace_id < AL_PMDT_AXI_MON_TRACE_NUM);
	al_assert(trace);

	regs = (struct al_pmdt_axi_mon_regs *)pmdt_axi_mon->regs;

	val = al_reg_read32(&regs->trace_matrix[trace_id].trc_sel0);

	trace->tracker_sel = AL_REG_FIELD_GET(val,
			PMDT_AXI_MON_TRACE_MATRIX_TRC_SEL0_TRK_COND_MASK,
			PMDT_AXI_MON_TRACE_MATRIX_TRC_SEL0_TRK_COND_SHIFT);
	trace->user_width = AL_REG_FIELD_GET(val,
			PMDT_AXI_MON_TRACE_MATRIX_TRC_SEL0_USER_WIDTH_MASK,
			PMDT_AXI_MON_TRACE_MATRIX_TRC_SEL0_USER_WIDTH_SHIFT);
	trace->incl_busy = AL_REG_FIELD_BIT_GET(val,
			PMDT_AXI_MON_TRACE_MATRIX_TRC_SEL0_INCL_BUSY);
	trace->incl_bcount = AL_REG_FIELD_BIT_GET(val,
			PMDT_AXI_MON_TRACE_MATRIX_TRC_SEL0_INCL_BCNT);
	trace->id_width = AL_REG_FIELD_GET(val,
			PMDT_AXI_MON_TRACE_MATRIX_TRC_SEL0_ID_WIDTH_MASK,
			PMDT_AXI_MON_TRACE_MATRIX_TRC_SEL0_ID_WIDTH_SHIFT);
	trace->incl_data = AL_REG_FIELD_BIT_GET(val,
			PMDT_AXI_MON_TRACE_MATRIX_TRC_SEL0_INCL_DATA);
	trace->incl_attr = AL_REG_FIELD_BIT_GET(val,
			PMDT_AXI_MON_TRACE_MATRIX_TRC_SEL0_INCL_ATTR);
	trace->tgtid_width = AL_REG_FIELD_GET(val,
			PMDT_AXI_MON_TRACE_MATRIX_TRC_SEL0_TGTID_WIDTH_MASK,
			PMDT_AXI_MON_TRACE_MATRIX_TRC_SEL0_TGTID_WIDTH_SHIFT);
	trace->addr_width = AL_REG_FIELD_GET(val,
			PMDT_AXI_MON_TRACE_MATRIX_TRC_SEL0_ADDR_WIDTH_MASK,
			PMDT_AXI_MON_TRACE_MATRIX_TRC_SEL0_ADDR_WIDTH_SHIFT);
}

/**
 * Enable/disable AXI Monitor trace
 */
void al_pmdt_axi_mon_trace_enable(
		struct al_pmdt_axi_mon *pmdt_axi_mon,
		int trace_id,
		al_bool is_enable)
{
	struct al_pmdt_axi_mon_regs *regs;
	uint32_t val;

	al_assert(pmdt_axi_mon);

	regs = (struct al_pmdt_axi_mon_regs *)pmdt_axi_mon->regs;

	val = al_reg_read32(&regs->trace_matrix[trace_id].trc_sel0);
	AL_REG_MASK_SET_VAL(val,
			PMDT_AXI_MON_TRACE_MATRIX_TRC_SEL0_ENABLE,
			is_enable);
	al_reg_write32(&regs->trace_matrix[trace_id].trc_sel0, val);
}


/** @} end of PMDT AXI MON group */
