/*******************************************************************************
Copyright (C) 2015 Annapurna Labs Ltd.

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

#ifndef __FLASH_CONTENTS_H__
#define __FLASH_CONTENTS_H__

#include "al_hal_common.h"

/** Flash TOC magic number */
#define AL_FLASH_TOC_MAGIC_NUM			0x070c070c

/** Flash TOC minimal format revision ID supported */
#define AL_FLASH_TOC_FORMAT_REV_ID_MIN		0

/** Flash TOC maximal number of entries */
#define AL_FLASH_TOC_MAX_NUM_ENTRIES		64

/** Flash TOC entries max description length */
#define AL_FLASH_TOC_ENTRY_OBJ_ID_STR_LEN	8

/** Flash object magic number */
#define AL_FLASH_OBJ_MAGIC_NUM			0x000b9ec7

/** Flash object minimal format revision ID supported */
#define AL_FLASH_OBJ_FORMAT_REV_ID_MIN		0

/** Flash object description length */
#define AL_FLASH_OBJ_DESC_LEN			16

/******************************************************************************
 * Flash objects IDs
 ******************************************************************************/
/** Macro for getting flash object ID of specific instance */
#define AL_FLASH_OBJ_ID(obj_id, instance_num)	\
	((obj_id) | ((instance_num) << AL_FLASH_OBJ_ID_INSTANCE_NUM_SHIFT))
/** Macro for adding signature indication to flash object ID */
#define AL_FLASH_OBJ_ID_ADD_SIG(obj_id)	\
	((obj_id) | AL_FLASH_OBJ_ID_SIG_BIT)
/** Macro for removing signature indication from flash object ID */
#define AL_FLASH_OBJ_ID_REMOVE_SIG(obj_id)	\
	((obj_id) & (~AL_FLASH_OBJ_ID_SIG_BIT))
/** Macro for getting flash object actual ID (without instance) */
#define AL_FLASH_OBJ_ID_ID(obj_id)	\
	AL_REG_FIELD_GET(obj_id, AL_FLASH_OBJ_ID_ID_MASK,\
		AL_FLASH_OBJ_ID_ID_SHIFT)
/** Macro for getting flash object instance number */
#define AL_FLASH_OBJ_ID_INSTANCE_NUM(obj_id)	\
	AL_REG_FIELD_GET(obj_id, AL_FLASH_OBJ_ID_INSTANCE_NUM_MASK,\
		AL_FLASH_OBJ_ID_INSTANCE_NUM_SHIFT)
/** Macro for checking if the object ID is of a signature */
#define AL_FLASH_OBJ_ID_IS_SIG(obj_id)	\
	(!!((obj_id) & AL_FLASH_OBJ_ID_SIG_BIT))
#define AL_FLASH_OBJ_ID_ID_MASK			AL_FIELD_MASK(27, 0)
#define AL_FLASH_OBJ_ID_ID_SHIFT		0
#define AL_FLASH_OBJ_ID_SIG_BIT			AL_BIT(24)
#define AL_FLASH_OBJ_ID_INSTANCE_NUM_MASK	AL_FIELD_MASK(31, 28)
#define AL_FLASH_OBJ_ID_INSTANCE_NUM_SHIFT	28

