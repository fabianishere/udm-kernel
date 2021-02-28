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
 * @file   al_mod_hal_eth_mac_internal.c
 *
 * @brief  Ethernet mac internal
 *
 */

#include "al_mod_hal_eth.h"
#include "al_mod_hal_eth_mac.h"
#include "al_mod_hal_eth_mac_internal.h"
#include "al_mod_hal_eth_mac_regs.h"

#define ETH_MAC_GEN_LED_CFG_BLINK_TIMER_VAL 5
#define ETH_MAC_GEN_LED_CFG_ACT_TIMER_VAL 7

/* micro seconds to wait when polling mdio status */
#define AL_ETH_MDIO_DELAY_PERIOD_MIN	(1)
#define AL_ETH_MDIO_DELAY_PERIOD_MAX	(AL_ETH_MDIO_DELAY_PERIOD_MIN + 10)
#define AL_ETH_MDIO_DELAY_COUNT		150 /* number of times to poll */

static void link_config_1g_mac(
				struct al_mod_eth_mac_obj *obj,
				al_mod_bool force_1000_base_x,
				al_mod_bool an_enable,
				uint32_t speed,
				al_mod_bool full_duplex)
{
	struct al_mod_eth_mac_regs *mac_regs_base = obj->mac_regs_base;
	uint32_t mac_ctrl;
	uint32_t sgmii_ctrl = 0;
	uint32_t sgmii_if_mode = 0;
	uint32_t rgmii_ctrl = 0;

	mac_ctrl = al_mod_reg_read32(&mac_regs_base->mac_1g.cmd_cfg);

	switch (obj->mac_mode) {
	case AL_ETH_MAC_MODE_SGMII:
		al_mod_reg_write32(&mac_regs_base->sgmii.reg_addr,
			ETH_MAC_SGMII_REG_ADDR_CTRL_REG);
		sgmii_ctrl = al_mod_reg_read32(&mac_regs_base->sgmii.reg_data);
		/*
		 * in case bit 0 is off in sgmii_if_mode register all the other
		 * bits are ignored.
		 */
		if (force_1000_base_x == AL_FALSE)
			sgmii_if_mode = ETH_MAC_SGMII_REG_DATA_IF_MODE_SGMII_EN;

		if (an_enable == AL_TRUE) {
			sgmii_if_mode |= ETH_MAC_SGMII_REG_DATA_IF_MODE_SGMII_AN;
			sgmii_ctrl |= ETH_MAC_SGMII_REG_DATA_CTRL_AN_ENABLE;
		} else {
			sgmii_ctrl &= ~(ETH_MAC_SGMII_REG_DATA_CTRL_AN_ENABLE);
		}

		if (full_duplex == AL_FALSE)
			sgmii_if_mode |= ETH_MAC_SGMII_REG_DATA_IF_MODE_SGMII_DUPLEX;

		if (speed == 1000)
			sgmii_if_mode |= ETH_MAC_SGMII_REG_DATA_IF_MODE_SGMII_SPEED_1000;
		else if (speed != 10)
			sgmii_if_mode |= ETH_MAC_SGMII_REG_DATA_IF_MODE_SGMII_SPEED_100;


		al_mod_reg_write32(&mac_regs_base->sgmii.reg_addr,
			       ETH_MAC_SGMII_REG_ADDR_IF_MODE_REG);
		al_mod_reg_write32(&mac_regs_base->sgmii.reg_data,
			       sgmii_if_mode);

		al_mod_reg_write32(&mac_regs_base->sgmii.reg_addr,
			       ETH_MAC_SGMII_REG_ADDR_CTRL_REG);
		al_mod_reg_write32(&mac_regs_base->sgmii.reg_data,
			       sgmii_ctrl);
		break;

	case AL_ETH_MAC_MODE_RGMII:
		/*
		 * Use the speed provided by the MAC instead of the PHY
		 */
		rgmii_ctrl = al_mod_reg_read32(&mac_regs_base->gen.rgmii_cfg);

		AL_REG_MASK_CLEAR(rgmii_ctrl, ETH_MAC_GEN_RGMII_CFG_ENA_AUTO);
		AL_REG_MASK_CLEAR(rgmii_ctrl, ETH_MAC_GEN_RGMII_CFG_SET_1000_SEL);
		AL_REG_MASK_CLEAR(rgmii_ctrl, ETH_MAC_GEN_RGMII_CFG_SET_10_SEL);

		al_mod_reg_write32(&mac_regs_base->gen.rgmii_cfg, rgmii_ctrl);
		break;

	default:
		break;

	}

	if (full_duplex == AL_TRUE)
		AL_REG_MASK_CLEAR(mac_ctrl, ETH_1G_MAC_CMD_CFG_HD_EN);
	else
		AL_REG_MASK_SET(mac_ctrl, ETH_1G_MAC_CMD_CFG_HD_EN);

	if (speed == 1000)
		AL_REG_MASK_SET(mac_ctrl, ETH_1G_MAC_CMD_CFG_1G_SPD);
	else {
		AL_REG_MASK_CLEAR(mac_ctrl, ETH_1G_MAC_CMD_CFG_1G_SPD);
		if (speed == 10)
			AL_REG_MASK_SET(mac_ctrl, ETH_1G_MAC_CMD_CFG_10M_SPD);
		else
			AL_REG_MASK_CLEAR(mac_ctrl, ETH_1G_MAC_CMD_CFG_10M_SPD);
	}
	al_mod_reg_write32(&mac_regs_base->mac_1g.cmd_cfg, mac_ctrl);
}

