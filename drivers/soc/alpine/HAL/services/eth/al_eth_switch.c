/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#include "al_hal_eth.h"
#include "al_eth_switch.h"

#define INDIRECT_CMD_REG	0
#define INDIRECT_VAL_REG	1

#define INDIRECT_CMD_BUSY		0x8000
#define INDIRECT_CMD_MODE_CLAUSE_22	0x1000
#define INDIRECT_CMD_OPCODE_READ	0x0800
#define INDIRECT_CMD_OPCODE_WRITE	0x0400
#define INDIRECT_CMD_DEV_ADDR_MASK	AL_FIELD_MASK(9, 5)
#define INDIRECT_CMD_DEV_ADDR_SHIFT	5
#define INDIRECT_CMD_REG_ADDR_MASK	AL_FIELD_MASK(4, 0)
#define INDIRECT_CMD_REG_ADDR_SHIFT	0

#define INDIRECT_CMD_PREPARE(dev_addr, reg_addr, opcode) \
	(INDIRECT_CMD_BUSY | INDIRECT_CMD_MODE_CLAUSE_22 | (opcode) | \
		(((dev_addr) << INDIRECT_CMD_DEV_ADDR_SHIFT) & INDIRECT_CMD_DEV_ADDR_MASK) | \
		(((reg_addr) << INDIRECT_CMD_REG_ADDR_SHIFT) & INDIRECT_CMD_REG_ADDR_MASK))

/* Switch port registers */
#define SWITCH_REG_PORT_STATUS		0
#define SWITCH_REG_PHYS_CTRL		1
#define SWITCH_REG_ID			3
#define SWITCH_REG_PORT_CTRL		4
#define SWITCH_REG_PORT_VLAN_MAP	6

#define SWITCH_DEVICE_GLOBAL1		0x1B
#define SWITCH_DEVICE_GLOBAL2		0x1C
/* Global 2 registers */
#define SWITCH_REG_GLOBAL2_SMI_CMD	0x18
#define SWITCH_REG_GLOBAL2_SMI_DATA	0x19

/* Phy registers */
#define SWITCH_PHY_REG_PAGE_ADDR	22

#define SWITCH_PORT2DEVICE(port)	(0x10 + (port))
#define SWITCH_PHY_CTRL_REG		0
#define SWITCH_PHY_CTRL_PAGE		0
#define SWITCH_PHY_CTRL_POWER_DOWN_BIT	11
#define SWITCH_PHY_CTRL_RESET_BIT	15

#define SWITCH_PORT_ENABLE_FORWARDING	0x3
#define SWITCH_REG_PORT_VLAN_FORCE_MAP	11
#define SWITCH_REG_PORT_VLAN_TABLE_MASK	0x7F

#define SWITCH_DEVICE_LAST		0x1D

#define	SWITCH_INDIRECT_CMD_TIMEOUT_US	100000 /* 100ms */

int al_eth_switch_handle_init(struct al_eth_switch_handle *handle,
		struct al_eth_switch_init_params *params)
{
	al_assert(params->mdio_read);
	al_assert(params->mdio_write);

	if ((params->model != AL_ETH_SWITCH_MODEL_MV88E6172) &&
		(params->model != AL_ETH_SWITCH_MODEL_MV88E6141)) {
		al_err("%s: unsupported switch model %d\n", __func__, params->model);
		return -EINVAL;
	}

	handle->model = params->model;
	handle->mdio_read = params->mdio_read;
	handle->mdio_write = params->mdio_write;
	handle->mdio_priv = params->mdio_priv;
	handle->mdio_addr = params->mdio_addr;
	if (params->model == AL_ETH_SWITCH_MODEL_MV88E6141)
		handle->phy_addr_offset = 0x10;
	else
		handle->phy_addr_offset = 0;
	return 0;
}

int al_eth_switch_mdio_read(struct al_eth_switch_handle *handle,
		unsigned int device, unsigned int reg, uint16_t *val)

