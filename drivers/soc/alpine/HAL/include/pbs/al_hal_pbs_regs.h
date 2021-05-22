/*
 * Copyright 2016, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

/**
 *  @{
 * @file   al_hal_pbs_regs.h
 *
 * @brief PBS registers
 *
 */

#ifndef __AL_HAL_PBS_REGS_H__
#define __AL_HAL_PBS_REGS_H__

#include "al_hal_plat_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Unit Registers
 */

struct al_pbs_unit {
	/* [0x0] Conf_bus, Configuration of the SB */
	uint32_t conf_bus;
	/* [0x4] PASW high */
	uint32_t dram_0_nb_bar_high_v2;
	/* [0x8] PASW low */
	uint32_t dram_0_nb_bar_low_v2;
	/* [0xc] PASW high */
	uint32_t dram_1_nb_bar_high_v2;
	/* [0x10] PASW low */
	uint32_t dram_1_nb_bar_low_v2;
	/* [0x14] PASW high */
	uint32_t dram_2_nb_bar_high_v2;
	/* [0x18] PASW low */
	uint32_t dram_2_nb_bar_low_v2;
	/* [0x1c] PASW high */
	uint32_t dram_3_nb_bar_high_v2;
	/* [0x20] PASW low */
	uint32_t dram_3_nb_bar_low_v2;
	/* [0x24] PASW high */
	uint32_t msix_nb_bar_high_v2;
	/* [0x28] PASW low */
	uint32_t msix_nb_bar_low_v2;
	/* [0x2c] PASW high */
	uint32_t dram_0_sb_bar_high_v2;
	/* [0x30] PASW low */
	uint32_t dram_0_sb_bar_low_v2;
	/* [0x34] PASW high */
	uint32_t dram_1_sb_bar_high_v2;
	/* [0x38] PASW low */
	uint32_t dram_1_sb_bar_low_v2;
	/* [0x3c] PASW high */
	uint32_t dram_2_sb_bar_high_v2;
	/* [0x40] PASW low */
	uint32_t dram_2_sb_bar_low_v2;
	/* [0x44] PASW high */
	uint32_t dram_3_sb_bar_high_v2;
	/* [0x48] PASW low */
	uint32_t dram_3_sb_bar_low_v2;
	/* [0x4c] PASW high */
	uint32_t msix_sb_bar_high_v2;
	/* [0x50] PASW low */
	uint32_t msix_sb_bar_low_v2;
	/* [0x54] PASW high */
	uint32_t pcie_mem0_bar_high_v2;
	/* [0x58] PASW low */
	uint32_t pcie_mem0_bar_low_v2;
	/* [0x5c] PASW high */
	uint32_t pcie_mem1_bar_high_v2;
	/* [0x60] PASW low */
	uint32_t pcie_mem1_bar_low_v2;
	/* [0x64] PASW high */
	uint32_t pcie_mem2_bar_high_v2;
	/* [0x68] PASW low */
	uint32_t pcie_mem2_bar_low_v2;
	/* [0x6c] PASW high */
	uint32_t pcie_ext_ecam0_bar_high_v2;
	/* [0x70] PASW low */
	uint32_t pcie_ext_ecam0_bar_low_v2;
	/* [0x74] PASW high */
	uint32_t pcie_ext_ecam1_bar_high_v2;
	/* [0x78] PASW low */
	uint32_t pcie_ext_ecam1_bar_low_v2;
	/* [0x7c] PASW high */
	uint32_t pcie_ext_ecam2_bar_high_v2;
	/* [0x80] PASW low */
	uint32_t pcie_ext_ecam2_bar_low_v2;
	/* [0x84] PASW high */
	uint32_t pbs_nor_bar_high_v2;
	/* [0x88] PASW low */
	uint32_t pbs_nor_bar_low_v2;
	/* [0x8c] PASW high */
	uint32_t pbs_spi_bar_high_v2;
	/* [0x90] PASW low */
	uint32_t pbs_spi_bar_low_v2;
	uint32_t rsrvd_0[3];
	/* [0xa0] PASW high */
	uint32_t pbs_nand_bar_high_v2;
	/* [0xa4] PASW low */
	uint32_t pbs_nand_bar_low_v2;
	/* [0xa8] PASW high */
	uint32_t pbs_int_mem_bar_high_v2;
	/* [0xac] PASW low */
	uint32_t pbs_int_mem_bar_low_v2;
	/* [0xb0] PASW high */
	uint32_t pbs_boot_bar_high_v2;
	/* [0xb4] PASW low */
	uint32_t pbs_boot_bar_low_v2;
	/* [0xb8] PASW high */
	uint32_t nb_int_bar_high_v2;
	/* [0xbc] PASW low */
	uint32_t nb_int_bar_low_v2;
	/* [0xc0] PASW high */
	uint32_t nb_stm_bar_high_v2;
	/* [0xc4] PASW low */
	uint32_t nb_stm_bar_low_v2;
	/* [0xc8] PASW high */
	uint32_t pcie_ecam_int_bar_high_v2;
	/* [0xcc] PASW low */
	uint32_t pcie_ecam_int_bar_low_v2;
	/* [0xd0] PASW high */
	uint32_t pcie_mem_int_bar_high_v2;
	/* [0xd4] PASW low */
	uint32_t pcie_mem_int_bar_low_v2;
	/* [0xd8] Control */
	uint32_t winit_cntl_v2;
	/* [0xdc] Control */
	uint32_t latch_bars_v2;
	/* [0xe0] Control */
	uint32_t pcie_conf_0;
	/* [0xe4] Control */
	uint32_t pcie_conf_1;
	/* [0xe8] Control */
	uint32_t serdes_mux_pipe;
	/* [0xec] Control */
	uint32_t dma_io_master_map_v2;
	/* [0xf0] Status */
	uint32_t i2c_pld_status_high;
	/*
	 * [0xf4] Status
	 * I2C pre-load status
	 */
	uint32_t i2c_pld_status_low;
	/* [0xf8] Status */
	uint32_t spi_dbg_status_high;
	/*
	 * [0xfc] Status
	 * SPI DBGg load status
	 */
	uint32_t spi_dbg_status_low;
	/* [0x100] Status */
	uint32_t spi_mst_status_high;
	/*
	 * [0x104] Status
	 * SPI MST load status
	 */
	uint32_t spi_mst_status_low;
	/* [0x108] Log */
	uint32_t mem_pbs_parity_err_high;
	/*
	 * [0x10c] Log
	 * Address latch in the case of a parity error
	 */
	uint32_t mem_pbs_parity_err_low;
	/*
	 * [0x110] Log
	 * Bootstrap value bits 31:0. Reflects the bootstrap value, according to the datasheet.
	 */
	uint32_t boot_strap;
	/* [0x114] Conf */
	uint32_t cfg_axi_conf_0;
	/* [0x118] Conf */
	uint32_t cfg_axi_conf_1;
	/* [0x11c] Conf */
	uint32_t cfg_axi_conf_2;
	/*
	 * [0x120] Conf
	 * Each configuration access is protected by a timeout, which expires when the unit does not
	 * respond after the counter reaches the register value limit.
	 */
	uint32_t cfg_axi_conf_3;
	/* [0x124] Conf */
	uint32_t spi_mst_conf_0;
	/* [0x128] Conf */
	uint32_t spi_mst_conf_1;
	/* [0x12c] Conf */
	uint32_t spi_slv_conf_0;
	/* [0x130] Conf */
	uint32_t apb_mem_conf_int;
	/* [0x134] PASW remap register */
	uint32_t sb2nb_cfg_dram_remap_v2;
	/*
	 * [0x138] Control
	 * value. Control on pins 7 to 0
	 * 0000 - Column A in datasheet.
	 * 0001 - Column B in datasheet.
	 * 0010 - Column C in datasheet.
	 * 0011 - Column D in datasheet.
	 * 0100 - Column E in datasheet
	 * 0101-1111 - Reserved
	 */
	uint32_t pbs_mux_sel_0;
	/*
	 * [0x13c] Control
	 * value. Control on pins 15 to 8
	 * 0000 - Column A in datasheet.
	 * 0001 - Column B in datasheet.
	 * 0010 - Column C in datasheet.
	 * 0011 - Column D in datasheet.
	 * 0100 - Column E in datasheet
	 * 0101-1111 - Reserved
	 */
	uint32_t pbs_mux_sel_1;
	/*
	 * [0x140] Control
	 * value. Control on pins 23 to 16
	 * 0000 - Column A in datasheet.
	 * 0001 - Column B in datasheet.
	 * 0010 - Column C in datasheet.
	 * 0011 - Column D in datasheet.
	 * 0100 - Column E in datasheet
	 * 0101-1111 - Reserved
	 */
	uint32_t pbs_mux_sel_2;
	/*
	 * [0x144] Control
	 * value. Control on pins 31 to 24
	 * 0000 - Column A in datasheet.
	 * 0001 - Column B in datasheet.
	 * 0010 - Column C in datasheet.
	 * 0011 - Column D in datasheet.
	 * 0100 - Column E in datasheet
	 * 0101-1111 - Reserved
	 */
	uint32_t pbs_mux_sel_3;
	/* [0x148] PASW high */
	uint32_t sb_int_bar_high_v2;
	/* [0x14c] PASW low */
	uint32_t sb_int_bar_low_v2;
	/* [0x150] log */
	uint32_t ufc_pbs_parity_err_high;
	/*
	 * [0x154] log
	 * Address latch in the case of a parity error in the Flash Controller internal memories.
	 */
	uint32_t ufc_pbs_parity_err_low;
	/*
	 * [0x158] Cntl - internal
	 * [0] Mask the NOR init
	 * [1] SD to wrapper controller
	 */
	uint32_t gen_conf;
	/* [0x15c] Device ID and Rev ID */
	uint32_t chip_id;
	/*
	 * [0x160] Status - internal
	 * Status of the UART
	 */
	uint32_t uart0_debug;
	/*
	 * [0x164] Status - internal
	 * Status of the UART
	 */
	uint32_t uart1_debug;
	/*
	 * [0x168] Status - internal
	 * Status of the UART
	 */
	uint32_t uart2_debug;
	/*
	 * [0x16c] Status - internal
	 * Status of the UART
	 */
	uint32_t uart3_debug;
	/* [0x170] Control - internal */
	uint32_t uart0_conf_status;
	/* [0x174] Control - internal */
	uint32_t uart1_conf_status;
	/* [0x178] Control - internal */
	uint32_t uart2_conf_status;
	/* [0x17c] Control - internal */
	uint32_t uart3_conf_status;
	/* [0x180] Control - internal */
	uint32_t gpio0_conf_status;
	/* [0x184] Control - internal */
	uint32_t gpio1_conf_status;
	/* [0x188] Control - internal */
	uint32_t gpio2_conf_status;
	/* [0x18c] Control - internal */
	uint32_t gpio3_conf_status;
	/* [0x190] Control - internal */
	uint32_t gpio4_conf_status;
	/* [0x194] Control - internal */
	uint32_t i2c_gen_conf_status;
	/*
	 * [0x198] Control - internal
	 * Debug
	 */
	uint32_t i2c_gen_debug;
	/* [0x19c] Cntl */
	uint32_t watch_dog_reset_out;
	/*
	 * [0x1a0] Cntl
	 * The value that enables burning of the fuses in the OTP.
	 */
	uint32_t otp_magic_num;
	/*
	 * [0x1a4] Control - internal
	 */
	uint32_t otp_cntl;
	/* [0x1a8] Cfg - internal */
	uint32_t otp_cfg_0;
	/* [0x1ac] Cfg - internal */
	uint32_t otp_cfg_1;
	/* [0x1b0] Cfg - internal */
	uint32_t otp_cfg_3;
	/*
	 * [0x1b4] Cfg
	 * [31] Register is valid or invalid
	 * [30:28] : Page size:
	 * 000 - 512 bytes
	 * 001 - Reserved
	 * 010 - 2048 bytes
	 * 011 - 4096 bytes
	 * 100 - 8192 bytes
	 * 101 - 16384 bytes
	 * 110 - Reserved
	 * 111 - Reserved
	 * [27:25]:Block size:
	 * 000 - 16 pages
	 * 001 - 32 pages
	 * 010 - 64 pages
	 * 011 - 128 pages
	 * 100 - 256 pages
	 *  101 - 512 pages
	 * 110 - 1024 pages
	 * 111 - 2048 pages
	 * [24] Bus width:
	 * 0 - 8 bits
	 * 1 - 16 bits
	 * [23:22] :Num column cycles:
	 * 00 - 1 cycle
	 * 01 - 2 cycles
	 * 10 - 3 cycles
	 * 11 - 4 cycles
	 * [21:19] Num row cycles:
	 * 000 - 1 cycle
	 * 001 - 2 cycles
	 * 010 - 3 cycles
	 * 011 - 4 cycles
	 * 100 - 5 cycles
	 * 101 - 6 cycles
	 * 110 - 7 cycles
	 * 111 - 8 cycles
	 * [18:16]: Bad block marking method:
	 * 000 - Disabled
	 * 001 - Check only first page of each block
	 * 010 - Check first and second page of each block
	 * 011 - Check only last page of each block
	 * 100 - Check last and last-2 page of each block
	 * 101 - reserved
	 * 110 - reserved
	 * 111 - reserved
	 * [15:12] 0000 - 1111 - Bad block marking location, word (depending on bus width) location
	 * within spare area
	 * [11:8] 0000 - 1111 - Bad block marking location, word (depending on bus width) location
	 * within spare area
	 * [7:5] Reserved
	 * [4:2] Timing parameter set:
	 * 000 - ONFI 0
	 * 001 - ONFI 1
	 * 010 - ONFI 2
	 * 011 - ONFI 3
	 * 100 - ONFI 4
	 * 101 - ONFI 5
	 * 110 - manual
	 * [1:0]ECC algorithm:
	 * 00 - ECC disabled
	 * 01 - Hamming (1 bit per 512 bytes)
	 * 10 - BCH
	 *
	 */
	uint32_t cfg_nand_0;
	/*
	 * [0x1b8] Cfg
	 * [31:28]: BCH required strength:
	 * 0000 - 4 bits
	 * 0001 - 8 bits
	 * 0010 - 12 bits
	 * 0011 - 16 bits
	 * 0100 - 20 bits
	 * 0101 - 24 bits
	 * 0110 - 28 bits
	 * 0111 - 32 bits
	 * 1000 - 36 bits
	 * 1001 - 40 bits
	 * [27] Code word size:
	 *   0:512 bytes.
	 *   1: 1024 bytes.
	 * [26:18]: ECC location in spare area
	 * [17:0]: Timing parameters internal
	 */
	uint32_t cfg_nand_1;
	/* [0x1bc] Cfg-- timing parameters internal. */
	uint32_t cfg_nand_2;
	/* [0x1c0] Cfg - internal */
	uint32_t cfg_nand_3;
	/* [0x1c4] PASW high */
	uint32_t nb_nic_regs_bar_high_v2;
	/* [0x1c8] PASW low */
	uint32_t nb_nic_regs_bar_low_v2;
	/* [0x1cc] PASW high */
	uint32_t sb_nic_regs_bar_high_v2;
	/* [0x1d0] PASW low */
	uint32_t sb_nic_regs_bar_low_v2;
	/* [0x1d4] Control */
	uint32_t serdes_mux_multi_0;
	/* [0x1d8] Control */
	uint32_t serdes_mux_multi_1;
	/* [0x1dc] Control - not in use any more - internal */
	uint32_t pbs_ulpi_mux_conf;
	/* [0x1e0] Cntl */
	uint32_t wr_once_dbg_dis_ovrd_reg;
	/* [0x1e4] Cntl - internal */
	uint32_t gpio5_conf_status;
	/* [0x1e8] PASW high */
	uint32_t pcie_mem3_bar_high_v2;
	/* [0x1ec] PASW low */
	uint32_t pcie_mem3_bar_low_v2;
	/* [0x1f0] PASW high */
	uint32_t pcie_mem4_bar_high_v2;
	/* [0x1f4] PASW low */
	uint32_t pcie_mem4_bar_low_v2;
	/* [0x1f8] PASW high */
	uint32_t pcie_mem5_bar_high_v2;
	/* [0x1fc] PASW low */
	uint32_t pcie_mem5_bar_low_v2;
	/* [0x200] PASW high */
	uint32_t pcie_ext_ecam3_bar_high_v2;
	/* [0x204] PASW low */
	uint32_t pcie_ext_ecam3_bar_low_v2;
	/* [0x208] PASW high */
	uint32_t pcie_ext_ecam4_bar_high_v2;
	/* [0x20c] PASW low */
	uint32_t pcie_ext_ecam4_bar_low_v2;
	/* [0x210] PASW high */
	uint32_t pcie_ext_ecam5_bar_high_v2;
	/* [0x214] PASW low */
	uint32_t pcie_ext_ecam5_bar_low_v2;
	/* [0x218] PASW high */
	uint32_t low_latency_sram_bar_high_v2;
	/* [0x21c] PASW low */
	uint32_t low_latency_sram_bar_low_v2;
	/*
	 * [0x220] Control
	 * value. Control on pins 39 to 32
	 * 0000 - Column A in datasheet.
	 * 0001 - Column B in datasheet.
	 * 0010 - Column C in datasheet.
	 * 0011 - Column D in datasheet.
	 * 0100 - Column E in datasheet
	 * 0101-1111 - Reserved
	 */
	uint32_t pbs_mux_sel_4;
	/*
	 * [0x224] Control
	 * value. Control on pins 47 to 40
	 * 0000 - Column A in datasheet.
	 * 0001 - Column B in datasheet.
	 * 0010 - Column C in datasheet.
	 * 0011 - Column D in datasheet.
	 * 0100 - Column E in datasheet
	 * 0101-1111 - Reserved
	 */
	uint32_t pbs_mux_sel_5;
	/* [0x228] Control */
	uint32_t serdes_mux_eth;
	/* [0x22c] Control */
	uint32_t serdes_mux_pcie;
	/* [0x230] Control */
	uint32_t serdes_mux_sata;
	/* [0x234] Control */
	uint32_t i2c_scl_reroute;
	/*
	 * [0x238] Conf
	 * Each configuration access is protected by a timeout, which expires when the unit does not
	 * respond after the counter reaches the register value limit. This configuration is
	 * dedicated for the preloaders
	 */
	uint32_t cfg_axi_conf_4;
	uint32_t rsrvd_6;
	/* [0x240] Conf */
	uint32_t cfg_axi_conf_5;
	/*
	 * [0x244] Control
	 * value. Control on pins 55 to 48
	 * 0000 - Column A in datasheet.
	 * 0001 - Column B in datasheet.
	 * 0010 - Column C in datasheet.
	 * 0011 - Column D in datasheet.
	 * 0100 - Column E in datasheet
	 * 0101-1111 - Reserved
	 */
	uint32_t pbs_mux_sel_6;
	/*
	 * [0x248] Control
	 * value. Control on pins 63 to 56
	 * 0000 - Column A in datasheet.
	 * 0001 - Column B in datasheet.
	 * 0010 - Column C in datasheet.
	 * 0011 - Column D in datasheet.
	 * 0100 - Column E in datasheet
	 * 0101-1111 - Reserved
	 */
	uint32_t pbs_mux_sel_7;
	uint32_t rsrvd_7[9];
	/* [0x270] Control - internal */
	uint32_t gpio6_conf_status;
	/* [0x274] Control - internal */
	uint32_t gpio7_conf_status;
	/*
	 * [0x278] Log
	 * Bootstrap value bits 63:32. Reflects the bootstrap value, according to the datasheet.
	 */
	uint32_t boot_strap_2;
	uint32_t rsrvd[33];
};

struct al_pbs_reserved {
	/*
	 * [0x0] reserved register with default 0
	 * Reserved
	 * Bit 0 - io_mdio_out_nodly_sel - Whether to add additional delay on MDIO bus
	 * Bit 1- io_mdio_out_dly_sel - Whether to use 1/2 delay amount or full delay amount
	 */
	uint32_t reserved_zero;
	/*
	 * [0x4] reserved register with default 1
	 * Reserved
	 */
	uint32_t reserved_one;
	uint32_t rsrvd[2];
};

struct al_pbs_qspi {
	/* [0x0] QSPI enable */
	uint32_t enable;
	/* [0x4] QSPI clear trigger */
	uint32_t clear_trig;
	/* [0x8] QSPI start trigger */
	uint32_t start_trig;
	/* [0xc] QSPI stop trigger */
	uint32_t stop_trig;
	/* [0x10] QSPI rst trigger */
	uint32_t rst_trig;
	/* [0x14] QSPI rsten trigger */
	uint32_t rsten_trig;
	/* [0x18] QSPI wen trigger */
	uint32_t wen_trig;
	/* [0x1c] QSPI a4enter trigger */
	uint32_t a4enter_trig;
	/* [0x20] QSPI a4exit trigger */
	uint32_t a4exit_trig;
	/* [0x24] QSPI configurations */
	uint32_t config;
	/* [0x28] QSPI Opcodes */
	uint32_t opcodes;
	/* [0x2c] QSPI Opcodes2 */
	uint32_t opcodes2;
	/* [0x30] QSPI slave enable */
	uint32_t slave_enable;
	uint32_t rsrvd[7];
};

struct al_pbs_spim_preload {
	/* [0x0] SPIM preload clear trigger */
	uint32_t spim_preload_clear_trig;
	/*
	 * [0x4] ADDR  high
	 * Addr high
	 */
	uint32_t spim_base_high;
	/*
	 * [0x8] ADDR  low
	 * Addr low
	 */
	uint32_t spim_base_low;
	/*
	 * [0xc] AXI assist
	 * Reserved
	 */
	uint32_t axi_assist;
	/*
	 * [0x10] Preload status
	 * Preload status
	 */
	uint32_t preload_status;
	uint32_t rsrvd[3];
};

struct al_pbs_chip_capabilities {
	/*
	 * [0x0] Write once register
	 * Bit per unit determines if the associate unit located on the embedded
	 * PCI bus is enabled or not
	 * When IOFAB_Unit_limit [N] = 1, unit N is disabled.
	 * Associate PCIe Adapter does not respond to any access
	 * Disable will only work if corresponding enable override bit (31:28) is et
	 * 0 - Ethernet 100_0
	 * 1 - Ethernet 10_0
	 * 2 - Ethernet 100_1
	 * 3 - Ethernet 10_1
	 * 4 - Ethernet 100_2
	 * 5 - Ethernet 100_3
	 * 6 - SSMAE 0
	 * 8 - SATA 0
	 * 9 - SATA 1
	 * 10 - SATA 2
	 * 11 - SATA 3
	 * 12 - SATA 4
	 * 13 - SATA 5
	 * 14 - SATA 6
	 * 15 - SATA 7
	 * 16 - SSMAE 1
	 * 28 - Enable override for ETH 100 adapters
	 * 29 - Enable override for ETH 10 adapters
	 * 30 - Enable override for SATA adapters
	 * 31 - Enable override foe SSMAE adapters
	 */
	uint32_t io_fabric_capability0;
	/*
	 * [0x4] Write once register
	 * Bit per unit determines if the associate unit that is NOT located on
	 * the embedded PCI bus is enabled or not
	 * When IOFAB_Unit_limit [N] = 1, unit N is disabled.
	 * Associate PCIe Adapter does not respond to any access
	 */
	uint32_t io_fabric_capability1;
	/* [0x8] Write once register */
	uint32_t eth_capability;
	/* [0xc] Write once register */
	uint32_t pcie_capability;
	/* [0x10] Write once register */
	uint32_t cpu_capability0;
	/* [0x14] Write once register */
	uint32_t cpu_capability1;
	/* [0x18] Write once register */
	uint32_t dram_capability;
	uint32_t rsrvd[9];
};

