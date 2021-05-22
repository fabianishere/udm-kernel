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
 * @defgroup group_watchdog Watchdog
 * @ingroup group_sys_services
 *  @{
 * @section overview Overview
 * The watchdog unit consists of 4 (AL_WD_NUM) watchdog timers in charge of
 * resetting the system in the event of a software failure.
 *
 * The watchdog timer is loaded with a predefined value from which countdown
 * begins and the counter is decremented at each positive edge clock cycle.
 * The watchdog timer generates an interrupt when counting begins, giving the
 * software a chance to report its liveliness by clearing the interrupt.
 * If countdown ends (The counter reaches 0) and the interrupt is still set,
 * the watchdog timer assumes a software failure and the reset line is asserted.
 * Please note that an interrupt is not generated the first time the timer is
 * activated- an interrupt will only be generated starting from the second
 * countdown. This also means that the reset line will not be asserted on the
 * first countdown (As the interrupt is not set).
 * @code
 * struct al_watchdog wd;
 *
 * int main() {
 *	int ret;
 *	ret = al_watchdog_init(&wd, (void __iomem *)AL_WD_BASE(0), 0);
 *	if (ret != 0) {
 *	 printf("Init failed\n");
 *	 return ret;
 *	}
 *
 *	// Set watchdog counter load value
 *	al_watchdog_load_set(&wd, 50000000);
 *
 *	// Clear interrupt, if pending
 *	al_watchdog_int_clear(&wd);
 *
 *	// Enable interrupt generation and reset assertion
 *	al_watchdog_int_enable(&wd, AL_TRUE);
 *	al_watchdog_reset_enable(&wd, AL_TRUE);
 * }
 *
 * void watchdog_isr() {
 *	// This is the ISR for the watchdog interrupt
 *	// Clearing the interrupt status when the interrupt is generated
 *  // signals to the watchdog unit that the SW is alive and will prevent the
 *	// unit from asserting the reset signal when the countdown is complete
 *  al_watchdog_int_clear(&wd);
 * }
 * @endcode
 * @file   al_hal_watchdog.h
 *
 */

#ifndef __AL_HAL_WATCHDOG_H__
#define __AL_HAL_WATCHDOG_H__

#include "al_hal_common.h"

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

/* Struct definitions */
struct al_watchdog {
	void __iomem *regs;
	uint8_t wd_id;
};

/* API definitions */

/**
 * Initialize the watchdog
 *
 * @param	watchdog
 *		The watchdog instance we will be working with
 *
 * @param	wd_regs_base
 *		The base address of the watchdog config registers
 *
 * @param	wd_id
 *		The watchdog id
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred
 */
int al_watchdog_init(
		struct al_watchdog *watchdog,
		void __iomem *wd_regs_base,
		uint8_t wd_id);

/**
 * Loads the watchdog timer with a new countdown value
 *
 * @param	watchdog
 *		The watchdog instance we are working with
 *
 * @param	val
 *		New value
 *
 */
void al_watchdog_load_set(
		struct al_watchdog *watchdog,
		uint32_t val);

/**
 * Returns the watchdog timer's countdown value
 *
 * @param	watchdog
 *		The watchdog instance we are working with
 *
 * @return	The watchdog countdown value
 */
uint32_t al_watchdog_load_get(
		struct al_watchdog *watchdog);

/**
 * Enables/Disables interrupt generation
 *
 * @param	watchdog
 *		The watchdog instance we are working with
 *
 * @param	int_en
 *		AL_TRUE to enable interrupt generation, AL_FALSE
 *		to disable.
 *		Default: AL_FALSE.
 *
 */
void al_watchdog_int_enable(
		struct al_watchdog *watchdog,
		al_bool int_en);

/**
 * Enables/Disables reset assertion when watchdog countdown
 * is complete.
 *
 * @param	watchdog
 *		The watchdog instance we are working with
 *
 * @param	reset_en
 *		AL_TRUE to enable reset out, AL_FALSE
 *		to disable.
 *		Default: AL_FALSE.
 *
 */
void al_watchdog_reset_enable(
		struct al_watchdog *watchdog,
		al_bool reset_en);

/**
 * Clears the interrupt for this watchdog timer
 *
 * @param	watchdog
 *		The watchdog instance we are working with
 */
void al_watchdog_int_clear(
		struct al_watchdog *watchdog);

/**
 * Returns the watchdog timer's current value
 *
 * @param	watchdog
 *		The watchdog instance we are working with
 *
 * @return	The watchdog current value
 */
uint32_t al_watchdog_value_get(
		struct al_watchdog *watchdog);

/**
 * Get the raw interrupt state of this watchdog timer
 *
 * This is the watchdog interrupt status before applying the
 * interrupt mask.
 *
 * @param	watchdog
 *		The watchdog instance we are working with
 *
 * @return	The int status (1 if int was asserted, else 0)
 */
unsigned int al_watchdog_int_raw_get(
		struct al_watchdog *watchdog);

/**
 * Get the masked interrupt state of this watchdog timer
 *
 * This value is the AND of the raw interrupt status with
 * the interrupt enable bit from the ctrl register.
 *
 * @param	watchdog
 *		The watchdog instance we are working with
 *
 * @return	The masked int status (1 if int&mask was asserted, else 0)
 */
unsigned int al_watchdog_int_masked_get(
		struct al_watchdog *watchdog);

/* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */
/** @} end of watchdog group */
#endif
