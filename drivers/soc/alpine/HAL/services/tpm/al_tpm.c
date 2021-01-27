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

#include "al_tpm.h"
#include "al_hal_common.h"

#define TPM_TAG_RCU_COMMAND			0x00c1
#define TPM_TAG_RSP_COMMAND			0x00c4

#define TPM_ORD_EXTEND				0x00000014
#define TPM_ORD_PCRREAD				0x00000015
#define TPM_ORD_SELFTESTFULL			0x00000050
#define TPM_ORD_FORCECLEAR			0x0000005D
#define TPM_ORD_GET_CAPABILITY			0x00000065
#define TPM_ORD_OWNERSETDISABLE			0x0000006e
#define TPM_ORD_PHYSICALENABLE			0x0000006f
#define TPM_ORD_PHYSICALDISABLE			0x00000070
#define TPM_ORD_PHYSICALSETDEACTIVATED		0x00000072
#define TPM_ORD_CREATEENDORSEMENTKEYPAIR	0x00000078
#define TPM_ORD_READPUBEK			0x0000007c
#define TPM_ORD_STARTUP				0x00000099
#define TPM_ORD_NV_READVALUE			0x000000CF
#define TPM_ORD_PHYSICALPRESENCE		0x4000000A
#define TPM_ORD_NV_READ				0x0000014E
#define TPM_ORD_NV_READPUBLIC			0x00000169

#define TPM_ALG_RSA				0x00000001

#define TPM_PHYSICAL_PRESENCE_PRESENT		0x0008
#define TPM_PHYSICAL_PRESENCE_NOT_PRESENT	0x0010
#define TPM_PHYSICAL_PRESENCE_HW_ENABLE		0x0040
#define TPM_PHYSICAL_PRESENCE_CMD_ENABLE	0x0020

#define TPM_CAP_FLAG		0x00000004
#define TPM_CAP_FLAG_PERMANENT	0x00000108
#define TPM_CAP_FLAG_VOLATILE	0x00000109

#define TPM_CAP_VERSION_VAL				0x0000001A
#define TPM_TAG_CAP_VERSION_INFO			0X0030
#define TPM2_CC_FIRST		 0x011F
#define TPM2_CC_SELF_TEST	 0x0143
#define TPM2_CC_STARTUP		 0x0144
#define TPM2_CC_SHUTDOWN	 0x0145
#define TPM2_CC_CREATE		 0x0153
#define TPM2_CC_LOAD		 0x0157
#define TPM2_CC_UNSEAL		 0x015E
#define TPM2_CC_FLUSH_CONTEXT	0x0165
#define TPM2_CC_GET_CAPABILITY	0x017A
#define TPM2_CC_GET_RANDOM	 0x017B
#define TPM2_CC_PCR_READ	 0x017E
#define TPM2_CC_PCR_EXTEND	 0x0182
#define TPM2_CC_LAST		 0x018F
#define TPM2_CC_CLEAR		0x0126
#define TPM2_CC_NV_READ		0x014E
#define TPM2_CC_NV_READPUBLIC	0x0169

#define TPM2_RH_OWNER		0x40000001
#define TPM2_RS_PW		0x40000009
#define TPM2_RH_PLATFORM	0x4000000C

#define TPM2_ST_NO_SESSIONS	 0x8001
#define TPM2_ST_SESSIONS	 0x8002

#define TPM2_PCR_SELECT_SIZE	3
#define TPM2_PCR_GRANULARITY	8

#define TPM2_MAX_RANDOM_BUF_SIZE	128
#define TPM2_MAX_NV_BUFFER_SIZE		256

#define SHA256_DIGEST_SIZE		32


struct __attribute__ ((__packed__)) al_tpm_cmd_base {
	uint16_t tag;
	uint32_t len;
	uint32_t cmd_id;
};

struct __attribute__ ((__packed__)) al_tpm_resp_base {
	uint16_t tag;
	uint32_t len;
	uint32_t result;
};

void cpu_to_tpm32(uint32_t num, void *be_num_v)
{
	uint8_t *be_num = (uint8_t *)be_num_v;

	be_num[0] = (num >> 24) & 0xff;
	be_num[1] = (num >> 16) & 0xff;
	be_num[2] = (num >> 8) & 0xff;
	be_num[3] = (num >> 0) & 0xff;
}

void cpu_to_tpm16(uint16_t num, void *be_num_v)
{
	uint8_t *be_num = (uint8_t *)be_num_v;

	be_num[0] = (num >> 8) & 0xff;
	be_num[1] = (num >> 0) & 0xff;
}

uint32_t cpu_from_tpm32(void *be_num_v)
{
	uint8_t *be_num = (uint8_t *)be_num_v;

	return (be_num[0] << 24) | (be_num[1] << 16) | (be_num[2] << 8) | (be_num[3] << 0);
}

uint16_t cpu_from_tpm16(void *be_num_v)
{
	uint8_t *be_num = (uint8_t *)be_num_v;

	return (be_num[0] << 8) | (be_num[1] << 0);
}

static int al_tpm_cmd_resp_unknown_size(
	struct al_tpm			*tpm,
	uint8_t				*cmd_buff,
	unsigned int			cmd_buff_size,
	al_bool				is_resp_fixed_size,
	uint8_t				*resp_buff,
	unsigned int			resp_buff_size,
	enum al_tpm_result		*result)
{
	int i;
	int err;
	struct al_tpm_cmd_base *cmd_base = (struct al_tpm_cmd_base *)cmd_buff;
	struct al_tpm_resp_base *resp_base = (struct al_tpm_resp_base *)resp_buff;
	int resp_len_base = sizeof(struct al_tpm_resp_base);
	int resp_len;
	int resp_len_actual;
	al_assert(cmd_buff);
	al_assert(cmd_buff_size >= sizeof(struct al_tpm_cmd_base));
	al_assert(resp_buff);
	al_assert((int)resp_buff_size >= resp_len_base);

	if (tpm->tpm_2_0 == AL_FALSE)
		cpu_to_tpm16(TPM_TAG_RCU_COMMAND, &cmd_base->tag);
	cpu_to_tpm32(cmd_buff_size, &cmd_base->len);
	al_dbg("%s: cmd (%u bytes) = ", __func__, cmd_buff_size);
	for (i = 0; i < (int)cmd_buff_size; i++)
		al_dbg("%02x ", cmd_buff[i]);
	al_dbg("\n");