{
	int timeout = SWITCH_INDIRECT_CMD_TIMEOUT_US;
	uint16_t indirect_cmd;
	int ret;

	al_assert(handle);
	al_assert(handle->mdio_read);
	al_assert(handle->mdio_write);
	al_assert(device <= SWITCH_DEVICE_LAST);

	indirect_cmd = INDIRECT_CMD_PREPARE(device, reg, INDIRECT_CMD_OPCODE_READ);
	ret = handle->mdio_write(handle->mdio_priv, handle->mdio_addr, 0,
			INDIRECT_CMD_REG, indirect_cmd);
	if (ret) {
		al_err("%s: write failed: device 0x%02x, reg %d, val 0x%04x\n",
				__func__, handle->mdio_addr, INDIRECT_CMD_REG, indirect_cmd);
		return ret;
	}

	while (timeout) {
		ret = handle->mdio_read(handle->mdio_priv, handle->mdio_addr, 0,
				INDIRECT_CMD_REG, &indirect_cmd);
		if (ret) {
			al_err("%s: read failed: device 0x%02x, reg %d\n",
					__func__, handle->mdio_addr, INDIRECT_CMD_REG);
			return ret;
		}
		if ((indirect_cmd & INDIRECT_CMD_BUSY) == 0)
			break;

		timeout--;
		al_udelay(1);
	}

	if (!timeout) {
		al_err("%s: indirect command timed out\n", __func__);
		return -ETIMEDOUT;
	}

	ret = handle->mdio_read(handle->mdio_priv, handle->mdio_addr, 0, INDIRECT_VAL_REG, val);
	if (ret) {
		al_err("%s: read failed: device 0x%02x, reg %d\n",
				__func__, handle->mdio_addr, INDIRECT_VAL_REG);
		return ret;
	}

	al_dbg("%s: read: device 0x%02x, reg %d, val 0x%04x\n",
			__func__, device, reg, *val);

	return 0;
}

int al_eth_switch_mdio_write(struct al_eth_switch_handle *handle,
		unsigned int device, unsigned int reg, uint16_t val)
{
	int timeout = SWITCH_INDIRECT_CMD_TIMEOUT_US;
	uint16_t indirect_cmd;
	int ret;

	al_assert(handle);
	al_assert(handle->mdio_read);
	al_assert(handle->mdio_write);
	al_assert(device <= SWITCH_DEVICE_LAST);

	ret = handle->mdio_write(handle->mdio_priv, handle->mdio_addr, 0, INDIRECT_VAL_REG, val);
	if (ret) {
		al_err("%s: write failed: device 0x%02x, reg %d, val 0x%04x\n",
				__func__, handle->mdio_addr, INDIRECT_VAL_REG, val);
		return ret;
	}

	indirect_cmd = INDIRECT_CMD_PREPARE(device, reg, INDIRECT_CMD_OPCODE_WRITE);
	ret = handle->mdio_write(handle->mdio_priv, handle->mdio_addr, 0,
			INDIRECT_CMD_REG, indirect_cmd);
	if (ret) {
		al_err("%s: write failed: device 0x%02x, reg %d, val 0x%04x\n",
				__func__, handle->mdio_addr, INDIRECT_CMD_REG, indirect_cmd);
		return ret;
	}

	while (timeout) {
		ret = handle->mdio_read(handle->mdio_priv, handle->mdio_addr, 0,
				INDIRECT_CMD_REG, &indirect_cmd);
		if (ret) {
			al_err("%s: read failed: device 0x%02x, reg %d\n",
					__func__, handle->mdio_addr, INDIRECT_CMD_REG);
			return ret;
		}

		if ((indirect_cmd & INDIRECT_CMD_BUSY) == 0)
			break;

		timeout--;
		al_udelay(1);
	}

	if (!timeout) {
		al_err("%s: indirect command timed out\n", __func__);
		return -ETIMEDOUT;
	}

	al_dbg("%s: write: device 0x%02x, reg %d, val 0x%04x\n",
			__func__, device, reg, val);

	return 0;
}

static int switch_phy_write_aux(struct al_eth_switch_handle *handle,
		unsigned int device, unsigned int reg, uint16_t val)
{
	int timeout = SWITCH_INDIRECT_CMD_TIMEOUT_US;
	uint16_t smi_cmd;
	int ret;

	ret = al_eth_switch_mdio_write(handle, SWITCH_DEVICE_GLOBAL2,
			SWITCH_REG_GLOBAL2_SMI_DATA, val);
	if (ret)
		return ret;

	smi_cmd = INDIRECT_CMD_PREPARE(device, reg, INDIRECT_CMD_OPCODE_WRITE);
	ret = al_eth_switch_mdio_write(handle, SWITCH_DEVICE_GLOBAL2,
			SWITCH_REG_GLOBAL2_SMI_CMD, smi_cmd);
	if (ret)
		return ret;

	while (timeout) {
		ret = al_eth_switch_mdio_read(handle, SWITCH_DEVICE_GLOBAL2,
				SWITCH_REG_GLOBAL2_SMI_CMD, &smi_cmd);
		if (ret)
			return ret;

		if ((smi_cmd & INDIRECT_CMD_BUSY) == 0)
			break;

		timeout--;
		al_udelay(1);
	}

	if (!timeout) {
		al_err("%s: indirect command timed out\n", __func__);
		return -ETIMEDOUT;
	}

	return 0;
}