struct al_pbs_bar_config {
	/*
	 * [0x0] PASW high
	 * Addr high
	 */
	uint32_t dram_0_nb_bar_high;
	/* [0x4] PASW low */
	uint32_t dram_0_nb_bar_low;
	/*
	 * [0x8] PASW high
	 * Addr high
	 */
	uint32_t dram_1_nb_bar_high;
	/* [0xc] PASW low */
	uint32_t dram_1_nb_bar_low;
	/*
	 * [0x10] PASW high
	 * Addr high
	 */
	uint32_t dram_2_nb_bar_high;
	/* [0x14] PASW low */
	uint32_t dram_2_nb_bar_low;
	/*
	 * [0x18] PASW high
	 * Addr high
	 */
	uint32_t dram_3_nb_bar_high;
	/* [0x1c] PASW low */
	uint32_t dram_3_nb_bar_low;
	uint32_t rsrvd_0[4];
	/*
	 * [0x30] PASW high
	 * Addr high
	 */
	uint32_t msix_nb_bar_high;
	/* [0x34] PASW low */
	uint32_t msix_nb_bar_low;
	uint32_t rsrvd_1[2];
	/*
	 * [0x40] PASW high
	 * Addr high
	 */
	uint32_t dram_0_sb_bar_high;
	/* [0x44] PASW low */
	uint32_t dram_0_sb_bar_low;
	/*
	 * [0x48] PASW high
	 * Addr high
	 */
	uint32_t dram_1_sb_bar_high;
	/* [0x4c] PASW low */
	uint32_t dram_1_sb_bar_low;
	/*
	 * [0x50] PASW high
	 * Addr high
	 */
	uint32_t dram_2_sb_bar_high;
	/* [0x54] PASW low */
	uint32_t dram_2_sb_bar_low;
	/*
	 * [0x58] PASW high
	 * Addr high
	 */
	uint32_t dram_3_sb_bar_high;
	/* [0x5c] PASW low */
	uint32_t dram_3_sb_bar_low;
	uint32_t rsrvd_2[4];
	/*
	 * [0x70] PASW high
	 * Addr high
	 */
	uint32_t msix_sb_bar_high;
	/* [0x74] PASW low */
	uint32_t msix_sb_bar_low;
	uint32_t rsrvd_3[2];
	/*
	 * [0x80] PASW high
	 * Addr high
	 */
	uint32_t pcie_mem0_bar_high;
	/* [0x84] PASW low */
	uint32_t pcie_mem0_bar_low;
	uint32_t rsrvd_4;
	/*
	 * [0x8c] PASW high
	 * Addr high
	 */
	uint32_t pcie_mem1_bar_high;
	/* [0x90] PASW low */
	uint32_t pcie_mem1_bar_low;
	/*
	 * [0x94] PASW high
	 * Addr high
	 */
	uint32_t pcie_mem2_bar_high;
	/* [0x98] PASW low */
	uint32_t pcie_mem2_bar_low;
	/*
	 * [0x9c] PASW high
	 * Addr high
	 */
	uint32_t pcie_mem3_bar_high;
	/* [0xa0] PASW low */
	uint32_t pcie_mem3_bar_low;
	/*
	 * [0xa4] PASW high
	 * Addr high
	 */
	uint32_t pcie_mem4_bar_high;
	/* [0xa8] PASW low */
	uint32_t pcie_mem4_bar_low;
	/*
	 * [0xac] PASW high
	 * Addr high
	 */
	uint32_t pcie_mem5_bar_high;
	/* [0xb0] PASW low */
	uint32_t pcie_mem5_bar_low;
	/*
	 * [0xb4] PASW high
	 * Addr high
	 */
	uint32_t pcie_mem6_bar_high;
	/* [0xb8] PASW low */
	uint32_t pcie_mem6_bar_low;
	uint32_t rsrvd_5;
	/*
	 * [0xc0] PASW high
	 * Addr high
	 */
	uint32_t pcie_ext_ecam0_bar_high;
	/* [0xc4] PASW low */
	uint32_t pcie_ext_ecam0_bar_low;
	/*
	 * [0xc8] PASW high
	 * Addr high
	 */
	uint32_t pcie_ext_ecam1_bar_high;
	/* [0xcc] PASW low */
	uint32_t pcie_ext_ecam1_bar_low;
	/*
	 * [0xd0] PASW high
	 * Addr high
	 */
	uint32_t pcie_ext_ecam2_bar_high;
	/* [0xd4] PASW low */
	uint32_t pcie_ext_ecam2_bar_low;
	/*
	 * [0xd8] PASW high
	 * Addr high
	 */
	uint32_t pcie_ext_ecam3_bar_high;
	/* [0xdc] PASW low */
	uint32_t pcie_ext_ecam3_bar_low;
	/*
	 * [0xe0] PASW high
	 * Addr high
	 */
	uint32_t pcie_ext_ecam4_bar_high;
	/* [0xe4] PASW low */
	uint32_t pcie_ext_ecam4_bar_low;
	/*
	 * [0xe8] PASW high
	 * Addr high
	 */
	uint32_t pcie_ext_ecam5_bar_high;
	/* [0xec] PASW low */
	uint32_t pcie_ext_ecam5_bar_low;
	/*
	 * [0xf0] PASW high
	 * Addr high
	 */
	uint32_t pcie_ext_ecam6_bar_high;
	/* [0xf4] PASW low */
	uint32_t pcie_ext_ecam6_bar_low;
	/*
	 * [0xf8] PASW high
	 * Addr high
	 */
	uint32_t pcie_ext_ecam7_bar_high;
	/* [0xfc] PASW low */
	uint32_t pcie_ext_ecam7_bar_low;
	/*
	 * [0x100] PASW high
	 * Addr high
	 */
	uint32_t coresight_bar_high;
	/* [0x104] PASW low */
	uint32_t coresight_bar_low;
	/*
	 * [0x108] PASW high
	 * Addr high
	 */
	uint32_t pbs_spi_bar_high;
	/* [0x10c] PASW low */
	uint32_t pbs_spi_bar_low;
	/*
	 * [0x110] PASW high
	 * Addr high
	 */
	uint32_t pbs_nand_bar_high;
	/* [0x114] PASW low */
	uint32_t pbs_nand_bar_low;
	/*
	 * [0x118] PASW high
	 * Addr high
	 */
	uint32_t pbs_int_mem_bar_high;
	/* [0x11c] PASW low */
	uint32_t pbs_int_mem_bar_low;
	/*
	 * [0x120] PASW high
	 * Addr high
	 */
	uint32_t pbs_boot_bar_high;
	/* [0x124] PASW low */
	uint32_t pbs_boot_bar_low;
	uint32_t rsrvd_6[2];
	/*
	 * [0x130] PASW high
	 * Addr high
	 */
	uint32_t nb_int_bar_high;
	/* [0x134] PASW low */
	uint32_t nb_int_bar_low;
	/*
	 * [0x138] PASW high
	 * Addr high
	 */
	uint32_t nb_stm_bar_high;
	/* [0x13c] PASW low */
	uint32_t nb_stm_bar_low;
	/*
	 * [0x140] PASW high
	 * Addr high
	 */
	uint32_t pcie_ecam_int_bar_high;
	/* [0x144] PASW low */
	uint32_t pcie_ecam_int_bar_low;
	/*
	 * [0x148] PASW high
	 * Addr high
	 */
	uint32_t pcie_mem_int_bar_high;
	/* [0x14c] PASW low */
	uint32_t pcie_mem_int_bar_low;
	uint32_t rsrvd_7[4];
	/*
	 * [0x160] PASW high
	 * Addr high
	 */
	uint32_t sb_int_bar_high;
	/* [0x164] PASW low */
	uint32_t sb_int_bar_low;
	uint32_t rsrvd_8[2];
	/*
	 * [0x170] PASW high
	 * Addr high
	 */
	uint32_t fast_sb_nic_regs_bar_high;
	/* [0x174] PASW low */
	uint32_t fast_sb_nic_regs_bar_low;
	/*
	 * [0x178] PASW high
	 * Addr high
	 */
	uint32_t sb_nic_regs_bar_high;
	/* [0x17c] PASW low */
	uint32_t sb_nic_regs_bar_low;
	/*
	 * [0x180] PASW high
	 * Addr high
	 */
	uint32_t low_latency_sram_bar_high;
	uint32_t rsrvd_9[2];
	/* [0x18c] PASW low */
	uint32_t low_latency_sram_bar_low;
	uint32_t rsrvd_10[4];
	/*
	 * [0x1a0] PASW high
	 * Addr high
	 */
	uint32_t eth_sram_0_bar_high;
	/* [0x1a4] PASW low */
	uint32_t eth_sram_0_bar_low;
	/*
	 * [0x1a8] PASW high
	 * Addr high
	 */
	uint32_t eth_sram_1_bar_high;
	/* [0x1ac] PASW low */
	uint32_t eth_sram_1_bar_low;
	/*
	 * [0x1b0] PASW high
	 * Addr high
	 */
	uint32_t trng_bar_high;
	/* [0x1b4] PASW low */
	uint32_t trng_bar_low;
	uint32_t rsrvd_11[2];
	/*
	 * [0x1c0] PASW high
	 * Addr high
	 */
	uint32_t tdm_bar_high;
	/* [0x1c4] PASW low */
	uint32_t tdm_bar_low;
	/*
	 * [0x1c8] PASW high
	 * Addr high
	 */
	uint32_t pcie_mem7_bar_high;
	/* [0x1cc] PASW low */
	uint32_t pcie_mem7_bar_low;
	/* [0x1d0] Control */
	uint32_t winit_cntl;
	/* [0x1d4] Control */
	uint32_t latch_bars;
	uint32_t rsrvd_12[2];
	/*
	 * [0x1e0] Control
	 * [0]: When set, if crypto_0 transaction doesn't hit any window, transaction is sent to
	 * NB/DRAM (otherwise to ERROR)
	 * [1]: When set, if crypto_1 transaction doesn't hit any window, transaction is sent to
	 * NB/DRAM (otherwise to ERROR)
	 * [2]: When set, if eth_0 transaction doesn't hit any window, transaction is sent to
	 * NB/DRAM (otherwise to ERROR)
	 * [3]: When set, if eth_1 transaction doesn't hit any window, transaction is sent to
	 * NB/DRAM (otherwise to ERROR)
	 * [4]: When set, if eth_2 transaction doesn't hit any window, transaction is sent to
	 * NB/DRAM (otherwise to ERROR)
	 * [5]: When set, if eth_3 transaction doesn't hit any window, transaction is sent to
	 * NB/DRAM (otherwise to ERROR)
	 * [6]: When set, if eth_4 transaction doesn't hit any window, transaction is sent to
	 * NB/DRAM (otherwise to ERROR)
	 * [7]: When set, if eth_5 transaction doesn't hit any window, transaction is sent to
	 * NB/DRAM (otherwise to ERROR)
	 * [9:8] - Reserved
	 * [10]: When set, if SPI_DEBUG transaction doesn't hit any window, transaction is sent to
	 * NB/DRAM (otherwise to ERROR)
	 * [11]: When set, if CPU_DEBUG transaction doesn't hit any window, transaction is sent to
	 * NB/DRAM (otherwise to ERROR)
	 * [12]: When set, if pcie_0 transaction doesn't hit any window, transaction is sent to
	 * NB/DRAM (otherwise to ERROR)
	 * [13]: When set, if pcie_1 transaction doesn't hit any window, transaction is sent to
	 * NB/DRAM (otherwise to ERROR)
	 * [14]: When set, if pcie_2 transaction doesn't hit any window, transaction is sent to
	 * NB/DRAM (otherwise to ERROR)
	 * [15]: When set, if pcie_3 transaction doesn't hit any window, transaction is sent to
	 * NB/DRAM (otherwise to ERROR)
	 * [16]: When set, if pcie_4 transaction doesn't hit any window, transaction is sent to
	 * NB/DRAM (otherwise to ERROR)
	 * [17]: When set, if pcie_5 transaction doesn't hit any window, transaction is sent to
	 * NB/DRAM (otherwise to ERROR)
	 * [18]: When set, if pcie_6 transaction doesn't hit any window, transaction is sent to
	 * NB/DRAM (otherwise to ERROR)
	 * [19]: When set, if pcie_7 transaction doesn't hit any window, transaction is sent to
	 * NB/DRAM (otherwise to ERROR)
	 * [21:20]: Reserved
	 * [22]: When set, if sata_0 transaction doesn't hit any window, transaction is sent to
	 * NB/DRAM (otherwise to ERROR)
	 * [23]: When set, if sata_1 transaction doesn't hit any window, transaction is sent to
	 * NB/DRAM (otherwise to ERROR)
	 * [24]: When set, if sata_2 transaction doesn't hit any window, transaction is sent to
	 * NB/DRAM (otherwise to ERROR)
	 * [25]: When set, if sata_3 transaction doesn't hit any window, transaction is sent to
	 * NB/DRAM (otherwise to ERROR)
	 * [26]: When set, if sata_4 transaction doesn't hit any window, transaction is sent to
	 * NB/DRAM (otherwise to ERROR)
	 * [27]: When set, if sata_5 transaction doesn't hit any window, transaction is sent to
	 * NB/DRAM (otherwise to ERROR)
	 * [28]: When set, if sata_6 transaction doesn't hit any window, transaction is sent to
	 * NB/DRAM (otherwise to ERROR)
	 * [29]: When set, if sata_7 transaction doesn't hit any window, transaction is sent to
	 * NB/DRAM (otherwise to ERROR)
	 * [30]: Reserved
	 * [31]: When set, if eth_db transaction doesn't hit any window, transaction is sent to
	 * NB/DRAM (otherwise to ERROR)
	 */
	uint32_t dma_io_master_map1;
	/*
	 * [0x1e4] Control
	 * [0]: When set, maps all the crypto_0 transactions to the NB/DRAM, regardless of the
	 * window hit.
	 * [1]: When set, maps all the crypto_1 transactions to the NB/DRAM, regardless of the
	 * window hit.
	 * [2]: When set, maps all the eth_0 transactions to the NB/DRAM, regardless of the window
	 * hit.
	 * [3]: When set, maps all the eth_1 transactions to the NB/DRAM, regardless of the window
	 * hit.
	 * [4]: When set, maps all the eth_2 transaction to the NB/DRAM, regardless of the window
	 * hit.
	 * [5]: When set, maps all the eth_3 transaction to the NB/DRAM, regardless of the window
	 * hit.
	 * [6]: When set, maps all the eth_4 transaction to the NB/DRAM, regardless of the window
	 * hit.
	 * [7]: When set, maps all the eth_5 transaction to the NB/DRAM, regardless of the window
	 * hit.
	 * [9:8] - Reserved
	 * [10]: When set, maps all the SPI debug port transactions to the NB/DRAM, regardless of
	 * the window hit.
	 * [11]: When set, maps all the CPU debug port transactions to the NB/DRAM, regardless of
	 * the window hit.
	 * [12]: When set, maps all the pcie_0 transactions to the NB/DRAM, regardless of the window
	 * hit.
	 * [13]: When set, maps all the pcie_1 transactions to the NB/DRAM, regardless of the window
	 * hit.
	 * [14]: When set, maps all the pcie_2 transactions to the NB/DRAM, regardless of the window
	 * hit.
	 * [15]: When set, maps all the pcie_3 transactions to the NB/DRAM, regardless of the window
	 * hit.
	 * [16]: When set, maps all the pcie_4 transactions to the NB/DRAM, regardless of the window
	 * hit.
	 * [17]: When set, maps all the pcie_5 transactions to the NB/DRAM, regardless of the window
	 * hit.
	 * [18]: When set, maps all the pcie_6 transactions to the NB/DRAM, regardless of the window
	 * hit.
	 * [19]: When set, maps all the pcie_7 transactions to the NB/DRAM, regardless of the window
	 * hit.
	 * [21:20] - Reserved
	 * [22]: When set, maps all the sata_0 transactions to the NB/DRAM, regardless of the window
	 * hit.
	 * [23]: When set, maps all the sata_1 transactions to the NB/DRAM, regardless of the window
	 * hit.
	 * [24]: When set, maps all the sata_2 transactions to the NB/DRAM, regardless of the window
	 * hit.
	 * [25]: When set, maps all the sata_3 transactions to the NB/DRAM, regardless of the window
	 * hit.
	 * [26]: When set, maps all the sata_4 transactions to the NB/DRAM, regardless of the window
	 * hit.
	 * [27]: When set, maps all the sata_5 transactions to the NB/DRAM, regardless of the window
	 * hit.
	 * [28]: When set, maps all the sata_6 transactions to the NB/DRAM, regardless of the window
	 * hit.
	 * [29]: When set, maps all the sata_7 transactions to the NB/DRAM, regardless of the window
	 * hit.
	 * [30] - Reserved
	 * [31]: When set, maps all the eth_db transactions to the NB/DRAM, regardless of the window
	 * hit.
	 */
	uint32_t dma_io_master_map2;
	/*
	 * [0x1e8] PASW remap register
	 * The default configuration maps 1GB from address 0x5 C000 0000 into 0xC000 0000.
	 * Currently Cfg_dram_remap_bar is by default 0.
	 * Should be:
	 * Cfg_dram_remap_bar[1:0] = 2'b10 (1GB)
	 * Cfg_dram_remap_bar[15:5] = 11'h02E (win address to compare to against addr[39:29])
	 * Cfg_dram_remap_bar[31:21] = 11'h006 (remap address to switch for addr[39:29])
	 */
	uint32_t sb2nb_cfg_dram_remap;
	uint32_t rsrvd[5];
};

struct al_pbs_low_latency_sram_remap {
	/* [0x0] PBS MEM Remap */
	uint32_t bar1_orig;
	/* [0x4] PBS MEM Remap */
	uint32_t bar1_remap;
	/* [0x8] ETH0 MEM Remap */
	uint32_t bar2_orig;
	/* [0xc] ETH0 MEM Remap */
	uint32_t bar2_remap;
	/* [0x10] ETH1 MEM Remap */
	uint32_t bar3_orig;
	/* [0x14] ETH1 MEM Remap */
	uint32_t bar3_remap;
	/* [0x18] ETH2 MEM Remap */
	uint32_t bar4_orig;
	/* [0x1c] ETH2 MEM Remap */
	uint32_t bar4_remap;
	/* [0x20] ETH3 MEM Remap */
	uint32_t bar5_orig;
	/* [0x24] ETH3 MEM Remap */
	uint32_t bar5_remap;
	/* [0x28] CRYPTO0 MEM Remap */
	uint32_t bar6_orig;
	/* [0x2c] CRYPTO0 MEM Remap */
	uint32_t bar6_remap;
	/* [0x30] RAID0 MEM Remap */
	uint32_t bar7_orig;
	/* [0x34] RAID0 MEM Remap */
	uint32_t bar7_remap;
	/* [0x38] CRYPTO1 MEM Remap */
	uint32_t bar8_orig;
	/* [0x3c] CRYPTO1 MEM Remap */
	uint32_t bar8_remap;
	/* [0x40] RAID1 MEM Remap */
	uint32_t bar9_orig;
	/* [0x44] RAID1 MEM Remap */
	uint32_t bar9_remap;
	/* [0x48] RESERVED MEM Remap */
	uint32_t bar10_orig;
	/* [0x4c] RESERVED MEM Remap */
	uint32_t bar10_remap;
};
struct al_pbs_target_id_enforcement_v2 {
	/* [0x0] target enforcement */
	uint32_t cpu;
	/* [0x4] target enforcement mask (bits which are 0 are not compared) */
	uint32_t cpu_mask;
	/* [0x8] target enforcement */
	uint32_t debug_nb;
	/* [0xc] target enforcement mask (bits which are 0 are not compared) */
	uint32_t debug_nb_mask;
	/* [0x10] target enforcement */
	uint32_t debug_sb;
	/* [0x14] target enforcement mask (bits which are 0 are not compared) */
	uint32_t debug_sb_mask;
	/* [0x18] target enforcement */
	uint32_t eth_0;
	/* [0x1c] target enforcement mask (bits which are 0 are not compared) */
	uint32_t eth_0_mask;
	/* [0x20] target enforcement */
	uint32_t eth_1;
	/* [0x24] target enforcement mask (bits which are 0 are not compared) */
	uint32_t eth_1_mask;
	/* [0x28] target enforcement */
	uint32_t eth_2;
	/* [0x2c] target enforcement mask (bits which are 0 are not compared) */
	uint32_t eth_2_mask;
	/* [0x30] target enforcement */
	uint32_t eth_3;
	/* [0x34] target enforcement mask (bits which are 0 are not compared) */
	uint32_t eth_3_mask;
	/* [0x38] target enforcement */
	uint32_t sata_0;
	/* [0x3c] target enforcement mask (bits which are 0 are not compared) */
	uint32_t sata_0_mask;
	/* [0x40] target enforcement */
	uint32_t sata_1;
	/* [0x44] target enforcement mask (bits which are 0 are not compared) */
	uint32_t sata_1_mask;
	/* [0x48] target enforcement */
	uint32_t crypto_0;
	/* [0x4c] target enforcement mask (bits which are 0 are not compared) */
	uint32_t crypto_0_mask;
	/* [0x50] target enforcement */
	uint32_t crypto_1;
	/* [0x54] target enforcement mask (bits which are 0 are not compared) */
	uint32_t crypto_1_mask;
	/* [0x58] target enforcement */
	uint32_t pcie_0;
	/* [0x5c] target enforcement mask (bits which are 0 are not compared) */
	uint32_t pcie_0_mask;
	/* [0x60] target enforcement */
	uint32_t pcie_1;
	/* [0x64] target enforcement mask (bits which are 0 are not compared) */
	uint32_t pcie_1_mask;
	/* [0x68] target enforcement */
	uint32_t pcie_2;
	/* [0x6c] target enforcement mask (bits which are 0 are not compared) */
	uint32_t pcie_2_mask;
	/* [0x70] target enforcement */
	uint32_t pcie_3;
	/* [0x74] target enforcement mask (bits which are 0 are not compared) */
	uint32_t pcie_3_mask;
	/* [0x78] Control */
	uint32_t latch;
	uint32_t rsrvd[33];
};

struct al_pbs_target_id_enforcement_v3 {
	/* [0x0] target enforcement */
	uint32_t cpu_high;
	/* [0x4] target enforcement */
	uint32_t cpu_low;
	/* [0x8] target enforcement mask (bits which are 0 are not compared) */
	uint32_t cpu_mask_high;
	/* [0xc] target enforcement mask (bits which are 0 are not compared) */
	uint32_t cpu_mask_low;
	/* [0x10] target enforcement */
	uint32_t debug_nb_high;
	/* [0x14] target enforcement */
	uint32_t debug_nb_low;
	/* [0x18] target enforcement mask (bits which are 0 are not compared) */
	uint32_t debug_nb_mask_high;
	/* [0x1c] target enforcement mask (bits which are 0 are not compared) */
	uint32_t debug_nb_mask_low;
	/* [0x20] target enforcement */
	uint32_t debug_sb_high;
	/* [0x24] target enforcement */
	uint32_t debug_sb_low;
	/* [0x28] target enforcement mask (bits which are 0 are not compared) */
	uint32_t debug_sb_mask_high;
	/* [0x2c] target enforcement mask (bits which are 0 are not compared) */
	uint32_t debug_sb_mask_low;
	/* [0x30] target enforcement */
	uint32_t eth_fast_0_high;
	/* [0x34] target enforcement */
	uint32_t eth_fast_0_low;
	/* [0x38] target enforcement mask (bits which are 0 are not compared) */
	uint32_t eth_fast_0_mask_high;
	/* [0x3c] target enforcement mask (bits which are 0 are not compared) */
	uint32_t eth_fast_0_mask_low;
	/* [0x40] target enforcement */
	uint32_t eth_fast_1_high;
	/* [0x44] target enforcement */
	uint32_t eth_fast_1_low;
	/* [0x48] target enforcement mask (bits which are 0 are not compared) */
	uint32_t eth_fast_1_mask_high;
	/* [0x4c] target enforcement mask (bits which are 0 are not compared) */
	uint32_t eth_fast_1_mask_low;
	/* [0x50] target enforcement */
	uint32_t eth_fast_2_high;
	/* [0x54] target enforcement */
	uint32_t eth_fast_2_low;
	/* [0x58] target enforcement mask (bits which are 0 are not compared) */
	uint32_t eth_fast_2_mask_high;
	/* [0x5c] target enforcement mask (bits which are 0 are not compared) */
	uint32_t eth_fast_2_mask_low;
	/* [0x60] target enforcement */
	uint32_t eth_fast_3_high;
	/* [0x64] target enforcement */
	uint32_t eth_fast_3_low;
	/* [0x68] target enforcement mask (bits which are 0 are not compared) */
	uint32_t eth_fast_3_mask_high;
	/* [0x6c] target enforcement mask (bits which are 0 are not compared) */
	uint32_t eth_fast_3_mask_low;
	/* [0x70] target enforcement */
	uint32_t sata_0_high;
	/* [0x74] target enforcement */
	uint32_t sata_0_low;
	/* [0x78] target enforcement mask (bits which are 0 are not compared) */
	uint32_t sata_0_mask_high;
	/* [0x7c] target enforcement mask (bits which are 0 are not compared) */
	uint32_t sata_0_mask_low;
	/* [0x80] target enforcement */
	uint32_t sata_1_high;
	/* [0x84] target enforcement */
	uint32_t sata_1_low;
	/* [0x88] target enforcement mask (bits which are 0 are not compared) */
	uint32_t sata_1_mask_high;
	/* [0x8c] target enforcement mask (bits which are 0 are not compared) */
	uint32_t sata_1_mask_low;
	/* [0x90] target enforcement */
	uint32_t sata_2_high;
	/* [0x94] target enforcement */
	uint32_t sata_2_low;
	/* [0x98] target enforcement mask (bits which are 0 are not compared) */
	uint32_t sata_2_mask_high;
	/* [0x9c] target enforcement mask (bits which are 0 are not compared) */
	uint32_t sata_2_mask_low;
	/* [0xa0] target enforcement */
	uint32_t sata_3_high;
	/* [0xa4] target enforcement */
	uint32_t sata_3_low;
	/* [0xa8] target enforcement mask (bits which are 0 are not compared) */
	uint32_t sata_3_mask_high;
	/* [0xac] target enforcement mask (bits which are 0 are not compared) */
	uint32_t sata_3_mask_low;
	/* [0xb0] target enforcement */
	uint32_t sata_4_high;
	/* [0xb4] target enforcement */
	uint32_t sata_4_low;
	/* [0xb8] target enforcement mask (bits which are 0 are not compared) */
	uint32_t sata_4_mask_high;
	/* [0xbc] target enforcement mask (bits which are 0 are not compared) */
	uint32_t sata_4_mask_low;
	/* [0xc0] target enforcement */
	uint32_t sata_5_high;
	/* [0xc4] target enforcement */
	uint32_t sata_5_low;
	/* [0xc8] target enforcement mask (bits which are 0 are not compared) */
	uint32_t sata_5_mask_high;
	/* [0xcc] target enforcement mask (bits which are 0 are not compared) */
	uint32_t sata_5_mask_low;
	/* [0xd0] target enforcement */
	uint32_t crypto_0_high;
	/* [0xd4] target enforcement */
	uint32_t crypto_0_low;
	/* [0xd8] target enforcement mask (bits which are 0 are not compared) */
	uint32_t crypto_0_mask_high;
	/* [0xdc] target enforcement mask (bits which are 0 are not compared) */
	uint32_t crypto_0_mask_low;
	/* [0xe0] target enforcement */
	uint32_t crypto_1_high;
	/* [0xe4] target enforcement */
	uint32_t crypto_1_low;
	/* [0xe8] target enforcement mask (bits which are 0 are not compared) */
	uint32_t crypto_1_mask_high;
	/* [0xec] target enforcement mask (bits which are 0 are not compared) */
	uint32_t crypto_1_mask_low;
	uint32_t rsrvd_0[4];
	/* [0x100] target enforcement */
	uint32_t pcie_0_high;
	/* [0x104] target enforcement */
	uint32_t pcie_0_low;
	/* [0x108] target enforcement mask (bits which are 0 are not compared) */
	uint32_t pcie_0_mask_high;
	/* [0x10c] target enforcement mask (bits which are 0 are not compared) */
	uint32_t pcie_0_mask_low;
	/* [0x110] target enforcement */
	uint32_t pcie_1_high;
	/* [0x114] target enforcement */
	uint32_t pcie_1_low;
	/* [0x118] target enforcement mask (bits which are 0 are not compared) */
	uint32_t pcie_1_mask_high;
	/* [0x11c] target enforcement mask (bits which are 0 are not compared) */
	uint32_t pcie_1_mask_low;
	/* [0x120] target enforcement */
	uint32_t pcie_2_high;
	/* [0x124] target enforcement */
	uint32_t pcie_2_low;
	/* [0x128] target enforcement mask (bits which are 0 are not compared) */
	uint32_t pcie_2_mask_high;
	/* [0x12c] target enforcement mask (bits which are 0 are not compared) */
	uint32_t pcie_2_mask_low;
	/* [0x130] target enforcement */
	uint32_t pcie_3_high;
	/* [0x134] target enforcement */
	uint32_t pcie_3_low;
	/* [0x138] target enforcement mask (bits which are 0 are not compared) */
	uint32_t pcie_3_mask_high;
	/* [0x13c] target enforcement mask (bits which are 0 are not compared) */
	uint32_t pcie_3_mask_low;
	/* [0x140] target enforcement */
	uint32_t pcie_4_high;
	/* [0x144] target enforcement */
	uint32_t pcie_4_low;
	/* [0x148] target enforcement mask (bits which are 0 are not compared) */
	uint32_t pcie_4_mask_high;
	/* [0x14c] target enforcement mask (bits which are 0 are not compared) */
	uint32_t pcie_4_mask_low;
	/* [0x150] target enforcement */
	uint32_t pcie_5_high;
	/* [0x154] target enforcement */
	uint32_t pcie_5_low;
	/* [0x158] target enforcement mask (bits which are 0 are not compared) */
	uint32_t pcie_5_mask_high;
	/* [0x15c] target enforcement mask (bits which are 0 are not compared) */
	uint32_t pcie_5_mask_low;
	/* [0x160] target enforcement */
	uint32_t pcie_6_high;
	/* [0x164] target enforcement */
	uint32_t pcie_6_low;
	/* [0x168] target enforcement mask (bits which are 0 are not compared) */
	uint32_t pcie_6_mask_high;
	/* [0x16c] target enforcement mask (bits which are 0 are not compared) */
	uint32_t pcie_6_mask_low;
	/* [0x170] target enforcement */
	uint32_t eth_0_high;
	/* [0x174] target enforcement */
	uint32_t eth_0_low;
	/* [0x178] target enforcement mask (bits which are 0 are not compared) */
	uint32_t eth_0_mask_high;
	/* [0x17c] target enforcement mask (bits which are 0 are not compared) */
	uint32_t eth_0_mask_low;
	/* [0x180] target enforcement */
	uint32_t eth_1_high;
	/* [0x184] target enforcement */
	uint32_t eth_1_low;
	/* [0x188] target enforcement mask (bits which are 0 are not compared) */
	uint32_t eth_1_mask_high;
	/* [0x18c] target enforcement mask (bits which are 0 are not compared) */
	uint32_t eth_1_mask_low;
	/* [0x190] target enforcement */
	uint32_t sata_6_high;
	/* [0x194] target enforcement */
	uint32_t sata_6_low;
	/* [0x198] target enforcement mask (bits which are 0 are not compared) */
	uint32_t sata_6_mask_high;
	/* [0x19c] target enforcement mask (bits which are 0 are not compared) */
	uint32_t sata_6_mask_low;
	/* [0x1a0] target enforcement */
	uint32_t sata_7_high;
	/* [0x1a4] target enforcement */
	uint32_t sata_7_low;
	/* [0x1a8] target enforcement mask (bits which are 0 are not compared) */
	uint32_t sata_7_mask_high;
	/* [0x1ac] target enforcement mask (bits which are 0 are not compared) */
	uint32_t sata_7_mask_low;
	/* [0x1b0] target enforcement */
	uint32_t eth_db_high;
	/* [0x1b4] target enforcement */
	uint32_t eth_db_low;
	/* [0x1b8] target enforcement mask (bits which are 0 are not compared) */
	uint32_t eth_db_mask_high;
	/* [0x1bc] target enforcement mask (bits which are 0 are not compared) */
	uint32_t eth_db_mask_low;
	/* [0x1c0] target enforcement */
	uint32_t pcie_7_high;
	/* [0x1c4] target enforcement */
	uint32_t pcie_7_low;
	/* [0x1c8] target enforcement mask (bits which are 0 are not compared) */
	uint32_t pcie_7_mask_high;
	/* [0x1cc] target enforcement mask (bits which are 0 are not compared) */
	uint32_t pcie_7_mask_low;
	uint32_t rsrvd_1[8];
	/* [0x1f0] Control */
	uint32_t latch;
	uint32_t rsrvd[3];
};

