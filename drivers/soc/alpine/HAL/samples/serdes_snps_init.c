#include "serdes_snps_init.h"
#include "al_hal_serdes_snps.h"
#include "al_hal_serdes_snps_init.h"
#include "al_hal_iomap.h"

#define LANE_CFG(mode)	\
	{ mode }

static struct al_serdes_complex_obj main_complex;
static struct al_serdes_grp_obj main_quads[AL_SRDS_QUADS_PER_COMPLEX_TC7_MAIN];
static struct al_serdes_snps_quad_cfg main_quads_quad_cfg[AL_SRDS_QUADS_PER_COMPLEX_TC7_MAIN] = {
	/*
	 * 0   1   2   3   4   5   6   7
	 * <--------- PCIe 0 ---------->
	 */
	LANE_CFG(AL_SRDS_SNPS_MODE_PCIE_0),
	LANE_CFG(AL_SRDS_SNPS_MODE_PCIE_0),
	};

static struct al_serdes_snps_complex_cfg complex_cfgs[] = {
	{ &main_complex, main_quads_quad_cfg, AL_ARR_SIZE(main_quads_quad_cfg) },
};

void serdes_snps_init(void)
{
	struct al_serdes_snps_init_params init_params = {
		.complex_cfgs = complex_cfgs,
		.num_complexes = AL_ARR_SIZE(complex_cfgs),
		};
	int i;

	/* Complexes & quads handle initialization */
	al_serdes_snps_complex_handle_init(
		(void __iomem *)(uintptr_t)AL_SB_SRD_CMPLX_MAIN_BASE,
		AL_SRDS_COMPLEX_TYPE_TC7_MAIN, &main_complex);
	for (i = 0; i < AL_SRDS_QUADS_PER_COMPLEX_TC7_MAIN; i++)
		al_serdes_snps_quad_handle_init(
			(void __iomem *)AL_SRD_CMPLX_MAIN_QUAD_BASE(i),
			&main_complex, i, &main_quads[i]);

	/* SerDes init */
	al_serdes_snps_init(&init_params);
}

