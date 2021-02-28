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
#include "al_hal_pmdt_pmu.h"
#include "al_hal_pmdt_axi_mon.h"
#include "al_hal_pmdt_ela_config.h"
#include "al_hal_pmdt_ela.h"
#include "al_hal_pmdt_cctm.h"
#include "al_pmdt.h"
#include "al_hal_iofic.h"
#include "al_hal_iofic_regs.h"
#include "al_hal_iomap.h"
#include "al_pmdt_config.h"

#define AL_PMDT_INT_GROUP_A 0
#define AL_PMDT_INT_GROUP_B 1

static int al_ela_retrieve_memory(
		struct al_pmdt_ela *ela,
		struct al_pmdt_ela_ram_entry *trace_info,
		uint32_t *num_of_entries_to_read);

int al_pmdt_monitor(
		struct al_pmdt_handle *pmdt,
		map_func_cb_type map,
		al_bool enable_interrupt,
		struct al_pmdt_special_setting *setting)
{
	int rc;

	al_assert(pmdt);

	/* TODO - add support for already initialized system (e.g. after reset)
	 * check cctm for activated pmus (al_pmdt_cctm_pmu_conf_get) if it's
	 * already activated, just initialize complex handles.
	 */
	al_memset(pmdt, 0, sizeof(struct al_pmdt_handle));
	rc = al_pmdt_start(pmdt, AL_TRUE, map, enable_interrupt, setting);
	al_assert(!rc);

	al_info("pmdt monitor ready\n");

	return 0;
}

static void al_pmdt_pmu_causes_get_and_clear(
		struct al_pmdt_pmu *pmu,
		uint32_t *pmu_cause_a,
		uint32_t *pmu_cause_b)
{
	void __iomem *pmu_iofic_regs;

	al_assert(pmu);
	al_assert(pmu_cause_a);
	al_assert(pmu_cause_a);

	pmu_iofic_regs = al_pmdt_pmu_iofic_base_get(pmu);

	*pmu_cause_a = al_iofic_read_cause(pmu_iofic_regs, AL_PMDT_INT_GROUP_A);
	*pmu_cause_b = al_iofic_read_cause(pmu_iofic_regs, AL_PMDT_INT_GROUP_B);

	al_iofic_clear_cause(pmu_iofic_regs, AL_PMDT_INT_GROUP_A, *pmu_cause_a);
	al_iofic_clear_cause(pmu_iofic_regs, AL_PMDT_INT_GROUP_B, *pmu_cause_b);

}
/* Retrieve all information logged in ELA memory */
static int al_pmdt_ela_err_info_get(
		struct al_pmdt_complex_handle	*cmplx,
		struct al_pmdt_isr_info *info)
{
	struct al_pmdt_ela_ram_entry	*trace_info;
	unsigned int num_entries_to_read = 0;
	int rc;

	al_assert(cmplx);
	al_assert(info);

	info->err_info.wentries_num = -1;
	info->err_info.rentries_num = -1;


	if (cmplx->ela_active_mask & AL_PMDT_INSTANCE_0) {
		trace_info = (cmplx->unit_config.default_axi_read ?
						info->err_info.read_trace_info :
						info->err_info.write_trace_info);

		rc = al_ela_retrieve_memory(
				&cmplx->ela[0], trace_info, &num_entries_to_read);
		if (rc)
			return -1;

		if (cmplx->unit_config.default_axi_read)
			info->err_info.rentries_num = num_entries_to_read;
		else
			info->err_info.wentries_num = num_entries_to_read;
	}

	if (cmplx->ela_active_mask & AL_PMDT_INSTANCE_1) {
		trace_info = (cmplx->unit_config.default_axi_read ?
				info->err_info.write_trace_info :
				info->err_info.read_trace_info);

		/* Get ELA unit information */
		rc = al_ela_retrieve_memory(
				&cmplx->ela[1], trace_info, &num_entries_to_read);
		if (rc)
			return -1;

		if (cmplx->unit_config.default_axi_read)
			info->err_info.wentries_num = num_entries_to_read;
		else
			info->err_info.rentries_num = num_entries_to_read;
	}

	return 0;
}

void al_pmdt_unit_name_print(enum al_pmdt_unit pmdt_unit)
{
	struct al_pmdt_map_entry entry;
	int rc;

	al_assert(pmdt_unit < AL_PMDT_UNIT_MAX);

	rc = al_pmdt_map_entry_get(0, pmdt_unit, NULL, &entry);
	if (!rc)
		al_info("%s", entry.name);
}

