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
 * @defgroup group_eth_epe_api API
 * Ethernet Controller Parsing Engine (EPE) HAL driver API
 * @ingroup group_eth
 * @{
 * @file   al_hal_eth_epe.h
 *
 * @brief Header file for Ethernet Controller Parsing Engine (EPE) HAL driver API
 *
 */

#ifndef __AL_HAL_ETH_EPE_H__
#define __AL_HAL_ETH_EPE_H__

#include "al_hal_common.h"
#include "al_hal_eth_field.h"

struct al_hal_eth_adapter;

/*********************************** IOFIC *****************************************/
/* EC EPE IOFIC group A */
#define AL_ETH_EPE_GROUP_A_RMP_HDR_INPUT_FIFO_MEM_PAR_ERR_0                              AL_BIT(0)
#define AL_ETH_EPE_GROUP_A_RMP_HDR_INPUT_FIFO_MEM_PAR_ERR_1                              AL_BIT(1)
#define AL_ETH_EPE_GROUP_A_RMP_HDR_INPUT_FIFO_MEM_PAR_ERR_2                              AL_BIT(2)
#define AL_ETH_EPE_GROUP_A_RMP_HDR_INPUT_FIFO_MEM_PAR_ERR_3                              AL_BIT(3)
#define AL_ETH_EPE_GROUP_A_RMP_HDR_INPUT_FIFO_MEM_PAR_ERR_4                              AL_BIT(4)
#define AL_ETH_EPE_GROUP_A_RMP_HDR_INPUT_FIFO_MEM_PAR_ERR_5                              AL_BIT(5)
#define AL_ETH_EPE_GROUP_A_RMP_HDR_INPUT_FIFO_MEM_PAR_ERR_6                              AL_BIT(6)
#define AL_ETH_EPE_GROUP_A_RMP_HDR_INPUT_FIFO_MEM_PAR_ERR_7                              AL_BIT(7)
#define AL_ETH_EPE_GROUP_A_RMP_DATA_FIFO_LO_MEM_PAR_ERR                                  AL_BIT(8)
#define AL_ETH_EPE_GROUP_A_RMP_DATA_FIFO_HI_MEM_PAR_ERR                                  AL_BIT(9)
/* EC EPE IOFIC group B */
#define AL_ETH_EPE_GROUP_B_EC_ERR_RPE_IN_EOP_IN_IDLE_0                                   AL_BIT(0)
#define AL_ETH_EPE_GROUP_B_EC_ERR_RPE_IN_EOP_IN_IDLE_1                                   AL_BIT(1)
#define AL_ETH_EPE_GROUP_B_EC_ERR_RPE_IN_EOP_IN_IDLE_2                                   AL_BIT(2)
#define AL_ETH_EPE_GROUP_B_EC_ERR_RPE_IN_EOP_IN_IDLE_3                                   AL_BIT(3)
#define AL_ETH_EPE_GROUP_B_EC_ERR_RPE_IN_EOP_IN_IDLE_4                                   AL_BIT(4)
#define AL_ETH_EPE_GROUP_B_EC_ERR_RPE_IN_EOP_IN_IDLE_5                                   AL_BIT(5)
#define AL_ETH_EPE_GROUP_B_EC_ERR_RPE_IN_EOP_IN_IDLE_6                                   AL_BIT(6)
#define AL_ETH_EPE_GROUP_B_EC_ERR_RPE_IN_EOP_IN_IDLE_7                                   AL_BIT(7)
#define AL_ETH_EPE_GROUP_B_HDR_INPUT_FIFO_ERR_IN_EOP_IN_IDLE_0                           AL_BIT(8)
#define AL_ETH_EPE_GROUP_B_HDR_INPUT_FIFO_ERR_IN_EOP_IN_IDLE_1                           AL_BIT(9)
#define AL_ETH_EPE_GROUP_B_HDR_INPUT_FIFO_ERR_IN_EOP_IN_IDLE_2                           AL_BIT(10)
#define AL_ETH_EPE_GROUP_B_HDR_INPUT_FIFO_ERR_IN_EOP_IN_IDLE_3                           AL_BIT(11)

/* EC EPE IOFIC group C */
#define AL_ETH_EPE_GROUP_C_EC_ERR_RPE_IN_SOP_IN_PKT_0                                    AL_BIT(0)
#define AL_ETH_EPE_GROUP_C_EC_ERR_RPE_IN_SOP_IN_PKT_1                                    AL_BIT(1)
#define AL_ETH_EPE_GROUP_C_EC_ERR_RPE_IN_SOP_IN_PKT_2                                    AL_BIT(2)
#define AL_ETH_EPE_GROUP_C_EC_ERR_RPE_IN_SOP_IN_PKT_3                                    AL_BIT(3)
#define AL_ETH_EPE_GROUP_C_EC_ERR_RPE_IN_SOP_IN_PKT_4                                    AL_BIT(4)
#define AL_ETH_EPE_GROUP_C_EC_ERR_RPE_IN_SOP_IN_PKT_5                                    AL_BIT(5)
#define AL_ETH_EPE_GROUP_C_EC_ERR_RPE_IN_SOP_IN_PKT_6                                    AL_BIT(6)
#define AL_ETH_EPE_GROUP_C_EC_ERR_RPE_IN_SOP_IN_PKT_7                                    AL_BIT(7)
#define AL_ETH_EPE_GROUP_C_HDR_INPUT_FIFO_ERR_IN_SOP_IN_PKT_0                            AL_BIT(8)
#define AL_ETH_EPE_GROUP_C_HDR_INPUT_FIFO_ERR_IN_SOP_IN_PKT_1                            AL_BIT(9)
#define AL_ETH_EPE_GROUP_C_HDR_INPUT_FIFO_ERR_IN_SOP_IN_PKT_2                            AL_BIT(10)
#define AL_ETH_EPE_GROUP_C_HDR_INPUT_FIFO_ERR_IN_SOP_IN_PKT_3                            AL_BIT(11)

/* EC EPE IOFIC group D */
#define AL_ETH_EPE_GROUP_D_EC_ERR_RPE_OUT_EOP_HDR_EMPTY_0                                AL_BIT(0)
#define AL_ETH_EPE_GROUP_D_EC_ERR_RPE_OUT_EOP_HDR_EMPTY_1                                AL_BIT(1)
#define AL_ETH_EPE_GROUP_D_EC_ERR_RPE_OUT_EOP_HDR_EMPTY_2                                AL_BIT(2)
#define AL_ETH_EPE_GROUP_D_EC_ERR_RPE_OUT_EOP_HDR_EMPTY_3                                AL_BIT(3)
#define AL_ETH_EPE_GROUP_D_EC_ERR_RPE_OUT_EOP_HDR_EMPTY_4                                AL_BIT(4)
#define AL_ETH_EPE_GROUP_D_EC_ERR_RPE_OUT_EOP_HDR_EMPTY_5                                AL_BIT(5)
#define AL_ETH_EPE_GROUP_D_EC_ERR_RPE_OUT_EOP_HDR_EMPTY_6                                AL_BIT(6)
#define AL_ETH_EPE_GROUP_D_EC_ERR_RPE_OUT_EOP_HDR_EMPTY_7                                AL_BIT(7)
#define AL_ETH_EPE_GROUP_D_HDR_INPUT_FIFO_ERR_OUT_EOP_HDR_EMPTY_0                        AL_BIT(8)
#define AL_ETH_EPE_GROUP_D_HDR_INPUT_FIFO_ERR_OUT_EOP_HDR_EMPTY_1                        AL_BIT(9)
#define AL_ETH_EPE_GROUP_D_HDR_INPUT_FIFO_ERR_OUT_EOP_HDR_EMPTY_2                        AL_BIT(10)
#define AL_ETH_EPE_GROUP_D_HDR_INPUT_FIFO_ERR_OUT_EOP_HDR_EMPTY_3                        AL_BIT(11)

/**
 * Ethernet EPE type
 *
 * Some EPEs have inner & outer parser while others only outer
 */
enum al_eth_epe_type {
	/** Outer parser */
	EPE_TYPE_OUTER = 0,
	/** Inner parser (where available) */
	EPE_TYPE_INNER = 1
};

/**
 * Ethernet EPE field write locations in parser result vector
 */
enum al_eth_epe_field_write {
	/** L3 protocol index */
	EPE_FIELD_WRITE_L3_PROTO_IDX,
	/** L4 protocol index */
	EPE_FIELD_WRITE_L4_PROTO_IDX,
	/** L3 header offset */
	EPE_FIELD_WRITE_L3_HDR_OFFSET,
	/** L4 header offset */
	EPE_FIELD_WRITE_L4_HDR_OFFSET,
	/** L3 header length */
	EPE_FIELD_WRITE_L3_HDR_LEN,
	/** L4 header length */
	EPE_FIELD_WRITE_L4_HDR_LEN,
	EPE_FIELD_WRITE_MAX
};

/**
 * Ethernet EPE handle initialization parameters
 */
struct al_eth_epe_handle_init_params {
	/**
	 * Registers base address
	 * For Ethernet revision <= 3 it's Ethernet Controller registers base
	 * For Ethernet revision >= 4 it's Ethernet Controller RMP registers base
	 */
	void __iomem *regs_base;

	/** Ethernet revision ID */
	uint8_t eth_rev_id;

	/** EPE type */
	enum al_eth_epe_type type;

	/**
	 * Field write locations
	 *
	 * Pointer to array of size EPE_FIELD_WRITE_MAX of pointers to field parameters
	 */
	struct al_eth_field **fields;
};

/**
 * Ethernet EPE handle
 */
struct al_eth_epe_handle {
	/** Registers base address */
	void __iomem *regs_base;

	/** IOFIC base address (relevant for eth v4 and above)*/
	void __iomem *ints_base;

	/** Ethernet revision ID */
	uint8_t eth_rev_id;

	/** EPE type */
	enum al_eth_epe_type type;

	/** Field write locations */
	struct al_eth_field **fields;
};

/**
 * Ethernet EPE initialization parameters
 */
struct al_eth_epe_init_params {
	/** Enable parser */
	al_bool enable;

	/** Initialize EPE tables default entries when Ethernet revision <= 3 */
	al_bool init_v3_default_entries;
};

/** EPE Control table entry size */
#define EPE_CTRL_SIZE_V1		(6)
#define EPE_CTRL_SIZE_V3		(6)
#define EPE_CTRL_SIZE_V4		(9)
#define EPE_CTRL_SIZE_MAX		EPE_CTRL_SIZE_V4

/** EPE result vector size */
#define EPE_CTRL_RESULT_VECTOR_SIZE_V1			(528)
#define EPE_CTRL_RESULT_VECTOR_SIZE_V3			(848)
#define EPE_CTRL_RESULT_VECTOR_SIZE_V4			(1552)
#define EPE_CTRL_RESULT_VECTOR_SIZE_BY_REV(rev_id)	\
	((rev_id) < 3 ? EPE_CTRL_RESULT_VECTOR_SIZE_V1 : \
	 (rev_id) == 3 ? EPE_CTRL_RESULT_VECTOR_SIZE_V3 : \
			 EPE_CTRL_RESULT_VECTOR_SIZE_V4)

/** EPE Control table data commands */
#define EPE_CTRL_DATA_CMD_MAX_V1			(2)
#define EPE_CTRL_DATA_CMD_MAX_V3			(2)
#define EPE_CTRL_DATA_CMD_MAX_V4			(4)
#define EPE_CTRL_DATA_CMD_MAX				EPE_CTRL_DATA_CMD_MAX_V4
#define EPE_CTRL_DATA_CMD_MAX_BY_REV(rev_id)	\
	((rev_id) < 3 ? EPE_CTRL_DATA_CMD_MAX_V1 : \
	 (rev_id) == 3 ? EPE_CTRL_DATA_CMD_MAX_V3 : \
			 EPE_CTRL_DATA_CMD_MAX_V4)

/** EPE data command min. offset in result vector */
#define EPE_CTRL_DATA_CMD_OFFSET_MIN_V1(cmd_idx)	(0)
#define EPE_CTRL_DATA_CMD_OFFSET_MIN_V3(cmd_idx)	(0)
#define EPE_CTRL_DATA_CMD_OFFSET_MIN_V4(cmd_idx)	((cmd_idx) < 2 ? 0 : 512)
#define EPE_CTRL_DATA_CMD_OFFSET_MIN_BY_REV(rev_id, cmd_idx)	\
	((rev_id) < 3 ? EPE_CTRL_DATA_CMD_OFFSET_MIN_V1(cmd_idx) : \
	 (rev_id) == 3 ? EPE_CTRL_DATA_CMD_OFFSET_MIN_V3(cmd_idx) : \
			 EPE_CTRL_DATA_CMD_OFFSET_MIN_V4(cmd_idx))

/** EPE data command max. offset in result vector */
#define EPE_CTRL_DATA_CMD_OFFSET_MAX_V1(cmd_idx)	(EPE_CTRL_RESULT_VECTOR_SIZE_V1 - 1)
#define EPE_CTRL_DATA_CMD_OFFSET_MAX_V3(cmd_idx)	(EPE_CTRL_RESULT_VECTOR_SIZE_V3 - 1)
#define EPE_CTRL_DATA_CMD_OFFSET_MAX_V4(cmd_idx)	((cmd_idx) < 2 ? \
		EPE_CTRL_DATA_CMD_OFFSET_MAX_V3(cmd_idx) : EPE_CTRL_RESULT_VECTOR_SIZE_V4 - 1)
