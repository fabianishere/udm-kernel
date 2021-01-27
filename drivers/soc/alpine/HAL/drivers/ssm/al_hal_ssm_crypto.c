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
#include "al_hal_ssm_crypto.h"
#include "al_hal_ssm_crypto_regs.h"
#include "al_hal_ssm_crypto_desc.h"
#include "al_hal_ssm_crypto_internal.h"
#include "al_hal_udma_iofic.h"

#define RX_COMP_STATUS_MASK	(AL_CRYPT_AUTH_ERROR			| \
				 AL_CRYPT_SA_IV_EVICT_FIFO_ERROR	| \
				 AL_CRYPT_DES_ILLEGAL_KEY_ERROR		| \
				 AL_CRYPT_M2S_ERROR			| \
				 AL_CRYPT_SRAM_PARITY_ERROR		| \
				 AL_CRYPT_INTERNAL_FLOW_VIOLATION_ERROR)

/*
 * SA
 */

/* Crypto Application Base offset in Alpine V3 */
#define AL_CRYPT_REGS_OFF_IN_APP_REGS_REV3			0x10000

/* Crypto operation check macros */
#define AL_CRYPT_OP_IS(op, is)		((op) == (is))
#define AL_CRYPT_OP_HAS(op, has)	((op) & (has))

/* Word 0 */
#define CRYPT_SAD_OP_MASK		(0x7<<29)/* Crypto Operation */
#define CRYPT_SAD_OP_SHIFT		(29)
#define CRYPT_SAD_OP_AUTH		(1 << 2)
#define CRYPT_SAD_OP_ENC		(1 << 1)
#define CRYPT_SAD_OP_CMPRS		(1 << 0)
#define CRYPT_SAD_ENC_TYPE_MASK		(0xf<<25)/* Crypto Type */
#define CRYPT_SAD_ENC_TYPE_SHIFT	(25)
#define CRYPT_SAD_SA_LENGTH_MASK	(0x3<<23)/* SA Length */
#define CRYPT_SAD_SA_LENGTH_SHIFT	(23)
#define CRYPT_SAD_TRIPDES_MODE_MASK	(0x1<<22)/* 3DES mode */
#define CRYPT_SAD_TRIPDES_MODE_SHIFT	(22)
#define CRYPT_SAD_AES_KEY_SIZE_MASK	(0x3<<20)/* AES key size */
#define CRYPT_SAD_AES_KEY_SIZE_SHIFT	(20)
#define CRYPT_SAD_AUTH_TYPE_MASK	(0xf<<12)/* Auth type */
#define CRYPT_SAD_AUTH_TYPE_SHIFT	(12)
#define CRYPT_SAD_SIGN_SIZE_MASK	(0xf<<8) /* Signature size */
#define CRYPT_SAD_SIGN_SIZE_SHIFT	(8)
#define CRYPT_SAD_SHA2_MODE_MASK	(0x3<<6) /* Sha2 mode */
#define CRYPT_SAD_SHA2_MODE_SHIFT	(6)
#define CRYPT_SAD_SHA3_MODE_MASK	(0x3<<6) /* Sha3 mode */
#define CRYPT_SAD_SHA3_MODE_SHIFT	(6)
#define CRYPT_SAD_SHA3_MODE_EXT_MASK	(0x3<<6) /* Sha3 mode extension */
#define CRYPT_SAD_SHA3_MODE_EXT_SHIFT	(6)
#define CRYPT_SAD_CRC8_MODE_MASK	(0x3<<6) /* CRC8 mode */
#define CRYPT_SAD_CRC8_MODE_SHIFT	(6)
#define CRYPT_SAD_CRC16_MODE_MASK	(0x3<<6) /* CRC16 mode */
#define CRYPT_SAD_CRC16_MODE_SHIFT	(6)
#define CRYPT_SAD_CRC32_MODE_MASK	(0x3<<6) /* CRC32 mode */
#define CRYPT_SAD_CRC32_MODE_SHIFT	(6)
#define CRYPT_SAD_CSUM32_MODE_MASK	(0x3<<6) /* CSUM32 mode */
#define CRYPT_SAD_CSUM32_MODE_SHIFT	(6)
#define CRYPT_SAD_HMAC_EN		(1<<5)   /* Hmac enable */
#define CRYPT_SAD_SIGN_AFTER_ENC	(1<<4)   /* Sign after encryption */
#define CRYPT_SAD_AUTH_AFTER_DEC	(1<<3)   /* Auth after decryption */
#define CRYPT_SAD_AUTH_MSB_BITS         (1<<2)   /* Auth use the MSB of the signature */
#define CRYPT_SAD_CNTR_SIZE_MASK	(0x3)    /* Counter size */
#define CRYPT_SAD_CNTR_SIZE_SHIFT	(0)
/**
* Note:
*   CRYPT_SAD_CTS_MODE_MASK is allowed to overlap with CRYPT_SAD_CNTR_SIZE_MASK,
*   since counter-based cipher modes never need to support cipher-text stealing
*/
#define CRYPT_SAD_CTS_MODE_MASK		(0x3)    /* CTS Mode */
#define CRYPT_SAD_CTS_MODE_SHIFT	(0)

/* Word 1 */
#define CRYPT_SAD_CMPRS_LZ_PROF_IDX_MASK	AL_FIELD_MASK(31, 27)
#define CRYPT_SAD_CMPRS_LZ_PROF_IDX_SHIFT	27
#define CRYPT_SAD_CMPRS_DYNAMIC_DEFLATE_TABLE_IDX_MASK	AL_FIELD_MASK(24, 24)
#define CRYPT_SAD_CMPRS_DYNAMIC_DEFLATE_TABLE_IDX_SHIFT	24
#define CRYPT_SAD_CMPRS_FORCE_COMPRESSED_OUT_MASK		AL_BIT(22)
#define CRYPT_SAD_CMPRS_FORCE_COMPRESSED_OUT_SHIFT		22
#define CRYPT_SAD_GMAC_MASK_ID_MASK		(0x3<<12) /* GMAC mask id */
#define CRYPT_SAD_GMAC_MASK_ID_SHIFT		(12)
#define CRYPT_SAD_ICRC_PROF_ID_MASK		(0xf<<12) /* Inline CRC profile id */
#define CRYPT_SAD_ICRC_PROF_ID_SHIFT		(12)
/*
 * This enables routing the calculated CRC/signature back to the compression/encryption engines.
 * Typically used for PI applications, where the sector and
 * the PI-metadata get compressed/encrypted as a single blob.
 */
#define CRYPT_SAD_ENCYPRTED_SIGNATURE  AL_BIT(11)
#define CRYPT_SAD_PI_SIZE_MASK  AL_FIELD_MASK(10, 9)
#define CRYPT_SAD_PI_SIZE_SHIFT 9
#define CRYPT_SAD_PI_CRC_OFFSET_MASK  AL_FIELD_MASK(7, 4)
#define CRYPT_SAD_PI_CRC_OFFSET_SHIFT 4

/* Word 1 - Encryption offsets */
/* Word 2 */
#define CRYPT_SAD_ENC_OFF_MASK		(0xffff<<16)/*Enc off- start of pkt*/
#define CRYPT_SAD_ENC_OFF_SHIFT		(16)
#define CRYPT_SAD_ENC_OFF_EOP_MASK	(0xffff)/*Enc off- end of pkt*/
#define CRYPT_SAD_ENC_OFF_EOP_SHIFT	(0)

/* Authentication offsets */
#define CRYPT_SAD_AUTH_OFF_MASK		(0xffff<<16) /*Auth off- start of pkt*/
#define CRYPT_SAD_AUTH_OFF_SHIFT	(16)
#define CRYPT_SAD_AUTH_OFF_EOP_MASK	(0xffff)    /*Auth off- end of pkt*/
#define CRYPT_SAD_AUTH_OFF_EOP_SHIFT	(0)

