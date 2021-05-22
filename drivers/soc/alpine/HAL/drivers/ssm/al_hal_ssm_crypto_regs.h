/*******************************************************************************
Copyright (C) 2016 Annapurna Labs Ltd.

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
 * @file   al_hal_ssm_crypto_regs.h
 *
 * @brief CRYPTO_Accelerator registers
 *
 */

#ifndef __AL_HAL_CRYPTO_REGS_H__
#define __AL_HAL_CRYPTO_REGS_H__

#include "al_hal_plat_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Unit Registers
 */

struct al_crypto_accelerator_configuration {
	/* [0x0] Unit Configuration register */
	uint32_t unit_conf;
	/* [0x4] Unit Debug Configuration register */
	uint32_t m2s_timeout;
	/* [0x8] Unit Debug Configuration register */
	uint32_t s2m_timeout;
	/* [0xc] enable memories for APB access (production testing / accelerated boot) */
	uint32_t mem_test;
};

struct al_crypto_accelerator_debug {
	/* [0x0] Unit Debug Configuration register */
	uint32_t unit_debug_conf;
	uint32_t rsrvd[3];
};

struct al_crypto_accelerator_log {
	/*
	 * [0x0] Descriptor word 0
	 * When a fatal error occurs, this register reflects the descriptor of the command that
	 * caused the fatal error.
	 */
	uint32_t desc_word0;
	/*
	 * [0x4] Descriptor word 1
	 * When a fatal error occurs, this register reflects the descriptor of the command that
	 * caused the fatal error.
	 */
	uint32_t desc_word1;
	/*
	 * [0x8] Transaction information of the command that triggered the error. When a fatal error
	 * occurs, this register reflects the transaction info of the command that caused the fatal
	 * error.
	 */
	uint32_t trans_info_1;
	/*
	 * [0xc] Transaction information of the command that triggered the error. When a fatal error
	 * occurs, this register reflects the transaction info of the command that caused the fatal
	 * error.
	 */
	uint32_t trans_info_2;
	uint32_t rsrvd[4];
};

struct al_crypto_accelerator_crypto_perf_counter {
	/*
	 * [0x0] The execution cycle counter measures the number of cycles that the CRYPTO
	 * accelerator is active (i.e., there is at least one valid command from the M2S or the pipe
	 * is not empty).
	 */
	uint32_t exec_cnt;
	/*
	 * [0x4] M2S active cycles counter
	 * Measures the number of cycles M2S sends command(s) to CRYPTO engine.
	 */
	uint32_t m2s_active_cnt;
	/*
	 * [0x8] M2S idle cycles counter
	 * Measures the number of idle cycles on M2S while the CRYPTO accelerator is waiting for
	 * data (the M2S_active_cnt is counting).
	 */
	uint32_t m2s_idle_cnt;
	/*
	 * [0xc] M2S backpressure cycles counter
	 * Measures the number of cycles the Crypto engine cannot accept data from the M2S while the
	 * M2S is ready to transfer data.
	 */
	uint32_t m2s_backp_cnt;
	/*
	 * [0x10] S2M active cycles counter
	 * Measures the number of cycles the Crypto engine sends command(s) to S2M. In cycles where
	 * more than one GDMA S2M is active, the counter is incremented by the number of the S2M
	 * interfaces that are active.
	 */
	uint32_t s2m_active_cnt;
	/*
	 * [0x14] S2M idle cycles counter
	 * Measures the number of idle cycles on S2M while S2M is waiting for data (the
	 * S2M_active_cnt is counting). In cycles where more than one GDMA is waiting for data from
	 * the CRYPTO, the counter is incremented by the number of S2M interfaces that are idle.
	 */
	uint32_t s2m_idle_cnt;
	/*
	 * [0x18] S2M backpressure cycles counter: Measures the number of cycles where the Crypto
	 * had data to send to the S2M, but it was not sent due to backpressure. In cycles where
	 * more than one S2M performs backpressure to the CRYPTO, the counter is incremented by the
	 * number of S2M interfaces that perform backpressure.
	 */
	uint32_t s2m_backp_cnt;
	/*
	 * [0x1c] Crypto Command Done Counter
	 * Total number of CRYPTO commands executed
	 */
	uint32_t cmd_dn_cnt;
	/*
	 * [0x20] CRYPTO Source Blocks Counter
	 * Total number of Source Blocks read in CRYPTO commands
	 */
	uint32_t src_blocks_cnt;
	/*
	 * [0x24] CRYPTO Destination Blocks Counter
	 * Total number of Destination Blocks written in CRYPTO commands
	 */
	uint32_t dst_blocks_cnt;
	/*
	 * [0x28] Recoverable Errors counter
	 * Total number of recoverable errors
	 */
	uint32_t recover_err_cnt;
	/* [0x2c] Counts the number of data beats entering CRYPTO. */
	uint32_t src_data_beats;
	/* [0x30] Counts the number of the data beats exiting CRYPTO. */
	uint32_t dst_data_beats;
	uint32_t rsrvd[7];
};

struct al_crypto_accelerator_perfm_cnt_cntl {
	/* [0x0] Performance counter control */
	uint32_t conf;
};

struct al_crypto_accelerator_crypto_status {
	/* [0x0] Crypto pipe status */
	uint32_t status;
};

struct al_crypto_accelerator_crypto_version {
	/* [0x0] Crypto Version */
	uint32_t revision;
	/* [0x4] Crypto Version */
	uint32_t date;
};

struct al_crypto_accelerator_crypto_algorithms {
	/* [0x0] Crypto Encryption Algorithms */
	uint32_t encryption;
	/* [0x4] Crypto Compression Algorithms */
	uint32_t compression;
};

struct al_crypto_accelerator_reserved {
	/* [0x0]  */
	uint32_t reserved_0;
	/* [0x4]  */
	uint32_t reserved_1;
	/* [0x8]  */
	uint32_t reserved_2;
	/* [0xc]  */
	uint32_t reserved_3;
};

struct al_crypto_accelerator_sha3_round_consts {
	/* [0x0]  */
	uint32_t hi;
	/* [0x4]  */
	uint32_t lo;
};

struct al_crypto_accelerator_sha3_params {
	/* [0x0]  */
	uint32_t num_rounds;
	/*
	 * [0x4] sponge function bitrate. This value determines the security level and performance
	 * of the hash function (defined by standard).
	 */
	uint32_t bitrate_sha3_224;
	/*
	 * [0x8] sponge function bitrate. This value determines the security level and performance
	 * of the hash function (defined by standard).
	 */
	uint32_t bitrate_sha3_256;
	/*
	 * [0xc] sponge function bitrate. This value determines the security level and performance
	 * of the hash function (defined by standard).
	 */
	uint32_t bitrate_sha3_384;
	/*
	 * [0x10] sponge function bitrate. This value determines the security level and performance
	 * of the hash function (defined by standard).
	 */
	uint32_t bitrate_sha3_512;
	/*
	 * [0x14] sponge function bitrate. This value determines the security level and performance
	 * of the hash function (defined by standard).
	 */
	uint32_t bitrate_shake_128;
	/*
	 * [0x18] sponge function bitrate. This value determines the security level and performance
	 * of the hash function (defined by standard).
	 */
	uint32_t bitrate_shake_256;
	/*
	 * [0x1c] sponge function bitrate. This value determines the security level and performance
	 * of the hash function (defined by standard).
	 */
	uint32_t bitrate_default;
	/* [0x20]  */
	uint32_t padding_sha3;
	/* [0x24]  */
	uint32_t padding_shake;
	/* [0x28]  */
	uint32_t endianity;
	uint32_t rsrvd;
};

struct al_crypto_accelerator_sha3_debug {
	/* [0x0]  */
	uint32_t fsm_state;
	uint32_t rsrvd[3];
};

struct al_crypto_accelerator_xts_conf {
	/* [0x0] lba increment factor, for multiple concatenated blocks in a single pkt (lsbs) */
	uint32_t lba_inc_0;
	/* [0x4] lba increment factor, for multiple concatenated blocks in a single pkt */
	uint32_t lba_inc_1;
	/* [0x8] lba increment factor, for multiple concatenated blocks in a single pkt */
	uint32_t lba_inc_2;
	/* [0xc] lba increment factor, for multiple concatenated blocks in a single pkt (msbs) */
	uint32_t lba_inc_3;
	/* [0x10] lba alpha constant, for multiple concatenated blocks in a single pkt (lsbs) */
	uint32_t alpha_0;
	/* [0x14] lba alpha constant, for multiple concatenated blocks in a single pkt */
	uint32_t alpha_1;
	/* [0x18] lba alpha constant, for multiple concatenated blocks in a single pkt */
	uint32_t alpha_2;
	/* [0x1c] lba alpha constant, for multiple concatenated blocks in a single pkt (msbs) */
	uint32_t alpha_3;
	/* [0x20] endianity flexibility support */
	uint32_t tweak_calc_swap;
	uint32_t rsrvd[7];
};

struct al_crypto_accelerator_gmac_conf {
	/* [0x0] FSM status */
	uint32_t fsm_status;
	/* [0x4] gmac override configuration */
	uint32_t override;
	/* [0x8] endianity flexibility support */
	uint32_t endianity_swap;
	/* [0xc] data mask polarity */
	uint32_t data_mask_polarity;
};

struct al_crypto_accelerator_gmac_data_mask {
	/* [0x0]  */
	uint32_t id_0;
	/* [0x4]  */
	uint32_t id_1;
	/* [0x8]  */
	uint32_t id_2;
	/* [0xc]  */
	uint32_t id_3;
};

struct al_crypto_accelerator_gmac_perf_counters {
	/* [0x0]  */
	uint32_t configuration;
	/* [0x4]  */
	uint32_t counter;
};

struct al_crypto_accelerator_lz_static_cfg {
	/* [0x0]  */
	uint32_t control;
	/* [0x4]  */
	uint32_t enc_history_status;
	/* [0x8]  */
	uint32_t dec_history_cache_status;
	/* [0xc]  */
	uint32_t dec_history_memory_status;
	/* [0x10]  */
	uint32_t enc_perf_status;
	/* [0x14]  */
	uint32_t dec_perf_status;
	/* [0x18]  */
	uint32_t memory_init;
};

struct al_crypto_accelerator_lz_algorithm {
	/* [0x0]  */
	uint32_t entry_alg;
	/* [0x4]  */
	uint32_t flags_offset;
	/* [0x8]  */
	uint32_t flags_pos_0;
	/* [0xc]  */
	uint32_t flags_pos_1;
	/* [0x10]  */
	uint32_t flags_pos_2;
	/* [0x14]  */
	uint32_t flags_pos_3;
	/* [0x18]  */
	uint32_t flags_cfg;
	/* [0x1c]  */
	uint32_t cmprs_form;
};

struct al_crypto_accelerator_lz_enc_profile {
	/* [0x0]  */
	uint32_t entry_enc_profile;
	/* [0x4]  */
	uint32_t cfg_enc_profile;
	/* [0x8]  */
	uint32_t thresholds_enc_profile;
	/* [0xc]  */
	uint32_t eop_policy_profile;
};

struct al_crypto_accelerator_lz_dec_profile {
	/* [0x0]  */
	uint32_t entry_dec_profile;
	/* [0x4]  */
	uint32_t cfg;
	/* [0x8]  */
	uint32_t alg;
};

struct al_crypto_accelerator_lz_output_arbiter {
	/* [0x0]  */
	uint32_t entry_out_arb;
	/* [0x4]  */
	uint32_t alu_data_0_1;
	/* [0x8]  */
	uint32_t alu_data_2_3;
	/* [0xc]  */
	uint32_t cfg_out_arb;
};