#define EPE_CTRL_DATA_CMD_OFFSET_MAX_BY_REV(rev_id, cmd_idx)	\
	((rev_id) < 3 ? EPE_CTRL_DATA_CMD_OFFSET_MAX_V1(cmd_idx) : \
	 (rev_id) == 3 ? EPE_CTRL_DATA_CMD_OFFSET_MAX_V3(cmd_idx) : \
			 EPE_CTRL_DATA_CMD_OFFSET_MAX_V4(cmd_idx))

/** EPE data command max. size */
#define EPE_CTRL_DATA_CMD_SIZE_MAX_V1(cmd_idx)		(256)
#define EPE_CTRL_DATA_CMD_SIZE_MAX_V3(cmd_idx)		(256)
#define EPE_CTRL_DATA_CMD_SIZE_MAX_V4(cmd_idx)		((cmd_idx) < 2 ? 384 : 512)
#define EPE_CTRL_DATA_CMD_SIZE_MAX_BY_REV(rev_id, cmd_idx)	\
	((rev_id) < 3 ? EPE_CTRL_DATA_CMD_SIZE_MAX_V1(cmd_idx) : \
	 (rev_id) == 3 ? EPE_CTRL_DATA_CMD_SIZE_MAX_V3(cmd_idx) : \
			 EPE_CTRL_DATA_CMD_SIZE_MAX_V4(cmd_idx))