/* Other words */
#define CRYPT_SAD_ENC_KEY_SWORD		(4)  /* Encryption Key */
#define CRYPT_SAD_ENC_KEY_SIZE		(8)
#define CRYPT_SAD_ENC_IV_SWORD		(12) /* Encryption IV */
#define CRYPT_SAD_ENC_IV_SIZE		(4)  /* Engine update this field */
#define CRYPT_SAD_GCM_AUTH_IV_SWORD	(16) /* GCM Auth IV */
#define CRYPT_SAD_GCM_AUTH_IV_SIZE	(4)
#define CRYPT_SAD_AUTH_IV_SWORD		(12) /* Auth Only IV */
#define CRYPT_SAD_AUTH_IV_SIZE		(16) /* Engine update this field */
#define CRYPT_SAD_HMAC_IV_IN_SWORD	(28) /* HMAC_IV_in H(k xor ipad) */
#define CRYPT_SAD_HMAC_IV_IN_SIZE	(16)
#define CRYPT_SAD_HMAC_IV_OUT_SWORD	(44) /* HMAC_IV_out H(k xor opad) */
#define CRYPT_SAD_HMAC_IV_OUT_SIZE	(16)
/* XTS support*/
#define CRYPT_SAD_XTS_SEC_SIZE_WORD	(16)   /* XTS sector size */
#define CRYPT_SAD_XTS_SEC_SIZE_MASK	(0xff) /*Auth off- start of pkt*/
#define CRYPT_SAD_XTS_SEC_SIZE_SHIFT	(0)
#define CRYPT_SAD_XTS_PROTECTION_WORD	(16)   /* XTS protection info (PI) size */
#define CRYPT_SAD_XTS_PROTECTION_MASK	(0xff<<8)
#define CRYPT_SAD_XTS_PROTECTION_SHIFT	(8)
#define CRYPT_SAD_XTS_TWEAK_KEY_SWORD	(20)   /* XTS Enc Tweak Key */
#define CRYPT_SAD_XTS_TWEAK_KEY_SIZE	(8)
/* GMAC Support */
#define CRYPT_SAD_GMAC_IV_IN_SWORD	(28) /* GMAC IV In {J0,32'b1} */
#define CRYPT_SAD_GMAC_IV_IN_SIZE	(4)
#define CRYPT_SAD_GMAC_H_IN_SWORD	(32) /* GMAC H SubKey In Enc{128'b0,AES Key,AES Key Size} */
#define CRYPT_SAD_GMAC_H_IN_SIZE	(4)
#define CRYPT_SAD_GMAC_AES_KEY_IN_SWORD	(36) /* GMAC AES Key */
#define CRYPT_SAD_GMAC_AES_KEY_IN_SIZE	(8)
/* ICRC Support */
#define CRYPT_SAD_ICRC_IV_IN_SWORD	(28) /* Inline CRC IV */
#define CRYPT_SAD_ICRC_IV_IN_SIZE	(1)

/* crypto error interrupts */
#define AL_CRYPTO_IOFIC_GROUP_A_ERROR_INTS \
	(AL_CRYPTO_APP_INT_A_S2M_TIMEOUT | \
	 AL_CRYPTO_APP_INT_A_M2S_TIMEOUT | \
	 AL_CRYPTO_APP_INT_A_EOP_WITHOUT_SOP | \
	 AL_CRYPTO_APP_INT_A_SOP_WITHOUT_EOP | \
	 AL_CRYPTO_APP_INT_A_SOP_WITH_EOP_TOGETHER | \
	 AL_CRYPTO_APP_INT_A_UNMAP_PROTOCOL | \
	 AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_SA | \
	 AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_IV | \
	 AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_ICV | \
	 AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_DMB_FUNC_0 | \
	 AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_DMB_FUNC_1 | \
	 AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_DMB_FUNC_2 | \
	 AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_DMB_FUNC_3 | \
	 AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_E2M_PAYLOAD | \
	 AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_E2M_SIGNATURE | \
	 AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_D2E_DATA | \
	 AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_D2E_CONTROL | \
	 AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_INGRESS | \
	 AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_ALIGNER | \
	 AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_D2E_CONTEXT | \
	 AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_D2E_HASH_ST | \
	 AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_D2E_OPAD | \
	 AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_E2D_WRITE_B | \
	 AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_E2M_INTR)

#define AL_CRYPTO_IOFIC_GROUP_A_ERROR_INTS_V3 \
	(AL_CRYPTO_IOFIC_GROUP_A_ERROR_INTS | \
	 AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_D2E_GMAC_STATE | \
	 AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_D2E_COMP_HIST | \
	 AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_D2E_HUFFMAN_D_TBL)

#define AL_CRYPTO_IOFIC_GROUP_B_ERROR_INTS_V2 \
	(AL_CRYPTO_APP_INT_B_SA_IV_FIFO_OUT_EMPTY | \
	 AL_CRYPTO_APP_INT_B_M2S_ERROR | \
	 AL_CRYPTO_APP_INT_B_PAR_ERR_SAD_MEMORIES)

#define AL_CRYPTO_IOFIC_GROUP_B_ERROR_INTS_V3 \
	(AL_CRYPTO_IOFIC_GROUP_B_ERROR_INTS_V2 | \
	 AL_CRYPTO_APP_INT_B_PAR_ERR_SAD_AUTH_IV_IN | \
	 AL_CRYPTO_APP_INT_B_PAR_ERR_SAD_ENC_KEY_AUTH_IV_OUT | \
	 AL_CRYPTO_APP_INT_B_PAR_ERR_SAD_ENC_PARAMS)

#define AL_CRYPTO_IOFIC_GROUP_C_ERROR_INTS \
	(AL_CRYPTO_APP_INT_C_PAR_ERR_DECOMP_HIST(0) | \
	 AL_CRYPTO_APP_INT_C_PAR_ERR_DECOMP_HIST(1) | \
	 AL_CRYPTO_APP_INT_C_PAR_ERR_DECOMP_HIST(2) | \
	 AL_CRYPTO_APP_INT_C_PAR_ERR_DECOMP_HIST(3) | \
	 AL_CRYPTO_APP_INT_C_PAR_ERR_COMP_OUT_CONT_COMP | \
	 AL_CRYPTO_APP_INT_C_PAR_ERR_COMP_OUT_CONT_ORIG | \
	 AL_CRYPTO_APP_INT_C_PAR_ERR_COMP_HIST(0) | \
	 AL_CRYPTO_APP_INT_C_PAR_ERR_COMP_HIST(1) | \
	 AL_CRYPTO_APP_INT_C_PAR_ERR_COMP_HIST(2) | \
	 AL_CRYPTO_APP_INT_C_PAR_ERR_COMP_HIST(3) | \
	 AL_CRYPTO_APP_INT_C_PAR_ERR_COMP_HIST(4) | \
	 AL_CRYPTO_APP_INT_C_PAR_ERR_COMP_HIST(5) | \
	 AL_CRYPTO_APP_INT_C_PAR_ERR_COMP_HIST(6) | \
	 AL_CRYPTO_APP_INT_C_PAR_ERR_COMP_HIST(7) | \
	 AL_CRYPTO_APP_INT_C_PAR_ERR_COMP_HIST(8) | \
	 AL_CRYPTO_APP_INT_C_PAR_ERR_COMP_HIST(9) | \
	 AL_CRYPTO_APP_INT_C_PAR_ERR_COMP_HIST(10) | \
	 AL_CRYPTO_APP_INT_C_PAR_ERR_COMP_HIST(11) | \
	 AL_CRYPTO_APP_INT_C_PAR_ERR_COMP_HIST(12) | \
	 AL_CRYPTO_APP_INT_C_PAR_ERR_COMP_HIST(13) | \
	 AL_CRYPTO_APP_INT_C_PAR_ERR_COMP_HIST(14) | \
	 AL_CRYPTO_APP_INT_C_PAR_ERR_COMP_HIST(15) | \
	 AL_CRYPTO_APP_INT_C_LZ77_LZSS_COMP_CAM_ENT_NOT_FOUND | \
	 AL_CRYPTO_APP_INT_C_LZ77_LZSS_COMP_INVALID_ALU_CMD | \
	 AL_CRYPTO_APP_INT_C_COMP_HIST_EXCEEDED | \
	 AL_CRYPTO_APP_INT_C_DECOMP_INVALID_FIELD_COMPOS | \
	 AL_CRYPTO_APP_INT_C_DECOMP_INVALID_FIELD_SIZE | \
	 AL_CRYPTO_APP_INT_C_DECOMP_OFFSET_NOT_RESOLVED | \
	 AL_CRYPTO_APP_INT_C_DECOMP_ZERO_CODE_WORD_CNT)

#define sa_init_field(dest, val, mask, shift, str)\
	do {\
		al_assert(!((val << shift) & ~(mask)));\
		al_debug(" SA %s - %x\n", str, val); \
		dest |= (val << shift) & mask;\
	} while (0);


#define ADDR_TGTID(addr, tgtid_shifted)		((addr) | (tgtid_shifted))

/**
 * DEBUG
 */
#ifdef CRYPTO_DEBUG
static void al_print_crypto_desc(union al_udma_desc *desc)
{
	al_dbg(" Crypto: Desc: %08x %08x %08x %08x\n",
			desc->tx_meta.len_ctrl, desc->tx_meta.meta_ctrl,
			desc->tx_meta.meta1, desc->tx_meta.meta2);
}

