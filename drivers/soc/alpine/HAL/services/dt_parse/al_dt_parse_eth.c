/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#include "al_hal_common.h"
#include "al_dt_parse_eth.h"
#include "al_dt_parse_internal.h"

/* To be used for printing 'ret' value as "(N/A)" */
#define RET_NA_VAL(ret)	\
	((ret) ? " (N/A)" : "")

/* To be used only inside retimers parsing loop */
#define RETIMER_PRI(format, ...)	\
	"\tRetimer %u " format, i, ##__VA_ARGS__

/* To be used only inside SerDes lane parsing loop */
#define SERDES_LANE_PRI(format, ...)	\
	"\tSerDes Lane %u " format, i, ##__VA_ARGS__

/* To be used only inside SerDes lane Retimer params parsing function */
#define SERDES_LANE_RETIMER_PRI(format, ...)	\
	"\t\t%s Retimer " format, (is_rx ? "RX" : "TX"), ##__VA_ARGS__

/*******************************************************************************
 ** Helper Functions
 *******************************************************************************/
static void al_dt_parse_eth_lm_lane_retimer(
	const struct al_dt_parse_handle *handle,
	const char *path,
	al_bool is_rx,
	struct al_eth_v4_lm_lane_retimer_params *lane_retimer,
	al_bool *retimer_exists)
{
	const void *node_context;
	uint32_t default_val32;
	uint32_t val32;
	int ret;

	*retimer_exists = AL_FALSE;

	ret = handle->node_open(handle->dt_context, path, &node_context);
	if (ret) {
		dt_debug(SERDES_LANE_RETIMER_PRI("not exists\n"));
		return;
	}

	*retimer_exists = AL_TRUE;
	dt_debug(SERDES_LANE_RETIMER_PRI("exists\n"));

	/* Retimer index */
	default_val32 = 0;
	ret = al_dt_parse_property_u32_get(handle, node_context, "idx", &val32, &default_val32);
	lane_retimer->idx = val32;
	dt_debug(SERDES_LANE_RETIMER_PRI("index: %u%s\n", lane_retimer->idx, RET_NA_VAL(ret)));

	/* Retimer channel */
	default_val32 = 0;
	ret = al_dt_parse_property_u32_get(handle, node_context, "channel", &val32, &default_val32);
	lane_retimer->channel = val32;
	dt_debug(SERDES_LANE_RETIMER_PRI("channel: %u%s\n", lane_retimer->channel,
					 RET_NA_VAL(ret)));

	handle->node_close(handle->dt_context, node_context);
}

/*******************************************************************************
 ** API Functions
 *******************************************************************************/
int al_dt_parse_eth_allow_random_mac(
	const struct al_dt_parse_handle *handle,
	unsigned int port_idx,
	unsigned int *allow_random_mac)
{
	char path[AL_DT_PARSE_PATH_LEN_MAX];
	const void *node_context;
	int ret;

	al_assert(handle);
	*allow_random_mac = 0;

	dt_debug("Ethernet port %u:\n", port_idx);

	al_sprintf(path, AL_DT_PARSE_ETH_PORT_FORMAT, port_idx);

	ret = handle->node_open(handle->dt_context, path, &node_context);
	if (ret) {
		al_err("Ethernet port %u DT node not found\n", port_idx);
		return -EINVAL;
	}

	*allow_random_mac = al_dt_parse_property_str_eq(handle, node_context, "allow-random-mac", "enabled");
	handle->node_close(handle->dt_context, node_context);

	dt_debug("\tallow-random-mac: %s\n", (*allow_random_mac ? "enabled" : "disabled"));

	return 0;
}

