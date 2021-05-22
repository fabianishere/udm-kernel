/*
 * Copyright 2018, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#ifndef __AL_UC_CMDS_H__
#define __AL_UC_CMDS_H__

#include "al_hal_common.h"
#include "al_hal_plat_types.h"
#include "al_hal_gpio.h"

#ifdef AL_HAL_HAS_UC_CMDS

#define AL_UC_CMD_GPIO_V1_V2			37
#define AL_UC_CMD_GPIO_V3			55

#define AL_UC_CMD_GPIO_CMD_START_VAL		0
#define AL_UC_CMD_GPIO_CMD_END_VAL		1

#define AL_UC_CMD_REGS_OFFSET_V1		0x1EB0
#define AL_UC_CMD_REGS_OFFSET_V2		0x170
#define AL_UC_CMD_REGS_OFFSET_V3		0x170

#define UC_SPI_TOGGLE_GPIO_PORT_V1_V2		2
#define UC_SPI_TOGGLE_GPIO_BIT_V2		2

#define UC_SPI_TOGGLE_GPIO_PORT_V3		2
#define UC_SPI_TOGGLE_GPIO_BIT_V3		3

/* Host commands */
#define AL_UC_OPCODE_BL_UPDATE			0xBABABBBBUL
#define AL_UC_OPCODE_BL_UPDATE_WAIT		0xBABABBBCUL
#define AL_UC_OPCODE_SW_UPDATE			0xBABACCCCUL
#define AL_UC_RESPONSE_SW_UPDATE_OK		0xCACA00DDUL
#define AL_UC_OPCODE_APP_VERSION		0xBABADDDDUL
#define AL_UC_OPCODE_SET_BL_AUTH_INFO		0xBABAEEEEUL
#define AL_UC_OPCODE_BL_VERSION			0xBABADEDEUL
#define AL_UC_OPCODE_RESET			0xBABA0101UL
#define AL_UC_OPCODE_THERMAL_EN			0xBABA0202UL
#define AL_UC_OPCODE_THERMAL_DIS		0xBABA0303UL
#define AL_UC_OPCODE_THERMAL_GET		0xBABA0404UL
#define AL_UC_OPCODE_MGMT_EN			0xBABA0505UL
#define AL_UC_OPCODE_MGMT_DIS			0xBABA0606UL
#define AL_UC_OPCODE_MGMT_GET			0xBABA0707UL
#define AL_UC_OPCODE_MGMT_RESET_REPORT		0xBABA0808UL
#define AL_UC_OPCODE_GPIO_READ			0xBABA0909UL
#define AL_UC_OPCODE_GPIO_WRITE			0xBABA0A0AUL
#define AL_UC_OPCODE_GPIO_GET_DIR		0xBABA0B0BUL
#define AL_UC_OPCODE_GPIO_SET_DIR		0xBABA0C0CUL
#define AL_UC_OPCODE_WATCHDOG_DIS		0xBABA0D0DUL
#define AL_UC_OPCODE_WATCHDOG_EN		0xBABA0E0EUL
#define AL_UC_OPCODE_WATCHDOG_GET		0xBABA0F0FUL
#define AL_UC_OPCODE_EEPROM_WP_LOCK		0xBABA1010UL
#define AL_UC_OPCODE_EEPROM_WP_UNLOCK		0xBABA1111UL

#define AL_UC_OPCODE_FRU_EN			0xBABA1313UL
#define AL_UC_OPCODE_FRU_DIS			0xBABA1414UL
#define AL_UC_OPCODE_FRU_BURN			0xBABA1515UL
#define AL_UC_OPCODE_FRU_PRINT			0xBABA1616UL
#define AL_UC_OPCODE_FRU_STATUS			0xBABA1717UL
#define AL_UC_OPCODE_FRU_ERASE			0xBABA3030UL
#define AL_UC_OPCODE_FRU_SRAM_READY		0xBABA3535UL
#define AL_UC_OPCODE_FRU_UNLOCK			0xBABA3737UL
#define AL_UC_OPCODE_FRU_VALID_GET		0xBABA3838UL
#define AL_UC_FRU_MAX_SIZE			1024

#define AL_UC_OPCODE_SDR_EN			0xBABA1818UL
#define AL_UC_OPCODE_SDR_DIS			0xBABA1919UL
#define AL_UC_OPCODE_SDR_GET			0xBABA1A1AUL
#define AL_UC_OPCODE_SDR_PRINT			0xBABA1B1BUL
#define AL_UC_OPCODE_SDR_SET_FW_UPDATE		0xBABA1C1CUL
#define AL_UC_OPCODE_SDR_GET_VERSION		0xBABA1D1DUL
#define AL_UC_OPCODE_SDR_SET_REG_MASK		0xBABA2D2DUL
#define AL_UC_OPCODE_SDR_CLEAR_REG_MASK		0xBABA3D3DUL
#define AL_UC_SDR_MAX_OFFSET_V1_V2		31
#define AL_UC_SDR_MAX_OFFSET_V3			40