#define AL_FLASH_OBJ_ID_BOOT_MODE		0x0
#define AL_FLASH_OBJ_ID_STG2			0x1
#define AL_FLASH_OBJ_ID_DT			0x2
#define AL_FLASH_OBJ_ID_STG2_5			0x3
#define AL_FLASH_OBJ_ID_STG3			0x4
#define AL_FLASH_OBJ_ID_UBOOT			0x5
#define AL_FLASH_OBJ_ID_UBOOT_SCRIPT		0x6
#define AL_FLASH_OBJ_ID_UBOOT_ENV		0x7
#define AL_FLASH_OBJ_ID_UBOOT_ENV_RED		0x8
#define AL_FLASH_OBJ_ID_KERNEL			0x9
#define AL_FLASH_OBJ_ID_ROOT_FS			0xa
#define AL_FLASH_OBJ_ID_2ND_TOC			0xb
#define AL_FLASH_OBJ_ID_PRE_BOOT		0xc
#define AL_FLASH_OBJ_ID_PRE_BOOT_V2		0xd
#define AL_FLASH_OBJ_ID_HW_PLD			0x10
#define AL_FLASH_OBJ_ID_ATF_BL31		0x11
#define AL_FLASH_OBJ_ID_ATF_BL32		0x12
#define AL_FLASH_OBJ_ID_APCEA			0x100
#define AL_FLASH_OBJ_ID_APCEA_CFG		0x101
#define AL_FLASH_OBJ_ID_IPXE			0x110
#define AL_FLASH_OBJ_ID_BOOT_APP		0x200
#define AL_FLASH_OBJ_ID_BOOT_APP_CFG		0x201
#define AL_FLASH_OBJ_ID_APP_0			0x210
#define AL_FLASH_OBJ_ID_APP_0_CFG		0x211
#define AL_FLASH_OBJ_ID_APP_1			0x220
#define AL_FLASH_OBJ_ID_APP_1_CFG		0x221
#define AL_FLASH_OBJ_ID_APP_2			0x230
#define AL_FLASH_OBJ_ID_APP_2_CFG		0x231
#define AL_FLASH_OBJ_ID_APP_3			0x240
#define AL_FLASH_OBJ_ID_APP_3_CFG		0x241
#define AL_FLASH_OBJ_ID_CRASH_DUMP		0x300
#define AL_FLASH_OBJ_ID_VPD			0x400
#define AL_FLASH_OBJ_ID_SERDES_25G_FW		0x500
#define AL_FLASH_OBJ_ID_SERDES_AVG_PCIE_FW	0x510
#define AL_FLASH_OBJ_ID_SERDES_AVG_NON_PCIE_FW	0x520
#define AL_FLASH_OBJ_ID_AVG_SBUS_MASTER_FW	0x530
#define AL_FLASH_OBJ_ID_PUK0			0x600
#define AL_FLASH_OBJ_ID_PUK1			0x601
#define AL_FLASH_OBJ_ID_PUK2			0x602
#define AL_FLASH_OBJ_ID_EEPROM			0x700
#define AL_FLASH_OBJ_ID_SPIS_PLD		0x701
#define AL_FLASH_OBJ_ID_USER_DEFINED_START	0x1000
#define AL_FLASH_OBJ_ID_USER_DEFINED_END	0x1fff

/******************************************************************************
 * Device IDs
 ******************************************************************************/
#define AL_FLASH_DEV_ID_CURRENT			0
#define AL_FLASH_DEV_ID_SPI			1
#define AL_FLASH_DEV_ID_NAND			2
#define AL_FLASH_DEV_ID_DRAM			3

#define AL_FLASH_PRE_BOOT_HEADER_OFFSET		0xa000
#define AL_FLASH_PRE_BOOT_STG2_5_OFFSET		0xb000
#define AL_FLASH_PRE_BOOT_STG3_OFFSET		0x11000

#define AL_FLASH_PRE_BOOT_V2_HEADER_OFFSET	0x20000
#define AL_FLASH_PRE_BOOT_V2_STG3_OFFSET	0x21000

/** Flash TOC entry */
struct al_flash_toc_entry {
	/** Object ID */
	uint32_t	obj_id;
	/** Object ID string */
	uint8_t		obj_id_str[AL_FLASH_TOC_ENTRY_OBJ_ID_STR_LEN];
	/** ID of the device on which the object resides */
	uint32_t	dev_id;
	/** Object offset in the device */
	uint32_t	offset;
	/** Object maximal size [bytes] */
	uint32_t	max_size;
	/** Object flags */
	uint32_t	flags;
	/** Reserved */
	uint32_t	reserved;
};

/** Flash TOC header */
struct al_flash_toc_hdr {
	/** TOC magic number */
	uint32_t	magic_num;
	/** TOC format revision ID */
	uint32_t	format_rev_id;
	/** TOC num entries */
	uint32_t	num_entries;
	/** Reserved */
	uint32_t	reserved;
	/** TOC header 32 bit checksum */
	uint32_t	checksum;
};

/** Flash TOC footer */
struct al_flash_toc_footer {
	/** TOC entries 32 bit checksum */
	uint32_t	checksum;
};

