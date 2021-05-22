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
#include "al_mod_hal_serdes_avg_init.h"
#include "al_mod_hal_serdes_avg_internal.h"
#include "al_mod_hal_serdes_avg_utils.h"
#include "al_mod_hal_serdes_avg_complex_regs.h"
#include "al_mod_hal_serdes_avg_lane_regs.h"

#ifndef AL_SERDES_EXTERNAL_FW_PCIE
#define AL_SERDES_EXTERNAL_FW_PCIE	0
#endif

#ifndef AL_SERDES_EXTERNAL_FW_NON_PCIE
#define AL_SERDES_EXTERNAL_FW_NON_PCIE	0
#endif

#ifndef AL_SERDES_EXTERNAL_FW
#define AL_SERDES_EXTERNAL_FW	0
#endif

#if (AL_SERDES_EXTERNAL_FW == 0) && (AL_SERDES_EXTERNAL_FW_PCIE == 0)
#include "al_mod_hal_serdes_avg_fw_pcie.h"
#endif
#if (AL_SERDES_EXTERNAL_FW == 0) && (AL_SERDES_EXTERNAL_FW_NON_PCIE == 0)
#include "al_mod_hal_serdes_avg_fw_non_pcie.h"
#endif

#define AL_SERDES_AVG_VER			100

#define STGR_CTRL_REG_PWRUP_DLY_MASK		AL_FIELD_MASK(3, 0)
#define STGR_CTRL_REG_PWRUP_DLY_SHIFT		0
#define STGR_CTRL_REG_RESET_DLY_MASK		AL_FIELD_MASK(7, 4)
#define STGR_CTRL_REG_RESET_DLY_SHIFT		4
#define STGR_CTRL_REG_PWRUP_DLY_BYPASS		AL_BIT(8)
#define STGR_CTRL_REG_RESET_DLY_BYPASS		AL_BIT(9)
#define STGR_CTRL_REG_DLY_NUM_CC_MASK		AL_FIELD_MASK(25, 16)
#define STGR_CTRL_REG_DLY_NUM_CC_SHIFT		16

#define SCRATCH_REG_LANE_CFG_REF_CLK_FREQ_MASK	AL_FIELD_MASK(3, 0)
#define SCRATCH_REG_LANE_CFG_REF_CLK_FREQ_SHIFT	0
#define SCRATCH_REG_LANE_CFG_BIT_RATE_MASK	AL_FIELD_MASK(7, 4)
#define SCRATCH_REG_LANE_CFG_BIT_RATE_SHIFT	4
#define SCRATCH_REG_LANE_CFG_BIT_WIDTH_MASK	AL_FIELD_MASK(9, 8)
#define SCRATCH_REG_LANE_CFG_BIT_WIDTH_SHIFT	8
#define SCRATCH_REG_LANE_CFG_MODE_MASK		AL_FIELD_MASK(14, 10)
#define SCRATCH_REG_LANE_CFG_MODE_SHIFT		10

/* SATA reference clock index */
#define REF_CLK_IDX_SATA			0

/* Ethernet reference clock index */
#define REF_CLK_IDX_ETH				5

#ifndef AL_SERDES_ERR_ONLY
#define AL_SERDES_ERR_ONLY	0
#endif

#if (AL_SERDES_ERR_ONLY)
#define AL_INFO(...)			do { } while (0)
#else
#define AL_INFO(...)			al_mod_info(__VA_ARGS__)
#endif

static al_mod_bool al_mod_serdes_avg_lane_cfg_is_pcie(
	struct al_mod_serdes_avg_lane_cfg	*lane_cfg)
{
	switch (lane_cfg->mode) {
	case AL_SRDS_AVG_MODE_PCIE_0:
	case AL_SRDS_AVG_MODE_PCIE_1:
	case AL_SRDS_AVG_MODE_PCIE_2:
	case AL_SRDS_AVG_MODE_PCIE_3:
	case AL_SRDS_AVG_MODE_PCIE_4:
	case AL_SRDS_AVG_MODE_PCIE_5:
	case AL_SRDS_AVG_MODE_PCIE_6:
	case AL_SRDS_AVG_MODE_PCIE_7:
		return AL_TRUE;
	default:
		break;
	}

	return AL_FALSE;
}

static al_mod_bool al_mod_serdes_avg_lane_cfg_is_sata(
	struct al_mod_serdes_avg_lane_cfg	*lane_cfg)
{
	switch (lane_cfg->mode) {
	case AL_SRDS_AVG_MODE_SATA_0:
	case AL_SRDS_AVG_MODE_SATA_1:
	case AL_SRDS_AVG_MODE_SATA_2:
	case AL_SRDS_AVG_MODE_SATA_3:
	case AL_SRDS_AVG_MODE_SATA_4:
	case AL_SRDS_AVG_MODE_SATA_5:
	case AL_SRDS_AVG_MODE_SATA_6:
	case AL_SRDS_AVG_MODE_SATA_7:
		return AL_TRUE;
	default:
		break;
	}

	return AL_FALSE;
}

static al_mod_bool al_mod_serdes_avg_lane_cfg_is_sgmii(
	struct al_mod_serdes_avg_lane_cfg	*lane_cfg)
{
	switch (lane_cfg->mode) {
	case AL_SRDS_AVG_MODE_STD_ETH_0_1G:
	case AL_SRDS_AVG_MODE_STD_ETH_1_1G:
		return AL_TRUE;
	default:
		break;
	}

	return AL_FALSE;
}

static al_mod_bool al_mod_serdes_avg_lane_cfg_is_eth(
	struct al_mod_serdes_avg_lane_cfg	*lane_cfg)
{
	switch (lane_cfg->mode) {
	case AL_SRDS_AVG_MODE_STD_ETH_0_1G:
	case AL_SRDS_AVG_MODE_STD_ETH_0_10G:
	case AL_SRDS_AVG_MODE_STD_ETH_1_1G:
	case AL_SRDS_AVG_MODE_STD_ETH_1_10G:
	case AL_SRDS_AVG_MODE_HS_ETH_0_10G:
	case AL_SRDS_AVG_MODE_HS_ETH_1_10G:
	case AL_SRDS_AVG_MODE_HS_ETH_2_10G:
	case AL_SRDS_AVG_MODE_HS_ETH_3_10G:
	case AL_SRDS_AVG_MODE_HS_ETH_0_25G:
	case AL_SRDS_AVG_MODE_HS_ETH_1_25G:
	case AL_SRDS_AVG_MODE_HS_ETH_2_25G:
	case AL_SRDS_AVG_MODE_HS_ETH_3_25G:
	case AL_SRDS_AVG_MODE_HS_ETH_0_50G:
	case AL_SRDS_AVG_MODE_HS_ETH_2_50G:
	case AL_SRDS_AVG_MODE_HS_ETH_0_100G:
		return AL_TRUE;
	default:
		break;
	}

	return AL_FALSE;
}

static void al_mod_serdes_avg_lane_cfg_hw_wr(
	struct al_mod_serdes_grp_obj	*obj,
	enum al_mod_serdes_clk_freq		ref_clk_freq,
	enum al_mod_serdes_bit_rate		bit_rate,
	enum al_mod_serdes_bit_width	bit_width,
	enum al_mod_serdes_avg_mode		mode)
{
	struct al_mod_srd_1lane_regs __iomem *lane_regs = obj->regs_base;

	obj->complex_obj->reg_write32(&lane_regs->ctl[0].sw_scratch,
		(ref_clk_freq << SCRATCH_REG_LANE_CFG_REF_CLK_FREQ_SHIFT) |
		(bit_rate << SCRATCH_REG_LANE_CFG_BIT_RATE_SHIFT) |
		(bit_width << SCRATCH_REG_LANE_CFG_BIT_WIDTH_SHIFT) |
		(mode << SCRATCH_REG_LANE_CFG_MODE_SHIFT));
}

static void al_mod_serdes_avg_wait_for_o_core_status_indication(
	struct al_mod_serdes_grp_obj	*obj)
{
	struct al_mod_srd_1lane_regs __iomem *lane_regs = obj->regs_base;

	/* TODO: check if indeed bit 5 and make it a define */
	while (!(obj->complex_obj->reg_read32(&lane_regs->gen.status) & AL_BIT(5)))
		;
}

