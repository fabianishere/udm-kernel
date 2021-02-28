/* al_mod_eth_sysfs.h: AnnapurnaLabs Unified 1GbE and 10GbE ethernet driver.
 *
 * Copyright (c) 2013 AnnapurnaLabs
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation.
 */
#ifndef __AL_ETH_SYSFS_H__
#define __AL_ETH_SYSFS_H__

#include <linux/version.h>

#if LINUX_VERSION_CODE > KERNEL_VERSION(3,3,0)

int al_mod_eth_sysfs_init(struct device *dev);

void al_mod_eth_sysfs_terminate(struct device *dev);

#else

static inline int al_mod_eth_sysfs_init(struct device *dev)
{
	return 0;
}

static inline void al_mod_eth_sysfs_terminate(struct device *dev)
{
}

#endif

#endif /* __AL_ETH_SYSFS_H__ */

