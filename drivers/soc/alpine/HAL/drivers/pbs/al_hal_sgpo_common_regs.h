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
 * @file   al_hal_sgpo_common_regs.h
 *
 * @brief SGPO_Common registers
 *
 */

#ifndef __AL_HAL_SGPO_COMMON_REGS_H__
#define __AL_HAL_SGPO_COMMON_REGS_H__

#include "al_hal_plat_types.h"

#ifdef __cplusplus
extern "C" {
#endif
/*
* Unit Registers
*/



struct al_sgpo_common_conf {
	/* [0x0] control */
	uint32_t control;
	/* [0x4]  */
	uint32_t cpu_vdd;
	/* [0x8]  */
	uint32_t out_en_sgpo;
	/* [0xc]  */
	uint32_t out_en_cpu_vdd_ctrl;
	/* [0x10]  */
	uint32_t swap_leds;
	/* [0x14]  */
	uint32_t leds_inuse_high;
	/* [0x18]  */
	uint32_t leds_inuse_low;
	uint32_t rsrvd[9];
};
struct al_sgpo_common_debug {
	/* [0x0] FSM states */
	uint32_t fsm;
	/* [0x4] CNTR values */
	uint32_t cntr;
	uint32_t rsrvd[14];
};

struct al_sgpo_common_regs {
	struct al_sgpo_common_conf conf;                        /* [0x0] */
	struct al_sgpo_common_debug debug;                      /* [0x40] */
	uint32_t rsrvd[992];					/* [0x80] */
};


/*
* Registers Fields
*/


/**** control register ****/
/*
 * 00 = All 32b of GPIOs are Serialized in 32 x Serial_clock cycles. Serial_DS[0] is used.
 * Serial_DS[3:1] is unused.
 * 01 = Two interfaces of 16 x GPIOs are Serialized in 16 x Serial_clock cycles. Serial_DS[1:0] is
 * used. Serial_DS[3:2] is unused
 * 10 = Four interfaces of 8 x GPIOs are Serialized in 8 x Serial_clock cycles. All Serial_DS[3:0]
 * are used.
 * 11 = Reserved.
 */
#define SGPO_COMMON_CONF_CONTROL_GROUPS_MASK 0x00000003
#define SGPO_COMMON_CONF_CONTROL_GROUPS_SHIFT 0
/* emergency clear to the entire block */
#define SGPO_COMMON_CONF_CONTROL_SGI_CLEAR (1 << 2)
/* enable the sgi_block */
#define SGPO_COMMON_CONF_CONTROL_SGI_ENABLE (1 << 3)
/*
 * 0 = CPU_VDD_Control_out[2:0] outputs as is from the OTP registers
 * 1 = CPU_VDD_Control_out[2:0] are used as serial interface to output CPU_VDD_Control[4:0] signals
 */
#define SGPO_COMMON_CONF_CONTROL_CPU_VDD_SEL (1 << 5)
/* enable the serial cpu_vdd_ctrl block */
#define SGPO_COMMON_CONF_CONTROL_CPU_VDD_EN (1 << 7)
/*
 * determines SATA LED functionality:
 * 00 - Reserved
 * 01 - SATA_act
 * 10 - SATA_presence
 * 11 - SATA_act_presence
 */
#define SGPO_COMMON_CONF_CONTROL_SATA_LED_SEL_MASK 0x00000300
#define SGPO_COMMON_CONF_CONTROL_SATA_LED_SEL_SHIFT 8
/*
 * data to clk rise setup time, in powers of 2 (up to N=8 --> 2^8) of IO Fabric clock cycle
 * Default 4 --> setup_time = 2^4*2.666ns =~ 40nSec
 */
#define SGPO_COMMON_CONF_CONTROL_SDI_SETUP_MASK 0x0000F000
#define SGPO_COMMON_CONF_CONTROL_SDI_SETUP_SHIFT 12
/* serial clk freq = freq_sb / [2^N] */
#define SGPO_COMMON_CONF_CONTROL_CLK_FREQ_MASK 0x000F0000
#define SGPO_COMMON_CONF_CONTROL_CLK_FREQ_SHIFT 16
/*
 * debug feature - used mainly for verif purposes
 * this value scales counter base val.
 * by default (k=10), all timings are 2^(20+N).
 * If a smaller value is used k<10, then timing are modified to :  2^(2k+N)
 */
#define SGPO_COMMON_CONF_CONTROL_CNTR_SCALE_MASK 0x00F00000
#define SGPO_COMMON_CONF_CONTROL_CNTR_SCALE_SHIFT 20
#define SGPO_COMMON_CONF_CONTROL_CNTR_SCALE_VAL_NORMAL_BLINK	\
	(0xd << SGPO_COMMON_CONF_CONTROL_CNTR_SCALE_SHIFT)
#define SGPO_COMMON_CONF_CONTROL_CNTR_SCALE_VAL_FAST_BLINK	\
	(0xc << SGPO_COMMON_CONF_CONTROL_CNTR_SCALE_SHIFT)
#define SGPO_COMMON_CONF_CONTROL_CNTR_SCALE_VAL_7_8125MHZ_BLINK	\
	(0x6 << SGPO_COMMON_CONF_CONTROL_CNTR_SCALE_SHIFT)
#define SGPO_COMMON_CONF_CONTROL_CNTR_SCALE_VAL_500MHZ_BLINK \
	(0x0 << SGPO_COMMON_CONF_CONTROL_CNTR_SCALE_SHIFT)
/* serial update freq = freq_sb / [2^N] */
#define SGPO_COMMON_CONF_CONTROL_UPDATE_FREQ_MASK 0x1F000000
#define SGPO_COMMON_CONF_CONTROL_UPDATE_FREQ_SHIFT 24

/**** cpu_vdd register ****/
/* This bit is self-negated. When set, Serial CPU_VDD Update sequence take place */
#define SGPO_COMMON_CONF_CPU_VDD_UPDATE  (1 << 0)

/**** out_en_sgpo register ****/
/* output enable for sgpo shcp signal */
#define SGPO_COMMON_CONF_OUT_EN_SGPO_SHCP (1 << 0)
/* output enable for sgpo stcp signal */
#define SGPO_COMMON_CONF_OUT_EN_SGPO_STCP (1 << 1)
/* output enable for sgpo ds[3:0] signals */
#define SGPO_COMMON_CONF_OUT_EN_SGPO_DS_3_0_MASK 0x0000003C
#define SGPO_COMMON_CONF_OUT_EN_SGPO_DS_3_0_SHIFT 2

/**** out_en_cpu_vdd_ctrl register ****/
/* output enable for sgpo shcp signal */
#define SGPO_COMMON_CONF_OUT_EN_CPU_VDD_CTRL_SHCP (1 << 0)
/* output enable for sgpo stcp signal */
#define SGPO_COMMON_CONF_OUT_EN_CPU_VDD_CTRL_STCP (1 << 1)
/* output enable for sgpo ds signal */
#define SGPO_COMMON_CONF_OUT_EN_CPU_VDD_CTRL_DS (1 << 2)

/**** swap_leds register ****/
/* Defines if to swap the order of leds from 0:3 to 3:0 */
#define SGPO_COMMON_CONF_SWAP_LEDS_EN    (1 << 0)

/**** leds_inuse_high register ****/
/* Defines which leds are in use (47:32). If all bits are zero - uses classic mode. */
#define SGPO_COMMON_CONF_LEDS_INUSE_HIGH_INUSE_MASK 0x0000FFFF
#define SGPO_COMMON_CONF_LEDS_INUSE_HIGH_INUSE_SHIFT 0

/**** fsm register ****/
/* SGPO FSM state */
#define SGPO_COMMON_DEBUG_FSM_SGPO_STATE_MASK 0x0000000F
#define SGPO_COMMON_DEBUG_FSM_SGPO_STATE_SHIFT 0
/* cpu_vdd FSM state */
#define SGPO_COMMON_DEBUG_FSM_CPU_VDD_STATE_MASK 0x000000F0
#define SGPO_COMMON_DEBUG_FSM_CPU_VDD_STATE_SHIFT 4

/**** cntr register ****/
/* value of the internal free runninng counter, used for updating the external device */
#define SGPO_COMMON_DEBUG_CNTR_VAL_MASK  0x03FFFFFF
#define SGPO_COMMON_DEBUG_CNTR_VAL_SHIFT 0

#ifdef __cplusplus
}
#endif

#endif

/** @} */


