/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#include "al_mod_err_events.h"
#include "al_mod_err_events_internal.h"
#include "al_mod_hal_iofic.h"
#include "al_mod_hal_iofic_regs.h"
#include "al_mod_hal_udma_iofic.h"

/* we want to run the self test at least twice
 * in order to make sure we didnt masked anything
 * and to make sure we cleaned up after the first one correctly
 */
#define ERR_EVENTS_SELF_TEST_ITER_NUM		2

/*******************************************************************************
 ** Field functions
 ******************************************************************************/
void al_mod_err_events_common_field_set_cause(struct al_mod_err_events_field *field)
{
	unsigned int ic_id;

	al_mod_assert(field);

	if (!field->parent_module->ic_size) {
		al_mod_warn("%s: Cannot set cause for module %s with ic_size == 0\n",
			__func__, field->parent_module->name);
		return;
	}

	ic_id = field->iofic_id;

	if (!field->parent_module->ic_regs_bases[ic_id])
		al_mod_assert_msg(0, "%s: ic regs base is NULL for ic id %d\n", __func__, ic_id);

	al_mod_iofic_set_cause(field->parent_module->ic_regs_bases[ic_id],
		field->iofic_group, field->iofic_bit);
}

void al_mod_err_events_common_field_get_attrs(const struct al_mod_err_events_field *field,
	al_mod_bool *mask_status, al_mod_bool *cause_status, al_mod_bool *abort_status)
{
	unsigned int ic_id;
	unsigned int grp;
	uint32_t mask, cause, abort_mask;

	al_mod_assert(field);
	al_mod_assert(mask_status);
	al_mod_assert(cause_status);
	al_mod_assert(abort_status);

	if (!field->parent_module->ic_size) {
		al_mod_warn("%s: Cannot read attributes for module %s with ic_size == 0\n",
			__func__, field->parent_module->name);
		return;
	}

	ic_id = field->iofic_id;
	if (!field->parent_module->ic_regs_bases[ic_id])
		al_mod_assert_msg(0, "%s: ic regs base is NULL for ic id %d\n", __func__, ic_id);

	grp = field->iofic_group;

	cause = al_mod_iofic_read_cause(field->parent_module->ic_regs_bases[ic_id], grp);
	mask = al_mod_iofic_read_mask(field->parent_module->ic_regs_bases[ic_id], grp);
	abort_mask = al_mod_iofic_abort_mask_read(field->parent_module->ic_regs_bases[ic_id], grp);

	*mask_status = !!(mask & field->iofic_bit);
	*cause_status = !!(cause & field->iofic_bit);
	*abort_status = !(abort_mask & field->iofic_bit);
}

void al_mod_err_events_common_field_error(struct al_mod_err_events_field *field, unsigned int counter)
{
	al_mod_assert(field);

	field->counter += counter;

	field->parent_module->handle->error_cb(field);

	if ((field->collect_mode != AL_ERR_EVENTS_COLLECT_INTERRUPT) ||
	    !field->parent_module->handle->error_threshold)
		return;

	if (field->parent_module->handle->error_threshold <= field->counter) {
		al_mod_warn("%s: WARN reached error threshold limit. masking %s,%s\n",
			 __func__,
			 field->parent_module->name,
			 field->name);

		field->mask(field);
	}
}

void al_mod_err_events_common_field_print(struct al_mod_err_events_field *field, al_mod_bool print_header)
{
	struct al_mod_err_events_module *module = field->parent_module;
	al_mod_err_events_print_cb print_cb = module->handle->print_cb;


	if (print_header)
		print_cb("%s ", module->name);
	else
		print_cb("\t");

	if (field->valid)
		print_cb("%s: %d\n", field->name, field->counter);
	else
		print_cb("%s: Ignored\n", field->name);
}

void al_mod_err_events_common_field_iofic_collect(struct al_mod_err_events_field *field, uint32_t cause)
{
	al_mod_assert(field);

	if (cause & field->iofic_bit) {
		al_mod_err_events_common_field_error(field, 1);

		al_mod_iofic_clear_cause(field->parent_module->ic_regs_bases[field->iofic_id],
				     field->iofic_group,
				     field->iofic_bit);

		/*
		 * check that the cause is not set after clearing it.
		 * there is a possible scenario that due to false positive the cause
		 * is still enabled but an interrupt will not be generated later on
		 * so this info is important
		 */
		cause = al_mod_iofic_read_cause(field->parent_module->ic_regs_bases[field->iofic_id],
					    field->iofic_group);
		if (cause & field->iofic_bit)
			al_mod_err("err_events: ERR %s,%s cause is still set after clearing it\n",
				field->parent_module->name,
				field->name);
	}
}

