/* al_mod_eth_sysfs.c: AnnapurnaLabs Unified 1GbE and 10GbE ethernet driver.
 *
 * Copyright (c) 2013 AnnapurnaLabs
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation.
 */

#include <linux/version.h>

#if LINUX_VERSION_CODE > KERNEL_VERSION(3,3,0)

#include <linux/device.h>
#include <linux/stat.h>
#include <linux/sysfs.h>
#ifdef CONFIG_ARCH_ALPINE
#include <al_mod_hal_udma.h>
#include <al_mod_hal_udma_debug.h>
#else
#include "al_mod_hal_udma.h"
#include "al_mod_hal_udma_debug.h"
#endif
#include "al_eth.h"
#include "al_mod_hal_eth_ec_regs.h"
#include "al_mod_hal_eth_common.h"
#include "al_mod_hal_pbs_regs.h"

#define to_ext_attr(x) container_of(x, struct dev_ext_attribute, attr)

static int al_mod_eth_validate_small_copy_len(struct al_mod_eth_adapter *adapter,
		unsigned long len)
{
	if (len > adapter->netdev->mtu)
		return -EINVAL;

	return 0;
}

static ssize_t al_mod_eth_store_small_copy_len(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t len)
{
	struct al_mod_eth_adapter *adapter = dev_get_drvdata(dev);
	unsigned long small_copy_len;
	int err;

	err = kstrtoul(buf, 10, &small_copy_len);
	if (err < 0)
		return err;

	err = al_mod_eth_validate_small_copy_len(adapter, small_copy_len);
	if (err)
		return err;

	rtnl_lock();
	adapter->small_copy_len = small_copy_len;
	rtnl_unlock();

	return len;
}

static ssize_t al_mod_eth_show_small_copy_len(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct al_mod_eth_adapter *adapter = dev_get_drvdata(dev);

	return sprintf(buf, "%d\n", adapter->small_copy_len);
}

static struct device_attribute dev_attr_small_copy_len = {
	.attr = {.name = "small_copy_len", .mode = (S_IRUGO | S_IWUSR)},
	.show = al_mod_eth_show_small_copy_len,
	.store = al_mod_eth_store_small_copy_len,
};

static ssize_t al_mod_eth_store_link_poll_interval(struct device *dev,
					      struct device_attribute *attr,
					      const char *buf, size_t len)
{
	struct al_mod_eth_adapter *adapter = dev_get_drvdata(dev);
	unsigned long link_poll_interval;
	int err;

	err = kstrtoul(buf, 10, &link_poll_interval);
	if (err < 0)
		return err;

	adapter->link_poll_interval = link_poll_interval;

	return len;
}

static ssize_t al_mod_eth_show_link_poll_interval(struct device *dev,
				       struct device_attribute *attr, char *buf)
{
	struct al_mod_eth_adapter *adapter = dev_get_drvdata(dev);

	return sprintf(buf, "%d\n", adapter->link_poll_interval);
}

static struct device_attribute dev_attr_link_poll_interval = {
	.attr = {.name = "link_poll_interval", .mode = (S_IRUGO | S_IWUSR)},
	.show = al_mod_eth_show_link_poll_interval,
	.store = al_mod_eth_store_link_poll_interval,
};

#ifdef CONFIG_ARCH_ALPINE
static ssize_t al_mod_eth_store_link_management_debug(struct device *dev,
					      struct device_attribute *attr,
					      const char *buf, size_t len)
{
	struct al_mod_eth_adapter *adapter = dev_get_drvdata(dev);
	unsigned long link_management_debug;
	int err;

	err = kstrtoul(buf, 10, &link_management_debug);
	if (err < 0)
		return err;

	if (adapter->rev_id >= AL_ETH_REV_ID_4)
		al_mod_eth_v4_lm_debug_mode_set(&adapter->eth_v4_lm_handle,
					    (link_management_debug) ?
					    true : false);
	else
		al_mod_eth_lm_debug_mode_set(&adapter->lm_context,
					 (link_management_debug) ?
					 true : false);

	return len;
}

static struct device_attribute dev_attr_link_management_debug = {
	.attr = {.name = "link_management_debug", .mode = S_IWUSR},
	.show = NULL,
	.store = al_mod_eth_store_link_management_debug,
};
#endif /* CONFIG_ARCH_ALPINE */

static ssize_t al_mod_eth_store_link_training_enable(struct device *dev,
					      struct device_attribute *attr,
					      const char *buf, size_t len)
{
	struct al_mod_eth_adapter *adapter = dev_get_drvdata(dev);
	unsigned long link_training_enable;
	int err;

	err = kstrtoul(buf, 10, &link_training_enable);
	if (err < 0)
		return err;

	adapter->lt_en = (link_training_enable == 0) ? AL_FALSE : AL_TRUE;

	if (adapter->up)
		dev_warn(dev,
			 "%s this action will take place in the next activation (up)\n",
			  __func__);

	return len;
}

static ssize_t al_mod_eth_show_link_training_enable(struct device *dev,
				       struct device_attribute *attr, char *buf)
{
	struct al_mod_eth_adapter *adapter = dev_get_drvdata(dev);

	return sprintf(buf, "%d\n", adapter->lt_en);
}

static struct device_attribute dev_attr_link_training_enable = {
	.attr = {.name = "link_training_enable", .mode = (S_IRUGO | S_IWUSR)},
	.show = al_mod_eth_show_link_training_enable,
	.store = al_mod_eth_store_link_training_enable,
};

static int al_mod_eth_sfp_probe_enable_set(struct device *dev, struct device_attribute *attr,
				       const char *buf, size_t len,
				       enum al_mod_eth_lm_sfp_probe_type type)
{
	struct al_mod_eth_adapter *adapter = dev_get_drvdata(dev);
	unsigned long enable;
	int err;

	err = kstrtoul(buf, 10, &enable);
	if (err < 0)
		return err;

	switch (type) {
	case AL_ETH_LM_SFP_PROBE_1G:
		adapter->sfp_probe_1g = !!enable;
		break;
	case AL_ETH_LM_SFP_PROBE_10G:
		adapter->sfp_probe_10g = !!enable;
		break;
	default:
		return -EOPNOTSUPP;
	}
	if (adapter->up)
		dev_warn(dev, "%s this action will take place in the next activation (up)\n",
			 __func__);
	return len;
}

static int al_mod_eth_sfp_probe_enable_get(struct device *dev, struct device_attribute *attr, char *buf,
				       enum al_mod_eth_lm_sfp_probe_type type)
{
	struct al_mod_eth_adapter *adapter = dev_get_drvdata(dev);
	int enable = 0;

	switch (type) {
	case AL_ETH_LM_SFP_PROBE_1G:
		enable = adapter->sfp_probe_1g;
		break;
	case AL_ETH_LM_SFP_PROBE_10G:
		enable = adapter->sfp_probe_10g;
		break;
	default:
		return -EOPNOTSUPP;
	}
	return sprintf(buf, "%d\n", enable);
}

static ssize_t al_mod_eth_show_sfp_probe_1g(struct device *dev, struct device_attribute *attr,
					char *buf)
{
	return al_mod_eth_sfp_probe_enable_get(dev, attr, buf, AL_ETH_LM_SFP_PROBE_1G);
}

static ssize_t al_mod_eth_store_sfp_probe_1g(struct device *dev, struct device_attribute *attr,
					 const char *buf, size_t len)
{
	return al_mod_eth_sfp_probe_enable_set(dev, attr, buf, len, AL_ETH_LM_SFP_PROBE_1G);
}

static struct device_attribute dev_attr_sfp_probe_1g = {
	.attr = { .name = "sfp_probe_1g", .mode = (S_IRUGO | S_IWUSR) },
	.show = al_mod_eth_show_sfp_probe_1g,
	.store = al_mod_eth_store_sfp_probe_1g,
};

static ssize_t al_mod_eth_show_sfp_probe_10g(struct device *dev, struct device_attribute *attr,
					 char *buf)
{
	return al_mod_eth_sfp_probe_enable_get(dev, attr, buf, AL_ETH_LM_SFP_PROBE_10G);
}

static ssize_t al_mod_eth_store_sfp_probe_10g(struct device *dev, struct device_attribute *attr,
					  const char *buf, size_t len)
{
	return al_mod_eth_sfp_probe_enable_set(dev, attr, buf, len, AL_ETH_LM_SFP_PROBE_10G);
}

