/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#include "al_mod_hal_eth_rfw.h"
#include "al_mod_hal_eth_ec_regs.h"
#include "al_mod_hal_eth_rfw_regs.h"
#include "al_mod_hal_eth_rx_fwd_lrss_tables.h"

#define AL_ETH_RFW_CPU_MAX 16

void al_mod_eth_rfw_handle_init(struct al_mod_eth_rfw_handle *handle,
		struct al_mod_eth_rfw_handle_init_params *handle_init_params)
{
	struct al_mod_ec_regs __iomem *ec_regs;
	struct al_mod_eth_rfw_cpu_regs __iomem *rfw_regs;

	al_mod_assert(handle);
	al_mod_assert(handle_init_params);
	al_mod_assert(handle_init_params->cpu < AL_ETH_RFW_CPU_MAX);
	al_mod_assert(handle_init_params->ec_regs_base);

	handle->cpu = handle_init_params->cpu;
	handle->ec_regs_base = handle_init_params->ec_regs_base;

	ec_regs = (struct al_mod_ec_regs __iomem *)handle->ec_regs_base;

	rfw_regs = (struct al_mod_eth_rfw_cpu_regs __iomem *)&ec_regs->rfw_shared_cfg[0];
	handle->rfw_regs = &rfw_regs[handle->cpu];
}

int al_mod_eth_rfw_lrss_table_get(
	const struct al_mod_eth_rfw_handle *handle,
	unsigned int index,
	struct al_mod_eth_rfw_rss_table_params *params)
{
	uint32_t *addr_reg, *data_reg;
	uint32_t entry[LRSS_TABLE_ENTRY_DWORD_COUNT] = { 0 };
	struct al_mod_eth_rfw_cpu_regs *rfw_regs = handle->rfw_regs;

	addr_reg = &rfw_regs->lrss_indirection_table_addr;
	data_reg = &rfw_regs->lrss_indirection_table_data0;

	al_mod_reg_write32(addr_reg, index);
	entry[0] = al_mod_reg_read32(data_reg);

	params->index = index;
	params->tid = LRSS_TABLE_FIELD_GET(entry, TID_VALUE);
	params->udma = LRSS_TABLE_FIELD_GET(entry, UDMA_VALUE);
	params->queue = LRSS_TABLE_FIELD_GET(entry, QUEUE_VALUE);
	params->tid_cmd = LRSS_TABLE_FIELD_GET(entry, TID_CMD);
	params->udma_cmd = LRSS_TABLE_FIELD_GET(entry, UDMA_CMD);
	params->queue_cmd = LRSS_TABLE_FIELD_GET(entry, QUEUE_CMD);

	return 0;
}

int al_mod_eth_rfw_lrss_table_update(const struct al_mod_eth_rfw_handle *handle,
		const struct al_mod_eth_rfw_rss_table_params *params)
{
	uint32_t *addr_reg, *data_reg;
	uint32_t entry[LRSS_TABLE_ENTRY_DWORD_COUNT] = {0};
	struct al_mod_eth_rfw_cpu_regs *rfw_regs = handle->rfw_regs;

	addr_reg = &rfw_regs->lrss_indirection_table_addr;
	data_reg = &rfw_regs->lrss_indirection_table_data0;

	LRSS_TABLE_FIELD_SET(entry, TID_VALUE, params->tid);
	LRSS_TABLE_FIELD_SET(entry, UDMA_VALUE, params->udma);
	LRSS_TABLE_FIELD_SET(entry, QUEUE_VALUE, params->queue);
	LRSS_TABLE_FIELD_SET(entry, TID_CMD, params->tid_cmd);
	LRSS_TABLE_FIELD_SET(entry, UDMA_CMD, params->udma_cmd);
	LRSS_TABLE_FIELD_SET(entry, QUEUE_CMD, params->queue_cmd);

	al_mod_reg_write32(addr_reg, params->index);
	al_mod_reg_write32(data_reg, entry[0]);

	return 0;
}

static const struct al_mod_eth_rfw_lrss_tables_params default_lrss_tables_params = {
	.num = AL_ETH_RFW_LRSS_TABLES_MAX,
	.table_size_order = AL_ETH_RFW_LRSS_SUBTABLE_ORDER,
	.default_queue = {
				[AL_ETH_RFW_GENERIC_LRSS_IDX] = 0,
			 },
};

int al_mod_eth_rfw_lrss_handle_init(
	struct al_mod_eth_rfw_lrss_handle *lrss_handle,
	const struct al_mod_eth_rfw_lrss_handle_init_params *handle_init_params)
{
	const struct al_mod_eth_rfw_lrss_tables_params *lrss_tables;
	uint64_t ind_table_total_size;
	unsigned int i;

	al_mod_assert(lrss_handle);
	al_mod_assert(handle_init_params);

	lrss_tables = handle_init_params->lrss_table_params;

	if (!lrss_tables)
		/** If no params supplied, use default params */
		lrss_tables = &default_lrss_tables_params;

	if (lrss_tables->num > AL_ETH_RFW_LRSS_TABLES_MAX) {
		al_mod_err("requested tables number is too large\n");
		return -EINVAL;
	}

	ind_table_total_size = lrss_tables->num * (1 << lrss_tables->table_size_order);

	if (lrss_tables->table_size_order > 16 || /* validate no overflow */
		ind_table_total_size > AL_ETH_RFW_LRSS_TABLE_ENTRIES) {
		al_mod_err("requested tables size is too large\n");
		return -EINVAL;
	}

	lrss_handle->fwd_handle = handle_init_params->fwd_handle;
	lrss_handle->num = lrss_tables->num;
	lrss_handle->table_size_order = lrss_tables->table_size_order;

	for (i = 0; i < lrss_handle->num; i++) {
		struct al_mod_eth_rfw_lrss_info *lrss_info;

		uint32_t table_size = 1 << lrss_tables->table_size_order;

		lrss_info = &lrss_handle->lrss_info[i];
		al_mod_memset(lrss_info, 0, sizeof(*lrss_info));
		lrss_info->rss_ind_table_offset = i * table_size;
		lrss_info->default_queue = lrss_tables->default_queue[i];
		lrss_info->hash_output_bit_mask = ~(table_size - 1);
	}

	return 0;
}


