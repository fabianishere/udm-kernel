/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#include "al_eth_lm_retimer.h"
#include "internal/al_eth_lm_retimer_internal.h"

/*******************************************************************************
 ** API Functions
 ******************************************************************************/
int al_eth_lm_retimer_handle_init(
	struct al_eth_lm_retimer *handle,
	const struct al_eth_lm_retimer_params *params)
{
	int ret = 0;

	al_assert(handle);
	al_assert(params);

	al_memset(handle, 0, sizeof(*handle));

	handle->type = params->type;
	handle->i2c_master = params->i2c_master;
	handle->i2c_bus = params->i2c_bus;
	handle->i2c_addr = params->i2c_addr;
	handle->i2c_context = params->i2c_context;
	handle->i2c_read = params->i2c_read;
	handle->i2c_write = params->i2c_write;
	handle->debug = params->debug;
	handle->retimer_lock_context = params->retimer_lock_context;
	handle->retimer_lock = params->retimer_lock;
	handle->retimer_unlock = params->retimer_unlock;

	switch (handle->type) {
	case AL_ETH_LM_RETIMER_TYPE_NONE:
		/* Nothing to do */
		break;
	case AL_ETH_LM_RETIMER_TYPE_BR_210:
		ret = al_eth_lm_retimer_br210_handle_init(handle, params);
		break;
	case AL_ETH_LM_RETIMER_TYPE_BR_410:
		ret = al_eth_lm_retimer_br410_handle_init(handle, params);
		break;
	case AL_ETH_LM_RETIMER_TYPE_DS_25:
		ret = al_eth_lm_retimer_ds25_handle_init(handle, params);
		break;
	default:
		al_assert(AL_FALSE);
		break;
	}

	return ret;
}
