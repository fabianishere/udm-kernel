/*
 * linux/rtl83xx.h: definitions for rtl83xx mdio read/write
 */

#ifndef _LINUX_RTL83XX_H
#define _LINUX_RTL83XX_H

struct rtl83xx_ops {
	void (*mdio_write)(struct net_device *, struct ifreq *);
	void (*mdio_read)(struct net_device *, struct ifreq *);
};
#endif /* _LINUX_RTL83XX_H */
