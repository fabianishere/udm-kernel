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
#include "al_hal_ssm_crypto_internal.h"

/** Compression encoder profile table entry */
struct al_crypto_cmprs_enc_prof_tbl_ent {
	enum al_crypto_cmprs_prof_id	id;
	unsigned int	history_size;
	enum al_crypto_cmprs_alg_idx	algorithm_index;
	al_bool		compression_optimizer_en;
	al_bool		force_original_sequence_out;
	al_bool		force_encoded_sequence_out;
	unsigned int	output_controller_index;
	unsigned int	concat_next_symbol;
	unsigned int	max_match_threshold;
	unsigned int	min_match_threshold;
	unsigned int	no_match_length;
	unsigned int	uncompressed_length;
};

/** Compression decoder profile table entry */
struct al_crypto_cmprs_dec_prof_tbl_ent {
	enum al_crypto_cmprs_prof_id	id;
	unsigned int	history_size;
	unsigned int	match_length_base;
	al_bool		force_original_sequence_out;
	al_bool		compressed_flag_polarity;
	al_bool		compressed_flag_en;
	unsigned int	compressed_flag_coalescing;
	al_bool		field_offset_size;
	al_bool		field_length_size;
	unsigned int	field_composition;
	unsigned int		algorithm_type;
	unsigned int	concat_next_symbol;
	al_bool		offset_base;
	al_bool		offset_endianity;
	al_bool   huffman_extra_bits_swap;
	al_bool   bit_swap;
};

/** Compression algorithm table entry */
struct al_crypto_cmprs_alg_tbl_ent {
	enum al_crypto_cmprs_alg_idx	id;
	unsigned int	flags_offset;
	unsigned int	flags_pos_0;
	unsigned int	flags_pos_1;
	unsigned int	flags_pos_2;
	unsigned int	flags_pos_3;
	al_bool		flags_cfg_polarity;
	al_bool		flags_cfg_en;
	unsigned int		algorithm_type;
	al_bool		offset_base;
	al_bool		offset_endianity;
	al_bool   huffman_extra_bits_swap;
	al_bool   bit_swap;
};

/** Compression CAM table entry */
struct al_crypto_cmprs_cam_tbl_ent {
	enum al_crypto_cmprs_cam_id	id;
	enum al_crypto_cmprs_format_idx	format_table_index;
	enum al_crypto_cmprs_alg_idx	algorithm_index;
	al_bool		compressed_symbol_en;
	al_bool		compressed_symbol;
	al_bool		match_length_offset_sel;
	al_bool		entry_valid;
	unsigned int	min_code_threshold;
	unsigned int	max_code_threshold;
};

/** Compression format table entry */
struct al_crypto_cmprs_fmt_tbl_ent {
	enum al_crypto_cmprs_format_idx	id;
	unsigned int	code_word_offset;
	unsigned int	alu_0_input_symbol;
	unsigned int	alu_1_input_symbol;
	unsigned int	alu_0_input_length;
	unsigned int	alu_1_input_length;
	unsigned int	alu_0_input_offset;
	unsigned int	alu_1_input_offset;
	unsigned int	alu_0_command_symbol;
	unsigned int	alu_1_command_symbol;
	unsigned int	alu_0_command_length;
	unsigned int	alu_1_command_length;
	unsigned int	alu_0_command_offset;
	unsigned int	alu_1_command_offset;
	unsigned int	field_byte_count;
};

/** Compression encoder output table entry */
struct al_crypto_cmprs_enc_out_tbl_ent {
	unsigned int	id;
	unsigned int	alu_const_0;
	unsigned int	alu_const_1;
	unsigned int	alu_const_2;
	unsigned int	alu_const_3;
	unsigned int	alu_in_mux_0;
	unsigned int	alu_in_mux_1;
	unsigned int	alu_in_mux_2;
	unsigned int	alu_in_mux_3;
	unsigned int	alu_opcode_0;
	unsigned int	alu_opcode_1;
	unsigned int	alu_opcode_2;
	al_bool		alu_negative_res_mux_sel;
	al_bool		alu_positive_res_mux_sel;
	al_bool		alu_zero_res_mux_sel;
};

/** GMAC override default */
static const struct al_crypto_gmac_ovrd gmac_ovrd_cfg = {
	.do_auth_serial = AL_FALSE,
	.auth_override_en = AL_FALSE,
	.auth_override_dis = AL_FALSE,
	.val_gen_n_overrid_en = AL_FALSE,
	.val_gen_n = 0,
	.auth_mode_override = AL_FALSE,
	.auth_mode = 0,
	.key_type_override = AL_FALSE,
	.key_type = 0,
	.sign_size_override = AL_FALSE,
	.sign_size = 0,
	.digest_from_msb_override = AL_FALSE,
	.digest_from_msb = 0,
};

/** GMAC endianity swap default */
static const struct al_crypto_gmac_endianity_swap gmac_endianity_swap_cfg = {
	.ingress = { .bit_swap = 0, .byte_swap = 0, .word_swap = 0, .aes_beat_swap = 0, },
	.egress = { .bit_swap = 0, .byte_swap = 0, .word_swap = 0, .aes_beat_swap = 0, },
	.aes_key = { .bit_swap = 0, .byte_swap = 0, .word_swap = 0, .aes_beat_swap = 0, },
	.h_subkey = { .bit_swap = 0, .byte_swap = 0, .word_swap = 0, .aes_beat_swap = 0, },
	.j0 = { .bit_swap = 0, .byte_swap = 0, .word_swap = 0, .aes_beat_swap = 0, },
	.sign_in = { .bit_swap = 0, .byte_swap = 0, .word_swap = 0, .aes_beat_swap = 0, },
	.sign_out = { .bit_swap = 0, .byte_swap = 0, .word_swap = 0, .aes_beat_swap = 0, },
};

/** GMAC data mask polarity default */
static const struct al_crypto_gmac_data_mask gmac_data_mask_cfg = {
	.pol_id_0_mask_val = 0,
	.pol_id_1_mask_val = 0,
	.pol_id_2_mask_val = 0,
	.pol_id_3_mask_val = 0,
	.arr = {
		{ .id_0 = 0, .id_1 = 0, .id_2 = 0, .id_3 = 0, },
		{ .id_0 = 0, .id_1 = 0, .id_2 = 0, .id_3 = 0, },
		{ .id_0 = 0, .id_1 = 0, .id_2 = 0, .id_3 = 0, },
		{ .id_0 = 0, .id_1 = 0, .id_2 = 0, .id_3 = 0, },
		{ .id_0 = 0, .id_1 = 0, .id_2 = 0, .id_3 = 0, },
		{ .id_0 = 0, .id_1 = 0, .id_2 = 0, .id_3 = 0, },
		{ .id_0 = 0, .id_1 = 0, .id_2 = 0, .id_3 = 0, },
		{ .id_0 = 0, .id_1 = 0, .id_2 = 0, .id_3 = 0, },
		{ .id_0 = 0, .id_1 = 0, .id_2 = 0, .id_3 = 0, },
		{ .id_0 = 0, .id_1 = 0, .id_2 = 0, .id_3 = 0, },
		{ .id_0 = 0, .id_1 = 0, .id_2 = 0, .id_3 = 0, },
		{ .id_0 = 0, .id_1 = 0, .id_2 = 0, .id_3 = 0, },
		{ .id_0 = 0, .id_1 = 0, .id_2 = 0, .id_3 = 0, },
		{ .id_0 = 0, .id_1 = 0, .id_2 = 0, .id_3 = 0, },
		{ .id_0 = 0, .id_1 = 0, .id_2 = 0, .id_3 = 0, },
		{ .id_0 = 0, .id_1 = 0, .id_2 = 0, .id_3 = 0, },
	},
};

