/*******************************************************************************
Copyright (C) 2015 Annapurna Labs Ltd.

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
 * @defgroup groupddrinit DDR controller & PHY hardrware initialization
 * @ingroup groupddr
 *  @{
 * @file   al_hal_ddr_init.h
 *
 * @brief Header file for the DDR initialization HAL driver
 */

#ifndef __AL_HAL_DDR_INIT_H__
#define __AL_HAL_DDR_INIT_H__

#include "al_hal_ddr.h"
#include "al_hal_ddr_common.h"

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

/* Convert nanoseconds to reference clock cycles (round up) */
#define AL_DDR_NS_TO_CK_RND_UP(clk_freq_mhz, ns)\
	((((ns) * (clk_freq_mhz)) + 999) / 1000)

/* DDR fine granularity refresh */
enum al_ddr_fine_granularity_refresh {
	AL_DDR_FINE_GRANULARITY_REFRESH_FIXED_1X,
	AL_DDR_FINE_GRANULARITY_REFRESH_FIXED_2X,
	AL_DDR_FINE_GRANULARITY_REFRESH_FIXED_4X
};

/* DDR frequency enumeration */
enum al_ddr_freq {
	AL_DDR_FREQ_800 = 0,
	AL_DDR_FREQ_1066,
	AL_DDR_FREQ_1333,
	AL_DDR_FREQ_1600,
	AL_DDR_FREQ_1866,
	AL_DDR_FREQ_2133,
	AL_DDR_FREQ_2400,
	AL_DDR_FREQ_2666,
	AL_DDR_FREQ_2933,
	AL_DDR_FREQ_3200,

	AL_DDR_FREQ_ENUM_SIZE
};

/**
 * Output Driver Impedance Control: Controls the output drive strength
 * Valid values:
 * DDR3 : AL_DDR_DIC_RZQ6, AL_DDR_DIC_RZQ7
 * DDR4 : AL_DDR_DIC_RZQ7, AL_DDR_DIC_RZQ5
 */
enum al_ddr_dic {
	AL_DDR_DIC_RZQ6,
	AL_DDR_DIC_RZQ7,
#if (AL_DEV_ID >= AL_DEV_ID_ALPINE_V2)
	AL_DDR_DIC_RZQ5,
#endif
};

/**
 * On Die Termination: Selects the effective resistance for SDRAM on
 * die termination.
 * Valid values:
 * DDR3 : AL_DDR_ODT_DIS, AL_DDR_ODT_RZQ2, AL_DDR_ODT_RZQ4,
 * AL_DDR_ODT_RZQ6, AL_DDR_ODT_RZQ8, AL_DDR_ODT_RZQ12
 * DDR4 : AL_DDR_ODT_DIS, AL_DDR_ODT_RZQ4, AL_DDR_ODT_RZQ2,
 * AL_DDR_ODT_RZQ6, AL_DDR_ODT_RZQ1, AL_DDR_ODT_RZQ5, AL_DDR_ODT_RZQ3,
 * AL_DDR_ODT_RZQ7
 */
enum al_ddr_odt {
	AL_DDR_ODT_DIS,
	AL_DDR_ODT_RZQ2,
	AL_DDR_ODT_RZQ4,
	AL_DDR_ODT_RZQ6,
	AL_DDR_ODT_RZQ8,
	AL_DDR_ODT_RZQ12,
#if (AL_DEV_ID >= AL_DEV_ID_ALPINE_V2)
	AL_DDR_ODT_RZQ1,
	AL_DDR_ODT_RZQ5,
	AL_DDR_ODT_RZQ3,
	AL_DDR_ODT_RZQ7,
#endif
};

/**
 * Dynamic ODT: Selects RTT for dynamic ODT
 * Valid values:
 * DDR3 : AL_DDR_ODT_DYN_DIS, AL_DDR_ODT_DYN_RZQ2, AL_DDR_ODT_DYN_RZQ4
 * DDR4 : AL_DDR_ODT_DYN_DIS, AL_DDR_ODT_DYN_RZQ2, AL_DDR_ODT_DYN_RZQ1,
 * AL_DDR_ODT_DYN_HI_Z
 */
