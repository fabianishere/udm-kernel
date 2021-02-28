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

#include "al_hal_timer.h"
#include "al_hal_timer_regs.h"

static inline void al_timer_mode_set(
		enum al_timer_mode mode,
		uint32_t *clear_mask,
		uint32_t *set_mask)
{
	switch (mode) {
	case AL_TIMER_MODE_ONE_SHOT:
		*clear_mask |= AL_TIMER_CTRL_ONE_SHOT_MASK;
		*set_mask |= AL_TIMER_CTRL_ONE_SHOT_ON;
		break;
	case AL_TIMER_MODE_PERIODIC:
		*clear_mask |= (AL_TIMER_CTRL_MODE_MASK |
				AL_TIMER_CTRL_ONE_SHOT_MASK);
		*set_mask |= (AL_TIMER_CTRL_MODE_PERIODIC |
				AL_TIMER_CTRL_ONE_SHOT_OFF);
		break;
	case AL_TIMER_MODE_FREE_RUN:
		*clear_mask |= (AL_TIMER_CTRL_MODE_MASK |
				AL_TIMER_CTRL_ONE_SHOT_MASK);
		*set_mask |= (AL_TIMER_CTRL_MODE_FREE_RUN |
				AL_TIMER_CTRL_ONE_SHOT_OFF);
		break;
	}
}

static inline void al_timer_size_set(
		enum al_timer_size size,
		uint32_t *clear_mask,
		uint32_t *set_mask)
{
	*clear_mask |= AL_TIMER_CTRL_SIZE_MASK;

	if (size == AL_TIMER_SIZE_32)
		*set_mask |= AL_TIMER_CTRL_SIZE_32;
	else
		*set_mask |= AL_TIMER_CTRL_SIZE_16;
}

static inline void al_timer_scale_set(
		enum al_timer_scale scale,
		uint32_t *clear_mask,
		uint32_t *set_mask)
{
	*clear_mask |= AL_TIMER_CTRL_PRESCALE_MASK;

	if (scale == AL_TIMER_SCALE_1)
		*set_mask |= AL_TIMER_CTRL_PRESCALE_1;
	else if (scale == AL_TIMER_SCALE_16)
		*set_mask |= AL_TIMER_CTRL_PRESCALE_16;
	else
		*set_mask |= AL_TIMER_CTRL_PRESCALE_256;
}

int al_timer_init(
		struct al_timer_subtimer *sub_timer,
		void __iomem *timer_regs_base,
		uint8_t timer_id,
		uint8_t sub_timer_id)
{
	al_assert(sub_timer != NULL);
	al_assert(timer_regs_base != NULL);

	sub_timer->regs = timer_regs_base;
	sub_timer->timer_id = timer_id;
	sub_timer->sub_timer_id = sub_timer_id;

	return 0;
}

void al_timer_config_set(
		struct al_timer_subtimer *sub_timer,
		enum al_timer_size size,
		enum al_timer_mode mode,
		enum al_timer_scale scale)
{
	struct al_timer_regs *regs;
	uint32_t control_reg;
	uint32_t clear_mask = 0;
	uint32_t set_mask = 0;

	al_assert(sub_timer != NULL);

	regs = (struct al_timer_regs *)sub_timer->regs;

	control_reg = al_reg_read32(&regs->control);

	al_timer_mode_set(mode, &clear_mask, &set_mask);
	al_timer_size_set(size, &clear_mask, &set_mask);
	al_timer_scale_set(scale, &clear_mask, &set_mask);

	AL_REG_CLEAR_AND_SET(control_reg, clear_mask, set_mask);

	al_reg_write32(&regs->control, control_reg);
}

void al_timer_load_set(
		struct al_timer_subtimer *sub_timer,
		uint32_t val)
{
	struct al_timer_regs *regs;

	al_assert(sub_timer != NULL);

	regs = (struct al_timer_regs *)sub_timer->regs;

	al_reg_write32(&regs->load, val);
}

