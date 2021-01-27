/*
 * Copyright (c) 2008, 2009 Michael Shalayeff
 * Copyright (c) 2009, 2010 Hans-Joerg Hoexer
 * Copyright (C) 2016 Annapurna Labs Ltd.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF MIND, USE, DATA OR PROFITS, WHETHER IN
 * AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#include "al_tpm_tis12.h"
#include "al_hal_common.h"

#define BURST_RW	1

#define	TPM_BUFSIZ	1024

#define TPM_HDRSIZE	10

#define TPM_PARAM_SIZE	0x0001

#define	TPM_ACCESS			0x0000	/* acess register */
#define	TPM_ACCESS_ESTABLISHMENT	0x01	/* establishment */
#define	TPM_ACCESS_REQUEST_USE		0x02	/* request using locality */
#define	TPM_ACCESS_REQUEST_PENDING	0x04	/* pending request */
#define	TPM_ACCESS_SEIZE		0x08	/* request locality seize */
#define	TPM_ACCESS_SEIZED		0x10	/* locality has been seized */
#define	TPM_ACCESS_ACTIVE_LOCALITY	0x20	/* locality is active */
#define	TPM_ACCESS_VALID		0x80	/* bits are valid */

#define	TPM_INTERRUPT_ENABLE	0x0008
#define	TPM_GLOBAL_INT_ENABLE	0x80000000	/* enable ints */
#define	TPM_CMD_READY_INT	0x00000080	/* cmd ready enable */
#define	TPM_INT_EDGE_FALLING	0x00000018
#define	TPM_INT_EDGE_RISING	0x00000010
#define	TPM_INT_LEVEL_LOW	0x00000008
#define	TPM_INT_LEVEL_HIGH	0x00000000
#define	TPM_LOCALITY_CHANGE_INT	0x00000004	/* locality change enable */
#define	TPM_STS_VALID_INT	0x00000002	/* int on TPM_STS_VALID is set */
#define	TPM_DATA_AVAIL_INT	0x00000001	/* int on TPM_STS_DATA_AVAIL is set */

#define	TPM_STS			0x0018		/* status register */
#define TPM_STS_MASK		0x000000ff	/* status bits */
#define	TPM_STS_BMASK		0x00ffff00	/* ro io burst size */
#define	TPM_STS_VALID		0x00000080	/* ro other bits are valid */
#define	TPM_STS_CMD_READY	0x00000040	/* rw chip/signal ready */
#define	TPM_STS_GO		0x00000020	/* wo start the command */
#define	TPM_STS_DATA_AVAIL	0x00000010	/* ro data available */
#define	TPM_STS_DATA_EXPECT	0x00000008	/* ro more data to be written */
#define	TPM_STS_RESP_RETRY	0x00000002	/* wo resend the response */

#define	TPM_DATA	0x0024
#define	TPM_ID		0x0f00

#define	TPM_ACCESS_TMO	2000		/* 2sec */
#define	TPM_READY_TMO	2000		/* 2sec */
#define	TPM_READ_TMO	120000		/* 2 minutes */
#define TPM_BURST_TMO	2000		/* 2sec */

static int tpm_request_locality(struct al_tpm_tis12_handle *handle, int l)
{
	uint32_t r;
	int to;

	if (l != 0)
		return -EINVAL;

	if ((handle->tpm_if->read1(handle->tpm_if, TPM_ACCESS) &
	    (TPM_ACCESS_VALID | TPM_ACCESS_ACTIVE_LOCALITY)) ==
	    (TPM_ACCESS_VALID | TPM_ACCESS_ACTIVE_LOCALITY))
		return 0;

	handle->tpm_if->write1(handle->tpm_if, TPM_ACCESS,
	    TPM_ACCESS_REQUEST_USE);

	to = TPM_ACCESS_TMO;

	while ((r = handle->tpm_if->read1(handle->tpm_if, TPM_ACCESS) &
	    (TPM_ACCESS_VALID | TPM_ACCESS_ACTIVE_LOCALITY)) !=
	    (TPM_ACCESS_VALID | TPM_ACCESS_ACTIVE_LOCALITY) && to--) {
		al_msleep(1);
	}

	if ((r & (TPM_ACCESS_VALID | TPM_ACCESS_ACTIVE_LOCALITY)) !=
	    (TPM_ACCESS_VALID | TPM_ACCESS_ACTIVE_LOCALITY)) {
		al_dbg("tpm_request_locality: access %x\n", r);
		return -EBUSY;
	}

	return 0;
}

static int tpm_getburst(struct al_tpm_tis12_handle *handle)
{
	int burst, to;

	to = TPM_BURST_TMO;

	burst = 0;
	while (burst == 0 && to--) {
		/*
		 * Burst count has to be read from bits 8 to 23 without
		 * touching any other bits, eg. the actual status bits 0
		 * to 7.
		 */
		burst = handle->tpm_if->read1(handle->tpm_if, TPM_STS + 1);
		burst |= handle->tpm_if->read1(handle->tpm_if, TPM_STS + 2)
		    << 8;
		al_dbg("tpm_getburst: read %d\n", burst);
		if (burst)
			return burst;

		al_msleep(1);
	}

	return 0;
}