/* Retrieve AXI monitor timeout logged information */
static void al_pmdt_axi_mon_tout_info_get(
		struct al_pmdt_complex_handle *cmplx,
		struct al_pmdt_isr_info *info,
		uint8_t axi_mon)
{
	al_assert(cmplx);
	al_assert(info);

	al_assert(axi_mon < AL_PMDT_AXI_MON_MAX_NUM);

	al_pmdt_axi_mon_log_get(
		&cmplx->axi_mon[axi_mon], NULL, NULL,
		(cmplx->unit_config.default_axi_read ?
			&info->err_info.read_timeout_info :
			&info->err_info.write_timeout_info));

	if (cmplx->unit_config.default_axi_read == AL_TRUE)
		info->err_info.read_timeout_info_valid = AL_TRUE;
	else
		info->err_info.write_timeout_info_valid = AL_TRUE;
}

/* Retrieve AXI monitor error information*/
static void al_pmdt_axi_err_info_get(
		struct al_pmdt_complex_handle	*cmplx,
		struct al_pmdt_pmu_cmplx_info	*cmplx_info,
		struct al_pmdt_isr_info *info,
		uint32_t cause)
{
	uint32_t tout_err_mask;
	uint32_t ovf_err_mask;
	uint32_t unf_err_mask;

	uint8_t axi_mon_mask = (cmplx_info->axi_mon_num == 2) ?
			(AL_PMDT_INSTANCE_0 | AL_PMDT_INSTANCE_1) :
			 AL_PMDT_INSTANCE_0;

	tout_err_mask = axi_mon_mask;
	if (cause & tout_err_mask) {
		/* axi timeout occurred */
		info->err |= AL_PMDT_ISR_AXI_TIME_OUT;
		if (cause & AL_PMDT_INSTANCE_0)
			al_pmdt_axi_mon_tout_info_get(cmplx, info, 0);

		if (cause & AL_PMDT_INSTANCE_1)
			al_pmdt_axi_mon_tout_info_get(cmplx, info, 1);
	}

	ovf_err_mask = (cmplx_info->axi_mon_num == 2) ?
			(axi_mon_mask << 2) : (axi_mon_mask << 1);
	if (cause & ovf_err_mask)
		/* axi overflow occurred */
		info->err |= AL_PMDT_ISR_AXI_OVERFLOW;

	unf_err_mask = (cmplx_info->axi_mon_num == 2) ?
			(axi_mon_mask << 4) : (axi_mon_mask << 2);
	if (cause & unf_err_mask)
		/* axi underflow occurred */
		info->err |= AL_PMDT_ISR_AXI_UNDERFLOW;
}

int al_pmdt_isr(
	struct al_pmdt_handle *pmdt,
	struct al_pmdt_isr_info *info)
{
	void __iomem *cctm_iofic_regs;
	struct al_pmdt_complex_handle *cmplx;
	struct al_pmdt_pmu_cmplx_info cmplx_info;
	struct al_pmdt_pmu_event_status status;
	struct al_pmdt_pmu_event_counters counters;
	uint32_t cctm_grp_cause, pmu_cause_a, pmu_cause_b, cctm_grp_mask;
	uint32_t event_clear_mask;
	uint8_t	detected_event;
	unsigned int grp;
	int rc = 0;
	int i;

	al_assert(pmdt);
	al_assert(info);

	cctm_iofic_regs = al_pmdt_cctm_iofic_base_get(&pmdt->cctm);

	for (grp = 0; grp < AL_PMDT_CCTM_MATRIX_LINE_SIZE; grp++) {
		cctm_grp_cause = al_iofic_read_cause(cctm_iofic_regs, grp);
		cctm_grp_mask = al_iofic_read_mask(cctm_iofic_regs, grp);
		cctm_grp_cause &= ~cctm_grp_mask;
		if (cctm_grp_cause) {
			/* isolate lsb bit from cctm_cause */
			cctm_grp_cause = AL_BIT(AL_SHIFT_OF_MASK(cctm_grp_cause));
			break;
		}
	}

	if (grp == AL_PMDT_CCTM_MATRIX_LINE_SIZE) { /* no pending interrupts */
		info->err = AL_PMDT_ISR_NO_ERROR;
		return 0;
	}

