/*******************************************************************************
Copyright (C) 2016 Annapurna Labs Ltd.

This file may be licensed under the terms of the Annapurna Labs Commercial
License Agreement.

Alternatively, this file can be distributed under the terms of the GNU General
Public License V2 as published by the Free Software Foundation and can be
found at http://www.gnu.org/licenses/gpl-2.0.html

Alternatively, redistribution and use in source and binary forms, with or
without modification, are permitted provided that the following conditions are
met:

    *     Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

    *     Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in
the documentation and/or other materials provided with the
distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/
#include "al_nand_early.h"
#include "al_hal_nand.h"
#include "al_hal_nand_defs.h"
#include "al_hal_muio_mux.h"
#include "al_hal_iomap.h"

#define PG_READ_SEQ_MAX_SIZE		32
#define PG_WRITE_SEQ_MAX_SIZE		32
#define BB_MAP_MAX_SIZE			sizeof(uint32_t)

#define ALIGN_DOWN_2_POWER(addr, size)	((addr)&(~((size)-1)))

#define NAND_DEVICE_CMD_RESET		0xFF
#define NAND_DEVICE_CMD_READ_ONFI	0xEC
#define NAND_DEVICE_READ_ONFI_SIZE	256

#define ONFI_PP_MAX_NUM_COPIES		16

#define ONFI_FEATURES_EPP_EXIST		AL_BIT(7)
#define ONFI_ECC_BITS_EXT_REQUIRED	255

#define ONFI_EPP_BLK_SZ			16
#define ONFI_EPP_ECC_SECT_ID		2
#define ONFI_EPP_ECC_WRD_IDX_NUM_BITS	0
#define ONFI_EPP_ECC_WRD_IDX_CW_SIZE	1

#define NAND_DEVICE_CMD_BLOCK_ERASE1		0x60
#define NAND_DEVICE_CMD_BLOCK_ERASE2		0xD0

#define WAIT_TIMEOUT_USEC		1000000

struct al_nand_onfi_params {
	/* rev info and features block */
	/* 'O' 'N' 'F' 'I'  */
	uint8_t sig[4];
	uint16_t revision;
	uint16_t features;
	uint16_t opt_cmd;
	uint8_t reserved0[2];
	uint16_t epp_size;
	uint8_t num_pp;
	uint8_t reserved1[17];

	/* manufacturer information block */
	char manufacturer[12];
	char model[20];
	uint8_t jedec_id;
	uint16_t date_code;
	uint8_t reserved2[13];

	/* memory organization block */
	uint32_t byte_per_page;
	uint16_t spare_bytes_per_page;
	uint32_t data_bytes_per_ppage;
	uint16_t spare_bytes_per_ppage;
	uint32_t pages_per_block;
	uint32_t blocks_per_lun;
	uint8_t lun_count;
	uint8_t addr_cycles;
	uint8_t bits_per_cell;
	uint16_t bb_per_lun;
	uint16_t block_endurance;
	uint8_t guaranteed_good_blocks;
	uint16_t guaranteed_block_endurance;
	uint8_t programs_per_page;
	uint8_t ppage_attr;
	uint8_t ecc_bits;
	uint8_t interleaved_bits;
	uint8_t interleaved_ops;
	uint8_t reserved3[13];

	/* electrical parameter block */
	uint8_t io_pin_capacitance_max;
	uint16_t async_timing_mode;
	uint16_t program_cache_timing_mode;
	uint16_t t_prog;
	uint16_t t_bers;
	uint16_t t_r;
	uint16_t t_ccs;
	uint16_t src_sync_timing_mode;
	uint16_t src_ssync_features;
	uint16_t clk_pin_capacitance_typ;
	uint16_t io_pin_capacitance_typ;
	uint16_t input_pin_capacitance_typ;
	uint8_t input_pin_capacitance_max;
	uint8_t driver_strenght_support;
	uint16_t t_int_r;
	uint16_t t_ald;
	uint8_t reserved4[7];

	/* vendor */
	uint8_t reserved5[90];

	uint16_t crc;
} __packed;

struct al_nand_onfi_epp {
	uint16_t crc;
	/* signature */
	/* 'E' 'P' 'P' 'S'  */
	uint8_t sig[4];
	uint8_t reserved[10];
	struct sec_len_type {
		uint8_t type;
		uint8_t len;
	} sec_len_type[8];
	uint8_t sec_info[32];
} __packed;

struct al_nand_early_bb_map {
	uint32_t	is_bad_map;
	uint32_t	is_known_map;
};

static struct nand_db {
	struct al_nand_ctrl_obj			obj;
	struct al_nand_extra_dev_properties	dev_ext_props;
	struct al_nand_ecc_config		ecc_config;
	struct al_nand_early_bb_map		bb_map;
} nand_db;

static int al_nand_early_spare_read(
	unsigned int	pg_addr,
	unsigned int	spare_addr,
	uint8_t	*buff,
	unsigned int	num_bytes);

static int al_nand_early_is_blk_bad_raw_within_pg_wrd(
	unsigned int	pg_addr,
	unsigned int	spare_wrd_idx,
	int		*bad);

static int al_nand_early_is_blk_bad_raw_within_pg(
	unsigned int	pg_addr,
	int		*bad);

static int al_nand_early_is_blk_bad_raw(
	unsigned int	blk_addr,
	int		*bad);

static int al_nand_early_is_block_bad(
	unsigned int	blk_addr,
	int		*bad);

static int al_nand_early_addr_logic_to_phys(
	unsigned int	logic_addr,
	unsigned int	*phys_addr);

static int al_nand_early_wait_cmd_fifo_empty(void);

