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
* @{
* @file   al_hal_addr_map.c
*
* @brief  includes Address Map HAL implementation
*
*/

#include "al_hal_addr_map.h"
#include "al_hal_pbs_regs.h"
#include "al_hal_pbs_utils.h"

#define AL_ADDR_MAP_REV_2_3_PASW_LOG2SIZE_MAX		48
#define AL_ADDR_MAP_REV_0_1_PASW_LOG2SIZE_MAX		40
#define AL_ADDR_MAP_PASW_LOG2SIZE_BASE				15

struct al_addr_map_dma_io_master_map_ent {
	enum al_addr_map_dma_io_master	master;
	int				bit_idx;
};

static const struct al_addr_map_dma_io_master_map_ent dma_io_master_map_v2[] = {
	{ AL_ADDR_MAP_DMA_IO_MASTER_CRYPTO_0, 0 },
	{ AL_ADDR_MAP_DMA_IO_MASTER_CRYPTO_1, 1 },
	{ AL_ADDR_MAP_DMA_IO_MASTER_ETH_0, 2 },
	{ AL_ADDR_MAP_DMA_IO_MASTER_ETH_1, 3 },
	{ AL_ADDR_MAP_DMA_IO_MASTER_ETH_2, 4 },
	{ AL_ADDR_MAP_DMA_IO_MASTER_ETH_3, 5 },
	{ AL_ADDR_MAP_DMA_IO_MASTER_SATA_0, 6 },
	{ AL_ADDR_MAP_DMA_IO_MASTER_SATA_1, 7 },
	{ AL_ADDR_MAP_DMA_IO_MASTER_PCIE_0, 8 },
	{ AL_ADDR_MAP_DMA_IO_MASTER_PCIE_1, 9 },
	{ AL_ADDR_MAP_DMA_IO_MASTER_PCIE_2, 10 },
	{ AL_ADDR_MAP_DMA_IO_MASTER_PCIE_3, 11 },
	{ AL_ADDR_MAP_DMA_IO_MASTER_SPI_DBG, 12 },
	{ AL_ADDR_MAP_DMA_IO_MASTER_CPU_DBG, 13 },
};

static const struct al_addr_map_dma_io_master_map_ent dma_io_master_map_v3[] = {
	{ AL_ADDR_MAP_DMA_IO_MASTER_CRYPTO_0, 0 },
	{ AL_ADDR_MAP_DMA_IO_MASTER_CRYPTO_1, 1 },
	{ AL_ADDR_MAP_DMA_IO_MASTER_ETH_0, 2 },
	{ AL_ADDR_MAP_DMA_IO_MASTER_ETH_1, 3 },
	{ AL_ADDR_MAP_DMA_IO_MASTER_ETH_2, 4 },
	{ AL_ADDR_MAP_DMA_IO_MASTER_ETH_3, 5 },
	{ AL_ADDR_MAP_DMA_IO_MASTER_ETH_4, 6 },
	{ AL_ADDR_MAP_DMA_IO_MASTER_ETH_5, 7 },
	{ AL_ADDR_MAP_DMA_IO_MASTER_SPI_DBG, 10 },
	{ AL_ADDR_MAP_DMA_IO_MASTER_CPU_DBG, 11 },
	{ AL_ADDR_MAP_DMA_IO_MASTER_PCIE_0, 12 },
	{ AL_ADDR_MAP_DMA_IO_MASTER_PCIE_1, 13 },
	{ AL_ADDR_MAP_DMA_IO_MASTER_PCIE_2, 14 },
	{ AL_ADDR_MAP_DMA_IO_MASTER_PCIE_3, 15 },
	{ AL_ADDR_MAP_DMA_IO_MASTER_PCIE_4, 16 },
	{ AL_ADDR_MAP_DMA_IO_MASTER_PCIE_5, 17 },
	{ AL_ADDR_MAP_DMA_IO_MASTER_PCIE_6, 18 },
	{ AL_ADDR_MAP_DMA_IO_MASTER_PCIE_7, 19 },
	{ AL_ADDR_MAP_DMA_IO_MASTER_SATA_0, 22 },
	{ AL_ADDR_MAP_DMA_IO_MASTER_SATA_1, 23 },
	{ AL_ADDR_MAP_DMA_IO_MASTER_SATA_2, 24 },
	{ AL_ADDR_MAP_DMA_IO_MASTER_SATA_3, 25 },
	{ AL_ADDR_MAP_DMA_IO_MASTER_SATA_4, 26 },
	{ AL_ADDR_MAP_DMA_IO_MASTER_SATA_5, 27 },
	{ AL_ADDR_MAP_DMA_IO_MASTER_SATA_6, 28 },
	{ AL_ADDR_MAP_DMA_IO_MASTER_SATA_7, 29 },
	{ AL_ADDR_MAP_DMA_IO_MASTER_ETH_DB, 31 },
};

static int al_addr_map_latch_set(
		void __iomem *pbs_regs_base,
		uint32_t dev_id,
		al_bool latch_en)
{
	struct al_pbs_regs *pbs_regs = (struct al_pbs_regs *)pbs_regs_base;
	uint32_t __iomem *latch_bars_reg;

	if (dev_id <= PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V2)
		latch_bars_reg = &pbs_regs->unit.latch_bars_v2;
	else
		latch_bars_reg = &pbs_regs->bar_config.latch_bars;

	if (latch_en == AL_TRUE)
		al_reg_write32(latch_bars_reg, 0);
	else
		al_reg_write32(latch_bars_reg, 1);

	return 0;
}

static int al_addr_map_get_bar_regs(
		struct al_pbs_regs *pbs_regs,
		uint32_t dev_id,
		uint32_t dev_rev_id,
		enum al_addr_map_pasw pasw,
		uint32_t **bar_reg_high,
		uint32_t **bar_reg_low)
{
	if (dev_id <= PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V2) {
		switch (pasw) {
		case AL_ADDR_MAP_PASW_NB_DRAM0:
			*bar_reg_high = &pbs_regs->unit.dram_0_nb_bar_high_v2;
			*bar_reg_low = &pbs_regs->unit.dram_0_nb_bar_low_v2;
			break;
		case AL_ADDR_MAP_PASW_NB_DRAM1:
			*bar_reg_high = &pbs_regs->unit.dram_1_nb_bar_high_v2;
			*bar_reg_low = &pbs_regs->unit.dram_1_nb_bar_low_v2;
			break;
		case AL_ADDR_MAP_PASW_NB_DRAM2:
			*bar_reg_high = &pbs_regs->unit.dram_2_nb_bar_high_v2;
			*bar_reg_low = &pbs_regs->unit.dram_2_nb_bar_low_v2;
			break;
		case AL_ADDR_MAP_PASW_NB_DRAM3:
			*bar_reg_high = &pbs_regs->unit.dram_3_nb_bar_high_v2;
			*bar_reg_low = &pbs_regs->unit.dram_3_nb_bar_low_v2;
			break;
		case AL_ADDR_MAP_PASW_NB_MSIX:
			*bar_reg_high = &pbs_regs->unit.msix_nb_bar_high_v2;
			*bar_reg_low = &pbs_regs->unit.msix_nb_bar_low_v2;
			break;
		case AL_ADDR_MAP_PASW_SB_DRAM0:
			*bar_reg_high = &pbs_regs->unit.dram_0_sb_bar_high_v2;
			*bar_reg_low = &pbs_regs->unit.dram_0_sb_bar_low_v2;
			break;
		case AL_ADDR_MAP_PASW_SB_DRAM1:
			*bar_reg_high = &pbs_regs->unit.dram_1_sb_bar_high_v2;
			*bar_reg_low = &pbs_regs->unit.dram_1_sb_bar_low_v2;
			break;
		case AL_ADDR_MAP_PASW_SB_DRAM2:
			*bar_reg_high = &pbs_regs->unit.dram_2_sb_bar_high_v2;
			*bar_reg_low = &pbs_regs->unit.dram_2_sb_bar_low_v2;
			break;
		case AL_ADDR_MAP_PASW_SB_DRAM3:
			*bar_reg_high = &pbs_regs->unit.dram_3_sb_bar_high_v2;
			*bar_reg_low = &pbs_regs->unit.dram_3_sb_bar_low_v2;
			break;
		case AL_ADDR_MAP_PASW_SB_MSIX:
			*bar_reg_high = &pbs_regs->unit.msix_sb_bar_high_v2;
			*bar_reg_low = &pbs_regs->unit.msix_sb_bar_low_v2;
			break;
		case AL_ADDR_MAP_PASW_PCIE_EXT_MEM0:
			*bar_reg_high = &pbs_regs->unit.pcie_mem0_bar_high_v2;
			*bar_reg_low = &pbs_regs->unit.pcie_mem0_bar_low_v2;
			break;
		case AL_ADDR_MAP_PASW_PCIE_EXT_MEM1:
			*bar_reg_high = &pbs_regs->unit.pcie_mem1_bar_high_v2;
			*bar_reg_low = &pbs_regs->unit.pcie_mem1_bar_low_v2;
			break;
		case AL_ADDR_MAP_PASW_PCIE_EXT_MEM2:
			*bar_reg_high = &pbs_regs->unit.pcie_mem2_bar_high_v2;
			*bar_reg_low = &pbs_regs->unit.pcie_mem2_bar_low_v2;
			break;
		case AL_ADDR_MAP_PASW_PCIE_EXT_MEM3:
			*bar_reg_high = &pbs_regs->unit.pcie_mem3_bar_high_v2;
			*bar_reg_low = &pbs_regs->unit.pcie_mem3_bar_low_v2;
			break;
		case AL_ADDR_MAP_PASW_PCIE_EXT_ECAM0:
			*bar_reg_high = &pbs_regs->unit.pcie_ext_ecam0_bar_high_v2;
			*bar_reg_low = &pbs_regs->unit.pcie_ext_ecam0_bar_low_v2;
			break;
		case AL_ADDR_MAP_PASW_PCIE_EXT_ECAM1:
			*bar_reg_high = &pbs_regs->unit.pcie_ext_ecam1_bar_high_v2;
			*bar_reg_low = &pbs_regs->unit.pcie_ext_ecam1_bar_low_v2;
			break;
		case AL_ADDR_MAP_PASW_PCIE_EXT_ECAM2:
			*bar_reg_high = &pbs_regs->unit.pcie_ext_ecam2_bar_high_v2;
			*bar_reg_low = &pbs_regs->unit.pcie_ext_ecam2_bar_low_v2;
			break;
		case AL_ADDR_MAP_PASW_PCIE_EXT_ECAM3:
			*bar_reg_high = &pbs_regs->unit.pcie_ext_ecam3_bar_high_v2;
			*bar_reg_low = &pbs_regs->unit.pcie_ext_ecam3_bar_low_v2;
			break;
		case AL_ADDR_MAP_PASW_PBS_SPI:
			*bar_reg_high = &pbs_regs->unit.pbs_spi_bar_high_v2;
			*bar_reg_low = &pbs_regs->unit.pbs_spi_bar_low_v2;
			break;
		case AL_ADDR_MAP_PASW_PBS_NAND:
			*bar_reg_high = &pbs_regs->unit.pbs_nand_bar_high_v2;
			*bar_reg_low = &pbs_regs->unit.pbs_nand_bar_low_v2;
			break;
		case AL_ADDR_MAP_PASW_PBS_NOR:
			*bar_reg_high = &pbs_regs->unit.pbs_nor_bar_high_v2;
			*bar_reg_low = &pbs_regs->unit.pbs_nor_bar_low_v2;
			break;
		case AL_ADDR_MAP_PASW_PBS_INT_MEM:
			*bar_reg_high = &pbs_regs->unit.pbs_int_mem_bar_high_v2;
			*bar_reg_low = &pbs_regs->unit.pbs_int_mem_bar_low_v2;
			break;
		case AL_ADDR_MAP_PASW_PBS_BOOT:
			*bar_reg_high = &pbs_regs->unit.pbs_boot_bar_high_v2;
			*bar_reg_low = &pbs_regs->unit.pbs_boot_bar_low_v2;
			break;
		case AL_ADDR_MAP_PASW_NB_INT:
			*bar_reg_high = &pbs_regs->unit.nb_int_bar_high_v2;
			*bar_reg_low = &pbs_regs->unit.nb_int_bar_low_v2;
			break;
		case AL_ADDR_MAP_PASW_NB_STM:
			*bar_reg_high = &pbs_regs->unit.nb_stm_bar_high_v2;
			*bar_reg_low = &pbs_regs->unit.nb_stm_bar_low_v2;
			break;
		case AL_ADDR_MAP_PASW_PCIE_INT_ECAM:
			*bar_reg_high = &pbs_regs->unit.pcie_ecam_int_bar_high_v2;
			*bar_reg_low = &pbs_regs->unit.pcie_ecam_int_bar_low_v2;
			break;
		case AL_ADDR_MAP_PASW_PCIE_INT_MEM:
			*bar_reg_high = &pbs_regs->unit.pcie_mem_int_bar_high_v2;
			*bar_reg_low = &pbs_regs->unit.pcie_mem_int_bar_low_v2;
			break;
		case AL_ADDR_MAP_PASW_SB_INT:
			*bar_reg_high = &pbs_regs->unit.sb_int_bar_high_v2;
			*bar_reg_low = &pbs_regs->unit.sb_int_bar_low_v2;
			break;
		default:
			al_err("addr_map: unknown pasw %d\n", pasw);
			return -EINVAL;
		}
	} else if ((dev_id == PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V3) &&
			(dev_rev_id == PBS_UNIT_CHIP_ID_DEV_REV_ID_ALPINE_V3_TC)) {
		switch (pasw) {
		case AL_ADDR_MAP_PASW_NB_DRAM0:
			*bar_reg_high = &pbs_regs->bar_config.dram_0_nb_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.dram_0_nb_bar_low;
			break;
		case AL_ADDR_MAP_PASW_NB_DRAM1:
			*bar_reg_high = &pbs_regs->bar_config.dram_1_nb_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.dram_1_nb_bar_low;
			break;
		case AL_ADDR_MAP_PASW_NB_DRAM2:
			*bar_reg_high = &pbs_regs->bar_config.dram_2_nb_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.dram_2_nb_bar_low;
			break;
		case AL_ADDR_MAP_PASW_NB_DRAM3:
			*bar_reg_high = &pbs_regs->bar_config.dram_3_nb_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.dram_3_nb_bar_low;
			break;
		case AL_ADDR_MAP_PASW_NB_MSIX:
			*bar_reg_high = &pbs_regs->bar_config.msix_nb_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.msix_nb_bar_low;
			break;
		case AL_ADDR_MAP_PASW_SB_DRAM0:
			*bar_reg_high = &pbs_regs->bar_config.dram_0_sb_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.dram_0_sb_bar_low;
			break;
		case AL_ADDR_MAP_PASW_SB_DRAM1:
			*bar_reg_high = &pbs_regs->bar_config.dram_1_sb_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.dram_1_sb_bar_low;
			break;
		case AL_ADDR_MAP_PASW_SB_DRAM2:
			*bar_reg_high = &pbs_regs->bar_config.dram_2_sb_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.dram_2_sb_bar_low;
			break;
		case AL_ADDR_MAP_PASW_SB_DRAM3:
			*bar_reg_high = &pbs_regs->bar_config.dram_3_sb_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.dram_3_sb_bar_low;
			break;
		case AL_ADDR_MAP_PASW_SB_MSIX:
			*bar_reg_high = &pbs_regs->bar_config.msix_sb_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.msix_sb_bar_low;
			break;
		case AL_ADDR_MAP_PASW_PCIE_EXT_MEM0:
			*bar_reg_high = &pbs_regs->bar_config.pcie_mem0_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.pcie_mem0_bar_low;
			break;
		case AL_ADDR_MAP_PASW_PCIE_EXT_MEM1:
			*bar_reg_high = &pbs_regs->bar_config.pcie_mem1_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.pcie_mem1_bar_low;
			break;
		case AL_ADDR_MAP_PASW_PCIE_EXT_MEM2:
			*bar_reg_high = &pbs_regs->bar_config.pcie_mem2_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.pcie_mem2_bar_low;
			break;
		case AL_ADDR_MAP_PASW_PCIE_EXT_MEM3:
			*bar_reg_high = &pbs_regs->bar_config.pcie_mem3_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.pcie_mem3_bar_low;
			break;
		case AL_ADDR_MAP_PASW_PCIE_EXT_MEM4:
			*bar_reg_high = &pbs_regs->bar_config.pcie_mem4_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.pcie_mem4_bar_low;
			break;
		case AL_ADDR_MAP_PASW_PCIE_EXT_MEM5:
			*bar_reg_high = &pbs_regs->bar_config.pcie_mem5_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.pcie_mem5_bar_low;
			break;
		case AL_ADDR_MAP_PASW_PCIE_EXT_MEM6:
			*bar_reg_high = &pbs_regs->bar_config.pcie_mem6_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.pcie_mem6_bar_low;
			break;
		case AL_ADDR_MAP_PASW_PCIE_EXT_MEM7:
			*bar_reg_high = &pbs_regs->bar_config.pcie_mem7_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.pcie_mem7_bar_low;
			break;
		case AL_ADDR_MAP_PASW_PCIE_EXT_ECAM0:
			*bar_reg_high = &pbs_regs->bar_config.pcie_ext_ecam0_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.pcie_ext_ecam0_bar_low;
			break;
		case AL_ADDR_MAP_PASW_PCIE_EXT_ECAM1:
			*bar_reg_high = &pbs_regs->bar_config.pcie_ext_ecam1_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.pcie_ext_ecam1_bar_low;
			break;
		case AL_ADDR_MAP_PASW_PCIE_EXT_ECAM2:
			*bar_reg_high = &pbs_regs->bar_config.pcie_ext_ecam2_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.pcie_ext_ecam2_bar_low;
			break;
		case AL_ADDR_MAP_PASW_PCIE_EXT_ECAM3:
			*bar_reg_high = &pbs_regs->bar_config.pcie_ext_ecam3_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.pcie_ext_ecam3_bar_low;
			break;
		case AL_ADDR_MAP_PASW_PCIE_EXT_ECAM4:
			*bar_reg_high = &pbs_regs->bar_config.pcie_ext_ecam4_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.pcie_ext_ecam4_bar_low;
			break;
		case AL_ADDR_MAP_PASW_PCIE_EXT_ECAM5:
			*bar_reg_high = &pbs_regs->bar_config.pcie_ext_ecam5_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.pcie_ext_ecam5_bar_low;
			break;
		case AL_ADDR_MAP_PASW_PCIE_EXT_ECAM6:
			*bar_reg_high = &pbs_regs->bar_config.pcie_ext_ecam6_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.pcie_ext_ecam6_bar_low;
			break;
		case AL_ADDR_MAP_PASW_PCIE_EXT_ECAM7:
			*bar_reg_high = &pbs_regs->bar_config.pcie_ext_ecam7_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.pcie_ext_ecam7_bar_low;
			break;
		case AL_ADDR_MAP_PASW_PBS_SPI:
			*bar_reg_high = &pbs_regs->bar_config.pbs_spi_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.pbs_spi_bar_low;
			break;
		case AL_ADDR_MAP_PASW_PBS_NAND:
			*bar_reg_high = &pbs_regs->bar_config.pbs_nand_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.pbs_nand_bar_low;
			break;
		case AL_ADDR_MAP_PASW_PBS_INT_MEM:
			*bar_reg_high = &pbs_regs->bar_config.pbs_int_mem_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.pbs_int_mem_bar_low;
			break;
		case AL_ADDR_MAP_PASW_PBS_BOOT:
			*bar_reg_high = &pbs_regs->bar_config.pbs_boot_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.pbs_boot_bar_low;
			break;
		case AL_ADDR_MAP_PASW_NB_INT:
			*bar_reg_high = &pbs_regs->bar_config.nb_int_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.nb_int_bar_low;
			break;
		case AL_ADDR_MAP_PASW_NB_STM:
			*bar_reg_high = &pbs_regs->bar_config.nb_stm_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.nb_stm_bar_low;
			break;
		case AL_ADDR_MAP_PASW_PCIE_INT_ECAM:
			*bar_reg_high = &pbs_regs->bar_config.pcie_ecam_int_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.pcie_ecam_int_bar_low;
			break;
		case AL_ADDR_MAP_PASW_PCIE_INT_MEM:
			*bar_reg_high = &pbs_regs->bar_config.pcie_mem_int_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.pcie_mem_int_bar_low;
			break;
		case AL_ADDR_MAP_PASW_SB_INT:
			*bar_reg_high = &pbs_regs->bar_config.sb_int_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.sb_int_bar_low;
			break;
		default:
			al_err("addr_map: unknown pasw %d\n", pasw);
			return -EINVAL;
		}
	} else {
		switch (pasw) {
		case AL_ADDR_MAP_PASW_SB_DRAM0:
			*bar_reg_high = &pbs_regs->bar_config.dram_0_sb_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.dram_0_sb_bar_low;
			break;
		case AL_ADDR_MAP_PASW_SB_DRAM1:
			*bar_reg_high = &pbs_regs->bar_config.dram_1_sb_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.dram_1_sb_bar_low;
			break;
		case AL_ADDR_MAP_PASW_SB_DRAM2:
			*bar_reg_high = &pbs_regs->bar_config.dram_2_sb_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.dram_2_sb_bar_low;
			break;
		case AL_ADDR_MAP_PASW_SB_DRAM3:
			*bar_reg_high = &pbs_regs->bar_config.dram_3_sb_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.dram_3_sb_bar_low;
			break;
		case AL_ADDR_MAP_PASW_SB_MSIX:
			*bar_reg_high = &pbs_regs->bar_config.msix_sb_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.msix_sb_bar_low;
			break;
		case AL_ADDR_MAP_PASW_PCIE_EXT_MEM0:
			*bar_reg_high = &pbs_regs->bar_config.pcie_mem0_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.pcie_mem0_bar_low;
			break;
		case AL_ADDR_MAP_PASW_PCIE_EXT_MEM1:
			*bar_reg_high = &pbs_regs->bar_config.pcie_mem1_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.pcie_mem1_bar_low;
			break;
		case AL_ADDR_MAP_PASW_PCIE_EXT_MEM2:
			*bar_reg_high = &pbs_regs->bar_config.pcie_mem2_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.pcie_mem2_bar_low;
			break;
		case AL_ADDR_MAP_PASW_PCIE_EXT_MEM3:
			*bar_reg_high = &pbs_regs->bar_config.pcie_mem3_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.pcie_mem3_bar_low;
			break;
		case AL_ADDR_MAP_PASW_PCIE_EXT_MEM4:
			*bar_reg_high = &pbs_regs->bar_config.pcie_mem4_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.pcie_mem4_bar_low;
			break;
		case AL_ADDR_MAP_PASW_PCIE_EXT_MEM5:
			*bar_reg_high = &pbs_regs->bar_config.pcie_mem5_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.pcie_mem5_bar_low;
			break;
		case AL_ADDR_MAP_PASW_PCIE_EXT_MEM6:
			*bar_reg_high = &pbs_regs->bar_config.pcie_mem6_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.pcie_mem6_bar_low;
			break;
		case AL_ADDR_MAP_PASW_PCIE_EXT_MEM7:
			*bar_reg_high = &pbs_regs->bar_config.pcie_mem7_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.pcie_mem7_bar_low;
			break;
		case AL_ADDR_MAP_PASW_PCIE_EXT_ECAM0:
			*bar_reg_high = &pbs_regs->bar_config.pcie_ext_ecam0_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.pcie_ext_ecam0_bar_low;
			break;
		case AL_ADDR_MAP_PASW_PCIE_EXT_ECAM1:
			*bar_reg_high = &pbs_regs->bar_config.pcie_ext_ecam1_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.pcie_ext_ecam1_bar_low;
			break;
		case AL_ADDR_MAP_PASW_PCIE_EXT_ECAM2:
			*bar_reg_high = &pbs_regs->bar_config.pcie_ext_ecam2_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.pcie_ext_ecam2_bar_low;
			break;
		case AL_ADDR_MAP_PASW_PCIE_EXT_ECAM3:
			*bar_reg_high = &pbs_regs->bar_config.pcie_ext_ecam3_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.pcie_ext_ecam3_bar_low;
			break;
		case AL_ADDR_MAP_PASW_PCIE_EXT_ECAM4:
			*bar_reg_high = &pbs_regs->bar_config.pcie_ext_ecam4_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.pcie_ext_ecam4_bar_low;
			break;
		case AL_ADDR_MAP_PASW_PCIE_EXT_ECAM5:
			*bar_reg_high = &pbs_regs->bar_config.pcie_ext_ecam5_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.pcie_ext_ecam5_bar_low;
			break;
		case AL_ADDR_MAP_PASW_PCIE_EXT_ECAM6:
			*bar_reg_high = &pbs_regs->bar_config.pcie_ext_ecam6_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.pcie_ext_ecam6_bar_low;
			break;
		case AL_ADDR_MAP_PASW_PCIE_EXT_ECAM7:
			*bar_reg_high = &pbs_regs->bar_config.pcie_ext_ecam7_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.pcie_ext_ecam7_bar_low;
			break;
		case AL_ADDR_MAP_PASW_PBS_SPI:
			*bar_reg_high = &pbs_regs->bar_config.pbs_spi_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.pbs_spi_bar_low;
			break;
		case AL_ADDR_MAP_PASW_PBS_NAND:
			*bar_reg_high = &pbs_regs->bar_config.pbs_nand_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.pbs_nand_bar_low;
			break;
		case AL_ADDR_MAP_PASW_PBS_INT_MEM:
			*bar_reg_high = &pbs_regs->bar_config.pbs_int_mem_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.pbs_int_mem_bar_low;
			break;
		case AL_ADDR_MAP_PASW_PBS_BOOT:
			*bar_reg_high = &pbs_regs->bar_config.pbs_boot_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.pbs_boot_bar_low;
			break;
		case AL_ADDR_MAP_PASW_NB_INT:
			*bar_reg_high = &pbs_regs->bar_config.nb_int_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.nb_int_bar_low;
			break;
		case AL_ADDR_MAP_PASW_NB_STM:
			*bar_reg_high = &pbs_regs->bar_config.nb_stm_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.nb_stm_bar_low;
			break;
		case AL_ADDR_MAP_PASW_PCIE_INT_ECAM:
			*bar_reg_high = &pbs_regs->bar_config.pcie_ecam_int_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.pcie_ecam_int_bar_low;
			break;
		case AL_ADDR_MAP_PASW_PCIE_INT_MEM:
			*bar_reg_high = &pbs_regs->bar_config.pcie_mem_int_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.pcie_mem_int_bar_low;
			break;
		case AL_ADDR_MAP_PASW_SB_INT:
			*bar_reg_high = &pbs_regs->bar_config.sb_int_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.sb_int_bar_low;
			break;
		case AL_ADDR_MAP_PASW_TRNG:
			*bar_reg_high = &pbs_regs->bar_config.trng_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.trng_bar_low;
			break;
		case AL_ADDR_MAP_PASW_ETH_SRAM_0:
			*bar_reg_high = &pbs_regs->bar_config.eth_sram_0_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.eth_sram_0_bar_low;
			break;
		case AL_ADDR_MAP_PASW_ETH_SRAM_1:
			*bar_reg_high = &pbs_regs->bar_config.eth_sram_1_bar_high;
			*bar_reg_low = &pbs_regs->bar_config.eth_sram_1_bar_low;
			break;
		default:
			al_err("addr_map: unknown pasw %d\n", pasw);
			return -EINVAL;
		}
	}

	return 0;
}

int al_addr_map_pasw_set(
		void __iomem *pbs_regs_base,
		enum al_addr_map_pasw pasw,
		al_phys_addr_t base,
		uint32_t log2size)
{
	struct al_pbs_regs *pbs_regs = (struct al_pbs_regs *)pbs_regs_base;
	uint32_t dev_id;
	uint32_t dev_rev_id;
	uint64_t size_mask;
	uint32_t base_low;
	uint32_t base_high;
	uint32_t *bar_reg_high;
	uint32_t *bar_reg_low;
	uint8_t	pasw_log2size_max;

	dev_id = al_pbs_dev_id_get(pbs_regs_base);
	dev_rev_id = al_pbs_dev_rev_id_get(pbs_regs_base);

	if (dev_id == PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V1)
		pasw_log2size_max = AL_ADDR_MAP_REV_0_1_PASW_LOG2SIZE_MAX;
	else
		pasw_log2size_max = AL_ADDR_MAP_REV_2_3_PASW_LOG2SIZE_MAX;

	if (log2size > pasw_log2size_max) {
			al_err("addr_map: max pasw log2size is %d\n", pasw_log2size_max);
		return -EINVAL;
	}

	if (log2size < AL_ADDR_MAP_PASW_LOG2SIZE_BASE) {
		al_err("addr_map: min pasw log2size is 15 (PASW disabled)\n");
		return -EINVAL;
	}

	if (log2size > AL_ADDR_MAP_PASW_LOG2SIZE_BASE) {
		size_mask = ((uint64_t)1 << log2size) - 1;
		if (base & size_mask) {
			al_err("addr_map: pasw base has to be aligned to size\n"
					"base=0x%16" PRIx64 "\n"
					"log2size=%d\n"
					"size_mask=0x%16" PRIx64 "\n",
							base,
							log2size,
							size_mask);
			return -EINVAL;
		}
	}

	/* check that base+size fit max allowed size */
	if ((base + ((uint64_t)1 << log2size)) > ((uint64_t)1 << pasw_log2size_max)) {
		al_err("addr_map: base + size doesn't fit %dbit\n", pasw_log2size_max);
		return -EINVAL;
	}

	if (al_addr_map_get_bar_regs(
			pbs_regs, dev_id, dev_rev_id, pasw, &bar_reg_high, &bar_reg_low))
		return -EINVAL;

	base_high = (uint32_t)((base >> 32) & 0xffffffff);
	base_low = (uint32_t)((base & 0xffffffff) |
			(log2size - AL_ADDR_MAP_PASW_LOG2SIZE_BASE));

	al_addr_map_latch_set(pbs_regs_base, dev_id, AL_FALSE);

	al_reg_write32(bar_reg_high, base_high);
	al_reg_write32(bar_reg_low, base_low);

	al_addr_map_latch_set(pbs_regs_base, dev_id, AL_TRUE);

	return 0;
}

int al_addr_map_pasw_get(
		void __iomem *pbs_regs_base,
		enum al_addr_map_pasw pasw,
		al_phys_addr_t *base,
		uint32_t *log2size)
{
	struct al_pbs_regs *pbs_regs = (struct al_pbs_regs *)pbs_regs_base;
	uint32_t dev_id;
	uint32_t dev_rev_id;
	uint32_t base_low;
	uint32_t base_high;
	uint32_t win_size;
	uint32_t *bar_reg_high;
	uint32_t *bar_reg_low;

	al_assert(base);
	al_assert(log2size);

	dev_id = al_pbs_dev_id_get(pbs_regs_base);
	dev_rev_id = al_pbs_dev_rev_id_get(pbs_regs_base);

	if (al_addr_map_get_bar_regs(
			pbs_regs, dev_id, dev_rev_id, pasw, &bar_reg_high, &bar_reg_low))
		return -EINVAL;

	base_high = al_reg_read32(bar_reg_high);
	base_low = al_reg_read32(bar_reg_low);

	win_size = base_low & PBS_PASW_WIN_SIZE_MASK;
	*log2size = win_size + AL_ADDR_MAP_PASW_LOG2SIZE_BASE;

	*base = ((al_phys_addr_t)base_high << 32) |
			(al_phys_addr_t)(base_low &
					PBS_PASW_BAR_LOW_ADDR_HIGH_MASK);

	return 0;
}

int al_addr_map_dram_remap_set(
		void __iomem *pbs_regs_base,
		al_phys_addr_t dram_remap_base,
		al_phys_addr_t dram_remap_transl_base,
		uint8_t window_size)
{
	struct al_pbs_regs *pbs_regs;
	uint32_t dev_id;
	uint32_t sb2nb_remap_reg_val = 0;
	uint32_t remap_base_val;
	uint32_t remap_transl_base_val;

	al_assert(window_size < 16);
	al_assert(pbs_regs_base);
	al_assert((dram_remap_base >=
			(1 << PBS_UNIT_DRAM_SRC_REMAP_BASE_ADDR_SHIFT)) ||
			(dram_remap_base == 0));


	pbs_regs = (struct al_pbs_regs *)pbs_regs_base;
	dev_id = al_pbs_dev_id_get(pbs_regs_base);

	remap_base_val =
		(dram_remap_base & PBS_UNIT_DRAM_REMAP_BASE_ADDR_MASK) >>
				PBS_UNIT_DRAM_SRC_REMAP_BASE_ADDR_SHIFT;

	remap_transl_base_val =
		(dram_remap_transl_base & PBS_UNIT_DRAM_REMAP_BASE_ADDR_MASK) >>
				PBS_UNIT_DRAM_DST_REMAP_BASE_ADDR_SHIFT;

	sb2nb_remap_reg_val =
			(remap_base_val <<
				PBS_UNIT_SB2NB_REMAP_BASE_ADDR_SHIFT) |
			(remap_transl_base_val <<
				PBS_UNIT_SB2NB_REMAP_TRANSL_BASE_ADDR_SHIFT) |
			window_size;

	al_addr_map_latch_set(pbs_regs_base, dev_id, AL_FALSE);

	if (dev_id <= PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V2)
		al_reg_write32(&pbs_regs->unit.sb2nb_cfg_dram_remap_v2, sb2nb_remap_reg_val);
	else
		al_reg_write32(&pbs_regs->bar_config.sb2nb_cfg_dram_remap, sb2nb_remap_reg_val);

	al_addr_map_latch_set(pbs_regs_base, dev_id, AL_TRUE);

	return 0;
}

static int dma_io_master_bit_idx_get(
	unsigned int			dev_id,
	enum al_addr_map_dma_io_master	master)
{
	unsigned int i;
	const struct al_addr_map_dma_io_master_map_ent *dma_io_master_map;
	unsigned int dma_io_master_map_size;

	if (dev_id <= PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V2) {
		dma_io_master_map = dma_io_master_map_v2;
		dma_io_master_map_size = AL_ARR_SIZE(dma_io_master_map_v2);
	} else {
		dma_io_master_map = dma_io_master_map_v3;
		dma_io_master_map_size = AL_ARR_SIZE(dma_io_master_map_v3);
	}

	for (i = 0; i < dma_io_master_map_size; i++)
		if (dma_io_master_map[i].master == master)
			return dma_io_master_map[i].bit_idx;

	return -EINVAL;
}

int al_addr_map_dma_io_master_cfg_set(
	void __iomem				*pbs_regs_base,
	enum al_addr_map_dma_io_master		master,
	enum al_addr_map_dma_io_master_cfg	cfg)
{
	struct al_pbs_regs *pbs_regs = (struct al_pbs_regs *)pbs_regs_base;
	unsigned int dev_id;
	int bit_idx;

	al_assert(pbs_regs_base);

	dev_id = al_pbs_dev_id_get(pbs_regs_base);

	bit_idx = dma_io_master_bit_idx_get(dev_id, master);
	if (bit_idx < 0)
		return bit_idx;

	if (dev_id <= PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V2) {
		al_reg_write32_masked(
			&pbs_regs->unit.dma_io_master_map_v2,
			AL_BIT(bit_idx + PBS_UNIT_DMA_IO_MASTER_MAP_CNTL_SHIFT) |
			AL_BIT(bit_idx + PBS_UNIT_DMA_IO_MASTER_MAP_DFLT_SLV_SHIFT),
			(cfg == AL_ADDR_MAP_DMA_IO_MASTER_CFG_DRAM_ALWAYS) ?
			AL_BIT(bit_idx + PBS_UNIT_DMA_IO_MASTER_MAP_CNTL_SHIFT) :
			(cfg == AL_ADDR_MAP_DMA_IO_MASTER_CFG_DRAM_ON_MISS) ?
			AL_BIT(bit_idx + PBS_UNIT_DMA_IO_MASTER_MAP_DFLT_SLV_SHIFT) :
			0);
	} else {
		al_reg_write32_masked(
			&pbs_regs->bar_config.dma_io_master_map1, AL_BIT(bit_idx),
			(cfg == AL_ADDR_MAP_DMA_IO_MASTER_CFG_DRAM_ON_MISS) ? AL_BIT(bit_idx) : 0);
		al_reg_write32_masked(
			&pbs_regs->bar_config.dma_io_master_map2, AL_BIT(bit_idx),
			(cfg == AL_ADDR_MAP_DMA_IO_MASTER_CFG_DRAM_ALWAYS) ? AL_BIT(bit_idx) : 0);
	}

	return 0;
}

int al_addr_map_dma_io_master_cfg_get(
	void __iomem				*pbs_regs_base,
	enum al_addr_map_dma_io_master		master,
	enum al_addr_map_dma_io_master_cfg	*cfg)
{
	struct al_pbs_regs *pbs_regs = (struct al_pbs_regs *)pbs_regs_base;
	unsigned int dev_id;
	int bit_idx;
	uint32_t reg_val;

	al_assert(pbs_regs_base);
	al_assert(cfg);

	dev_id = al_pbs_dev_id_get(pbs_regs_base);

	bit_idx = dma_io_master_bit_idx_get(dev_id, master);
	if (bit_idx < 0)
		return bit_idx;

	if (dev_id <= PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V2) {
		reg_val = al_reg_read32(&pbs_regs->unit.dma_io_master_map_v2);

		if (reg_val & AL_BIT(bit_idx + PBS_UNIT_DMA_IO_MASTER_MAP_CNTL_SHIFT))
			*cfg = AL_ADDR_MAP_DMA_IO_MASTER_CFG_DRAM_ALWAYS;
		else if (reg_val & AL_BIT(bit_idx + PBS_UNIT_DMA_IO_MASTER_MAP_DFLT_SLV_SHIFT))
			*cfg = AL_ADDR_MAP_DMA_IO_MASTER_CFG_DRAM_ON_MISS;
		else
			*cfg = AL_ADDR_MAP_DMA_IO_MASTER_CFG_ERROR_ON_MISS;
	} else {
		if (al_reg_read32(&pbs_regs->bar_config.dma_io_master_map2) & AL_BIT(bit_idx))
			*cfg = AL_ADDR_MAP_DMA_IO_MASTER_CFG_DRAM_ALWAYS;
		else if (al_reg_read32(&pbs_regs->bar_config.dma_io_master_map1) & AL_BIT(bit_idx))
			*cfg = AL_ADDR_MAP_DMA_IO_MASTER_CFG_DRAM_ON_MISS;
		else
			*cfg = AL_ADDR_MAP_DMA_IO_MASTER_CFG_ERROR_ON_MISS;
	}

	return 0;
}

/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_HIGH_RESERVED_31_8_SHIFT 8

/**** eth_fast_0_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_LOW_PCIE_6_SHIFT 28

/**** eth_fast_0_mask_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_HIGH_RESERVED_31_8_SHIFT 8

/**** eth_fast_0_mask_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_LOW_PCIE_6_SHIFT 28

/**** master_generic register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_PCIE_3_SHIFT 16
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_DRAM_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_DRAM_SHIFT 28

/**** master_generic_mask register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_PCIE_3_SHIFT 16
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_DRAM_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_DRAM_SHIFT 28

struct tgt_enforcement_slv_reg_ent {
	enum al_addr_map_tgt_enforcement_slave		slv;
	uint32_t					*val_reg;
	unsigned int					val_mask;
	unsigned int					val_shift;
	uint32_t					*mask_reg;
	unsigned int					mask_mask;
	unsigned int					mask_shift;
};

static void al_addr_map_tgt_enforcement_slv_reg_ent_get_v2(
	struct al_pbs_regs			*regs,
	enum al_addr_map_tgt_enforcement_master	mst,
	enum al_addr_map_tgt_enforcement_slave	slv,
	struct tgt_enforcement_slv_reg_ent	*ent)
{
	al_memset(ent, 0, sizeof(*ent));

	switch (slv) {
	case AL_ADDR_MAP_TGT_ENFORCEMENT_SLV_ADDR_DEC:
		ent->val_mask = PBS_TARGET_ID_ENFORCEMENT_GENERIC_NO_ENFORCEMENT_MASK;
		ent->val_shift = PBS_TARGET_ID_ENFORCEMENT_GENERIC_NO_ENFORCEMENT_SHIFT;
		ent->mask_mask = PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_NO_ENFORCEMENT_MASK;
		ent->mask_shift = PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_NO_ENFORCEMENT_SHIFT;
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_SLV_PCIE_0:
		ent->val_mask = PBS_TARGET_ID_ENFORCEMENT_GENERIC_PCIE_0_MASK;
		ent->val_shift = PBS_TARGET_ID_ENFORCEMENT_GENERIC_PCIE_0_SHIFT;
		ent->mask_mask = PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_PCIE_0_MASK;
		ent->mask_shift = PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_PCIE_0_SHIFT;
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_SLV_PCIE_1:
		ent->val_mask = PBS_TARGET_ID_ENFORCEMENT_GENERIC_PCIE_1_MASK;
		ent->val_shift = PBS_TARGET_ID_ENFORCEMENT_GENERIC_PCIE_1_SHIFT;
		ent->mask_mask = PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_PCIE_1_MASK;
		ent->mask_shift = PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_PCIE_1_SHIFT;
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_SLV_PCIE_2:
		ent->val_mask = PBS_TARGET_ID_ENFORCEMENT_GENERIC_PCIE_2_MASK;
		ent->val_shift = PBS_TARGET_ID_ENFORCEMENT_GENERIC_PCIE_2_SHIFT;
		ent->mask_mask = PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_PCIE_2_MASK;
		ent->mask_shift = PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_PCIE_2_SHIFT;
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_SLV_PCIE_3:
		ent->val_mask = PBS_TARGET_ID_ENFORCEMENT_GENERIC_PCIE_3_MASK;
		ent->val_shift = PBS_TARGET_ID_ENFORCEMENT_GENERIC_PCIE_3_SHIFT;
		ent->mask_mask = PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_PCIE_3_MASK;
		ent->mask_shift = PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_PCIE_3_SHIFT;
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_SLV_DRAM:
		ent->val_mask = PBS_TARGET_ID_ENFORCEMENT_GENERIC_DRAM_MASK;
		ent->val_shift = PBS_TARGET_ID_ENFORCEMENT_GENERIC_DRAM_SHIFT;
		ent->mask_mask = PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_DRAM_MASK;
		ent->mask_shift = PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_DRAM_SHIFT;
		break;
	default:
		al_err("%s: Invalid slave (%d)\n", __func__, slv);
		al_assert(0);
		return;
	}

	switch (mst) {
	case AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_CPU:
		ent->val_reg = &regs->target_id_enforcement_v2.cpu;
		ent->mask_reg = &regs->target_id_enforcement_v2.cpu_mask;
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_DEBUG_NB:
		ent->val_reg = &regs->target_id_enforcement_v2.debug_nb;
		ent->mask_reg = &regs->target_id_enforcement_v2.debug_nb_mask;
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_DEBUG_SB:
		ent->val_reg = &regs->target_id_enforcement_v2.debug_sb;
		ent->mask_reg = &regs->target_id_enforcement_v2.debug_sb_mask;
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_ETH_0:
		ent->val_reg = &regs->target_id_enforcement_v2.eth_0;
		ent->mask_reg = &regs->target_id_enforcement_v2.eth_0_mask;
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_ETH_1:
		ent->val_reg = &regs->target_id_enforcement_v2.eth_1;
		ent->mask_reg = &regs->target_id_enforcement_v2.eth_1_mask;
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_SATA_0:
		ent->val_reg = &regs->target_id_enforcement_v2.sata_0;
		ent->mask_reg = &regs->target_id_enforcement_v2.sata_0_mask;
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_SATA_1:
		ent->val_reg = &regs->target_id_enforcement_v2.sata_1;
		ent->mask_reg = &regs->target_id_enforcement_v2.sata_1_mask;
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_CRYPTO_0:
		ent->val_reg = &regs->target_id_enforcement_v2.crypto_0;
		ent->mask_reg = &regs->target_id_enforcement_v2.crypto_0_mask;
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_CRYPTO_1:
		ent->val_reg = &regs->target_id_enforcement_v2.crypto_1;
		ent->mask_reg = &regs->target_id_enforcement_v2.crypto_1_mask;
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_PCIE_0:
		ent->val_reg = &regs->target_id_enforcement_v2.pcie_0;
		ent->mask_reg = &regs->target_id_enforcement_v2.pcie_0_mask;
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_PCIE_1:
		ent->val_reg = &regs->target_id_enforcement_v2.pcie_1;
		ent->mask_reg = &regs->target_id_enforcement_v2.pcie_1_mask;
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_PCIE_2:
		ent->val_reg = &regs->target_id_enforcement_v2.pcie_2;
		ent->mask_reg = &regs->target_id_enforcement_v2.pcie_2_mask;
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_PCIE_3:
		ent->val_reg = &regs->target_id_enforcement_v2.pcie_3;
		ent->mask_reg = &regs->target_id_enforcement_v2.pcie_3_mask;
		break;
	default:
		al_err("%s: Invalid master (%d)\n", __func__, mst);
		al_assert(0);
		return;
	}
}

static void al_addr_map_tgt_enforcement_slv_reg_ent_get(
	struct al_pbs_regs			*regs,
	enum al_addr_map_tgt_enforcement_master	mst,
	enum al_addr_map_tgt_enforcement_slave	slv,
	struct tgt_enforcement_slv_reg_ent	*ent)
{
	al_bool is_low = AL_TRUE;

	al_memset(ent, 0, sizeof(*ent));

	switch (slv) {
	case AL_ADDR_MAP_TGT_ENFORCEMENT_SLV_ADDR_DEC:
		ent->val_mask = PBS_TARGET_ID_ENFORCEMENT_GENERIC_LOW_NO_ENFORCEMENT_MASK;
		ent->val_shift = PBS_TARGET_ID_ENFORCEMENT_GENERIC_LOW_NO_ENFORCEMENT_SHIFT;
		ent->mask_mask = PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_LOW_NO_ENFORCEMENT_MASK;
		ent->mask_shift = PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_LOW_NO_ENFORCEMENT_SHIFT;
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_SLV_PCIE_0:
		ent->val_mask = PBS_TARGET_ID_ENFORCEMENT_GENERIC_LOW_PCIE_0_MASK;
		ent->val_shift = PBS_TARGET_ID_ENFORCEMENT_GENERIC_LOW_PCIE_0_SHIFT;
		ent->mask_mask = PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_LOW_PCIE_0_MASK;
		ent->mask_shift = PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_LOW_PCIE_0_SHIFT;
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_SLV_PCIE_1:
		ent->val_mask = PBS_TARGET_ID_ENFORCEMENT_GENERIC_LOW_PCIE_1_MASK;
		ent->val_shift = PBS_TARGET_ID_ENFORCEMENT_GENERIC_LOW_PCIE_1_SHIFT;
		ent->mask_mask = PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_LOW_PCIE_1_MASK;
		ent->mask_shift = PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_LOW_PCIE_1_SHIFT;
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_SLV_PCIE_2:
		ent->val_mask = PBS_TARGET_ID_ENFORCEMENT_GENERIC_LOW_PCIE_2_MASK;
		ent->val_shift = PBS_TARGET_ID_ENFORCEMENT_GENERIC_LOW_PCIE_2_SHIFT;
		ent->mask_mask = PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_LOW_PCIE_2_MASK;
		ent->mask_shift = PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_LOW_PCIE_2_SHIFT;
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_SLV_PCIE_3:
		ent->val_mask = PBS_TARGET_ID_ENFORCEMENT_GENERIC_LOW_PCIE_3_MASK;
		ent->val_shift = PBS_TARGET_ID_ENFORCEMENT_GENERIC_LOW_PCIE_3_SHIFT;
		ent->mask_mask = PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_LOW_PCIE_3_MASK;
		ent->mask_shift = PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_LOW_PCIE_3_SHIFT;
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_SLV_PCIE_4:
		ent->val_mask = PBS_TARGET_ID_ENFORCEMENT_GENERIC_LOW_PCIE_4_MASK;
		ent->val_shift = PBS_TARGET_ID_ENFORCEMENT_GENERIC_LOW_PCIE_4_SHIFT;
		ent->mask_mask = PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_LOW_PCIE_4_MASK;
		ent->mask_shift = PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_LOW_PCIE_4_SHIFT;
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_SLV_PCIE_5:
		ent->val_mask = PBS_TARGET_ID_ENFORCEMENT_GENERIC_LOW_PCIE_5_MASK;
		ent->val_shift = PBS_TARGET_ID_ENFORCEMENT_GENERIC_LOW_PCIE_5_SHIFT;
		ent->mask_mask = PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_LOW_PCIE_5_MASK;
		ent->mask_shift = PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_LOW_PCIE_5_SHIFT;
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_SLV_PCIE_6:
		ent->val_mask = PBS_TARGET_ID_ENFORCEMENT_GENERIC_LOW_PCIE_6_MASK;
		ent->val_shift = PBS_TARGET_ID_ENFORCEMENT_GENERIC_LOW_PCIE_6_SHIFT;
		ent->mask_mask = PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_LOW_PCIE_6_MASK;
		ent->mask_shift = PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_LOW_PCIE_6_SHIFT;
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_SLV_PCIE_7:
		is_low = AL_FALSE;
		ent->val_mask = PBS_TARGET_ID_ENFORCEMENT_GENERIC_HIGH_PCIE_7_MASK;
		ent->val_shift = PBS_TARGET_ID_ENFORCEMENT_GENERIC_HIGH_PCIE_7_SHIFT;
		ent->mask_mask = PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_HIGH_PCIE_7_MASK;
		ent->mask_shift = PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_HIGH_PCIE_7_SHIFT;
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_SLV_DRAM:
		is_low = AL_FALSE;
		ent->val_mask = PBS_TARGET_ID_ENFORCEMENT_GENERIC_HIGH_DRAM_MASK;
		ent->val_shift = PBS_TARGET_ID_ENFORCEMENT_GENERIC_HIGH_DRAM_SHIFT;
		ent->mask_mask = PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_HIGH_DRAM_MASK;
		ent->mask_shift = PBS_TARGET_ID_ENFORCEMENT_GENERIC_MASK_HIGH_DRAM_SHIFT;
		break;
	default:
		al_err("%s: Invalid slave (%d)\n", __func__, slv);
		al_assert(0);
		return;
	}

	switch (mst) {
	case AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_CPU:
		if (is_low) {
			ent->val_reg = &regs->target_id_enforcement.cpu_low;
			ent->mask_reg = &regs->target_id_enforcement.cpu_mask_low;
		} else {
			ent->val_reg = &regs->target_id_enforcement.cpu_high;
			ent->mask_reg = &regs->target_id_enforcement.cpu_mask_high;
		}
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_DEBUG_NB:
		if (is_low) {
			ent->val_reg = &regs->target_id_enforcement.debug_nb_low;
			ent->mask_reg = &regs->target_id_enforcement.debug_nb_mask_low;
		} else {
			ent->val_reg = &regs->target_id_enforcement.debug_nb_high;
			ent->mask_reg = &regs->target_id_enforcement.debug_nb_mask_high;
		}
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_DEBUG_SB:
		if (is_low) {
			ent->val_reg = &regs->target_id_enforcement.debug_sb_low;
			ent->mask_reg = &regs->target_id_enforcement.debug_sb_mask_low;
		} else {
			ent->val_reg = &regs->target_id_enforcement.debug_sb_high;
			ent->mask_reg = &regs->target_id_enforcement.debug_sb_mask_high;
		}
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_ETH_FAST_0:
		if (is_low) {
			ent->val_reg = &regs->target_id_enforcement.eth_fast_0_low;
			ent->mask_reg = &regs->target_id_enforcement.eth_fast_0_mask_low;
		} else {
			ent->val_reg = &regs->target_id_enforcement.eth_fast_0_high;
			ent->mask_reg = &regs->target_id_enforcement.eth_fast_0_mask_high;
		}
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_ETH_FAST_1:
		if (is_low) {
			ent->val_reg = &regs->target_id_enforcement.eth_fast_1_low;
			ent->mask_reg = &regs->target_id_enforcement.eth_fast_1_mask_low;
		} else {
			ent->val_reg = &regs->target_id_enforcement.eth_fast_1_high;
			ent->mask_reg = &regs->target_id_enforcement.eth_fast_1_mask_high;
		}
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_ETH_FAST_2:
		if (is_low) {
			ent->val_reg = &regs->target_id_enforcement.eth_fast_2_low;
			ent->mask_reg = &regs->target_id_enforcement.eth_fast_2_mask_low;
		} else {
			ent->val_reg = &regs->target_id_enforcement.eth_fast_2_high;
			ent->mask_reg = &regs->target_id_enforcement.eth_fast_2_mask_high;
		}
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_ETH_FAST_3:
		if (is_low) {
			ent->val_reg = &regs->target_id_enforcement.eth_fast_3_low;
			ent->mask_reg = &regs->target_id_enforcement.eth_fast_3_mask_low;
		} else {
			ent->val_reg = &regs->target_id_enforcement.eth_fast_3_high;
			ent->mask_reg = &regs->target_id_enforcement.eth_fast_3_mask_high;
		}
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_ETH_DB:
		if (is_low) {
			ent->val_reg = &regs->target_id_enforcement.eth_db_low;
			ent->mask_reg = &regs->target_id_enforcement.eth_db_mask_low;
		} else {
			ent->val_reg = &regs->target_id_enforcement.eth_db_high;
			ent->mask_reg = &regs->target_id_enforcement.eth_db_mask_high;
		}
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_ETH_0:
		if (is_low) {
			ent->val_reg = &regs->target_id_enforcement.eth_0_low;
			ent->mask_reg = &regs->target_id_enforcement.eth_0_mask_low;
		} else {
			ent->val_reg = &regs->target_id_enforcement.eth_0_high;
			ent->mask_reg = &regs->target_id_enforcement.eth_0_mask_high;
		}
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_ETH_1:
		if (is_low) {
			ent->val_reg = &regs->target_id_enforcement.eth_1_low;
			ent->mask_reg = &regs->target_id_enforcement.eth_1_mask_low;
		} else {
			ent->val_reg = &regs->target_id_enforcement.eth_1_high;
			ent->mask_reg = &regs->target_id_enforcement.eth_1_mask_high;
		}
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_SATA_0:
		if (is_low) {
			ent->val_reg = &regs->target_id_enforcement.sata_0_low;
			ent->mask_reg = &regs->target_id_enforcement.sata_0_mask_low;
		} else {
			ent->val_reg = &regs->target_id_enforcement.sata_0_high;
			ent->mask_reg = &regs->target_id_enforcement.sata_0_mask_high;
		}
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_SATA_1:
		if (is_low) {
			ent->val_reg = &regs->target_id_enforcement.sata_1_low;
			ent->mask_reg = &regs->target_id_enforcement.sata_1_mask_low;
		} else {
			ent->val_reg = &regs->target_id_enforcement.sata_1_high;
			ent->mask_reg = &regs->target_id_enforcement.sata_1_mask_high;
		}
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_SATA_2:
		if (is_low) {
			ent->val_reg = &regs->target_id_enforcement.sata_2_low;
			ent->mask_reg = &regs->target_id_enforcement.sata_2_mask_low;
		} else {
			ent->val_reg = &regs->target_id_enforcement.sata_2_high;
			ent->mask_reg = &regs->target_id_enforcement.sata_2_mask_high;
		}
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_SATA_3:
		if (is_low) {
			ent->val_reg = &regs->target_id_enforcement.sata_3_low;
			ent->mask_reg = &regs->target_id_enforcement.sata_3_mask_low;
		} else {
			ent->val_reg = &regs->target_id_enforcement.sata_3_high;
			ent->mask_reg = &regs->target_id_enforcement.sata_3_mask_high;
		}
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_SATA_4:
		if (is_low) {
			ent->val_reg = &regs->target_id_enforcement.sata_4_low;
			ent->mask_reg = &regs->target_id_enforcement.sata_4_mask_low;
		} else {
			ent->val_reg = &regs->target_id_enforcement.sata_4_high;
			ent->mask_reg = &regs->target_id_enforcement.sata_4_mask_high;
		}
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_SATA_5:
		if (is_low) {
			ent->val_reg = &regs->target_id_enforcement.sata_5_low;
			ent->mask_reg = &regs->target_id_enforcement.sata_5_mask_low;
		} else {
			ent->val_reg = &regs->target_id_enforcement.sata_5_high;
			ent->mask_reg = &regs->target_id_enforcement.sata_5_mask_high;
		}
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_SATA_6:
		if (is_low) {
			ent->val_reg = &regs->target_id_enforcement.sata_6_low;
			ent->mask_reg = &regs->target_id_enforcement.sata_6_mask_low;
		} else {
			ent->val_reg = &regs->target_id_enforcement.sata_6_high;
			ent->mask_reg = &regs->target_id_enforcement.sata_6_mask_high;
		}
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_SATA_7:
		if (is_low) {
			ent->val_reg = &regs->target_id_enforcement.sata_7_low;
			ent->mask_reg = &regs->target_id_enforcement.sata_7_mask_low;
		} else {
			ent->val_reg = &regs->target_id_enforcement.sata_7_high;
			ent->mask_reg = &regs->target_id_enforcement.sata_7_mask_high;
		}
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_CRYPTO_0:
		if (is_low) {
			ent->val_reg = &regs->target_id_enforcement.crypto_0_low;
			ent->mask_reg = &regs->target_id_enforcement.crypto_0_mask_low;
		} else {
			ent->val_reg = &regs->target_id_enforcement.crypto_0_high;
			ent->mask_reg = &regs->target_id_enforcement.crypto_0_mask_high;
		}
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_CRYPTO_1:
		if (is_low) {
			ent->val_reg = &regs->target_id_enforcement.crypto_1_low;
			ent->mask_reg = &regs->target_id_enforcement.crypto_1_mask_low;
		} else {
			ent->val_reg = &regs->target_id_enforcement.crypto_1_high;
			ent->mask_reg = &regs->target_id_enforcement.crypto_1_mask_high;
		}
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_PCIE_0:
		if (is_low) {
			ent->val_reg = &regs->target_id_enforcement.pcie_0_low;
			ent->mask_reg = &regs->target_id_enforcement.pcie_0_mask_low;
		} else {
			ent->val_reg = &regs->target_id_enforcement.pcie_0_high;
			ent->mask_reg = &regs->target_id_enforcement.pcie_0_mask_high;
		}
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_PCIE_1:
		if (is_low) {
			ent->val_reg = &regs->target_id_enforcement.pcie_1_low;
			ent->mask_reg = &regs->target_id_enforcement.pcie_1_mask_low;
		} else {
			ent->val_reg = &regs->target_id_enforcement.pcie_1_high;
			ent->mask_reg = &regs->target_id_enforcement.pcie_1_mask_high;
		}
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_PCIE_2:
		if (is_low) {
			ent->val_reg = &regs->target_id_enforcement.pcie_2_low;
			ent->mask_reg = &regs->target_id_enforcement.pcie_2_mask_low;
		} else {
			ent->val_reg = &regs->target_id_enforcement.pcie_2_high;
			ent->mask_reg = &regs->target_id_enforcement.pcie_2_mask_high;
		}
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_PCIE_3:
		if (is_low) {
			ent->val_reg = &regs->target_id_enforcement.pcie_3_low;
			ent->mask_reg = &regs->target_id_enforcement.pcie_3_mask_low;
		} else {
			ent->val_reg = &regs->target_id_enforcement.pcie_3_high;
			ent->mask_reg = &regs->target_id_enforcement.pcie_3_mask_high;
		}
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_PCIE_4:
		if (is_low) {
			ent->val_reg = &regs->target_id_enforcement.pcie_4_low;
			ent->mask_reg = &regs->target_id_enforcement.pcie_4_mask_low;
		} else {
			ent->val_reg = &regs->target_id_enforcement.pcie_4_high;
			ent->mask_reg = &regs->target_id_enforcement.pcie_4_mask_high;
		}
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_PCIE_5:
		if (is_low) {
			ent->val_reg = &regs->target_id_enforcement.pcie_5_low;
			ent->mask_reg = &regs->target_id_enforcement.pcie_5_mask_low;
		} else {
			ent->val_reg = &regs->target_id_enforcement.pcie_5_high;
			ent->mask_reg = &regs->target_id_enforcement.pcie_5_mask_high;
		}
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_PCIE_6:
		if (is_low) {
			ent->val_reg = &regs->target_id_enforcement.pcie_6_low;
			ent->mask_reg = &regs->target_id_enforcement.pcie_6_mask_low;
		} else {
			ent->val_reg = &regs->target_id_enforcement.pcie_6_high;
			ent->mask_reg = &regs->target_id_enforcement.pcie_6_mask_high;
		}
		break;
	case AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_PCIE_7:
		if (is_low) {
			ent->val_reg = &regs->target_id_enforcement.pcie_7_low;
			ent->mask_reg = &regs->target_id_enforcement.pcie_7_mask_low;
		} else {
			ent->val_reg = &regs->target_id_enforcement.pcie_7_high;
			ent->mask_reg = &regs->target_id_enforcement.pcie_7_mask_high;
		}
		break;
	default:
		al_err("%s: Invalid master (%d)\n", __func__, mst);
		al_assert(0);
		return;
	}
}

void al_addr_map_tgt_enforcement_cfg_set(
	void __iomem							*pbs_regs_base,
	const struct al_addr_map_tgt_enforcement_master_cfg_entry	*entries,
	unsigned int							entries_num)
{
	unsigned int mst_idx;
	unsigned int slv_idx;
	struct al_pbs_regs *pbs_regs;
	unsigned int dev_id;

	al_assert(pbs_regs_base);
	al_assert(entries);
	al_assert(entries_num);

	pbs_regs = (struct al_pbs_regs *)pbs_regs_base;

	dev_id = al_pbs_dev_id_get(pbs_regs_base);

	al_reg_write32_masked(
		(dev_id <= PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V2) ?
		&pbs_regs->target_id_enforcement_v2.latch :
		&pbs_regs->target_id_enforcement.latch,
		PBS_TARGET_ID_ENFORCEMENT_LATCH_ENABLE, 0);

	for (mst_idx = 0; mst_idx < entries_num; mst_idx++) {
		const struct al_addr_map_tgt_enforcement_master_cfg_entry *ent = &entries[mst_idx];
		struct tgt_enforcement_slv_reg_ent slv_reg_ent;

		for (slv_idx = 0; slv_idx < ent->slv_cfg_entries_num; slv_idx++) {
			const struct al_addr_map_tgt_enforcement_slave_cfg_entry *slv_ent =
				&ent->slv_cfg_entries[slv_idx];

			if (dev_id <= PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V2)
				al_addr_map_tgt_enforcement_slv_reg_ent_get_v2(
					pbs_regs,
					ent->master,
					slv_ent->slave,
					&slv_reg_ent);
			else
				al_addr_map_tgt_enforcement_slv_reg_ent_get(
					pbs_regs,
					ent->master,
					slv_ent->slave,
					&slv_reg_ent);

			al_reg_write32_masked(
				slv_reg_ent.val_reg,
				slv_reg_ent.val_mask,
				slv_ent->axuser_19_16_val << slv_reg_ent.val_shift);
			al_reg_write32_masked(
				slv_reg_ent.mask_reg,
				slv_reg_ent.mask_mask,
				slv_ent->axuser_19_16_mask << slv_reg_ent.mask_shift);
		}
	}

	al_reg_write32_masked(
		(dev_id <= PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V2) ?
		&pbs_regs->target_id_enforcement_v2.latch :
		&pbs_regs->target_id_enforcement.latch,
		PBS_TARGET_ID_ENFORCEMENT_LATCH_ENABLE,
		PBS_TARGET_ID_ENFORCEMENT_LATCH_ENABLE);
}