/** Compression encoder profile table initial entries */
static const struct al_crypto_cmprs_enc_prof_tbl_ent enc_prof_tbl_entries[] = {
		{
			/*LZ77 256B History 3B Code Word Size*/
			.id = AL_CRYPT_CMPRS_LZ77_1B_OFFSET_256B_HISTORY,
			.history_size = 1,
			.algorithm_index = AL_CRYPT_CMPRS_LZ77_1B_OFFSET,
			.compression_optimizer_en = 1,
			.force_original_sequence_out = 0,
			.force_encoded_sequence_out = 1,
			.output_controller_index = 0,
			.concat_next_symbol = 1,
			.max_match_threshold = 255,
			.min_match_threshold = 0,
			.no_match_length = 0,
			.uncompressed_length = 0,
		},
		{
			/*LZ77 512B History 4B Code Word Size*/
			.id = AL_CRYPT_CMPRS_LZ77_2B_OFFSET_512B_HISTORY,
			.history_size = 2,
			.algorithm_index = AL_CRYPT_CMPRS_LZ77_2B_OFFSET,
			.compression_optimizer_en = 1,
			.force_original_sequence_out = 0,
			.force_encoded_sequence_out = 1,
			.output_controller_index = 0,
			.concat_next_symbol = 1,
			.max_match_threshold = 255,
			.min_match_threshold = 0,
			.no_match_length = 0,
			.uncompressed_length = 0,
		},
		{
			/*LZ77 1KB History 4B Code Word Size*/
			.id = AL_CRYPT_CMPRS_LZ77_2B_OFFSET_1KB_HISTORY,
			.history_size = 4,
			.algorithm_index = AL_CRYPT_CMPRS_LZ77_2B_OFFSET,
			.compression_optimizer_en = 1,
			.force_original_sequence_out = 0,
			.force_encoded_sequence_out = 1,
			.output_controller_index = 0,
			.concat_next_symbol = 1,
			.max_match_threshold = 255,
			.min_match_threshold = 0,
			.no_match_length = 0,
			.uncompressed_length = 0,
		},
		{
			/*LZ77 4KB History 4B Code Word Size*/
			.id = AL_CRYPT_CMPRS_LZ77_2B_OFFSET_4KB_HISTORY,
			.history_size = 16,
			.algorithm_index = AL_CRYPT_CMPRS_LZ77_2B_OFFSET,
			.compression_optimizer_en = 1,
			.force_original_sequence_out = 0,
			.force_encoded_sequence_out = 1,
			.output_controller_index = 0,
			.concat_next_symbol = 1,
			.max_match_threshold = 255,
			.min_match_threshold = 0,
			.no_match_length = 0,
			.uncompressed_length = 0,
		},
		{
			/*LZ77 8KB History 4B Code Word Size*/
			.id = AL_CRYPT_CMPRS_LZ77_2B_OFFSET_8KB_HISTORY,
			.history_size = 32,
			.algorithm_index = AL_CRYPT_CMPRS_LZ77_2B_OFFSET,
			.compression_optimizer_en = 1,
			.force_original_sequence_out = 0,
			.force_encoded_sequence_out = 1,
			.output_controller_index = 0,
			.concat_next_symbol = 1,
			.max_match_threshold = 255,
			.min_match_threshold = 0,
			.no_match_length = 0,
			.uncompressed_length = 0,
		},
		{
			/*LZ77 16KB History 4B Code Word Size*/
			.id = AL_CRYPT_CMPRS_LZ77_2B_OFFSET_16KB_HISTORY,
			.history_size = 64,
			.algorithm_index = AL_CRYPT_CMPRS_LZ77_2B_OFFSET,
			.compression_optimizer_en = 1,
			.force_original_sequence_out = 0,
			.force_encoded_sequence_out = 1,
			.output_controller_index = 0,
			.concat_next_symbol = 1,
			.max_match_threshold = 255,
			.min_match_threshold = 0,
			.no_match_length = 0,
			.uncompressed_length = 0,
		},
		{
			/*LZ77 32KB History 4B Code Word Size*/
			.id = AL_CRYPT_CMPRS_LZ77_2B_OFFSET_32KB_HISTORY,
			.history_size = 128,
			.algorithm_index = AL_CRYPT_CMPRS_LZ77_2B_OFFSET,
			.compression_optimizer_en = 1,
			.force_original_sequence_out = 0,
			.force_encoded_sequence_out = 1,
			.output_controller_index = 0,
			.concat_next_symbol = 1,
			.max_match_threshold = 255,
			.min_match_threshold = 0,
			.no_match_length = 0,
			.uncompressed_length = 0,
		},
		{
			/*LZSS 256B History 3B Code Word Size*/
			.id = AL_CRYPT_CMPRS_LZSS_1B_OFFSET_256B_HISTORY,
			.history_size = 1,
			.algorithm_index = AL_CRYPT_CMPRS_LZSS_1B_OFFSET,
			.compression_optimizer_en = 1,
			.force_original_sequence_out = 0,
			.force_encoded_sequence_out = 1,
			.output_controller_index = 0,
			.concat_next_symbol = 0,
			.max_match_threshold = 258,
			.min_match_threshold = 3,
			.no_match_length = 0,
			.uncompressed_length = 0,
		},
		{
			/*LZSS 512B History 4B Code Word Size*/
			.id = AL_CRYPT_CMPRS_LZSS_2B_OFFSET_512B_HISTORY,
			.history_size = 2,
			.algorithm_index = AL_CRYPT_CMPRS_LZSS_2B_OFFSET,
			.compression_optimizer_en = 1,
			.force_original_sequence_out = 0,
			.force_encoded_sequence_out = 1,
			.output_controller_index = 0,
			.concat_next_symbol = 0,
			.max_match_threshold = 258,
			.min_match_threshold = 3,
			.no_match_length = 0,
			.uncompressed_length = 0,
		},
		{
			/*LZSS 1KB History 4B Code Word Size*/
			.id = AL_CRYPT_CMPRS_LZSS_2B_OFFSET_1KB_HISTORY,
			.history_size = 4,
			.algorithm_index = AL_CRYPT_CMPRS_LZSS_2B_OFFSET,
			.compression_optimizer_en = 1,
			.force_original_sequence_out = 0,
			.force_encoded_sequence_out = 1,
			.output_controller_index = 0,
			.concat_next_symbol = 0,
			.max_match_threshold = 258,
			.min_match_threshold = 3,
			.no_match_length = 0,
			.uncompressed_length = 0,
		},
		{
			/*LZSS 4KB History 4B Code Word Size*/
			.id = AL_CRYPT_CMPRS_LZSS_2B_OFFSET_4KB_HISTORY,
			.history_size = 16,
			.algorithm_index = AL_CRYPT_CMPRS_LZSS_2B_OFFSET,
			.compression_optimizer_en = 1,
			.force_original_sequence_out = 0,
			.force_encoded_sequence_out = 1,
			.output_controller_index = 0,
			.concat_next_symbol = 0,
			.max_match_threshold = 258,
			.min_match_threshold = 3,
			.no_match_length = 0,
			.uncompressed_length = 0,
		},
		{
			/*LZSS 8KB History 4B Code Word Size*/
			.id = AL_CRYPT_CMPRS_LZSS_2B_OFFSET_8KB_HISTORY,
			.history_size = 32,
			.algorithm_index = AL_CRYPT_CMPRS_LZSS_2B_OFFSET,
			.compression_optimizer_en = 1,
			.force_original_sequence_out = 0,
			.force_encoded_sequence_out = 1,
			.output_controller_index = 0,
			.concat_next_symbol = 0,
			.max_match_threshold = 258,
			.min_match_threshold = 3,
			.no_match_length = 0,
			.uncompressed_length = 0,
		},
		{
			/*LZSS 16KB History 4B Code Word Size*/
			.id = AL_CRYPT_CMPRS_LZSS_2B_OFFSET_16KB_HISTORY,
			.history_size = 64,
			.algorithm_index = AL_CRYPT_CMPRS_LZSS_2B_OFFSET,
			.compression_optimizer_en = 1,
			.force_original_sequence_out = 0,
			.force_encoded_sequence_out = 1,
			.output_controller_index = 0,
			.concat_next_symbol = 0,
			.max_match_threshold = 258,
			.min_match_threshold = 3,
			.no_match_length = 0,
			.uncompressed_length = 0,
		},
		{
			/*LZSS 32KB History 4B Code Word Size*/
			.id = AL_CRYPT_CMPRS_LZSS_2B_OFFSET_32KB_HISTORY,
			.history_size = 128,
			.algorithm_index = AL_CRYPT_CMPRS_LZSS_2B_OFFSET,
			.compression_optimizer_en = 1,
			.force_original_sequence_out = 0,
			.force_encoded_sequence_out = 1,
			.output_controller_index = 0,
			.concat_next_symbol = 0,
			.max_match_threshold = 258,
			.min_match_threshold = 3,
			.no_match_length = 0,
			.uncompressed_length = 0,
		},
		{
			/*LZ4 512B History Stream*/
			.id = AL_CRYPT_CMPRS_LZ4_512B_HISTORY,
			.history_size = 2,
			.algorithm_index = AL_CRYPT_CMPRS_LZ4,
			.compression_optimizer_en = 1,
			.force_original_sequence_out = 0,
			.force_encoded_sequence_out = 1,
			.output_controller_index = 0,
			.concat_next_symbol = 0,
			.max_match_threshold = 273,
			.min_match_threshold = 4,
			.no_match_length = 12,
			.uncompressed_length = 5,
		},
		{
			/*LZ4 1KB History Stream*/
			.id = AL_CRYPT_CMPRS_LZ4_1KB_HISTORY,
			.history_size = 4,
			.algorithm_index = AL_CRYPT_CMPRS_LZ4,
			.compression_optimizer_en = 1,
			.force_original_sequence_out = 0,
			.force_encoded_sequence_out = 1,
			.output_controller_index = 0,
			.concat_next_symbol = 0,
			.max_match_threshold = 273,
			.min_match_threshold = 4,
			.no_match_length = 12,
			.uncompressed_length = 5,
		},
		{
			/*LZ4 4KB History Stream*/
			.id = AL_CRYPT_CMPRS_LZ4_4KB_HISTORY,
			.history_size = 16,
			.algorithm_index = AL_CRYPT_CMPRS_LZ4,
			.compression_optimizer_en = 1,
			.force_original_sequence_out = 0,
			.force_encoded_sequence_out = 1,
			.output_controller_index = 0,
			.concat_next_symbol = 0,
			.max_match_threshold = 273,
			.min_match_threshold = 4,
			.no_match_length = 12,
			.uncompressed_length = 5,
		},
		{
			/*LZ4 8KB History Stream*/
			.id = AL_CRYPT_CMPRS_LZ4_8KB_HISTORY,
			.history_size = 32,
			.algorithm_index = AL_CRYPT_CMPRS_LZ4,
			.compression_optimizer_en = 1,
			.force_original_sequence_out = 0,
			.force_encoded_sequence_out = 1,
			.output_controller_index = 0,
			.concat_next_symbol = 0,
			.max_match_threshold = 273,
			.min_match_threshold = 4,
			.no_match_length = 12,
			.uncompressed_length = 5,
		},
		{
			/*LZ4 16KB History Stream*/
			.id = AL_CRYPT_CMPRS_LZ4_16KB_HISTORY,
			.history_size = 64,
			.algorithm_index = AL_CRYPT_CMPRS_LZ4,
			.compression_optimizer_en = 1,
			.force_original_sequence_out = 0,
			.force_encoded_sequence_out = 1,
			.output_controller_index = 0,
			.concat_next_symbol = 0,
			.max_match_threshold = 273,
			.min_match_threshold = 4,
			.no_match_length = 12,
			.uncompressed_length = 5,
		},
		{
			/*LZ4 32KB History Stream*/
			.id = AL_CRYPT_CMPRS_LZ4_32KB_HISTORY,
			.history_size = 128,
			.algorithm_index = AL_CRYPT_CMPRS_LZ4,
			.compression_optimizer_en = 1,
			.force_original_sequence_out = 0,
			.force_encoded_sequence_out = 1,
			.output_controller_index = 0,
			.concat_next_symbol = 0,
			.max_match_threshold = 273,
			.min_match_threshold = 4,
			.no_match_length = 12,
			.uncompressed_length = 5,
		},
		{
			/*Static Deflate 512B History Stream*/
			.id = AL_CRYPT_CMPRS_STATIC_DEFLATE_512B_HISTORY,
			.history_size = 2,
			.algorithm_index = AL_CRYPT_CMPRS_DEFLATE,
			.compression_optimizer_en = 1,
			.force_original_sequence_out = 0,
			.force_encoded_sequence_out = 1,
			.output_controller_index = 0,
			.concat_next_symbol = 0,
			.max_match_threshold = 258,
			.min_match_threshold = 3,
			.no_match_length = 0,
			.uncompressed_length = 0,
		},
		{
			/*Static Deflate 1KB History Stream*/
			.id = AL_CRYPT_CMPRS_STATIC_DEFLATE_1KB_HISTORY,
			.history_size = 4,
			.algorithm_index = AL_CRYPT_CMPRS_DEFLATE,
			.compression_optimizer_en = 1,
			.force_original_sequence_out = 0,
			.force_encoded_sequence_out = 1,
			.output_controller_index = 0,
			.concat_next_symbol = 0,
			.max_match_threshold = 258,
			.min_match_threshold = 3,
			.no_match_length = 0,
			.uncompressed_length = 0,
		},
		{
			/*Static Deflate 4KB History Stream*/
			.id = AL_CRYPT_CMPRS_STATIC_DEFLATE_4KB_HISTORY,
			.history_size = 16,
			.algorithm_index = AL_CRYPT_CMPRS_DEFLATE,
			.compression_optimizer_en = 1,
			.force_original_sequence_out = 0,
			.force_encoded_sequence_out = 1,
			.output_controller_index = 0,
			.concat_next_symbol = 0,
			.max_match_threshold = 258,
			.min_match_threshold = 3,
			.no_match_length = 0,
			.uncompressed_length = 0,
		},
		{
			/*Static Deflate 8KB History Stream*/
			.id = AL_CRYPT_CMPRS_STATIC_DEFLATE_8KB_HISTORY,
			.history_size = 32,
			.algorithm_index = AL_CRYPT_CMPRS_DEFLATE,
			.compression_optimizer_en = 1,
			.force_original_sequence_out = 0,
			.force_encoded_sequence_out = 1,
			.output_controller_index = 0,
			.concat_next_symbol = 0,
			.max_match_threshold = 258,
			.min_match_threshold = 3,
			.no_match_length = 0,
			.uncompressed_length = 0,
		},
		{
			/*Static Deflate 16KB History Stream*/
			.id = AL_CRYPT_CMPRS_STATIC_DEFLATE_16KB_HISTORY,
			.history_size = 64,
			.algorithm_index = AL_CRYPT_CMPRS_DEFLATE,
			.compression_optimizer_en = 1,
			.force_original_sequence_out = 0,
			.force_encoded_sequence_out = 1,
			.output_controller_index = 0,
			.concat_next_symbol = 0,
			.max_match_threshold = 258,
			.min_match_threshold = 3,
			.no_match_length = 0,
			.uncompressed_length = 0,
		},
		{
			/*Static Deflate 32KB History Stream*/
			.id = AL_CRYPT_CMPRS_STATIC_DEFLATE_32KB_HISTORY,
			.history_size = 128,
			.algorithm_index = AL_CRYPT_CMPRS_DEFLATE,
			.compression_optimizer_en = 1,
			.force_original_sequence_out = 0,
			.force_encoded_sequence_out = 1,
			.output_controller_index = 0,
			.concat_next_symbol = 0,
			.max_match_threshold = 258,
			.min_match_threshold = 3,
			.no_match_length = 0,
			.uncompressed_length = 0,
		},
	};