static int al_nand_early_wait_for_ready(void);

#if defined(AL_NAND_MUX_NAND8_CS0) && (AL_NAND_MUX_NAND8_CS0 == 1)
static int al_nand_early_init_mux_nand8_cs0(void)
{
	struct al_muio_mux_obj obj;
	int err;
	static const struct al_muio_mux_if_and_arg ifaces[] = {
		{ AL_MUIO_MUX_IF_NAND_8, 0 },
		{ AL_MUIO_MUX_IF_NAND_CS_0, 0 },
	};

	err = al_muio_mux_init((void __iomem *)AL_PBS_REGFILE_BASE, "muio_mux", &obj);
	if (err) {
		al_err("%s: al_muio_mux_init failed!\n", __func__);
		return err;
	}

	err = al_muio_mux_iface_alloc_multi(&obj, ifaces, AL_ARR_SIZE(ifaces));
	if (err) {
		al_err("%s: al_muio_mux_iface_alloc_multi failed!\n", __func__);
		return err;
	}

	return 0;
}
#else
static inline int al_nand_early_init_mux_nand8_cs0(void)
{
	return 0;
}
#endif

#if defined(AL_NAND_DEV_RESET) && (AL_NAND_DEV_RESET == 1)
#define WAIT_TIME_OUT_CYCLES		1000000
static int al_nand_early_init_dev_reset(
	struct al_nand_ctrl_obj			*obj)
{
	static const uint32_t cmd_seq_dev_rst[] = {
		AL_NAND_CMD_SEQ_ENTRY(AL_NAND_COMMAND_TYPE_CMD, NAND_DEVICE_CMD_RESET),
		AL_NAND_CMD_SEQ_ENTRY(AL_NAND_COMMAND_TYPE_WAIT_FOR_READY, 0) };
	unsigned int i;

	al_nand_cmd_seq_execute(obj, cmd_seq_dev_rst, AL_ARR_SIZE(cmd_seq_dev_rst));

	for (i = WAIT_TIME_OUT_CYCLES; i; i--) {
		if (al_nand_cmd_buff_is_empty(obj))
			break;

		al_udelay(1);
	}

	if (!i) {
		al_err("Timeed out waiting for cmd fifo to become empty!\n");
		return -ETIME;
	}

	return 0;
}
#else
static inline int al_nand_early_init_dev_reset(
	struct al_nand_ctrl_obj			*obj __attribute__((__unused__)))
{
	return 0;
}
#endif

static uint16_t al_nand_onfi_crc16(
	uint8_t *buff,
	unsigned int buff_size)
{
	uint16_t crc;
	int i;

	crc = 0x4f4e;
	for (; buff_size; buff++, buff_size--) {
		crc = crc ^ ((uint16_t)(*buff) << 8);
		for (i = 0; i < 8; i++)
			if (crc & 0x8000)
				crc = (crc << 1) ^ 0x8005;
			else
				crc = crc << 1;
	}

	return crc;
}

static int al_nand_early_init_onfi_params_read(
	struct al_nand_ctrl_obj		*obj,
	struct al_nand_onfi_params	*onfi_params)
{
	int err;
	uint32_t start_read_onfi_seq[] = {
		AL_NAND_CMD_SEQ_ENTRY(
			AL_NAND_COMMAND_TYPE_CMD,
			NAND_DEVICE_CMD_READ_ONFI),
		AL_NAND_CMD_SEQ_ENTRY(AL_NAND_COMMAND_TYPE_ADDRESS, 0)};
	uint32_t read_onfi_seq[] = {
		AL_NAND_CMD_SEQ_ENTRY(
			AL_NAND_COMMAND_TYPE_DATA_READ_COUNT,
			(NAND_DEVICE_READ_ONFI_SIZE & 0xff)),
		AL_NAND_CMD_SEQ_ENTRY(
			AL_NAND_COMMAND_TYPE_DATA_READ_COUNT,
			(NAND_DEVICE_READ_ONFI_SIZE & 0xff00) >> 8)};
	uint8_t *sig = onfi_params->sig;
	uint16_t crc_actual;
	unsigned int copy_num = 0;

	al_nand_cmd_seq_execute(obj, start_read_onfi_seq,
		sizeof(start_read_onfi_seq) / sizeof(start_read_onfi_seq[0]));

	for (copy_num = 0; copy_num < ONFI_PP_MAX_NUM_COPIES; copy_num++) {
		al_nand_cw_config(
			obj,
			NAND_DEVICE_READ_ONFI_SIZE,
			1);

		al_nand_cmd_seq_execute(obj, read_onfi_seq,
			sizeof(read_onfi_seq) / sizeof(read_onfi_seq[0]));

		err = al_nand_data_buff_read(
			obj, NAND_DEVICE_READ_ONFI_SIZE, 0, 0, (uint8_t *)onfi_params);
		if (err) {
			al_err("Unable to read ONFI param page (Copy #%u)!\n", copy_num);
			continue;
		}

		if ((sig[0] != 'O') || (sig[1] != 'N') || (sig[2] != 'F') || (sig[3] != 'I')) {
			al_err("Invalid ONFI signature (Copy #%u, %02x %02x %02x %02x)!\n",
				copy_num, sig[0], sig[1], sig[2], sig[3]);
			continue;
		}

		crc_actual = al_nand_onfi_crc16((uint8_t *)onfi_params,
			NAND_DEVICE_READ_ONFI_SIZE - 2);
		if (crc_actual != onfi_params->crc) {
			al_err("Invalid ONFI CRC (Copy #%u, actual %04x, expected %04x)!\n",
				copy_num, crc_actual, onfi_params->crc);
			continue;
		}

		return 0;
	}