static
void al_print_crypto_xaction(struct al_crypto_transaction *xaction)
{
	unsigned int i;

	al_dbg("Crypto: Transaction debug\n");
	al_dbg(" Direction %s\n",
		(xaction->dir == AL_CRYPT_ENCRYPT) ? "Encrypt" : "Decrypt");
	al_dbg(" Flags %d\n", xaction->flags);

	al_dbg("-SRC buf size %d num of buffers  %d\n",
		xaction->src_size, xaction->src.num);
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

	al_dbg("-SA index %d address 0x%016" PRIx64 " len %d\n",
		xaction->sa_indx, xaction->sa_in.addr,
		xaction->sa_in.len);
	al_dbg(" SA OUT size: %d , addr 0x%016" PRIx64 "\n",
		xaction->sa_out.len,
		xaction->sa_out.addr);

	al_dbg("-Enc IV IN size: %d, addr 0x%016" PRIx64 "\n",
		xaction->enc_iv_in.len,
		xaction->enc_iv_in.addr);
	al_dbg(" Enc IV OUT size: %d, addr 0x%016" PRIx64 "\n",
		xaction->enc_iv_out.len,
		xaction->enc_iv_out.addr);
	al_dbg(" Enc Next IV OUT size: %d, addr 0x%016" PRIx64 "\n",
		xaction->enc_next_iv_out.len,
		xaction->enc_next_iv_out.addr);
	al_dbg(" Enc Offset %d EOP valid %d EOP %d Len %d\n",
		xaction->enc_in_off, xaction->enc_in_off_eop_valid, xaction->enc_in_off_eop,
		xaction->enc_in_len);

	al_dbg("-Auth fl_valid %d, first %d last %d\n",
		xaction->auth_fl_valid, xaction->auth_first,
		xaction->auth_last);
	al_dbg(" Auth IV IN size: %d, addr 0x%016" PRIx64 "\n",
		xaction->auth_iv_in.len,
		xaction->auth_iv_in.addr);
	al_dbg(" Auth IV OUT size: %d, addr 0x%016" PRIx64 "\n",
		xaction->auth_iv_out.len,
		xaction->auth_iv_out.addr);
	al_dbg(" Auth SIGN IN size: %d, addr 0x%016" PRIx64 "\n",
		xaction->auth_sign_in.len,
		xaction->auth_sign_in.addr);
	al_dbg(" Auth SIGN OUT size: %d, addr 0x%016" PRIx64 "\n",
		xaction->auth_sign_out.len,
		xaction->auth_sign_out.addr);
	al_dbg(" Auth Offset %d EOP valid %d EOP %d Len %d\n",
		xaction->auth_in_off, xaction->auth_in_off_eop_valid, xaction->auth_in_off_eop,
		xaction->auth_in_len);
	al_dbg(" Auth Byte Count %d\n",
		xaction->auth_bcnt);

	al_dbg("-Cmprs fl_valid %d\n", xaction->cmprs_fl_valid);
	al_dbg(" Cmprs his win in size: %d, addr 0x%016llx\n",
		xaction->cmprs_his_win_in.len,
		(unsigned long long)xaction->cmprs_his_win_in.addr);
	al_dbg(" Cmprs dynamic deflate dec tbl size: %d, addr 0x%016llx\n",
		xaction->cmprs_dynamic_deflate_dec_tbl.len,
		(unsigned long long)xaction->cmprs_dynamic_deflate_dec_tbl.addr);

	al_dbg(" Protection info metadata in size: %d, addr 0x%016llx\n",
		xaction->protection_info_in.len,
		(unsigned long long)xaction->protection_info_in.addr);
}

#else
#define al_print_crypto_desc(x)
#define al_print_crypto_xaction(x)
#endif

/**
 * Memcpy to HW SA
 *
 * @param dst destination buffer
 * @param src source buffer
 * @param size size in words
 */
static
void al_crypto_sa_copy(uint32_t *dst, uint8_t *src, uint32_t size)
{
	uint32_t i;
	uint8_t *cdst = (uint8_t *)dst;
	for (i = 0; i < size*4; i++)
		cdst[i] = src[i];
}

/**
 * Get number of rx submission descriptors needed for crypto transaction
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
uint32_t al_crypto_xaction_rx_descs_count(struct al_crypto_transaction *xaction)
{
	uint32_t count = xaction->dst.num + (xaction->sa_out.len ? 1 : 0) +
	       (xaction->enc_iv_out.len ? 1 : 0) +
	       ((xaction->enc_next_iv_out.len ||
	       xaction->auth_iv_out.len) ? 1 : 0) +
	       (xaction->auth_sign_out.len ? 1 : 0);

	return count;
}

/**
 * Get number of tx submission descriptors needed for crypto transaction
 *
 * we need tx descriptor for each source buffer.
 *
 * @param xaction transaction context
 *
 * @return number of tx submission descriptors
 */
static INLINE
uint32_t al_crypto_xaction_tx_descs_count(struct al_crypto_transaction *xaction)
{
	uint32_t count = xaction->src.num + (xaction->sa_in.len ? 1 : 0) +
		(xaction->enc_iv_in.len ? 1 : 0) +
		(xaction->auth_iv_in.len ? 1 : 0) +
		(xaction->auth_sign_in.len ? 1 : 0) +
		(xaction->protection_info_in.len ? 1 : 0) +
		(xaction->cmprs_his_win_in.len ? 1 : 0) +
		(xaction->cmprs_dynamic_deflate_dec_tbl.len ? 1 : 0);

	/* valid tx descs count */
	al_assert(count);
	/* Need one for metadata if offsets are valid */
	count += (xaction->enc_in_off_eop_valid || xaction->enc_in_len ||
		xaction->auth_in_off_eop_valid || xaction->auth_in_len) ? 1 : 0;

	return count;
}

/**
 * Fill the crypto rx submission descriptors
 *
 * this function writes the contents of the rx submission descriptors
 *
 * @param rx_udma_q rx udma handle
 * @param xaction transaction context
 * @param rx_desc_cnt number of total rx descriptors
 */
static
void al_crypto_set_rx_descs(struct al_udma_q *rx_udma_q,
		struct al_crypto_transaction *xaction, uint32_t rx_desc_cnt)
{
	al_phys_addr_t dst_tgtid;
	union al_udma_desc *rx_desc;
	uint32_t flags;
	uint32_t buf_idx;

	dst_tgtid = ((al_phys_addr_t)xaction->dst.tgtid) << AL_UDMA_DESC_TGTID_SHIFT;

	/* Set descriptor flags */
	flags = (xaction->flags & AL_SSM_INTERRUPT) ?
		AL_S2M_DESC_INT_EN : 0;
	flags |= (xaction->flags & AL_SSM_DEST_NO_SNOOP) ?
		AL_S2M_DESC_NO_SNOOP_H : 0;

	/* if the xaction doesn't have destination buffers,
	 * allocate single Meta descriptor,
	 */
	if (unlikely(!rx_desc_cnt)) {
		al_debug("Crypto: Preparing Meta Rx dec\n");
		rx_desc = al_udma_desc_get(rx_udma_q);
		flags |= al_udma_ring_id_get(rx_udma_q)
			<< AL_S2M_DESC_RING_ID_SHIFT;
		flags |= RX_DESC_META;
		/* write back flags */
		rx_desc->rx.len_ctrl = swap32_to_le(flags);
		al_print_crypto_desc(rx_desc);
		return;
	}

	/* prepare descriptors for the required feilds */
	if (unlikely(xaction->sa_out.len)) {
		al_debug("Crypto: Preparing SA out Rx desc\n");
		rx_desc = al_crypto_prep_one_rx_desc(rx_udma_q, flags,
						     ADDR_TGTID(xaction->sa_out.addr, dst_tgtid),
						     xaction->sa_out.len);
		al_print_crypto_desc(rx_desc);
	}

	if (unlikely(xaction->enc_iv_out.len)) {
		al_debug("Crypto: Preparing ENC IV out Rx desc\n");
		rx_desc = al_crypto_prep_one_rx_desc(rx_udma_q, flags,
				ADDR_TGTID(xaction->enc_iv_out.addr, dst_tgtid),
				xaction->enc_iv_out.len);
		al_print_crypto_desc(rx_desc);
	}

	if (xaction->dst.num) {
		struct al_buf *buf = xaction->dst.bufs;
		al_debug("Crypto: Preparing %d Crypto DST Rx desc\n",
				xaction->dst.num);
		for (buf_idx = 0; buf_idx < xaction->dst.num; buf_idx++) {
			rx_desc = al_crypto_prep_one_rx_desc(rx_udma_q, flags,
							     ADDR_TGTID(buf->addr, dst_tgtid),
							     buf->len);
			al_print_crypto_desc(rx_desc);
			buf++;
		}
	}