/** Compression decoder profile table initial entries */
static const struct al_crypto_cmprs_dec_prof_tbl_ent dec_prof_tbl_entries[] = {
		{
			/*LZ77 256B History 3B Code Word Size*/
			.id = AL_CRYPT_CMPRS_LZ77_1B_OFFSET_256B_HISTORY,
			.history_size = 1,
			.match_length_base = 0,
			.force_original_sequence_out = 0,
			.compressed_flag_polarity = 0,
			.compressed_flag_en = 0,
			.compressed_flag_coalescing = 0,
			.field_offset_size = 0,
			.field_length_size = 0,
			.field_composition = 0,
			.algorithm_type = AL_CRYPT_CMPRS_LZ_CODE_WORD,
			.concat_next_symbol = 1,
			.offset_base = 1,
			.offset_endianity = 0,
			.huffman_extra_bits_swap = 0,
			.bit_swap = 0,
		},
		{
			/*LZ77 512B History 4B Code Word Size*/
			.id = AL_CRYPT_CMPRS_LZ77_2B_OFFSET_512B_HISTORY,
			.history_size = 2,
			.match_length_base = 0,
			.force_original_sequence_out = 0,
			.compressed_flag_polarity = 0,
			.compressed_flag_en = 0,
			.compressed_flag_coalescing = 0,
			.field_offset_size = 1,
			.field_length_size = 0,
			.field_composition = 0,
			.algorithm_type = AL_CRYPT_CMPRS_LZ_CODE_WORD,
			.concat_next_symbol = 1,
			.offset_base = 1,
			.offset_endianity = 0,
			.huffman_extra_bits_swap = 0,
			.bit_swap = 0,
		},
		{
			/*LZ77 1KB History 4B Code Word Size*/
			.id = AL_CRYPT_CMPRS_LZ77_2B_OFFSET_1KB_HISTORY,
			.history_size = 4,
			.match_length_base = 0,
			.force_original_sequence_out = 0,
			.compressed_flag_polarity = 0,
			.compressed_flag_en = 0,
			.compressed_flag_coalescing = 0,
			.field_offset_size = 1,
			.field_length_size = 0,
			.field_composition = 0,
			.algorithm_type = AL_CRYPT_CMPRS_LZ_CODE_WORD,
			.concat_next_symbol = 1,
			.offset_base = 1,
			.offset_endianity = 0,
			.huffman_extra_bits_swap = 0,
			.bit_swap = 0,
		},
		{
			/*LZ77 4KB History 4B Code Word Size*/
			.id = AL_CRYPT_CMPRS_LZ77_2B_OFFSET_4KB_HISTORY,
			.history_size = 16,
			.match_length_base = 0,
			.force_original_sequence_out = 0,
			.compressed_flag_polarity = 0,
			.compressed_flag_en = 0,
			.compressed_flag_coalescing = 0,
			.field_offset_size = 1,
			.field_length_size = 0,
			.field_composition = 0,
			.algorithm_type = AL_CRYPT_CMPRS_LZ_CODE_WORD,
			.concat_next_symbol = 1,
			.offset_base = 1,
			.offset_endianity = 0,
			.huffman_extra_bits_swap = 0,
			.bit_swap = 0,
		},
		{
			/*LZ77 4KB History 8B Code Word Size*/
			.id = AL_CRYPT_CMPRS_LZ77_2B_OFFSET_8KB_HISTORY,
			.history_size = 32,
			.match_length_base = 0,
			.force_original_sequence_out = 0,
			.compressed_flag_polarity = 0,
			.compressed_flag_en = 0,
			.compressed_flag_coalescing = 0,
			.field_offset_size = 1,
			.field_length_size = 0,
			.field_composition = 0,
			.algorithm_type = AL_CRYPT_CMPRS_LZ_CODE_WORD,
			.concat_next_symbol = 1,
			.offset_base = 1,
			.offset_endianity = 0,
			.huffman_extra_bits_swap = 0,
			.bit_swap = 0,
		},
		{
			/*LZ77 4KB History 8B Code Word Size*/
			.id = AL_CRYPT_CMPRS_LZ77_2B_OFFSET_16KB_HISTORY,
			.history_size = 64,
			.match_length_base = 0,
			.force_original_sequence_out = 0,
			.compressed_flag_polarity = 0,
			.compressed_flag_en = 0,
			.compressed_flag_coalescing = 0,
			.field_offset_size = 1,
			.field_length_size = 0,
			.field_composition = 0,
			.algorithm_type = AL_CRYPT_CMPRS_LZ_CODE_WORD,
			.concat_next_symbol = 1,
			.offset_base = 1,
			.offset_endianity = 0,
			.huffman_extra_bits_swap = 0,
			.bit_swap = 0,
		},
		{
			/*LZ77 32KB History 4B Code Word Size*/
			.id = AL_CRYPT_CMPRS_LZ77_2B_OFFSET_32KB_HISTORY,
			.history_size = 128,
			.match_length_base = 0,
			.force_original_sequence_out = 0,
			.compressed_flag_polarity = 0,
			.compressed_flag_en = 0,
			.compressed_flag_coalescing = 0,
			.field_offset_size = 1,
			.field_length_size = 0,
			.field_composition = 0,
			.algorithm_type = AL_CRYPT_CMPRS_LZ_CODE_WORD,
			.concat_next_symbol = 1,
			.offset_base = 1,
			.offset_endianity = 0,
			.huffman_extra_bits_swap = 0,
			.bit_swap = 0,
		},
		{
			/*LZSS 256B History 3B Code Word Size*/
			.id = AL_CRYPT_CMPRS_LZSS_1B_OFFSET_256B_HISTORY,
			.history_size = 1,
			.match_length_base = 3,
			.force_original_sequence_out = 0,
			.compressed_flag_polarity = 0,
			.compressed_flag_en = 1,
			.compressed_flag_coalescing = 1,
			.field_offset_size = 0,
			.field_length_size = 0,
			.field_composition = 0,
			.algorithm_type = AL_CRYPT_CMPRS_LZ_CODE_WORD,
			.concat_next_symbol = 0,
			.offset_base = 1,
			.offset_endianity = 0,
			.huffman_extra_bits_swap = 0,
			.bit_swap = 0,
		},
		{
			/*LZSS 512B History 4B Code Word Size*/
			.id = AL_CRYPT_CMPRS_LZSS_2B_OFFSET_512B_HISTORY,
			.history_size = 2,
			.match_length_base = 3,
			.force_original_sequence_out = 0,
			.compressed_flag_polarity = 0,
			.compressed_flag_en = 1,
			.compressed_flag_coalescing = 1,
			.field_offset_size = 1,
			.field_length_size = 0,
			.field_composition = 0,
			.algorithm_type = AL_CRYPT_CMPRS_LZ_CODE_WORD,
			.concat_next_symbol = 0,
			.offset_base = 1,
			.offset_endianity = 0,
			.huffman_extra_bits_swap = 0,
			.bit_swap = 0,
		},
		{
			/*LZSS 1KB History 4B Code Word Size*/
			.id = AL_CRYPT_CMPRS_LZSS_2B_OFFSET_1KB_HISTORY,
			.history_size = 4,
			.match_length_base = 3,
			.force_original_sequence_out = 0,
			.compressed_flag_polarity = 0,
			.compressed_flag_en = 1,
			.compressed_flag_coalescing = 1,
			.field_offset_size = 1,
			.field_length_size = 0,
			.field_composition = 0,
			.algorithm_type = AL_CRYPT_CMPRS_LZ_CODE_WORD,
			.concat_next_symbol = 0,
			.offset_base = 1,
			.offset_endianity = 0,
			.huffman_extra_bits_swap = 0,
			.bit_swap = 0,
		},
		{
			/*LZSS 4KB History 4B Code Word Size*/
			.id = AL_CRYPT_CMPRS_LZSS_2B_OFFSET_4KB_HISTORY,
			.history_size = 16,
			.match_length_base = 3,
			.force_original_sequence_out = 0,
			.compressed_flag_polarity = 0,
			.compressed_flag_en = 1,
			.compressed_flag_coalescing = 1,
			.field_offset_size = 1,
			.field_length_size = 0,
			.field_composition = 0,
			.algorithm_type = AL_CRYPT_CMPRS_LZ_CODE_WORD,
			.concat_next_symbol = 0,
			.offset_base = 1,
			.offset_endianity = 0,
			.huffman_extra_bits_swap = 0,
			.bit_swap = 0,
		},
		{
			/*LZSS 8KB History 4B Code Word Size*/
			.id = AL_CRYPT_CMPRS_LZSS_2B_OFFSET_8KB_HISTORY,
			.history_size = 32,
			.match_length_base = 3,
			.force_original_sequence_out = 0,
			.compressed_flag_polarity = 0,
			.compressed_flag_en = 1,
			.compressed_flag_coalescing = 1,
			.field_offset_size = 1,
			.field_length_size = 0,
			.field_composition = 0,
			.algorithm_type = AL_CRYPT_CMPRS_LZ_CODE_WORD,
			.concat_next_symbol = 0,
			.offset_base = 1,
			.offset_endianity = 0,
			.huffman_extra_bits_swap = 0,
			.bit_swap = 0,
		},
		{
			/*LZSS 16KB History 4B Code Word Size*/
			.id = AL_CRYPT_CMPRS_LZSS_2B_OFFSET_16KB_HISTORY,
			.history_size = 64,
			.match_length_base = 3,
			.force_original_sequence_out = 0,
			.compressed_flag_polarity = 0,
			.compressed_flag_en = 1,
			.compressed_flag_coalescing = 1,
			.field_offset_size = 1,
			.field_length_size = 0,
			.field_composition = 0,
			.algorithm_type = AL_CRYPT_CMPRS_LZ_CODE_WORD,
			.concat_next_symbol = 0,
			.offset_base = 1,
			.offset_endianity = 0,
			.huffman_extra_bits_swap = 0,
			.bit_swap = 0,
		},
		{
			/*LZSS 32KB History 4B Code Word Size*/
			.id = AL_CRYPT_CMPRS_LZSS_2B_OFFSET_32KB_HISTORY,
			.history_size = 128,
			.match_length_base = 3,
			.force_original_sequence_out = 0,
			.compressed_flag_polarity = 0,
			.compressed_flag_en = 1,
			.compressed_flag_coalescing = 1,
			.field_offset_size = 1,
			.field_length_size = 0,
			.field_composition = 0,
			.algorithm_type = AL_CRYPT_CMPRS_LZ_CODE_WORD,
			.concat_next_symbol = 0,
			.offset_base = 1,
			.offset_endianity = 0,
			.huffman_extra_bits_swap = 0,
			.bit_swap = 0,
		},
		{
			/*LZ4 512B History Stream*/
			.id = AL_CRYPT_CMPRS_LZ4_512B_HISTORY,
			.history_size = 2,
			.match_length_base = 4,
			.force_original_sequence_out = 0,
			.compressed_flag_polarity = 0,
			.compressed_flag_en = 0,
			.compressed_flag_coalescing = 0,
			.field_offset_size = 0,
			.field_length_size = 0,
			.field_composition = 0,
			.algorithm_type = AL_CRYPT_CMPRS_LZ_STREAM,
			.concat_next_symbol = 0,
			.offset_base = 1,
			.offset_endianity = 1,
			.huffman_extra_bits_swap = 0,
			.bit_swap = 0,
		},
		{
			/*LZ4 1KB History Stream*/
			.id = AL_CRYPT_CMPRS_LZ4_1KB_HISTORY,
			.history_size = 4,
			.match_length_base = 4,
			.force_original_sequence_out = 0,
			.compressed_flag_polarity = 0,
			.compressed_flag_en = 0,
			.compressed_flag_coalescing = 0,
			.field_offset_size = 0,
			.field_length_size = 0,
			.field_composition = 0,
			.algorithm_type = AL_CRYPT_CMPRS_LZ_STREAM,
			.concat_next_symbol = 0,
			.offset_base = 1,
			.offset_endianity = 1,
			.huffman_extra_bits_swap = 0,
			.bit_swap = 0,
		},
		{
			/*LZ4 4KB History Stream*/
			.id = AL_CRYPT_CMPRS_LZ4_4KB_HISTORY,
			.history_size = 16,
			.match_length_base = 4,
			.force_original_sequence_out = 0,
			.compressed_flag_polarity = 0,
			.compressed_flag_en = 0,
			.compressed_flag_coalescing = 0,
			.field_offset_size = 0,
			.field_length_size = 0,
			.field_composition = 0,
			.algorithm_type = AL_CRYPT_CMPRS_LZ_STREAM,
			.concat_next_symbol = 0,
			.offset_base = 1,
			.offset_endianity = 1,
			.huffman_extra_bits_swap = 0,
			.bit_swap = 0,
		},
		{
			/*LZ4 8KB History Stream*/
			.id = AL_CRYPT_CMPRS_LZ4_8KB_HISTORY,
			.history_size = 32,
			.match_length_base = 4,
			.force_original_sequence_out = 0,
			.compressed_flag_polarity = 0,
			.compressed_flag_en = 0,
			.compressed_flag_coalescing = 0,
			.field_offset_size = 0,
			.field_length_size = 0,
			.field_composition = 0,
			.algorithm_type = AL_CRYPT_CMPRS_LZ_STREAM,
			.concat_next_symbol = 0,
			.offset_base = 1,
			.offset_endianity = 1,
			.huffman_extra_bits_swap = 0,
			.bit_swap = 0,
		},
		{
			/*LZ4 16KB History Stream*/
			.id = AL_CRYPT_CMPRS_LZ4_16KB_HISTORY,
			.history_size = 64,
			.match_length_base = 4,
			.force_original_sequence_out = 0,
			.compressed_flag_polarity = 0,
			.compressed_flag_en = 0,
			.compressed_flag_coalescing = 0,
			.field_offset_size = 0,
			.field_length_size = 0,
			.field_composition = 0,
			.algorithm_type = AL_CRYPT_CMPRS_LZ_STREAM,
			.concat_next_symbol = 0,
			.offset_base = 1,
			.offset_endianity = 1,
			.huffman_extra_bits_swap = 0,
			.bit_swap = 0,
		},
		{
			/*LZ4 32KB History Stream*/
			.id = AL_CRYPT_CMPRS_LZ4_32KB_HISTORY,
			.history_size = 128,
			.match_length_base = 4,
			.force_original_sequence_out = 0,
			.compressed_flag_polarity = 0,
			.compressed_flag_en = 0,
			.compressed_flag_coalescing = 0,
			.field_offset_size = 0,
			.field_length_size = 0,
			.field_composition = 0,
			.algorithm_type = AL_CRYPT_CMPRS_LZ_STREAM,
			.concat_next_symbol = 0,
			.offset_base = 1,
			.offset_endianity = 1,
			.huffman_extra_bits_swap = 0,
			.bit_swap = 0,
		},
		{
			/*Static Deflate 512B History Stream*/
			.id = AL_CRYPT_CMPRS_STATIC_DEFLATE_512B_HISTORY,
			.history_size = 2,
			.match_length_base = 3,
			.force_original_sequence_out = 0,
			.compressed_flag_polarity = 0,
			.compressed_flag_en = 0,
			.compressed_flag_coalescing = 0,
			.field_offset_size = 0,
			.field_length_size = 0,
			.field_composition = 0,
			.algorithm_type = AL_CRYPT_CMPRS_STATIC_DEFLATE,
			.concat_next_symbol = 0,
			.offset_base = 1,
			.offset_endianity = 0,
			.huffman_extra_bits_swap = 1,
			.bit_swap = 1,
		},
		{
			/*Static Deflate 1KB History Stream*/
			.id = AL_CRYPT_CMPRS_STATIC_DEFLATE_1KB_HISTORY,
			.history_size = 4,
			.match_length_base = 3,
			.force_original_sequence_out = 0,
			.compressed_flag_polarity = 0,
			.compressed_flag_en = 0,
			.compressed_flag_coalescing = 0,
			.field_offset_size = 0,
			.field_length_size = 0,
			.field_composition = 0,
			.algorithm_type = AL_CRYPT_CMPRS_STATIC_DEFLATE,
			.concat_next_symbol = 0,
			.offset_base = 1,
			.offset_endianity = 0,
			.huffman_extra_bits_swap = 1,
			.bit_swap = 1,
		},
		{
			/*Static Deflate 4KB History Stream*/
			.id = AL_CRYPT_CMPRS_STATIC_DEFLATE_4KB_HISTORY,
			.history_size = 16,
			.match_length_base = 3,
			.force_original_sequence_out = 0,
			.compressed_flag_polarity = 0,
			.compressed_flag_en = 0,
			.compressed_flag_coalescing = 0,
			.field_offset_size = 0,
			.field_length_size = 0,
			.field_composition = 0,
			.algorithm_type = AL_CRYPT_CMPRS_STATIC_DEFLATE,
			.concat_next_symbol = 0,
			.offset_base = 1,
			.offset_endianity = 0,
			.huffman_extra_bits_swap = 1,
			.bit_swap = 1,
		},
		{
			/*Static Deflate 8KB History Stream*/
			.id = AL_CRYPT_CMPRS_STATIC_DEFLATE_8KB_HISTORY,
			.history_size = 32,
			.match_length_base = 3,
			.force_original_sequence_out = 0,
			.compressed_flag_polarity = 0,
			.compressed_flag_en = 0,
			.compressed_flag_coalescing = 0,
			.field_offset_size = 0,
			.field_length_size = 0,
			.field_composition = 0,
			.algorithm_type = AL_CRYPT_CMPRS_STATIC_DEFLATE,
			.concat_next_symbol = 0,
			.offset_base = 1,
			.offset_endianity = 0,
			.huffman_extra_bits_swap = 1,
			.bit_swap = 1,
		},
		{
			/*Static Deflate 16KB History Stream*/
			.id = AL_CRYPT_CMPRS_STATIC_DEFLATE_16KB_HISTORY,
			.history_size = 64,
			.match_length_base = 3,
			.force_original_sequence_out = 0,
			.compressed_flag_polarity = 0,
			.compressed_flag_en = 0,
			.compressed_flag_coalescing = 0,
			.field_offset_size = 0,
			.field_length_size = 0,
			.field_composition = 0,
			.algorithm_type = AL_CRYPT_CMPRS_STATIC_DEFLATE,
			.concat_next_symbol = 0,
			.offset_base = 1,
			.offset_endianity = 0,
			.huffman_extra_bits_swap = 1,
			.bit_swap = 1,
		},
		{
			/*Static Deflate 32KB History Stream*/
			.id = AL_CRYPT_CMPRS_STATIC_DEFLATE_32KB_HISTORY,
			.history_size = 128,
			.match_length_base = 3,
			.force_original_sequence_out = 0,
			.compressed_flag_polarity = 0,
			.compressed_flag_en = 0,
			.compressed_flag_coalescing = 0,
			.field_offset_size = 0,
			.field_length_size = 0,
			.field_composition = 0,
			.algorithm_type = AL_CRYPT_CMPRS_STATIC_DEFLATE,
			.concat_next_symbol = 0,
			.offset_base = 1,
			.offset_endianity = 0,
			.huffman_extra_bits_swap = 1,
			.bit_swap = 1,
		},
		{
			/*Dynamic Deflate 512B History Stream*/
			.id = AL_CRYPT_CMPRS_DYNAMIC_DEFLATE_512B_HISTORY,
			.history_size = 2,
			.match_length_base = 3,
			.force_original_sequence_out = 0,
			.compressed_flag_polarity = 0,
			.compressed_flag_en = 0,
			.compressed_flag_coalescing = 0,
			.field_offset_size = 0,
			.field_length_size = 0,
			.field_composition = 0,
			.algorithm_type = AL_CRYPT_CMPRS_DYNAMIC_DEFLATE,
			.concat_next_symbol = 0,
			.offset_base = 1,
			.offset_endianity = 0,
			.huffman_extra_bits_swap = 1,
			.bit_swap = 1,
		},
		{
			/*Dynamic Deflate 1KB History Stream*/
			.id = AL_CRYPT_CMPRS_DYNAMIC_DEFLATE_1KB_HISTORY,
			.history_size = 4,
			.match_length_base = 3,
			.force_original_sequence_out = 0,
			.compressed_flag_polarity = 0,
			.compressed_flag_en = 0,
			.compressed_flag_coalescing = 0,
			.field_offset_size = 0,
			.field_length_size = 0,
			.field_composition = 0,
			.algorithm_type = AL_CRYPT_CMPRS_DYNAMIC_DEFLATE,
			.concat_next_symbol = 0,
			.offset_base = 1,
			.offset_endianity = 0,
			.huffman_extra_bits_swap = 1,
			.bit_swap = 1,
		},
		{
			/*Dynamic Deflate 4KB History Stream*/
			.id = AL_CRYPT_CMPRS_DYNAMIC_DEFLATE_4KB_HISTORY,
			.history_size = 16,
			.match_length_base = 3,
			.force_original_sequence_out = 0,
			.compressed_flag_polarity = 0,
			.compressed_flag_en = 0,
			.compressed_flag_coalescing = 0,
			.field_offset_size = 0,
			.field_length_size = 0,
			.field_composition = 0,
			.algorithm_type = AL_CRYPT_CMPRS_DYNAMIC_DEFLATE,
			.concat_next_symbol = 0,
			.offset_base = 1,
			.offset_endianity = 0,
			.huffman_extra_bits_swap = 1,
			.bit_swap = 1,
		},
		{
			/*Dynamic Deflate 8KB History Stream*/
			.id = AL_CRYPT_CMPRS_DYNAMIC_DEFLATE_8KB_HISTORY,
			.history_size = 32,
			.match_length_base = 3,
			.force_original_sequence_out = 0,
			.compressed_flag_polarity = 0,
			.compressed_flag_en = 0,
			.compressed_flag_coalescing = 0,
			.field_offset_size = 0,
			.field_length_size = 0,
			.field_composition = 0,
			.algorithm_type = AL_CRYPT_CMPRS_DYNAMIC_DEFLATE,
			.concat_next_symbol = 0,
			.offset_base = 1,
			.offset_endianity = 0,
			.huffman_extra_bits_swap = 1,
			.bit_swap = 1,
		},
		{
			/*Dynamic Deflate 16KB History Stream*/
			.id = AL_CRYPT_CMPRS_DYNAMIC_DEFLATE_16KB_HISTORY,
			.history_size = 64,
			.match_length_base = 3,
			.force_original_sequence_out = 0,
			.compressed_flag_polarity = 0,
			.compressed_flag_en = 0,
			.compressed_flag_coalescing = 0,
			.field_offset_size = 0,
			.field_length_size = 0,
			.field_composition = 0,
			.algorithm_type = AL_CRYPT_CMPRS_DYNAMIC_DEFLATE,
			.concat_next_symbol = 0,
			.offset_base = 1,
			.offset_endianity = 0,
			.huffman_extra_bits_swap = 1,
			.bit_swap = 1,
		},
		{
			/*Dynamic Deflate 32KB History Stream*/
			.id = AL_CRYPT_CMPRS_DYNAMIC_DEFLATE_32KB_HISTORY,
			.history_size = 128,
			.match_length_base = 3,
			.force_original_sequence_out = 0,
			.compressed_flag_polarity = 0,
			.compressed_flag_en = 0,
			.compressed_flag_coalescing = 0,
			.field_offset_size = 0,
			.field_length_size = 0,
			.field_composition = 0,
			.algorithm_type = AL_CRYPT_CMPRS_DYNAMIC_DEFLATE,
			.concat_next_symbol = 0,
			.offset_base = 1,
			.offset_endianity = 0,
			.huffman_extra_bits_swap = 1,
			.bit_swap = 1,
		},
	};