	err = (tpm->tpm_2_0 == AL_TRUE) ?
			al_tpm_tis20_start(tpm->tis20_handle, AL_TRUE)
			: al_tpm_tis12_start(tpm->tis12_handle, AL_TRUE);
	if (err)
		return err;
	err = (tpm->tpm_2_0 == AL_TRUE) ?
			al_tpm_tis20_write(tpm->tis20_handle, cmd_buff, cmd_buff_size)
			: al_tpm_tis12_write(tpm->tis12_handle, cmd_buff, cmd_buff_size);
	if (err)
		return err;
	err = (tpm->tpm_2_0 == AL_TRUE) ?
			al_tpm_tis20_end(tpm->tis20_handle, AL_TRUE, AL_FALSE)
			: al_tpm_tis12_end(tpm->tis12_handle, AL_TRUE, AL_FALSE);
	if (err)
		return err;

	err = (tpm->tpm_2_0 == AL_TRUE) ?
			al_tpm_tis20_start(tpm->tis20_handle, AL_FALSE)
			: al_tpm_tis12_start(tpm->tis12_handle, AL_FALSE);
	if (err)
		return err;

	err = (tpm->tpm_2_0 == AL_TRUE) ?
			al_tpm_tis20_read
				(tpm->tis20_handle, resp_buff, resp_len_base, &resp_len_actual)
			: al_tpm_tis12_read
				(tpm->tis12_handle, resp_buff, resp_len_base, &resp_len_actual, 0);
	if (err)
		return err;

	if (resp_len_actual != resp_len_base)
		return -1;

	resp_len = cpu_from_tpm32(&resp_base->len);
	al_assert(resp_len >= resp_len_base);
	err = (tpm->tpm_2_0 == AL_TRUE) ?
			al_tpm_tis20_read(tpm->tis20_handle, resp_buff + resp_len_base,
				resp_len - resp_len_base, &resp_len_actual)
			: al_tpm_tis12_read(tpm->tis12_handle, resp_buff + resp_len_base,
				resp_len - resp_len_base, &resp_len_actual, 0);
	if (err)
		return err;
	resp_len_actual += resp_len_base;

	al_dbg("%s: resp (%u bytes) = ", __func__, resp_len_actual);
	for (i = 0; i < resp_len_actual; i++)
		al_dbg("%02x ", resp_buff[i]);
	al_dbg("\n");

	err = (tpm->tpm_2_0 == AL_TRUE) ?
			al_tpm_tis20_end(tpm->tis20_handle, AL_FALSE, AL_FALSE)
			: al_tpm_tis12_end(tpm->tis12_handle, AL_FALSE, AL_FALSE);
	if (err)
		return err;

	if (resp_len_actual > (int)resp_buff_size)
		return -EIO;

	if (is_resp_fixed_size && (resp_len_actual != (int)resp_buff_size))
		return -EIO;
	if (tpm->tpm_2_0 == AL_TRUE) {
		if (cpu_from_tpm16(&resp_base->tag) != cpu_from_tpm16(&cmd_base->tag))
			return -EIO;
	} else {
		if (cpu_from_tpm16(&resp_base->tag) != TPM_TAG_RSP_COMMAND)
			return -EIO;
	}
	if (resp_len != resp_len_actual)
		return -EIO;

	*result = cpu_from_tpm32(&resp_base->result);
	return 0;
}

static int  al_tpm_cmd(
	struct al_tpm			*tpm,
	uint8_t				*cmd_buff,
	unsigned int			cmd_buff_size,
	uint8_t				*resp_buff,
	unsigned int			resp_buff_size,
	enum al_tpm_result		*result)
{
	return al_tpm_cmd_resp_unknown_size(tpm, cmd_buff, cmd_buff_size, AL_TRUE,
		resp_buff, resp_buff_size, result);
}

void al_tpm_handle_init(
	struct al_tpm			*tpm,
	struct al_tpm_tis12_handle	*tis12_handle)
{
	al_assert(tpm);
	tpm->tpm_2_0 = AL_FALSE;
	tpm->tis12_handle = tis12_handle;
	tpm->tis20_handle = NULL;

}
void al_tpm20_handle_init(
	struct al_tpm			*tpm,
	struct al_tpm_tis20_handle	*tis20_handle)
{
	al_assert(tpm);
	tpm->tpm_2_0 = AL_TRUE;
	tpm->tis20_handle = tis20_handle;
	tpm->tis12_handle = NULL;
}

int al_tpm_init(
	struct al_tpm			*tpm)
{
	int err;
	enum al_tpm_result      result;
	struct __attribute__ ((__packed__)) {
		struct al_tpm_cmd_base  base;
		uint16_t        size;
	} cmd;
	struct __attribute__ ((__packed__)) {
		struct al_tpm_resp_base base;
		uint8_t random[128];
	} resp;

	al_assert(tpm);
	if (tpm->tpm_2_0 == AL_TRUE) {
		al_assert(tpm->tis20_handle);
		err = al_tpm_tis20_init(tpm->tis20_handle);
		if (err)
			return err;
		cpu_to_tpm32(TPM2_CC_GET_RANDOM, &cmd.base.cmd_id);
		cpu_to_tpm16(TPM2_ST_NO_SESSIONS, &cmd.base.tag);
		cpu_to_tpm16(sizeof(cmd), &cmd.base.len);
		cpu_to_tpm16(16, &cmd.size);
		/* Perform TPM 2.0 transaction, just to verify it is TPM 2.0 */
		err = al_tpm_cmd_resp_unknown_size(tpm, (uint8_t *)&cmd, sizeof(cmd), AL_FALSE,
		(uint8_t *)&resp, sizeof(resp), &result);
		if (err || (cpu_from_tpm16(&resp.base.tag) != TPM2_ST_NO_SESSIONS)) {
			al_err("Failed to detect TPM 2.0 protocol\n");
			return -1;
		}

	} else {
		al_assert(tpm->tis12_handle);
		err = al_tpm_tis12_init(tpm->tis12_handle);
	}
	return err;
}

int al_tpm_startup(
	struct al_tpm			*tpm,
	enum al_tpm_startup_type	type,
	enum al_tpm_result		*result)
{
	int err;
	struct __attribute__ ((__packed__)) {
		struct al_tpm_cmd_base	base;
		uint16_t		type;
	} cmd;
	struct __attribute__ ((__packed__)) {
		struct al_tpm_resp_base	base;
	} resp;

	al_assert(tpm);
	al_assert(result);

	if (tpm->tpm_2_0 == AL_TRUE) {
		cpu_to_tpm32(TPM2_CC_STARTUP, &cmd.base.cmd_id);
		cpu_to_tpm16(TPM2_ST_NO_SESSIONS, &cmd.base.tag);
	} else
		cpu_to_tpm32(TPM_ORD_STARTUP, &cmd.base.cmd_id);
	cpu_to_tpm16(type, &cmd.type);
	err = al_tpm_cmd(
		tpm, (uint8_t *)&cmd, sizeof(cmd), (uint8_t *)&resp, sizeof(resp), result);
	if (err)
		return err;

	return 0;
}

