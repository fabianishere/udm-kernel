/*
 * Copyright 2018, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#include "al_hal_common.h"
#include "al_tpm.h"


enum tis_access {
	TPM_ACCESS_VALID = 0x80,
	TPM_ACCESS_ACTIVE_LOCALITY = 0x20,
	TPM_ACCESS_REQUEST_PENDING = 0x04,
	TPM_ACCESS_REQUEST_USE = 0x02,
};

enum tis_status {
	TPM_STS_VALID = 0x80,
	TPM_STS_COMMAND_READY = 0x40,
	TPM_STS_GO = 0x20,
	TPM_STS_DATA_AVAIL = 0x10,
	TPM_STS_DATA_EXPECT = 0x08,
};

#define TPM_MAX_RNG_DATA	128
#define TPM_DIGEST_SIZE 20
#define TPM_HEADER_SIZE		10
#define	TPM_ACCESS(l)			(0x0000 | ((l) << 12))
#define	TPM_INT_ENABLE(l)		(0x0008 | ((l) << 12))
#define	TPM_INT_VECTOR(l)		(0x000C | ((l) << 12))
#define	TPM_INT_STATUS(l)		(0x0010 | ((l) << 12))
#define	TPM_INTF_CAPS(l)		(0x0014 | ((l) << 12))
#define	TPM_STS(l)			(0x0018 | ((l) << 12))
#define	TPM_STS3(l)			(0x001b | ((l) << 12))
#define	TPM_DATA_FIFO(l)		(0x0024 | ((l) << 12))

#define	TPM_DID_VID(l)			(0x0F00 | ((l) << 12))
#define	TPM_RID(l)			(0x0F04 | ((l) << 12))
#define TPM_TIMEOUT				1000 /*1 second*/

#define TPM_DEFAULT_LOCALITY 0
enum tpm2_const {
	TPM2_PLATFORM_PCR	= 24,
	TPM2_PCR_SELECT_MIN	= ((TPM2_PLATFORM_PCR + 7) / 8),
	TPM2_TIMEOUT_A		= 750,
	TPM2_TIMEOUT_B		= 2000,
	TPM2_TIMEOUT_C		= 200,
	TPM2_TIMEOUT_D		= 30,
	TPM2_DURATION_SHORT	= 20,
	TPM2_DURATION_MEDIUM	= 750,
	TPM2_DURATION_LONG	= 2000,
};


struct tpm2_startup_in {
	uint16_t	startup_type;
} __packed;

struct tpm2_self_test_in {
	uint8_t	full_test;
} __packed;

struct tpm2_pcr_read_in {
	uint32_t	pcr_selects_cnt;
	uint16_t	hash_alg;
	uint8_t	pcr_select_size;
	uint8_t	pcr_select[TPM2_PCR_SELECT_MIN];
} __packed;

struct tpm2_pcr_read_out {
	uint32_t	update_cnt;
	uint32_t	pcr_selects_cnt;
	uint16_t	hash_alg;
	uint8_t	pcr_select_size;
	uint8_t	pcr_select[TPM2_PCR_SELECT_MIN];
	uint32_t	digests_cnt;
	uint16_t	digest_size;
	uint8_t	digest[TPM_DIGEST_SIZE];
} __packed;

struct tpm2_null_auth_area {
	uint32_t			handle;
	uint16_t			nonce_size;
	uint8_t			attributes;
	uint16_t			auth_size;
} __packed;

struct tpm2_pcr_extend_in {
	uint32_t				pcr_idx;
	uint32_t				auth_area_size;
	struct tpm2_null_auth_area	auth_area;
	uint32_t				digest_cnt;
	uint16_t				hash_alg;
	uint8_t				digest[TPM_DIGEST_SIZE];
} __packed;

struct tpm2_get_tpm_pt_in {
	uint32_t	cap_id;
	uint32_t	property_id;
	uint32_t	property_cnt;
} __packed;

struct tpm2_get_tpm_pt_out {
	uint8_t	more_data;
	uint32_t	subcap_id;
	uint32_t	property_cnt;
	uint32_t	property_id;
	uint32_t	value;
} __packed;

struct tpm2_get_random_in {
	uint16_t	size;
} __packed;

struct tpm2_get_random_out {
	uint16_t	size;
	uint8_t	buffer[TPM_MAX_RNG_DATA];
} __packed;

