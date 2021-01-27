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
#ifndef __AL_PMDT_CONFIG_H__
#define __AL_PMDT_CONFIG_H__

#include "al_hal_pmdt_pmu.h"
#include "al_hal_pmdt_axi_mon.h"
#include "al_hal_pmdt_ela_config.h"
#include "al_hal_pmdt_ela.h"
#include "al_hal_pmdt_cctm.h"
#include "al_hal_pmdt_map_regs.h"
#include "al_hal_bootstrap.h"

/* PMDT complex related definitions */
#define AL_PMDT_UNIT_NAME_LEN 30

#define AL_PMDT_INSTANCE_0 0x1
#define AL_PMDT_INSTANCE_1 0x2

/* PMDT time stamp period in usec */
#define AL_PMDT_TS_PERIOD_USEC 10

#define AL_PMDT_HZ_TO_KHZ(x) ((x) / 1000)

#define AL_PMDT_MAP_ACTIVE_UNIT_ENTRIES ((AL_PMDT_UNIT_MAX) / 32)

/** PMU internal trigger types */
enum al_pmdt_pmu_int_trig_type {
	AL_PMDT_INT_TRIG_LATENCY, /** AXI_MON latency bar triggers */
	AL_PMDT_INT_TRIG_TRACKER_INC_1, /** tracker match trigger, +1 per event */
	AL_PMDT_INT_TRIG_TRACKER_INC_VAL, /** tracker match trigger, +inc_val */
};

/** PMU interrupts triggered by AXI Monitor (Group B) */
enum al_pmdt_axi_mon_irq_type {
	AL_PMDT_AXIMON_TIMEOUT_IRQ,
	AL_PMDT_AXIMON_OVERFLOW_IRQ,
	AL_PMDT_AXIMON_UNDERFLOW_IRQ,
};

/**  */
enum al_pmdt_trapping_filter_type {
	AL_PMDT_AXIMON_FILTER_ALL		= 0,
	AL_PMDT_AXIMON_WR_FILTER_ADDR		= AL_BIT(0),
	AL_PMDT_AXIMON_WR_FILTER_M2S_COMP	= AL_BIT(1),
	AL_PMDT_AXIMON_WR_FILTER_S2M_COMP	= AL_BIT(2),
	AL_PMDT_AXIMON_WR_FILTER_DATA		= AL_BIT(3),
	AL_PMDT_AXIMON_WR_FILTER_MSIX		= AL_BIT(4),
	AL_PMDT_AXIMON_RD_FILTER_ADDR		= AL_BIT(5),
	AL_PMDT_AXIMON_RD_FILTER_M2S_DESC	= AL_BIT(6),
	AL_PMDT_AXIMON_RD_FILTER_S2M_DESC	= AL_BIT(7),
	AL_PMDT_AXIMON_RD_FILTER_DATA		= AL_BIT(8),
};

/* PMDT features */
enum al_pmdt_features {
	AL_PMDT_FEATURE_AXI_TIMEOUT		= AL_BIT(0),
	AL_PMDT_FEATURE_AXI_LATENCY_HISTOGRAM	= AL_BIT(1),
	AL_PMDT_FEATURE_AXI_TRAP		= AL_BIT(2),
	AL_PMDT_FEATURE_AXI_TRACK		= AL_BIT(3),
	AL_PMDT_FEATURE_EXT_TRIG_TRACK		= AL_BIT(4),
};

/* AXI monitor direction */
enum al_pmdt_monitor_direction {
	AL_PMDT_MONITOR_READ_DIR,
	AL_PMDT_MONITOR_WRITE_DIR,
	AL_PMDT_MONITOR_READ_WRITE_DIR,
};

/* AXI trap event types */
enum al_pmdt_trap_events {
	AL_PMDT_TRAP_SLV_ERR_EVENT,
	AL_PMDT_TRAP_DEC_ERR_EVENT,
	AL_PMDT_TRAP_SLV_DEC_ERR_EVENT,
	AL_PMDT_TRAP_CUSTOM_EVENT,
};

/* AXI tracking type */
enum al_pmdt_track_type {
	AL_PMDT_TRACK_BANDWIDTH,
	AL_PMDT_TRACK_OCCURRENCES,
	AL_PMDT_TRACK_BYTE_COUNT,
};