void al_mod_err_events_common_field_empty_collect(
		struct al_mod_err_events_field *field __attribute__((unused)),
		uint32_t cause __attribute__((unused)))
{

}

void al_mod_err_events_common_field_empty_mask(
		struct al_mod_err_events_field *field __attribute__((unused)))
{

}

void al_mod_err_events_common_field_iofic_mask(struct al_mod_err_events_field *field)
{
	al_mod_assert(field);

	al_mod_iofic_mask(field->parent_module->ic_regs_bases[field->iofic_id],
		      field->iofic_group,
		      field->iofic_bit);
}

/*******************************************************************************
 ** Module functions
 ******************************************************************************/
void al_mod_err_events_common_module_init(struct al_mod_err_events_handle *handle,
				      struct al_mod_err_events_module *module,
				      struct al_mod_err_events_common_module_params *params)
{
	unsigned int i;

	al_mod_assert(handle);
	al_mod_assert(module);
	al_mod_assert(params);
	al_mod_assert(params->primary_module);
	al_mod_assert(params->private_data);
	al_mod_assert(params->collect);
	al_mod_assert(params->print);
	al_mod_assert(params->clear);
	al_mod_assert(params->ic_size <= AL_ERR_EVENTS_IC_MAX);
	if (params->fields_size) {
		al_mod_assert(params->fields);
		al_mod_assert(params->fields_template);
	}

	module->handle = handle;
	module->primary_module = params->primary_module;
	module->secondary_module = params->secondary_module;
	module->primary_index = params->primary_index;
	module->secondary_index = params->secondary_index;
	module->ic_size = params->ic_size;
	module->private_data = params->private_data;
	module->collect = params->collect;
	module->print = params->print;
	module->clear = params->clear;
	module->test = params->test;
	module->set_cause = params->set_cause;
	module->get_attrs = params->get_attrs;
	module->next_comp = NULL;
	module->fields_size = params->fields_size;
	if (params->fields_size)
		module->fields = params->fields;

	for (i = 0; i < module->ic_size; i++) {
		al_mod_assert(params->ic_regs_bases[i]);
		module->ic_regs_bases[i] = params->ic_regs_bases[i];
	}

	al_mod_memcpy(module->name, params->name, sizeof(module->name));

	if (params->fields_size)
		al_mod_memcpy(module->fields, params->fields_template,
			(module->fields_size * sizeof(struct al_mod_err_events_field)));

	if (params->collect_mode != AL_ERR_EVENTS_COLLECT_DEFAULT)
		for (i = 0; i < module->fields_size; i++)
			module->fields[i].collect_mode = params->collect_mode;

	for (i = 0; i < module->fields_size; i++) {
		al_mod_assert(module->fields[i].print);
		al_mod_assert(module->fields[i].collect);
		al_mod_assert(module->fields[i].mask);
		/**
		 * for usage of int cause regs & their fields we may define ic_size = 0
		 * and therefore we dont compare against fields iofic ID
		 */
		if (module->ic_size)
			al_mod_assert(module->fields[i].iofic_id < module->ic_size);

		module->fields[i].valid = AL_TRUE;
		module->fields[i].parent_module = module;
	}

	module->clear(module);

	/** In module post init we check for polling mode fields */
	module->has_polling_fields = AL_FALSE;

	/* module is now enabled */
	module->enabled = AL_TRUE;

	/* update handle module list */
	if (!handle->first_comp)
		handle->first_comp = module;
	else {
		struct al_mod_err_events_module *mlink = handle->first_comp;

		while (mlink->next_comp)
			mlink = mlink->next_comp;

		mlink->next_comp = module;
	}
}

static void common_module_post_init_ic(struct al_mod_err_events_module *module, unsigned int ic_id)
{
	unsigned int i;
	uint32_t mask[AL_IOFIC_MAX_GROUPS];
	al_mod_bool mask_valid[AL_IOFIC_MAX_GROUPS] = { [0 ...  AL_IOFIC_MAX_GROUPS - 1] = 0 };

	for (i = 0; i < module->fields_size; i++) {
		struct al_mod_err_events_field *field = &module->fields[i];
		unsigned int group = field->iofic_group;

		if (field->iofic_id != ic_id)
			continue;

		if (!mask_valid[group]) {
			mask[group] = al_mod_iofic_read_mask(module->ic_regs_bases[ic_id], group);
			mask_valid[group] = AL_TRUE;
		}

		if (!module->has_polling_fields &&
				(field->collect_mode == AL_ERR_EVENTS_COLLECT_POLLING))
			module->has_polling_fields = AL_TRUE;

		if (field->valid && !(mask[group] & field->iofic_bit) &&
			(field->collect_mode == AL_ERR_EVENTS_COLLECT_POLLING)) {
				al_mod_info("err_events: masking %s %s interrupt "
					"as its configured for polling mode\n",
					module->name,
					field->name);
				field->mask(field);
		}
	}

}

