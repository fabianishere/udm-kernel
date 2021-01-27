/*
 * ubnt_poe.h: PoE API
 * Copyright 2019 Ubiquiti Networks, Inc.
 */
#ifndef _UBNT_POE_DEV_H_
#define _UBNT_POE_DEV_H_

#include <linux/types.h>

struct ubnt_poe_dev_ops;
struct ubnt_poe_status;

/**
 * @brief PoE dev control structure
 */
struct ubnt_poe_dev {
	/* ops for controlling PoE chip */
	const struct ubnt_poe_dev_ops *ops;
	/* number of ports */
	uint32_t port_no;

	/* will be automatically filled */
	int  uaddr;
	struct mutex lock;
	struct list_head list;
	/* device structure */
	struct device *dev;
};

/**
 * @brief PoE s2p control structure
 */
struct ubnt_poe_s2p {
	uint8_t state;
	/* Strobe STCB */
	uint32_t uaddr;
	int gpio_stcp;
	struct mutex lock;
	struct spi_device *spi_device;
	struct list_head list;
};

/**
 * @brief Operations over PoE chip
 */
struct ubnt_poe_dev_ops {
	/* get port's mode */
	int (*port_mode_set)(struct ubnt_poe_dev *dev, uint32_t port, uint32_t mode);
	/* set port's mode */
	int (*port_mode_get)(struct ubnt_poe_dev *dev, uint32_t port, uint32_t  *mode);
	/* get port's capabilities */
	int (*port_cap_get)(struct ubnt_poe_dev *dev, uint32_t port, uint32_t *mode);
	/* get status information */
	int (*port_status_get)(struct ubnt_poe_dev *dev, uint32_t port, struct ubnt_poe_status *status);
};

/**
 * @brief Enable/Disable port
 *
 * @param s2p - s2p control structure
 * @param port - port number - port number
 * @param mode - poe mode - poe mode
 * @return int errno
 */
void ubnt_poe_s2p_mode_set(struct ubnt_poe_s2p *s2p, uint32_t port, uint32_t mode);

/**
 * @brief Lock & get PoE s2p by its address
 *
 * NOTE: ubnt_poe_s2p_put has to  called after work is done to free the s2p
 * @param uaddr - s2p uaddr
 * @return struct ubnt_poe_s2p*, pointer ubnt-poe s2p structure, NULL on fail
 */
struct ubnt_poe_s2p *ubnt_poe_s2p_get_by_uaddr(int uaddr);

/**
 * @brief Unlock PoE s2p after ubnt_poe_s2p_get
 *
 * @param s2p - ubnt-poe s2p structure
 */
void ubnt_poe_s2p_put(struct ubnt_poe_s2p *s2p);

/**
 * @brief Lock & get PoE device by its address
 *
 * NOTE: ubnt_poe_dev_put has to be called after work is done to free the device
 * @param uaddr - device uaddr
 * @return struct ubnt_poe_dev*, pointer ubnt-poe device structure NULL on fail
 */
struct ubnt_poe_dev *ubnt_poe_dev_get_by_uaddr(int uaddr);

/**
 * @brief Unlock PoE device after ubnt_poe_dev_get
 *
 * @param dev - ubnt-poe device structure
 */
void ubnt_poe_dev_put(struct ubnt_poe_dev *dev);


#endif /* _UBNT_POE_DEV_H_ */
