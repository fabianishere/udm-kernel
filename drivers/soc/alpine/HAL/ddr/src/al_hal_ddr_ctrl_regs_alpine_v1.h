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
 * @file   al_hal_ddr_ctrl_regs_alpine_v1.h
 *
 * @brief  DDR controller registers for alpine V1
 *
 */

#ifndef __AL_HAL_DDR_CTRL_REGS_ALPINE_V1_H__
#define __AL_HAL_DDR_CTRL_REGS_ALPINE_V1_H__

#include "al_hal_plat_types.h"

#ifdef __cplusplus
extern "C" {
#endif
/*
* Unit Registers
*/

#define CTRL_REG(field) ((ddr_cfg->rev == AL_DDR_REV_ID_ALPINE_V1) ? \
	(&ctrl_regs->alpine_v1.umctl2_regs.field) : \
	(ddr_cfg->rev == AL_DDR_REV_ID_ALPINE_V3) ? \
	(&ctrl_regs->alpine_v3.umctl2_regs.field) : \
	(&ctrl_regs->alpine_v2.umctl2_regs.field))

struct al_dwc_ddr_umctl2_regs_alpine_v1 {
	uint32_t mstr;                  /* Master Register */
	uint32_t stat;                  /* Operating Mode Status Regi ... */
	uint32_t rsrvd_0[2];
	uint32_t mrctrl0;               /* Mode Register Read/Write C ... */
	uint32_t mrctrl1;               /* Mode Register Read/Write C ... */
	uint32_t mrstat;                /* Mode Register Read/Write S ... */
	uint32_t rsrvd_1[5];
	uint32_t pwrctl;                /* Low Power Control Register ... */
	uint32_t pwrtmg;                /* Low Power Timing Register */
	uint32_t rsrvd_2[6];
	uint32_t rfshctl0;              /* Refresh Control Register 0 ... */
	uint32_t rfshctl1;              /* Refresh Control Register 1 ... */
	uint32_t rfshctl2;              /* Refresh Control Register 2 ... */
	uint32_t rsrvd_3;
	uint32_t rfshctl3;              /* Refresh Control Register 0 ... */
	uint32_t rfshtmg;               /* Refresh Timing Register */
	uint32_t rsrvd_4[2];
	uint32_t ecccfg0;               /* ECC Configuration Register ... */
	uint32_t ecccfg1;               /* ECC Configuration Register ... */
	uint32_t eccstat;               /* ECC Status Register */
	uint32_t eccclr;                /* ECC Clear Register */
	uint32_t eccerrcnt;             /* ECC Error Counter Register ... */
	uint32_t ecccaddr0;             /* ECC Corrected Error Addres ... */
	uint32_t ecccaddr1;             /* ECC Corrected Error Addres ... */
	uint32_t ecccsyn0;              /* ECC Corrected Syndrome Reg ... */
	uint32_t ecccsyn1;              /* ECC Corrected Syndrome Reg ... */
	uint32_t ecccsyn2;              /* ECC Corrected Syndrome Reg ... */
	uint32_t eccbitmask0;           /* ECC Corrected Data Bit Mas ... */
	uint32_t eccbitmask1;           /* ECC Corrected Data Bit Mas ... */
	uint32_t eccbitmask2;           /* ECC Corrected Data Bit Mas ... */
	uint32_t eccuaddr0;             /* ECC Uncorrected Error Addr ... */
	uint32_t eccuaddr1;             /* ECC Unorrected Error Addre ... */
	uint32_t eccusyn0;              /* ECC Unorrected Syndrome Re ... */
	uint32_t eccusyn1;              /* ECC Uncorrected Syndrome R ... */
	uint32_t eccusyn2;              /* ECC Uncorrected Syndrome R ... */
	uint32_t eccpoisonaddr0;        /* ECC Data Poisoning Address ... */
	uint32_t eccpoisonaddr1;        /* ECC Data Poisoning Address ... */
	uint32_t parctl;                /* Parity Control Register */
	uint32_t parstat;               /* Parity Status Register */
	uint32_t rsrvd_5[2];
	uint32_t init0;                 /* SDRAM Initialization Regis ... */
	uint32_t init1;                 /* SDRAM Initialization Regis ... */
	uint32_t rsrvd_6;
	uint32_t init3;                 /* SDRAM Initialization Regis ... */
	uint32_t init4;                 /* SDRAM Initialization Regis ... */
	uint32_t init5;                 /* SDRAM Initialization Regis ... */
	uint32_t rsrvd_7[2];
	uint32_t dimmctl;               /* DIMM Control Register */
	uint32_t rankctl;               /* Rank Control Register */
	uint32_t rsrvd_8[2];
	uint32_t dramtmg0;              /* SDRAM Timing Register 0 */
	uint32_t dramtmg1;              /* SDRAM Timing Register 1 */
	uint32_t dramtmg2;              /* SDRAM Timing Register 2 */
	uint32_t dramtmg3;              /* SDRAM Timing Register 3 */
	uint32_t dramtmg4;              /* SDRAM Timing Register 4 */
	uint32_t dramtmg5;              /* SDRAM Timing Register 5 */
	uint32_t rsrvd_9[2];
	uint32_t dramtmg8;              /* SDRAM Timing Register 8 */
	uint32_t rsrvd_10[23];
	uint32_t zqctl0;                /* ZQ Control Register 0 */
	uint32_t zqctl1;                /* ZQ Control Register 1 */
	uint32_t rsrvd_11[2];
	uint32_t dfitmg0;               /* DFI Timing Register 0 */
	uint32_t dfitmg1;               /* DFI Timing Register 1 */
	uint32_t rsrvd_12[2];
	uint32_t dfiupd0;               /* DFI Update Register 0 */
	uint32_t dfiupd1;               /* DFI Update Register 1 */
	uint32_t dfiupd2;               /* DFI Update Register 2 */
	uint32_t dfiupd3;               /* DFI Update Register 3 */
	uint32_t dfimisc;               /* DFI Miscellaneous Control  ... */
	uint32_t rsrvd_13[19];
	uint32_t addrmap0;              /* Address Map Register 0 */
	uint32_t addrmap1;              /* Address Map Register 1 */
	uint32_t addrmap2;              /* Address Map Register 2 */
	uint32_t addrmap3;              /* Address Map Register 3 */
	uint32_t addrmap4;              /* Address Map Register 4 */
	uint32_t addrmap5;              /* Address Map Register 5 */
	uint32_t addrmap6;              /* Address Map Register 6 */
	uint32_t rsrvd_14[9];
	uint32_t odtcfg;                /* ODT Configuration Register ... */
	uint32_t odtmap;                /* ODT/Rank Map Register */
	uint32_t rsrvd_15[2];
	uint32_t sched;                 /* Scheduler Control Register ... */
	uint32_t rsrvd_16;
	uint32_t perfhpr0;              /* High Priority Read CAM Reg ... */
	uint32_t perfhpr1;              /* High Priority Read CAM Reg ... */
	uint32_t perflpr0;              /* Low Priority Read CAM Regi ... */
	uint32_t perflpr1;              /* Low Priority Read CAM Regi ... */
	uint32_t perfwr0;               /* Write CAM Register 0 */
	uint32_t perfwr1;               /* Write CAM Register 1 */
	uint32_t rsrvd_17[36];
	uint32_t dbg0;                  /* Debug Register 0 */
	uint32_t dbg1;                  /* Debug Register 1 */
	uint32_t dbgcam;                /* CAM Debug Register */
	uint32_t rsrvd[61];
};

struct al_dwc_ddr_umctl2_mp_alpine_v1 {
		uint32_t pccfg;                 /* Port Common Configuration  ... */
		uint32_t pcfgr_0;               /* Port 0 Configuration Read  ... */
		uint32_t pcfgw_0;               /* Port 0 Configuration Write ... */
		uint32_t pcfgidmaskch0_0;       /* Port 0 Channel 0 Configura ... */
		uint32_t pcfgidvaluech0_0;      /* Port 0 Channel 0 Configura ... */
		uint32_t rsrvd[1787];
};

struct al_ddr_ctrl_regs_alpine_v1 {
	struct al_dwc_ddr_umctl2_regs_alpine_v1 umctl2_regs;
	struct al_dwc_ddr_umctl2_mp_alpine_v1 umctl2_mp;
};

#ifdef __cplusplus
}
#endif

#endif /* __AL_HAL_DDR_CTRL_REGS_ALPINE_V1_H__ */

/** @} end of DDR group */