static struct device_attribute dev_attr_sfp_probe_10g = {
	.attr = { .name = "sfp_probe_10g", .mode = (S_IRUGO | S_IWUSR) },
	.show = al_mod_eth_show_sfp_probe_10g,
	.store = al_mod_eth_store_sfp_probe_10g,
};

static ssize_t al_mod_eth_store_force_1000_base_x(struct device *dev,
					      struct device_attribute *attr,
					      const char *buf, size_t len)
{
	struct al_mod_eth_adapter *adapter = dev_get_drvdata(dev);
	unsigned long force_1000_base_x;
	int err;

	err = kstrtoul(buf, 10, &force_1000_base_x);
	if (err < 0)
		return err;

	adapter->link_config.force_1000_base_x =
				(force_1000_base_x == 0) ? AL_FALSE : AL_TRUE;

	if (adapter->up)
		dev_warn(dev,
			 "%s this action will take place in the next activation (up)\n",
			  __func__);

	return len;
}

static ssize_t al_mod_eth_show_force_1000_base_x(struct device *dev,
				       struct device_attribute *attr, char *buf)
{
	struct al_mod_eth_adapter *adapter = dev_get_drvdata(dev);

	return sprintf(buf, "%d\n", adapter->link_config.force_1000_base_x);
}

static struct device_attribute dev_attr_force_1000_base_x = {
	.attr = {.name = "force_1000_base_x", .mode = (S_IRUGO | S_IWUSR)},
	.show = al_mod_eth_show_force_1000_base_x,
	.store = al_mod_eth_store_force_1000_base_x,
};

static ssize_t al_mod_eth_show_serdes_tx_param(struct device *dev,
					 struct device_attribute *attr,
					 char *buf)
{
	struct al_mod_eth_adapter *adapter = dev_get_drvdata(dev);
	struct dev_ext_attribute *ea = to_ext_attr(attr);
	uintptr_t offset = (uintptr_t)ea->var;
	struct al_mod_serdes_adv_tx_params	tx_params;
	uint8_t val;

	/* Guarantees lm_context is initialized */
	if (!adapter->lm_context.serdes_obj)
		return -EINVAL;

	al_mod_eth_lm_static_parameters_get(&adapter->lm_context,
					&tx_params,
					NULL);

	val = *(((uint8_t *)&tx_params) + offset);

	return sprintf(buf, "%d\n", val);
}

static ssize_t al_mod_eth_show_serdes_rx_param(struct device *dev,
					 struct device_attribute *attr,
					 char *buf)
{
	struct al_mod_eth_adapter *adapter = dev_get_drvdata(dev);
	struct dev_ext_attribute *ea = to_ext_attr(attr);
	uintptr_t offset = (uintptr_t)ea->var;
	struct al_mod_serdes_adv_rx_params	rx_params;
	uint8_t val;

	/* Guarantees lm_context is initialized */
	if (!adapter->lm_context.serdes_obj)
		return -EINVAL;

	al_mod_eth_lm_static_parameters_get(&adapter->lm_context,
					NULL,
					&rx_params);

	val = *(((uint8_t *)&rx_params) + offset);

	return sprintf(buf, "%d\n", val);
}

#define AL_ETH_SERDES_TX_PARAMS_ATTR(_name) {			\
	__ATTR(serdes_tx_##_name, 0444,				\
		al_mod_eth_show_serdes_tx_param, NULL),		\
	(void*)offsetof(struct al_mod_serdes_adv_tx_params, _name) }

#define AL_ETH_SERDES_RX_PARAMS_ATTR(_name) {			\
	__ATTR(serdes_rx_##_name, 0444,				\
		al_mod_eth_show_serdes_rx_param, NULL),		\
	(void*)offsetof(struct al_mod_serdes_adv_rx_params, _name) }

struct dev_ext_attribute dev_attr_serdes_params[] = {
	AL_ETH_SERDES_TX_PARAMS_ATTR(amp),
	AL_ETH_SERDES_TX_PARAMS_ATTR(total_driver_units),
	AL_ETH_SERDES_TX_PARAMS_ATTR(c_plus_1),
	AL_ETH_SERDES_TX_PARAMS_ATTR(c_plus_2),
	AL_ETH_SERDES_TX_PARAMS_ATTR(c_minus_1),
	AL_ETH_SERDES_TX_PARAMS_ATTR(slew_rate),
	AL_ETH_SERDES_RX_PARAMS_ATTR(dcgain),
	AL_ETH_SERDES_RX_PARAMS_ATTR(dfe_3db_freq),
	AL_ETH_SERDES_RX_PARAMS_ATTR(dfe_gain),
	AL_ETH_SERDES_RX_PARAMS_ATTR(dfe_first_tap_ctrl),
	AL_ETH_SERDES_RX_PARAMS_ATTR(dfe_secound_tap_ctrl),
	AL_ETH_SERDES_RX_PARAMS_ATTR(dfe_third_tap_ctrl),
	AL_ETH_SERDES_RX_PARAMS_ATTR(dfe_fourth_tap_ctrl),
	AL_ETH_SERDES_RX_PARAMS_ATTR(low_freq_agc_gain),
	AL_ETH_SERDES_RX_PARAMS_ATTR(precal_code_sel),
	AL_ETH_SERDES_RX_PARAMS_ATTR(high_freq_agc_boost)
};

static ssize_t al_mod_eth_store_max_rx_buff_alloc_size(struct device *dev,
					      struct device_attribute *attr,
					      const char *buf, size_t len)
{
	struct al_mod_eth_adapter *adapter = dev_get_drvdata(dev);
	unsigned long max_rx_buff_alloc_size;
	int err;

	err = kstrtoul(buf, 10, &max_rx_buff_alloc_size);
	if (err < 0)
		return err;

	adapter->max_rx_buff_alloc_size = max_rx_buff_alloc_size;

	return len;
}

static ssize_t al_mod_eth_show_max_rx_buff_alloc_size(struct device *dev,
				       struct device_attribute *attr, char *buf)
{
	struct al_mod_eth_adapter *adapter = dev_get_drvdata(dev);

	return sprintf(buf, "%d\n", adapter->max_rx_buff_alloc_size);
}

static struct device_attribute dev_attr_max_rx_buff_alloc_size = {
	.attr = {.name = "max_rx_buff_alloc_size", .mode = (S_IRUGO | S_IWUSR)},
	.show = al_mod_eth_show_max_rx_buff_alloc_size,
	.store = al_mod_eth_store_max_rx_buff_alloc_size,
};



#define UDMA_DUMP_PREP_ATTR(_name, _type) {\
        __ATTR(udma_dump_##_name, (S_IRUGO | S_IWUSR | S_IWGRP), rd_udma_dump, wr_udma_dump),\
        (void*)_type }

enum udma_dump_type {
	UDMA_DUMP_M2S_REGS,
	UDMA_DUMP_M2S_Q_STRUCT,
	UDMA_DUMP_M2S_Q_POINTERS,
	UDMA_DUMP_S2M_REGS,
	UDMA_DUMP_S2M_Q_STRUCT,
	UDMA_DUMP_S2M_Q_POINTERS
};

static ssize_t rd_udma_dump(
	struct device *dev,
	struct device_attribute *attr,
	char *buf);

static ssize_t wr_udma_dump(
	struct device *dev,
	struct device_attribute *attr,
	const char *buf,
	size_t count);


/* Device attrs - udma debug */
static struct dev_ext_attribute dev_attr_udma_debug[] = {
	UDMA_DUMP_PREP_ATTR(m2s_regs, UDMA_DUMP_M2S_REGS),
	UDMA_DUMP_PREP_ATTR(m2s_q_struct, UDMA_DUMP_M2S_Q_STRUCT),
	UDMA_DUMP_PREP_ATTR(m2s_q_pointers, UDMA_DUMP_M2S_Q_POINTERS),
	UDMA_DUMP_PREP_ATTR(s2m_regs, UDMA_DUMP_S2M_REGS),
	UDMA_DUMP_PREP_ATTR(s2m_q_struct, UDMA_DUMP_S2M_Q_STRUCT),
	UDMA_DUMP_PREP_ATTR(s2m_q_pointers, UDMA_DUMP_S2M_Q_POINTERS)
};