enum al_ddr_odt_dyn {
	AL_DDR_ODT_DYN_DIS,
	AL_DDR_ODT_DYN_RZQ2,
	AL_DDR_ODT_DYN_RZQ4,
#if (AL_DEV_ID >= AL_DEV_ID_ALPINE_V2)
	AL_DDR_ODT_DYN_RZQ1,
	AL_DDR_ODT_DYN_HI_Z,
#endif
};

/* RTT PARK: Selects RTT for RTT PARK */
enum al_ddr_rtt_park {
	AL_DDR_RTT_PARK_DIS,
	AL_DDR_RTT_PARK_RZQ4,
	AL_DDR_RTT_PARK_RZQ2,
	AL_DDR_RTT_PARK_RZQ6,
	AL_DDR_RTT_PARK_RZQ1,
	AL_DDR_RTT_PARK_RZQ5,
	AL_DDR_RTT_PARK_RZQ3,
	AL_DDR_RTT_PARK_RZQ7,
};

/* DQS Resistance */
enum al_ddr_dqs_res {
	AL_DDR_DQS_RES_PULL_DOWN_500OHM,
	AL_DDR_DQS_RES_PULL_UP_611OHM,
};

/* DQS# Resistance */
enum al_ddr_dqsn_res {
	AL_DDR_DQSN_RES_PULL_UP_458OHM,
	AL_DDR_DQSN_RES_PULL_UP_500OHM,
};

/* Memory organization */
struct al_ddr_init_cfg_org {
	/* Data width: 32 or 64 bits */
	enum al_ddr_data_width	data_width;

	/* Number of ranks: 1, 2, 4 */
	unsigned int		ranks;

	/* Number of cids: 1, 2, 4 */
	unsigned int		cids;

	/* Number of dimms: 1, 2 */
	unsigned int		dimms;

	/**
	 * Determines whether the DIMM is RDIMM (registered, buffered), or
	 * UDIMM.
	 *
	 * Required value: DIMM specific
	 */
	al_bool			rdimm;

	/**
	 * Determines whether the UDIMM implements address mirroring
	 *
	 * Required value: DIMM specific
	 */
	al_bool			udimm_addr_mirroring;

	/**
	 * Determines whether or not the DIMM has ECC support
	 *
	 * Required value: DIMM specific
	 */
	al_bool			ecc_is_supported;

#if (AL_DEV_ID >= AL_DEV_ID_ALPINE_V2)
	/**
	 * Determines whether the DIMM is DDR3 or DDR4.
	 *
	 * Required value: DIMM specific
	 */
	enum al_ddr_type	ddr_type;

	/**
	 * Determines whether the device is x4/x8/x16.
	 *
	 * Required value: device specific
	 */
	enum al_ddr_device	ddr_device;

	/**
	 * Determines whether the DIMM implements DQ rank swap
	 * Applicable to DDR4 only
	 *
	 * Required value: DIMM specific
	 */
	al_bool			dq_rank_swap_enabled;

	/**
	 * Determines the DIMM nibble mapping (DQ and ECC)
	 * Applicable to DDR4 only
	 *
	 * Required value: DIMM specific
	 */
	uint8_t			dq_nibble_map[(AL_DDR_PHY_NUM_BYTE_LANES-1)*2];
	uint8_t			cb_nibble_map[2];
#endif
};

struct al_ddr_init_cfg_impedance_ctrl {
	/**
	 * Output Driver Impedance Control: Controls the output drive strength
	 *
	 * Required value: board specific
	 */
	enum al_ddr_dic		dic;

	/**
	 * On Die Termination: Selects the effective resistance for SDRAM on
	 * die termination.
	 *
	 * Required value: board specific
	 */
	enum al_ddr_odt		odt;

