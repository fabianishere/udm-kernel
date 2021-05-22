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
 * @defgroup group_pmdt_ela_config_api API
 * Performance Monitoring Debug and Trace - ELA-CONF, ELA Configuration wrapper
 * @ingroup group_pmdt_ela_config
 * @{
 * The ELA CONF is a configuration wrapper of the ELA module. The main runtime
 * operations it enables are ext_trig input enforcement and reset signals
 * pattern settings.
 *
 * @file   al_hal_pmdt_ela_config.h
 */

#ifndef __AL_HAL_PMDT_ELA_CONFIG_H__
#define __AL_HAL_PMDT_ELA_CONFIG_H__

#include "al_hal_common.h"

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

/** ELA-CONF configuration parameters */
struct al_pmdt_elacfg_config {
	unsigned int reset_delay_exp; /** pause delay of 2^x after reset x=[0-15] */
	al_bool ela_cond_en; /** COND_TRIG signal to ELA. Enables alt-state/action*/
	al_bool rst_assert_en; /**enable reset assert trigger via ext_trig[0]*/
	al_bool rst_deassert_en; /**enable reset de-assert trigger via ext_trig[1]*/
	al_bool ext_dbg_en; /** enable external debugger access */
	al_bool snid_dbg; /** enable secure non-invasive debug */
	al_bool sid_dbg; /** enable secure invasive debug */
	al_bool	nsnid_dbg; /** enable non-secure non-invasive debug */
	al_bool nsid_dbg; /** enable non-secure invasive debug */
};

/** ELA-CONF context */
struct al_pmdt_elacfg {
	const char *name;
	void __iomem *regs;
};

/**
 * Initialize the ELA-CONF handle
 *
 * @param pmdt_elacfg
 *		An allocated, non-initialized instance
 * @param core_name
 *		The name of the core ELA-CONF is integrated into
 *		Upper layer must keep string area
 * @param pmu_cmplx_base
 *		The base address of the PMU complex registers
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_pmdt_elacfg_handle_init(
		struct al_pmdt_elacfg *pmdt_elacfg,
		const char *core_name,
		void __iomem *pmu_cmplx_base);

/**
 * Configure the ELA-CONF
 *
 * @param pmdt_elacfg
 *		ELA-CONF handle
 * @param config
 *		ELA-CONF config parameters
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_pmdt_elacfg_config(
		struct al_pmdt_elacfg *pmdt_elacfg,
		const struct al_pmdt_elacfg_config *config);

/**
 * Locally enable/disable ELA regardless of CCTM/PMU enable
 *
 * Note. usually this function shouldn't be called as enablement shall arrive
 * from CCTM. Its main purpose is for testing ELA as stand alone.
 *
 * @param pmdt_elacfg
 *		ELA-CONF handle
 * @param is_enable
 *		0/1 - disable/enable
 */
void al_pmdt_elacfg_local_ela_enable(
		struct al_pmdt_elacfg *pmdt_elacfg,
		al_bool is_enable);

/**
 * Override ELA's ext_trig[2:7] input
 *
 * @param pmdt_elacfg
 *		ELA-CONF handle
 * @param exttrig_mask
 *		bit-mask for overriding ext_trig. Bits [0:1] are ignored. The other 1's
 *		bits override ext_trig[2:7] input with constant 1.
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_pmdt_elacfg_exttrig_set(
		struct al_pmdt_elacfg *pmdt_elacfg,
		uint8_t exttrig_mask);

/**
 * Set bit pattern that will be injected into ELA in reset events
 *
 * The 128-bit pattern is injected via all signal groups on reset
 * assertion / de-assertion (depend on rst_assert_en/rst_deassert_en values).
 * The injected 128-bit patterns are as follows:
 *
 * ASSERT:
 *  127               32 31   20 19   16 15                                 0
 * ---------------------------------------------------------------------------
 * |      all-zeros     | 0xADC |   0   | assert_pattern (default: 0xA55E)   |
 * ---------------------------------------------------------------------------
 *
 * DE-ASSERT:
 *  127               32 31   20 19   16 15                                 0
 * ---------------------------------------------------------------------------
 * |      all-zeros     | 0xADC |   1   | deassert_pattern (default: 0xDEA5) |
 * ---------------------------------------------------------------------------
 *
 * @param pmdt_elacfg
 *		ELA-CONF handle
 * @param assert_pattern
 *		user defined reset assertion pattern
 * @param deassert_pattern
 *		user defined reset de-assertion pattern
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_pmdt_elacfg_rst_pattern_set(
		struct al_pmdt_elacfg *pmdt_elacfg,
		uint16_t assert_pattern,
		uint16_t deassert_pattern);

/**
 * Retrieve current ELA time stamp
 *
 * - time stamp configuration is done via CCTM to all units
 *
 * @param pmdt_elacfg
 *		ELA-CONF handle
 *
 * @return current time stamp
 */
uint64_t al_pmdt_elacfg_timestamp_get(struct al_pmdt_elacfg *pmdt_elacfg);

/* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */

#endif /* __AL_HAL_PMDT_ELA_CONFIG_H__ */
/** @} end of group_pmdt_ela_config */

