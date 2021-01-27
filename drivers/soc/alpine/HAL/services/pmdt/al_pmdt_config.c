#include "al_pmdt.h"
#include "al_pmdt_config.h"
#include "al_hal_pmdt_map.h"
#include "al_hal_anpa_regs.h"
#include "al_hal_udma_iofic.h"
#include "al_hal_iofic_regs.h"
#include "al_hal_iomap.h"

#define AL_PMDT_AXI_SLAVE_ERROR 0x2
#define AL_PMDT_AXI_SLAVE_ERROR_MASK 0x3
#define AL_PMDT_AXI_DEC_ERROR	0x3
#define AL_PMDT_AXI_DEC_ERROR_MASK 0x3
#define AL_PMDT_AXI_SLV_DEC_ERROR 0x2
#define AL_PMDT_AXI_SLV_DEC_ERROR_MASK 0x2

#define AL_PMDT_AXI_TRANS_VALID_WORD 4
#define AL_PMDT_AXI_TRANS_VALID_BIT 31
#define AL_PMDT_AXI_TRACE_VALID_WORD 3
#define AL_PMDT_AXI_TRACE_VALID_BIT 31
#define AL_PMDT_INFINITE_OVF_THR AL_BIT_64(63)
#define AL_PMDT_INT_GROUP_A AL_INT_GROUP_A
#define AL_PMDT_INT_GROUP_B AL_INT_GROUP_B

#define AL_PMDT_AXI_TRANS_ID_WORD 2
#define AL_PMDT_AXI_RD_M2S_DESC_ID 0x0
#define AL_PMDT_AXI_RD_M2S_DESC_ID_MASK 0x1
#define AL_PMDT_AXI_RD_DATA_ID 0x1
#define AL_PMDT_AXI_RD_DATA_ID_MASK 0x1
#define AL_PMDT_AXI_RD_S2M_DESC_ID 0x2
#define AL_PMDT_AXI_RD_S2M_DESC_ID_MASK 0x2

#define AL_PMDT_AXI_WR_M2S_COMP_ID 0x0
#define AL_PMDT_AXI_WR_M2S_COMP_ID_MASK	0x1
#define AL_PMDT_AXI_WR_S2M_COMP_ID 0x2
#define AL_PMDT_AXI_WR_S2M_COMP_ID_MASK	0x2
#define AL_PMDT_AXI_WR_DATA_ID 0x3
#define AL_PMDT_AXI_WR_DATA_ID_MASK 0x3
#define AL_PMDT_AXI_WR_MSIX_ID 0x10
#define AL_PMDT_AXI_WR_MSIX_ID_MASK	0x10

#define AL_PMDT_FEATURES_MASK \
		(AL_PMDT_FEATURE_AXI_TIMEOUT | \
		AL_PMDT_FEATURE_AXI_LATENCY_HISTOGRAM | \
		AL_PMDT_FEATURE_AXI_TRAP | \
		AL_PMDT_FEATURE_AXI_TRACK | \
		AL_PMDT_FEATURE_EXT_TRIG_TRACK)

#define AL_PMDT_AXI_TRACE_FIELD_MASK(bits) (AL_BIT_64(bits) - 1)

#define AL_ANPA_CPU_CLUSTER_DBG_PMU_AXI_TRACK_EN	AL_BIT(2)

static al_bool al_pmdt_is_unit_active(
		struct al_pmdt_handle *pmdt,
		enum al_pmdt_unit id)
{
	if (id >= AL_PMDT_UNIT_MAX)
		return AL_FALSE;

	return AL_REG_BIT_GET(pmdt->active_units_mask[id/32], id%32) ?
			AL_TRUE : AL_FALSE;
}

static void al_pmdt_cpu_clust_tracking_enable(unsigned int cluster_id)
{
	struct al_anpa_regs *anpa_cpu_regs;

	al_assert(cluster_id < AL_NB_ANPA_NUM_CLUSTERS);

	anpa_cpu_regs = (struct al_anpa_regs *)(uintptr_t)AL_NB_ANPA_BASE(cluster_id);

	/* Enable AXI tracking in CPU Cluster */
	al_reg_write32_masked(&anpa_cpu_regs->cluster_debug.pmu,
			AL_ANPA_CPU_CLUSTER_DBG_PMU_AXI_TRACK_EN,
			AL_ANPA_CPU_CLUSTER_DBG_PMU_AXI_TRACK_EN);

	/* Make sure that write finished before send Read for tracking */
	al_reg_read32(&anpa_cpu_regs->cluster_debug.pmu);
}

static unsigned int calc_req_num_of_axi_mon(struct al_pmdt_config *config)
{
	unsigned int max_axi_mon_num = 0;
	unsigned int temp_axi_mon_num;

	/* If we are going to count external events ,
	 * not inside COMPLEX, no need in AXI_MON initialization */
	if (((config->feature_mask & AL_PMDT_FEATURES_MASK) ==
			AL_PMDT_FEATURE_EXT_TRIG_TRACK) ||
			(config->feature_mask == 0))
		temp_axi_mon_num = 0;

	if (config->feature_mask & AL_PMDT_FEATURE_AXI_TIMEOUT) {
		if (config->timeout_config.direction == AL_PMDT_MONITOR_READ_WRITE_DIR)
			temp_axi_mon_num = AL_PMDT_AXI_MON_MAX_NUM;
		else
			temp_axi_mon_num = AL_PMDT_AXI_MON_MAX_NUM - 1;

		max_axi_mon_num = max_axi_mon_num > temp_axi_mon_num ?
							max_axi_mon_num : temp_axi_mon_num;
	}

	if (config->feature_mask & AL_PMDT_FEATURE_AXI_LATENCY_HISTOGRAM) {
		if (config->latency_config.direction == AL_PMDT_MONITOR_READ_WRITE_DIR)
			temp_axi_mon_num = AL_PMDT_AXI_MON_MAX_NUM;
		else
			temp_axi_mon_num = AL_PMDT_AXI_MON_MAX_NUM - 1;

		max_axi_mon_num = max_axi_mon_num > temp_axi_mon_num ?
							max_axi_mon_num : temp_axi_mon_num;
	}

	if (config->feature_mask & AL_PMDT_FEATURE_AXI_TRAP) {
		if (config->trap_config.direction == AL_PMDT_MONITOR_READ_WRITE_DIR)
			temp_axi_mon_num = AL_PMDT_AXI_MON_MAX_NUM;
		else
			temp_axi_mon_num = AL_PMDT_AXI_MON_MAX_NUM - 1;

		max_axi_mon_num = max_axi_mon_num > temp_axi_mon_num ?
							max_axi_mon_num : temp_axi_mon_num;
	}

	if (config->feature_mask&AL_PMDT_FEATURE_AXI_TRACK) {
		if (config->track_config.direction == AL_PMDT_MONITOR_READ_WRITE_DIR)
			temp_axi_mon_num = AL_PMDT_AXI_MON_MAX_NUM;
		else
			temp_axi_mon_num = AL_PMDT_AXI_MON_MAX_NUM - 1;

		max_axi_mon_num = max_axi_mon_num > temp_axi_mon_num ?
							max_axi_mon_num : temp_axi_mon_num;
	}

	return max_axi_mon_num;
}

static unsigned int calc_req_num_of_ela(struct al_pmdt_config *config)
{
	unsigned int max_ela_num = 0;
	unsigned int temp_ela_num;

	if (config->feature_mask & AL_PMDT_FEATURE_AXI_TRAP) {
		if (config->trap_config.is_trace) {
			if (config->trap_config.direction ==
					AL_PMDT_MONITOR_READ_WRITE_DIR)
				temp_ela_num = AL_PMDT_ELA_MAX_NUM;
			else
				temp_ela_num = AL_PMDT_ELA_MAX_NUM - 1;

			max_ela_num = max_ela_num > temp_ela_num ?
								max_ela_num : temp_ela_num;
		}
	}
	if (config->feature_mask & AL_PMDT_FEATURE_AXI_TRACK) {
		if (config->track_config.stop_on_cross) {
			if (config->track_config.direction ==
					AL_PMDT_MONITOR_READ_WRITE_DIR)
				temp_ela_num = AL_PMDT_ELA_MAX_NUM;
			else
				temp_ela_num = AL_PMDT_ELA_MAX_NUM - 1;

			max_ela_num = max_ela_num > temp_ela_num ?
							max_ela_num : temp_ela_num;
		}
	}

	return max_ela_num;
}

static int al_pmdt_pmu_timeout_events_config(
	struct al_pmdt_complex_handle *cmplx,
	struct al_pmdt_pmu_cmplx_info *cmplx_info,
	int *event_num,
	uint32_t *event_mask)
{
	int trig_num, rc;
	int next_event_num = *event_num;
	enum al_pmdt_monitor_direction direction;
	struct al_pmdt_pmu_event_config event_config;
	unsigned int axi_mon_inst;
	unsigned int req_axi_mon_num = 1;

	direction = cmplx->unit_config.timeout_config.direction;

	if ((cmplx_info->axi_mon_num == AL_PMDT_AXI_MON_MAX_NUM) &&
			(direction == AL_PMDT_MONITOR_WRITE_DIR))
		axi_mon_inst = 1;
	else
		axi_mon_inst = 0;

	req_axi_mon_num =
			(direction == AL_PMDT_MONITOR_READ_WRITE_DIR) ? 2 : 1;

	cmplx->event_bars_number = 0;
	al_memset(&event_config, 0, sizeof(struct al_pmdt_pmu_event_config));
	event_config.window_en = AL_FALSE; /* accumulate histogram forever */
	event_config.pause_en = AL_FALSE;
	event_config.thr_low = 0;
	event_config.thr_high = AL_PMDT_INFINITE_OVF_THR;

	/* configure histogram event counters
	 * if number of bars is less than AL_PMDT_AXI_MON_BAR_NUM,
	 * first max_bars events (0,1,..,max_bars-1) will be configured*/
	trig_num = al_pmdt_map_trig_num_get(
			cmplx_info->axi_mon_num,
			axi_mon_inst,
			AL_PMDT_INT_TRIG_LATENCY,
			0);
	al_assert(trig_num >= 0);
	event_config.trig_num = (unsigned int)trig_num;
	rc = al_pmdt_pmu_event_config(
			&cmplx->pmu,
			next_event_num,
			&event_config);
	al_assert(!rc);
	*event_mask |= AL_BIT(next_event_num - 1); /* for event enablement */


	/* if latency histogram required for both AXI Monitors,
	 * configure and initialize events for second monitor too */
	if (req_axi_mon_num == 2) {
		/* configure histogram event counters
		 * if number of bars is less than AL_PMDT_AXI_MON_BAR_NUM,
		 * first max_bars events (0,1,..,max_bars-1) will be configured*/
		trig_num = al_pmdt_map_trig_num_get(
				cmplx_info->axi_mon_num,
				(axi_mon_inst + 1),
				AL_PMDT_INT_TRIG_LATENCY,
				0);
		al_assert(trig_num >= 0);
		event_config.trig_num = (unsigned int)trig_num;
		rc = al_pmdt_pmu_event_config(
				&cmplx->pmu,
				next_event_num++,
				&event_config);
		al_assert(!rc);
		*event_mask |= AL_BIT(next_event_num - 1); /* for event enablement */
	}

	*event_num = next_event_num;

	return 0;
}