/* intr moderation */

extern void al_mod_eth_initial_moderation_table_restore_default(void);

static ssize_t al_mod_eth_store_intr_moderation_restore_default(struct device *dev,
					      struct device_attribute *attr,
					      const char *buf, size_t len)
{
	unsigned long intr_moderation_restore_default;
	int err;

	err = kstrtoul(buf, 10, &intr_moderation_restore_default);
	if (err < 0)
		return err;

	if (intr_moderation_restore_default)
		al_mod_eth_initial_moderation_table_restore_default();

	return len;
}

static struct device_attribute dev_attr_intr_moderation_restore_default = {
	.attr = {.name = "intr_moderation_restore_default", .mode = (S_IWUSR | S_IWGRP)},
	.show = NULL,
	.store = al_mod_eth_store_intr_moderation_restore_default,
};

static ssize_t al_mod_eth_show_intr_moderation(
	struct device *dev,
	struct device_attribute *attr,
	char *buf);

static ssize_t al_mod_eth_store_intr_moderation(
	struct device *dev,
	struct device_attribute *attr,
	const char *buf,
	size_t count);

#define INTR_MODERATION_PREPARE_ATTR(_name, _type) {				    \
	__ATTR(intr_moderation_##_name, (S_IRUGO | S_IWUSR | S_IWGRP),			    \
		al_mod_eth_show_intr_moderation, al_mod_eth_store_intr_moderation), \
		(void*)_type }

/* Device attrs - intr moderation */
static struct dev_ext_attribute dev_attr_intr_moderation[] = {
	INTR_MODERATION_PREPARE_ATTR(lowest, AL_ETH_INTR_MODERATION_LOWEST),
	INTR_MODERATION_PREPARE_ATTR(low, AL_ETH_INTR_MODERATION_LOW),
	INTR_MODERATION_PREPARE_ATTR(mid, AL_ETH_INTR_MODERATION_MID),
	INTR_MODERATION_PREPARE_ATTR(high, AL_ETH_INTR_MODERATION_HIGH),
	INTR_MODERATION_PREPARE_ATTR(highest, AL_ETH_INTR_MODERATION_HIGHEST),
};

static int al_mod_eth_sysfs_flow_steer_parse_args(struct device *dev, const char *buf, size_t len,
					      struct al_mod_eth_flow_steer_command *command)
{
	char *temp_str, *end, *arg;
	int rc = 0;
	u16 src_port, src_port_mask;
	int cnt;

	temp_str = kmalloc(len + 1, GFP_KERNEL);
	if (temp_str == NULL)
		return -ENOMEM;
	end = temp_str;

	strncpy(temp_str, buf, len + 1);

	arg = strsep(&temp_str, " ");
	rc = kstrtoint((const char *)arg, 10, (int *)&command->opcode);
	if (rc < 0)
		goto error;

	switch (command->opcode) {
	case AL_ETH_UDMA_OVERRIDE:
		rc = kstrtou8((const char *)temp_str, 10, &command->udma_mask);
		if (rc < 0)
			goto error;
		break;
	case AL_ETH_QUEUE_OVERRIDE:
		rc = kstrtoint((const char *)temp_str, 10, &command->qid);
		if (rc < 0)
			goto error;
		break;
		dev_warn(dev, "This opcode (%d) isn't implemented in this driver version\n",
			command->opcode);
		break;
	case AL_ETH_FLOW_STEER_ADD:
		cnt = sscanf((const char *)temp_str, "%hhu %hu %hu %hu %hu %hhu %hhd %u",
			&command->ip_flow_char.protocol_type,
			&src_port,
			&src_port_mask,
			&command->ip_flow_char.dst_port,
			&command->ip_flow_char.dst_port_mask,
			&command->action.udma_mask,
			&command->action.queue_num,
			&command->action.icrc_hw_enable);
		if (cnt < 8) {
			dev_err(dev, "Not enough arguments to flow_steer_addd command\n");
			rc = -EINVAL;
			goto error;
		}

		if (command->ip_flow_char.protocol_type != IPPROTO_UDP) {
			dev_err(dev, "Currently only UDP (17) protocol type supported\n");
			rc = -EPROTONOSUPPORT;
			goto error;
		}

		if ((src_port != 0) || (src_port_mask != 0))
			dev_warn(dev, "Flow steering according to src_port isn't currently supported, port and mask are ignored\n");

		if ((command->action.icrc_hw_enable != AL_TRUE) &&
			(command->action.icrc_hw_enable != AL_FALSE)) {
			dev_err(dev, "Invalid value for icrc_hw_enable (%u)! Please re-enter the rule with 0 or 1.\n",
				command->action.icrc_hw_enable);
			rc = -EINVAL;
			goto error;
		}

		dev_dbg(dev, "opcode=%d, proto_type=%d, src_port=%d, src_port_mask=0x%04x\n"
			"dst_port=%d, dst_port_mask=0x%04x, udma_mask=0x%08x, queue_num=%d\n,"
			"icrc_hw_enable=%u\n",
			command->opcode,
			command->ip_flow_char.protocol_type,
			src_port,
			src_port_mask,
			command->ip_flow_char.dst_port,
			command->ip_flow_char.dst_port_mask,
			command->action.udma_mask,
			command->action.queue_num,
			command->action.icrc_hw_enable);

		break;
	case AL_ETH_FLOW_STEER_RESET_VF:
		rc = kstrtouint(temp_str, 10, &command->udma_num);
		dev_dbg(dev, "command->opcode:%d, command->udma_num:%u, rc=%d\n",
			command->opcode, command->udma_num, rc);

		break;
	default:
		dev_err(dev, "Invalid OPCODE for RX flow_steering_config (%d)\n", command->opcode);
	}

error:
	kfree(end);

	return rc;
}

extern int al_mod_eth_flow_steer_config(struct al_mod_eth_adapter *adapter,
				    struct al_mod_eth_flow_steer_command *command);

static ssize_t al_mod_eth_store_flow_steer_config(struct device *dev,
					      struct device_attribute *attr,
					      const char *buf, size_t len)
{
	struct al_mod_eth_adapter *adapter = dev_get_drvdata(dev);
	struct al_mod_eth_flow_steer_command flow_steer_command = {0};
	int err;

	err = al_mod_eth_sysfs_flow_steer_parse_args(dev, buf, len, &flow_steer_command);
	if (err) {
		netdev_err(adapter->netdev, "Error when parsing arguments to flow_steer_config\n");
		return err;
	}

	err = al_mod_eth_flow_steer_config(adapter, &flow_steer_command);
	if (err) {
		netdev_err(adapter->netdev, "Failed to configure flow_steering\n");
		return err;
	}

	return len;
}

static const char flow_steer_help[] = {"Format: OPCODE <opcode_specific_delimited_by_spaces>\n\n"
				       "\tOPCODE 0 (UDMA_OVERRIDE) - udma_mask\n"
				       "\tOPCODE 1 (QUEUE_OVERRIDE) - queue_id\n"
				       "\tOPCODE 2 (FLOW_STEER_ADD) - ip_protocol_type src_port src_port_mask dst_port dst_port_mask dst_udma_mask dst_queue_id icrc_enable\n"
				       "\tOPCODE 3 (RESET_FLOW_STEER_PER_UDMA) - udma_num\n"
				       "NOTE: OPCODE 2 currently supports only UDP (ip_protocol_type=17) and steering according to dst_port only (src_port and maska re ignored)\n"
				       "      All values should be decimal\n"};

static ssize_t al_mod_eth_show_flow_steer_config(struct device *dev,
					     struct device_attribute *attr, char *buf)
{
	struct al_mod_eth_adapter *adapter = dev_get_drvdata(dev);

	return scnprintf(buf, PAGE_SIZE, "UDMA override mask:0x%hhx\nQueue override id:%d\n" "%s",
			 adapter->udma_steer_mask, adapter->queue_steer_id, flow_steer_help);
}

static struct device_attribute dev_attr_flow_steer_config = {
	.attr = {.name = "flow_steer_config", .mode = (S_IRUGO | S_IWUSR)},
	.show = al_mod_eth_show_flow_steer_config,
	.store = al_mod_eth_store_flow_steer_config,
};

