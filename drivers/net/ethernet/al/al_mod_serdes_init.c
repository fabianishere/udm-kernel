/*******************************************************************************
Copyright (C) 2013 Annapurna Labs Ltd.

This file is licensed under the terms of the Annapurna Labs' Commercial License
Agreement distributed with the file or available on the software download site.
Recipient shall use the content of this file only on semiconductor devices or
systems developed by or for Annapurna Labs.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#include "al_mod_serdes_init.h"
#include "al_mod_hal_serdes_25g_init.h"
#include "al_mod_hal_serdes_hssp_init.h"
#include "al_mod_hal_pbs_utils.h"

#if (CHECK_ALPINE_V2) && defined(AL_SERDES_PCIE_ONLY) && (AL_SERDES_PCIE_ONLY)
#define SRD_NUM_GROUPS	AL_SRDS_NUM_HSSP_GROUPS
#else
#define SRD_NUM_GROUPS	AL_SRDS_NUM_GROUPS
#endif

#if (CHECK_ALPINE_V2) && (!defined(AL_SERDES_PCIE_ONLY) || (AL_SERDES_PCIE_ONLY == 0))
#define SRD_GRP_E_EN	1
#else
#define SRD_GRP_E_EN	0
#endif

/* Temporary data used during SerDes initialization */
struct al_mod_serdes_init_grp_tmp {
	/* Core clock frequency */
	enum al_mod_serdes_clk_freq		core_clk_freq;

	/* ICTL PMA value */
	uint32_t			ictl_pma_val;
};

/* Temporary data used during SerDes initialization */
struct al_mod_serdes_init_tmp {
	/* Parent object */
	struct al_mod_serdes_grp_obj	*obj[AL_SRDS_NUM_GROUPS];

	/* Required configuration */
	struct al_mod_serdes_cfg		cfg;

	/* Group temporary informaion */
	struct al_mod_serdes_init_grp_tmp	grp[AL_SRDS_NUM_GROUPS];
};

/**
 * One-hot 0 - select input 0 in a one hot selector
 */
#define OH0	(1 << 0)

/**
 * One-hot 1 - select input 1 in a one hot selector
 */
#define OH1	(1 << 1)

/**
 * One-hot 2 - select input 2 in a one hot selector
 */
#define OH2	(1 << 2)

/**
 * SERDES clock routing initialization
 */
static int al_mod_serdes_init_clk_route(
	struct al_mod_serdes_init_tmp	*init_tmp);

/**
 * SERDES cores initialization
 */
static int al_mod_serdes_init_cores(
	struct al_mod_serdes_init_tmp	*init_tmp);

/**
 * SERDES R2L clock frequency update according to configured clock routing
 */
static int al_mod_serdes_group_r2l_clk_freq_update(
	struct al_mod_serdes_group_cfg	*grp_cfg,
	enum al_mod_serdes_clk_freq		r2l_in_freq,
	enum al_mod_serdes_clk_freq		*r2l_out_freq);

/**
 * SERDES L2R clock frequency update according to configured clock routing
 */
static int al_mod_serdes_group_l2r_clk_freq_update(
	struct al_mod_serdes_group_cfg	*grp_cfg,
	enum al_mod_serdes_clk_freq		l2r_in_freq,
	enum al_mod_serdes_clk_freq		*l2r_out_freq);

/**
 * SERDES core clock frequency update according to configured clock routing
 */
static int al_mod_serdes_group_core_clk_freq_update(
	struct al_mod_serdes_group_cfg	*grp_cfg,
	struct al_mod_serdes_init_grp_tmp	*init_grp_tmp);

/**
 * SERDES mux initialization
 */
#if CHECK_ALPINE_V1
static int al_mod_serdes_init_mux(
	void __iomem			*pbs_regs_base,
	struct al_mod_serdes_init_tmp	*init_tmp);
#endif

#if CHECK_ALPINE_V2
static int  al_mod_serdes_init_mux_alpine_v2(
	void __iomem			*pbs_regs_base,
	struct al_mod_serdes_init_tmp	*init_tmp);
#endif


/******************************************************************************/
/******************************************************************************/
static const char *al_mod_serdes_group_clk_src_convert_to_str(
	enum al_mod_serdes_clk_src val)
{
	switch (val) {
	case AL_SRDS_CLK_SRC_LOGIC_0:
		return "LOGIC_0";
	case AL_SRDS_CLK_SRC_REF_PINS:
		return "REF_PINS";
	case AL_SRDS_CLK_SRC_R2L:
		return "R2L";
	case AL_SRDS_CLK_SRC_R2L_PLL:
		return "R2L_PLL";
	case AL_SRDS_CLK_SRC_L2R:
		return "L2R";
	}

	al_mod_err("%s: invalid value (%d)!\n", __func__, val);

	return "";
}

/******************************************************************************/
/******************************************************************************/
static const char *al_mod_serdes_group_clk_freq_convert_to_str(
	enum al_mod_serdes_clk_freq val)
{
	switch (val) {
	case AL_SRDS_CLK_FREQ_NA:
		return "N/A";
	case AL_SRDS_CLK_FREQ_100_MHZ:
		return "100Mhz";
	case AL_SRDS_CLK_FREQ_125_MHZ:
		return "125Mhz";
	case AL_SRDS_CLK_FREQ_156_MHZ:
		return "156Mhz";
	}

	al_mod_err("%s: invalid value (%d)!\n", __func__, val);

	return "";
}

/******************************************************************************/
/******************************************************************************/
static const char *al_mod_serdes_group_mode_convert_to_str(
	enum al_mod_serdes_group_mode val)
{
	switch (val) {
	case AL_SRDS_CFG_OFF:
		return "OFF";
	case AL_SRDS_CFG_OFF_BP:
		return "OFF_BP";
	case AL_SRDS_CFG_SKIP:
		return "SKIP";
	case AL_SRDS_CFG_PCIE_A_G2X2_PCIE_B_G2X2:
		return "PCIE_A_G2X2_PCIE_B_G2X2";
	case AL_SRDS_CFG_PCIE_A_G2X4:
		return "PCIE_A_G2X4";
	case AL_SRDS_CFG_PCIE_A_G3X4:
		return "PCIE_A_G3X4";
	case AL_SRDS_CFG_PCIE_A_G3X8:
		return "PCIE_A_G3X8";
	case AL_SRDS_CFG_SATA_A:
		return "SATA_A";
	case AL_SRDS_CFG_PCIE_B_G2X4:
		return "PCIE_B_G2X4";
	case AL_SRDS_CFG_PCIE_B_G3X4:
		return "PCIE_B_G3X4";
	case AL_SRDS_CFG_PCIE_C_G2X2_PCIE_D_G2X2:
		return "PCIE_C_G2X2_PCIE_D_G2X2";
	case AL_SRDS_CFG_PCIE_C_G2X4:
		return "PCIE_C_G2X4";
	case AL_SRDS_CFG_PCIE_C_G3X4:
		return "PCIE_C_G3X4";
	case AL_SRDS_CFG_PCIE_D_G2X4:
		return "PCIE_D_G2X4";
	case AL_SRDS_CFG_PCIE_D_G3X4:
		return "PCIE_D_G3X4";
	case AL_SRDS_CFG_SATA_B:
		return "SATA_B";
	case AL_SRDS_CFG_SGMII:
		return "SGMII";
	case AL_SRDS_CFG_SGMII_2_5G:
		return "SGMII_2_5G";
	case AL_SRDS_CFG_KR:
		return "KR";
	case AL_SRDS_CFG_ETH_A_XLAUI:
		return "ETH_A_XLAUI";
	case AL_SRDS_CFG_ETH_C_XLAUI:
		return "ETH_C_XLAUI";
	case AL_SRDS_CFG_ETH_25G:
		return "ETH_25G";
	}

	al_mod_err("%s: invalid value (%d)!\n", __func__, val);

	return "";
}