union tpm2_cmd_params {
	struct	tpm2_startup_in		startup_in;
	struct	tpm2_self_test_in	selftest_in;
	struct	tpm2_pcr_read_in	pcrread_in;
	struct	tpm2_pcr_read_out	pcrread_out;
	struct	tpm2_pcr_extend_in	pcrextend_in;
	struct	tpm2_get_tpm_pt_in	get_tpm_pt_in;
	struct	tpm2_get_tpm_pt_out	get_tpm_pt_out;
	struct	tpm2_get_random_in	getrandom_in;
	struct	tpm2_get_random_out	getrandom_out;
};

struct tpm_input_header {
	uint16_t	tag;
	uint32_t	length;
	uint32_t	ordinal;
} __packed;

struct tpm_output_header {
	uint16_t	tag;
	uint32_t	length;
	uint32_t	return_code;
} __packed;

struct tpm2_cmd {
	union {
	struct	tpm_input_header in;
	struct	tpm_output_header out;
	} header;
	union tpm2_cmd_params	params;
} __packed;

enum tpm2_startup_types {
	TPM2_SU_CLEAR	= 0x0000,
	TPM2_SU_STATE	= 0x0001,
};

enum tpm2_start_method {
	TPM2_START_ACPI = 2,
	TPM2_START_FIFO = 6,
	TPM2_START_CRB = 7,
	TPM2_START_CRB_WITH_ACPI = 8,
};

enum tpm2_structures {
	TPM2_ST_NO_SESSIONS	= 0x8001,
	TPM2_ST_SESSIONS	= 0x8002,
};

enum tpm2_return_codes {
	TPM2_RC_INITIALIZE	= 0x0100,
	TPM2_RC_TESTING		= 0x090A,
	TPM2_RC_DISABLED	= 0x0120,
};

enum tpm2_command_codes {
	TPM2_CC_FIRST		= 0x011F,
	TPM2_CC_SELF_TEST	= 0x0143,
	TPM2_CC_STARTUP		= 0x0144,
	TPM2_CC_SHUTDOWN	= 0x0145,
	TPM2_CC_CREATE		= 0x0153,
	TPM2_CC_LOAD		= 0x0157,
	TPM2_CC_UNSEAL		= 0x015E,
	TPM2_CC_FLUSH_CONTEXT	= 0x0165,
	TPM2_CC_GET_CAPABILITY	= 0x017A,
	TPM2_CC_GET_RANDOM	= 0x017B,
	TPM2_CC_PCR_READ	= 0x017E,
	TPM2_CC_PCR_EXTEND	= 0x0182,
	TPM2_CC_LAST		= 0x018F,
};

enum tpm2_permanent_handles {
	TPM2_RS_PW		= 0x40000009,
};

#define TPM2_GET_TPM_PT_IN_SIZE \
	(sizeof(struct tpm_input_header) + \
	 sizeof(struct tpm2_get_tpm_pt_in))

#define TPM2_GET_RANDOM_IN_SIZE \
	(sizeof(struct tpm_input_header) + \
	 sizeof(struct tpm2_get_random_in))

void al_tpm_tis20_handle_init(
	struct al_tpm_tis20_handle	*handle,
	struct al_tpm_if		*tpm_if)
{
	al_assert(tpm_if);
	al_memset(handle, 0, sizeof(struct al_tpm_tis20_handle));
	handle->tpm_if = tpm_if;
}

static int al_tpm_tis20_wait_startup(struct al_tpm_tis20_handle *handle, int l)
{
	unsigned char access;
	unsigned int tmo = TPM_TIMEOUT;
	while (tmo-- > 0) {
		access =
			handle->tpm_if->read1(handle->tpm_if, TPM_ACCESS(l));

		if (access & TPM_ACCESS_VALID)
			return 0;
		al_udelay(10000);
	}
	return -1;
}

static int al_tpm_tis20_check_locality(struct al_tpm_tis20_handle *handle, int l)
{
	unsigned char access;
	access =
			handle->tpm_if->read1(handle->tpm_if, TPM_ACCESS(l));

	if ((access & (TPM_ACCESS_ACTIVE_LOCALITY | TPM_ACCESS_VALID)) ==
	    (TPM_ACCESS_ACTIVE_LOCALITY | TPM_ACCESS_VALID))

		return handle->locality = l;

	return -1;
}

static int al_tpm_tis20_request_locality(struct al_tpm_tis20_handle *handle, int l)
{
	unsigned int tmo = TPM_TIMEOUT;
	if (al_tpm_tis20_check_locality(handle, l) >= 0)
		return l;

	handle->tpm_if->write1(handle->tpm_if, TPM_ACCESS(l), TPM_ACCESS_REQUEST_USE);
	while (tmo-- > 0) {
		if (al_tpm_tis20_check_locality(handle, l) >= 0)
			return l;
		al_udelay(10000);
	}
	return -1;
}

