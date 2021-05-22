/*
 * Copyright 2018, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#include "al_hal_common.h"
#include "al_dt_parse_pcie.h"
#include "al_dt_parse_internal.h"

#define AL_DT_PARSE_PCIE_FORMAT					\
	"/soc/board-cfg/pcie"

#define PCIE_PRI(format, ...)					\
	"PCIE " format, ##__VA_ARGS__

/* PCIE Ports */
#define AL_DT_PARSE_PCIE_PORT_FORMAT				\
	AL_DT_PARSE_PCIE_FORMAT "/port%u"

#define AL_DT_PARSE_PORT_PF_FORMAT				\
	AL_DT_PARSE_PCIE_PORT_FORMAT "/pf%u"

#define AL_DT_PARSE_PORT_CFG_SPACE_HEADER_FORMAT		\
	AL_DT_PARSE_PORT_PF_FORMAT "/pci-header"

#define AL_DT_PARSE_PORT_BAR_HEADER_FORMAT			\
	AL_DT_PARSE_PORT_CFG_SPACE_HEADER_FORMAT "/bar%u"

#define AL_DT_PARSE_PORT_MSIX_CAP				\
	AL_DT_PARSE_PORT_PF_FORMAT "/msix-cap"

#define AL_DT_PARSE_PCIE_PORT_SRIS_PARAMS_FORMAT		\
	AL_DT_PARSE_PCIE_PORT_FORMAT "/sris-params"

#define PCIE_PORT_PRI(format, ...)				\
	"\tPcie Port %u " format, port_idx, ##__VA_ARGS__

#define PORT_PF_PRI(format, ...)				\
	"\t\tPf %u " format, pf_idx, ##__VA_ARGS__

#define SRIS_PARAMS_PRI(format, ...)				\
	"\t\tSRIS params " format, ##__VA_ARGS__

#define DEFAULT_RC_LINK_PRE_DELAY		100
#define DEFAULT_RC_LINK_UP_TIMEOUT		100

static void al_dt_parse_pcie_port_gen(const struct al_dt_parse_handle *handle,
		const void *node_context,
		struct al_pcie_port_init_params *port_params)
{
	unsigned int default_val32, val32;
	int ret;

	default_val32 = 0;
	ret = al_dt_parse_property_u32_get(handle, node_context, "gen",
					   &val32, &default_val32);
	if (ret == 0) {
		dt_debug("\t\tgen: %u\n", val32);
		port_params->max_speed = val32;
	} else {
		dt_debug("\t\tgen doesn't exists, using default: %u\n", val32);
	}
}

static void al_dt_parse_pcie_port_is_ep(const struct al_dt_parse_handle *handle,
		const void *node_context,
		struct al_pcie_port_init_params *port_params)
{
	if (al_dt_parse_property_str_eq(handle, node_context, "operation-mode", "ep"))
		port_params->ep = AL_TRUE;
	else
		port_params->ep = AL_FALSE;

	dt_debug("\t\tis ep: %s\n", port_params->ep ? "True" : "False");
}

static void al_dt_parse_pcie_port_iof_regs_access_en(const struct al_dt_parse_handle *handle,
		const void *node_context,
		struct al_pcie_port_init_params *port_params)
{
	if (al_dt_parse_property_str_eq(handle, node_context, "iof-regs-access-en", "enabled"))
		port_params->iof_regs_access_en = AL_TRUE;
	else
		port_params->iof_regs_access_en = AL_FALSE;

	dt_debug("\t\tiof regs access enable: %s\n",
		port_params->iof_regs_access_en ? "Enabled" : "Disabled");
}

static void al_dt_parse_pcie_port_present(const struct al_dt_parse_handle *handle,
		const void *node_context,
		struct al_pcie_port_init_params *port_params)
{
	unsigned int default_val32, val32;
	int ret;

