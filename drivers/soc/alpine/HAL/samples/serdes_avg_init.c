#include "serdes_avg_init.h"
#include "al_hal_serdes_avg.h"
#include "al_hal_serdes_avg_init.h"
#include "al_hal_iomap.h"

#define LANE_CFG_CLK0_100MHZ(mode)	\
	{ mode, AL_SRDS_REF_CLK_SEL_0, AL_SRDS_CLK_FREQ_100_MHZ, AL_FALSE, NULL }
#define LANE_CFG_CLK0_156MHZ(mode)	\
	{ mode, AL_SRDS_REF_CLK_SEL_0, AL_SRDS_CLK_FREQ_156_MHZ, AL_FALSE, NULL }

static struct al_serdes_complex_obj main_complex;
static struct al_serdes_grp_obj main_lanes[AL_SRDS_LANES_PER_COMPLEX_TR_MAIN];
static struct al_serdes_avg_lane_cfg main_lanes_lane_cfg[AL_SRDS_LANES_PER_COMPLEX_TR_MAIN] = {
	/*
	 * 0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15
	 * <--------- PCIe 0 ---------->   <-- PCIe 1 ->  <-- PCIe 5 -->
	 */
	LANE_CFG_CLK0_100MHZ(AL_SRDS_AVG_MODE_PCIE_0),
	LANE_CFG_CLK0_100MHZ(AL_SRDS_AVG_MODE_PCIE_0),
	LANE_CFG_CLK0_100MHZ(AL_SRDS_AVG_MODE_PCIE_0),
	LANE_CFG_CLK0_100MHZ(AL_SRDS_AVG_MODE_PCIE_0),
	LANE_CFG_CLK0_100MHZ(AL_SRDS_AVG_MODE_PCIE_0),
	LANE_CFG_CLK0_100MHZ(AL_SRDS_AVG_MODE_PCIE_0),
	LANE_CFG_CLK0_100MHZ(AL_SRDS_AVG_MODE_PCIE_0),
	LANE_CFG_CLK0_100MHZ(AL_SRDS_AVG_MODE_PCIE_0),
	LANE_CFG_CLK0_100MHZ(AL_SRDS_AVG_MODE_PCIE_1),
	LANE_CFG_CLK0_100MHZ(AL_SRDS_AVG_MODE_PCIE_1),
	LANE_CFG_CLK0_100MHZ(AL_SRDS_AVG_MODE_PCIE_1),
	LANE_CFG_CLK0_100MHZ(AL_SRDS_AVG_MODE_PCIE_1),
	LANE_CFG_CLK0_100MHZ(AL_SRDS_AVG_MODE_PCIE_5),
	LANE_CFG_CLK0_100MHZ(AL_SRDS_AVG_MODE_PCIE_5),
	LANE_CFG_CLK0_100MHZ(AL_SRDS_AVG_MODE_PCIE_5),
	LANE_CFG_CLK0_100MHZ(AL_SRDS_AVG_MODE_PCIE_5),
	};

static struct al_serdes_complex_obj sec_complex;
static struct al_serdes_grp_obj sec_lanes[AL_SRDS_LANES_PER_COMPLEX_TR_SEC];
static struct al_serdes_avg_lane_cfg sec_lanes_lane_cfg[AL_SRDS_LANES_PER_COMPLEX_TR_SEC] = {
	/*
	 * 0    1    2    3    4    5    6    7    8    9
	 * <--- PCIe 2 --->    <--- SATA 5 --->    <STD ETH>
	 */
	LANE_CFG_CLK0_100MHZ(AL_SRDS_AVG_MODE_PCIE_2),
	LANE_CFG_CLK0_100MHZ(AL_SRDS_AVG_MODE_PCIE_2),
	LANE_CFG_CLK0_100MHZ(AL_SRDS_AVG_MODE_PCIE_2),
	LANE_CFG_CLK0_100MHZ(AL_SRDS_AVG_MODE_PCIE_2),
	LANE_CFG_CLK0_100MHZ(AL_SRDS_AVG_MODE_SATA_5),
	LANE_CFG_CLK0_100MHZ(AL_SRDS_AVG_MODE_SATA_5),
	LANE_CFG_CLK0_100MHZ(AL_SRDS_AVG_MODE_SATA_5),
	LANE_CFG_CLK0_100MHZ(AL_SRDS_AVG_MODE_SATA_5),
	LANE_CFG_CLK0_156MHZ(AL_SRDS_AVG_MODE_STD_ETH_0_10G),
	LANE_CFG_CLK0_156MHZ(AL_SRDS_AVG_MODE_STD_ETH_1_10G),
	};

