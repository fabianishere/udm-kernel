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
#if defined(AL_DEV_ID)

#include "al_hal_cmos.h"
#include "al_hal_cmos_regs.h"
#include "al_hal_reg_utils.h"

/******************************************************************************
 ******************************************************************************/
int al_cmos_handle_init(
	struct al_cmos_handle	*handle,
	void __iomem		*cmos_reg_base)
{
	al_assert(handle);
	al_assert(cmos_reg_base);

	handle->regs = (struct al_cmos_regs __iomem *)cmos_reg_base;

	return 0;
}

/******************************************************************************
 ******************************************************************************/
void al_cmos_cfg_set(
	struct al_cmos_handle	*handle,
	unsigned int		group,
	unsigned int		data_mask,
	unsigned int		pullup)
{
	al_assert(handle);
	al_assert(group < AL_CMOS_NUM_GROUPS);

	al_reg_write32_masked(
		&handle->regs->groups[group].setup,
		AL_CMOS_REGS_SETUP_DATA_MASK_MASK |
		AL_CMOS_REGS_SETUP_PULLUP_EN_MASK,
		(data_mask << AL_CMOS_REGS_SETUP_DATA_MASK_SHIFT) |
		(pullup << AL_CMOS_REGS_SETUP_PULLUP_EN_SHIFT));
}

/******************************************************************************
 ******************************************************************************/
void al_cmos_cfg_get(
	struct al_cmos_handle	*handle,
	unsigned int		group,
	unsigned int		*data_mask,
	unsigned int		*pullup)
{
	uint32_t reg_val;

	al_assert(handle);
	al_assert(group < AL_CMOS_NUM_GROUPS);
	al_assert(data_mask);
	al_assert(pullup);

	reg_val = al_reg_read32(&handle->regs->groups[group].setup);

	*data_mask = (reg_val & AL_CMOS_REGS_SETUP_DATA_MASK_MASK) >>
		AL_CMOS_REGS_SETUP_DATA_MASK_SHIFT;

	*pullup = (reg_val & AL_CMOS_REGS_SETUP_PULLUP_EN_MASK) >>
		AL_CMOS_REGS_SETUP_PULLUP_EN_SHIFT;
}

/******************************************************************************
 ******************************************************************************/
#if (AL_DEV_ID >= AL_DEV_ID_ALPINE_V3)
void al_cmos_rgmii_buff_drv_strength_set(
	struct al_cmos_handle			*handle,
	enum al_cmos_rgmii			rgmii,
	enum al_cmos_buffer_drive_strength	val)
{
	unsigned int group;

	al_assert(handle);
	al_assert((rgmii == AL_CMOS_RGMII_A) || (rgmii == AL_CMOS_RGMII_B));

	group = (rgmii == AL_CMOS_RGMII_A) ?
		AL_CMOS_GROUP_RGMII_A : AL_CMOS_GROUP_RGMII_B;

	al_reg_write32(
		&handle->regs->groups[group].cmos_drv,
		AL_CMOS_REGS_CMOS_DRV_TYPE_VAL(val));
}
#else
void al_cmos_rgmii_buff_drv_strength_set(
	struct al_cmos_handle			*handle,
	enum al_cmos_rgmii			rgmii,
	enum al_cmos_buffer_drive_strength	val __attribute__((unused)))
{
	al_assert(handle);
	al_assert((rgmii == AL_CMOS_RGMII_A) || (rgmii == AL_CMOS_RGMII_B));

	al_err("%s: not supported!\n", __func__);
	al_assert(0);
}
#endif

/******************************************************************************
 ******************************************************************************/
void al_cmos_rgmii_ddr_enable_set(
	struct al_cmos_handle		*handle,
	enum al_cmos_rgmii		rgmii,
	al_bool				enable)
{
	unsigned int group;

	al_assert(handle);
	al_assert((rgmii == AL_CMOS_RGMII_A) || (rgmii == AL_CMOS_RGMII_B));

	group = (rgmii == AL_CMOS_RGMII_A) ?
		AL_CMOS_GROUP_RGMII_A : AL_CMOS_GROUP_RGMII_B;

	al_reg_write32_masked(
		&handle->regs->groups[group].ddr_setup,
		AL_CMOS_REGS_DDR_SETUP_DDR_EN,
		enable ? AL_CMOS_REGS_DDR_SETUP_DDR_EN : 0);
}

