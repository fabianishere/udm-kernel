/*
 * Copyright 2018, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#ifndef __AL_HAL_ETH_DESC_H__
#define __AL_HAL_ETH_DESC_H__

#include "al_mod_hal_eth.h"
#ifdef AL_ETH_EX
#include "al_mod_hal_eth_desc_ex.h"
#endif

/* RX Descriptor defines */
#define AL_ETH_RX_L3_OFFSET_SHIFT	9
#define AL_ETH_RX_L3_OFFSET_MASK	(0x7f << AL_ETH_RX_L3_OFFSET_SHIFT)
#define AL_ETH_RX_L3_PROTO_IDX_MASK	0x1F
#define AL_ETH_RX_L4_PROTO_IDX_MASK	0x1F
#define AL_ETH_RX_L4_PROTO_IDX_SHIFT	8
#define AL_ETH_RX_PROTO_NUM_MASK	0x3F
#define AL_ETH_RX_HASH_SHIFT		16
#define AL_ETH_RX_HASH_MASK		(0xffff << AL_ETH_RX_HASH_SHIFT)
#define AL_ETH_RX_W3_INNER_L4_CKSUM_ERROR_BIT		AL_BIT(29)

/* TX metadata Descriptor defines */
#define AL_ETH_TX_META_STORE			AL_BIT(21)
#define AL_ETH_TX_META_VALID_WORDS_SHIFT	12
#define AL_ETH_TX_META_L3_LEN_MASK		0xFF
#define AL_ETH_TX_META_L3_LEN_SHIFT		0
#define AL_ETH_TX_META_L3_OFF_MASK		0xFF
#define AL_ETH_TX_META_L3_OFF_SHIFT		8
#define AL_ETH_TX_META_L4_LEN_MASK		0x3F
#define AL_ETH_TX_META_L4_LEN_SHIFT		16
#define AL_ETH_TX_META_OUTER_L3_LEN_FIELD_MASK		AL_FIELD_MASK(28, 24)
#define AL_ETH_TX_META_OUTER_L3_LEN_SHIFT			24
#define AL_ETH_TX_META_OUTER_L3_OFF_HIGH_FIELD_MASK	AL_FIELD_MASK(11, 10)
#define AL_ETH_TX_META_OUTER_L3_OFF_HIGH_SHIFT		10
#define AL_ETH_TX_META_OUTER_L3_OFF_LOW_FIELD_MASK	AL_FIELD_MASK(31, 29)
#define AL_ETH_TX_META_OUTER_L3_OFF_LOW_SHIFT		29
#define AL_ETH_TX_META_OUTER_L3_OFF_LOW_NUM_BITS	3

/* Offload config defines */
#define AL_ETH_TX_META_OUTER_L3_LEN_CFG_MASK		0x1F
#define AL_ETH_TX_META_OUTER_L3_LEN_CFG_SHIFT		24
#define AL_ETH_TX_META_OUTER_L3_OFF_CFG_MASK		0x1F
#define AL_ETH_TX_META_OUTER_L3_OFF_CFG_SHIFT		10

/* TX Buffer Descriptor defines */
#define AL_ETH_TX_L4_PROTO_IDX_SHIFT		8
#define AL_ETH_TX_FLAGS_IPV4_L3_CSUM_SHIFT	13
#define AL_ETH_TX_FLAGS_L4_CSUM_SHIFT		14
#define AL_ETH_TX_PROTOCOL_NUM_MASK		AL_FIELD_MASK(27, 22)
#define AL_ETH_TX_PROTOCOL_NUM_SHIFT		22

/*
 * Get next completed packet's buffers count from completion ring of the queue
 *
 * @param udma_q Queue handle
 * @param pkt_context Pointer that set by this function
 *
 * @return Number of buffers on success, 0 if there are no new packets
 */
static inline uint32_t al_mod_eth_rx_pkt_get(struct al_mod_udma_q *udma_q, void **pkt_context)
{
	uint32_t buf_count = al_mod_udma_cdesc_packet_get(udma_q,
						     (volatile union al_mod_udma_cdesc **)pkt_context);
	return buf_count;
}

