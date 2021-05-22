/*
 * Copyright 2016, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#include "al_hal_vt_sensor.h"
#include "al_hal_vt_sensor_thermal_regs.h"
#include "al_hal_reg_utils.h"
#include "al_hal_pbs_utils.h"
#include "al_uc_shared.h"


/* Alpine V1/V2 definitions */
#define OFFSET_V1		1154	/* 115.4 x 10 - for better accuracy */
#define MULTIPLIER_V1	3523	/* 352.3 x 10 - for better accuracy */

#define OFFSET_V2		1090	/* 109.0 x 10 */
#define MULTIPLIER_V2	3520	/* 352.0 x 10 */

#define READOUT_TO_CELCIUS(readout, offset, multiplier)		\
		((((((int)(readout)) * ((int)(multiplier))) / 4096) - ((int)(offset))) / 10)

#define CELCIUS_TO_READOUT(celcius, offset, multiplier)		\
		((((10 * (celcius)) + (offset)) * 4096) / (multiplier))

#define AL_THERMAL_SENSOR_MIN_THRESHOLD_VAL	0
#define AL_THERMAL_SENSOR_MAX_THRESHOLD_VAL	0xfff

/* Alpine V3 definitions */
#define SENSOR_CLK_DIVIDE_REG_NUM			1
#define MODE_REG_NUM					3
#define MODE_VAL_TEMP					AL_BIT(0)
#define MODE_VAL_TEMP_RMT_0				AL_BIT(1)
#define MODE_VAL_TEMP_RMT_1				AL_BIT(2)
#define MODE_VAL_TEMP_RMT_2				AL_BIT(3)
#define MODE_VAL_CPU_VDD_REV_0				AL_BIT(9)
#define MODE_VAL_CPU_VDD_REV_1				AL_BIT(13)
#define MODE_VAL_CPU_VDD(rev_id)			\
	(((rev_id) == 0) ? MODE_VAL_CPU_VDD_REV_0 : MODE_VAL_CPU_VDD_REV_1)
#define MODE_VAL_CORE_VDD				AL_BIT(10)
#define MODE_VAL_AVDD					AL_BIT(11)
#define CTRL_REG_NUM					0
#define CTRL_RESET					AL_BIT(0)
#define CTRL_START					AL_BIT(1)

#define VT_SENSOR_TEMPERATURE_REG_NUM			65
#define VT_SENSOR_TEMPERATURE_RMT_0_REG_NUM		66
#define VT_SENSOR_TEMPERATURE_RMT_1_REG_NUM		67
#define VT_SENSOR_TEMPERATURE_RMT_2_REG_NUM		68
#define VT_SENSOR_CPU_VDD_REG_NUM_REV_0			74
#define VT_SENSOR_CPU_VDD_REG_NUM_REV_1			78
#define VT_SENSOR_CORE_VDD_REG_NUM			75
#define VT_SENSOR_AVDD_REG_NUM				76
#define VT_SENSOR_CPU_VDD_REG_NUM(rev_id) (rev_id == 0 ? VT_SENSOR_CPU_VDD_REG_NUM_REV_0 : \
							VT_SENSOR_CPU_VDD_REG_NUM_REV_1)
#define VT_SENSOR_VALUE_MASK				AL_FIELD_MASK(11, 0)
#define VT_SENSOR_VALUE_SHIFT				0
#define VT_SENSOR_VALID					AL_BIT(15)

#define REF_CLK_FREQ_MHZ				100
#define CLK_FREQ_MHZ					2
#define CLK_DIV						(REF_CLK_FREQ_MHZ / CLK_FREQ_MHZ)

#define RESET_NUM_CC					2

/******************************************************************************
 ******************************************************************************/