	/* search for the relevant pmdt complex */
	for (i = 0; i < AL_PMDT_UNIT_MAX; i++) {
		cmplx = &pmdt->cmplx[i];
		if (cmplx->ready == AL_FALSE)
			continue;

		if ((cmplx->cctm_grp == grp) &&
				(AL_BIT(cmplx->cctm_pmu_bit) & cctm_grp_cause))
			break;
	}

	/* got an interrupt that wasn't enabled */
	if (i == AL_PMDT_UNIT_MAX)
		return -ENODEV;

	/* retrieve complex information */
	rc = al_pmdt_pmu_cmplx_info_get(&cmplx->pmu, &cmplx_info);
	al_assert(!rc);

	/* retrieve and clear pmu iofic causes*/
	al_pmdt_pmu_causes_get_and_clear(&cmplx->pmu, &pmu_cause_a, &pmu_cause_b);
	if ((pmu_cause_a == 0) && (pmu_cause_b == 0)) {
		al_err("No active IRQ in Complex\n");
		return -1;
	}

	/* retrieve and clear cctm causes*/
	cctm_grp_cause = al_iofic_read_cause(cctm_iofic_regs, grp);
	al_iofic_clear_cause(cctm_iofic_regs, grp, cctm_grp_cause);

	/* extract error info */
	al_memset(info, 0, sizeof(struct al_pmdt_isr_info));
	info->pmdt_unit = i;

	if (pmu_cause_a) {
		/* Detect counter number which caused interrupt */
		detected_event = AL_SHIFT_OF_MASK(pmu_cause_a);
		event_clear_mask = AL_BIT(detected_event);

		if ((detected_event & cmplx->events_map.aximon_track_event_num[0]) ||
			(detected_event & cmplx->events_map.aximon_track_event_num[1])) {
			/* Retrieve status of counter that cause to interrupt */
			rc = al_pmdt_pmu_event_status_get(
					&cmplx->pmu, detected_event, &status);
			al_assert(!rc);
			rc = al_pmdt_pmu_event_counters_get(
					&cmplx->pmu, detected_event, &counters);
			al_assert(!rc);

			info->err_info.track_err_info.counter_value =
					counters.last_event_cnt;

			if (detected_event & cmplx->events_map.aximon_track_event_num[0])
				info->err_info.track_err_info.is_read =
						al_pmdt_axi_mon_is_read(&cmplx->axi_mon[0]);
			else
				info->err_info.track_err_info.is_read =
						al_pmdt_axi_mon_is_read(&cmplx->axi_mon[1]);

			if (status.ovf_violation) {
				/* Upper bound crossing occurred */
				info->err |= AL_PMDT_ISR_AXI_TRACK_UPPER_BOUND_CROSS;
			}
			if (status.unf_violation) {
				/* Lower bound crossing occurred */
				info->err |= AL_PMDT_ISR_AXI_TRACK_LOWER_BOUND_CROSS;
			}
		}

		if ((detected_event & cmplx->events_map.aximon_trap_event_num[0]) ||
				(detected_event & cmplx->events_map.aximon_trap_event_num[1]))
			/* slv/dec error occurred */
			info->err |= AL_PMDT_ISR_AXI_TRANS_ERROR;

		/* Clear event by performing disable/enable action */
		al_pmdt_pmu_events_enable(&cmplx->pmu, AL_FALSE, &event_clear_mask);

		event_clear_mask |= AL_BIT(AL_SHIFT_OF_MASK(pmu_cause_a));
		al_pmdt_pmu_events_enable(&cmplx->pmu, AL_TRUE, &event_clear_mask);
	}

	if (pmu_cause_b)
		al_pmdt_axi_err_info_get(cmplx, &cmplx_info, info, pmu_cause_b);

	/* Retrieve ELA memory */
	al_pmdt_ela_err_info_get(cmplx, info);

	return 0;
}

