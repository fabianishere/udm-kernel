/*
 * Copyright 2016, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#include "al_otp_storage.h"
#include "al_otp_storage_layout.h"
#include "al_otp_storage_layout_get_v2.h"
#include "al_otp_storage_layout_get_v3.h"
#include "al_hal_pbs_utils.h"

#define WORD_NUM_BYTES	4
#define WORD_NUM_BITS	(8 * WORD_NUM_BYTES)

#define OTP_FIELD_WORD_OFFSET_DEVICE_SER_NUM			\
	(handle->layout.WORD_OFFSET_DEVICE_SER_NUM)
#define OTP_FIELD_WORD_SIZE_DEVICE_SER_NUM			\
	(handle->layout.WORD_SIZE_DEVICE_SER_NUM)
#define OTP_FIELD_WORD_OFFSET_MINIMAL_SW_VER			\
	(handle->layout.WORD_OFFSET_MINIMAL_SW_VER)
#define OTP_FIELD_WORD_SIZE_MINIMAL_SW_VER			\
	(handle->layout.WORD_SIZE_MINIMAL_SW_VER)
#define OTP_FIELD_WORD_OFFSET_PUK0_MOD_HASH			\
	(handle->layout.WORD_OFFSET_PUK0_MOD_HASH)
#define OTP_FIELD_WORD_SIZE_PUK0_MOD_HASH			\
	(handle->layout.WORD_SIZE_PUK0_MOD_HASH)
#define OTP_FIELD_WORD_OFFSET_PUK1_MOD_HASH			\
	(handle->layout.WORD_OFFSET_PUK1_MOD_HASH)
#define OTP_FIELD_WORD_SIZE_PUK1_MOD_HASH			\
	(handle->layout.WORD_SIZE_PUK1_MOD_HASH)
#define OTP_FIELD_WORD_OFFSET_PUK2_MOD_HASH			\
	(handle->layout.WORD_OFFSET_PUK2_MOD_HASH)
#define OTP_FIELD_WORD_SIZE_PUK2_MOD_HASH			\
	(handle->layout.WORD_SIZE_PUK2_MOD_HASH)
#define OTP_FIELD_WORD_OFFSET_PUK_IDX				\
	(handle->layout.WORD_OFFSET_PUK_IDX)
#define OTP_FIELD_BIT_OFFSET_PUK_IDX				\
	(handle->layout.BIT_OFFSET_PUK_IDX)
#define OTP_FIELD_WORD_OFFSET_EFUSE_PLD				\
	(handle->layout.WORD_OFFSET_EFUSE_PLD)
#define OTP_FIELD_WORD_SIZE_EFUSE_PLD				\
	(handle->layout.WORD_SIZE_EFUSE_PLD)
#define OTP_FIELD_WORD_OFFSET_PUK3_MOD_HASH			\
	(handle->layout.WORD_OFFSET_PUK3_MOD_HASH)
#define OTP_FIELD_WORD_SIZE_PUK3_MOD_HASH			\
	(handle->layout.WORD_SIZE_PUK3_MOD_HASH)
#define OTP_FIELD_WORD_OFFSET_HW_INITIATED_MEM_REPAIR		\
	(handle->layout.WORD_OFFSET_HW_INITIATED_MEM_REPAIR)
#define OTP_FIELD_BIT_OFFSET_HW_INITIATED_MEM_REPAIR		\
	(handle->layout.BIT_OFFSET_HW_INITIATED_MEM_REPAIR)
#define OTP_FIELD_WORD_OFFSET_SW_INITIATED_MEM_REPAIR		\
	(handle->layout.WORD_OFFSET_SW_INITIATED_MEM_REPAIR)
#define OTP_FIELD_BIT_OFFSET_HW_INITIATED_MEM_REPAIR		\
	(handle->layout.BIT_OFFSET_HW_INITIATED_MEM_REPAIR)
#define OTP_FIELD_WORD_OFFSET_DIRECT_BOOT_DIS			\
	(handle->layout.WORD_OFFSET_DIRECT_BOOT_DIS)
#define OTP_FIELD_BIT_OFFSET_DIRECT_BOOT_DIS			\
	(handle->layout.BIT_OFFSET_DIRECT_BOOT_DIS)
#define OTP_FIELD_WORD_OFFSET_PRELOAD_DISABLE			\
	(handle->layout.WORD_OFFSET_PRELOAD_DISABLE)
#define OTP_FIELD_BIT_OFFSET_PRELOAD_DISABLE			\
	(handle->layout.BIT_OFFSET_PRELOAD_DISABLE)
#define OTP_FIELD_WORD_OFFSET_SPI_DBG_DISABLE			\
	(handle->layout.WORD_OFFSET_SPI_DBG_DISABLE)
#define OTP_FIELD_BIT_OFFSET_SPI_DBG_DISABLE			\
	(handle->layout.BIT_OFFSET_SPI_DBG_DISABLE)
#define OTP_FIELD_WORD_OFFSET_JTAG_DBG_DISABLE			\
	(handle->layout.WORD_OFFSET_JTAG_DBG_DISABLE)
#define OTP_FIELD_BIT_OFFSET_JTAG_DBG_DISABLE			\
	(handle->layout.BIT_OFFSET_JTAG_DBG_DISABLE)
#define OTP_FIELD_WORD_OFFSET_SECURE_BOOT_EN			\
	(handle->layout.WORD_OFFSET_SECURE_BOOT_EN)
#define OTP_FIELD_BIT_OFFSET_SECURE_BOOT_EN			\
	(handle->layout.BIT_OFFSET_SECURE_BOOT_EN)
#define OTP_FIELD_WORD_OFFSET_ROM_DBG_DIS			\
	(handle->layout.WORD_OFFSET_ROM_DBG_DIS)
#define OTP_FIELD_BIT_OFFSET_ROM_DBG_DIS			\
	(handle->layout.BIT_OFFSET_ROM_DBG_DIS)
#define OTP_FIELD_WORD_OFFSET_SNOR_SECURE_BOOT_DIS		\
	(handle->layout.WORD_OFFSET_SNOR_SECURE_BOOT_DIS)
#define OTP_FIELD_BIT_OFFSET_SNOR_SECURE_BOOT_DIS		\
	(handle->layout.BIT_OFFSET_SNOR_SECURE_BOOT_DIS)
#define OTP_FIELD_WORD_OFFSET_NAND_SECURE_BOOT_DIS		\
	(handle->layout.WORD_OFFSET_NAND_SECURE_BOOT_DIS)
#define OTP_FIELD_BIT_OFFSET_NAND_SECURE_BOOT_DIS		\
	(handle->layout.BIT_OFFSET_NAND_SECURE_BOOT_DIS)
#define OTP_FIELD_WORD_OFFSET_UART_SECURE_BOOT_DIS		\
	(handle->layout.WORD_OFFSET_UART_SECURE_BOOT_DIS)
#define OTP_FIELD_BIT_OFFSET_UART_SECURE_BOOT_DIS		\
	(handle->layout.BIT_OFFSET_UART_SECURE_BOOT_DIS)
#define OTP_FIELD_WORD_OFFSET_UART_CLI_SECURE_BOOT_DIS		\
	(handle->layout.WORD_OFFSET_UART_CLI_SECURE_BOOT_DIS)
#define OTP_FIELD_BIT_OFFSET_UART_CLI_SECURE_BOOT_DIS		\
	(handle->layout.BIT_OFFSET_UART_CLI_SECURE_BOOT_DIS)
#define OTP_FIELD_WORD_OFFSET_TZ_EN				\
	(handle->layout.WORD_OFFSET_TZ_EN)
#define OTP_FIELD_BIT_OFFSET_TZ_EN				\
	(handle->layout.BIT_OFFSET_TZ_EN)
#define OTP_FIELD_WORD_OFFSET_JTAG_SOC_DISABLE			\
	(handle->layout.WORD_OFFSET_JTAG_SOC_DISABLE)
#define OTP_FIELD_BIT_OFFSET_JTAG_SOC_DISABLE			\
	(handle->layout.BIT_OFFSET_JTAG_SOC_DISABLE)
#define OTP_FIELD_WORD_OFFSET_SPI_DBG_PASW_EN			\
	(handle->layout.WORD_OFFSET_SPI_DBG_PASW_EN)
#define OTP_FIELD_BIT_OFFSET_SPI_DBG_PASW_EN			\
	(handle->layout.BIT_OFFSET_SPI_DBG_PASW_EN)
#define OTP_FIELD_WORD_OFFSET_OTPR_OVERRIDE_BY_PLD_DIS		\
	(handle->layout.WORD_OFFSET_OTPR_OVERRIDE_BY_PLD_DIS)
#define OTP_FIELD_BIT_OFFSET_OTPR_OVERRIDE_BY_PLD_DIS		\
	(handle->layout.BIT_OFFSET_OTPR_OVERRIDE_BY_PLD_DIS)
#define OTP_FIELD_WORD_OFFSET_CPU_DBG_AND_TRACE_DIS		\
	(handle->layout.WORD_OFFSET_CPU_DBG_AND_TRACE_DIS)
#define OTP_FIELD_BIT_OFFSET_CPU_DBG_AND_TRACE_DIS		\
	(handle->layout.BIT_OFFSET_CPU_DBG_AND_TRACE_DIS)
#define OTP_FIELD_WORD_OFFSET_SECURE_DBG_AND_TRACE_DIS		\
	(handle->layout.WORD_OFFSET_SECURE_DBG_AND_TRACE_DIS)
#define OTP_FIELD_BIT_OFFSET_SECURE_DBG_AND_TRACE_DIS		\
	(handle->layout.BIT_OFFSET_SECURE_DBG_AND_TRACE_DIS)
#define OTP_FIELD_WORD_OFFSET_DBG_EN				\
	(handle->layout.WORD_OFFSET_DBG_EN)
#define OTP_FIELD_BIT_OFFSET_CHIP_REVISION_ID				\
	(handle->layout.BIT_OFFSET_CHIP_REVISION_ID)
#define OTP_FIELD_WORD_OFFSET_CHIP_REVISION_ID				\
	(handle->layout.WORD_OFFSET_CHIP_REVISION_ID)
#define OTP_FIELD_BIT_OFFSET_DBG_EN				\
	(handle->layout.BIT_OFFSET_DBG_EN)
#define OTP_FIELD_BIT_MASK_SET_PUK_IDX				\
	(handle->layout.BIT_MASK_SET_PUK_IDX)
#define OTP_FIELD_BIT_MASK_SET_HW_INITIATED_MEM_REPAIR		\
	(handle->layout.BIT_MASK_SET_HW_INITIATED_MEM_REPAIR)
#define OTP_FIELD_BIT_MASK_SET_SW_INITIATED_MEM_REPAIR		\
	(handle->layout.BIT_MASK_SET_SW_INITIATED_MEM_REPAIR)
#define OTP_FIELD_BIT_MASK_SET_DIRECT_BOOT_DIS			\
	(handle->layout.BIT_MASK_SET_DIRECT_BOOT_DIS)
#define OTP_FIELD_BIT_MASK_SET_PRELOAD_DISABLE			\
	(handle->layout.BIT_MASK_SET_PRELOAD_DISABLE)
#define OTP_FIELD_BIT_MASK_SET_SPI_DBG_DISABLE			\
	(handle->layout.BIT_MASK_SET_SPI_DBG_DISABLE)
#define OTP_FIELD_BIT_MASK_SET_JTAG_DBG_DISABLE			\
	(handle->layout.BIT_MASK_SET_JTAG_DBG_DISABLE)
#define OTP_FIELD_BIT_MASK_SET_SECURE_BOOT_EN			\
	(handle->layout.BIT_MASK_SET_SECURE_BOOT_EN)
#define OTP_FIELD_BIT_MASK_SET_ROM_DBG_DIS			\
	(handle->layout.BIT_MASK_SET_ROM_DBG_DIS)
#define OTP_FIELD_BIT_MASK_SET_SNOR_SECURE_BOOT_DIS		\
	(handle->layout.BIT_MASK_SET_SNOR_SECURE_BOOT_DIS)
#define OTP_FIELD_BIT_MASK_SET_NAND_SECURE_BOOT_DIS		\
	(handle->layout.BIT_MASK_SET_NAND_SECURE_BOOT_DIS)
#define OTP_FIELD_BIT_MASK_SET_UART_SECURE_BOOT_DIS		\
	(handle->layout.BIT_MASK_SET_UART_SECURE_BOOT_DIS)
#define OTP_FIELD_BIT_MASK_SET_UART_CLI_SECURE_BOOT_DIS		\
	(handle->layout.BIT_MASK_SET_UART_CLI_SECURE_BOOT_DIS)
#define OTP_FIELD_BIT_MASK_SET_TZ_EN				\
	(handle->layout.BIT_MASK_SET_TZ_EN)
#define OTP_FIELD_BIT_MASK_SET_JTAG_SOC_DISABLE			\
	(handle->layout.BIT_MASK_SET_JTAG_SOC_DISABLE)
#define OTP_FIELD_BIT_MASK_SET_SPI_DBG_PASW_EN			\
	(handle->layout.BIT_MASK_SET_SPI_DBG_PASW_EN)
#define OTP_FIELD_BIT_MASK_SET_OTPR_OVERRIDE_BY_PLD_DIS		\
	(handle->layout.BIT_MASK_SET_OTPR_OVERRIDE_BY_PLD_DIS)
#define OTP_FIELD_BIT_MASK_SET_CPU_DBG_AND_TRACE_DIS		\
	(handle->layout.BIT_MASK_SET_CPU_DBG_AND_TRACE_DIS)
#define OTP_FIELD_BIT_MASK_SET_SECURE_DBG_AND_TRACE_DIS		\
	(handle->layout.BIT_MASK_SET_SECURE_DBG_AND_TRACE_DIS)
#define OTP_FIELD_BIT_MASK_SET_DBG_EN				\
	(handle->layout.BIT_MASK_SET_DBG_EN)
#define OTP_FIELD_BIT_MASK_SET_REVISION_ID			\
	(handle->layout.BIT_MASK_SET_CHIP_REVISION_ID)
#define OTP_FIELD_BIT_OFFSET_DBG_EN_EN		OTP_FIELD_BIT_OFFSET_DBG_EN
#define OTP_FIELD_BIT_MASK_SET_DBG_EN_EN	AL_BIT(OTP_FIELD_BIT_OFFSET_DBG_EN_EN)
#define OTP_FIELD_BIT_OFFSET_DBG_EN_DIS		(OTP_FIELD_BIT_OFFSET_DBG_EN + 1)
#define OTP_FIELD_BIT_MASK_SET_DBG_EN_DIS	AL_BIT(OTP_FIELD_BIT_OFFSET_DBG_EN_DIS)

/**************************************************************************************************/
/**************************************************************************************************/
static uint32_t otp_word_read(
	const struct al_otp_storage_handle	*handle,
	al_bool					direct,
	unsigned int				word_idx)
{
	if (direct)
		return al_otp_read_word_direct(&handle->otp_handle, word_idx);
	else
		return al_otp_read_word(&handle->otp_handle, word_idx);
}