	/*
	 * IV output:Encryption IV next to use or In case of auth only SA and
	 * auth_last isnt set, this is the intermidiate auto output.
	 */
	if (xaction->enc_next_iv_out.len) {
		al_debug("Crypto: Preparing ENC Next IV OUT Rx desc\n");
		rx_desc = al_crypto_prep_one_rx_desc(rx_udma_q, flags,
				ADDR_TGTID(xaction->enc_next_iv_out.addr, dst_tgtid),
				xaction->enc_next_iv_out.len);
		al_print_crypto_desc(rx_desc);
	} else {
		if (xaction->auth_iv_out.len) {
			al_debug("Crypto: Preparing AUTH IV OUT Rx desc\n");
			rx_desc = al_crypto_prep_one_rx_desc(rx_udma_q, flags,
					ADDR_TGTID(xaction->auth_iv_out.addr, dst_tgtid),
					xaction->auth_iv_out.len);
			al_print_crypto_desc(rx_desc);
		}
	}

	if (xaction->auth_sign_out.len) {
		al_debug("Crypto: Preparing SIGN out Rx desc\n");
		rx_desc = al_crypto_prep_one_rx_desc(rx_udma_q, flags,
				ADDR_TGTID(xaction->auth_sign_out.addr, dst_tgtid),
				xaction->auth_sign_out.len);
		al_print_crypto_desc(rx_desc);
	}
}

static INLINE
void al_crypto_prep_tx_begin_xaction(struct al_crypto_transaction *xaction,
				     uint32_t *flags_ptr, uint32_t *word1_meta_ptr)
{
	uint32_t flags;
	uint32_t word1_meta;

	al_crypto_prep_tx_begin(xaction->dir, xaction->sa_indx, &flags, &word1_meta);

	/* Set flags */
	flags |= unlikely(xaction->flags & AL_SSM_SRC_NO_SNOOP) ?
		AL_M2S_DESC_NO_SNOOP_H : 0;

	/* Set first desc word1 metadata */
	word1_meta |= unlikely(xaction->sa_out.len) ?
			TX_DESC_META_CRYPT_S_GET_SA : 0;
	word1_meta |= unlikely(xaction->enc_iv_out.len) ?
			TX_DESC_META_CRYPT_S_GET_ENCIV : 0;

	word1_meta |= unlikely(xaction->dst.num) ?
		TX_DESC_META_CRYPT_GET_ORIG : 0;

	word1_meta |=
		unlikely(xaction->enc_next_iv_out.len ||
			 xaction->auth_iv_out.len) ?
			TX_DESC_META_CRYPT_GET_AUTHIV : 0;

	word1_meta |= likely(xaction->auth_sign_out.len) ?
		TX_DESC_META_CRYPT_S_GET_SIGN : 0;

	word1_meta |= unlikely(xaction->protection_info_in.len) ?
		TX_DESC_META_SEND_PI : 0;

	if (unlikely(xaction->auth_fl_valid)) {
		word1_meta |= xaction->auth_first ? TX_DESC_META_AUTH_FIRST : 0;
		word1_meta |= xaction->auth_last ? TX_DESC_META_AUTH_LAST : 0;
	} else if (unlikely(xaction->cmprs_fl_valid)) {
		word1_meta |= xaction->cmprs_his_win_rst ? TX_DESC_META_CMPRS_HIS_WIN_RST : 0;
		word1_meta |= xaction->cmprs_his_win_in.len ? TX_DESC_META_CMPRS_HIS_WIN_SND : 0;
		word1_meta |= xaction->cmprs_dynamic_deflate_dec_tbl.len ?
				TX_DESC_META_CMPRS_DYN_DEFLATE_DEC : 0;
	} else {
		word1_meta |= TX_DESC_META_AUTH_FIRST | TX_DESC_META_AUTH_LAST;
	}

	word1_meta |= unlikely(xaction->auth_sign_in.len) ?
		TX_DESC_META_AUTH_VALID : 0;

	*flags_ptr = flags;
	*word1_meta_ptr = word1_meta;
}

/**
 * Fill the crypto tx submission descriptors
 *
 * this function writes the contents of the tx submission descriptors
 *
 * @param tx_udma_q tx udma handle
 * @param xaction transaction context
 * @param tx_desc_cnt number of total tx descriptors
 */
static
void al_crypto_set_tx_descs(struct al_udma_q *tx_udma_q,
		struct al_crypto_transaction *xaction, uint32_t tx_desc_cnt)
{
	uint32_t flags;
	uint32_t word1_meta;
	al_phys_addr_t src_tgtid;
	union al_udma_desc *tx_desc;
	uint32_t desc_cnt = tx_desc_cnt;

	al_crypto_prep_tx_begin_xaction(xaction, &flags, &word1_meta);
	src_tgtid = ((al_phys_addr_t)xaction->src.tgtid) << AL_UDMA_DESC_TGTID_SHIFT;

	/* First Meta data desc */
	if (xaction->enc_in_off_eop_valid || xaction->enc_in_len ||
		xaction->auth_in_off_eop_valid || xaction->auth_in_len ||
		xaction->cmprs_enc_padding_valid) {
		uint32_t flags_len = flags;
		uint32_t ring_id;
		uint32_t enc_meta;
		uint32_t auth_meta;
		uint32_t enc_in_off_eop;
		uint32_t auth_in_off_eop;

		al_dbg("Crypto: preparing metadata desc:\n");
		al_dbg("	Enc Offset %d EOP valid %d EOP %d Len %d\n",
			xaction->enc_in_off, xaction->enc_in_off_eop_valid,
			xaction->enc_in_off_eop, xaction->enc_in_len);
		al_dbg("	Auth Offset %d EOP valid %d EOP %d Len %d\n",
			xaction->auth_in_off, xaction->auth_in_off_eop_valid,
			xaction->auth_in_off_eop, xaction->auth_in_len);
		al_dbg("	Cmprs+Enc padding resolution %d Auto padding %d\n",
			xaction->cmprs_enc_padding_res, xaction->cmprs_enc_auto_padding);
		/* having only metdata desc isnt valid */
		desc_cnt--;
		/* Valid desc count */
		al_assert(desc_cnt);

		tx_desc = al_udma_desc_get(tx_udma_q);
		/* UDMA feilds */
		ring_id = al_udma_ring_id_get(tx_udma_q)
			<< AL_M2S_DESC_RING_ID_SHIFT;
		flags_len |= ring_id;
		flags_len |= AL_M2S_DESC_META_DATA;
		tx_desc->tx_meta.len_ctrl = swap32_to_le(flags_len);
		/* Word1 metadata */
		tx_desc->tx_meta.meta_ctrl = 0;
		if (xaction->cmprs_enc_padding_valid) {
			tx_desc->tx_meta.meta_ctrl |= (xaction->cmprs_enc_padding_res
			<< TX_DESC_META_CMPRS_ENC_PADDING_RESOLUTION_SHIFT)
				& TX_DESC_META_CMPRS_ENC_PADDING_RESOLUTION_MASK;
			tx_desc->tx_meta.meta_ctrl |= xaction->cmprs_enc_auto_padding ?
				TX_DESC_META_CMPRS_ENC_AUTO_PADDING : 0;
		}
		if (xaction->auth_bcnt) {
			/* Auth only, prev auth byte count */
			tx_desc->tx_meta.meta1 =
				swap32_to_le(xaction->auth_bcnt);
		} else {
			enc_in_off_eop = xaction->enc_in_off_eop_valid ?
				xaction->enc_in_off_eop :
				(xaction->src_size - (xaction->enc_in_len + xaction->enc_in_off));
			/* Encryption offsets */
			enc_meta = enc_in_off_eop & TX_DESC_META_ENC_OFF_EOP_MASK;
			enc_meta |= (xaction->enc_in_off << TX_DESC_META_ENC_OFF_SHIFT)
				& TX_DESC_META_ENC_OFF_MASK;

			tx_desc->tx_meta.meta1 = swap32_to_le(enc_meta);
		}
		auth_in_off_eop = xaction->auth_in_off_eop_valid ?
			xaction->auth_in_off_eop :
			(xaction->src_size - (xaction->auth_in_len + xaction->auth_in_off));
		/* Authentication offsets */
		auth_meta = auth_in_off_eop & TX_DESC_META_AUTH_OFF_EOP_MASK;
		auth_meta |= (xaction->auth_in_off << TX_DESC_META_AUTH_OFF_SHIFT)
			& TX_DESC_META_AUTH_OFF_MASK;
		tx_desc->tx_meta.meta2 = swap32_to_le(auth_meta);
		al_print_crypto_desc(tx_desc);
		/* clear first flag, keep no snoop hint flag */
		flags &= AL_M2S_DESC_NO_SNOOP_H;
	}