struct al_pbs_spi_debug_pasw {
	/* [0x0] TZPASW */
	uint32_t spi_debug_pasw_high;
	/* [0x4] TZPASW */
	uint32_t spi_debug_pasw_low;
};

struct al_pbs_spi_debug_pasw_enable {
	/* [0x0] */
	uint32_t spi_debug_pasw_enable_reg;
	/* [0x4] */
	uint32_t spi_debug_pasw_write_protect_reg;
	/* [0x8] */
	uint32_t spi_debug_pasw_read_protect_reg;
	uint32_t rsrvd;
};

struct al_pbs_regs {
	/* [0x0] */
	struct al_pbs_unit unit;
	uint32_t iofic_base;						/* [0x300] */
	uint32_t rsrvd_0[63];
	struct al_pbs_target_id_enforcement_v2 target_id_enforcement_v2;	/* [0x400] */
	/*
	 * [0x500] INTCA:
	 * intc_triggers_a_pbs[31] = cpu_rd_nic_sram_out_parity_err | cpu_rd_nic_sram_in_parity_err
	 * intc_triggers_a_pbs[30] = fts_rd_sram_out_parity_err
	 * intc_triggers_a_pbs[29] = fts_rd_sram_in_parity_err
	 * intc_triggers_a_pbs[28] = fts_wr_sram_out_parity_err
	 * intc_triggers_a_pbs[27] = fts_wr_sram_in_parity_err
	 * intc_triggers_a_pbs[26] = pcie2_pll_test
	 * intc_triggers_a_pbs[25] = pcie1_pll_test
	 * intc_triggers_a_pbs[24] = pcie0_pll_test
	 * intc_triggers_a_pbs[23] = tdm_zsi
	 * intc_triggers_a_pbs[22] = tdm_spi
	 * intc_triggers_a_pbs[21] = tdm_dbase_rd_par_err
	 * intc_triggers_a_pbs[20] = tdm_tx_par_err
	 * intc_triggers_a_pbs[19] = tdm_rx_empty
	 * intc_triggers_a_pbs[18] = tdm_rx_overrun
	 * intc_triggers_a_pbs[17] = tdm_rx_full
	 * intc_triggers_a_pbs[16] = tdm_rx_afull
	 * intc_triggers_a_pbs[15] = tdm_tx_empty
	 * intc_triggers_a_pbs[14] = tdm_tx_aempty
	 * intc_triggers_a_pbs[13] = tdm_tx_full
	 * intc_triggers_a_pbs[12:11] = cpu_pll_test[1:0]
	 * intc_triggers_a_pbs[10] = nb_pll_test
	 * intc_triggers_a_pbs[9] = sb_pll_test
	 * intc_triggers_a_pbs[8] = temp_sense_low_temp
	 * intc_triggers_a_pbs[7] = temp_sense_high_temp
	 * intc_triggers_a_pbs[6] = temp_sense_result
	 * intc_triggers_a_pbs[5] = error_2_pready
	 * intc_triggers_a_pbs[4] = error_pready_wo_psel;
	 * intc_triggers_a_pbs[3] = |pbs_ufc_parity_error;
	 * intc_triggers_a_pbs[2] = pbs_bootrom_parity_error;
	 * intc_triggers_a_pbs[1] = pbs_ufcecc_corr_err;
	 * intc_triggers_a_pbs[0] = (pbs_ufc_ecc_uncorr_err);
	 */
	uint32_t iofic_base_v3;					/* [0x500] */
	uint32_t rsrvd_1[63];
	/* [0x600] */
	struct al_pbs_qspi qspi;
	/* [0x650] */
	struct al_pbs_spim_preload spim_preload;
	/* [0x670] */
	uint32_t rsrvd_2[36];
	/* [0x700] */
	struct al_pbs_chip_capabilities chip_capabilities;
	/* [0x740] */
	uint32_t rsrvd_3[560];
	/* [0x1000] */
	struct al_pbs_bar_config bar_config;
	/* [0x1200] */
	struct al_pbs_low_latency_sram_remap low_latency_sram_remap;
	/* [0x1250] */
	uint32_t rsrvd_4[44];
	/* [0x1300] */
	struct al_pbs_target_id_enforcement_v3 target_id_enforcement;
	/* [0x1500] */
	struct al_pbs_spi_debug_pasw spi_debug_pasw[16];
	/* [0x1580] */
	uint32_t rsrvd_5[32];
	/* [0x1600] */
	struct al_pbs_spi_debug_pasw_enable spi_debug_pasw_enable;
};


/*
 * Registers Fields
 */

/**** conf_bus register ****/
/* Read slave error enable */
#define PBS_UNIT_CONF_BUS_RD_SLVERR_EN   (1 << 0)
/* Write slave error enable */
#define PBS_UNIT_CONF_BUS_WR_SLVERR_EN   (1 << 1)
/* Read decode error enable */
#define PBS_UNIT_CONF_BUS_RD_DECERR_EN   (1 << 2)
/* Write decode error enable */
#define PBS_UNIT_CONF_BUS_WR_DECERR_EN   (1 << 3)
/* For debug clear the APB SM */
#define PBS_UNIT_CONF_BUS_CLR_APB_FSM    (1 << 4)
/* For debug clear the WFIFO */
#define PBS_UNIT_CONF_BUS_CLR_WFIFO_CLEAR (1 << 5)
/* Arbiter between read and write channel */
#define PBS_UNIT_CONF_BUS_WRR_CNT_MASK   0x000001C0
#define PBS_UNIT_CONF_BUS_WRR_CNT_SHIFT  6

/**** pcie_conf_0 register ****/
/* Reserved */
#define PBS_UNIT_PCIE_CONF_0_DEVS_TYPE_MASK 0x00000FFF
#define PBS_UNIT_PCIE_CONF_0_DEVS_TYPE_SHIFT 0
/* sys_aux_det value */
#define PBS_UNIT_PCIE_CONF_0_SYS_AUX_PWR_DET_VEC_MASK 0x000FF000
#define PBS_UNIT_PCIE_CONF_0_SYS_AUX_PWR_DET_VEC_SHIFT 12
/* Reserved */
#define PBS_UNIT_PCIE_CONF_0_RSRVD_MASK  0xFFF00000
#define PBS_UNIT_PCIE_CONF_0_RSRVD_SHIFT 20

/**** pcie_conf_1 register ****/
/* Which PCIe exists? The PCIe device is under reset until the coresponding bit is set. */
#define PBS_UNIT_PCIE_CONF_1_PCIE_EXIST_MASK 0x000000FF
#define PBS_UNIT_PCIE_CONF_1_PCIE_EXIST_SHIFT 0
/* Reserved */
#define PBS_UNIT_PCIE_CONF_1_RSRVD_MASK  0xFFFFFF00
#define PBS_UNIT_PCIE_CONF_1_RSRVD_SHIFT 8


/* general PASWS */
/* window size = 2 ^ (15 + win_size), zero value disable the win ... */
#define PBS_PASW_WIN_SIZE_MASK 0x0000003F
#define PBS_PASW_WIN_SIZE_SHIFT 0
/* reserved fields */
#define PBS_PASW_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_PASW_BAR_LOW_RSRVD_SHIFT 6
/* bar low address 16 MSB bits */
#define PBS_PASW_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_PASW_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** winit_cntl register ****/
/* When set, enables access to winit regs, in normal mode. */
#define PBS_UNIT_WINIT_CNTL_ENABLE_WINIT_REGS_ACCESS (1 << 0)
/* Reserved */
#define PBS_UNIT_WINIT_CNTL_RSRVD_MASK   0xFFFFFFFE
#define PBS_UNIT_WINIT_CNTL_RSRVD_SHIFT  1

/**** latch_bars register ****/
/*
 * Software clears this bit before any bar update, and set it after all bars
 * updated.
 */
#define PBS_UNIT_LATCH_BARS_ENABLE       (1 << 0)
/* Reserved */
#define PBS_UNIT_LATCH_BARS_RSRVD_MASK   0xFFFFFFFE
#define PBS_UNIT_LATCH_BARS_RSRVD_SHIFT  1

/**** serdes_mux_pipe register ****/
/* SerDes one hot mux control.  For details see datasheet.  */
#define PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_SERDES_2_MASK 0x00000007
#define PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_SERDES_2_SHIFT 0
/* Reserved */
#define PBS_UNIT_SERDES_MUX_PIPE_RSRVD_3 (1 << 3)
/* SerDes one hot mux control.  For details see datasheet.  */
#define PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_SERDES_3_MASK 0x00000070
#define PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_SERDES_3_SHIFT 4
/* Reserved */
#define PBS_UNIT_SERDES_MUX_PIPE_RSRVD_7 (1 << 7)
/* SerDes one hot mux control.  For details see datasheet.  */
#define PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_PCI_B_0_MASK 0x00000300
#define PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_PCI_B_0_SHIFT 8
/* SerDes one hot mux control.  For details see datasheet.  */
#define PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_PCI_B_1_MASK 0x00000C00
#define PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_PCI_B_1_SHIFT 10
/* SerDes one hot mux control.  For details see datasheet.  */
#define PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_PCI_C_0_MASK 0x00003000
#define PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_PCI_C_0_SHIFT 12
/* SerDes one hot mux control.  For details see datasheet.  */
#define PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_PCI_C_1_MASK 0x0000C000
#define PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_PCI_C_1_SHIFT 14
/* SerDes one hot mux control.  For details see datasheet.  */
#define PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_USB_A_0_MASK 0x00030000
#define PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_USB_A_0_SHIFT 16
/* SerDes one hot mux control.  For details see datasheet.  */
#define PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_USB_B_0_MASK 0x000C0000
#define PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_USB_B_0_SHIFT 18
/* SerDes one hot mux control.  For details see datasheet.  */
#define PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_CLKI_SER_2_MASK 0x00300000
#define PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_CLKI_SER_2_SHIFT 20
/* Reserved */
#define PBS_UNIT_SERDES_MUX_PIPE_RSRVD_23_22_MASK 0x00C00000
#define PBS_UNIT_SERDES_MUX_PIPE_RSRVD_23_22_SHIFT 22
/* SerDes one hot mux control.  For details see datasheet.  */
#define PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_CLKI_SER_3_MASK 0x07000000
#define PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_CLKI_SER_3_SHIFT 24
/* Reserved */
#define PBS_UNIT_SERDES_MUX_PIPE_RSRVD_MASK 0xF8000000
#define PBS_UNIT_SERDES_MUX_PIPE_RSRVD_SHIFT 27

/*
 * 2'b01 - select pcie_b[0]
 * 2'b10 - select pcie_a[2]
 */
#define PBS_UNIT_SERDES_MUX_PIPE_ALPINE_V2_SELECT_OH_SERDES_2_MASK 0x00000003
#define PBS_UNIT_SERDES_MUX_PIPE_ALPINE_V2_SELECT_OH_SERDES_2_SHIFT 0
/*
 * 2'b01 - select pcie_b[1]
 * 2'b10 - select pcie_a[3]
 */
#define PBS_UNIT_SERDES_MUX_PIPE_ALPINE_V2_SELECT_OH_SERDES_3_MASK 0x00000030
#define PBS_UNIT_SERDES_MUX_PIPE_ALPINE_V2_SELECT_OH_SERDES_3_SHIFT 4
/*
 * 2'b01 - select pcie_b[0]
 * 2'b10 - select pcie_a[4]
 */
#define PBS_UNIT_SERDES_MUX_PIPE_ALPINE_V2_SELECT_OH_SERDES_4_MASK 0x00000300
#define PBS_UNIT_SERDES_MUX_PIPE_ALPINE_V2_SELECT_OH_SERDES_4_SHIFT 8
/*
 * 2'b01 - select pcie_b[1]
 * 2'b10 - select pcie_a[5]
 */
#define PBS_UNIT_SERDES_MUX_PIPE_ALPINE_V2_SELECT_OH_SERDES_5_MASK 0x00003000
#define PBS_UNIT_SERDES_MUX_PIPE_ALPINE_V2_SELECT_OH_SERDES_5_SHIFT 12
/*
 * 2'b01 - select pcie_b[2]
 * 2'b10 - select pcie_a[6]
 */
#define PBS_UNIT_SERDES_MUX_PIPE_ALPINE_V2_SELECT_OH_SERDES_6_MASK 0x00030000
#define PBS_UNIT_SERDES_MUX_PIPE_ALPINE_V2_SELECT_OH_SERDES_6_SHIFT 16
/*
 * 2'b01 - select pcie_b[3]
 * 2'b10 - select pcie_a[7]
 */
#define PBS_UNIT_SERDES_MUX_PIPE_ALPINE_V2_SELECT_OH_SERDES_7_MASK 0x00300000
#define PBS_UNIT_SERDES_MUX_PIPE_ALPINE_V2_SELECT_OH_SERDES_7_SHIFT 20
/*
 * 2'b01 - select pcie_d[0]
 * 2'b10 - select pcie_c[2]
 */
#define PBS_UNIT_SERDES_MUX_PIPE_ALPINE_V2_SELECT_OH_SERDES_10_MASK 0x03000000
#define PBS_UNIT_SERDES_MUX_PIPE_ALPINE_V2_SELECT_OH_SERDES_10_SHIFT 24
/*
 * 2'b01 - select pcie_d[1]
 * 2'b10 - select pcie_c[3]
 */
#define PBS_UNIT_SERDES_MUX_PIPE_ALPINE_V2_SELECT_OH_SERDES_11_MASK 0x30000000
#define PBS_UNIT_SERDES_MUX_PIPE_ALPINE_V2_SELECT_OH_SERDES_11_SHIFT 28

/**** dma_io_master_map register ****/
/*
 * [0]: When set, maps all the crypto_0  transactions to the NB/DRAM, regardless of the window hit.
 * [1]: When set, maps all the crypto_1  transactions to the NB/DRAM, regardless of the window hit.
 * [2]: When set, maps all the eth_0 transactions to the NB/DRAM, regardless of the window hit.
 * [3]: When set, maps all the eth_1 transactions to the NB/DRAM, regardless of the window hit.
 * [4]: When set, maps all the eth_2 transaction to the NB/DRAM, regardless of the window hit.
 * [5]: When set, maps all the eth_3 transaction to the NB/DRAM, regardless of the window hit.
 * [6]: When set, maps all the sata_0 transactions to the NB/DRAM, regardless of the window hit.
 * [7]: When set, maps all the sata_1 transactions to the NB/DRAM, regardless of the window hit.
 * [8]: When set, maps all the pcie_0 master  transactions to the NB/DRAM, regardless of the window
 * hit.
 * [9]: When set, maps all the pcie_1 master  transactions to the NB/DRAM, regardless of the window
 * hit.
 * [10]: When set, maps all the pcie_2 master  transactions to the NB/DRAM, regardless of the window
 * hit.
 * [11]: When set, maps all the pcie_3 master  transactions to the NB/DRAM, regardless of the window
 * hit.
 * [12]: When set, maps all the SPI debug port  transactions to the NB/DRAM, regardless of the
 * window hit.
 * [13]: When set, maps all the CPU debug port  transactions to the NB/DRAM, regardless of the
 * window hit.
 * [15:14] - Reserved
 */
#define PBS_UNIT_DMA_IO_MASTER_MAP_CNTL_MASK 0x0000FFFF
#define PBS_UNIT_DMA_IO_MASTER_MAP_CNTL_SHIFT 0
/*
 * [0]: When set, if crypto_0  transaction doesn't hit any window, transaction is sent to NB/DRAM
 * (otherwise to ERROR)
 * [1]: When set, if crypto_1  transaction doesn't hit any window, transaction is sent to NB/DRAM
 * (otherwise to ERROR)
 * [2]: When set, if eth_0  transaction doesn't hit any window, transaction is sent to NB/DRAM
 * (otherwise to ERROR)
 * [3]: When set, if eth_1  transaction doesn't hit any window, transaction is sent to NB/DRAM
 * (otherwise to ERROR)
 * [4]: When set, if eth_2  transaction doesn't hit any window, transaction is sent to NB/DRAM
 * (otherwise to ERROR)
 * [5]: When set, if eth_3  transaction doesn't hit any window, transaction is sent to NB/DRAM
 * (otherwise to ERROR)
 * [6]: When set, if sata_0  transaction doesn't hit any window, transaction is sent to NB/DRAM
 * (otherwise to ERROR)
 * [7]: When set, if sata_1  transaction doesn't hit any window, transaction is sent to NB/DRAM
 * (otherwise to ERROR)
 * [8]: When set, if pcie_0  transaction doesn't hit any window, transaction is sent to NB/DRAM
 * (otherwise to ERROR)
 * [9]: When set, if pcie_1  transaction doesn't hit any window, transaction is sent to NB/DRAM
 * (otherwise to ERROR)
 * [10]: When set, if pcie_2  transaction doesn't hit any window, transaction is sent to NB/DRAM
 * (otherwise to ERROR)
 * [11]: When set, if pcie_3  transaction doesn't hit any window, transaction is sent to NB/DRAM
 * (otherwise to ERROR)
 * [12]: When set, if SPI_DEBUG transaction doesn't hit any window, transaction is sent to NB/DRAM
 * (otherwise to ERROR)
 * [13]: When set, if CPU_DEBUG transaction doesn't hit any window, transaction is sent to NB/DRAM
 * (otherwise to ERROR)
 * [15:14] - Reserved
 */
#define PBS_UNIT_DMA_IO_MASTER_MAP_DFLT_SLV_MASK 0xFFFF0000
#define PBS_UNIT_DMA_IO_MASTER_MAP_DFLT_SLV_SHIFT 16

/**** i2c_pld_status_high register ****/
/* I2C pre-load status  */
#define PBS_UNIT_I2C_PLD_STATUS_HIGH_STATUS_MASK 0x000000FF
#define PBS_UNIT_I2C_PLD_STATUS_HIGH_STATUS_SHIFT 0

/**** spi_dbg_status_high register ****/
/* SPI DBG load status */
#define PBS_UNIT_SPI_DBG_STATUS_HIGH_STATUS_MASK 0x000000FF
#define PBS_UNIT_SPI_DBG_STATUS_HIGH_STATUS_SHIFT 0

/**** spi_mst_status_high register ****/
/* SP IMST load status */
#define PBS_UNIT_SPI_MST_STATUS_HIGH_STATUS_MASK 0x000000FF
#define PBS_UNIT_SPI_MST_STATUS_HIGH_STATUS_SHIFT 0

/**** mem_pbs_parity_err_high register ****/
/* Address latch in the case of a parity error */
#define PBS_UNIT_MEM_PBS_PARITY_ERR_HIGH_ADDR_MASK 0x000000FF
#define PBS_UNIT_MEM_PBS_PARITY_ERR_HIGH_ADDR_SHIFT 0

/**** cfg_axi_conf_0 register ****/
/* Sets the AXI field in the I2C preloader  interface. */
#define PBS_UNIT_CFG_AXI_CONF_0_DBG_RD_ID_MASK 0x0000007F
#define PBS_UNIT_CFG_AXI_CONF_0_DBG_RD_ID_SHIFT 0
/* Sets the AXI field in the I2C preloader  interface. */
#define PBS_UNIT_CFG_AXI_CONF_0_DBG_WR_ID_MASK 0x00003F80
#define PBS_UNIT_CFG_AXI_CONF_0_DBG_WR_ID_SHIFT 7
/* Sets the AXI field in the I2C preloader  interface. */
#define PBS_UNIT_CFG_AXI_CONF_0_PLD_WR_ID_MASK 0x001FC000
#define PBS_UNIT_CFG_AXI_CONF_0_PLD_WR_ID_SHIFT 14
/* Sets the AXI field in the SPI debug interface. */
#define PBS_UNIT_CFG_AXI_CONF_0_DBG_AWCACHE_MASK 0x01E00000
#define PBS_UNIT_CFG_AXI_CONF_0_DBG_AWCACHE_SHIFT 21
/* Sets the AXI field in the SPI debug interface. */
#define PBS_UNIT_CFG_AXI_CONF_0_DBG_ARCACHE_MASK 0x1E000000
#define PBS_UNIT_CFG_AXI_CONF_0_DBG_ARCACHE_SHIFT 25
/* Sets the AXI field in the SPI debug interface. */
#define PBS_UNIT_CFG_AXI_CONF_0_DBG_AXPROT_MASK 0xE0000000
#define PBS_UNIT_CFG_AXI_CONF_0_DBG_AXPROT_SHIFT 29

/**** cfg_axi_conf_1 register ****/
/* Sets the AXI field in the SPI debug interface. */
#define PBS_UNIT_CFG_AXI_CONF_1_DBG_ARUSER_MASK 0x03FFFFFF
#define PBS_UNIT_CFG_AXI_CONF_1_DBG_ARUSER_SHIFT 0
/* Sets the AXI field in the SPI debug interface. */
#define PBS_UNIT_CFG_AXI_CONF_1_DBG_ARQOS_MASK 0x3C000000
#define PBS_UNIT_CFG_AXI_CONF_1_DBG_ARQOS_SHIFT 26

/**** cfg_axi_conf_2 register ****/
/* Sets the AXI field in the SPI debug interface. */
#define PBS_UNIT_CFG_AXI_CONF_2_DBG_AWUSER_MASK 0x03FFFFFF
#define PBS_UNIT_CFG_AXI_CONF_2_DBG_AWUSER_SHIFT 0
/* Sets the AXI field in the SPI debug interface. */
#define PBS_UNIT_CFG_AXI_CONF_2_DBG_AWQOS_MASK 0x3C000000
#define PBS_UNIT_CFG_AXI_CONF_2_DBG_AWQOS_SHIFT 26

