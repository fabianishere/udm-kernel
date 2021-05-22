/*
 * Copyright 2018, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#ifdef AL_HAL_HAS_UC_CMDS

#include "al_uc_cmds.h"

#define IMAGE_CRC_SIZE				4
struct al_uc_gpio_handle uc_gpio;

static void uc_gpio_handle_init_v1_v2(struct al_uc_gpio_handle *uc_gpio)
{
	uc_gpio->value_mask = AL_UC_GPIO_PARAM_VAL_MASK;
	uc_gpio->value_shift = AL_UC_GPIO_PARAM_VAL_SHIFT;

	uc_gpio->port_num = AL_UC_GPIO_NUM_PORTS_V1_V2;
	uc_gpio->port_mask = AL_UC_GPIO_PARAM_PORT_MASK_V1_V2;
	uc_gpio->port_shift = AL_UC_GPIO_PARAM_PORT_SHIFT;

	uc_gpio->bit_num = AL_UC_GPIO_NUM_BITS_V1_V2;
	uc_gpio->bit_mask = AL_UC_GPIO_PARAM_BIT_MASK_V1_V2;
	uc_gpio->bit_shift = AL_UC_GPIO_PARAM_BIT_SHIFT_V1_V2;
}

static void uc_gpio_handle_init_v3(struct al_uc_gpio_handle *uc_gpio)
{
	uc_gpio->value_mask = AL_UC_GPIO_PARAM_VAL_MASK;
	uc_gpio->value_shift = AL_UC_GPIO_PARAM_VAL_SHIFT;

	uc_gpio->port_num = AL_UC_GPIO_NUM_PORTS_V3;
	uc_gpio->port_mask = AL_UC_GPIO_PARAM_PORT_MASK_V3;
	uc_gpio->port_shift = AL_UC_GPIO_PARAM_PORT_SHIFT;

	uc_gpio->bit_num = AL_UC_GPIO_NUM_BITS_V3;
	uc_gpio->bit_mask = AL_UC_GPIO_PARAM_BIT_MASK_V3;
	uc_gpio->bit_shift = AL_UC_GPIO_PARAM_BIT_SHIFT_V3;
}

void al_uc_gpio_handle_init(struct al_uc_handle *handle, struct al_uc_gpio_handle *uc_gpio)
{
	al_assert(handle->uc_cmd_gpio);
	al_assert(uc_gpio);

	/* Since for V3 we have more uC gpios, we need to set uC gpio handle differently */
	if (handle->uc_cmd_gpio == AL_UC_CMD_GPIO_V1_V2)
		uc_gpio_handle_init_v1_v2(uc_gpio);
	else if (handle->uc_cmd_gpio == AL_UC_CMD_GPIO_V3)
		uc_gpio_handle_init_v3(uc_gpio);
}

void al_uc_cmd_handle_init(
	struct al_uc_handle		*handle,
	struct al_uc_handle_init	*init)
{
	al_assert(handle);
	al_assert(init);
	al_assert(init->cmd_regs_base);
	al_assert(init->gpio_regs_base);
	al_assert(init->uc_cmd_gpio);

	handle->cmd_regs_base = init->cmd_regs_base;
	handle->gpio_regs_base = init->gpio_regs_base;
	handle->uc_cmd_gpio = init->uc_cmd_gpio;

	al_uc_gpio_handle_init(handle, &uc_gpio);
}

static int al_uc_cmd_adv(
	struct al_uc_handle	*handle,
	uint32_t		opcode,
	uint32_t		param,
	uint32_t		expected_resp,
	uint32_t		*resp_param,
	unsigned int		timeout_ms)
{
	struct uc_cmd_regs *cmd_regs = (struct uc_cmd_regs *)handle->cmd_regs_base;
	struct al_gpio_interface gpio_if;
	int pin_idx;
	int err = 0;

	cmd_regs->opcode = opcode;
	cmd_regs->param = param;
	cmd_regs->response = 0;
	cmd_regs->resp_param = 0;

	err = al_gpio_init(&gpio_if, handle->gpio_regs_base);
	if (err) {
		al_err("%s(%x): al_gpio_init failed!\n", __func__, opcode);
		return err;
	}

	pin_idx = AL_GPIO_ID_TO_PIN(handle->uc_cmd_gpio);

	al_gpio_dir_set(&gpio_if, pin_idx, AL_GPIO_DIR_OUT);

	al_data_memory_barrier();

	al_gpio_set(&gpio_if, pin_idx, AL_UC_CMD_GPIO_CMD_START_VAL); /* execute command */

	/* wait for app version response */
	while (timeout_ms && !cmd_regs->response) {
		al_udelay(1000);
		timeout_ms--;
	}

	al_gpio_set(&gpio_if, pin_idx, AL_UC_CMD_GPIO_CMD_END_VAL);

	if (!timeout_ms) {
		al_err("%s(%x): timeout!\n", __func__, opcode);
		return -ETIME;
	} else if (cmd_regs->response != expected_resp) {
		al_err("%s(%x): failed (%08x)\n", __func__, opcode, cmd_regs->response);
		return -EIO;
	}

	al_data_memory_barrier();

	*resp_param = cmd_regs->resp_param;

	return 0;
}