	flags |= unlikely(xaction->flags & AL_SSM_BARRIER) ?
		AL_M2S_DESC_DMB : 0;

	/* prepare descriptors for the SA_in if found */
	if (xaction->sa_in.len) {
		al_debug("Crypto: Preparing SA Tx desc sa_index %d\n",
				xaction->sa_indx);
		tx_desc = al_crypto_prep_tx_sa_update(tx_udma_q, flags, word1_meta,
						      ADDR_TGTID(xaction->sa_in.addr, src_tgtid),
						      xaction->sa_in.len);
		desc_cnt--;
		if (unlikely(desc_cnt == 0))
			al_crypto_prep_tx_end(tx_desc);
		al_print_crypto_desc(tx_desc);
		al_crypto_prep_tx_next(&flags, &word1_meta);
	}

	/* prepare descriptors for the enc_IV_in if found */
	if (likely(xaction->enc_iv_in.len)) {
		al_debug("Crypto: Preparing IV in Tx desc\n");
		tx_desc = al_crypto_prep_tx_enc_iv(tx_udma_q, flags, word1_meta,
						   ADDR_TGTID(xaction->enc_iv_in.addr, src_tgtid),
						   xaction->enc_iv_in.len);
		desc_cnt--;
		if (unlikely(desc_cnt == 0))
			al_crypto_prep_tx_end(tx_desc);
		al_print_crypto_desc(tx_desc);
		al_crypto_prep_tx_next(&flags, &word1_meta);
	}

	/* prepare descriptors for the auth_IV_in if found */
	if (unlikely(xaction->auth_iv_in.len)) {
		al_debug("Crypto: Preparing Auth IV in Tx desc\n");
		tx_desc = al_crypto_prep_tx_auth_iv(tx_udma_q, flags, word1_meta,
						    ADDR_TGTID(xaction->auth_iv_in.addr, src_tgtid),
						    xaction->auth_iv_in.len);
		desc_cnt--;
		if (unlikely(desc_cnt == 0))
			al_crypto_prep_tx_end(tx_desc);
		al_print_crypto_desc(tx_desc);
		al_crypto_prep_tx_next(&flags, &word1_meta);
	}

	if (unlikely(xaction->protection_info_in.len)) {
		al_debug("Crypto: Preparing protection-info in Tx desc\n");
		tx_desc = al_crypto_prep_tx_protection_info(tx_udma_q, flags, word1_meta,
				ADDR_TGTID(xaction->protection_info_in.addr, src_tgtid),
				xaction->protection_info_in.len);
		desc_cnt--;
		if (unlikely(desc_cnt == 0))
			al_crypto_prep_tx_end(tx_desc);
		al_print_crypto_desc(tx_desc);
		al_crypto_prep_tx_next(&flags, &word1_meta);
	}

	/* prepare descriptors for the cmprs_his_win if found */
	if (unlikely(xaction->cmprs_his_win_in.len)) {
		al_debug("Crypto: Preparing compression history window in Tx desc\n");
		tx_desc = al_crypto_prep_tx_cmprs_his_win(tx_udma_q, flags, word1_meta,
				ADDR_TGTID(xaction->cmprs_his_win_in.addr, src_tgtid),
				xaction->cmprs_his_win_in.len);
		desc_cnt--;
		if (unlikely(desc_cnt == 0))
			al_crypto_prep_tx_end(tx_desc);
		al_print_crypto_desc(tx_desc);
		al_crypto_prep_tx_next(&flags, &word1_meta);
	}

	/* prepare descriptors for the cmprs_dyn_deflate_dec_tbl if found */
	if (unlikely(xaction->cmprs_dynamic_deflate_dec_tbl.len)) {
		al_debug("Crypto: Preparing compression deflate tree table in Tx desc\n");
		tx_desc = al_crypto_prep_tx_cmprs_hfmn_tree_tbl(tx_udma_q, flags, word1_meta,
				ADDR_TGTID(xaction->cmprs_dynamic_deflate_dec_tbl.addr, src_tgtid),
				xaction->cmprs_dynamic_deflate_dec_tbl.len);
		desc_cnt--;
		if (unlikely(desc_cnt == 0))
			al_crypto_prep_tx_end(tx_desc);
		al_print_crypto_desc(tx_desc);
		al_crypto_prep_tx_next(&flags, &word1_meta);
	}

	/* prepare descriptors for the source buffer if found */
	if (likely(xaction->src.num)) {
		struct al_buf *buf = xaction->src.bufs;
		uint32_t buf_idx;

		al_debug("Crypto: Preparing SRC %d Tx desc\n",
				xaction->src.num);
		tx_desc = al_crypto_prep_tx_src(tx_udma_q, flags, word1_meta,
						ADDR_TGTID(buf->addr, src_tgtid), buf->len);
		desc_cnt--;
		if (unlikely(desc_cnt == 0))
			al_crypto_prep_tx_end(tx_desc);
		al_print_crypto_desc(tx_desc);
		al_crypto_prep_tx_next(&flags, &word1_meta);
		buf++;

		for (buf_idx = 1; buf_idx < xaction->src.num; buf_idx++) {
			tx_desc = al_crypto_prep_tx_concat(tx_udma_q, flags, word1_meta,
							   ADDR_TGTID(buf->addr, src_tgtid),
							   buf->len);
			desc_cnt--;
			if (unlikely(desc_cnt == 0))
				al_crypto_prep_tx_end(tx_desc);
			al_print_crypto_desc(tx_desc);
			buf++;
			/* do not reset flags and meta as they haven't changed */
		}
	}

	/* prepare descriptors for the auth signature if found */
	if (unlikely(xaction->auth_sign_in.len)) {
		al_debug("Crypto: Preparing Signature in Tx desc\n");
		tx_desc = al_crypto_prep_tx_auth_sign(tx_udma_q, flags, word1_meta,
				ADDR_TGTID(xaction->auth_sign_in.addr, src_tgtid),
				xaction->auth_sign_in.len);
		desc_cnt--;
		if (unlikely(desc_cnt == 0))
			al_crypto_prep_tx_end(tx_desc);
		al_print_crypto_desc(tx_desc);
		al_crypto_prep_tx_next(&flags, &word1_meta);
	}

	al_assert(!desc_cnt);
}

/**
 * Initialize a single hw_sa
 *
 * @param sa crypto SW SA containing the desired SA parameters
 * @param hw_sa crypto HW SA filled with zeros
 *           to be initialized according to the sa
 * @param copy_only_ivs true -> copy only iv data, false -> copy everything
 */
static INLINE
void al_crypto_hw_sa_init_entry(struct al_crypto_sa *sa,
	struct al_crypto_hw_sa *hw_sa, al_bool copy_only_ivs)
{
	uint32_t tword;
	al_bool is_auth_type_sha3 = AL_CRYPT_OP_HAS(sa->sa_op, AL_CRYPT_AUTH) &&
				((sa->auth_type == AL_CRYPT_AUTH_SHA3) ||
				(sa->auth_type == AL_CRYPT_AUTH_SHA3_CONT));
	al_bool is_quad_sa_hmac = is_auth_type_sha3 && sa->auth_hmac_en;

	/* Words [4-63] */
	/* AES XTS */
	if (sa->enc_type == AL_CRYPT_AES_XTS) {
		al_crypto_sa_copy(
			&hw_sa->sa_word[CRYPT_SAD_XTS_TWEAK_KEY_SWORD],
			sa->enc_xts_tweak_key, CRYPT_SAD_XTS_TWEAK_KEY_SIZE);
		tword = 0;
		sa_init_field(tword,
			sa->enc_xts_sector_size,
			CRYPT_SAD_XTS_SEC_SIZE_MASK,
			CRYPT_SAD_XTS_SEC_SIZE_SHIFT,
			"valid xts sector size");
		sa_init_field(tword,
			sa->enc_xts_protection,
			CRYPT_SAD_XTS_PROTECTION_MASK,
			CRYPT_SAD_XTS_PROTECTION_SHIFT,
			"valid xts protection");
		hw_sa->sa_word[CRYPT_SAD_XTS_SEC_SIZE_WORD] = tword;
	}