static int al_mod_serdes_avg_fw_init_specific(
	struct al_mod_serdes_complex_obj	*complex,
	const uint32_t			*fw,
	unsigned int			fw_size,
	void				(*fw_rd_cb)(
						uint32_t	*buff,
						unsigned int	size),
	uint32_t			*fw_temp_buff,
	unsigned int			fw_temp_buff_size,
	unsigned int			receiver_addr,
	al_mod_bool				wv)
{
	struct al_mod_sbus_obj *sbus = &complex->sbus_obj;
	unsigned int i;
	unsigned int fw_temp_buff_avail = 0;
	int err = 0;

	/* Place serdes in reset and disable Spico */
	err = al_mod_sbus_write(sbus, receiver_addr, 0x07, 0x00000011, wv);
	if (err)
		goto done;
	/* Remove Serdes reset */
	err = al_mod_sbus_write(sbus, receiver_addr, 0x07, 0x00000010, wv);
	if (err)
		goto done;
	/* Assert IMEM override */
	err = al_mod_sbus_write(sbus, receiver_addr, 0x00, 0x40000000, wv);
	if (err)
		goto done;

	if (fw_rd_cb) {
		al_mod_assert(!fw);
		al_mod_assert(fw_temp_buff);
		al_mod_assert(fw_temp_buff_size);
		fw = fw_temp_buff;
	} else {
		al_mod_assert(fw);
		fw_temp_buff_size = fw_size;
	}

	al_mod_sbus_lock(sbus, receiver_addr);

	/*  Note: A delayed version of the write-enable signal is used to write data */
	/*	into rom after address and data are set up.  This allows loading one */
	/*	new word for each sbus write. */
	for (i = 0; i < fw_size; i++) {
		unsigned int k = (i % fw_temp_buff_size);
		unsigned int sbus_data;

		if (fw_rd_cb && (!fw_temp_buff_avail)) {
			if ((fw_size - i) < fw_temp_buff_size)
				fw_temp_buff_avail = (fw_size - i);
			else
				fw_temp_buff_avail = fw_temp_buff_size;

			fw_rd_cb(fw_temp_buff, fw_temp_buff_avail);
		}

		if (!(i % (1024 / sizeof(uint32_t))))
			AL_INFO("FW DL %u/%u\n", (unsigned int)(i * sizeof(uint32_t)),
				(unsigned int)(sizeof(uint32_t) * fw_size));
		sbus_data = (3 << 30) | swap32_from_le(fw[k]);

		al_mod_dbg("Sbus burst data is %x\n", sbus_data);
		err = al_mod_sbus_write_no_lock(sbus, receiver_addr, 0x0A, sbus_data, wv);
		if (err) {
			al_mod_sbus_unlock(sbus, receiver_addr);
			goto done;
		}

		if (fw_rd_cb)
			fw_temp_buff_avail--;
	}

	al_mod_sbus_unlock(sbus, receiver_addr);

	/*
	 * Pad with 0's - required for Verilog simulations, but may be bypassed when loading
	 * firmware onto chip.
	 */
	err = al_mod_sbus_write(sbus, receiver_addr, 0x0A, 0xc000, wv);
	if (err)
		goto done;
	err = al_mod_sbus_write(sbus, receiver_addr, 0x0A, 0xc000, wv);
	if (err)
		goto done;
	err = al_mod_sbus_write(sbus, receiver_addr, 0x0A, 0xc000, wv);
	if (err)
		goto done;

	/* IMEM override off */
	err = al_mod_sbus_write(sbus, receiver_addr, 0x00, 0x00000000, wv);
	if (err)
		goto done;
	/* Turn ECC on */
	err = al_mod_sbus_write(sbus, receiver_addr, 0x0B, 0x000c0000, wv);
	if (err)
		goto done;
	/* Turn SPICO Enable on */
	err = al_mod_sbus_write(sbus, receiver_addr, 0x07, 0x00000002, wv);
	if (err)
		goto done;
	/* Enable core and hardware interrupts */
	err = al_mod_sbus_write(sbus, receiver_addr, 0x08, 0x00000000, wv);
	if (err)
		goto done;

done:
	if (err)
		al_mod_err("%s: Failed!\n", __func__);

	return err;
}

static int al_mod_serdes_avg_complex_mux_cfg_tnk(
	struct al_mod_serdes_complex_obj	*obj,
	struct al_mod_serdes_avg_lane_cfg	*lane_cfg,
	unsigned int			num_lanes)
{
	al_mod_assert(obj);
	al_mod_assert(lane_cfg);
	al_mod_assert(num_lanes == AL_SRDS_LANES_PER_COMPLEX_TNK_MAIN);

	/* TODO */

	return 0;
}