enum tpm2_algorithms {
	TPM2_ALG_SHA1		= 0x0004,
	TPM2_ALG_KEYEDHASH	= 0x0008,
	TPM2_ALG_SHA256		= 0x000B,
	TPM2_ALG_NULL		= 0x0010
};

static int al_tpm2_self_test_full(
	struct al_tpm		*tpm,
	enum al_tpm_result	*result)
{
	int err;
	struct __attribute__ ((__packed__)) {
		struct al_tpm_cmd_base	base;
		uint8_t	full;
	} cmd;
	struct __attribute__ ((__packed__)) {
		struct al_tpm_resp_base	base;
	} resp;
	struct __attribute__ ((__packed__)) tpm2_pcr_read_in {
	uint32_t	pcr_selects_cnt;
	uint16_t	hash_alg;
	uint8_t	pcr_select_size;
	uint8_t	pcr_select[3];
	};
	struct __attribute__ ((__packed__)) tpm2_pcr_read_out {
	uint32_t	update_cnt;
	uint32_t	pcr_selects_cnt;
	uint16_t	hash_alg;
	uint8_t	pcr_select_size;
	uint8_t	pcr_select[3];
	uint32_t	digests_cnt;
	uint16_t	digest_size;
	uint8_t	digest[20];
	};
	struct __attribute__ ((__packed__)) {
		struct al_tpm_cmd_base	base;
		struct tpm2_pcr_read_in pcr_in;
	} cmd_pcr;
	struct __attribute__ ((__packed__)) {
		struct al_tpm_resp_base	base;
		struct tpm2_pcr_read_out pcr_out;
	} resp_pcr;

	cpu_to_tpm16(TPM2_ST_NO_SESSIONS, &cmd.base.tag);
	cpu_to_tpm32(TPM2_CC_SELF_TEST, &cmd.base.cmd_id);
	cmd.full = 1;


	err = al_tpm_cmd(
		tpm, (uint8_t *)&cmd, sizeof(cmd), (uint8_t *)&resp, sizeof(resp), result);
	if (err)
		return err;

	cpu_to_tpm16(TPM2_ST_NO_SESSIONS, &cmd_pcr.base.tag);
	cpu_to_tpm32(TPM2_CC_PCR_READ, &cmd_pcr.base.cmd_id);
	cpu_to_tpm32(1, &cmd_pcr.pcr_in.pcr_selects_cnt);
	cpu_to_tpm16(TPM2_ALG_SHA1, &cmd_pcr.pcr_in.hash_alg);
	cmd_pcr.pcr_in.pcr_select_size = 3;
	cmd_pcr.pcr_in.pcr_select[0] = 0x01;
	cmd_pcr.pcr_in.pcr_select[1] = 0x00;
	cmd_pcr.pcr_in.pcr_select[2] = 0x00;

	err = al_tpm_cmd(
		tpm, (uint8_t *)&cmd_pcr, sizeof(cmd_pcr),
			(uint8_t *)&resp_pcr, sizeof(resp_pcr), result);
	if (err)
		return err;
	return 0;
}

static int al_tpm2_extend(
	struct al_tpm		*tpm,
	unsigned int		pcr_idx,
	uint8_t			*digest,
	enum al_tpm_result	*result,
	uint8_t			*out_digest)
{
	int err;

	struct __attribute__ ((__packed__)) tpm2_null_auth_area {
		uint32_t handle;
		uint16_t nonce_size;
		uint8_t	 attributes;
		uint16_t auth_size;
	};

	struct  __attribute__ ((__packed__)) tpm2_digest {
		uint16_t alg_id;
		uint8_t digest[AL_TPM_DIGEST_NUM_BYTES];
	};

	struct __attribute__ ((__packed__)) {
		struct al_tpm_cmd_base base;
		uint32_t pcr_idx;
		uint32_t auth_area_size;
		struct tpm2_null_auth_area auth_area;
		uint32_t pcr_selects_cnt;
		struct tpm2_digest digest;
	} cmd;

	struct __attribute__ ((__packed__)) {
		struct al_tpm_resp_base	base;
		uint8_t		buf[10];
	} resp;

	cpu_to_tpm16(TPM2_ST_SESSIONS, &cmd.base.tag);
	cpu_to_tpm32(TPM2_CC_PCR_EXTEND, &cmd.base.cmd_id);
	cpu_to_tpm32(pcr_idx, &cmd.pcr_idx);
	cpu_to_tpm32(sizeof(cmd.auth_area), &cmd.auth_area_size);
	cpu_to_tpm32(TPM2_RS_PW, &cmd.auth_area.handle);
	cmd.auth_area.nonce_size = 0;
	cmd.auth_area.auth_size = 0;
	cmd.auth_area.attributes = 0;
	cpu_to_tpm32(1, &cmd.pcr_selects_cnt);
	cpu_to_tpm16(TPM2_ALG_SHA1, &cmd.digest.alg_id);
	al_memcpy(&cmd.digest.digest, digest, AL_TPM_DIGEST_NUM_BYTES);

	err = al_tpm_cmd_resp_unknown_size(tpm, (uint8_t *)&cmd, sizeof(cmd),
		AL_FALSE, (uint8_t *)&resp, sizeof(resp), result);
	if (err)
		return err;

	al_memcpy(out_digest, &cmd.digest.digest, AL_TPM_DIGEST_NUM_BYTES);

	return 0;
}

static int al_tpm2_force_clear(
	struct al_tpm		*tpm,
	enum al_tpm_result	*result)
{
	int err;

	struct __attribute__ ((__packed__)) tpm2_auth_session {
		uint32_t handle;
		uint16_t nonce_size;
		uint8_t	 attributes;
		uint16_t auth_size;
	};

	struct __attribute__ ((__packed__)) {
		struct al_tpm_cmd_base base;
		uint32_t auth_handle;
		uint32_t auth_size;
		struct tpm2_auth_session auth_session;
	} cmd;

	struct __attribute__ ((__packed__)) {
		struct al_tpm_resp_base	base;
		uint32_t auth_size;
		struct tpm2_auth_session auth_session;
	} resp;

	cpu_to_tpm16(TPM2_ST_SESSIONS, &cmd.base.tag);
	cpu_to_tpm32(TPM2_CC_CLEAR, &cmd.base.cmd_id);
	cpu_to_tpm32(TPM2_RH_PLATFORM, &cmd.auth_handle);
	cpu_to_tpm32(sizeof(cmd.auth_session), &cmd.auth_size);
	cpu_to_tpm32(TPM2_RS_PW, &cmd.auth_session.handle);
	cmd.auth_session.nonce_size = 0;
	cmd.auth_session.auth_size = 0;
	cmd.auth_session.attributes = 0;