	/**
	 * Dynamic ODT: Selects RTT for dynamic ODT.
	 *
	 * Required value: board specific
	 */
	enum al_ddr_odt_dyn	odt_dyn;

#if (AL_DEV_ID >= AL_DEV_ID_ALPINE_V2)
	/**
	 * RTT PARK: Selects RTT_PARK
	 *
	 * Required value: board specific
	 */
	enum al_ddr_rtt_park	rtt_park;

	uint8_t			host_initial_vref;
	uint8_t			vrefdq;

#define AL_DDR_INIT_CFG_HAS_HV_MIN_MAX   1
	uint32_t		hv_min;
	uint32_t		hv_max;

	/**
	 * PHY Pull up ROUT: Selects Output impedance pull up of the PHY for each ZQ segment.
	 * Applicable to DDR4 only
	 *
	 * Required value: board specific
	 */
	uint8_t			phy_rout_pu[AL_DDR_PHY_NUM_ZQ_SEGMENTS_ALPINE_V2];

	/**
	 * PHY Pull down ROUT: Selects Output impedance pull down of the PHY for each ZQ segment.
	 * Applicable to DDR4 only
	 *
	 * Required value: board specific
	 */
	uint8_t			phy_rout_pd[AL_DDR_PHY_NUM_ZQ_SEGMENTS_ALPINE_V2];

	/**
	 * PHY Pull up ODT: Selects On-die pull up termination of the PHY for each ZQ segment.
	 * Applicable to DDR4 only
	 *
	 * Required value: board specific
	 */
	uint8_t			phy_pu_odt[AL_DDR_PHY_NUM_ZQ_SEGMENTS_ALPINE_V2];
#endif

	/**
	 * WR ODT MAP: Selects which ranks ODT to assert during writes.
	 * Each value control the ODT when writing to this rank (aka first
	 * value control ODT during writes to rank 0), where each bit
	 * indicates if the correspondent rank ODT should be asserted
	 *
	 * Required value: board specific
	 */
	unsigned short		wr_odt_map[AL_DDR_NUM_RANKS];

	/**
	 * WR ODT MAP: Selects which ranks ODT to assert during reads.
	 * Each value control the ODT when reading from this rank (aka first
	 * value control ODT during reads from rank 0), where each bit
	 * indicates if the correspondent rank ODT should be asserted
	 *
	 * Required value: board specific
	 */
	unsigned short		rd_odt_map[AL_DDR_NUM_RANKS];

	/**
	 * PHY ROUT: Selects Output impedance of the PHY for each ZQ segment.
	 *
	 * Required value: board specific
	 */
#if (AL_DEV_ID == AL_DEV_ID_ALPINE_V1)
	enum al_ddr_phy_rout	phy_rout[AL_DDR_PHY_NUM_ZQ_SEGMENTS_ALPINE_V1];
#elif (AL_DEV_ID >= AL_DEV_ID_ALPINE_V2)
	enum al_ddr_phy_rout	phy_rout[AL_DDR_PHY_NUM_ZQ_SEGMENTS_ALPINE_V2];
#endif

	/**
	 * PHY ODT: Selects On-die termination of the PHY for each ZQ segment.
	 *
	 * Required value: board specific
	 */
#if (AL_DEV_ID == AL_DEV_ID_ALPINE_V1)
	enum al_ddr_phy_odt	phy_odt[AL_DDR_PHY_NUM_ZQ_SEGMENTS_ALPINE_V1];
#elif (AL_DEV_ID >= AL_DEV_ID_ALPINE_V2)
	enum al_ddr_phy_odt	phy_odt[AL_DDR_PHY_NUM_ZQ_SEGMENTS_ALPINE_V2];
#endif

	/**
	 * DQS Resistance
	 *
	 * Required value: board specific
	 * Only applicable for DDR3. For DDR4 there is no pull up/down
	 */
	enum al_ddr_dqs_res	dqs_res;

	/**
	 * DQS# Resistance
	 *
	 * Required value: board specific
	 * Only applicable for DDR3. For DDR4 there is no pull up/down
	 */
	enum al_ddr_dqsn_res	dqsn_res;
};

