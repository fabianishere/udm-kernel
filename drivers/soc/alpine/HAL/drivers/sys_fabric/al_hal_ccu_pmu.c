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
 * @addtogroup groupccu
 *
 *  @{
 * @file   al_hal_ccu_pmu.c
 *
 * @brief  CCU PMU HAL driver
 *
 */


#include "al_hal_common.h"
#include "al_hal_ccu_regs.h"
#include "al_hal_ccu_pmu.h"

/**
 * @brief Initialize ccu pmu handle
 *
 * @param  pmu_handle
 *	   CCU pmu handle struct
 *
 * @param  ccu_regs_base
 *         ccu registers base address
 */
void al_ccu_pmu_handle_init(struct al_hal_ccu_pmu_handle *pmu_handle, void *ccu_regs_base)
{
	pmu_handle->ccu_regs = (struct al_ccu_regs*)ccu_regs_base;
}



/**
 * @brief Enable all PMU counters
 *
 * @param  pmu_handle
 *	   CCU pmu handle struct
 */
void al_ccu_pmu_enable(struct al_hal_ccu_pmu_handle *pmu_handle)
{
	struct al_ccu_regs* al_ccu_regs = pmu_handle->ccu_regs;
	al_reg_write32_masked(&al_ccu_regs->pmcr,
			      CCU_PMU_PMCR_ENABLE,
			      CCU_PMU_PMCR_ENABLE);
	return;
}

/**
 * @brief Disable all PMU counters
 *
 * @param  pmu_handle
 *	   CCU pmu handle struct
 */
void al_ccu_pmu_disable(struct al_hal_ccu_pmu_handle *pmu_handle)
{
	struct al_ccu_regs* al_ccu_regs = pmu_handle->ccu_regs;
	al_reg_write32_masked(&al_ccu_regs->pmcr,
			      CCU_PMU_PMCR_ENABLE,
			      0);

	return;
}


/**
 * @brief Resets al PMU counters
 *
 * @param  pmu_handle
 *	   CCU pmu handle struct
 */
 void al_ccu_pmu_cntrs_reset(struct al_hal_ccu_pmu_handle *pmu_handle) {
	struct al_ccu_regs* al_ccu_regs = pmu_handle->ccu_regs;

	al_reg_write32_masked(&al_ccu_regs->pmcr,
			      CCU_PMU_PMCR_RESET,
			      CCU_PMU_PMCR_RESET);
	return;
}

/**
 * @brief Set type of counter
 *
 * @param       pmu_handle
 *	        CCU pmu handle struct
 *
 * @param	index
 *		Index of the counter
 *
 * @param       event
 *              Type of the counter
 *
 * @param       src
 *              Source of the counter
 *
 */
void al_ccu_pmu_set_counter(struct al_hal_ccu_pmu_handle *pmu_handle,
			    int index,
			    enum AL_CCU_PMU_EVENT_TYPE event,
			    enum AL_CCU_PMU_SRC_INTERFACE src)
{
	struct al_ccu_regs* al_ccu_regs = pmu_handle->ccu_regs;
	al_assert(index < AL_CCU_PMU_REG_COUNT);
	al_assert(src < AL_CCU_PMU_SRC_LAST);
	al_assert(event < AL_CCU_PMU_EVENT_LAST);
	al_reg_write32(&al_ccu_regs->pmu_cntrs[index].sel,
		       (event | (src << AL_CCU_PMU_SRC_OFFSET)));
	return;
}

/**
 * @brief Enable PMU counter
 *
 * @param       pmu_handle
 *	        CCU pmu handle struct
 *
 * @param	index
 *		Index of the counter
 */
void al_ccu_pmu_counter_enable(struct al_hal_ccu_pmu_handle *pmu_handle,
			       int index)
{
	struct al_ccu_regs* al_ccu_regs = pmu_handle->ccu_regs;
	al_assert(index < AL_CCU_PMU_REG_COUNT);
	al_reg_write32(&al_ccu_regs->pmu_cntrs[index].ctrl,
		       CCU_PMU_COUNT_CTRL_ENABLE);
	return;
}


/**
 * @brief Disable PMU counter
 *
 * @param       pmu_handle
 *	        CCU pmu handle struct
 *
 * @param	index
 *		Index of the counter
 */
void al_ccu_pmu_counter_disable(struct al_hal_ccu_pmu_handle *pmu_handle,
				int index)
{
	struct al_ccu_regs* al_ccu_regs = pmu_handle->ccu_regs;
	al_assert(index < AL_CCU_PMU_REG_COUNT);
	al_reg_write32(&al_ccu_regs->pmu_cntrs[index].ctrl, 0x0);
	return;
}


/**
 * @brief Read PMU counter
 *
 * @param       pmu_handle
 *	        CCU pmu handle struct
 *
 * @param	index
 *		Index of the counter
 *
 * @returns	Value of the counter
 */
uint32_t al_ccu_pmu_counter_read(struct al_hal_ccu_pmu_handle *pmu_handle,
				 int index)
{
	struct al_ccu_regs* al_ccu_regs = pmu_handle->ccu_regs;
	al_assert(index < AL_CCU_PMU_REG_COUNT);
	return al_reg_read32(&al_ccu_regs->pmu_cntrs[index].counter);
}

/** @} end of CCU group */
