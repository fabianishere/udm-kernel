/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */
#ifndef __AL_HAL_PCIE_REG_PTR_SET_REV3_H__
#define __AL_HAL_PCIE_REG_PTR_SET_REV3_H__

#include "al_mod_hal_pcie.h"

void al_mod_pcie_port_handle_init_reg_ptr_set_rev3(
	struct al_mod_pcie_port	*pcie_port,
	void __iomem		*pcie_reg_base);

#endif