	return -EIO;
}

static int al_nand_early_init_onfi_epp_read(
	struct al_nand_ctrl_obj	*obj,
	struct al_nand_onfi_epp	*onfi_epp,
	unsigned int		offset,
	unsigned int		length)
{
	int err;
	uint32_t read_onfi_seq[] = {
		AL_NAND_CMD_SEQ_ENTRY(
			AL_NAND_COMMAND_TYPE_CMD,
			NAND_DEVICE_CMD_READ_ONFI),
		AL_NAND_CMD_SEQ_ENTRY(AL_NAND_COMMAND_TYPE_ADDRESS, 0),
		AL_NAND_CMD_SEQ_ENTRY(
			AL_NAND_COMMAND_TYPE_DATA_READ_COUNT,
			((offset + length) & 0xff)),
		AL_NAND_CMD_SEQ_ENTRY(
			AL_NAND_COMMAND_TYPE_DATA_READ_COUNT,
			((offset + length) & 0xff00) >> 8)};
	uint8_t *sig = onfi_epp->sig;
	uint16_t crc_actual;

	if (length > sizeof(struct al_nand_onfi_epp)) {
		al_err("Too big ONFI extended param page (offset %u, length %u)!\n",
			offset, length);
		return -EINVAL;
	}

	al_nand_cw_config(obj, (offset + length), 1);

	al_nand_cmd_seq_execute(obj, read_onfi_seq,
		sizeof(read_onfi_seq) / sizeof(read_onfi_seq[0]));

	err = al_nand_data_buff_read(obj, offset + length, offset, 0, (uint8_t *)onfi_epp);
	if (err) {
		al_err("Unable to read ONFI extended param page (offset %u, length %u)!\n",
			offset, length);
		return err;
	}

	if ((sig[0] != 'E') || (sig[1] != 'P') || (sig[2] != 'P') || (sig[3] != 'S')) {
		al_err("Invalid ONFI EPP signature (offset %u, length %u, %02x %02x %02x %02x)!\n",
			offset, length, sig[0], sig[1], sig[2], sig[3]);
		return -EIO;
	}

	crc_actual = al_nand_onfi_crc16((uint8_t *)onfi_epp + 2, length - 2);
	if (crc_actual != onfi_epp->crc) {
		al_err("Invalid ONFI EPP CRC (offset %u, length %u, actual %04x, expected %04x)!\n",
			offset, length, crc_actual, onfi_epp->crc);
		return -EIO;
	}

	return 0;
}

static int al_nand_early_init_onfi_params_apply(
	struct al_nand_ctrl_obj			*obj,
	struct al_nand_onfi_params		*onfi_params,
	struct al_nand_dev_properties		*dev_props,
	struct al_nand_ecc_config		*ecc_config,
	struct al_nand_extra_dev_properties	*dev_ext_props)
{
	/*******************************************************************************************
	 * dev_props
	 ******************************************************************************************/
	al_memset(dev_props, 0, sizeof(struct al_nand_dev_properties));

	dev_props->timingMode = AL_NAND_DEVICE_TIMING_MODE_ONFI_0;
	dev_props->sdrDataWidth = AL_NAND_DEVICE_SDR_DATA_WIDTH_8;
	dev_props->num_col_cyc = ((onfi_params->addr_cycles & 0xf0) >> 4);
	dev_props->num_row_cyc = (onfi_params->addr_cycles & 0x0f);

	switch (onfi_params->byte_per_page) {
	case 2048:
		dev_props->pageSize = AL_NAND_DEVICE_PAGE_SIZE_2K;
		break;
	case 4096:
		dev_props->pageSize = AL_NAND_DEVICE_PAGE_SIZE_4K;
		break;
	case 8192:
		dev_props->pageSize = AL_NAND_DEVICE_PAGE_SIZE_8K;
		break;
	case 16384:
		dev_props->pageSize = AL_NAND_DEVICE_PAGE_SIZE_16K;
		break;
	default:
		al_err("Non supported page size (%u)!\n", onfi_params->byte_per_page);
		return -EINVAL;
	}

	/*******************************************************************************************
	 * ecc_config
	 ******************************************************************************************/
	al_memset(ecc_config, 0, sizeof(struct al_nand_ecc_config));

