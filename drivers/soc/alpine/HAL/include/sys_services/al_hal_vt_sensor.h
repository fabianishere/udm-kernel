/*
 * Copyright 2016, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

/**
 * @defgroup group_vt_sensor vt Sensor
 * @ingroup group_sys_services
 *  @{
 *      @file  al_hal_vt_sensor.h
 *      @brief HAL Driver Header for the Amazon thermal and voltage sensor unit
 *
 */

#ifndef _AL_HAL_VT_SENSE_H_
#define _AL_HAL_VT_SENSE_H_

#include "al_hal_sbus.h"

/******************************* Constants ***********************************/
/** Value to be used for disabling the low temperature threshold */
#define AL_VT_SENSOR_THERMAL_LOW_THRESHOLD_DISABLE	-1000
/** Value to be used for disabling the high temperature threshold */
#define AL_VT_SENSOR_THERMAL_HIGH_THRESHOLD_DISABLE	1000


/******************************* Enums ***********************************/
/**
 * al_vt_sensor_type: enum for indicate which sensor is sampling. *
 * Sensor_type initialization should be at the handle_init parameters before any
 * other APIs. Measurement unit for temperature - Celsius, for Voltage - mV.*/
enum al_vt_sensor_type {
	/** Main thermal sensor - Default sensor (Celsius). */
	al_vt_sensor_type_thermal,
	/** Optional thermal sensor in remote area of interest on the die (Celsius). */
	al_vt_sensor_type_thermal_remote_0,
	/** Optional thermal sensor in remote area of interest on the die (Celsius). */
	al_vt_sensor_type_thermal_remote_1,
	/** Optional thermal sensor in remote area of interest on the die (Celsius). */
	al_vt_sensor_type_thermal_remote_2,
	/** Voltage sensor for CPU VDD (mV). */
	al_vt_sensor_type_cpu_vdd,
	/** Voltage sensor for CORE VDD (mV). */
	al_vt_sensor_type_core_vdd,
	/** Voltage sensor for AVDD (mV). */
	al_vt_sensor_type_avdd
};

/*********************** Data Structures and Types ***************************/

/** Handle initialization parameters */
struct al_vt_sensor_handle_init_params {
	/** PBS register base */
	void __iomem			*pbs_regs_base;
	/** Thermal sensor register base - relevant to Alpine V1/V2 only */
	void __iomem			*thermal_sensor_reg_base;
	/** SBUS initialized handle - relevant to Alpine V3 only */
	const struct al_sbus_obj	*sbus_obj;
	/** Thermal sensor SBUS slave address - relevant to Alpine V3 only */
	unsigned int			sbus_slave_addr;
	/** Indication whether sensor reading was valid in the past -
	 *										relevant to Alpine V1/V2 only */
	int				ever_valid;
	/** Indication whether voltage or thermal sensor is sampling -
	 *										relevant to Alpine V3 only */
	enum al_vt_sensor_type		sensor_type;
	/** uC shared handle - relevant to Alpine V3 only in case there is an uC in system */
	struct al_uc_shared_handle	*uc_shared_handle;
};


/**
 * al_vt_sensor_handle: data structure used by the HAL to handle voltage and
 * temperature sensor functionality. this structure is allocated and set to zeros
 * by the * upper layer, then it is initialized by the al_vt_sensor_handle_init()
 * that should be called before any other function of this API. later, this
 * handle passed to the API functions.
 */
struct al_vt_sensor_handle {
	struct al_vt_sensor_thermal_regs __iomem	*regs;
	const struct al_sbus_obj		*sbus_obj;
	unsigned int				sbus_slave_addr;
	uint32_t				readout_raw;
	uint32_t				offset;
	uint32_t				multiplier;
	unsigned int			dev_id;
	unsigned int			rev_id;
	int ever_valid;
	enum al_vt_sensor_type		sensor_type;
	struct al_uc_shared_handle	*uc_shared_handle;
};

/************************************* API ***********************************/


/**
 * @brief Initialize a sensor handle structure - struct based parametrized function
 *
 * @param	sensor_handle
 *		an allocated, non-initialized instance.
 * @param	params
 *		initialization parameters
 */
