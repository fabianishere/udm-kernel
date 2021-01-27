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
#include "al_hal_ddr_utils.h"
#include "al_hal_sys_fabric_utils.h"
#include "al_hal_ddr_phy_regs_alpine_v1.h"
#include "al_hal_ddrc_regs.h"

#define AL_DDRC_VERSION_ALPINE_V2	0x00020001
#define AL_DDRC_VERSION_ALPINE_V3_TC	0x00030000
#define AL_DDRC_VERSION_ALPINE_V3	0x00030001

#if (!defined(AL_DEV_ID))
extern void al_ddr_imp_alpine_v1(struct al_ddr_imp *imp);
extern void al_ddr_imp_alpine_v2(struct al_ddr_imp *imp);
extern void al_ddr_imp_alpine_v3(struct al_ddr_imp *imp);

#if 0
#define IMP(func, ...)							\
	do {								\
		if (!ddr_cfg->imp.func) {				\
			al_err("%s: not implemented!\n", __func__);	\
			return -EINVAL;					\
		}							\
									\
		return ddr_cfg->imp.func(__VA_ARGS__);			\
	} while (0)

#define IMP_NO_RET(func, ...)						\
	do {								\
		if (!ddr_cfg->imp.func) {				\
			al_err("%s: not implemented!\n", __func__);	\
			return;						\
		}							\
									\
		ddr_cfg->imp.func(__VA_ARGS__);				\
	} while (0)
#else
#define IMP(func, ...)							\
	do {								\
		if (!ddr_cfg->imp.func)					\
			return -EINVAL;					\
									\
		return ddr_cfg->imp.func(__VA_ARGS__);			\
	} while (0)

#define IMP_NO_RET(func, ...)						\
	do {								\
		if (!ddr_cfg->imp.func)					\
			return;						\
									\
		ddr_cfg->imp.func(__VA_ARGS__);				\
	} while (0)
#endif


/*******************************************************************************
 ******************************************************************************/
void al_ddr_phy_datx_bist_pre(
	struct al_ddr_cfg	*ddr_cfg)
{
	IMP_NO_RET(al_ddr_phy_datx_bist_pre, ddr_cfg);
}

/*******************************************************************************
 ******************************************************************************/
void al_ddr_phy_datx_bist_pre_adv(
	struct al_ddr_cfg	*ddr_cfg,
	al_bool			vt_calc_disable)
{
	IMP_NO_RET(al_ddr_phy_datx_bist_pre_adv, ddr_cfg, vt_calc_disable);
}

/*******************************************************************************
 ******************************************************************************/
void al_ddr_phy_datx_bist_post(
	struct al_ddr_cfg	*ddr_cfg)
{
	IMP_NO_RET(al_ddr_phy_datx_bist_post, ddr_cfg);
}

/*******************************************************************************
 ******************************************************************************/
void al_ddr_phy_datx_bist_post_adv(
	struct al_ddr_cfg	*ddr_cfg,
	al_bool			vt_calc_enable)
{
	IMP_NO_RET(al_ddr_phy_datx_bist_post_adv, ddr_cfg, vt_calc_enable);
}

/*******************************************************************************
 ******************************************************************************/
int al_ddr_phy_datx_bist(
	struct al_ddr_cfg		*ddr_cfg,
	struct al_ddr_bist_params	*params,
	struct al_ddr_bist_err_status	*bist_err_status)
{
	IMP(al_ddr_phy_datx_bist, ddr_cfg, params, bist_err_status);
}

/*******************************************************************************
 ******************************************************************************/
int al_ddr_phy_ac_bist(
	struct al_ddr_cfg		*ddr_cfg,
	struct al_ddr_bist_err_status	*bist_err_status,
	enum al_ddr_bist_pat		pat)
{
	IMP(al_ddr_phy_ac_bist, ddr_cfg, bist_err_status, pat);
}

/*******************************************************************************
 ******************************************************************************/
void al_ddr_address_map_get(
	struct al_ddr_cfg	*ddr_cfg,
	struct al_ddr_addrmap	*addrmap)
{
	IMP_NO_RET(al_ddr_address_map_get, ddr_cfg, addrmap);
}

/*******************************************************************************
 ******************************************************************************/
enum al_ddr_data_width al_ddr_data_width_get(
	struct al_ddr_cfg	*ddr_cfg)
{
	IMP(al_ddr_data_width_get, ddr_cfg);
}

/*******************************************************************************
 ******************************************************************************/
