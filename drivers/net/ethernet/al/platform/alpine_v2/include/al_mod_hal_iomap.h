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
 * This file contains the default I/O mappings for Annapurna Labs
 */

#ifndef __AL_HAL_IOMAP_H__
#define __AL_HAL_IOMAP_H__

/* Primary Windows */
#define AL_PCIE_0_BASE		0xc0000000UL
#define AL_PCIE_0_SIZE		SZ_128M
#define AL_PCIE_1_BASE		0xc8000000UL
#define AL_PCIE_1_SIZE		SZ_128M
#define AL_PCIE_2_BASE		0xd0000000UL
#define AL_PCIE_2_SIZE		SZ_128M
#define AL_PCIE_3_BASE		0xd8000000UL
#define AL_PCIE_3_SIZE		SZ_128M

#define AL_PCIE_BASE(idx)	(\
	((idx) == 0) ? AL_PCIE_0_BASE :	\
	((idx) == 1) ? AL_PCIE_1_BASE :	\
	((idx) == 2) ? AL_PCIE_2_BASE :	\
	((idx) == 3) ? AL_PCIE_3_BASE :	\
	0)

#define AL_PCIE_SIZE(idx)	(\
	((idx) == 0) ? AL_PCIE_0_SIZE :	\
	((idx) == 1) ? AL_PCIE_1_SIZE :	\
	((idx) == 2) ? AL_PCIE_2_SIZE :	\
	((idx) == 3) ? AL_PCIE_3_SIZE :	\
	0)

#define AL_NOR_BASE		0xf4000000UL
#define AL_SPI_BASE		0xf8000000UL
#define AL_NAND_BASE		0xfa100000UL
#define AL_SB_BASE		0xfc000000UL
#define AL_SB_SIZE		SZ_32M
#define AL_NB_BASE		0xf0000000UL
#define AL_NB_SIZE		SZ_32M
#define AL_LL_SRAM_BASE		0xf2000000UL
#define AL_LL_SRAM_SIZE		SZ_256K
#define AL_ADV_ETH_SRAM_BASE	0xf2200000UL
#define AL_ADV_ETH_SRAM_SIZE	SZ_1M

#define AL_ETH_SRAM_TYPE_ADV	0
#define AL_ETH_SRAM_BASE(unit, type)		\
	(((type) != AL_ETH_SRAM_TYPE_ADV) ? 0 :	\
	(((unit) == 0) ? 0xf2200000UL :		\
	(((unit) == 2) ? 0xf2280000UL : 0)))
#define AL_ETH_SRAM_SIZE(unit, type)		\
	(((type) != AL_ETH_SRAM_TYPE_ADV) ? 0 :	(416 * SZ_1K))

#define AL_TDM_AXI_BASE		0xf2300000UL
#define AL_TDM_AXI_SIZE		SZ_128K
#define AL_PCIE_0_ECAM_BASE	0xfb600000UL
#define AL_PCIE_0_ECAM_SIZE	SZ_1M
#define AL_PCIE_1_ECAM_BASE	0xfb700000UL
#define AL_PCIE_1_ECAM_SIZE	SZ_1M
#define AL_PCIE_2_ECAM_BASE	0xfb800000UL
#define AL_PCIE_2_ECAM_SIZE	SZ_1M
#define AL_PCIE_3_ECAM_BASE	0xfb900000UL
#define AL_PCIE_3_ECAM_SIZE	SZ_1M

#define AL_PCIE_ECAM_BASE(idx)	(\
	((idx) == 0) ? AL_PCIE_0_ECAM_BASE :	\
	((idx) == 1) ? AL_PCIE_1_ECAM_BASE :	\
	((idx) == 2) ? AL_PCIE_2_ECAM_BASE :	\
	((idx) == 3) ? AL_PCIE_3_ECAM_BASE :	\
	0)

#define AL_PCIE_ECAM_SIZE(idx)	(\
	((idx) == 0) ? AL_PCIE_0_ECAM_SIZE :	\
	((idx) == 1) ? AL_PCIE_1_ECAM_SIZE :	\
	((idx) == 2) ? AL_PCIE_2_ECAM_SIZE :	\
	((idx) == 3) ? AL_PCIE_3_ECAM_SIZE :	\
	0)

