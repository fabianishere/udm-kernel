/*
 * Copyright 2016, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#ifndef __AL_OTP_STORAGE_H__
#define __AL_OTP_STORAGE_H__

#include "al_hal_otp.h"
#include "al_otp_storage_layout.h"

/** Device serial number size */
#define AL_OTP_STORAGE_DEV_SER_NUM_SIZE	8
/** Public key hash size */
#define AL_OTP_STORAGE_PUK_HASH_SIZE	32
/** Password for irreversible operations */
#define AL_OTP_STORAGE_PASSWORD		0xb074b074

/**
 * Number of supported public key hashes
 * - Alpine V1/V2: 1
 * - Alpine V3 onwards: 4
 */
#define AL_OTP_STORAGE_PUK_HASH_NUM	4

/** Handle init parameters */
struct al_otp_storage_handle_init_params {
	/** OTP register base */
	void __iomem	*otp_regs_base;
	/** PBS register base */
	void __iomem	*pbs_regs_base;
};

/** Public key hash information */
struct al_otp_storage_db_puk_hash {
	/** Public key hash */
	uint8_t		puk_hash[AL_OTP_STORAGE_PUK_HASH_SIZE];
	/** Public key hash is locked */
	al_bool		puk_hash_is_locked;
};

/** Database */
struct al_otp_storage_db {
	/** Device serial number */
	uint8_t		dev_ser_num[AL_OTP_STORAGE_DEV_SER_NUM_SIZE];
	/** Device serial number is locked */
	al_bool		dev_ser_num_is_locked;
	/** Direct boot disabled */
	al_bool		direct_boot_disabled;
	/** Preload disabled */
	al_bool		preload_disabled;
	/** SPI debug disabled */
	al_bool		spi_debug_disabled;
	/** JTAG debug disabled */
	al_bool		jtag_debug_disabled;
	/** Secure boot disabled */
	al_bool		secure_boot_enabled;
	/** Boot ROM debug disabled */
	al_bool		boot_rom_debug_disabled;
	/** Boot from SPI disabled */
	al_bool		boot_from_spi_disabled;
	/** Boot form NAND disabled */
	al_bool		boot_from_nand_disabled;
	/** Boot from UART disabled */
	al_bool		boot_from_uart_disabled;
	/** Public key hash 0 */
	uint8_t		puk_hash[AL_OTP_STORAGE_PUK_HASH_SIZE];
	/** Public key hash 0 is locked */
	al_bool		puk_hash_is_locked;
	/** Minimal SW version */
	unsigned int	min_sw_ver;

	/* Alpine V3 onwards fields */

#define AL_OTP_STORAGE_DB_HAS_SW_INITIATED_MEM_REPAIR	1
	/** SW initiated memory repair */
	al_bool		sw_initiated_mem_repair;
	/** Public key hash information for all hashes */
	struct al_otp_storage_db_puk_hash	puk_hashes[AL_OTP_STORAGE_PUK_HASH_NUM];
	/** TZ enabled */
	al_bool		tz_enabled;
	/** UART CLI disabled */
	al_bool		uart_cli_disabled;
	/** JTAG SoC disabled */
	al_bool		jtag_soc_disabled;
	/** SPI debugger PASW enabled */
	al_bool		spi_dbg_pasw_enabled;
	/** OTPR override by preloader disabled */
	al_bool		otpr_override_by_pld_disabled;
	/** CPU debug and trace disabled */
	al_bool		cpu_dbg_trace_disabled;
	/** Secure debug and trace disabled */
	al_bool		secure_dbg_trace_disabled;
	/** Debug enabled */
	al_bool		dbg_enabled;
	/** Debug disabled */
	al_bool		dbg_disabled;
	/** Public key hash index */
	unsigned int	puk_idx;
	/** Public key hash index is locked */
	al_bool		puk_idx_is_locked;
	/** Chip Revision*/
	unsigned int	chip_revision;
};

/** Handle */
struct al_otp_storage_handle {
	struct al_otp_handle		otp_handle;
	unsigned int			dev_id;
	struct al_otp_storage_layout	layout;
};

/**
 * Handle initialization
 *
 * @param	handle
 *		Uninitialized handle
 * @param	params
 *		Initialization parameters
 */
void al_otp_storage_handle_init(
	struct al_otp_storage_handle			*handle,
	const struct al_otp_storage_handle_init_params	*params);

/**
 * Read database from OTP
 *
 * @param	handle
 *		Initialized handle
 * @param	db
 *		Database structure
 * @param	direct
 *		- AL_TRUE - read directly from OTP
 *		  This is required in order to make sure OTP was properly programmed
 *		  The programming will have effect only after SoC reset
 *		- AL_FALSE - read OTP shadow
 *		  This is required in order to get active OTP status according to which the HW
 *		  behaves
 */
