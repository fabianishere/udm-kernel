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

#ifndef __AL_TPM_H__
#define __AL_TPM_H__

#include "al_tpm_tis12.h"
#include "al_tpm_tis20.h"


#define AL_TPM_DIGEST_NUM_BYTES			20
#define AL_TPM_NV_MAX_READ_SIZE_NUM_BYTES	128


/** The Endorsement credential */
#define AL_TPM_NV_INDEX_EKCERT		0x0000F000

/**
 * TPM service result
 */
enum al_tpm_result {
	AL_TPM_STATUS_OK,
	AL_TPM_STATUS_FAIL,
};

/**
 * TPM startup type
 */
enum al_tpm_startup_type {
	AL_TPM_STARTUP_TYPE_CLEAR	= 1,
	AL_TPM_STARTUP_TYPE_SAVE	= 2,
	AL_TPM_STARTUP_TYPE_DEACTIVATED	= 3,
};

/**
 * TPM SW handle
 */
struct al_tpm {
	al_bool tpm_2_0;
	struct al_tpm_tis12_handle *tis12_handle;
	struct al_tpm_tis20_handle *tis20_handle;
};

/**
 * TPM Permanent flags
 */
struct tpm_permanent_flags {
	uint8_t disable;
	uint8_t ownership;
	uint8_t deactivated;
	uint8_t read_pubek;
	uint8_t disable_owner_clear;
	uint8_t allow_maintenance;
	uint8_t physical_presence_lifetime_lock;
	uint8_t physical_presence_hw_enable;
	uint8_t physical_presence_cmd_enable;
	uint8_t cekp_used;
	uint8_t tpm_post;
	uint8_t tpm_post_lock;
	uint8_t fips;
	uint8_t tpm_operator;
	uint8_t enable_revoke_ek;
	uint8_t nv_locked;
	uint8_t read_srk_pub;
	uint8_t tpm_established;
	uint8_t maintenance_done;
	uint8_t disable_full_da_logic_info;
};

/**
 * TPM volatile flags
 */
struct tpm_volatile_flags {
	uint8_t deactivated;
	uint8_t disable_force_clear;
	uint8_t physical_presence;
	uint8_t physical_presence_lock;
	uint8_t bglobal_lock;
};

/**
 * TPM version info
 */
struct tpm_version {
	uint8_t major;
	uint8_t minor;
	uint8_t rev_major;
	uint8_t rev_minor;
};

struct tpm_cap_version_info {
	uint16_t tag;
	struct tpm_version version;
	uint16_t spec_level;
	uint8_t errata_rev;
	uint8_t tpm_vendor_id[4];
	uint16_t vendor_specific_size;
	uint8_t vendor_specific[TPM_MAX_RESP_SIZE];
};

/**
 * TPM SW handle initialization
 *
 * @param	tpm
 *		Uninitialized handle
 * @param	tis12_handle
 *		Initialized TIS 1.2 handle
 */
void al_tpm_handle_init(
	struct al_tpm			*tpm,
	struct al_tpm_tis12_handle	*tis12_handle);

/**
 * TPM SW handle initialization for TPM 2.0
 *
 * @param	tpm
 *		Uninitialized handle
 * @param	tis20_handle
 *		Initialized TIS 2.0 handle
 */
void al_tpm20_handle_init(
	struct al_tpm			*tpm,
	struct al_tpm_tis20_handle	*tis20_handle);

/**
 * TPM HW initialization
 *
 * @param	tpm
 *		Initialized TPM handle
 *
 * @returns	0 upon success
 *		errno otherwise
 */
int al_tpm_init(
	struct al_tpm			*tpm);

/**
 * TPM post initialization startup (once per power-up)
 *
 * @param	tpm
 *		Initialized TPM handle
 * @param	type
 * @param	result
 *		TPM result
 *
 * @returns	0 upon success
 *		errno otherwise
 */
int al_tpm_startup(
	struct al_tpm			*tpm,
	enum al_tpm_startup_type	type,
	enum al_tpm_result		*result);

/**
 * TPM selt test
 *
 * @param	tpm
 *		Initialized TPM handle
 * @param	result
 *		TPM result
 *
 * @returns	0 upon success
 *		errno otherwise
 */
int al_tpm_self_test_full(
	struct al_tpm		*tpm,
	enum al_tpm_result	*result);

/**
 * TPM read public endorsement key
 *
 * @param	tpm
 *		Initialized TPM handle
 * @param	nonce
 *		Nonce
 * @param	pk
 *		Public key
 * @param	digest
 *		Public key digest
 * @param	result
 *		TPM result
 *
 * @returns	0 upon success
 *		errno otherwise
 */
int al_tpm_read_pubek(
	struct al_tpm		*tpm,
	uint8_t			*nonce,
	uint8_t			*pk,
	uint8_t			*digest,
	enum al_tpm_result	*result);

