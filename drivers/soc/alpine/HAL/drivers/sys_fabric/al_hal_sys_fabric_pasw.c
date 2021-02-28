/*******************************************************************************
Copyright (C) 2016 Annapurna Labs Ltd.

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
* @{
* @file   al_hal_sys_fabric_pasw.c
*
* @brief  includes Address Map HAL implementation
*
*/

#if (defined(AL_DEV_ID) && defined(AL_DEV_REV_ID) && ((AL_DEV_ID > AL_DEV_ID_ALPINE_V3) ||\
	((AL_DEV_ID == AL_DEV_ID_ALPINE_V3) && (AL_DEV_REV_ID > 0))))

#include "al_hal_sys_fabric_pasw.h"
#include "al_hal_nb_regs.h"

#define AL_SYS_FABRIC_PASW_LOG2SIZE_MAX		48
#define AL_SYS_FABRIC_PASW_LOG2SIZE_BASE	15

#define AL_SYS_FABRIC_STRIPE_BIT_BASE		6
#define AL_SYS_FABRIC_STRIPE_BIT_MAX		20

/* general PASWS */
/* Window size = 2 ^ (15 + win_size). Zero value: disable the window. */
#define NB_ADDRESS_MAP_LOW_WIN_SIZE_MASK 0x0000003F
#define NB_ADDRESS_MAP_LOW_WIN_SIZE_SHIFT 0

/*
 * DRAM channel
 * 1: DRAM0
 * 2: DRAM1
 * 3: Interleave DRAM0 / DRAM1
 */
#define NB_ADDRESS_MAP_LOW_PORT_MASK 0x00000F00
#define NB_ADDRESS_MAP_LOW_PORT_SHIFT 8
/*
 * Target Memory
 * 0: DRAM
 * 1: SRAM
 */
#define NB_ADDRESS_MAP_LOW_TARGET_MEMORY (1 << 12)
/* Bar low address bits [31:16] */
#define NB_ADDRESS_MAP_LOW_ADDR_LOW_MASK 0xFFFF0000
#define NB_ADDRESS_MAP_LOW_ADDR_LOW_SHIFT 16

/* For remapping bits [39 - 29] of DRAM 40bit Physical address are used */
#define NB_ADDRESS_MAP_DRAM_REMAP_BASE_ADDR_SHIFT	29
#define NB_ADDRESS_MAP_DRAM_REMAP_BASE_ADDR_MASK	0xFFE0000000ULL
#define NB_ADDRESS_MAP_BIT_MAX				40

static void al_sys_fabric_pasw_latch_set(
	void __iomem	*nb_regs_base,
	al_bool		en)
{
	struct al_nb_regs *nb_regs = (struct al_nb_regs *)nb_regs_base;

	if (en)
		al_data_memory_barrier();
	al_reg_write32_masked(
		&nb_regs->address_map.latch_bars, NB_ADDRESS_MAP_LATCH_BARS_ENABLE,
		en ? NB_ADDRESS_MAP_LATCH_BARS_ENABLE : 0);
	if (!en)
		al_data_memory_barrier();
}

static int al_sys_fabric_pasw_get_bar_regs(
	struct al_nb_regs	*nb_regs,
	enum al_sys_fabric_pasw	pasw,
	uint32_t		**bar_reg_high,
	uint32_t		**bar_reg_low)
{
	switch (pasw) {
	case AL_SYS_FABRIC_PASW_DDR_CPU_BAR0:
		*bar_reg_low = &nb_regs->address_map.ddr_cpu_bar0_low;
		*bar_reg_high = &nb_regs->address_map.ddr_cpu_bar0_high;
		break;
	case AL_SYS_FABRIC_PASW_DDR_CPU_BAR1:
		*bar_reg_low = &nb_regs->address_map.ddr_cpu_bar1_low;
		*bar_reg_high = &nb_regs->address_map.ddr_cpu_bar1_high;
		break;
	case AL_SYS_FABRIC_PASW_DDR_CPU_BAR2:
		*bar_reg_low = &nb_regs->address_map.ddr_cpu_bar2_low;
		*bar_reg_high = &nb_regs->address_map.ddr_cpu_bar2_high;
		break;
	case AL_SYS_FABRIC_PASW_DDR_CPU_BAR3:
		*bar_reg_low = &nb_regs->address_map.ddr_cpu_bar3_low;
		*bar_reg_high = &nb_regs->address_map.ddr_cpu_bar3_high;
		break;
	case AL_SYS_FABRIC_PASW_DDR_CPU_BAR4:
		*bar_reg_low = &nb_regs->address_map.ddr_cpu_bar4_low;
		*bar_reg_high = &nb_regs->address_map.ddr_cpu_bar4_high;
		break;
	case AL_SYS_FABRIC_PASW_DDR_CPU_BAR5:
		*bar_reg_low = &nb_regs->address_map.ddr_cpu_bar5_low;
		*bar_reg_high = &nb_regs->address_map.ddr_cpu_bar5_high;
		break;
	case AL_SYS_FABRIC_PASW_DDR_IO_BAR0:
		*bar_reg_low = &nb_regs->address_map.ddr_io_bar0_low;
		*bar_reg_high = &nb_regs->address_map.ddr_io_bar0_high;
		break;
	case AL_SYS_FABRIC_PASW_DDR_IO_BAR1:
		*bar_reg_low = &nb_regs->address_map.ddr_io_bar1_low;
		*bar_reg_high = &nb_regs->address_map.ddr_io_bar1_high;
		break;
	case AL_SYS_FABRIC_PASW_DDR_IO_BAR2:
		*bar_reg_low = &nb_regs->address_map.ddr_io_bar2_low;
		*bar_reg_high = &nb_regs->address_map.ddr_io_bar2_high;
		break;
	case AL_SYS_FABRIC_PASW_DDR_IO_BAR3:
		*bar_reg_low = &nb_regs->address_map.ddr_io_bar3_low;
		*bar_reg_high = &nb_regs->address_map.ddr_io_bar3_high;
		break;
	case AL_SYS_FABRIC_PASW_DDR_IO_BAR4:
		*bar_reg_low = &nb_regs->address_map.ddr_io_bar4_low;
		*bar_reg_high = &nb_regs->address_map.ddr_io_bar4_high;
		break;
	case AL_SYS_FABRIC_PASW_DDR_IO_BAR5:
		*bar_reg_low = &nb_regs->address_map.ddr_io_bar5_low;
		*bar_reg_high = &nb_regs->address_map.ddr_io_bar5_high;
		break;
	case AL_SYS_FABRIC_PASW_MSIX_BAR:
		*bar_reg_low = &nb_regs->address_map.msix_bar_low;
		*bar_reg_high = &nb_regs->address_map.msix_bar_high;
		break;
	default:
		al_err("%s: unknown pasw %d\n", __func__, pasw);
		return -EINVAL;
	}

	return 0;
}