int al_uc_cmd(
	struct al_uc_handle	*handle,
	uint32_t		opcode,
	uint32_t		param,
	uint32_t		*resp_param)
{
	return al_uc_cmd_adv(handle, opcode, param, AL_UC_RESPONSE_DONE, resp_param,
		AL_UC_HOSTCMD_TIMEOUT_MSEC);
}

int al_uc_app_version_get(
	struct al_uc_handle	*handle,
	uint32_t		*ver)
{
	int err = al_uc_cmd(handle, AL_UC_OPCODE_APP_VERSION, 0, ver);

	return err;
}

int al_uc_bl_version_get(
	struct al_uc_handle	*handle,
	uint32_t		*ver)
{
	int err = al_uc_cmd(handle, AL_UC_OPCODE_BL_VERSION, 0, ver);

	return err;
}

static void update_crc16(const uint8_t *pu8Data, uint16_t *u16CRC)
{
	uint8_t i;

	i = 8;
	*u16CRC = *u16CRC ^ (((uint16_t) *pu8Data++) << 8);

	do {
		if (*u16CRC & 0x8000)
			*u16CRC = *u16CRC << 1 ^ 0x1021;
		else
			*u16CRC = *u16CRC << 1;
	} while (--i);
}

static void update_crc8(const uint8_t *pu8Data, uint8_t *u8CRC)
{
	*u8CRC = (*u8CRC + *pu8Data) % 256;
}

static int al_uc_update(
	struct al_uc_handle	*handle,
	unsigned int		image_size,
	uint32_t		image_phys_addr,
	uint32_t		opcode,
	unsigned int		timeout_ms)
{
	unsigned int i;
	uint8_t *p;
	uint16_t crc = 0;
	unsigned int image_net_size = image_size - IMAGE_CRC_SIZE;
	uint32_t resp_param;
	int err;
	al_phys_addr_t addr = (al_phys_addr_t)image_phys_addr;

	al_assert(addr == image_phys_addr);

	/* Make sure image is not all zeros */
	p = (uint8_t *)(uintptr_t)addr;
	for (i = 0; (!(*p)) && (i < image_net_size); i++, p++)
		;
	if (i == image_net_size) {
		al_err("%s: app image is all zeros!\n", __func__);
		return -EINVAL;
	}

	/* Validate image CRC */
	p = (uint8_t *)(uintptr_t)addr;
	for (i = 0; i < image_net_size; i++, p++)
		update_crc16(p, &crc);

	if (crc != *(uint16_t *)p) {
		al_err("%s: app crc in file %hx doesn't match calculated crc %hx, exiting!\n",
			__func__, *(uint16_t *)p, crc);
		return -EINVAL;
	}

	err = al_uc_cmd_adv(handle, opcode, image_phys_addr,
				AL_UC_RESPONSE_SW_UPDATE_OK, &resp_param, timeout_ms);
	return err;
}

int al_uc_app_update(
	struct al_uc_handle	*handle,
	uint32_t		image,
	unsigned int		image_size)
{
	int err;
	unsigned int timeout_ms;

	if (image_size == IMAGE_SIZE_12K)
		timeout_ms = AL_UC_UPDATE_12K_TIMEOUT_MSEC;
	else if (image_size == IMAGE_SIZE_43K || image_size == IMAGE_SIZE_44K ||
		 image_size == IMAGE_SIZE_43K + AL_UC_SIGNATURE_SIZE)
		timeout_ms = AL_UC_UPDATE_44K_TIMEOUT_MSEC;
	else if (image_size == IMAGE_SIZE_192K ||
		 image_size == IMAGE_SIZE_192K + AL_UC_SIGNATURE_SIZE)
		timeout_ms = AL_UC_UPDATE_192K_TIMEOUT_MSEC;
	else {
		al_err("image file size is supposed to be %lu or %lu or %lu\n",
			(unsigned long)IMAGE_SIZE_12K,
			(unsigned long)IMAGE_SIZE_192K,
			(unsigned long)IMAGE_SIZE_43K);
		return -1;
	}

	err = al_uc_update(handle, image_size, image,
				AL_UC_OPCODE_SW_UPDATE, timeout_ms);
	return err;
}

int al_uc_bootloader_update(
	struct al_uc_handle	*handle,
	uint32_t		image,
	unsigned int		image_size)
{
	int err;
	unsigned int timeout_ms;

	if (image_size == IMAGE_SIZE_19K)
		timeout_ms = AL_UC_UPDATE_20K_TIMEOUT_MSEC;
	else if (image_size == IMAGE_SIZE_60K)
		timeout_ms = AL_UC_UPDATE_60K_TIMEOUT_MSEC;
	else {
		al_err("image file size is supposed to be %lu or %lu\n",
			(unsigned long)IMAGE_SIZE_19K,
			(unsigned long)IMAGE_SIZE_60K);
		return -1;
	}

	err = al_uc_update(handle, image_size, image,
				AL_UC_OPCODE_BL_UPDATE, timeout_ms);
	return err;
}

