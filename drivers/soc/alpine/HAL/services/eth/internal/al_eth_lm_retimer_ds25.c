/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#include "al_eth_lm_retimer.h"
#include "al_eth_lm_retimer_internal.h"

/*******************************************************************************
 ** Retimer Registers
 ******************************************************************************/
#define LM_DS25_CHANNEL_EN_REG		0xff
#define LM_DS25_CHANNEL_EN_MASK		0x03
#define LM_DS25_CHANNEL_EN_VAL		0x01

#define LM_DS25_CHANNEL_SEL_REG		0xfc
#define LM_DS25_CHANNEL_SEL_MASK	0xff

#define LM_DS25_CDR_RESET_REG		0x0a
#define LM_DS25_CDR_RESET_MASK		0x0c
#define LM_DS25_CDR_RESET_ASSERT	0x0c
#define LM_DS25_CDR_RESET_RELEASE	0x00

#define LM_DS25_SIGNAL_DETECT_REG	0x78
#define LM_DS25_SIGNAL_DETECT_MASK	0x20

#define LM_DS25_CDR_LOCK_REG		0x78
#define LM_DS25_CDR_LOCK_MASK		0x10

#define LM_DS25_DRV_PD_REG		0x15
#define LM_DS25_DRV_PD_MASK		0x08

#define LM_DS25_CDR_STATUS_CTRL_REG	0xc
#define LM_DS25_CDR_STATUS_CTRL_DIV_MASK 0xff
#define LM_DS25_CDR_STATUS_CTRL_DIV_VAL	0x30
#define LM_DS25_CDR_STATUS_REG		0x2
#define LM_DS25_CDR_STATUS_DIV_10G	0x9
#define LM_DS25_CDR_STATUS_RX_EQ	0x40

/*******************************************************************************
 ** Registers configuration sequences
 ******************************************************************************/
struct retimer_config_reg {
	uint8_t addr;
	uint8_t value;
	uint8_t mask;
};

/** 25G TX channel */
static struct retimer_config_reg retimer_ds25_25g_mode_tx_ch[] = {
	{.addr = 0x2F, .value = 0x54, .mask = 0xff },
	{.addr = 0x31, .value = 0x20, .mask = 0xff },
	{.addr = 0x1E, .value = 0xE9, .mask = 0xff },
	{.addr = 0x1F, .value = 0x0B, .mask = 0xff },
	{.addr = 0xA6, .value = 0x43, .mask = 0xff },
	{.addr = 0x2A, .value = 0x5A, .mask = 0xff },
	{.addr = 0x2B, .value = 0x0A, .mask = 0xff },
	{.addr = 0x2C, .value = 0xF6, .mask = 0xff },
	{.addr = 0x70, .value = 0x05, .mask = 0xff },
	{.addr = 0x6A, .value = 0x21, .mask = 0xff },
	{.addr = 0x35, .value = 0x0F, .mask = 0xff },
	{.addr = 0x12, .value = 0x83, .mask = 0xff },
	{.addr = 0x9C, .value = 0x24, .mask = 0xff },
	{.addr = 0x98, .value = 0x00, .mask = 0xff },
	{.addr = 0x42, .value = 0x50, .mask = 0xff },
	{.addr = 0x44, .value = 0x90, .mask = 0xff },
	{.addr = 0x45, .value = 0xC0, .mask = 0xff },
	{.addr = 0x46, .value = 0xD0, .mask = 0xff },
	{.addr = 0x47, .value = 0xD1, .mask = 0xff },
	{.addr = 0x48, .value = 0xD5, .mask = 0xff },
	{.addr = 0x49, .value = 0xD8, .mask = 0xff },
	{.addr = 0x4A, .value = 0xEA, .mask = 0xff },
	{.addr = 0x4B, .value = 0xF7, .mask = 0xff },
	{.addr = 0x4C, .value = 0xFD, .mask = 0xff },
	{.addr = 0x8E, .value = 0x00, .mask = 0xff },
	{.addr = 0x3D, .value = 0x94, .mask = 0xff },
	{.addr = 0x3F, .value = 0x40, .mask = 0xff },
	{.addr = 0x3E, .value = 0x43, .mask = 0xff },
};

