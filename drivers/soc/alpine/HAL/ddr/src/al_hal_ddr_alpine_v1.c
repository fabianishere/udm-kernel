/*******************************************************************************
Copyright (C) 2015 Annapurna Labs Ltd.

This file may be licensed under the terms of the Annapurna Labs Commercial
License Agreement.

Alternatively, this file can be distributed under the terms of the GNU General
Public License V2 as published by the Free Software Foundation and can be
found at http://www.gnu.org/licenses/gpl-2.0.html

Alternatively, redistribution and use in source and binary forms, with or
without modification, are permitted provided that the following conditions are
met:

    *     Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

    *     Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in
the documentation and/or other materials provided with the
distribution.

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

/**
 * @addtogroup groupddr
 *
 *  @{
 * @file   al_hal_ddr.c
 *
 * @brief  DDR controller & PHY HAL driver
 *
 */
#include "al_hal_ddr.h"
#include "al_hal_ddr_ctrl_regs_common.h"
#include "al_hal_ddr_ctrl_regs_alpine_v1.h"
#include "al_hal_ddr_phy_regs_alpine_v1.h"
#include "al_hal_nb_regs_v1_v2.h"
#include "al_hal_ddr_utils.h"

#if (!defined(AL_DEV_ID)) || (AL_DEV_ID == AL_DEV_ID_ALPINE_V1)

#define BIST_TIMEOUT	1000	/* 1ms */

#define BIST_LFSR_RESET_VAL	0x1234ABCD

#if (defined(AL_DEV_ID))
#define DDR_STATIC
#else
#define DDR_STATIC	static
#endif

#define STRUCT_PHY_REGS			\
	struct al_dwc_ddrphy_top_dwc_ddrphy_pub_alpine_v1
#define PHY_REGS(ddr_phy_regs_base)	\
	(&((struct al_dwc_ddrphy_top_regs_alpine_v1 __iomem *)ddr_phy_regs_base)->dwc_ddrphy_pub)

/* Wait for PHY BIST to be done */
static int al_ddr_phy_wait_for_bist_done(
	struct al_ddr_cfg		*ddr_cfg,
	struct al_ddr_bist_err_status	*bist_err_status);

static void read_bist_err_status(
	struct al_ddr_cfg		*ddr_cfg,
	struct al_ddr_bist_err_status	*bist_err_status);

/*******************************************************************************
 ******************************************************************************/
DDR_STATIC void _al_ddr_phy_datx_bist_pre_adv(
	struct al_ddr_cfg	*ddr_cfg,
	al_bool			vt_calc_disable)
{
	STRUCT_PHY_REGS __iomem *phy_regs = PHY_REGS(ddr_cfg->ddr_phy_regs_base);

	if (vt_calc_disable)
		al_ddr_phy_vt_calc_disable(ddr_cfg);
	al_ddr_ctrl_stop(ddr_cfg);

	/* Disabling refreshes from PHY as it cause BIST to get stuck */
	/* Stop PHY initiated update request */
	/* Make sure PHY initiated updates were on when running BIST */
	al_assert(al_reg_read32(&phy_regs->DSGCR) &
			DWC_DDR_PHY_REGS_DSGCR_PUREN);
	al_reg_write32_masked(
		&phy_regs->DTCR,
		DWC_DDR_PHY_REGS_DTCR_RFSHDT_MASK,
		(0x0 << DWC_DDR_PHY_REGS_DTCR_RFSHDT_SHIFT));
	al_reg_write32_masked(
		&phy_regs->DSGCR,
		DWC_DDR_PHY_REGS_DSGCR_PUREN,
		0);
}
/*******************************************************************************
 ******************************************************************************/
DDR_STATIC void _al_ddr_phy_datx_bist_pre(
	struct al_ddr_cfg	*ddr_cfg)
{
	al_ddr_phy_datx_bist_pre_adv(ddr_cfg, AL_TRUE);
}
/*******************************************************************************
 ******************************************************************************/
DDR_STATIC void _al_ddr_phy_datx_bist_post_adv(
	struct al_ddr_cfg	*ddr_cfg,
	al_bool			vt_calc_enable)
{
	STRUCT_PHY_REGS __iomem *phy_regs = PHY_REGS(ddr_cfg->ddr_phy_regs_base);

	/* PGCR3 - after BIST re-apply power down of unused DQs */
	al_reg_write32_masked(
		&phy_regs->PGCR[3],
		DWC_DDR_PHY_REGS_PGCR3_GATEDXCTLCLK |
		DWC_DDR_PHY_REGS_PGCR3_GATEDXDDRCLK |
		DWC_DDR_PHY_REGS_PGCR3_GATEDXRDCLK,
		DWC_DDR_PHY_REGS_PGCR3_GATEDXCTLCLK |
		DWC_DDR_PHY_REGS_PGCR3_GATEDXDDRCLK |
		DWC_DDR_PHY_REGS_PGCR3_GATEDXRDCLK);

	/* re-enable refreshes from PHY */
	al_reg_write32_masked(
		&phy_regs->DTCR,
		DWC_DDR_PHY_REGS_DTCR_RFSHDT_MASK,
		(0x9 << DWC_DDR_PHY_REGS_DTCR_RFSHDT_SHIFT));

	/* re-enable PHY initiated updates */
	al_reg_write32_masked(
		&phy_regs->DSGCR,
		DWC_DDR_PHY_REGS_DSGCR_PUREN,
		DWC_DDR_PHY_REGS_DSGCR_PUREN);

	if (vt_calc_enable)
		al_ddr_phy_vt_calc_enable(ddr_cfg);
	al_ddr_ctrl_resume(ddr_cfg);
}
/*******************************************************************************
 ******************************************************************************/
DDR_STATIC void _al_ddr_phy_datx_bist_post(
	struct al_ddr_cfg	*ddr_cfg)
{
	al_ddr_phy_datx_bist_post_adv(ddr_cfg, AL_TRUE);
}
/*******************************************************************************
 ******************************************************************************/
DDR_STATIC int _al_ddr_phy_datx_bist(
	struct al_ddr_cfg		*ddr_cfg,
	struct al_ddr_bist_params	*params,
	struct al_ddr_bist_err_status	*bist_err_status)
{
	int i;
	int err = 0;

	STRUCT_PHY_REGS __iomem *phy_regs = PHY_REGS(ddr_cfg->ddr_phy_regs_base);

	uint32_t mode;
	uint32_t pattern;

	mode =
		(params->mode == AL_DDR_BIST_MODE_LOOPBACK) ?
		DWC_DDR_PHY_REGS_BISTRR_BMODE_LOOPBACK :
		DWC_DDR_PHY_REGS_BISTRR_BMODE_DRAM;

	pattern =
		(params->pat == AL_DDR_BIST_PATTERN_WALK_0) ?
		DWC_DDR_PHY_REGS_BISTRR_BDPAT_WALKING_0 :
		(params->pat == AL_DDR_BIST_PATTERN_WALK_1) ?
		DWC_DDR_PHY_REGS_BISTRR_BDPAT_WALKING_1 :
		(params->pat == AL_DDR_BIST_PATTERN_LFSR) ?
		DWC_DDR_PHY_REGS_BISTRR_BDPAT_LFSR :
		DWC_DDR_PHY_REGS_BISTRR_BDPAT_USER;

	/**
	* Init BIST mode of operation
	*/

	/* BISTUDPR */
	al_reg_write32_masked(
		&phy_regs->BISTUDPR,
		DWC_DDR_PHY_REGS_BISTUDPR_BUDP0_MASK |
		DWC_DDR_PHY_REGS_BISTUDPR_BUDP1_MASK,
		DWC_DDR_PHY_REGS_BISTUDPR_BUDP0(params->user_pat_even) |
		DWC_DDR_PHY_REGS_BISTUDPR_BUDP1(params->user_pat_odd));

	/* BISTWCR */
	al_reg_write32_masked(
		&phy_regs->BISTWCR,
		DWC_DDR_PHY_REGS_BISTWCR_BWCNT_MASK,
		DWC_DDR_PHY_REGS_BISTWCR_BWCNT(params->wc));

	/* BISTAR0 */
	al_reg_write32_masked(
		&phy_regs->BISTAR[0],
		DWC_DDR_PHY_REGS_BISTAR0_BCOL_MASK |
		DWC_DDR_PHY_REGS_BISTAR0_BROW_MASK |
		DWC_DDR_PHY_REGS_BISTAR0_BBANK_MASK,
		DWC_DDR_PHY_REGS_BISTAR0_BCOL(params->col_min) |
		DWC_DDR_PHY_REGS_BISTAR0_BROW(params->row_min) |
		DWC_DDR_PHY_REGS_BISTAR0_BBANK(params->bank_min));

	/* BISTAR1 */
	al_reg_write32_masked(
		&phy_regs->BISTAR[1],
		DWC_DDR_PHY_REGS_BISTAR1_BRANK_MASK |
		DWC_DDR_PHY_REGS_BISTAR1_BMRANK_MASK |
		DWC_DDR_PHY_REGS_BISTAR1_BAINC_MASK,
		DWC_DDR_PHY_REGS_BISTAR1_BRANK(params->rank_min) |
		DWC_DDR_PHY_REGS_BISTAR1_BMRANK(params->rank_max) |
		DWC_DDR_PHY_REGS_BISTAR1_BAINC(params->inc));

	/* BISTAR2 */
	al_reg_write32_masked(
		&phy_regs->BISTAR[2],
		DWC_DDR_PHY_REGS_BISTAR2_BMCOL_MASK |
		DWC_DDR_PHY_REGS_BISTAR2_BMROW_MASK |
		DWC_DDR_PHY_REGS_BISTAR2_BMBANK_MASK,
		DWC_DDR_PHY_REGS_BISTAR2_BMCOL(params->col_max) |
		DWC_DDR_PHY_REGS_BISTAR2_BMROW(params->row_max) |
		DWC_DDR_PHY_REGS_BISTAR2_BMBANK(params->bank_max));

	/* Run DATX8 BIST */
	for (i = 0; i < AL_DDR_PHY_NUM_BYTE_LANES; i++) {
		if (!params->active_byte_lanes[i])
			continue;

		/* Reset status */
		al_reg_write32_masked(
			&phy_regs->BISTRR,
			DWC_DDR_PHY_REGS_BISTRR_BINST_MASK,
			DWC_DDR_PHY_REGS_BISTRR_BINST_RESET);

		/* Run BIST */
		al_reg_write32_masked(
			&phy_regs->BISTRR,
			DWC_DDR_PHY_REGS_BISTRR_BINST_MASK |
			DWC_DDR_PHY_REGS_BISTRR_BMODE_MASK |
			DWC_DDR_PHY_REGS_BISTRR_BDXEN |
			DWC_DDR_PHY_REGS_BISTRR_BACEN |
			DWC_DDR_PHY_REGS_BISTRR_BDPAT_MASK |
			DWC_DDR_PHY_REGS_BISTRR_BDXSEL_MASK,
			DWC_DDR_PHY_REGS_BISTRR_BINST_RUN |
			mode |
			DWC_DDR_PHY_REGS_BISTRR_BDXEN |
			pattern |
			DWC_DDR_PHY_REGS_BISTRR_BDXSEL(i));

		al_data_memory_barrier();

		/* Read BISTGSR for BIST done */
		err = al_ddr_phy_wait_for_bist_done(ddr_cfg, bist_err_status);
		if (err) {
			if (bist_err_status)
				bist_err_status->lane_err = i;
			al_dbg(
				"%s:%d: al_ddr_phy_wait_for_bist_done failed "
				"(byte lane %d)!\n",
				__func__,
				__LINE__,
				i);
			break;
		}
	}

	/* stop BIST */
	al_reg_write32_masked(
		&phy_regs->BISTRR,
		DWC_DDR_PHY_REGS_BISTRR_BINST_MASK,
		DWC_DDR_PHY_REGS_BISTRR_BINST_STOP);

	al_data_memory_barrier();

	/* Waiting BIST to stop */
	err = al_ddr_phy_wait_for_bist_done(ddr_cfg, bist_err_status);
	if (err)
		al_dbg(
			"%s:%d: al_ddr_phy_wait_for_bist_done failed\n",
			__func__,
			__LINE__);

	al_reg_write32_masked(
		&phy_regs->BISTRR,
		DWC_DDR_PHY_REGS_BISTRR_BINST_MASK,
		DWC_DDR_PHY_REGS_BISTRR_BINST_NOP);

	return err;
}

/*******************************************************************************
 ******************************************************************************/