int al_uc_auth_info_update(
	struct al_uc_handle	*handle,
	uint32_t		image,
	unsigned int		image_size)
{
	int err;
	unsigned int timeout_ms;

	if (image_size == AL_UC_AUTH_INFO_SIZE)
		timeout_ms = AL_UC_UPDATE_1K_TIMEOUT_MSEC;
	else {
		al_err("image file size is supposed to be %lu\n",
			(unsigned long)AL_UC_AUTH_INFO_SIZE);
		return -1;
	}

	err = al_uc_update(handle, image_size, image,
				AL_UC_OPCODE_SET_BL_AUTH_INFO, timeout_ms);
	return err;
}

int al_uc_soc_reset(
	struct al_uc_handle	*handle,
	uint32_t		delay_before_reset)
{
	uint32_t resp_param;
	int err;
	struct uc_cmd_regs *cmd_regs = (struct uc_cmd_regs *)handle->cmd_regs_base;

	if (delay_before_reset > MAX_DELAY_BEFORE_ALPINE_RESET) {
		al_err("Error: Maximum delay before alpine reset must be between 0 to %u sec\n",
			MAX_DELAY_BEFORE_ALPINE_RESET);
		return -EINVAL;
	}

	err = al_uc_cmd_adv(handle, AL_UC_OPCODE_RESET, delay_before_reset, AL_UC_RESPONSE_DONE,
				&resp_param, AL_UC_HOSTCMD_TIMEOUT_MSEC);

	/* actually reset is not done yet in this case, but uC signals it received the cmd.
	 * uC will reset after the response is set to zero and after the required delay
	 */
	if (!err)
		cmd_regs->response = 0;

	return err;
}

int al_uc_thermal_status_get(
	struct al_uc_handle	*handle,
	al_bool			*en,
	unsigned int		*i2c_addr)
{
	uint32_t resp_param;
	int err = al_uc_cmd(handle, AL_UC_OPCODE_THERMAL_GET, 0, &resp_param);

	*en = (resp_param & AL_UC_I2C_GET_EN_MASK) >> AL_UC_I2C_GET_EN_SHIFT;
	*i2c_addr = (resp_param & AL_UC_I2C_GET_ADDRESS_MASK) >> AL_UC_I2C_GET_ADDRESS_SHIFT;

	return err;
}

int al_uc_thermal_enable_set(
	struct al_uc_handle	*handle,
	al_bool			en,
	unsigned int		i2c_addr)
{
	uint32_t resp_param;
	int err;

	if (en)
		err = al_uc_cmd(handle, AL_UC_OPCODE_THERMAL_EN, i2c_addr, &resp_param);
	else
		err = al_uc_cmd(handle, AL_UC_OPCODE_THERMAL_DIS, 0, &resp_param);

	return err;
}

int al_uc_mgmt_status_get(
	struct al_uc_handle	*handle,
	al_bool			*en,
	unsigned int		*i2c_addr)
{
	uint32_t resp_param;
	int err = al_uc_cmd(handle, AL_UC_OPCODE_MGMT_GET, 0, &resp_param);

	*en = (resp_param & AL_UC_I2C_GET_EN_MASK) >> AL_UC_I2C_GET_EN_SHIFT;
	*i2c_addr = (resp_param & AL_UC_I2C_GET_ADDRESS_MASK) >> AL_UC_I2C_GET_ADDRESS_SHIFT;

	return err;
}

int al_uc_mgmt_enable_set(
	struct al_uc_handle	*handle,
	al_bool			en,
	unsigned int		i2c_addr)
{
	uint32_t resp_param;
	int err;

	if (en)
		err = al_uc_cmd(handle, AL_UC_OPCODE_MGMT_EN, i2c_addr, &resp_param);
	else
		err = al_uc_cmd(handle, AL_UC_OPCODE_MGMT_DIS, 0, &resp_param);

	return err;
}

int al_uc_mgmt_report_reset(
	struct al_uc_handle			*handle,
	enum al_uc_mgmt_report_reset_cause	cause)
{
	uint32_t resp_param;
	int err = al_uc_cmd(handle, AL_UC_OPCODE_MGMT_RESET_REPORT, cause, &resp_param);

	return err;
}

int al_uc_gpio_get(
	struct al_uc_handle	*handle,
	unsigned int		port,
	unsigned int		bit,
	unsigned int		*val)
{
	uint32_t param = (bit << uc_gpio.bit_shift) |
		(port << uc_gpio.port_shift);
	uint32_t resp_param;
	int err = al_uc_cmd(handle, AL_UC_OPCODE_GPIO_READ, param, &resp_param);

	*val = resp_param;

	return err;
}

int al_uc_gpio_set(
	struct al_uc_handle	*handle,
	unsigned int		port,
	unsigned int		bit,
	unsigned int		val)
{
	uint32_t param = (bit << uc_gpio.bit_shift) |
		(port << uc_gpio.port_shift) |
		(val << uc_gpio.value_shift);
	uint32_t resp_param;
	int err = al_uc_cmd(handle, AL_UC_OPCODE_GPIO_WRITE, param, &resp_param);

	return err;
}