/**************************************************************************************************/
/**************************************************************************************************/
static uint32_t otp_word_is_locked(
	const struct al_otp_storage_handle	*handle,
	al_bool					direct,
	unsigned int				word_idx)
{
	if (direct)
		return al_otp_word_is_locked_direct(&handle->otp_handle, word_idx);
	else
		return al_otp_word_is_locked(&handle->otp_handle, word_idx);
}

/**************************************************************************************************/
/**************************************************************************************************/
void al_otp_storage_handle_init(
	struct al_otp_storage_handle			*handle,
	const struct al_otp_storage_handle_init_params	*params)
{
	unsigned int dev_id;
	int err;

	al_assert(handle);
	al_assert(params);

	dev_id = al_pbs_dev_id_get(params->pbs_regs_base);
	handle->dev_id = dev_id;

	if ((dev_id == PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V1) ||
		(dev_id == PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V2))
		al_otp_storage_layout_get_v2(&handle->layout);
	else
		al_otp_storage_layout_get_v3(&handle->layout);

	al_assert((WORD_NUM_BYTES * OTP_FIELD_WORD_SIZE_DEVICE_SER_NUM) ==
		AL_OTP_STORAGE_DEV_SER_NUM_SIZE);
	al_assert((WORD_NUM_BYTES * OTP_FIELD_WORD_SIZE_PUK0_MOD_HASH) ==
		AL_OTP_STORAGE_PUK_HASH_SIZE);

	if (dev_id >= PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V3) {
		al_assert((WORD_NUM_BYTES * OTP_FIELD_WORD_SIZE_PUK1_MOD_HASH) ==
			AL_OTP_STORAGE_PUK_HASH_SIZE);
		al_assert((WORD_NUM_BYTES * OTP_FIELD_WORD_SIZE_PUK2_MOD_HASH) ==
			AL_OTP_STORAGE_PUK_HASH_SIZE);
		al_assert((WORD_NUM_BYTES * OTP_FIELD_WORD_SIZE_PUK3_MOD_HASH) ==
			AL_OTP_STORAGE_PUK_HASH_SIZE);
	}
	al_assert(OTP_FIELD_WORD_OFFSET_ROM_DBG_DIS == OTP_FIELD_WORD_OFFSET_SECURE_BOOT_EN);
	al_assert(OTP_FIELD_WORD_OFFSET_ROM_DBG_DIS == OTP_FIELD_WORD_OFFSET_SNOR_SECURE_BOOT_DIS);
	al_assert(OTP_FIELD_WORD_OFFSET_ROM_DBG_DIS == OTP_FIELD_WORD_OFFSET_NAND_SECURE_BOOT_DIS);
	al_assert(OTP_FIELD_WORD_OFFSET_ROM_DBG_DIS == OTP_FIELD_WORD_OFFSET_UART_SECURE_BOOT_DIS);
	al_assert(OTP_FIELD_WORD_SIZE_MINIMAL_SW_VER == 1);
	err = al_otp_handle_init(&handle->otp_handle, params->otp_regs_base, params->pbs_regs_base);
	al_assert(!err);
}