/** 25G RX channel */
static struct retimer_config_reg retimer_ds25_25g_mode_rx_ch[] = {
	{.addr = 0x0A, .value = 0x0C, .mask = 0xff},
	{.addr = 0x2F, .value = 0x54, .mask = 0xff},
	{.addr = 0x31, .value = 0x40, .mask = 0xff},
	{.addr = 0x1E, .value = 0xE3, .mask = 0xff},
	{.addr = 0x1F, .value = 0x0B, .mask = 0xff},
	{.addr = 0xA6, .value = 0x43, .mask = 0xff},
	{.addr = 0x2A, .value = 0x5A, .mask = 0xff},
	{.addr = 0x2B, .value = 0x0A, .mask = 0xff},
	{.addr = 0x2C, .value = 0xF6, .mask = 0xff},
	{.addr = 0x70, .value = 0x05, .mask = 0xff},
	{.addr = 0x6A, .value = 0x21, .mask = 0xff},
	{.addr = 0x35, .value = 0x0F, .mask = 0xff},
	{.addr = 0x12, .value = 0x83, .mask = 0xff},
	{.addr = 0x9C, .value = 0x24, .mask = 0xff},
	{.addr = 0x98, .value = 0x00, .mask = 0xff},
	{.addr = 0x42, .value = 0x50, .mask = 0xff},
	{.addr = 0x44, .value = 0x90, .mask = 0xff},
	{.addr = 0x45, .value = 0xC0, .mask = 0xff},
	{.addr = 0x46, .value = 0xD0, .mask = 0xff},
	{.addr = 0x47, .value = 0xD1, .mask = 0xff},
	{.addr = 0x48, .value = 0xD5, .mask = 0xff},
	{.addr = 0x49, .value = 0xD8, .mask = 0xff},
	{.addr = 0x4A, .value = 0xEA, .mask = 0xff},
	{.addr = 0x4B, .value = 0xF7, .mask = 0xff},
	{.addr = 0x4C, .value = 0xFD, .mask = 0xff},
	{.addr = 0x8E, .value = 0x00, .mask = 0xff},
	{.addr = 0x3D, .value = 0x94, .mask = 0xff},
	{.addr = 0x3F, .value = 0x40, .mask = 0xff},
	{.addr = 0x3E, .value = 0x43, .mask = 0xff},
	{.addr = 0x0A, .value = 0x00, .mask = 0xff},
};

/** Auto mode TX channel */
static struct retimer_config_reg retimer_ds25_auto_mode_tx_ch[] = {
	{.addr = 0x0A, .value = 0x0C, .mask = 0xff },
	{.addr = 0x2F, .value = 0x64, .mask = 0xff },
	{.addr = 0x31, .value = 0x20, .mask = 0xff },
	{.addr = 0x1E, .value = 0xE9, .mask = 0xff },
	{.addr = 0x1F, .value = 0x0B, .mask = 0xff },
	{.addr = 0xA6, .value = 0x43, .mask = 0xff },
	{.addr = 0x2A, .value = 0x5A, .mask = 0xff },
	{.addr = 0x2B, .value = 0x0A, .mask = 0xff },
	{.addr = 0x2C, .value = 0xF6, .mask = 0xff },
	{.addr = 0x70, .value = 0x05, .mask = 0xff },
	{.addr = 0x6A, .value = 0x23, .mask = 0xff },
	{.addr = 0x35, .value = 0x0F, .mask = 0xff },
	{.addr = 0x12, .value = 0x83, .mask = 0xff },
	{.addr = 0x9C, .value = 0x24, .mask = 0xff },
	{.addr = 0x98, .value = 0x00, .mask = 0xff },
	{.addr = 0x42, .value = 0x50, .mask = 0xff },
	{.addr = 0x44, .value = 0x90, .mask = 0xff },
	{.addr = 0x45, .value = 0xC0, .mask = 0xff },
	{.addr = 0x46, .value = 0xD0, .mask = 0xff },
	{.addr = 0x47, .value = 0xD1, .mask = 0xff },
	{.addr = 0x48, .value = 0xD5, .mask = 0xff },
	{.addr = 0x49, .value = 0xD8, .mask = 0xff },
	{.addr = 0x4A, .value = 0xEA, .mask = 0xff },
	{.addr = 0x4B, .value = 0xF7, .mask = 0xff },
	{.addr = 0x4C, .value = 0xFD, .mask = 0xff },
	{.addr = 0x8E, .value = 0x00, .mask = 0xff },
	{.addr = 0x3D, .value = 0x94, .mask = 0xff },
	{.addr = 0x3F, .value = 0x40, .mask = 0xff },
	{.addr = 0x3E, .value = 0x43, .mask = 0xff },
	{.addr = 0x0A, .value = 0x00, .mask = 0xff },
};