/*
 * Return the i's buffer of a packet
 *
 * @param udma_q Queue handle
 * @param pkt_context Pointer to packet context which returned in al_mod_eth_packet_get()
 * @param i Buffer number (Must be smaller than return value of al_mod_eth_packet_get())
 *
 * @return Buffer context
 */
static inline void *al_mod_eth_rx_pkt_next_buf_get(struct al_mod_udma_q *udma_q,
					       void *pkt_context, uint32_t i)
{
	return (void *)al_mod_cdesc_next_unsafe(udma_q, (union al_mod_udma_cdesc *)pkt_context, i);
}

/**
 * Acknowledge the driver that the upper layer completed receiving buffers
 *
 * @param udma_q UDMA queue handle
 * @param num Number of buffers to acknowledge
 *
 * @return 0
 */
static inline void al_mod_eth_rx_pkt_ack(struct al_mod_udma_q *udma_q, uint32_t num)
{
	al_mod_udma_cdesc_ack(udma_q, num);
}

/*
 * Return buffer's length out of buffer context
 *
 * @param buf_context
 *
 * @return Buffer's length
 */
static inline size_t al_mod_eth_rx_buf_len_get(void *buf_context)
{
	al_mod_eth_rx_cdesc *rx_desc = buf_context;

	if (rx_desc->ctrl_meta & AL_UDMA_CDESC_BUF1_USED)
		return swap32_from_le(rx_desc->len) & AL_S2M_DESC_LEN_MASK;
	return 0;
}

/*
 * Return packet's flags
 *
 * @param last_buf_context
 *
 * @return Packet's flags
 */
static inline uint32_t al_mod_eth_rx_pkt_flags_get(void *last_buf_context)
{
	al_mod_eth_rx_cdesc *rx_desc = last_buf_context;

	return swap32_from_le(rx_desc->ctrl_meta);
}

/*
 * Return packet's extended flags
 *
 * @param last_buf_context
 *
 * @return Packet's extended flags
 */
static inline uint32_t al_mod_eth_rx_pkt_ex_flags_get(void *last_buf_context)
{
	al_mod_eth_rx_cdesc *rx_desc = last_buf_context;

	return swap32_from_le(rx_desc->word3);
}

/*
 * Return packet's rx hash starting in AL5+
 *
 * @param last_buf_context
 *
 * @return Packet's rx hash
 */
static inline uint32_t al_mod_eth_rx_pkt_hash_get(void *last_buf_context)
{
	al_mod_eth_rx_cdesc *rx_desc = last_buf_context;

	return (swap32_from_le(rx_desc->len) & AL_ETH_RX_HASH_MASK) >> AL_ETH_RX_HASH_SHIFT;
}

/*
 * Return packet's HRSS rx hash starting in AL7
 * Relevant for ENA protocol
 *
 * @param last_buf_context
 *
 * @return Packet's HRSS rx hash
 */
static inline uint32_t al_mod_eth_rx_pkt_hrss_hash_get(void *last_buf_context)
{
	al_mod_eth_rx_cdesc *rx_desc = last_buf_context;

	return swap32_from_le(rx_desc->word5);
}

/*
 * Return packet's LRSS rx hash starting in AL7
 *
 * @param last_buf_context
 *
 * @return Packet's LRSS rx hash
 */
static inline uint32_t al_mod_eth_rx_pkt_lrss_hash_get(void *last_buf_context)
{
	al_mod_eth_rx_cdesc *rx_desc = last_buf_context;

	return swap32_from_le(rx_desc->word6);
}

/*
 * Return packet L3 header offset
 *
 * @param last_buf_context
 *
 * @return Packet's L3 header offset
 */
static inline uint32_t al_mod_eth_rx_pkt_l3_hdr_off_get(void *last_buf_context)
{
	al_mod_eth_rx_cdesc *rx_desc = last_buf_context;

	return (swap32_from_le(rx_desc->word2) & AL_ETH_RX_L3_OFFSET_MASK)
			>> AL_ETH_RX_L3_OFFSET_SHIFT;
}

