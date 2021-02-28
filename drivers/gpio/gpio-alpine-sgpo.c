/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2015 Annapurna Labs Ltd.
 */

#include <linux/kernel.h>
#include <linux/io.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#include <linux/module.h>
#include <al_hal_sgpo.h>

struct al_sgpo {
	void __iomem		*base;
	struct gpio_chip	chip;
};

static void al_sgpo_set(struct gpio_chip *chip, unsigned offset, int value)
{
	struct al_sgpo *gpio = container_of(chip, struct al_sgpo, chip);
	struct al_sgpo_if sgpo_if;

	al_sgpo_handle_init(&sgpo_if, gpio->base, 1);
	al_sgpo_user_val_set(&sgpo_if, offset, value);
}

static int al_sgpo_dir_out(struct gpio_chip *chip, unsigned offset, int value)
{
	al_sgpo_set(chip, offset, value);

	return 0;
}

static int al_sgpo_probe(struct platform_device *pdev)
{
	struct al_sgpo *gpio;
	struct gpio_chip *chip;
	struct resource *res_mem;
	int err = 0;

	gpio = devm_kzalloc(&pdev->dev, sizeof(*gpio), GFP_KERNEL);
	if (!gpio)
		return -ENOMEM;
	chip = &gpio->chip;

	res_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (res_mem == NULL) {
		dev_err(&pdev->dev, "found no memory resource\n");
		err = -ENXIO;
		goto out;
	}

	if (!devm_request_mem_region(
			&pdev->dev, res_mem->start, resource_size(res_mem), res_mem->name)) {
		dev_err(&pdev->dev, "request_region failed\n");
		err = -ENXIO;
		goto out;
	}

	gpio->base = devm_ioremap(&pdev->dev, res_mem->start, resource_size(res_mem));

	pdev->dev.platform_data = chip;
	chip->label = "al-sgpo";
	chip->dev = &pdev->dev;
	chip->owner = THIS_MODULE;

	chip->base = -1;
	if (pdev->dev.of_node) {
		const void *ptr = of_get_property(pdev->dev.of_node, "baseidx", NULL);
		if (ptr)
			chip->base = be32_to_cpup(ptr);
	}

	chip->can_sleep = false;
	chip->ngpio = AL_SGPO_PINS_NUM;
	chip->direction_output = al_sgpo_dir_out;
	chip->set = al_sgpo_set;
	err = gpiochip_add(chip);
	if (err)
		goto out;

	dev_info(&pdev->dev, "Alpine SGPO driver probed.\n");
out:
	return err;
}

static const struct of_device_id al_sgpo_ids[] = {
	{ .compatible = "annapurna-labs,alpine-sgpo" },
	{ }
};
MODULE_DEVICE_TABLE(of, alpine_sgpo_ids);

static struct platform_driver al_sgpo_driver = {
	.driver = {
		.name		= "al-sgpo",
		.of_match_table	= al_sgpo_ids,
	},
};
module_platform_driver_probe(al_sgpo_driver, al_sgpo_probe);

MODULE_DESCRIPTION("Annapurna Labs SGPO driver");
MODULE_AUTHOR("Barak Wasserstrom");
MODULE_LICENSE("GPL");