/******************************************************************************/
/******************************************************************************/
int al_mod_serdes_init(
	struct al_mod_serdes_grp_obj	*obj,
	void __iomem			*pbs_regs_base,
	const struct al_mod_serdes_cfg	*cfg)
{
	struct al_mod_serdes_init_tmp init_tmp;
	int err = 0;
	int i;
#if (CHECK_ALPINE_V1 || CHECK_ALPINE_V2)
	unsigned int dev_id;
#endif

	al_mod_assert(obj);
	al_mod_assert(pbs_regs_base);
	al_mod_assert(cfg);

	al_mod_dbg(
		"%s(%p, %p, (%d, %d, %d, %d))\n",
		__func__,
		obj,
		pbs_regs_base,
		cfg->grp_cfg[0].mode,
		cfg->grp_cfg[1].mode,
		cfg->grp_cfg[2].mode,
		cfg->grp_cfg[3].mode);

	init_tmp.cfg = *cfg;
	for (i = 0; i < SRD_NUM_GROUPS; i++)
		init_tmp.obj[i] = &obj[i];

#if (CHECK_ALPINE_V1 || CHECK_ALPINE_V2)
	dev_id = al_mod_pbs_dev_id_get(pbs_regs_base);
#endif

#if CHECK_ALPINE_V1
	if (dev_id == PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V1)
		err = al_mod_serdes_init_mux(pbs_regs_base, &init_tmp);
	else
#endif
#if CHECK_ALPINE_V2
	if (dev_id == PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V2)
		err = al_mod_serdes_init_mux_alpine_v2(pbs_regs_base, &init_tmp);
	else
#endif
	{ }

	if (err) {
		al_mod_err("%s: al_mod_serdes_init_mux failed!\n", __func__);
		return err;
	}

	err = al_mod_serdes_init_clk_route(&init_tmp);
	if (err) {
		al_mod_err("%s: al_mod_serdes_init_clk_route failed!\n", __func__);
		return err;
	}

	err = al_mod_serdes_init_cores(&init_tmp);
	if (err) {
		al_mod_err("%s: al_mod_serdes_init_cores failed!\n", __func__);
		return err;
	}

	for (i = 0; i < SRD_NUM_GROUPS; i++) {
		struct al_mod_serdes_group_cfg *grp_cfg = &init_tmp.cfg.grp_cfg[i];
		struct al_mod_serdes_init_grp_tmp *init_grp_tmp = &init_tmp.grp[i];

		al_mod_dbg("Serdes group %d configuration:\n", i);
		al_mod_dbg(" - Mode: %s, Core clk freq: %s from %s, Ref clk freq: %s\n",
			al_mod_serdes_group_mode_convert_to_str(grp_cfg->mode),
			al_mod_serdes_group_clk_freq_convert_to_str(init_grp_tmp->core_clk_freq),
			al_mod_serdes_group_clk_src_convert_to_str(grp_cfg->clk_src_core),
			al_mod_serdes_group_clk_freq_convert_to_str(grp_cfg->ref_clk_freq));
		al_mod_dbg(" - R2L clk freq: %s from %s, L2R clk freq: %s from %s\n",
			al_mod_serdes_group_clk_freq_convert_to_str(grp_cfg->r2l_clk_freq),
			al_mod_serdes_group_clk_src_convert_to_str(grp_cfg->clk_src_r2l),
			al_mod_serdes_group_clk_freq_convert_to_str(grp_cfg->l2r_clk_freq),
			al_mod_serdes_group_clk_src_convert_to_str(grp_cfg->clk_src_l2r));
		al_mod_dbg(" - Active lanes: %s%s%s%s\n",
			(grp_cfg->active_lanes[0] == AL_TRUE) ? "0, " : "",
			(grp_cfg->active_lanes[1] == AL_TRUE) ? "1, " : "",
			(grp_cfg->active_lanes[2] == AL_TRUE) ? "2, " : "",
			(grp_cfg->active_lanes[3] == AL_TRUE) ? "3, " : "");
		al_mod_dbg(" - SSC: %s\n",
			(grp_cfg->ssc_en == AL_TRUE) ? "enabled" : "disabled");
		al_mod_dbg(" - Inverted Tx lanes: %s%s%s%s\n",
			(grp_cfg->inv_tx_lanes[0] == AL_TRUE) ? "0, " : "",
			(grp_cfg->inv_tx_lanes[1] == AL_TRUE) ? "1, " : "",
			(grp_cfg->inv_tx_lanes[2] == AL_TRUE) ? "2, " : "",
			(grp_cfg->inv_tx_lanes[3] == AL_TRUE) ? "3, " : "");
		al_mod_dbg(" - Inverted Rx lanes: %s%s%s%s\n",
			(grp_cfg->inv_rx_lanes[0] == AL_TRUE) ? "0, " : "",
			(grp_cfg->inv_rx_lanes[1] == AL_TRUE) ? "1, " : "",
			(grp_cfg->inv_rx_lanes[2] == AL_TRUE) ? "2, " : "",
			(grp_cfg->inv_rx_lanes[3] == AL_TRUE) ? "3, " : "");
	}

	return 0;
}

/******************************************************************************/
/******************************************************************************/
#if CHECK_ALPINE_V1
static int al_mod_serdes_init_mux(
	void __iomem			*pbs_regs_base,
	struct al_mod_serdes_init_tmp	*init_tmp)
{
	struct al_mod_pbs_regs __iomem *pbs_regs = (struct al_mod_pbs_regs __iomem *)pbs_regs_base;
	int status = 0;

	uint32_t serdes_mux_pipe;
	uint32_t serdes_mux_multi0;
	uint32_t serdes_mux_multi1;

	serdes_mux_pipe =
		al_mod_reg_read32(&pbs_regs->unit.serdes_mux_pipe);
	serdes_mux_multi0 =
		al_mod_reg_read32(&pbs_regs->unit.serdes_mux_multi_0);
	serdes_mux_multi1 =
		al_mod_reg_read32(&pbs_regs->unit.serdes_mux_multi_1);

	switch (init_tmp->cfg.grp_cfg[AL_SRDS_GRP_A].mode) {
	case AL_SRDS_CFG_PCIE_A_G2X2_PCIE_B_G2X2:
		serdes_mux_pipe &=
			~PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_SERDES_2_MASK;
		serdes_mux_pipe |= (OH1) <<
			PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_SERDES_2_SHIFT;

		serdes_mux_pipe &=
			~PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_SERDES_3_MASK;
		serdes_mux_pipe |= (OH1) <<
			PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_SERDES_3_SHIFT;

		serdes_mux_pipe &=
			~PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_CLKI_SER_2_MASK;
		serdes_mux_pipe |= (OH0) <<
			PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_CLKI_SER_2_SHIFT;

		serdes_mux_pipe &=
			~PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_CLKI_SER_3_MASK;
		serdes_mux_pipe |= (OH1) <<
			PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_CLKI_SER_3_SHIFT;

		serdes_mux_pipe &=
			~PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_PCI_B_0_MASK;
		serdes_mux_pipe |= (OH1) <<
			PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_PCI_B_0_SHIFT;

		serdes_mux_pipe &=
			~PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_PCI_B_1_MASK;
		serdes_mux_pipe |= (OH1) <<
			PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_PCI_B_1_SHIFT;

		break;

	case AL_SRDS_CFG_PCIE_A_G2X4:
	case AL_SRDS_CFG_PCIE_A_G3X4:
		serdes_mux_pipe &=
			~PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_SERDES_2_MASK;
		serdes_mux_pipe |= (OH2) <<
			PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_SERDES_2_SHIFT;

		serdes_mux_pipe &=
			~PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_SERDES_3_MASK;
		serdes_mux_pipe |= (OH2) <<
			PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_SERDES_3_SHIFT;

		serdes_mux_pipe &=
			~PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_CLKI_SER_2_MASK;
		serdes_mux_pipe |= (OH1) <<
			PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_CLKI_SER_2_SHIFT;

		serdes_mux_pipe &=
			~PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_CLKI_SER_3_MASK;
		serdes_mux_pipe |= (OH2) <<
			PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_CLKI_SER_3_SHIFT;

		break;

	case AL_SRDS_CFG_KR:
	case AL_SRDS_CFG_OFF:
	case AL_SRDS_CFG_OFF_BP:
	case AL_SRDS_CFG_SKIP:
		break;

	default:
		al_mod_err(
			"%s: invalid group A configuration: %d\n",
			__func__,
			init_tmp->cfg.grp_cfg[AL_SRDS_GRP_A].mode);
			status = -EINVAL;
			goto done;
	}

	switch (init_tmp->cfg.grp_cfg[AL_SRDS_GRP_B].mode) {
	case AL_SRDS_CFG_SATA_A:
		/**
		 * Group B can only be either SATA or PCIE - Multi or Pipe
		 * For Multi no special configuration is required - later when
		 * the core is configured, Multi is selected.
		 */
		break;

	case AL_SRDS_CFG_PCIE_B_G2X4:
	case AL_SRDS_CFG_PCIE_B_G3X4:
		serdes_mux_pipe &=
			~PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_PCI_B_0_MASK;
		serdes_mux_pipe |= (OH0) <<
			PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_PCI_B_0_SHIFT;
		break;

	case AL_SRDS_CFG_KR:
	case AL_SRDS_CFG_OFF:
	case AL_SRDS_CFG_OFF_BP:
	case AL_SRDS_CFG_SKIP:
		break;

	default:
		al_mod_err(
			"%s: invalid group B configuration: %d\n",
			__func__,
			init_tmp->cfg.grp_cfg[AL_SRDS_GRP_B].mode);
		status = -EINVAL;
		goto done;
	}

	switch (init_tmp->cfg.grp_cfg[AL_SRDS_GRP_C].mode) {
	case AL_SRDS_CFG_SATA_B:
		serdes_mux_multi0 &=
			~PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_8_MASK;
		serdes_mux_multi0 |= (OH0) <<
			PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_8_SHIFT;

		serdes_mux_multi0 &=
			~PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_9_MASK;
		serdes_mux_multi0 |= (OH0) <<
			PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_9_SHIFT;

		serdes_mux_multi0 &=
			~PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_10_MASK;
		serdes_mux_multi0 |= (OH0) <<
			PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_10_SHIFT;

		serdes_mux_multi0 &=
			~PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_11_MASK;
		serdes_mux_multi0 |= (OH0) <<
			PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_11_SHIFT;

		break;

	case AL_SRDS_CFG_SGMII:
	case AL_SRDS_CFG_SGMII_2_5G:
	case AL_SRDS_CFG_KR:
		serdes_mux_multi0 &=
			~PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_8_MASK;
		serdes_mux_multi0 |= (OH1) <<
			PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_8_SHIFT;

		serdes_mux_multi0 &=
			~PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_9_MASK;
		serdes_mux_multi0 |= (OH1) <<
			PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_9_SHIFT;

		serdes_mux_multi0 &=
			~PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_10_MASK;
		serdes_mux_multi0 |= (OH1) <<
			PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_10_SHIFT;

		serdes_mux_multi0 &=
			~PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_11_MASK;
		serdes_mux_multi0 |= (OH1) <<
			PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_11_SHIFT;

		serdes_mux_multi1 &=
			~PBS_UNIT_SERDES_MUX_MULTI_1_SELECT_OH_ETH_A_0_MASK;
		serdes_mux_multi1 |= (OH0) <<
			PBS_UNIT_SERDES_MUX_MULTI_1_SELECT_OH_ETH_A_0_SHIFT;

		serdes_mux_multi1 &=
			~PBS_UNIT_SERDES_MUX_MULTI_1_SELECT_OH_ETH_B_0_MASK;
		serdes_mux_multi1 |= (OH1) <<
			PBS_UNIT_SERDES_MUX_MULTI_1_SELECT_OH_ETH_B_0_SHIFT;

		serdes_mux_multi1 &=
			~PBS_UNIT_SERDES_MUX_MULTI_1_SELECT_OH_ETH_C_0_MASK;
		serdes_mux_multi1 |= (OH0) <<
			PBS_UNIT_SERDES_MUX_MULTI_1_SELECT_OH_ETH_C_0_SHIFT;

		serdes_mux_multi1 &=
			~PBS_UNIT_SERDES_MUX_MULTI_1_SELECT_OH_ETH_D_0_MASK;
		serdes_mux_multi1 |= (OH2) <<
			PBS_UNIT_SERDES_MUX_MULTI_1_SELECT_OH_ETH_D_0_SHIFT;

		break;

	case AL_SRDS_CFG_PCIE_C_G2X4:
	case AL_SRDS_CFG_PCIE_C_G3X4:
		serdes_mux_pipe &=
			~PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_PCI_C_0_MASK;
		serdes_mux_pipe |= (OH0) <<
			PBS_UNIT_SERDES_MUX_PIPE_SELECT_OH_PCI_C_0_SHIFT;

		break;

	case AL_SRDS_CFG_OFF:
	case AL_SRDS_CFG_OFF_BP:
	case AL_SRDS_CFG_SKIP:
		break;

	default:
		al_mod_err(
			"%s: invalid group C configuration: %d\n",
			__func__,
			init_tmp->cfg.grp_cfg[AL_SRDS_GRP_C].mode);
		status = -EINVAL;
		goto done;
	}

