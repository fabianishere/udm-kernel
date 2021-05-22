/*
 * rtl8370.c: RTL8370 switch driver
 * Copyright 2018 Ubiquiti Networks, Inc.
 */

#include <linux/bitops.h>
#include <linux/etherdevice.h>
#include <linux/if.h>
#include <linux/if_ether.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/lockdep.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/netdevice.h>
#include <linux/netlink.h>
#include <linux/phy.h>
#include <linux/skbuff.h>
#include <linux/switch.h>
#include <linux/version.h>
#include <linux/workqueue.h>
#include <linux/of_gpio.h>
#include <linux/gpio.h>
#include <net/genetlink.h>

#include <linux/of.h>

#include "rtl83xx_api/rtk_error.h"
#include "rtl83xx_api/rtk_switch.h"
#include "rtl83xx_api/rtk_types.h"

#include "rtl83xx_api/port.h"
#include "rtl83xx_api/stat.h"
#include "rtl83xx_api/vlan.h"
#include "rtl83xx_api/svlan.h"
#include "rtl83xx_api/acl.h"
#include "rtl83xx_api/led.h"
#include "rtl83xx_api/l2.h"

#include "rtl8370.h"

/**
 * @brief This allows to translate error code to appropriate error message.
 */
#ifdef RTL8370_INTERPRET_API_ERRORS

/* clang-format off */
static const rtl_api_err_t err_table[] = {
#define X(code, desc) { code, desc },
	RTL_API_ERRORS_DEFS(X)
#undef X
	{ RT_ERR_END, "" }
};
/* clang-format on */

static const char *rtl_strerror(uint32_t code)
{
	int i = 0;
	for (i = 0; i < RTL_API_ERR_COUNT; ++i) {
		if (err_table[i].code == code) {
			return err_table[i].desc;
		}
	}
	return "";
}

#define rtl_err(rc, fmt, ...) \
	printk(KERN_ERR pr_fmt(fmt) " rt_err :0x%0x (%s)\n", ##__VA_ARGS__, rc, rtl_strerror(rc))
#define rtl_info(...) pr_info(__VA_ARGS__)
#define rtl_warn(...) pr_warn(__VA_ARGS__)

#else
#define rtl_err(rc, fmt, ...) printk(KERN_ERR pr_fmt(fmt) " rt_err :0x%0x\n", ##__VA_ARGS__, rc)
#define rtl_info(...) pr_info(__VA_ARGS__)
#define rtl_warn(...) pr_warn(__VA_ARGS__)

#endif /* RTL8370_INTERPRET_API_ERRORS */

/**
 * @brief Port mapping translation table
 */
/* clang-format off */
static const struct rtl8370_port_mapping _rtl_port_mapping [] =
{
#define X(phy, log, desc) log,
#define Y(phy, log, desc) [log] = phy,
	{
		.name = "RTL8370B",
		.id = CHIP_RTL8370B,
		.p2l = { PORT_MAPPING_DEFAULT_RTL8370(X) },
		.l2p = { PORT_MAPPING_DEFAULT_RTL8370(Y) },
		.cpu = { PHY_PORT8, PHY_PORT9 },
		.port_cnt = 10,
		.cpu_cnt = 2,
	},
	{
		.name = "RTL8367(S/C)",
		.id = CHIP_RTL8367C,
		.p2l = { PORT_MAPPING_DEFAULT_RTL8367(X) },
		.l2p = { PORT_MAPPING_DEFAULT_RTL8367(Y) },
		.cpu = { PHY_PORT5, PHY_PORT6 },
		.port_cnt = 7,
		.cpu_cnt = 2,
	},
#undef Y
#undef X
};
/* clang-format on */

/**
 * @brief OF device id
 */
static const struct of_device_id realtek_rtl8370_of_table[] =
{
	{ .compatible = "realtek,rtl8370mb", },
	{ .compatible = "realtek,rtl8367c",  },
	{}
};

/* OF LED profile attribute key */
#define RTL8370_OF_ATT_LED_PROFILE "led-profile"
#define RTL8370_OF_ATT_RESET_ON_INIT "reset-on-init"
#define RTL8370_OF_ATT_RESET_GPIOS "reset-gpios"
#define RTL8370_OF_ATT_PHY_ADDR "reg"
#define RTL8370_OF_NODE_MAC "gmac"
#define RTL8370_OF_NODE_CPU_PORT "cpu_port"
#define RTL8370_OF_ATT_MAC_MODE "mode"
#define RTL8370_OF_ATT_TX_DELAY "delay-tx"
#define RTL8370_OF_ATT_RX_DELAY "delay-rx"
#define RTL8370_OF_ATT_NWAY_ENABLE "nway_enable"

#define RTL8370_OF_VAL_MODE_DISABLED "disabled"
#define RTL8370_OF_VAL_MODE_SGMII_2_5G "sgmii-2.5g"
#define RTL8370_OF_VAL_MODE_1000X "1000x"
#define RTL8370_OF_VAL_MODE_RGMII "rgmii"

/**
 * @brief LED profiles
 */
enum {
	LED_PROFILE_OFF = 0,
	LED_PROFILE_DEFAULT_SERIAL,
	LED_PROFILE_DEFAULT_PARALLEL,
	LED_PROFILE_SINGLE_STATE,
	LED_PROFILE_COUNT,

};

/**
 * @brief Retrieve pointer to the rtl8370's private structure
 *
 * @param dev - switch control structure
 * @return struct rtl8370_priv* - rtl8370's private structure
 */
static inline struct rtl8370_priv *sw_to_rtl8370(struct switch_dev *dev)
{
	return container_of(dev, struct rtl8370_priv, swdev);
}

/**
 * @brief Retrieve pointer to the rtl8370's private structure from acl chip structure
 *
 * @param hw - platform dependent ACL control structure
 * @return struct rtl8370_priv* - rtl8370's private structure
 */
static inline struct rtl8370_priv *acl_hw_to_rtl8370_priv(struct acl_hw *hw)
{
	return container_of(hw, struct rtl8370_priv, hw_acl);
}

/**
 * @brief Translates logical port to physical
 *
 * @param l - logical port
 * @param priv - rtl8370's private structure
 * @return uint8_t - physical port
 */
static inline uint8_t _rtl_port_l2p(struct rtl8370_priv *priv, uint8_t l)
{
	return (l < RTL8370_PORT_MAX_LOG) ? priv->port_map->l2p[l] : PHY_PORT_UNKNOWN;
}

/**
 * @brief Translates physical port to logical
 *
 * @param p - physical port
 * @param priv - rtl8370's private structure
 * @return uint8_t - logical port
 */
static inline uint8_t _rtl_port_p2l(struct rtl8370_priv *priv, uint8_t p)
{
	return (p < priv->swdev.ports) ? priv->port_map->p2l[p] : UNDEFINE_PORT;
}

/**
 * @brief Test whether a port hasn't PHY
 *
 * @param p - logical port
 * @param priv - rtl8370's private structure
 * @return bool true for CPU port, false otherwise
 */
static inline bool _rtl_port_has_no_phy(struct rtl8370_priv *priv, uint8_t p)
{
	return (RT_ERR_OK == rtk_switch_isExtPort(_rtl_port_p2l(priv, p)));
}

/**
 * @brief Construct a new define mutex object
 */
static DEFINE_MUTEX(rtl8370_dev_list_lock);

/**
 * @brief Construct a new list head object
 */
static LIST_HEAD(rtl8370_dev_list);

/***
 * TRANSLATION HELPERS
 */

#define RTL_API_TR_ENTRY(val_api, val_linux) \
	case val_api: {                      \
		ret = val_linux;             \
		break;                       \
	}

#define RTL_API_TR_SPEED_SWITCH(X)                  \
	X(PORT_SPEED_10M, SWITCH_PORT_SPEED_10)     \
	X(PORT_SPEED_100M, SWITCH_PORT_SPEED_100)   \
	X(PORT_SPEED_1000M, SWITCH_PORT_SPEED_1000) \
	X(PORT_SPEED_2500M, SWITCH_PORT_SPEED_2500)

#define RTL_API_TR_SPEED_PHY(X)         \
	X(PORT_SPEED_10M, SPEED_10)     \
	X(PORT_SPEED_100M, SPEED_100)   \
	X(PORT_SPEED_1000M, SPEED_1000) \
	X(PORT_SPEED_2500M, SPEED_2500)

/**
 * @brief Translate API value to swconfig value
 *
 * @param speed - speed value (API)
 * @return int speed value (SWCONFIG)
 */
static inline int rtl8370_tr_port_speed_switch(rtk_port_speed_t speed)
{
	int ret = SWITCH_PORT_SPEED_UNKNOWN;

	switch (speed) {

	RTL_API_TR_SPEED_SWITCH(RTL_API_TR_ENTRY);

	default:
		ret = SWITCH_PORT_SPEED_UNKNOWN;
	}

	return ret;
}

/**
 * @brief Translate API value to ethtool value
 *
 * @param speed - speed value (API)
 * @return int speed value (ethtool)
 */
static inline int rtl8370_tr_port_speed_ethtool(rtk_port_speed_t speed)
{
	int ret = SPEED_UNKNOWN;

	switch (speed) {

	RTL_API_TR_SPEED_PHY(RTL_API_TR_ENTRY);

	default:
		ret = SPEED_UNKNOWN;
	}

	return ret;
}

/**
 * @brief Check if VLAN num is valid
 *
 * @param dev - switch control structure
 * @param vlan - VLAN number
 * @return 0 success, 1 on error
 */
static inline int rtl8370_vlan_valid(struct switch_dev *dev, uint16_t vlan)
{
	return (vlan < dev->vlans) ? 0 : 1;
}

/**
 * @brief Check if SVLAN num is valid
 *
 * @param svlan - SVLAN number
 * @return 0 success, 1 on error
 */
static inline int rtl8370_svlan_valid(uint16_t svlan)
{
	return (svlan <= RTL8370_SVLAN_MAX_VID) ? 0 : 1;
}

/**
 * @brief Sync VLAN state
 *
 * @param priv - rtl8370's private structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_vlan_apply(struct rtl8370_priv *priv)
{
	struct rtl8370_svlan_table *svlan = &priv->svlan_table;
	rtk_api_ret_t rc;
	rtk_vlan_cfg_t vlan_cfg = { 0 };
	rtk_portmask_t keep_mask_port = { 0 }, keep_mask_trunk = { 0 }, keep_default = { 0 };
	rtk_svlan_memberCfg_t svlan_cfg = { 0 };
	acl_entry_t acl_entry = { 0 };

/* helper */
#define IS_VLAN_AWARE(i) (priv->vlan_enabled & BIT(i))
	uint16_t i, j, svlan_trunk = 0, svlan_port = 0, svlan_acl = 0;

	/* Init 802.1q VLAN - should be called before PVID setting*/
	rc = rtk_vlan_init();
	if (RT_ERR_OK != rc) {
		rtl_err(rc, "%s: Unable to init vlans.", __func__);
		return -ENODEV;
	}

	/* filter SVLAN members & trunks */
	for (i = 0; i < RTL8370_NUM_SVLAN; i++) {
		if (!svlan->lut[i].member)
			continue;

		for (j = 0; j < priv->swdev.ports; j++) {
			if (svlan->lut[i].member & BIT(j)) {
				if (svlan->lut[i].trunk & BIT(j)) {
					RTK_PORTMASK_PORT_SET(keep_mask_trunk, _rtl_port_p2l(priv, j));
				} else if (!IS_VLAN_AWARE(j)) {
					RTK_PORTMASK_PORT_SET(keep_mask_port, _rtl_port_p2l(priv, j));
				}
			}
		}
		svlan_trunk |= svlan->lut[i].trunk;
		acl_entry.svid = svlan->lut[i].id;
		acl_entry.type = ACL_RULE_VLAN_ASSIGNMENT;
		if(!ubnt_acl_rule_get(&priv->hw_acl, &acl_entry)) {
			/**
			 * @note
			 * SVLANs used by ACL are designated only for filtered traffic.
			 * The reset of the traffic will be directed to default SVLAN.
			 */
			svlan_acl |= (svlan->lut[i].member & ~svlan->lut[i].trunk);
		}	else {
			svlan_port |= (svlan->lut[i].member & ~svlan->lut[i].trunk);
		}
	}

/* helpers */
#define IS_SVLAN_ACL(i) (svlan_acl & BIT(i))
#define IS_SVLAN_TRUNK(i) (svlan_trunk & BIT(i))
#define IS_SVLAN_PORT(i) (svlan_port & BIT(i))

	/* Set PVIDs, keep/transparent mask */
	for (i = 0; i < priv->swdev.ports; i++) {
		rtk_portmask_t *mask;
		/* Set PVID, use default SVLAN if port is a part of SVLAN */
		uint16_t pvid = (IS_SVLAN_PORT(i) && !IS_VLAN_AWARE(i)) ? svlan->default_svid[i] :
									  priv->pvid_table[i];
		rc = rtk_vlan_portPvid_set(_rtl_port_p2l(priv, i), pvid, RTL8370_DEFAULT_PRIORITY);
		if (RT_ERR_OK != rc) {
			rtl_err(rc, "%s: Failed to set port (%d) to specified VLAN ID (PVID: %d)",
				__func__, i, pvid);
			return -ENODATA;
		}

		rc = rtk_port_efid_set(_rtl_port_p2l(priv, i), priv->efid_table[i]);
		if (RT_ERR_OK != rc) {
			rtl_err(rc, "%s: Failed to set efid for port %d.", __func__,
				priv->efid_table[i]);
			return -ENODATA;
		}

		if ((IS_SVLAN_PORT(i) && !IS_VLAN_AWARE(i)) || IS_SVLAN_TRUNK(i)) {
			/*
			* Set transparent/keep mode for trunk and all non VLAN aware ports.
			* Don't apply VLAN ingress filter between these ports.
			*/
			mask = (IS_SVLAN_TRUNK(i)) ? &keep_mask_port : &keep_mask_trunk;
		} else {
			/* use default for all other ports */
			mask = &keep_default;
		}

		rc = rtk_vlan_transparent_set(_rtl_port_p2l(priv, i), mask);
		if (RT_ERR_OK != rc) {
			rtl_err(rc, "%s: Failed to set transparent mask for port %d",
				__func__, i);
			return -ENODATA;
		}
		rc = rtk_vlan_keep_set(_rtl_port_p2l(priv, i), mask);
		if (RT_ERR_OK != rc) {
			rtl_err(rc, "%s: Failed to set keep mask for port %d", __func__, i);
			return -ENODATA;
		}
	}

	/* Apply 802.1q VLANs */
	for (i = 0; i < priv->swdev.vlans; i++) {
		if (!priv->vlan_table[i].members)
			continue;

		RTK_PORTMASK_CLEAR(vlan_cfg.untag);
		RTK_PORTMASK_CLEAR(vlan_cfg.mbr);
		for (j = 0; j < priv->swdev.ports; j++) {
			if (!(priv->vlan_table[i].members & BIT(j)))
				continue;

			RTK_PORTMASK_PORT_SET(vlan_cfg.mbr, _rtl_port_p2l(priv, j));
			/* untag ports with pvid - port based vlans */
			if (!(priv->pvid_tagged & BIT(j)) && (priv->pvid_table[j] == i)) {
				RTK_PORTMASK_PORT_SET(vlan_cfg.untag, _rtl_port_p2l(priv, j));
			}
		}
		/* Apply FID */
		vlan_cfg.fid_msti = priv->vlan_table[i].fid;
		/* Enable IVL learning */
		vlan_cfg.ivl_en = (priv->vlan_table[i].ivl_enabled) ? ENABLED : DISABLED;
		/* update VLAN table*/
		rc = rtk_vlan_set(i, &vlan_cfg);
		if (RT_ERR_OK != rc) {
			rtl_err(rc, "%s: Failed to set VLAN.", __func__);
			return -ENODATA;
		}
	}

	/* Init 802.1ad VLAN */
	rc = rtk_svlan_init();
	if (RT_ERR_OK != rc) {
		rtl_err(rc, "%s: Unable to init svlans.", __func__);
		return -ENODEV;
	}

	/*
	 * Create a default SVLAN
	 * @note when SVLAN filter is enabled, every port should be assigned to some SVLAN.
	 * By default port is assigned in default SVLAN (RTL8370_DEFAULT_SVLAN_VID).
	 */
	if (svlan_port || svlan_acl) {
		RTK_PORTMASK_CLEAR(svlan_cfg.untagport);
		RTK_PORTMASK_CLEAR(svlan_cfg.memberport);
		for (i = 0; i < priv->swdev.ports; i++) {
			if (!IS_SVLAN_PORT(i) || IS_SVLAN_ACL(i)) {
				RTK_PORTMASK_PORT_SET(svlan_cfg.memberport, _rtl_port_p2l(priv, i));
			}
		}

		/* No need to set efid for default SVLAN group */
		svlan_cfg.efiden = DISABLED;
		svlan_cfg.untagport = svlan_cfg.memberport;
		/* update SVLAN table*/
		rc = rtk_svlan_memberPortEntry_set(RTL8370_DEFAULT_SVLAN_VID, &svlan_cfg);
		if (RT_ERR_OK != rc) {
			rtl_err(rc, "%s: Failed to set default SVLAN.", __func__);
			return -ENODATA;
		}
		/* Assing SVLAN default SVLAN to untagged traffic */
		rc = rtk_svlan_untag_action_set(UNTAG_ASSIGN, RTL8370_DEFAULT_SVLAN_VID);
		if (RT_ERR_OK != rc) {
			rtl_err(rc, "%s: Unable to set untag action to UNTAG_ASSIGN.", __func__);
			return -ENODATA;
		}
	}

	/* Apply 802.1ad VLANs */
	for (i = 0; i < RTL8370_NUM_SVLAN; i++) {
		if (!svlan->lut[i].member)
			continue;
		RTK_PORTMASK_CLEAR(svlan_cfg.untagport);
		RTK_PORTMASK_CLEAR(svlan_cfg.memberport);
		for (j = 0; j < priv->swdev.ports; j++) {
			if (!(svlan->lut[i].member & BIT(j)))
				continue;
			RTK_PORTMASK_PORT_SET(svlan_cfg.memberport, _rtl_port_p2l(priv, j));
			if (IS_SVLAN_PORT(j) || IS_SVLAN_ACL(j)) {
				RTK_PORTMASK_PORT_SET(svlan_cfg.untagport, _rtl_port_p2l(priv, j));
			}
		}

		svlan_cfg.efid = svlan->lut[i].efid;
		svlan_cfg.efiden = ENABLED;

		/* update SVLAN table*/
		rc = rtk_svlan_memberPortEntry_set(svlan->lut[i].id, &svlan_cfg);
		if (RT_ERR_OK != rc) {
			rtl_err(rc, "%s: Failed to set SVLAN %d.", __func__, svlan->lut[i].id);
			return -ENODATA;
		}
	}

	/* Enable VLAN ingress filter for VLAN aware ports, assign default SVLAN to all ports, add trunk */
	for (i = 0; i < priv->swdev.ports; i++) {
		rtk_enable_t enable = (IS_VLAN_AWARE(i) && !IS_SVLAN_TRUNK(i)) ? ENABLED : DISABLED;
		rc = rtk_vlan_portIgrFilterEnable_set(_rtl_port_p2l(priv, i), enable);
		if (RT_ERR_OK != rc) {
			rtl_err(rc, "%s: Failed to set VLAN ingress filter.", __func__);
			return -ENODATA;
		}

		if (svlan_port || svlan_acl) {
			if (IS_SVLAN_TRUNK(i)) {
				rc = rtk_svlan_servicePort_add(_rtl_port_p2l(priv, i));
				if (RT_ERR_OK != rc) {
					rtl_err(rc, "%s: Failed to add SVLAN trunk port.",
						__func__);
					return -ENODATA;
				}
			} else {
				rc = rtk_svlan_defaultSvlan_set(_rtl_port_p2l(priv, i),
								IS_SVLAN_PORT(i) ?
									svlan->default_svid[i] :
									RTL8370_DEFAULT_SVLAN_VID);
				if (RT_ERR_OK != rc) {
					rtl_err(rc, "%s: Failed to set default SVLAN for port %d",
						__func__, i);
					return -ENODATA;
				}
			}
		}
	}

	/* Enable/Disable egress VLAN filters */
	rc = rtk_vlan_egrFilterEnable_set((priv->vlan_enabled || svlan_port || svlan_acl) ? ENABLED : DISABLED);
	if (RT_ERR_OK != rc) {
		rtl_err(rc, "%s: Failed to set VLAN egress filter.", __func__);
		return -ENODATA;
	}

/* undef helpers */
#undef IS_SVLAN_TRUNK
#undef IS_SVLAN_PORT
#undef IS_VLAN_AWARE

	return 0;
}