	default_val32 = 0;
	ret = al_dt_parse_property_u32_get(handle, node_context, "present",
					   &val32, &default_val32);
	if (ret == 0) {
		dt_debug("\t\tpresent: %u\n", val32);
		port_params->present = val32;
	} else {
		dt_debug("\t\tpresent, using default\n");
	}
}

static void al_dt_parse_pcie_port_width(const struct al_dt_parse_handle *handle,
		const void *node_context,
		struct al_pcie_port_init_params *port_params)
{
	unsigned int default_val32, val32;
	int ret;

	default_val32 = 0;
	ret = al_dt_parse_property_u32_get(handle, node_context, "width",
					   &val32, &default_val32);
	if (ret == 0) {
		dt_debug("\t\tmax lanes: %u\n", val32);
		port_params->num_lanes = val32;
	} else {
		dt_debug("\t\tmax lanes doesn't exitsts, using default\n");
	}
}

static void al_dt_parse_pcie_port_ref_clk(const struct al_dt_parse_handle *handle,
		const void *node_context,
		struct al_pcie_port_init_params *port_params)
{
	unsigned int default_val32, val32;
	int ret;

	default_val32 = 0;
	ret = al_dt_parse_property_u32_get(handle, node_context, "ref-clk-idx",
					   &val32, &default_val32);
	if (ret == 0) {
		dt_debug("\t\tref-clk-idx: %u\n", val32);
		port_params->ref_clk_idx = val32;
	} else {
		dt_debug("\t\tref-clk-idx doesn't exists, using default\n");
	}
}

static int al_dt_parse_cfg_space_header_bar(const struct al_dt_parse_handle *handle,
		unsigned int port_idx,
		unsigned int pf_idx,
		unsigned int bar_idx,
		struct al_pcie_port_pf_init_params *pf_params)
{
	int ret;
	char path[AL_DT_PARSE_PATH_LEN_MAX];
	const void *node_context;
	uint64_t val64;
	uint64_t default_val = 0;
	struct al_pcie_ep_bar_params *bars = &pf_params->bars[bar_idx];

	dt_debug("\t\t\t\tbar: %d\n", bar_idx);

	al_sprintf(path, AL_DT_PARSE_PORT_BAR_HEADER_FORMAT, port_idx, pf_idx, bar_idx);

	ret = handle->node_open(handle->dt_context, path, &node_context);
	if (ret) {
		dt_debug("\t\t\t\tbar %u entry not found in DT\n", bar_idx);
		return -EINVAL;
	}

	bars->enable = al_dt_parse_property_str_eq(handle, node_context,
							       "status", "enabled");
	dt_debug("\t\t\t\t\tstatus: %s\n", bars->enable ? "enabled" : "disabled");

	bars->memory_space = al_dt_parse_property_str_eq(handle, node_context,
							       "is-memory-space", "enabled");
	dt_debug("\t\t\t\t\tmemory-space: %s\n", bars->memory_space ? "enabled" : "disabled");

	bars->memory_64_bit = al_dt_parse_property_str_eq(handle, node_context,
							       "is-memory-64-bit", "enabled");
	dt_debug("\t\t\t\t\tis-memory-64-bit: %s\n", bars->memory_64_bit ? "enabled" : "disabled");

	bars->memory_is_prefetchable = al_dt_parse_property_str_eq(handle, node_context,
							       "is-memory-prefetchable", "enabled");
	dt_debug("\t\t\t\t\tis-memory-prefetchable: %s\n",
		 bars->memory_is_prefetchable ? "enabled" : "disabled");

	ret = al_dt_parse_property_u64_get(handle, node_context, "size",
					   &val64, &default_val);
	if (ret == 0)
		dt_debug("\t\t\t\t\tsize: 0x%" PRIx64 "\n", val64);
	else
		dt_debug("\t\t\t\t\tsize doesn't exists, using default: 0x%" PRIx64 "\n", val64);

	bars->size = val64;