unsigned int al_ddr_active_ranks_get(
	struct al_ddr_cfg	*ddr_cfg)
{
	IMP(al_ddr_active_ranks_get, ddr_cfg);
}

/*******************************************************************************
 ******************************************************************************/
unsigned int al_ddr_active_ranks_mask_get(
	struct al_ddr_cfg	*ddr_cfg)
{
	IMP(al_ddr_active_ranks_mask_get, ddr_cfg);
}

/*******************************************************************************
 ******************************************************************************/
unsigned int al_ddr_logical_rank_from_phys(
	struct al_ddr_cfg	*ddr_cfg,
	unsigned int		rank)
{
	IMP(al_ddr_logical_rank_from_phys, ddr_cfg, rank);
}

/*******************************************************************************
 ******************************************************************************/
unsigned int al_ddr_active_banks_get(
	struct al_ddr_cfg	*ddr_cfg)
{
	IMP(al_ddr_active_banks_get, ddr_cfg);
}

/*******************************************************************************
 ******************************************************************************/
unsigned int al_ddr_active_bg_get(
	struct al_ddr_cfg	*ddr_cfg)
{
	IMP(al_ddr_active_bg_get, ddr_cfg);
}

/*******************************************************************************
 ******************************************************************************/
unsigned int al_ddr_active_columns_get(
	struct al_ddr_cfg	*ddr_cfg)
{
	IMP(al_ddr_active_columns_get, ddr_cfg);
}


/*******************************************************************************
 ******************************************************************************/
unsigned int al_ddr_active_rows_get(
	struct al_ddr_cfg	*ddr_cfg)
{
	IMP(al_ddr_active_rows_get, ddr_cfg);
}

/*******************************************************************************
 ******************************************************************************/
int al_ddr_ecc_status_get(
	struct al_ddr_cfg		*ddr_cfg,
	struct al_ddr_ecc_status	*corr_status,
	struct al_ddr_ecc_status	*uncorr_status)
{
	IMP(al_ddr_ecc_status_get, ddr_cfg, corr_status, uncorr_status);
}

/*******************************************************************************
 ******************************************************************************/
void al_ddr_ecc_cfg_get(
	struct al_ddr_cfg		*ddr_cfg,
	struct al_ddr_ecc_cfg		*ecc_cfg)
{
	IMP_NO_RET(al_ddr_ecc_cfg_get, ddr_cfg, ecc_cfg);
}

/*******************************************************************************
 ******************************************************************************/
int al_ddr_ecc_corr_count_clear(
	struct al_ddr_cfg	*ddr_cfg)
{
	IMP(al_ddr_ecc_corr_count_clear, ddr_cfg);
}

/*******************************************************************************
 ******************************************************************************/
int al_ddr_ecc_corr_int_clear(
	struct al_ddr_cfg	*ddr_cfg)
{
	IMP(al_ddr_ecc_corr_int_clear, ddr_cfg);
}


/*******************************************************************************
 ******************************************************************************/
int al_ddr_ecc_uncorr_count_clear(
	struct al_ddr_cfg	*ddr_cfg)
{
	IMP(al_ddr_ecc_uncorr_count_clear, ddr_cfg);
}

/*******************************************************************************
 ******************************************************************************/
int al_ddr_ecc_uncorr_int_clear(
	struct al_ddr_cfg	*ddr_cfg)
{
	IMP(al_ddr_ecc_uncorr_int_clear, ddr_cfg);
}

/*******************************************************************************
 ******************************************************************************/
int al_ddr_ecc_data_poison_enable(
	struct al_ddr_cfg	*ddr_cfg,
	unsigned int		rank,
	unsigned int		bank,
	unsigned int		bg,
	unsigned int		col,
	unsigned int		row,
	al_bool			correctable)
{
	IMP(al_ddr_ecc_data_poison_enable, ddr_cfg, rank, bank, bg, col, row, correctable);
}

/*******************************************************************************
 ******************************************************************************/
int al_ddr_ecc_data_poison_disable(
	struct al_ddr_cfg	*ddr_cfg)
{
	IMP(al_ddr_ecc_data_poison_disable, ddr_cfg);
}

/*******************************************************************************
 ******************************************************************************/
void al_ddr_ecc_poison_addr_get(
	struct al_ddr_cfg	*ddr_cfg,
	struct al_ddr_addr	*ddr_addr)
{
	IMP_NO_RET(al_ddr_ecc_poison_addr_get, ddr_cfg, ddr_addr);
}

/*******************************************************************************
 ******************************************************************************/
unsigned int al_ddr_parity_count_get(
	struct al_ddr_cfg	*ddr_cfg)
{
	IMP(al_ddr_parity_count_get, ddr_cfg);
}

