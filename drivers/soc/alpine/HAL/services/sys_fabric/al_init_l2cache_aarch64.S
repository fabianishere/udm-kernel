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
#ifdef __aarch64__

#include "al_hal_iomap.h"
#include "al_sysreg_aarch64.h"

/**
 * Initialize l2 cache
 *
 * @param	x0
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
	ldr x2, =(AL_SB_DFX_BASE_ASM + 0x0000)
	mov w3, #1
	str w3, [x2]
	ldr x2, =(AL_SB_DFX_BASE_ASM + 0x0080)
1:	ldr w3, [x2]
	cmp w3, #2
	bne 1b
#endif
#if (AL_DEV_ID <= AL_DEV_ID_ALPINE_V3)
	/* set L2ACTLR */
	MRS x2, L2ACTLR_EL1
	/* ARM ERRATA 798870 WA (no problem using it other CPUs)
	 * Addressing RMN: 1732, 1843
	 */
	/* enable hazard detect timeout. */
	ORR x2, x2, #L2ACTLR_EL1_HAZARD_DETECT_TO

#if (!defined(AL_NB_CCI_SNOOP_FILTER)) || (!AL_NB_CCI_SNOOP_FILTER)
	/* disable push evict to external */
	ORR x2, x2, #L2ACTLR_EL1_CLN_EVCT_PUSH_DIS
#endif
	/* Enable WriteUnique and WriteLineUnique transactions */
	bic x2, x2, #L2ACTLR_EL1_WR_UNQ_WR_LN_UNQ_DIS
#if (!defined(AL_NB_ANPA_NUM_CLUSTERS)) || (AL_NB_ANPA_NUM_CLUSTERS == 1)
	/* disable DVM/cache maint. broadcast */
	ORR x2, x2, #L2ACTLR_EL1_DVM_CM_BCAST_DIS
#endif
#if (AL_DEV_ID >= AL_DEV_ID_ALPINE_V3)
	/* Disable PLRU dynamic insertion */
	orr x2, x2, #L2ACTLR_EL1_PLRU_DYN_INS_DIS
#endif
	MSR L2ACTLR_EL1, x2

	/* Set L2CTR - data & tag DRAM latency */
	MRS x2, L2CTLR_EL1
	  /* data RAM latency: 0x2 - 3 cycles */
	bic x2, x2, #L2CTLR_EL1_DRAM_LAT_MASK
	orr x2, x2, #L2CTLR_EL1_DRAM_LAT_VAL_3_CYC
	  /* tag RAM latency: 0x2 - 3 cycles */
	bic x2, x2, #L2CTLR_EL1_TRAM_LAT_MASK
	orr x2, x2, #L2CTLR_EL1_TRAM_LAT_VAL_3_CYC
	/* ECC enable */
	cmp x0, #0
	beq 1f
	orr x2, x2, #L2CTLR_EL1_ECC_EN
1:	MSR L2CTLR_EL1, x2
#else
	/* ECC enable */
	cmp x0, #0
	beq 1f
	mov x2, #0
	MSR ERRSELR_EL1, x2
	MRS x2, ERRXCTLR_EL1
	orr x2, x2, #ERR0CTLR_EL1_ED
	MSR ERRXCTLR_EL1, x2
1:
#endif
	isb
	ret
.endfunc

#endif

