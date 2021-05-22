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
 * @defgroup group_pmdt_axi_mon_api API
 * Performance monitoring debug and trace - AXI Monitor
 * @ingroup group_pmdt_axi_mon
 * @{
 * The AXI Monitor is part of a single PMU complex instance, embedded in a
 * specific HW core (there can be several monitors in a single complex, e.g. 2).
 * AXI transaction latencies are classified according to predefined histogram
 * bars. For each transaction the AXI MON triggers the matching bar event
 * toward the PMU event counters. The measured transactions can be filtered
 * according to predefined tracker patterns. There is an option to trace AXI
 * transactions via ELA tracer as well as getting a local snapshot log of the
 * monitor state.
 *
 * @file   al_hal_pmdt_axi_mon.h
 */

#ifndef __AL_HAL_PMDT_AXI_MON_H__
#define __AL_HAL_PMDT_AXI_MON_H__

#include "al_hal_common.h"

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */
/** number of latency histogram bars that are monitored by AXI MON */
#define AL_PMDT_AXI_MON_BAR_NUM			6
/** number of tracking filters for specific transaction types monitoring */
#define AL_PMDT_AXI_MON_TRACKER_NUM		4
/** tracker width in 32bits words equivalent to single transaction width */
#define AL_PMDT_AXI_MON_TRACKER_WIDTH		5
/** number of trace channels from AXI MON to ELA tracer */
/*
 * TODO: Currently, AL_PMDT_AXI_MON_TRACE_NUM is set to 1 in design.
 * Below value should be changed to 2 within design changing
 * */
#define AL_PMDT_AXI_MON_TRACE_NUM		1
/** number that represent the max work load counter of the monitor [0-128) */
#define AL_PMDT_AXI_MON_MAX_BUSY_CNT		128
/** AXI Monitor transaction tracking line size (in words, 5x32 = 160 bits) */
#define AL_PMDT_AXI_MON_TRACKING_LINE_SIZE	5

/** AXI latency log modes */
enum al_pmdt_axi_mon_log_mode {
	AL_AXI_MON_LOG_TRANS_LATENCY = 0, /** req accept till last resp accept */
	AL_AXI_MON_LOG_REQ_ACCEPT_LATENCY = 1, /** AxValid till AxReady */
	AL_AXI_MON_LOG_RESP_ACCEPT_LATENCY = 2, /** R/Bvalid till R/Bready */
};

/** AXI monitor special operation modes */
enum al_pmdt_axi_mon_special_op_mode {
	AL_AXI_MON_OP_MODE_NORMAL = 0, /** act normally and measure latencies */
	AL_AXI_MON_OP_MODE_LOAD = 1, /** measure monitor-load instead of latency */
	AL_AXI_MON_OP_MODE_LOAD_CONT = 2, /** measure monitor-load on each cycle */
};

/** AXI MON operation mode parameters */
struct al_pmdt_axi_mon_mode {
	/** read tracking (0- write tracking) */
	al_bool is_read;
	/** bit mask of tracker selection for latency logging condition(0-track all)*/
	unsigned int latency_tracker_sel;
	enum al_pmdt_axi_mon_log_mode log_mode;
	/** use cycle count terminology (0-use timestamp ticks) */
	al_bool cyc_cnt;
	/** log the first match (0-log the last match) */
	al_bool log_first;
	enum al_pmdt_axi_mon_special_op_mode special_op_mode;
};

/** AXI MON bar configuration parameters
 *  Note. time units are determined by al_pmdt_axi_mon_mode.cyc_cnt */
struct al_pmdt_axi_mon_bars {
	uint64_t bar_thr[AL_PMDT_AXI_MON_BAR_NUM-1]; /** x thresh. define x+1 bars*/
	uint64_t timeout;
};

/** AXI MON tracker configuration parameters */
struct al_pmdt_axi_mon_tracker {
	uint32_t mask[AL_PMDT_AXI_MON_TRACKER_WIDTH]; /** mask bits for tracking */
	uint32_t comp[AL_PMDT_AXI_MON_TRACKER_WIDTH]; /** tracked value */
};

#define AL_PMDT_AXI_TRACE_ATTR_FIELD_BIT_WIDTH			26	/* 26 bits */
#define AL_PMDT_AXI_TRACE_DATA_FIELD_BIT_WIDTH			32	/* 8 bytes lsb*/
#define AL_PMDT_AXI_TRACE_BCOUNT_FIELD_BIT_WIDTH		10	/* 10 bits */
#define AL_PMDT_AXI_TRACE_OUTSTAND_FIELD_BIT_WIDTH		32	/* 32 bits */
#define AL_PMDT_AXI_TRACE_TRACK_HIT_FIELD_BIT_WIDTH		4	/* 4 bits */
#define AL_PMDT_AXI_TRACE_TRACK_HIT_FIELD_SHIFT			59

/** AXI MON trace configuration parameters
 *  There are 160bits per transaction and only 128bits per trace vector. The
 *  user can choose which 128bits will be reflected by setting the below params.
 *  The general structure of trace vector is as follows:
 *  (second row - size in bits.'param' == defined in the below parameters)
 *------------------------------------------------------------------------------
 *|valid|track hit|busy|bcount| data | AxUser | attr | tgtid  | address | AxID  |
 *|  1  |    4    |0/32| 0/10 | 0/32 | param  | 0/26 | param | param   | param |
 *------------------------------------------------------------------------------
 *
 * */
struct al_pmdt_axi_mon_trace {
	unsigned int tracker_sel; /** bit mask of tracker selection (0-trace all)*/
	al_bool incl_busy; /** include busy field */
	al_bool incl_bcount; /** include byte count field */
	al_bool incl_data; /** include data field (last response low word) */
	unsigned int user_width; /** AxUser field width in bits */
	al_bool incl_attr; /** include attributes field */
	unsigned int tgtid_width; /** tgtid bytes (if partial address was selected) */
	unsigned int addr_width; /** address field width in bytes */
	unsigned int id_width; /** AxID width in bits */
};

/** AXI MON log entry */
struct al_pmdt_axi_mon_log_entry {
	uint64_t latency; /** request latency */
	uint64_t address; /** request address */
	uint64_t attr; /** request attributes */
};

/** AXI MON context */
struct al_pmdt_axi_mon {
	const char *name;
	unsigned int id;
	void __iomem *regs;
};

/**
 * Initialize the AXI Monitor handle
 *
 * @param pmdt_axi_mon
 *		An allocated, non-initialized instance
 * @param core_name
 *		The name of the core AXI MON is integrated into
 *		Upper layer must keep string area
 * @param id
 *      The AXI MON instance ID
 * @param pmu_cmplx_base
 *		The base address of the PMU complex registers
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_pmdt_axi_mon_handle_init(
		struct al_pmdt_axi_mon *pmdt_axi_mon,
		const char *core_name,
		unsigned int id,
		void __iomem *pmu_cmplx_base);

/**
 * Configure AXI Monitor mode of operation
 *
 * @param pmdt_axi_mon
 *		AXI MON handle
 * @param mode
 *		Mode configuration parameters
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_pmdt_axi_mon_mode_config(
		struct al_pmdt_axi_mon *pmdt_axi_mon,
		const struct al_pmdt_axi_mon_mode *mode);

/**
 * Retrieve AXI Monitor monitoring mode (read or write)
 *
 * @param pmdt_axi_mon
 *		AXI MON handle
 *
 * @return
 *		AL_TRUE if Read Mode
 *		AL_FALSE if Write Mode
 */
al_bool al_pmdt_axi_mon_is_read(
		struct al_pmdt_axi_mon *pmdt_axi_mon);

/**
 * Configure AXI Monitor latency bars
 *
 * @param pmdt_axi_mon
 *		AXI MON handle
 * @param bars
 *		Latency bars configuration parameters
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_pmdt_axi_mon_bars_config(
		struct al_pmdt_axi_mon *pmdt_axi_mon,
		const struct al_pmdt_axi_mon_bars *bars);

/**
 * Configure AXI Monitor tracker
 *
 * @param pmdt_axi_mon
 *		AXI MON handle
 * @param tracker_id
 *		Tracker ID (between 0 and AL_PMDT_AXI_MON_TRACKER_NUM-1)
 * @param tracker
 *		Tracker configuration parameters
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_pmdt_axi_mon_tracker_config(
		struct al_pmdt_axi_mon *pmdt_axi_mon,
		int tracker_id,
		const struct al_pmdt_axi_mon_tracker *tracker);

/**
 * Enable/disable AXI Monitor
 *
 * @param pmdt_axi_mon
 *		AXI MON handle
 * @param is_enable
 *		0/1 - disable/enable
 */
void al_pmdt_axi_mon_enable(
		struct al_pmdt_axi_mon *pmdt_axi_mon,
		al_bool is_enable);

/**
 * Clear AXI MON results and database
 *
 * @param pmdt_axi_mon
 *		PMU handle
 * @param tracker_res
 *		Clear tracker results
 * @param tracker_db
 *		Clear tracker database
 * @param timeout_log
 *		Clear timeout log
 */
void al_pmdt_axi_mon_clear(
		struct al_pmdt_axi_mon *pmdt_axi_mon,
		al_bool tracker_res,
		al_bool tracker_db,
		al_bool timeout_log);

/**
 * Retrieve current time stamp
 *
 * - time stamp configuration is done via CCTM to all units
 *
 * @param pmdt_axi_mon
 *		AXI MON handle
 *
 * @return current time stamp
 */
uint64_t al_pmdt_axi_mon_timestamp_get(struct al_pmdt_axi_mon *pmdt_axi_mon);

/**
 * Retrieve AXI MON log
 *
 * @param pmdt_axi_mon
 *		AXI MON handle
 * @param last
 *		log of last event per latency bar (set to NULL if not needed)
 * @param max
 *		log of max latency event (set to NULL if not needed)
 * @param timeout
 *		log of last timeout event (set to NULL if not needed)
 */
void al_pmdt_axi_mon_log_get(
		struct al_pmdt_axi_mon *pmdt_axi_mon,
		struct al_pmdt_axi_mon_log_entry (*last)[AL_PMDT_AXI_MON_BAR_NUM],
		struct al_pmdt_axi_mon_log_entry *max,
		struct al_pmdt_axi_mon_log_entry *timeout);

/**
 * Retrieve AXI MON busy level
 *
 * A number between 0 and AL_PMDT_AXI_MON_MAX_BUSY_CNT that denotes the monitor
 * stress level. Higher number means higher work load.
 * In other words - if work load gets to AL_AXI_MON_MAX_BUSY_CNT then the
 * monitor may lose information and its measurements shall not be trusted.
 *
 * @param pmdt_axi_mon
 *		AXI MON handle
 *
 * @return current busy level
 */
uint32_t al_pmdt_axi_mon_busy_level_get(struct al_pmdt_axi_mon *pmdt_axi_mon);

/**
 * Configure AXI Monitor trace via ELA
 *
 * @param pmdt_axi_mon
 *		AXI MON handle
 * @param trace_id
 *		Trace ID (between 0 and AL_PMDT_AXI_MON_TRACE_NUM-1)
 * @param trace
 *		Trace configuration parameters
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_pmdt_axi_mon_trace_config(
		struct al_pmdt_axi_mon *pmdt_axi_mon,
		int trace_id,
		const struct al_pmdt_axi_mon_trace *trace);

/**
 * Get current AXI Monitor trace configuration
 *
 * @param pmdt_axi_mon
 *		AXI MON handle
 * @param trace_id
 *		Trace ID (between 0 and AL_PMDT_AXI_MON_TRACE_NUM-1)
 * @param trace (out)
 *		Trace configuration parameters
 */
void al_pmdt_axi_mon_trace_config_get(
		struct al_pmdt_axi_mon *pmdt_axi_mon,
		int trace_id,
		struct al_pmdt_axi_mon_trace *trace);

/**
 * Enable/disable AXI Monitor trace
 *
 * @param pmdt_axi_mon
 *		AXI MON handle
 * @param trace_id
 *		Trace ID to enable
 * @param is_enable
 *		0/1 - disable/enable
 */
void al_pmdt_axi_mon_trace_enable(
		struct al_pmdt_axi_mon *pmdt_axi_mon,
		int trace_id,
		al_bool is_enable);

/* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */

#endif /* __AL_HAL_PMDT_AXI_MON_H__ */
/** @} end of group_pmdt_axi_mon */