uint32_t al_mod_eth_rfw_lrss_ind_table_offset_get(
	const struct al_mod_eth_rfw_lrss_handle *lrss_tables,
	uint32_t subtable_index)
{
	al_mod_assert(lrss_tables);
	al_mod_assert(subtable_index < lrss_tables->num);

	return lrss_tables->lrss_info[subtable_index].rss_ind_table_offset;
}

int al_mod_eth_rfw_lrss_entry_set(const struct al_mod_eth_rfw_lrss_handle *lrss_handle,
				 uint32_t subtable_index, uint32_t entry_index,
				 uint32_t udma, uint32_t queue)
{
	const struct al_mod_eth_rfw_lrss_info *lrss_info;
	struct al_mod_eth_rfw_rss_table_params rss_entry;
	int rc;

	al_mod_assert(lrss_handle);
	al_mod_assert(subtable_index < lrss_handle->num);
	al_mod_assert(entry_index < (uint32_t)(1 << lrss_handle->table_size_order));

	lrss_info = &lrss_handle->lrss_info[subtable_index];
	/* check if entry_index is valid */
	al_mod_assert((entry_index & lrss_info->hash_output_bit_mask) == 0);

	al_mod_memset(&rss_entry, 0, sizeof(rss_entry));
	rss_entry.index = lrss_info->rss_ind_table_offset + entry_index;
	rss_entry.udma = AL_BIT(udma);
	rss_entry.queue = queue;
	rc = al_mod_eth_rfw_lrss_table_update(lrss_handle->fwd_handle,
					    &rss_entry);
	return rc;
}

static void rfw_lrss_handle_init_helper(const struct al_mod_eth_rfw_handle *rx_fwd_handle,
	struct al_mod_eth_rfw_lrss_handle *lrss_tables_handle)
{
	int rc;
	struct al_mod_eth_rfw_lrss_handle_init_params lrss_tables_handle_init_params = {
			.fwd_handle = rx_fwd_handle,
			.lrss_table_params = &default_lrss_tables_params,
	};

	al_mod_assert(rx_fwd_handle);
	al_mod_assert(lrss_tables_handle);

	al_mod_memset(lrss_tables_handle, 0, sizeof(struct al_mod_eth_rfw_lrss_handle));

	rc = al_mod_eth_rfw_lrss_handle_init(lrss_tables_handle, &lrss_tables_handle_init_params);
	if (rc != 0) {
		al_mod_err("%s: Failed at al_mod_eth_rfw_lrss_handle_init (rc = %d)", __func__, rc);
		al_mod_assert(0);
	}
}

int al_mod_eth_rfw_lrss_generic_entry_set(const struct al_mod_eth_rfw_handle *rx_fwd_handle,
				 uint32_t entry_index,
				 unsigned int udma_num, uint8_t queue)
{
	struct al_mod_eth_rfw_lrss_handle lrss_tables_handle;
	int rc;

	al_mod_assert(rx_fwd_handle);

	rfw_lrss_handle_init_helper(rx_fwd_handle, &lrss_tables_handle);

	rc = al_mod_eth_rfw_lrss_entry_set(
		&lrss_tables_handle, AL_ETH_RFW_GENERIC_LRSS_IDX, entry_index, udma_num, queue);
	if (rc)
		al_mod_err("%s: Failed at rfw_lrss_entry_set (rc = %d)\n", __func__, rc);

	return rc;
}

int al_mod_eth_rfw_lrss_generic_config(
	struct al_mod_eth_rfw_handle *rx_fwd_handle,
	struct al_mod_eth_rfw_lrss_params *params)
{
	uint32_t i;
	struct al_mod_eth_rfw_lrss_handle lrss_tables_handle;
	int rc;

	al_mod_assert(rx_fwd_handle);
	al_mod_assert(params);
	al_mod_assert(params->table_size <= (1 << AL_ETH_RFW_LRSS_SUBTABLE_ORDER));

	rfw_lrss_handle_init_helper(rx_fwd_handle, &lrss_tables_handle);

	for (i = 0; i < params->table_size; i++) {
		rc = al_mod_eth_rfw_lrss_entry_set(
			&lrss_tables_handle, AL_ETH_RFW_GENERIC_LRSS_IDX,
			i, params->udma_num, params->indir_table[i]);
		if (rc != 0) {
			al_mod_err("%s: Failed at al_mod_eth_rfw_lrss_entry_set (rc = %d)", __func__, rc);
			return rc;
		}
	}

	return 0;
}
