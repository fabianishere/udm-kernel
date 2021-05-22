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
#include "al_hal_ssm.h"
#include "al_hal_ssm_crc_memcpy.h"
#include "al_hal_udma_iofic.h"

/* CRC error interrupts */
#define AL_CRC_IOFIC_GROUP_A_ERROR_INTS \
	(AL_CRC_APP_INT_A_S2M_TIMEOUT | \
	 AL_CRC_APP_INT_A_M2S_TIMEOUT | \
	 AL_CRC_APP_INT_A_CMD_DECODE_ERROR | \
	 AL_CRC_APP_INT_A_INPUT_LENGTH_ERROR | \
	 AL_CRC_APP_INT_A_MEM_SW_ERROR | \
	 AL_CRC_APP_INT_A_LENGTH_TOO_BIG | \
	 AL_CRC_APP_INT_A_M2S_ERROR | \
	 AL_CRC_APP_INT_A_FIFO_ACK_ERROR_0 | \
	 AL_CRC_APP_INT_A_FIFO_ACK_ERROR_1 | \
	 AL_CRC_APP_INT_A_FIFO_ACK_ERROR_2 | \
	 AL_CRC_APP_INT_A_FIFO_ACK_ERROR_3 | \
	 AL_CRC_APP_INT_A_FIFO_Q_OVERRUN | \
	 AL_CRC_APP_INT_A_SOP_AFTER_SOP | \
	 AL_CRC_APP_INT_A_NO_SOP | \
	 AL_CRC_APP_INT_A_SOP_WITH_EOP | \
	 AL_CRC_APP_INT_A_EOP_AFTER_EOP)

/** How many descriptors to save between head and tail in case of
 * wrap around.
 */
#define AL_CRC_MEMCPY_DESC_RES 0

#define CRC_MEMCPY_DEBUG

#ifdef CRC_MEMCPY_DEBUG
#define al_debug al_dbg
#else
#define al_debug(...)
#endif

#ifdef CRC_MEMCPY_DEBUG
static void al_print_desc(union al_udma_desc *desc)
{
	al_dbg("crc_memcpy: Desc: %08x %08x %08x %08x\n",
			desc->tx_meta.len_ctrl, desc->tx_meta.meta_ctrl,
			desc->tx_meta.meta1, desc->tx_meta.meta2);
}