/*
 * Return packet header length
 *
 * @param first_buf_context
 *
 * @return Packet's header length
 */
static inline uint32_t al_mod_eth_rx_buf_len2_get(void *first_buf_context)
{
	al_mod_eth_rx_cdesc *rx_desc = first_buf_context;

	if (swap32_from_le(rx_desc->word2) & AL_UDMA_CDESC_BUF2_USED) {
		return (swap32_from_le(rx_desc->word2) & AL_S2M_DESC_LEN2_MASK) >>
				AL_S2M_DESC_LEN2_SHIFT;
	}
	return 0;
}

/*
 * Return true if L2 error detected by the HW, otherwise return false
 *
 * @param last_buf_context
 *
 * @return L2 error status
 */
static inline al_mod_bool al_mod_eth_rx_pkt_l2_err_get(void *last_buf_context)
{
	al_mod_eth_rx_cdesc *rx_desc = last_buf_context;

	return !!(swap32_from_le(rx_desc->ctrl_meta) & AL_ETH_RX_ERROR);
}

/*
 * Return true if L3 checksum error detected by the HW, otherwise return false
 *
 * @param last_buf_context
 *
 * @return L3 checksum error status
 */
static inline al_mod_bool al_mod_eth_rx_pkt_l3_csum_err_get(void *last_buf_context)
{
	al_mod_eth_rx_cdesc *rx_desc = last_buf_context;

	return !!(swap32_from_le(rx_desc->ctrl_meta) & AL_ETH_RX_FLAGS_L3_CSUM_ERR);
}

/*
 * Return true if L4 checksum error detected by the HW, otherwise return false
 *
 * @param last_buf_context
 *
 * @return L4 checksum error status
 */
static inline al_mod_bool al_mod_eth_rx_pkt_l4_csum_err_get(void *last_buf_context)
{
	al_mod_eth_rx_cdesc *rx_desc = last_buf_context;

	return !!(swap32_from_le(rx_desc->ctrl_meta) & AL_ETH_RX_FLAGS_L4_CSUM_ERR);
}

/*
 * Return true if CRC error detected by the HW, otherwise return false
 *
 * @param last_buf_context
 *
 * @return CRC error status
 */
static inline al_mod_bool al_mod_eth_rx_pkt_crc_err_get(void *last_buf_context)
{
	al_mod_eth_rx_cdesc *rx_desc = last_buf_context;

	return !!(swap32_from_le(rx_desc->word3) & AL_ETH_RX_FLAGS_CRC);
}

/*
 * Return true if iCRC error detected by the HW, otherwise return false
 * Use this function in AL7 devices instead of al_mod_eth_rx_pkt_crc_err_get
 *
 * @param last_buf_context
 *
 * @return iCRC error status
 */
static inline al_mod_bool al_mod_eth_v4_rx_pkt_icrc_err_get(void *last_buf_context)
{
	al_mod_eth_rx_cdesc *rx_desc = last_buf_context;

	return !!(swap32_from_le(rx_desc->ctrl_meta) & AL_ETH_RX_FLAGS_V4_ICRC);
}

/*
 * Return true if L4 inner checksum error detected by the HW, otherwise return false
 *
 * @param last_buf_context
 *
 * @return L4 inner checksum error status
 */
static inline al_mod_bool al_mod_eth_rx_pkt_l4_csum_inner_err_get(void *last_buf_context)
{
	al_mod_eth_rx_cdesc *rx_desc = last_buf_context;

	return !!(swap32_from_le(rx_desc->word3) & AL_ETH_RX_W3_INNER_L4_CKSUM_ERROR_BIT);
}

/*
 * Return L3 protocol index
 *
 * @param last_buf_context
 *
 * @return L3 protocol index
 */
static inline enum AL_ETH_PROTO_ID al_mod_eth_rx_pkt_l3_proto_idx_get(void *last_buf_context)
{
	al_mod_eth_rx_cdesc *rx_desc = last_buf_context;

	return swap32_from_le(rx_desc->ctrl_meta) & AL_ETH_RX_L3_PROTO_IDX_MASK;
}