static int al_pmdt_pmu_latency_events_config(
	struct al_pmdt_complex_handle *cmplx,
	struct al_pmdt_pmu_cmplx_info *cmplx_info,
	int *event_num, uint32_t *event_mask)
{
	int max_bars;
	int i;
	int trig_num, rc;
	int next_event_num = *event_num;

	struct al_pmdt_config *config = &cmplx->unit_config;
	struct al_pmdt_pmu_event_config event_config;

	unsigned int features = config->feature_mask;

	unsigned int axi_mon_inst = 0;
	al_bool cfg_events_for_aximon[AL_PMDT_AXI_MON_MAX_NUM] = {AL_FALSE};
	/* count number of events that should be reserved for other features needs*/
	uint8_t reserved_events_num = 0;

	if ((features & AL_PMDT_FEATURE_AXI_TRAP)) {
		if (config->trap_config.direction == AL_PMDT_MONITOR_READ_WRITE_DIR)
			reserved_events_num += 2;
		else
			reserved_events_num += 1;
	}

	if ((features & AL_PMDT_FEATURE_AXI_TRACK)) {
		if (config->track_config.direction == AL_PMDT_MONITOR_READ_WRITE_DIR)
			reserved_events_num += 2;
		else
			reserved_events_num += 1;
	}

	/* -1 defines that no events were configured for AXI mon latency feature */
	cmplx->events_map.aximon_latency_frst_event_num[0] = -1;
	cmplx->events_map.aximon_latency_frst_event_num[1] = -1;

	/* In some units number of events in PMU not enough
	 * to fill 2 monitors latency counters full requirements
	 * ( 2*AL_PMDT_AXI_MON_BAR_NUM),so max_bars may be
	 * less than AL_PMDT_AXI_MON_BAR_NUM. If no needs for latency tracking
	 * for one of monitors, other monitor will able to get full amount of
	 * required events
	 */
	switch (config->latency_config.direction) {
	case (AL_PMDT_MONITOR_READ_WRITE_DIR):
		max_bars = al_min_t(int,
			((cmplx_info->events_num - reserved_events_num) / 2),
			AL_PMDT_AXI_MON_BAR_NUM);

		cfg_events_for_aximon[0] = AL_TRUE;
		cfg_events_for_aximon[1] = AL_TRUE;
		cmplx->events_map.aximon_latency_frst_event_num[0] =
				next_event_num;
		cmplx->events_map.aximon_latency_frst_event_num[1] =
				next_event_num + max_bars;

		break;
	case (AL_PMDT_MONITOR_WRITE_DIR):
		max_bars = al_min_t(int,
			((cmplx_info->events_num - reserved_events_num) / 2),
			AL_PMDT_AXI_MON_BAR_NUM);

		axi_mon_inst = ((config->default_axi_read) &&
			(cmplx_info->axi_mon_num == AL_PMDT_AXI_MON_BAR_NUM)) ?
						1 : 0;

		cfg_events_for_aximon[axi_mon_inst] = AL_TRUE;
		cmplx->events_map.aximon_latency_frst_event_num[axi_mon_inst] =
				next_event_num;

		break;
	case (AL_PMDT_MONITOR_READ_DIR):
		max_bars = al_min_t(int,
			((cmplx_info->events_num - reserved_events_num) / 2),
			AL_PMDT_AXI_MON_BAR_NUM);

		axi_mon_inst = ((config->default_axi_read == AL_FALSE) &&
			(cmplx_info->axi_mon_num == AL_PMDT_AXI_MON_BAR_NUM)) ?
			1 : 0;

		cfg_events_for_aximon[axi_mon_inst] = AL_TRUE;
		cmplx->events_map.aximon_latency_frst_event_num[axi_mon_inst] =
				next_event_num;

		break;
	default:
		al_assert(0);
		return -EINVAL;
	}
	cmplx->event_bars_number = max_bars;
	al_memset(&event_config, 0, sizeof(struct al_pmdt_pmu_event_config));
	event_config.window_en = AL_FALSE; /* accumulate histogram forever */
	event_config.pause_en = AL_TRUE;
	event_config.thr_low = 0;
	event_config.thr_high = AL_PMDT_INFINITE_OVF_THR;
	event_config.thr_high = 0;

	/* configure histogram event counters
	 * if number of bars is less than AL_PMDT_AXI_MON_BAR_NUM,
	 * last max_bars events (0,1,..,max_bars-1) will be configured*/
	for (axi_mon_inst = 0; axi_mon_inst < cmplx_info->axi_mon_num;
			axi_mon_inst++) {
		if (cfg_events_for_aximon[axi_mon_inst] == AL_FALSE)
			continue;

		for (i = (AL_PMDT_AXI_MON_BAR_NUM - max_bars);
				i < AL_PMDT_AXI_MON_BAR_NUM; i++) {
			trig_num = al_pmdt_map_trig_num_get(
					cmplx_info->axi_mon_num,
					axi_mon_inst,
					AL_PMDT_INT_TRIG_LATENCY,
					i);
			al_assert(trig_num >= 0);
			event_config.trig_num = (unsigned int)trig_num;
			rc = al_pmdt_pmu_event_config(
					&cmplx->pmu,
					next_event_num++,
					&event_config);
			al_assert(!rc);

			*event_mask |= AL_BIT(next_event_num - 1); /*for event enablement*/
		}
	}

	*event_num = next_event_num;

	return 0;
}

static int al_pmdt_pmu_trap_events_config(
	struct al_pmdt_complex_handle *cmplx,
	struct al_pmdt_pmu_cmplx_info *cmplx_info,
	int *event_num, int *filter_num,
	uint32_t *event_mask,
	uint32_t *irq_mask)
{
	int trig_num, rc;
	struct al_pmdt_config *config;
	int next_event_num; /* next free to configuration event number */
	int next_filt_num; /* next free to configuration tracker number */
	struct al_pmdt_pmu_event_config event_config;
	unsigned int axi_mon_inst;
	al_bool cfg_events_for_aximon[AL_PMDT_AXI_MON_MAX_NUM] = {AL_FALSE};

	config = &cmplx->unit_config;
	next_event_num = *event_num;
	next_filt_num = *filter_num;

	/* -1 defines that no events were configured for AXI mon trapping feature */
	cmplx->events_map.aximon_trap_event_num[0] = -1;
	cmplx->events_map.aximon_trap_event_num[1] = -1;

	switch (config->trap_config.direction) {
	case (AL_PMDT_MONITOR_READ_WRITE_DIR):
		cfg_events_for_aximon[0] = AL_TRUE;
		cfg_events_for_aximon[1] = AL_TRUE;
		cmplx->events_map.aximon_trap_event_num[0] = next_event_num;
		cmplx->events_map.aximon_trap_event_num[1] = next_event_num + 1;

		break;
	case (AL_PMDT_MONITOR_WRITE_DIR):
		axi_mon_inst = ((config->default_axi_read) &&
			(cmplx_info->axi_mon_num == AL_PMDT_AXI_MON_BAR_NUM)) ?
			1 : 0;
		cfg_events_for_aximon[axi_mon_inst] = AL_TRUE;
		cmplx->events_map.aximon_trap_event_num[axi_mon_inst] = next_event_num;

		break;
	case (AL_PMDT_MONITOR_READ_DIR):
		axi_mon_inst = ((config->default_axi_read == AL_FALSE) &&
			(cmplx_info->axi_mon_num == AL_PMDT_AXI_MON_BAR_NUM)) ?
			1 : 0;
		cfg_events_for_aximon[axi_mon_inst] = AL_TRUE;
		cmplx->events_map.aximon_trap_event_num[axi_mon_inst] = next_event_num;

		break;
	}

	al_memset(&event_config, 0, sizeof(struct al_pmdt_pmu_event_config));
	event_config.window_en = AL_TRUE;
	event_config.pause_en = AL_TRUE;
	event_config.thr_low = 0;
	event_config.thr_high = 0; /* trigger overflow on first error */

	/* configure trap event */
	for (axi_mon_inst = 0; axi_mon_inst < cmplx_info->axi_mon_num;
			axi_mon_inst++) {
		if (cfg_events_for_aximon[axi_mon_inst] == AL_FALSE)
			continue;
		trig_num = al_pmdt_map_trig_num_get(
				cmplx_info->axi_mon_num,
				axi_mon_inst,
				AL_PMDT_INT_TRIG_TRACKER_INC_1,
				next_filt_num);
		al_assert(trig_num >= 0);
		event_config.trig_num = (unsigned int)trig_num;
		rc = al_pmdt_pmu_event_config(
				&cmplx->pmu,
				next_event_num++,
				&event_config);
		al_assert(!rc);
		*event_mask |= AL_BIT(next_event_num - 1); /* for event enablement */
		*irq_mask |= AL_BIT(next_event_num - 1); /* for irq  enablement */
	}

	*event_num = next_event_num;
	*filter_num = next_filt_num + 1;

	return 0;
}