void al_vt_sensor_handle_init(
		struct al_vt_sensor_handle				*sensor_handle,
		const struct al_vt_sensor_handle_init_params	*params)
{
	unsigned int dev_id;
	unsigned int rev_id;

	al_assert(params);
	al_assert(params->pbs_regs_base);
	al_assert(sensor_handle);

	dev_id = al_pbs_dev_id_get(params->pbs_regs_base);
	rev_id = al_pbs_dev_rev_id_get(params->pbs_regs_base);
	sensor_handle->dev_id = dev_id;
	sensor_handle->rev_id = rev_id;

	if (params->uc_shared_handle != NULL) {
		if (al_uc_shared_vt_sensor_vld_get(params->uc_shared_handle))
			sensor_handle->uc_shared_handle = params->uc_shared_handle;
		else
			sensor_handle->uc_shared_handle = NULL;
	} else {
		sensor_handle->uc_shared_handle = NULL;
	}

	if (sensor_handle->dev_id <= PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V2) {
		al_assert(params->sensor_type == al_vt_sensor_type_thermal);
		al_assert(params->thermal_sensor_reg_base);
		sensor_handle->regs = (struct al_vt_sensor_thermal_regs __iomem *)
					params->thermal_sensor_reg_base;

		if (sensor_handle->dev_id == PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V1) {
			sensor_handle->offset = OFFSET_V1;
			sensor_handle->multiplier = MULTIPLIER_V1;
		} else if (sensor_handle->dev_id == PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V2) {
			sensor_handle->offset = OFFSET_V2;
			sensor_handle->multiplier = MULTIPLIER_V2;
		}
	} else {
		al_assert(params->sbus_obj);
		al_assert(rev_id || (params->sensor_type != al_vt_sensor_type_thermal_remote_2));
		al_assert(rev_id || (params->sensor_type != al_vt_sensor_type_avdd));

		sensor_handle->regs = NULL;
		sensor_handle->sbus_obj = params->sbus_obj;
		sensor_handle->sbus_slave_addr = params->sbus_slave_addr;
	}

	sensor_handle->sensor_type = params->sensor_type;
	sensor_handle->ever_valid = params->ever_valid;
}

/******************************************************************************
 ******************************************************************************/
void al_vt_sensor_thermal_trim_set(
		struct al_vt_sensor_handle *sensor_handle,
		unsigned int			trim)
{
	struct al_vt_sensor_thermal_regs __iomem *regs;

	al_assert(sensor_handle);
	al_assert(sensor_handle->sensor_type == al_vt_sensor_type_thermal);

	if (sensor_handle->dev_id > PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V2) {
		al_err("%s: only supported on Alpine V1/V2!\n", __func__);
		return;
	}

	regs = sensor_handle->regs;

	al_reg_write32_masked(
		&regs->unit.static_config,
		THERMAL_SENSE_UNIT_STATIC_CONFIG_TRIM_MASK,
		trim << THERMAL_SENSE_UNIT_STATIC_CONFIG_TRIM_SHIFT);
}

/******************************************************************************
 ******************************************************************************/
void al_vt_sensor_thermal_threshold_config(
		struct al_vt_sensor_handle *sensor_handle,
		int				low_temp_threshold,
		int				high_temp_threshold)
{
	struct al_vt_sensor_thermal_regs __iomem *regs;
	unsigned int low_threshold, high_threshold;

	al_assert(sensor_handle);
	al_assert(sensor_handle->sensor_type == al_vt_sensor_type_thermal);


	if (sensor_handle->dev_id > PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V2) {
		al_err("%s: only supported on Alpine V1/V2!\n", __func__);
		return;
	}

	low_threshold =
		(low_temp_threshold ==
		AL_VT_SENSOR_THERMAL_LOW_THRESHOLD_DISABLE) ?
		AL_THERMAL_SENSOR_MIN_THRESHOLD_VAL :
		(unsigned int)CELCIUS_TO_READOUT(low_temp_threshold,
			sensor_handle->offset,
			sensor_handle->multiplier);
	high_threshold =
		(high_temp_threshold ==
		AL_VT_SENSOR_THERMAL_HIGH_THRESHOLD_DISABLE) ?
		AL_THERMAL_SENSOR_MAX_THRESHOLD_VAL :
		(unsigned int)CELCIUS_TO_READOUT(high_temp_threshold,
			sensor_handle->offset,
			sensor_handle->multiplier);

	regs = sensor_handle->regs;

	al_reg_write32(
		&regs->unit.threshold_config,
		(low_threshold <<
		THERMAL_SENSE_UNIT_THRESHOLD_CONFIG_LOW_TEMP_THRESHOLD_SHIFT) |
		(high_threshold <<
		THERMAL_SENSE_UNIT_THRESHOLD_CONFIG_HIGH_TEMP_THRESHOLD_SHIFT));
}

/******************************************************************************
 ******************************************************************************/
