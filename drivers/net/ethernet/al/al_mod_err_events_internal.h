/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#ifndef __AL_ERR_EVENTS_INTERNAL_H__
#define __AL_ERR_EVENTS_INTERNAL_H__

#include "al_mod_err_events.h"

struct al_mod_err_events_common_module_params {
	char name[32];

	/** Module primary ID */
	enum al_mod_err_events_primary_module primary_module;

	/** Module secondary ID */
	enum al_mod_err_events_sec_module secondary_module;

	/** Module Primary index ID */
	unsigned int primary_index;

	/** Module secondary index ID */
	unsigned int secondary_index;

	/** collection mode */
	enum al_mod_err_events_collect collect_mode;

	/** IC regs bases */
	void __iomem *ic_regs_bases[AL_ERR_EVENTS_IC_MAX];

	/** Number of ICs */
	unsigned int ic_size;

	unsigned int fields_size;

	struct al_mod_err_events_field *fields;

	const struct al_mod_err_events_field *fields_template;

	/** module private data */
	void *private_data;

	/**
	 * Collect errors
	 *
	 * @param module module object
	 * @param collect collection mode
	 *
	 * @return 0 on success, errno otherwise
	 */
	int (*collect)(struct al_mod_err_events_module *,
		       enum al_mod_err_events_collect);

	/**
	 * Print statistics
	 *
	 * @param module module object
	 *
	 * @return none
	 */
	void (*print)(struct al_mod_err_events_module *);

	/**
	 * Clear statistics
	 *
	 * @param module module object
	 *
	 * @return none
	 */
	void (*clear)(struct al_mod_err_events_module *);

	/**
	 * Test errors collection
	 *
	 * @param module module object
	 *
	 * @return AL_TRUE on success, AL_FALSE otherwise
	 */
	al_mod_bool (*test)(struct al_mod_err_events_module *);

	/**
	 * Set cause for a field
	 *
	 * @param field field to set cause for
	 *
	 * @return none
	 */
	void (*set_cause)(struct al_mod_err_events_field *field);

	/**
	 * Get a field attributes
	 *
	 * @param field field pointer
	 * @param mask_status field is masked
	 * @param cause_status field cause is set
	 * @param abort_status field abort enabled
	 *
	 * @return none
	 */
	void (*get_attrs)(const struct al_mod_err_events_field *field,
		al_mod_bool *mask_status, al_mod_bool *cause_status, al_mod_bool *abort_status);
};

/*******************************************************************************
 ** Field functions
 ******************************************************************************/
/*
 * field error implementation
 *
 * @param field initialized object
 * @param counter number of errors
 *
 * @return none
 */
void al_mod_err_events_common_field_error(struct al_mod_err_events_field *field, unsigned int counter);

/*
 * Print error field statistics
 *
 * @param field initialized object
 * @param print_header print field module information
 *
 * @return none
 */
void al_mod_err_events_common_field_print(struct al_mod_err_events_field *field, al_mod_bool print_header);

/*
 * collect field iofic error
 *
 * @param field initialized object
 * @param cause cause register value
 *
 * @return none
 */
void al_mod_err_events_common_field_iofic_collect(struct al_mod_err_events_field *field, uint32_t cause);

/*
 * Field set cause
 *
 * @param field field pointer
 *
 * @return none
 */
void al_mod_err_events_common_field_set_cause(struct al_mod_err_events_field *field);

/**
 * Get a field attributes
 *
 * @param field field pointer
 * @param mask_status field is masked
 * @param cause_status field cause is set
 * @param abort_status field abort enabled
 *
 * @return none
 */
void al_mod_err_events_common_field_get_attrs(const struct al_mod_err_events_field *field,
	al_mod_bool *mask_status, al_mod_bool *cause_status, al_mod_bool *abort_status);

/*
 * empty collect field function
 *
 * @param field initialized object
 * @param cause cause register value
 *
 * @return none
 */
void al_mod_err_events_common_field_empty_collect(struct al_mod_err_events_field *field, uint32_t cause);

/*
 * mask field iofic error
 *
 * @param field initialized object
 *
 * @return none
 */
void al_mod_err_events_common_field_iofic_mask(struct al_mod_err_events_field *field);

/*
 * empty mask field function
 *
 * @param field initialized object
 *
 * @return none
 */
void al_mod_err_events_common_field_empty_mask(struct al_mod_err_events_field *field);

/*******************************************************************************
 ** Module functions
 ******************************************************************************/
/*
 * Add module to error handler
 *
 * @param handle initialized object
 * @param module initialized object
 *
 * @return none
 */
void al_mod_err_events_common_module_init(struct al_mod_err_events_handle *handle,
				      struct al_mod_err_events_module *module,
				      struct al_mod_err_events_common_module_params *params);

/*
 * do post actions on the module after it is initialized
 *
 * @param module initialized object
 *
 * @return none
 */
void al_mod_err_events_common_module_post_init(struct al_mod_err_events_module *module);

/*
 * run self test on module
 *
 * @param module initialized object
 *
 * @return AL_TRUE if test passed, AL_FALSE otherwise
 */
al_mod_bool al_mod_err_events_common_module_test(struct al_mod_err_events_module *module);

/*
 * clear module statistics
 *
 * @param module initialized object
 *
 * @return none
 */
void al_mod_err_events_common_module_clear(struct al_mod_err_events_module *module);

/*
 * print module statistics
 *
 * @param module initialized object
 *
 * @return none
 */
void al_mod_err_events_common_module_print(struct al_mod_err_events_module *module);

/*
 * collect module errors and run callback in case an error is found
 *
 * @param module initialized object
 * @param collect collection mode
 * @param iofic_id IOFIC ID number
 * @param iofic_group IOFIC group number
 *
 * @return 0 on success, errno otherwise
 */
int al_mod_err_events_common_module_iofic_collect(struct al_mod_err_events_module *module,
						 enum al_mod_err_events_collect collect,
						 unsigned int iofic_id,
						 unsigned int iofic_group);

/*
 * Get iofic error field mask for a given group
 *
 * @param module initialized object
 * @param iofic_id IOFIC ID number
 * @param iofic_group IOFIC group number
 *
 * @return error mask
 */
uint32_t al_mod_err_events_common_group_mask_get(struct al_mod_err_events_module *module,
						unsigned int iofic_id,
						unsigned int iofic_group);

/*
 * Update non valid fields in module according to mask
 *
 * @param module initialized object
 * @param iofic_id IOFIC ID number
 * @param a_mask iofic group A mask
 * @param b_mask iofic group B mask
 * @param c_mask iofic group C mask
 * @param d_mask iofic group D mask
 */
void al_mod_err_event_common_update_valid_fields(struct al_mod_err_events_module *module,
					     unsigned int iofic_id,
					     uint32_t a_mask,
					     uint32_t b_mask,
					     uint32_t c_mask,
					     uint32_t d_mask);

#endif /* __AL_ERR_EVENTS_INTERNAL_H__ */