/******************************************************************************
 ******************************************************************************/
void al_cmos_rgmii_dll_enable_set(
	struct al_cmos_handle		*handle,
	enum al_cmos_rgmii		rgmii,
	al_bool				enable,
	al_bool				tx_relock_en,
	al_bool				rx_relock_en,
	enum al_cmos_rgmii_dll_clk	clk_sel)
{
	unsigned int group;

	al_assert(handle);
	al_assert((rgmii == AL_CMOS_RGMII_A) || (rgmii == AL_CMOS_RGMII_B));

	group = (rgmii == AL_CMOS_RGMII_A) ?
		AL_CMOS_GROUP_RGMII_A : AL_CMOS_GROUP_RGMII_B;

	al_reg_write32_masked(
		&handle->regs->groups[group].ddr_setup,
		AL_CMOS_REGS_DDR_SETUP_DLL_EN |
		AL_CMOS_REGS_DDR_SETUP_DLL_TX_RELOCK_EN |
		AL_CMOS_REGS_DDR_SETUP_DLL_RX_RELOCK_EN |
		AL_CMOS_REGS_DDR_SETUP_DLL_CLK_SEL,
		(enable ? AL_CMOS_REGS_DDR_SETUP_DLL_EN : 0) |
		(tx_relock_en ? AL_CMOS_REGS_DDR_SETUP_DLL_TX_RELOCK_EN : 0) |
		(rx_relock_en ? AL_CMOS_REGS_DDR_SETUP_DLL_RX_RELOCK_EN : 0) |
		((clk_sel == AL_CMOS_RGMII_DLL_CLK_TX) ?
		 AL_CMOS_REGS_DDR_SETUP_DLL_CLK_SEL : 0));
}

/******************************************************************************
 ******************************************************************************/
void al_cmos_rgmii_dll_start(
	struct al_cmos_handle		*handle,
	enum al_cmos_rgmii		rgmii,
	al_bool				offset_en,
	unsigned int			offset)
{
	unsigned int group;

	al_assert(handle);
	al_assert((rgmii == AL_CMOS_RGMII_A) || (rgmii == AL_CMOS_RGMII_B));

	group = (rgmii == AL_CMOS_RGMII_A) ?
		AL_CMOS_GROUP_RGMII_A : AL_CMOS_GROUP_RGMII_B;

	/*
	 * RMN #3327:
	 * In the RGMII DLL device there is a feature that allows configuring
	 * an early offset to the search mechanism.
	 * If this feature is activated after the first time the device was
	 * used, when the device was not cleared, then the offset insertion may
	 * fail the device activation and a false result may be received.
	 *
	 * Software flow:
	 * Clear previous results before using the offset mechanism
	 */
	if (offset_en)
		al_reg_write32_masked(
			&handle->regs->groups[group].ddr_setup,
			AL_CMOS_REGS_DDR_SETUP_DLL_CLEAR,
			AL_CMOS_REGS_DDR_SETUP_DLL_CLEAR);

	al_reg_write32_masked(
		&handle->regs->groups[group].ddr_setup,
		AL_CMOS_REGS_DDR_SETUP_DLL_OFFSET_EN |
		AL_CMOS_REGS_DDR_SETUP_DLL_OFFSET_MASK,
		(offset_en ? AL_CMOS_REGS_DDR_SETUP_DLL_OFFSET_EN : 0) |
		(offset << AL_CMOS_REGS_DDR_SETUP_DLL_OFFSET_SHIFT));

	al_reg_write32_masked(
		&handle->regs->groups[group].ddr_setup,
		AL_CMOS_REGS_DDR_SETUP_DLL_BUSY,
		AL_CMOS_REGS_DDR_SETUP_DLL_BUSY);
}

/******************************************************************************
 ******************************************************************************/
void al_cmos_rgmii_dll_failure_clear(
	struct al_cmos_handle		*handle,
	enum al_cmos_rgmii		rgmii)
{
	unsigned int group;

	al_assert(handle);
	al_assert((rgmii == AL_CMOS_RGMII_A) || (rgmii == AL_CMOS_RGMII_B));

	group = (rgmii == AL_CMOS_RGMII_A) ?
		AL_CMOS_GROUP_RGMII_A : AL_CMOS_GROUP_RGMII_B;