static int al_latency_histogram_get_aux(
	struct al_pmdt_handle *pmdt,
	enum al_pmdt_unit pmdt_unit,
	unsigned int *bin_num,
	uint64_t (*read_hist)[AL_PMDT_AXI_MON_BAR_NUM],
	uint64_t (*write_hist)[AL_PMDT_AXI_MON_BAR_NUM])
{
	struct al_pmdt_pmu_event_status status;
	struct al_pmdt_config *config;
	struct al_pmdt_complex_handle *cmplx;
	int read_event_start, write_event_start;
	al_bool collect_read_hist = AL_FALSE;
	al_bool collect_write_hist = AL_FALSE;
	int bins;
	int i, idx;
	int rc;

	cmplx = &pmdt->cmplx[pmdt_unit];
	bins = cmplx->event_bars_number;
	config = &cmplx->unit_config;
	*bin_num = bins;

	if (config->default_axi_read) {
			read_event_start = 0;
			write_event_start = bins;
	} else {
			read_event_start = bins;
			write_event_start = 0;
	}

	if (config->latency_config.direction == AL_PMDT_MONITOR_READ_WRITE_DIR) {
		collect_read_hist = AL_TRUE;
		collect_write_hist = AL_TRUE;
	} else {
		if (config->latency_config.direction == AL_PMDT_MONITOR_WRITE_DIR)
			collect_write_hist = AL_TRUE;
		else if (config->latency_config.direction == AL_PMDT_MONITOR_READ_DIR)
			collect_read_hist = AL_TRUE;
	}

	if (collect_read_hist) {
		idx = 0;
		for (i = read_event_start; i < read_event_start + bins; i++) {
			rc = al_pmdt_pmu_event_counters_get(
					&cmplx->pmu, i, &status.counters);
			if (rc < 0)
				return rc;

			(*read_hist)[idx++] = status.counters.cur_event_cnt;
		}
	}

	if (collect_write_hist) {
		idx = 0;
		for (i = write_event_start; i < write_event_start + bins; i++) {
			rc = al_pmdt_pmu_event_counters_get(
					&cmplx->pmu, i, &status.counters);
			if (rc < 0)
				return rc;

			(*write_hist)[idx++] = status.counters.cur_event_cnt;
		}
	}

	for (i = 12; i < 16; i++) {
		rc = al_pmdt_pmu_event_counters_get(
				&cmplx->pmu, i, &status.counters);
	}

	return 0;
}

int al_pmdt_byte_count_get(
	struct al_pmdt_handle *pmdt,
	enum al_pmdt_unit pmdt_unit,
	uint64_t *count)
{
	struct al_pmdt_pmu_event_status status;
	struct al_pmdt_complex_handle *cmplx;
	int i, event;
	int rc;

	al_assert(pmdt);
	al_assert(count);

	cmplx = &pmdt->cmplx[pmdt_unit];
	*count = 0;
	for (i = 0; i < AL_PMDT_AXI_MON_MAX_NUM; i++) {
		event = cmplx->events_map.aximon_track_event_num[i];
		if (event != -1) {
			rc = al_pmdt_pmu_event_counters_get(&cmplx->pmu, event, &status.counters);
			if (rc)
				return -1;
			*count += status.counters.cur_event_cnt;
		}
	}

	return 0;
}

int al_pmdt_info_print(
		struct al_pmdt_handle *pmdt,
		enum al_pmdt_unit pmdt_unit)
{
	struct al_pmdt_pmu_cmplx_info info;
	struct al_pmdt_pmu_report report;
	struct al_pmdt_axi_mon_log_entry last[AL_PMDT_AXI_MON_BAR_NUM];
	struct al_pmdt_axi_mon_log_entry max;
	struct al_pmdt_axi_mon_log_entry timeout;
	int i;
	int rc;

	al_assert(pmdt);
	rc = al_pmdt_pmu_cmplx_info_get(&pmdt->cmplx[pmdt_unit].pmu, &info);

	al_info("info:\nrev %d\nela_num %d\naxi_mon_num %d\ntrk_num %d\n"
			"trc_num %d\nevents_num %d\nactive_events_num %d\n"
			"active_events 0x%x\n",
			info.rev, info.ela_num, info.axi_mon_num, info.trk_num,
			info.trc_num, info.events_num, info.active_events_num,
			info.active_events);

	rc = al_pmdt_pmu_report_get(&pmdt->cmplx[pmdt_unit].pmu, &report);

	al_info("report:\novf_violation %d\nunf_violation %d\nevent_num %d\n"
			"violation_cnt %d\nreport_timestamp "
			"%" PRId64 "\nwindow_cnt %" PRId64 "\n",
			report.ovf_violation, report.unf_violation, report.event_num,
			report.violation_cnt, report.report_timestamp, report.window_cnt);

