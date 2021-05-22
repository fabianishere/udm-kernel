/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

/**
 *  @{
 * @file   al_hal_udma_regs_gen_section_ctrl.h
 *
 * @brief udma_gen_section_ctrl registers
 *
 */

#ifndef __AL_HAL_UDMA_GEN_SECTION_CTRL_REGS_H__
#define __AL_HAL_UDMA_GEN_SECTION_CTRL_REGS_H__

#include "al_hal_plat_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Unit Registers
 */

struct udma_gen_section_ctrl {
	/* [0x0] */
	uint32_t cfg_v4_section_en;
	/* [0x4] When drop is  remaped to alternative processing this is the lower 32 bits of the address */
	uint32_t drop_addr_lsb;
	/* [0x8] When drop is  remaped to alternative processing this is the high 32 bits of the address */
	uint32_t drop_addr_msb;
	uint32_t rsrvd;
};

struct udma_gen_section_ctrl_init_transaction_table {
	/* [0x0] */
	uint32_t init_trig;
	/* [0x4] */
	uint32_t init_in_progress;
};

struct udma_gen_section_ctrl_transaction_type_table {
	/* [0x0] */
	uint32_t addr;
	/* [0x4] Write Command */
	uint32_t write_cmd;
	/* [0x8] Cound Command */
	uint32_t count_cmd;
	/*
	 * [0xc] AXI Address High Register
	 * AXI Address High
	 */
	uint32_t axi_msb_addr;
	/*
	 * [0x10] AXI Address Selector Register
	 * AXI Address selector
	 */
	uint32_t axi_msb_addr_sel;
	/* [0x14] AXI Target-ID values */
	uint32_t axi_tgtid_values;
	/* [0x18] Write Padding (Bytes) */
	uint32_t axi_write_padding;
};

struct udma_gen_section_ctrl_regs {
	/* [0x0] */
	uint32_t rsrvd_0[512];
	/* [0x800] */
	struct udma_gen_section_ctrl section_ctrl;
	/* [0x810] */
	uint32_t rsrvd_1[4];
	/* [0x820] */
	struct udma_gen_section_ctrl_init_transaction_table init_transaction_table;
	/* [0x828] */
	uint32_t rsrvd_2[246];
	/* [0xc00] */
	struct udma_gen_section_ctrl_transaction_type_table transaction_type_table;
};


/*
 * Registers Fields
 */

/**** cfg_v4_section_en register ****/
/* enable section feature */
#define UDMA_GEN_SECTION_CTRL_CFG_V4_SECTION_EN_VAL (1 << 0)
/* when enable drop command is mapped to alternative processing cmd with address taken from below registers */
#define UDMA_GEN_SECTION_CTRL_CFG_V4_SECTION_EN_REMAP_DROP_TO_ALT_PROCESSING (1 << 1)
/* Enable addr round up in section table (Double protection) */
#define UDMA_GEN_SECTION_CTRL_CFG_V4_SECTION_EN_ALLOW_SEC_ADDR_ROUND_UP (1 << 2)

/**** init_trig register ****/
/* When '1' start memory init flow - need to poll init_in_progress and then release */
#define UDMA_GEN_SECTION_CTRL_INIT_TRANSACTION_TABLE_INIT_TRIG_VAL (1 << 0)

/**** init_in_progress register ****/
/* indication that init is in proress */
#define UDMA_GEN_SECTION_CTRL_INIT_TRANSACTION_TABLE_INIT_IN_PROGRESS_VAL (1 << 0)

/**** addr register ****/
/*
 * [8:5] - Queue number
 * [4:0] - Transaction type
 */
#define UDMA_GEN_SECTION_CTRL_TRANSACTION_TYPE_TABLE_ADDR_VAL_MASK 0x000001FF
#define UDMA_GEN_SECTION_CTRL_TRANSACTION_TYPE_TABLE_ADDR_VAL_SHIFT 0

/**** write_cmd register ****/
/*
 * Write Command
 * 0 - BUFF1
 * 1 - BUFF 2
 * 2 - alternative processing
 * 3 - drop
 */
#define UDMA_GEN_SECTION_CTRL_TRANSACTION_TYPE_TABLE_WRITE_CMD_VAL_MASK 0x00000003
#define UDMA_GEN_SECTION_CTRL_TRANSACTION_TYPE_TABLE_WRITE_CMD_VAL_SHIFT 0