	ret = al_dt_parse_property_u64_get(handle, node_context, "target-address",
					   &val64, &default_val);
	if (ret == 0)
		dt_debug("\t\t\t\t\ttarget-address: 0x%" PRIx64 "\n", val64);
	else
		dt_debug("\t\t\t\t\ttarget-address doesn't exists, "
			 "using default: 0x%" PRIx64 "\n", val64);

	pf_params->target_addr[bar_idx] = val64;

	handle->node_close(handle->dt_context, node_context);

	return 0;
}

static int al_dt_parse_port_cfg_space_header(const struct al_dt_parse_handle *handle,
		unsigned int port_idx,
		unsigned int pf_idx,
		struct al_pcie_ep_id_params *cfg_space_params)
{
	unsigned int default_val32, val32;
	int ret;
	char path[AL_DT_PARSE_PATH_LEN_MAX];
	const void *node_context;

	dt_debug("\t\t\tpci header:\n");

	al_sprintf(path, AL_DT_PARSE_PORT_CFG_SPACE_HEADER_FORMAT, port_idx, pf_idx);

	ret = handle->node_open(handle->dt_context, path, &node_context);
	if (ret) {
		al_err("\t\tDT node not found\n");
		return -EINVAL;
	}

	default_val32 = 0xffff;

	ret = al_dt_parse_property_u32_get(handle, node_context, "vendor-id",
					   &val32, &default_val32);
	if (ret == 0)
		dt_debug("\t\t\t\tvendor id: 0x%x\n", val32);
	else
		dt_debug("\t\t\t\tvendor id doesn't exists, using default: 0x%x\n", val32);

	cfg_space_params->vendor_id = val32;

	ret = al_dt_parse_property_u32_get(handle, node_context, "device-id",
					   &val32, &default_val32);
	if (ret == 0)
		dt_debug("\t\t\t\tdevice id: 0x%x\n", val32);
	else
		dt_debug("\t\t\t\tdevice id doesn't exists, using default: 0x%x\n", val32);

	cfg_space_params->device_id = val32;

	ret = al_dt_parse_property_u32_get(handle, node_context, "class-code",
					   &val32, &default_val32);
	if (ret == 0)
		dt_debug("\t\t\t\tclass code: 0x%x\n", val32);
	else
		dt_debug("\t\t\t\tclass code doesn't exists, using default: 0x%x\n", val32);

	cfg_space_params->class_code = val32;

	ret = al_dt_parse_property_u32_get(handle, node_context, "revision-id",
					   &val32, &default_val32);
	if (ret == 0)
		dt_debug("\t\t\t\trevision id: 0x%x\n", val32);
	else
		dt_debug("\t\t\t\trevision id doesn't exists, using default: 0x%x\n", val32);

	cfg_space_params->revision_id = val32;

	handle->node_close(handle->dt_context, node_context);

	return 0;
}

static int al_dt_parse_port_msix_cap(const struct al_dt_parse_handle *handle,
		unsigned int port_idx,
		unsigned int pf_idx,
		struct al_pcie_port_pf_init_params *pf_params)
{
	char path[AL_DT_PARSE_PATH_LEN_MAX];
	const void *node_context;
	int ret;
	unsigned int default_val32, val32;

	dt_debug("\t\t\tmsix-cap:\n");

	al_sprintf(path, AL_DT_PARSE_PORT_MSIX_CAP, port_idx, pf_idx);
	ret = handle->node_open(handle->dt_context, path, &node_context);
	if (ret) {
		dt_debug("\t\t\tmsix-cap: Node not found in DT\n");
		return -EINVAL;
	}

	pf_params->msix_enable = al_dt_parse_property_str_eq(handle, node_context,
							       "status", "enabled");
	dt_debug("\t\t\t\tstatus: %s\n", pf_params->msix_enable ? "enabled" : "disabled");

