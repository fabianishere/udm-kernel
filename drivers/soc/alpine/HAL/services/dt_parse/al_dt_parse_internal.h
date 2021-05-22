/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#ifndef __AL_DT_PARSE_INTERNAL_H__
#define __AL_DT_PARSE_INTERNAL_H__

#include "al_hal_common.h"
#include "al_dt_parse.h"

/**
 * Annapurna Labs Device-tree (DT) Parsing internal functions
 *
 * For internal usage by this service DT parsing components
 */

#define dt_debug(...)				\
	do {					\
		if (handle->debug)		\
			al_warn(__VA_ARGS__);	\
		else				\
			al_dbg(__VA_ARGS__);	\
	} while (0)

#define AL_DT_PARSE_PATH_LEN_MAX		100

/*******************************************************************************
 ** API Functions
 ******************************************************************************/

/**
 * Get a node property value as a single unsigned 32-bit value from a given node context
 *
 * @param handle
 *	DT Parse handle
 * @param node_context
 *	Node context
 * @param property
 *	Name of the property inside the node
 * @param val
 *	Pointer to an unsigned 32-bit variable
 * @param default_val
 *	Pointer to an unsigned 32-bit default value variable
 *	If provided and getting property fails this value will be provided
 *	as 'val', although function returned with fail status.
 *
 * @return
 *	0 on success, errno otherwise.
 */
int al_dt_parse_property_u32_get(
	const struct al_dt_parse_handle *handle,
	const void *node_context,
	const char *property,
	uint32_t *val,
	uint32_t *default_val);

/**
 * Get a node property value as a single unsigned 64-bit value from a given node context
 *
 * @param handle
 *	DT Parse handle
 * @param node_context
 *	Node context
 * @param property
 *	Name of the property inside the node
 * @param val
 *	Pointer to an unsigned 64-bit variable
 * @param default_val
 *	Pointer to an unsigned 64-bit default value variable
 *	If provided and getting property fails this value will be provided
 *	as 'val', although function returned with fail status.
 *
 * @return
 *	0 on success, errno otherwise.
 */
int al_dt_parse_property_u64_get(
	const struct al_dt_parse_handle *handle,
	const void *node_context,
	const char *property,
	uint64_t *val,
	uint64_t *default_val);

/**
 * Open a node property value as a string value from a given node context
 *
 * @param handle
 *	DT Parse handle
 * @param node_context
 *	Node context
 * @param property
 *	Name of the property inside the node
 * @param property_context
 *	Returned node property context
 * @param val
 *	Pointer to a string pointer
 *
 * @return
 *	0 on success, errno otherwise
 */
int al_dt_parse_property_str_open(
	const struct al_dt_parse_handle *handle,
	const void *node_context,
	const char *property,
	const void **property_context,
	const char **val);

/**
 * Close an node property that was opened as a string
 *
 * @param handle
 *	DT Parse handle
 * @param node_context
 *	Node context
 * @param property_context
 *	Node property context
 *
 * @return
 *	none
 */
void al_dt_parse_property_str_close(
	const struct al_dt_parse_handle *handle,
	const void *node_context,
	const void *property_context);

/**
 * Compare a node property value as a string value from a given node context to a provided string
 *
 * @param handle
 *	DT Parse handle
 * @param node_context
 *	Node context
 * @param property
 *	Name of the property inside the node
 * @param cmp_str
 *	String to compare to
 *
 * @return
 *	AL_TRUE if property exists and content is equals to given string,
 *	AL_FALSE otherwise
 */
al_bool al_dt_parse_property_str_eq(
	const struct al_dt_parse_handle *handle,
	const void *node_context,
	const char *property,
	const char *cmp_str);

#endif /* __AL_DT_PARSE_H__ */