static
void al_print_crc_xaction(struct al_crc_transaction *xaction) {
	unsigned int i;

	al_dbg("crc_memcpy: CRC Transaction debug\n");
	al_dbg(" CRC TYPE: ");
	switch (xaction->crcsum_type) {
		case(AL_CRC_CHECKSUM_NULL):
			al_dbg(" NULL\n");
					break;
		case(AL_CRC_CHECKSUM_CRC32):
			al_dbg(" CRC32\n");
			break;
		case(AL_CRC_CHECKSUM_CRC32C):
			al_dbg(" CRC32C\n");
			break;
		case(AL_CRC_CHECKSUM_CKSM16):
			al_dbg(" CKSM16\n");
			break;
		case(AL_CRC_CHECKSUM_CKSM32):
			al_dbg(" CKSM32\n");
			break;
		case(AL_CRC_CHECKSUM_CRC16):
			al_dbg(" CRC16\n");
			break;
		case(AL_CRC_CHECKSUM_CKSM32A):
			al_dbg(" CKSM32 Adler\n");
			break;
		case(AL_CRC_CHECKSUM_CRC8):
			al_dbg(" CRC8\n");
			break;
		case(AL_CRC_CHECKSUM_CRC8N):
			al_dbg(" CRC8N\n");
			break;
		case(AL_CRC_CHECKSUM_CKSM8):
			al_dbg(" CKSM8\n");
			break;
	}
	al_dbg(" Flags %d\n", xaction->flags);

	al_dbg("-SRC num of buffers  %d\n",
			xaction->src.num);
	for (i = 0 ; i < xaction->src.num; i++)
		al_dbg(" addr 0x%016" PRIx64 " len %d\n",
			xaction->src.bufs[i].addr,
			xaction->src.bufs[i].len);

	al_dbg("-DST num of buffers  %d\n",
			xaction->dst.num);
	for (i = 0 ; i < xaction->dst.num; i++)
		al_dbg(" addr 0x%016" PRIx64 " len %d\n",
			xaction->dst.bufs[i].addr,
			xaction->dst.bufs[i].len);

	al_dbg(" CRC IV IN size: %d, addr 0x%016" PRIx64 "\n",
			xaction->crc_iv_in.len,
			xaction->crc_iv_in.addr);
	al_dbg(" Cached CRC index %d\n", xaction->cached_crc_indx);
	al_dbg(" Save CRC IV in cache: %d\n", xaction->st_crc_iv);
	al_dbg(" Store CRC Out in cache: %d\n", xaction->st_crc_out);
	al_dbg(" CRC Expected size: %d, addr 0x%016" PRIx64 "\n",
			xaction->crc_expected.len,
			xaction->crc_expected.addr);
	al_dbg(" CRC OUT size: %d, addr 0x%016" PRIx64 "\n",
			xaction->crc_out.len,
			xaction->crc_out.addr);
	al_dbg(" SWAP flags %x\n", xaction->swap_flags);
	al_dbg(" XOR Valid: %d XOR in: %x XOR out: %x\n",
			xaction->xor_valid, xaction->in_xor, xaction->res_xor);


}
#else
#define al_print_desc(x)
#define al_print_crc_xaction(x)
#endif

/**
 * Fill one rx submission descriptor
 *
 * @param rx_udma_q rx udma handle
 * @param flags flags for the descriptor
 * @param buf destination buffer
 * @param tgtid virtual machine ID
 */
static INLINE
void al_crc_memcpy_prep_one_rx_desc(struct al_udma_q *rx_udma_q,
		uint32_t flags, struct al_buf *buf, uint16_t tgtid)
{
	uint64_t tgtid_shifted =	((uint64_t)tgtid) << AL_UDMA_DESC_TGTID_SHIFT;
	uint32_t flags_len = flags;
	union al_udma_desc *rx_desc;
	uint32_t ring_id;

	rx_desc = al_udma_desc_get(rx_udma_q);
	/* get ring id */
	ring_id = al_udma_ring_id_get(rx_udma_q)
		<< AL_S2M_DESC_RING_ID_SHIFT;

	flags_len |= ring_id;

	flags_len |= buf->len & AL_S2M_DESC_LEN_MASK;
	rx_desc->rx.len_ctrl = swap32_to_le(flags_len);
	rx_desc->rx.buf1_ptr = swap64_to_le(buf->addr | tgtid_shifted);
	al_print_desc(rx_desc);
}

/**
 * Fill one tx submission descriptor
 *
 * @param tx_udma_q tx udma handle
 * @param flags flags for the descriptor
 * @param meta metadata word1
 * @param buf source buffer
 * @param tgtid virtual machine ID
 */
static INLINE void al_crc_memcpy_prep_one_tx_desc(struct al_udma_q *tx_udma_q,
		uint32_t flags, uint32_t meta, struct al_buf *buf,
		uint16_t tgtid)
{
	uint64_t tgtid_shifted =	((uint64_t)tgtid) << AL_UDMA_DESC_TGTID_SHIFT;
	uint32_t flags_len = flags;
	union al_udma_desc *tx_desc;
	uint32_t ring_id;

	tx_desc = al_udma_desc_get(tx_udma_q);
	/* get ring id */
	ring_id = al_udma_ring_id_get(tx_udma_q)
		<< AL_M2S_DESC_RING_ID_SHIFT;

	flags_len |= ring_id;

	flags_len |= buf->len & AL_M2S_DESC_LEN_MASK;
	tx_desc->tx.len_ctrl = swap32_to_le(flags_len);
	tx_desc->tx.meta_ctrl = swap32_to_le(meta);
	tx_desc->tx.buf_ptr = swap64_to_le(buf->addr | tgtid_shifted);
	al_print_desc(tx_desc);
}