void al_timer_bgload_set(
		struct al_timer_subtimer *sub_timer,
		uint32_t val)
{
	struct al_timer_regs *regs;

	al_assert(sub_timer != NULL);

	regs = (struct al_timer_regs *)sub_timer->regs;

	al_reg_write32(&regs->bg_load, val);
}


void al_timer_int_enable(
		struct al_timer_subtimer *sub_timer,
		al_bool int_en)
{
	struct al_timer_regs *regs;
	uint32_t control_reg;
	uint32_t val;

	al_assert(sub_timer != NULL);

	regs = (struct al_timer_regs *)sub_timer->regs;
	control_reg = al_reg_read32(&regs->control);

	val = (int_en == AL_TRUE) ?
			AL_TIMER_CTRL_INT_ENABLE :
			AL_TIMER_CTRL_INT_DISABLE;

	AL_REG_CLEAR_AND_SET(control_reg, AL_TIMER_CTRL_INT_ENABLE_MASK, val);

	al_reg_write32(&regs->control, control_reg);
}

void al_timer_enable(
		struct al_timer_subtimer *sub_timer,
		al_bool en)
{
	struct al_timer_regs *regs;
	uint32_t control_reg;
	uint32_t val;

	al_assert(sub_timer != NULL);

	regs = (struct al_timer_regs *)sub_timer->regs;
	control_reg = al_reg_read32(&regs->control);

	val = (en == AL_TRUE) ?
			AL_TIMER_CTRL_TIMER_ENABLE :
			AL_TIMER_CTRL_TIMER_DISABLE;

	AL_REG_CLEAR_AND_SET(control_reg, AL_TIMER_CTRL_TIMER_ENABLE_MASK, val);

	al_reg_write32(&regs->control, control_reg);
}


void al_timer_int_clear(
		struct al_timer_subtimer *sub_timer)
{
	struct al_timer_regs *regs;

	al_assert(sub_timer != NULL);

	regs = (struct al_timer_regs *)sub_timer->regs;

	al_reg_write32(&regs->intclr, 1);
}


uint32_t al_timer_load_get(
		struct al_timer_subtimer *sub_timer)
{
	struct al_timer_regs *regs;

	al_assert(sub_timer != NULL);

	regs = (struct al_timer_regs *)sub_timer->regs;

	return al_reg_read32(&regs->load);
}

uint32_t al_timer_bgload_get(
		struct al_timer_subtimer *sub_timer)
{
	struct al_timer_regs *regs;

	al_assert(sub_timer != NULL);

	regs = (struct al_timer_regs *)sub_timer->regs;

	return al_reg_read32(&regs->bg_load);
}

uint32_t al_timer_value_get(
		struct al_timer_subtimer *sub_timer)
{
	struct al_timer_regs *regs;

	al_assert(sub_timer != NULL);

	regs = (struct al_timer_regs *)sub_timer->regs;

	return al_reg_read32(&regs->value);
}

unsigned int al_timer_int_raw_get(
		struct al_timer_subtimer *sub_timer)
{
	struct al_timer_regs *regs;
	uint32_t ris;

	al_assert(sub_timer != NULL);

	regs = (struct al_timer_regs *)sub_timer->regs;
	ris = al_reg_read32(&regs->ris);

	return AL_REG_FIELD_GET(ris, AL_TIMER_RAW_MASK, AL_TIMER_RAW_SHIFT);
}

unsigned int al_timer_int_masked_get(
		struct al_timer_subtimer *sub_timer)
{
	struct al_timer_regs *regs;
	uint32_t mis;

	al_assert(sub_timer != NULL);

	regs = (struct al_timer_regs *)sub_timer->regs;
	mis = al_reg_read32(&regs->mis);

	return AL_REG_FIELD_GET(mis, AL_TIMER_MASKED_MASK,
					AL_TIMER_MASKED_SHIFT);
}

