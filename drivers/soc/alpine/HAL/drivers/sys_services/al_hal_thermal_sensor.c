/*******************************************************************************
Copyright (C) 2015 Annapurna Labs Ltd.

This file may be licensed under the terms of the Annapurna Labs Commercial
License Agreement.

Alternatively, this file can be distributed under the terms of the GNU General
Public License V2 as published by the Free Software Foundation and can be
found at http://www.gnu.org/licenses/gpl-2.0.html

Alternatively, redistribution and use in source and binary forms, with or
without modification, are permitted provided that the following conditions are
met:

    *     Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

    *     Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in
the documentation and/or other materials provided with the
distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#include "al_hal_thermal_sensor.h"
#include "al_hal_pbs_utils.h"

/******************************************************************************
 ******************************************************************************/
int al_thermal_sensor_handle_init(
	struct al_thermal_sensor_handle	*thermal_sensor_handle,
	void __iomem		*thermal_sensor_reg_base,
	void __iomem		*pbs_regs_base)
{
	struct al_vt_sensor_handle_init_params vt_sensor_init = {
				.pbs_regs_base = pbs_regs_base,
				.thermal_sensor_reg_base = thermal_sensor_reg_base,
				.sbus_obj = NULL,
				.sbus_slave_addr = 0,
				.sensor_type = al_vt_sensor_type_thermal
		};

	al_assert(thermal_sensor_handle);
	al_assert(thermal_sensor_reg_base);
	al_assert(pbs_regs_base);

	al_vt_sensor_handle_init(
			&thermal_sensor_handle->vt_sensor_handle,
			&vt_sensor_init);

	return 0;
}

/******************************************************************************
 ******************************************************************************/
void al_thermal_sensor_trim_set(
	struct al_thermal_sensor_handle *thermal_sensor_handle,
	unsigned int			trim)
{
	al_assert(thermal_sensor_handle);

	al_vt_sensor_thermal_trim_set(&thermal_sensor_handle->vt_sensor_handle, trim);
}

/******************************************************************************
 ******************************************************************************/
void al_thermal_sensor_threshold_config(
	struct al_thermal_sensor_handle *thermal_sensor_handle,
	int				low_temp_threshold,
	int				high_temp_threshold)
{
	al_assert(thermal_sensor_handle);

	al_vt_sensor_thermal_threshold_config(
			&thermal_sensor_handle->vt_sensor_handle,
			low_temp_threshold,
			high_temp_threshold);
}

/******************************************************************************
 ******************************************************************************/
void al_thermal_sensor_threshold_get(
	struct al_thermal_sensor_handle *thermal_sensor_handle,
	int				*low_temp_threshold,
	int				*high_temp_threshold)
{
	al_assert(thermal_sensor_handle);

	al_vt_sensor_thermal_threshold_get(
			&thermal_sensor_handle->vt_sensor_handle,
			low_temp_threshold,
			high_temp_threshold);
}

/******************************************************************************
 ******************************************************************************/
void al_thermal_sensor_handle_init_ex(
	struct al_thermal_sensor_handle				*thermal_sensor_handle,
	const struct al_thermal_sensor_handle_init_params	*params)
{
	struct al_vt_sensor_handle_init_params vt_sensor_init = {
				.sensor_type = al_vt_sensor_type_thermal
		};

	al_assert(thermal_sensor_handle);
	al_assert(params);

	vt_sensor_init.pbs_regs_base = params->pbs_regs_base;
	vt_sensor_init.thermal_sensor_reg_base = params->thermal_sensor_reg_base;
	vt_sensor_init.sbus_obj = params->sbus_obj;
	vt_sensor_init.sbus_slave_addr = params->sbus_slave_addr;
	vt_sensor_init.uc_shared_handle = params->uc_shared_handle;

	al_vt_sensor_handle_init(
			&thermal_sensor_handle->vt_sensor_handle,
			&vt_sensor_init);
}

/******************************************************************************
 ******************************************************************************/
void al_thermal_sensor_enable_set(
	struct al_thermal_sensor_handle	*thermal_sensor_handle,
	int				enable)
{
	al_assert(thermal_sensor_handle);

	al_vt_sensor_enable_set(
			&thermal_sensor_handle->vt_sensor_handle,
			enable);
}

/******************************************************************************
 ******************************************************************************/
int al_thermal_sensor_is_ready(
	struct al_thermal_sensor_handle	*thermal_sensor_handle)
{
	al_assert(thermal_sensor_handle);

	return al_vt_sensor_is_ready(&thermal_sensor_handle->vt_sensor_handle);
}

/******************************************************************************
 ******************************************************************************/
void al_thermal_sensor_trigger_once(
	struct al_thermal_sensor_handle	*thermal_sensor_handle)
{
	al_assert(thermal_sensor_handle);

	al_vt_sensor_trigger_once(&thermal_sensor_handle->vt_sensor_handle);
}

/******************************************************************************
 ******************************************************************************/
void al_thermal_sensor_trigger_continuous(
	struct al_thermal_sensor_handle	*thermal_sensor_handle)
{
	al_assert(thermal_sensor_handle);

	al_vt_sensor_trigger_continuous(&thermal_sensor_handle->vt_sensor_handle);
}

/******************************************************************************
 ******************************************************************************/
int al_thermal_sensor_readout_is_valid(
	struct al_thermal_sensor_handle	*thermal_sensor_handle)
{
	al_assert(thermal_sensor_handle);

	return al_vt_sensor_readout_is_valid(&thermal_sensor_handle->vt_sensor_handle);
}

/******************************************************************************
 ******************************************************************************/
int al_thermal_sensor_readout_get(
	struct al_thermal_sensor_handle	*thermal_sensor_handle)
{
	al_assert(thermal_sensor_handle);

	return al_vt_sensor_readout_get(&thermal_sensor_handle->vt_sensor_handle);
}