int al_eth_switch_phy_read(struct al_eth_switch_handle *handle,
		unsigned int device, unsigned int page, unsigned int reg, uint16_t *val)
{
	int timeout = SWITCH_INDIRECT_CMD_TIMEOUT_US;
	uint16_t smi_cmd;
	int ret;

	al_assert(handle);
	al_assert(device <= SWITCH_DEVICE_LAST);

	ret = switch_phy_write_aux(handle, device + handle->phy_addr_offset,
		SWITCH_PHY_REG_PAGE_ADDR, page);
	if (ret)
		return ret;

	smi_cmd = INDIRECT_CMD_PREPARE(device + handle->phy_addr_offset,
		reg, INDIRECT_CMD_OPCODE_READ);
	ret = al_eth_switch_mdio_write(handle, SWITCH_DEVICE_GLOBAL2,
			SWITCH_REG_GLOBAL2_SMI_CMD, smi_cmd);
	if (ret)
		return ret;

	while (timeout) {
		ret = al_eth_switch_mdio_read(handle, SWITCH_DEVICE_GLOBAL2,
				SWITCH_REG_GLOBAL2_SMI_CMD, &smi_cmd);
		if (ret)
			return ret;

		if ((smi_cmd & INDIRECT_CMD_BUSY) == 0)
			break;

		timeout--;
		al_udelay(1);
	}

	if (!timeout) {
		al_err("%s: indirect command timed out\n", __func__);
		return -ETIMEDOUT;
	}

	ret = al_eth_switch_mdio_read(handle, SWITCH_DEVICE_GLOBAL2,
			SWITCH_REG_GLOBAL2_SMI_DATA, val);
	if (ret)
		return ret;

	return 0;
}

int al_eth_switch_phy_write(struct al_eth_switch_handle *handle,
		unsigned int device, unsigned int page, unsigned int reg, uint16_t val)
{
	int ret;

	al_assert(handle);
	al_assert(device <= SWITCH_DEVICE_LAST);

	ret = switch_phy_write_aux(handle, device + handle->phy_addr_offset,
		SWITCH_PHY_REG_PAGE_ADDR, page);
	if (ret)
		return ret;

	ret = switch_phy_write_aux(handle, device + handle->phy_addr_offset, reg, val);
	if (ret)
		return ret;

	return 0;
}

int al_eth_switch_vlan_clear(struct al_eth_switch_handle *handle, unsigned int port)
{
	int rc;

	al_assert(handle);
	al_assert(port < AL_ETH_SWITCH_MAX_PORTS);

	rc = al_eth_switch_mdio_write(handle, SWITCH_PORT2DEVICE(port),
			SWITCH_REG_PORT_VLAN_MAP, 0);

	return rc;
}

int al_eth_switch_vlan_enable(struct al_eth_switch_handle *handle, unsigned int src_port,
		unsigned int dst_port)
{
	uint16_t val;
	int rc;

	al_assert(handle);
	al_assert(src_port < AL_ETH_SWITCH_MAX_PORTS);
	al_assert(dst_port < AL_ETH_SWITCH_MAX_PORTS);

	al_dbg("%s: src_port %d dst_port %d\n", __func__, src_port, dst_port);

	if (src_port == dst_port)
		return 0;

	/* Turn on vlan map, and configure map of src_port->dst_port */
	rc = al_eth_switch_mdio_read(handle, SWITCH_PORT2DEVICE(src_port),
			SWITCH_REG_PORT_VLAN_MAP, &val);
	if (rc)
		return rc;

	val |= ((1 << SWITCH_REG_PORT_VLAN_FORCE_MAP) + (1 << dst_port));
	rc = al_eth_switch_mdio_write(handle, SWITCH_PORT2DEVICE(src_port),
			SWITCH_REG_PORT_VLAN_MAP, val);
	if (rc)
		return rc;

	return 0;
}

