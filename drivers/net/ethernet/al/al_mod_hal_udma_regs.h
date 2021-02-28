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
 * @file   al_mod_hal_udma_regs.h
 *
 * @brief  udma registers definition
 *
 *
 */
#ifndef __AL_HAL_UDMA_REG_H
#define __AL_HAL_UDMA_REG_H

#include "al_mod_hal_udma_regs_m2s.h"
#include "al_mod_hal_udma_regs_s2m.h"
#include "al_mod_hal_udma_regs_gen.h"
#ifdef AL_UDMA_EX
#include "al_mod_hal_udma_regs_gen_ex.h"
#endif

#define AL_UDMA_REV_ID_REV0   0
#define AL_UDMA_REV_ID_REV1   1
#define AL_UDMA_REV_ID_REV2   2
#define AL_UDMA_REV_ID_REV4   4

#ifdef __cplusplus
extern "C" {
#endif

/** UDMA registers, either m2s or s2m */
union udma_regs {
	struct udma_m2s_regs_v4 m2s;
	struct udma_s2m_regs_v4 s2m;
};

struct unit_regs_v3 {
	struct udma_m2s_regs_v3 m2s;
	uint32_t rsrvd0[(0x10000 - sizeof(struct udma_m2s_regs_v3)) >> 2];
	struct udma_s2m_regs_v3 s2m;
	uint32_t rsrvd1[((0x1C000 - 0x10000) - sizeof(struct udma_s2m_regs_v3)) >> 2];
	struct udma_gen_regs_v3 gen;
	uint32_t rsrvd2[((0x1E800 - 0x1C000) - sizeof(struct udma_gen_regs_v3)) >> 2];
	struct udma_gen_ex_regs gen_ex;
};

struct unit_regs_v4 {
	struct udma_m2s_regs_v4 m2s;
	uint32_t rsrvd0[(0x20000 - sizeof(struct udma_m2s_regs_v4)) >> 2];
	struct udma_s2m_regs_v4 s2m;
	uint32_t rsrvd1[((0x38000 - 0x20000) - sizeof(struct udma_s2m_regs_v4)) >> 2];
	struct udma_gen_regs_v4 gen;
	uint32_t rsrvd2[((0x3c000 - 0x38000) - sizeof(struct udma_gen_regs_v4)) >> 2];
	struct udma_gen_ex_regs gen_ex;
};

/** UDMA submission and completion registers, M2S and S2M UDMAs have same stucture */
struct udma_rings_regs {
	uint32_t rsrvd0[8];
	uint32_t cfg;		/* Descriptor ring configuration */
	uint32_t status;	/* Descriptor ring status and information */
	uint32_t drbp_low;	/* Descriptor Ring Base Pointer [31:4] */
	uint32_t drbp_high;	/* Descriptor Ring Base Pointer [63:32] */
	uint32_t drl;		/* Descriptor Ring Length[23:2] */
	uint32_t drhp;		/* Descriptor Ring Head Pointer */
	uint32_t drtp_inc;	/* Descriptor Tail Pointer increment */
	uint32_t drtp;		/* Descriptor Tail Pointer */
	uint32_t dcp;		/* Descriptor Current Pointer */
	uint32_t crbp_low;	/* Completion Ring Base Pointer [31:4] */
	uint32_t crbp_high;	/* Completion Ring Base Pointer [63:32] */
	uint32_t crhp;		/* Completion Ring Head Pointer */
	uint32_t crhp_internal;	/* Completion Ring Head Pointer internal, before AX ... */
};

/** M2S and S2M generic structure of Q registers */
union udma_q_regs {
	struct udma_rings_regs	rings;
	struct udma_m2s_q	m2s_q;
	struct udma_s2m_q	s2m_q;
};

#ifdef __cplusplus
}
#endif

#endif /* __AL_HAL_UDMA_REG_H */
/** @} end of UDMA group */