/**************************************************************************************************/
/**************************************************************************************************/
static unsigned int al_otp_storage_num_puks_get(
	const struct al_otp_storage_handle	*handle)
{
	unsigned int num_puks;

	if (handle->dev_id >= PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V3)
		num_puks = 4;
	else
		num_puks = 1;

	return num_puks;
}

/**************************************************************************************************/
/**************************************************************************************************/
static int al_otp_storage_puk_layout_get(
	const struct al_otp_storage_handle	*handle,
	unsigned int				puk_idx,
	unsigned int				*word_offset,
	unsigned int				*word_size)
{
	if (puk_idx >= al_otp_storage_num_puks_get(handle))
		return -EINVAL;

	switch (puk_idx) {
	case 0:
		*word_offset = OTP_FIELD_WORD_OFFSET_PUK0_MOD_HASH;
		*word_size = OTP_FIELD_WORD_SIZE_PUK0_MOD_HASH;
		break;
	case 1:
		*word_offset = OTP_FIELD_WORD_OFFSET_PUK1_MOD_HASH;
		*word_size = OTP_FIELD_WORD_SIZE_PUK1_MOD_HASH;
		break;
	case 2:
		*word_offset = OTP_FIELD_WORD_OFFSET_PUK2_MOD_HASH;
		*word_size = OTP_FIELD_WORD_SIZE_PUK2_MOD_HASH;
		break;
	case 3:
		*word_offset = OTP_FIELD_WORD_OFFSET_PUK3_MOD_HASH;
		*word_size = OTP_FIELD_WORD_SIZE_PUK3_MOD_HASH;
		break;
	default:
		al_assert(0);
		return -EINVAL;
	}

	return 0;
}

