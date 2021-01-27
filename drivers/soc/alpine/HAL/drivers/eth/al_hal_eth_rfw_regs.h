/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

/**
 *  @{
 */

#ifndef __AL_HAL_ETH_RFW_REGS_H__
#define __AL_HAL_ETH_RFW_REGS_H__

#include "al_hal_plat_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Unit Registers
 */

struct al_eth_rfw_cpu_regs {
	uint32_t rsrvd_0[116];
	/* [0x1d0] lrss indireciton table */
	uint32_t lrss_indirection_table_size;
	/* [0x1d4] lrss indireciton table */
	uint32_t lrss_indirection_table_base;
	/* [0x1d8] */
	uint32_t lrss_indirection_table_addr;
	/* [0x1dc] */
	uint32_t lrss_indirection_table_data0;
	uint32_t rsrvd[904];
};



/*
 * Registers Fields
 */

/**** lrss_indirection_table_size register ****/

#define ETH_RFW_CPU_LRSS_INDIRECTION_TABLE_SIZE_VAL_MASK 0x00007FFF
#define ETH_RFW_CPU_LRSS_INDIRECTION_TABLE_SIZE_VAL_SHIFT 0

/**** lrss_indirection_table_base register ****/

#define ETH_RFW_CPU_LRSS_INDIRECTION_TABLE_BASE_VAL_MASK 0x00003FFF
#define ETH_RFW_CPU_LRSS_INDIRECTION_TABLE_BASE_VAL_SHIFT 0

/**** lrss_indirection_table_addr register ****/

#define ETH_RFW_CPU_LRSS_INDIRECTION_TABLE_ADDR_VAL_MASK 0x00003FFF
#define ETH_RFW_CPU_LRSS_INDIRECTION_TABLE_ADDR_VAL_SHIFT 0

/**** lrss_indirection_table_data0 register ****/

#define ETH_RFW_CPU_LRSS_INDIRECTION_TABLE_DATA0_TID_VALUE_MASK 0x000FFFFF
#define ETH_RFW_CPU_LRSS_INDIRECTION_TABLE_DATA0_TID_VALUE_SHIFT 0

#define ETH_RFW_CPU_LRSS_INDIRECTION_TABLE_DATA0_UDMA_VALUE_MASK 0x00F00000
#define ETH_RFW_CPU_LRSS_INDIRECTION_TABLE_DATA0_UDMA_VALUE_SHIFT 20

#define ETH_RFW_CPU_LRSS_INDIRECTION_TABLE_DATA0_QUEUE_VALUE_MASK 0x0F000000
#define ETH_RFW_CPU_LRSS_INDIRECTION_TABLE_DATA0_QUEUE_VALUE_SHIFT 24

#define ETH_RFW_CPU_LRSS_INDIRECTION_TABLE_DATA0_TID_CMD (1 << 28)

#define ETH_RFW_CPU_LRSS_INDIRECTION_TABLE_DATA0_UDMA_CMD (1 << 29)

#define ETH_RFW_CPU_LRSS_INDIRECTION_TABLE_DATA0_QUEUE_CMD (1 << 30)

#ifdef __cplusplus
}
#endif

#endif

/** @} */


