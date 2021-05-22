/*
 * Annapurna Labs thermal driver.
 *
 * Copyright (C) 2013 Annapurna Labs
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 */

#include <linux/err.h>
#include <linux/of.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/thermal.h>
#include <linux/of_address.h>
#include "al_hal_vt_sensor.h"

#define THERMAL_UDELAY_TIME	10
#define THERMAL_DELAY_ITER	100
#define THERMAL_MAX_ITER	\
	((THERMAL_DELAY_ITER / THERMAL_UDELAY_TIME) * 100000)
#define THERMAL_V3_SENSOR_NUM	4

struct al_thermal_v3_sensor_data;
struct al_thermal_v3_data;

struct al_thermal_v3_sensor_data {
	struct al_vt_sensor_handle vt_handle;
	struct thermal_zone_device *tzd;
	unsigned int sensor_id;
	unsigned int sensor_type;
	struct al_thermal_v3_data *data;
};

struct al_thermal_v3_data {
	struct al_thermal_v3_sensor_data sensor_data[THERMAL_V3_SENSOR_NUM];
	struct al_sbus_obj sbus_obj;
	struct mutex lock;
	struct device *dev;
};

static void al_thermal_v3_sensor_enable(struct al_thermal_v3_sensor_data *sensor_data)
{
	struct al_vt_sensor_handle *handle = &sensor_data->vt_handle;

	al_vt_sensor_enable_set(handle, 1);
	al_vt_sensor_trigger_continuous(handle);
}

static void al_thermal_v3_sensor_reset(struct al_thermal_v3_sensor_data *sensor_data)
{
	struct al_vt_sensor_handle *handle = &sensor_data->vt_handle;

	al_vt_sensor_enable_set(handle, 0);
}

static int al_thermal_v3_sensor_get_temp(void *devdata, int *temp)
{
	int err;
	unsigned int i;
	struct al_thermal_v3_sensor_data *sensor_data = (struct al_thermal_v3_sensor_data *)devdata;
	struct al_thermal_v3_data *thermal_data = sensor_data->data;
	struct mutex *lock = &thermal_data->lock;
	struct al_vt_sensor_handle *handle = &sensor_data->vt_handle;

	mutex_lock(lock);

	al_thermal_v3_sensor_enable(sensor_data);

	for (i = 1; (i <= THERMAL_MAX_ITER) && !al_vt_sensor_readout_is_valid(handle); i++) {
		if (!(i % THERMAL_DELAY_ITER))
			udelay(THERMAL_UDELAY_TIME);
	}

	if (i > THERMAL_MAX_ITER) {
		err = -ETIME;
		dev_err(thermal_data->dev,
			"sensor[%d] temp is not valid\n",
			sensor_data->sensor_id);
		goto read_end;
	}

	*temp = 1000 * al_vt_sensor_readout_get(handle);

	err = 0;
read_end:
	al_thermal_v3_sensor_reset(sensor_data);
	mutex_unlock(lock);

	return err;
}

static struct thermal_zone_of_device_ops of_ops = {
	.get_temp = al_thermal_v3_sensor_get_temp,
};