static int al_pmdt_pmu_track_events_config(
	struct al_pmdt_complex_handle *cmplx,
	struct al_pmdt_pmu_cmplx_info *cmplx_info,
	int *event_num, int *filter_num,
	uint32_t *event_mask,
	uint32_t *irq_mask)
{
	int trig_num, rc;
	struct al_pmdt_config *config = &cmplx->unit_config;
	int next_event_num = *event_num;
	int next_filt_num = *filter_num;
	struct al_pmdt_pmu_event_config event_config;
	unsigned int axi_mon_inst;
	al_bool cfg_events_for_aximon[AL_PMDT_AXI_MON_MAX_NUM] = {AL_FALSE};

	/* -1 defines that no events were configured for AXI mon trapping feature */
	cmplx->events_map.aximon_track_event_num[0] = -1;
	cmplx->events_map.aximon_track_event_num[1] = -1;

	switch (config->track_config.direction) {
	case (AL_PMDT_MONITOR_READ_WRITE_DIR):
		cfg_events_for_aximon[0] = AL_TRUE;
		cfg_events_for_aximon[1] = AL_TRUE;
		cmplx->events_map.aximon_track_event_num[0] = next_event_num;
		cmplx->events_map.aximon_track_event_num[1] = next_event_num + 1;

		break;
	case (AL_PMDT_MONITOR_WRITE_DIR):
		axi_mon_inst = ((config->default_axi_read) &&
				(cmplx_info->axi_mon_num == AL_PMDT_AXI_MON_BAR_NUM)) ?
				1 : 0;
		cfg_events_for_aximon[axi_mon_inst] = AL_TRUE;
		cmplx->events_map.aximon_track_event_num[axi_mon_inst] = next_event_num;

		break;
	case (AL_PMDT_MONITOR_READ_DIR):
		axi_mon_inst = ((config->default_axi_read == AL_FALSE) &&
				(cmplx_info->axi_mon_num == AL_PMDT_AXI_MON_BAR_NUM)) ?
				1 : 0;
		cfg_events_for_aximon[axi_mon_inst] = AL_TRUE;
		cmplx->events_map.aximon_track_event_num[axi_mon_inst] = next_event_num;

		break;
	}
	al_memset(&event_config, 0, sizeof(struct al_pmdt_pmu_event_config));
	event_config.window_en = AL_TRUE;
	event_config.pause_en = AL_TRUE;

	if (config->track_config.stop_on_cross) {
		event_config.thr_low =
				config->track_config.lower_bound; /* trigger underflow */
		event_config.thr_high =
				config->track_config.upper_bound; /* trigger overflow */
	} else {
		event_config.thr_low = 0;
		event_config.thr_high = config->track_config.upper_bound;
	}

	/* configure track event */
	for (axi_mon_inst = 0; axi_mon_inst < cmplx_info->axi_mon_num;
			axi_mon_inst++) {
		if (cfg_events_for_aximon[axi_mon_inst] == AL_FALSE)
			continue;
		trig_num = al_pmdt_map_trig_num_get(
			cmplx_info->axi_mon_num,
			axi_mon_inst,
			(config->track_config.track_type == AL_PMDT_TRACK_OCCURRENCES) ?
				AL_PMDT_INT_TRIG_TRACKER_INC_1 :
				AL_PMDT_INT_TRIG_TRACKER_INC_VAL,
			next_filt_num);
		al_assert(trig_num >= 0);

		event_config.trig_num = (unsigned int)trig_num;

		if (config->track_config.stop_on_cross)
			event_config.cross_trig = AL_BIT(next_event_num); /* self paused */
		rc = al_pmdt_pmu_event_config(
				&cmplx->pmu,
				next_event_num++,
				&event_config);
		al_assert(!rc);
		*event_mask |= AL_BIT(next_event_num - 1); /* for event enablement */
		if (config->track_config.stop_on_cross)
			*irq_mask |= AL_BIT(next_event_num - 1); /* for irq  enablement */
	}

	*event_num = next_event_num;
	*filter_num = next_filt_num + 1;

	return 0;
}

static void al_pmdt_axi_trap_config(
	struct al_pmdt_axi_mon *axi_mon,
	struct al_pmdt_config *config,
	unsigned int *filter_num)
{
	enum al_pmdt_trap_events trap_event = config->trap_config.trap_event;
	struct al_pmdt_axi_mon_tracker tracker;
	struct al_pmdt_axi_mon_trace trace;
	uint8_t next_filter_num = *filter_num;
	uint8_t trace_num = 0;
	uint8_t axi_resp_word;
	uint8_t axi_resp_pos_in_word;
	int i;
	int rc;

	al_memset(&tracker, 0 , sizeof(struct al_pmdt_axi_mon_tracker));
	if (trap_event == AL_PMDT_TRAP_CUSTOM_EVENT) {
		for (i = 0; i < AL_PMDT_AXI_MON_TRACKING_LINE_SIZE; i++) {
			tracker.comp[i] = config->trap_config.trap_mask.comp[i];
			tracker.mask[i] = config->trap_config.trap_mask.mask[i];
		}
	} else {
		axi_resp_word = config->trap_config.axi_resp_field_position/32;
		axi_resp_pos_in_word = config->trap_config.axi_resp_field_position%32;
		if (trap_event == AL_PMDT_TRAP_SLV_ERR_EVENT) {
			tracker.comp[axi_resp_word] |=
					AL_PMDT_AXI_SLAVE_ERROR << axi_resp_pos_in_word;
			tracker.mask[axi_resp_word] |=
					AL_PMDT_AXI_SLAVE_ERROR_MASK << axi_resp_pos_in_word;
		} else {
			if (trap_event == AL_PMDT_TRAP_DEC_ERR_EVENT) {
				tracker.comp[axi_resp_word] |=
					AL_PMDT_AXI_DEC_ERROR << axi_resp_pos_in_word;
				tracker.mask[axi_resp_word] |=
					AL_PMDT_AXI_DEC_ERROR_MASK << axi_resp_pos_in_word;
			} else { /* trap_event == AL_PMDT_TRAP_SLV_DEC_ERR_EVENT */
				tracker.comp[axi_resp_word] |=
					AL_PMDT_AXI_SLV_DEC_ERROR << axi_resp_pos_in_word;
				tracker.mask[axi_resp_word] |=
					AL_PMDT_AXI_SLV_DEC_ERROR_MASK << axi_resp_pos_in_word;
			}
		}
	}

	/* Add AXI Transaction valid bit to tracker mask and compare */
	tracker.comp[AL_PMDT_AXI_TRANS_VALID_WORD] |=
			AL_BIT(AL_PMDT_AXI_TRANS_VALID_BIT);
	tracker.mask[AL_PMDT_AXI_TRANS_VALID_WORD] |=
			AL_BIT(AL_PMDT_AXI_TRANS_VALID_BIT);

	rc = al_pmdt_axi_mon_tracker_config(
			axi_mon,
			next_filter_num,
			&tracker);
	al_assert(!rc);

	if (config->trap_config.is_trace) {
		al_memset(&trace, 0, sizeof(struct al_pmdt_axi_mon_trace));
		/* Get current trace configuration */
		al_pmdt_axi_mon_trace_config_get(axi_mon, trace_num, &trace);
		/* configure trace data */
		/* add current filter to list of filter
		 * that would send trace to ELA from current AXI monitor
		 */
		trace.tracker_sel |= AL_BIT(next_filter_num);
		/* outstanding information will be included in trace */
		trace.incl_busy = AL_TRUE;
		/* byte count of logged transaction  will be included in trace */
		trace.incl_bcount = AL_TRUE;
		trace.incl_data = AL_FALSE;
		trace.user_width = 0;
		/* attributes of logged transaction will be included in trace */
		trace.incl_attr = AL_TRUE;
		trace.tgtid_width = 0;
		/* 32 LSB bits of transaction address (4 words) will be included */
		trace.addr_width = 4;
		/* transaction ID will be included in trace */
		trace.id_width = 8;
		rc |= al_pmdt_axi_mon_trace_config(axi_mon, trace_num, &trace);
		al_assert(!rc);

		al_pmdt_axi_mon_trace_enable(axi_mon, trace_num, AL_TRUE);
	}

	*filter_num = next_filter_num+1;
}

static void al_pmdt_axi_track_config(
	struct al_pmdt_axi_mon *axi_mon,
	unsigned int instance,
	struct al_pmdt_config *config,
	unsigned int *filter_num)
{
	struct al_pmdt_axi_mon_tracker tracker;
	struct al_pmdt_axi_mon_trace trace;
	struct al_pmdt_axi_mon_tracker *track_mask;
	uint8_t next_filter_num = *filter_num;
	uint8_t trace_num = 0;
	int i;
	int rc;

	/* Return if all tracking filter should be
	 * configured and such one already configured */
	if ((config->track_config.is_track_all == AL_TRUE) &&
			(config->track_config.all_track_filter_num != -1))
		return;

	if ((instance == 1) || (config->default_axi_read == AL_FALSE))
		track_mask = &config->track_config.wtrack_mask;
	else
		track_mask = &config->track_config.rtrack_mask;


	al_memset(&tracker, 0 , sizeof(struct al_pmdt_axi_mon_tracker));
	if (config->track_config.is_track_all == AL_FALSE)
		for (i = 0; i < 5; i++) {
			tracker.comp[i] = track_mask->comp[i];
			tracker.mask[i] = track_mask->mask[i];
		}
	else
		config->track_config.all_track_filter_num = next_filter_num;

	/* Add AXI Transaction valid bit to tracker mask and compare */
	tracker.comp[AL_PMDT_AXI_TRANS_VALID_WORD] |=
			AL_BIT(AL_PMDT_AXI_TRANS_VALID_BIT);
	tracker.mask[AL_PMDT_AXI_TRANS_VALID_WORD] |=
			AL_BIT(AL_PMDT_AXI_TRANS_VALID_BIT);

	rc = al_pmdt_axi_mon_tracker_config(
			axi_mon,
			next_filter_num,
			&tracker);
	al_assert(!rc);

	al_memset(&trace, 0, sizeof(struct al_pmdt_axi_mon_trace));
	/* Get current trace configuration */
	al_pmdt_axi_mon_trace_config_get(axi_mon, trace_num, &trace);
	/* configure trace data */
	/* add current filter to list of filters
	 * that would send trace to ELA from current AXI monitor
	 */
	trace.tracker_sel |= AL_BIT(next_filter_num);
	/* outstanding information will be included in trace */
	trace.incl_busy = AL_TRUE;
	/* byte count of logged transaction  will be included in trace */
	trace.incl_bcount = AL_TRUE;
	trace.incl_data = AL_FALSE;
	trace.user_width = 0;
	/* attributes of logged transaction will be included in trace */
	trace.incl_attr = AL_TRUE;
	trace.tgtid_width = 0;
	/* 32 LSB bits of transaction address (4 words) will be included in trace */
	trace.addr_width = 4;
	/* transaction ID will be included in trace */
	trace.id_width = 8;
	rc = al_pmdt_axi_mon_trace_config(axi_mon, trace_num, &trace);
	al_assert(!rc);

	al_pmdt_axi_mon_trace_enable(axi_mon, trace_num, AL_TRUE);

	*filter_num = next_filter_num+1;
}

