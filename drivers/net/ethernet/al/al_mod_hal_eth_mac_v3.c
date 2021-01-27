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
 * @file   al_mod_hal_eth_mac_v3.c
 *
 * @brief  Ethernet MAC v3 HAL
 *
 */

#include "al_mod_hal_eth.h"
#include "al_mod_hal_eth_mac.h"
#include "al_mod_hal_eth_mac_internal.h"
#include "al_mod_hal_eth_mac_regs.h"


/* Number of xfi_txclk cycles that accumulate into 100ns */
#define ETH_MAC_KR_10_PCS_CFG_EEE_TIMER_VAL 52
#define ETH_MAC_KR_25_PCS_CFG_EEE_TIMER_VAL 80
#define ETH_MAC_XLG_40_PCS_CFG_EEE_TIMER_VAL 63
#define ETH_MAC_XLG_50_PCS_CFG_EEE_TIMER_VAL 85

/**
 * read KR PCS registers (indirect access)
 *
 * @param mac_regs_base MAC register base
 * @param reg_addr address in the an registers
 *
 * @return the register value
 */
static uint32_t al_mod_eth_kr_pcs_reg_read(
			struct al_mod_eth_mac_regs *mac_regs_base,
			uint32_t reg_addr)
{
	uint32_t val;

	/* indirect access */
	al_mod_reg_write32(&mac_regs_base->kr.pcs_addr, reg_addr);
	val = al_mod_reg_read32(&mac_regs_base->kr.pcs_data);

	al_mod_dbg("%s: reg %d. val 0x%x\n", __func__, reg_addr, val);

	return val;
}

/**
 * write KR PCS registers (indirect access)
 *
 * @param mac_regs_base MAC register base
 * @param reg_addr address in the an registers
 * @param reg_data value to write to the register
 *
 */
static void al_mod_eth_kr_pcs_reg_write(
			struct al_mod_eth_mac_regs *mac_regs_base,
			uint32_t reg_addr,
			uint32_t reg_data)
{
	al_mod_reg_write32(&mac_regs_base->kr.pcs_addr, reg_addr);
	al_mod_reg_write32(&mac_regs_base->kr.pcs_data, reg_data);

	al_mod_dbg("%s: reg %d. val 0x%x\n", __func__, reg_addr, reg_data);
}

/**
 * write masked KR PCS registers (indirect access)
 *
 * @param mac_regs_base MAC register base
 * @param reg_addr address in the an registers
 * @param reg_mask mask of value to write to the register
 * @param reg_data value to write to the register
 *
 */
static void al_mod_eth_kr_pcs_reg_write_masked(
			struct al_mod_eth_mac_regs *mac_regs_base,
			uint32_t reg_addr,
			uint32_t reg_mask,
			uint32_t reg_data)
{
	uint32_t temp;

	temp = al_mod_eth_kr_pcs_reg_read(mac_regs_base, reg_addr);
	al_mod_eth_kr_pcs_reg_write(mac_regs_base, reg_addr, AL_MASK_VAL(reg_mask, reg_data, temp));

	al_mod_dbg("%s: reg %d. val 0x%x\n", __func__, reg_addr, reg_data);
}

/**
 * read 40G MAC registers (indirect access)
 *
 * @param mac_regs_base MAC register base
 * @param reg_addr address in the an registers
 *
 * @return the register value
 */
static uint32_t al_mod_eth_40g_mac_reg_read(
			struct al_mod_eth_mac_regs *mac_regs_base,
			uint32_t reg_addr)
{
	uint32_t val;

	al_mod_reg_write32(&mac_regs_base->gen_v3.mac_40g_ll_addr, reg_addr);
	val = al_mod_reg_read32(&mac_regs_base->gen_v3.mac_40g_ll_data);

	al_mod_dbg("%s: reg %d. val 0x%x\n", __func__, reg_addr, val);

	return val;
}

/**
 * write 40G MAC registers (indirect access)
 *
 * @param mac_regs_base MAC register base
 * @param reg_addr address in the an registers
 * @param reg_data value to write to the register
 *
 */
static void al_mod_eth_40g_mac_reg_write(
			struct al_mod_eth_mac_regs *mac_regs_base,
			uint32_t reg_addr,
			uint32_t reg_data)
{
	al_mod_reg_write32(&mac_regs_base->gen_v3.mac_40g_ll_addr, reg_addr);
	al_mod_reg_write32(&mac_regs_base->gen_v3.mac_40g_ll_data, reg_data);

	al_mod_dbg("%s: reg %d. val 0x%x\n", __func__, reg_addr, reg_data);
}

/**
 * read 40G PCS registers (indirect access)
 *
 * @param mac_regs_base MAC register base
 * @param reg_addr address in the an registers
 *
 * @return the register value
 */
static uint32_t al_mod_eth_40g_pcs_reg_read(
			struct al_mod_eth_mac_regs *mac_regs_base,
			uint32_t reg_addr)
{
	uint32_t val;

	al_mod_reg_write32(&mac_regs_base->gen_v3.pcs_40g_ll_addr, reg_addr);
	val = al_mod_reg_read32(&mac_regs_base->gen_v3.pcs_40g_ll_data);

	al_mod_dbg("%s: reg %d. val 0x%x\n", __func__, reg_addr, val);

	return val;
}

/**
 * write 40G PCS registers (indirect access)
 *
 * @param mac_regs_base MAC register base
 * @param reg_addr address in the an registers
 * @param reg_data value to write to the register
 *
 */
static void al_mod_eth_40g_pcs_reg_write(
			struct al_mod_eth_mac_regs *mac_regs_base,
			uint32_t reg_addr,
			uint32_t reg_data)
{
	al_mod_reg_write32(&mac_regs_base->gen_v3.pcs_40g_ll_addr, reg_addr);
	al_mod_reg_write32(&mac_regs_base->gen_v3.pcs_40g_ll_data, reg_data);

	al_mod_dbg("%s: reg %d. val 0x%x\n", __func__, reg_addr, reg_data);
}

/**
 * write masked 40G PCS registers (indirect access)
 *
 * @param mac_regs_base MAC register base
 * @param reg_addr address in the an registers
 * @param reg_mask mask of value to write to the register
 * @param reg_data value to write to the register
 *
 */