/**
 * Get number of rx submission descriptors needed for crc transaction
 *
 * we need rx descriptor for each destination buffer.
 * if the transaction doesn't have destination buffers, then one
 * descriptor is needed
 *
 * @param xaction transaction context
 *
 * @return number of rx submission descriptors
 */
static INLINE
uint32_t _al_crcsum_xaction_rx_descs_count(struct al_crc_transaction *xaction)
{
	uint32_t count = xaction->dst.num + (xaction->crc_out.len ? 1 : 0);

	/* w/o destination buffers we need only 1 descriptor */
	if (!count)
		count = 1;

	return count;
}

/**
 * Get number of tx submission descriptors needed for crc transaction
 *
 * we need tx descriptor for each source buffer.
 *
 * @param xaction transaction context
 *
 * @return number of tx submission descriptors
 */
static INLINE
uint32_t _al_crcsum_xaction_tx_descs_count(struct al_crc_transaction *xaction)
{
	uint32_t count = xaction->src.num + (xaction->crc_iv_in.len ? 1 : 0) +
		(xaction->crc_expected.len ? 1 : 0);

	/* valid crc tx descs count */
	al_assert(count);
	/* Need one for metadata if offsets are valid */
	count += (xaction->xor_valid) ? 1 : 0;

	return count;
}

/**
 * Fill the memcpy rx submission descriptors
 *
 * this function writes the contents of the rx submission descriptors
 *
 * @param rx_udma_q rx udma handle
 * @param xaction transaction context
 */
static INLINE
void al_crc_memcpy_set_memcpy_rx_descs(struct al_udma_q *rx_udma_q,
	       struct al_memcpy_transaction *xaction)
{
	uint32_t flags = 0;
	uint32_t buf_idx;

	/* Set descriptor flags */
	flags |= (xaction->flags & AL_SSM_INTERRUPT) ? AL_S2M_DESC_INT_EN : 0;
	flags |= (xaction->flags & AL_SSM_DEST_NO_SNOOP) ?
		AL_S2M_DESC_NO_SNOOP_H : 0;

	/* dst exist -> will copy the buf to the destination */
	if (xaction->dst.num) {
		struct al_buf *buf = xaction->dst.bufs;
		al_debug("crc_memcpy: Preparing %d Memcpy DST Rx desc\n",
				xaction->dst.num);
		for (buf_idx = 0; buf_idx < xaction->dst.num; buf_idx++) {
			al_crc_memcpy_prep_one_rx_desc(
				rx_udma_q, flags, buf, xaction->dst.tgtid);
			buf++;
		}
	}

}


/**
 * Fill the crc/checksum rx submission descriptors
 *
 * this function writes the contents of the rx submission descriptors
 *
 * @param rx_udma_q rx udma handle
 * @param xaction transaction context
 */
static INLINE
void al_crc_memcpy_set_crc_rx_descs(struct al_udma_q *rx_udma_q,
	       struct al_crc_transaction *xaction)
{
	uint32_t flags = 0;
	union al_udma_desc *rx_desc;
	uint32_t buf_idx;

	/* Set descriptor flags */
	flags = (xaction->flags & AL_SSM_INTERRUPT) ? AL_S2M_DESC_INT_EN : 0;
	flags |= (xaction->flags & AL_SSM_DEST_NO_SNOOP) ?
		AL_S2M_DESC_NO_SNOOP_H : 0;

