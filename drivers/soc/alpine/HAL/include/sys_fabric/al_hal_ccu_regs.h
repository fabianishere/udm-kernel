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
 * @defgroup groupccu abstraction layer
 *  @{
 * @file   al_hal_ccu_regs.h
 *
 * @brief Header file for the CCU HAL driver
 */

#ifndef __AL_HAL_CCU_REGS_H__
#define __AL_HAL_CCU_REGS_H__

#include "al_hal_common.h"

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */


#if (AL_DEV_ID < AL_DEV_ID_ALPINE_V3)
#define AL_CCU_PMU_REG_COUNT	4
#else
#define AL_CCU_PMU_REG_COUNT	8
#endif

#define AL_CCU_PMU_SRC_OFFSET 5

#define AL_CCU_NUM_SLAVES		7
#define AL_CCU_SLAVE_IDX_CLUSTER(idx)	(3 + (idx))

struct al_ccu_pmu_regs {
	uint32_t sel;                            /*[0x0]*/
	uint32_t counter;                        /*[0x4]*/
	uint32_t ctrl;                           /*[0x8]*/
	uint32_t overflow;                       /*[0xC]*/
#if (AL_DEV_ID < AL_DEV_ID_ALPINE_V3)
	uint32_t resv_0[1020];                   /*[0x10]*/
#else
	uint8_t resv_0[0x10000 - 0x10];          /*[0x10]*/
#endif
};

struct al_ccu_slave_regs {
	/* [0x0] The Snoop_Control_Register controls the issuing of snoop and DVM requests on
	 * the slave interface */
	uint32_t snoop_control_register;
	uint32_t resvd[1023];
};

struct al_ccu_regs {
	/* [0x0] Additional control register that provides a fail-safe override for some CCU
	 * functions, if these cause problems that you cannot otherwise work around. */
	uint32_t control_override_register;
	/* [0x4] Speculation control register */
	uint32_t speculation_ctrl_register_v1_v2;
	/* [0x8] The Secure_Access_Register controls whether only Secure transactions can read and
	 * program the CCU registers. */
	uint32_t secure_access_register;
	uint32_t rsrvd_0[61];
	/* [0x100] The Performance_Monitor_Control_Register controls the Performance Monitor Unit */
	uint32_t pmcr;
	uint32_t resvd_1[951];
	/* [0xfe0] The Peripheral_ID0 register is a standard JEP106 Peripheral IDentification
	 * register that contains the lower 8 bits of the CCU part number. */
	uint32_t peripheral_id0;
	uint32_t resvd_2[7];
	/* [0x1000] */
	struct al_ccu_slave_regs slaves[AL_CCU_NUM_SLAVES];
	uint32_t resvd_3[2048];
#if (AL_DEV_ID >= AL_DEV_ID_ALPINE_V3)
	uint8_t resvd_4[0x10000 - 0xA000];
	/* [0x10000] */
#else
	/* [0xA000] */
#endif
	struct al_ccu_pmu_regs pmu_cntrs[AL_CCU_PMU_REG_COUNT];
};


/**** Control_Override_Register register ****/
/* Snoop Disable: */
#define CCU_CTRL_OVRD_SNOOP_DISABLE				(1 << 0)
/* Disables propagation of all DVM messages. */
#define CCU_CTRL_OVRD_DVM_MESSAGE_DISABLE			(1 << 1)
/* Disables the snoop filter. */
#define CCU_CTRL_OVRD_DISABLE_SNOOP_FILTER			(1 << 2)
/* Enables dual layer snoop data. */
#define CCU_CTRL_CD_LAYER2_ACE_ENABLE				(1 << 4)
#define CCU_CTRL_OVRD_M0_BARRIER_TERM_DIS			(1 << 27)
#define CCU_CTRL_OVRD_WR_BUFF_EN				(1 << 25)
#define CCU_CTRL_OVRD_WR_BUFF_BYPASS_N				(1 << 24)

/**** Secure_Access_Register register ****/
/* Non-secure register access override. */
#define CCU_SECURE_ACCESS_REGISTER_SECURE_ACCESS_CONTROL	(1 << 0)

/**** Performance Monitor Control Register ****/
#define CCU_PMU_PMCR_ENABLE					(1 << 0)
#define CCU_PMU_PMCR_RESET					(1 << 1)

/**** Peripheral_ID0 register ****/
/* The lower 8 bits of the product part number. */
#define CCU_PERIPHERAL_ID0_PART_NUMBER_P0_MASK 0x000000FF
#define CCU_PERIPHERAL_ID0_PART_NUMBER_P0_SHIFT 0
#define CCU_PERIPHERAL_ID0_PART_NUMBER_P0_VAL_V1_V2		0x20
#define CCU_PERIPHERAL_ID0_PART_NUMBER_P0_VAL_V3		0x22

/**** Snoop_Control_Register_SX register ****/
/* Enable issuing of snoop requests from slave interface SX. */
#define CCU_SNOOP_CONTROL_REGISTER_SX_ENABLE_SNOOP_REQUESTS	(1 << 0)
/* Enable issuing of DVM message requests from slave interface SX. */
#define CCU_SNOOP_CONTROL_REGISTER_SX_ENABLE_DVMS		(1 << 1)


/**** PMU Count Control Register ****/
#define CCU_PMU_COUNT_CTRL_ENABLE				(1 << 0)


/* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */
/** @} end of DDR group */

#endif
