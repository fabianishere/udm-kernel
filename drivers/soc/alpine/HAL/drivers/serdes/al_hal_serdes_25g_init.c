/*******************************************************************************
Copyright (C) 2015 Annapurna Labs Ltd.

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

#include "al_hal_serdes_25g_fw.h"
#include "al_hal_serdes_25g_regs.h"
#include "al_hal_serdes_25g_internal_regs.h"
#include "al_hal_serdes_25g_init.h"

/******************************************************************************/
/******************************************************************************/
int al_serdes_25g_fw_init(
	struct al_serdes_grp_obj	*obj,
	const char			*fw,
	unsigned int			fw_size)
{
	struct al_serdes_c_regs __iomem *regs_base =
		(struct al_serdes_c_regs __iomem *)obj->regs_base;
	uint32_t i;
	uint32_t reg;

	/* start writing from address 0 */
	/* the address is automatically incremented after every write */
	al_reg_write32(&regs_base->gen.cpu_prog_addr, 0);

	if (!fw) {
		fw = al_serdes_25g_fw;
		fw_size = sizeof(al_serdes_25g_fw);
	}

	for (i = 0; i < fw_size; i += sizeof(uint32_t)) {
		reg = ((fw[i] << 24) | (fw[i + 1] << 16) | (fw[i + 2] << 8) | (fw[i + 3]));
		al_reg_write32(&regs_base->gen.cpu_prog_data, reg);
	}

	return 0;
}

/******************************************************************************/
/******************************************************************************/
int al_serdes_25g_group_ictl_pma_val_set(
	struct al_serdes_grp_obj	*grp_obj,
	uint32_t			*ictl_pma_val,
	enum al_serdes_clk_src		clk_src_r2l,
	enum al_serdes_clk_src		clk_src_l2r,
	enum al_serdes_clk_src		clk_src_core)
{
	struct al_serdes_c_regs __iomem *regs_base =
		(struct al_serdes_c_regs __iomem *)grp_obj->regs_base;
	int status = 0;

	*ictl_pma_val = 0;

	switch (clk_src_core) {
	case AL_SRDS_CLK_SRC_REF_PINS:
		*ictl_pma_val |=
			SERDES_C_GEN_CTRL_REFCLK_INPUT_SEL_REF;
		break;

	case AL_SRDS_CLK_SRC_L2R:
		*ictl_pma_val |=
			SERDES_C_GEN_CTRL_REFCLK_INPUT_SEL_L2R;
		break;

	case AL_SRDS_CLK_SRC_R2L:
		*ictl_pma_val |=
			SERDES_C_GEN_CTRL_REFCLK_INPUT_SEL_R2L;
		break;

	default:
		al_err(
			"%s: invalid core clock source: %d\n",
			__func__,
			clk_src_core);
		status = -EINVAL;
		goto done;
	}

	switch (clk_src_r2l) {
	case AL_SRDS_CLK_SRC_LOGIC_0:
		*ictl_pma_val |=
		SERDES_C_GEN_CTRL_REFCLK_LEFT_SEL_0;
		break;

	case AL_SRDS_CLK_SRC_REF_PINS:
		*ictl_pma_val |=
		SERDES_C_GEN_CTRL_REFCLK_LEFT_SEL_REF;
		break;

	case AL_SRDS_CLK_SRC_R2L:
		*ictl_pma_val |=
		SERDES_C_GEN_CTRL_REFCLK_LEFT_SEL_R2L;
		break;

	default:
		al_err(
			"%s: invalid R2L clock source: %d\n",
			__func__,
			clk_src_r2l);
		status = -EINVAL;
		goto done;
	}

	switch (clk_src_l2r) {
	case AL_SRDS_CLK_SRC_LOGIC_0:
		*ictl_pma_val |=
		SERDES_C_GEN_CTRL_REFCLK_RIGHT_SEL_0;
		break;

	case AL_SRDS_CLK_SRC_REF_PINS:
		*ictl_pma_val |=
		SERDES_C_GEN_CTRL_REFCLK_RIGHT_SEL_REF;
		break;

	case AL_SRDS_CLK_SRC_L2R:
		*ictl_pma_val |=
		SERDES_C_GEN_CTRL_REFCLK_RIGHT_SEL_L2R;
		break;

	default:
		al_err(
			"%s: invalid L2R clock source: %d\n",
			__func__,
			clk_src_l2r);
		status = -EINVAL;
		goto done;
	}

	al_reg_write32(
		&regs_base->gen.ctrl,
		*ictl_pma_val);

done:
	return status;
}

void al_serdes_25g_group_lanes_tx_rx_pol_inv(
	struct al_serdes_grp_obj	*grp_obj,
	al_bool				*inv_tx_lanes,
	al_bool				*inv_rx_lanes)
{
	struct al_serdes_c_regs __iomem *regs_base =
		(struct al_serdes_c_regs __iomem *)grp_obj->regs_base;
	int i;

	for (i = 0; i < AL_SRDS_NUM_LANES; i++) {
		if (inv_tx_lanes[i] == AL_TRUE)
			al_reg_write32_masked(&regs_base->lane[i].cfg,
					SERDES_C_LANE_CFG_TX_LANE_POLARITY,
					SERDES_C_LANE_CFG_TX_LANE_POLARITY);
		if (inv_rx_lanes[i] == AL_TRUE)
			al_reg_write32_masked(&regs_base->lane[i].cfg,
					SERDES_C_LANE_CFG_LN_CTRL_RXPOLARITY,
					SERDES_C_LANE_CFG_LN_CTRL_RXPOLARITY);
	}
}

/******************************************************************************/
/******************************************************************************/
int al_serdes_25g_group_cfg_25g_mode(struct al_serdes_grp_obj	*obj)
{
	return obj->mode_set_25g(obj);
}

int al_serdes_25g_group_cfg_10g_mode(struct al_serdes_grp_obj	*obj)
{
	return obj->mode_set_kr(obj);
}