static int al_pmdt_cmplx_configuration_verify(
	struct al_pmdt_config *config,
	struct al_pmdt_pmu_cmplx_info *cmplx_info,
	uint8_t *req_axi_mon_num, uint8_t *req_ela_num)
{
	enum al_pmdt_monitor_direction direction =
			AL_PMDT_MONITOR_READ_WRITE_DIR + 1;

	*req_axi_mon_num = calc_req_num_of_axi_mon(config);
	if (*req_axi_mon_num > cmplx_info->axi_mon_num) {
		al_err("Number of requested AXI_MON units cannot supported by Complex "
				"(max %d)\n",
				cmplx_info->axi_mon_num);
		return -EINVAL;
	}
	*req_ela_num = calc_req_num_of_ela(config);
	if (*req_ela_num > cmplx_info->ela_num) {
		al_err("Number of requested ELA units cannot supported by Complex "
				"(max %d)\n",
				cmplx_info->ela_num);
		return -EINVAL;
	}

	/* Validate that directions of all requested features are same */
	config->default_axi_read = AL_TRUE;

	if (config->feature_mask & AL_PMDT_FEATURE_AXI_TIMEOUT) {
		if (direction > AL_PMDT_MONITOR_READ_WRITE_DIR) {
			/* Set current direction */
			direction = config->timeout_config.direction;
			if ((*req_axi_mon_num == AL_PMDT_AXI_MON_MAX_NUM - 1) &&
					(direction == AL_PMDT_MONITOR_WRITE_DIR))
				config->default_axi_read = AL_FALSE;
		} else {
			if ((*req_axi_mon_num == AL_PMDT_AXI_MON_MAX_NUM - 1) &&
					(config->timeout_config.direction != direction)) {
				al_err("Timeout direction is different from others\n");
				return -EINVAL;
			}
		}
	}

	if (config->feature_mask & AL_PMDT_FEATURE_AXI_LATENCY_HISTOGRAM) {
		if (direction > AL_PMDT_MONITOR_READ_WRITE_DIR) {
			direction = config->latency_config.direction;
			if ((cmplx_info->axi_mon_num ==
					AL_PMDT_AXI_MON_MAX_NUM - 1) &&
					(direction == AL_PMDT_MONITOR_WRITE_DIR))
				config->default_axi_read = AL_FALSE;
		} else {
			if ((*req_axi_mon_num == AL_PMDT_AXI_MON_MAX_NUM - 1) &&
					(config->latency_config.direction != direction)) {
				al_err("Latency direction is different from others\n");
				return -EINVAL;
			}
		}
	}

	if (config->feature_mask & AL_PMDT_FEATURE_AXI_TRAP) {
		if (direction > AL_PMDT_MONITOR_READ_WRITE_DIR) {
			direction = config->trap_config.direction;
			if ((cmplx_info->axi_mon_num ==
					AL_PMDT_AXI_MON_MAX_NUM - 1) &&
					(direction == AL_PMDT_MONITOR_WRITE_DIR))
				config->default_axi_read = AL_FALSE;
	} else {
		if ((*req_axi_mon_num == AL_PMDT_AXI_MON_MAX_NUM - 1) &&
				(config->trap_config.direction != direction)) {
				al_err("Trapping direction is different from others");
				return -EINVAL;
			}
		}
	}

	if (config->feature_mask & AL_PMDT_FEATURE_AXI_TRACK) {
		if (direction > AL_PMDT_MONITOR_READ_WRITE_DIR) {
			direction = config->track_config.direction;
			if ((cmplx_info->axi_mon_num ==
					AL_PMDT_AXI_MON_MAX_NUM - 1) &&
					(direction == AL_PMDT_MONITOR_WRITE_DIR))
				config->default_axi_read = AL_FALSE;
		} else {
			if ((*req_axi_mon_num == AL_PMDT_AXI_MON_MAX_NUM - 1) &&
					(config->track_config.direction != direction)) {
					al_err("Tracking direction is different from others");
					return -EINVAL;
			}
		}
	}

	return 0;
}

static int al_pmdt_irq_enable(
		struct al_iofic_regs __iomem *iofic_regs,
		int group,
		uint32_t mask)
{
	int rc;
	rc = al_iofic_config(
			iofic_regs,
			group,
			INT_CONTROL_GRP_SET_ON_POSEDGE |
			INT_CONTROL_GRP_MASK_MSI_X);
	if (rc)
		return -1;

	al_iofic_clear_cause(iofic_regs, group,	mask);
	al_iofic_unmask(iofic_regs, group, mask);

	return 0;
}

static int al_pmdt_pmu_program(
	struct al_pmdt_complex_handle *cmplx,
	struct al_pmdt_pmu_cmplx_info *cmplx_info)
{
	struct al_pmdt_config *config;
	struct al_pmdt_pmu *pmu;
	struct al_pmdt_pmu_config pmu_config;
	void __iomem *iofic_regs;
	int rc;
	int event_num;
	int filter_num;
	uint32_t event_mask;
	int irq_num;
	uint32_t groupa_irq_mask = 0;
	uint32_t groupb_irq_mask = 0;
	unsigned int features;
	uint32_t sampl_wind_size;

	pmu = &cmplx->pmu;
	config = &cmplx->unit_config;
	features = config->feature_mask;
	event_num = 0;
	filter_num = 0;
	event_mask = 0;

	if (features == 0) {
		al_err("No active features\n");
		return -1;
	}
	/* set pmu config */
	al_memset(&pmu_config, 0, sizeof(struct al_pmdt_pmu_config));

	if ((features & AL_PMDT_FEATURES_MASK) != AL_PMDT_FEATURE_AXI_TIMEOUT) {
		pmu_config.windows_en = AL_FALSE;
		pmu_config.window_unit_exp = AL_PMDT_PMU_WINDOW_DEFAULT_EXP;
	}

	pmu_config.gpio_en = AL_TRUE;
	pmu_config.ovf_int_en = AL_TRUE;
	pmu_config.ovf_trig_en = AL_TRUE;
	pmu_config.unf_int_en = AL_TRUE;
	pmu_config.unf_trig_en = AL_TRUE;
	pmu_config.inc_val_en = AL_TRUE;
	pmu_config.tstamp_init = AL_TRUE;
	pmu_config.error_trig_en = AL_TRUE;
	pmu_config.error_gpio_en = AL_TRUE;

	rc = al_pmdt_pmu_config(pmu, &pmu_config);
	if (rc)
		return -1;
	if (pmu_config.windows_en) {
		if (features & AL_PMDT_FEATURE_AXI_TRAP)
			sampl_wind_size = AL_PMDT_PMU_WINDOW_TRAP_SAMPLE_TIME;
		else if (features & AL_PMDT_FEATURE_AXI_TRACK)
			sampl_wind_size = config->track_config.sampling_window_us;
		else
			sampl_wind_size = AL_PMDT_PMU_WINDOW_DEFAULT_SAMPLE_TIME;
		/* set window size */
		al_pmdt_pmu_window_size_set(pmu, sampl_wind_size);
	}

	/* set event counters */
	if (features & AL_PMDT_FEATURE_AXI_LATENCY_HISTOGRAM) {
		rc = al_pmdt_pmu_latency_events_config(
			cmplx, cmplx_info, &event_num, &event_mask);
		al_assert(!rc);
	}

	if (features & AL_PMDT_FEATURE_AXI_TRAP) {
		rc = al_pmdt_pmu_trap_events_config(
				cmplx, cmplx_info, &event_num,
				&filter_num, &event_mask , &groupa_irq_mask);
		al_assert(!rc);
	}

	if (features & AL_PMDT_FEATURE_AXI_TRACK) {
		rc = al_pmdt_pmu_track_events_config(
				cmplx, cmplx_info, &event_num,
				&filter_num, &event_mask , &groupa_irq_mask);
		al_assert(!rc);
	}

	/* When ONLY TIMEOUT detection is required, at least one event ,
	 * for each AXI, in PMU should be configured and enabled.
	 * This will enable appropriate AXI MON too.
	 */
	if ((features & AL_PMDT_FEATURES_MASK) == AL_PMDT_FEATURE_AXI_TIMEOUT) {
		rc = al_pmdt_pmu_timeout_events_config(
			cmplx, cmplx_info, &event_num, &event_mask);
		al_assert(!rc);
	}

	/* enable interrupts */
	iofic_regs = al_pmdt_pmu_iofic_base_get(pmu);
	rc = al_pmdt_irq_enable(iofic_regs, AL_PMDT_INT_GROUP_A, groupa_irq_mask);
	al_assert(!rc);

	/* Configure PMU iofic group B */
	if ((cmplx_info->axi_mon_num == AL_PMDT_AXI_MON_MAX_NUM)  &&
			(config->timeout_config.direction != AL_PMDT_MONITOR_READ_DIR)) {
		if (features & AL_PMDT_FEATURE_AXI_TIMEOUT) {
			irq_num = al_pmdt_map_pmu_irq_num_get(
					cmplx_info->axi_mon_num, 1, AL_PMDT_AXIMON_TIMEOUT_IRQ);
			groupb_irq_mask |= AL_BIT(irq_num);
		}

		irq_num = al_pmdt_map_pmu_irq_num_get(
				cmplx_info->axi_mon_num, 1, AL_PMDT_AXIMON_OVERFLOW_IRQ);
		groupb_irq_mask |= AL_BIT(irq_num);
		irq_num = al_pmdt_map_pmu_irq_num_get(
				cmplx_info->axi_mon_num, 1, AL_PMDT_AXIMON_UNDERFLOW_IRQ);
		groupb_irq_mask |= AL_BIT(irq_num);


	}

	if (features & AL_PMDT_FEATURE_AXI_TIMEOUT) {
		irq_num = al_pmdt_map_pmu_irq_num_get(
				cmplx_info->axi_mon_num, 0, AL_PMDT_AXIMON_TIMEOUT_IRQ);
		groupb_irq_mask |= AL_BIT(irq_num);
	}

	irq_num = al_pmdt_map_pmu_irq_num_get(
			cmplx_info->axi_mon_num, 0, AL_PMDT_AXIMON_OVERFLOW_IRQ);
	groupb_irq_mask |= AL_BIT(irq_num);
	irq_num = al_pmdt_map_pmu_irq_num_get(
			cmplx_info->axi_mon_num, 0, AL_PMDT_AXIMON_UNDERFLOW_IRQ);
	groupb_irq_mask |= AL_BIT(irq_num);

	rc = al_pmdt_irq_enable(iofic_regs, AL_PMDT_INT_GROUP_B, groupb_irq_mask);
	al_assert(!rc);

	/* enable events */
	al_pmdt_pmu_events_enable(pmu, AL_TRUE, &event_mask);

	/* enable PMU */
	al_pmdt_pmu_clear(pmu);
	al_pmdt_pmu_enable(pmu, AL_TRUE);

	return 0;
}

static int al_pmdt_axi_mon_program(
	struct al_pmdt_complex_handle *cmplx,
	unsigned int instance,
	struct al_pmdt_map_entry *entry)
{
	struct al_pmdt_axi_mon *axi_mon;
	struct al_pmdt_config *config;
	struct al_pmdt_axi_mon_mode axi_mon_mode;
	struct al_pmdt_axi_mon_bars bars;
	uint64_t *bar_thr;
	unsigned int features;
	unsigned int track_id;
	int i;
	int rc;
	al_bool track_all_tmp;

	axi_mon = &cmplx->axi_mon[instance];
	config = &cmplx->unit_config;
	features = config->feature_mask;

	/*Initialize AXI Monitor Handle*/
	rc = al_pmdt_axi_mon_handle_init(
			axi_mon,
			entry->name,
			instance,
			entry->regs_base);
	if (rc)
		return -1;

