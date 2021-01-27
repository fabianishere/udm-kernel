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
 * @defgroup group_serdes_api API
 * SerDes HAL driver API
 * @ingroup group_serdes SerDes
 * @{
 *
 * @file   al_mod_hal_serdes_interface.h
 *
 * @brief Header file for the SerDes HAL driver
 *
 */

#ifndef __AL_HAL_SERDES_INTERFACE_H__
#define __AL_HAL_SERDES_INTERFACE_H__

#include "al_mod_hal_common.h"

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

enum al_mod_serdes_type {
	AL_SRDS_TYPE_HSSP,
	AL_SRDS_TYPE_25G,
	AL_SRDS_TYPE_AVG,
	AL_SRDS_TYPE_SNPS,
};

enum al_mod_serdes_reg_page {
	/* Relevant to Serdes hssp, 25g, and AVG */
	AL_SRDS_REG_PAGE_0_LANE_0 = 0,
	/* Relevant to Serdes hssp and 25g */
	AL_SRDS_REG_PAGE_1_LANE_1,
	/* Relevant to Serdes hssp only */
	AL_SRDS_REG_PAGE_2_LANE_2,
	AL_SRDS_REG_PAGE_3_LANE_3,
	/* Relevant to Serdes hssp and 25g */
	AL_SRDS_REG_PAGE_4_COMMON,
	/* Relevant to Serdes hssp only */
	AL_SRDS_REG_PAGE_0123_LANES_0123 = 7,
	/* Relevant to Serdes 25g only */
	AL_SRDS_REG_PAGE_TOP,
};

/* Relevant to Serdes hssp only */
enum al_mod_serdes_reg_type {
	AL_SRDS_REG_TYPE_PMA = 0,
	AL_SRDS_REG_TYPE_PCS,
};

enum al_mod_serdes_lane {
	/* Relevant to Serdes hssp, 25g, and AVG */
	AL_SRDS_LANE_0 = AL_SRDS_REG_PAGE_0_LANE_0,
	/* Relevant to Serdes hssp and 25g */
	AL_SRDS_LANE_1 = AL_SRDS_REG_PAGE_1_LANE_1,
	/* Relevant to Serdes hssp only */
	AL_SRDS_LANE_2 = AL_SRDS_REG_PAGE_2_LANE_2,
	AL_SRDS_LANE_3 = AL_SRDS_REG_PAGE_3_LANE_3,

	AL_SRDS_NUM_LANES,
	AL_SRDS_LANES_0123 = AL_SRDS_REG_PAGE_0123_LANES_0123,
};

#define AL_SRDS_25G_NUM_LANES	2

/** SerDes group mode */
enum al_mod_serdes_group_mode {
	/*
	 * Lanes 0-3: off
	 *
	 * Supported by groups: All
	 */
	AL_SRDS_CFG_OFF,

	/*
	 * Lanes 0-3: initialization skipped
	 *
	 * Supported by groups: All
	 */
	AL_SRDS_CFG_SKIP,

	/*
	 * Lanes 0-3: off, but support L2R/R2L clock bypass
	 *
	 * Supported by groups: All
	 */
	AL_SRDS_CFG_OFF_BP,

	/*
	 * Lanes 0-1: PCIE A Gen 2 x2
	 * Lanes 2-3: PCIE B Gen 2 x2
	 *
	 * Supported by groups: A
	 */
	AL_SRDS_CFG_PCIE_A_G2X2_PCIE_B_G2X2,

	/*
	 * Lanes 0-3: PCIE A Gen 2 x4
	 *
	 * Supported by groups: A
	 */
	AL_SRDS_CFG_PCIE_A_G2X4,

	/*
	 * Lanes 0-3: PCIE A Gen 3 x4
	 *
	 * Supported by groups: A
	 */
	AL_SRDS_CFG_PCIE_A_G3X4,

	/*
	 * Lanes 0-3: part of PCIE A Gen 3 x8
	 *
	 * Supported by groups: A (lanes 0-3), B (lanes 4-7)
	 * (Alpine V2 only)
	 */
	AL_SRDS_CFG_PCIE_A_G3X8,

	/*
	 * Lanes 0-3: PCIE B Gen 2 x4
	 *
	 * Supported by groups: B
	 */
	AL_SRDS_CFG_PCIE_B_G2X4,

	/*
	 * Lanes 0-3: PCIE B Gen 3 x4
	 *
	 * Supported by groups: B
	 */
	AL_SRDS_CFG_PCIE_B_G3X4,

	/*
	 * Lanes 0-1: PCIE C Gen 2 x2
	 * Lanes 2-3: PCIE D Gen 2 x2
	 *
	 * Supported by groups: C
	 * (Alpine V2 only)
	 */
	AL_SRDS_CFG_PCIE_C_G2X2_PCIE_D_G2X2,

	/*
	 * Lanes 0-3: PCIE C Gen 2 x4
	 *
	 * Supported by groups: C
	 */
	AL_SRDS_CFG_PCIE_C_G2X4,

	/*
	 * Lanes 0-3: PCIE C Gen 3 x4
	 *
	 * Supported by groups: C
	 */
	AL_SRDS_CFG_PCIE_C_G3X4,

	/*
	 * Lanes 0-3: PCIE D Gen 2 x4
	 *
	 * Supported by groups: D
	 * (Alpine V2 only)
	 */
	AL_SRDS_CFG_PCIE_D_G2X4,

	/*
	 * Lanes 0-3: PCIE D Gen 3 x4
	 *
	 * Supported by groups: D
	 * (Alpine V2 only)
	 */
	AL_SRDS_CFG_PCIE_D_G3X4,

	/*
	 * Lane 0: SATA A 0
	 * Lane 1: SATA A 1
	 * Lane 2: SATA A 2
	 * Lane 3: SATA A 3
	 *
	 * Supported by groups: B, D
	 * (Group D on Alpine V2 only)
	 */
	AL_SRDS_CFG_SATA_A,

	/*
	 * Lane 0: SATA B 0
	 * Lane 1: SATA B 1
	 * Lane 2: SATA B 2
	 * Lane 3: SATA B 3
	 *
	 * Supported by groups: C
	 */
	AL_SRDS_CFG_SATA_B,

	/*
	 * Lane 0: SGMII A
	 * Lane 1: SGMII B
	 * Lane 2: SGMII C
	 * Lane 3: SGMII D
	 *
	 * Supported by groups: C, D
	 */
	AL_SRDS_CFG_SGMII,

	/*
	 * Lane 0: SGMII 2.5G A
	 * Lane 1: SGMII 2.5G B
	 * Lane 2: SGMII 2.5G C
	 * Lane 3: SGMII 2.5G D
	 *
	 * Supported by groups: C, D
	 */
	AL_SRDS_CFG_SGMII_2_5G,

	/*
	 * Lane 0: KR A
	 * Lane 1: KR B
	 * Lane 2: KR C
	 * Lane 3: KR D
	 *
	 * Supported by groups: C, D, E
	 * Supported for test mode by groups: A, B
	 */
	AL_SRDS_CFG_KR,

	/*
	 * Lane 0-3: Ethernet A XLAUI
	 *
	 * Supported by groups: C
	 * (Alpine V2 only)
	 */
	AL_SRDS_CFG_ETH_A_XLAUI,

	/*
	 * Lane 0-3: Ethernet C XLAUI
	 *
	 * Supported by groups: D
	 * (Alpine V2 only)
	 */
	AL_SRDS_CFG_ETH_C_XLAUI,

	/*
	 * Lane 0-1: Ethernet 25G
	 *
	 * Supported by groups: E
	 * (Alpine V2 only)
	 */
	AL_SRDS_CFG_ETH_25G,
};

/* SerDes group mode class */
enum al_mod_serdes_group_mode_class {
	AL_SRDS_MODE_CLASS_UNKNOWN,
	AL_SRDS_MODE_CLASS_PCIE,
	AL_SRDS_MODE_CLASS_ETH,
	AL_SRDS_MODE_CLASS_SATA,
};

/** Serdes loopback mode */
enum al_mod_serdes_lb_mode {
	/** No loopback */
	AL_SRDS_LB_MODE_OFF,

	/**
	 * Transmits the untimed, partial equalized RX signal out the transmit
	 * IO pins.
	 * No clock used (untimed)
	 */
	AL_SRDS_LB_MODE_PMA_IO_UN_TIMED_RX_TO_TX,

	/**
	 * Loops back the TX serializer output into the CDR.
	 * CDR recovered bit clock used (without attenuation)
	 */
	AL_SRDS_LB_MODE_PMA_INTERNALLY_BUFFERED_SERIAL_TX_TO_RX,

	/**
	 * Loops back the TX driver IO signal to the RX IO pins
	 * CDR recovered bit clock used (only through IO)
	 */
	AL_SRDS_LB_MODE_PMA_SERIAL_TX_IO_TO_RX_IO,

	/**
	 * Parallel loopback from the PMA receive lane data ports, to the
	 * transmit lane data ports
	 * CDR recovered bit clock used
	 */
	AL_SRDS_LB_MODE_PMA_PARALLEL_RX_TO_TX,

	/** Loops received data after elastic buffer to transmit path */
	AL_SRDS_LB_MODE_PCS_PIPE,

	/** Loops TX data (to PMA) to RX path (instead of PMA data) */
	AL_SRDS_LB_MODE_PCS_NEAR_END,

	/** Loops receive data prior to interface block to transmit path */
	AL_SRDS_LB_MODE_PCS_FAR_END,

	/** SerDes wrapper based Rx to Tx FIFO loopback */
	AL_SRDS_LB_MODE_WRAPPER_RX_TO_TX_FIFO,
};

enum al_mod_serdes_clk_freq {
	AL_SRDS_CLK_FREQ_NA,
	AL_SRDS_CLK_FREQ_100_MHZ,
	AL_SRDS_CLK_FREQ_125_MHZ,
	AL_SRDS_CLK_FREQ_156_MHZ,
};

enum al_mod_serdes_clk_src {
	AL_SRDS_CLK_SRC_LOGIC_0,
	AL_SRDS_CLK_SRC_REF_PINS,
	AL_SRDS_CLK_SRC_R2L,
	AL_SRDS_CLK_SRC_R2L_PLL,
	AL_SRDS_CLK_SRC_L2R,
};

/** Serdes BIST pattern */
enum al_mod_serdes_bist_pattern {
	AL_SRDS_BIST_PATTERN_USER,
	AL_SRDS_BIST_PATTERN_PRBS7,
	AL_SRDS_BIST_PATTERN_PRBS23,
	AL_SRDS_BIST_PATTERN_PRBS31,
	AL_SRDS_BIST_PATTERN_CLK1010,
	AL_SRDS_BIST_PATTERN_PRBS9,
	AL_SRDS_BIST_PATTERN_PRBS11,
	AL_SRDS_BIST_PATTERN_PRBS15,
};

/** SerDes group rate */
enum al_mod_serdes_rate {
	AL_SRDS_RATE_1_8,
	AL_SRDS_RATE_1_4,
	AL_SRDS_RATE_1_2,
	AL_SRDS_RATE_FULL,
};

/** SerDes power mode */
enum al_mod_serdes_pm {
	AL_SRDS_PM_PD,
	AL_SRDS_PM_P2,
	AL_SRDS_PM_P1,
	AL_SRDS_PM_P0S,
	AL_SRDS_PM_P0,
};

/**
 * Tx de-emphasis parameters
 */
enum al_mod_serdes_tx_deemph_param {
	AL_SERDES_TX_DEEMP_C_ZERO,	/*< c(0) */
	AL_SERDES_TX_DEEMP_C_PLUS,	/*< c(1) */
	AL_SERDES_TX_DEEMP_C_MINUS,	/*< c(-1) */
};

struct al_mod_serdes_adv_tx_params {
	/*
	 * select the input values location.
	 * When set to true the values will be taken from the internal registers
	 * that will be override with the next following parameters.
	 * When set to false the values will be taken from external pins (the
	 * other parameters in this case is not needed)
	 */
	al_mod_bool				override;
	/*
	 * Transmit Amplitude control signal. Used to define the full-scale
	 * maximum swing of the driver.
	 *	000 - Not Supported
	 *	001 - 952mVdiff-pkpk
	 *	010 - 1024mVdiff-pkpk
	 *	011 - 1094mVdiff-pkpk
	 *	100 - 1163mVdiff-pkpk
	 *	101 - 1227mVdiff-pkpk
	 *	110 - 1283mVdiff-pkpk
	 *	111 - 1331mVdiff-pkpk
	 */
	uint8_t				amp;
	/* Defines the total number of driver units allocated in the driver */
	uint8_t				total_driver_units;
	/* Defines the total number of driver units allocated to the
	 * first post-cursor (C+1) tap. */
	uint8_t				c_plus_1;
	/* Defines the total number of driver units allocated to the
	 * second post-cursor (C+2) tap. */
	uint8_t				c_plus_2;
	/* Defines the total number of driver units allocated to the
	 * first pre-cursor (C-1) tap. */
	uint8_t				c_minus_1;
	/* TX driver Slew Rate control:
	 *	00 - 31ps
	 *	01 - 33ps
	 *	10 - 68ps
	 *	11 - 170ps
	 */
	uint8_t				slew_rate;
};

struct al_mod_serdes_adv_rx_params {
	/*
	 * select the input values location.
	 * When set to true the values will be taken from the internal registers
	 * that will be override with the next following parameters.
	 * When set to false the values will be taken based in the equalization
	 * results (the other parameters in this case is not needed)
	 */
	al_mod_bool				override;
	/* RX agc high frequency dc gain:
	 *	-3'b000: -3dB
	 *	-3'b001: -2.5dB
	 *	-3'b010: -2dB
	 *	-3'b011: -1.5dB
	 *	-3'b100: -1dB
	 *	-3'b101: -0.5dB
	 *	-3'b110: -0dB
	 *	-3'b111: 0.5dB
	 */
	uint8_t				dcgain;
	/* DFE post-shaping tap 3dB frequency
	 *	-3'b000: 684MHz
	 *	-3'b001: 576MHz
	 *	-3'b010: 514MHz
	 *	-3'b011: 435MHz
	 *	-3'b100: 354MHz
	 *	-3'b101: 281MHz
	 *	-3'b110: 199MHz
	 *	-3'b111: 125MHz
	 */
	uint8_t				dfe_3db_freq;
	/* DFE post-shaping tap gain
	 *	0: no pulse shaping tap
	 *	1: -24mVpeak
	 *	2: -45mVpeak
	 *	3: -64mVpeak
	 *	4: -80mVpeak
	 *	5: -93mVpeak
	 *	6: -101mVpeak
	 *	7: -105mVpeak
	 */
	uint8_t				dfe_gain;
	/* DFE first tap gain control
	 *	-4'b0000: +1mVpeak
	 *	-4'b0001: +10mVpeak
	 *	....
	 *	-4'b0110: +55mVpeak
	 *	-4'b0111: +64mVpeak
	 *	-4'b1000: -1mVpeak
	 *	-4'b1001: -10mVpeak
	 *	....
	 *	-4'b1110: -55mVpeak
	 *	-4'b1111: -64mVpeak
	 */
	uint8_t				dfe_first_tap_ctrl;
	/* DFE second tap gain control
	 *	-4'b0000: +0mVpeak
	 *	-4'b0001: +9mVpeak
	 *	....
	 *	-4'b0110: +46mVpeak
	 *	-4'b0111: +53mVpeak
	 *	-4'b1000: -0mVpeak
	 *	-4'b1001: -9mVpeak
	 *	....
	 *	-4'b1110: -46mVpeak
	 *	-4'b1111: -53mVpeak
	 */
	uint8_t				dfe_secound_tap_ctrl;
	/* DFE third tap gain control
	 *	-4'b0000: +0mVpeak
	 *	-4'b0001: +7mVpeak
	 *	....
	 *	-4'b0110: +38mVpeak
	 *	-4'b0111: +44mVpeak
	 *	-4'b1000: -0mVpeak
	 *	-4'b1001: -7mVpeak
	 *	....
	 *	-4'b1110: -38mVpeak
	 *	-4'b1111: -44mVpeak
	 */
	uint8_t				dfe_third_tap_ctrl;
	/* DFE fourth tap gain control
	 *	-4'b0000: +0mVpeak
	 *	-4'b0001: +6mVpeak
	 *	....
	 *	-4'b0110: +29mVpeak
	 *	-4'b0111: +33mVpeak
	 *	-4'b1000: -0mVpeak
	 *	-4'b1001: -6mVpeak
	 *	....
	 *	-4'b1110: -29mVpeak
	 *	-4'b1111: -33mVpeak
	 */
	uint8_t				dfe_fourth_tap_ctrl;
	/* Low frequency agc gain (att) select
	 *	-3'b000: Disconnected
	 *	-3'b001: -18.5dB
	 *	-3'b010: -12.5dB
	 *	-3'b011: -9dB
	 *	-3'b100: -6.5dB
	 *	-3'b101: -4.5dB
	 *	-3'b110: -2.9dB
	 *	-3'b111: -1.6dB
	 */
	uint8_t				low_freq_agc_gain;
	/* Provides a RX Equalizer pre-hint, prior to beginning
	 * adaptive equalization */
	uint8_t				precal_code_sel;
	/* High frequency agc boost control
	 *	Min d0: Boost ~4dB
	 *	Max d31: Boost ~20dB
	 */
	uint8_t				high_freq_agc_boost;
};

struct al_mod_serdes_25g_adv_rx_params {
	/* ATT (PLE Flat-Band Gain) */
	uint8_t				att;
	/* APG (CTLE's Flat-Band Gain) */
	uint8_t				apg;
	/* LFG (Low-Freq Gain) */
	uint8_t				lfg;
	/* HFG (High-Freq Gain) */
	uint8_t				hfg;
	/* MBG (MidBand-Freq-knob Gain) */
	uint8_t				mbg;
	/* MBF (MidBand-Freq-knob Frequency position Gain) */
	uint8_t				mbf;
	/* DFE Tap1 even#0 Value */
	int8_t				dfe_first_tap_even0_ctrl;
	/* DFE Tap1 even#1 Value */
	int8_t				dfe_first_tap_even1_ctrl;
	/* DFE Tap1 odd#0 Value */
	int8_t				dfe_first_tap_odd0_ctrl;
	/* DFE Tap1 odd#1 Value */
	int8_t				dfe_first_tap_odd1_ctrl;
	/* DFE Tap2 Value */
	int8_t				dfe_second_tap_ctrl;
	/* DFE Tap3 Value */
	int8_t				dfe_third_tap_ctrl;
	/* DFE Tap4 Value */
	int8_t				dfe_fourth_tap_ctrl;
	/* DFE Tap5 Value */
	int8_t				dfe_fifth_tap_ctrl;
};

struct al_mod_serdes_25g_tx_diag_info {
	uint8_t regulated_supply;
	int8_t dcd_trim;
	uint8_t clk_delay;
	uint8_t calp_multiplied_by_2;
	uint8_t caln_multiplied_by_2;
};

struct al_mod_serdes_25g_rx_diag_info {
	int8_t los_offset;
	int8_t agc_offset;
	int8_t leq_gainstage_offset;
	int8_t leq_eq1_offset;
	int8_t leq_eq2_offset;
	int8_t leq_eq3_offset;
	int8_t leq_eq4_offset;
	int8_t leq_eq5_offset;
	int8_t summer_even_offset;
	int8_t summer_odd_offset;
	int8_t vscan_even_offset;
	int8_t vscan_odd_offset;
	int8_t data_slicer_even0_offset;
	int8_t data_slicer_even1_offset;
	int8_t data_slicer_odd0_offset;
	int8_t data_slicer_odd1_offset;
	int8_t edge_slicer_even_offset;
	int8_t edge_slicer_odd_offset;
	int8_t eye_slicer_even_offset;
	int8_t eye_slicer_odd_offset;
	uint8_t cdr_clk_i;
	uint8_t cdr_clk_q;
	uint8_t cdr_dll;
	uint8_t cdr_vco_dosc;
	uint8_t cdr_vco_fr;
	uint16_t cdr_dlpf;
	uint8_t ple_resistance;
	uint8_t rx_term_mode;
	uint8_t rx_coupling;
	uint8_t rx_term_cal_code;
	uint8_t rx_sheet_res_cal_code;
};

struct al_mod_serdes_25g_fw_init_status {
	unsigned int top_error;
	unsigned int cm_error;
	unsigned int cm_critical_error;
	unsigned int lane_error[AL_SRDS_25G_NUM_LANES];
	unsigned int lane_critical_error[AL_SRDS_25G_NUM_LANES];
};

/** AVG SerDes FW init status */
struct al_mod_serdes_avg_fw_init_status {
	/** CRC error */
	al_mod_bool		crc_err;
	/** FW revision */
	unsigned int	fw_rev;
	/** FW engineering revision */
	unsigned int	fw_rev_eng;
	/** FW build number */
	unsigned int	fw_build;
};

/**
 * SRIS parameters
 */
struct al_mod_serdes_sris_params {
	/* Controls the frequency accuracy threshold (ppm) for lock detection CDR */
	uint16_t	ppm_drift_count;
	/* Controls the frequency accuracy threshold (ppm) for lock detection in the CDR */
	uint16_t	ppm_drift_max;
	/* Controls the frequency accuracy threshold (ppm) for lock detection in PLL */
	uint16_t	synth_ppm_drift_max;
	/* Elastic buffer full threshold for PCIE modes: GEN1/GEN2 */
	uint8_t		full_d2r1;
	/* Elastic buffer full threshold for PCIE modes: GEN3 */
	uint8_t		full_pcie_g3;
	/* Elastic buffer midpoint threshold.
	 * Sets the depth of the buffer while in PCIE mode, GEN1/GEN2
	 */
	uint8_t		rd_threshold_d2r1;
	/* Elastic buffer midpoint threshold.
	 * Sets the depth of the buffer while in PCIE mode, GEN3
	 */
	uint8_t		rd_threshold_pcie_g3;
};

/** SerDes PCIe Rate - values are important for proper behavior */
enum al_mod_serdes_pcie_rate {
	AL_SRDS_PCIE_RATE_GEN1 = 0,
	AL_SRDS_PCIE_RATE_GEN2,
	AL_SRDS_PCIE_RATE_GEN3,
};

/** SerDes TX PLL WA Status */
struct al_mod_serdes_tx_pll_wa_status {
	/** This status structure is valid */
	al_mod_bool valid;
	/** Temperature on sampling */
	int temperature;
	/** CMU VCO swing init value */
	int swing_init;
	/** CMU VCO swing adjusted value */
	int swing_adjusted;
	/** TX PLL is inside window on start of searching for window size */
	al_mod_bool pll_inside_window_on_start;
	/** DSOC window available */
	al_mod_bool dosc_window_available;
	/** DOSC value on init */
	int dosc_init;
	/** DOSC window start */
	int dosc_window_start;
	/** DOSC window end */
	int dosc_window_end;
	/** Target DOSC */
	int dosc_target;
	/** Was TX PLL finally locked after all WA procedures */
	al_mod_bool locked_final;
};

/** SerDes mode set advanced parameters */
struct al_mod_serdes_mode_set_adv_params {
	enum al_mod_serdes_group_mode group_mode;

	/** TX PLL WA status - filled if not NULL */
	struct al_mod_serdes_tx_pll_wa_status *tx_pll_wa_status;
};

struct al_mod_serdes_grp_obj {
	void __iomem				*regs_base;

	/** iofic base address - Relevant to SerDes ABG only */
	void __iomem				*int_mem_base;

	/** Parent complex - Relevant to SerDes AVG and SNPS only */
	struct al_mod_serdes_complex_obj		*complex_obj;
	/** Lane in parent complex - Relevant to SerDes AVG only */
	unsigned int				lane_in_complex;
	/** Last set BIST pattern - Relevant to SerDes AVG only */
	enum al_mod_serdes_bist_pattern		bist_pattern;
	al_mod_bool					bist_pattern_valid;
	/** Core interrupt debug enabled - Relevant to SerDes AVG only */
	al_mod_bool					core_int_dbg_en;
	/** PCIe Rx equalization config sequence state - Relevant to SerDes AVG only */
	unsigned int				pcie_ical_config_seq_state;
	/** Quad in parent complex - Relevant to SerDes SNPS only */
	unsigned int				quad_in_complex;

	/**
	 * get the type of the serdes.
	 * Must be implemented for all SerDes unit.
	 *
	 * @return the serdes type.
	 */
	enum al_mod_serdes_type (*type_get)(void);

	/**
	 * Reads a SERDES internal register
	 *
	 * @param obj		The object context
	 * @param page		The SERDES register page within the group
	 * @param type		The SERDES register type (PMA /PCS)
	 * @param offset	The SERDES register offset (0 - 4095)
	 * @param data		The read data
	 *
	 * @return 0 if no error found.
	 */
	int (*reg_read)(struct al_mod_serdes_grp_obj *, enum al_mod_serdes_reg_page,
			enum al_mod_serdes_reg_type, uint16_t, uint8_t *);

	/**
	 * Writes a SERDES internal register
	 *
	 * @param obj		The object context
	 * @param page		The SERDES register page within the group
	 * @param type		The SERDES register type (PMA /PCS)
	 * @param offset	The SERDES register offset (0 - 4095)
	 * @param data		The data to write
	 *
	 * @return 0 if no error found.
	 */
	int (*reg_write)(struct al_mod_serdes_grp_obj *, enum al_mod_serdes_reg_page,
			enum al_mod_serdes_reg_type, uint16_t, uint8_t);

	/**
	 * Enable BIST required overrides
	 *
	 * @param obj		The object context
	 * @param grp		The SERDES group
	 * @param rate		The required speed rate
	 */
	void (*bist_overrides_enable)(struct al_mod_serdes_grp_obj *, enum al_mod_serdes_rate);
	/**
	 * Disable BIST required overrides
	 *
	 * @param obj		The object context
	 * @param grp		The SERDES group
	 * @param rate		The required speed rate
	 */
	void (*bist_overrides_disable)(struct al_mod_serdes_grp_obj *);
	/**
	 * Rx rate change
	 *
	 * @param obj		The object context
	 * @param grp		The SERDES group
	 * @param rate		The Rx required rate
	 */
	void (*rx_rate_change)(struct al_mod_serdes_grp_obj *, enum al_mod_serdes_rate);
	/**
	 * Soft reset RX
	 *
	 * @param obj		The object context
	 * @param lane		The SERDES lane within the group
	 */
	void (*lane_rx_soft_reset)(struct al_mod_serdes_grp_obj *, enum al_mod_serdes_lane);
	/**
	 * Check if Rx/Tx lane is ready
	 *
	 * @param obj		The object context
	 * @param lane		The SERDES lane within the group
	 */
	al_mod_bool (*lane_ready)(struct al_mod_serdes_grp_obj *, enum al_mod_serdes_lane);
	/**
	 * SERDES lane Rx rate change software flow enable
	 *
	 * @param obj		The object context
	 * @param lane		The SERDES lane within the group
	 */
	void (*rx_rate_change_sw_flow_en)(struct al_mod_serdes_grp_obj *, enum al_mod_serdes_lane);
	/**
	 * SERDES lane Rx rate change software flow disable
	 *
	 * @param obj		The object context
	 * @param lane		The SERDES lane within the group
	 */
	void (*rx_rate_change_sw_flow_dis)(struct al_mod_serdes_grp_obj *, enum al_mod_serdes_lane);
	/**
	 * PCIe lane rate override check
	 *
	 * @param obj		The object context
	 * @param grp		The SERDES group
	 * @param lane		The SERDES lane within the group
	 *
	 * @returns	AL_TRUE if the override is enabled
	 */
	al_mod_bool (*pcie_rate_override_is_enabled)(struct al_mod_serdes_grp_obj *, enum al_mod_serdes_lane);
	/**
	 * PCIe lane rate override control
	 *
	 * @param obj		The object context
	 * @param lane		The SERDES lane within the group
	 * @param en		Enable/disable
	 */
	void (*pcie_rate_override_enable_set)(struct al_mod_serdes_grp_obj *, enum al_mod_serdes_lane,
					      al_mod_bool en);
	/**
	 * PCIe lane rate get
	 *
	 * @param obj		The object context
	 * @param lane		The SERDES lane within the group
	 */
	enum al_mod_serdes_pcie_rate (*pcie_rate_get)(struct al_mod_serdes_grp_obj *, enum al_mod_serdes_lane);
	/**
	 * PCIe lane rate set
	 *
	 * @param obj		The object context
	 * @param lane		The SERDES lane within the group
	 * @param rate		The required rate
	 */
	void (*pcie_rate_set)(struct al_mod_serdes_grp_obj *, enum al_mod_serdes_lane,
			      enum al_mod_serdes_pcie_rate rate);
	/**
	 * SERDES group power mode control
	 *
	 * @param obj		The object context
	 * @param grp		The SERDES group
	 * @param pm		The required power mode
	 */
	void (*group_pm_set)(struct al_mod_serdes_grp_obj *, enum al_mod_serdes_pm);
	/**
	 * SERDES lane power mode control
	 *
	 * @param obj		The object context
	 * @param grp		The SERDES group
	 * @param lane		The SERDES lane within the group
	 * @param rx_pm		The required RX power mode
	 * @param tx_pm		The required TX power mode
	 */
	void (*lane_pm_set)(struct al_mod_serdes_grp_obj *, enum al_mod_serdes_lane,
			    enum al_mod_serdes_pm, enum al_mod_serdes_pm);

	/**
	 * SERDES group PMA hard reset
	 * Controls Serdes group PMA hard reset
	 *
	 * @param obj		The object context
	 * @param grp		The SERDES group
	 * @param enable	Enable/disable hard reset
	 */
	void (*pma_hard_reset_group)(struct al_mod_serdes_grp_obj *, al_mod_bool);
	/**
	 * SERDES lane PMA hard reset
	 * Controls Serdes lane PMA hard reset
	 *
	 * @param obj		The object context
	 * @param lane		The SERDES lane within the group
	 * @param enable	Enable/disable hard reset
	 */
	void (*pma_hard_reset_lane)(struct al_mod_serdes_grp_obj *, enum al_mod_serdes_lane, al_mod_bool);
	/**
	 * Configure SERDES loopback
	 * Controls the loopback
	 *
	 * @param obj		The object context
	 * @param lane		The SERDES lane within the group
	 * @param mode		The requested loopback mode
	 */
	void (*loopback_control)(struct al_mod_serdes_grp_obj *, enum al_mod_serdes_lane,
				 enum al_mod_serdes_lb_mode);
	/**
	 * SERDES BIST pattern selection
	 * Selects the BIST pattern to be used
	 *
	 * @param obj		The object context
	 * @param pattern	The pattern to set
	 * @param user_data	The pattern user data (when pattern == AL_SRDS_BIST_PATTERN_USER)
	 *			80 bits (8 bytes array)
	 */
	void (*bist_pattern_select)(struct al_mod_serdes_grp_obj *,
				    enum al_mod_serdes_bist_pattern, uint8_t *);
	/**
	 * SERDES BIST TX Enable
	 * Enables/disables TX BIST per lane
	 *
	 * @param obj		The object context
	 * @param lane		The SERDES lane within the group
	 * @param enable	Enable or disable TX BIST
	 */
	void (*bist_tx_enable)(struct al_mod_serdes_grp_obj *, enum al_mod_serdes_lane, al_mod_bool);
	/**
	 * SERDES BIST TX single bit error injection
	 * Injects single bit error during a TX BIST
	 *
	 * @param obj		The object context
	 */
	void (*bist_tx_err_inject)(struct al_mod_serdes_grp_obj *);
	/**
	 * SERDES BIST RX Enable
	 * Enables/disables RX BIST per lane
	 *
	 * @param obj		The object context
	 * @param lane		The SERDES lane within the group
	 * @param enable	Enable or disable TX BIST
	 */
	void (*bist_rx_enable)(struct al_mod_serdes_grp_obj *, enum al_mod_serdes_lane, al_mod_bool);
	/**
	 * SERDES BIST RX status
	 * Checks the RX BIST status for a specific SERDES lane
	 *
	 * @param obj		The object context
	 * @param lane		The SERDES lane within the group
	 * @param is_locked	An indication whether RX BIST is locked
	 * @param err_cnt_overflow	An indication whether error count overflow occured
	 * @param err_cnt	Current bit error count
	 */
	void (*bist_rx_status)(struct al_mod_serdes_grp_obj *, enum al_mod_serdes_lane, al_mod_bool *,
			       al_mod_bool *, uint32_t *);

	/**
	 * Set the tx de-emphasis to preset values
	 *
	 * @param obj		The object context
	 * @param lane		The SERDES lane within the group
	 *
	 */
	void (*tx_deemph_preset)(struct al_mod_serdes_grp_obj *, enum al_mod_serdes_lane);
	/**
	 * Increase tx de-emphasis param.
	 *
	 * @param obj		The object context
	 * @param lane		The SERDES lane within the group
	 * @param param		which tx de-emphasis to change
	 *
	 * @return false in case max is reached. true otherwise.
	 */
	al_mod_bool (*tx_deemph_inc)(struct al_mod_serdes_grp_obj *, enum al_mod_serdes_lane,
				 enum al_mod_serdes_tx_deemph_param);
	/**
	 * Decrease tx de-emphasis param.
	 *
	 * @param obj		The object context
	 * @param lane		The SERDES lane within the group
	 * @param param		which tx de-emphasis to change
	 *
	 * @return false in case min is reached. true otherwise.
	 */
	al_mod_bool (*tx_deemph_dec)(struct al_mod_serdes_grp_obj *, enum al_mod_serdes_lane,
				 enum al_mod_serdes_tx_deemph_param);
	/**
	 * run Rx eye measurement.
	 *
	 * @param obj		The object context
	 * @param lane		The SERDES lane within the group
	 * @param timeout	timeout in uSec
	 * @param value		Rx eye measurement value
	 *			(0 - completely closed eye, 0xffff - completely open eye).
	 *
	 * @return 0 if no error found.
	 */
	int (*eye_measure_run)(struct al_mod_serdes_grp_obj *, enum al_mod_serdes_lane,
			       uint32_t, unsigned int *);

	/**
	 * Eye diagram staged sampling - pre-stage
	 *
	 * @param obj		The object context
	 * @param lane		The SERDES lane within the group
	 * @param sample_count	retrieve last programmed sample count (should be handed back
	 *						in eye_diag_post_sample)
	 * @param rxcal_mask retrieve rxcal register mask for fast start/stop later
	 *						on. Use NULL if not needed.
	 *
	 * @return 0 if no error found.
	 */
	int (*eye_diag_pre_sample)(struct al_mod_serdes_grp_obj *obj, enum al_mod_serdes_lane lane,
			unsigned int *sample_count, unsigned int *rxcal_mask);

	/**
	 * Eye diagram staged sampling - start-stage
	 *
	 * @param obj		The object context
	 * @param lane		The SERDES lane within the group
	 * @param rxcal_mask rxcal register mask for fast start/stop. Use NULL if not needed.
	 * @param x		Sampling X position (0-63 --> -1.00 UI...1.00 UI),-1=ignored
	 * @param y		Sampling Y position (0-62 --> 500mV...-500mV), -1=ignored
	 *
	 * @return 0 if no error found.
	 */
	int (*eye_diag_start_sample)(struct al_mod_serdes_grp_obj *obj, enum al_mod_serdes_lane lane,
			unsigned int *rxcal_mask, int x, int y);

	/**
	 * Eye diagram staged sampling - polling-stage
	 *
	 * @param obj		The object context
	 * @param lane		The SERDES lane within the group
	 *
	 * @return 0/1 - result is ready
	 */
	int (*eye_diag_poll_sample)(struct al_mod_serdes_grp_obj *obj, enum al_mod_serdes_lane lane);

	/**
	 * Eye diagram staged sampling - stop-stage
	 *
	 * @param obj		The object context
	 * @param lane		The SERDES lane within the group
	 * @param rxcal_mask rxcal register mask for fast start/stop. Use NULL if not needed.
	 * @param value		Eye diagram sample value (BER - 0x0000 - 0xffff)
	 *
	 * @return 0 if no error found.
	 */
	int (*eye_diag_stop_sample)(struct al_mod_serdes_grp_obj *obj, enum al_mod_serdes_lane lane,
			unsigned int *rxcal_mask, unsigned int *value);

	/**
	 * Eye diagram staged sampling - post-stage
	 *
	 * @param obj		The object context
	 * @param lane		The SERDES lane within the group
	 * @param sample_count	original sample_count to be re-programmed
	 *
	 * @return 0 if no error found.
	 */
	int (*eye_diag_post_sample)(struct al_mod_serdes_grp_obj *obj, enum al_mod_serdes_lane lane,
			unsigned int sample_count);

	/**
	 * Eye diagram single sampling
	 *
	 * @param obj		The object context
	 * @param lane		The SERDES lane within the group
	 * @param x		Sampling X position (0 - 63 --> -1.00 UI ... 1.00 UI)
	 * @param y		Sampling Y position (0 - 62 --> 500mV ... -500mV)
	 * @param timeout	timeout in uSec
	 * @param value		Eye diagram sample value (BER - 0x0000 - 0xffff)
	 *
	 * @return 0 if no error found.
	 */
	int (*eye_diag_sample)(struct al_mod_serdes_grp_obj *, enum al_mod_serdes_lane,
			       unsigned int, int, unsigned int, unsigned int *);

	/**
	 * Eye diagram full run
	 *
	 * @param obj			The object context
	 * @param lane			The SERDES lane within the group
	 * @param x_start		Sampling from X position
	 * @param x_stop		Sampling to X position
	 * @param x_step		jump in x_step
	 * @param y_start		Sampling from Y position
	 * @param y_stop		Sampling to Y position
	 * @param y_step		jump in y_step
	 * @param num_bits_per_sample	How many bits to check
	 * @param buf			array of results
	 * @param buf_size		array size - must be equal to
	 *				(((y_stop - y_start) / y_step) + 1) *
	 *				(((x_stop - x_start) / x_step) + 1)
	 *
	 * @return 0 if no error found.
	 */
	int (*eye_diag_run)(struct al_mod_serdes_grp_obj	*, enum al_mod_serdes_lane,
			    int, int, unsigned int, int, int, unsigned int, uint64_t, uint64_t *,
			    uint32_t);
	/**
	 * Check if signal is detected
	 *
	 * @param obj		The object context
	 * @param lane		The SERDES lane within the group
	 *
	 * @return true if signal is detected. false otherwise.
	 */
	al_mod_bool (*signal_is_detected)(struct al_mod_serdes_grp_obj *, enum al_mod_serdes_lane);

	/**
	 * Check if CDR is locked
	 *
	 * @param obj		The object context
	 * @param lane		The SERDES lane within the group
	 *
	 * @return true if cdr is locked. false otherwise.
	 */
	al_mod_bool (*cdr_is_locked)(struct al_mod_serdes_grp_obj *, enum al_mod_serdes_lane);

	/**
	 * Check if rx is valid for this lane
	 *
	 * @param obj		The object context
	 * @param lane		The SERDES lane within the group
	 *
	 * @return true if rx is valid. false otherwise.
	 */
	al_mod_bool (*rx_valid)(struct al_mod_serdes_grp_obj *, enum al_mod_serdes_lane);

	/**
	 * configure tx advanced parameters
	 *
	 * @param obj		The object context
	 * @param lane		The SERDES lane within the group
	 * @param params	pointer to the tx parameters
	 */
	void (*tx_advanced_params_set)(struct al_mod_serdes_grp_obj *, enum al_mod_serdes_lane, void *);
	/**
	 * read tx advanced parameters
	 *
	 * @param obj		The object context
	 * @param lane		The SERDES lane within the group
	 * @param params	pointer to the tx parameters
	 */
	void (*tx_advanced_params_get)(struct al_mod_serdes_grp_obj *, enum al_mod_serdes_lane, void *);
	/**
	 * configure rx advanced parameters
	 *
	 * @param obj		The object context
	 * @param lane		The SERDES lane within the group
	 * @param params	pointer to the rx parameters
	 */
	void (*rx_advanced_params_set)(struct al_mod_serdes_grp_obj *, enum al_mod_serdes_lane, void *);
	/**
	 * read rx advanced parameters
	 *
	 * @param obj		The object context
	 * @param lane		The SERDES lane within the group
	 * @param params	pointer to the rx parameters
	 */
	void (*rx_advanced_params_get)(struct al_mod_serdes_grp_obj *, enum al_mod_serdes_lane, void *);
	/**
	 *  Switch entire SerDes group to SGMII mode based on 156.25 Mhz reference clock
	 *
	 * @param obj		The object context
	 *
	 */
	void (*mode_set_sgmii)(struct al_mod_serdes_grp_obj *);
	/**
	 *  Switch entire SerDes group to KR mode based on 156.25 Mhz reference clock
	 *
	 * @param obj		The object context
	 *
	 */
	int (*mode_set_kr)(struct al_mod_serdes_grp_obj *);
	/**
	 *  Switch entire SerDes group to 25g mode
	 *
	 * @param obj		The object context
	 *
	 */
	int (*mode_set_25g)(struct al_mod_serdes_grp_obj *);
	/**
	 *  Switch entire SerDes group to specified mode with advanced parameters
	 *
	 * @param obj		The object context
	 * @param params	The mode set advanced parameters
	 *
	 */
	int (*mode_set_adv)(struct al_mod_serdes_grp_obj *,
		struct al_mod_serdes_mode_set_adv_params *);
	/**
	 * Resets SerDes HW equalization engine
	 * Call it in case rx_equalization returns 0
	 *
	 * @param obj		the object context
	 * @param lane		The SERDES lane within the group
	 */
	void (*rx_equalization_reset)(struct al_mod_serdes_grp_obj *, enum al_mod_serdes_lane);
	/**
	 * performs SerDes HW equalization test and update equalization parameters
	 *
	 * @param obj		the object context
	 * @param lane		The SERDES lane within the group
	 */
	int (*rx_equalization)(struct al_mod_serdes_grp_obj *, enum al_mod_serdes_lane);
	/**
	 * performs Rx equalization and compute the width and height of the eye
	 *
	 * @param obj		the object context
	 * @param lane		The SERDES lane within the group
	 * @param width		the output width of the eye
	 * @param height	the output height of the eye
	 */
	int (*calc_eye_size)(struct al_mod_serdes_grp_obj *, enum al_mod_serdes_lane, int *, int *);
	/**
	 * SRIS: Separate Refclk Independent SSC (Spread Spectrum Clocking)
	 * Currently available only for PCIe interfaces.
	 * When working with local Refclk, same SRIS configuration in both serdes sides
	 * (EP and RC in PCIe interface) is required.
	 *
	 * performs SRIS configuration according to params
	 *
	 * @param obj		the object context
	 * @param params	the SRIS parameters
	 */
	void (*sris_config)(struct al_mod_serdes_grp_obj *, void *);
	/**
	 * set SERDES dcgain parameter
	 *
	 * @param obj		the object context
	 * @param dcgain	dcgain value to set
	 */
	void (*dcgain_set)(struct al_mod_serdes_grp_obj *, uint8_t);
	/**
	 * read tx diagnostics info
	 *
	 * @param obj		The object context
	 * @param lane		The SERDES lane within the group
	 * @param params	pointer to the tx diagnostics info structure
	 */
	void (*tx_diag_info_get)(struct al_mod_serdes_grp_obj *, enum al_mod_serdes_lane, void *);
	/**
	 * read rx diagnostics info
	 *
	 * @param obj		The object context
	 * @param lane		The SERDES lane within the group
	 * @param params	pointer to the rx diagnostics info structure
	 */
	void (*rx_diag_info_get)(struct al_mod_serdes_grp_obj *, enum al_mod_serdes_lane, void *);
	/**
	 * PCS interrupt enable/disable
	 *
	 * @param obj		Thge object context
	 * @param en		PCS interrupt enable (non zero) / disable (zero)
	 */
	void (*pcs_interrupt_enable_set)(struct al_mod_serdes_grp_obj *obj, al_mod_bool en);
	/**
	 * Controls which execution of RX_EQ_EVAL will launch the iCal procedure (PCIe Gen 3 only)
	 *
	 * @param obj		Thge object context
	 */
	void (*pcie_run_ical_config)(struct al_mod_serdes_grp_obj *obj);
	/**
	 * Rx equalization config sequence begin (PCIe Gen 3 only)
	 *
	 * @param obj		Thge object context
	 */
	void (*pcie_ical_config_begin)(struct al_mod_serdes_grp_obj *obj);
	/**
	 * Rx equalization config sequence iteration (PCIe Gen 3 only)
	 *
	 * @param obj		Thge object context
	 * @return		AL_TRUE if additional calls to pcie_ical_config_itrerate are
	 *			required
	 */
	al_mod_bool (*pcie_ical_config_iterate)(struct al_mod_serdes_grp_obj *obj);
	/**
	 * callback to read temperature
	 *
	 * @param obj		The object context
	 * @return current temperature [degrees celcius]
	 */
	int (*temperature_get_cb)(void);
	/**
	 * read pll lock status
	 *
	 * @param obj		The object context
	 * @return AL_TRUE if pll is locked. AL_FALSE otherwise.
	 */
	al_mod_bool (*pll_lock_get)(struct al_mod_serdes_grp_obj *);
	/**
	 * read fw init status
	 *
	 * @param obj				The object context
	 * @param fw_init_status	pointer to fw init status structure
	 */
	void (*fw_init_status_get)(struct al_mod_serdes_grp_obj *, void *);
	/**
	 * check if fw is alive
	 *
	 * @param obj				The object context
	 * @return AL_TRUE if fw is alive
	 */
	al_mod_bool (*fw_is_alive)(struct al_mod_serdes_grp_obj *);
	/**
	 * callback to udelay function
	 *
	 * @param context	udelay callback context
	 * @param delay		delay period in us
	 */
	void (*udelay_cb)(void *context, unsigned int delay);
	void *udelay_cb_context;
	/**
	 * callback to msleep function
	 *
	 * @param context	msleep callback context
	 * @param delay		sleep period in ms
	 */
	void (*msleep_cb)(void *context, unsigned int delay);
	void *msleep_cb_context;
	/**
	 * Get current mode_class of the serdes
	 *
	 * @param obj				The object context
	 * @param serdes_group_mode_class	pointer through which to return current
	 *					serdes_group_mode_class
	 */
	void (*serdes_mode_class_get)(struct al_mod_serdes_grp_obj *,
				      enum al_mod_serdes_group_mode_class *);
	/**
	 * Get current mode of the serdes
	 *
	 * @param obj			The object context
	 * @param serdes_group_mode	pointer through which to return current serdes_group_mode
	 *
	 * @return 0 on success, < 0 on fail
	 *
	 * @note This function can be used only for serdes groups which are known to be ETH ports
	 */
	int (*serdes_mode_get)(struct al_mod_serdes_grp_obj *, enum al_mod_serdes_group_mode *);

	/**
	 * Rx initial calibration start
	 *
	 * May take few seconds to complete
	 * Use 'ical_wait_for_completion' to wait for completion
	 *
	 * @param	obj
	 *		SerDes group object
	 * @param	lane
	 *		SerDes lane number within the group
	 *
	 * @return 0 on success, < 0 on fail
	 */
	int (*ical_start)(struct al_mod_serdes_grp_obj *obj, enum al_mod_serdes_lane lane);

	/**
	 * Rx-TX reset serdes
	 *
	 * @param	obj
	 *		SerDes group object
	 * @param	is_eth
	 *		Ethernet mode indication

	 * @return 0 on success, < 0 on fail
	 */
	int (*rx_tx_reset)(struct al_mod_serdes_grp_obj *obj, al_mod_bool is_eth);

	/**
	 * Rx-TX reset serdes
	 *
	 * @param	obj
	 *		SerDes group object

	 * @return AL_TRUE if serdes reset is done, AL_FALSE otherwise.
	 */
	al_mod_bool (*rx_tx_reset_is_done)(struct al_mod_serdes_grp_obj *obj);

	/**
	 * Rx initial calibration check for completion
	 *
	 * @param	obj
	 *		SerDes group object
	 * @param	lane
	 *		SerDes lane number within the group
	 *
	 * @return AL_TRUE if finished calibration, AL_FALSE otherwise.
	 */
	al_mod_bool (*ical_is_done)(struct al_mod_serdes_grp_obj *obj, enum al_mod_serdes_lane lane);

	/**
	 * Rx initial calibration wait for completion
	 *
	 * @param	obj
	 *		SerDes group object
	 * @param	lane
	 *		SerDes lane number within the group
	 *
	 * @return 0 on success, < 0 on fail
	 */
	int (*ical_wait_for_completion)(struct al_mod_serdes_grp_obj *obj, enum al_mod_serdes_lane lane);

	/**
	 * Rx single shot periodic calibration start
	 *
	 * May take 60 seconds to complete
	 * Use 'pcal_wait_for_completion' to wait for completion
	 *
	 * @param	obj
	 *		SerDes group object
	 * @param	lane
	 *		SerDes lane number within the group
	 *
	 * @return 0 on success, < 0 on fail
	 */
	int (*pcal_start)(struct al_mod_serdes_grp_obj *obj, enum al_mod_serdes_lane lane);

	/**
	 * Rx single shot periodic calibration check for completion
	 *
	 * @param	obj
	 *		SerDes group object
	 * @param	lane
	 *		SerDes lane number within the group
	 *
	 * @return AL_TRUE if finished calibration, AL_FALSE otherwise.
	 */
	al_mod_bool (*pcal_is_done)(struct al_mod_serdes_grp_obj *obj, enum al_mod_serdes_lane lane);

	/**
	 * Rx single shot periodic calibration wait for completion
	 *
	 * @param	obj
	 *		SerDes group object
	 * @param	lane
	 *		SerDes lane number within the group
	 *
	 * @return 0 on success, < 0 on fail
	 */
	int (*pcal_wait_for_completion)(struct al_mod_serdes_grp_obj *obj, enum al_mod_serdes_lane lane);

	/**
	 * Rx adaptive periodic calibration start
	 *
	 * @param	obj
	 *		SerDes group object
	 * @param	lane
	 *		SerDes lane number within the group
	 *
	 * @return 0 on success, < 0 on fail
	 */
	int (*pcal_adaptive_start)(struct al_mod_serdes_grp_obj *obj, enum al_mod_serdes_lane lane);

	/**
	 * Rx adaptive periodic calibration stop
	 *
	 * @param	obj
	 *		SerDes group object
	 * @param	lane
	 *		SerDes lane number within the group
	 *
	 * @return 0 on success, < 0 on fail
	 */
	int (*pcal_adaptive_stop)(struct al_mod_serdes_grp_obj *obj, enum al_mod_serdes_lane lane);

	/**
	 * Get interrupts masks
	 *
	 * @param	obj
	 *		SerDes group object
	 * @param	a
	 *		Group A mask
	 * @param	b
	 *		Group B mask
	 * @param	c
	 *		Group C mask
	 * @param	d
	 *		Group D mask
	 */
	void (*error_masks_get)(struct al_mod_serdes_grp_obj *obj,
		uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d);

	/**
	 * Unmask interrupts for serdes lane
	 *
	 * @param	obj
	 *		SerDes group object
	 */
	void (*errors_unmask)(struct al_mod_serdes_grp_obj *obj);
};


/* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif

/* *INDENT-ON* */
#endif		/* __AL_HAL_SERDES_INTERFACE_H__ */

/** @} end of SERDES group */