static void al_mod_eth_epe_table_dump(struct al_mod_eth_adapter *adapter)
{
	uint32_t i;
	struct al_mod_eth_epe_p_reg_entry reg_entry;
	struct al_mod_eth_epe_control_entry control_entry;
	struct al_mod_hal_eth_adapter *hal_adapter = &adapter->hal_adapter;

	netdev_info(adapter->netdev, "EPE entry table:\n");
	for (i = 0; i < AL_ETH_PROTOCOLS_NUM; i++) {
		reg_entry.data = al_mod_reg_read32(&hal_adapter->ec_regs_base->epe_p[i].comp_data);
		reg_entry.mask = al_mod_reg_read32(&hal_adapter->ec_regs_base->epe_p[i].comp_mask);
		reg_entry.ctrl = al_mod_reg_read32(&hal_adapter->ec_regs_base->epe_p[i].comp_ctrl);

		netdev_info(adapter->netdev, "%d - data 0x%08x, mask 0x%08x, ctrl 0x%08x\n",
			i,
			reg_entry.data,
			reg_entry.mask,
			reg_entry.ctrl);
	}

	netdev_info(adapter->netdev, "EPE action table 0:\n");
	for (i = 0; i < AL_ETH_PROTOCOLS_NUM; i++) {
		al_mod_reg_write32(&hal_adapter->ec_regs_base->epe[0].act_table_addr, i);
		control_entry.data[0] =
			al_mod_reg_read32(&hal_adapter->ec_regs_base->epe[0].act_table_data_1);
		control_entry.data[1] =
			al_mod_reg_read32(&hal_adapter->ec_regs_base->epe[0].act_table_data_2);
		control_entry.data[2] =
			al_mod_reg_read32(&hal_adapter->ec_regs_base->epe[0].act_table_data_3);
		control_entry.data[3] =
			al_mod_reg_read32(&hal_adapter->ec_regs_base->epe[0].act_table_data_4);
		control_entry.data[4] =
			al_mod_reg_read32(&hal_adapter->ec_regs_base->epe[0].act_table_data_5);
		control_entry.data[5] =
			al_mod_reg_read32(&hal_adapter->ec_regs_base->epe[0].act_table_data_6);

		netdev_info(adapter->netdev, "%d - act_data_1 0x%08x, act_data_2 0x%08x, act_data_3 0x%08x, act_data_4 0x%08x, act_data_5 0x%08x, act_data_6 0x%08x\n",
			    i,
			    control_entry.data[0],
			    control_entry.data[1],
			    control_entry.data[2],
			    control_entry.data[3],
			    control_entry.data[4],
			    control_entry.data[5]);
	}
}

static ssize_t al_mod_eth_show_epe_tables_dump(struct device *dev,
					   struct device_attribute *attr, char *buf)
{
	return scnprintf(buf, PAGE_SIZE, "echo 1 to dump EPE (parsing engine) tables\n");
}

static ssize_t al_mod_eth_store_epe_tables_dump(struct device *dev,
					    struct device_attribute *attr,
					    const char *buf, size_t len)
{
	struct al_mod_eth_adapter *adapter = dev_get_drvdata(dev);
	unsigned int dump = 0;
	int err;

	err = kstrtouint(buf, 10, &dump);
	if (err < 0)
		return err;

	if (dump == 1)
		al_mod_eth_epe_table_dump(adapter);

	return len;
}

static struct device_attribute dev_attr_epe_tables_dump = {
	.attr = {.name = "epe_tables_dump", .mode = (S_IRUGO | S_IWUSR)},
	.show = al_mod_eth_show_epe_tables_dump,
	.store = al_mod_eth_store_epe_tables_dump,
};

static void al_mod_eth_rfw_control_table_dump(struct al_mod_eth_adapter *adapter)
{
	uint32_t val;
	uint32_t i;
	struct al_mod_hal_eth_adapter *hal_adapter = &adapter->hal_adapter;

	netdev_info(adapter->netdev, "rfw.ctrl_table entries:\n");
	for (i = 0; i < AL_ETH_RX_CTRL_TABLE_SIZE; i++) {
		al_mod_reg_write32(&hal_adapter->ec_regs_base->rfw.ctrl_table_addr, i);
		val = al_mod_reg_read32(&hal_adapter->ec_regs_base->rfw.ctrl_table_data);
		netdev_info(adapter->netdev, "%d - l3_proto_idx %lu - prio_sel[3:0] 0x%lx, q_sel_1[7:4] 0x%lx, q_sel2[9:8] 0x%lx, udma_sel[13:10] 0x%lx, hdr_split_len_sel[17:15] 0x%lx, hdr_split_ctrl[18] %lx, drop[19] %lx\n",
			i,
			AL_REG_FIELD_GET(i, AL_FIELD_MASK(8, 4), 4),
			AL_REG_FIELD_GET(val, AL_FIELD_MASK(3, 0), 0),
			AL_REG_FIELD_GET(val, AL_FIELD_MASK(7, 4), 4),
			AL_REG_FIELD_GET(val, AL_FIELD_MASK(9, 8), 8),
			AL_REG_FIELD_GET(val, AL_FIELD_MASK(13, 10), 10),
			/* TODO: bit 14 is listed as part of udma_sel but not used */
			AL_REG_FIELD_GET(val,  AL_FIELD_MASK(17, 15), 15),
			AL_REG_FIELD_GET(val,  AL_FIELD_MASK(18, 18), 18),
			AL_REG_FIELD_GET(val, AL_FIELD_MASK(19, 19), 19));
	}

	netdev_info(adapter->netdev, "rfw_default registers:\n");
	for (i = 0; i < 8; i++) {/* TODO: add define for this in HAL */
		val = al_mod_reg_read32(&hal_adapter->ec_regs_base->rfw_default[i].opt_1);

		netdev_info(adapter->netdev, "%d - 0x%08x\n", i, val);
	}
}

static ssize_t al_mod_eth_show_rfw_control_table_dump(struct device *dev,
						  struct device_attribute *attr, char *buf)
{
	return scnprintf(buf, PAGE_SIZE,
			"echo 1 to dump RX forwarding control table and rfw_default registers\n");
}

static ssize_t al_mod_eth_store_rfw_control_table_dump(struct device *dev,
						   struct device_attribute *attr,
						   const char *buf, size_t len)
{
	struct al_mod_eth_adapter *adapter = dev_get_drvdata(dev);
	unsigned int dump = 0;
	int err;

	err = kstrtouint(buf, 10, &dump);
	if (err < 0)
		return err;

	if (dump == 1)
		al_mod_eth_rfw_control_table_dump(adapter);

	return len;
}

static struct device_attribute dev_attr_rfw_control_table_dump = {
	.attr = {.name = "rfw_control_table_dump", .mode = (S_IRUGO | S_IWUSR)},
	.show = al_mod_eth_show_rfw_control_table_dump,
	.store = al_mod_eth_store_rfw_control_table_dump,
};