/*
 * Return L4 protocol index
 *
 * @param last_buf_context
 *
 * @return L4 protocol index
 */
static inline enum AL_ETH_PROTO_ID al_mod_eth_rx_pkt_l4_proto_idx_get(void *last_buf_context)
{
	al_mod_eth_rx_cdesc *rx_desc = last_buf_context;

	return (swap32_from_le(rx_desc->ctrl_meta) >> AL_ETH_RX_L4_PROTO_IDX_SHIFT)
			& AL_ETH_RX_L4_PROTO_IDX_MASK;
}

/*
 * Return protocol number
 *
 * @param last_buf_context
 *
 * @return packet's protocol number
 */
static inline uint8_t al_mod_eth_rx_pkt_proto_num_get(void *last_buf_context)
{
	al_mod_eth_rx_cdesc *rx_desc = last_buf_context;

	return swap32_from_le(rx_desc->ctrl_meta) & AL_ETH_RX_PROTO_NUM_MASK;
}


/*
 * Return IPv4 Fragmentation status
 *
 * @param last_buf_context
 *
 * @return true if packet is fragmented
 */
static inline al_mod_bool al_mod_eth_rx_pkt_ipv4_frag_get(void *last_buf_context)
{
	al_mod_eth_rx_cdesc *rx_desc = last_buf_context;

	return !!(swap32_from_le(rx_desc->ctrl_meta) & AL_ETH_RX_IPV4_FRAG);
}


/*
 * Initialize new UDMA TX descriptor
 *
 * @param dma_q
 *
 * @return Pointer to the new descriptor context
 */
static inline void *al_mod_eth_tx_desc_init(struct al_mod_udma_q *dma_q)
{
	union al_mod_udma_desc *tx_desc = al_mod_udma_desc_get(dma_q);

	tx_desc->raw.qword0 = 0;
	tx_desc->raw.qword1 = 0;
	return tx_desc;
}

/*
 * Finish handling UDMA TX descriptor
 *
 * @param context Context of the descriptor
 *
 */
static inline void al_mod_eth_tx_desc_finish(void *context __attribute__((__unused__)))
{
}

/*
 * Set metadata as first
 *
 * @param context Context of the metadata
 *
 */
static inline void al_mod_eth_tx_meta_first_set(void *context)
{
	union al_mod_udma_desc *tx_desc = context;

	tx_desc->tx_meta.len_ctrl |= AL_M2S_DESC_FIRST;
}

/*
 * Set metadata with concatenate flag
 *
 * @param context Context of the metadata
 *
 */
static inline void al_mod_eth_tx_meta_concat_set(void *context)
{
	union al_mod_udma_desc *tx_desc = context;

	tx_desc->tx_meta.len_ctrl |= AL_M2S_DESC_CONCAT;
}

/*
 * Set metadata flag
 *
 * @param context Context of the metadata
 *
 */
static inline void al_mod_eth_tx_meta_metadata_set(void *context)
{
	union al_mod_udma_desc *tx_desc = context;

	tx_desc->tx_meta.len_ctrl |= AL_M2S_DESC_META_DATA;
}

/*
 * Set metadata with store flag
 *
 * @param context Context of the metadata
 *
 */
static inline void al_mod_eth_tx_meta_store_set(void *context)
{
	union al_mod_udma_desc *tx_desc = context;

	tx_desc->tx_meta.len_ctrl |= AL_ETH_TX_META_STORE;
}

/*
 * Set metadata with ring ID
 *
 * @param context Context of the metadata
 * @param ring_id Ring ID
 *
 */
static inline void al_mod_eth_tx_meta_ring_id_set(void *context, uint32_t ring_id)
{
	union al_mod_udma_desc *tx_desc = context;

	tx_desc->tx_meta.len_ctrl |= (ring_id << AL_M2S_DESC_RING_ID_SHIFT);
}

/*
 * Set metadata with valid words
 *
 * @param context Context of the metadata
 * @param valid_words Valid words
 *
 */
