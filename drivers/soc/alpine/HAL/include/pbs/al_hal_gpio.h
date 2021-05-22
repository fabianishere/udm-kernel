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
 * @defgroup group_gpio	GPIO
 * @ingroup group_pbs
 *  @{
 *
 * The GPIO provides eight programmable pins, that can be configured as an
 * inputs or outputs.
 * The GPIO pins can be configured as an interrupt source as well, using the
 * provided API functions. Each pin can generate an interrupt according to its
 * level, or transitional edge.
 *
 * Common operation example:
 * @code
 *      int main()
 *      {
 *              struct al_gpio_interface gpio_if;
 *              int interrupt_val = 0, ret = 0;
 *
 *		// initialize the gpio interface
 *		ret = al_gpio_init(&gpio_if, GPIO_REGS_BASE_ADDRESS);
 *		if (ret != 0) {
 *			printf("Initialization failed\n");
 *			return ret;
 *		}
 *
 *		// config pin#0 as an output with rising edge sensitivity
 *		al_gpio_dir_set(&gpio_if, 0, AL_GPIO_DIR_OUT);
 *		al_gpio_int_event_set(&gpio_if, 0,
 *					AL_GPIO_INT_EVENT_RISING_EDGE);
 *		al_gpio_int_enable_set(&gpio_if, 0, AL_TRUE);
 *
 *		// config pin#2 as an input with low level sensitivity
 *		al_gpio_dir_set(&gpio_if, 2, AL_GPIO_DIR_IN);
 *		al_gpio_int_event_set(&gpio_if, 2,
 *					AL_GPIO_INT_EVENT_LOW_LEVEL);
 *		al_gpio_int_enable_set(&gpio_if, 2, AL_TRUE);
 *
 *		// assert pin#0
 *		al_gpio_set(&gpio_if, 0, 0x1);
 *
 *		// get pin#2 interrupt status
 *		al_gpio_int_get_masked(&gpio_if, 2, &interrupt_val);
 *
 *		return 0;
 *      }
 * @endcode
 * 
 * @file   al_hal_gpio.h
 * @brief Header file for the GPIO HAL driver
 *
 */

#ifndef __AL_HAL_GPIO_H__
#define __AL_HAL_GPIO_H__

#include "al_hal_common.h"

#define AL_GPIO_DEVICE_PIN_AMOUNT		8

#ifndef AL_GPIO_ID_TO_DEVICE
#define AL_GPIO_ID_TO_DEVICE(id)		((id) / AL_GPIO_DEVICE_PIN_AMOUNT)
#endif
#ifndef AL_GPIO_ID_TO_PIN
#define AL_GPIO_ID_TO_PIN(id)			((id) % AL_GPIO_DEVICE_PIN_AMOUNT)
#endif

/* Struct definitions */
struct al_gpio_interface {
	void __iomem *regs;
};

/* Enum definitions */
enum al_gpio_dir_t {
	AL_GPIO_DIR_IN,
	AL_GPIO_DIR_OUT
};

enum al_gpio_int_event_t {
	AL_GPIO_INT_EVENT_FALLING_EDGE,
	AL_GPIO_INT_EVENT_RISING_EDGE,
	AL_GPIO_INT_EVENT_FALLING_OR_RISING_EDGE,
	AL_GPIO_INT_EVENT_LOW_LEVEL,
	AL_GPIO_INT_EVENT_HIGH_LEVEL
};

/* API definitions */

/**
 * Initialize the GPIO controller
 *
 * @param	gpio_if
 *		The gpio instance we are working with
 * @param	gpio_regs_base
 *		The base address of the GPIO configuration registers
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred
 */
int al_gpio_init(
		struct al_gpio_interface *gpio_if,
		void __iomem *gpio_regs_base);

/**
 * Set the GPIO pin direction value
 *
 * @param	gpio_if
 *		The gpio instance we are working with
 * @param	id
 *		The id of the GPIO pin
 * @param	dir
 *		The direction to set
 */
void al_gpio_dir_set(
		struct al_gpio_interface *gpio_if,
		unsigned int id,
		enum al_gpio_dir_t dir);

/**
 * Set the GPIO event for triggering interrupt
 *
 * @param	gpio_if
 *		The gpio instance we are working with
 * @param	id
 *		The id of the pin we would like to config
 * @param	ievent
 *		Config the trigger event
 */
void al_gpio_int_event_set(
		struct al_gpio_interface *gpio_if,
		unsigned int id,
		enum al_gpio_int_event_t ievent);

/**
 * Disables/Enables interrupt propagation from specific a GPIO pin to the CPU
 *
 * Masks the interrupt triggers. Bits set to HIGH allow
 * their corresponding pins to trigger their individual interrupts.
 *
 * @param	gpio_if
 *		The gpio instance we are working with
 * @param	id
 *		The id of the GPIO pin
 * @param	enable
 *		true to enable interrupts, false to disable interrupts
 */

void al_gpio_int_enable_set(
		struct al_gpio_interface *gpio_if,
		unsigned int id,
		al_bool enable);

/**
 * Get the current data bit from the GPIO pin
 *
 * If the pin is configured as INPUT, the return value will be corresponding
 * bit current value.
 * If the pin is configured as OUTPUT, the return value will be the last
 * written value.
 *
 * @param	gpio_if
 *		The gpio instance we are working with
 * @param	id
 *		The id of the GPIO pin
 *
 * @return	The data bit value
 */
int al_gpio_get(
		struct al_gpio_interface *gpio_if,
		unsigned int id);

/**
 * Set the GPIO pin output value
 *
 * @param	gpio_if
 *		The gpio instance we are working with
 * @param	id
 *		The id of the GPIO pin
 * @param	val
 *		The value to set
 */
void al_gpio_set(
		struct al_gpio_interface *gpio_if,
		unsigned int id,
		int val);

/**
 * Clears a pending interrupt for a specific GPIO pin
 *
 * Setting this register will reset all of the current interrupts.
 *
 * @param	gpio_if
 *		The gpio instance we are working with
 * @param	id
 *		The id of the GPIO pin
 */
void al_gpio_int_clear(
		struct al_gpio_interface *gpio_if,
		unsigned int id);

/**
 * Get the RAW interrupt state
 *
 * Raw interrupt state is the value of the Interrupt State before applying the
 * interrupt mask. Bit read equals 1 means that an interrupt trigger detected
 * PRIOR to masking.
 *
 * @param	gpio_if
 *		The gpio instance we are working with
 * @param	id
 *		The id of the GPIO pin
 *
 * @return	The interrupt bit value
 */
int al_gpio_int_get_raw(
		struct al_gpio_interface *gpio_if,
		unsigned int id);

/**
 * Get the MASKED interrupt state
 *
 * Masked interrupt state is the value of the Interrupt State after applying
 * the interrupt mask. Bit read equals 1 means that an interrupt trigger
 * has been detected.
 *
 * @param	gpio_if
 *		The gpio instance we are working with
 * @param	id
 *		The id of the GPIO pin
 *
 * @return	The interrupt bit value
 */
int al_gpio_int_get_masked(
		struct al_gpio_interface *gpio_if,
		unsigned int id);

#endif

/** @} end of GPIO group */