static int al_mod_serdes_avg_complex_mux_cfg_main_sec(
	struct al_mod_serdes_complex_obj	*obj,
	struct al_mod_serdes_avg_lane_cfg	*lane_cfg,
	unsigned int			num_lanes,
	al_mod_bool				is_main)
{
	struct al_mod_srd_complex_regs __iomem *complex_regs;
	uint32_t main_sel;
	uint32_t main_mux;
	uint32_t multi_sel;

	al_mod_assert(obj);
	al_mod_assert(lane_cfg);
	al_mod_assert(num_lanes == AL_SRDS_LANES_PER_COMPLEX_TR_MAIN);

	complex_regs = obj->regs_base;

	main_sel = obj->reg_read32(&complex_regs->tpr_cfg.main_sel);
	main_mux = obj->reg_read32(&complex_regs->tpr_cfg.main_mux);
	multi_sel = obj->reg_read32(&complex_regs->tpr_cfg.multi_sel);

	switch (lane_cfg[4].mode) {
	case AL_SRDS_AVG_MODE_PCIE_0:
	case AL_SRDS_AVG_MODE_PCIE_2:
		AL_REG_FIELD_SET(main_mux,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD4_MASK,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD4_SHIFT,
			1);
		AL_REG_FIELD_SET(main_mux,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD5_MASK,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD5_SHIFT,
			1);
		AL_REG_FIELD_SET(main_mux,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD6_MASK,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD6_SHIFT,
			1);
		AL_REG_FIELD_SET(main_mux,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD7_MASK,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD7_SHIFT,
			1);
		break;
	case AL_SRDS_AVG_MODE_PCIE_4:
	case AL_SRDS_AVG_MODE_PCIE_6:
		AL_REG_FIELD_SET(main_sel,
			SRD_COMPLEX_TPR_CFG_MAIN_SEL_PCIE4_LN0_SELECT_OH_MASK,
			SRD_COMPLEX_TPR_CFG_MAIN_SEL_PCIE4_LN0_SELECT_OH_SHIFT,
			1);
		AL_REG_FIELD_SET(main_sel,
			SRD_COMPLEX_TPR_CFG_MAIN_SEL_PCIE4_LN1_SELECT_OH_MASK,
			SRD_COMPLEX_TPR_CFG_MAIN_SEL_PCIE4_LN1_SELECT_OH_SHIFT,
			1);
		AL_REG_FIELD_SET(main_mux,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD4_MASK,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD4_SHIFT,
			2);
		AL_REG_FIELD_SET(main_mux,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD5_MASK,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD5_SHIFT,
			2);
		AL_REG_FIELD_SET(main_mux,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD6_MASK,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD6_SHIFT,
			2);
		AL_REG_FIELD_SET(main_mux,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD7_MASK,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD7_SHIFT,
			2);
		break;
	default:
		break;
	}

	switch (lane_cfg[8].mode) {
	case AL_SRDS_AVG_MODE_PCIE_0:
	case AL_SRDS_AVG_MODE_PCIE_2:
		AL_REG_FIELD_SET(main_mux,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD8_MASK,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD8_SHIFT,
			1);
		AL_REG_FIELD_SET(main_mux,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD9_MASK,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD9_SHIFT,
			1);
		AL_REG_FIELD_SET(main_mux,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD10_MASK,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD10_SHIFT,
			1);
		AL_REG_FIELD_SET(main_mux,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD11_MASK,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD11_SHIFT,
			1);
		break;
	case AL_SRDS_AVG_MODE_PCIE_1:
	case AL_SRDS_AVG_MODE_PCIE_3:
		AL_REG_FIELD_SET(main_mux,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD8_MASK,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD8_SHIFT,
			2);
		AL_REG_FIELD_SET(main_mux,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD9_MASK,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD9_SHIFT,
			2);
		AL_REG_FIELD_SET(main_mux,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD10_MASK,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD10_SHIFT,
			2);
		AL_REG_FIELD_SET(main_mux,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD11_MASK,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD11_SHIFT,
			2);
		break;
	default:
		break;
	}

	switch (lane_cfg[12].mode) {
	case AL_SRDS_AVG_MODE_PCIE_0:
	case AL_SRDS_AVG_MODE_PCIE_2:
		AL_REG_FIELD_SET(main_mux,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD12_MASK,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD12_SHIFT,
			1);
		AL_REG_FIELD_SET(main_mux,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD13_MASK,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD13_SHIFT,
			1);
		break;
	case AL_SRDS_AVG_MODE_PCIE_1:
	case AL_SRDS_AVG_MODE_PCIE_3:
		AL_REG_FIELD_SET(main_mux,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD12_MASK,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD12_SHIFT,
			2);
		AL_REG_FIELD_SET(main_mux,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD13_MASK,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD13_SHIFT,
			2);
		break;
	case AL_SRDS_AVG_MODE_PCIE_5:
	case AL_SRDS_AVG_MODE_PCIE_7:
		AL_REG_FIELD_SET(main_mux,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD12_MASK,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD12_SHIFT,
			4);
		AL_REG_FIELD_SET(main_mux,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD13_MASK,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD13_SHIFT,
			4);
		break;
	default:
		break;
	}

	switch (lane_cfg[14].mode) {
	case AL_SRDS_AVG_MODE_PCIE_0:
	case AL_SRDS_AVG_MODE_PCIE_2:
		AL_REG_FIELD_SET(main_mux,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD14_MASK,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD14_SHIFT,
			1);
		AL_REG_FIELD_SET(main_mux,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD15_MASK,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD15_SHIFT,
			1);
		break;
	case AL_SRDS_AVG_MODE_PCIE_1:
	case AL_SRDS_AVG_MODE_PCIE_3:
		AL_REG_FIELD_SET(main_mux,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD14_MASK,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD14_SHIFT,
			2);
		AL_REG_FIELD_SET(main_mux,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD15_MASK,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD15_SHIFT,
			2);
		break;
	case AL_SRDS_AVG_MODE_PCIE_5:
	case AL_SRDS_AVG_MODE_PCIE_7:
		AL_REG_FIELD_SET(main_mux,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD14_MASK,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD14_SHIFT,
			4);
		AL_REG_FIELD_SET(main_mux,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD15_MASK,
			SRD_COMPLEX_TPR_CFG_MAIN_MUX_SELECT_OH_SRD15_SHIFT,
			4);
		break;
	case AL_SRDS_AVG_MODE_SATA_3:
	case AL_SRDS_AVG_MODE_SATA_7:
		AL_REG_FIELD_SET(multi_sel,
				SRD_COMPLEX_TPR_CFG_MULTI_SEL_LN14_MASK,
				SRD_COMPLEX_TPR_CFG_MULTI_SEL_LN14_SHIFT,
				1);
		break;
	case AL_SRDS_AVG_MODE_STD_ETH_0_1G:
	case AL_SRDS_AVG_MODE_STD_ETH_0_10G:
		AL_REG_FIELD_SET(multi_sel,
				SRD_COMPLEX_TPR_CFG_MULTI_SEL_LN14_MASK,
				SRD_COMPLEX_TPR_CFG_MULTI_SEL_LN14_SHIFT,
				is_main ? 2 : 4);
		AL_REG_FIELD_SET(multi_sel,
				SRD_COMPLEX_TPR_CFG_MULTI_SEL_ETH0_MASK,
				SRD_COMPLEX_TPR_CFG_MULTI_SEL_ETH0_SHIFT,
				is_main ? 1 : 2);
		break;
	default:
		break;
	}

	switch (lane_cfg[15].mode) {
	case AL_SRDS_AVG_MODE_SATA_3:
	case AL_SRDS_AVG_MODE_SATA_7:
		AL_REG_FIELD_SET(multi_sel,
				SRD_COMPLEX_TPR_CFG_MULTI_SEL_LN15_MASK,
				SRD_COMPLEX_TPR_CFG_MULTI_SEL_LN15_SHIFT,
				1);
		break;
	case AL_SRDS_AVG_MODE_STD_ETH_1_1G:
	case AL_SRDS_AVG_MODE_STD_ETH_1_10G:
		AL_REG_FIELD_SET(multi_sel,
				SRD_COMPLEX_TPR_CFG_MULTI_SEL_LN15_MASK,
				SRD_COMPLEX_TPR_CFG_MULTI_SEL_LN15_SHIFT,
				is_main ? 2 : 4);
		AL_REG_FIELD_SET(multi_sel,
				SRD_COMPLEX_TPR_CFG_MULTI_SEL_ETH1_MASK,
				SRD_COMPLEX_TPR_CFG_MULTI_SEL_ETH1_SHIFT,
				is_main ? 1 : 2);
		break;
	default:
		break;
	}


	if ((lane_cfg[14].mode != AL_SRDS_AVG_MODE_STD_ETH_0_1G) &&
		(lane_cfg[14].mode != AL_SRDS_AVG_MODE_STD_ETH_0_10G))
		AL_REG_FIELD_SET(multi_sel,
			   SRD_COMPLEX_TPR_CFG_MULTI_SEL_ETH0_MASK,
			   SRD_COMPLEX_TPR_CFG_MULTI_SEL_ETH0_SHIFT,
			   is_main ? 2 : 1);

	if ((lane_cfg[15].mode != AL_SRDS_AVG_MODE_STD_ETH_1_1G) &&
		(lane_cfg[15].mode != AL_SRDS_AVG_MODE_STD_ETH_1_10G))
		AL_REG_FIELD_SET(multi_sel,
			   SRD_COMPLEX_TPR_CFG_MULTI_SEL_ETH1_MASK,
			   SRD_COMPLEX_TPR_CFG_MULTI_SEL_ETH1_SHIFT,
			   is_main ? 2 : 1);


	obj->reg_write32(&complex_regs->tpr_cfg.main_sel, main_sel);
	obj->reg_write32(&complex_regs->tpr_cfg.main_mux, main_mux);
	obj->reg_write32(&complex_regs->tpr_cfg.multi_sel, multi_sel);

	return 0;
}

static int al_mod_serdes_avg_complex_mux_cfg_hs(
	struct al_mod_serdes_complex_obj	*obj,
	struct al_mod_serdes_avg_lane_cfg	*lane_cfg,
	unsigned int			num_lanes)
{
	struct al_mod_srd_complex_regs __iomem *complex_regs;
	uint32_t hs_sel;
	uint32_t hs_mux;

	al_mod_assert(obj);
	al_mod_assert(lane_cfg);
	al_mod_assert(num_lanes == AL_SRDS_LANES_PER_COMPLEX_TR_HS);

	complex_regs = obj->regs_base;

	hs_sel = obj->reg_read32(&complex_regs->tpr_cfg.hs_sel);
	hs_mux = obj->reg_read32(&complex_regs->tpr_cfg.hs_mux);

	/* TODO hs_mux */

	switch (lane_cfg[0].mode) {
	case AL_SRDS_AVG_MODE_HS_ETH_0_10G:
	case AL_SRDS_AVG_MODE_HS_ETH_0_25G:
	case AL_SRDS_AVG_MODE_HS_ETH_0_50G:
	case AL_SRDS_AVG_MODE_HS_ETH_0_100G:
		AL_REG_FIELD_SET(hs_sel,
			SRD_COMPLEX_TPR_CFG_HS_SEL_TX0_CLKSEL_MASK,
			SRD_COMPLEX_TPR_CFG_HS_SEL_TX0_CLKSEL_SHIFT,
			0);
		break;
	default:
		break;
	}

	switch (lane_cfg[1].mode) {
	case AL_SRDS_AVG_MODE_HS_ETH_1_10G:
	case AL_SRDS_AVG_MODE_HS_ETH_1_25G:
		AL_REG_FIELD_SET(hs_sel,
			SRD_COMPLEX_TPR_CFG_HS_SEL_TX1_CLKSEL_MASK,
			SRD_COMPLEX_TPR_CFG_HS_SEL_TX1_CLKSEL_SHIFT,
			1);
		break;
	case AL_SRDS_AVG_MODE_HS_ETH_0_50G:
	case AL_SRDS_AVG_MODE_HS_ETH_0_100G:
		AL_REG_FIELD_SET(hs_sel,
			SRD_COMPLEX_TPR_CFG_HS_SEL_TX1_CLKSEL_MASK,
			SRD_COMPLEX_TPR_CFG_HS_SEL_TX1_CLKSEL_SHIFT,
			0);
		break;
	default:
		break;
	}

	switch (lane_cfg[2].mode) {
	case AL_SRDS_AVG_MODE_HS_ETH_2_10G:
	case AL_SRDS_AVG_MODE_HS_ETH_2_25G:
	case AL_SRDS_AVG_MODE_HS_ETH_2_50G:
		AL_REG_FIELD_SET(hs_sel,
			SRD_COMPLEX_TPR_CFG_HS_SEL_TX2_CLKSEL_MASK,
			SRD_COMPLEX_TPR_CFG_HS_SEL_TX2_CLKSEL_SHIFT,
			2);
		break;
	case AL_SRDS_AVG_MODE_HS_ETH_0_100G:
		AL_REG_FIELD_SET(hs_sel,
			SRD_COMPLEX_TPR_CFG_HS_SEL_TX2_CLKSEL_MASK,
			SRD_COMPLEX_TPR_CFG_HS_SEL_TX2_CLKSEL_SHIFT,
			0);
		break;
	default:
		break;
	}

	switch (lane_cfg[3].mode) {
	case AL_SRDS_AVG_MODE_HS_ETH_3_10G:
	case AL_SRDS_AVG_MODE_HS_ETH_3_25G:
		AL_REG_FIELD_SET(hs_sel,
			SRD_COMPLEX_TPR_CFG_HS_SEL_TX3_CLKSEL_MASK,
			SRD_COMPLEX_TPR_CFG_HS_SEL_TX3_CLKSEL_SHIFT,
			3);
		break;
	case AL_SRDS_AVG_MODE_HS_ETH_2_50G:
		AL_REG_FIELD_SET(hs_sel,
			SRD_COMPLEX_TPR_CFG_HS_SEL_TX3_CLKSEL_MASK,
			SRD_COMPLEX_TPR_CFG_HS_SEL_TX3_CLKSEL_SHIFT,
			2);
		break;
	case AL_SRDS_AVG_MODE_HS_ETH_0_100G:
		AL_REG_FIELD_SET(hs_sel,
			SRD_COMPLEX_TPR_CFG_HS_SEL_TX3_CLKSEL_MASK,
			SRD_COMPLEX_TPR_CFG_HS_SEL_TX3_CLKSEL_SHIFT,
			0);
		break;
	default:
		break;
	}

	obj->reg_write32(&complex_regs->tpr_cfg.hs_sel, hs_sel);
	obj->reg_write32(&complex_regs->tpr_cfg.hs_mux, hs_mux);

	return 0;
}