int al_uc_gpio_dir_get(
	struct al_uc_handle	*handle,
	unsigned int		port,
	unsigned int		bit,
	al_bool			*is_output)
{
	uint32_t param = (bit << uc_gpio.bit_shift) |
		(port << uc_gpio.port_shift);
	uint32_t resp_param;
	int err = al_uc_cmd(handle, AL_UC_OPCODE_GPIO_GET_DIR, param, &resp_param);

	*is_output = resp_param;

	return err;
}

int al_uc_gpio_dir_set(
	struct al_uc_handle	*handle,
	unsigned int		port,
	unsigned int		bit,
	al_bool			is_output)
{
	uint32_t param = (bit << uc_gpio.bit_shift) |
		(port << uc_gpio.port_shift) |
		(is_output << uc_gpio.value_shift);
	uint32_t resp_param;
	int err = al_uc_cmd(handle, AL_UC_OPCODE_GPIO_SET_DIR, param, &resp_param);

	return err;
}

int al_uc_wd_status_get(
	struct al_uc_handle	*handle,
	al_bool			*en,
	unsigned int		*rst_cnt,
	unsigned int		*timeout)
{
	uint32_t resp_param;
	int err = al_uc_cmd(handle, AL_UC_OPCODE_WATCHDOG_GET, 0, &resp_param);
	unsigned int en_raw = ((resp_param & AL_UC_WATCHDOG_GET_DIS_MASK) >>
			       AL_UC_WATCHDOG_GET_DIS_SHIFT);

	if (en_raw == AL_UC_WATCHDOG_EN)
		*en = AL_TRUE;
	else if (en_raw == AL_UC_WATCHDOG_DIS)
		*en = AL_FALSE;
	else
		return -EIO;

	*rst_cnt = (resp_param & AL_UC_WATCHDOG_GET_RESET_COUNT_MASK) >>
		AL_UC_WATCHDOG_GET_RESET_COUNT_SHIFT;
	*timeout = (resp_param & AL_UC_WATCHDOG_GET_TIMEOUT_MASK) >>
		AL_UC_WATCHDOG_GET_TIMEOUT_SHIFT;

	return err;
}

int al_uc_wd_enable_set(
	struct al_uc_handle	*handle,
	al_bool			en,
	unsigned int		timeout)
{
	uint32_t resp_param;
	int err;

	if (en)
		err = al_uc_cmd(handle, AL_UC_OPCODE_WATCHDOG_EN, timeout, &resp_param);
	else
		err = al_uc_cmd(handle, AL_UC_OPCODE_WATCHDOG_DIS, 0, &resp_param);

	return err;
}

int al_uc_eeprom_wp_lock_set(
	struct al_uc_handle	*handle,
	al_bool			lock)
{
	uint32_t resp_param;
	int err;

	if (lock)
		err = al_uc_cmd(handle, AL_UC_OPCODE_EEPROM_WP_LOCK, 0, &resp_param);
	else
		err = al_uc_cmd(handle, AL_UC_OPCODE_EEPROM_WP_UNLOCK, 0, &resp_param);

	return err;
}

int al_uc_pin_save(
	struct al_uc_handle	*handle,
	unsigned int		port,
	unsigned int		bit,
	struct al_uc_pin_info	*pin_info)
{
	int err;
	int fail_count = 0;

	if ((pin_info == NULL) || (handle == NULL))
		return -EINVAL;

	pin_info->port = port;
	pin_info->bit = bit;

	err = al_uc_gpio_get(handle, pin_info->port, pin_info->bit, &pin_info->val);
	if (err)
		fail_count++;
	err = al_uc_gpio_dir_get(handle, pin_info->port, pin_info->bit,
				&pin_info->is_output);
	if (err)
		fail_count++;

	return fail_count;
}

int al_uc_pin_restore(
	struct al_uc_handle	*handle,
	struct al_uc_pin_info	*pin_info)
{
	int err;
	int fail_count = 0;

	if ((pin_info == NULL) || (handle == NULL))
		return -EINVAL;
	err = al_uc_gpio_dir_set(handle, pin_info->port, pin_info->bit,	pin_info->is_output);
	if (err)
		fail_count++;
	if (pin_info->is_output) {
		err = al_uc_gpio_set(handle, pin_info->port, pin_info->bit,
					pin_info->val);
		if (err)
			fail_count++;
	}

	return fail_count;
}

int al_uc_set_secure(
	struct al_uc_handle	*handle,
	al_bool			is_enabled)
{
	uint32_t resp_param;
	uint32_t param = is_enabled;
	int err;

	err = al_uc_cmd(handle, AL_UC_OPCODE_SET_SECURE, param, &resp_param);

	return err;
}

int al_uc_set_pp(
	struct al_uc_handle	*handle,
	al_bool			is_enabled)
{
	uint32_t resp_param;
	uint32_t param = is_enabled;
	int err;

	err = al_uc_cmd(handle, AL_UC_OPCODE_SET_PP, param, &resp_param);

	return err;
}

int al_uc_sdr_reset(
	struct al_uc_handle *handle)
{
	uint32_t resp_param;
	int err;

	err = al_uc_cmd(handle, AL_UC_OPCODE_SDR_RESET, 0, &resp_param);

	return err;
}