static void link_config_10g_mac(
	struct al_mod_eth_mac_obj *obj,
	al_mod_bool force_1000_base_x,
	al_mod_bool an_enable,
	uint32_t speed,
	al_mod_bool full_duplex)
{
	struct al_mod_eth_mac_regs *mac_regs_base = obj->mac_regs_base;
	uint32_t if_mode;
	uint32_t val;

	if_mode = al_mod_reg_read32(&mac_regs_base->mac_10g.if_mode);

	if (force_1000_base_x) {
		uint32_t control;

		AL_REG_MASK_CLEAR(if_mode, ETH_10G_MAC_IF_MODE_SGMII_EN_MASK);

		control = al_mod_reg_read32(&mac_regs_base->mac_10g.control);

		if (an_enable)
			AL_REG_MASK_SET(control, ETH_10G_MAC_CONTROL_AN_EN_MASK);
		else
			AL_REG_MASK_CLEAR(control, ETH_10G_MAC_CONTROL_AN_EN_MASK);

		al_mod_reg_write32(&mac_regs_base->mac_10g.control, control);

	} else {
		AL_REG_MASK_SET(if_mode, ETH_10G_MAC_IF_MODE_SGMII_EN_MASK);
		if (an_enable) {
			AL_REG_MASK_SET(if_mode, ETH_10G_MAC_IF_MODE_SGMII_AN_MASK);
		} else {
			AL_REG_MASK_CLEAR(if_mode, ETH_10G_MAC_IF_MODE_SGMII_AN_MASK);

			if (speed == 1000)
				val = ETH_10G_MAC_IF_MODE_SGMII_SPEED_1G;
			else if (speed == 100)
				val = ETH_10G_MAC_IF_MODE_SGMII_SPEED_100M;
			else
				val = ETH_10G_MAC_IF_MODE_SGMII_SPEED_10M;

			AL_REG_FIELD_SET(if_mode,
					 ETH_10G_MAC_IF_MODE_SGMII_SPEED_MASK,
					 ETH_10G_MAC_IF_MODE_SGMII_SPEED_SHIFT,
					 val);

			AL_REG_FIELD_SET(if_mode,
					 ETH_10G_MAC_IF_MODE_SGMII_DUPLEX_MASK,
					 ETH_10G_MAC_IF_MODE_SGMII_DUPLEX_SHIFT,
					 ((full_duplex) ?
						ETH_10G_MAC_IF_MODE_SGMII_DUPLEX_FULL :
						ETH_10G_MAC_IF_MODE_SGMII_DUPLEX_HALF));
		}
	}

	al_mod_reg_write32(&mac_regs_base->mac_10g.if_mode, if_mode);
}