/** Compression algorithm table initial entries */
static const struct al_crypto_cmprs_alg_tbl_ent alg_tbl_entries[] = {
		{
			/*LZ77 3B Code Word Algorithm*/
			.id = AL_CRYPT_CMPRS_LZ77_1B_OFFSET,
			.flags_offset = 0,
			.flags_pos_0 = 0,
			.flags_pos_1 = 0,
			.flags_pos_2 = 0,
			.flags_pos_3 = 0,
			.flags_cfg_polarity = 0,
			.flags_cfg_en = 0,
			.algorithm_type = AL_CRYPT_CMPRS_LZ_CODE_WORD,
			.offset_base = 1,
			.offset_endianity = 0,
			.huffman_extra_bits_swap = 0,
			.bit_swap = 0,
		},
		{
			/*LZ77 4B Code Word Algorithm*/
			.id = AL_CRYPT_CMPRS_LZ77_2B_OFFSET,
			.flags_offset = 0,
			.flags_pos_0 = 0,
			.flags_pos_1 = 0,
			.flags_pos_2 = 0,
			.flags_pos_3 = 0,
			.flags_cfg_polarity = 0,
			.flags_cfg_en = 0,
			.algorithm_type = AL_CRYPT_CMPRS_LZ_CODE_WORD,
			.offset_base = 1,
			.offset_endianity = 0,
			.huffman_extra_bits_swap = 0,
			.bit_swap = 0,
		},
		{
			/*LZSS 3B Code Word Algorithm*/
			.id = AL_CRYPT_CMPRS_LZSS_1B_OFFSET,
			.flags_offset = 0x00010203,
			.flags_pos_0 = 0xFFFFFF00, /* count all previous valid symbols */
			.flags_pos_1 = 0xFFFF0000,
			.flags_pos_2 = 0xFF000000,
			.flags_pos_3 = 0x00000000,
			.flags_cfg_polarity = 1,
			.flags_cfg_en = 1,
			.algorithm_type = AL_CRYPT_CMPRS_LZ_CODE_WORD,
			.offset_base = 1,
			.offset_endianity = 0,
			.huffman_extra_bits_swap = 0,
			.bit_swap = 0,
		},
		{
			/*LZSS 4B Code Word Algorithm*/
			.id = AL_CRYPT_CMPRS_LZSS_2B_OFFSET,
			.flags_offset = 0x00010203,
			.flags_pos_0 = 0xFFFFFF00, /* count all previous valid symbols */
			.flags_pos_1 = 0xFFFF0000,
			.flags_pos_2 = 0xFF000000,
			.flags_pos_3 = 0x00000000,
			.flags_cfg_polarity = 1,
			.flags_cfg_en = 1,
			.algorithm_type = AL_CRYPT_CMPRS_LZ_CODE_WORD,
			.offset_base = 1,
			.offset_endianity = 0,
			.huffman_extra_bits_swap = 0,
			.bit_swap = 0,
		},
		{
			/*LZ4 Stream Algorithm*/
			.id = AL_CRYPT_CMPRS_LZ4,
			.flags_offset = 0,
			.flags_pos_0 = 0,
			.flags_pos_1 = 0,
			.flags_pos_2 = 0,
			.flags_pos_3 = 0,
			.flags_cfg_polarity = 0,
			.flags_cfg_en = 0,
			.algorithm_type = AL_CRYPT_CMPRS_LZ_STREAM,
			.offset_base = 1,
			.offset_endianity = 1,
			.huffman_extra_bits_swap = 0,
			.bit_swap = 0,
		},
		{
			/*Static Deflate Algorithm*/
			.id = AL_CRYPT_CMPRS_DEFLATE,
			.flags_offset = 0,
			.flags_pos_0 = 0,
			.flags_pos_1 = 0,
			.flags_pos_2 = 0,
			.flags_pos_3 = 0,
			.flags_cfg_polarity = 0,
			.flags_cfg_en = 0,
			.algorithm_type = AL_CRYPT_CMPRS_STATIC_DEFLATE,
			.offset_base = 1,
			.offset_endianity = 0,
			.huffman_extra_bits_swap = 1,
			.bit_swap = 1,
		},
	};

/** Compression CAM table initial entries */
static const struct al_crypto_cmprs_cam_tbl_ent cam_tbl_entries[] = {
		{
			/*LZ77 3B Code Word*/
			.id = AL_CRYPT_CMPRS_LZ77_1B_OFFSET_NO_FLAGS,
			.format_table_index = AL_CRYPT_CMPRS_CW_3B_OFFSET_LENGTH_SYMBOL,
			.algorithm_index = AL_CRYPT_CMPRS_LZ77_1B_OFFSET,
			.compressed_symbol_en = 0,
			.compressed_symbol = 0,
			.match_length_offset_sel = 1,
			.entry_valid = 1,
			.min_code_threshold = 0,
			.max_code_threshold = 255,
		},
		{
			/*LZ77 4B Code Word*/
			.id = AL_CRYPT_CMPRS_LZ77_2B_OFFSET_NO_FLAGS,
			.format_table_index = AL_CRYPT_CMPRS_CW_4B_OFFSET_LENGTH_SYMBOL,
			.algorithm_index = AL_CRYPT_CMPRS_LZ77_2B_OFFSET,
			.compressed_symbol_en = 0,
			.compressed_symbol = 0,
			.match_length_offset_sel = 1,
			.entry_valid = 1,
			.min_code_threshold = 0,
			.max_code_threshold = 255,
		},
		{
			/*LZSS 2B Code Word*/
			.id = AL_CRYPT_CMPRS_LZSS_1B_OFFSET_COMPRESSED,
			.format_table_index = AL_CRYPT_CMPRS_CW_2B_OFFSET_LENGTH,
			.algorithm_index = AL_CRYPT_CMPRS_LZSS_1B_OFFSET,
			.compressed_symbol_en = 1,
			.compressed_symbol = 1,
			.match_length_offset_sel = 1,
			.entry_valid = 1,
			.min_code_threshold = 0,
			.max_code_threshold = 255,
		},
		{
			/*LZSS 1B Code Word*/
			.id = AL_CRYPT_CMPRS_LZSS_1B_OFFSET_UNCOMPRESSED,
			.format_table_index = AL_CRYPT_CMPRS_CW_1B_SYMBOL,
			.algorithm_index = AL_CRYPT_CMPRS_LZSS_1B_OFFSET,
			.compressed_symbol_en = 1,
			.compressed_symbol = 0,
			.match_length_offset_sel = 1,
			.entry_valid = 1,
			.min_code_threshold = 0,
			.max_code_threshold = 255,
		},
		{
			/*LZSS 3B Code Word*/
			.id = AL_CRYPT_CMPRS_LZSS_2B_OFFSET_COMPRESSED,
			.format_table_index = AL_CRYPT_CMPRS_CW_3B_OFFSET_LENGTH,
			.algorithm_index = AL_CRYPT_CMPRS_LZSS_2B_OFFSET,
			.compressed_symbol_en = 1,
			.compressed_symbol = 1,
			.match_length_offset_sel = 1,
			.entry_valid = 1,
			.min_code_threshold = 0,
			.max_code_threshold = 255,
		},
		{
			/*LZSS 1B Code Word*/
			.id = AL_CRYPT_CMPRS_LZSS_2B_OFFSET_UNCOMPRESSED,
			.format_table_index = AL_CRYPT_CMPRS_CW_1B_SYMBOL,
			.algorithm_index = AL_CRYPT_CMPRS_LZSS_2B_OFFSET,
			.compressed_symbol_en = 1,
			.compressed_symbol = 0,
			.match_length_offset_sel = 1,
			.entry_valid = 1,
			.min_code_threshold = 0,
			.max_code_threshold = 255,
		},
	};