/** Flash object header */
struct al_flash_obj_hdr {
	/** Object magic number */
	uint32_t	magic_num;
	/** Object format revision ID */
	uint32_t	format_rev_id;
	/** Object ID */
	uint32_t	id;
	/** Object major version */
	uint32_t	major_ver;
	/** Object minor version */
	uint32_t	minor_ver;
	/** Object fix version */
	uint32_t	fix_ver;
	/** Object description */
	uint8_t		desc[AL_FLASH_OBJ_DESC_LEN];
	/** Object size */
	uint32_t	size;
	/** Object loading address - high 32 bits */
	uint32_t	load_addr_hi;
	/** Object loading address - low 32 bits */
	uint32_t	load_addr_lo;
	/** Object execution address - high 32 bits */
	uint32_t	exec_addr_hi;
	/** Object execution address - low 32 bits */
	uint32_t	exec_addr_lo;
	/** Object flags */
	uint32_t	flags;
	/** Reserved */
	uint32_t	reserved;
	/** Object header 32 bit checksum */
	uint32_t	checksum;
};

/** Flash object footer */
struct al_flash_obj_footer {
	/** Object data 32 bit checksum */
	uint32_t	data_checksum;
};

/** Flash stage2 header */
struct al_flash_stage2_hdr {
	/** Reserved */
	uint8_t		reserved[12];
	/** Size */
	uint16_t	size;
	/** Header 16 bit checksum */
	uint16_t	checksum;
};

/** Flash stage2 v2 header */
struct al_flash_stage2_v2_hdr {
	/** Reserved */
	uint8_t		reserved[12];
	/** Size */
	uint32_t	size;
	/** Reserved */
	uint8_t		reserved2[14];
	/** Header 16 bit checksum */
	uint16_t	checksum;
};

/** Boot mode */
struct al_flash_boot_mode {
	/** Format revision ID */
	uint32_t	format_rev_id;
	/** Object ID */
	uint32_t	default_app_id;
	/** Reserved */
	uint8_t		reserved[56];
};

#define AL_FLASH_PUK_TYPE_RSA_2K			0
#define AL_FLASH_SIG_TYPE_PKCS_1_V15_RSA_2K_SHA_256	0

/** Public Key header */
struct al_flash_puk_hdr {
	/** Format revision ID */
	uint32_t	format_rev_id;
	/** Public key type - AL_FLASH_PUK_TYPE_* */
	uint32_t	type;
	/** Reserved */
	uint32_t	reserved[6];
};

/** Signature header */
struct al_flash_sig_hdr {
	/** Format revision ID */
	uint32_t	format_rev_id;
	/** Public key index */
	uint32_t	puk_idx;
	/** Signature type - AL_FLASH_SIG_TYPE_* */
	uint32_t	type;
	/** Reserved */
	uint32_t	reserved[5];
};

/**
 * Flash device read function pointer
 *
 * @param	offset inside the device
 * @param	address of memory buffer that will store read data
 * @param	size of read (in Bytes)
 *
 * @returns	0 if successful
 *		<0 otherwise
 */
typedef int (*al_flash_dev_read)(unsigned int, void *, unsigned int);

/**
 * Flash object ID conversion to string
 *
 * @param	id
 *		Object ID
 *
 * @returns	Pointer to a const string representing the object ID
 */
const char *al_flash_obj_id_to_str(
	unsigned int id);

/**
 * Flash object ID string conversion to object ID enumeration
 *
 * @param	str
 *		Object ID string
 *
 * @returns	Object ID enumeration
 */
int al_flash_obj_id_from_str(
	const char *str);

/**
 * Flash TOC searching
 *
 * @param	dev_read_func
 *		Device reading function pointer
 * @param	first_offset_on_dev
 *		First offset on device to begin searching for TOC
 * @param	skip_size
 *		Searching skip size
 * @param	max_num_skips
 *		MAximal number of skips
 * @param	found_offset_on_dev
 *		Offset on device in which TOC has been found
 *
 * @returns	0 if successful
 *		<0 otherwise
 */
int al_flash_toc_search(
	al_flash_dev_read	dev_read_func,
	unsigned int		first_offset_on_dev,
	unsigned int		skip_size,
	unsigned int		max_num_skips,
	unsigned int		*found_offset_on_dev);

/**
 * Flash TOC validation
 *
 * @param	dev_read_func
 *		Device reading function pointer
 * @param	offset_on_dev
 *		Offset on device
 * @param	total_size
 *		TOC total size
 * @param	num_entries
 *		TOC number of entries
 *
 * @returns	0 if successful
 *		<0 otherwise
 */
int al_flash_toc_validate(
	al_flash_dev_read	dev_read_func,
	unsigned int		offset_on_dev,
	unsigned int		*total_size,
	unsigned int		*num_entries);