/**
 * TPM create public endorsement key pair
 *
 * @param	tpm
 *		Initialized TPM handle
 * @param	nonce
 *		Nonce
 * @param	pk
 *		Generated public key
 * @param	result
 *		TPM result
 *
 * @returns	0 upon success
 *		errno otherwise
 */
int al_tpm_create_endorsement_key_pair(
	struct al_tpm		*tpm,
	uint8_t			*nonce,
	uint8_t			*pk,
	enum al_tpm_result	*result);

/**
 * TPM owner set disabled
 *
 * @param	tpm
 *		Initialized TPM handle
 * @param	disable_state
 *		Disable state
 * @param	auth_handle
 *		Authentication handle
 * @param	nonce
 *		Nonce
 * @param	continue_auth_session
 *		Continue authentication session
 * @param	owner_auth
 *		Owner authentication
 * @param	result
 *		TPM result
 *
 * @returns	0 upon success
 *		errno otherwise
 */
int al_tpm_owner_set_disable(
	struct al_tpm		*tpm,
	al_bool			disable_state,
	uint32_t		auth_handle,
	uint8_t			*nonce,
	al_bool			continue_auth_session,
	uint8_t			*owner_auth,
	enum al_tpm_result	*result);

/**
 * TPM physical presence HW enable
 *
 * @param	tpm
 *		Initialized TPM handle
 * @param	result
 *		TPM result
 *
 * @returns	0 upon success
 *		errno otherwise
 */
int al_tpm_physical_presence_hw_enable(
	struct al_tpm		*tpm,
	enum al_tpm_result	*result);

/**
 * TPM physical presence command enable
 *
 * @param	tpm
 *		Initialized TPM handle
 * @param	result
 *		TPM result
 *
 * @returns	0 upon success
 *		errno otherwise
 */
int al_tpm_physical_presence_cmd_enable(
	struct al_tpm		*tpm,
	enum al_tpm_result	*result);

/**
 * TPM physical presence set present (through command)
 *
 * @param	tpm
 *		Initialized TPM handle
 * @param	result
 *		TPM result
 *
 * @returns	0 upon success
 *		errno otherwise
 */
int al_tpm_physical_presence_present(
	struct al_tpm		*tpm,
	enum al_tpm_result	*result);

/**
 * TPM physical presence set not present (through command)
 *
 * @param	tpm
 *		Initialized TPM handle
 * @param	result
 *		TPM result
 *
 * @returns	0 upon success
 *		errno otherwise
 */
int al_tpm_physical_presence_not_present(
	struct al_tpm		*tpm,
	enum al_tpm_result	*result);

/**
 * TPM physical enable
 *
 * @param	tpm
 *		Initialized TPM handle
 * @param	result
 *		TPM result
 *
 * @returns	0 upon success
 *		errno otherwise
 */
int al_tpm_physical_enable(
	struct al_tpm		*tpm,
	enum al_tpm_result	*result);

/**
 * TPM physical disable
 *
 * @param	tpm
 *		Initialized TPM handle
 * @param	result
 *		TPM result
 *
 * @returns	0 upon success
 *		errno otherwise
 */
int al_tpm_physical_disable(
	struct al_tpm		*tpm,
	enum al_tpm_result	*result);

/**
 * TPM physical set deactivated
 *
 * @param	tpm
 *		Initialized TPM handle
 * @param	deactivated
 * @param	result
 *		TPM result
 *
 * @returns	0 upon success
 *		errno otherwise
 */
int al_tpm_physical_set_deactivated(
	struct al_tpm		*tpm,
	unsigned int		deactivated,
	enum al_tpm_result	*result);

/**
 * TPM PCR extend
 *
 * @param	tpm
 *		Initialized TPM handle
 * @param	pcr_idx
 *		PCR index
 * @param	digest
 *		Digest to extend
 * @param	result
 *		TPM result
 * @param	out_digest
 *		Extended digest
 *
 * @note In case of TPM2.0 only, out_digest will hold exactly the bytes that
 *       were used as the input to the extend, not the PCR value after the
 *       extension. Why? because according to the TPM 2.0 spec, the extend
 *       response doesn't contain the PCR value after the digest.
 *       If one needs the post-extend PCR value, he can call PCR_read.
 *
 * @returns	0 upon success
 *		errno otherwise
 */
int al_tpm_extend(
	struct al_tpm		*tpm,
	unsigned int		pcr_idx,
	uint8_t			*digest,
	enum al_tpm_result	*result,
	uint8_t			*out_digest);

/**
 * TPM PCR read
 *
 * @param	tpm
 *		Initialized TPM handle
 * @param	pcr_idx
 *		PCR index
 * @param	result
 *		TPM result
 * @param	out_digest
 *		PCR digest
 *
 * @returns	0 upon success
 *		errno otherwise
 */