void al_vt_sensor_thermal_threshold_get(
		struct al_vt_sensor_handle *sensor_handle,
		int				*low_temp_threshold,
		int				*high_temp_threshold)
{
	struct al_vt_sensor_thermal_regs __iomem *regs;
	uint32_t threshold_config;
	unsigned int low_threshold;
	unsigned int high_threshold;
	int temp_threshold;

	al_assert(sensor_handle);
	al_assert(sensor_handle->sensor_type == al_vt_sensor_type_thermal);

	if (sensor_handle->dev_id > PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V2) {
		al_err("%s: only supported on Alpine V1/V2!\n", __func__);
		return;
	}

	regs = sensor_handle->regs;
	threshold_config = al_reg_read32(&regs->unit.threshold_config);

	low_threshold = AL_REG_FIELD_GET(threshold_config,
		THERMAL_SENSE_UNIT_THRESHOLD_CONFIG_LOW_TEMP_THRESHOLD_MASK,
		THERMAL_SENSE_UNIT_THRESHOLD_CONFIG_LOW_TEMP_THRESHOLD_SHIFT);
	if (low_threshold == AL_THERMAL_SENSOR_MIN_THRESHOLD_VAL)
		temp_threshold = AL_VT_SENSOR_THERMAL_LOW_THRESHOLD_DISABLE;
	else
		temp_threshold = READOUT_TO_CELCIUS(low_threshold,
			sensor_handle->offset,
			sensor_handle->multiplier);

	if (low_temp_threshold != NULL)
		*low_temp_threshold = temp_threshold;

	high_threshold = AL_REG_FIELD_GET(threshold_config,
		THERMAL_SENSE_UNIT_THRESHOLD_CONFIG_HIGH_TEMP_THRESHOLD_MASK,
		THERMAL_SENSE_UNIT_THRESHOLD_CONFIG_HIGH_TEMP_THRESHOLD_SHIFT);
	if (high_threshold == AL_THERMAL_SENSOR_MAX_THRESHOLD_VAL)
		temp_threshold = AL_VT_SENSOR_THERMAL_HIGH_THRESHOLD_DISABLE;
	else
		temp_threshold = READOUT_TO_CELCIUS(high_threshold,
			sensor_handle->offset,
			sensor_handle->multiplier);

	if (high_temp_threshold != NULL)
		*high_temp_threshold = temp_threshold;
}

/******************************************************************************
 ******************************************************************************/
static void al_vt_sensor_thermal_cc_delay_v3(
		unsigned int			cc)
{
	al_udelay((cc + CLK_FREQ_MHZ - 1) / CLK_FREQ_MHZ);
}

/******************************************************************************
 ******************************************************************************/
static void al_vt_sensor_thermal_enable_set_v2(
		struct al_vt_sensor_handle	*sensor_handle,
		int				enable)
{
	struct al_vt_sensor_thermal_regs __iomem *regs = sensor_handle->regs;

	if (!enable)
		al_reg_write32(&regs->unit.dynamic_config, 0);

	al_reg_write32_masked(
		&regs->unit.static_config,
		THERMAL_SENSE_UNIT_STATIC_CONFIG_POWER_DOWN |
		THERMAL_SENSE_UNIT_STATIC_CONFIG_ENABLE,
		enable ?
			THERMAL_SENSE_UNIT_STATIC_CONFIG_ENABLE :
			THERMAL_SENSE_UNIT_STATIC_CONFIG_POWER_DOWN);
}

/******************************************************************************
 ******************************************************************************/
