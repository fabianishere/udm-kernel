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
 * @defgroup group_gpio GPIO
 *  GPIO
 *  @{
 * @file   al_hal_gpio.c
 *
 * @brief  GPIO HAL driver
 *
 */

#include "al_hal_gpio.h"
#include "al_hal_gpio_regs.h"

/**
 * Reads register, config the value of a specific bit, and write the new value
 */
static inline void al_gpio_read_modify_write_bit(
		uint32_t *reg_addr,
		int bit_index,
		int bit_value)
{
	uint32_t write_value = al_reg_read32(reg_addr);
	AL_REG_BIT_VAL_SET(write_value, bit_index, bit_value);
	al_reg_write32(reg_addr, write_value);
}


int al_gpio_init(
		struct al_gpio_interface *gpio_if,
		void __iomem *gpio_regs_base)
{
	al_assert(gpio_if != NULL);

	gpio_if->regs = gpio_regs_base;

	return 0;
}

void al_gpio_dir_set(
		struct al_gpio_interface *gpio_if,
		unsigned int id,
		enum al_gpio_dir_t dir)
{
	struct al_gpio_regs *regs;
	int write_val;

	al_assert(gpio_if != NULL);
	al_assert(id < AL_GPIO_DEVICE_PIN_AMOUNT);

	regs = (struct al_gpio_regs __iomem *)gpio_if->regs;

	write_val = (AL_GPIO_DIR_IN == dir) ?
			AL_GPIO_DIRECTION_INPUT :
			AL_GPIO_DIRECTION_OUTPUT;

	al_gpio_read_modify_write_bit(&regs->gpiodir, id, write_val);
}

void al_gpio_int_event_set(
		struct al_gpio_interface *gpio_if,
		unsigned int id,
		enum al_gpio_int_event_t ievent)
{
	struct al_gpio_regs *regs;
	int write_val;

	al_assert(gpio_if != NULL);
	al_assert(id < AL_GPIO_DEVICE_PIN_AMOUNT);

	regs = (struct al_gpio_regs __iomem *)gpio_if->regs;

	/* If interrupt is edge-sensitive, config the GPIOIS reg accordingly */
	write_val = (AL_GPIO_INT_EVENT_FALLING_EDGE == ievent ||
			AL_GPIO_INT_EVENT_RISING_EDGE == ievent ||
			AL_GPIO_INT_EVENT_FALLING_OR_RISING_EDGE == ievent) ?
				AL_GPIO_INT_SENSE_EDGE :
				AL_GPIO_INT_SENSE_LEVEL;
	al_gpio_read_modify_write_bit(&regs->gpiois, id, write_val);

	write_val = (AL_GPIO_INT_EVENT_FALLING_EDGE == ievent ||
			AL_GPIO_INT_EVENT_LOW_LEVEL == ievent) ?
				AL_GPIO_INT_EVENT_LOW_OR_FALL :
				AL_GPIO_INT_EVENT_HIGH_OR_RISE;
	al_gpio_read_modify_write_bit(&regs->gpioiev, id, write_val);

	write_val = (AL_GPIO_INT_EVENT_FALLING_OR_RISING_EDGE == ievent) ?
				AL_GPIO_INT_BOTH_EDGES_ACTIVE :
				AL_GPIO_INT_BOTH_EDGES_INACTIVE;
	al_gpio_read_modify_write_bit(&regs->gpioibe, id, write_val);
}

void al_gpio_int_enable_set(
		struct al_gpio_interface *gpio_if,
		unsigned int id,
		al_bool enable)
{
	struct al_gpio_regs *regs;
	int write_val;

	al_assert(gpio_if != NULL);
	al_assert(id < AL_GPIO_DEVICE_PIN_AMOUNT);

	regs = (struct al_gpio_regs __iomem *)gpio_if->regs;

	write_val = (enable == AL_TRUE) ?
			AL_GPIO_INT_ENABLE_SET :
			AL_GPIO_INT_ENABLE_CLEAR;

	al_gpio_read_modify_write_bit(&regs->gpioie, id, write_val);
}

int al_gpio_get(
		struct al_gpio_interface *gpio_if,
		unsigned int id)
{
	struct al_gpio_regs *regs;
	int idx;

	al_assert(gpio_if != NULL);
	al_assert(id < AL_GPIO_DEVICE_PIN_AMOUNT);

	regs = (struct al_gpio_regs __iomem *)gpio_if->regs;

	idx = AL_GPIO_DATA_ARRAY_ID_TO_IDX(id);

	return (0 == al_reg_read32(&regs->gpiodata[idx])) ? 0 : 1;
}

void al_gpio_set(
		struct al_gpio_interface *gpio_if,
		unsigned int id,
		int val)
{
	struct al_gpio_regs *regs;
	int idx;

	al_assert(gpio_if != NULL);
	al_assert(id < AL_GPIO_DEVICE_PIN_AMOUNT);

	regs = (struct al_gpio_regs __iomem *)gpio_if->regs;

	idx = AL_GPIO_DATA_ARRAY_ID_TO_IDX(id);
	al_reg_write32(&regs->gpiodata[idx], val << id);
}

void al_gpio_int_clear(
		struct al_gpio_interface *gpio_if,
		unsigned int id)
{
	struct al_gpio_regs *regs;

	al_assert(gpio_if != NULL);
	al_assert(id < AL_GPIO_DEVICE_PIN_AMOUNT);

	regs = (struct al_gpio_regs __iomem *)gpio_if->regs;

	al_reg_write32(&regs->gpioic, AL_GPIO_INT_CLEAR(id));
}

int al_gpio_int_get_raw(
		struct al_gpio_interface *gpio_if,
		unsigned int id)
{
	struct al_gpio_regs *regs;

	al_assert(gpio_if != NULL);
	al_assert(id < AL_GPIO_DEVICE_PIN_AMOUNT);

	regs = (struct al_gpio_regs __iomem *)gpio_if->regs;

	return (0 == (al_reg_read32(&regs->gpioris) &
				(AL_GPIO_INT_RAW_STATUS(id)))) ?
						0 : 1;
}

int al_gpio_int_get_masked(
		struct al_gpio_interface *gpio_if,
		unsigned int id)
{
	struct al_gpio_regs *regs;

	al_assert(gpio_if != NULL);
	al_assert(id < AL_GPIO_DEVICE_PIN_AMOUNT);

	regs = (struct al_gpio_regs __iomem *)gpio_if->regs;

	return (0 == (al_reg_read32(&regs->gpiomis) &
				(AL_GPIO_INT_MASKED_STATUS(id)))) ?
						0 : 1;
}

/** @} end of GPIO group */