static void al_mod_eth_40g_pcs_reg_write_masked(
			struct al_mod_eth_mac_regs *mac_regs_base,
			uint32_t reg_addr,
			uint32_t reg_mask,
			uint32_t reg_data)
{
	uint32_t temp;

	temp = al_mod_eth_40g_pcs_reg_read(mac_regs_base, reg_addr);
	al_mod_eth_40g_pcs_reg_write(mac_regs_base, reg_addr, AL_MASK_VAL(reg_mask, reg_data, temp));

	al_mod_dbg("%s: reg %d. val 0x%x\n", __func__, reg_addr, reg_data);
}

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
	case AL_ETH_MAC_MODE_SGMII:
		/* configure and enable the ASYNC FIFO between the MACs and the EC */
		/* TX min packet size */
		al_mod_reg_write32(&mac_regs_base->gen_v3.tx_afifo_cfg_1, 0x00000010);
		/* TX max packet size */
		al_mod_reg_write32(&mac_regs_base->gen_v3.tx_afifo_cfg_2, 0x00002800);
		/* TX input bus configuration */
		al_mod_reg_write32(&mac_regs_base->gen_v3.tx_afifo_cfg_3, 0x00000080);
		/* TX output bus configuration */
		al_mod_reg_write32(&mac_regs_base->gen_v3.tx_afifo_cfg_4, 0x00030020);
		/* TX Valid/ready configuration */
		al_mod_reg_write32(&mac_regs_base->gen_v3.tx_afifo_cfg_5, 0x00000121);
		/* RX input bus configuration */
		al_mod_reg_write32(&mac_regs_base->gen_v3.rx_afifo_cfg_3, 0x00030020);
		/* RX output bus configuration */
		al_mod_reg_write32(&mac_regs_base->gen_v3.rx_afifo_cfg_4, 0x00000080);
		/* RX Valid/ready configuration */
		al_mod_reg_write32(&mac_regs_base->gen_v3.rx_afifo_cfg_5, 0x00000212);
		/* V3 additional MAC selection */
		al_mod_reg_write32(&mac_regs_base->gen_v3.mac_sel, 0x00000000);
		al_mod_reg_write32(&mac_regs_base->gen_v3.mac_10g_ll_cfg, 0x00000001);
		al_mod_reg_write32(&mac_regs_base->gen_v3.mac_10g_ll_ctrl, 0x00000000);
		al_mod_reg_write32(&mac_regs_base->gen_v3.pcs_10g_ll_cfg, 0x00000000);
		/* ASYNC FIFO ENABLE */
		al_mod_reg_write32(&mac_regs_base->gen_v3.afifo_ctrl, 0x00003333);
		/* Timestamp_configuration */
		al_mod_reg_write32(&mac_regs_base->gen_v3.spare,
				ETH_MAC_GEN_V3_SPARE_CHICKEN_DISABLE_TIMESTAMP_STRETCH);

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
		/* configure and enable the ASYNC FIFO between the MACs and the EC */
		/* TX min packet size */
		al_mod_reg_write32(&mac_regs_base->gen_v3.tx_afifo_cfg_1, 0x00000010);
		/* TX max packet size */
		al_mod_reg_write32(&mac_regs_base->gen_v3.tx_afifo_cfg_2, 0x00002800);
		/* TX input bus configuration */
		al_mod_reg_write32(&mac_regs_base->gen_v3.tx_afifo_cfg_3, 0x00000080);
		/* TX output bus configuration */
		al_mod_reg_write32(&mac_regs_base->gen_v3.tx_afifo_cfg_4, 0x00030020);
		/* TX Valid/ready configuration */
		al_mod_reg_write32(&mac_regs_base->gen_v3.tx_afifo_cfg_5, 0x00000023);
		/* RX input bus configuration */
		al_mod_reg_write32(&mac_regs_base->gen_v3.rx_afifo_cfg_3, 0x00030020);
		/* RX output bus configuration */
		al_mod_reg_write32(&mac_regs_base->gen_v3.rx_afifo_cfg_4, 0x00000080);
		/* RX Valid/ready configuration */
		al_mod_reg_write32(&mac_regs_base->gen_v3.rx_afifo_cfg_5, 0x00000012);
		/* V3 additional MAC selection */
		al_mod_reg_write32(&mac_regs_base->gen_v3.mac_sel, 0x00000000);
		al_mod_reg_write32(&mac_regs_base->gen_v3.mac_10g_ll_cfg, 0x00000000);
		al_mod_reg_write32(&mac_regs_base->gen_v3.mac_10g_ll_ctrl, 0x00000000);
		al_mod_reg_write32(&mac_regs_base->gen_v3.pcs_10g_ll_cfg, 0x00000050);
		/* ASYNC FIFO ENABLE */
		al_mod_reg_write32(&mac_regs_base->gen_v3.afifo_ctrl, 0x00003333);

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
		/* configure and enable the ASYNC FIFO between the MACs and the EC */
		/* TX min packet size */
		al_mod_reg_write32(&mac_regs_base->gen_v3.tx_afifo_cfg_1, 0x00000010);
		/* TX max packet size */
		al_mod_reg_write32(&mac_regs_base->gen_v3.tx_afifo_cfg_2, 0x00002800);
		/* TX input bus configuration */
		al_mod_reg_write32(&mac_regs_base->gen_v3.tx_afifo_cfg_3, 0x00000080);
		/* TX output bus configuration */
		al_mod_reg_write32(&mac_regs_base->gen_v3.tx_afifo_cfg_4, 0x00030020);
		/* TX Valid/ready configuration */
		al_mod_reg_write32(&mac_regs_base->gen_v3.tx_afifo_cfg_5, 0x00000023);
		/* RX input bus configuration */
		al_mod_reg_write32(&mac_regs_base->gen_v3.rx_afifo_cfg_3, 0x00030020);
		/* RX output bus configuration */
		al_mod_reg_write32(&mac_regs_base->gen_v3.rx_afifo_cfg_4, 0x00000080);
		/* RX Valid/ready configuration */
		al_mod_reg_write32(&mac_regs_base->gen_v3.rx_afifo_cfg_5, 0x00000012);
		/* V3 additional MAC selection */
		al_mod_reg_write32(&mac_regs_base->gen_v3.mac_sel, 0x00000000);
		al_mod_reg_write32(&mac_regs_base->gen_v3.mac_10g_ll_cfg, 0x00000000);
		al_mod_reg_write32(&mac_regs_base->gen_v3.mac_10g_ll_ctrl, 0x00000000);
		al_mod_reg_write32(&mac_regs_base->gen_v3.pcs_10g_ll_cfg, 0x00000050);
		/* ASYNC FIFO ENABLE */
		al_mod_reg_write32(&mac_regs_base->gen_v3.afifo_ctrl, 0x00003333);

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
	case AL_ETH_MAC_MODE_KR_LL_25G:
		/* select 25G SERDES lane 0 and lane 1 */
		al_mod_reg_write32(&mac_regs_base->gen_v3.ext_serdes_ctrl, 0x0002110f);

		/* configure and enable the ASYNC FIFO between the MACs and the EC */
		/* TX min packet size */
		al_mod_reg_write32(&mac_regs_base->gen_v3.tx_afifo_cfg_1, 0x00000010);
		/* TX max packet size */
		al_mod_reg_write32(&mac_regs_base->gen_v3.tx_afifo_cfg_2, 0x00002800);
		/* TX input bus configuration */
		al_mod_reg_write32(&mac_regs_base->gen_v3.tx_afifo_cfg_3, 0x00000080);
		/* TX output bus configuration */
		al_mod_reg_write32(&mac_regs_base->gen_v3.tx_afifo_cfg_4, 0x00030020);
		/* TX Valid/ready configuration */
		al_mod_reg_write32(&mac_regs_base->gen_v3.tx_afifo_cfg_5, 0x00000023);
		/* RX input bus configuration */
		al_mod_reg_write32(&mac_regs_base->gen_v3.rx_afifo_cfg_3, 0x00030020);
		/* RX output bus configuration */
		al_mod_reg_write32(&mac_regs_base->gen_v3.rx_afifo_cfg_4, 0x00000080);
		/* RX Valid/ready configuration */
		al_mod_reg_write32(&mac_regs_base->gen_v3.rx_afifo_cfg_5, 0x00000012);

		/* V3 additional MAC selection */
		al_mod_reg_write32(&mac_regs_base->gen_v3.mac_sel, 0x00000000);
		al_mod_reg_write32(&mac_regs_base->gen_v3.mac_10g_ll_cfg, 0x00000000);
		al_mod_reg_write32(&mac_regs_base->gen_v3.mac_10g_ll_ctrl, 0x00000000);
		al_mod_reg_write32(&mac_regs_base->gen_v3.pcs_10g_ll_cfg, 0x000000a0);
		/* ASYNC FIFO ENABLE */
		al_mod_reg_write32(&mac_regs_base->gen_v3.afifo_ctrl, 0x00003333);

		/* MAC register file */
		al_mod_reg_write32(&mac_regs_base->mac_10g.cmd_cfg, 0x01022810);
		/* XAUI MAC control register */
		al_mod_reg_write32(&mac_regs_base->gen.cfg, 0x00000005);
		/* RXAUI MAC control register */
		al_mod_reg_write32(&mac_regs_base->gen.rxaui_cfg, 0x00000007);
		al_mod_reg_write32(&mac_regs_base->gen.sd_cfg, 0x000001F1);
		al_mod_reg_write32(&mac_regs_base->gen.xgmii_dfifo_32_64, 0x00000401);
		al_mod_reg_write32(&mac_regs_base->gen.xgmii_dfifo_64_32, 0x00000401);

		if (obj->serdes_lane == 0)
			al_mod_reg_write32_masked(&mac_regs_base->gen.mux_sel,
					~ETH_MAC_GEN_MUX_SEL_KR_IN_MASK, 0x00073910);
		else
			al_mod_reg_write32(&mac_regs_base->gen.mux_sel, 0x00077910);

		if (obj->serdes_lane == 0)
			al_mod_reg_write32(&mac_regs_base->gen.clk_cfg, 0x10003210);
		else
			al_mod_reg_write32(&mac_regs_base->gen.clk_cfg, 0x10000101);

		al_mod_reg_write32(&mac_regs_base->gen.sd_fifo_ctrl, 0x000004f0);
		al_mod_reg_write32(&mac_regs_base->gen.sd_fifo_ctrl, 0x00000401);

		al_mod_reg_write32_masked(&mac_regs_base->gen.led_cfg,
				      ETH_MAC_GEN_LED_CFG_SEL_MASK,
				      ETH_MAC_GEN_LED_CFG_SEL_DEFAULT_REG);

		if (obj->serdes_lane == 1)
			al_mod_reg_write32(&mac_regs_base->gen.los_sel, 0x101);
		break;
	case AL_ETH_MAC_MODE_XLG_LL_40G:
		/* configure and enable the ASYNC FIFO between the MACs and the EC */
		/* TX min packet size */
		al_mod_reg_write32(&mac_regs_base->gen_v3.tx_afifo_cfg_1, 0x00000010);
		/* TX max packet size */
		al_mod_reg_write32(&mac_regs_base->gen_v3.tx_afifo_cfg_2, 0x00002800);
		/* TX input bus configuration */
		al_mod_reg_write32(&mac_regs_base->gen_v3.tx_afifo_cfg_3, 0x00000080);
		/* TX output bus configuration */
		al_mod_reg_write32(&mac_regs_base->gen_v3.tx_afifo_cfg_4, 0x00010040);
		/* TX Valid/ready configuration */
		al_mod_reg_write32(&mac_regs_base->gen_v3.tx_afifo_cfg_5, 0x00000023);
		/* RX input bus configuration */
		al_mod_reg_write32(&mac_regs_base->gen_v3.rx_afifo_cfg_3, 0x00010040);
		/* RX output bus configuration */
		al_mod_reg_write32(&mac_regs_base->gen_v3.rx_afifo_cfg_4, 0x00000080);
		/* RX Valid/ready configuration */
		al_mod_reg_write32(&mac_regs_base->gen_v3.rx_afifo_cfg_5, 0x00000112);

		/* V3 additional MAC selection */
		al_mod_reg_write32(&mac_regs_base->gen_v3.mac_sel, 0x00000010);
		al_mod_reg_write32(&mac_regs_base->gen_v3.mac_10g_ll_cfg, 0x00000000);
		al_mod_reg_write32(&mac_regs_base->gen_v3.mac_10g_ll_ctrl, 0x00000000);
		al_mod_reg_write32(&mac_regs_base->gen_v3.pcs_10g_ll_cfg, 0x00000000);
		/* ASYNC FIFO ENABLE */
		al_mod_reg_write32(&mac_regs_base->gen_v3.afifo_ctrl, 0x00003333);

		/* cmd_cfg */
		al_mod_reg_write32(&mac_regs_base->gen_v3.mac_40g_ll_addr, 0x00000008);
		al_mod_reg_write32(&mac_regs_base->gen_v3.mac_40g_ll_data, 0x01022810);