/**** cfg_axi_conf_3 register ****/
/* Each configuration access is protected by a timeout, which expires when the unit does not respond after the counter reaches the register value limit. */
#define PBS_UNIT_CFG_AXI_CONF_3_TIMEOUT_LOW_MASK	0xFFFF
#define PBS_UNIT_CFG_AXI_CONF_3_TIMEOUT_LOW_SHIFT	0
#define PBS_UNIT_CFG_AXI_CONF_3_TIMEOUT_HI_MASK		0xFF0000
#define PBS_UNIT_CFG_AXI_CONF_3_TIMEOUT_HI_SHIFT	16
#define PBS_UNIT_CFG_AXI_CONF_3_TIMEOUT_SPI_HI_MASK	0xFF000000
#define PBS_UNIT_CFG_AXI_CONF_3_TIMEOUT_SPI_HI_SHIFT	24

/**** spi_mst_conf_0 register ****/
/* Sets the SPI master Configuration. For details see the SPI section in the documentation. */
#define PBS_UNIT_SPI_MST_CONF_0_CFG_SPI_MST_SRL (1 << 0)
/* Sets the SPI master Configuration. For details see the SPI section in the documentation. */
#define PBS_UNIT_SPI_MST_CONF_0_CFG_SPI_MST_SCPOL (1 << 1)
/* Sets the SPI master Configuration. For details see the SPI section in the documentation. */
#define PBS_UNIT_SPI_MST_CONF_0_CFG_SPI_MST_SCPH (1 << 2)
/* Set the SPI master configuration. For details see the SPI section in the documentation. */
#define PBS_UNIT_SPI_MST_CONF_0_CFG_SPI_MST_SER_MASK 0x00000078
#define PBS_UNIT_SPI_MST_CONF_0_CFG_SPI_MST_SER_SHIFT 3
/* Set the SPI master configuration. For details see the SPI section in the documentation. */
#define PBS_UNIT_SPI_MST_CONF_0_CFG_SPI_MST_BAUD_MASK 0x007FFF80
#define PBS_UNIT_SPI_MST_CONF_0_CFG_SPI_MST_BAUD_SHIFT 7
/* Sets the SPI master configuration. For details see the SPI section in the documentation. */
#define PBS_UNIT_SPI_MST_CONF_0_CFG_SPI_MST_RD_CMD_MASK 0x7F800000
#define PBS_UNIT_SPI_MST_CONF_0_CFG_SPI_MST_RD_CMD_SHIFT 23

/**** spi_mst_conf_1 register ****/
/* Sets the SPI master Configuration. For details see the SPI section in the documentation. */
#define PBS_UNIT_SPI_MST_CONF_1_CFG_SPI_MST_WR_CMD_MASK 0x000000FF
#define PBS_UNIT_SPI_MST_CONF_1_CFG_SPI_MST_WR_CMD_SHIFT 0
/* Sets the SPI master Configuration. For details see the SPI section in the documentation. */
#define PBS_UNIT_SPI_MST_CONF_1_CFG_SPI_MST_ADDR_BYTES_NUM_MASK 0x00000700
#define PBS_UNIT_SPI_MST_CONF_1_CFG_SPI_MST_ADDR_BYTES_NUM_SHIFT 8
/* Sets the SPI master Configuration. For details see the SPI section in the documentation. */
#define PBS_UNIT_SPI_MST_CONF_1_CFG_SPI_MST_TMODE_MASK 0x00001800
#define PBS_UNIT_SPI_MST_CONF_1_CFG_SPI_MST_TMODE_SHIFT 11
/* Sets the SPI master Configuration. For details see the SPI section in the documentation. */
#define PBS_UNIT_SPI_MST_CONF_1_CFG_SPI_MST_FAST_RD (1 << 13)
/* Sets the SPI master Configuration. For details see the SPI section in the documentation. */
#define PBS_UNIT_SPI_MST_CONF_1_CFG_SPI_MST_RXDEL_MASK 0x0001C000
#define PBS_UNIT_SPI_MST_CONF_1_CFG_SPI_MST_RXDEL_SHIFT 14

/**** spi_slv_conf_0 register ****/
/* Sets the SPI slave configuration. For details see the SPI section in the documentation. */
#define PBS_UNIT_SPI_SLV_CONF_0_CFG_SPI_SLV_BAUD_MASK 0x0000FFFF
#define PBS_UNIT_SPI_SLV_CONF_0_CFG_SPI_SLV_BAUD_SHIFT 0
/* Value. The reset value is according to bootstrap. */
#define PBS_UNIT_SPI_SLV_CONF_0_CFG_SPI_SLV_SCPOL (1 << 16)
/* Value. The reset value is according to bootstrap. */
#define PBS_UNIT_SPI_SLV_CONF_0_CFG_SPI_SLV_SCPH (1 << 17)
/* Sets the SPI slave configuration. For details see the SPI section in the documentation. */
#define PBS_UNIT_SPI_SLV_CONF_0_CFG_SPI_SLV_SER_MASK 0x03FC0000
#define PBS_UNIT_SPI_SLV_CONF_0_CFG_SPI_SLV_SER_SHIFT 18
/* Sets the SPI slave configuration. For details see the SPI section in the documentation. */
#define PBS_UNIT_SPI_SLV_CONF_0_CFG_SPI_SLV_SRL (1 << 26)
/* Sets the SPI slave configuration. For details see the SPI section in the documentation. */
#define PBS_UNIT_SPI_SLV_CONF_0_CFG_SPI_SLV_TMODE_MASK 0x18000000
#define PBS_UNIT_SPI_SLV_CONF_0_CFG_SPI_SLV_TMODE_SHIFT 27

/**** apb_mem_conf_int register ****/
/* Value-- internal */
#define PBS_UNIT_APB_MEM_CONF_INT_CFG_PBS_WRR_CNT_MASK 0x00000007
#define PBS_UNIT_APB_MEM_CONF_INT_CFG_PBS_WRR_CNT_SHIFT 0
/* Value-- internal */
#define PBS_UNIT_APB_MEM_CONF_INT_CFG_I2C_PLD_APB_MIX_ARB (1 << 3)
/* Value-- internal */
#define PBS_UNIT_APB_MEM_CONF_INT_CFG_SPI_DBG_APB_MIX_ARB (1 << 4)
/* Value-- internal */
#define PBS_UNIT_APB_MEM_CONF_INT_CFG_SPI_MST_APB_MIX_ARB (1 << 5)
/* Value-- internal */
#define PBS_UNIT_APB_MEM_CONF_INT_CFG_I2C_PLD_CLEAR_FSM (1 << 6)
/* Value-- internal */
#define PBS_UNIT_APB_MEM_CONF_INT_CFG_SPI_DBG_CLEAR_FSM (1 << 7)
/* Value-- internal */
#define PBS_UNIT_APB_MEM_CONF_INT_CFG_SPI_MST_CLEAR_FSM (1 << 8)
/* Value-- internal */
#define PBS_UNIT_APB_MEM_CONF_INT_CFG_PBS_AXI_FSM_CLEAR (1 << 9)
/* Value-- internal */
#define PBS_UNIT_APB_MEM_CONF_INT_CFG_PBS_AXI_FIFOS_CLEAR (1 << 10)
/* Enables parity protection on the integrated SRAM. */
#define PBS_UNIT_APB_MEM_CONF_INT_CFG_BOOTROM_PARITY_EN (1 << 11)
/*
 * When set, reports a slave error whenthe slave returns an AXI slave error, for configuration
 * access to the internal configuration space.
 */
#define PBS_UNIT_APB_MEM_CONF_INT_CFG_RD_SLV_ERR_EN (1 << 12)
/*
 * When set, reports a decode error when timeout has occurred for configuration access to the
 * internal configuration space.
 */
#define PBS_UNIT_APB_MEM_CONF_INT_CFG_RD_DEC_ERR_EN (1 << 13)
/*
 * When set, reports a slave error, when the slave returns an AXI slave error, for configuration
 * access to the internal configuration space.
 */
#define PBS_UNIT_APB_MEM_CONF_INT_CFG_WR_SLV_ERR_EN (1 << 14)
/*
 * When set, reports a decode error when timeout has occurred for configuration access to the
 * internal configuration space.
 */
#define PBS_UNIT_APB_MEM_CONF_INT_CFG_WR_DEC_ERR_EN (1 << 15)

/**** ufc_pbs_parity_err_high register ****/
/* Address latch in the case of a parity error in the Flash Controller internal memories. */
#define PBS_UNIT_UFC_PBS_PARITY_ERR_HIGH_ADDR_MASK 0x000000FF
#define PBS_UNIT_UFC_PBS_PARITY_ERR_HIGH_ADDR_SHIFT 0

/**** chip_id register ****/
/* [15:0] : Dev Rev ID */
#define PBS_UNIT_CHIP_ID_DEV_REV_ID_MASK 0x0000FFFF
#define PBS_UNIT_CHIP_ID_DEV_REV_ID_SHIFT 0
/* [31:16] : 0x0 - Dev ID */
#define PBS_UNIT_CHIP_ID_DEV_ID_MASK     0xFFFF0000
#define PBS_UNIT_CHIP_ID_DEV_ID_SHIFT    16

#define PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V1       	0
#define PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V2		1
#define PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V3		2
#define PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V4		3

#define PBS_UNIT_CHIP_ID_DEV_REV_ID_ALPINE_V3_TC	0
#define PBS_UNIT_CHIP_ID_DEV_REV_ID_ALPINE_V3_MAIN	1

/**** uart0_conf_status register ****/
/*
 * Conf:
 * // [0] -- DSR_N RW bit
 * // [1] -- DCD_N RW bit
 * // [2] -- RI_N bit
 * // [3] -- dma_tx_ack_n
 * // [4] - dma_rx_ack_n
 */
#define PBS_UNIT_UART0_CONF_STATUS_CONF_MASK 0x0000FFFF
#define PBS_UNIT_UART0_CONF_STATUS_CONF_SHIFT 0
/*
 * Status:
 * // [16] -- dtr_n RO bit
 * // [17] -- OUT1_N RO bit
 * // [18] -- OUT2_N RO bit
 * // [19] -- dma_tx_req_n RO bit
 * // [20] -- dma_tx_single_n RO bit
 * // [21] -- dma_rx_req_n RO bit
 * // [22] -- dma_rx_single_n RO bit
 * // [23] -- uart_lp_req_pclk RO bit
 * // [24] -- baudout_n RO bit
 */
#define PBS_UNIT_UART0_CONF_STATUS_STATUS_MASK 0xFFFF0000
#define PBS_UNIT_UART0_CONF_STATUS_STATUS_SHIFT 16

/**** uart1_conf_status register ****/
/*
 * Conf: // [0] -- DSR_N RW bit // [1] -- DCD_N RW bit // [2] -- RI_N bit // [3] -- dma_tx_ack_n //
 * [4] - dma_rx_ack_n
 */
#define PBS_UNIT_UART1_CONF_STATUS_CONF_MASK 0x0000FFFF
#define PBS_UNIT_UART1_CONF_STATUS_CONF_SHIFT 0
/*
 * Status: // [16] -- dtr_n RO bit // [17] -- OUT1_N RO bit // [18] -- OUT2_N RO bit // [19] --
 * dma_tx_req_n RO bit // [20] -- dma_tx_single_n RO bit // [21] -- dma_rx_req_n RO bit // [22] --
 * dma_rx_single_n RO bit // [23] -- uart_lp_req_pclk RO bit // [24] -- baudout_n RO bit
 */
#define PBS_UNIT_UART1_CONF_STATUS_STATUS_MASK 0xFFFF0000
#define PBS_UNIT_UART1_CONF_STATUS_STATUS_SHIFT 16

/**** uart2_conf_status register ****/
/*
 * Conf: // [0] -- DSR_N RW bit // [1] -- DCD_N RW bit // [2] -- RI_N bit // [3] -- dma_tx_ack_n //
 * [4] - dma_rx_ack_n
 */
#define PBS_UNIT_UART2_CONF_STATUS_CONF_MASK 0x0000FFFF
#define PBS_UNIT_UART2_CONF_STATUS_CONF_SHIFT 0
/*
 * Status: // [16] -- dtr_n RO bit // [17] -- OUT1_N RO bit // [18] -- OUT2_N RO bit // [19] --
 * dma_tx_req_n RO bit // [20] -- dma_tx_single_n RO bit // [21] -- dma_rx_req_n RO bit // [22] --
 * dma_rx_single_n RO bit // [23] -- uart_lp_req_pclk RO bit // [24] -- baudout_n RO bit
 */
#define PBS_UNIT_UART2_CONF_STATUS_STATUS_MASK 0xFFFF0000
#define PBS_UNIT_UART2_CONF_STATUS_STATUS_SHIFT 16

/**** uart3_conf_status register ****/
/*
 * Conf: // [0] -- DSR_N RW bit // [1] -- DCD_N RW bit // [2] -- RI_N bit // [3] -- dma_tx_ack_n //
 * [4] - dma_rx_ack_n
 */
#define PBS_UNIT_UART3_CONF_STATUS_CONF_MASK 0x0000FFFF
#define PBS_UNIT_UART3_CONF_STATUS_CONF_SHIFT 0
/*
 * Status: // [16] -- dtr_n RO bit // [17] -- OUT1_N RO bit // [18] -- OUT2_N RO bit // [19] --
 * dma_tx_req_n RO bit // [20] -- dma_tx_single_n RO bit // [21] -- dma_rx_req_n RO bit // [22] --
 * dma_rx_single_n RO bit // [23] -- uart_lp_req_pclk RO bit // [24] -- baudout_n RO bit
 */
#define PBS_UNIT_UART3_CONF_STATUS_STATUS_MASK 0xFFFF0000
#define PBS_UNIT_UART3_CONF_STATUS_STATUS_SHIFT 16

/**** gpio0_conf_status register ****/
/*
 * Cntl:
 * //  [7:0] nGPAFEN;              // from regfile
 * //  [15:8] GPAFOUT;             // from regfile
 */
#define PBS_UNIT_GPIO0_CONF_STATUS_CONF_MASK 0x0000FFFF
#define PBS_UNIT_GPIO0_CONF_STATUS_CONF_SHIFT 0
/*
 * Status:
 * //  [24:16] GPAFIN;             // to regfile
 */
#define PBS_UNIT_GPIO0_CONF_STATUS_STATUS_MASK 0xFFFF0000
#define PBS_UNIT_GPIO0_CONF_STATUS_STATUS_SHIFT 16

/**** gpio1_conf_status register ****/
/*
 * Cntl:
 * //  [7:0] nGPAFEN;              // from regfile
 * //  [15:8] GPAFOUT;             // from regfile
 */
#define PBS_UNIT_GPIO1_CONF_STATUS_CONF_MASK 0x0000FFFF
#define PBS_UNIT_GPIO1_CONF_STATUS_CONF_SHIFT 0
/*
 * Status:
 * //  [24:16] GPAFIN;             // to regfile
 */
#define PBS_UNIT_GPIO1_CONF_STATUS_STATUS_MASK 0xFFFF0000
#define PBS_UNIT_GPIO1_CONF_STATUS_STATUS_SHIFT 16

/**** gpio2_conf_status register ****/
/*
 * Cntl:
 * //  [7:0] nGPAFEN;              // from regfile
 * //  [15:8] GPAFOUT;             // from regfile
 */
#define PBS_UNIT_GPIO2_CONF_STATUS_CONF_MASK 0x0000FFFF
#define PBS_UNIT_GPIO2_CONF_STATUS_CONF_SHIFT 0
/*
 * Status:
 * //  [24:16] GPAFIN;             // to regfile
 */
#define PBS_UNIT_GPIO2_CONF_STATUS_STATUS_MASK 0xFFFF0000
#define PBS_UNIT_GPIO2_CONF_STATUS_STATUS_SHIFT 16

/**** gpio3_conf_status register ****/
/*
 * Cntl:
 * //  [7:0] nGPAFEN;              // from regfile
 * //  [15:8] GPAFOUT;             // from regfile
 */
#define PBS_UNIT_GPIO3_CONF_STATUS_CONF_MASK 0x0000FFFF
#define PBS_UNIT_GPIO3_CONF_STATUS_CONF_SHIFT 0
/*
 * Status:
 * //  [24:16] GPAFIN;             // to regfile
 */
#define PBS_UNIT_GPIO3_CONF_STATUS_STATUS_MASK 0xFFFF0000
#define PBS_UNIT_GPIO3_CONF_STATUS_STATUS_SHIFT 16

/**** gpio4_conf_status register ****/
/*
 * Cntl:
 * //  [7:0] nGPAFEN;              // from regfile
 * //  [15:8] GPAFOUT;             // from regfile
 */
#define PBS_UNIT_GPIO4_CONF_STATUS_CONF_MASK 0x0000FFFF
#define PBS_UNIT_GPIO4_CONF_STATUS_CONF_SHIFT 0
/*
 * Status:
 * //  [24:16] GPAFIN;             // to regfile
 */
#define PBS_UNIT_GPIO4_CONF_STATUS_STATUS_MASK 0xFFFF0000
#define PBS_UNIT_GPIO4_CONF_STATUS_STATUS_SHIFT 16

/**** i2c_gen_conf_status register ****/
/*
 * cntl
 * // [0] -- dma_tx_ack
 * // [1] -- dma_rx_ack
 */
#define PBS_UNIT_I2C_GEN_CONF_STATUS_CONF_MASK 0x0000FFFF
#define PBS_UNIT_I2C_GEN_CONF_STATUS_CONF_SHIFT 0
/*
 * Status
 *
 * // [16] - dma_tx_req RO bit
 * // [17] -- dma_tx_single RO bit
 * // [18] -- dma_rx_req RO bit
 * // [19] -- dma_rx_single RO bit
 */
#define PBS_UNIT_I2C_GEN_CONF_STATUS_STATUS_MASK 0xFFFF0000
#define PBS_UNIT_I2C_GEN_CONF_STATUS_STATUS_SHIFT 16

/**** watch_dog_reset_out register ****/
/*
 * [0] If set to 1'b1, WD0 cannot generate reset_out_n
 * [1] If set to 1'b1, WD1 cannot generate reset_out_n
 * [2] If set to 1'b1, WD2 cannot generate reset_out_n
 * [3] If set to 1'b1, WD3 cannot generate reset_out_n
 * [4] If set to 1'b1, WD4 cannot generate reset_out_n
 * [5] If set to 1'b1, WD5 cannot generate reset_out_n
 * [6] If set to 1'b1, WD6 cannot generate reset_out_n
 * [7] If set to 1'b1, WD7 cannot generate reset_out_n
 */
#define PBS_UNIT_WATCH_DOG_RESET_OUT_DISABLE_MASK 0x000000FF
#define PBS_UNIT_WATCH_DOG_RESET_OUT_DISABLE_SHIFT 0

/**** otp_cntl register ****/
/* Reserved */
#define PBS_UNIT_OTP_CNTL_IGNORE_OTPW    (1 << 0)
/* Not in use.Comes from bond. */
#define PBS_UNIT_OTP_CNTL_IGNORE_PRELOAD (1 << 1)
/* Margin read from the fuse box */
#define PBS_UNIT_OTP_CNTL_OTPW_MARGIN_READ (1 << 2)
/* Indicates when OTP is  busy.  */
#define PBS_UNIT_OTP_CNTL_OTP_BUSY       (1 << 3)
/* Indicates when OTP 4K is  busy.  */
#define PBS_UNIT_OTP_CNTL_OTP_4K_BUSY    (1 << 4)
/* Configures OTP TRCS port */
#define PBS_UNIT_OTP_CNTL_CFG_OTP_TRCS   (1 << 5)
/* Configures OTP AT  port */
#define PBS_UNIT_OTP_CNTL_CFG_OTP_AT_MASK 0x000000C0
#define PBS_UNIT_OTP_CNTL_CFG_OTP_AT_SHIFT 6

/**** otp_cfg_0 register ****/
/* Cfg  to OTP cntl. */
#define PBS_UNIT_OTP_CFG_0_CFG_OTPW_PWRDN_CNT_MASK 0x0000FFFF
#define PBS_UNIT_OTP_CFG_0_CFG_OTPW_PWRDN_CNT_SHIFT 0
/* Cfg  to OTP cntl. */
#define PBS_UNIT_OTP_CFG_0_CFG_OTPW_READ_CNT_MASK 0xFFFF0000
#define PBS_UNIT_OTP_CFG_0_CFG_OTPW_READ_CNT_SHIFT 16

/**** otp_cfg_1 register ****/
/* Cfg  to OTP cntl.  */
#define PBS_UNIT_OTP_CFG_1_CFG_OTPW_PGM_CNT_MASK 0x0000FFFF
#define PBS_UNIT_OTP_CFG_1_CFG_OTPW_PGM_CNT_SHIFT 0
/* Cfg  to OTP cntl. */
#define PBS_UNIT_OTP_CFG_1_CFG_OTPW_PREP_CNT_MASK 0xFFFF0000
#define PBS_UNIT_OTP_CFG_1_CFG_OTPW_PREP_CNT_SHIFT 16

/**** otp_cfg_3 register ****/
/* Cfg  to OTP cntl. */
#define PBS_UNIT_OTP_CFG_3_CFG_OTPW_PS18_CNT_MASK 0x0000FFFF
#define PBS_UNIT_OTP_CFG_3_CFG_OTPW_PS18_CNT_SHIFT 0
/* Cfg  to OTP cntl. */
#define PBS_UNIT_OTP_CFG_3_CFG_OTPW_PWRUP_CNT_MASK 0xFFFF0000
#define PBS_UNIT_OTP_CFG_3_CFG_OTPW_PWRUP_CNT_SHIFT 16

/**** serdes_mux_multi_0 register ****/
/* SerDes one hot mux control.  For details see datasheet.  */
#define PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_8_MASK 0x00000007
#define PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_8_SHIFT 0
/* Reserved */
#define PBS_UNIT_SERDES_MUX_MULTI_0_RSRVD_3 (1 << 3)
/* SerDes one hot mux control.  For details see datasheet.  */
#define PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_9_MASK 0x00000070
#define PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_9_SHIFT 4
/* Reserved */
#define PBS_UNIT_SERDES_MUX_MULTI_0_RSRVD_7 (1 << 7)
/* SerDes one hot mux control.  For details see datasheet.  */
#define PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_10_MASK 0x00000700
#define PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_10_SHIFT 8
/* Reserved */
#define PBS_UNIT_SERDES_MUX_MULTI_0_RSRVD_11 (1 << 11)
/* SerDes one hot mux control.  For details see datasheet.  */
#define PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_11_MASK 0x00007000
#define PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_11_SHIFT 12
/* Reserved */
#define PBS_UNIT_SERDES_MUX_MULTI_0_RSRVD_15 (1 << 15)
/* SerDes one hot mux control.  For details see datasheet.  */
#define PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_12_MASK 0x00030000
#define PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_12_SHIFT 16
/* SerDes one hot mux control.  For details see datasheet.  */
#define PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_13_MASK 0x000C0000
#define PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_13_SHIFT 18
/* SerDes one hot mux control.  For details see datasheet.  */
#define PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_14_MASK 0x00300000
#define PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_14_SHIFT 20
/* SerDes one hot mux control.  For details see datasheet.  */
#define PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_15_MASK 0x00C00000
#define PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_15_SHIFT 22
/* Reserved */
#define PBS_UNIT_SERDES_MUX_MULTI_0_RSRVD_MASK 0xFF000000
#define PBS_UNIT_SERDES_MUX_MULTI_0_RSRVD_SHIFT 24

/*
 * 2'b01 - select sata_b[0]
 * 2'b10 - select eth_a[0]
 */
#define PBS_UNIT_SERDES_MUX_MULTI_0_ALPINE_V2_SELECT_OH_SERDES_8_MASK 0x00000003
#define PBS_UNIT_SERDES_MUX_MULTI_0_ALPINE_V2_SELECT_OH_SERDES_8_SHIFT 0
/*
 * 3'b001 - select sata_b[1]
 * 3'b010 - select eth_b[0]
 * 3'b100 - select eth_a[1]
 */
#define PBS_UNIT_SERDES_MUX_MULTI_0_ALPINE_V2_SELECT_OH_SERDES_9_MASK 0x00000070
#define PBS_UNIT_SERDES_MUX_MULTI_0_ALPINE_V2_SELECT_OH_SERDES_9_SHIFT 4
/*
 * 3'b001 - select sata_b[2]
 * 3'b010 - select eth_c[0]
 * 3'b100 - select eth_a[2]
 */
#define PBS_UNIT_SERDES_MUX_MULTI_0_ALPINE_V2_SELECT_OH_SERDES_10_MASK 0x00000700
#define PBS_UNIT_SERDES_MUX_MULTI_0_ALPINE_V2_SELECT_OH_SERDES_10_SHIFT 8
/*
 * 3'b001 - select sata_b[3]
 * 3'b010 - select eth_d[0]
 * 3'b100 - select eth_a[3]
 */
#define PBS_UNIT_SERDES_MUX_MULTI_0_ALPINE_V2_SELECT_OH_SERDES_11_MASK 0x00007000
#define PBS_UNIT_SERDES_MUX_MULTI_0_ALPINE_V2_SELECT_OH_SERDES_11_SHIFT 12
/*
 * 2'b01 - select eth_a[0]
 * 2'b10 - select sata_a[0]
 */
#define PBS_UNIT_SERDES_MUX_MULTI_0_ALPINE_V2_SELECT_OH_SERDES_12_MASK 0x00030000
#define PBS_UNIT_SERDES_MUX_MULTI_0_ALPINE_V2_SELECT_OH_SERDES_12_SHIFT 16
/*
 * 3'b001 - select eth_b[0]
 * 3'b010 - select eth_c[1]
 * 3'b100 - select sata_a[1]
 */
