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
#include "stdlib.h"

/**
 * @defgroup group_crypto_samples Code Samples
 * @ingroup group_crypto
 * @{
 * crypto.c: this file can be found in samples directory.
 *
 * @code */
#include "samples.h"
#include "al_hal_ssm.h"
#include "al_hal_ssm_crypto.h"

static struct al_ssm_dma_params crypto_dma_params;
static struct al_ssm_dma crypto_dma;

/* Crypto initialization flow */
int crypto_init(void)
{
	int rc;
	struct al_udma_q_params tx_params;
	struct al_udma_q_params rx_params;
	void *tx_dma_desc_virt, *rx_dma_desc_virt, *rx_dma_cdesc_virt;
	al_phys_addr_t tx_dma_desc_phys, rx_dma_desc_phys, rx_dma_cdesc_phys;
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

	strcpy(crypto_dma_params.name, "crypto_dev_name");
	crypto_dma_params.num_of_queues = 4;

	/* Init crypto dma */
	rc = al_ssm_dma_init(&crypto_dma, &crypto_dma_params);
	if (!rc)
		return rc;

	/* Start dma engine */
	rc = al_ssm_dma_state_set(&crypto_dma, UDMA_NORMAL);

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
	rc = al_ssm_dma_q_init(&crypto_dma, 0,
				&tx_params, &rx_params,
				AL_CRYPT_AUTH_Q);
	return rc;
}

/* Crypto encryption transaction submission */
int crypto_xaction_submit(const uint8_t *key, size_t keylen,
		al_phys_addr_t buf, uint32_t nbytes)
{
	int rc;
	struct al_crypto_sa sa;
	struct al_crypto_hw_sa hw_sa;
	al_phys_addr_t hw_sa_dma_addr;
	struct al_crypto_transaction xaction;
	uint32_t comp_status;
	struct al_buf src_buf, dst_buf;

	memset(&sa, 0, sizeof(struct al_crypto_sa));

	/* Fill in the necessary sa fields */
	sa.enc_type = AL_CRYPT_AES_ECB;
	sa.sa_op = AL_CRYPT_ENC_ONLY;
	sa.aes_ksize = AL_CRYPT_AES_128;
	memcpy(&sa.enc_key, key, keylen);
	/* ... */

	/* Init hw sa struct, will be passed with the transaction */
	al_crypto_hw_sa_init(&sa, &hw_sa);

	/* Use os dma_map service to map hw_sa
	 * Here we just copy */
	hw_sa_dma_addr = (al_phys_addr_t)(unsigned long)&hw_sa;

	/* Fill in the necessary transaction fields */
	memset(&xaction, 0, sizeof(struct al_crypto_transaction));
	xaction.dir = AL_CRYPT_ENCRYPT;
	xaction.sa_indx = 0;
	xaction.sa_in.len = sizeof(struct al_crypto_hw_sa);
	xaction.sa_in.addr = hw_sa_dma_addr;

	xaction.src_size = xaction.enc_in_len = nbytes;

	src_buf.addr = buf;
	src_buf.len = nbytes;
	xaction.src.bufs = &src_buf;
	xaction.src.bufs->len = nbytes;
	xaction.src.num = 1;

	dst_buf.addr = buf;
	dst_buf.len = nbytes;
	xaction.dst.bufs = &dst_buf;
	xaction.dst.num = 1;

	/* If interrupts are used, set AL_CRYPT_INTERRUPT flag */
	/* xaction->flags = AL_CRYPT_INTERRUPT; */

	/* Send crypto transaction to dma queue 0 */
	rc = al_crypto_dma_prepare(&crypto_dma, 0,
				&xaction);
	if (rc) {
		/* error handling */
		return rc;
	}

	/* Trigger transaction execution */
	al_crypto_dma_action(&crypto_dma, 0, xaction.tx_descs_count);

	/* Here we handle completion using polling, in real system interrupts
	 * can be used */
	while(!al_crypto_dma_completion(&crypto_dma, 0, &comp_status));

	if (comp_status) {
		/* error handling */
	}

	return 0;
}

/** @endcode */

/** @} */