int al_sys_fabric_pasw_set(
	void __iomem			*nb_regs_base,
	enum al_sys_fabric_pasw		pasw,
	struct al_sys_fabric_pasw_cfg	*cfg)
{
	struct al_nb_regs *nb_regs = (struct al_nb_regs *)nb_regs_base;
	al_phys_addr_t base;
	uint32_t log2size;
	uint64_t size_mask;
	uint32_t base_low;
	uint32_t base_high;
	uint32_t *bar_reg_high;
	uint32_t *bar_reg_low;

	al_assert(cfg);

	base = cfg->base;
	log2size = cfg->log2size;

	al_assert(log2size >= AL_SYS_FABRIC_PASW_LOG2SIZE_BASE);
	al_assert(log2size <= AL_SYS_FABRIC_PASW_LOG2SIZE_MAX);

	if (log2size > AL_SYS_FABRIC_PASW_LOG2SIZE_BASE) {
		size_mask = ((uint64_t)1 << log2size) - 1;
		if (base & size_mask) {
			al_err("%s: pasw base has to be aligned to size\n"
				"base=0x%16" PRIx64 "\n"
				"log2size=%d\n"
				"size_mask=0x%16" PRIx64 "\n",
				__func__,
				base,
				log2size,
				size_mask);
			return -EINVAL;
		}
	}

	/* check that base+size fit max allowed size */
	if ((base + ((uint64_t)1 << log2size)) > ((uint64_t)1 << AL_SYS_FABRIC_PASW_LOG2SIZE_MAX)) {
		al_err("%s: base + size doesn't fit %dbit\n", __func__,
			AL_SYS_FABRIC_PASW_LOG2SIZE_MAX);
		return -EINVAL;
	}

	if (al_sys_fabric_pasw_get_bar_regs(nb_regs, pasw, &bar_reg_high, &bar_reg_low))
		return -EINVAL;

	base_high = (uint32_t)((base >> 32) & 0xffffffff);
	base_low =
		(uint32_t)(base & 0xffffffff) |
		((log2size - AL_SYS_FABRIC_PASW_LOG2SIZE_BASE) <<
		NB_ADDRESS_MAP_LOW_WIN_SIZE_SHIFT) |
		(cfg->ch << NB_ADDRESS_MAP_LOW_PORT_SHIFT) |
		(cfg->tgt_mem ? NB_ADDRESS_MAP_LOW_TARGET_MEMORY : 0);

	al_sys_fabric_pasw_latch_set(nb_regs_base, AL_FALSE);
	al_reg_write32(bar_reg_high, base_high);
	al_reg_write32(bar_reg_low, base_low);
	al_sys_fabric_pasw_latch_set(nb_regs_base, AL_TRUE);

	return 0;
}

int al_sys_fabric_pasw_get(
	void __iomem			*nb_regs_base,
	enum al_sys_fabric_pasw		pasw,
	struct al_sys_fabric_pasw_cfg	*cfg)
{
	struct al_nb_regs *nb_regs = (struct al_nb_regs *)nb_regs_base;
	uint32_t base_low;
	uint32_t base_high;
	uint32_t *bar_reg_high;
	uint32_t *bar_reg_low;

	al_assert(nb_regs_base);
	al_assert(cfg);

	if (al_sys_fabric_pasw_get_bar_regs(nb_regs, pasw, &bar_reg_high, &bar_reg_low))
		return -EINVAL;