	al_pmdt_axi_mon_log_get(
			&pmdt->cmplx[pmdt_unit].axi_mon[0], &last, &max, &timeout);

	for (i = 0; i < AL_PMDT_AXI_MON_BAR_NUM; i++) {
		al_info("last[%d], latency %" PRId64 ", address 0x%" PRIx64
				", attr 0x%" PRIx64 "\n",
				i, last[i].latency, last[i].address, last[i].attr);
	}
	al_info("max, latency %" PRId64 ", address 0x%" PRIx64 ", attr 0x%"
			PRIx64 "\n", max.latency, max.address, max.attr);
	al_info("timeout, latency %" PRId64 ", address 0x%" PRIx64
			", attr 0x%" PRIx64 "\n",
			timeout.latency, timeout.address, timeout.attr);

	al_pmdt_axi_mon_log_get(
			&pmdt->cmplx[pmdt_unit].axi_mon[1], &last, &max, &timeout);

	for (i = 0; i < AL_PMDT_AXI_MON_BAR_NUM; i++) {
		al_info("last[%d], latency %" PRId64 ", address 0x%" PRIx64
				", attr 0x%" PRIx64 "\n",
				i, last[i].latency, last[i].address, last[i].attr);
	}
	al_info("max, latency %" PRId64 ", address 0x%" PRIx64 ", attr 0x%"
			PRIx64 "\n", max.latency, max.address, max.attr);
	al_info("timeout, latency %" PRId64 ", address 0x%" PRIx64
			", attr 0x%" PRIx64"\n",
			timeout.latency, timeout.address, timeout.attr);

	return rc;
}

int al_latency_histograms_get(
	struct al_pmdt_handle *pmdt,
	enum al_pmdt_unit pmdt_unit,
	unsigned int *bin_num,
	uint64_t (*read_hist)[AL_PMDT_AXI_MON_BAR_NUM],
	uint64_t (*write_hist)[AL_PMDT_AXI_MON_BAR_NUM])
{
	al_assert(pmdt);
	al_assert(pmdt_unit < AL_PMDT_UNIT_MAX);
	al_assert(bin_num);
	al_assert(read_hist);
	al_assert(write_hist);

	return al_latency_histogram_get_aux(
			pmdt, pmdt_unit, bin_num, read_hist, write_hist);
}

int al_latency_histograms_print(
		struct al_pmdt_handle *pmdt,
		enum al_pmdt_unit pmdt_unit)
{
	unsigned int bin_num;
	int rc = 0;
	unsigned int i;
	uint64_t read_hist[AL_PMDT_AXI_MON_BAR_NUM];
	uint64_t write_hist[AL_PMDT_AXI_MON_BAR_NUM];
	uint64_t last_bin;
	struct al_pmdt_config *unit_config;
	struct al_pmdt_pmu_cmplx_info cmplx_info;
	al_bool dump_read_hist = AL_FALSE;
	al_bool dump_write_hist = AL_FALSE;

	al_assert(pmdt);
	al_assert(pmdt_unit < AL_PMDT_UNIT_MAX);

	if (pmdt->cmplx[pmdt_unit].ready == AL_FALSE) {
		al_err("%s unit does not initialized\n", pmdt->cmplx[pmdt_unit].name);
		return -1;
	}
	rc = al_pmdt_pmu_cmplx_info_get(&pmdt->cmplx[pmdt_unit].pmu, &cmplx_info);
	if (rc)
		return -1;

	if (cmplx_info.axi_mon_num == 0) {
		al_err("No latency capability for unit %s\n",
				pmdt->cmplx[pmdt_unit].name);
		return -1;
	}
	rc = al_latency_histogram_get_aux(
		pmdt, pmdt_unit, &bin_num, &read_hist, &write_hist);
	if (rc)
		return -1;

	unit_config = &pmdt->cmplx[pmdt_unit].unit_config;
	if (unit_config->latency_config.direction ==
			AL_PMDT_MONITOR_READ_WRITE_DIR) {
		dump_read_hist = AL_TRUE;
		dump_write_hist = AL_TRUE;
	} else {
		if (unit_config->latency_config.direction ==
				AL_PMDT_MONITOR_WRITE_DIR)
			dump_write_hist = AL_TRUE;
		else if (unit_config->latency_config.direction ==
				AL_PMDT_MONITOR_READ_DIR)
			dump_read_hist = AL_TRUE;
	}