/**************************************************************************************************/
/**************************************************************************************************/
void al_otp_storage_db_read(
	const struct al_otp_storage_handle	*handle,
	struct al_otp_storage_db		*db,
	al_bool					direct)
{
	uint32_t w;
	unsigned int i;
	unsigned int puk_idx;
	unsigned int num_puks;

	al_assert(handle);
	al_assert(db);

	/* Device serial # */
	db->dev_ser_num_is_locked = AL_TRUE;
	for (i = 0; i < OTP_FIELD_WORD_SIZE_DEVICE_SER_NUM; i++) {
		unsigned int w_idx = OTP_FIELD_WORD_OFFSET_DEVICE_SER_NUM + i;

		w = otp_word_read(handle, direct, w_idx);
		al_memcpy(&db->dev_ser_num[i * WORD_NUM_BYTES], &w, WORD_NUM_BYTES);
		if (!otp_word_is_locked(handle, direct, w_idx))
			db->dev_ser_num_is_locked = AL_FALSE;
	}


	/* Various flags */
	w = otp_word_read(handle, direct, OTP_FIELD_WORD_OFFSET_ROM_DBG_DIS);

	db->direct_boot_disabled = !!(w & OTP_FIELD_BIT_MASK_SET_DIRECT_BOOT_DIS);
	db->preload_disabled = !!(w & OTP_FIELD_BIT_MASK_SET_PRELOAD_DISABLE);
	db->spi_debug_disabled = !!(w & OTP_FIELD_BIT_MASK_SET_SPI_DBG_DISABLE);
	db->jtag_debug_disabled = !!(w & OTP_FIELD_BIT_MASK_SET_JTAG_DBG_DISABLE);
	db->secure_boot_enabled = !!(w & OTP_FIELD_BIT_MASK_SET_SECURE_BOOT_EN);
	db->boot_rom_debug_disabled = !!(w & OTP_FIELD_BIT_MASK_SET_ROM_DBG_DIS);
	db->boot_from_spi_disabled = !!(w & OTP_FIELD_BIT_MASK_SET_SNOR_SECURE_BOOT_DIS);
	db->boot_from_nand_disabled = !!(w & OTP_FIELD_BIT_MASK_SET_NAND_SECURE_BOOT_DIS);
	db->boot_from_uart_disabled = !!(w & OTP_FIELD_BIT_MASK_SET_UART_SECURE_BOOT_DIS);

	if (handle->dev_id >= PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V3) {
		w = otp_word_read(handle, direct, OTP_FIELD_WORD_OFFSET_SW_INITIATED_MEM_REPAIR);
		db->sw_initiated_mem_repair =
			!!(w & OTP_FIELD_BIT_MASK_SET_SW_INITIATED_MEM_REPAIR);
	} else {
		db->sw_initiated_mem_repair = AL_FALSE;
	}

	/* Minimal SW version */
	db->min_sw_ver = 0;
	w = otp_word_read(handle, direct, OTP_FIELD_WORD_OFFSET_MINIMAL_SW_VER);
	for (i = 0; i < WORD_NUM_BITS; i++, w >>= 1) {
		if ((w & 1))
			db->min_sw_ver = i + 1;
	}

	/* PUK hash */
	num_puks = al_otp_storage_num_puks_get(handle);
	for (puk_idx = 0; puk_idx < AL_OTP_STORAGE_PUK_HASH_NUM; puk_idx++) {
		unsigned int word_offset;
		unsigned int word_size;
		int err;

		if (puk_idx >= num_puks) {
			al_memset(&db->puk_hashes[puk_idx], 0,
				sizeof(struct al_otp_storage_db_puk_hash));
			continue;
		}

		err = al_otp_storage_puk_layout_get(handle, puk_idx, &word_offset, &word_size);
		if (err) {
			al_assert(0);
			return;
		}

		db->puk_hashes[puk_idx].puk_hash_is_locked = AL_TRUE;
		for (i = 0; i < word_size; i++) {
			unsigned int w_idx = word_offset + i;

			w = otp_word_read(handle, direct, w_idx);
			al_memcpy(&db->puk_hashes[puk_idx].puk_hash[i * WORD_NUM_BYTES],
				&w, WORD_NUM_BYTES);
			if (!otp_word_is_locked(handle, direct, w_idx))
				db->puk_hashes[puk_idx].puk_hash_is_locked = AL_FALSE;
		}
	}
	db->puk_hash_is_locked = db->puk_hashes[0].puk_hash_is_locked;
	al_memcpy(db->puk_hash, db->puk_hashes[0].puk_hash, AL_OTP_STORAGE_PUK_HASH_SIZE);

	/* Misc Alpine V3 fields */
	if (handle->dev_id >= PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V3) {
		db->puk_idx = 0;
		w = otp_word_read(handle, direct, OTP_FIELD_WORD_OFFSET_PUK_IDX);
		for (i = 0; i < (num_puks - 1); i++, w >>= 1) {
			if ((w & AL_BIT(OTP_FIELD_BIT_OFFSET_PUK_IDX)))
				db->puk_idx = i + 1;
		}

		w = otp_word_read(handle, direct, OTP_FIELD_WORD_OFFSET_SW_INITIATED_MEM_REPAIR);
		db->sw_initiated_mem_repair =
			!!(w & OTP_FIELD_BIT_MASK_SET_SW_INITIATED_MEM_REPAIR);

		w = otp_word_read(handle, direct, OTP_FIELD_WORD_OFFSET_TZ_EN);
		db->tz_enabled =
			!!(w & OTP_FIELD_BIT_MASK_SET_TZ_EN);

		w = otp_word_read(handle, direct, OTP_FIELD_WORD_OFFSET_UART_CLI_SECURE_BOOT_DIS);
		db->uart_cli_disabled = !!(w & OTP_FIELD_BIT_MASK_SET_UART_CLI_SECURE_BOOT_DIS);

		w = otp_word_read(handle, direct, OTP_FIELD_WORD_OFFSET_JTAG_SOC_DISABLE);
		db->jtag_soc_disabled = !!(w & OTP_FIELD_BIT_MASK_SET_JTAG_SOC_DISABLE);

		w = otp_word_read(handle, direct, OTP_FIELD_WORD_OFFSET_SPI_DBG_PASW_EN);
		db->spi_dbg_pasw_enabled = !!(w & OTP_FIELD_BIT_MASK_SET_SPI_DBG_PASW_EN);

		w = otp_word_read(handle, direct, OTP_FIELD_WORD_OFFSET_OTPR_OVERRIDE_BY_PLD_DIS);
		db->otpr_override_by_pld_disabled =
			!!(w & OTP_FIELD_BIT_MASK_SET_OTPR_OVERRIDE_BY_PLD_DIS);

		w = otp_word_read(handle, direct, OTP_FIELD_WORD_OFFSET_CPU_DBG_AND_TRACE_DIS);
		db->cpu_dbg_trace_disabled =
			!!(w & OTP_FIELD_BIT_MASK_SET_CPU_DBG_AND_TRACE_DIS);

		w = otp_word_read(handle, direct, OTP_FIELD_WORD_OFFSET_SECURE_DBG_AND_TRACE_DIS);
		db->secure_dbg_trace_disabled =
			!!(w & OTP_FIELD_BIT_MASK_SET_SECURE_DBG_AND_TRACE_DIS);

		w = otp_word_read(handle, direct, OTP_FIELD_WORD_OFFSET_DBG_EN);
		db->dbg_disabled = !!(w & OTP_FIELD_BIT_MASK_SET_DBG_EN_DIS);
		db->dbg_enabled =
			(!db->dbg_disabled) && (!!(w & OTP_FIELD_BIT_MASK_SET_DBG_EN_EN));

		db->puk_idx_is_locked =
			otp_word_is_locked(handle, direct, OTP_FIELD_WORD_OFFSET_PUK_IDX);
		w = otp_word_read(handle, direct, OTP_FIELD_WORD_OFFSET_CHIP_REVISION_ID);
		db->chip_revision =
			(w & OTP_FIELD_BIT_MASK_SET_REVISION_ID)
				>> OTP_FIELD_BIT_OFFSET_CHIP_REVISION_ID;
	} else {
		db->puk_idx = 0;
		db->puk_idx_is_locked = AL_FALSE;
		db->tz_enabled = AL_FALSE;
		db->uart_cli_disabled = AL_FALSE;
		db->jtag_soc_disabled = AL_FALSE;
		db->spi_dbg_pasw_enabled = AL_FALSE;
		db->otpr_override_by_pld_disabled = AL_FALSE;
		db->cpu_dbg_trace_disabled = AL_FALSE;
		db->secure_dbg_trace_disabled = AL_FALSE;
		db->dbg_enabled = AL_FALSE;
		db->dbg_disabled = AL_FALSE;
		db->chip_revision = 0;
	}
}

