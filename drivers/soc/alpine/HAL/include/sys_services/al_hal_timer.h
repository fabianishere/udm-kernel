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
 * @defgroup group_timer Timer
 * @ingroup group_sys_services
 *  @{
 * @section overview Overview
 * The Timer consists of two 32/16 bit down counters that can generate
 * interrupts on reaching zero.
 *
 * Each sub-timer can be configured to work in 3 modes:
 * -# Free run - after reaching zero the timer will be load to max value
 * -# Periodic - after reaching zero the timer will be load with the
 * original value
 * -# One-shot - the timer will be halted after reaching zero
 *
 * @code
 *	int main()
 *	{
 *		struct al_timer_subtimer sub_timer;
 *		int ret = 0;
 *		int interrupt_status = 0;
 *		// initialize the sub-timer
 *		ret = al_timer_init(&sub_timer, SUBTIMER_REGS_BASE_ADDR,
 *					TIMER1_ID, SUBTIMER1_ID);
 *		if (ret != 0) {
 *			printf("Initialize failed\n");
 *			return ret;
 *		}
 *
 *		// configure this sub-timer to work as periodic timer, 16 bit
 *		// size with scale of sys_clock / 16
 *		al_timer_config_set(&sub_timer, AL_TIMER_SIZE_16,
 *				AL_TIMER_MODE_PERIODIC, AL_TIMER_SCALE_16);
 *
 *		// set the value the sub timer will count down from
 *		al_timer_load_set(&sub_timer, TIME_PERIOD);
 *
 *		// enable interrupt for this sub-timer
 *		al_timer_int_enable(&sub_timer, AL_TRUE);
 *
 *		// enable the timer
 *		al_timer_enable(&sub_timer, AL_TRUE);
 *
 *		// get interrupt status
 *		interrupt_status = al_timer_int_raw_get(&sub_timer);
 *	}
 * @endcode
 * @file   al_hal_timer.h
 *
 * @brief Header file for the timer HAL
 */

#ifndef __AL_HAL_TIMER_H__
#define __AL_HAL_TIMER_H__

#include "al_hal_common.h"

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

/* Struct definitions */
struct al_timer_subtimer {
	void __iomem *regs;
	uint8_t timer_id;
	uint8_t sub_timer_id;
};

/* Enum definitions */
/* each sub-timer can be configured to work as 16/32 bit size */
enum al_timer_size {
	AL_TIMER_SIZE_16,
	AL_TIMER_SIZE_32
};

/* The modes a sub-timer can work on */
enum al_timer_mode {
	AL_TIMER_MODE_FREE_RUN,
	AL_TIMER_MODE_PERIODIC,
	AL_TIMER_MODE_ONE_SHOT
};

/* used to set the scale of a sub-timer clock */
enum al_timer_scale {
	AL_TIMER_SCALE_1,
	AL_TIMER_SCALE_16,
	AL_TIMER_SCALE_256
};

/* API definitions */

/**
 * Initialize the timer
 *
 * @param	sub_timer
 *		The sub timer instance we are working with
 *
 * @param	sub_timer_regs_base
 *		The base address of the subtimer configuration registers
 *
 * @param	timer_id
 *		The timer id could be
 *		 - [0..3] for ALPINE_V1 (supports 4 timers)
 *		 - [0..7] for ALPINE_V2 (supports 8 timers)
 *
 *
 * @param	sub_timer_id
 *		The sub timer id
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred
 */
int al_timer_init(
		struct al_timer_subtimer *sub_timer,
		void __iomem *sub_timer_regs_base,
		uint8_t timer_id,
		uint8_t sub_timer_id);

/**
 * Configure the specific sub-timer
 * Timer can work on 3 modes (free run / one shot /periodic)
 * Timer size could be 16/32 bit
 * Timer scale can be sys_clock OR sys_clock/16 OR sys_clock/256
 *
 * @param	sub_timer
 *		The sub timer instance we are working with
 *
 * @param	size
 *		the size of the timer (16 / 32 bit)
 *		default: 16 bit
 *
 * @param	mode
 *		the mode the timer should work on (periodic/one shot/free run)
 *		default: free run mode
 *
 * @param	scale
 *		the scale of the timer (CLCK / CLCK div 16 / CLCK div 256 )
 *		default: scale 1 (= sys clock)
 */
void al_timer_config_set(
		struct al_timer_subtimer *sub_timer,
		enum al_timer_size size,
		enum al_timer_mode mode,
		enum al_timer_scale scale);

/**
 * Set the value from which the counter is to decrement
 * this is also the value that reloaded on periodic mode
 * the current counter will be immediately reset to val after this call
 *
 * @param	sub_timer
 *		The sub timer instance we are working with
 *
 * @param	val
 *		The value to count down from
 *		default: 0
 */
void al_timer_load_set(
		struct al_timer_subtimer *sub_timer,
		uint32_t val);

/**
 * Set the background load
 *
 * This is the value used to reload the counter on periodic mode.
 * This call won't restart the current counter (will be change after the
 * counter will reach zero)
 *
 * @param	sub_timer
 *		The sub timer instance we are working with
 *
 * @param	val
 *		The background load to set
 */
void al_timer_bgload_set(
		struct al_timer_subtimer *sub_timer,
		uint32_t val);

/**
 * Enable / Disable interrupt for this subtimer
 *
 * @param	sub_timer
 *		The sub timer instance we are working with
 *
 * @param	int_en
 *		true to enable interrupts / false to disable interrupts
 *		default: interrupt enabled
 */
void al_timer_int_enable(
		struct al_timer_subtimer *sub_timer,
		al_bool int_en);

/**
 * Enable / Disable this subtimer
 *
 * @param	sub_timer
 *		The sub timer instance we are working with
 *
 * @param	en
 *		true to enable the timer / false to disable it
 *		default: timer disabled
 */
void al_timer_enable(
		struct al_timer_subtimer *sub_timer,
		al_bool en);

/**
 * Clear the interrupt for this timer
 *
 * @param	sub_timer
 *		The sub timer instance we are working with
 */
void al_timer_int_clear(
		struct al_timer_subtimer *sub_timer);

/**
 * Get the value from which the counter is to decrement
 *
 * @param	sub_timer
 *		The sub timer instance we are working with
 *
 * @return	the count down value
 */
uint32_t al_timer_load_get(
		struct al_timer_subtimer *sub_timer);

/**
 * Get the background value
 *
 * @param	sub_timer
 *		The sub timer instance we are working with
 *
 * @return	the background load
 */
uint32_t al_timer_bgload_get(
		struct al_timer_subtimer *sub_timer);

/**
 * Get the current value of the timer
 *
 * @param	sub_timer
 *		The sub timer instance we are working with
 *
 * @return	the counter current value
 */
uint32_t al_timer_value_get(
		struct al_timer_subtimer *sub_timer);

/**
 * Get the interrupt state of this timer before applying the interrupt mask.
 *
 * @param	sub_timer
 *		The sub timer instance we are working with
 *
 * @return	The int status (1 if int was set, else 0)
 */
unsigned int al_timer_int_raw_get(
		struct al_timer_subtimer *sub_timer);

/**
 * Get the masked interrupt status of this timer
 *
 * This value is the AND of the raw interrupt status with
 * the interrupt enable bit from the ctrl register.
 *
 * @param	sub_timer
 *		The sub timer instance we are working with
 *
 * @return	The masked interrupt status (1 if int & msk were set, else 0)
 */
unsigned int al_timer_int_masked_get(
		struct al_timer_subtimer *sub_timer);

/* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */
/** @} end of timer group */
#endif
