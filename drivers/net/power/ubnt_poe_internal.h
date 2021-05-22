/*
 * ubnt_poe_internal.h: PoE API
 * Copyright 2019 Ubiquiti Networks, Inc.
 */
#ifndef _UBNT_POE_INTERNAL_H_
#define _UBNT_POE_INTERNAL_H_

#include <linux/types.h>
#include <linux/ubnt_poe.h>

/* @note used in both s2p & dev nodes */
#define UBNT_POE_OF_ATT_UADDR "reg"

/**
 * ############################################################
 * @note !!!! THOSE SHOULD MATCH proc-poe-api.h IN UBNTHAL !!!!
 * ############################################################
 */

/**
 * @brief Known PoE modes
 * @note those were taken from udapi
 */
#define POE_MODES(MODE)                \
	MODE(off, OFF)                     \
	MODE(24v, PASV_24V)                \
	MODE(48v, PASV_48V)                \
	MODE(54v, PASV_54V)                \
	MODE(24v_4pair, PASV_24V4P)        \
	MODE(54v_4pair, PASV_54V4P)        \
	MODE(pthru, PTHRU)            \
	MODE(active_3at, ACTV_3AT) \
	MODE(active_3af, ACTV_3AF)         \
	MODE(active_3bt, ACTV_3BT)

/**
 * @brief Enum with PoE modes
 */
enum {
    #define MODE(_str, _enum) POE_MODE_##_enum,
        POE_MODES(MODE)
    #undef MODE
	POE_MODE_COUNT,
	POE_MODE_UNSPEC
};

/**
 * @brief Eum with PoE capabilities
 */
enum {
    POE_CAP_UNDEF = 0,
    #define MODE(_str, _enum) POE_CAP_##_enum = (1 << POE_MODE_##_enum ),
        POE_MODES(MODE)
    #undef MODE
};

struct ubnt_poe_status {
	uint32_t power_mw;
};

/**
 * ############################################################
 */

/**
 * @brief Register PoE driver to ubnt-poe
 *
 * @param dev - ubnt-poe device structure
 * @return int errno
 */
int ubnt_poe_dev_reg(struct ubnt_poe_dev *dev);

/**
 * @brief Unregister PoE driver from ubnt-poe
 *
 * @param dev - ubnt-poe device structure
 */
void ubnt_poe_dev_unreg(struct ubnt_poe_dev *dev);

#endif /* _UBNT_POE_INTERNAL_H_ */