struct al_crypto_accelerator_lz_format {
	/* [0x0]  */
	uint32_t entry_format;
	/* [0x4]  */
	uint32_t offset;
	/* [0x8]  */
	uint32_t alu_symbol_0;
	/* [0xc]  */
	uint32_t alu_symbol_1;
	/* [0x10]  */
	uint32_t alu_length_0;
	/* [0x14]  */
	uint32_t alu_length_1;
	/* [0x18]  */
	uint32_t alu_offset_0;
	/* [0x1c]  */
	uint32_t alu_offset_1;
	/* [0x20]  */
	uint32_t cfg_format;
};

struct al_crypto_accelerator_lz_cam {
	/* [0x0]  */
	uint32_t entry_cam;
	/* [0x4]  */
	uint32_t cfg_cam;
	/* [0x8]  */
	uint32_t threshold_cam;
};

struct al_crypto_accelerator_lz_decoder_history_stat {
	/* [0x0] */
	uint32_t counter_control;
	/* [0x4] Multiple (> 1) access to history domain 4KB - 32KB */
	uint32_t mca_sram;
	/* [0x8] Multiple (> 2) access to history domain 1KB - 4KB */
	uint32_t mca_large_cache;
	/* [0xc] Multiple (> 8) access to history domain 1B - 1KB */
	uint32_t mca_small_cache;
	/* [0x10] Accumulated code words reading more than 64B from history */
	uint32_t lsrh;
	/* [0x14] Single code words reading more than 64B from history */
	uint32_t larh;
	/* [0x18] Number of processed code words bits [63:32] */
	uint32_t code_word_counter_msb;
	/* [0x1c] Number of processed code words bits [31:0] */
	uint32_t code_word_counter_lsb;
};

struct al_crypto_accelerator_huffman_static_table {
	/* [0x0]  */
	uint32_t entry;
	/* [0x4]  */
	uint32_t value;
	/* [0x8]  */
	uint32_t alphabet;
};

struct al_crypto_accelerator_comp_to_enc_sub_block_size {
	/* [0x0]  */
	uint32_t pad_0;
	/* [0x4]  */
	uint32_t pad_1;
	/* [0x8]  */
	uint32_t pad_2;
	/* [0xc]  */
	uint32_t pad_3;
};

struct al_crypto_accelerator_inline_crc_cfg {
	/* [0x0] reserved register for ECOs, with default value 0 */
	uint32_t icrc_reserved_0;
	/* [0x4] reserved register for ECOs, with default value 1 */
	uint32_t icrc_reserved_1;
	/* [0x8]  */
	uint32_t icrc_profile_table_idx;
	uint32_t rsrvd[13];
};

struct al_crypto_accelerator_inline_crc_start_mask {
	/* [0x0] mask lsbs */
	uint32_t c_mask_0;
	/* [0x4]  */
	uint32_t c_mask_1;
	/* [0x8]  */
	uint32_t c_mask_2;
	/* [0xc]  */
	uint32_t c_mask_3;
};

struct al_crypto_accelerator_crc_profile_table {
	/* [0x0] profile table address */
	uint32_t crc_addr;
	/* [0x4] profile table data to read/write */
	uint32_t crc_data;
	uint32_t rsrvd[6];
};

struct al_crypto_accelerator_spare {
	/* [0x0]  */
	uint32_t generate_1;
	/* [0x4]  */
	uint32_t generate_2;
	/* [0x8]  */
	uint32_t generate_3;
	/* [0xc]  */
	uint32_t generate_4;
	/* [0x10]  */
	uint32_t transmit_1;
	/* [0x14] */
	uint32_t transmit_2;
	/* [0x18] */
	uint32_t transmit_3;
	/* [0x1c] */
	uint32_t transmit_4;
	/* [0x20] */
	uint32_t parse_1;
	/* [0x24] */
	uint32_t parse_2;
	/* [0x28] */
	uint32_t parse_3;
	/* [0x2c] */
	uint32_t parse_4;
	/* [0x30] */
	uint32_t extract_1;
	/* [0x34] */
	uint32_t extract_2;
	/* [0x38] */
	uint32_t extract_3;
	/* [0x3c] */
	uint32_t extract_4;
	/* [0x40] */
	uint32_t huffman_1;
	/* [0x44] */
	uint32_t huffman_2;
	/* [0x48] */
	uint32_t huffman_3;
	/* [0x4c] */
	uint32_t huffman_4;
	/* [0x50] */
	uint32_t ssmae_1;
	/* [0x54] */
	uint32_t ssmae_2;
	/* [0x58] */
	uint32_t ssmae_3;
	/* [0x5c] */
	uint32_t ssmae_4;
};

struct crypto_regs {
	uint32_t rsrvd_0[192];
	struct al_crypto_accelerator_configuration configuration; /* [0x300] */
	struct al_crypto_accelerator_debug debug;               /* [0x310] */
	struct al_crypto_accelerator_log log;                   /* [0x320] */
	struct al_crypto_accelerator_crypto_perf_counter crypto_perf_counter; /* [0x340] */
	struct al_crypto_accelerator_perfm_cnt_cntl perfm_cnt_cntl; /* [0x390] */
	struct al_crypto_accelerator_crypto_status crypto_status; /* [0x394] */
	struct al_crypto_accelerator_crypto_version crypto_version; /* [0x398] */
	struct al_crypto_accelerator_crypto_algorithms crypto_algorithms; /* [0x3a0] */
	uint32_t rsrvd_1[18];
	struct al_crypto_accelerator_reserved reserved;         /* [0x3f0] */
	struct al_crypto_accelerator_sha3_round_consts sha3_round_consts[64]; /* [0x400] */
	struct al_crypto_accelerator_sha3_params sha3_params;   /* [0x600] */
	struct al_crypto_accelerator_sha3_debug sha3_debug;     /* [0x630] */
	struct al_crypto_accelerator_xts_conf xts_conf;         /* [0x640] */
	struct al_crypto_accelerator_gmac_conf gmac_conf;       /* [0x680] */
	struct al_crypto_accelerator_gmac_data_mask gmac_data_mask[16]; /* [0x690] */
	struct al_crypto_accelerator_gmac_perf_counters gmac_perf_counters; /* [0x790] */
	uint32_t rsrvd_2[26];
	struct al_crypto_accelerator_lz_static_cfg lz_static_cfg; /* [0x800] */
	uint32_t rsrvd_3;
	struct al_crypto_accelerator_lz_algorithm lz_algorithm; /* [0x820] */
	struct al_crypto_accelerator_lz_enc_profile lz_enc_profile; /* [0x840] */
	struct al_crypto_accelerator_lz_dec_profile lz_dec_profile; /* [0x850] */
	uint32_t rsrvd_4;
	struct al_crypto_accelerator_lz_output_arbiter lz_output_arbiter; /* [0x860] */
	struct al_crypto_accelerator_lz_format lz_format;       /* [0x870] */
	uint32_t rsrvd_5[3];
	struct al_crypto_accelerator_lz_cam lz_cam;             /* [0x8a0] */
	uint32_t rsrvd_6;
	struct al_crypto_accelerator_lz_decoder_history_stat lz_decoder_history_stat; /* [0x8b0] */
	uint32_t rsrvd_7[12];
	struct al_crypto_accelerator_huffman_static_table huffman_static_table; /* [0x900] */
	uint32_t rsrvd_8[5];
struct al_crypto_accelerator_comp_to_enc_sub_block_size comp_to_enc_sub_block_size;
/* [0x920] */
	uint32_t rsrvd_9[436];
	struct al_crypto_accelerator_inline_crc_cfg inline_crc_cfg; /* [0x1000] */
	struct al_crypto_accelerator_inline_crc_start_mask inline_crc_start_mask[4]; /* [0x1040] */
	struct al_crypto_accelerator_crc_profile_table crc_profile_table; /* [0x1080] */
	uint32_t rsrvd_10[4056];
	struct al_crypto_accelerator_spare spare;               /* [0x5000] */
};


/*
 * Registers Fields
 */

/**** unit_conf register ****/
/*
 * When this bit is set to 1, the Crypto engine accepts new commands, if possible before previous
 * command completion.
 */
#define CRYPTO_CONFIGURATION_UNIT_CONF_MUL_CMD_EN (1 << 0)
/* When this bit is set to 1, when an error occurs, the pipe will hold. */
#define CRYPTO_CONFIGURATION_UNIT_CONF_HOLD_PIPE_WHEN_ERROR (1 << 1)
/*
 * Enable smart clk-gating of unused Crypto engines.
 * Bits 14:11 enabling/disabling of smart clk gating in COMP/AES/SHA/DES respectively, e.g., for
 * smart clk gating in AES & DES, but no clk gating in SHA, set this field to 'b0101.
 */
#define CRYPTO_CONFIGURATION_UNIT_CONF_CLK_GATE_UNUSED_ENGINES_MASK 0x0000003C
#define CRYPTO_CONFIGURATION_UNIT_CONF_CLK_GATE_UNUSED_ENGINES_SHIFT 2
/* [9:6] Disable clk to COMP/AES/SHA/DES core (respectively). */
#define CRYPTO_CONFIGURATION_UNIT_CONF_CLK_SHUTDOWN_MASK 0x000003C0
#define CRYPTO_CONFIGURATION_UNIT_CONF_CLK_SHUTDOWN_SHIFT 6
/*
 * 0x0 - RR from VF #0 to VF #(N-1).
 * 0x1 - RR from VF #(N-1) to VF #0.
 * 0x2 - Strict Priority according to vf_arb_prio
 */
#define CRYPTO_CONFIGURATION_UNIT_CONF_VF_ARB_MODE_MASK 0x00000C00
#define CRYPTO_CONFIGURATION_UNIT_CONF_VF_ARB_MODE_SHIFT 10
/* Index of prioritized VF (RR will start from this VF). */
#define CRYPTO_CONFIGURATION_UNIT_CONF_VF_ARB_PRIO_MASK 0x00007000
#define CRYPTO_CONFIGURATION_UNIT_CONF_VF_ARB_PRIO_SHIFT 12
/* Enable key parity check for DES */
#define CRYPTO_CONFIGURATION_UNIT_CONF_DES_KEY_PARITY_CHECK_EN (1 << 15)
/* Enable DES EEE mode */
#define CRYPTO_CONFIGURATION_UNIT_CONF_CHICKEN_DES_EEE_EN (1 << 16)
/* Enable Auth to start together with Enc */
#define CRYPTO_CONFIGURATION_UNIT_CONF_CHICKEN_AUTH_START_WITH_ENC_EN (1 << 17)
/* Enable SHA-3 engine */
#define CRYPTO_CONFIGURATION_UNIT_CONF_CHICKEN_SHA3_EN (1 << 18)
/* Enable long SAs (useful for modes requiring HMAC/broken-pkt while using SHA-3 algorithm) */
#define CRYPTO_CONFIGURATION_UNIT_CONF_CHICKEN_LONG_SA_EN (1 << 19)
/* Disable HW padding of incoming pkts */
#define CRYPTO_CONFIGURATION_UNIT_CONF_CHICKEN_SHA_PADDING_DISABLE (1 << 20)
/* give each GDMA ability to access all SAD lines */
#define CRYPTO_CONFIGURATION_UNIT_CONF_GDMA_FULL_SAD_ACCESS_EN (1 << 21)

/**** mem_test register ****/
/* 0xA enables test mode */
#define CRYPTO_CONFIGURATION_MEM_TEST_MASK	0x0000000F
#define CRYPTO_CONFIGURATION_MEM_TEST_SHIFT	0
#define CRYPTO_CONFIGURATION_MEM_TEST_VAL_EN	\
	(0xa << CRYPTO_CONFIGURATION_MEM_TEST_SHIFT)
#define CRYPTO_CONFIGURATION_MEM_TEST_VAL_DIS	\
	(0x0 << CRYPTO_CONFIGURATION_MEM_TEST_SHIFT)

