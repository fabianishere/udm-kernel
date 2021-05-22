/*
 * Copyright 2018, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */
#ifndef _AL_HAL_ETH_V3_LM_LIVE_UPDATE_H_
#define _AL_HAL_ETH_V3_LM_LIVE_UPDATE_H_

#include "al_mod_init_eth_lm.h"
#include "al_mod_hal_live_update.h"

/** v3_packed_v0 is 224 bytes, multiply by 2 for future proofing, and round up */
#define AL_LU_ETH_LM_V3_PACKED_DATA_LEN (512)
#define AL_LU_ETH_LM_V3_PACKED_BUFFER_COUNT (2)
#define AL_LU_ETH_LM_V3_PACKED_META_DATA_LEN (128)

/* NOTE: This struct must be allocated 64bit aligned */
struct al_mod_hal_eth_lm_v3_packed_container {
	uint8_t packed[AL_LU_ETH_LM_V3_PACKED_DATA_LEN * AL_LU_ETH_LM_V3_PACKED_BUFFER_COUNT +
			AL_LU_ETH_LM_V3_PACKED_META_DATA_LEN];
} __packed_a8;

/**
 * Pack an al_mod_eth_lm_context struct
 *
 * @param lm_packed_cont pointer to an initialized
 *        al_mod_hal_eth_lm_v3_packed_container struct
 * @param eth_lm pointer to an al_mod_eth_lm_context struct to pack
 * @param idx instance index used for distinguishing between packed structs if
 *        using multiple instances
 * @param dst_ver HAL destination version to pack to. Use HAL_CURR_VER for
 *        current version
 *
 * @return 0 on success. Error code otherwise.
 */
int al_mod_eth_lm_v3_handle_pack(struct al_mod_hal_eth_lm_v3_packed_container *lm_packed_cont,
			     struct al_mod_eth_lm_context *eth_lm,
			     int idx,
			     enum al_mod_lu_hal_packed_ver dst_ver);

/**
 * Unpack an al_mod_hal_eth_lm_v3_packed_container struct into an
 * al_mod_eth_lm_context struct
 *
 * @param eth_lm pointer to a destination al_mod_eth_lm_context struct, which was
 *        already initialized with al_mod_eth_lm_init
 * @param lm_packed_cont pointer to an al_mod_hal_eth_lm_v3_packed_container struct
 *        to unpack
 *
 * @return 0 on success. Error code otherwise.
 */
int al_mod_eth_lm_v3_handle_unpack(struct al_mod_eth_lm_context *eth_lm,
			       struct al_mod_hal_eth_lm_v3_packed_container *lm_packed_cont);

/**
 * Get the idx value of an al_mod_eth_lm_v3_packed struct
 *
 * @param lm_packed_cont pointer to an al_mod_hal_eth_lm_v3_packed_container struct
 */
unsigned int al_mod_eth_lm_v3_packed_idx_get(
		struct al_mod_hal_eth_lm_v3_packed_container *lm_packed_cont);

#endif