/**************************************************************************************************/
/**************************************************************************************************/
void al_otp_storage_db_print(
	struct al_otp_storage_db	*db)
{
	unsigned int puk_idx;
	unsigned int i;

	al_print("- Device serial number:\n  - ");
	for (i = 0; i < AL_OTP_STORAGE_DEV_SER_NUM_SIZE; i++)
		al_print("%02x ", db->dev_ser_num[i]);
	al_print("\n");
	al_print("- Chip Revision: %u\n", db->chip_revision);
	al_print("- Device serial number lock: %s\n",
		db->dev_ser_num_is_locked ? "enabled" : "disabled");

	al_print("- Direct boot: %s\n", db->direct_boot_disabled ? "disabled" : "enabled");
	al_print("- Preload: %s\n", db->preload_disabled ? "disabled" : "enabled");
	al_print("- SPI debug: %s\n", db->spi_debug_disabled ? "disabled" : "enabled");
	al_print("- JTAG debug: %s\n", db->jtag_debug_disabled ? "disabled" : "enabled");
	al_print("- Secure boot: %s\n", db->secure_boot_enabled ? "enabled" : "disabled");
	al_print("- Boot rom debug: %s\n", db->boot_rom_debug_disabled ? "disabled" : "enabled");
	al_print("- Boot from SPI: %s\n", db->boot_from_spi_disabled ? "disabled" : "enabled");
	al_print("- Boot from NAND: %s\n", db->boot_from_nand_disabled ? "disabled" : "enabled");
	al_print("- Boot from UART: %s\n", db->boot_from_uart_disabled ? "disabled" : "enabled");
	al_print("- SW initiated memory repair: %s\n",
		db->sw_initiated_mem_repair ? "enabled" : "disabled");
	al_print("- Minimal SW version: %u\n", db->min_sw_ver);

	al_print("- TZ: %s\n", db->tz_enabled ? "enabled" : "disabled");
	al_print("- UART CLI: %s\n", db->uart_cli_disabled ? "disabled" : "enabled");
	al_print("- SoC JTAG: %s\n", db->jtag_soc_disabled ? "disabled" : "enabled");
	al_print("- SPI debugger PASW: %s\n", db->spi_dbg_pasw_enabled ? "enabled" : "disabled");
	al_print("- OTP shadow override by preloader: %s\n",
		db->otpr_override_by_pld_disabled ? "disabled" : "enabled");
	al_print("- CPU debug & trace: %s\n", db->cpu_dbg_trace_disabled ? "disabled" : "enabled");
	al_print("- Secure debug & trace: %s\n",
		db->secure_dbg_trace_disabled ? "disabled" : "enabled");
	al_print("- RMA Debug: %s\n",
		db->dbg_disabled ? "disabled and locked" :
		(db->dbg_enabled ? "enabled" : "disabled and not locked"));