void al_mod_err_events_common_module_post_init(struct al_mod_err_events_module *module)
{
	unsigned int i;

	al_mod_assert(module);

	for (i = 0; i < module->ic_size; i++)
		common_module_post_init_ic(module, i);
}

static al_mod_bool _al_err_events_common_module_self_test(struct al_mod_err_events_module *module,
						     unsigned int iteration)
{
	unsigned int i, j;
	uint32_t flags, int_mask, abort_mask;
	al_mod_bool ret = AL_TRUE;

	for (i = 0; i < module->fields_size; i++) {
		struct al_mod_err_events_field *field = &module->fields[i];
		void __iomem *ic_regs_base = module->ic_regs_bases[field->iofic_id];

		if (!field->valid)
			continue;

		if (field->collect_mode == AL_ERR_EVENTS_COLLECT_POLLING) {
			int_mask = al_mod_iofic_read_mask(ic_regs_base, field->iofic_group);
			if (!(int_mask & field->iofic_bit)) {
				al_mod_err("err_events: ERROR %s %s interrupt is unmasked in poll mode\n",
					module->name,
					field->name);
				ret = AL_FALSE;
			}

			continue;
		}

		if (field->collect_mode != AL_ERR_EVENTS_COLLECT_INTERRUPT) {
			al_mod_err("err_events: ERROR %s %s has invalid collect mode %d\n",
				module->name,
				field->name,
				field->collect_mode);
			continue;
		}

		flags = al_mod_iofic_control_flags_get(ic_regs_base, field->iofic_group);
		if (flags & INT_CONTROL_GRP_CLEAR_ON_READ) {
			al_mod_err("err_events: ERROR %s iofic clear on read flag is set for group %d\n",
				module->name,
				field->iofic_group);
			ret = AL_FALSE;
			continue;
		}

		int_mask = al_mod_iofic_read_mask(ic_regs_base, field->iofic_group);
		if (int_mask & field->iofic_bit) {
			if (iteration == 0)
				al_mod_warn("err_events: WARN skipping self test for %s:%s because "
					"field is masked\n",
					module->name,
					field->name);
			continue;
		}

		al_mod_dbg("testing %s,%s\n", module->name, field->name);

		abort_mask = al_mod_iofic_abort_mask_read(ic_regs_base,
						      field->iofic_group);
		/*
		 * disable abort before generating an interrupt
		 * as this can cause UDMA/application to abort
		 */
		if (!(abort_mask & field->iofic_bit))
			al_mod_iofic_abort_mask_set(ic_regs_base,
						field->iofic_group,
						field->iofic_bit);

		/* create an Interrupt */
		al_mod_iofic_set_cause(ic_regs_base,
				   field->iofic_group,
				   field->iofic_bit);

		al_mod_data_memory_barrier();

		/* wait up to 10 uSec for the interrupt to happen */
		for (j = 0; (j < 1000) && (field->counter == 0); j++) {
			if (!(j % 100))
				al_mod_udelay(1);
		}

		/*
		 * restore abort if it was previously disabled
		 */
		if (!(abort_mask & field->iofic_bit))
			al_mod_iofic_abort_mask_clear(ic_regs_base,
						  field->iofic_group,
						  field->iofic_bit);

		/* check that the counter
		 * was incremented
		 */
		if (field->counter != 1) {
			al_mod_err("err_events: ERROR %s error events test: %s was not updated %d\n",
				module->name,
				field->name,
				field->counter);
			ret = AL_FALSE;
		}

		al_mod_dbg("done testing %s,%s\n", module->name, field->name);
	}

	return ret;
}

void al_mod_err_events_common_module_clear(struct al_mod_err_events_module *module)
{
	unsigned int i;

	for (i = 0; i < module->fields_size; i++)
		module->fields[i].counter = 0;
}

void al_mod_err_events_common_module_print(struct al_mod_err_events_module *module)
{
	unsigned int i;
	al_mod_err_events_print_cb print_cb = module->handle->print_cb;

	print_cb("%s:\n", module->name);

	if (!module->enabled) {
		print_cb("\tIgnored\n");
		return;
	}

	for (i = 0; i < module->fields_size; i++) {
		struct al_mod_err_events_field *field = &module->fields[i];

		al_mod_assert(field->print);

		field->print(field, AL_FALSE);
	}
}