	err = al_tpm_cmd_resp_unknown_size(tpm, (uint8_t *)&cmd, sizeof(cmd),
		AL_FALSE, (uint8_t *)&resp, sizeof(resp), result);
	if (err)
		return err;

	return 0;
}

static int al_tpm2_pcr_read(
	struct al_tpm		*tpm,
	unsigned int		pcr_idx,
	enum al_tpm_result	*result,
	uint8_t			*out_digest)
{
	int err;
	int i;

	struct __attribute__ ((__packed__)) tpm2_pcr_selection {
	uint32_t	pcr_selects_cnt;
	uint16_t	hash_alg;
	uint8_t		pcr_select_size;
	uint8_t		pcr_select[TPM2_PCR_SELECT_SIZE];
	};

	struct __attribute__ ((__packed__)) tpm2_pcr_read_out {
	uint32_t	update_cnt;
	struct tpm2_pcr_selection pcr_selection;
	uint32_t	digests_cnt;
	uint16_t	digest_size;
	uint8_t		digest[AL_TPM_DIGEST_NUM_BYTES];
	};

	struct __attribute__ ((__packed__)) {
		struct al_tpm_cmd_base	base;
		struct tpm2_pcr_selection pcr_selection;
	} cmd;

	struct __attribute__ ((__packed__)) {
		struct al_tpm_resp_base	base;
		struct tpm2_pcr_read_out pcr_out;
	} resp;

	cpu_to_tpm16(TPM2_ST_NO_SESSIONS, &cmd.base.tag);
	cpu_to_tpm32(TPM2_CC_PCR_READ, &cmd.base.cmd_id);
	cpu_to_tpm32(1, &cmd.pcr_selection.pcr_selects_cnt);
	cpu_to_tpm16(TPM2_ALG_SHA1, &cmd.pcr_selection.hash_alg);
	cmd.pcr_selection.pcr_select_size = TPM2_PCR_SELECT_SIZE;
	for (i = 0; i < TPM2_PCR_SELECT_SIZE; i++)
		cmd.pcr_selection.pcr_select[i] = 0;
	cmd.pcr_selection.pcr_select[pcr_idx / TPM2_PCR_GRANULARITY] =
				(1 << (pcr_idx % TPM2_PCR_GRANULARITY));

	err = al_tpm_cmd(tpm, (uint8_t *)&cmd, sizeof(cmd),
			(uint8_t *)&resp, sizeof(resp), result);
	if (err)
		return err;

	al_memcpy(out_digest, &resp.pcr_out.digest, AL_TPM_DIGEST_NUM_BYTES);

	return err;
}

int al_tpm_self_test_full(
	struct al_tpm		*tpm,
	enum al_tpm_result	*result)
{
	int err;
	struct __attribute__ ((__packed__)) {
		struct al_tpm_cmd_base	base;
	} cmd;
	struct __attribute__ ((__packed__)) {
		struct al_tpm_resp_base	base;
	} resp;

	al_assert(tpm);
	al_assert(result);

	if (tpm->tpm_2_0 == AL_TRUE)
		return al_tpm2_self_test_full(tpm, result);

	cpu_to_tpm32(TPM_ORD_SELFTESTFULL, &cmd.base.cmd_id);
	err = al_tpm_cmd(
		tpm, (uint8_t *)&cmd, sizeof(cmd), (uint8_t *)&resp, sizeof(resp), result);
	if (err)
		return err;

	return 0;
}

int al_tpm_read_pubek(
	struct al_tpm		*tpm,
	uint8_t			*nonce,
	uint8_t			*pk,
	uint8_t			*digest __attribute__((unused)),
	enum al_tpm_result	*result)
{
	int err;
	struct __attribute__ ((__packed__)) {
		struct al_tpm_cmd_base	base;
		uint8_t			nonce[AL_TPM_DIGEST_NUM_BYTES];
	} cmd;
	struct __attribute__ ((__packed__)) {
		struct al_tpm_resp_base	base;
	} resp;

	al_assert(tpm);
	al_assert(nonce);
	al_assert(pk);
	al_assert(result);
	if (tpm->tpm_2_0 == AL_TRUE) {

		al_err("TPM 2.0 is not supported for this function\n");
		return -1;
	}

	cpu_to_tpm32(TPM_ORD_READPUBEK, &cmd.base.cmd_id);
	al_memcpy(&cmd.nonce, nonce, AL_TPM_DIGEST_NUM_BYTES);

	err = al_tpm_cmd(
		tpm, (uint8_t *)&cmd, sizeof(cmd), (uint8_t *)&resp, sizeof(resp), result);
	if (err)
		return err;

	return 0;
}

int al_tpm_create_endorsement_key_pair(
	struct al_tpm		*tpm,
	uint8_t			*nonce,
	uint8_t			*pk,
	enum al_tpm_result	*result)
{
	int err;
	struct __attribute__ ((__packed__)) {
		struct al_tpm_cmd_base	base;
		uint8_t			nonce[AL_TPM_DIGEST_NUM_BYTES];
		uint32_t		alg_id;
		uint16_t		enc_scheme;
		uint16_t		sig_scheme;
		uint32_t		parm_size;
		uint32_t		key_length;
		uint32_t		num_primes;
		uint32_t		exponent_size;
	} cmd;
	struct __attribute__ ((__packed__)) {
		struct al_tpm_resp_base	base;
	} resp;

	al_assert(tpm);
	al_assert(pk);
	al_assert(result);
	if (tpm->tpm_2_0 == AL_TRUE) {

		al_err("TPM 2.0 is not supported for this function\n");
		return -1;

	}

	cpu_to_tpm32(TPM_ORD_CREATEENDORSEMENTKEYPAIR, &cmd.base.cmd_id);
	al_memcpy(&cmd.nonce, nonce, AL_TPM_DIGEST_NUM_BYTES);
	cpu_to_tpm32(TPM_ALG_RSA, &cmd.alg_id);
	cpu_to_tpm16(0, &cmd.enc_scheme);
	cpu_to_tpm16(0, &cmd.sig_scheme);
	cpu_to_tpm32(12, &cmd.parm_size);
	cpu_to_tpm32(2048, &cmd.key_length);
	cpu_to_tpm32(2, &cmd.num_primes);
	cpu_to_tpm32(0, &cmd.exponent_size);

	err = al_tpm_cmd(
		tpm, (uint8_t *)&cmd, sizeof(cmd), (uint8_t *)&resp, sizeof(resp), result);
	if (err)
		return err;

	return 0;
}