#if (defined(AL_ETH_FAST_AN)) && (AL_ETH_FAST_AN == 1)
		al_mod_eth_40g_pcs_reg_write(mac_regs_base, 0x00010004, 1023);
		al_mod_eth_40g_pcs_reg_write(mac_regs_base, 0x00000000, 0xA04c);
		al_mod_eth_40g_pcs_reg_write(mac_regs_base, 0x00000000, 0x204c);

#endif

		/* XAUI MAC control register */
		al_mod_reg_write32_masked(&mac_regs_base->gen.mux_sel,
					~ETH_MAC_GEN_MUX_SEL_KR_IN_MASK, 0x06883910);
		al_mod_reg_write32(&mac_regs_base->gen.sd_fifo_ctrl, 0x0000040f);

		/* MAC register file */
		/* XAUI MAC control register */
		al_mod_reg_write32(&mac_regs_base->gen.cfg, 0x00000005);
		/* RXAUI MAC control register */
		al_mod_reg_write32(&mac_regs_base->gen.rxaui_cfg, 0x00000007);
		al_mod_reg_write32(&mac_regs_base->gen.sd_cfg, 0x000001F1);
		al_mod_reg_write32(&mac_regs_base->gen.xgmii_dfifo_32_64, 0x00000401);
		al_mod_reg_write32(&mac_regs_base->gen.xgmii_dfifo_64_32, 0x00000401);
		al_mod_reg_write32(&mac_regs_base->gen.clk_cfg, 0x10003210);

		al_mod_reg_write32_masked(&mac_regs_base->gen.led_cfg,
				      ETH_MAC_GEN_LED_CFG_SEL_MASK,
				      ETH_MAC_GEN_LED_CFG_SEL_DEFAULT_REG);
		break;
	case AL_ETH_MAC_MODE_XLG_LL_25G:
		/* xgmii_mode: 0=xlgmii, 1=xgmii */
		al_mod_reg_write32(&mac_regs_base->gen_v3.mac_40g_ll_addr, 0x0080);
		al_mod_reg_write32(&mac_regs_base->gen_v3.mac_40g_ll_data, 0x00000001);

		/* configure and enable the ASYNC FIFO between the MACs and the EC */
		/* TX min packet size */
		al_mod_reg_write32(&mac_regs_base->gen_v3.tx_afifo_cfg_1, 0x00000010);
		/* TX max packet size */
		al_mod_reg_write32(&mac_regs_base->gen_v3.tx_afifo_cfg_2, 0x00002800);
		/* TX input bus configuration */
		al_mod_reg_write32(&mac_regs_base->gen_v3.tx_afifo_cfg_3, 0x00000080);
		/* TX output bus configuration */
		al_mod_reg_write32(&mac_regs_base->gen_v3.tx_afifo_cfg_4, 0x00010040);
		/* TX Valid/ready configuration */
		al_mod_reg_write32(&mac_regs_base->gen_v3.tx_afifo_cfg_5, 0x00000023);
		/* RX input bus configuration */
		al_mod_reg_write32(&mac_regs_base->gen_v3.rx_afifo_cfg_3, 0x00010040);
		/* RX output bus configuration */
		al_mod_reg_write32(&mac_regs_base->gen_v3.rx_afifo_cfg_4, 0x00000080);
		/* RX Valid/ready configuration */
		al_mod_reg_write32(&mac_regs_base->gen_v3.rx_afifo_cfg_5, 0x00000112);
		/* V3 additional MAC selection */
		al_mod_reg_write32(&mac_regs_base->gen_v3.mac_sel, 0x00000010);
		al_mod_reg_write32(&mac_regs_base->gen_v3.mac_10g_ll_cfg, 0x00000000);
		al_mod_reg_write32(&mac_regs_base->gen_v3.mac_10g_ll_ctrl, 0x00000000);
		al_mod_reg_write32(&mac_regs_base->gen_v3.pcs_10g_ll_cfg, 0x00000000);
		/* ASYNC FIFO ENABLE */
		al_mod_reg_write32(&mac_regs_base->gen_v3.afifo_ctrl, 0x00003333);

		/* cmd_cfg */
		al_mod_reg_write32(&mac_regs_base->gen_v3.mac_40g_ll_addr, 0x00000008);
		al_mod_reg_write32(&mac_regs_base->gen_v3.mac_40g_ll_data, 0x01022810);

		/* select the 25G serdes for lanes 0/1 */
		al_mod_reg_write32(&mac_regs_base->gen_v3.ext_serdes_ctrl, 0x0002110f);
		/* configure the PCS to work with 2 lanes */
		/* configure which two of the 4 PCS Lanes (VL) are combined to one RXLAUI lane */
		/* use VL 0-2 for RXLAUI lane 0, use VL 1-3 for RXLAUI lane 1 */
		al_mod_eth_40g_pcs_reg_write(mac_regs_base, 0x00010008, 0x0d80);
		/* configure the PCS to work 32 bit interface */
		al_mod_reg_write32(&mac_regs_base->gen_v3.pcs_40g_ll_cfg, 0x00440000);

		/* disable MLD and move to clause 49 PCS: */
		al_mod_reg_write32(&mac_regs_base->gen_v3.pcs_40g_ll_addr, 0xE);
		al_mod_reg_write32(&mac_regs_base->gen_v3.pcs_40g_ll_data, 0);

#if (defined(AL_ETH_FAST_AN)) && (AL_ETH_FAST_AN == 1)
		al_mod_eth_40g_pcs_reg_write(mac_regs_base, 0x00010004, 1023);
		al_mod_eth_40g_pcs_reg_write(mac_regs_base, 0x00000000, 0xA04c);
		al_mod_eth_40g_pcs_reg_write(mac_regs_base, 0x00000000, 0x204c);
