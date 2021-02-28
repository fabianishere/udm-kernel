/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#include "al_hal_serdes_snps_init.h"
#include "al_hal_serdes_snps_quad_regs.h"

#define RESET_PEROID_POWERUP	15	/* [uSec] */
#define RESET_PEROID_WARM	10	/* [nSec] */

#define SRAM_INIT_TIMEOUT	50	/* [uSec] */

static void al_serdes_snps_quad_config(
	struct al_serdes_grp_obj	*obj,
	struct al_serdes_snps_quad_cfg	*quad_cfg)
{
	struct al_serdes_snps_quad_regs __iomem *regs =
		(struct al_serdes_snps_quad_regs __iomem *)obj->regs_base;
	unsigned int time_left;

	al_assert(quad_cfg);

	/* Preconfigure the phy and PLL, assert phy reset, wait, and de-assert it */
	al_reg_write32_masked(&regs->glb.clk,
		SERDES_SNPS_QUAD_GLB_CLK_PHY_REF_USE_PAD,
		obj->quad_in_complex ? 0 : SERDES_SNPS_QUAD_GLB_CLK_PHY_REF_USE_PAD);
	al_reg_write32_masked(&regs->glb.config, SERDES_SNPS_QUAD_GLB_CONFIG_PHY_RESET,
		SERDES_SNPS_QUAD_GLB_CONFIG_PHY_RESET);
	al_udelay(RESET_PEROID_POWERUP);
	al_reg_write32_masked(&regs->glb.config, SERDES_SNPS_QUAD_GLB_CONFIG_PHY_RESET, 0);

	/* Wait for SRAM init to complete */
	for (time_left = SRAM_INIT_TIMEOUT; time_left; time_left--) {
		if (al_reg_read32(&regs->glb.config) & SERDES_SNPS_QUAD_GLB_CONFIG_SRAM_INIT_DONE)
			break;
		al_udelay(1);
	}
	al_assert(time_left);
	al_reg_write32_masked(&regs->glb.config, SERDES_SNPS_QUAD_GLB_CONFIG_SRAM_EXT_LD_DONE,
		SERDES_SNPS_QUAD_GLB_CONFIG_SRAM_EXT_LD_DONE);
}

static int al_serdes_snps_complex_config(
	struct al_serdes_complex_obj	*obj,
	struct al_serdes_snps_quad_cfg	*quad_cfg,
	unsigned int			num_quads)
{
	unsigned int i;

	al_assert(obj);
	al_assert(quad_cfg);
	al_assert(obj->complex_type == AL_SRDS_COMPLEX_TYPE_TC7_MAIN);

	switch (obj->complex_type) {
	case AL_SRDS_COMPLEX_TYPE_TC7_MAIN:
		/* No muxing is required */
		break;
	default:
		al_err("%s: invalid complex type (%d)\n", __func__, obj->complex_type);
		return -EINVAL;
	}

	for (i = 0; i < num_quads; i++) {
		if (quad_cfg[i].mode == AL_SRDS_SNPS_MODE_SKIP)
			continue;

		al_assert(obj->quads[i]);
		al_serdes_snps_quad_config(obj->quads[i], &quad_cfg[i]);
	}

	return 0;
}

/******************************************************************************/
/******************************************************************************/
static const char *al_serdes_snps_mode_to_str(enum al_serdes_snps_mode mode)
{
	switch (mode) {
	case AL_SRDS_SNPS_MODE_SKIP:
		return "SKIP";
	case AL_SRDS_SNPS_MODE_PCIE_0:
		return "PCIE_0";
	}

	al_err("%s: invalid value (%d)!\n", __func__, mode);

	return "";
}

/******************************************************************************/
/******************************************************************************/
static const char *al_serdes_snps_complex_type_to_str(
	enum al_serdes_complex_type type)
{
	switch (type) {
	case AL_SRDS_COMPLEX_TYPE_TC7_MAIN:
		return "TC7_MAIN";
	}

	al_err("%s: invalid type (%d)!\n", __func__, type);

	return "";
}

/******************************************************************************/
/******************************************************************************/
#define CMPLX_CFG_PRINT(...)		al_info(__VA_ARGS__)

/******************************************************************************/
/******************************************************************************/
static void al_serdes_snps_depth_print(
	int				depth)
{
	int i;

	for (i = 0; i < depth; i++)
		CMPLX_CFG_PRINT("\t");
}

/******************************************************************************/
/******************************************************************************/
static void al_serdes_snps_quad_cfg_print(
	struct al_serdes_snps_quad_cfg	*cfg,
	int				depth)
{
	al_serdes_snps_depth_print(depth);
	CMPLX_CFG_PRINT("mode = %s\n", al_serdes_snps_mode_to_str(cfg->mode));
}

/******************************************************************************/
/******************************************************************************/
static void al_serdes_snps_complex_cfg_print(
	struct al_serdes_snps_complex_cfg	*cfg,
	int					depth)
{
	unsigned int i;

	al_serdes_snps_depth_print(depth);
	CMPLX_CFG_PRINT("type = %s\n",
			al_serdes_snps_complex_type_to_str(cfg->complex_obj->complex_type));
	al_serdes_snps_depth_print(depth);
	CMPLX_CFG_PRINT("quads:\n");
	for (i = 0; i < cfg->num_quads; i++) {
		al_serdes_snps_depth_print(depth + 1);
		CMPLX_CFG_PRINT("quad %d:\n", i);
		al_serdes_snps_quad_cfg_print(&cfg->quad_cfg[i], depth + 2);
	}
}

int al_serdes_snps_init(
	struct al_serdes_snps_init_params *init_params)
{
	struct al_serdes_snps_complex_cfg *complex_cfgs;
	unsigned int num_complexes;
	unsigned int i;

	al_assert(init_params);

	complex_cfgs = init_params->complex_cfgs;
	num_complexes = init_params->num_complexes;

	al_assert(complex_cfgs);
	al_assert(num_complexes >= 1);
	for (i = 0; i < num_complexes; i++) {
		al_assert(complex_cfgs[i].complex_obj);
		al_assert(complex_cfgs[i].quad_cfg);
	}

	/* Print initialization params */
	for (i = 0; i < num_complexes; i++)
		al_serdes_snps_complex_cfg_print(&complex_cfgs[i], 0);

	for (i = 0; i < num_complexes; i++) {
		struct al_serdes_complex_obj *complex_obj = complex_cfgs[i].complex_obj;
		struct al_serdes_snps_quad_cfg *quad_cfg = complex_cfgs[i].quad_cfg;
		unsigned int num_quads = complex_cfgs[i].num_quads;
		unsigned int j;
		int err;
		al_bool all_skipped = AL_TRUE;

		for (j = 0; j < AL_SRDS_QUADS_PER_COMPLEX_MAX; j++) {
			struct al_serdes_grp_obj *quad_obj = complex_obj->quads[j];
			if (quad_obj && (quad_cfg[j].mode != AL_SRDS_SNPS_MODE_SKIP)) {
				all_skipped = AL_FALSE;
				break;
			}
		}

		if (all_skipped)
			continue;

		err = al_serdes_snps_complex_config(complex_obj, quad_cfg, num_quads);
		if (err) {
			al_err("%s: al_serdes_snps_complex_config failed for complex %d\n",
				__func__, i);
			return err;
		}
	}

	return 0;
}