	al_memset(&axi_mon_mode, 0, sizeof(struct al_pmdt_axi_mon_mode));
	/* set monitor mode */
	if (config->default_axi_read && (instance == 0))
		axi_mon_mode.is_read = AL_TRUE;
	else
		axi_mon_mode.is_read = AL_FALSE;

	track_id = 0;

	axi_mon_mode.log_mode = AL_AXI_MON_LOG_TRANS_LATENCY; /* full latency */
	axi_mon_mode.cyc_cnt = AL_TRUE; /*we want fine granularity (e.g. nano-sec)*/
	axi_mon_mode.log_first = 0; /* don't care */

	rc = al_pmdt_axi_mon_mode_config(axi_mon, &axi_mon_mode);
	al_assert(!rc);

	if (features &
		(AL_PMDT_FEATURE_AXI_TIMEOUT | AL_PMDT_FEATURE_AXI_LATENCY_HISTOGRAM)) {
		al_memset(&bars, 0, sizeof(struct al_pmdt_axi_mon_bars));
		if (features & AL_PMDT_FEATURE_AXI_TIMEOUT) {
			bars.timeout = axi_mon_mode.is_read ?
					config->timeout_config.read_timeout_us :
					config->timeout_config.write_timeout_us;
		}

		if (features & AL_PMDT_FEATURE_AXI_LATENCY_HISTOGRAM) {
			/* set histogram bars */
			bar_thr = axi_mon_mode.is_read ?
					config->latency_config.read_bar_thr :
					config->latency_config.write_bar_thr;
			/* There are 5 latency bars available in AXI monitor ,
			 * allow to dispense transactions latencies to 6 bins as following:
			 * bin_0: latency < bar0.value
			 * bin_1: bar0.value <= latency < bar1.value
			 * bin_2: bar1.value <= latency < bar2.value
			 * bin_3: bar2.value <= latency < bar3.value
			 * bin_4: bar3.value <= latency < bar4.value
			 * bin_5: bar4.value <= latency
			 */
			for (i = 0; i < (AL_PMDT_AXI_MON_BAR_NUM - 1); i++)
				bars.bar_thr[i] = bar_thr[i];

			if (bars.bar_thr[AL_PMDT_AXI_MON_BAR_NUM - 2] > bars.timeout) {
				al_err("Bad timeout value is required to be configured\n");
				return -EINVAL;
			}
		}
		rc = al_pmdt_axi_mon_bars_config(axi_mon, &bars);
		al_assert(!rc);
	}

	track_id = 0;
	if (features & AL_PMDT_FEATURE_AXI_TRAP)
		al_pmdt_axi_trap_config(axi_mon, config, &track_id);

	if (features & AL_PMDT_FEATURE_AXI_TRACK)
		al_pmdt_axi_track_config(axi_mon, instance, config, &track_id);

	if ((features & AL_PMDT_FEATURE_AXI_TRAP) ||
			(features & AL_PMDT_FEATURE_AXI_TIMEOUT)) {
		track_all_tmp = config->track_config.is_track_all;
		if (config->track_config.is_track_all == AL_FALSE)
			config->track_config.is_track_all = AL_TRUE;

		al_pmdt_axi_track_config(axi_mon, instance, config, &track_id);
		config->track_config.is_track_all = track_all_tmp;
	}

	return 0;
}

static int al_pmdt_ela_program(
	struct al_pmdt_complex_handle *cmplx,
	unsigned int instance,
	struct al_pmdt_map_entry *entry,
	struct al_pmdt_pmu_cmplx_info *cmplx_info)
{
	struct al_pmdt_elacfg *elacfg = &cmplx->elacfg;
	struct al_pmdt_ela *ela = &cmplx->ela[instance];
	struct al_pmdt_elacfg_config elacfg_config;
	struct al_pmdt_ela_config ela_config;
	struct al_pmdt_ela_trig_config ela_trig_state;
	int signal_group;
	int rc;

	/* set ELA_Config (only when instance 0 is called) */
	if (instance == 0) {
		rc = al_pmdt_elacfg_handle_init(
				elacfg,
				entry->name,
				entry->regs_base);
		al_assert(!rc);

		al_memset(&elacfg_config, 0, sizeof(struct al_pmdt_elacfg_config));
		elacfg_config.nsnid_dbg = AL_TRUE;
		elacfg_config.nsid_dbg = AL_TRUE;
		rc = al_pmdt_elacfg_config(elacfg, &elacfg_config);
		al_assert(!rc);
	}

	/* set ELA */
	rc = al_pmdt_ela_handle_init(
			ela,
			entry->name,
			instance,
			entry->regs_base);
	al_assert(!rc);

	al_pmdt_ela_write_lock(ela, AL_FALSE); /* unlock ELA registers */
	al_pmdt_ela_start(ela, AL_FALSE); /* stop ELA (if it's already running) */

	al_memset(&ela_config, 0, sizeof(struct al_pmdt_ela_config));
	ela_config.st4_trace_en = AL_TRUE;
	ela_config.timestamp_en = AL_TRUE;
	ela_config.ts_int = 0; /*ts inserted only when ACTION.TRACE disables trace*/
	ela_config.tcsel0 = 0; /* keep counter's bits 0 & 1*/
	ela_config.tcsel1 = 1;
	ela_config.st4_trace_en = AL_TRUE;
	ela_config.init_action.trace_en = AL_TRUE;
	rc = al_pmdt_ela_config(ela, &ela_config);
	al_assert(!rc);

	al_memset(&ela_trig_state, 0, sizeof(struct al_pmdt_ela_trig_config));
	signal_group = al_pmdt_map_trace_sig_get(cmplx_info->axi_mon_num, instance);
	if (signal_group < 0) {
		al_err("pmdt illegal signal_group\n");
		return -ENXIO;
	}
	ela_trig_state.signal_group = (unsigned int)signal_group;
	ela_trig_state.trig_ctrl.comp_mode		= AL_ELA_COMP_MODE_SIGNAL;
	ela_trig_state.trig_ctrl.comp_type		= AL_ELA_COMP_EQUAL;
	ela_trig_state.trig_ctrl.trace_cap_ctrl	= AL_ELA_TCC_SIG_MATCH;
	ela_trig_state.ext_trig_mask = 0x0;
	ela_trig_state.ext_trig_comp = 0x0; /* cancel ext_trig filtering*/
	ela_trig_state.action.trace_en = AL_TRUE;
	ela_trig_state.sig_mask[AL_PMDT_AXI_TRACE_VALID_WORD] =
			AL_BIT(AL_PMDT_AXI_TRACE_VALID_BIT);
	ela_trig_state.sig_comp[AL_PMDT_AXI_TRACE_VALID_WORD] =
			AL_BIT(AL_PMDT_AXI_TRACE_VALID_BIT);

	/* TODO:
	 * Correct behavior of ELA after logging entry should be discussed
	 * Currently ELA will return to same state as before logging trace
	 * */
#if 0
	ela_trig_state.next_state = AL_ELA_TRIG_STATE_FINAL;
#endif

	ela_trig_state.next_state = AL_ELA_TRIG_STATE_0;
	rc = al_pmdt_ela_trigger_state_config(ela, 0, &ela_trig_state);

/* Enable System reset event logging, using state_4 */
	ela_trig_state.signal_group = (unsigned int)signal_group;
	ela_trig_state.ext_trig_mask = 0x3;
	ela_trig_state.ext_trig_comp = 0x1; /* reset-assert */
	ela_trig_state.next_state = AL_ELA_TRIG_STATE_4; /* don't change state */
	ela_trig_state.sig_mask[3] = 0; /* don't care */
	ela_trig_state.sig_comp[3] = 0;

	rc |= al_pmdt_ela_trigger_state_config(ela, 4, &ela_trig_state);
	al_assert(!rc);

	return 0;
}

static int al_pmdt_cctm_program(
	struct al_pmdt_handle *pmdt,
	uint32_t clk_freq,
	uint32_t (*pmu_mask)[AL_PMDT_CCTM_MATRIX_LINE_SIZE],
	al_bool enable_interrupt)
{
	int ts_ratio = clk_freq / (AL_PMDT_TS_PERIOD_USEC * 1000000);
	void __iomem *cctm_regs = (void __iomem *)AL_PBS_CCTM_BASE;
	void __iomem *iofic_regs;
	enum al_pmdt_unit unit;
	int i, rc;
	struct al_pmdt_complex_handle *cmplx;
	struct al_pmdt_config *config;
	uint32_t unit_pmu_id;
	uint32_t unit_ela_id;
	uint32_t features;
	uint32_t cross_matrix_mask[AL_PMDT_CCTM_MATRIX_LINE_SIZE] = {0};

	if (pmdt->map_func)
		cctm_regs = pmdt->map_func(cctm_regs,
				(void __iomem *)AL_SB_BASE, AL_PMDT_CCTM_MAP_SIZE);
	rc = al_pmdt_cctm_handle_init(&pmdt->cctm, cctm_regs);
	al_assert(!rc);

	if (enable_interrupt) {
		/* enable relevant PMU interrupts */
		iofic_regs = al_pmdt_cctm_iofic_base_get(&pmdt->cctm);

		for (i = 0; i < AL_PMDT_CCTM_MATRIX_LINE_SIZE; i++) {
			al_iofic_config(iofic_regs, i,
					INT_CONTROL_GRP_SET_ON_POSEDGE |
					INT_CONTROL_GRP_MASK_MSI_X);
			rc = al_pmdt_irq_enable(iofic_regs, i, (*pmu_mask)[i]);
			al_assert(!rc);
		}
	}

	/* Enable GPIO trigger from CCTM*/
	al_pmdt_cctm_gpio_enable(&pmdt->cctm, AL_TRUE, pmu_mask);

	for (unit = 0; unit < AL_PMDT_UNIT_MAX; unit++) {
		cmplx = &pmdt->cmplx[unit];
		config = &cmplx->unit_config;
		if (cmplx->ready == AL_TRUE) {
			features = config->feature_mask;
			/* check if Complex pause as react to trigger
			 * in CCTM is required by it configuration*/
			if ((features &
					(AL_PMDT_FEATURE_AXI_TIMEOUT | AL_PMDT_FEATURE_AXI_TRAP)) ||
				((features & AL_PMDT_FEATURE_AXI_TRACK) &&
						config->track_config.stop_on_cross)) {
				cross_matrix_mask[cmplx->cctm_grp] =
						AL_BIT(cmplx->cctm_pmu_bit);
				unit_pmu_id = cmplx->cctm_grp*32 + cmplx->cctm_pmu_bit;
				unit_ela_id = cmplx->cctm_grp*32 + cmplx->cctm_ela_bit;
				/* Configure CCTM cross_matrix to propagate pause signal to
				 * Complex as react to trigger from itself
				 */
				al_pmdt_cctm_cross_pause_enable(
						&pmdt->cctm, AL_TRUE,
						unit_pmu_id, &cross_matrix_mask);
				al_pmdt_cctm_cross_pause_enable(
						&pmdt->cctm, AL_TRUE,
						unit_ela_id, &cross_matrix_mask);
				cross_matrix_mask[cmplx->cctm_grp] = 0;
			}
		}
	}

	/* start timestamp generator */
	rc = al_pmdt_cctm_timestamp_ratio_set(&pmdt->cctm, ts_ratio);
	al_assert(!rc);
	al_pmdt_cctm_timestamp_clear(&pmdt->cctm);
	al_pmdt_cctm_timestamp_enable(&pmdt->cctm, AL_TRUE);
	return rc;
}