#endif

		/* XAUI MAC control register */
		if (obj->serdes_lane == 0)
			al_mod_reg_write32_masked(&mac_regs_base->gen.mux_sel,
					      ~ETH_MAC_GEN_MUX_SEL_KR_IN_MASK, 0x06883910);
		else
			al_mod_reg_write32(&mac_regs_base->gen.mux_sel, 0x06803950);

		al_mod_reg_write32(&mac_regs_base->gen.sd_fifo_ctrl, 0x0000040f);

		/* XAUI MAC control register */
		al_mod_reg_write32(&mac_regs_base->gen.cfg, 0x00000005);
		/* RXAUI MAC control register */
		al_mod_reg_write32(&mac_regs_base->gen.rxaui_cfg, 0x00000007);
		al_mod_reg_write32(&mac_regs_base->gen.sd_cfg, 0x000001F1);
		al_mod_reg_write32(&mac_regs_base->gen.xgmii_dfifo_32_64, 0x00000401);
		al_mod_reg_write32(&mac_regs_base->gen.xgmii_dfifo_64_32, 0x00000401);
		if (obj->serdes_lane == 0)
			al_mod_reg_write32(&mac_regs_base->gen.clk_cfg, 0x10003210);
		else
			al_mod_reg_write32(&mac_regs_base->gen.clk_cfg, 0x10000101);

		al_mod_reg_write32_masked(&mac_regs_base->gen.led_cfg,
					ETH_MAC_GEN_LED_CFG_SEL_MASK,
					ETH_MAC_GEN_LED_CFG_SEL_DEFAULT_REG);

		if (obj->serdes_lane == 1)
			al_mod_reg_write32(&mac_regs_base->gen.los_sel, 0x101);

		break;
	case AL_ETH_MAC_MODE_XLG_LL_50G:
		/* configure and enable the ASYNC FIFO between the MACs and the EC */
		/* TX min packet size */
		al_mod_reg_write32(&mac_regs_base->gen_v3.tx_afifo_cfg_1, 0x00000010);
		/* TX max packet size */
		al_mod_reg_write32(&mac_regs_base->gen_v3.tx_afifo_cfg_2, 0x00002800);
		/* TX input bus configuration */
		al_mod_reg_write32(&mac_regs_base->gen_v3.tx_afifo_cfg_3, 0x00000080);
		/* TX output bus configuration */
		al_mod_reg_write32(&mac_regs_base->gen_v3.tx_afifo_cfg_4, 0x00010040);
		/* TX Valid/ready configuration */
		al_mod_reg_write32(&mac_regs_base->gen_v3.tx_afifo_cfg_5, 0x00000023);
		/* RX input bus configuration */
		al_mod_reg_write32(&mac_regs_base->gen_v3.rx_afifo_cfg_3, 0x00010040);
		/* RX output bus configuration */
		al_mod_reg_write32(&mac_regs_base->gen_v3.rx_afifo_cfg_4, 0x00000080);
		/* RX Valid/ready configuration */
		al_mod_reg_write32(&mac_regs_base->gen_v3.rx_afifo_cfg_5, 0x00000112);

		/* V3 additional MAC selection */
		al_mod_reg_write32(&mac_regs_base->gen_v3.mac_sel, 0x00000010);
		al_mod_reg_write32(&mac_regs_base->gen_v3.mac_10g_ll_cfg, 0x00000000);
		al_mod_reg_write32(&mac_regs_base->gen_v3.mac_10g_ll_ctrl, 0x00000000);
		al_mod_reg_write32(&mac_regs_base->gen_v3.pcs_10g_ll_cfg, 0x00000000);
		/* ASYNC FIFO ENABLE */
		al_mod_reg_write32(&mac_regs_base->gen_v3.afifo_ctrl, 0x00003333);

		/* cmd_cfg */
		al_mod_reg_write32(&mac_regs_base->gen_v3.mac_40g_ll_addr, 0x00000008);
		al_mod_reg_write32(&mac_regs_base->gen_v3.mac_40g_ll_data, 0x01022810);

		/* select the 25G serdes for lanes 0/1 */
		al_mod_reg_write32(&mac_regs_base->gen_v3.ext_serdes_ctrl, 0x0382110F);
		/* configure the PCS to work with 2 lanes */
		/* configure which two of the 4 PCS Lanes (VL) are combined to one RXLAUI lane */
		/* use VL 0-2 for RXLAUI lane 0, use VL 1-3 for RXLAUI lane 1 */
		al_mod_eth_40g_pcs_reg_write(mac_regs_base, 0x00010008, 0x0d81);
		/* configure the PCS to work 32 bit interface */
		al_mod_reg_write32(&mac_regs_base->gen_v3.pcs_40g_ll_cfg, 0x00440000);


#if (defined(AL_ETH_FAST_AN)) && (AL_ETH_FAST_AN == 1)
		al_mod_eth_40g_pcs_reg_write(mac_regs_base, 0x00010004, 1023);
		al_mod_eth_40g_pcs_reg_write(mac_regs_base, 0x00000000, 0xA04c);
		al_mod_eth_40g_pcs_reg_write(mac_regs_base, 0x00000000, 0x204c);