static int al_tpm_tis20_get_burstcount(struct al_tpm_tis20_handle *handle)
{
	int burstcnt;
	burstcnt = handle->tpm_if->read1(handle->tpm_if, TPM_STS(0) + 1) +
	(handle->tpm_if->read1(handle->tpm_if, TPM_STS(0) + 2) << 8);
	/*SPI frame limit*/
	return (burstcnt < TPM_MAX_COMMAND_SIZE) ? burstcnt : TPM_MAX_COMMAND_SIZE;

}

static void  al_tpm_tis20_release_locality(struct al_tpm_tis20_handle *handle, int l, int force)
{
	unsigned char access;
	access =  handle->tpm_if->read1(handle->tpm_if, TPM_ACCESS(l));
	if (force ||
	    (access & (TPM_ACCESS_REQUEST_PENDING | TPM_ACCESS_VALID)) ==
	    (TPM_ACCESS_REQUEST_PENDING | TPM_ACCESS_VALID))
		handle->tpm_if->write1(handle->tpm_if, TPM_ACCESS(l), TPM_ACCESS_ACTIVE_LOCALITY);
}

static unsigned char  al_tpm_tis20_tpm_tis_status(struct al_tpm_tis20_handle *handle)
{
	return handle->tpm_if->read1(handle->tpm_if, TPM_STS(0));
}

static void al_tpm_tis20_tpm_tis_ready(struct al_tpm_tis20_handle *handle)
{
	/* this causes the current command to be aborted */
	handle->tpm_if->write1(handle->tpm_if, TPM_STS(0), TPM_STS_COMMAND_READY);
}

static int al_tpm_tis20_wait_for_tpm_stat(struct al_tpm_tis20_handle *handle, unsigned char mask)
{
	unsigned char status;
	unsigned int tmo = TPM_TIMEOUT;
	do {
		al_udelay(10000);
		status = al_tpm_tis20_tpm_tis_status(handle);
	} while (tmo-- > 0 && ((status & mask) != mask));
	if (tmo <= 0)
		al_err("al_tpm_tis20_wait_for_tpm_stat:	Timeout expired\n");
	return (status & mask) ? status & mask : 0;
}

static int al_tpm_tis20_tpm_send(struct al_tpm_tis20_handle *handle,
			unsigned char *buf, size_t len)
{
	unsigned char status;
	size_t count = 0, burstcnt, transfer_size;

	if (al_tpm_tis20_request_locality(handle, TPM_DEFAULT_LOCALITY) < 0)
		return -1;

	status = al_tpm_tis20_tpm_tis_status(handle);
	if ((status & TPM_STS_COMMAND_READY) == 0) {
		al_tpm_tis20_tpm_tis_ready(handle);
		status = al_tpm_tis20_wait_for_tpm_stat(handle, TPM_STS_COMMAND_READY);
		if (status == 0)
			goto err;
	}

	burstcnt = al_tpm_tis20_get_burstcount(handle);

	while (count < len - 1) {
		transfer_size = (len - count - 1 < burstcnt) ? len - count - 1 : burstcnt;
		handle->tpm_if->write(handle->tpm_if,  TPM_DATA_FIFO(0),
			&buf[count], transfer_size);
		count +=  transfer_size;
		al_tpm_tis20_wait_for_tpm_stat(handle, TPM_STS_VALID);
		status = al_tpm_tis20_tpm_tis_status(handle);
		if ((status & TPM_STS_DATA_EXPECT) == 0) {
			al_err(" al_tpm_tis20_tpm_send:  TPM_STS_DATA_EXPECT is down\n");
			goto err;
		}
	}
	/* write last byte */
	handle->tpm_if->write1(handle->tpm_if, TPM_DATA_FIFO(0), buf[count]);
	/* Execute the command */
	handle->tpm_if->write1(handle->tpm_if, TPM_STS(0), TPM_STS_GO);
	return len;
err:
	al_tpm_tis20_tpm_tis_ready(handle);
	al_tpm_tis20_release_locality(handle, 0, 0);
	return -1;
}