	al_print("- Public keys:\n");
	al_print("  > Public key index: %u\n", db->puk_idx);
	al_print("  > Public key index lock: %s\n", db->puk_idx_is_locked ? "enabled" : "disabled");
	for (puk_idx = 0; puk_idx < AL_OTP_STORAGE_PUK_HASH_NUM; puk_idx++) {
		al_print("  > Public key %u hash:", puk_idx);
		for (i = 0; i < AL_OTP_STORAGE_PUK_HASH_SIZE; i++) {
			if (!(i % 16))
				al_print("\n  - ");
			al_print("%02x ", db->puk_hashes[puk_idx].puk_hash[i]);
		}
		al_print("\n");

		al_print("  > Public key %u hash lock: %s\n", puk_idx,
			db->puk_hashes[puk_idx].puk_hash_is_locked ? "enabled" : "disabled");
	}
}

/**************************************************************************************************/
/**************************************************************************************************/
static int al_otp_storage_flag_set(
	const char				*func_name,
	const struct al_otp_storage_handle	*handle,
	uint32_t				password,
	unsigned int				word_idx,
	unsigned int				bit)
{
	unsigned int bit_mask = AL_BIT(bit);
	uint32_t w;
	int err;

	if (password != AL_OTP_STORAGE_PASSWORD) {
		al_err("%s: wrong password!\n", func_name);
		return -EINVAL;
	}

	if (al_otp_word_is_locked(&handle->otp_handle, word_idx)) {
		al_err("%s: word %u locked!\n", func_name, word_idx);
		return -EINVAL;
	}

	w = al_otp_read_word_direct(&handle->otp_handle, word_idx);
	if (w & bit_mask) {
		al_err("%s: word %u bit %u already set!\n", func_name, word_idx, bit);
		return -EINVAL;
	}

	al_otp_write_enable(&handle->otp_handle);

	w |= bit_mask;
	err = al_otp_write_word(&handle->otp_handle, word_idx, w);
	if (err)
		al_err("%s: al_otp_write_word(%u, %08x) failed!\n", func_name, word_idx, w);

	al_otp_write_disable(&handle->otp_handle);

	return err;
}

/**************************************************************************************************/
/**************************************************************************************************/
static int al_otp_storage_word_arr_set(
	const char				*func_name,
	const struct al_otp_storage_handle	*handle,
	uint32_t				password,
	unsigned int				word_idx_first,
	unsigned int				num_words,
	uint32_t				*word_arr)
{
	unsigned int word_idx_last = word_idx_first + num_words - 1;
	unsigned int word_idx;
	unsigned int i;
	uint32_t w;
	int err;

	if (word_idx_last < word_idx_first) {
		al_assert(0);
		return -EINVAL;
	}

	if (password != AL_OTP_STORAGE_PASSWORD) {
		al_err("%s: wrong password!\n", func_name);
		return -EINVAL;
	}

	for (word_idx = word_idx_first; word_idx <= word_idx_last; word_idx++) {
		if (al_otp_word_is_locked(&handle->otp_handle, word_idx)) {
			al_err("%s: word %u locked!\n", func_name, word_idx);
			return -EINVAL;
		}

		w = al_otp_read_word_direct(&handle->otp_handle, word_idx);
		if (w) {
			al_err("%s: word %u is non zero!\n", func_name, word_idx);
			return -EINVAL;
		}
	}

	al_otp_write_enable(&handle->otp_handle);

	for (i = 0, word_idx = word_idx_first; word_idx <= word_idx_last; i++, word_idx++) {
		w = word_arr[i];
		err = al_otp_write_word(&handle->otp_handle, word_idx, w);
		if (err) {
			al_err("%s: al_otp_write_word(%u, %08x) failed!\n", func_name, word_idx, w);
			break;
		}
	}

	al_otp_write_disable(&handle->otp_handle);

	return err;
}

/**************************************************************************************************/
/**************************************************************************************************/
static int al_otp_storage_word_arr_lock(
	const char				*func_name,
	const struct al_otp_storage_handle	*handle,
	uint32_t				password,
	unsigned int				word_idx_first,
	unsigned int				num_words)
{
	unsigned int word_idx_last = word_idx_first + num_words - 1;
	unsigned int word_idx;
	int err;

	if (word_idx_last < word_idx_first) {
		al_assert(0);
		return -EINVAL;
	}

	if (password != AL_OTP_STORAGE_PASSWORD) {
		al_err("%s: wrong password!\n", func_name);
		return -EINVAL;
	}

	for (word_idx = word_idx_first; word_idx <= word_idx_last; word_idx++) {
		if (al_otp_word_is_locked(&handle->otp_handle, word_idx)) {
			al_err("%s: word %u locked!\n", func_name, word_idx);
			return -EINVAL;
		}
	}

	al_otp_write_enable(&handle->otp_handle);

	for (word_idx = word_idx_first; word_idx <= word_idx_last; word_idx++) {
		err = al_otp_lock_word(&handle->otp_handle, word_idx);
		if (err) {
			al_err("%s: al_otp_lock_word(%u) failed!\n", func_name, word_idx);
			break;
		}
	}

	al_otp_write_disable(&handle->otp_handle);

	return err;
}

/**************************************************************************************************/
/**************************************************************************************************/
#define WORD_ARR_MAX_SIZE	8

static int al_otp_storage_byte_arr_set(
	const char				*func_name,
	const struct al_otp_storage_handle	*handle,
	uint32_t				password,
	unsigned int				word_idx_first,
	unsigned int				num_bytes,
	uint8_t					*byte_arr)
{
	unsigned int num_words = num_bytes / sizeof(uint32_t);
	uint32_t word_arr[WORD_ARR_MAX_SIZE];

	al_assert(!(num_bytes % sizeof(uint32_t)));
	al_assert(num_words <= WORD_ARR_MAX_SIZE);
	al_assert(OTP_FIELD_WORD_SIZE_PUK0_MOD_HASH <= WORD_ARR_MAX_SIZE);

	al_memcpy(word_arr, byte_arr, num_bytes);

	return al_otp_storage_word_arr_set(
		func_name, handle, password, word_idx_first, num_words, word_arr);
}

/**************************************************************************************************/
/**************************************************************************************************/
int al_otp_storage_dev_ser_num_set(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password,
	uint8_t					dev_ser_num[AL_OTP_STORAGE_DEV_SER_NUM_SIZE])
{
	return al_otp_storage_byte_arr_set(__func__, handle, password,
		OTP_FIELD_WORD_OFFSET_DEVICE_SER_NUM, AL_OTP_STORAGE_DEV_SER_NUM_SIZE,
		dev_ser_num);
}