/**** unit_debug_conf register ****/
/* When these bits are set to 0, reset the ack FIFOs. */
#define CRYPTO_DEBUG_UNIT_DEBUG_CONF_FIFO_ENABLE_ACK_MASK 0x0000000F
#define CRYPTO_DEBUG_UNIT_DEBUG_CONF_FIFO_ENABLE_ACK_SHIFT 0
/* When this bit is set to 0, reset the payload FIFO between the engines and the dispatcher. */
#define CRYPTO_DEBUG_UNIT_DEBUG_CONF_FIFO_ENABLE_E2M_PYLD (1 << 4)
/* When this bit is set to 0, reset the sign FIFO between the engines and the dispatcher. */
#define CRYPTO_DEBUG_UNIT_DEBUG_CONF_FIFO_ENABLE_E2M_SIGN (1 << 5)
/* When this bit is set to 0, reset the intr FIFO between the engines and the dispatcher. */
#define CRYPTO_DEBUG_UNIT_DEBUG_CONF_FIFO_ENABLE_E2M_INTR (1 << 6)
/* When this bit is set to 0, reset the data FIFO between the decoder and the engines. */
#define CRYPTO_DEBUG_UNIT_DEBUG_CONF_FIFO_ENABLE_D2E_DATA (1 << 7)
/* When this bit is set to 0, reset the control FIFO between the decoder and the engines. */
#define CRYPTO_DEBUG_UNIT_DEBUG_CONF_FIFO_ENABLE_D2E_CTRL (1 << 8)
/* When this bit is set to 0, reset the context FIFO between the decoder and the engines. */
#define CRYPTO_DEBUG_UNIT_DEBUG_CONF_FIFO_ENABLE_D2E_CONTEXT (1 << 9)
/* When this bit is set to 0, reset the SA bypass FIFO between the decoder and the dispatcher. */
#define CRYPTO_DEBUG_UNIT_DEBUG_CONF_FIFO_ENABLE_D2M_SA_BYPASS (1 << 10)
/* When this bit is set to 0, reset the IV bypass FIFO between the decoder and the dispatcher. */
#define CRYPTO_DEBUG_UNIT_DEBUG_CONF_FIFO_ENABLE_D2M_IV_BYPASS (1 << 11)
/* When this bit is set to 0, reset the ICV bypass FIFO between the decoder and the dispatcher. */
#define CRYPTO_DEBUG_UNIT_DEBUG_CONF_FIFO_ENABLE_D2M_ICV_BYPASS (1 << 12)
/* When this bit is set to 0, reset the aligner FIFO. */
#define CRYPTO_DEBUG_UNIT_DEBUG_CONF_FIFO_ENABLE_ALIGNER_AXIS (1 << 13)
/* When this bit is set to 0, reset the ingress FIFO. */
#define CRYPTO_DEBUG_UNIT_DEBUG_CONF_FIFO_ENABLE_INGRESS (1 << 14)
/* Clear the data_aligner (decode stage). */
#define CRYPTO_DEBUG_UNIT_DEBUG_CONF_CLR_ALIGNER (1 << 15)
/* Clear the data_splitter (dispatch stage). */
#define CRYPTO_DEBUG_UNIT_DEBUG_CONF_CLR_SPLITTER (1 << 16)
/* Don't perform memory init. */
#define CRYPTO_DEBUG_UNIT_DEBUG_CONF_MEM_INIT_SKIP (1 << 17)

/**** trans_info_1 register ****/
/* Transaction length in bytes */
#define CRYPTO_LOG_TRANS_INFO_1_TRANS_LEN_MASK 0x000FFFFF
#define CRYPTO_LOG_TRANS_INFO_1_TRANS_LEN_SHIFT 0
/* Number of descriptors in the transaction */
#define CRYPTO_LOG_TRANS_INFO_1_NUM_OF_DESC_MASK 0x00F00000
#define CRYPTO_LOG_TRANS_INFO_1_NUM_OF_DESC_SHIFT 20
/* Reserved */
#define CRYPTO_LOG_TRANS_INFO_1_RESERVED_MASK 0xFF000000
#define CRYPTO_LOG_TRANS_INFO_1_RESERVED_SHIFT 24

/**** trans_info_2 register ****/
/* Queue Number of the transaction */
#define CRYPTO_LOG_TRANS_INFO_2_Q_NUM_MASK 0x00000FFF
#define CRYPTO_LOG_TRANS_INFO_2_Q_NUM_SHIFT 0
/* GDMA ID of the transaction */
#define CRYPTO_LOG_TRANS_INFO_2_DMA_ID_MASK 0x0000F000
#define CRYPTO_LOG_TRANS_INFO_2_DMA_ID_SHIFT 12
/* Internal Serial Number of the transaction */
#define CRYPTO_LOG_TRANS_INFO_2_SERIAL_NUM_MASK 0x03FF0000
#define CRYPTO_LOG_TRANS_INFO_2_SERIAL_NUM_SHIFT 16
/* Reserved */
#define CRYPTO_LOG_TRANS_INFO_2_RESERVED_MASK 0xFC000000
#define CRYPTO_LOG_TRANS_INFO_2_RESERVED_SHIFT 26

/**** conf register ****/
/*
 * Does not affect the recover_err_cnt
 * 0 - Clear performance counter
 * 1 - Stop performance counter
 * 2 - Active performance counter
 */
#define CRYPTO_PERFM_CNT_CNTL_CONF_CONT_PERFORM_MASK 0x00000003
#define CRYPTO_PERFM_CNT_CNTL_CONF_CONT_PERFORM_SHIFT 0

/**** status register ****/
/* Indicates when CRYPTO is empty */
#define CRYPTO_CRYPTO_STATUS_STATUS_CRYPTO_EMPTY (1 << 0)
/* Internal state of decoder */
#define CRYPTO_CRYPTO_STATUS_STATUS_CRYPTO_DEC_STATE_MASK 0x0000001E
#define CRYPTO_CRYPTO_STATUS_STATUS_CRYPTO_DEC_STATE_SHIFT 1
/* Internal state of dispatcher */
#define CRYPTO_CRYPTO_STATUS_STATUS_CRYPTO_DISPATCHER_STATE_MASK 0x000001E0
#define CRYPTO_CRYPTO_STATUS_STATUS_CRYPTO_DISPATCHER_STATE_SHIFT 5
/* Reserved */
#define CRYPTO_CRYPTO_STATUS_STATUS_RESERVED_MASK 0xFFFFFE00
#define CRYPTO_CRYPTO_STATUS_STATUS_RESERVED_SHIFT 9

/**** revision register ****/
/* 0x0 for M0, 0x1 for A0 */
#define CRYPTO_CRYPTO_VERSION_REVISION_REV_ID_MASK 0x0000FFFF
#define CRYPTO_CRYPTO_VERSION_REVISION_REV_ID_SHIFT 0
/* Annapurna Labs PCI vendor ID */
#define CRYPTO_CRYPTO_VERSION_REVISION_VENDOR_ID_MASK 0xFFFF0000
#define CRYPTO_CRYPTO_VERSION_REVISION_VENDOR_ID_SHIFT 16

/**** date register ****/
/* Date of release */
#define CRYPTO_CRYPTO_VERSION_DATE_DATE_DAY_MASK 0x0000001F
#define CRYPTO_CRYPTO_VERSION_DATE_DATE_DAY_SHIFT 0
/* Date of release */
#define CRYPTO_CRYPTO_VERSION_DATE_RESERVED_7_5_MASK 0x000000E0
#define CRYPTO_CRYPTO_VERSION_DATE_RESERVED_7_5_SHIFT 5
/* Month of release */
#define CRYPTO_CRYPTO_VERSION_DATE_DATE_MONTH_MASK 0x00000F00
#define CRYPTO_CRYPTO_VERSION_DATE_DATE_MONTH_SHIFT 8
/* Date of release */
#define CRYPTO_CRYPTO_VERSION_DATE_RESERVED_15_12_MASK 0x0000F000
#define CRYPTO_CRYPTO_VERSION_DATE_RESERVED_15_12_SHIFT 12
/* Month of release */
#define CRYPTO_CRYPTO_VERSION_DATE_DATE_YEAR_MASK 0x00FF0000
#define CRYPTO_CRYPTO_VERSION_DATE_DATE_YEAR_SHIFT 16
/* Date of release */
#define CRYPTO_CRYPTO_VERSION_DATE_RESERVED_31_24_MASK 0xFF000000
#define CRYPTO_CRYPTO_VERSION_DATE_RESERVED_31_24_SHIFT 24

/**** encryption register ****/
/* AES ECB */
#define CRYPTO_CRYPTO_ALGORITHMS_ENCRYPTION_AES_ECB (1 << 0)
/* AES CBC */
#define CRYPTO_CRYPTO_ALGORITHMS_ENCRYPTION_AES_CBC (1 << 1)
/* AES_CTR */
#define CRYPTO_CRYPTO_ALGORITHMS_ENCRYPTION_AES_CTR (1 << 2)
/* AES CFB */
#define CRYPTO_CRYPTO_ALGORITHMS_ENCRYPTION_AES_CFB (1 << 3)
/* AES OFB */
#define CRYPTO_CRYPTO_ALGORITHMS_ENCRYPTION_AES_OFB (1 << 4)
/* AES GCM */
#define CRYPTO_CRYPTO_ALGORITHMS_ENCRYPTION_AES_GCM (1 << 5)
/* AES CCM */
#define CRYPTO_CRYPTO_ALGORITHMS_ENCRYPTION_AES_CCM (1 << 6)
/* AES XTS */
#define CRYPTO_CRYPTO_ALGORITHMS_ENCRYPTION_AES_XTS (1 << 7)
/* SHA3_DEFAULT_PARAMS */
#define CRYPTO_CRYPTO_ALGORITHMS_ENCRYPTION_SHA_3_DEFAULT (1 << 8)
/* SHA3_224 */
#define CRYPTO_CRYPTO_ALGORITHMS_ENCRYPTION_SHA_3_224 (1 << 9)
/* SHA3_256 */
#define CRYPTO_CRYPTO_ALGORITHMS_ENCRYPTION_SHA_3_256 (1 << 10)
/* SHA3_384 */
#define CRYPTO_CRYPTO_ALGORITHMS_ENCRYPTION_SHA_3_384 (1 << 11)
/* SHA3_512 */
#define CRYPTO_CRYPTO_ALGORITHMS_ENCRYPTION_SHA_3_512 (1 << 12)
/* SHA-1 */
#define CRYPTO_CRYPTO_ALGORITHMS_ENCRYPTION_SHA_1 (1 << 13)
/* SHA2_256 */
#define CRYPTO_CRYPTO_ALGORITHMS_ENCRYPTION_SHA_2_256 (1 << 14)
/* SHA2_384 */
#define CRYPTO_CRYPTO_ALGORITHMS_ENCRYPTION_SHA_2_384 (1 << 15)
/* SHA2_512 */
#define CRYPTO_CRYPTO_ALGORITHMS_ENCRYPTION_SHA_2_512 (1 << 16)
/* MD-5 */
#define CRYPTO_CRYPTO_ALGORITHMS_ENCRYPTION_MD_5 (1 << 17)
/* SHAKE_128 */
#define CRYPTO_CRYPTO_ALGORITHMS_ENCRYPTION_SHAKE_128 (1 << 18)
/* SHAKE_256 */
#define CRYPTO_CRYPTO_ALGORITHMS_ENCRYPTION_SHAKE_256 (1 << 19)
/* Reserved */
#define CRYPTO_CRYPTO_ALGORITHMS_ENCRYPTION_RESERVED (1 << 20)
/* AES XTS CipherText Stealing */
#define CRYPTO_CRYPTO_ALGORITHMS_ENCRYPTION_AES_XTS_CTS (1 << 21)
/* AES ECB CipherText Stealing */
#define CRYPTO_CRYPTO_ALGORITHMS_ENCRYPTION_AES_ECB_CTS (1 << 22)
/* AES CBC CipherText Stealing */
#define CRYPTO_CRYPTO_ALGORITHMS_ENCRYPTION_AES_CBC_CTS (1 << 23)
/* DES ECB */
#define CRYPTO_CRYPTO_ALGORITHMS_ENCRYPTION_DES_ECB (1 << 24)
/* DES CBC */
#define CRYPTO_CRYPTO_ALGORITHMS_ENCRYPTION_DES_CBC (1 << 25)
/* DES3 ECB */
#define CRYPTO_CRYPTO_ALGORITHMS_ENCRYPTION_DES3_ECB (1 << 26)
/* DES3 CBC */
#define CRYPTO_CRYPTO_ALGORITHMS_ENCRYPTION_DES3CBC (1 << 27)
/* Reserved */
#define CRYPTO_CRYPTO_ALGORITHMS_ENCRYPTION_RESERVED_31_28_MASK 0xF0000000
#define CRYPTO_CRYPTO_ALGORITHMS_ENCRYPTION_RESERVED_31_28_SHIFT 28