#define AL_PCIE_INT_ECAM_BASE	0xfbc00000UL
#define AL_PCIE_INT_ECAM_SIZE	SZ_1M
#define AL_PCIE_INT_BASE	0xfe000000UL
#define AL_PCIE_INT_SIZE	SZ_16M

#define AL_MSIX_SPACE_BASE_LOW	0xfbe00000UL
#define AL_MSIX_SPACE_BASE_HIGH	0x0UL

#define AL_PBS_INT_MEM_BASE	0xfbff0000UL

#define AL_DRAM_2_HIGH		0x00000001UL	/* 4GB - 8GB */
#define AL_DRAM_3_HIGH		0x00000002UL	/* 3GB - 4GB */

#define AL_CPU_PUSH_PACKET_BASE	0xfb100000UL

/******************/
/* SB Sub Windows */
/******************/
#define AL_ETH_IS_ADV(idx)	((idx) % 2 ? 0 : 1)
#define AL_ETH_OFFSET(idx)	((idx) * 0x00100000)
#define AL_ETH_BASE(idx)	(AL_SB_BASE + AL_ETH_OFFSET(idx))
#define AL_ETH_NUM			4
#define AL_ETH_DEV_NUM(idx)	(idx)

#define AL_SSM_DEV_NUM(idx)	(((idx) == 0) ? 4 : 5)
#define AL_SSM_OFFSET(idx)	(AL_SSM_DEV_NUM(idx) * 0x00100000)
#define AL_SSM_BASE(idx)	(AL_SB_BASE + AL_SSM_OFFSET(idx))
#define AL_SSM_NUM		2

#define AL_CRYPTO_DEV_NUM	AL_SSM_DEV_NUM(0)
#define AL_CRYPTO_BASE		AL_SSM_BASE(0)

#define AL_RAID_DEV_NUM		AL_SSM_DEV_NUM(1)
#define AL_RAID_BASE		AL_SSM_BASE(1)

#define AL_SATA_OFFSET(idx)	(0x800000 + ((idx) * 0x100000))
#define AL_SATA_BASE(idx)	(AL_SB_BASE + AL_SATA_OFFSET(idx))
#define AL_SATA_NUM		2
#define AL_SATA_DEV_NUM(idx)	(8 + (idx))

#define AL_SB_PCIE_OFFSET(idx)	(((idx) < 3) ? \
				(0x01800000 + ((idx) * 0x20000)) : \
				(0x01900000 + ((idx % 3) * 0x20000)))
#define AL_SB_PCIE_BASE(idx)	(AL_SB_BASE + AL_SB_PCIE_OFFSET(idx))

#define AL_SB_PCIE_NUM		4

#define AL_SB_RING_BASE		(AL_SB_BASE + 0x01860000)

#define AL_SB_PBS_OFF	(0x01880000)
#define AL_SB_PBS_BASE		(AL_SB_BASE + AL_SB_PBS_OFF)

#define AL_SB_SERDES_OFFSET	0x018c0000
#define AL_SB_SERDES_BASE	(AL_SB_BASE + AL_SB_SERDES_OFFSET)

#define AL_SB_DFX_BASE		(AL_SB_BASE + 0x018e0000)
#define AL_SB_DFX_BASE_ASM	0xfd8e0000

/* Only available for AL V3, define a dummy value */
#define AL_SB_SRD_CMPLX_MAIN_OFFSET	0
#define AL_SB_SRD_CMPLX_SEC_OFFSET	0
#define AL_SB_SRD_CMPLX_HS_OFFSET	0

#define AL_SB_SRD_CMPLX_MAIN_BASE	0
#define AL_SB_SRD_CMPLX_SEC_BASE	0
#define AL_SB_SRD_CMPLX_HS_BASE		0

/******************/
/* NB Sub Windows */
/******************/

/* NB main / secondary GICs and their Sub Windows*/
#define AL_NB_GIC_MAIN		0
#define AL_NB_GIC_SECONDARY	1

#define AL_NB_GIC_DIST_BASE_MAIN	0xf0200000
#define AL_NB_GIC_DIST_BASE(id) (AL_NB_BASE + (((id) == AL_NB_GIC_MAIN) ? 0x200000 : 0x9000))
#define AL_NB_GIC_CPU_BASE(id)  (AL_NB_BASE + (((id) == AL_NB_GIC_MAIN) ? 0x100000 : 0xa000))

#define AL_NB_GIC_REDIST_BASE(id, cpu)	\
	(AL_NB_BASE + (((id) == AL_NB_GIC_MAIN) ? (0x280000 + (cpu)*0x20000) : 0x9000))