DDR_STATIC int _al_ddr_phy_ac_bist(
	struct al_ddr_cfg		*ddr_cfg,
	struct al_ddr_bist_err_status	*bist_err_status,
	enum al_ddr_bist_pat		pat)
{
	int err;

	STRUCT_PHY_REGS __iomem *phy_regs = PHY_REGS(ddr_cfg->ddr_phy_regs_base);

	uint32_t pattern;

	/* Run AC BIST */
	pattern =
		(pat == AL_DDR_BIST_PATTERN_WALK_0) ?
		DWC_DDR_PHY_REGS_BISTRR_BDPAT_WALKING_0 :
		(pat == AL_DDR_BIST_PATTERN_WALK_1) ?
		DWC_DDR_PHY_REGS_BISTRR_BDPAT_WALKING_1 :
		DWC_DDR_PHY_REGS_BISTRR_BDPAT_LFSR;

	/* Reset status */
	al_reg_write32_masked(
		&phy_regs->BISTRR,
		DWC_DDR_PHY_REGS_BISTRR_BINST_MASK,
		DWC_DDR_PHY_REGS_BISTRR_BINST_RESET);

	/* Power up I/O receivers */
	al_reg_write32_masked(
		&phy_regs->ACIOCR[0],
		DWC_DDR_PHY_REGS_ACIOCR0_ACPDR |
		DWC_DDR_PHY_REGS_ACIOCR0_CKPDR_MASK |
		DWC_DDR_PHY_REGS_ACIOCR0_RANKPDR_MASK |
		DWC_DDR_PHY_REGS_ACIOCR0_RSTPDR,
		0);

	/* Loopback before buffer in I/O */
	al_reg_write32_masked(
		&phy_regs->PGCR[1],
		DWC_DDR_PHY_REGS_PGCR1_IOLB,
		DWC_DDR_PHY_REGS_PGCR1_IOLB);

	/* Run BIST */
	al_reg_write32_masked(
		&phy_regs->BISTRR,
		DWC_DDR_PHY_REGS_BISTRR_BINST_MASK |
		DWC_DDR_PHY_REGS_BISTRR_BMODE_MASK |
		DWC_DDR_PHY_REGS_BISTRR_BDXEN |
		DWC_DDR_PHY_REGS_BISTRR_BACEN |
		DWC_DDR_PHY_REGS_BISTRR_BDPAT_MASK,
		DWC_DDR_PHY_REGS_BISTRR_BINST_RUN |
		DWC_DDR_PHY_REGS_BISTRR_BMODE_LOOPBACK |
		DWC_DDR_PHY_REGS_BISTRR_BACEN |
		pattern);

	al_data_memory_barrier();

	/* Read BISTGSR for BIST done */
	err = al_ddr_phy_wait_for_bist_done(ddr_cfg, bist_err_status);
	if (err) {
		al_err(
			"%s:%d: al_ddr_phy_wait_for_bist_done failed!\n",
			__func__,
			__LINE__);
		return err;
	}

	/* Power down I/O receivers */
	al_reg_write32_masked(
		&phy_regs->ACIOCR[0],
		DWC_DDR_PHY_REGS_ACIOCR0_ACPDR |
		DWC_DDR_PHY_REGS_ACIOCR0_CKPDR_MASK |
		DWC_DDR_PHY_REGS_ACIOCR0_RANKPDR_MASK |
		DWC_DDR_PHY_REGS_ACIOCR0_RSTPDR,
		DWC_DDR_PHY_REGS_ACIOCR0_ACPDR |
		DWC_DDR_PHY_REGS_ACIOCR0_CKPDR_MASK |
		DWC_DDR_PHY_REGS_ACIOCR0_RANKPDR_MASK |
		DWC_DDR_PHY_REGS_ACIOCR0_RSTPDR);