	/* if the xaction doesn't have destination buffers,
	 * allocate single Meta descriptor,
	 */
	if (unlikely((xaction->dst.num == 0) && (xaction->crc_out.len == 0))) {
		al_debug("crc_memcpy: Preparing CRC Meta Rx desc\n");
		rx_desc = al_udma_desc_get(rx_udma_q);
		flags |= al_udma_ring_id_get(rx_udma_q)
			<< AL_S2M_DESC_RING_ID_SHIFT;
		flags |= RX_DESC_META;
		/* write back flags */
		rx_desc->rx.len_ctrl = swap32_to_le(flags);
		al_print_desc(rx_desc);
		return;
	}

	/* dst exist -> will copy the buf to the destination */
	if (xaction->dst.num) {
		struct al_buf *buf = xaction->dst.bufs;
		al_debug("crc_memcpy: Preparing %d CRC DST Rx desc\n",
				xaction->dst.num);
		for (buf_idx = 0; buf_idx < xaction->dst.num; buf_idx++) {
			al_crc_memcpy_prep_one_rx_desc(
				rx_udma_q, flags, buf, xaction->dst.tgtid);
			buf++;
		}
	}

	/* crc/checksum output */
	if (xaction->crc_out.len) {
		al_debug("crc_memcpy: Preparing CRC out Rx desc\n");
		al_crc_memcpy_prep_one_rx_desc(rx_udma_q, flags,
				&xaction->crc_out, xaction->dst.tgtid);
	}

}

/**
 * Fill the memcpy tx submission descriptors
 *
 * this function writes the contents of the tx submission descriptors
 *
 * @param tx_udma_q tx udma handle
 * @param xaction transaction context
 */
static INLINE
void al_crc_memcpy_set_memcpy_tx_descs(struct al_udma_q *tx_udma_q,
		struct al_memcpy_transaction *xaction)
{
	struct al_buf *buf = xaction->src.bufs;
	uint32_t flags = 0;
	uint32_t buf_idx;
	uint32_t word1_meta = 0;

	/* Set flags */
	flags |= AL_M2S_DESC_FIRST;
	flags |= (xaction->flags & AL_SSM_SRC_NO_SNOOP) ?
		AL_M2S_DESC_NO_SNOOP_H : 0;

	/* Set first desc word1 metatdata */
	word1_meta |= AL_CRC_CHECKSUM << TX_DESC_META_OP_SHIFT;
	word1_meta |= AL_CRC_CHECKSUM_NULL << TX_DESC_META_CRC_OP_TYPE_SHIFT;
	word1_meta |= TX_DESC_META_CRC_SEND_ORIG;
	word1_meta |= RX_DESC_META_CRC_FIRST_BUF;
	word1_meta |= RX_DESC_META_CRC_LAST_BUF;

	flags |= (xaction->flags & AL_SSM_BARRIER) ? AL_M2S_DESC_DMB : 0;

	al_debug("crc_memcpy: Preparing %d Memcpy SRC Tx desc\n",
				xaction->src.num);

	for (buf_idx = 0; buf_idx < xaction->src.num; buf_idx++) {
		/* check for last */
		if (buf_idx == (xaction->src.num - 1))
			flags |= AL_M2S_DESC_LAST;

		al_crc_memcpy_prep_one_tx_desc(
			tx_udma_q, flags, word1_meta, buf, xaction->src.tgtid);
		word1_meta = 0;
		/* clear first and DMB flags, keep no snoop hint flag */
		flags &= AL_M2S_DESC_NO_SNOOP_H;
		flags |= AL_M2S_DESC_CONCAT;
		buf++;
	}

}

/**
 * Fill the crc/checksum tx submission descriptors
 *
 * this function writes the contents of the tx submission descriptors
 *
 * @param tx_udma_q tx udma handle
 * @param xaction transaction context
 */
static INLINE
void al_crc_memcpy_set_crc_tx_descs(struct al_udma_q *tx_udma_q,
		struct al_crc_transaction *xaction)
{
	struct al_buf *buf = xaction->src.bufs;
	uint32_t flags = 0;
	uint32_t buf_idx;
	uint32_t word1_meta;