static int al_mod_eth_crc_tables_dump(struct al_mod_eth_adapter *adapter)
{
	int i;
	struct al_mod_eth_tx_gpd_cam_entry tx_gpd_entry = {0};
	struct al_mod_eth_tx_gcp_table_entry tx_gcp_entry = {0};
	struct al_mod_eth_tx_crc_chksum_replace_cmd_for_protocol_num_entry tx_replace_entry = {0};
	struct al_mod_hal_eth_adapter *hal_adapter = &adapter->hal_adapter;
	struct al_mod_eth_rx_gpd_cam_entry rx_gpd_entry = {0};
	struct al_mod_eth_rx_gcp_table_entry rx_gcp_entry = {0};

	netdev_info(adapter->netdev, "TX GPD Table:\n");
	for (i = 0; i < 32; i++) {
		netdev_info(adapter->netdev, "Entry idx: %d\n", i);
		al_mod_eth_tx_protocol_detect_table_entry_get(hal_adapter, i, &tx_gpd_entry);
		netdev_info(adapter->netdev, "%u, %u, %u, %u, %u\n0x%x, 0x%x, 0x%x, 0x%x\n",
			tx_gpd_entry.l3_proto_idx,
			tx_gpd_entry.l4_proto_idx,
			tx_gpd_entry.tunnel_control,
			tx_gpd_entry.source_vlan_count,
			tx_gpd_entry.tx_gpd_cam_ctrl,
			tx_gpd_entry.l3_proto_idx_mask,
			tx_gpd_entry.l4_proto_idx_mask,
			tx_gpd_entry.tunnel_control_mask,
			tx_gpd_entry.source_vlan_count_mask);
	}

	netdev_info(adapter->netdev, "TX GCP Table:\n");
	for (i = 0; i < 32; i++) {
		netdev_info(adapter->netdev, "Entry idx: %d\n", i);
		al_mod_eth_tx_generic_crc_table_entry_get(hal_adapter, 0, i, &tx_gcp_entry);
		netdev_info(adapter->netdev, "%u, %u, %u, %u, %u\n"
			"%u, %u, %u, %u, %u\n"
			"%u, %u, %u, %u, %u\n"
			"%u, %u, {0x%08x, 0x%08x, 0x%08x,\n"
			"0x%08x, 0x%08x, 0x%08x}, 0x%08x, 0x%x\n"
			"%u}\n",
			tx_gcp_entry.poly_sel,
			tx_gcp_entry.crc32_bit_comp,
			tx_gcp_entry.crc32_bit_swap,
			tx_gcp_entry.crc32_byte_swap,
			tx_gcp_entry.data_bit_swap,
			tx_gcp_entry.data_byte_swap,
			tx_gcp_entry.trail_size,
			tx_gcp_entry.head_size,
			tx_gcp_entry.head_calc,
			tx_gcp_entry.mask_polarity,
			tx_gcp_entry.tx_alu_opcode_1,
			tx_gcp_entry.tx_alu_opcode_2,
			tx_gcp_entry.tx_alu_opcode_3,
			tx_gcp_entry.tx_alu_opsel_1,
			tx_gcp_entry.tx_alu_opsel_2,
			tx_gcp_entry.tx_alu_opsel_3,
			tx_gcp_entry.tx_alu_opsel_4,
			tx_gcp_entry.gcp_mask[0],
			tx_gcp_entry.gcp_mask[1],
			tx_gcp_entry.gcp_mask[2],
			tx_gcp_entry.gcp_mask[3],
			tx_gcp_entry.gcp_mask[4],
			tx_gcp_entry.gcp_mask[5],
			tx_gcp_entry.crc_init,
			tx_gcp_entry.gcp_table_res,
			tx_gcp_entry.alu_val);
	}

	netdev_info(adapter->netdev, "TX CRC_CHKSUM_REPLACE Table:\n");
	for (i = 0; i < 32; i++) {
		al_mod_eth_tx_crc_chksum_replace_cmd_entry_get(hal_adapter, i, &tx_replace_entry);
		netdev_info(adapter->netdev, "Entry idx: %d\n", i);
		netdev_info(adapter->netdev, "%u,%u,%u,%u,\t\t%u,%u,%u,%u,\t\t%u,%u,%u,%u\n",
			tx_replace_entry.crc_en_00,
			tx_replace_entry.crc_en_01,
			tx_replace_entry.crc_en_10,
			tx_replace_entry.crc_en_11,
			tx_replace_entry.l4_csum_en_00,
			tx_replace_entry.l4_csum_en_01,
			tx_replace_entry.l4_csum_en_10,
			tx_replace_entry.l4_csum_en_11,
			tx_replace_entry.l3_csum_en_00,
			tx_replace_entry.l3_csum_en_01,
			tx_replace_entry.l3_csum_en_10,
			tx_replace_entry.l3_csum_en_11);
	}

	netdev_info(adapter->netdev, "RX GPD Table:\n");
	for (i = 0; i < AL_ETH_RX_GPD_TABLE_SIZE; i++) {
		netdev_info(adapter->netdev, "Entry idx: %d\n", i);
		al_mod_eth_rx_protocol_detect_table_entry_get(hal_adapter, i, &rx_gpd_entry);
		netdev_info(adapter->netdev, "{%u, %u, %u, %u,\n%u, %u, %u, %u, %u,\n0x%x, 0x%x, 0x%x, 0x%x,\n0x%x, 0x%x, 0x%x, 0x%x}\n",
			rx_gpd_entry.outer_l3_proto_idx,
			rx_gpd_entry.outer_l4_proto_idx,
			rx_gpd_entry.inner_l3_proto_idx,
			rx_gpd_entry.inner_l4_proto_idx,
			rx_gpd_entry.parse_ctrl,
			rx_gpd_entry.outer_l3_len,
			rx_gpd_entry.l3_priority,
			rx_gpd_entry.l4_dst_port_lsb,
			rx_gpd_entry.rx_gpd_cam_ctrl,
			rx_gpd_entry.outer_l3_proto_idx_mask,
			rx_gpd_entry.outer_l4_proto_idx_mask,
			rx_gpd_entry.inner_l3_proto_idx_mask,
			rx_gpd_entry.inner_l4_proto_idx_mask,
			rx_gpd_entry.parse_ctrl_mask,
			rx_gpd_entry.outer_l3_len_mask,
			rx_gpd_entry.l3_priority_mask,
			rx_gpd_entry.l4_dst_port_lsb_mask);
	}

	netdev_info(adapter->netdev, "RX GCP Table:\n");
	for (i = 0; i < 32; i++) {
		netdev_info(adapter->netdev, "Entry idx: %d\n", i);
		al_mod_eth_rx_generic_crc_table_entry_get(hal_adapter, i, &rx_gcp_entry);
		netdev_info(adapter->netdev, "%u, %u, %u, %u, %u\n"
			"%u, %u, %u, %u, %u\n"
			"%u, %u, %u, %u, %u\n"
			"%u, %u, {0x%08x, 0x%08x, 0x%08x,\n"
			"0x%08x, 0x%08x, 0x%08x}, 0x%08x, 0x%08x\n"
			"%u}\n",
			rx_gcp_entry.poly_sel,
			rx_gcp_entry.crc32_bit_comp,
			rx_gcp_entry.crc32_bit_swap,
			rx_gcp_entry.crc32_byte_swap,
			rx_gcp_entry.data_bit_swap,
			rx_gcp_entry.data_byte_swap,
			rx_gcp_entry.trail_size,
			rx_gcp_entry.head_size,
			rx_gcp_entry.head_calc,
			rx_gcp_entry.mask_polarity,
			rx_gcp_entry.rx_alu_opcode_1,
			rx_gcp_entry.rx_alu_opcode_2,
			rx_gcp_entry.rx_alu_opcode_3,
			rx_gcp_entry.rx_alu_opsel_1,
			rx_gcp_entry.rx_alu_opsel_2,
			rx_gcp_entry.rx_alu_opsel_3,
			rx_gcp_entry.rx_alu_opsel_4,
			rx_gcp_entry.gcp_mask[0],
			rx_gcp_entry.gcp_mask[1],
			rx_gcp_entry.gcp_mask[2],
			rx_gcp_entry.gcp_mask[3],
			rx_gcp_entry.gcp_mask[4],
			rx_gcp_entry.gcp_mask[5],
			rx_gcp_entry.crc_init,
			rx_gcp_entry.gcp_table_res,
			rx_gcp_entry.alu_val);
	}


	return 0;
}

static ssize_t al_mod_eth_show_crc_tables_dump(struct device *dev,
						  struct device_attribute *attr, char *buf)
{
	return scnprintf(buf, PAGE_SIZE, "echo 1 to this file to dump CRC GPD/GCP tables\n");
}

static ssize_t al_mod_eth_store_crc_tables_dump(struct device *dev,
						   struct device_attribute *attr,
						   const char *buf, size_t len)
{
	struct al_mod_eth_adapter *adapter = dev_get_drvdata(dev);
	unsigned int dump = 0;
	int err;

	if (adapter->rev_id >= AL_ETH_REV_ID_4) {
		netdev_err(adapter->netdev, "%s: Not supported on rev id %d\n",
			__func__, adapter->rev_id);
		return len;
	}

	err = kstrtouint(buf, 10, &dump);
	if (err < 0)
		return err;

	if (dump == 1)
		al_mod_eth_crc_tables_dump(adapter);

	return len;
}

static struct device_attribute dev_attr_crc_tables_dump = {
	.attr = {.name = "crc_tables_dump", .mode = (S_IRUGO | S_IWUSR)},
	.show = al_mod_eth_show_crc_tables_dump,
	.store = al_mod_eth_store_crc_tables_dump,
};