#define PBS_UNIT_SERDES_MUX_MULTI_0_ALPINE_V2_SELECT_OH_SERDES_13_MASK 0x00700000
#define PBS_UNIT_SERDES_MUX_MULTI_0_ALPINE_V2_SELECT_OH_SERDES_13_SHIFT 20
/*
 * 3'b001 - select eth_a[0]
 * 3'b010 - select eth_c[2]
 * 3'b100 - select sata_a[2]
 */
#define PBS_UNIT_SERDES_MUX_MULTI_0_ALPINE_V2_SELECT_OH_SERDES_14_MASK 0x07000000
#define PBS_UNIT_SERDES_MUX_MULTI_0_ALPINE_V2_SELECT_OH_SERDES_14_SHIFT 24
/*
 * 3'b001 - select eth_d[0]
 * 3'b010 - select eth_c[3]
 * 3'b100 - select sata_a[3]
 */
#define PBS_UNIT_SERDES_MUX_MULTI_0_ALPINE_V2_SELECT_OH_SERDES_15_MASK 0x70000000
#define PBS_UNIT_SERDES_MUX_MULTI_0_ALPINE_V2_SELECT_OH_SERDES_15_SHIFT 28

/**** serdes_mux_multi_1 register ****/
/* SerDes one hot mux control.  For details see datasheet.  */
#define PBS_UNIT_SERDES_MUX_MULTI_1_SELECT_OH_ETH_A_0_MASK 0x00000003
#define PBS_UNIT_SERDES_MUX_MULTI_1_SELECT_OH_ETH_A_0_SHIFT 0
/* Reserved */
#define PBS_UNIT_SERDES_MUX_MULTI_1_RSRVD_3_2_MASK 0x0000000C
#define PBS_UNIT_SERDES_MUX_MULTI_1_RSRVD_3_2_SHIFT 2
/* SerDes one hot mux control.  For details see datasheet.  */
#define PBS_UNIT_SERDES_MUX_MULTI_1_SELECT_OH_ETH_B_0_MASK 0x00000070
#define PBS_UNIT_SERDES_MUX_MULTI_1_SELECT_OH_ETH_B_0_SHIFT 4
/* Reserved */
#define PBS_UNIT_SERDES_MUX_MULTI_1_RSRVD_7 (1 << 7)
/* SerDes one hot mux control.  For details see datasheet.  */
#define PBS_UNIT_SERDES_MUX_MULTI_1_SELECT_OH_ETH_C_0_MASK 0x00000300
#define PBS_UNIT_SERDES_MUX_MULTI_1_SELECT_OH_ETH_C_0_SHIFT 8
/* Reserved */
#define PBS_UNIT_SERDES_MUX_MULTI_1_RSRVD_11_10_MASK 0x00000C00
#define PBS_UNIT_SERDES_MUX_MULTI_1_RSRVD_11_10_SHIFT 10
/* SerDes one hot mux control.  For details see datasheet.  */
#define PBS_UNIT_SERDES_MUX_MULTI_1_SELECT_OH_ETH_D_0_MASK 0x00007000
#define PBS_UNIT_SERDES_MUX_MULTI_1_SELECT_OH_ETH_D_0_SHIFT 12
/* Reserved */
#define PBS_UNIT_SERDES_MUX_MULTI_1_RSRVD_MASK 0xFFFF8000
#define PBS_UNIT_SERDES_MUX_MULTI_1_RSRVD_SHIFT 15

/**** pbs_ulpi_mux_conf register ****/
/*
 * Value 0 - Select dedicated pins for the USB-1 inputs.
 * Value 1-    Select PBS mux pins for the USB-1 inputs.
 * [0] ULPI_B_CLK
 * [1] ULPI_B_DIR
 * [2] ULPI_B_NXT
 * [10:3] ULPI_B_DATA[7:0]
 */
#define PBS_UNIT_PBS_ULPI_MUX_CONF_SEL_UPLI_IN_PBSMUX_MASK 0x000007FF
#define PBS_UNIT_PBS_ULPI_MUX_CONF_SEL_UPLI_IN_PBSMUX_SHIFT 0
/*
 * [4] - Force to zero
 * [3] == 1 - Force register selection
 * [2 : 0] -Binary selection of the input in bypass mode
 */
#define PBS_UNIT_PBS_ULPI_MUX_CONF_REG_MDIO_BYPASS_SEL_MASK 0x0001F000
#define PBS_UNIT_PBS_ULPI_MUX_CONF_REG_MDIO_BYPASS_SEL_SHIFT 12
/*
 * [0] Sets the clk_ulpi OE for USB0, 1'b0 set to input, 1'b1 set to output.
 * [1] Sets the clk_ulpi OE for USB01, 1'b0 set to input, 1'b1 set to output.
 */
#define PBS_UNIT_PBS_ULPI_MUX_CONF_RSRVD_MASK 0xFFFE0000
#define PBS_UNIT_PBS_ULPI_MUX_CONF_RSRVD_SHIFT 17

/**** wr_once_dbg_dis_ovrd_reg register ****/
/* This register can be written only once. Use in the secure boot process. */
#define PBS_UNIT_WR_ONCE_DBG_DIS_OVRD_REG_WR_ONCE_DBG_DIS_OVRD (1 << 0)

#define PBS_UNIT_WR_ONCE_DBG_DIS_OVRD_REG_RSRVD_MASK 0xFFFFFFFE
#define PBS_UNIT_WR_ONCE_DBG_DIS_OVRD_REG_RSRVD_SHIFT 1

/**** gpio5_conf_status register ****/
/*
 * Cntl: // [7:0] nGPAFEN; // from regfile // [15:8] GPAFOUT; // from regfile
 */
#define PBS_UNIT_GPIO5_CONF_STATUS_CONF_MASK 0x0000FFFF
#define PBS_UNIT_GPIO5_CONF_STATUS_CONF_SHIFT 0
/* Status: //  [24:16] GPAFIN;             // to regfile */
#define PBS_UNIT_GPIO5_CONF_STATUS_STATUS_MASK 0xFFFF0000
#define PBS_UNIT_GPIO5_CONF_STATUS_STATUS_SHIFT 16

/**** pbs_sb2nb_cfg_dram_remap register ****/
#define PBS_UNIT_SB2NB_REMAP_BASE_ADDR_SHIFT		5
#define PBS_UNIT_SB2NB_REMAP_BASE_ADDR_MASK		0x0000FFE0
#define PBS_UNIT_SB2NB_REMAP_TRANSL_BASE_ADDR_SHIFT	21
#define PBS_UNIT_SB2NB_REMAP_TRANSL_BASE_ADDR_MASK	0xFFE00000

/* For remapping are used bits [39 - 29] of DRAM 40bit Physical address */
#define PBS_UNIT_DRAM_SRC_REMAP_BASE_ADDR_SHIFT	29
#define PBS_UNIT_DRAM_DST_REMAP_BASE_ADDR_SHIFT	29
#define PBS_UNIT_DRAM_REMAP_BASE_ADDR_MASK	0xFFE0000000ULL


/**** serdes_mux_eth register ****/
/*
 * 2'b01 - eth_a[0] from serdes_8
 * 2'b10 - eth_a[0] from serdes_14
 */
#define PBS_UNIT_SERDES_MUX_ETH_ALPINE_V2_SELECT_OH_ETH_A_0_MASK 0x00000003
#define PBS_UNIT_SERDES_MUX_ETH_ALPINE_V2_SELECT_OH_ETH_A_0_SHIFT 0
/*
 * 2'b01 - eth_b[0] from serdes_9
 * 2'b10 - eth_b[0] from serdes_13
 */
#define PBS_UNIT_SERDES_MUX_ETH_ALPINE_V2_SELECT_OH_ETH_B_0_MASK 0x00000030
#define PBS_UNIT_SERDES_MUX_ETH_ALPINE_V2_SELECT_OH_ETH_B_0_SHIFT 4
/*
 * 2'b01 - eth_c[0] from serdes_10
 * 2'b10 - eth_c[0] from serdes_12
 */
#define PBS_UNIT_SERDES_MUX_ETH_ALPINE_V2_SELECT_OH_ETH_C_0_MASK 0x00000300
#define PBS_UNIT_SERDES_MUX_ETH_ALPINE_V2_SELECT_OH_ETH_C_0_SHIFT 8
/*
 * 2'b01 - eth_d[0] from serdes_11
 * 2'b10 - eth_d[0] from serdes_15
 */
#define PBS_UNIT_SERDES_MUX_ETH_ALPINE_V2_SELECT_OH_ETH_D_0_MASK 0x00003000
#define PBS_UNIT_SERDES_MUX_ETH_ALPINE_V2_SELECT_OH_ETH_D_0_SHIFT 12
/* which lane's is master clk */
#define PBS_UNIT_SERDES_MUX_ETH_ALPINE_V2_SELECT_OH_ETH_A_ICK_MASTER_MASK 0x00030000
#define PBS_UNIT_SERDES_MUX_ETH_ALPINE_V2_SELECT_OH_ETH_A_ICK_MASTER_SHIFT 16
/* which lane's is master clk */
#define PBS_UNIT_SERDES_MUX_ETH_ALPINE_V2_SELECT_OH_ETH_C_ICK_MASTER_MASK 0x00300000
#define PBS_UNIT_SERDES_MUX_ETH_ALPINE_V2_SELECT_OH_ETH_C_ICK_MASTER_SHIFT 20
/* enable xlaui on eth a */
#define PBS_UNIT_SERDES_MUX_ETH_ALPINE_V2_SELECT_OH_ETH_A_XLAUI_ENABLE (1 << 24)
/* enable xlaui on eth c */
#define PBS_UNIT_SERDES_MUX_ETH_ALPINE_V2_SELECT_OH_ETH_C_XLAUI_ENABLE (1 << 28)

/**** serdes_mux_pcie register ****/
/*
 * 2'b01 - select pcie_b[0] from serdes 2
 * 2'b10 - select pcie_b[0] from serdes 4
 */
#define PBS_UNIT_SERDES_MUX_PCIE_ALPINE_V2_SELECT_OH_PCIE_B_0_MASK 0x00000003
#define PBS_UNIT_SERDES_MUX_PCIE_ALPINE_V2_SELECT_OH_PCIE_B_0_SHIFT 0
/*
 * 2'b01 - select pcie_b[1] from serdes 3
 * 2'b10 - select pcie_b[1] from serdes 5
 */
#define PBS_UNIT_SERDES_MUX_PCIE_ALPINE_V2_SELECT_OH_PCIE_B_1_MASK 0x00000030
#define PBS_UNIT_SERDES_MUX_PCIE_ALPINE_V2_SELECT_OH_PCIE_B_1_SHIFT 4
/*
 * 2'b01 - select pcie_d[0] from serdes 10
 * 2'b10 - select pcie_d[0] from serdes 12
 */
#define PBS_UNIT_SERDES_MUX_PCIE_ALPINE_V2_SELECT_OH_PCIE_D_0_MASK 0x00000300
#define PBS_UNIT_SERDES_MUX_PCIE_ALPINE_V2_SELECT_OH_PCIE_D_0_SHIFT 8
/*
 * 2'b01 - select pcie_d[1] from serdes 11
 * 2'b10 - select pcie_d[1] from serdes 13
 */
#define PBS_UNIT_SERDES_MUX_PCIE_ALPINE_V2_SELECT_OH_PCIE_D_1_MASK 0x00003000
#define PBS_UNIT_SERDES_MUX_PCIE_ALPINE_V2_SELECT_OH_PCIE_D_1_SHIFT 12

/**** serdes_mux_sata register ****/
/*
 * 2'b01 - select sata_a from serdes group 1
 * 2'b10 - select sata_a from serdes group 3
 */
#define PBS_UNIT_SERDES_MUX_SATA_SELECT_OH_SATA_A_MASK 0x00000003
#define PBS_UNIT_SERDES_MUX_SATA_SELECT_OH_SATA_A_SHIFT 0
/* Reserved */
#define PBS_UNIT_SERDES_MUX_SATA_RESERVED_3_2_MASK 0x0000000C
#define PBS_UNIT_SERDES_MUX_SATA_RESERVED_3_2_SHIFT 2
/* Reserved */
#define PBS_UNIT_SERDES_MUX_SATA_RESERVED_MASK 0xFFFFFFF0
#define PBS_UNIT_SERDES_MUX_SATA_RESERVED_SHIFT 4

/**** i2c_scl_reroute register ****/
/*
 * If 1, take I2C SCL locally from inside PBS. Otherwise always take incoming SCL from pad.
 */
#define PBS_UNIT_I2C_SCL_REROUTE_SELECT_REROUTE (1 << 0)
/* Reserved */
#define PBS_UNIT_I2C_SCL_REROUTE_RESERVED_MASK 0xFFFFFFFE
#define PBS_UNIT_I2C_SCL_REROUTE_RESERVED_SHIFT 1

/**** cfg_axi_conf_5 register ****/
/* Soft reset for 128to256 converter */
#define PBS_UNIT_CFG_AXI_CONF_5_CFG_AXI_NIC_CLEAR (1 << 0)
/* Soft reset for CPU 128to256 converter in fast IO */
#define PBS_UNIT_CFG_AXI_CONF_5_CFG_CPU_AXI_NIC_CLEAR (1 << 1)
/* If set, will cause the 128to256 converter to always recalculate parity on RUSER bits */
#define PBS_UNIT_CFG_AXI_CONF_5_CFG_AXI_NIC_PAR_OVRD (1 << 2)
/* Reserved */
#define PBS_UNIT_CFG_AXI_CONF_5_RESERVED_MASK 0xFFFFFFF8
#define PBS_UNIT_CFG_AXI_CONF_5_RESERVED_SHIFT 3

/**** gpio6_conf_status register ****/
/*
 * Cntl:
 * //  [7:0] nGPAFEN;              // from regfile
 * //  [15:8] GPAFOUT;             // from regfile
 */
#define PBS_UNIT_GPIO6_CONF_STATUS_CONF_MASK 0x0000FFFF
#define PBS_UNIT_GPIO6_CONF_STATUS_CONF_SHIFT 0
/*
 * Status:
 * //  [24:16] GPAFIN;             // to regfile
 */
#define PBS_UNIT_GPIO6_CONF_STATUS_STATUS_MASK 0xFFFF0000
#define PBS_UNIT_GPIO6_CONF_STATUS_STATUS_SHIFT 16

/**** gpio7_conf_status register ****/
/*
 * Cntl:
 * //  [7:0] nGPAFEN;              // from regfile
 * //  [15:8] GPAFOUT;             // from regfile
 */
#define PBS_UNIT_GPIO7_CONF_STATUS_CONF_MASK 0x0000FFFF
#define PBS_UNIT_GPIO7_CONF_STATUS_CONF_SHIFT 0
/*
 * Status:
 * //  [24:16] GPAFIN;             // to regfile
 */
#define PBS_UNIT_GPIO7_CONF_STATUS_STATUS_MASK 0xFFFF0000
#define PBS_UNIT_GPIO7_CONF_STATUS_STATUS_SHIFT 16

/**** enable register ****/
/* Static enable (qualifies approval to enter the dynamic mode at all) */
#define PBS_QSPI_ENABLE_ENABLE           (1 << 0)

/**** clear_trig register ****/
/* Trigger brute force break of the QPI flows. */
#define PBS_QSPI_CLEAR_TRIG_CLEAR_FSM    (1 << 0)

/**** start_trig register ****/
/* Trigger start of QPI config */
#define PBS_QSPI_START_TRIG_START        (1 << 0)

/**** stop_trig register ****/
/* Trigger end of QPI config  */
#define PBS_QSPI_STOP_TRIG_STOP          (1 << 0)

/**** rst_trig register ****/
/* Trigger  reset   */
#define PBS_QSPI_RST_TRIG_RST            (1 << 0)

/**** rsten_trig register ****/
/* Trigger  reset */
#define PBS_QSPI_RSTEN_TRIG_RSTEN        (1 << 0)

/**** wen_trig register ****/
/* Trigger write enable command */
#define PBS_QSPI_WEN_TRIG_WEN            (1 << 0)

/**** a4enter_trig register ****/
/* Enter 4bytes address mode - not assumed locally, only for the device… */
#define PBS_QSPI_A4ENTER_TRIG_A4ENTER    (1 << 0)

/**** a4exit_trig register ****/
/* Exit 4bytes address mode - not assumed locally, only for the device… */
#define PBS_QSPI_A4EXIT_TRIG_A4EXIT      (1 << 0)

/**** config register ****/
/*
 * How many dummy cycles post Address...(excluding Enh_Mode point that is covered by the single
 * dummy post address.
 */
#define PBS_QSPI_CONFIG_DUMMY_CNT_MASK   0x00000007
#define PBS_QSPI_CONFIG_DUMMY_CNT_SHIFT  0
/* How many of the dummy cycles should the Tx still drive the bus... */
#define PBS_QSPI_CONFIG_DUMMY_DRV_MASK   0x00000070
#define PBS_QSPI_CONFIG_DUMMY_DRV_SHIFT  4
/*
 * Special care as on-board RESET# adn WP#/HOLD# may be driven now by our SoC rather than other
 * means, due to sharing with QPI
 */
#define PBS_QSPI_CONFIG_TXD_DRV_EN_MASK  0x00000300
#define PBS_QSPI_CONFIG_TXD_DRV_EN_SHIFT 8
/* txd value */
#define PBS_QSPI_CONFIG_TXD_VALUE_MASK   0x00003000
#define PBS_QSPI_CONFIG_TXD_VALUE_SHIFT  12
/* Drive delay */
#define PBS_QSPI_CONFIG_DELAY_DRV_MASK   0x00030000
#define PBS_QSPI_CONFIG_DELAY_DRV_SHIFT  16

/**** opcodes register ****/
/* Reserved fields */
#define PBS_QSPI_OPCODES_WRITE_EN_OP_MASK 0x000000FF
#define PBS_QSPI_OPCODES_WRITE_EN_OP_SHIFT 0
/* Reserved */
#define PBS_QSPI_OPCODES_ADDR4_ENTER_OP_MASK 0x0000FF00
#define PBS_QSPI_OPCODES_ADDR4_ENTER_OP_SHIFT 8
/* Reserved fields */
#define PBS_QSPI_OPCODES_ADDR4_EXIT_OP_MASK 0x00FF0000
#define PBS_QSPI_OPCODES_ADDR4_EXIT_OP_SHIFT 16
/* Reserved */
#define PBS_QSPI_OPCODES_READ_OP_MASK    0xFF000000
#define PBS_QSPI_OPCODES_READ_OP_SHIFT   24

/**** opcodes2 register ****/
/* Reserved fields */
#define PBS_QSPI_OPCODES2_ENTER_OP_MASK  0x000000FF
#define PBS_QSPI_OPCODES2_ENTER_OP_SHIFT 0
/* Reserved */
#define PBS_QSPI_OPCODES2_EXIT_OP_MASK   0x0000FF00
#define PBS_QSPI_OPCODES2_EXIT_OP_SHIFT  8
/* Reserved fields */
#define PBS_QSPI_OPCODES2_RST_EN_OP_MASK 0x00FF0000
#define PBS_QSPI_OPCODES2_RST_EN_OP_SHIFT 16
/* Reserved */
#define PBS_QSPI_OPCODES2_RST_CMD_OP_MASK 0xFF000000
#define PBS_QSPI_OPCODES2_RST_CMD_OP_SHIFT 24

/**** slave_enable register ****/
/* Static enable */
#define PBS_QSPI_SLAVE_ENABLE_ENABLE     (1 << 0)

/**** spim_preload_clear_trig register ****/
/* Trigger brute force break of the SPIM preload flows. */
#define PBS_SPIM_PRELOAD_SPIM_PRELOAD_CLEAR_TRIG_SPIM_CLEAR_FSM (1 << 0)

/**** eth_capability register ****/
/*
 * Limit Ethernet speed
 * 00 - Up to 100G
 * 01 - Up to 50G
 * 10 - Up to 25G
 * 11 - Up to 10G
 * Other values are reserved
 */
#define PBS_CHIP_CAPABILITIES_ETH_CAPABILITY_ETH_SPEED_LIMIT_MASK 0x00000003
#define PBS_CHIP_CAPABILITIES_ETH_CAPABILITY_ETH_SPEED_LIMIT_SHIFT 0
/* Reserved */
#define PBS_CHIP_CAPABILITIES_ETH_CAPABILITY_RESERVED_7_2_MASK 0x000000FC
#define PBS_CHIP_CAPABILITIES_ETH_CAPABILITY_RESERVED_7_2_SHIFT 2
/*
 * Limit RDMA functionality
 * 0 - RDMA exist
 * 1 - No RDMA
 */
#define PBS_CHIP_CAPABILITIES_ETH_CAPABILITY_RDMA_LIMIT (1 << 8)
/* Reserved */
#define PBS_CHIP_CAPABILITIES_ETH_CAPABILITY_RESERVED_31_9_MASK 0xFFFFFE00
#define PBS_CHIP_CAPABILITIES_ETH_CAPABILITY_RESERVED_31_9_SHIFT 9

/**** pcie_capability register ****/
/*
 * Limit PCIe width
 * 00 - Up to x16
 * 01 - Up to x8
 * 10 - Up to x4
 */
#define PBS_CHIP_CAPABILITIES_PCIE_CAPABILITY_PCIE_WIDTH_LIMIT_MASK 0x00000003
#define PBS_CHIP_CAPABILITIES_PCIE_CAPABILITY_PCIE_WIDTH_LIMIT_SHIFT 0
/* Reserved */
#define PBS_CHIP_CAPABILITIES_PCIE_CAPABILITY_RESERVED_31_20_MASK 0xFFFFFFFC
#define PBS_CHIP_CAPABILITIES_PCIE_CAPABILITY_RESERVED_31_20_SHIFT 2

/**** cpu_capability0 register ****/
/*
 * Bit per cluster determines if the associate cluster is disabled
 * When Cluster_disable [N] = 1 Cluster N is disabled.
 * If cluster 0 is disabled then chip boot from Cluster 1
 * If cluster 0 and 1 are disabled the chip boot from Cluster 2
 * If cluster 0, 1 and 2 are disabled the chip boot from Cluster 3
 */
#define PBS_CHIP_CAPABILITIES_CPU_CAPABILITY0_CLUSTER_EXIST_MASK 0x0000000F
#define PBS_CHIP_CAPABILITIES_CPU_CAPABILITY0_CLUSTER_EXIST_SHIFT 0
/* Reserved */
#define PBS_CHIP_CAPABILITIES_CPU_CAPABILITY0_RESERVED_31_4_MASK 0xFFFFFFF0
#define PBS_CHIP_CAPABILITIES_CPU_CAPABILITY0_RESERVED_31_4_SHIFT 4

/**** cpu_capability1 register ****/
/* Reserved */
#define PBS_CHIP_CAPABILITIES_CPU_CAPABILITY1_RESERVED_3_0_MASK 0x0000000F
#define PBS_CHIP_CAPABILITIES_CPU_CAPABILITY1_RESERVED_3_0_SHIFT 0
/*
 * Determine/Limit number of clusters
 * 00 - Quad Clusters
 * 01 - Triple Clusters
 * 10 - Dual Clusters
 * 11 - single Clusters
 */
#define PBS_CHIP_CAPABILITIES_CPU_CAPABILITY1_NUM_OF_CLUSTERS_MASK 0x00000030
#define PBS_CHIP_CAPABILITIES_CPU_CAPABILITY1_NUM_OF_CLUSTERS_SHIFT 4
/*
 * Determine/Limit number of CPUs
 * 00 - Quad CPUs
 * 01 - Triple CPUs
 * 10 - Dual CPUs
 * 11 - single CPUs
 */
#define PBS_CHIP_CAPABILITIES_CPU_CAPABILITY1_NUM_OF_CPUS_MASK 0x000000C0
#define PBS_CHIP_CAPABILITIES_CPU_CAPABILITY1_NUM_OF_CPUS_SHIFT 6
/* Reserved */
#define PBS_CHIP_CAPABILITIES_CPU_CAPABILITY1_RESERVED_15_8_MASK 0x0000FF00
#define PBS_CHIP_CAPABILITIES_CPU_CAPABILITY1_RESERVED_15_8_SHIFT 8
/*
 * Limit CPU speed
 * 0000 - No limit
 * 0001 - Limit to 2.5Ghz
 * 0010 - Limit to 2.0Ghz
 * 0001 - Limit to 1.5Ghz
 * 0010 - Limit to 1.0Ghz
 * Other values are reserved.
 */
#define PBS_CHIP_CAPABILITIES_CPU_CAPABILITY1_CPU_SPEED_MASK 0x000F0000
#define PBS_CHIP_CAPABILITIES_CPU_CAPABILITY1_CPU_SPEED_SHIFT 16
/* Reserved */
#define PBS_CHIP_CAPABILITIES_CPU_CAPABILITY1_RESERVED_31_20_MASK 0xFFF00000
#define PBS_CHIP_CAPABILITIES_CPU_CAPABILITY1_RESERVED_31_20_SHIFT 20

/**** dram_capability register ****/
/*
 * Bit per disabled
 * When DRAM_disable[0] is set to 1, DRAM controller 0 is disabled
 * When DRAM_disable[1] is set to 1, DRAM controller 1 is disabled
 */
#define PBS_CHIP_CAPABILITIES_DRAM_CAPABILITY_DRAM_DISABLE_MASK 0x00000003
#define PBS_CHIP_CAPABILITIES_DRAM_CAPABILITY_DRAM_DISABLE_SHIFT 0
/* Reserved */
#define PBS_CHIP_CAPABILITIES_DRAM_CAPABILITY_RESERVED_15_2_MASK 0x0000FFFC
#define PBS_CHIP_CAPABILITIES_DRAM_CAPABILITY_RESERVED_15_2_SHIFT 2
/*
 * Limit DRAM speed
 * 0000 - Up to 2667MHz
 * 0001 - Up to 2400Mhz
 * 0010 - Up to 2133Mhz
 * 0001 - Up to 1866Mhz
 * Other values are reserved.
 */
