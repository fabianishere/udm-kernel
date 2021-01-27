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
#ifndef __AL_PMDT_H__
#define __AL_PMDT_H__

#include "al_pmdt_config.h"
#include "al_hal_pmdt_pmu.h"
#include "al_hal_pmdt_axi_mon.h"
#include "al_hal_pmdt_ela_config.h"
#include "al_hal_pmdt_ela.h"
#include "al_hal_pmdt_cctm.h"
#include "al_hal_pmdt_map.h"

/**
 * @defgroup group_pmdt PMDT
 * @file al_pmdt.h
 * @brief Header file for the PMDT Service
 *
 */

enum al_pmdt_error_type {
	AL_PMDT_ISR_NO_ERROR		= 0,
	AL_PMDT_ISR_AXI_TRANS_ERROR = AL_BIT(0),/* SLV/DEC Error*/
	AL_PMDT_ISR_AXI_TIME_OUT	= AL_BIT(1), /* AXI transaction Timeout error  */
	AL_PMDT_ISR_AXI_OVERFLOW	= AL_BIT(2), /* AXI Monitor Overflow*/
	AL_PMDT_ISR_AXI_UNDERFLOW	= AL_BIT(3), /* AXI Monitor Underflow*/
	AL_PMDT_ISR_AXI_TRACK_UPPER_BOUND_CROSS	= AL_BIT(4), /*  */
	AL_PMDT_ISR_AXI_TRACK_LOWER_BOUND_CROSS	= AL_BIT(5),
};

enum al_pmdt_ela_memory_dump_resolution {
	AL_PMDT_DUMP_LAST_1 = 0, /* last retrieved entry will be printed */
	AL_PMDT_DUMP_LAST_5 = 1, /* last 5 retrieved entries will be printed */
	AL_PMDT_DUMP_LAST_10 = 2,/* last 10 retrieved entries will be printed */
	AL_PMDT_DUMP_LAST_QUATER = 3,
	AL_PMDT_DUMP_LAST_HALF = 4,
	AL_PMDT_DUMP_ALL = 5, /* all retrieved entries will be printed */
};

struct al_pmdt_track_err_info {
	uint64_t counter_value;
	al_bool is_read;
};

struct al_pmdt_err_info {
	struct al_pmdt_axi_mon_log_entry read_timeout_info;
	al_bool read_timeout_info_valid; /* if FALSE - no available read timeout log */
	struct al_pmdt_axi_mon_log_entry write_timeout_info;
	al_bool write_timeout_info_valid; /* if FALSE - no available write timeout log */
	/* Following array represents read ELA memory state
	 * at moment of retrieving error information from PMDTU
	 * First entry in the array is a oldest entry in ELA and
	 * rentries_num parameter points to latest(newest) one
	 */
	struct al_pmdt_ela_ram_entry read_trace_info[AL_PMDT_ELA_RAM_MAX_ENTRIES];
	int rentries_num; /* if -1, no available read logs in ELA */
	/* Following array represents write ELA memory state
	 * at moment of retrieving error information from PMDTU
	 * First entry in the array is a oldest entry in ELA and
	 * wentries_num parameter points to latest(newest) one
	 */
	struct al_pmdt_ela_ram_entry write_trace_info[AL_PMDT_ELA_RAM_MAX_ENTRIES];
	int wentries_num;  /* if -1, no available write logs in ELA */
	struct al_pmdt_track_err_info track_err_info;
};

struct al_pmdt_isr_info {
	enum al_pmdt_unit pmdt_unit; /* pmdt unit that caused the isr */
	unsigned int err; /* mask of enum al_pmdt_error_type */
	struct al_pmdt_err_info err_info; /* error info (according to error type) */
};

/**
 * Optional map function callback to be used when iomem regs shall be mapped
 * addr - the peripheral address (as it's seen from local cpu)
 * bus_addr - the base address of the peripheral bus (SB, NB, etc.)
 * size - mapping size
 */
typedef void __iomem * (*map_func_cb_type)(
		void __iomem *addr, void __iomem *bus_addr, unsigned int size);