static void al_mod_serdes_avg_lane_cfg_pre_sbus_reset(
	struct al_mod_serdes_grp_obj	*obj,
	struct al_mod_serdes_avg_lane_cfg	*lane_cfg)
{
	al_mod_bool is_pcie = al_mod_serdes_avg_lane_cfg_is_pcie(lane_cfg);
	al_mod_bool is_sata = al_mod_serdes_avg_lane_cfg_is_sata(lane_cfg);
	al_mod_bool is_eth = al_mod_serdes_avg_lane_cfg_is_eth(lane_cfg);
	unsigned int ref_clk_sel =
		is_sata ? REF_CLK_IDX_SATA : ((is_eth) ? REF_CLK_IDX_ETH : lane_cfg->ref_clk_sel);
	struct al_mod_srd_1lane_regs __iomem *lane_regs = obj->regs_base;
	uint32_t reg_val;

	if (is_pcie)
		reg_val =
			(ref_clk_sel << SRD_1LANE_PCIE_CFG_REFCLK_SEL_SHIFT) |
			SRD_1LANE_PCIE_CFG_PCS_INTERRUPT_DISABLE |
			SRD_1LANE_PCIE_CFG_RX_8B10B_REALIGN_EN |
			SRD_1LANE_PCIE_CFG_DISABLE_FSM |
			SRD_1LANE_PCIE_CFG_SBUS_IF_BYPASS |
			SRD_1LANE_PCIE_CFG_CORE_CLK_MUX_SEL |
			SRD_1LANE_PCIE_CFG_PCLK_MUX_SEL |
			SRD_1LANE_PCIE_CFG_I_PCIE_CORE_CLK_MUX_SEL;
	else if (is_eth)
		reg_val =
			(ref_clk_sel << SRD_1LANE_PCIE_CFG_REFCLK_SEL_SHIFT) |
			SRD_1LANE_PCIE_CFG_MULTI_COMMA_DET_BYPASS |
			SRD_1LANE_PCIE_CFG_PCS_INTERRUPT_DISABLE |
			SRD_1LANE_PCIE_CFG_DISABLE_FSM |
			SRD_1LANE_PCIE_CFG_BYPASS_EN |
			SRD_1LANE_PCIE_CFG_I_PCIE_CORE_CLK_DISABLE |
			SRD_1LANE_PCIE_CFG_ASYNC_RESET_N;
	else
		reg_val =
			(ref_clk_sel << SRD_1LANE_PCIE_CFG_REFCLK_SEL_SHIFT) |
			SRD_1LANE_PCIE_CFG_PCS_INTERRUPT_DISABLE |
			SRD_1LANE_PCIE_CFG_DISABLE_FSM |
			SRD_1LANE_PCIE_CFG_BYPASS_EN |
			SRD_1LANE_PCIE_CFG_I_PCIE_CORE_CLK_DISABLE |
			SRD_1LANE_PCIE_CFG_ASYNC_RESET_N;

	obj->complex_obj->reg_write32(&lane_regs->pcie.cfg, reg_val);

	/* Wait for the reference clock to be valid */
	al_mod_udelay(5);

	/* PCIe additional configuration after clocks are valid */
	if (is_pcie) {
		reg_val &= ~SRD_1LANE_PCIE_CFG_SBUS_IF_BYPASS;
		reg_val |= SRD_1LANE_PCIE_CFG_ASYNC_RESET_N;
		obj->complex_obj->reg_write32(&lane_regs->pcie.cfg, reg_val);

		/*
		 * Stagger the reset & powerup delay in the PIPE interface per lane
		 * - Delay 1 for lanes 0, 4, 8, 12
		 * - Delay 2 for lanes 1, 5, 9, 13
		 * - Delay 3 for lanes 2, 6, 10, 14
		 * - Delay 4 for lanes 3, 7, 11, 15
		 * - Set delay factor to 250 clock cycles
		 */
		if ((obj->complex_obj->complex_type == AL_SRDS_COMPLEX_TYPE_TR_MAIN) ||
			(obj->complex_obj->complex_type == AL_SRDS_COMPLEX_TYPE_TR_SEC)) {
			static const unsigned int dly_num_cc = 250;
			static const unsigned int dly[16] = {
				1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4 };

			reg_val = obj->complex_obj->reg_read32(&lane_regs->ctl[0].reserved);
			reg_val &= ~(
				STGR_CTRL_REG_RESET_DLY_MASK |
				STGR_CTRL_REG_PWRUP_DLY_MASK |
				STGR_CTRL_REG_RESET_DLY_BYPASS |
				STGR_CTRL_REG_PWRUP_DLY_BYPASS |
				STGR_CTRL_REG_DLY_NUM_CC_MASK);
			reg_val |=
				(dly[obj->lane_in_complex] << STGR_CTRL_REG_RESET_DLY_SHIFT) |
				(dly[obj->lane_in_complex] << STGR_CTRL_REG_PWRUP_DLY_SHIFT) |
				(dly_num_cc << STGR_CTRL_REG_DLY_NUM_CC_SHIFT);
			obj->complex_obj->reg_write32(&lane_regs->ctl[0].reserved, reg_val);
		}
	}
}

static void al_mod_serdes_avg_complex_config_pre_sbus_reset(
	struct al_mod_serdes_complex_obj	*obj,
	struct al_mod_serdes_avg_lane_cfg	*lane_cfg,
	unsigned int			num_lanes)
{
	unsigned int i;

	for (i = 0; i < num_lanes; i++) {
		if (lane_cfg[i].mode == AL_SRDS_AVG_MODE_SKIP)
			continue;

		al_mod_assert(obj->lanes[i]);
		al_mod_serdes_avg_lane_cfg_pre_sbus_reset(obj->lanes[i], &lane_cfg[i]);
	}
}

static void al_mod_serdes_avg_bit_rate_and_width_get(
	struct al_mod_serdes_avg_lane_cfg	*lane_cfg,
	enum al_mod_serdes_bit_rate		*bit_rate,
	enum al_mod_serdes_bit_width	*bit_width)
{
	enum al_mod_serdes_bit_rate br = AL_SRDS_BIT_RATE_6_0G;
	enum al_mod_serdes_bit_width bw = AL_SRDS_BIT_WIDTH_20;

