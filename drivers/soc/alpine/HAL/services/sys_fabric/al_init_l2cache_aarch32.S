/*******************************************************************************
Copyright (C) 2015 Annapurna Labs Ltd.

This file may be licensed under the terms of the Annapurna Labs Commercial
License Agreement.

Alternatively, this file can be distributed under the terms of the GNU General
Public License V2 or V3 as published by the Free Software Foundation and can be
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
#ifdef __arm__

#include "al_hal_iomap.h"

/**
 * Initialize l2 cache
 *
 * @param	r0
 *		L2 cache ECC enable
 */
.global al_l2cache_init
.type al_l2cache_init, %function
.func al_l2cache_init
al_l2cache_init:

	/*
	 * Addressing RMN: 2096
	 *
	 * RMN description:
	 * L2 SRAM does not go out of RSCRST, if not using redundancy setting
	 * flow.
	 * Software flow:
	 * Write the value 0x1 to the register in address (AL_SB_DFX_BASE_ASM + 0x0000).
	 * Wait for a "ready" indication by polling the register in address
	 * (AL_SB_DFX_BASE_ASM + 0x0080) for the value 0x2.
	 */
#if (AL_DEV_ID < AL_DEV_ID_ALPINE_V3) ||\
		((AL_DEV_ID == AL_DEV_ID_ALPINE_V3) && (AL_DEV_REV_ID == 0))
	ldr r2, =(AL_SB_DFX_BASE_ASM + 0x0000)
	mov r3, #1
	str r3, [r2]
	ldr r2, =(AL_SB_DFX_BASE_ASM + 0x0080)
1:	ldr r3, [r2]
	cmp r3, #2
	bne 1b
#endif

	/* set L2ACTLR */
	MRC p15, 1, r2, c15, c0, 0
#if (!defined(AL_NB_CCI_SNOOP_FILTER)) || (!AL_NB_CCI_SNOOP_FILTER)
	ORR r2, r2, #(1<<3) /* disable push evict to external */
#endif
	bic r2, r2, #(1<<4) /* Enable WriteUnique and WriteLineUnique transactions */
#if (!defined(AL_NB_ANPA_NUM_CLUSTERS)) || (AL_NB_ANPA_NUM_CLUSTERS == 1)
	ORR r2, r2, #(1<<8) /* disable DVM/cache maint. broadcast */
#endif
#if (AL_DEV_ID >= AL_DEV_ID_ALPINE_V3)
	/* Disable PLRU dynamic insertion */
	orr r2, r2, #(1 << 24)
#endif
	/* ARM ERRATA 798870 WA (no problem using it other CPUs)
	 * Addressing RMN: 1732, 1843
	 */
	ORR r2, r2, #(1<<7) /* enable hazard detect timeout. */

	MCR p15, 1, r2, c15, c0, 0
	/* Set L2CTR - data & tag DRAM latency */
	MRC p15, 1, r2, c9, c0, 2
	  /* data RAM latency: 0x2 - 3 cycles */
	bic r2, r2, #0x7
	orr r2, r2, #0x2
	  /* tag RAM latency: 0x2 - 3 cycles */
	bic r2, r2, #(0x7 << 6)
	orr r2, r2, #(0x2 << 6)
	/* ECC enable, if r0 != 0 */
	cmp r0, #0
	orrne r2, r2, #(0x1 << 21)
	MCR p15, 1, r2, c9, c0, 2
	isb
	mov pc, lr
.endfunc

#endif