	/* stop BIST */
	al_reg_write32_masked(
		&phy_regs->BISTRR,
		DWC_DDR_PHY_REGS_BISTRR_BINST_MASK,
		DWC_DDR_PHY_REGS_BISTRR_BINST_NOP);

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
DDR_STATIC int _al_ddr_power_mode_set_reg_ptr_and_val(
	struct al_ddr_cfg	*ddr_cfg,
	enum al_ddr_power_mode	power_mode,
	al_bool			clk_disable,
	unsigned int		timer_x32,
	uint32_t __iomem	**reg_ptrs,
	uint32_t		*reg_vals,
	unsigned int		*num_regs)
{
	union al_ddr_ctrl_regs __iomem *ctrl_regs =
		(union al_ddr_ctrl_regs __iomem *)
		ddr_cfg->ddr_ctrl_regs_base;
	unsigned int reg_num = 0;
	uint32_t mode =
		(power_mode == AL_DDR_POWERMODE_SELF_REFRESH) ?
		DWC_DDR_UMCTL2_REGS_PWRCTL_SELFREF_EN :
		(power_mode == AL_DDR_POWERMODE_POWER_DOWN) ?
		DWC_DDR_UMCTL2_REGS_PWRCTL_POWERDOWN_EN :
		(power_mode == AL_DDR_POWERMODE_MPSM) ?
		DWC_DDR_UMCTL2_REGS_PWRCTL_MPSM_EN :
		(power_mode == AL_DDR_POWERMODE_SELF_REFRESH_SW) ?
		DWC_DDR_UMCTL2_REGS_PWRCTL_SELFREF_SW :
		0;

	if ((*num_regs) != 2)
		return -EINVAL;

	if (clk_disable)
		mode |= DWC_DDR_UMCTL2_REGS_PWRCTL_EN_DFI_DRAM_CLK_DISABLE;

	/*
	* Addressing RMN: 1037
	*
	* RMN description:
	* In the current logic, it is possible for DRAM Read data
	* and/or Write data to be active while/after one of the
	* following occurs:
	* Power Down Entry (PDE)
	* Self Refresh Entry (SRE)
	* This would violate the memory protocol DDR3 which require
	* "no data bursts are in progress" when the above
	* commands occur.
	* Software flow:
	* For violations related to PDE, issue can be avoided by
	* ensuring that timer_x32>1. For violations related to SRE,
	* ensure that AL_DDR_POWERMODE_SELF_REFRESH is set only after
	* all Read data has been returned on your application
	* interface and all write data has reached the DRAM.
	*/

	al_assert((power_mode == AL_DDR_POWERMODE_SELF_REFRESH) ||
		(power_mode == AL_DDR_POWERMODE_POWER_DOWN) ||
		(power_mode == AL_DDR_POWERMODE_OFF));

	if (timer_x32 <= 1) {
		al_err(
			"%s:%d: power mode timer must be greater than 1!\n",
			__func__,
			__LINE__);
		return -EIO;
	}

	reg_ptrs[reg_num] = CTRL_REG(pwrtmg);
	reg_vals[reg_num] = timer_x32 << DWC_DDR_UMCTL2_REGS_PWRTMG_POWERDOWN_TO_X32_SHIFT;
	reg_num++;

	reg_ptrs[reg_num] = CTRL_REG(pwrctl);
	reg_vals[reg_num] = mode;
	reg_num++;

	*num_regs = reg_num;

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
DDR_STATIC int _al_ddr_power_mode_set(
	struct al_ddr_cfg	*ddr_cfg,
	enum al_ddr_power_mode	power_mode,
	al_bool			clk_disable,
	unsigned int		timer_x32)
{
	uint32_t __iomem *reg_ptrs[2];
	unsigned int num_regs = 2;
	uint32_t reg_vals[2];
	unsigned int i;
	int err;

	err = _al_ddr_power_mode_set_reg_ptr_and_val(ddr_cfg, power_mode, clk_disable, timer_x32,
		reg_ptrs, reg_vals, &num_regs);
	if (err)
		return err;

	for (i = 0; i < num_regs; i++)
		al_reg_write32(reg_ptrs[i], reg_vals[i]);

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
DDR_STATIC enum al_ddr_operating_mode _al_ddr_operating_mode_get(
	struct al_ddr_cfg		*ddr_cfg)
{
	union al_ddr_ctrl_regs __iomem *ctrl_regs =
		(union al_ddr_ctrl_regs __iomem *)
		ddr_cfg->ddr_ctrl_regs_base;
	uint32_t reg_val;
	enum al_ddr_operating_mode operating_mode;

	reg_val = al_reg_read32(CTRL_REG(stat));
	reg_val &= DWC_DDR_UMCTL2_REGS_STAT_OPERATING_MODE_MASK;

	operating_mode =
		(reg_val == DWC_DDR_UMCTL2_REGS_STAT_OPERATING_MODE_INIT) ?
		AL_DDR_OPERATING_MODE_INIT :
		(reg_val == DWC_DDR_UMCTL2_REGS_STAT_OPERATING_MODE_NORMAL) ?
		AL_DDR_OPERATING_MODE_NORMAL :
		(reg_val == DWC_DDR_UMCTL2_REGS_STAT_OPERATING_MODE_POWER_DOWN) ?
		AL_DDR_OPERATING_MODE_POWER_DOWN :
		AL_DDR_OPERATING_MODE_SELF_REFRESH;

	return operating_mode;
}

/*******************************************************************************
 ******************************************************************************/
static void read_bist_err_status(
	struct al_ddr_cfg		*ddr_cfg,
	struct al_ddr_bist_err_status	*bist_err_status)
{
	uint32_t reg_val;
	int i;

	STRUCT_PHY_REGS __iomem *phy_regs = PHY_REGS(ddr_cfg->ddr_phy_regs_base);

	bist_err_status->word_err =
		(al_reg_read32(&phy_regs->BISTWER) &
		DWC_DDR_PHY_REGS_BISTWER_DXWER_MASK) >>
		DWC_DDR_PHY_REGS_BISTWER_DXWER_SHIFT;

	reg_val = al_reg_read32(&phy_regs->BISTBER[2]);
	for (i = 0; i < AL_DDR_BIST_DQ_BITS; ++i) {
		bist_err_status->even_risg_err[i] =
			(reg_val &
			DWC_DDR_PHY_REGS_BISTBER2_DQBER0_RISG_DQ_I_MASK(i)) >>
			DWC_DDR_PHY_REGS_BISTBER2_DQBER0_RISG_DQ_I_SHIFT(i);

		bist_err_status->even_fall_err[i] =
			(reg_val &
			DWC_DDR_PHY_REGS_BISTBER2_DQBER0_FALL_DQ_I_MASK(i)) >>
			DWC_DDR_PHY_REGS_BISTBER2_DQBER0_FALL_DQ_I_SHIFT(i);
	}

	for (i = 0; i < AL_DDR_BIST_DQ_BITS; ++i) {
		reg_val = al_reg_read32(&phy_regs->BISTBER[3]);
		bist_err_status->odd_risg_err[i] =
			(reg_val &
			DWC_DDR_PHY_REGS_BISTBER3_DQBER1_RISG_DQ_I_MASK(i)) >>
			DWC_DDR_PHY_REGS_BISTBER3_DQBER1_RISG_DQ_I_SHIFT(i);

		bist_err_status->odd_fall_err[i] =
			(reg_val &
			DWC_DDR_PHY_REGS_BISTBER3_DQBER1_FALL_DQ_I_MASK(i)) >>
			DWC_DDR_PHY_REGS_BISTBER3_DQBER1_FALL_DQ_I_SHIFT(i);
	}

	bist_err_status->word_count =
		(al_reg_read32(&phy_regs->BISTWCSR) &
		DWC_DDR_PHY_REGS_BISTWCSR_DXWCNT_MASK) >>
		DWC_DDR_PHY_REGS_BISTWCSR_DXWCNT_SHIFT;

	reg_val = al_reg_read32(&phy_regs->BISTFWR[2]);
	for (i = 0; i < AL_DDR_BIST_DQ_BITS; ++i) {
		bist_err_status->beat_1st_err[i] =
			reg_val & DWC_DDR_PHY_REGS_BISTFWR2_DQWEBS_1ST_DQ_I(i);

		bist_err_status->beat_2nd_err[i] =
			reg_val & DWC_DDR_PHY_REGS_BISTFWR2_DQWEBS_2ND_DQ_I(i);

		bist_err_status->beat_3rd_err[i] =
			reg_val & DWC_DDR_PHY_REGS_BISTFWR2_DQWEBS_3RD_DQ_I(i);

		bist_err_status->beat_4th_err[i] =
			reg_val & DWC_DDR_PHY_REGS_BISTFWR2_DQWEBS_4TH_DQ_I(i);
	}

	bist_err_status->timeout = AL_FALSE;
}

/*******************************************************************************
 ******************************************************************************/
static int al_ddr_phy_wait_for_bist_done(
	struct al_ddr_cfg		*ddr_cfg,
	struct al_ddr_bist_err_status	*bist_err_status)
{
	int err = 0;
	uint32_t reg_val;

	STRUCT_PHY_REGS __iomem *phy_regs = PHY_REGS(ddr_cfg->ddr_phy_regs_base);

	err = al_ddr_reg_poll32(
		&phy_regs->BISTGSR,
		DWC_DDR_PHY_REGS_BISTGSR_BDONE,
		DWC_DDR_PHY_REGS_BISTGSR_BDONE,
		BIST_TIMEOUT);

	if (err) {
		int err2 = 0;
		al_reg_write32_masked(&phy_regs->PIR,
				DWC_DDR_PHY_REGS_PIR_INIT |
				DWC_DDR_PHY_REGS_PIR_PHYRST,
				DWC_DDR_PHY_REGS_PIR_INIT |
				DWC_DDR_PHY_REGS_PIR_PHYRST);
		al_data_memory_barrier();
		err2 = al_ddr_reg_poll32(
			&phy_regs->PGSR[0],
			DWC_DDR_PHY_REGS_PGSR0_IDONE,
			DWC_DDR_PHY_REGS_PGSR0_IDONE,
			DEFAULT_TIMEOUT);
		if (err2)
			al_err("%s: al_ddr_reg_poll32 failed!\n", __func__);

		if (bist_err_status) {
			read_bist_err_status(ddr_cfg, bist_err_status);
			bist_err_status->timeout = AL_TRUE;

			/* Reinitialize LFSR seed */
			al_reg_write32(
				&phy_regs->BISTLSR,
				BIST_LFSR_RESET_VAL);
		}

		al_dbg("%s: al_ddr_reg_poll32 failed!\n", __func__);
		al_dbg("%s: PHY bist error (BISTGSR = %08x)!\n",
			__func__, al_reg_read32(&phy_regs->BISTGSR));
		al_dbg("%s: (BISTWER = %08x)!\n",
			__func__, al_reg_read32(&phy_regs->BISTWER));
		al_dbg("%s: (BISTBER2 = %08x)!\n",
			__func__, al_reg_read32(&phy_regs->BISTBER[2]));
		al_dbg("%s: (BISTBER3 = %08x)!\n",
			__func__, al_reg_read32(&phy_regs->BISTBER[3]));
		al_dbg("%s: (BISTWCSR = %08x)!\n",
			__func__, al_reg_read32(&phy_regs->BISTWCSR));
		al_dbg("%s: (BISTFWR2 = %08x)!\n",
			__func__, al_reg_read32(&phy_regs->BISTFWR[2]));
		return err;
	}

	reg_val = al_reg_read32(&phy_regs->BISTGSR);
	/* Make sure no bist errors */
	if (reg_val &
		(DWC_DDR_PHY_REGS_BISTGSR_BACERR |
		DWC_DDR_PHY_REGS_BISTGSR_BDXERR)) {
		if (bist_err_status) {
			read_bist_err_status(ddr_cfg, bist_err_status);
			bist_err_status->timeout = AL_FALSE;
		}
		al_dbg("%s: PHY bist error (BISTGSR = %08x)!\n",
			__func__, reg_val);
		al_dbg("%s: (BISTWER = %08x)!\n",
			__func__, al_reg_read32(
			&phy_regs->BISTWER));
		al_dbg("%s: (BISTBER2 = %08x)!\n",
			__func__, al_reg_read32(
			&phy_regs->BISTBER[2]));
		al_dbg("%s: (BISTBER3 = %08x)!\n",
			__func__, al_reg_read32(
			&phy_regs->BISTBER[3]));
		al_dbg("%s: (BISTWCSR = %08x)!\n",
			__func__, al_reg_read32(
			&phy_regs->BISTWCSR));
		al_dbg("%s: (BISTFWR2 = %08x)!\n",
			__func__, al_reg_read32(
			&phy_regs->BISTFWR[2]));
		return -EIO;
	}

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
DDR_STATIC unsigned int _al_ddr_active_ranks_get(
	struct al_ddr_cfg		*ddr_cfg)
{
	union al_ddr_ctrl_regs __iomem *ctrl_regs =
		(union al_ddr_ctrl_regs __iomem *)
		ddr_cfg->ddr_ctrl_regs_base;
	uint32_t mstr_val;
	unsigned int active_ranks = 0;

	mstr_val = al_reg_read32(CTRL_REG(mstr));
	mstr_val &= DWC_DDR_UMCTL2_REGS_MSTR_ACTIVE_RANKS_MASK;
	mstr_val >>= DWC_DDR_UMCTL2_REGS_MSTR_ACTIVE_RANKS_SHIFT;

	/* each bit on mstr_val is corresponding to an available rank */
	while (mstr_val > 0) {
		if (mstr_val & AL_BIT(0))
			active_ranks++;
		mstr_val >>= 1;
	}

	return active_ranks;
}

/*******************************************************************************
 ******************************************************************************/
DDR_STATIC unsigned int _al_ddr_active_ranks_mask_get(
	struct al_ddr_cfg		*ddr_cfg)
{
	union al_ddr_ctrl_regs __iomem *ctrl_regs =
		(union al_ddr_ctrl_regs __iomem *)
		ddr_cfg->ddr_ctrl_regs_base;
	uint32_t mstr_val;

	mstr_val = al_reg_read32(CTRL_REG(mstr));
	mstr_val &= DWC_DDR_UMCTL2_REGS_MSTR_ACTIVE_RANKS_MASK;
	mstr_val >>= DWC_DDR_UMCTL2_REGS_MSTR_ACTIVE_RANKS_SHIFT;

	return mstr_val;
}

/*******************************************************************************
 ******************************************************************************/
DDR_STATIC unsigned int _al_ddr_logical_rank_from_phys(
	struct al_ddr_cfg		*ddr_cfg __attribute__((unused)),
	unsigned int			rank)
{
	return rank;
}

/*******************************************************************************
 ******************************************************************************/
DDR_STATIC unsigned int _al_ddr_active_banks_get(
	struct al_ddr_cfg		*ddr_cfg)

{
	struct al_ddr_addrmap addrmap;
	unsigned int num = 0;
	unsigned int i;

	al_ddr_address_map_get(ddr_cfg, &addrmap);

	for (i = 0; i < AL_ARR_SIZE(addrmap.bank_b0_2); i++) {
		if (addrmap.bank_b0_2[i] != AL_DDR_ADDRMAP_NC)
			num++;
	}

	return 1 << num;
}

/*******************************************************************************
 ******************************************************************************/
DDR_STATIC unsigned int _al_ddr_active_columns_get(
	struct al_ddr_cfg		*ddr_cfg)

{
	struct al_ddr_addrmap addrmap;
	unsigned int num = 3; /* At least 3 column bits */
	unsigned int i;

	al_ddr_address_map_get(ddr_cfg, &addrmap);

	for (i = 0; i < AL_ARR_SIZE(addrmap.col_b3_9_b11_13); i++) {
		if (addrmap.col_b3_9_b11_13[i] != AL_DDR_ADDRMAP_NC)
			num++;
	}

	return 1 << num;
}

/*******************************************************************************
 ******************************************************************************/
DDR_STATIC unsigned int _al_ddr_active_rows_get(
	struct al_ddr_cfg		*ddr_cfg)
{
	struct al_ddr_addrmap addrmap;
	unsigned int num = 0;
	unsigned int i;

	al_ddr_address_map_get(ddr_cfg, &addrmap);

	for (i = 0; i < (AL_ARR_SIZE(addrmap.row_b0_17)); i++) {
		if (addrmap.row_b0_17[i] != AL_DDR_ADDRMAP_NC)
			num++;
	}

	return 1 << num;
}

/*******************************************************************************
 ******************************************************************************/
DDR_STATIC int _al_ddr_ecc_status_get(
	struct al_ddr_cfg		*ddr_cfg,
	struct al_ddr_ecc_status	*corr_status,
	struct al_ddr_ecc_status	*uncorr_status)
{
	uint32_t reg_val;

	union al_ddr_ctrl_regs __iomem *ctrl_regs =
		(union al_ddr_ctrl_regs __iomem *)
		ddr_cfg->ddr_ctrl_regs_base;

	/* Correctable status */
	if (corr_status) {
		reg_val = al_reg_read32(CTRL_REG(eccstat));
		corr_status->ecc_corrected_bit_num = (reg_val &
			DWC_DDR_UMCTL2_REGS_ECCSTAT_ECC_CORRECTED_BIT_NUM_MASK)
			>> DWC_DDR_UMCTL2_REGS_ECCSTAT_ECC_CORRECTED_BIT_NUM_SHIFT;

		reg_val = al_reg_read32(CTRL_REG(eccerrcnt));
		corr_status->err_cnt = (reg_val &
			DWC_DDR_UMCTL2_REGS_ECCERRCNT_ECC_CORR_ERR_CNT_MASK)
			>> DWC_DDR_UMCTL2_REGS_ECCERRCNT_ECC_CORR_ERR_CNT_SHIFT;

		reg_val = al_reg_read32(CTRL_REG(ecccaddr0));
		corr_status->row = (reg_val &
			DWC_DDR_UMCTL2_REGS_ECCCADDR0_ECC_CORR_ROW_MASK)
			>> DWC_DDR_UMCTL2_REGS_ECCCADDR0_ECC_CORR_ROW_SHIFT;
		corr_status->rank = (reg_val &
			DWC_DDR_UMCTL2_REGS_ECCCADDR0_ECC_CORR_RANK_MASK)
			>> DWC_DDR_UMCTL2_REGS_ECCCADDR0_ECC_CORR_RANK_SHIFT;

		reg_val = al_reg_read32(CTRL_REG(ecccaddr1));
		corr_status->bg = 0;
		corr_status->bank = (reg_val &
			DWC_DDR_UMCTL2_REGS_ECCCADDR1_ECC_CORR_BANK_MASK)
			>> DWC_DDR_UMCTL2_REGS_ECCCADDR1_ECC_CORR_BANK_SHIFT;
		corr_status->col = (reg_val &
			DWC_DDR_UMCTL2_REGS_ECCCADDR1_ECC_CORR_COL_MASK)
			>> DWC_DDR_UMCTL2_REGS_ECCCADDR1_ECC_CORR_COL_SHIFT;

		corr_status->syndromes_31_0 = al_reg_read32(CTRL_REG(ecccsyn0));
		corr_status->syndromes_63_32 = al_reg_read32(CTRL_REG(ecccsyn1));
		reg_val = al_reg_read32(CTRL_REG(ecccsyn2));
		corr_status->syndromes_ecc = (reg_val &
			DWC_DDR_UMCTL2_REGS_ECCCSYN2_ECC_CORR_SYNDROMES_71_64_MASK)
			>> DWC_DDR_UMCTL2_REGS_ECCCSYN2_ECC_CORR_SYNDROMES_71_64_SHIFT;

		corr_status->corr_bit_mask_31_0 =
			al_reg_read32(CTRL_REG(eccbitmask0));
		corr_status->corr_bit_mask_63_32 =
			al_reg_read32(CTRL_REG(eccbitmask1));
		reg_val = al_reg_read32(CTRL_REG(eccbitmask2));
		corr_status->corr_bit_mask_ecc = (reg_val &
			DWC_DDR_UMCTL2_REGS_ECCBITMASK2_ECC_CORR_BIT_MASK_71_64_MASK) >>
			DWC_DDR_UMCTL2_REGS_ECCBITMASK2_ECC_CORR_BIT_MASK_71_64_SHIFT;

		/* CID not relevant for alpine-v1 */
		corr_status->cid = 0;
	}

	/* Uncorrectable status */
	if (uncorr_status) {
		reg_val = al_reg_read32(CTRL_REG(eccerrcnt));
		uncorr_status->err_cnt = (reg_val &
			DWC_DDR_UMCTL2_REGS_ECCERRCNT_ECC_UNCORR_ERR_CNT_MASK)
			>> DWC_DDR_UMCTL2_REGS_ECCERRCNT_ECC_UNCORR_ERR_CNT_SHIFT;

		reg_val = al_reg_read32(CTRL_REG(eccuaddr0));
		uncorr_status->row = (reg_val &
			DWC_DDR_UMCTL2_REGS_ECCUADDR0_ECC_UNCORR_ROW_MASK)
			>> DWC_DDR_UMCTL2_REGS_ECCUADDR0_ECC_UNCORR_ROW_SHIFT;
		uncorr_status->rank = (reg_val &
			DWC_DDR_UMCTL2_REGS_ECCUADDR0_ECC_UNCORR_RANK_MASK)
			>> DWC_DDR_UMCTL2_REGS_ECCUADDR0_ECC_UNCORR_RANK_SHIFT;

		reg_val = al_reg_read32(CTRL_REG(eccuaddr1));
		uncorr_status->bg = 0;
		uncorr_status->bank = (reg_val &
			DWC_DDR_UMCTL2_REGS_ECCUADDR1_ECC_UNCORR_BANK_MASK)
			>> DWC_DDR_UMCTL2_REGS_ECCUADDR1_ECC_UNCORR_BANK_SHIFT;
		uncorr_status->col = (reg_val &
			DWC_DDR_UMCTL2_REGS_ECCUADDR1_ECC_UNCORR_COL_MASK)
			>> DWC_DDR_UMCTL2_REGS_ECCUADDR1_ECC_UNCORR_COL_SHIFT;

		uncorr_status->syndromes_31_0 = al_reg_read32(CTRL_REG(eccusyn0));
		uncorr_status->syndromes_63_32 = al_reg_read32(CTRL_REG(eccusyn1));
		reg_val = al_reg_read32(CTRL_REG(eccusyn2));
		uncorr_status->syndromes_ecc = (reg_val &
			DWC_DDR_UMCTL2_REGS_ECCUSYN2_ECC_UNCORR_SYNDROMES_71_64_MASK) >>
			DWC_DDR_UMCTL2_REGS_ECCUSYN2_ECC_UNCORR_SYNDROMES_71_64_SHIFT;

		/* CID not relevant for alpine-v1 */
		uncorr_status->cid = 0;
	}

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
static al_bool _al_ddr_ecc_is_enabled(
	struct al_ddr_cfg		*ddr_cfg)
{
	union al_ddr_ctrl_regs __iomem *ctrl_regs =
		(union al_ddr_ctrl_regs __iomem *)
		ddr_cfg->ddr_ctrl_regs_base;
	uint32_t reg_val;

	reg_val = al_reg_read32(CTRL_REG(ecccfg0));

	return ((reg_val & DWC_DDR_UMCTL2_REGS_ECCCFG0_ECC_MODE_MASK) ==
		DWC_DDR_UMCTL2_REGS_ECCCFG0_ECC_MODE_DIS) ? AL_FALSE : AL_TRUE;
}

/*******************************************************************************
 ******************************************************************************/
DDR_STATIC void _al_ddr_ecc_cfg_get(
	struct al_ddr_cfg		*ddr_cfg,
	struct al_ddr_ecc_cfg		*ecc_cfg)
{
	union al_ddr_ctrl_regs __iomem *ctrl_regs =
		(union al_ddr_ctrl_regs __iomem *)
		ddr_cfg->ddr_ctrl_regs_base;
	uint32_t cfg_val;

	cfg_val = al_reg_read32(CTRL_REG(ecccfg0));

	ecc_cfg->ecc_enabled =
		((cfg_val & DWC_DDR_UMCTL2_REGS_ECCCFG0_ECC_MODE_MASK) ==
		DWC_DDR_UMCTL2_REGS_ECCCFG0_ECC_MODE_DIS) ? AL_FALSE : AL_TRUE;

	/* dis_scrub is valid only when ecc mode is enabled */
	if (ecc_cfg->ecc_enabled)
		ecc_cfg->scrub_enabled =
			(cfg_val & DWC_DDR_UMCTL2_REGS_ECCCFG0_DIS_SCRUB) ?
			AL_FALSE : AL_TRUE;
}

/*******************************************************************************
 ******************************************************************************/
DDR_STATIC int _al_ddr_ecc_corr_count_clear(
	struct al_ddr_cfg		*ddr_cfg)
{
	union al_ddr_ctrl_regs __iomem *ctrl_regs =
		(union al_ddr_ctrl_regs __iomem *)
		ddr_cfg->ddr_ctrl_regs_base;

	al_reg_write32(CTRL_REG(eccclr),
			DWC_DDR_UMCTL2_REGS_ECCCLR_ECC_CLR_CORR_ERR_CNT);

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
DDR_STATIC int _al_ddr_ecc_corr_int_clear(
	struct al_ddr_cfg		*ddr_cfg)
{
	union al_ddr_ctrl_regs __iomem *ctrl_regs =
		(union al_ddr_ctrl_regs __iomem *)
		ddr_cfg->ddr_ctrl_regs_base;

	al_reg_write32(CTRL_REG(eccclr),
			DWC_DDR_UMCTL2_REGS_ECCCLR_ECC_CLR_CORR_ERR);

	if (ddr_cfg->nb_regs_base) {
		struct al_nb_regs __iomem *nb_regs;

		al_data_memory_barrier();

		nb_regs = (struct al_nb_regs __iomem *)ddr_cfg->nb_regs_base;

		al_reg_write32(&nb_regs->global.nb_int_cause,
			~(NB_GLOBAL_NB_INT_CAUSE_MCTL_ECC_CORR_ERR |
				NB_GLOBAL_NB_INT_CAUSE_ERR_CAUSE_SUM));
	}

	return 0;
}


/*******************************************************************************
 ******************************************************************************/
DDR_STATIC int _al_ddr_ecc_uncorr_count_clear(
	struct al_ddr_cfg		*ddr_cfg)
{
	union al_ddr_ctrl_regs __iomem *ctrl_regs =
		(union al_ddr_ctrl_regs __iomem *)
		ddr_cfg->ddr_ctrl_regs_base;

	al_reg_write32(CTRL_REG(eccclr),
			DWC_DDR_UMCTL2_REGS_ECCCLR_ECC_CLR_UNCORR_ERR_CNT);

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
DDR_STATIC int _al_ddr_ecc_uncorr_int_clear(
	struct al_ddr_cfg		*ddr_cfg)
{
	union al_ddr_ctrl_regs __iomem *ctrl_regs =
		(union al_ddr_ctrl_regs __iomem *)
		ddr_cfg->ddr_ctrl_regs_base;

	al_reg_write32(CTRL_REG(eccclr),
			DWC_DDR_UMCTL2_REGS_ECCCLR_ECC_CLR_UNCORR_ERR);

	if (ddr_cfg->nb_regs_base) {
		struct al_nb_regs __iomem *nb_regs;

		al_data_memory_barrier();

		nb_regs = (struct al_nb_regs __iomem *)ddr_cfg->nb_regs_base;

		al_reg_write32(&nb_regs->global.nb_int_cause,
			~(NB_GLOBAL_NB_INT_CAUSE_MCTL_ECC_UNCORR_ERR |
				NB_GLOBAL_NB_INT_CAUSE_ERR_CAUSE_SUM));
	}

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
DDR_STATIC int _al_ddr_ecc_data_poison_enable(
	struct al_ddr_cfg	*ddr_cfg,
	unsigned int		rank,
	unsigned int		bank,
	unsigned int		bg,
	unsigned int		col,
	unsigned int		row,
	al_bool			correctable)
{
	union al_ddr_ctrl_regs __iomem *ctrl_regs =
		(union al_ddr_ctrl_regs __iomem *)
		ddr_cfg->ddr_ctrl_regs_base;

	al_assert(!bg);
	al_assert(!correctable);

	/* Set data poison address */
	al_reg_write32(CTRL_REG(eccpoisonaddr0),
		(col <<
		DWC_DDR_UMCTL2_REGS_ECCPOISONADDR0_ECC_POISON_COL_SHIFT) |
		(rank <<
		DWC_DDR_UMCTL2_REGS_ECCPOISONADDR0_ECC_POISON_RANK_SHIFT));
	al_reg_write32(CTRL_REG(eccpoisonaddr1),
		(row <<
		DWC_DDR_UMCTL2_REGS_ECCPOISONADDR1_ECC_POISON_ROW_SHIFT) |
		(bank <<
		DWC_DDR_UMCTL2_REGS_ECCPOISONADDR1_ECC_POISON_BANK_SHIFT));

	/* Enable data poisoning */
	al_reg_write32(CTRL_REG(ecccfg1),
			DWC_DDR_UMCTL2_REGS_ECCCFG1_DATA_POISON_EN);

	/* Dummy read to make sure register was written before further access to the DDR */
	al_reg_read32(CTRL_REG(ecccfg1));

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
DDR_STATIC int _al_ddr_ecc_data_poison_disable(
	struct al_ddr_cfg		*ddr_cfg)
{
	union al_ddr_ctrl_regs __iomem *ctrl_regs =
		(union al_ddr_ctrl_regs __iomem *)
		ddr_cfg->ddr_ctrl_regs_base;

	/* Disable data poisoning */
	al_reg_write32(CTRL_REG(ecccfg1), 0);

	/* Dummy read to make sure register was written before further access to the DDR */
	al_reg_read32(CTRL_REG(ecccfg1));

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
DDR_STATIC unsigned int _al_ddr_parity_count_get(
	struct al_ddr_cfg		*ddr_cfg)
{
	uint32_t reg_val;
	unsigned int parity_count;

	union al_ddr_ctrl_regs __iomem *ctrl_regs =
		(union al_ddr_ctrl_regs __iomem *)
		ddr_cfg->ddr_ctrl_regs_base;

	reg_val = al_reg_read32(&ctrl_regs->alpine_v1.umctl2_regs.parstat);
	parity_count = (reg_val &
		DWC_DDR_UMCTL2_REGS_CRCPARSTAT_DFI_ALERT_ERR_CNT_MASK)
		>> DWC_DDR_UMCTL2_REGS_CRCPARSTAT_DFI_ALERT_ERR_CNT_SHIFT;

	return parity_count;
}

/*******************************************************************************
 ******************************************************************************/
DDR_STATIC void _al_ddr_parity_count_clear(
	struct al_ddr_cfg		*ddr_cfg)
{
	union al_ddr_ctrl_regs __iomem *ctrl_regs =
		(union al_ddr_ctrl_regs __iomem *)
		ddr_cfg->ddr_ctrl_regs_base;

	al_reg_write32_masked(&ctrl_regs->alpine_v1.umctl2_regs.parctl,
			DWC_DDR_UMCTL2_REGS_CRCPARCTL0_DFI_ALERT_ERR_CNT_CLR,
			DWC_DDR_UMCTL2_REGS_CRCPARCTL0_DFI_ALERT_ERR_CNT_CLR);
}

/*******************************************************************************
 ******************************************************************************/
DDR_STATIC void _al_ddr_parity_int_clear(
	struct al_ddr_cfg		*ddr_cfg)
{
	union al_ddr_ctrl_regs __iomem *ctrl_regs =
		(union al_ddr_ctrl_regs __iomem *)
		ddr_cfg->ddr_ctrl_regs_base;

	struct al_nb_regs __iomem *nb_regs =
		(struct al_nb_regs __iomem *)ddr_cfg->nb_regs_base;

	al_reg_write32_masked(&ctrl_regs->alpine_v1.umctl2_regs.parctl,
			DWC_DDR_UMCTL2_REGS_CRCPARCTL0_DFI_ALERT_ERR_INT_CLR,
			DWC_DDR_UMCTL2_REGS_CRCPARCTL0_DFI_ALERT_ERR_INT_CLR);

	al_data_memory_barrier();

	/* TODO : check if correct for ALPINE_V2 */
	al_reg_write32(&nb_regs->global.nb_int_cause,
		~NB_GLOBAL_NB_INT_CAUSE_MCTL_PARITY_ERR);
}

/*******************************************************************************
 ******************************************************************************/
DDR_STATIC void _al_ddr_address_map_get(
	struct al_ddr_cfg	*ddr_cfg,
	struct al_ddr_addrmap	*addrmap)
{
	union al_ddr_ctrl_regs __iomem *ctrl_regs;
	struct al_nb_regs __iomem *nb_regs;
	enum al_ddr_data_width data_width;

	uint32_t reg_val;
	int addrmap_col_b2_11[10];
	int addrmap_bank_b0_2[3];
	int addrmap_row_b0_17[18];
	int addrmap_cs_b0_1[2];

	unsigned int i;
	unsigned int addrmap_offset = AL_DDR_ADDR_MAP_OFFSET_V1_V2;

	al_assert(ddr_cfg);
	al_assert(addrmap);

	ctrl_regs = (union al_ddr_ctrl_regs __iomem *) ddr_cfg->ddr_ctrl_regs_base;
	nb_regs = (struct al_nb_regs __iomem *)ddr_cfg->nb_regs_base;
	data_width = al_ddr_data_width_get(ddr_cfg);

	reg_val = al_reg_read32(&nb_regs->debug.cpus_general);
	addrmap->swap_14_16_with_17_19 = (reg_val &
		NB_DEBUG_CPUS_GENERAL_ADDR_MAP_ECO) ==
		NB_DEBUG_CPUS_GENERAL_ADDR_MAP_ECO ? AL_TRUE : AL_FALSE;

	/**
	* CS address mapping
	*/
	reg_val = al_reg_read32(CTRL_REG(addrmap0));
	addrmap_cs_b0_1[1] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP0_ADDRMAP_CS_BIT1_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP0_ADDRMAP_CS_BIT1_SHIFT;
	addrmap_cs_b0_1[0] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP0_ADDRMAP_CS_BIT0_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP0_ADDRMAP_CS_BIT0_SHIFT;

	/**
	* Bank address mapping
	*/
	reg_val = al_reg_read32(CTRL_REG(addrmap1));
	addrmap_bank_b0_2[2] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP1_ADDRMAP_BANK_B2_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP1_ADDRMAP_BANK_B2_SHIFT;
	addrmap_bank_b0_2[1] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP1_ADDRMAP_BANK_B1_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP1_ADDRMAP_BANK_B1_SHIFT;
	addrmap_bank_b0_2[0] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP1_ADDRMAP_BANK_B0_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP1_ADDRMAP_BANK_B0_SHIFT;

	/**
	* Column address mapping
	*/
	reg_val = al_reg_read32(CTRL_REG(addrmap2));
	addrmap_col_b2_11[3] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP2_ADDRMAP_COL_B5_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP2_ADDRMAP_COL_B5_SHIFT;
	addrmap_col_b2_11[2] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP2_ADDRMAP_COL_B4_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP2_ADDRMAP_COL_B4_SHIFT;
	addrmap_col_b2_11[1] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP2_ADDRMAP_COL_B3_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP2_ADDRMAP_COL_B3_SHIFT;
	addrmap_col_b2_11[0] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP2_ADDRMAP_COL_B2_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP2_ADDRMAP_COL_B2_SHIFT;

	reg_val = al_reg_read32(CTRL_REG(addrmap3));
	addrmap_col_b2_11[7] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP3_ADDRMAP_COL_B9_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP3_ADDRMAP_COL_B9_SHIFT;
	addrmap_col_b2_11[6] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP3_ADDRMAP_COL_B8_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP3_ADDRMAP_COL_B8_SHIFT;
	addrmap_col_b2_11[5] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP3_ADDRMAP_COL_B7_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP3_ADDRMAP_COL_B7_SHIFT;
	addrmap_col_b2_11[4] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP3_ADDRMAP_COL_B6_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP3_ADDRMAP_COL_B6_SHIFT;

	reg_val = al_reg_read32(CTRL_REG(addrmap4));
	addrmap_col_b2_11[9] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP4_ADDRMAP_COL_B11_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP4_ADDRMAP_COL_B11_SHIFT;
	addrmap_col_b2_11[8] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP4_ADDRMAP_COL_B10_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP4_ADDRMAP_COL_B10_SHIFT;

	/**
	* Row address mapping
	*/
	reg_val = al_reg_read32(CTRL_REG(addrmap5));
	addrmap_row_b0_17[11] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP5_ADDRMAP_ROW_B11_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP5_ADDRMAP_ROW_B11_SHIFT;
	addrmap_row_b0_17[2] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP5_ADDRMAP_ROW_B2_10_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP5_ADDRMAP_ROW_B2_10_SHIFT;
	for (i = 3; i < 11; i++)
		addrmap_row_b0_17[i] = addrmap_row_b0_17[2];
	addrmap_row_b0_17[1] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP5_ADDRMAP_ROW_B1_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP5_ADDRMAP_ROW_B1_SHIFT;
	addrmap_row_b0_17[0] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP5_ADDRMAP_ROW_B0_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP5_ADDRMAP_ROW_B0_SHIFT;

	reg_val = al_reg_read32(CTRL_REG(addrmap6));
	addrmap_row_b0_17[15] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP6_ADDRMAP_ROW_B15_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP6_ADDRMAP_ROW_B15_SHIFT;
	addrmap_row_b0_17[14] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP6_ADDRMAP_ROW_B14_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP6_ADDRMAP_ROW_B14_SHIFT;
	addrmap_row_b0_17[13] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP6_ADDRMAP_ROW_B13_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP6_ADDRMAP_ROW_B13_SHIFT;
	addrmap_row_b0_17[12] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP6_ADDRMAP_ROW_B12_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP6_ADDRMAP_ROW_B12_SHIFT;

	addrmap_row_b0_17[17] = AL_DDR_ADDR_MAP_ROW_DISABLED;
	addrmap_row_b0_17[16] = AL_DDR_ADDR_MAP_ROW_DISABLED;

	/* Column */
	for (i = 0; i < (AL_ARR_SIZE(addrmap->col_b3_9_b11_13) - 1); i++) {
		int user_val;

		user_val = addrmap_col_b2_11[i];

		if (data_width == AL_DDR_DATA_WIDTH_64_BITS)
			addrmap->col_b3_9_b11_13[i] =
				(user_val == AL_DDR_ADDR_MAP_COL_DISABLED) ?
				AL_DDR_ADDRMAP_NC :
				(user_val + addrmap_offset +
				AL_DDR_ADDR_MAP_COL_2_BASE + i);
		else
			addrmap->col_b3_9_b11_13[i + 1] =
				(user_val == AL_DDR_ADDR_MAP_COL_DISABLED) ?
				AL_DDR_ADDRMAP_NC :
				(user_val + addrmap_offset +
				AL_DDR_ADDR_MAP_COL_2_BASE + i);
	}

	if (data_width == AL_DDR_DATA_WIDTH_64_BITS)
		addrmap->col_b3_9_b11_13[i] = AL_DDR_ADDRMAP_NC;
	if (data_width == AL_DDR_DATA_WIDTH_32_BITS)
		addrmap->col_b3_9_b11_13[0] = 5;

	/* Bank */
	for (i = 0; i < AL_ARR_SIZE(addrmap->bank_b0_2); i++) {
		int user_val = addrmap_bank_b0_2[i];

		addrmap->bank_b0_2[i] =
			(user_val == AL_DDR_ADDR_MAP_BANK_DISABLED_ALPINE_V1) ?
			AL_DDR_ADDRMAP_NC :
			(user_val + addrmap_offset +
			AL_DDR_ADDR_MAP_BANK_0_BASE + i);
	}

	/* Bank Group */
	for (i = 0; i < AL_ARR_SIZE(addrmap->bg_b0_1); i++)
		addrmap->bg_b0_1[i] = AL_DDR_ADDRMAP_NC;

	/* CS */
	for (i = 0; i < AL_ARR_SIZE(addrmap->cs_b0_1); i++) {
		int user_val = addrmap_cs_b0_1[i];

		addrmap->cs_b0_1[i] =
			(user_val == AL_DDR_ADDR_MAP_CS_DISABLED) ?
			AL_DDR_ADDRMAP_NC :
			(user_val + addrmap_offset +
			AL_DDR_ADDR_MAP_CS_0_BASE + i);
	}

	/* CID */
	for (i = 0; i < AL_ARR_SIZE(addrmap->cid_b0_1); i++)
		addrmap->cid_b0_1[i] = AL_DDR_ADDRMAP_NC;

	/* Row */
	for (i = 0; i < AL_ARR_SIZE(addrmap->row_b0_17); i++) {
		int user_val = addrmap_row_b0_17[i];

		addrmap->row_b0_17[i] =
			(user_val == AL_DDR_ADDR_MAP_ROW_DISABLED) ?
			AL_DDR_ADDRMAP_NC :
			(user_val + addrmap_offset +
			AL_DDR_ADDR_MAP_ROW_0_BASE + i);
	}
}

/*******************************************************************************
 ******************************************************************************/
DDR_STATIC enum al_ddr_data_width _al_ddr_data_width_get(
	struct al_ddr_cfg		*ddr_cfg)
{
	union al_ddr_ctrl_regs __iomem *ctrl_regs =
		(union al_ddr_ctrl_regs __iomem *)
		ddr_cfg->ddr_ctrl_regs_base;
	uint32_t reg_val;
	enum al_ddr_data_width data_width;

	reg_val = al_reg_read32(CTRL_REG(mstr));
	reg_val &= DWC_DDR_UMCTL2_REGS_MSTR_DATA_BUS_WIDTH_MASK;

	data_width =
		(reg_val == DWC_DDR_UMCTL2_REGS_MSTR_DATA_BUS_WIDTH_64) ?
		AL_DDR_DATA_WIDTH_64_BITS :
		(reg_val == DWC_DDR_UMCTL2_REGS_MSTR_DATA_BUS_WIDTH_32) ?
		AL_DDR_DATA_WIDTH_32_BITS :
		AL_DDR_DATA_WIDTH_16_BITS;

	return data_width;
}

/*******************************************************************************
 ******************************************************************************/
DDR_STATIC int _al_ddr_address_translate_sys2dram_ext(
	struct al_ddr_cfg		*ddr_cfg,
	al_phys_addr_t			sys_address,
	struct al_ddr_addr		*ddr_addr)
{
	unsigned int i;
	unsigned int temp_rank = 0;
	unsigned int temp_bank = 0;
	unsigned int temp_bg = 0;
	unsigned int temp_col = 0;
	unsigned int temp_row = 0;
	struct al_ddr_addrmap addrmap;

	enum al_ddr_data_width data_width =
			al_ddr_data_width_get(ddr_cfg);

	al_ddr_address_map_get(ddr_cfg, &addrmap);

	if (addrmap.swap_14_16_with_17_19 == AL_TRUE) {
		/* swap sysaddr[16:14] with sysaddr[19:17] */
		AL_REG_CLEAR_AND_SET(sys_address, AL_FIELD_MASK(19, 14),
			(AL_REG_FIELD_GET(sys_address, AL_FIELD_MASK(16, 14), 14) << 17) |
			(AL_REG_FIELD_GET(sys_address, AL_FIELD_MASK(19, 17), 17) << 14));
	}

	if (data_width == AL_DDR_DATA_WIDTH_64_BITS)
		temp_col += ((sys_address >> 3) & 0x7);
	else
		temp_col += ((sys_address >> 2) & 0x7);

	for (i = 0; i < 7; i++) {
		if (addrmap.col_b3_9_b11_13[i] != AL_DDR_ADDRMAP_NC)
			temp_col +=
				(((sys_address >> addrmap.col_b3_9_b11_13[i]) & 0x1) << (i + 3));
	}
	if (addrmap.col_b3_9_b11_13[7] != AL_DDR_ADDRMAP_NC)
		temp_col += (((sys_address >> addrmap.col_b3_9_b11_13[7]) & 0x1) << 11);
	if (addrmap.col_b3_9_b11_13[8] != AL_DDR_ADDRMAP_NC)
		temp_col += (((sys_address >> addrmap.col_b3_9_b11_13[8]) & 0x1) << 13);

	for (i = 0; i < 3; i++)
		if (addrmap.bank_b0_2[i] != AL_DDR_ADDRMAP_NC)
			temp_bank += (((sys_address >> addrmap.bank_b0_2[i]) & 0x1) << i);

	for (i = 0; i < 2; i++)
		if (addrmap.bg_b0_1[i] != AL_DDR_ADDRMAP_NC)
			temp_bg += (((sys_address >> addrmap.bg_b0_1[i]) & 0x1) << i);

	for (i = 0; i < AL_ARR_SIZE(addrmap.row_b0_17); i++)
		if (addrmap.row_b0_17[i] != AL_DDR_ADDRMAP_NC)
			temp_row += (((sys_address >> addrmap.row_b0_17[i]) & 0x1) << i);

	for (i = 0; i < 2; i++)
		if (addrmap.cs_b0_1[i] != AL_DDR_ADDRMAP_NC)
			temp_rank += (((sys_address >> addrmap.cs_b0_1[i]) & 0x1) << i);

	ddr_addr->rank = temp_rank;
	ddr_addr->bank = temp_bank;
	ddr_addr->bg = temp_bg;
	ddr_addr->col = temp_col;
	ddr_addr->row = temp_row;
	ddr_addr->cid = 0;

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
DDR_STATIC int _al_ddr_address_translate_sys2dram(
	struct al_ddr_cfg		*ddr_cfg,
	al_phys_addr_t			sys_address,
	unsigned int			*rank,
	unsigned int			*bank,
	unsigned int			*bg,
	unsigned int			*col,
	unsigned int			*row)
{
	struct al_ddr_addr ddr_addr;

	_al_ddr_address_translate_sys2dram_ext(ddr_cfg, sys_address, &ddr_addr);

	*rank = ddr_addr.rank;
	*bank = ddr_addr.bank;
	*bg = ddr_addr.bg;
	*col = ddr_addr.col;
	*row = ddr_addr.row;

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
DDR_STATIC int _al_ddr_address_translate_dram2sys_ext(
	struct al_ddr_cfg		*ddr_cfg,
	al_phys_addr_t			*sys_address,
	struct al_ddr_addr		*ddr_addr)
{
	unsigned int i;
	struct al_ddr_addrmap addrmap;
	al_phys_addr_t address = 0;

	enum al_ddr_data_width data_width =
			al_ddr_data_width_get(ddr_cfg);
	al_ddr_address_map_get(ddr_cfg, &addrmap);

	if (data_width == AL_DDR_DATA_WIDTH_64_BITS)
		address += ((ddr_addr->col & 0x7) << 3);
	else
		address += ((ddr_addr->col & 0x7) << 2);

	for (i = 0; i < 7; i++) {
		if (addrmap.col_b3_9_b11_13[i] != AL_DDR_ADDRMAP_NC)
			address += ((((al_phys_addr_t)ddr_addr->col >> (i + 3)) & 0x1) <<
				addrmap.col_b3_9_b11_13[i]);
	}

	if (addrmap.col_b3_9_b11_13[7] != AL_DDR_ADDRMAP_NC)
		address += ((((al_phys_addr_t)ddr_addr->col >> 11) & 0x1) <<
			addrmap.col_b3_9_b11_13[7]);
	if (addrmap.col_b3_9_b11_13[8] != AL_DDR_ADDRMAP_NC)
		address += ((((al_phys_addr_t)ddr_addr->col >> 13) & 0x1) <<
			addrmap.col_b3_9_b11_13[8]);

	for (i = 0; i < 3; i++)
		if (addrmap.bank_b0_2[i] != AL_DDR_ADDRMAP_NC)
			address += ((((al_phys_addr_t)ddr_addr->bank >> (i)) & 0x1) <<
				addrmap.bank_b0_2[i]);

	for (i = 0; i < 2; i++)
		if (addrmap.bg_b0_1[i] != AL_DDR_ADDRMAP_NC)
			address += ((((al_phys_addr_t)ddr_addr->bg >> (i)) & 0x1) <<
				addrmap.bg_b0_1[i]);

	for (i = 0; i < AL_ARR_SIZE(addrmap.row_b0_17); i++)
		if (addrmap.row_b0_17[i] != AL_DDR_ADDRMAP_NC)
			address += ((((al_phys_addr_t)ddr_addr->row >> (i)) & 0x1) <<
				addrmap.row_b0_17[i]);

	for (i = 0; i < 2; i++)
		if (addrmap.cs_b0_1[i] != AL_DDR_ADDRMAP_NC)
			address += ((((al_phys_addr_t)ddr_addr->rank >> (i)) & 0x1) <<
				addrmap.cs_b0_1[i]);

	if (addrmap.swap_14_16_with_17_19 == AL_TRUE) {
		/* swap sysaddr[16:14] with sysaddr[19:17] */
		AL_REG_CLEAR_AND_SET(address, AL_FIELD_MASK(19, 14),
			(AL_REG_FIELD_GET(address, AL_FIELD_MASK(16, 14), 14) << 17) |
			(AL_REG_FIELD_GET(address, AL_FIELD_MASK(19, 17), 17) << 14));
	}

	*sys_address = address;

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
DDR_STATIC int _al_ddr_address_translate_dram2sys(
	struct al_ddr_cfg		*ddr_cfg,
	al_phys_addr_t			*sys_address,
	unsigned int			rank,
	unsigned int			bank,
	unsigned int			bg,
	unsigned int			col,
	unsigned int			row)
{
	struct al_ddr_addr ddr_addr = {
		.rank = rank,
		.bank = bank,
		.bg = bg,
		.col = col,
		.cid = 0,
		.row = row};


	return _al_ddr_address_translate_dram2sys_ext(ddr_cfg, sys_address, &ddr_addr);
}

/*******************************************************************************
 ******************************************************************************/
DDR_STATIC unsigned int _al_ddr_bits_per_rank_get(
	struct al_ddr_cfg		*ddr_cfg)
{
	unsigned int i, active_bits = 0;
	struct al_ddr_addrmap addrmap;

	enum al_ddr_data_width data_width =
			al_ddr_data_width_get(ddr_cfg);

	al_ddr_address_map_get(ddr_cfg, &addrmap);

	/* 64bit systems have a 6bit offset, 32bit systems have a 5bit offset */
	if (data_width == AL_DDR_DATA_WIDTH_64_BITS)
		active_bits += 6;
	else
		active_bits += 5;

	/* iterate over addrmap, count the amount of connected bits */
	for (i = 0; i < 9; i++)
		if (addrmap.col_b3_9_b11_13[i] != AL_DDR_ADDRMAP_NC)
			active_bits++;

	for (i = 0; i < 3; i++)
		if (addrmap.bank_b0_2[i] != AL_DDR_ADDRMAP_NC)
			active_bits++;

	for (i = 0; i < 2; i++)
		if (addrmap.bg_b0_1[i] != AL_DDR_ADDRMAP_NC)
			active_bits++;

	for (i = 0; i < AL_ARR_SIZE(addrmap.row_b0_17); i++)
		if (addrmap.row_b0_17[i] != AL_DDR_ADDRMAP_NC)
			active_bits++;

	return active_bits;
}

/*******************************************************************************
 ******************************************************************************/
DDR_STATIC void _al_ddr_active_byte_lanes_get(
	struct al_ddr_cfg	*ddr_cfg,
	int		active_byte_lanes[AL_DDR_PHY_NUM_BYTE_LANES])
{
	enum al_ddr_data_width ddr_data_width = _al_ddr_data_width_get(ddr_cfg);
	unsigned int val_active_byte_lanes_64_bits =
		(ddr_data_width == AL_DDR_DATA_WIDTH_64_BITS) ? 1 : 0;
	unsigned int val_active_byte_lane_ecc =
		_al_ddr_ecc_is_enabled(ddr_cfg) ? 1 : 0;
	int i;

	/**
		DRAM Data	Alpine V1
		DQ[7:0]   (LSB)	Octet#0
		DQ[15:8]	Octet#1
		DQ[23:16]	Octet#2
		DQ[31:24]	Octet#3
		DQ[39:32]	Octet#5
		DQ[47:40]	Octet#6
		DQ[55:48]	Octet#7
		DQ[63:56] (MSB)	Octet#8
		DQ[71:64] - ECC	Octet#4
	*/
	for (i = 0; i < AL_DDR_PHY_ECC_BYTE_LANE_INDEX_ALPINE_V1; i++)
		active_byte_lanes[i] = 1;

	active_byte_lanes[i++] = val_active_byte_lane_ecc;

	for (; i < AL_DDR_PHY_NUM_BYTE_LANES; i++)
		active_byte_lanes[i] = val_active_byte_lanes_64_bits;
}

/*******************************************************************************
 ******************************************************************************/
DDR_STATIC unsigned int _al_ddr_mode_register_get(
	struct al_ddr_cfg	*ddr_cfg,
	unsigned int		mr_index)
{

	STRUCT_PHY_REGS __iomem *phy_regs = PHY_REGS(ddr_cfg->ddr_phy_regs_base);

	uint32_t reg_val;

	al_assert(mr_index < 4);
	reg_val = al_reg_read32(&phy_regs->mr[mr_index]);

	return reg_val;
}

/*******************************************************************************
 ******************************************************************************/
DDR_STATIC int _al_ddr_mode_register_set(
	struct al_ddr_cfg	*ddr_cfg,
	unsigned int		ranks,
	unsigned int		mr_index,
	unsigned int		mr_value)
{
	union al_ddr_ctrl_regs __iomem *ctrl_regs =
		(union al_ddr_ctrl_regs __iomem *)
		ddr_cfg->ddr_ctrl_regs_base;
	int err;

	err = al_ddr_reg_poll32(
		CTRL_REG(mrstat),
		DWC_DDR_UMCTL2_REGS_MRSTAT_MR_WR_BUSY,
		0,
		DEFAULT_TIMEOUT);

	if (err) {
		al_err("%s: al_ddr_reg_poll32 failed!\n", __func__);
		return err;
	}

	al_reg_write32_masked(
		CTRL_REG(mrctrl0),
		DWC_DDR_UMCTL2_REGS_MRCTRL0_MR_TYPE |
		DWC_DDR_UMCTL2_REGS_MRCTRL0_MR_RANK_MASK |
		DWC_DDR_UMCTL2_REGS_MRCTRL0_MR_ADDR_MASK,
		((ranks << DWC_DDR_UMCTL2_REGS_MRCTRL0_MR_RANK_SHIFT) |
		(mr_index << DWC_DDR_UMCTL2_REGS_MRCTRL0_MR_ADDR_SHIFT) |
		DWC_DDR_UMCTL2_REGS_MRCTRL0_MR_TYPE_WRITE)
		);

	al_reg_write32(
		CTRL_REG(mrctrl1),
		(mr_value << DWC_DDR_UMCTL2_REGS_MRCTRL1_MR_DATA_SHIFT));

	/* Trigger MRS */
	al_reg_write32_masked(
		CTRL_REG(mrctrl0),
		DWC_DDR_UMCTL2_REGS_MRCTRL0_MR_WR,
		DWC_DDR_UMCTL2_REGS_MRCTRL0_MR_WR);

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
DDR_STATIC int _al_ddr_phy_zq_get(
	struct al_ddr_cfg	*ddr_cfg,
	unsigned int		zq_segment,
	enum al_ddr_phy_rout	*phy_rout,
	enum al_ddr_phy_odt	*phy_odt)
{
	STRUCT_PHY_REGS __iomem *phy_regs = PHY_REGS(ddr_cfg->ddr_phy_regs_base);

	uint32_t reg_val;

	al_assert(zq_segment < AL_DDR_PHY_NUM_ZQ_SEGMENTS_ALPINE_V1);
	reg_val = al_reg_read32(&phy_regs->ZQ[zq_segment].PR);

	switch (reg_val & DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_MASK) {
	case DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_80OHM:
		*phy_rout = AL_DDR_PHY_ROUT_80OHM;
		break;
	case DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_68OHM:
		*phy_rout = AL_DDR_PHY_ROUT_68OHM;
		break;
	case DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_60OHM:
		*phy_rout = AL_DDR_PHY_ROUT_60OHM;
		break;
	case DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_53OHM:
		*phy_rout = AL_DDR_PHY_ROUT_53OHM;
		break;
	case DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_48OHM:
		*phy_rout = AL_DDR_PHY_ROUT_48OHM;
		break;
	case DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_44OHM:
		*phy_rout = AL_DDR_PHY_ROUT_44OHM;
		break;
	case DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_40OHM:
		*phy_rout = AL_DDR_PHY_ROUT_40OHM;
		break;
	case DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_37OHM:
		*phy_rout = AL_DDR_PHY_ROUT_37OHM;
		break;
	case DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_34OHM:
		*phy_rout = AL_DDR_PHY_ROUT_34OHM;
		break;
	case DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_32OHM:
		*phy_rout = AL_DDR_PHY_ROUT_32OHM;
		break;
	case DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_30OHM:
		*phy_rout = AL_DDR_PHY_ROUT_30OHM;
		break;
	default:
		al_err("invalid PHY ROUT (%d)!\n",
			reg_val & DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_MASK);
		return -EINVAL;
	};

	switch (reg_val & DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_MASK) {
	case DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_200OHM:
		*phy_odt = AL_DDR_PHY_ODT_200OHM;
		break;
	case DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_133OHM:
		*phy_odt = AL_DDR_PHY_ODT_133OHM;
		break;
	case DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_100OHM:
		*phy_odt = AL_DDR_PHY_ODT_100OHM;
		break;
	case DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_77OHM:
		*phy_odt = AL_DDR_PHY_ODT_77OHM;
		break;
	case DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_66OHM:
		*phy_odt = AL_DDR_PHY_ODT_66OHM;
		break;
	case DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_56OHM:
		*phy_odt = AL_DDR_PHY_ODT_56OHM;
		break;
	case DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_50OHM:
		*phy_odt = AL_DDR_PHY_ODT_50OHM;
		break;
	case DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_44OHM:
		*phy_odt = AL_DDR_PHY_ODT_44OHM;
		break;
	case DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_40OHM:
		*phy_odt = AL_DDR_PHY_ODT_40OHM;
		break;
	case DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_36OHM:
		*phy_odt = AL_DDR_PHY_ODT_36OHM;
		break;
	case DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_33OHM:
		*phy_odt = AL_DDR_PHY_ODT_33OHM;
		break;
	case DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_30OHM:
		*phy_odt = AL_DDR_PHY_ODT_30OHM;
		break;
	case DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_28OHM:
		*phy_odt = AL_DDR_PHY_ODT_28OHM;
		break;
	case DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_26OHM:
		*phy_odt = AL_DDR_PHY_ODT_26OHM;
		break;
	case DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_25OHM:
		*phy_odt = AL_DDR_PHY_ODT_25OHM;
		break;
	default:
		al_err("invalid PHY ODT (%d)!\n",
			reg_val & DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_MASK);
		return -EINVAL;
	};

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
static void _al_ddr_phy_training_results_octet_get(
	struct al_ddr_cfg				*ddr_cfg,
	int						octet,
	struct al_ddr_phy_training_results_per_octet	*results)
{
	int rank;
	uint32_t reg_val;
	STRUCT_PHY_REGS __iomem *phy_regs = PHY_REGS(ddr_cfg->ddr_phy_regs_base);

	uint32_t gtr;

	gtr = al_reg_read32(&phy_regs->datx8[octet].GTR);
	reg_val = al_reg_read32(&phy_regs->datx8[octet].MDLR);
	results->dll_num_taps_init =
		(reg_val & DWC_DDR_PHY_REGS_DXNMDLR_IPRD_MASK) >>
		DWC_DDR_PHY_REGS_DXNMDLR_IPRD_SHIFT;
	results->dll_num_taps_curr =
		(reg_val & DWC_DDR_PHY_REGS_DXNMDLR_TPRD_MASK) >>
		DWC_DDR_PHY_REGS_DXNMDLR_TPRD_SHIFT;

	for (rank = 0; rank < AL_DDR_NUM_RANKS; rank++) {
		reg_val = al_reg_read32(
			&phy_regs->datx8[octet].LCDLR[0]);
		results->wld[rank] = ((reg_val >>
			(DWC_DDR_PHY_REGS_DXNLCDLR0_RNWLD_NUM_BITS * rank)) &
			DWC_DDR_PHY_REGS_DXNLCDLR0_RNWLD_MASK) >>
			DWC_DDR_PHY_REGS_DXNLCDLR0_RNWLD_SHIFT;

		results->wld_extra[rank] =
			((gtr >> (DWC_DDR_PHY_REGS_DXNGTR_RNWLSL_NUM_BITS *
			rank)) & DWC_DDR_PHY_REGS_DXNGTR_RNWLSL_MASK) >>
			DWC_DDR_PHY_REGS_DXNGTR_RNWLSL_SHIFT;

		reg_val = al_reg_read32(
			&phy_regs->datx8[octet].LCDLR[2]);
		results->rdqsgd[rank] = ((reg_val >>
			(DWC_DDR_PHY_REGS_DXNLCDLR2_RNDQSGD_NUM_BITS*rank)) &
			DWC_DDR_PHY_REGS_DXNLCDLR2_RNDQSGD_MASK) >>
			DWC_DDR_PHY_REGS_DXNLCDLR2_RNDQSGD_SHIFT;

		results->rdqsgd_extra[rank] = ((gtr >>
			(DWC_DDR_PHY_REGS_DXNGTR_RNDGSL_NUM_BITS*rank)) &
			DWC_DDR_PHY_REGS_DXNGTR_RNDGSL_MASK) >>
			DWC_DDR_PHY_REGS_DXNGTR_RNDGSL_SHIFT;
	}

	reg_val = al_reg_read32(&phy_regs->datx8[octet].LCDLR[1]);
	results->wdqd[0] = (reg_val & DWC_DDR_PHY_REGS_DXNLCDLR1_WDQD_MASK) >>
		DWC_DDR_PHY_REGS_DXNLCDLR1_WDQD_SHIFT;
	results->rdqsd[0] = (reg_val & DWC_DDR_PHY_REGS_DXNLCDLR1_RDQSD_MASK) >>
		DWC_DDR_PHY_REGS_DXNLCDLR1_RDQSD_SHIFT;
	results->rdqsnd[0] = (reg_val & DWC_DDR_PHY_REGS_DXNLCDLR1_RDQSND_MASK) >>
		DWC_DDR_PHY_REGS_DXNLCDLR1_RDQSND_SHIFT;

	al_reg_write32_masked(
		&phy_regs->DTCR,
		DWC_DDR_PHY_REGS_DTCR_DTDBS_MASK,
		DWC_DDR_PHY_REGS_DTCR_DTDBS(octet));

	reg_val = al_reg_read32(&phy_regs->DTEDR[0]);
	results->dtwlmn = (reg_val & DWC_DDR_PHY_REGS_DTEDR0_DTWLMN_MASK) >>
		DWC_DDR_PHY_REGS_DTEDR0_DTWLMN_SHIFT;
	results->dtwlmx = (reg_val & DWC_DDR_PHY_REGS_DTEDR0_DTWLMX_MASK) >>
		DWC_DDR_PHY_REGS_DTEDR0_DTWLMX_SHIFT;
	results->dtwbmn = (reg_val & DWC_DDR_PHY_REGS_DTEDR0_DTWBMN_MASK) >>
		DWC_DDR_PHY_REGS_DTEDR0_DTWBMN_SHIFT;
	results->dtwbmx = (reg_val & DWC_DDR_PHY_REGS_DTEDR0_DTWBMX_MASK) >>
		DWC_DDR_PHY_REGS_DTEDR0_DTWBMX_SHIFT;

	reg_val = al_reg_read32(&phy_regs->DTEDR[1]);
	results->dtrlmn = (reg_val & DWC_DDR_PHY_REGS_DTEDR1_DTRLMN_MASK) >>
		DWC_DDR_PHY_REGS_DTEDR1_DTRLMN_SHIFT;
	results->dtrlmx = (reg_val & DWC_DDR_PHY_REGS_DTEDR1_DTRLMX_MASK) >>
		DWC_DDR_PHY_REGS_DTEDR1_DTRLMX_SHIFT;
	results->dtrbmn = (reg_val & DWC_DDR_PHY_REGS_DTEDR1_DTRBMN_MASK) >>
		DWC_DDR_PHY_REGS_DTEDR1_DTRBMN_SHIFT;
	results->dtrbmx = (reg_val & DWC_DDR_PHY_REGS_DTEDR1_DTRBMX_MASK) >>
		DWC_DDR_PHY_REGS_DTEDR1_DTRBMX_SHIFT;
}

/*******************************************************************************
 ******************************************************************************/
DDR_STATIC void _al_ddr_phy_training_results_get(
	struct al_ddr_cfg			*ddr_cfg,
	struct al_ddr_phy_training_results	*results)
{
	int octet;

	for (octet = 0; octet < AL_DDR_PHY_NUM_BYTE_LANES; octet++) {
		_al_ddr_phy_training_results_octet_get(
			ddr_cfg, octet, &results->octets[octet]);
	}
}

/*******************************************************************************
 ******************************************************************************/
DDR_STATIC void _al_ddr_phy_training_results_print(
	struct al_ddr_cfg *ddr_cfg)
{
	struct al_ddr_phy_training_results results;
	int octet, rank;
	int active_byte_lanes[AL_DDR_PHY_NUM_BYTE_LANES];

	al_ddr_active_byte_lanes_get(
		ddr_cfg,
		active_byte_lanes);

	al_ddr_phy_training_results_get(
		ddr_cfg,
		&results);

	al_print(" ___________________________________________________________________________________________________________\n");
	al_print("|     | #Taps DLL | ---W R I T E  L E V E L I N G---               | ---R E A D   D Q S   D E L A Y---      |\n");
	al_print("|OCTET| init  now |   r3          r2          r1         r0        |  r3        r2        r1       r0       |\n");
	al_print("|-----+-----------+------------------------------------------------+----------------------------------------|\n");

	for (octet = 0; octet <= 8; octet++) {
		if (!active_byte_lanes[octet])
			continue;

		al_print("| #%d: |", octet);
		al_print("  %3d  ", results.octets[octet].dll_num_taps_init);
		al_print("%3d |", results.octets[octet].dll_num_taps_curr);

		for (rank = 3; rank >= 0; rank--) {
			al_print("%4d", results.octets[octet].wld[rank]);

			switch (results.octets[octet].wld_extra[rank]) {
			case 0:
				al_print("-CK    ");
				break;
			case 1:
				al_print("-1/2CK ");
				break;
			case 2:
				al_print("       ");
				break;
			case 3:
				al_print("+1/2CK ");
				break;
			case 4:
				al_print("+CK    ");
				break;
			case 5:
				al_print("+1.5CK ");
				break;
			case 6:
				al_print("+2CK   ");
				break;
			case 7:
				al_print("+2.5CK ");
				break;
			case 8:
				al_print("+3CK   ");
				break;
			case 9:
				al_print("+3.5CK ");
				break;
			case 10:
				al_print("+4CK   ");
				break;
			default:
				al_print("+XX    ");
				break;
			}

			al_print(" ");
		}
		al_print("|");

		for (rank = 3; rank >= 0; rank--) {
			al_print("%3d", results.octets[octet].rdqsgd[rank]);
			al_print("+%d/2CK", results.octets[octet].rdqsgd_extra[rank]);
			al_print(" ");
		}

		al_print("|\n");

		al_print("|#%dx4:|", octet);
		al_print("  %3d  ", results.octets[octet].dll_num_taps_init);
		al_print("%3d |", results.octets[octet].dll_num_taps_curr);

		for (rank = 3; rank >= 0; rank--) {
			al_print("%4d", results.octets[octet].x4wld[rank]);

			switch (results.octets[octet].x4wld_extra[rank]) {
			case 0:
				al_print("-CK    ");
				break;
			case 1:
				al_print("-1/2CK ");
				break;
			case 2:
				al_print("       ");
				break;
			case 3:
				al_print("+1/2CK ");
				break;
			case 4:
				al_print("+CK    ");
				break;
			case 5:
				al_print("+1.5CK ");
				break;
			case 6:
				al_print("+2CK   ");
				break;
			case 7:
				al_print("+2.5CK ");
				break;
			case 8:
				al_print("+3CK   ");
				break;
			case 9:
				al_print("+3.5CK ");
				break;
			case 10:
				al_print("+4CK   ");
				break;
			default:
				al_print("+XX    ");
				break;
			}

			al_print(" ");
		}
		al_print("|");

		for (rank = 3; rank >= 0; rank--) {
			al_print("%3d", results.octets[octet].x4rdqsgd[rank]);
			al_print("+%d/2CK", results.octets[octet].x4rdqsgd_extra[rank]);
			al_print(" ");
		}

		al_print("|\n");

	}

	al_print("|_____|___________|________________________________________________|________________________________________|\n");

	al_print(" _______________________________________________________________________________________________________________________________________\n");
	al_print("| OCTET | WDQD            | RDQSD           | RDQSND           | DQSGSD | WLMN | WBMN | WLMX | WBMX | RLMN | RBMN | RLMX | RBMX | RDDLY |\n");
	al_print("|       |  r0  r1  r2  r3 |  r0  r1  r2  r3 |  r0  r1  r2  r3  |                                                                        |\n");
	al_print("|-------+-----------------+-----------------+------------------+--------+------+------+------+------+------+------+------+------+-------|\n");

	for (octet = 0; octet <= 8; octet++) {
		if (!active_byte_lanes[octet])
			continue;

		al_print("| #%d:   | %3d %3d %3d %3d | %3d %3d %3d %3d | %3d %3d %3d %3d  | %3d    | %3d  | %3d  | %3d  | %3d  | %3d  | %3d  | %3d  | %3d  | %3d   |\n",
			octet,
			results.octets[octet].wdqd[0],
			results.octets[octet].wdqd[1],
			results.octets[octet].wdqd[2],
			results.octets[octet].wdqd[3],
			results.octets[octet].rdqsd[0],
			results.octets[octet].rdqsd[1],
			results.octets[octet].rdqsd[2],
			results.octets[octet].rdqsd[3],
			results.octets[octet].rdqsnd[0],
			results.octets[octet].rdqsnd[1],
			results.octets[octet].rdqsnd[2],
			results.octets[octet].rdqsnd[3],
			results.octets[octet].dqsgsd,
			results.octets[octet].dtwlmn,
			results.octets[octet].dtwbmn,
			results.octets[octet].dtwlmx,
			results.octets[octet].dtwbmx,
			results.octets[octet].dtrlmn,
			results.octets[octet].dtrbmn,
			results.octets[octet].dtrlmx,
			results.octets[octet].dtrbmx,
			results.octets[octet].rddly);

		al_print("| #%dx4: | %3d %3d %3d %3d | %3d %3d %3d %3d | %3d %3d %3d %3d  | %3d    | %3d  | %3d  | %3d  | %3d  | %3d  | %3d  | %3d  | %3d  | %3d   |\n",
			octet,
			results.octets[octet].x4wdqd[0],
			results.octets[octet].x4wdqd[1],
			results.octets[octet].x4wdqd[2],
			results.octets[octet].x4wdqd[3],
			results.octets[octet].x4rdqsd[0],
			results.octets[octet].x4rdqsd[1],
			results.octets[octet].x4rdqsd[2],
			results.octets[octet].x4rdqsd[3],
			results.octets[octet].x4rdqsnd[0],
			results.octets[octet].x4rdqsnd[1],
			results.octets[octet].x4rdqsnd[2],
			results.octets[octet].x4rdqsnd[3],
			results.octets[octet].x4dqsgsd,
			results.octets[octet].x4dtwlmn,
			results.octets[octet].x4dtwbmn,
			results.octets[octet].x4dtwlmx,
			results.octets[octet].x4dtwbmx,
			results.octets[octet].x4dtrlmn,
			results.octets[octet].x4dtrbmn,
			results.octets[octet].x4dtrlmx,
			results.octets[octet].x4dtrbmx,
			results.octets[octet].x4rddly);
	}

	al_print("|_______|_________________|_________________|__________________|________|______|______|______|______|______|______|______|______|_______|\n");

	al_print(" ________________________________________________________________________________________\n");
	al_print("| OCTET | DQWBD0 | DQWBD1 | DQWBD2 | DQWBD3 | DQWBD4 | DQWBD5 | DQWBD6 | DQWBD7 | DMWBD  |\n");
	al_print("|-------+--------+--------+--------+--------+--------+--------+--------+--------+--------|\n");

	for (octet = 0; octet <= 8; octet++) {
		if (!active_byte_lanes[octet])
			continue;

		al_print("| #%d:   | %3d    | %3d    | %3d    | %3d    | %3d    | %3d    | %3d    | %3d    | %3d    |\n",
			octet,
			results.octets[octet].dqwbd[0],
			results.octets[octet].dqwbd[1],
			results.octets[octet].dqwbd[2],
			results.octets[octet].dqwbd[3],
			results.octets[octet].dqwbd[4],
			results.octets[octet].dqwbd[5],
			results.octets[octet].dqwbd[6],
			results.octets[octet].dqwbd[7],
			results.octets[octet].dmwbd);
	}

	al_print("|_______|________|________|________|________|________|________|________|________|________|\n");

	al_print(" ________________________________________________________________________________________\n");
	al_print("| OCTET | DQRBD0 | DQRBD1 | DQRBD2 | DQRBD3 | DQRBD4 | DQRBD5 | DQRBD6 | DQRBD7 | DMRBD  |\n");
	al_print("|-------+--------+--------+--------+--------+--------+--------+--------+--------+--------|\n");

	for (octet = 0; octet <= 8; octet++) {
		if (!active_byte_lanes[octet])
			continue;

		al_print("| #%d:   | %3d    | %3d    | %3d    | %3d    | %3d    | %3d    | %3d    | %3d    | %3d    |\n",
			octet,
			results.octets[octet].dqrbd[0],
			results.octets[octet].dqrbd[1],
			results.octets[octet].dqrbd[2],
			results.octets[octet].dqrbd[3],
			results.octets[octet].dqrbd[4],
			results.octets[octet].dqrbd[5],
			results.octets[octet].dqrbd[6],
			results.octets[octet].dqrbd[7],
			results.octets[octet].dmrbd);
	}

	al_print("|_______|________|________|________|________|________|________|________|________|________|\n");

	al_print(" _________________________________________________________________________________________________________________________________________\n");
	al_print("|     | ---  DRAM VREF  --- | ---DRAM VREF MIN--- | ---DRAM VREF MAX--- | ---  HOST VREF  --- | ---HOST VREF MIN--- | ---HOST VREF MAX--- |\n");
	al_print("|OCTET| r3   r2   r1   r0   | r3   r2   r1   r0   | r3   r2   r1   r0   | r3   r2   r1   r0   | r3   r2   r1   r0   | r3   r2   r1   r0   |\n");
	al_print("|-----+---------------------+---------------------+---------------------+---------------------+---------------------+---------------------|\n");

	for (octet = 0; octet <= 8; octet++) {
		if (!active_byte_lanes[octet])
			continue;

		al_print("| #%d: |", octet);

		for (rank = 3; rank >= 0; rank--) {
			al_print("%3d", results.octets[octet].dxdqvrefr[rank]);
			al_print("  ");
		}
		al_print(" |");

		for (rank = 3; rank >= 0; rank--) {
			al_print("%3d", results.octets[octet].dvrefmn[rank]);
			al_print("  ");
		}
		al_print(" |");

		for (rank = 3; rank >= 0; rank--) {
			al_print("%3d", results.octets[octet].dvrefmx[rank]);
			al_print("  ");
		}
		al_print(" |");

		for (rank = 3; rank >= 0; rank--) {
			al_print("%3d", results.octets[octet].dxrefisel[rank]);
			al_print("  ");
		}
		al_print(" |");
		for (rank = 3; rank >= 0; rank--) {
			al_print("%3d", results.octets[octet].hvrefmn[rank]);
			al_print("  ");
		}
		al_print(" |");

		for (rank = 3; rank >= 0; rank--) {
			al_print("%3d", results.octets[octet].hvrefmx[rank]);
			al_print("  ");
		}

		al_print(" |\n");


		al_print("|#%dx4:|", octet);

		for (rank = 3; rank >= 0; rank--) {
			al_print("%3d", results.octets[octet].x4dxdqvrefr[rank]);
			al_print("  ");
		}
		al_print(" |");

		for (rank = 3; rank >= 0; rank--) {
			al_print("%3d", results.octets[octet].x4dvrefmn[rank]);
			al_print("  ");
		}
		al_print(" |");

		for (rank = 3; rank >= 0; rank--) {
			al_print("%3d", results.octets[octet].x4dvrefmx[rank]);
			al_print("  ");
		}
		al_print(" |");

		for (rank = 3; rank >= 0; rank--) {
			al_print("%3d", results.octets[octet].x4dxrefisel[rank]);
			al_print("  ");
		}
		al_print(" |");
		for (rank = 3; rank >= 0; rank--) {
			al_print("%3d", results.octets[octet].x4hvrefmn[rank]);
			al_print("  ");
		}
		al_print(" |");

		for (rank = 3; rank >= 0; rank--) {
			al_print("%3d", results.octets[octet].x4hvrefmx[rank]);
			al_print("  ");
		}

		al_print(" |\n");

	}

	al_print("|_____|_____________________|_____________________|_____________________|_____________________|_____________________|_____________________|\n");
}

/*******************************************************************************
 ******************************************************************************/
DDR_STATIC int _al_ddr_phy_vt_calc_disable(
	struct al_ddr_cfg	*ddr_cfg)
{
	STRUCT_PHY_REGS __iomem *phy_regs = PHY_REGS(ddr_cfg->ddr_phy_regs_base);
	int err;

	al_reg_write32_masked(
		&phy_regs->PGCR[1],
		DWC_DDR_PHY_REGS_PGCR1_INHVT,
		DWC_DDR_PHY_REGS_PGCR1_INHVT);

	err = al_ddr_reg_poll32(
		&phy_regs->PGSR[1],
		DWC_DDR_PHY_REGS_PGSR1_VTSTOP,
		DWC_DDR_PHY_REGS_PGSR1_VTSTOP,
		DEFAULT_TIMEOUT);
	if (err) {
		al_err("%s: al_ddr_reg_poll32 failed!\n", __func__);
		return err;
	}

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
DDR_STATIC void _al_ddr_phy_vt_calc_enable(
	struct al_ddr_cfg	*ddr_cfg)
{
	STRUCT_PHY_REGS __iomem *phy_regs = PHY_REGS(ddr_cfg->ddr_phy_regs_base);

	al_reg_write32_masked(
		&phy_regs->PGCR[1],
		DWC_DDR_PHY_REGS_PGCR1_INHVT,
		0);
}

/*******************************************************************************
 ******************************************************************************/
DDR_STATIC unsigned int _al_ddr_vpw_cfg_get(
	struct al_ddr_cfg	*ddr_cfg __attribute__((unused)))
{
	return 0;
}
#endif

#if (!defined(AL_DEV_ID))
void al_ddr_imp_alpine_v1(struct al_ddr_imp *imp);

void al_ddr_imp_alpine_v1(struct al_ddr_imp *imp)
{
	al_memset(imp, 0, sizeof(struct al_ddr_imp));

	imp->al_ddr_phy_datx_bist_pre = _al_ddr_phy_datx_bist_pre;
	imp->al_ddr_phy_datx_bist_pre_adv = _al_ddr_phy_datx_bist_pre_adv;
	imp->al_ddr_phy_datx_bist_post = _al_ddr_phy_datx_bist_post;
	imp->al_ddr_phy_datx_bist_post_adv = _al_ddr_phy_datx_bist_post_adv;
	imp->al_ddr_phy_datx_bist = _al_ddr_phy_datx_bist;
	imp->al_ddr_phy_ac_bist = _al_ddr_phy_ac_bist;
	imp->al_ddr_address_map_get = _al_ddr_address_map_get;
	imp->al_ddr_data_width_get = _al_ddr_data_width_get;
	imp->al_ddr_active_ranks_get = _al_ddr_active_ranks_get;
	imp->al_ddr_active_ranks_mask_get = _al_ddr_active_ranks_mask_get;
	imp->al_ddr_logical_rank_from_phys = _al_ddr_logical_rank_from_phys;
	imp->al_ddr_active_banks_get = _al_ddr_active_banks_get;
	imp->al_ddr_active_columns_get = _al_ddr_active_columns_get;
	imp->al_ddr_active_rows_get = _al_ddr_active_rows_get;
	imp->al_ddr_ecc_status_get = _al_ddr_ecc_status_get;
	imp->al_ddr_ecc_cfg_get = _al_ddr_ecc_cfg_get;
	imp->al_ddr_ecc_corr_count_clear = _al_ddr_ecc_corr_count_clear;
	imp->al_ddr_ecc_corr_int_clear = _al_ddr_ecc_corr_int_clear;
	imp->al_ddr_ecc_uncorr_count_clear = _al_ddr_ecc_uncorr_count_clear;
	imp->al_ddr_ecc_uncorr_int_clear = _al_ddr_ecc_uncorr_int_clear;
	imp->al_ddr_ecc_data_poison_enable = _al_ddr_ecc_data_poison_enable;
	imp->al_ddr_ecc_data_poison_disable = _al_ddr_ecc_data_poison_disable;
	imp->al_ddr_parity_count_get = _al_ddr_parity_count_get;
	imp->al_ddr_parity_count_clear = _al_ddr_parity_count_clear;
	imp->al_ddr_parity_int_clear = _al_ddr_parity_int_clear;
	imp->al_ddr_power_mode_set = _al_ddr_power_mode_set;
	imp->al_ddr_power_mode_set_reg_ptr_and_val = _al_ddr_power_mode_set_reg_ptr_and_val;
	imp->al_ddr_operating_mode_get = _al_ddr_operating_mode_get;
	imp->al_ddr_address_translate_sys2dram = _al_ddr_address_translate_sys2dram;
	imp->al_ddr_address_translate_dram2sys = _al_ddr_address_translate_dram2sys;
	imp->al_ddr_address_translate_sys2dram_ext = _al_ddr_address_translate_sys2dram_ext;
	imp->al_ddr_address_translate_dram2sys_ext = _al_ddr_address_translate_dram2sys_ext;
	imp->al_ddr_bits_per_rank_get = _al_ddr_bits_per_rank_get;
	imp->al_ddr_active_byte_lanes_get = _al_ddr_active_byte_lanes_get;
	imp->al_ddr_mode_register_get = _al_ddr_mode_register_get;
	imp->al_ddr_mode_register_set = _al_ddr_mode_register_set;
	imp->al_ddr_phy_zq_get = _al_ddr_phy_zq_get;
	imp->al_ddr_phy_training_results_get = _al_ddr_phy_training_results_get;
	imp->al_ddr_phy_training_results_print = _al_ddr_phy_training_results_print;
	imp->al_ddr_phy_vt_calc_disable = _al_ddr_phy_vt_calc_disable;
	imp->al_ddr_phy_vt_calc_enable = _al_ddr_phy_vt_calc_enable;
	imp->al_ddr_vpw_cfg_get = _al_ddr_vpw_cfg_get;
}
#endif

/** @} end of DDR group */