/**
 * Start PMDT monitoring of AXI transactions on all selected PMDT units
 *
 * @param pmdt
 *		pmdt handle
 * @param map
 *      optional map function callback (use NULL if no mapping is required)
 * @param enable_interrupt
 *      enable pmdt interrupt (in CCTM not in GIC)
 * @param setting
 *      special pmdt settings that override the default ones (use NULL for none)
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_pmdt_monitor(
		struct al_pmdt_handle *pmdt,
		map_func_cb_type map,
		al_bool enable_interrupt,
		struct al_pmdt_special_setting *setting);

/**
 * PMDT ISR to be called from IRQ handler
 *
 * @param pmdt
 *		pmdt handle
 * @param info
 *		isr info (out)
 * @return	0 if finished successfully
 *		1 if there are still pending irq sources (need to re-call till 0)
 *		<0 if an error occurred (errno value)
 */
int al_pmdt_isr(
		struct al_pmdt_handle *pmdt,
		struct al_pmdt_isr_info *info);

/**
 * Print PMDT unit name
 *
 * @param pmdt_unit
 *		requested pmdt unit
 */
void al_pmdt_unit_name_print(
		enum al_pmdt_unit pmdt_unit);

/**
 * Print PMDT unit info
 *
 * @param pmdt
 *		pmdt handle
 * @param pmdt_unit
 *		requested pmdt unit
 */
int al_pmdt_info_print(
	struct al_pmdt_handle *pmdt,
	enum al_pmdt_unit pmdt_unit);

/**
 * Retrieve total transferred byte count of specific unit
 *
 * @param pmdt
 *		pmdt handle
 * @param pmdt_unit
 *		requested pmdt unit
 * @param count (out)
 *      total read and write transferred bytes
 */
int al_pmdt_byte_count_get(
	struct al_pmdt_handle *pmdt,
	enum al_pmdt_unit pmdt_unit,
	uint64_t *count);

/**
 * Retrieve current AXI latency histogram
 *
 * @param pmdt
 *		pmdt handle
 * @param pmdt_unit
 *		requested pmdt unit
 * @param bin_num
 *		number of sampled bins
 *		- in case <AL_PMDT_AXI_MON_BAR_NUM it means that long latencies are
 *		accumulated in the last bin
 * @param read_hist
 *		read latency histogram
 * @param write_hist
 *		write latency histogram
 *
 * @return	0 if finished successfully
 *			<0 if an error occurred (errno value)
 *			in case of -EBUSY just retry few cycles later
 */
int al_latency_histograms_get(
		struct al_pmdt_handle *pmdt,
		enum al_pmdt_unit pmdt_unit,
		unsigned int *bin_num,
		uint64_t (*read_hist)[AL_PMDT_AXI_MON_BAR_NUM],
		uint64_t (*write_hist)[AL_PMDT_AXI_MON_BAR_NUM]);

/**
 * Print PMDT unit AXI latency histograms
 *
 * @param pmdt
 *		pmdt handle
 * @param pmdt_unit
 *		requested pmdt unit
 */
int al_latency_histograms_print(
		struct al_pmdt_handle *pmdt,
		enum al_pmdt_unit pmdt_unit);

/**
 * Print current ELA memory
 *
 * @param pmdt
 *		pmdt handle
 * @param pmdt_unit
 *		requested pmdt unit
 * @param ela_instance
 *		requested ela instance
 *		Used when ela_ram_in == NULL
 * @param ela_ram_in
 *		ELA data to print
 *		If NULL, data will be retrieved inside the function
 * @param num_of_entries
 *		total retrieved memory size (entries)
 *		Relevant when  ela_ram_in != NULL
 * @param dump_res
 *		defines amount (number of entries) of ELA memory to be dump
 * @param print_parsed_data
 *		defines if memory will be printed as was logged to ELA (raw data)
 *		or will be parsed according to trace configuration
 *
 * @return	0 if finished successfully
 *			<0 if an error occurred (errno value)
 *			in case of -EBUSY just retry few cycles later
 */
int al_ela_history_print(
		struct al_pmdt_handle *pmdt,
		enum al_pmdt_unit pmdt_unit,
		unsigned int ela_instance,
		struct al_pmdt_ela_ram_entry *ela_ram_in,
		unsigned int num_of_entries,
		enum al_pmdt_ela_memory_dump_resolution dump_res,
		al_bool print_parsed_data);

#endif