int al_tpm_owner_set_disable(
	struct al_tpm		*tpm,
	al_bool			disable_state,
	uint32_t		auth_handle,
	uint8_t			*nonce,
	al_bool			continue_auth_session,
	uint8_t			*owner_auth,
	enum al_tpm_result	*result)
{
	int err;
	struct __attribute__ ((__packed__)) {
		struct al_tpm_cmd_base	base;
		uint8_t			disable_state;
		uint32_t		auth_handle;
		uint8_t			nonce[AL_TPM_DIGEST_NUM_BYTES];
		uint8_t			continue_auth_session;
		uint8_t			owner_auth[AL_TPM_DIGEST_NUM_BYTES];
	} cmd;
	struct __attribute__ ((__packed__)) {
		struct al_tpm_resp_base	base;
	} resp;

	al_assert(tpm);
	al_assert(nonce);
	al_assert(owner_auth);
	al_assert(result);
	if (tpm->tpm_2_0 == AL_TRUE) {

		al_err("TPM 2.0 is not supported for this function\n");
		return -1;

	}

	cpu_to_tpm32(TPM_ORD_OWNERSETDISABLE, &cmd.base.cmd_id);
	cmd.disable_state = disable_state;
	cpu_to_tpm32(auth_handle, &cmd.auth_handle);
	al_memcpy(&cmd.nonce, nonce, AL_TPM_DIGEST_NUM_BYTES);
	cmd.continue_auth_session = continue_auth_session;
	al_memcpy(&cmd.owner_auth, owner_auth, AL_TPM_DIGEST_NUM_BYTES);

	err = al_tpm_cmd(
		tpm, (uint8_t *)&cmd, sizeof(cmd), (uint8_t *)&resp, sizeof(resp), result);
	if (err)
		return err;

	return 0;
}

int al_tpm_physical_presence_hw_enable(
	struct al_tpm		*tpm,
	enum al_tpm_result	*result)
{
	int err;
	struct __attribute__ ((__packed__)) {
		struct al_tpm_cmd_base	base;
		uint16_t		physical_presence;
	} cmd;
	struct __attribute__ ((__packed__)) {
		struct al_tpm_resp_base	base;
	} resp;

	al_assert(tpm);
	al_assert(result);
	if (tpm->tpm_2_0 == AL_TRUE)
		return 0; /* no physical presence in TPM 2.0 */

	cpu_to_tpm32(TPM_ORD_PHYSICALPRESENCE, &cmd.base.cmd_id);
	cpu_to_tpm16(TPM_PHYSICAL_PRESENCE_HW_ENABLE, &cmd.physical_presence);

	err = al_tpm_cmd(
		tpm, (uint8_t *)&cmd, sizeof(cmd), (uint8_t *)&resp, sizeof(resp), result);
	if (err)
		return err;

	return 0;
}

int al_tpm_physical_presence_cmd_enable(
	struct al_tpm		*tpm,
	enum al_tpm_result	*result)
{
	int err;
	struct __attribute__ ((__packed__)) {
		struct al_tpm_cmd_base	base;
		uint16_t		physical_presence;
	} cmd;
	struct __attribute__ ((__packed__)) {
		struct al_tpm_resp_base	base;
	} resp;

	al_assert(tpm);
	al_assert(result);
	if (tpm->tpm_2_0 == AL_TRUE)
		return 0; /* no physical presence in TPM 2.0 */

	cpu_to_tpm32(TPM_ORD_PHYSICALPRESENCE, &cmd.base.cmd_id);
	cpu_to_tpm16(TPM_PHYSICAL_PRESENCE_CMD_ENABLE, &cmd.physical_presence);

	err = al_tpm_cmd(
		tpm, (uint8_t *)&cmd, sizeof(cmd), (uint8_t *)&resp, sizeof(resp), result);
	if (err)
		return err;

	return 0;
}

static int al_tpm_physical_presence_set_present(
	struct al_tpm		*tpm,
	enum al_tpm_result	*result,
	int			value)
{
	int err;
	struct __attribute__ ((__packed__)) {
		struct al_tpm_cmd_base	base;
		uint16_t		physical_presence;
	} cmd;
	struct __attribute__ ((__packed__)) {
		struct al_tpm_resp_base	base;
	} resp;

	al_assert(tpm);
	al_assert(result);

	if (tpm->tpm_2_0 == AL_TRUE)
		return 0; /* no physical presence in TPM 2.0 */

	cpu_to_tpm32(TPM_ORD_PHYSICALPRESENCE, &cmd.base.cmd_id);
	cpu_to_tpm16(value, &cmd.physical_presence);

	err = al_tpm_cmd(
		tpm, (uint8_t *)&cmd, sizeof(cmd), (uint8_t *)&resp, sizeof(resp), result);
	if (err)
		return err;

	return 0;
}

int al_tpm_physical_presence_present(
	struct al_tpm		*tpm,
	enum al_tpm_result	*result)
{
	return al_tpm_physical_presence_set_present(tpm, result, TPM_PHYSICAL_PRESENCE_PRESENT);
}

int al_tpm_physical_presence_not_present(
	struct al_tpm		*tpm,
	enum al_tpm_result	*result)
{
	return al_tpm_physical_presence_set_present(tpm, result, TPM_PHYSICAL_PRESENCE_NOT_PRESENT);
}

int al_tpm_physical_enable(
	struct al_tpm		*tpm,
	enum al_tpm_result	*result)
{
	int err;
	struct __attribute__ ((__packed__)) {
		struct al_tpm_cmd_base	base;
	} cmd;
	struct __attribute__ ((__packed__)) {
		struct al_tpm_resp_base	base;
	} resp;

	al_assert(tpm);
	al_assert(result);
	if (tpm->tpm_2_0 == AL_TRUE)
		return 0; /* no physical presence in TPM 2.0 */

	cpu_to_tpm32(TPM_ORD_PHYSICALENABLE, &cmd.base.cmd_id);

	err = al_tpm_cmd(
		tpm, (uint8_t *)&cmd, sizeof(cmd), (uint8_t *)&resp, sizeof(resp), result);
	if (err)
		return err;

	return 0;
}

int al_tpm_physical_disable(
	struct al_tpm		*tpm,
	enum al_tpm_result	*result)
{
	int err;
	struct __attribute__ ((__packed__)) {
		struct al_tpm_cmd_base	base;
	} cmd;
	struct __attribute__ ((__packed__)) {
		struct al_tpm_resp_base	base;
	} resp;

	al_assert(tpm);
	al_assert(result);
	if (tpm->tpm_2_0 == AL_TRUE)
		return 0; /* no physical presence in TPM 2.0 */

	cpu_to_tpm32(TPM_ORD_PHYSICALDISABLE, &cmd.base.cmd_id);

	err = al_tpm_cmd(
		tpm, (uint8_t *)&cmd, sizeof(cmd), (uint8_t *)&resp, sizeof(resp), result);
	if (err)
		return err;

	return 0;
}