/** Auto mode RX channel */
static struct retimer_config_reg retimer_ds25_auto_mode_rx_ch[] = {
	{.addr = 0x0A, .value = 0x0C, .mask = 0xff},
	{.addr = 0x2F, .value = 0x64, .mask = 0xff},
	{.addr = 0x31, .value = 0x40, .mask = 0xff},
	{.addr = 0x1E, .value = 0xE3, .mask = 0xff},
	{.addr = 0x1F, .value = 0x0B, .mask = 0xff},
	{.addr = 0xA6, .value = 0x43, .mask = 0xff},
	{.addr = 0x2A, .value = 0x5A, .mask = 0xff},
	{.addr = 0x2B, .value = 0x0A, .mask = 0xff},
	{.addr = 0x2C, .value = 0xF6, .mask = 0xff},
	{.addr = 0x70, .value = 0x05, .mask = 0xff},
	{.addr = 0x6A, .value = 0x23, .mask = 0xff},
	{.addr = 0x35, .value = 0x0F, .mask = 0xff},
	{.addr = 0x12, .value = 0x83, .mask = 0xff},
	{.addr = 0x9C, .value = 0x24, .mask = 0xff},
	{.addr = 0x98, .value = 0x00, .mask = 0xff},
	{.addr = 0x42, .value = 0x50, .mask = 0xff},
	{.addr = 0x44, .value = 0x90, .mask = 0xff},
	{.addr = 0x45, .value = 0xC0, .mask = 0xff},
	{.addr = 0x46, .value = 0xD0, .mask = 0xff},
	{.addr = 0x47, .value = 0xD1, .mask = 0xff},
	{.addr = 0x48, .value = 0xD5, .mask = 0xff},
	{.addr = 0x49, .value = 0xD8, .mask = 0xff},
	{.addr = 0x4A, .value = 0xEA, .mask = 0xff},
	{.addr = 0x4B, .value = 0xF7, .mask = 0xff},
	{.addr = 0x4C, .value = 0xFD, .mask = 0xff},
	{.addr = 0x8E, .value = 0x00, .mask = 0xff},
	{.addr = 0x3D, .value = 0x94, .mask = 0xff},
	{.addr = 0x3F, .value = 0x40, .mask = 0xff},
	{.addr = 0x3E, .value = 0x43, .mask = 0xff},
	{.addr = 0x0A, .value = 0x00, .mask = 0xff},
};

/** 10G */
static struct retimer_config_reg retimer_ds25_10g_mode[] = {
	/* Assert CDR reset (6.3) */
	{.addr = 0x0A, .value = 0x0C, .mask = 0x0C},
	/* Select 10.3125Gbps standard rate mode (6.6) */
	{.addr = 0x2F, .value = 0x00, .mask = 0xF0},
	/* Enable loop filter auto-adjust */
	{.addr = 0x1F, .value = 0x08, .mask = 0x08},
	/* Set Adapt Mode 1 (6.13) */
	{.addr = 0x31, .value = 0x20, .mask = 0x60},
	/* Disable the DFE since most applications do not need it (6.18) */
	{.addr = 0x1E, .value = 0x08, .mask = 0x08},
	/* Release CDR reset (6.4) */
	{.addr = 0x0A, .value = 0x00, .mask = 0x0C},
	/* Enable FIR (6.12) */
	{.addr = 0x3D, .value = 0x80, .mask = 0x80},
	/* Set Main-cursor tap sign to positive (6.12) */
	{.addr = 0x3D, .value = 0x00, .mask = 0x40},
	/* Set Post-cursor tap sign to negative (6.12) */
	{.addr = 0x3F, .value = 0x40, .mask = 0x40},
	/* Set Pre-cursor tap sign to negative (6.12) */
	{.addr = 0x3E, .value = 0x40, .mask = 0x40},
	/* Set Main-cursor tap magnitude to 13 (6.12) */
	{.addr = 0x3D, .value = 0x0D, .mask = 0x1F},
};