static int al_thermal_v3_probe(struct platform_device *pdev)
{
	int i;
	int err;
	uint32_t slave_addr;
	void __iomem *thermal_base;
	void __iomem *pbs_reg_base;
	struct al_thermal_v3_data *thermal_data;
	struct al_vt_sensor_handle_init_params vt_params = {0};
	struct thermal_zone_device *tz;
	struct device_node *np = pdev->dev.of_node;
	struct resource *al_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);

	if (!np) {
		dev_err(&pdev->dev, "failed to find DT data\n");
		return -EINVAL;
	}

	if (!al_res) {
		dev_err(&pdev->dev, "memory resource missing\n");
		return -ENODEV;
	}

	np = of_find_compatible_node(NULL, NULL, "annapurna-labs,al-pbs");
	if (!np) {
		dev_err(&pdev->dev, "failed to find PBS data in DT\n");
		return -EINVAL;
	}

	pbs_reg_base = of_iomap(np, 0);
	if (!pbs_reg_base) {
		dev_err(&pdev->dev, "of_iomap of PBS address failed\n");
		return -EINVAL;
	}

	err = of_property_read_u32(pdev->dev.of_node, "slave_addr", &slave_addr);
	if (err) {
		dev_err(&pdev->dev, "failed to find slave address in DT\n");
		return -EINVAL;
	}

	thermal_data = devm_kzalloc(&pdev->dev, sizeof(*thermal_data), GFP_KERNEL);
	if (!thermal_data) {
		dev_err(&pdev->dev, "failed to allocate memory\n");
		return -ENOMEM;
	}

	thermal_base = devm_ioremap(&pdev->dev, al_res->start, resource_size(al_res));
	if (!thermal_base) {
		dev_err(&pdev->dev, "failed to ioremap memory\n");
		return -ENOMEM;
	}

	thermal_data->dev = &pdev->dev;

	mutex_init(&thermal_data->lock);

	al_sbus_handle_init(&thermal_data->sbus_obj, thermal_base);

	vt_params.pbs_regs_base = pbs_reg_base;
	vt_params.sbus_slave_addr = slave_addr;
	vt_params.sbus_obj = &thermal_data->sbus_obj;

	for (i = 0; i < THERMAL_V3_SENSOR_NUM; i++) {
		thermal_data->sensor_data[i].sensor_id = i;
		thermal_data->sensor_data[i].data = thermal_data;

		switch(i) {
		case 0:
			thermal_data->sensor_data[i].sensor_type = al_vt_sensor_type_thermal;
			break;
		case 1:
			thermal_data->sensor_data[i].sensor_type = al_vt_sensor_type_thermal_remote_0;
			break;
		case 2:
			thermal_data->sensor_data[i].sensor_type = al_vt_sensor_type_thermal_remote_1;
			break;
		case 3:
			thermal_data->sensor_data[i].sensor_type = al_vt_sensor_type_thermal_remote_2;
			break;
		}

		vt_params.sensor_type = thermal_data->sensor_data[i].sensor_type;
		al_vt_sensor_handle_init(&thermal_data->sensor_data[i].vt_handle, &vt_params);

		tz = thermal_zone_of_sensor_register(&pdev->dev, i, &thermal_data->sensor_data[i], &of_ops);
		if (IS_ERR(tz)) {
			err = PTR_ERR(tz);
			dev_err(&pdev->dev, "failed to register sensor [%d]\n", i);
			goto unregister_tzs;
		}

		thermal_data->sensor_data[i].tzd = tz;

		dev_info(&pdev->dev, "Thermal zone device [%d] registered.\n", i);
	}

	dev_info(&pdev->dev, "Thermal Sensors Loaded\n");

	platform_set_drvdata(pdev, thermal_data);

	return 0;

unregister_tzs:
	while (i--)
		thermal_zone_of_sensor_unregister(&pdev->dev,
						  thermal_data->sensor_data[i].tzd);
	return err;
}

static int al_thermal_v3_remove(struct platform_device *pdev)
{
	int i;
	struct al_thermal_v3_data *thermal_data = platform_get_drvdata(pdev);

	for (i = 0; i < THERMAL_V3_SENSOR_NUM; i++)
		thermal_zone_of_sensor_unregister(&pdev->dev,
						  thermal_data->sensor_data[i].tzd);

	mutex_destroy(&thermal_data->lock);

	return 0;
}

static const struct of_device_id al_thermal_v3_id_table[] = {
	{ .compatible = "annapurna-labs,al-thermal-v3" },
	{}
};
MODULE_DEVICE_TABLE(of, al_thermal_v3_id_table);

static struct platform_driver al_thermal_v3_driver = {
	.probe = al_thermal_v3_probe,
	.remove = al_thermal_v3_remove,
	.driver = {
		.name = "al_thermal_v3",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(al_thermal_v3_id_table),
	},
};

module_platform_driver(al_thermal_v3_driver);

MODULE_DESCRIPTION("Annapurna Labs V3 thermal driver");
MODULE_LICENSE("GPL");
