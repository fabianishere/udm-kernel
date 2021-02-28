/*
 * Copyright 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 */

#ifndef __AL_HAL_CRYPTO_DESC_H__
#define __AL_HAL_CRYPTO_DESC_H__

/*
 * Rx (S2M) Descriptors
 */
#define RX_DESC_META			(1<<30)	/* Meta data */

/* Tx (M2S) word1 common Descriptors */
#define TX_DESC_META_OP_MASK		(0x3<<23)
#define TX_DESC_META_OP_SHIFT		(23)

/*
 * Crypto
 */
#define TX_DESC_META_CRYPT_DIR_SHIFT	(22)	/* Direction: enc/dec and/or sign/auth */
#define TX_DESC_META_CRYPT_S_GET_SA	(1<<21)	/* Evict SA */
#define TX_DESC_META_CRYPT_S_GET_ENCIV	(1<<20)	/* Get IV */
#define TX_DESC_META_CRYPT_GET_ORIG	(1<<19) /* Get Output original packet, after calc is done */
#define TX_DESC_META_CRYPT_GET_AUTHIV	(1<<18) /* Get Authentication IV */
#define TX_DESC_META_CRYPT_S_GET_SIGN	(1<<17)	/* Get Sign */

#define TX_DESC_META_AUTH_FIRST		(1<<16)	/* Auth only first bit */
#define TX_DESC_META_AUTH_LAST		(1<<15)	/* Auth only last bit */

#define TX_DESC_META_CMPRS_HIS_WIN_SND	(1<<16)	/* Cmprs send history window */
#define TX_DESC_META_CMPRS_DYN_DEFLATE_DEC	(1<<15)	/* Deflate dynamic decode preload */

#define TX_DESC_META_AUTH_VALID		(1<<14)	/* Validate Signature */

#define TX_DESC_META_CMPRS_HIS_WIN_RST	(1 << 13) /* Cmprs reset history window */

#define TX_DESC_META_SEND_PI		(1 << 12)

#define TX_DESC_META_SA_IDX_MASK	(0x7f<<5) /* SA index mask */
#define TX_DESC_META_SA_IDX_SHIFT	(5)

#define TX_DESC_META_BUF_TYPE_MASK	(0x7)/* Buffer type mask */
#define TX_DESC_META_BUF_TYPE_SHIFT	(0)

/* Tx (M2S) word1 Descriptors */
#define TX_DESC_META_CMPRS_ENC_AUTO_PADDING	(1 << 5)

#define TX_DESC_META_CMPRS_ENC_PADDING_RESOLUTION_MASK	(0x1f)
#define TX_DESC_META_CMPRS_ENC_PADDING_RESOLUTION_SHIFT	(0)

/* Tx (M2S) word2 Descriptors */
#define TX_DESC_META_ENC_OFF_MASK	(0xffff<<16)
#define TX_DESC_META_ENC_OFF_SHIFT	(16)
#define TX_DESC_META_ENC_OFF_EOP_MASK	(0xffff)
#define TX_DESC_META_ENC_OFF_EOP_SHIFT	(0)

/* Tx (M2S) word3 Descriptors */
#define TX_DESC_META_AUTH_OFF_MASK	(0xffff<<16)
#define TX_DESC_META_AUTH_OFF_SHIFT	(16)
#define TX_DESC_META_AUTH_OFF_EOP_MASK	(0xffff)
#define TX_DESC_META_AUTH_OFF_EOP_SHIFT	(0)

/*
 * Crypto DMA operation (Enc, Auth or Enc + Auth)
 */
#define AL_CRYPT_OP 3

/** Crypto DMA buffer types */
enum al_crypto_buf_type {
	AL_CRYPT_BUF_SA_UPDATE = 0,
	AL_CRYPT_BUF_ENC_IV = 1,
	AL_CRYPT_BUF_AUTH_IV = 2,
	AL_CRYPT_BUF_SRC = 3,
	AL_CRYPT_BUF_AUTH_SIGN = 4,
	AL_CRYPT_BUF_CMPRS_HIS_WIN = 5,
	AL_CRYPT_BUF_CMPRS_HFMN_TREE_TBL = 6,
	AL_CRYPT_BUF_PI = 7,
};