/** EPE Compare CAM - compare command */
enum al_eth_epe_comp_cmd {
	/** A == B */
	EPE_COMP_CMD_EQ = 0,
	/** A <= B */
	EPE_COMP_CMD_LE = 1,
	/** A >= B  */
	EPE_COMP_CMD_GE = 2,
	/** Ignore compare */
	EPE_COMP_CMD_IGNORE = 3,
};

/** EPE Compare CAM entry */
struct al_eth_epe_comp_entry {
	/**
	 * Table index
	 *
	 * Ethernet revision <= 3 - 0 to 31
	 * Ethernet revision >= 4 - 0 to 63
	 */
	unsigned int idx;

	/** Valid entry */
	al_bool valid;

	/** Header compare value (first 16-bit) */
	uint16_t header_val;
	/** Header compare mask (first 16-bit) */
	uint16_t header_mask;
	/** Header compare command */
	enum al_eth_epe_comp_cmd header_cmd;

	/** Branch compare value */
	unsigned int branch_val;
	/** Branch compare mask */
	unsigned int branch_mask;

	/** Stage compare value */
	unsigned int stage_val;
	/** Stage compare mask */
	unsigned int stage_mask;
	/** Stage compare command */
	enum al_eth_epe_comp_cmd stage_cmd;

	/**
	 * Output - Protocol index
	 *
	 * Ethernet revision <= 3 - also the control table index
	 */
	unsigned int output_proto_idx;
	/**
	 * Output - Control table index
	 *
	 * Only for Ethernet revision >= 4
	 */
	unsigned int output_ctrl_idx;
};