	if (onfi_params->ecc_bits == ONFI_ECC_BITS_EXT_REQUIRED) {
		struct al_nand_onfi_epp onfi_epp;
		unsigned int i;
		al_bool ecc_sect_exists = AL_FALSE;
		unsigned int ecc_sect_idx;
		unsigned int ecc_sect_offset = 0;
		int err;

		if (!(onfi_params->features & ONFI_FEATURES_EPP_EXIST)) {
			al_err("Extended ECC information needed, but ONFI EPP doesn't exist!\n");
			return -EINVAL;
		}

		err = al_nand_early_init_onfi_epp_read(obj, &onfi_epp,
			onfi_params->num_pp * NAND_DEVICE_READ_ONFI_SIZE,
			onfi_params->epp_size * ONFI_EPP_BLK_SZ);
		if (err) {
			al_err("al_nand_early_init_onfi_epp_read failed!\n");
			return err;
		}

		for (i = 0; i < AL_ARR_SIZE(onfi_epp.sec_len_type); i++) {
			if (onfi_epp.sec_len_type[i].type == ONFI_EPP_ECC_SECT_ID) {
				ecc_sect_exists = AL_TRUE;
				ecc_sect_idx = i;
				break;
			} else {
				ecc_sect_offset += onfi_epp.sec_len_type[i].len * ONFI_EPP_BLK_SZ;
			}
		}

		if (!ecc_sect_exists) {
			al_err("Extended ECC information needed, but ONFI EPP doesn't have it!\n");
			return -EINVAL;
		}

		if (!onfi_epp.sec_len_type[ecc_sect_idx].len) {
			al_err("Extended ECC information needed, but is of length 0!\n");
			return -EINVAL;
		}

		if (ecc_sect_offset >= sizeof(onfi_epp.sec_info)) {
			al_err("Extended ECC information needed, but exceeds!\n");
			return -EINVAL;
		}

		ecc_config->algorithm = AL_NAND_ECC_ALGORITHM_BCH;

		onfi_params->ecc_bits =
			onfi_epp.sec_info[ecc_sect_offset + ONFI_EPP_ECC_WRD_IDX_NUM_BITS];

		switch (onfi_epp.sec_info[ecc_sect_offset + ONFI_EPP_ECC_WRD_IDX_CW_SIZE]) {
		case 9:
			ecc_config->messageSize = AL_NAND_ECC_BCH_MESSAGE_SIZE_512;
			break;
		case 10:
			ecc_config->messageSize = AL_NAND_ECC_BCH_MESSAGE_SIZE_1024;
			break;
		default:
			al_err("Non supported code word size num bits (%u)!\n",
				onfi_epp.sec_info[ecc_sect_offset + ONFI_EPP_ECC_WRD_IDX_CW_SIZE]);
			return -EINVAL;
		}
	} else {
		ecc_config->algorithm = (onfi_params->ecc_bits == 1) ?
			AL_NAND_ECC_ALGORITHM_HAMMING :
			AL_NAND_ECC_ALGORITHM_BCH;

		if (ecc_config->algorithm == AL_NAND_ECC_ALGORITHM_BCH)
			ecc_config->messageSize = AL_NAND_ECC_BCH_MESSAGE_SIZE_512;
	}

	if (ecc_config->algorithm == AL_NAND_ECC_ALGORITHM_BCH) {
		switch (onfi_params->ecc_bits) {
		case 4:
			ecc_config->num_corr_bits = AL_NAND_ECC_BCH_NUM_CORR_BITS_4;
			break;
		case 8:
			ecc_config->num_corr_bits = AL_NAND_ECC_BCH_NUM_CORR_BITS_8;
			break;
		case 12:
			ecc_config->num_corr_bits = AL_NAND_ECC_BCH_NUM_CORR_BITS_12;
			break;
		case 16:
			ecc_config->num_corr_bits = AL_NAND_ECC_BCH_NUM_CORR_BITS_16;
			break;
		case 20:
			ecc_config->num_corr_bits = AL_NAND_ECC_BCH_NUM_CORR_BITS_20;
			break;
		case 24:
			ecc_config->num_corr_bits = AL_NAND_ECC_BCH_NUM_CORR_BITS_24;
			break;
		case 28:
			ecc_config->num_corr_bits = AL_NAND_ECC_BCH_NUM_CORR_BITS_28;
			break;
		case 32:
			ecc_config->num_corr_bits = AL_NAND_ECC_BCH_NUM_CORR_BITS_32;
			break;
		case 36:
			ecc_config->num_corr_bits = AL_NAND_ECC_BCH_NUM_CORR_BITS_36;
			break;
		case 40:
			ecc_config->num_corr_bits = AL_NAND_ECC_BCH_NUM_CORR_BITS_40;
			break;
		default:
			al_err("Non supported num correctable bits (%u)!\n",
				onfi_params->ecc_bits);
			return -EINVAL;
		}
	}

	ecc_config->spareAreaOffset = onfi_params->byte_per_page + 4;

	/*******************************************************************************************
	 * dev_ext_props
	 ******************************************************************************************/
	al_memset(dev_ext_props, 0, sizeof(struct al_nand_extra_dev_properties));

	dev_ext_props->pageSize = onfi_params->byte_per_page;
	dev_ext_props->blockSize = onfi_params->byte_per_page * onfi_params->pages_per_block;
	dev_ext_props->wordSize = 1;
	dev_ext_props->badBlockMarking.method = NAND_BAD_BLOCK_MARKING_CHECK_1ST_PAGE;
	dev_ext_props->badBlockMarking.location1 = 0;
	dev_ext_props->badBlockMarking.location2 = 0;
	dev_ext_props->eccIsEnabled = (onfi_params->ecc_bits > 0);

	al_info("------------------------------------------------------------------------------\n");
	al_info("NAND properties according to ONFI\n");
	al_info("------------------------------------------------------------------------------\n");
	al_info("- pageSize = %u\n", dev_ext_props->pageSize);
	al_info("- blockSize = %u\n", dev_ext_props->blockSize);
	al_info("- num_col_cyc = %u\n", dev_props->num_col_cyc);
	al_info("- num_row_cyc = %u\n", dev_props->num_row_cyc);
	al_info("- num_corr_bits = %u\n", onfi_params->ecc_bits);
	al_info("- code word size = %u\n",
		(ecc_config->messageSize == AL_NAND_ECC_BCH_MESSAGE_SIZE_512) ? 512 : 1024);
	al_info("------------------------------------------------------------------------------\n");

	return 0;
}

