/*******************************************************************************
Copyright (C) 2016 Annapurna Labs Ltd.

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
#include "al_hal_ddr_ctrl_regs_alpine_v2.h"
#include "al_hal_ddr_phy_regs_alpine_v2.h"
#include "al_hal_nb_regs_v1_v2.h"
#include "al_hal_ddr_utils.h"
#include "al_hal_ddr_jedec_ddr4.h"
#include "al_hal_sys_fabric_utils.h"

#if (!defined(AL_DEV_ID)) || (AL_DEV_ID == AL_DEV_ID_ALPINE_V2)

#define BIST_TIMEOUT	1000	/* 1ms */

#define BIST_LFSR_RESET_VAL	0x1234ABCD

#if (defined(AL_DEV_ID))
#define DDR_STATIC
#else
#define DDR_STATIC	static
#endif

#define STRUCT_PHY_REGS			\
	struct al_dwc_ddrphy_top_dwc_ddrphy_pub_alpine_v2
#define PHY_REGS(ddr_phy_regs_base)	\
	(&((struct al_dwc_ddrphy_top_regs_alpine_v2 __iomem *)ddr_phy_regs_base)->dwc_ddrphy_pub)

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
	/*
	* Addressing RMN: 6960
	*
	* RMN description:
	* If refresh is enabled, the MBIST stop on N fail (SONF) may fail
	* to stop on nth failure
	* Software flow:
	* Disable refreshes (DTCR0.RFSHDT)
	*/
	/* Make sure PHY initiated updates were on when running BIST */
	al_assert(al_reg_read32(&phy_regs->dsgcr) &
			ALPINE_V2_DWC_DDR_PHY_REGS_DSGCR_PUREN);
	al_reg_write32_masked(
		&phy_regs->dtcr[0],
		ALPINE_V2_DWC_DDR_PHY_REGS_DTCR0_RFSHDT_MASK,
		(0x0 << ALPINE_V2_DWC_DDR_PHY_REGS_DTCR0_RFSHDT_SHIFT));
	al_reg_write32_masked(
		&phy_regs->dsgcr,
		ALPINE_V2_DWC_DDR_PHY_REGS_DSGCR_PUREN,
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
		&phy_regs->pgcr[3],
		ALPINE_V2_DWC_DDR_PHY_REGS_PGCR3_GATEDXCTLCLK |
		ALPINE_V2_DWC_DDR_PHY_REGS_PGCR3_GATEDXDDRCLK |
		ALPINE_V2_DWC_DDR_PHY_REGS_PGCR3_GATEDXRDCLK,
		ALPINE_V2_DWC_DDR_PHY_REGS_PGCR3_GATEDXCTLCLK |
		ALPINE_V2_DWC_DDR_PHY_REGS_PGCR3_GATEDXDDRCLK |
		ALPINE_V2_DWC_DDR_PHY_REGS_PGCR3_GATEDXRDCLK);

	/* re-enable refreshes from PHY */
	al_reg_write32_masked(
		&phy_regs->dtcr[0],
		ALPINE_V2_DWC_DDR_PHY_REGS_DTCR0_RFSHDT_MASK,
		(0x9 << ALPINE_V2_DWC_DDR_PHY_REGS_DTCR0_RFSHDT_SHIFT));

	/* re-enable PHY initiated updates */
	al_reg_write32_masked(
		&phy_regs->dsgcr,
		ALPINE_V2_DWC_DDR_PHY_REGS_DSGCR_PUREN,
		ALPINE_V2_DWC_DDR_PHY_REGS_DSGCR_PUREN);

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
DDR_STATIC int _al_ddr_phy_ac_bist(
	struct al_ddr_cfg		*ddr_cfg __attribute__((unused)),
	struct al_ddr_bist_err_status	*bist_err_status __attribute__((unused)),
	enum al_ddr_bist_pat		pat __attribute__((unused)))
{
	al_err("%s - Not supported for ALPINE_V2.\n", __func__);
	return -EINVAL;
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
	uint32_t reg_val;

	STRUCT_PHY_REGS __iomem *phy_regs = PHY_REGS(ddr_cfg->ddr_phy_regs_base);

	uint32_t mode;
	uint32_t pattern;

	mode =
		(params->mode == AL_DDR_BIST_MODE_LOOPBACK) ?
		ALPINE_V2_DWC_DDR_PHY_REGS_BISTRR_BMODE_LOOPBACK :
		ALPINE_V2_DWC_DDR_PHY_REGS_BISTRR_BMODE_DRAM;

	pattern =
		(params->pat == AL_DDR_BIST_PATTERN_WALK_0) ?
		ALPINE_V2_DWC_DDR_PHY_REGS_BISTRR_BDPAT_WALKING_0 :
		(params->pat == AL_DDR_BIST_PATTERN_WALK_1) ?
		ALPINE_V2_DWC_DDR_PHY_REGS_BISTRR_BDPAT_WALKING_1 :
		(params->pat == AL_DDR_BIST_PATTERN_LFSR) ?
		ALPINE_V2_DWC_DDR_PHY_REGS_BISTRR_BDPAT_LFSR :
		ALPINE_V2_DWC_DDR_PHY_REGS_BISTRR_BDPAT_USER;

	/**
	* Init BIST mode of operation
	*/

	/* BISTUDPR */
	al_reg_write32_masked(
		&phy_regs->bistudpr,
		ALPINE_V2_DWC_DDR_PHY_REGS_BISTUDPR_BUDP0_MASK |
		ALPINE_V2_DWC_DDR_PHY_REGS_BISTUDPR_BUDP1_MASK,
		ALPINE_V2_DWC_DDR_PHY_REGS_BISTUDPR_BUDP0(
			params->user_pat_even) |
		ALPINE_V2_DWC_DDR_PHY_REGS_BISTUDPR_BUDP1(
			params->user_pat_odd));

	/* BISTWCR */
	al_reg_write32_masked(
		&phy_regs->bistwcr,
		ALPINE_V2_DWC_DDR_PHY_REGS_BISTWCR_BWCNT_MASK,
		(params->wc <<
		 ALPINE_V2_DWC_DDR_PHY_REGS_BISTWCR_BWCNT_SHIFT));

	/* BISTAR0 */
	al_reg_write32_masked(
		&phy_regs->bistar[0],
		ALPINE_V2_DWC_DDR_PHY_REGS_BISTAR0_BCOL_MASK |
		ALPINE_V2_DWC_DDR_PHY_REGS_BISTAR0_BBANK_MASK,
		(params->col_min <<
		 ALPINE_V2_DWC_DDR_PHY_REGS_BISTAR0_BCOL_SHIFT) |
		(params->bank_min <<
		 ALPINE_V2_DWC_DDR_PHY_REGS_BISTAR0_BBANK_SHIFT));

	/* BISTAR1 */
	al_reg_write32_masked(
		&phy_regs->bistar[1],
		ALPINE_V2_DWC_DDR_PHY_REGS_BISTAR1_BRANK_MASK |
		ALPINE_V2_DWC_DDR_PHY_REGS_BISTAR1_BMRANK_MASK |
		ALPINE_V2_DWC_DDR_PHY_REGS_BISTAR1_BAINC_MASK,
		(params->rank_min <<
		 ALPINE_V2_DWC_DDR_PHY_REGS_BISTAR1_BRANK_SHIFT) |
		(params->rank_max <<
		 ALPINE_V2_DWC_DDR_PHY_REGS_BISTAR1_BMRANK_SHIFT) |
		(params->inc <<
		 ALPINE_V2_DWC_DDR_PHY_REGS_BISTAR1_BAINC_SHIFT));

	/* BISTAR2 */
	al_reg_write32_masked(
		&phy_regs->bistar[2],
		ALPINE_V2_DWC_DDR_PHY_REGS_BISTAR2_BMCOL_MASK |
		ALPINE_V2_DWC_DDR_PHY_REGS_BISTAR2_BMBANK_MASK,
		(params->col_max <<
		 ALPINE_V2_DWC_DDR_PHY_REGS_BISTAR2_BMCOL_SHIFT) |
		(params->bank_max <<
		 ALPINE_V2_DWC_DDR_PHY_REGS_BISTAR2_BMBANK_SHIFT));

	/* BISTAR3 */
	al_reg_write32_masked(
		&phy_regs->bistar[3],
		ALPINE_V2_DWC_DDR_PHY_REGS_BISTAR3_BROW_MASK,
		(params->row_min <<
		 ALPINE_V2_DWC_DDR_PHY_REGS_BISTAR3_BROW_SHIFT));
	/* BISTAR4 */
	al_reg_write32_masked(
		&phy_regs->bistar[4],
		ALPINE_V2_DWC_DDR_PHY_REGS_BISTAR4_BMROW_MASK,
		(params->row_max <<
		 ALPINE_V2_DWC_DDR_PHY_REGS_BISTAR4_BMROW_SHIFT));