/**
 * @brief Init L2 module
 *
 * @param priv - rtl8370's private structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_l2_init(struct rtl8370_priv *priv)
{
	rtk_api_ret_t rc;

	rc = rtk_l2_init();
	if (RT_ERR_OK != rc) {
		rtl_err(rc, "%s: Unable to init l2 module.", __func__);
		return -ENODEV;
	}

	return 0;
}

/**
 * @brief Init VLAN
 *
 * @param priv - rtl8370's private structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_vlan_init(struct rtl8370_priv *priv)
{
	rtk_api_ret_t rc;
	int i;

	memset(priv->vlan_table, 0, sizeof(priv->vlan_table));
	memset(priv->efid_table, 0, sizeof(priv->efid_table));
	memset(priv->pvid_table, 0, sizeof(priv->pvid_table));
	memset(&priv->svlan_table, 0, sizeof(priv->svlan_table));

	rc = rtk_vlan_reset();
	if (RT_ERR_OK != rc) {
		rtl_err(rc, "%s: Failed to reset VLAN table.", __func__);
		return -ENODATA;
	}

	/* enable VLAN for all edge ports by default - prevent leaking */
	priv->vlan_enabled = DISABLED;
	for(i = 0; i < priv->swdev.ports; i++) {
		if(_rtl_port_has_no_phy(priv, i))
			continue;

		priv->vlan_enabled |= BIT(i);
	}

	return rtl8370_vlan_apply(priv);
}

/**
 * @brief Init LEDs
 *
 * @param priv - rtl8370's private structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_led_init(struct rtl8370_priv *priv)
{
	rtk_api_ret_t rc;
	rtk_portmask_t port_mask;
	rtk_led_congig_t led_group[2];
	rtk_led_serialOutput_t serial_output;
	int i = 0;
	rtk_led_operation_t led_op = LED_OP_SERIAL;

	if (LED_PROFILE_OFF == priv->led_profile) {
		return 0;
	} /* else LED_PROFILE_DEFAULT_XXX */

	if (LED_PROFILE_DEFAULT_PARALLEL == priv->led_profile) {
		led_op = LED_OP_PARALLEL;
	}

	rc = rtk_led_operation_set(led_op);
	if (RT_ERR_OK != rc) {
		rtl_err(rc, "%s: Unable to set operation mode", __func__);
		return -ENODATA;
	}

	rc = rtk_led_OutputEnable_set(ENABLED);
	if (RT_ERR_OK != rc) {
		rtl_err(rc, "%s: Unable to set LED I/O state", __func__);
		return -ENODATA;
	}

	/* Default mapping */
	RTK_PORTMASK_CLEAR(port_mask);
	RTK_PORTMASK_PORT_SET(port_mask, UTP_PORT0);
	RTK_PORTMASK_PORT_SET(port_mask, UTP_PORT1);
	RTK_PORTMASK_PORT_SET(port_mask, UTP_PORT2);
	RTK_PORTMASK_PORT_SET(port_mask, UTP_PORT3);
	RTK_PORTMASK_PORT_SET(port_mask, UTP_PORT4);

	if(priv->port_map->id == CHIP_RTL8370B) {
		RTK_PORTMASK_PORT_SET(port_mask, UTP_PORT5);
		RTK_PORTMASK_PORT_SET(port_mask, UTP_PORT6);
		RTK_PORTMASK_PORT_SET(port_mask, UTP_PORT7);
	}

	/* only use LED group 0 & 1 */
	rc = rtk_led_enable_set(LED_GROUP_0, &port_mask);
	if (RT_ERR_OK != rc) {
		rtl_err(rc, "%s: Unable to enable port mask for led group 0", __func__);
		return -ENODATA;
	}

	if (LED_PROFILE_SINGLE_STATE != priv->led_profile) {
		rc = rtk_led_enable_set(LED_GROUP_1, &port_mask);
		if (RT_ERR_OK != rc) {
			rtl_err(rc, "%s: Unable to enable port mask for led group 1", __func__);
			return -ENODATA;
		}
	}

	if (LED_PROFILE_DEFAULT_SERIAL == priv->led_profile) {
		serial_output = SERIAL_LED_0_1;
		led_group[LED_GROUP_0] = LED_CONFIG_SPD1000ACT;
		led_group[LED_GROUP_1] = LED_CONFIG_SPD10010ACT;
	} else if (LED_PROFILE_SINGLE_STATE == priv->led_profile) {
		serial_output = SERIAL_LED_0;
		led_group[LED_GROUP_0] = LED_CONFIG_LINK_ACT;
	} else {
		serial_output = SERIAL_LED_NONE;
		led_group[LED_GROUP_0] = LED_CONFIG_SPD10010ACT;
		led_group[LED_GROUP_1] = LED_CONFIG_SPD1000ACT;
	}

	rc = rtk_led_serialModePortmask_set(serial_output, &port_mask);
	if (RT_ERR_OK != rc) {
		rtl_err(rc, "%s: Unable to set port mask for led group (%d)", __func__, serial_output);
		return -ENODATA;
	}

	for (i = 0; i < ARRAY_SIZE(led_group); ++i) {
		rc = rtk_led_groupConfig_set(i, led_group[i]);
		if (RT_ERR_OK != rc) {
			rtl_err(rc, "%s: Unable to set configuration for LED group %d", __func__, i);
			return -ENODATA;
		}
	}

	rc = rtk_led_serialMode_set(LED_ACTIVE_HIGH);
	if (RT_ERR_OK != rc) {
		rtl_err(rc, "%s: Unable to set serial mode configuration", __func__);
		return -ENODATA;
	}

	return 0;
}