int al_uc_sdr_print(
	struct al_uc_handle	*handle,
	uint16_t		*sdr_data,
	unsigned int		*num_bytes,
	int			buffer_size)
{
	int i;
	uint32_t resp_param;
	int err = 0;
	*num_bytes = 0;

	for (i = 0; i < buffer_size; i++) {
		err = al_uc_cmd(handle, AL_UC_OPCODE_SDR_PRINT, i, &resp_param);
		if (err) {
			al_err("Failed reading SDR byte at offset 0x%08x\n", i);
			*num_bytes = 0;
			break;
		}

		sdr_data[i] = (uint16_t)resp_param;
		*num_bytes += sizeof(uint16_t);
	}
	return err;
}

int al_uc_get_secure(
	struct al_uc_handle	*handle,
	uint32_t		*resp_param)
{
	uint32_t param = 0;
	int err;

	err = al_uc_cmd(handle, AL_UC_OPCODE_GET_SECURE, param, resp_param);

	return err;
}

int al_uc_get_pp(
	struct al_uc_handle	*handle,
	uint32_t		*resp_param)
{
	uint32_t param = 0;
	int err;

	err = al_uc_cmd(handle, AL_UC_OPCODE_GET_PP, param, resp_param);

	return err;
}

int al_uc_get_secure_log(
	struct al_uc_handle	*handle,
	uint32_t		*resp_param)
{
	uint32_t param = 0;
	int err;

	err = al_uc_cmd(handle, AL_UC_OPCODE_GET_SECURE_LOG, param, resp_param);

	return err;
}

int al_uc_clear_secure_log(struct al_uc_handle	*handle)
{
	uint32_t resp_param;
	uint32_t param = 0;
	int err;

	err = al_uc_cmd(handle, AL_UC_OPCODE_CLEAR_SECURE_LOG, param, &resp_param);

	return err;
}

int al_uc_thermal_shutdown_status(
	struct al_uc_handle	*handle,
	uint32_t		*thermal_shutdown_status)
{
	uint32_t resp_param = 0;
	uint32_t param = 0;
	int err;

	err = al_uc_cmd(handle, AL_UC_OPCODE_THERMAL_SHUTDOWN_STATUS, param, &resp_param);
	*thermal_shutdown_status = resp_param;

	return err;
}

int al_uc_thermal_shutdown(
	struct al_uc_handle	*handle,
	al_bool			is_enabled)
{
	uint32_t resp_param;
	int err;

	if (is_enabled)
		err = al_uc_cmd(handle, AL_UC_OPCODE_THERMAL_SHUTDOWN_EN, 0, &resp_param);
	else
		err = al_uc_cmd(handle, AL_UC_OPCODE_THERMAL_SHUTDOWN_DIS, 0, &resp_param);

	return err;
}

int al_uc_set_spi_buf(
	struct al_uc_handle	*handle,
	al_bool			is_enabled)
{
	uint32_t resp_param;
	int err;

	err = al_uc_cmd(handle, HOSTCMD_OPCODE_SET_SPI_BUFFER, is_enabled, &resp_param);

	return err;
}

int al_uc_thermal_shutdown_set_tmp(
	struct al_uc_handle	*handle,
	unsigned int            thermal_shutdown_max_temp)
{
	uint32_t resp_param;
	uint32_t param = thermal_shutdown_max_temp;
	int err;

	err = al_uc_cmd(handle, AL_UC_OPCODE_THERMAL_SHUTDOWN_SET_TMP, param, &resp_param);

	return err;
}

int al_uc_thermal_shutdown_get_tmp(
	struct al_uc_handle	*handle,
	unsigned int		*thermal_shutdown_max_temp)
{
	uint32_t resp_param;
	int err;

	err = al_uc_cmd(handle, AL_UC_OPCODE_THERMAL_SHUTDOWN_GET_TMP, 0, &resp_param);
	*thermal_shutdown_max_temp = resp_param;

	return err;
}

int al_uc_nitro_mode_get(
	struct al_uc_handle	*handle,
	uint32_t		*nitro_mode_status)
{
	int err;

	err = al_uc_cmd(handle, HOSTCMD_OPCODE_NITRO_MODE_GET, 0, nitro_mode_status);

	return err;
}

int al_uc_nitro_mode_set(
	struct al_uc_handle	*handle,
	uint32_t		new_nitro_mode_status,
	uint32_t		nitro_perst_asserted_threshold,
	uint32_t		*nitro_mode_status)
{
	int err;

	err = -EINVAL;

	if (new_nitro_mode_status == AL_UC_NITRO_MODE_EN)
		err = al_uc_cmd(handle, HOSTCMD_OPCODE_NITRO_MODE_EN, 0, nitro_mode_status);
	else if (new_nitro_mode_status == AL_UC_NITRO_MODE_DIS)
		err = al_uc_cmd(handle, HOSTCMD_OPCODE_NITRO_MODE_DIS, 0, nitro_mode_status);
	else if ((new_nitro_mode_status == AL_UC_NITRO_MODE_MVP) &&
		 (nitro_perst_asserted_threshold > 0))
		err = al_uc_cmd(handle, HOSTCMD_OPCODE_NITRO_MODE_MVP_EN,
				nitro_perst_asserted_threshold, nitro_mode_status);
	else if ((new_nitro_mode_status == AL_UC_NITRO_MODE_IPR) &&
		 (nitro_perst_asserted_threshold > 0))
		err = al_uc_cmd(handle, HOSTCMD_OPCODE_NITRO_MODE_IPR_EN,
				nitro_perst_asserted_threshold, nitro_mode_status);

	return err;
}

