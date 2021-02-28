/*
 * Copyright 2018, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */
#ifndef _AL_HAL_LIVE_UPDATE_H_
#define _AL_HAL_LIVE_UPDATE_H_

enum al_mod_lu_hal_packed_ver {
	AL_LU_HAL_VERSION_INVALID = 0,
	AL_LU_HAL_VERSION_V0 = 100,
	AL_LU_HAL_VERSION_V1 = 200,
	AL_LU_HAL_VERSION_V2 = 300,
	AL_LU_HAL_VERSION_MAX,
};
#define AL_LU_HAL_CURR_VER (AL_LU_HAL_VERSION_V2)

struct al_mod_lu_ver_to_func_tuple {
	enum al_mod_lu_hal_packed_ver ver;
	int (*convert_func)(void *packed);
};

#endif