	/* Encryption Key and IV, also relevant for GCM Auth */
	if (AL_CRYPT_OP_HAS(sa->sa_op, AL_CRYPT_ENC) || (AL_CRYPT_OP_HAS(sa->sa_op, AL_CRYPT_AUTH)
			&& (sa->auth_type == AL_CRYPT_AUTH_AES_GCM))) {
		al_crypto_sa_copy(&hw_sa->sa_word[CRYPT_SAD_ENC_KEY_SWORD],
			sa->enc_key, CRYPT_SAD_ENC_KEY_SIZE);
		al_crypto_sa_copy(&hw_sa->sa_word[CRYPT_SAD_ENC_IV_SWORD],
			sa->enc_iv, CRYPT_SAD_ENC_IV_SIZE);
	}

	/* AES GCM IV */
	if (sa->enc_type == AL_CRYPT_AES_GCM) {
		al_crypto_sa_copy(&hw_sa->sa_word[CRYPT_SAD_GCM_AUTH_IV_SWORD],
			sa->aes_gcm_auth_iv, CRYPT_SAD_GCM_AUTH_IV_SIZE);
	}

	/* Authentication */
	if (AL_CRYPT_OP_HAS(sa->sa_op, AL_CRYPT_AUTH)) {
		if (sa->auth_hmac_en) {
			al_crypto_sa_copy(
				&hw_sa->sa_word[CRYPT_SAD_HMAC_IV_IN_SWORD],
				sa->hmac_iv_in, CRYPT_SAD_HMAC_IV_IN_SIZE);
			al_crypto_sa_copy(
				&hw_sa->sa_word[CRYPT_SAD_HMAC_IV_OUT_SWORD],
				sa->hmac_iv_out, CRYPT_SAD_HMAC_IV_OUT_SIZE);
		} else {
			switch (sa->auth_type) {
			case AL_CRYPT_AUTH_GMAC:
				al_crypto_sa_copy(
					&hw_sa->sa_word[CRYPT_SAD_GMAC_IV_IN_SWORD],
					sa->gmac_iv_in, CRYPT_SAD_GMAC_IV_IN_SIZE);
				al_crypto_sa_copy(
					&hw_sa->sa_word[CRYPT_SAD_GMAC_H_IN_SWORD],
					sa->gmac_h_in, CRYPT_SAD_GMAC_H_IN_SIZE);
				al_crypto_sa_copy(
					&hw_sa->sa_word[CRYPT_SAD_GMAC_AES_KEY_IN_SWORD],
					sa->gmac_aes_key_in, CRYPT_SAD_GMAC_AES_KEY_IN_SIZE);
				break;
			case AL_CRYPT_AUTH_CRC8:
			case AL_CRYPT_AUTH_CRC16:
			case AL_CRYPT_AUTH_CRC32:
			case AL_CRYPT_AUTH_CSUM8:
			case AL_CRYPT_AUTH_CSUM16:
			case AL_CRYPT_AUTH_CSUM32:
				al_crypto_sa_copy(
					&hw_sa->sa_word[CRYPT_SAD_ICRC_IV_IN_SWORD],
					sa->icrc_iv_in, CRYPT_SAD_ICRC_IV_IN_SIZE);
				break;
			default:
				break;
			}
		}
	}
	/* IV for broken Auth, overlap GCM feilds
	   which dont support broken Auth */
	if (AL_CRYPT_OP_IS(sa->sa_op, AL_CRYPT_AUTH) &&
				(sa->auth_type != AL_CRYPT_AUTH_AES_GCM)) {
		al_crypto_sa_copy(&hw_sa->sa_word[CRYPT_SAD_AUTH_IV_SWORD],
				sa->auth_iv_in, CRYPT_SAD_AUTH_IV_SIZE);
	}

	if(copy_only_ivs)
		return;

	/* Word 0 */
	tword = 0;
	/* Valid SA operation */
	al_assert(sa->sa_op);
	sa_init_field(tword,
		((AL_CRYPT_OP_HAS(sa->sa_op, AL_CRYPT_CMPRS) ? CRYPT_SAD_OP_CMPRS : 0) |
		(AL_CRYPT_OP_HAS(sa->sa_op, AL_CRYPT_ENC) ? CRYPT_SAD_OP_ENC : 0) |
		(AL_CRYPT_OP_HAS(sa->sa_op, AL_CRYPT_AUTH) ? CRYPT_SAD_OP_AUTH : 0)),
		CRYPT_SAD_OP_MASK, CRYPT_SAD_OP_SHIFT, "valid sa_op");
	sa_init_field(tword, sa->sa_length, CRYPT_SAD_SA_LENGTH_MASK,
			CRYPT_SAD_SA_LENGTH_SHIFT, "valid sa_length");

	/* Make sure SA length is not single line only in SHA3 mode */
	al_assert((sa->sa_length == AL_CRYPT_SINGLE_LINE) || is_auth_type_sha3);
	/* Make sure SA length is not single line in quad SA HMAC mode */
	al_assert((!is_quad_sa_hmac) || (sa->sa_length != AL_CRYPT_SINGLE_LINE));

	/* Encryption */
	if (AL_CRYPT_OP_HAS(sa->sa_op, AL_CRYPT_ENC) ||
			(AL_CRYPT_OP_HAS(sa->sa_op, AL_CRYPT_AUTH) &&
			((sa->auth_type == AL_CRYPT_AUTH_AES_GCM) ||
			(sa->auth_type == AL_CRYPT_AUTH_AES_CCM)))) {
		sa_init_field(tword, sa->enc_type, CRYPT_SAD_ENC_TYPE_MASK,
			CRYPT_SAD_ENC_TYPE_SHIFT, "valid enc type");
		if ((sa->enc_type == AL_CRYPT_TRIPDES_ECB) ||
				(sa->enc_type == AL_CRYPT_TRIPDES_CBC)) {
			sa_init_field(tword,
				sa->tripdes_m,
				CRYPT_SAD_TRIPDES_MODE_MASK,
				CRYPT_SAD_TRIPDES_MODE_SHIFT,
				"valid 3des mode");
		}
		if (sa->enc_type > AL_CRYPT_TRIPDES_CBC) {
			sa_init_field(tword,
				sa->aes_ksize,
				CRYPT_SAD_AES_KEY_SIZE_MASK,
				CRYPT_SAD_AES_KEY_SIZE_SHIFT,
				"valid aes key size");
		}
		if ((sa->enc_type == AL_CRYPT_AES_CBC) ||
		    (sa->enc_type == AL_CRYPT_AES_ECB) ||
		    (sa->enc_type == AL_CRYPT_AES_XTS)) {
			sa_init_field(tword,
				sa->cts_mode,
				CRYPT_SAD_CTS_MODE_MASK,
				CRYPT_SAD_CTS_MODE_SHIFT,
				"valid ciphertext stealing mode");
		} else {
			sa_init_field(tword,
				sa->cntr_size,
				CRYPT_SAD_CNTR_SIZE_MASK,
				CRYPT_SAD_CNTR_SIZE_SHIFT,
				"valid counter loop");
		}
	}

	/* Authentication */
	if (AL_CRYPT_OP_HAS(sa->sa_op, AL_CRYPT_AUTH)) {
		sa_init_field(tword,
			sa->auth_type,
			CRYPT_SAD_AUTH_TYPE_MASK,
			CRYPT_SAD_AUTH_TYPE_SHIFT,
			"valid auth type");
		sa_init_field(tword,
			sa->signature_size,
			CRYPT_SAD_SIGN_SIZE_MASK,
			CRYPT_SAD_SIGN_SIZE_SHIFT,
			"valid sign size");
		if (sa->auth_type == AL_CRYPT_AUTH_SHA2)
			sa_init_field(tword,
				sa->sha2_mode,
				CRYPT_SAD_SHA2_MODE_MASK,
				CRYPT_SAD_SHA2_MODE_SHIFT,
				"valid sha2 mode");
		if (sa->auth_type == AL_CRYPT_AUTH_SHA3)
			sa_init_field(tword,
				sa->sha3_mode,
				CRYPT_SAD_SHA3_MODE_MASK,
				CRYPT_SAD_SHA3_MODE_SHIFT,
				"valid sha3 mode");
		if (sa->auth_type == AL_CRYPT_AUTH_SHA3_CONT)
			sa_init_field(tword,
				sa->sha3_mode_ext,
				CRYPT_SAD_SHA3_MODE_EXT_MASK,
				CRYPT_SAD_SHA3_MODE_EXT_SHIFT,
				"valid sha3 mode");
		if (sa->auth_type == AL_CRYPT_AUTH_CRC8)
			sa_init_field(tword,
				sa->crc8_mode,
				CRYPT_SAD_CRC8_MODE_MASK,
				CRYPT_SAD_CRC8_MODE_SHIFT,
				"valid crc8 mode");
		if (sa->auth_type == AL_CRYPT_AUTH_CRC16)
			sa_init_field(tword,
				sa->crc16_mode,
				CRYPT_SAD_CRC16_MODE_MASK,
				CRYPT_SAD_CRC16_MODE_SHIFT,
				"valid crc16 mode");
		if (sa->auth_type == AL_CRYPT_AUTH_CRC32)
			sa_init_field(tword,
				sa->crc32_mode,
				CRYPT_SAD_CRC32_MODE_MASK,
				CRYPT_SAD_CRC32_MODE_SHIFT,
				"valid crc32 mode");
		if (sa->auth_type == AL_CRYPT_AUTH_CSUM32)
			sa_init_field(tword,
				sa->csum32_mode,
				CRYPT_SAD_CSUM32_MODE_MASK,
				CRYPT_SAD_CSUM32_MODE_SHIFT,
				"valid csum32 mode");
		if (sa->auth_type == AL_CRYPT_AUTH_GMAC) {
			sa_init_field(tword,
			      sa->aes_ksize,
			      CRYPT_SAD_AES_KEY_SIZE_MASK,
			      CRYPT_SAD_AES_KEY_SIZE_SHIFT,
			       "valid aes key size");
		}
		tword |= sa->auth_signature_msb ? CRYPT_SAD_AUTH_MSB_BITS : 0;
		tword |= sa->auth_hmac_en ? CRYPT_SAD_HMAC_EN : 0;
	}