/* HW register space sizes - for mapping purposes */
#define AL_PMDT_BOOTSTRAP_MAP_SIZE	(4*1024)
#define AL_PMDT_CCTM_MAP_SIZE		(4*1024)
#define AL_PMDT_CMPLX_MAP_SIZE		(16*1024)

/* optional map function callback to be used when iomem regs shall be mapped
 * addr - the peripheral address (as it's seen from local cpu)
 * bus_addr - the base address of the peripheral bus (SB, NB, etc.)
 * size - mapping size
 */
typedef void __iomem * (*map_func_type)(
		void __iomem *addr, void __iomem *bus_addr, unsigned int size);

/* AXI monitor trace entry pushed in/pulled from ELA memory*/
struct al_pmdt_axi_trace_entry_data {
	uint8_t id;
	uint32_t addr;
	uint16_t tgtid_addr;
	uint32_t attr;
	uint32_t axuser;
	uint32_t data;
	uint16_t bcount;
	uint32_t outstand;
	uint8_t track_hit;
	uint64_t timestamp[2];
	al_bool is_ts;
};

/* pmdt feature configuration - axi timeout */
struct al_pmdt_feature_timeout {
	unsigned int read_timeout_us; /* timeout definition in usec */
	unsigned int write_timeout_us; /* timeout definition in usec */
	enum al_pmdt_monitor_direction direction; /* AXI direction to monitor tout*/
};

/* pmdt feature configuration - axi latency */
struct al_pmdt_feature_latency {
	enum al_pmdt_monitor_direction direction; /* AXI direction to monitor tout*/
	uint64_t read_bar_thr[AL_PMDT_AXI_MON_BAR_NUM-1]; /* histogram bar thresholds */
	uint64_t write_bar_thr[AL_PMDT_AXI_MON_BAR_NUM-1]; /* histogram bar thresholds */
};

/* pmdt feature configuration - axi trap */
struct al_pmdt_feature_trap {
	enum al_pmdt_monitor_direction direction;
	enum al_pmdt_trap_events trap_event;
	struct al_pmdt_axi_mon_tracker trap_mask;/*relevant for custom event only*/
	/* position of AXI response field inside 160 bit AXI transaction*/
	unsigned int axi_resp_field_position;
	al_bool is_trace; /* collect transaction history trace till trapping event*/
};

/* pmdt feature configuration - axi track
 * User shall register al_pmdt_isr to catch bound crossing. If stop_n_trace is
 * enabled the trace info is reflected via isr info */
struct al_pmdt_feature_track {
	enum al_pmdt_monitor_direction direction;
	al_bool is_track_all; /* track all valid AXI transactions */
	/* number of filter responsible for tracking any AXI transaction.
	 * -1 - not defined */
	int	all_track_filter_num;
	struct al_pmdt_axi_mon_tracker rtrack_mask;/* relevant when is_track_all=0 */
	struct al_pmdt_axi_mon_tracker wtrack_mask;/* relevant when is_track_all=0 */
	enum al_pmdt_track_type track_type;
	uint64_t upper_bound; /* high threshold for catching high peaks */
	uint64_t lower_bound; /* low threshold for catching low peaks */
	unsigned int sampling_window_us; /* event sampling window in usec */
	al_bool stop_on_cross; /*stop tracking on bound crossing */
};

/* pmdt configuration for specific HW unit
 *
 * Note. For each enabled feature in feature_mask, the caller can provide a
 * specific feature configuration or keep it as NULL. In case it is NULL then
 * the configuration will be taken from pmdt_map.
 */
struct al_pmdt_config {
	unsigned int feature_mask; /* mask of al_pmdt_features */
	struct al_pmdt_feature_timeout timeout_config;
	struct al_pmdt_feature_latency latency_config;
	struct al_pmdt_feature_trap trap_config;
	struct al_pmdt_feature_track track_config;
	al_bool default_axi_read;
};

/* events map inside PMU */
struct al_pmdt_pmu_events_map {
	int aximon_latency_frst_event_num[AL_PMDT_AXI_MON_MAX_NUM];
	int aximon_trap_event_num[AL_PMDT_AXI_MON_MAX_NUM];
	int aximon_track_event_num[AL_PMDT_AXI_MON_MAX_NUM];
};

