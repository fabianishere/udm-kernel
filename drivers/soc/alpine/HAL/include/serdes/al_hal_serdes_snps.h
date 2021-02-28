/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

/**
 * @defgroup group_serdes_api API
 * SerDes HAL driver API
 * @ingroup group_serdes SerDes
 * @{
 *
 * @file   al_hal_serdes_snps.h
 *
 * @brief Header file for the SerDes HAL driver
 *
 */

#ifndef __AL_HAL_SERDES_SNPS_H__
#define __AL_HAL_SERDES_SNPS_H__

#include "al_hal_common.h"
#include "al_hal_serdes_interface.h"
#include "al_hal_sbus.h"

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

/* Number of quads per complex type */
#define AL_SRDS_QUADS_PER_COMPLEX_TC7_MAIN	2
#define AL_SRDS_QUADS_PER_COMPLEX_MAX		2

/* SerDes complex types */
enum al_serdes_complex_type {
	/* Alpine V4 main complex */
	AL_SRDS_COMPLEX_TYPE_TC7_MAIN,
};

struct al_serdes_complex_obj {
	void __iomem			*regs_base;
	enum al_serdes_complex_type	complex_type;
	struct al_serdes_grp_obj	*quads[AL_SRDS_QUADS_PER_COMPLEX_MAX];
};

/**
 * SerDes complex handle initialization
 *
 * @param	regs_base
 *		Complex register base
 * @param	complex_type
 *		Complex type
 * @param	obj
 *		A pointer to a non initialized complex handle structure
 */
void al_serdes_snps_complex_handle_init(
	void __iomem			*regs_base,
	enum al_serdes_complex_type	complex_type,
	struct al_serdes_complex_obj	*obj);

/**
 * SerDes quad handle initialization
 *
 * @param	regs_base
 *		Lane register base
 * @param	complex_obj
 *		Parent complex initialized object
 * @param	quad_in_complex
 *		The number of the quad inside its parent complex
 * @param	obj
 *		A pointer to a non initialized quad handle structure
 */
void al_serdes_snps_quad_handle_init(
	void __iomem			*regs_base,
	struct al_serdes_complex_obj	*complex_obj,
	unsigned int			quad_in_complex,
	struct al_serdes_grp_obj	*obj);

/* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif

/* *INDENT-ON* */
#endif

/** @} end of SERDES group */

