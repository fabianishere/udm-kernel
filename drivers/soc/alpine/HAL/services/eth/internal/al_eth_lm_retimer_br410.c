/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#include "al_eth_lm_retimer.h"
#include "al_eth_lm_retimer_internal.h"

/*******************************************************************************
 ** Registers
 ******************************************************************************/
#define RETIMER_BR410_DE_EMPHASIS_CTRL_REG 0x11
#define RETIMER_BR410_DE_EMPHASIS_CTRL_MASK(channel) (0x3 << 2*(channel))
#define RETIMER_BR410_DE_EMPHASIS_CTRL_SHIFT(channel) (2*(channel))

#define RETIMER_BR410_DRIVER_VOD_CTRL_REG 0x08
#define RETIMER_BR410_DRIVER_VOD_CTRL_MASK 0xC
#define RETIMER_BR410_DRIVER_VOD_CTRL_SHIFT 2

/*******************************************************************************
 ** Registers configuration sequences
 ******************************************************************************/
static const uint32_t al_eth_retimer_boost_addr[AL_ETH_RETIMER_CHANNEL_MAX] = {
	/* AL_ETH_RETIMER_CHANNEL_A */	0x1a,
	/* AL_ETH_RETIMER_CHANNEL_B */	0x18,
	/* AL_ETH_RETIMER_CHANNEL_C */	0x16,
	/* AL_ETH_RETIMER_CHANNEL_D */	0x14,
};

/* The br410 retimer params for 1m and 3m are calibrated for EVP board */
static const struct al_eth_lm_retimer_da_config
da_config_params[AL_ETH_RETIMER_DA_LENS_MAX + 1] = {
	/* 0m */{.eq_boost = 0x0,},
	/* 1m */{.eq_boost = 0x1, .de_emphasis = 0, .vod = 0x1}, /*7.3 dB, 0dB, 0.8V*/
	/* 2m */{.eq_boost = 0x1,},
	/* 3m */{.eq_boost = 0x3, .de_emphasis = 0, .vod = 0x1}, /*12.2 dB, 0dB, 0.8V*/
	/* 5m */{.eq_boost = 0x3,},
	/*+5m */{.eq_boost = 0x7,},
};

/*******************************************************************************
 ** Callbacks
 ******************************************************************************/
static int al_eth_lm_retimer_br410_config(struct al_eth_lm_retimer *handle,
	unsigned int channel,
	struct al_eth_lm_retimer_config_params *params)
{
	int rc = 0;
	struct al_eth_lm_retimer_da_config retimer_params;
	uint32_t boost_addr;
	uint8_t reg;
	uint8_t vod_val;

	al_assert((((int)channel) >= 0) && (channel < 4));

	al_memset(&retimer_params, 0, sizeof(retimer_params));

	boost_addr = al_eth_retimer_boost_addr[channel];

	/* config eq_boost value */
	if (params->da_len <= 1)
		retimer_params = da_config_params[1];
	else
		retimer_params = da_config_params[3];

	if ((params->da_len != 1) && (params->da_len != 3)) {
		al_warn("%s: Uncalibrated cable length (%dm) for retimer_type BR410 on this "
			"board is being used! Link might not be optimal.\n",
			__func__, params->da_len);
		al_warn("Retimer params have been calibrated for 1m and 3m cables only\n");
	}

	retimer_debug("%s: Configuring eq_boost for retimer_type BR410 in channel %d "
		"(addr 0x%x) to 0x%x\n",
		__func__, channel, boost_addr,
		retimer_params.eq_boost);

	rc = handle->i2c_write(handle->i2c_context,
				   handle->i2c_master,
				   handle->i2c_bus,
				   handle->i2c_addr,
				   boost_addr,
				   retimer_params.eq_boost);

	if (rc) {
		al_err("%s: Error occurred (%d) while writing to eq_boost to retimer "
			"(bus-id %x i2c-addr %x)\n",
		       __func__, rc, handle->i2c_bus, handle->i2c_addr);
		return rc;
	}

