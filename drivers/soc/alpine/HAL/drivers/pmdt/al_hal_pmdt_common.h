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
 *  @{
 * @file   al_hal_pmdt_common.h
 *
 * @brief common macros to be used by PMDT modules
 *
 */

#ifndef __AL_HAL_PMDT_COMMON_H__
#define __AL_HAL_PMDT_COMMON_H__

#include "al_hal_reg_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AL_PMDT_PMU_CMPLX_AXI_MON_NUM 2
#define AL_PMDT_PMU_CMPLX_ELA_NUM 2

/**
 * Read 64bit value from two 32bit registers that represent high a low 32bit
 * values
 *
 * @param	reglo
 *		low register address
 * @param	reghi
 *		high register address
 */
static INLINE uint64_t al_hal_pmdt_read64(
		uint32_t *reglo,
		uint32_t *reghi)
{
	uint64_t val, high_word;

	val = (uint64_t)al_reg_read32(reglo);
	high_word = (uint64_t)al_reg_read32(reghi);
	val |= (high_word << 32);

	return val;
}

/**
 * Read 64bit value from two 32bit registers while high register value is
 * latched by reading low register value (hence ordering is important)
 *
 * @param	reglo
 *		low register address
 * @param	reghi
 *		high register address
 */
static INLINE uint64_t al_hal_pmdt_read64_latched(
		uint32_t *reglo,
		uint32_t *reghi)
{
	uint64_t val, high_word;

	val = (uint64_t)al_reg_read32(reglo);
	al_data_memory_barrier();
	high_word = (uint64_t)al_reg_read32(reghi);
	val |= (high_word << 32);

	return val;
}

/**
 * Read 64bit incremental counter value from two 32bit registers that represent
 * high a low 32bit values
 *
 * @param	reglo
 *		low register address
 * @param	reghi
 *		high register address
 */
static INLINE uint64_t al_hal_pmdt_read64_counter(
		uint32_t *reglo,
		uint32_t *reghi)
{
	uint64_t val, high_word;

	val = (uint64_t)al_reg_read32(reglo);
	high_word = (uint64_t)al_reg_read32(reghi);

	/* check for wrap around in low register */
	if (val > (uint64_t)al_reg_read32(reglo)) {
		val = (uint64_t)al_reg_read32(reglo);
		high_word = (uint64_t)al_reg_read32(reghi);
	}

	val |= (high_word << 32);

	return val;
}

/**
 * Write 64bit value into two 32bit registers that represent high and low 32bit
 * values
 *
 * @param	val
 *		value to write
 * @param	reglo
 *		low register address
 * @param	reghi
 *		high register address
 */
static INLINE void al_hal_pmdt_write64(
		uint64_t val,
		uint32_t *reglo,
		uint32_t *reghi)
{
	al_reg_write32(reglo, (uint32_t)val);
	al_reg_write32(reghi, (uint32_t)(val>>32));
}

/**
 * Write 64bit value into two 32bit registers that represent high a low 32bit
 * values. low register is latched till high register is written
 *
 * @param	val
 *		value to write
 * @param	reglo
 *		low register address
 * @param	reghi
 *		high register address
 */
static INLINE void al_hal_pmdt_write64_latched(
		uint64_t val,
		uint32_t *reglo,
		uint32_t *reghi)
{
	al_reg_write32(reglo, (uint32_t)val);
	al_data_memory_barrier();
	al_reg_write32(reghi, (uint32_t)(val>>32));
}

#ifdef __cplusplus
}
#endif

#endif /* __AL_HAL_PMDT_COMMON_H__*/

/** @} */