/**
 * @brief Apply port isolation mask
 *
 * @param priv - rtl8370's private structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_port_isolation_apply(struct rtl8370_priv *priv)
{
	rtk_portmask_t port_mask;
	rtk_api_ret_t rc;
	int i;

	for (i = 0; i < priv->swdev.ports; ++i) {
		if(RTK_PORTMASK_IS_EMPTY(priv->port_isolation[i])) {
			RTK_PORTMASK_ALLPORT_SET(port_mask);
		} else {
			port_mask = priv->port_isolation[i];
		}
		rc = rtk_port_isolation_set(_rtl_port_p2l(priv, i), &port_mask);
		if (RT_ERR_OK != rc) {
			rtl_err(rc, "%s: Failed to set port isolation for port %d\n", __func__, i);
			return -ENODATA;
		}
	}

	return 0;
}

/**
 * @brief Initialization of ASIC submodules
 *
 * @param priv - rtl8370 private structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_asic_init(struct rtl8370_priv *priv)
{
	rtk_api_ret_t rc;

	/* clear port isolation masks */
	memset(priv->port_isolation, 0, sizeof(priv->port_isolation));

	/* Set default ARL age time */
	priv->arl_age_time = RTL8370_ARL_AGE_TIME_DEFAULT_S;

	if ((rc = ubnt_acl_flush(&priv->hw_acl))) {
		return rc;
	}

	if ((rc = rtl8370_vlan_init(priv))) {
		return rc;
	}

	return 0;
}

/**
 * @brief Initialization of PHY/MAC layer
 *
 * @param priv - rtl8370 private structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_phy_init(struct rtl8370_priv *priv)
{
	int i = 0;
	rtk_port_t cpu_port;
	rtk_api_ret_t rc;
	rtk_port_mac_ability_t mac_ability;

	/* NOTE: keep the order of initializing PHY & MAC */

	/* initialize PHY */
	rc = rtk_port_phyEnableAll_set(ENABLED);
	if (RT_ERR_OK != rc) {
		rtl_err(rc, "%s: Unable to enable PHYs", __func__);
		return -ENODEV;
	}

	/* initialize MAC */
	/* defaults for GMAC1, GMAC2 */
	mac_ability.forcemode = ENABLED;
	mac_ability.duplex = PORT_FULL_DUPLEX;
	mac_ability.link = PORT_LINKUP;
	mac_ability.txpause = DISABLED;
	mac_ability.rxpause = DISABLED;

	for (i = 0; i < priv->port_map->cpu_cnt; ++i) {
		mac_ability.speed = priv->mac_mode[i].speed;
		mac_ability.nway = priv->mac_mode[i].nway;
		cpu_port = _rtl_port_p2l(priv, priv->port_map->cpu[i]);

		pr_info("Setting %d to s %d m %d rx %d tx %d\n", i, mac_ability.speed,
			priv->mac_mode[i].mode, priv->mac_mode[i].delay_rx,
			priv->mac_mode[i].delay_tx);
		rc = rtk_port_macForceLinkExt_set(cpu_port, priv->mac_mode[i].mode, &mac_ability);
		if (RT_ERR_OK != rc) {
			rtl_err(rc, "%s: Unable to setup ext's MAC (CPU %d)", __func__, i);
			return -ENODEV;
		}

		if (MODE_EXT_RGMII == priv->mac_mode[i].mode) {
			rc = rtk_port_rgmiiDelayExt_set(cpu_port, priv->mac_mode[i].delay_tx,
							priv->mac_mode[i].delay_rx);
			if (RT_ERR_OK != rc) {
				rtl_err(rc, "%s: Unable to setup RGMII delay (CPU %d)", __func__, i);
				return -ENODEV;
			}
		} else if (MODE_EXT_SGMII == priv->mac_mode[i].mode ||
			   MODE_EXT_HSGMII == priv->mac_mode[i].mode) {
			rc = rtk_port_sgmiiNway_set(cpu_port, mac_ability.nway);
			if (RT_ERR_OK != rc) {
				rtl_err(rc, "%s: Unable to setup SGMII Nway (CPU %d)", __func__, i);
				return -ENODEV;
			}
		}
	}
	return 0;
}

/**
 * @brief RTL8370 initialization
 *
 * @param priv - rtl8370 private structure
 * @param hard_reset - perform hard reset
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_setup(struct rtl8370_priv *priv, bool hard_reset)
{
	rtk_api_ret_t rc;

	if (hard_reset) {
		rc = rtk_switch_hard_reset();
		if (RT_ERR_OK != rc) {
			rtl_err(rc, "%s: Failed to reset switch.", __func__);
			return -ENODATA;
		}
	}

	rc = rtk_switch_init();
	if (RT_ERR_OK != rc) {
		rtl_err(rc, "%s: Unable to setup RTL8370.", __func__);
		return -ENODEV;
	}

	if ((rc = ubnt_acl_init(&priv->hw_acl))) {
		return rc;
	}

	if ((rc = rtl8370_led_init(priv))) {
		return rc;
	}

	/*
	 * Realtek support suggested initializing PHY as the last thing.
	 * Unfortunately, when the chip is hard reset on init, PHY CAN'T
	 * be initialized after rtl8370_asic_init as reseting vlan using
	 * rtk_vlan_reset would brick the device (LEDs are not working
	 * properly, all frames on all ports are dropped).
	 *
	 * Keep the order of rtl8370_phy_init and rtl8370_asic_init.
	 */
	if ((rc = rtl8370_phy_init(priv))) {
		return rc;
	}

	if ((rc = rtl8370_asic_init(priv))) {
		return rc;
	}

	if((rc = rtl8370_l2_init(priv))){
		return rc;
	}

	return 0;
}

/**
 * @brief Reset all MIB counters
 *
 * @param dev - switch control structure
 * @param attr - switch attribute structure
 * @param val - switch value structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_sw_reset_mibs(struct switch_dev *dev, const struct switch_attr *attr,
				 struct switch_val *val)
{
	rtk_api_ret_t rc;

	rc = rtk_stat_global_reset();
	if (RT_ERR_OK != rc) {
		rtl_err(rc, "%s: Failed to reset all MIB counters.", __func__);
		return -ENODATA;
	}

	return 0;
}

/**
 * @brief Reset port's MIB counters
 *
 * @param dev - switch control structure
 * @param attr - switch attribute structure
 * @param val - switch value structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_sw_reset_port_mibs(struct switch_dev *dev, const struct switch_attr *attr,
				      struct switch_val *val)
{
	struct rtl8370_priv *priv = sw_to_rtl8370(dev);
	rtk_api_ret_t rc;

	if (val->port_vlan >= dev->ports) {
		return -EINVAL;
	}

	rc = rtk_stat_port_reset(_rtl_port_p2l(priv, val->port_vlan));
	if (RT_ERR_OK != rc) {
		rtl_err(rc, "%s: Failed to reset MIB counters for port %d.", __func__,
			val->port_vlan);
		return -ENODATA;
	}

	return 0;
}

/**
 * @brief Retrieve port's MIB counters
 *
 * @param dev - switch control structure
 * @param attr - switch attribute structure
 * @param val - switch value structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_sw_get_port_mib(struct switch_dev *dev, const struct switch_attr *attr,
				   struct switch_val *val)
{
	struct rtl8370_priv *priv = sw_to_rtl8370(dev);
	rtk_api_ret_t rc;
	rtk_stat_port_cntr_t port_counter;

	/** Output string */
	char *str = priv->buffer;
	uint32_t str_len = 0;
	uint32_t str_max = sizeof(priv->buffer);

	if (val->port_vlan >= dev->ports) {
		return -EINVAL;
	}

	rc = rtk_stat_port_getAll(_rtl_port_p2l(priv, val->port_vlan), &port_counter);
	if (RT_ERR_OK != rc) {
		rtl_err(rc, "%s: Failed to retrive stats for port %d.", __func__, val->port_vlan);
		return -ENODATA;
	}

	BUF_APPEND_PRINTF("Port %d MIB counters\n", val->port_vlan);

#define X(field, type) \
	BUF_APPEND_PRINTF("%-36s: %llu\n", #field, (rtk_stat_counter_t)port_counter.field);
	MIB_LIST_OF_STATS(X)
#undef X

	val->value.s = str;
	val->len = str_len;

	return 0;

error:
	return -ENOMEM;
}

/**
 * @brief Retrieve info about VLAN support status
 *
 * @param dev - switch control structure
 * @param attr - switch attribute structure
 * @param val - switch value structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_sw_get_vlan_enable(struct switch_dev *dev, const struct switch_attr *attr,
				      struct switch_val *val)
{
	struct rtl8370_priv *priv = sw_to_rtl8370(dev);
	int port = val->port_vlan;
	if (port >= dev->ports) {
		return -EINVAL;
	}

	val->value.i = !!(priv->vlan_enabled & BIT(port));

	return 0;
}

/**
 * @brief Enable/Disable VLAN support
 *
 * @param dev - switch control structure
 * @param attr - switch attribute structure
 * @param val - switch value structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_sw_set_vlan_enable(struct switch_dev *dev, const struct switch_attr *attr,
				      struct switch_val *val)
{
	struct rtl8370_priv *priv = sw_to_rtl8370(dev);
	int port = val->port_vlan;

	if (port >= dev->ports) {
		return -EINVAL;
	}

	if (val->value.i) {
		priv->vlan_enabled |= BIT(port);
	} else {
		priv->vlan_enabled &= ~BIT(port);
	}
	return 0;
}

/**
 * @brief Retrieve info about specific VLAN
 *
 * @param dev - switch control structure
 * @param attr - switch attribute structure
 * @param val - switch value structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_sw_get_vlan_info(struct switch_dev *dev, const struct switch_attr *attr,
				    struct switch_val *val)
{
	struct rtl8370_priv *priv = sw_to_rtl8370(dev);
	int i;
	/** Output string */
	char *str = priv->buffer;
	uint32_t str_len = 0;
	uint32_t str_max = sizeof(priv->buffer);

	str[0] = '\0';
	val->len = 0;
	val->value.s = str;

	if (rtl8370_vlan_valid(dev, val->port_vlan)) {
		return -EINVAL;
	}

	if (!priv->vlan_table[val->port_vlan].members) {
		return 0;
	}

	BUF_APPEND_PRINTF("VLAN %d: Ports: '", val->port_vlan);

	for (i = 0; i < dev->ports; ++i) {
		if (!(priv->vlan_table[val->port_vlan].members & BIT(i))) {
			continue;
		}

		BUF_APPEND_PRINTF("%d%s", i,
				  (priv->pvid_tagged & BIT(i) ||
				   val->port_vlan != priv->pvid_table[i]) ?
					  "t" :
					  "");
	}

	str[str_max - 1] = '\0';

	val->value.s = str;
	val->len = str_len;

	return 0;

error:
	return -ENOMEM;
}