	/* Run DATX8 BIST */
	for (i = 0; i < AL_DDR_PHY_NUM_BYTE_LANES; i++) {
		if (params->all_lanes_active && (i != 0))
			break;
		if (!params->active_byte_lanes[i] && !params->all_lanes_active)
			continue;

		/* Reinitialize LFSR seed each iteration */
		al_reg_write32(
			&phy_regs->bistlsr,
			BIST_LFSR_RESET_VAL);

		/* Reset status */
		al_reg_write32_masked(
			&phy_regs->bistrr,
			ALPINE_V2_DWC_DDR_PHY_REGS_BISTRR_BINST_MASK,
			ALPINE_V2_DWC_DDR_PHY_REGS_BISTRR_BINST_RESET);

		/* Run BIST */
		al_reg_write32_masked(
			&phy_regs->bistrr,
			ALPINE_V2_DWC_DDR_PHY_REGS_BISTRR_BINST_MASK |
			ALPINE_V2_DWC_DDR_PHY_REGS_BISTRR_BMODE |
			ALPINE_V2_DWC_DDR_PHY_REGS_BISTRR_BINF |
			ALPINE_V2_DWC_DDR_PHY_REGS_BISTRR_NFAIL_MASK |
			ALPINE_V2_DWC_DDR_PHY_REGS_BISTRR_BSONF |
			ALPINE_V2_DWC_DDR_PHY_REGS_BISTRR_BDXEN |
			ALPINE_V2_DWC_DDR_PHY_REGS_BISTRR_BACEN |
			ALPINE_V2_DWC_DDR_PHY_REGS_BISTRR_BDPAT_MASK |
			ALPINE_V2_DWC_DDR_PHY_REGS_BISTRR_BDXSEL_MASK,
			ALPINE_V2_DWC_DDR_PHY_REGS_BISTRR_BINST_RUN |
			mode |
			(params->inf ? ALPINE_V2_DWC_DDR_PHY_REGS_BISTRR_BINF : 0) |
			(params->sonf ? ALPINE_V2_DWC_DDR_PHY_REGS_BISTRR_BSONF : 0) |
			(0 << ALPINE_V2_DWC_DDR_PHY_REGS_BISTRR_NFAIL_SHIFT) |
			ALPINE_V2_DWC_DDR_PHY_REGS_BISTRR_BDXEN |
			pattern |
			(params->all_lanes_active ?
				ALPINE_V2_DWC_DDR_PHY_REGS_BISTRR_BDXSEL_MASK :
				(i << ALPINE_V2_DWC_DDR_PHY_REGS_BISTRR_BDXSEL_SHIFT)));

		al_data_memory_barrier();

		/* Read BISTGSR for BIST done */
		if (params->inf)
			while (1) {
				reg_val = al_reg_read32(&phy_regs->bistgsr);
				if (params->sonf &&
					((reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_BISTGSR_BDONE) ==
					 ALPINE_V2_DWC_DDR_PHY_REGS_BISTGSR_BDONE)) {
					al_dbg(
						"%s:%d: infinte bist run - failed "
						"(byte lane %d)!\n",
						__func__,
						__LINE__,
						i);
					break;
				}
				if (!params->sonf && (reg_val &
					(ALPINE_V2_DWC_DDR_PHY_REGS_BISTGSR_BACERR |
					 ALPINE_V2_DWC_DDR_PHY_REGS_BISTGSR_BDXERR_MASK))) {
					al_dbg(
						"%s:%d: infinte bist run - failed "
						"(byte lane %d)!\n",
						__func__,
						__LINE__,
						i);
				}
				al_udelay(1);
			}
		else
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
		&phy_regs->bistrr,
		ALPINE_V2_DWC_DDR_PHY_REGS_BISTRR_BINST_MASK,
		ALPINE_V2_DWC_DDR_PHY_REGS_BISTRR_BINST_STOP);

	al_data_memory_barrier();

	/* Waiting BIST to stop */
	err = al_ddr_phy_wait_for_bist_done(ddr_cfg, bist_err_status);
	if (err)
		al_dbg(
			"%s:%d: al_ddr_phy_wait_for_bist_done failed\n",
			__func__,
			__LINE__);

	al_reg_write32_masked(
		&phy_regs->bistrr,
		ALPINE_V2_DWC_DDR_PHY_REGS_BISTRR_BINST_MASK,
		ALPINE_V2_DWC_DDR_PHY_REGS_BISTRR_BINST_NOP);

	return err;
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

	if (power_mode == AL_DDR_POWERMODE_SELF_REFRESH) {
		reg_ptrs[reg_num] = CTRL_REG(pwrtmg);
		reg_vals[reg_num] = timer_x32 <<
			DWC_DDR_UMCTL2_REGS_PWRTMG_SELFREF_TO_X32_SHIFT;
		reg_num++;
	} else if (power_mode == AL_DDR_POWERMODE_POWER_DOWN) {
		reg_ptrs[reg_num] = CTRL_REG(pwrtmg);
		reg_vals[reg_num] = timer_x32 <<
			DWC_DDR_UMCTL2_REGS_PWRTMG_POWERDOWN_TO_X32_SHIFT;
		reg_num++;
	}

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
		(al_reg_read32(&phy_regs->bistwer[1]) &
		ALPINE_V2_DWC_DDR_PHY_REGS_BISTWER1_DXWER_MASK) >>
		ALPINE_V2_DWC_DDR_PHY_REGS_BISTWER1_DXWER_SHIFT;

	reg_val = al_reg_read32(&phy_regs->bistber[2]);
	for (i = 0; i < AL_DDR_BIST_DQ_BITS; ++i) {
		bist_err_status->even_risg_err[i] =
			(reg_val &
			ALPINE_V2_DWC_DDR_PHY_REGS_BISTBER2_DQBER0_RISG_DQ_I_MASK(i)) >>
			ALPINE_V2_DWC_DDR_PHY_REGS_BISTBER2_DQBER0_RISG_DQ_I_SHIFT(i);

		bist_err_status->even_fall_err[i] =
			(reg_val &
			ALPINE_V2_DWC_DDR_PHY_REGS_BISTBER2_DQBER0_FALL_DQ_I_MASK(i)) >>
			ALPINE_V2_DWC_DDR_PHY_REGS_BISTBER2_DQBER0_FALL_DQ_I_SHIFT(i);
	}

	for (i = 0; i < AL_DDR_BIST_DQ_BITS; ++i) {
		reg_val = al_reg_read32(&phy_regs->bistber[3]);
		bist_err_status->odd_risg_err[i] =
			(reg_val &
			ALPINE_V2_DWC_DDR_PHY_REGS_BISTBER3_DQBER1_RISG_DQ_I_MASK(i)) >>
			ALPINE_V2_DWC_DDR_PHY_REGS_BISTBER3_DQBER1_RISG_DQ_I_SHIFT(i);

		bist_err_status->odd_fall_err[i] =
			(reg_val &
			ALPINE_V2_DWC_DDR_PHY_REGS_BISTBER3_DQBER1_FALL_DQ_I_MASK(i)) >>
			ALPINE_V2_DWC_DDR_PHY_REGS_BISTBER3_DQBER1_FALL_DQ_I_SHIFT(i);
	}

	bist_err_status->word_count =
		(al_reg_read32(&phy_regs->bistwcsr) &
		ALPINE_V2_DWC_DDR_PHY_REGS_BISTWCSR_DXWCNT_MASK) >>
		ALPINE_V2_DWC_DDR_PHY_REGS_BISTWCSR_DXWCNT_SHIFT;

	reg_val = al_reg_read32(&phy_regs->bistfwr[2]);
	for (i = 0; i < AL_DDR_BIST_DQ_BITS; ++i) {
		bist_err_status->beat_1st_err[i] =
			reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_BISTFWR2_DQWEBS_1ST_DQ_I(i);

		bist_err_status->beat_2nd_err[i] =
			reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_BISTFWR2_DQWEBS_2ND_DQ_I(i);

		bist_err_status->beat_3rd_err[i] =
			reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_BISTFWR2_DQWEBS_3RD_DQ_I(i);

		bist_err_status->beat_4th_err[i] =
			reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_BISTFWR2_DQWEBS_4TH_DQ_I(i);
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
		&phy_regs->bistgsr,
		ALPINE_V2_DWC_DDR_PHY_REGS_BISTGSR_BDONE,
		ALPINE_V2_DWC_DDR_PHY_REGS_BISTGSR_BDONE,
		BIST_TIMEOUT);

	if (err) {
		int err2 = 0;
		al_reg_write32_masked(&phy_regs->pir,
				ALPINE_V2_DWC_DDR_PHY_REGS_PIR_INIT |
				ALPINE_V2_DWC_DDR_PHY_REGS_PIR_PHYRST,
				ALPINE_V2_DWC_DDR_PHY_REGS_PIR_INIT |
				ALPINE_V2_DWC_DDR_PHY_REGS_PIR_PHYRST);
		al_data_memory_barrier();
		err2 = al_ddr_reg_poll32(
			&phy_regs->pgsr[0],
			ALPINE_V2_DWC_DDR_PHY_REGS_PGSR0_IDONE,
			ALPINE_V2_DWC_DDR_PHY_REGS_PGSR0_IDONE,
			DEFAULT_TIMEOUT);
		if (err2)
			al_err("%s: al_ddr_reg_poll32 failed!\n", __func__);

		if (bist_err_status) {
			read_bist_err_status(ddr_cfg, bist_err_status);
			bist_err_status->timeout = AL_TRUE;
		}

		/* TODO : check if there are any new status registers */
		al_dbg("%s: al_ddr_reg_poll32 failed!\n", __func__);
		al_dbg("%s: PHY bist error (BISTGSR = %08x)!\n", __func__,
			al_reg_read32(&phy_regs->bistgsr));
		al_dbg("%s: (BISTWER1 = %08x)!\n", __func__,
			al_reg_read32(&phy_regs->bistwer[1]));
		al_dbg("%s: (BISTBER2 = %08x)!\n", __func__,
			al_reg_read32(&phy_regs->bistber[2]));
		al_dbg("%s: (BISTBER3 = %08x)!\n", __func__,
			al_reg_read32(&phy_regs->bistber[3]));
		al_dbg("%s: (BISTWCSR = %08x)!\n", __func__,
			al_reg_read32(&phy_regs->bistwcsr));
		al_dbg("%s: (BISTFWR2 = %08x)!\n", __func__,
			al_reg_read32(&phy_regs->bistfwr[2]));
		return err;
	}