static int al_tpm_tis20_recv_data(struct al_tpm_tis20_handle *handle,
			unsigned char *buf, size_t count)
{
	size_t size = 0, burstcnt, transfer_size;

	while (size < count &&
	       al_tpm_tis20_wait_for_tpm_stat(handle, TPM_STS_DATA_AVAIL | TPM_STS_VALID)) {
		burstcnt = al_tpm_tis20_get_burstcount(handle);
		transfer_size = (count - size < burstcnt) ? count - size : burstcnt;
		handle->tpm_if->read(handle->tpm_if, TPM_DATA_FIFO(0), &buf[size], transfer_size);
		size += transfer_size;
	}
	return size;
}

/* Setup TPM using TIS 2.0 interface. */
int al_tpm_tis20_init(struct al_tpm_tis20_handle *handle)
{
	if (al_tpm_tis20_wait_startup(handle, TPM_DEFAULT_LOCALITY)) {
		al_err("TPM 2.0 failed to detect correct startup conditions\n");
		return -1;
	}
	return (al_tpm_tis20_request_locality(handle, TPM_DEFAULT_LOCALITY)
		== TPM_DEFAULT_LOCALITY) ? 0 : -1;
}

/* Vendor id and device id get */
void al_tpm_tis20_vid_did_get(struct al_tpm_tis20_handle *handle,
			unsigned int *vid, unsigned int *did)
{
	*vid = (handle->tpm_if->read1(handle->tpm_if, TPM_DID_VID(0) + 1) << 8) |
		handle->tpm_if->read1(handle->tpm_if, TPM_DID_VID(0) + 0);
	*did = (handle->tpm_if->read1(handle->tpm_if, TPM_DID_VID(0) + 3) << 8) |
		handle->tpm_if->read1(handle->tpm_if, TPM_DID_VID(0) + 2);
}

/* Start transaction. */
int al_tpm_tis20_start(struct al_tpm_tis20_handle *handle, al_bool is_write)
{
	unsigned char status;
	if (!is_write) {
		status = al_tpm_tis20_wait_for_tpm_stat(handle, TPM_STS_DATA_AVAIL | TPM_STS_VALID);
		return (status == 0) ? -1 : 0;
	}

	if (al_tpm_tis20_request_locality(handle, TPM_DEFAULT_LOCALITY) != TPM_DEFAULT_LOCALITY)
		return -1;
	status = al_tpm_tis20_tpm_tis_status(handle);
	if (status & TPM_STS_COMMAND_READY)
		return 0;
	/* Abort previous and restart. */
	handle->tpm_if->write1(handle->tpm_if, TPM_STS(TPM_DEFAULT_LOCALITY),
		TPM_STS_COMMAND_READY);
	status = al_tpm_tis20_wait_for_tpm_stat(handle, TPM_STS_COMMAND_READY);
	if (status == 0)
		return -1;
	return 0;
}

int al_tpm_tis20_read(struct al_tpm_tis20_handle *handle, void *buf, int len, int *count)
{
	*count = al_tpm_tis20_recv_data(handle, buf, len);
	return (*count < 0) ? -1 : 0;
}

int al_tpm_tis20_write(struct al_tpm_tis20_handle *handle, void *buf, int len)
{
	return (al_tpm_tis20_tpm_send(handle, buf, len) == len) ? 0 : -1;
}

/* Finish transaction. */
int al_tpm_tis20_end(struct al_tpm_tis20_handle *handle, al_bool is_write, al_bool is_err)
{
	unsigned char status;

	if (!is_write) {
			status = al_tpm_tis20_wait_for_tpm_stat(handle, TPM_STS_VALID);
			if (status == 0)
				return -1;
			status = al_tpm_tis20_tpm_tis_status(handle);
			if (status & TPM_STS_DATA_AVAIL) {
				al_err("al_tpm_tis20_end: read failed\n");
				return -1;
			}
			handle->tpm_if->write1(handle->tpm_if, TPM_STS(TPM_DEFAULT_LOCALITY),
				TPM_STS_COMMAND_READY);
			al_tpm_tis20_release_locality(handle, TPM_DEFAULT_LOCALITY, 0);
	} else {
			status = al_tpm_tis20_tpm_tis_status(handle);
			if (status & TPM_STS_DATA_EXPECT) {
				al_err("al_tpm_tis20_end: write failed\n");
				/*abort*/
				handle->tpm_if->write1(handle->tpm_if,
					TPM_STS(TPM_DEFAULT_LOCALITY),
					TPM_STS_COMMAND_READY);
				return -1;
			}
	}
	if (is_err)
		al_dbg("al_tpm_tis20_end: error\n");
	return 0;
}