static struct al_serdes_complex_obj hs_complex;
static struct al_serdes_grp_obj hs_lanes[AL_SRDS_LANES_PER_COMPLEX_TR_HS];
static struct al_serdes_avg_lane_cfg hs_lanes_lane_cfg[AL_SRDS_LANES_PER_COMPLEX_TR_HS] = {
	/*
	 * 0    1    2    3
	 * <- HS ETH 0/2 ->
	 */
	LANE_CFG_CLK0_156MHZ(AL_SRDS_AVG_MODE_HS_ETH_0_50G),
	LANE_CFG_CLK0_156MHZ(AL_SRDS_AVG_MODE_HS_ETH_0_50G),
	LANE_CFG_CLK0_156MHZ(AL_SRDS_AVG_MODE_HS_ETH_2_50G),
	LANE_CFG_CLK0_156MHZ(AL_SRDS_AVG_MODE_HS_ETH_2_50G),
	};

static struct al_serdes_avg_complex_cfg complex_cfgs[] = {
	{ &main_complex, main_lanes_lane_cfg, AL_ARR_SIZE(main_lanes_lane_cfg) },
	{ &sec_complex, sec_lanes_lane_cfg, AL_ARR_SIZE(sec_lanes_lane_cfg) },
	{ &hs_complex, hs_lanes_lane_cfg, AL_ARR_SIZE(hs_lanes_lane_cfg) },
};

void serdes_avg_init(void)
{
	struct al_serdes_avg_init_params init_params = {
		.complex_cfgs = complex_cfgs,
		.num_complexes = AL_ARR_SIZE(complex_cfgs),
		.fw_pcie_download = AL_TRUE,
		.fw_pcie = NULL,
		.fw_pcie_size = 0,
		.fw_non_pcie_download = AL_TRUE,
		.fw_non_pcie = NULL,
		.fw_non_pcie_size = 0,
		.sbus_master_rom_download = AL_FALSE,
		};
	int i;

	/* Complexes & lanes handle initialization */
	al_serdes_avg_complex_handle_init(
		(void __iomem *)(uintptr_t)AL_SB_SRD_CMPLX_MAIN_BASE,
		(void __iomem *)(uintptr_t)AL_SRD_CMPLX_MAIN_SBUS_MASTER_BASE,
		AL_SRDS_COMPLEX_TYPE_TR_MAIN, NULL, NULL, &main_complex);
	for (i = 0; i < AL_SRDS_LANES_PER_COMPLEX_TR_MAIN; i++)
		al_serdes_avg_handle_init(
			(void __iomem *)AL_SRD_CMPLX_MAIN_LANE_BASE(i),
			&main_complex, i, &main_lanes[i]);

	al_serdes_avg_complex_handle_init(
		(void __iomem *)(uintptr_t)AL_SB_SRD_CMPLX_SEC_BASE,
		(void __iomem *)(uintptr_t)AL_SRD_CMPLX_SEC_SBUS_MASTER_BASE,
		AL_SRDS_COMPLEX_TYPE_TR_SEC, NULL, NULL, &sec_complex);
	for (i = 0; i < AL_SRDS_LANES_PER_COMPLEX_TR_SEC; i++)
		al_serdes_avg_handle_init(
			(void __iomem *)AL_SRD_CMPLX_SEC_LANE_BASE(i),
			&sec_complex, i, &sec_lanes[i]);

	al_serdes_avg_complex_handle_init(
		(void __iomem *)(uintptr_t)AL_SB_SRD_CMPLX_HS_BASE,
		(void __iomem *)(uintptr_t)AL_SRD_CMPLX_HS_SBUS_MASTER_BASE,
		AL_SRDS_COMPLEX_TYPE_TR_HS, NULL, NULL, &hs_complex);
	for (i = 0; i < AL_SRDS_LANES_PER_COMPLEX_TR_HS; i++)
		al_serdes_avg_handle_init(
			(void __iomem *)AL_SRD_CMPLX_HS_LANE_BASE(i),
			&hs_complex, i, &hs_lanes[i]);

	/* SerDes init */
	al_serdes_avg_init(&init_params);
}

