/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#include "al_uc_shared.h"
#include "al_err_events_sys_fabric.h"

#define MAX_VALUE		255
#define GET_MIN_VAL(a, b)	al_min_t(uint8_t, (a), (b))

/***************************************************************************************
 * API Functions
 ***************************************************************************************/

void al_uc_shared_handle_init(struct al_uc_shared_handle *handle,
		       const struct al_uc_shared_handle_init_params *init)
{
	al_assert(handle);
	al_assert(init);
	al_assert(init->regs_base);

	handle->regs_base = init->regs_base;
}

void al_uc_shared_data_init(const struct al_uc_shared_handle *handle)
{
	struct al_general_shared_data *shared_data;

	al_assert(handle);

	shared_data = (void *)(uintptr_t)(handle->regs_base);

	shared_data->critical_err = 0;
	shared_data->uncritical_err = 0;
	shared_data->boot_err = 0;
	shared_data->watchdog = 0;
	shared_data->mem_corr_err = 0;
	shared_data->mem_uncorr_err = 0;
	shared_data->pcie_corr_err = 0;
	shared_data->pcie_uncorr_err = 0;
}

al_bool al_uc_shared_fru_vld_get(const struct al_uc_shared_handle *handle)
{
	struct al_uc_shared_data *shared_data;

	al_assert(handle);

	shared_data = (void *)(uintptr_t)(handle->regs_base);

	return shared_data->fru.magic_number == AL_UC_SHARED_FRU_MN;
}

void al_uc_shared_fru_data_get(const struct al_uc_shared_handle *handle,
				struct al_uc_shared_fru_data *fru_data)
{
	struct al_uc_shared_data *shared_data;
	unsigned int i;

	al_assert(handle);
	al_assert(fru_data);

	shared_data = (void *)(uintptr_t)(handle->regs_base);

	al_assert(shared_data->fru.magic_number == AL_UC_SHARED_FRU_MN);

	/* memcpy does not work over PBS memory because it uses SIMD instructions */
	for (i = 0; i < sizeof(struct al_uc_shared_fru_data); i++)
		((uint8_t *)fru_data)[i] = ((uint8_t *)(&shared_data->fru))[i];
}

int al_uc_shared_vt_sensor_data_get(const struct al_uc_shared_handle *handle,
				  enum al_vt_sensor_type type)
{
	struct al_uc_shared_data *shared_data;
	int vt_data;

	al_assert(handle);

	shared_data = (void *)(uintptr_t)(handle->regs_base);

	al_assert(shared_data->telemetry.al_temp_magic_num == AL_UC_SHARED_AL_TEMP_MN);

	switch (type) {
	case al_vt_sensor_type_thermal:
		vt_data = shared_data->telemetry.al_temp_sensors[0];
		break;
	case al_vt_sensor_type_thermal_remote_0:
		vt_data = shared_data->telemetry.al_temp_sensors[1];
		break;
	case al_vt_sensor_type_thermal_remote_1:
		vt_data = shared_data->telemetry.al_temp_sensors[2];
		break;
	case al_vt_sensor_type_thermal_remote_2:
		vt_data = shared_data->telemetry.al_temp_sensors[3];
		break;
	case al_vt_sensor_type_cpu_vdd:
		vt_data = shared_data->telemetry.al_cpu_volt;
		break;
	case al_vt_sensor_type_core_vdd:
		vt_data = shared_data->telemetry.al_core_volt;
		break;
	case al_vt_sensor_type_avdd:
		vt_data = shared_data->telemetry.al_avdd;
		break;
	default:
		vt_data = 0;
		al_assert(0);
	}

	return vt_data;
}

al_bool al_uc_shared_vt_sensor_vld_get(const struct al_uc_shared_handle *handle)
{
	struct al_uc_shared_data *shared_data;

	al_assert(handle);

	shared_data = (void *)(uintptr_t)(handle->regs_base);

	return shared_data->telemetry.al_temp_magic_num == AL_UC_SHARED_AL_TEMP_MN;
}

static void al_uc_shared_mem_err_store(const struct al_uc_shared_handle *handle,
				       const struct al_err_events_field *field)
{
	struct al_general_shared_data *shared_data = (void *)(uintptr_t)(handle->regs_base);

	if (field->sev == AL_ERR_EVENTS_CORRECTABLE) {
		if (field->id == AL_ERR_EVENTS_SYS_FABRIC_DDR_CORRECTABLE_ERROR)
			shared_data->mem_corr_err = GET_MIN_VAL(field->counter, MAX_VALUE);
		else
			shared_data->uncritical_err = GET_MIN_VAL(field->counter, MAX_VALUE);
	} else {
		if (field->id == AL_ERR_EVENTS_SYS_FABRIC_DDR_UNCORRECTABLE_ERROR)
			shared_data->mem_uncorr_err = GET_MIN_VAL(field->counter, MAX_VALUE);
		else
			shared_data->critical_err = GET_MIN_VAL(field->counter, MAX_VALUE);
	}
}

void al_uc_shared_err_events_store(const struct al_uc_shared_handle *handle,
				   const struct al_err_events_field *field)
{
	struct al_general_shared_data *shared_data;

	al_assert(handle);
	al_assert(field);
	al_assert(field->parent_module);

	shared_data = (void *)(uintptr_t)(handle->regs_base);

	switch (field->parent_module->primary_module) {
	case AL_ERR_EVENTS_MODULE_SYS_FABRIC:
		al_uc_shared_mem_err_store(handle, field);
		break;
	default:
		if (field->sev == AL_ERR_EVENTS_CORRECTABLE)
			shared_data->uncritical_err = GET_MIN_VAL(field->counter, MAX_VALUE);
		else
			shared_data->critical_err = GET_MIN_VAL(field->counter, MAX_VALUE);
		break;
	}
}

void al_uc_shared_watchdog_store(const struct al_uc_shared_handle *handle,
				 enum al_uc_shared_watchdog_values wd_val)
{
	struct al_general_shared_data *shared_data;
	uint8_t val;

	al_assert(handle);
	al_assert(((int)wd_val <= AL_UC_SHARED_POWER_DOWN) &&
		  ((int)wd_val >= AL_UC_SHARED_TIMER_EXPIRED));

	shared_data = (void *)(uintptr_t)(handle->regs_base);
	val = 1 << wd_val;

	shared_data->watchdog = val;
}

void al_uc_shared_current_sensor_store(const struct al_uc_shared_handle *handle,
				       const struct al_current_sensor_data *current_channels,
				       const int current_channels_size)
{
	struct al_general_shared_data *shared_data;
	int i;

	al_assert(handle);
	al_assert(current_channels);
	al_assert(current_channels_size <= CURRENT_SENSOR_MAX_CHANNELS);

	shared_data = (void *)(uintptr_t)(handle->regs_base);

	for (i = 0; i < current_channels_size; i++)
		shared_data->current_sensor_ch[i] = current_channels[i];
}

void al_uc_shared_temp_sensor_store(const struct al_uc_shared_handle *handle,
				    const unsigned int temperature)
{
	struct al_general_shared_data *shared_data;

	al_assert(handle);
	al_assert(temperature);

	shared_data = (void *)(uintptr_t)(handle->regs_base);

	shared_data->temp_sensor = temperature;

}
