/*
 * Copyright 2018, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

/**
 *  @{
 * @file  al_mod_hal_eth_crc_tables.h
 *
 * @brief Ethernet Generic CRC Tables initialization arrays
 *
 * @note This file should be included in just one file, to avoid multiple
 *       instantiations of the arrays.
 */

#ifndef __AL_HAL_ETH_CRC_TABLES_H__
#define __AL_HAL_ETH_CRC_TABLES_H__

#include "al_mod_hal_eth.h"

/****** Default tables with minimal necessary entries ******/

static struct al_mod_eth_tx_gpd_cam_entry
al_mod_eth_generic_tx_crc_gpd_v3[AL_ETH_TX_GPD_TABLE_SIZE] = {
	/* [31] default match */
	[31] = {
		0,		0,		0,		0,		1,
		0x0,		0x0,		0x0,		0x0
	}
};

#define TX_GCP_TABLE_ENTRY_ETH_V4_STUB \
	{0, 0, 0, 0, 0, 0}

static struct al_mod_eth_tx_gcp_table_entry
al_mod_eth_generic_tx_crc_gcp_v3[AL_ETH_TX_GPD_TABLE_SIZE] = {
	/* [31] default match */
	[31] = {
		0,		0,		0,		0,		0,
		0,		0,		0,		0,		0,
		0,		0,		0,		0,		0,
		0,		0,		{0x00000000,	0x00000000,	0x00000000,
		0x00000000,	0x00000000,	  0x00000000},	0x00000000,	0x0,
		0,
		TX_GCP_TABLE_ENTRY_ETH_V4_STUB,
	}
};

static struct al_mod_eth_tx_crc_chksum_replace_cmd_for_protocol_num_entry
al_mod_eth_tx_crc_chksum_replace_cmd_v3[AL_ETH_TX_GPD_TABLE_SIZE] = {
	/* [31] default match */
	[31] = {
		0,0,0,0,		0,0,1,1,		0,1,0,1
	}
};

static struct al_mod_eth_rx_gpd_cam_entry
al_mod_eth_generic_rx_crc_gpd_v3[AL_ETH_RX_GCP_TABLE_SIZE] = {
	/* [0] l3_pkt - IPV4 */
	{8,		0,		0,		0,
	0,		0,		0,		0,		1,
	0x1f,		0x1f,		0x0,		0x0,
	0x4,		0x0,		0x0,		0x0},
	/* [1] l4_hdr over IPV4 */
	{8,		12,		0,		0,
	0,		0,		0,		0,		1,
	0x1f,		0x1e,		0x0,		0x0,
	0x4,		0x0,		0x0,		0x0},
	/* [2] l3_pkt - IPV6 */
	{11,		0,		0,		0,
	0,		0,		0,		0,		1,
	0x1f,		0x1f,		0x0,		0x0,
	0x4,		0x0,		0x0,		0x0},
	/* [3] l4_hdr over IPV6 */
	{11,		12,		0,		0,
	0,		0,		0,		0,		1,
	0x1f,		0x1e,		0x0,		0x0,
	0x4,		0x0,		0x0,		0x0},
	/* [4] IPV4 over IPV4 */
	{8,		0,		8,		0,
	4,		0,		0,		0,		1,
	0x1f,		0x0,		0x1f,		0x1f,
	0x4,		0x0,		0x0,		0x0},
	/* [5] l4_hdr over IPV4 over IPV4 */
	{8,		0,		8,		12,
	4,		0,		0,		0,		1,
	0x1f,		0x0,		0x1f,		0x1e,
	0x4,		0x0,		0x0,		0x0},
	/* [6] IPV4 over IPV6 */
	{11,		0,		8,		0,
	4,		0,		0,		0,		1,
	0x1f,		0x0,		0x1f,		0x1f,
	0x4,		0x0,		0x0,		0x0},
	/* [7] l4_hdr over IPV4 over IPV6 */
	{11,		0,		8,		12,
	4,		0,		0,		0,		1,
	0x1f,		0x0,		0x1f,		0x1e,
	0x4,		0x0,		0x0,		0x0},
	/* [8] IPV6 over IPV4 */
	{8,		0,		11,		0,
	4,		0,		0,		0,		1,
	0x1f,		0x0,		0x1f,		0x1f,
	0x4,		0x0,		0x0,		0x0},
	/* [9] l4_hdr over IPV6 over IPV4 */
	{8,		0,		11,		12,
	4,		0,		0,		0,		1,
	0x1f,		0x0,		0x1f,		0x1e,
	0x4,		0x0,		0x0,		0x0},
	/* [10] IPV6 over IPV6 */
	{11,		0,		11,		0,
	4,		0,		0,		0,		1,
	0x1f,		0x0,		0x1f,		0x1f,
	0x4,		0x0,		0x0,		0x0},
	/* [11] l4_hdr over IPV6 over IPV6 */
	{11,		0,		11,		12,
	4,		0,		0,		0,		1,
	0x1f,		0x0,		0x1f,		0x1e,
	0x4,		0x0,		0x0,		0x0},
	/* [31] default match */
	[31] = {
	0,		0,		0,		0,
	0,		0,		0,		0,		1,
	0x0,		0x0,		0x0,		0x0,
	0x0,		0x0,		0x0,		0x0}
};

#define RX_GCP_TABLE_ENTRY_ETH_V4_STUB \
	0, 0, 0, \
	0, 0, 0, 0, \
	0, 0, 0, 0, 0, 0, 0, \
	0, 0, 0, \
	0, 0,