	switch (lane_cfg->mode) {
	case AL_SRDS_AVG_MODE_PCIE_0:
	case AL_SRDS_AVG_MODE_PCIE_1:
	case AL_SRDS_AVG_MODE_PCIE_2:
	case AL_SRDS_AVG_MODE_PCIE_3:
	case AL_SRDS_AVG_MODE_PCIE_4:
	case AL_SRDS_AVG_MODE_PCIE_5:
	case AL_SRDS_AVG_MODE_PCIE_6:
	case AL_SRDS_AVG_MODE_PCIE_7:
		/* Bit width & rate are controlled by the PCIe unit */
		break;

	case AL_SRDS_AVG_MODE_SATA_0:
	case AL_SRDS_AVG_MODE_SATA_1:
	case AL_SRDS_AVG_MODE_SATA_2:
	case AL_SRDS_AVG_MODE_SATA_3:
	case AL_SRDS_AVG_MODE_SATA_4:
	case AL_SRDS_AVG_MODE_SATA_5:
	case AL_SRDS_AVG_MODE_SATA_6:
	case AL_SRDS_AVG_MODE_SATA_7:
		br = AL_SRDS_BIT_RATE_6_0G;
		bw = AL_SRDS_BIT_WIDTH_20;
		break;

	case AL_SRDS_AVG_MODE_STD_ETH_0_1G:
	case AL_SRDS_AVG_MODE_STD_ETH_1_1G:
		br = AL_SRDS_BIT_RATE_1_25G;
		bw = AL_SRDS_BIT_WIDTH_10;
		break;

	case AL_SRDS_AVG_MODE_STD_ETH_0_10G:
	case AL_SRDS_AVG_MODE_STD_ETH_1_10G:
	case AL_SRDS_AVG_MODE_HS_ETH_0_10G:
	case AL_SRDS_AVG_MODE_HS_ETH_1_10G:
	case AL_SRDS_AVG_MODE_HS_ETH_2_10G:
	case AL_SRDS_AVG_MODE_HS_ETH_3_10G:
		br = AL_SRDS_BIT_RATE_10_3125G;
		bw = AL_SRDS_BIT_WIDTH_20;
		break;

	case AL_SRDS_AVG_MODE_HS_ETH_0_25G:
	case AL_SRDS_AVG_MODE_HS_ETH_1_25G:
	case AL_SRDS_AVG_MODE_HS_ETH_2_25G:
	case AL_SRDS_AVG_MODE_HS_ETH_3_25G:
	case AL_SRDS_AVG_MODE_HS_ETH_0_50G:
	case AL_SRDS_AVG_MODE_HS_ETH_2_50G:
	case AL_SRDS_AVG_MODE_HS_ETH_0_100G:
		br = AL_SRDS_BIT_RATE_25_78G;
		bw = AL_SRDS_BIT_WIDTH_40;
		break;

	default:
		al_mod_err("%s: invalid mode (%d)\n", __func__, lane_cfg->mode);
		al_mod_assert(0);
	};

	*bit_width = bw;
	*bit_rate = br;
}

/**
 * Fixes needed due to different backend parameters per lane on Alpine V3 Rev 1 silicon.
 *
 * Main and secondary complexes values for non-PCIe lanes:
 * Values are for 20 bit lanes (SATA and KR) and should be divided
 * by 2 (rounded down) for 10-bit lanes (SGMII).
 * - Lane 0 - 17
 * - Lane 1 - 17
 * - Lane 2 - 17
 * - Lane 3 - 17
 * - Lane 4 - 17
 * - Lane 5 - 18
 * - Lane 6 - 18
 * - Lane 7 - 18
 * - Lane 8 - 17
 * - Lane 9 - 16
 * - Lane 10 - 16
 * - Lane 11 - 16
 * - Lane 12 - 18
 * - Lane 13 - 18
 * - Lane 14 - 16
 * - Lane 15 - 16
 *
 *  High-speed complex values:
 * - Lane 0 - 35
 * - Lane 1 - 35
 * - Lane 2 - 37
 * - Lane 3 - 35
 */
static unsigned int al_mod_serdes_avg_rx_phase_slip_val(
	struct al_mod_serdes_grp_obj	*obj,
	al_mod_bool is_pcie,
	enum al_mod_serdes_bit_width bit_width)
{
	unsigned int val = 0;

	switch (obj->complex_obj->complex_type) {
	case AL_SRDS_COMPLEX_TYPE_TR_MAIN:
	case AL_SRDS_COMPLEX_TYPE_TR_SEC:
		if (is_pcie)
			break;

		if ((bit_width != AL_SRDS_BIT_WIDTH_10) &&
			(bit_width != AL_SRDS_BIT_WIDTH_20))
			break;

		switch (obj->lane_in_complex) {
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 8:
			val = 17;
			break;
		case 5:
		case 6:
		case 7:
		case 12:
		case 13:
			val = 18;
			break;
		case 9:
		case 10:
		case 11:
		case 14:
		case 15:
			val = 16;
			break;
		default:
			al_mod_assert(AL_FALSE);
			break;
		}

		if (bit_width == AL_SRDS_BIT_WIDTH_10)
			val /= 2;
		break;
	case AL_SRDS_COMPLEX_TYPE_TR_HS:
		switch (obj->lane_in_complex) {
		case 0:
		case 1:
		case 3:
			val = 35;
			break;
		case 2:
			val = 37;
			break;
		default:
			al_mod_assert(AL_FALSE);
			break;
		}
		break;
	default:
		break;
	}

	return val;
}