/**
 * Flash TOC printout
 *
 * @param	dev_read_func
 *		Device reading function pointer
 * @param	offset_on_dev
 *		Offset on device
 *
 * @returns	0 if successful
 *		<0 otherwise
 */
int al_flash_toc_print(
	al_flash_dev_read	dev_read_func,
	unsigned int		offset_on_dev);

/**
 * Flash TOC version printout
 *
 * @param	dev_read_func
 *		Device reading function pointer
 * @param	offset_on_dev
 *		Offset on device
 * @param	temp_buff
 *		Temporary buffer for object data processing
 * @param	temp_buff_size
 *		Temporary buffer size*
 * @returns	0 if successful
 *		<0 otherwise
 */
int al_flash_toc_print_ex(
	al_flash_dev_read	dev_read_func,
	unsigned int		offset_on_dev,
	void			*temp_buff,
	unsigned int		temp_buff_size);

/**
 * Flash object information printout
 *
 * @param	dev_read_func
 *		Device reading function pointer
 * @param	offset_on_dev
 *		Offset on device
 * @param	obj_id
 *		Object ID
 * @param	temp_buff
 *		Temporary buffer for object data processing
 * @param	temp_buff_size
 *		Temporary buffer size
 *
 * @returns	0 if successful
 *		<0 otherwise
 */
int al_flash_obj_info_print(
	al_flash_dev_read	dev_read_func,
	unsigned int		offset_on_dev,
	unsigned int		obj_id,
	void			*temp_buff,
	unsigned int		temp_buff_size);

/**
 * Flash TOC entry get
 *
 * @param	dev_read_func
 *		Device reading function pointer
 * @param	offset_on_dev
 *		Offset on device
 * @param	index
 *		TOC entry index
 * @param	found_entry
 *		Found entry
 *
 * @returns	0 if successful
 *		<0 otherwise
 */
int al_flash_toc_entry_get(
	al_flash_dev_read		dev_read_func,
	unsigned int			offset_on_dev,
	unsigned int			index,
	struct al_flash_toc_entry	*found_entry);

/**
 * Flash TOC object finding by ID
 *
 * @param	dev_read_func
 *		Device reading function pointer
 * @param	offset_on_dev
 *		Offset on device
 * @param	obj_id
 *		Object ID
 * @param	first_index
 *		First TOC entry to begin the search with
 * @param	found_index
 *		Index of the found entry
 * @param	found_entry
 *		Found entry
 *
 * @returns	0 if successful
 *		<0 otherwise
 */
int al_flash_toc_find_id(
	al_flash_dev_read		dev_read_func,
	unsigned int		offset_on_dev,
	uint32_t		obj_id,
	unsigned int		first_index,
	int			*found_index,
	struct al_flash_toc_entry	*found_entry);

/**
 * Flash TOC object finding by ID and fallback ID
 *
 * @param	dev_read_func
 *		Device reading function pointer
 * @param	offset_on_dev
 *		Offset on device
 * @param	obj_id
 *		Object ID
 * @param	obj_fallback_id
 *		Object fallback ID
 * @param	first_index
 *		First TOC entry to begin the search with
 * @param	found_index
 *		Index of the found entry
 * @param	found_entry
 *		Found entry
 *
 * @returns	0 if successful
 *		<0 otherwise
 */
int al_flash_toc_find_id_with_fallback(
	al_flash_dev_read		dev_read_func,
	unsigned int			offset_on_dev,
	uint32_t			obj_id,
	uint32_t			obj_fallback_id,
	unsigned int			first_index,
	int				*found_index,
	struct al_flash_toc_entry	*found_entry);

/**
 * Flash TOC active stage 2 finding
 *
 * @param	pbs_sram_base
 *		Pointer to the PBS SRAM
 * @param	dev_read_func
 *		Device reading function pointer
 * @param	offset_on_dev
 *		Offset on device
 * @param	instance_num
 *		Stage 2 instance number
 *
 * @returns	0 if successful
 *		<0 otherwise
 */
int al_flash_toc_stage2_active_instance_get(
	void			*pbs_sram_base,
	al_flash_dev_read	dev_read_func,
	unsigned int		offset_on_dev,
	unsigned int		*instance_num);