	default_val32 = 0;
	ret = al_dt_parse_property_u32_get(handle, node_context,
					   "table-size",
					   &val32, &default_val32);
	if (ret == 0)
		dt_debug("\t\t\t\ttable-size: 0x%x\n", val32);
	else
		dt_debug("\t\t\t\ttable-size doesn't exists, using default: 0x%x\n", val32);

	pf_params->msix.table_size = val32;

	default_val32 = 0;
	ret = al_dt_parse_property_u32_get(handle, node_context,
					   "table-offset",
					   &val32, &default_val32);
	if (ret == 0)
		dt_debug("\t\t\t\ttable-offset: 0x%x\n", val32);
	else
		dt_debug("\t\t\t\ttable-offset doesn't exists, using default: 0x%x\n", val32);

	pf_params->msix.table_offset = val32;


	default_val32 = 0;
	ret = al_dt_parse_property_u32_get(handle, node_context,
					   "table-bir",
					   &val32, &default_val32);
	if (ret == 0)
		dt_debug("\t\t\t\ttable-bir: 0x%x\n", val32);
	else
		dt_debug("\t\t\t\ttable-bir doesn't exists, using default: 0x%x\n", val32);

	pf_params->msix.table_bar = val32;

	default_val32 = 0;
	ret = al_dt_parse_property_u32_get(handle, node_context,
					   "pba-offset",
					   &val32, &default_val32);
	if (ret == 0)
		dt_debug("\t\t\t\tpba-offset: 0x%x\n", val32);
	else
		dt_debug("\t\t\t\tpba-offset doesn't exists, using default: 0x%x\n", val32);

	pf_params->msix.pba_offset = val32;

	default_val32 = 0;
	ret = al_dt_parse_property_u32_get(handle, node_context,
					   "pba-bir",
					   &val32, &default_val32);
	if (ret == 0)
		dt_debug("\t\t\t\tpba-bir: 0x%x\n", val32);
	else
		dt_debug("\t\t\t\tpba-bir doesn't exists, using default: 0x%x\n", val32);

	pf_params->msix.pba_bar = val32;

	handle->node_close(handle->dt_context, node_context);

	return 0;
}

static int al_dt_parse_pcie_port_pf_params(
		const struct al_dt_parse_handle *handle,
		unsigned int port_idx,
		unsigned int pf_idx,
		struct al_pcie_port_pf_init_params *pf_params)
{
	char path[AL_DT_PARSE_PATH_LEN_MAX];
	const void *node_context;
	int ret;
	unsigned int i;

	al_assert(handle);
	al_assert(pf_params);

	dt_debug(PORT_PF_PRI("\n"));

	al_sprintf(path, AL_DT_PARSE_PORT_PF_FORMAT, port_idx, pf_idx);

	ret = handle->node_open(handle->dt_context, path, &node_context);
	if (ret) {
		dt_debug(PORT_PF_PRI("DT node not found\n"));
		return -EINVAL;
	}

	pf_params->status = al_dt_parse_property_str_eq(handle, node_context,
							       "status", "enabled");

	dt_debug("\t\t\tstatus: %s\n", (pf_params->status ? "enabled" : "disabled"));

	handle->node_close(handle->dt_context, node_context);

	ret = al_dt_parse_port_cfg_space_header(handle, port_idx, pf_idx, &pf_params->cfg_space);
	if (ret)
		return ret;

	for (i = 0; i < AL_PCIE_PORT_PF_INIT_PARAMS_NUM_BARS; i++)
		al_dt_parse_cfg_space_header_bar(handle, port_idx, pf_idx, i, pf_params);

	ret = al_dt_parse_port_msix_cap(handle, port_idx, pf_idx, pf_params);
	if (ret)
		return ret;

	return 0;
}

