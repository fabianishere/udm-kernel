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
#include "stdlib.h"
#include "al_hal_iomap.h"

/**
 * @defgroup group_pmdt_samples Code Samples
 * @ingroup group_pmdt
 * @{
 * pmdt.c: this file can be found in samples directory.
 */

/*
 * -------------------------------  PMDT Example  -----------------------------
 *
 * Assumed HW Configuration:
 *
 * Core A:
 * ---------     ----------------     ---------
 * | PMU_A |<----| AXI_Monitor_A|---->| ELA_A |
 * ---------     ----------------     ---------
 *          \
 *           \
 *         --------
 *         | CCTM |
 *         --------
 *           /
 * Core B:  /
 * ---------     ----------------
 * | PMU_B |<----| AXI_Monitor_B|
 * ---------     ----------------
 *
 * Story:
 * - AXI_Monitor_A monitors core A read latency from address 0x1000
 *   (in 100ns quantum)
 * - PMU_A count the monitor results in 6 counters, each represents histogram
 *   bar of 100ns
 * - All tracked read requests are logged in ELA_A trace in a cyclic RAM
 * - In parallel, AXI_Monitor_B waits for Core B write access to address 0x2000
 * - When it happens PMU_B send pause trigger to PMU_A via CCTM and fires
 *   an interrupt (in this example we configure the interrupt but use polling
 *   for simplicity)
 * - When PMU_A is stopped, we print ELA_A trace data. In addition we print
 *   read latency histogram results from PMU_A.
 */
#include "samples.h"
#include "al_hal_pmdt_pmu.h"
#include "al_hal_pmdt_axi_mon.h"
#include "al_hal_pmdt_ela_config.h"
#include "al_hal_pmdt_ela.h"
#include "al_hal_pmdt_cctm.h"

/* PMDT base addresses (all addresses are fiction...) */
#define CORE_A_BASE_ADDR 0xFA000000
#define CORE_B_BASE_ADDR 0xFB000000
#define CCTM_BASE_ADDRESS 0xFC000000
#define PMU_COMPLEX_OFFSET 0x10000 /* offset from CORE base address */

#define PMU_COMPLEX_A_BASE_ADDRESS (CORE_A_BASE_ADDR + PMU_COMPLEX_OFFSET)
#define PMU_COMPLEX_B_BASE_ADDRESS (CORE_B_BASE_ADDR + PMU_COMPLEX_OFFSET)

/* Auxiliary definitions */
#define PMU_A_CLOCK_KHZ 1000000 /* 1GHz */
#define PMU_B_CLOCK_KHZ 2000000 /* 2GHz */
#define CCTM_CLOCK_KHZ	1000000	/* 1GHz */
#define INFINITE (1ULL << (63))

/* PMDT handles */
static struct al_pmdt_pmu pmu_a;
static struct al_pmdt_axi_mon axi_mon_a;
static struct al_pmdt_elacfg elacfg_a;
static struct al_pmdt_ela ela_a;
static struct al_pmdt_pmu pmu_b;
static struct al_pmdt_axi_mon axi_mon_b;
static struct al_pmdt_cctm cctm;