/**
 * Flash TOC active stage 2 finding by offset
 *
 * @param	pbs_sram_base
 *		Pointer to the PBS SRAM
 * @param	stage2_actual_offset_addr_in_pbs_sram
 *		Offset on PBS SRAM of Stage 2 actual offset address
 *		(e.g. STAGE2_ACTUAL_OFFSET_ADDR_IN_PBS_SRAM)
 * @param	dev_read_func
 *		Device reading function pointer
 * @param	offset_on_dev
 *		Offset on device
 * @param	instance_num
 *		Stage 2 instance number
 *
 * @returns	0 if successful
 *		<0 otherwise
 */
int al_flash_toc_stage2_active_instance_get_by_off(
	void			*pbs_sram_base,
	unsigned int	stage2_actual_offset_addr_in_pbs_sram,
	al_flash_dev_read	dev_read_func,
	unsigned int		offset_on_dev,
	unsigned int		*instance_num);

/**
 * Flash TOC active stage 2 finding with fallback
 *
 * @param	pbs_sram_base
 *		Pointer to the PBS SRAM
 * @param	stage2_actual_offset_addr_in_pbs_sram
 *		Offset on PBS SRAM of Stage 2 actual offset address
 *		(e.g. STAGE2_ACTUAL_OFFSET_ADDR_IN_PBS_SRAM)
 * @param	dev_read_func
 *		Device reading function pointer
 * @param	offset_on_dev
 *		Offset on device
 * @param	instance_num
 *		Stage 2 instance number
 * @param	instance_fallback_num
 *		Stage 2 instance fallback number
 *
 * @returns	0 if successful
 *		<0 otherwise
 */
int al_flash_toc_stage2_active_instance_get_with_fallback(
	void			*pbs_sram_base,
	unsigned int		stage2_actual_offset_addr_in_pbs_sram,
	al_flash_dev_read	dev_read_func,
	unsigned int		offset_on_dev,
	unsigned int		*instance_num,
	unsigned int		*instance_fallback_num);

/**
 * Flash TOC active stage 2 temporary forcing (until next reboot)
 *
 * @param	pbs_sram_base
 *		Pointer to the PBS SRAM
 * @param	stage2_actual_offset_addr_in_pbs_sram
 *		Offset on PBS SRAM of Stage 2 actual offset address
 *		(e.g. STAGE2_ACTUAL_OFFSET_ADDR_IN_PBS_SRAM)
 * @param	instance_num
 *		Instance number
 * @param	instance_fallback_num
 *		Instance fallback number
 */
void al_flash_toc_stage2_active_instance_force_temporary(
	void			*pbs_sram_base,
	unsigned int		stage2_actual_offset_addr_in_pbs_sram,
	unsigned int		instance_num,
	unsigned int		instance_fallback_num);

/**
 * Flash object header validation
 *
 * @param	dev_read_func
 *		Device reading function pointer
 * @param	offset_on_dev
 *		Offset on device
 * @param	read_hdr
 *		Flash object header
 *
 * @returns	0 if successful
 *		<0 otherwise
 */
int al_flash_obj_header_read_and_validate(
	al_flash_dev_read	dev_read_func,
	unsigned int		offset_on_dev,
	struct al_flash_obj_hdr	*read_hdr);

/**
 * Flash object data edit
 *
 * note that: 1. this API works only with entries currently stored in DRAM
 *	      2. higher hierarchy should ensure that the object does not
 *		 cross its maximal allowed size
 *
 * @param	entry
 *		Pointer to the start of an al_flash_toc_entry
 *		(!) entry is assumed to be stored in DRAM
 * @param	data
 *		New entry data buffer
 * @param	size
 *		Data buffer total size
 *
 * @returns	0 if successful
 *		<0 otherwise
 */
int al_flash_obj_data_write(
	void			*entry,
	void			*data,
	size_t			size);

/**
 * Flash object data offset get
 *
 * @param	obj_offset_on_dev
 *		Object offset on device
 *
 * @returns	Object data offset on device
 */
unsigned int al_flash_obj_data_offset_get(
	unsigned int		obj_offset_on_dev);

/**
 * Flash object data loading
 *
 * @param	dev_read_func
 *		Device reading function pointer
 * @param	offset_on_dev
 *		Offset on device
 * @param	load_addr
 *		Address to load the data to
 *
 * @returns	0 if successful
 *		<0 otherwise
 */
int al_flash_obj_data_load(
	al_flash_dev_read	dev_read_func,
	unsigned int		offset_on_dev,
	void			*load_addr);