int al_tpm_physical_set_deactivated(
	struct al_tpm		*tpm,
	unsigned int		deactivated,
	enum al_tpm_result	*result)
{
	int err;
	struct __attribute__ ((__packed__)) {
		struct al_tpm_cmd_base	base;
		uint8_t			deactivated;
	} cmd;
	struct __attribute__ ((__packed__)) {
		struct al_tpm_resp_base	base;
	} resp;

	al_assert(tpm);
	al_assert(result);
	if (tpm->tpm_2_0 == AL_TRUE)
		return 0; /* no physical presence in TPM 2.0 */

	cpu_to_tpm32(TPM_ORD_PHYSICALSETDEACTIVATED, &cmd.base.cmd_id);
	cmd.deactivated = deactivated;

	err = al_tpm_cmd(
		tpm, (uint8_t *)&cmd, sizeof(cmd), (uint8_t *)&resp, sizeof(resp), result);
	if (err)
		return err;

	return 0;
}

int al_tpm_extend(
	struct al_tpm		*tpm,
	unsigned int		pcr_idx,
	uint8_t			*digest,
	enum al_tpm_result	*result,
	uint8_t			*out_digest)
{
	int err;
	struct __attribute__ ((__packed__)) {
		struct al_tpm_cmd_base	base;
		uint32_t		pcr_idx;
		uint8_t			digest[AL_TPM_DIGEST_NUM_BYTES];
	} cmd;
	struct __attribute__ ((__packed__)) {
		struct al_tpm_resp_base	base;
		uint8_t			out_digest[AL_TPM_DIGEST_NUM_BYTES];
	} resp;

	al_assert(tpm);
	al_assert(digest);
	al_assert(result);
	al_assert(out_digest);
	if (tpm->tpm_2_0 == AL_TRUE)
		return al_tpm2_extend(tpm, pcr_idx, digest, result, out_digest);

	cpu_to_tpm32(TPM_ORD_EXTEND, &cmd.base.cmd_id);
	cpu_to_tpm32(pcr_idx, &cmd.pcr_idx);
	al_memcpy(&cmd.digest, digest, AL_TPM_DIGEST_NUM_BYTES);
	err = al_tpm_cmd(
		tpm, (uint8_t *)&cmd, sizeof(cmd), (uint8_t *)&resp, sizeof(resp), result);
	if (err)
		return err;

	al_memcpy(out_digest, &resp.out_digest, AL_TPM_DIGEST_NUM_BYTES);

	return 0;
}

int al_tpm_pcr_read(
	struct al_tpm		*tpm,
	unsigned int		pcr_idx,
	enum al_tpm_result	*result,
	uint8_t			*out_digest)
{
	int err;
	struct __attribute__ ((__packed__)) {
		struct al_tpm_cmd_base	base;
		uint32_t		pcr_idx;
	} cmd;
	struct __attribute__ ((__packed__)) {
		struct al_tpm_resp_base	base;
		uint8_t			out_digest[AL_TPM_DIGEST_NUM_BYTES];
	} resp;

	al_assert(tpm);
	al_assert(result);
	al_assert(out_digest);
	if (tpm->tpm_2_0 == AL_TRUE)
		return al_tpm2_pcr_read(tpm, pcr_idx, result, out_digest);

	cpu_to_tpm32(TPM_ORD_PCRREAD, &cmd.base.cmd_id);
	cpu_to_tpm32(pcr_idx, &cmd.pcr_idx);
	err = al_tpm_cmd(
		tpm, (uint8_t *)&cmd, sizeof(cmd), (uint8_t *)&resp, sizeof(resp), result);
	if (err)
		return err;

	al_memcpy(out_digest, &resp.out_digest, AL_TPM_DIGEST_NUM_BYTES);

	return 0;
}

int al_tpm_pcr_reset(
	struct al_tpm		*tpm,
	unsigned int		pcr_idx __attribute__((unused)),
	enum al_tpm_result	*result)
{
	al_assert(tpm);
	al_assert(result);

	return 0;
}

int al_tpm_nv_read(
	struct al_tpm		*tpm,
	unsigned int		nv_idx,
	unsigned int		offset,
	unsigned int		data_size,
	enum al_tpm_result	*result,
	unsigned int		*actual_data_size,
	uint8_t			*data)
{
	int err;
	struct __attribute__ ((__packed__)) {
		struct al_tpm_cmd_base	base;
		uint32_t		nv_idx;
		uint32_t		offset;
		uint32_t		data_size;
	} cmd;
	struct __attribute__ ((__packed__)) {
		struct al_tpm_resp_base	base;
		uint32_t		data_size;
		uint8_t			data[AL_TPM_NV_MAX_READ_SIZE_NUM_BYTES];
	} resp;

	al_assert(tpm);
	al_assert(data_size);
	al_assert(result);
	al_assert(actual_data_size);
	al_assert(data);
	if (tpm->tpm_2_0 == AL_TRUE) {

		al_err("TPM 2.0 is not supported for this function\n");
		return -1;

	}

	cpu_to_tpm32(TPM_ORD_NV_READVALUE, &cmd.base.cmd_id);
	cpu_to_tpm32(nv_idx, &cmd.nv_idx);
	cpu_to_tpm32(offset, &cmd.offset);
	cpu_to_tpm32(data_size, &cmd.data_size);
	err = al_tpm_cmd(
		tpm, (uint8_t *)&cmd, sizeof(cmd), (uint8_t *)&resp,
		(sizeof(resp) - AL_TPM_NV_MAX_READ_SIZE_NUM_BYTES) + data_size, result);
	if (err)
		return err;

	*actual_data_size = cpu_from_tpm32(&resp.data_size);
	al_assert((*actual_data_size) == data_size);

	al_memcpy(data, resp.data, *actual_data_size);

	return 0;
}

int al_tpm_force_clear(
	struct al_tpm		*tpm,
	enum al_tpm_result	*result)
{
	int err;
	struct al_tpm_cmd_base	cmd;
	struct al_tpm_resp_base	resp;

	al_assert(tpm);
	al_assert(result);
	if (tpm->tpm_2_0 == AL_TRUE)
		return al_tpm2_force_clear(tpm, result);

	cpu_to_tpm32(TPM_ORD_FORCECLEAR, &cmd.cmd_id);
	err = al_tpm_cmd(
		tpm, (uint8_t *)&cmd, sizeof(cmd), (uint8_t *)&resp, sizeof(resp), result);
	if (err)
		return err;

	return 0;
}

int al_tpm_get_permanent_flags(
	struct al_tpm		*tpm,
	enum al_tpm_result	*result,
	struct tpm_permanent_flags *flags)
{
	int err;
	struct __attribute__ ((__packed__)) {
		struct al_tpm_cmd_base	base;
		uint32_t		cap_area;
		uint32_t		sub_cap_size;
		uint32_t		sub_cap;
	} cmd;
	struct __attribute__ ((__packed__)) {
		struct al_tpm_resp_base	base;
		uint32_t		data_size;
		uint16_t		tag;
		uint8_t			data[sizeof(struct tpm_permanent_flags)];
	} resp;
	unsigned int size;

