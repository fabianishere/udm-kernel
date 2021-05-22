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
 * @defgroup group_services Platform Services API
 *  @{
 * The Platform Services API provides miscellaneous system services to HAL
 * drivers, such as:
 * - Registers read/write
 * - Assertions
 * - Memory barriers
 * - Endianness conversions
 *
 * And more.
 * @file   plat_api/sample/al_hal_plat_services.h
 *
 * @brief  API for Platform services provided for to HAL drivers
 *
 *
 */

#ifndef __PLAT_SERVICES_H__
#define __PLAT_SERVICES_H__

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

/**
 * Read MMIO 8 bits register
 * @param  offset	register offset
 *
 * @return register value
 */
uint8_t al_reg_read8(uint8_t * offset);

/**
 * Read MMIO 16 bits register
 * @param  offset	register offset
 *
 * @return register value
 */
uint16_t al_reg_read16(uint16_t * offset);

/**
 * Read MMIO 32 bits register
 * @param  offset	register offset
 *
 * @return register value
 */
uint32_t al_reg_read32(uint32_t * offset);

/**
 * Read MMIO 64 bits register
 * @param  offset	register offset
 *
 * @return register value
 */
uint64_t al_reg_read64(uint64_t * offset);

/**
 * Relaxed read MMIO 32 bits register
 *
 * Relaxed register read/write functions don't involve cpu instructions that
 * force syncronization, nor ordering between the register access and memory
 * data access.
 * These instructions are used in performance critical code to avoid the
 * overhead of the synchronization instructions.
 *
 * @param  offset	register offset
 *
 * @return register value
 */
uint32_t al_reg_read32_relaxed(uint32_t *offset);

/**
 * Write to MMIO 8 bits register
 * @param  offset	register offset
 * @param  val		value to write to the register
 */
void al_reg_write8(uint8_t * offset, uint8_t val);

/**
 * Write to MMIO 16 bits register
 * @param  offset	register offset
 * @param  val		value to write to the register
 */
void al_reg_write16(uint16_t * offset, uint16_t val);

/**
 * Write to MMIO 32 bits register
 * @param  offset	register offset
 * @param  val		value to write to the register
 */
void al_reg_write32(uint32_t * offset, uint32_t val);

/**
 * Relaxed write to MMIO 32 bits register
 *
 * Relaxed register read/write functions don't involve cpu instructions that
 * force syncronization, nor ordering between the register access and memory
 * data access.
 * These instructions are used in performance critical code to avoid the
 * overhead of the synchronization instructions.
 *
 * @param  offset	register offset
 * @param  val		value to write to the register
 */
void al_reg_write32_relaxed(uint32_t *offset, uint32_t val);

/**
 * Write to MMIO 64 bits register
 * @param  offset	register offset
 * @param  val		value to write to the register
 */
void al_reg_write64(uint64_t * offset, uint64_t val);


/**
 * print message
 *
 * @param format The format string
 * @param ... Additional arguments
 */
#define al_print printf


/**
 * print error message
 *
 * @param format The format string
 * @param ... Additional arguments
 */
#define al_err al_print

/**
 * print warning message
 *
 * @param format The format string
 * @param ... Additional arguments
 */
#define al_warn al_print


/**
 * print info message
 *
 * @param format The format string
 * @param ... Additional arguments
 */
#define al_info al_print

/**
 * print debug message
 *
 * @param format The format string
 * @param ... Additional arguments
 */
#define al_dbg al_print

/**
 * Send formatted output to a string
 */
#define al_sprintf sprintf

/**
 * Assertion
 *
 * @param COND condition
 */
#define al_assert(COND)		\
	do {			\
		if (!(COND)) {	\
			al_err(	\
      			"%s:%d:%s: Assertion failed! (%s)\n",	\
      			__FILE__, __LINE__, __func__, #COND);	\
			exit(-1);				\
		}						\
  	} while(AL_FALSE)

/**
 * Assertion with message
 *
 * @param COND condition
 * @param ... Additional arguments
 */
#define al_assert_msg(COND, ...)\
	do {			\
		if (!(COND)) {	\
			al_err(	\
			"%s:%d:%s: Assertion failed! (%s)\n",	\
			__FILE__, __LINE__, __func__, #COND);	\
			al_err(__VA_ARGS__);			\
			exit(-1);				\
		}						\
	} while (AL_FALSE)

/**
  * Make sure data will be visible by other masters (other CPUS and DMA).
  * usually this is achieved by the ARM DMB instruction.
  */
void al_data_memory_barrier(void);

/**
  * Make sure data will be visible by DMA masters, no restriction for other cpus
  */
void al_local_data_memory_barrier(void);

/**
  * Make sure data will be visible in order by other cpus masters.
  */
void al_smp_data_memory_barrier(void);

/**
  * Make sure write data will be visible in order by other cpus masters.
  */
void al_smp_write_data_memory_barrier(void);

/**
 * al_udelay - micro sec delay
 */
void al_udelay(unsigned long loops);

/**
 * al_msleep - mili sec delay
 */
void al_msleep(unsigned long loops);

/* Assume Little Endian CPU */
#if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
#error "CPU is not little endian"
#endif

/**
 * swap half word to little endian
 *
 * @param x 16 bit value
 *
 * @return the value in little endian
 */
#define swap16_to_le(x)		(x)
/**
 * swap word to little endian
 *
 * @param x 32 bit value
 *
 * @return the value in little endian
 */
#define swap32_to_le(x)		(x)

/**
 * swap 8 bytes to little endian
 *
 * @param x 64 bit value
 *
 * @return the value in little endian
 */
#define swap64_to_le(x)		(x)

/**
 * swap half word from little endian
 *
 * @param x 16 bit value
 *
 * @return the value in the cpu endianess
 */
#define swap16_from_le(x)		(x)

/**
 * swap word from little endian
 *
 * @param x 32 bit value
 *
 * @return the value in the cpu endianess
 */
#define swap32_from_le(x)		(x)

/**
 * swap 8 bytes from little endian
 *
 * @param x 64 bit value
 *
 * @return the value in the cpu endianess
 */
#define swap64_from_le(x)		(x)

/**
 * swap half word to big endian
 *
 * @param x 16 bit value
 *
 * @return the value in big endian
 */
#define swap16_to_be(x)		__builtin_bswap16(x)
/**
 * swap word to big endian
 *
 * @param x 32 bit value
 *
 * @return the value in big endian
 */
#define swap32_to_be(x)		__builtin_bswap32(x)

/**
 * swap 8 bytes to big endian
 *
 * @param x 64 bit value
 *
 * @return the value in big endian
 */
#define swap64_to_be(x)		__builtin_bswap64(x)

/**
 * swap half word from big endian
 *
 * @param x 16 bit value
 *
 * @return the value in the cpu endianess
 */
#define swap16_from_be(x)		__builtin_bswap16(x)

/**
 * swap word from little endian
 *
 * @param x 32 bit value
 *
 * @return the value in the cpu endianess
 */
#define swap32_from_be(x)		__builtin_bswap32(x)

/**
 * swap 8 bytes from little endian
 *
 * @param x 64 bit value
 *
 * @return the value in the cpu endianess
 */
#define swap64_from_be(x)		__builtin_bswap64(x)

/**
 * Memory set
 *
 * @param p memory pointer
 * @param val value for setting
 * @param cnt number of bytes to set
 */
#define al_memset(p, val, cnt)	memset(p, val, cnt)

/**
 * Memory copy
 *
 * @param p1 memory pointer
 * @param p2 memory pointer
 * @param cnt number of bytes to copy
 */
#define al_memcpy(p1, p2, cnt)	memcpy(p1, p2, cnt)

/**
 * Memory compare
 *
 * @param p1 memory pointer
 * @param p2 memory pointer
 * @param cnt number of bytes to compare
 */
#define al_memcmp(p1, p2, cnt)	memcmp(p1, p2, cnt)

/**
 * String compare
 *
 * @param s1 string pointer
 * @param s2 string pointer
 */
#define al_strcmp(s1, s2)	strcmp(s1, s2)

#define al_get_cpu_id()		0

#define al_get_cluster_id() 0

/* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */
/** @} end of Platform Services API group */
#endif				/* __PLAT_SERVICES_H__ */