/* update link speed and duplex mode */
int al_mod_eth_mac_v1_v3_mac_link_config(struct al_mod_eth_mac_obj *obj,
			   al_mod_bool force_1000_base_x,
			   al_mod_bool an_enable,
			   uint32_t speed,
			   al_mod_bool full_duplex)
{
	switch (obj->mac_mode) {
	case AL_ETH_MAC_MODE_RGMII:
	case AL_ETH_MAC_MODE_SGMII:
	case AL_ETH_MAC_MODE_SGMII_2_5G:
		break;
	default:
		al_mod_err("eth [%s]: this function not supported in this mac mode.\n",
			       obj->name);
		return -EINVAL;
	}

	if ((obj->mac_mode != AL_ETH_MAC_MODE_RGMII) && (an_enable)) {
		/*
		 * an_enable is not relevant to RGMII mode.
		 * in AN mode speed and duplex aren't relevant.
		 */
		al_mod_info("eth [%s]: set auto negotiation to enable\n", obj->name);
	} else {
		al_mod_info("eth [%s]: set link speed to %dMbps. %s duplex.\n", obj->name,
			speed, full_duplex == AL_TRUE ? "full" : "half");

		if ((speed != 10) && (speed != 100) && (speed != 1000)) {
			al_mod_err("eth [%s]: bad speed parameter (%d).\n",
					obj->name, speed);
			return -EINVAL;
		}
		if ((speed == 1000) && (full_duplex == AL_FALSE)) {
			al_mod_err("eth [%s]: half duplex in 1Gbps is not supported.\n",
					obj->name);
			return -EINVAL;
		}
	}

	switch (obj->mac_mode) {
	case AL_ETH_MAC_MODE_SGMII:
	case AL_ETH_MAC_MODE_RGMII:
		link_config_1g_mac(obj, force_1000_base_x, an_enable,
					      speed, full_duplex);
		break;
	case AL_ETH_MAC_MODE_SGMII_2_5G:
		link_config_10g_mac(obj, force_1000_base_x, an_enable,
					       speed, full_duplex);
		break;
	default:
		break;
	}

	return 0;
}

/* MDIO */
int al_mod_eth_ref_clk_freq_val(enum al_mod_eth_ref_clk_freq ref_clk_freq)
{
	switch (ref_clk_freq) {
	case AL_ETH_REF_FREQ_375_MHZ:
		return 375000;
	case AL_ETH_REF_FREQ_187_5_MHZ:
		return 187500;
	case AL_ETH_REF_FREQ_250_MHZ:
		return 250000;
	case AL_ETH_REF_FREQ_500_MHZ:
		return 500000;
	case AL_ETH_REF_FREQ_428_MHZ:
		return 428000;
	default:
		return -EPERM;
	};
}