int al_dt_parse_eth_v4_lm_params(
	const struct al_dt_parse_handle *handle,
	unsigned int port_idx,
	struct al_eth_v4_lm_params *lm_params,
	al_bool *found)
{
	char path[AL_DT_PARSE_PATH_LEN_MAX];
	const void *node_context;
	al_bool retimer_is_used[AL_ARR_SIZE(lm_params->retimers)] = {0};
	unsigned int i;
	int ret;

	al_assert(handle);
	al_assert(lm_params);
	al_assert(found);

	*found = AL_FALSE;

	dt_debug("Ethernet port %u:\n", port_idx);

	al_sprintf(path, AL_DT_PARSE_ETH_PORT_FORMAT, port_idx);

	ret = handle->node_open(handle->dt_context, path, &node_context);
	if (ret) {
		al_err("Ethernet port %u DT node not found\n", port_idx);
		return -EINVAL;
	}

	*found = al_dt_parse_property_str_eq(handle, node_context, "status", "enabled");
	handle->node_close(handle->dt_context, node_context);

	dt_debug("\tstatus: %s\n", (*found ? "enabled" : "disabled"));

	if (!(*found))
		return 0;

	/* Lanes params */
	lm_params->lanes_num = 0;

	for (i = 0; i < AL_ETH_V4_LM_LANE_MAX; i++) {
		struct al_eth_v4_lm_lane_params *lane = &lm_params->lanes[i];
		uint32_t default_val32;
		uint32_t val32;
		al_bool retimer_exists;

		/* Find node */
		al_sprintf(path, AL_DT_PARSE_ETH_PORT_SERDES_LANE_FORMAT, port_idx, i);

		ret = handle->node_open(handle->dt_context, path, &node_context);
		if (ret)
			break;

		lm_params->lanes_num++;
		dt_debug(SERDES_LANE_PRI("node found\n"));

		/* Lane number */
		default_val32 = 0;
		ret = al_dt_parse_property_u32_get(handle, node_context, "lane",
			&val32, &default_val32);
		lane->lane_idx = val32;
		dt_debug(SERDES_LANE_PRI("lane: %u%s\n", val32, RET_NA_VAL(ret)));

		/* SFP presence GPIO  */
		default_val32 = 0;
		ret = al_dt_parse_property_u32_get(handle, node_context, "gpio-sfp-present",
			&val32, &default_val32);
		lane->sfp_presence_exists = (ret == 0 ? AL_TRUE : AL_FALSE);
		lane->sfp_presence_gpio = val32;

		if (ret == 0)
			dt_debug(SERDES_LANE_PRI("SFP presence GPIO: %u\n", val32));
		else
			dt_debug(SERDES_LANE_PRI("SFP presence GPIO: Not exists\n"));

		handle->node_close(handle->dt_context, node_context);

		/* RX Retimer */
		al_sprintf(path, AL_DT_PARSE_ETH_PORT_SERDES_LANE_RX_RETIMER_FORMAT, port_idx, i);
		al_dt_parse_eth_lm_lane_retimer(handle, path,
			AL_TRUE, &lane->rx_retimer, &retimer_exists);

		if (retimer_exists)
			retimer_is_used[lane->rx_retimer.idx] = AL_TRUE;

		/* TX Retimer */
		al_sprintf(path, AL_DT_PARSE_ETH_PORT_SERDES_LANE_TX_RETIMER_FORMAT, port_idx, i);
		al_dt_parse_eth_lm_lane_retimer(handle, path,
			AL_FALSE, &lane->tx_retimer, &retimer_exists);

		if (retimer_exists)
			retimer_is_used[lane->tx_retimer.idx] = AL_TRUE;
	}

	/* Retimers */
	lm_params->retimers_num = 0;

	for (i = 0; i < AL_ARR_SIZE(lm_params->retimers); i++) {
		struct al_eth_lm_retimer_params *retimer = &lm_params->retimers[i];
		const void *property_context;
		const char *str;
		uint32_t default_val32;
		uint32_t val32;

		retimer->type = AL_ETH_LM_RETIMER_TYPE_NONE;

		if (!retimer_is_used[i])
			continue;

		/* Find node */
		al_sprintf(path, AL_DT_PARSE_ETH_RETIMER_FORMAT, i);

		ret = handle->node_open(handle->dt_context, path, &node_context);
		if (ret)
			break;

		lm_params->retimers_num++;
		dt_debug(RETIMER_PRI("node found\n"));

		/* Retimer exists? */
		if (!al_dt_parse_property_str_eq(handle, node_context, "exist", "enabled")) {
			dt_debug(RETIMER_PRI("exist: no\n"));

			handle->node_close(handle->dt_context, node_context);
			continue;
		}

		dt_debug(RETIMER_PRI("exist: yes\n"));

		/* Type */
		ret = al_dt_parse_property_str_open(handle, node_context, "type",
			&property_context, &str);
		if (ret == 0) {
			if (!al_strcmp(str, "br410")) {
				retimer->type = AL_ETH_LM_RETIMER_TYPE_BR_410;
			} else if (!al_strcmp(str, "ds25")) {
				retimer->type = AL_ETH_LM_RETIMER_TYPE_DS_25;
			} else {
				/* for backward compatibility assume BR_210
				 * retimer in case type is not exist.
				 */
				retimer->type = AL_ETH_LM_RETIMER_TYPE_BR_210;
			}

			dt_debug(RETIMER_PRI("type: %s\n", str));

			al_dt_parse_property_str_close(handle, node_context, property_context);
		} else  {
			retimer->type = AL_ETH_RETIMER_BR_210;
			dt_debug(RETIMER_PRI("type is invalid (setting BR210)\n"));
		}

		/* I2C Master */
		ret = al_dt_parse_property_str_open(handle, node_context, "i2c-master",
			&property_context, &str);
		if (ret == 0) {
			if (!al_strcmp(str, "gen")) {
				retimer->i2c_master = AL_I2C_MASTER_GEN;
			} else {
				/* Fallback to pre-loader */
				retimer->i2c_master = AL_I2C_MASTER_PLD;
			}

			dt_debug(RETIMER_PRI("I2C master: %s\n", str));

			al_dt_parse_property_str_close(handle, node_context, property_context);
		} else  {
			retimer->i2c_master = AL_I2C_MASTER_PLD;
			dt_debug(RETIMER_PRI("I2C master is invalid (setting PLD)\n"));
		}

		/* I2C bus */
		default_val32 = 0;
		ret = al_dt_parse_property_u32_get(handle, node_context, "i2c-bus",
			&val32, &default_val32);
		retimer->i2c_bus = val32;
		dt_debug(RETIMER_PRI("I2C bus ID: %u%s\n", retimer->i2c_bus, RET_NA_VAL(ret)));

		/* I2C address */
		default_val32 = 0;
		ret = al_dt_parse_property_u32_get(handle, node_context, "i2c-addr",
			&val32, &default_val32);
		retimer->i2c_addr = val32;
		dt_debug(RETIMER_PRI("I2C address: %u%s\n", retimer->i2c_addr, RET_NA_VAL(ret)));

		handle->node_close(handle->dt_context, node_context);
	}

	return 0;
}
