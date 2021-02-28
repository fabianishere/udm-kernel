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
 *  @{
 * @file   al_hal_udma_iofic.c
 *
 * @brief  unit interrupts configurations
 *
 */

#include "al_hal_udma_iofic.h"

/*
 * configure the interrupt registers, interrupts will are kept masked
 */
static int al_udma_main_iofic_config(struct al_iofic_regs __iomem *base,
				    enum al_iofic_mode mode)
{
	switch (mode) {
	case AL_IOFIC_MODE_LEGACY:
		al_iofic_config(base, AL_INT_GROUP_A,
				INT_CONTROL_GRP_SET_ON_POSEDGE |
				INT_CONTROL_GRP_MASK_MSI_X |
				INT_CONTROL_GRP_CLEAR_ON_READ);
		al_iofic_config(base, AL_INT_GROUP_B,
				INT_CONTROL_GRP_CLEAR_ON_READ |
				INT_CONTROL_GRP_MASK_MSI_X);
		al_iofic_config(base, AL_INT_GROUP_C,
				INT_CONTROL_GRP_CLEAR_ON_READ |
				INT_CONTROL_GRP_MASK_MSI_X);
		al_iofic_config(base, AL_INT_GROUP_D,
				INT_CONTROL_GRP_SET_ON_POSEDGE |
				INT_CONTROL_GRP_MASK_MSI_X);
		break;
	case AL_IOFIC_MODE_MSIX_PER_Q:
		al_iofic_config(base, AL_INT_GROUP_A,
				INT_CONTROL_GRP_SET_ON_POSEDGE |
				INT_CONTROL_GRP_AUTO_MASK |
				INT_CONTROL_GRP_AUTO_CLEAR);
		al_iofic_config(base, AL_INT_GROUP_B,
				INT_CONTROL_GRP_AUTO_CLEAR |
				INT_CONTROL_GRP_AUTO_MASK |
				INT_CONTROL_GRP_CLEAR_ON_READ);
		al_iofic_config(base, AL_INT_GROUP_C,
				INT_CONTROL_GRP_AUTO_CLEAR |
				INT_CONTROL_GRP_AUTO_MASK |
				INT_CONTROL_GRP_CLEAR_ON_READ);
		al_iofic_config(base, AL_INT_GROUP_D,
				INT_CONTROL_GRP_SET_ON_POSEDGE |
				INT_CONTROL_GRP_MASK_MSI_X);
		break;
	case AL_IOFIC_MODE_MSIX_PER_GROUP:
		al_iofic_config(base, AL_INT_GROUP_A,
				INT_CONTROL_GRP_SET_ON_POSEDGE |
				INT_CONTROL_GRP_AUTO_CLEAR |
				INT_CONTROL_GRP_AUTO_MASK);
		al_iofic_config(base, AL_INT_GROUP_B,
				INT_CONTROL_GRP_CLEAR_ON_READ |
				INT_CONTROL_GRP_MASK_MSI_X);
		al_iofic_config(base, AL_INT_GROUP_C,
				INT_CONTROL_GRP_CLEAR_ON_READ |
				INT_CONTROL_GRP_MASK_MSI_X);
		al_iofic_config(base, AL_INT_GROUP_D,
				INT_CONTROL_GRP_SET_ON_POSEDGE |
				INT_CONTROL_GRP_MASK_MSI_X);
		break;
	default:
		al_err("%s: invalid mode (%d)\n", __func__, mode);
		return -EINVAL;
	}

	al_dbg("%s: base.%p mode %d\n", __func__, base, mode);
	return 0;
}

/*
 * configure the UDMA interrupt registers, interrupts are kept masked
 */
int al_udma_iofic_config_ex(
	struct al_udma			*udma,
	const struct al_udma_iofic_cfg	*cfg)
{
	struct udma_iofic_regs *int_regs = al_udma_gen_int_regs_get(udma);
	int rc;
	enum al_iofic_mode mode;
	uint32_t m2s_errors_disable;
	uint32_t m2s_aborts_disable;
	uint32_t s2m_errors_disable;
	uint32_t s2m_aborts_disable;
	uint32_t m2s_errors_disable_ex;
	uint32_t m2s_aborts_disable_ex;
	uint32_t s2m_errors_disable_ex;
	uint32_t s2m_aborts_disable_ex;

	al_assert(udma);
	al_assert(cfg);

	mode = cfg->mode;
	m2s_errors_disable = cfg->m2s_errors_disable;
	m2s_aborts_disable = cfg->m2s_aborts_disable;
	s2m_errors_disable = cfg->s2m_errors_disable;
	s2m_aborts_disable = cfg->s2m_aborts_disable;

	rc = al_udma_main_iofic_config(&int_regs->main_iofic, mode);
	if (rc != 0)
		return rc;

	al_iofic_unmask(&int_regs->secondary_iofic_ctrl, AL_INT_GROUP_A, ~m2s_errors_disable);
	al_iofic_abort_mask(&int_regs->secondary_iofic_ctrl, AL_INT_GROUP_A, m2s_aborts_disable);

	al_iofic_unmask(&int_regs->secondary_iofic_ctrl, AL_INT_GROUP_B, ~s2m_errors_disable);
	al_iofic_abort_mask(&int_regs->secondary_iofic_ctrl, AL_INT_GROUP_B, s2m_aborts_disable);

	if (udma->rev_id >= AL_UDMA_REV_ID_4) {
			uint32_t grp_c_errs;
			uint32_t grp_c_aborts;
			m2s_errors_disable_ex = cfg->m2s_errors_disable_ex;
			m2s_aborts_disable_ex = cfg->m2s_aborts_disable_ex;
			s2m_errors_disable_ex = cfg->s2m_errors_disable_ex;
			s2m_aborts_disable_ex = cfg->s2m_aborts_disable_ex;

			grp_c_errs = m2s_errors_disable_ex | s2m_errors_disable_ex;
			grp_c_aborts = m2s_aborts_disable_ex | s2m_aborts_disable_ex;

			/**
			 * Check m2s & s2m are not overlapping
			 * Overlap if (a & b != 0), so assert (a & b == 0).
			 */
			al_assert_msg(!(m2s_errors_disable_ex & s2m_errors_disable_ex),
				"%s: M2S (0x%x) & S2M (0x%x) errors mask overlap\n",
				__func__, m2s_errors_disable_ex, s2m_errors_disable_ex);

			al_assert_msg(!(m2s_aborts_disable_ex & s2m_aborts_disable_ex),
				"%s: M2S (0x%x) & S2M (0x%x) abort mask overlap\n",
				__func__, m2s_aborts_disable_ex, s2m_aborts_disable_ex);

		al_iofic_unmask(
			&int_regs->secondary_iofic_ctrl,
			AL_INT_GROUP_C,
			(~grp_c_errs) & AL_UDMA_V4_IOFIC_2ND_GROUP_C_ERROR_INTS);
		al_iofic_abort_mask(
			&int_regs->secondary_iofic_ctrl, AL_INT_GROUP_C,
			grp_c_aborts & AL_UDMA_V4_IOFIC_2ND_GROUP_C_ERROR_INTS);
	} else {
		/** Mask those bits so we dont count them while collecting errors */
		al_iofic_mask(
			&int_regs->secondary_iofic_ctrl,
			AL_INT_GROUP_C,
			AL_UDMA_V4_IOFIC_2ND_GROUP_C_ERROR_INTS);
	}

	al_dbg("%s int_regs %p mode %d\n", __func__, int_regs, mode);
	return 0;
}

/*
 * return the offset of the unmask register for a given group
 */
uint32_t __iomem *al_udma_iofic_unmask_offset_get_adv(
	struct al_udma			*udma,
	enum al_udma_iofic_level	level,
	int				group)
{
	al_assert(al_udma_iofic_level_and_group_valid(udma, level, group));
	return al_iofic_unmask_offset_get(al_udma_iofic_reg_base_get_adv(udma, level), group);
}

void al_udma_iofic_m2s_error_ints_unmask(
	struct al_udma			*udma)
{
	uint32_t primary_grp_mask;
	al_assert(udma);

	primary_grp_mask = AL_INT_GROUP_D_M2S;
	if (udma->rev_id >= AL_UDMA_REV_ID_4)
		primary_grp_mask |= AL_INT_UDMA_V4_GROUP_D_2ND_IOFIC_GROUP_C;

	/* config IOFIC */
	al_iofic_config(
			al_udma_iofic_reg_base_get_adv(udma,
						   AL_UDMA_IOFIC_LEVEL_SECONDARY),
			AL_INT_GROUP_A,
			INT_CONTROL_GRP_SET_ON_POSEDGE |
			INT_CONTROL_GRP_MASK_MSI_X);

	al_iofic_config(
			al_udma_iofic_reg_base_get_adv(udma,
						   AL_UDMA_IOFIC_LEVEL_PRIMARY),
			AL_INT_GROUP_D,
			INT_CONTROL_GRP_SET_ON_POSEDGE |
			INT_CONTROL_GRP_MASK_MSI_X);

	/**
	 * Addressing RMN: 10743
	 *
	 * RMN description:
	 * SSMAE Commands may end with corrupted data but with successful completion indication
	 * Software flow:
	 * HAL should set abort on error in SSMAE to avoid this scenario.
	 */
	/* abort on these interrupts */
	al_iofic_abort_mask_clear(
			al_udma_iofic_reg_base_get_adv(udma,
						   AL_UDMA_IOFIC_LEVEL_SECONDARY),
			AL_INT_GROUP_A,
			AL_UDMA_IOFIC_2ND_GROUP_A_ERROR_INTS);

	if (udma->rev_id >= AL_UDMA_REV_ID_4) {
		al_iofic_abort_mask_clear(
				al_udma_iofic_reg_base_get_adv(udma, AL_UDMA_IOFIC_LEVEL_SECONDARY),
				AL_INT_GROUP_C,
				AL_UDMA_V4_IOFIC_2ND_GROUP_C_M2S_ERROR_INTS);
	}

	/* unmask interrupts */
	al_udma_iofic_unmask_adv(udma,
			     AL_UDMA_IOFIC_LEVEL_SECONDARY,
			     AL_INT_GROUP_A,
			     AL_UDMA_IOFIC_2ND_GROUP_A_ERROR_INTS);

	if (udma->rev_id >= AL_UDMA_REV_ID_4) {
		al_udma_iofic_unmask_adv(
				udma, AL_UDMA_IOFIC_LEVEL_SECONDARY,
				AL_INT_GROUP_C,
				AL_UDMA_V4_IOFIC_2ND_GROUP_C_M2S_ERROR_INTS);
	}

	al_udma_iofic_unmask_adv(udma,
			     AL_UDMA_IOFIC_LEVEL_PRIMARY,
			     AL_INT_GROUP_D,
			     primary_grp_mask);
}

void al_udma_iofic_s2m_error_ints_unmask(
	struct al_udma			*udma)
{
	uint32_t primary_grp_mask;
	uint32_t sec_b;
	al_assert(udma);

	primary_grp_mask = AL_INT_GROUP_D_S2M;
	if (udma->rev_id >= AL_UDMA_REV_ID_4)
		primary_grp_mask |= AL_INT_UDMA_V4_GROUP_D_2ND_IOFIC_GROUP_C;

	sec_b = (udma->rev_id >= AL_UDMA_REV_ID_4) ?
		AL_UDMA_V4_IOFIC_2ND_GROUP_B_ERROR_INTS :
		AL_UDMA_IOFIC_2ND_GROUP_B_ERROR_INTS;

	/* config IOFIC */
	al_iofic_config(
			al_udma_iofic_reg_base_get_adv(udma,
						   AL_UDMA_IOFIC_LEVEL_SECONDARY),
			AL_INT_GROUP_B,
			INT_CONTROL_GRP_SET_ON_POSEDGE |
			INT_CONTROL_GRP_MASK_MSI_X);

	al_iofic_config(
			al_udma_iofic_reg_base_get_adv(udma,
						   AL_UDMA_IOFIC_LEVEL_PRIMARY),
			AL_INT_GROUP_D,
			INT_CONTROL_GRP_SET_ON_POSEDGE |
			INT_CONTROL_GRP_MASK_MSI_X);

	/* abort on these interrupts */
	al_iofic_abort_mask_clear(
			al_udma_iofic_reg_base_get_adv(udma,
						   AL_UDMA_IOFIC_LEVEL_SECONDARY),
			AL_INT_GROUP_B,
			sec_b);

	if (udma->rev_id >= AL_UDMA_REV_ID_4) {
		al_iofic_abort_mask_clear(
				al_udma_iofic_reg_base_get_adv(udma,
				AL_UDMA_IOFIC_LEVEL_SECONDARY),
				AL_INT_GROUP_C,
				AL_UDMA_V4_IOFIC_2ND_GROUP_C_S2M_ERROR_INTS);
	}

	/* unmask interrupts */
	al_udma_iofic_unmask_adv(udma,
			     AL_UDMA_IOFIC_LEVEL_SECONDARY,
			     AL_INT_GROUP_B,
			     sec_b);

	if (udma->rev_id >= AL_UDMA_REV_ID_4) {
		al_udma_iofic_unmask_adv(udma,
				     AL_UDMA_IOFIC_LEVEL_SECONDARY,
				     AL_INT_GROUP_C,
				     AL_UDMA_V4_IOFIC_2ND_GROUP_C_S2M_ERROR_INTS);
	}

	al_udma_iofic_unmask_adv(udma,
			     AL_UDMA_IOFIC_LEVEL_PRIMARY,
			     AL_INT_GROUP_D,
			     primary_grp_mask);
}

uint32_t al_udma_iofic_get_ext_app_bit(struct al_udma *udma)
{
	al_assert(udma);

	return (udma->rev_id >= AL_UDMA_REV_ID_4) ?
		AL_INT_UDMA_V4_GROUP_D_APP_EXT_INT : AL_INT_GROUP_D_APP_EXT_INT;

}

uint32_t al_udma_iofic_sec_level_int_get(struct al_udma *udma, unsigned int group)
{
	uint32_t bit = 0;
	uint32_t rev3_bits[AL_IOFIC_MAX_GROUPS] = {
		AL_INT_GROUP_D_M2S,
		AL_INT_GROUP_D_S2M,
		0,
		0,
	};
	uint32_t rev4_bits[AL_IOFIC_MAX_GROUPS] = {
		AL_INT_GROUP_D_M2S,
		AL_INT_GROUP_D_S2M,
		AL_INT_UDMA_V4_GROUP_D_2ND_IOFIC_GROUP_C,
		AL_INT_UDMA_V4_GROUP_D_2ND_IOFIC_GROUP_D,
	};

	al_assert(udma);
	al_assert(group < AL_IOFIC_MAX_GROUPS);

	bit = (udma->rev_id >= AL_UDMA_REV_ID_4) ? rev4_bits[group] : rev3_bits[group];

	al_assert(bit); /** Make sure result is not 0, if so - wrong request for this rev id */

	return bit;
}

void al_udma_iofic_unmask_ext_app(struct al_udma *udma, al_bool clear)
{
	uint32_t ext_app_bit;

	al_assert(udma);

	ext_app_bit = al_udma_iofic_get_ext_app_bit(udma);

	if (clear)
		al_udma_iofic_clear_cause_adv(udma,
			AL_UDMA_IOFIC_LEVEL_PRIMARY,
			AL_INT_GROUP_D, ext_app_bit);

	al_udma_iofic_unmask_adv(udma,
		AL_UDMA_IOFIC_LEVEL_PRIMARY,
		AL_INT_GROUP_D, ext_app_bit);
}

/** @} end of UDMA group */