#define AL_NB_GIC_REDIST_SGI_BASE(id, cpu) (AL_NB_GIC_REDIST_BASE(id, cpu) + 0x10000)

#define AL_NB_IOMMU_NUM		0

/* NB service registers */
#define AL_NB_SERVICE_OFFSET	0x70000
#define AL_NB_SERVICE_BASE	(AL_NB_BASE + AL_NB_SERVICE_OFFSET)

/* DDR Controller */
#define AL_NB_DDR_CTL_BASE	(AL_NB_BASE + 0x00080000)
#define AL_DDR_CORE_BASE(idx)	AL_NB_DDR_CTL_BASE

/* DDR PHY */
#define AL_NB_DDR_PHY_BASE	(AL_NB_BASE + 0x00088000)
#define AL_DDR_PHY_BASE(idx)	AL_NB_DDR_PHY_BASE

/* DDR Controller service */
#define AL_DDRC_BASE(idx)	0

#define AL_DDR_NUM		1

/* CCI Controller */
#define AL_NB_CCU_OFFSET	0x90000
#define AL_NB_CCI_BASE		(AL_NB_BASE + AL_NB_CCU_OFFSET)
#define AL_NB_CCI_SNOOP_FILTER	0

/* ANPA Cluster Controller */
#define AL_NB_ANPA_NUM_CLUSTERS	1

/* SB Ethernet Common Sub Windows - Dummy values */
#define AL_ETH_COMMON_OFFSET			0
#define AL_ETH_COMMON_BASE			0
#define AL_ETH_COMMON_MAC_COMMON_OFFSET		0
#define AL_ETH_COMMON_MAC_COMMON_BASE		0

/* SB Ethernet Sub Windows */
#define AL_ETH_EC_OFFSET(idx)	0x5000
#define AL_ETH_EC_BASE(idx)	(AL_ETH_BASE(idx) + AL_ETH_EC_OFFSET(idx))
#define AL_ETH_MAC_OFFSET(idx)	(AL_ETH_IS_ADV(idx) ? 0x18000 : 0x9000)
#define AL_ETH_MAC_BASE(idx)	(AL_ETH_BASE(idx) + AL_ETH_MAC_OFFSET(idx))
#define AL_ETH_UDMA_OFFSET(idx, udma_idx)	((udma_idx + 1) * 0x20000)
#define AL_ETH_UDMA_BASE(idx, udma_idx)		(AL_ETH_BASE(idx) + \
						AL_ETH_UDMA_OFFSET(idx, udma_idx))
#define AL_ETH_UDMA_NUM(idx)			(AL_ETH_IS_ADV(idx) ? 4 : 1)
#define AL_ETH_ADAPTER_BASE(idx)		AL_ETH_BASE(idx)

/* SB SSM UDMA */
#define AL_SSM_UDMA_OFFSET(idx, udma_idx)	((udma_idx + 1) * 0x20000)
#define AL_SSM_UDMA_BASE(idx, udma_idx)		(AL_SSM_BASE(idx) + \
						AL_SSM_UDMA_OFFSET(idx, udma_idx))
#define AL_SSM_UDMA_NUM(idx)			2

/* SB SSM Adapter */
#define AL_SSM_ADAPTER_BASE(idx)		AL_SSM_BASE(idx)

/* SB SSM Crypto Sub Windows */
#define AL_SSM_CRYPTO_UDMA_BASE(udma_idx)	AL_SSM_UDMA_BASE(0, udma_idx)
#define AL_SSM_CRYPTO_CRC_BASE(crc_idx)		(AL_SSM_BASE(0) + 0x5000 + (crc_idx) * 0x1000)
#define AL_SSM_CRYPTO_CRYPTO_BASE		(AL_SSM_BASE(0) + 0x9000)
#define AL_SSM_CRYPTO_RAID_BASE			(AL_SSM_BASE(0) + 0xa000)

/* SB SSM RAID Sub Windows */
#define AL_SSM_RAID_UDMA_BASE(udma_idx)	AL_SSM_UDMA_BASE(1, udma_idx)
#define AL_SSM_RAID_CRC_BASE(crc_idx)	(AL_SSM_BASE(1) + 0x5000 + (crc_idx) * 0x1000)
#define AL_SSM_RAID_CRYPTO_BASE		(AL_SSM_BASE(1) + 0x9000)
#define AL_SSM_RAID_RAID_BASE		(AL_SSM_BASE(1) + 0xa000)
/* SB SSM Application BAR */
#define AL_SSM_APP_OFFSET		0x5000
#define AL_SSM_APP_BASE(idx)		(AL_SSM_BASE(idx) + AL_SSM_APP_OFFSET)