static uint8_t tpm_status(struct al_tpm_tis12_handle *handle)
{
	uint8_t status;

	status = handle->tpm_if->read1(handle->tpm_if, TPM_STS) &
	    TPM_STS_MASK;

	al_dbg("%s: status = %02x\n", __func__, status);

	return status;
}

/* Wait for given status bits using polling. */
static int tpm_waitfor_poll(struct al_tpm_tis12_handle *handle, uint8_t mask, int tmo)
{
	/*
	 * Poll until either the requested condition or a time out is
	 * met.
	 */
	while (((handle->stat = tpm_status(handle)) & mask) != mask && tmo--)
		al_msleep(1);

	if (!tmo)
		return -ETIME;

	return 0;
}

/*
 * Wait on given status bits, uses interrupts where possible, otherwise polls.
 */
static int tpm_waitfor(struct al_tpm_tis12_handle *handle, uint8_t b0, int tmo)
{
	uint8_t b;
	int re, to, rv;

	al_dbg("tpm_waitfor: b0 %02x\n", b0);

	re = 3;
restart:
	/*
	 * If requested wait for TPM_STS_VALID before dealing with
	 * any other flag.  Eg. when both TPM_STS_DATA_AVAIL and TPM_STS_VALID
	 * are requested, wait for the latter first.
	 */
	b = b0;
	if (b0 & TPM_STS_VALID)
		b = TPM_STS_VALID;

again:
	rv = tpm_waitfor_poll(handle, b, tmo);
	if (rv)
		return rv;

	if ((b & handle->stat) == TPM_STS_VALID) {
		/* Now wait for other flags. */
		b = b0 & ~TPM_STS_VALID;
		to++;
		goto again;
	}

	if ((handle->stat & b) != b) {
		al_dbg("tpm_waitfor: timeout: stat=%02x b=%02x\n", handle->stat, b);
		if (re-- && (b0 & TPM_STS_VALID)) {
			handle->tpm_if->write1(handle->tpm_if, TPM_STS,
			    TPM_STS_RESP_RETRY);
			goto restart;
		}
		return -EIO;
	}

	return 0;
}

void al_tpm_tis12_handle_init(
	struct al_tpm_tis12_handle	*handle,
	struct al_tpm_if		*tpm_if)
{
	al_assert(tpm_if);
	al_memset(handle, 0, sizeof(struct al_tpm_tis12_handle));
	handle->tpm_if = tpm_if;
}

void al_tpm_tis12_id_get(
	struct al_tpm_tis12_handle	*handle,
	unsigned int			*vid,
	unsigned int			*did)
{
	*vid = (handle->tpm_if->read1(handle->tpm_if, TPM_ID + 1) << 8) |
		handle->tpm_if->read1(handle->tpm_if, TPM_ID + 0);
	*did = (handle->tpm_if->read1(handle->tpm_if, TPM_ID + 3) << 8) |
		handle->tpm_if->read1(handle->tpm_if, TPM_ID + 2);
}

/* Setup TPM using TIS 1.2 interface. */
int al_tpm_tis12_init(struct al_tpm_tis12_handle *handle)
{
	int rv;

	rv = tpm_request_locality(handle, 0);
	if (rv)
		return rv;

	/* Abort whatever it thought it was doing. */
	handle->tpm_if->write1(handle->tpm_if, TPM_STS, TPM_STS_CMD_READY);

	return 0;
}

/* Start transaction. */
int al_tpm_tis12_start(struct al_tpm_tis12_handle *handle, al_bool is_write)
{
	int rv;

	if (!is_write) {
		rv = tpm_waitfor(handle, TPM_STS_DATA_AVAIL | TPM_STS_VALID, TPM_READ_TMO);
		return rv;
	}

	/* Own our (0th) locality. */
	rv = tpm_request_locality(handle, 0);
	if (rv)
		return rv;

	handle->stat = tpm_status(handle);
	if (handle->stat & TPM_STS_CMD_READY) {
		al_dbg("al_tpm_tis12_start: UIO_WRITE status %02x\n", handle->stat);
		return 0;
	}

	al_dbg("al_tpm_tis12_start: UIO_WRITE readying chip\n");

	/* Abort previous and restart. */
	handle->tpm_if->write1(handle->tpm_if, TPM_STS, TPM_STS_CMD_READY);
	rv = tpm_waitfor(handle, TPM_STS_CMD_READY, TPM_READY_TMO);
	if (rv) {
		al_dbg("al_tpm_tis12_start: UIO_WRITE readying failed %d\n", rv);
		return rv;
	}

	al_dbg("al_tpm_tis12_start: UIO_WRITE readying done\n");

	return 0;
}