/*******************************************************************************
 ** Helper Functions
 ******************************************************************************/

static int al_eth_lm_retimer_ds25_write_reg(struct al_eth_lm_retimer	*handle,
					    uint8_t			reg_addr,
					    uint8_t			reg_mask,
					    uint8_t			reg_value)
{
	uint8_t reg = 0;
	int rc;

	al_assert((reg_mask & reg_value) == reg_value);

	if (reg_mask != 0xff) {
		rc = handle->i2c_read(handle->i2c_context,
					handle->i2c_master,
					handle->i2c_bus,
					handle->i2c_addr,
					reg_addr,
					&reg);

		if (rc)
			return -EIO;

		reg &= ~(reg_mask);
	}

	reg |= reg_value;

	rc = handle->i2c_write(handle->i2c_context,
				   handle->i2c_master,
				   handle->i2c_bus,
				   handle->i2c_addr,
				   reg_addr,
				   reg);

	if (rc)
		return -EIO;

	return 0;
}

static void al_eth_lm_retimer_ds25_channel_deselect(struct al_eth_lm_retimer	*handle)
{

	if (handle->retimer_unlock != NULL)
		handle->retimer_unlock(handle->retimer_lock_context);

}

static int al_eth_lm_retimer_ds25_channel_select(struct al_eth_lm_retimer	*handle,
						 uint8_t			channel)
{
	int rc = 0;

	if (handle->retimer_lock != NULL)
		handle->retimer_lock(handle->retimer_lock_context);

	/* Write to specific channel */
	rc = al_eth_lm_retimer_ds25_write_reg(handle,
					      LM_DS25_CHANNEL_SEL_REG,
					      LM_DS25_CHANNEL_SEL_MASK,
					      (1 << channel));
	if (rc)
		goto config_done;

	rc = al_eth_lm_retimer_ds25_write_reg(handle,
					      LM_DS25_CHANNEL_EN_REG,
					      LM_DS25_CHANNEL_EN_MASK,
					      LM_DS25_CHANNEL_EN_VAL);

config_done:
	if (rc)
		al_eth_lm_retimer_ds25_channel_deselect(handle);

	return rc;
}

static int al_eth_lm_retimer_ds25_channel_config(struct al_eth_lm_retimer	*handle,
						 uint8_t			channel,
						 struct retimer_config_reg	*config,
						 uint8_t			config_size)
{
	uint8_t i;
	int rc = 0;

	rc = al_eth_lm_retimer_ds25_channel_select(handle, channel);
	if (rc)
		goto config_done_no_unlock;

	for (i = 0; i < config_size; i++) {
		rc = al_eth_lm_retimer_ds25_write_reg(handle,
						      config[i].addr,
						      config[i].mask,
						      config[i].value);

		if (rc)
			goto config_done;
	}

config_done:
	retimer_debug("%s: retimer channel config done for channel %d\n", __func__, channel);

	al_eth_lm_retimer_ds25_channel_deselect(handle);

config_done_no_unlock:
	if (rc)
		al_err("%s: failed to access to the retimer\n", __func__);

	return rc;
}

/*******************************************************************************
 ** Callbacks
 ******************************************************************************/