#define AL_SSM_CRC_NUM		2

/* SB SATA Sub Windows */
#define AL_SATA_CTRL_OFFSET		0
#define AL_SATA_CTRL_BASE(idx)		(AL_SATA_BASE(idx) + AL_SATA_CTRL_OFFSET)
#define AL_SATA_ADAPTER_OFFSET		0xf8000
#define AL_SATA_ADAPTER_BASE(idx)	(AL_SATA_BASE(idx) + AL_SATA_ADAPTER_OFFSET)
#define AL_SATA_IOFIC_OFFSET		0x2000
#define AL_SATA_IOFIC_BASE(idx)		(AL_SATA_BASE(idx) + AL_SATA_IOFIC_OFFSET)

/* SB PBS Sub Windows */
#define AL_I2C_PLD_BASE			(AL_SB_PBS_BASE + 0x00000000)
#define AL_SPI_SLAVE_BASE		(AL_SB_PBS_BASE + 0x00001000)
#define AL_SPI_MASTER_BASE		(AL_SB_PBS_BASE + 0x00002000)

#define AL_UART_BASE(idx)	(AL_SB_PBS_BASE + 0x00003000 \
				+ ((idx) * 0x1000))
#define AL_UART_NUM		4

#define AL_GPIO_BASE(idx)	(AL_SB_PBS_BASE + \
		((idx != 5) ? 0x00007000 + ((idx) * 0x1000) : 0x17000))
#define AL_GPIO_NUM		6

#define AL_WD_BASE(idx)		(AL_SB_PBS_BASE \
			+ (((idx) < 4) ? 0x0000c000 : 0x0002c000) \
			+ (((idx) < 4) ? ((idx) * 0x1000) : ((idx - 4) * 0x1000)))
#define AL_WD_NUM		8

#define AL_TIMER_BASE(idx, sub_idx) (AL_SB_PBS_BASE \
			+ (((idx) < 4) ? 0x00010000 : 0x00030000) \
			+ (((idx) < 4) ? ((idx) * 0x1000) : ((idx - 4) * 0x1000)) \
			+ ((sub_idx) * 0x20))

#define AL_TIMER_NUM		8
#define AL_TIMER_SUB_TIMERS_NUM	2

#define AL_I2C_GEN_BASE		(AL_SB_PBS_BASE + 0x00014000)
#define AL_PBS_UFC_WRAP_BASE	(AL_SB_PBS_BASE + 0x00015000)
#define AL_PBS_UFC_CNTL_BASE	(AL_SB_PBS_BASE + 0x00015800)
#define AL_PBS_OTP_BASE		(AL_SB_PBS_BASE + 0x00016000)
#define AL_PBS_OTP_4K_BASE(idx)	(idx * 0) /* Dummy value */
#define AL_PBS_SBUS_MASTER_BASE	0
#define AL_PBS_BOOT_ROM_BASE	(AL_SB_PBS_BASE + 0x00020000)
#define AL_PBS_SRAM_BASE	(AL_SB_PBS_BASE + 0x00024000)
#define AL_PBS_REGFILE_OFF	(0x00028000)
#define AL_PBS_REGFILE_BASE	(AL_SB_PBS_BASE + AL_PBS_REGFILE_OFF)
#define AL_PBS_SGPO_BASE	(AL_SB_PBS_BASE + 0x00034000)
#define AL_PBS_TDM_APB_BASE		(AL_SB_PBS_BASE + 0x0003c000)

/* SB Ring Sub Windows */
#define AL_CMOS_NUM_GROUPS	10

#define AL_CMOS_GROUP_BASE(idx)	(AL_SB_RING_BASE + (idx) * 0x100)

#define AL_TEMP_SENSOR_BASE	(AL_SB_RING_BASE + 0xa00)

#define AL_PLL_SB		0
#define AL_PLL_NB		1
#define AL_PLL_CPU		2

#define AL_PLL_BASE(id)		(AL_SB_RING_BASE + 0xb00 + (id) * 0x100)