int al_eth_switch_vlan_enable_all(struct al_eth_switch_handle *handle, unsigned int port)
{
	uint16_t val;
	int rc;

	al_assert(handle);
	al_assert(port < AL_ETH_SWITCH_MAX_PORTS);

	rc = al_eth_switch_mdio_read(handle, SWITCH_PORT2DEVICE(port),
			SWITCH_REG_PORT_VLAN_MAP, &val);
	if (rc)
		return rc;
	val |= (SWITCH_REG_PORT_VLAN_TABLE_MASK & (~(1 << port)));
	rc = al_eth_switch_mdio_write(handle, SWITCH_PORT2DEVICE(port),
			SWITCH_REG_PORT_VLAN_MAP, val);
	if (rc)
		return rc;

	return 0;
}

int al_eth_switch_port_enable(struct al_eth_switch_handle *handle,
		unsigned int port, al_bool is_enabled)
{
	uint16_t val;
	int rc;

	al_assert(handle);
	al_assert(port < AL_ETH_SWITCH_MAX_PORTS);

	al_dbg("%s: port %d is_enabled %d\n", __func__, port, is_enabled);

	if (is_enabled) {
		/* Deassert phy reset */
		rc = al_eth_switch_phy_read(handle, port, SWITCH_PHY_CTRL_PAGE,
				SWITCH_PHY_CTRL_REG, &val);
		if (rc)
			return rc;

		val &= ~(1 << SWITCH_PHY_CTRL_RESET_BIT);
		rc = al_eth_switch_phy_write(handle, port, SWITCH_PHY_CTRL_PAGE,
				SWITCH_PHY_CTRL_REG, val);
		if (rc)
			return rc;

		/* Enable phy */
		rc = al_eth_switch_phy_read(handle, port, SWITCH_PHY_CTRL_PAGE,
				SWITCH_PHY_CTRL_REG, &val);
		if (rc)
			return rc;

		val &= ~(1 << SWITCH_PHY_CTRL_POWER_DOWN_BIT);
		rc = al_eth_switch_phy_write(handle, port, SWITCH_PHY_CTRL_PAGE,
				SWITCH_PHY_CTRL_REG, val);
		if (rc)
			return rc;

		/* Change port status to forwarding */
		rc = al_eth_switch_mdio_read(handle, SWITCH_PORT2DEVICE(port),
				SWITCH_REG_PORT_CTRL, &val);
		if (rc)
			return rc;

		val |= SWITCH_PORT_ENABLE_FORWARDING;
		rc = al_eth_switch_mdio_write(handle, SWITCH_PORT2DEVICE(port),
				SWITCH_REG_PORT_CTRL, val);
		if (rc)
			return rc;

		rc = al_eth_switch_vlan_clear(handle, port);
		if (rc)
			return rc;
	} else {
		rc = al_eth_switch_vlan_clear(handle, port);
		if (rc)
			return rc;

		/* Disable forwarding */
		rc = al_eth_switch_mdio_read(handle, SWITCH_PORT2DEVICE(port),
				SWITCH_REG_PORT_CTRL, &val);
		if (rc)
			return rc;

		val &= ~SWITCH_PORT_ENABLE_FORWARDING;
		rc = al_eth_switch_mdio_write(handle, SWITCH_PORT2DEVICE(port),
				SWITCH_REG_PORT_CTRL, val);
		if (rc)
			return rc;

		/* Disable phy */
		rc = al_eth_switch_phy_read(handle, port, SWITCH_PHY_CTRL_PAGE,
				SWITCH_PHY_CTRL_REG, &val);
		if (rc)
			return rc;

		val |= 1 << SWITCH_PHY_CTRL_POWER_DOWN_BIT;
		rc = al_eth_switch_phy_write(handle, port, SWITCH_PHY_CTRL_PAGE,
				SWITCH_PHY_CTRL_REG, val);
		if (rc)
			return rc;

		/* Assert phy reset */
		rc = al_eth_switch_phy_read(handle, port, SWITCH_PHY_CTRL_PAGE,
				SWITCH_PHY_CTRL_REG, &val);
		val |= 1 << SWITCH_PHY_CTRL_RESET_BIT;
		rc = al_eth_switch_phy_write(handle, port, SWITCH_PHY_CTRL_PAGE,
				SWITCH_PHY_CTRL_REG, val);
		if (rc)
			return rc;
	}

	return 0;
}