/**** compression register ****/
/* LZ77 Compression */
#define CRYPTO_CRYPTO_ALGORITHMS_COMPRESSION_LZ77_ENC (1 << 0)
/* LZ77 Decompression */
#define CRYPTO_CRYPTO_ALGORITHMS_COMPRESSION_LZ77_DEC (1 << 1)
/* LZSS Compression */
#define CRYPTO_CRYPTO_ALGORITHMS_COMPRESSION_LZSS_ENC (1 << 2)
/* LZSS Decompression */
#define CRYPTO_CRYPTO_ALGORITHMS_COMPRESSION_LZSS_DEC (1 << 3)
/* LZ4 Compression */
#define CRYPTO_CRYPTO_ALGORITHMS_COMPRESSION_LZ4_ENC (1 << 4)
/* LZ4 Decompression */
#define CRYPTO_CRYPTO_ALGORITHMS_COMPRESSION_LZ4_DEC (1 << 5)
/* Huffman Static Compression */
#define CRYPTO_CRYPTO_ALGORITHMS_COMPRESSION_HUFFMAN_STAT_ENC (1 << 6)
/* Huffman Static Decompression */
#define CRYPTO_CRYPTO_ALGORITHMS_COMPRESSION_HUFFMAN_STAT_DEC (1 << 7)
/* Huffman Dynamic Compression */
#define CRYPTO_CRYPTO_ALGORITHMS_COMPRESSION_HUFFMAN_DYN_ENC (1 << 8)
/* Huffman Dynamic Decompression */
#define CRYPTO_CRYPTO_ALGORITHMS_COMPRESSION_HUFFMAN_DYN_DEC (1 << 9)
/* Deflate Static Compression */
#define CRYPTO_CRYPTO_ALGORITHMS_COMPRESSION_DEFLATE_STAT_ENC (1 << 10)
/* Deflate Static Decompression */
#define CRYPTO_CRYPTO_ALGORITHMS_COMPRESSION_DEFLATE_STAT_DEC (1 << 11)
/* Deflate Dynamic Compression */
#define CRYPTO_CRYPTO_ALGORITHMS_COMPRESSION_DEFLATE_DYN_ENC (1 << 12)
/* Deflate Dynamic Decompression */
#define CRYPTO_CRYPTO_ALGORITHMS_COMPRESSION_DEFLATE_DYN_DEC (1 << 13)
/* LZO Compression */
#define CRYPTO_CRYPTO_ALGORITHMS_COMPRESSION_LZO_ENC (1 << 14)
/* LZO Decompression */
#define CRYPTO_CRYPTO_ALGORITHMS_COMPRESSION_LZO_DEC (1 << 15)
/* Snappy Compression */
#define CRYPTO_CRYPTO_ALGORITHMS_COMPRESSION_SNAPPY_ENC (1 << 16)
/* Snappy Decompression */
#define CRYPTO_CRYPTO_ALGORITHMS_COMPRESSION_SNAPPY_DEC (1 << 17)
/* Reserved */
#define CRYPTO_CRYPTO_ALGORITHMS_COMPRESSION_RESERVED_31_18_MASK 0xFFFC0000
#define CRYPTO_CRYPTO_ALGORITHMS_COMPRESSION_RESERVED_31_18_SHIFT 18

/**** num_rounds register ****/
/* number of iterations in SHA3 calculation */
#define CRYPTO_SHA3_PARAMS_NUM_ROUNDS_VAL_MASK 0x0000003F
#define CRYPTO_SHA3_PARAMS_NUM_ROUNDS_VAL_SHIFT 0

/**** bitrate_sha3_224 register ****/
/*
 * bitrate value in bytes. value must be 0-200.
 * change from default value with care (value is defined by standard)
 */
#define CRYPTO_SHA3_PARAMS_BITRATE_SHA3_224_VAL_MASK 0x000000FF
#define CRYPTO_SHA3_PARAMS_BITRATE_SHA3_224_VAL_SHIFT 0

/**** bitrate_sha3_256 register ****/
/*
 * bitrate value in bytes. value must be 0-200.
 * change from default value with care (value is defined by standard)
 */
#define CRYPTO_SHA3_PARAMS_BITRATE_SHA3_256_VAL_MASK 0x000000FF
#define CRYPTO_SHA3_PARAMS_BITRATE_SHA3_256_VAL_SHIFT 0

/**** bitrate_sha3_384 register ****/
/*
 * bitrate value in bytes. value must be 0-200.
 * change from default value with care (value is defined by standard)
 */
#define CRYPTO_SHA3_PARAMS_BITRATE_SHA3_384_VAL_MASK 0x000000FF
#define CRYPTO_SHA3_PARAMS_BITRATE_SHA3_384_VAL_SHIFT 0

/**** bitrate_sha3_512 register ****/
/*
 * bitrate value in bytes. value must be 0-200.
 * change from default value with care (value is defined by standard)
 */
#define CRYPTO_SHA3_PARAMS_BITRATE_SHA3_512_VAL_MASK 0x000000FF
#define CRYPTO_SHA3_PARAMS_BITRATE_SHA3_512_VAL_SHIFT 0

/**** bitrate_shake_128 register ****/
/*
 * bitrate value in bytes. value must be 0-200.
 * change from default value with care (value is defined by standard)
 */
#define CRYPTO_SHA3_PARAMS_BITRATE_SHAKE_128_VAL_MASK 0x000000FF
#define CRYPTO_SHA3_PARAMS_BITRATE_SHAKE_128_VAL_SHIFT 0

/**** bitrate_shake_256 register ****/
/*
 * bitrate value in bytes. value must be 0-200.
 * change from default value with care (value is defined by standard)
 */
#define CRYPTO_SHA3_PARAMS_BITRATE_SHAKE_256_VAL_MASK 0x000000FF
#define CRYPTO_SHA3_PARAMS_BITRATE_SHAKE_256_VAL_SHIFT 0

/**** bitrate_default register ****/
/*
 * bitrate value in bytes. value must be 0-200.
 * change from default value with care (value is defined by standard)
 */
#define CRYPTO_SHA3_PARAMS_BITRATE_DEFAULT_VAL_MASK 0x000000FF
#define CRYPTO_SHA3_PARAMS_BITRATE_DEFAULT_VAL_SHIFT 0

/**** padding_sha3 register ****/
/* padding on first byte after payload */
#define CRYPTO_SHA3_PARAMS_PADDING_SHA3_START_MASK 0x000000FF
#define CRYPTO_SHA3_PARAMS_PADDING_SHA3_START_SHIFT 0
/* padding on bytes which are not first after payload, or last in word */
#define CRYPTO_SHA3_PARAMS_PADDING_SHA3_MID_MASK 0x0000FF00
#define CRYPTO_SHA3_PARAMS_PADDING_SHA3_MID_SHIFT 8
/* padding on last byte in word */
#define CRYPTO_SHA3_PARAMS_PADDING_SHA3_END_MASK 0x00FF0000
#define CRYPTO_SHA3_PARAMS_PADDING_SHA3_END_SHIFT 16
/* single byte padding value */
#define CRYPTO_SHA3_PARAMS_PADDING_SHA3_START_END_MASK 0xFF000000
#define CRYPTO_SHA3_PARAMS_PADDING_SHA3_START_END_SHIFT 24

/**** padding_shake register ****/
/* padding on first byte after payload */
#define CRYPTO_SHA3_PARAMS_PADDING_SHAKE_START_MASK 0x000000FF
#define CRYPTO_SHA3_PARAMS_PADDING_SHAKE_START_SHIFT 0
/* padding on bytes which are not first after payload, or last in word */
#define CRYPTO_SHA3_PARAMS_PADDING_SHAKE_MID_MASK 0x0000FF00
#define CRYPTO_SHA3_PARAMS_PADDING_SHAKE_MID_SHIFT 8
/* padding on last byte in word */
#define CRYPTO_SHA3_PARAMS_PADDING_SHAKE_END_MASK 0x00FF0000
#define CRYPTO_SHA3_PARAMS_PADDING_SHAKE_END_SHIFT 16
/* single byte padding value */
#define CRYPTO_SHA3_PARAMS_PADDING_SHAKE_START_END_MASK 0xFF000000
#define CRYPTO_SHA3_PARAMS_PADDING_SHAKE_START_END_SHIFT 24

/**** endianity register ****/
/* when asserted, sha3 performs bit-swap for every byte on the input data */
#define CRYPTO_SHA3_PARAMS_ENDIANITY_BIT_SWAP_IN (1 << 0)
/* when asserted, sha3 performs bit-swap for every byte on the output data */
#define CRYPTO_SHA3_PARAMS_ENDIANITY_BIT_SWAP_OUT (1 << 1)
/* take digests from state's msbs */
#define CRYPTO_SHA3_PARAMS_ENDIANITY_DIGEST_FROM_MSBS (1 << 2)

/**** fsm_state register ****/
/* current state of preprocessor block (in charge of padding and serial->parallel abosrption */
#define CRYPTO_SHA3_DEBUG_FSM_STATE_PREPROC_MASK 0x0000000F
#define CRYPTO_SHA3_DEBUG_FSM_STATE_PREPROC_SHIFT 0
/* current state of sha3 engine */
#define CRYPTO_SHA3_DEBUG_FSM_STATE_ENGINE_MASK 0x000000F0
#define CRYPTO_SHA3_DEBUG_FSM_STATE_ENGINE_SHIFT 4
/* current state of sha3 engine */
#define CRYPTO_SHA3_DEBUG_FSM_STATE_PERM_MASK 0x00000F00
#define CRYPTO_SHA3_DEBUG_FSM_STATE_PERM_SHIFT 8
/* current round counter value */
#define CRYPTO_SHA3_DEBUG_FSM_STATE_ROUND_CNTR_MASK 0x0003F000
#define CRYPTO_SHA3_DEBUG_FSM_STATE_ROUND_CNTR_SHIFT 12

/**** tweak_calc_swap register ****/

#define CRYPTO_XTS_CONF_TWEAK_CALC_SWAP_PRE_INC_BITS (1 << 0)

#define CRYPTO_XTS_CONF_TWEAK_CALC_SWAP_PRE_INC_BYTES (1 << 1)

#define CRYPTO_XTS_CONF_TWEAK_CALC_SWAP_PRE_INC_WORDS (1 << 2)

#define CRYPTO_XTS_CONF_TWEAK_CALC_SWAP_POST_INC_BITS (1 << 3)

#define CRYPTO_XTS_CONF_TWEAK_CALC_SWAP_POST_INC_BYTES (1 << 4)

#define CRYPTO_XTS_CONF_TWEAK_CALC_SWAP_POST_INC_WORDS (1 << 5)

#define CRYPTO_XTS_CONF_TWEAK_CALC_SWAP_RESERVED_31_6_MASK 0xFFFFFFC0
#define CRYPTO_XTS_CONF_TWEAK_CALC_SWAP_RESERVED_31_6_SHIFT 6

/**** fsm_status register ****/