int al_tpm_pcr_read(
	struct al_tpm		*tpm,
	unsigned int		pcr_idx,
	enum al_tpm_result	*result,
	uint8_t			*out_digest);

/**
 * TPM PCR reset
 *
 * @param	tpm
 *		Initialized TPM handle
 * @param	pcr_idx
 *		PCR index
 * @param	result
 *		TPM result
 *
 * @returns	0 upon success
 *		errno otherwise
 */
int al_tpm_pcr_reset(
	struct al_tpm		*tpm,
	unsigned int		pcr_idx,
	enum al_tpm_result	*result);

/**
 * TPM non volatile read
 *
 * @param	tpm
 *		Initialized TPM handle
 * @param	nv_idx
 *		Non volatile area index
 * @param	offset
 *		Offset from beginning of area
 * @param	data_size
 *		Number of bytes to read
 * @param	result
 *		TPM result
 * @param	actual_data_size
 *		Actual number of bytes read
 * @param	data
 *		Buffer to read to
 *
 * @returns	0 upon success
 *		errno otherwise
 */
int al_tpm_nv_read(
	struct al_tpm		*tpm,
	unsigned int		nv_idx,
	unsigned int		offset,
	unsigned int		data_size,
	enum al_tpm_result	*result,
	unsigned int		*actual_data_size,
	uint8_t			*data);

#endif

/**
 * TPM Force clear
 *
 * @param	tpm
 *		Initialized TPM handle
 *
 * @param	result
 *		TPM result
 *
 * @returns	0 upon success
 *		errno otherwise
 */
int al_tpm_force_clear(
	struct al_tpm		*tpm,
	enum al_tpm_result	*result);


/**
 * TPM get permanent flags
 *
 * @param	tpm
 *		Initialized TPM handle
 *
 * @param	result
 *		TPM result
 * @param	flags
 *		The returned permanent flags
 *
 * @returns	0 upon success
 *		errno otherwise
 */
int al_tpm_get_permanent_flags(
	struct al_tpm		*tpm,
	enum al_tpm_result	*result,
	struct tpm_permanent_flags *flags);

/**
 * TPM get volatile flags
 *
 * @param	tpm
 *		Initialized TPM handle
 *
 * @param	result
 *		TPM result
 * @param	flags
 *		The returned volatile flags
 *
 * @returns	0 upon success
 *		errno otherwise
 */
int al_tpm_get_volatile_flags(
	struct al_tpm		*tpm,
	enum al_tpm_result	*result,
	struct tpm_volatile_flags *flags);

/**
 * TPM get version info
 *
 * @param	tpm
 *		Initialized TPM handle
 * @param	result
 *		TPM result
 * @param	version_info
 *		The returned version info
 * @returns	0 upon success
 *		errno otherwise
 */
int al_tpm_get_version(
	struct al_tpm		*tpm,
	enum al_tpm_result	*result,
	struct tpm_cap_version_info *version_info);

/**
 * TPM 2.0 get specific capability
 * We always request only 1 property because TPM 2.0 spec guarantees
 * Only 1 valid property for each request
 *
 * @param	tpm
 *		Initialized TPM handle
 * @param	result
 *		TPM result
 * @param	capability
 *		capability type
 * @param	property
 *		property type isnide the desired capability
 * @param	data
 *		point to store the output
 * @returns	0 upon success
 *		errno otherwise
 */
int al_tpm2_get_capability(
	struct al_tpm		*tpm,
	enum al_tpm_result	*result,
	uint32_t		capability,
	uint32_t		property,
	uint32_t		*data);

/**
 * TPM 2.0 get random bytes
 * This command returns the next bytes_requested octets
 * from the random number generator (RNG)
 *
 * @param	tpm
 *		Initialized TPM handle
 * @param	result
 *		TPM result
 * @param	bytes_requested
 *		number of bytes to be generated
 * @param	random_bytes
 *		buf to store the RNG result
 * @returns	0 upon success
 *		errno otherwise
 */
int al_tpm2_get_random(
	struct al_tpm		*tpm,
	enum al_tpm_result	*result,
	uint8_t			bytes_requested,
	uint8_t			*random_bytes);

/**
 * TPM read certificate from NVRAM
 *
 * @param	tpm
 *		Initialized TPM handle
 * @param	nv_idx
 *		NV index
 * @param	data_size
 *		size of data buffer
 * @param	result
 *		TPM result
 * @param	actual_data_size
 *		returned data size
 * @param	data
 *		data buffer
 * @returns	0 upon success
 *		errno otherwise
 */

int al_tpm2_nv_read_cert(
	struct al_tpm		*tpm,
	unsigned int		nv_idx,
	unsigned int		data_size,
	enum al_tpm_result	*result,
	unsigned int		*actual_data_size,
	uint8_t			*data);