	/* Set flags */
	flags = AL_M2S_DESC_FIRST;
	flags |= (xaction->flags & AL_SSM_SRC_NO_SNOOP) ?
		AL_M2S_DESC_NO_SNOOP_H : 0;

	/* Set first desc word1 metatdata */
	word1_meta = AL_CRC_CHECKSUM << TX_DESC_META_OP_SHIFT;
	word1_meta |= (xaction->crcsum_type << TX_DESC_META_CRC_OP_TYPE_SHIFT)
		& TX_DESC_META_CRC_OP_TYPE_MASK;
	word1_meta |= ((xaction->crcsum_type >> 3) << TX_DESC_META_CRC_OP_TYPE_EXTENDED_SHIFT)
		& TX_DESC_META_CRC_OP_TYPE_EXTENDED_MASK;
	word1_meta |= xaction->dst.num ? TX_DESC_META_CRC_SEND_ORIG : 0;
	word1_meta |= xaction->st_crc_iv ? TX_DESC_META_CRC_ST_CRC_IV : 0;
	word1_meta |= xaction->st_crc_out ? TX_DESC_META_CRC_SAVE_IV : 0;
	word1_meta |= xaction->crc_out.len ? TX_DESC_META_CRC_SEND_CRC : 0;
	word1_meta |= xaction->crc_iv_in.len ? 0 : TX_DESC_META_CRC_USE_ST_IV;
	word1_meta |= xaction->crc_expected.len ? TX_DESC_META_CRC_VALID : 0;
	word1_meta |= (xaction->swap_flags << TX_DESC_META_CRC_SWAP_SHIFT)
		& TX_DESC_META_CRC_SWAP_MASK;
	word1_meta |= (xaction->cached_crc_indx << TX_DESC_META_CRC_IDX_SHIFT)
		& TX_DESC_META_CRC_IDX_MASK;
	word1_meta |= ((xaction->cached_crc_indx >> 3) << TX_DESC_META_CRC_EXTENDED_IDX_SHIFT)
		& TX_DESC_META_CRC_EXTENDED_IDX_MASK;

	/* if xor fields are valid first desc is metadata */
	if (unlikely(xaction->xor_valid)) {
		uint32_t flags_len = flags;
		union al_udma_desc *tx_desc;
		uint32_t ring_id;

		al_debug("crc_memcpy: preparing CRC metadata desc\n");
		tx_desc = al_udma_desc_get(tx_udma_q);
		/* UDMA feilds */
		ring_id = al_udma_ring_id_get(tx_udma_q)
			<< AL_M2S_DESC_RING_ID_SHIFT;
		flags_len |= ring_id;
		flags_len |= AL_M2S_DESC_META_DATA;
		tx_desc->tx_meta.len_ctrl = swap32_to_le(flags_len);
		/* Word1 metadata */
		tx_desc->tx_meta.meta_ctrl = 0;
		/* Word 2 metadat */
		tx_desc->tx_meta.meta1 = swap32_to_le(xaction->in_xor);
		/* Word 3 metadata */
		tx_desc->tx_meta.meta2 = swap32_to_le(xaction->res_xor);
		al_print_desc(tx_desc);
		/* clear first flag, keep no snoop hint flag */
		/* Indicate Last Block */
		flags &= AL_M2S_DESC_NO_SNOOP_H;

	}

	flags |= (xaction->flags & AL_SSM_BARRIER) ? AL_M2S_DESC_DMB : 0;

	word1_meta |= TX_DESC_META_CRC_FIRST_BUF;

	/* CRC IV in */
	if (xaction->crc_iv_in.len) {
		al_debug("CRC_memcpy: Preparing CRC IV in Tx desc\n");
		/* check for last */
		flags |= xaction->src.num ? 0 : AL_M2S_DESC_LAST;

		word1_meta |= xaction->src.num ? 0 : TX_DESC_META_CRC_LAST_BUF;

		al_crc_memcpy_prep_one_tx_desc(tx_udma_q, flags, word1_meta,
				&xaction->crc_iv_in, xaction->src.tgtid);
		word1_meta = 0;
		/* clear first and DMB flags, keep no snoop hint flag */
		flags &= AL_M2S_DESC_NO_SNOOP_H;

	}