static int _al_nand_early_params_obtain(
	struct al_nand_ctrl_obj			*obj,
	struct al_nand_dev_properties		*dev_properties,
	struct al_nand_ecc_config		*ecc_config,
	struct al_nand_extra_dev_properties	*dev_ext_props,
	al_bool					*dev_was_selected_and_reset)
{
	int err;

	*dev_was_selected_and_reset = AL_FALSE;

	if (al_nand_properties_are_valid((void __iomem *)AL_PBS_REGFILE_BASE)) {
		err = al_nand_properties_decode(
			(void __iomem *)AL_PBS_REGFILE_BASE,
			dev_properties,
			ecc_config,
			dev_ext_props);
		if (err) {
			al_err("al_nand_properties_decode failed\n");
			return err;
		}
	} else {
		struct al_nand_onfi_params onfi_params;

		al_nand_dev_select(obj, 0);

		err = al_nand_early_init_dev_reset(obj);
		if (err) {
			al_err("al_nand_early_init_dev_reset failed\n");
			return err;
		}

		*dev_was_selected_and_reset = AL_TRUE;

		err = al_nand_early_init_onfi_params_read(obj, &onfi_params);
		if (err) {
			al_err("al_nand_early_init_onfi_params_read failed!\n");
			return err;
		}

		err = al_nand_early_init_onfi_params_apply(
			obj,
			&onfi_params,
			dev_properties,
			ecc_config,
			dev_ext_props);
		if (err) {
			al_err("al_nand_early_init_onfi_params_apply failed!\n");
			return err;
		}

		al_nand_properties_encode(
			(void __iomem *)AL_PBS_REGFILE_BASE,
			dev_properties,
			ecc_config,
			dev_ext_props);
	}

#ifdef AL_NAND_BBM_OVERRIDE
	dev_ext_props->badBlockMarking.method = NAND_BAD_BLOCK_MARKING_CHECK_1ST_PAGE;
#endif

	return 0;
}

int al_nand_early_params_obtain(
	struct al_nand_ctrl_obj			*obj,
	struct al_nand_dev_properties		*dev_properties,
	struct al_nand_ecc_config		*ecc_config,
	struct al_nand_extra_dev_properties	*dev_ext_props)
{
	al_bool dev_was_selected_and_reset;

	return _al_nand_early_params_obtain(obj, dev_properties, ecc_config, dev_ext_props,
		&dev_was_selected_and_reset);
}

int al_nand_early_init(void)
{
	al_bool dev_was_selected_and_reset;
	struct al_nand_dev_properties dev_props;
	int err;

	al_memset(&nand_db, 0, sizeof(struct nand_db));

	err = al_nand_early_init_mux_nand8_cs0();
	if (err) {
		al_err("al_nand_early_init_mux_nand8_cs0 failed\n");
		return err;
	}
	err = al_nand_init(&nand_db.obj, (void *)(uintptr_t)AL_NAND_BASE, NULL, 0);
	if (err) {
		al_err("nand init failed\n");
		return err;
	}

	err = _al_nand_early_params_obtain(
		&nand_db.obj, &dev_props, &nand_db.ecc_config, &nand_db.dev_ext_props,
		&dev_was_selected_and_reset);
	if (err) {
		al_err("al_nand_early_params_obtain failed\n");
		return err;
	}

	if (!dev_was_selected_and_reset) {
		al_nand_dev_select(&nand_db.obj, 0);

		err = al_nand_early_init_dev_reset(&nand_db.obj);
		if (err) {
			al_err("al_nand_early_init_dev_reset failed\n");
			return err;
		}
	}

	err = al_nand_dev_config(&nand_db.obj, &dev_props, &nand_db.ecc_config);
	if (err) {
		al_err("al_nand_dev_config failed\n");
		return err;
	}

	return 0;
}