static void al_vt_sensor_enable_set_v3(
		struct al_vt_sensor_handle	*sensor_handle,
		int				enable)
{
	const struct al_sbus_obj *sbus = sensor_handle->sbus_obj;
	unsigned int sbus_slave_addr = sensor_handle->sbus_slave_addr;
	unsigned int mode_val_main;

	if (enable) {
		/* Deassert the SBus controller reset */
		al_sbus_master_reset_deassert(sbus);
		/* Configure the sensor clock divider to produce 2.0MHz */
		al_sbus_write(
			sbus,
			sbus_slave_addr,
			SENSOR_CLK_DIVIDE_REG_NUM,
			CLK_DIV,
			AL_TRUE);
		/* Configure the mode to main temperature reading */
		switch (sensor_handle->sensor_type) {
		case al_vt_sensor_type_thermal:
			mode_val_main = MODE_VAL_TEMP;
			break;
		case al_vt_sensor_type_thermal_remote_0:
			mode_val_main = MODE_VAL_TEMP_RMT_0;
			break;
		case al_vt_sensor_type_thermal_remote_1:
			mode_val_main = MODE_VAL_TEMP_RMT_1;
			break;
		case al_vt_sensor_type_thermal_remote_2:
			mode_val_main = MODE_VAL_TEMP_RMT_2;
			break;
		case al_vt_sensor_type_cpu_vdd:
			mode_val_main = MODE_VAL_CPU_VDD(sensor_handle->rev_id);
			break;
		case al_vt_sensor_type_core_vdd:
			mode_val_main = MODE_VAL_CORE_VDD;
			break;
		case al_vt_sensor_type_avdd:
			mode_val_main = MODE_VAL_AVDD;
			break;
		default:
			al_err("%s: support voltage or thermal sensor only!\n", __func__);
			al_assert(0);
			return;
		}
		al_sbus_write(
				sbus,
				sbus_slave_addr,
				MODE_REG_NUM,
				mode_val_main,
				AL_TRUE);

		/**
		 * Assert the reset bit for a minimum of  two INTERNAL_SENSOR_CLK cycles clears all
		 * sensor values and sensor valid bits.
		 */
		al_sbus_write(sbus, sbus_slave_addr, CTRL_REG_NUM, CTRL_RESET, AL_TRUE);
		al_vt_sensor_thermal_cc_delay_v3(RESET_NUM_CC);
		al_sbus_write(sbus, sbus_slave_addr, CTRL_REG_NUM, 0, AL_TRUE);
	} else {
		al_sbus_write(sbus, sbus_slave_addr, CTRL_REG_NUM, CTRL_RESET, AL_TRUE);
	}
}


/******************************************************************************
 ******************************************************************************/
void al_vt_sensor_enable_set(
		struct al_vt_sensor_handle	*sensor_handle,
		int				enable)
{
	al_assert(sensor_handle);

	if (sensor_handle->uc_shared_handle != NULL) {
		if (al_uc_shared_vt_sensor_vld_get(sensor_handle->uc_shared_handle))
			return;
	}

	if (sensor_handle->dev_id <= PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V2) {
		al_assert(sensor_handle->sensor_type == al_vt_sensor_type_thermal);
		al_vt_sensor_thermal_enable_set_v2(sensor_handle, enable);
	} else {
		al_vt_sensor_enable_set_v3(sensor_handle, enable);
	}
}

/******************************************************************************
 ******************************************************************************/
static int al_vt_sensor_is_ready_v3(
		struct al_vt_sensor_handle	*sensor_handle __attribute__((unused)))
{
	return AL_TRUE;
}

/******************************************************************************
 ******************************************************************************/
static int al_vt_sensor_thermal_is_ready_v2(
		struct al_vt_sensor_handle	*sensor_handle)
{
	struct al_vt_sensor_thermal_regs __iomem *regs = sensor_handle->regs;
	uint32_t status_reg_val;
	int is_valid;

	status_reg_val = al_reg_read32(
			&regs->unit.status);

	is_valid = ((status_reg_val & THERMAL_SENSE_UNIT_STATUS_T_PWR_OK) &&
		(status_reg_val & THERMAL_SENSE_UNIT_STATUS_T_INIT_DONE));

	return is_valid;
}

/******************************************************************************
 ******************************************************************************/
int al_vt_sensor_is_ready(
		struct al_vt_sensor_handle	*sensor_handle)
{
	al_assert(sensor_handle);

	if (sensor_handle->uc_shared_handle != NULL) {
		if (al_uc_shared_vt_sensor_vld_get(sensor_handle->uc_shared_handle))
			return 1;
	}

	if (sensor_handle->dev_id <= PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V2) {
		al_assert(sensor_handle->sensor_type == al_vt_sensor_type_thermal);
		return al_vt_sensor_thermal_is_ready_v2(sensor_handle);
	} else {
		return al_vt_sensor_is_ready_v3(sensor_handle);
	}

	return 0;
}

/******************************************************************************
 ******************************************************************************/
static int al_vt_sensor_thermal_readout_is_valid_v2(
		struct al_vt_sensor_handle	*sensor_handle)
{
	struct al_vt_sensor_thermal_regs __iomem *regs = sensor_handle->regs;
	uint32_t status_reg_val;
	int is_valid;

	status_reg_val = al_reg_read32(
		&regs->unit.status);

	is_valid = ((status_reg_val & THERMAL_SENSE_UNIT_STATUS_T_PWR_OK) &&
		(status_reg_val & THERMAL_SENSE_UNIT_STATUS_T_INIT_DONE) &&
		(status_reg_val & THERMAL_SENSE_UNIT_STATUS_T_VALID));

