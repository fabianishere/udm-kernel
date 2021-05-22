/*
 * Copyright 2018, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */


#ifndef __AL_TPM_TIS20_H__
#define __AL_TPM_TIS20_H__

#include "al_tpm_if.h"

enum tpm2_capabilities {
	TPM2_CAP_TPM_PROPERTIES = 6,
};

#define TPM2_PT_NONE			0x00000000
#define TPM2_PT_GROUP			0x00000100
#define TPM2_PT_FIXED			(TPM2_PT_GROUP * 1)
#define TPM2_PT_FAMILY_INDICATOR	((uint32_t)(TPM2_PT_FIXED + 0))
#define TPM2_PT_LEVEL			((uint32_t)(TPM2_PT_FIXED + 1))
#define TPM2_PT_REVISION		((uint32_t)(TPM2_PT_FIXED + 2))
#define TPM2_PT_DAY_OF_YEAR		((uint32_t)(TPM2_PT_FIXED + 3))
#define TPM2_PT_YEAR			((uint32_t)(TPM2_PT_FIXED + 4))
#define TPM2_PT_MANUFACTURER		((uint32_t)(TPM2_PT_FIXED + 5))
#define TPM2_PT_VENDOR_STRING_1		((uint32_t)(TPM2_PT_FIXED + 6))
#define TPM2_PT_VENDOR_STRING_2		((uint32_t)(TPM2_PT_FIXED + 7))
#define TPM2_PT_VENDOR_STRING_3		((uint32_t)(TPM2_PT_FIXED + 8))
#define TPM2_PT_VENDOR_STRING_4		((uint32_t)(TPM2_PT_FIXED + 9))
#define TPM2_PT_VENDOR_TPM_TYPE		((uint32_t)(TPM2_PT_FIXED + 10))
#define TPM2_PT_FIRMWARE_VERSION_1	((uint32_t)(TPM2_PT_FIXED + 11))
#define TPM2_PT_FIRMWARE_VERSION_2	((uint32_t)(TPM2_PT_FIXED + 12))

struct al_tpm_tis20_handle {
	struct al_tpm_if	*tpm_if;
	uint8_t			stat;
	uint8_t locality;
};

/**
 * Initialize a handle for TPM TIS 2.0 (TPM interface specification) services
 *
 * @param	handle
 *		Uninitialized handle
 * @param	tpm_if
 *		TPM interface initialized handle
 */
void al_tpm_tis20_handle_init(
	struct al_tpm_tis20_handle	*handle,
	struct al_tpm_if		*tpm_if);

/**
 * TPM vendor ID & device ID get
 *
 * @param	handle
 *		Initialized handle
 * @param	vid
 *		Vendor ID
 * @param	did
 *		Device ID
 */
void al_tpm_tis20_vid_did_get(
	struct al_tpm_tis20_handle	*handle,
	unsigned int			*vid,
	unsigned int			*did);

/**
 * Setup TPM
 *
 * @param	handle
 *		Initialized handle
 *
 * @returns	0 upon success
 *		errno otherwise
 */
int al_tpm_tis20_init(
	struct al_tpm_tis20_handle *handle);

/**
 * TPM start transaction
 *
 * @param	handle
 *		Initialized handle
 * @param	is_write
 *		AL_TRUE if starting a write transaction
 *
 * @returns	0 upon success
 *		errno otherwise
 */
int al_tpm_tis20_start(
	struct al_tpm_tis20_handle	*handle,
	al_bool				is_write);

/**
 * Read from TPM
 *
 * @param	handle
 *		Initialized handle
 * @param	buf
 *		Buffer to read to
 * @param	len
 *		How many bytes to read
 * @param	count
 *		How many bytes actually have been read
 *
 * @returns	0 upon success
 *		errno otherwise
 */
int al_tpm_tis20_read(
	struct al_tpm_tis20_handle	*handle,
	void				*buf,
	int				len,
	int				*count);

/**
 * Write to TPM
 *
 * @param	handle
 *		Initialized handle
 * @param	buf
 *		Buffer to write
 * @param	len
 *		How many bytes to write
 *
 * @returns	0 upon success
 *		errno otherwise
 */
int al_tpm_tis20_write(
	struct al_tpm_tis20_handle	*handle,
	void				*buf,
	int				len);

/**
 * TPM finish transaction
 *
 * @param	handle
 *		Initialized handle
 * @param	is_write
 *		AL_TRUE if finishing a write transaction
 * @param	is_err
 *		AL_TRUE if recovering from error
 *
 * @returns	0 upon success
 *		errno otherwise
 */
int al_tpm_tis20_end(
	struct al_tpm_tis20_handle	*handle,
	al_bool				is_write,
	al_bool				is_err);

#endif


void cpu_to_tpm32(uint32_t num, void *be_num_v);
void cpu_to_tpm16(uint16_t num, void *be_num_v);
uint32_t cpu_from_tpm32(void *be_num_v);
uint16_t cpu_from_tpm16(void *be_num_v);