#define AL_UC_OPCODE_SET_SECURE			0xBABA1E1EUL
#define AL_UC_OPCODE_SET_PP			0xBABA1F1FUL
#define AL_UC_OPCODE_GET_SECURE			0xBABA2121UL
#define AL_UC_OPCODE_GET_PP			0xBABA2222UL
#define AL_UC_OPCODE_GET_SECURE_LOG		0xBABA2323UL
#define AL_UC_OPCODE_CLEAR_SECURE_LOG		0xBABA2424UL

#define AL_UC_OPCODE_SDR_RESET			0xBABA2020UL

#define AL_UC_OPCODE_THERMAL_SHUTDOWN_STATUS	0xBABA2729UL
#define AL_UC_OPCODE_THERMAL_SHUTDOWN_EN	0xBABA2525UL
#define AL_UC_OPCODE_THERMAL_SHUTDOWN_DIS	0xBABA2626UL
#define AL_UC_OPCODE_THERMAL_SHUTDOWN_SET_TMP	0xBABA2727UL
#define AL_UC_OPCODE_THERMAL_SHUTDOWN_GET_TMP	0xBABA2728UL

#define HOSTCMD_OPCODE_NITRO_MODE_DIS		0xBABA2828UL
#define HOSTCMD_OPCODE_NITRO_MODE_EN		0xBABA2929UL
#define HOSTCMD_OPCODE_NITRO_MODE_MVP_EN	0xBABA2930UL
#define HOSTCMD_OPCODE_NITRO_PERST_THRESH_GET	0xBABA2931UL
#define HOSTCMD_OPCODE_NITRO_MODE_GET		0xBABA2A2AUL
#define HOSTCMD_OPCODE_NITRO_MODE_IPR_EN	0xBABA2B2BUL

#define HOSTCMD_OPCODE_SET_SPI_BUFFER		0xCACA3737UL

#define AL_UC_OPCODE_SET_CONFIG			0xBABA3131UL

#define AL_UC_OPCODE_CFG_MEM_ERR_GET		0xBABA3232UL
#define AL_UC_OPCODE_CFG_MEM_ERR_SET		0xBABA3333UL
#define AL_UC_OPCODE_CFG_MEM_ERR_CLEAR		0xBABA3434UL

#define AL_UC_OPCODE_I2C_MASTER_PROBE_SLAVE	0xBABA3B3BUL

#define AL_UC_OPCODE_CPU_VOLTAGE_SET		0xBABA4B4BUL
#define AL_UC_OPCODE_CPU_VOLTAGE_GET		0xBABA5B5BUL
#define AL_UC_OPCODE_DC_CONTROLLER_MAX_VOUT_SET	0xBABAEAEAUL
#define MAX_VOUT_VOLTAGE_MV			1120

#define AL_UC_OPCODE_SPI_FLASH_SET		0xBABA6B6BUL
#define AL_UC_OPCODE_SPI_FLASH_GET		0xBABA7B7BUL
#define AL_UC_OPCODE_SPI_BOOTED_FROM		0xBABA8B8BUL

#define AL_UC_OPCODE_GET_LIVENESS_TIMER		0xBABAABABUL

#define AL_UC_OPCODE_SPI_PREALODER_READ		0xBABA9999UL
#define AL_UC_OPCODE_SPI_PREALODER_BURN		0xBABAAAAAUL
#define AL_UC_HOSTCMD_ALPINE_I2C_CLEAN_FIFO	0xBABAA9A9UL

#define AL_UC_OPCODE_UC_UART_TEST		0xBABA3939UL
#define AL_UC_OPCODE_UC_UART_PHYS_LB		0xBABA3C3CUL

#define AL_UC_OPCODE_ALPACA_CONFIG_TEST		0xBABA4C4CUL

#define AL_UC_OPCODE_APP_IDX_GET		0xBABA3E3EUL
#define AL_UC_OPCODE_APP_IDX_SET		0xBABA3F3FUL

#define AL_UC_OPCODE_ENTROPY_GET		0xBABA5C5CUL

#define AL_UC_OPCODE_RESET_REQUEST		0xBABA9B9BUL

#define AL_UC_OPCODE_KR_GET_KRB_VERSION		0xBABA4040UL
#define AL_UC_OPCODE_KR_UPDATE_KRB		0xBABA4141UL
#define AL_UC_OPCODE_KR_GEN_SBK			0xBABA4242UL
#define AL_UC_OPCODE_KR_GET_SBK_STATE		0xBABA4343UL
#define AL_UC_OPCODE_KR_ENROLL_SBK		0xBABA4444UL
#define AL_UC_OPCODE_KR_STG2_SIGN		0xBABA4545UL
#define AL_UC_OPCODE_KR_STG2_DONE_EXIT_SECURE	0xBABA4646UL
#define AL_UC_OPCODE_KR_EXIT_SECURE		0xBABA4747UL

#define AL_UC_RESPONSE_DONE			0xCACA00AAUL
#define AL_UC_RESPONSE_FAIL			0xCACAFAFAUL