#define PBS_CHIP_CAPABILITIES_DRAM_CAPABILITY_DRAM_SPEED_MASK 0x000F0000
#define PBS_CHIP_CAPABILITIES_DRAM_CAPABILITY_DRAM_SPEED_SHIFT 16
/* Reserved */
#define PBS_CHIP_CAPABILITIES_DRAM_CAPABILITY_RESERVED_31_20_MASK 0xFFF00000
#define PBS_CHIP_CAPABILITIES_DRAM_CAPABILITY_RESERVED_31_20_SHIFT 20

/**** dram_0_nb_bar_low register ****/
/* Window size = 2 ^ (15 + win_size). Zero value: disable the window. */
#define PBS_BAR_CONFIG_DRAM_0_NB_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_BAR_CONFIG_DRAM_0_NB_BAR_LOW_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_BAR_CONFIG_DRAM_0_NB_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_BAR_CONFIG_DRAM_0_NB_BAR_LOW_RSRVD_SHIFT 6
/* Reserved */
#define PBS_BAR_CONFIG_DRAM_0_NB_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_BAR_CONFIG_DRAM_0_NB_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** dram_1_nb_bar_low register ****/
/* Window size = 2 ^ (15 + win_size). Zero value: disable the window. */
#define PBS_BAR_CONFIG_DRAM_1_NB_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_BAR_CONFIG_DRAM_1_NB_BAR_LOW_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_BAR_CONFIG_DRAM_1_NB_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_BAR_CONFIG_DRAM_1_NB_BAR_LOW_RSRVD_SHIFT 6
/* Reserved */
#define PBS_BAR_CONFIG_DRAM_1_NB_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_BAR_CONFIG_DRAM_1_NB_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** dram_2_nb_bar_low register ****/
/* Window size = 2 ^ (15 + win_size). Zero value: disable the window. */
#define PBS_BAR_CONFIG_DRAM_2_NB_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_BAR_CONFIG_DRAM_2_NB_BAR_LOW_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_BAR_CONFIG_DRAM_2_NB_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_BAR_CONFIG_DRAM_2_NB_BAR_LOW_RSRVD_SHIFT 6
/* Reserved */
#define PBS_BAR_CONFIG_DRAM_2_NB_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_BAR_CONFIG_DRAM_2_NB_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** dram_3_nb_bar_low register ****/
/* Window size = 2 ^ (15 + win_size). Zero value: disable the window. */
#define PBS_BAR_CONFIG_DRAM_3_NB_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_BAR_CONFIG_DRAM_3_NB_BAR_LOW_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_BAR_CONFIG_DRAM_3_NB_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_BAR_CONFIG_DRAM_3_NB_BAR_LOW_RSRVD_SHIFT 6
/* Reserved */
#define PBS_BAR_CONFIG_DRAM_3_NB_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_BAR_CONFIG_DRAM_3_NB_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** msix_nb_bar_low register ****/
/* Window size = 2 ^ (15 + win_size). Zero value: disable the window. */
#define PBS_BAR_CONFIG_MSIX_NB_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_BAR_CONFIG_MSIX_NB_BAR_LOW_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_BAR_CONFIG_MSIX_NB_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_BAR_CONFIG_MSIX_NB_BAR_LOW_RSRVD_SHIFT 6
/* Reserved */
#define PBS_BAR_CONFIG_MSIX_NB_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_BAR_CONFIG_MSIX_NB_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** dram_0_sb_bar_low register ****/
/* Window size = 2 ^ (15 + win_size). Zero value: disable the window. */
#define PBS_BAR_CONFIG_DRAM_0_SB_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_BAR_CONFIG_DRAM_0_SB_BAR_LOW_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_BAR_CONFIG_DRAM_0_SB_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_BAR_CONFIG_DRAM_0_SB_BAR_LOW_RSRVD_SHIFT 6
/* Reserved */
#define PBS_BAR_CONFIG_DRAM_0_SB_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_BAR_CONFIG_DRAM_0_SB_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** dram_1_sb_bar_low register ****/
/* Window size = 2 ^ (15 + win_size). Zero value: disable the window. */
#define PBS_BAR_CONFIG_DRAM_1_SB_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_BAR_CONFIG_DRAM_1_SB_BAR_LOW_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_BAR_CONFIG_DRAM_1_SB_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_BAR_CONFIG_DRAM_1_SB_BAR_LOW_RSRVD_SHIFT 6
/* Reserved */
#define PBS_BAR_CONFIG_DRAM_1_SB_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_BAR_CONFIG_DRAM_1_SB_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** dram_2_sb_bar_low register ****/
/* Window size = 2 ^ (15 + win_size). Zero value: disable the window. */
#define PBS_BAR_CONFIG_DRAM_2_SB_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_BAR_CONFIG_DRAM_2_SB_BAR_LOW_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_BAR_CONFIG_DRAM_2_SB_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_BAR_CONFIG_DRAM_2_SB_BAR_LOW_RSRVD_SHIFT 6
/* Reserved */
#define PBS_BAR_CONFIG_DRAM_2_SB_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_BAR_CONFIG_DRAM_2_SB_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** dram_3_sb_bar_low register ****/
/* Window size = 2 ^ (15 + win_size). Zero value: disable the window. */
#define PBS_BAR_CONFIG_DRAM_3_SB_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_BAR_CONFIG_DRAM_3_SB_BAR_LOW_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_BAR_CONFIG_DRAM_3_SB_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_BAR_CONFIG_DRAM_3_SB_BAR_LOW_RSRVD_SHIFT 6
/* Reserved */
#define PBS_BAR_CONFIG_DRAM_3_SB_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_BAR_CONFIG_DRAM_3_SB_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** msix_sb_bar_low register ****/
/* Window size = 2 ^ (15 + win_size). Zero value: disable the window. */
#define PBS_BAR_CONFIG_MSIX_SB_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_BAR_CONFIG_MSIX_SB_BAR_LOW_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_BAR_CONFIG_MSIX_SB_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_BAR_CONFIG_MSIX_SB_BAR_LOW_RSRVD_SHIFT 6
/* Reserved */
#define PBS_BAR_CONFIG_MSIX_SB_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_BAR_CONFIG_MSIX_SB_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** pcie_mem0_bar_low register ****/
/* Window size = 2 ^ (15 + win_size). Zero value: disable the window. */
#define PBS_BAR_CONFIG_PCIE_MEM0_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_BAR_CONFIG_PCIE_MEM0_BAR_LOW_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_BAR_CONFIG_PCIE_MEM0_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_BAR_CONFIG_PCIE_MEM0_BAR_LOW_RSRVD_SHIFT 6
/* Reserved */
#define PBS_BAR_CONFIG_PCIE_MEM0_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_BAR_CONFIG_PCIE_MEM0_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** pcie_mem1_bar_low register ****/
/* Window size = 2 ^ (15 + win_size). Zero value: disable the window. */
#define PBS_BAR_CONFIG_PCIE_MEM1_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_BAR_CONFIG_PCIE_MEM1_BAR_LOW_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_BAR_CONFIG_PCIE_MEM1_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_BAR_CONFIG_PCIE_MEM1_BAR_LOW_RSRVD_SHIFT 6
/* Reserved */
#define PBS_BAR_CONFIG_PCIE_MEM1_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_BAR_CONFIG_PCIE_MEM1_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** pcie_mem2_bar_low register ****/
/* Window size = 2 ^ (15 + win_size). Zero value: disable the window. */
#define PBS_BAR_CONFIG_PCIE_MEM2_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_BAR_CONFIG_PCIE_MEM2_BAR_LOW_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_BAR_CONFIG_PCIE_MEM2_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_BAR_CONFIG_PCIE_MEM2_BAR_LOW_RSRVD_SHIFT 6
/* Reserved */
#define PBS_BAR_CONFIG_PCIE_MEM2_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_BAR_CONFIG_PCIE_MEM2_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** pcie_mem3_bar_low register ****/
/* Window size = 2 ^ (15 + win_size). Zero value: disable the window. */
#define PBS_BAR_CONFIG_PCIE_MEM3_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_BAR_CONFIG_PCIE_MEM3_BAR_LOW_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_BAR_CONFIG_PCIE_MEM3_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_BAR_CONFIG_PCIE_MEM3_BAR_LOW_RSRVD_SHIFT 6
/* Reserved */
#define PBS_BAR_CONFIG_PCIE_MEM3_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_BAR_CONFIG_PCIE_MEM3_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** pcie_mem4_bar_low register ****/
/* Window size = 2 ^ (15 + win_size). Zero value: disable the window. */
#define PBS_BAR_CONFIG_PCIE_MEM4_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_BAR_CONFIG_PCIE_MEM4_BAR_LOW_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_BAR_CONFIG_PCIE_MEM4_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_BAR_CONFIG_PCIE_MEM4_BAR_LOW_RSRVD_SHIFT 6
/* Reserved */
#define PBS_BAR_CONFIG_PCIE_MEM4_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_BAR_CONFIG_PCIE_MEM4_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** pcie_mem5_bar_low register ****/
/* Window size = 2 ^ (15 + win_size). Zero value: disable the window. */
#define PBS_BAR_CONFIG_PCIE_MEM5_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_BAR_CONFIG_PCIE_MEM5_BAR_LOW_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_BAR_CONFIG_PCIE_MEM5_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_BAR_CONFIG_PCIE_MEM5_BAR_LOW_RSRVD_SHIFT 6
/* Reserved */
#define PBS_BAR_CONFIG_PCIE_MEM5_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_BAR_CONFIG_PCIE_MEM5_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** pcie_mem6_bar_low register ****/
/* Window size = 2 ^ (15 + win_size). Zero value: disable the window. */
#define PBS_BAR_CONFIG_PCIE_MEM6_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_BAR_CONFIG_PCIE_MEM6_BAR_LOW_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_BAR_CONFIG_PCIE_MEM6_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_BAR_CONFIG_PCIE_MEM6_BAR_LOW_RSRVD_SHIFT 6
/* Reserved */
#define PBS_BAR_CONFIG_PCIE_MEM6_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_BAR_CONFIG_PCIE_MEM6_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** pcie_ext_ecam0_bar_low register ****/
/* Window size = 2 ^ (15 + win_size). Zero value: disable the window. */
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM0_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM0_BAR_LOW_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM0_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM0_BAR_LOW_RSRVD_SHIFT 6
/* Reserved */
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM0_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM0_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** pcie_ext_ecam1_bar_low register ****/
/* Window size = 2 ^ (15 + win_size). Zero value: disable the window. */
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM1_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM1_BAR_LOW_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM1_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM1_BAR_LOW_RSRVD_SHIFT 6
/* Reserved */
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM1_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM1_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** pcie_ext_ecam2_bar_low register ****/
/* Window size = 2 ^ (15 + win_size). Zero value: disable the window. */
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM2_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM2_BAR_LOW_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM2_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM2_BAR_LOW_RSRVD_SHIFT 6
/* Reserved */
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM2_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM2_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** pcie_ext_ecam3_bar_low register ****/
/* Window size = 2 ^ (15 + win_size). Zero value: disable the window. */
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM3_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM3_BAR_LOW_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM3_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM3_BAR_LOW_RSRVD_SHIFT 6
/* Reserved */
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM3_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM3_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** pcie_ext_ecam4_bar_low register ****/
/* Window size = 2 ^ (15 + win_size). Zero value: disable the window. */
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM4_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM4_BAR_LOW_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM4_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM4_BAR_LOW_RSRVD_SHIFT 6
/* Reserved */
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM4_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM4_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** pcie_ext_ecam5_bar_low register ****/
/* Window size = 2 ^ (15 + win_size). Zero value: disable the window. */
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM5_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM5_BAR_LOW_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM5_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM5_BAR_LOW_RSRVD_SHIFT 6
/* Reserved */
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM5_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM5_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** pcie_ext_ecam6_bar_low register ****/
/* Window size = 2 ^ (15 + win_size). Zero value: disable the window. */
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM6_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM6_BAR_LOW_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM6_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM6_BAR_LOW_RSRVD_SHIFT 6
/* Reserved */
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM6_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM6_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** pcie_ext_ecam7_bar_low register ****/
/* Window size = 2 ^ (15 + win_size). Zero value: disable the window. */
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM7_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM7_BAR_LOW_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM7_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM7_BAR_LOW_RSRVD_SHIFT 6
/* Reserved */
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM7_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_BAR_CONFIG_PCIE_EXT_ECAM7_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** coresight_bar_low register ****/
/* Window size = 2 ^ (15 + win_size). Zero value: disable the window. */
#define PBS_BAR_CONFIG_CORESIGHT_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_BAR_CONFIG_CORESIGHT_BAR_LOW_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_BAR_CONFIG_CORESIGHT_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_BAR_CONFIG_CORESIGHT_BAR_LOW_RSRVD_SHIFT 6
/* Reserved */
#define PBS_BAR_CONFIG_CORESIGHT_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_BAR_CONFIG_CORESIGHT_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** pbs_spi_bar_low register ****/
/* Window size = 2 ^ (15 + win_size). Zero value: disable the window. */
#define PBS_BAR_CONFIG_PBS_SPI_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_BAR_CONFIG_PBS_SPI_BAR_LOW_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_BAR_CONFIG_PBS_SPI_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_BAR_CONFIG_PBS_SPI_BAR_LOW_RSRVD_SHIFT 6
/* Reserved */
#define PBS_BAR_CONFIG_PBS_SPI_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_BAR_CONFIG_PBS_SPI_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** pbs_nand_bar_low register ****/
/* Window size = 2 ^ (15 + win_size). Zero value: disable the window. */
#define PBS_BAR_CONFIG_PBS_NAND_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_BAR_CONFIG_PBS_NAND_BAR_LOW_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_BAR_CONFIG_PBS_NAND_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_BAR_CONFIG_PBS_NAND_BAR_LOW_RSRVD_SHIFT 6
/* Reserved */
#define PBS_BAR_CONFIG_PBS_NAND_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_BAR_CONFIG_PBS_NAND_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** pbs_int_mem_bar_low register ****/
/* Window size = 2 ^ (15 + win_size). Zero value: disable the window. */
#define PBS_BAR_CONFIG_PBS_INT_MEM_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_BAR_CONFIG_PBS_INT_MEM_BAR_LOW_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_BAR_CONFIG_PBS_INT_MEM_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_BAR_CONFIG_PBS_INT_MEM_BAR_LOW_RSRVD_SHIFT 6
/* Reserved */
#define PBS_BAR_CONFIG_PBS_INT_MEM_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_BAR_CONFIG_PBS_INT_MEM_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** pbs_boot_bar_low register ****/
/* Window size = 2 ^ (15 + win_size). Zero value: disable the window. */
#define PBS_BAR_CONFIG_PBS_BOOT_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_BAR_CONFIG_PBS_BOOT_BAR_LOW_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_BAR_CONFIG_PBS_BOOT_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_BAR_CONFIG_PBS_BOOT_BAR_LOW_RSRVD_SHIFT 6
/* Reserved */
#define PBS_BAR_CONFIG_PBS_BOOT_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_BAR_CONFIG_PBS_BOOT_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** nb_int_bar_low register ****/
/* Window size = 2 ^ (15 + win_size). Zero value: disable the window. */
#define PBS_BAR_CONFIG_NB_INT_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_BAR_CONFIG_NB_INT_BAR_LOW_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_BAR_CONFIG_NB_INT_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_BAR_CONFIG_NB_INT_BAR_LOW_RSRVD_SHIFT 6
/* Reserved */
#define PBS_BAR_CONFIG_NB_INT_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_BAR_CONFIG_NB_INT_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** nb_stm_bar_low register ****/
/* Window size = 2 ^ (15 + win_size). Zero value: disable the window. */
#define PBS_BAR_CONFIG_NB_STM_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_BAR_CONFIG_NB_STM_BAR_LOW_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_BAR_CONFIG_NB_STM_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_BAR_CONFIG_NB_STM_BAR_LOW_RSRVD_SHIFT 6
/* Reserved */
#define PBS_BAR_CONFIG_NB_STM_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_BAR_CONFIG_NB_STM_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** pcie_ecam_int_bar_low register ****/
/* Window size = 2 ^ (15 + win_size). Zero value: disable the window. */
#define PBS_BAR_CONFIG_PCIE_ECAM_INT_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_BAR_CONFIG_PCIE_ECAM_INT_BAR_LOW_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_BAR_CONFIG_PCIE_ECAM_INT_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_BAR_CONFIG_PCIE_ECAM_INT_BAR_LOW_RSRVD_SHIFT 6
/* Reserved */
#define PBS_BAR_CONFIG_PCIE_ECAM_INT_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_BAR_CONFIG_PCIE_ECAM_INT_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** pcie_mem_int_bar_low register ****/
/* Window size = 2 ^ (15 + win_size). Zero value: disable the window. */
#define PBS_BAR_CONFIG_PCIE_MEM_INT_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_BAR_CONFIG_PCIE_MEM_INT_BAR_LOW_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_BAR_CONFIG_PCIE_MEM_INT_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_BAR_CONFIG_PCIE_MEM_INT_BAR_LOW_RSRVD_SHIFT 6
/* Reserved */
#define PBS_BAR_CONFIG_PCIE_MEM_INT_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_BAR_CONFIG_PCIE_MEM_INT_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** sb_int_bar_low register ****/
/* Window size = 2 ^ (15 + win_size). Zero value: disable the window. */
#define PBS_BAR_CONFIG_SB_INT_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_BAR_CONFIG_SB_INT_BAR_LOW_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_BAR_CONFIG_SB_INT_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_BAR_CONFIG_SB_INT_BAR_LOW_RSRVD_SHIFT 6
/* Reserved */
#define PBS_BAR_CONFIG_SB_INT_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_BAR_CONFIG_SB_INT_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** fast_sb_nic_regs_bar_low register ****/
/* Window size = 2 ^ (15 + win_size). Zero value: disable the window. */
#define PBS_BAR_CONFIG_FAST_SB_NIC_REGS_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_BAR_CONFIG_FAST_SB_NIC_REGS_BAR_LOW_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_BAR_CONFIG_FAST_SB_NIC_REGS_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_BAR_CONFIG_FAST_SB_NIC_REGS_BAR_LOW_RSRVD_SHIFT 6
/* Reserved */
#define PBS_BAR_CONFIG_FAST_SB_NIC_REGS_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_BAR_CONFIG_FAST_SB_NIC_REGS_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** sb_nic_regs_bar_low register ****/
/* Window size = 2 ^ (15 + win_size). Zero value: disable the window. */
#define PBS_BAR_CONFIG_SB_NIC_REGS_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_BAR_CONFIG_SB_NIC_REGS_BAR_LOW_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_BAR_CONFIG_SB_NIC_REGS_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_BAR_CONFIG_SB_NIC_REGS_BAR_LOW_RSRVD_SHIFT 6
/* Reserved */
#define PBS_BAR_CONFIG_SB_NIC_REGS_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_BAR_CONFIG_SB_NIC_REGS_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** low_latency_sram_bar_low register ****/
/* Window size = 2 ^ (15 + win_size). Zero value: disable the window. */
#define PBS_BAR_CONFIG_LOW_LATENCY_SRAM_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_BAR_CONFIG_LOW_LATENCY_SRAM_BAR_LOW_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_BAR_CONFIG_LOW_LATENCY_SRAM_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_BAR_CONFIG_LOW_LATENCY_SRAM_BAR_LOW_RSRVD_SHIFT 6
/* Reserved */
#define PBS_BAR_CONFIG_LOW_LATENCY_SRAM_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_BAR_CONFIG_LOW_LATENCY_SRAM_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** eth_sram_0_bar_low register ****/
/* Window size = 2 ^ (15 + win_size). Zero value: disable the window. */
#define PBS_BAR_CONFIG_ETH_SRAM_0_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_BAR_CONFIG_ETH_SRAM_0_BAR_LOW_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_BAR_CONFIG_ETH_SRAM_0_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_BAR_CONFIG_ETH_SRAM_0_BAR_LOW_RSRVD_SHIFT 6
/* Reserved */
#define PBS_BAR_CONFIG_ETH_SRAM_0_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_BAR_CONFIG_ETH_SRAM_0_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** eth_sram_1_bar_low register ****/
/* Window size = 2 ^ (15 + win_size). Zero value: disable the window. */
#define PBS_BAR_CONFIG_ETH_SRAM_1_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_BAR_CONFIG_ETH_SRAM_1_BAR_LOW_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_BAR_CONFIG_ETH_SRAM_1_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_BAR_CONFIG_ETH_SRAM_1_BAR_LOW_RSRVD_SHIFT 6
/* Reserved */
#define PBS_BAR_CONFIG_ETH_SRAM_1_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_BAR_CONFIG_ETH_SRAM_1_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** trng_bar_low register ****/
/* Window size = 2 ^ (15 + win_size). Zero value: disable the window. */
#define PBS_BAR_CONFIG_TRNG_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_BAR_CONFIG_TRNG_BAR_LOW_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_BAR_CONFIG_TRNG_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_BAR_CONFIG_TRNG_BAR_LOW_RSRVD_SHIFT 6
/* Reserved */
#define PBS_BAR_CONFIG_TRNG_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_BAR_CONFIG_TRNG_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** tdm_bar_low register ****/
/* Window size = 2 ^ (15 + win_size). Zero value: disable the window. */
#define PBS_BAR_CONFIG_TDM_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_BAR_CONFIG_TDM_BAR_LOW_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_BAR_CONFIG_TDM_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_BAR_CONFIG_TDM_BAR_LOW_RSRVD_SHIFT 6
/* Reserved */
#define PBS_BAR_CONFIG_TDM_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_BAR_CONFIG_TDM_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** pcie_mem7_bar_low register ****/
/* Window size = 2 ^ (15 + win_size). Zero value: disable the window. */
#define PBS_BAR_CONFIG_PCIE_MEM7_BAR_LOW_WIN_SIZE_MASK 0x0000003F
#define PBS_BAR_CONFIG_PCIE_MEM7_BAR_LOW_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_BAR_CONFIG_PCIE_MEM7_BAR_LOW_RSRVD_MASK 0x0000FFC0
#define PBS_BAR_CONFIG_PCIE_MEM7_BAR_LOW_RSRVD_SHIFT 6
/* Reserved */
#define PBS_BAR_CONFIG_PCIE_MEM7_BAR_LOW_ADDR_HIGH_MASK 0xFFFF0000
#define PBS_BAR_CONFIG_PCIE_MEM7_BAR_LOW_ADDR_HIGH_SHIFT 16

/**** winit_cntl register ****/
/* When set, enables access to winit regs, in normal mode. */
#define PBS_BAR_CONFIG_WINIT_CNTL_ENABLE_WINIT_REGS_ACCESS (1 << 0)
/* Reserved */
#define PBS_BAR_CONFIG_WINIT_CNTL_RSRVD_MASK 0xFFFFFFFE
#define PBS_BAR_CONFIG_WINIT_CNTL_RSRVD_SHIFT 1

/**** latch_bars register ****/
/* Software clears this bit before any bar update, and set it after all bars updated. */
#define PBS_BAR_CONFIG_LATCH_BARS_ENABLE (1 << 0)
/* Reserved */
#define PBS_BAR_CONFIG_LATCH_BARS_RSRVD_MASK 0xFFFFFFFE
#define PBS_BAR_CONFIG_LATCH_BARS_RSRVD_SHIFT 1

/**** dma_io_master_map1 register ****/
/*
 * [0]: When set, if crypto_0  transaction doesn't hit any window, transaction is sent to NB/DRAM
 * (otherwise to ERROR)
 * [1]: When set, if crypto_1  transaction doesn't hit any window, transaction is sent to NB/DRAM
 * (otherwise to ERROR)
 * [2]: When set, if eth_0 transaction doesn't hit any window, transaction is sent to NB/DRAM
 * (otherwise to ERROR)
 * [3]: When set, if eth_1 transaction doesn't hit any window, transaction is sent to NB/DRAM
 * (otherwise to ERROR)
 * [4]: When set, if eth_2 transaction doesn't hit any window, transaction is sent to NB/DRAM
 * (otherwise to ERROR)
 * [5]: When set, if eth_3 transaction doesn't hit any window, transaction is sent to NB/DRAM
 * (otherwise to ERROR)
 * [6]: When set, if eth_4 transaction doesn't hit any window, transaction is sent to NB/DRAM
 * (otherwise to ERROR)
 * [7]: When set, if eth_5 transaction doesn't hit any window, transaction is sent to NB/DRAM
 * (otherwise to ERROR)
 * [9:8] - Reserved
 * [10]: When set, if SPI_DEBUG transaction doesn't hit any window, transaction is sent to NB/DRAM
 * (otherwise to ERROR)
 * [11]: When set, if CPU_DEBUG transaction doesn't hit any window, transaction is sent to NB/DRAM
 * (otherwise to ERROR)
 * [12]: When set, if pcie_0 transaction doesn't hit any window, transaction is sent to
 * NB/DRAM (otherwise to ERROR)
 * [13]: When set, if pcie_1 transaction doesn't hit any window, transaction is sent to
 * NB/DRAM (otherwise to ERROR)
 * [14]: When set, if pcie_2 transaction doesn't hit any window, transaction is sent to NB/DRAM
 * (otherwise to ERROR)
 * [15]: When set, if pcie_3 transaction doesn't hit any window, transaction is sent to NB/DRAM
 * (otherwise to ERROR)
 * [16]: When set, if pcie_4 transaction doesn't hit any window, transaction is sent to NB/DRAM
 * (otherwise to ERROR)
 * [17]: When set, if pcie_5 transaction doesn't hit any window, transaction is sent to NB/DRAM
 * (otherwise to ERROR)
 * [18]: When set, if pcie_6 transaction doesn't hit any window, transaction is sent to NB/DRAM
 * (otherwise to ERROR)
 * [19]: When set, if pcie_7 transaction doesn't hit any window, transaction is sent to NB/DRAM
 * (otherwise to ERROR)
 * [21:20] - Reserved
 * [22]: When set, if sata_0 transaction doesn't hit any window, transaction is sent to NB/DRAM
 * (otherwise to ERROR)
 * [23]: When set, if sata_1 transaction doesn't hit any window, transaction is sent to NB/DRAM
 * (otherwise to ERROR)
 * [24]: When set, if sata_2 transaction doesn't hit any window, transaction is sent to NB/DRAM
 * (otherwise to ERROR)
 * [25]: When set, if sata_3 transaction doesn't hit any window, transaction is sent to NB/DRAM
 * (otherwise to ERROR)
 * [26]: When set, if sata_4 transaction doesn't hit any window, transaction is sent to NB/DRAM
 * (otherwise to ERROR)
 * [27]: When set, if sata_5 transaction doesn't hit any window, transaction is sent to NB/DRAM
 * (otherwise to ERROR)
 * [28]: When set, if sata_6 transaction doesn't hit any window, transaction is sent to NB/DRAM
 * (otherwise to ERROR)
 * [29]: When set, if sata_7 transaction doesn't hit any window, transaction is sent to NB/DRAM
 * (otherwise to ERROR)
 * [30] – Reserved
 * [31]: When set, if eth_db transaction doesn't hit any window, transaction is sent to NB/DRAM
 * (otherwise to ERROR)
 */