int al_mod_eth_mac_v1_v3_mdio_config(struct al_mod_eth_mac_obj *obj, enum al_mod_eth_mdio_type mdio_type,
	al_mod_bool shared_mdio_if, enum al_mod_eth_ref_clk_freq ref_clk_freq,
	unsigned int mdio_clk_freq_khz, al_mod_bool ignore_read_status)
{
	struct al_mod_eth_mac_regs *mac_regs_base = obj->mac_regs_base;

	enum al_mod_eth_mdio_if mdio_if = AL_ETH_MDIO_IF_10G_MAC;
	const char *if_name = "10G MAC";
	const char *type_name = al_mod_eth_mdio_type_str(mdio_type);
	const char *shared_name = (shared_mdio_if == AL_TRUE) ?
		"Yes" : "No";

	int ref_clk_freq_khz;
	uint32_t val;

	al_mod_dbg("eth [%s]: mdio config: interface %s. type %s. shared: %s\n",
		obj->name, if_name, type_name, shared_name);
	obj->shared_mdio_if = shared_mdio_if;
	obj->ignore_read_status = ignore_read_status;

	val = al_mod_reg_read32(&mac_regs_base->gen.cfg);
	al_mod_dbg("eth [%s]: mdio config: 10G mac\n", obj->name);

	switch (mdio_if) {
	case AL_ETH_MDIO_IF_1G_MAC:
		val &= ~AL_BIT(10);
		break;
	case AL_ETH_MDIO_IF_10G_MAC:
		val |= AL_BIT(10);
		break;
	default:
		al_mod_assert(0);
		return -EINVAL;
	}
	al_mod_reg_write32(&mac_regs_base->gen.cfg, val);
	obj->mdio_if = mdio_if;

	if (mdio_if == AL_ETH_MDIO_IF_10G_MAC) {
		val = al_mod_reg_read32(&mac_regs_base->mac_10g.mdio_cfg_status);
		switch (mdio_type) {
		case AL_ETH_MDIO_TYPE_CLAUSE_22:
			val &= ~AL_BIT(6);
			break;
		case AL_ETH_MDIO_TYPE_CLAUSE_45:
			val |= AL_BIT(6);
			break;
		default:
			al_mod_assert(0);
			return -EINVAL;
		}

		/* set clock div to get 'mdio_clk_freq_khz' */
		ref_clk_freq_khz = al_mod_eth_ref_clk_freq_val(ref_clk_freq);
		if (ref_clk_freq_khz < 0) {
			al_mod_err("eth [%s]: %s: invalid reference clock frequency (%d)\n",
				obj->name, __func__, ref_clk_freq);
			return -EPERM;
		}

		val &= ~(0x1FF << 7);
		val |= (ref_clk_freq_khz / (2 * mdio_clk_freq_khz)) << 7;
		AL_REG_FIELD_SET(val, ETH_10G_MAC_MDIO_CFG_HOLD_TIME_MASK,
				 ETH_10G_MAC_MDIO_CFG_HOLD_TIME_SHIFT,
				 ETH_10G_MAC_MDIO_CFG_HOLD_TIME_7_CLK);
		al_mod_reg_write32(&mac_regs_base->mac_10g.mdio_cfg_status, val);
	} else {
		if (mdio_type != AL_ETH_MDIO_TYPE_CLAUSE_22) {
			al_mod_err("eth [%s] mdio type not supported for this interface\n",
				 obj->name);
			return -EINVAL;
		}
	}
	obj->mdio_type = mdio_type;

	return 0;
}

/**
 * Acquire mdio interface ownership
 * when mdio interface shared between multiple eth controllers,
 * this function waits until the ownership granted for this controller.
 * this function does nothing when the mdio interface is used only by this controller.
 *
 * @param obj
 * @return 0 on success, -ETIMEDOUT  on timeout.
 */
static int mdio_lock(struct al_mod_eth_mac_obj *obj)
{
	int	count = 0;
	uint32_t mdio_ctrl_1;
	struct al_mod_eth_mac_regs *mac_regs_base = obj->mac_regs_base;

	if (obj->shared_mdio_if == AL_FALSE)
		return 0; /* nothing to do when interface is not shared */

	do {
		mdio_ctrl_1 = al_mod_reg_read32(&mac_regs_base->gen.mdio_ctrl_1);

		/** Error */
		/* if (mdio_cfg_status & AL_BIT(1)) {
			al_mod_err(" %s mdio read failed on error. phy_addr 0x%x reg 0x%x\n",
				udma_params.name, phy_addr, reg);
			return -EIO;
		}*/
		if (mdio_ctrl_1 & AL_BIT(0)) {
			if (count > 0)
				al_mod_dbg("eth %s mdio interface still busy!\n", obj->name);
		} else {
			return 0;
		}
		al_mod_usleep_range(AL_ETH_MDIO_DELAY_PERIOD_MIN, AL_ETH_MDIO_DELAY_PERIOD_MAX);
	} while (count++ < (AL_ETH_MDIO_DELAY_COUNT * 4));
	al_mod_err(" %s mdio failed to take ownership. MDIO info reg: 0x%08x\n",
		obj->name, al_mod_reg_read32(&mac_regs_base->gen.mdio_1));

	return -ETIMEDOUT;
}

/**
 * Free mdio interface ownership
 * when mdio interface shared between multiple eth controllers,
 * this function releases the ownership granted for this controller.
 * this function does nothing when the mdio interface is used only by this controller.
 *
 * @param obj
 * @return 0.
 */