/* UC version */
#define AL_UC_VERSION_SUBMINOR_MASK		0x00FF0000
#define AL_UC_VERSION_SUBMINOR_SHIFT		16
#define AL_UC_VERSION_MAJOR_MASK		0x0000FF00
#define AL_UC_VERSION_MAJOR_SHIFT		8
#define AL_UC_VERSION_MINOR_MASK		0x000000FF
#define AL_UC_VERSION_MINOR_SHIFT		0

#define AL_UC_VERSION_MAJOR(v)			\
	((v & AL_UC_VERSION_MAJOR_MASK) >> AL_UC_VERSION_MAJOR_SHIFT)

#define AL_UC_VERSION_MINOR(v)			\
	((v & AL_UC_VERSION_MINOR_MASK) >> AL_UC_VERSION_MINOR_SHIFT)

#define AL_UC_VERSION_SUBMINOR(v)		\
	((v & AL_UC_VERSION_SUBMINOR_MASK) >> AL_UC_VERSION_SUBMINOR_SHIFT)

/* UC image size */
#define IMAGE_SIZE_12K				(12 * 1024)
/* 20K - 1 last page of shared memory */
#define IMAGE_SIZE_19K				(19 * 1024 + 768)
#define IMAGE_SIZE_43K				(43 * 1024)
#define IMAGE_SIZE_44K				(44 * 1024)
#define IMAGE_SIZE_60K				(60 * 1024)
#define IMAGE_SIZE_192K				(192 * 1024)

#define AL_UC_NUM_OF_APP			2
#define AL_UC_APP_HEADER_MN			0xABCDDCBA

#define AL_UC_SIGNATURE_SIZE			256
#define AL_UC_AUTH_INFO_SIZE			512

/* UC timeout */
#define AL_UC_UPDATE_1K_TIMEOUT_MSEC		(2 * 1000)
#define AL_UC_UPDATE_12K_TIMEOUT_MSEC		(10 * 1000)
#define AL_UC_UPDATE_20K_TIMEOUT_MSEC		(35 * 1000)
#define AL_UC_UPDATE_44K_TIMEOUT_MSEC		(50 * 1000)
#define AL_UC_UPDATE_60K_TIMEOUT_MSEC		(70 * 1000)
#define AL_UC_UPDATE_192K_TIMEOUT_MSEC		(300 * 1000)
#define AL_UC_UPDATE_DUAL_IMAGE_TIMEOUT_MSEC	(15 * 1000)

#define AL_UC_HOSTCMD_TIMEOUT_MSEC		(2 * 1000)
#define AL_UC_HOSTCMD_TRNG_MSEC			(200 * 1000)

#define AL_UC_KR_KRB_UPDATE_HOSTCMD_TIMEOUT_MSEC	(10 * 1000)
#define AL_UC_KR_SBK_ENROLL_HOSTCMD_TIMEOUT_MSEC	(15 * 1000)
#define AL_UC_KR_STG2_SIGN_HOSTCMD_TIMEOUT_MSEC		(40 * 1000)

/* Thermal address */
#define AL_UC_THERMAL_I2C_VALID_ADDR_START	0x48
#define AL_UC_THERMAL_I2C_VALID_ADDR_END	0x4F

/* MGMT address */
#define AL_UC_MGMT_I2C_VALID_ADDR_START		0x40
#define AL_UC_MGMT_I2C_VALID_ADDR_END		0x47

/* FRU address */
#define AL_UC_FRU_I2C_VALID_ADDR_START		0x30
#define AL_UC_FRU_I2C_VALID_ADDR_END		0x35

/* SDR address */
#define AL_UC_SDR_I2C_VALID_ADDR_START		0x36
#define AL_UC_SDR_I2C_VALID_ADDR_END		0x3B

/* I2C interface */
#define AL_UC_I2C_EN				0x1
#define AL_UC_I2C_DIS				0xFFFF

#define AL_UC_I2C_GET_ADDRESS_MASK		0xFFFF0000
#define AL_UC_I2C_GET_ADDRESS_SHIFT		16
#define AL_UC_I2C_GET_EN_MASK			0x0000FFFF
#define AL_UC_I2C_GET_EN_SHIFT			0

/* UC Watchdog */
#define AL_UC_WATCHDOG_DIS			0x1
#define AL_UC_WATCHDOG_EN			0xFF

#define AL_UC_WATCHDOG_TIMEOUT_VALID_MIN	1
#define AL_UC_WATCHDOG_TIMEOUT_VALID_MAX	254

#define AL_UC_WATCHDOG_GET_DIS_MASK		0x000000FF
#define AL_UC_WATCHDOG_GET_DIS_SHIFT		0
#define AL_UC_WATCHDOG_GET_TIMEOUT_MASK		0x0000FF00
#define AL_UC_WATCHDOG_GET_TIMEOUT_SHIFT	8
#define AL_UC_WATCHDOG_GET_RESET_COUNT_MASK	0xFFFF0000
#define AL_UC_WATCHDOG_GET_RESET_COUNT_SHIFT	16

/* UC GPIO */
/* Value */
#define AL_UC_GPIO_PARAM_VAL_MASK		0x0000FFFF
#define AL_UC_GPIO_PARAM_VAL_SHIFT		0

/* GPIO Port */
#define AL_UC_GPIO_NUM_PORTS_V1_V2		4
#define AL_UC_GPIO_NUM_PORTS_V3			5