void al_otp_storage_db_read(
	const struct al_otp_storage_handle	*handle,
	struct al_otp_storage_db		*db,
	al_bool					direct);

/**
 * Print database
 *
 * @param	db
 *		Database structure
 */
void al_otp_storage_db_print(
	struct al_otp_storage_db	*db);

/**
 * Device serial number set - irreversible!
 *
 * @param	handle
 *		Initialized handle
 * @param	password
 *		Password for irreversible operations (AL_OTP_STORAGE_PASSWORD)
 * @param	dev_ser_num
 *		Required device serial number
 *
 * @returns	0 upon success
 */
int al_otp_storage_dev_ser_num_set(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password,
	uint8_t					dev_ser_num[AL_OTP_STORAGE_DEV_SER_NUM_SIZE]);

/**
 * Device serial number lock - irreversible!
 *
 * @param	handle
 *		Initialized handle
 * @param	password
 *		Password for irreversible operations (AL_OTP_STORAGE_PASSWORD)
 *
 * @returns	0 upon success
 */
int al_otp_storage_dev_ser_num_lock(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password);

/**
 * Direct boot disable - irreversible!
 *
 * @param	handle
 *		Initialized handle
 * @param	password
 *		Password for irreversible operations (AL_OTP_STORAGE_PASSWORD)
 *
 * @returns	0 upon success
 */
int al_otp_storage_direct_boot_disable(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password);

/**
 * Preload disable - irreversible!
 *
 * @param	handle
 *		Initialized handle
 * @param	password
 *		Password for irreversible operations (AL_OTP_STORAGE_PASSWORD)
 *
 * @returns	0 upon success
 */
int al_otp_storage_preload_disable(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password);

/**
 * SPI debug disable - irreversible!
 *
 * @param	handle
 *		Initialized handle
 * @param	password
 *		Password for irreversible operations (AL_OTP_STORAGE_PASSWORD)
 *
 * @returns	0 upon success
 */
int al_otp_storage_spi_debug_disable(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password);

/**
 * JTAG debug disable - irreversible!
 *
 * @param	handle
 *		Initialized handle
 * @param	password
 *		Password for irreversible operations (AL_OTP_STORAGE_PASSWORD)
 *
 * @returns	0 upon success
 */
int al_otp_storage_jtag_debug_disable(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password);

/**
 * Secure boot enable - irreversible!
 *
 * @param	handle
 *		Initialized handle
 * @param	password
 *		Password for irreversible operations (AL_OTP_STORAGE_PASSWORD)
 *
 * @returns	0 upon success
 */
int al_otp_storage_secure_boot_enable(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password);

/**
 * Boot ROM debug disable - irreversible!
 *
 * @param	handle
 *		Initialized handle
 * @param	password
 *		Password for irreversible operations (AL_OTP_STORAGE_PASSWORD)
 *
 * @returns	0 upon success
 */
int al_otp_storage_boot_rom_debug_disable(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password);

/**
 * Boot from SPI disable - irreversible!
 *
 * @param	handle
 *		Initialized handle
 * @param	password
 *		Password for irreversible operations (AL_OTP_STORAGE_PASSWORD)
 *
 * @returns	0 upon success
 */
int al_otp_storage_boot_from_spi_disable(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password);

/**
 * Boot from NAND disable - irreversible!
 *
 * @param	handle
 *		Initialized handle
 * @param	password
 *		Password for irreversible operations (AL_OTP_STORAGE_PASSWORD)
 *
 * @returns	0 upon success
 */
int al_otp_storage_boot_from_nand_disable(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password);

/**
 * Boot from UART disable - irreversible!
 *
 * @param	handle
 *		Initialized handle
 * @param	password
 *		Password for irreversible operations (AL_OTP_STORAGE_PASSWORD)
 *
 * @returns	0 upon success
 */
int al_otp_storage_boot_from_uart_disable(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password);

/**
 * Public key hash set - irreversible!
 *
 * @param	handle
 *		Initialized handle
 * @param	password
 *		Password for irreversible operations (AL_OTP_STORAGE_PASSWORD)
 * @param	puk_hash
 *		Required public key hash
 *
 * @returns	0 upon success
 */
int al_otp_storage_puk_hash_set(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password,
	uint8_t					puk_hash[AL_OTP_STORAGE_PUK_HASH_SIZE]);

/**
 * Public key hash lock - irreversible!
 *
 * @param	handle
 *		Initialized handle
 * @param	password
 *		Password for irreversible operations (AL_OTP_STORAGE_PASSWORD)
 *
 * @returns	0 upon success
 */
int al_otp_storage_puk_hash_lock(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password);

/**
 * Minimal SW version set - irreversible!
 *
 * @param	handle
 *		Initialized handle
 * @param	password
 *		Password for irreversible operations (AL_OTP_STORAGE_PASSWORD)
 * @param	min_sw_ver
 *		Required minimal SW version
 *
 * @returns	0 upon success
 */
