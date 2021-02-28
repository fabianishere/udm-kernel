/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

/**
 * @defgroup group_serdes_init SerDes Initialization
 * @ingroup group_serdes SerDes
 * @{
 *
 * @file   al_hal_serdes_snps_init.h
 *
 */

#ifndef __AL_HAL_SERDES_SNPS_INIT_H__
#define __AL_HAL_SERDES_SNPS_INIT_H__

#include "al_hal_serdes_snps.h"

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

enum al_serdes_snps_mode {
	/*
	 * Lane muxing skipped
	 * Supported by lanes: All
	 */
	AL_SRDS_SNPS_MODE_SKIP,

	/*
	 * Quad is part of PCIE 0
	 * Supported by Alpine V4 main complex quads: 0, 1
	 */
	AL_SRDS_SNPS_MODE_PCIE_0,
};

/* Lane configuration */
struct al_serdes_snps_quad_cfg {
	enum al_serdes_snps_mode	mode;
};

/* Complex configuration */
struct al_serdes_snps_complex_cfg {
	struct al_serdes_complex_obj	*complex_obj;
	struct al_serdes_snps_quad_cfg	*quad_cfg;
	unsigned int			num_quads;
};

/** Serdes initialization params */
struct al_serdes_snps_init_params {
	/** A list of complex configuration pointers - the first one must be the master complex */
	struct al_serdes_snps_complex_cfg	*complex_cfgs;
	/** The number of complexes in the list */
	unsigned int				num_complexes;
};

/**
 * SNPS SerDes init (FW download, muxing, clock selection, and mode configuration)
 *
 * @param	init_params
 *		Initialization parameters
 *
 * @returns	0 upon success
 */
int al_serdes_snps_init(
	struct al_serdes_snps_init_params *init_params);

/* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif

/* *INDENT-ON* */
#endif

/** @} end of SERDES group */