	al_assert(tpm);
	al_assert(result);
	al_assert(flags);
	if (tpm->tpm_2_0 == AL_TRUE) {

		al_err("TPM 2.0 is not supported for this function\n");
		return -1;

	}

	cpu_to_tpm32(TPM_ORD_GET_CAPABILITY, &cmd.base.cmd_id);
	cpu_to_tpm32(TPM_CAP_FLAG, &cmd.cap_area);
	cpu_to_tpm32(sizeof(cmd.sub_cap), &cmd.sub_cap_size);
	cpu_to_tpm32(TPM_CAP_FLAG_PERMANENT, &cmd.sub_cap);

	err = al_tpm_cmd(
		tpm, (uint8_t *)&cmd, sizeof(cmd), (uint8_t *)&resp, sizeof(resp), result);
	if (err)
		return err;
	if (*result)
		al_err("al_get_capability failed. result = %x\n", *result);

	size = cpu_from_tpm32(&resp.data_size);
	if (size != sizeof(struct tpm_permanent_flags) + sizeof(uint16_t)) {
		al_err("%s: unexpected response size - %u\n", __func__, size);
		return -1;
	}

	al_memcpy((void *)(flags), resp.data, sizeof(struct tpm_permanent_flags));

	return 0;
}

int al_tpm_get_volatile_flags(
	struct al_tpm		*tpm,
	enum al_tpm_result	*result,
	struct tpm_volatile_flags *flags)
{
	int err;
	struct __attribute__ ((__packed__)) {
		struct al_tpm_cmd_base	base;
		uint32_t		cap_area;
		uint32_t		sub_cap_size;
		uint32_t		sub_cap;
	} cmd;
	struct __attribute__ ((__packed__)) {
		struct al_tpm_resp_base	base;
		uint32_t		data_size;
		uint16_t		tag;
		uint8_t			data[sizeof(struct tpm_volatile_flags)];
	} resp;
	unsigned int size;

	al_assert(tpm);
	al_assert(result);
	al_assert(flags);
	if (tpm->tpm_2_0 == AL_TRUE) {

		al_err("TPM 2.0 is not supported for this function\n");
		return -1;

	}

	cpu_to_tpm32(TPM_ORD_GET_CAPABILITY, &cmd.base.cmd_id);
	cpu_to_tpm32(TPM_CAP_FLAG, &cmd.cap_area);
	cpu_to_tpm32(sizeof(cmd.sub_cap), &cmd.sub_cap_size);
	cpu_to_tpm32(TPM_CAP_FLAG_VOLATILE, &cmd.sub_cap);

	err = al_tpm_cmd(
		tpm, (uint8_t *)&cmd, sizeof(cmd), (uint8_t *)&resp, sizeof(resp), result);
	if (err)
		return err;
	if (*result)
		al_err("al_get_capability failed. result = %x\n", *result);

	size = cpu_from_tpm32(&resp.data_size);
	if (size != sizeof(struct tpm_volatile_flags) + sizeof(uint16_t)) {
		al_err("%s: unexpected response size - %u\n", __func__, size);
		return -1;
	}

	al_memcpy((void *)(flags), resp.data, sizeof(struct tpm_volatile_flags));

	return 0;
}

int al_tpm_get_version(
	struct al_tpm		*tpm,
	enum al_tpm_result	*result,
	struct tpm_cap_version_info *version_info){

	int err;
	struct __attribute__ ((__packed__)) {
		struct al_tpm_cmd_base	base;
		uint32_t		cap_area;
		uint32_t		sub_cap_size;
		uint32_t		sub_cap;
	} cmd;

	struct __attribute__ ((__packed__)) {
		struct al_tpm_resp_base	base;
		uint32_t		data_size;
		uint8_t			data[sizeof(struct tpm_cap_version_info)];
	} resp;
	unsigned int size;

	al_assert(tpm);
	al_assert(result);
	al_assert(version_info);
	if (tpm->tpm_2_0 == AL_TRUE) {

		al_err("TPM 2.0 is not supported for this function\n");
		return -1;

	}

	cpu_to_tpm32(TPM_ORD_GET_CAPABILITY, &cmd.base.cmd_id);
	cpu_to_tpm32(TPM_CAP_VERSION_VAL, &cmd.cap_area);
	cpu_to_tpm32(sizeof(cmd.sub_cap_size), &cmd.sub_cap_size);

	err = al_tpm_cmd_resp_unknown_size(
		tpm, (uint8_t *)&cmd, sizeof(cmd), AL_FALSE, (uint8_t *)&resp,
		sizeof(resp), result);
	if (err)
		return err;
	if (*result) {
		al_err("al_get_capability failed. result = %x\n", *result);
		return -1;
	}

	size = cpu_from_tpm32(&resp.data_size);
	if ((size > sizeof(resp.data)) || (size > sizeof(struct tpm_cap_version_info))) {
		al_err("%s: unexpected response size - %u\n", __func__, size);
		return -1;
	}

	al_memcpy((void *)(version_info), resp.data, size);

	uint16_t tag = cpu_from_tpm16(&version_info->tag);
	if (tag != TPM_TAG_CAP_VERSION_INFO) {
		al_err("%s: unexpected response tag - %u\n", __func__, tag);
		return -1;
	}

	version_info->tag = tag;
	version_info->spec_level = cpu_from_tpm16(&version_info->spec_level);
	version_info->vendor_specific_size =
		cpu_from_tpm16(&version_info->vendor_specific_size);

	return 0;
}

int al_tpm2_get_capability(
	struct al_tpm		*tpm,
	enum al_tpm_result	*result,
	uint32_t		capability,
	uint32_t		property,
	uint32_t		*data)
{
	int err;

	struct __attribute__ ((__packed__)) {
		struct al_tpm_cmd_base	base;
		uint32_t	capability;
		uint32_t	property;
		uint32_t	property_cnt;
	} cmd;

	struct __attribute__ ((__packed__)) tpm2_property {
		uint32_t	property;
		uint32_t	value;
	};

	struct __attribute__ ((__packed__)) tpm2_capability_data {
		uint32_t		capability;
		uint32_t		property_count;
		struct tpm2_property	tpm_property;
	};

	struct __attribute__ ((__packed__)) {
		struct al_tpm_resp_base		base;
		uint8_t				more_data;
		struct tpm2_capability_data	cap_data;
	} resp;

	al_assert(tpm);
	al_assert(result);
	al_assert(data);

	cpu_to_tpm16(TPM2_ST_NO_SESSIONS, &cmd.base.tag);
	cpu_to_tpm32(TPM2_CC_GET_CAPABILITY, &cmd.base.cmd_id);
	cpu_to_tpm32(capability, &cmd.capability);
	cpu_to_tpm32(property, &cmd.property);
	cpu_to_tpm32(1, &cmd.property_cnt);

	err = al_tpm_cmd_resp_unknown_size(tpm, (uint8_t *)&cmd, sizeof(cmd),
		AL_FALSE, (uint8_t *)&resp, sizeof(resp), result);
	if (err)
		return err;

	*data = cpu_from_tpm32(&resp.cap_data.tpm_property.value);

	return 0;
}

