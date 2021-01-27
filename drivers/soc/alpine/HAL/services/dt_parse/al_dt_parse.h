/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#ifndef __AL_DT_PARSE_H__
#define __AL_DT_PARSE_H__

#include "al_hal_plat_types.h"

/**
 * Annapurna Labs Device-tree (DT) Parsing functions
 */

/*******************************************************************************
 ** Initialization Structures
 ******************************************************************************/

/**
 * Device-tree (DT) Parsing handle parameters
 */
struct al_dt_parse_handle_params {
	/** DT context for DT functions */
	void				*dt_context;

	/**
	 * Open a DT node
	 *
	 * Notice: Returned node context is implementation defined.
	 *
	 * @param dt_context
	 *	DT context
	 * @param path
	 *	Path to the node
	 * @param node_context
	 *	Node context will be returned if node is found
	 *
	 * @return
	 *	0 if node is found, errno otherwise.
	 */
	int				(*node_open)(
						void *dt_context,
						const char *path,
						const void **node_context);

	/**
	 * Close opened DT node
	 *
	 * @param dt_context
	 *	DT context
	 * @param node_context
	 *	Node context
	 *
	 * @return
	 *	none
	 */
	void				(*node_close)(
						void *dt_context,
						const void *node_context);

	/**
	 * Open a DT node property
	 *
	 * @param dt_context
	 *	DT context
	 * @param node_context
	 *	Node context
	 * @param property
	 *	Name of the property inside the node
	 * @param property_context
	 *	Node property context will be returned if property is found in node
	 *
	 * @return
	 *	0 if property is found, errno otherwise.
	 */
	int				(*property_open)(
						void *dt_context,
						const void *node_context,
						const char *property,
						const void **property_context);

	/**
	 * Open DT node property value from a property context
	 *
	 * @param dt_context
	 *	DT context
	 * @param node_context
	 *	Node context
	 * @param property_context
	 *	Node property context
	 * @param val
	 *	Value of property will be returned if property is found
	 * @param len
	 *	If not NULL - property value length will be returned
	 *
	 * @return
	 *	0 if property is found, errno otherwise.
	 */
	int				(*property_get)(
						void *dt_context,
						const void *node_context,
						const void *property_context,
						const void **val,
						int *len);

	/**
	 * Close opened DT node property
	 *
	 * @param dt_context
	 *	DT context
	 * @param node_context
	 *	Node context
	 * @param property_context
	 *	Node property context
	 *
	 * @return
	 *	none
	 */
	void				(*property_close)(
						void *dt_context,
						const void *node_context,
						const void *property_context);

	/** Debug prints will be printed in higher level */
	al_bool				debug;
};

/*******************************************************************************
 ** Internal Usage Structures
 ******************************************************************************/

struct al_dt_parse_handle {
	/** DT context for DT functions */
	void				*dt_context;

	int				(*node_open)(
						void *dt_context,
						const char *path,
						const void **node_context);

	void				(*node_close)(
						void *dt_context,
						const void *node_context);

	int				(*property_open)(
						void *dt_context,
						const void *node_context,
						const char *property,
						const void **property_context);

	int				(*property_get)(
						void *dt_context,
						const void *node_context,
						const void *property_context,
						const void **val,
						int *len);

	void				(*property_close)(
						void *dt_context,
						const void *node_context,
						const void *property_context);

	/** Debug prints will be printed in higher level */
	al_bool				debug;
};


/*******************************************************************************
 ** API Functions
 ******************************************************************************/

/**
 * DT Parse handle initialization
 *
 * @param handle
 *	DT Parse handle
 * @param params
 *	DT Parse handle parameters
 *
 * @return
 *	none
 */
void al_dt_parse_handle_init(
	struct al_dt_parse_handle *handle,
	const struct al_dt_parse_handle_params *params);

#endif /* __AL_DT_PARSE_H__ */