static void al_mod_eth_tx_metacache_table_dump(struct al_mod_eth_adapter *adapter)
{
	unsigned int i;
	uint32_t data[4];
	struct al_mod_hal_eth_adapter *hal_adapter = &adapter->hal_adapter;

	netdev_info(adapter->netdev, "TX Metacache Table:\n");
	for (i = 0; i < 32; i++) {
		netdev_info(adapter->netdev, "Entry idx: %d\n", i);

		al_mod_eth_tso_metacahe_line_dump(hal_adapter, i, data);

		netdev_info(adapter->netdev, "cache_table_data_1:0x%08x\n", data[0]);
		netdev_info(adapter->netdev, "cache_table_data_2:0x%08x\n", data[1]);
		netdev_info(adapter->netdev, "cache_table_data_3:0x%08x\n", data[2]);
		netdev_info(adapter->netdev, "cache_table_data_4:0x%08x\n", data[3]);
	}
}

static ssize_t al_mod_eth_show_tx_metacache_table_dump(struct device *dev,
						  struct device_attribute *attr, char *buf)
{
	return scnprintf(buf, PAGE_SIZE, "echo 1 to this file to dump TX Metadata Cache table\n");
}

static ssize_t al_mod_eth_store_tx_metacache_table_dump(struct device *dev,
						   struct device_attribute *attr,
						   const char *buf, size_t len)
{
	struct al_mod_eth_adapter *adapter = dev_get_drvdata(dev);
	unsigned int dump = 0;
	int err;

	err = kstrtouint(buf, 10, &dump);
	if (err < 0)
		return err;

	if (dump == 1)
		al_mod_eth_tx_metacache_table_dump(adapter);

	return len;
}

static struct device_attribute dev_attr_tx_metacache_table_dump = {
	.attr = {.name = "tx_metacache_table_dump", .mode = (S_IRUGO | S_IWUSR)},
	.show = al_mod_eth_show_tx_metacache_table_dump,
	.store = al_mod_eth_store_tx_metacache_table_dump,
};

static ssize_t al_mod_eth_show_errors(struct device *dev,
				  struct device_attribute *attr, char *buf,
				  enum al_mod_err_events_sec_module sec_module)
{
	struct al_mod_eth_adapter *adapter = dev_get_drvdata(dev);
	ssize_t written = 0;
	struct al_mod_err_events_module *m;

	for (m = adapter->err_events_handle.first_comp; m; m = m->next_comp) {
		struct al_mod_err_events_field *f;
		int i;

		if (m->secondary_module != sec_module)
			continue;

		for (f = m->fields, i = 0; i < m->fields_size; f++, i++) {
			if (!f->valid)
				continue;

			written += scnprintf(buf + written, PAGE_SIZE - written,
					     "%s: %d\n", f->name, f->counter);
		}
	}

	written += scnprintf(buf + written, PAGE_SIZE - written,
			     "echo 1 to this file to clear all stats\n");

	return written;
}

static ssize_t al_mod_eth_clear_errors(struct al_mod_eth_adapter *adapter,
				   struct device_attribute *attr,
				   const char *buf, size_t len,
				   enum al_mod_err_events_sec_module sec_module,
				   u64 *counter)
{
	unsigned int clear = 0;
	int err;
	struct al_mod_err_events_module *m;

	err = kstrtouint(buf, 10, &clear);
	if (err < 0)
		return err;

	if (clear != 1)
		return -EINVAL;

	for (m = adapter->err_events_handle.first_comp; m; m = m->next_comp) {
		if (m->secondary_module != sec_module)
			continue;

		al_mod_err_events_module_clear(m);
	}

	/* Protect both counter and syncp */
	spin_lock_irq(&adapter->stats_lock);

	u64_stats_update_begin(&adapter->syncp);
	(*counter) = 0;
	u64_stats_update_end(&adapter->syncp);

	spin_unlock_irq(&adapter->stats_lock);
	return len;
}

static ssize_t al_mod_eth_show_udma_errors(struct device *dev,
				       struct device_attribute *attr, char *buf)
{
	return al_mod_eth_show_errors(dev, attr, buf,
				  AL_ERR_EVENTS_SEC_MODULE_UDMA);
}

static ssize_t al_mod_eth_store_udma_errors(struct device *dev,
					struct device_attribute *attr,
					const char *buf, size_t len)
{
	struct al_mod_eth_adapter *adapter = dev_get_drvdata(dev);

	return al_mod_eth_clear_errors(adapter, attr, buf, len,
				   AL_ERR_EVENTS_SEC_MODULE_UDMA,
				   &adapter->dev_stats.udma_errors);
}

static struct device_attribute dev_attr_udma_errors = {
	.attr = {.name = "udma_errors", .mode = (S_IRUGO | S_IWUSR)},
	.show = al_mod_eth_show_udma_errors,
	.store = al_mod_eth_store_udma_errors,
};

static ssize_t al_mod_eth_show_ec_errors(struct device *dev,
				      struct device_attribute *attr, char *buf)
{
	return al_mod_eth_show_errors(dev, attr, buf,
				  AL_ERR_EVENTS_SEC_MODULE_ETH_EC);
}

static ssize_t al_mod_eth_store_ec_errors(struct device *dev,
				      struct device_attribute *attr,
				      const char *buf, size_t len)
{
	struct al_mod_eth_adapter *adapter = dev_get_drvdata(dev);

	return al_mod_eth_clear_errors(adapter, attr, buf, len,
				   AL_ERR_EVENTS_SEC_MODULE_ETH_EC,
				   &adapter->dev_stats.ec_errors);
}

static struct device_attribute dev_attr_ec_errors = {
	.attr = {.name = "ec_errors", .mode = (S_IRUGO | S_IWUSR)},
	.show = al_mod_eth_show_ec_errors,
	.store = al_mod_eth_store_ec_errors,
};

static ssize_t al_mod_eth_show_mac_errors(struct device *dev,
				      struct device_attribute *attr, char *buf)
{
	return al_mod_eth_show_errors(dev, attr, buf,
				  AL_ERR_EVENTS_SEC_MODULE_ETH_MAC);
}

static ssize_t al_mod_eth_store_mac_errors(struct device *dev,
				       struct device_attribute *attr,
				       const char *buf, size_t len)
{
	struct al_mod_eth_adapter *adapter = dev_get_drvdata(dev);

	return al_mod_eth_clear_errors(adapter, attr, buf, len,
				   AL_ERR_EVENTS_SEC_MODULE_ETH_MAC,
				   &adapter->dev_stats.mac_errors);
}

static struct device_attribute dev_attr_mac_errors = {
	.attr = {.name = "mac_errors", .mode = (S_IRUGO | S_IWUSR)},
	.show = al_mod_eth_show_mac_errors,
	.store = al_mod_eth_store_mac_errors,
};

static ssize_t al_mod_eth_show_ua_errors(struct device *dev,
				     struct device_attribute *attr, char *buf)
{
	return al_mod_eth_show_errors(dev, attr, buf,
				  AL_ERR_EVENTS_SEC_MODULE_UNIT_ADAPTER);
}

static ssize_t al_mod_eth_store_ua_errors(struct device *dev,
				      struct device_attribute *attr,
				      const char *buf, size_t len)
{
	struct al_mod_eth_adapter *adapter = dev_get_drvdata(dev);

	return al_mod_eth_clear_errors(adapter, attr, buf, len,
				   AL_ERR_EVENTS_SEC_MODULE_UNIT_ADAPTER,
				   &adapter->dev_stats.ua_errors);
}

static struct device_attribute dev_attr_ua_errors = {
	.attr = {.name = "ua_errors", .mode = (S_IRUGO | S_IWUSR)},
	.show = al_mod_eth_show_ua_errors,
	.store = al_mod_eth_store_ua_errors,
};

static ssize_t al_mod_eth_show_err_events_active(struct device *dev,
					     struct device_attribute *attr,
					     char *buf)
{
	struct al_mod_eth_adapter *adapter = dev_get_drvdata(dev);

	return sprintf(buf, "%d\n", !!adapter->eth_error_ready);
}

static struct device_attribute dev_attr_err_events_active = {
	.attr = {.name = "err_events_active", .mode = 0444},
	.show = al_mod_eth_show_err_events_active,
};