/* Miscelaneous params */
struct al_ddr_init_cfg_misc {
	/**
	 * Determines whether or not to enable error correction
	 *
	 * Suggested value: according to 'ecc_is_supported'
	 */
	al_bool			ecc_is_enabled;

	/**
	 * Disable ECC scrub (correcting single-bit error in the DRAM itself)
	 *
	 * Suggested value: 0
	 */
	al_bool			ecc_scrub_dis;

	/**
	 * Determines whether or not to perform PHY training
	 *
	 * Suggested value: 1
	 */
	al_bool			training_en;

	/*
	 * Enable PHY DLL calibrations
	 * Should set to AL_FALSE for V4 and beyond.
	 *
	 * Suggested value : 1
	 */
	al_bool			phy_dll_en;

#if (AL_DEV_ID >= AL_DEV_ID_ALPINE_V2)
	/**
	 * Determines whether or not to enable CRC
	 * Applicable to DDR4 only
	 *
	 * Suggested value: AL_FALSE
	 */
	al_bool			crc_enabled;

	/**
	 * Determines whether or not to enable DDR4 C/A parity
	 * Applicable to DDR4 only
	 *
	 * Suggested value: AL_FALSE
	 */
	al_bool			parity_enabled;

	/**
	 * Determines whether or not to enable On-Chip parity
	 *
	 * Suggested value: AL_TRUE
	 */
	al_bool			ocpar_enabled;

	/** Determines whether to enable reduced package mux - Obsolete */
	al_bool			reduced_pkg_mux_enabled;

	/**
	 * Determines whether or not to enable read DBI
	 * Applicable to DDR4 only
	 *
	 * Suggested value: AL_FALSE
	 * Must be set to AL_FALSE for DDR3
	 */
	al_bool			read_dbi_enabled;

	/**
	 * Determines whether or not to enable write DBI
	 * Applicable to DDR4 only
	 *
	 * Suggested value: AL_FALSE
	 * Must be set to AL_FALSE for DDR3
	 */
	al_bool			write_dbi_enabled;

#endif
#if (AL_DEV_ID >= AL_DEV_ID_ALPINE_V3)
	/**
	 * Determines whether or not to enable average VREF calculation
	 * Applicable to DDR4 only
	 *
	 */
#define AL_DDR_INIT_CFG_MISC_HAS_AVERAGE_VREF_EN     1
	al_bool			average_vref_en;

	/* ECC poison address is statically defined during init, cannot be changed afterwards */
#define AL_DDR_INIT_CFG_MISC_HAS_STATIC_ECC_POISON   1
	/* ECC Poison enable */
	al_bool ecc_poison_addr_cfg;

	/* ECC Poison address */
	al_phys_addr_t ecc_poison_address;
#endif
};

/* Timing */
struct al_ddr_init_cfg_tmg {
	/**
	 * DDR controller reference clock [MHz]
	 *
	 * Required value: according to bootstraps
	 */
	unsigned int		ref_clk_freq_mhz;

	/**
	 * Speed bin
	 *
	 * Required value: derived from 'ref_clk_freq_mhz'
	 */
	enum al_ddr_freq	ddr_freq;

	/**
	* Four activate window
	* [picoseconds]
	*
	* Required value: DIMM specific
	*/
	unsigned int		t_faw_ps;

	/**
	* Activate to precharge command period - minimal value
	* [picoseconds]
	*
	* Required value: DIMM specific
	*/
	unsigned int		t_ras_min_ps;

	/**
	* Minimum time between two successive activates to a given bank.
	* [picoseconds]
	*
	* Required value: DIMM specific
	*/
	unsigned int		t_rc_ps;

	/**
	* Minimum time from activate to read or write command to same bank
	* [picoseconds]
	*
	* Required value: DIMM specific
	*/
	unsigned int		t_rcd_ps;

	/**
	* ACTIVE to ACTIVE command period
	*
	* Required value: DIMM specific
	*/
	unsigned int		t_rrd_ps;

