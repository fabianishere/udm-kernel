/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#ifndef __AL_DT_PARSE_ETH_H__
#define __AL_DT_PARSE_ETH_H__

#include "al_hal_common.h"
#include "al_dt_parse.h"

#include "al_eth_v4_lm.h"

/**
 * Annapurna Labs Ethernet related Device-tree (DT) nodes parsing functions
 */

#define AL_DT_PARSE_ETH_FORMAT					\
	"/soc/board-cfg/ethernet"

/* Ethernet Ports */
#define AL_DT_PARSE_ETH_PORT_FORMAT				\
	AL_DT_PARSE_ETH_FORMAT "/port%u"
#define AL_DT_PARSE_ETH_PORT_SERDES_LANE_FORMAT			\
	AL_DT_PARSE_ETH_PORT_FORMAT "/serdes-lane@%u"
#define AL_DT_PARSE_ETH_PORT_SERDES_LANE_RX_RETIMER_FORMAT	\
	AL_DT_PARSE_ETH_PORT_SERDES_LANE_FORMAT "/rx-retimer"
#define AL_DT_PARSE_ETH_PORT_SERDES_LANE_TX_RETIMER_FORMAT	\
	AL_DT_PARSE_ETH_PORT_SERDES_LANE_FORMAT "/tx-retimer"

/* Ethernet Retimers */
#define AL_DT_PARSE_ETH_RETIMER_FORMAT				\
	AL_DT_PARSE_ETH_FORMAT "/retimer@%u"

/*******************************************************************************
 ** API Functions
 ******************************************************************************/

/**
 * Parse Ethernet v4 Link Management parameters struct from DT
 *
 * @param handle
 *	DT Parse handle
 * @param port_idx
 *	Port index
 * @param lm_params
 *	Ethernet v4 Link Management parameters
 * @param found
 *	Will be filled with AL_TRUE if port is found
 *
 * @return
 *	0 on success, errno otherwise
 */
/*
 * List of paths and parameters that will be filled:
 * - /soc/board-cfg/ethernet/portX
 *	- exists:	Port is found if value is "enabled"
 * - /soc/board-cfg/ethernet/portX/serdes-lane@Y
 *	- lane:		TODO: not stored
 *	- gpio-sfp-present:
 *			lm_params->lane[Y].sfp_presence_exists = AL_TRUE (if property exists)
 *			lm_params->lane[Y].sfp_presence_gpio
 * - /soc/board-cfg/ethernet/portX/serdes-lane@Y/rx-retimer
 *	- idx:		lm_params->lane[Y].rx_retimer.idx
 *	- channel:	lm_params->lane[Y].rx_retimer.channel
 * - /soc/board-cfg/ethernet/portX/serdes-lane@Y/tx-retimer
 *	- idx:		lm_params->lane[Y].tx_retimer.idx
 *	- channel:	lm_params->lane[Y].tx_retimer.channel
 * - /soc/board-cfg/ethernet/retimer@X
 *	- exists:	lm_params->retimers[X].type = NONE (if not set to "enabled")
 *	- type:		lm_params->retimers[X].type
 *	- i2c-master:	lm_params->retimers[X].i2c_master
 *	- i2c-bus:	lm_params->retimers[X].i2c_bus
 *	- i2c-addr:	lm_params->retimers[X].i2c_addr
 */
int al_dt_parse_eth_v4_lm_params(
	const struct al_dt_parse_handle *handle,
	unsigned int port_idx,
	struct al_eth_v4_lm_params *lm_params,
	al_bool *found);

/**
 * Parse arandom mac generation flag from DT
 *
 * @param handle
 *	DT Parse handle
 * @param port_idx
 *	Port index
 * @param allow_random_mac
 *	Ethernet port random mac generation flag
 *
 * @return
 *	0 on success, errno otherwise
 */
/*
 * - /soc/board-cfg/ethernet/portX/allow-random-mac
 *	- enabled: allow_random_mac = 1
 *	- otherwise: allow_random_mac  = 0
 */
int al_dt_parse_eth_allow_random_mac(
	const struct al_dt_parse_handle *handle,
	unsigned int port_idx,
	unsigned int *allow_random_mac);

#endif /* __AL_DT_PARSE_ETH_H__ */
