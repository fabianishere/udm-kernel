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
 * @addtogroup groupddr
 *
 *  @{
 * @file   al_hal_ddr_pmu.c
 *
 * @brief  DDR controller PMU HAL driver
 *
 */

#if (defined(AL_DEV_ID) && (AL_DEV_ID <= AL_DEV_ID_ALPINE_V2))

#include "al_hal_ddr_pmu.h"
#include "al_hal_nb_regs.h"

/* Disable counter. */
#define NB_MC_PMU_COUNTERS_CNTL_CNT_STATE_DISABLE \
		(0x0 << NB_MC_PMU_COUNTERS_CNTL_CNT_STATE_SHIFT)
/* Enable counter.  */
#define NB_MC_PMU_COUNTERS_CNTL_CNT_STATE_ENABLE \
		(0x1 << NB_MC_PMU_COUNTERS_CNTL_CNT_STATE_SHIFT)
/* Pause counter.  */
#define NB_MC_PMU_COUNTERS_CNTL_CNT_STATE_PAUSE \
		(0x3 << NB_MC_PMU_COUNTERS_CNTL_CNT_STATE_SHIFT)

/*******************************************************************************
 ** Static Functions
 ******************************************************************************/
static inline int al_ddr_pmu_counter_valid(
	void __iomem			*nb_regs_base,
	unsigned int			idx)
{
	return idx < al_ddr_pmu_num_counters_get(nb_regs_base);
}

static void _al_ddr_pmu_disable_counter(
	void __iomem			*nb_regs_base,
	unsigned int			idx)
{
	struct al_nb_regs __iomem *nb_regs =
		(struct al_nb_regs __iomem *)nb_regs_base;

	al_reg_write32_masked(&nb_regs->mc_pmu_counters[idx].cntl,
			NB_MC_PMU_COUNTERS_CNTL_CNT_STATE_MASK,
			NB_MC_PMU_COUNTERS_CNTL_CNT_STATE_DISABLE);
}

static void _al_ddr_pmu_enable_counter(
	void __iomem			*nb_regs_base,
	unsigned int			idx)
{
	struct al_nb_regs __iomem *nb_regs =
		(struct al_nb_regs __iomem *)nb_regs_base;

	al_reg_write32_masked(&nb_regs->mc_pmu_counters[idx].cntl,
			NB_MC_PMU_COUNTERS_CNTL_CNT_STATE_MASK,
			NB_MC_PMU_COUNTERS_CNTL_CNT_STATE_ENABLE);

}

/*******************************************************************************
 ** API Implementations
 ******************************************************************************/

uint64_t al_ddr_pmu_counter_get(
	void __iomem			*nb_regs_base,
	unsigned int			idx)
{
	struct al_nb_regs __iomem *nb_regs =
		(struct al_nb_regs __iomem *)nb_regs_base;
	uint32_t counter_low, counter_high_presample, counter_high_postsample;
	uint64_t counter;

	al_assert(nb_regs_base);
	al_assert(al_ddr_pmu_counter_valid(nb_regs_base,idx));

	do {
		counter_high_presample =
			al_reg_read32(&nb_regs->mc_pmu_counters[idx].high);
		counter_low =
			al_reg_read32(&nb_regs->mc_pmu_counters[idx].low);
		counter_high_postsample =
			al_reg_read32(&nb_regs->mc_pmu_counters[idx].high);
	/* if cnt_high changed during sampling, re-sample the value */
	} while (counter_high_presample != counter_high_postsample);

	counter = (((uint64_t)counter_high_postsample << 32) | (uint64_t)counter_low);
	return counter;
}

void al_ddr_pmu_event_sel(
	void __iomem			*nb_regs_base,
	unsigned int			idx,
	enum al_ddr_perf_types		type)
{
	struct al_nb_regs __iomem *nb_regs =
		(struct al_nb_regs __iomem *)nb_regs_base;

	al_assert(nb_regs_base);
	al_assert(al_ddr_pmu_counter_valid(nb_regs_base,idx));

	_al_ddr_pmu_disable_counter(nb_regs_base,idx);

	al_reg_write32_masked(&nb_regs->mc_pmu_counters[idx].cfg,
			NB_MC_PMU_COUNTERS_CFG_EVENT_SEL_MASK,
			type << NB_MC_PMU_COUNTERS_CFG_EVENT_SEL_SHIFT);

	_al_ddr_pmu_enable_counter(nb_regs_base,idx);

}

void al_ddr_pmu_enable_counter(
	void __iomem			*nb_regs_base,
	unsigned int			idx)
{
	al_assert(nb_regs_base);
	al_assert(al_ddr_pmu_counter_valid(nb_regs_base,idx));

	_al_ddr_pmu_enable_counter(nb_regs_base,idx);

}

void al_ddr_pmu_disable_counter(
	void __iomem			*nb_regs_base,
	unsigned int			idx)
{
	al_assert(nb_regs_base);
	al_assert(al_ddr_pmu_counter_valid(nb_regs_base,idx));

	_al_ddr_pmu_disable_counter(nb_regs_base,idx);
}

void al_ddr_pmu_pause_counter(
	void __iomem			*nb_regs_base,
	unsigned int			idx)
{
	struct al_nb_regs __iomem *nb_regs =
		(struct al_nb_regs __iomem *)nb_regs_base;

	al_assert(nb_regs_base);
	al_assert(al_ddr_pmu_counter_valid(nb_regs_base,idx));

	al_reg_write32_masked(&nb_regs->mc_pmu_counters[idx].cntl,
			NB_MC_PMU_COUNTERS_CNTL_CNT_STATE_MASK,
			NB_MC_PMU_COUNTERS_CNTL_CNT_STATE_PAUSE);
}

void al_ddr_pmu_disable(
	void __iomem			*nb_regs_base)
{
	struct al_nb_regs __iomem *nb_regs =
		(struct al_nb_regs __iomem *)nb_regs_base;

	al_assert(nb_regs_base);

	al_reg_write32_masked(&nb_regs->mc_pmu.pmu_control,
			NB_MC_PMU_PMU_CONTROL_DISABLE_ALL,
			NB_MC_PMU_PMU_CONTROL_DISABLE_ALL);

}

void al_ddr_pmu_pause(
	void __iomem			*nb_regs_base)
{
	struct al_nb_regs __iomem *nb_regs =
		(struct al_nb_regs __iomem *)nb_regs_base;

	al_assert(nb_regs_base);

	al_reg_write32_masked(&nb_regs->mc_pmu.pmu_control,
			NB_MC_PMU_PMU_CONTROL_PAUSE_ALL,
			NB_MC_PMU_PMU_CONTROL_PAUSE_ALL);
}

void al_ddr_pmu_enable(
	void __iomem			*nb_regs_base)
{
	struct al_nb_regs __iomem *nb_regs =
		(struct al_nb_regs __iomem *)nb_regs_base;

	al_assert(nb_regs_base);

	al_reg_write32_masked(&nb_regs->mc_pmu.pmu_control,
			NB_MC_PMU_PMU_CONTROL_PAUSE_ALL |
			NB_MC_PMU_PMU_CONTROL_DISABLE_ALL,
			0);
}

unsigned int al_ddr_pmu_num_counters_get(
	void __iomem			*nb_regs_base)
{
	struct al_nb_regs __iomem *nb_regs =
		(struct al_nb_regs __iomem *)nb_regs_base;
	uint32_t reg_val;
	unsigned int nb_cnt;

	al_assert(nb_regs_base);

	reg_val = al_reg_read32(&nb_regs->mc_pmu.pmu_control);
	nb_cnt = (reg_val & NB_MC_PMU_PMU_CONTROL_NUM_OF_CNTS_MASK) >> NB_MC_PMU_PMU_CONTROL_NUM_OF_CNTS_SHIFT;

	return nb_cnt;
}

#endif

/** @} end of DDR group */