int al_nand_early_read(
	unsigned int	address,
	void		*buff_ptr,
	unsigned int	num_bytes)
{
	int err = 0;
	uint8_t *buff = buff_ptr;
	uint32_t cmd_seq_pg_rd[PG_READ_SEQ_MAX_SIZE];
	uint32_t cw_size;
	uint32_t cw_count;
	int cmd_seq_pg_rd_num_entries;

	al_dbg("%s(%u, %u)\n", __func__, address, num_bytes);

	if (nand_db.dev_ext_props.eccIsEnabled) {
		al_nand_uncorr_err_clear(&nand_db.obj);
		al_nand_corr_err_clear(&nand_db.obj);
	}

	while (num_bytes) {
		unsigned int pg_base_addr_phys;
		int pg_base_addr = ALIGN_DOWN_2_POWER(address, nand_db.dev_ext_props.pageSize);
		int next_pg_base_addr = pg_base_addr + nand_db.dev_ext_props.pageSize;
		int num_bytes_skip_head = address - pg_base_addr;
		int num_bytes_curr =
			((num_bytes + num_bytes_skip_head) < nand_db.dev_ext_props.pageSize) ?
			(num_bytes) :
			(nand_db.dev_ext_props.pageSize - num_bytes_skip_head);
		int num_bytesSkipTail = next_pg_base_addr - (address + num_bytes_curr);

		err = al_nand_early_addr_logic_to_phys(
			pg_base_addr,
			&pg_base_addr_phys);
		if (err) {
			al_err("al_nand_early_addr_logic_to_phys failed!\n");
			return err;
		}

		al_nand_tx_set_enable(&nand_db.obj, 1);
		al_nand_tx_set_enable(&nand_db.obj, 0);

		cmd_seq_pg_rd_num_entries = PG_READ_SEQ_MAX_SIZE;

		if (nand_db.dev_ext_props.eccIsEnabled) {
			err = al_nand_cmd_seq_gen_page_read(
				&nand_db.obj,
				0,
				pg_base_addr_phys / nand_db.dev_ext_props.pageSize,
				nand_db.dev_ext_props.pageSize,
				1,
				cmd_seq_pg_rd,
				&cmd_seq_pg_rd_num_entries,
				&cw_size,
				&cw_count);
			if (err) {
				al_err("al_nand_cmd_seq_gen_page_read failed!\n");
				return err;
			}
		} else {
			int num_bytes;

			num_bytes = num_bytes_curr;

			err = al_nand_cmd_seq_gen_page_read(
				&nand_db.obj,
				num_bytes_skip_head,
				pg_base_addr_phys / nand_db.dev_ext_props.pageSize,
				num_bytes,
				0,
				cmd_seq_pg_rd,
				&cmd_seq_pg_rd_num_entries,
				&cw_size,
				&cw_count);
			if (err) {
				al_err("al_nand_cmd_seq_gen_page_read failed!\n");
				return err;
			}
		}

		al_nand_cw_config(&nand_db.obj, cw_size, cw_count);

		if (nand_db.dev_ext_props.eccIsEnabled)
			al_nand_ecc_set_enabled(&nand_db.obj, 1);

		al_nand_cmd_seq_execute(&nand_db.obj, cmd_seq_pg_rd, cmd_seq_pg_rd_num_entries);

		if (nand_db.dev_ext_props.eccIsEnabled) {
			err = al_nand_data_buff_read(
				&nand_db.obj,
				nand_db.dev_ext_props.pageSize,
				num_bytes_skip_head,
				num_bytesSkipTail,
				buff);
			if (err) {
				al_err("al_nand_data_buff_read failed!\n");
				return err;
			}
		} else {
			int num_bytes_skip_head;

			num_bytes_skip_head = 0;

			err = al_nand_data_buff_read(
				&nand_db.obj,
				num_bytes_curr + num_bytes_skip_head,
				num_bytes_skip_head,
				0,
				buff);
			if (err) {
				al_err("al_nand_data_buff_read failed!\n");
				return err;
			}
		}

		if (nand_db.dev_ext_props.eccIsEnabled)
			al_nand_ecc_set_enabled(&nand_db.obj, 0);

		num_bytes -= num_bytes_curr;
		address = next_pg_base_addr;
		buff += num_bytes_curr;
	}

	if (nand_db.dev_ext_props.eccIsEnabled) {
		if (al_nand_uncorr_err_get(&nand_db.obj)) {
			al_err("uncorrectable errors!\n");
			return -EIO;
		}

		if (al_nand_corr_err_get(&nand_db.obj))
			al_err("correctable errors!\n");
	}

	return 0;
}


int al_nand_early_write(
	unsigned int	address,
	void		*buff_ptr,
	unsigned int	num_bytes)
{
	int err = 0;
	uint8_t *buff = buff_ptr;
	uint32_t cmd_seq_pg_wr[PG_WRITE_SEQ_MAX_SIZE];
	uint32_t cw_size;
	uint32_t cw_count;
	int cmd_seq_pg_wr_num_entries;

	al_dbg("%s(%u, %u)\n", __func__, address, num_bytes);

	al_nand_tx_set_enable(&nand_db.obj, 1);
	al_nand_wp_set_enable(&nand_db.obj, 0);

	if (nand_db.dev_ext_props.eccIsEnabled)
		al_nand_uncorr_err_clear(&nand_db.obj);

	while (num_bytes) {
		unsigned int pg_base_addr_phys;
		int pg_base_addr = ALIGN_DOWN_2_POWER(address, nand_db.dev_ext_props.pageSize);
		int next_pg_base_addr = pg_base_addr + nand_db.dev_ext_props.pageSize;
		int num_bytes_skip_head = address - pg_base_addr;
		int num_bytes_curr =
			((num_bytes + num_bytes_skip_head) < nand_db.dev_ext_props.pageSize) ?
			(num_bytes) :
			(nand_db.dev_ext_props.pageSize - num_bytes_skip_head);
		int ecc_enable = 0;
		int num_of_bytes = num_bytes_curr;

		err = al_nand_early_addr_logic_to_phys(
				pg_base_addr,
				&pg_base_addr_phys);
		if (err) {
			al_err("al_nand_early_addr_logic_to_phys failed!\n");
			goto done;
		}

		cmd_seq_pg_wr_num_entries = PG_WRITE_SEQ_MAX_SIZE;


		if (nand_db.dev_ext_props.eccIsEnabled) {
			num_bytes_skip_head = 0;
			ecc_enable = 1;
			num_of_bytes = nand_db.dev_ext_props.pageSize;
		}

		err = al_nand_cmd_seq_gen_page_write(
				&nand_db.obj,
				num_bytes_skip_head,
				pg_base_addr_phys / nand_db.dev_ext_props.pageSize,
				num_of_bytes,
				ecc_enable,
				cmd_seq_pg_wr,
				&cmd_seq_pg_wr_num_entries,
				&cw_size,
				&cw_count);
		if (err) {
			al_err("al_nand_cmd_seq_gen_page_write failed!\n");
			goto done;
		}

		al_nand_cw_config(&nand_db.obj, cw_size, cw_count);

		if (nand_db.dev_ext_props.eccIsEnabled)
			al_nand_ecc_set_enabled(&nand_db.obj, 1);

		al_nand_cmd_seq_execute(&nand_db.obj,
				cmd_seq_pg_wr,
				cmd_seq_pg_wr_num_entries);

		err = al_nand_data_buff_write(&nand_db.obj, num_bytes_curr, buff);
		if (err) {
			al_err("al_nand_data_buff_write failed\n");
			goto done;
		}

		err = al_nand_early_wait_cmd_fifo_empty();
		if (err) {
			al_err("%s: al_nand_early_wait_cmd_fifo_empty failed!\n", __func__);
			goto done;
		}
		if (nand_db.dev_ext_props.eccIsEnabled)
			al_nand_ecc_set_enabled(&nand_db.obj, 0);

		num_bytes -= num_bytes_curr;
		address = next_pg_base_addr;
		buff += num_bytes_curr;
	}

done:
	al_nand_tx_set_enable(&nand_db.obj, 0);
	al_nand_wp_set_enable(&nand_db.obj, 1);

	return err;
}

