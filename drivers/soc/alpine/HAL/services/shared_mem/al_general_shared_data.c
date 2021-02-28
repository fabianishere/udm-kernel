/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#include "al_general_shared_data.h"
#include "al_hal_plat_services.h"
#include "al_hal_reg_utils.h"


#define AL_STG2_SHARED_DATA_MN_V1			0x3B
#define AL_STG2_SHARED_DATA_MN_LATEST			AL_STG2_SHARED_DATA_MN_V1

#define AL_STG2_SHARED_DATA_XMODEDM_LOAD_ENFORCE_BIT	0
#define AL_STG2_SHARED_DATA_SPI_LOAD_ENFORCE_BIT	1
#define AL_STG2_SHARED_DATA_NITRO_MODE_BIT		2
#define AL_STG2_SHARED_DATA_FLAG_UBOOT_USER_BOOT_ABORT_BIT  7

#define AL_GENERAL_SHARED_DATA_MAC_ADDR_MN		0x2D

/*************************************************************************************************
 * Static functions										 *
 *************************************************************************************************/
static inline al_bool is_stg2_shared_data_version_gteq(
	struct al_stage2_shared_data		*stg2_shared_data,
	int					magic_num)
{
	return stg2_shared_data->magic_num >= magic_num;
}

/*************************************************************************************************
 * API												 *
 *************************************************************************************************/
void al_general_shared_data_init(
	struct al_general_shared_data		*shared_data)
{
	al_memset(shared_data, 0, sizeof(struct al_general_shared_data));
	shared_data->magic_num = AL_GENERAL_SHARED_DATA_MN;
}

void al_stg2_shared_data_init(
	struct al_general_shared_data		*shared_data)
{
	struct al_stage2_shared_data *stg2_shared_data = &(shared_data->stage2_shared_data);
	al_memset(stg2_shared_data, 0, sizeof(struct al_stage2_shared_data));
	stg2_shared_data->magic_num = AL_STG2_SHARED_DATA_MN_LATEST;
}

al_bool al_stg2_shared_data_flag_get(
	struct al_general_shared_data		*shared_data,
	enum al_stg2_shared_data_flag		flag)
{
	struct al_stage2_shared_data *stg2_shared_data = &(shared_data->stage2_shared_data);
	int required_magic_number;
	al_bool val;

	switch (flag) {
	case AL_STG2_SHARED_DATA_FLAG_XMODEM_LOAD_ENFORCE:
		val = AL_REG_BIT_GET(stg2_shared_data->flags,
				     AL_STG2_SHARED_DATA_XMODEDM_LOAD_ENFORCE_BIT);
		required_magic_number = AL_STG2_SHARED_DATA_MN_V1;
		break;
	case AL_STG2_SHARED_DATA_FLAG_SPI_LOAD_ENFORCE:
		val = AL_REG_BIT_GET(stg2_shared_data->flags,
				     AL_STG2_SHARED_DATA_SPI_LOAD_ENFORCE_BIT);
		required_magic_number = AL_STG2_SHARED_DATA_MN_V1;
		break;
	case AL_STG2_SHARED_DATA_FLAG_NITRO_MODE:
		val = AL_REG_BIT_GET(stg2_shared_data->flags,
				     AL_STG2_SHARED_DATA_NITRO_MODE_BIT);
		required_magic_number = AL_STG2_SHARED_DATA_MN_V1;
		break;
	case AL_STG2_SHARED_DATA_FLAG_UBOOT_USER_BOOT_ABORT:
		val = AL_REG_BIT_GET(stg2_shared_data->flags,
					AL_STG2_SHARED_DATA_FLAG_UBOOT_USER_BOOT_ABORT_BIT);
		required_magic_number = AL_STG2_SHARED_DATA_MN_V1;
		break;
	default:
		al_assert(0);
		return AL_FALSE;
	}

	if (!is_stg2_shared_data_version_gteq(stg2_shared_data, required_magic_number))
		return AL_FALSE;

	return val;
}

void al_stg2_shared_data_flag_set(
	struct al_general_shared_data		*shared_data,
	enum al_stg2_shared_data_flag		flag,
	al_bool					flag_value)
{
	struct al_stage2_shared_data *stg2_shared_data = &(shared_data->stage2_shared_data);

	switch (flag) {
	case AL_STG2_SHARED_DATA_FLAG_XMODEM_LOAD_ENFORCE:
		AL_REG_BIT_VAL_SET(stg2_shared_data->flags,
				   AL_STG2_SHARED_DATA_XMODEDM_LOAD_ENFORCE_BIT,
				   flag_value);
		break;
	case AL_STG2_SHARED_DATA_FLAG_SPI_LOAD_ENFORCE:
		AL_REG_BIT_VAL_SET(stg2_shared_data->flags,
				   AL_STG2_SHARED_DATA_SPI_LOAD_ENFORCE_BIT,
				   flag_value);
		break;
	case AL_STG2_SHARED_DATA_FLAG_NITRO_MODE:
		AL_REG_BIT_VAL_SET(stg2_shared_data->flags,
				   AL_STG2_SHARED_DATA_NITRO_MODE_BIT,
				   flag_value);
		break;
	case AL_STG2_SHARED_DATA_FLAG_UBOOT_USER_BOOT_ABORT:
		AL_REG_BIT_VAL_SET(stg2_shared_data->flags,
					AL_STG2_SHARED_DATA_FLAG_UBOOT_USER_BOOT_ABORT_BIT,
				    flag_value);
		break;
	default:
		al_assert(0);
	}
}

void al_general_shared_data_mac_addr_set(
	struct al_general_shared_data		*shared_data,
	uint8_t					*mac_addr,
	unsigned int				mac_addr_num)
{
	al_assert(shared_data);
	al_assert(mac_addr);
	al_assert(mac_addr_num < AL_GENERAL_SHARED_MAC_ADDR_NUM);

	al_memcpy(shared_data->mac_addr_data[mac_addr_num].addr,
		  mac_addr,
		  AL_GENERAL_SHARED_MAC_ADDR_LEN);

	shared_data->mac_addr_data[mac_addr_num].magic_num = AL_GENERAL_SHARED_DATA_MAC_ADDR_MN;
}

al_bool al_general_shared_data_mac_addr_get(
	struct al_general_shared_data		*shared_data,
	unsigned int				mac_addr_num,
	uint8_t					*mac_addr)
{
	al_bool val;

	al_assert(shared_data);
	al_assert(mac_addr);
	al_assert(mac_addr_num < AL_GENERAL_SHARED_MAC_ADDR_NUM);

	if (shared_data->mac_addr_data[mac_addr_num].magic_num ==
				AL_GENERAL_SHARED_DATA_MAC_ADDR_MN) {
		val = AL_TRUE;
		al_memcpy(mac_addr,
			  shared_data->mac_addr_data[mac_addr_num].addr,
			  AL_GENERAL_SHARED_MAC_ADDR_LEN);
	} else {
		val = AL_FALSE;
	}

	return val;
}
