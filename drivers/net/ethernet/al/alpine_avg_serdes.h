/**
 * Copyright 2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 */
#ifndef __ALPINE_AVG_SERDES_H__
#define __ALPINE_AVG_SERDES_H__

#include "al_mod_hal_serdes_avg_init.h"

enum alpine_avg_serdes_cmplx_id {
	ALPINE_AVG_SERDES_CMPLX_ID_MAIN = 0,
	ALPINE_AVG_SERDES_CMPLX_ID_SEC,
	ALPINE_AVG_SERDES_CMPLX_ID_HS,
	ALPINE_AVG_SERDES_CMPLX_ID_MAX
};

struct al_mod_serdes_grp_obj *alpine_avg_serdes_obj_get(
		enum alpine_avg_serdes_cmplx_id type,
		int lane);
void __iomem *alpine_avg_serdes_resource_get(
		enum alpine_avg_serdes_cmplx_id type);

#endif /* __ALPINE_AVG_SERDES_H__ */
