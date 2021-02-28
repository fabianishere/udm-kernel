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
 * @defgroup group_eth_api API
 * Ethernet MAC HAL driver API
 * @ingroup group_eth
 * @{
 * @file   al_mod_hal_eth_mac_internal.h
 *
 * @brief Annpurna Labs Ethernet MAC API header file
 *
 */

#ifndef __AL_HAL_ETH_MAC_INTERNAL_H__
#define __AL_HAL_ETH_MAC_INTERNAL_H__

#include "al_mod_hal_common.h"
#include "al_mod_hal_eth_mac.h"

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

/* MAC error interrupts */

#define AL_ETH_MAC_GROUP_A_ERROR_INTS \
	(AL_ETH_MAC_IOFIC_GROUP_A_SERDES_3_TX_FIFO_ERR | \
	 AL_ETH_MAC_IOFIC_GROUP_A_SERDES_2_TX_FIFO_ERR | \
	 AL_ETH_MAC_IOFIC_GROUP_A_SERDES_1_TX_FIFO_ERR | \
	 AL_ETH_MAC_IOFIC_GROUP_A_SERDES_0_TX_FIFO_ERR | \
	 AL_ETH_MAC_IOFIC_GROUP_A_SERDES_3_TX_FIFO_FULL | \
	 AL_ETH_MAC_IOFIC_GROUP_A_SERDES_2_TX_FIFO_FULL | \
	 AL_ETH_MAC_IOFIC_GROUP_A_SERDES_1_TX_FIFO_FULL | \
	 AL_ETH_MAC_IOFIC_GROUP_A_SERDES_0_TX_FIFO_FULL)

#define AL_ETH_V1_V2_MAC_GROUP_A_ERROR_INTS \
	(AL_ETH_MAC_GROUP_A_ERROR_INTS | \
	 AL_ETH_V1_V2_MAC_IOFIC_GROUP_A_XGMII_DFIFO_64_32_ERR | \
	 AL_ETH_V1_V2_MAC_IOFIC_GROUP_A_XGMII_DFIFO_64_32_FULL | \
	 AL_ETH_V1_V2_MAC_IOFIC_GROUP_A_XGMII_DFIFO_32_64_ERR | \
	 AL_ETH_V1_V2_MAC_IOFIC_GROUP_A_XGMII_DFIFO_32_64_FULL)

#define AL_ETH_V1_MAC_GROUP_B_ERROR_INTS \
	(AL_ETH_MAC_IOFIC_GROUP_B_1G_TX_RTSM_PARITY | \
	 AL_ETH_MAC_IOFIC_GROUP_B_1G_RX_FIFO_PARITY | \
	 AL_ETH_V1_V2_MAC_IOFIC_GROUP_B_10G_RX_DATA_FIFO_PARITY | \
	 AL_ETH_V1_V2_MAC_IOFIC_GROUP_B_10G_RX_CTRL_FIFO_PARITY)

#define AL_ETH_V2_MAC_GROUP_B_ERROR_INTS \
	(AL_ETH_V2_V3_MAC_IOFIC_GROUP_B_1G_TX_FIFO_PARITY | \
	 AL_ETH_MAC_IOFIC_GROUP_B_1G_TX_RTSM_PARITY | \
	 AL_ETH_MAC_IOFIC_GROUP_B_1G_RX_FIFO_PARITY | \
	 AL_ETH_V1_V2_MAC_IOFIC_GROUP_B_10G_RX_DATA_FIFO_PARITY | \
	 AL_ETH_V1_V2_MAC_IOFIC_GROUP_B_10G_RX_CTRL_FIFO_PARITY | \
	 AL_ETH_V2_MAC_IOFIC_GROUP_B_10G_TX_DATA_FIFO_PARITY | \
	 AL_ETH_V2_MAC_IOFIC_GROUP_B_10G_TX_CTRL_FIFO_PARITY)

#define AL_ETH_V3_MAC_GROUP_B_ERROR_INTS \
	(AL_ETH_V2_V3_MAC_IOFIC_GROUP_B_1G_TX_FIFO_PARITY | \
	 AL_ETH_MAC_IOFIC_GROUP_B_1G_TX_RTSM_PARITY | \
	 AL_ETH_MAC_IOFIC_GROUP_B_1G_RX_FIFO_PARITY | \
	 AL_ETH_V3_MAC_IOFIC_GROUP_B_10G_LL_STATS_TX_PARITY | \
	 AL_ETH_V3_MAC_IOFIC_GROUP_B_10G_LL_STATS_RX_PARITY | \
	 AL_ETH_V3_MAC_IOFIC_GROUP_B_40G_LL_STATS_TX_PARITY | \
	 AL_ETH_V3_MAC_IOFIC_GROUP_B_40G_LL_STATS_RX_PARITY)

#define AL_ETH_MAC_GROUP_C_ERROR_INTS \
	(AL_ETH_MAC_IOFIC_GROUP_C_10G_PCS_MEM_PARITY | \
	 AL_ETH_MAC_IOFIC_GROUP_C_10G_PCS_LOOPBACK_PARITY | \
	 AL_ETH_MAC_IOFIC_GROUP_C_10G_PCS_DELAY_RAM_PARITY)

int al_mod_eth_ref_clk_freq_val(enum al_mod_eth_ref_clk_freq ref_clk_freq);

int al_mod_eth_mac_v1_v2_handle_init(struct al_mod_eth_mac_obj *obj,
	struct al_mod_eth_mac_obj_init_params *params);
int al_mod_eth_mac_v3_handle_init(struct al_mod_eth_mac_obj *obj,
	struct al_mod_eth_mac_obj_init_params *params);
int al_mod_eth_mac_v4_handle_init(struct al_mod_eth_mac_obj *obj,
	struct al_mod_eth_mac_obj_init_params *params);

int al_mod_eth_mac_v1_v3_mac_link_config(struct al_mod_eth_mac_obj *obj,
	al_mod_bool force_1000_base_x, al_mod_bool an_enable,
	uint32_t speed, al_mod_bool full_duplex);

int al_mod_eth_mac_v1_v3_mdio_config(struct al_mod_eth_mac_obj *, enum al_mod_eth_mdio_type mdio_type,
	al_mod_bool shared_mdio_if, enum al_mod_eth_ref_clk_freq ref_clk_freq,
	unsigned int mdio_clk_freq_khz, al_mod_bool ignore_read_status);
int al_mod_eth_mac_v1_v3_mdio_read(struct al_mod_eth_mac_obj *obj, uint32_t phy_addr, uint32_t device,
	uint32_t reg, uint16_t *val);
int al_mod_eth_mac_v1_v3_mdio_write(struct al_mod_eth_mac_obj *obj, uint32_t phy_addr, uint32_t device,
	uint32_t reg, uint16_t val);

int al_mod_eth_mac_v1_v3_led_set(struct al_mod_eth_mac_obj *obj, al_mod_bool link_is_up);

int al_mod_eth_mac_v1_v3_scratch_reg_read32(struct al_mod_eth_mac_obj *obj, unsigned int idx,
	uint32_t *val);
int al_mod_eth_mac_v1_v3_scratch_reg_write32(struct al_mod_eth_mac_obj *obj, unsigned int idx,
	uint32_t val);

#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */
#endif /* __AL_HAL_ETH_MAC_INTERNAL_H__ */
/** @} end of Ethernet group */
