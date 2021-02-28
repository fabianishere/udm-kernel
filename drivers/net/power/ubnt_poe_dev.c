/*
 * ubnt_poe_dev.c: UBNT PoE API
 * Copyright 2019 Ubiquiti Networks, Inc.
 */

#include <linux/types.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/if.h>
#include <linux/device.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <../fs/proc/internal.h>

#include <linux/of.h>
#include "ubnt_poe_internal.h"

/**
 * @brief Static declarations
 */
static DEFINE_MUTEX(dev_list_lock);
static struct list_head dev_list;

/**
 * @brief Helpers
 */
#define ubnt_poe_lock() mutex_lock(&dev_list_lock)
#define ubnt_poe_unlock() mutex_unlock(&dev_list_lock)

/**
 * @brief Get group index of PoE device
 *
 * @param dev - parent device node
 * @return unit addr if success, errno otherwise
 */
static int ubnt_poe_of_get_uaddr(struct ubnt_poe_dev *dev)
{
	int rc = -ENODEV;
	uint32_t uaddr;

	rc = of_property_read_u32(dev->dev->of_node, UBNT_POE_OF_ATT_UADDR, &uaddr);
	if (rc) {
		pr_err("%s Unable to obtain unit-address", __func__);
		return rc;
	}
	return uaddr;
};

/**
 * @brief Lock & get PoE device
 *
 * NOTE: ubnt_poe_dev_put should be called after work is done to free the device
 * @param dev - pointer ubnt-poe device structure
 */
static void ubnt_poe_dev_get(struct ubnt_poe_dev *dev)
{
	if (dev) {
		mutex_lock(&dev->lock);
	}
}

/**
 * @brief Unlock PoE device after ubnt_poe_dev_get
 *
 * NOTE: ubnt_poe_dev_put has to be called after work is done to free the device
 * @param dev - ubnt-poe device structure
 */
void ubnt_poe_dev_put(struct ubnt_poe_dev *dev)
{
	if (dev) {
		mutex_unlock(&dev->lock);
	}
}
EXPORT_SYMBOL(ubnt_poe_dev_put);

/**
 * @brief Lock & get PoE device by its address
 *
 * NOTE: ubnt_poe_dev_put should be called after work is done to free the device
 * @param uaddr - device uaddr
 * @return struct ubnt_poe_dev*, pointer ubnt-poe device structure NULL on fail
 */
struct ubnt_poe_dev *ubnt_poe_dev_get_by_uaddr(int uaddr)
{
	struct ubnt_poe_dev *dev = NULL;
	struct ubnt_poe_dev *dev_it;

	ubnt_poe_lock();

	list_for_each_entry (dev_it, &dev_list, list) {
		if (uaddr != dev_it->uaddr) {
			continue;
		}
		dev = dev_it;
		break;
	}

	ubnt_poe_dev_get(dev);

	ubnt_poe_unlock();
	return dev;
}
EXPORT_SYMBOL(ubnt_poe_dev_get_by_uaddr);

/**
 * @brief Register PoE driver to ubnt-poe
 *
 * @param dev - ubnt-poe device structure
 * @return int errno
 */
int ubnt_poe_dev_reg(struct ubnt_poe_dev *dev)
{
	struct ubnt_poe_dev *dev_it;
	int rc = 0;

	if (NULL == dev) {
		return -ENODEV;
	}

	ubnt_poe_lock();

	list_for_each_entry (dev_it, &dev_list, list) {
		if (&dev_it->dev == &dev->dev) {
			rc = 0;
			goto exit;
		}
	}

	dev->uaddr = ubnt_poe_of_get_uaddr(dev);

	if (dev->uaddr < 0) {
		pr_err("%s Unable to assign PoE chip to ubnt-poe group", __func__);
		goto exit;
	}

	/* init HEAD */
	INIT_LIST_HEAD(&dev->list);
	/* lock init */
	mutex_init(&dev->lock);
	list_add_tail(&dev->list, &dev_list);

	pr_info("%s new device %s registred with uaddr %d\n", __func__,
		(dev->dev->driver) ? dev->dev->driver->name : "unknown", dev->uaddr);

exit:
	ubnt_poe_unlock();
	return rc;
}

/**
 * @brief Unregister PoE driver from ubnt-poe
 *
 * @param dev - ubnt-poe device structure
 */
void ubnt_poe_dev_unreg(struct ubnt_poe_dev *dev)
{
	mutex_lock(&dev->lock);
	ubnt_poe_lock();
	list_del(&dev->list);
	ubnt_poe_unlock();
	mutex_unlock(&dev->lock);
}

static int __init ubnt_poe_dev_init(void)
{
	INIT_LIST_HEAD(&dev_list);
	return 0;
}

static void __exit ubnt_poe_dev_exit(void)
{
	INIT_LIST_HEAD(&dev_list);
	return;
}

module_init(ubnt_poe_dev_init);
module_exit(ubnt_poe_dev_exit);