	switch (init_tmp->cfg.grp_cfg[AL_SRDS_GRP_D].mode) {
	case AL_SRDS_CFG_SGMII:
	case AL_SRDS_CFG_SGMII_2_5G:
	case AL_SRDS_CFG_KR:
		serdes_mux_multi0 &=
			~PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_12_MASK;
		serdes_mux_multi0 |= (OH0) <<
			PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_12_SHIFT;

		serdes_mux_multi0 &=
			~PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_13_MASK;
		serdes_mux_multi0 |= (OH0) <<
			PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_13_SHIFT;

		serdes_mux_multi0 &=
			~PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_14_MASK;
		serdes_mux_multi0 |= (OH0) <<
			PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_14_SHIFT;

		serdes_mux_multi0 &=
			~PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_15_MASK;
		serdes_mux_multi0 |= (OH0) <<
			PBS_UNIT_SERDES_MUX_MULTI_0_SELECT_OH_SERDES_15_SHIFT;
		/*
		 * Handle the case where both SGMII and KR ports exist
		 * In this case SRDS D will be used only for its active lanes
		 */
		if (init_tmp->cfg.grp_cfg[AL_SRDS_GRP_D].active_lanes[0]) {
			serdes_mux_multi1 &=
				~PBS_UNIT_SERDES_MUX_MULTI_1_SELECT_OH_ETH_A_0_MASK;
			serdes_mux_multi1 |= (OH1) <<
				PBS_UNIT_SERDES_MUX_MULTI_1_SELECT_OH_ETH_A_0_SHIFT;
		}
		if (init_tmp->cfg.grp_cfg[AL_SRDS_GRP_D].active_lanes[1]) {
			serdes_mux_multi1 &=
				~PBS_UNIT_SERDES_MUX_MULTI_1_SELECT_OH_ETH_B_0_MASK;
			serdes_mux_multi1 |= (OH2) <<
				PBS_UNIT_SERDES_MUX_MULTI_1_SELECT_OH_ETH_B_0_SHIFT;
		}
		if (init_tmp->cfg.grp_cfg[AL_SRDS_GRP_D].active_lanes[2]) {
			serdes_mux_multi1 &=
				~PBS_UNIT_SERDES_MUX_MULTI_1_SELECT_OH_ETH_C_0_MASK;
			serdes_mux_multi1 |= (OH1) <<
				PBS_UNIT_SERDES_MUX_MULTI_1_SELECT_OH_ETH_C_0_SHIFT;
		}
		if (init_tmp->cfg.grp_cfg[AL_SRDS_GRP_D].active_lanes[3]) {
			serdes_mux_multi1 &=
				~PBS_UNIT_SERDES_MUX_MULTI_1_SELECT_OH_ETH_D_0_MASK;
			serdes_mux_multi1 |= (OH1) <<
				PBS_UNIT_SERDES_MUX_MULTI_1_SELECT_OH_ETH_D_0_SHIFT;
		}

		break;

	case AL_SRDS_CFG_OFF:
	case AL_SRDS_CFG_OFF_BP:
	case AL_SRDS_CFG_SKIP:
		break;

	default:
		al_mod_err(
			"%s: invalid group D configuration: %d\n",
			__func__,
			init_tmp->cfg.grp_cfg[AL_SRDS_GRP_D].mode);
		status = -EINVAL;
		goto done;
	}

	al_mod_reg_write32(
		&pbs_regs->unit.serdes_mux_pipe,
		serdes_mux_pipe);
	al_mod_reg_write32(
		&pbs_regs->unit.serdes_mux_multi_0,
		serdes_mux_multi0);
	al_mod_reg_write32(
		&pbs_regs->unit.serdes_mux_multi_1,
		serdes_mux_multi1);

done:
	return status;
}
#endif

/******************************************************************************/
/******************************************************************************/
#if CHECK_ALPINE_V2
static int al_mod_serdes_init_mux_alpine_v2(
	void __iomem			*pbs_regs_base,
	struct al_mod_serdes_init_tmp	*init_tmp)
{
	struct al_mod_pbs_regs __iomem *pbs_regs = (struct al_mod_pbs_regs __iomem *)pbs_regs_base;
	int status = 0;

	uint32_t serdes_mux_pipe;
	uint32_t serdes_mux_multi_0;
	uint32_t serdes_mux_eth;
	uint32_t serdes_mux_pcie;
	uint32_t serdes_mux_sata;

	serdes_mux_pipe =
		al_mod_reg_read32(&pbs_regs->unit.serdes_mux_pipe);
	serdes_mux_multi_0 =
		al_mod_reg_read32(&pbs_regs->unit.serdes_mux_multi_0);
	serdes_mux_eth =
		al_mod_reg_read32(&pbs_regs->unit.serdes_mux_eth);
	serdes_mux_pcie =
		al_mod_reg_read32(&pbs_regs->unit.serdes_mux_pcie);
	serdes_mux_sata =
		al_mod_reg_read32(&pbs_regs->unit.serdes_mux_sata);

	if (init_tmp->cfg.grp_cfg[AL_SRDS_GRP_A].mode != AL_SRDS_CFG_SKIP) {
		/*
		 * 2'b01 - select pcie_b[0]
		 * 2'b10 - select pcie_a[2]
		 */
		serdes_mux_pipe &=
			~PBS_UNIT_SERDES_MUX_PIPE_ALPINE_V2_SELECT_OH_SERDES_2_MASK;
		serdes_mux_pipe |=
			((init_tmp->cfg.grp_cfg[AL_SRDS_GRP_A].mode ==
			  AL_SRDS_CFG_PCIE_A_G2X2_PCIE_B_G2X2) ? OH0 : OH1) <<
			PBS_UNIT_SERDES_MUX_PIPE_ALPINE_V2_SELECT_OH_SERDES_2_SHIFT;

		/*
		 * 2'b01 - select pcie_b[1]
		 * 2'b10 - select pcie_a[3]
		 */
		serdes_mux_pipe &=
			~PBS_UNIT_SERDES_MUX_PIPE_ALPINE_V2_SELECT_OH_SERDES_3_MASK;
		serdes_mux_pipe |=
			((init_tmp->cfg.grp_cfg[AL_SRDS_GRP_A].mode ==
			  AL_SRDS_CFG_PCIE_A_G2X2_PCIE_B_G2X2) ? OH0 : OH1) <<
			PBS_UNIT_SERDES_MUX_PIPE_ALPINE_V2_SELECT_OH_SERDES_3_SHIFT;
	}

	if (init_tmp->cfg.grp_cfg[AL_SRDS_GRP_B].mode != AL_SRDS_CFG_SKIP) {
		/*
		 * 2'b01 - select pcie_b[0]
		 * 2'b10 - select pcie_a[4]
		 */
		serdes_mux_pipe &=
			~PBS_UNIT_SERDES_MUX_PIPE_ALPINE_V2_SELECT_OH_SERDES_4_MASK;
		serdes_mux_pipe |=
			((init_tmp->cfg.grp_cfg[AL_SRDS_GRP_B].mode == AL_SRDS_CFG_PCIE_A_G3X8) ?
			OH1 : OH0) <<
			PBS_UNIT_SERDES_MUX_PIPE_ALPINE_V2_SELECT_OH_SERDES_4_SHIFT;
		/*
		 * 2'b01 - select pcie_b[1]
		 * 2'b10 - select pcie_a[5]
		 */
		serdes_mux_pipe &=
			~PBS_UNIT_SERDES_MUX_PIPE_ALPINE_V2_SELECT_OH_SERDES_5_MASK;
		serdes_mux_pipe |=
			((init_tmp->cfg.grp_cfg[AL_SRDS_GRP_B].mode == AL_SRDS_CFG_PCIE_A_G3X8) ?
			OH1 : OH0) <<
			PBS_UNIT_SERDES_MUX_PIPE_ALPINE_V2_SELECT_OH_SERDES_5_SHIFT;
		/*
		 * 2'b01 - select pcie_b[2]
		 * 2'b10 - select pcie_a[6]
		 */
		serdes_mux_pipe &=
			~PBS_UNIT_SERDES_MUX_PIPE_ALPINE_V2_SELECT_OH_SERDES_6_MASK;
		serdes_mux_pipe |=
			((init_tmp->cfg.grp_cfg[AL_SRDS_GRP_B].mode ==
			  AL_SRDS_CFG_PCIE_A_G3X8) ? OH1 : OH0) <<
			PBS_UNIT_SERDES_MUX_PIPE_ALPINE_V2_SELECT_OH_SERDES_6_SHIFT;
		/*
		 * 2'b01 - select pcie_b[3]
		 * 2'b10 - select pcie_a[7]
		 */
		serdes_mux_pipe &=
			~PBS_UNIT_SERDES_MUX_PIPE_ALPINE_V2_SELECT_OH_SERDES_7_MASK;
		serdes_mux_pipe |=
			((init_tmp->cfg.grp_cfg[AL_SRDS_GRP_B].mode ==
			  AL_SRDS_CFG_PCIE_A_G3X8) ? OH1 : OH0) <<
			PBS_UNIT_SERDES_MUX_PIPE_ALPINE_V2_SELECT_OH_SERDES_7_SHIFT;
	}