/**
 * Fill one rx submission descriptor
 *
 * @param rx_udma_q rx udma handle
 * @param flags flags for the descriptor
 * @param addr_tgtid destination buffer address with tgtid
 * @param len destination buffer length
 */
static INLINE
union al_udma_desc *al_crypto_prep_one_rx_desc(struct al_udma_q *rx_udma_q, uint32_t flags,
					       al_phys_addr_t addr_tgtid, uint32_t len)
{
	uint32_t flags_len = flags;
	union al_udma_desc *rx_desc;
	uint32_t ring_id;

	rx_desc = al_udma_desc_get(rx_udma_q);
	/* get ring id */
	ring_id = al_udma_ring_id_get(rx_udma_q)
		<< AL_S2M_DESC_RING_ID_SHIFT;

	flags_len |= ring_id;

	flags_len |= len & AL_S2M_DESC_LEN_MASK;
	rx_desc->rx.len_ctrl = swap32_to_le(flags_len);
	rx_desc->rx.buf1_ptr = swap64_to_le(addr_tgtid);
	return rx_desc;
}

/**
 * Set metadata for specified al_crypto_buf_type
 *
 * @param buf_type crypto DMA buffer type
 * @param word1_meta_ptr pointer to metadata word1
 */
static INLINE
void al_crypto_prep_tx_buf_type(enum al_crypto_buf_type buf_type, uint32_t *word1_meta_ptr)
{
	*word1_meta_ptr |= ((uint32_t)buf_type) << TX_DESC_META_BUF_TYPE_SHIFT;
}

/**
 * Initialize flags and metadata for first transaction descriptor
 *
 * @param dir crypto operation direction
 * @param sa_indx Security Association cache entry index to use
 * @param flags_ptr pointer to flags for the descriptor
 * @param word1_meta_ptr pointer to metadata word1 for the descriptor
 */
static INLINE
void al_crypto_prep_tx_begin(enum al_crypto_dir dir, uint32_t sa_indx,
			     uint32_t *flags_ptr, uint32_t *word1_meta_ptr)
{
	/* Set flags */
	*flags_ptr = AL_M2S_DESC_FIRST;

	/* Set first desc word1 metadata */
	*word1_meta_ptr = AL_CRYPT_OP << TX_DESC_META_OP_SHIFT;
	*word1_meta_ptr |= dir << TX_DESC_META_CRYPT_DIR_SHIFT;
	*word1_meta_ptr |= (sa_indx << TX_DESC_META_SA_IDX_SHIFT)
		& TX_DESC_META_SA_IDX_MASK;
}

/**
 * Reset flags and metadata after first descriptor
 *
 * @param flags_ptr pointer to flags for the descriptor
 * @param word1_meta_ptr pointer to metadata word1 for the descriptor
 */
static INLINE
void al_crypto_prep_tx_next(uint32_t *flags_ptr, uint32_t *word1_meta_ptr)
{
	*word1_meta_ptr = 0;
	/* clear first and DMB flags, keep no snoop hint flag */
	*flags_ptr &= AL_M2S_DESC_NO_SNOOP_H;
}

/**
 * Mark descriptor as last in transaction
 *
 * @param tx_desc_last tx descriptor to set last flag for
 */
static INLINE
void al_crypto_prep_tx_end(union al_udma_desc *tx_desc_last)
{
	tx_desc_last->tx.len_ctrl |= swap32_to_le(AL_M2S_DESC_LAST);
}

/**
 * Fill one tx submission descriptor
 *
 * @param tx_udma_q tx udma handle
 * @param flags flags for the descriptor
 * @param meta metadata word1 for the descriptor
 * @param addr_tgtid source buffer address with tgtid
 * @param len source buffer length
 */