int al_uc_nitro_perst_asserted_threshold_get(
	struct al_uc_handle	*handle,
	uint32_t		*perst_asserted_threshold)
{
	int err;

	err = al_uc_cmd(handle,
			HOSTCMD_OPCODE_NITRO_PERST_THRESH_GET,
			0,
			perst_asserted_threshold);

	return err;
}

int al_uc_fru_erase(struct al_uc_handle	*handle)
{
	int err;
	uint32_t resp_param;

	err = al_uc_cmd(handle, AL_UC_OPCODE_FRU_ERASE, 0, &resp_param);

	return err;
}

int al_uc_fru_print(
	struct al_uc_handle	*handle,
	uint32_t		*fru_data,
	unsigned int		*num_bytes,
	int			buffer_size)
{
	int i;
	uint32_t resp_param;
	int err = 0;
	*num_bytes = 0;

	for (i = 0; i < buffer_size; i++) {
		err = al_uc_cmd(handle, AL_UC_OPCODE_FRU_PRINT, i, &resp_param);
		if (err) {
			if (err != -EIO)
				al_err("Failed reading FRU byte at offset 0x%08x\n", i);
			break;
		}

		fru_data[i] = resp_param;
		*num_bytes += sizeof(uint32_t);
	}

	return (err != -EIO) ? err : 0;
}

int al_uc_fru_burn(
	struct al_uc_handle	*handle,
	uint8_t			*image,
	unsigned int		image_size,
	al_phys_addr_t		image_phys_addr)
{
	unsigned int i;
	uint8_t *p;
	uint8_t crc = 0;
	unsigned int image_net_size = image_size - 1;
	uint32_t resp_param;
	int err;

	al_assert(((al_phys_addr_t)(uint32_t)image_phys_addr) == image_phys_addr);

	p = image;
	*(uint32_t *)p = image_size;
	p += sizeof(uint32_t);

	/* Validate image CRC */
	for (i = 0; i < image_net_size; i++, p++)
		update_crc8(p, &crc);
	crc = 256 - crc;

	if (crc != *(uint8_t *)p) {
		al_err("FRU crc in file %hx doesn't match calculated crc %hx, exiting!\n",
				*(uint8_t *)p, crc);
		return -EINVAL;
	}

	err = al_uc_cmd_adv(handle, AL_UC_OPCODE_FRU_BURN, image_phys_addr,
				AL_UC_RESPONSE_DONE, &resp_param, AL_UC_HOSTCMD_TIMEOUT_MSEC);
	return err;
}

int al_uc_set_config(
		struct al_uc_handle	*handle,
		uint8_t			address,
		uint8_t			data)
{
	int err;
	uint32_t resp_param;
	uint32_t param = address;

	param += ((uint16_t)data << 8);

	err = al_uc_cmd(handle, AL_UC_OPCODE_SET_CONFIG, param, &resp_param);

	return err;
}

int al_uc_sdr_status_get(
	struct al_uc_handle	*handle,
	al_bool			*en,
	unsigned int		*i2c_address)
{
	uint32_t resp_param;
	int err = al_uc_cmd(handle, AL_UC_OPCODE_SDR_GET, 0, &resp_param);

	*en = (resp_param & AL_UC_I2C_GET_EN_MASK) >> AL_UC_I2C_GET_EN_SHIFT;
	*i2c_address = (resp_param & AL_UC_I2C_GET_ADDRESS_MASK) >> AL_UC_I2C_GET_ADDRESS_SHIFT;

	return err;
}

int al_uc_sdr_status_set(
	struct al_uc_handle	*handle,
	al_bool			en,
	unsigned int		i2c_address)
{
	uint32_t resp_param;
	int err;

	if (en)
		err = al_uc_cmd(handle, AL_UC_OPCODE_SDR_EN, i2c_address, &resp_param);
	else
		err = al_uc_cmd(handle, AL_UC_OPCODE_SDR_DIS, 0, &resp_param);

	return err;
}

int al_uc_sdr_fw_update_set(
	struct al_uc_handle	*handle,
	uint8_t			data)
{
	int err;
	uint32_t resp_param;

	err = al_uc_cmd(handle, AL_UC_OPCODE_SDR_SET_FW_UPDATE, data, &resp_param);

	return err;
}

int al_uc_sdr_version_get(
	struct al_uc_handle	*handle,
	uint32_t		*ver)
{
	int err;

	err = al_uc_cmd(handle, AL_UC_OPCODE_SDR_GET_VERSION, 0, ver);

	return err;
}

