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
 *  @{
 * @file   al_mod_hal_eth_mac_v1_v2.c
 *
 * @brief  Ethernet MAC v1 / v2 HAL
 *
 */

#include "al_mod_hal_eth.h"
#include "al_mod_hal_eth_mac.h"
#include "al_mod_hal_eth_mac_internal.h"
#include "al_mod_hal_eth_mac_regs.h"
#include "al_mod_hal_eth_mac_config_defs.h"

/* Number of xfi_txclk cycles that accumulate into 100ns */
#define ETH_MAC_KR_10_PCS_CFG_EEE_TIMER_VAL 52

static int mac_mode_set(struct al_mod_eth_mac_obj *obj, enum al_mod_eth_mac_mode mode)
{
	al_mod_assert(obj);

	obj->mac_mode = mode;

	return 0;
}

static int mac_mode_get(__attribute__((unused)) struct al_mod_eth_mac_obj *obj,
	__attribute__((unused)) enum al_mod_eth_mac_mode *mode)
{
	al_mod_assert(0);

	return 0;
}

static int mac_config(struct al_mod_eth_mac_obj *obj, enum al_mod_eth_mac_mode mode)
{
	struct al_mod_eth_mac_regs *mac_regs_base = obj->mac_regs_base;

	switch (mode) {
	case AL_ETH_MAC_MODE_RGMII:
		al_mod_reg_write32(&mac_regs_base->gen.clk_cfg,
			(ETH_MAC_GEN_CLK_CFG_RX_CLK_SEL |
				AL_ETH_MAC_GEN_CLK_CFG_LANES_CLK_SEL(0x3, 0x2, 0x1, 0x0)));

		/* 1G MAC control register */
		/* bit[0]  - TX_ENA - zeroed by default. Should be asserted by al_mod_eth_mac_start
		 * bit[1]  - RX_ENA - zeroed by default. Should be asserted by al_mod_eth_mac_start
		 * bit[3]  - ETH_SPEED - zeroed to enable 10/100 Mbps Ethernet
		 * bit[4]  - PROMIS_EN - asserted to enable MAC promiscuous mode
		 * bit[23] - CNTL_FRM-ENA - asserted to enable control frames
		 * bit[24] - NO_LGTH_CHECK - asserted to disable length checks,
		 * 		which is done in the controller
		 */
		al_mod_reg_write32(&mac_regs_base->mac_1g.cmd_cfg,
			(ETH_MAC_MAC_1G_CMD_CFG_NO_LGTH_CHECK |
				ETH_MAC_MAC_1G_CMD_CFG_CNTL_FRM_ENA  |
				ETH_MAC_MAC_1G_CMD_CFG_PROMIS_EN));

		/* RX_SECTION_EMPTY,  */
		al_mod_reg_write32(&mac_regs_base->mac_1g.rx_section_empty, 0x00000000);
		/* RX_SECTION_FULL,  */
		/* must be larger than almost empty */
		al_mod_reg_write32(&mac_regs_base->mac_1g.rx_section_full, 0x0000000c);
		/* RX_ALMOST_EMPTY,  */
		al_mod_reg_write32(&mac_regs_base->mac_1g.rx_almost_empty, 0x00000008);
		/* RX_ALMOST_FULL,  */
		al_mod_reg_write32(&mac_regs_base->mac_1g.rx_almost_full, 0x00000008);


		/* TX_SECTION_EMPTY,  */
		/* 8 ? */
		al_mod_reg_write32(&mac_regs_base->mac_1g.tx_section_empty, 0x00000008);
		/* TX_SECTION_FULL, 0 - store and forward, */
		al_mod_reg_write32(&mac_regs_base->mac_1g.tx_section_full, 0x0000000c);
		/* TX_ALMOST_EMPTY,  */
		al_mod_reg_write32(&mac_regs_base->mac_1g.tx_almost_empty, 0x00000008);
		/* TX_ALMOST_FULL,  */
		al_mod_reg_write32(&mac_regs_base->mac_1g.tx_almost_full, 0x00000008);

		/* XAUI MAC control register */
		al_mod_reg_write32(&mac_regs_base->gen.cfg,
			(ETH_MAC_GEN_CFG_SWAP_SERDES_TX(0x0) |
				ETH_MAC_GEN_CFG_SWAP_SERDES_RX(0x0) |
				ETH_MAC_GEN_CFG_MDIO_POL_NORMAL |
				ETH_MAC_GEN_CFG_MDIO_1_10_1G_MAC |
				ETH_MAC_GEN_CFG_TBI_MSB_TX_LSB |
				ETH_MAC_GEN_CFG_RGMII_SGMII_SEL_RGMII |
				ETH_MAC_GEN_CFG_SGMII_SEL_10G_MAC |
				ETH_MAC_GEN_CFG_TBI_MSB_RX_LSB |
				ETH_MAC_GEN_CFG_XAUI_RXAUI_XAUI |
				ETH_MAC_GEN_CFG_XGMII_SGMII_SGMII |
				ETH_MAC_GEN_CFG_MAC_1_10_1G_MAC));

		/* 1G MACSET 1G */
		/* taking sel_1000/sel_10 inputs from rgmii PHY, and not from register.
		 * disabling magic_packets detection in mac */
		al_mod_reg_write32(&mac_regs_base->gen.mac_1g_cfg,
			(ETH_MAC_GEN_MAC_1G_CFG_SLEEPN_SLEEP |
			ETH_MAC_GEN_MAC_1G_CFG_SET_10_DEF_0 |
			ETH_MAC_GEN_MAC_1G_CFG_SET_10_SEL_FROM_RGMII |
			ETH_MAC_GEN_MAC_1G_CFG_SET_1000_DEF_1 |
			ETH_MAC_GEN_MAC_1G_CFG_SET_1000_SEL_FROM_RGMII));
		/* RGMII set 1G */
		al_mod_reg_write32_masked(&mac_regs_base->gen.mux_sel,
			(ETH_MAC_GEN_MUX_SEL_SERDES_3_TX_MASK |
			ETH_MAC_GEN_MUX_SEL_SERDES_2_TX_MASK |
			ETH_MAC_GEN_MUX_SEL_SERDES_1_TX_MASK |
			ETH_MAC_GEN_MUX_SEL_SERDES_0_TX_MASK |
			ETH_MAC_GEN_MUX_SEL_XAUI_3_IN_MASK |
			ETH_MAC_GEN_MUX_SEL_XAUI_2_IN_MASK |
			ETH_MAC_GEN_MUX_SEL_XAUI_1_IN_MASK |
			ETH_MAC_GEN_MUX_SEL_XAUI_0_IN_MASK |
			ETH_MAC_GEN_MUX_SEL_RXAUI_1_IN_MASK |
			ETH_MAC_GEN_MUX_SEL_RXAUI_0_IN_MASK |
			ETH_MAC_GEN_MUX_SEL_SGMII_IN_MASK),
			(ETH_MAC_GEN_MUX_SEL_SERDES_3_TX(
			ETH_MAC_GEN_MUX_SEL_SERDES_TX_XAUI_0) |
			ETH_MAC_GEN_MUX_SEL_SERDES_2_TX(
			ETH_MAC_GEN_MUX_SEL_SERDES_TX_XAUI_0) |
			ETH_MAC_GEN_MUX_SEL_SERDES_1_TX(
			ETH_MAC_GEN_MUX_SEL_SERDES_TX_XAUI_0) |
			ETH_MAC_GEN_MUX_SEL_SERDES_0_TX(
			ETH_MAC_GEN_MUX_SEL_SERDES_TX_SGMII)  |
			ETH_MAC_GEN_MUX_SEL_XAUI_3_IN(0x3) |
			ETH_MAC_GEN_MUX_SEL_XAUI_2_IN(0x2) |
			ETH_MAC_GEN_MUX_SEL_XAUI_1_IN(0x1) |
			ETH_MAC_GEN_MUX_SEL_XAUI_0_IN(0x0) |
			ETH_MAC_GEN_MUX_SEL_RXAUI_1_IN(0x1) |
			ETH_MAC_GEN_MUX_SEL_RXAUI_0_IN(0x0) |
			ETH_MAC_GEN_MUX_SEL_SGMII_IN(0x0)));
		al_mod_reg_write32(&mac_regs_base->gen.rgmii_sel,
			(ETH_MAC_GEN_RGMII_SEL_RX_SWAP_9 |
			ETH_MAC_GEN_RGMII_SEL_RX_SWAP_7_3 |
			ETH_MAC_GEN_RGMII_SEL_RX_SWAP_4 |
			ETH_MAC_GEN_RGMII_SEL_RX_SWAP_3_0));
		break;
	case AL_ETH_MAC_MODE_SGMII:
		al_mod_reg_write32(&mac_regs_base->gen.clk_cfg, 0x40053210);

		/* 1G MAC control register */
		/* bit[0]  - TX_ENA - zeroed by default. Should be asserted by al_mod_eth_mac_start
		 * bit[1]  - RX_ENA - zeroed by default. Should be asserted by al_mod_eth_mac_start
		 * bit[3]  - ETH_SPEED - zeroed to enable 10/100 Mbps Ethernet
		 * bit[4]  - PROMIS_EN - asserted to enable MAC promiscuous mode
		 * bit[23] - CNTL_FRM-ENA - asserted to enable control frames
		 * bit[24] - NO_LGTH_CHECK - asserted to disable length checks,
		 *				which is done in the controller
		 */
		al_mod_reg_write32(&mac_regs_base->mac_1g.cmd_cfg, 0x01800010);

		/* RX_SECTION_EMPTY,  */
		al_mod_reg_write32(&mac_regs_base->mac_1g.rx_section_empty, 0x00000000);
		/* RX_SECTION_FULL,  */
		/* must be larger than almost empty */
		al_mod_reg_write32(&mac_regs_base->mac_1g.rx_section_full, 0x0000000c);
		/* RX_ALMOST_EMPTY,  */
		al_mod_reg_write32(&mac_regs_base->mac_1g.rx_almost_empty, 0x00000008);
		/* RX_ALMOST_FULL,  */
		al_mod_reg_write32(&mac_regs_base->mac_1g.rx_almost_full, 0x00000008);


		/* TX_SECTION_EMPTY,  */
		al_mod_reg_write32(&mac_regs_base->mac_1g.tx_section_empty, 0x00000008); /* 8 ? */
		/* TX_SECTION_FULL, 0 - store and forward, */
		al_mod_reg_write32(&mac_regs_base->mac_1g.tx_section_full, 0x0000000c);
		/* TX_ALMOST_EMPTY,  */
		al_mod_reg_write32(&mac_regs_base->mac_1g.tx_almost_empty, 0x00000008);
		/* TX_ALMOST_FULL,  */
		al_mod_reg_write32(&mac_regs_base->mac_1g.tx_almost_full, 0x00000008);

		/* XAUI MAC control register */
		al_mod_reg_write32(&mac_regs_base->gen.cfg, 0x000000c0);

		/* 1G MACSET 1G */
		/* taking sel_1000/sel_10 inputs from rgmii_converter, and not from register.
		 * disabling magic_packets detection in mac */
		al_mod_reg_write32(&mac_regs_base->gen.mac_1g_cfg, 0x00000002);
		/* SerDes configuration */
		al_mod_reg_write32_masked(&mac_regs_base->gen.mux_sel,
			~ETH_MAC_GEN_MUX_SEL_KR_IN_MASK, 0x00063910);
		al_mod_reg_write32(&mac_regs_base->gen.sd_fifo_ctrl, 0x000004f0);
		al_mod_reg_write32(&mac_regs_base->gen.sd_fifo_ctrl, 0x00000401);

		/* FAST AN -- Testing only */
#if (defined(AL_ETH_FAST_AN)) && (AL_ETH_FAST_AN == 1)
		al_mod_reg_write32(&mac_regs_base->sgmii.reg_addr, 0x00000012);
		al_mod_reg_write32(&mac_regs_base->sgmii.reg_data, 0x00000040);
		al_mod_reg_write32(&mac_regs_base->sgmii.reg_addr, 0x00000013);
		al_mod_reg_write32(&mac_regs_base->sgmii.reg_data, 0x00000000);
#endif

		/* Setting PCS i/f mode to SGMII (instead of default 1000Base-X) */
		al_mod_reg_write32(&mac_regs_base->sgmii.reg_addr, 0x00000014);
		al_mod_reg_write32(&mac_regs_base->sgmii.reg_data, 0x0000000b);
		/* setting dev_ability to have speed of 1000Mb, [11:10] = 2'b10 */
		al_mod_reg_write32(&mac_regs_base->sgmii.reg_addr, 0x00000004);
		al_mod_reg_write32(&mac_regs_base->sgmii.reg_data, 0x000009A0);
		al_mod_reg_write32_masked(&mac_regs_base->gen.led_cfg,
				      ETH_MAC_GEN_LED_CFG_SEL_MASK,
				      ETH_MAC_GEN_LED_CFG_SEL_DEFAULT_REG);
		break;
	case AL_ETH_MAC_MODE_SGMII_2_5G:
		/* MAC register file */
		al_mod_reg_write32(&mac_regs_base->mac_10g.cmd_cfg, 0x01022830);
		/* XAUI MAC control register */
		al_mod_reg_write32(&mac_regs_base->gen.cfg, 0x00000001);
		al_mod_reg_write32(&mac_regs_base->mac_10g.if_mode, 0x00000028);
		al_mod_reg_write32(&mac_regs_base->mac_10g.control, 0x00001140);
		/* RXAUI MAC control register */
		al_mod_reg_write32(&mac_regs_base->gen.xgmii_dfifo_32_64, 0x00000401);
		al_mod_reg_write32(&mac_regs_base->gen.xgmii_dfifo_64_32, 0x00000401);
		al_mod_reg_write32_masked(&mac_regs_base->gen.mux_sel,
				      ~ETH_MAC_GEN_MUX_SEL_KR_IN_MASK, 0x00063910);
		al_mod_reg_write32(&mac_regs_base->gen.clk_cfg, 0x40003210);
		al_mod_reg_write32(&mac_regs_base->gen.sd_fifo_ctrl, 0x000004f0);
		al_mod_reg_write32(&mac_regs_base->gen.sd_fifo_ctrl, 0x00000401);

		al_mod_reg_write32_masked(&mac_regs_base->gen.led_cfg,
				      ETH_MAC_GEN_LED_CFG_SEL_MASK,
				      ETH_MAC_GEN_LED_CFG_SEL_DEFAULT_REG);
		break;
	case AL_ETH_MAC_MODE_10GbE_Serial:
		/* MAC register file */
		al_mod_reg_write32(&mac_regs_base->mac_10g.cmd_cfg, 0x01022810);
		/* XAUI MAC control register */
		al_mod_reg_write32(&mac_regs_base->gen.cfg, 0x00000005);
		/* RXAUI MAC control register */
		al_mod_reg_write32(&mac_regs_base->gen.rxaui_cfg, 0x00000007);
		al_mod_reg_write32(&mac_regs_base->gen.sd_cfg, 0x000001F1);
		al_mod_reg_write32(&mac_regs_base->gen.xgmii_dfifo_32_64, 0x00000401);
		al_mod_reg_write32(&mac_regs_base->gen.xgmii_dfifo_64_32, 0x00000401);
		al_mod_reg_write32_masked(&mac_regs_base->gen.mux_sel,
					~ETH_MAC_GEN_MUX_SEL_KR_IN_MASK, 0x00073910);
		al_mod_reg_write32(&mac_regs_base->gen.clk_cfg, 0x10003210);
		al_mod_reg_write32(&mac_regs_base->gen.sd_fifo_ctrl, 0x000004f0);
		al_mod_reg_write32(&mac_regs_base->gen.sd_fifo_ctrl, 0x00000401);

		al_mod_reg_write32_masked(&mac_regs_base->gen.led_cfg,
				      ETH_MAC_GEN_LED_CFG_SEL_MASK,
				      ETH_MAC_GEN_LED_CFG_SEL_DEFAULT_REG);
		break;
	case AL_ETH_MAC_MODE_10G_SGMII:
		/* MAC register file */
		al_mod_reg_write32(&mac_regs_base->mac_10g.cmd_cfg, 0x01022810);

		/* XAUI MAC control register */
		al_mod_reg_write32(&mac_regs_base->gen.cfg, 0x00000001);

		al_mod_reg_write32(&mac_regs_base->mac_10g.if_mode, 0x0000002b);
		al_mod_reg_write32(&mac_regs_base->mac_10g.control, 0x00009140);
		/* FAST AN -- Testing only */
#if (defined(AL_ETH_FAST_AN)) && (AL_ETH_FAST_AN == 1)
		al_mod_reg_write32(&mac_regs_base->mac_10g.link_timer_lo, 0x00000040);
		al_mod_reg_write32(&mac_regs_base->mac_10g.link_timer_hi, 0x00000000);
#endif

		/* RXAUI MAC control register */
		al_mod_reg_write32(&mac_regs_base->gen.rxaui_cfg, 0x00000007);
		al_mod_reg_write32(&mac_regs_base->gen.xgmii_dfifo_32_64, 0x00000401);
		al_mod_reg_write32(&mac_regs_base->gen.xgmii_dfifo_64_32, 0x00000401);
		al_mod_reg_write32_masked(&mac_regs_base->gen.mux_sel,
					~ETH_MAC_GEN_MUX_SEL_KR_IN_MASK, 0x00063910);
		al_mod_reg_write32(&mac_regs_base->gen.clk_cfg, 0x40003210);
		al_mod_reg_write32(&mac_regs_base->gen.sd_fifo_ctrl, 0x00000401);

		al_mod_reg_write32_masked(&mac_regs_base->gen.led_cfg,
				      ETH_MAC_GEN_LED_CFG_SEL_MASK,
				      ETH_MAC_GEN_LED_CFG_SEL_DEFAULT_REG);
		break;
	default:
		return -EINVAL;
	}

	mac_mode_set(obj, mode);

	return 0;
}

static int mac_start_stop_adv(struct al_mod_eth_mac_obj *obj,
			      al_mod_bool tx,
			      al_mod_bool rx,
			      al_mod_bool tx_start,
			      al_mod_bool rx_start)
{
	struct al_mod_eth_mac_regs *mac_regs_base = obj->mac_regs_base;
	uint32_t cmd_cfg = 0;
	uint32_t mask = 0;

	al_mod_dbg("%s: mac_mode:%s, tx_modify:%d, tx_start:%d, rx_modify:%d, rx_start:%d\n",
	       __func__,
	       al_mod_eth_mac_mode_str(obj->mac_mode),
	       tx,
	       tx_start,
	       rx,
	       rx_start);

	switch (obj->mac_mode) {
	case AL_ETH_MAC_MODE_RGMII:
	case AL_ETH_MAC_MODE_SGMII:
		if (tx) {
			mask |= ETH_1G_MAC_CMD_CFG_TX_ENA;
			if (tx_start)
				cmd_cfg |= ETH_1G_MAC_CMD_CFG_TX_ENA;
		}

		if (rx) {
			mask |= ETH_1G_MAC_CMD_CFG_RX_ENA;
			if (rx_start)
				cmd_cfg |= ETH_1G_MAC_CMD_CFG_RX_ENA;
		}
		/* 1G MAC control register */
		al_mod_reg_write32_masked(&mac_regs_base->mac_1g.cmd_cfg,
				      mask,
				      cmd_cfg);
		break;
	case AL_ETH_MAC_MODE_SGMII_2_5G:
	case AL_ETH_MAC_MODE_10GbE_Serial:
	case AL_ETH_MAC_MODE_10G_SGMII:
		if (tx) {
			mask |= ETH_10G_MAC_CMD_CFG_TX_ENA;
			if (tx_start)
				cmd_cfg |= ETH_10G_MAC_CMD_CFG_TX_ENA;
		}

		if (rx) {
			mask |= ETH_10G_MAC_CMD_CFG_RX_ENA;
			if (rx_start)
				cmd_cfg |= ETH_10G_MAC_CMD_CFG_RX_ENA;
		}
		/* 10G MAC control register  */
		al_mod_reg_write32_masked(&mac_regs_base->mac_10g.cmd_cfg,
				      mask,
				      cmd_cfg);
		break;
	default:
		al_mod_err("%s@%s: Unsupported mac_mode: %s\n",
		       __func__, __FILE__,
		       al_mod_eth_mac_mode_str(obj->mac_mode));
		return -EINVAL;
	}

	return 0;
}

static int mac_loopback_config(struct al_mod_eth_mac_obj *obj, al_mod_bool enable)
{
	struct al_mod_eth_mac_regs *mac_regs_base = obj->mac_regs_base;

	switch (obj->mac_mode) {
	case AL_ETH_MAC_MODE_RGMII:
	case AL_ETH_MAC_MODE_SGMII:
		al_mod_reg_write32_masked(&mac_regs_base->mac_1g.cmd_cfg, AL_BIT(15),
			(enable ? AL_BIT(15) : 0));
		break;
	case AL_ETH_MAC_MODE_10GbE_Serial:
		al_mod_reg_write32(&mac_regs_base->kr.pcs_addr, ETH_MAC_KR_PCS_CONTROL_1_ADDR);
		al_mod_reg_write32_masked(&mac_regs_base->kr.pcs_data, AL_BIT(14),
			enable ? AL_BIT(14) : 0);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static int mac_stats_get(struct al_mod_eth_mac_obj *obj,  struct al_mod_eth_mac_stats *stats)
{
	struct al_mod_eth_mac_regs *mac_regs_base = obj->mac_regs_base;
	struct al_mod_eth_mac_1g_stats __iomem *stats_1g = &mac_regs_base->mac_1g.stats;
	struct al_mod_eth_mac_10g_stats_v2 __iomem *stats_10g = &mac_regs_base->mac_10g.stats.v2;
	uint64_t octets;

	al_mod_assert(stats);

	al_mod_memset(stats, 0, sizeof(struct al_mod_eth_mac_stats));

	switch (obj->mac_mode) {
	case AL_ETH_MAC_MODE_RGMII:
	case AL_ETH_MAC_MODE_SGMII:
		stats->if_in_ucast_pkts = al_mod_reg_read32(&stats_1g->if_in_ucast_pkts);
		stats->if_in_mcast_pkts = al_mod_reg_read32(&stats_1g->if_in_mcast_pkts);
		stats->if_in_bcast_pkts = al_mod_reg_read32(&stats_1g->if_in_bcast_pkts);
		stats->pkts = al_mod_reg_read32(&stats_1g->pkts);
		stats->if_out_ucast_pkts = al_mod_reg_read32(&stats_1g->if_out_ucast_pkts);
		stats->if_out_mcast_pkts = al_mod_reg_read32(&stats_1g->if_out_mcast_pkts);
		stats->if_out_bcast_pkts = al_mod_reg_read32(&stats_1g->if_out_bcast_pkts);
		stats->if_in_errors = al_mod_reg_read32(&stats_1g->if_in_errors);
		stats->if_out_errors = al_mod_reg_read32(&stats_1g->if_out_errors);
		stats->frames_rxed_ok = al_mod_reg_read32(&stats_1g->frames_rxed_ok);
		stats->frames_txed_ok = al_mod_reg_read32(&stats_1g->frames_txed_ok);
		stats->octets_rxed_ok = al_mod_reg_read32(&stats_1g->octets_rxed_ok);
		stats->octets_txed_ok = al_mod_reg_read32(&stats_1g->octets_txed_ok);
		stats->undersize_pkts = al_mod_reg_read32(&stats_1g->undersize_pkts);
		stats->fragments = al_mod_reg_read32(&stats_1g->fragments);
		stats->jabbers = al_mod_reg_read32(&stats_1g->jabbers);
		stats->oversize_pkts = al_mod_reg_read32(&stats_1g->oversize_pkts);
		stats->fcs_errors =
			al_mod_reg_read32(&stats_1g->fcs_errors);
		stats->alignment_errors = al_mod_reg_read32(&stats_1g->alignment_errors);
		stats->drop_events = al_mod_reg_read32(&stats_1g->drop_events);
		stats->pause_mac_ctrl_frames_txed =
			al_mod_reg_read32(&stats_1g->pause_mac_ctrl_frames_txed);
		stats->pause_mac_ctrl_frames_rxed =
			al_mod_reg_read32(&stats_1g->pause_mac_ctrl_frames_rxed);
		stats->frame_too_long_errs = 0; /* N/A */
		stats->in_range_length_errs = 0; /* N/A */
		stats->vlan_txed_ok = 0; /* N/A */
		stats->vlan_rxed_ok = 0; /* N/A */
		stats->octets = al_mod_reg_read32(&stats_1g->octets);
		stats->pkts_64_octets = al_mod_reg_read32(&stats_1g->pkts_64_octets);
		stats->pkts_65_to_127_octets =
			al_mod_reg_read32(&stats_1g->pkts_65_to_127_octets);
		stats->pkts_128_to_255_octets =
			al_mod_reg_read32(&stats_1g->pkts_128_to_255_octets);
		stats->pkts_256_to_511_octets =
			al_mod_reg_read32(&stats_1g->pkts_256_to_511_octets);
		stats->pkts_512_to_1023_octets =
			al_mod_reg_read32(&stats_1g->pkts_512_to_1023_octets);
		stats->pkts_1024_to_1518_octets =
			al_mod_reg_read32(&stats_1g->pkts_1024_to_1518_octets);
		stats->pkts_1519_to_x = al_mod_reg_read32(&stats_1g->pkts_1519_to_x);
		break;
	case AL_ETH_MAC_MODE_SGMII_2_5G:
	case AL_ETH_MAC_MODE_10GbE_Serial:
	case AL_ETH_MAC_MODE_10G_SGMII:
		stats->if_in_ucast_pkts = al_mod_reg_read32(&stats_10g->if_in_ucast_pkts);
		stats->if_in_mcast_pkts = al_mod_reg_read32(&stats_10g->if_in_mcast_pkts);
		stats->if_in_bcast_pkts = al_mod_reg_read32(&stats_10g->if_in_bcast_pkts);
		stats->pkts = al_mod_reg_read32(&stats_10g->pkts);
		stats->if_out_ucast_pkts = al_mod_reg_read32(&stats_10g->if_out_ucast_pkts);
		stats->if_out_mcast_pkts = al_mod_reg_read32(&stats_10g->if_out_mcast_pkts);
		stats->if_out_bcast_pkts = al_mod_reg_read32(&stats_10g->if_out_bcast_pkts);
		stats->if_in_errors = al_mod_reg_read32(&stats_10g->if_in_errors);
		stats->if_out_errors = al_mod_reg_read32(&stats_10g->if_out_errors);
		stats->frames_rxed_ok = al_mod_reg_read32(&stats_10g->frames_rxed_ok);
		stats->frames_txed_ok = al_mod_reg_read32(&stats_10g->frames_txed_ok);

		/* octets_rxed_ok = if_in_octets - 18 * frames_rxed_ok - 4 * vlan_rxed_ok */
		octets = al_mod_reg_read32(&stats_10g->if_in_octets_l);
		octets |= (uint64_t)(al_mod_reg_read32(&stats_10g->if_in_octets_h)) << 32;
		octets -= 18 * stats->frames_rxed_ok;
		octets -= 4 * al_mod_reg_read32(&stats_10g->vlan_rxed_ok);
		stats->octets_rxed_ok = octets;

		/* octets_txed_ok = if_out_octets - 18 * frames_txed_ok
		 * - 4 * vlan_txed_ok */
		octets = al_mod_reg_read32(&stats_10g->if_out_octets_l);
		octets |= (uint64_t)(al_mod_reg_read32(&stats_10g->if_out_octets_h)) << 32;
		octets -= 18 * stats->frames_txed_ok;
		octets -= 4 * al_mod_reg_read32(&stats_10g->vlan_txed_ok);
		stats->octets_txed_ok = octets;

		stats->undersize_pkts =
			al_mod_reg_read32(&stats_10g->undersize_pkts);
		stats->fragments = al_mod_reg_read32(&stats_10g->fragments);
		stats->jabbers = al_mod_reg_read32(&stats_10g->jabbers);
		stats->oversize_pkts = al_mod_reg_read32(&stats_10g->oversize_pkts);
		stats->fcs_errors =
			al_mod_reg_read32(&stats_10g->fcs_errors);
		stats->alignment_errors = al_mod_reg_read32(&stats_10g->alignment_errors);
		stats->drop_events = al_mod_reg_read32(&stats_10g->drop_events);
		stats->pause_mac_ctrl_frames_txed =
			al_mod_reg_read32(&stats_10g->pause_mac_ctrl_frames_txed);
		stats->pause_mac_ctrl_frames_rxed =
			al_mod_reg_read32(&stats_10g->pause_mac_ctrl_frames_rxed);
		stats->frame_too_long_errs = al_mod_reg_read32(&stats_10g->frame_too_long_errs);
		stats->in_range_length_errs = al_mod_reg_read32(&stats_10g->in_range_length_errs);
		stats->vlan_txed_ok = al_mod_reg_read32(&stats_10g->vlan_txed_ok);
		stats->vlan_rxed_ok = al_mod_reg_read32(&stats_10g->vlan_rxed_ok);
		stats->octets = al_mod_reg_read32(&stats_10g->octets);
		stats->pkts_64_octets = al_mod_reg_read32(&stats_10g->pkts_64_octets);
		stats->pkts_65_to_127_octets =
			al_mod_reg_read32(&stats_10g->pkts_65_to_127_octets);
		stats->pkts_128_to_255_octets =
			al_mod_reg_read32(&stats_10g->pkts_128_to_255_octets);
		stats->pkts_256_to_511_octets =
			al_mod_reg_read32(&stats_10g->pkts_256_to_511_octets);
		stats->pkts_512_to_1023_octets =
			al_mod_reg_read32(&stats_10g->pkts_512_to_1023_octets);
		stats->pkts_1024_to_1518_octets =
			al_mod_reg_read32(&stats_10g->pkts_1024_to_1518_octets);
		stats->pkts_1519_to_x = al_mod_reg_read32(&stats_10g->pkts_1519_to_x);
		break;
	default:
		return -EPERM;
	}

	stats->eee_in = al_mod_reg_read32(&mac_regs_base->stat.eee_in);
	stats->eee_out = al_mod_reg_read32(&mac_regs_base->stat.eee_out);

	return 0;
}

static int fec_enable(
	struct al_mod_eth_mac_obj *obj,
	enum al_mod_eth_fec_type fec_type __attribute__((unused)),
	al_mod_bool enable)
{
	al_mod_assert(obj);
	al_mod_assert(!enable);

	return 0;
}

static al_mod_bool fec_is_enabled(__attribute__((unused)) struct al_mod_eth_mac_obj *obj)
{
	return AL_FALSE;
}

static int link_status_get(struct al_mod_eth_mac_obj *obj,
			   struct al_mod_eth_link_status *status)
{
	uint32_t reg;
	struct al_mod_eth_mac_regs *mac_regs_base = obj->mac_regs_base;

	switch (obj->mac_mode) {
	case AL_ETH_MAC_MODE_10G_SGMII:
	case AL_ETH_MAC_MODE_10GbE_Serial:
	case AL_ETH_MAC_MODE_SGMII_2_5G:
		status->link_up = AL_FALSE;
		status->local_fault = AL_TRUE;
		status->remote_fault = AL_TRUE;

		al_mod_reg_write32(&mac_regs_base->kr.pcs_addr, ETH_MAC_KR_PCS_BASE_R_STATUS2);
		reg = al_mod_reg_read32(&mac_regs_base->kr.pcs_data);

		if (reg & AL_BIT(15)) {
			reg = al_mod_reg_read32(&mac_regs_base->mac_10g.status);

			status->remote_fault = ((reg & ETH_MAC_GEN_MAC_10G_STAT_REM_FAULT) ?
							AL_TRUE : AL_FALSE);
			status->local_fault = ((reg & ETH_MAC_GEN_MAC_10G_STAT_LOC_FAULT) ?
							AL_TRUE : AL_FALSE);

			status->link_up = ((status->remote_fault == AL_FALSE) &&
					   (status->local_fault == AL_FALSE));
		}
		break;

	case AL_ETH_MAC_MODE_SGMII:
		al_mod_reg_write32(&mac_regs_base->sgmii.reg_addr, 1);
		/*
		 * This register is latched low so need to read twice to get
		 * the current link status
		 */
		reg = al_mod_reg_read32(&mac_regs_base->sgmii.reg_data);
		reg = al_mod_reg_read32(&mac_regs_base->sgmii.reg_data);

		status->link_up = AL_FALSE;

		if (reg & AL_BIT(2))
			status->link_up = AL_TRUE;

		reg = al_mod_reg_read32(&mac_regs_base->sgmii.link_stat);

		if ((reg & AL_BIT(5)) == 0)
			status->link_up = AL_FALSE;
		break;

	case AL_ETH_MAC_MODE_RGMII:
		reg = al_mod_reg_read32(&mac_regs_base->gen.rgmii_stat);

		status->link_up = AL_FALSE;

		if (reg & AL_BIT(4))
			status->link_up = AL_TRUE;
		break;

	default:
		/* not implemented yet */
		return -EPERM;
	}

	return 0;
}

static int link_status_clear(struct al_mod_eth_mac_obj *obj)
{
	int status = 0;
	struct al_mod_eth_mac_regs *mac_regs_base = obj->mac_regs_base;

	switch (obj->mac_mode) {
	case AL_ETH_MAC_MODE_10G_SGMII:
	case AL_ETH_MAC_MODE_10GbE_Serial:
	case AL_ETH_MAC_MODE_SGMII_2_5G:
		al_mod_reg_write32(&mac_regs_base->kr.pcs_addr, ETH_MAC_KR_PCS_BASE_R_STATUS2);
		al_mod_reg_read32(&mac_regs_base->kr.pcs_data);
		al_mod_reg_read32(&mac_regs_base->mac_10g.status);
		break;

	default:
		status = -EPERM;
		break;
	}

	return status;
}

/** This complements the function in al_mod_hal_eth_main.c */
static int rx_pkt_limit_config(struct al_mod_eth_mac_obj *obj,
	unsigned int min_rx_len __attribute__((__unused__)), unsigned int max_rx_len)
{
	struct al_mod_eth_mac_regs *mac_regs_base = obj->mac_regs_base;

	/* configure the MAC's max rx length, add 16 bytes so the packet get
	 * trimmed by the EC/Async_fifo rather by the MAC
	*/
	if (AL_ETH_IS_1G_MAC(obj->mac_mode))
		al_mod_reg_write32(&mac_regs_base->mac_1g.frm_len, max_rx_len + 16);
	else if (AL_ETH_IS_10G_MAC(obj->mac_mode) || AL_ETH_IS_25G_MAC(obj->mac_mode))
		/* 10G MAC control register  */
		al_mod_reg_write32(&mac_regs_base->mac_10g.frm_len, (max_rx_len + 16));

	return 0;
}

static int eee_config(struct al_mod_eth_mac_obj *obj,
	struct al_mod_eth_eee_params *params __attribute__((__unused__)))
{
	struct al_mod_eth_mac_regs *mac_regs_base = obj->mac_regs_base;

	switch (obj->mac_mode) {
	/** Do nothing at SGMII, RGMII */
	case AL_ETH_MAC_MODE_RGMII:
	case AL_ETH_MAC_MODE_SGMII:
		break;
	case AL_ETH_MAC_MODE_10G_SGMII:
	case AL_ETH_MAC_MODE_10GbE_Serial:
	case AL_ETH_MAC_MODE_SGMII_2_5G:
		al_mod_reg_write32_masked(
			&mac_regs_base->kr.pcs_cfg,
			ETH_MAC_KR_PCS_CFG_EEE_TIMER_VAL_MASK,
			ETH_MAC_KR_10_PCS_CFG_EEE_TIMER_VAL);
		break;
	default:
		return -EPERM;
		break;
	}

	return 0;
}

static int tx_flush_config(struct al_mod_eth_mac_obj *obj, al_mod_bool enable)
{
	struct al_mod_eth_mac_regs *mac_regs_base = obj->mac_regs_base;

	if (!AL_ETH_IS_10G_MAC(obj->mac_mode))
		return -EPERM;

	al_mod_reg_write32_masked(&mac_regs_base->mac_10g.cmd_cfg,
		ETH_10G_MAC_CMD_CFG_TX_FLUSH,
		(enable ? ETH_10G_MAC_CMD_CFG_TX_FLUSH : 0));

	return 0;
}

static void iofic_attrs_get_hw_v1(struct al_mod_eth_mac_obj *obj __attribute__((unused)),
				  uint32_t *mac_a_mask,
				  uint32_t *mac_b_mask,
				  uint32_t *mac_c_mask,
				  uint32_t *mac_d_mask)
{
	al_mod_assert(mac_a_mask);
	al_mod_assert(mac_b_mask);
	al_mod_assert(mac_c_mask);
	al_mod_assert(mac_d_mask);

	*mac_a_mask = AL_ETH_V1_V2_MAC_GROUP_A_ERROR_INTS;
	*mac_b_mask = AL_ETH_V1_MAC_GROUP_B_ERROR_INTS;
	*mac_c_mask = 0;
	*mac_d_mask = 0;

	/**
	 * Addressing RMN: 11186
	 *
	 * RMN description:
	 * False indication of parity interrupts
	 *
	 * Software flow:
	 * mask and ignore this interrupts
	 */
	*mac_a_mask &= ~(AL_ETH_MAC_IOFIC_GROUP_A_SERDES_0_TX_FIFO_ERR |
			 AL_ETH_MAC_IOFIC_GROUP_A_SERDES_3_TX_FIFO_FULL |
			 AL_ETH_MAC_IOFIC_GROUP_A_SERDES_2_TX_FIFO_FULL |
			 AL_ETH_MAC_IOFIC_GROUP_A_SERDES_1_TX_FIFO_FULL |
			 AL_ETH_V1_V2_MAC_IOFIC_GROUP_A_XGMII_DFIFO_64_32_ERR |
			 AL_ETH_V1_V2_MAC_IOFIC_GROUP_A_XGMII_DFIFO_64_32_FULL);
	*mac_b_mask &= ~AL_ETH_V1_V2_MAC_IOFIC_GROUP_B_10G_RX_DATA_FIFO_PARITY;
}

static void iofic_attrs_get_runtime_inval_v1(struct al_mod_eth_mac_obj *obj,
					     uint32_t *mac_a_mask,
					     uint32_t *mac_b_mask,
					     uint32_t *mac_c_mask,
					     uint32_t *mac_d_mask)
{
	al_mod_assert(mac_a_mask);
	al_mod_assert(mac_b_mask);
	al_mod_assert(mac_c_mask);
	al_mod_assert(mac_d_mask);

	*mac_a_mask = 0;
	*mac_b_mask = 0;
	*mac_c_mask = 0;
	*mac_d_mask = 0;

	if (AL_ETH_IS_1G_MAC(obj->mac_mode))
		*mac_b_mask |= (AL_ETH_V1_V2_MAC_IOFIC_GROUP_B_10G_RX_DATA_FIFO_PARITY |
				AL_ETH_V1_V2_MAC_IOFIC_GROUP_B_10G_RX_CTRL_FIFO_PARITY);
	else
		*mac_b_mask |= (AL_ETH_MAC_IOFIC_GROUP_B_1G_TX_RTSM_PARITY |
				AL_ETH_MAC_IOFIC_GROUP_B_1G_RX_FIFO_PARITY);
}

static void iofic_attrs_get_transient_v1(struct al_mod_eth_mac_obj *obj,
					 uint32_t *mac_a_mask,
					 uint32_t *mac_b_mask,
					 uint32_t *mac_c_mask,
					 uint32_t *mac_d_mask)
{
	/* All v1 IOFIC bits are considered transient */
	iofic_attrs_get_hw_v1(obj, mac_a_mask, mac_b_mask, mac_c_mask, mac_d_mask);
}

static void iofic_attrs_get_non_fatal_v1(struct al_mod_eth_mac_obj *obj __attribute__((unused)),
					 uint32_t *mac_a_mask,
					 uint32_t *mac_b_mask,
					 uint32_t *mac_c_mask,
					 uint32_t *mac_d_mask)
{
	al_mod_assert(mac_a_mask);
	al_mod_assert(mac_b_mask);
	al_mod_assert(mac_c_mask);
	al_mod_assert(mac_d_mask);

	*mac_a_mask = 0;
	*mac_b_mask = 0;
	*mac_c_mask = 0;
	*mac_d_mask = 0;
}

static void iofic_attrs_get_hw_v2(struct al_mod_eth_mac_obj *obj,
				  uint32_t *mac_a_mask,
				  uint32_t *mac_b_mask,
				  uint32_t *mac_c_mask,
				  uint32_t *mac_d_mask)
{
	al_mod_assert(mac_a_mask);
	al_mod_assert(mac_b_mask);
	al_mod_assert(mac_c_mask);
	al_mod_assert(mac_d_mask);

	*mac_a_mask = AL_ETH_V1_V2_MAC_GROUP_A_ERROR_INTS;
	*mac_b_mask = AL_ETH_V2_MAC_GROUP_B_ERROR_INTS;
	*mac_c_mask = 0;
	*mac_d_mask = 0;

	if (obj->eth_original_rev_id < AL_ETH_REV_ID_4) {
		/**
		 * Addressing RMN: 12109
		 *
		 * RMN description:
		 * False indication of parity interrupts
		 *
		 * Software flow:
		 * mask and ignore these interrupts
		 */
		*mac_b_mask &= ~AL_ETH_MAC_IOFIC_GROUP_B_1G_RX_FIFO_PARITY;
	}
}

static void iofic_attrs_get_runtime_inval_v2(struct al_mod_eth_mac_obj *obj,
					     uint32_t *mac_a_mask,
					     uint32_t *mac_b_mask,
					     uint32_t *mac_c_mask,
					     uint32_t *mac_d_mask)
{
	al_mod_assert(mac_a_mask);
	al_mod_assert(mac_b_mask);
	al_mod_assert(mac_c_mask);
	al_mod_assert(mac_d_mask);

	*mac_a_mask = 0;
	*mac_b_mask = 0;
	*mac_c_mask = 0;
	*mac_d_mask = 0;

	if (AL_ETH_IS_1G_MAC(obj->mac_mode)) {
		*mac_a_mask |= (AL_ETH_V1_V2_MAC_IOFIC_GROUP_A_XGMII_DFIFO_64_32_FULL |
				AL_ETH_V1_V2_MAC_IOFIC_GROUP_A_XGMII_DFIFO_64_32_ERR |
				AL_ETH_V1_V2_MAC_IOFIC_GROUP_A_XGMII_DFIFO_32_64_FULL |
				AL_ETH_V1_V2_MAC_IOFIC_GROUP_A_XGMII_DFIFO_32_64_ERR);

		*mac_b_mask |= (AL_ETH_V1_V2_MAC_IOFIC_GROUP_B_10G_RX_DATA_FIFO_PARITY |
				AL_ETH_V1_V2_MAC_IOFIC_GROUP_B_10G_RX_CTRL_FIFO_PARITY |
				AL_ETH_V2_MAC_IOFIC_GROUP_B_10G_TX_DATA_FIFO_PARITY |
				AL_ETH_V2_MAC_IOFIC_GROUP_B_10G_TX_CTRL_FIFO_PARITY);
	} else {
		*mac_b_mask |= (AL_ETH_V2_V3_MAC_IOFIC_GROUP_B_1G_TX_FIFO_PARITY |
				AL_ETH_MAC_IOFIC_GROUP_B_1G_RX_FIFO_PARITY);
	}

	/* FIFO for retransmission in half-duplex mode - never used */
	*mac_b_mask |= AL_ETH_MAC_IOFIC_GROUP_B_1G_TX_RTSM_PARITY;
}

static void iofic_attrs_get_transient_v2(struct al_mod_eth_mac_obj *obj,
					 uint32_t *mac_a_mask,
					 uint32_t *mac_b_mask,
					 uint32_t *mac_c_mask,
					 uint32_t *mac_d_mask)
{
	/* All v2 IOFIC bits are considered transient */
	iofic_attrs_get_hw_v2(obj, mac_a_mask, mac_b_mask, mac_c_mask, mac_d_mask);
}

static void iofic_attrs_get_non_fatal_v2(struct al_mod_eth_mac_obj *obj __attribute__((unused)),
					 uint32_t *mac_a_mask,
					 uint32_t *mac_b_mask,
					 uint32_t *mac_c_mask,
					 uint32_t *mac_d_mask)
{
	al_mod_assert(mac_a_mask);
	al_mod_assert(mac_b_mask);
	al_mod_assert(mac_c_mask);
	al_mod_assert(mac_d_mask);

	*mac_a_mask = 0;
	*mac_b_mask = 0;
	*mac_c_mask = 0;
	*mac_d_mask = 0;
}

int al_mod_eth_mac_v1_v2_handle_init(struct al_mod_eth_mac_obj *obj,
	__attribute__((unused)) struct al_mod_eth_mac_obj_init_params *params)
{
	obj->mode_set = mac_mode_set;
	obj->mode_get = mac_mode_get;
	obj->config = mac_config;
	obj->link_config = al_mod_eth_mac_v1_v3_mac_link_config;
	obj->fec_enable = fec_enable;
	obj->link_status_get = link_status_get;
	obj->link_status_clear = link_status_clear;
	obj->led_set = al_mod_eth_mac_v1_v3_led_set;
	obj->loopback_config = mac_loopback_config;
	obj->stats_get = mac_stats_get;
	obj->mdio_config = al_mod_eth_mac_v1_v3_mdio_config;
	obj->mdio_read = al_mod_eth_mac_v1_v3_mdio_read;
	obj->mdio_write = al_mod_eth_mac_v1_v3_mdio_write;
	obj->rx_pkt_limit_config = rx_pkt_limit_config;
	obj->eee_config = eee_config;
	obj->scratch_read = al_mod_eth_mac_v1_v3_scratch_reg_read32;
	obj->scratch_write = al_mod_eth_mac_v1_v3_scratch_reg_write32;
	obj->tx_flush_config = tx_flush_config;
	obj->fec_is_enabled = fec_is_enabled;
	if (params->eth_rev_id == AL_ETH_REV_ID_1) {
		obj->iofic_attrs_get_hw = iofic_attrs_get_hw_v1;
		obj->iofic_attrs_get_runtime_inval = iofic_attrs_get_runtime_inval_v1;
		obj->iofic_attrs_get_transient = iofic_attrs_get_transient_v1;
		obj->iofic_attrs_get_non_fatal = iofic_attrs_get_non_fatal_v1;
	} else {
		obj->iofic_attrs_get_hw = iofic_attrs_get_hw_v2;
		obj->iofic_attrs_get_runtime_inval = iofic_attrs_get_runtime_inval_v2;
		obj->iofic_attrs_get_transient = iofic_attrs_get_transient_v2;
		obj->iofic_attrs_get_non_fatal = iofic_attrs_get_non_fatal_v2;
	}
	obj->mac_start_stop_adv = mac_start_stop_adv;

	return 0;
}

/** @} end of Ethernet group */