/** Control table */

/** EPE end of parsing control */
enum al_eth_epe_ctrl_end {
	/** Invalid state */
	EPE_CTRL_END_INVALID,
	/** Continue with this parser to next iteration */
	EPE_CTRL_END_CONTINUE,
	/** Pass to next parser */
	EPE_CTRL_END_NEXT_PARSER,
	/** Finish parsing */
	EPE_CTRL_END_FINISH,
};

/** EPE header length operation */
enum al_eth_epe_ctrl_hdr_len_op {
	/** Zero length */
	EPE_CTRL_HDR_LEN_ZERO = 0,
	/** Length from packet */
	EPE_CTRL_HDR_LEN_PKT = 1,
	/** Length from packet passed through translation table 1 */
	EPE_CTRL_HDR_LEN_PKT_TABLE_1 = 2,
	/** Length from packet passed through translation table 2 */
	EPE_CTRL_HDR_LEN_PKT_TABLE_2 = 3
};

/** EPE layer select */
enum al_eth_epe_ctrl_layer {
	/** No layer */
	EPE_CTRL_LAYER_NONE,
	/** Layer 3 */
	EPE_CTRL_LAYER_L3,
	/** Layer 4 */
	EPE_CTRL_LAYER_L4,
};

/** EPE data source select */
enum al_eth_epe_ctrl_data_src {
	/** Next iteration value */
	EPE_CTRL_DATA_SRC_NEXT,
	/** Current iteration value */
	EPE_CTRL_DATA_SRC_CURRENT,
};