static int al_dt_parse_pcie_port_sris_params(
	const struct al_dt_parse_handle *handle,
	unsigned int port_idx,
	struct al_pcie_sris_params *sris_params)
{
	char path[AL_DT_PARSE_PATH_LEN_MAX];
	const void *node_context;
	int ret;

	al_assert(handle);
	al_assert(sris_params);

	dt_debug(SRIS_PARAMS_PRI("\n"));

	al_sprintf(path, AL_DT_PARSE_PCIE_PORT_SRIS_PARAMS_FORMAT, port_idx);
	ret = handle->node_open(handle->dt_context, path, &node_context);
	if (ret)
		return -EINVAL;

	sris_params->use_defaults = al_dt_parse_property_str_eq(handle, node_context,
								"use_defaults", "true");
	dt_debug("\t\t\tuse_defaults: %s\n", (sris_params->use_defaults ? "true" : "false"));

	handle->node_close(handle->dt_context, node_context);

	return 0;
}

static int al_dt_parse_pcie_port_params(const struct al_dt_parse_handle *handle,
		unsigned int port_idx,
		struct al_pcie_port_init_params *port_params)
{
	char path[AL_DT_PARSE_PATH_LEN_MAX];
	const void *node_context;
	const void *property_context;
	int ret;
	unsigned int i;

	al_assert(handle);
	al_assert(port_params);

	dt_debug(PCIE_PORT_PRI("\n"));

	al_sprintf(path, AL_DT_PARSE_PCIE_PORT_FORMAT, port_idx);

	ret = handle->node_open(handle->dt_context, path, &node_context);
	if (ret) {
		dt_debug(PCIE_PORT_PRI("DT node not found\n"));
		return -EINVAL;
	}

	port_params->port_idx = port_idx;
	dt_debug("\t\tport_idx: %d\n", port_params->port_idx);

	port_params->status = al_dt_parse_property_str_eq(handle,
							  node_context, "status", "enabled");

	dt_debug("\t\tstatus: %s\n", (port_params->status ? "enabled" : "disabled"));

	/* Assuming we will have port_init_stage entry in DT */
	port_params->port_init_found = AL_TRUE;

	ret = al_dt_parse_property_str_open(handle, node_context, "port-init-stage",
					    &property_context,
					    (const char **)&port_params->port_init_stage);
	if (ret) {
		dt_debug(PCIE_PORT_PRI("port-init-stage not found!\n"));
		port_params->port_init_found = AL_FALSE;
	} else {
		dt_debug("\t\tport-init-stage: %s\n", port_params->port_init_stage);
	}

	al_dt_parse_property_str_close(handle, node_context, property_context);

	ret = al_dt_parse_property_str_open(handle, node_context, "link-init-stage",
					    &property_context,
					    (const char **)&port_params->link_init_stage);
	if (ret)
		dt_debug(PCIE_PORT_PRI("link-init-stage not found!\n"));
	else
		dt_debug("\t\tlink-init-stage: %s\n", port_params->link_init_stage);

	ret = al_dt_parse_property_str_open(handle, node_context, "cfg-space-init-stage",
					    &property_context,
					    (const char **)&port_params->cfg_space_init_stage);
	if (ret)
		dt_debug(PCIE_PORT_PRI("cfg-space-init-stage not found!\n"));
	else
		dt_debug("\t\tcfg-space-init-stage: %s\n", port_params->cfg_space_init_stage);

	port_params->sris_config_status = al_dt_parse_property_str_eq(handle, node_context,
							 "sris-config-status", "enabled");

	dt_debug("\tsris config status: %s\n",
		 (port_params->sris_config_status ? "enabled" : "disabled"));

	ret = al_dt_parse_property_str_open(handle, node_context, "sris-config-stage",
					    &property_context,
					    (const char **)&port_params->sris_config_stage);
	if (ret)
		dt_debug(PCIE_PORT_PRI("sris-config-stage not found!\n"));

	al_dt_parse_property_str_close(handle, node_context, property_context);

