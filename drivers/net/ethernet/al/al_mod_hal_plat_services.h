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
 *  Platform Services API
 *  @{
 * @file   al_mod_hal_plat_services.h
 *
 * @brief  API for Platform services provided for to HAL drivers
 *
 *
 */

#ifndef __PLAT_SERVICES_H__
#define __PLAT_SERVICES_H__

#include <linux/kernel.h>
#include <linux/io.h>
#include <linux/version.h>
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,37))
#include <linux/printk.h>
#endif
#include <linux/delay.h>

#include <linux/errno.h>
#include <linux/bitops.h>

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

#define al_mod_reg_read8(l)		readb(l)
#define al_mod_reg_read16(l)	readw(l)
#define al_mod_reg_read32(l)	readl(l)
#define al_mod_reg_read64(l)

#define al_mod_reg_write8(l,v)	writeb(v,l)
#define al_mod_reg_write16(l,v)	writew(v,l)
#define al_mod_reg_write32(l,v)	writel(v,l)

#ifdef CONFIG_ARM
/*
 * Relaxed register read/write functions don't involve cpu instructions that
 * force syncronization, nor ordering between the register access and memory
 * data access.
 * These instructions are used in performance critical code to avoid the
 * overhead of the synchronization instructions.
 */
#define al_mod_reg_read32_relaxed(l)	readl_relaxed(l)
#define al_mod_reg_write32_relaxed(l,v)	writel_relaxed(v,l)
#else
#define al_mod_reg_read32_relaxed(l)	readl(l)
#define al_mod_reg_write32_relaxed(l,v)	writel(v,l)
#endif

/**
 * print message
 *
 * @param type of message
 * @param format
 */
#define al_mod_print(fmt, ...)		printk(KERN_INFO fmt, ##__VA_ARGS__)

/**
 * print error message
 *
 * @param format
 */
#define al_mod_err(...)			pr_err(__VA_ARGS__)

/**
 * print warning message
 *
 * @param format
 */
#define al_mod_warn(...)			pr_warn(__VA_ARGS__)

/**
 * print info message
 *
 * @param format
 */
#define al_mod_info(...)			pr_info(__VA_ARGS__)

/**
 * print debug message
 *
 * @param format
 */
#define al_mod_dbg(...)			pr_debug(__VA_ARGS__)

/**
 * Assertion
 *
 * @param condition
 */
#define al_mod_assert(COND)		BUG_ON(!(COND))

/**
 * Assertion with message
 *
 * @param COND condition
 * @param format The format string
 * @param ... Additional arguments
 */
#define al_mod_assert_msg(COND, ...)\
	do {			\
		if (!(COND)) {	\
			al_mod_err(	\
			"%s:%d:%s: Assertion failed! (%s)\n",	\
			__FILE__, __LINE__, __func__, #COND);	\
			al_mod_err(__VA_ARGS__);			\
			al_mod_assert(0);				\
		}						\
	} while (AL_FALSE)

/**
  * Make sure data will be visible by DMA masters. usually this is achieved by
  * the ARM DMB instruction.
  */

static inline void al_mod_data_memory_barrier(void)
{
	mb();
}

/**
  * Make sure data will be visible in order by other cpus masters.
  */
static inline void al_mod_smp_data_memory_barrier(void)
{
	smp_mb();
}

static inline void al_mod_local_data_memory_barrier(void)
{
	mb();
}

/**
 * al_mod_udelay - micro sec delay
 * @param micro seconds to delay
 */
#define al_mod_udelay(u)    udelay(u)
#define al_mod_usleep_range(x,y) usleep_range(x,y)

#define al_mod_msleep(m)		msleep(m)

#define swap16_to_le(x)		cpu_to_le16(x)
#define swap32_to_le(x)		cpu_to_le32(x)
#define swap64_to_le(x)		cpu_to_le64(x)
#define swap16_from_le(x)	le16_to_cpu(x)
#define swap32_from_le(x)	le32_to_cpu(x)
#define swap64_from_le(x)	le64_to_cpu(x)

/**
 * Memory set
 *
 * @param memory pointer
 * @param value for setting
 * @param number of bytes to set
 */
#define al_mod_memset(p, val, cnt)  memset(p, val, cnt)

/**
 * memory compare
 *
 * @param  p1 memory pointer
 * @param  p2 memory pointer
 * @param  cnt number of bytes to compare
 *
 * @return 0 if equal, else otherwise
 */
#define al_mod_memcmp(p1, p2, cnt)  memcmp(p1, p2, cnt)

/**
 * memory copy
 *
 * @param  dest memory pointer to destination
 * @param  src memory pointer to source
 * @param  cnt number of bytes to copy
 */
#define al_mod_memcpy(dest, src, cnt)	memcpy(dest, src, cnt)

#define al_mod_popcount(x) hweight_long(x)

/* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */
/** @} end of Platform Services API group */
#endif				/* __PLAT_SERVICES_H__ */
