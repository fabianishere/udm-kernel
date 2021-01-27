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

#ifndef __AL_HAL_GICV2_H__
#define __AL_HAL_GICV2_H__

/*
 * These are the registers that maps to a standard ARM GICv2
 */

#ifndef __ASSEMBLER__
/* Two levels of gic in annapurna lab's system */
enum al_nb_gic_id {NB_GIC_MAIN = 0, NB_GIC_SECONDARY = 1};
#endif

/* types of interrupts - from gicv2 architecture*/
#define GIC_SGI_OFFSET 0
#define GIC_SGI_NUM    16
#define GIC_PPI_OFFSET (GIC_SGI_OFFSET + GIC_SGI_NUM)
#define GIC_PPI_NUM    16
#define GIC_SPI_OFFSET (GIC_PPI_OFFSET + GIC_PPI_NUM)
/* number of SPIs is implementation defined, and different
 * for our two GICs */
#if defined(AL_DEV_ID) && ((AL_DEV_ID >= AL_DEV_ID_ALPINE_V2))
#define GIC_SPI_NUM(id) (((id) == NB_GIC_MAIN) * 352 + \
			 ((id) == NB_GIC_SECONDARY) * 480)
#else
#define GIC_SPI_NUM(id) (((id) == NB_GIC_MAIN) * 224 + \
			 ((id) == NB_GIC_SECONDARY) * 480)
#endif
#define GIC_IRQ_NUM(id) (GIC_SPI_OFFSET + GIC_SPI_NUM(id))

/* offsets from gic */
#define GIC_DIST_OFFSET			0x1000
#define GIC_CPU_OFFSET			0x2000

/* GIC registers, most defined in linux */
#define GIC_CPU_CTRL			0x00
#define GIC_CPU_PRIMASK			0x04
#define GIC_CPU_BINPOINT		0x08
#define GIC_CPU_INTACK			0x0c
#define GIC_CPU_EOI			0x10
#define GIC_CPU_RUNNINGPRI		0x14
#define GIC_CPU_HIGHPRI			0x18

#define GIC_DIST_CTRL			0x000
#define GIC_DIST_CTR			0x004
#define GIC_DIST_IGROUPR		0x080 /*not in linux*/
#define GIC_DIST_ENABLE_SET		0x100
#define GIC_DIST_ENABLE_CLEAR		0x180
#define GIC_DIST_PENDING_SET		0x200
#define GIC_DIST_PENDING_CLEAR		0x280
#define GIC_DIST_ACTIVE_BIT		0x300
#define GIC_DIST_PRI			0x400
#define GIC_DIST_TARGET			0x800
#define GIC_DIST_CONFIG			0xc00
#define GIC_DIST_SOFTINT		0xf00

/* mask of interrupts id inside iack register */
#define GIC_IACK_INTID_MASK	0x3ff
#define GIC_IACK_INTID_SHIFT	0
#define GIC_IACK_SRCID_SHIFT	10
#define GIC_IACK_SRCID_MASK	0x1c00

/* IDs of special interrupts (defined by GIC architecture spec)*/
#define GIC_INTID_SPURIOUS		1023
#define GIC_INTID_LEGACY_FIQ		28
#define GIC_INTID_LEGACY_IRQ		31
#if defined(AL_DEV_ID) && ((AL_DEV_ID == AL_DEV_ID_ALPINE_V2))
#define GIC_INTID_SECONDARY_FIQ		16
#define GIC_INTID_SECONDARY_IRQ		19
#else
#define GIC_INTID_SECONDARY_FIQ		GIC_INTID_LEGACY_FIQ
#define GIC_INTID_SECONDARY_IRQ		GIC_INTID_LEGACY_IRQ
#endif

#endif /* __AL_HAL_GICV2_H__ */
