/*
 * ubnt_acl.h: Ubiquiti's ACL abstraction for PHY drivers
 * Copyright 2018 Ubiquiti Networks, Inc.
 */
#ifndef _UBNT_ACL_H_
#define _UBNT_ACL_H_

#define ACL_BUFF_SIZE(entries) (entries * 130 + 16)
#define ACL_TOTAL_CNT(acl_table) (acl_table.cnt_active + acl_table.cnt_inactive)

/**
 * @brief Helpers
 *
 */
#define BUF_APPEND_PRINTF(fmt, ...)                                                        \
	do {                                                                               \
		str_len += snprintf(str + str_len, str_max - str_len, fmt, ##__VA_ARGS__); \
		if (str_len >= str_max) {                                                  \
			printk(KERN_ERR "%s: snprintf error buffer (%d bytes) too small.", \
			       __func__, str_max);                                         \
			goto error;                                                        \
		}                                                                          \
	} while (0)

/**
 * NOTE: The implementation of ACL is simplified for our
 * pourposes. General ACL rule is much more complex/robust.
 * Might be extended in the future.
 */
#define ACL_RULE_STATE(X) \
	X(INACTIVE)       \
	X(MODIFIED)       \
	X(ACTIVE)         \
	X(DELETED)

typedef enum {
#define X(state) ACL_RULE_##state,
	ACL_RULE_STATE(X)
#undef X
		ACL_RULE_UNKNOWN
} acl_entry_state_t;

typedef enum {
	ACL_RULE_PORT_REDIRECTION = 0,
	ACL_RULE_VLAN_ASSIGNMENT,
	ACL_RULE_ETHVLAN,
} acl_entry_type_t;

typedef struct {
	uint8_t uc[ETH_ALEN];
} acl_mac_t;

/**
 * SW representation of ACL record
 */
typedef struct {
	acl_entry_type_t type;
	acl_mac_t mac_sa;
	acl_mac_t mac_da;

	uint32_t port_dst;
	uint32_t port_src;

	/* vlan assignment */
	uint16_t svid;
	uint16_t ether_type;

	/* extra for ethvlan redirection */
	uint16_t force_vtu;
	uint16_t vlan_dst;
	uint16_t vlan_src;

	int idx;

	acl_entry_state_t state;
	struct list_head list;
} acl_entry_t;

struct acl_list {
	uint8_t enable;
	int idx_last;
	unsigned int cnt_active;
	unsigned int cnt_inactive;
	struct list_head list;
};

struct acl_hw;
struct acl_hw_ops {
	/* These are mandatory */
	const uint32_t max_entries;

	int (*const alloc)(struct acl_hw *hw);
	void (*const destroy)(struct acl_hw *hw);

	int (*const init)(struct acl_hw *hw);
	int (*const flush)(struct acl_hw *hw);
	int (*const enable)(struct acl_hw *hw, uint8_t enable);
	int (*const rule_delete)(struct acl_hw *hw, int idx);
	int (*const rule_set)(struct acl_hw *hw, int idx, void *data_in);
	int (*const rule_sw_to_hw)(struct acl_hw *hw, acl_entry_t *entry,
				   void *data_out);

	/* These are optional */
	int (*const rule_hw_to_sw)(struct acl_hw *hw, void *data_in,
				   acl_entry_t *entry);

	int (*const rule_get)(struct acl_hw *hw, int idx, void *data_out);
};

struct acl_hw {

	const struct acl_hw_ops *ops;
	uint32_t max_ports;

	/* HW private data structure */
	void *data;

	/* Internal use */
	char *str;
	uint32_t str_len;
	uint32_t str_max;
	struct acl_list acl_table;
};

/**
 * @brief Dealloc the ACL control structures
 *
 * @param hw - platform dependent ACL control structure
 */
void ubnt_acl_destroy(struct acl_hw *hw);

/**
 * @brief Init the ACL
 *
 * @param hw - platform dependent ACL control structure
 * @return int - error from errno.h, 0 on success
 */
int ubnt_acl_init(struct acl_hw *hw);

/**
 * @brief Flush the switch hw's ACL & clear the ACL linked list
 *
 * @param hw - platform dependent ACL control structure
 * @return int - error from errno.h, 0 on success
 */
int ubnt_acl_flush(struct acl_hw *hw);

/**
 * @brief Load switch hw's ACL list to linked list and sync
 *
 * @param hw - platform dependent ACL control structure
 * @return int - error from errno.h, 0 on success
 */
int ubnt_acl_preload(struct acl_hw *hw);

/**
 * @brief Sync the ACL linked list with switch hw's linked list
 *
 * @param hw - platform dependent ACL control structure
 * @return int - error from errno.h, 0 on success
 */
int ubnt_acl_sync(struct acl_hw *hw);

/**
 * @brief Parse ACL rule
 *
 * @param hw - platform dependent ACL control structure
 * @param str - port's numbers string
 * @param entry - the ACL entry
 * @param rule_type - type of ACL rule
 * @return int - error from errno.h, 0 on success
 */
int ubnt_acl_rule_process(struct acl_hw *hw, const char *str,
			  acl_entry_type_t rule_type);

/**
 * @brief Find ACL rule
 *
 * @param hw - platform dependent ACL control structure
 * @param entry_in - entry to find - key should be filled
 * @return int - error from errno.h, 0 on success
 */
int ubnt_acl_rule_get(struct acl_hw *hw, acl_entry_t *entry_in);

/**
 * @brief Get ACL table
 *
 * On success the ACL table is returned as a string in "str"
 * member of "acl_hw" structure. Member "str_len" contains the
 * size of the returned string.
 *
 * @param hw - platform dependent ACL control structure
 * @return int - error from errno.h, 0 on success
 *
 */
int ubnt_acl_get_acl_table(struct acl_hw *hw);

/**
 * @brief Retrive ACL status
 *
 * @param hw - platform dependent ACL control structure
 * @param enable ACL status
 * @return int - error from errno.h, 0 on success
 */
int ubnt_acl_enable_get(struct acl_hw *hw, int *enable);

/**
 * @brief Set ACL status
 *
 * @param hw - platform dependent ACL control structure
 * @param enable ACL status
 * @return int - error from errno.h, 0 on success
 */
int ubnt_acl_enable_set(struct acl_hw *hw, int enable);

/**
 * @brief Check if all bytes of mac address are set to zero
 *
 * @param mac mac address to compare
 * @return int 0 - all bytes set to zero
 */
int ubnt_mac_zero_addr(acl_mac_t *mac);

// same as std::isblank()
static inline int isblank(int c)
{
	return (c == ' ' || c == '\t') ? 1 : 0;
}
#endif /* _UBNT_ACL_H_ */
