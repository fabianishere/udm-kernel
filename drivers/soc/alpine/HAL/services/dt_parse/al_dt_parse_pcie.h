/*
 * Copyright 2018, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#ifndef __AL_DT_PARSE_PCIE_H__
#define __AL_DT_PARSE_PCIE_H__

#include "al_hal_common.h"
#include "al_dt_parse.h"
#include "al_hal_pcie.h"
#include "al_init_pcie.h"

/**
 * Annapurna Labs PCIE related Device-tree (DT) nodes parsing functions
 */

/**
 * Parse PCIE parameters struct from DT
 * @param handle		DT parse handle
 * @param pcie_params		PCIE parameters
 *
 * @return 0 on success, errno otherwise
 */
int al_dt_parse_pcie_params(const struct al_dt_parse_handle *handle,
		struct al_pcie_init_params_adv *pcie_params);

#endif
