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
 * @defgroup group_addr_map Address Map
 * @ingroup group_pbs
 *  The Address Map HAL can be used to adjust the Physical Address Slave
 *  Window (PASW) configuration.
 *
 *  @{
 * @file   al_hal_addr_map.h
 *
 * @brief  includes Address Map API
 *
 */

#ifndef __AL_HAL_ADDR_MAP_H__
#define __AL_HAL_ADDR_MAP_H__

#include "al_hal_common.h"

/* enum definitions */

/* Address map physical address slave windows */
enum al_addr_map_pasw {
	AL_ADDR_MAP_PASW_NB_DRAM0, /**< DRAM (system fabric) */
	AL_ADDR_MAP_PASW_NB_DRAM1, /**< DRAM (system fabric) */
	AL_ADDR_MAP_PASW_NB_DRAM2, /**< DRAM (system fabric) */
	AL_ADDR_MAP_PASW_NB_DRAM3, /**< DRAM (system fabric) */
	AL_ADDR_MAP_PASW_NB_MSIX, /**< MSI-X Target Address region, to which
					all the MSI-X messages are forwarded.
					(system fabric) */
	AL_ADDR_MAP_PASW_SB_DRAM0, /**< DRAM (io fabric) */
	AL_ADDR_MAP_PASW_SB_DRAM1, /**< DRAM (io fabric) */
	AL_ADDR_MAP_PASW_SB_DRAM2, /**< DRAM (io fabric) */
	AL_ADDR_MAP_PASW_SB_DRAM3, /**< DRAM (io fabric) */
	AL_ADDR_MAP_PASW_SB_MSIX, /**< MSI-X Target Address region, to which
					all the MSI-X messages are forwarded.
					(io fabric)*/
	AL_ADDR_MAP_PASW_PCIE_EXT_MEM0, /**< PCI Express interface 1 memory
					mapped space*/
	AL_ADDR_MAP_PASW_PCIE_EXT_MEM1,	/**< PCI Express interface 2 memory
						mapped space*/
	AL_ADDR_MAP_PASW_PCIE_EXT_MEM2, /**< PCI Express interface 3 memory
					mapped space*/
	AL_ADDR_MAP_PASW_PCIE_EXT_MEM3, /**< PCI Express interface 4 memory
					mapped space*/
	AL_ADDR_MAP_PASW_PCIE_EXT_MEM4, /**< PCI Express interface 5 memory
					mapped space*/
	AL_ADDR_MAP_PASW_PCIE_EXT_MEM5, /**< PCI Express interface 6 memory
					mapped space*/
	AL_ADDR_MAP_PASW_PCIE_EXT_MEM6, /**< PCI Express interface 7 memory
					mapped space*/
	AL_ADDR_MAP_PASW_PCIE_EXT_MEM7, /**< PCI Express interface 8 memory
					mapped space*/
	AL_ADDR_MAP_PASW_PCIE_EXT_ECAM0, /**< PCI Express interface 1 – ECAM,
				i.e. memory-mapped configuration space */
	AL_ADDR_MAP_PASW_PCIE_EXT_ECAM1, /**< PCI Express interface 2 – ECAM,
				i.e. memory-mapped configuration space */
	AL_ADDR_MAP_PASW_PCIE_EXT_ECAM2, /**< PCI Express interface 3 – ECAM,
				i.e. memory-mapped configuration space */
	AL_ADDR_MAP_PASW_PCIE_EXT_ECAM3, /**< PCI Express interface 4 – ECAM,
				i.e. memory-mapped configuration space */
	AL_ADDR_MAP_PASW_PCIE_EXT_ECAM4, /**< PCI Express interface 5 – ECAM,
				i.e. memory-mapped configuration space */
	AL_ADDR_MAP_PASW_PCIE_EXT_ECAM5, /**< PCI Express interface 6 – ECAM,
				i.e. memory-mapped configuration space */
	AL_ADDR_MAP_PASW_PCIE_EXT_ECAM6, /**< PCI Express interface 7 – ECAM,
				i.e. memory-mapped configuration space */
	AL_ADDR_MAP_PASW_PCIE_EXT_ECAM7, /**< PCI Express interface 8 – ECAM,
				i.e. memory-mapped configuration space */
	AL_ADDR_MAP_PASW_PBS_SPI, /**< PBS SPI */
	AL_ADDR_MAP_PASW_PBS_NAND, /**< PBS NAND */
	AL_ADDR_MAP_PASW_PBS_NOR, /**< PBS NOR */
	AL_ADDR_MAP_PASW_PBS_INT_MEM, /**< PBS Boot ROM and SRAM */
	AL_ADDR_MAP_PASW_PBS_BOOT, /**< Boot address as specified by ARM
				architecture*/
	AL_ADDR_MAP_PASW_NB_INT, /**< System Fabric Internal registers */
	AL_ADDR_MAP_PASW_NB_STM, /**< System Fabric System Trace Module (STM) */
	AL_ADDR_MAP_PASW_PCIE_INT_ECAM, /**< Internal virtual PCIe bus – ECAM,
				i.e. memory-mapped configuration space */
	AL_ADDR_MAP_PASW_PCIE_INT_MEM, /**< Internal virtual PCIe bus
				memory-mapped address space */
	AL_ADDR_MAP_PASW_SB_INT, /**< IO Fabric Internal registers */
	AL_ADDR_MAP_PASW_TRNG, /**< TRNG regs */
	AL_ADDR_MAP_PASW_ETH_SRAM_0,	/**< Ethernet SRAM 0 */
	AL_ADDR_MAP_PASW_ETH_SRAM_1,	/**< Ethernet SRAM 1 */
};

/* DMA-IO masters */
enum al_addr_map_dma_io_master {
	AL_ADDR_MAP_DMA_IO_MASTER_CRYPTO_0,
	AL_ADDR_MAP_DMA_IO_MASTER_CRYPTO_1,
	AL_ADDR_MAP_DMA_IO_MASTER_ETH_0,
	AL_ADDR_MAP_DMA_IO_MASTER_ETH_1,
	AL_ADDR_MAP_DMA_IO_MASTER_ETH_2,
	AL_ADDR_MAP_DMA_IO_MASTER_ETH_3,
	AL_ADDR_MAP_DMA_IO_MASTER_ETH_4,	/* Alpine V3 onwards */
	AL_ADDR_MAP_DMA_IO_MASTER_ETH_5,	/* Alpine V3 onwards */
	AL_ADDR_MAP_DMA_IO_MASTER_ETH_DB,	/* Alpine V3 onwards */
	AL_ADDR_MAP_DMA_IO_MASTER_FDMA_0,	/* Alpine V3 onwards */
	AL_ADDR_MAP_DMA_IO_MASTER_FDMA_1,	/* Alpine V3 onwards */
	AL_ADDR_MAP_DMA_IO_MASTER_SATA_0,
	AL_ADDR_MAP_DMA_IO_MASTER_SATA_1,
	AL_ADDR_MAP_DMA_IO_MASTER_SATA_2,	/* Alpine V3 onwards */
	AL_ADDR_MAP_DMA_IO_MASTER_SATA_3,	/* Alpine V3 onwards */
	AL_ADDR_MAP_DMA_IO_MASTER_SATA_4,	/* Alpine V3 onwards */
	AL_ADDR_MAP_DMA_IO_MASTER_SATA_5,	/* Alpine V3 onwards */
	AL_ADDR_MAP_DMA_IO_MASTER_SATA_6,	/* Alpine V3 onwards */
	AL_ADDR_MAP_DMA_IO_MASTER_SATA_7,	/* Alpine V3 onwards */
	AL_ADDR_MAP_DMA_IO_MASTER_PCIE_0,
	AL_ADDR_MAP_DMA_IO_MASTER_PCIE_1,
	AL_ADDR_MAP_DMA_IO_MASTER_PCIE_2,
	AL_ADDR_MAP_DMA_IO_MASTER_PCIE_3,
	AL_ADDR_MAP_DMA_IO_MASTER_PCIE_4,	/* Alpine V3 onwards */
	AL_ADDR_MAP_DMA_IO_MASTER_PCIE_5,	/* Alpine V3 onwards */
	AL_ADDR_MAP_DMA_IO_MASTER_PCIE_6,	/* Alpine V3 onwards */
	AL_ADDR_MAP_DMA_IO_MASTER_PCIE_7,	/* Alpine V3 onwards */
	AL_ADDR_MAP_DMA_IO_MASTER_SPI_DBG,
	AL_ADDR_MAP_DMA_IO_MASTER_CPU_DBG,
};

/* DMA-IO master configuration */
enum al_addr_map_dma_io_master_cfg {
	/* If transactions doesn't hit any window, error is condition is raised (default) */
	AL_ADDR_MAP_DMA_IO_MASTER_CFG_ERROR_ON_MISS,
	/* If transactions doesn't hit any window, transaction is sent to DRAM */
	AL_ADDR_MAP_DMA_IO_MASTER_CFG_DRAM_ON_MISS,
	/* Transaction is sent to DRAM regardless of a window hit */
	AL_ADDR_MAP_DMA_IO_MASTER_CFG_DRAM_ALWAYS,
};

/**
 * Update Physical Slave Address Window (PASW) in system address map
 *
 * This function is used to update a certain PASW in the system address map
 *
 * @param	pbs_regs_base
 *		The address of the PBS Regfile
 * @param	pasw
 *		The PASW to update
 * @param	base
 *		Base address for the PASW
 * @param	log2size
 *		Log2 of the PASW size, has to be >=15 and <=40.
 *		If log2size==15, the PASW is disabled.
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred
 */
int al_addr_map_pasw_set(
		void __iomem *pbs_regs_base,
		enum al_addr_map_pasw pasw,
		al_phys_addr_t base,
		uint32_t log2size);

/**
 * Get Physical Slave Address Window (PASW) on the system address map
 *
 * This function is used to get a certain PASW on the system address map
 *
 * @param	pbs_regs_base
 *		The address of the PBS Regfile
 * @param	pasw
 *		Required PASW
 * @param	*base
 *		returned base address
 * @param	*log2size
 *		returned Log2 of the PASW size
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred
 */
int al_addr_map_pasw_get(
		void __iomem *pbs_regs_base,
		enum al_addr_map_pasw pasw,
		al_phys_addr_t *base,
		uint32_t *log2size);

/**
 * Set DRAM remapping
 *
 * This function is used for remapping the DRAM.
 * By default the 4th DRAM GB is accessed through
 * system addresses 2_4000_0000 - 2_8000_0000.
 * This function can be used for changing this default behavior.
 *
 * @param	pbs_regs_base
 *		The address of the PBS Regfile
 * @param	dram_remap_base
 *		Remaping source DRAM area base address.
 *		Source DRAM area should be any area beginning from 0x20000000
 *		and up 40Bit addressing
 *		Any area below this address will be undestood by system
 *		as area that begins from address 0, that defined as system area
 * @param	dram_remap_transl_base
 *		Remaping destination DRAM translated area base address.
 *		Destination DRAM area should be any area beginning from 0x20000000
 *		and up 40Bit addressing
 *		Any area below this address will be undestood by system
 *		as area that begins from address 0, that defined as system area
 *		Destination area depends from system installed memory (DRAM)
 * @param	window_size
 * 		This field determines the remaping window size.
 * 		Maximum allowed to remaping window size is of 2GByte.
 * 		Following values should be used to define window size:
 * 			0 - window is disabled.
 * 			1 - Window size is 512MByte.
 * 			2 - Window size is 1GByte.
 * 			3 - Window size is 2GByte.
 *		Any other values are prohibited
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred
 */
int al_addr_map_dram_remap_set(
		void __iomem *pbs_regs_base,
		al_phys_addr_t dram_remap_base,
		al_phys_addr_t dram_remap_transl_base,
		uint8_t window_size);

/**
 * DMA-IO master configuration set
 *
 * @param	pbs_regs_base
 *		The address of the PBS Regfile
 * @param	master
 *		The DMA-IO master to configure
 * @param	cfg
 *		The required configuration for this master
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred
 */
int al_addr_map_dma_io_master_cfg_set(
	void __iomem				*pbs_regs_base,
	enum al_addr_map_dma_io_master		master,
	enum al_addr_map_dma_io_master_cfg	cfg);

/**
 * DMA-IO master configuration get
 *
 * @param	pbs_regs_base
 *		The address of the PBS Regfile
 * @param	master
 *		The DMA-IO master to configure
 * @param	cfg
 *		The current configuration for this master
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred
 */
int al_addr_map_dma_io_master_cfg_get(
	void __iomem				*pbs_regs_base,
	enum al_addr_map_dma_io_master		master,
	enum al_addr_map_dma_io_master_cfg	*cfg);

/** Target enforcement masters */
enum al_addr_map_tgt_enforcement_master {
	AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_CPU,
	AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_DEBUG_NB,
	AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_DEBUG_SB,
	AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_ETH_FAST_0,
	AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_ETH_FAST_1,
	AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_ETH_FAST_2,
	AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_ETH_FAST_3,
	AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_ETH_DB,
	AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_ETH_0,
	AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_ETH_1,
	AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_SATA_0,
	AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_SATA_1,
	AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_SATA_2,
	AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_SATA_3,
	AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_SATA_4,
	AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_SATA_5,
	AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_SATA_6,
	AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_SATA_7,
	AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_CRYPTO_0,
	AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_CRYPTO_1,
	AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_PCIE_0,
	AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_PCIE_1,
	AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_PCIE_2,
	AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_PCIE_3,
	AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_PCIE_4,
	AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_PCIE_5,
	AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_PCIE_6,
	AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_PCIE_7,
};

/** Target enforcement slaves */
enum al_addr_map_tgt_enforcement_slave {
	AL_ADDR_MAP_TGT_ENFORCEMENT_SLV_ADDR_DEC,
	AL_ADDR_MAP_TGT_ENFORCEMENT_SLV_DRAM,
	AL_ADDR_MAP_TGT_ENFORCEMENT_SLV_PCIE_0,
	AL_ADDR_MAP_TGT_ENFORCEMENT_SLV_PCIE_1,
	AL_ADDR_MAP_TGT_ENFORCEMENT_SLV_PCIE_2,
	AL_ADDR_MAP_TGT_ENFORCEMENT_SLV_PCIE_3,
	AL_ADDR_MAP_TGT_ENFORCEMENT_SLV_PCIE_4,
	AL_ADDR_MAP_TGT_ENFORCEMENT_SLV_PCIE_5,
	AL_ADDR_MAP_TGT_ENFORCEMENT_SLV_PCIE_6,
	AL_ADDR_MAP_TGT_ENFORCEMENT_SLV_PCIE_7,
};

/** Target enforcement slave configuration entry */
struct al_addr_map_tgt_enforcement_slave_cfg_entry {
	/** ID of the slave to which the transactions should be mapped to */
	enum al_addr_map_tgt_enforcement_slave	slave;
	/** axuser[19:16] value to be used for mapping transactions from the master to this slave */
	unsigned int				axuser_19_16_val;
	/** axuser[19:16] mask to be used for mapping transactions from the master to this slave */
	unsigned int				axuser_19_16_mask;
};

/** Target enforcement master configuration entry */
struct al_addr_map_tgt_enforcement_master_cfg_entry {
	/** ID of the master for which the configuration applies */
	enum al_addr_map_tgt_enforcement_master				master;
	/** Slave configurations */
	const struct al_addr_map_tgt_enforcement_slave_cfg_entry	*slv_cfg_entries;
	/** Number of slave configuration entries */
	unsigned int							slv_cfg_entries_num;
};

/**
 * Target enforcement master configuration setting
 *
 * @param	pbs_regs_base
 *		The address of the PBS Regfile
 * @param	entries
 *		Master configuration entries
 * @param	entries_num
 *		Number of master configuration entries
 */
void al_addr_map_tgt_enforcement_cfg_set(
	void __iomem							*pbs_regs_base,
	const struct al_addr_map_tgt_enforcement_master_cfg_entry	*entries,
	unsigned int							entries_num);

#endif /*__AL_HAL_ADDR_MAP_H__ */

/** @} end of Address Map group */
