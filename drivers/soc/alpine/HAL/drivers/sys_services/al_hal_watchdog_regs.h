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

#ifndef __AL_HAL_WATCHDOG_REGS_H__
#define __AL_HAL_WATCHDOG_REGS_H__

/**
 * The following struct describes the watchdog configuration registers
 */

#include "al_hal_plat_types.h"

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

struct al_watchdog_regs {
	uint32_t load;		/* the value the counter count down from */
	uint32_t value;		/* current value - read only */
	uint32_t control;	/* watchdog control- inten, reseten */
	uint32_t intclr;	/* every write to this reg will clear
						the interrupt  */
	uint32_t ris;		/* raw interrupt status */
	uint32_t mis;		/* mask interrupt status */
};

#define AL_WATCHDOG_CTRL_INT_ENABLE_SHIFT 0 /* Interrupt enable */
#define AL_WATCHDOG_CTRL_INT_ENABLE_MASK  0X00000001
#define AL_WATCHDOG_CTRL_INT_ENABLE       \
	(1 << AL_WATCHDOG_CTRL_INT_ENABLE_SHIFT)
#define AL_WATCHDOG_CTRL_INT_DISABLE      \
	(0 << AL_WATCHDOG_CTRL_INT_ENABLE_SHIFT)

#define AL_WATCHDOG_CTRL_RESET_ENABLE_SHIFT 1 /* Reset enable */
#define AL_WATCHDOG_CTRL_RESET_ENABLE_MASK  0X00000002
#define AL_WATCHDOG_CTRL_RESET_ENABLE       \
	(1 << AL_WATCHDOG_CTRL_RESET_ENABLE_SHIFT)
#define AL_WATCHDOG_CTRL_RESET_DISABLE      \
	(0 << AL_WATCHDOG_CTRL_RESET_ENABLE_SHIFT)

#define AL_WATCHDOG_RAW_MASK		0x00000001 /* raw interrupt status */
#define AL_WATCHDOG_RAW_SHIFT		0

#define AL_WATCHDOG_MASKED_MASK		0x00000001 /* masked interrupt status */
#define AL_WATCHDOG_MASKED_SHIFT	0


/* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */
#endif
