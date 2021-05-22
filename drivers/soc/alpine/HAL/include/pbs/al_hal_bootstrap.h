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
 * @defgroup group_bootstrap Bootstrap
 * @ingroup group_pbs
 *  Bootstrap
 *  @{
 * @file   al_hal_bootstrap.h
 *
 * @brief  includes bootstrap API
 *
 */

#ifndef __AL_HAL_BOOTSTRAP_H__
#define __AL_HAL_BOOTSTRAP_H__

#include "al_hal_common.h"
#include "al_hal_pbs_regs.h"

/* enum definitions */

/* I2C preload enabled/disabled */
enum i2c_preload {
	I2C_PRELOAD_ENABLED,
	I2C_PRELOAD_DISABLED
};

/* I2C preload speed */
enum i2c_preload_speed {
	I2C_PRELOAD_SPEED_FAST_400K,
	I2C_PRELOAD_SPEED_HIGH_750K,
	I2C_PRELOAD_SPEED_HIGH_1M,
	I2C_PRELOAD_SPEED_STANDARD_100K,
};

/* SPI slave preload enabled/disabled */
enum spis_preload {
	SPIS_PRELOAD_ENABLED,
	SPIS_PRELOAD_DISABLED
};

/* SPI master preload enabled/disabled */
enum spim_preload {
	SPIM_PRELOAD_ENABLED,
	SPIM_PRELOAD_DISABLED
};

/* SPI master preload mode */
enum spim_preload_mode {
	SPIM_PRELOAD_MODE_SPI_12_5,
	SPIM_PRELOAD_MODE_SPI_25_0,
	SPIM_PRELOAD_MODE_QSPI_25_0,
	SPIM_PRELOAD_MODE_QSPI_50_0,
};

/* SPI master SW mode */
enum spim_sw_mode {
	SPIM_SW_MODE_NOT_APPLICABLE,
	SPIM_SW_MODE_SPI_25_0,
	SPIM_SW_MODE_SPI_50_0,
	SPIM_SW_MODE_SPI_83_0,
	SPIM_SW_MODE_QPI_25_0,
	SPIM_SW_MODE_QPI_50_0,
	SPIM_SW_MODE_QPI_83_0,
};

/* Boot through boot ROM */
enum boot_rom {
	BOOT_ROM_DISABLED,
	BOOT_ROM_ENABLED
};

/* Boot device */
enum boot_device {
	BOOT_DEVICE_NAND_8BIT,
	BOOT_DEVICE_UART,
	BOOT_DEVICE_SPI_MODE_3,
	BOOT_DEVICE_SPI_MODE_0,
	BOOT_DEVICE_UART_CLI,
};

/* Boot ROM debug mode enabled/disabled */
enum debug_mode {
	DEBUG_MODE_ENABLED,
	DEBUG_MODE_DISABLED
};

/* CPU existance indication */
enum cpu_exist {
	CPU_EXIST_0,
	CPU_EXIST_0_1,
	CPU_EXIST_0_1_2_3
};

/* struct definitions */

/* struct al_bootstrap
 * All freq values are measured in Hz units
 */
struct al_bootstrap {
	/* CPU PLL frequency - CPU clock has the same frequency */
	uint32_t		cpu_pll_freq;

	/* NB PLL frequency - DDR clock has the same frequency */
	uint32_t		ddr_pll_freq;

	/* SB PLL frequency */
	uint32_t		sb_pll_freq;

	/* SB main clock frequency */
	uint32_t		sb_clk_freq;

	/* Reference clock frequency */
	uint32_t		pll_ref_clk_freq;

	enum i2c_preload	i2c_preload;
	enum spis_preload	spis_preload;
	enum spim_preload	spim_preload;
	enum boot_rom		boot_rom;
	enum boot_device	boot_device;
	unsigned int		boot_from_uart_baud_rate;
	enum debug_mode		debug_mode;
	enum cpu_exist		cpu_exist;
	uint8_t			i2c_preload_addr;
	enum i2c_preload_speed	i2c_preload_speed;
	enum spim_preload_mode	spim_preload_mode;
	unsigned int 		clusters_num;
	al_bool			dcache;
	al_bool			llc;
	enum spim_sw_mode	spim_sw_mode;
};

/* API definitions */

/**
 * Set the boot parameters according to the bootstrap register
 *
 * This function reads the Bootstrap register, parses its value, and
 * updates the bootstrap struct accordingly. The parsing is achieved by
 * using a per-parameter static functions.
 *
 * @param	pbs_regfile_ptr
 *		The address of the PBS Regfile
 * @param bootstrap
 *              The boot options struct we are initializing, should be allocated
 *		by the function's caller
 */
int al_bootstrap_parse(
		void __iomem *pbs_regfile_ptr,
		struct al_bootstrap *bootstrap);

/**
 * Prints the bootstrap configuration
 *
 * @param bootstrap
 *              An initialized boot options struct
 */
void al_bootstrap_print(
		struct al_bootstrap *bootstrap);

#endif

/** @} end of Bootstrap group */

