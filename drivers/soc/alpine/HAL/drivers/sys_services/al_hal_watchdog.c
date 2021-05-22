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
 * @file   al_hal_watchdog.c
 *
 * @brief Watchdog HAL driver
 *
 */

#include "al_hal_watchdog.h"
#include "al_hal_watchdog_regs.h"

int al_watchdog_init(
		struct al_watchdog *watchdog,
		void __iomem *wd_regs_base,
		uint8_t wd_id)
{
	al_assert(watchdog != NULL);
	al_assert(wd_regs_base != NULL);

	watchdog->regs = wd_regs_base;
	watchdog->wd_id = wd_id;

	return 0;
}

void al_watchdog_load_set(
		struct al_watchdog *watchdog,
		uint32_t val)
{
	struct al_watchdog_regs *regs;

	al_assert(watchdog != NULL);

	regs = (struct al_watchdog_regs *)watchdog->regs;

	al_reg_write32(&regs->load, val);
}

uint32_t al_watchdog_load_get(
		struct al_watchdog *watchdog)
{
	struct al_watchdog_regs *regs;

	al_assert(watchdog != NULL);

	regs = (struct al_watchdog_regs *)watchdog->regs;

	return al_reg_read32(&regs->load);
}

void al_watchdog_int_enable(
		struct al_watchdog *watchdog,
		al_bool int_en)
{
	struct al_watchdog_regs *regs;
	uint32_t val;

	al_assert(watchdog != NULL);

	regs = (struct al_watchdog_regs *)watchdog->regs;

	val = (int_en == AL_TRUE) ?
			AL_WATCHDOG_CTRL_INT_ENABLE :
			AL_WATCHDOG_CTRL_INT_DISABLE;

	al_reg_write32_masked(&regs->control,
		AL_WATCHDOG_CTRL_INT_ENABLE_MASK, val);
}

void al_watchdog_reset_enable(
		struct al_watchdog *watchdog,
		al_bool reset_en)
{
	struct al_watchdog_regs *regs;
	uint32_t val;

	al_assert(watchdog != NULL);

	regs = (struct al_watchdog_regs *)watchdog->regs;

	val = (reset_en == AL_TRUE) ?
			AL_WATCHDOG_CTRL_RESET_ENABLE :
			AL_WATCHDOG_CTRL_RESET_DISABLE;

	al_reg_write32_masked(&regs->control,
		AL_WATCHDOG_CTRL_RESET_ENABLE_MASK, val);
}

void al_watchdog_int_clear(
		struct al_watchdog *watchdog)
{
	struct al_watchdog_regs *regs;

	al_assert(watchdog != NULL);

	regs = (struct al_watchdog_regs *)watchdog->regs;

	al_reg_write32(&regs->intclr, 1);
}

uint32_t al_watchdog_value_get(
		struct al_watchdog *watchdog)
{
	struct al_watchdog_regs *regs;

	al_assert(watchdog != NULL);

	regs = (struct al_watchdog_regs *)watchdog->regs;

	return al_reg_read32(&regs->value);
}

unsigned int al_watchdog_int_raw_get(
		struct al_watchdog *watchdog)
{
	struct al_watchdog_regs *regs;
	uint32_t ris;

	al_assert(watchdog != NULL);

	regs = (struct al_watchdog_regs *)watchdog->regs;
	ris = al_reg_read32(&regs->ris);

	return AL_REG_FIELD_GET(ris, AL_WATCHDOG_RAW_MASK,
		AL_WATCHDOG_RAW_SHIFT);
}

unsigned int al_watchdog_int_masked_get(
		struct al_watchdog *watchdog)
{
	struct al_watchdog_regs *regs;
	uint32_t mis;

	al_assert(watchdog != NULL);

	regs = (struct al_watchdog_regs *)watchdog->regs;
	mis = al_reg_read32(&regs->mis);

	return AL_REG_FIELD_GET(mis, AL_WATCHDOG_MASKED_MASK,
					AL_WATCHDOG_MASKED_SHIFT);
}

/** @} end of Watchdog group */
