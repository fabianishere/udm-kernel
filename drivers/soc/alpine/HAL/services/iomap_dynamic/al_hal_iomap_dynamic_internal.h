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

#ifndef __AL_HAL_IOMAP_DYNAMIC_INTERNAL_H__
#define __AL_HAL_IOMAP_DYNAMIC_INTERNAL_H__

#include "al_hal_common.h"
#include "al_hal_iomap_dynamic.h"

/** Internal function declarations per platform */
al_phys_addr_t al_iomap_v1_offset_idx_sub_idx_get(
	enum al_iomap_type type,
	unsigned int idx,
	unsigned int sub_idx);
al_phys_addr_t al_iomap_v2_offset_idx_sub_idx_get(
	enum al_iomap_type type,
	unsigned int idx,
	unsigned int sub_idx);
al_phys_addr_t al_iomap_v3_offset_idx_sub_idx_get(
	enum al_iomap_type type,
	unsigned int idx,
	unsigned int sub_idx);
al_phys_addr_t al_iomap_v4_offset_idx_sub_idx_get(
	enum al_iomap_type type,
	unsigned int idx,
	unsigned int sub_idx);

/**
 * I/O resource address
 *
 * @param	type
 *		Type of I/O resource
 * @param	idx
 *		Index of resource, if applicable to type
 * @param	sub_idx
 *		Sub-Index of resource, if applicable to type
 *
 * @returns	Address of I/O resource from I/O map
 */
