/*
 * Copyright 2018, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */
#ifndef __AL_HAL_LIVE_UPDATE_INTERNAL_H__
#define __AL_HAL_LIVE_UPDATE_INTERNAL_H__

#include "al_mod_hal_live_update.h"

#define AL_HAL_LU_SWITCH_BUFF(__packed) \
	(__packed->active_buff = 1 - __packed->active_buff)
#define AL_HAL_LU_SET_PACKED_VER(__packed, __ver) \
	(__packed->ver = __ver)

/** Convert a packed structure to the destination version, using the convert functions.
 *
 * @param packed pointer to the packed structure
 * @param hal_dst_ver destination version
 * @param convert_up_funcs pointer to an array of convert up functions
 * @param convert_down_funcs pointer to an array of convert down functions
 * @param get_ver_func pointer to a function that reads the struct version from the struct
 * @param convert_funcs_count length of the convert_functions arrays
 *
 * @return 0 on success. Error code otherwise.
 */
int al_mod_lu_packed_convert(void *packed,
			enum al_mod_lu_hal_packed_ver hal_dst_ver,
			struct al_mod_lu_ver_to_func_tuple *convert_up_funcs,
			struct al_mod_lu_ver_to_func_tuple *convert_down_funcs,
			enum al_mod_lu_hal_packed_ver get_ver_func(void *packed),
			unsigned int convert_funcs_count);

#endif
