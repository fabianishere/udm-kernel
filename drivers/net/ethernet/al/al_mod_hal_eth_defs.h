/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

/**
 *  @{
 * @file   al_mod_hal_eth_defs.h
 *
 * @brief HAL Ethernet definitions
 *
 */
#ifndef __AL_HAL_ETH_DEFS_H__
#define __AL_HAL_ETH_DEFS_H__

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

#define AL_ETH_TX_FLAGS_TSO		AL_BIT(7)  /**< Enable TCP/UDP segmentation offloading */

/* MDIO */
/* Reference clock frequency (platform specific) */
enum al_mod_eth_ref_clk_freq {
	AL_ETH_REF_FREQ_375_MHZ		= 0,
	AL_ETH_REF_FREQ_187_5_MHZ	= 1,
	AL_ETH_REF_FREQ_250_MHZ		= 2,
	AL_ETH_REF_FREQ_500_MHZ		= 3,
	AL_ETH_REF_FREQ_428_MHZ         = 4,
};

#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */
#endif		/* __AL_HAL_ETH_DEFS_H__ */
/** @} end of Ethernet group */