#define AL_UC_GPIO_PARAM_PORT_MASK_V1_V2	0x0FFF0000
#define AL_UC_GPIO_PARAM_PORT_MASK_V3		0x00FF0000
#define AL_UC_GPIO_PARAM_PORT_SHIFT		16

/* GPIO Bit */
#define AL_UC_GPIO_NUM_BITS_V1_V2		12
#define AL_UC_GPIO_NUM_BITS_V3			32

#define AL_UC_GPIO_PARAM_BIT_MASK_V1_V2		0xF0000000
#define AL_UC_GPIO_PARAM_BIT_SHIFT_V1_V2	28

#define AL_UC_GPIO_PARAM_BIT_MASK_V3		0xFF000000
#define AL_UC_GPIO_PARAM_BIT_SHIFT_V3		24

/* UC config space */
#define AL_UC_CONFIG_OFFSET_MIN			0
#define AL_UC_CONFIG_OFFSET_MAX			255
#define AL_UC_CONFIG_DATA_MIN			0
#define AL_UC_CONFIG_DATA_MAX			255

/* UC NITRO mode */
#define AL_UC_NITRO_MODE_EN			0x1
#define AL_UC_NITRO_MODE_MVP			0x2
#define AL_UC_NITRO_MODE_IPR			0x3
#define AL_UC_NITRO_MODE_DIS			0xFF

/* UC Thermal Shutdown */
/* Must be greater then boot default (50) */
#define AL_UC_THERMAL_SHUTDOWN_MIN_TEMP		51
#define AL_UC_THERMAL_SHUTDOWN_MAX_TEMP		115

/* uC preload data */
#define AL_UC_SPI_PRELOADER_MAX_SIZE		512


#define MAX_DELAY_BEFORE_ALPINE_RESET		60 /* Sec */

/* Key Rotation */
#define AL_UC_KRB_VRSION_MAJOR(version) ((version) >> 16)
#define AL_UC_KRB_VRSION_MINOR(version) ((version) & 0xFFFF)
#define AL_UC_KRB_SIZE			1064
#define AL_UC_STG2_SIGN_SIZE		256
#define AL_UC_STG2_HASH_SIZE		32
#define AL_UC_MODULUS_SIZE		256
#define AL_UC_KR_CMD_TIMEOUT		(3 * 1024)

/** uc_gpio */
struct al_uc_gpio_handle {
	/* UC GPIO value */
	int value_mask; /**< GPIO value mask */
	int value_shift; /**< GPIO value shift */

	/* UC GPIO port */
	int port_num; /**< GPIO port max num */
	int port_mask; /**< GPIO port mask */
	int port_shift; /**< GPIO port shift */

	/* UC GPIO bit */
	int bit_num; /**< GPIO bit max num */
	int bit_mask; /**< GPIO bit mask */
	int bit_shift; /**< GPIO bit shift */
};

/** uc_cmd_regs */
struct uc_cmd_regs {
	uint32_t opcode; /**< Host command opcode */
	uint32_t param;	/**< Host command param */
	uint32_t response; /**< uC response */
	uint32_t resp_param; /**< uC response param */
};

/** al_uc_handle */
struct al_uc_handle_init {
	void __iomem	*cmd_regs_base; /**< cmd regs base */
	void __iomem	*gpio_regs_base; /**< gpio regs base */
	int		uc_cmd_gpio; /**< uc command gpio */
};

/** al_uc_handle */
struct al_uc_handle {
	void __iomem	*cmd_regs_base; /**< cmd regs base */
	void __iomem	*gpio_regs_base; /**< gpio regs base */
	int		uc_cmd_gpio; /**< gpio num for uC communication */
};

/** al_uc_pin_info */
struct al_uc_pin_info {
	unsigned int port; /**< pin port */
	unsigned int bit; /**< pin bit */
	unsigned int val; /**< pin value */
	al_bool is_output; /**< is pin output */
};

/** mgmt reset causes */
enum al_uc_mgmt_report_reset_cause {
	AL_UC_MGMT_REPORT_RESET_CAUSE_MIN		= 0,
	AL_UC_MGMT_REPORT_RESET_CAUSE_NOT_SPECIFIED	= AL_UC_MGMT_REPORT_RESET_CAUSE_MIN,
	AL_UC_MGMT_REPORT_RESET_CAUSE_FW_UPGRADE	= 1,
	AL_UC_MGMT_REPORT_RESET_CAUSE_CARD_ERROR	= 2,
	AL_UC_MGMT_REPORT_RESET_CAUSE_TEST		= 3,
	AL_UC_MGMT_REPORT_RESET_CAUSE_MAX		= 3,
};

/** al_uc_app_header */
struct al_uc_app_header {
	uint32_t offset_app_0;
	uint32_t offset_app_1;
	uint32_t size_app_0;
	uint32_t size_app_1;
	uint32_t header_mn;
	uint32_t reserved[3];
};

/**
 * Inititalize uc gpio handle
 * @param handle - uC controller handle
 * @param uc_gpio - uc gpio handle
 */