/**
 * @brief Set VLAN's FID
 *
 * @param dev - switch control structure
 * @param attr - switch attribute structure
 * @param val - switch value structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_sw_set_vlan_fid(struct switch_dev *dev, const struct switch_attr *attr,
				   struct switch_val *val)
{
	struct rtl8370_priv *priv = sw_to_rtl8370(dev);

	if (rtl8370_vlan_valid(dev, val->port_vlan)) {
		return -EINVAL;
	}

	if (val->value.i < 0 || val->value.i > attr->max) {
		return -EINVAL;
	}

	priv->vlan_table[val->port_vlan].fid = val->value.i;

	return 0;
}

/**
 * @brief Retrieve VLAN's FID
 *
 * @param dev - switch control structure
 * @param attr - switch attribute structure
 * @param val - switch value structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_sw_get_vlan_fid(struct switch_dev *dev, const struct switch_attr *attr,
				   struct switch_val *val)
{
	struct rtl8370_priv *priv = sw_to_rtl8370(dev);

	if (rtl8370_vlan_valid(dev, val->port_vlan)) {
		return -EINVAL;
	}

	val->value.i = priv->vlan_table[val->port_vlan].fid;

	return 0;
}

/**
 * @brief Set IVL (Independent Vlan Learning)
 *
 * @param dev - switch control structure
 * @param attr - switch attribute structure
 * @param val - switch value structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_sw_set_ivl(struct switch_dev *dev, const struct switch_attr *attr,
		   struct switch_val *val)
{
	struct rtl8370_priv *priv = sw_to_rtl8370(dev);

	if (rtl8370_vlan_valid(dev, val->port_vlan)) {
		return -EINVAL;
	}

	if (val->value.i < 0 || val->value.i > attr->max) {
		return -EINVAL;
	}

	priv->vlan_table[val->port_vlan].ivl_enabled = !!val->value.i;

	return 0;
}

/**
 * @brief Get the state of IVL
 *
 * @param dev - switch control structure
 * @param attr - switch attribute structure
 * @param val - switch value structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_sw_get_ivl(struct switch_dev *dev, const struct switch_attr *attr,
		   struct switch_val *val)
{
	struct rtl8370_priv *priv = sw_to_rtl8370(dev);

	if (rtl8370_vlan_valid(dev, val->port_vlan)) {
		return -EINVAL;
	}

	val->value.i = priv->vlan_table[val->port_vlan].ivl_enabled;

	return 0;
}

/**
 * @brief Retrieve VLAN's port
 *
 * @param dev - switch control structure
 * @param val - switch value structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_sw_get_vlan_ports(struct switch_dev *dev, struct switch_val *val)
{
	struct rtl8370_priv *priv = sw_to_rtl8370(dev);
	uint16_t ports = priv->vlan_table[val->port_vlan].members;

	int i;
	val->len = 0;

	if (rtl8370_vlan_valid(dev, val->port_vlan)) {
		return -EINVAL;
	}

	for (i = 0; i < dev->ports; i++) {
		struct switch_port *p;

		if (!(ports & BIT(i)))
			continue;

		p = &val->value.ports[val->len++];
		p->id = i;
		p->flags = ((priv->pvid_tagged & BIT(i)) || val->port_vlan != priv->pvid_table[i]) ?
				   BIT(SWITCH_PORT_FLAG_TAGGED) :
				   0;
	}

	return 0;
}

/**
 * @brief Add ports to VLAN
 *
 * @param dev - switch control structure
 * @param val - switch value structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_sw_set_vlan_ports(struct switch_dev *dev, struct switch_val *val)
{
	struct rtl8370_priv *priv = sw_to_rtl8370(dev);
	struct switch_port *port;
	int i = 0;

	if (rtl8370_vlan_valid(dev, val->port_vlan)) {
		return -EINVAL;
	}

	priv->vlan_table[val->port_vlan].members = 0;
	port = &val->value.ports[0];

	for (i = 0; i < val->len; ++i, ++port) {

		if ((port->flags & BIT(SWITCH_PORT_FLAG_TAGGED)) &&
			(val->port_vlan == priv->pvid_table[port->id])) {
			priv->pvid_tagged |= BIT(port->id);
		} else {
			priv->pvid_tagged &= ~BIT(port->id);
		}
		if (!(port->flags & BIT(SWITCH_PORT_FLAG_TAGGED))) {
			/* Update the pvid to the vid we are untagging */
			priv->pvid_table[port->id] = val->port_vlan;
		}
		priv->vlan_table[val->port_vlan].members |= BIT(port->id);
	}

	return 0;
}

/**
 * @brief Get entry from SVLAN table
 * @param priv - rtl8370's private structure
 * @param id - SVLAN id
 * @return pointer to entry in SVLAN table, NULL when no entry is found
 */
static inline struct rtl8370_svlan_entry *rtl8370_sw_svlan_table_get(struct rtl8370_priv *priv,
								     uint16_t id)
{
	struct rtl8370_svlan_table *svlan = &priv->svlan_table;
	int i;
	for (i = 0; i < RTL8370_NUM_SVLAN; ++i) {
		if (svlan->lut[i].id == id) {
			return &svlan->lut[i];
		}
	}

	return NULL;
}

/**
 * @brief Update/Insert SVLAN table entry
 *
 * @param priv - rtl8370's private structure
 * @param entry - SVLAN entry
 * @return int - error from errno.h, 0 on success
 */
static inline int rtl8370_sw_svlan_table_set(struct rtl8370_priv *priv,
					     struct rtl8370_svlan_entry *entry)
{
	struct rtl8370_svlan_table *svlan = &priv->svlan_table;
	int idx_first_empty = -1;
	int i;

	for (i = 0; i < RTL8370_NUM_SVLAN; ++i) {
		if (svlan->lut[i].member) {
			if (svlan->lut[i].id == entry->id) {
				/* update existing entry */
				svlan->lut[i] = *entry;
				if (!entry->member) {
					/* delete */
					svlan->lut_cnt--;
				}
				return 0;
			}
		} else if (idx_first_empty < 0) {
			idx_first_empty = i;
		}
	}

	if (idx_first_empty < 0) {
		/* no free space */
		rtl_warn("%s: can't insert SVLAN entry, number of entries is limited to %d.",
			__func__, RTL8370_NUM_SVLAN);
		return -EINVAL;
	}

	if (entry->member) {
		/* insert new */
		svlan->lut[idx_first_empty] = *entry;
		svlan->lut_cnt++;
	}

	return 0;
}

/**
 * @brief Get SVLAN EFID
 *
 * @param dev - switch control structure
 * @param attr - switch attribute structure
 * @param val - switch value structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_sw_set_svlan_efid(struct switch_dev *dev, const struct switch_attr *attr,
				   struct switch_val *val)
{
	struct rtl8370_priv *priv = sw_to_rtl8370(dev);
	struct rtl8370_svlan_entry *entry = NULL;

	int i;
	val->len = 0;

	if (rtl8370_svlan_valid(val->port_vlan)) {
		return -EINVAL;
	}

	if (val->value.i < 0 || val->value.i > attr->max) {
		return -EINVAL;
	}

	if ((entry = rtl8370_sw_svlan_table_get(priv, val->port_vlan))) {
		entry->efid = val->value.i;

		for (i = 0; i < dev->ports; i++) {
			if (!(entry->member & BIT(i)))
				continue;

			priv->efid_table[i] = entry->efid;
		}
		return 0;
	}

	return -EINVAL;
}

/**
 * @brief Retrieve SVLAN EFID
 *
 * @param dev - switch control structure
 * @param attr - switch attribute structure
 * @param val - switch value structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_sw_get_svlan_efid(struct switch_dev *dev, const struct switch_attr *attr,
				   struct switch_val *val)
{
	struct rtl8370_priv *priv = sw_to_rtl8370(dev);
	struct rtl8370_svlan_entry *entry = NULL;

	if (rtl8370_svlan_valid(val->port_vlan)) {
		return -EINVAL;
	}

	if ((entry = rtl8370_sw_svlan_table_get(priv, val->port_vlan))) {
		val->value.i  = entry->efid;
		return 0;
	}

	return -EINVAL;
}

/**
 * @brief Retrieve SVLAN's members
 *
 * @param dev - switch control structure
 * @param val - switch value structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_sw_get_svlan_ports(struct switch_dev *dev, struct switch_val *val)
{
	struct rtl8370_priv *priv = sw_to_rtl8370(dev);
	struct rtl8370_svlan_entry *entry = NULL;

	int i;
	val->len = 0;

	if (rtl8370_svlan_valid(val->port_vlan)) {
		return -EINVAL;
	}

	if ((entry = rtl8370_sw_svlan_table_get(priv, val->port_vlan))) {
		for (i = 0; i < dev->ports; i++) {
			struct switch_port *p;

			if (!(entry->member & BIT(i)))
				continue;

			p = &val->value.ports[val->len++];
			p->id = i;
			p->flags = (entry->trunk & BIT(i)) ? BIT(SWITCH_PORT_FLAG_TAGGED) : 0;
		}
	}

	return 0;
}

/**
 * @brief Add ports to SVLAN
 *
 * @param dev - switch control structure
 * @param val - switch value structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_sw_set_svlan_ports(struct switch_dev *dev, struct switch_val *val)
{
	struct rtl8370_priv *priv = sw_to_rtl8370(dev);
	struct rtl8370_svlan_entry entry = { 0 };
	struct switch_port *port;

	int i = 0, rc;

	if (rtl8370_svlan_valid(val->port_vlan)) {
		return -EINVAL;
	}
	entry.id = val->port_vlan;

	/* Validate trunk mask */
	for (i = 0, port = &val->value.ports[0]; i < val->len; ++i, ++port) {
		entry.member |= BIT(port->id);
		if (port->flags & BIT(SWITCH_PORT_FLAG_TAGGED)) {
			entry.trunk |= BIT(port->id);
		}
	}

	rc = rtl8370_sw_svlan_table_set(priv, &entry);
	if(rc) {
		rtl_warn("%s: can't insert SVLAN entry.", __func__);
		return rc;
	}

	for (i = 0, port = &val->value.ports[0]; i < val->len; ++i, ++port) {
		if (!(port->flags & BIT(SWITCH_PORT_FLAG_TAGGED))) {
			priv->svlan_table.default_svid[port->id] = val->port_vlan;
		}
	}

	return 0;
}

/**
 * @brief Retrieve port isolation members
 *
 * @param dev - switch control structure
 * @param attr - switch attribute structure
 * @param val - switch value structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_sw_get_port_isolation(struct switch_dev *dev, const struct switch_attr *attr,
					 struct switch_val *val)
{
	struct rtl8370_priv *priv = sw_to_rtl8370(dev);
	struct switch_port *p_it;
	rtk_portmask_t *port_members;
	int i, port = val->port_vlan;

	if (port >= dev->ports) {
		return -EINVAL;
	}

	port_members = &priv->port_isolation[port];

	for (i = 0; i < dev->ports; ++i) {
		if (!RTK_PORTMASK_IS_PORT_SET(*port_members, _rtl_port_p2l(priv, i))) {
			continue;
		}
		p_it = &val->value.ports[val->len++];
		p_it->id = i;
	}

	return 0;
}

/**
 * @brief Set port isolation members
 *
 * @param dev - switch control structure
 * @param attr - switch attribute structure
 * @param val - switch value structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_sw_set_port_isolation(struct switch_dev *dev, const struct switch_attr *attr,
					 struct switch_val *val)
{
	struct rtl8370_priv *priv = sw_to_rtl8370(dev);
	struct switch_port *p_it;
	rtk_portmask_t *port_members;
	int i, port = val->port_vlan;

	if (port >= dev->ports) {
		return -EINVAL;
	}

	port_members = &priv->port_isolation[port];

	RTK_PORTMASK_CLEAR(*port_members);

	for (i = 0; i < val->len; ++i) {
		p_it = &val->value.ports[i];
		RTK_PORTMASK_PORT_SET(*port_members, _rtl_port_p2l(priv, p_it->id));
	}

	return 0;
}

/**
 * @brief Retrieve port's EFID
 *
 * @param dev - switch control structure
 * @param attr - switch attribute structure
 * @param val - switch value structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_sw_get_port_efid(struct switch_dev *dev, const struct switch_attr *attr,
					 struct switch_val *val)
{
	struct rtl8370_priv *priv = sw_to_rtl8370(dev);

	if (val->port_vlan >= dev->ports) {
		return -EINVAL;
	}

	val->value.i = priv->efid_table[val->port_vlan];

	return 0;
}

/**
 * @brief Set port's EFID
 *
 * @param dev - switch control structure
 * @param attr - switch attribute structure
 * @param val - switch value structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_sw_set_port_efid(struct switch_dev *dev, const struct switch_attr *attr,
				      struct switch_val *val)
{
	struct rtl8370_priv *priv = sw_to_rtl8370(dev);

	if (val->port_vlan >= dev->ports) {
		return -EINVAL;
	}

	if (val->value.i < 0 || val->value.i > attr->max) {
		return -EINVAL;
	}

	priv->efid_table[val->port_vlan] = val->value.i;

	return 0;
}

/**
 * @brief Retrieve port's VID
 *
 * @param dev - switch control structure
 * @param port - port number (physical)
 * @param val - port's vid
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_sw_get_port_pvid(struct switch_dev *dev, int port, int *val)
{
	struct rtl8370_priv *priv = sw_to_rtl8370(dev);

	if (port >= dev->ports) {
		return -EINVAL;
	}

	*val = priv->pvid_table[port];

	return 0;
}

/**
 * @brief Set port's VID
 *
 * @param dev - switch control structure
 * @param port - port number (physical)
 * @param val - port's vid
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_sw_set_port_pvid(struct switch_dev *dev, int port, int pvid)
{
	struct rtl8370_priv *priv = sw_to_rtl8370(dev);

	if (port >= dev->ports) {
		return -EINVAL;
	}

	priv->pvid_table[port] = pvid;

	return 0;
}

/**
 * @brief Hard reset
 *
 * @param dev - switch control structure
 * @param attr - switch attribute structure
 * @param val - switch value structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_hw_reset_switch(struct switch_dev *dev, const struct switch_attr *attr,
				 struct switch_val *val)
{
	struct rtl8370_priv *priv = sw_to_rtl8370(dev);

	/* Setup switch */
	return rtl8370_setup(priv, true);
}

/**
 * @brief Switch SW reset
 *
 * @param dev - switch control structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_sw_reset_switch(struct switch_dev *dev)
{
	/* Setup switch */
	return rtl8370_asic_init(sw_to_rtl8370(dev));
}