/* SB SerDes Sub Windows */
#define AL_SERDES_BASE(idx)	(AL_SB_SERDES_BASE + \
				((idx < 4) ? ((idx) * 0x400) : 0x2000))

/* SB DFX Sub Windows */
#define AL_DFX_CTRL_BASE	(AL_SB_DFX_BASE + 0x0)
#define AL_DAP2JTAG_BASE	(AL_SB_DFX_BASE + 0x10000)

#define AL_SRD_CMPLX_MAIN_QUAD_BASE(i)		0

#define AL_SRD_CMPLX_MAIN_LANE_BASE(i)	0
#define AL_SRD_CMPLX_MAIN_SBUS_MASTER_BASE	0
#define AL_SRD_CMPLX_SEC_LANE_BASE(i)	0
#define AL_SRD_CMPLX_SEC_SBUS_MASTER_BASE	0
#define AL_SRD_CMPLX_HS_LANE_BASE(i)	0
#define AL_SRD_CMPLX_HS_SBUS_MASTER_BASE	0

/***************************/
/* PBS int mem sub windows */
/***************************/
#define AL_PBS_INT_MEM_BOOT_ROM_BASE	(AL_PBS_INT_MEM_BASE + 0x0)
#define AL_PBS_INT_MEM_SRAM_BASE	(AL_PBS_INT_MEM_BASE + 0x4000)

/******************************/
/* BOOT ROM Related addresses */
/******************************/
#define BOOT_ROM_NAND_READ_FUNC_PTR				0xffff0021
#define BOOT_ROM_XMODEM_RECIEVE_FUNC_PTR			0xffff12f9

/************************************/
/* SRAM related addresses / offsets */
/************************************/
#define STAGE2_ACTUAL_OFFSET_ADDR_IN_PBS_SRAM			0x0000
#define SRAM_AGENT_ADDRESS					0xfbff4200
#define SRAM_CPU_RESUME_ADDRESS					0xfbff4120
#define SRAM_DEV_INFO_ADDRESS					0xfbff4100
#define PBS_INT_MEM_SHARED_PARAMS_OFFSET			0x0150
#define SRAM_GENERAL_SHARED_DATA_OFFSET			0x180

/* uC shared location in SRAM */
#define AL_UC_SHARED_BASE	(AL_PBS_SRAM_BASE + 0x3f00)

/* TDM sub windows */
#define AL_TDM_AXI_TX_FIFO_BASE (AL_TDM_AXI_BASE + 0x00000)
#define AL_TDM_AXI_RX_FIFO_BASE (AL_TDM_AXI_BASE + 0x01000)
#define AL_TDM_AXI_CFG_BASE	(AL_TDM_AXI_BASE + 0x10000)
#define AL_SPI_MASTER_AUX_AXI_BASE	(AL_TDM_AXI_BASE + 0x10100)

#define AL_TDM_APB_TX_BASE (AL_PBS_TDM_APB_BASE + 0x00000)
#define AL_TDM_APB_RX_BASE (AL_PBS_TDM_APB_BASE + 0x01000)
#define AL_TDM_APB_CFG_BASE (AL_PBS_TDM_APB_BASE + 0x02000)
#define AL_SPI_MASTER_AUX_BASE	(AL_PBS_TDM_APB_BASE + 0x0002100)

/* Not relevant for V2 - dummy values */
#define AL_PBS_CCTM_BASE	0
#define AL_NB_ANPA_BASE(i)	0
#define AL_SATA_PMDT_BASE(i)	0
#define AL_PCIE_MSTR_PMDT_BASE(i)	0
#define AL_PCIE_SLV_PMDT_BASE(i)	0
#define AL_PCIE_CORE_PMDT_BASE(i)	0
#define AL_NB_ANPA_PMDT_BASE(i)		0
#define AL_SSM_PMDT_BASE(i)			0
#define AL_SSM_UDMA_PMDT_BASE(i, j)	0
#define AL_ETH_PMDT_BASE(i)			0
#define AL_ETH_SHARED_RESOURCE_PMDT_BASE	0
#define AL_ETH_SHARED_CACHE_PMDT_BASE		0
#define AL_ETH_EC_PMDT_BASE(i)		0
#define AL_DDRC_PMDT_BASE(i)		0
#define AL_ETH_UDMA_PMDT_BASE(i, j)	0
#define AL_NB_PMDT_BASE			0

#endif