	if (is_valid)
		sensor_handle->ever_valid = is_valid;

	if (sensor_handle->ever_valid) {
		sensor_handle->readout_raw = (status_reg_val &
		THERMAL_SENSE_UNIT_STATUS_T_RESULT_MASK) >>
		THERMAL_SENSE_UNIT_STATUS_T_RESULT_SHIFT;
	}

	return sensor_handle->ever_valid;
}

/******************************************************************************
 ******************************************************************************/
static int al_vt_sensor_readout_is_valid_v3(
		struct al_vt_sensor_handle	*sensor_handle)
{
	const struct al_sbus_obj *sbus = sensor_handle->sbus_obj;
	unsigned int sbus_slave_addr = sensor_handle->sbus_slave_addr;
	unsigned int reg_val;
	unsigned int reg_num;

	int is_valid;
	int err;

	switch (sensor_handle->sensor_type) {
	case al_vt_sensor_type_thermal:
		reg_num = VT_SENSOR_TEMPERATURE_REG_NUM;
		break;
	case al_vt_sensor_type_thermal_remote_0:
		reg_num = VT_SENSOR_TEMPERATURE_RMT_0_REG_NUM;
		break;
	case al_vt_sensor_type_thermal_remote_1:
		reg_num = VT_SENSOR_TEMPERATURE_RMT_1_REG_NUM;
		break;
	case al_vt_sensor_type_thermal_remote_2:
		reg_num = VT_SENSOR_TEMPERATURE_RMT_2_REG_NUM;
		break;
	case al_vt_sensor_type_cpu_vdd:
		reg_num = VT_SENSOR_CPU_VDD_REG_NUM(sensor_handle->rev_id);
		break;
	case al_vt_sensor_type_core_vdd:
		reg_num = VT_SENSOR_CORE_VDD_REG_NUM;
		break;
	case al_vt_sensor_type_avdd:
		reg_num = VT_SENSOR_AVDD_REG_NUM;
		break;
	default:
		al_err("%s: support voltage or thermal sensor only!\n", __func__);
		al_assert(0);
		return -EINVAL;
	}

	err = al_sbus_read(sbus, sbus_slave_addr, reg_num, &reg_val);
	if (err)
		return 0;

	is_valid = !!(reg_val & VT_SENSOR_VALID);

	if (is_valid)
		sensor_handle->readout_raw =
				(reg_val & VT_SENSOR_VALUE_MASK) >> VT_SENSOR_VALUE_SHIFT;

	return is_valid;
}

/******************************************************************************
 ******************************************************************************/
int al_vt_sensor_readout_is_valid(struct al_vt_sensor_handle	*sensor_handle)
{
	al_assert(sensor_handle);

	if (sensor_handle->uc_shared_handle != NULL) {
		if (al_uc_shared_vt_sensor_vld_get(sensor_handle->uc_shared_handle))
			return 1;
	}

	if (sensor_handle->dev_id <= PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V2) {
		al_assert(sensor_handle->sensor_type == al_vt_sensor_type_thermal);
		return al_vt_sensor_thermal_readout_is_valid_v2(sensor_handle);
	} else {
		return al_vt_sensor_readout_is_valid_v3(sensor_handle);
	}

	return 0;
}



/******************************************************************************
 ******************************************************************************/
static void al_vt_sensor_thermal_trigger_once_v2(
		struct al_vt_sensor_handle	*sensor_handle)
{
	struct al_vt_sensor_thermal_regs __iomem *regs = sensor_handle->regs;

	al_reg_write32(&regs->unit.dynamic_config,
		THERMAL_SENSE_UNIT_DYNAMIC_CONFIG_RUN_ONCE);
}

/******************************************************************************
 ******************************************************************************/
static void al_vt_sensor_trigger_v3(
		struct al_vt_sensor_handle	*sensor_handle)
{
	const struct al_sbus_obj *sbus = sensor_handle->sbus_obj;
	unsigned int sbus_slave_addr = sensor_handle->sbus_slave_addr;

	al_sbus_write(sbus, sbus_slave_addr, CTRL_REG_NUM, CTRL_START, AL_TRUE);
}

/******************************************************************************
 ******************************************************************************/
void al_vt_sensor_trigger_once(
		struct al_vt_sensor_handle	*sensor_handle)
{
	al_assert(sensor_handle);

