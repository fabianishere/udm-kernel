/*
 * Copyright 2018, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */
#include "al_mod_hal_live_update.h"
#include "al_mod_hal_live_update_internal.h"
#include "al_mod_hal_common.h"

/* Convert packed struct to a desired version
 * This function is being used during pack/unpack, and not intend to be used by the user */
int al_mod_lu_packed_convert(void *packed,
			enum al_mod_lu_hal_packed_ver hal_dst_ver,
			struct al_mod_lu_ver_to_func_tuple *convert_up_funcs,
			struct al_mod_lu_ver_to_func_tuple *convert_down_funcs,
			enum al_mod_lu_hal_packed_ver get_ver_func(void *packed),
			unsigned int convert_funcs_count)

{
	unsigned int i;
	int rc;

	al_mod_assert(hal_dst_ver != AL_LU_HAL_VERSION_INVALID);
	al_mod_assert(hal_dst_ver != AL_LU_HAL_VERSION_MAX);

	/* Handle convert-up functions */
	if (get_ver_func(packed) < hal_dst_ver) {
		/* Find our location in the convert_up_funcs array */
		for (i = 0; i < convert_funcs_count; i++) {
			if (get_ver_func(packed) <= convert_up_funcs[i].ver)
				break;
		}

		/* Perform the conversions iteratively, until getting to an updated enough */
		for (; (get_ver_func(packed) < hal_dst_ver) && (i < convert_funcs_count); i++) {
			rc = convert_up_funcs[i].convert_func(packed);
			if (rc != 0)
				return rc;
		}

		return 0;
	}

	/* Handle convert-down functions */
	if (get_ver_func(packed) > hal_dst_ver) {
		/* Find our location in the convert_down_funcs array */
		for (i = convert_funcs_count; i > 0; i--) {
			if (get_ver_func(packed) >= convert_down_funcs[i - 1].ver)
				break;
		}

		/* Perform the conversions iteratively, until getting to an old enough */
		for (; (get_ver_func(packed) > hal_dst_ver) && (i > 0); i--) {
			rc = convert_down_funcs[i - 1].convert_func(packed);
			if (rc != 0)
				return rc;
		}

		return 0;
	}

	return 0;
}