#endif

		/* XAUI MAC control register */
		al_mod_reg_write32_masked(&mac_regs_base->gen.mux_sel,
			~ETH_MAC_GEN_MUX_SEL_KR_IN_MASK, 0x06883910);
		al_mod_reg_write32(&mac_regs_base->gen.sd_fifo_ctrl, 0x0000040f);

		/* MAC register file */
		/* XAUI MAC control register */
		al_mod_reg_write32(&mac_regs_base->gen.cfg, 0x00000005);
		/* RXAUI MAC control register */
		al_mod_reg_write32(&mac_regs_base->gen.rxaui_cfg, 0x00000007);
		al_mod_reg_write32(&mac_regs_base->gen.sd_cfg, 0x000001F1);
		al_mod_reg_write32(&mac_regs_base->gen.xgmii_dfifo_32_64, 0x00000401);
		al_mod_reg_write32(&mac_regs_base->gen.xgmii_dfifo_64_32, 0x00000401);
		al_mod_reg_write32(&mac_regs_base->gen.clk_cfg, 0x10001010);

		al_mod_reg_write32_masked(&mac_regs_base->gen.led_cfg,
				      ETH_MAC_GEN_LED_CFG_SEL_MASK,
				      ETH_MAC_GEN_LED_CFG_SEL_DEFAULT_REG);
		break;


	default:
		return -EPERM;
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
	uint32_t data = 0;

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
	case AL_ETH_MAC_MODE_KR_LL_25G:
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
	case AL_ETH_MAC_MODE_XLG_LL_40G:
	case AL_ETH_MAC_MODE_XLG_LL_50G:
	case AL_ETH_MAC_MODE_XLG_LL_25G:
		cmd_cfg = al_mod_eth_40g_mac_reg_read(mac_regs_base,
				ETH_MAC_GEN_V3_MAC_40G_COMMAND_CONFIG_ADDR);

		if (tx) {
			mask |= ETH_MAC_GEN_V3_MAC_40G_COMMAND_CONFIG_TX_ENA;
			if (tx_start)
				data |= ETH_MAC_GEN_V3_MAC_40G_COMMAND_CONFIG_TX_ENA;
		}

		if (rx) {
			mask |= ETH_MAC_GEN_V3_MAC_40G_COMMAND_CONFIG_RX_ENA;
			if (rx_start)
				data |= ETH_MAC_GEN_V3_MAC_40G_COMMAND_CONFIG_RX_ENA;
		}

		cmd_cfg = AL_MASK_VAL(mask, data, cmd_cfg);

		al_mod_eth_40g_mac_reg_write(mac_regs_base,
				ETH_MAC_GEN_V3_MAC_40G_COMMAND_CONFIG_ADDR,
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
	case AL_ETH_MAC_MODE_SGMII:
		al_mod_reg_write32_masked(&mac_regs_base->mac_1g.cmd_cfg, AL_BIT(15),
			(enable ? AL_BIT(15) : 0));
		break;
	case AL_ETH_MAC_MODE_SGMII_2_5G:
	case AL_ETH_MAC_MODE_10GbE_Serial:
	case AL_ETH_MAC_MODE_KR_LL_25G:
		al_mod_eth_kr_pcs_reg_write_masked(mac_regs_base, ETH_MAC_KR_PCS_CONTROL_1_ADDR,
			AL_BIT(14), (enable ? AL_BIT(14) : 0));
		break;
	case AL_ETH_MAC_MODE_XLG_LL_40G:
	case AL_ETH_MAC_MODE_XLG_LL_50G:
	case AL_ETH_MAC_MODE_XLG_LL_25G:
		al_mod_eth_40g_pcs_reg_write_masked(mac_regs_base,
			ETH_MAC_GEN_V3_PCS_40G_CONTROL_STATUS_ADDR,
			AL_BIT(14), (enable ? AL_BIT(14) : 0));
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static al_mod_bool mac_loopback_get(struct al_mod_eth_mac_obj *obj)
{
	struct al_mod_eth_mac_regs *mac_regs_base = obj->mac_regs_base;
	uint32_t reg;

	switch (obj->mac_mode) {
	case AL_ETH_MAC_MODE_SGMII:
		reg = al_mod_reg_read32(&mac_regs_base->mac_1g.cmd_cfg);
		return !!(reg & AL_BIT(15));
	case AL_ETH_MAC_MODE_SGMII_2_5G:
	case AL_ETH_MAC_MODE_10GbE_Serial:
	case AL_ETH_MAC_MODE_KR_LL_25G:
		reg = al_mod_eth_kr_pcs_reg_read(mac_regs_base, ETH_MAC_KR_PCS_CONTROL_1_ADDR);
		return !!(reg & AL_BIT(14));
	case AL_ETH_MAC_MODE_XLG_LL_40G:
	case AL_ETH_MAC_MODE_XLG_LL_50G:
	case AL_ETH_MAC_MODE_XLG_LL_25G:
		reg = al_mod_eth_40g_pcs_reg_read(mac_regs_base,
			ETH_MAC_GEN_V3_PCS_40G_CONTROL_STATUS_ADDR);
		return !!(reg & AL_BIT(14));
	default:
		break;
	}

	return AL_FALSE;
}

static int mac_stats_get(struct al_mod_eth_mac_obj *obj, struct al_mod_eth_mac_stats *stats)
{
	struct al_mod_eth_mac_regs *mac_regs_base = obj->mac_regs_base;
	struct al_mod_eth_mac_1g_stats __iomem *stats_1g = &mac_regs_base->mac_1g.stats;
	struct al_mod_eth_mac_10g_stats_v3_rx __iomem *stats_10g_rx =
		&mac_regs_base->mac_10g.stats.v3.rx;
	struct al_mod_eth_mac_10g_stats_v3_tx __iomem *stats_10g_tx =
		&mac_regs_base->mac_10g.stats.v3.tx;
	uint64_t octets;

	al_mod_assert(stats);

	al_mod_memset(stats, 0, sizeof(struct al_mod_eth_mac_stats));

	switch (obj->mac_mode) {
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
	case AL_ETH_MAC_MODE_KR_LL_25G:
		stats->if_in_ucast_pkts = al_mod_reg_read32(&stats_10g_rx->if_in_ucast_pkts);
		stats->if_in_mcast_pkts = al_mod_reg_read32(&stats_10g_rx->if_in_mcast_pkts);
		stats->if_in_bcast_pkts = al_mod_reg_read32(&stats_10g_rx->if_in_bcast_pkts);
		stats->pkts = al_mod_reg_read32(&stats_10g_rx->pkts);
		stats->if_out_ucast_pkts = al_mod_reg_read32(&stats_10g_tx->if_ucast_pkts);
		stats->if_out_mcast_pkts = al_mod_reg_read32(&stats_10g_tx->if_multicast_pkts);
		stats->if_out_bcast_pkts = al_mod_reg_read32(&stats_10g_tx->if_broadcast_pkts);
		stats->if_in_errors = al_mod_reg_read32(&stats_10g_rx->if_in_errors);
		stats->if_out_errors = al_mod_reg_read32(&stats_10g_tx->if_out_errors);
		stats->frames_rxed_ok = al_mod_reg_read32(&stats_10g_rx->frames_ok);
		stats->frames_txed_ok = al_mod_reg_read32(&stats_10g_tx->frames_ok);

		/* octets_rxed_ok = if_in_octets - 18 * frames_rxed_ok - 4 * vlan_rxed_ok */
		octets = al_mod_reg_read32(&stats_10g_rx->if_octets_l);
		octets |= (uint64_t)(al_mod_reg_read32(&stats_10g_rx->if_octets_h)) << 32;
		octets -= 18 * stats->frames_rxed_ok;
		octets -= 4 * al_mod_reg_read32(&stats_10g_rx->vlan_ok);
		stats->octets_rxed_ok = octets;

		/* octets_txed_ok = if_out_octets - 18 * frames_txed_ok
		 * - 4 * vlan_txed_ok */
		octets = al_mod_reg_read32(&stats_10g_tx->if_octets_l);
		octets |= (uint64_t)(al_mod_reg_read32(&stats_10g_tx->if_octets_h)) << 32;
		octets -= 18 * stats->frames_txed_ok;
		octets -= 4 * al_mod_reg_read32(&stats_10g_tx->vlan_ok);
		stats->octets_txed_ok = octets;

		stats->undersize_pkts =
			al_mod_reg_read32(&stats_10g_rx->undersize_pkts);
		stats->fragments = al_mod_reg_read32(&stats_10g_rx->fragments);
		stats->jabbers = al_mod_reg_read32(&stats_10g_rx->jabbers);
		stats->oversize_pkts =
			al_mod_reg_read32(&stats_10g_rx->oversize_pkts);
		stats->fcs_errors = al_mod_reg_read32(&stats_10g_rx->crc_errs);
		stats->alignment_errors = al_mod_reg_read32(&stats_10g_rx->alignment_errors);
		stats->drop_events = al_mod_reg_read32(&stats_10g_rx->drop_events);
		stats->pause_mac_ctrl_frames_txed =
			al_mod_reg_read32(&stats_10g_tx->pause_mac_ctrl_frames);
		stats->pause_mac_ctrl_frames_rxed =
			al_mod_reg_read32(&stats_10g_rx->pause_mac_ctrl_frames);
		stats->frame_too_long_errs = al_mod_reg_read32(&stats_10g_rx->aFrameTooLong);
		stats->in_range_length_errs = al_mod_reg_read32(&stats_10g_rx->in_range_length_errs);
		stats->vlan_txed_ok = al_mod_reg_read32(&stats_10g_tx->vlan_ok);
		stats->vlan_rxed_ok = al_mod_reg_read32(&stats_10g_rx->vlan_ok);
		stats->octets = al_mod_reg_read32(&stats_10g_rx->octets);
		stats->pkts_64_octets =
			al_mod_reg_read32(&stats_10g_rx->pkts_64_octets);
		stats->pkts_65_to_127_octets =
			al_mod_reg_read32(&stats_10g_rx->pkts_65_to_127_octets);
		stats->pkts_128_to_255_octets =
			al_mod_reg_read32(&stats_10g_rx->pkts_128_to_255_octets);
		stats->pkts_256_to_511_octets =
			al_mod_reg_read32(&stats_10g_rx->pkts_256_to_511_octets);
		stats->pkts_512_to_1023_octets =
			al_mod_reg_read32(&stats_10g_rx->pkts_512_to_1023_octets);
		stats->pkts_1024_to_1518_octets =
			al_mod_reg_read32(&stats_10g_rx->pkts_1024_to_1518_octets);
		stats->pkts_1519_to_x =
			al_mod_reg_read32(&stats_10g_rx->pkts1519toMax);
		break;
	case AL_ETH_MAC_MODE_XLG_LL_40G:
	case AL_ETH_MAC_MODE_XLG_LL_50G:
	case AL_ETH_MAC_MODE_XLG_LL_25G:
		/* 40G MAC statistics registers are the same, only read indirectly */
		#define _40g_mac_reg_read32(field)	al_mod_eth_40g_mac_reg_read(mac_regs_base,	\
			((uint8_t *)(field)) - ((uint8_t *)&mac_regs_base->mac_10g))

		stats->if_in_ucast_pkts = _40g_mac_reg_read32(&stats_10g_rx->if_in_ucast_pkts);
		stats->if_in_mcast_pkts = _40g_mac_reg_read32(&stats_10g_rx->if_in_mcast_pkts);
		stats->if_in_bcast_pkts = _40g_mac_reg_read32(&stats_10g_rx->if_in_bcast_pkts);
		stats->pkts = _40g_mac_reg_read32(&stats_10g_rx->pkts);
		stats->if_out_ucast_pkts = _40g_mac_reg_read32(&stats_10g_tx->if_ucast_pkts);
		stats->if_out_mcast_pkts = _40g_mac_reg_read32(&stats_10g_tx->if_multicast_pkts);
		stats->if_out_bcast_pkts = _40g_mac_reg_read32(&stats_10g_tx->if_broadcast_pkts);
		stats->if_in_errors = _40g_mac_reg_read32(&stats_10g_rx->if_in_errors);
		stats->if_out_errors = _40g_mac_reg_read32(&stats_10g_tx->if_out_errors);
		stats->frames_rxed_ok = _40g_mac_reg_read32(&stats_10g_rx->frames_ok);
		stats->frames_txed_ok = _40g_mac_reg_read32(&stats_10g_tx->frames_ok);

		/* octets_rxed_ok = if_in_octets - 18 * frames_rxed_ok - 4 * vlan_rxed_ok */
		octets = _40g_mac_reg_read32(&stats_10g_rx->if_octets_l);
		octets |= (uint64_t)(_40g_mac_reg_read32(&stats_10g_rx->if_octets_h)) << 32;
		octets -= 18 * stats->frames_rxed_ok;
		octets -= 4 * _40g_mac_reg_read32(&stats_10g_rx->vlan_ok);
		stats->octets_rxed_ok = octets;

		/* octets_txed_ok = if_out_octets - 18 * frames_txed_ok
		 * - 4 * vlan_txed_ok */
		octets = _40g_mac_reg_read32(&stats_10g_tx->if_octets_l);
		octets |= (uint64_t)(_40g_mac_reg_read32(&stats_10g_tx->if_octets_h)) << 32;
		octets -= 18 * stats->frames_txed_ok;
		octets -= 4 * _40g_mac_reg_read32(&stats_10g_tx->vlan_ok);
		stats->octets_txed_ok = octets;

		stats->undersize_pkts =
			_40g_mac_reg_read32(&stats_10g_rx->undersize_pkts);
		stats->fragments =
			_40g_mac_reg_read32(&stats_10g_rx->fragments);
		stats->jabbers = _40g_mac_reg_read32(&stats_10g_rx->jabbers);
		stats->oversize_pkts =
			_40g_mac_reg_read32(&stats_10g_rx->oversize_pkts);
		stats->fcs_errors = _40g_mac_reg_read32(&stats_10g_rx->crc_errs);
		stats->alignment_errors = _40g_mac_reg_read32(&stats_10g_rx->alignment_errors);
		stats->drop_events =
			_40g_mac_reg_read32(&stats_10g_rx->drop_events);
		stats->pause_mac_ctrl_frames_txed =
			_40g_mac_reg_read32(&stats_10g_tx->pause_mac_ctrl_frames);
		stats->pause_mac_ctrl_frames_rxed =
			_40g_mac_reg_read32(&stats_10g_rx->pause_mac_ctrl_frames);
		stats->frame_too_long_errs = _40g_mac_reg_read32(&stats_10g_rx->aFrameTooLong);
		stats->in_range_length_errs =
			_40g_mac_reg_read32(&stats_10g_rx->in_range_length_errs);
		stats->vlan_txed_ok = _40g_mac_reg_read32(&stats_10g_tx->vlan_ok);
		stats->vlan_rxed_ok = _40g_mac_reg_read32(&stats_10g_rx->vlan_ok);
		stats->octets = _40g_mac_reg_read32(&stats_10g_rx->octets);
		stats->pkts_64_octets =
			_40g_mac_reg_read32(&stats_10g_rx->pkts_64_octets);
		stats->pkts_65_to_127_octets =
			_40g_mac_reg_read32(&stats_10g_rx->pkts_65_to_127_octets);
		stats->pkts_128_to_255_octets =
			_40g_mac_reg_read32(&stats_10g_rx->pkts_128_to_255_octets);
		stats->pkts_256_to_511_octets =
			_40g_mac_reg_read32(&stats_10g_rx->pkts_256_to_511_octets);
		stats->pkts_512_to_1023_octets =
			_40g_mac_reg_read32(&stats_10g_rx->pkts_512_to_1023_octets);
		stats->pkts_1024_to_1518_octets =
			_40g_mac_reg_read32(&stats_10g_rx->pkts_1024_to_1518_octets);
		stats->pkts_1519_to_x =
			_40g_mac_reg_read32(&stats_10g_rx->pkts1519toMax);

		break;
	default:
		return -EPERM;
	}

	stats->eee_in = al_mod_reg_read32(&mac_regs_base->stat.eee_in);
	stats->eee_out = al_mod_reg_read32(&mac_regs_base->stat.eee_out);

	return 0;
}