/**************************************************************************************************/
/**************************************************************************************************/
int al_otp_storage_dev_ser_num_lock(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password)
{
	return al_otp_storage_word_arr_lock(__func__, handle, password,
		OTP_FIELD_WORD_OFFSET_DEVICE_SER_NUM,
		OTP_FIELD_WORD_SIZE_DEVICE_SER_NUM);
}

/**************************************************************************************************/
/**************************************************************************************************/
int al_otp_storage_direct_boot_disable(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password)
{
	return al_otp_storage_flag_set(__func__, handle, password,
		OTP_FIELD_WORD_OFFSET_DIRECT_BOOT_DIS,
		OTP_FIELD_BIT_OFFSET_DIRECT_BOOT_DIS);
}

/**************************************************************************************************/
/**************************************************************************************************/
int al_otp_storage_preload_disable(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password)
{
	return al_otp_storage_flag_set(__func__, handle, password,
		OTP_FIELD_WORD_OFFSET_PRELOAD_DISABLE,
		OTP_FIELD_BIT_OFFSET_PRELOAD_DISABLE);
}

/**************************************************************************************************/
/**************************************************************************************************/
int al_otp_storage_spi_debug_disable(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password)
{
	return al_otp_storage_flag_set(__func__, handle, password,
		OTP_FIELD_WORD_OFFSET_SPI_DBG_DISABLE,
		OTP_FIELD_BIT_OFFSET_SPI_DBG_DISABLE);
}

/**************************************************************************************************/
/**************************************************************************************************/
int al_otp_storage_jtag_debug_disable(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password)
{
	return al_otp_storage_flag_set(__func__, handle, password,
		OTP_FIELD_WORD_OFFSET_JTAG_DBG_DISABLE,
		OTP_FIELD_BIT_OFFSET_JTAG_DBG_DISABLE);
}

/**************************************************************************************************/
/**************************************************************************************************/
int al_otp_storage_secure_boot_enable(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password)
{
	return al_otp_storage_flag_set(__func__, handle, password,
		OTP_FIELD_WORD_OFFSET_SECURE_BOOT_EN,
		OTP_FIELD_BIT_OFFSET_SECURE_BOOT_EN);
}

/**************************************************************************************************/
/**************************************************************************************************/
int al_otp_storage_boot_rom_debug_disable(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password)
{
	return al_otp_storage_flag_set(__func__, handle, password,
		OTP_FIELD_WORD_OFFSET_ROM_DBG_DIS,
		OTP_FIELD_BIT_OFFSET_ROM_DBG_DIS);
}

/**************************************************************************************************/
/**************************************************************************************************/
int al_otp_storage_boot_from_spi_disable(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password)
{
	return al_otp_storage_flag_set(__func__, handle, password,
		OTP_FIELD_WORD_OFFSET_SNOR_SECURE_BOOT_DIS,
		OTP_FIELD_BIT_OFFSET_SNOR_SECURE_BOOT_DIS);
}

/**************************************************************************************************/
/**************************************************************************************************/
int al_otp_storage_boot_from_nand_disable(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password)
{
	return al_otp_storage_flag_set(__func__, handle, password,
		OTP_FIELD_WORD_OFFSET_NAND_SECURE_BOOT_DIS,
		OTP_FIELD_BIT_OFFSET_NAND_SECURE_BOOT_DIS);
}

/**************************************************************************************************/
/**************************************************************************************************/
int al_otp_storage_boot_from_uart_disable(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password)
{
	return al_otp_storage_flag_set(__func__, handle, password,
		OTP_FIELD_WORD_OFFSET_UART_SECURE_BOOT_DIS,
		OTP_FIELD_BIT_OFFSET_UART_SECURE_BOOT_DIS);
}

/**************************************************************************************************/
/**************************************************************************************************/
int al_otp_storage_puk_hash_set(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password,
	uint8_t					puk_hash[AL_OTP_STORAGE_PUK_HASH_SIZE])
{
	return al_otp_storage_puk_hash_adv_set(handle, password, 0, puk_hash);
}

/**************************************************************************************************/
/**************************************************************************************************/
int al_otp_storage_puk_hash_lock(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password)
{
	return al_otp_storage_puk_hash_adv_lock(handle, password, 0);
}

/**************************************************************************************************/
/**************************************************************************************************/
int al_otp_storage_min_sw_ver_set(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password,
	unsigned int				min_sw_ver)
{
	unsigned int word_idx = OTP_FIELD_WORD_OFFSET_MINIMAL_SW_VER;
	unsigned int min_sw_ver_cur = 0;
	unsigned int i;
	uint32_t w;
	int err;

	if (min_sw_ver > WORD_NUM_BITS) {
		al_err("%s: maximal supported minimal SW version is %u!\n",
			__func__, WORD_NUM_BITS);
		return -EINVAL;
	}

	if (password != AL_OTP_STORAGE_PASSWORD) {
		al_err("%s: wrong password!\n", __func__);
		return -EINVAL;
	}

	if (al_otp_word_is_locked(&handle->otp_handle, word_idx)) {
		al_err("%s: word %u locked!\n", __func__, word_idx);
		return -EINVAL;
	}

	w = al_otp_read_word_direct(&handle->otp_handle, word_idx);
	for (i = 0; i < WORD_NUM_BITS; i++, w >>= 1) {
		if ((w & 1))
			min_sw_ver_cur = i + 1;
	}
	if (min_sw_ver <= min_sw_ver_cur) {
		al_err("%s: Minimal SW version <= current setting (%u <= %u)!\n",
			__func__, min_sw_ver, min_sw_ver_cur);
		return -EINVAL;
	}

	al_otp_write_enable(&handle->otp_handle);

	w = (1ULL << min_sw_ver) - 1;
	err = al_otp_write_word(&handle->otp_handle, word_idx, w);
	if (err)
		al_err("%s: al_otp_write_word(%u, %08x) failed!\n", __func__, word_idx, w);

	al_otp_write_disable(&handle->otp_handle);

	return err;
}

/* Alpine V3 onwards functions */

/**************************************************************************************************/
/**************************************************************************************************/
int al_otp_storage_puk_hash_adv_set(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password,
	unsigned int				puk_idx,
	uint8_t					puk_hash[AL_OTP_STORAGE_PUK_HASH_SIZE])
{
	unsigned int word_offset;
	unsigned int word_size;
	int err;

	err = al_otp_storage_puk_layout_get(handle, puk_idx, &word_offset, &word_size);
	if (err)
		return err;

	return al_otp_storage_byte_arr_set(
		__func__, handle, password, word_offset, WORD_NUM_BYTES * word_size, puk_hash);
}

