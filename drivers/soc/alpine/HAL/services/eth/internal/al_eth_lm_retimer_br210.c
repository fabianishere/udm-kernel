/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#include "al_eth_lm_retimer.h"
#include "al_eth_lm_retimer_internal.h"

/*******************************************************************************
 ** Registers configuration sequences
 ******************************************************************************/

static const uint32_t al_eth_retimer_boost_addr[AL_ETH_RETIMER_CHANNEL_MAX] = {
	/* AL_ETH_RETIMER_CHANNEL_A */	0xf,
	/* AL_ETH_RETIMER_CHANNEL_B */	0x16,
	/* AL_ETH_RETIMER_CHANNEL_C */	0x0,
	/* AL_ETH_RETIMER_CHANNEL_D */	0x0,
};

static const uint32_t al_eth_retimer_boost_lens[AL_ETH_RETIMER_DA_LENS_MAX] = {0, 1, 2, 3, 5};

static const struct al_eth_lm_retimer_da_config
da_config_params[AL_ETH_RETIMER_DA_LENS_MAX + 1] = {
	/* 0m */{.eq_boost = 0x0,},
	/* 1m */{.eq_boost = 0x1,},
	/* 2m */{.eq_boost = 0x2,},
	/* 3m */{.eq_boost = 0x3,},
	/* 5m */{.eq_boost = 0x7,},
	/*+5m */{.eq_boost = 0xb,},
};

/*******************************************************************************
 ** Callbacks
 ******************************************************************************/

static int al_eth_lm_retimer_br210_config(struct al_eth_lm_retimer *handle,
	unsigned int channel,
	struct al_eth_lm_retimer_config_params *params)
{
	int i;
	int rc = 0;
	uint8_t boost = 0;
	uint32_t boost_addr = al_eth_retimer_boost_addr[channel];

	for (i = 0; i < AL_ETH_RETIMER_DA_LENS_MAX; i++) {
		if (params->da_len <= al_eth_retimer_boost_lens[i]) {
			boost = da_config_params[i].eq_boost;
			break;
		}
	}

	if (i == AL_ETH_RETIMER_DA_LENS_MAX)
		boost = da_config_params[AL_ETH_RETIMER_DA_LENS_MAX].eq_boost;

	retimer_debug("config retimer boost in channel %d (addr %x) to 0x%x\n",
		 channel, boost_addr, boost);

	rc = handle->i2c_write(handle->i2c_context,
				   handle->i2c_master,
				   handle->i2c_bus,
				   handle->i2c_addr,
				   boost_addr,
				   boost);
	if (rc) {
		al_err("%s: Error occurred (%d) while writing retimer configuration "
			"(bus-id %x i2c-addr %x)\n",
		       __func__, rc, handle->i2c_bus, handle->i2c_addr);
		return rc;
	}

	return 0;
}

/*******************************************************************************
 ** Internal API Functions
 ******************************************************************************/
int al_eth_lm_retimer_br210_handle_init(
	struct al_eth_lm_retimer *handle,
	const struct al_eth_lm_retimer_params *params)
{
	al_assert(handle);
	al_assert(params);

	handle->config = al_eth_lm_retimer_br210_config;

	return 0;
}