/**
 * EPE header length calculation
 *
 * Operations are ordered by execution order:
 * 1. Extract
 * 2. Shift left
 * 3. Operation
 * 4. Add operation
 */
struct al_eth_epe_ctrl_hdr_len_calc {
	/** Offset in header (bits) */
	unsigned int offset;
	/** Field size (bits) */
	unsigned int size;

	/** Shift left extracted header length */
	unsigned int shift;
	/** Operation  (zero, as is or indirection table) */
	enum al_eth_epe_ctrl_hdr_len_op op;
	/** Add value */
	unsigned int add_val;
};

/** EPE layer data write command to parser result vector */
struct al_eth_epe_ctrl_layer_cmd {
	/** Layer */
	enum al_eth_epe_ctrl_layer layer;
	/** Source */
	enum al_eth_epe_ctrl_data_src data_src;
};

/** EPE generic data write command to parser result vector */
struct al_eth_epe_ctrl_data_cmd {
	/** Offset in header (bits) */
	unsigned int offset;

	/**
	 * Parser result vector field to write
	 *
	 * If NULL - no write
	 */
	struct al_eth_field *field;
};

struct al_eth_epe_ctrl_entry {
	/**
	 * Table index
	 *
	 * Ethernet revision <= 3 - 0 to 31
	 * Ethernet revision >= 4 - 0 to 63
	 */
	unsigned int idx;

	/**
	 * Next protocol available
	 *
	 * If not - default control table index specified in this entry will be used
	 */
	al_bool next_proto_avaliable;

	/** Offset to skip for next protocol (or next iteration) (bytes) */
	unsigned int offset_to_next_proto;

	/** Next stage */
	unsigned int next_stage;

	/** Next branch value */
	unsigned int next_branch_val;
	/** Next branch mask */
	unsigned int next_branch_mask;

	/**
	 * Default next protocol index
	 *
	 * Ethernet revision <= 3 - also the control table index
	 */
	unsigned int default_next_proto_idx;
	/**
	 * Default next control table index
	 *
	 * Only for Ethernet revision >= 4
	 */
	unsigned int default_next_ctrl_idx;

	/** End parsing control */
	enum al_eth_epe_ctrl_end end_ctrl;

	/** Header length calculation */
	struct al_eth_epe_ctrl_hdr_len_calc hdr_len_calc;

	/** Protocol index write command */
	struct al_eth_epe_ctrl_layer_cmd proto_idx_cmd;
	/** Header offset write command */
	struct al_eth_epe_ctrl_layer_cmd hdr_offset_cmd;
	/** Header length write command */
	struct al_eth_epe_ctrl_layer_cmd hdr_len_cmd;

	/**
	 * Generic data write commands
	 *
	 * Ethernet revision <= 3 - 2 commands
	 * Ethernet revision >= 4 - 4 commands
	 */
	struct al_eth_epe_ctrl_data_cmd data_cmds[EPE_CTRL_DATA_CMD_MAX];

	/**
	 * Control bits
	 */

	/** Control bits value (8 bits) */
	unsigned int ctrl_bits_val;
	/** Control bits mask */
	unsigned int ctrl_bits_mask;

	/**
	 * Control bits 2 value (16 bits)
	 *
	 * Only for Ethernet revision >= 4
	 */
	unsigned int ctrl_bits2_val;
	/** Control bits 2 mask */
	unsigned int ctrl_bits2_mask;
};

/** Compare table raw entry */
struct al_eth_epe_p_reg_entry {
	uint32_t data;
	uint32_t mask;
	uint32_t ctrl;
};

/** Control table raw entry */
struct al_eth_epe_control_entry {
	uint32_t data[EPE_CTRL_SIZE_MAX];
};

/**
 * Initialize Ethernet EPE handle
 *
 * @param handle
 *	Ethernet EPE handle
 * @param params
 *	Ethernet EPE handle initialization parameters
 *
 * @return 0 on success, errno on failure.
 */
int al_eth_epe_handle_init(struct al_eth_epe_handle *handle,
	struct al_eth_epe_handle_init_params *params);

