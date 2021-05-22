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
 * @file   al_hal_pmdt_ela_config.c
 *
 * @brief  PMDT Embedded Logic Analyzer Configuration wrapper HAL functions
 *
 */

#include "al_hal_pmdt_pmu_complex_regs.h"
#include "al_hal_pmdt_ela_config.h"
#include "al_hal_pmdt_ela_config_regs.h"
#include "al_hal_pmdt_common.h"

/**
 * Initialize the ELA-CONF handle
 */
int al_pmdt_elacfg_handle_init(
		struct al_pmdt_elacfg *pmdt_elacfg,
		const char *core_name,
		void __iomem *pmu_cmplx_base)
{
	struct al_pmdt_pmu_complex_regs *cmplx_regs;

	al_assert(pmdt_elacfg);
	al_assert(pmu_cmplx_base);

	if (core_name)
		pmdt_elacfg->name = core_name;
	else
		pmdt_elacfg->name = "";

	cmplx_regs =  (struct al_pmdt_pmu_complex_regs *)pmu_cmplx_base;
	pmdt_elacfg->regs =
			(struct al_pmdt_ela_config_regs *)&cmplx_regs->ela_config;

	al_dbg("pmdt ela config [%s]: init\n", pmdt_elacfg->name);

	return 0;
}

/**
 * Configure the ELA-CONF
 */
int al_pmdt_elacfg_config(
		struct al_pmdt_elacfg *pmdt_elacfg,
		const struct al_pmdt_elacfg_config *config)
{
	struct al_pmdt_ela_config_regs *regs;
	uint32_t val = 0;

	al_assert(pmdt_elacfg);
	al_assert(config);

	regs = (struct al_pmdt_ela_config_regs *)pmdt_elacfg->regs;

	AL_REG_MASK_SET_VAL(val,
			PMDT_ELA_CONFIG_ELA_CONFIG_ELA_CTRL0_DBGEN,
			config->nsid_dbg);
	AL_REG_MASK_SET_VAL(val,
			PMDT_ELA_CONFIG_ELA_CONFIG_ELA_CTRL0_NIDEN,
			config->nsnid_dbg);
	AL_REG_MASK_SET_VAL(val,
			PMDT_ELA_CONFIG_ELA_CONFIG_ELA_CTRL0_SPIDEN,
			config->sid_dbg);
	AL_REG_MASK_SET_VAL(val,
			PMDT_ELA_CONFIG_ELA_CONFIG_ELA_CTRL0_SPNIDEN,
			config->snid_dbg);
	AL_REG_MASK_SET_VAL(val,
			PMDT_ELA_CONFIG_ELA_CONFIG_ELA_CTRL0_EXT_DEBUG,
			config->ext_dbg_en);
	AL_REG_MASK_SET_VAL(val,
			PMDT_ELA_CONFIG_ELA_CONFIG_ELA_CTRL0_RST_ASSERT_LOG_EN,
			config->rst_assert_en);
	AL_REG_MASK_SET_VAL(val,
			PMDT_ELA_CONFIG_ELA_CONFIG_ELA_CTRL0_RST_DEASSERT_LOG_EN,
			config->rst_deassert_en);
	AL_REG_MASK_SET_VAL(val,
			PMDT_ELA_CONFIG_ELA_CONFIG_ELA_CTRL0_ELA_COND_EN,
			config->ela_cond_en);
	AL_REG_FIELD_SET(val,
			PMDT_ELA_CONFIG_ELA_CONFIG_ELA_CTRL0_PAUSE_EXTEND_MASK,
			PMDT_ELA_CONFIG_ELA_CONFIG_ELA_CTRL0_PAUSE_EXTEND_SHIFT,
			config->reset_delay_exp);

	al_reg_write32(&regs->ela_config.ela_ctrl0, val);

	return 0;
}

/*
 * Locally enable/disable ELA regardless of CCTM/PMU enable
 */
void al_pmdt_elacfg_local_ela_enable(
		struct al_pmdt_elacfg *pmdt_elacfg,
		al_bool is_enable)
{
	struct al_pmdt_ela_config_regs *regs;
	uint32_t val = 0;

	al_assert(pmdt_elacfg);

	regs = (struct al_pmdt_ela_config_regs *)pmdt_elacfg->regs;

	val = al_reg_read32(&regs->ela_config.ela_ctrl0);
	AL_REG_MASK_SET_VAL(val,
			PMDT_ELA_CONFIG_ELA_CONFIG_ELA_CTRL0_ENABLE_OVERRIDE,
			is_enable);
	al_reg_write32(&regs->ela_config.ela_ctrl0, val);
}

/*
 * Override ELA's ext_trig[2:7] input
 */
int al_pmdt_elacfg_exttrig_set(
		struct al_pmdt_elacfg *pmdt_elacfg,
		uint8_t exttrig_mask)
{
	struct al_pmdt_ela_config_regs *regs;
	uint32_t val = 0;

	al_assert(pmdt_elacfg);

	regs = (struct al_pmdt_ela_config_regs *)pmdt_elacfg->regs;

	val = al_reg_read32(&regs->ela_config.ela_ctrl0);
	exttrig_mask >>= 2; /* ignore 2 lsb triggers - reserved for reset signals*/
	AL_REG_FIELD_SET(val,
			PMDT_ELA_CONFIG_ELA_CONFIG_ELA_CTRL0_EVENT_OVRD_MASK,
			PMDT_ELA_CONFIG_ELA_CONFIG_ELA_CTRL0_EVENT_OVRD_SHIFT,
			(uint32_t)exttrig_mask);
	al_reg_write32(&regs->ela_config.ela_ctrl0, val);

	return 0;
}

/*
 * Set bit pattern that will be injected into ELA in reset events
 */
int al_pmdt_elacfg_rst_pattern_set(
		struct al_pmdt_elacfg *pmdt_elacfg,
		uint16_t assert_pattern,
		uint16_t deassert_pattern)
{
	struct al_pmdt_ela_config_regs *regs;
	uint32_t val;

	al_assert(pmdt_elacfg);

	regs = (struct al_pmdt_ela_config_regs *)pmdt_elacfg->regs;

	val = ((uint32_t)deassert_pattern) << 16;
	val |= (uint32_t)assert_pattern;
	al_reg_write32(&regs->ela_config.ela_ctrl1, val);

	return 0;
}

/*
 * Retrieve current ELA time stamp
 */
uint64_t al_pmdt_elacfg_timestamp_get(struct al_pmdt_elacfg *pmdt_elacfg)
{
	struct al_pmdt_ela_config_regs *regs;
	uint64_t ts;

	al_assert(pmdt_elacfg);

	regs = (struct al_pmdt_ela_config_regs *)pmdt_elacfg->regs;
	ts = al_hal_pmdt_read64_counter(&regs->ela_stat.ela_tstamp_low,
			&regs->ela_stat.ela_tstamp_high);

	return ts;
}

/** @} end of PMDT ELA Config group */