	/* CRC IV expected */
	if (xaction->crc_expected.len) {
		al_debug("CRC_memcpy: Preparing CRC expected Tx desc\n");

		al_crc_memcpy_prep_one_tx_desc(tx_udma_q, flags, word1_meta,
				&xaction->crc_expected, xaction->src.tgtid);
		word1_meta = 0;
		/* clear first and DMB flags, keep no snoop hint flag */
		flags &= AL_M2S_DESC_NO_SNOOP_H;

	}

	al_debug("CRC_memcpy: Preparing %d CRC SRC Tx desc\n",
				xaction->src.num);

	/* Indicate Last Block */
	word1_meta |= TX_DESC_META_CRC_LAST_BUF;
	for (buf_idx = 0; buf_idx < xaction->src.num; buf_idx++) {
		/* check for last */
		if (buf_idx == (xaction->src.num - 1))
			flags |= AL_M2S_DESC_LAST;

		al_crc_memcpy_prep_one_tx_desc(
			tx_udma_q, flags, word1_meta, buf, xaction->src.tgtid);
		word1_meta = 0;
		/* clear first and DMB flags, keep no snoop hint flag */
		flags &= AL_M2S_DESC_NO_SNOOP_H;
		flags |= AL_M2S_DESC_CONCAT;
		buf++;
	}

}

int al_memcpy_prepare(struct al_ssm_dma *dma, uint32_t qid,
		struct al_memcpy_transaction *xaction)
{
	uint32_t rx_descs = xaction->dst.num;
	uint32_t tx_descs = xaction->src.num;
	struct al_udma_q *rx_udma_q;
	struct al_udma_q *tx_udma_q;
	int rc;

	al_debug("%s\n", __func__);
	al_assert(rx_descs);
	al_assert(tx_descs);

	/* Queue is for memcpy transactions */
	al_assert(dma->q_types[qid] == AL_MEM_CRC_MEMCPY_Q);

	/* ensure enough rx/tx udma descs */
	rc = al_udma_q_handle_get(&dma->m2m_udma.rx_udma, qid, &rx_udma_q);
	al_assert(!rc);
	if (unlikely(al_udma_available_get(rx_udma_q) < rx_descs)) {
		al_dbg("memcpy[%s]:rx q has no enough free desc\n",
			 dma->m2m_udma.name);
		return -ENOSPC;
	}

	rc = al_udma_q_handle_get(&dma->m2m_udma.tx_udma, qid, &tx_udma_q);
	al_assert(!rc);
	if (unlikely(al_udma_available_get(tx_udma_q) < tx_descs
				+ AL_CRC_MEMCPY_DESC_RES)) {
		uint32_t cdesc_count;

		/* cleanup tx completion queue */
		cdesc_count = al_udma_cdesc_get_all(tx_udma_q, NULL);
		if (likely(cdesc_count != 0))
			al_udma_cdesc_ack(tx_udma_q, cdesc_count);

		/* check again */
		if (unlikely(al_udma_available_get(tx_udma_q) < tx_descs
				+ AL_CRC_MEMCPY_DESC_RES)) {
			al_err("memcpy[%s]:tx q has no enough free desc\n",
				 dma->m2m_udma.name);
			return -ENOSPC;
		}
	}

	/* prepare memcpy rx/tx descs */
	al_crc_memcpy_set_memcpy_rx_descs(rx_udma_q, xaction);
	al_crc_memcpy_set_memcpy_tx_descs(tx_udma_q, xaction);
	/* add rx descriptors */
	al_udma_desc_action_add(rx_udma_q, rx_descs);