	/* Encryption/Compression + Authentication */
	if ((AL_CRYPT_OP_HAS(sa->sa_op, AL_CRYPT_ENC) ||
				AL_CRYPT_OP_HAS(sa->sa_op, AL_CRYPT_CMPRS)) &&
				AL_CRYPT_OP_HAS(sa->sa_op, AL_CRYPT_AUTH)) {
		tword |= sa->sign_after_enc ? CRYPT_SAD_SIGN_AFTER_ENC : 0;
		tword |= sa->auth_after_dec ? CRYPT_SAD_AUTH_AFTER_DEC : 0;
	}

	hw_sa->sa_word[0] = swap32_to_le(tword);

	/* Word 1 */
	tword = 0;

	/* Authentication */
	if (AL_CRYPT_OP_HAS(sa->sa_op, AL_CRYPT_AUTH)) {
		if ((sa->auth_type == AL_CRYPT_AUTH_CRC8) ||
			(sa->auth_type == AL_CRYPT_AUTH_CRC16) ||
			(sa->auth_type == AL_CRYPT_AUTH_CRC32) ||
			(sa->auth_type == AL_CRYPT_AUTH_CSUM8) ||
			(sa->auth_type == AL_CRYPT_AUTH_CSUM16) ||
			(sa->auth_type == AL_CRYPT_AUTH_CSUM32)) {
			sa_init_field(tword,
				sa->icrc_prof_id,
				CRYPT_SAD_ICRC_PROF_ID_MASK,
				CRYPT_SAD_ICRC_PROF_ID_SHIFT,
				"icrc prof id");
		}

		if (sa->auth_type == AL_CRYPT_AUTH_GMAC) {
			sa_init_field(tword,
				sa->gmac_mask_id,
				CRYPT_SAD_GMAC_MASK_ID_MASK,
				CRYPT_SAD_GMAC_MASK_ID_SHIFT,
				"gmac mask id");
		}
	}

	/* Compression profile index */
	if (AL_CRYPT_OP_HAS(sa->sa_op, AL_CRYPT_CMPRS)) {
		sa_init_field(tword,
			sa->cmprs_lz_prof_idx,
			CRYPT_SAD_CMPRS_LZ_PROF_IDX_MASK,
			CRYPT_SAD_CMPRS_LZ_PROF_IDX_SHIFT,
			"compression lz profile index");
		sa_init_field(tword,
			sa->cmprs_dynamic_deflate_table_idx,
			CRYPT_SAD_CMPRS_DYNAMIC_DEFLATE_TABLE_IDX_MASK,
			CRYPT_SAD_CMPRS_DYNAMIC_DEFLATE_TABLE_IDX_SHIFT,
			"compression dynamic deflate dev tbl id");
		sa_init_field(tword,
			sa->cmprs_forced_compressed_out,
			CRYPT_SAD_CMPRS_FORCE_COMPRESSED_OUT_MASK,
			CRYPT_SAD_CMPRS_FORCE_COMPRESSED_OUT_SHIFT,
			"compression skip output controller arbitration");
	}

	/* Compressed/Encrypted CRC/Signature */
	/* encrypted signature bit can only be asserted if doing
	   authentication with compression or encryption or both. */
	if (sa->encrypted_signature) {
		al_assert(AL_CRYPT_OP_HAS(sa->sa_op, AL_CRYPT_AUTH)  &&
				  (AL_CRYPT_OP_HAS(sa->sa_op, AL_CRYPT_ENC) ||
				   AL_CRYPT_OP_HAS(sa->sa_op, AL_CRYPT_CMPRS)));
		al_assert(!sa->sign_after_enc && sa->auth_after_dec);
		al_debug(" SA compressed/encrypted crc/signature - %x\n", sa->encrypted_signature);
		tword |= CRYPT_SAD_ENCYPRTED_SIGNATURE;
		sa_init_field(tword,
					  sa->pi_size,
					  CRYPT_SAD_PI_SIZE_MASK,
					  CRYPT_SAD_PI_SIZE_SHIFT,
					  "pi md size");
		sa_init_field(tword,
					  sa->pi_crc_offset,
					  CRYPT_SAD_PI_CRC_OFFSET_MASK,
					  CRYPT_SAD_PI_CRC_OFFSET_SHIFT,
					  "pi crc offset");
	}

	hw_sa->sa_word[1] = swap32_to_le(tword);

	/* Word 2 - Encryption offsets */
	tword = 0;
	if (AL_CRYPT_OP_HAS(sa->sa_op, AL_CRYPT_ENC) ||
				AL_CRYPT_OP_HAS(sa->sa_op, AL_CRYPT_CMPRS)) {
		sa_init_field(tword,
			sa->enc_offset,
			CRYPT_SAD_ENC_OFF_MASK,
			CRYPT_SAD_ENC_OFF_SHIFT,
			"valid enc off");
		sa_init_field(tword,
			sa->enc_offset_eop,
			CRYPT_SAD_ENC_OFF_EOP_MASK,
			CRYPT_SAD_ENC_OFF_EOP_SHIFT,
			"valid enc off eop");
		hw_sa->sa_word[2] = swap32_to_le(tword);
	}

	/* Word 3 - Authentication offsets */
	tword = 0;
	if (AL_CRYPT_OP_HAS(sa->sa_op, AL_CRYPT_AUTH)) {
		sa_init_field(tword,
			sa->auth_offset,
			CRYPT_SAD_AUTH_OFF_MASK,
			CRYPT_SAD_AUTH_OFF_SHIFT,
			"valid auth off");
		sa_init_field(tword,
			sa->auth_offset_eop,
			CRYPT_SAD_AUTH_OFF_EOP_MASK,
			CRYPT_SAD_AUTH_OFF_EOP_SHIFT,
			"valid auth off eop");
		hw_sa->sa_word[3] = swap32_to_le(tword);
	}
}

/****************************** API functions *********************************/
void al_crypto_hw_sa_init(struct al_crypto_sa *sa,
			 struct al_crypto_hw_sa *hw_sa)
{
	uint32_t sa_idx;
	uint32_t sa_num_entries = (1 << sa->sa_length);

	/*
	 * Init the first entry with all data and the rest entries only with ivs
	 */
	for (sa_idx = 0; sa_idx < sa_num_entries; sa_idx++)
    		al_crypto_hw_sa_init_entry(&sa[sa_idx], &hw_sa[sa_idx],
    			sa_idx ? AL_TRUE : AL_FALSE);
}

/******************************************************************************
 *****************************************************************************/
int al_crypto_dma_prepare(struct al_ssm_dma *dma, uint32_t qid,
			   struct al_crypto_transaction *xaction)
{
	uint32_t rx_descs;
	uint32_t tx_descs;
	struct al_udma_q *rx_udma_q;
	struct al_udma_q *tx_udma_q;
	int rc;

	al_debug("al_crypto_dma_prepare\n");
	al_print_crypto_xaction(xaction);

