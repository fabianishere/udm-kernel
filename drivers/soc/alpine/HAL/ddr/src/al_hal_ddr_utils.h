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
 * @file   al_hal_ddr_utils.h
 *
 */

#ifndef __AL_HAL_DDR_UTILS_H__
#define __AL_HAL_DDR_UTILS_H__

#include "al_hal_common.h"
#include "al_hal_ddr_ctrl_regs_common.h"
#include "al_hal_ddr_cfg.h"
#include "al_hal_reg_utils.h"

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

/* Default timeout for register polling operations */
#define DEFAULT_TIMEOUT		5000

#define AL_DDR_PHY_VERSION_ALPINE_V1	0x00120120
#define AL_DDR_PHY_VERSION_ALPINE_V2	0x00250231
#define AL_DDR_PHY_VERSION_ALPINE_V3_TC	0x00100233
#define AL_DDR_PHY_VERSION_ALPINE_V3	0x00110310

#if (!defined(AL_DEV_ID)) || (AL_DEV_ID == AL_DEV_ID_ALPINE_V1)
#define NB_DEBUG_CPUS_GENERAL_ADDR_MAP_ECO (1 << 23)
#endif

/*******************************************************************************
 ******************************************************************************/
static int al_ddr_reg_poll32(
	uint32_t __iomem	*reg,
	uint32_t		mask,
	uint32_t		data,
	unsigned int		timeout)
{


	while ((al_reg_read32(reg) & mask) != data) {

		if (timeout) {
			al_udelay(1);
			timeout--;
		} else {
			return -ETIME;
		}
	}

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
/* Wait for controller normal operating mode */
static int al_ddr_ctrl_wait_for_normal_operating_mode(
	struct al_ddr_cfg	*ddr_cfg)
{
	union al_ddr_ctrl_regs __iomem *ctrl_regs =
		(union al_ddr_ctrl_regs __iomem *)
		ddr_cfg->ddr_ctrl_regs_base;

	int err;

	err = al_ddr_reg_poll32(
		CTRL_REG(stat),
		DWC_DDR_UMCTL2_REGS_STAT_OPERATING_MODE_MASK,
		DWC_DDR_UMCTL2_REGS_STAT_OPERATING_MODE_NORMAL,
		DEFAULT_TIMEOUT);

	if (err) {
		al_err("%s: al_ddr_reg_poll32 failed!\n", __func__);
		return err;
	}

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
/* Stop DDR controller access to the PHY*/
static inline void al_ddr_ctrl_stop(
	struct al_ddr_cfg	*ddr_cfg)
{
	union al_ddr_ctrl_regs __iomem *ctrl_regs =
		(union al_ddr_ctrl_regs __iomem *)
		ddr_cfg->ddr_ctrl_regs_base;

	al_reg_write32_masked(
		CTRL_REG(dfimisc),
		DWC_DDR_UMCTL2_REGS_DFIMISC_DFI_INIT_COMPLETE_EN,
		0);

	/* Stop controller refresh and ZQ calibration commands */
	al_reg_write32_masked(
		CTRL_REG(rfshctl3),
		DWC_DDR_UMCTL2_REGS_RFSHCTL3_DIS_AUTO_REFRESH,
		DWC_DDR_UMCTL2_REGS_RFSHCTL3_DIS_AUTO_REFRESH);

	al_reg_write32_masked(
		CTRL_REG(zqctl0),
		DWC_DDR_UMCTL2_REGS_ZQCTL0_DIS_AUTO_ZQ,
		DWC_DDR_UMCTL2_REGS_ZQCTL0_DIS_AUTO_ZQ);

	al_data_memory_barrier();

	/* Wait for controller to issue all pending refresh commands */
	al_udelay(5);

}

/*******************************************************************************
 ******************************************************************************/
/* Resume DDR controller access to the PHY*/
static inline void al_ddr_ctrl_resume(
	struct al_ddr_cfg	*ddr_cfg)
{
	union al_ddr_ctrl_regs __iomem *ctrl_regs =
		(union al_ddr_ctrl_regs __iomem *)
		ddr_cfg->ddr_ctrl_regs_base;

	al_reg_write32_masked(
		CTRL_REG(dfimisc),
		DWC_DDR_UMCTL2_REGS_DFIMISC_DFI_INIT_COMPLETE_EN,
		DWC_DDR_UMCTL2_REGS_DFIMISC_DFI_INIT_COMPLETE_EN);

	/* Resume controller refresh and ZQ calibration commands */
	al_reg_write32_masked(
		CTRL_REG(rfshctl3),
		DWC_DDR_UMCTL2_REGS_RFSHCTL3_DIS_AUTO_REFRESH,
		0);

	al_reg_write32_masked(
		CTRL_REG(zqctl0),
		DWC_DDR_UMCTL2_REGS_ZQCTL0_DIS_AUTO_ZQ,
		0);

	al_data_memory_barrier();

	al_ddr_ctrl_wait_for_normal_operating_mode(ddr_cfg);
}

/*******************************************************************************
 ******************************************************************************/
/* Disable read write access to the DDR*/
static inline int al_ddr_ctrl_read_write_disable(
	struct al_ddr_cfg	*ddr_cfg)
{
	union al_ddr_ctrl_regs __iomem *ctrl_regs =
		(union al_ddr_ctrl_regs __iomem *)
		ddr_cfg->ddr_ctrl_regs_base;
	int err;

	al_reg_write32_masked(
		CTRL_REG(dbg1),
		DWC_DDR_UMCTL2_REGS_DBG1_DIS_DQ,
		DWC_DDR_UMCTL2_REGS_DBG1_DIS_DQ);

	al_data_memory_barrier();

	err = al_ddr_reg_poll32(
		CTRL_REG(dbgcam),
		(DWC_DDR_UMCTL2_REGS_DBGCAM_WR_DATA_PIPELINE_EMPTY |
			DWC_DDR_UMCTL2_REGS_DBGCAM_RD_DATA_PIPELINE_EMPTY),
		(DWC_DDR_UMCTL2_REGS_DBGCAM_WR_DATA_PIPELINE_EMPTY |
			DWC_DDR_UMCTL2_REGS_DBGCAM_RD_DATA_PIPELINE_EMPTY),
		DEFAULT_TIMEOUT);

	if (err) {
		al_err("%s: al_ddr_reg_poll32 failed!\n", __func__);
		return err;
	}

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
/* Enable read write access to the DDR*/
static inline void al_ddr_ctrl_read_write_enable(
	struct al_ddr_cfg	*ddr_cfg)
{
	union al_ddr_ctrl_regs __iomem *ctrl_regs =
		(union al_ddr_ctrl_regs __iomem *)
		ddr_cfg->ddr_ctrl_regs_base;

	/* re-enable reads/writes*/
	al_reg_write32_masked(
		CTRL_REG(dbg1),
		DWC_DDR_UMCTL2_REGS_DBG1_DIS_DQ,
		0);
}

/* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */
/** @} end of DDR group */
#endif