#define LANE_CFG_NUM_STEPS	4
static int al_mod_serdes_avg_lane_config(
	struct al_mod_serdes_grp_obj	*obj,
	struct al_mod_serdes_avg_lane_cfg	*lane_cfg,
	unsigned int			step)
{
	enum al_mod_serdes_clk_freq ref_clk_freq = lane_cfg->ref_clk_freq;
	al_mod_bool is_pcie = al_mod_serdes_avg_lane_cfg_is_pcie(lane_cfg);
	al_mod_bool is_sata = al_mod_serdes_avg_lane_cfg_is_sata(lane_cfg);
	al_mod_bool is_eth = al_mod_serdes_avg_lane_cfg_is_eth(lane_cfg);
	al_mod_bool is_sgmii = al_mod_serdes_avg_lane_cfg_is_sgmii(lane_cfg);
	struct al_mod_srd_1lane_regs __iomem *lane_regs = obj->regs_base;
	enum al_mod_serdes_bit_rate bit_rate;
	enum al_mod_serdes_bit_width bit_width;
	unsigned int rx_phase_slip_val;
	int err;

	al_mod_serdes_avg_bit_rate_and_width_get(lane_cfg, &bit_rate, &bit_width);

	switch (step) {
	case 0:
		/* Set bit-rate ratio */
		if (!is_pcie)
			al_mod_serdes_avg_core_interrupt_no_wait(obj,
				CORE_INT_TX_BIT_RATE_REG_NUM,
				(is_eth ? CORE_INT_TX_BIT_RATE_REFCLK_SYNC_MASTER : 0) |
				(al_mod_ratio_field_val_get(ref_clk_freq, bit_rate) <<
				CORE_INT_TX_BIT_RATE_RATIO_SHIFT) |
				(CORE_INT_TX_BIT_RATE_RATE_VAL_1_1 <<
				CORE_INT_TX_BIT_RATE_RATE_SHIFT));
		break;

	case 1:
		if (!is_pcie)
			al_mod_serdes_avg_core_interrupt_res_wait_and_get(obj);

		/* Set bit-rate ratio */
		if (!is_pcie)
			al_mod_serdes_avg_core_interrupt_no_wait(obj,
				CORE_INT_RX_BIT_RATE_REG_NUM,
				(al_mod_ratio_field_val_get(ref_clk_freq, bit_rate) <<
				CORE_INT_RX_BIT_RATE_RATIO_SHIFT) |
				(CORE_INT_RX_BIT_RATE_RATE_VAL_1_1 <<
				 CORE_INT_RX_BIT_RATE_RATE_SHIFT));
		break;

	case 2:
		if (!is_pcie)
			al_mod_serdes_avg_core_interrupt_res_wait_and_get(obj);

		/* Programming bit-width */
		if (!is_pcie)
			al_mod_serdes_avg_core_interrupt(obj,
				CORE_INT_WIDTH_MODE_REG_NUM, al_mod_width_field_val_get(bit_width));

		/* RX Bit Slip Programming */
		rx_phase_slip_val =
			al_mod_serdes_avg_rx_phase_slip_val(obj, is_pcie, bit_width);

		if (rx_phase_slip_val)
			al_mod_serdes_avg_core_interrupt(obj,
				CORE_INT_RX_PHASE_SLIP_REG_NUM,
				(rx_phase_slip_val <<
				CORE_INT_RX_PHASE_SLIP_RX_FIFO_CLK_SHIFT) |
				CORE_INT_RX_PHASE_SLIP_ON_RX_ENABLE);

		/* Disable Tx electrical idle gate (SATA) */
		if (is_sata && !(lane_cfg->init_for_prbs)) {
			unsigned int rval;

			rval = al_mod_serdes_avg_core_interrupt(obj, CORE_INT_ESB_REG_RD(0x21), 0x0);
			al_mod_serdes_avg_core_interrupt(obj, CORE_INT_ESB_REG_WR(0x21),
				rval & ~AL_BIT(7));
		}

		/* Enable electrical idle detection */
		if (is_sata) {
			al_mod_serdes_avg_core_interrupt(obj, CORE_INT_IDLE_DETECTOR_REG_NUM,
				CORE_INT_IDLE_DETECTOR_RX_IDLE_DETECTOR_EN);
		}

		/* Clear the signal loss sticky indication */
		if (!is_pcie)
			al_mod_serdes_avg_signal_loss_sticky_get(obj, AL_TRUE);

		/* Configure RX signal detect as RX ready (Ethernet) */
		if (is_eth) {
			uint32_t reg;

			reg = obj->complex_obj->reg_read32(&lane_regs->sata.conf);
			reg &= ~SRD_1LANE_SATA_CONF_RXSIGNALDETECT_AS_RXRDY;
			obj->complex_obj->reg_write32(&lane_regs->sata.conf, reg);
			/*  Enable postdiv clocks  */
			reg = obj->complex_obj->reg_read32(&lane_regs->eth.postdiv);
			reg |= SRD_1LANE_ETH_POSTDIV_ENABLE;
			obj->complex_obj->reg_write32(&lane_regs->eth.postdiv, reg);
		}

		/* Ensure Tx/rx disabled */
		if (is_eth)
			al_mod_serdes_avg_core_interrupt(obj,
				CORE_INT_SRDS_EN_REG_NUM, 0);

		/* Loads TX EQ with client configuration */
		if (!is_pcie && lane_cfg->tx_eq_params) {
			al_mod_serdes_avg_core_interrupt(obj, CORE_INT_TX_EQ_REG_NUM,
				CORE_INT_TX_EQ_TAP_SELECTION_PRE1_SET(
					lane_cfg->tx_eq_params->pre1));
			al_mod_serdes_avg_core_interrupt(obj, CORE_INT_TX_EQ_REG_NUM,
				CORE_INT_TX_EQ_TAP_SELECTION_ATTEN_SET(
					lane_cfg->tx_eq_params->atten));
			al_mod_serdes_avg_core_interrupt(obj, CORE_INT_TX_EQ_REG_NUM,
				CORE_INT_TX_EQ_TAP_SELECTION_POST1_SET(
					lane_cfg->tx_eq_params->post1));
		}

		if (is_sgmii) {
			al_mod_serdes_avg_core_interrupt(obj,
				CORE_INT_SET_RX_EQ_REG_NUM, CORE_INT_SET_RX_EQ_CTLE_HF(0x0));
			al_mod_serdes_avg_core_interrupt(obj,
				CORE_INT_SET_RX_EQ_REG_NUM, CORE_INT_SET_RX_EQ_CTLE_LF(0xf));
			al_mod_serdes_avg_core_interrupt(obj,
				CORE_INT_SET_RX_EQ_REG_NUM, CORE_INT_SET_RX_EQ_CTLE_BW(0x5));
		}

		/* Reset RX/TX Ethernet serdes */
		if (is_eth || is_sata) {
			err = al_mod_serdes_avg_rx_tx_reset(obj, is_eth);
			if (err) {
				al_mod_err("%s: al_mod_serdes_avg_rx_tx_reset failed!\n", __func__);
				return err;
			}
		}

		/* Save lane configuration to scratch register */
		al_mod_serdes_avg_lane_cfg_hw_wr(obj, ref_clk_freq, bit_rate, bit_width,
			lane_cfg->mode);

		break;

	case 3:
		/* Set SATA speed handling manualy */
		if (is_sata) {
			uint32_t reg;
			reg = obj->complex_obj->reg_read32(&lane_regs->sata.conf);
			reg &= ~SRD_1LANE_SATA_CONF_MANUAL_SPEED;
			obj->complex_obj->reg_write32(&lane_regs->sata.conf, reg);
		}

		AL_INFO("SRD_INIT: done serdes_init through core interrupt Interface\n");

		break;

	default:
		al_mod_assert(0);
	}

	return 0;
}

static int al_mod_serdes_avg_complex_config(
	struct al_mod_serdes_complex_obj	*obj,
	struct al_mod_serdes_avg_lane_cfg	*lane_cfg,
	unsigned int			num_lanes)
{
	int err;
	unsigned int i;
	unsigned int lane_cfg_step;

	al_mod_assert(obj);
	al_mod_assert(lane_cfg);
	al_mod_assert(
		(obj->complex_type == AL_SRDS_COMPLEX_TYPE_TNK_MAIN) ||
		(obj->complex_type == AL_SRDS_COMPLEX_TYPE_TR_TC_MAIN) ||
		(obj->complex_type == AL_SRDS_COMPLEX_TYPE_TR_MAIN) ||
		(obj->complex_type == AL_SRDS_COMPLEX_TYPE_TR_SEC) ||
		(obj->complex_type == AL_SRDS_COMPLEX_TYPE_TR_HS));

	switch (obj->complex_type) {
	case AL_SRDS_COMPLEX_TYPE_TNK_MAIN:
		err = al_mod_serdes_avg_complex_mux_cfg_tnk(obj, lane_cfg, num_lanes);
		if (err) {
			al_mod_err("%s: al_mod_serdes_avg_complex_mux_cfg_tnk failed!\n", __func__);
			return err;
		}
		break;
	case AL_SRDS_COMPLEX_TYPE_TR_TC_MAIN:
		/* No muxing is required */
		break;

	case AL_SRDS_COMPLEX_TYPE_TR_MAIN:
	case AL_SRDS_COMPLEX_TYPE_TR_SEC:
		err = al_mod_serdes_avg_complex_mux_cfg_main_sec(obj, lane_cfg, num_lanes,
			(obj->complex_type == AL_SRDS_COMPLEX_TYPE_TR_MAIN));
		if (err) {
			al_mod_err("%s: al_mod_serdes_avg_complex_mux_cfg_main_sec failed!\n", __func__);
			return err;
		}
		break;
	case AL_SRDS_COMPLEX_TYPE_TR_HS:
		err = al_mod_serdes_avg_complex_mux_cfg_hs(obj, lane_cfg, num_lanes);
		if (err) {
			al_mod_err("%s: al_mod_serdes_avg_complex_mux_cfg_hs failed!\n", __func__);
			return err;
		}
		break;
	default:
		al_mod_err("%s: invalid complex type (%d)\n", __func__, obj->complex_type);
		return -EINVAL;
	}

	for (lane_cfg_step = 0; lane_cfg_step < LANE_CFG_NUM_STEPS; lane_cfg_step++) {
		for (i = 0; i < num_lanes; i++) {
			if (lane_cfg[i].mode == AL_SRDS_AVG_MODE_SKIP)
				continue;

			al_mod_assert(obj->lanes[i]);
			err = al_mod_serdes_avg_lane_config(obj->lanes[i], &lane_cfg[i], lane_cfg_step);
			if (err) {
				al_mod_err("%s: al_mod_serdes_avg_lane_config failed!\n", __func__);
				return err;
			}
		}
	}

	return 0;
}