	if (init_tmp->cfg.grp_cfg[AL_SRDS_GRP_C].mode != AL_SRDS_CFG_SKIP) {
		/*
		 * 2'b01 - select pcie_c[2]
		 * 2'b10 - select pcie_d[0]
		 */
		serdes_mux_pipe &=
			~PBS_UNIT_SERDES_MUX_PIPE_ALPINE_V2_SELECT_OH_SERDES_10_MASK;
		serdes_mux_pipe |=
			((init_tmp->cfg.grp_cfg[AL_SRDS_GRP_C].mode ==
			AL_SRDS_CFG_PCIE_C_G2X2_PCIE_D_G2X2) ? OH1 : OH0) <<
			PBS_UNIT_SERDES_MUX_PIPE_ALPINE_V2_SELECT_OH_SERDES_10_SHIFT;
		/*
		 * 2'b01 - select pcie_c[3]
		 * 2'b10 - select pcie_d[1]
		 */
		serdes_mux_pipe &=
			~PBS_UNIT_SERDES_MUX_PIPE_ALPINE_V2_SELECT_OH_SERDES_11_MASK;
		serdes_mux_pipe |=
			((init_tmp->cfg.grp_cfg[AL_SRDS_GRP_C].mode ==
			AL_SRDS_CFG_PCIE_C_G2X2_PCIE_D_G2X2) ? OH1 : OH0) <<
			PBS_UNIT_SERDES_MUX_PIPE_ALPINE_V2_SELECT_OH_SERDES_11_SHIFT;

		/*
		 * 2'b01 - select sata_b[0]
		 * 2'b10 - select eth_a[0]
		 */
		serdes_mux_multi_0 &=
			~PBS_UNIT_SERDES_MUX_MULTI_0_ALPINE_V2_SELECT_OH_SERDES_8_MASK;
		serdes_mux_multi_0 |=
			((init_tmp->cfg.grp_cfg[AL_SRDS_GRP_C].mode == AL_SRDS_CFG_SATA_B) ?
			OH0 : OH1) <<
			PBS_UNIT_SERDES_MUX_MULTI_0_ALPINE_V2_SELECT_OH_SERDES_8_SHIFT;
		/*
		 * 3'b001 - select sata_b[1]
		 * 3'b010 - select eth_b[0]
		 * 3'b100 - select eth_a[1]
		 */
		serdes_mux_multi_0 &=
			~PBS_UNIT_SERDES_MUX_MULTI_0_ALPINE_V2_SELECT_OH_SERDES_9_MASK;
		serdes_mux_multi_0 |=
			((init_tmp->cfg.grp_cfg[AL_SRDS_GRP_C].mode == AL_SRDS_CFG_SATA_B) ? OH0 :
			((init_tmp->cfg.grp_cfg[AL_SRDS_GRP_C].mode == AL_SRDS_CFG_ETH_A_XLAUI) ?
			OH2 : OH1)) <<
			PBS_UNIT_SERDES_MUX_MULTI_0_ALPINE_V2_SELECT_OH_SERDES_9_SHIFT;
		/*
		 * 3'b001 - select sata_b[2]
		 * 3'b010 - select eth_c[0]
		 * 3'b100 - select eth_a[2]
		 */
		serdes_mux_multi_0 &=
			~PBS_UNIT_SERDES_MUX_MULTI_0_ALPINE_V2_SELECT_OH_SERDES_10_MASK;
		serdes_mux_multi_0 |=
			((init_tmp->cfg.grp_cfg[AL_SRDS_GRP_C].mode == AL_SRDS_CFG_SATA_B) ? OH0 :
			((init_tmp->cfg.grp_cfg[AL_SRDS_GRP_C].mode == AL_SRDS_CFG_ETH_A_XLAUI) ?
			OH2 : OH1)) <<
			PBS_UNIT_SERDES_MUX_MULTI_0_ALPINE_V2_SELECT_OH_SERDES_10_SHIFT;
		/*
		 * 3'b001 - select sata_b[3]
		 * 3'b010 - select eth_d[0]
		 * 3'b100 - select eth_a[3]
		 */
		serdes_mux_multi_0 &=
			~PBS_UNIT_SERDES_MUX_MULTI_0_ALPINE_V2_SELECT_OH_SERDES_11_MASK;
		serdes_mux_multi_0 |=
			((init_tmp->cfg.grp_cfg[AL_SRDS_GRP_C].mode == AL_SRDS_CFG_SATA_B) ? OH0 :
			((init_tmp->cfg.grp_cfg[AL_SRDS_GRP_C].mode == AL_SRDS_CFG_ETH_A_XLAUI) ?
			OH2 : OH1)) <<
			PBS_UNIT_SERDES_MUX_MULTI_0_ALPINE_V2_SELECT_OH_SERDES_11_SHIFT;
	}

	if (init_tmp->cfg.grp_cfg[AL_SRDS_GRP_D].mode != AL_SRDS_CFG_SKIP) {
		/*
		 * 2'b01 - select eth_c[0]
		 * 2'b10 - select sata_a[0]
		 */
		serdes_mux_multi_0 &=
			~PBS_UNIT_SERDES_MUX_MULTI_0_ALPINE_V2_SELECT_OH_SERDES_12_MASK;
		serdes_mux_multi_0 |=
			((init_tmp->cfg.grp_cfg[AL_SRDS_GRP_D].mode == AL_SRDS_CFG_SATA_A) ?
			OH1 : OH0) <<
			PBS_UNIT_SERDES_MUX_MULTI_0_ALPINE_V2_SELECT_OH_SERDES_12_SHIFT;
		/*
		 * 3'b001 - select eth_b[0]
		 * 3'b010 - select eth_c[1]
		 * 3'b100 - select sata_a[1]
		 */
		serdes_mux_multi_0 &=
			~PBS_UNIT_SERDES_MUX_MULTI_0_ALPINE_V2_SELECT_OH_SERDES_13_MASK;
		serdes_mux_multi_0 |=
			((init_tmp->cfg.grp_cfg[AL_SRDS_GRP_D].mode == AL_SRDS_CFG_SATA_A) ? OH2 :
			((init_tmp->cfg.grp_cfg[AL_SRDS_GRP_D].mode == AL_SRDS_CFG_ETH_C_XLAUI) ?
			OH1 : OH0)) <<
			PBS_UNIT_SERDES_MUX_MULTI_0_ALPINE_V2_SELECT_OH_SERDES_13_SHIFT;
		/*
		 * 3'b001 - select eth_a[0]
		 * 3'b010 - select eth_c[2]
		 * 3'b100 - select sata_a[2]
		 */
		serdes_mux_multi_0 &=
			~PBS_UNIT_SERDES_MUX_MULTI_0_ALPINE_V2_SELECT_OH_SERDES_14_MASK;
		serdes_mux_multi_0 |=
			((init_tmp->cfg.grp_cfg[AL_SRDS_GRP_D].mode == AL_SRDS_CFG_SATA_A) ? OH2 :
			((init_tmp->cfg.grp_cfg[AL_SRDS_GRP_D].mode == AL_SRDS_CFG_ETH_C_XLAUI) ?
			OH1 : OH0)) <<
			PBS_UNIT_SERDES_MUX_MULTI_0_ALPINE_V2_SELECT_OH_SERDES_14_SHIFT;
		/*
		 * 3'b001 - select eth_d[0]
		 * 3'b010 - select eth_c[3]
		 * 3'b100 - select sata_a[3]
		 */
		serdes_mux_multi_0 &=
			~PBS_UNIT_SERDES_MUX_MULTI_0_ALPINE_V2_SELECT_OH_SERDES_15_MASK;
		serdes_mux_multi_0 |=
			((init_tmp->cfg.grp_cfg[AL_SRDS_GRP_D].mode == AL_SRDS_CFG_SATA_A) ? OH2 :
			((init_tmp->cfg.grp_cfg[AL_SRDS_GRP_D].mode == AL_SRDS_CFG_ETH_C_XLAUI) ?
			OH1 : OH0)) <<
			PBS_UNIT_SERDES_MUX_MULTI_0_ALPINE_V2_SELECT_OH_SERDES_15_SHIFT;
	}

	if ((init_tmp->cfg.grp_cfg[AL_SRDS_GRP_C].mode == AL_SRDS_CFG_SGMII) ||
		(init_tmp->cfg.grp_cfg[AL_SRDS_GRP_C].mode == AL_SRDS_CFG_KR) ||
		(init_tmp->cfg.grp_cfg[AL_SRDS_GRP_C].mode == AL_SRDS_CFG_ETH_A_XLAUI)) {
		/* 2'b01 - eth_a[0] from serdes_8 */
		serdes_mux_eth &=
			~PBS_UNIT_SERDES_MUX_ETH_ALPINE_V2_SELECT_OH_ETH_A_0_MASK;
		serdes_mux_eth |=
			OH0 << PBS_UNIT_SERDES_MUX_ETH_ALPINE_V2_SELECT_OH_ETH_A_0_SHIFT;
	} else if ((init_tmp->cfg.grp_cfg[AL_SRDS_GRP_D].mode == AL_SRDS_CFG_SGMII) ||
		(init_tmp->cfg.grp_cfg[AL_SRDS_GRP_D].mode == AL_SRDS_CFG_KR)) {
		/* 2'b10 - eth_a[0] from serdes_14 */
		serdes_mux_eth &=
			~PBS_UNIT_SERDES_MUX_ETH_ALPINE_V2_SELECT_OH_ETH_A_0_MASK;
		serdes_mux_eth |=
			OH1 << PBS_UNIT_SERDES_MUX_ETH_ALPINE_V2_SELECT_OH_ETH_A_0_SHIFT;
	}

	if ((init_tmp->cfg.grp_cfg[AL_SRDS_GRP_C].mode == AL_SRDS_CFG_SGMII) ||
		(init_tmp->cfg.grp_cfg[AL_SRDS_GRP_C].mode == AL_SRDS_CFG_KR)) {
		/* 2'b01 - eth_b[0] from serdes_9 */
		serdes_mux_eth &=
			~PBS_UNIT_SERDES_MUX_ETH_ALPINE_V2_SELECT_OH_ETH_B_0_MASK;
		serdes_mux_eth |=
			OH0 << PBS_UNIT_SERDES_MUX_ETH_ALPINE_V2_SELECT_OH_ETH_B_0_SHIFT;
	} else if ((init_tmp->cfg.grp_cfg[AL_SRDS_GRP_D].mode == AL_SRDS_CFG_SGMII) ||
		(init_tmp->cfg.grp_cfg[AL_SRDS_GRP_D].mode == AL_SRDS_CFG_KR)) {
		/* 2'b10 - eth_b[0] from serdes_13 */
		serdes_mux_eth &=
			~PBS_UNIT_SERDES_MUX_ETH_ALPINE_V2_SELECT_OH_ETH_B_0_MASK;
		serdes_mux_eth |=
			OH1 << PBS_UNIT_SERDES_MUX_ETH_ALPINE_V2_SELECT_OH_ETH_B_0_SHIFT;
	}