#define CRYPTO_GMAC_CONF_FSM_STATUS_FSM_STATE_MASK 0x00000007
#define CRYPTO_GMAC_CONF_FSM_STATUS_FSM_STATE_SHIFT 0

#define CRYPTO_GMAC_CONF_FSM_STATUS_RESERVED_31_3_MASK 0xFFFFFFF8
#define CRYPTO_GMAC_CONF_FSM_STATUS_RESERVED_31_3_SHIFT 3

/**** override register ****/
/*
 * Chicken bit that disables parallel operation of gmac. Allows more reliable work with high latency
 * of minimum 16 cycles per packet
 */
#define CRYPTO_GMAC_CONF_OVERRIDE_DO_AUTH_SERIAL (1 << 0)
/* Override profile table to enforce gmac calculation */
#define CRYPTO_GMAC_CONF_OVERRIDE_AUTH_OVERRIDE_EN (1 << 1)
/* Override profile table to disable gmac calculation */
#define CRYPTO_GMAC_CONF_OVERRIDE_AUTH_OVERRIDE_DIS (1 << 2)
/* Override profile table to determine gmac operation direction (generation or validation) */
#define CRYPTO_GMAC_CONF_OVERRIDE_VAL_GEN_N_OVERRID_EN (1 << 3)
/* 0 - Signature generation. 1 - Signature validation. Only relevant if override bit is enabled */
#define CRYPTO_GMAC_CONF_OVERRIDE_VAL_GEN_N (1 << 4)
/* Override profile table to determine gmac authentication mode */
#define CRYPTO_GMAC_CONF_OVERRIDE_AUTH_MODE_OVERRIDE (1 << 5)
/* 0 - GMAC. 1:3 - Reserved. Only relevant if override bit is enabled */
#define CRYPTO_GMAC_CONF_OVERRIDE_AUTH_MODE_MASK 0x000000C0
#define CRYPTO_GMAC_CONF_OVERRIDE_AUTH_MODE_SHIFT 6
/* Override profile table to determine aes key type */
#define CRYPTO_GMAC_CONF_OVERRIDE_KEY_TYPE_OVERRIDE (1 << 8)
/*
 * 0 - 128-bit key. 1 -192-bit key. 2 - 256-bit key. 3 - Reserved. Only relevant if override bit is
 * enabled
 */
#define CRYPTO_GMAC_CONF_OVERRIDE_KEY_TYPE_MASK 0x00000600
#define CRYPTO_GMAC_CONF_OVERRIDE_KEY_TYPE_SHIFT 9
/* Override profile table to determine signature size */
#define CRYPTO_GMAC_CONF_OVERRIDE_SIGN_SIZE_OVERRIDE (1 << 11)
/* Sign size is 4*(N+1) bytes. Only lower 2 bits are used. */
#define CRYPTO_GMAC_CONF_OVERRIDE_SIGN_SIZE_MASK 0x0000F000
#define CRYPTO_GMAC_CONF_OVERRIDE_SIGN_SIZE_SHIFT 12
/* Override profile table to determine signature digest polarity */
#define CRYPTO_GMAC_CONF_OVERRIDE_DIGEST_FROM_MSB_OVERRIDE (1 << 16)
/* 0 - Digest from LSB. 1 - Digest from MSB. Only relevant if overrid bit is enabled */
#define CRYPTO_GMAC_CONF_OVERRIDE_DIGEST_FROM_MSB (1 << 17)

#define CRYPTO_GMAC_CONF_OVERRIDE_RESERVED_31_18_MASK 0xFFFC0000
#define CRYPTO_GMAC_CONF_OVERRIDE_RESERVED_31_18_SHIFT 18

/**** endianity_swap register ****/
/* bit[0] - bit swap bit[1] - byte swap bit[2] - word swap bit[3] - aes beat swap (128 bit) */
#define CRYPTO_GMAC_CONF_ENDIANITY_SWAP_INGRESS_MASK 0x0000000F
#define CRYPTO_GMAC_CONF_ENDIANITY_SWAP_INGRESS_SHIFT 0
/* bit[0] - bit swap bit[1] - byte swap bit[2] - word swap bit[3] - aes beat swap (128 bit) */
#define CRYPTO_GMAC_CONF_ENDIANITY_SWAP_EGRESS_MASK 0x000000F0
#define CRYPTO_GMAC_CONF_ENDIANITY_SWAP_EGRESS_SHIFT 4
/* bit[0] - bit swap bit[1] - byte swap bit[2] - word swap bit[3] - aes beat swap (128 bit) */
#define CRYPTO_GMAC_CONF_ENDIANITY_SWAP_AES_KEY_MASK 0x00000F00
#define CRYPTO_GMAC_CONF_ENDIANITY_SWAP_AES_KEY_SHIFT 8
/* bit[0] - bit swap bit[1] - byte swap bit[2] - word swap */
#define CRYPTO_GMAC_CONF_ENDIANITY_SWAP_H_SUBKEY_MASK 0x00007000
#define CRYPTO_GMAC_CONF_ENDIANITY_SWAP_H_SUBKEY_SHIFT 12

#define CRYPTO_GMAC_CONF_ENDIANITY_SWAP_RESERVED_15_15 (1 << 15)
/* bit[0] - bit swap bit[1] - byte swap bit[2] - word swap */
#define CRYPTO_GMAC_CONF_ENDIANITY_SWAP_J0_MASK 0x00070000
#define CRYPTO_GMAC_CONF_ENDIANITY_SWAP_J0_SHIFT 16

#define CRYPTO_GMAC_CONF_ENDIANITY_SWAP_RESERVED_19_19 (1 << 19)
/* bit[0] - bit swap bit[1] - byte swap bit[2] - word swap */
#define CRYPTO_GMAC_CONF_ENDIANITY_SWAP_SIGN_IN_MASK 0x00700000
#define CRYPTO_GMAC_CONF_ENDIANITY_SWAP_SIGN_IN_SHIFT 20

#define CRYPTO_GMAC_CONF_ENDIANITY_SWAP_RESERVED_23_23 (1 << 23)
/* bit[0] - bit swap bit[1] - byte swap bit[2] - word swap */
#define CRYPTO_GMAC_CONF_ENDIANITY_SWAP_SIGN_OUT_MASK 0x07000000
#define CRYPTO_GMAC_CONF_ENDIANITY_SWAP_SIGN_OUT_SHIFT 24

#define CRYPTO_GMAC_CONF_ENDIANITY_SWAP_RESERVED_31_27_MASK 0xF8000000
#define CRYPTO_GMAC_CONF_ENDIANITY_SWAP_RESERVED_31_27_SHIFT 27

/**** data_mask_polarity register ****/
/* 0 - Mask to zero. 1 - Mask to one. */
#define CRYPTO_GMAC_CONF_DATA_MASK_POLARITY_POL_ID_0 (1 << 0)
/* 0 - Mask to zero. 1 - Mask to one. */
#define CRYPTO_GMAC_CONF_DATA_MASK_POLARITY_POL_ID_1 (1 << 1)
/* 0 - Mask to zero. 1 - Mask to one. */
#define CRYPTO_GMAC_CONF_DATA_MASK_POLARITY_POL_ID_2 (1 << 2)
/* 0 - Mask to zero. 1 - Mask to one. */
#define CRYPTO_GMAC_CONF_DATA_MASK_POLARITY_POL_ID_3 (1 << 3)

#define CRYPTO_GMAC_CONF_DATA_MASK_POLARITY_RESERVED_31_4_MASK 0xFFFFFFF0
#define CRYPTO_GMAC_CONF_DATA_MASK_POLARITY_RESERVED_31_4_SHIFT 4

/**** configuration register ****/
/* Enable performance counter */
#define CRYPTO_GMAC_PERF_COUNTERS_CONFIGURATION_ENABLE (1 << 0)
/* Reset performance counter */
#define CRYPTO_GMAC_PERF_COUNTERS_CONFIGURATION_RESET (1 << 1)
/* Clear on read performance counter */
#define CRYPTO_GMAC_PERF_COUNTERS_CONFIGURATION_CLEAR_ON_READ (1 << 2)

#define CRYPTO_GMAC_PERF_COUNTERS_CONFIGURATION_RESERVED_31_3_MASK 0xFFFFFFF8
#define CRYPTO_GMAC_PERF_COUNTERS_CONFIGURATION_RESERVED_31_3_SHIFT 3

/**** control register ****/
/* Compression engine enable */
#define CRYPTO_LZ_STATIC_CFG_CONTROL_ENC_EN (1 << 0)
/* Clearing internal compression state */
#define CRYPTO_LZ_STATIC_CFG_CONTROL_ENC_CFG_CLR (1 << 1)
/* Clearing encoder history read pointer */
#define CRYPTO_LZ_STATIC_CFG_CONTROL_ENC_CLEAR_HISTORY_READ_POINTER (1 << 2)
/* Clearing encoder history write pointer */
#define CRYPTO_LZ_STATIC_CFG_CONTROL_ENC_CLEAR_HISTORY_WRITE_POINTER (1 << 3)
/* Clearing encoder history bank offset pointer */
#define CRYPTO_LZ_STATIC_CFG_CONTROL_ENC_CLEAR_HISTORY_BANK_POINTER (1 << 4)
/* Enable performance counter */
#define CRYPTO_LZ_STATIC_CFG_CONTROL_ENC_PERF_COUNTER_EN (1 << 5)
/* Reset performance counter */
#define CRYPTO_LZ_STATIC_CFG_CONTROL_ENC_PERF_COUNTER_RESET (1 << 6)
/* Clear on read performance counter */
#define CRYPTO_LZ_STATIC_CFG_CONTROL_ENC_PERF_COUNTER_CLEAR_ON_READ (1 << 7)
/* Compression engine enable */
#define CRYPTO_LZ_STATIC_CFG_CONTROL_DEC_EN (1 << 8)
/* Clearing internal compression state */
#define CRYPTO_LZ_STATIC_CFG_CONTROL_DEC_CFG_CLR (1 << 9)
/* Clearing decoder history cache pointer */
#define CRYPTO_LZ_STATIC_CFG_CONTROL_DEC_CLEAR_CACHE_POINTER (1 << 10)
/* Clearing decoder history memory pointer */
#define CRYPTO_LZ_STATIC_CFG_CONTROL_DEC_CLEAR_MEMORY_POINTER (1 << 11)
/* Enable performance counter */
#define CRYPTO_LZ_STATIC_CFG_CONTROL_DEC_PERF_COUNTER_EN (1 << 12)
/* Reset performance counter */
#define CRYPTO_LZ_STATIC_CFG_CONTROL_DEC_PERF_COUNTER_RESET (1 << 13)
/* Clear on read performance counter */
#define CRYPTO_LZ_STATIC_CFG_CONTROL_DEC_PERF_COUNTER_CLEAR_ON_READ (1 << 14)

#define CRYPTO_LZ_STATIC_CFG_CONTROL_RESERVED_31_15_MASK 0xFFFF8000
#define CRYPTO_LZ_STATIC_CFG_CONTROL_RESERVED_31_15_SHIFT 15

/**** enc_history_status register ****/
/* History read pointer value */
#define CRYPTO_LZ_STATIC_CFG_ENC_HISTORY_STATUS_READ_POINTER_MASK 0x000000FF
#define CRYPTO_LZ_STATIC_CFG_ENC_HISTORY_STATUS_READ_POINTER_SHIFT 0
/* History write pointer value */
#define CRYPTO_LZ_STATIC_CFG_ENC_HISTORY_STATUS_WRITE_POINTER_MASK 0x0000FF00
#define CRYPTO_LZ_STATIC_CFG_ENC_HISTORY_STATUS_WRITE_POINTER_SHIFT 8
/* History bank offset pointer value */
#define CRYPTO_LZ_STATIC_CFG_ENC_HISTORY_STATUS_BANK_POINTER_MASK 0x00FF0000
#define CRYPTO_LZ_STATIC_CFG_ENC_HISTORY_STATUS_BANK_POINTER_SHIFT 16