/**
 * @brief Write data to PHY
 *
 * @param dev - switch control structure
 * @param addr - port number (physical)
 * @param reg - PHY register
 * @param value - data to write
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_sw_phy_write16(struct switch_dev *dev, int port, uint8_t reg, uint16_t value)
{
	struct rtl8370_priv *priv = sw_to_rtl8370(dev);
	rtk_api_ret_t rc;

	rc = rtk_port_phyReg_set(_rtl_port_p2l(priv, port), reg, value);
	if (RT_ERR_OK != rc) {
		rtl_err(rc, "%s: Unable to write into port's PHY register", __func__);
		return -ENODATA;
	}
	return 0;
}

/**
 * @brief Set a port link status
 *
 * @param dev - switch control structure
 * @param port - port number (physical)
 * @param link - switch port link structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_sw_set_port_link(struct switch_dev *sw_dev, int port,
				    struct switch_port_link *link)
{
	struct rtl8370_priv *priv = sw_to_rtl8370(sw_dev);
	rtk_port_mac_ability_t mac_status;
	rtk_api_ret_t rc;

	/* don't allow to control MAC of CPU ports */
	if (_rtl_port_has_no_phy(priv, port)) {
		return -ENOTSUPP;
	}

	/* Handle PHY */
	if ((rc = switch_generic_set_link(sw_dev, port, link))) {
		return rc;
	}

	/* Handle MAC */
	memset(&mac_status, 0, sizeof(mac_status));
#ifdef CONFIG_SWCONFIG_UBNT_EXT
	if (!link->power_down)
#endif
	{
		mac_status.link = PORT_LINKUP;
		if (link->aneg) {
			mac_status.nway = ENABLED;
		} else {
			switch (link->speed) {
			case SWITCH_PORT_SPEED_10:
				mac_status.speed = PORT_SPEED_10M;
				break;
			case SWITCH_PORT_SPEED_100:
				mac_status.speed = PORT_SPEED_100M;
				break;
			case SWITCH_PORT_SPEED_1000:
			default:
				mac_status.speed = PORT_SPEED_1000M;
				break;
			}
			mac_status.nway = DISABLED;
			mac_status.duplex = link->duplex;
			mac_status.txpause = link->tx_flow;
			mac_status.rxpause = link->rx_flow;
		}
	}

	rc = rtk_port_macForceLink_set(_rtl_port_p2l(priv, port), &mac_status);
	if (RT_ERR_OK != rc) {
		rtl_err(rc, "%s: Unable to set port's MAC register", __func__);
		return -ENODATA;
	}

	return 0;
}

/**
 * @brief Retrive a port link status
 *
 * @param dev - switch control structure
 * @param port - port number (physical)
 * @param link - switch port link structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_sw_get_port_link(struct switch_dev *dev, int port, struct switch_port_link *link)
{
	struct rtl8370_priv *priv = sw_to_rtl8370(dev);
	rtk_api_ret_t rc;
	rtk_port_mac_ability_t mac_status;

	if (port >= dev->ports) {
		return -EINVAL;
	}

#ifdef CONFIG_SWCONFIG_UBNT_EXT
	if (!_rtl_port_has_no_phy(priv, port)) {
		rtk_port_phy_data_t phy_status;
		rc = rtk_port_phyReg_get(_rtl_port_p2l(priv, port), MII_BMCR, &phy_status);
		if (RT_ERR_OK != rc) {
			rtl_err(rc, "%s: Unable to read port's PHY control register", __func__);
			return -ENODATA;
		}
		link->power_down = !!(phy_status & BMCR_PDOWN);
	} else {
		link->power_down = 0;
	}
#endif

	rc = rtk_port_macStatus_get(_rtl_port_p2l(priv, port), &mac_status);
	if (RT_ERR_OK != rc) {
		rtl_err(rc, "%s: Unable to get port's MAC status.", __func__);
		return -ENODATA;
	}

	link->link = (PORT_LINKUP == mac_status.link);

	if (!link->link) {
		return 0;
	}

	link->duplex = mac_status.duplex;
	link->speed = rtl8370_tr_port_speed_switch(mac_status.speed);
	link->rx_flow = mac_status.rxpause;
	link->tx_flow = mac_status.txpause;
	link->aneg = mac_status.nway;

	return 0;
}

/**
 * @brief Get a port statistics counters
 *
 * @param dev - switch control structure
 * @param port - port number (physical)
 * @param stats - switch stats structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_sw_get_port_stats(struct switch_dev *dev, int port,
				     struct switch_port_stats *stats)
{
	struct rtl8370_priv *priv = sw_to_rtl8370(dev);
	rtk_stat_counter_t cntr;
	rtk_api_ret_t rc;

	if (port >= dev->ports) {
		return -EINVAL;
	}

	/* Retrive RX octet counter */
	rc = rtk_stat_port_get(_rtl_port_p2l(priv, port), STAT_IfInOctets, &cntr);
	if (RT_ERR_OK != rc) {
		rtl_err(rc, "%s: Unable to get port RX PHY stat.", __func__);
		return -ENODATA;
	}
	stats->rx_bytes = cntr;

	/* Retrive TX octet counter */
	rc = rtk_stat_port_get(_rtl_port_p2l(priv, port), STAT_IfOutOctets, &cntr);
	if (RT_ERR_OK != rc) {
		rtl_err(rc, "%s: Unable to get port TX PHY stat.", __func__);
		return -ENODATA;
	}
	stats->tx_bytes = cntr;

	return 0;
}

/**
 * @brief Convert the SW structure of ACL rule into the HW structure
 *
 * @param hw - platform dependent ACL control structure
 * @param entry SW representation of ACL rule
 * @param data_out - HW representation of ACL rule
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_acl_rule_sw_to_hw(struct acl_hw *hw, acl_entry_t *entry,
				    void *data_out)
{
	struct rtl8370_priv *priv = acl_hw_to_rtl8370_priv(hw);
	struct rtl8370_svlan_table *svlan = &priv->svlan_table;
	rtk_filter_action_t *act;
	rtk_filter_cfg_t *cfg;
	rtk_filter_field_t *fld;
	rtk_api_ret_t rc;
	int i, j;
	rt8370_acl_entry_hw_t *entry_hw = data_out;

	if (NULL == entry_hw) {
		return -EINVAL;
	}

	act = &entry_hw->in.act;
	cfg = &entry_hw->in.cfg;
	fld = &entry_hw->in.fld;

	memset(entry_hw, 0, sizeof(*entry_hw));

	switch(entry->type) {
		/*
		* NOTE : In our case there is harcoded action for ACL_RULE_PORT_REDIRECTION type which is
		* redirect incoming frame from port_src with SA to port_dst.
		*/
		case ACL_RULE_PORT_REDIRECTION:
			/* Set pattern */
			fld->fieldType = FILTER_FIELD_SMAC;
			if (ubnt_mac_zero_addr(&entry->mac_sa)) {
				fld->filter_pattern_union.smac.dataType = FILTER_FIELD_DATA_MASK;
				memcpy(fld->filter_pattern_union.smac.value.octet, entry->mac_sa.uc,
					sizeof(fld->filter_pattern_union.smac.value.octet));
				memset(fld->filter_pattern_union.smac.mask.octet, 0xFF,
					sizeof(fld->filter_pattern_union.smac.value.octet));
			}
			fld->next = NULL;

			rc = rtk_filter_igrAcl_field_add(cfg, fld);
			if (RT_ERR_OK != rc) {
				rtl_err(rc, "%s: Unable to add pattern field to the ACL list", __func__);
				return -ENODATA;
			}

			/* Destination port */
			act->actEnable[FILTER_ENACT_REDIRECT] = TRUE;
			RTK_PORTMASK_ALLPORT_SET(cfg->activeport.mask);
			cfg->invert = FALSE;

			for (i = 0; i < hw->max_ports; ++i) {
				/* Destination port */
				if (entry->port_dst & BIT(i)) {
					RTK_PORTMASK_PORT_SET(act->filterPortmask, _rtl_port_p2l(priv, i));
				}
				/* Source port */
				if (entry->port_src & BIT(i)) {
					RTK_PORTMASK_PORT_SET(cfg->activeport.value, _rtl_port_p2l(priv, i));
				}
			}
			break;

		case ACL_RULE_VLAN_ASSIGNMENT:
			fld->fieldType = FILTER_FIELD_ETHERTYPE;
			fld->filter_pattern_union.etherType.dataType = FILTER_FIELD_DATA_MASK;
			fld->filter_pattern_union.etherType.value = entry->ether_type;
			fld->filter_pattern_union.etherType.mask = 0xffff;
			fld->next = NULL;

			rc = rtk_filter_igrAcl_field_add(cfg, fld);
			if (RT_ERR_OK != rc) {
				rtl_err(rc, "%s: Unable to add pattern field to the ACL list", __func__);
				return -ENODATA;
			}

			/* ! Forwarding action must be defined as well ! */
			act->actEnable[FILTER_ENACT_REDIRECT] = TRUE;
			act->actEnable[FILTER_ENACT_SVLAN_EGRESS] = TRUE;
			for (i = 0; i < RTL8370_NUM_SVLAN; ++i) {
				if (svlan->lut[i].id == entry->svid) {
					act->filterSvlanVid = entry->svid;
					RTK_PORTMASK_ALLPORT_SET(cfg->activeport.mask);
					RTK_PORTMASK_CLEAR(act->filterPortmask);
					for (j = 0; j < hw->max_ports; ++j) {
						if (svlan->lut[i].trunk & BIT(j))
							RTK_PORTMASK_PORT_SET(act->filterPortmask, _rtl_port_p2l(priv, j));
						else if (svlan->lut[i].member & BIT(j))
							RTK_PORTMASK_PORT_SET(cfg->activeport.value, _rtl_port_p2l(priv, j));
					}
				}
			}

			if(!act->filterSvlanVid) {
				return -ENODATA;
			}
			cfg->invert = FALSE;
			break;
		default:
			return -EOPNOTSUPP;
	}
	return 0;
}

/**
 * @brief Flush the switch chip's ACL HW
 *
 * @param hw - platform dependent ACL control structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_acl_hw_flush(struct acl_hw *hw)
{
	rtk_api_ret_t rc;

	rc = rtk_filter_igrAcl_cfg_delAll();
	if (RT_ERR_OK != rc) {
		rtl_err(rc, "%s: Unable to flush ACL list from ASIC", __func__);
		return -ENODATA;
	}
	return 0;
}

/**
 * @brief Enable ACL module
 *
 * @param hw - platform dependent ACL control structure
 * @param enable - ACL status
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_acl_hw_enable(struct acl_hw *hw, uint8_t enable)
{
	struct rtl8370_priv *priv = acl_hw_to_rtl8370_priv(hw);
	rtk_api_ret_t rc;
	int i = 0;

	/* Enable ACL module */
	for (i = 0; i < hw->max_ports; ++i) {

		rc = rtk_filter_igrAcl_state_set(_rtl_port_p2l(priv, i), enable ? ENABLED : DISABLED);
		if (RT_ERR_OK != rc) {
			rtl_err(rc, "%s: Failed to enable the ACL for port %d.\n", __func__, i);
			return -EIO;
		}

		rc = rtk_filter_igrAcl_unmatchAction_set(_rtl_port_p2l(priv, i), FILTER_UNMATCH_PERMIT);
		if (RT_ERR_OK != rc) {
			rtl_err(rc, "%s: Failed to set unmatch action of ACL.", __func__);
			return -EIO;
		}
	}

	return 0;
}

/**
 * @brief Remove the ACL rule from the ACL list
 *
 * @param hw - platform dependent ACL control structure
 * @param idx - rule ID
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_acl_hw_rule_delete(struct acl_hw *hw, int idx)
{
	rtk_api_ret_t rc = 0;

	rc = rtk_filter_igrAcl_cfg_del(idx);
	if (RT_ERR_OK != rc) {
		rtl_err(rc, "%s: Failed to delete an ACL configuration from ASIC.", __func__);
		return -EIO;
	}

	return 0;
}

/**
 * @brief Add an ACL rule to the ACL list
 *
 * @param hw - platform dependent ACL control structure
 * @param idx - rule ID
 * @param data_in - HW representation of ACL rule
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_acl_hw_rule_set(struct acl_hw *hw, int idx, void *data_in)
{
	rtk_api_ret_t rc = 0;
	rtk_filter_number_t	rule_num;
	rt8370_acl_entry_hw_t *entry_hw = data_in;

	if(NULL == entry_hw){
		return -EINVAL;
	}

	rc = rtk_filter_igrAcl_cfg_add(idx, &entry_hw->in.cfg, &entry_hw->in.act, &rule_num);
	if (RT_ERR_OK != rc) {
		rtl_err(rc, "%s: Failed to add the ACL configuration to ASIC.", __func__);
		return -EIO;
	}

	return 0;
}

/**
 * @brief Init HW ACL
 *
 * @param hw - platform dependent ACL control structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_acl_hw_init(struct acl_hw *hw)
{
	rtk_api_ret_t rc = 0;

	if (NULL == hw) {
		return -EINVAL;
	}

	rc = rtk_filter_igrAcl_init();
	if (RT_ERR_OK != rc) {
		rtl_err(rc, "%s: Failed to init the ACL.", __func__);
		return -EIO;
	}
	return 0;
}

/**
 * @brief Allocate platform dependent data structure
 *
 * @param hw - platform dependent ACL control structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_acl_hw_alloc(struct acl_hw *hw)
{
	if (NULL == hw) {
		return -EINVAL;
	}
	hw->data = kzalloc(sizeof(rt8370_acl_entry_hw_t), GFP_KERNEL);
	if (NULL == hw->data) {
		return -ENOMEM;
	}
	return 0;
}

/**
 * @brief Destroy platform dependent data structure
 *
 * @param hw - platform dependent ACL control structure
 */
