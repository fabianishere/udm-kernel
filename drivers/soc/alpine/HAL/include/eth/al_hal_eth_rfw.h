/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#ifndef __AL_HAL_ETH_RFW_H__
#define __AL_HAL_ETH_RFW_H__

#include "al_hal_common.h"

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

#define AL_HAL_ETH_HAS_RFW_API	1

struct al_eth_rfw_handle {
	/** CPU number uses this handle */
	unsigned int cpu;
	/** Ethernet Controller registers base address */
	void __iomem *ec_regs_base;
	/** Ethernet Forwarding unit registers per CPU base address */
	void __iomem *rfw_regs;
};

struct al_eth_rfw_handle_init_params {
	/** CPU number uses this handle */
	unsigned int cpu;
	/** Ethernet Controller registers base address */
	void __iomem *ec_regs_base;
};

struct al_eth_rfw_lrss_params {
	/** UDMA number to forward to */
	unsigned int udma_num;
	/** Table size */
	unsigned int table_size;
	/** Pointer to table, each element holds the target queue */
	uint8_t *indir_table;
};

struct al_eth_rfw_rss_table_params {
	uint32_t index;
	uint32_t tid; /* 20 bit */
	uint8_t udma;
	uint8_t queue;
	al_bool tid_cmd;
	al_bool udma_cmd;
	al_bool queue_cmd;
};

/** Total number of LRSS entries - 16k entries */
#define AL_ETH_RFW_LRSS_TABLE_ENTRIES		(16 * 1024)
/** Number of LRSS entries per subtable - set to 256 entries*/
#define AL_ETH_RFW_LRSS_SUBTABLE_ORDER		8
#define AL_ETH_RFW_LRSS_SUBTABLE_ENTRIES	(1 << AL_ETH_RFW_LRSS_SUBTABLE_ORDER)
/** Total number of sub-tables - 16k / 256 = 64 */
#define AL_ETH_RFW_LRSS_TABLES_MAX		\
		(AL_ETH_RFW_LRSS_TABLE_ENTRIES / AL_ETH_RFW_LRSS_SUBTABLE_ENTRIES)

#define AL_ETH_RFW_GENERIC_LRSS_IDX	6

struct al_eth_rfw_lrss_tables_params {
	uint32_t num; /**< number of sub-tables to initialize */
	uint32_t table_size_order; /**< order of the size of each sub-table */
	uint8_t default_queue[AL_ETH_RFW_LRSS_TABLES_MAX]; /**< default queue index for
								    * each sub-table
								    */
};

struct al_eth_rfw_lrss_handle_init_params {
	const struct al_eth_rfw_handle *fwd_handle;
	const struct al_eth_rfw_lrss_tables_params *lrss_table_params;
};

struct al_eth_rfw_lrss_info {
	uint32_t rss_ind_table_offset;
	uint32_t hash_output_bit_mask;
	uint8_t default_queue;
};

struct al_eth_rfw_lrss_handle {
	const struct al_eth_rfw_handle *fwd_handle;
	uint32_t num; /**< number of sub-tables to initialize */
	uint32_t table_size_order; /**< order of the size of each sub-table */
	uint8_t default_queue[AL_ETH_RFW_LRSS_TABLES_MAX]; /**< default queue index for
								    * each sub-table
								    */
	struct al_eth_rfw_lrss_info lrss_info[AL_ETH_RFW_LRSS_TABLES_MAX];
};


/**
 * RX forwardig engine init function
 *
 * @param	handle
 *		Ethernet forwarding engine handle
 * @param	handle_init_params
 *		Initialization parameters
 */
void al_eth_rfw_handle_init(struct al_eth_rfw_handle *handle,
		struct al_eth_rfw_handle_init_params *handle_init_params);

/**
 * get an entry from LRSS table
 *
 * @param handle
 *	Ethernet forwarding engine handle
 * @param index entry index
 * @param params result params
  *
 * @return 0 on success. otherwise on failure.
 */
int al_eth_rfw_lrss_table_get(
	const struct al_eth_rfw_handle *handle,
	unsigned int index,
	struct al_eth_rfw_rss_table_params *params);

/**
 * Update an entry in LRSS table
 *
 * @param handle
 *	Ethernet forwarding engine handle
 * @param params
  *
 * @return 0 on success. otherwise on failure.
 */
int al_eth_rfw_lrss_table_update(const struct al_eth_rfw_handle *handle,
		const struct al_eth_rfw_rss_table_params *params);


/**
 * Initialize LRSS subtables
 *
 * @param lrss_handle lrss handle to initialize
 * @param handle_init_params handle init params
 *
 * @return 0 on success. negative number otherwise.
 */
int al_eth_rfw_lrss_handle_init(
	struct al_eth_rfw_lrss_handle *lrss_handle,
	const struct al_eth_rfw_lrss_handle_init_params *handle_init_params);


/**
 * Get indirection table offset
 *
 * @param lrss_handle lrss subtables handle
 * @param subtable_index lrss sub-table.
 *
 * @return offset of indirection table
 */
uint32_t al_eth_rfw_lrss_ind_table_offset_get(
	const struct al_eth_rfw_lrss_handle *lrss_handle,
	uint32_t subtable_index);

/**
 * Update L_RSS entry
 *
 * @param lrss_handle lrss subtables handle
 * @param subtable_index lrss sub-table.
 * @param entry_index within the lrss sub-table.
 * @param udma UDMA number
 * @param queue Queue number
 */
int al_eth_rfw_lrss_entry_set(const struct al_eth_rfw_lrss_handle *lrss_handle,
				 uint32_t subtable_index, uint32_t entry_index,
				 uint32_t udma, uint32_t queue);

/**
 * Update L_RSS entry inside generic traffic table
 *
 * @param rx_fwd_handle RX forwarding engine handle
 * @param entry_index within the lrss sub-table.
 * @param udma_num UDMA number
 * @param queue Queue number
 */
int al_eth_rfw_lrss_generic_entry_set(const struct al_eth_rfw_handle *rx_fwd_handle,
				 uint32_t entry_index,
				 unsigned int udma_num, uint8_t queue);

/**
 * Update LRSS generic table
 *
 * @param rx_fwd_handle RX forwarding engine handle
 * @param params lrss params
 */
int al_eth_rfw_lrss_generic_config(
	struct al_eth_rfw_handle *rx_fwd_handle,
	struct al_eth_rfw_lrss_params *params);

#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */

#endif /* __AL_HAL_ETH_RFW_H__ */