int al_nand_early_erase_block(unsigned int block_off)
{
	struct al_nand_onfi_params onfi_params;
	unsigned int block_phy_address;
	unsigned int block_phy_number;
	uint32_t erase_seq[5];
	int ret;

	if (al_nand_early_init_onfi_params_read(&nand_db.obj, &onfi_params)) {
		al_err("al_nand_early_init_onfi_params_read failed!\n");
		return -EINVAL;
	}

	if (al_nand_early_addr_logic_to_phys(block_off, &block_phy_address)) {
		al_err("al_nand_early_addr_logic_to_phys failed!\n");
		return -EINVAL;
	}

	block_phy_number = block_phy_address /
			nand_db.dev_ext_props.pageSize /
			onfi_params.pages_per_block;

	block_phy_number *= onfi_params.pages_per_block;

	erase_seq[0] = AL_NAND_CMD_SEQ_ENTRY(
				AL_NAND_COMMAND_TYPE_CMD,
				NAND_DEVICE_CMD_BLOCK_ERASE1);
	erase_seq[1] = AL_NAND_CMD_SEQ_ENTRY(
				AL_NAND_COMMAND_TYPE_ADDRESS,
				(block_phy_number & 0xff));
	erase_seq[2] = AL_NAND_CMD_SEQ_ENTRY(
				AL_NAND_COMMAND_TYPE_ADDRESS,
				(block_phy_number & 0xff00) >> 8),
	erase_seq[3] = AL_NAND_CMD_SEQ_ENTRY(
				AL_NAND_COMMAND_TYPE_ADDRESS,
				(block_phy_number & 0xff0000) >> 16),
	erase_seq[4] = AL_NAND_CMD_SEQ_ENTRY(
				AL_NAND_COMMAND_TYPE_CMD,
				NAND_DEVICE_CMD_BLOCK_ERASE2);

	al_nand_cmd_seq_execute(&nand_db.obj, erase_seq, AL_ARR_SIZE(erase_seq));

	ret = al_nand_early_wait_cmd_fifo_empty();
	if (ret) {
		al_err("%s: al_nand_early_wait_cmd_fifo_empty failed!\n", __func__);
		return ret;
	}

	ret = al_nand_early_wait_for_ready();
	if (ret)
		al_err("%s: al_nand_early_wait_for_ready failed!\n", __func__);

	return ret;
}

struct nand_db *al_nand_early_get_obj(void)
{
	return &nand_db;
}

static int al_nand_early_spare_read(
	unsigned int	pg_addr,
	unsigned int	spare_addr,
	uint8_t		*buff,
	unsigned int	num_bytes)
{
	int err = 0;
	uint32_t cmd_seq_pg_rd[PG_READ_SEQ_MAX_SIZE];
	uint32_t cw_size;
	uint32_t cw_count;
	int cmd_seq_pg_rd_num_entries;

	al_dbg("al_nand_early_spare_read(%u, %u, %u)\n", pg_addr, spare_addr, num_bytes);

	cmd_seq_pg_rd_num_entries = PG_READ_SEQ_MAX_SIZE;

	err = al_nand_cmd_seq_gen_page_read(
		&nand_db.obj,
		nand_db.dev_ext_props.pageSize + spare_addr,
		pg_addr / nand_db.dev_ext_props.pageSize,
		num_bytes,
		0,
		cmd_seq_pg_rd,
		&cmd_seq_pg_rd_num_entries,
		&cw_size,
		&cw_count);
	if (err) {
		al_err("al_nand_cmd_seq_gen_page_read failed!\n");
		return err;
	}

	al_nand_cw_config(&nand_db.obj, cw_size, cw_count);
	al_nand_cmd_seq_execute(&nand_db.obj, cmd_seq_pg_rd, cmd_seq_pg_rd_num_entries);

	err = al_nand_data_buff_read(
		&nand_db.obj,
		num_bytes,
		0,
		0,
		buff);
	if (err) {
		al_err("al_nand_data_buff_read failed!\n");
		return err;
	}

	return 0;
}

static int al_nand_early_is_blk_bad_raw_within_pg_wrd(
	unsigned int	pg_addr,
	unsigned int	spare_wrd_idx,
	int		*bad)
{
	int err = 0;
	uint16_t word = 0xFFFF;

	err = al_nand_early_spare_read(pg_addr, spare_wrd_idx, (uint8_t *)&word, 1);
	if (err) {
		al_err("al_nand_early_spare_read failed\n");
		return err;
	}

	*bad = (word != 0xFFFF);

	return 0;
}

static int al_nand_early_is_blk_bad_raw_within_pg(
	unsigned int	pg_addr,
	int		*bad)
{
	int err = 0;
	struct al_nand_extra_dev_properties *dev_ext_props = &nand_db.dev_ext_props;
	struct al_nand_bad_block_marking *bbm = &dev_ext_props->badBlockMarking;
	int bad_local;

	err = al_nand_early_is_blk_bad_raw_within_pg_wrd(
			pg_addr,
			bbm->location1,
			&bad_local);
	if (err) {
		al_err("al_nand_early_is_blk_bad_raw_within_pg_wrd failed!\n");
		return err;
	}

	if ((0 == bad_local) && (bbm->location1 != bbm->location2)) {
		err = al_nand_early_is_blk_bad_raw_within_pg_wrd(
			pg_addr,
			bbm->location2,
			&bad_local);
		if (err) {
			al_err("al_nand_early_is_blk_bad_raw_within_pg_wrd failed!\n");
			return err;
		}
	}

	*bad = bad_local;

	return 0;
}