void al_vt_sensor_handle_init(
		struct al_vt_sensor_handle				*sensor_handle,
		const struct al_vt_sensor_handle_init_params	*params);


/**
 * @brief Set SW digital trimming
 *	relevant just for thermal sensor at V1+V2 Alpine
 *
 * @param sensor_handle
 *		vt_sensor handle
 * @param trim
 *		the required auxiliary trimming (process dependent, usually
 *		obtained from eFuse, default value is 8)
 */
void al_vt_sensor_thermal_trim_set(
	struct al_vt_sensor_handle *sensor_handle,
	unsigned int			trim);

/**
 * @brief Set low and high temperature thresholds for interrupt generation
 *	relevant just for thermal sensor at V1+V2 Alpine
 *
 * @param sensor_handle
 *		vt_sensor handle
 * @param low_temp_threshold
 *		low temperature threshold in celsius degrees
 *		a value of AL_VT_SENSOR_THERMAL_LOW_THRESHOLD_DISABLE disables
 *		the threshold
 * @param high_temp_threshold
 *		high temperature threshold in celsius degrees
 *		a value of AL_VT_SENSOR_THERMAL_HIGH_THRESHOLD_DISABLE disables
 *		the threshold
 */
void al_vt_sensor_thermal_threshold_config(
	struct al_vt_sensor_handle *sensor_handle,
	int				low_temp_threshold,
	int				high_temp_threshold);


/**
 * @brief Get low and high temperature thresholds for interrupt generation
 *	relevant just for thermal sensor at V1+V2 Alpine
 *
 * @param sensor_handle
 *		vt_sensor handle
 * @param low_temp_threshold
 *		pointer for low temperature threshold in celsius degrees
 *		a value of AL_VT_SENSOR_THERMAL_LOW_THRESHOLD_DISABLE means
 *		the threshold is disabled. can be NULL.
 * @param high_temp_threshold
 *		pointer for high temperature threshold in celsius degrees
 *		a value of AL_VT_SENSOR_THERMAL_HIGH_THRESHOLD_DISABLE means
 *		the threshold is disabled. can be NULL.
 */
void al_vt_sensor_thermal_threshold_get(
	struct al_vt_sensor_handle *sensor_handle,
	int				*low_temp_threshold,
	int				*high_temp_threshold);


/**
 * @brief Power up/down and enable/disable the sensor unit
 *
 * @param sensor_handle
 *		vt_sensor handle
 * @param enable
 *		Whether to power-up and enable the sensor or to disable
 *		it and power it down
 */
void al_vt_sensor_enable_set(
	struct al_vt_sensor_handle	*sensor_handle,
	int				enable);


/**
 * @brief Determine if the sensor is ready
 *
 * @param sensor_handle
 *		vt_sensor handle
 *
 * @return non zero if the any sensor is ready
 */
int al_vt_sensor_is_ready(
	struct al_vt_sensor_handle	*sensor_handle);

/**
 * @brief Trigger single sample
 *
 * @param sensor_handle
 *		vt_sensor handle
 */
void al_vt_sensor_trigger_once(
	struct al_vt_sensor_handle	*sensor_handle);

/**
 * @brief Trigger continuous sampling
 *
 * @param sensor_handle
 *		vt_sensor handle
 */
void al_vt_sensor_trigger_continuous(
	struct al_vt_sensor_handle	*sensor_handle);

/**
 * @brief Determine if the sensor readout is valid
 *
 * @param sensor_handle
 *		vt_sensor handle
 *
 * @return non zero if the sensor readout is valid
 */
int al_vt_sensor_readout_is_valid(
	struct al_vt_sensor_handle	*sensor_handle);

/**
 * @brief Get the sensor last valid readout
 *
 * @param sensor_handle
 *		 vt_sensor handle
 *
 * @return current sensor readout
 *		[thermal sensor return degrees celcius]
 *		[voltage sensor return mV]
 */
int al_vt_sensor_readout_get(
		struct al_vt_sensor_handle	*sensor_handle);


#endif
/** @} end of groupthermal_sensor group */