/**
 * Flash object validation
 *
 * @param	dev_read_func
 *		Device reading function pointer
 * @param	offset_on_dev
 *		Offset on device
 * @param	temp_buff
 *		Temporary buffer for object data processing
 * @param	temp_buff_size
 *		Temporary buffer size
 * @param	read_hdr
 *		The object read header
 *
 * @returns	0 if successful
 *		<0 otherwise
 */
int al_flash_obj_validate(
	al_flash_dev_read	dev_read_func,
	unsigned int		offset_on_dev,
	void			*temp_buff,
	unsigned int		temp_buff_size,
	struct al_flash_obj_hdr	*read_hdr);

/**
 * Flash stage2 validation
 *
 * @param	dev_read_func
 *		Device reading function pointer
 * @param	offset_on_dev
 *		Offset on device
 * @param	temp_buff
 *		Temporary buffer for object data processing
 * @param	temp_buff_size
 *		Temporary buffer size
 * @param	read_hdr
 *		The object read header
 *
 * @returns	0 if successful
 *		<0 otherwise
 */
int al_flash_stage2_validate(
	al_flash_dev_read	dev_read_func,
	unsigned int		offset_on_dev,
	void			*temp_buff,
	unsigned int		temp_buff_size,
	struct al_flash_obj_hdr	*read_hdr);

int al_flash_stage2_v2_validate(
	al_flash_dev_read	dev_read_func,
	unsigned int		offset_on_dev,
	void			*temp_buff,
	unsigned int		temp_buff_size,
	struct al_flash_obj_hdr	*read_hdr);

/**
 * Flash pre-boot validation
 *
 * @param	dev_read_func
 *		Device reading function pointer
 * @param	offset_on_dev
 *		Offset on device
 * @param	temp_buff
 *		Temporary buffer for object data processing
 * @param	temp_buff_size
 *		Temporary buffer size
 * @param	read_hdr
 *		The object read header
 *
 * @returns	0 if successful
 *		<0 otherwise
 */
int al_flash_pre_boot_validate(
	al_flash_dev_read	dev_read_func,
	unsigned int		offset_on_dev,
	void			*temp_buff,
	unsigned int		temp_buff_size,
	struct al_flash_obj_hdr	*read_hdr);

int al_flash_pre_boot_v2_validate(
	al_flash_dev_read	dev_read_func,
	unsigned int		offset_on_dev,
	void			*temp_buff,
	unsigned int		temp_buff_size,
	struct al_flash_obj_hdr	*read_hdr);

/**
 * Flash HW preload object validation
 *
 * @param	dev_read_func
 *		Device reading function pointer
 * @param	offset_on_dev
 *		Offset on device
 * @param	temp_buff
 *		Temporary buffer for object data processing
 * @param	temp_buff_size
 *		Temporary buffer size
 * @param	read_hdr
 *		The object read header
 *
 * @returns	0 if successful
 *		<0 otherwise
 */
int al_flash_hw_pld_validate(
	al_flash_dev_read	dev_read_func,
	unsigned int		offset_on_dev,
	void			*temp_buff,
	unsigned int		temp_buff_size,
	struct al_flash_obj_hdr	*read_hdr);

typedef int (*al_flash_buff_auth_pkcs_1_v15)(
	const void	*data,
	unsigned int	data_size,
	const void	*sig,
	unsigned int	sig_size,
	const void	*public_exponent,
	unsigned int	public_exponent_size,
	const void	*modulus,
	unsigned int	modulus_size);

typedef int (*al_flash_buff_auth_hash_in_efuse)(
	const void	*data,
	unsigned int	data_size);

typedef int (*al_flash_obj_ver_auth)(
	struct al_flash_obj_hdr	*obj_hdr);

int al_flash_obj_auth(
	al_flash_dev_read			dev_read_func,
	unsigned int				toc_offset_on_dev,
	uint32_t				obj_id,
	void					*temp_buff,
	unsigned int				temp_buff_size,
	al_flash_buff_auth_hash_in_efuse	auth_hash_in_efuse,
	al_flash_buff_auth_pkcs_1_v15		auth_pkcs_1_v15,
	al_flash_obj_ver_auth			auth_ver,
	al_bool					load,
	void					*load_addr,
	struct al_flash_obj_hdr			*authenticated_fimg_hdr);

#endif
