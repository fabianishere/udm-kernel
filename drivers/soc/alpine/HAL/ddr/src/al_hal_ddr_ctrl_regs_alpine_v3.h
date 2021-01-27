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
 * @file   al_hal_ddr_ctrl_regs_alpine_v3.h
 *
 * @brief  DDR controller registers for alpine V3
 *
 */

#ifndef __AL_HAL_DDR_CTRL_REGS_ALPINE_V3_H__
#define __AL_HAL_DDR_CTRL_REGS_ALPINE_V3_H__

#include "al_hal_plat_types.h"

#ifdef __cplusplus
extern "C" {
#endif
/*
* Unit Registers
*/

struct al_dwc_ddr_umctl2_regs_alpine_v3 {
	/* [0x0] Master Register0 */
	uint32_t mstr;
	/* [0x4] Operating Mode Status Register */
	uint32_t stat;
	/* [0x8] Master Register1 */
	uint32_t mstr1;
	uint32_t rsrvd_0;
	/* [0x10] Mode Register Read/Write Control Register 0. */
	uint32_t mrctrl0;
	/* [0x14] Mode Register Read/Write Control Register 1 */
	uint32_t mrctrl1;
	/* [0x18] Mode Register Read/Write Status Register */
	uint32_t mrstat;
	/* [0x1c] Mode Register Read/Write Control Register 2 */
	uint32_t mrctrl2;
	uint32_t rsrvd_1[4];
	/* [0x30] Low Power Control Register */
	uint32_t pwrctl;
	/* [0x34] Low Power Timing Register */
	uint32_t pwrtmg;
	/* [0x38] Hardware Low Power Control Register */
	uint32_t hwlpctl;
	uint32_t rsrvd_2[5];
	/* [0x50] Refresh Control Register 0 */
	uint32_t rfshctl0;
	/* [0x54] Refresh Control Register 1 */
	uint32_t rfshctl1;
	/* [0x58] Refresh Control Register 2 */
	uint32_t rfshctl2;
	/* [0x5c] Refresh Control Register 4 */
	uint32_t rfshctl4;
	/* [0x60] Refresh Control Register 3 */
	uint32_t rfshctl3;
	/* [0x64] Refresh Timing Register */
	uint32_t rfshtmg;
	/* [0x68] Refresh Timing Register1 */
	uint32_t rfshtmg1;
	uint32_t rsrvd_3;
	/* [0x70] ECC Configuration Register 0 */
	uint32_t ecccfg0;
	/* [0x74] ECC Configuration Register 1 */
	uint32_t ecccfg1;
	/*
	 * [0x78] SECDED ECC Status Register (Valid only in MEMC_ECC_SUPPORT==1 (SECDED ECC mode))
	 */
	uint32_t eccstat;
	/* [0x7c] ECC Clear Register */
	uint32_t eccclr;
	/* [0x80] ECC Error Counter Register */
	uint32_t eccerrcnt;
	/* [0x84] ECC Corrected Error Address Register 0 */
	uint32_t ecccaddr0;
	/* [0x88] ECC Corrected Error Address Register 1 */
	uint32_t ecccaddr1;
	/* [0x8c] ECC Corrected Syndrome Register 0 */
	uint32_t ecccsyn0;
	/* [0x90] ECC Corrected Syndrome Register 1 */
	uint32_t ecccsyn1;
	/* [0x94] ECC Corrected Syndrome Register 2 */
	uint32_t ecccsyn2;
	/* [0x98] ECC Corrected Data Bit Mask Register 0 */
	uint32_t eccbitmask0;
	/* [0x9c] ECC Corrected Data Bit Mask Register 1 */
	uint32_t eccbitmask1;
	/* [0xa0] ECC Corrected Data Bit Mask Register 2 */
	uint32_t eccbitmask2;
	/* [0xa4] ECC Uncorrected Error Address Register 0 */
	uint32_t eccuaddr0;
	/* [0xa8] ECC Uncorrected Error Address Register 1 */
	uint32_t eccuaddr1;
	/* [0xac] ECC Uncorrected Syndrome Register 0 */
	uint32_t eccusyn0;
	/* [0xb0] ECC Uncorrected Syndrome Register 1 */
	uint32_t eccusyn1;
	/* [0xb4] ECC Uncorrected Syndrome Register 2 */
	uint32_t eccusyn2;
	/*
	 * [0xb8] ECC Data Poisoning Address Register 0. If a HIF write data beat matches the
	 * address specified in this register, an ECC error will be introduced on that transaction
	 * (write/RMW), if ECCCFG1.data_poison_en=1
	 */
	uint32_t eccpoisonaddr0;
	/*
	 * [0xbc] ECC Data Poisoning Address Register 1. If a HIF write data beat matches the
	 * address specified in this register, an ECC error will be introduced on that transaction
	 * (write/RMW), if ECCCFG1.data_poison_en=1
	 */
	uint32_t eccpoisonaddr1;
	/* [0xc0] CRC Parity Control Register0 */
	uint32_t crcparctl0;
	/* [0xc4] CRC Parity Control Register1 */
	uint32_t crcparctl1;
	/* [0xc8] CRC Parity Control Register2 */
	uint32_t crcparctl2;
	/* [0xcc] CRC Parity Status Register */
	uint32_t crcparstat;
	/* [0xd0] SDRAM Initialization Register 0 */
	uint32_t init0;
	/* [0xd4] SDRAM Initialization Register 1 */
	uint32_t init1;
	uint32_t rsrvd_4;
	/* [0xdc] SDRAM Initialization Register 3 */
	uint32_t init3;
	/* [0xe0] SDRAM Initialization Register 4 */
	uint32_t init4;
	/* [0xe4] SDRAM Initialization Register 5 */
	uint32_t init5;
	/* [0xe8] SDRAM Initialization Register 6 */
	uint32_t init6;
	/* [0xec] SDRAM Initialization Register 7 */
	uint32_t init7;
	/* [0xf0] DIMM Control Register */
	uint32_t dimmctl;
	/* [0xf4] Rank Control Register */
	uint32_t rankctl;
	uint32_t rsrvd_5[2];
	/* [0x100] SDRAM Timing Register 0 */
	uint32_t dramtmg0;
	/* [0x104] SDRAM Timing Register 1 */
	uint32_t dramtmg1;
	/* [0x108] SDRAM Timing Register 2 */
	uint32_t dramtmg2;
	/* [0x10c] SDRAM Timing Register 3 */
	uint32_t dramtmg3;
	/* [0x110] SDRAM Timing Register 4 */
	uint32_t dramtmg4;
	/* [0x114] SDRAM Timing Register 5 */
	uint32_t dramtmg5;
	uint32_t rsrvd_6[2];
	/* [0x120] SDRAM Timing Register 8 */
	uint32_t dramtmg8;
	/* [0x124] SDRAM Timing Register 9 */
	uint32_t dramtmg9;
	/* [0x128] SDRAM Timing Register 10 */
	uint32_t dramtmg10;
	/* [0x12c] SDRAM Timing Register 11 */
	uint32_t dramtmg11;
	/* [0x130] SDRAM Timing Register 12 */
	uint32_t dramtmg12;
	uint32_t rsrvd_7[2];
	/* [0x13c] SDRAM Timing Register 15 */
	uint32_t dramtmg15;
	/* [0x140] SDRAM Timing Register 16 */
	uint32_t dramtmg16;
	uint32_t rsrvd_8[11];
	/* [0x170] MRAM Timing Register 0 */
	uint32_t mramtmg0;
	/* [0x174] MRAM Timing Register 1 */
	uint32_t mramtmg1;
	/* [0x178] MRAM Timing Register 4 */
	uint32_t mramtmg4;
	/* [0x17c] MRAM Timing Register 9 */
	uint32_t mramtmg9;
	/* [0x180] ZQ Control Register 0 */
	uint32_t zqctl0;
	/* [0x184] ZQ Control Register 1 */
	uint32_t zqctl1;
	/* [0x188] ZQ Control Register 2 */
	uint32_t zqctl2;
	uint32_t rsrvd_9;
	/* [0x190] DFI Timing Register 0 */
	uint32_t dfitmg0;
	/* [0x194] DFI Timing Register 1 */
	uint32_t dfitmg1;
	/* [0x198] DFI Low Power Configuration Register 0 */
	uint32_t dfilpcfg0;
	/* [0x19c] DFI Low Power Configuration Register 1 */
	uint32_t dfilpcfg1;
	/* [0x1a0] DFI Update Register 0 */
	uint32_t dfiupd0;
	/* [0x1a4] DFI Update Register 1 */
	uint32_t dfiupd1;
	/* [0x1a8] DFI Update Register 2 */
	uint32_t dfiupd2;
	uint32_t rsrvd_10;
	/* [0x1b0] DFI Miscellaneous Control Register */
	uint32_t dfimisc;
	uint32_t rsrvd_11;
	/* [0x1b8] DFI Timing Register 3 */
	uint32_t dfitmg3;
	/* [0x1bc] DFI Status Register */
	uint32_t dfistat;
	/* [0x1c0] DM/DBI Control Register */
	uint32_t dbictl;
	uint32_t rsrvd_12[15];
	/* [0x200] Address Map Register 0 */
	uint32_t addrmap0;
	/* [0x204] Address Map Register 1 */
	uint32_t addrmap1;
	/* [0x208] Address Map Register 2 */
	uint32_t addrmap2;
	/* [0x20c] Address Map Register 3 */
	uint32_t addrmap3;
	/* [0x210] Address Map Register 4 */
	uint32_t addrmap4;
	/* [0x214] Address Map Register 5 */
	uint32_t addrmap5;
	/* [0x218] Address Map Register 6 */
	uint32_t addrmap6;
	/* [0x21c] Address Map Register 7 */
	uint32_t addrmap7;
	/* [0x220] Address Map Register 8 */
	uint32_t addrmap8;
	/* [0x224] Address Map Register 9 */
	uint32_t addrmap9;
	/* [0x228] Address Map Register 10 */
	uint32_t addrmap10;
	/* [0x22c] Address Map Register 11 */
	uint32_t addrmap11;
	uint32_t rsrvd_13[4];
	/* [0x240] ODT Configuration Register */
	uint32_t odtcfg;
	/* [0x244] ODT/Rank Map Register */
	uint32_t odtmap;
	uint32_t rsrvd_14[2];
	/* [0x250] Scheduler Control Register */
	uint32_t sched;
	/* [0x254] Scheduler Control Register 1 */
	uint32_t sched1;
	uint32_t rsrvd_15;
	/* [0x25c] High Priority Read CAM Register 1 */
	uint32_t perfhpr1;
	uint32_t rsrvd_16;
	/* [0x264] Low Priority Read CAM Register 1 */
	uint32_t perflpr1;
	uint32_t rsrvd_17;
	/* [0x26c] Write CAM Register 1 */
	uint32_t perfwr1;
	uint32_t rsrvd_18[4];
	/* [0x280] DQ Map Register 0 */
	uint32_t dqmap0;
	/* [0x284] DQ Map Register 1 */
	uint32_t dqmap1;
	/* [0x288] DQ Map Register 2 */
	uint32_t dqmap2;
	/* [0x28c] DQ Map Register 3 */
	uint32_t dqmap3;
	/* [0x290] DQ Map Register 4 */
	uint32_t dqmap4;
	/* [0x294] DQ Map Register 5 */
	uint32_t dqmap5;
	uint32_t rsrvd_19[26];
	/* [0x300] Debug Register 0 */
	uint32_t dbg0;
	/* [0x304] Debug Register 1 */
	uint32_t dbg1;
	/* [0x308] CAM Debug Register */
	uint32_t dbgcam;
	/* [0x30c] Command Debug Register */
	uint32_t dbgcmd;
	/* [0x310] Status Debug Register */
	uint32_t dbgstat;
	uint32_t rsrvd_20[3];
	/* [0x320] Software Register Programming Control Enable */
	uint32_t swctl;
	/* [0x324] Software Register Programming Control Status */
	uint32_t swstat;
	uint32_t rsrvd_21[2];
	/* [0x330] On-Chip Parity Configuration Register 0 */
	uint32_t ocparcfg0;
	/* [0x334] On-Chip Parity Configuration Register 1 */
	uint32_t ocparcfg1;
	/* [0x338] On-Chip Parity Configuration Register 2 */
	uint32_t ocparcfg2;
	/* [0x33c] On-Chip Parity Configuration Register 3 */
	uint32_t ocparcfg3;
	/* [0x340] On-Chip Parity Status Register 0 */
	uint32_t ocparstat0;
	/* [0x344] On-Chip Parity Status Register 1 */
	uint32_t ocparstat1;
	/* [0x348] On-Chip Parity Write Data Log Register 0 */
	uint32_t ocparwlog0;
	/* [0x34c] On-Chip Parity Write Data Log Register 1 */
	uint32_t ocparwlog1;
	/* [0x350] On-Chip Parity Write Data Log Register 2 */
	uint32_t ocparwlog2;
	/* [0x354] On-Chip Parity Write Address Log Register 0 */
	uint32_t ocparawlog0;
	/* [0x358] On-Chip Parity Write Address Log Register 1 */
	uint32_t ocparawlog1;
	/* [0x35c] On-Chip Parity Read Data Log Register 0 */
	uint32_t ocparrlog0;
	/* [0x360] On-Chip Parity Read Data Log Register 1 */
	uint32_t ocparrlog1;
	/* [0x364] On-Chip Parity Read Address Log Register 0 */
	uint32_t ocpararlog0;
	/* [0x368] On-Chip Parity Read Address Log Register 1 */
	uint32_t ocpararlog1;
	/* [0x36c] AXI Poison Configuration Register. Common for all AXI ports */
	uint32_t poisoncfg;
	/* [0x370] AXI Poison Status Register */
	uint32_t poisonstat;
	/* [0x374] Advanced ECC Index Register */
	uint32_t adveccindex;
	uint32_t rsrvd_22;
	/* [0x37c] ECC Poison Pattern 0 Register */
	uint32_t eccpoisonpat0;
	/* [0x380] ECC Poison Pattern 1 Register */
	uint32_t eccpoisonpat1;
	/* [0x384] ECC Poison Pattern 2 Register */
	uint32_t eccpoisonpat2;
	uint32_t rsrvd[28];
};

struct al_dwc_ddr_umctl2_mp_alpine_v3 {
	uint32_t rsrvd_0;
	/* [0x4] Port Status Register */
	uint32_t pstat;
	/* [0x8] Port Common Configuration Register */
	uint32_t pccfg;
	/* [0xc] Port n Configuration Read Register */
	uint32_t pcfgr_0;
	/* [0x10] Port n Configuration Write Register */
	uint32_t pcfgw_0;
	uint32_t rsrvd_1;
	/* [0x18] Port n Channel m Configuration ID Mask Register */
	uint32_t pcfgidmaskch0_0;
	/* [0x1c] Port n Channel m Configuration ID Value Register */
	uint32_t pcfgidvaluech0_0;
	/* [0x20] Port n Channel m Configuration ID Mask Register */
	uint32_t pcfgidmaskch1_0;
	/* [0x24] Port n Channel m Configuration ID Value Register */
	uint32_t pcfgidvaluech1_0;
	/* [0x28] Port n Channel m Configuration ID Mask Register */
	uint32_t pcfgidmaskch2_0;
	/* [0x2c] Port n Channel m Configuration ID Value Register */
	uint32_t pcfgidvaluech2_0;
	/* [0x30] Port n Channel m Configuration ID Mask Register */
	uint32_t pcfgidmaskch3_0;
	/* [0x34] Port n Channel m Configuration ID Value Register */
	uint32_t pcfgidvaluech3_0;
	/* [0x38] Port n Channel m Configuration ID Mask Register */
	uint32_t pcfgidmaskch4_0;
	/* [0x3c] Port n Channel m Configuration ID Value Register */
	uint32_t pcfgidvaluech4_0;
	/* [0x40] Port n Channel m Configuration ID Mask Register */
	uint32_t pcfgidmaskch5_0;
	/* [0x44] Port n Channel m Configuration ID Value Register */
	uint32_t pcfgidvaluech5_0;
	/* [0x48] Port n Channel m Configuration ID Mask Register */
	uint32_t pcfgidmaskch6_0;
	/* [0x4c] Port n Channel m Configuration ID Value Register */
	uint32_t pcfgidvaluech6_0;
	/* [0x50] Port n Channel m Configuration ID Mask Register */
	uint32_t pcfgidmaskch7_0;
	/* [0x54] Port n Channel m Configuration ID Value Register */
	uint32_t pcfgidvaluech7_0;
	uint32_t rsrvd_2[16];
	/* [0x98] Port n Control Register */
	uint32_t pctrl_0;
	/* [0x9c] Port n Read QoS Configuration Register 0 */
	uint32_t pcfgqos0_0;
	/* [0xa0] Port n Read QoS Configuration Register 1 */
	uint32_t pcfgqos1_0;
	/* [0xa4] Port n Write QoS Configuration Register 0 */
	uint32_t pcfgwqos0_0;
	/* [0xa8] Port n Write QoS Configuration Register 1 */
	uint32_t pcfgwqos1_0;
	uint32_t rsrvd[1431];
};
struct al_dwc_ddr_umctl2_regs_addrmap_alt_alpine_v3 {
	/* [0x0] Alternative Address Map Register 0 */
	uint32_t addrmap0_alt;
	/* [0x4] Alternative Address Map Register 1 */
	uint32_t addrmap1_alt;
	/* [0x8] Alternative Address Map Register 2 */
	uint32_t addrmap2_alt;
	/* [0xc] Alternative Address Map Register 3 */
	uint32_t addrmap3_alt;
	/* [0x10] Alternative Address Map Register 4 */
	uint32_t addrmap4_alt;
	/* [0x14] Alternative Address Map Register 5 */
	uint32_t addrmap5_alt;
	/* [0x18] Alternative Address Map Register 6 */
	uint32_t addrmap6_alt;
	/* [0x1c] Alternative Address Map Register 7 */
	uint32_t addrmap7_alt;
	/* [0x20] Alternative Address Map Register 8 */
	uint32_t addrmap8_alt;
	/* [0x24] Alternative Address Map Register 9 */
	uint32_t addrmap9_alt;
	/* [0x28] Alternative Address Map Register 10 */
	uint32_t addrmap10_alt;
	/* [0x2c] Alternative Address Map Register 11 */
	uint32_t addrmap11_alt;
	uint32_t rsrvd[4];
};

struct al_ddr_ctrl_regs_alpine_v3 {
	struct al_dwc_ddr_umctl2_regs_alpine_v3 umctl2_regs;       /* [0x0] */
	struct al_dwc_ddr_umctl2_mp_alpine_v3 umctl2_mp;           /* [0x3f8] */
	uint32_t rsrvd_0[574];
	struct al_dwc_ddr_umctl2_regs_addrmap_alt_alpine_v3 umctl2_regs_addrmap_alt; /* [0x23f8] */

};

#ifdef __cplusplus
}
#endif

#endif /* __AL_HAL_DDR_CTRL_REGS_ALPINE_V3_H__ */

/** @} end of DDR group */