static inline void al_mod_eth_tx_meta_valid_words_set(void *context, uint8_t valid_words)
{
	union al_mod_udma_desc *tx_desc = context;

	tx_desc->tx_meta.len_ctrl |= (valid_words << AL_ETH_TX_META_VALID_WORDS_SHIFT);
}

/*
 * Set L3 header length
 *
 * @param context Context of the metadata
 * @param l3_header_len L3 header length
 *
 */
static inline void al_mod_eth_tx_meta_l3_hdr_len_set(void *context, uint8_t l3_hdr_len)
{
	union al_mod_udma_desc *tx_desc = context;

	AL_REG_FIELD_SET(tx_desc->tx_meta.meta1, AL_ETH_TX_META_L3_LEN_MASK,
			 AL_ETH_TX_META_L3_LEN_SHIFT, l3_hdr_len);
}

/*
 * Set L3 header offset
 *
 * @param context Context of the metadata
 * @param l3_header_off L3 header offset
 *
 */
static inline void al_mod_eth_tx_meta_l3_hdr_off_set(void *context, uint8_t l3_hdr_off)
{
	union al_mod_udma_desc *tx_desc = context;

	tx_desc->tx_meta.meta1 |= ((l3_hdr_off & AL_ETH_TX_META_L3_OFF_MASK) <<
				   AL_ETH_TX_META_L3_OFF_SHIFT);
}

/*
 * Set L4 header length
 *
 * @param context Context of the metadata
 * @param l4_header_len L4 header length
 *
 */
static inline void al_mod_eth_tx_meta_l4_hdr_len_set(void *context, uint8_t l4_hdr_len)
{
	union al_mod_udma_desc *tx_desc = context;

	tx_desc->tx_meta.meta1 |= ((l4_hdr_len & AL_ETH_TX_META_L4_LEN_MASK) <<
				   AL_ETH_TX_META_L4_LEN_SHIFT);
}

/*
 * Set outer L3 header offset
 *
 * @param context Context of the metadata
 * @param outer_l3_off Outer L3 header offset
 *
 */
static inline void al_mod_eth_tx_meta_outer_l3_hdr_off_set(void *context, uint8_t outer_l3_off)
{
	union al_mod_udma_desc *tx_desc = context;
	/*
	 * move from bytes to multiplication of 2 as the HW
	 * expect to get it
	 */
	uint32_t offset = (outer_l3_off >> 1);

	AL_REG_FIELD_SET(tx_desc->tx_meta.len_ctrl, AL_ETH_TX_META_OUTER_L3_OFF_HIGH_FIELD_MASK,
			 AL_ETH_TX_META_OUTER_L3_OFF_HIGH_SHIFT,
			 (offset >> AL_ETH_TX_META_OUTER_L3_OFF_LOW_NUM_BITS));

	AL_REG_FIELD_SET(tx_desc->tx_meta.meta2, AL_ETH_TX_META_OUTER_L3_OFF_LOW_FIELD_MASK,
			 AL_ETH_TX_META_OUTER_L3_OFF_LOW_SHIFT, offset);
}

/*
 * Set outer L3 header length
 *
 * @param context Context of the metadata
 * @param outer_l3_len Outer L3 header length
 *
 */
static inline void al_mod_eth_tx_meta_outer_l3_hdr_len_set(void *context, uint8_t outer_l3_len)
{
	union al_mod_udma_desc *tx_desc = context;
	/*
	 * shift right 2 bits to work in multiplication of 4
	 * as the HW expect to get it
	 */
	outer_l3_len >>= 2;
	AL_REG_FIELD_SET(tx_desc->tx_meta.meta2, AL_ETH_TX_META_OUTER_L3_LEN_FIELD_MASK,
			 AL_ETH_TX_META_OUTER_L3_LEN_SHIFT, outer_l3_len);
}

/*
 * Set buffer as first
 *
 * @param context Context of the buffer
 *
 */
static inline void al_mod_eth_tx_buf_first_set(void *context)
{
	union al_mod_udma_desc *tx_desc = context;

	tx_desc->tx.len_ctrl |= AL_M2S_DESC_FIRST;
}

/*
 * Set buffer as last
 *
 * @param context Context of the buffer
 *
 */