static int fec_enable(struct al_mod_eth_mac_obj *obj, enum al_mod_eth_fec_type type, al_mod_bool enable)
{
	struct al_mod_eth_mac_regs *mac_regs_base = obj->mac_regs_base;

	switch (type) {
	case AL_ETH_FEC_TYPE_CLAUSE_74:
		al_mod_reg_write32_masked(&mac_regs_base->gen_v3.pcs_10g_ll_cfg,
			(ETH_MAC_GEN_V3_PCS_10G_LL_CFG_FEC_EN_RX |
				ETH_MAC_GEN_V3_PCS_10G_LL_CFG_FEC_EN_TX),
				(enable ? (ETH_MAC_GEN_V3_PCS_10G_LL_CFG_FEC_EN_RX |
					ETH_MAC_GEN_V3_PCS_10G_LL_CFG_FEC_EN_TX) : 0));
		break;
	default:
		return -EPERM;
	}

	return 0;
}

static al_mod_bool fec_is_enabled(struct al_mod_eth_mac_obj *obj)
{
	struct al_mod_eth_mac_regs *mac_regs_base = obj->mac_regs_base;
	uint32_t reg;

	reg = al_mod_reg_read32(&mac_regs_base->gen_v3.pcs_10g_ll_cfg);
	if (!(reg & ETH_MAC_GEN_V3_PCS_10G_LL_CFG_FEC_EN_RX) ||
	    !(reg & ETH_MAC_GEN_V3_PCS_10G_LL_CFG_FEC_EN_TX))
		return AL_FALSE;

	return AL_TRUE;
}

static int fec_stats_get(struct al_mod_eth_mac_obj *obj, uint32_t *corrected, uint32_t *uncorrectable)
{
	struct al_mod_eth_mac_regs *mac_regs_base = obj->mac_regs_base;

	*corrected = al_mod_reg_read32(&mac_regs_base->stat.v3_pcs_10g_ll_cerr);
	*uncorrectable = al_mod_reg_read32(&mac_regs_base->stat.v3_pcs_10g_ll_ncerr);

	return 0;
}

static int gearbox_reset(struct al_mod_eth_mac_obj *obj, al_mod_bool tx_reset, al_mod_bool rx_reset)
{
	struct al_mod_eth_mac_regs *mac_regs_base = obj->mac_regs_base;
	uint32_t reg, orig_val;

	orig_val = al_mod_reg_read32(&mac_regs_base->gen_v3.ext_serdes_ctrl);
	reg = orig_val;

	if (tx_reset) {
		reg |= (ETH_MAC_GEN_V3_EXT_SERDES_CTRL_LANE_0_TX_25_GS_SW_RESET |
			ETH_MAC_GEN_V3_EXT_SERDES_CTRL_LANE_1_TX_25_GS_SW_RESET);
	}

	if (rx_reset) {
		reg |= (ETH_MAC_GEN_V3_EXT_SERDES_CTRL_LANE_0_RX_25_GS_SW_RESET |
			ETH_MAC_GEN_V3_EXT_SERDES_CTRL_LANE_1_RX_25_GS_SW_RESET);
	}

	al_mod_dbg("%s: perform gearbox reset (Tx %d, Rx %d)\n", __func__, tx_reset, rx_reset);
	al_mod_reg_write32(&mac_regs_base->gen_v3.ext_serdes_ctrl, reg);

	al_mod_udelay(10);

	al_mod_reg_write32(&mac_regs_base->gen_v3.ext_serdes_ctrl, orig_val);

	return 0;
}