/******************************************************************************/
/******************************************************************************/
#if (AL_SERDES_ERR_ONLY == 0)
static const char *al_mod_serdes_avg_mode_to_str(enum al_mod_serdes_avg_mode mode)
{
	switch (mode) {
	case AL_SRDS_AVG_MODE_SKIP:
		return "SKIP";
	case AL_SRDS_AVG_MODE_PCIE_0:
		return "PCIE_0";
	case AL_SRDS_AVG_MODE_PCIE_1:
		return "PCIE_1";
	case AL_SRDS_AVG_MODE_PCIE_4:
		return "PCIE_4";
	case AL_SRDS_AVG_MODE_PCIE_5:
		return "PCIE_5";
	case AL_SRDS_AVG_MODE_PCIE_2:
		return "PCIE_2";
	case AL_SRDS_AVG_MODE_PCIE_3:
		return "PCIE_3";
	case AL_SRDS_AVG_MODE_PCIE_6:
		return "PCIE_6";
	case AL_SRDS_AVG_MODE_PCIE_7:
		return "PCIE_7";
	case AL_SRDS_AVG_MODE_SATA_0:
		return "SATA_0";
	case AL_SRDS_AVG_MODE_SATA_1:
		return "SATA_1";
	case AL_SRDS_AVG_MODE_SATA_2:
		return "SATA_2";
	case AL_SRDS_AVG_MODE_SATA_3:
		return "SATA_3";
	case AL_SRDS_AVG_MODE_SATA_4:
		return "SATA_4";
	case AL_SRDS_AVG_MODE_SATA_5:
		return "SATA_5";
	case AL_SRDS_AVG_MODE_SATA_6:
		return "SATA_6";
	case AL_SRDS_AVG_MODE_SATA_7:
		return "SATA_7";
	case AL_SRDS_AVG_MODE_STD_ETH_0_1G:
		return "STD_ETH_0_1G";
	case AL_SRDS_AVG_MODE_STD_ETH_0_10G:
		return "STD_ETH_0_10G";
	case AL_SRDS_AVG_MODE_STD_ETH_1_1G:
		return "STD_ETH_1_1G";
	case AL_SRDS_AVG_MODE_STD_ETH_1_10G:
		return "STD_ETH_1_10G";
	case AL_SRDS_AVG_MODE_HS_ETH_0_10G:
		return "HS_ETH_0_10G";
	case AL_SRDS_AVG_MODE_HS_ETH_1_10G:
		return "HS_ETH_1_10G";
	case AL_SRDS_AVG_MODE_HS_ETH_2_10G:
		return "HS_ETH_2_10G";
	case AL_SRDS_AVG_MODE_HS_ETH_3_10G:
		return "HS_ETH_3_10G";
	case AL_SRDS_AVG_MODE_HS_ETH_0_25G:
		return "HS_ETH_0_25G";
	case AL_SRDS_AVG_MODE_HS_ETH_1_25G:
		return "HS_ETH_1_25G";
	case AL_SRDS_AVG_MODE_HS_ETH_2_25G:
		return "HS_ETH_2_25G";
	case AL_SRDS_AVG_MODE_HS_ETH_3_25G:
		return "HS_ETH_3_25G";
	case AL_SRDS_AVG_MODE_HS_ETH_0_50G:
		return "HS_ETH_0_50G";
	case AL_SRDS_AVG_MODE_HS_ETH_2_50G:
		return "HS_ETH_2_50G";
	case AL_SRDS_AVG_MODE_HS_ETH_0_100G:
		return "HS_ETH_0_100G";
	}

	al_mod_err("%s: invalid value (%d)!\n", __func__, mode);

	return "";
}
#endif

