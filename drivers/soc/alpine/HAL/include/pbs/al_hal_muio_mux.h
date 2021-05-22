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
 * @defgroup group_muio_mux MUIO Multiplexing
 * @ingroup group_pbs
 *
 * Some of the chip pins have more than one functionality.
 * This abstraction layer provides services for selecting the functionality of
 * the chip pins.
 *
 * Common operation exmaple:
 * @code
 *	int main()
 *	{
 *		struct al_muio_mux_obj obj;
 *		int al_status;
 *
 *		struct al_muio_mux_if_and_arg ifaces[] = {
 *			{ AL_MUIO_MUX_IF_GPIO, 7 },
 *			{ AL_MUIO_MUX_IF_GPIO, 19 },
 *			{ AL_MUIO_MUX_IF_GPIO, 17 },
 *			{ AL_MUIO_MUX_IF_SATA_0_LEDS, 0 },
 *			{ AL_MUIO_MUX_IF_SATA_1_LEDS, 0 },
 *			{ AL_MUIO_MUX_IF_ETH_LEDS, 0 },
 *			{ AL_MUIO_MUX_IF_UART_1, 0 },
 *			{ AL_MUIO_MUX_IF_UART_2, 0 },
 *			{ AL_MUIO_MUX_IF_UART_3, 0 },
 *			{ AL_MUIO_MUX_IF_I2C_GEN, 0 },
 *		};
 *
 *		al_status = al_muio_mux_init(
 *			(void __iomem *)SLR_PBS_REG_BASE,
 *			"muio_mux",
 *			&obj);
 *
 *		if (al_status != 0) {
 *			printf("%al_muio_mux_init failed!\n");
 *			return -1;
 *		}
 *
 *		al_status = al_muio_mux_iface_alloc_multi(
 *			&obj,
 *			ifaces,
 *			sizeof(ifaces) / sizeof(ifaces[0]));
 *
 *		if (al_status != 0) {
 *			printf("al_muio_mux_iface_alloc_multi failed!\n");
 *			return -1;
 *		}
 *
 *		return 0;
 *	}
 * @endcode
 * @file   al_hal_muio_mux.h
 *
 * @brief Header file for the MUIO mux HAL driver
 *
 */

#ifndef __AL_HAL_MUIO_MUX__
#define __AL_HAL_MUIO_MUX__

#include "al_hal_common.h"

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

#define AL_MUIO_MUX_NUM_MUIO			64

enum al_muio_mux_if {
	AL_MUIO_MUX_IF_NONE,
	AL_MUIO_MUX_IF_NOR_8,
	AL_MUIO_MUX_IF_NOR_16,
	AL_MUIO_MUX_IF_NOR_CS_0,
	AL_MUIO_MUX_IF_NOR_CS_1,
	AL_MUIO_MUX_IF_NOR_CS_2,
	AL_MUIO_MUX_IF_NOR_CS_3,
	AL_MUIO_MUX_IF_NOR_WP,
	AL_MUIO_MUX_IF_NAND_8,
	AL_MUIO_MUX_IF_NAND_16,
	AL_MUIO_MUX_IF_NAND_CS_0,
	AL_MUIO_MUX_IF_NAND_CS_1,
	AL_MUIO_MUX_IF_NAND_CS_2,
	AL_MUIO_MUX_IF_NAND_CS_3,
	AL_MUIO_MUX_IF_NAND_WP,
	AL_MUIO_MUX_IF_SRAM_8,
	AL_MUIO_MUX_IF_SRAM_16,
	AL_MUIO_MUX_IF_SRAM_CS_0,
	AL_MUIO_MUX_IF_SRAM_CS_1,
	AL_MUIO_MUX_IF_SRAM_CS_2,
	AL_MUIO_MUX_IF_SRAM_CS_3,
	AL_MUIO_MUX_IF_SATA_0_LEDS,
	AL_MUIO_MUX_IF_SATA_1_LEDS,
	AL_MUIO_MUX_IF_ETH_0_LED,
	AL_MUIO_MUX_IF_ETH_1_LED,
	AL_MUIO_MUX_IF_ETH_2_LED,
	AL_MUIO_MUX_IF_ETH_3_LED,
	AL_MUIO_MUX_IF_ETH_LEDS,
	AL_MUIO_MUX_IF_ETH_GPIO,
	AL_MUIO_MUX_IF_UART_1,
	AL_MUIO_MUX_IF_UART_1_MODEM,
	AL_MUIO_MUX_IF_UART_2,
	AL_MUIO_MUX_IF_UART_3,
	AL_MUIO_MUX_IF_I2C_GEN,
	AL_MUIO_MUX_IF_ULPI_0_RST_N,
	AL_MUIO_MUX_IF_ULPI_1_RST_N,
	AL_MUIO_MUX_IF_PCI_EP_INT_A,
	AL_MUIO_MUX_IF_PCI_EP_RESET_OUT,
	AL_MUIO_MUX_IF_SPIM_A_SS_1,
	AL_MUIO_MUX_IF_SPIM_A_SS_2,
	AL_MUIO_MUX_IF_SPIM_A_SS_3,
	AL_MUIO_MUX_IF_SPIM_AUX,
	AL_MUIO_MUX_IF_SPIS_QPI,
	AL_MUIO_MUX_IF_TDM,
	AL_MUIO_MUX_IF_ULPI_1_B,
	AL_MUIO_MUX_IF_SGPO_CLK,
	AL_MUIO_MUX_IF_SGPO_DS_0,
	AL_MUIO_MUX_IF_SGPO_DS_1,
	AL_MUIO_MUX_IF_SGPO_DS_2,
	AL_MUIO_MUX_IF_SGPO_DS_3,
	AL_MUIO_MUX_IF_EC_PTP_TRIG_IN_0,
	AL_MUIO_MUX_IF_EC_PTP_TRIG_IN_1,
	AL_MUIO_MUX_IF_EC_PTP_TRIG_OUT_0,
	AL_MUIO_MUX_IF_EC_PTP_TRIG_OUT_1,
	AL_MUIO_MUX_IF_CPU_INTR_0,
	AL_MUIO_MUX_IF_CPU_INTR_1,
	AL_MUIO_MUX_IF_RGMII_B,
	AL_MUIO_MUX_IF_PMDT,