static void rtl8370_acl_hw_destroy(struct acl_hw *hw)
{
	if(hw->data){
		kfree(hw->data);
	}
}

/**
 * @brief Add MAC redirect ACL rule for src port
 *
 * @param dev - switch control structure
 * @param attr - switch attribute structure
 * @param val - switch value structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_sw_set_acl_redirect(struct switch_dev *dev, const struct switch_attr *attr,
					   struct switch_val *val)
{
	struct rtl8370_priv *priv = sw_to_rtl8370(dev);
	return ubnt_acl_rule_process(&priv->hw_acl, val->value.s, ACL_RULE_PORT_REDIRECTION);
}

/**
 * @brief Assign SVLAN/VLAN to a specific traffic
 *
 * @param dev - switch control structure
 * @param attr - switch attribute structure
 * @param val - switch value structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_sw_set_acl_vlan_assign(struct switch_dev *dev, const struct switch_attr *attr,
					   struct switch_val *val)
{
	struct rtl8370_priv *priv = sw_to_rtl8370(dev);
	return ubnt_acl_rule_process(&priv->hw_acl, val->value.s, ACL_RULE_VLAN_ASSIGNMENT);
}

/**
 * @brief Get ACL table
 *
 * @param dev - switch control structure
 * @param attr - switch attribute structure
 * @param val - switch value structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_sw_get_acl_table(struct switch_dev *dev, const struct switch_attr *attr,
				   struct switch_val *val)
{
	struct rtl8370_priv *priv = sw_to_rtl8370(dev);
	int rc = 0;

	if ((rc = ubnt_acl_get_acl_table(&priv->hw_acl))) {
		return rc;
	}

	val->value.s = priv->hw_acl.str;
	val->len = priv->hw_acl.str_len;

	return 0;
}

/**
 * @brief Flush ACL list
 *
 * @param dev - switch control structure
 * @param attr - switch attribute structure
 * @param val - switch value structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_sw_flush_acl_table(struct switch_dev *dev, const struct switch_attr *attr,
				       struct switch_val *val)
{
	struct rtl8370_priv *priv = sw_to_rtl8370(dev);
	return ubnt_acl_flush(&priv->hw_acl);
}

/**
 * @brief Enable the ACL module
 *
 * @param dev - switch control structure
 * @param attr - switch attribute structure
 * @param val - switch value structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_sw_set_acl(struct switch_dev *dev, const struct switch_attr *attr,
		   struct switch_val *val)
{
	struct rtl8370_priv *priv = sw_to_rtl8370(dev);
	return 	ubnt_acl_enable_set(&priv->hw_acl, val->value.i);
}

/**
 * @brief Retrive the status of the ACL module
 *
 * @param dev - switch control structure
 * @param attr - switch attribute structure
 * @param val - switch value structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_sw_get_acl(struct switch_dev *dev, const struct switch_attr *attr,
		   struct switch_val *val)
{
	int enable, rc;
	struct rtl8370_priv *priv = sw_to_rtl8370(dev);

	rc = ubnt_acl_enable_get(&priv->hw_acl, &enable);
	if (rc) {
		return -EINVAL;
	}
	val->value.i = enable;
	return 0;
}

/**
 * @brief Set ARL age time
 *
 * @param dev - switch control structure
 * @param attr - switch attribute structure
 * @param val - switch value structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_sw_set_arl_age_time(struct switch_dev *dev, const struct switch_attr *attr,
				       struct switch_val *val)
{
	struct rtl8370_priv *priv = sw_to_rtl8370(dev);
	int age_time = val->value.i;

	if (age_time <= 0 || age_time > RTL8370_ARL_AGE_TIME_MAX_S) {
		return -EINVAL;
	}

	priv->arl_age_time = age_time;
	return 0;
}

/**
 * @brief Get ARL age time
 *
 * @param dev - switch control structure
 * @param attr - switch attribute structure
 * @param val - switch value structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_sw_get_arl_age_time(struct switch_dev *dev, const struct switch_attr *attr,
				       struct switch_val *val)
{
	struct rtl8370_priv *priv = sw_to_rtl8370(dev);
	val->value.i = priv->arl_age_time;
	return 0;
}

/**
 * @brief Flush ARL table
 *
 * @param dev - switch control structure
 * @param attr - switch attribute structure
 * @param val - switch value structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_sw_set_flush_arl_table(struct switch_dev *dev, const struct switch_attr *attr,
					  struct switch_val *val)
{
	rtk_api_ret_t rc;

	/* Set ARL aging time */
	rc = rtk_l2_table_clear();
	if(rc) {
		rtl_err(rc, "%s: Failed to flush ARL table.", __func__);
		return -EIO;
	}

	return 0;
}

/**
 * @brief Get ARL table
 *
 * @param dev - switch control structure
 * @param attr - switch attribute structure
 * @param val - switch value structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_sw_get_arl_table(struct switch_dev *dev, const struct switch_attr *attr,
				   struct switch_val *val)
{
	struct rtl8370_priv *priv = sw_to_rtl8370(dev);
	rtk_uint32 i;
	rtk_uint32 address = 0;
	rtk_l2_ucastAddr_t l2_data;
	rtk_l2_ipMcastAddr_t ipMcastAddr;

	/** Output string */
	char *str = priv->buffer;
	uint32_t str_len = 0;
	uint32_t str_max = sizeof(priv->buffer);

	BUF_APPEND_PRINTF("\nhash  port(0:18)      fid   vid  mac-address    ivl  efid\n");
	while (RT_ERR_OK == rtk_l2_addr_next_get(READMETHOD_NEXT_L2UC, UTP_PORT0, &address, &l2_data)) {
		BUF_APPEND_PRINTF("%03x   ", l2_data.address);
		for (i = 0; i < priv->swdev.ports; i++) {
			BUF_APPEND_PRINTF("%c", (_rtl_port_l2p(priv, l2_data.port) == i ) ? '1' : '-');
		}

		BUF_APPEND_PRINTF("      %2d", l2_data.fid);
		BUF_APPEND_PRINTF("  %4d", l2_data.cvid);
		BUF_APPEND_PRINTF("  %02x%02x%02x%02x%02x%02x", l2_data.mac.octet[0],
		l2_data.mac.octet[1], l2_data.mac.octet[2], l2_data.mac.octet[3],
		l2_data.mac.octet[4], l2_data.mac.octet[5]);
		BUF_APPEND_PRINTF("  %4d", l2_data.ivl);
		BUF_APPEND_PRINTF("  %4d\n", l2_data.efid);
		address++;
	}

	BUF_APPEND_PRINTF("\n");

	address = 0;
	while (RT_ERR_OK == rtk_l2_ipMcastAddr_next_get(&address, &ipMcastAddr)) {
		BUF_APPEND_PRINTF("%03x   ", ipMcastAddr.address);
		for (i = 0; i < priv->swdev.ports; i++) {
			BUF_APPEND_PRINTF("%c", (RTK_PORTMASK_IS_PORT_SET(ipMcastAddr.portmask, _rtl_port_l2p(priv, i)) ? '1' : '-'));
		}
		BUF_APPEND_PRINTF("                ");
		BUF_APPEND_PRINTF("01005E%06x\n", (ipMcastAddr.dip & 0xefffff));
		address++;
	}

	val->value.s = str;
	val->len = str_len;

	return 0;
error:
	return -ENOMEM;
}

#ifdef CONFIG_SWCONFIG_UBNT_EXT

/**
 * @note
 * These are counters of rtl83xx we need to calculate uext mib counters.
 */
#define UEXT_RTL83XX_MIBS(COUNTER)  \
	COUNTER(IfInOctets)         \
	COUNTER(IfOutOctets)        \
	COUNTER(IfInUcastPkts)      \
	COUNTER(IfOutUcastPkts)     \
	COUNTER(IfInBroadcastPkts)  \
	COUNTER(IfOutBroadcastPkts) \
	COUNTER(IfInMulticastPkts)  \
	COUNTER(IfOutMulticastPkts) \
	COUNTER(Dot3InPauseFrames)  \
	COUNTER(Dot3OutPauseFrames) \
	COUNTER(EtherOversizeStats) \
	COUNTER(Dot3StatsFCSErrors) \
	COUNTER(Dot3StatsSymbolErrors) \
	COUNTER(EtherStatsFragments) \
	COUNTER(EtherStatsJabbers) \
	COUNTER(EtherStatsDropEvents) \
	COUNTER(Dot3StatsExcessiveCollisions) \
	COUNTER(Dot1dTpPortInDiscards) \
	COUNTER(InMldChecksumError) \
	COUNTER(InIgmpChecksumError) \
	COUNTER(InReportSuppressionDrop) \
	COUNTER(InLeaveSuppressionDrop)

/**
 * @brief Get MIB counter for port (swconfig ubnt extension)
 *
 * @param dev - switch control structure
 * @param mib - mib counter
 * @param port - port number
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_uext_get_port_mib(struct switch_dev *dev, struct switch_port_mib *mib, int port)
{
#define ENUM(x) UEXT_RTL83XX_MIB_##x
#define MIB(x) cnt[ENUM(x)]
	enum {
#define COUNTER(_enum) ENUM(_enum),
		UEXT_RTL83XX_MIBS(COUNTER)
#undef COUNTER
		UEXT_RTL83XX_MIB_COUNT
	};

	struct rtl8370_priv *priv = sw_to_rtl8370(dev);
	rtk_api_ret_t rc;
	int i = 0;
	/* Prefill the array with counter's IDs */
	rtk_stat_counter_t cnt[UEXT_RTL83XX_MIB_COUNT] = {
#define COUNTER(_enum) STAT_##_enum,
		UEXT_RTL83XX_MIBS(COUNTER)
#undef COUNTER
	};

	if (NULL == dev || NULL == priv || NULL == mib) {
		return -EINVAL;
	}

	if (port >= dev->ports) {
		return -EINVAL;
	}

	for (i = 0; i < UEXT_RTL83XX_MIB_COUNT; ++i) {
		rc = rtk_stat_port_get(_rtl_port_p2l(priv, port), (rtk_stat_port_type_t)cnt[i], &cnt[i]);
		if (RT_ERR_OK != rc) {
			return (RT_ERR_NOT_INIT == rc) ? -EBUSY : -ENODATA;
		}
	}

/**
 * @note counters ifOutUcastPkts, ifOutMulticastPkts, ifOutBroadcastPkts include discarded
 * or not sent frames as well.
 */
#define UEXT_RTL83XX_MIB_SUMS(COUNTER)                                                         \
	COUNTER(rx_byte, MIB(IfInOctets))                                                          \
	COUNTER(tx_byte, MIB(IfOutOctets))                                                         \
	COUNTER(rx_total, MIB(IfInUcastPkts) + MIB(IfInMulticastPkts) + MIB(IfInBroadcastPkts))    \
	COUNTER(tx_total, MIB(IfOutUcastPkts) + MIB(IfOutMulticastPkts) + MIB(IfOutBroadcastPkts)) \
	COUNTER(rx_bcast, MIB(IfInBroadcastPkts))                                                  \
	COUNTER(tx_bcast, MIB(IfOutBroadcastPkts))                                                 \
	COUNTER(rx_mcast, MIB(IfInMulticastPkts))                                                  \
	COUNTER(tx_mcast, MIB(IfOutMulticastPkts))                                                 \
	COUNTER(rx_pause, MIB(Dot3InPauseFrames))                                                  \
	COUNTER(tx_pause, MIB(Dot3OutPauseFrames))                                                 \
	COUNTER(rx_jumbo, MIB(EtherOversizeStats))                                                 \
	COUNTER(tx_jumbo, 0 /*not supported */)                                                    \
	COUNTER(rx_dropped, MIB(Dot3StatsFCSErrors) + MIB(Dot3StatsSymbolErrors) +                 \
				        MIB(EtherStatsFragments) + MIB(EtherStatsJabbers) +                    \
				        MIB(EtherStatsDropEvents) + MIB(InMldChecksumError) +                  \
				        MIB(InIgmpChecksumError) + MIB(InReportSuppressionDrop) +              \
				        MIB(InReportSuppressionDrop) + MIB(InLeaveSuppressionDrop) +           \
				        MIB(Dot1dTpPortInDiscards))                                            \
	COUNTER(tx_dropped, MIB(Dot3StatsExcessiveCollisions))                                     \
	COUNTER(rx_error,   MIB(Dot3StatsFCSErrors) + MIB(Dot3StatsSymbolErrors) +                 \
				        MIB(EtherStatsFragments) + MIB(EtherStatsJabbers) +                    \
				        MIB(EtherStatsDropEvents) + MIB(InMldChecksumError) +                  \
				        MIB(InIgmpChecksumError) + MIB(InReportSuppressionDrop) +              \
				        MIB(InReportSuppressionDrop) + MIB(InLeaveSuppressionDrop))            \
	COUNTER(tx_error, 0 /*not supported */)

#define COUNTER(_field, _sum) mib->_field = (_sum);
	UEXT_RTL83XX_MIB_SUMS(COUNTER)