static int al_nand_early_is_blk_bad_raw(
	unsigned int	blk_addr,
	int		*bad)
{
	int err = 0;
	struct al_nand_extra_dev_properties *dev_ext_props = &nand_db.dev_ext_props;
	struct al_nand_bad_block_marking *bbm = &dev_ext_props->badBlockMarking;
	int num_pgs = dev_ext_props->blockSize / dev_ext_props->pageSize;
	int i;
	int pg_idx_arr[2] = { -1, -1 };

	*bad = 0;

	switch (bbm->method) {
	case NAND_BAD_BLOCK_MARKING_CHECK_1ST_PAGE:
		pg_idx_arr[0] = 0;
		break;

	case NAND_BAD_BLOCK_MARKING_CHECK_1ST_PAGES:
		pg_idx_arr[0] = 0;
		pg_idx_arr[1] = 1;
		break;

	case NAND_BAD_BLOCK_MARKING_CHECK_LAST_PAGE:
		pg_idx_arr[0] = num_pgs - 1;
		break;

	case NAND_BAD_BLOCK_MARKING_CHECK_LAST_PAGES:
		pg_idx_arr[0] = num_pgs - 1;
		pg_idx_arr[1] = num_pgs - 3;
		break;

	default:
		al_err("%s: invalid method (%d)\n", __func__, bbm->method);
		return -EINVAL;
	}

	for (i = 0; (i < 2) && (pg_idx_arr[i] != -1) && (0 == (*bad)); i++) {
		err = al_nand_early_is_blk_bad_raw_within_pg(
			blk_addr + pg_idx_arr[i] * dev_ext_props->pageSize,
			bad);
		if (err) {
			al_err("al_nand_early_is_blk_bad_raw_within_pg failed!\n");
			return err;
		}
	}

	return 0;
}

static int al_nand_early_is_block_bad(
	unsigned int	blk_addr,
	int		*bad)
{
	int err = 0;
	struct al_nand_early_bb_map *bb_map = &nand_db.bb_map;
	struct al_nand_extra_dev_properties *dev_ext_props = &nand_db.dev_ext_props;
	unsigned int blk_num = blk_addr / dev_ext_props->blockSize;

	if ((blk_num >= BB_MAP_MAX_SIZE) || (!(bb_map->is_known_map & AL_BIT(blk_num)))) {
		err = al_nand_early_is_blk_bad_raw(blk_addr, bad);
		if (err) {
			al_err("al_nand_early_is_blk_bad_raw failed!\n");
			return err;
		}

		if (blk_num < BB_MAP_MAX_SIZE) {
			bb_map->is_known_map |= AL_BIT(blk_num);
			if (*bad)
				bb_map->is_bad_map |= AL_BIT(blk_num);
			else
				bb_map->is_bad_map &= ~AL_BIT(blk_num);
		}
	} else {
		*bad = !!(bb_map->is_bad_map & AL_BIT(blk_num));
	}

	return 0;
}

static int al_nand_early_addr_logic_to_phys(
	unsigned int	logic_addr,
	unsigned int	*phys_addr)
{
	int err = 0;
	struct al_nand_extra_dev_properties *dev_ext_props = &nand_db.dev_ext_props;
	struct al_nand_bad_block_marking *bbm = &dev_ext_props->badBlockMarking;
	int num_bad_blks = 0;

	if (bbm->method != NAND_BAD_BLOCK_MARKING_METHOD_DISABLED) {
		int logic_blk_num = logic_addr / dev_ext_props->blockSize;
		int i;

		for (i = 0, num_bad_blks = 0; i <= (logic_blk_num + num_bad_blks); i++) {
			int is_bad;

			err = al_nand_early_is_block_bad(
				i * dev_ext_props->blockSize,
				&is_bad);
			if (err) {
				al_err("al_nand_early_is_block_bad failed!\n");
				return err;
			}

			if (is_bad)
				num_bad_blks++;
		}
	}

	*phys_addr = logic_addr + num_bad_blks * dev_ext_props->blockSize;

	if ((*phys_addr) != logic_addr)
		al_err("%s: %u --> %u\n", __func__, logic_addr, *phys_addr);

	return 0;
}

static int al_nand_early_wait_cmd_fifo_empty(void)
{
	int cmd_buff_empty;
	uint32_t i = WAIT_TIMEOUT_USEC;

	while (i) {
		cmd_buff_empty = al_nand_cmd_buff_is_empty(&nand_db.obj);
		if (cmd_buff_empty)
			break;

		al_udelay(1);
		i--;
	}

	if (!i) {
		al_err("Nand wait for empty cmd fifo - Timeout is over!\n");
		return -EINVAL;
	}

	return 0;
}

static int al_nand_early_wait_for_ready(void)
{
	int is_ready;
	uint32_t i = WAIT_TIMEOUT_USEC;

	while (i) {
		is_ready = al_nand_dev_is_ready(&nand_db.obj);
		if (is_ready)
			break;

		al_udelay(1);
		i--;
	}

	if (!i) {
		al_err("Nand wait for ready - Timeout is over!\n");
		return -EINVAL;
	}

	return 0;
}