int al_uc_fru_status_get(
	struct al_uc_handle	*handle,
	al_bool			*en,
	unsigned int		*i2c_address)
{
	uint32_t resp_param;
	int err = al_uc_cmd(handle, AL_UC_OPCODE_FRU_STATUS, 0, &resp_param);

	*en = (resp_param & AL_UC_I2C_GET_EN_MASK) >> AL_UC_I2C_GET_EN_SHIFT;
	*i2c_address = (resp_param & AL_UC_I2C_GET_ADDRESS_MASK) >> AL_UC_I2C_GET_ADDRESS_SHIFT;

	return err;

}

int al_uc_fru_status_set(
	struct al_uc_handle	*handle,
	al_bool			en,
	unsigned int		i2c_address)
{
	uint32_t resp_param;
	int err;

	if (en)
		err = al_uc_cmd(handle, AL_UC_OPCODE_FRU_EN, i2c_address, &resp_param);
	else
		err = al_uc_cmd(handle, AL_UC_OPCODE_FRU_DIS, 0, &resp_param);

	return err;

}

int al_uc_cfg_mem_err_get(
	struct al_uc_handle	*handle,
	uint32_t		*mem_err_val)
{
	int err;

	err = al_uc_cmd(handle, AL_UC_OPCODE_CFG_MEM_ERR_GET, 0, mem_err_val);

	return err;
}

int al_uc_cfg_mem_err_set(
	struct al_uc_handle	*handle,
	uint8_t			data)
{
	int err;
	uint32_t resp_param;

	err = al_uc_cmd(handle, AL_UC_OPCODE_CFG_MEM_ERR_SET, data, &resp_param);

	return err;
}

int al_uc_cfg_mem_err_clear(
	struct al_uc_handle	*handle)
{
	int err;
	uint32_t resp_param;

	err = al_uc_cmd(handle, AL_UC_OPCODE_CFG_MEM_ERR_CLEAR, 0, &resp_param);

	return err;
}

int al_uc_i2c_master_probe_slave(
	struct al_uc_handle	*handle,
	uint8_t			slave_addr)
{
	int err;
	uint32_t resp_param;

	err = al_uc_cmd(handle, AL_UC_OPCODE_I2C_MASTER_PROBE_SLAVE, slave_addr, &resp_param);

	return err;
}

int al_uc_fru_sram_ready(
	struct al_uc_handle     *handle)
{
	int err;
	uint32_t resp_param;

	err = al_uc_cmd(handle, AL_UC_OPCODE_FRU_SRAM_READY, 0, &resp_param);
	return err;
}

int al_uc_uart_test(
	struct al_uc_handle	*handle,
	al_bool			en)
{
	int err;
	uint32_t resp_param;

	err = al_uc_cmd(handle, AL_UC_OPCODE_UC_UART_TEST, en, &resp_param);

	return err;
}

int al_uc_i2c_clear_fifo(
	struct al_uc_handle	*handle)
{
	uint32_t resp_param;

	int err = al_uc_cmd(handle, AL_UC_HOSTCMD_ALPINE_I2C_CLEAN_FIFO, 0, &resp_param);

	return err;
}

int al_uc_spi_flash_set(
	struct al_uc_handle	*handle,
	uint32_t		val)
{

	uint32_t resp_param;

	int err = al_uc_cmd(handle, AL_UC_OPCODE_SPI_FLASH_SET, val, &resp_param);

	return err;
}

int al_uc_spi_flash_get(
	struct al_uc_handle	*handle,
	uint32_t		*val)
{
	uint32_t resp_param;

	int err = al_uc_cmd(handle, AL_UC_OPCODE_SPI_FLASH_GET, 0, &resp_param);

	*val = resp_param;

	return err;
}

int al_uc_spi_flash_booted_from(
	struct al_uc_handle	*handle,
	uint32_t		*val)
{
	uint32_t resp_param;

	int err = al_uc_cmd(handle, AL_UC_OPCODE_SPI_BOOTED_FROM, 0, &resp_param);

	*val = resp_param;

	return err;

}

int al_uc_cpu_voltage_set(
	struct al_uc_handle	*handle,
	uint32_t		val)
{
	uint32_t resp_param;

	int err = al_uc_cmd(handle, AL_UC_OPCODE_CPU_VOLTAGE_SET, val, &resp_param);

	return err;
}

int al_uc_cpu_voltage_get(
	struct al_uc_handle	*handle,
	uint32_t		*val)
{
	uint32_t resp_param;

	int err = al_uc_cmd(handle, AL_UC_OPCODE_CPU_VOLTAGE_GET, 0, &resp_param);

	*val = resp_param;

	return err;
}

int al_uc_dc_controller_max_vout_set(
	struct al_uc_handle	*handle)
{
	uint32_t resp_param;

	int err = al_uc_cmd(handle, AL_UC_OPCODE_DC_CONTROLLER_MAX_VOUT_SET, 0, &resp_param);

	return err;
}

int al_uc_reset_request(
	struct al_uc_handle	*handle,
	uint32_t		req_reset)
{
	uint32_t resp_param;

	int err = al_uc_cmd(handle, AL_UC_OPCODE_RESET_REQUEST, req_reset, &resp_param);

	return err;

}