int pmdt_config_test(void)
{
	int rc = 0;
	struct al_pmdt_axi_mon_mode axi_mon_mode;
	struct al_pmdt_axi_mon_bars bars;
	struct al_pmdt_axi_mon_tracker tracker;
	struct al_pmdt_axi_mon_trace trace;
	struct al_pmdt_pmu_config pmu_config;
	struct al_pmdt_pmu_event_config event_config;
	struct al_pmdt_elacfg_config elacfg_config;
	struct al_pmdt_ela_config ela_config;
	struct al_pmdt_ela_trig_config ela_trig_state;
	uint32_t pmu_mask[AL_PMDT_CCTM_MATRIX_LINE_SIZE] = {0};
	int i;

	/*******************************/
	/* AXI Monitor A configuration */
	/*******************************/
	rc |= al_pmdt_axi_mon_handle_init(&axi_mon_a, "Core A", 0,
			(void __iomem *)PMU_COMPLEX_A_BASE_ADDRESS);
	/* set monitor mode */
	axi_mon_mode.is_read = 1; /* we want to monitor read requests */
	axi_mon_mode.latency_tracker_sel = 0;
	axi_mon_mode.log_mode = AL_AXI_MON_LOG_TRANS_LATENCY; /* full latency */
	axi_mon_mode.cyc_cnt = 1; /* we want fine granularity (e.g. nano-sec) */
	axi_mon_mode.log_first = 0; /* don't care */
	rc |= al_pmdt_axi_mon_mode_config(&axi_mon_a, &axi_mon_mode);

	/* set histogram bars */
	for (i = 0; i < AL_PMDT_AXI_MON_BAR_NUM-1; i++)
		bars.bar_thr[i] = i * 100; /* local clock 1GHz and we want 100ns bars */

	bars.timeout = INFINITE; /* we don't define timeouts */
	rc |= al_pmdt_axi_mon_bars_config(&axi_mon_a, &bars);

	/* set tracker 0 pattern */
	memset(&tracker, 0, sizeof(struct al_pmdt_axi_mon_tracker));
	/* we assume address size of 48 bits and AxID size of 4 bits */
	tracker.mask[0] = 0xFFFFFFF0;
	tracker.mask[1] = 0x000FFFFF;
	tracker.comp[0] = 0x1000 << 4; /* we monitor read address 0x1000 */
	rc |= al_pmdt_axi_mon_tracker_config(&axi_mon_a, 0, &tracker);

	/* configure trace data */
	memset(&trace, 0, sizeof(struct al_pmdt_axi_mon_trace));
	trace.tracker_sel = AL_BIT(0); /* we use tracker 0 for trapping */
	trace.addr_width = 2; /* should be 0x1000 */
	trace.incl_data = 1; /* we will only trace the read data */
	rc |= al_pmdt_axi_mon_trace_config(&axi_mon_a, 0, &trace);

	/*******************************/
	/* PMU A configuration         */
	/*******************************/
	rc |= al_pmdt_pmu_handle_init(&pmu_a, "Core A",
			(void __iomem *)PMU_COMPLEX_A_BASE_ADDRESS,
			PMU_A_CLOCK_KHZ);
	/* set pmu config */
	memset(&pmu_config, 0, sizeof(struct al_pmdt_pmu_config));
	pmu_config.window_unit_exp = -6; /* 1usec window unit */
	pmu_config.windows_en = AL_TRUE;
	rc |= al_pmdt_pmu_config(&pmu_a, &pmu_config);

	/* set window size */
	al_pmdt_pmu_window_size_set(&pmu_a, 10); /* sampling window size = 10usec */

	/* set event counters */
	memset(&event_config, 0, sizeof(struct al_pmdt_pmu_event_config));
	event_config.window_en = 1;
	event_config.pause_en = 1;
	event_config.thr_low = 0;
	event_config.thr_high = INFINITE;
	/* configure 6 event counters */
	for (i = 0; i < AL_PMDT_AXI_MON_BAR_NUM; i++) {
		event_config.trig_num = 64 + i; /* AXI monitor triggers start at 64 */
		rc |= al_pmdt_pmu_event_config(&pmu_a, i, &event_config);
	}

	/*******************************/
	/* ELA A configuration         */
	/*******************************/
	/* Init & Set ELA Config */
	rc |= al_pmdt_elacfg_handle_init(&elacfg_a, "Core A",
			(void __iomem *)PMU_COMPLEX_A_BASE_ADDRESS);

	memset(&elacfg_config, 0, sizeof(struct al_pmdt_elacfg_config));
	/* enable nothing*/
	rc |= al_pmdt_elacfg_config(&elacfg_a, &elacfg_config);

	/* we force ext_trig[0] signal to 1, so we can use it to trace all signals*/
	al_pmdt_elacfg_exttrig_set(&elacfg_a, 0xFC);

	/* Init & Set ELA */
	rc |= al_pmdt_ela_handle_init(&ela_a, "Core A", 0,
			(void __iomem *)PMU_COMPLEX_A_BASE_ADDRESS);

	al_pmdt_ela_write_lock(&ela_a, AL_FALSE); /* unlock ELA registers */

	memset(&ela_config, 0, sizeof(struct al_pmdt_ela_config));
	ela_config.timestamp_en = 1;
	/* every 32 cycles inject timestamp to trace data */
	ela_config.ts_int = 5;
	/* for each trace entry we keep counter's bits 5 & 4*/
	ela_config.tcsel1 = 5;
	ela_config.tcsel0 = 4;
	rc |= al_pmdt_ela_config(&ela_a, &ela_config);

	memset(&ela_trig_state, 0, sizeof(struct al_pmdt_ela_trig_config));
	ela_trig_state.signal_group = AL_BIT(9); /* aximon0 connected to group 9 */
	ela_trig_state.trig_ctrl.comp_mode = AL_ELA_COMP_MODE_SIGNAL;
	ela_trig_state.trig_ctrl.comp_type = AL_ELA_COMP_EQUAL;
	ela_trig_state.trig_ctrl.trace_cap_ctrl = AL_ELA_TCC_SIG_MATCH;
	ela_trig_state.ext_trig_mask = 0x1;
	/* this will cancel ext_trig filtering*/
	ela_trig_state.ext_trig_comp = 0x1;
	ela_trig_state.action.trace_en = 1;
	/* we don't change state */
	ela_trig_state.next_state = AL_ELA_TRIG_STATE_0;
	ela_trig_state.sig_mask[0] = 0xFFFF; /* see aximon trace data */
	ela_trig_state.sig_comp[0] = 0x1000;
	rc |= al_pmdt_ela_trigger_state_config(&ela_a, 0, &ela_trig_state);

	/*******************************/
	/* AXI Monitor B configuration */
	/*******************************/
	rc |= al_pmdt_axi_mon_handle_init(&axi_mon_b, "Core B", 0,
			(void __iomem *)PMU_COMPLEX_B_BASE_ADDRESS);
	/* set monitor mode */
	memset(&axi_mon_mode, 0, sizeof(struct al_pmdt_axi_mon_mode));
	/* we want to monitor write requests */
	axi_mon_mode.is_read = AL_FALSE;
	axi_mon_mode.latency_tracker_sel = 0;
	rc |= al_pmdt_axi_mon_mode_config(&axi_mon_b, &axi_mon_mode);

	/* all bars are 0, as we just want to catch a single write occurrence */
	bars.timeout = INFINITE; /* we don't define timeout */
	rc |= al_pmdt_axi_mon_bars_config(&axi_mon_a, &bars);

	/* set tracker 0 pattern */
	memset(&tracker, 0, sizeof(struct al_pmdt_axi_mon_tracker));
	/* we assume address size of 48 bits and AxID size of 4 bits */
	tracker.mask[0] = 0xFFFFFFF0;
	tracker.mask[1] = 0x000FFFFF;
	tracker.comp[0] = 0x2000 << 4; /* we monitor write address 0x2000 */
	rc |= al_pmdt_axi_mon_tracker_config(&axi_mon_a, 0, &tracker);

	/*******************************/
	/* PMU B configuration         */
	/*******************************/
	rc |= al_pmdt_pmu_handle_init(
			&pmu_b, "Core B",
			(void __iomem *)PMU_COMPLEX_B_BASE_ADDRESS,
			PMU_B_CLOCK_KHZ);
	/* set pmu config */
	memset(&pmu_config, 0, sizeof(struct al_pmdt_pmu_config));
	pmu_config.windows_en = 0; /* no need in time window */
	pmu_config.window_unit_exp = -6; /* yet we must program some value */
	pmu_config.ovf_int_en = 1; /* we want to use overflow interrupt */
	rc |= al_pmdt_pmu_config(&pmu_b, &pmu_config);

	/* set event counters */
	memset(&event_config, 0, sizeof(struct al_pmdt_pmu_event_config));
	event_config.window_en = 0;
	event_config.pause_en = 0;
	event_config.thr_low = 0;
	event_config.thr_high = 0;
	event_config.trig_num = 64; /* AXI monitor trigger 0 */
	rc |= al_pmdt_pmu_event_config(&pmu_b, i, &event_config);


	/*******************************/
	/* CCTM configuration          */
	/*******************************/
	rc |= al_pmdt_cctm_handle_init(&cctm, (void __iomem *)CCTM_BASE_ADDRESS);

	/* setup timestamp generator */
	rc |= al_pmdt_cctm_timestamp_ratio_set(&cctm,
			CCTM_CLOCK_KHZ/100); /* ts every 10usec */
	al_pmdt_cctm_timestamp_clear(&cctm);
	al_pmdt_cctm_timestamp_enable(&cctm, 1);

	/* enable PMU B to pause PMU A */
	pmu_mask[0] = AL_BIT(0); /* dst PMU - PMU A */
	al_pmdt_cctm_cross_pause_enable(&cctm, AL_TRUE, AL_BIT(1), &pmu_mask);

	return rc;
}