	/**
	* Minimum time from precharge to activate of same bank
	*
	* Required value: DIMM specific
	*/
	unsigned int		t_rp_ps;

	/**
	 * Minimum time between refresh commands to a given rank [picoseconds]
	 *
	 * Required value: DIMM specific
	 */
	unsigned int		t_rfc_min_ps;
#if (AL_DEV_ID >= AL_DEV_ID_ALPINE_V2)
	unsigned int		t_rfc2_ps;
	unsigned int		t_rfc4_ps;
#endif

	/**
	 * Delay from start of internal write transaction to internal read
	 * command. [picoseconds]
	 *
	 * Required only for DDR3
	 * Required value: DIMM specific
	 */
	unsigned int		t_wtr_ps;

	/**
	 * Internal READ Command to PRECHARGE Command delay [picoseconds]
	 *
	 * Required only for DDR3
	 * Required value: DIMM specific
	 */
	unsigned int		t_rtp_ps;

	/**
	 * WRITE recovery time [picoseconds]
	 *
	 * Required only for DDR3
	 * Required value: DIMM specific
	 */
	unsigned int		t_wr_ps;

	/**
	 * CAS Latency [nCK]
	 *
	 * Required value: DIMM specific
	 */
	unsigned int		cl;

	/**
	 * CAS Write Latency [nCK]
	 *
	 * Required value: DIMM specific
	 */
	unsigned int		cwl;

	/**
	 * Additive Latency [nCK]
	 *
	 * Required value: board specific
	 */
	unsigned int		al;

#if (AL_DEV_ID >= AL_DEV_ID_ALPINE_V2)
	/**
	 * CAS# to CAS# command delay same bank group [picoseconds]
	 *
	 * Required value: DIMM specific
	 */
	unsigned int		t_ccd_ps;

	/**
	* ACTIVE to ACTIVE command period different bank group
	*
	* Required value: DIMM specific
	*/
	unsigned int		t_rrd_s_ps;

#endif

#if (AL_DEV_ID > AL_DEV_ID_ALPINE_V3) ||\
	((AL_DEV_ID == AL_DEV_ID_ALPINE_V3) && ((!defined(AL_DEV_REV_ID)) || (AL_DEV_REV_ID == 1)))
	/**
	 * Refresh interval [ns]
	 *
	 * Required value: 7800
	 */
	#define AL_DDR_INIT_CFG_TMG_HAS_REFRESH_INTERVAL
	unsigned int		refresh_interval_ns;
#endif
};

/* Performace params */
struct al_ddr_init_cfg_perf {
	/**
	 * The programmed value is the number of refresh time-outs that is
	 * allowed to accumulate before traffic is blocked and the refreshes are
	 * forced to execute. Closing pages to perform a refresh is a one-time
	 * penalty that must be paid for each group of refreshes. Therefore,
	 * performing refreshes in a burst reduces the per-refresh penalty of
	 * these page closings. Higher numbers increases utilization; lower
	 * numbers decreases the worst-case latency associated with refreshes.
	 *
	 * Valid values:
	 *    1 - single refresh
	 *    8 - burst-of-8 refresh.
	 *
	 * Suggested value: 1
	 */
	unsigned int		refresh_burst;

	/**
	 * When the preferred transaction store is empty for these many clock
	 * cycles, switch to the alternate transaction store if it is non-empty.
	 * The read transaction store (both high and low priority) is the
	 * default preferred transaction store and the write transaction store
	 * is the alternative store. When prefer write over read is set this is
	 * reversed.
	 *
	 * Valid values: 0-127
	 *
	 * Suggested value: 0x10
	 */
	unsigned int		rdwr_idle_gap;		/* [nCK] */

	/**
	 * Number of entries in the low priority transaction store is this
	 * value. (32 - lpr_num_entries) is the
	 * number of entries available for the high priority transaction store.
	 * Setting this to maximum value allocates all entries to low priority
	 * transaction store. Setting this to 0 allocates 1 entry to low
	 * priority transaction store and the rest to high priority transaction
	 * store.
	 *
	 * Valid values: 0-30
	 *
	 * Suggested value: 0x17
	 */
	unsigned int		lpr_num_entries;