int al_uc_liveness_timer_get(
	struct al_uc_handle	*handle,
	uint32_t		*timer)
{
	int err;

	err = al_uc_cmd(handle, AL_UC_OPCODE_GET_LIVENESS_TIMER, 0, timer);

	return err;
}

int al_uc_fru_unlock(
	struct al_uc_handle	*handle)
{
	int err;
	uint32_t resp_param;

	err = al_uc_cmd(handle, AL_UC_OPCODE_FRU_UNLOCK, 0, &resp_param);

	return err;
}

int al_uc_fru_shared_valid_get(
	struct al_uc_handle	*handle,
	al_bool			*is_valid)
{
	int err;
	uint32_t resp_param;

	err = al_uc_cmd(handle, AL_UC_OPCODE_FRU_VALID_GET, 0, &resp_param);

	*is_valid = resp_param ? AL_TRUE : AL_FALSE;

	return err;
}

int al_uc_spi_preloader_read(
	struct al_uc_handle *handle,
	uint8_t *buf,
	unsigned int size)
{
	int err = 0;
	union {
		uint32_t resp_param_uint32;
		uint8_t resp_param_uint8[sizeof(uint32_t)];
	} resp_param;
	int i;
	unsigned int resp_param_idx = sizeof(uint32_t);

	if ((!buf) || (size > AL_UC_SPI_PRELOADER_MAX_SIZE))
		return -EINVAL;

	for (i = 0; i < size; i++) {
		if (resp_param_idx == sizeof(uint32_t)) {
			err = al_uc_cmd(handle, AL_UC_OPCODE_SPI_PREALODER_READ, i,
						&resp_param.resp_param_uint32);
			if (err)
				return err;
			resp_param_idx = 0;
		}

		buf[i] = resp_param.resp_param_uint8[resp_param_idx];
		resp_param_idx++;
	}

	return err;
}

int al_uc_spi_preloader_burn(
	struct al_uc_handle *handle,
	uint8_t *buf,
	unsigned int size,
	al_phys_addr_t load_address)
{
	int err = 0;
	uint32_t resp_param;
	struct preload_data_t {
		uint32_t size;
		uint8_t buf[AL_UC_SPI_PRELOADER_MAX_SIZE];
	} volatile * preload;
	int i;

	al_assert(((al_phys_addr_t)(uint32_t)load_address) == load_address);

	if ((!buf) || (size > AL_UC_SPI_PRELOADER_MAX_SIZE))
		return -EINVAL;

	preload = (struct preload_data_t *)(uintptr_t)(load_address);
	preload->size = size;
	for (i = 0; i < size; i++)
		preload->buf[i] = buf[i];

	err = al_uc_cmd(handle, AL_UC_OPCODE_SPI_PREALODER_BURN, load_address, &resp_param);

	return err;
}

int al_uc_uart_phys_lb_test(struct al_uc_handle *handle)
{
	int err;
	uint32_t resp_param;

	err = al_uc_cmd(handle, AL_UC_OPCODE_UC_UART_PHYS_LB, 0, &resp_param);

	return err;
}

int al_uc_sdr_reg_mask_set(
	struct al_uc_handle *handle,
	uint32_t reg_num)
{
	int err;
	uint32_t resp_param;

	err = al_uc_cmd(handle, AL_UC_OPCODE_SDR_SET_REG_MASK, reg_num, &resp_param);

	return err;
}

int al_uc_sdr_reg_mask_clear(
	struct al_uc_handle *handle,
	uint32_t reg_num)
{
	int err;
	uint32_t resp_param;

	err = al_uc_cmd(handle, AL_UC_OPCODE_SDR_CLEAR_REG_MASK, reg_num, &resp_param);

	return err;
}

int al_uc_alpaca_config_test(
	struct al_uc_handle *handle,
	uint32_t value)
{
	int err;
	uint32_t resp_param;

	err = al_uc_cmd(handle, AL_UC_OPCODE_ALPACA_CONFIG_TEST, value, &resp_param);

	return err;
}

int al_uc_app_idx_get(
	struct al_uc_handle *handle,
	uint32_t *app_idx)
{
	int err;
	uint32_t resp_param;

	err = al_uc_cmd(handle, AL_UC_OPCODE_APP_IDX_GET, 0, &resp_param);
	*app_idx = resp_param;

	return err;
}

int al_uc_app_idx_set(
	struct al_uc_handle *handle,
	uint32_t app_idx)
{
	int err;
	uint32_t resp_param;

	err = al_uc_cmd(handle, AL_UC_OPCODE_APP_IDX_SET, app_idx, &resp_param);

	return err;
}

int al_uc_stg2_done_exit_secure(
	struct al_uc_handle *handle,
	int val)
{
	int err;
	uint32_t resp_param;

	err = al_uc_cmd(handle, AL_UC_OPCODE_KR_STG2_DONE_EXIT_SECURE, (uint32_t)val, &resp_param);

	return err;
}

int al_uc_exit_secure(struct al_uc_handle *handle)
{
	int err;
	uint32_t resp_param;

	err = al_uc_cmd(handle, AL_UC_OPCODE_KR_EXIT_SECURE, (uint32_t)0, &resp_param);

	return err;
}

#endif /* AL_HAL_HAS_UC_CMDS */