	/* set number of tx descriptors */
	xaction->tx_descs_count = tx_descs;

	return 0;
}

/******************************************************************************
 *****************************************************************************/
int al_crc_csum_prepare(struct al_ssm_dma *dma, uint32_t qid,
		struct al_crc_transaction *xaction)
{
	uint32_t rx_descs;
	uint32_t tx_descs;
	struct al_udma_q *rx_udma_q;
	struct al_udma_q *tx_udma_q;
	int rc;

	al_debug("al_crc_csum\n");
	al_print_crc_xaction(xaction);

	/* Queue is for crc/csum transactions */
	al_assert(dma->q_types[qid] == AL_MEM_CRC_MEMCPY_Q);

	/* Save and store together */
	al_assert(((xaction->st_crc_iv == 0) || (xaction->st_crc_out == 0)));

	/* calc tx (M2S) descriptors */
	tx_descs = _al_crcsum_xaction_tx_descs_count(xaction);
	/* valid crc tx descs count */
	al_assert(tx_descs <= dma->ssm_max_src_descs);

	rc = al_udma_q_handle_get(&dma->m2m_udma.tx_udma, qid, &tx_udma_q);
	/* valid CRC/CSUM tx q handle */
	al_assert(!rc);
	if (unlikely(al_udma_available_get(tx_udma_q) < tx_descs
				+ AL_CRC_MEMCPY_DESC_RES)) {
		uint32_t cdesc_count;

		/* cleanup tx completion queue */
		cdesc_count = al_udma_cdesc_get_all(tx_udma_q, NULL);
		if (likely(cdesc_count != 0))
			al_udma_cdesc_ack(tx_udma_q, cdesc_count);

		/* check again */
		if (unlikely(al_udma_available_get(tx_udma_q) < tx_descs
				+ AL_CRC_MEMCPY_DESC_RES)) {

			al_dbg("crc_csum[%s]:tx q has no enough free desc\n",
				 dma->m2m_udma.name);
			return -ENOSPC;
		}
	}

	/* calc rx (S2M) descriptors, at least one desc is required */
	rx_descs = _al_crcsum_xaction_rx_descs_count(xaction);
	/* valid crc rx descs count */
	al_assert(rx_descs <= dma->ssm_max_dst_descs);
	al_assert(rx_descs >= 1);

	rc = al_udma_q_handle_get(&dma->m2m_udma.rx_udma, qid, &rx_udma_q);
	/* valid CRC/CSUM rx q handle */
	al_assert(!rc);
	if (unlikely(al_udma_available_get(rx_udma_q) < rx_descs)) {
		al_dbg("crc_csum[%s]:rx q has no enough free desc\n",
			 dma->m2m_udma.name);
		return -ENOSPC;
	}

	/* prepare crc/checksum rx descs */
	al_crc_memcpy_set_crc_rx_descs(rx_udma_q, xaction);
	/* prepare crc/checksum tx descs */
	al_crc_memcpy_set_crc_tx_descs(tx_udma_q, xaction);
	/* add rx descriptors */
	al_udma_desc_action_add(rx_udma_q, rx_descs);

	/* set number of tx descriptors */
	xaction->tx_descs_count = tx_descs;

	return 0;
}

/******************************************************************************
 *****************************************************************************/
int al_crc_memcpy_dma_action(struct al_ssm_dma *dma, uint32_t qid,
		int tx_descs)
{
	struct al_udma_q *tx_udma_q;
	int rc;

	rc = al_udma_q_handle_get(&dma->m2m_udma.tx_udma, qid, &tx_udma_q);
	/* valid CRC/CSUM tx q handle */
	al_assert(!rc);

	/* add tx descriptors */
	al_udma_desc_action_add(tx_udma_q, tx_descs);
	return 0;
}

/******************************************************************************
 *****************************************************************************/