#undef COUNTER
#undef ENUM
#undef MIB

	return 0;
}

/**
 * @brief Get ARL entry (swconfig ubnt extension)
 *
 * @param dev - switch control structure
 * @param entry_out [out] - ARL entry
 * @param it - iterator structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_uext_get_arl_entry(struct switch_dev *dev, struct uext_arl_lut_entry *entry_out,
				      struct uext_arl_iter *it)
{
	struct rtl8370_priv *priv = sw_to_rtl8370(dev);
	rtk_l2_ucastAddr_t rtl8370_entry = { 0 };
	rtl_arl_iter_t *rtl_it = NULL;
	int rc = 0;

	if (NULL == dev || NULL == priv || NULL == it) {
		return -EINVAL;
	}

	switch (it->op) {
	case UEXT_ARL_IT_BEGIN:
		/* Start of iteration */
		rtl_it = kzalloc(sizeof(*rtl_it), GFP_KERNEL);
		if (NULL == rtl_it) {
			return -ENOMEM;
		}
		/* Set address to 0 */
		*rtl_it = 0;
		it->data = rtl_it;
		break;

	case UEXT_ARL_IT_END:
		rtl_it = it->data;
		if (rtl_it) {
			kfree(rtl_it);
			it->data = NULL;
		}
		break;

	case UEXT_ARL_IT_NEXT:
		if (NULL == entry_out || NULL == it->data) {
			return -EINVAL;
		}

		rtl_it = it->data;

		rc = rtk_l2_addr_next_get(READMETHOD_NEXT_L2UC, 0, rtl_it, &rtl8370_entry);
		if (rc) {
			switch(rc) {
				case RT_ERR_NOT_INIT :
					rc = -EBUSY;
					break;
				case RT_ERR_L2_ENTRY_NOTFOUND:
				case RT_ERR_L2_L2UNI_PARAM:
					rc = -ENODATA;
					break;
				default:
					rtl_err(rc, "failed to get arl entry");
					rc = -ENODATA;
					break;
			}
			return rc;
		}

		entry_out->port = _rtl_port_l2p(priv, rtl8370_entry.port);
		entry_out->vid = rtl8370_entry.cvid;
		memcpy(entry_out->mac, rtl8370_entry.mac.octet, sizeof(entry_out->mac));
		(*rtl_it)++;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}
#endif /* CONFIG_SWCONFIG_UBNT_EXT */

/**
 * @brief Sync VLAN state
 *
 * @param priv - rtl8370's private structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_arl_age_time_apply(struct rtl8370_priv *priv)
{
	rtk_api_ret_t rc;
	int i;

	/* Set ARL aging time */
	rc = rtk_l2_aging_set(priv->arl_age_time);
	if(rc) {
		rtl_err(rc, "%s: Failed to set ARL aging time.", __func__);
		return -EIO;
	}

	/* Enable ARL aging for all ports by default */
	for (i = 0; i < priv->swdev.ports; ++i) {
		rc = rtk_l2_agingEnable_set(_rtl_port_p2l(priv, i), ENABLED);
		if (RT_ERR_OK != rc) {
			rtl_err(rc, "%s: Failed to enable ARL aging port %d.", __func__, i);
			return -EIO;
		}
	}

	return 0;
}

/**
 * @brief Apply configuration
 *
 * @param dev - switch control structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_sw_hw_apply(struct switch_dev *dev)
{
	int rc;
	struct rtl8370_priv *priv = sw_to_rtl8370(dev);

	if ((rc = rtl8370_port_isolation_apply(priv))) {
		return rc;
	}

	if ((rc = rtl8370_vlan_apply(priv))) {
		return rc;
	}

	if ((rc = ubnt_acl_sync(&priv->hw_acl))) {
		return rc;
	}

	if ((rc = rtl8370_arl_age_time_apply(priv))) {
		return rc;
	}

	rc = rtk_switch_soft_reset();
	if (RT_ERR_OK != rc) {
		rtl_err(rc, "%s: Failed to soft reset switch.", __func__);
		return -ENODATA;
	}

	return 0;
}

/* clang-format off */

/**
 * @brief Switch global attributes
 */
static struct switch_attr rtl8370_globals[] = {
	{
		.type = SWITCH_TYPE_NOVAL,
		.name = "reset_mibs",
		.description = "Reset all MIB counters",
		.set = rtl8370_sw_reset_mibs
	},
	{
		.type = SWITCH_TYPE_NOVAL,
		.name = "hard_reset",
		.description = "Hard reset",
		.set = rtl8370_hw_reset_switch
	},
	{
		.type = SWITCH_TYPE_STRING,
		.name = "acl_table",
		.description = "Get ACL table",
		.set = NULL,
		.get = rtl8370_sw_get_acl_table,
	},
	{
		.type = SWITCH_TYPE_NOVAL,
		.name = "flush_acl_table",
		.description = "Flush ACL table",
		.set = rtl8370_sw_flush_acl_table,
	},
	{
		.type = SWITCH_TYPE_STRING,
		.name = "acl_redirect",
		.description = "Add MAC redirection rule to ACL",
		.set = rtl8370_sw_set_acl_redirect,
		.get = NULL,
	},
	{
		.type = SWITCH_TYPE_STRING,
		.name = "acl_assign_vlan",
		.description = "Assign VLAN/SVLAN to a specific traffic",
		.set = rtl8370_sw_set_acl_vlan_assign,
		.get = NULL,
	},
	{
		.type = SWITCH_TYPE_INT,
		.name = "enable_acl",
		.description = "Enable ACL mode",
		.set = rtl8370_sw_set_acl,
		.get = rtl8370_sw_get_acl,
		.max = 1
	},
	{
		.type = SWITCH_TYPE_INT,
		.name = "arl_age_time",
		.description = "ARL age time (secs)",
		.set = rtl8370_sw_set_arl_age_time,
		.get = rtl8370_sw_get_arl_age_time,
	},
	{
		.type = SWITCH_TYPE_NOVAL,
		.name = "flush_arl_table",
		.description = "Flush ARL table",
		.set = rtl8370_sw_set_flush_arl_table,
	},
	{
		.type = SWITCH_TYPE_STRING,
		.name = "arl_table",
		.description = "Get ARL table",
		.set = NULL,
		.get = rtl8370_sw_get_arl_table
	}
};

/**
 * @brief Switch port based attributes
 */
static struct switch_attr rtl8370_port[] = {
	{
		.type = SWITCH_TYPE_NOVAL,
		.name = "reset_mib",
		.description = "Reset single port MIB counters",
		.set = rtl8370_sw_reset_port_mibs,
	},
	{
		.type = SWITCH_TYPE_STRING,
		.name = "mib",
		.description = "Get MIB counters for port",
		.max = 33,
		.set = NULL,
		.get = rtl8370_sw_get_port_mib,
	},
	{
		.type = SWITCH_TYPE_INT,
		.name = "enable_vlan",
		.description = "Enable VLAN mode",
		.set = rtl8370_sw_set_vlan_enable,
		.get = rtl8370_sw_get_vlan_enable,
		.max = 1,
		.ofs = 1
	},
	{
		.type = SWITCH_TYPE_PORTS,
		.name = "isolation",
		.description ="Get/Set a port isolation mask",
		.set = rtl8370_sw_set_port_isolation,
		.get = rtl8370_sw_get_port_isolation,
	},
	{
		.type = SWITCH_TYPE_INT,
		.name = "efid",
		.description ="Get/Set port efid",
		.max = RTK_EFID_MAX,
		.set = rtl8370_sw_set_port_efid,
		.get = rtl8370_sw_get_port_efid,
	},
};

/**
 * @brief Switch VLAN based attributes
 */
static struct switch_attr rtl8370_vlan[] = {
	{
		.type = SWITCH_TYPE_STRING,
		.name = "info",
		.description = "Get vlan information",
		.max = 1,
		.set = NULL,
		.get = rtl8370_sw_get_vlan_info,
	},
	{
		.type = SWITCH_TYPE_INT,
		.name = "fid",
		.description = "Get/Set vlan FID",
		.max = RTK_FID_MAX,
		.set = rtl8370_sw_set_vlan_fid,
		.get = rtl8370_sw_get_vlan_fid,
	},
	{
		.type = SWITCH_TYPE_INT,
		.name = "enable_ivl",
		.description = "Enable IVL",
		.set = rtl8370_sw_set_ivl,
		.get = rtl8370_sw_get_ivl,
		.max = 1,
		.ofs = 1
	},
};

#ifdef CONFIG_SWCONFIG_UBNT_EXT
/**
 * @brief Switch SVLAN based attributes
 */
static struct switch_attr rtl8370_svlan[] = {
	{
		.type = SWITCH_TYPE_INT,
		/** @note
		 * It's believed that "efid" is a chip-specific feature. In order to keep swconfig API
		 * as general as possible and to avoid exposing another attribute of the same functionality
		 * ("fid") to UUS, it was decided to name this attribute "fid" over the preferable "efid".
		 */
		.name = "fid",
		.description = "Get/Set svlan FID (implemented by enhanced FID identifier). ",
		.max = RTK_EFID_MAX,
		.set = rtl8370_sw_set_svlan_efid,
		.get = rtl8370_sw_get_svlan_efid,
	}
};
#endif

/**
 * @brief Switch ops
 */
static const struct switch_dev_ops rtl8370_sw_ops = {

	/* clang-format off */
	.attr_global = {
		.attr = rtl8370_globals,
		.n_attr = ARRAY_SIZE(rtl8370_globals),
	},
	.attr_port = {
		.attr = rtl8370_port,
		.n_attr = ARRAY_SIZE(rtl8370_port),
	},
	.attr_vlan = {
		.attr = rtl8370_vlan,
		.n_attr = ARRAY_SIZE(rtl8370_vlan),
	},
	/* clang-format on */

	.get_vlan_ports = rtl8370_sw_get_vlan_ports,
	.set_vlan_ports = rtl8370_sw_set_vlan_ports,
	.get_port_pvid = rtl8370_sw_get_port_pvid,
	.set_port_pvid = rtl8370_sw_set_port_pvid,
	.reset_switch = rtl8370_sw_reset_switch,
	.get_port_link = rtl8370_sw_get_port_link,
	.get_port_stats = rtl8370_sw_get_port_stats,
	.apply_config = rtl8370_sw_hw_apply,
#ifdef CONFIG_SWCONFIG_UBNT_EXT
	.ubnt_ext_ops = {
		.get_arl_entry = rtl8370_uext_get_arl_entry,
		.get_port_mib = rtl8370_uext_get_port_mib,
		.get_svlan_ports = rtl8370_sw_get_svlan_ports,
		.set_svlan_ports = rtl8370_sw_set_svlan_ports,
	},

	.attr_svlan = {
		.attr = rtl8370_svlan,
		.n_attr = ARRAY_SIZE(rtl8370_svlan),
	},
#endif
	.set_port_link = rtl8370_sw_set_port_link,
	.phy_write16 = rtl8370_sw_phy_write16,
};

/**
 * @brief ACL platform dependent structure
 */
const struct acl_hw_ops rtl8370_acl_ops = {
	.init = rtl8370_acl_hw_init,
	.alloc = rtl8370_acl_hw_alloc,
	.destroy = rtl8370_acl_hw_destroy,
	.flush = rtl8370_acl_hw_flush,
	.enable = rtl8370_acl_hw_enable,
	.rule_delete = rtl8370_acl_hw_rule_delete,
	.rule_set = rtl8370_acl_hw_rule_set,
	.rule_sw_to_hw = rtl8370_acl_rule_sw_to_hw,
	.max_entries = RTL8370_NUM_ACL_ENTRIES,

	.rule_get = NULL,
	.rule_hw_to_sw = NULL,
};

/* clang-format on */

/**
 * @brief Retrieve PHY status
 *
 * @param phydev - PHY device control structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_phy_read_status(struct phy_device *phydev)
{
	int idx = (RTL8370_PHY_ADDR_CPU_0 == phydev->mdio.addr) ? 0 : 1;
	struct rtl8370_priv *priv = phydev->priv;

	/**
	 * Fix the link of all CPU ports. We don't want to
	 * propagate link status of physical ports to CPU ports.
	 */

	phydev->speed = rtl8370_tr_port_speed_ethtool(priv->mac_mode[idx].speed);
	phydev->duplex = DUPLEX_FULL;
	phydev->link = 1;

	/*
	 * Bypass generic PHY status read,
	 * it doesn't work with this switch
	 */
	phydev->state = PHY_RUNNING;
	netif_carrier_on(phydev->attached_dev);
	phydev->adjust_link(phydev->attached_dev);

	return 0;
}

/**
 * @brief Initialization of PHY
 *
 * @param phydev - PHY device control structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_phy_config_init(struct phy_device *phydev)
{
	struct rtl8370_priv *priv = phydev->priv;

	if (WARN_ON(!priv)) {
		return -ENODEV;
	}

	return 0;
}

/**
 * @brief PHY auto negotiation
 *
 * @param phydev - PHY device control structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_phy_config_aneg(struct phy_device *phydev)
{
	/* No need. The link status for the CPU ports is fixed, */
	return 0;
}