#define CRYPTO_LZ_STATIC_CFG_ENC_HISTORY_STATUS_RESERVED_31_24_MASK 0xFF000000
#define CRYPTO_LZ_STATIC_CFG_ENC_HISTORY_STATUS_RESERVED_31_24_SHIFT 24

/**** dec_history_cache_status register ****/
/* History cache pointer value */
#define CRYPTO_LZ_STATIC_CFG_DEC_HISTORY_CACHE_STATUS_POINTER_MASK 0x000000FF
#define CRYPTO_LZ_STATIC_CFG_DEC_HISTORY_CACHE_STATUS_POINTER_SHIFT 0
/* History cache byte offset value */
#define CRYPTO_LZ_STATIC_CFG_DEC_HISTORY_CACHE_STATUS_OFFSET_MASK 0x0000FF00
#define CRYPTO_LZ_STATIC_CFG_DEC_HISTORY_CACHE_STATUS_OFFSET_SHIFT 8

#define CRYPTO_LZ_STATIC_CFG_DEC_HISTORY_CACHE_STATUS_RESERVED_31_16_MASK 0xFFFF0000
#define CRYPTO_LZ_STATIC_CFG_DEC_HISTORY_CACHE_STATUS_RESERVED_31_16_SHIFT 16

/**** dec_history_memory_status register ****/
/* History memory pointer value */
#define CRYPTO_LZ_STATIC_CFG_DEC_HISTORY_MEMORY_STATUS_POINTER_MASK 0x0000FFFF
#define CRYPTO_LZ_STATIC_CFG_DEC_HISTORY_MEMORY_STATUS_POINTER_SHIFT 0
/* History memory byte offset value */
#define CRYPTO_LZ_STATIC_CFG_DEC_HISTORY_MEMORY_STATUS_OFFSET_MASK 0x00FF0000
#define CRYPTO_LZ_STATIC_CFG_DEC_HISTORY_MEMORY_STATUS_OFFSET_SHIFT 16

#define CRYPTO_LZ_STATIC_CFG_DEC_HISTORY_MEMORY_STATUS_RESERVED_31_24_MASK 0xFF000000
#define CRYPTO_LZ_STATIC_CFG_DEC_HISTORY_MEMORY_STATUS_RESERVED_31_24_SHIFT 24

/**** memory_init register ****/
/* Skipping memory initialization for compression memories */
#define CRYPTO_LZ_STATIC_CFG_MEMORY_INIT_SKIP (1 << 0)
/* Triggering memory initialization for compression memories */
#define CRYPTO_LZ_STATIC_CFG_MEMORY_INIT_TRIGGER (1 << 1)

#define CRYPTO_LZ_STATIC_CFG_MEMORY_INIT_RESERVED_31_2_MASK 0xFFFFFFFC
#define CRYPTO_LZ_STATIC_CFG_MEMORY_INIT_RESERVED_31_2_SHIFT 2

/**** entry_alg register ****/
/* Algorithm table ID */
#define CRYPTO_LZ_ALGORITHM_ENTRY_ALG_ID_MASK 0x0000000F
#define CRYPTO_LZ_ALGORITHM_ENTRY_ALG_ID_SHIFT 0

#define CRYPTO_LZ_ALGORITHM_ENTRY_ALG_RESERVED_31_4_MASK 0xFFFFFFF0
#define CRYPTO_LZ_ALGORITHM_ENTRY_ALG_RESERVED_31_4_SHIFT 4

/**** flags_offset register ****/
/* Byte offset for fourth encoded section */
#define CRYPTO_LZ_ALGORITHM_FLAGS_OFFSET_OFFSET_3_MASK 0x000000FF
#define CRYPTO_LZ_ALGORITHM_FLAGS_OFFSET_OFFSET_3_SHIFT 0
/* Byte offset for third encoded section */
#define CRYPTO_LZ_ALGORITHM_FLAGS_OFFSET_OFFSET_2_MASK 0x0000FF00
#define CRYPTO_LZ_ALGORITHM_FLAGS_OFFSET_OFFSET_2_SHIFT 8
/* Byte offset for second encoded section */
#define CRYPTO_LZ_ALGORITHM_FLAGS_OFFSET_OFFSET_1_MASK 0x00FF0000
#define CRYPTO_LZ_ALGORITHM_FLAGS_OFFSET_OFFSET_1_SHIFT 16
/* Byte offset for first encoded section */
#define CRYPTO_LZ_ALGORITHM_FLAGS_OFFSET_OFFSET_0_MASK 0xFF000000
#define CRYPTO_LZ_ALGORITHM_FLAGS_OFFSET_OFFSET_0_SHIFT 24

/**** flags_cfg register ****/
/* Value of encoded code word indication */
#define CRYPTO_LZ_ALGORITHM_FLAGS_CFG_COMPRESSED_POLARITY (1 << 0)
/* Use flags for this algorithm */
#define CRYPTO_LZ_ALGORITHM_FLAGS_CFG_ENABLE (1 << 1)

#define CRYPTO_LZ_ALGORITHM_FLAGS_CFG_RESERVED_31_2_MASK 0xFFFFFFFC
#define CRYPTO_LZ_ALGORITHM_FLAGS_CFG_RESERVED_31_2_SHIFT 2

/**** cmprs_form register ****/
/*
 * Algorithm type : 2'b00 - LZ code word based algorithm (LZ77, LZSS). 2'b01 - LZ stream based
 * algorithm (LZ4). 2'b10 - Static Deflate. 2'b11 - Reserved.
 */
#define CRYPTO_LZ_ALGORITHM_CMPRS_FORM_ALGORITHM_TYPE_MASK 0x00000003
#define CRYPTO_LZ_ALGORITHM_CMPRS_FORM_ALGORITHM_TYPE_SHIFT 0

#define CRYPTO_LZ_ALGORITHM_CMPRS_FORM_RESERVED_3_2_MASK 0x0000000C
#define CRYPTO_LZ_ALGORITHM_CMPRS_FORM_RESERVED_3_2_SHIFT 2
/* A bit specifying whether offset starts from 0 or 1 */
#define CRYPTO_LZ_ALGORITHM_CMPRS_FORM_OFFSET_BASE (1 << 4)
/* 0 - Big Endian. 1 - Little Endian. */
#define CRYPTO_LZ_ALGORITHM_CMPRS_FORM_OFFSET_ENDIANITY (1 << 5)
/* 0 - Huffman extra bits are msb-to-lsb aligned. 1 - Huffman extra bits are lsb-to-msb aligned. */
#define CRYPTO_LZ_ALGORITHM_CMPRS_FORM_HUFFMAN_EXTRA_BITS_SWAP (1 << 6)
/* when asserted, bit-swap is performed for every byte on the compressed data */
#define CRYPTO_LZ_ALGORITHM_CMPRS_FORM_BIT_SWAP (1 << 7)

#define CRYPTO_LZ_ALGORITHM_CMPRS_FORM_RESERVED_31_8_MASK 0xFFFFFF00
#define CRYPTO_LZ_ALGORITHM_CMPRS_FORM_RESERVED_31_8_SHIFT 8

/**** entry_enc_profile register ****/
/* Profile table ID */
#define CRYPTO_LZ_ENC_PROFILE_ENTRY_ENC_PROFILE_ID_MASK 0x0000001F
#define CRYPTO_LZ_ENC_PROFILE_ENTRY_ENC_PROFILE_ID_SHIFT 0

#define CRYPTO_LZ_ENC_PROFILE_ENTRY_ENC_PROFILE_RESERVED_31_5_MASK 0xFFFFFFE0
#define CRYPTO_LZ_ENC_PROFILE_ENTRY_ENC_PROFILE_RESERVED_31_5_SHIFT 5

/**** cfg_enc_profile register ****/
/*
 * History size used for compression in 256B resolution. History of 256[B] and 512*K[B] for
 * K=1,...,64 are supported.
 */
#define CRYPTO_LZ_ENC_PROFILE_CFG_ENC_PROFILE_HISTORY_SIZE_MASK 0x000000FF
#define CRYPTO_LZ_ENC_PROFILE_CFG_ENC_PROFILE_HISTORY_SIZE_SHIFT 0
/* The profile algorithm index */
#define CRYPTO_LZ_ENC_PROFILE_CFG_ENC_PROFILE_ALGORITHM_INDEX_MASK 0x00000F00
#define CRYPTO_LZ_ENC_PROFILE_CFG_ENC_PROFILE_ALGORITHM_INDEX_SHIFT 8
/* A internal optimization scheme of compression */
#define CRYPTO_LZ_ENC_PROFILE_CFG_ENC_PROFILE_CMPRS_OPTIMIZER_EN (1 << 12)
/*
 * A bit indicating that packet with relevant ID should not be encoded regardless of the output
 * controller index
 */
#define CRYPTO_LZ_ENC_PROFILE_CFG_ENC_PROFILE_FORCE_ORIGINAL_OUT (1 << 13)
/*
 * A bit indicating that packet with relevant ID should be encoded regardless of the output
 * controller index
 */
#define CRYPTO_LZ_ENC_PROFILE_CFG_ENC_PROFILE_FORCE_ENCODED_OUT (1 << 14)
/*
 * Output controller index arbitrating between the original and encoded data according to the
 * compression quality
 */
#define CRYPTO_LZ_ENC_PROFILE_CFG_ENC_PROFILE_OUTPUT_CONTROLLER_INDEX_MASK 0x00018000
#define CRYPTO_LZ_ENC_PROFILE_CFG_ENC_PROFILE_OUTPUT_CONTROLLER_INDEX_SHIFT 15
/*
 * Adding next symbol to a compressed code word that is described with offset and length. Required
 * for LZ77.
 */
#define CRYPTO_LZ_ENC_PROFILE_CFG_ENC_PROFILE_CONCAT_NEXT_SYMBOL (1 << 17)

#define CRYPTO_LZ_ENC_PROFILE_CFG_ENC_PROFILE_RESERVED_31_18_MASK 0xFFFC0000
#define CRYPTO_LZ_ENC_PROFILE_CFG_ENC_PROFILE_RESERVED_31_18_SHIFT 18

/**** thresholds_enc_profile register ****/
/* The maximal match to be encoded */
#define CRYPTO_LZ_ENC_PROFILE_THRESHOLDS_ENC_PROFILE_MAX_MASK 0x0000FFFF
#define CRYPTO_LZ_ENC_PROFILE_THRESHOLDS_ENC_PROFILE_MAX_SHIFT 0
/* The minimal match to be encoded */
#define CRYPTO_LZ_ENC_PROFILE_THRESHOLDS_ENC_PROFILE_MIN_MASK 0x00FF0000
#define CRYPTO_LZ_ENC_PROFILE_THRESHOLDS_ENC_PROFILE_MIN_SHIFT 16

#define CRYPTO_LZ_ENC_PROFILE_THRESHOLDS_ENC_PROFILE_RESERVED_31_24_MASK 0xFF000000
#define CRYPTO_LZ_ENC_PROFILE_THRESHOLDS_ENC_PROFILE_RESERVED_31_24_SHIFT 24

/**** eop_policy_profile register ****/
/* The last N bytes within a new compressed code word cannot begin */
#define CRYPTO_LZ_ENC_PROFILE_EOP_POLICY_PROFILE_NO_MATCH_LENGTH_MASK 0x000000FF
#define CRYPTO_LZ_ENC_PROFILE_EOP_POLICY_PROFILE_NO_MATCH_LENGTH_SHIFT 0
/* The last N bytes that would not be compressed */
#define CRYPTO_LZ_ENC_PROFILE_EOP_POLICY_PROFILE_UNCOMPRESSED_LENGTH_MASK 0x0000FF00
#define CRYPTO_LZ_ENC_PROFILE_EOP_POLICY_PROFILE_UNCOMPRESSED_LENGTH_SHIFT 8

