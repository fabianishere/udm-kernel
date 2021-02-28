/*
 * Copyright 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 */

#include "samples.h"

#include "al_hal_ssm_crypto.h"
#include "al_hal_ssm_crypto_desc.h"

/**
 * @defgroup group_fast_crypto_samples Code Samples
 * @ingroup group_fast_crypto
 * @{
 * fast_crypto.c: this file can be found in samples directory.
 *
 * al_crypto_prep_tx_* API consists of two parts:
 * - Tracking of flags and meta fields in hardware descriptor
 * - Preparation of hardware descriptor
 *
 * @code */

static uint32_t fast_crypto_prep_enc_tx(struct al_udma_q *tx_udma_q,
					uint32_t sa_index,
					struct al_buf *iv,
					struct al_buf *bufs,
					unsigned int bufs_count)
{
	union al_udma_desc *tx_desc;	/* last prepared descriptor */
	uint32_t flags, meta;
	uint32_t tx_desc_count;		/* number of descriptors prepared */
	unsigned int i;


	/* Initialize flags and meta for the first descriptor in transaction */
	al_crypto_prep_tx_begin(AL_CRYPT_ENCRYPT, sa_index, &flags, &meta);
	tx_desc_count = 0;

	/* Assume that SA cahe entry sa_index is already initialized */

	if (iv != NULL) {
		/* Prepare descriptor for encryption IV */
		tx_desc = al_crypto_prep_tx_enc_iv(tx_udma_q, flags, meta,
						   iv->addr, iv->len);
		/* Set up flags and meta for consequent descriptors in transaction */
		al_crypto_prep_tx_next(&flags, &meta);
		tx_desc_count += 1;
	}

	/* Prepare descriptor for first payload buffer */
	tx_desc = al_crypto_prep_tx_src(tx_udma_q, flags, meta,
					bufs[0].addr, bufs[0].len);
	/* Set up flags and meta for consequent descriptors in transaction */
	al_crypto_prep_tx_next(&flags, &meta);
	tx_desc_count += 1;

	for (i = 1; i < bufs_count; i++) {
		/* Prepare concatenate descriptors remaining payload buffers */
		tx_desc = al_crypto_prep_tx_concat(tx_udma_q, flags, meta,
						   bufs[i].addr, bufs[i].len);
		/* al_crypto_prep_tx_next() is not required as flags and meta haven't changed */
		tx_desc_count += 1;
	}


	/* Mark tx_desc as last descriptor in transaction. */
	al_crypto_prep_tx_end(tx_desc);

	/*
	 * Return without flushing the queue. The following may be use to submit descriptors:
	 * al_reg_write32_relaxed(&tx_udma_q->q_regs->rings.drtp_inc, tx_desc_count);
	*/
	return tx_desc_count;
}

static int fast_crypto_init(struct al_ssm_dma *crypto_dma)
{
	struct al_ssm_dma_params crypto_dma_params;
	struct al_udma_q_params tx_params;
	struct al_udma_q_params rx_params;
	void *tx_dma_desc_virt, *rx_dma_desc_virt, *rx_dma_cdesc_virt;
	al_phys_addr_t tx_dma_desc_phys, rx_dma_desc_phys, rx_dma_cdesc_phys;
	int rc;
	int descs_num = 8;
	int tx_cdesc_size = 16; /* Validate with documentation! */
	int rx_cdesc_size = 16; /* Validate with documentation! */

	/*
	 * Replace with actual regs base after PCI resource allocation
	 * Device ID is extracted from the PCI configuration space address 0, bits 31:16
	 * Revision ID is extracted from the PCI configuration space address 8, bits 7:0
	 */
	crypto_dma_params.rev_id = AL_CRYPTO_REV_ID_0;
	crypto_dma_params.udma_regs_base = NULL;

	crypto_dma_params.name = malloc(strlen("fast_crypto_dev_name") + 1);
	if (!crypto_dma_params.name)
		return -1;

	strcpy(crypto_dma_params.name, "fast_crypto_dev_name");
	crypto_dma_params.num_of_queues = 4;

	/* Init crypto dma */
	rc = al_ssm_dma_init(crypto_dma, &crypto_dma_params);
	if (!rc)
		return rc;

	/* Start dma engine */
	rc = al_ssm_dma_state_set(crypto_dma, UDMA_NORMAL);

	/* Allocate dma queue memory */
	/* Allocate memory for Tx submission descriptors */
	tx_dma_desc_virt = malloc(descs_num * sizeof(union al_udma_desc));
	/* Use os dma_map service to map tx_dma_desc_phys
	 * Here we just copy */
	tx_dma_desc_phys = (al_phys_addr_t)(unsigned long)tx_dma_desc_virt;

	/* Allocate memory for Rx submission descriptors */
	rx_dma_desc_virt = malloc(descs_num * sizeof(union al_udma_desc));
	/* Use os dma_map service to map rx_dma_desc_phys
	 * Here we just copy */
	rx_dma_desc_phys = (al_phys_addr_t)(unsigned long)rx_dma_desc_virt;

	/* Allocate memory for Rx completion descriptors */
	rx_dma_cdesc_virt = malloc(descs_num * rx_cdesc_size);
	/* Use os dma_map service to map rx_dma_desc_phys
	 * Here we just copy */
	rx_dma_cdesc_phys = (al_phys_addr_t)(unsigned long)rx_dma_cdesc_virt;

	/* Fill in dma queue params */
	tx_params.size = descs_num;
	tx_params.desc_base = (union al_udma_desc *)tx_dma_desc_virt;
	tx_params.desc_phy_base = tx_dma_desc_phys;
	tx_params.cdesc_base = NULL; /* don't use Tx completion ring */
	tx_params.cdesc_phy_base = 0;
	tx_params.cdesc_size = tx_cdesc_size;

	rx_params.size = descs_num;
	rx_params.desc_base = (union al_udma_desc *)rx_dma_desc_virt;
	rx_params.desc_phy_base = rx_dma_desc_phys;
	rx_params.cdesc_base = (uint8_t *)rx_dma_cdesc_virt;
	rx_params.cdesc_phy_base = rx_dma_cdesc_phys;
	rx_params.cdesc_size = rx_cdesc_size;

	/* Init dma queue 0 using the params above */
	rc = al_ssm_dma_q_init(crypto_dma, 0,
			       &tx_params, &rx_params,
			       AL_CRYPT_AUTH_Q);
	return rc;
}

int main(void)
{
	struct al_ssm_dma crypto_dma;
	struct al_buf iv;
	struct al_buf tx_buf;
	void *iv_virt;
	void *tx_buf_virt;
	uint32_t tx_desc_count;
	int rc;

	rc = fast_crypto_init(&crypto_dma);
	al_assert(rc == 0);

	iv.len = 16;
	iv_virt = malloc(iv.len);
	iv.addr = (al_phys_addr_t)(unsigned long)iv_virt;

	tx_buf.len = 16;
	tx_buf_virt = malloc(tx_buf.len);
	tx_buf.addr = (al_phys_addr_t)(unsigned long)tx_buf_virt;

	tx_desc_count = fast_crypto_prep_enc_tx(&crypto_dma.m2m_udma.tx_udma.udma_q[0],
						0, &iv, &tx_buf, 1);
	al_assert(tx_desc_count != 0);

	return 0;
}

/** @endcode */

/** @} */