	base_high = al_reg_read32(bar_reg_high);
	base_low = al_reg_read32(bar_reg_low);

	cfg->log2size =	((base_low & NB_ADDRESS_MAP_LOW_WIN_SIZE_MASK) >>
		NB_ADDRESS_MAP_LOW_WIN_SIZE_SHIFT) + AL_SYS_FABRIC_PASW_LOG2SIZE_BASE;
	cfg->base = ((al_phys_addr_t)base_high << 32) |
		(al_phys_addr_t)(base_low & NB_ADDRESS_MAP_LOW_ADDR_LOW_MASK);
	cfg->ch = ((base_low & NB_ADDRESS_MAP_LOW_PORT_MASK) >>
		NB_ADDRESS_MAP_LOW_PORT_SHIFT);
	cfg->tgt_mem = !!(base_low & NB_ADDRESS_MAP_LOW_TARGET_MEMORY);

	return 0;
}

int al_sys_fabric_pasw_dram_remap_set(
	void __iomem	*nb_regs_base,
	al_phys_addr_t	dram_remap_base,
	al_phys_addr_t	dram_remap_transl_base,
	unsigned int	window_size)
{
	struct al_nb_regs *nb_regs;
	uint32_t reg_val = 0;
	uint32_t remap_base_val;
	uint32_t remap_transl_base_val;

	al_assert(nb_regs_base);
	al_assert(window_size <=
		  (NB_ADDRESS_MAP_BIT_MAX - NB_ADDRESS_MAP_DRAM_REMAP_BASE_ADDR_SHIFT+1));
	al_assert(dram_remap_base >= (1 << NB_ADDRESS_MAP_DRAM_REMAP_BASE_ADDR_SHIFT));
	al_assert(!(dram_remap_transl_base) ||
		  (dram_remap_transl_base >= (1 << NB_ADDRESS_MAP_DRAM_REMAP_BASE_ADDR_SHIFT)));

	nb_regs = (struct al_nb_regs *)nb_regs_base;

	remap_base_val =
		(dram_remap_base & NB_ADDRESS_MAP_DRAM_REMAP_BASE_ADDR_MASK) >>
		NB_ADDRESS_MAP_DRAM_REMAP_BASE_ADDR_SHIFT;

	remap_transl_base_val =
		(dram_remap_transl_base & NB_ADDRESS_MAP_DRAM_REMAP_BASE_ADDR_MASK) >>
		NB_ADDRESS_MAP_DRAM_REMAP_BASE_ADDR_SHIFT;

	reg_val = (remap_base_val << NB_ADDRESS_MAP_DRAM_REMAP_ADDR_SHIFT) |
		(remap_transl_base_val << NB_ADDRESS_MAP_DRAM_REMAP_TRANS_ADDR_SHIFT) |
		window_size;

	al_sys_fabric_pasw_latch_set(nb_regs_base, AL_FALSE);
	al_reg_write32(&nb_regs->address_map.dram_remap, reg_val);
	al_sys_fabric_pasw_latch_set(nb_regs_base, AL_TRUE);

	return 0;
}

void al_sys_fabric_ccu_hash_2_low_set(
	void __iomem	*nb_regs_base,
	unsigned int	val)
{
	struct al_nb_regs *nb_regs;

	al_assert(nb_regs_base);

	nb_regs = (struct al_nb_regs *)nb_regs_base;

	al_reg_write32(&nb_regs->acf.hash_2_low, val);
}

void al_sys_fabric_pasw_dram_intrlv_stripe_bit_set(
	void __iomem	*nb_regs_base,
	unsigned int	bit)
{
	struct al_nb_regs *nb_regs;

	al_assert(nb_regs_base);
	al_assert((bit == 0) || (bit >= AL_SYS_FABRIC_STRIPE_BIT_BASE));
	al_assert(bit <= AL_SYS_FABRIC_STRIPE_BIT_MAX);

	nb_regs = (struct al_nb_regs *)nb_regs_base;

	if (bit >= AL_SYS_FABRIC_STRIPE_BIT_BASE)
		bit -= (AL_SYS_FABRIC_STRIPE_BIT_BASE - 1);

	al_reg_write32_masked(&nb_regs->address_map.config, NB_ADDRESS_MAP_CONFIG_STRIPE_BIT_MASK,
		bit << NB_ADDRESS_MAP_CONFIG_STRIPE_BIT_SHIFT);
}

unsigned int al_sys_fabric_pasw_dram_intrlv_stripe_bit_get(
	void __iomem	*nb_regs_base)
{
	struct al_nb_regs *nb_regs;
	unsigned int bit;

	al_assert(nb_regs_base);

	nb_regs = (struct al_nb_regs *)nb_regs_base;

	bit = ((al_reg_read32(&nb_regs->address_map.config) &
		NB_ADDRESS_MAP_CONFIG_STRIPE_BIT_MASK) >> NB_ADDRESS_MAP_CONFIG_STRIPE_BIT_SHIFT);
	if (bit > 0)
		bit += AL_SYS_FABRIC_STRIPE_BIT_BASE - 1;

	return bit;
}

#endif
