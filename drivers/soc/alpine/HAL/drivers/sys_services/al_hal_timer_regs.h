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

#ifndef __AL_HAL_TIMER_REGS_H__
#define __AL_HAL_TIMER_REGS_H__

/**
 * The following struct describes the timer configuration registers
 */

#include "al_hal_common.h"
#include "al_hal_timer.h"

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

struct al_timer_regs {
	uint32_t load;		/* the value the counter count down from */
	uint32_t value;		/* current value - read only */
	uint32_t control;	/* timer configuration - mode/size/scale etc */
	uint32_t intclr;	/* every write to this reg will clear
				   the interrupt  */
	uint32_t ris;		/* raw interrupt status */
	uint32_t mis;		/* mask interrupt status */
	uint32_t bg_load;	/* background load - writing value to this
				   register will take place in the next count
				   and won't interrupt the current run */
};

#define AL_TIMER_RAW_MASK		0x00000001
#define AL_TIMER_RAW_SHIFT		0

#define AL_TIMER_MASKED_MASK		0x00000001
#define AL_TIMER_MASKED_SHIFT		0

/* parse control register  */
#define AL_TIMER_CTRL_ONE_SHOT_MASK	0X00000001
#define AL_TIMER_CTRL_ONE_SHOT_SHIFT	0
#define AL_TIMER_CTRL_ONE_SHOT_ON	(1 << AL_TIMER_CTRL_ONE_SHOT_SHIFT)
#define AL_TIMER_CTRL_ONE_SHOT_OFF	(0 << AL_TIMER_CTRL_ONE_SHOT_SHIFT)

#define AL_TIMER_CTRL_SIZE_MASK		0X00000002
#define AL_TIMER_CTRL_SIZE_SHIFT	1
#define AL_TIMER_CTRL_SIZE_16		(0 << AL_TIMER_CTRL_SIZE_SHIFT)
#define AL_TIMER_CTRL_SIZE_32		(1 << AL_TIMER_CTRL_SIZE_SHIFT)

#define AL_TIMER_CTRL_PRESCALE_MASK	0X0000000C
#define AL_TIMER_CTRL_PRESCALE_SHIFT	2
#define AL_TIMER_CTRL_PRESCALE_1	(0 << AL_TIMER_CTRL_PRESCALE_SHIFT)
#define AL_TIMER_CTRL_PRESCALE_16	(1 << AL_TIMER_CTRL_PRESCALE_SHIFT)
#define AL_TIMER_CTRL_PRESCALE_256	(2 << AL_TIMER_CTRL_PRESCALE_SHIFT)

#define AL_TIMER_CTRL_INT_ENABLE_MASK	0X00000020
#define AL_TIMER_CTRL_INT_ENABLE_SHIFT	5
#define AL_TIMER_CTRL_INT_ENABLE	(1 << AL_TIMER_CTRL_INT_ENABLE_SHIFT)
#define AL_TIMER_CTRL_INT_DISABLE	(0 << AL_TIMER_CTRL_INT_ENABLE_SHIFT)

#define AL_TIMER_CTRL_MODE_MASK		0X00000040
#define AL_TIMER_CTRL_MODE_SHIFT	6
#define AL_TIMER_CTRL_MODE_PERIODIC	(1 << AL_TIMER_CTRL_MODE_SHIFT)
#define AL_TIMER_CTRL_MODE_FREE_RUN	(0 << AL_TIMER_CTRL_MODE_SHIFT)

#define AL_TIMER_CTRL_TIMER_ENABLE_MASK	0X00000080
#define AL_TIMER_CTRL_TIMER_ENABLE_SHIFT 7
#define AL_TIMER_CTRL_TIMER_ENABLE	(1 << AL_TIMER_CTRL_TIMER_ENABLE_SHIFT)
#define AL_TIMER_CTRL_TIMER_DISABLE	(0 << AL_TIMER_CTRL_TIMER_ENABLE_SHIFT)

/* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */
#endif