/**
 * @brief PHY soft reset
 *
 * @param phydev - PHY device control structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_phy_soft_reset(struct phy_device *phydev)
{
	/* we don't need an extra reset */
	return 0;
}

/**
 * @brief Retrieve auto negotiation status
 *
 * @param phydev - PHY device control structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_phy_aneg_done(struct phy_device *phydev)
{
	/* No need. The link status for the CPU ports is fixed, */
	return 0;
}

/**
 * @brief Alloc & init rtl8370's private structure
 *
 * @param phydev - PHY device control structure
 * @return struct rtl8370_priv* - rtl8370's private structure
 */
static struct rtl8370_priv *rtl8370_alloc(struct phy_device *phydev)
{
	struct rtl8370_priv *priv;

	priv = kzalloc(sizeof(struct rtl8370_priv), GFP_KERNEL);

	if (NULL == priv) {
		return NULL;
	}

	mutex_init(&priv->api_ctl.api_mutex);
	priv->api_ctl.bus = phydev->mdio.bus;
	priv->hw_acl.ops = &rtl8370_acl_ops;

	if (RT_ERR_OK != rtk_switch_glueCtl_register(&priv->api_ctl)) {
		kfree(priv);
		return NULL;
	}

	return priv;
}

/**
 * @brief Dealloc rtl8370's private structure
 *
 * @param priv - rtl8370's private structure
 */
static void rtl8370_free(struct rtl8370_priv *priv)
{
	if (priv) {
		ubnt_acl_destroy(&priv->hw_acl);
		rtk_switch_glueCtl_unregister(&priv->api_ctl);
		kfree(priv);
	}
}

/**
 * @brief Set default parameters
 *
 * @param priv - rtl8370's private structure
 */
static void rtl8370_of_defaults(struct rtl8370_priv *priv)
{
	int i = 0;

	priv->reset_on_init = false;

	for (i = 0; i < priv->port_map->cpu_cnt; ++i) {
		memset(&priv->mac_mode[i], 0, sizeof(priv->mac_mode[i]));
		priv->mac_mode[i].mode = MODE_EXT_1000X;
		priv->mac_mode[i].speed = PORT_SPEED_1000M;
	}
}

/**
 * @brief Set mac mode structure by of property value
 *
 * @param mac_mode - mac mode structure
 * @param str - of property value
 */
static inline void rtl8370_mac_mode_set(struct rtl8370_mac_mode *mac_mode, const char *str)
{
	if (!strcmp(str, RTL8370_OF_VAL_MODE_DISABLED)) {
		mac_mode->mode = MODE_EXT_DISABLE;
		mac_mode->speed = PORT_SPEED_10M;
	} else if (!strcmp(str, RTL8370_OF_VAL_MODE_SGMII_2_5G)) {
		mac_mode->mode = MODE_EXT_HSGMII;
		mac_mode->speed = PORT_SPEED_2500M;
	} else if (!strcmp(str, RTL8370_OF_VAL_MODE_1000X)) {
		mac_mode->mode = MODE_EXT_1000X;
		mac_mode->speed = PORT_SPEED_1000M;
	} else if  (!strcmp(str, RTL8370_OF_VAL_MODE_RGMII)) {
		mac_mode->mode = MODE_EXT_RGMII;
		mac_mode->speed = PORT_SPEED_1000M;
	}
}

/**
 * @brief Parse OF tree
 *
 * @param priv - rtl8370's private structure
 * @param phydev - PHY device control structure
 */
static void rtl8370_of_setup(struct rtl8370_priv *priv, struct phy_device *phydev)
{
	struct device_node *np_dev = NULL, *np_mac = NULL, *child = NULL;
	uint32_t reg = 0;
	const char *str;
	struct rtl8370_mac_mode *mac_mode;

	rtl8370_of_defaults(priv);

	for_each_matching_node (np_dev, realtek_rtl8370_of_table) {
		if (!of_property_read_u32(np_dev, RTL8370_OF_ATT_PHY_ADDR, &reg)) {
			if (reg != phydev->mdio.addr) {
				continue;
			}

			priv->reset_on_init = of_property_read_bool(np_dev, RTL8370_OF_ATT_RESET_ON_INIT);

			/* Led profile */
			if (!of_property_read_u32(np_dev, RTL8370_OF_ATT_LED_PROFILE, &reg)) {
				if (reg < LED_PROFILE_COUNT) {
					priv->led_profile = reg;
				} else {
					priv->led_profile = LED_PROFILE_OFF;
				}
			}
			/* MAC mode settings */
			of_node_get(np_dev); /* np_dev is going to be consumed by of_find_node_by_name vv */
			np_mac = of_find_node_by_name(np_dev, RTL8370_OF_NODE_MAC);
			if (np_mac) {
				for_each_child_of_node (np_mac, child) {
					if (strcmp(child->name, RTL8370_OF_NODE_CPU_PORT)) {
						continue;
					}

					if (of_property_read_u32(child, RTL8370_OF_ATT_PHY_ADDR,
								 &reg)) {
						continue;
					}
					if (reg >= 0 && reg < priv->port_map->cpu_cnt) {
						mac_mode = &priv->mac_mode[reg];
						if (of_property_read_string(
							    child, RTL8370_OF_ATT_MAC_MODE, &str)) {
							continue;
						}
						/* set mac mode with of property value */
						rtl8370_mac_mode_set(mac_mode, str);
						/* read rx/tx delay */
						if (!of_property_read_u32(child, RTL8370_OF_ATT_TX_DELAY, &reg)) {
							mac_mode->delay_tx = reg;
						}
						if (!of_property_read_u32(child, RTL8370_OF_ATT_RX_DELAY, &reg)) {
							mac_mode->delay_rx = reg;
						}

						mac_mode->nway =
							of_property_read_bool(child, RTL8370_OF_ATT_NWAY_ENABLE) ?
								ENABLED :
								DISABLED;
					}
				}
				of_node_put(np_mac);
			}
		}
	}
}

/**
 * @brief Assert GPIO reset signal
 *
 * @param priv - rtl8370's private structure
 * @param value - reset signal state
 */
static inline void rtl8370_switch_reset_set(struct rtl8370_priv *priv, unsigned value)
{
	if (gpio_cansleep(priv->reset_pin)) {
		gpio_set_value_cansleep(priv->reset_pin, value);
	} else {
		gpio_set_value(priv->reset_pin, value);
	}
}

/**
 * @brief RTL8370 handle GPIO reset
 *
 * @param priv - rtl8370's private structure
 * @param phydev - PHY device control structure
 */
static void rtl8370_switch_reset(struct rtl8370_priv *priv, struct phy_device *phydev)
{
	struct device_node *np_dev = NULL;
	int rc = 0;
	uint32_t reg = 0;

	for_each_matching_node (np_dev, realtek_rtl8370_of_table) {
		if (!of_property_read_u32(np_dev, RTL8370_OF_ATT_PHY_ADDR, &reg)) {
			if (reg != phydev->mdio.addr) {
				continue;
			}

			priv->reset_pin = of_get_named_gpio(np_dev, RTL8370_OF_ATT_RESET_GPIOS, 0);
			if (!gpio_is_valid(priv->reset_pin)) {
				of_node_put(np_dev);
				goto of_break;
			}

			rc = gpio_request_one(priv->reset_pin, GPIOF_OUT_INIT_LOW, "rt8370,reset-pin");
			if (rc) {
				rtl_warn("rtl8370: failed to request reset GPIO %d\n", rc);
				goto of_break;
			}

			rtl8370_switch_reset_set(priv, 0);
			/* sleep ~ 1 ms */
			usleep_range(1000, 1100);
			/* set reset HIGH */
			rtl8370_switch_reset_set(priv, 1);
			/* wait 500 ms */
			mdelay(500);
			/* free reset GPIO */
			gpio_free(priv->reset_pin);
of_break:
			of_node_put(np_dev);
			break;
		}
	}
}

/**
 * @brief RTL8370 probing & initialization
 *
 * @param priv - rtl8370's private structure
 * @param phydev - PHY device control structure
 * @return error from errno.h, 0 on success
 */

static int rtl8370_switch_detected(struct rtl8370_priv *priv, struct phy_device *phydev)
{
	struct switch_dev *swdev;
	rtk_api_ret_t rc;
	switch_chip_t switch_chip;
	int i = 0;

	/* toggle reset if possible */
	rtl8370_switch_reset(priv, phydev);

	/* probe switch */
	rc = rtk_switch_probe(&switch_chip);

	if (RT_ERR_OK != rc) {
		return -ENXIO;
	}

	priv->port_map = NULL;
	for (i = 0; i < ARRAY_SIZE(_rtl_port_mapping); ++i) {
		if(_rtl_port_mapping[i].id == switch_chip) {
			priv->port_map = &_rtl_port_mapping[i];
		}
	}

	if(NULL == priv->port_map) {
		return -ENXIO;
	}

	swdev = &priv->swdev;
	swdev->cpu_port = priv->port_map->cpu[0];
	swdev->name = priv->port_map->name;
	swdev->ports = priv->port_map->port_cnt;
	swdev->vlans = RTL8370_NUM_VIDS;
	swdev->ops = &rtl8370_sw_ops;
	swdev->alias = dev_name(&priv->mii_bus->dev);

	/* set port count for ubnt_acl */
	priv->hw_acl.max_ports = swdev->ports;

	rtl8370_of_setup(priv, phydev);

	rtl8370_setup(priv, priv->reset_on_init);

	return 0;
}

/**
 * @brief PHY probing
 *
 * @param phydev - PHY device control structure
 * @return int - error from errno.h, 0 on success
 */
static int rtl8370_phy_probe(struct phy_device *phydev)
{
	struct rtl8370_priv *priv;
	struct switch_dev *swdev;
	int ret, idx;

	/* skip PHYs at unused adresses */
	if (RTL8370_PHY_ADDR_CPU_0 != phydev->mdio.addr && RTL8370_PHY_ADDR_CPU_1 != phydev->mdio.addr) {
		return -ENODEV;
	}

	mutex_lock(&rtl8370_dev_list_lock);

	list_for_each_entry (priv, &rtl8370_dev_list, list) {
		if (priv->mii_bus == phydev->mdio.bus) {
			goto found;
		}
	}

	priv = rtl8370_alloc(phydev);
	if (!priv) {
		ret = -ENOMEM;
		goto unlock;
	}

	priv->mii_bus = phydev->mdio.bus;

	ret = rtl8370_switch_detected(priv, phydev);
	if (ret) {
		goto free_priv;
	}

	swdev = &priv->swdev;
	ret = register_switch(swdev, NULL);
	if (ret)
		goto free_priv;

	rtl_info("%s: %s switch registered on %s\n", swdev->devname, swdev->name,
		 dev_name(&priv->mii_bus->dev));

	list_add(&priv->list, &rtl8370_dev_list);

found:
	priv->use_count++;
	idx = (RTL8370_PHY_ADDR_CPU_0 == phydev->mdio.addr) ? 0 : 1;
	if (MODE_EXT_HSGMII == priv->mac_mode[idx].mode) {
		phydev->supported |= SUPPORTED_2500baseX_Full;
		phydev->advertising |= ADVERTISED_2500baseX_Full;
	} else {
		phydev->supported |= SUPPORTED_1000baseT_Full;
		phydev->advertising |= ADVERTISED_1000baseT_Full;
	}
	phydev->priv = priv;

	mutex_unlock(&rtl8370_dev_list_lock);

	return 0;

free_priv:
	rtl8370_free(priv);
unlock:
	mutex_unlock(&rtl8370_dev_list_lock);
	return ret;
}

/**
 * @brief PHY remove function
 *
 * @param phydev - PHY device control structure
 */
static void rtl8370_phy_remove(struct phy_device *phydev)
{
	struct rtl8370_priv *priv = phydev->priv;

	if (WARN_ON(!priv)) {
		return;
	}

	phydev->priv = NULL;

	mutex_lock(&rtl8370_dev_list_lock);

	if (--priv->use_count > 0) {
		mutex_unlock(&rtl8370_dev_list_lock);
		return;
	}

	list_del(&priv->list);
	mutex_unlock(&rtl8370_dev_list_lock);
	unregister_switch(&priv->swdev);
	rtl8370_free(priv);
}

/**
 * @brief PHY driver control structure
 */
static struct phy_driver rtl870_phy_driver[] = {
	{
	/* Fake PHY ID */
	.phy_id = PHY_FAKE_ID_RTL8370,
	.name = "Realtek RTL8370MB",
	.phy_id_mask = 0xffffffff,
	.features = PHY_BASIC_FEATURES,
	.probe = rtl8370_phy_probe,
	.remove = rtl8370_phy_remove,
	.config_init = rtl8370_phy_config_init,
	.config_aneg = rtl8370_phy_config_aneg,
	.aneg_done = rtl8370_phy_aneg_done,

	.read_status = rtl8370_phy_read_status,
	.soft_reset = rtl8370_phy_soft_reset,
	}
};


module_phy_driver(rtl870_phy_driver);

MODULE_DESCRIPTION("Realtek RTL8370 PHY driver");
MODULE_AUTHOR("Jan Kapic <jan.kapic@ubnt.com>");