	al_reg_write32_masked(
		&handle->regs->groups[group].ddr_setup,
		AL_CMOS_REGS_DDR_SETUP_DLL_CLEAR,
		AL_CMOS_REGS_DDR_SETUP_DLL_CLEAR);
}

/******************************************************************************
 ******************************************************************************/
void al_cmos_rgmii_dll_status_get(
	struct al_cmos_handle		*handle,
	enum al_cmos_rgmii		rgmii,
	struct al_cmos_rgmii_dll_status	*status)
{
	unsigned int group;
	uint32_t reg_val;

	al_assert(handle);
	al_assert((rgmii == AL_CMOS_RGMII_A) || (rgmii == AL_CMOS_RGMII_B));
	al_assert(status);

	group = (rgmii == AL_CMOS_RGMII_A) ?
		AL_CMOS_GROUP_RGMII_A : AL_CMOS_GROUP_RGMII_B;

	reg_val = al_reg_read32(&handle->regs->groups[group].ddr_setup);

	status->err = (reg_val & AL_CMOS_REGS_DDR_SETUP_DLL_ERR) ?
	       AL_TRUE : AL_FALSE;

	status->done = (reg_val & AL_CMOS_REGS_DDR_SETUP_DLL_DONE) ?
	       AL_TRUE : AL_FALSE;

	status->busy = (reg_val & AL_CMOS_REGS_DDR_SETUP_DLL_BUSY) ?
	       AL_TRUE : AL_FALSE;

	status->sweep_result =
		(reg_val & AL_CMOS_REGS_DDR_SETUP_DLL_OFFSET_MASK) >>
		AL_CMOS_REGS_DDR_SETUP_DLL_OFFSET_SHIFT;
}

/******************************************************************************
 ******************************************************************************/
void al_cmos_rgmii_dll_delay_set(
	struct al_cmos_handle		*handle,
	enum al_cmos_rgmii		rgmii,
	unsigned int			tx_dly,
	unsigned int			rx_dly)
{
	unsigned int group;

	al_assert(handle);
	al_assert((rgmii == AL_CMOS_RGMII_A) || (rgmii == AL_CMOS_RGMII_B));

	group = (rgmii == AL_CMOS_RGMII_A) ?
		AL_CMOS_GROUP_RGMII_A : AL_CMOS_GROUP_RGMII_B;

	al_reg_write32_masked(
		&handle->regs->groups[group].ddr_setup,
		AL_CMOS_REGS_DDR_SETUP_TX_DLY_MASK |
		AL_CMOS_REGS_DDR_SETUP_RX_DLY_MASK,
		(tx_dly << AL_CMOS_REGS_DDR_SETUP_TX_DLY_SHIFT) |
		(rx_dly << AL_CMOS_REGS_DDR_SETUP_RX_DLY_SHIFT));
}

/******************************************************************************
 ******************************************************************************/
void al_cmos_io_bist_start(
	struct al_cmos_handle		*handle,
	enum al_cmos_rgmii		rgmii,
	struct al_cmos_io_bist_cfg	*cfg)
{
	unsigned int group;
	int i;

	al_assert(handle);
	al_assert((rgmii == AL_CMOS_RGMII_A) || (rgmii == AL_CMOS_RGMII_B));
	al_assert(cfg);

	group = (rgmii == AL_CMOS_RGMII_A) ?
		AL_CMOS_GROUP_RGMII_A : AL_CMOS_GROUP_RGMII_B;

	for (i = 0; i < 8; i += 2)
		al_reg_write32_masked(
			&handle->regs->groups[group].test_pattern[i / 2],
			AL_CMOS_REGS_TST_PAT_DAT_LOW_0_MASK |
			AL_CMOS_REGS_TST_PAT_DAT_HIGH_0_MASK |
			AL_CMOS_REGS_TST_PAT_DAT_LOW_1_MASK |
			AL_CMOS_REGS_TST_PAT_DAT_HIGH_1_MASK,
			(cfg->data_low_pattern[i] <<
			 AL_CMOS_REGS_TST_PAT_DAT_LOW_0_SHIFT) |
			(cfg->data_high_pattern[i] <<
			 AL_CMOS_REGS_TST_PAT_DAT_HIGH_0_SHIFT) |
			(cfg->data_low_pattern[i + 1] <<
			 AL_CMOS_REGS_TST_PAT_DAT_LOW_1_SHIFT) |
			(cfg->data_high_pattern[i + 1] <<
			 AL_CMOS_REGS_TST_PAT_DAT_HIGH_1_SHIFT));