#define CRYPTO_LZ_ENC_PROFILE_EOP_POLICY_PROFILE_RESERVED_31_16_MASK 0xFFFF0000
#define CRYPTO_LZ_ENC_PROFILE_EOP_POLICY_PROFILE_RESERVED_31_16_SHIFT 16

/**** entry_dec_profile register ****/
/* Profile table ID */
#define CRYPTO_LZ_DEC_PROFILE_ENTRY_DEC_PROFILE_ID_MASK 0x0000001F
#define CRYPTO_LZ_DEC_PROFILE_ENTRY_DEC_PROFILE_ID_SHIFT 0

#define CRYPTO_LZ_DEC_PROFILE_ENTRY_DEC_PROFILE_RESERVED_31_5_MASK 0xFFFFFFE0
#define CRYPTO_LZ_DEC_PROFILE_ENTRY_DEC_PROFILE_RESERVED_31_5_SHIFT 5

/**** cfg register ****/
/*
 * History size used for decompression in 256B resolution. History of 256[B] and 512*K[B] for
 * K=1,...,64 are supported.
 */
#define CRYPTO_LZ_DEC_PROFILE_CFG_HISTORY_SIZE_MASK 0x000000FF
#define CRYPTO_LZ_DEC_PROFILE_CFG_HISTORY_SIZE_SHIFT 0
/* The minimal match length of encoded sequence */
#define CRYPTO_LZ_DEC_PROFILE_CFG_MATCH_LENGTH_BASE_MASK 0x0000FF00
#define CRYPTO_LZ_DEC_PROFILE_CFG_MATCH_LENGTH_BASE_SHIFT 8
/* A bit indicating that packet with relevant ID is not encoded */
#define CRYPTO_LZ_DEC_PROFILE_CFG_FORCE_ORIGINAL_OUT (1 << 16)

#define CRYPTO_LZ_DEC_PROFILE_CFG_RESERVED_31_17_MASK 0xFFFE0000
#define CRYPTO_LZ_DEC_PROFILE_CFG_RESERVED_31_17_SHIFT 17

/**** alg register ****/
/* Flag field polarity of a bit specifying that the following data field is encoded */
#define CRYPTO_LZ_DEC_PROFILE_ALG_FLAG_COMPRESSED_POLARITY (1 << 0)
/* A bit indicating whether a flag indication is used */
#define CRYPTO_LZ_DEC_PROFILE_ALG_FLAG_ENABLE (1 << 1)
/* 0 - Reserved. 1 - Byte coalescing. 2 - Half word coalescing. 3 - Word coalescing */
#define CRYPTO_LZ_DEC_PROFILE_ALG_FLAG_COALESCING_MASK 0x0000000C
#define CRYPTO_LZ_DEC_PROFILE_ALG_FLAG_COALESCING_SHIFT 2
/* 0 - Offset is encoded with 1 byte. 1 - Offset is encoded in 2 bytes. */
#define CRYPTO_LZ_DEC_PROFILE_ALG_FIELD_OFFSET_SIZE (1 << 4)
/* 0 - Length is encoded with 1 byte. 1 - Length is encoded in 2 bytes. */
#define CRYPTO_LZ_DEC_PROFILE_ALG_FIELD_LENGTH_SIZE (1 << 5)
/*
 * The encoded field ordering. 0 - (offset, length, symbol). 1 - (offset, symbol, length). 2 -
 * (length, offset, symbol). 3 - (length, symbol, offset). 4 - (symbol, offset, length). 5 -
 * (symbol, length, offset)
 */
#define CRYPTO_LZ_DEC_PROFILE_ALG_FIELD_COMPOSITION_MASK 0x000001C0
#define CRYPTO_LZ_DEC_PROFILE_ALG_FIELD_COMPOSITION_SHIFT 6
/*
 * Algorithm type : 2'b00 - LZ code word based algorithm (LZ77, LZSS). 2'b01 - LZ stream based
 * algorithm (LZ4). 2'b10 - Static Deflate. 2'b11 - Dynamic Deflate.
 */
#define CRYPTO_LZ_DEC_PROFILE_ALG_ALGORITHM_TYPE_MASK 0x00000600
#define CRYPTO_LZ_DEC_PROFILE_ALG_ALGORITHM_TYPE_SHIFT 9
/* Compressed code word indication also includes next symbol. Required for LZ77. */
#define CRYPTO_LZ_DEC_PROFILE_ALG_CONCAT_NEXT_SYMBOL (1 << 11)
/* A bit specifying whether offset starts from 0 or 1 */
#define CRYPTO_LZ_DEC_PROFILE_ALG_OFFSET_BASE (1 << 12)
/* 0 - Big Endian. 1 - Little Endian. */
#define CRYPTO_LZ_DEC_PROFILE_ALG_OFFSET_ENDIANITY (1 << 13)
/* 0 - Huffman extra bits are msb-to-lsb aligned. 1 - Huffman extra bits are lsb-to-msb aligned. */
#define CRYPTO_LZ_DEC_PROFILE_ALG_HUFFMAN_EXTRA_BITS_SWAP (1 << 14)
/* when asserted, bit-swap is performed for every byte on the compressed data */
#define CRYPTO_LZ_DEC_PROFILE_ALG_BIT_SWAP (1 << 15)

#define CRYPTO_LZ_DEC_PROFILE_ALG_RESERVED_31_16_MASK 0xFFFF0000
#define CRYPTO_LZ_DEC_PROFILE_ALG_RESERVED_31_16_SHIFT 16

/**** entry_out_arb register ****/
/* Output table ID */
#define CRYPTO_LZ_OUTPUT_ARBITER_ENTRY_OUT_ARB_ID_MASK 0x00000003
#define CRYPTO_LZ_OUTPUT_ARBITER_ENTRY_OUT_ARB_ID_SHIFT 0

#define CRYPTO_LZ_OUTPUT_ARBITER_ENTRY_OUT_ARB_RESERVED_31_2_MASK 0xFFFFFFFC
#define CRYPTO_LZ_OUTPUT_ARBITER_ENTRY_OUT_ARB_RESERVED_31_2_SHIFT 2

/**** alu_data_0_1 register ****/
/* Data for ALU */
#define CRYPTO_LZ_OUTPUT_ARBITER_ALU_DATA_0_1_CONST0_MASK 0x0000FFFF
#define CRYPTO_LZ_OUTPUT_ARBITER_ALU_DATA_0_1_CONST0_SHIFT 0
/* Data for ALU */
#define CRYPTO_LZ_OUTPUT_ARBITER_ALU_DATA_0_1_CONST1_MASK 0xFFFF0000
#define CRYPTO_LZ_OUTPUT_ARBITER_ALU_DATA_0_1_CONST1_SHIFT 16

/**** alu_data_2_3 register ****/
/* Data for ALU */
#define CRYPTO_LZ_OUTPUT_ARBITER_ALU_DATA_2_3_CONST2_MASK 0x0000FFFF
#define CRYPTO_LZ_OUTPUT_ARBITER_ALU_DATA_2_3_CONST2_SHIFT 0
/* Data for ALU */
#define CRYPTO_LZ_OUTPUT_ARBITER_ALU_DATA_2_3_CONST3_MASK 0xFFFF0000
#define CRYPTO_LZ_OUTPUT_ARBITER_ALU_DATA_2_3_CONST3_SHIFT 16

/**** cfg_out_arb register ****/
/*
 * First alu input mux between original data (5), encoded data (4), const3 (3), const2 (2), const1
 * (1), const0 (0)
 */
#define CRYPTO_LZ_OUTPUT_ARBITER_CFG_OUT_ARB_ALU_MUX_0_MASK 0x00000007
#define CRYPTO_LZ_OUTPUT_ARBITER_CFG_OUT_ARB_ALU_MUX_0_SHIFT 0
/*
 * Second alu input mux between original data (5), encoded data (4), const3 (3), const2 (2), const1
 * (1), const0 (0)
 */
#define CRYPTO_LZ_OUTPUT_ARBITER_CFG_OUT_ARB_ALU_MUX_1_MASK 0x00000038
#define CRYPTO_LZ_OUTPUT_ARBITER_CFG_OUT_ARB_ALU_MUX_1_SHIFT 3
/*
 * Third alu input mux between original data (5), encoded data (4), const3 (3), const2 (2), const1
 * (1), const0 (0)
 */
#define CRYPTO_LZ_OUTPUT_ARBITER_CFG_OUT_ARB_ALU_MUX_2_MASK 0x000001C0
#define CRYPTO_LZ_OUTPUT_ARBITER_CFG_OUT_ARB_ALU_MUX_2_SHIFT 6
/*
 * Fourth alu input mux between original data (5), encoded data (4), const3 (3), const2 (2), const1
 * (1), const0 (0)
 */
#define CRYPTO_LZ_OUTPUT_ARBITER_CFG_OUT_ARB_ALU_MUX_3_MASK 0x00000E00
#define CRYPTO_LZ_OUTPUT_ARBITER_CFG_OUT_ARB_ALU_MUX_3_SHIFT 9
/* ALU0 command between in0 and in1 */
#define CRYPTO_LZ_OUTPUT_ARBITER_CFG_OUT_ARB_ALU_OPCODE_0_MASK 0x0000F000
#define CRYPTO_LZ_OUTPUT_ARBITER_CFG_OUT_ARB_ALU_OPCODE_0_SHIFT 12
/* ALU1 command between in2 and in3 */
#define CRYPTO_LZ_OUTPUT_ARBITER_CFG_OUT_ARB_ALU_OPCODE_1_MASK 0x000F0000
#define CRYPTO_LZ_OUTPUT_ARBITER_CFG_OUT_ARB_ALU_OPCODE_1_SHIFT 16
/* ALU2 command between the alu_out0 and alu_out1 */
#define CRYPTO_LZ_OUTPUT_ARBITER_CFG_OUT_ARB_ALU_OPCODE_2_MASK 0x00F00000
#define CRYPTO_LZ_OUTPUT_ARBITER_CFG_OUT_ARB_ALU_OPCODE_2_SHIFT 20
/* Compressing (1) or not (0) the packet if result of ALUs is negative */
#define CRYPTO_LZ_OUTPUT_ARBITER_CFG_OUT_ARB_ALU_NEGATIVE_RES_MUX (1 << 24)
/* Compressing (1) or not (0) the packet if result of ALUs is positive */
#define CRYPTO_LZ_OUTPUT_ARBITER_CFG_OUT_ARB_ALU_POSITIVE_RES_MUX (1 << 25)
/* Compressing (1) or not (0) the packet if result of ALUs is zero */
#define CRYPTO_LZ_OUTPUT_ARBITER_CFG_OUT_ARB_ALU_ZERO_RES_MUX (1 << 26)

#define CRYPTO_LZ_OUTPUT_ARBITER_CFG_OUT_ARB_RESERVED_31_27_MASK 0xF8000000
#define CRYPTO_LZ_OUTPUT_ARBITER_CFG_OUT_ARB_RESERVED_31_27_SHIFT 27

/**** entry_format register ****/
/* Format table ID */
#define CRYPTO_LZ_FORMAT_ENTRY_FORMAT_ID_MASK 0x00000007
#define CRYPTO_LZ_FORMAT_ENTRY_FORMAT_ID_SHIFT 0

#define CRYPTO_LZ_FORMAT_ENTRY_FORMAT_RESERVED_31_3_MASK 0xFFFFFFF8
#define CRYPTO_LZ_FORMAT_ENTRY_FORMAT_RESERVED_31_3_SHIFT 3