int al_crc_memcpy_dma_completion(struct al_ssm_dma *dma, uint32_t qid,
			      uint32_t *comp_status)
{
	struct al_udma_q *rx_udma_q;
	volatile union al_udma_cdesc *cdesc;
	int rc;
	uint32_t cdesc_count;

	rc = al_udma_q_handle_get(&dma->m2m_udma.rx_udma, qid, &rx_udma_q);
	/* valid comp rx q handle */
	al_assert(!rc);

	cdesc_count = al_udma_cdesc_packet_get(rx_udma_q, &cdesc);
	if (!cdesc_count)
		return 0;

	/* if we have multiple completion descriptors,
	   then last one will have the valid status */
	if (unlikely(cdesc_count > 1))
		cdesc = al_cdesc_next(rx_udma_q, cdesc, cdesc_count - 1);

	*comp_status = swap32_from_le(cdesc->al_desc_comp_rx.ctrl_meta) &
		RX_COMP_STATUS_MASK;

	al_udma_cdesc_ack(rx_udma_q, cdesc_count);

	al_debug("crc_memcpy packet completed. count %d status desc %p meta %x\n",
			cdesc_count, cdesc, cdesc->al_desc_comp_rx.ctrl_meta);

	return 1;
}

/******************************************************************************
 *****************************************************************************/
void al_crc_error_ints_mask_get(unsigned int rev_id __attribute__((__unused__)),
				    uint32_t *crc_a_mask,
				    uint32_t *crc_b_mask,
				    uint32_t *crc_c_mask,
				    uint32_t *crc_d_mask)
{
	*crc_a_mask = AL_CRC_IOFIC_GROUP_A_ERROR_INTS;
	*crc_b_mask = 0;
	*crc_c_mask = 0;
	*crc_d_mask = 0;
}

/******************************************************************************
 *****************************************************************************/
void al_crc_memcpy_error_ints_unmask(uint8_t rev_id,
				     unsigned int num_of_crc,
				     void __iomem *crc_regs_base[AL_SSM_MAX_CRC],
				     void __iomem *udma_regs_base)
{
	struct al_udma udma;
	struct al_udma_params udma_params = {
		.udma_regs_base = udma_regs_base
	};
	uint32_t a_mask;
	uint32_t b_mask;
	uint32_t c_mask;
	uint32_t d_mask;
	unsigned int i;
	void *iofic_regs_base;
	uint32_t ext_app_bit;

	if (rev_id <= AL_SSM_REV_ID_REV2)
		al_assert(num_of_crc <= AL_SSM_V1_V2_CRC_NUM);
	else
		al_assert(num_of_crc <= AL_SSM_V3_CRC_NUM);

	al_udma_handle_init(&udma, &udma_params);

	al_crc_error_ints_mask_get(rev_id,
				   &a_mask,
				   &b_mask,
				   &c_mask,
				   &d_mask);

	for (i = 0; i < num_of_crc; i++) {
		al_assert(crc_regs_base[i]);
		iofic_regs_base = (uint8_t *)crc_regs_base[i] + AL_CRC_APP_IOFIC_OFFSET;

		al_iofic_config(iofic_regs_base,
				AL_INT_GROUP_A,
				INT_CONTROL_GRP_MASK_MSI_X);

		al_iofic_unmask(iofic_regs_base,
				AL_INT_GROUP_A,
				a_mask);
	}

	al_iofic_config(
			al_udma_iofic_reg_base_get_adv(&udma,
				AL_UDMA_IOFIC_LEVEL_PRIMARY),
			AL_INT_GROUP_D,
			INT_CONTROL_GRP_SET_ON_POSEDGE |
			INT_CONTROL_GRP_MASK_MSI_X);

	ext_app_bit = al_udma_iofic_get_ext_app_bit(&udma);

	al_udma_iofic_unmask_adv(&udma,
			     AL_UDMA_IOFIC_LEVEL_PRIMARY,
			     AL_INT_GROUP_D,
			     ext_app_bit);
}
