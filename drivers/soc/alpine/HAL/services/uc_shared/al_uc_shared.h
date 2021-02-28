/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#ifndef __AL_UC_SHARED_H__
#define __AL_UC_SHARED_H__

#include "al_hal_common.h"
#include "al_hal_i2c.h"
#include "al_err_events.h"
#include "al_general_shared_data.h"
#include "al_hal_vt_sensor.h"

/** Watchdog values */
enum al_uc_shared_watchdog_values {
	AL_UC_SHARED_TIMER_EXPIRED, /**< Watchdog timer expired */
	AL_UC_SHARED_HARD_RESET, /**< Watchdog hard reset */
	AL_UC_SHARED_POWER_DOWN, /**< Watchdog power down */
};

struct al_uc_shared_handle {
	/** address for shared memory */
	void __iomem *regs_base;
};

struct al_uc_shared_handle_init_params {
	/** Address for shared memory */
	void __iomem *regs_base;
};

/**
 * Initialize uc shared handle
 * @param handle uc_shared handle
 * @param init initialization paramaters
 *
 * @return none
 */
void al_uc_shared_handle_init(struct al_uc_shared_handle *handle,
			      const struct al_uc_shared_handle_init_params *init);

/**
 * Initialize uc shared data
 * @param handle uc_shared handle
 *
 * @return none
 */
void al_uc_shared_data_init(const struct al_uc_shared_handle *handle);

/**
 * Stores the err events in shared memory
 * @param handle uc_shared handle
 * @param field error event
 *
 * @return none
 */
void al_uc_shared_err_events_store(const struct al_uc_shared_handle *handle,
				   const struct al_err_events_field *field);

/**
 * Store watchdog error in shared memory
 * @param handle initialized object
 * @param wd_val watchdog value
 *
 * @return none
 */
void al_uc_shared_watchdog_store(const struct al_uc_shared_handle *handle,
				 enum al_uc_shared_watchdog_values wd_val);

/**
 * Store current and voltage in shared memory
 * @param handle uc_shared handle
 * @param current_channels current sensor channels buffer
 * @param current_channels_size number of current sensor channels
 *
 * @return none
 */
void al_uc_shared_current_sensor_store(const struct al_uc_shared_handle *handle,
				       const struct al_current_sensor_data *current_channels,
				       const int current_channels_size);

/**
 * Store temperature in shared memory
 * @param handle uc_shared handle
 * @param temperature temperature sensor data in celsius
 *
 * @return none
 */
void al_uc_shared_temp_sensor_store(const struct al_uc_shared_handle *handle,
				    const unsigned int temperature);

/**
 * Get the status of the alpine vt sensor that
 * is stored in shared memory, viable only for V3
 * If this function returns AL_TRUE this means that the data
 * that is written in the sram is valid and al_uc_shared_vt_sensor_data_get
 * can be used
 * @param handle uc_shared handle
 *
 * @return AL_TRUE if data is valid, AL_FALSE otherwise
 */
al_bool al_uc_shared_vt_sensor_vld_get(const struct al_uc_shared_handle *handle);

/**
 * Get alpine vt sensor data that is stored in shared memory, viable only for V3
 * This function must be called only if al_uc_shared_vt_sensor_vld_get
 * returns AL_TRUE
 * @param handle uc_shared handle
 * @param type of the alpine vt sensor(temp or volt)
 *
 * @return temperature of the desired sensor in celcious or
 * voltage of the desired sensor in milivolts
 */
int al_uc_shared_vt_sensor_data_get(const struct al_uc_shared_handle *handle,
				  enum al_vt_sensor_type type);

/**
 * Get the status of the fru that
 * is stored in shared memory, viable only for V3
 * If this function returns AL_TRUE this means that the data
 * that is written in the sram is valid and al_uc_shared_fru_data_get
 * can be used
 * @param handle uc_shared handle
 *
 * @return AL_TRUE if data is valid, AL_FALSE otherwise
 */
al_bool al_uc_shared_fru_vld_get(const struct al_uc_shared_handle *handle);

/**
 * This function returns FRU data structure, filled by uC on boot
 * viable for V3 forward
 * This function can only be used if al_uc_shared_fru_vld_get
 * returns AL_TRUE
 * @param handle uc_shared handle
 * @param fru_data pointer to container of type al_uc_shared_fru_data
 * @return none
 */
void al_uc_shared_fru_data_get(const struct al_uc_shared_handle *handle,
				struct al_uc_shared_fru_data *fru_data);

#endif