/** Compression format table initial entries */
static const struct al_crypto_cmprs_fmt_tbl_ent fmt_tbl_entries[] = {
		{
			/*LZSS 1B Code Word*/
			.id = AL_CRYPT_CMPRS_CW_1B_SYMBOL,
			.code_word_offset = 0x01020304,
			.alu_0_input_symbol = 24,
			.alu_1_input_symbol = 0,
			.alu_0_input_length = 0,
			.alu_1_input_length = 0,
			.alu_0_input_offset = 0,
			.alu_1_input_offset = 0,
			.alu_0_command_symbol = 3,
			.alu_1_command_symbol = 0,
			.alu_0_command_length = 1,
			.alu_1_command_length = 0,
			.alu_0_command_offset = 1,
			.alu_1_command_offset = 0,
			.field_byte_count = 1,
		},
		{
			/*LZSS 2B Code Word*/
			.id = AL_CRYPT_CMPRS_CW_2B_OFFSET_LENGTH,
			.code_word_offset = 0x01020304,
			.alu_0_input_symbol = 0,
			.alu_1_input_symbol = 0,
			.alu_0_input_length = 16,
			.alu_1_input_length = 0,
			.alu_0_input_offset = 24,
			.alu_1_input_offset = 0,
			.alu_0_command_symbol = 1,
			.alu_1_command_symbol = 0,
			.alu_0_command_length = 3,
			.alu_1_command_length = 0,
			.alu_0_command_offset = 3,
			.alu_1_command_offset = 0,
			.field_byte_count = 2,
		},
		{
			/*LZSS 3B Code Word*/
			.id = AL_CRYPT_CMPRS_CW_3B_OFFSET_LENGTH,
			.code_word_offset = 0x01020304,
			.alu_0_input_symbol = 0,
			.alu_1_input_symbol = 0,
			.alu_0_input_length = 8,
			.alu_1_input_length = 0,
			.alu_0_input_offset = 16,
			.alu_1_input_offset = 0,
			.alu_0_command_symbol = 1,
			.alu_1_command_symbol = 0,
			.alu_0_command_length = 3,
			.alu_1_command_length = 0,
			.alu_0_command_offset = 3,
			.alu_1_command_offset = 0,
			.field_byte_count = 3,
		},
		{
			/*LZ77 3B Code Word*/
			.id = AL_CRYPT_CMPRS_CW_3B_OFFSET_LENGTH_SYMBOL,
			.code_word_offset = 0,
			.alu_0_input_symbol = 8,
			.alu_1_input_symbol = 0,
			.alu_0_input_length = 16,
			.alu_1_input_length = 0,
			.alu_0_input_offset = 24,
			.alu_1_input_offset = 0,
			.alu_0_command_symbol = 3,
			.alu_1_command_symbol = 0,
			.alu_0_command_length = 3,
			.alu_1_command_length = 0,
			.alu_0_command_offset = 3,
			.alu_1_command_offset = 0,
			.field_byte_count = 3,
		},
		{
			/*LZ77 4B Code Word*/
			.id = AL_CRYPT_CMPRS_CW_4B_OFFSET_LENGTH_SYMBOL,
			.code_word_offset = 0,
			.alu_0_input_symbol = 0,
			.alu_1_input_symbol = 0,
			.alu_0_input_length = 8,
			.alu_1_input_length = 0,
			.alu_0_input_offset = 16,
			.alu_1_input_offset = 0,
			.alu_0_command_symbol = 0,
			.alu_1_command_symbol = 0,
			.alu_0_command_length = 3,
			.alu_1_command_length = 0,
			.alu_0_command_offset = 3,
			.alu_1_command_offset = 0,
			.field_byte_count = 4,
		},
		{
			/*LZ4*/
			.id = AL_CRYPT_CMPRS_STREAM,
			.code_word_offset = 0,
			.alu_0_input_symbol = 0,
			.alu_1_input_symbol = 0,
			.alu_0_input_length = 0,
			.alu_1_input_length = 0,
			.alu_0_input_offset = 0,
			.alu_1_input_offset = 0,
			.alu_0_command_symbol = 0,
			.alu_1_command_symbol = 0,
			.alu_0_command_length = 0,
			.alu_1_command_length = 0,
			.alu_0_command_offset = 0,
			.alu_1_command_offset = 0,
			.field_byte_count = 0,
		},
		{
			/*Static Deflate*/
			.id = AL_CRYPT_CMPRS_PROB_TABLES,
			.code_word_offset = 0,
			.alu_0_input_symbol = 0,
			.alu_1_input_symbol = 0,
			.alu_0_input_length = 0,
			.alu_1_input_length = 0,
			.alu_0_input_offset = 0,
			.alu_1_input_offset = 0,
			.alu_0_command_symbol = 0,
			.alu_1_command_symbol = 0,
			.alu_0_command_length = 0,
			.alu_1_command_length = 0,
			.alu_0_command_offset = 0,
			.alu_1_command_offset = 0,
			.field_byte_count = 0,
		},
	};

/** Compression encoder output table initial entries */
static const struct al_crypto_cmprs_enc_out_tbl_ent enc_out_tbl_entries[] = {
		{
			/*Send Encoded Sequence Regardless Of Compression Rate*/
			.id = 0,
			.alu_const_0 = 0,
			.alu_const_1 = 0,
			.alu_const_2 = 0,
			.alu_const_3 = 0,
			.alu_in_mux_0 = 0,
			.alu_in_mux_1 = 0,
			.alu_in_mux_2 = 0,
			.alu_in_mux_3 = 0,
			.alu_opcode_0 = 0,
			.alu_opcode_1 = 0,
			.alu_opcode_2 = 0,
			.alu_negative_res_mux_sel = 0,
			.alu_positive_res_mux_sel = 0,
			.alu_zero_res_mux_sel = 0,
		},
	};

static void al_crypto_gmac_ovrd_cfg(
	struct crypto_regs __iomem		*crypto_regs,
	const struct al_crypto_gmac_ovrd	*cfg)
{
	al_reg_write32(
		&crypto_regs->gmac_conf.override,
		(cfg->do_auth_serial ? CRYPTO_GMAC_CONF_OVERRIDE_DO_AUTH_SERIAL : 0) |
		(cfg->auth_override_en ? CRYPTO_GMAC_CONF_OVERRIDE_AUTH_OVERRIDE_EN : 0) |
		(cfg->auth_override_dis ? CRYPTO_GMAC_CONF_OVERRIDE_AUTH_OVERRIDE_DIS : 0) |
		(cfg->val_gen_n_overrid_en ? CRYPTO_GMAC_CONF_OVERRIDE_VAL_GEN_N_OVERRID_EN : 0) |
		(cfg->val_gen_n ? CRYPTO_GMAC_CONF_OVERRIDE_VAL_GEN_N : 0) |
		(cfg->auth_mode_override ? CRYPTO_GMAC_CONF_OVERRIDE_AUTH_MODE_OVERRIDE : 0) |
		(cfg->auth_mode << CRYPTO_GMAC_CONF_OVERRIDE_AUTH_MODE_SHIFT) |
		(cfg->key_type_override ? CRYPTO_GMAC_CONF_OVERRIDE_KEY_TYPE_OVERRIDE : 0) |
		(cfg->key_type << CRYPTO_GMAC_CONF_OVERRIDE_KEY_TYPE_SHIFT) |
		(cfg->sign_size_override ? CRYPTO_GMAC_CONF_OVERRIDE_SIGN_SIZE_OVERRIDE : 0) |
		(cfg->sign_size << CRYPTO_GMAC_CONF_OVERRIDE_SIGN_SIZE_SHIFT) |
		(cfg->digest_from_msb_override ?
		CRYPTO_GMAC_CONF_OVERRIDE_DIGEST_FROM_MSB_OVERRIDE : 0) |
		(cfg->digest_from_msb ?  CRYPTO_GMAC_CONF_OVERRIDE_DIGEST_FROM_MSB : 0));
}

static unsigned int al_crypto_gmac_endianity_swap_val(
	const struct al_crypto_gmac_endianity_swap_element *e)
{
	unsigned int val =
		(e->bit_swap ? AL_BIT(0) : 0) |
		(e->byte_swap ? AL_BIT(1) : 0) |
		(e->word_swap ? AL_BIT(2) : 0) |
		(e->aes_beat_swap ? AL_BIT(3) : 0);

	return val;
}

static void al_crypto_gmac_endianity_swap_cfg(
	struct crypto_regs __iomem			*crypto_regs,
	const struct al_crypto_gmac_endianity_swap	*cfg)
{
	al_reg_write32(
		&crypto_regs->gmac_conf.endianity_swap,
		(al_crypto_gmac_endianity_swap_val(&cfg->ingress) <<
		CRYPTO_GMAC_CONF_ENDIANITY_SWAP_INGRESS_SHIFT) |
		(al_crypto_gmac_endianity_swap_val(&cfg->egress) <<
		CRYPTO_GMAC_CONF_ENDIANITY_SWAP_EGRESS_SHIFT) |
		(al_crypto_gmac_endianity_swap_val(&cfg->aes_key) <<
		CRYPTO_GMAC_CONF_ENDIANITY_SWAP_AES_KEY_SHIFT) |
		(al_crypto_gmac_endianity_swap_val(&cfg->h_subkey) <<
		CRYPTO_GMAC_CONF_ENDIANITY_SWAP_H_SUBKEY_SHIFT) |
		(al_crypto_gmac_endianity_swap_val(&cfg->j0) <<
		CRYPTO_GMAC_CONF_ENDIANITY_SWAP_J0_SHIFT) |
		(al_crypto_gmac_endianity_swap_val(&cfg->sign_in) <<
		CRYPTO_GMAC_CONF_ENDIANITY_SWAP_SIGN_IN_SHIFT) |
		(al_crypto_gmac_endianity_swap_val(&cfg->sign_out) <<
		CRYPTO_GMAC_CONF_ENDIANITY_SWAP_SIGN_OUT_SHIFT));
}

static void al_crypto_gmac_data_mask_cfg(
	struct crypto_regs __iomem		*crypto_regs,
	const struct al_crypto_gmac_data_mask	*cfg)
{
	unsigned int i;

	al_reg_write32(
		&crypto_regs->gmac_conf.data_mask_polarity,
		(cfg->pol_id_0_mask_val ? CRYPTO_GMAC_CONF_DATA_MASK_POLARITY_POL_ID_0 : 0) |
		(cfg->pol_id_1_mask_val ? CRYPTO_GMAC_CONF_DATA_MASK_POLARITY_POL_ID_1 : 0) |
		(cfg->pol_id_2_mask_val ? CRYPTO_GMAC_CONF_DATA_MASK_POLARITY_POL_ID_2 : 0) |
		(cfg->pol_id_3_mask_val ? CRYPTO_GMAC_CONF_DATA_MASK_POLARITY_POL_ID_3 : 0));

	for (i = 0; i < AL_ARR_SIZE(cfg->arr); i++) {
		al_reg_write32(&crypto_regs->gmac_data_mask[i].id_0, cfg->arr[i].id_0);
		al_reg_write32(&crypto_regs->gmac_data_mask[i].id_1, cfg->arr[i].id_1);
		al_reg_write32(&crypto_regs->gmac_data_mask[i].id_2, cfg->arr[i].id_2);
		al_reg_write32(&crypto_regs->gmac_data_mask[i].id_3, cfg->arr[i].id_3);
	}
}