	if ((init_tmp->cfg.grp_cfg[AL_SRDS_GRP_D].mode == AL_SRDS_CFG_SGMII) ||
		(init_tmp->cfg.grp_cfg[AL_SRDS_GRP_D].mode == AL_SRDS_CFG_KR) ||
		(init_tmp->cfg.grp_cfg[AL_SRDS_GRP_D].mode == AL_SRDS_CFG_ETH_C_XLAUI)) {
		/* 2'b10 - eth_c[0] from serdes_12 */
		serdes_mux_eth &=
			~PBS_UNIT_SERDES_MUX_ETH_ALPINE_V2_SELECT_OH_ETH_C_0_MASK;
		serdes_mux_eth |=
			OH1 << PBS_UNIT_SERDES_MUX_ETH_ALPINE_V2_SELECT_OH_ETH_C_0_SHIFT;
	} else if ((init_tmp->cfg.grp_cfg[AL_SRDS_GRP_C].mode == AL_SRDS_CFG_SGMII) ||
		(init_tmp->cfg.grp_cfg[AL_SRDS_GRP_C].mode == AL_SRDS_CFG_KR)) {
		/* 2'b01 - eth_c[0] from serdes_10 */
		serdes_mux_eth &=
			~PBS_UNIT_SERDES_MUX_ETH_ALPINE_V2_SELECT_OH_ETH_C_0_MASK;
		serdes_mux_eth |=
			OH0 << PBS_UNIT_SERDES_MUX_ETH_ALPINE_V2_SELECT_OH_ETH_C_0_SHIFT;
	}

	if ((init_tmp->cfg.grp_cfg[AL_SRDS_GRP_D].mode == AL_SRDS_CFG_SGMII) ||
		(init_tmp->cfg.grp_cfg[AL_SRDS_GRP_D].mode == AL_SRDS_CFG_KR)) {
		/* 2'b10 - eth_d[0] from serdes_15 */
		serdes_mux_eth &=
			~PBS_UNIT_SERDES_MUX_ETH_ALPINE_V2_SELECT_OH_ETH_D_0_MASK;
		serdes_mux_eth |=
			OH1 << PBS_UNIT_SERDES_MUX_ETH_ALPINE_V2_SELECT_OH_ETH_D_0_SHIFT;
	} else if ((init_tmp->cfg.grp_cfg[AL_SRDS_GRP_C].mode == AL_SRDS_CFG_SGMII) ||
		(init_tmp->cfg.grp_cfg[AL_SRDS_GRP_C].mode == AL_SRDS_CFG_KR)) {
		/* 2'b01 - eth_d[0] from serdes_11 */
		serdes_mux_eth &=
			~PBS_UNIT_SERDES_MUX_ETH_ALPINE_V2_SELECT_OH_ETH_D_0_MASK;
		serdes_mux_eth |=
			OH0 << PBS_UNIT_SERDES_MUX_ETH_ALPINE_V2_SELECT_OH_ETH_D_0_SHIFT;
	}

	/* which lane's is master clk */
	serdes_mux_eth &=
		~PBS_UNIT_SERDES_MUX_ETH_ALPINE_V2_SELECT_OH_ETH_A_ICK_MASTER_MASK;
	serdes_mux_eth |=
		(0) <<
		PBS_UNIT_SERDES_MUX_ETH_ALPINE_V2_SELECT_OH_ETH_A_ICK_MASTER_SHIFT;
	/* which lane's is master clk */
	serdes_mux_eth &=
		~PBS_UNIT_SERDES_MUX_ETH_ALPINE_V2_SELECT_OH_ETH_C_ICK_MASTER_MASK;
	serdes_mux_eth |=
		(0) <<
		PBS_UNIT_SERDES_MUX_ETH_ALPINE_V2_SELECT_OH_ETH_C_ICK_MASTER_SHIFT;

	if (init_tmp->cfg.grp_cfg[AL_SRDS_GRP_C].mode != AL_SRDS_CFG_SKIP) {
		/* enable xlaui on eth a */
		serdes_mux_eth &=
			~PBS_UNIT_SERDES_MUX_ETH_ALPINE_V2_SELECT_OH_ETH_A_XLAUI_ENABLE;
		serdes_mux_eth |=
			(init_tmp->cfg.grp_cfg[AL_SRDS_GRP_C].mode == AL_SRDS_CFG_ETH_A_XLAUI) ?
			PBS_UNIT_SERDES_MUX_ETH_ALPINE_V2_SELECT_OH_ETH_A_XLAUI_ENABLE : 0;
	}

	if (init_tmp->cfg.grp_cfg[AL_SRDS_GRP_D].mode != AL_SRDS_CFG_SKIP) {
		/* enable xlaui on eth c */
		serdes_mux_eth &=
			~PBS_UNIT_SERDES_MUX_ETH_ALPINE_V2_SELECT_OH_ETH_C_XLAUI_ENABLE;
		serdes_mux_eth |=
			(init_tmp->cfg.grp_cfg[AL_SRDS_GRP_D].mode == AL_SRDS_CFG_ETH_C_XLAUI) ?
			PBS_UNIT_SERDES_MUX_ETH_ALPINE_V2_SELECT_OH_ETH_C_XLAUI_ENABLE : 0;
	}

	if ((init_tmp->cfg.grp_cfg[AL_SRDS_GRP_B].mode == AL_SRDS_CFG_PCIE_B_G2X4) ||
		(init_tmp->cfg.grp_cfg[AL_SRDS_GRP_B].mode == AL_SRDS_CFG_PCIE_B_G3X4)) {
		/* 2'b10 - select pcie_b[0] from serdes 4 */
		serdes_mux_pcie &=
			~PBS_UNIT_SERDES_MUX_PCIE_ALPINE_V2_SELECT_OH_PCIE_B_0_MASK;
		serdes_mux_pcie |=
			OH1 << PBS_UNIT_SERDES_MUX_PCIE_ALPINE_V2_SELECT_OH_PCIE_B_0_SHIFT;

		/* 2'b10 - select pcie_b[1] from serdes 5 */
		serdes_mux_pcie &=
			~PBS_UNIT_SERDES_MUX_PCIE_ALPINE_V2_SELECT_OH_PCIE_B_1_MASK;
		serdes_mux_pcie |=
			OH1 << PBS_UNIT_SERDES_MUX_PCIE_ALPINE_V2_SELECT_OH_PCIE_B_1_SHIFT;
	} else if (init_tmp->cfg.grp_cfg[AL_SRDS_GRP_A].mode ==
			AL_SRDS_CFG_PCIE_A_G2X2_PCIE_B_G2X2) {
		/* 2'b01 - select pcie_b[0] from serdes 2 */
		serdes_mux_pcie &=
			~PBS_UNIT_SERDES_MUX_PCIE_ALPINE_V2_SELECT_OH_PCIE_B_0_MASK;
		serdes_mux_pcie |=
			OH0 << PBS_UNIT_SERDES_MUX_PCIE_ALPINE_V2_SELECT_OH_PCIE_B_0_SHIFT;

		/* 2'b01 - select pcie_b[1] from serdes 3 */
		serdes_mux_pcie &=
			~PBS_UNIT_SERDES_MUX_PCIE_ALPINE_V2_SELECT_OH_PCIE_B_1_MASK;
		serdes_mux_pcie |=
			OH0 << PBS_UNIT_SERDES_MUX_PCIE_ALPINE_V2_SELECT_OH_PCIE_B_1_SHIFT;
	}

	if ((init_tmp->cfg.grp_cfg[AL_SRDS_GRP_D].mode == AL_SRDS_CFG_PCIE_D_G2X4) ||
		(init_tmp->cfg.grp_cfg[AL_SRDS_GRP_D].mode == AL_SRDS_CFG_PCIE_D_G3X4)) {
		/* 2'b10 - select pcie_d[0] from serdes 10 */
		serdes_mux_pcie &=
			~PBS_UNIT_SERDES_MUX_PCIE_ALPINE_V2_SELECT_OH_PCIE_D_0_MASK;
		serdes_mux_pcie |=
			OH1 << PBS_UNIT_SERDES_MUX_PCIE_ALPINE_V2_SELECT_OH_PCIE_D_0_SHIFT;

		/* 2'b10 - select pcie_d[1] from serdes 12 */
		serdes_mux_pcie &=
			~PBS_UNIT_SERDES_MUX_PCIE_ALPINE_V2_SELECT_OH_PCIE_D_1_MASK;
		serdes_mux_pcie |=
			OH1 << PBS_UNIT_SERDES_MUX_PCIE_ALPINE_V2_SELECT_OH_PCIE_D_1_SHIFT;
	} else if (init_tmp->cfg.grp_cfg[AL_SRDS_GRP_C].mode ==
		AL_SRDS_CFG_PCIE_C_G2X2_PCIE_D_G2X2) {
		/* 2'b01 - select pcie_d[0] from serdes 11 */
		serdes_mux_pcie &=
			~PBS_UNIT_SERDES_MUX_PCIE_ALPINE_V2_SELECT_OH_PCIE_D_0_MASK;
		serdes_mux_pcie |=
			OH0 << PBS_UNIT_SERDES_MUX_PCIE_ALPINE_V2_SELECT_OH_PCIE_D_0_SHIFT;

		/* 2'b01 - select pcie_d[1] from serdes 13 */
		serdes_mux_pcie &=
			~PBS_UNIT_SERDES_MUX_PCIE_ALPINE_V2_SELECT_OH_PCIE_D_1_MASK;
		serdes_mux_pcie |=
			OH0 << PBS_UNIT_SERDES_MUX_PCIE_ALPINE_V2_SELECT_OH_PCIE_D_1_SHIFT;
	}