void al_uc_gpio_handle_init(
	struct al_uc_handle		*handle,
	struct al_uc_gpio_handle	*uc_gpio);

/**
 * Initialize uc_handle
 * @param handle - uC controller handle
 * @param init - initialization params
 */
void al_uc_cmd_handle_init(
	struct al_uc_handle		*handle,
	struct al_uc_handle_init	*init);

/**
 * send host command to uC
 * @param handle uC handle
 * @param opcode host command opcode
 * @param param	param to pass to uC
 * @param resp_param response param from uC
 *
 * @return 0 upen success, other value upon failure
 */
int al_uc_cmd(
	struct al_uc_handle	*handle,
	uint32_t		opcode,
	uint32_t		param,
	uint32_t		*resp_param);

/**
 * Get uC application version
 * @param handle uC handle
 * @param ver uC application version
 *
 * @return 0 upon success, other value upon failure
 */
int al_uc_app_version_get(
	struct al_uc_handle	*handle,
	uint32_t		*ver);

/**
 * Get uC bootloader version
 * @param handle uC handle
 * @param ver uC bootloader version
 *
 * @return 0 upon success, other value upon failure
 */
int al_uc_bl_version_get(
	struct al_uc_handle	*handle,
	uint32_t		*ver);

/**
 * Update uC application
 * @param handle uC handle
 * @param image new uC image
 * @param image_size new uC image size
 *
 * @return 0 upon success
 */
int al_uc_app_update(
	struct al_uc_handle	*handle,
	uint32_t		image,
	unsigned int		image_size);

/**
 * Update uC bootloader
 * @param handle uC handle
 * @param image new uC image
 * @param image_size new uC image size
 *
 * @return 0 upon success
 */
int al_uc_bootloader_update(
	struct al_uc_handle	*handle,
	uint32_t		image,
	unsigned int		image_size);

/**
 * Update uC auth info
 * @param handle uC handle
 * @param image new uC image
 * @param image_size new uC image size
 *
 * @return 0 upon success
 */
int al_uc_auth_info_update(
	struct al_uc_handle	*handle,
	uint32_t		image,
	unsigned int		image_size);

/**
 * Reset Alpine
 * @param handle uC handle
 * @param delay_before_reset delay in secs before reset
 *
 * @return 0 upon success
 */
int al_uc_soc_reset(
	struct al_uc_handle	*handle,
	uint32_t		delay_before_reset);

/**
 * Get thermal interface status
 * @param handle uC handle
 * @param en is thermal interface enabled
 * @param i2c_addr thermal interface i2c address
 *
 * @return 0 upon success
 */
int al_uc_thermal_status_get(
	struct al_uc_handle	*handle,
	al_bool			*en,
	unsigned int		*i2c_addr);

/**
 * Set thermal interface
 * @param handle uC controller handle
 * @param en set enbaled or not
 * @param i2c_addr interface address
 *
 * @return 0 upon success
 */
int al_uc_thermal_enable_set(
	struct al_uc_handle	*handle,
	al_bool			en,
	unsigned int		i2c_addr);

/**
 * Get mgmt interface status
 * @param handle uC handle
 * @param en is mgmt interface enabled
 * @param i2c_addr mgmt interface i2c address
 *
 * @return 0 upon success
 */
int al_uc_mgmt_status_get(
	struct al_uc_handle	*handle,
	al_bool			*en,
	unsigned int		*i2c_addr);

/**
 * Set mgmt interface
 * @param handle uC handle
 * @param en  set enbaled or not
 * @param i2c_addr interface address
 *
 * @return 0 upon success
 */
int al_uc_mgmt_enable_set(
	struct al_uc_handle	*handle,
	al_bool			en,
	unsigned int		i2c_addr);

int al_uc_mgmt_report_reset(
	struct al_uc_handle			*handle,
	enum al_uc_mgmt_report_reset_cause	cause);

int al_uc_gpio_get(
	struct al_uc_handle	*handle,
	unsigned int		port,
	unsigned int		bit,
	unsigned int		*val);

int al_uc_gpio_set(
	struct al_uc_handle	*handle,
	unsigned int		port,
	unsigned int		bit,
	unsigned int		val);

int al_uc_gpio_dir_get(
	struct al_uc_handle	*handle,
	unsigned int		port,
	unsigned int		bit,
	al_bool			*is_output);

int al_uc_gpio_dir_set(
	struct al_uc_handle	*handle,
	unsigned int		port,
	unsigned int		bit,
	al_bool			is_output);

int al_uc_wd_status_get(
	struct al_uc_handle	*handle,
	al_bool			*en,
	unsigned int		*rst_cnt,
	unsigned int		*timeout);

int al_uc_wd_enable_set(
	struct al_uc_handle	*handle,
	al_bool			en,
	unsigned int		timeout);

int al_uc_eeprom_wp_lock_set(
	struct al_uc_handle	*handle,
	al_bool			lock);

/**
 * Saves pin state (value&direction)
 * @param  handle - uc controller handle
 * @param  port - port of pin to save
 * @param  bit - bit of pin to save
 * @param  pin_info - structure to save pin information
 * return  0 on success. Other value on fail.
 */
