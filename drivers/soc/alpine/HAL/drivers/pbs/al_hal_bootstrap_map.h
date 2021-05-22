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
 * @defgroup group_bootstrap Bootstrap
 *  Bootstrap
 *  @{
 * @file   al_hal_bootstrap_map.h
 *
 * @brief  includes bootstrap register fields bits
 *
 */

#ifndef __AL_HAL_BOOTSTRAP_MAPS_H__
#define __AL_HAL_BOOTSTRAP_MAPS_H__

#include "al_hal_common.h"

#define AL_HAL_BOOTSTRAP_CPU_PLL_FREQ_MASK		(AL_FIELD_MASK(3, 0))
#define AL_HAL_BOOTSTRAP_CPU_PLL_FREQ_SHIFT		0
#define AL_HAL_BOOTSTRAP_NB_PLL_FREQ_MASK		(AL_FIELD_MASK(6, 4))
#define AL_HAL_BOOTSTRAP_NB_PLL_FREQ_SHIFT		4
#define AL_HAL_BOOTSTRAP_SB_PLL_FREQ_MASK		(AL_FIELD_MASK(8, 7))
#define AL_HAL_BOOTSTRAP_SB_PLL_FREQ_SHIFT		7
#define AL_HAL_BOOTSTRAP_SB_CLK_FREQ_MASK		(AL_FIELD_MASK(10, 9))
#define AL_HAL_BOOTSTRAP_SB_CLK_FREQ_SHIFT		9
#define AL_HAL_BOOTSTRAP_I2C_PRELOAD_MASK		(AL_FIELD_MASK(12, 12))
#define AL_HAL_BOOTSTRAP_I2C_PRELOAD_SHIFT		12
#define AL_HAL_BOOTSTRAP_SPIS_PRELOAD_MASK		(AL_FIELD_MASK(13, 13))
#define AL_HAL_BOOTSTRAP_SPIS_PRELOAD_SHIFT		13
#define AL_HAL_BOOTSTRAP_PRELOAD_MASK			(AL_FIELD_MASK(13, 12))
#define AL_HAL_BOOTSTRAP_PRELOAD_SHIFT			12
#define AL_HAL_BOOTSTRAP_BOOT_ROM_MASK			(AL_FIELD_MASK(14, 14))
#define AL_HAL_BOOTSTRAP_BOOT_ROM_SHIFT			14
#define AL_HAL_BOOTSTRAP_BOOT_DEVICE_MASK		(AL_FIELD_MASK(17, 15))
#define AL_HAL_BOOTSTRAP_BOOT_DEVICE_SHIFT		15
#define AL_HAL_BOOTSTRAP_DEBUG_MODE_MASK		(AL_FIELD_MASK(18, 18))
#define AL_HAL_BOOTSTRAP_DEBUG_MODE_SHIFT		18
#define AL_HAL_BOOTSTRAP_PLL_REF_CLK_FREQ_MASK		(AL_FIELD_MASK(19, 19))
#define AL_HAL_BOOTSTRAP_PLL_REF_CLK_FREQ_SHIFT		19
#define AL_HAL_BOOTSTRAP_CPU_EXIST_MASK			(AL_FIELD_MASK(21, 20))
#define AL_HAL_BOOTSTRAP_CPU_EXIST_SHIFT		20
#define AL_HAL_BOOTSTRAP_SPIM_SW_MODE_MASK		(AL_FIELD_MASK(23, 22))
#define AL_HAL_BOOTSTRAP_SPIM_SW_MODE_SHIFT		22
#define AL_HAL_BOOTSTRAP_I2C_PRELOAD_ADDR_MASK		(AL_FIELD_MASK(23, 23))
#define AL_HAL_BOOTSTRAP_I2C_PRELOAD_ADDR_SHIFT		23
#define AL_HAL_BOOTSTRAP_PRELOAD_ATT_MASK		(AL_FIELD_MASK(27, 26))
#define AL_HAL_BOOTSTRAP_PRELOAD_ATT_SHIFT	26
#define AL_HAL_BOOTSTRAP_DCACHE_DIS			AL_BIT(30)
#define AL_HAL_BOOTSTRAP_LLC_DIS			AL_BIT(31)

#define AL_HAL_BOOTSTRAP_2_SPIM_SW_MODE_MSB_MASK	(AL_FIELD_MASK(0, 0))
#define AL_HAL_BOOTSTRAP_2_SPIM_SW_MODE_MSB_SHIFT	0

#endif /* __AL_HAL_BOOTSTRAP_MAPS_H__ */

/** @} end of Bootstrap group */

