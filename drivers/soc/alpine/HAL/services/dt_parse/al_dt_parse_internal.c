/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#include "al_hal_common.h"
#include "al_dt_parse.h"
#include "al_dt_parse_internal.h"

#ifndef swap64_from_be
#define swap64_from_be(x)	__builtin_bswap64(x)
#endif

/*******************************************************************************
 ** API Functions
 *******************************************************************************/
int al_dt_parse_property_u32_get(
	const struct al_dt_parse_handle *handle,
	const void *node_context,
	const char *property,
	uint32_t *val,
	uint32_t *default_val)
{
	const void *property_context;
	const void *val_ptr;
	int len;
	int ret = 0;

	ret = handle->property_open(handle->dt_context, node_context, property, &property_context);
	if (ret)
		goto done;

	ret = handle->property_get(handle->dt_context, node_context, property_context,
		&val_ptr, &len);
	if (ret)
		goto close;

	if (len != sizeof(uint32_t)) {
		ret = -EINVAL;
		goto close;
	}

	/* DT specification defines big-endian */
	*val = swap32_from_be(*((const uint32_t *)val_ptr));

close:
	handle->property_close(handle->dt_context, node_context, property_context);

done:
	/**
	 * Apply default value
	 * if we failed to get real value and default value is provided
	 */
	if (ret && (default_val != NULL))
		*val = *default_val;

	return ret;
}

int al_dt_parse_property_u64_get(
	const struct al_dt_parse_handle *handle,
	const void *node_context,
	const char *property,
	uint64_t *val,
	uint64_t *default_val)
{
	const void *property_context;
	const void *val_ptr;
	int len;
	int ret = 0;

	ret = handle->property_open(handle->dt_context, node_context, property, &property_context);
	if (ret)
		goto done;

	ret = handle->property_get(handle->dt_context, node_context, property_context,
		&val_ptr, &len);
	if (ret)
		goto close;

	if (len != sizeof(uint64_t)) {
		ret = -EINVAL;
		goto close;
	}

	/* DT specification defines big-endian */
	al_memcpy(val, val_ptr, sizeof(uint64_t));
	*val = swap64_from_be(*val);

close:
	handle->property_close(handle->dt_context, node_context, property_context);
done:
	/**
	 * Apply default value
	 * if we failed to get real value and default value is provided
	 */
	if (ret && (default_val != NULL))
		*val = *default_val;

	return ret;
}

int al_dt_parse_property_str_open(
	const struct al_dt_parse_handle *handle,
	const void *node_context,
	const char *property,
	const void **property_context,
	const char **val)
{
	const void *val_ptr;
	int len;
	int ret = 0;

	ret = handle->property_open(handle->dt_context, node_context, property, property_context);
	if (ret)
		return -EINVAL;

	ret = handle->property_get(handle->dt_context, node_context, *property_context,
		&val_ptr, &len);
	if (ret)
		goto close;

	*val = (const char *)val_ptr;

	/* We expect at least '\0' */
	if (len <= 0) {
		ret = -EINVAL;
		goto close;
	}

	/* String must end with '\0' */
	if ((*val)[len - 1] != '\0') {
		ret = -EINVAL;
		goto close;
	}

	return 0;

close:
	handle->property_close(handle->dt_context, node_context, *property_context);

	return ret;
}

void al_dt_parse_property_str_close(
	const struct al_dt_parse_handle *handle,
	const void *node_context,
	const void *property_context)
{
	handle->property_close(handle->dt_context, node_context, property_context);
}

al_bool al_dt_parse_property_str_eq(
	const struct al_dt_parse_handle *handle,
	const void *node_context,
	const char *property,
	const char *cmp_str)
{
	const void *property_context;
	const char *str;
	int ret;

	ret = al_dt_parse_property_str_open(handle, node_context, property,
		&property_context, &str);
	if (ret)
		return AL_FALSE;

	ret = al_strcmp(str, cmp_str);

	al_dt_parse_property_str_close(handle, node_context, property_context);

	return (ret == 0);
}