#ifdef CONFIG_AL_ETH_ERR_EVENTS_TEST
void al_mod_eth_err_events_self_test(struct al_mod_eth_adapter *adapter);

static ssize_t al_mod_eth_show_err_events_test(struct device *dev,
					   struct device_attribute *attr,
					   char *buf)
{
	return scnprintf(buf, PAGE_SIZE,
			 "echo 1 to this file to run self test\n"
			 "NOTE: make sure to run on the same cpu as "
			 "the al_eth mgmt irq\n");
}

#define AL_ETH_ERR_EVENTS_SELF_TEST_RUN	1
static ssize_t al_mod_eth_store_err_events_test(struct device *dev,
					    struct device_attribute *attr,
					    const char *buf, size_t len)
{
	struct al_mod_eth_adapter *adapter = dev_get_drvdata(dev);
	unsigned int cmd;
	int err;

	err = kstrtouint(buf, 10, &cmd);
	if (err < 0)
		return err;

	switch (cmd) {
	case AL_ETH_ERR_EVENTS_SELF_TEST_RUN:
		al_mod_eth_err_events_self_test(adapter);
		break;
	default:
		return -EINVAL;
	}

	return len;
}

static struct device_attribute dev_attr_err_events_test = {
	.attr = {.name = "err_events_test", .mode = (S_IRUGO | S_IWUSR)},
	.show = al_mod_eth_show_err_events_test,
	.store = al_mod_eth_store_err_events_test,
};
#endif

#ifdef CONFIG_ARCH_ALPINE

extern int al_mod_eth_default_rx_udma_num_set(struct al_mod_eth_adapter *adapter, unsigned int rx_udma_num);

static ssize_t al_mod_eth_show_default_rx_udma_num(struct device *dev,
					       struct device_attribute *attr, char *buf)
{
	struct al_mod_eth_adapter *adapter = dev_get_drvdata(dev);

	if ((adapter->dev_id == AL_ETH_DEV_ID_ADVANCED) && (adapter->rev_id == AL_ETH_REV_ID_3))
		return scnprintf(buf,
				 PAGE_SIZE,
				"Default UDMA is set to: %u. echo 0-%u to this file to change\n",
				 adapter->default_rx_udma_num,
				 AL_ETH_ADV_REV_ID_3_UDMA_NUM - 1);

	return scnprintf(buf, PAGE_SIZE, "This feature is not supported for this device\n");
}

static ssize_t al_mod_eth_store_default_rx_udma_num(struct device *dev,
						struct device_attribute *attr,
						const char *buf, size_t len)
{
	struct al_mod_eth_adapter *adapter = dev_get_drvdata(dev);
	unsigned int new_rx_udma_num;
	int err;
	int rc;

	err = kstrtouint(buf, 10, &new_rx_udma_num);
	if (err < 0)
		return err;

	if (new_rx_udma_num != adapter->default_rx_udma_num) {
		rc = al_mod_eth_default_rx_udma_num_set(adapter, new_rx_udma_num);
		if (rc)
			return -EINVAL;
	}

	return len;
}

static struct device_attribute dev_attr_default_rx_udma_num = {
	.attr = {.name = "default_rx_udma_num", .mode = (S_IRUGO | S_IWUSR)},
	.show = al_mod_eth_show_default_rx_udma_num,
	.store = al_mod_eth_store_default_rx_udma_num,
};
#endif /* CONFIG_ARCH_ALPINE */
/******************************************************************************
 *****************************************************************************/
int al_mod_eth_sysfs_init(
	struct device *dev)
{
	struct al_mod_eth_adapter *adapter = dev_get_drvdata(dev);
	int status = 0;

	int i;

	if (device_create_file(dev, &dev_attr_small_copy_len))
		dev_info(dev, "failed to create small_copy_len sysfs entry");

	if (device_create_file(dev, &dev_attr_link_poll_interval))
		dev_info(dev, "failed to create link_poll_interval sysfs entry");

#ifdef CONFIG_ARCH_ALPINE
	if (device_create_file(dev, &dev_attr_link_management_debug))
		dev_info(dev, "failed to create link_management_debug sysfs entry");
#endif /* CONFIG_ARCH_ALPINE */

	if (device_create_file(dev, &dev_attr_max_rx_buff_alloc_size))
		dev_info(dev, "failed to create max_rx_buff_alloc_size sysfs entry");

	if (device_create_file(dev, &dev_attr_link_training_enable))
		dev_info(dev, "failed to create link_training_enable sysfs entry");

	if (device_create_file(dev, &dev_attr_sfp_probe_1g))
		dev_info(dev, "failed to create sfp_probe_1g sysfs entry");

	if (device_create_file(dev, &dev_attr_sfp_probe_10g))
		dev_info(dev, "failed to create sfp_probe_10g sysfs entry");

	if (device_create_file(dev, &dev_attr_force_1000_base_x))
		dev_info(dev, "failed to create force_1000_base_x sysfs entry");

	if (device_create_file(dev, &dev_attr_intr_moderation_restore_default))
		dev_info(dev, "failed to create intr_moderation_restore_default");

	if (device_create_file(dev, &dev_attr_flow_steer_config))
		dev_info(dev, "failed to create %s", dev_attr_flow_steer_config.attr.name);

	if (device_create_file(dev, &dev_attr_epe_tables_dump))
		dev_info(dev, "failed to create %s", dev_attr_epe_tables_dump.attr.name);

	if (device_create_file(dev, &dev_attr_rfw_control_table_dump))
		dev_info(dev, "failed to create %s", dev_attr_rfw_control_table_dump.attr.name);

	if (device_create_file(dev, &dev_attr_crc_tables_dump))
		dev_info(dev, "failed to create %s", dev_attr_crc_tables_dump.attr.name);

	if (device_create_file(dev, &dev_attr_tx_metacache_table_dump))
		dev_info(dev, "failed to create %s", dev_attr_tx_metacache_table_dump.attr.name);

	if (adapter->err_events_enabled) {
		if (device_create_file(dev, &dev_attr_udma_errors)) {
			dev_info(dev, "failed to create %s",
				 dev_attr_udma_errors.attr.name);
		}

		if (device_create_file(dev, &dev_attr_ec_errors)) {
			dev_info(dev, "failed to create %s",
				 dev_attr_ec_errors.attr.name);
		}

		if (device_create_file(dev, &dev_attr_mac_errors)) {
			dev_info(dev, "failed to create %s",
				 dev_attr_mac_errors.attr.name);
		}

		if (device_create_file(dev, &dev_attr_ua_errors)) {
			dev_info(dev, "failed to create %s",
				 dev_attr_ua_errors.attr.name);
		}

		if (device_create_file(dev, &dev_attr_err_events_active)) {
			dev_info(dev, "failed to create %s",
				 dev_attr_err_events_active.attr.name);
		}
#ifdef CONFIG_AL_ETH_ERR_EVENTS_TEST
		if (device_create_file(dev, &dev_attr_err_events_test)) {
			dev_info(dev, "failed to create %s",
				 dev_attr_err_events_test.attr.name);
		}
#endif
	}

#ifdef CONFIG_ARCH_ALPINE
	if (device_create_file(dev, &dev_attr_default_rx_udma_num))
		dev_info(dev, "failed to create %s", dev_attr_default_rx_udma_num.attr.name);
#endif /* CONFIG_ARCH_ALPINE */

	if ((adapter->board_type == ALPINE_INTEGRATED) &&
	    (adapter->use_lm) &&
	    (adapter->al_mod_chip_id == PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V2)) {
		for (i = 0; i < ARRAY_SIZE(dev_attr_serdes_params); i++) {
			status = sysfs_create_file(&dev->kobj,
						   &dev_attr_serdes_params[i].attr.attr);
			if (status) {
				dev_err(dev, "%s: sysfs_create_file(serdes_params %d) failed\n",
					__func__, i);
				goto done;
			}
		}
	}

	for (i = 0; i < ARRAY_SIZE(dev_attr_udma_debug); i++ ) {
		status = sysfs_create_file(
			&dev->kobj,
			&dev_attr_udma_debug[i].attr.attr);
		if (status) {
			dev_err(
				dev,
				"%s: sysfs_create_file(stats_udma %d) failed\n",
				__func__,
				i);
			goto done;
		}
	}