	al_reg_write32_masked(
		&handle->regs->groups[group].io_bist_flow,
		AL_CMOS_REGS_IO_BIST_FLOW_RX_START |
		AL_CMOS_REGS_IO_BIST_FLOW_TX_START |
		AL_CMOS_REGS_IO_BIST_FLOW_RXH_MASK_MASK |
		AL_CMOS_REGS_IO_BIST_FLOW_RXL_MASK_MASK,
		(cfg->rx_en ? AL_CMOS_REGS_IO_BIST_FLOW_RX_START : 0) |
		(cfg->tx_en ? AL_CMOS_REGS_IO_BIST_FLOW_TX_START : 0) |
		(cfg->rxh_mask << AL_CMOS_REGS_IO_BIST_FLOW_RXH_MASK_SHIFT) |
		(cfg->rxl_mask << AL_CMOS_REGS_IO_BIST_FLOW_RXL_MASK_SHIFT));
}

/******************************************************************************
 ******************************************************************************/
void al_cmos_io_bist_status_get(
	struct al_cmos_handle		*handle,
	enum al_cmos_rgmii		rgmii,
	struct al_cmos_io_bist_status	*status)
{
	unsigned int group;
	uint32_t reg_val;

	al_assert(handle);
	al_assert((rgmii == AL_CMOS_RGMII_A) || (rgmii == AL_CMOS_RGMII_B));
	al_assert(status);

	group = (rgmii == AL_CMOS_RGMII_A) ?
		AL_CMOS_GROUP_RGMII_A : AL_CMOS_GROUP_RGMII_B;

	reg_val = al_reg_read32(&handle->regs->groups[group].io_bist_status);

	status->rx_test_valid =
		(reg_val & AL_CMOS_REGS_IO_BIST_STATUS_RX_TEST_VALID) ?
		AL_TRUE : AL_FALSE;

	status->rx_test_err_detected =
		(reg_val & AL_CMOS_REGS_IO_BIST_STATUS_RX_TEST_ERR) ?
		AL_TRUE : AL_FALSE;

	status->test_stage_preamble =
		(reg_val & AL_CMOS_REGS_IO_BIST_STATUS_RX_TEST_STAGE) ?
		AL_FALSE : AL_TRUE;

	status->test_vector_cnt =
		(reg_val & AL_CMOS_REGS_IO_BIST_STATUS_TEST_VEC_CNT_MASK) >>
		AL_CMOS_REGS_IO_BIST_STATUS_TEST_VEC_CNT_SHIFT;

	status->rxh_ref =
		(reg_val & AL_CMOS_REGS_IO_BIST_STATUS_RXH_REF_MASK) >>
		AL_CMOS_REGS_IO_BIST_STATUS_RXH_REF_SHIFT;

	status->rxl_ref =
		(reg_val & AL_CMOS_REGS_IO_BIST_STATUS_RXL_REF_MASK) >>
		AL_CMOS_REGS_IO_BIST_STATUS_RXL_REF_SHIFT;

	status->rxh_in =
		(reg_val & AL_CMOS_REGS_IO_BIST_STATUS_RXH_IN_MASK) >>
		AL_CMOS_REGS_IO_BIST_STATUS_RXH_IN_SHIFT;

	status->rxl_in =
		(reg_val & AL_CMOS_REGS_IO_BIST_STATUS_RXL_IN_MASK) >>
		AL_CMOS_REGS_IO_BIST_STATUS_RXL_IN_SHIFT;
}

/******************************************************************************
 ******************************************************************************/
void al_cmos_nb_cpu_clk_out_en(
	struct al_cmos_handle	*handle,
	al_bool			en)
{
	al_assert(handle);

	al_reg_write32_masked(
		&handle->regs->groups[AL_CMOS_GROUP_NB_CPU_CLK_OUT_EN].setup,
		AL_CMOS_REGS_SETUP_NB_CPU_CLK_OUT_EN,
		en ? AL_CMOS_REGS_SETUP_NB_CPU_CLK_OUT_EN : 0);
}

#endif