	/* config de_emphasis value */
	rc = handle->i2c_read(handle->i2c_context,
				  handle->i2c_master,
				  handle->i2c_bus,
				  handle->i2c_addr,
				  RETIMER_BR410_DE_EMPHASIS_CTRL_REG,
				  &reg);

	if (rc) {
		al_err("%s: Error occurred (%d) while reading de_emphasis control reg from "
			"retimer (bus-id 0x%x i2c-addr 0x%x internal_reg 0x%x)\n",
			__func__, rc, handle->i2c_bus, handle->i2c_addr,
			RETIMER_BR410_DE_EMPHASIS_CTRL_REG);
		return rc;
	}

	AL_REG_FIELD_SET(reg,
			 RETIMER_BR410_DE_EMPHASIS_CTRL_MASK(channel),
			 RETIMER_BR410_DE_EMPHASIS_CTRL_SHIFT(channel),
			 retimer_params.de_emphasis);

	retimer_debug("%s: Configuring de_emphasis for retimer_type BR410 in channel %d to 0x%x\n",
		__func__, channel,
		retimer_params.de_emphasis);

	rc = handle->i2c_write(handle->i2c_context,
				  handle->i2c_master,
				  handle->i2c_bus,
				  handle->i2c_addr,
				  RETIMER_BR410_DE_EMPHASIS_CTRL_REG,
				  reg);

	if (rc) {
		al_err("%s: Error occurred (%d) while writing to de_emphasis control reg of "
			"retimer (bus-id 0x%x i2c-addr 0x%x internal_reg 0x%x)\n",
			__func__, rc, handle->i2c_bus, handle->i2c_addr,
			RETIMER_BR410_DE_EMPHASIS_CTRL_REG);
		return rc;
	}

	/* config Vod value */
	rc = handle->i2c_read(handle->i2c_context,
				  handle->i2c_master,
				  handle->i2c_bus,
				  handle->i2c_addr,
				  RETIMER_BR410_DRIVER_VOD_CTRL_REG,
				  &reg);

	if (rc) {
		al_err("%s: Error occurred (%d) while reading Vod control reg from retimer "
			"(bus-id 0x%x i2c-addr 0x%x internal_reg 0x%x)\n",
			__func__, rc, handle->i2c_bus, handle->i2c_addr,
			RETIMER_BR410_DRIVER_VOD_CTRL_REG);
		return rc;
	}

	vod_val = AL_REG_FIELD_GET(reg, RETIMER_BR410_DRIVER_VOD_CTRL_MASK,
				RETIMER_BR410_DRIVER_VOD_CTRL_SHIFT);

	if (vod_val != retimer_params.vod) {
		AL_REG_FIELD_SET(reg,
				RETIMER_BR410_DRIVER_VOD_CTRL_MASK,
				RETIMER_BR410_DRIVER_VOD_CTRL_SHIFT,
				retimer_params.vod);

		retimer_debug("%s: Configuring Vod for retimer_type BR410 in channel %d to 0x%x\n",
		__func__, channel,
		retimer_params.vod);

		rc = handle->i2c_write(handle->i2c_context,
				  handle->i2c_master,
				  handle->i2c_bus,
				  handle->i2c_addr,
				  RETIMER_BR410_DRIVER_VOD_CTRL_REG,
				  reg);

		if (rc) {
			al_err("%s: Error occurred (%d) while writing to Vod control reg in "
				"retimer (bus-id 0x%x i2c-addr 0x%x internal_reg 0x%x)\n",
				__func__, rc, handle->i2c_bus,
				handle->i2c_addr,
				RETIMER_BR410_DRIVER_VOD_CTRL_REG);
			return rc;
		}
	}

	return 0;
}

/*******************************************************************************
 ** Internal API Functions
 ******************************************************************************/
int al_eth_lm_retimer_br410_handle_init(
	struct al_eth_lm_retimer *handle,
	const struct al_eth_lm_retimer_params *params)
{
	al_assert(handle);
	al_assert(params);

	handle->config = al_eth_lm_retimer_br410_config;

	return 0;
}