static int al_eth_lm_retimer_ds25_config(struct al_eth_lm_retimer *handle,
	unsigned int channel,
	struct al_eth_lm_retimer_config_params *params)
{
	int rc = 0;
	struct retimer_config_reg *config_regs = NULL;
	uint32_t config_regs_size;

	if (params->speed == AL_ETH_LM_RETIMER_SPEED_AUTO) {
		if (params->dir == AL_ETH_LM_RETIMER_CH_DIR_TX) {
			config_regs = retimer_ds25_auto_mode_tx_ch;
			config_regs_size = AL_ARR_SIZE(retimer_ds25_auto_mode_tx_ch);
		} else {
			config_regs = retimer_ds25_auto_mode_rx_ch;
			config_regs_size = AL_ARR_SIZE(retimer_ds25_auto_mode_rx_ch);
		}
	} else if (params->speed == AL_ETH_LM_RETIMER_SPEED_25G) {
		if (params->dir == AL_ETH_LM_RETIMER_CH_DIR_TX) {
			config_regs = retimer_ds25_25g_mode_tx_ch;
			config_regs_size = AL_ARR_SIZE(retimer_ds25_25g_mode_tx_ch);
		} else {
			config_regs = retimer_ds25_25g_mode_rx_ch;
			config_regs_size = AL_ARR_SIZE(retimer_ds25_25g_mode_rx_ch);
		}
	} else {
		if (params->dir == AL_ETH_LM_RETIMER_CH_DIR_TX) {
			config_regs = retimer_ds25_10g_mode;
			config_regs_size = AL_ARR_SIZE(retimer_ds25_10g_mode);

		} else {
			config_regs = retimer_ds25_10g_mode;
			config_regs_size = AL_ARR_SIZE(retimer_ds25_10g_mode);
		}
	}

	rc = al_eth_lm_retimer_ds25_channel_config(handle,
					channel,
					config_regs,
					config_regs_size);

	return rc;
}

static int al_eth_lm_retimer_ds25_cdr_reset(struct al_eth_lm_retimer *handle, unsigned int channel)
{
	int rc = 0;

	retimer_debug("Perform CDR reset to channel %d\n", channel);

	rc = al_eth_lm_retimer_ds25_channel_select(handle, channel);
	if (rc)
		goto config_done_no_unlock;

	rc = al_eth_lm_retimer_ds25_write_reg(handle,
					      LM_DS25_CDR_RESET_REG,
					      LM_DS25_CDR_RESET_MASK,
					      LM_DS25_CDR_RESET_ASSERT);

	if (rc)
		goto config_done;

	rc = al_eth_lm_retimer_ds25_write_reg(handle,
					      LM_DS25_CDR_RESET_REG,
					      LM_DS25_CDR_RESET_MASK,
					      LM_DS25_CDR_RESET_RELEASE);

	if (rc)
		goto config_done;

config_done:
	al_eth_lm_retimer_ds25_channel_deselect(handle);

config_done_no_unlock:
	if (rc)
		al_err("%s: failed to access to the retimer\n", __func__);

	return rc;
}

static int al_eth_lm_retimer_ds25_signal_detect(struct al_eth_lm_retimer *handle,
						    unsigned int channel,
						    al_bool *detected)
{
	int rc = 0;
	uint8_t reg;

	*detected = AL_FALSE;

	rc = al_eth_lm_retimer_ds25_channel_select(handle, channel);
	if (rc)
		goto config_done_no_unlock;

	rc = handle->i2c_read(handle->i2c_context,
				  handle->i2c_master,
				  handle->i2c_bus,
				  handle->i2c_addr,
				  LM_DS25_SIGNAL_DETECT_REG,
				  &reg);

	if (rc)
		goto config_done;

	if (reg & LM_DS25_SIGNAL_DETECT_MASK)
		*detected = AL_TRUE;

config_done:
	al_eth_lm_retimer_ds25_channel_deselect(handle);

config_done_no_unlock:
	if (rc)
		al_err("%s: failed to access to the retimer\n", __func__);

	return rc;
}