static int mdio_free(struct al_mod_eth_mac_obj *obj)
{
	struct al_mod_eth_mac_regs *mac_regs_base = obj->mac_regs_base;

	if (obj->shared_mdio_if == AL_FALSE)
		return 0; /* nothing to do when interface is not shared */

	al_mod_reg_write32(&mac_regs_base->gen.mdio_ctrl_1, 0);

	/*
	 * Addressing RMN: 2917
	 *
	 * RMN description:
	 * The HW spin-lock is stateless and doesn't maintain any scheduling
	 * policy.
	 *
	 * Software flow:
	 * After getting the lock wait 2 times the delay period in order to give
	 * the other port chance to take the lock and prevent starvation.
	 * This is not scalable to more than two ports.
	 */
	al_mod_usleep_range(2 * AL_ETH_MDIO_DELAY_PERIOD_MIN, (2 * AL_ETH_MDIO_DELAY_PERIOD_MIN) + 10);

	return 0;
}

static int mdio_1g_mac_read(struct al_mod_eth_mac_obj *obj,
			    uint32_t phy_addr __attribute__((__unused__)),
			    uint32_t reg, uint16_t *val)
{
	struct al_mod_eth_mac_regs *mac_regs_base = obj->mac_regs_base;

	*val = al_mod_reg_read32(
		&mac_regs_base->mac_1g.phy_regs_base + reg);
	return 0;
}

static int mdio_1g_mac_write(struct al_mod_eth_mac_obj *obj,
			     uint32_t phy_addr __attribute__((__unused__)),
			     uint32_t reg, uint16_t val)
{
	struct al_mod_eth_mac_regs *mac_regs_base = obj->mac_regs_base;

	al_mod_reg_write32(
		&mac_regs_base->mac_1g.phy_regs_base + reg, val);
	return 0;
}

static int mdio_10g_mac_wait_busy(struct al_mod_eth_mac_obj *obj)
{
	int	count = 0;
	uint32_t mdio_cfg_status;
	struct al_mod_eth_mac_regs *mac_regs_base = obj->mac_regs_base;

	do {
		mdio_cfg_status = al_mod_reg_read32(&mac_regs_base->mac_10g.mdio_cfg_status);
		/** Error */
		/*if (mdio_cfg_status & AL_BIT(1)){
			al_mod_err(" %s mdio read failed on error. phy_addr 0x%x reg 0x%x\n",
				udma_params.name, phy_addr, reg);
			return -EIO;
		}*/
		if (mdio_cfg_status & AL_BIT(0)) {
			if (count > 0)
				al_mod_dbg("eth [%s] mdio: still busy!\n", obj->name);
		} else {
			return 0;
		}
		al_mod_usleep_range(AL_ETH_MDIO_DELAY_PERIOD_MIN, AL_ETH_MDIO_DELAY_PERIOD_MAX);
	} while (count++ < AL_ETH_MDIO_DELAY_COUNT);

	return -ETIMEDOUT;
}

static int mdio_10g_mac_type22(
		struct al_mod_eth_mac_obj *obj,
	int read, uint32_t phy_addr, uint32_t reg, uint16_t *val)
{
	int rc;
	const char *op = (read == 1) ? "read" : "write";
	uint32_t mdio_cfg_status;
	uint16_t mdio_cmd;
	struct al_mod_eth_mac_regs *mac_regs_base = obj->mac_regs_base;

	/** Wait if the HW is busy */
	rc = mdio_10g_mac_wait_busy(obj);
	if (rc) {
		al_mod_err(" eth [%s] mdio %s failed. HW is busy\n", obj->name, op);
		return rc;
	}

	mdio_cmd = (uint16_t)(0x1F & reg);
	mdio_cmd |= (0x1F & phy_addr) << 5;

	/** READ command */
	if (read)
		mdio_cmd |= AL_BIT(15);

	al_mod_reg_write16(&mac_regs_base->mac_10g.mdio_cmd,
			mdio_cmd);
	if (!read)
		al_mod_reg_write16(&mac_regs_base->mac_10g.mdio_data,
				*val);

	/** Wait for the busy to clear */
	rc = mdio_10g_mac_wait_busy(obj);
	if (rc != 0) {
		al_mod_err(" %s mdio %s failed on timeout\n", obj->name, op);
		return -ETIMEDOUT;
	}

	mdio_cfg_status = al_mod_reg_read32(&mac_regs_base->mac_10g.mdio_cfg_status);

	/** Error */
	if ((!obj->ignore_read_status) && (mdio_cfg_status & AL_BIT(1))) {
		al_mod_dbg(" %s mdio %s failed on error. phy_addr 0x%x reg 0x%x\n",
			obj->name, op, phy_addr, reg);
			return -EIO;
	}
	if (read)
		*val = al_mod_reg_read16(
			(uint16_t *)&mac_regs_base->mac_10g.mdio_data);
	return 0;
}

