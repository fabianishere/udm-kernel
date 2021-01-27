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

/**
 *  @{
 * @file   al_hal_msg_ipc.c
 *
 */

#include "al_hal_msg_ipc.h"
#include "al_hal_ssm_raid.h"

/******************************************************************************
 * RAID engine specific definitions/macros/type-definitions
 ******************************************************************************/
#define TX_DESC_META_ATTR_MEMCPY_REDIRECT		(1 << 16)

#define TX_DESC_META_ATTR_MEMCPY_DEST_UDMA_MASK		(0xf << 12)
#define TX_DESC_META_ATTR_MEMCPY_DEST_UDMA_SHIFT	12

#define TX_DESC_META_ATTR_MEMCPY_DEST_Q_MASK		(0xf << 8)
#define TX_DESC_META_ATTR_MEMCPY_DEST_Q_SHIFT		8

#define TX_DESC_META_MEMCPY_REDIRECT(dst_udma, dst_q)	\
	(AL_RAID_MEM_CPY_OPCODE |\
	 AL_RAID_TX_DESC_META_FIRST_SOURCE |\
	AL_RAID_TX_DESC_META_LAST_SOURCE |\
	TX_DESC_META_ATTR_MEMCPY_REDIRECT |\
	 ((dst_udma) << TX_DESC_META_ATTR_MEMCPY_DEST_UDMA_SHIFT) |\
	 ((dst_q) << TX_DESC_META_ATTR_MEMCPY_DEST_Q_SHIFT))

#define RX_CDESC_STATUS_REDIRECTED	AL_BIT(16)

#define RX_CDESC_STATUS_SRC_UDMA_MASK	AL_FIELD_MASK(15, 12)
#define RX_CDESC_STATUS_SRC_UDMA_SHIFT	12

#define RX_CDESC_STATUS_SRC_QUEUE_MASK	AL_FIELD_MASK(11, 8)
#define RX_CDESC_STATUS_SRC_QUEUE_SHIFT	8

/* RX completion descriptor */
struct al_msg_ipc_rx_cdesc {
	uint32_t ctrl_meta;
	uint32_t len;
	uint32_t word2;
	uint32_t word3;
};

/******************************************************************************
 ******************************************************************************/
int al_msg_ipc_tx_pkt_prepare(
	struct al_udma_q		*tx_dma_q,
	struct al_msg_ipc_pkt	*pkt)
{
	union al_udma_desc *tx_desc;
	uint32_t tx_descs;
	uint32_t flags = AL_M2S_DESC_FIRST;
	uint32_t meta_ctrl =
		TX_DESC_META_MEMCPY_REDIRECT(pkt->udma_id, pkt->queue_id);
	uint32_t ring_id;
	int buf_idx;

	al_dbg("[%s %d]: new tx pkt\n", tx_dma_q->udma->name, tx_dma_q->qid);

	tx_descs = pkt->num_of_bufs;

	if (unlikely(al_udma_available_get(tx_dma_q) < tx_descs)) {
		al_dbg("[%s %d]: failed to allocate (%d) descriptors\n",
			 tx_dma_q->udma->name, tx_dma_q->qid, tx_descs);
		return 0;
	}

	flags |= pkt->flags;

	for (buf_idx = 0; buf_idx < pkt->num_of_bufs; buf_idx++) {
		uint32_t flags_len = flags;

		tx_desc = al_udma_desc_get(tx_dma_q);

		ring_id = al_udma_ring_id_get(tx_dma_q) <<
			AL_M2S_DESC_RING_ID_SHIFT;

		flags_len |= ring_id;

		if (buf_idx == (pkt->num_of_bufs - 1)) {
			flags_len |= AL_M2S_DESC_LAST;
		}

		flags &= AL_MSG_IPC_TX_FLAGS_NO_SNOOP;
		flags |= AL_M2S_DESC_CONCAT;

		flags_len |= pkt->bufs[buf_idx].len & AL_M2S_DESC_LEN_MASK;

		tx_desc->tx.len_ctrl = swap32_to_le(flags_len);
		tx_desc->tx.meta_ctrl = swap32_to_le(meta_ctrl);
		tx_desc->tx.buf_ptr = swap64_to_le(pkt->bufs[buf_idx].addr);

		al_dbg("[%s %d]: tx_desc = %08x, %08x, %08x%08x\n",
			tx_dma_q->udma->name, tx_dma_q->qid,
			flags_len, meta_ctrl,
			(uint32_t)(pkt->bufs[buf_idx].addr >> 32),
			(uint32_t)(pkt->bufs[buf_idx].addr & 0xffffffff));

		meta_ctrl = 0;
	}