	if (init_tmp->cfg.grp_cfg[AL_SRDS_GRP_D].mode == AL_SRDS_CFG_SATA_A) {
		/* 2'b10 - select sata_a from serdes group 3 */
		serdes_mux_sata &=
			~PBS_UNIT_SERDES_MUX_SATA_SELECT_OH_SATA_A_MASK;
		serdes_mux_sata |=
			OH1 << PBS_UNIT_SERDES_MUX_SATA_SELECT_OH_SATA_A_SHIFT;
	} else if (init_tmp->cfg.grp_cfg[AL_SRDS_GRP_B].mode == AL_SRDS_CFG_SATA_A) {
		/* 2'b01 - select sata_a from serdes group 1 */
		serdes_mux_sata &=
			~PBS_UNIT_SERDES_MUX_SATA_SELECT_OH_SATA_A_MASK;
		serdes_mux_sata |=
			OH0 << PBS_UNIT_SERDES_MUX_SATA_SELECT_OH_SATA_A_SHIFT;
	}

	al_mod_reg_write32(
		&pbs_regs->unit.serdes_mux_pipe,
		serdes_mux_pipe);
	al_mod_reg_write32(
		&pbs_regs->unit.serdes_mux_multi_0,
		serdes_mux_multi_0);
	al_mod_reg_write32(
		&pbs_regs->unit.serdes_mux_eth,
		serdes_mux_eth);
	al_mod_reg_write32(
		&pbs_regs->unit.serdes_mux_pcie,
		serdes_mux_pcie);
	al_mod_reg_write32(
		&pbs_regs->unit.serdes_mux_sata,
		serdes_mux_sata);

	return status;
}
#endif


/******************************************************************************/
/******************************************************************************/
static int al_mod_serdes_init_cores(
	struct al_mod_serdes_init_tmp	*init_tmp)
{
	struct al_mod_serdes_cfg *cfg = &init_tmp->cfg;
	int status = 0;

	switch (cfg->grp_cfg[AL_SRDS_GRP_A].mode) {
	case AL_SRDS_CFG_PCIE_A_G2X2_PCIE_B_G2X2:
	case AL_SRDS_CFG_PCIE_A_G2X4:
		status = al_mod_serdes_hssp_group_cfg_pcie3_mode(
			init_tmp->obj[AL_SRDS_GRP_A],
			cfg->skip_core_init,
			init_tmp->grp[AL_SRDS_GRP_A].ictl_pma_val,
			1, 0,
			cfg->grp_cfg[AL_SRDS_GRP_A].override_agc_ac_boost,
			cfg->grp_cfg[AL_SRDS_GRP_A].override_agc_ac_boost_val);
		break;

	case AL_SRDS_CFG_PCIE_A_G3X4:
	case AL_SRDS_CFG_PCIE_A_G3X8:
		status = al_mod_serdes_hssp_group_cfg_pcie3_mode(
			init_tmp->obj[AL_SRDS_GRP_A],
			cfg->skip_core_init,
			init_tmp->grp[AL_SRDS_GRP_A].ictl_pma_val,
			0, 0,
			cfg->grp_cfg[AL_SRDS_GRP_A].override_agc_ac_boost,
			cfg->grp_cfg[AL_SRDS_GRP_A].override_agc_ac_boost_val);
		break;
#if (!defined(AL_SERDES_PCIE_ONLY)) || (AL_SERDES_PCIE_ONLY == 0)
	case AL_SRDS_CFG_KR:
		status = al_mod_serdes_hssp_group_cfg_eth_kr_mode(
			init_tmp->obj[AL_SRDS_GRP_A],
			cfg->skip_core_init,
			init_tmp->grp[AL_SRDS_GRP_A].ictl_pma_val,
			init_tmp->grp[AL_SRDS_GRP_A].core_clk_freq,
			AL_FALSE);
		break;
#endif
	case AL_SRDS_CFG_OFF:
		status = al_mod_serdes_hssp_group_cfg_off(
			init_tmp->obj[AL_SRDS_GRP_A]);
		break;

	case AL_SRDS_CFG_OFF_BP:
		status = al_mod_serdes_hssp_group_cfg_off_bp(
			init_tmp->obj[AL_SRDS_GRP_A]);
		break;

	case AL_SRDS_CFG_SKIP:
		break;

	default:
		al_mod_err(
			"%s: invalid group A configuration: %d\n",
			__func__,
			cfg->grp_cfg[AL_SRDS_GRP_A].mode);
		status = -EINVAL;
		goto done;
	}

	if ((cfg->grp_cfg[AL_SRDS_GRP_A].mode != AL_SRDS_CFG_OFF) &&
		(cfg->grp_cfg[AL_SRDS_GRP_A].mode != AL_SRDS_CFG_OFF_BP) &&
		(cfg->grp_cfg[AL_SRDS_GRP_A].mode != AL_SRDS_CFG_SKIP)) {
		status = al_mod_serdes_hssp_group_non_active_lanes_pd(
			init_tmp->obj[AL_SRDS_GRP_A],
			cfg->grp_cfg[AL_SRDS_GRP_A].active_lanes);
		al_mod_serdes_hssp_group_lanes_tx_rx_pol_inv(
			init_tmp->obj[AL_SRDS_GRP_A],
			cfg->grp_cfg[AL_SRDS_GRP_A].inv_tx_lanes,
			cfg->grp_cfg[AL_SRDS_GRP_A].inv_rx_lanes);
	}

	if (status) {
		al_mod_err("%s: group A configuration failed\n", __func__);
		goto done;
	}

	switch (cfg->grp_cfg[AL_SRDS_GRP_B].mode) {
#if (!defined(AL_SERDES_PCIE_ONLY)) || (AL_SERDES_PCIE_ONLY == 0)
	case AL_SRDS_CFG_SATA_A:
		status = al_mod_serdes_hssp_group_cfg_sata_mode(
			init_tmp->obj[AL_SRDS_GRP_B],
			cfg->skip_core_init,
			init_tmp->grp[AL_SRDS_GRP_B].ictl_pma_val,
			init_tmp->grp[AL_SRDS_GRP_B].core_clk_freq,
			cfg->grp_cfg[AL_SRDS_GRP_B].ssc_en);
		break;
#endif

	case AL_SRDS_CFG_PCIE_B_G2X4:
		status = al_mod_serdes_hssp_group_cfg_pcie3_mode(
			init_tmp->obj[AL_SRDS_GRP_B],
			cfg->skip_core_init,
			init_tmp->grp[AL_SRDS_GRP_B].ictl_pma_val,
			1, 0,
			cfg->grp_cfg[AL_SRDS_GRP_B].override_agc_ac_boost,
			cfg->grp_cfg[AL_SRDS_GRP_B].override_agc_ac_boost_val);
		break;

	case AL_SRDS_CFG_PCIE_B_G3X4:
		status = al_mod_serdes_hssp_group_cfg_pcie3_mode(
			init_tmp->obj[AL_SRDS_GRP_B],
			cfg->skip_core_init,
			init_tmp->grp[AL_SRDS_GRP_B].ictl_pma_val,
			0, 0,
			cfg->grp_cfg[AL_SRDS_GRP_B].override_agc_ac_boost,
			cfg->grp_cfg[AL_SRDS_GRP_B].override_agc_ac_boost_val);
		break;

	case AL_SRDS_CFG_PCIE_A_G3X8:
		status = al_mod_serdes_hssp_group_cfg_pcie3_mode(
			init_tmp->obj[AL_SRDS_GRP_B],
			cfg->skip_core_init,
			init_tmp->grp[AL_SRDS_GRP_B].ictl_pma_val,
			0, 1,
			cfg->grp_cfg[AL_SRDS_GRP_B].override_agc_ac_boost,
			cfg->grp_cfg[AL_SRDS_GRP_B].override_agc_ac_boost_val);
		break;
#if (!defined(AL_SERDES_PCIE_ONLY)) || (AL_SERDES_PCIE_ONLY == 0)
	case AL_SRDS_CFG_KR:
		status = al_mod_serdes_hssp_group_cfg_eth_kr_mode(
			init_tmp->obj[AL_SRDS_GRP_B],
			cfg->skip_core_init,
			init_tmp->grp[AL_SRDS_GRP_B].ictl_pma_val,
			init_tmp->grp[AL_SRDS_GRP_B].core_clk_freq,
			AL_FALSE);
		break;
#endif
	case AL_SRDS_CFG_OFF:
		status = al_mod_serdes_hssp_group_cfg_off(
			init_tmp->obj[AL_SRDS_GRP_B]);
		break;

	case AL_SRDS_CFG_OFF_BP:
		status = al_mod_serdes_hssp_group_cfg_off_bp(
			init_tmp->obj[AL_SRDS_GRP_B]);
		break;

	case AL_SRDS_CFG_SKIP:
		break;

	default:
		al_mod_err(
			"%s: invalid group B configuration: %d\n",
			__func__,
			cfg->grp_cfg[AL_SRDS_GRP_B].mode);
		status = -EINVAL;
		goto done;
	}

	if ((cfg->grp_cfg[AL_SRDS_GRP_B].mode != AL_SRDS_CFG_OFF) &&
		(cfg->grp_cfg[AL_SRDS_GRP_B].mode != AL_SRDS_CFG_OFF_BP) &&
		(cfg->grp_cfg[AL_SRDS_GRP_B].mode != AL_SRDS_CFG_SKIP)) {
		status = al_mod_serdes_hssp_group_non_active_lanes_pd(
			init_tmp->obj[AL_SRDS_GRP_B],
			cfg->grp_cfg[AL_SRDS_GRP_B].active_lanes);
		al_mod_serdes_hssp_group_lanes_tx_rx_pol_inv(
			init_tmp->obj[AL_SRDS_GRP_B],
			cfg->grp_cfg[AL_SRDS_GRP_B].inv_tx_lanes,
			cfg->grp_cfg[AL_SRDS_GRP_B].inv_rx_lanes);
	}

	if (status) {
		al_mod_err("%s: group B configuration failed\n", __func__);
		goto done;
	}