	reg_val = al_reg_read32(&phy_regs->bistgsr);
	/* Make sure no bist errors */
	if (reg_val &
		(ALPINE_V2_DWC_DDR_PHY_REGS_BISTGSR_BACERR |
		ALPINE_V2_DWC_DDR_PHY_REGS_BISTGSR_BDXERR_MASK)) {
		if (bist_err_status) {
			read_bist_err_status(ddr_cfg, bist_err_status);
			bist_err_status->timeout = AL_FALSE;
		}
		/* TODO : check if there are any new status registers */
		al_dbg("%s: PHY bist error (BISTGSR = %08x)!\n",
			__func__, reg_val);
		al_dbg("%s: (BISTWER0 = %08x)!\n",
			__func__, al_reg_read32(
			&phy_regs->bistwer[0]));
		al_dbg("%s: (BISTBER2 = %08x)!\n",
			__func__, al_reg_read32(
			&phy_regs->bistber[2]));
		al_dbg("%s: (BISTBER3 = %08x)!\n",
			__func__, al_reg_read32(
			&phy_regs->bistber[3]));
		al_dbg("%s: (BISTWCSR = %08x)!\n",
			__func__, al_reg_read32(
			&phy_regs->bistwcsr));
		al_dbg("%s: (BISTFWR2 = %08x)!\n",
			__func__, al_reg_read32(
			&phy_regs->bistfwr[2]));
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
DDR_STATIC unsigned int _al_ddr_active_bg_get(
	struct al_ddr_cfg		*ddr_cfg)
{
	struct al_ddr_addrmap addrmap;
	unsigned int num = 0;
	unsigned int i;

	al_ddr_address_map_get(ddr_cfg, &addrmap);

	for (i = 0; i < AL_ARR_SIZE(addrmap.bg_b0_1); i++) {
		if (addrmap.bg_b0_1[i] != AL_DDR_ADDRMAP_NC)
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
	unsigned int ecc_corrected_bit_num;

	union al_ddr_ctrl_regs __iomem *ctrl_regs =
		(union al_ddr_ctrl_regs __iomem *)
		ddr_cfg->ddr_ctrl_regs_base;

	/* Correctable status */
	if (corr_status) {
		reg_val = al_reg_read32(CTRL_REG(eccerrcnt));
		corr_status->err_cnt = (reg_val &
			DWC_DDR_UMCTL2_REGS_ECCERRCNT_ECC_CORR_ERR_CNT_MASK)
			>> DWC_DDR_UMCTL2_REGS_ECCERRCNT_ECC_CORR_ERR_CNT_SHIFT;

		reg_val = al_reg_read32(CTRL_REG(eccstat));
		ecc_corrected_bit_num = (reg_val &
			DWC_DDR_UMCTL2_REGS_ECCSTAT_ECC_CORRECTED_BIT_NUM_MASK)
			>> DWC_DDR_UMCTL2_REGS_ECCSTAT_ECC_CORRECTED_BIT_NUM_SHIFT;

		if (corr_status->err_cnt)
			al_assert(ecc_corrected_bit_num < AL_ARR_SIZE(ecc_corrected_bit_map));

		corr_status->ecc_corrected_bit_num = ecc_corrected_bit_map[ecc_corrected_bit_num];

		reg_val = al_reg_read32(CTRL_REG(ecccaddr0));
		corr_status->row = (reg_val &
			DWC_DDR_UMCTL2_REGS_ECCCADDR0_ECC_CORR_ROW_MASK)
			>> DWC_DDR_UMCTL2_REGS_ECCCADDR0_ECC_CORR_ROW_SHIFT;
		corr_status->rank = (reg_val &
			DWC_DDR_UMCTL2_REGS_ECCCADDR0_ECC_CORR_RANK_MASK)
			>> DWC_DDR_UMCTL2_REGS_ECCCADDR0_ECC_CORR_RANK_SHIFT;

		reg_val = al_reg_read32(CTRL_REG(ecccaddr1));
		corr_status->bg = (reg_val &
			DWC_DDR_UMCTL2_REGS_ECCCADDR1_ECC_CORR_BG_MASK)
			>> DWC_DDR_UMCTL2_REGS_ECCCADDR1_ECC_CORR_BG_SHIFT;
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

		/* CID not relevant for alpine-v2 */
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
		uncorr_status->bg = (reg_val &
			DWC_DDR_UMCTL2_REGS_ECCUADDR1_ECC_UNCORR_BG_MASK)
			>> DWC_DDR_UMCTL2_REGS_ECCUADDR1_ECC_UNCORR_BG_SHIFT;
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

		/* CID not relevant for alpine-v2 */
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

		al_reg_write32(&nb_regs->dram_channels.ddr_int_cause
			, ~NB_DRAM_CHANNELS_DDR_INT_CAUSE_ECC_CORRECTED_ERR);
		al_reg_write32(&nb_regs->global.error_cause,
			~NB_GLOBAL_ERROR_CAUSE_DDR_CAUSE_SUM);

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

		al_reg_write32(&nb_regs->dram_channels.ddr_int_cause
			, ~NB_DRAM_CHANNELS_DDR_INT_CAUSE_ECC_UNCORRECTED_ERR);
		al_reg_write32(&nb_regs->global.error_cause,
			~NB_GLOBAL_ERROR_CAUSE_DDR_CAUSE_SUM);

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
		DWC_DDR_UMCTL2_REGS_ECCPOISONADDR1_ECC_POISON_BANK_SHIFT) |
		(bg <<
		DWC_DDR_UMCTL2_REGS_ECCPOISONADDR1_ECC_POISON_BG_SHIFT));

	/* Enable data poisoning */
	al_reg_write32(CTRL_REG(ecccfg1),
		DWC_DDR_UMCTL2_REGS_ECCCFG1_DATA_POISON_EN |
		(correctable ? DWC_DDR_UMCTL2_REGS_ECCCFG1_DATA_POISON_BIT : 0));

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

	reg_val = al_reg_read32(&ctrl_regs->alpine_v2.umctl2_regs.crcparstat);
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

	al_reg_write32_masked(&ctrl_regs->alpine_v2.umctl2_regs.crcparctl0,
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

	al_reg_write32_masked(&ctrl_regs->alpine_v2.umctl2_regs.crcparctl0,
			DWC_DDR_UMCTL2_REGS_CRCPARCTL0_DFI_ALERT_ERR_INT_CLR,
			DWC_DDR_UMCTL2_REGS_CRCPARCTL0_DFI_ALERT_ERR_INT_CLR);

	al_data_memory_barrier();

	/* TODO : check if correct for ALPINE_V2 */
	al_reg_write32(&nb_regs->global.nb_int_cause,
		~NB_GLOBAL_NB_INT_CAUSE_MCTL_PARITY_ERR);
}

/*******************************************************************************
 ******************************************************************************/
DDR_STATIC void _al_ddr_onchip_parity_status_get(
	struct al_ddr_cfg		*ddr_cfg,
	struct al_ddr_onchip_par_status	*status)
{
	uint32_t reg_val;
	union al_ddr_ctrl_regs __iomem *ctrl_regs =
		(union al_ddr_ctrl_regs __iomem *)
		ddr_cfg->ddr_ctrl_regs_base;

	reg_val = al_reg_read32(&ctrl_regs->alpine_v2.umctl2_regs.ocparstat0);
	status->raddr_err = (reg_val & DWC_DDR_UMCTL2_REGS_OCPARSTAT0_PAR_RADDR_ERR_INTR_0) ?
			AL_TRUE : AL_FALSE;
	status->waddr_err = (reg_val & DWC_DDR_UMCTL2_REGS_OCPARSTAT0_PAR_WADDR_ERR_INTR_0) ?
			AL_TRUE : AL_FALSE;

	reg_val = al_reg_read32(&ctrl_regs->alpine_v2.umctl2_regs.ocparstat1);
	status->rdata_err = (reg_val & DWC_DDR_UMCTL2_REGS_OCPARSTAT1_PAR_RDATA_ERR_INTR_0) ?
			AL_TRUE : AL_FALSE;
	status->wdata_in_err = (reg_val & DWC_DDR_UMCTL2_REGS_OCPARSTAT1_PAR_WDATA_IN_ERR_INTR_0) ?
			AL_TRUE : AL_FALSE;

	reg_val = al_reg_read32(&ctrl_regs->alpine_v2.umctl2_regs.ocparwlog0);
	status->wdata_out_err = (reg_val & DWC_DDR_UMCTL2_REGS_OCPARWLOG0_PAR_WDATA_OUT_ERR_INTR) ?
			AL_TRUE : AL_FALSE;
	status->wdata_out_rank = (reg_val &
		DWC_DDR_UMCTL2_REGS_OCPARWLOG0_PAR_WDATA_LOG_RANK_MASK)
		>> DWC_DDR_UMCTL2_REGS_OCPARWLOG0_PAR_WDATA_LOG_RANK_SHIFT;
	status->wdata_out_row = (reg_val &
		DWC_DDR_UMCTL2_REGS_OCPARWLOG0_PAR_WDATA_LOG_ROW_MASK)
		>> DWC_DDR_UMCTL2_REGS_OCPARWLOG0_PAR_WDATA_LOG_ROW_SHIFT;
	status->wdata_out_bank = (reg_val &
		DWC_DDR_UMCTL2_REGS_OCPARWLOG0_PAR_WDATA_LOG_BANK_MASK)
		>> DWC_DDR_UMCTL2_REGS_OCPARWLOG0_PAR_WDATA_LOG_BANK_SHIFT;

	reg_val = al_reg_read32(&ctrl_regs->alpine_v2.umctl2_regs.ocparwlog1);
	status->wdata_out_col = (reg_val &
		DWC_DDR_UMCTL2_REGS_OCPARWLOG1_PAR_WDATA_LOG_COL_MASK)
		>> DWC_DDR_UMCTL2_REGS_OCPARWLOG1_PAR_WDATA_LOG_COL_SHIFT;
	status->wdata_out_bg = (reg_val &
		DWC_DDR_UMCTL2_REGS_OCPARWLOG1_PAR_WDATA_LOG_BG_MASK)
		>> DWC_DDR_UMCTL2_REGS_OCPARWLOG1_PAR_WDATA_LOG_BG_SHIFT;

	status->wdata_out_byte_loc = al_reg_read32(&ctrl_regs->alpine_v2.umctl2_regs.ocparwlog2);
	status->rdata_byte_loc = al_reg_read32(&ctrl_regs->alpine_v2.umctl2_regs.ocparrlog0);

	status->raddr_address = al_reg_read32(&ctrl_regs->alpine_v2.umctl2_regs.ocpararlog0);
	reg_val = al_reg_read32(&ctrl_regs->alpine_v2.umctl2_regs.ocpararlog1);
	status->raddr_address += (((al_phys_addr_t)reg_val &
		DWC_DDR_UMCTL2_REGS_OCPARARLOG1_PAR_RADDR_LOG_HIGH_MASK)
		>> DWC_DDR_UMCTL2_REGS_OCPARARLOG1_PAR_RADDR_LOG_HIGH_SHIFT)
		<< 32;

	status->waddr_address = al_reg_read32(&ctrl_regs->alpine_v2.umctl2_regs.ocparawlog0);
	reg_val = al_reg_read32(&ctrl_regs->alpine_v2.umctl2_regs.ocparawlog1);
	status->waddr_address += (((al_phys_addr_t)reg_val &
		DWC_DDR_UMCTL2_REGS_OCPARAWLOG1_PAR_WADDR_LOG_HIGH_MASK)
		>> DWC_DDR_UMCTL2_REGS_OCPARAWLOG1_PAR_WADDR_LOG_HIGH_SHIFT)
		<< 32;
}

/*******************************************************************************
 ******************************************************************************/
#if defined(AL_DEV_ID)
DDR_STATIC void _al_ddr_onchip_parity_int_unmask(
	struct al_ddr_cfg		*ddr_cfg,
	unsigned int			local_int_idx)
{
	struct al_sys_fabric_handle handle;
	struct al_nb_regs __iomem *nb_regs;

	al_assert(ddr_cfg);
	al_assert(ddr_cfg->rev != AL_DDR_REV_ID_ALPINE_V1);

	nb_regs = (struct al_nb_regs __iomem *)ddr_cfg->nb_regs_base;

	/* Unmask DDR on chip interrupts to DDR cause summary */
	al_reg_write32(&nb_regs->dram_channels.ddr_cause_mask,
		       NB_DRAM_CHANNELS_DDR_INT_CAUSE_PAR_WDATA_OUT_ERR |
		       NB_DRAM_CHANNELS_DDR_INT_CAUSE_PAR_RADDR_ERR |
		       NB_DRAM_CHANNELS_DDR_INT_CAUSE_PAR_RDATA_ERR |
		       NB_DRAM_CHANNELS_DDR_INT_CAUSE_PAR_WADDR_ERR |
		       NB_DRAM_CHANNELS_DDR_INT_CAUSE_PAR_WDATA_IN_ERR);

	al_sys_fabric_handle_init(&handle, nb_regs);

	/* Unmask DDR summary interrupt to error cause */
	al_sys_fabric_error_int_unmask(&handle,
				       NB_GLOBAL_ERROR_CAUSE_DDR_CAUSE_SUM);

	/* Unmask error cause summary interrupt */
	al_sys_fabric_int_local_unmask(&handle,
				       local_int_idx,
				       NB_GLOBAL_NB_INT_CAUSE_ERR_CAUSE_SUM);
}
#else
DDR_STATIC void _al_ddr_onchip_parity_int_unmask(
	struct al_ddr_cfg		*ddr_cfg,
	unsigned int			local_int_idx __attribute__((unused)))
{
	al_assert(ddr_cfg);
	al_assert(ddr_cfg->rev != AL_DDR_REV_ID_ALPINE_V1);
	al_assert(0);
}
#endif

/*******************************************************************************
 ******************************************************************************/
DDR_STATIC void _al_ddr_onchip_parity_int_clear(
	struct al_ddr_cfg		*ddr_cfg)
{
	union al_ddr_ctrl_regs __iomem *ctrl_regs =
		(union al_ddr_ctrl_regs __iomem *)
		ddr_cfg->ddr_ctrl_regs_base;

	al_reg_write32_masked(&ctrl_regs->alpine_v2.umctl2_regs.ocparcfg0,
		DWC_DDR_UMCTL2_REGS_OCPARCFG0_PAR_WDATA_ERR_INTR_CLR |
		DWC_DDR_UMCTL2_REGS_OCPARCFG0_PAR_RDATA_ERR_INTR_CLR |
		DWC_DDR_UMCTL2_REGS_OCPARCFG0_PAR_WADDR_ERR_INTR_CLR |
		DWC_DDR_UMCTL2_REGS_OCPARCFG0_PAR_RADDR_ERR_INTR_CLR,
		DWC_DDR_UMCTL2_REGS_OCPARCFG0_PAR_WDATA_ERR_INTR_CLR |
		DWC_DDR_UMCTL2_REGS_OCPARCFG0_PAR_RDATA_ERR_INTR_CLR |
		DWC_DDR_UMCTL2_REGS_OCPARCFG0_PAR_WADDR_ERR_INTR_CLR |
		DWC_DDR_UMCTL2_REGS_OCPARCFG0_PAR_RADDR_ERR_INTR_CLR);

	if (ddr_cfg->nb_regs_base) {
		struct al_nb_regs __iomem *nb_regs;

		al_data_memory_barrier();

		nb_regs = (struct al_nb_regs __iomem *)ddr_cfg->nb_regs_base;

		al_reg_write32(&nb_regs->dram_channels.ddr_int_cause,
				~(NB_DRAM_CHANNELS_DDR_INT_CAUSE_PAR_WDATA_OUT_ERR |
				  NB_DRAM_CHANNELS_DDR_INT_CAUSE_PAR_RADDR_ERR |
				  NB_DRAM_CHANNELS_DDR_INT_CAUSE_PAR_RDATA_ERR |
				  NB_DRAM_CHANNELS_DDR_INT_CAUSE_PAR_WADDR_ERR |
				  NB_DRAM_CHANNELS_DDR_INT_CAUSE_PAR_WDATA_IN_ERR));
		al_reg_write32(&nb_regs->global.error_cause,
				~NB_GLOBAL_ERROR_CAUSE_DDR_CAUSE_SUM);

		al_reg_write32(&nb_regs->global.nb_int_cause,
				~NB_GLOBAL_NB_INT_CAUSE_ERR_CAUSE_SUM);
	}
}

/*******************************************************************************
 ******************************************************************************/
DDR_STATIC void _al_ddr_onchip_parity_poison_enable(
	struct al_ddr_cfg	*ddr_cfg,
	al_bool			wr_inject,
	al_bool			rd_sf_inject,
	al_bool			rd_dfi_inject,
	al_bool			continuous,
	unsigned int		byte_num,
	uint32_t		pattern0,
	uint32_t		pattern1)
{
	union al_ddr_ctrl_regs __iomem *ctrl_regs =
		(union al_ddr_ctrl_regs __iomem *)
		ddr_cfg->ddr_ctrl_regs_base;

	/* Set on chip parity poison pattern */
	al_reg_write32(&ctrl_regs->alpine_v2.umctl2_regs.ocparcfg2, pattern0);
	al_reg_write32(&ctrl_regs->alpine_v2.umctl2_regs.ocparcfg3, pattern1);

	/* Enable on chip parity poisoning */
	al_reg_write32_masked(&ctrl_regs->alpine_v2.umctl2_regs.ocparcfg1,
		DWC_DDR_UMCTL2_REGS_OCPARCFG1_PAR_POISON_EN |
		DWC_DDR_UMCTL2_REGS_OCPARCFG1_PAR_POISON_MODE |
		DWC_DDR_UMCTL2_REGS_OCPARCFG1_PAR_POISON_LOC_RD_DFI |
		DWC_DDR_UMCTL2_REGS_OCPARCFG1_PAR_POISON_LOC_RD_PORT_MASK |
		DWC_DDR_UMCTL2_REGS_OCPARCFG1_PAR_POISON_LOC_WR_PORT_MASK |
		DWC_DDR_UMCTL2_REGS_OCPARCFG1_PAR_POISON_BYTE_NUM_MASK,
		DWC_DDR_UMCTL2_REGS_OCPARCFG1_PAR_POISON_EN |
		(continuous ? 0 : DWC_DDR_UMCTL2_REGS_OCPARCFG1_PAR_POISON_MODE) |
		(rd_dfi_inject ? DWC_DDR_UMCTL2_REGS_OCPARCFG1_PAR_POISON_LOC_RD_DFI : 0) |
		(rd_sf_inject ? 0 : DWC_DDR_UMCTL2_REGS_OCPARCFG1_PAR_POISON_LOC_RD_PORT_MASK) |
		(wr_inject ? 0 : DWC_DDR_UMCTL2_REGS_OCPARCFG1_PAR_POISON_LOC_WR_PORT_MASK) |
		(byte_num << DWC_DDR_UMCTL2_REGS_OCPARCFG1_PAR_POISON_BYTE_NUM_SHIFT));

	 /* Dummy read to make sure register was written before further access to the DDR */
	 al_reg_read32(&ctrl_regs->alpine_v2.umctl2_regs.ocparcfg1);
}

/*******************************************************************************
 ******************************************************************************/
DDR_STATIC void _al_ddr_onchip_parity_poison_disable(
	struct al_ddr_cfg	*ddr_cfg)
{
	union al_ddr_ctrl_regs __iomem *ctrl_regs =
		(union al_ddr_ctrl_regs __iomem *)
		ddr_cfg->ddr_ctrl_regs_base;

	/* Disable on chip parity poisoning */
	al_reg_write32_masked(&ctrl_regs->alpine_v2.umctl2_regs.ocparcfg1,
			DWC_DDR_UMCTL2_REGS_OCPARCFG1_PAR_POISON_EN,
			0);

	/* Dummy read to make sure register was written before further access to the DDR */
	al_reg_read32(&ctrl_regs->alpine_v2.umctl2_regs.ocparcfg1);
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
	int addrmap_bg_b0_1[2];
	int addrmap_row_b0_17[18];
	int addrmap_cs_b0_1[2];

	unsigned int i;
	unsigned int addrmap_offset = AL_DDR_ADDR_MAP_OFFSET_V1_V2;

	al_assert(ddr_cfg);
	al_assert(addrmap);

	ctrl_regs = (union al_ddr_ctrl_regs __iomem *) ddr_cfg->ddr_ctrl_regs_base;
	nb_regs = (struct al_nb_regs __iomem *)ddr_cfg->nb_regs_base;
	data_width = al_ddr_data_width_get(ddr_cfg);

	addrmap->swap_14_16_with_17_19 = 0;

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
	* Bank Group address mapping
	*/
	reg_val = al_reg_read32(&ctrl_regs->alpine_v2.umctl2_regs.addrmap8);
	addrmap_bg_b0_1[1] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP8_ADDRMAP_BG_B1_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP8_ADDRMAP_BG_B1_SHIFT;
	addrmap_bg_b0_1[0] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP8_ADDRMAP_BG_B0_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP8_ADDRMAP_BG_B0_SHIFT;

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
	addrmap_row_b0_17[6] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP5_ADDRMAP_ROW_B2_10_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP5_ADDRMAP_ROW_B2_10_SHIFT;
	for (i = 7; i < 11; i++)
		addrmap_row_b0_17[i] = addrmap_row_b0_17[6];
	addrmap_row_b0_17[1] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP5_ADDRMAP_ROW_B1_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP5_ADDRMAP_ROW_B1_SHIFT;
	addrmap_row_b0_17[0] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP5_ADDRMAP_ROW_B0_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP5_ADDRMAP_ROW_B0_SHIFT;

	reg_val = al_reg_read32(&nb_regs->dram_channels.address_map);
	addrmap_row_b0_17[2] = (reg_val &
		NB_DRAM_CHANNELS_ADDRESS_MAP_ADDRMAP_ROW_B2_MASK)
		>> NB_DRAM_CHANNELS_ADDRESS_MAP_ADDRMAP_ROW_B2_SHIFT;
	addrmap_row_b0_17[3] = (reg_val &
		NB_DRAM_CHANNELS_ADDRESS_MAP_ADDRMAP_ROW_B3_MASK)
		>> NB_DRAM_CHANNELS_ADDRESS_MAP_ADDRMAP_ROW_B3_SHIFT;
	addrmap_row_b0_17[4] = (reg_val &
		NB_DRAM_CHANNELS_ADDRESS_MAP_ADDRMAP_ROW_B4_MASK)
		>> NB_DRAM_CHANNELS_ADDRESS_MAP_ADDRMAP_ROW_B4_SHIFT;
	addrmap_row_b0_17[5] = (reg_val &
		NB_DRAM_CHANNELS_ADDRESS_MAP_ADDRMAP_ROW_B5_MASK)
		>> NB_DRAM_CHANNELS_ADDRESS_MAP_ADDRMAP_ROW_B5_SHIFT;

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

	reg_val = al_reg_read32(&ctrl_regs->alpine_v2.umctl2_regs.addrmap7);
	addrmap_row_b0_17[17] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP7_ADDRMAP_ROW_B17_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP7_ADDRMAP_ROW_B17_SHIFT;
	addrmap_row_b0_17[16] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP7_ADDRMAP_ROW_B16_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP7_ADDRMAP_ROW_B16_SHIFT;

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
			(user_val == AL_DDR_ADDR_MAP_BANK_DISABLED_ALPINE_V2) ?
			AL_DDR_ADDRMAP_NC :
			(user_val + addrmap_offset +
			AL_DDR_ADDR_MAP_BANK_0_BASE + i);
	}

	/* Bank Group */
	for (i = 0; i < AL_ARR_SIZE(addrmap->bg_b0_1); i++) {
		int user_val = addrmap_bg_b0_1[i];

		addrmap->bg_b0_1[i] =
			(user_val == AL_DDR_ADDR_MAP_BG_DISABLED) ?
			AL_DDR_ADDRMAP_NC :
			(user_val + addrmap_offset +
			AL_DDR_ADDR_MAP_BG_0_BASE + i);
	}

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
	unsigned int val_active_byte_lanes_32_bits =
		(ddr_data_width == AL_DDR_DATA_WIDTH_32_BITS) ? 1 : 0;
	unsigned int val_active_byte_lane_ecc =
		_al_ddr_ecc_is_enabled(ddr_cfg) ? 1 : 0;
	int i;

	/** Lane muxing
		DRAM Data	Alpine V2	Chop-32b only
		DQ[7:0]   (LSB)	Octet#0		Octet#1
		DQ[15:8]	Octet#1		Octet#3
		DQ[23:16]	Octet#2		Octet#5
		DQ[31:24]	Octet#3		Octet#6
		DQ[39:32]	Octet#4
		DQ[47:40]	Octet#5
		DQ[55:48]	Octet#6
		DQ[63:56] (MSB)	Octet#7
		DQ[71:64] - ECC	Octet#8		Octet#4
	*/
	for (i = 0; i < AL_DDR_PHY_16BIT_BYTE_LANES_ALPINE_V2; i++)
		active_byte_lanes[i] = 1;

	for (; i < AL_DDR_PHY_32BIT_BYTE_LANES_ALPINE_V2; i++)
		active_byte_lanes[i] = val_active_byte_lanes_32_bits
					| val_active_byte_lanes_64_bits;

	for (; i < AL_DDR_PHY_ECC_BYTE_LANE_INDEX_ALPINE_V2; i++)
		active_byte_lanes[i] = val_active_byte_lanes_64_bits;

	active_byte_lanes[i++] = val_active_byte_lane_ecc;
}

/*******************************************************************************
 ******************************************************************************/
DDR_STATIC unsigned int _al_ddr_mode_register_get(
	struct al_ddr_cfg	*ddr_cfg,
	unsigned int		mr_index)
{

	STRUCT_PHY_REGS __iomem *phy_regs = PHY_REGS(ddr_cfg->ddr_phy_regs_base);

	uint32_t reg_val;

	al_assert(mr_index < 8);
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

	/*
	* Addressing RMN: 11030
	*
	* RMN description:
	* The register MRSTAT.mr_wr_busy is not cleared after
	* completing the software-driven MR access
	* Software flow:
	* Disable the sources of hardware-driven MR accesses by writing the following
	* registers before initiating the software-driven MR access.
	* PWRCTL.mpsm_en = 0
	* PWRCTL.selfref_sw = 0
	* PWRCTL.selfref_en = 0
	* HWLPCTL.hw_lp_en = 0
	*/
	uint32_t pwrctl = al_reg_read32(CTRL_REG(pwrctl));
	uint32_t hwlpctl = al_reg_read32(&ctrl_regs->alpine_v2.umctl2_regs.hwlpctl);

	al_reg_write32_masked(
		CTRL_REG(pwrctl),
		(DWC_DDR_UMCTL2_REGS_PWRCTL_MPSM_EN |
		DWC_DDR_UMCTL2_REGS_PWRCTL_SELFREF_SW |
		DWC_DDR_UMCTL2_REGS_PWRCTL_SELFREF_EN),
		0);
	al_reg_write32_masked(
		&ctrl_regs->alpine_v2.umctl2_regs.hwlpctl,
		DWC_DDR_UMCTL2_REGS_HWLPCTL_HW_LP_EN,
		0);

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

	al_reg_write32_masked(
		CTRL_REG(pwrctl),
		(DWC_DDR_UMCTL2_REGS_PWRCTL_MPSM_EN |
		DWC_DDR_UMCTL2_REGS_PWRCTL_SELFREF_SW |
		DWC_DDR_UMCTL2_REGS_PWRCTL_SELFREF_EN),
		pwrctl);
	al_reg_write32_masked(
		&ctrl_regs->alpine_v2.umctl2_regs.hwlpctl,
		DWC_DDR_UMCTL2_REGS_HWLPCTL_HW_LP_EN,
		hwlpctl);

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

	al_assert(zq_segment < AL_DDR_PHY_NUM_ZQ_SEGMENTS_ALPINE_V2);

	reg_val = al_reg_read32(&phy_regs->zq[zq_segment].pr);

	switch (reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_MASK) {
	case ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_80OHM:
		*phy_rout = AL_DDR_PHY_ROUT_80OHM;
		break;
	case ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_68OHM:
		*phy_rout = AL_DDR_PHY_ROUT_68OHM;
		break;
	case ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_60OHM:
		*phy_rout = AL_DDR_PHY_ROUT_60OHM;
		break;
	case ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_53OHM:
		*phy_rout = AL_DDR_PHY_ROUT_53OHM;
		break;
	case ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_48OHM:
		*phy_rout = AL_DDR_PHY_ROUT_48OHM;
		break;
	case ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_44OHM:
		*phy_rout = AL_DDR_PHY_ROUT_44OHM;
		break;
	case ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_40OHM:
		*phy_rout = AL_DDR_PHY_ROUT_40OHM;
		break;
	case ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_37OHM:
		*phy_rout = AL_DDR_PHY_ROUT_37OHM;
		break;
	case ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_34OHM:
		*phy_rout = AL_DDR_PHY_ROUT_34OHM;
		break;
	case ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_32OHM:
		*phy_rout = AL_DDR_PHY_ROUT_32OHM;
		break;
	case ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_30OHM:
		*phy_rout = AL_DDR_PHY_ROUT_30OHM;
		break;
	default:
		al_err("invalid PHY ROUT (%d)!\n",
			reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_MASK);
		return -EINVAL;
	};

	switch (reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_MASK) {
	case ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_200OHM:
		*phy_odt = AL_DDR_PHY_ODT_200OHM;
		break;
	case ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_133OHM:
		*phy_odt = AL_DDR_PHY_ODT_133OHM;
		break;
	case ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_100OHM:
		*phy_odt = AL_DDR_PHY_ODT_100OHM;
		break;
	case ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_77OHM:
		*phy_odt = AL_DDR_PHY_ODT_77OHM;
		break;
	case ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_66OHM:
		*phy_odt = AL_DDR_PHY_ODT_66OHM;
		break;
	case ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_56OHM:
		*phy_odt = AL_DDR_PHY_ODT_56OHM;
		break;
	case ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_50OHM:
		*phy_odt = AL_DDR_PHY_ODT_50OHM;
		break;
	case ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_44OHM:
		*phy_odt = AL_DDR_PHY_ODT_44OHM;
		break;
	case ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_40OHM:
		*phy_odt = AL_DDR_PHY_ODT_40OHM;
		break;
	case ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_36OHM:
		*phy_odt = AL_DDR_PHY_ODT_36OHM;
		break;
	case ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_33OHM:
		*phy_odt = AL_DDR_PHY_ODT_33OHM;
		break;
	case ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_30OHM:
		*phy_odt = AL_DDR_PHY_ODT_30OHM;
		break;
	case ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_28OHM:
		*phy_odt = AL_DDR_PHY_ODT_28OHM;
		break;
	case ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_26OHM:
		*phy_odt = AL_DDR_PHY_ODT_26OHM;
		break;
	case ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_25OHM:
		*phy_odt = AL_DDR_PHY_ODT_25OHM;
		break;
	default:
		al_err("invalid PHY ODT (%d)!\n",
		reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_MASK);
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
	uint32_t vtdr;

	reg_val = al_reg_read32(&phy_regs->datx8[octet].mdlr[0]);
	results->dll_num_taps_init =
		(reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNMDLR0_IPRD_MASK) >>
		ALPINE_V2_DWC_DDR_PHY_REGS_DXNMDLR0_IPRD_SHIFT;
	results->dll_num_taps_curr =
		(reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNMDLR0_TPRD_MASK) >>
		ALPINE_V2_DWC_DDR_PHY_REGS_DXNMDLR0_TPRD_SHIFT;
	for (rank = 0; rank < AL_DDR_NUM_RANKS; rank++) {
		/* Select rank */
		al_reg_write32(
			&phy_regs->rankidr,
			(rank << ALPINE_V2_DWC_DDR_PHY_REGS_RANKIDR_RANKWID_SHIFT) |
			(rank << ALPINE_V2_DWC_DDR_PHY_REGS_RANKIDR_RANKRID_SHIFT));
		al_reg_write32_masked(
			&phy_regs->dtcr[0],
			ALPINE_V2_DWC_DDR_PHY_REGS_DTCR0_DTDRS_MASK,
			(rank << ALPINE_V2_DWC_DDR_PHY_REGS_DTCR0_DTDRS_SHIFT));

		gtr = al_reg_read32(&phy_regs->datx8[octet].gtr);
		reg_val = al_reg_read32(
			&phy_regs->datx8[octet].lcdlr[0]);
		results->wld[rank] =
			(reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNLCDLR0_WLD_MASK) >>
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNLCDLR0_WLD_SHIFT;
		results->x4wld[rank] =
			(reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNLCDLR0_X4WLD_MASK) >>
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNLCDLR0_X4WLD_SHIFT;

		results->wld_extra[rank] =
			(gtr & ALPINE_V2_DWC_DDR_PHY_REGS_DXNGTR0_WLSL_MASK) >>
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNGTR0_WLSL_SHIFT;
		results->x4wld_extra[rank] =
			(gtr & ALPINE_V2_DWC_DDR_PHY_REGS_DXNGTR0_X4WLSL_MASK) >>
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNGTR0_X4WLSL_SHIFT;

		reg_val = al_reg_read32(
			&phy_regs->datx8[octet].lcdlr[2]);
		results->rdqsgd[rank] =
			(reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNLCDLR2_DQSGD_MASK) >>
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNLCDLR2_DQSGD_SHIFT;
		results->x4rdqsgd[rank] =
			(reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNLCDLR2_X4DQSGD_MASK) >>
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNLCDLR2_X4DQSGD_SHIFT;

		results->rdqsgd_extra[rank] =
			(gtr & ALPINE_V2_DWC_DDR_PHY_REGS_DXNGTR0_DGSL_MASK) >>
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNGTR0_DGSL_SHIFT;
		results->x4rdqsgd_extra[rank] =
			(gtr & ALPINE_V2_DWC_DDR_PHY_REGS_DXNGTR0_X4DGSL_MASK) >>
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNGTR0_X4DGSL_SHIFT;

		reg_val = al_reg_read32(
			&phy_regs->datx8[octet].lcdlr[1]);
		results->wdqd[rank] =
			(reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNLCDLR1_WDQD_MASK) >>
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNLCDLR1_WDQD_SHIFT;
		results->x4wdqd[rank] =
			(reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNLCDLR1_X4WDQD_MASK) >>
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNLCDLR1_X4WDQD_SHIFT;

		reg_val = al_reg_read32(
			&phy_regs->datx8[octet].lcdlr[3]);
		results->rdqsd[rank] =
			(reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNLCDLR3_RDQSD_MASK) >>
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNLCDLR3_RDQSD_SHIFT;
		results->x4rdqsd[rank] =
			(reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNLCDLR3_X4RDQSD_MASK) >>
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNLCDLR3_X4RDQSD_SHIFT;

		reg_val = al_reg_read32(
			&phy_regs->datx8[octet].lcdlr[4]);
		results->rdqsnd[rank] =
			(reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNLCDLR4_RDQSND_MASK) >>
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNLCDLR4_RDQSND_SHIFT;
		results->x4rdqsnd[rank] =
			(reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNLCDLR4_X4RDQSND_MASK) >>
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNLCDLR4_X4RDQSND_SHIFT;

		reg_val = al_reg_read32(
			&phy_regs->datx8[octet].bdlr0_2[0]);
		results->dqwbd[0] =
			(reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR0_DQ0WBD_MASK) >>
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR0_DQ0WBD_SHIFT;
		results->dqwbd[1] =
			(reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR0_DQ1WBD_MASK) >>
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR0_DQ1WBD_SHIFT;
		results->dqwbd[2] =
			(reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR0_DQ2WBD_MASK) >>
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR0_DQ2WBD_SHIFT;
		results->dqwbd[3] =
			(reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR0_DQ3WBD_MASK) >>
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR0_DQ3WBD_SHIFT;

		reg_val = al_reg_read32(
			&phy_regs->datx8[octet].bdlr0_2[1]);
		results->dqwbd[4] =
			(reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR1_DQ4WBD_MASK) >>
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR1_DQ4WBD_SHIFT;
		results->dqwbd[5] =
			(reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR1_DQ5WBD_MASK) >>
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR1_DQ5WBD_SHIFT;
		results->dqwbd[6] =
			(reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR1_DQ6WBD_MASK) >>
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR1_DQ6WBD_SHIFT;
		results->dqwbd[7] =
			(reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR1_DQ7WBD_MASK) >>
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR1_DQ7WBD_SHIFT;

		/* DM is muxed on DQS[1] pad */
		reg_val = al_reg_read32(
			&phy_regs->datx8[octet].bdlr6_9[1]);
		results->dmwbd =
			(reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR7_X4DSWBD_MASK) >>
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR7_X4DSWBD_SHIFT;


		reg_val = al_reg_read32(
			&phy_regs->datx8[octet].bdlr3_5[0]);
		results->dqrbd[0] =
			(reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR3_DQ0RBD_MASK) >>
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR3_DQ0RBD_SHIFT;
		results->dqrbd[1] =
			(reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR3_DQ1RBD_MASK) >>
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR3_DQ1RBD_SHIFT;
		results->dqrbd[2] =
			(reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR3_DQ2RBD_MASK) >>
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR3_DQ2RBD_SHIFT;
		results->dqrbd[3] =
			(reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR3_DQ3RBD_MASK) >>
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR3_DQ3RBD_SHIFT;

		reg_val = al_reg_read32(
			&phy_regs->datx8[octet].bdlr3_5[1]);
		results->dqrbd[4] =
			(reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR4_DQ4RBD_MASK) >>
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR4_DQ4RBD_SHIFT;
		results->dqrbd[5] =
			(reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR4_DQ5RBD_MASK) >>
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR4_DQ5RBD_SHIFT;
		results->dqrbd[6] =
			(reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR4_DQ6RBD_MASK) >>
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR4_DQ6RBD_SHIFT;
		results->dqrbd[7] =
			(reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR4_DQ7RBD_MASK) >>
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR4_DQ7RBD_SHIFT;

		reg_val = al_reg_read32(
			&phy_regs->datx8[octet].bdlr3_5[2]);
		results->dmrbd =
			(reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR5_DMRBD_MASK) >>
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR5_DMRBD_SHIFT;

		/* Select lower Nibble */
		al_reg_write32_masked(
			&phy_regs->dtcr[0],
			ALPINE_V2_DWC_DDR_PHY_REGS_DTCR0_DTDBS_MASK,
			ALPINE_V2_DWC_DDR_PHY_REGS_DTCR0_DTDBS(2*octet));
		vtdr = al_reg_read32(&phy_regs->vtdr);
		results->dvrefmn[rank] =
			(vtdr & ALPINE_V2_DWC_DDR_PHY_REGS_VTDR_DVREFMN_MASK) >>
			ALPINE_V2_DWC_DDR_PHY_REGS_VTDR_DVREFMN_SHIFT;
		results->dvrefmx[rank] =
			(vtdr & ALPINE_V2_DWC_DDR_PHY_REGS_VTDR_DVREFMX_MASK) >>
			ALPINE_V2_DWC_DDR_PHY_REGS_VTDR_DVREFMX_SHIFT;
		results->hvrefmn[rank] =
			(vtdr & ALPINE_V2_DWC_DDR_PHY_REGS_VTDR_HVREFMN_MASK) >>
			ALPINE_V2_DWC_DDR_PHY_REGS_VTDR_HVREFMN_SHIFT;
		results->hvrefmx[rank] =
			(vtdr & ALPINE_V2_DWC_DDR_PHY_REGS_VTDR_HVREFMX_MASK) >>
			ALPINE_V2_DWC_DDR_PHY_REGS_VTDR_HVREFMX_SHIFT;

		/* Select upper Nibble */
		al_reg_write32_masked(
			&phy_regs->dtcr[0],
			ALPINE_V2_DWC_DDR_PHY_REGS_DTCR0_DTDBS_MASK,
			ALPINE_V2_DWC_DDR_PHY_REGS_DTCR0_DTDBS(2*octet + 1));
		vtdr = al_reg_read32(&phy_regs->vtdr);
		results->x4dvrefmn[rank] =
			(vtdr & ALPINE_V2_DWC_DDR_PHY_REGS_VTDR_DVREFMN_MASK) >>
			ALPINE_V2_DWC_DDR_PHY_REGS_VTDR_DVREFMN_SHIFT;
		results->x4dvrefmx[rank] =
			(vtdr & ALPINE_V2_DWC_DDR_PHY_REGS_VTDR_DVREFMX_MASK) >>
			ALPINE_V2_DWC_DDR_PHY_REGS_VTDR_DVREFMX_SHIFT;
		results->x4hvrefmn[rank] =
			(vtdr & ALPINE_V2_DWC_DDR_PHY_REGS_VTDR_HVREFMN_MASK) >>
			ALPINE_V2_DWC_DDR_PHY_REGS_VTDR_HVREFMN_SHIFT;
		results->x4hvrefmx[rank] =
			(vtdr & ALPINE_V2_DWC_DDR_PHY_REGS_VTDR_HVREFMX_MASK) >>
			ALPINE_V2_DWC_DDR_PHY_REGS_VTDR_HVREFMX_SHIFT;

		reg_val = al_reg_read32(
			&phy_regs->datx8[octet].gcr[5]);
		results->dxrefisel[rank] = ((reg_val >>
			(8*rank)) &
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR5_DXREFISELR0_MASK) >>
			0;
		reg_val = al_reg_read32(
			&phy_regs->datx8[octet].gcr[8]);
		results->x4dxrefisel[rank] = ((reg_val >>
			(8*rank)) &
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR8_X4DXREFISELR0_MASK) >>
			0;

		reg_val = al_reg_read32(
			&phy_regs->datx8[octet].gcr[6]);
		results->dxdqvrefr[rank] = ((reg_val >>
			(8*rank)) &
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR6_DXDQVREFR0_MASK) >>
			0;
		reg_val = al_reg_read32(
			&phy_regs->datx8[octet].gcr[9]);
		results->x4dxdqvrefr[rank] = ((reg_val >>
			(8*rank)) &
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR9_X4DXDQVREFR0_MASK) >>
			0;

	}

	reg_val = al_reg_read32(
		&phy_regs->datx8[octet].lcdlr[5]);
	results->dqsgsd =
		(reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNLCDLR5_DQSGSD_MASK) >>
		ALPINE_V2_DWC_DDR_PHY_REGS_DXNLCDLR5_DQSGSD_SHIFT;
	results->x4dqsgsd =
		(reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNLCDLR5_X4DQSGSD_MASK) >>
		ALPINE_V2_DWC_DDR_PHY_REGS_DXNLCDLR5_X4DQSGSD_SHIFT;

	reg_val = al_reg_read32(&phy_regs->datx8[octet].gcr[0]);
	results->rddly = (reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR0_RDDLY_MASK) >>
		ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR0_RDDLY_SHIFT;
	reg_val = al_reg_read32(&phy_regs->datx8[octet].gcr[7]);
	results->x4rddly = (reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR7_X4RDDLY_MASK) >>
		ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR7_X4RDDLY_SHIFT;

	/* Select lower Nibble */
	al_reg_write32_masked(
		&phy_regs->dtcr[0],
		ALPINE_V2_DWC_DDR_PHY_REGS_DTCR0_DTDBS_MASK,
		ALPINE_V2_DWC_DDR_PHY_REGS_DTCR0_DTDBS(2*octet));

	reg_val = al_reg_read32(&phy_regs->dtedr[0]);
	results->dtwlmn = (reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DTEDR0_DTWLMN_MASK) >>
		ALPINE_V2_DWC_DDR_PHY_REGS_DTEDR0_DTWLMN_SHIFT;
	results->dtwlmx = (reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DTEDR0_DTWLMX_MASK) >>
		ALPINE_V2_DWC_DDR_PHY_REGS_DTEDR0_DTWLMX_SHIFT;
	results->dtwbmn = (reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DTEDR0_DTWBMN_MASK) >>
		ALPINE_V2_DWC_DDR_PHY_REGS_DTEDR0_DTWBMN_SHIFT;
	results->dtwbmx = (reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DTEDR0_DTWBMX_MASK) >>
		ALPINE_V2_DWC_DDR_PHY_REGS_DTEDR0_DTWBMX_SHIFT;

	reg_val = al_reg_read32(&phy_regs->dtedr[1]);
	results->dtrlmn = (reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DTEDR1_DTRLMN_MASK) >>
		ALPINE_V2_DWC_DDR_PHY_REGS_DTEDR1_DTRLMN_SHIFT;
	results->dtrlmx = (reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DTEDR1_DTRLMX_MASK) >>
		ALPINE_V2_DWC_DDR_PHY_REGS_DTEDR1_DTRLMX_SHIFT;
	results->dtrbmn = (reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DTEDR1_DTRBMN_MASK) >>
		ALPINE_V2_DWC_DDR_PHY_REGS_DTEDR1_DTRBMN_SHIFT;
	results->dtrbmx = (reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DTEDR1_DTRBMX_MASK) >>
		ALPINE_V2_DWC_DDR_PHY_REGS_DTEDR1_DTRBMX_SHIFT;

	/* Select upper Nibble */
	al_reg_write32_masked(
		&phy_regs->dtcr[0],
		ALPINE_V2_DWC_DDR_PHY_REGS_DTCR0_DTDBS_MASK,
		ALPINE_V2_DWC_DDR_PHY_REGS_DTCR0_DTDBS(2*octet + 1));

	reg_val = al_reg_read32(&phy_regs->dtedr[0]);
	results->x4dtwlmn = (reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DTEDR0_DTWLMN_MASK) >>
		ALPINE_V2_DWC_DDR_PHY_REGS_DTEDR0_DTWLMN_SHIFT;
	results->x4dtwlmx = (reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DTEDR0_DTWLMX_MASK) >>
		ALPINE_V2_DWC_DDR_PHY_REGS_DTEDR0_DTWLMX_SHIFT;
	results->x4dtwbmn = (reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DTEDR0_DTWBMN_MASK) >>
		ALPINE_V2_DWC_DDR_PHY_REGS_DTEDR0_DTWBMN_SHIFT;
	results->x4dtwbmx = (reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DTEDR0_DTWBMX_MASK) >>
		ALPINE_V2_DWC_DDR_PHY_REGS_DTEDR0_DTWBMX_SHIFT;

	reg_val = al_reg_read32(&phy_regs->dtedr[1]);
	results->x4dtrlmn = (reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DTEDR1_DTRLMN_MASK) >>
		ALPINE_V2_DWC_DDR_PHY_REGS_DTEDR1_DTRLMN_SHIFT;
	results->x4dtrlmx = (reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DTEDR1_DTRLMX_MASK) >>
		ALPINE_V2_DWC_DDR_PHY_REGS_DTEDR1_DTRLMX_SHIFT;
	results->x4dtrbmn = (reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DTEDR1_DTRBMN_MASK) >>
		ALPINE_V2_DWC_DDR_PHY_REGS_DTEDR1_DTRBMN_SHIFT;
	results->x4dtrbmx = (reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DTEDR1_DTRBMX_MASK) >>
		ALPINE_V2_DWC_DDR_PHY_REGS_DTEDR1_DTRBMX_SHIFT;
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
DDR_STATIC int _al_ddr_mpr_get(
	struct al_ddr_cfg	*ddr_cfg,
	unsigned int		rank_num,
	unsigned int		device_num,
	unsigned int		page_num,
	unsigned int		*page_data)
{
	union al_ddr_ctrl_regs __iomem *ctrl_regs =
		(union al_ddr_ctrl_regs __iomem *)
		ddr_cfg->ddr_ctrl_regs_base;
	struct al_nb_regs __iomem *nb_regs =
		(struct al_nb_regs __iomem *)ddr_cfg->nb_regs_base;
	int i;
	int err;
	unsigned int mr3;
	unsigned int ranks = 1 << rank_num;
	unsigned int temp_reg;
	unsigned int byte_offset = device_num & 0x3;
	unsigned int crc_parity_retry_enable;

	*page_data = 0;
	/*
	* Addressing RMN: 5006
	*
	* RMN description:
	* In ECC Configurations uMCTL2 Incorrectly Returns MRR Data on HIF Read Data
	* Software flow:
	* before performing MRR/MPR (read) through the software, disable reads and writes
	* from being issued on the DFI by setting DBG1.dis_dq = 1. After finishing MPR,
	* re-enable reads and writes being issued on the DFI by setting DBG1.dis_dq = 0.
	*/

	/* disable reads/writes */
	err = al_ddr_ctrl_read_write_disable(ddr_cfg);
	if (err) {
		al_err("%s: al_ddr_ctrl_read_write_dis failed!\n", __func__);
		return err;
	}

	/* TODO : disable DBI */

	/* TODO : should poll STAT.cmd_in_err_window if CRC retry is
	enabled and not handling CRC error currently */


	/*
	* Addressing RMN: 5247
	*
	* RMN description:
	* DDR4 Retry: Controller can Issue Precharge-all in MPR Mode
	* Software flow:
	* Disable crc_retry before entering MPR mode
	*/
	crc_parity_retry_enable =
		al_reg_read32(&ctrl_regs->alpine_v2.umctl2_regs.crcparctl1) &
		DWC_DDR_UMCTL2_REGS_CRCPARCTL1_CRC_PARITY_RETRY_ENABLE;
	al_reg_write32_masked(
		&ctrl_regs->alpine_v2.umctl2_regs.crcparctl1,
		DWC_DDR_UMCTL2_REGS_CRCPARCTL1_CRC_PARITY_RETRY_ENABLE,
		0);


	/* Enable MPR access */
	mr3 = al_ddr_mode_register_get(ddr_cfg, 3);
	AL_REG_CLEAR_AND_SET(mr3,
			(AL_DDR_JEDEC_DDR4_MR3_MPR_PAGE_SEL_MASK |
			 AL_DDR_JEDEC_DDR4_MR3_MPR_OPER_MASK |
			 AL_DDR_JEDEC_DDR4_MR3_MPR_RD_FORMAT_MASK),
			((page_num << AL_DDR_JEDEC_DDR4_MR3_MPR_PAGE_SEL_SHIFT) |
			 AL_DDR_JEDEC_DDR4_MR3_MPR_OPER_MPR |
			 AL_DDR_JEDEC_DDR4_MR3_MPR_RD_FORMAT_SERIAL));

	err = al_ddr_mode_register_set(ddr_cfg, ranks, 3, mr3);
	if (err) {
		al_err("%s: al_ddr_mode_register_set failed!\n", __func__);
		return err;
	}

	/* Read MPR page */
	for (i = 0; i < 4; i++) {
		err = al_ddr_reg_poll32(
			CTRL_REG(mrstat),
			DWC_DDR_UMCTL2_REGS_MRSTAT_MR_WR_BUSY,
			0,
			DEFAULT_TIMEOUT);

		if (err) {
			al_err("%s: al_ddr_reg_poll32 failed!\n", __func__);
			return err;
		}

		al_reg_write32(
			CTRL_REG(mrctrl0),
			((ranks << DWC_DDR_UMCTL2_REGS_MRCTRL0_MR_RANK_SHIFT) |
			(i << DWC_DDR_UMCTL2_REGS_MRCTRL0_MR_ADDR_SHIFT) |
			DWC_DDR_UMCTL2_REGS_MRCTRL0_MR_TYPE_READ |
			DWC_DDR_UMCTL2_REGS_MRCTRL0_MPR_EN));

		al_reg_write32(
			CTRL_REG(mrctrl1),
			((1 << 12) << DWC_DDR_UMCTL2_REGS_MRCTRL1_MR_DATA_SHIFT));

		/* Trigger MPR read */
		al_reg_write32_masked(
			CTRL_REG(mrctrl0),
			DWC_DDR_UMCTL2_REGS_MRCTRL0_MR_WR,
			DWC_DDR_UMCTL2_REGS_MRCTRL0_MR_WR);

		/* read data from register */
		err = al_ddr_reg_poll32(
			&nb_regs->dram_channels.mrr_control_status,
			(1 << 0),
			(1 << 0),
			DEFAULT_TIMEOUT);

		if (err) {
			al_err("%s: al_ddr_reg_poll32 failed!\n", __func__);
			return err;
		}

		temp_reg = al_reg_read32(
			&nb_regs->ddr_0_mrr[device_num >> 2].val);

		temp_reg = AL_REG_FIELD_GET(temp_reg, 0xff << (byte_offset*8), byte_offset*8);
		*page_data |= (temp_reg << 8*i);

		al_reg_write32(
			&nb_regs->dram_channels.mrr_control_status,
			(1 << 16));
		al_reg_write32(
			&nb_regs->dram_channels.mrr_control_status,
			(0 << 16));

		al_data_memory_barrier();
	}

	/* return DRAM access */
	al_reg_write32_masked(
		CTRL_REG(mrctrl0),
		DWC_DDR_UMCTL2_REGS_MRCTRL0_MPR_EN,
		0);
	AL_REG_CLEAR_AND_SET(
			mr3,
			AL_DDR_JEDEC_DDR4_MR3_MPR_OPER_MASK,
			AL_DDR_JEDEC_DDR4_MR3_MPR_OPER_NORM);
	err = al_ddr_mode_register_set(ddr_cfg, ranks, 3, mr3);
	if (err) {
		al_err("%s: al_ddr_mode_register_set failed!\n", __func__);
		return err;
	}

	al_data_memory_barrier();
	err = al_ddr_reg_poll32(
		CTRL_REG(mrstat),
		DWC_DDR_UMCTL2_REGS_MRSTAT_MR_WR_BUSY,
		0,
		DEFAULT_TIMEOUT);

	if (err) {
		al_err("%s: al_ddr_reg_poll32 failed!\n", __func__);
		return err;
	}

	/* re-enable crc retry */
	al_reg_write32_masked(
		&ctrl_regs->alpine_v2.umctl2_regs.crcparctl1,
		DWC_DDR_UMCTL2_REGS_CRCPARCTL1_CRC_PARITY_RETRY_ENABLE,
		crc_parity_retry_enable);

	/* TODO : re-enable DBI */

	/* re-enable reads/writes*/
	al_ddr_ctrl_read_write_enable(ddr_cfg);

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
DDR_STATIC int _al_ddr_phy_vt_calc_disable(
	struct al_ddr_cfg	*ddr_cfg)
{
	STRUCT_PHY_REGS __iomem *phy_regs = PHY_REGS(ddr_cfg->ddr_phy_regs_base);
	int err;

	al_reg_write32_masked(
		&phy_regs->pgcr[6],
		ALPINE_V2_DWC_DDR_PHY_REGS_PGCR6_INHVT,
		ALPINE_V2_DWC_DDR_PHY_REGS_PGCR6_INHVT);

	err = al_ddr_reg_poll32(
		&phy_regs->pgsr[1],
		ALPINE_V2_DWC_DDR_PHY_REGS_PGSR1_VTSTOP,
		ALPINE_V2_DWC_DDR_PHY_REGS_PGSR1_VTSTOP,
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
		&phy_regs->pgcr[6],
		ALPINE_V2_DWC_DDR_PHY_REGS_PGCR6_INHVT,
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
void al_ddr_imp_alpine_v2(struct al_ddr_imp *imp);

void al_ddr_imp_alpine_v2(struct al_ddr_imp *imp)
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
	imp->al_ddr_active_bg_get = _al_ddr_active_bg_get;
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
	imp->al_ddr_onchip_parity_int_unmask = _al_ddr_onchip_parity_int_unmask;
	imp->al_ddr_onchip_parity_status_get = _al_ddr_onchip_parity_status_get;
	imp->al_ddr_onchip_parity_int_clear = _al_ddr_onchip_parity_int_clear;
	imp->al_ddr_onchip_parity_poison_enable = _al_ddr_onchip_parity_poison_enable;
	imp->al_ddr_onchip_parity_poison_disable = _al_ddr_onchip_parity_poison_disable;
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
	imp->al_ddr_mpr_get = _al_ddr_mpr_get;
	imp->al_ddr_phy_vt_calc_disable = _al_ddr_phy_vt_calc_disable;
	imp->al_ddr_phy_vt_calc_enable = _al_ddr_phy_vt_calc_enable;
	imp->al_ddr_vpw_cfg_get = _al_ddr_vpw_cfg_get;
}
#endif

/** @} end of DDR group */