int al_otp_storage_min_sw_ver_set(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password,
	unsigned int				min_sw_ver);

/* Alpine V3 onwards functions */

#define AL_OTP_STORAGE_HAS_V3_SUPPORT	1

/**
 * Public key hash set - irreversible!
 *
 * @param	handle
 *		Initialized handle
 * @param	password
 *		Password for irreversible operations (AL_OTP_STORAGE_PASSWORD)
 * @param	puk_idx
 *		Public key hash index
 * @param	puk_hash
 *		Required public key hash
 *
 * @returns	0 upon success
 */
int al_otp_storage_puk_hash_adv_set(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password,
	unsigned int				puk_idx,
	uint8_t					puk_hash[AL_OTP_STORAGE_PUK_HASH_SIZE]);

/**
 * Public key hash lock - irreversible!
 *
 * @param	handle
 *		Initialized handle
 * @param	password
 *		Password for irreversible operations (AL_OTP_STORAGE_PASSWORD)
 * @param	puk_idx
 *		Public key hash index
 *
 * @returns	0 upon success
 */
int al_otp_storage_puk_hash_adv_lock(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password,
	unsigned int				puk_idx);

/**
 * Public key hash index set - irreversible!
 *
 * @param	handle
 *		Initialized handle
 * @param	password
 *		Password for irreversible operations (AL_OTP_STORAGE_PASSWORD)
 * @param	puk_idx
 *		Required public key hash index
 *
 * @returns	0 upon success
 */
int al_otp_storage_puk_hash_idx_set(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password,
	unsigned int				puk_idx);

/**
 * Public key hash index lock - irreversible!
 *
 * @param	handle
 *		Initialized handle
 * @param	password
 *		Password for irreversible operations (AL_OTP_STORAGE_PASSWORD)
 *
 * @returns	0 upon success
 */
int al_otp_storage_puk_hash_idx_lock(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password);

/**
 * UART CLI disable - irreversible!
 *
 * @param	handle
 *		Initialized handle
 * @param	password
 *		Password for irreversible operations (AL_OTP_STORAGE_PASSWORD)
 *
 * @returns	0 upon success
 */
int al_otp_storage_uart_cli_disable(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password);

/**
 * TZ enable - irreversible!
 *
 * @param	handle
 *		Initialized handle
 * @param	password
 *		Password for irreversible operations (AL_OTP_STORAGE_PASSWORD)
 *
 * @returns	0 upon success
 */
int al_otp_storage_tz_enable(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password);

/**
 * JTAG SoC disable - irreversible!
 *
 * @param	handle
 *		Initialized handle
 * @param	password
 *		Password for irreversible operations (AL_OTP_STORAGE_PASSWORD)
 *
 * @returns	0 upon success
 */
int al_otp_storage_jtag_soc_disable(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password);

/**
 * SPI debug PASW enable - irreversible!
 *
 * @param	handle
 *		Initialized handle
 * @param	password
 *		Password for irreversible operations (AL_OTP_STORAGE_PASSWORD)
 *
 * @returns	0 upon success
 */
int al_otp_storage_spi_dbg_pasw_enable(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password);

/**
 * OTPR override by preloader disable - irreversible!
 *
 * @param	handle
 *		Initialized handle
 * @param	password
 *		Password for irreversible operations (AL_OTP_STORAGE_PASSWORD)
 *
 * @returns	0 upon success
 */
int al_otp_storage_otpr_override_by_pld_disable(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password);

/**
 * CPU debug & trace disable - irreversible!
 *
 * @param	handle
 *		Initialized handle
 * @param	password
 *		Password for irreversible operations (AL_OTP_STORAGE_PASSWORD)
 *
 * @returns	0 upon success
 */
int al_otp_storage_cpu_dbg_trace_disable(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password);

/**
 * secure debug & trace disable - irreversible!
 *
 * @param	handle
 *		Initialized handle
 * @param	password
 *		Password for irreversible operations (AL_OTP_STORAGE_PASSWORD)
 *
 * @returns	0 upon success
 */
int al_otp_storage_secure_dbg_trace_disable(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password);

/**
 * RMA debug enable - irreversible!
 *
 * @param	handle
 *		Initialized handle
 * @param	password
 *		Password for irreversible operations (AL_OTP_STORAGE_PASSWORD)
 *
 * @returns	0 upon success
 */
int al_otp_storage_dbg_enable(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password);

/**
 * RMA debug disable - irreversible!
 *
 * @param	handle
 *		Initialized handle
 * @param	password
 *		Password for irreversible operations (AL_OTP_STORAGE_PASSWORD)
 *
 * @returns	0 upon success
 */
int al_otp_storage_dbg_disable(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password);

#endif

