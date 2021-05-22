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
 *  @{
 * @file   al_hal_sgpo_pergroup_regs.h
 *
 * @brief ... registers
 *
 */

#ifndef __AL_HAL_SGPO_PerGroup_REGS_H__
#define __AL_HAL_SGPO_PerGroup_REGS_H__

#include "al_hal_plat_types.h"

#ifdef __cplusplus
extern "C" {
#endif
/*
* Unit Registers
*/



struct al_sgpo_pergroup_conf {
	/* [0x0] blink and stretch rates */
	uint32_t rates;
	/* [0x4] mode select */
	uint32_t mode;
	/* [0x8] active low/high */
	uint32_t invert;
	/* [0xc] enable blink */
	uint32_t blink;
	/* [0x10] enable stretch */
	uint32_t stretch;
	uint32_t rsrvd[11];
};
struct al_sgpo_pergroup_conf_vec {
	/* [0x0] enable stretch */
	uint32_t val;
};

struct al_sgpo_pergroup_regs {
	struct al_sgpo_pergroup_conf conf;                      /* [0x0] */
	uint32_t rsrvd_0[240];
	struct al_sgpo_pergroup_conf_vec conf_vec[256];         /* [0x400] */
	uint32_t rsrvd_1[512];
};


/*
* Registers Fields
*/


/**** rates register ****/
/* Serial blink low (1'b0) length [2^(N+20)] x IO Fabric cycles */
#define SGPO_PERGROUP_CONF_RATES_BLINK_LOW_MASK 0x0000000F
#define SGPO_PERGROUP_CONF_RATES_BLINK_LOW_SHIFT 0
/* Serial blink high (1'b1) length [2^(N+20)] x IO Fabric cycles */
#define SGPO_PERGROUP_CONF_RATES_BLINK_HIGH_MASK 0x000000F0
#define SGPO_PERGROUP_CONF_RATES_BLINK_HIGH_SHIFT 4
/* Serial stretch length [2^(N+20)] x IO Fabric cycles */
#define SGPO_PERGROUP_CONF_RATES_STRETCH_MASK 0x00000F00
#define SGPO_PERGROUP_CONF_RATES_STRETCH_SHIFT 8

/**** mode register ****/
/*
 * 2-bit for GPO, selects the mode for that output. See RM for further details
 */
#define SGPO_PERGROUP_CONF_MODE_SEL_MASK 0x0000FFFF
#define SGPO_PERGROUP_CONF_MODE_SEL_SHIFT 0

/**** invert register ****/
/* 1-bit for GPO, defines if the GPO value is used as-is or inverted. */
#define SGPO_PERGROUP_CONF_INVERT_GPO_MASK 0x000000FF
#define SGPO_PERGROUP_CONF_INVERT_GPO_SHIFT 0

/**** blink register ****/
/* 1-bit for GPO, defines if the GPO value is used as-is or inverted */
#define SGPO_PERGROUP_CONF_BLINK_EN_MASK 0x000000FF
#define SGPO_PERGROUP_CONF_BLINK_EN_SHIFT 0

/**** stretch register ****/
/* 1-bit for GPO, defines if the GPO value is used as-is or stretched */
#define SGPO_PERGROUP_CONF_STRETCH_EN_MASK 0x000000FF
#define SGPO_PERGROUP_CONF_STRETCH_EN_SHIFT 0

/**** val register ****/
/*
 * 1-bit for GPO, defines its value when selected by the relevant serial_mode is
 * selected.
 */
#define SGPO_PERGROUP_CONF_VEC_VAL_GPO_MASK 0x000000FF
#define SGPO_PERGROUP_CONF_VEC_VAL_GPO_SHIFT 0

#ifdef __cplusplus
}
#endif

#endif /* __AL_HAL_SGPO_PerGroup_REGS_H__ */

/** @} end of ... group */