	/* Keep the GPIO interface last */
	AL_MUIO_MUX_IF_GPIO,
};

struct al_muio_mux_if_and_arg {
	enum al_muio_mux_if	iface;
	int			arg;
};

struct al_muio_mux_obj {
	struct al_pbs_regs	*regs_base;
	const char		*name;

	enum al_muio_mux_if	pins_iface[AL_MUIO_MUX_NUM_MUIO];

	struct dev_prop {
		int regs_num;
		uint8_t func_mask;
		unsigned int func_num_bits;
		unsigned int dev_id;
	} dev_prop;
};

/**
 * Initialization
 *
 * Initializes a MUIO mux singleton
 *
 * @param  regs_base
 *             The PBS register file base pointer
 *
 * @param  name
 *             A name for the object - should remain allocated until a call to
 *             'al_muio_mux_terminate'
 *
 * @param obj
 *             An allocated, non initialized object context
 *
 * @return 0 if no error found.
 *
 */
int al_muio_mux_init(
	void __iomem		*regs_base,
	const char		*name,
	struct al_muio_mux_obj	*obj);

/**
 * Termination
 *
 * Terminate the MUIO mux singleton
 *
 * @param  obj
 *             The object context
 *
 * No function besides 'al_muio_mux_init' can be called after calling this
 * function.
 *
 * @return 0 if no error found.
 *
 */
int al_muio_mux_terminate(
	struct al_muio_mux_obj	*obj);

/**
 * Interface allocation
 *
 * Allocates a MUIO interface.
 * Allocation of an interface which uses pins which are currently used by
 * another allocated interface will fail.
 *
 * @param  obj
 *             The object context
 *
 * @param  iface
 *             The required interface
 *
 * @param  arg
 *             The required interface argument:
 *             - For AL_MUIO_MUX_IF_GPIO - the required GPIO (0 - 43)
 *             - Otherwise - 0
 *
 * @return 0 if no error found.
 *
 */
int al_muio_mux_iface_alloc(
	struct al_muio_mux_obj	*obj,
	enum al_muio_mux_if	iface,
	int			arg);

/**
 * Multiple Interface allocation
 *
 * Allocates multiple MUIO interfaces.
 * Allocation of an interface which uses pins which are currently used by
 * another allocated interface will fail.
 *
 * @param  obj
 *             The object context
 *
 * @param  ifaces_and_args
 *             The required interfaces and arguments:
 *             - For AL_MUIO_MUX_IF_GPIO - the required GPIO (0 - 43)
 *             - Otherwise - 0
 *
 * @param  ifaces_cnt
 *             The num of required interfaces
 *
 * @return 0 if no error found.
 *
 */
int al_muio_mux_iface_alloc_multi(
	struct al_muio_mux_obj			*obj,
	const struct al_muio_mux_if_and_arg	*ifaces_and_args,
	int					ifaces_cnt);

/**
 * Interface deallocation
 *
 * Frees a previsouly allocated MUIO interface.
 *
 * @param  obj
 *             The object context
 *
 * @param  iface
 *             The interface to be freed
 *
 * @param  arg
 *             The argument of the interface to be freed:
 *             - For AL_MUIO_MUX_IF_GPIO - the required GPIO (0 - 43)
 *             - Otherwise - 0
 *
 * @return 0 if no error found.
 *
 */
int al_muio_mux_iface_free(
	struct al_muio_mux_obj	*obj,
	enum al_muio_mux_if	iface,
	int			arg);

/**
 * Interface forced allocation
 *
 * Forcely allocates a MUIO interface - for debug purposes only.
 * Allocation of an interface which uses pins which are currently used by
 * another allocated interface will not fail and thus previously allocated
 * interfaces will be partially allocated.
 *
 * @param  obj
 *             The object context
 *
 * @param  iface
 *             The required interface
 *
 * @param  arg
 *             The required interface argument:
 *             - For AL_MUIO_MUX_IF_GPIO - the required GPIO (0 - 43)
 *             - Otherwise - 0
 *
 * @return 0 if no error found.
 *
 */
int al_muio_mux_iface_alloc_force(
	struct al_muio_mux_obj	*obj,
	enum al_muio_mux_if	iface,
	int			arg);


/* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */
/** @} end of MUIO mux group */
#endif		/* __AL_HAL_MUIO_MUX__ */

/** @} end of group_muio_mux group */