	/**
	 * If true, bank is closed until transactions are available for it.
	 * This is different from auto-precharge in that:
	 * (a) explicit precharge commands are used, and not read/write with
	 *     auto-precharge and
	 * (b) page is not closed after a read/write if there is another
	 *     read/write pending to the same page.
	 * If false, bank remains open until there is a need to close it (to
	 * open a different page, or for page time-out or refresh time-out.)
	 * This does not apply when auto-precharge is used. One open page can be
	 * maintained per bank.
	 *
	 * Suggested value: 1 for ALPINE V1, 0 for ALPINE V2
	 *
	 */
	al_bool			pageclose;
#if ((AL_DEV_ID == AL_DEV_ID_ALPINE_V3) && ((!defined(AL_DEV_REV_ID)) || (AL_DEV_REV_ID == 1))) ||\
	(AL_DEV_ID > AL_DEV_ID_ALPINE_V3)
	al_bool			pageclose_wr;
#endif

#if (AL_DEV_ID >= AL_DEV_ID_ALPINE_V2)
	/**
	 * This field works in conjunction with pageclose.
	 * It only has meaning if pageclose==1.
	 * If pageclose==1 and pageclose_timer==0, then an auto-precharge may be
	 * scheduled for last read or write command in the CAM with a bank and page hit.
	 * If pageclose==1 and pageclose_timer>0, then an auto-precharge is not scheduled
	 * for last read or write command in the CAM with a bank and page hit.
	 * Instead, a timer is started, with pageclose_timer as the initial value.
	 * There is a timer on a per bank basis.
	 * The timer decrements unless the next read or write in the CAM to a bank is
	 * a page hit. It gets reset to pageclose_timer value if the next read or write
	 * in the CAM to a bank is a page hit. Once the timer has reached zero,
	 * an explcit precharge will be attempted to be scheduled.
	 *
	 * Valid values: 0-255
	 *
	 * Suggested value: 100
	 */
	unsigned int		pageclose_timer;
#if ((AL_DEV_ID == AL_DEV_ID_ALPINE_V3) && ((!defined(AL_DEV_REV_ID)) || (AL_DEV_REV_ID == 1))) ||\
	(AL_DEV_ID > AL_DEV_ID_ALPINE_V3)
	unsigned int		pageclose_wr_timer;
#endif
#endif

	/**
	 * Number of clocks that the HPR queue is guaranteed to stay in
	 * non-critical state before returning back to critical state.
	 *
	 * Valid values: 0-65535
	 *
	 * Suggested value: 0x20
	 */
	unsigned int		hpr_min_non_critical;	/* [nCK] */

	/**
	 * Number of transactions that are serviced once the HPR queue goes
	 * critical is the smaller of:
	 * - This number
	 * - Number of transactions available
	 *
	 * Valid values: 0-255
	 *
	 *   Suggested value: 0xf
	 */
	unsigned int		hpr_xact_run_length;

	/**
	 * Number of clocks that the HPR queue can be starved before it goes
	 * critical.
	 *
	 * Valid values: 0-65535
	 *
	 * Suggested value: 0x20
	 */
	unsigned int		hpr_max_starve;		/* [nCK] */

	/**
	 * Number of clocks that the LPR queue is guaranteed to be non-critical.
	 *
	 * Valid values: 0-65535
	 *
	 * Suggested value: 0xff
	 */
	unsigned int		lpr_min_non_critical;	/* [nCK] */

	/**
	 * Number of transactions that are serviced once the LPR queue goes
	 * critical is the smaller of:
	 * - This number
	 * - Number of transactions available
	 *
	 * Valid values: 0-255
	 *
	 *   Suggested value: 0xf
	 */
	unsigned int		lpr_xact_run_length;