	if (sensor_handle->uc_shared_handle != NULL) {
		if (al_uc_shared_vt_sensor_vld_get(sensor_handle->uc_shared_handle))
			return;
	}

	if (sensor_handle->dev_id <= PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V2) {
		al_assert(sensor_handle->sensor_type == al_vt_sensor_type_thermal);
		al_vt_sensor_thermal_trigger_once_v2(sensor_handle);
	} else {
		al_vt_sensor_trigger_v3(sensor_handle);
	}
}

/******************************************************************************
 ******************************************************************************/
static void al_vt_sensor_thermal_trigger_continuous_v2(
		struct al_vt_sensor_handle	*sensor_handle)
{
	struct al_vt_sensor_thermal_regs __iomem *regs = sensor_handle->regs;

	al_reg_write32(&regs->unit.dynamic_config,
		THERMAL_SENSE_UNIT_DYNAMIC_CONFIG_KEEP_RUNNING);
}

/******************************************************************************
 ******************************************************************************/
void al_vt_sensor_trigger_continuous(
		struct al_vt_sensor_handle	*sensor_handle)
{
	al_assert(sensor_handle);

	if (sensor_handle->uc_shared_handle != NULL) {
		if (al_uc_shared_vt_sensor_vld_get(sensor_handle->uc_shared_handle))
			return;
	}

	if (sensor_handle->dev_id <= PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V2) {
		al_assert(sensor_handle->sensor_type == al_vt_sensor_type_thermal);
		al_vt_sensor_thermal_trigger_continuous_v2(sensor_handle);
	} else {
		al_vt_sensor_trigger_v3(sensor_handle);
	}
}


/******************************************************************************
 ******************************************************************************/
static int al_vt_readout_to_celcius_v3(int readout)
{
	const unsigned int orig_size = 9;
	const unsigned int new_size = 8 * sizeof(int);
	const unsigned int shift_amount = new_size - orig_size;

	/* Get rid of fractions */
	readout >>= 3;
	/* Shift left so MSB will be the sign bit */
	readout <<= shift_amount;
	/* Right shift to extend the sign */
	readout >>= shift_amount;

	return readout;
}

/******************************************************************************
 ******************************************************************************/
static int al_vt_readout_to_voltage(int readout)
{
	/* Get rid of fractions */
	readout >>= 1;

	return readout;
}


/******************************************************************************
 ******************************************************************************/
static int al_vt_sensor_thermal_readout_get(
		struct al_vt_sensor_handle	*sensor_handle)
{
	int readout;

	if (sensor_handle->dev_id <= PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V2)
		readout = READOUT_TO_CELCIUS(sensor_handle->readout_raw,
			sensor_handle->offset,
			sensor_handle->multiplier);
	else
		readout = al_vt_readout_to_celcius_v3(sensor_handle->readout_raw);

	return readout;
}

/******************************************************************************
 ******************************************************************************/
static int al_vt_sensor_voltage_readout_get(
		struct al_vt_sensor_handle	*voltage_sensor_handle)
{
	int readout;

	readout = al_vt_readout_to_voltage(voltage_sensor_handle->readout_raw);

	return readout;
}

/******************************************************************************
 ******************************************************************************/
int al_vt_sensor_readout_get(
		struct al_vt_sensor_handle	*sensor_handle)
{
	al_assert(sensor_handle);

	if (sensor_handle->uc_shared_handle != NULL) {
		if (al_uc_shared_vt_sensor_vld_get(sensor_handle->uc_shared_handle))
			return al_uc_shared_vt_sensor_data_get(sensor_handle->uc_shared_handle,
				sensor_handle->sensor_type);
	}

	switch (sensor_handle->sensor_type) {
	case al_vt_sensor_type_thermal:
	case al_vt_sensor_type_thermal_remote_0:
	case al_vt_sensor_type_thermal_remote_1:
	case al_vt_sensor_type_thermal_remote_2:
		return al_vt_sensor_thermal_readout_get(sensor_handle);
	case al_vt_sensor_type_cpu_vdd:
	case al_vt_sensor_type_core_vdd:
	case al_vt_sensor_type_avdd:
		al_assert(sensor_handle->dev_id > PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V2);
		return al_vt_sensor_voltage_readout_get(sensor_handle);
	default:
		al_err("%s: support voltage or thermal sensor only!\n", __func__);
		al_assert(0);
		return -EINVAL;
	}
}

