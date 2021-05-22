/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */
#ifndef __AL_HAL_PCIE_REG_PTR_SET_REV1_H__
#define __AL_HAL_PCIE_REG_PTR_SET_REV1_H__

#include "al_hal_pcie.h"

void al_pcie_port_handle_init_reg_ptr_set_rev1(
	struct al_pcie_port	*pcie_port,
	void __iomem		*pcie_reg_base);

#endif

