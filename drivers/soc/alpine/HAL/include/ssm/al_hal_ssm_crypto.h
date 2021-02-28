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
 * @defgroup group_crypto_api API
 * Cryptographic Acceleration Engine HAL driver API
 * @ingroup group_crypto
 * @{
 * @file   al_hal_ssm_crypto.h
 */

#ifndef __AL_HAL_CRYPT_H__
#define __AL_HAL_CRYPT_H__

#include "al_hal_common.h"
#include "al_hal_udma.h"
#include "al_hal_m2m_udma.h"
#include "al_hal_ssm.h"

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

#define CRYPTO_DEBUG

#ifdef CRYPTO_DEBUG
#define al_debug al_dbg
#else
#define al_debug(...)
#endif

/* PCI Adapter Device/Revision ID */
#define AL_CRYPTO_ALPINE_V1_DEV_ID		0x0011
#define AL_CRYPTO_ALPINE_V2_DEV_ID	0x0022
#define AL_CRYPTO_ALPINE_V3_DEV_ID	AL_CRYPTO_ALPINE_V2_DEV_ID
#define AL_CRYPTO_REV_ID_0		0
#define AL_CRYPTO_REV_ID_1		AL_SSM_REV_ID_REV1
#define AL_CRYPTO_REV_ID_2		AL_SSM_REV_ID_REV2
#define AL_CRYPTO_REV_ID_3		AL_SSM_REV_ID_REV3

#define CACHED_SAD_SIZE			16
#define CRC_IV_CACHE_SIZE		8

/** How many descriptors to save between head and tail in case of
 * wrap around.
 */
#define AL_CRYPT_DESC_RES 0

/* Application IOFIC definitions */
#define AL_CRYPTO_APP_REGS_BASE_OFFSET(rev_id) (((rev_id) < AL_CRYPTO_REV_ID_2) ? 0x800 : 0x4000)
#define AL_CRYPTO_APP_IOFIC_OFFSET	0x0

/* interrupt controller group A */
#define AL_CRYPTO_APP_INT_A_S2M_TIMEOUT				AL_BIT(0)
#define AL_CRYPTO_APP_INT_A_M2S_TIMEOUT				AL_BIT(1)
#define AL_CRYPTO_APP_INT_A_EOP_WITHOUT_SOP			AL_BIT(2)
#define AL_CRYPTO_APP_INT_A_SOP_WITHOUT_EOP			AL_BIT(3)
#define AL_CRYPTO_APP_INT_A_SOP_WITH_EOP_TOGETHER		AL_BIT(4)
#define AL_CRYPTO_APP_INT_A_UNMAP_PROTOCOL			AL_BIT(5)
#define AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_SA			AL_BIT(6)
#define AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_IV			AL_BIT(7)
#define AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_ICV			AL_BIT(8)
#define AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_DMB_FUNC_0		AL_BIT(9)
#define AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_DMB_FUNC_1		AL_BIT(10)
#define AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_DMB_FUNC_2		AL_BIT(11)
#define AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_DMB_FUNC_3		AL_BIT(12)
#define AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_E2M_PAYLOAD		AL_BIT(13)
#define AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_E2M_SIGNATURE		AL_BIT(14)
#define AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_D2E_DATA		AL_BIT(15)
#define AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_D2E_CONTROL		AL_BIT(16)
#define AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_INGRESS		AL_BIT(17)
#define AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_ALIGNER		AL_BIT(18)
#define AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_D2E_CONTEXT		AL_BIT(19)
#define AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_D2E_HASH_ST		AL_BIT(20)
#define AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_D2E_OPAD		AL_BIT(21)
#define AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_E2D_WRITE_B		AL_BIT(22)
#define AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_E2M_INTR		AL_BIT(23)
/* interrupt controller group A rev V3 */
#define AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_D2E_GMAC_STATE		AL_BIT(24)
#define AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_D2E_COMP_HIST		AL_BIT(25)
#define AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_D2E_HUFFMAN_D_TBL	AL_BIT(26)
/***********************************************************************/
#define AL_CRYPTO_APP_INT_A_ALL	\
	AL_CRYPTO_APP_INT_A_S2M_TIMEOUT | \
	AL_CRYPTO_APP_INT_A_M2S_TIMEOUT | \
	AL_CRYPTO_APP_INT_A_EOP_WITHOUT_SOP | \
	AL_CRYPTO_APP_INT_A_SOP_WITHOUT_EOP | \
	AL_CRYPTO_APP_INT_A_SOP_WITH_EOP_TOGETHER | \
	AL_CRYPTO_APP_INT_A_UNMAP_PROTOCOL | \
	AL_CRYPTO_APP_INT_A_FIFO_OVERRUN_SA

/* interrupt controller group B rev V2*/
#define AL_CRYPTO_APP_INT_B_SA_IV_FIFO_OUT_EMPTY		AL_BIT(0)
#define AL_CRYPTO_APP_INT_B_M2S_ERROR				AL_BIT(2)
#define AL_CRYPTO_APP_INT_B_PAR_ERR_SAD_MEMORIES		AL_BIT(3)

/* interrupt controller group B rev V3*/
#define AL_CRYPTO_APP_INT_B_PAR_ERR_SAD_AUTH_IV_IN		AL_BIT(4)
#define AL_CRYPTO_APP_INT_B_PAR_ERR_SAD_ENC_KEY_AUTH_IV_OUT	AL_BIT(5)
#define AL_CRYPTO_APP_INT_B_PAR_ERR_SAD_ENC_PARAMS		AL_BIT(6)

/* interrupt controller group C */
#define AL_CRYPTO_APP_INT_C_PAR_ERR_DECOMP_HIST(idx)		AL_BIT(0 + (idx))
#define AL_CRYPTO_APP_INT_C_PAR_ERR_DECOMP_HIST_MAX		3
#define AL_CRYPTO_APP_INT_C_PAR_ERR_COMP_OUT_CONT_COMP		AL_BIT(4)
#define AL_CRYPTO_APP_INT_C_PAR_ERR_COMP_OUT_CONT_ORIG		AL_BIT(5)
#define AL_CRYPTO_APP_INT_C_PAR_ERR_COMP_HIST(idx)		AL_BIT(6 + (idx))
#define AL_CRYPTO_APP_INT_C_PAR_ERR_COMP_HIST_MAX		15
#define AL_CRYPTO_APP_INT_C_LZ77_LZSS_COMP_CAM_ENT_NOT_FOUND	AL_BIT(22)
#define AL_CRYPTO_APP_INT_C_LZ77_LZSS_COMP_INVALID_ALU_CMD	AL_BIT(23)
#define AL_CRYPTO_APP_INT_C_COMP_HIST_EXCEEDED			AL_BIT(24)
#define AL_CRYPTO_APP_INT_C_DECOMP_INVALID_FIELD_COMPOS		AL_BIT(25)
#define AL_CRYPTO_APP_INT_C_DECOMP_INVALID_FIELD_SIZE		AL_BIT(26)
#define AL_CRYPTO_APP_INT_C_DECOMP_OFFSET_NOT_RESOLVED		AL_BIT(27)
#define AL_CRYPTO_APP_INT_C_DECOMP_ZERO_CODE_WORD_CNT		AL_BIT(28)

/**
* Crypto operations
* These operations may be combined
*/
#define AL_CRYPT_ENC			1
#define AL_CRYPT_AUTH			2
#define AL_CRYPT_CMPRS			4

/* Combinations (deprecated) */
#define AL_CRYPT_ENC_ONLY		AL_CRYPT_ENC
#define AL_CRYPT_AUTH_ONLY		AL_CRYPT_AUTH
#define AL_CRYPT_ENC_AUTH		(AL_CRYPT_ENC | AL_CRYPT_AUTH)

/******************************************************************************
 * Crypto Enums                                                               *
 * Note: Crypto Enumerations values are important and must not be changed     *
 *       since their values are used to configure the hardware                *
 ******************************************************************************/

/** Encryption types */
enum al_crypto_sa_enc_type {
	AL_CRYPT_DES_ECB = 0,
	AL_CRYPT_DES_CBC = 1,
	AL_CRYPT_TRIPDES_ECB = 2,
	AL_CRYPT_TRIPDES_CBC = 3,
	AL_CRYPT_AES_ECB = 4,
	AL_CRYPT_AES_CBC = 5,
	AL_CRYPT_AES_CTR = 6,
	AL_CRYPT_AES_CCM = 7,
	AL_CRYPT_AES_GCM = 8,
	AL_CRYPT_AES_XTS = 9,
	AL_CRYPT_MAX = 10
};

/** 3des modes */
enum al_crypto_sa_tripdes_m {
	AL_CRYPT_TRIPDES_EDE = 1
};

/** AES key sizes */
enum al_crypto_sa_aes_ksize {
	AL_CRYPT_AES_128 = 0,
	AL_CRYPT_AES_192 = 1,
	AL_CRYPT_AES_256 = 2
};

/** Authentication types */
enum al_crypto_sa_auth_type {
	AL_CRYPT_AUTH_MD5 = 0,
	AL_CRYPT_AUTH_SHA1 = 1,
	AL_CRYPT_AUTH_SHA2 = 2,
	AL_CRYPT_AUTH_SHA3 = 3,
	AL_CRYPT_AUTH_SHA3_CONT = 4,
	AL_CRYPT_AUTH_AES_CCM = 5,
	AL_CRYPT_AUTH_AES_GCM = 6,
	AL_CRYPT_AUTH_GMAC = 7,
	AL_CRYPT_AUTH_CRC8 = 8,
	AL_CRYPT_AUTH_CRC16 = 9,
	AL_CRYPT_AUTH_CRC32 = 10,
	AL_CRYPT_AUTH_CSUM8 = 11,
	AL_CRYPT_AUTH_CSUM16 = 12,
	AL_CRYPT_AUTH_CSUM32 = 13,
};

/** SHA2 modes */
enum al_crypto_sa_sha2_mode {
	AL_CRYPT_SHA2_256 = 0,
	AL_CRYPT_SHA2_384 = 1,
	AL_CRYPT_SHA2_512 = 2
};

/** SHA3 modes */
enum al_crypto_sa_sha3_mode {
	AL_CRYPT_SHA3_224 = 0,
	AL_CRYPT_SHA3_256 = 1,
	AL_CRYPT_SHA3_384 = 2,
	AL_CRYPT_SHA3_512 = 3
};

/** CRC8 modes */
enum al_crypto_sa_crc8_mode {
	AL_CRYPT_CRC8_CRC8 = 0,
	AL_CRYPT_CRC8_CRC8N = 1,
};

/** CRC16 modes */
enum al_crypto_sa_crc16_mode {
	AL_CRYPT_CRC16_T10 = 0,
};

/** CRC32 modes */
enum al_crypto_sa_crc32_mode {
	AL_CRYPT_CRC32_CRC32 = 0,
	AL_CRYPT_CRC32_CRC32C = 1,
};

/** CSUM32 modes */
enum al_crypto_sa_csum32_mode {
	AL_CRYPT_CSUM32_STD = 0,
	AL_CRYPT_CSUM32_ADLER = 1,
};

/** GMAC Mask ID */
enum al_crypto_gmac_mask_id {
	AL_CRYPT_MASK_ID_0 = 0,
	AL_CRYPT_MASK_ID_1 = 1,
	AL_CRYPT_MASK_ID_2 = 2,
	AL_CRYPT_MASK_ID_3 = 3
};

/** SHA3 modes extension */
enum al_crypto_sa_sha3_mode_ext {
	AL_CRYPT_SHA3_DEF = 0,
	AL_CRYPT_SHAKE_128 = 1,
	AL_CRYPT_SHAKE_256 = 2
};

/** CNTR size */
enum al_crypto_cntr_size {
	AL_CRYPT_CNTR_16_BIT = 0,
	AL_CRYPT_CNTR_32_BIT = 1,
	AL_CRYPT_CNTR_64_BIT = 2,
	AL_CRYPT_CNTR_128_BIT = 3
};

/** CTS Mode **/
enum al_crypto_cts_mode {
	AL_CRYPT_CTS_DISABLE = 0,
	AL_CRYPT_CTS_ECB_XTS_CBC1 = 1,
	AL_CRYPT_CTS_CBC2 = 2,
	AL_CRYPT_CTS_CBC3 = 3
};

/** Crypto PI sizes */
enum al_crypto_sa_pi_size {
	AL_CRYPT_PI_SIZE_0B = 0,
	AL_CRYPT_PI_SIZE_4B = 1,
	AL_CRYPT_PI_SIZE_8B = 2,
	AL_CRYPT_PI_SIZE_16B = 3,
};

/** SA Length
 * Crypto SA length, dictates number of entries in SA:
 * number of entries = (2^al_crypto_sa_length).
 * Note1: most crypto modes requires single sa entry
 * Caution: enum values are important and should NOT be changed
 * The crypto modes that require multiple sa entries are listed below:
 * 1. sha-3 / shake broken-pkt (4 entries)
 * 2. sha-3 / shake HMAC (4 entries)
 */
enum al_crypto_sa_length {
	AL_CRYPT_SINGLE_LINE = 0,
	AL_CRYPT_DOUBLE_LINE = 1,
	AL_CRYPT_QUAD_LINE = 2,
	AL_CRYPT_OCTA_LINE = 3
};

/**
 * Inline CRC Profile ID
 * Crypto inline CRC profile ID specifying the inline CRC variant performed
 */
enum al_crypto_icrc_prof_id {
	AL_CRYPT_ICRC_PROF_ID_0 = 0,
	AL_CRYPT_ICRC_PROF_ID_1 = 1,
	AL_CRYPT_ICRC_PROF_ID_2 = 2,
	AL_CRYPT_ICRC_PROF_ID_3 = 3,
	AL_CRYPT_ICRC_PROF_ID_4 = 4,
	AL_CRYPT_ICRC_PROF_ID_5 = 5,
	AL_CRYPT_ICRC_PROF_ID_6 = 6,
	AL_CRYPT_ICRC_PROF_ID_7 = 7,
	AL_CRYPT_ICRC_PROF_ID_8 = 8,
	AL_CRYPT_ICRC_PROF_ID_9 = 9,
	AL_CRYPT_ICRC_PROF_ID_10 = 10,
	AL_CRYPT_ICRC_PROF_ID_11 = 11,
	AL_CRYPT_ICRC_PROF_ID_12 = 12,
	AL_CRYPT_ICRC_PROF_ID_13 = 13,
	AL_CRYPT_ICRC_PROF_ID_14 = 14,
	AL_CRYPT_ICRC_PROF_ID_15 = 15,
};

/** Compression Profile ID
 * Crypto compression profile ID specifying the compression variant performed
 */
enum al_crypto_cmprs_prof_id {
	AL_CRYPT_CMPRS_LZ77_1B_OFFSET_256B_HISTORY = 0,
	AL_CRYPT_CMPRS_LZ77_2B_OFFSET_512B_HISTORY = 1,
	AL_CRYPT_CMPRS_LZ77_2B_OFFSET_1KB_HISTORY = 2,
	AL_CRYPT_CMPRS_LZ77_2B_OFFSET_4KB_HISTORY = 3,
	AL_CRYPT_CMPRS_LZ77_2B_OFFSET_8KB_HISTORY = 4,
	AL_CRYPT_CMPRS_LZ77_2B_OFFSET_16KB_HISTORY = 5,
	AL_CRYPT_CMPRS_LZ77_2B_OFFSET_32KB_HISTORY = 6,
	AL_CRYPT_CMPRS_LZSS_1B_OFFSET_256B_HISTORY = 7,
	AL_CRYPT_CMPRS_LZSS_2B_OFFSET_512B_HISTORY = 8,
	AL_CRYPT_CMPRS_LZSS_2B_OFFSET_1KB_HISTORY = 9,
	AL_CRYPT_CMPRS_LZSS_2B_OFFSET_4KB_HISTORY = 10,
	AL_CRYPT_CMPRS_LZSS_2B_OFFSET_8KB_HISTORY = 11,
	AL_CRYPT_CMPRS_LZSS_2B_OFFSET_16KB_HISTORY = 12,
	AL_CRYPT_CMPRS_LZSS_2B_OFFSET_32KB_HISTORY = 13,
	AL_CRYPT_CMPRS_LZ4_512B_HISTORY = 14,
	AL_CRYPT_CMPRS_LZ4_1KB_HISTORY = 15,
	AL_CRYPT_CMPRS_LZ4_4KB_HISTORY = 16,
	AL_CRYPT_CMPRS_LZ4_8KB_HISTORY = 17,
	AL_CRYPT_CMPRS_LZ4_16KB_HISTORY = 18,
	AL_CRYPT_CMPRS_LZ4_32KB_HISTORY = 19,
	AL_CRYPT_CMPRS_STATIC_DEFLATE_512B_HISTORY = 20,
	AL_CRYPT_CMPRS_STATIC_DEFLATE_1KB_HISTORY = 21,
	AL_CRYPT_CMPRS_STATIC_DEFLATE_4KB_HISTORY = 22,
	AL_CRYPT_CMPRS_STATIC_DEFLATE_8KB_HISTORY = 23,
	AL_CRYPT_CMPRS_STATIC_DEFLATE_16KB_HISTORY = 24,
	AL_CRYPT_CMPRS_STATIC_DEFLATE_32KB_HISTORY = 25,
	AL_CRYPT_CMPRS_DYNAMIC_DEFLATE_512B_HISTORY = 26,
	AL_CRYPT_CMPRS_DYNAMIC_DEFLATE_1KB_HISTORY = 27,
	AL_CRYPT_CMPRS_DYNAMIC_DEFLATE_4KB_HISTORY = 28,
	AL_CRYPT_CMPRS_DYNAMIC_DEFLATE_8KB_HISTORY = 29,
	AL_CRYPT_CMPRS_DYNAMIC_DEFLATE_16KB_HISTORY = 30,
	AL_CRYPT_CMPRS_DYNAMIC_DEFLATE_32KB_HISTORY = 31
};

/** Compression Algorithm Index
 * Crypto compression algorithm performed
 */
enum al_crypto_cmprs_alg_idx {
	AL_CRYPT_CMPRS_LZ77_1B_OFFSET = 0,
	AL_CRYPT_CMPRS_LZ77_2B_OFFSET = 1,
	AL_CRYPT_CMPRS_LZSS_1B_OFFSET = 2,
	AL_CRYPT_CMPRS_LZSS_2B_OFFSET = 3,
	AL_CRYPT_CMPRS_LZ4 = 4,
	AL_CRYPT_CMPRS_SNAPPY = 5,
	AL_CRYPT_CMPRS_LZO = 6,
	AL_CRYPT_CMPRS_DEFLATE = 7,
	AL_CRYPT_CMPRS_ALG_RESERVED = 8
};

/** Compression Algorithm Types
 * Crypto compression supports multiple types:
 * 1. LZ code word based supports strict form compression and is used for LZ77 and LZSS variants.
 * 2. LZ stream based support flexible stream code words and is used for LZ4 and Snappy variants.
 * 3. Static deflate performs compression of code words with pre-determined Huffman probability tables.
 * 4. Dynamic deflate performs compression of code words with dynamically loaded Huffman probability tables.
 */
enum al_crypto_algorithm_type {
	AL_CRYPT_CMPRS_LZ_CODE_WORD = 0,
	AL_CRYPT_CMPRS_LZ_STREAM = 1,
	AL_CRYPT_CMPRS_STATIC_DEFLATE = 2,
	AL_CRYPT_CMPRS_DYNAMIC_DEFLATE = 3
};

/** Compression Format Index
 * Crypto compression form of compressed code word
 */
enum al_crypto_cmprs_format_idx {
	AL_CRYPT_CMPRS_CW_1B_SYMBOL = 0,
	AL_CRYPT_CMPRS_CW_2B_OFFSET_LENGTH = 1,
	AL_CRYPT_CMPRS_CW_3B_OFFSET_LENGTH = 2,
	AL_CRYPT_CMPRS_CW_3B_OFFSET_LENGTH_SYMBOL = 3,
	AL_CRYPT_CMPRS_CW_4B_OFFSET_LENGTH_SYMBOL = 4,
	AL_CRYPT_CMPRS_STREAM = 5,
	AL_CRYPT_CMPRS_PROB_TABLES = 6
};

/** Compression CAM Entries
 * Crypto compression CAM searching for a proper code word encoding format
 */
enum al_crypto_cmprs_cam_id {
	AL_CRYPT_CMPRS_LZ77_1B_OFFSET_NO_FLAGS = 0,
	AL_CRYPT_CMPRS_LZ77_2B_OFFSET_NO_FLAGS = 1,
	AL_CRYPT_CMPRS_LZSS_1B_OFFSET_COMPRESSED = 2,
	AL_CRYPT_CMPRS_LZSS_1B_OFFSET_UNCOMPRESSED = 3,
	AL_CRYPT_CMPRS_LZSS_2B_OFFSET_COMPRESSED = 4,
	AL_CRYPT_CMPRS_LZSS_2B_OFFSET_UNCOMPRESSED = 5,
	AL_CRYPT_CMPRS_DEFLATE_COMPRESSED = 6,
	AL_CRYPT_CMPRS_DEFLATE_UNCOMPRESSED = 7,
	AL_CRYPT_CMPRS_FORMAT_RESERVED_0 = 8
};

struct al_crypto_cmprs_huffman_static_tbl_entry {
	/** The Huffman coding for entry with appropriate distance. */
	unsigned int code;
	/** Huffman code length. The code should be lsb aligned for partial length. */
	unsigned int length;
	/**
	 * Extra bits following the code found in the stream, used to determine the exact value.
	 * Irrelevant for literal and end-of-buffer entries whereas extra_bits==0.
	 */
	unsigned int extra_bits;
	/**
	 * The distance range for the encoded entry.
	 * Irrelevant for literal and end-of-buffer entries whereas dist==entry_id.
	 * For length entries only 9 lsb bits are used.
	 */
	unsigned int dist;
};

/** Crypto SA (Security Association) parameters match the HW crypto SAD
 * The cached SAD is not managed by the HAL, The HAL only supply the ability
 * to push new SA to the cached SAD and evict a cached SAD through the
 * al_crypto_dma_action API.
 * Evicting an SA may be required in the following cases:
 * - Each time SA is evicted while using IV generated by the Crypto engine
 * - Each time SA is evicted while using the SA to hold a temp MAC signature
 * - On the first time SA is evicted when using AES decryption key
 *   generated by the HW
 * Fetching an SA can be done by pushing a new SA entry through
 * al_crypto_transaction SA_in and placing an appropriate buffer in the
 * SA_out.
 * Initializing a new SA entry should be done through al_crypto_hw_sa_init.
 *
 */
struct al_crypto_sa {
	/** Crypto operations - Combination of AL_CRYPT_* */
	unsigned int sa_op;   /**< crypto operation */

	/* Enc */
	enum al_crypto_sa_enc_type enc_type;
	enum al_crypto_sa_tripdes_m tripdes_m;	/**< 3des mode EDE */
	enum al_crypto_sa_aes_ksize aes_ksize;
	enum al_crypto_cntr_size cntr_size;  /**< relevant only for Alg using CNTR mode*/
	enum al_crypto_cts_mode cts_mode;  /**< relevant only for Alg using CTS - ECB, CBC, XTS*/
	uint32_t enc_offset; /**<
			enc/cmprs start offset from start of buffer,
			used only if not set through the crypto operation */
	uint32_t enc_offset_eop; /**<
			enc/cmprs offset from end of buffer,
			used only if not set through the crypto operation */

	uint8_t enc_key[32];
	uint8_t enc_iv[16];
	/* XTS support - Not relevant for Alpine V1 */
	uint8_t enc_xts_tweak_key[32]; /**< xts tweak key */
	uint8_t enc_xts_sector_size;   /**<
			sector size for XTS (8 LSBs):
			0 – no multiple sectors support
			N>0 – multiple sectors support enabled, with sector size = 16*2^N */
	uint8_t enc_xts_protection;   /**<
			protection data for XTS ([15:8])
			0 – no additional protection data
			N>0 – N bytes of additional protection data */

	/* Auth */
	enum al_crypto_sa_auth_type auth_type;
	enum al_crypto_sa_sha2_mode sha2_mode;
	enum al_crypto_sa_sha3_mode sha3_mode;
	enum al_crypto_sa_sha3_mode_ext sha3_mode_ext;
	enum al_crypto_sa_crc8_mode crc8_mode;
	enum al_crypto_sa_crc16_mode crc16_mode;
	enum al_crypto_sa_crc32_mode crc32_mode;
	enum al_crypto_sa_csum32_mode csum32_mode;
	enum al_crypto_icrc_prof_id icrc_prof_id;
	enum al_crypto_gmac_mask_id gmac_mask_id;
	al_bool auth_hmac_en;
	uint32_t signature_size; /**< sign size out in 4 * (size + 1) bytes */
	al_bool auth_signature_msb; /**< when the signature output size is smaller than
			the authentication algorithm output size take the more significant
			bits from the full size signature */
	uint32_t auth_offset;    /**<
			auth start offser from start of buffer,
			used only if not set through the crypto operation */
	uint32_t auth_offset_eop;/**<
			auth offset from end of buffer,
			used only if not set through the crypto operation */
	uint8_t auth_iv_in[64];
	uint8_t icrc_iv_in[4];
	union {
		uint8_t hmac_iv_in[64]; /**< H(K xor ipad) */
		struct {
			uint8_t gmac_iv_in[16]; /** GMAC IV */
			uint8_t gmac_h_in[16]; /** GMAC H Subkey */
			uint8_t gmac_aes_key_in[32]; /** GMAC AES Key */
		};
	};
	uint8_t hmac_iv_out[64];/**< H(K xor opad) */
	uint8_t enc_ccm_cbc_iv_add[4]; /**<
			Used in CCM to generate Auth IV from encryption IV */
	uint8_t aes_gcm_auth_iv[16]; /**< GCM auth IV */

	/* Combined */
	al_bool sign_after_enc;  /**< common case is true */
	al_bool auth_after_dec;  /**< common case is false */

	/* Compression */
	al_bool cmprs_lz_en;			/** LZ compression enabled */
	enum al_crypto_cmprs_prof_id cmprs_lz_prof_idx;		/** LZ compression profile index */
	unsigned int cmprs_dynamic_deflate_table_idx;	/** Dynamic deflate decompression table ID */
	al_bool cmprs_forced_compressed_out;			/** LZ compression skip output controller and send
																							compressed data directly */

	/* Encrypted Signatue */
	/* PI == protection information.
	   The signature is a part of the PI buffer.
	   pi_size determines the size of the PI buffer.
	   pi_crc_offset determine the location of the signature within the PI.
	   Encrypted signature is a feature where the signature is encrypted/compressed
	   along the regular encryption/compression buffer. */
	al_bool encrypted_signature;
	enum al_crypto_sa_pi_size pi_size;
	uint8_t pi_crc_offset;

	/* SA Length */
	enum al_crypto_sa_length sa_length;
};

/** A single Crypto SA HW as cached in the SAD, each SA is described as an
 * array of 32-bit words */
struct al_crypto_hw_sa {
	uint32_t sa_word[64];
};

/** Crypto operation direction, values according to HW descriptor setting */
enum al_crypto_dir {
	AL_CRYPT_ENCRYPT = 0,
	AL_CRYPT_DECRYPT = 1,
};

/* transaction completion status */
#define AL_CRYPT_AUTH_ERROR			AL_BIT(0)
#define AL_CRYPT_SA_IV_EVICT_FIFO_ERROR		AL_BIT(8)
#define AL_CRYPT_DES_ILLEGAL_KEY_ERROR		AL_BIT(9)
#define AL_CRYPT_M2S_ERROR			AL_BIT(10)
#define AL_CRYPT_SRAM_PARITY_ERROR		AL_BIT(11)
#define AL_CRYPT_INTERNAL_FLOW_VIOLATION_ERROR	AL_BIT(15)

/** Crypto transaction for enc, auth, cmprs or their combination
 * In case sa_update, iv_*, auth_* are not valid, set the al_buf->len to 0
 * In case dst is not valid set the al_block->num to 0.
 *
 * All Crypto transaction are associated with a cached SA,
 * this SA is passed as the SA index into the cached SAD sa_index.
 *
 * The al_crypto_dma_action support source scatter list buffer encryption,
 * authentication, compression, and their combination in one pass.
 *
 * When using an SA type of Authentication only, the Crypto can support
 * splitting the Authentication operation into few requests by using the
 * auth_first, last and valid flags and using an Authentication IV auth_iv_in
 * and auth_iv_out.
 *
 * When using an SA type of Encryption, the Crypto can get the IV required for the encryption from
 * the upper layer enc_iv_in, or using IV generated by the engine (based on the previous encryption
 * executed using this SA). In any case the IV used by the engine can be passed to the upper layer
 * through the enc_iv_out.
 *
 * When executing a signature verification operation on an SA type of
 * Encryption and Authentication or of Authentication Only with last
 * indication, the crypto can compare the actual buffer signature
 * auth_sign_in to the engine outcome and indicate the result on the S2M
 * completion. In any case the engine signature can be passed to the upper
 * layer through the auth_sign_out.
 *
 * When using Authentication only and dst isnt empty the src will be copied
 * to the dst.
 */
struct al_crypto_transaction {
	enum al_crypto_dir dir;
	enum al_ssm_op_flags flags;
	struct al_block src;	/**< In data - scatter gather*/
	uint32_t src_size;	/**< Size of source buffer */
	struct al_block dst;    /**< Out data - scatter gather */
	uint32_t tx_descs_count;/* number of tx descriptors created for this */
				/* transaction, this field set by the hal */

	/* SA */
	uint32_t sa_indx;	/**< SA index in the cached SAD to use */
	struct al_buf sa_in;	/**< pointer to SA al_crypto_hw_sa to
				  update in the cached SAD */
	struct al_buf sa_out;	/**< pointer to SA where to place
				  old cached SA */

	/* Enc */
	struct al_buf enc_iv_in;	/**< IV from user, if not set will
					  use IV from the SA */
	struct al_buf enc_iv_out;	/**< Optional - Buffer to place
					  the used IV */
	struct al_buf enc_next_iv_out;	/**< Optional - Buffer to place
					  next used IV */
	uint32_t enc_in_off;		/**< offset where to start enc/cmprs */
	/**<
	 * length of enc/cmprs, if len set to 0 will use SA defaults
	 * valid if 'enc_in_off_eop_valid' is AL_FALSE
	 */
	uint32_t enc_in_len;
	/**< 'enc_in_off_eop' valid indication */
	al_bool enc_in_off_eop_valid;
	/**< offset where to stop enc/cmprs (relative to end of packet) */
	uint32_t enc_in_off_eop;

	/* Auth */
	al_bool auth_fl_valid;		/**<
			valid indication for the auth first and last
			indications */
	al_bool auth_first;		/**< Relevant for auth only SA */
	al_bool auth_last;		/**< Relevant for auth only SA */
	struct al_buf auth_iv_in;	/**< In case of auth only SA and
					  auth_first isnt set this is the
					  intermediate auth input. */
	struct al_buf auth_iv_out;	/**< In case of auth only SA and
					  auth_last isnt set, This is the
					  intermediate auth output. */
	struct al_buf auth_sign_in;	/**< The Signature to validate in front
					  of auth output.*/
	struct al_buf auth_sign_out;	/**< In case of authentication SA with last
					  this is the signature output
					  of the auth.
					  Size should be as indicated in the
					  SA signature_size
					  (sign_size+1)*4		*/
	uint32_t auth_in_off; /**< offset where to start auth */
	/**<
	 * length of auth, if len set to 0 will use SA defaults
	 * valid if 'auth_in_off_eop_valid' is AL_FALSE
	 */
	uint32_t auth_in_len;
	/**< 'auth_in_off_eop' valid indication */
	al_bool auth_in_off_eop_valid;
	/**< offset where to stop auth (relative to end of packet) */
	uint32_t auth_in_off_eop;
	uint32_t auth_bcnt;   /**<
			This field should be zero, unless this
			this packet is for AUTH only SA with
			last and not first.
			In this case is will indicate the byte
			count of the auth data till this point.
			When using this feild the auth off and len
			must contain a valid data */

	/* Compression */
	al_bool	cmprs_fl_valid;			/** Valid indication for the compression fields */
	al_bool cmprs_his_win_rst;		/** History window reset (pre packet) */
	struct al_buf	cmprs_his_win_in;	/** History window (input) */
	struct al_buf	cmprs_his_win_out_post;	/** History window (output, post packet) */
	struct al_buf	cmprs_dynamic_deflate_dec_tbl; /** Dynamic deflate decode table (pre packet) */

	/* Compression + Encryption */
	al_bool	cmprs_enc_padding_valid; /** Valid indication for compression+Encryption */
	al_bool	cmprs_enc_auto_padding;	/**<
			If this bit is set to one, padding is applided
			for all compression+Encryption packet sizes.
		  Otherwise, padding is applied only in cases
			where the compressed packet < cmprs_enc_padding_res */
	uint32_t cmprs_enc_padding_res;	/**<
			This field defines automatic padding that is added/removed on
			compression->encryption / decryption->decompression flows.
			On compress/encrypt direction – this field determines the required
			block-alignment towards the encryption engine. Supported values are
			1 (no padding), 8 and 16 (N means pad to N-bytes boundary).
			On decrypt/decompress direction – this value represents the amount
			of padding that were added to the compressed block prior to
			encryption, in bytes. The Crypto engine will decrypt the block,
			disregard the last N bytes (as coded in this field),
			and then decompress the result. */

	/* Encrypted Signatue */
	struct al_buf protection_info_in;
};



/** GMAC override */
struct al_crypto_gmac_ovrd {
	/**
	 * Chicken bit that disables parallel operation of gmac. Allows more reliable
	 * work with high latency of minimum 16 cycles per packet
	 */
	al_bool do_auth_serial;

	/** Override profile table to enforce gmac calculation */
	al_bool auth_override_en;

	/** Override profile table to disable gmac calculation */
	al_bool auth_override_dis;

	/**
	 * Override profile table to determine gmac operation direction (generation or
	 * validation)
	 */
	al_bool val_gen_n_overrid_en;

	/**
	 * 0 – Signature generation. 1 – Signature validation. Only relevant if override
	 * bit is enabled
	 */
	al_bool val_gen_n;

	/** Override profile table to determine gmac authentication mode */
	al_bool auth_mode_override;

	/** 0 – GMAC. 1:3 – Reserved. Only relevant if override bit is enabled */
	unsigned int auth_mode;

	/** Override profile table to determine aes key type */
	al_bool key_type_override;

	/**
	 * 0 – 128-bit key. 1 –192-bit key. 2 – 256-bit key. 3 – Reserved. Only relevant
	 * if override bit is enabled
	 */
	unsigned int key_type;

	/** Override profile table to determine signature size */
	al_bool sign_size_override;

	/** Sign size is 4*(N+1) bytes. Only lower 2 bits are used. */
	unsigned int sign_size;

	/** Override profile table to determine signature digest polarity */
	al_bool digest_from_msb_override;

	/** 0 – Digest from LSB. 1 – Digest from MSB. Only relevant if overrid bit is enabled */
	al_bool digest_from_msb;
};

/** GMAC endianity swap element */
struct al_crypto_gmac_endianity_swap_element {
	al_bool bit_swap;
	al_bool byte_swap;
	al_bool word_swap;
	al_bool aes_beat_swap; /* when relevant */
};

/** GMAC endianity swap */
struct al_crypto_gmac_endianity_swap {
	struct al_crypto_gmac_endianity_swap_element ingress;
	struct al_crypto_gmac_endianity_swap_element egress;
	struct al_crypto_gmac_endianity_swap_element aes_key;
	struct al_crypto_gmac_endianity_swap_element h_subkey;
	struct al_crypto_gmac_endianity_swap_element j0;
	struct al_crypto_gmac_endianity_swap_element sign_in;
	struct al_crypto_gmac_endianity_swap_element sign_out;
};

/** GMAC data mask polarity */
struct al_crypto_gmac_data_mask {
	al_bool		pol_id_0_mask_val;
	al_bool		pol_id_1_mask_val;
	al_bool		pol_id_2_mask_val;
	al_bool		pol_id_3_mask_val;

	struct {
		unsigned int	id_0;
		unsigned int	id_1;
		unsigned int	id_2;
		unsigned int	id_3;
	} arr[16];
};

/** GMAC config */
struct al_crypto_gmac_cfg {
	/** override config - NULL if defaults are required */
	const struct al_crypto_gmac_ovrd		*ovrd_cfg;
	/** endianity swap config - NULL if defaults are required */
	const struct al_crypto_gmac_endianity_swap	*endianity_swap_cfg;
	/** data mask polarity config - NULL if defaults are required */
	const struct al_crypto_gmac_data_mask		*data_mask_cfg;
};

/**
 * Crypto GMAC init
 *
 * @param	dma
 *		Crypto DMA handle
 * @param	app_regs
 *		Crypto application registers
 * @param	cfg
 *		The required configuration, or NULL if defaults are required
 */
void al_crypto_gmac_init(
	struct al_ssm_dma		*dma,
	void __iomem			*app_regs,
	const struct al_crypto_gmac_cfg	*cfg);

/** Inline CRC start mask */
struct al_crypto_icrc_cfg_start_mask {
	uint32_t c_mask[4];
};

/** Inline CRC config */
struct al_crypto_icrc_cfg {
	al_bool					prof_id_override_en;
	enum al_crypto_icrc_prof_id		prof_id;
	struct al_crypto_icrc_cfg_start_mask	start_masks[4];
};

/**
 * Crypto Inline CRC init
 *
 * @param	dma
 *		Crypto DMA handle
 * @param	app_regs
 *		Crypto application registers
 * @param	cfg
 *		The required configuration, or NULL if defaults are required
 */
void al_crypto_icrc_init(
	struct al_ssm_dma		*dma,
	void __iomem			*app_regs,
	const struct al_crypto_icrc_cfg	*cfg);

/**
 * Crypto compression LZ init
 *
 * @param	dma
 *		Crypto DMA handle
 * @param	app_regs
 *		Crypto application registers
 */
void al_crypto_cmprs_lz_init(
	struct al_ssm_dma	*dma,
	void __iomem		*app_regs);

/**
 * Crypto compression Huffman init
 *
 * @param	dma
 *		Crypto DMA handle
 * @param	app_regs
 *		Crypto application registers
 */
void al_crypto_cmprs_huffman_init(
	struct al_ssm_dma	*dma,
	void __iomem		*app_regs);

/**
 * Crypto compression Huffman static table entry setting
 *
 * @param	dma
 *		Crypto DMA handle
 * @param	app_regs
 *		Crypto application registers
 * @param	entry_index
 *		Entry index
 * @param	entry
 *		Entry contents
 */
void al_crypto_cmprs_huffman_static_tbl_entry_set(
	struct al_ssm_dma				*dma,
	void __iomem					*app_regs,
	unsigned int					entry_index,
	struct al_crypto_cmprs_huffman_static_tbl_entry	*entry);

/**
 * Crypto compression Huffman dynamic table default setting
 *
 * @param	buff
 *		Huffman dynamic table buffer
 * @param	buff_size
 *		Huffman dynamic table buffer size - should be 1294 bytes
 */
void al_crypto_cmprs_huffman_static_tbl_default_set(
	void						*buff,
	unsigned int					buff_size);

/**
 * Crypto compression Huffman dynamic table entry setting
 *
 * @param	buff
 *		Huffman dynamic table buffer
 * @param	buff_size
 *		Huffman dynamic table buffer size - should be 1294 bytes
 * @param	entry_index
 *		Entry index
 * @param	entry
 *		Entry contents
 */
void al_crypto_cmprs_huffman_dynamic_tbl_entry_set(
	void						*buff,
	unsigned int					buff_size,
	unsigned int					entry_index,
	struct al_crypto_cmprs_huffman_static_tbl_entry	*entry);

/**
 * Initialize hw_sa (n entries, according to sa length)
 *
 * @param sa crypto SA array containing the desired SA parameters
 * @param hw_sa crypto HW SA array filled with zeros
 * to be initialized according to the sa
 * caution: sa and hw_sa arrays should have 2^sa_length initialized entries
 */
void al_crypto_hw_sa_init(struct al_crypto_sa *sa,
			 struct al_crypto_hw_sa *hw_sa);

/**
 * Prepare crypto/auth transaction to the HW
 *
 * Perform the following steps:
 *  - Calculate needed RX descriptors and check if the RX UDMA have available
 *   space. The number of descriptors depends on which buffers are passed in
 *   the transaction (SA_out, enc_IV_out, Sign_out) and the number of dest
 *   buffers.
 *  - Do the same for TX descriptors. The number of descriptors depends on
 *   which buffers are passed in the transaction (SA_in, enc_IV_in, Sign_in)
 *   and the number of source buffers.
 *  - Prepare the RX descriptors.
 *  - Update the tail pointer of the submission ring of the RX UDMA
 *   about the new prepared descriptors.
 *  - Prepare the TX descriptors.
 *
 * Note: UDMA queue can be used either for crypto/authentication transactions
 * or for crc/csum/memcpy transactions, but not for both types.
 *
 * @param dma crypto DMA handle
 * @param qid queue index
 * @param xaction transaction context, number of prepared TX descriptors is
 * returned in xaction->tx_descs_count
 *
 * @return 0 if no error found.
 *	   -ENOSPC if no space available.
 */
int al_crypto_dma_prepare(struct al_ssm_dma *dma, uint32_t qid,
			  struct al_crypto_transaction *xaction);

/**
 * Start asynchronous execution of crypto/auth or CRC/Checksum transaction
 *
 * Update the tail pointer of the submission ring of the TX UDMA about
 * previously prepared descriptors.
 * This function could return before the hardware start the work as its an
 * asynchronous non-blocking call to the hardware.
 *
 * @param dma crypto DMA handle
 * @param qid queue index
 * @param tx_descs number of tx descriptors to be processed by the engine
 *
 * @return 0 if no error found.
 *	   -EINVAL if quid is out of range
 */
int al_crypto_dma_action(struct al_ssm_dma *dma, uint32_t qid,
			 int tx_descs);

/**
 * Check and cleanup completed transaction
 *
 *  when the upper layer decides to check for completed transaction
 *  (e.g. due to interrupt) it calls al_crypto_dma_completion()
 *  API function provided by this driver. this function will call helper
 *  function provided by the m2m_udma module to check for completed requests.
 *  The al_crypto_dma_completion() is responsible for the cleanup of the
 *  completed request from the completion ring, so upper layer don't need to
 *  worry about the queues management.
 *  This driver doesn't provide the upper layer which transaction was
 *  completed, the upper layer should find this information by itself relying
 *  on the fact that for a given queue, the transaction completed in the same
 *  order it was sent to the same queue, no ordering is guaranteed between
 *  transaction that sent to different queues.

 * @param dma crypto DMA handle
 * @param qid queue index
 * @param comp_status status reported by rx completion descriptor
 *
 * @return the number of completed transactions.
 */
int al_crypto_dma_completion(struct al_ssm_dma *dma,
			     uint32_t qid,
			     uint32_t *comp_status);

/** Crypto XTS configuration */
struct al_crypto_xts_cfg {
	/** Enable pre increment bits */
	al_bool swap_pre_inc_bits;
	/** Enable pre increment bytes */
	al_bool swap_pre_inc_bytes;
	/** Enable pre increment words */
	al_bool swap_pre_inc_words;
	/** Enable post increment bits */
	al_bool swap_post_inc_bits;
	/** Enable post increment bytes */
	al_bool swap_post_inc_bytes;
	/** Enable post increment words */
	al_bool swap_post_inc_words;
};

/**
 * Crypto XTS default initialization
 *
 * @param	dma
 *		Crypto DMA handle
 * @param	app_regs
 *		Crypto accelerator register file
 * @param	cfg
 *		Required configuration (NULL for default configuration)
 */
void al_crypto_xts_cfg_set(
	struct al_ssm_dma		*dma,
	void __iomem			*app_regs,
	const struct al_crypto_xts_cfg	*cfg);

/**
 * Get SSM crypto error interrupts masks
 *
 * @param rev_id - PCI revision ID
 * @param crypto_a_mask - pointer to iofic group A mask
 * @param crypto_b_mask - pointer to iofic group B mask
 * @param crypto_c_mask - pointer to iofic group C mask
 * @param crypto_d_mask - pointer to iofic group D mask
 */
void al_crypto_error_ints_mask_get(unsigned int rev_id,
				   uint32_t *crypto_a_mask,
				   uint32_t *crypto_b_mask,
				   uint32_t *crypto_c_mask,
				   uint32_t *crypto_d_mask);

/**
 * Unmask crypto error interrupts
 *
 * @param	rev_id
 *		PCI adapter revision ID
 * @param	crypto_regs_base
 *		cryoto register base address
 * @param	udma_regs_base
 *		UDMA register base address
 */
void al_crypto_error_ints_unmask(uint8_t rev_id,
				 void __iomem *crypto_regs_base,
				 void __iomem *udma_regs_base);
/* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */
/** @} end of Crypto group */
#endif		/* __AL_HAL_CRYPT_H__ */