#define PBS_UNIT_DMA_IO_MASTER_MAP1_DFLT_SLV_MASK 0xFFFFFFFF
#define PBS_UNIT_DMA_IO_MASTER_MAP1_DFLT_SLV_SHIFT 0

/**** dma_io_master_map2 register ****/
/*
 * [0]: When set, maps all the crypto_0  transactions to the NB/DRAM, regardless of the window hit.
 * [1]: When set, maps all the crypto_1  transactions to the NB/DRAM, regardless of the window hit.
 * [2]: When set, maps all the eth_0 transactions to the NB/DRAM, regardless of the window hit.
 * [3]: When set, maps all the eth_1 transactions to the NB/DRAM, regardless of the window hit.
 * [4]: When set, maps all the eth_2 transaction to the NB/DRAM, regardless of the window hit.
 * [5]: When set, maps all the eth_3 transaction to the NB/DRAM, regardless of the window hit.
 * [6]: When set, maps all the eth_4 transaction to the NB/DRAM, regardless of the window hit.
 * [7]: When set, maps all the eth_5 transaction to the NB/DRAM, regardless of the window hit.
 * [9:8] - Reserved
 * [10]: When set, maps all the SPI debug port  transactions to the NB/DRAM, regardless of the
 * window hit.
 * [11]: When set, maps all the CPU debug port  transactions to the NB/DRAM, regardless of the
 * window hit.
 * [12]: When set, maps all the pcie_0  transactions to the NB/DRAM, regardless of the window hit.
 * [13]: When set, maps all the pcie_1  transactions to the NB/DRAM, regardless of the window hit.
 * [14]: When set, maps all the pcie_2  transactions to the NB/DRAM, regardless of the window hit.
 * [15]: When set, maps all the pcie_3  transactions to the NB/DRAM, regardless of the window hit.
 * [16]: When set, maps all the pcie_4  transactions to the NB/DRAM, regardless of the window hit.
 * [17]: When set, maps all the pcie_5  transactions to the NB/DRAM, regardless of the window hit.
 * [18]: When set, maps all the pcie_6  transactions to the NB/DRAM, regardless of the window hit.
 * [19]: When set, maps all the pcie_7  transactions to the NB/DRAM, regardless of the window hit.
 * [21:20] - Reserved
 * [22]: When set, maps all the sata_0 transactions to the NB/DRAM, regardless of the window hit.
 * [23]: When set, maps all the sata_1  transactions to the NB/DRAM, regardless of the window hit.
 * [24]: When set, maps all the sata_2  transactions to the NB/DRAM, regardless of the window hit.
 * [25]: When set, maps all the sata_3  transactions to the NB/DRAM, regardless of the window hit.
 * [26]: When set, maps all the sata_4  transactions to the NB/DRAM, regardless of the window hit.
 * [27]: When set, maps all the sata_5  transactions to the NB/DRAM, regardless of the window hit.
 * [28]: When set, maps all the sata_6  transactions to the NB/DRAM, regardless of the window hit.
 * [29]: When set, maps all the sata_7  transactions to the NB/DRAM, regardless of the window hit.
 * [30] – Reserved
 * [31]: When set, maps all the eth_db transactions to the NB/DRAM, regardless of the window hit.
 */
#define PBS_UNIT_DMA_IO_MASTER_MAP2_CNTL_MASK 0xFFFFFFFF
#define PBS_UNIT_DMA_IO_MASTER_MAP2_CNTL_SHIFT 0

/**** bar1_orig register ****/
/*
 * Window size = 2 ^ (11 + win_size).
 * Zero value: disable the window.
 */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR1_ORIG_WIN_SIZE_MASK 0x0000003F
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR1_ORIG_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR1_ORIG_RSRVD_MASK 0x00000FC0
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR1_ORIG_RSRVD_SHIFT 6
/*
 * offset within the SRAM, in resolution of 4KB.
 * Only offsets which are inside the boundaries of the SRAM bar are allowed
 */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR1_ORIG_ADDR_HIGH_MASK 0xFFFFF000
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR1_ORIG_ADDR_HIGH_SHIFT 12

/**** bar1_remap register ****/
/* Reserved fields */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR1_REMAP_RSRVD_MASK 0x00000FFF
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR1_REMAP_RSRVD_SHIFT 0
/* remapped address */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR1_REMAP_ADDR_HIGH_MASK 0xFFFFF000
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR1_REMAP_ADDR_HIGH_SHIFT 12

/**** bar2_orig register ****/
/*
 * Window size = 2 ^ (11 + win_size).
 * Zero value: disable the window.
 */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR2_ORIG_WIN_SIZE_MASK 0x0000003F
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR2_ORIG_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR2_ORIG_RSRVD_MASK 0x00000FC0
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR2_ORIG_RSRVD_SHIFT 6
/*
 * offset within the SRAM, in resolution of 4KB.
 * Only offsets which are inside the boundaries of the SRAM bar are allowed
 */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR2_ORIG_ADDR_HIGH_MASK 0xFFFFF000
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR2_ORIG_ADDR_HIGH_SHIFT 12

/**** bar2_remap register ****/
/* Reserved fields */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR2_REMAP_RSRVD_MASK 0x00000FFF
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR2_REMAP_RSRVD_SHIFT 0
/* remapped address */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR2_REMAP_ADDR_HIGH_MASK 0xFFFFF000
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR2_REMAP_ADDR_HIGH_SHIFT 12

/**** bar3_orig register ****/
/*
 * Window size = 2 ^ (11 + win_size).
 * Zero value: disable the window.
 */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR3_ORIG_WIN_SIZE_MASK 0x0000003F
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR3_ORIG_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR3_ORIG_RSRVD_MASK 0x00000FC0
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR3_ORIG_RSRVD_SHIFT 6
/*
 * offset within the SRAM, in resolution of 4KB.
 * Only offsets which are inside the boundaries of the SRAM bar are allowed
 */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR3_ORIG_ADDR_HIGH_MASK 0xFFFFF000
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR3_ORIG_ADDR_HIGH_SHIFT 12

/**** bar3_remap register ****/
/* Reserved fields */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR3_REMAP_RSRVD_MASK 0x00000FFF
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR3_REMAP_RSRVD_SHIFT 0
/* remapped address */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR3_REMAP_ADDR_HIGH_MASK 0xFFFFF000
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR3_REMAP_ADDR_HIGH_SHIFT 12

/**** bar4_orig register ****/
/*
 * Window size = 2 ^ (11 + win_size).
 * Zero value: disable the window.
 */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR4_ORIG_WIN_SIZE_MASK 0x0000003F
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR4_ORIG_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR4_ORIG_RSRVD_MASK 0x00000FC0
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR4_ORIG_RSRVD_SHIFT 6
/*
 * offset within the SRAM, in resolution of 4KB.
 * Only offsets which are inside the boundaries of the SRAM bar are allowed
 */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR4_ORIG_ADDR_HIGH_MASK 0xFFFFF000
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR4_ORIG_ADDR_HIGH_SHIFT 12

/**** bar4_remap register ****/
/* Reserved fields */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR4_REMAP_RSRVD_MASK 0x00000FFF
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR4_REMAP_RSRVD_SHIFT 0
/* remapped address */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR4_REMAP_ADDR_HIGH_MASK 0xFFFFF000
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR4_REMAP_ADDR_HIGH_SHIFT 12

/**** bar5_orig register ****/
/*
 * Window size = 2 ^ (11 + win_size).
 * Zero value: disable the window.
 */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR5_ORIG_WIN_SIZE_MASK 0x0000003F
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR5_ORIG_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR5_ORIG_RSRVD_MASK 0x00000FC0
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR5_ORIG_RSRVD_SHIFT 6
/*
 * offset within the SRAM, in resolution of 4KB.
 * Only offsets which are inside the boundaries of the SRAM bar are allowed
 */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR5_ORIG_ADDR_HIGH_MASK 0xFFFFF000
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR5_ORIG_ADDR_HIGH_SHIFT 12

/**** bar5_remap register ****/
/* Reserved fields */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR5_REMAP_RSRVD_MASK 0x00000FFF
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR5_REMAP_RSRVD_SHIFT 0
/* remapped address */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR5_REMAP_ADDR_HIGH_MASK 0xFFFFF000
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR5_REMAP_ADDR_HIGH_SHIFT 12

/**** bar6_orig register ****/
/*
 * Window size = 2 ^ (11 + win_size).
 * Zero value: disable the window.
 */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR6_ORIG_WIN_SIZE_MASK 0x0000003F
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR6_ORIG_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR6_ORIG_RSRVD_MASK 0x00000FC0
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR6_ORIG_RSRVD_SHIFT 6
/*
 * offset within the SRAM, in resolution of 4KB.
 * Only offsets which are inside the boundaries of the SRAM bar are allowed
 */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR6_ORIG_ADDR_HIGH_MASK 0xFFFFF000
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR6_ORIG_ADDR_HIGH_SHIFT 12

/**** bar6_remap register ****/
/* Reserved fields */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR6_REMAP_RSRVD_MASK 0x00000FFF
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR6_REMAP_RSRVD_SHIFT 0
/* remapped address */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR6_REMAP_ADDR_HIGH_MASK 0xFFFFF000
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR6_REMAP_ADDR_HIGH_SHIFT 12

/**** bar7_orig register ****/
/*
 * Window size = 2 ^ (11 + win_size).
 * Zero value: disable the window.
 */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR7_ORIG_WIN_SIZE_MASK 0x0000003F
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR7_ORIG_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR7_ORIG_RSRVD_MASK 0x00000FC0
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR7_ORIG_RSRVD_SHIFT 6
/*
 * offset within the SRAM, in resolution of 4KB.
 * Only offsets which are inside the boundaries of the SRAM bar are allowed
 */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR7_ORIG_ADDR_HIGH_MASK 0xFFFFF000
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR7_ORIG_ADDR_HIGH_SHIFT 12

/**** bar7_remap register ****/
/* Reserved fields */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR7_REMAP_RSRVD_MASK 0x00000FFF
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR7_REMAP_RSRVD_SHIFT 0
/* remapped address */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR7_REMAP_ADDR_HIGH_MASK 0xFFFFF000
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR7_REMAP_ADDR_HIGH_SHIFT 12

/**** bar8_orig register ****/
/*
 * Window size = 2 ^ (11 + win_size).
 * Zero value: disable the window.
 */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR8_ORIG_WIN_SIZE_MASK 0x0000003F
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR8_ORIG_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR8_ORIG_RSRVD_MASK 0x00000FC0
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR8_ORIG_RSRVD_SHIFT 6
/*
 * offset within the SRAM, in resolution of 4KB.
 * Only offsets which are inside the boundaries of the SRAM bar are allowed
 */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR8_ORIG_ADDR_HIGH_MASK 0xFFFFF000
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR8_ORIG_ADDR_HIGH_SHIFT 12

/**** bar8_remap register ****/
/* Reserved fields */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR8_REMAP_RSRVD_MASK 0x00000FFF
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR8_REMAP_RSRVD_SHIFT 0
/* remapped address */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR8_REMAP_ADDR_HIGH_MASK 0xFFFFF000
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR8_REMAP_ADDR_HIGH_SHIFT 12

/**** bar9_orig register ****/
/*
 * Window size = 2 ^ (11 + win_size).
 * Zero value: disable the window.
 */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR9_ORIG_WIN_SIZE_MASK 0x0000003F
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR9_ORIG_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR9_ORIG_RSRVD_MASK 0x00000FC0
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR9_ORIG_RSRVD_SHIFT 6
/*
 * offset within the SRAM, in resolution of 4KB.
 * Only offsets which are inside the boundaries of the SRAM bar are allowed
 */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR9_ORIG_ADDR_HIGH_MASK 0xFFFFF000
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR9_ORIG_ADDR_HIGH_SHIFT 12

/**** bar9_remap register ****/
/* Reserved fields */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR9_REMAP_RSRVD_MASK 0x00000FFF
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR9_REMAP_RSRVD_SHIFT 0
/* remapped address */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR9_REMAP_ADDR_HIGH_MASK 0xFFFFF000
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR9_REMAP_ADDR_HIGH_SHIFT 12

/**** bar10_orig register ****/
/*
 * Window size = 2 ^ (11 + win_size).
 * Zero value: disable the window.
 */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR10_ORIG_WIN_SIZE_MASK 0x0000003F
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR10_ORIG_WIN_SIZE_SHIFT 0
/* Reserved fields */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR10_ORIG_RSRVD_MASK 0x00000FC0
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR10_ORIG_RSRVD_SHIFT 6
/*
 * offset within the SRAM, in resolution of 4KB.
 * Only offsets which are inside the boundaries of the SRAM bar are allowed
 */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR10_ORIG_ADDR_HIGH_MASK 0xFFFFF000
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR10_ORIG_ADDR_HIGH_SHIFT 12

/**** bar10_remap register ****/
/* Reserved fields */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR10_REMAP_RSRVD_MASK 0x00000FFF
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR10_REMAP_RSRVD_SHIFT 0
/* remapped address */
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR10_REMAP_ADDR_HIGH_MASK 0xFFFFF000
#define PBS_LOW_LATENCY_SRAM_REMAP_BAR10_REMAP_ADDR_HIGH_SHIFT 12

