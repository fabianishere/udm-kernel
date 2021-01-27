/**
 * Copyright 2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 */
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/of_platform.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>

#include "alpine_avg_serdes.h"

#define ALPINE_AVG_SERDES_SBUS_OFFSET	0x0001000
#define ALPINE_AVG_SERDES_LANE_BASE(i)	(0x10000 + ((i) * 0x400))

#define ALPINE_AVG_SERDES_LANE_NAME_MAX	8

static const char * const alpine_avg_serdes_compatible[] = {
	[ALPINE_AVG_SERDES_CMPLX_ID_MAIN] =
		"annapurna-labs,al-serdes-avg-complex-main",
	[ALPINE_AVG_SERDES_CMPLX_ID_SEC] =
		"annapurna-labs,al-serdes-avg-complex-sec",
	[ALPINE_AVG_SERDES_CMPLX_ID_HS] =
		"annapurna-labs,al-serdes-avg-complex-hs"
};

struct alpine_avg_serdes {
	enum alpine_avg_serdes_cmplx_id type;
	void __iomem *base;
	void __iomem *sbus_base;
	void __iomem *lanes_base;
	int lanes_count;
	struct al_mod_serdes_complex_obj hal_serdes;
	struct al_mod_serdes_grp_obj hal_lanes[0];
};

static struct alpine_avg_serdes *serdeses[ALPINE_AVG_SERDES_CMPLX_ID_MAX];

void __iomem *alpine_avg_serdes_resource_get(
		enum alpine_avg_serdes_cmplx_id type)
{
	if (type >= ALPINE_AVG_SERDES_CMPLX_ID_MAX) {
		pr_err("%s() serdes type %d does not exist\n",
		       __func__,
		       (int)type);
		return NULL;
	}
	if (!serdeses[type]) {
		pr_err("%s(): serdes type %d was not initialized\n",
		       __func__,
		       (int)type);
		return NULL;
	}
	return serdeses[type]->base;
}
EXPORT_SYMBOL(alpine_avg_serdes_resource_get);

struct al_mod_serdes_grp_obj *alpine_avg_serdes_obj_get(
		enum alpine_avg_serdes_cmplx_id type,
		int lane)
{
	if (!serdeses[type]) {
		pr_err("%s(): serdes type %d was not initialized\n",
		       __func__,
		       (int)type);
		return NULL;
	}

	if (lane >= serdeses[type]->lanes_count) {
		pr_err("%s(): lane %d not exist in serdes type %d\n",
		       __func__,
		       lane,
		       (int)type);
		return NULL;
	}

	return &serdeses[type]->hal_lanes[lane];
}
EXPORT_SYMBOL(alpine_avg_serdes_obj_get);

static int alpine_avg_serdes_handles_init(
		struct alpine_avg_serdes *serdes)
{
	int lane;
	void __iomem *lane_base;
	enum al_mod_serdes_complex_type complex_type;

	switch (serdes->type) {
	case ALPINE_AVG_SERDES_CMPLX_ID_MAIN:
		complex_type = AL_SRDS_COMPLEX_TYPE_TR_MAIN;
		break;
	case ALPINE_AVG_SERDES_CMPLX_ID_SEC:
		complex_type = AL_SRDS_COMPLEX_TYPE_TR_SEC;
		break;
	case ALPINE_AVG_SERDES_CMPLX_ID_HS:
		complex_type = AL_SRDS_COMPLEX_TYPE_TR_HS;
		break;
	default:
		pr_err("%s(): bad serdes type %d\n",
		       __func__,
		       (int)serdes->type);
		return -EINVAL;
	}

	al_mod_serdes_avg_complex_handle_init(
			serdes->base,
			serdes->sbus_base,
			complex_type,
			NULL,
			NULL,
			&serdes->hal_serdes);

	for (lane = 0; lane < serdes->lanes_count; lane++) {
		lane_base = serdes->base + ALPINE_AVG_SERDES_LANE_BASE(lane);

		al_mod_serdes_avg_handle_init(
			lane_base,
			&serdes->hal_serdes,
			lane,
			&serdes->hal_lanes[lane]);
	}

	return 0;
}

static bool alpine_avg_serdes_of_device_is_available(
			const struct device_node *device)
{
	const char *status;
	int statlen;

	if (!device)
		return false;

	if (of_device_is_available(device))
		return true;

	status = of_get_property(device, "status", &statlen);
	if (status == NULL)
		return true;

	if (statlen > 0 && !strcmp(status, "enabled"))
		return true;

	return false;
}

static int __init alpine_avg_serdes_init(
		enum alpine_avg_serdes_cmplx_id type)
{
	struct device_node *np;
	int lanes_count = 0;
	char lane_name[ALPINE_AVG_SERDES_LANE_NAME_MAX];
	int size;
	struct alpine_avg_serdes *serdes;

	np = of_find_compatible_node(NULL,
				     NULL,
				     alpine_avg_serdes_compatible[type]);
	if (!np) {
		pr_debug("%s(): could not find dt for %s\n",
			 __func__, alpine_avg_serdes_compatible[type]);
		goto fail;
	}

	if (!alpine_avg_serdes_of_device_is_available(np)) {
		pr_debug("%s(): %s is disabled\n",
			 __func__, alpine_avg_serdes_compatible[type]);

		goto fail;
	}

	while (true) {
		snprintf(lane_name, sizeof(lane_name), "lane%d", lanes_count);
		if (!of_get_child_by_name(np, lane_name))
			break;
		lanes_count++;
	}
	size = sizeof(struct alpine_avg_serdes) +
		lanes_count * sizeof(struct al_mod_serdes_grp_obj);
	serdes = kmalloc(size, GFP_KERNEL);
	if (!serdes) {
		pr_err("%s() fail to allocate memory\n", __func__);
		return -ENOMEM;
	}

	serdes->type = type;
	serdes->lanes_count = lanes_count;
	serdes->base = of_iomap(np, 0);
	if (!serdes->base) {
		pr_err("%s(): fail to map device address\n", __func__);
		return -ENOMEM;
	}
	serdes->sbus_base = (char *)serdes->base +
		ALPINE_AVG_SERDES_SBUS_OFFSET;

	alpine_avg_serdes_handles_init(serdes);

	serdeses[type] = serdes;

	pr_info("alpine_avg_serdes initialized serdes type %d with %d lanes\n",
		serdes->type,
		serdes->lanes_count);

	return 0;

fail:
	return -EINVAL;
}

static int __init alpine_avg_serdeses_init(void)
{
	int ret;

	ret = alpine_avg_serdes_init(ALPINE_AVG_SERDES_CMPLX_ID_MAIN);
	if (ret)
		pr_debug("%s(): fail to init avg-main serdes (%d)\n",
			 __func__,
			 ret);

	ret = alpine_avg_serdes_init(ALPINE_AVG_SERDES_CMPLX_ID_SEC);
	if (ret)
		pr_debug("%s(): fail to init avg-sec serdes (%d)\n",
			 __func__,
			 ret);

	ret = alpine_avg_serdes_init(ALPINE_AVG_SERDES_CMPLX_ID_HS);
	if (ret)
		pr_debug("%s(): fail to init avg-hs serdes (%d)\n",
			 __func__,
			 ret);

	return 0;
}

module_init(alpine_avg_serdeses_init);