static int al_pmdt_start_modules(
	struct al_pmdt_handle *pmdt,
	uint32_t (*pmu_mask)[AL_PMDT_CCTM_MATRIX_LINE_SIZE])
{
	int i, rc;
	struct al_pmdt_complex_handle *cmplx;

	for (i = 0; i < AL_PMDT_UNIT_MAX; i++) {
		if (pmdt->cmplx[i].ready == AL_FALSE)
			continue;

		cmplx = &pmdt->cmplx[i];

		/* enable AXI monitors */
		if (cmplx->aximon_active_mask & AL_PMDT_INSTANCE_0) {
			al_pmdt_axi_mon_clear(&cmplx->axi_mon[0],
					AL_TRUE, AL_FALSE, AL_TRUE);
			al_pmdt_axi_mon_enable(&cmplx->axi_mon[0], AL_TRUE);
		}
		if (cmplx->aximon_active_mask & AL_PMDT_INSTANCE_1) {
			al_pmdt_axi_mon_clear(&cmplx->axi_mon[1],
					AL_TRUE, AL_FALSE, AL_TRUE);
			al_pmdt_axi_mon_enable(&cmplx->axi_mon[1], AL_TRUE);
		}

		/* enable PMU */
		al_pmdt_pmu_clear(&cmplx->pmu);
		al_pmdt_pmu_enable(&cmplx->pmu, AL_TRUE);

		/* start and lock ELA */
		if (cmplx->ela_active_mask & AL_PMDT_INSTANCE_0) {
			rc = al_pmdt_ela_start(&cmplx->ela[0], AL_TRUE);
			al_assert(!rc);
		}
		if (cmplx->ela_active_mask & AL_PMDT_INSTANCE_1) {
			rc = al_pmdt_ela_start(&cmplx->ela[1], AL_TRUE);
			al_assert(!rc);
		}
	}

	/* enable all pmdts via cctm */
	al_pmdt_cctm_pmu_clear(&pmdt->cctm, pmu_mask);
	al_pmdt_cctm_pmu_enable(&pmdt->cctm, AL_TRUE, pmu_mask);

	return 0;
}

#if 0
/* Should be found a way how to check if HW unit (module)
 * exists and enabled in system
 */

static al_bool is_hw_unit_presents(enum al_pmdt_unit pmdtu)
{
	pci_dev_t	dev;
	struct al_pbs_regs *pbs_regs =
			(struct al_pbs_regs *)(uintptr_t)AL_PBS_REGFILE_BASE;
	static al_bool ssame_unit_exist[] = {AL_FALSE};
	static al_bool ssame_unit_presents_tested[] = {AL_FALSE};
	static al_bool sata_unit_exist[] = {AL_FALSE};
	static al_bool sata_unit_presents_tested[] = {AL_FALSE};
	static al_bool pcie_unit_exist[] = {AL_FALSE};
	static al_bool pcie_unit_presents_tested[] = {AL_FALSE};
	static al_bool eth_std_unit_exist[] = {AL_FALSE};
	static al_bool eth_std_unit_presents_tested[] = {AL_FALSE};
	static al_bool eth_v4_unit_exist[] = {AL_FALSE};
	static al_bool eth_v4_unit_presents_tested[] = {AL_FALSE};

	al_bool return_value;

	switch (pmdtu) {

	case (AL_PMDT_UNIT_CPU_CLUSTER_0):
	case (AL_PMDT_UNIT_CPU_CLUSTER_1):
	case (AL_PMDT_UNIT_CPU_CLUSTER_2):
	case (AL_PMDT_UNIT_CPU_CLUSTER_3):
		return_value = AL_TRUE;
		break;
	case (AL_PMDT_UNIT_ADAPT_SSMAE_0):
	case (AL_PMDT_UNIT_ADAPT_SSMAE_1):
		dev = PCI_BDF(0, dev_num, 0);
		pcie_read_config16(0, dev, PCI_VENDOR_ID, &vendorid);
		if (vendorid == 0xffff) {
			al_err("%s was not been detected\n", units_names[dev_num]);
			continue;
		}
	case (AL_PMDT_UNIT_UDMA_0_SSMAE_0):
	case (AL_PMDT_UNIT_UDMA_1_SSMAE_0):
	case (AL_PMDT_UNIT_UDMA_2_SSMAE_0):
	case (AL_PMDT_UNIT_UDMA_3_SSMAE_0):
	case (AL_PMDT_UNIT_UDMA_0_SSMAE_1):
	case (AL_PMDT_UNIT_UDMA_1_SSMAE_1):
	case (AL_PMDT_UNIT_UDMA_2_SSMAE_1):
	case (AL_PMDT_UNIT_UDMA_3_SSMAE_1):

	case (AL_PMDT_UNIT_ADAPT_ETH_10_0):
	case (AL_PMDT_UNIT_ADAPT_ETH_10_1):
	case (AL_PMDT_UNIT_ADAPT_ETH_100_0):
	case (AL_PMDT_UNIT_ADAPT_ETH_100_1):
	case (AL_PMDT_UNIT_ADAPT_ETH_100_2):
	case (AL_PMDT_UNIT_ADAPT_ETH_100_3):
	case (AL_PMDT_UNIT_ADAPT_SATA_0):
	case (AL_PMDT_UNIT_ADAPT_SATA_1):
	case (AL_PMDT_UNIT_ADAPT_SATA_2):
	case (AL_PMDT_UNIT_ADAPT_SATA_3):
	case (AL_PMDT_UNIT_ADAPT_SATA_4):
	case (AL_PMDT_UNIT_ADAPT_SATA_5):
	case (AL_PMDT_UNIT_ADAPT_SATA_6):
	case (AL_PMDT_UNIT_ADAPT_SATA_7):
		config->timeout_config.read_timeout_us =
				AL_PMDT_SB_AXI_TRANS_DEFAULT_TIMEOUT;
		config->timeout_config.write_timeout_us =
				AL_PMDT_SB_AXI_TRANS_DEFAULT_TIMEOUT;
		/* Initialize default latency bins */
		config->latency_config.read_bar_thr[0] =
				AL_PMDT_DDR_READ_ACCESS_THRHLD_BIN_0;
		config->latency_config.write_bar_thr[0] =
				AL_PMDT_DDR_WRITE_ACCESS_THRHLD_BIN_0;
		config->latency_config.read_bar_thr[1] =
				AL_PMDT_DDR_READ_ACCESS_THRHLD_BIN_1;
		config->latency_config.write_bar_thr[1] =
				AL_PMDT_DDR_WRITE_ACCESS_THRHLD_BIN_1;
		config->latency_config.read_bar_thr[2] =
				AL_PMDT_DDR_READ_ACCESS_THRHLD_BIN_2;
		config->latency_config.write_bar_thr[2] =
				AL_PMDT_DDR_WRITE_ACCESS_THRHLD_BIN_2;
		config->latency_config.read_bar_thr[3] =
				AL_PMDT_DDR_READ_ACCESS_THRHLD_BIN_3;
		config->latency_config.write_bar_thr[3] =
				AL_PMDT_DDR_WRITE_ACCESS_THRHLD_BIN_3;
		config->latency_config.read_bar_thr[4] =
				AL_PMDT_DDR_READ_ACCESS_THRHLD_BIN_4;
		config->latency_config.write_bar_thr[4] =
				AL_PMDT_DDR_WRITE_ACCESS_THRHLD_BIN_4;
		/*
		 * Initialize starting position of AXI transaction response bits
		 * in AXI transaction vector
		 * */
		config->trap_config.axi_resp_field_position = 119;
		break;
	case (AL_PMDT_UNIT_PCIe_SLVAXI_0):
	case (AL_PMDT_UNIT_PCIe_SLVAXI_1):
	case (AL_PMDT_UNIT_PCIe_SLVAXI_2):
	case (AL_PMDT_UNIT_PCIe_SLVAXI_3):
	case (AL_PMDT_UNIT_PCIe_SLVAXI_4):
	case (AL_PMDT_UNIT_PCIe_SLVAXI_5):
	case (AL_PMDT_UNIT_PCIe_SLVAXI_6):
	case (AL_PMDT_UNIT_PCIe_SLVAXI_7):
		config->timeout_config.direction = AL_PMDT_MONITOR_READ_DIR;
		config->trap_config.direction = AL_PMDT_MONITOR_READ_DIR;
		config->track_config.direction = AL_PMDT_MONITOR_READ_DIR;
		config->latency_config.direction = AL_PMDT_MONITOR_READ_DIR;

		config->timeout_config.read_timeout_us =
				AL_PMDT_SB_AXI_TRANS_DEFAULT_TIMEOUT;
		/* Initialize default latency bins */
		config->latency_config.read_bar_thr[0] =
				AL_PMDT_DDR_READ_ACCESS_THRHLD_BIN_0;
		config->latency_config.read_bar_thr[1] =
				AL_PMDT_DDR_READ_ACCESS_THRHLD_BIN_1;
		config->latency_config.read_bar_thr[2] =
				AL_PMDT_DDR_READ_ACCESS_THRHLD_BIN_2;
		config->latency_config.read_bar_thr[3] =
				AL_PMDT_DDR_READ_ACCESS_THRHLD_BIN_3;
		config->latency_config.read_bar_thr[4] =
				AL_PMDT_DDR_READ_ACCESS_THRHLD_BIN_4;
		/*TODO: AXI response field in PCIE depends on it configuration
		 * For PCIex4 Slave position it is 133, and for PCIeX8,16 it is 132.
		 * Right way how to define position according to PCIE configuration
		 * should be found
		 */
		config->trap_config.axi_resp_field_position = 132;
		break;
	case (AL_PMDT_UNIT_PCIe_MSTRAXI_0):
	case (AL_PMDT_UNIT_PCIe_MSTRAXI_1):
	case (AL_PMDT_UNIT_PCIe_MSTRAXI_2):
	case (AL_PMDT_UNIT_PCIe_MSTRAXI_3):
	case (AL_PMDT_UNIT_PCIe_MSTRAXI_4):
	case (AL_PMDT_UNIT_PCIe_MSTRAXI_5):
	case (AL_PMDT_UNIT_PCIe_MSTRAXI_6):
	case (AL_PMDT_UNIT_PCIe_MSTRAXI_7):
		config->trap_config.direction = AL_PMDT_MONITOR_WRITE_DIR;
		config->track_config.direction = AL_PMDT_MONITOR_WRITE_DIR;
		config->timeout_config.direction = AL_PMDT_MONITOR_WRITE_DIR;
		config->latency_config.direction = AL_PMDT_MONITOR_WRITE_DIR;
		config->timeout_config.write_timeout_us =
				AL_PMDT_SB_AXI_TRANS_DEFAULT_TIMEOUT;
		/* Initialize default latency bins */
		config->latency_config.write_bar_thr[0] =
				AL_PMDT_DDR_WRITE_ACCESS_THRHLD_BIN_0;
		config->latency_config.write_bar_thr[1] =
				AL_PMDT_DDR_WRITE_ACCESS_THRHLD_BIN_1;
		config->latency_config.write_bar_thr[2] =
				AL_PMDT_DDR_WRITE_ACCESS_THRHLD_BIN_2;
		config->latency_config.write_bar_thr[3] =
				AL_PMDT_DDR_WRITE_ACCESS_THRHLD_BIN_3;
		config->latency_config.write_bar_thr[4] =
				AL_PMDT_DDR_WRITE_ACCESS_THRHLD_BIN_4;
		/*TODO: AXI response field in PCIE depends on it configuration
		 * For PCIex4 Master position it is 120 and for PCIeX8,16 it is 119.
		 * Right way how to define position according to PCIE configuration
		 * should be found
		 */
		config->trap_config.axi_resp_field_position = 119;
		break;
	case (AL_PMDT_UNIT_UDMA_0_SSMAE_0):
	case (AL_PMDT_UNIT_UDMA_1_SSMAE_0):
	case (AL_PMDT_UNIT_UDMA_2_SSMAE_0):
	case (AL_PMDT_UNIT_UDMA_3_SSMAE_0):
	case (AL_PMDT_UNIT_UDMA_0_SSMAE_1):
	case (AL_PMDT_UNIT_UDMA_1_SSMAE_1):
	case (AL_PMDT_UNIT_UDMA_2_SSMAE_1):
	case (AL_PMDT_UNIT_UDMA_3_SSMAE_1):
	case (AL_PMDT_UNIT_NBC):
	case (AL_PMDT_UNIT_DDR_CONTROLER_0):
	case (AL_PMDT_UNIT_DDR_CONTROLER_1):
	case (AL_PMDT_UNIT_UDMA_ETH_100_0):
	case (AL_PMDT_UNIT_UDMA_ETH_100_1):
	case (AL_PMDT_UNIT_UDMA_ETH_100_2):
	case (AL_PMDT_UNIT_UDMA_ETH_100_3):
	case (AL_PMDT_UNIT_EC_ETH_100_0):
	case (AL_PMDT_UNIT_EC_ETH_100_1):
	case (AL_PMDT_UNIT_EC_ETH_100_2):
	case (AL_PMDT_UNIT_EC_ETH_100_3):
	case (AL_PMDT_UNIT_SHARED_RESOURCE_ETH_100_0):
	case (AL_PMDT_UNIT_SHARED_CACHE_ETH_100_0):
	case (AL_PMDT_UNIT_PCIECORE_0):
	case (AL_PMDT_UNIT_PCIECORE_1):
	case (AL_PMDT_UNIT_PCIECORE_2):
	case (AL_PMDT_UNIT_PCIECORE_3):
	case (AL_PMDT_UNIT_PCIECORE_4):
	case (AL_PMDT_UNIT_PCIECORE_5):
	case (AL_PMDT_UNIT_PCIECORE_6):
	case (AL_PMDT_UNIT_PCIECORE_7):
		config->feature_mask = 0;
		break;
	default:
		break;
	}
}