/**** cpu register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_CPU_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_CPU_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_CPU_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_CPU_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_CPU_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_CPU_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_CPU_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_CPU_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_CPU_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_CPU_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_CPU_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_CPU_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_CPU_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_CPU_PCIE_5_SHIFT 24
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_CPU_DRAM_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_CPU_DRAM_SHIFT 28

/**** cpu_mask register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_CPU_MASK_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_CPU_MASK_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_CPU_MASK_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_CPU_MASK_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_CPU_MASK_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_CPU_MASK_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_CPU_MASK_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_CPU_MASK_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_CPU_MASK_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_CPU_MASK_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_CPU_MASK_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_CPU_MASK_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_CPU_MASK_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_CPU_MASK_PCIE_5_SHIFT 24
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_CPU_MASK_DRAM_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_CPU_MASK_DRAM_SHIFT 28

/**** debug_nb register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_PCIE_5_SHIFT 24
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_DRAM_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_DRAM_SHIFT 28

/**** debug_nb_mask register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_MASK_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_MASK_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_MASK_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_MASK_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_MASK_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_MASK_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_MASK_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_MASK_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_MASK_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_MASK_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_MASK_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_MASK_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_MASK_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_MASK_PCIE_5_SHIFT 24
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_MASK_DRAM_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_MASK_DRAM_SHIFT 28

/**** debug_sb register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_PCIE_5_SHIFT 24
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_DRAM_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_DRAM_SHIFT 28

/**** debug_sb_mask register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_MASK_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_MASK_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_MASK_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_MASK_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_MASK_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_MASK_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_MASK_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_MASK_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_MASK_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_MASK_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_MASK_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_MASK_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_MASK_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_MASK_PCIE_5_SHIFT 24
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_MASK_DRAM_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_MASK_DRAM_SHIFT 28

/**** eth_0 register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_PCIE_5_SHIFT 24
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_DRAM_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_DRAM_SHIFT 28

/**** eth_0_mask register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_MASK_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_MASK_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_MASK_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_MASK_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_MASK_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_MASK_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_MASK_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_MASK_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_MASK_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_MASK_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_MASK_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_MASK_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_MASK_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_MASK_PCIE_5_SHIFT 24
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_MASK_DRAM_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_MASK_DRAM_SHIFT 28

/**** eth_1 register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_PCIE_5_SHIFT 24
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_DRAM_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_DRAM_SHIFT 28

/**** eth_1_mask register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_MASK_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_MASK_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_MASK_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_MASK_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_MASK_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_MASK_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_MASK_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_MASK_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_MASK_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_MASK_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_MASK_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_MASK_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_MASK_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_MASK_PCIE_5_SHIFT 24
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_MASK_DRAM_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_MASK_DRAM_SHIFT 28

/**** eth_2 register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_2_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_ETH_2_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_2_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_ETH_2_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_2_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_ETH_2_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_2_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_2_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_2_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_2_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_2_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_2_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_2_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_2_PCIE_5_SHIFT 24
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_2_DRAM_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_2_DRAM_SHIFT 28

/**** eth_2_mask register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_2_MASK_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_ETH_2_MASK_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_2_MASK_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_ETH_2_MASK_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_2_MASK_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_ETH_2_MASK_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_2_MASK_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_2_MASK_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_2_MASK_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_2_MASK_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_2_MASK_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_2_MASK_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_2_MASK_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_2_MASK_PCIE_5_SHIFT 24
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_2_MASK_DRAM_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_2_MASK_DRAM_SHIFT 28

/**** eth_3 register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_3_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_ETH_3_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_3_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_ETH_3_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_3_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_ETH_3_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_3_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_3_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_3_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_3_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_3_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_3_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_3_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_3_PCIE_5_SHIFT 24
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_3_DRAM_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_3_DRAM_SHIFT 28

/**** eth_3_mask register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_3_MASK_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_ETH_3_MASK_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_3_MASK_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_ETH_3_MASK_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_3_MASK_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_ETH_3_MASK_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_3_MASK_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_3_MASK_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_3_MASK_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_3_MASK_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_3_MASK_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_3_MASK_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_3_MASK_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_3_MASK_PCIE_5_SHIFT 24
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_3_MASK_DRAM_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_3_MASK_DRAM_SHIFT 28

/**** sata_0 register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_PCIE_5_SHIFT 24
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_DRAM_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_DRAM_SHIFT 28

/**** sata_0_mask register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_MASK_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_MASK_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_MASK_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_MASK_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_MASK_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_MASK_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_MASK_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_MASK_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_MASK_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_MASK_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_MASK_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_MASK_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_MASK_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_MASK_PCIE_5_SHIFT 24
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_MASK_DRAM_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_MASK_DRAM_SHIFT 28

/**** sata_1 register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_PCIE_5_SHIFT 24
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_DRAM_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_DRAM_SHIFT 28

/**** sata_1_mask register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_MASK_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_MASK_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_MASK_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_MASK_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_MASK_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_MASK_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_MASK_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_MASK_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_MASK_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_MASK_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_MASK_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_MASK_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_MASK_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_MASK_PCIE_5_SHIFT 24
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_MASK_DRAM_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_MASK_DRAM_SHIFT 28

/**** crypto_0 register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_PCIE_5_SHIFT 24
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_DRAM_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_DRAM_SHIFT 28

/**** crypto_0_mask register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_MASK_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_MASK_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_MASK_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_MASK_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_MASK_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_MASK_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_MASK_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_MASK_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_MASK_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_MASK_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_MASK_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_MASK_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_MASK_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_MASK_PCIE_5_SHIFT 24
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_MASK_DRAM_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_MASK_DRAM_SHIFT 28

/**** crypto_1 register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_PCIE_5_SHIFT 24
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_DRAM_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_DRAM_SHIFT 28

/**** crypto_1_mask register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_MASK_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_MASK_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_MASK_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_MASK_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_MASK_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_MASK_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_MASK_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_MASK_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_MASK_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_MASK_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_MASK_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_MASK_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_MASK_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_MASK_PCIE_5_SHIFT 24
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_MASK_DRAM_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_MASK_DRAM_SHIFT 28

/**** pcie_0 register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_PCIE_5_SHIFT 24
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_DRAM_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_DRAM_SHIFT 28

/**** pcie_0_mask register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_MASK_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_MASK_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_MASK_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_MASK_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_MASK_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_MASK_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_MASK_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_MASK_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_MASK_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_MASK_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_MASK_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_MASK_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_MASK_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_MASK_PCIE_5_SHIFT 24
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_MASK_DRAM_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_MASK_DRAM_SHIFT 28

/**** pcie_1 register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_PCIE_5_SHIFT 24
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_DRAM_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_DRAM_SHIFT 28

/**** pcie_1_mask register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_MASK_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_MASK_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_MASK_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_MASK_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_MASK_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_MASK_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_MASK_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_MASK_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_MASK_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_MASK_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_MASK_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_MASK_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_MASK_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_MASK_PCIE_5_SHIFT 24
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_MASK_DRAM_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_MASK_DRAM_SHIFT 28

/**** pcie_2 register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_PCIE_5_SHIFT 24
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_DRAM_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_DRAM_SHIFT 28

/**** pcie_2_mask register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_MASK_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_MASK_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_MASK_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_MASK_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_MASK_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_MASK_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_MASK_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_MASK_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_MASK_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_MASK_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_MASK_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_MASK_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_MASK_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_MASK_PCIE_5_SHIFT 24
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_MASK_DRAM_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_MASK_DRAM_SHIFT 28

/**** pcie_3 register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_PCIE_5_SHIFT 24
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_DRAM_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_DRAM_SHIFT 28

/**** pcie_3_mask register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_MASK_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_MASK_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_MASK_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_MASK_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_MASK_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_MASK_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_MASK_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_MASK_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_MASK_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_MASK_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_MASK_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_MASK_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_MASK_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_MASK_PCIE_5_SHIFT 24
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_MASK_DRAM_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_MASK_DRAM_SHIFT 28

/**** cpu_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_CPU_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_CPU_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_CPU_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_CPU_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_CPU_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_CPU_HIGH_RESERVED_31_8_SHIFT 8

/**** cpu_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_CPU_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_CPU_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_CPU_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_CPU_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_CPU_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_CPU_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_CPU_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_CPU_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_CPU_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_CPU_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_CPU_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_CPU_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_CPU_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_CPU_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_CPU_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_CPU_LOW_PCIE_6_SHIFT 28

/**** cpu_mask_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_CPU_MASK_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_CPU_MASK_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_CPU_MASK_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_CPU_MASK_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_CPU_MASK_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_CPU_MASK_HIGH_RESERVED_31_8_SHIFT 8

/**** cpu_mask_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_CPU_MASK_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_CPU_MASK_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_CPU_MASK_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_CPU_MASK_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_CPU_MASK_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_CPU_MASK_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_CPU_MASK_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_CPU_MASK_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_CPU_MASK_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_CPU_MASK_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_CPU_MASK_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_CPU_MASK_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_CPU_MASK_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_CPU_MASK_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_CPU_MASK_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_CPU_MASK_LOW_PCIE_6_SHIFT 28

/**** debug_nb_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_HIGH_RESERVED_31_8_SHIFT 8

/**** debug_nb_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_LOW_PCIE_6_SHIFT 28

/**** debug_nb_mask_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_MASK_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_MASK_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_MASK_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_MASK_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_MASK_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_MASK_HIGH_RESERVED_31_8_SHIFT 8

/**** debug_nb_mask_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_MASK_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_MASK_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_MASK_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_MASK_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_MASK_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_MASK_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_MASK_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_MASK_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_MASK_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_MASK_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_MASK_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_MASK_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_MASK_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_MASK_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_MASK_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_NB_MASK_LOW_PCIE_6_SHIFT 28

/**** debug_sb_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_HIGH_RESERVED_31_8_SHIFT 8

/**** debug_sb_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_LOW_PCIE_6_SHIFT 28

/**** debug_sb_mask_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_MASK_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_MASK_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_MASK_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_MASK_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_MASK_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_MASK_HIGH_RESERVED_31_8_SHIFT 8

/**** debug_sb_mask_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_MASK_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_MASK_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_MASK_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_MASK_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_MASK_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_MASK_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_MASK_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_MASK_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_MASK_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_MASK_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_MASK_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_MASK_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_MASK_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_MASK_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_MASK_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_DEBUG_SB_MASK_LOW_PCIE_6_SHIFT 28

/**** eth_fast_0_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_0_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_0_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_0_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_0_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_0_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_0_HIGH_RESERVED_31_8_SHIFT 8

/**** eth_fast_0_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_0_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_0_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_0_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_0_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_0_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_0_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_0_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_0_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_0_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_0_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_0_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_0_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_0_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_0_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_0_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_0_LOW_PCIE_6_SHIFT 28

/**** eth_fast_0_mask_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_0_MASK_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_0_MASK_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_0_MASK_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_0_MASK_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_0_MASK_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_0_MASK_HIGH_RESERVED_31_8_SHIFT 8

/**** eth_fast_0_mask_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_0_MASK_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_0_MASK_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_0_MASK_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_0_MASK_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_0_MASK_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_0_MASK_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_0_MASK_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_0_MASK_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_0_MASK_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_0_MASK_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_0_MASK_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_0_MASK_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_0_MASK_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_0_MASK_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_0_MASK_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_0_MASK_LOW_PCIE_6_SHIFT 28

/**** eth_fast_1_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_1_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_1_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_1_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_1_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_1_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_1_HIGH_RESERVED_31_8_SHIFT 8

/**** eth_fast_1_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_1_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_1_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_1_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_1_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_1_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_1_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_1_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_1_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_1_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_1_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_1_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_1_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_1_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_1_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_1_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_1_LOW_PCIE_6_SHIFT 28

/**** eth_fast_1_mask_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_1_MASK_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_1_MASK_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_1_MASK_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_1_MASK_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_1_MASK_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_1_MASK_HIGH_RESERVED_31_8_SHIFT 8

/**** eth_fast_1_mask_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_1_MASK_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_1_MASK_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_1_MASK_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_1_MASK_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_1_MASK_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_1_MASK_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_1_MASK_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_1_MASK_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_1_MASK_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_1_MASK_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_1_MASK_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_1_MASK_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_1_MASK_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_1_MASK_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_1_MASK_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_1_MASK_LOW_PCIE_6_SHIFT 28

/**** eth_fast_2_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_2_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_2_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_2_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_2_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_2_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_2_HIGH_RESERVED_31_8_SHIFT 8

/**** eth_fast_2_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_2_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_2_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_2_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_2_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_2_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_2_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_2_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_2_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_2_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_2_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_2_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_2_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_2_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_2_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_2_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_2_LOW_PCIE_6_SHIFT 28

/**** eth_fast_2_mask_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_2_MASK_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_2_MASK_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_2_MASK_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_2_MASK_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_2_MASK_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_2_MASK_HIGH_RESERVED_31_8_SHIFT 8

/**** eth_fast_2_mask_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_2_MASK_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_2_MASK_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_2_MASK_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_2_MASK_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_2_MASK_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_2_MASK_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_2_MASK_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_2_MASK_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_2_MASK_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_2_MASK_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_2_MASK_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_2_MASK_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_2_MASK_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_2_MASK_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_2_MASK_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_2_MASK_LOW_PCIE_6_SHIFT 28

/**** eth_fast_3_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_3_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_3_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_3_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_3_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_3_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_3_HIGH_RESERVED_31_8_SHIFT 8

/**** eth_fast_3_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_3_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_3_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_3_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_3_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_3_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_3_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_3_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_3_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_3_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_3_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_3_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_3_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_3_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_3_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_3_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_3_LOW_PCIE_6_SHIFT 28

/**** eth_fast_3_mask_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_3_MASK_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_3_MASK_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_3_MASK_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_3_MASK_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_3_MASK_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_3_MASK_HIGH_RESERVED_31_8_SHIFT 8

/**** eth_fast_3_mask_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_3_MASK_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_3_MASK_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_3_MASK_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_3_MASK_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_3_MASK_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_3_MASK_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_3_MASK_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_3_MASK_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_3_MASK_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_3_MASK_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_3_MASK_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_3_MASK_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_3_MASK_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_3_MASK_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_3_MASK_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_FAST_3_MASK_LOW_PCIE_6_SHIFT 28

/**** sata_0_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_HIGH_RESERVED_31_8_SHIFT 8

/**** sata_0_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_LOW_PCIE_6_SHIFT 28

/**** sata_0_mask_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_MASK_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_MASK_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_MASK_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_MASK_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_MASK_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_MASK_HIGH_RESERVED_31_8_SHIFT 8

/**** sata_0_mask_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_MASK_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_MASK_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_MASK_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_MASK_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_MASK_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_MASK_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_MASK_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_MASK_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_MASK_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_MASK_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_MASK_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_MASK_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_MASK_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_MASK_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_MASK_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_0_MASK_LOW_PCIE_6_SHIFT 28

/**** sata_1_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_HIGH_RESERVED_31_8_SHIFT 8

/**** sata_1_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_LOW_PCIE_6_SHIFT 28

/**** sata_1_mask_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_MASK_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_MASK_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_MASK_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_MASK_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_MASK_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_MASK_HIGH_RESERVED_31_8_SHIFT 8

/**** sata_1_mask_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_MASK_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_MASK_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_MASK_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_MASK_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_MASK_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_MASK_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_MASK_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_MASK_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_MASK_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_MASK_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_MASK_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_MASK_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_MASK_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_MASK_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_MASK_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_1_MASK_LOW_PCIE_6_SHIFT 28

/**** sata_2_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_2_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_SATA_2_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_2_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_SATA_2_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_2_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_SATA_2_HIGH_RESERVED_31_8_SHIFT 8

/**** sata_2_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_2_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_SATA_2_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_2_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_SATA_2_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_2_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_SATA_2_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_2_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_2_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_2_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_2_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_2_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_2_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_2_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_2_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_2_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_2_LOW_PCIE_6_SHIFT 28

/**** sata_2_mask_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_2_MASK_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_SATA_2_MASK_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_2_MASK_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_SATA_2_MASK_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_2_MASK_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_SATA_2_MASK_HIGH_RESERVED_31_8_SHIFT 8

/**** sata_2_mask_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_2_MASK_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_SATA_2_MASK_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_2_MASK_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_SATA_2_MASK_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_2_MASK_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_SATA_2_MASK_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_2_MASK_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_2_MASK_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_2_MASK_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_2_MASK_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_2_MASK_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_2_MASK_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_2_MASK_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_2_MASK_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_2_MASK_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_2_MASK_LOW_PCIE_6_SHIFT 28

/**** sata_3_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_3_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_SATA_3_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_3_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_SATA_3_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_3_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_SATA_3_HIGH_RESERVED_31_8_SHIFT 8

/**** sata_3_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_3_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_SATA_3_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_3_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_SATA_3_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_3_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_SATA_3_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_3_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_3_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_3_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_3_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_3_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_3_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_3_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_3_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_3_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_3_LOW_PCIE_6_SHIFT 28

/**** sata_3_mask_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_3_MASK_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_SATA_3_MASK_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_3_MASK_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_SATA_3_MASK_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_3_MASK_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_SATA_3_MASK_HIGH_RESERVED_31_8_SHIFT 8

/**** sata_3_mask_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_3_MASK_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_SATA_3_MASK_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_3_MASK_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_SATA_3_MASK_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_3_MASK_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_SATA_3_MASK_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_3_MASK_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_3_MASK_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_3_MASK_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_3_MASK_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_3_MASK_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_3_MASK_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_3_MASK_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_3_MASK_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_3_MASK_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_3_MASK_LOW_PCIE_6_SHIFT 28

/**** sata_4_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_4_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_SATA_4_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_4_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_SATA_4_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_4_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_SATA_4_HIGH_RESERVED_31_8_SHIFT 8

/**** sata_4_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_4_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_SATA_4_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_4_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_SATA_4_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_4_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_SATA_4_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_4_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_4_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_4_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_4_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_4_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_4_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_4_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_4_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_4_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_4_LOW_PCIE_6_SHIFT 28

/**** sata_4_mask_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_4_MASK_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_SATA_4_MASK_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_4_MASK_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_SATA_4_MASK_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_4_MASK_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_SATA_4_MASK_HIGH_RESERVED_31_8_SHIFT 8

/**** sata_4_mask_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_4_MASK_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_SATA_4_MASK_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_4_MASK_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_SATA_4_MASK_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_4_MASK_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_SATA_4_MASK_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_4_MASK_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_4_MASK_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_4_MASK_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_4_MASK_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_4_MASK_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_4_MASK_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_4_MASK_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_4_MASK_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_4_MASK_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_4_MASK_LOW_PCIE_6_SHIFT 28

/**** sata_5_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_5_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_SATA_5_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_5_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_SATA_5_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_5_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_SATA_5_HIGH_RESERVED_31_8_SHIFT 8

/**** sata_5_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_5_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_SATA_5_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_5_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_SATA_5_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_5_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_SATA_5_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_5_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_5_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_5_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_5_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_5_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_5_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_5_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_5_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_5_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_5_LOW_PCIE_6_SHIFT 28

/**** sata_5_mask_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_5_MASK_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_SATA_5_MASK_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_5_MASK_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_SATA_5_MASK_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_5_MASK_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_SATA_5_MASK_HIGH_RESERVED_31_8_SHIFT 8

/**** sata_5_mask_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_5_MASK_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_SATA_5_MASK_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_5_MASK_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_SATA_5_MASK_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_5_MASK_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_SATA_5_MASK_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_5_MASK_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_5_MASK_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_5_MASK_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_5_MASK_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_5_MASK_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_5_MASK_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_5_MASK_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_5_MASK_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_5_MASK_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_5_MASK_LOW_PCIE_6_SHIFT 28

/**** crypto_0_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_HIGH_RESERVED_31_8_SHIFT 8

/**** crypto_0_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_LOW_PCIE_6_SHIFT 28

/**** crypto_0_mask_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_MASK_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_MASK_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_MASK_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_MASK_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_MASK_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_MASK_HIGH_RESERVED_31_8_SHIFT 8

/**** crypto_0_mask_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_MASK_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_MASK_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_MASK_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_MASK_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_MASK_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_MASK_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_MASK_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_MASK_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_MASK_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_MASK_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_MASK_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_MASK_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_MASK_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_MASK_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_MASK_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_0_MASK_LOW_PCIE_6_SHIFT 28

/**** crypto_1_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_HIGH_RESERVED_31_8_SHIFT 8

/**** crypto_1_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_LOW_PCIE_6_SHIFT 28

/**** crypto_1_mask_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_MASK_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_MASK_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_MASK_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_MASK_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_MASK_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_MASK_HIGH_RESERVED_31_8_SHIFT 8

/**** crypto_1_mask_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_MASK_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_MASK_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_MASK_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_MASK_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_MASK_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_MASK_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_MASK_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_MASK_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_MASK_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_MASK_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_MASK_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_MASK_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_MASK_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_MASK_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_MASK_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_CRYPTO_1_MASK_LOW_PCIE_6_SHIFT 28

/**** pcie_0_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_HIGH_RESERVED_31_8_SHIFT 8

/**** pcie_0_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_LOW_PCIE_6_SHIFT 28

/**** pcie_0_mask_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_MASK_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_MASK_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_MASK_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_MASK_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_MASK_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_MASK_HIGH_RESERVED_31_8_SHIFT 8

/**** pcie_0_mask_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_MASK_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_MASK_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_MASK_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_MASK_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_MASK_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_MASK_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_MASK_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_MASK_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_MASK_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_MASK_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_MASK_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_MASK_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_MASK_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_MASK_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_MASK_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_0_MASK_LOW_PCIE_6_SHIFT 28

/**** pcie_1_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_HIGH_RESERVED_31_8_SHIFT 8

/**** pcie_1_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_LOW_PCIE_6_SHIFT 28

/**** pcie_1_mask_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_MASK_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_MASK_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_MASK_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_MASK_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_MASK_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_MASK_HIGH_RESERVED_31_8_SHIFT 8

/**** pcie_1_mask_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_MASK_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_MASK_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_MASK_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_MASK_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_MASK_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_MASK_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_MASK_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_MASK_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_MASK_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_MASK_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_MASK_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_MASK_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_MASK_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_MASK_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_MASK_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_1_MASK_LOW_PCIE_6_SHIFT 28

/**** pcie_2_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_HIGH_RESERVED_31_8_SHIFT 8

/**** pcie_2_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_LOW_PCIE_6_SHIFT 28

/**** pcie_2_mask_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_MASK_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_MASK_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_MASK_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_MASK_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_MASK_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_MASK_HIGH_RESERVED_31_8_SHIFT 8

/**** pcie_2_mask_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_MASK_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_MASK_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_MASK_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_MASK_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_MASK_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_MASK_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_MASK_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_MASK_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_MASK_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_MASK_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_MASK_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_MASK_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_MASK_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_MASK_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_MASK_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_2_MASK_LOW_PCIE_6_SHIFT 28

/**** pcie_3_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_HIGH_RESERVED_31_8_SHIFT 8

/**** pcie_3_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_LOW_PCIE_6_SHIFT 28

/**** pcie_3_mask_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_MASK_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_MASK_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_MASK_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_MASK_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_MASK_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_MASK_HIGH_RESERVED_31_8_SHIFT 8

/**** pcie_3_mask_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_MASK_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_MASK_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_MASK_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_MASK_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_MASK_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_MASK_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_MASK_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_MASK_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_MASK_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_MASK_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_MASK_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_MASK_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_MASK_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_MASK_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_MASK_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_3_MASK_LOW_PCIE_6_SHIFT 28

/**** pcie_4_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_4_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_4_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_4_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_4_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_4_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_4_HIGH_RESERVED_31_8_SHIFT 8

/**** pcie_4_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_4_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_4_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_4_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_4_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_4_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_4_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_4_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_4_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_4_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_4_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_4_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_4_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_4_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_4_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_4_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_4_LOW_PCIE_6_SHIFT 28

/**** pcie_4_mask_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_4_MASK_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_4_MASK_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_4_MASK_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_4_MASK_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_4_MASK_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_4_MASK_HIGH_RESERVED_31_8_SHIFT 8

/**** pcie_4_mask_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_4_MASK_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_4_MASK_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_4_MASK_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_4_MASK_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_4_MASK_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_4_MASK_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_4_MASK_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_4_MASK_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_4_MASK_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_4_MASK_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_4_MASK_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_4_MASK_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_4_MASK_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_4_MASK_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_4_MASK_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_4_MASK_LOW_PCIE_6_SHIFT 28

/**** pcie_5_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_5_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_5_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_5_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_5_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_5_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_5_HIGH_RESERVED_31_8_SHIFT 8

/**** pcie_5_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_5_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_5_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_5_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_5_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_5_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_5_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_5_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_5_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_5_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_5_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_5_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_5_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_5_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_5_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_5_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_5_LOW_PCIE_6_SHIFT 28

/**** pcie_5_mask_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_5_MASK_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_5_MASK_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_5_MASK_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_5_MASK_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_5_MASK_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_5_MASK_HIGH_RESERVED_31_8_SHIFT 8

/**** pcie_5_mask_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_5_MASK_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_5_MASK_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_5_MASK_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_5_MASK_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_5_MASK_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_5_MASK_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_5_MASK_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_5_MASK_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_5_MASK_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_5_MASK_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_5_MASK_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_5_MASK_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_5_MASK_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_5_MASK_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_5_MASK_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_5_MASK_LOW_PCIE_6_SHIFT 28

/**** pcie_6_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_6_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_6_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_6_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_6_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_6_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_6_HIGH_RESERVED_31_8_SHIFT 8

/**** pcie_6_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_6_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_6_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_6_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_6_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_6_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_6_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_6_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_6_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_6_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_6_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_6_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_6_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_6_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_6_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_6_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_6_LOW_PCIE_6_SHIFT 28

/**** pcie_6_mask_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_6_MASK_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_6_MASK_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_6_MASK_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_6_MASK_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_6_MASK_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_6_MASK_HIGH_RESERVED_31_8_SHIFT 8

/**** pcie_6_mask_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_6_MASK_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_6_MASK_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_6_MASK_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_6_MASK_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_6_MASK_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_6_MASK_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_6_MASK_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_6_MASK_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_6_MASK_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_6_MASK_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_6_MASK_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_6_MASK_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_6_MASK_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_6_MASK_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_6_MASK_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_6_MASK_LOW_PCIE_6_SHIFT 28

/**** eth_0_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_HIGH_RESERVED_31_8_SHIFT 8

/**** eth_0_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_LOW_PCIE_6_SHIFT 28

/**** eth_0_mask_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_MASK_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_MASK_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_MASK_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_MASK_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_MASK_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_MASK_HIGH_RESERVED_31_8_SHIFT 8

/**** eth_0_mask_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_MASK_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_MASK_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_MASK_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_MASK_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_MASK_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_MASK_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_MASK_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_MASK_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_MASK_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_MASK_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_MASK_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_MASK_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_MASK_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_MASK_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_MASK_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_0_MASK_LOW_PCIE_6_SHIFT 28

/**** eth_1_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_HIGH_RESERVED_31_8_SHIFT 8

/**** eth_1_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_LOW_PCIE_6_SHIFT 28

/**** eth_1_mask_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_MASK_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_MASK_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_MASK_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_MASK_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_MASK_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_MASK_HIGH_RESERVED_31_8_SHIFT 8

/**** eth_1_mask_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_MASK_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_MASK_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_MASK_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_MASK_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_MASK_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_MASK_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_MASK_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_MASK_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_MASK_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_MASK_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_MASK_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_MASK_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_MASK_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_MASK_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_MASK_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_1_MASK_LOW_PCIE_6_SHIFT 28

/**** sata_6_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_6_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_SATA_6_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_6_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_SATA_6_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_6_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_SATA_6_HIGH_RESERVED_31_8_SHIFT 8

/**** sata_6_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_6_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_SATA_6_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_6_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_SATA_6_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_6_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_SATA_6_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_6_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_6_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_6_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_6_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_6_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_6_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_6_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_6_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_6_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_6_LOW_PCIE_6_SHIFT 28

/**** sata_6_mask_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_6_MASK_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_SATA_6_MASK_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_6_MASK_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_SATA_6_MASK_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_6_MASK_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_SATA_6_MASK_HIGH_RESERVED_31_8_SHIFT 8

/**** sata_6_mask_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_6_MASK_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_SATA_6_MASK_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_6_MASK_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_SATA_6_MASK_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_6_MASK_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_SATA_6_MASK_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_6_MASK_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_6_MASK_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_6_MASK_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_6_MASK_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_6_MASK_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_6_MASK_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_6_MASK_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_6_MASK_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_6_MASK_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_6_MASK_LOW_PCIE_6_SHIFT 28

/**** sata_7_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_7_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_SATA_7_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_7_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_SATA_7_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_7_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_SATA_7_HIGH_RESERVED_31_8_SHIFT 8

/**** sata_7_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_7_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_SATA_7_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_7_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_SATA_7_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_7_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_SATA_7_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_7_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_7_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_7_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_7_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_7_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_7_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_7_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_7_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_7_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_7_LOW_PCIE_6_SHIFT 28

/**** sata_7_mask_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_7_MASK_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_SATA_7_MASK_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_7_MASK_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_SATA_7_MASK_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_7_MASK_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_SATA_7_MASK_HIGH_RESERVED_31_8_SHIFT 8

/**** sata_7_mask_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_7_MASK_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_SATA_7_MASK_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_7_MASK_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_SATA_7_MASK_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_7_MASK_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_SATA_7_MASK_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_7_MASK_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_7_MASK_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_7_MASK_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_7_MASK_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_7_MASK_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_7_MASK_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_7_MASK_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_7_MASK_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_SATA_7_MASK_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_SATA_7_MASK_LOW_PCIE_6_SHIFT 28

/**** eth_db_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_DB_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_ETH_DB_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_DB_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_ETH_DB_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_DB_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_ETH_DB_HIGH_RESERVED_31_8_SHIFT 8

/**** eth_db_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_DB_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_ETH_DB_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_DB_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_ETH_DB_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_DB_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_ETH_DB_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_DB_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_DB_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_DB_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_DB_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_DB_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_DB_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_DB_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_DB_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_DB_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_DB_LOW_PCIE_6_SHIFT 28

/**** eth_db_mask_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_DB_MASK_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_ETH_DB_MASK_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_DB_MASK_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_ETH_DB_MASK_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_DB_MASK_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_ETH_DB_MASK_HIGH_RESERVED_31_8_SHIFT 8

/**** eth_db_mask_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_DB_MASK_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_ETH_DB_MASK_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_DB_MASK_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_ETH_DB_MASK_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_DB_MASK_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_ETH_DB_MASK_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_DB_MASK_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_DB_MASK_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_DB_MASK_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_DB_MASK_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_DB_MASK_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_DB_MASK_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_DB_MASK_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_DB_MASK_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_ETH_DB_MASK_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_ETH_DB_MASK_LOW_PCIE_6_SHIFT 28

/**** pcie_7_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_7_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_7_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_7_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_7_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_7_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_7_HIGH_RESERVED_31_8_SHIFT 8

/**** pcie_7_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_7_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_7_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_7_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_7_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_7_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_7_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_7_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_7_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_7_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_7_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_7_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_7_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_7_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_7_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_7_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_7_LOW_PCIE_6_SHIFT 28

/**** pcie_7_mask_high register ****/
/* map transactions to pcie_7 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_7_MASK_HIGH_PCIE_7_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_7_MASK_HIGH_PCIE_7_SHIFT 0
/* map transactions to dram */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_7_MASK_HIGH_DRAM_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_7_MASK_HIGH_DRAM_SHIFT 4
/* Reserved */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_7_MASK_HIGH_RESERVED_31_8_MASK 0xFFFFFF00
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_7_MASK_HIGH_RESERVED_31_8_SHIFT 8

/**** pcie_7_mask_low register ****/
/* map transactions according to address decoding */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_7_MASK_LOW_NO_ENFORCEMENT_MASK 0x0000000F
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_7_MASK_LOW_NO_ENFORCEMENT_SHIFT 0
/* map transactions to pcie_0 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_7_MASK_LOW_PCIE_0_MASK 0x000000F0
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_7_MASK_LOW_PCIE_0_SHIFT 4
/* map transactions to pcie_1 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_7_MASK_LOW_PCIE_1_MASK 0x00000F00
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_7_MASK_LOW_PCIE_1_SHIFT 8
/* map transactions to pcie_2 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_7_MASK_LOW_PCIE_2_MASK 0x0000F000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_7_MASK_LOW_PCIE_2_SHIFT 12
/* map transactions to pcie_3 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_7_MASK_LOW_PCIE_3_MASK 0x000F0000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_7_MASK_LOW_PCIE_3_SHIFT 16
/* map transactions to pcie_4 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_7_MASK_LOW_PCIE_4_MASK 0x00F00000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_7_MASK_LOW_PCIE_4_SHIFT 20
/* map transactions to pcie_5 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_7_MASK_LOW_PCIE_5_MASK 0x0F000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_7_MASK_LOW_PCIE_5_SHIFT 24
/* map transactions to pcie_6 */
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_7_MASK_LOW_PCIE_6_MASK 0xF0000000
#define PBS_TARGET_ID_ENFORCEMENT_PCIE_7_MASK_LOW_PCIE_6_SHIFT 28

/**** latch register ****/
/* Software clears this bit before any bar update, and set it after all bars updated. */
#define PBS_TARGET_ID_ENFORCEMENT_LATCH_ENABLE (1 << 0)

/**** spi_debug_pasw_high register ****/
/* Window base address [47:32] */
#define PBS_SPI_DEBUG_PASW_SPI_DEBUG_PASW_HIGH_BASE_HIGH_MASK 0x0000FFFF
#define PBS_SPI_DEBUG_PASW_SPI_DEBUG_PASW_HIGH_BASE_HIGH_SHIFT 0
/* Reserved */
#define PBS_SPI_DEBUG_PASW_SPI_DEBUG_PASW_HIGH_RSRVD_MASK 0x00FF0000
#define PBS_SPI_DEBUG_PASW_SPI_DEBUG_PASW_HIGH_RSRVD_SHIFT 16
/*
 * Minimum size is 4B. Maximum size is 4GB
 * 0: Disable and ignore the window
 * 1: Window size = 4 byte
 * 2: Window size = 8 byte
 * S: Window size = 2^(S+1) bytes
 * When the TZPASW determines the window within DRAM, its minimum size is
 * 1MB (S >18).
 */
#define PBS_SPI_DEBUG_PASW_SPI_DEBUG_PASW_HIGH_WINDOW_SIZE_MASK 0x3F000000
#define PBS_SPI_DEBUG_PASW_SPI_DEBUG_PASW_HIGH_WINDOW_SIZE_SHIFT 24
/* Reserved */
#define PBS_SPI_DEBUG_PASW_SPI_DEBUG_PASW_HIGH_SECURITY_MODE_MASK 0xC0000000
#define PBS_SPI_DEBUG_PASW_SPI_DEBUG_PASW_HIGH_SECURITY_MODE_SHIFT 30

/**** spi_debug_pasw_low register ****/
/* Reserved */
#define PBS_SPI_DEBUG_PASW_SPI_DEBUG_PASW_LOW_RSRVD_MASK 0x00000003
#define PBS_SPI_DEBUG_PASW_SPI_DEBUG_PASW_LOW_RSRVD_SHIFT 0
/*
 * Window base address [31:2]
 * Only the bits above the TZPASW size are checked, rest are masked.
 */
#define PBS_SPI_DEBUG_PASW_SPI_DEBUG_PASW_LOW_BASE_LOW_MASK 0xFFFFFFFC
#define PBS_SPI_DEBUG_PASW_SPI_DEBUG_PASW_LOW_BASE_LOW_SHIFT 2

/**** spi_debug_pasw_enable_reg register ****/
/*
 * When SPI Debug PASW enable bit is set, access to/from SPI debugger is restricted to
 * SPI_DEBUG_PASW windows
 * This bit is set only
 */
#define PBS_SPI_DEBUG_PASW_ENABLE_SPI_DEBUG_PASW_ENABLE_REG_PASW_ENABLE (1 << 0)
/* Reserved */
#define PBS_SPI_DEBUG_PASW_ENABLE_SPI_DEBUG_PASW_ENABLE_REG_RESERVED_MASK 0xFFFFFFFE
#define PBS_SPI_DEBUG_PASW_ENABLE_SPI_DEBUG_PASW_ENABLE_REG_RESERVED_SHIFT 1

/**** spi_debug_pasw_write_protect_reg register ****/
/*
 * When SPI debug PASW write protect is set, the SPI_PASW windows are locked and cannot be changed.
 * This bit is set only
 */
#define PBS_SPI_DEBUG_PASW_ENABLE_SPI_DEBUG_PASW_WRITE_PROTECT_REG_PASW_WRITE_PROTECT (1 << 0)
/* Reserved */
#define PBS_SPI_DEBUG_PASW_ENABLE_SPI_DEBUG_PASW_WRITE_PROTECT_REG_RESERVED_MASK 0xFFFFFFFE
#define PBS_SPI_DEBUG_PASW_ENABLE_SPI_DEBUG_PASW_WRITE_PROTECT_REG_RESERVED_SHIFT 1

/**** spi_debug_pasw_read_protect_reg register ****/
/*
 * When SPI debug PASW write protect is set, the SPI_PASW windows cannot be read
 * This bit is set only
 */
#define PBS_SPI_DEBUG_PASW_ENABLE_SPI_DEBUG_PASW_READ_PROTECT_REG_PASW_READ_PROTECT (1 << 0)
/* Reserved */
#define PBS_SPI_DEBUG_PASW_ENABLE_SPI_DEBUG_PASW_READ_PROTECT_REG_RESERVED_MASK 0xFFFFFFFE
#define PBS_SPI_DEBUG_PASW_ENABLE_SPI_DEBUG_PASW_READ_PROTECT_REG_RESERVED_SHIFT 1

#ifdef __cplusplus
}
#endif

#endif

/** @} */


