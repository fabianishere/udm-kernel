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
 * @defgroup group_common HAL Common Layer
 *  @{
 * @file   al_hal_reg_utils.h
 *
 * @brief  Register utilities used by HALs and platform layer
 *
 *
 */

#ifndef __AL_HAL_REG_UTILS_H__
#define __AL_HAL_REG_UTILS_H__

#include "al_hal_types.h"

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

#define AL_BIT(b)		(1UL << (b))
#define AL_BIT_64(b)	(1ULL << (b))

#define AL_UINT64_LOW(x)	((uint32_t)(((uint64_t)(x)) & 0xffffffffULL))
#define AL_UINT64_HIGH(x)	((uint32_t)((x) >> 32))

#define AL_ADDR_LOW(x)	((uint32_t)((al_phys_addr_t)(x)))
#define AL_ADDR_HIGH(x)	((uint32_t)((((al_phys_addr_t)(x)) >> 16) >> 16))

/** get field out of 32 bit register */
#define AL_REG_FIELD_GET(reg, mask, shift)  (((reg) & (mask)) >> (shift))

/** get field which is single bit out of 32 bit register */
#define AL_REG_FIELD_BIT_GET(reg, mask)  (!!((reg) & (mask)))

/** set field of 32 bit register */
#define AL_REG_FIELD_SET(reg, mask, shift, val)			\
	(reg) =							\
		(((reg) & (~(mask))) |				\
		((((unsigned)(val)) << (shift)) & (mask)))

/** set field of 64 bit register */
#define AL_REG_FIELD_SET_64(reg, mask, shift, val)		\
	((reg) =						\
		(((reg) & (~(mask))) |				\
		((((uint64_t)(val)) << (shift)) & (mask))))

/** set field which is single bit of 32 bit register */
#define AL_REG_FIELD_BIT_SET(reg, mask, val)			\
	((reg) =						\
		((val) ? (reg) | (mask) : (reg) & ~(mask)))

/** get single bit out of 32 bit register */
#define AL_REG_BIT_GET(reg, shift)				\
	AL_REG_FIELD_GET(reg, AL_BIT(shift), shift)

#define AL_REG_BITS_FIELD(shift, val)				\
		(((unsigned)(val)) << (shift))

/** set single bit field of 32 bit register to a given value */
#define AL_REG_BIT_VAL_SET(reg, shift, val)				\
	AL_REG_FIELD_SET(reg, AL_BIT(shift), shift, val)

/** set single bit of 32 bit register to 1 */
#define AL_REG_BIT_SET(reg, shift)				\
	AL_REG_BIT_VAL_SET(reg, shift, 1)

/** clear single bit of 32 bit register */
#define AL_REG_BIT_CLEAR(reg, shift)				\
	AL_REG_BIT_VAL_SET(reg, shift, 0)


#define AL_BIT_MASK(n)						\
	(AL_BIT(n) - 1)

#define AL_BIT_MASK_64(n)						\
	(AL_BIT_64(n) - 1)

#define AL_FIELD_MASK(msb, lsb)					\
	(AL_BIT(msb) + AL_BIT_MASK(msb) - AL_BIT_MASK(lsb))

#define AL_FIELD_MASK_64(msb, lsb)				\
	(AL_BIT_64(msb) + AL_BIT_MASK_64(msb) - AL_BIT_MASK_64(lsb))

/** set value (0/1) to the bits specified by mask */
#define AL_REG_MASK_SET_VAL(reg, mask, val)			\
	((reg) = (val) ? (((reg) | (mask))) : (((reg) & (~(mask)))))

/** indicates whether any bit specified by mask is set */
#define AL_REG_MASK_IS_SET(reg, mask)			\
	(((reg) & (mask)) ? 1 : 0)

/** clear bits specified by mask */
#define AL_REG_MASK_CLEAR(reg, mask)			\
	AL_REG_MASK_SET_VAL(reg, mask, 0)

/** set bits specified by mask */
#define AL_REG_MASK_SET(reg, mask)			\
	AL_REG_MASK_SET_VAL(reg, mask, 1)

/** clear bits specified by clear_mask, and set bits specified by set_mask */
#define AL_REG_CLEAR_AND_SET(reg, clear_mask, set_mask)			\
	(reg) =	(((reg) & (~(clear_mask))) | (set_mask))

#define AL_ALIGN_UP(val, size)					\
	((size) * (((val) + (size) - 1) / (size)))

/** take bits selected by mask from one data, the rest from background */
#define AL_MASK_VAL(mask, data, background)		\
	(((mask) & (data)) | ((~(mask)) & (background)))

/** check validity of field value range */
#define AL_REG_FIELD_RANGE_VALID(val, mask, shift) \
	((val) <= ((mask) >> (shift)))

/* Extract the shift of the least significant bit that is on (1) or zero if no bit is on */
#define AL_SHIFT_OF_MASK(x)		(((x) == 0) ? 0 : __builtin_ctz(x))

/* is a number power of 2 */
#define AL_IS_POW_OF_TWO(x) ((x) && !((x) & ((x) - 1)))

/**
 * 8 bits register masked write
 *
 * @param	reg
 *	register address
 * @param	mask
 *	bits not selected (1) by mask will be left unchanged
 * @param	data
 *	data to write. bits not selected by mask ignored.
 */
static inline void al_reg_write8_masked(uint8_t __iomem *reg, uint8_t mask
								, uint8_t data)
{
	uint8_t temp;
	temp = al_reg_read8(reg);
	al_reg_write8(reg, AL_MASK_VAL(mask, data, temp));
}


/**
 * 16 bits register masked write
 *
 * @param	reg
 *	register address
 * @param	mask
 *	bits not selected (1) by mask will be left unchanged
 * @param	data
 *	data to write. bits not selected by mask ignored.
 */
static inline void al_reg_write16_masked(uint16_t __iomem *reg, uint16_t mask
								, uint16_t data)
{
	uint16_t temp;
	temp = al_reg_read16(reg);
	al_reg_write16(reg, AL_MASK_VAL(mask, data, temp));
}


/**
 * 32 bits register masked write
 *
 * @param	reg
 *	register address
 * @param	mask
 *	bits not selected (1) by mask will be left unchanged
 * @param	data
 *	data to write. bits not selected by mask ignored.
 */
#if !defined(AL_REG_WRITE_32_MASKED_DEFINED)
static inline void al_reg_write32_masked(uint32_t __iomem *reg, uint32_t mask, uint32_t data)
{
	uint32_t temp;
	temp = al_reg_read32(reg);
	al_reg_write32(reg, AL_MASK_VAL(mask, data, temp));
}
#endif

/* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */
/** @} end of Common group */
#endif

