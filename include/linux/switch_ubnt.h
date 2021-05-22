/*
 * swconfig_ubnt.c: swconfig UBNT extension
 * Copyright 2020 Ubiquiti Networks, Inc.
 */

#ifndef __SWCONFIG_UBNT_EXT_H
#define __SWCONFIG_UBNT_EXT_H

#ifdef CONFIG_SWCONFIG_UBNT_EXT

#include <linux/types.h>
#include <linux/list.h>
#include <uapi/linux/switch.h>

#define UEXT_ARL_LIST_HASHSIZE 32
#define UEXT_ARL_LIST_HASH(addr) \
	((((const uint8_t *)(addr))[0] ^ ((const uint8_t *)(addr))[5]) % UEXT_ARL_LIST_HASHSIZE)
#define UEXT_ARL_READ_WORK_DELAY_MS 6000
#define UEXT_ARL_DEFAULT_AGE_TIME_S 300
#define UEXT_MIB_READ_WORK_DELAY_MS 1000

#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC2STR(addr) addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]

/**
 * @brief uext mib counters
 *
 */
struct switch_port_mib {
#define COUNTER(_enum, _name) uint64_t _name;
	SWITCH_MIB_COUNTERS(COUNTER)
#undef COUNTER
	unsigned long timestamp;
};

/**
 * @brief Forward declaration
 */
struct switch_dev;
struct switch_attr;
struct switch_val;

/**
 * @brief Iterator initialization
 */
enum uext_arl_iter_op {
	UEXT_ARL_IT_BEGIN,
	UEXT_ARL_IT_NEXT,
	UEXT_ARL_IT_END
};

/**
 * @brief iterator structure
 */
struct uext_arl_iter {
	enum uext_arl_iter_op op;
	void *data;
};

/**
 * @brief LUT entry
 */
struct uext_arl_lut_entry {
	uint8_t port;
	/**
	 * @note
	 * uext expects ARL entry's mac address in big endian
	 */
	uint8_t mac[ETH_ALEN];
	uint16_t vid;
};

/**
 * @brief Cache entry
 */
struct uext_arl_cache_entry {
	struct uext_arl_lut_entry lut_e;
	unsigned long last_seen;
	unsigned long first_add;
	struct list_head list;
};

/**
 * @brief ARL cache
 */
struct uext_arl_cache {
	uint32_t age_time;
	struct mutex lock;
	struct uext_arl_cache_entry table[UEXT_ARL_LIST_HASHSIZE];
	struct delayed_work worker;
	struct proc_dir_entry *procfs;
	void *private;
};

/**
 * @brief MIB cache
 */
struct uext_mib_cache {
	struct mutex lock;
	struct switch_port_mib *counters;
	struct delayed_work worker;
	void *private;
};

/**
 * @brief UBNT extension structure
 */
struct uext {
	/* procfs entry */
	struct proc_dir_entry *procfs;

	/* ARL cache */
	struct uext_arl_cache *arl_cache;

	/* MIB cache */
	struct uext_mib_cache *mib_cache;
	/* more things to come */
};

/**
 * @brief UBNT extension structure
 */
struct uext_ops {
	int (*get_arl_entry)(struct switch_dev *dev, struct uext_arl_lut_entry *entry_out,
			     struct uext_arl_iter *it);
	int (*get_port_mib)(struct switch_dev *dev, struct switch_port_mib *entry_out,
				  int port);

	/* SVLAN support */
	int (*get_svlan_ports)(struct switch_dev *dev, struct switch_val *val);
	int (*set_svlan_ports)(struct switch_dev *dev, struct switch_val *val);

};

/**
 * @brief Global attribute
 */
/* ATT ( _name, _desc, _enum, _type, _read_handler, _write_handler )   */

#define UBNT_EXT_GLOBAL_ATT(UEXT_ATT)                                                    \
	UEXT_ATT("uext_arl_cache_age_time", "ARL cache age time (secs)", ARL_CACHE_AGE_TIME, \
		 SWITCH_TYPE_INT, swconfig_uext_arl_cache_age_time_get,                          \
		 swconfig_uext_arl_cache_age_time_set)

#define UBNT_EXT_PORT_ATT(UEXT_ATT)                                                      \
	UEXT_ATT("uext_mib", "Get port's cached MIB counters", MIB, SWITCH_TYPE_MIB,         \
		 swconfig_uext_port_mib_get, NULL)