static void al_crypto_cmprs_enc_prof_tbl_init(
	struct crypto_regs __iomem			*crypto_regs,
	const struct al_crypto_cmprs_enc_prof_tbl_ent	*entries,
	unsigned int					num_entries)
{
	struct al_crypto_accelerator_lz_enc_profile __iomem *regs =
		&crypto_regs->lz_enc_profile;

	for (; num_entries; num_entries--, entries++) {
		al_reg_write32(&regs->entry_enc_profile, entries->id);
		al_reg_read32(&regs->entry_enc_profile); /* Ensure ordering */
		al_reg_write32(&regs->cfg_enc_profile,
			(entries->history_size <<
			CRYPTO_LZ_ENC_PROFILE_CFG_ENC_PROFILE_HISTORY_SIZE_SHIFT) |
			(entries->algorithm_index <<
			CRYPTO_LZ_ENC_PROFILE_CFG_ENC_PROFILE_ALGORITHM_INDEX_SHIFT) |
			(entries->compression_optimizer_en ?
			CRYPTO_LZ_ENC_PROFILE_CFG_ENC_PROFILE_CMPRS_OPTIMIZER_EN : 0) |
			(entries->force_original_sequence_out ?
			CRYPTO_LZ_ENC_PROFILE_CFG_ENC_PROFILE_FORCE_ORIGINAL_OUT : 0) |
			(entries->force_encoded_sequence_out ?
			CRYPTO_LZ_ENC_PROFILE_CFG_ENC_PROFILE_FORCE_ENCODED_OUT : 0) |
			(entries->output_controller_index <<
			CRYPTO_LZ_ENC_PROFILE_CFG_ENC_PROFILE_OUTPUT_CONTROLLER_INDEX_SHIFT) |
			(entries->concat_next_symbol ?
			CRYPTO_LZ_ENC_PROFILE_CFG_ENC_PROFILE_CONCAT_NEXT_SYMBOL : 0));
		al_reg_write32(&regs->thresholds_enc_profile,
			(entries->max_match_threshold <<
			CRYPTO_LZ_ENC_PROFILE_THRESHOLDS_ENC_PROFILE_MAX_SHIFT) |
			(entries->min_match_threshold <<
			CRYPTO_LZ_ENC_PROFILE_THRESHOLDS_ENC_PROFILE_MIN_SHIFT));
		al_reg_write32(&regs->eop_policy_profile,
			(entries->no_match_length <<
			CRYPTO_LZ_ENC_PROFILE_EOP_POLICY_PROFILE_NO_MATCH_LENGTH_SHIFT) |
			(entries->uncompressed_length <<
			CRYPTO_LZ_ENC_PROFILE_EOP_POLICY_PROFILE_UNCOMPRESSED_LENGTH_SHIFT));
	}
}

static void al_crypto_cmprs_dec_prof_tbl_init(
	struct crypto_regs __iomem			*crypto_regs,
	const struct al_crypto_cmprs_dec_prof_tbl_ent	*entries,
	unsigned int					num_entries)
{
	struct al_crypto_accelerator_lz_dec_profile __iomem *regs =
		&crypto_regs->lz_dec_profile;

	for (; num_entries; num_entries--, entries++) {
		al_reg_write32(&regs->entry_dec_profile, entries->id);
		al_reg_read32(&regs->entry_dec_profile); /* Ensure ordering */
		al_reg_write32(&regs->cfg,
			(entries->history_size <<
			CRYPTO_LZ_DEC_PROFILE_CFG_HISTORY_SIZE_SHIFT) |
			(entries->match_length_base <<
			CRYPTO_LZ_DEC_PROFILE_CFG_MATCH_LENGTH_BASE_SHIFT) |
			(entries->force_original_sequence_out ?
			CRYPTO_LZ_DEC_PROFILE_CFG_FORCE_ORIGINAL_OUT : 0));
		al_reg_write32(&regs->alg,
			(entries->compressed_flag_polarity ?
			CRYPTO_LZ_DEC_PROFILE_ALG_FLAG_COMPRESSED_POLARITY : 0) |
			(entries->compressed_flag_en ?
			CRYPTO_LZ_DEC_PROFILE_ALG_FLAG_ENABLE : 0) |
			(entries->compressed_flag_coalescing <<
			CRYPTO_LZ_DEC_PROFILE_ALG_FLAG_COALESCING_SHIFT) |
			(entries->field_offset_size ?
			CRYPTO_LZ_DEC_PROFILE_ALG_FIELD_OFFSET_SIZE : 0) |
			(entries->field_length_size ?
			CRYPTO_LZ_DEC_PROFILE_ALG_FIELD_LENGTH_SIZE : 0) |
			(entries->field_composition <<
			CRYPTO_LZ_DEC_PROFILE_ALG_FIELD_COMPOSITION_SHIFT) |
			(entries->algorithm_type <<
			CRYPTO_LZ_DEC_PROFILE_ALG_ALGORITHM_TYPE_SHIFT) |
			(entries->concat_next_symbol ?
			CRYPTO_LZ_DEC_PROFILE_ALG_CONCAT_NEXT_SYMBOL : 0) |
			(entries->offset_base ?
			CRYPTO_LZ_DEC_PROFILE_ALG_OFFSET_BASE : 0) |
			(entries->offset_endianity ?
			CRYPTO_LZ_DEC_PROFILE_ALG_OFFSET_ENDIANITY : 0) |
			(entries->huffman_extra_bits_swap ?
			CRYPTO_LZ_DEC_PROFILE_ALG_HUFFMAN_EXTRA_BITS_SWAP : 0) |
			(entries->bit_swap ?
			CRYPTO_LZ_DEC_PROFILE_ALG_BIT_SWAP : 0));

	}
}

static void al_crypto_cmprs_alg_tbl_init(
	struct crypto_regs __iomem			*crypto_regs,
	const struct al_crypto_cmprs_alg_tbl_ent	*entries,
	unsigned int					num_entries)
{
	struct al_crypto_accelerator_lz_algorithm __iomem *regs = &crypto_regs->lz_algorithm;

	for (; num_entries; num_entries--, entries++) {
		al_reg_write32(&regs->entry_alg, entries->id);
		al_reg_read32(&regs->entry_alg); /* Ensure ordering */
		al_reg_write32(&regs->flags_offset, entries->flags_offset);
		al_reg_write32(&regs->flags_pos_0, entries->flags_pos_0);
		al_reg_write32(&regs->flags_pos_1, entries->flags_pos_1);
		al_reg_write32(&regs->flags_pos_2, entries->flags_pos_2);
		al_reg_write32(&regs->flags_pos_3, entries->flags_pos_3);
		al_reg_write32(&regs->flags_cfg,
			(entries->flags_cfg_polarity ?
			CRYPTO_LZ_ALGORITHM_FLAGS_CFG_COMPRESSED_POLARITY : 0) |
			(entries->flags_cfg_en ?
			CRYPTO_LZ_ALGORITHM_FLAGS_CFG_ENABLE : 0));
		al_reg_write32(&regs->cmprs_form,
			(entries->algorithm_type <<
			CRYPTO_LZ_ALGORITHM_CMPRS_FORM_ALGORITHM_TYPE_SHIFT) |
			(entries->offset_base ?
			CRYPTO_LZ_ALGORITHM_CMPRS_FORM_OFFSET_BASE : 0) |
			(entries->offset_endianity ?
			CRYPTO_LZ_ALGORITHM_CMPRS_FORM_OFFSET_ENDIANITY : 0) |
			(entries->huffman_extra_bits_swap ?
			CRYPTO_LZ_ALGORITHM_CMPRS_FORM_HUFFMAN_EXTRA_BITS_SWAP : 0) |
			(entries->bit_swap ?
			CRYPTO_LZ_ALGORITHM_CMPRS_FORM_BIT_SWAP : 0));
	}
}

static void al_crypto_cmprs_cam_tbl_init(
	struct crypto_regs __iomem			*crypto_regs,
	const struct al_crypto_cmprs_cam_tbl_ent	*entries,
	unsigned int					num_entries)
{
	struct al_crypto_accelerator_lz_cam __iomem *regs = &crypto_regs->lz_cam;

	for (; num_entries; num_entries--, entries++) {
		al_reg_write32(&regs->entry_cam, entries->id);
		al_reg_read32(&regs->entry_cam); /* Ensure ordering */
		al_reg_write32(&regs->cfg_cam,
			(entries->format_table_index <<
			CRYPTO_LZ_CAM_CFG_CAM_FORMAT_TABLE_INDEX_SHIFT) |
			(entries->algorithm_index <<
			CRYPTO_LZ_CAM_CFG_CAM_ALGORITHM_INDEX_SHIFT) |
			(entries->compressed_symbol_en ?
			CRYPTO_LZ_CAM_CFG_CAM_COMPRESSED_SYMBOL_EN : 0) |
			(entries->compressed_symbol ?
			CRYPTO_LZ_CAM_CFG_CAM_COMPRESSED_SYMBOL : 0) |
			(entries->match_length_offset_sel ?
			CRYPTO_LZ_CAM_CFG_CAM_MATCH_LENGTH_OFFSET_SEL : 0) |
			(entries->entry_valid ?
			CRYPTO_LZ_CAM_CFG_CAM_CAM_ENTRY_VALID : 0));
		al_reg_write32(&regs->threshold_cam,
			(entries->min_code_threshold <<
			CRYPTO_LZ_CAM_THRESHOLD_CAM_MIN_CODE_THRESHOLD_SHIFT) |
			(entries->max_code_threshold <<
			CRYPTO_LZ_CAM_THRESHOLD_CAM_MAX_CODE_THRESHOLD_SHIFT));
	}
}

static void al_crypto_cmprs_fmt_tbl_init(
	struct crypto_regs __iomem			*crypto_regs,
	const struct al_crypto_cmprs_fmt_tbl_ent	*entries,
	unsigned int					num_entries)
{
	struct al_crypto_accelerator_lz_format __iomem *regs = &crypto_regs->lz_format;

	for (; num_entries; num_entries--, entries++) {
		al_reg_write32(&regs->entry_format, entries->id);
		al_reg_read32(&regs->entry_format); /* Ensure ordering */
		al_reg_write32(&regs->offset, entries->code_word_offset);
		al_reg_write32(&regs->alu_symbol_0, entries->alu_0_input_symbol);
		al_reg_write32(&regs->alu_symbol_1, entries->alu_1_input_symbol);
		al_reg_write32(&regs->alu_length_0, entries->alu_0_input_length);
		al_reg_write32(&regs->alu_length_1, entries->alu_1_input_length);
		al_reg_write32(&regs->alu_offset_0, entries->alu_0_input_offset);
		al_reg_write32(&regs->alu_offset_1, entries->alu_1_input_offset);
		al_reg_write32(&regs->cfg_format,
			(entries->alu_0_command_symbol <<
			CRYPTO_LZ_FORMAT_CFG_FORMAT_CMD_ALU_SYMBOL_0_SHIFT) |
			(entries->alu_1_command_symbol <<
			CRYPTO_LZ_FORMAT_CFG_FORMAT_CMD_ALU_SYMBOL_1_SHIFT) |
			(entries->alu_0_command_length <<
			CRYPTO_LZ_FORMAT_CFG_FORMAT_CMD_ALU_LENGTH_0_SHIFT) |
			(entries->alu_1_command_length <<
			CRYPTO_LZ_FORMAT_CFG_FORMAT_CMD_ALU_LENGTH_1_SHIFT) |
			(entries->alu_0_command_offset <<
			CRYPTO_LZ_FORMAT_CFG_FORMAT_CMD_ALU_OFFSET_0_SHIFT) |
			(entries->alu_1_command_offset <<
			CRYPTO_LZ_FORMAT_CFG_FORMAT_CMD_ALU_OFFSET_1_SHIFT) |
			(entries->field_byte_count <<
			CRYPTO_LZ_FORMAT_CFG_FORMAT_FIELD_BYTE_COUNT_SHIFT));
	}
}

static void al_crypto_cmprs_enc_out_tbl_init(
	struct crypto_regs __iomem			*crypto_regs,
	const struct al_crypto_cmprs_enc_out_tbl_ent	*entries,
	unsigned int					num_entries)
{
	struct al_crypto_accelerator_lz_output_arbiter __iomem *regs =
		&crypto_regs->lz_output_arbiter;

	for (; num_entries; num_entries--, entries++) {
		al_reg_write32(&regs->entry_out_arb, entries->id);
		al_reg_read32(&regs->entry_out_arb); /* Ensure ordering */
		al_reg_write32(&regs->alu_data_0_1,
			(entries->alu_const_0 <<
			CRYPTO_LZ_OUTPUT_ARBITER_ALU_DATA_0_1_CONST0_SHIFT) |
			(entries->alu_const_1 <<
			CRYPTO_LZ_OUTPUT_ARBITER_ALU_DATA_0_1_CONST1_SHIFT));
		al_reg_write32(&regs->alu_data_2_3,
			(entries->alu_const_2 <<
			CRYPTO_LZ_OUTPUT_ARBITER_ALU_DATA_2_3_CONST2_SHIFT) |
			(entries->alu_const_3 <<
			CRYPTO_LZ_OUTPUT_ARBITER_ALU_DATA_2_3_CONST3_SHIFT));
		al_reg_write32(&regs->cfg_out_arb,
			(entries->alu_in_mux_0 <<
			CRYPTO_LZ_OUTPUT_ARBITER_CFG_OUT_ARB_ALU_MUX_0_SHIFT) |
			(entries->alu_in_mux_1 <<
			CRYPTO_LZ_OUTPUT_ARBITER_CFG_OUT_ARB_ALU_MUX_1_SHIFT) |
			(entries->alu_in_mux_2 <<
			CRYPTO_LZ_OUTPUT_ARBITER_CFG_OUT_ARB_ALU_MUX_2_SHIFT) |
			(entries->alu_in_mux_3 <<
			CRYPTO_LZ_OUTPUT_ARBITER_CFG_OUT_ARB_ALU_MUX_3_SHIFT) |
			(entries->alu_opcode_0 <<
			CRYPTO_LZ_OUTPUT_ARBITER_CFG_OUT_ARB_ALU_OPCODE_0_SHIFT) |
			(entries->alu_opcode_1 <<
			CRYPTO_LZ_OUTPUT_ARBITER_CFG_OUT_ARB_ALU_OPCODE_1_SHIFT) |
			(entries->alu_opcode_2 <<
			CRYPTO_LZ_OUTPUT_ARBITER_CFG_OUT_ARB_ALU_OPCODE_2_SHIFT) |
			(entries->alu_negative_res_mux_sel ?
			CRYPTO_LZ_OUTPUT_ARBITER_CFG_OUT_ARB_ALU_NEGATIVE_RES_MUX : 0) |
			(entries->alu_positive_res_mux_sel ?
			CRYPTO_LZ_OUTPUT_ARBITER_CFG_OUT_ARB_ALU_POSITIVE_RES_MUX : 0) |
			(entries->alu_zero_res_mux_sel ?
			CRYPTO_LZ_OUTPUT_ARBITER_CFG_OUT_ARB_ALU_ZERO_RES_MUX : 0));
	}
}