/******************************************************************************/
/******************************************************************************/
#if (AL_SERDES_ERR_ONLY == 0)
static const char *al_mod_serdes_avg_clk_freq_to_str(
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
#endif

/******************************************************************************/
/******************************************************************************/
#if (AL_SERDES_ERR_ONLY == 0)
static const char *al_mod_serdes_avg_complex_type_to_str(
	enum al_mod_serdes_complex_type type)
{
	switch (type) {
	case AL_SRDS_COMPLEX_TYPE_TNK_MAIN:
		return "TNK_MAIN";
	case AL_SRDS_COMPLEX_TYPE_TR_TC_MAIN:
		return "TR_TC_MAIN";
	case AL_SRDS_COMPLEX_TYPE_TR_MAIN:
		return "TR_MAIN";
	case AL_SRDS_COMPLEX_TYPE_TR_SEC:
		return "TR_SEC";
	case AL_SRDS_COMPLEX_TYPE_TR_HS:
		return "TR_HS";
	}

	al_mod_err("%s: intypeid type (%d)!\n", __func__, type);

	return "";
}
#endif

/******************************************************************************/
/******************************************************************************/
#define CMPLX_CFG_PRINT(...)		AL_INFO(__VA_ARGS__)

/******************************************************************************/
/******************************************************************************/
static void al_mod_serdes_avg_depth_print(
	int				depth)
{
	int i;

	for (i = 0; i < depth; i++)
		CMPLX_CFG_PRINT("\t");
}

/******************************************************************************/
/******************************************************************************/
static void al_mod_serdes_avg_lane_cfg_print(
	struct al_mod_serdes_avg_lane_cfg	*cfg,
	int				depth)
{
	al_mod_serdes_avg_depth_print(depth);
	CMPLX_CFG_PRINT("mode = %s\n", al_mod_serdes_avg_mode_to_str(cfg->mode));
	al_mod_serdes_avg_depth_print(depth);
	CMPLX_CFG_PRINT("ref_clk_sel = %d\n", cfg->ref_clk_sel);
	al_mod_serdes_avg_depth_print(depth);
	CMPLX_CFG_PRINT("ref_clk_freq = %s\n", al_mod_serdes_avg_clk_freq_to_str(cfg->ref_clk_freq));
}

/******************************************************************************/
/******************************************************************************/
static void al_mod_serdes_avg_complex_cfg_print(
	struct al_mod_serdes_avg_complex_cfg	*cfg,
	int					depth)
{
	unsigned int i;

	al_mod_serdes_avg_depth_print(depth);
	CMPLX_CFG_PRINT("type = %s\n",
			al_mod_serdes_avg_complex_type_to_str(cfg->complex_obj->complex_type));
	al_mod_serdes_avg_depth_print(depth);
	CMPLX_CFG_PRINT("lanes:\n");
	for (i = 0; i < cfg->num_lanes; i++) {
		al_mod_serdes_avg_depth_print(depth + 1);
		CMPLX_CFG_PRINT("lane %d:\n", i);
		al_mod_serdes_avg_lane_cfg_print(&cfg->lane_cfg[i], depth + 2);
	}
}

int al_mod_serdes_avg_init(
	struct al_mod_serdes_avg_init_params *init_params)
{
	struct al_mod_serdes_avg_complex_cfg *complex_cfgs;
	unsigned int num_complexes;
	al_mod_bool fw_pcie_download;
	const uint32_t *fw_pcie;
	unsigned int fw_pcie_size;
	al_mod_bool fw_non_pcie_download;
	const uint32_t *fw_non_pcie;
	unsigned int fw_non_pcie_size;
	al_mod_bool sbus_master_rom_download;
	al_mod_bool wv = AL_FALSE;
	unsigned int i;

	al_mod_assert(init_params);

	complex_cfgs = init_params->complex_cfgs;
	num_complexes = init_params->num_complexes;
	fw_pcie_download = init_params->fw_pcie_download;
	fw_pcie = init_params->fw_pcie;
	fw_pcie_size = init_params->fw_pcie_size;
	fw_non_pcie_download = init_params->fw_non_pcie_download;
	fw_non_pcie = init_params->fw_non_pcie;
	fw_non_pcie_size = init_params->fw_non_pcie_size;
	sbus_master_rom_download = init_params->sbus_master_rom_download;

	al_mod_assert(complex_cfgs);
	al_mod_assert(num_complexes >= 1);
	for (i = 0; i < num_complexes; i++) {
		al_mod_assert(complex_cfgs[i].complex_obj);
		al_mod_assert(complex_cfgs[i].lane_cfg);
	}

	/* Print initialization params */
	for (i = 0; i < num_complexes; i++) {
		al_mod_serdes_avg_complex_cfg_print(&complex_cfgs[i], 0);
		CMPLX_CFG_PRINT("fw_pcie_download = %d\n", fw_pcie_download);
		CMPLX_CFG_PRINT("fw_non_pcie_download = %d\n", fw_non_pcie_download);
	}

	for (i = 0; i < num_complexes; i++) {
		struct al_mod_serdes_complex_obj *complex_obj = complex_cfgs[i].complex_obj;
		struct al_mod_sbus_obj *sbus = &complex_obj->sbus_obj;
		struct al_mod_serdes_avg_lane_cfg *lane_cfg = complex_cfgs[i].lane_cfg;
		unsigned int num_lanes = complex_cfgs[i].num_lanes;
		unsigned int j;
		int err;
		al_mod_bool all_skipped = AL_TRUE;

		for (j = 0; j < AL_SRDS_LANES_PER_COMPLEX_MAX; j++) {
			struct al_mod_serdes_grp_obj *lane_obj = complex_obj->lanes[j];
			if (lane_obj && (lane_cfg[j].mode != AL_SRDS_AVG_MODE_SKIP)) {
				all_skipped = AL_FALSE;
				break;
			}
		}

		if (all_skipped)
			continue;

		al_mod_serdes_avg_complex_config_pre_sbus_reset(complex_obj, lane_cfg, num_lanes);

		/* De-assert SBUS master reset */
		al_mod_sbus_master_reset_deassert(sbus);

		/*
		* This delay is needed for simulation.
		* It enables the clock change to propagate before we move to backdoor mem load.
		*/
		al_mod_udelay(1);

		/*
		 * Addressing RMN: 8050
		 *
		 * RMN description:
		 * Sbus clock is need to run in slower than 1/2 freq of serdes ref-clock.
		 * the recommendation from Avago is that it should be at not higher than
		 * 1/2 ref clock, however when ppms enabled on ref clock this can cause it
		 * to be fatser than that.
		 *
		 * Software flow:
		 * Change sbus master clock to 1/2 of ref clock
		 */
		al_mod_sbus_master_clk_out_div_set(sbus, 1);

		if (sbus_master_rom_download) {
			int err = al_mod_sbus_rom_download(sbus);
			if (err) {
				al_mod_err("%s: al_mod_sbus_rom_download failed!\n", __func__);
				return err;
			}
		}

		/* Change the 2nd broadcast address of all PCIe / Non PCIe SerDes modules */
		for (j = 0; j < AL_SRDS_LANES_PER_COMPLEX_MAX; j++) {
			struct al_mod_serdes_grp_obj *lane_obj = complex_obj->lanes[j];
			if (lane_obj && (lane_cfg[j].mode != AL_SRDS_AVG_MODE_SKIP)) {
				unsigned int receiver_addr = al_mod_serdes_avg_sbus_addr_serdes(
					complex_obj->complex_type, j);
				al_mod_bool is_pcie = al_mod_serdes_avg_lane_cfg_is_pcie(&lane_cfg[j]);
				unsigned int bcast_addr_1 = AL_SBUS_ADDR_BCAST(AL_SBUS_ADDR_SERDES);
				unsigned int bcast_addr_2 = is_pcie ?
					AL_SBUS_ADDR_BCAST_PCIE : AL_SBUS_ADDR_BCAST_NON_PCIE;

				err = al_mod_sbus_write(sbus, receiver_addr, 0xfd,
					(bcast_addr_2 << 16) | (bcast_addr_1 << 4), wv);
				if (err) {
					al_mod_err("%s: al_mod_sbus_write failed!\n", __func__);
					return err;
				}
			}
		}

		if (fw_pcie_download) {
			if ((!fw_pcie) && (!init_params->fw_pcie_rd_cb)) {
#if (AL_SERDES_EXTERNAL_FW == 0) && (AL_SERDES_EXTERNAL_FW_PCIE == 0)
				fw_pcie = al_mod_serdes_avg_fw_pcie;
				fw_pcie_size = AL_ARR_SIZE(al_mod_serdes_avg_fw_pcie);
#else
				al_mod_err("%s: PCIe FW not provided!\n", __func__);
#endif
			}
			err = al_mod_serdes_avg_fw_init_specific(
				complex_obj, (const uint32_t *)fw_pcie,
				fw_pcie_size,
				init_params->fw_pcie_rd_cb,
				init_params->fw_pcie_temp_buff,
				init_params->fw_pcie_temp_buff_size,
				AL_SBUS_ADDR_BCAST_PCIE, wv);
			if (err) {
				al_mod_err("%s: al_mod_serdes_avg_fw_init_specific(pcie) failed!\n",
					__func__);
				return err;
			}
		}

		if (fw_non_pcie_download) {
			if (!fw_non_pcie) {
#if (AL_SERDES_EXTERNAL_FW == 0) && (AL_SERDES_EXTERNAL_FW_NON_PCIE == 0)
				fw_non_pcie = al_mod_serdes_avg_fw_non_pcie;
				fw_non_pcie_size = AL_ARR_SIZE(al_mod_serdes_avg_fw_non_pcie);
#else
				al_mod_err("%s: Non PCIe FW not provided!\n", __func__);
#endif
			}
			err = al_mod_serdes_avg_fw_init_specific(
				complex_obj, (const uint32_t *)fw_non_pcie,
				fw_non_pcie_size,
				NULL, NULL, 0,
				AL_SBUS_ADDR_BCAST_NON_PCIE, wv);
			if (err) {
				al_mod_err("%s: al_mod_serdes_avg_fw_init_specific(non pcie) failed!\n",
					__func__);
				return err;
			}
		}

		/* Restore broadcast address */
		for (j = 0; j < AL_SRDS_LANES_PER_COMPLEX_MAX; j++) {
			struct al_mod_serdes_grp_obj *lane_obj = complex_obj->lanes[j];
			if (lane_obj && (lane_cfg[j].mode != AL_SRDS_AVG_MODE_SKIP)) {
				unsigned int receiver_addr = al_mod_serdes_avg_sbus_addr_serdes(
					complex_obj->complex_type, j);
				unsigned int bcast_addr_1 = AL_SBUS_ADDR_BCAST(AL_SBUS_ADDR_SERDES);
				unsigned int bcast_addr_2 = AL_SBUS_ADDR_BCAST(AL_SBUS_ADDR_SERDES);

				err = al_mod_sbus_write(sbus, receiver_addr, 0xfd,
					(bcast_addr_2 << 16) | (bcast_addr_1 << 4), wv);
				if (err) {
					al_mod_err("%s: al_mod_sbus_write failed!\n", __func__);
					return err;
				}
			}
		}

		for (j = 0; j < AL_SRDS_LANES_PER_COMPLEX_MAX; j++) {
			struct al_mod_serdes_grp_obj *lane_obj = complex_obj->lanes[j];

			if (lane_obj && (lane_cfg[j].mode != AL_SRDS_AVG_MODE_SKIP)) {
				al_mod_bool is_pcie = al_mod_serdes_avg_lane_cfg_is_pcie(&lane_cfg[j]);

				al_mod_serdes_avg_wait_for_o_core_status_indication(lane_obj);
				if (!is_pcie) {
					struct al_mod_serdes_avg_fw_init_status fw_init_status;

					lane_obj->fw_init_status_get(lane_obj, &fw_init_status);
					if (fw_init_status.crc_err) {
						al_mod_err("Complex %d lane %u FW CRC error!\n", i, j);
						return -EIO;
					}
				}
			}
		}

		AL_INFO("Complex %d FW download done via Core Interface\n", i);

		err = al_mod_serdes_avg_complex_config(complex_obj, lane_cfg, num_lanes);
		if (err) {
			al_mod_err("%s: al_mod_serdes_avg_complex_config failed for complex %d\n",
				__func__, i);
			return err;
		}
	}

	return 0;
}

void al_mod_serdes_avg_lane_cfg_retrieve(
	struct al_mod_serdes_grp_obj		*obj,
	struct al_mod_serdes_avg_lane_cfg_retrieve	*retrieve)
{
	struct al_mod_srd_1lane_regs __iomem *lane_regs;
	uint32_t reg_val;

	al_mod_assert(obj);
	al_mod_assert(retrieve);

	lane_regs = obj->regs_base;

	reg_val = obj->complex_obj->reg_read32(&lane_regs->ctl[0].sw_scratch);

	retrieve->mode = AL_REG_FIELD_GET(reg_val,
		SCRATCH_REG_LANE_CFG_MODE_MASK,
		SCRATCH_REG_LANE_CFG_MODE_SHIFT);
	retrieve->ref_clk_sel = AL_REG_FIELD_GET(reg_val,
		SCRATCH_REG_LANE_CFG_REF_CLK_FREQ_MASK,
		SCRATCH_REG_LANE_CFG_REF_CLK_FREQ_SHIFT);
	retrieve->bit_rate = AL_REG_FIELD_GET(reg_val,
		SCRATCH_REG_LANE_CFG_BIT_RATE_MASK,
		SCRATCH_REG_LANE_CFG_BIT_RATE_SHIFT);
	retrieve->bit_width = AL_REG_FIELD_GET(reg_val,
		SCRATCH_REG_LANE_CFG_BIT_WIDTH_MASK,
		SCRATCH_REG_LANE_CFG_BIT_WIDTH_SHIFT);
}