	/**
	 * Number of clocks that the LPR queue can be starved before it goes
	 * critical.
	 *
	 * Valid values: 0-65535
	 *
	 * Suggested value: 0xff
	 */
	unsigned int		lpr_max_starve;		/* [nCK] */

	/**
	 * Number of clocks that the write queue is guaranteed to be
	 * non-critical.
	 *
	 * Valid values: 0-65535
	 *
	 * Suggested value: 0xff
	 */
	unsigned int		w_min_non_critical;	/* [nCK] */

	/**
	 * Number of transactions that are serviced once the WR queue goes
	 * critical is the smaller of:
	 * - This number
	 * - Number of transactions available
	 *
	 * Valid values: 0-255
	 *
	 *   Suggested value: 0xf
	 */
	unsigned int		w_xact_run_length;

	/**
	 * Number of clocks that the write queue can be starved before it goes
	 * critical.
	 *
	 * Valid values: 0-65535
	 *
	 * Suggested value: 0xff
	 */
	unsigned int		w_max_starve;		/* [nCK] */

#if ((AL_DEV_ID == AL_DEV_ID_ALPINE_V3) && ((!defined(AL_DEV_REV_ID)) || (AL_DEV_REV_ID == 1))) ||\
	(AL_DEV_ID > AL_DEV_ID_ALPINE_V3)
	/**
	 * Maximum number of reads that can be scheduled consecutively to the same rank.
	 * Higher numbers increase bandwidth utilization, lower numbers increase fairness.
	 * When set to 0, the Controller will stay on the same rank as long as commands
	 * are available for it.
	 *
	 * Valid values: 0-15
	 *
	 *   Suggested value: 0xf
	 */
	unsigned int		max_rank_rd;

	/**
	* Determines Fine Granularity refresh mode
	* Applicable to DDR4 only
	*/
	#define AL_DDR_INIT_CFG_PERF_HAS_FINE_GRANULARITY_REFRESH_MODE
	enum al_ddr_fine_granularity_refresh	fine_granularity_refresh_mode;
#endif
};

/* Calculated params */
struct al_ddr_calc_params {
	unsigned int		t_rp;
	unsigned int		t_ras_min;
	unsigned int		t_rc;
	unsigned int		t_rrd;
	unsigned int		t_rcd;
	unsigned int		t_rtp;
	unsigned int		t_wtr;
	unsigned int		rd_odt_delay;
	uint16_t		mr0;
	uint16_t		mr1;
	uint16_t		mr2;
#if (AL_DEV_ID >= AL_DEV_ID_ALPINE_V2)
	uint16_t		mr3;
	uint16_t		mr4;
	uint16_t		mr5;
	uint16_t		mr6;
	unsigned int		t_pl;
	unsigned int		t_ccd;
	unsigned int		t_rfc_min;
	unsigned int		t_dllk;
#endif
#if (AL_DEV_ID >= AL_DEV_ID_ALPINE_V3)
	unsigned int t_mrd;
	unsigned int t_rfc_ps;
#endif

	uint8_t			active_byte_lanes[AL_DDR_PHY_NUM_BYTE_LANES];
};

struct al_ddr_init_cfg {
	/* DDR config object */
	struct al_ddr_cfg			ddr_cfg;

	/* Memory organization */
	struct al_ddr_init_cfg_org		org;

	/* Address mapping */
	struct al_ddr_addrmap			addrmap;

	/* Timing */
	struct al_ddr_init_cfg_tmg		tmg;

	/* Impedance control params */
	struct al_ddr_init_cfg_impedance_ctrl	impedance_ctrl;

	/* Miscelaneous params */
	struct al_ddr_init_cfg_misc		misc;

	/* Performace params - valid only if non NULL */
	struct al_ddr_init_cfg_perf		*perf;

	/* Calculated params - filled by the HAL */
	struct al_ddr_calc_params		calc;
};

int al_ddr_init(
	struct al_ddr_init_cfg	*cfg);

/* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */
/** @} end of DDR group */
#endif