/*******************************************************************************
 ******************************************************************************/
void al_ddr_parity_count_clear(
	struct al_ddr_cfg	*ddr_cfg)
{
	IMP_NO_RET(al_ddr_parity_count_clear, ddr_cfg);
}

/*******************************************************************************
 ******************************************************************************/
void al_ddr_parity_int_clear(
	struct al_ddr_cfg	*ddr_cfg)
{
	IMP_NO_RET(al_ddr_parity_int_clear, ddr_cfg);
}

/*******************************************************************************
 ******************************************************************************/
void al_ddr_onchip_parity_status_get(
	struct al_ddr_cfg		*ddr_cfg,
	struct al_ddr_onchip_par_status	*status)
{
	IMP_NO_RET(al_ddr_onchip_parity_status_get, ddr_cfg, status);
}

/*******************************************************************************
 ******************************************************************************/
void al_ddr_onchip_parity_int_clear(
	struct al_ddr_cfg	*ddr_cfg)
{
	IMP_NO_RET(al_ddr_onchip_parity_int_clear, ddr_cfg);
}

/*******************************************************************************
 ******************************************************************************/
void al_ddr_onchip_parity_int_unmask(
	struct al_ddr_cfg		*ddr_cfg,
	unsigned int			local_int_idx)
{
	IMP_NO_RET(al_ddr_onchip_parity_int_unmask, ddr_cfg, local_int_idx);
}

/*******************************************************************************
 ******************************************************************************/
void al_ddr_onchip_parity_poison_enable(
	struct al_ddr_cfg	*ddr_cfg,
	al_bool			wr_inject,
	al_bool			rd_sf_inject,
	al_bool			rd_dfi_inject,
	al_bool			continuous,
	unsigned int		byte_num,
	uint32_t		pattern0,
	uint32_t		pattern1)
{
	IMP_NO_RET(al_ddr_onchip_parity_poison_enable, ddr_cfg, wr_inject, rd_sf_inject, rd_dfi_inject, continuous, byte_num, pattern0, pattern1);
}

/*******************************************************************************
 ******************************************************************************/
void al_ddr_onchip_parity_poison_disable(
	struct al_ddr_cfg	*ddr_cfg)
{
	IMP_NO_RET(al_ddr_onchip_parity_poison_disable, ddr_cfg);
}

/*******************************************************************************
 ******************************************************************************/
int al_ddr_power_mode_set(
	struct al_ddr_cfg	*ddr_cfg,
	enum al_ddr_power_mode	power_mode,
	al_bool			clk_disable,
	unsigned int		timer_x32)
{
	IMP(al_ddr_power_mode_set, ddr_cfg, power_mode, clk_disable, timer_x32);
}

/*******************************************************************************
 ******************************************************************************/
enum al_ddr_operating_mode al_ddr_operating_mode_get(
	struct al_ddr_cfg	*ddr_cfg)
{
	IMP(al_ddr_operating_mode_get, ddr_cfg);
}

/*******************************************************************************
 ******************************************************************************/
int al_ddr_address_translate_sys2dram(
	struct al_ddr_cfg		*ddr_cfg,
	al_phys_addr_t			sys_address,
	unsigned int			*rank,
	unsigned int			*bank,
	unsigned int			*bg,
	unsigned int			*col,
	unsigned int			*row)
{
	IMP(al_ddr_address_translate_sys2dram, ddr_cfg, sys_address, rank, bank, bg, col, row);
}

/*******************************************************************************
 ******************************************************************************/
int al_ddr_address_translate_sys2dram_ext(
	struct al_ddr_cfg		*ddr_cfg,
	al_phys_addr_t			sys_address,
	struct al_ddr_addr		*ddr_addr)
{
	IMP(al_ddr_address_translate_sys2dram_ext, ddr_cfg, sys_address, ddr_addr);
}

/*******************************************************************************
 ******************************************************************************/
int al_ddr_address_translate_dram2sys(
	struct al_ddr_cfg		*ddr_cfg,
	al_phys_addr_t			*sys_address,
	unsigned int			rank,
	unsigned int			bank,
	unsigned int			bg,
	unsigned int			col,
	unsigned int			row)
{
	IMP(al_ddr_address_translate_dram2sys, ddr_cfg, sys_address, rank, bank, bg, col, row);
}

/*******************************************************************************
 ******************************************************************************/
