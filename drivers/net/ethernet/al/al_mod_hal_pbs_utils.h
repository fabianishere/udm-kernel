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
 * @defgroup group_pbs_utils PBS Utils
 * @ingroup group_pbs_api
 *  PBS UTILS API
 *  @{
 * @file   al_mod_hal_pbs_utils.h
 *
 * @brief C Header file for PBS miscelaneous utils
 */

#ifndef __AL_HAL_PBS_UTILS_H__
#define __AL_HAL_PBS_UTILS_H__

#include <al_mod_hal_pbs_regs.h>
#include <al_mod_hal_types.h>

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

/**
 * Get Device ID
 *
 * @param	pbs_regs
 *		PBS register file base
 *
 * @returns	Device ID:
 *		PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V1 - Alpine V1
 *		PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V2 - Alpine V2
 *		PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V3 - Alpine V3
 *		...
 */
unsigned int al_mod_pbs_dev_id_get(
	void __iomem *pbs_regs);

/**
 * Get Device revision ID
 *
 * @param	pbs_regs
 *		PBS register file base
 *
 * @returns	Device revision ID:
 *		Alpine V1/V2 - always 0
 *		Alpine V3:
 *		- PBS_UNIT_CHIP_ID_DEV_REV_ID_ALPINE_V3_TC
 *		- PBS_UNIT_CHIP_ID_DEV_REV_ID_ALPINE_V3_MAIN
 */
unsigned int al_mod_pbs_dev_rev_id_get(
	void __iomem *pbs_regs);

/**
 * AXI timeout set
 *
 * Each configuration access is protected by a timeout, which expires when the unit does not
 * respond after the counter reaches the register value limit.
 * On Alpine V2 onwards there's a separate timeout for SPI slave.
 *
 * @param	pbs_regs
 *		PBS register file base
 * @param	num_cyc
 *		Number of I/O fabric clock cycles until expiration
 *		Note: On Alpine V2 onwards, bits 31..24 must be zero
 * @param	num_cyc_spi
 *		Number of I/O fabric clock cycles until expiration of SPI slave access
 *		Note: Bits 15..0 of 'num_cyc' and 'num_cyc_spi' must be equal
 *		Note: Bits 24..16 must be zero
 *		Note: Ignored on Alpine V1 and 'num_cyc' used instead
 */
void al_mod_pbs_axi_timeout_set(
	void __iomem	*pbs_regs,
	unsigned int	num_cyc,
	unsigned int	num_cyc_spi);

/**
 * Enable/disable access to 'winit' registers
 *
 * @param	pbs_regs
 *		PBS register file base
 * @param	en
 *		Non zero for enabling, zero for disabling
 */
void al_mod_pbs_winit_access_en(
	void __iomem	*pbs_regs,
	al_mod_bool		en);

struct al_mod_pbs_apb_mem_config {
	/** Enables parity protection on the integrated SRAM. */
	al_mod_bool bootrom_parity_en;
	/**
	 * When set, reports a slave error whenthe slave returns an AXI slave error, for
	 * configuration access to the internal configuration space.
	 */
	al_mod_bool rd_slv_err_en;
	/**
	 * When set, reports a decode error when timeout has occurred for configuration access to
	 * the internal configuration space.
	 */
	al_mod_bool rd_dec_err_en;
	/**
	 * When set, reports a slave error, when the slave returns an AXI slave error, for
	 * configuration access to the internal configuration space.
	 */
	al_mod_bool wr_slv_err_en;
	/**
	 * When set, reports a decode error when timeout has occurred for configuration access to
	 * the internal configuration space.
	 */
	al_mod_bool wr_dec_err_en;
};

/**
 * APB access configuration
 *
 * @param	pbs_regs
 *		PBS register file base
 * @param	cfg
 *		Required configuration
 */
void al_mod_pbs_apb_mem_config_set(
	void __iomem			*pbs_regs,
	struct al_mod_pbs_apb_mem_config	*cfg);

#ifdef __cplusplus
}
#endif

#endif
/** @} end of PBS group */
