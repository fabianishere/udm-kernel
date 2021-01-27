/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#ifndef __AL_ETH_SWITCH_H__
#define __AL_ETH_SWITCH_H__

#include "al_hal_plat_types.h"

/* Maximum number of ports supported by the switch */
#define AL_ETH_SWITCH_MAX_PORTS		8

/** Ethernet switch model */
enum al_eth_switch_model {
	AL_ETH_SWITCH_MODEL_MV88E6172 = 1,
#define AL_ETH_SWITCH_MODEL_HAS_MV88E6141
	AL_ETH_SWITCH_MODEL_MV88E6141 = 2,
};

/** Ethernet switch initialization parameters structure */
struct al_eth_switch_init_params {
	enum al_eth_switch_model model; /**< Ethernet switch model */
	int (*mdio_read)(void *priv, unsigned int phy_addr, unsigned int dev_addr,
			unsigned int regnum, uint16_t *val); /**< MDIO read function pointer,
								used to configure the switch*/
	int (*mdio_write)(void *priv, unsigned int phy_addr, unsigned int dev_addr,
			unsigned int regnum, uint16_t val); /**< MDIO write fuction pointer,
								used to configure the switch*/
	void *mdio_priv; /**< private pointer passed to mdio read/write functions */
	int mdio_addr; /**< MDIO address of the switch */
};

/** Ethernet switch handle structure */
struct al_eth_switch_handle {
	enum al_eth_switch_model model;
	int (*mdio_read)(void *priv, unsigned int phy_addr, unsigned int dev_addr,
			unsigned int regnum, uint16_t *val);
	int (*mdio_write)(void *priv, unsigned int phy_addr, unsigned int dev_addr,
			unsigned int regnum, uint16_t val);
	void *mdio_priv;
	int mdio_addr;
	int phy_addr_offset;
};

/**
 * Initialize eth switch handle
 *
 * @param handle pointer to eth switch handle structure
 * @param params initialization params
 *
 * @return  0 in case of success. otherwise on failure.
 */
int al_eth_switch_handle_init(struct al_eth_switch_handle *handle,
		struct al_eth_switch_init_params *params);

/**
 * Read eth switch mdio register
 *
 * @param handle pointer to eth switch handle structure
 * @param device internal device id
 * @param reg register address
 * @param val register value
 *
 * @return  0 in case of success. otherwise on failure.
 */
int al_eth_switch_mdio_read(struct al_eth_switch_handle *handle,
		unsigned int device, unsigned int reg, uint16_t *val);

/**
 * Write eth switch mdio register
 *
 * @param handle pointer to eth switch handle structure
 * @param device internal device id
 * @param reg register address
 * @param val register value
 *
 * @return  0 in case of success. otherwise on failure.
 */
int al_eth_switch_mdio_write(struct al_eth_switch_handle *handle,
		unsigned int device, unsigned int reg, uint16_t val);

/**
 * Read eth switch phy register
 *
 * @param handle pointer to eth switch handle structure
 * @param device internal device id
 * @param page phy page
 * @param reg phy register address
 * @param val phy register value
 *
 * @return  0 in case of success. otherwise on failure.
 */
int al_eth_switch_phy_read(struct al_eth_switch_handle *handle,
		unsigned int device, unsigned int page, unsigned int reg, uint16_t *val);

/**
 * Write eth switch phy register
 *
 * @param handle pointer to eth switch handle structure
 * @param device internal device id
 * @param page phy page
 * @param reg phy register address
 * @param val phy register value
 *
 * @return  0 in case of success. otherwise on failure.
 */
int al_eth_switch_phy_write(struct al_eth_switch_handle *handle,
		unsigned int device, unsigned int page, unsigned int reg, uint16_t val);

/**
 * Clear port-based vlan configuration for eth switch port
 *
 * @param handle pointer to eth switch handle structure
 * @param port eth switch port
 *
 * @return  0 in case of success. otherwise on failure.
 */
int al_eth_switch_vlan_clear(struct al_eth_switch_handle *handle, unsigned int port);

/**
 * Enable port-based vlan between 2 eth switch ports
 *
 * This sets up single direction forwarding: src_port->dst_port
 *
 * @param handle pointer to eth switch handle structure
 * @param src_port eth switch source port
 * @param dst_port eth switch destination port
 *
 * @return  0 in case of success. otherwise on failure.
 */
int al_eth_switch_vlan_enable(struct al_eth_switch_handle *handle, unsigned int src_port,
		unsigned int dst_port);

/**
 * Enable port-based vlan to all ports for a given eth switch port
 *
 * @param handle pointer to eth switch handle structure
 * @param port eth switch port
  *
 * @return  0 in case of success. otherwise on failure.
 */
int al_eth_switch_vlan_enable_all(struct al_eth_switch_handle *handle, unsigned int port);

/**
 * Enable eth switch port
 *
 * @param handle pointer to eth switch handle structure
 * @param port eth switch port
 * @param is_enabled AL_TRUE to enable the port, AL_FALSE to disable
 *
 * @return  0 in case of success. otherwise on failure.
 */
int al_eth_switch_port_enable(struct al_eth_switch_handle *handle,
		unsigned int port, al_bool is_enabled);

#endif
