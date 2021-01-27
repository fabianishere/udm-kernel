/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#include "al_err_events.h"
#include "al_hal_pbs_utils.h"

/*******************************************************************************
 ** Module functions
 ******************************************************************************/
int al_err_events_module_collect(struct al_err_events_module *module,
				 enum al_err_events_collect collect)
{
	al_assert(module);
	al_assert_msg(module->collect, "Undefined collect function for %s\n", module->name);
	al_assert(collect != AL_ERR_EVENTS_COLLECT_DEFAULT);

	if (!module->enabled) {
		al_err("%s: ERROR module %s is not enabled\n",
			__func__,
			module->name);
		return -EINVAL;
	}

	return module->collect(module, collect);
}

void al_err_events_module_print(struct al_err_events_module *module)
{
	al_assert(module);
	al_assert_msg(module->print, "Undefined print function for %s\n", module->name);

	module->print(module);
}

void al_err_events_module_clear(struct al_err_events_module *module)
{
	al_assert(module);
	al_assert_msg(module->clear, "Undefined clear function for %s\n", module->name);

	module->clear(module);
}

al_bool al_err_events_module_test(struct al_err_events_module *module)
{
	al_assert(module);

	if (module->test)
		return module->test(module);

	return AL_TRUE;
}

void al_err_events_module_ints_mask(struct al_err_events_module *module)
{
	unsigned int i;

	al_assert(module);

	for (i = 0; i < module->fields_size; i++) {
		struct al_err_events_field *field = &module->fields[i];

		if (!field->valid || (field->collect_mode != AL_ERR_EVENTS_COLLECT_INTERRUPT))
			continue;

		field->mask(field);
	}
}

/*******************************************************************************
 ** Handle functions
 ******************************************************************************/
int al_err_events_handle_init(struct al_err_events_handle *handle,
			      struct al_err_events_handle_init_params *init)
{
	al_assert(handle);
	al_assert(init);
	al_assert(init->pbs_regs_base);
	al_assert(init->print_cb);
	al_assert(init->error_cb);

	handle->pbs_regs_base = init->pbs_regs_base;
	handle->print_cb = init->print_cb;
	handle->error_cb = init->error_cb;
	handle->error_threshold = init->error_threshold;
	handle->dev_id = al_pbs_dev_id_get(handle->pbs_regs_base);

	handle->first_comp = NULL;

	return 0;
}

void al_err_events_handle_print(struct al_err_events_handle *handle)
{
	al_err_events_print_cb print_cb;
	struct al_err_events_module *module;

	al_assert(handle);

	print_cb = handle->print_cb;

	print_cb("********************************************************\n");
	print_cb("**************  ERR_EVENTS REPORT START   **************\n");
	print_cb("********************************************************\n");

	for (module = handle->first_comp; module; module = module->next_comp)
		al_err_events_module_print(module);

	print_cb("********************************************************\n");
	print_cb("**************   ERR_EVENTS REPORT END    **************\n");
	print_cb("********************************************************\n");
}

void al_err_events_handle_clear(struct al_err_events_handle *handle)
{
	struct al_err_events_module *module;

	al_assert(handle);

	for (module = handle->first_comp; module; module = module->next_comp)
		al_err_events_module_clear(module);
}

al_bool al_err_events_handle_test(struct al_err_events_handle *handle)
{
	struct al_err_events_module *module;
	al_bool ret = AL_TRUE;
	al_bool test_res;

	al_assert(handle);

	for (module = handle->first_comp; module; module = module->next_comp) {
		test_res = al_err_events_module_test(module);
		if (!test_res)
			ret = AL_FALSE;
	}

	return ret;
}

int al_err_events_handle_collect(struct al_err_events_handle *handle,
				 enum al_err_events_collect collect)
{
	int ret;
	struct al_err_events_module *module;

	al_assert(handle);

	for (module = handle->first_comp; module; module = module->next_comp) {
		ret = al_err_events_module_collect(module, collect);
		if (ret)
			return ret;
	}

	return 0;
}

void al_err_events_handle_ints_mask(struct al_err_events_handle *handle)
{
	struct al_err_events_module *module;

	al_assert(handle);

	for (module = handle->first_comp; module; module = module->next_comp)
		al_err_events_module_ints_mask(module);
}