	al_dbg("[%s %d]: pkt descs written into the tx queue. descs num (%d)\n",
		tx_dma_q->udma->name, tx_dma_q->qid, tx_descs);

	return tx_descs;
}

/******************************************************************************
 ******************************************************************************/
int al_msg_ipc_comp_tx_get(struct al_udma_q *tx_dma_q)
{
	int rc;

	rc = al_udma_cdesc_get_all(tx_dma_q, NULL);
	if (rc != 0) {
		al_udma_cdesc_ack(tx_dma_q, rc);
		al_dbg("[%s %d]: tx completion: descs (%d)\n",
			 tx_dma_q->udma->name, tx_dma_q->qid, rc);
	}

	return rc;
}

/******************************************************************************
 ******************************************************************************/
int al_msg_ipc_rx_buffer_add(
	struct al_udma_q	*rx_dma_q,
	struct al_buf		*buf,
	unsigned int		flags)
{
	uint32_t flags_len = flags;
	union al_udma_desc *rx_desc;

	al_dbg("[%s %d]: add rx buffer.\n",
		rx_dma_q->udma->name, rx_dma_q->qid);

	if (unlikely(al_udma_available_get(rx_dma_q) < 1)) {
		al_dbg("[%s]: rx q (%d) has no enough free descriptor\n",
			 rx_dma_q->udma->name, rx_dma_q->qid);
		return -ENOSPC;
	}

	rx_desc = al_udma_desc_get(rx_dma_q);

	flags_len |= al_udma_ring_id_get(rx_dma_q) << AL_M2S_DESC_RING_ID_SHIFT;
	flags_len |= buf->len & AL_M2S_DESC_LEN_MASK;

	rx_desc->rx.len_ctrl = swap32_to_le(flags_len);
	rx_desc->rx.buf1_ptr = swap64_to_le(buf->addr);

	return 0;
}

/******************************************************************************
 ******************************************************************************/
unsigned int al_msg_ipc_pkt_rx(
	struct al_udma_q		*rx_dma_q,
	struct al_msg_ipc_pkt	*pkt)
{
	volatile union al_udma_cdesc *cdesc;
	volatile struct al_msg_ipc_rx_cdesc *rx_desc = NULL;
	uint32_t i;
	uint32_t rc;

	rc = al_udma_cdesc_packet_get(rx_dma_q, &cdesc);
	if (rc == 0)
		return 0;

	al_dbg("[%s]: fetch rx packet: queue %d.\n",
		 rx_dma_q->udma->name, rx_dma_q->qid);

	for (i = 0; i < rc; i++) {
		uint32_t len;

		/* get next descriptor */
		rx_desc = (volatile struct al_msg_ipc_rx_cdesc *)
			al_cdesc_next(rx_dma_q, cdesc, i);

		len = swap32_from_le(rx_desc->len);

		pkt->bufs[i].len = len & AL_M2S_DESC_LEN_MASK;

		al_dbg("[%s %d]: rx_cdesc = %08x, %08x\n",
			rx_dma_q->udma->name, rx_dma_q->qid,
			swap32_from_le(rx_desc->ctrl_meta), len);
	}
	/* get flags from last desc */
	pkt->flags = swap32_from_le(rx_desc->ctrl_meta);

	al_udma_cdesc_ack(rx_dma_q, rc);

	al_assert(pkt->flags & RX_CDESC_STATUS_REDIRECTED);

	pkt->udma_id = (pkt->flags & RX_CDESC_STATUS_SRC_UDMA_MASK) >>
		RX_CDESC_STATUS_SRC_UDMA_SHIFT;

	pkt->queue_id = (pkt->flags & RX_CDESC_STATUS_SRC_QUEUE_MASK) >>
		RX_CDESC_STATUS_SRC_QUEUE_SHIFT;

	return rc;
}

/** @} */

