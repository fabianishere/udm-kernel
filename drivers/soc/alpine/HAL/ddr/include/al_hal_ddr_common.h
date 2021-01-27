/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

/**
 * @defgroup groupddr DDR controller & PHY hardrware abstraction layer
 *  @{
 * @file   al_hal_ddr_common.h
 *
 * @brief Common file for the DDR HAL
 */

#ifndef __AL_HAL_DDR_COMMON_H__
#define __AL_HAL_DDR_COMMON_H__

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

/* DDR type enumeration */
enum al_ddr_type {
	AL_DDR_TYPE_DDR3,
	AL_DDR_TYPE_DDR4
};

/* DDR device enumeration */
enum al_ddr_device {
	AL_DDR_DEVICE_X4,
	AL_DDR_DEVICE_X8,
	AL_DDR_DEVICE_X16
};

/** DIMM type enumeration */
enum al_ddr_dimm_type {
	AL_DDR_DIMM_TYPE_UDIMM,
	AL_DDR_DIMM_TYPE_SODIMM,
	AL_DDR_DIMM_TYPE_RDIMM,
	AL_DDR_DIMM_TYPE_LRDIMM,
	AL_DDR_DIMM_TYPE_NODIMM
};

/* Length of read/write preamble in DDR4 mode.
 * Equivalent to MR4-OP[12]
 */
enum al_ddr_preamble_length {
	AL_DDR_PREAMBLE_LENGTH_1TCK = 0,
	AL_DDR_PREAMBLE_LENGTH_2TCK,
};

/* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */
/** @} end of DDR group */
#endif