#define AL_CRYPT_CMPRS_HUFFMAN_STATIC_TABLE_NUM_ENTRIES_SMALL	286
#define AL_CRYPT_CMPRS_HUFFMAN_STATIC_TABLE_NUM_ENTRIES_BIG	30
#define AL_CRYPT_CMPRS_HUFFMAN_DYN_TBL_ENT_SIZE_SMALL		4
#define AL_CRYPT_CMPRS_HUFFMAN_DYN_TBL_ENT_SIZE_BIG		5
#define AL_CRYPT_CMPRS_HUFFMAN_STATIC_TABLE_NUM_ENTRIES		\
	(AL_CRYPT_CMPRS_HUFFMAN_STATIC_TABLE_NUM_ENTRIES_SMALL +	\
	AL_CRYPT_CMPRS_HUFFMAN_STATIC_TABLE_NUM_ENTRIES_BIG)
#define AL_CRYPT_CMPRS_HUFFMAN_DYN_TBL_SIZE				\
	((AL_CRYPT_CMPRS_HUFFMAN_DYN_TBL_ENT_SIZE_SMALL *		\
	AL_CRYPT_CMPRS_HUFFMAN_STATIC_TABLE_NUM_ENTRIES_SMALL) +	\
	 (AL_CRYPT_CMPRS_HUFFMAN_DYN_TBL_ENT_SIZE_BIG *			\
	AL_CRYPT_CMPRS_HUFFMAN_STATIC_TABLE_NUM_ENTRIES_BIG))
#define AL_CRYPT_CMPRS_HUFFMAN_CODE_NUM_BITS			15
#define AL_CRYPT_CMPRS_HUFFMAN_CODE_LEN_NUM_BITS		4
#define AL_CRYPT_CMPRS_HUFFMAN_CODE_EXTRA_NUM_BITS		4
#define AL_CRYPT_CMPRS_HUFFMAN_DIST_SMALL_NUM_BITS		9
#define AL_CRYPT_CMPRS_HUFFMAN_DIST_BIG_NUM_BITS		15

static void _al_crypto_cmprs_huffman_static_tbl_entry_set(
	struct crypto_regs __iomem			*crypto_regs,
	unsigned int					entry_index,
	struct al_crypto_cmprs_huffman_static_tbl_entry	*entry)
{
	struct al_crypto_accelerator_huffman_static_table __iomem *regs =
		&crypto_regs->huffman_static_table;

	al_reg_write32(&regs->entry, entry_index);
	al_reg_read32(&regs->entry); /* Ensure ordering */
	al_reg_write32(&regs->value, entry->dist);
	al_reg_write32(&regs->alphabet,
		(entry->code << CRYPTO_HUFFMAN_STATIC_TABLE_ALPHABET_CODE_SHIFT) |
		(entry->length << CRYPTO_HUFFMAN_STATIC_TABLE_ALPHABET_LENGTH_SHIFT) |
		(entry->extra_bits << CRYPTO_HUFFMAN_STATIC_TABLE_ALPHABET_EXTRA_BITS_SHIFT));
}

static void _al_crypto_cmprs_huffman_dynamic_tbl_entry_set(
	void						*buff,
	unsigned int					entry_index,
	struct al_crypto_cmprs_huffman_static_tbl_entry	*entry)
{
	uint8_t *buff_entry;
	uint64_t coded_entry;
	int entry_size;
	int i;

	/*
	 * Entry encoding:
	 * Legends:
	 *	C - code
	 *	L - code length
	 *	X - code extra
	 *	D - dist
	 * Small entry (32 bits):
	 *	C[L-1]..C[0], 0[15 - L]..0[0], L[3]..L[0], X[3]..X[0], D[8]..D[0]
	 * Big entry (40 bits):
	 *	0[1]..0[0], C[L-1]..C[0], 0[15 - L]..0[0], L[3]..L[0], X[3]..X[0], D[14]..D[0]
	 */
	al_assert(entry->code < AL_BIT(AL_CRYPT_CMPRS_HUFFMAN_CODE_NUM_BITS));
	coded_entry = entry->code;
	al_assert(entry->length < AL_BIT(AL_CRYPT_CMPRS_HUFFMAN_CODE_LEN_NUM_BITS));
	coded_entry <<= (15 - entry->length);
	coded_entry <<= AL_CRYPT_CMPRS_HUFFMAN_CODE_LEN_NUM_BITS;
	coded_entry |= entry->length;
	coded_entry <<= AL_CRYPT_CMPRS_HUFFMAN_CODE_EXTRA_NUM_BITS;
	al_assert(entry->extra_bits < AL_BIT(AL_CRYPT_CMPRS_HUFFMAN_CODE_EXTRA_NUM_BITS));
	coded_entry |= entry->extra_bits;
	if (entry_index < AL_CRYPT_CMPRS_HUFFMAN_STATIC_TABLE_NUM_ENTRIES_SMALL) {
		entry_size = AL_CRYPT_CMPRS_HUFFMAN_DYN_TBL_ENT_SIZE_SMALL;
		coded_entry <<= AL_CRYPT_CMPRS_HUFFMAN_DIST_SMALL_NUM_BITS;
		al_assert(entry->dist < AL_BIT(AL_CRYPT_CMPRS_HUFFMAN_DIST_SMALL_NUM_BITS));
	} else {
		entry_size = AL_CRYPT_CMPRS_HUFFMAN_DYN_TBL_ENT_SIZE_BIG;
		coded_entry <<= AL_CRYPT_CMPRS_HUFFMAN_DIST_BIG_NUM_BITS;
		al_assert(entry->dist < AL_BIT(AL_CRYPT_CMPRS_HUFFMAN_DIST_BIG_NUM_BITS));
	}
	coded_entry |= entry->dist;

	if (entry_index < AL_CRYPT_CMPRS_HUFFMAN_STATIC_TABLE_NUM_ENTRIES_SMALL)
		buff_entry = ((uint8_t *)buff) +
			(entry_index * AL_CRYPT_CMPRS_HUFFMAN_DYN_TBL_ENT_SIZE_SMALL);
	else
		buff_entry = ((uint8_t *)buff) +
			(AL_CRYPT_CMPRS_HUFFMAN_STATIC_TABLE_NUM_ENTRIES_SMALL *
			AL_CRYPT_CMPRS_HUFFMAN_DYN_TBL_ENT_SIZE_SMALL) +
			((entry_index - AL_CRYPT_CMPRS_HUFFMAN_STATIC_TABLE_NUM_ENTRIES_SMALL) *
			AL_CRYPT_CMPRS_HUFFMAN_DYN_TBL_ENT_SIZE_BIG);

	for (i = 0; i < entry_size; i++, coded_entry >>= 8)
		*(buff_entry + entry_size - 1 - i) = coded_entry & 0xff;
}

static void al_crypto_cmprs_huffman_static_tbl_init(
	struct crypto_regs __iomem			*crypto_regs,
	void						*buff)
{
	struct al_crypto_cmprs_huffman_static_tbl_entry	entry;
	unsigned int idx;

	for (idx = 0; idx < AL_CRYPT_CMPRS_HUFFMAN_STATIC_TABLE_NUM_ENTRIES; idx++) {
		if (idx <= 143) {
			entry.code = idx + 48;
			entry.length = 8;
			entry.extra_bits = 0;
			entry.dist = idx;
		} else if (idx <= 255) {
			entry.code = idx + 256;
			entry.length = 9;
			entry.extra_bits = 0;
			entry.dist = idx;
		} else if (idx <= 256) {
			entry.code = 0;
			entry.length = 7;
			entry.extra_bits = 0;
			entry.dist = idx;
		} else if (idx <= 264) {
			entry.code = idx - 256;
			entry.length = 7;
			entry.extra_bits = 0;
			entry.dist = idx - 254;
		} else if (idx <= 279) {
			entry.code = idx - 256;
			entry.length = 7;
			entry.extra_bits = (idx - 261) / 4;
			entry.dist = (1 << (entry.extra_bits + 2)) + 3 +
				((idx - 1) % 4) * (1 << entry.extra_bits);
		} else if (idx <= 284) {
			entry.code = idx - 88;
			entry.length = 8;
			entry.extra_bits = (idx - 261) / 4;
			entry.dist = (1 << (entry.extra_bits + 2)) + 3 +
				((idx - 1) % 4) * (1 << entry.extra_bits);
		} else if (idx <= 285) {
			entry.code = idx - 88;
			entry.length = 8;
			entry.extra_bits = 0;
			entry.dist = 258;
		} else if (idx <= 289) {
			entry.code = idx - 286;
			entry.length = 5;
			entry.extra_bits = 0;
			entry.dist = idx - 285;
		} else {
			entry.code = idx - 286;
			entry.length = 5;
			entry.extra_bits = (idx - 288) / 2;
			entry.dist = (1 << (entry.extra_bits + 1)) + 1 +
				(idx % 2) * (1 << entry.extra_bits);
		}

		if (crypto_regs)
			_al_crypto_cmprs_huffman_static_tbl_entry_set(crypto_regs, idx, &entry);
		if (buff)
			_al_crypto_cmprs_huffman_dynamic_tbl_entry_set(buff, idx, &entry);
	}
}

/****************************** API functions *********************************/
void al_crypto_gmac_init(
	struct al_ssm_dma		*dma __attribute__ ((unused)),
	void __iomem			*app_regs,
	const struct al_crypto_gmac_cfg	*cfg)
{
	struct crypto_regs __iomem *regs;

	al_assert(dma->rev_id >= AL_SSM_REV_ID_REV3);

	regs = al_ssm_crypto_regs_get(app_regs, dma->rev_id);

	al_crypto_gmac_ovrd_cfg(regs,
		(cfg && cfg->ovrd_cfg) ? cfg->ovrd_cfg : &gmac_ovrd_cfg);
	al_crypto_gmac_endianity_swap_cfg(regs,
		(cfg && cfg->endianity_swap_cfg) ? cfg->endianity_swap_cfg :
		&gmac_endianity_swap_cfg);
	al_crypto_gmac_data_mask_cfg(regs,
		(cfg && cfg->data_mask_cfg) ? cfg->data_mask_cfg : &gmac_data_mask_cfg);
}

/** Inline CRC profile table entry */
struct al_crypto_icrc_prof_tbl_ent {
	/** ID */
	enum al_crypto_icrc_prof_id	id;
	/** byte swap on ingress data */
	al_bool				ingress_byte_swap;
	/** bit swap on ingress data */
	al_bool				ingress_bit_swap;
	/** byte swap on result */
	al_bool				res_byte_swap;
	/** bit swap on result */
	al_bool				res_bit_swap;
	/** perform one's completement on the result */
	al_bool				res_one_completement;
	/** byte swap on iv data */
	al_bool				iv_byte_swap;
	/** bit swap on iv data */
	al_bool				iv_bit_swap;
	/** perform one's completement on the iv */
	al_bool				iv_one_completement;
	/** index to crc header mask */
	unsigned int			start_mask_idx;
};

