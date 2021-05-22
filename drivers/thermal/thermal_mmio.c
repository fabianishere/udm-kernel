/*
 * Copyright 2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 */

#include <linux/bug.h>
#include <linux/module.h>
#include <linux/of_address.h>
#include <linux/platform_device.h>
#include <linux/thermal.h>

MODULE_AUTHOR("Talel Shenhar <talel@amazon.com>");
MODULE_DESCRIPTION("Thermal MMIO Driver");
MODULE_LICENSE("GPL");

struct thermal_mmio_sensor {
	void __iomem *mmio_base;
	u32 factor;
	u32 width;
};

struct thermal_mmio {
	u32 sensors_count;
	struct thermal_mmio_sensor sensors_data[0];
};

static int thermal_mmio_read(struct thermal_mmio_sensor *sensor)
{
	u64 raw_temperature = 0;

	switch (sensor->width) {
	case 1:
		raw_temperature = readb(sensor->mmio_base);
		break;
	case 2:
		raw_temperature = readw(sensor->mmio_base);
		break;
	case 4:
		raw_temperature = readl(sensor->mmio_base);
		break;
	case 8:
		raw_temperature = readq(sensor->mmio_base);
		break;
	default:
		WARN_ON(1);
		break;
	}

	return raw_temperature*sensor->factor;
}

static int thermal_mmio_get_temperature(void *private, int *temp)
{
	struct thermal_mmio_sensor *sensor =
		(struct thermal_mmio_sensor *)private;

	*temp = thermal_mmio_read(sensor);

	return 0;
}

static struct thermal_zone_of_device_ops thermal_mmio_ops = {
	.get_temp = thermal_mmio_get_temperature,
};

static int thermal_mmio_probe(struct platform_device *pdev)
{
	struct thermal_mmio *thermal_mmio;
	void __iomem *mmio_base;
	struct resource *resource;
	int temprature;
	u32 sensors_count;
	u32 sensor_width;
	u32 sensor_factor;
	int i;
	int ret;
	struct thermal_zone_device *thermal_zone;

	resource = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!resource) {
		dev_err(&pdev->dev, "fail to get platfom memory resource\n");
		return -EINVAL;
	}

	mmio_base = devm_ioremap(&pdev->dev, resource->start,
				 resource_size(resource));
	if (!mmio_base) {
		dev_err(&pdev->dev, "failed to ioremap memory\n");
		return -ENOMEM;
	}

	ret = of_property_read_u32(pdev->dev.of_node, "sensors-count",
				   &sensors_count);

	if (ret < 0) {
		dev_err(&pdev->dev, "missing sensors_count property\n");
		return -EINVAL;
	}

	if (!sensors_count) {
		dev_err(&pdev->dev, "bad sensors-count property supplied: %d\n",
			sensors_count);
		return -EINVAL;
	}

	ret = of_property_read_u32(pdev->dev.of_node, "sensor-width",
				   &sensor_width);
	if (ret < 0) {
		dev_err(&pdev->dev, "missing sensor-width property\n");
		return -EINVAL;
	}
	if (!sensor_width || (sensor_width != sizeof(u8) &&
			      sensor_width != sizeof(u16) &&
			      sensor_width != sizeof(u32) &&
			      sensor_width != sizeof(u64))) {
		dev_err(&pdev->dev, "bad sensor-width property supplied: %d\n",
			sensor_width);
		return -EINVAL;
	}

	ret = of_property_read_u32(pdev->dev.of_node, "sensor-factor",
				   &sensor_factor);
	if (ret < 0) {
		dev_err(&pdev->dev, "missing sensor-factor property\n");
		return -EINVAL;
	}
	if (!sensor_factor) {
		dev_err(&pdev->dev, "bad sensor-factor property supplied: %d\n",
			sensor_factor);
		return -EINVAL;
	}

	dev_dbg(&pdev->dev, "mmio_phy=%pa, count=%d, width=%d, factor=%d\n",
		&resource->start, sensors_count, sensor_width, sensor_factor);

	if (sensors_count*sensor_width > resource_size(resource)) {
		dev_err(&pdev->dev, "bad sensors mmio configuration\n");
		return -EINVAL;
	}

	thermal_mmio = devm_kzalloc(&pdev->dev, sizeof(*thermal_mmio) +
				(sizeof(*thermal_mmio->sensors_data)*
				 sensors_count), GFP_KERNEL);
	if (!thermal_mmio) {
		dev_err(&pdev->dev, "failed to allocate memory\n");
		return -ENOMEM;
	}

	thermal_mmio->sensors_count = sensors_count;
	for (i = 0; i < sensors_count; i++) {
		struct thermal_mmio_sensor *sensor_data =
			&thermal_mmio->sensors_data[i];
		sensor_data->mmio_base =
			(void __iomem *)((u8 *)mmio_base + sensor_width);
		sensor_data->factor = sensor_factor;
		sensor_data->width = sensor_width;
		temprature = thermal_mmio_read(sensor_data);
		thermal_zone =
			thermal_zone_of_sensor_register(&pdev->dev, i,
							sensor_data,
							&thermal_mmio_ops);
		if (IS_ERR(thermal_zone)) {
	                dev_err(&pdev->dev, "fail to register dev %d\n", i);
	                continue;

		}

		dev_info(&pdev->dev,
			 "thermal mmio %d registered, current temp = %d\n", i,
			 temprature);
	}

	platform_set_drvdata(pdev, thermal_mmio);

	return 0;
}

static int thermal_mmio_remove(struct platform_device *pdev)
{
	return 0;
}

static const struct of_device_id thermal_mmio_id_table[] = {
	{ .compatible = "thermal-mmio" },
	{}
};
MODULE_DEVICE_TABLE(of, thermal_mmio_id_table);

static struct platform_driver thermal_mmio_driver = {
	.probe = thermal_mmio_probe,
	.remove = thermal_mmio_remove,
	.driver = {
		.name = "thermal-mmio",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(thermal_mmio_id_table),
	},
};

module_platform_driver(thermal_mmio_driver);