/**
 * Initialize Ethernet EPE
 *
 * Note: Initialization of outer EPE will trigger memories initialization which might affect
 *       inner EPE too.
 *
 * @param handle
 *	Ethernet EPE handle
 * @param params
 *	Ethernet EPE initialization parameters
 *
 * @return 0 on success, errno on failure.
 */
int al_eth_epe_init(struct al_eth_epe_handle *handle,
	struct al_eth_epe_init_params *params);

/**
 * Configure EPE compare table entry
 *
 * Note: Ethernet revision <= 3 has a shared compare table for outer & inner parsers so we allow
 *	 accessing only through outer parser handle.
 *
 * @param handle
 *	Ethernet EPE handle
 * @param comp_entry
 *	Compare table entry
 */
void al_eth_epe_comp_entry_set(struct al_eth_epe_handle *handle,
	struct al_eth_epe_comp_entry *comp_entry);

/**
 * Configure EPE compare table entry with raw values
 *
 * Note: Ethernet revision <= 3 has a shared compare table for outer & inner parsers so we allow
 *	 accessing only through outer parser handle.
 *
 * @param handle
 *	Ethernet EPE handle
 * @param idx
 *	Compare table index
 * @param comp_entry_raw
 *	Compare table raw entry
 */
void al_eth_epe_comp_entry_raw_set(struct al_eth_epe_handle *handle,
	unsigned int idx, struct al_eth_epe_p_reg_entry *comp_entry_raw);

/**
 * update rx parser entry valid state
 *
 * @param handle Ethernet EPE handle
 * @param idx the compare table index to update
 * @param state the requested state
 *
 */
void al_eth_epe_comp_entry_valid_set(struct al_eth_epe_handle *handle,
				     unsigned int idx, al_bool state);

/**
 * Configure EPE control table entry
 *
 * @param handle
 *	Ethernet EPE handle
 * @param ctrl_entry
 *	Control table entry
 */
void al_eth_epe_ctrl_entry_set(struct al_eth_epe_handle *handle,
	struct al_eth_epe_ctrl_entry *ctrl_entry);

/**
 * Configure EPE control table entry with raw values
 *
 * @param handle
 *	Ethernet EPE handle
 * @param idx
 *	Control table index
 * @param ctrl_entry_raw
 *	Control table entry
 */
void al_eth_epe_ctrl_entry_raw_set(struct al_eth_epe_handle *handle,
	unsigned int idx, struct al_eth_epe_control_entry *ctrl_entry_raw);

/**
 * read rx ethernet parser engine entry
 *
 * @param handle pointer to the private structure
 * @param idx the protocol index to read
 * @param reg_entry pointer to store entry contents from reg table entry
 * @param control_entry pointer to store entry contents from control table entry
 * @param control_table_idx index of control table from which to read the control entry
 */
void al_eth_epe_v3_entry_get(struct al_eth_epe_handle *handle, uint32_t idx,
			struct al_eth_epe_p_reg_entry *reg_entry,
			struct al_eth_epe_control_entry *control_entry,
			unsigned int control_table_idx);

/**
 * Update a range of entries in EPE tables, starting from a specific offset
 *
 * @param adapter pointer to the private structure
 * @param epe_p_reg_entries array of EPE compare table entries to update
 * @param epe_control_entries array of EPE control table entries to update
 * @param num_entries size of the above arrays
 * @param first_idx offset in EPE tables from which to start updating
 *
 * @return 0 on success. otherwise on failure.
 */
int al_eth_epe_entry_range_update(struct al_hal_eth_adapter *adapter,
				  struct al_eth_epe_p_reg_entry epe_p_reg_entries[],
				  struct al_eth_epe_control_entry epe_control_entries[],
				  unsigned int num_entries,
				  unsigned int first_idx);

/**
 * Get EPE error interrupts iofic masks
 *
 * @param handle private EPE data structure
 * @param epe_a_mask pointer to EPE group A mask
 * @param epe_b_mask pointer to EPE group B mask
 * @param epe_c_mask pointer to EPE group C mask
 * @param epe_d_mask pointer to EPE group D mask
 *
 */
void al_eth_epe_error_ints_masks_get(struct al_eth_epe_handle *handle,
					uint32_t *epe_a_mask,
					uint32_t *epe_b_mask,
					uint32_t *epe_c_mask,
					uint32_t *epe_d_mask);

#endif /* __AL_HAL_ETH_EPE_H__ */

/** @} */