	for (i = 0; i < ARRAY_SIZE(dev_attr_intr_moderation); i++ ) {
		status = sysfs_create_file(
			&dev->kobj,
			&dev_attr_intr_moderation[i].attr.attr);
		if (status) {
			dev_err(
				dev,
				"%s: sysfs_create_file(intr_moderation %d) failed\n",
				__func__,
				i);
			goto done;
		}
	}
done:

	return status;
}

/******************************************************************************
 *****************************************************************************/
void al_mod_eth_sysfs_terminate(
	struct device *dev)
{
	struct al_mod_eth_adapter *adapter = dev_get_drvdata(dev);
	int i;

	device_remove_file(dev, &dev_attr_small_copy_len);
	device_remove_file(dev, &dev_attr_link_poll_interval);
#ifdef CONFIG_ARCH_ALPINE
	device_remove_file(dev, &dev_attr_link_management_debug);
#endif /* CONFIG_ARCH_ALPINE */
	device_remove_file(dev, &dev_attr_max_rx_buff_alloc_size);
	device_remove_file(dev, &dev_attr_link_training_enable);
	device_remove_file(dev, &dev_attr_sfp_probe_1g);
	device_remove_file(dev, &dev_attr_sfp_probe_10g);
	device_remove_file(dev, &dev_attr_force_1000_base_x);
	device_remove_file(dev, &dev_attr_intr_moderation_restore_default);
	device_remove_file(dev, &dev_attr_flow_steer_config);
	device_remove_file(dev, &dev_attr_epe_tables_dump);
	device_remove_file(dev, &dev_attr_rfw_control_table_dump);
	device_remove_file(dev, &dev_attr_crc_tables_dump);
	device_remove_file(dev, &dev_attr_tx_metacache_table_dump);
	if (adapter->err_events_enabled) {
		device_remove_file(dev, &dev_attr_udma_errors);
		device_remove_file(dev, &dev_attr_ec_errors);
		device_remove_file(dev, &dev_attr_mac_errors);
		device_remove_file(dev, &dev_attr_ua_errors);
		device_remove_file(dev, &dev_attr_err_events_active);
#ifdef CONFIG_AL_ETH_ERR_EVENTS_TEST
		device_remove_file(dev, &dev_attr_err_events_test);
#endif
	}
#ifdef CONFIG_ARCH_ALPINE
	device_remove_file(dev, &dev_attr_default_rx_udma_num);
#endif /* CONFIG_ARCH_ALPINE */

	for (i = 0; i < ARRAY_SIZE(dev_attr_intr_moderation); i++ ) {
		sysfs_remove_file(
			&dev->kobj,
			&dev_attr_intr_moderation[i].attr.attr);
	}

	for (i = 0; i < ARRAY_SIZE(dev_attr_udma_debug); i++)
		sysfs_remove_file(
			&dev->kobj,
			&dev_attr_udma_debug[i].attr.attr);

	if ((adapter->board_type == ALPINE_INTEGRATED) &&
	    (adapter->use_lm) &&
	    (adapter->al_mod_chip_id == PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V2)) {
		for (i = 0; i < ARRAY_SIZE(dev_attr_serdes_params); i++) {
			sysfs_remove_file(&dev->kobj,
					  &dev_attr_serdes_params[i].attr.attr);
		}
	}
}

/******************************************************************************
 *****************************************************************************/
static ssize_t rd_udma_dump(
	struct device *dev,
	struct device_attribute *attr,
	char *buf)
{
	struct dev_ext_attribute *ea = to_ext_attr(attr);
	enum udma_dump_type dump_type = (enum udma_dump_type)ea->var;
	ssize_t rc = 0;

	switch (dump_type) {
	case UDMA_DUMP_M2S_REGS:
	case UDMA_DUMP_S2M_REGS:
		rc = sprintf(
			buf,
			"Write mask to dump corresponding udma regs\n");
		break;
	case UDMA_DUMP_M2S_Q_STRUCT:
	case UDMA_DUMP_S2M_Q_STRUCT:
		rc = sprintf(
			buf,
			"Write q num to dump correspoding q struct\n");
		break;
	case UDMA_DUMP_M2S_Q_POINTERS:
	case UDMA_DUMP_S2M_Q_POINTERS:
		rc = sprintf(
			buf,
			"Write q num (in hex) and add 1 for submission ring,"
			" for ex:\n"
			"0 for completion ring of q 0\n"
			"10 for submission ring of q 0\n");
		break;
	default:
		break;
	}

	return rc;
}

struct al_mod_eth_adapter;
extern struct al_mod_udma *al_mod_eth_udma_get(struct al_mod_eth_adapter *adapter, int tx);

/******************************************************************************
 *****************************************************************************/
static ssize_t wr_udma_dump(
	struct device *dev,
	struct device_attribute *attr,
	const char *buf,
	size_t count)
{
	int err;
	int q_id;
	unsigned long val;
	struct dev_ext_attribute *ea = to_ext_attr(attr);
	enum udma_dump_type dump_type = (enum udma_dump_type)ea->var;
	enum al_mod_udma_ring_type ring_type = AL_RING_COMPLETION;
	struct al_mod_eth_adapter *adapter = dev_get_drvdata(dev);

	err = kstrtoul(buf, 16, &val);
	if (err < 0)
		return err;

	switch (dump_type) {
	case UDMA_DUMP_M2S_REGS:
		al_mod_udma_regs_print(al_mod_eth_udma_get(adapter, 1), val);
		break;
	case UDMA_DUMP_S2M_REGS:
		al_mod_udma_regs_print(al_mod_eth_udma_get(adapter, 0), val);
		break;
	case UDMA_DUMP_M2S_Q_STRUCT:
		al_mod_udma_q_struct_print(al_mod_eth_udma_get(adapter, 1), val);
		break;
	case UDMA_DUMP_S2M_Q_STRUCT:
		al_mod_udma_q_struct_print(al_mod_eth_udma_get(adapter, 0), val);
		break;
	case UDMA_DUMP_M2S_Q_POINTERS:
		if (val & 0x10)
			ring_type = AL_RING_SUBMISSION;
		q_id = val & 0xf;
		al_mod_udma_ring_print(al_mod_eth_udma_get(adapter, 1), q_id, ring_type);
		break;
	case UDMA_DUMP_S2M_Q_POINTERS:
		if (val & 0x10)
			ring_type = AL_RING_SUBMISSION;
		q_id = val & 0xf;
		al_mod_udma_ring_print(al_mod_eth_udma_get(adapter, 0), q_id, ring_type);
		break;
	default:
		break;
	}

	return count;
}

/******************************************************************************
 *****************************************************************************/
extern void al_mod_eth_get_intr_moderation_entry(enum al_mod_eth_intr_moderation_level level,
		struct al_mod_eth_intr_moderation_entry *entry);

extern void al_mod_eth_init_intr_moderation_entry(enum al_mod_eth_intr_moderation_level level,
		struct al_mod_eth_intr_moderation_entry *entry);

static ssize_t al_mod_eth_show_intr_moderation(
	struct device *dev,
	struct device_attribute *attr,
	char *buf)
{
	struct al_mod_eth_intr_moderation_entry entry;
	struct dev_ext_attribute *ea = to_ext_attr(attr);
	enum al_mod_eth_intr_moderation_level level =
			(enum al_mod_eth_intr_moderation_level)ea->var;
	ssize_t rc = 0;

	al_mod_eth_get_intr_moderation_entry(level, &entry);

	rc = sprintf(buf, "%u %u %u\n",
			entry.intr_moderation_interval,
			entry.packets_per_interval,
			entry.bytes_per_interval);

	return rc;
}

static ssize_t al_mod_eth_store_intr_moderation(
	struct device *dev,
	struct device_attribute *attr,
	const char *buf,
	size_t count)
{
	int cnt;
	struct al_mod_eth_intr_moderation_entry entry;
	struct dev_ext_attribute *ea = to_ext_attr(attr);
	enum al_mod_eth_intr_moderation_level level =
			(enum al_mod_eth_intr_moderation_level)ea->var;

	cnt = sscanf(buf, "%u %u %u",
			&entry.intr_moderation_interval,
			&entry.packets_per_interval,
			&entry.bytes_per_interval);

	if (cnt != 3)
		return -EINVAL;

	al_mod_eth_init_intr_moderation_entry(level, &entry);

	return count;
}


#endif