void pmdt_run_test(void)
{
	uint32_t pmu_mask[AL_PMDT_CCTM_MATRIX_LINE_SIZE] = {0};
	struct al_pmdt_ela_dev_info ela_info;
	struct al_pmdt_ela_ram_entry entry;
	unsigned int last_addr, trace_size, i;
	al_bool is_wrap;
	struct al_pmdt_pmu_event_counters counters;

	/* enable AXI monitors */
	al_pmdt_axi_mon_clear(&axi_mon_a, AL_TRUE, AL_TRUE, AL_TRUE);
	al_pmdt_axi_mon_enable(&axi_mon_a, AL_TRUE);
	al_pmdt_axi_mon_clear(&axi_mon_b, AL_TRUE, AL_TRUE, AL_TRUE);
	al_pmdt_axi_mon_enable(&axi_mon_b, AL_TRUE);

	/* enable PMU */
	al_pmdt_pmu_clear(&pmu_a);
	al_pmdt_pmu_enable(&pmu_a, AL_TRUE);
	al_pmdt_pmu_clear(&pmu_b);
	al_pmdt_pmu_enable(&pmu_b, AL_TRUE);

	/* start and lock ELA */
	al_pmdt_ela_start(&ela_a, AL_TRUE);
	/*
	 * this is probably good for cases we
	 * suspect memory corruption and want
	 * to keep trace data safe
	 * */
	al_pmdt_ela_write_lock(&ela_a, AL_TRUE);

	/* note that both PMU complexes A & B are still disabled by CCTM */

	/***** CCTM - enable PMU A & B - start test *****/
	pmu_mask[0] = 0x3;
	al_pmdt_cctm_pmu_clear(&cctm, &pmu_mask);
	al_pmdt_cctm_pmu_enable(&cctm, AL_TRUE, &pmu_mask);

	/* now we wait for PMU A pause
	 * (can be done via irq, here we do polling
	 * */
	do {
		al_pmdt_cctm_paused_get(&cctm, &pmu_mask);
	} while (!(pmu_mask[0] & AL_BIT(0)));

	/***** print ELA A trace data & PMU latency histogram *****/
	/* we must unlock & stop ELA first */
	al_pmdt_ela_write_lock(&ela_a, 0);
	al_pmdt_ela_start(&ela_a, 0);
	/* get current write pointer */
	al_pmdt_ela_ram_write_ptr_get(&ela_a, &last_addr, &is_wrap);
	/* set read pointer and calculate read size */
	if (is_wrap) {
		al_info("trace wrap\n");
		/* set read pointer to next entry */
		al_pmdt_ela_ram_read(&ela_a, 0, last_addr, NULL);
		/* get RAM size */
		al_pmdt_ela_device_info_get(&ela_a, &ela_info);
		trace_size = 1 << ela_info.ram_addr_size;
	} else {
		trace_size = last_addr;
	}
	/* print trace */
	for (i = 0; i < trace_size; i++) {
		al_pmdt_ela_ram_read(&ela_a, 1, 0, &entry);
		if (entry.ts)
			al_info("%04x: timestamp | 0x%016" PRIx64 "\n",
					i, entry.payload_0);
		else
			al_info("%04x: tcsel %d:%d | 0x%016" PRIx64
					" 0x%016" PRIx64 "\n",
				i, entry.tcsel1, entry.tcsel0,
				entry.payload_0, entry.payload_1);
	}
	/* print latency histogram */
	for (i = 0; i < AL_PMDT_AXI_MON_BAR_NUM; i++) {
		al_pmdt_pmu_event_counters_get(&pmu_a, i, &counters);
		al_info("BIN%d: 0x%016" PRIx64 "\n",
				i, counters.cur_event_cnt);
	}
}

/** @endcode */

/** @} */
