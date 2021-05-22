/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#include "al_hal_common.h"
#include "al_dt_parse.h"

/*******************************************************************************
 ** API Functions
 *******************************************************************************/
void al_dt_parse_handle_init(
	struct al_dt_parse_handle *handle,
	const struct al_dt_parse_handle_params *params)
{
	al_assert(handle);
	al_assert(params);
	al_assert(params->node_open);
	al_assert(params->node_close);
	al_assert(params->property_open);
	al_assert(params->property_get);
	al_assert(params->property_close);

	handle->dt_context = params->dt_context;
	handle->node_open = params->node_open;
	handle->node_close = params->node_close;
	handle->property_open = params->property_open;
	handle->property_get = params->property_get;
	handle->property_close = params->property_close;
	handle->debug = params->debug;
}