/**************************************************************************************************/
/**************************************************************************************************/
int al_otp_storage_puk_hash_adv_lock(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password,
	unsigned int				puk_idx)
{
	unsigned int word_offset;
	unsigned int word_size;
	int err;

	err = al_otp_storage_puk_layout_get(handle, puk_idx, &word_offset, &word_size);
	if (err)
		return err;

	return al_otp_storage_word_arr_lock(__func__, handle, password, word_offset, word_size);
}

/**************************************************************************************************/
/**************************************************************************************************/
int al_otp_storage_puk_hash_idx_set(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password,
	unsigned int				puk_idx)
{
	unsigned int word_idx = OTP_FIELD_WORD_OFFSET_PUK_IDX;
	unsigned int puk_idx_cur = 0;
	unsigned int num_puks;
	unsigned int i;
	uint32_t w;
	int err;

	num_puks = al_otp_storage_num_puks_get(handle);
	if (puk_idx >= num_puks) {
		al_err("%s: maximal supported PUK index is %u!\n",
			__func__, num_puks - 1);
		return -EINVAL;
	}

	if (password != AL_OTP_STORAGE_PASSWORD) {
		al_err("%s: wrong password!\n", __func__);
		return -EINVAL;
	}

	if (al_otp_word_is_locked(&handle->otp_handle, word_idx)) {
		al_err("%s: word %u locked!\n", __func__, word_idx);
		return -EINVAL;
	}

	w = al_otp_read_word_direct(&handle->otp_handle, word_idx);
	for (i = 0; i < (num_puks - 1); i++, w >>= 1) {
		if ((w & 1))
			puk_idx_cur = i + 1;
	}
	if (puk_idx <= puk_idx_cur) {
		al_err("%s: PUK index <= current setting (%u <= %u)!\n",
			__func__, puk_idx, puk_idx_cur);
		return -EINVAL;
	}

	al_otp_write_enable(&handle->otp_handle);

	w = al_otp_read_word_direct(&handle->otp_handle, word_idx);
	w |= ((1ULL << puk_idx) - 1) << OTP_FIELD_BIT_OFFSET_PUK_IDX;
	err = al_otp_write_word(&handle->otp_handle, word_idx, w);
	if (err)
		al_err("%s: al_otp_write_word(%u, %08x) failed!\n", __func__, word_idx, w);

	al_otp_write_disable(&handle->otp_handle);

	return err;
}

/**************************************************************************************************/
/**************************************************************************************************/
int al_otp_storage_puk_hash_idx_lock(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password)
{
	if (handle->dev_id < PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V3)
		return -EINVAL;

	return al_otp_storage_word_arr_lock(__func__, handle, password,
		OTP_FIELD_WORD_OFFSET_PUK_IDX, 1);
}

/**************************************************************************************************/
/**************************************************************************************************/
int al_otp_storage_uart_cli_disable(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password)
{
	return al_otp_storage_flag_set(__func__, handle, password,
		OTP_FIELD_WORD_OFFSET_UART_CLI_SECURE_BOOT_DIS,
		OTP_FIELD_BIT_OFFSET_UART_CLI_SECURE_BOOT_DIS);
}

/**************************************************************************************************/
/**************************************************************************************************/
int al_otp_storage_tz_enable(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password)
{
	return al_otp_storage_flag_set(__func__, handle, password,
		OTP_FIELD_WORD_OFFSET_TZ_EN,
		OTP_FIELD_BIT_OFFSET_TZ_EN);
}

/**************************************************************************************************/
/**************************************************************************************************/
int al_otp_storage_jtag_soc_disable(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password)
{
	return al_otp_storage_flag_set(__func__, handle, password,
		OTP_FIELD_WORD_OFFSET_JTAG_SOC_DISABLE,
		OTP_FIELD_BIT_OFFSET_JTAG_SOC_DISABLE);
}

/**************************************************************************************************/
/**************************************************************************************************/
int al_otp_storage_spi_dbg_pasw_enable(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password)
{
	return al_otp_storage_flag_set(__func__, handle, password,
		OTP_FIELD_WORD_OFFSET_SPI_DBG_PASW_EN,
		OTP_FIELD_BIT_OFFSET_SPI_DBG_PASW_EN);
}

/**************************************************************************************************/
/**************************************************************************************************/
int al_otp_storage_otpr_override_by_pld_disable(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password)
{
	return al_otp_storage_flag_set(__func__, handle, password,
		OTP_FIELD_WORD_OFFSET_OTPR_OVERRIDE_BY_PLD_DIS,
		OTP_FIELD_BIT_OFFSET_OTPR_OVERRIDE_BY_PLD_DIS);
}

/**************************************************************************************************/
/**************************************************************************************************/
int al_otp_storage_cpu_dbg_trace_disable(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password)
{
	return al_otp_storage_flag_set(__func__, handle, password,
		OTP_FIELD_WORD_OFFSET_CPU_DBG_AND_TRACE_DIS,
		OTP_FIELD_BIT_OFFSET_CPU_DBG_AND_TRACE_DIS);
}

/**************************************************************************************************/
/**************************************************************************************************/
int al_otp_storage_secure_dbg_trace_disable(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password)
{
	return al_otp_storage_flag_set(__func__, handle, password,
		OTP_FIELD_WORD_OFFSET_SECURE_DBG_AND_TRACE_DIS,
		OTP_FIELD_BIT_OFFSET_SECURE_DBG_AND_TRACE_DIS);
}

/**************************************************************************************************/
/**************************************************************************************************/
int al_otp_storage_dbg_enable(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password)
{
	return al_otp_storage_flag_set(__func__, handle, password,
		OTP_FIELD_WORD_OFFSET_DBG_EN,
		OTP_FIELD_BIT_OFFSET_DBG_EN_EN);
}

/**************************************************************************************************/
/**************************************************************************************************/
int al_otp_storage_dbg_disable(
	const struct al_otp_storage_handle	*handle,
	uint32_t				password)
{
	return al_otp_storage_flag_set(__func__, handle, password,
		OTP_FIELD_WORD_OFFSET_DBG_EN,
		OTP_FIELD_BIT_OFFSET_DBG_EN_DIS);
}