static int link_status_get(struct al_mod_eth_mac_obj *obj,
			   struct al_mod_eth_link_status *status)
{
	uint32_t reg;
	struct al_mod_eth_mac_regs *mac_regs_base = obj->mac_regs_base;
	uint32_t mask;

	switch (obj->mac_mode) {
	case AL_ETH_MAC_MODE_10GbE_Serial:
	case AL_ETH_MAC_MODE_10G_SGMII:
	case AL_ETH_MAC_MODE_SGMII_2_5G:
	case AL_ETH_MAC_MODE_KR_LL_25G:
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

	case AL_ETH_MAC_MODE_XLG_LL_25G:
	case AL_ETH_MAC_MODE_XLG_LL_40G:
	case AL_ETH_MAC_MODE_XLG_LL_50G:

		status->link_up = AL_FALSE;
		status->local_fault = AL_TRUE;
		status->remote_fault = AL_TRUE;

		reg = al_mod_reg_read32(&mac_regs_base->gen_v3.pcs_40g_ll_status);

		/* need to check that in PCS each lane is block-locked
		 * (for 40G/50G also that alignment has been achieved)
		 */
		mask = ETH_MAC_GEN_V3_PCS_40G_LL_STATUS_BLOCK_LOCK_MASK;

		if ((obj->mac_mode == AL_ETH_MAC_MODE_XLG_LL_40G) ||
			(obj->mac_mode == AL_ETH_MAC_MODE_XLG_LL_50G))
			mask |= ETH_MAC_GEN_V3_PCS_40G_LL_STATUS_ALIGN_DONE;

		if ((reg & mask) == mask) {
			reg = al_mod_reg_read32(&mac_regs_base->gen_v3.mac_40g_ll_status);

			status->remote_fault = ((reg & ETH_MAC_GEN_V3_MAC_40G_LL_STATUS_REM_FAULT) ?
							AL_TRUE : AL_FALSE);
			status->local_fault = ((reg & ETH_MAC_GEN_V3_MAC_40G_LL_STATUS_LOC_FAULT) ?
							AL_TRUE : AL_FALSE);

			status->link_up = ((status->remote_fault == AL_FALSE) &&
					   (status->local_fault == AL_FALSE));
		}
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
	case AL_ETH_MAC_MODE_KR_LL_25G:
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
static int rx_pkt_limit_config(struct al_mod_eth_mac_obj *obj, unsigned int min_rx_len,
	unsigned int max_rx_len)
{
	struct al_mod_eth_mac_regs *mac_regs_base = obj->mac_regs_base;

	al_mod_reg_write32(&mac_regs_base->gen_v3.rx_afifo_cfg_1, min_rx_len);
	al_mod_reg_write32(&mac_regs_base->gen_v3.rx_afifo_cfg_2, max_rx_len);

	/* configure the MAC's max rx length, add 16 bytes so the packet get
	 * trimmed by the EC/Async_fifo rather by the MAC
	*/
	if (AL_ETH_IS_1G_MAC(obj->mac_mode))
		al_mod_reg_write32(&mac_regs_base->mac_1g.frm_len, max_rx_len + 16);
	else if (AL_ETH_IS_10G_MAC(obj->mac_mode) || AL_ETH_IS_25G_MAC(obj->mac_mode))
		/* 10G MAC control register  */
		al_mod_reg_write32(&mac_regs_base->mac_10g.frm_len, (max_rx_len + 16));
	else
		al_mod_eth_40g_mac_reg_write(mac_regs_base, ETH_MAC_GEN_V3_MAC_40G_FRM_LENGTH_ADDR,
			(max_rx_len + 16));

	return 0;
}

static int eee_config(struct al_mod_eth_mac_obj *obj, struct al_mod_eth_eee_params *params)
{
	struct al_mod_eth_mac_regs *mac_regs_base = obj->mac_regs_base;

	switch (obj->mac_mode) {
	/** Do nothing at SGMII */
	case AL_ETH_MAC_MODE_SGMII:
		break;
	case AL_ETH_MAC_MODE_10GbE_Serial:
	case AL_ETH_MAC_MODE_10G_SGMII:
	case AL_ETH_MAC_MODE_SGMII_2_5G:
		al_mod_reg_write32_masked(
			&mac_regs_base->kr.pcs_cfg,
			ETH_MAC_KR_PCS_CFG_EEE_TIMER_VAL_MASK,
			ETH_MAC_KR_10_PCS_CFG_EEE_TIMER_VAL);
			break;
	case AL_ETH_MAC_MODE_KR_LL_25G:
		al_mod_reg_write32_masked(
			&mac_regs_base->kr.pcs_cfg,
			ETH_MAC_KR_PCS_CFG_EEE_TIMER_VAL_MASK,
			ETH_MAC_KR_25_PCS_CFG_EEE_TIMER_VAL <<
			ETH_MAC_KR_PCS_CFG_EEE_TIMER_VAL_SHIFT);
			break;
	case AL_ETH_MAC_MODE_XLG_LL_40G:
		al_mod_reg_write32_masked(
			&mac_regs_base->gen_v3.pcs_40g_ll_eee_cfg,
			ETH_MAC_GEN_V3_PCS_40G_LL_EEE_CFG_TIMER_VAL_MASK,
			ETH_MAC_XLG_40_PCS_CFG_EEE_TIMER_VAL);
		/* set Deep sleep mode as the LPI function (instead of Fast wake mode) */
		al_mod_eth_40g_pcs_reg_write(mac_regs_base,
			ETH_MAC_GEN_V3_PCS_40G_EEE_CONTROL_ADDR,
			params->fast_wake ? 1 : 0);
			break;
	case AL_ETH_MAC_MODE_XLG_LL_50G:
		al_mod_reg_write32_masked(
			&mac_regs_base->gen_v3.pcs_40g_ll_eee_cfg,
			ETH_MAC_GEN_V3_PCS_40G_LL_EEE_CFG_TIMER_VAL_MASK,
			ETH_MAC_XLG_50_PCS_CFG_EEE_TIMER_VAL <<
			ETH_MAC_GEN_V3_PCS_40G_LL_EEE_CFG_TIMER_VAL_SHIFT);
		/* set Deep sleep mode as the LPI function (instead of Fast wake mode) */
		al_mod_eth_40g_pcs_reg_write(mac_regs_base,
			ETH_MAC_GEN_V3_PCS_40G_EEE_CONTROL_ADDR,
			params->fast_wake ? 1 : 0);
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
	uint32_t reg;

	if (AL_ETH_IS_10G_MAC(obj->mac_mode) || AL_ETH_IS_25G_MAC(obj->mac_mode)) {
		al_mod_reg_write32_masked(&mac_regs_base->mac_10g.cmd_cfg,
			ETH_10G_MAC_CMD_CFG_TX_FLUSH,
			(enable ? ETH_10G_MAC_CMD_CFG_TX_FLUSH : 0));

		return 0;
	}

	switch (obj->mac_mode) {
	/** Do nothing for SGMII & RGMII */
	case AL_ETH_MAC_MODE_SGMII:
	case AL_ETH_MAC_MODE_RGMII:
		break;
	case AL_ETH_MAC_MODE_XLG_LL_40G:
	case AL_ETH_MAC_MODE_XLG_LL_50G:
	case AL_ETH_MAC_MODE_XLG_LL_25G:
		reg = al_mod_eth_40g_mac_reg_read(mac_regs_base,
			ETH_MAC_GEN_V3_MAC_40G_COMMAND_CONFIG_ADDR);

		if (enable)
			reg |= ETH_MAC_GEN_V3_MAC_40G_COMMAND_CONFIG_TX_FLUSH;
		else
			reg &= ~ETH_MAC_GEN_V3_MAC_40G_COMMAND_CONFIG_TX_FLUSH;

		al_mod_eth_40g_mac_reg_write(mac_regs_base,
			ETH_MAC_GEN_V3_MAC_40G_COMMAND_CONFIG_ADDR, reg);
		break;
	default:
		return -EPERM;
	}

	return 0;
}

static void iofic_attrs_get_hw(struct al_mod_eth_mac_obj *obj __attribute__((unused)),
			       uint32_t *mac_a_mask,
			       uint32_t *mac_b_mask,
			       uint32_t *mac_c_mask,
			       uint32_t *mac_d_mask)
{
	al_mod_assert(mac_a_mask);
	al_mod_assert(mac_b_mask);
	al_mod_assert(mac_c_mask);
	al_mod_assert(mac_d_mask);

	*mac_a_mask = AL_ETH_MAC_GROUP_A_ERROR_INTS;
	*mac_b_mask = AL_ETH_V3_MAC_GROUP_B_ERROR_INTS;
	*mac_c_mask = AL_ETH_MAC_GROUP_C_ERROR_INTS;
	*mac_d_mask = AL_ETH_MAC_GROUP_D_ERROR_INTS;

	/**
	 * Addressing RMN: 11056
	 *
	 * RMN description:
	 * False indication of parity interrupts
	 *
	 * Software flow:
	 * mask and ignore this interrupts
	 */
	*mac_b_mask &= ~(AL_ETH_V3_MAC_IOFIC_GROUP_B_10G_LL_STATS_RX_PARITY |
			 AL_ETH_V3_MAC_IOFIC_GROUP_B_10G_LL_STATS_TX_PARITY);
	*mac_c_mask &= ~AL_ETH_MAC_IOFIC_GROUP_C_10G_PCS_MEM_PARITY;
}

static void iofic_attrs_get_runtime_inval(struct al_mod_eth_mac_obj *obj,
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

	if (!AL_ETH_IS_1G_MAC(obj->mac_mode))
		*mac_b_mask |= (AL_ETH_V2_V3_MAC_IOFIC_GROUP_B_1G_TX_FIFO_PARITY |
				AL_ETH_MAC_IOFIC_GROUP_B_1G_TX_RTSM_PARITY |
				AL_ETH_MAC_IOFIC_GROUP_B_1G_RX_FIFO_PARITY);

	if (!(AL_ETH_IS_10G_MAC(obj->mac_mode) ||
	      AL_ETH_IS_25G_MAC(obj->mac_mode))) {
		*mac_b_mask |= (AL_ETH_V3_MAC_IOFIC_GROUP_B_10G_LL_STATS_TX_PARITY |
				AL_ETH_V3_MAC_IOFIC_GROUP_B_10G_LL_STATS_RX_PARITY);
		*mac_c_mask |= (AL_ETH_MAC_IOFIC_GROUP_C_10G_PCS_MEM_PARITY |
				AL_ETH_MAC_IOFIC_GROUP_C_10G_PCS_DELAY_RAM_PARITY |
				AL_ETH_MAC_IOFIC_GROUP_C_10G_PCS_LOOPBACK_PARITY);
	} else {
		/* check if loopback is enabled */
		if (obj->loopback_get && !obj->loopback_get(obj))
			*mac_c_mask |= AL_ETH_MAC_IOFIC_GROUP_C_10G_PCS_LOOPBACK_PARITY;

		/* check if FEC is enabled */
		if (!obj->fec_is_enabled(obj))
			*mac_c_mask |= (AL_ETH_MAC_IOFIC_GROUP_C_10G_PCS_DELAY_RAM_PARITY |
					AL_ETH_MAC_IOFIC_GROUP_C_10G_PCS_MEM_PARITY);
	}

	if (!AL_ETH_IS_40G_MAC(obj->mac_mode)) {
		*mac_b_mask |= (AL_ETH_V3_MAC_IOFIC_GROUP_B_40G_LL_STATS_TX_PARITY |
				AL_ETH_V3_MAC_IOFIC_GROUP_B_40G_LL_STATS_RX_PARITY);
		*mac_d_mask |= AL_ETH_MAC_GROUP_D_ERROR_INTS;
	} else {
		struct al_mod_eth_mac_regs *mac_regs_base = obj->mac_regs_base;
		uint32_t reg;

		/* check if FEC is enabled */
		reg = al_mod_reg_read32(&mac_regs_base->gen_v3.pcs_10g_ll_cfg);
		if (!(AL_REG_FIELD_GET(reg,
				       ETH_MAC_GEN_V3_PCS_40G_LL_CFG_FEC_EN_RX_MASK,
				       ETH_MAC_GEN_V3_PCS_40G_LL_CFG_FEC_EN_RX_SHIFT)) ||
		    !(AL_REG_FIELD_GET(reg,
				       ETH_MAC_GEN_V3_PCS_40G_LL_CFG_FEC_EN_TX_MASK,
				       ETH_MAC_GEN_V3_PCS_40G_LL_CFG_FEC_EN_TX_SHIFT))) {
			*mac_d_mask |=
				(AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_DELAY_1_PARITY |
				 AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_DELAY_0_PARITY |
				 AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_DELAY_RAM_3_PARITY |
				 AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_DELAY_RAM_2_PARITY |
				 AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_DELAY_RAM_1_PARITY |
				 AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_DELAY_RAM_0_PARITY |
				 AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_MEM_3_PARITY |
				 AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_MEM_2_PARITY |
				 AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_MEM_1_PARITY |
				 AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_MEM_0_PARITY);
		} else if (!(reg & ETH_MAC_GEN_V3_PCS_40G_LL_CFG_FEC91_ENA)) {
			*mac_d_mask |= (AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_DELAY_1_PARITY |
					AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_DELAY_0_PARITY);
		} else {
			*mac_d_mask |=
				(AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_DELAY_RAM_3_PARITY |
				 AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_DELAY_RAM_2_PARITY |
				 AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_DELAY_RAM_1_PARITY |
				 AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_DELAY_RAM_0_PARITY |
				 AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_MEM_3_PARITY |
				 AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_MEM_2_PARITY |
				 AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_MEM_1_PARITY |
				 AL_ETH_MAC_IOFIC_GROUP_D_40G_PCS_MEM_0_PARITY);
		}
	}
}

static void iofic_attrs_get_transient(struct al_mod_eth_mac_obj *obj,
				      uint32_t *mac_a_mask,
				      uint32_t *mac_b_mask,
				      uint32_t *mac_c_mask,
				      uint32_t *mac_d_mask)
{
	/* All v3 IOFIC bits are considered transient */
	iofic_attrs_get_hw(obj, mac_a_mask, mac_b_mask, mac_c_mask, mac_d_mask);
}

static void iofic_attrs_get_non_fatal(struct al_mod_eth_mac_obj *obj __attribute__((unused)),
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

int al_mod_eth_mac_v3_handle_init(struct al_mod_eth_mac_obj *obj,
	__attribute__((unused)) struct al_mod_eth_mac_obj_init_params *params)
{
	obj->mode_set = mac_mode_set;
	obj->mode_get = mac_mode_get;
	obj->config = mac_config;
	obj->link_config = al_mod_eth_mac_v1_v3_mac_link_config;
	obj->link_status_get = link_status_get;
	obj->link_status_clear = link_status_clear;
	obj->led_set = al_mod_eth_mac_v1_v3_led_set;
	obj->loopback_config = mac_loopback_config;
	obj->loopback_get = mac_loopback_get;
	obj->stats_get = mac_stats_get;
	obj->mdio_config = al_mod_eth_mac_v1_v3_mdio_config;
	obj->mdio_read = al_mod_eth_mac_v1_v3_mdio_read;
	obj->mdio_write = al_mod_eth_mac_v1_v3_mdio_write;
	obj->fec_enable = fec_enable;
	obj->fec_is_enabled = fec_is_enabled;
	obj->fec_stats_get = fec_stats_get;
	obj->gearbox_reset = gearbox_reset;
	obj->rx_pkt_limit_config = rx_pkt_limit_config;
	obj->eee_config = eee_config;
	obj->scratch_read = al_mod_eth_mac_v1_v3_scratch_reg_read32;
	obj->scratch_write = al_mod_eth_mac_v1_v3_scratch_reg_write32;
	obj->tx_flush_config = tx_flush_config;
	obj->iofic_attrs_get_hw = iofic_attrs_get_hw;
	obj->iofic_attrs_get_runtime_inval = iofic_attrs_get_runtime_inval;
	obj->iofic_attrs_get_transient = iofic_attrs_get_transient;
	obj->iofic_attrs_get_non_fatal = iofic_attrs_get_non_fatal;
	obj->mac_start_stop_adv = mac_start_stop_adv;

	return 0;
}

/** @} end of Ethernet group */