/**** count_cmd register ****/
/*
 * 0 - Override existing value
 * 1 - add to the existing value (accumulate)
 */
#define UDMA_GEN_SECTION_CTRL_TRANSACTION_TYPE_TABLE_COUNT_CMD_CALC_CMD (1 << 0)
/*
 * 0 - don’t add to any field/counter
 * 1 - add to BUFF1 len
 * 2 - add to BUFF2 len
 * 3 - reserved
 */
#define UDMA_GEN_SECTION_CTRL_TRANSACTION_TYPE_TABLE_COUNT_CMD_FIELD_SELECTION_MASK 0x00000006
#define UDMA_GEN_SECTION_CTRL_TRANSACTION_TYPE_TABLE_COUNT_CMD_FIELD_SELECTION_SHIFT 1

/**** axi_tgtid_values register ****/
/* AXI Target-ID value */
#define UDMA_GEN_SECTION_CTRL_TRANSACTION_TYPE_TABLE_AXI_TGTID_VALUES_AXI_TGTID_MASK 0x0000FFFF
#define UDMA_GEN_SECTION_CTRL_TRANSACTION_TYPE_TABLE_AXI_TGTID_VALUES_AXI_TGTID_SHIFT 0
/* AXI Target-ID selector */
#define UDMA_GEN_SECTION_CTRL_TRANSACTION_TYPE_TABLE_AXI_TGTID_VALUES_AXI_TGTID_SEL_MASK 0xFFFF0000
#define UDMA_GEN_SECTION_CTRL_TRANSACTION_TYPE_TABLE_AXI_TGTID_VALUES_AXI_TGTID_SEL_SHIFT 16

/**** axi_write_padding register ****/
/*
 * 0 -don't pad
 * 1 - pad to 64B cache alignment
 * 2-3 reserved
 */
#define UDMA_GEN_SECTION_CTRL_TRANSACTION_TYPE_TABLE_AXI_WRITE_PADDING_PADDING_CMD_MASK 0x00000003
#define UDMA_GEN_SECTION_CTRL_TRANSACTION_TYPE_TABLE_AXI_WRITE_PADDING_PADDING_CMD_SHIFT 0
/*
 * 0 - nothing
 * 1 - force the assertion of all AXI write strobe (MSB) at the end of section
 */
#define UDMA_GEN_SECTION_CTRL_TRANSACTION_TYPE_TABLE_AXI_WRITE_PADDING_SOS_WSTRB_CMD (1 << 2)
/*
 * 0 - nothing
 * 1 - force the assertion of all AXI write strobe (LSB) at the start of section
 */
#define UDMA_GEN_SECTION_CTRL_TRANSACTION_TYPE_TABLE_AXI_WRITE_PADDING_EOS_WSTRB_CMD (1 << 3)
/*
 * On End of secion - decide if align next address
 * 0 - Align address to 8B (for example if address was 71'd -> next address will be 72'd)
 * 1 - Align address to 16B (for example if address was 71'd -> next address will be 80'd)
 * 2 - Align address to 32B (for example if address was 71'd -> next address will be 96'd)
 * 3 - Align address to 64B (for example if address was 71'd -> next address will be 128'd)
 * 4 - Align address to 128B (for example if address was 71'd -> next address will be 128'd)
 * 5 - Align address to 256B (for example if address was 71'd -> next address will be 256'd)
 * 6 - Reserved
 * 7 - Do nothing
 */
#define UDMA_GEN_SECTION_CTRL_TRANSACTION_TYPE_TABLE_AXI_WRITE_PADDING_EOS_ADDR_ROUND_UP_CMD_MASK 0x00000070
#define UDMA_GEN_SECTION_CTRL_TRANSACTION_TYPE_TABLE_AXI_WRITE_PADDING_EOS_ADDR_ROUND_UP_CMD_SHIFT 4
/* Reseverd - but in memory */
#define UDMA_GEN_SECTION_CTRL_TRANSACTION_TYPE_TABLE_AXI_WRITE_PADDING_SPARE (1 << 7)

#ifdef __cplusplus
}
#endif

#endif

/** @} */