static int al_eth_lm_retimer_ds25_cdr_lock(struct al_eth_lm_retimer *handle,
					       unsigned int channel,
					       al_bool *locked)
{
	int rc = 0;
	uint8_t reg;

	*locked = AL_FALSE;

	rc = al_eth_lm_retimer_ds25_channel_select(handle, channel);
	if (rc)
		goto config_done_no_unlock;

	rc = handle->i2c_read(handle->i2c_context,
			handle->i2c_master,
			handle->i2c_bus,
			handle->i2c_addr,
			LM_DS25_CDR_LOCK_REG,
			&reg);

	if (rc)
		goto config_done;

	if (reg & LM_DS25_CDR_LOCK_MASK)
		*locked = AL_TRUE;

config_done:
	al_eth_lm_retimer_ds25_channel_deselect(handle);

config_done_no_unlock:
	if (rc)
		al_err("%s: failed to access to the retimer\n", __func__);

	return rc;
}

static int al_eth_lm_retimer_ds25_rx_eq_done(struct al_eth_lm_retimer *handle,
					       unsigned int channel,
					       al_bool *done)
{
	int rc = 0;
	uint8_t reg;

	*done = AL_FALSE;

	rc = al_eth_lm_retimer_ds25_channel_select(handle, channel);
	if (rc)
		goto config_done_no_unlock;

	rc = handle->i2c_read(handle->i2c_context,
		handle->i2c_master,
		handle->i2c_bus,
		handle->i2c_addr,
		LM_DS25_CDR_STATUS_REG,
		&reg);
	if (rc)
		goto config_done;

	if (reg & LM_DS25_CDR_STATUS_RX_EQ)
		*done = AL_TRUE;

config_done:
	al_eth_lm_retimer_ds25_channel_deselect(handle);

config_done_no_unlock:
	if (rc)
		al_err("%s: failed to access to the retimer\n", __func__);

	return rc;
}

static int al_eth_lm_retimer_ds25_speed_get(
					struct al_eth_lm_retimer *handle,
					unsigned int channel,
					enum al_eth_lm_retimer_speed *speed)
{
	int rc = 0;
	uint8_t reg;

	*speed = AL_ETH_LM_RETIMER_SPEED_25G;

	rc = al_eth_lm_retimer_ds25_channel_select(handle, channel);
	if (rc)
		goto i2c_done_no_unlock;

	/* the following write will set the div value register LM_DS25_DIV_REG */
	rc = al_eth_lm_retimer_ds25_write_reg(
					handle,
					LM_DS25_CDR_STATUS_CTRL_REG,
					LM_DS25_CDR_STATUS_CTRL_DIV_MASK,
					LM_DS25_CDR_STATUS_CTRL_DIV_VAL);

	if (rc)
		goto i2c_done;

	rc = handle->i2c_read(handle->i2c_context,
				  handle->i2c_master,
				  handle->i2c_bus,
				  handle->i2c_addr,
				  LM_DS25_CDR_STATUS_REG,
				  &reg);

	if (rc)
		goto i2c_done;

	if (reg & LM_DS25_CDR_STATUS_DIV_10G)
		*speed = AL_ETH_LM_RETIMER_SPEED_10G;

i2c_done:
	al_eth_lm_retimer_ds25_channel_deselect(handle);

i2c_done_no_unlock:
	if (rc)
		al_err("%s: failed to access to the retimer\n", __func__);

	return rc;
}

/*******************************************************************************
 ** Internal API Functions
 ******************************************************************************/
int al_eth_lm_retimer_ds25_handle_init(
	struct al_eth_lm_retimer *handle,
	const struct al_eth_lm_retimer_params *params)
{
	al_assert(handle);
	al_assert(params);

	handle->config = al_eth_lm_retimer_ds25_config;
	handle->reset = al_eth_lm_retimer_ds25_cdr_reset;
	handle->signal_detect = al_eth_lm_retimer_ds25_signal_detect;
	handle->cdr_lock = al_eth_lm_retimer_ds25_cdr_lock;
	handle->rx_eq_done = al_eth_lm_retimer_ds25_rx_eq_done;
	handle->speed_detect = al_eth_lm_retimer_ds25_speed_get;

	return 0;
}