int al_mod_err_events_common_module_iofic_collect(struct al_mod_err_events_module *module,
						 enum al_mod_err_events_collect collect,
						 unsigned int iofic_id,
						 unsigned int iofic_group)
{
	unsigned int i;
	void __iomem *ic_regs_base;
	uint32_t cause;

	al_mod_assert(module);
	al_mod_assert(iofic_id < AL_ERR_EVENTS_IC_MAX && iofic_id < module->ic_size);
	al_mod_assert(iofic_group < AL_IOFIC_MAX_GROUPS);

	if ((collect == AL_ERR_EVENTS_COLLECT_POLLING) && !module->has_polling_fields)
		return 0;

	ic_regs_base = module->ic_regs_bases[iofic_id];

	cause = al_mod_iofic_read_cause(ic_regs_base, iofic_group);
	if (cause) {
		uint32_t mask = al_mod_iofic_read_mask(ic_regs_base, iofic_group);

		for (i = 0; i < module->fields_size; i++) {
			struct al_mod_err_events_field *field = &module->fields[i];

			if (!field->valid ||
			    (field->collect_mode != collect) ||
			    (field->iofic_id != iofic_id) ||
			    (field->iofic_group != iofic_group))
				continue;

			if ((collect == AL_ERR_EVENTS_COLLECT_INTERRUPT) &&
			    (mask & field->iofic_bit))
				continue;

			field->collect(field, cause);
		}
	}

	return 0;
}

al_mod_bool al_mod_err_events_common_module_test(struct al_mod_err_events_module *module)
{
	unsigned int i;
	al_mod_bool res = AL_TRUE;

	if (!module->fields_size) {
		al_mod_dbg("err events: Skipping %s self test as it doesnt consists any fields\n",
			module->name);
		return AL_TRUE;
	}

	for (i = 0; i < module->fields_size; i++) {
		uint32_t int_mask;
		struct al_mod_err_events_field *field = &module->fields[i];
		void __iomem *ic_regs_base = module->ic_regs_bases[field->iofic_id];

		if (!field->valid)
			continue;

		int_mask = al_mod_iofic_read_mask(ic_regs_base, field->iofic_group);
		if (!(int_mask & field->iofic_bit))
			break;
	}

	if (i >= module->fields_size) {
		al_mod_warn("err_events: WARN skipping %s self test because iofic is masked\n",
			module->name);
		return AL_TRUE;
	}

	al_mod_dbg("Running %s self test...\n", module->name);

	for (i = 0; (i < ERR_EVENTS_SELF_TEST_ITER_NUM) && res; i++) {
		res = _al_err_events_common_module_self_test(module, i);
		module->clear(module);
	}

	if (res)
		al_mod_dbg("%s test %s!\n", module->name, "passed");
	else
		al_mod_warn("%s test %s!\n", module->name, "failed");

	return res;
}

uint32_t al_mod_err_events_common_group_mask_get(struct al_mod_err_events_module *module,
						unsigned int iofic_id,
						unsigned int iofic_group)
{
	unsigned int i;
	uint32_t mask = 0;

	al_mod_assert(iofic_id < AL_ERR_EVENTS_IC_MAX && iofic_id < module->ic_size);
	al_mod_assert(iofic_group < AL_IOFIC_MAX_GROUPS);

	for (i = 0; i < module->fields_size; i++) {
		struct al_mod_err_events_field *field = &module->fields[i];

		if (field->valid &&
		    (iofic_id == field->iofic_id) &&
		    (iofic_group == field->iofic_group) &&
		    (field->collect_mode == AL_ERR_EVENTS_COLLECT_INTERRUPT))
			mask |= field->iofic_bit;
	}

	return mask;
}

void al_mod_err_event_common_update_valid_fields(struct al_mod_err_events_module *module,
						unsigned int iofic_id,
						uint32_t a_mask,
						uint32_t b_mask,
						uint32_t c_mask,
						uint32_t d_mask)
{
	uint32_t mask;
	unsigned int i;

	al_mod_assert(iofic_id < AL_ERR_EVENTS_IC_MAX && iofic_id < module->ic_size);

	for (i = 0; i < module->fields_size; i++) {
		struct al_mod_err_events_field *field = &module->fields[i];

		if (field->iofic_id != iofic_id)
			continue;

		switch (field->iofic_group) {
		case AL_INT_GROUP_A:
			mask = a_mask;
			break;
		case AL_INT_GROUP_B:
			mask = b_mask;
			break;
		case AL_INT_GROUP_C:
			mask = c_mask;
			break;
		case AL_INT_GROUP_D:
			mask = d_mask;
			break;
		default:
			al_mod_err("%s: ERROR unknown iofic group %d for field %s\n",
				__func__,
				field->iofic_group,
				field->name);
			al_mod_assert(0);
			return;
		}

		if (!(field->iofic_bit & mask))
			field->valid = AL_FALSE;
	}
}