int al_ddr_address_translate_dram2sys_ext(
	struct al_ddr_cfg		*ddr_cfg,
	al_phys_addr_t			*sys_address,
	struct al_ddr_addr		*ddr_addr)
{
	IMP(al_ddr_address_translate_dram2sys_ext, ddr_cfg, sys_address, ddr_addr);
}

/*******************************************************************************
 ******************************************************************************/
unsigned int al_ddr_bits_per_rank_get(
	struct al_ddr_cfg	*ddr_cfg)
{
	IMP(al_ddr_bits_per_rank_get, ddr_cfg);
}


/*******************************************************************************
 ******************************************************************************/
void al_ddr_active_byte_lanes_get(
	struct al_ddr_cfg	*ddr_cfg,
	int		active_byte_lanes[AL_DDR_PHY_NUM_BYTE_LANES])
{
	IMP_NO_RET(al_ddr_active_byte_lanes_get, ddr_cfg, active_byte_lanes);
}

/*******************************************************************************
 ******************************************************************************/
unsigned int al_ddr_mode_register_get(
	struct al_ddr_cfg	*ddr_cfg,
	unsigned int		mr_index)
{
	IMP(al_ddr_mode_register_get, ddr_cfg, mr_index);
}

/*******************************************************************************
 ******************************************************************************/
int al_ddr_mode_register_set(
	struct al_ddr_cfg	*ddr_cfg,
	unsigned int		ranks,
	unsigned int		mr_index,
	unsigned int		mr_value)
{
	IMP(al_ddr_mode_register_set, ddr_cfg, ranks, mr_index, mr_value);
}

/*******************************************************************************
 ******************************************************************************/
int al_ddr_phy_zq_get(
	struct al_ddr_cfg	*ddr_cfg,
	unsigned int		zq_segment,
	enum al_ddr_phy_rout	*phy_rout,
	enum al_ddr_phy_odt	*phy_odt)
{
	IMP(al_ddr_phy_zq_get, ddr_cfg, zq_segment, phy_rout, phy_odt);
}

/*******************************************************************************
 ******************************************************************************/
void al_ddr_phy_training_results_get(
	struct al_ddr_cfg			*ddr_cfg,
	struct al_ddr_phy_training_results	*results)
{
	IMP_NO_RET(al_ddr_phy_training_results_get, ddr_cfg, results);
}

/*******************************************************************************
 ******************************************************************************/
void al_ddr_phy_training_results_print(
	struct al_ddr_cfg *ddr_cfg)
{
	IMP_NO_RET(al_ddr_phy_training_results_print, ddr_cfg);
}

/*******************************************************************************
 ******************************************************************************/
int al_ddr_mpr_get(
	struct al_ddr_cfg	*ddr_cfg,
	unsigned int		rank_num,
	unsigned int		device_num,
	unsigned int		page_num,
	unsigned int		*page_data)
{
	IMP(al_ddr_mpr_get, ddr_cfg, rank_num, device_num, page_num, page_data);
}

int al_ddr_phy_vt_calc_disable(
	struct al_ddr_cfg	*ddr_cfg)
{
	IMP(al_ddr_phy_vt_calc_disable, ddr_cfg);
}

void al_ddr_phy_vt_calc_enable(
	struct al_ddr_cfg	*ddr_cfg)
{
	IMP_NO_RET(al_ddr_phy_vt_calc_enable, ddr_cfg);
}

void al_ddr_pmdt_pmu_event_group_sel(
	struct al_ddr_cfg		*ddr_cfg,
	enum al_ddr_pmdt_event_group	grp)
{
	IMP_NO_RET(al_ddr_pmdt_pmu_event_group_sel, ddr_cfg, grp);
}

int al_ddr_pmdt_event_bank_sel(
	struct al_ddr_cfg		*ddr_cfg,
	struct al_ddr_pmu_perf_sel	*pmu_perf_sel)
{
	IMP(al_ddr_pmdt_event_bank_sel, ddr_cfg, pmu_perf_sel);
}
#endif

/*******************************************************************************
 ******************************************************************************/
static int al_ddr_rev_get(
	void __iomem		*ddr_phy_regs_base,
	void __iomem		*ddrc_regs_base,
	enum al_ddr_revision	*rev)
{
#if (!defined(AL_DDR_PHY_DISABLED) || (AL_DDR_PHY_DISABLED == 0))
	struct al_dwc_ddrphy_top_regs_alpine_v1 __iomem *phy_regs;
	struct al_ddrc_regs __iomem *ddrc_regs;
	uint32_t phy_version;
	uint32_t ddrc_version;

