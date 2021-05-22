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
 * @defgroup group_sys_fabric_pasw System Fabric PASW
 * @ingroup group_nb
 *  The System Fabric PASW HAL can be used to adjust the Physical Address Slave
 *  Window (PASW) configuration.
 *
 *  @{
 * @file   al_hal_sys_fabric_pasw.h
 *
 * @brief  includes System Fabric PASW API
 *
 */

#ifndef __AL_HAL_SYS_FABRIC_PASW_H__
#define __AL_HAL_SYS_FABRIC_PASW_H__

#include "al_hal_common.h"

/** Physical address slave windows */
enum al_sys_fabric_pasw {
	AL_SYS_FABRIC_PASW_DDR_CPU_BAR0,
	AL_SYS_FABRIC_PASW_DDR_CPU_BAR1,
	AL_SYS_FABRIC_PASW_DDR_CPU_BAR2,
	AL_SYS_FABRIC_PASW_DDR_CPU_BAR3,
	AL_SYS_FABRIC_PASW_DDR_CPU_BAR4,
	AL_SYS_FABRIC_PASW_DDR_CPU_BAR5,
	AL_SYS_FABRIC_PASW_DDR_IO_BAR0,
	AL_SYS_FABRIC_PASW_DDR_IO_BAR1,
	AL_SYS_FABRIC_PASW_DDR_IO_BAR2,
	AL_SYS_FABRIC_PASW_DDR_IO_BAR3,
	AL_SYS_FABRIC_PASW_DDR_IO_BAR4,
	AL_SYS_FABRIC_PASW_DDR_IO_BAR5,
	AL_SYS_FABRIC_PASW_MSIX_BAR,
};

/** DRAM channel */
enum al_sys_fabric_pasw_dram_ch {
	/** DRAM 0 */
	AL_SYS_FABRIC_PASW_DRAM_CH_DRAM0 = 1,
	/** DRAM 1 */
	AL_SYS_FABRIC_PASW_DRAM_CH_DRAM1 = 2,
	/** Interleave DRAM 0 / DRAM 1 */
	AL_SYS_FABRIC_PASW_DRAM_CH_INTRLVD = 3,
};

/** Target memory */
enum al_sys_fabric_pasw_tgt_mem {
	/** DRAM */
	AL_SYS_FABRIC_PASW_TGT_MEM_DRAM = 0,
	/** SRAM */
	AL_SYS_FABRIC_PASW_TGT_MEM_SRAM = 1,
};

/** PASW configuration */
struct al_sys_fabric_pasw_cfg {
	/** Base address for the PASW */
	al_phys_addr_t			base;

	/** Log2 of the PASW size, has to be >=15 and <=48 (15 - disabled) */
	unsigned int			log2size;

	/** DRAM channel */
	enum al_sys_fabric_pasw_dram_ch	ch;

	/** Target memory */
	enum al_sys_fabric_pasw_tgt_mem	tgt_mem;
};

/**
 * Update Physical Slave Address Window (PASW) in system address map
 *
 * This function is used to update a certain PASW in the system address map
 *
 * @param	nb_regs_base
 *		The address of the System Fabric Regfile
 * @param	pasw
 *		The PASW to update
 * @param	cfg
 *		The required PASW configuration
 *
 * @return	0 upon success
 */
int al_sys_fabric_pasw_set(
	void __iomem			*nb_regs_base,
	enum al_sys_fabric_pasw		pasw,
	struct al_sys_fabric_pasw_cfg	*cfg);

/**
 * Get Physical Slave Address Window (PASW) on the system address map
 *
 * This function is used to get a certain PASW on the system address map
 *
 * @param	nb_regs_base
 *		The address of the System Fabric Regfile
 * @param	pasw
 *		Required PASW
 * @param	cfg
 *		The PASW configuration
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred
 */
int al_sys_fabric_pasw_get(
	void __iomem			*nb_regs_base,
	enum al_sys_fabric_pasw		pasw,
	struct al_sys_fabric_pasw_cfg	*cfg);

/**
 * Set DRAM remapping
 *
 * This function is used for remapping the DRAM.
 * By default the 4th DRAM GB is accessed through
 * system addresses 2_4000_0000 - 2_8000_0000.
 * This function can be used for changing this default behavior.
 *
 * @param	nb_regs_base
 *		The address of the System Fabric Regfile
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
 *		This field determines the remaping window size.
 *		Maximum allowed to remaping window size is of 1TByte.
 *		Following values should be used to define window size:
 *			0  - window is disabled.
 *			1  - Window size is 512MByte.
 *			2  - Window size is 1GByte.
 *			3  - Window size is 2GByte.
 *			4  - Window size is 4GByte.
 *			5  - Window size is 8GByte.
 *			6  - Window size is 16GByte.
 *			7  - Window size is 32GByte.
 *			8  - Window size is 64GByte.
 *			9  - Window size is 128GByte.
 *			10 - Window size is 256GByte.
 *			11 - Window size is 512GByte.
 *			12 - Window size is 1TByte.
 *		Any other values are prohibited
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred
 */
int al_sys_fabric_pasw_dram_remap_set(
	void __iomem	*nb_regs_base,
	al_phys_addr_t	dram_remap_base,
	al_phys_addr_t	dram_remap_transl_base,
	unsigned int	window_size);

/**
 * Set on which granularity the DRAM's are interleaved
 *
 * @param	nb_regs_base
 *		The address of the System Fabric Regfile
 * @param	bit
 *		Address bit
 *		0 - no interleaving
 *		6, 7, 8..., 20 - address bit number
 */
void al_sys_fabric_pasw_dram_intrlv_stripe_bit_set(
	void __iomem	*nb_regs_base,
	unsigned int	bit);

/**
 * Selects on which bits the CCU does hashing to select the partition.
 *
 * @param	nb_regs_base
 *		The address of the System Fabric Regfile
 * @param	val
 *		Data for reg hash_2_low
 */
void al_sys_fabric_ccu_hash_2_low_set(
	void __iomem	*nb_regs_base,
	unsigned int	val);

/**
 * Get DRAM interleaving granularity (stripe bit)
 *
 * @param	nb_regs_base
 *		The address of the System Fabric Regfile
 * @return	Address bit
 *		0 - no interleaving
 *		6, 7, 8..., 20 - address bit number
 */
unsigned int al_sys_fabric_pasw_dram_intrlv_stripe_bit_get(
	void __iomem    *nb_regs_base);

#endif

/** @} */