static INLINE
union al_udma_desc *al_crypto_prep_one_tx_desc(struct al_udma_q *tx_udma_q,
					       uint32_t flags, uint32_t meta,
					       al_phys_addr_t addr_tgtid, uint32_t len)
{
	uint32_t flags_len = flags;
	union al_udma_desc *tx_desc;
	uint32_t ring_id;

	tx_desc = al_udma_desc_get(tx_udma_q);
	/* get ring id */
	ring_id = al_udma_ring_id_get(tx_udma_q)
		<< AL_M2S_DESC_RING_ID_SHIFT;

	flags_len |= ring_id;

	flags_len |= len & AL_M2S_DESC_LEN_MASK;
	tx_desc->tx.len_ctrl = swap32_to_le(flags_len);
	tx_desc->tx.meta_ctrl = swap32_to_le(meta);
	tx_desc->tx.buf_ptr = swap64_to_le(addr_tgtid);
	return tx_desc;
}

/**
 * Fill tx submission descriptor for SA update
 *
 * @param tx_udma_q tx udma handle
 * @param flags flags for the descriptor
 * @param word1_meta metadata word1 for the descriptor
 * @param addr_tgtid source buffer address with tgtid
 * @param len source buffer length
 */
static INLINE
union al_udma_desc *al_crypto_prep_tx_sa_update(struct al_udma_q *tx_udma_q,
						uint32_t flags, uint32_t word1_meta,
						al_phys_addr_t addr_tgtid,
						uint32_t len)
{
	/* update buffer type in metadata */
	al_crypto_prep_tx_buf_type(AL_CRYPT_BUF_SA_UPDATE, &word1_meta);

	return al_crypto_prep_one_tx_desc(tx_udma_q, flags, word1_meta, addr_tgtid, len);
}

/**
 * Fill tx submission descriptor for encryption Initialization Vector update
 *
 * @param tx_udma_q tx udma handle
 * @param flags flags for the descriptor
 * @param word1_meta metadata word1 for the descriptor
 * @param addr_tgtid source buffer address with tgtid
 * @param len source buffer length
 */
static INLINE
union al_udma_desc *al_crypto_prep_tx_enc_iv(struct al_udma_q *tx_udma_q,
					     uint32_t flags, uint32_t word1_meta,
					     al_phys_addr_t addr_tgtid,
					     uint32_t len)
{
	/* update buffer type in metadata */
	al_crypto_prep_tx_buf_type(AL_CRYPT_BUF_ENC_IV, &word1_meta);

	return al_crypto_prep_one_tx_desc(tx_udma_q, flags, word1_meta, addr_tgtid, len);
}

/**
 * Fill tx submission descriptor for authentication Initialization Vector update
 *
 * @param tx_udma_q tx udma handle
 * @param flags flags for the descriptor
 * @param word1_meta metadata word1 for the descriptor
 * @param addr_tgtid source buffer address with tgtid
 * @param len source buffer length
 */
static INLINE
union al_udma_desc *al_crypto_prep_tx_auth_iv(struct al_udma_q *tx_udma_q,
					      uint32_t flags, uint32_t word1_meta,
					      al_phys_addr_t addr_tgtid,
					      uint32_t len)
{
	/* update buffer type in metadata */
	al_crypto_prep_tx_buf_type(AL_CRYPT_BUF_AUTH_IV, &word1_meta);

	return al_crypto_prep_one_tx_desc(tx_udma_q, flags, word1_meta, addr_tgtid, len);
}

/**
 * Fill tx submission descriptor for protection info
 *
 * @param tx_udma_q tx udma handle
 * @param flags flags for the descriptor
 * @param word1_meta metadata word1 for the descriptor
 * @param addr_tgtid source buffer address with tgtid
 * @param len source buffer length
 */
static INLINE
union al_udma_desc *al_crypto_prep_tx_protection_info(struct al_udma_q *tx_udma_q,
						      uint32_t flags, uint32_t word1_meta,
						      al_phys_addr_t addr_tgtid,
						      uint32_t len)
{
	/* update buffer type in metadata */
	al_crypto_prep_tx_buf_type(AL_CRYPT_BUF_PI, &word1_meta);

	return al_crypto_prep_one_tx_desc(tx_udma_q, flags, word1_meta, addr_tgtid, len);
}