/* al_pmdt single pmdt complex context handle */
struct al_pmdt_complex_handle {
	enum al_pmdt_unit cmplx_id;
	char name[AL_PMDT_UNIT_NAME_LEN];
	struct al_pmdt_pmu pmu;
	struct al_pmdt_axi_mon axi_mon[AL_PMDT_AXI_MON_MAX_NUM];
	struct al_pmdt_elacfg elacfg;
	struct al_pmdt_ela ela[AL_PMDT_ELA_MAX_NUM];
	unsigned int cctm_grp;
	unsigned int cctm_pmu_bit;
	unsigned int cctm_ela_bit;
	struct al_pmdt_config unit_config;
	unsigned int event_bars_number;
	struct al_pmdt_pmu_events_map events_map;
	uint32_t aximon_active_mask;
	uint32_t ela_active_mask;
	al_bool ready;
	al_bool is_override;
};

/* special axi monitor latency bar setting */
struct al_pmdt_bar_setting {
	uint64_t start;
	uint64_t step;
};

struct al_pmdt_special_setting {
	uint32_t units[AL_PMDT_MAP_ACTIVE_UNIT_ENTRIES]; /* bitmap of active units*/
	struct al_pmdt_bar_setting bars;
};

/* al_pmdt context handle */
struct al_pmdt_handle {
	struct al_pmdt_cctm cctm;
	struct al_pmdt_complex_handle cmplx[AL_PMDT_UNIT_MAX];
	struct al_bootstrap bs;
	map_func_type map_func;
	unsigned int active_units_mask[AL_PMDT_MAP_ACTIVE_UNIT_ENTRIES];
};

/**
 * PMDT configuration of specific HW unit
 *
 * @param pmdt
 *		pmdt handle
 * @param unit
 *		hardware unit to be configured
 * @param config
 *		pmdt configuration
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_pmdt_unit_config(
		struct al_pmdt_handle *pmdt,
		enum al_pmdt_unit unit,
		struct al_pmdt_config *config);

/**
 * Start PMDT monitoring
 *
 * Note. this function replaces the al_pmdt_monitor function. It assumes the
 * caller configured the required HW units using al_pmdt_unit_config.
 *
 * @param pmdt
 *		pmdt handle
 * @param default_unit_enable
 *		enable units that were configured by al_pmdt_unit_config or not.
 *		In case of true, those units will be configured according to pmdt_map.
 * @param map
 *      optional map function callback (use NULL if no mapping is required)
 * @param enable_interrupt
 *      enable CCTM interrupt
 * @param setting
 *      special pmdt settings that override the default ones (use NULL for none)
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_pmdt_start(
		struct al_pmdt_handle *pmdt,
		al_bool default_unit_enable,
		map_func_type map,
		al_bool enable_interrupt,
		struct al_pmdt_special_setting *setting);

/**
 * Pause/resume PMDT Complex
 *
 * @param pmdt
 *		pmdt handle
 * @param unit
 *		hardware unit to be paused/resumed
 * @param is_pause
 *		Pause i
 */
void al_pmdt_unit_pause(
		struct al_pmdt_handle *pmdt,
		enum al_pmdt_unit unit,
		al_bool is_pause);

/**
 * Retrieve current status for all active events inside PMU
 *
 * @param pmdt
 *		pmdt handle
 * @param unit
 *		requested pmdt unit
 * @param event_status (out)
 *		events status
 *
 * @return	0 if finished successfully
 *			<0 if an error occurred (errno value)
 */
int al_events_info_get(
		struct al_pmdt_handle *pmdt,
		enum al_pmdt_unit unit,
		struct al_pmdt_pmu_event_status event_status[AL_PMDT_PMU_MAX_EVENT_NUM]);

/**
 * Parse ELA memory entry to fields according to configured trace configuration
 *
 * @param trace_cfg
 *		pmdt handle
 * @param entry_raw_data (in)
 *		raw entry data, as was retrieved from ELA memory
 * @param entry_parsed_data (out)
 *		parsed entry data
 */
void al_pmdt_ela_entry_parser(
		struct al_pmdt_axi_mon_trace *trace_cfg,
		struct al_pmdt_ela_ram_entry *entry_raw_data,
		struct al_pmdt_axi_trace_entry_data *entry_parsed_data);

#endif /*__AL_PMDT_CONFIG_H__ */