int al_uc_pin_save(
	struct al_uc_handle	*handle,
	unsigned int		port,
	unsigned int		bit,
	struct al_uc_pin_info	*pin_info);

/**
 * Restore pin state (value & direction) of uc pin.
 * @param  handle - uc controller handle
 * @param  pin_info - structure to restore pin information
 * return  0 on success. Other value on fail.
 */
int al_uc_pin_restore(
	struct al_uc_handle	*handle,
	struct al_uc_pin_info	*pin_info);

/**
 * Set the secure state
 * @param  handle - uc controller handle
 * @param  is_enabled - AL_TRUE - for secure state and AL_FALSE for none-secure state
 * return  0 on success. Other value on fail.
 */
int al_uc_set_secure(
	struct al_uc_handle	*handle,
	al_bool			is_enabled);

/**
 * Set value to physical preset line
 * @param  handle - uc controller handle
 * @param  is_enabled - AL_TRUE - physical present and AL_FALSE physical not present
 * return  0 on success. Other value on fail.
 */
int al_uc_set_pp(
	struct al_uc_handle	*handle,
	al_bool			is_enabled);

/**
 * Print sdr
 * @param  handle - uc controller handle
 * @param  sdr_data - sdr data
 * @param  num_bytes - number of bytes copied
 * @param  buffer_size - the max sdr_data buffer size
 * return  0 on success. Other value on fail.
 */
int al_uc_sdr_print(
	struct al_uc_handle	*handle,
	uint16_t		*sdr_data,
	unsigned int		*num_bytes,
	int			buffer_size);

int al_uc_sdr_reset(
	struct al_uc_handle	*handle);

/**
 * Get the secure state
 * @param  handle - uc controller handle
 * @param  resp_param - The secure state - 1 for secure 2 for not-secure. Otherwise - Unknown state
 * return  0 on success. Other value on fail.
 */
int al_uc_get_secure(
	struct al_uc_handle	*handle,
	uint32_t		*resp_param);

/**
 * Get physical present line value
 * @param  handle - uc controller handle
 * @param  resp_param - pp line value
 * return  0 on success. Other value on fail.
 */
int al_uc_get_pp(
	struct al_uc_handle	*handle,
	uint32_t		*resp_param);

/**
 * Get secure log
 * @param  handle - uc controller handle
 * @param  resp_param - the secure log
 * return  0 on success. Other value on fail.
 */
int al_uc_get_secure_log(
	struct al_uc_handle	*handle,
	uint32_t		*resp_param);
/**
 * Clear the secure log
 * @param  handle - uc controller handle
 * return  0 on success. Other value on fail.
 */
int al_uc_clear_secure_log(struct al_uc_handle	*handle);

/**
 * Retrieve thermal shutdown status(enabled or disabled)
 * @param  handle - uc controller handle
 * @param  thermal_shutdown_status - current thermal shutdown status
 * return  0 on success. Other value on fail.
 */
int al_uc_thermal_shutdown_status(
	struct al_uc_handle	*handle,
	uint32_t		*thermal_shutdown_status);

/**
 * Enable or disable thermal shutdown feature
 * @param  handle - uc controller handle
 * @param  is_enabled - AL_TRUE - thermal shutdown enabled and AL_FALSE thermal shutdown disabled
 * return  0 on success. Other value on fail.
 */
int al_uc_thermal_shutdown(
	struct al_uc_handle	*handle,
	al_bool			is_enabled);

/**
 * Set max temperature (celsius) before thermal shutdown
 * @param  handle - uc controller handle
 * @param  thermal_shutdown_max_temp - max temp for thermal shutdown
 * return  0 on success. Other value on fail.
 */
int al_uc_thermal_shutdown_set_tmp(
	struct al_uc_handle	*handle,
	unsigned int            thermal_shutdown_max_temp);

/**
 * Get current thermal shutdown temperature (celcious)
 * @param  handle - uc controller handle
 * @param  thermal_shutdown_max_temp - pointer to current shutdown temp
 * return  0 on success. Other value on fail.
 */
int al_uc_thermal_shutdown_get_tmp(
	struct al_uc_handle	*handle,
	unsigned int		*thermal_shutdown_max_temp);

int al_uc_nitro_mode_get(
	struct al_uc_handle	*handle,
	uint32_t		*nitro_mode_status);

int al_uc_nitro_mode_set(
	struct al_uc_handle	*handle,
	uint32_t		new_nitro_mode_status,
	uint32_t		nitro_perst_asserted_threshold,
	uint32_t		*nitro_mode_status);

int al_uc_nitro_perst_asserted_threshold_get(
	struct al_uc_handle	*handle,
	uint32_t		*perst_asserted_threshold);

/**
 * Send command to erase all 1024 [bytes] of FRU data
 * @param  handle - uc controller handle
 * return  0 on success. Other value on fail.
 */
int al_uc_fru_erase(struct al_uc_handle	*handle);

/**
 * Print fru
 * @param  handle - uc controller handle
 * @param  fru_data - fru data
 * @param  num_bytes - number of bytes copied
 * @param  buffer_size - the max fru_data buffer size
 * return  0 on success. Other value on fail.
 */