/**
 * Fill tx submission descriptor for compression history window
 *
 * @param tx_udma_q tx udma handle
 * @param flags flags for the descriptor
 * @param word1_meta metadata word1 for the descriptor
 * @param addr_tgtid source buffer address with tgtid
 * @param len source buffer length
 */
static INLINE
union al_udma_desc *al_crypto_prep_tx_cmprs_his_win(struct al_udma_q *tx_udma_q,
						    uint32_t flags, uint32_t word1_meta,
						    al_phys_addr_t addr_tgtid,
						    uint32_t len)
{
	/* update buffer type in metadata */
	al_crypto_prep_tx_buf_type(AL_CRYPT_BUF_CMPRS_HIS_WIN, &word1_meta);

	return al_crypto_prep_one_tx_desc(tx_udma_q, flags, word1_meta, addr_tgtid, len);
}

/**
 * Fill tx submission descriptor for compression deflate tree table
 *
 * @param tx_udma_q tx udma handle
 * @param flags flags for the descriptor
 * @param word1_meta metadata word1 for the descriptor
 * @param addr_tgtid source buffer address with tgtid
 * @param len source buffer length
 */
static INLINE
union al_udma_desc *al_crypto_prep_tx_cmprs_hfmn_tree_tbl(struct al_udma_q *tx_udma_q,
							  uint32_t flags, uint32_t word1_meta,
							  al_phys_addr_t addr_tgtid,
							  uint32_t len)
{
	/* update buffer type in metadata */
	al_crypto_prep_tx_buf_type(AL_CRYPT_BUF_CMPRS_HFMN_TREE_TBL, &word1_meta);

	return al_crypto_prep_one_tx_desc(tx_udma_q, flags, word1_meta, addr_tgtid, len);
}


/**
 * Fill tx submission descriptor for first payload source buffer
 *
 * @param tx_udma_q tx udma handle
 * @param flags flags for the descriptor
 * @param word1_meta metadata word1 for the descriptor
 * @param addr_tgtid source buffer address with tgtid
 * @param len source buffer length
 */
static INLINE
union al_udma_desc *al_crypto_prep_tx_src(struct al_udma_q *tx_udma_q,
					  uint32_t flags, uint32_t word1_meta,
					  al_phys_addr_t addr_tgtid,
					  uint32_t len)
{
	/* update buffer type in metadata */
	al_crypto_prep_tx_buf_type(AL_CRYPT_BUF_SRC, &word1_meta);

	return al_crypto_prep_one_tx_desc(tx_udma_q, flags, word1_meta, addr_tgtid, len);
}

/**
 * Fill tx submission descriptor for concatenating buffers
 *
 * @param tx_udma_q tx udma handle
 * @param flags flags for the descriptor
 * @param word1_meta metadata word1 for the descriptor
 * @param addr_tgtid source buffer address with tgtid
 * @param len source buffer length
 */
static INLINE
union al_udma_desc *al_crypto_prep_tx_concat(struct al_udma_q *tx_udma_q,
					     uint32_t flags, uint32_t word1_meta,
					     al_phys_addr_t addr_tgtid,
					     uint32_t len)
{
	/* update buffer type in metadata */
	flags |= AL_M2S_DESC_CONCAT;

	return al_crypto_prep_one_tx_desc(tx_udma_q, flags, word1_meta, addr_tgtid, len);
}

/**
 * Fill tx submission descriptor for authentication signature
 *
 * @param tx_udma_q tx udma handle
 * @param flags flags for the descriptor
 * @param word1_meta metadata word1 for the descriptor
 * @param addr_tgtid source buffer address with tgtid
 * @param len source buffer length
 */
static INLINE
union al_udma_desc *al_crypto_prep_tx_auth_sign(struct al_udma_q *tx_udma_q,
						uint32_t flags, uint32_t word1_meta,
						al_phys_addr_t addr_tgtid,
						uint32_t len)
{
	/* update buffer type in metadata */
	al_crypto_prep_tx_buf_type(AL_CRYPT_BUF_AUTH_SIGN, &word1_meta);

	return al_crypto_prep_one_tx_desc(tx_udma_q, flags, word1_meta, addr_tgtid, len);
}

#endif