	if (!ddrc_regs_base) {
		phy_regs = (struct al_dwc_ddrphy_top_regs_alpine_v1 __iomem *)ddr_phy_regs_base;

		/* PHY version is always the first register in PHY */
		phy_version = al_reg_read32(&phy_regs->dwc_ddrphy_pub.RIDR);

		if (phy_version == AL_DDR_PHY_VERSION_ALPINE_V1)
			*rev = AL_DDR_REV_ID_ALPINE_V1;
		else if (phy_version == AL_DDR_PHY_VERSION_ALPINE_V2)
			*rev = AL_DDR_REV_ID_ALPINE_V2;
		else {
			al_err("%s: Unknown DDR rev %08x\n", __func__, phy_version);
			return -EIO;
		}
	} else {
		ddrc_regs = (struct al_ddrc_regs __iomem *)ddrc_regs_base;

		ddrc_version = al_reg_read32(&ddrc_regs->global.ver);

		/* There was no ddrc in alpine v1 */
		if (ddrc_version == AL_DDRC_VERSION_ALPINE_V2)
			*rev = AL_DDR_REV_ID_ALPINE_V2;
		else if (ddrc_version == AL_DDRC_VERSION_ALPINE_V3_TC)
			*rev = AL_DDR_REV_ID_ALPINE_V3_TC;
		else if (ddrc_version == AL_DDRC_VERSION_ALPINE_V3)
			*rev = AL_DDR_REV_ID_ALPINE_V3;
		else {
			al_err("%s: Unknown DDR rev %08x\n", __func__, ddrc_version);
			return -EIO;
		}
	}
#elif defined(AL_DEV_ID) && (AL_DEV_ID == AL_DEV_ID_ALPINE_V1)
	*rev = AL_DDR_REV_ID_ALPINE_V1;
#elif defined(AL_DEV_ID) && (AL_DEV_ID == AL_DEV_ID_ALPINE_V2)
	*rev = AL_DDR_REV_ID_ALPINE_V2;
#elif defined(AL_DEV_ID) && (AL_DEV_ID == AL_DEV_ID_ALPINE_V3) && \
	defined(AL_DEV_REV_ID) && (AL_DEV_REV_ID == 0)
	*rev = AL_DDR_REV_ID_ALPINE_V3_TC;
#elif defined(AL_DEV_ID) && (AL_DEV_ID == AL_DEV_ID_ALPINE_V3)
	*rev = AL_DDR_REV_ID_ALPINE_V3;
#else
#error Not supported!
#endif

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
int al_ddr_cfg_init_v3(
	void __iomem		*nb_regs_base,
	void __iomem		*ddr_ctrl_regs_base,
	void __iomem		*ddr_phy_regs_base,
	void __iomem		*ddrc_regs_base,
	unsigned int		ch,
	struct al_ddr_cfg	*ddr_cfg)
{
	int err;

	ddr_cfg->nb_regs_base = nb_regs_base;
	ddr_cfg->ddr_ctrl_regs_base = ddr_ctrl_regs_base;
	ddr_cfg->ddr_phy_regs_base = ddr_phy_regs_base;
	ddr_cfg->ddrc_regs_base = ddrc_regs_base;
	ddr_cfg->ch = ch;
	err = al_ddr_rev_get(ddr_phy_regs_base, ddrc_regs_base, &ddr_cfg->rev);
	if (err) {
		al_err("%s: al_ddr_rev_get failed!\n", __func__);
		return err;
	}

#if (!defined(AL_DEV_ID))
	switch (ddr_cfg->rev) {
	case AL_DDR_REV_ID_ALPINE_V1:
		al_ddr_imp_alpine_v1(&ddr_cfg->imp);
		break;
	case AL_DDR_REV_ID_ALPINE_V2:
		al_ddr_imp_alpine_v2(&ddr_cfg->imp);
		break;
	case AL_DDR_REV_ID_ALPINE_V3:
		al_ddr_imp_alpine_v3(&ddr_cfg->imp);
		break;
	default:
		al_err("%s: non supported revision (%d)\n", __func__, ddr_cfg->rev);
		return -EINVAL;
	}
#endif

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
int al_ddr_cfg_init(
	void __iomem		*nb_regs_base,
	void __iomem		*ddr_ctrl_regs_base,
	void __iomem		*ddr_phy_regs_base,
	struct al_ddr_cfg	*ddr_cfg)
{
	return al_ddr_cfg_init_v3(
		nb_regs_base, ddr_ctrl_regs_base, ddr_phy_regs_base, NULL, 0, ddr_cfg);
}

/** @} end of DDR group */

