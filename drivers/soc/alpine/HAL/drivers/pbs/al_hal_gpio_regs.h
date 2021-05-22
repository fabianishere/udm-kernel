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

#ifndef __AL_HAL_GPIO_REGS_H__
#define __AL_HAL_GPIO_REGS_H__

/**
 * The following struct describes the gpio device's configuration registers
 *
 * Important notes:
 *	- All of the registers are 8-bit, and each bit represents a GPIO pin.
 *	  Setting/clearing a bit will affect the corresponding GPIO register,
 *	  e.g. writing 0x18 to gpiodir will config pin#3 and pin#4 as outputs,
 *	  and config the rest of the pins are inputs.
 *	- The gpiodata register is 8-bit wide, yet have a large addressing
 *	  space. The reason for it is operational - by using a large address
 *	  space, the user can control each of the data bits seperately. Using
 *	  address[9:2] bits, the user can set/clear any combination of the 8
 *	  GPIO pins with a single action.
 *	  e.g. writing 0xF0 to address 0x44 will clear bit#0, set bit#4, and
 *	  leave the rest of the data bits unchanged. Bit example below:
 *	  Original gpiodata state:		00000000 (0x00)
 *	  Data to write:			11110000 (0xF0)
 *	  Address mask (bits [9:2] of address):	00010001 (0x44 >> 2 == 0x11)
 *	  New gpiodata state:			00010000 (line2 & line3)
 */

#include "al_hal_common.h"

struct al_gpio_regs {
	/* 0x0 */
	uint32_t gpiodata[0x100];	/* read/write data */
	/* 0x400 */
	uint32_t gpiodir;	/* direction - in/out */
	uint32_t gpiois;	/* interrupt sense - edge/level */
	uint32_t gpioibe;	/* toggle both-edges interrupt */
	uint32_t gpioiev;	/* interrupt event - (low/fall)/(high/rise) */
	/* 0x410 */
	uint32_t gpioie;	/* interrupt enable - mask/unmask interrupt */
	uint32_t gpioris;	/* raw interrupt status (READ ONLY) -
				   interrupt status regardless of masking */
	uint32_t gpiomis;	/* interrupt status (READ ONLY) - interrupt
				   status after applying the mask */
	uint32_t gpioic;	/* interrupt clear (WRITE ONLY) - clears
				   interrupt from the corresponding edge
				   detection register */
};

/**
 * The following bit fields correspond to the available values of the GPIO
 * registers.
 *
 * Note that the current version support a SINGLE bit change only.
 */

/* Because the data array has a 4-byte granularity, incrementing it by 1 is
 * the same as incrementing the address by 4, which is coherent with the
 * gpiodata addressing space configuration */
#define AL_GPIO_DATA_ARRAY_ID_TO_IDX(id)	(1 << id)

#define AL_GPIO_DATA_BIT_CLEAR			0x0
#define AL_GPIO_DATA_BIT_SET			0x1

#define AL_GPIO_DIRECTION_INPUT			0x0
#define AL_GPIO_DIRECTION_OUTPUT		0x1

#define AL_GPIO_INT_SENSE_EDGE			0x0
#define AL_GPIO_INT_SENSE_LEVEL			0x1

#define AL_GPIO_INT_BOTH_EDGES_INACTIVE		0x0
#define AL_GPIO_INT_BOTH_EDGES_ACTIVE		0x1

#define AL_GPIO_INT_EVENT_LOW_OR_FALL		0x0
#define AL_GPIO_INT_EVENT_HIGH_OR_RISE		0x1

#define AL_GPIO_INT_ENABLE_CLEAR		0x0
#define AL_GPIO_INT_ENABLE_SET			0x1

#define AL_GPIO_INT_RAW_STATUS(i)		AL_BIT(i)

#define AL_GPIO_INT_MASKED_STATUS(i)		AL_BIT(i)

#define AL_GPIO_INT_CLEAR(i)			AL_BIT(i)



#endif