int al_tpm_tis12_read(struct al_tpm_tis12_handle *handle, void *buf, int len, int *count, int flags)
{
	uint8_t *p = (uint8_t *)buf;
	int cnt;
	int rv, n, bcnt;

	al_dbg("al_tpm_tis12_read: len %d\n", len);
	cnt = 0;
	while (len > 0) {
		rv = tpm_waitfor(handle, TPM_STS_DATA_AVAIL | TPM_STS_VALID, TPM_READ_TMO);
		if (rv)
			return rv;

		bcnt = tpm_getburst(handle);
		n = (len < bcnt) ? len : bcnt;
		al_dbg("al_tpm_tis12_read: fetching %d, burst is %d\n", n, bcnt);
#if (BURST_RW == 1)
		handle->tpm_if->read(handle->tpm_if, TPM_DATA, p, n);
		len -= n;
		cnt += n;
		p += n;
#else
		for (; n--; len--) {
			*p++ = handle->tpm_if->read1(handle->tpm_if, TPM_DATA);
			cnt++;
		}
#endif

		{
			int i;
			al_dbg("%s: resp (%u bytes) = ", __func__, cnt);
			for (i = 0; i < cnt; i++)
				al_dbg("%02x ", ((uint8_t *)buf)[i]);
			al_dbg("\n");
		}

		if ((flags & TPM_PARAM_SIZE) == 0 && cnt >= 6)
			break;
	}
	al_dbg("al_tpm_tis12_read: read %d bytes, len %d\n", cnt, len);

	if (count)
		*count = cnt;

	return 0;
}

int al_tpm_tis12_write(struct al_tpm_tis12_handle *handle, void *buf, int len)
{
	uint8_t *p = (uint8_t *)buf;
	int cnt;
	int rv, r;

	al_dbg("al_tpm_tis12_write: handle %p buf %p len %d\n", handle, buf, len);

	rv = tpm_request_locality(handle, 0);
	if (rv)
		return rv;

	cnt = 0;
	while (cnt < len - 1) {
#if (BURST_RW == 1)
		r = tpm_getburst(handle);
		if (r > ((len - 1) - cnt))
			r = (len - 1) - cnt;
		if (r) {
			al_dbg("%s: writing bytes %u-%u/%u...\n",
				__func__, cnt + 1, cnt + 1 + r, len);
			handle->tpm_if->write(handle->tpm_if, TPM_DATA, p, r);
			cnt += r;
			p += r;
		}
#else
		for (r = tpm_getburst(handle); r > 0 && cnt < len - 1; r--) {
			al_dbg("%s: writing byte %u/%u...\n", __func__, cnt + 1, len);
			handle->tpm_if->write1(handle->tpm_if, TPM_DATA, *p++);
			cnt++;
		}
#endif
		rv = tpm_waitfor(handle, TPM_STS_VALID, TPM_READ_TMO);
		if (rv) {
			al_dbg("al_tpm_tis12_write: failed burst rv %d\n", rv);
			return rv;
		}
		handle->stat = tpm_status(handle);
		if (!(handle->stat & TPM_STS_DATA_EXPECT)) {
			al_dbg("al_tpm_tis12_write: tpm doesn't expect more data: rv %d stat=%02x\n"
				, rv, handle->stat);
			return -EIO;
		}
	}

	al_dbg("%s: writing byte %u/%u...\n", __func__, cnt + 1, len);
	handle->tpm_if->write1(handle->tpm_if, TPM_DATA, *p++);
	cnt++;

	rv = tpm_waitfor(handle, TPM_STS_VALID, TPM_READ_TMO);
	if (rv) {
		al_dbg("al_tpm_tis12_write: failed last byte rv %d\n", rv);
		return rv;
	}
	if ((handle->stat & TPM_STS_DATA_EXPECT) != 0) {
		al_dbg("al_tpm_tis12_write: tpm expects more data: failed rv %d stat=%02x\n",
			rv, handle->stat);
		return -EIO;
	}

	al_dbg("al_tpm_tis12_write: wrote %d byte\n", cnt);

	return 0;
}

/* Finish transaction. */
int al_tpm_tis12_end(struct al_tpm_tis12_handle *handle, al_bool is_write, al_bool is_err)
{
	int rv = 0;

	if (!is_write) {
		rv = tpm_waitfor(handle, TPM_STS_VALID, TPM_READ_TMO);
		if (rv)
			return rv;

		/* Still more data? */
		handle->stat = tpm_status(handle);
		if (!is_err && ((handle->stat & TPM_STS_DATA_AVAIL) == TPM_STS_DATA_AVAIL)) {
			al_dbg("al_tpm_tis12_end: read failed stat=%02x\n", handle->stat);
			rv = EIO;
		}

		handle->tpm_if->write1(handle->tpm_if, TPM_STS,
		    TPM_STS_CMD_READY);

		/* Release our (0th) locality. */
		handle->tpm_if->write1(handle->tpm_if, TPM_ACCESS, TPM_ACCESS_ACTIVE_LOCALITY);
	} else {
		/* Hungry for more? */
		handle->stat = tpm_status(handle);
		if (!is_err && (handle->stat & TPM_STS_DATA_EXPECT)) {
			al_dbg("al_tpm_tis12_end: write failed stat=%02x\n", handle->stat);
			rv = EIO;
		}

		handle->tpm_if->write1(handle->tpm_if, TPM_STS,
		    is_err ? TPM_STS_CMD_READY : TPM_STS_GO);
	}

	return rv;
}