#define UBNT_EXT_SVLAN_ATT(UEXT_ATT)                                                     \
	UEXT_ATT("ports", "SVLAN port mapping", PORTS, SWITCH_TYPE_PORTS,                    \
		 swconfig_uext_get_svlan_ports, swconfig_uext_set_svlan_ports)

/* Prototypes */

/**
 * @brief Handle swconfig reset across all caches of ubnt extension
 *
 * @param dev - switch control structure
 * @return int - error from errno.h, 0 on success
 */
int swconfig_uext_reset(struct switch_dev *dev);

/**
 * @brief Get ARL table
 *
 * @param dev - switch control structure
 * @param attr - switch attribute structure
 * @param val - switch value structure
 * @return int - error from errno.h, 0 on success
 */
int swconfig_uext_arl_table_get(struct switch_dev *dev, const struct switch_attr *attr,
				struct switch_val *val);

/**
 * @brief Get ARL cache age time
 *
 * @param dev - switch control structure
 * @param attr - switch attribute structure
 * @param val - switch value structure
 * @return int - error from errno.h, 0 on success
 */
int swconfig_uext_arl_cache_age_time_get(struct switch_dev *dev, const struct switch_attr *attr,
					 struct switch_val *val);

/**
 * @brief Set ARL cache age time
 *
 * @param dev - switch control structure
 * @param attr - switch attribute structure
 * @param val - switch value structure
 * @return int - error from errno.h, 0 on success
 */
int swconfig_uext_arl_cache_age_time_set(struct switch_dev *dev, const struct switch_attr *attr,
					 struct switch_val *val);

/**
 * @brief Get port's cached MIB counter
 *
 * @param dev - switch control structure
 * @param attr - switch attribute structure
 * @param val - switch value structure
 * @return int - error from errno.h, 0 on success
 */
int swconfig_uext_port_mib_get(struct switch_dev *dev, const struct switch_attr *attr,
					 struct switch_val *val);

/**
 * @brief Part of register_switch - initialize ubnt extension
 *
 * @param dev - switch control structure
 * @return int - error from errno.h, 0 on success
 */
int swconfig_uext_register(struct switch_dev *dev);

/**
 * @brief Part of unregister_switch - destroy ubnt extension
 *
 * @param dev - switch control structure
 * @return int - error from errno.h, 0 on success
 */
int swconfig_uext_unregister(struct switch_dev *dev);

/**
 * @brief Initialize ubnt swconfig extension (uext)
 *
 * @return int - error from errno.h, 0 on success
 */
int swconfig_uext_init(void);

/**
 * @brief Destroy ubnt swconfig extension (uext)
 *
 * @return int - error from errno.h, 0 on success
 */
void swconfig_uext_exit(void);

/**
 * @brief Netlink extension for sending SWITCH_TYPE_MIB
 *
 * @param msg - skbuf structure
 * @param info - netlink informative structure
 * @param attr - attribute number
 * @param mib - mib counter structure
 * @return int - error from errno.h, 0 on success
 */

int swconfig_uext_send_mib(struct sk_buff *msg, struct genl_info *info, int attr,
		   const struct switch_port_mib *mib);

/**
 * @brief Get SVLAN members
 *
 * @param dev - switch control structure
 * @param attr - switch attribute structure
 * @param val - switch value structure
 * @return int - error from errno.h, 0 on success
 */
int swconfig_uext_get_svlan_ports(struct switch_dev *dev, const struct switch_attr *attr,
			struct switch_val *val);

/**
 * @brief Set SVLAN members
 *
 * @param dev - switch control structure
 * @param attr - switch attribute structure
 * @param val - switch value structure
 * @return int - error from errno.h, 0 on success
 */
int swconfig_uext_set_svlan_ports(struct switch_dev *dev, const struct switch_attr *attr,
			struct switch_val *val);

#else /* CONFIG_SWCONFIG_UBNT_EXT */
#define UBNT_EXT_GLOBAL_ATT(UEXT_ATT)
#define UBNT_EXT_PORT_ATT(UEXT_ATT)
#define UBNT_EXT_SVLAN_ATT(UEXT_ATT)
#endif /*CONFIG_SWCONFIG_UBNT_EXT*/

#endif /* __SWCONFIG_UBNT_EXT_H */