	if (dump_read_hist) {
		al_info("%s READ_HISTOGRAM "
				"==========================================\n",
			pmdt->cmplx[pmdt_unit].name);
		last_bin = 0;
		for (i = 0; i < bin_num-1; i++) {
			al_info("\tbin_%d [%" PRId64 "-%" PRId64 "]\t = %" PRId64 "\n",
			 i,
			 last_bin,
			 pmdt->cmplx[pmdt_unit].unit_config.latency_config.read_bar_thr[i],
			 read_hist[i]);
			last_bin =
			 pmdt->cmplx[pmdt_unit].unit_config.latency_config.read_bar_thr[i];
		}
		al_info("\tbin_%d [>%" PRId64 "]\t = %" PRId64 "\n",
				i,
				last_bin,
				read_hist[i]);
	}
	if (dump_write_hist) {
		al_info("%s WRITE_HISTOGRAM "
				"==========================================\n",
			pmdt->cmplx[pmdt_unit].name);
		last_bin = 0;
		for (i = 0; i < bin_num-1; i++) {
			al_info("\tbin_%d [%" PRId64 "-%" PRId64 "]\t= %" PRId64 "\n",
			 i,
			 last_bin,
			 pmdt->cmplx[pmdt_unit].unit_config.latency_config.write_bar_thr[i],
			 write_hist[i]);
			last_bin =
			 pmdt->cmplx[pmdt_unit].unit_config.latency_config.write_bar_thr[i];
		}
		al_info("\tbin_%d [>%" PRId64 "]\t= %" PRId64 "\n",
				i,
				last_bin,
				write_hist[i]);
	}
	return 0;
}

int al_ela_history_print(
	struct al_pmdt_handle *pmdt,
	enum al_pmdt_unit pmdt_unit,
	unsigned int ela_instance,
	struct al_pmdt_ela_ram_entry *ela_ram_in,
	unsigned int num_of_entries,
	enum al_pmdt_ela_memory_dump_resolution dump_res,
	al_bool print_parsed_data)
{
	unsigned int ela_entry, ela_entry_start_idx;
	int rc;
	uint32_t num_entries_to_read;

	struct al_pmdt_axi_mon_trace trace_cfg;
	struct al_pmdt_complex_handle *cmplx;
	struct al_pmdt_ela_ram_entry ela_ram_loc[AL_PMDT_ELA_RAM_MAX_ENTRIES];
	struct al_pmdt_ela_ram_entry *ela_ram_ptr;
	struct al_pmdt_axi_trace_entry_data entry_data;
	struct al_pmdt_ela_dev_info ela_info;

	al_assert(pmdt);
	al_assert(pmdt_unit < AL_PMDT_UNIT_MAX);
	al_assert(ela_instance < AL_PMDT_ELA_MAX_NUM);

	cmplx = &pmdt->cmplx[pmdt_unit];

	/* Sanity check on num_of_entries */
	al_pmdt_ela_device_info_get(&cmplx->ela[ela_instance], &ela_info);
	al_assert(num_of_entries <= AL_BIT(ela_info.ram_addr_size));

	/* If ela memory still didn't retrieved*/
	if (ela_ram_in == NULL) {
		rc = al_ela_retrieve_memory(
				&cmplx->ela[ela_instance], ela_ram_loc,
				&num_entries_to_read);
		al_assert(!rc);
	} else
		num_entries_to_read = num_of_entries;

	if (print_parsed_data)
		al_pmdt_axi_mon_trace_config_get(
				&cmplx->axi_mon[ela_instance], 0, &trace_cfg);

	ela_ram_ptr = ela_ram_in ? ela_ram_in : ela_ram_loc;

	switch (dump_res) {
	case AL_PMDT_DUMP_LAST_1:
		ela_entry_start_idx = num_entries_to_read - 1;
		break;
	case AL_PMDT_DUMP_LAST_5:
		ela_entry_start_idx = (num_entries_to_read > 5) ?
				(num_entries_to_read - 5) : 0;
		break;
	case AL_PMDT_DUMP_LAST_10:
		ela_entry_start_idx = (num_entries_to_read > 10) ?
				(num_entries_to_read - 10) : 0;
		break;
	case AL_PMDT_DUMP_LAST_QUATER:
		ela_entry_start_idx =
				num_entries_to_read - (num_entries_to_read >> 2);
		break;
	case AL_PMDT_DUMP_LAST_HALF:
		ela_entry_start_idx =
				num_entries_to_read - (num_entries_to_read >> 1);
		break;
	case AL_PMDT_DUMP_ALL:
	default:
		ela_entry_start_idx = 0;
		break;
	}
	for (ela_entry = ela_entry_start_idx;
			ela_entry < num_entries_to_read; ela_entry++) {
		if (print_parsed_data) {
			al_pmdt_ela_entry_parser(
					&trace_cfg, &ela_ram_ptr[ela_entry],
					&entry_data);
			if (entry_data.is_ts)
				al_info("[%d]: =====================\n"
						"\ttimestamp = %" PRId64 "\n",
						ela_entry, entry_data.timestamp[0]);
			else
				al_info("[%d]: =====================\n"
					"\tid = %d\n"
					"\taddress		= 0x%x\n"
					"\ttgtid_addr	= 0x%x\n"
					"\tattributes	= 0x%x\n"
					"\taxuser		= 0x%x\n"
					"\tdata			= 0x%x\n"
					"\tbyte count	= %dBytes\n"
					"\toutstandings	= 0x%x\n"
					"\ttrack_hit	= 0x%x\n",
					ela_entry,
					entry_data.id, entry_data.addr,
					entry_data.tgtid_addr, entry_data.attr,
					entry_data.axuser, entry_data.data,
					entry_data.bcount, entry_data.outstand,
					entry_data.track_hit);
		} else {
			al_info("[%d]: =====================\n"
				"\tpayload	= %" PRIx64 "%"PRIx64"\n"
				"\tst4_ovrd	= %d\n"
				"\ttrig_state_num = %d\n"
				"\tts		= %d\n"
				"\ttcsel1	= %d\n"
				"\ttcsel0	= %d\n",
				ela_entry,
				ela_ram_ptr[ela_entry].payload_1,
				ela_ram_ptr[ela_entry].payload_0,
				ela_ram_ptr[ela_entry].st4_ovrd,
				ela_ram_ptr[ela_entry].trig_state_num,
				ela_ram_ptr[ela_entry].ts,
				ela_ram_ptr[ela_entry].tcsel1,
				ela_ram_ptr[ela_entry].tcsel0
				);
		}
	}
	return 0;
}

