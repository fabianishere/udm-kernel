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
 * This file contains the dynamic I/O mappings for Annapurna Labs.
 * Can be used to get I/O mappings depending on Alpine version on run-time.
 */

#ifndef __AL_HAL_IOMAP_DYNAMIC_H__
#define __AL_HAL_IOMAP_DYNAMIC_H__

#include "al_hal_common.h"

/**
 * I/O types
 * Most of type names mapped to similar names in al_hal_iomap.h
 */
enum al_iomap_type {
	/* Primary Windows */
	AL_IOMAP_SB_BASE,
	AL_IOMAP_NB_BASE,

	AL_IOMAP_PBS_INT_MEM_BASE,

	/******************/
	/* SB Sub Windows */
	/******************/
	AL_IOMAP_ETH_OFFSET, /* provide an index */
	AL_IOMAP_ETH_BASE, /* provide an index */
	AL_IOMAP_ETH_NUM,

	AL_IOMAP_SB_PCIE_OFFSET,
	AL_IOMAP_SB_PCIE_BASE, /* provide an index */
	AL_IOMAP_SB_PCIE_NUM,

	AL_IOMAP_SB_PBS_BASE,

	AL_IOMAP_SB_SERDES_OFFSET,

	AL_IOMAP_SB_BASE_SRD_CMPLX_MAIN_OFFSET,
	AL_IOMAP_SB_BASE_SRD_CMPLX_SEC_OFFSET,
	AL_IOMAP_SB_BASE_SRD_CMPLX_HS_OFFSET,
	/******************/
	/* NB Sub Windows */
	/******************/

	/* DDR Controller */
	AL_IOMAP_NB_DDR_CTL_BASE,

	/* DDR PHY */
	AL_IOMAP_NB_DDR_PHY_BASE,

	/* NB Service Registers */
	AL_IOMAP_NB_SERVICE_OFFSET,

	AL_IOMAP_DDRC_BASE, /* provide an index */

	/* SB Ethernet Common Sub Windows */
	AL_IOMAP_ETH_COMMON_OFFSET,
	AL_IOMAP_ETH_COMMON_MAC_COMMON_OFFSET,

	/* SB Ethernet Sub Windows */
	AL_IOMAP_ETH_EC_OFFSET, /* provide an index */
	AL_IOMAP_ETH_EC_BASE, /* provide an index */
	AL_IOMAP_ETH_MAC_OFFSET, /* provide an index */
	AL_IOMAP_ETH_MAC_BASE, /* provide an index */
	AL_IOMAP_ETH_MAC_COMMON_BASE,
	AL_IOMAP_ETH_UDMA_OFFSET, /* provide an index & sub-index */
	AL_IOMAP_ETH_UDMA_BASE, /* provide an index & sub-index */
	AL_IOMAP_ETH_UDMA_NUM, /* provide an index */

	/* SB PBS Sub Windows */
	AL_IOMAP_I2C_PLD_BASE,
	AL_IOMAP_I2C_GEN_BASE,
	AL_IOMAP_SPI_MASTER_BASE,

	AL_IOMAP_GPIO_BASE,
	AL_IOMAP_GPIO_NUM,

	AL_IOMAP_PBS_SRAM_BASE,

	AL_IOMAP_SB_PBS_OFFSET,
	AL_IOMAP_PBS_REGFILE_OFFSET,
	AL_IOMAP_PBS_REGFILE_BASE,

	/* OTP */
	AL_IOMAP_PBS_OTP_BASE,
	AL_IOMAP_PBS_OTP_4K_BASE,

	/* SB Ring Sub Windows */
	AL_IOMAP_TEMP_SENSOR_BASE,

	AL_IOMAP_PLL_SB,
	AL_IOMAP_PLL_NB,
	AL_IOMAP_PLL_CPU,

	AL_IOMAP_PLL_BASE, /* provide an index (from above SB, NB, CPU) */

	/* SB SerDes Sub Windows */
	AL_IOMAP_SERDES_BASE, /* provide an index */

	/***************************/
	/* PBS int mem sub windows */
	/***************************/
	AL_IOMAP_PBS_INT_MEM_SRAM_BASE,

	/************************************/
	/* SRAM related addresses / offsets */
	/************************************/
	AL_IOMAP_STAGE2_ACTUAL_OFFSET_ADDR_IN_PBS_SRAM,
	AL_IOMAP_PBS_INT_MEM_SHARED_PARAMS_OFFSET,
	AL_IOMAP_SRAM_GENERAL_SHARED_DATA_OFFSET,

	AL_IOMAP_PBS_UFC_WRAP_BASE,
	AL_IOMAP_PBS_UFC_CNTL_BASE,
	AL_IOMAP_UART_BASE, /* provide an index */
	AL_IOMAP_UC_SHARED_BASE,

	/* TDM sub windows */
	AL_IOMAP_SPI_MASTER_AUX_BASE,
};

/**
 * I/O resource address according to platform with index specifiers.
 * For I/O resources that accepts index & sub-index.
 *
 * @param	dev_id
 *		Alpine version ID
 * @param	type
 *		Type of I/O resource
 * @param	idx
 *		Index of resource
 * @param	sub_idx
 *		Sub-Index of resource
 *
 * @returns	Address of I/O resource from I/O map
 */
al_phys_addr_t al_iomap_offset_idx_sub_idx_get(
	unsigned int dev_id,
	enum al_iomap_type type,
	unsigned int idx,
	unsigned int sub_idx);

/**
 * I/O resource address according to platform with index specifier.
 * For I/O resources that accepts index.
 *
 * @param	dev_id
 *		Alpine version ID
 * @param	type
 *		Type of I/O resource
 * @param	idx
 *		Index of resource
 *
 * @returns	Address of I/O resource from I/O map
 */
static inline al_phys_addr_t al_iomap_offset_idx_get(
	unsigned int dev_id,
	enum al_iomap_type type,
	unsigned int idx)
{
	return al_iomap_offset_idx_sub_idx_get(dev_id, type, idx, 0);
}

/**
 * I/O resource address according to platform
 *
 * @param	dev_id
 *		Alpine version ID
 * @param	type
 *		Type of I/O resource
 *
 * @returns	Address of I/O resource from I/O map
 */
static inline al_phys_addr_t al_iomap_offset_get(
	unsigned int dev_id,
	enum al_iomap_type type)
{
	return al_iomap_offset_idx_sub_idx_get(dev_id, type, 0, 0);
}

#endif /* __AL_HAL_IOMAP_DYNAMIC_H__ */
