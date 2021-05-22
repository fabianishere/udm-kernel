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
 * @defgroup group_pbs_interrupts PBS Interrupt Controller
 * @ingroup group_pbs_api
 *  PBS IOFIC API
 *  @{
 * @file   al_mod_hal_pbs_iofic.h
 *
 * @brief C Header file for programming the interrupt controller that found
 * in the PBS unit. These APIs rely and use some the Interrupt controller
 * API under al_mod_hal_iofic.h
 */

#ifndef __AL_HAL_PBS_IOFIC_H__
#define __AL_HAL_PBS_IOFIC_H__

#include <al_mod_hal_iofic.h>
#include <al_mod_hal_pbs_utils.h>

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

/* PBS interrupt lines */
#define AL_PBS_IOFIC_INT_ID_BOOTROOM_PARITY_ERROR	AL_BIT(2)
#define AL_PBS_IOFIC_INT_ID_UFC_PARITY_ERROR		AL_BIT(3)
#define AL_PBS_IOFIC_INT_ID_APB_PREADY_NO_PSEL_ERROR	AL_BIT(4)
#define AL_PBS_IOFIC_INT_ID_APB_DOUBLE_PREADY_ERROR	AL_BIT(5)
#define AL_PBS_IOFIC_INT_ID_THERMAL_SENSOR_RESULT	AL_BIT(6)
#define AL_PBS_IOFIC_INT_ID_THERMAL_SENSOR_HIGH		AL_BIT(7)
#define AL_PBS_IOFIC_INT_ID_THERMAL_SENSOR_LOW		AL_BIT(8)
#define AL_PBS_IOFIC_INT_ID_PLL_SB			AL_BIT(9)
#define AL_PBS_IOFIC_INT_ID_PLL_NB			AL_BIT(10)
#define AL_PBS_IOFIC_INT_ID_PLL_CPU			AL_BIT(11)
#define AL_PBS_IOFIC_INT_ID_TDM_TX_FULL			AL_BIT(12)
#define AL_PBS_IOFIC_INT_ID_TDM_TX_ALMOST_EMPTY		AL_BIT(13)
#define AL_PBS_IOFIC_INT_ID_TDM_TX_EMPTY		AL_BIT(14)
#define AL_PBS_IOFIC_INT_ID_TDM_RX_ALMOST_FULL		AL_BIT(15)
#define AL_PBS_IOFIC_INT_ID_TDM_RX_FULL			AL_BIT(16)
#define AL_PBS_IOFIC_INT_ID_TDM_RX_OVERRRUN		AL_BIT(17)
#define AL_PBS_IOFIC_INT_ID_TDM_RX_EMPTY		AL_BIT(18)
#define AL_PBS_IOFIC_INT_ID_TDM_TX_PARITY_ERROR		AL_BIT(19)
#define AL_PBS_IOFIC_INT_ID_TDM_RX_PARITY_ERROR		AL_BIT(20)
#define AL_PBS_IOFIC_INT_ID_TDM_SPI			AL_BIT(21)
#define AL_PBS_IOFIC_INT_ID_TDM_ZSI			AL_BIT(22)
/* Alpine V3 */
#define AL_PBS_IOFIC_INT_ID_PLL_CPU_1			AL_BIT(23)
#define AL_PBS_IOFIC_INT_ID_PLL_PCIE0			AL_BIT(24)
#define AL_PBS_IOFIC_INT_ID_PLL_PCIE1			AL_BIT(25)
#define AL_PBS_IOFIC_INT_ID_PLL_PCIE2			AL_BIT(26)

static inline struct al_mod_iofic_regs __iomem *al_mod_pbs_iofic_regs_get(void __iomem *pbs_regs)
{
	struct al_mod_pbs_regs __iomem *regs = (struct al_mod_pbs_regs __iomem *)pbs_regs;
	unsigned int dev_id = al_mod_pbs_dev_id_get(pbs_regs);
	struct al_mod_iofic_regs __iomem *iofic_regs =
		(dev_id <= PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V2) ?
		(struct al_mod_iofic_regs __iomem *)&regs->iofic_base :
		(struct al_mod_iofic_regs __iomem *)&regs->iofic_base_v3;

	return iofic_regs;
}

#endif
/** @} end of PBS group */