	al_dt_parse_pcie_port_width(handle, node_context, port_params);

	al_dt_parse_pcie_port_gen(handle, node_context, port_params);

	al_dt_parse_pcie_port_ref_clk(handle, node_context, port_params);

	al_dt_parse_pcie_port_is_ep(handle, node_context, port_params);

	al_dt_parse_pcie_port_iof_regs_access_en(handle, node_context, port_params);

	al_dt_parse_pcie_port_present(handle, node_context, port_params);

	handle->node_close(handle->dt_context, node_context);

	for (i = 0; i < AL_MAX_NUM_OF_PFS; i++)
		al_dt_parse_pcie_port_pf_params(handle, port_idx, i, &port_params->pf_params[i]);

	al_dt_parse_pcie_port_sris_params(handle, port_idx, &port_params->pcie_sris_params);

	return 0;
}

/*******************************************************************************
 ** API Functions
 *******************************************************************************/
int al_dt_parse_pcie_params(const struct al_dt_parse_handle *handle,
		struct al_pcie_init_params_adv *pcie_params)
{
	char path[AL_DT_PARSE_PATH_LEN_MAX];
	const void *node_context;
	unsigned int default_val32, val32;
	int ret, i;

	al_assert(handle);
	al_assert(pcie_params);

	dt_debug(PCIE_PRI("\n"));

	al_sprintf(path, AL_DT_PARSE_PCIE_FORMAT);

	ret = handle->node_open(handle->dt_context, path, &node_context);
	if (ret) {
		dt_debug(PCIE_PRI("DT node not found\n"));
		return -EINVAL;
	}

	default_val32 = DEFAULT_RC_LINK_PRE_DELAY;
	ret = al_dt_parse_property_u32_get(handle, node_context,
					   "rc-link-pre-delay",
					   &val32, &default_val32);
	if (ret == 0)
		dt_debug("\trc-link-pre-delay: %u\n", val32);
	else
		dt_debug("\trc-link-pre-delay doesn't exists, using default: %u\n", val32);

	pcie_params->pcie_rc_link_pre_delay = val32;

	default_val32 = DEFAULT_RC_LINK_UP_TIMEOUT;
	ret = al_dt_parse_property_u32_get(handle, node_context,
					   "rc-link-up-timeout",
					   &val32, &default_val32);
	if (ret == 0)
		dt_debug("\trc-link-up-timeout: %u\n", val32);
	else
		dt_debug("\trc-link-up-timeout doesn't exists, using default: %u\n", val32);

	pcie_params->pcie_rc_link_up_timeout = val32;

	default_val32 = 0;
	ret = al_dt_parse_property_u32_get(handle, node_context,
					   "rc-link-post-delay",
					   &val32, &default_val32);
	if (ret == 0)
		dt_debug("\trc-link-post-delay: %u\n", val32);
	else
		dt_debug("\trc-link-port-delay doesn't exists, using default: %u\n", val32);

	pcie_params->pcie_rc_link_post_delay = val32;

	pcie_params->pcie_rc_link_retry = al_dt_parse_property_str_eq(handle, node_context,
								 "rc-link-retry", "enabled");

	dt_debug("\trc-link-retry: %s\n", pcie_params->pcie_rc_link_retry ? "enabled" : "disabled");

	pcie_params->pcie_rc_retrain_if_not_full_width = al_dt_parse_property_str_eq(handle,
							node_context,
							"rc-link-retrain-if-not-full-width",
							"enabled");

	dt_debug("\trc-link-retrain-if-not-full-width: %s\n",
		 pcie_params->pcie_rc_retrain_if_not_full_width ? "enabled" : "disabled");

	handle->node_close(handle->dt_context, node_context);

	for (i = 0; i < AL_PCIE_PORT_NUM_MAX; i++)
		al_dt_parse_pcie_port_params(handle, i, &pcie_params->port_params[i]);

	return 0;
}