static int mdio_10g_mac_type45(
		struct al_mod_eth_mac_obj *obj,
	int read, uint32_t port_addr, uint32_t device, uint32_t reg, uint16_t *val)
{
	int rc;
	const char *op = (read == 1) ? "read" : "write";
	uint32_t mdio_cfg_status;
	uint16_t mdio_cmd;
	struct al_mod_eth_mac_regs *mac_regs_base = obj->mac_regs_base;

	/** Wait if the HW is busy */
	rc = mdio_10g_mac_wait_busy(obj);
	if (rc) {
		al_mod_err(" %s mdio %s failed. HW is busy\n", obj->name, op);
		return rc;
	}
	/**  Set command register */
	mdio_cmd = (uint16_t)(0x1F & device);
	mdio_cmd |= (0x1F & port_addr) << 5;
	al_mod_reg_write16(&mac_regs_base->mac_10g.mdio_cmd,
			mdio_cmd);

	/**  Send address frame */
	al_mod_reg_write16(&mac_regs_base->mac_10g.mdio_regaddr, reg);
	/** Wait for the busy to clear */
	rc = mdio_10g_mac_wait_busy(obj);
	if (rc) {
		al_mod_err(" %s mdio %s (address frame) failed on timeout\n", obj->name, op);
		return rc;
	}

	/** If read, write again to the command register with READ bit set */
	if (read) {
		/** Read command */
		mdio_cmd |= AL_BIT(15);
		al_mod_reg_write16(
			(uint16_t *)&mac_regs_base->mac_10g.mdio_cmd,
			mdio_cmd);
	} else {
		al_mod_reg_write16(
			(uint16_t *)&mac_regs_base->mac_10g.mdio_data,
			*val);
	}
	/** Wait for the busy to clear */
	rc = mdio_10g_mac_wait_busy(obj);
	if (rc) {
		al_mod_err(" %s mdio %s failed on timeout\n", obj->name, op);
		return rc;
	}

	mdio_cfg_status = al_mod_reg_read32(&mac_regs_base->mac_10g.mdio_cfg_status);
	/** Error */
	if (mdio_cfg_status & AL_BIT(1)) {
		al_mod_err(" %s mdio %s failed on error. port 0x%x, device 0x%x reg 0x%x\n",
			obj->name, op, port_addr, device, reg);
			return -EIO;
	}
	if (read)
		*val = al_mod_reg_read16(
			(uint16_t *)&mac_regs_base->mac_10g.mdio_data);
	return 0;
}

int al_mod_eth_mac_v1_v3_mdio_read(struct al_mod_eth_mac_obj *obj, uint32_t phy_addr, uint32_t device,
	uint32_t reg, uint16_t *val)
{
	int rc;
	rc = mdio_lock(obj);

	/* Interface ownership taken */
	if (rc)
		return rc;

	if (obj->mdio_if == AL_ETH_MDIO_IF_1G_MAC)
		rc = mdio_1g_mac_read(obj, phy_addr, reg, val);
	else
		if (obj->mdio_type == AL_ETH_MDIO_TYPE_CLAUSE_22)
			rc = mdio_10g_mac_type22(obj, 1, phy_addr, reg, val);
		else
			rc = mdio_10g_mac_type45(obj, 1, phy_addr, device, reg, val);

	mdio_free(obj);
	al_mod_dbg("eth mdio read: phy_addr %x, device %x, reg %x val %x\n",
		phy_addr, device, reg, *val);
	return rc;
}

