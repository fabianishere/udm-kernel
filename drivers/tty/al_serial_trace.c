/*
* Copyright (C) 2016 Annapurna Labs Ltd.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include <linux/of_address.h>
#include <linux/of_platform.h>
#include <linux/io-mapping.h>
#include <linux/module.h>
#include <linux/ktime.h>
#include "al_serial_trace.h"
#include "al_trace.h"

#define TRACE_TMP_BUFF_SIZE	SZ_64K

struct al_serial_trace_singleton {
	struct al_trace_handle trace_tmp;
	struct al_trace_handle trace_final;
	struct al_trace_handle *trace;
	unsigned int disabled;
	void *regs;
	void *trace_buff_tmp;
};

static const char drv_name[] = "al-serial-trace";

static struct al_serial_trace_singleton singleton;

static unsigned int time_stamp_str_get(char *s, unsigned int len)
{
	unsigned int i;

	i = snprintf(s, len, "%010u", (unsigned int)(ktime_to_us(ktime_get())));
	if (i < len)
		s[i++] = 'u';
	if (i < len)
		s[i++] = 's';
	if (i < len)
		s[i++] = ':';
	if (i < len)
		s[i++] = ' ';

	return i;
}

void al_serial_trace_putchar(int c)
{
	if (singleton.disabled)
		return;

	if (!singleton.trace) {
		singleton.disabled = 1;
		singleton.trace_buff_tmp = kmalloc(TRACE_TMP_BUFF_SIZE, GFP_KERNEL);

		if (singleton.trace_buff_tmp) {
			singleton.trace = &singleton.trace_tmp;
			al_trace_init(singleton.trace, singleton.trace_buff_tmp,
				TRACE_TMP_BUFF_SIZE, AL_FALSE, time_stamp_str_get);
			singleton.disabled = 0;
			pr_info("AL Serial Trace temporarily initialized to virt %p, size %x\n",
				singleton.trace_buff_tmp, TRACE_TMP_BUFF_SIZE);
		} else {
			pr_err("AL Serial Trace cannot allocate temporary buffer!\n");
			return;
		}
	}

	al_trace_putchar(singleton.trace, c);
}

static int al_trace_of_probe(struct platform_device *op)
{
	struct device_node *of_node = op->dev.of_node;
	struct resource res;
	int ret = 0;
	struct al_trace_handle *trace_prev = singleton.trace;

	singleton.disabled = 1;

	/* Get the physical address of the trace */
	ret = of_address_to_resource(of_node, 0, &res);
	if (ret) {
		dev_err(&op->dev, "Unable to find trace physical address\n");
		ret = -ENODEV;
		kfree(singleton.trace_buff_tmp);
		goto out_return;
	}

	/* ioremap the registers for use */
	singleton.regs = of_iomap(of_node, 0);
	if (!singleton.regs) {
		dev_err(&op->dev, "Unable to ioremap registers\n");
		ret = -ENOMEM;
		kfree(singleton.trace_buff_tmp);
		goto out_return;
	}

	singleton.trace = &singleton.trace_final;
	al_trace_init(singleton.trace, singleton.regs, resource_size(&res), AL_TRUE,
		time_stamp_str_get);
	if (trace_prev) {
		al_trace_append(singleton.trace, trace_prev);
		kfree(singleton.trace_buff_tmp);
		singleton.disabled = 0;
		dev_info(&op->dev, "trace moved to phys %llx, virt %p, size %llx\n",
			res.start, singleton.regs, resource_size(&res));
	} else {
		singleton.disabled = 0;
		dev_info(&op->dev, "trace initialized to phys %llx, virt %p, size %llx\n",
			res.start, singleton.regs, resource_size(&res));
	}

	dev_info(&op->dev, "AL Serial Trace driver Loaded\n");

out_return:
	return ret;
}

static int al_trace_of_remove(struct platform_device *op)
{
	iounmap(singleton.regs);

	return 0;
}

static const struct of_device_id al_trace_of_match[] = {
	{ .compatible = "annapurna-labs,al-serial-trace", },
	{},
};

static struct platform_driver al_trace_of_driver = {
	.probe		= al_trace_of_probe,
	.remove		= al_trace_of_remove,
	.driver		= {
		.name		= drv_name,
		.of_match_table	= al_trace_of_match,
	},
};

module_platform_driver(al_trace_of_driver);

MODULE_AUTHOR("Barak Wasserstrom <barak@annapurnalabs.com>");
MODULE_DESCRIPTION("AL Serial Trace");
MODULE_LICENSE("GPL");