static struct al_mod_eth_rx_gcp_table_entry
al_mod_eth_generic_rx_crc_gcp_v3[AL_ETH_RX_GCP_TABLE_SIZE] = {
	/* [0] l3_pkt - IPV4 */
	{0,		0,		0,		0,		0,
	 0,		0,		0,		0,		0,
	 0,		0,		0,		0,		0,
	 0,		0,		{0x00000000,	0x00000000,	0x00000000,
	 0x00000000,	0x00000000,	0x00000000},	0x00000000,	0x00000001,
	 0, RX_GCP_TABLE_ENTRY_ETH_V4_STUB},
	/* [1] l4_hdr over IPV4 */
	{0,		0,		0,		0,		0,
	 0,		0,		0,		0,		0,
	 0,		0,		0,		0,		0,
	 0,		0,		{0x00000000,	0x00000000,	0x00000000,
	 0x00000000,	0x00000000,	0x00000000},	0x00000000,	0x00000041,
	 0, RX_GCP_TABLE_ENTRY_ETH_V4_STUB},
	/* [2] l3_pkt - IPV6 */
	{0,		0,		0,		0,		0,
	 0,		0,		0,		0,		0,
	 0,		0,		0,		0,		0,
	 0,		0,		{0x00000000,	0x00000000,	0x00000000,
	 0x00000000,	0x00000000,	0x00000000},	0x00000000,	0x00000000,
	 0, RX_GCP_TABLE_ENTRY_ETH_V4_STUB},
	/* [3] l4_hdr over IPV6 */
	{0,		0,		0,		0,		0,
	 0,		0,		0,		0,		0,
	 0,		0,		0,		0,		0,
	 0,		0,		{0x00000000,	0x00000000,	0x00000000,
	 0x00000000,	0x00000000,	0x00000000},	0x00000000,	0x00000040,
	 0, RX_GCP_TABLE_ENTRY_ETH_V4_STUB},
	/* [4] IPV4 over IPV4 */
	{0,		0,		0,		0,		0,
	 0,		0,		0,		0,		0,
	 0,		0,		0,		0,		0,
	 0,		0,		{0x00000000,	0x00000000,	0x00000000,
	 0x00000000,	0x00000000,	0x00000000},	0x00000000,	0x00008004,
	 0, RX_GCP_TABLE_ENTRY_ETH_V4_STUB},
	/* [5] l4_hdr over IPV4 over IPV4 */
	{0,		0,		0,		0,		0,
	 0,		0,		0,		0,		0,
	 0,		0,		0,		0,		0,
	 0,		0,		{0x00000000,	0x00000000,	0x00000000,
	 0x00000000,	0x00000000,	0x00000000},	0x00000000,	0x00008804,
	 0, RX_GCP_TABLE_ENTRY_ETH_V4_STUB},
	/* [6] IPV4 over IPV6 */
	{0,		0,		0,		0,		0,
	 0,		0,		0,		0,		0,
	 0,		0,		0,		0,		0,
	 0,		0,		{0x00000000,	0x00000000,	0x00000000,
	 0x00000000,	0x00000000,	0x00000000},	0x00000000,	0x00008000,
	 0, RX_GCP_TABLE_ENTRY_ETH_V4_STUB},
	/* [7] l4_hdr over IPV4 over IPV6 */
	{0,		0,		0,		0,		0,
	 0,		0,		0,		0,		0,
	 0,		0,		0,		0,		0,
	 0,		0,		{0x00000000,	0x00000000,	0x00000000,
	 0x00000000,	0x00000000,	0x00000000},	0x00000000,	0x00008800,
	 0, RX_GCP_TABLE_ENTRY_ETH_V4_STUB},
	/* [8] IPV6 over IPV4 */
	{0,		0,		0,		0,		0,
	 0,		0,		0,		0,		0,
	 0,		0,		0,		0,		0,
	 0,		0,		{0x00000000,	0x00000000,	0x00000000,
	 0x00000000,	0x00000000,	0x00000000},	0x00000000,	0x00000004,
	 0, RX_GCP_TABLE_ENTRY_ETH_V4_STUB},
	/* [9] l4_hdr over IPV6 over IPV4 */
	{0,		0,		0,		0,		0,
	 0,		0,		0,		0,		0,
	 0,		0,		0,		0,		0,
	 0,		0,		{0x00000000,	0x00000000,	0x00000000,
	 0x00000000,	0x00000000,	0x00000000},	0x00000000,	0x00000804,
	 0, RX_GCP_TABLE_ENTRY_ETH_V4_STUB},
	/* [10] IPV6 over IPV6 */
	{0,		0,		0,		0,		0,
	 0,		0,		0,		0,		0,
	 0,		0,		0,		0,		0,
	 0,		0,		{0x00000000,	0x00000000,	0x00000000,
	 0x00000000,	0x00000000,	0x00000000},	0x00000000,	0x00000000,
	 0, RX_GCP_TABLE_ENTRY_ETH_V4_STUB},
	/* [11] l4_hdr over IPV6 over IPV6 */
	{0,		0,		0,		0,		0,
	 0,		0,		0,		0,		0,
	 0,		0,		0,		0,		0,
	 0,		0,		{0x00000000,	0x00000000,	0x00000000,
	 0x00000000,	0x00000000,	0x00000000},	0x00000000,	0x00000800,
	 0, RX_GCP_TABLE_ENTRY_ETH_V4_STUB},
	/* [31] default match */
	[31] = {
	0,		0,		0,		0,		0,
	0,		0,		0,		0,		0,
	0,		0,		0,		0,		0,
	0,		0,		{0x00000000,	0x00000000,	0x00000000,
	0x00000000,	0x00000000,	0x00000000},	0x00000000,	0x01008904,
	 0, RX_GCP_TABLE_ENTRY_ETH_V4_STUB},
};

#endif	/* __AL_HAL_ETH_CRC_TABLES_H__ */