int al_tpm2_get_random(
	struct al_tpm		*tpm,
	enum al_tpm_result	*result,
	uint8_t			bytes_requested,
	uint8_t			*random_bytes)
{
	int err;

	struct __attribute__ ((__packed__)) {
			struct al_tpm_cmd_base	base;
			uint16_t		bytes_requested;
	} cmd;

	struct __attribute__ ((__packed__)) {
		struct al_tpm_resp_base		base;
		uint8_t				random_bytes[TPM2_MAX_RANDOM_BUF_SIZE];
	} resp;

	al_assert(tpm);
	al_assert(bytes_requested <= TPM2_MAX_RANDOM_BUF_SIZE);

	cpu_to_tpm32(TPM2_CC_GET_RANDOM, &cmd.base.cmd_id);
	cpu_to_tpm16(TPM2_ST_NO_SESSIONS, &cmd.base.tag);
	cpu_to_tpm32(sizeof(cmd), &cmd.base.len);
	cpu_to_tpm16(bytes_requested, &cmd.bytes_requested);

	err = al_tpm_cmd_resp_unknown_size(tpm, (uint8_t *)&cmd, sizeof(cmd),
		AL_FALSE, (uint8_t *)&resp, sizeof(resp), result);
	if (err)
		return err;

	al_memcpy(random_bytes, resp.random_bytes, bytes_requested);

	return 0;
}

int al_tpm2_nv_read_cert(
	struct al_tpm		*tpm,
	unsigned int		nv_idx,
	unsigned int		data_size,
	enum al_tpm_result	*result,
	unsigned int		*actual_data_size,
	uint8_t			*data)
{
	int err;
	uint16_t size;
	uint16_t offset = 0;
	uint32_t tmp;
	uint8_t *buffer;

	/* Command and response defs*/
	struct __attribute__ ((__packed__)) {
		struct al_tpm_cmd_base	base;
		uint32_t		nv_idx;
	} cmd1;
	struct __attribute__ ((__packed__)) {
		struct al_tpm_resp_base	base;
		uint8_t			buffer[TPM2_MAX_NV_BUFFER_SIZE];
	} resp1;

	struct __attribute__ ((__packed__)) nv_auth_command {
		uint32_t		nv_session_handle;
		uint16_t		nv_auth_nonce;
		uint8_t			nv_auth_session_attr;
		uint16_t		nv_auth_hmac;
	};

	struct __attribute__ ((__packed__)) {
		struct al_tpm_cmd_base	base;
		uint32_t		nv_auth_handle;
		uint32_t		nv_idx;
		uint32_t		nv_auth_session_size;
		struct nv_auth_command	nv_auth_session;
		uint16_t		size;
		uint16_t		offset;
	} cmd2;
	struct __attribute__ ((__packed__)) {
		struct al_tpm_resp_base	base;
		uint32_t		auth_session_size;
		uint16_t		size;
		uint8_t			buffer[TPM2_MAX_NV_BUFFER_SIZE];
	} resp2;

	uint16_t bytes_to_read = TPM2_MAX_NV_BUFFER_SIZE - sizeof(cmd2);

	al_assert(tpm);
	al_assert(result);
	al_assert(actual_data_size);
	al_assert(data);

	cpu_to_tpm16(TPM2_ST_NO_SESSIONS, &cmd1.base.tag);
	cpu_to_tpm32(TPM2_CC_NV_READPUBLIC, &cmd1.base.cmd_id);
	cpu_to_tpm32(nv_idx, &cmd1.nv_idx);

	err = al_tpm_cmd_resp_unknown_size(tpm, (uint8_t *)&cmd1, sizeof(cmd1), AL_FALSE,
		(uint8_t *)&resp1, sizeof(resp1), result);
	if (err)
		return err;

	/* parse response */
	tmp = cpu_from_tpm16((uint16_t *)&resp1.buffer[0]);
	buffer = &resp1.buffer[0];
	buffer += sizeof(uint16_t);

	buffer += tmp - sizeof(uint16_t);

	size = cpu_from_tpm16((uint16_t *)buffer);

	buffer += sizeof(uint16_t);
	tmp = cpu_from_tpm16((uint16_t *)buffer);

	/* Response should be SHA256_DIGEST_SIZE + 2 bytes of config header */
	if (tmp != SHA256_DIGEST_SIZE + 2)
		return -1;

	/* set up nv read cmd */
	cpu_to_tpm16(TPM2_ST_SESSIONS, &cmd2.base.tag);
	cpu_to_tpm32(TPM2_CC_NV_READ, &cmd2.base.cmd_id);
	cpu_to_tpm32(TPM2_RH_OWNER, &cmd2.nv_auth_handle);
	cpu_to_tpm32(sizeof(struct nv_auth_command), &cmd2.nv_auth_session_size);

	cpu_to_tpm32(TPM2_RS_PW, &cmd2.nv_auth_session.nv_session_handle);
	cmd2.nv_auth_session.nv_auth_nonce = 0;
	cmd2.nv_auth_session.nv_auth_session_attr = 0;
	cmd2.nv_auth_session.nv_auth_hmac = 0;
	cpu_to_tpm32(nv_idx, &cmd2.nv_idx);

	*actual_data_size = 0;
	offset = 0;
	do {
		cpu_to_tpm16(offset, &cmd2.offset);
		if (offset + bytes_to_read > size)
			bytes_to_read = size - offset;
		cpu_to_tpm16(bytes_to_read, &cmd2.size);

		err = al_tpm_cmd_resp_unknown_size(tpm, (uint8_t *)&cmd2, sizeof(cmd2), AL_FALSE,
			(uint8_t *)&resp2, sizeof(resp2), result);
		if (err)
			return err;

		bytes_to_read = cpu_from_tpm16(&resp2.size);
		if (bytes_to_read == 0)
			break;

		if (data_size < (*actual_data_size + bytes_to_read))
			return -1;

		al_memcpy(&data[*actual_data_size], resp2.buffer, bytes_to_read);
		*actual_data_size += bytes_to_read;
		offset += bytes_to_read;
	} while (offset < size);

	return 0;
}