#endif

/* Configure PMDT complex according to requested configuration.
 * Order of configuration as following:
 *  - Configure PMU inside the complex
 *  - Configure AXI monitor(s), if exist
 *  - Configure ELA unit(s)
 *  */
static int al_pmdt_unit_complex_config_aux(
	struct al_pmdt_handle *pmdt,
	struct al_pmdt_complex_handle *cmplx,
	enum al_pmdt_unit unit,
	struct al_pmdt_config *config)
{
	struct al_pmdt_map_entry entry;
	void __iomem *bs_regs = (void __iomem *)AL_PBS_REGFILE_BASE;
	struct al_pmdt_pmu_cmplx_info cmplx_info;
	int cpu_clust_num = 0;

	int rc;
	int axi_mon;
	int ela;
	uint8_t req_axi_mon_num;
	uint8_t req_ela_num;

	if (pmdt->map_func)
		bs_regs = pmdt->map_func(bs_regs,
				(void __iomem *)AL_SB_BASE, AL_PMDT_BOOTSTRAP_MAP_SIZE);

	al_memset(&entry, 0, sizeof(struct al_pmdt_map_entry));
	cmplx->cmplx_id = unit;
	entry.id = unit;

	if (!pmdt->bs.sb_clk_freq) {
		rc = al_bootstrap_parse(bs_regs, &pmdt->bs);
		al_assert(!rc);
	}

	rc = al_pmdt_map_entry_get(0, unit, &pmdt->bs, &entry);
	if (rc < 0) {
		al_err("pmdt[%d] %s: can't retrieve entry\n", unit, entry.name);
		return -ENODEV;
	}
	if (pmdt->map_func)
		entry.regs_base = pmdt->map_func(
				entry.regs_base, entry.bus_base, AL_PMDT_CMPLX_MAP_SIZE);

	al_dbg("Initialize PMDT %s unit\n", entry.name);
	al_memcpy(cmplx->name, entry.name, AL_PMDT_UNIT_NAME_LEN);
	cmplx->cctm_grp = entry.cctm_grp;
	cmplx->cctm_pmu_bit = entry.cctm_pmu_bit;
	cmplx->cctm_ela_bit = entry.cctm_ela_bit;

	/* Define which configuration structure will be used
	 * If function input config parameter is NULL,
	 * default configuration structure will be used for PMDT Complex
	 * configuration, otherwise  config will be used
	 * */
	al_memcpy(&cmplx->unit_config,
			(config == NULL) ? &entry.pmdtu_config : config,
			sizeof(struct al_pmdt_config));

	/* Skip unit initialization*/
	if (cmplx->unit_config.feature_mask == 0) {
		al_dbg("No active features, unit initialization will be skipped");
		return 0;
	}
	/* Init PMU and check complex info */
	rc = al_pmdt_pmu_handle_init(
			&cmplx->pmu,
			entry.name,
			entry.regs_base,
			AL_PMDT_HZ_TO_KHZ(entry.core_freq));
	al_assert(!rc);

	rc = al_pmdt_pmu_cmplx_info_get(&cmplx->pmu, &cmplx_info);
	al_assert(!rc);

	/* Validate correctness of required configuration */
	rc = al_pmdt_cmplx_configuration_verify(
			&cmplx->unit_config, &cmplx_info, &req_axi_mon_num, &req_ela_num);
	al_assert(!rc);
	al_dbg("pmdt %s: req_axi_mon_num = %d | req_ela_num = %d\n",
			entry.name, req_axi_mon_num, req_ela_num);

	/* program PMU */
	rc = al_pmdt_pmu_program(cmplx,  &cmplx_info);
	if (rc < 0) {
		if (rc == -ENODEV)
			al_err("pmdt %s: no monitoring support\n", entry.name);
		else
			al_err("pmdt %s: pmu prog fail\n", entry.name);

		return rc;
	}
	cmplx->aximon_active_mask = 0;
	for (axi_mon = 0; axi_mon < req_axi_mon_num; axi_mon++) {
		/* program AXI_MONITOR */
		rc = al_pmdt_axi_mon_program(cmplx, axi_mon, &entry);
		if (rc < 0) {
			al_err("pmdt %s: aximon#%d prog fail\n", entry.name, axi_mon);

			return rc;
		}

		cmplx->aximon_active_mask |= AL_BIT(axi_mon);
	}

	cmplx->ela_active_mask = 0;
	for (ela = 0; ela < req_axi_mon_num; ela++) {
		/* program ELA*/
		rc = al_pmdt_ela_program(cmplx, ela, &entry, &cmplx_info);
		if (rc < 0) {
			al_err("pmdt %s: ela#%d prog fail\n", entry.name, ela);

			return rc;
		}
		cmplx->ela_active_mask |= AL_BIT(ela);
	}

	cmplx->ready = AL_TRUE;
	if (config)
		cmplx->is_override = AL_TRUE;

	if ((cmplx->cmplx_id >= AL_PMDT_UNIT_CPU_CLUSTER_0) &&
			(cmplx->cmplx_id <= AL_PMDT_UNIT_CPU_CLUSTER_3)) {

		cpu_clust_num = cmplx->cmplx_id - AL_PMDT_UNIT_CPU_CLUSTER_0;
		al_pmdt_cpu_clust_tracking_enable(cpu_clust_num);
	}
	return 0;
}

static int al_pmdt_config_settings(
		struct al_pmdt_config *config,
		struct al_pmdt_special_setting *setting)
{
	int i;

	if (setting->bars.start && setting->bars.step) {
		for (i = 0; i < AL_PMDT_AXI_MON_BAR_NUM-1; i++) {
			config->latency_config.read_bar_thr[i] =
					setting->bars.start +
					(i * setting->bars.step);
			config->latency_config.write_bar_thr[i] =
					setting->bars.start +
					(i * setting->bars.step);
		}
	}
	return 0;
}

int al_pmdt_unit_config(
	struct al_pmdt_handle *pmdt,
	enum al_pmdt_unit unit,
	struct al_pmdt_config *config)
{
	struct al_pmdt_complex_handle *cmplx;
	int rc;

	al_assert(pmdt);
	al_assert(unit < AL_PMDT_UNIT_MAX);

	cmplx = &pmdt->cmplx[unit];
	al_memset(cmplx, 0, sizeof(struct al_pmdt_complex_handle));

	rc = al_pmdt_unit_complex_config_aux(pmdt, cmplx, unit, config);

	return rc;
}

int al_pmdt_start(
	struct al_pmdt_handle *pmdt,
	al_bool default_unit_enable,
	map_func_type map,
	al_bool enable_interrupt,
	struct al_pmdt_special_setting *setting)
{
	struct al_pmdt_complex_handle *cmplx;
	uint32_t pmu_mask[AL_PMDT_CCTM_MATRIX_LINE_SIZE] = {0};
	enum al_pmdt_unit unit;
	void __iomem *bs_regs = (void __iomem *)AL_PBS_REGFILE_BASE;
	struct al_pmdt_map_entry entry;
	struct al_pmdt_config config;
	int rc;