static inline al_phys_addr_t al_iomap_internal_offset_idx_sub_idx_get(
	enum al_iomap_type type,
	unsigned int idx,
	unsigned int sub_idx)
{
	switch (type) {
	case AL_IOMAP_SB_BASE:
		return AL_SB_BASE;
	case AL_IOMAP_NB_BASE:
		return AL_NB_BASE;

	case AL_IOMAP_PBS_INT_MEM_BASE:
		return AL_PBS_INT_MEM_BASE;

	/******************/
	/* SB Sub Windows */
	/******************/
	case AL_IOMAP_ETH_OFFSET:
		return AL_ETH_OFFSET(idx);
	case AL_IOMAP_ETH_BASE:
		return AL_ETH_BASE(idx);
	case AL_IOMAP_ETH_NUM:
		return AL_ETH_NUM;

	case AL_IOMAP_SB_PCIE_OFFSET:
		return AL_SB_PCIE_OFFSET(idx);

	case AL_IOMAP_SB_PCIE_BASE:
		return AL_SB_PCIE_BASE(idx);
	case AL_IOMAP_SB_PCIE_NUM:
		return AL_SB_PCIE_NUM;

	case AL_IOMAP_SB_PBS_BASE:
		return AL_SB_PBS_BASE;

	/******************/
	/* NB Sub Windows */
	/******************/

	/* DDR Controller */
	case AL_IOMAP_NB_DDR_CTL_BASE:
		return AL_NB_DDR_CTL_BASE;

	/* DDR PHY */
	case AL_IOMAP_NB_DDR_PHY_BASE:
		return AL_NB_DDR_PHY_BASE;

	/* NB service registers */
	case AL_IOMAP_NB_SERVICE_OFFSET:
		return AL_NB_SERVICE_OFFSET;

	case AL_IOMAP_DDRC_BASE:
		return AL_DDRC_BASE(idx);

	/* SB Ethenet Common Sub Windows */
	case AL_IOMAP_ETH_COMMON_OFFSET:
		return AL_ETH_COMMON_OFFSET;

	case AL_IOMAP_ETH_COMMON_MAC_COMMON_OFFSET:
		return AL_ETH_COMMON_MAC_COMMON_OFFSET;

	/* SB Ethernet Sub Windows */
	case AL_IOMAP_ETH_EC_OFFSET:
		return AL_ETH_EC_OFFSET(idx);
	case AL_IOMAP_ETH_EC_BASE:
		return AL_ETH_EC_BASE(idx);
	case AL_IOMAP_ETH_MAC_OFFSET:
		return AL_ETH_MAC_OFFSET(idx);
	case AL_IOMAP_ETH_MAC_BASE:
		return AL_ETH_MAC_BASE(idx);
	case AL_IOMAP_ETH_MAC_COMMON_BASE:
		return AL_ETH_COMMON_MAC_COMMON_BASE;
	case AL_IOMAP_ETH_UDMA_OFFSET:
		return AL_ETH_UDMA_OFFSET(idx, sub_idx);
	case AL_IOMAP_ETH_UDMA_BASE:
		return AL_ETH_UDMA_BASE(idx, sub_idx);
	case AL_IOMAP_ETH_UDMA_NUM:
		return AL_ETH_UDMA_NUM(idx);

	/* SB PBS Sub Windows */
	case AL_IOMAP_I2C_PLD_BASE:
		return AL_I2C_PLD_BASE;
	case AL_IOMAP_I2C_GEN_BASE:
		return AL_I2C_GEN_BASE;
	case AL_IOMAP_SPI_MASTER_BASE:
		return AL_SPI_MASTER_BASE;

	case AL_IOMAP_GPIO_BASE:
		return AL_GPIO_BASE(idx);
	case AL_IOMAP_GPIO_NUM:
		return AL_GPIO_NUM;

	case AL_IOMAP_PBS_SRAM_BASE:
		return AL_PBS_SRAM_BASE;

	case AL_IOMAP_SB_PBS_OFFSET:
		return AL_SB_PBS_OFF;
	case AL_IOMAP_PBS_REGFILE_OFFSET:
		return AL_PBS_REGFILE_OFF;
	case AL_IOMAP_PBS_REGFILE_BASE:
		return AL_PBS_REGFILE_BASE;

	/* OTP */
	case AL_IOMAP_PBS_OTP_BASE:
		return AL_PBS_OTP_BASE;
	case AL_IOMAP_PBS_OTP_4K_BASE:
		return AL_PBS_OTP_4K_BASE(idx);

	/* SB Ring Sub Windows */
	case AL_IOMAP_TEMP_SENSOR_BASE:
		return AL_TEMP_SENSOR_BASE;

	case AL_IOMAP_PLL_SB:
		return AL_PLL_SB;
	case AL_IOMAP_PLL_NB:
		return AL_PLL_NB;
	case AL_IOMAP_PLL_CPU:
		return AL_PLL_CPU;

	case AL_IOMAP_PLL_BASE:
		return AL_PLL_BASE(idx);

	/* SB SerDes Sub Windows */
	case AL_IOMAP_SB_SERDES_OFFSET:
		return AL_SB_SERDES_OFFSET;
	case AL_IOMAP_SERDES_BASE:
		return AL_SERDES_BASE(idx);
	case AL_IOMAP_SB_BASE_SRD_CMPLX_MAIN_OFFSET:
		return AL_SB_SRD_CMPLX_MAIN_OFFSET;
	case AL_IOMAP_SB_BASE_SRD_CMPLX_SEC_OFFSET:
		return AL_SB_SRD_CMPLX_SEC_OFFSET;
	case AL_IOMAP_SB_BASE_SRD_CMPLX_HS_OFFSET:
		return AL_SB_SRD_CMPLX_HS_OFFSET;
	/***************************/
	/* PBS int mem sub windows */
	/***************************/
	case AL_IOMAP_PBS_INT_MEM_SRAM_BASE:
		return AL_PBS_INT_MEM_SRAM_BASE;

	/************************************/
	/* SRAM related addresses / offsets */
	/************************************/
	case AL_IOMAP_STAGE2_ACTUAL_OFFSET_ADDR_IN_PBS_SRAM:
		return STAGE2_ACTUAL_OFFSET_ADDR_IN_PBS_SRAM;
	case AL_IOMAP_PBS_INT_MEM_SHARED_PARAMS_OFFSET:
		return PBS_INT_MEM_SHARED_PARAMS_OFFSET;
	case AL_IOMAP_SRAM_GENERAL_SHARED_DATA_OFFSET:
		return SRAM_GENERAL_SHARED_DATA_OFFSET;
	case AL_IOMAP_PBS_UFC_WRAP_BASE:
		return AL_PBS_UFC_WRAP_BASE;
	case AL_IOMAP_PBS_UFC_CNTL_BASE:
		return AL_PBS_UFC_CNTL_BASE;
	case AL_IOMAP_UART_BASE:
		return AL_UART_BASE(idx);
	case AL_IOMAP_UC_SHARED_BASE:
		return AL_UC_SHARED_BASE;

	/* TDM sub windows */
	case AL_IOMAP_SPI_MASTER_AUX_BASE:
		return AL_SPI_MASTER_AUX_BASE;
	default:
		al_err("%s: Unknown I/O offset\n", __func__);
		al_assert(0);
	}

	return 0;
}

#endif /* __AL_HAL_IOMAP_DYNAMIC_INTERNAL_H__ */
