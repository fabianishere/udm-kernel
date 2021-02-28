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
*  Bootstrap
*  @{
* @file   al_hal_bootstrap.c
*
* @brief  includes bootstrap HAL implementation
*
*/

#include "al_hal_bootstrap.h"
#include "al_hal_bootstrap_map.h"
#include "al_hal_pbs_utils.h"

/**
 * The following al_bootstrap_get_* functions are all implemented the same way:
 * Update the appropriate field on the bootstrap struct, according to the
 * register value.
 *
 * Note that in case of an error, no value will be written to the bootstrap
 * struct - so any uninitialized value should be ignored.
 *
 * @param [in]	bootstrap_reg_val
 *		The value of the bootstrap register
 * @param [in]	bypass_freq
 *		Frequency to be returned in case bypass mode is requested
 * @param [out]	bootstrap
 *		The bootstrap struct we are setting
 * @param [in]	dev_id
 *		Device ID
 */
static inline void al_bootstrap_cpu_pll_freq_get(
		uint32_t bootstrap_reg_val,
		uint32_t bypass_freq,
		struct al_bootstrap *bootstrap,
		unsigned int dev_id)
{
	uint32_t field, parsed_field = 2000000000U;

	field = AL_REG_FIELD_GET(bootstrap_reg_val,
			AL_HAL_BOOTSTRAP_CPU_PLL_FREQ_MASK,
			AL_HAL_BOOTSTRAP_CPU_PLL_FREQ_SHIFT);

	if (dev_id <= PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V2) {
		switch (field) {
		case 0x0:
			parsed_field = bypass_freq;
			break;
		case 0x1:
			parsed_field = 1000000000U;
			break;
		case 0x2:
			parsed_field = 1400000000U;
			break;
		case 0x3:
			parsed_field = 1500000000U;
			break;
		case 0x4:
			parsed_field = 1600000000U;
			break;
		case 0x5:
			parsed_field = 1700000000U;
			break;
		case 0x6:
			parsed_field = 1800000000U;
			break;
		case 0x7:
			parsed_field = 1900000000U;
			break;
		case 0x8:
			parsed_field = 2100000000U;
			break;
		case 0x9:
			parsed_field = 2200000000U;
			break;
		case 0xA:
			parsed_field = 2300000000U;
			break;
		case 0xB:
			parsed_field = 2400000000U;
			break;
		case 0xC:
			parsed_field = 2500000000U;
			break;
		case 0xD:
			parsed_field = 2600000000U;
			break;
		case 0xE:
			parsed_field = 2700000000U;
			break;
		case 0xF:
			parsed_field = 2000000000U;
			break;
		default:
			al_assert(0);
			break;
		}
	} else {
		switch (field) {
		case 0x0:
			parsed_field = bypass_freq;
			break;
		case 0x1:
			parsed_field = 1200000000U;
			break;
		case 0x2:
			parsed_field = 1500000000U;
			break;
		case 0x3:
			parsed_field = 1700000000U;
			break;
		case 0x4:
			parsed_field = 1800000000U;
			break;
		case 0x5:
			parsed_field = 1900000000U;
			break;
		case 0x6:
			parsed_field = 2100000000U;
			break;
		case 0x7:
			parsed_field = 2200000000U;
			break;
		case 0x8:
			parsed_field = 2300000000U;
			break;
		case 0x9:
			parsed_field = 2400000000U;
			break;
		case 0xA:
			parsed_field = 2500000000U;
			break;
		case 0xB:
			parsed_field = 2600000000U;
			break;
		case 0xC:
			parsed_field = 2700000000U;
			break;
		case 0xD:
			parsed_field = 2800000000U;
			break;
		case 0xE:
			parsed_field = 3000000000U;
			break;
		case 0xF:
			parsed_field = 2000000000U;
			break;
		default:
			al_assert(0);
			break;
		}
	}

	bootstrap->cpu_pll_freq = parsed_field;
}

static inline void al_bootstrap_ddr_pll_freq_get(
		uint32_t bootstrap_reg_val,
		uint32_t bypass_freq,
		struct al_bootstrap *bootstrap,
		unsigned int dev_id)
{
	uint32_t field, parsed_field = 800000000;

	field = AL_REG_FIELD_GET(bootstrap_reg_val,
			AL_HAL_BOOTSTRAP_NB_PLL_FREQ_MASK,
			AL_HAL_BOOTSTRAP_NB_PLL_FREQ_SHIFT);
	switch (field) {
	case 0x0:
		parsed_field = bypass_freq;
		break;
	case 0x1:
		if (dev_id >= PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V2)
			parsed_field = 1066666666;
		else
			parsed_field = 533333333;
		break;
	case 0x2:
		parsed_field = 666666666;
		break;
	case 0x3:
		if (dev_id >= PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V2)
			parsed_field = 1300000000;
		else
			parsed_field = 900000000;
		break;
	case 0x4:
		parsed_field = 933333333;
		break;
	case 0x5:
		if (dev_id >= PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V3)
			parsed_field = 1333333333;
		else if (dev_id >= PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V2)
			parsed_field = 1050000000;
		else
			parsed_field = 1066666666;
		break;
	case 0x6: /* >= Alpine V2 */
		parsed_field = 1200000000;
		break;
	case 0x7:
		parsed_field = 800000000;
		break;
	default:
		al_assert(0);
		break;
	}

	bootstrap->ddr_pll_freq = parsed_field;
}

/* This set function sets both the sb_pll_freq and sb_clk_freq, because the pll
 * affects the clk when its set to bypass mode */
static inline void al_bootstrap_sb_pll_and_clk_freq_get(
		uint32_t bootstrap_reg_val,
		uint32_t bypass_freq,
		struct al_bootstrap *bootstrap,
		unsigned int dev_id)
{
	uint32_t pll_field, pll_parsed_field = 1500000000;
	uint32_t clk_field, clk_parsed_field = 375000000;
	int bypass_flag = 0;

	pll_field = AL_REG_FIELD_GET(bootstrap_reg_val,
		AL_HAL_BOOTSTRAP_SB_PLL_FREQ_MASK,
		AL_HAL_BOOTSTRAP_SB_PLL_FREQ_SHIFT);

	if (pll_field == 0) {
		pll_parsed_field = bypass_freq;
		bypass_flag = 1;
	} else if ((dev_id > PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V1) && (pll_field == 1)) {
		pll_parsed_field = 3000000000U;
	} else {
		pll_parsed_field = 1500000000;
	}

	bootstrap->sb_pll_freq = pll_parsed_field;

	if (bypass_flag) {
		/* if pll is on bypass mode, clk_freq = bypass_freq */
		clk_parsed_field = bypass_freq;
	} else {
		clk_field = AL_REG_FIELD_GET(bootstrap_reg_val,
			AL_HAL_BOOTSTRAP_SB_CLK_FREQ_MASK,
			AL_HAL_BOOTSTRAP_SB_CLK_FREQ_SHIFT);
		if (dev_id > PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V1) {
			switch (clk_field) {
			case 0x0:
				clk_parsed_field = 250000000;
				break;
			case 0x1:
				clk_parsed_field = 375000000;
				break;
			case 0x2:
				clk_parsed_field = 428000000;
				break;
			case 0x3:
				clk_parsed_field = 500000000;
				break;
			default:
				al_assert(0);
				break;
			}
		} else {
			switch (clk_field) {
			case 0x0:
				clk_parsed_field = 250000000;
				break;
			case 0x1:
				clk_parsed_field = 300000000;
				break;
			case 0x2:
				clk_parsed_field = 500000000;
				break;
			case 0x3:
				clk_parsed_field = 375000000;
				break;
			default:
				al_assert(0);
				break;
			}
		}
	}

	bootstrap->sb_clk_freq = clk_parsed_field;
}

static inline void al_bootstrap_i2c_preload_get(
		uint32_t bootstrap_reg_val,
		struct al_bootstrap *bootstrap)
{
	uint32_t field;
	enum i2c_preload parsed_field = I2C_PRELOAD_DISABLED;

	field = AL_REG_FIELD_GET(bootstrap_reg_val,
			AL_HAL_BOOTSTRAP_I2C_PRELOAD_MASK,
			AL_HAL_BOOTSTRAP_I2C_PRELOAD_SHIFT);
	switch (field) {
	case 0x0:
		parsed_field = I2C_PRELOAD_ENABLED;
		break;
	case 0x1:
		parsed_field = I2C_PRELOAD_DISABLED;
		break;
	default:
		al_assert(0);
		break;
	}

	bootstrap->i2c_preload = parsed_field;
}

static inline void al_bootstrap_spis_preload_get(
		uint32_t bootstrap_reg_val,
		struct al_bootstrap *bootstrap)
{
	uint32_t field;
	enum spis_preload parsed_field = SPIS_PRELOAD_DISABLED;

	field = AL_REG_FIELD_GET(bootstrap_reg_val,
			AL_HAL_BOOTSTRAP_SPIS_PRELOAD_MASK,
			AL_HAL_BOOTSTRAP_SPIS_PRELOAD_SHIFT);
	switch (field) {
	case 0x0:
		parsed_field = SPIS_PRELOAD_ENABLED;
		break;
	case 0x1:
		parsed_field = SPIS_PRELOAD_DISABLED;
		break;
	default:
		al_assert(0);
		break;
	}

	bootstrap->spis_preload = parsed_field;
}

static inline void al_bootstrap_preload_get(
		uint32_t bootstrap_reg_val,
		struct al_bootstrap *bootstrap,
		unsigned int dev_id)
{
	uint32_t field;
	enum i2c_preload i2c_parsed_field = I2C_PRELOAD_DISABLED;
	enum spis_preload spis_parsed_field = SPIS_PRELOAD_DISABLED;
	enum spim_preload spim_parsed_field = SPIM_PRELOAD_DISABLED;
	enum i2c_preload_speed i2c_preload_speed_parsed_field = I2C_PRELOAD_SPEED_STANDARD_100K;
	enum spim_preload_mode spim_preload_mode_parsed_field = SPIM_PRELOAD_MODE_SPI_12_5;

	if (dev_id > PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V1) {
		field = AL_REG_FIELD_GET(bootstrap_reg_val,
				AL_HAL_BOOTSTRAP_PRELOAD_MASK,
				AL_HAL_BOOTSTRAP_PRELOAD_SHIFT);
		switch (field) {
		case 0x0:
			if (dev_id >= PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V3)
				spim_parsed_field = SPIM_PRELOAD_ENABLED;
			break;
		case 0x1:
		case 0x3:
			i2c_parsed_field = I2C_PRELOAD_ENABLED;
			break;
		case 0x2:
			spis_parsed_field = SPIS_PRELOAD_ENABLED;
			break;
		default:
			al_assert(0);
			break;
		}

		field = AL_REG_FIELD_GET(bootstrap_reg_val,
			AL_HAL_BOOTSTRAP_PRELOAD_ATT_MASK,
			AL_HAL_BOOTSTRAP_PRELOAD_ATT_SHIFT);

		if ((spis_parsed_field == SPIS_PRELOAD_ENABLED) &&
				(dev_id >= PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V3)) {
			if (!field)
				spis_parsed_field = SPIS_PRELOAD_DISABLED;
		} else if (i2c_parsed_field == I2C_PRELOAD_ENABLED) {
			switch (field) {
			case 0:
				i2c_preload_speed_parsed_field = I2C_PRELOAD_SPEED_FAST_400K;
				break;
			case 1:
				i2c_preload_speed_parsed_field = I2C_PRELOAD_SPEED_HIGH_750K;
				break;
			case 2:
				i2c_preload_speed_parsed_field = I2C_PRELOAD_SPEED_HIGH_1M;
				break;
			case 3:
				i2c_preload_speed_parsed_field = I2C_PRELOAD_SPEED_STANDARD_100K;
				break;
			default:
				al_assert(0);
				break;
			}
		} else if ((spim_parsed_field == SPIM_PRELOAD_ENABLED) &&
				(dev_id >= PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V3)) {
			switch (field) {
			case 0:
				spim_preload_mode_parsed_field = SPIM_PRELOAD_MODE_SPI_12_5;
				break;
			case 1:
				spim_preload_mode_parsed_field = SPIM_PRELOAD_MODE_SPI_25_0;
				break;
			case 2:
				spim_preload_mode_parsed_field = SPIM_PRELOAD_MODE_QSPI_25_0;
				break;
			case 3:
				spim_preload_mode_parsed_field = SPIM_PRELOAD_MODE_QSPI_50_0;
				break;
			default:
				al_assert(0);
				break;
			}
		}
	} else {
		i2c_parsed_field = AL_REG_FIELD_GET(bootstrap_reg_val,
			AL_HAL_BOOTSTRAP_I2C_PRELOAD_MASK,
			AL_HAL_BOOTSTRAP_I2C_PRELOAD_SHIFT) ?
			I2C_PRELOAD_DISABLED : I2C_PRELOAD_ENABLED;
		spis_parsed_field = AL_REG_FIELD_GET(bootstrap_reg_val,
			AL_HAL_BOOTSTRAP_SPIS_PRELOAD_MASK,
			AL_HAL_BOOTSTRAP_SPIS_PRELOAD_SHIFT) ?
			SPIS_PRELOAD_DISABLED : SPIS_PRELOAD_ENABLED;
	}

	bootstrap->i2c_preload = i2c_parsed_field;
	bootstrap->spis_preload = spis_parsed_field;
	bootstrap->spim_preload = spim_parsed_field;
	bootstrap->i2c_preload_speed = i2c_preload_speed_parsed_field;
	bootstrap->spim_preload_mode = spim_preload_mode_parsed_field;
}

static inline void al_bootstrap_boot_rom_get(
		uint32_t bootstrap_reg_val,
		struct al_bootstrap *bootstrap)
{
	uint32_t field;
	enum boot_rom parsed_field = BOOT_ROM_ENABLED;

	field = AL_REG_FIELD_GET(bootstrap_reg_val,
			AL_HAL_BOOTSTRAP_BOOT_ROM_MASK,
			AL_HAL_BOOTSTRAP_BOOT_ROM_SHIFT);
	switch (field) {
	case 0x0:
		parsed_field = BOOT_ROM_DISABLED;
		break;
	case 0x1:
		parsed_field = BOOT_ROM_ENABLED;
		break;
	default:
		al_assert(0);
		break;
	}

	bootstrap->boot_rom = parsed_field;
}

static inline void al_bootstrap_boot_device_get(
		uint32_t bootstrap_reg_val,
		struct al_bootstrap *bootstrap)
{
	uint32_t field;
	enum boot_device parsed_field = BOOT_DEVICE_SPI_MODE_0;
	unsigned int boot_from_uart_baud_rate = 115200;

	field = AL_REG_FIELD_GET(bootstrap_reg_val,
			AL_HAL_BOOTSTRAP_BOOT_DEVICE_MASK,
			AL_HAL_BOOTSTRAP_BOOT_DEVICE_SHIFT);
	switch (field) {
	case 0x0:
		parsed_field = BOOT_DEVICE_UART_CLI;
		break;
	case 0x1:
		parsed_field = BOOT_DEVICE_UART;
		boot_from_uart_baud_rate = 2000000;
		break;
	case 0x2:
		parsed_field = BOOT_DEVICE_NAND_8BIT;
		break;
	case 0x4:
		parsed_field = BOOT_DEVICE_UART;
		break;
	case 0x5:
		parsed_field = BOOT_DEVICE_SPI_MODE_3;
		break;
	case 0x6:
		parsed_field = BOOT_DEVICE_UART;
		boot_from_uart_baud_rate = 1000000;
		break;
	case 0x7:
		parsed_field = BOOT_DEVICE_SPI_MODE_0;
		break;
	default:
		al_assert(0);
		break;
	}

	bootstrap->boot_device = parsed_field;
	bootstrap->boot_from_uart_baud_rate = boot_from_uart_baud_rate;
}

static inline void al_bootstrap_debug_mode_get(
		uint32_t bootstrap_reg_val,
		struct al_bootstrap *bootstrap)
{
	uint32_t field;
	enum debug_mode parsed_field = DEBUG_MODE_DISABLED;

	field = AL_REG_FIELD_GET(bootstrap_reg_val,
			AL_HAL_BOOTSTRAP_DEBUG_MODE_MASK,
			AL_HAL_BOOTSTRAP_DEBUG_MODE_SHIFT);
	switch (field) {
	case 0x0:
		parsed_field = DEBUG_MODE_ENABLED;
		break;
	case 0x1:
		parsed_field = DEBUG_MODE_DISABLED;
		break;
	default:
		al_assert(0);
		break;
	}

	bootstrap->debug_mode = parsed_field;
}

static inline void al_bootstrap_pll_ref_clk_freq_get(
		uint32_t bootstrap_reg_val,
		struct al_bootstrap *bootstrap)
{
	uint32_t field, parsed_field = 25000000;

	field = AL_REG_FIELD_GET(bootstrap_reg_val,
			AL_HAL_BOOTSTRAP_PLL_REF_CLK_FREQ_MASK,
			AL_HAL_BOOTSTRAP_PLL_REF_CLK_FREQ_SHIFT);
	switch (field) {
	case 0x0:
		parsed_field = 25000000;
		break;
	case 0x1:
		parsed_field = 100000000;
		break;
	default:
		al_assert(0);
		break;
	}

	bootstrap->pll_ref_clk_freq = parsed_field;
}

static inline void al_bootstrap_cpu_cfg_get(
		uint32_t bootstrap_reg_val,
		struct al_bootstrap *bootstrap,
		unsigned int dev_id)
{
	uint32_t field;
	enum cpu_exist cpu_exist = CPU_EXIST_0;
	unsigned int num_clusters = 1;

	field = AL_REG_FIELD_GET(bootstrap_reg_val,
			AL_HAL_BOOTSTRAP_CPU_EXIST_MASK,
			AL_HAL_BOOTSTRAP_CPU_EXIST_SHIFT);
	if (dev_id >= PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V3) {
		cpu_exist = CPU_EXIST_0_1_2_3;
		num_clusters = field + 1;
	} else {
		switch (field) {
		case 0x0:
			cpu_exist = CPU_EXIST_0;
			break;
		case 0x1:
			cpu_exist = CPU_EXIST_0_1;
			break;
		case 0x3:
			cpu_exist = CPU_EXIST_0_1_2_3;
			break;
		default:
			al_assert(0);
			break;
		}
	}

	bootstrap->cpu_exist = cpu_exist;
	bootstrap->clusters_num = num_clusters;
}

static inline void al_bootstrap_i2c_preload_addr_get(
		uint32_t bootstrap_reg_val,
		struct al_bootstrap *bootstrap,
		unsigned int dev_id)
{
	uint8_t parsed_field = 0x57;

	if (dev_id <= PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V2) {
		uint32_t field;
		field = AL_REG_FIELD_GET(bootstrap_reg_val,
				AL_HAL_BOOTSTRAP_I2C_PRELOAD_ADDR_MASK,
				AL_HAL_BOOTSTRAP_I2C_PRELOAD_ADDR_SHIFT);
		switch (field) {
		case 0x0:
			parsed_field = (dev_id == PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V1) ?
					0x57 : 0x50;
			break;
		case 0x1:
			parsed_field = (dev_id == PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V1) ?
					0x50 : 0x57;
			break;
		default:
			al_assert(0);
			break;
		}
	}

	bootstrap->i2c_preload_addr = parsed_field;
}

static inline void al_bootstrap_dcache_llc_get(
		uint32_t bootstrap_reg_val,
		struct al_bootstrap *bootstrap,
		unsigned int dev_id)
{
	al_bool dcache = AL_FALSE;
	al_bool llc = AL_FALSE;

	if (dev_id >= PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V3) {
		dcache = !(bootstrap_reg_val & AL_HAL_BOOTSTRAP_DCACHE_DIS);
		llc = !(bootstrap_reg_val & AL_HAL_BOOTSTRAP_LLC_DIS);
	}

	bootstrap->dcache = dcache;
	bootstrap->llc = llc;
}

static inline void al_bootstrap_spim_sw_mode_get(
		uint32_t bootstrap_reg_val,
		uint32_t bootstrap_2_reg_val,
		struct al_bootstrap *bootstrap,
		unsigned int dev_id)
{
	enum spim_sw_mode spim_sw_mode = SPIM_SW_MODE_NOT_APPLICABLE;

	if (dev_id >= PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V3) {
		unsigned int val =
			(AL_REG_FIELD_GET(bootstrap_2_reg_val,
				AL_HAL_BOOTSTRAP_2_SPIM_SW_MODE_MSB_MASK,
				AL_HAL_BOOTSTRAP_2_SPIM_SW_MODE_MSB_SHIFT) << 2) |
			AL_REG_FIELD_GET(bootstrap_reg_val,
				AL_HAL_BOOTSTRAP_SPIM_SW_MODE_MASK,
				AL_HAL_BOOTSTRAP_SPIM_SW_MODE_SHIFT);

		switch (val) {
		case 0:
			spim_sw_mode = SPIM_SW_MODE_QPI_25_0;
			break;
		case 1:
			spim_sw_mode = SPIM_SW_MODE_QPI_50_0;
			break;
		case 2:
			spim_sw_mode = SPIM_SW_MODE_QPI_83_0;
			break;
		case 4:
			spim_sw_mode = SPIM_SW_MODE_SPI_25_0;
			break;
		case 5:
			spim_sw_mode = SPIM_SW_MODE_SPI_50_0;
			break;
		case 6:
			spim_sw_mode = SPIM_SW_MODE_SPI_83_0;
			break;
		case 3:
		case 7:
			spim_sw_mode = SPIM_SW_MODE_NOT_APPLICABLE;
			break;
		default:
			al_assert(0);
			break;
		}
	}

	bootstrap->spim_sw_mode = spim_sw_mode;
}

int al_bootstrap_parse(
		void __iomem *pbs_regfile_ptr,
		struct al_bootstrap *bootstrap)
{
	struct al_pbs_regs *pbs_regfile;
	uint32_t bootstrap_reg_val;
	uint32_t bootstrap_2_reg_val;
	unsigned int dev_id;
	unsigned int rev_id;

	al_assert(pbs_regfile_ptr);
	al_assert(bootstrap);

	pbs_regfile = (struct al_pbs_regs *)pbs_regfile_ptr;

#ifndef AL_DEV_ID
	/* check board device id */
	dev_id = al_pbs_dev_id_get(pbs_regfile);
#else
#if (AL_DEV_ID == AL_DEV_ID_ALPINE_V1)
	dev_id = PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V1;
#elif (AL_DEV_ID == AL_DEV_ID_ALPINE_V2)
	dev_id = PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V2;
#elif (AL_DEV_ID == AL_DEV_ID_ALPINE_V3)
	dev_id = PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V3;
#elif (AL_DEV_ID == AL_DEV_ID_ALPINE_V4)
	dev_id = PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V4;
#else
#error "Not supported!\n";
#endif
#endif

#ifndef AL_DEV_REV_ID
	rev_id = al_pbs_dev_rev_id_get(pbs_regfile);
#else
	rev_id = AL_DEV_REV_ID;
#endif

	/* Read the value of the bootstrap register */
	bootstrap_reg_val = al_reg_read32(&pbs_regfile->unit.boot_strap);
	if ((dev_id > PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V3) ||
			((dev_id == PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V3) &&
			(rev_id != PBS_UNIT_CHIP_ID_DEV_REV_ID_ALPINE_V3_TC)))
		bootstrap_2_reg_val = al_reg_read32(&pbs_regfile->unit.boot_strap_2);
	else
		bootstrap_2_reg_val = 0xffffffff;

	/* Parse the bootstrap register into the al_bootstrap struct
	 * The following code parse each field separately. In case of multiple
	 * errors, the function will return the first error it encounters.
	 */
	al_bootstrap_pll_ref_clk_freq_get(bootstrap_reg_val, bootstrap);
	al_bootstrap_cpu_pll_freq_get(
		bootstrap_reg_val, bootstrap->pll_ref_clk_freq, bootstrap, dev_id);
	al_bootstrap_ddr_pll_freq_get(
		bootstrap_reg_val, bootstrap->pll_ref_clk_freq, bootstrap, dev_id);
	al_bootstrap_sb_pll_and_clk_freq_get(
		bootstrap_reg_val, bootstrap->pll_ref_clk_freq, bootstrap, dev_id);
	al_bootstrap_preload_get(bootstrap_reg_val, bootstrap, dev_id);
	al_bootstrap_boot_rom_get(bootstrap_reg_val, bootstrap);
	al_bootstrap_boot_device_get(bootstrap_reg_val, bootstrap);
	al_bootstrap_debug_mode_get(bootstrap_reg_val, bootstrap);
	al_bootstrap_cpu_cfg_get(bootstrap_reg_val, bootstrap, dev_id);
	al_bootstrap_i2c_preload_addr_get(bootstrap_reg_val, bootstrap, dev_id);
	al_bootstrap_dcache_llc_get(bootstrap_reg_val, bootstrap, dev_id);
	al_bootstrap_spim_sw_mode_get(bootstrap_reg_val, bootstrap_2_reg_val, bootstrap, dev_id);

	return 0;
}

static void al_bootstrap_print_freq(const char *name, uint32_t freq)
{
	al_print("- %s %uMHz\n", name, freq / 1000000);
}

void al_bootstrap_print(
	struct al_bootstrap *bootstrap)
{
	al_assert(bootstrap);

	al_print("Boot configuration:\n");
	al_bootstrap_print_freq("cpu_pll_freq", bootstrap->cpu_pll_freq);
	al_bootstrap_print_freq("ddr_pll_freq", bootstrap->ddr_pll_freq);
	al_bootstrap_print_freq("sb_pll_freq", bootstrap->sb_pll_freq);
	al_bootstrap_print_freq("sb_clk_freq", bootstrap->sb_clk_freq);
	al_bootstrap_print_freq("pll_ref_clk_freq", bootstrap->pll_ref_clk_freq);
	if (bootstrap->i2c_preload == I2C_PRELOAD_ENABLED) {
		al_print("- i2c_preload %uK from %02x\n",
			(bootstrap->i2c_preload_speed == I2C_PRELOAD_SPEED_FAST_400K) ? 400 :
			(bootstrap->i2c_preload_speed == I2C_PRELOAD_SPEED_HIGH_750K) ? 750 :
			(bootstrap->i2c_preload_speed == I2C_PRELOAD_SPEED_HIGH_1M) ? 1000 : 100,
			bootstrap->i2c_preload_addr);
	} else {
		al_print("- i2c_preload disabled\n");
	}
	al_print("- spis_preload %s\n",
		(bootstrap->spis_preload == SPIS_PRELOAD_ENABLED) ? "enabled" : "disabled");
	if (bootstrap->spim_preload == SPIM_PRELOAD_ENABLED) {
		al_print("- spim_preload %s\n",
			(bootstrap->spim_preload_mode == SPIM_PRELOAD_MODE_SPI_12_5) ? "1x12.5MHz" :
			(bootstrap->spim_preload_mode == SPIM_PRELOAD_MODE_SPI_25_0) ? "1x25MHz" :
			(bootstrap->spim_preload_mode == SPIM_PRELOAD_MODE_QSPI_25_0) ? "4x25MHz" :
			"4x50MHz");
	} else {
		al_print("- spim_preload disabled\n");
	}

	al_print("- spim_sw_mode ");
	switch (bootstrap->spim_sw_mode) {
	case SPIM_SW_MODE_SPI_25_0:
		al_print("1x25MHz\n");
		break;
	case SPIM_SW_MODE_SPI_50_0:
		al_print("1x50MHz\n");
		break;
	case SPIM_SW_MODE_SPI_83_0:
		al_print("1x83MHz\n");
		break;
	case SPIM_SW_MODE_QPI_25_0:
		al_print("4x25MHz\n");
		break;
	case SPIM_SW_MODE_QPI_50_0:
		al_print("4x50MHz\n");
		break;
	case SPIM_SW_MODE_QPI_83_0:
		al_print("4x83MHz\n");
		break;
	case SPIM_SW_MODE_NOT_APPLICABLE:
	default:
		al_print("N/A\n");
		break;
	}

	al_print("- boot_device ");
	switch (bootstrap->boot_device) {
	case BOOT_DEVICE_NAND_8BIT:
		al_print("NAND\n");
		break;
	case BOOT_DEVICE_UART:
		al_print("UART (%ubps)\n", bootstrap->boot_from_uart_baud_rate);
		break;
	case BOOT_DEVICE_SPI_MODE_3:
		al_print("SPI(M3)\n");
		break;
	case BOOT_DEVICE_SPI_MODE_0:
		al_print("SPI(M0)\n");
		break;
	case BOOT_DEVICE_UART_CLI:
		al_print("UART CLI\n");
		break;
	default:
		al_print("N/A");
	}

	al_print("- cores %ux%u\n", bootstrap->clusters_num,
		(bootstrap->cpu_exist == CPU_EXIST_0) ? 1 :
		(bootstrap->cpu_exist == CPU_EXIST_0_1) ? 2 : 4);
	al_print("- debug_mode %u\n", bootstrap->debug_mode == DEBUG_MODE_ENABLED);
	al_print("- dcache %s\n", bootstrap->dcache ? "enabled" : "disabled");
	al_print("- llc %s\n", bootstrap->llc ? "enabled" : "disabled");
}

/** @} end of Bootstrap group */