/** Inline CRC profile table initial entries */
static const struct al_crypto_icrc_prof_tbl_ent icrc_prof_tbl_entries[] = {
	{
		.id = AL_CRYPT_ICRC_PROF_ID_0, /* CRC16-T10-DIF */
		.ingress_byte_swap = AL_FALSE,
		.ingress_bit_swap = AL_FALSE,
		.res_byte_swap = AL_TRUE,
		.res_bit_swap = AL_FALSE,
		.res_one_completement = AL_FALSE,
		.iv_byte_swap = AL_TRUE,
		.iv_bit_swap = AL_FALSE,
		.iv_one_completement = AL_FALSE,
		.start_mask_idx = 0,
	},
	{
		.id = AL_CRYPT_ICRC_PROF_ID_1, /* CRC32/CRC32C */
		.ingress_byte_swap = AL_FALSE,
		.ingress_bit_swap = AL_TRUE,
		.res_byte_swap = AL_FALSE,
		.res_bit_swap = AL_TRUE,
		.res_one_completement = AL_TRUE,
		.iv_byte_swap = AL_FALSE,
		.iv_bit_swap = AL_TRUE,
		.iv_one_completement = AL_FALSE,
		.start_mask_idx = 0,
	},
	{
		.id = AL_CRYPT_ICRC_PROF_ID_2, /* CHECKSUM_ADLER32 */
		.ingress_byte_swap = AL_TRUE,
		.ingress_bit_swap = AL_FALSE,
		.res_byte_swap = AL_TRUE,
		.res_bit_swap = AL_FALSE,
		.res_one_completement = AL_FALSE,
		.iv_byte_swap = AL_TRUE,
		.iv_bit_swap = AL_FALSE,
		.iv_one_completement = AL_FALSE,
		.start_mask_idx = 0,
	},
	{
		.id = AL_CRYPT_ICRC_PROF_ID_3,
		.ingress_byte_swap = AL_TRUE,
		.ingress_bit_swap = AL_TRUE,
		.res_byte_swap = AL_FALSE,
		.res_bit_swap = AL_FALSE,
		.res_one_completement = AL_FALSE,
		.iv_byte_swap = AL_FALSE,
		.iv_bit_swap = AL_FALSE,
		.iv_one_completement = AL_FALSE,
		.start_mask_idx = 0,
	},
	{
		.id = AL_CRYPT_ICRC_PROF_ID_4,
		.ingress_byte_swap = AL_FALSE,
		.ingress_bit_swap = AL_FALSE,
		.res_byte_swap = AL_FALSE,
		.res_bit_swap = AL_TRUE,
		.res_one_completement = AL_FALSE,
		.iv_byte_swap = AL_FALSE,
		.iv_bit_swap = AL_FALSE,
		.iv_one_completement = AL_FALSE,
		.start_mask_idx = 0,
	},
	{
		.id = AL_CRYPT_ICRC_PROF_ID_5,
		.ingress_byte_swap = AL_FALSE,
		.ingress_bit_swap = AL_FALSE,
		.res_byte_swap = AL_TRUE,
		.res_bit_swap = AL_FALSE,
		.res_one_completement = AL_FALSE,
		.iv_byte_swap = AL_FALSE,
		.iv_bit_swap = AL_FALSE,
		.iv_one_completement = AL_FALSE,
		.start_mask_idx = 0,
	},
	{
		.id = AL_CRYPT_ICRC_PROF_ID_6,
		.ingress_byte_swap = AL_FALSE,
		.ingress_bit_swap = AL_FALSE,
		.res_byte_swap = AL_TRUE,
		.res_bit_swap = AL_TRUE,
		.res_one_completement = AL_FALSE,
		.iv_byte_swap = AL_FALSE,
		.iv_bit_swap = AL_FALSE,
		.iv_one_completement = AL_FALSE,
		.start_mask_idx = 0,
	},
	{
		.id = AL_CRYPT_ICRC_PROF_ID_7,
		.ingress_byte_swap = AL_FALSE,
		.ingress_bit_swap = AL_TRUE,
		.res_byte_swap = AL_FALSE,
		.res_bit_swap = AL_TRUE,
		.res_one_completement = AL_FALSE,
		.iv_byte_swap = AL_FALSE,
		.iv_bit_swap = AL_FALSE,
		.iv_one_completement = AL_FALSE,
		.start_mask_idx = 0,
	},
	{
		.id = AL_CRYPT_ICRC_PROF_ID_8,
		.ingress_byte_swap = AL_TRUE,
		.ingress_bit_swap = AL_FALSE,
		.res_byte_swap = AL_TRUE,
		.res_bit_swap = AL_FALSE,
		.res_one_completement = AL_FALSE,
		.iv_byte_swap = AL_FALSE,
		.iv_bit_swap = AL_FALSE,
		.iv_one_completement = AL_FALSE,
		.start_mask_idx = 0,
	},
	{
		.id = AL_CRYPT_ICRC_PROF_ID_9,
		.ingress_byte_swap = AL_TRUE,
		.ingress_bit_swap = AL_TRUE,
		.res_byte_swap = AL_TRUE,
		.res_bit_swap = AL_TRUE,
		.res_one_completement = AL_FALSE,
		.iv_byte_swap = AL_FALSE,
		.iv_bit_swap = AL_FALSE,
		.iv_one_completement = AL_FALSE,
		.start_mask_idx = 0,
	},
	{
		.id = AL_CRYPT_ICRC_PROF_ID_10,
		.ingress_byte_swap = AL_FALSE,
		.ingress_bit_swap = AL_FALSE,
		.res_byte_swap = AL_FALSE,
		.res_bit_swap = AL_FALSE,
		.res_one_completement = AL_TRUE,
		.iv_byte_swap = AL_FALSE,
		.iv_bit_swap = AL_FALSE,
		.iv_one_completement = AL_FALSE,
		.start_mask_idx = 0,
	},
	{
		.id = AL_CRYPT_ICRC_PROF_ID_11,
		.ingress_byte_swap = AL_TRUE,
		.ingress_bit_swap = AL_TRUE,
		.res_byte_swap = AL_TRUE,
		.res_bit_swap = AL_TRUE,
		.res_one_completement = AL_TRUE,
		.iv_byte_swap = AL_FALSE,
		.iv_bit_swap = AL_FALSE,
		.iv_one_completement = AL_FALSE,
		.start_mask_idx = 0,
	},
	{
		.id = AL_CRYPT_ICRC_PROF_ID_12,
		.ingress_byte_swap = AL_FALSE,
		.ingress_bit_swap = AL_FALSE,
		.res_byte_swap = AL_FALSE,
		.res_bit_swap = AL_FALSE,
		.res_one_completement = AL_FALSE,
		.iv_byte_swap = AL_FALSE,
		.iv_bit_swap = AL_FALSE,
		.iv_one_completement = AL_FALSE,
		.start_mask_idx = 1,
	},
	{
		.id = AL_CRYPT_ICRC_PROF_ID_13,
		.ingress_byte_swap = AL_FALSE,
		.ingress_bit_swap = AL_FALSE,
		.res_byte_swap = AL_FALSE,
		.res_bit_swap = AL_FALSE,
		.res_one_completement = AL_FALSE,
		.iv_byte_swap = AL_FALSE,
		.iv_bit_swap = AL_FALSE,
		.iv_one_completement = AL_FALSE,
		.start_mask_idx = 2,
	},
	{
		.id = AL_CRYPT_ICRC_PROF_ID_14,
		.ingress_byte_swap = AL_TRUE,
		.ingress_bit_swap = AL_FALSE,
		.res_byte_swap = AL_TRUE,
		.res_bit_swap = AL_FALSE,
		.res_one_completement = AL_TRUE,
		.iv_byte_swap = AL_FALSE,
		.iv_bit_swap = AL_FALSE,
		.iv_one_completement = AL_FALSE,
		.start_mask_idx = 3,
	},
	{
		.id = AL_CRYPT_ICRC_PROF_ID_15,
		.ingress_byte_swap = AL_TRUE,
		.ingress_bit_swap = AL_TRUE,
		.res_byte_swap = AL_TRUE,
		.res_bit_swap = AL_TRUE,
		.res_one_completement = AL_TRUE,
		.iv_byte_swap = AL_FALSE,
		.iv_bit_swap = AL_FALSE,
		.iv_one_completement = AL_FALSE,
		.start_mask_idx = 0,
	},
};

static void al_crypto_icrc_prof_tbl_init(
	struct crypto_regs __iomem			*crypto_regs,
	const struct al_crypto_icrc_prof_tbl_ent	*entries,
	unsigned int					num_entries)
{
	struct al_crypto_accelerator_crc_profile_table __iomem *regs =
		&crypto_regs->crc_profile_table;

	for (; num_entries; num_entries--, entries++) {
		al_reg_write32(&regs->crc_addr, entries->id);
		al_reg_read32(&regs->crc_addr); /* Ensure ordering */
		al_reg_write32(&regs->crc_data,
			(entries->ingress_byte_swap ?
			CRYPTO_CRC_PROFILE_TABLE_CRC_DATA_INGRESS_BYTE_SWAP : 0) |
			(entries->ingress_bit_swap ?
			CRYPTO_CRC_PROFILE_TABLE_CRC_DATA_INGRESS_BIT_SWAP : 0) |
			(entries->res_byte_swap ?
			CRYPTO_CRC_PROFILE_TABLE_CRC_DATA_RES_BYTE_SWAP : 0) |
			(entries->res_bit_swap ?
			CRYPTO_CRC_PROFILE_TABLE_CRC_DATA_RES_BIT_SWAP : 0) |
			(entries->res_one_completement ?
			CRYPTO_CRC_PROFILE_TABLE_CRC_DATA_RES_ONE_COMPLETEMENT : 0) |
			(entries->iv_byte_swap ?
			CRYPTO_CRC_PROFILE_TABLE_CRC_DATA_IV_BYTE_SWAP : 0) |
			(entries->iv_bit_swap ?
			CRYPTO_CRC_PROFILE_TABLE_CRC_DATA_IV_BIT_SWAP : 0) |
			(entries->iv_one_completement ?
			CRYPTO_CRC_PROFILE_TABLE_CRC_DATA_IV_ONE_COMPLETEMENT : 0) |
			(entries->start_mask_idx <<
			CRYPTO_CRC_PROFILE_TABLE_CRC_DATA_START_MASK_IDX_SHIFT));
	}
}

void al_crypto_icrc_init(
	struct al_ssm_dma		*dma __attribute__ ((unused)),
	void __iomem			*app_regs,
	const struct al_crypto_icrc_cfg	*cfg)
{
	struct crypto_regs __iomem *regs;
	unsigned int i;

	al_assert(dma->rev_id >= AL_SSM_REV_ID_REV3);

	regs = al_ssm_crypto_regs_get(app_regs, dma->rev_id);

	al_reg_write32(&regs->inline_crc_cfg.icrc_profile_table_idx,
		(cfg->prof_id_override_en ?
		CRYPTO_INLINE_CRC_CFG_ICRC_PROFILE_TABLE_IDX_OVERRIDE_EN : 0) |
		(cfg->prof_id <<
		CRYPTO_INLINE_CRC_CFG_ICRC_PROFILE_TABLE_IDX_OVERRIDE_VAL_SHIFT));

	for (i = 0; i < AL_ARR_SIZE(cfg->start_masks); i++) {
		al_reg_write32(&regs->inline_crc_start_mask[i].c_mask_0,
			cfg->start_masks[i].c_mask[0]);
		al_reg_write32(&regs->inline_crc_start_mask[i].c_mask_1,
			cfg->start_masks[i].c_mask[1]);
		al_reg_write32(&regs->inline_crc_start_mask[i].c_mask_2,
			cfg->start_masks[i].c_mask[2]);
		al_reg_write32(&regs->inline_crc_start_mask[i].c_mask_3,
			cfg->start_masks[i].c_mask[3]);
	}

	al_crypto_icrc_prof_tbl_init(
		regs, icrc_prof_tbl_entries, AL_ARR_SIZE(icrc_prof_tbl_entries));
}

void al_crypto_cmprs_lz_init(
	struct al_ssm_dma	*dma __attribute__ ((unused)),
	void __iomem		*app_regs)
{
	struct crypto_regs __iomem *regs;
	struct al_crypto_accelerator_lz_static_cfg __iomem *static_regs;

	al_assert(dma->rev_id >= AL_SSM_REV_ID_REV3);

	regs = al_ssm_crypto_regs_get(app_regs, dma->rev_id);
	static_regs = &regs->lz_static_cfg;

	al_reg_write32(&static_regs->memory_init, CRYPTO_LZ_STATIC_CFG_MEMORY_INIT_TRIGGER);
	al_crypto_cmprs_enc_prof_tbl_init(
		regs, enc_prof_tbl_entries, AL_ARR_SIZE(enc_prof_tbl_entries));
	al_crypto_cmprs_dec_prof_tbl_init(
		regs, dec_prof_tbl_entries, AL_ARR_SIZE(dec_prof_tbl_entries));
	al_crypto_cmprs_alg_tbl_init(
		regs, alg_tbl_entries, AL_ARR_SIZE(alg_tbl_entries));
	al_crypto_cmprs_cam_tbl_init(
		regs, cam_tbl_entries, AL_ARR_SIZE(cam_tbl_entries));
	al_crypto_cmprs_fmt_tbl_init(
		regs, fmt_tbl_entries, AL_ARR_SIZE(fmt_tbl_entries));
	al_crypto_cmprs_enc_out_tbl_init(
		regs, enc_out_tbl_entries, AL_ARR_SIZE(enc_out_tbl_entries));
}

void al_crypto_cmprs_huffman_init(
	struct al_ssm_dma	*dma __attribute__ ((unused)),
	void __iomem		*app_regs)
{
	struct crypto_regs __iomem *regs;

	al_assert(dma->rev_id >= AL_SSM_REV_ID_REV3);

	regs = al_ssm_crypto_regs_get(app_regs, dma->rev_id);

	al_crypto_cmprs_huffman_static_tbl_init(regs, NULL);
}

void al_crypto_cmprs_huffman_static_tbl_entry_set(
	struct al_ssm_dma				*dma __attribute__ ((unused)),
	void __iomem					*app_regs,
	unsigned int					entry_index,
	struct al_crypto_cmprs_huffman_static_tbl_entry	*entry)
{
	struct crypto_regs __iomem *regs;

	al_assert(dma->rev_id >= AL_SSM_REV_ID_REV3);

	regs = al_ssm_crypto_regs_get(app_regs, dma->rev_id);

	_al_crypto_cmprs_huffman_static_tbl_entry_set(regs, entry_index, entry);
}

void al_crypto_cmprs_huffman_static_tbl_default_set(
	void						*buff,
	unsigned int					buff_size)
{
	al_assert(buff);
	al_assert(buff_size == AL_CRYPT_CMPRS_HUFFMAN_DYN_TBL_SIZE);

	al_crypto_cmprs_huffman_static_tbl_init(NULL, buff);
}

void al_crypto_cmprs_huffman_dynamic_tbl_entry_set(
	void						*buff,
	unsigned int					buff_size,
	unsigned int					entry_index,
	struct al_crypto_cmprs_huffman_static_tbl_entry	*entry)
{
	al_assert(buff);
	al_assert(buff_size == AL_CRYPT_CMPRS_HUFFMAN_DYN_TBL_SIZE);
	al_assert(entry);
	al_assert(entry_index < AL_CRYPT_CMPRS_HUFFMAN_STATIC_TABLE_NUM_ENTRIES);

	_al_crypto_cmprs_huffman_dynamic_tbl_entry_set(buff, entry_index, entry);
}

