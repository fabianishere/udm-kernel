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
 * @defgroup group_cmos CMOS
 * @ingroup group_ring
 * Alpine PoC includes Low-voltage CMOS pins that are used for various
 * single-ended I/O functionality like RGMII, ULPI, I2C etc.
 * These CMOS pins are in groups of 13 general purpose CMOS buffers.
 *
 * The RGMII groups support I/O BIST - Once triggered, the TX flow submits a
 * user defined pattern out, while the RX flow compares the pattern with the
 * incoming RX data.
 *
 * I/O BIST - TX:
 * Once triggered, the sent pattern will be the following (using TX clock):
 * - Preamble: 8 cycles of '1' on all lines, followed by 8 cycles of '0' –
 *   enabling the external tester to align with the outgoing pattern.
 * - Run – Once done with the preamble cycles, submit the pattern –
 *   8 cycles, which in double data rate submits out 16 phases of pattern.
 * When Run stage is done, the test ends. No status is logged as all the info
 * is well observed by the tester.
 *
 * I/O BIST - RX:
 * Once triggered, the incoming data is polled (Using RX clock):
 * - Preamble: Long term '1', followed by 8 cycles of '0' on all lines -
 *   enabling the internal compare alignment with the incoming pattern.
 * - Run – Once done with the preamble cycles, receive the pattern - 8 cycles,
 *   which in double data rate accepts 16 phases of pattern.
 * When Run stage is done, the test ends, and result and status are logged
 * and can be obtaind through the API mention below, including failing cycle,
 * expected, and acquired data.
 * Incremental Check:
 * Since the RX pattern may fail due to selected bit delay that may need
 * trimming, compare may be hard to do (and even preamble detect) as it looks
 * for complete pattern over the 10 bits (Low 5 and High 5).
 * Therefore – a mask is provided per bit, for enabling screening while bit
 * lines are severely misaligned.
 * An error is logged for a bit:
 * - Only if it was not masked off.
 * - Only the first error detected during the current flow.
 *
 * The API functions below are expecting the group of interest as parameter.
 * Most users do not need to configure or change default behaviour, and most
 * APIs presented here are for internal manufacturing test and chip
 * characterizations. It is typically called by U-Boot or pre-boot stage2.
 * Operating systems (like Linux) will usually not use it.
 *
 *  @{
 *	Common Operation Example for setting data mask, starting RGMII DLL, and
 *	running RGMII built in self test.
 *	Customers should not call the BIST sequence - it is meant for interal
 *	manufacturing test and chip characterizations.
 *
 * @code
 * void main()
 * {
 *	struct al_cmos_handle handle;
 *	struct al_cmos_cfg cfg;
 *	struct al_cmos_rgmii_dll_status dll_status;
 *	struct al_cmos_io_bist_cfg bist_cfg {
 *		.rx_en = AL_TRUE,
 *		.tx_en = AL_TRUE,
 *		.rxh_mask = 0x0,
 *		.rxl_mask = 0x0,
 *		.data_low_pattern = { 0, 1, 0, 1, 0, 1, 1, 0 },
 *		.data_high_pattern = { 1, 1, 0, 1, 0, 1, 0, 0 }};
 *	struct al_cmos_io_bist_status bist_status;
 *	int err;
 *
 *	err = al_cmos_handle_init(&handle, (void __iomem*)0xfd860000);
 *	if (err)
 *		return err;
 *
 *	// Modify the group data mask
 *	al_cmos_cfg_get(&handle, 2, &cfg);
 *	cfg.data_mask |= 1;
 *	al_cmos_cfg_set(&handle, 2, &cfg);
 *
 *	// Start RGMII DLL
 *	al_cmos_rgmii_dll_enable_set(
 *		&handle, AL_CMOS_RGMII_A, AL_TRUE, AL_TRUE, AL_TURE,
 *		AL_CMOS_RGMII_DLL_CLK_INTERNAL);
 *	al_cmos_rgmii_dll_start(&handle, AL_CMOS_RGMII_A, AL_FALSE, 0);
 *	while (1) {
 *		al_cmos_rgmii_dll_status_get(
 *			&handle, AL_CMOS_RGMII_A, &dll_status);
 *
 *		if (dll_status.err == AL_TRUE)
 *			return -EIO;
 *
 *		if (dll_status.done == AL_TRUE)
 *			break;
 *	}
 *
 *	// Initiate RGMII A CMOS group I/O BIST (loopback)
 *	al_cmos_io_bist_start(&handle, AL_CMOS_RGMII_A, &bist_cfg);
 *	do {
 *		al_cmos_io_bist_status_get(
 *			&handle, AL_CMOS_RGMII_A, &bist_status);
 *	} while (!bist_status.rx_test_valid);
 * }
 * @endcode
 *      @file  al_hal_cmos.h
 *      @brief HAL Driver Header for the Annapurna Labs CMOS unit
 *
 */

#ifndef _AL_HAL_CMOS_H_
#define _AL_HAL_CMOS_H_

#include "al_hal_common.h"

/*********************** Data Structures and Types ***************************/

/**
 * al_cmos_handle: data structure used by the HAL to handle cmos functionality.
 * this structure is allocated and set to zeros by the upper layer, then it is
 * initialized by the al_cmos_init() that should be called before any other
 * function of this API. later, this handle passed to the API functions.
 */
struct al_cmos_handle {
	struct al_cmos_regs __iomem	*regs;
};

/**
 * CMOS RGMII ports for Alpine PoC family
 */
enum al_cmos_rgmii {
	AL_CMOS_RGMII_A = 0,
	AL_CMOS_RGMII_B,
};

/** Buffer drive strength */
enum al_cmos_buffer_drive_strength {
	/** 4mA */
	AL_CMOS_BUFF_DRV_STRENGTH_4 = 0,
	/** 6mA */
	AL_CMOS_BUFF_DRV_STRENGTH_6,
	/** 8mA */
	AL_CMOS_BUFF_DRV_STRENGTH_8,
	/** 12mA */
	AL_CMOS_BUFF_DRV_STRENGTH_12,
};

/**
 * Clock for DLL sweep
 */
enum al_cmos_rgmii_dll_clk {
	/* Internal PLL 125MHz channel */
	AL_CMOS_RGMII_DLL_CLK_INTERNAL = 0,
	/* TX clock */
	AL_CMOS_RGMII_DLL_CLK_TX,
};

/**
 * RGMII double data rate (DDR) status structure
 */
struct al_cmos_rgmii_dll_status {
	al_bool		err; /* DLL error */
	al_bool		done; /* DLL sweep done */
	al_bool		busy; /* DLL sweep in progress */
	unsigned int	sweep_result; /* DLL sweep result */
};

/**
 * I/O BIST configuration structure
 */
struct al_cmos_io_bist_cfg {
	/**
	 * RX test flow enabled
	 * RX and TX together activates a loopback test
	 */
	al_bool		rx_en;

	/**
	 * TX test flow enabled
	 * RX and TX together activates a loopback test
	 */
	al_bool		tx_en;

	/**
	 * RHX mask
	 * 5 bits: {CTRL, RXD[3:0]}
	 */
	unsigned int	rxh_mask;

	/**
	 * RHL mask
	 * 5 bits: {CTRL, RXD[3:0]}
	 */
	unsigned int	rxl_mask;

	/**
	 * Data low pattern array
	 * Each value should be in the range 0 - 31
	 */
	unsigned int	data_low_pattern[8];

	/**
	 * Data high pattern array
	 * Each value should be in the range 0 - 31
	 */
	unsigned int	data_high_pattern[8];
};

/**
 * I/O BIST status structure
 */
struct al_cmos_io_bist_status {
	al_bool		rx_test_valid;
	al_bool		rx_test_err_detected;
	al_bool		test_stage_preamble;
	unsigned int	test_vector_cnt;
	unsigned int	rxh_ref;
	unsigned int	rxl_ref;
	unsigned int	rxh_in;
	unsigned int	rxl_in;
};

/************************************* API ***********************************/

/**
 * @brief Initialize a cmos handle structure
 *
 * @param handle
 *		an allocated, non-initialized instance.
 * @param cmos_reg_base
 *		the virtual base address of the cmos internal
 *		registers
 *
 * @return 0 if no error found.
 */
int al_cmos_handle_init(
	struct al_cmos_handle	*handle,
	void __iomem		*cmos_reg_base);

/**
 * CMOS group set configuration
 *
 * @param	handle - context
 * @param	group - CMOS group selection (0 - 9)
 * @param	data_mask - Data Masks for bits [12:0]
 * @param	pullup - Buffer pull-up enable for bits [12:0]
 */
void al_cmos_cfg_set(
	struct al_cmos_handle	*handle,
	unsigned int		group,
	unsigned int		data_mask,
	unsigned int		pullup);

/**
 * CMOS group get configuration (to be used for get, modify, set)
 *
 * @param	handle - context
 * @param	group - CMOS group selection (0 - 9)
 * @param	data_mask - Data Masks for bits [12:0]
 * @param	pullup - Buffer pull-up enable for bits [12:0]
 */
void al_cmos_cfg_get(
	struct al_cmos_handle	*handle,
	unsigned int		group,
	unsigned int		*data_mask,
	unsigned int		*pullup);

#define AL_CMOS_HAS_RGMII_BUFF_DRV_STRENGTH_SET		1
/**
 * CMOS RGMII buffer drive strength setting
 *
 * @param	handle - context
 * @param	rgmii - RGMII selection
 * @param	val - required val
 */
void al_cmos_rgmii_buff_drv_strength_set(
	struct al_cmos_handle			*handle,
	enum al_cmos_rgmii			rgmii,
	enum al_cmos_buffer_drive_strength	val);

/**
 * CMOS RGMII DDR enable/disable
 *
 * @param	handle - context
 * @param	rgmii - RGMII selection
 * @param	enable - required state
 */
void al_cmos_rgmii_ddr_enable_set(
	struct al_cmos_handle		*handle,
	enum al_cmos_rgmii		rgmii,
	al_bool				enable);

/**
 * CMOS RGMII DLL enable/disable
 *
 * @param	handle - context
 * @param	rgmii - RGMII selection
 * @param	enable - DLL enable/disable
 * @param	tx_relock_en - Allow the sweep result to be applied
 * @param	rx_relock_en - Allow the sweep result to be applied
 * @param	clk_sel - Clock select for DLL sweep
 */
void al_cmos_rgmii_dll_enable_set(
	struct al_cmos_handle		*handle,
	enum al_cmos_rgmii		rgmii,
	al_bool				enable,
	al_bool				tx_relock_en,
	al_bool				rx_relock_en,
	enum al_cmos_rgmii_dll_clk	clk_sel);

/**
 * CMOS RGMII DLL sweep flow start
 *
 * @param handle - context
 * @param rgmii - RGMII selection
 * @param offset_en - Load offset as the start point for the sweep
 * @param offset - Sweep start point offset (if offset_en is AL_TRUE)
 *		   Notice: offset value can not be bigger than 4ns/delay_time
 *		   Notice: offset value must not exceed 0x25 in any case
 */
void al_cmos_rgmii_dll_start(
	struct al_cmos_handle		*handle,
	enum al_cmos_rgmii		rgmii,
	al_bool				offset_en,
	unsigned int			offset);

/**
 * CMOS RGMII DLL sweep flow clear failure
 *
 * @param	handle - context
 * @param	rgmii - RGMII selection
 */
void al_cmos_rgmii_dll_failure_clear(
	struct al_cmos_handle		*handle,
	enum al_cmos_rgmii		rgmii);

/**
 * CMOS RGMII DLL get status
 *
 * @param	handle - context
 * @param	rgmii - RGMII selection
 * @param	status - returned status
 */
void al_cmos_rgmii_dll_status_get(
	struct al_cmos_handle		*handle,
	enum al_cmos_rgmii		rgmii,
	struct al_cmos_rgmii_dll_status	*status);

/**
 * CMOS RGMII DLL delay set
 *
 * @param	handle - context
 * @param	rgmii - RGMII selection
 * @param	tx_dly - Required TX delay
 * @param	rx_dly - Required RX delay
 */
void al_cmos_rgmii_dll_delay_set(
	struct al_cmos_handle		*handle,
	enum al_cmos_rgmii		rgmii,
	unsigned int			tx_dly,
	unsigned int			rx_dly);

/**
 * CMOS RGMII start I/O BIST
 *
 * @param	handle - context
 * @param	rgmii - RGMII selection
 * @param	cfg - required configuration
 */
void al_cmos_io_bist_start(
	struct al_cmos_handle		*handle,
	enum al_cmos_rgmii		rgmii,
	struct al_cmos_io_bist_cfg	*cfg);

/**
 * CMOS RGMII get I/O BIST status
 *
 * @param	handle - context
 * @param	rgmii - RGMII selection
 * @param	status - returned status
 */
void al_cmos_io_bist_status_get(
	struct al_cmos_handle		*handle,
	enum al_cmos_rgmii		rgmii,
	struct al_cmos_io_bist_status	*status);

/**
 * Enable system fabric and CPU	PLL clock output to the DTO ports
 *
 * @param	handle - context
 * @param	en - 0 - disabled, 1 - enable
 */
void al_cmos_nb_cpu_clk_out_en(
	struct al_cmos_handle	*handle,
	al_bool			en);

#endif
/** @} end of groupcmos group */