int al_mod_eth_mac_v1_v3_mdio_write(struct al_mod_eth_mac_obj *obj, uint32_t phy_addr, uint32_t device,
	uint32_t reg, uint16_t val)
{
	int rc;
	al_mod_dbg("eth mdio write: phy_addr %x, device %x, reg %x, val %x\n",
		phy_addr, device, reg, val);
	rc = mdio_lock(obj);

	/* Interface ownership taken */
	if (rc)
		return rc;

	if (obj->mdio_if == AL_ETH_MDIO_IF_1G_MAC)
		rc = mdio_1g_mac_write(obj, phy_addr, reg, val);
	else
		if (obj->mdio_type == AL_ETH_MDIO_TYPE_CLAUSE_22)
			rc = mdio_10g_mac_type22(obj, 0, phy_addr, reg, &val);
		else
			rc = mdio_10g_mac_type45(obj, 0, phy_addr, device, reg, &val);

	mdio_free(obj);
	return rc;
}

/** set LED mode and value */
int al_mod_eth_mac_v1_v3_led_set(struct al_mod_eth_mac_obj *obj, al_mod_bool link_is_up)
{
	struct al_mod_eth_mac_regs *mac_regs_base = obj->mac_regs_base;
	uint32_t reg = 0;
	uint32_t mode  = ETH_MAC_GEN_LED_CFG_SEL_DEFAULT_REG;

	if (link_is_up)
		mode = ETH_MAC_GEN_LED_CFG_SEL_LINK_ACTIVITY;

	AL_REG_FIELD_SET(reg,  ETH_MAC_GEN_LED_CFG_SEL_MASK,
			 ETH_MAC_GEN_LED_CFG_SEL_SHIFT, mode);

	AL_REG_FIELD_SET(reg, ETH_MAC_GEN_LED_CFG_BLINK_TIMER_MASK,
			 ETH_MAC_GEN_LED_CFG_BLINK_TIMER_SHIFT,
			 ETH_MAC_GEN_LED_CFG_BLINK_TIMER_VAL);

	AL_REG_FIELD_SET(reg, ETH_MAC_GEN_LED_CFG_ACT_TIMER_MASK,
			 ETH_MAC_GEN_LED_CFG_ACT_TIMER_SHIFT,
			 ETH_MAC_GEN_LED_CFG_ACT_TIMER_VAL);

	al_mod_reg_write32(&mac_regs_base->gen.led_cfg, reg);

	return 0;
}

static int v1_v3_scratch_reg_set_offset(struct al_mod_eth_mac_obj *obj, unsigned int idx,
	uint32_t **offset)
{
	uint32_t *val;
	struct al_mod_eth_mac_regs *mac_regs_base = obj->mac_regs_base;

	switch (idx) {
	case 0:
		val = &mac_regs_base->mac_1g.scratch;
		break;
	case 1:
		val = &mac_regs_base->mac_10g.scratch;
		break;
	case 2:
		val = &mac_regs_base->mac_1g.mac_0;
		break;
	default:
		return -EPERM;
	}

	*offset = val;
	return 0;
}

int al_mod_eth_mac_v1_v3_scratch_reg_write32(struct al_mod_eth_mac_obj *obj, unsigned int idx,
	uint32_t val)
{
	uint32_t *offset;

	if (v1_v3_scratch_reg_set_offset(obj, idx, &offset) != 0) {
		al_mod_err("%s: Error while writing to scratch register"
			": No scratch register %d\n", __func__, idx);
		return -EPERM;
	}
	al_mod_reg_write32(offset, val);

	return 0;
}

int al_mod_eth_mac_v1_v3_scratch_reg_read32(struct al_mod_eth_mac_obj *obj, unsigned int idx,
	uint32_t *val)
{
	uint32_t *offset;

	if (v1_v3_scratch_reg_set_offset(obj, idx, &offset) != 0) {
		al_mod_err("%s: Error while reading from scratch register"
			": No scratch register %d\n", __func__, idx);
		return -EPERM;
	}
	*val = al_mod_reg_read32(offset);

	return 0;
}

/** @} end of Ethernet mac internal group */