	switch (cfg->grp_cfg[AL_SRDS_GRP_C].mode) {
#if (!defined(AL_SERDES_PCIE_ONLY)) || (AL_SERDES_PCIE_ONLY == 0)
	case AL_SRDS_CFG_SATA_B:
		status = al_mod_serdes_hssp_group_cfg_sata_mode(
			init_tmp->obj[AL_SRDS_GRP_C],
			cfg->skip_core_init,
			init_tmp->grp[AL_SRDS_GRP_C].ictl_pma_val,
			init_tmp->grp[AL_SRDS_GRP_C].core_clk_freq,
			cfg->grp_cfg[AL_SRDS_GRP_C].ssc_en);
		break;

	case AL_SRDS_CFG_SGMII:
		status = al_mod_serdes_hssp_group_cfg_eth_sgmii_mode(
			init_tmp->obj[AL_SRDS_GRP_C],
			cfg->skip_core_init,
			init_tmp->grp[AL_SRDS_GRP_C].core_clk_freq,
			init_tmp->grp[AL_SRDS_GRP_C].ictl_pma_val);
		break;

	case AL_SRDS_CFG_SGMII_2_5G:
		status = al_mod_serdes_hssp_group_cfg_eth_sgmii_2_5g_mode(
			init_tmp->obj[AL_SRDS_GRP_C],
			cfg->skip_core_init,
			init_tmp->grp[AL_SRDS_GRP_C].core_clk_freq,
			init_tmp->grp[AL_SRDS_GRP_C].ictl_pma_val);
		break;

	case AL_SRDS_CFG_KR:
		status = al_mod_serdes_hssp_group_cfg_eth_kr_mode(
			init_tmp->obj[AL_SRDS_GRP_C],
			cfg->skip_core_init,
			init_tmp->grp[AL_SRDS_GRP_C].ictl_pma_val,
			init_tmp->grp[AL_SRDS_GRP_C].core_clk_freq,
			AL_FALSE);
		break;

	case AL_SRDS_CFG_ETH_A_XLAUI:
		status = al_mod_serdes_hssp_group_cfg_eth_kr_mode(
			init_tmp->obj[AL_SRDS_GRP_C],
			cfg->skip_core_init,
			init_tmp->grp[AL_SRDS_GRP_C].ictl_pma_val,
			init_tmp->grp[AL_SRDS_GRP_C].core_clk_freq,
			AL_TRUE);
		break;
#endif

	case AL_SRDS_CFG_PCIE_C_G2X4:
	case AL_SRDS_CFG_PCIE_C_G2X2_PCIE_D_G2X2:
		status = al_mod_serdes_hssp_group_cfg_pcie3_mode(
			init_tmp->obj[AL_SRDS_GRP_C],
			cfg->skip_core_init,
			init_tmp->grp[AL_SRDS_GRP_C].ictl_pma_val,
			1, 0,
			cfg->grp_cfg[AL_SRDS_GRP_C].override_agc_ac_boost,
			cfg->grp_cfg[AL_SRDS_GRP_C].override_agc_ac_boost_val);
		break;

	case AL_SRDS_CFG_PCIE_C_G3X4:
		status = al_mod_serdes_hssp_group_cfg_pcie3_mode(
			init_tmp->obj[AL_SRDS_GRP_C],
			cfg->skip_core_init,
			init_tmp->grp[AL_SRDS_GRP_C].ictl_pma_val,
			0, 0,
			cfg->grp_cfg[AL_SRDS_GRP_C].override_agc_ac_boost,
			cfg->grp_cfg[AL_SRDS_GRP_C].override_agc_ac_boost_val);
		break;

	case AL_SRDS_CFG_OFF:
		status = al_mod_serdes_hssp_group_cfg_off(
			init_tmp->obj[AL_SRDS_GRP_C]);
		break;

	case AL_SRDS_CFG_OFF_BP:
		status = al_mod_serdes_hssp_group_cfg_off_bp(
			init_tmp->obj[AL_SRDS_GRP_C]);
		break;

	case AL_SRDS_CFG_SKIP:
		break;

	default:
		al_mod_err(
			"%s: invalid group C configuration: %d\n",
			__func__,
			cfg->grp_cfg[AL_SRDS_GRP_C].mode);
		status = -EINVAL;
		goto done;
	}

	if ((cfg->grp_cfg[AL_SRDS_GRP_C].mode != AL_SRDS_CFG_OFF) &&
		(cfg->grp_cfg[AL_SRDS_GRP_C].mode != AL_SRDS_CFG_OFF_BP) &&
		(cfg->grp_cfg[AL_SRDS_GRP_C].mode != AL_SRDS_CFG_SKIP)) {
		status = al_mod_serdes_hssp_group_non_active_lanes_pd(
			init_tmp->obj[AL_SRDS_GRP_C],
			cfg->grp_cfg[AL_SRDS_GRP_C].active_lanes);
		al_mod_serdes_hssp_group_lanes_tx_rx_pol_inv(
			init_tmp->obj[AL_SRDS_GRP_C],
			cfg->grp_cfg[AL_SRDS_GRP_C].inv_tx_lanes,
			cfg->grp_cfg[AL_SRDS_GRP_C].inv_rx_lanes);
	}

	if (status) {
		al_mod_err("%s: group C configuration failed\n", __func__);
		goto done;
	}

	switch (cfg->grp_cfg[AL_SRDS_GRP_D].mode) {
#if (!defined(AL_SERDES_PCIE_ONLY)) || (AL_SERDES_PCIE_ONLY == 0)
	case AL_SRDS_CFG_SATA_A:
		status = al_mod_serdes_hssp_group_cfg_sata_mode(
			init_tmp->obj[AL_SRDS_GRP_D],
			cfg->skip_core_init,
			init_tmp->grp[AL_SRDS_GRP_D].ictl_pma_val,
			init_tmp->grp[AL_SRDS_GRP_D].core_clk_freq,
			cfg->grp_cfg[AL_SRDS_GRP_D].ssc_en);
		break;

	case AL_SRDS_CFG_SGMII:
		status = al_mod_serdes_hssp_group_cfg_eth_sgmii_mode(
			init_tmp->obj[AL_SRDS_GRP_D],
			cfg->skip_core_init,
			init_tmp->grp[AL_SRDS_GRP_D].core_clk_freq,
			init_tmp->grp[AL_SRDS_GRP_D].ictl_pma_val);
		break;

	case AL_SRDS_CFG_SGMII_2_5G:
		status = al_mod_serdes_hssp_group_cfg_eth_sgmii_2_5g_mode(
			init_tmp->obj[AL_SRDS_GRP_D],
			cfg->skip_core_init,
			init_tmp->grp[AL_SRDS_GRP_D].core_clk_freq,
			init_tmp->grp[AL_SRDS_GRP_D].ictl_pma_val);
		break;

	case AL_SRDS_CFG_KR:
		status = al_mod_serdes_hssp_group_cfg_eth_kr_mode(
			init_tmp->obj[AL_SRDS_GRP_D],
			cfg->skip_core_init,
			init_tmp->grp[AL_SRDS_GRP_D].ictl_pma_val,
			init_tmp->grp[AL_SRDS_GRP_D].core_clk_freq,
			AL_FALSE);
		break;

	case AL_SRDS_CFG_ETH_C_XLAUI:
		status = al_mod_serdes_hssp_group_cfg_eth_kr_mode(
			init_tmp->obj[AL_SRDS_GRP_D],
			cfg->skip_core_init,
			init_tmp->grp[AL_SRDS_GRP_D].ictl_pma_val,
			init_tmp->grp[AL_SRDS_GRP_D].core_clk_freq,
			AL_TRUE);
		break;
#endif

	case AL_SRDS_CFG_PCIE_D_G2X4:
		status = al_mod_serdes_hssp_group_cfg_pcie3_mode(
			init_tmp->obj[AL_SRDS_GRP_D],
			cfg->skip_core_init,
			init_tmp->grp[AL_SRDS_GRP_D].ictl_pma_val,
			1, 0,
			cfg->grp_cfg[AL_SRDS_GRP_D].override_agc_ac_boost,
			cfg->grp_cfg[AL_SRDS_GRP_D].override_agc_ac_boost_val);
		break;

	case AL_SRDS_CFG_PCIE_D_G3X4:
		status = al_mod_serdes_hssp_group_cfg_pcie3_mode(
			init_tmp->obj[AL_SRDS_GRP_D],
			cfg->skip_core_init,
			init_tmp->grp[AL_SRDS_GRP_D].ictl_pma_val,
			0, 0,
			cfg->grp_cfg[AL_SRDS_GRP_D].override_agc_ac_boost,
			cfg->grp_cfg[AL_SRDS_GRP_D].override_agc_ac_boost_val);
		break;

	case AL_SRDS_CFG_OFF:
		status = al_mod_serdes_hssp_group_cfg_off(
			init_tmp->obj[AL_SRDS_GRP_D]);
		break;

	case AL_SRDS_CFG_OFF_BP:
		status = al_mod_serdes_hssp_group_cfg_off_bp(
			init_tmp->obj[AL_SRDS_GRP_D]);
		break;

	case AL_SRDS_CFG_SKIP:
		break;

	default:
		al_mod_err(
			"%s: invalid group D configuration: %d\n",
			__func__,
			cfg->grp_cfg[AL_SRDS_GRP_D].mode);
		status = -EINVAL;
	}

	if ((cfg->grp_cfg[AL_SRDS_GRP_D].mode != AL_SRDS_CFG_OFF) &&
		(cfg->grp_cfg[AL_SRDS_GRP_D].mode != AL_SRDS_CFG_OFF_BP) &&
		(cfg->grp_cfg[AL_SRDS_GRP_D].mode != AL_SRDS_CFG_SKIP)) {
		status = al_mod_serdes_hssp_group_non_active_lanes_pd(
			init_tmp->obj[AL_SRDS_GRP_D],
			cfg->grp_cfg[AL_SRDS_GRP_D].active_lanes);
		al_mod_serdes_hssp_group_lanes_tx_rx_pol_inv(
			init_tmp->obj[AL_SRDS_GRP_D],
			cfg->grp_cfg[AL_SRDS_GRP_D].inv_tx_lanes,
			cfg->grp_cfg[AL_SRDS_GRP_D].inv_rx_lanes);
	}