/**** offset register ****/
/* Code Words 0-7 offset */
#define CRYPTO_LZ_FORMAT_OFFSET_CODE_WORDS_0_7_MASK 0x000000FF
#define CRYPTO_LZ_FORMAT_OFFSET_CODE_WORDS_0_7_SHIFT 0
/* Code Words 8-15 offset */
#define CRYPTO_LZ_FORMAT_OFFSET_CODE_WORDS_8_15_MASK 0x0000FF00
#define CRYPTO_LZ_FORMAT_OFFSET_CODE_WORDS_8_15_SHIFT 8
/* Code Words 16-23 offset */
#define CRYPTO_LZ_FORMAT_OFFSET_CODE_WORDS_16_23_MASK 0x00FF0000
#define CRYPTO_LZ_FORMAT_OFFSET_CODE_WORDS_16_23_SHIFT 16
/* Code Words 24-31 offset */
#define CRYPTO_LZ_FORMAT_OFFSET_CODE_WORDS_24_31_MASK 0xFF000000
#define CRYPTO_LZ_FORMAT_OFFSET_CODE_WORDS_24_31_SHIFT 24

/**** cfg_format register ****/
/* ALU symbol0 command */
#define CRYPTO_LZ_FORMAT_CFG_FORMAT_CMD_ALU_SYMBOL_0_MASK 0x0000000F
#define CRYPTO_LZ_FORMAT_CFG_FORMAT_CMD_ALU_SYMBOL_0_SHIFT 0
/* ALU symbol1 command */
#define CRYPTO_LZ_FORMAT_CFG_FORMAT_CMD_ALU_SYMBOL_1_MASK 0x000000F0
#define CRYPTO_LZ_FORMAT_CFG_FORMAT_CMD_ALU_SYMBOL_1_SHIFT 4
/* ALU length0 command */
#define CRYPTO_LZ_FORMAT_CFG_FORMAT_CMD_ALU_LENGTH_0_MASK 0x00000F00
#define CRYPTO_LZ_FORMAT_CFG_FORMAT_CMD_ALU_LENGTH_0_SHIFT 8
/* ALU length1 command */
#define CRYPTO_LZ_FORMAT_CFG_FORMAT_CMD_ALU_LENGTH_1_MASK 0x0000F000
#define CRYPTO_LZ_FORMAT_CFG_FORMAT_CMD_ALU_LENGTH_1_SHIFT 12
/* ALU offset1 command */
#define CRYPTO_LZ_FORMAT_CFG_FORMAT_CMD_ALU_OFFSET_0_MASK 0x000F0000
#define CRYPTO_LZ_FORMAT_CFG_FORMAT_CMD_ALU_OFFSET_0_SHIFT 16
/* ALU offset1 command */
#define CRYPTO_LZ_FORMAT_CFG_FORMAT_CMD_ALU_OFFSET_1_MASK 0x00F00000
#define CRYPTO_LZ_FORMAT_CFG_FORMAT_CMD_ALU_OFFSET_1_SHIFT 20

#define CRYPTO_LZ_FORMAT_CFG_FORMAT_FIELD_BYTE_COUNT_MASK 0x07000000
#define CRYPTO_LZ_FORMAT_CFG_FORMAT_FIELD_BYTE_COUNT_SHIFT 24

#define CRYPTO_LZ_FORMAT_CFG_FORMAT_RESERVED_31_27_MASK 0xF8000000
#define CRYPTO_LZ_FORMAT_CFG_FORMAT_RESERVED_31_27_SHIFT 27

/**** entry_cam register ****/
/* CAM entry id */
#define CRYPTO_LZ_CAM_ENTRY_CAM_ID_MASK 0x0000000F
#define CRYPTO_LZ_CAM_ENTRY_CAM_ID_SHIFT 0

#define CRYPTO_LZ_CAM_ENTRY_CAM_RESERVED_31_4_MASK 0xFFFFFFF0
#define CRYPTO_LZ_CAM_ENTRY_CAM_RESERVED_31_4_SHIFT 4

/**** cfg_cam register ****/
/* The format table to be used for encoding */
#define CRYPTO_LZ_CAM_CFG_CAM_FORMAT_TABLE_INDEX_MASK 0x0000000F
#define CRYPTO_LZ_CAM_CFG_CAM_FORMAT_TABLE_INDEX_SHIFT 0
/* Algorithm condition */
#define CRYPTO_LZ_CAM_CFG_CAM_ALGORITHM_INDEX_MASK 0x000000F0
#define CRYPTO_LZ_CAM_CFG_CAM_ALGORITHM_INDEX_SHIFT 4
/* Compressed symbol condition enabled */
#define CRYPTO_LZ_CAM_CFG_CAM_COMPRESSED_SYMBOL_EN (1 << 8)
/* Compressed symbol condition */
#define CRYPTO_LZ_CAM_CFG_CAM_COMPRESSED_SYMBOL (1 << 9)
/* Search CAM entry according to length (1)/offset (0) */
#define CRYPTO_LZ_CAM_CFG_CAM_MATCH_LENGTH_OFFSET_SEL (1 << 10)
/* Entry valid indication */
#define CRYPTO_LZ_CAM_CFG_CAM_CAM_ENTRY_VALID (1 << 11)

#define CRYPTO_LZ_CAM_CFG_CAM_RESERVED_31_12_MASK 0xFFFFF000
#define CRYPTO_LZ_CAM_CFG_CAM_RESERVED_31_12_SHIFT 12

/**** threshold_cam register ****/
/* Minimal length/offset threshold */
#define CRYPTO_LZ_CAM_THRESHOLD_CAM_MIN_CODE_THRESHOLD_MASK 0x0000FFFF
#define CRYPTO_LZ_CAM_THRESHOLD_CAM_MIN_CODE_THRESHOLD_SHIFT 0
/* Maximal length/offset threshold */
#define CRYPTO_LZ_CAM_THRESHOLD_CAM_MAX_CODE_THRESHOLD_MASK 0xFFFF0000
#define CRYPTO_LZ_CAM_THRESHOLD_CAM_MAX_CODE_THRESHOLD_SHIFT 16

/**** counter_control register ****/
/* Enable performance counter */
#define CRYPTO_LZ_DEC_HISTORY_STAT_COUNTER_CONTROL_ENABLE (1 << 0)
/* Reset performance counter */
#define CRYPTO_LZ_DEC_HISTORY_STAT_COUNTER_CONTROL_RESET (1 << 1)
/* Clear on read performance counter */
#define CRYPTO_LZ_DEC_HISTORY_STAT_COUNTER_CONTROL_CLEAR_ON_READ (1 << 2)

#define CRYPTO_LZ_DEC_HISTORY_STAT_COUNTER_CONTROL_RESERVED_31_3_MASK 0xFFFFFFF8
#define CRYPTO_LZ_DEC_HISTORY_STAT_COUNTER_CONTROL_RESERVED_31_3_SHIFT 3

/**** entry register ****/
/*
 * 0-255 : Literal entries. 256 : End-of-buffer entry. 257-285 : Length entries. 286-315 : Offset
 * entries.
 */
#define CRYPTO_HUFFMAN_STATIC_TABLE_ENTRY_ID_MASK 0x000001FF
#define CRYPTO_HUFFMAN_STATIC_TABLE_ENTRY_ID_SHIFT 0

#define CRYPTO_HUFFMAN_STATIC_TABLE_ENTRY_RESERVED_31_9_MASK 0xFFFFFE00
#define CRYPTO_HUFFMAN_STATIC_TABLE_ENTRY_RESERVED_31_9_SHIFT 9

/**** value register ****/
/*
 * The distance range for the encoded entry. Irrelevant for literal and end-of-buffer entries
 * whereas dist==entry_id. For length entries only 9 lsb bits are used.
 */
#define CRYPTO_HUFFMAN_STATIC_TABLE_VALUE_DIST_MASK 0x00007FFF
#define CRYPTO_HUFFMAN_STATIC_TABLE_VALUE_DIST_SHIFT 0

#define CRYPTO_HUFFMAN_STATIC_TABLE_VALUE_RESERVED_31_15_MASK 0xFFFF8000
#define CRYPTO_HUFFMAN_STATIC_TABLE_VALUE_RESERVED_31_15_SHIFT 15

/**** alphabet register ****/
/* The Huffman coding for entry with appropriate distance. */
#define CRYPTO_HUFFMAN_STATIC_TABLE_ALPHABET_CODE_MASK 0x00007FFF
#define CRYPTO_HUFFMAN_STATIC_TABLE_ALPHABET_CODE_SHIFT 0

#define CRYPTO_HUFFMAN_STATIC_TABLE_ALPHABET_RESERVED_15_15 (1 << 15)
/* Huffman code length. The code should be lsb aligned for partial length. */
#define CRYPTO_HUFFMAN_STATIC_TABLE_ALPHABET_LENGTH_MASK 0x000F0000
#define CRYPTO_HUFFMAN_STATIC_TABLE_ALPHABET_LENGTH_SHIFT 16
/*
 * Extra bits following the code found in the stream, used to determine the exact value. Irrelevant
 * for literal and end-of-buffer entries whereas extra_bits==0.
 */
#define CRYPTO_HUFFMAN_STATIC_TABLE_ALPHABET_EXTRA_BITS_MASK 0x00F00000
#define CRYPTO_HUFFMAN_STATIC_TABLE_ALPHABET_EXTRA_BITS_SHIFT 20

#define CRYPTO_HUFFMAN_STATIC_TABLE_ALPHABET_RESERVED_31_24_MASK 0xFF000000
#define CRYPTO_HUFFMAN_STATIC_TABLE_ALPHABET_RESERVED_31_24_SHIFT 24

/**** icrc_profile_table_idx register ****/
/* enable statically overriding the profile index */
#define CRYPTO_INLINE_CRC_CFG_ICRC_PROFILE_TABLE_IDX_OVERRIDE_EN (1 << 0)
/* profile index (only relevant if override_en=1) */
#define CRYPTO_INLINE_CRC_CFG_ICRC_PROFILE_TABLE_IDX_OVERRIDE_VAL_MASK 0x000000F0
#define CRYPTO_INLINE_CRC_CFG_ICRC_PROFILE_TABLE_IDX_OVERRIDE_VAL_SHIFT 4

/**** crc_addr register ****/
/* profile table address */
#define CRYPTO_CRC_PROFILE_TABLE_CRC_ADDR_VAL_MASK 0x0000000F
#define CRYPTO_CRC_PROFILE_TABLE_CRC_ADDR_VAL_SHIFT 0

/**** crc_data register ****/
/* byte swap on ingress data */
#define CRYPTO_CRC_PROFILE_TABLE_CRC_DATA_INGRESS_BYTE_SWAP (1 << 0)
/* bit swap on ingress data */
#define CRYPTO_CRC_PROFILE_TABLE_CRC_DATA_INGRESS_BIT_SWAP (1 << 1)
/* byte swap on result */
#define CRYPTO_CRC_PROFILE_TABLE_CRC_DATA_RES_BYTE_SWAP (1 << 2)
/* bit swap on result */
#define CRYPTO_CRC_PROFILE_TABLE_CRC_DATA_RES_BIT_SWAP (1 << 3)
/* perform one's completement on the result */
#define CRYPTO_CRC_PROFILE_TABLE_CRC_DATA_RES_ONE_COMPLETEMENT (1 << 4)
/* byte swap on iv */
#define CRYPTO_CRC_PROFILE_TABLE_CRC_DATA_IV_BYTE_SWAP (1 << 5)
/* bit swap on iv */
#define CRYPTO_CRC_PROFILE_TABLE_CRC_DATA_IV_BIT_SWAP (1 << 6)
/* perform one's completement on the iv */
#define CRYPTO_CRC_PROFILE_TABLE_CRC_DATA_IV_ONE_COMPLETEMENT (1 << 7)
/* index to crc header mask */
#define CRYPTO_CRC_PROFILE_TABLE_CRC_DATA_START_MASK_IDX_MASK 0x00030000
#define CRYPTO_CRC_PROFILE_TABLE_CRC_DATA_START_MASK_IDX_SHIFT 16

#ifdef __cplusplus
}
#endif

#endif

/** @} */