int al_uc_fru_print(
	struct al_uc_handle	*handle,
	uint32_t		*fru_data,
	unsigned int		*num_bytes,
	int			buffer_size);

/**
 * fru burn
 * @param  handle - uc controller handle
 * @param  image - the fru data
 * @param  image_size - size of the fru data
 * @param  image_phys_addr - image physical address
 * return  0 on success. Other value on fail.
 */
int al_uc_fru_burn(
	struct al_uc_handle	*handle,
	uint8_t			*image,
	unsigned int		image_size,
	al_phys_addr_t		image_phys_addr);

/**
 * Set value to uC config space (256 bytes) at a specific offset
 * @param  handle - uc controller handle
 * @param  address - offset to write to
 * @param  data - value to write
 * return  0 on success. Other value on fail.
 */
int al_uc_set_config(
	struct al_uc_handle	*handle,
	uint8_t			address,
	uint8_t			data);

/**
 * Get SDR status
 * @param handle - uc controller handle
 * @param en - sdr status
 * @param i2c_address - SDR interface i2c address
 * @return 0 on success, other value for failure
 */
int al_uc_sdr_status_get(
	struct al_uc_handle	*handle,
	al_bool			*en,
	unsigned int		*i2c_address);

/**
 * Set SDR status
 * @param handle - uc controller handle
 * @param en - sdr status
 * @param i2c_address - SDR interface i2c address
 * @return 0 on success, other value for failure
 */
int al_uc_sdr_status_set(
	struct al_uc_handle	*handle,
	al_bool			en,
	unsigned int		i2c_address);

/**
 * Set fw_update value in SDR
 * @param handle - uc controller handle
 * @param data - value to set
 * @return 0 on success, other value on fail

*/
int al_uc_sdr_fw_update_set(
	struct al_uc_handle	*handle,
	uint8_t			data);

/**
 * Get SDR version
 * @param handle - uc controller handle
 * @param ver - sdr version
 * @return 0 on success, other value on fail
 */
int al_uc_sdr_version_get(
	struct al_uc_handle	*handle,
	uint32_t		*ver);

/**
 * Get FRU status
 * @param handle - uC controller handle
 * @param en - FRU status
 * @param i2c_address - FRU i2c address
 * @return 0 on success, ther value on fail
 */
int al_uc_fru_status_get(
	struct al_uc_handle	*handle,
	al_bool			*en,
	unsigned int		*i2c_address);

/**
 * Set FRU status
 * @param handle - uC controller handle
 * @param en - FRU status
 * @param i2c_address - FRU interface i2c address
 * @return 0 on success, other value on fail
 */
int al_uc_fru_status_set(
	struct al_uc_handle	*handle,
	al_bool			en,
	unsigned int		i2c_address);

/**
 * Get mem_err counter from uC config
 * @param handle - uC controller handle
 * @param mem_err_val - counter value from uC
 * @return 0 on success, other value on fail
 */
int al_uc_cfg_mem_err_get(
	struct al_uc_handle	*handle,
	uint32_t		*mem_err_val);

/**
 * Set mem_err counter in uC config
 * @param handle - uC controller handle
 * @param data - value to write
 * @return 0 on success, other value on fail
 */
int al_uc_cfg_mem_err_set(
	struct al_uc_handle	*handle,
	uint8_t			data);

/**
 * Clear mem_err counter in uC config
 * @param handle - uC controller handle
 * @return 0 on success, other value on fail
 */
int al_uc_cfg_mem_err_clear(
	struct al_uc_handle	*handle);

/**
 * Set uC spi buffer to be enabled or disabled
 * @param handle - uC controller handle
 * @param is_enabled - AL_TRUE for enabling MOBO,
 * AL_FALSE for enabling TPM
 * @return 0 on success, other value on fail
 */
int al_uc_set_spi_buf(
	struct al_uc_handle	*handle,
	al_bool			is_enabled);

/**
 * Probe i2c slave of uC.
 * @param handle - uC controller handle
 * @param slave_addr - slave addr of the desired hardware
 * return  0 on success. Other value on fail.
 */
int al_uc_i2c_master_probe_slave(
	struct al_uc_handle	*handle,
	uint8_t			slave_addr);

/**
 * Update FRU image into SRAM.
 * @param  handle - uc controller handle
 * return  0 on success. Other value on fail.
 */
int al_uc_fru_sram_ready(
	struct al_uc_handle     *handle);

/**
 * Config uC pins to uart or gpios
 * @param handle - uC controller handle
 * @param en     - AL_TRUE for gpio, AL_FALSE for uart
 * return 0 on success
 */
int al_uc_uart_test(
	struct al_uc_handle	*handle,
	al_bool			en);

/**
 * Clear i2c fifo
 * @param handle - uC controller handle
 *
 * @return 0 on success
 */
int al_uc_i2c_clear_fifo(
	struct al_uc_handle	*handle);

/**
 * Set spi flash
 * @param handle - uC controller handle
 * @param val - spi flash to set
 *
 * @return 0 upon success
 */
int al_uc_spi_flash_set(
	struct al_uc_handle	*handle,
	uint32_t		val);