	if (status) {
		al_mod_err("%s: group D configuration failed\n", __func__);
		goto done;
	}

#if SRD_GRP_E_EN
	if (cfg->grp_cfg[AL_SRDS_GRP_E].mode != AL_SRDS_CFG_SKIP) {
		status = al_mod_serdes_25g_fw_init(
			init_tmp->obj[AL_SRDS_GRP_E], cfg->serdes_25g_fw, cfg->serdes_25g_fw_size);

		if (!cfg->grp_cfg[AL_SRDS_GRP_E].mode_set_adv_params) {
			switch (cfg->grp_cfg[AL_SRDS_GRP_E].mode) {
			case AL_SRDS_CFG_ETH_25G:
				status = al_mod_serdes_25g_group_cfg_25g_mode(
					init_tmp->obj[AL_SRDS_GRP_E]);
				break;
			case AL_SRDS_CFG_KR:
				status = al_mod_serdes_25g_group_cfg_10g_mode(
					init_tmp->obj[AL_SRDS_GRP_E]);
				break;

			default:
				al_mod_err(
					"%s: invalid group E configuration: %d\n",
					__func__,
					cfg->grp_cfg[AL_SRDS_GRP_E].mode);
				status = -EINVAL;
			}
		} else  {
			al_mod_assert(cfg->grp_cfg[AL_SRDS_GRP_E].mode_set_adv_params->group_mode ==
				cfg->grp_cfg[AL_SRDS_GRP_E].mode);

			status = init_tmp->obj[AL_SRDS_GRP_E]->mode_set_adv(
				init_tmp->obj[AL_SRDS_GRP_E],
				cfg->grp_cfg[AL_SRDS_GRP_E].mode_set_adv_params);
		}
	}

	if ((cfg->grp_cfg[AL_SRDS_GRP_E].mode != AL_SRDS_CFG_OFF) &&
		(cfg->grp_cfg[AL_SRDS_GRP_E].mode != AL_SRDS_CFG_OFF_BP) &&
		(cfg->grp_cfg[AL_SRDS_GRP_E].mode != AL_SRDS_CFG_SKIP)) {
		/* TODO [Guy]: power down unused lanes */
		al_mod_serdes_25g_group_lanes_tx_rx_pol_inv(
			init_tmp->obj[AL_SRDS_GRP_E],
			cfg->grp_cfg[AL_SRDS_GRP_E].inv_tx_lanes,
			cfg->grp_cfg[AL_SRDS_GRP_E].inv_rx_lanes);
	}

	if (status) {
		al_mod_err("%s: group E configuration failed\n", __func__);
		goto done;
	}

#endif
done:
	return status;
}


/******************************************************************************/
/******************************************************************************/
static int al_mod_serdes_group_r2l_clk_freq_update(
	struct al_mod_serdes_group_cfg	*grp_cfg,
	enum al_mod_serdes_clk_freq		r2l_in_freq,
	enum al_mod_serdes_clk_freq		*r2l_out_freq)
{
	int status = 0;

	grp_cfg->r2l_clk_freq = r2l_in_freq;

	switch (grp_cfg->clk_src_r2l) {
	case AL_SRDS_CLK_SRC_LOGIC_0:
		*r2l_out_freq = AL_SRDS_CLK_FREQ_NA;
		break;

	case AL_SRDS_CLK_SRC_R2L:
	case AL_SRDS_CLK_SRC_R2L_PLL:
		*r2l_out_freq = r2l_in_freq;
		break;

	case AL_SRDS_CLK_SRC_REF_PINS:
		*r2l_out_freq = grp_cfg->ref_clk_freq;
		break;

	default:
		al_mod_err(
			"%s: clk_src_r2l invalid (%d)!\n",
			__func__,
			grp_cfg->clk_src_r2l);
		status = -EINVAL;
	}

	return status;
}

/******************************************************************************/
/******************************************************************************/
static int al_mod_serdes_group_l2r_clk_freq_update(
	struct al_mod_serdes_group_cfg	*grp_cfg,
	enum al_mod_serdes_clk_freq		l2r_in_freq,
	enum al_mod_serdes_clk_freq		*l2r_out_freq)
{
	int status = 0;

	grp_cfg->l2r_clk_freq = l2r_in_freq;

	switch (grp_cfg->clk_src_l2r) {
	case AL_SRDS_CLK_SRC_LOGIC_0:
		*l2r_out_freq = AL_SRDS_CLK_FREQ_NA;
		break;

	case AL_SRDS_CLK_SRC_L2R:
		*l2r_out_freq = l2r_in_freq;
		break;

	case AL_SRDS_CLK_SRC_REF_PINS:
		*l2r_out_freq = grp_cfg->ref_clk_freq;
		break;

	default:
		al_mod_err(
			"%s: clk_src_l2r invalid (%d)!\n",
			__func__,
			grp_cfg->clk_src_l2r);
		status = -EINVAL;
	}

	return status;
}

/******************************************************************************/
/******************************************************************************/
static int al_mod_serdes_group_core_clk_freq_update(
	struct al_mod_serdes_group_cfg	*grp_cfg,
	struct al_mod_serdes_init_grp_tmp	*init_grp_tmp)
{
	int status = 0;

	switch (grp_cfg->clk_src_core) {
	case AL_SRDS_CLK_SRC_R2L:
	case AL_SRDS_CLK_SRC_R2L_PLL:
		init_grp_tmp->core_clk_freq = grp_cfg->r2l_clk_freq;
		break;

	case AL_SRDS_CLK_SRC_L2R:
		init_grp_tmp->core_clk_freq = grp_cfg->l2r_clk_freq;
		break;

	case AL_SRDS_CLK_SRC_REF_PINS:
		init_grp_tmp->core_clk_freq = grp_cfg->ref_clk_freq;
		break;

	default:
		al_mod_err(
			"%s: clk_src_core invalid (%d)!\n",
			__func__,
			grp_cfg->clk_src_core);
		status = -EINVAL;
	}

	return status;
}

/******************************************************************************/
/******************************************************************************/
static int al_mod_serdes_init_clk_route(
	struct al_mod_serdes_init_tmp	*init_tmp)
{
	struct al_mod_serdes_cfg *cfg = &init_tmp->cfg;
	int status = 0;
	int i;

	enum al_mod_serdes_clk_freq cur_freq_r2l =
		cfg->grp_cfg[AL_SRDS_GRP_A].r2l_clk_freq;

	enum al_mod_serdes_clk_freq cur_freq_l2r =
		cfg->grp_cfg[AL_SRDS_GRP_D].l2r_clk_freq;

	/* TODO: Set PCIe ref clock out - wrapper reg at 0x40 per group */
	/*sv_reg_write32(SERDES_WRAP_REG_BASE + i*'h400 + 'h100 + 'h40 ,'h20);*/

	for (i = 0; i < AL_SRDS_NUM_HSSP_GROUPS; i++) {
		if (cfg->grp_cfg[i].mode == AL_SRDS_CFG_SKIP)
			continue;

		al_mod_serdes_hssp_basic_power_up(
			init_tmp->obj[i],
			(i == AL_SRDS_GRP_C) &&
			(cfg->grp_cfg[i].clk_src_r2l == AL_SRDS_CLK_SRC_R2L_PLL));
	}

	for (i = 0; i < SRD_NUM_GROUPS; i++) {
		status =  al_mod_serdes_group_r2l_clk_freq_update(
			&init_tmp->cfg.grp_cfg[i],
			cur_freq_r2l,
			&cur_freq_r2l);
		if (status) {
			al_mod_err(
				"%s: al_mod_serdes_group_r2l_clk_freq_update(%d) "
				"failed!\n",
				__func__,
				i);
			goto done;
		}
	}

	for (i = SRD_NUM_GROUPS - 1; i >= 0; i--) {
		status =  al_mod_serdes_group_l2r_clk_freq_update(
			&init_tmp->cfg.grp_cfg[i],
			cur_freq_l2r,
			&cur_freq_l2r);
		if (status) {
			al_mod_err(
				"%s: al_mod_serdes_group_l2r_clk_freq_update(%d) "
				"failed!\n",
				__func__,
				i);
			goto done;
		}
	}

	for (i = 0; i < SRD_NUM_GROUPS; i++) {
		status =  al_mod_serdes_group_core_clk_freq_update(
			&init_tmp->cfg.grp_cfg[i],
			&init_tmp->grp[i]);
		if (status) {
			al_mod_err(
				"%s: al_mod_serdes_group_core_clk_freq_update(%d) "
				"failed!\n",
				__func__,
				i);
			goto done;
		}
	}

	for (i = AL_SRDS_GRP_A; i <= AL_SRDS_GRP_B; i++) {
		if (cfg->grp_cfg[i].mode == AL_SRDS_CFG_SKIP)
			continue;
		status = al_mod_serdes_hssp_group_ictl_pma_val_set(
			init_tmp->obj[i],
			&init_tmp->grp[i].ictl_pma_val,
			init_tmp->cfg.grp_cfg[i].clk_src_r2l,
			init_tmp->cfg.grp_cfg[i].clk_src_l2r,
			init_tmp->cfg.grp_cfg[i].clk_src_core);
		if (status) {
			al_mod_err(
				"%s: al_mod_serdes_hssp_group_ictl_pma_val_set(%d) "
				"failed!\n",
				__func__,
				i);
			goto done;
		}
	}

	/* Groups C and D are flipped with regards to clock directions */
	for (i = AL_SRDS_GRP_C; i <= AL_SRDS_GRP_D; i++) {
		if (cfg->grp_cfg[i].mode == AL_SRDS_CFG_SKIP)
			continue;
		status = al_mod_serdes_hssp_group_ictl_pma_val_set_flp(
			init_tmp->obj[i],
			&init_tmp->grp[i].ictl_pma_val,
			init_tmp->cfg.grp_cfg[i].clk_src_r2l,
			init_tmp->cfg.grp_cfg[i].clk_src_l2r,
			init_tmp->cfg.grp_cfg[i].clk_src_core);
		if (status) {
			al_mod_err(
				"%s: al_mod_serdes_hssp_group_ictl_pma_val_set_flp(%d) "
				"failed!\n",
				__func__,
				i);
			goto done;
		}
	}

#if SRD_GRP_E_EN
	status = al_mod_serdes_25g_group_ictl_pma_val_set(
			init_tmp->obj[AL_SRDS_GRP_E],
			&init_tmp->grp[AL_SRDS_GRP_E].ictl_pma_val,
			init_tmp->cfg.grp_cfg[AL_SRDS_GRP_E].clk_src_r2l,
			init_tmp->cfg.grp_cfg[AL_SRDS_GRP_E].clk_src_l2r,
			init_tmp->cfg.grp_cfg[AL_SRDS_GRP_E].clk_src_core);
	if (status) {
		al_mod_err(
			"%s: al_mod_serdes_25g_group_ictl_pma_val_set(%d) "
			"failed!\n",
			__func__,
			AL_SRDS_GRP_E);
		goto done;
	}
#endif

done:
	return status;
}