	al_assert(pmdt);

	al_dbg("**************************************************************\n"
		"********************     %s     *******************\n"
		"**************************************************************\n",
		__func__);

	pmdt->map_func = map;
	if (pmdt->map_func)
		bs_regs = pmdt->map_func(
				bs_regs,
				(void __iomem *)AL_SB_BASE, AL_PMDT_BOOTSTRAP_MAP_SIZE);

	if (setting && setting->units)
		al_memcpy(pmdt->active_units_mask,
				setting->units,
				AL_PMDT_MAP_ACTIVE_UNIT_ENTRIES * sizeof(unsigned int));
	else
		al_memset(pmdt->active_units_mask,
				0xFF,
				AL_PMDT_MAP_ACTIVE_UNIT_ENTRIES * sizeof(unsigned int));

	if (!pmdt->bs.sb_clk_freq) {
		rc = al_bootstrap_parse(bs_regs, &pmdt->bs);
		al_assert(!rc);
	}

	for (unit = 0; unit < AL_PMDT_UNIT_MAX; unit++) {
		cmplx = &pmdt->cmplx[unit];

		if (cmplx->ready == AL_FALSE) {
			if (default_unit_enable && al_pmdt_is_unit_active(pmdt, unit)) {
				if (setting) {
					rc = al_pmdt_map_entry_get(0, unit, &pmdt->bs, &entry);
					al_memcpy(&config,
							&entry.pmdtu_config,
							sizeof(struct al_pmdt_config));
					rc = al_pmdt_config_settings(&config, setting);
					al_assert(!rc);

					rc = al_pmdt_unit_complex_config_aux(
							pmdt, cmplx, unit, &config);
				} else
					rc = al_pmdt_unit_complex_config_aux(
							pmdt, cmplx, unit, NULL);
				al_assert(!rc);

				pmu_mask[cmplx->cctm_grp] |= AL_BIT(cmplx->cctm_pmu_bit);
				pmu_mask[cmplx->cctm_grp] |= AL_BIT(cmplx->cctm_ela_bit);
			}
		} else {
			if (cmplx->is_override == AL_TRUE)
				cmplx->is_override = AL_FALSE;

			pmu_mask[cmplx->cctm_grp] |= AL_BIT(cmplx->cctm_pmu_bit);
			pmu_mask[cmplx->cctm_grp] |= AL_BIT(cmplx->cctm_ela_bit);
		}
	}

	rc = al_pmdt_cctm_program(
			pmdt, pmdt->bs.sb_clk_freq, &pmu_mask, enable_interrupt);
	al_assert(!rc);

	al_data_memory_barrier();

	rc = al_pmdt_start_modules(pmdt, &pmu_mask);
	al_assert(!rc);

	return 0;
}

void al_pmdt_unit_pause(
	struct al_pmdt_handle *pmdt,
	enum al_pmdt_unit unit,
	al_bool is_pause)
{
	struct al_pmdt_complex_handle *cmplx;
	uint32_t pmu_mask[AL_PMDT_CCTM_MATRIX_LINE_SIZE] = {0};

	al_assert(pmdt);
	al_assert(unit < AL_PMDT_UNIT_MAX);

	cmplx = &pmdt->cmplx[unit];
	pmu_mask[cmplx->cctm_grp] |= AL_BIT(cmplx->cctm_pmu_bit);
	pmu_mask[cmplx->cctm_grp] |= AL_BIT(cmplx->cctm_ela_bit);

	al_pmdt_cctm_pmu_pause(&pmdt->cctm, is_pause, &pmu_mask);
}

int al_events_info_get(
	struct al_pmdt_handle *pmdt,
	enum al_pmdt_unit unit,
	struct al_pmdt_pmu_event_status event_status[AL_PMDT_PMU_MAX_EVENT_NUM])
{
	struct al_pmdt_pmu_cmplx_info cmplx_info;
	struct al_pmdt_pmu_event_status *status;
	int rc;
	unsigned int event_num;

	al_assert(pmdt);
	al_assert(event_status);
	al_assert(unit < AL_PMDT_UNIT_MAX);

	rc = al_pmdt_pmu_cmplx_info_get(&pmdt->cmplx[unit].pmu, &cmplx_info);
	if (rc)
		return rc;

	for (event_num = 0; event_num < cmplx_info.events_num; event_num++) {
		if ((cmplx_info.active_events & AL_BIT(event_num)) == 0)
				continue;
		status = &event_status[event_num];
		rc = al_pmdt_pmu_event_status_get(
				&pmdt->cmplx[unit].pmu, event_num, status);
		if (rc)
			return rc;

		if (status->active) {
			al_pmdt_pmu_event_counters_get(
					&pmdt->cmplx[unit].pmu, event_num, &status->counters);
		}

		cmplx_info.active_events_num--;
		if (cmplx_info.active_events_num == 0)
			break;
	}

	return 0;
}

void al_pmdt_ela_entry_parser(
		struct al_pmdt_axi_mon_trace *trace_cfg,
		struct al_pmdt_ela_ram_entry *entry_raw_data,
		struct al_pmdt_axi_trace_entry_data *entry_parsed_data)
{
	uint64_t field_mask;
	uint32_t field_shift;
	uint64_t payload_lsb;
	uint64_t payload_msb;
	uint32_t total_shifted = 0;

	al_assert(trace_cfg);
	al_assert(entry_raw_data);
	al_assert(entry_parsed_data);

	payload_lsb = entry_raw_data->payload_0;
	payload_msb = entry_raw_data->payload_1;

	al_memset(entry_parsed_data, 0,
			sizeof(struct al_pmdt_axi_trace_entry_data));

	entry_parsed_data->track_hit =
		(payload_msb >> AL_PMDT_AXI_TRACE_TRACK_HIT_FIELD_SHIFT) &
		AL_PMDT_AXI_TRACE_FIELD_MASK(
				AL_PMDT_AXI_TRACE_TRACK_HIT_FIELD_BIT_WIDTH);

	if (entry_raw_data->ts) {
		entry_parsed_data->is_ts = AL_TRUE;
		entry_parsed_data->timestamp[0] = payload_lsb;
		entry_parsed_data->timestamp[1] = payload_msb;

		return;
	}

	if (trace_cfg->id_width) {
		field_mask = AL_PMDT_AXI_TRACE_FIELD_MASK(trace_cfg->id_width);
		field_shift = trace_cfg->id_width;
		entry_parsed_data->id = payload_lsb & field_mask;

		payload_lsb = payload_lsb >> field_shift;
		payload_lsb |= ((payload_msb & field_mask) << (64 - field_shift));
		payload_msb = payload_msb >> field_shift;

		total_shifted += field_shift;
	}

	if (trace_cfg->addr_width) {
		field_mask = AL_PMDT_AXI_TRACE_FIELD_MASK(trace_cfg->addr_width*8);
		field_shift = trace_cfg->addr_width*8;
		entry_parsed_data->addr = payload_lsb & field_mask;

		payload_lsb = payload_lsb >> field_shift;
		payload_lsb |= ((payload_msb & field_mask) << (64 - field_shift));
		payload_msb = payload_msb >> field_shift;

		total_shifted += field_shift;
	}

	if (trace_cfg->tgtid_width) {
		field_mask = AL_PMDT_AXI_TRACE_FIELD_MASK(trace_cfg->tgtid_width*8);
		field_shift = trace_cfg->tgtid_width*8;
		entry_parsed_data->tgtid_addr = payload_lsb & field_mask;

		payload_lsb = payload_lsb >> field_shift;
		payload_lsb |= ((payload_msb & field_mask) << (64 - field_shift));
		payload_msb = payload_msb >> field_shift;

		total_shifted += field_shift;
	}

	if (trace_cfg->incl_attr) {
		field_mask = AL_PMDT_AXI_TRACE_FIELD_MASK(
				AL_PMDT_AXI_TRACE_ATTR_FIELD_BIT_WIDTH);
		field_shift = AL_PMDT_AXI_TRACE_ATTR_FIELD_BIT_WIDTH;
		entry_parsed_data->attr = payload_lsb & field_mask;

		payload_lsb = payload_lsb >> field_shift;
		payload_lsb |= ((payload_msb & field_mask) << (64 - field_shift));
		payload_msb = payload_msb >> field_shift;

		total_shifted += field_shift;
	}

	if (trace_cfg->user_width) {
		field_mask =
			AL_PMDT_AXI_TRACE_FIELD_MASK(trace_cfg->user_width);
		field_shift = trace_cfg->user_width;
		entry_parsed_data->axuser = payload_lsb & field_mask;

		payload_lsb = payload_lsb >> field_shift;
		payload_lsb |= ((payload_msb & field_mask) << (64 - field_shift));
		payload_msb = payload_msb >> field_shift;

		total_shifted += field_shift;
	}
	if (trace_cfg->incl_data) {
		field_mask =
			AL_PMDT_AXI_TRACE_FIELD_MASK(
					AL_PMDT_AXI_TRACE_DATA_FIELD_BIT_WIDTH);
		field_shift = AL_PMDT_AXI_TRACE_DATA_FIELD_BIT_WIDTH;
		entry_parsed_data->data = payload_lsb & field_mask;

		payload_lsb = payload_lsb >> field_shift;
		payload_lsb |= ((payload_msb & field_mask) << (64 - field_shift));
		payload_msb = payload_msb >> field_shift;

		total_shifted += field_shift;
	}

	if (trace_cfg->incl_bcount) {
		field_mask =
			AL_PMDT_AXI_TRACE_FIELD_MASK(
					AL_PMDT_AXI_TRACE_BCOUNT_FIELD_BIT_WIDTH);
		field_shift = AL_PMDT_AXI_TRACE_BCOUNT_FIELD_BIT_WIDTH;
		entry_parsed_data->bcount = payload_lsb & field_mask;

		payload_lsb = payload_lsb >> field_shift;
		payload_lsb |= ((payload_msb & field_mask) << (64 - field_shift));
		payload_msb = payload_msb >> field_shift;

		total_shifted += field_shift;
	}

	if (trace_cfg->incl_busy) {
		field_mask =
			AL_PMDT_AXI_TRACE_FIELD_MASK(
					AL_PMDT_AXI_TRACE_OUTSTAND_FIELD_BIT_WIDTH);
		field_shift = AL_PMDT_AXI_TRACE_OUTSTAND_FIELD_BIT_WIDTH;
		entry_parsed_data->outstand = payload_lsb & field_mask;

		payload_lsb = payload_lsb >> field_shift;
		payload_lsb |= (payload_msb & field_mask) << (64 - field_shift);
		payload_msb = payload_msb >> field_shift;

		total_shifted += field_shift;
	}
}