static inline void al_mod_eth_tx_buf_last_set(void *context)
{
	union al_mod_udma_desc *tx_desc = context;

	tx_desc->tx.len_ctrl |= AL_M2S_DESC_LAST;
}

/*
 * Set buffer with concatenate flag
 *
 * @param context Context of the buffer
 *
 */
static inline void al_mod_eth_tx_buf_concat_set(void *context)
{
	union al_mod_udma_desc *tx_desc = context;

	tx_desc->tx.len_ctrl |= AL_M2S_DESC_CONCAT;
}

/*
 * Set buffer with ring ID
 *
 * @param context Context of the buffer
 * @param ring_id Ring ID
 *
 */
static inline void al_mod_eth_tx_buf_ring_id_set(void *context, uint32_t ring_id)
{
	union al_mod_udma_desc *tx_desc = context;

	tx_desc->tx.len_ctrl |= (ring_id << AL_M2S_DESC_RING_ID_SHIFT);
}

/*
 * Set buffer's length
 *
 * @param context Context of the buffer
 * @param length Buffer's length
 *
 */
static inline void al_mod_eth_tx_buf_length_set(void *context, uint16_t length)
{
	union al_mod_udma_desc *tx_desc = context;

	tx_desc->tx.len_ctrl |= (length & AL_M2S_DESC_LEN_MASK);
}

/*
 * Set buffer's L3 protocol
 *
 * @param context Context of the buffer
 * @param l3_proto Buffer's L3 protocol
 *
 */
static inline void al_mod_eth_tx_buf_l3_proto_set(void *context, enum AL_ETH_PROTO_ID l3_proto)
{
	union al_mod_udma_desc *tx_desc = context;

	tx_desc->tx.meta_ctrl |= l3_proto;
}

/*
 * Set buffer's L4 protocol
 *
 * @param context Context of the buffer
 * @param l4_proto Buffer's L4 protocol
 *
 */
static inline void al_mod_eth_tx_buf_l4_proto_set(void *context, enum AL_ETH_PROTO_ID l4_proto)
{
	union al_mod_udma_desc *tx_desc = context;

	tx_desc->tx.meta_ctrl |= (l4_proto << AL_ETH_TX_L4_PROTO_IDX_SHIFT);
}

/*
 * Set protocol number in buffer descriptor
 * This replaces L3 and L4 protocol index starting from Ethernet V4
 * @param context Context of the metadata
 * @param protocol_number Protocol number for packet transmission
 *
 */
static inline void al_mod_eth_tx_buf_proto_num_set(void *context, uint8_t protocol_number)
{
	union al_mod_udma_desc *tx_desc = context;

	AL_REG_FIELD_SET(tx_desc->tx.meta_ctrl, AL_ETH_TX_PROTOCOL_NUM_MASK,
			 AL_ETH_TX_PROTOCOL_NUM_SHIFT, protocol_number);
}

/*
 * Enable L3 checksum
 *
 * @param context Context of the buffer
 *
 */
static inline void al_mod_eth_tx_buf_l3_csum_set(void *context, al_mod_bool enable)
{
	union al_mod_udma_desc *tx_desc = context;

	tx_desc->tx.meta_ctrl |= enable << AL_ETH_TX_FLAGS_IPV4_L3_CSUM_SHIFT;
}

/*
 * Enable L4 checksum
 *
 * @param context Context of the buffer
 *
 */
static inline void al_mod_eth_tx_buf_l4_csum_set(void *context, al_mod_bool enable)
{
	union al_mod_udma_desc *tx_desc = context;

	tx_desc->tx.meta_ctrl |= enable << AL_ETH_TX_FLAGS_L4_CSUM_SHIFT;
}

/*
 * Set buffer's address
 *
 * @param context Context of the buffer
 * @param dma_address Buffer's Address
 *
 */
static inline void al_mod_eth_tx_buf_dma_addr_set(void *context, uint64_t dma_address)
{
	union al_mod_udma_desc *tx_desc = context;

	tx_desc->tx.buf_ptr = dma_address;
}

#endif /* __AL_HAL_ETH_DESC_H__ */