static int al_ela_retrieve_memory(
	struct al_pmdt_ela *ela,
	struct al_pmdt_ela_ram_entry *trace_info,
	uint32_t *entries_num)
{
	struct al_pmdt_ela_dev_info ela_info;
	unsigned int num_entries_to_read = 0;
	unsigned int ela_wr_ptr = 0;
	al_bool ela_wr_ptr_is_wrap;
	int rc = 0;
	unsigned int ela_entry;

	al_assert(ela);
	al_assert(trace_info);

	al_pmdt_ela_device_info_get(ela, &ela_info);
	/* unlock & stop ela */
	al_pmdt_ela_write_lock(ela, AL_FALSE);
	rc = al_pmdt_ela_start(ela, AL_FALSE);
	if (rc)
		return -1;
	/* Get current ELA write pointer */
	rc = al_pmdt_ela_ram_write_ptr_get(
			ela,
			&ela_wr_ptr,
			&ela_wr_ptr_is_wrap);
	if (rc)
		return -1;

	/* Calculate number of entries to be read and set read starting pointer
	 * in depending if ELA memory wrap around occurred or not*/
	if (ela_wr_ptr_is_wrap) {
		num_entries_to_read = AL_BIT(ela_info.ram_addr_size);
	} else {
		num_entries_to_read = ela_wr_ptr;
		ela_wr_ptr = 0;
	}

	/* Retrieve ELA entries */
	for (ela_entry = 0; ela_entry < num_entries_to_read; ela_entry++) {
		rc = al_pmdt_ela_ram_read(
				ela,
				AL_FALSE,
				((ela_wr_ptr + ela_entry) %
					AL_BIT(ela_info.ram_addr_size)),
				&trace_info[ela_entry]);
		if (rc)
			return -1;
	}

	*entries_num = num_entries_to_read;

	/* lock & start ela */
	rc = al_pmdt_ela_start(ela, AL_TRUE);
	if (rc)
		return -1;

	al_pmdt_ela_write_lock(ela, AL_TRUE);

	return 0;
}