/**
 * Get SPI flash
 * @param handle - uC controller handle
 * @param val - spi flash value
 *
 * @return 0 upon success
 */
int al_uc_spi_flash_get(
	struct al_uc_handle	*handle,
	uint32_t		*val);

/**
 * Get SPI flash that we booted from
 * @param handle - uC controller handle
 * @param val - spi flash value
 *
 * @return 0 upon success
 */
int al_uc_spi_flash_booted_from(
	struct al_uc_handle	*handle,
	uint32_t		*val);

/**
 * Set CPU voltage
 * @param handle - uC controller handle
 * @param val - CPU value
 *
 * @return 0 upon success
 */
int al_uc_cpu_voltage_set(
	struct al_uc_handle	*handle,
	uint32_t		val);

/**
 * Get CPU voltage
 * @param handle - uC controller handle
 * @param val - CPU voltage value
 *
 * @return 0 upon success
 */
int al_uc_cpu_voltage_get(
	struct al_uc_handle	*handle,
	uint32_t		*val);

/**
 * Set uC DC controller max vout
 * @param handle - uC controller handle
 *
 * @return 0 upon success
 */
int al_uc_dc_controller_max_vout_set(
	struct al_uc_handle	*handle);

/**
 * Request uC to reset Alpine
 * @param handle - uC controller handle
 * @param req_reset
 *
 * @return 0 upon success
 */
int al_uc_reset_request(
	struct al_uc_handle	*handle,
	uint32_t		req_reset);

/**
 * Get uc liveness timer
 * @param handle - uC controller handle
 * @param timer - liveness timer
 *
 * @return 0 upon success
 */
int al_uc_liveness_timer_get(
	struct al_uc_handle	*handle,
	uint32_t		*timer);

/**
 * Send command to unlock FRU data
 * @param  handle - uc controller handle
 * @return  0 on success. Other value on fail.
 */
int al_uc_fru_unlock(
	struct al_uc_handle	*handle);

/**
 * Get FRU shared data state
 * @param  handle - uc controller handle
 * @param  is_valid - validation indication
 * @return  0 on success. Other value on fail.
 */
int al_uc_fru_shared_valid_get(
	struct al_uc_handle	*handle,
	al_bool			*is_valid);

/**
 * uC SPI preload read
 * @param  handle - uc controller handle
 * @param  buf - buffer to store the preload data
 * @param  size - size to read in bytes
 * @return  0 on success. Other value on fail.
 */
int al_uc_spi_preloader_read(
	struct al_uc_handle *handle,
	uint8_t *buf,
	unsigned int size);

/**
 * Burn preload data
 * @param  handle - uc controller handle
 * @param  buf - preload buffer to burn
 * @param  size - size to burn in bytes
 * @param  load_address - load address in DRAM to store the buffer
 * @return  0 on success. Other value on fail.
 */
int al_uc_spi_preloader_burn(
	struct al_uc_handle *handle,
	uint8_t *buf,
	unsigned int size,
	al_phys_addr_t load_address);

/**
 * Runs loopback test on physical loopback
 * @param handle - uc controller handle
 * @return 0 on success
 */
int al_uc_uart_phys_lb_test(struct al_uc_handle *handle);

/**
 * Set sdr register to masked
 * @param handle - uc controller handle
 * @param reg_num - sdr reg number
 * @return 0 on success
 */
int al_uc_sdr_reg_mask_set(
	struct al_uc_handle *handle,
	uint32_t reg_num);

/**
 * Cleat mask of sdr register
 * @param handle - uc controller handle
 * @param reg_num - sdr reg number
 * @return 0 on success
 */
int al_uc_sdr_reg_mask_clear(
	struct al_uc_handle *handle,
	uint32_t reg_num);
/**
 * Run alpaca config test
 * @param handle - uc controller handle
 * @param value - alpaca config test type
 * @return 0 on success
 */
int al_uc_alpaca_config_test(
	struct al_uc_handle *handle,
	uint32_t value);
/**
 * Get current uC application index(dual image)
 * @param handle - uc controller handle
 * @param app_idx - pointer to app idx
 * @return 0 on success
 */
int al_uc_app_idx_get(
	struct al_uc_handle *handle,
	uint32_t *app_idx);
/**
 * Set uC application index(dual image)
 * The action will take place only after uC AC power cycle
 * @param handle - uc controller handle
 * @param app_idx - desired app idx
 * @return 0 on success
 */
int al_uc_app_idx_set(
	struct al_uc_handle *handle,
	uint32_t app_idx);

/**
 * STG2 Notify uC about exiting secure mode
 * @param handle - uc controller handle
 * @param val - whether or not exit secure mode was successful
 * @return 0 on success
 */
int al_uc_stg2_done_exit_secure(
	struct al_uc_handle *handle,
	int val);

/**
 * host tells uC to exit secure mode
 * @param handle - uc controller handle
 * @return 0 on success
 */
int al_uc_exit_secure(struct al_uc_handle *handle);

#endif /* AL_HAL_HAS_UC_CMDS */

#endif /* __AL_UC_CMDS_H__ */
