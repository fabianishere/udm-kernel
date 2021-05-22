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
 * @addtogroup group_muio_mux
 *
 *  @{
 * @file   al_hal_muio_mux_map.h
 *
 * @brief  MUIO mux HAL driver - mapping structures
 *
 */

#include "al_hal_muio_mux.h"
#include "al_hal_pbs_regs.h"

enum al_muio_mux_func {
	AL_MUIO_MUX_FUNC_0 = 0,
	AL_MUIO_MUX_FUNC_1 = 1,
	AL_MUIO_MUX_FUNC_2 = 2,
	AL_MUIO_MUX_FUNC_3 = 3,
	AL_MUIO_MUX_FUNC_4 = 4
};

#define AL_MUIO_MUX_FUNC_GPIO	AL_MUIO_MUX_FUNC_0
#define AL_MUIO_MUX_CHIP_VER_MIN 0
#define AL_MUIO_MUX_CHIP_VER_MAX 0xffffffff

#define AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(iface, mux_func, pins)\
	{\
		(iface),\
		(mux_func),\
		(sizeof(pins) / sizeof((pins)[0])),\
		(pins),\
		(AL_MUIO_MUX_CHIP_VER_MIN), \
		(AL_MUIO_MUX_CHIP_VER_MAX),\
	}
#define AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT_EX(iface, mux_func, pins, min_ver, max_ver)\
	{\
		(iface),\
		(mux_func),\
		(sizeof(pins) / sizeof((pins)[0])),\
		(pins),\
		(min_ver),\
		(max_ver),\
	}

struct al_muio_mux_iface_pins {
	enum al_muio_mux_if	iface;
	int			mux_func;
	int			num_pins;
	const int	*pins;
	unsigned int chip_version_min;
	unsigned int chip_version_max;
};

static const int al_muio_mux_iface_pins_nor_8[] = {
		6, 7, 8, 9, 10, 11, 12, 13,	/* DATA[0:7] */
		15,				/* ALE */
		16,				/* OE_N */
		17,				/* WE_N */
		18,				/* RB_N[0] */
		32, 33, 34, 35,			/* ADDR[24:27] */
		36,				/* ADDR[0] */
		37, 38, 39, 40, 41, 42, 43,	/* ADDR[1:7] */
	};

static const int al_muio_mux_iface_pins_nor_16[] = {
		6, 7, 8, 9, 10, 11, 12, 13,	/* DATA[0:7] */
		14,				/* WBE_N[0] */
		15,				/* ALE */
		16,				/* RE_N */
		17,				/* WE_N */
		18,				/* RB_N[0] */
		24, 25, 26, 27, 28, 29, 30, 31,	/* DATA[8:15] */
		32, 33, 34, 35,			/* ADDR[24:27] */
		36,				/* WBE_N[1] */
		37, 38, 39, 40, 41, 42, 43,	/* ADDR[1:7] */
	};

static const int al_muio_mux_iface_pins_nor_cs0[] = {
		19,				/* CS_N[0] */
	};

static const int al_muio_mux_iface_pins_nor_cs1[] = {
		20,				/* CS_N[1] */
	};

static const int al_muio_mux_iface_pins_nor_cs2[] = {
		21,				/* CS_N[2] */
	};

static const int al_muio_mux_iface_pins_nor_cs3[] = {
		22,				/* CS_N[3] */
	};

static const int al_muio_mux_iface_pins_nor_wp[] = {
		23,				/* WP_N[0] */
	};

static const int al_muio_mux_iface_pins_nand_8[] = {
		13, 12, 11, 10, 9, 8, 7, 6,	/* NAND_DATA[7:0] */
		14,				/* NAND_CLE */
		15,				/* NAND_ALE */
		16,				/* NAND_RE_N */
		17,				/* NAND_WE_N */
		18,				/* NAND_RB_N[0] */
	};

static const int al_muio_mux_iface_pins_nand_16[] = {
		13, 12, 11, 10, 9, 8, 7, 6,	/* NAND_DATA[7:0] */
		14,				/* NAND_CLE */
		15,				/* NAND_ALE */
		16,				/* NAND_RE_N */
		17,				/* NAND_WE_N */
		18,				/* NAND_RB_N[0] */
		31, 30, 29, 28, 27, 26, 25, 24,	/* NAND_DATA[15:8] */
	};

static const int al_muio_mux_iface_pins_nand_cs0[] = {
		19,				/* CS_N[0] */
	};

static const int al_muio_mux_iface_pins_nand_cs1[] = {
		20,				/* CS_N[1] */
	};

static const int al_muio_mux_iface_pins_nand_cs2[] = {
		21,				/* CS_N[2] */
	};

static const int al_muio_mux_iface_pins_nand_cs3[] = {
		22,				/* CS_N[3] */
	};

static const int al_muio_mux_iface_pins_nand_wp[] = {
		23,				/* WP_N[0] */
	};

static const int al_muio_mux_iface_pins_sram_8[] = {
		6, 7, 8, 9, 10, 11, 12, 13,	/* DATA[0:7] */
		15,				/* ALE */
		16,				/* OE_N */
		17,				/* RW */
		23,				/* BLAST_N */
		32, 33, 34, 35,			/* ADDR[24:27] */
		36,				/* ADDR[0] */
		37, 38, 39, 40, 41, 42, 43,	/* ADDR[1:7] */
	};

static const int al_muio_mux_iface_pins_sram_16[] = {
		6, 7, 8, 9, 10, 11, 12, 13,	/* DATA[0:7] */
		14,				/* WBE_N[0] */
		15,				/* ALE */
		16,				/* OE_N */
		17,				/* RW */
		23,				/* BLAST_N */
		24, 25, 26, 27, 28, 29, 30, 31,	/* DATA[8:15] */
		32, 33, 34, 35,			/* ADDR[24:27] */
		36,				/* WBE_N[1] */
		37, 38, 39, 40, 41, 42, 43,	/* ADDR[1:7] */
	};

static const int al_muio_mux_iface_pins_sram_cs0[] = {
		19,				/* CS_N[0] */
	};

static const int al_muio_mux_iface_pins_sram_cs1[] = {
		20,				/* CS_N[1] */
	};

static const int al_muio_mux_iface_pins_sram_cs2[] = {
		21,				/* CS_N[2] */
	};

static const int al_muio_mux_iface_pins_sram_cs3[] = {
		22,				/* CS_N[3] */
	};

static const int al_muio_mux_iface_pins_sata_0_leds[] = {
		20, 21, 22, 23,			/* LED_SATA_0[0:3]*/
	};

static const int al_muio_mux_iface_pins_sata_1_leds[] = {
		24, 25, 26, 27,			/* LED_SATA_1[0:3]*/
	};

static const int al_muio_mux_iface_pins_eth_0_led[] = {
		28,				/* LED_ETH[0]*/
	};

static const int al_muio_mux_iface_pins_eth_1_led[] = {
		29,				/* LED_ETH[1]*/
	};

static const int al_muio_mux_iface_pins_eth_2_led[] = {
		30,				/* LED_ETH[2]*/
	};

static const int al_muio_mux_iface_pins_eth_3_led[] = {
		31,				/* LED_ETH[3]*/
	};

static const int al_muio_mux_iface_pins_eth_leds[] = {
		28, 29, 30, 31,			/* LED_ETH[0:3]*/
	};

static const int al_muio_mux_iface_pins_eth_gpio[] = {
		32,				/* eth_ec_gpio_pause_on_in */
		33,				/* eth_ec_gpio_pause_on_in */
		34,				/* eth_ec_gpio_pause_on_in */
		35,				/* eth_ec_gpio_pause_on_in */
		36,				/* eeth_ec_gpio_pause_on_in */
		37,				/* eeth_ec_gpio_pause_on_in */
		38,				/* eeth_ec_gpio_pause_on_in */
		39,				/* eeth_ec_gpio_pause_on_in */
		40,				/* eeth_ec_gpio_xoff_out */
		41,				/* eeth_mac_gpio_int_in */
		42,				/* eeth_mac_gpio_los_in */
		43,				/* eeth_ec_gpio_int_in */
	};

static const int al_muio_mux_iface_pins_uart_1[] = {
		20,				/* UART1_SIN */
		21,				/* UART1_SOUT */
	};

static const int al_muio_mux_iface_pins_uart_1_modem[] = {
		20,				/* UART1_SIN */
		21,				/* UART1_SOUT */
		22,				/* UART1_SIR_IN */
		23,				/* UART1_SIR_OUT_n */
		24,				/* UART1_CTS_n */
		25,				/* UART1_RTS_n */
	};

static const int al_muio_mux_iface_pins_uart_2[] = {
		26,				/* UART2_SIN */
		27,				/* UART2_SOUT */
};

static const int al_muio_mux_iface_pins_uart_3[] = {
		28,				/* UART3_SIN */
		29,				/* UART3_SOUT */
	};

static const int al_muio_mux_iface_pins_i2c_gen[] = {
		30,				/* I2C_B_SCL */
		31,				/* I2C_B_SDA */
	};

static const int al_muio_mux_iface_pins_ulpi_0_rst[] = {
		3,				/* ULPI0_reset_n */
	};

static const int al_muio_mux_iface_pins_ulpi_1_rst[] = {
		4,				/* ULPI1_reset_n */
	};

static const int al_muio_mux_iface_pins_pci_ep_int_a[] = {
		5,				/* PCI_EP_IntA */
	};

static const int al_muio_mux_iface_pins_pci_ep_reset_out[] = {
		2,				/* PCI_EP_RESET_OUT */
	};

static const int al_muio_mux_iface_pins_spim_a_ss_1[] = {
		0,				/* SPIM_A_SS_1 */
	};

static const int al_muio_mux_iface_pins_spim_a_ss_2[] = {
		1,				/* SPIM_A_SS_2 */
	};

static const int al_muio_mux_iface_pins_spim_a_ss_3[] = {
		2,				/* SPIM_A_SS_3 */
	};

static const int al_muio_mux_iface_pins_ulpi_1_b[] = {
		32,				/* ULPI1_B_CLK */
		33,				/* ULPI1_B_DIR */
		34,				/* ULPI1_B_NEXT */
		35,				/* ULPI1_B_DATA[0] */
		36,				/* ULPI1_B_DATA[1] */
		37,				/* ULPI1_B_DATA[2] */
		38,				/* ULPI1_B_DATA[3] */
		39,				/* ULPI1_B_DATA[4] */
		40,				/* ULPI1_B_DATA[5] */
		41,				/* ULPI1_B_DATA[6] */
		42,				/* ULPI1_B_DATA[7] */
		43,				/* ULPI1_B_STP */
	};

static const int al_muio_mux_iface_pins_tdm[] = {
		26,				/* PCM_CLK */
		27,				/* PCM_FRAME_SINC */
		28,				/* PCM_DATA_IN */
		29,				/* PCM_DATA_OUT */
		30,				/* TDM_EXT_REF */
		35,				/* PCM_Tx_Channel_en[0] */
		36,				/* PCM_Tx_Channel_en[1] */
		37,				/* PCM_Rx_Channel_en[0] */
		38,				/* PCM_Rx_Channel_en[1] */
	};
static const int al_muio_mux_iface_pins_spim_aux[] = {
		31,				/* SPIM_B_TX */
		32,				/* SPIM_B_RX */
		33,				/* SPIM_B_CLK */
		34,				/* SPIM_B_CS */
	};
static const int al_muio_mux_iface_pins_spim_aux_v3[] = {
		39,				/* SPIM_B_TX */
		32,				/* SPIM_B_RX */
		33,				/* SPIM_B_CLK */
		34,				/* SPIM_B_CS */
	};

static const int al_muio_mux_iface_pins_spis_qpi[] = {
		45,				/* SPIS_IO2 */
		46,				/* SPIS_IO3 */
	};

static const int al_muio_mux_iface_pins_sgpo_clk[] = {
		24,				/* Serial_SHCP  */
		25				/* Serial_STCP  */
	};

static const int al_muio_mux_iface_pins_sgpo_ds0[] = {
		23,				/* Serial_DS[0] */
	};

static const int al_muio_mux_iface_pins_sgpo_ds1[] = {
		22,				/* Serial_DS[1] */
	};

static const int al_muio_mux_iface_pins_sgpo_ds2[] = {
		21,				/* Serial_DS[2] */
	};

static const int al_muio_mux_iface_pins_sgpo_ds3[] = {
		20,				/* Serial_DS[3] */
	};

static const int al_muio_mux_iface_pins_ec_ptp_trig_in_0[] = {
		26,				/* ec_ptp_trigger_in[0]  */
	};

static const int al_muio_mux_iface_pins_ec_ptp_trig_in_1[] = {
		27,				/* ec_ptp_trigger_in[1]  */
	};

static const int al_muio_mux_iface_pins_ec_ptp_trig_out_0[] = {
		28,				/* ec_ptp_trigger_out[0] */
	};

static const int al_muio_mux_iface_pins_ec_ptp_trig_out_1[] = {
		29				/* ec_ptp_trigger_out[1] */
	};

static const int al_muio_mux_iface_pins_cpu_intr_0[] = {
		3,				/* CPU Interrupt Out[0]  */
	};

static const int al_muio_mux_iface_pins_cpu_intr_1[] = {
		4				/* CPU Interrupt Out[1]  */
	};

static const int al_muio_mux_iface_pins_rgmii_b[] = {
		44, 45, 46, 47, /* RGMII_B_RXD [0:3] */
		48, /* RGMII_B_RXC */
		49, /* RGMII_B_RX_CTL */
		50, 51, 52, 53, /* RGMII_B_TXD [0:3] */
		54, /* RGMII_B_TXC */
		55, /* RGMII_B_TX_CTL */
	};

static const int al_muio_mux_iface_pins_pmdt[] = {
		41				/* PMDT CCTM GPIO OUT */
	};

static const struct al_muio_mux_iface_pins _al_muio_mux_iface_pins[] = {
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_NOR_8,
			AL_MUIO_MUX_FUNC_3,
			al_muio_mux_iface_pins_nor_8),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_NOR_16,
			AL_MUIO_MUX_FUNC_3,
			al_muio_mux_iface_pins_nor_16),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_NOR_CS_0,
			AL_MUIO_MUX_FUNC_3,
			al_muio_mux_iface_pins_nor_cs0),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_NOR_CS_1,
			AL_MUIO_MUX_FUNC_3,
			al_muio_mux_iface_pins_nor_cs1),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_NOR_CS_2,
			AL_MUIO_MUX_FUNC_3,
			al_muio_mux_iface_pins_nor_cs2),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_NOR_CS_3,
			AL_MUIO_MUX_FUNC_3,
			al_muio_mux_iface_pins_nor_cs3),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_NOR_WP,
			AL_MUIO_MUX_FUNC_3,
			al_muio_mux_iface_pins_nor_wp),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_NAND_8,
			AL_MUIO_MUX_FUNC_3,
			al_muio_mux_iface_pins_nand_8),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_NAND_16,
			AL_MUIO_MUX_FUNC_3,
			al_muio_mux_iface_pins_nand_16),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_NAND_CS_0,
			AL_MUIO_MUX_FUNC_3,
			al_muio_mux_iface_pins_nand_cs0),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_NAND_CS_1,
			AL_MUIO_MUX_FUNC_3,
			al_muio_mux_iface_pins_nand_cs1),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_NAND_CS_2,
			AL_MUIO_MUX_FUNC_3,
			al_muio_mux_iface_pins_nand_cs2),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_NAND_CS_3,
			AL_MUIO_MUX_FUNC_3,
			al_muio_mux_iface_pins_nand_cs3),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_NAND_WP,
			AL_MUIO_MUX_FUNC_3,
			al_muio_mux_iface_pins_nand_wp),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_SRAM_8,
			AL_MUIO_MUX_FUNC_3,
			al_muio_mux_iface_pins_sram_8),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_SRAM_16,
			AL_MUIO_MUX_FUNC_3,
			al_muio_mux_iface_pins_sram_16),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_SRAM_CS_0,
			AL_MUIO_MUX_FUNC_3,
			al_muio_mux_iface_pins_sram_cs0),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_SRAM_CS_1,
			AL_MUIO_MUX_FUNC_3,
			al_muio_mux_iface_pins_sram_cs1),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_SRAM_CS_2,
			AL_MUIO_MUX_FUNC_3,
			al_muio_mux_iface_pins_sram_cs2),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_SRAM_CS_3,
			AL_MUIO_MUX_FUNC_3,
			al_muio_mux_iface_pins_sram_cs3),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_SATA_0_LEDS,
			AL_MUIO_MUX_FUNC_1,
			al_muio_mux_iface_pins_sata_0_leds),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_SATA_1_LEDS,
			AL_MUIO_MUX_FUNC_1,
			al_muio_mux_iface_pins_sata_1_leds),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_ETH_0_LED,
			AL_MUIO_MUX_FUNC_1,
			al_muio_mux_iface_pins_eth_0_led),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_ETH_1_LED,
			AL_MUIO_MUX_FUNC_1,
			al_muio_mux_iface_pins_eth_1_led),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_ETH_2_LED,
			AL_MUIO_MUX_FUNC_1,
			al_muio_mux_iface_pins_eth_2_led),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_ETH_3_LED,
			AL_MUIO_MUX_FUNC_1,
			al_muio_mux_iface_pins_eth_3_led),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_ETH_LEDS,
			AL_MUIO_MUX_FUNC_1,
			al_muio_mux_iface_pins_eth_leds),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_ETH_GPIO,
			AL_MUIO_MUX_FUNC_1,
			al_muio_mux_iface_pins_eth_gpio),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_UART_1,
			AL_MUIO_MUX_FUNC_2,
			al_muio_mux_iface_pins_uart_1),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_UART_1_MODEM,
			AL_MUIO_MUX_FUNC_2,
			al_muio_mux_iface_pins_uart_1_modem),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_UART_2,
			AL_MUIO_MUX_FUNC_2,
			al_muio_mux_iface_pins_uart_2),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_UART_3,
			AL_MUIO_MUX_FUNC_2,
			al_muio_mux_iface_pins_uart_3),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_I2C_GEN,
			AL_MUIO_MUX_FUNC_2,
			al_muio_mux_iface_pins_i2c_gen),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_ULPI_0_RST_N,
			AL_MUIO_MUX_FUNC_1,
			al_muio_mux_iface_pins_ulpi_0_rst),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_ULPI_1_RST_N,
			AL_MUIO_MUX_FUNC_1,
			al_muio_mux_iface_pins_ulpi_1_rst),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_PCI_EP_INT_A,
			AL_MUIO_MUX_FUNC_1,
			al_muio_mux_iface_pins_pci_ep_int_a),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_PCI_EP_RESET_OUT,
			AL_MUIO_MUX_FUNC_1,
			al_muio_mux_iface_pins_pci_ep_reset_out),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_SPIM_A_SS_1,
			AL_MUIO_MUX_FUNC_2,
			al_muio_mux_iface_pins_spim_a_ss_1),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_SPIM_A_SS_2,
			AL_MUIO_MUX_FUNC_2,
			al_muio_mux_iface_pins_spim_a_ss_2),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_SPIM_A_SS_3,
			AL_MUIO_MUX_FUNC_2,
			al_muio_mux_iface_pins_spim_a_ss_3),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_ULPI_1_B,
			AL_MUIO_MUX_FUNC_2,
			al_muio_mux_iface_pins_ulpi_1_b),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
				AL_MUIO_MUX_IF_TDM,
				AL_MUIO_MUX_FUNC_3,
				al_muio_mux_iface_pins_tdm),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT_EX(
			AL_MUIO_MUX_IF_SPIM_AUX,
			AL_MUIO_MUX_FUNC_3,
			al_muio_mux_iface_pins_spim_aux,
			PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V1,
			PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V2),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT_EX(
			AL_MUIO_MUX_IF_SPIM_AUX,
			AL_MUIO_MUX_FUNC_3,
			al_muio_mux_iface_pins_spim_aux_v3,
			PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V3,
			AL_MUIO_MUX_CHIP_VER_MAX),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
				AL_MUIO_MUX_IF_SPIS_QPI,
				AL_MUIO_MUX_FUNC_3,
				al_muio_mux_iface_pins_spis_qpi),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_SGPO_CLK,
			AL_MUIO_MUX_FUNC_4,
			al_muio_mux_iface_pins_sgpo_clk),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_SGPO_DS_0,
			AL_MUIO_MUX_FUNC_4,
			al_muio_mux_iface_pins_sgpo_ds0),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_SGPO_DS_1,
			AL_MUIO_MUX_FUNC_4,
			al_muio_mux_iface_pins_sgpo_ds1),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_SGPO_DS_2,
			AL_MUIO_MUX_FUNC_4,
			al_muio_mux_iface_pins_sgpo_ds2),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_SGPO_DS_3,
			AL_MUIO_MUX_FUNC_4,
			al_muio_mux_iface_pins_sgpo_ds3),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_EC_PTP_TRIG_IN_0,
			AL_MUIO_MUX_FUNC_4,
			al_muio_mux_iface_pins_ec_ptp_trig_in_0),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_EC_PTP_TRIG_IN_1,
			AL_MUIO_MUX_FUNC_4,
			al_muio_mux_iface_pins_ec_ptp_trig_in_1),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_EC_PTP_TRIG_OUT_0,
			AL_MUIO_MUX_FUNC_4,
			al_muio_mux_iface_pins_ec_ptp_trig_out_0),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_EC_PTP_TRIG_OUT_1,
			AL_MUIO_MUX_FUNC_4,
			al_muio_mux_iface_pins_ec_ptp_trig_out_1),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_CPU_INTR_0,
			AL_MUIO_MUX_FUNC_2,
			al_muio_mux_iface_pins_cpu_intr_0),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_CPU_INTR_1,
			AL_MUIO_MUX_FUNC_2,
			al_muio_mux_iface_pins_cpu_intr_1),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_RGMII_B,
			AL_MUIO_MUX_FUNC_1,
			al_muio_mux_iface_pins_rgmii_b),
		AL_MUIO_MUX_IFACE_PINS_STRUCT_ENT(
			AL_MUIO_MUX_IF_PMDT,
			AL_MUIO_MUX_FUNC_3,
			al_muio_mux_iface_pins_pmdt),
	};

#define AL_MUIO_MUX_NUM_IFACES	(int)(\
	sizeof(_al_muio_mux_iface_pins)/sizeof(_al_muio_mux_iface_pins[0]))

/** @} end of MUIO mux group */