	/* Check some parameters */
	/* SA out -> SA in */
	al_assert(!xaction->sa_out.len || xaction->sa_in.len);
	/* Valid SA index */
	al_assert(!(xaction->sa_indx >> TX_DESC_META_SA_IDX_SHIFT
			& ~TX_DESC_META_SA_IDX_MASK));
	/* No last -> No sign_in */
	al_assert(!(xaction->auth_fl_valid &&
		!xaction->auth_last && xaction->auth_sign_in.len));
	/* Queue is for crypt/auth transactions */
	al_assert(dma->q_types[qid] == AL_CRYPT_AUTH_Q);

	/* calc tx (M2S) descriptors */
	tx_descs = al_crypto_xaction_tx_descs_count(xaction);
	/*valid tx descs count*/
	al_assert(tx_descs <= dma->ssm_max_src_descs);

	rc = al_udma_q_handle_get(&dma->m2m_udma.tx_udma, qid, &tx_udma_q);
	/* valid crypto tx q handle */
	al_assert(!rc);

	if (unlikely(al_udma_available_get(tx_udma_q) < tx_descs
				+ AL_CRYPT_DESC_RES)) {
		uint32_t cdesc_count;

		/* cleanup tx completion queue */
		cdesc_count = al_udma_cdesc_get_all(tx_udma_q, NULL);
		if (likely(cdesc_count != 0))
			al_udma_cdesc_ack(tx_udma_q, cdesc_count);

		/* check again */
		if (unlikely(al_udma_available_get(tx_udma_q) < tx_descs
				+ AL_CRYPT_DESC_RES)) {
			al_dbg("crypt[%s]:tx q has no enough free desc\n",
					dma->m2m_udma.name);
			return -ENOSPC;
		}
	}

	/* calc rx (S2M) descriptors, at least one desc is required */
	rx_descs = al_crypto_xaction_rx_descs_count(xaction);
	/* valid rx descs count */
	al_assert(rx_descs <= dma->ssm_max_dst_descs);

	rc = al_udma_q_handle_get(&dma->m2m_udma.rx_udma, qid, &rx_udma_q);
	/* valid crypto rx q handle */
	al_assert(!rc);
	if (unlikely(al_udma_available_get(rx_udma_q)
		     < (rx_descs ? rx_descs : 1))) {
		al_dbg("crypto [%s]: rx q has no enough free desc\n",
			 dma->m2m_udma.name);
		return -ENOSPC;
	}

	/* prepare rx descs */
	al_crypto_set_rx_descs(rx_udma_q, xaction, rx_descs);
	/* add rx descriptors */
	al_udma_desc_action_add(rx_udma_q, rx_descs ? rx_descs : 1);

	/* prepare tx descriptors */
	al_crypto_set_tx_descs(tx_udma_q, xaction, tx_descs);

	/* set number of tx descriptors */
	xaction->tx_descs_count = tx_descs;

	return 0;
}

/******************************************************************************
 *****************************************************************************/
int al_crypto_dma_action(struct al_ssm_dma *dma, uint32_t qid,
		int tx_descs)
{
	return al_ssm_dma_action(dma, qid, tx_descs);
}

/******************************************************************************
 *****************************************************************************/
int al_crypto_dma_completion(struct al_ssm_dma *dma, uint32_t qid,
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

	al_debug("crypto packet completed. count %d status desc %p meta %x\n",
			cdesc_count, cdesc, cdesc->al_desc_comp_rx.ctrl_meta);

	return 1;
}

/******************************************************************************
 *****************************************************************************/
struct crypto_regs __iomem *al_ssm_crypto_regs_get(void __iomem *app_regs, uint8_t rev_id)
{
	if (rev_id < AL_SSM_REV_ID_REV3)
		return app_regs;
	else
		return (struct crypto_regs __iomem *)((uint8_t __iomem *)app_regs +
			AL_CRYPT_REGS_OFF_IN_APP_REGS_REV3);
}

/******************************************************************************
 *****************************************************************************/
void al_crypto_xts_cfg_set(
	struct al_ssm_dma		*dma,
	void __iomem			*app_regs,
	const struct al_crypto_xts_cfg	*cfg)
{
	static const struct al_crypto_xts_cfg cfg_default = { .swap_pre_inc_bytes = AL_TRUE };
	struct crypto_regs __iomem *regs = al_ssm_crypto_regs_get(app_regs, dma->rev_id);

	al_assert(app_regs);

	if (!cfg)
		cfg = &cfg_default;

	al_reg_write32(&regs->xts_conf.tweak_calc_swap,
		(cfg->swap_pre_inc_bits ? CRYPTO_XTS_CONF_TWEAK_CALC_SWAP_PRE_INC_BITS : 0) |
		(cfg->swap_pre_inc_bytes ? CRYPTO_XTS_CONF_TWEAK_CALC_SWAP_PRE_INC_BYTES : 0) |
		(cfg->swap_pre_inc_words ? CRYPTO_XTS_CONF_TWEAK_CALC_SWAP_PRE_INC_WORDS : 0) |
		(cfg->swap_post_inc_bits ? CRYPTO_XTS_CONF_TWEAK_CALC_SWAP_POST_INC_BITS : 0) |
		(cfg->swap_post_inc_bytes ? CRYPTO_XTS_CONF_TWEAK_CALC_SWAP_POST_INC_BYTES : 0) |
		(cfg->swap_post_inc_words ? CRYPTO_XTS_CONF_TWEAK_CALC_SWAP_POST_INC_WORDS : 0));
}

/******************************************************************************
 *****************************************************************************/
void al_crypto_error_ints_mask_get(unsigned int rev_id,
				   uint32_t *crypto_a_mask,
				   uint32_t *crypto_b_mask,
				   uint32_t *crypto_c_mask,
				   uint32_t *crypto_d_mask)
{
	al_assert(crypto_a_mask);
	al_assert(crypto_b_mask);
	al_assert(crypto_c_mask);
	al_assert(crypto_d_mask);

	if (rev_id <= AL_CRYPTO_REV_ID_2)
		*crypto_a_mask = AL_CRYPTO_IOFIC_GROUP_A_ERROR_INTS;
	else
		*crypto_a_mask = AL_CRYPTO_IOFIC_GROUP_A_ERROR_INTS_V3;

	/**
	 * Addressing RMN: 9991
	 *
	 * RMN description:
	 * registers of group B in the intc are not accessible.
	 * only 0x40 bytes of memory space were allocated for the crypto intc,
	 * which is equivalent for a single interrupt group.
	 * The following 0x40 bytes of memory space , which should have been
	 * allocated to group B of the interrupt controller,
	 * are allocated to other registers of the regfile
	 *
	 * Software flow:
	 * On V1 do not use intc group B
	 */
	if (rev_id <= AL_CRYPTO_REV_ID_1)
		*crypto_b_mask = 0;
	else if (rev_id == AL_CRYPTO_REV_ID_2)
		*crypto_b_mask = AL_CRYPTO_IOFIC_GROUP_B_ERROR_INTS_V2;
	else
		*crypto_b_mask = AL_CRYPTO_IOFIC_GROUP_B_ERROR_INTS_V3;

	if (rev_id <= AL_CRYPTO_REV_ID_2)
		*crypto_c_mask = 0;
	else
		*crypto_c_mask = AL_CRYPTO_IOFIC_GROUP_C_ERROR_INTS;

	*crypto_d_mask = 0;
}

/******************************************************************************
 *****************************************************************************/
void al_crypto_error_ints_unmask(uint8_t rev_id,
				 void __iomem *crypto_regs_base,
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
	void *iofic_regs_base;
	uint32_t ext_app_bit;


	al_udma_handle_init(&udma, &udma_params);

	al_crypto_error_ints_mask_get(rev_id,
				      &a_mask,
				      &b_mask,
				      &c_mask,
				      &d_mask);

	iofic_regs_base = (uint8_t *)crypto_regs_base + AL_CRYPTO_APP_IOFIC_OFFSET;

	al_iofic_config(iofic_regs_base,
			AL_INT_GROUP_A,
			INT_CONTROL_GRP_MASK_MSI_X);

	al_iofic_unmask(iofic_regs_base,
			AL_INT_GROUP_A,
			a_mask);

	if (b_mask) {
		al_iofic_config(iofic_regs_base,
			AL_INT_GROUP_B,
			INT_CONTROL_GRP_MASK_MSI_X);

		al_iofic_unmask(iofic_regs_base,
			AL_INT_GROUP_B,
			b_mask);
	}

	if (c_mask) {
		al_iofic_config(iofic_regs_base,
			AL_INT_GROUP_C,
			INT_CONTROL_GRP_MASK_MSI_X);

		al_iofic_unmask(iofic_regs_base,
			AL_INT_GROUP_C,
			c_mask);
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
