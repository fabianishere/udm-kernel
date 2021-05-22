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
 * @file   al_hal_ddr_phy_regs_alpine_v3.h
 *
 * @brief  DDR PHY registers
 *
 */
#ifndef __AL_HAL_DDR_PHY_REGS_ALPINE_V3_H__
#define __AL_HAL_DDR_PHY_REGS_ALPINE_V3_H__

#include "al_hal_ddr_cfg.h"
#include "al_hal_plat_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* The index of the ECC byte lane */
#define AL_DDR_PHY_ECC_BYTE_LANE_INDEX_ALPINE_V3		8

/* The number of byte lanes used for 16bit */
#define AL_DDR_PHY_16BIT_BYTE_LANES_ALPINE_V3			2

/* The number of byte lanes used for 32bit */
#define AL_DDR_PHY_32BIT_BYTE_LANES_ALPINE_V3			4

struct al_ddr_phy_zq_regs_alpine_v3 {
	/* [0x0] ZQ Impedance Control Program Register */
	uint32_t pr;
	/* [0x4] ZQ Impedance Control Data Register */
	uint32_t dr;
	/* [0x8] ZQ Impedance Control Status Register */
	uint32_t sr;
	uint32_t reserved;
};

struct al_ddr_phy_datx8_regs_alpine_v3 {
	/* [0x0] DATX8 General Configuration Registers 0-9 */
	uint32_t gcr[10];
	uint32_t rsrvd_0[6];
	/* [0x40] DATX8 Bit Delay Line Registers 0-2 */
	uint32_t bdlr0_2[3];
	uint32_t rsrvd_1;
	/* [0x50] DATX8 Bit Delay Line Registers 3-5 */
	uint32_t bdlr3_5[3];
	uint32_t rsrvd_2;
	/* [0x60] DATX8 Bit Delay Line Registers 6-9 */
	uint32_t bdlr6_9[4];
	uint32_t rsrvd_3[4];
	/* [0x80] DATX8 Local Calibrated Delay Line Registers 0-5 */
	uint32_t lcdlr[6];
	uint32_t rsrvd_4[2];
	/* [0xa0] DATX8 Master Delay Line Registers 0-1 */
	uint32_t mdlr[2];
	uint32_t rsrvd_5[6];
	/* [0xc0] DATX8 General Timing Registers */
	uint32_t gtr;
	uint32_t rsrvd_6[3];
	/* [0xd0] DATX8 Rank Status Registers 0-3 */
	uint32_t rsr[4];
	/* [0xe0] DATX8 General Status Registers 0-6 */
	uint32_t gsr[7];
	uint32_t rsrvd_7;
};

struct al_dwc_ddrphy_top_dwc_ddrphy_pub_alpine_v3 {
	/* [0x0] Revision Identification Register. */
	uint32_t ridr;
	/* [0x4] PHY Initialization Register */
	uint32_t pir;
	/* [0x8]  */
	uint32_t cgcr;
	/* [0xc]  */
	uint32_t cgcr1;
	/* [0x10] PHY General Configuration Register 0-8 */
	uint32_t pgcr[9];
	/* [0x34] PHY General Status Register 0-1 */
	uint32_t pgsr[2];
	uint32_t rsrvd_0;
	/* [0x40] PHY Timing Register 0-6 */
	uint32_t ptr[7];
	uint32_t rsrvd_1[3];
	/* [0x68] PLL Control Register 0-5 (Type B PLL Only) */
	uint32_t pllcr_typ_b[6];
	/* [0x80] PLL Control Register (Type A PLL Only) */
	uint32_t pllcr_typ_a;
	uint32_t rsrvd_2;
	/* [0x88] DATX8 Common Configuration Register */
	uint32_t dxccr;
	uint32_t rsrvd_3;
	/* [0x90] DDR System General Configuration Register */
	uint32_t dsgcr;
	uint32_t rsrvd_4;
	/* [0x98] ODT Configuration Register */
	uint32_t odtcr;
	uint32_t rsrvd_5;
	/* [0xa0] Anti-Aging Control Register */
	uint32_t aacr;
	uint32_t rsrvd_6[7];
	/* [0xc0] General Purpose Register 0-1 */
	uint32_t gpr[2];
	uint32_t rsrvd_7[14];
	/* [0x100] DRAM Configuration Register */
	uint32_t dcr;
	uint32_t rsrvd_8[3];
	/* [0x110] DRAM Timing Parameters Register 0-6 */
	uint32_t dtpr[7];
	uint32_t rsrvd_9[5];
	/* [0x140] RDIMM General Configuration Register 0-2 */
	uint32_t rdimmgcr[3];
	uint32_t rsrvd_10;
	/* [0x150] RDIMM Control Register 0-4 */
	uint32_t rdimmcr[5];
	uint32_t rsrvd_11;
	/* [0x168] Scheduler Command Register 0-1 */
	uint32_t schcr[2];
	uint32_t rsrvd_12[4];
	/* [0x180] DDR3 Mode Register 0-7 */
	uint32_t mr[8];
	uint32_t rsrvd_13[3];
	/* [0x1ac] DDR3 Mode Register 11 */
	uint32_t mr11;
	uint32_t rsrvd_14[20];
	/* [0x200] Data Training Configuration Register 0-1 */
	uint32_t dtcr[2];
	/* [0x208] Data Training Address Register 0-2 */
	uint32_t dtar[3];
	uint32_t rsrvd_15;
	/* [0x218] Data Training Data Register 0-1 */
	uint32_t dtdr[2];
	/* [0x220] User Defined Data Register 0-1 */
	uint32_t uddr[2];
	uint32_t rsrvd_16[2];
	/* [0x230] Data Training Eye Data Register 0-2 */
	uint32_t dtedr[3];
	/* [0x23c] VREF Training Data Register */
	uint32_t vtdr;
	/* [0x240] CA Training Register 0-1 */
	uint32_t catr[2];
	uint32_t rsrvd_17[2];
	/* [0x250] DQS Drift Register 0-2 */
	uint32_t dqsdr[3];
	uint32_t rsrvd_18[41];
	/* [0x300] DCU Address Register */
	uint32_t dcuar;
	/* [0x304] DCU Data Register */
	uint32_t dcudr;
	/* [0x308] DCU Run Register */
	uint32_t dcurr;
	/* [0x30c] DCU Loop Register */
	uint32_t dculr;
	/* [0x310] DCU General Configuration Register */
	uint32_t dcugcr;
	/* [0x314] DCU Timing Parameters Register */
	uint32_t dcutpr;
	/* [0x318] DCU Status Register 0-1 */
	uint32_t dcusr[2];
	uint32_t rsrvd_19[56];
	/* [0x400] BIST Run Register */
	uint32_t bistrr;
	/* [0x404] BIST Word Count Register */
	uint32_t bistwcr;
	/* [0x408] BIST Mask Register 0-2 */
	uint32_t bistmskr[3];
	/* [0x414] BIST LFSR Seed Register */
	uint32_t bistlsr;
	/* [0x418] BIST Address Register 0-4 */
	uint32_t bistar[5];
	/* [0x42c] BIST User Data Pattern Register */
	uint32_t bistudpr;
	/* [0x430] BIST General Status Register */
	uint32_t bistgsr;
	/* [0x434] BIST Word Error Register 0-1 */
	uint32_t bistwer[2];
	/* [0x43c] BIST Bit Error Register 0-4 */
	uint32_t bistber[5];
	/* [0x450] BIST Word Count Status Register */
	uint32_t bistwcsr;
	/* [0x454] BIST Fail Word Register 0-2 */
	uint32_t bistfwr[3];
	/* [0x460] BIST Bit Error Register 5 */
	uint32_t bistber5;
	uint32_t rsrvd_20[30];
	/* [0x4dc] Rank ID Register */
	uint32_t rankidr;
	/* [0x4e0] Rank I/O Configuration Register 0-5 */
	uint32_t riocr[6];
	uint32_t rsrvd_21[2];
	/* [0x500] AC I/O Configuration Register 0-4 */
	uint32_t aciocr[5];
	uint32_t rsrvd_22[3];
	/* [0x520] IO  VREF Control Register 0-1 */
	uint32_t iovcr[2];
	/* [0x528] VREF Training Control Register 0-1 */
	uint32_t vtcr[2];
	uint32_t rsrvd_23[4];
	/* [0x540] AC Bit Delay Line Register 0-14 */
	uint32_t acbdlr[15];
	uint32_t rsrvd_24;
	/* [0x580] AC Local Calibrated Delay Line Register */
	uint32_t aclcdlr;
	uint32_t rsrvd_25[7];
	/* [0x5a0] AC Master Delay Line Register 0-1 */
	uint32_t acmdlr[2];
	uint32_t rsrvd_26[54];
	/* [0x680] ZQ Impedance Control Register */
	uint32_t zqcr;
	/* [0x684] */
	struct al_ddr_phy_zq_regs_alpine_v3 zq[AL_DDR_PHY_NUM_ZQ_SEGMENTS_ALPINE_V3];
	uint32_t rsrvd_27[23];
	struct al_ddr_phy_datx8_regs_alpine_v3 datx8[AL_DDR_PHY_NUM_BYTE_LANES];
};


struct al_dwc_ddrphy_top_regs_alpine_v3 {
	struct al_dwc_ddrphy_top_dwc_ddrphy_pub_alpine_v3 dwc_ddrphy_pub;
};


/*
* Registers Fields
*/


/**** RIDR register ****/
/* PUB Minor Revision: Indicates minor update of the PUB such as ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIDR_PUBMNR_MASK 0x0000000F
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIDR_PUBMNR_SHIFT 0
/* PUB Moderate Revision: Indicates moderate revision of the PUB ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIDR_PUBMDR_MASK 0x000000F0
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIDR_PUBMDR_SHIFT 4
/* PUB Major Revision: Indicates major revision of the PUB such  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIDR_PUBMJR_MASK 0x00000F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIDR_PUBMJR_SHIFT 8
/* PHY Minor Revision: Indicates minor update of the PHY such as ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIDR_PHYMNR_MASK 0x0000F000
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIDR_PHYMNR_SHIFT 12
/* PHY Moderate Revision: Indicates moderate revision of the PHY ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIDR_PHYMDR_MASK 0x000F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIDR_PHYMDR_SHIFT 16
/* PHY Major Revision: Indicates major revision of the PHY such  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIDR_PHYMJR_MASK 0x00F00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIDR_PHYMJR_SHIFT 20
/* User-Defined Revision ID: General purpose revision identifica ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIDR_UDRID_MASK 0xFF000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIDR_UDRID_SHIFT 24

/**** PIR register ****/
/* Initialization Trigger: A write of 1b1 to this bit triggers t ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PIR_INIT (1 << 0)
/* Impedance Calibration: Performs PHY impedance calibration */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PIR_ZCAL (1 << 1)
/* CA Training: Performs PHY LPDDR3 CA training */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PIR_CA (1 << 2)
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PIR_RESERVED_3 (1 << 3)
/* PLL Initialiazation: Executes the PLL initialization sequence ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PIR_PLLINIT (1 << 4)
/* Digital Delay Line (DDL) Calibration: Performs PHY delay line ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PIR_DCAL (1 << 5)
/* PHY Reset: Resets the AC and DATX8 modules by asserting the A ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PIR_PHYRST (1 << 6)
/* DRAM Reset: Issues a reset to the DRAM (by driving the DRAM r ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PIR_DRAMRST (1 << 7)
/* DRAM Initialization: Executes the DRAM initialization sequenc ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PIR_DRAMINIT (1 << 8)
/* Write Leveling: Executes a PUB write leveling routine */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PIR_WL (1 << 9)
/* Read DQS Gate Training: Executes a PUB training routine to de ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PIR_QSGATE (1 << 10)
/* Write Leveling Adjust: Executes a PUB training routine that r ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PIR_WLADJ (1 << 11)
/* Read Data Bit Deskew: Executes a PUB training routine to desk ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PIR_RDDSKW (1 << 12)
/* Write Data Bit Deskew: Executes a PUB training routine to des ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PIR_WRDSKW (1 << 13)
/* Read Data Eye Training: Executes a PUB training routine to ma ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PIR_RDEYE (1 << 14)
/* Write Data Eye Training: Executes a PUB training routine to m ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PIR_WREYE (1 << 15)
/* Static Read Training: Executes a PUB training routine to comp ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PIR_SRD (1 << 16)
/* Execute a PUB training routine for DRAM and HOST DQ  IO VREF  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PIR_VREF (1 << 17)
/* Controller DRAM Initialization: Indicates if set that DRAM in ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PIR_CTLDINIT (1 << 18)
/* RDIMM Initialization: Executes the RDIMM buffer chip initiali ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PIR_RDIMMINIT (1 << 19)
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PIR_RESERVED_28_20_MASK 0x1FF00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_PIR_RESERVED_28_20_SHIFT 20
/* Digital Delay Line (DDL) Calibration Pause: Pauses or halts,  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PIR_DCALPSE (1 << 29)
/* Impedance Calibration Bypass: Bypasses or stops, if set, impe ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PIR_ZCALBYP (1 << 30)
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PIR_RESERVED_31 (1 << 31)

/**** CGCR register ****/
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_CGCR_RESERVED_7_0_MASK 0x000000FF
#define ALPINE_V3_DWC_DDR_PHY_REGS_CGCR_RESERVED_7_0_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_CGCR_RESERVED_31_8_MASK 0xFFFFFF00
#define ALPINE_V3_DWC_DDR_PHY_REGS_CGCR_RESERVED_31_8_SHIFT 8

/**** CGCR1 register ****/
/* Enable Clock Gating for AC ctl_clk. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_CGCR1_GATEACCTLCLK (1 << 0)
/* Enable Clock Gating for AC ddr_clk. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_CGCR1_GATEACDDRCLK (1 << 1)
/* Enable Clock Gating for AC ctl_rd_clk. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_CGCR1_GATEACRDCLK (1 << 2)
/* Enable Clock Gating for DX ctl_clk. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_CGCR1_GATEDXCTLCLK_MASK 0x00000FF8
#define ALPINE_V3_DWC_DDR_PHY_REGS_CGCR1_GATEDXCTLCLK_SHIFT 3
/* Enable Clock Gating for DX ddr_clk. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_CGCR1_GATEDXDDRCLK_MASK 0x001FF000
#define ALPINE_V3_DWC_DDR_PHY_REGS_CGCR1_GATEDXDDRCLK_SHIFT 12
/* Enable Clock Gating for DX rd_clk. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_CGCR1_GATEDXRDCLK_MASK 0x3FE00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_CGCR1_GATEDXRDCLK_SHIFT 21
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_CGCR1_RESERVED_31_30_MASK 0xC0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_CGCR1_RESERVED_31_30_SHIFT 30

/**** PGCR0 register ****/
/* Initialization Complete Pin Configuration: Specifies how the  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR0_ICPC (1 << 0)
/* Clear Impedance Calibration: A write of 1b1 to this bit will  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR0_CLRZCAL (1 << 1)
/* PLL FSM Bypass: Forces, if set, the PLL FSM to the DONE state */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR0_PLLFSMBYP (1 << 2)
/* Initialization Bypass: Forces, if set, the Initialization FSM ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR0_INITFSMBYP (1 << 3)
/* Clear Partiy Error: A write of 1b1 to this bit will clear the ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR0_CLRPERR (1 << 4)
/* Clear Training Status Registers: A write of 1b1 to this bit w ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR0_CLRTSTAT (1 << 5)
/* Delay Line Test Mode: Selects, if set, the delay line oscilla ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR0_DLTMODE (1 << 6)
/* Delay Line Test Start: A write of 1b1 to this bit will trigge ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR0_DLTST (1 << 7)
/* Oscillator Enable: Enables, if set, the delay line oscillatio ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR0_OSCEN (1 << 8)
/* Oscillator Mode Division: Specifies the factor by which the d ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR0_OSCDIV_MASK 0x00001E00
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR0_OSCDIV_SHIFT 9
/* Address/Command Write leveling Pipeline Always On */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR0_ACWLPON (1 << 13)
/* Digital Test Output Select: Selects the PHY digital test outp ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR0_DTOSEL_MASK 0x0007C000
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR0_DTOSEL_SHIFT 14
/* Oscillator Mode Write-Leveling Delay Line Select: Selects whi ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR0_OSCWDL_MASK 0x00180000
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR0_OSCWDL_SHIFT 19
/* Reserved. Returns zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR0_RESERVED_21 (1 << 21)
/* Oscillator Mode Write-Data Delay Line Select: Selects which o ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR0_OSCWDDL_MASK 0x00C00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR0_OSCWDDL_SHIFT 22
/* Oscillator Mode Address/Command Delay Line Select: Selects wh ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR0_OSCACDL_MASK 0x03000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR0_OSCACDL_SHIFT 24
/* PHY FIFO Reset: A write of 1b0 to this bit resets the AC and  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR0_PHYFRST (1 << 26)
/* X4 Oscillator Mode Write-Leveling Delay Line Select: Selects  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR0_X4OSCWDL_MASK 0x18000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR0_X4OSCWDL_SHIFT 27
/* X4 Oscillator Mode Write-Data Delay Line Select: Selects whic ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR0_X4OSCWDDL_MASK 0x60000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR0_X4OSCWDDL_SHIFT 29
/* Address Copy: Enables, if set, the use of {RAS#,BA[2:0] and A ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR0_ADCP (1 << 31)

/**** PGCR1 register ****/
/* Digital Test Output Mode: Selects whether the digital test ou ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_DTOMODE (1 << 0)
/* Write Leveling (Software) Mode: Indicates if set that the PUB ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_WLMODE (1 << 1)
/* Write Leveling Step: Specifies the number of delay step-size  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_WLSTEP (1 << 2)
/* Write Level Uncertainty Region: Indicates, if set, that an ex ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_WLUNCRT (1 << 3)
/* Controls DDL  Bypass Modes */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_DDLBYPMODE_MASK 0x00000030
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_DDLBYPMODE_SHIFT 4

#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_DDLBYPMODE_DYNAMIC	\
	(0 << ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_DDLBYPMODE_SHIFT)

#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_DDLBYPMODE_BYPASS	\
	(1 << ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_DDLBYPMODE_SHIFT)

#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_DDLBYPMODE_NO_BYPASS	\
	(2 << ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_DDLBYPMODE_SHIFT)

/* Enables, if set, the PUB to control the interface to the PHY  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_PUBMODE (1 << 6)
/* I/O DDR Mode (D3F I/O Only): Selects the DDR mode for the I/O ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_IODDRM_MASK 0x00000180
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_IODDRM_SHIFT 7

#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_IODDRM_D4M		\
	(1 << ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_IODDRM_SHIFT)

/* Master Delay Line Enable: Enables, if set, the AC master dela ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_MDLEN (1 << 9)
/* Low-Pass Filter Enable: Enables, if set, the low pass filteri ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_LPFEN (1 << 10)
/* Low-Pass Filter Depth: Specifies the number of measurements o ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_LPFDEPTH_MASK 0x00001800
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_LPFDEPTH_SHIFT 11
/* Filter Depth: Specifies the number of measurements over which ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_FDEPTH_MASK 0x00006000
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_FDEPTH_SHIFT 13

#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_FDEPTH_2		\
	(0 << ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_FDEPTH_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_FDEPTH_4		\
	(1 << ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_FDEPTH_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_FDEPTH_8		\
	(2 << ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_FDEPTH_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_FDEPTH_16		\
	(3 << ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_FDEPTH_SHIFT)

/* Dual Channel Configuration: Set to 1 to enable shared address ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_DUALCHN (1 << 15)
/* AC Power-Down with Dual Channels: Set to 1 to power-down addr ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_ACPDDC (1 << 16)
/* Low-Power Master Channel 0: set to 1 to have channel 0 act as ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_LPMSTRC0 (1 << 17)
/* DFI Update Master Channel 0: set to 1 to have channel 0 act a ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_UPDMSTRC0 (1 << 18)
/* Per Rank/RCD Configuration Enable. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_PRCFG_EN (1 << 19)
/* LRDIMM Software Training. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_LRDIMMST (1 << 20)
/* AC Loopback Valid Delay: Specifies the delay that should be a ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_ACVLDDLY_MASK 0x00E00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_ACVLDDLY_SHIFT 21
/* AC Loopback Valid Train: Indicates, if set, that AC loopback  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_ACVLDTRN (1 << 24)
/* PHY High-Speed Reset: A write of 1b0 to this bit resets the A ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_PHYHRST (1 << 25)
/* Delay Load Timing: Specifies the timing of the signal that is ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_DLYLDTM (1 << 26)
/* I/O Loop-Back Select: Selects where inside the I/O the loop-b ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_IOLB (1 << 27)
/* Loopback DQS Shift: Selects how the read DQS is shifted durin ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_LBDQSS (1 << 28)
/* Loopback DQS Gating: Selects the DQS gating mode that should  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_LBGDQS_MASK 0x60000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_LBGDQS_SHIFT 29
/* Loopback Mode: Indicates, if set, that the PHY/PUB is in loop ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR1_LBMODE (1 << 31)

/**** PGCR2 register ****/
/* Refresh Period: Indicates the period in clock cycles after wh ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR2_TREFPRD_MASK 0x0003FFFF
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR2_TREFPRD_SHIFT 0
/* CSN and CID Multiplexing. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR2_CSNCIDMUX (1 << 18)
/* Fixed Latency. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR2_FXDLAT (1 << 19)
/* Data Training PUB Mode Exit Timer: Specifies the number of co ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR2_DTPMXTMR_MASK 0x0FF00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR2_DTPMXTMR_SHIFT 20
/* Fixed Latency programmable Increment */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR2_FXDLATINCR (1 << 28)
/* Refresh Mode. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR2_RFSHMODE_MASK 0x60000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR2_RFSHMODE_SHIFT 29
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR2_RESERVED_31 (1 << 31)

/**** PGCR3 register ****/
/* Selects the level to which clocks will be stalled when clock  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR3_CLKLEVEL_MASK 0x00000003
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR3_CLKLEVEL_SHIFT 0
/* Disables the Read FIFO reset: When set, read receive fifo can ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR3_DISRST (1 << 2)
/* DATX8 Receive FIFO Read Mode */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR3_RDMODE_MASK 0x00000018
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR3_RDMODE_SHIFT 3

#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR3_RDMODE_STATIC_RD_RSP_OFF	\
	(0 << ALPINE_V3_DWC_DDR_PHY_REGS_PGCR3_RDMODE_SHIFT)

#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR3_RDMODE_STATIC_RD_RSP	\
	(1 << ALPINE_V3_DWC_DDR_PHY_REGS_PGCR3_RDMODE_SHIFT)

#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR3_RDMODE_STATIC_RD_RSP_IO_ASSIST_GATING	\
	(2 << ALPINE_V3_DWC_DDR_PHY_REGS_PGCR3_RDMODE_SHIFT)

/* Read DBI CAS Latency. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR3_RDBICL_MASK 0x000000E0
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR3_RDBICL_SHIFT 5
/* Load GSDQS LCDL with 2x the calibrated GSDQSPRD value (equivalent to one CK period). */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR3_LBGSDQS (1 << 8)
/* Enable Clock Gating for AC ctl_clk. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR3_LPACCTLCLKGEN (1 << 9)
/* Enable Clock Gating for AC ddr_clk. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR3_LPACDDRCLKGEN (1 << 10)
/* Enable Clock Gating for AC ctl_rd_clk. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR3_LPACRDCLKGEN (1 << 11)
/* Disables the Output Enables for all 35 bit AC. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR3_DISACOE (1 << 12)
/* Enable Clock Gating for DX ctl_clk. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR3_LPDXCTLCLKGEN (1 << 13)
/* Enable Clock Gating for DX  ctl_rd_clk. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR3_LPDXDDRCLKGEN (1 << 14)
/* Enable Clock Gating for DX ddr_clk. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR3_LPDXRDCLKGEN (1 << 15)
/* CK Enable. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR3_CKEN_MASK 0x00FF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR3_CKEN_SHIFT 16
/* Pub Read FIFO Bypass: When set to 1b1, the read capture FIFO  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR3_PRFBYP (1 << 24)
/* Write Data Bus Inversion Enable: when set to 1b1 (and MR5[11: ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR3_WDBI (1 << 25)
/* Read Data Bus Inversion Enable: when set to 1b1 (and MR5[12]  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR3_RDBI (1 << 26)
/* Read DBI CAS Latency Select. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR3_RDBICLSEL (1 << 27)
/* Low Power Wakeup Threshold. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR3_LPWAKEUP_THRSH_MASK 0xF0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR3_LPWAKEUP_THRSH_SHIFT 28

/**** PGCR4 register ****/
/* DATX8 DDL Bypass: Specifies, if set to 1b1 that the DDL delay ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR4_DXDDLBYP_MASK 0x0000FFFF
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR4_DXDDLBYP_SHIFT 0
/* DATX8 DDL Delay Select Dymainc Load: Specifies whether the re ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR4_DXDDLLD_MASK 0x001F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR4_DXDDLLD_SHIFT 16
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR4_RESERVED_22_21_MASK 0x00600000
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR4_RESERVED_22_21_SHIFT 21
/* AC Loopback Valid Train Pattern. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR4_ACVLDTRNP (1 << 23)
/* AC DDL Bypass. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR4_ACDDLBYP_MASK 0x1F000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR4_ACDDLBYP_SHIFT 24
/* AC DDL Delay Select Dymainc Load: Specifies whether the regis ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR4_ACDDLLD (1 << 29)
/* AC Duty Cycle Correction Bypass */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR4_ACDCCBYP (1 << 30)
/* AC Loopback FIFO Read Mode */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR4_ACRDMODE (1 << 31)

/**** PGCR5 register ****/
/* Frequency Change. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR5_FRQCHANGE (1 << 0)
/* Active Frequency. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR5_FRQACT (1 << 1)
/* Fast Frequency Change Delay Calibration. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR5_FFCDCAL (1 << 2)
/* Fast Frequency Change DFI Enable. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR5_FFCDFIEN (1 << 3)
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR5_RESERVED_11_4_MASK 0x00000FF0
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR5_RESERVED_11_4_SHIFT 4
/* PLL Frequency B Select. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR5_FRQBSEL_MASK 0x0000F000
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR5_FRQBSEL_SHIFT 12
/* Frequency A Ratio Term. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR5_FRQAT_MASK 0x00FF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR5_FRQAT_SHIFT 16
/* Frequency B Ratio Term:  This 8-bit value represents the  val ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR5_FRQBT_MASK 0xFF000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR5_FRQBT_SHIFT 24

/**** PGCR6 register ****/
/* VT Calculation Inhibit: Inhibits calculation of the next VT c ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR6_INHVT (1 << 0)
/* Forced  VT Compensation Trigger: When written to 1b1 a single ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR6_FVT (1 << 1)
/* Reserved. Returns zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR6_RESERVED_2 (1 << 2)
/* PAR Bit Delay VT Compensation. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR6_PARBVT (1 << 3)
/* ACTN Bit Delay VT Compensation. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR6_ACTNBVT (1 << 4)
/* A17 Bit Delay VT Compensation. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR6_A17BVT (1 << 5)
/* A16 Bit Delay VT Compensation. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR6_A16BVT (1 << 6)
/* CID Bit Delay VT Compensation. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR6_CIDBVT (1 << 7)
/* CK Bit Delay VT Compensation: Enables, if set the VT drift co ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR6_CKBVT (1 << 8)
/* CSN Bit Delay VT Compensation: Enables, if set the VT drift c ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR6_CSNBVT (1 << 9)
/* CKE Bit Delay VT Compensation: Enables, if set the VT drift c ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR6_CKEBVT (1 << 10)
/* ODT Bit Delay VT Compensation: Enables, if set the VT drift c ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR6_ODTBVT (1 << 11)
/* Address/Command Bit Delay VT Compensation: Enables, if set th ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR6_A9_0BVT (1 << 12)
/* AC  Address/Command Delay LCDL VT Compensation: Enables, if s ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR6_ACDLVT (1 << 13)
/* Reserved. Returns zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR6_A15_10BVT (1 << 14)
/* BA/BG Bit Delay LCDL VT Compensation. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR6_BABVT (1 << 15)
/* Delay Line VT Drift Limit. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR6_DLDLMT_MASK 0x00FF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR6_DLDLMT_SHIFT 16
/* Reserved. Returns zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR6_RESERVED_31_24_MASK 0xFF000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR6_RESERVED_31_24_SHIFT 24

/**** PGCR7 register ****/
/* AC Test Mode: This is used to enable special test mode in the ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR7_ACTMODE (1 << 0)
/* AC Digital Test Output Select: This is used to select the AC  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR7_ACDTOSEL (1 << 1)
/* This bit is reserved for future AC special PHY modes but the  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR7_ACRSVD_2 (1 << 2)
/* AC DDL Load Type: Specifies how the delay select signal is ap ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR7_ACDLDT (1 << 3)
/* AC read Clock Mode: Valid values are:1b0 = Read clock (ctl_rd ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR7_ACRCLKMD (1 << 4)
/* These bits are reserved for future AC special PHY modes but t ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR7_ACCALCLK (1 << 5)
/* AC Digital Test Output Multiplex. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR7_ACDTOMUX (1 << 6)
/* CKN Stop Low. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR7_CKNSTOPL (1 << 7)
/* Reserved. Returns zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR7_RESERVED_15_8_MASK 0x0000FF00
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR7_RESERVED_15_8_SHIFT 8
/* DATX8 Test Mode: This is used to enable special test mode in  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR7_DXTMODE (1 << 16)
/* Read DQS gate delay load bypass mode */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR7_DXGDBYP (1 << 17)
/* Read DQS/DQS# delay load bypass mode */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR7_DXQSDBYP (1 << 18)
/* DX DDL Load Type: Specifies how a new delay select value is a ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR7_DXDDLLDT (1 << 19)
/* Read DQS gating status mode: Indicates if set that the read D ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR7_DXGSMD (1 << 20)
/* DATX8 Digital Test Output Select: This is used to select the  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR7_DXDTOSEL_MASK 0x00600000
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR7_DXDTOSEL_SHIFT 21
/* DQS Gate Select: Selects the gate scheme used */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR7_DXQSGSEL (1 << 23)
/* DATX8 read Clock Mode: Valid values are:1b0 = Read clock (ctl ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR7_DXRCLKMD (1 << 24)
/* These bits are reserved for future DATX8 special PHY modes bu ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR7_DXCALCLK (1 << 25)
/* DATXn Digital Test Output Multiplex. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR7_DXDTOMUX (1 << 26)
/* These bits are reserved for future DATX8 special PHY modes but the registers are already
 * connected to existing (unused) DATX8 phy_mode bits. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR7_DXRSVD_MASK 0xF8000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR7_DXRSVD_SHIFT 27

/**** PGCR8 register ****/
/* DDL Calibration Starting Value. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR8_DCALSVAL_MASK 0x000001FF
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR8_DCALSVAL_SHIFT 0
/* DDL Calibration Type. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR8_DCALTYPE (1 << 9)
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR8_RESERVED_31_10_MASK 0xFFFFFC00
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGCR8_RESERVED_31_10_SHIFT 10

/**** PGSR0 register ****/
/* Initialization Done: Indicates if set that the DDR system ini ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGSR0_IDONE (1 << 0)
/* PLL Lock Done: Indicates if set that PLL locking has complete ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGSR0_PLDONE (1 << 1)
/* Digital Delay Line (DDL) Calibration Done: Indicates if set t ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGSR0_DCDONE (1 << 2)
/* Impedance Calibration Done: Indicates if set that impedance c ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGSR0_ZCDONE (1 << 3)
/* DRAM Initialization Done: Indicates if set that DRAM initiali ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGSR0_DIDONE (1 << 4)
/* Write Leveling Done: Indicates if set that write leveling has ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGSR0_WLDONE (1 << 5)
/* DQS Gate Training Done: Indicates if set that DQS gate traini ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGSR0_QSGDONE (1 << 6)
/* Write Leveling Adjustment Done: Indicates if set that write l ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGSR0_WLADONE (1 << 7)
/* Read Bit Deskew Done: Indicates if set that read bit deskew h ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGSR0_RDDONE (1 << 8)
/* Write Bit Deskew Done: Indicates if set that write bit deskew ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGSR0_WDDONE (1 << 9)
/* Read Eye Training Done: Indicates if set that read eye traini ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGSR0_REDONE (1 << 10)
/* Write Eye Training Done: Indicates if set that write eye trai ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGSR0_WEDONE (1 << 11)
/* CA Training Done: Indicates if set that LPDDR3 CA training ha ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGSR0_CADONE (1 << 12)
/* Static Read Done: : Indicates if set that static read trainin ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGSR0_SRDDONE (1 << 13)
/* VREF Training Done: Indicates if set that DRAM and Host VREF  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGSR0_VDONE (1 << 14)
/* Reserved. Returns zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGSR0_RESERVED_18_15_MASK 0x00078000
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGSR0_RESERVED_18_15_SHIFT 15
/* VREF Training Error: Indicates if set that there is and error ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGSR0_VERR (1 << 19)
/* Impedance Calibration Error: Indicates if set that there is a ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGSR0_ZCERR (1 << 20)
/* Write Leveling Error: Indicates if set that there is an error ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGSR0_WLERR (1 << 21)
/* DQS Gate Training Error: Indicates if set that there is an er ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGSR0_QSGERR (1 << 22)
/* Write Leveling Adjustment Error: Indicates if set that there  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGSR0_WLAERR (1 << 23)
/* Read Bit Deskew Error: Indicates if set that there is an erro ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGSR0_RDERR (1 << 24)
/* Write Bit Deskew Error: Indicates if set that there is an err ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGSR0_WDERR (1 << 25)
/* Read Eye Training Error: Indicates if set that there is an er ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGSR0_REERR (1 << 26)
/* Write Eye Training Error: Indicates if set that there is an e ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGSR0_WEERR (1 << 27)
/* CA Training Error: Indicates if set that there is an error in ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGSR0_CAERR (1 << 28)
/* CA Training Warning: Indicates if set that there is a warning ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGSR0_CAWRN (1 << 29)
/* Static Read Error: Indicates if set that there is an error in ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGSR0_SRDERR (1 << 30)
/* AC PLL Lock: Indicates, if set, that that AC PLL has locked */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGSR0_APLOCK (1 << 31)

/**** PGSR1 register ****/
/* Delay Line Test Done: Indicates, if set, that the PHY control ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGSR1_DLTDONE (1 << 0)
/* Delay Line Test Code: Returns the code measured by the PHY co ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGSR1_DLTCODE_MASK 0x01FFFFFE
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGSR1_DLTCODE_SHIFT 1
/* Reserved. Returns zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGSR1_RESERVED_28_25_MASK 0x1E000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGSR1_RESERVED_28_25_SHIFT 25
/* Fast Frequency Change Done. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGSR1_FFCDONE (1 << 29)
/* VT Stop: Indicates if set that the VT calculation logic has s ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGSR1_VTSTOP (1 << 30)
/* RDIMM Parity Error: Indicates, if set, that there was a parit ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PGSR1_PARERR (1 << 31)

/**** PTR0 register ****/
/* PHY Reset Time: Number ctl_clk cycles that the PHY reset must ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR0_TPHYRST_MASK 0x0000003F
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR0_TPHYRST_SHIFT 0
/* PLL Gear Shift Time: Number of ctl_clk cycles from when the P ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR0_TPLLGS_MASK 0x001FFFC0
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR0_TPLLGS_SHIFT 6
/* PLL Power-Down Time: Number of ctl_clk cycles that the PLL mu ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR0_TPLLPD_MASK 0xFFE00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR0_TPLLPD_SHIFT 21

/**** PTR1 register ****/
/* PLL Reset Time: Number of ctl_clk cycles that the PLL must re ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR1_TPLLRST_MASK 0x00001FFF
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR1_TPLLRST_SHIFT 0
/* Reserved. Returns zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR1_RESERVED_14_13_MASK 0x00006000
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR1_RESERVED_14_13_SHIFT 13
/* PLL Lock Time. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR1_TPLLLOCK_MASK 0xFFFF8000
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR1_TPLLLOCK_SHIFT 15

/**** PTR2 register ****/
/* Calibration On Time: Number of controller clock cycles that t ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR2_TCALON_MASK 0x0000001F
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR2_TCALON_SHIFT 0
/* Calibration Setup Time: Number of controller clock cycles fro ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR2_TCALS_MASK 0x000003E0
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR2_TCALS_SHIFT 5
/* Calibration Hold Time: Number of controller clock cycles from ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR2_TCALH_MASK 0x00007C00
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR2_TCALH_SHIFT 10
/* Write Leveling Delay Settling Time: Number of controller cloc ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR2_TWLDLYS_MASK 0x000F8000
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR2_TWLDLYS_SHIFT 15
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR2_RESERVED_31_20_MASK 0xFFF00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR2_RESERVED_31_20_SHIFT 20

/**** PTR3 register ****/
/* DRAM Initialization Time 0: DRAM initialization time in DRAM  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR3_TDINIT0_MASK 0x000FFFFF
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR3_TDINIT0_SHIFT 0
/* DRAM Initialization Time 1: DRAM initialization time in DRAM  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR3_TDINIT1_MASK 0x3FF00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR3_TDINIT1_SHIFT 20
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR3_RESERVED_31_30_MASK 0xC0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR3_RESERVED_31_30_SHIFT 30

/**** PTR4 register ****/
/* DRAM Initialization Time 2: DRAM initialization time in DRAM  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR4_TDINIT2_MASK 0x0003FFFF
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR4_TDINIT2_SHIFT 0
/* DRAM Initialization Time 3: DRAM initialization time in DRAM  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR4_TDINIT3_MASK 0x1FFC0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR4_TDINIT3_SHIFT 18
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR4_RESERVED_31_29_MASK 0xE0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR4_RESERVED_31_29_SHIFT 29

/**** PTR5 register ****/
/* Digital Test Control: Selects various PLL digital test signal ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR5_TPLLFFCGS_MASK 0x00000FFF
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR5_TPLLFFCGS_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR5_RESERVED_13_12_MASK 0x00003000
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR5_RESERVED_13_12_SHIFT 12
/* PLL Fast Frequency Change Relock Gear Shift Time. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR5_TPLLFFCRGS_MASK 0x00FFC000
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR5_TPLLFFCRGS_SHIFT 14
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR5_RESERVED_25_24_MASK 0x03000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR5_RESERVED_25_24_SHIFT 24
/* PLL Fast Frequency Change Frequency Select Time. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR5_TPLLFRQSEL_MASK 0xFC000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR5_TPLLFRQSEL_SHIFT 26

/**** PTR6 register ****/
/* PLL Re-lock Time 1. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR6_TPLLRLCK1_MASK 0x00003FFF
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR6_TPLLRLCK1_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR6_RESERVED_31_14_MASK 0xFFFFC000
#define ALPINE_V3_DWC_DDR_PHY_REGS_PTR6_RESERVED_31_14_SHIFT 14

/**** PLLCR0 register ****/
/* Digital Test Control. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_DTC_MASK 0x0000000F
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_DTC_SHIFT 0
/* Analog Test Control: Selects various PLL analog test signals  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_ATC_MASK 0x000000F0
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_ATC_SHIFT 4
/* Analog Test Enable (ATOEN): Selects the analog test signal th ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_ATOEN_MASK 0x00000F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_ATOEN_SHIFT 8
/* Gear Shift: Enables, if set, rapid locking mode */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_GSHIFT (1 << 12)
/* Charge Pump Integrating Current Control */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_CPIC_MASK 0x0001E000
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_CPIC_SHIFT 13

#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_CPIC_333MHZ_667MHZ		\
	(0x0 << ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_CPIC_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_CPIC_236MHZ_332MHZ		\
	(0x1 << ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_CPIC_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_CPIC_198MHZ_235MHZ		\
	(0x2 << ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_CPIC_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_CPIC_166MHZ_197MHZ		\
	(0x3 << ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_CPIC_SHIFT)

/* Charge Pump Proportional Current Control */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_CPPC_MASK 0x007E0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_CPPC_SHIFT 17

#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_CPPC_561MHZ_667MHZ		\
	(0x0 << ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_CPPC_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_CPPC_472MHZ_560MHZ		\
	(0x8 << ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_CPPC_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_CPPC_397MHZ_471MHZ		\
	(0xC << ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_CPPC_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_CPPC_333MHZ_396MHZ		\
	(0xE << ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_CPPC_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_CPPC_280MHZ_332MHZ		\
	(0xF << ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_CPPC_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_CPPC_236MHZ_279MHZ		\
	(0x8 << ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_CPPC_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_CPPC_198MHZ_235MHZ		\
	(0xC << ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_CPPC_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_CPPC_166MHZ_197MHZ		\
	(0xE << ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_CPPC_SHIFT)

/* Relock Mode: Enables, if set, rapid relocking mode */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_RLOCKM (1 << 23)
/* PLL Frequency Select: Selects the operating range of the PLL */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_FRQSEL_MASK 0x0F000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_FRQSEL_SHIFT 24

#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_FRQSEL_561MHZ_667MHZ		\
	(0x0 << ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_FRQSEL_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_FRQSEL_472MHZ_560MHZ		\
	(0x1 << ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_FRQSEL_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_FRQSEL_397MHZ_471MHZ		\
	(0x2 << ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_FRQSEL_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_FRQSEL_333MHZ_396MHZ		\
	(0x3 << ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_FRQSEL_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_FRQSEL_280MHZ_332MHZ		\
	(0x4 << ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_FRQSEL_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_FRQSEL_236MHZ_279MHZ		\
	(0x5 << ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_FRQSEL_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_FRQSEL_198MHZ_235MHZ		\
	(0x6 << ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_FRQSEL_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_FRQSEL_166MHZ_197MHZ		\
	(0x7 << ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_FRQSEL_SHIFT)

/* Reference Stop Mode */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_RSTOPM (1 << 28)
/* PLL Power Down: Puts the PLLs in power down mode by driving t ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_PLLPD (1 << 29)
/* PLL Rest: Resets the PLLs by driving the PLL reset pin */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_PLLRST (1 << 30)
/* PLL Bypass: Bypasses the PLL if set to 1b1. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR0_PLLBYP (1 << 31)

/**** PLLCR1 register ****/
/* Lock Detector Select */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR1_LOCKDS (1 << 0)
/* Lock Detector Counter Select */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR1_LOCKCS (1 << 1)
/* Lock Detector Phase Select */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR1_LOCKPS (1 << 2)
/* PLL VDD voltage level control */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR1_BYPVDD (1 << 3)
/* Bypass PLL vreg_dig */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR1_BYPVREGDIG (1 << 4)
/* Bypass PLL vreg_cp */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR1_BYPVREGCP (1 << 5)
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR1_RESERVED_15_6_MASK 0x0000FFC0
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR1_RESERVED_15_6_SHIFT 6
/* Connects to the PLL PLL_PROG bus. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR1_PLLPROG_MASK 0xFFFF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR1_PLLPROG_SHIFT 16

/**** PLLCR5 register ****/
/* Connects to bits [103:96] of the PLL general control bus PLL_ ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR5_PLLCTRL_103_96_MASK 0x000000FF
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR5_PLLCTRL_103_96_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR5_RESERVED_31_8_MASK 0xFFFFFF00
#define ALPINE_V3_DWC_DDR_PHY_REGS_PLLCR5_RESERVED_31_8_SHIFT 8

/**** DXCCR register ****/
/* Data On-Die Termination: Enables, when set, the on-die termin ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXCCR_DXODT (1 << 0)
/* Data I/O Mode: Selects SSTL mode (when set to 0) or CMOS mode ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXCCR_DXIOM (1 << 1)
/* Master Delay Line Enable: Enables, if set, all DATX8 master d ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXCCR_MDLEN (1 << 2)
/* Read DQS Gate I/O Loopback: Controls the loopback signal (LB) ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXCCR_DQSGLB_MASK 0x00000018
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXCCR_DQSGLB_SHIFT 3
/* DQS Resistor: Selects the on-die pull-down for DQS pins */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXCCR_DQSRES_MASK 0x000001E0
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXCCR_DQSRES_SHIFT 5
/* DQS# Resistor: Selects the on-die pull-up resistor for DQS pi ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXCCR_DQSNRES_MASK 0x00001E00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXCCR_DQSNRES_SHIFT 9
/* Data Slew Rate (D3F I/O Only): Selects slew rate of the I/O f ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXCCR_DXSR_MASK 0x00006000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXCCR_DXSR_SHIFT 13
/* Most Significant Byte Unused DQs: Specifies the number of DQ  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXCCR_MSBUDQ_MASK 0x00038000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXCCR_MSBUDQ_SHIFT 15
/* Reserved. Return zeroes on reads */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXCCR_RESERVED_20_18_MASK 0x001C0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXCCR_RESERVED_20_18_SHIFT 18
/* Unused DQ I/O Mode: Selects SSTL mode (when set to 1b0) or CM ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXCCR_UDQIOM (1 << 21)
/* QS Counter Enable */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXCCR_QSCNTEN (1 << 22)
/* DATX8 Duty Cycle Correction Bypass */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXCCR_DXDCCBYP (1 << 23)
/* Reserved. Return zeroes on reads */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXCCR_RESERVED_28_24_MASK 0x1F000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXCCR_RESERVED_28_24_SHIFT 24
/* Rank looping (per-rank eye centering) enable */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXCCR_RKLOOP (1 << 29)
/* X4 DQS Mode: Indicates, if set, that the data bit 8 (DQ[8]) p ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXCCR_X4DQSMD (1 << 30)
/* X4 SDRAM Mode: Indicates if set that the DATX4X2 macro is ope ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXCCR_X4MODE (1 << 31)

/**** DSGCR register ****/
/* PHY Update Request Enable: Specifies if set, that the PHY sho ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DSGCR_PUREN (1 << 0)
/* Byte Disable Enable: Specifies if set that the PHY should res ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DSGCR_BDISEN (1 << 1)
/* Controller Impedance Update Enable. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DSGCR_CTLZUEN (1 << 2)
/* Low Power I/O Power Down: Specifies if set that the PHY shoul ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DSGCR_LPIOPD (1 << 3)
/* Low Power PLL Power Down: Specifies if set that the PHY shoul ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DSGCR_LPPLLPD (1 << 4)
/* Controller Update Acknowledge Enable: Specifies, if set, that ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DSGCR_CUAEN (1 << 5)
/* DQS Gate Extension: Specifies if set that the read DQS gate w ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DSGCR_DQSGX_MASK 0x000000C0
#define ALPINE_V3_DWC_DDR_PHY_REGS_DSGCR_DQSGX_SHIFT 6
/* PHY Update Acknowledge Delay: Specifies the number of clock c ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DSGCR_PUAD_MASK 0x00000F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DSGCR_PUAD_SHIFT 8
/* DTO On-Die Termination: Enables, when set, the on-die termina ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DSGCR_DTOODT (1 << 12)
/* DTO Power Down Driver: Powers down, when set, the output driv ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DSGCR_RESERVED_13 (1 << 13)
/* DTO Power Down Receiver: Powers down, when set, the input rec ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DSGCR_DTOPDR (1 << 14)
/* DTO I/O Mode: Selects SSTL mode (when set to 0) or CMOS mode  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DSGCR_DTOIOM (1 << 15)
/* DTO Output Enable: Enables, when set, the output driver on th ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DSGCR_DTOOE (1 << 16)
/* ATO Analog Test Enable: Enables, if set, the analog test outp ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DSGCR_ATOAE (1 << 17)
/* Write Path Rise-to-Rise Mode: Indicates if set that the PHY m ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DSGCR_WRRMODE (1 << 18)
/* Single Data Rate Mode: Inidcates if the controller or the PHY ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DSGCR_SDRMODE_MASK 0x00180000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DSGCR_SDRMODE_SHIFT 19

#define ALPINE_V3_DWC_DDR_PHY_REGS_DSGCR_SDRMODE_HDR		\
	(0x0 << ALPINE_V3_DWC_DDR_PHY_REGS_DSGCR_SDRMODE_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_DSGCR_SDRMODE_CSDR		\
	(0x1 << ALPINE_V3_DWC_DDR_PHY_REGS_DSGCR_SDRMODE_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_DSGCR_SDRMODE_SDR		\
	(0x2 << ALPINE_V3_DWC_DDR_PHY_REGS_DSGCR_SDRMODE_SHIFT)

/* SDRAM Reset Output Enable. */
/* SDRAM Reset Output Enable: Enables, when set, the output driv ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DSGCR_RSTOE (1 << 21)
/* Read Path Rise-to-Rise Mode: Indicates if set that the PHY mi ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DSGCR_RRRMODE (1 << 22)
/* PHY Impedance Update Enable. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DSGCR_PHYZUEN (1 << 23)
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DSGCR_RESERVED_31_24_MASK 0xFF000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DSGCR_RESERVED_31_24_SHIFT 24

/**** ODTCR register ****/
/* Read ODT. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ODTCR_RDODT_MASK 0x0000000F
#define ALPINE_V3_DWC_DDR_PHY_REGS_ODTCR_RDODT_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ODTCR_RDODT_RSVD_MASK 0x00000FF0
#define ALPINE_V3_DWC_DDR_PHY_REGS_ODTCR_RDODT_RSVD_SHIFT 4
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ODTCR_RESERVED_15_12_MASK 0x0000F000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ODTCR_RESERVED_15_12_SHIFT 12
/* Write ODT. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ODTCR_WRODT_MASK 0x000F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ODTCR_WRODT_SHIFT 16
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ODTCR_WRODT_RSVD_MASK 0x0FF00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ODTCR_WRODT_RSVD_SHIFT 20
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ODTCR_RESERVED_31_28_MASK 0xF0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ODTCR_RESERVED_31_28_SHIFT 28

/**** AACR register ****/
/* Anti-Aging Toggle Rate: Defines the number of controller cloc ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_AACR_AATR_MASK 0x3FFFFFFF
#define ALPINE_V3_DWC_DDR_PHY_REGS_AACR_AATR_SHIFT 0
/* Anti-Aging Enable Control: Enables if set the automatic toggl ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_AACR_AAENC (1 << 30)
/* Anti-Aging PAD Output Enable Control: Enables if set anti-agi ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_AACR_AAOENC (1 << 31)

/**** GPR1 register ****/
/* Read ODT Delay Enable: Enable using RDODT_DLY_VAL value for Read ODT delay */
#define ALPINE_V3_DWC_DDR_PHY_REGS_GPR1_RDODT_DLY_EN (1 << 8)
/* Read ODT Delay: Defines the number of DDR clock cycles */
#define ALPINE_V3_DWC_DDR_PHY_REGS_GPR1_RDODT_DLY_VAL_MASK 0x00007e00
#define ALPINE_V3_DWC_DDR_PHY_REGS_GPR1_RDODT_DLY_VAL_SHIFT 9

/**** DCR register ****/
/* DDR Mode: SDRAM DDR mode */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCR_DDRMD_MASK 0x00000007
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCR_DDRMD_SHIFT 0

#define ALPINE_V3_DWC_DDR_PHY_REGS_DCR_DDRMD_DDR3		\
	(0x3 << ALPINE_V3_DWC_DDR_PHY_REGS_DCR_DDRMD_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCR_DDRMD_DDR4		\
	(0x4 << ALPINE_V3_DWC_DDR_PHY_REGS_DCR_DDRMD_SHIFT)

/* DDR 8-Bank: Indicates, if set, that the SDRAM used has 8 bank ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCR_DDR8BNK (1 << 3)
/* Primary DQ (DDR3 Only): Specifies the DQ pin in a byte that i ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCR_PDQ_MASK 0x00000070
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCR_PDQ_SHIFT 4
/* Multi-Purpose Register (MPR) DQ (DDR3 Only): Specifies the va ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCR_MPRDQ (1 << 7)
/* DDR Type: Selects the DDR type for the specified DDR mode */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCR_DDRTYPE_MASK 0x00000300
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCR_DDRTYPE_SHIFT 8
/* Byte Mask: Mask applied to all beats of read data on all byte ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCR_BYTEMASK_MASK 0x0003FC00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCR_BYTEMASK_SHIFT 10
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCR_RESERVED_26_18_MASK 0x07FC0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCR_RESERVED_26_18_SHIFT 18
/* No Simultaneous Rank Access: Specifies if set that simultaneo ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCR_NOSRA (1 << 27)
/* DDR 2T Timing: Indicates if set that 2T timing should be used ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCR_DDR2T (1 << 28)
/* Un-buffered DIMM Address Mirroring: Indicates if set that the ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCR_UDIMM (1 << 29)
/* Un-used Bank Group. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCR_UBG (1 << 30)
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCR_RESERVED_31 (1 << 31)

/**** DTPR0 register ****/
/* Internal read to precharge command delay */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR0_TRTP_MASK 0x0000000F
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR0_TRTP_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR0_RESERVED_7_4_MASK 0x000000F0
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR0_RESERVED_7_4_SHIFT 4
/* Precharge command period: The minimum time between a precharg ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR0_TRP_MASK 0x00007F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR0_TRP_SHIFT 8
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR0_RESERVED_15 (1 << 15)
/* Activate to precharge command delay */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR0_TRAS_MASK 0x007F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR0_TRAS_SHIFT 16
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR0_RESERVED_23 (1 << 23)
/* Activate to activate command delay (different banks) */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR0_TRRD_MASK 0x3F000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR0_TRRD_SHIFT 24
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR0_RESERVED_31_30_MASK 0xC0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR0_RESERVED_31_30_SHIFT 30

/**** DTPR1 register ****/
/* Load mode cycle time: The minimum time between a load mode re ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR1_TMRD_MASK 0x0000001F
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR1_TMRD_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR1_RESERVED_7_5_MASK 0x000000E0
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR1_RESERVED_7_5_SHIFT 5
/* Load mode update delay (DDR4 and DDR3 only) */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR1_TMOD_MASK 0x00000700
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR1_TMOD_SHIFT 8
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR1_RESERVED_15_11_MASK 0x0000F800
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR1_RESERVED_15_11_SHIFT 11
/* 4-bank activate period */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR1_TFAW_MASK 0x00FF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR1_TFAW_SHIFT 16
/* Minimum delay from when write leveling mode is programmed to  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR1_TWLMRD_MASK 0x3F000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR1_TWLMRD_SHIFT 24
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR1_RESERVED_31_30_MASK 0xC0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR1_RESERVED_31_30_SHIFT 30

/**** DTPR2 register ****/
/* Self refresh exit delay */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR2_TXS_MASK 0x000003FF
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR2_TXS_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR2_RESERVED_15_10_MASK 0x0000FC00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR2_RESERVED_15_10_SHIFT 10
/* CKE minimum pulse width */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR2_TCKE_MASK 0x000F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR2_TCKE_SHIFT 16
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR2_RESERVED_23_20_MASK 0x00F00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR2_RESERVED_23_20_SHIFT 20
/* Read to ODT delay (DDR3 only) */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR2_TRTODT (1 << 24)
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR2_RESERVED_27_25_MASK 0x0E000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR2_RESERVED_27_25_SHIFT 25
/* Read to Write command delay */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR2_TRTW (1 << 28)
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR2_RESERVED_31_29_MASK 0xE0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR2_RESERVED_31_29_SHIFT 29

/**** DTPR3 register ****/
/* DQS output access time from CK/CK# (LPDDR2/3 only) */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR3_TDQSCK_MASK 0x00000007
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR3_TDQSCK_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR3_RESERVED_7_3_MASK 0x000000F8
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR3_RESERVED_7_3_SHIFT 3
/* Maximum DQS output access time from CK/CK# (LPDDR2/3 only) */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR3_TDQSCKMAX_MASK 0x00000700
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR3_TDQSCKMAX_SHIFT 8
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR3_RESERVED_15_11_MASK 0x0000F800
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR3_RESERVED_15_11_SHIFT 11
/* DLL locking time. Valid values are 2 to 1023. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR3_TDLLK_MASK 0x03FF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR3_TDLLK_SHIFT 16
/* Read to read and write to write command delay */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR3_TCCD_MASK 0x1C000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR3_TCCD_SHIFT 26
/* ODT turn-off delay extension */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR3_TOFDX_MASK 0xE0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR3_TOFDX_SHIFT 29

/**** DTPR4 register ****/
/* Power down exit delay */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR4_TXP_MASK 0x0000001F
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR4_TXP_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR4_RESERVED_7_5_MASK 0x000000E0
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR4_RESERVED_7_5_SHIFT 5
/* Write leveling output delay: Number of clock cycles from when ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR4_TWLO_MASK 0x00000F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR4_TWLO_SHIFT 8
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR4_RESERVED_15_12_MASK 0x0000F000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR4_RESERVED_15_12_SHIFT 12
/* Refresh-to-Refresh: Indicates the minimum time, in clock cycl ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR4_TRFC_MASK 0x03FF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR4_TRFC_SHIFT 16
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR4_RESERVED_27_26_MASK 0x0C000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR4_RESERVED_27_26_SHIFT 26
/* ODT turn-on/turn-off delays (DDR2 only) */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR4_TAOND_TAOFD_MASK 0x30000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR4_TAOND_TAOFD_SHIFT 28
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR4_RESERVED_31_30_MASK 0xC0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR4_RESERVED_31_30_SHIFT 30

/**** DTPR5 register ****/
/* Internal write to read command delay */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR5_TWTR_MASK 0x0000001F
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR5_TWTR_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR5_RESERVED_7_5_MASK 0x000000E0
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR5_RESERVED_7_5_SHIFT 5
/* Activate to read or write delay */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR5_TRCD_MASK 0x00007F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR5_TRCD_SHIFT 8
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR5_RESERVED_15 (1 << 15)
/* Activate to activate command delay (same bank) */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR5_TRC_MASK 0x00FF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR5_TRC_SHIFT 16
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR5_RESERVED_31_24_MASK 0xFF000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR5_RESERVED_31_24_SHIFT 24

/**** DTPR6 register ****/
/* Read Latency: Specifies the read latency that should be used  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR6_PUBRL_MASK 0x0000003F
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR6_PUBRL_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR6_RESERVED_7_6_MASK 0x000000C0
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR6_RESERVED_7_6_SHIFT 6
/* Write Latency: Specifies the write latency that should be use ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR6_PUBWL_MASK 0x00003F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR6_PUBWL_SHIFT 8
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR6_RESERVED_29_14_MASK 0x3FFFC000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR6_RESERVED_29_14_SHIFT 14
/* PUB Read Latency Enable: Specifies, if set, that the PUB shou ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR6_PUBRLEN (1 << 30)
/* PUB Write Latency Enable: Specifies, if set, that the PUB sho ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTPR6_PUBWLEN (1 << 31)

/**** RDIMMGCR0 register ****/
/* Registered DIMM: Indicates if set that a registered DIMM is u ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMGCR0_RDIMM (1 << 0)
/* Parity Error No Registering: Indicates, if set, that parity e ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMGCR0_ERRNOREG (1 << 1)
/* Stop on Parity Error: Stops PUB transactions when RDIMM parit ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMGCR0_SOPERR (1 << 2)
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMGCR0_PERRDIS (1 << 3)
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMGCR0_RESERVED_13_4_MASK 0x00003FF0
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMGCR0_RESERVED_13_4_SHIFT 4
/* PAR_IN On-Die Termination: Enables, when set, the on-die term ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMGCR0_PARINODT (1 << 14)
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMGCR0_RESERVED_15 (1 << 15)
/* PAR_IN Power Down Receiver: Powers down, when set, the input  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMGCR0_PARINPDR (1 << 16)
/* PAR_IN I/O Mode: Selects SSTL mode (when set to 0) or CMOS mo ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMGCR0_PARINIOM (1 << 17)
/* Load Reduced DIMM. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMGCR0_LRDIMM (1 << 18)
/* ERROUT# On-Die Termination: Enables, when set, the on-die ter ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMGCR0_ERROUTODT (1 << 19)
/* ERROUT# Power Down Driver: Powers down, when set, the output  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMGCR0_RESERVED_20 (1 << 20)
/* ERROUT# Power Down Receiver: Powers down, when set, the input ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMGCR0_ERROUTPDR (1 << 21)
/* ERROUT# I/O Mode: Selects SSTL mode (when set to 0) or CMOS m ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMGCR0_ERROUTIOM (1 << 22)
/* ERROUT# Output Enable: Enables, when set, the output driver o ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMGCR0_ERROUTOE (1 << 23)
/* RDIMM Outputs On-Die Termination: Enables, when set, the on-d ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMGCR0_RDIMMODT (1 << 24)
/* RDIMM Outputs Power Down Driver: Powers down, when set, the o ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMGCR0_RESERVED_25 (1 << 25)
/* RDIMM Outputs Power Down Receiver: Powers down, when set, the ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMGCR0_RDIMMPDR (1 << 26)
/* RDIMM Outputs I/O Mode: Selects SSTL mode (when set to 0) or  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMGCR0_RDIMMIOM (1 << 27)
/* QCSEN# Output Enable: Enables, when set, the output driver on ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMGCR0_QCSENOE (1 << 28)
/* MIRROR Output Enable: Enables, when set, the output driver on ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMGCR0_MIRROROE (1 << 29)
/* RDMIMM Quad CS Enable: Enables, if set, the Quad CS mode for  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMGCR0_QCSEN (1 << 30)
/* RDIMM Mirror: Selects between two different ballouts of the R ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMGCR0_MIRROR (1 << 31)

/**** RDIMMGCR1 register ****/
/* Stabilization time: Number of DRAM clock cycles for the RDIMM ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMGCR1_TBCSTAB_MASK 0x00003FFF
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMGCR1_TBCSTAB_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMGCR1_RESERVED_15_14_MASK 0x0000C000
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMGCR1_RESERVED_15_14_SHIFT 14
/* Command word to command word programming delay: Number of DRA ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMGCR1_TBCMRD_MASK 0x00070000
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMGCR1_TBCMRD_SHIFT 16
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMGCR1_RESERVED_19 (1 << 19)
/* Command word to command word programming delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMGCR1_TBCMRD_L_MASK 0x00700000
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMGCR1_TBCMRD_L_SHIFT 20
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMGCR1_RESERVED_23 (1 << 23)
/* Command word to command word programming delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMGCR1_TBCMRD_L2_MASK 0x07000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMGCR1_TBCMRD_L2_SHIFT 24
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMGCR1_RESERVED_27 (1 << 27)
/* Address [17] B-side Inversion Disable */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMGCR1_A17BID_MASK 0xF0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMGCR1_A17BID_SHIFT 28

/**** RDIMMCR0 register ****/
/* Control Word 0 (Global Features Control Word): Bit definition ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR0_RC0_MASK 0x0000000F
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR0_RC0_SHIFT 0
/* Control Word 1 (Clock Driver Enable Control Word): Bit defini ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR0_RC1_MASK 0x000000F0
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR0_RC1_SHIFT 4
/* Control Word 2 (Timing and IBT Control Word): Bit definitions ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR0_RC2_MASK 0x00000F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR0_RC2_SHIFT 8
/* Control Word 3 (CA and CS Signals Driver Characteristics Cont ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR0_RC3_MASK 0x0000F000
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR0_RC3_SHIFT 12
/* Control Word 4 (ODT and CKE Signals Driver Characteristics Co ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR0_RC4_MASK 0x000F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR0_RC4_SHIFT 16
/* Control Word 5 (CK Driver Characteristics Control Word): RC5[ ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR0_RC5_MASK 0x00F00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR0_RC5_SHIFT 20
/* Control Word 6 (Comman space Control Word): Command Space Con ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR0_RC6_MASK 0x0F000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR0_RC6_SHIFT 24
/* Control Word 7: RC07 - RESERVED. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR0_RC7_MASK 0xF0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR0_RC7_SHIFT 28

/**** RDIMMCR1 register ****/
/* Control Word 8 (Input/Output Configuration Control Word): (DA ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR1_RC8_MASK 0x0000000F
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR1_RC8_SHIFT 0
/* Control Word 9 (Power Saving Settings Control Word): Bit defi ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR1_RC9_MASK 0x000000F0
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR1_RC9_SHIFT 4
/* Control Word 10 (RDIMM Operating Speed Control Word): DA[3:0] ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR1_RC10_MASK 0x00000F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR1_RC10_SHIFT 8
/* Control Word 11 (Operating Voltage VDD and VREFCA Source Cont ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR1_RC11_MASK 0x0000F000
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR1_RC11_SHIFT 12
/* Control Word 12: Training Control Word */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR1_RC12_MASK 0x000F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR1_RC12_SHIFT 16
/* Control Word 13: DIMM Configuration Control Word */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR1_RC13_MASK 0x00F00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR1_RC13_SHIFT 20
/* Control Word 14: Parity Control Word */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR1_RC14_MASK 0x0F000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR1_RC14_SHIFT 24
/* Control Word 15: Reserved for future use */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR1_RC15_MASK 0xF0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR1_RC15_SHIFT 28

/**** RDIMMCR2 register ****/
/* Control Word RC1x */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR2_RC1X_MASK 0x000000FF
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR2_RC1X_SHIFT 0
/* Control Word RC2x */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR2_RC2X_MASK 0x0000FF00
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR2_RC2X_SHIFT 8
/* Control Word RC3x */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR2_RC3X_MASK 0x00FF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR2_RC3X_SHIFT 16
/* Control Word RC4x */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR2_RC4X_MASK 0xFF000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR2_RC4X_SHIFT 24

/**** RDIMMCR3 register ****/
/* Control Word RC5x */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR3_RC5X_MASK 0x000000FF
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR3_RC5X_SHIFT 0
/* Control Word RC6x */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR3_RC6X_MASK 0x0000FF00
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR3_RC6X_SHIFT 8
/* Control Word RC7x */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR3_RC7X_MASK 0x00FF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR3_RC7X_SHIFT 16
/* Control Word RC8x */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR3_RC8X_MASK 0xFF000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR3_RC8X_SHIFT 24

/**** RDIMMCR4 register ****/
/* Control Word RC5x */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR4_RC9X_MASK 0x000000FF
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR4_RC9X_SHIFT 0
/* Control Word RCAx */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR4_RCAX_MASK 0x0000FF00
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR4_RCAX_SHIFT 8
/* Control Word RCBx */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR4_RCBX_MASK 0x00FF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR4_RCBX_SHIFT 16
/* Reserved for future use. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR4_RCXX_MASK 0xFF000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_RDIMMCR4_RCXX_SHIFT 24

/**** SCHCR0 register ****/
/* Mode Register Command Trigger: Initialization Trigger: A writ ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR0_SCHTRIG_MASK 0x0000000F
#define ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR0_SCHTRIG_SHIFT 0
/* Specifies the Command to be issued */
#define ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR0_CMD_MASK 0x000000F0
#define ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR0_CMD_SHIFT 4

#define ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR0_CMD_NOP \
	(0 << ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR0_CMD_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR0_CMD_LOAD_MODE \
	(1 << ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR0_CMD_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR0_CMD_SELF_REFRESH \
	(2 << ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR0_CMD_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR0_CMD_REFRESH \
	(3 << ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR0_CMD_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR0_CMD_PRECHARGE \
	(4 << ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR0_CMD_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR0_CMD_PRECHAREGE_ALL \
	(5 << ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR0_CMD_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR0_CMD_ACTIVATE \
	(6 << ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR0_CMD_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR0_CMD_SPECIAL_COMMAND \
	(7 << ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR0_CMD_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR0_CMD_WRITE \
	(8 << ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR0_CMD_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR0_CMD_WRITE_PRECHG \
	(9 << ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR0_CMD_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR0_CMD_READ \
	(10 << ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR0_CMD_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR0_CMD_READ_PRECHG \
	(11 << ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR0_CMD_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR0_CMD_ZQCAL_SHORT \
	(12 << ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR0_CMD_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR0_CMD_ZQCAL_LONG \
	(13 << ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR0_CMD_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR0_CMD_POWER_DOWN \
	(14 << ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR0_CMD_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR0_CMD_SDRAM_NOP \
	(15 << ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR0_CMD_SHIFT)

/* Special Command codes: Only applicable when CMD field is set  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR0_SP_CMD_MASK 0x00000F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR0_SP_CMD_SHIFT 8
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR0_RESERVED_13_12_MASK 0x00003000
#define ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR0_RESERVED_13_12_SHIFT 12
/* Scheduler Command DQ Value */
#define ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR0_SCHDQV_MASK 0xFFFFC000
#define ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR0_SCHDQV_SHIFT 14

/**** SCHCR1 register ****/
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR1_RESERVED_1_0_MASK 0x00000003
#define ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR1_RESERVED_1_0_SHIFT 0
/* All Ranks enabled: When set the commands issued by the mode r ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR1_ALLRANK (1 << 2)
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR1_RESERVED_3 (1 << 3)
/* Scheduler Command Bank Address:  Specifies the value to be dr ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR1_SCBK_MASK 0x00000030
#define ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR1_SCBK_SHIFT 4
/* Scheduler Command Bank Group: Specifies the value to be drive ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR1_SCBG_MASK 0x000000C0
#define ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR1_SCBG_SHIFT 6
/* Scheduler Command Address Specifies the value to be driven on ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR1_SCADDR_MASK 0x0FFFFF00
#define ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR1_SCADDR_SHIFT 8
/* Scheduler Rank Address. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR1_SCRNK_MASK 0xF0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_SCHCR1_SCRNK_SHIFT 28

/**** MR0 register ****/
/* Burst Length: Determines the maximum number of column locatio ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR0_BL_MASK 0x00000003
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR0_BL_SHIFT 0
/* CAS Latency: The delay, in clock cycles, between when the SDR ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR0_CL_2 (1 << 2)
/* Burst Type: Indicates whether a burst is sequential (1b0) or  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR0_BT (1 << 3)
/* CAS Latency: The delay, in clock cycles, between when the SDR ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR0_CL_6_4_MASK 0x00000070
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR0_CL_6_4_SHIFT 4
/* Operating Mode: Selects either normal operating mode (1b0) or ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR0_TM (1 << 7)
/* DLL Reset: Writing a 1b1 to this bit will reset the SDRAM DLL */
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR0_DR (1 << 8)
/* Write Recovery and Read to Precharge */
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR0_WR_MASK 0x00000E00
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR0_WR_SHIFT 9
/* Power-Down Control: Controls the exit time for power-down mod ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR0_PD (1 << 12)
/* Reserved */
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR0_RSVD_MASK 0x0000E000
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR0_RSVD_SHIFT 13
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR0_RESERVED_31_16_MASK 0xFFFF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR0_RESERVED_31_16_SHIFT 16

/**** MR1 register ****/
/* DLL Enable/Disable: Enable (1b0) or disable (1b1) the DLL */
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR1_DE (1 << 0)
/* Output Driver Impedance Control: Controls the output drive st ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR1_DIC_1 (1 << 1)
/* On Die Termination: Selects the effective resistance for SDRA ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR1_RTT_2 (1 << 2)
/* Posted CAS Additive Latency: Setting additive latency that al ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR1_AL_MASK 0x00000018
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR1_AL_SHIFT 3
/* Output Driver Impedance Control: Controls the output drive st ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR1_DIC_5 (1 << 5)
/* On Die Termination: Selects the effective resistance for SDRA ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR1_RTT_6 (1 << 6)
/* Write Leveling Enable: Enables write-leveling when set. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR1_LEVEL (1 << 7)
/* Reserved */
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR1_RSVD_8 (1 << 8)
/* On Die Termination: Selects the effective resistance for SDRA ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR1_RTT_9 (1 << 9)
/* Reserved */
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR1_RSVD_10 (1 << 10)
/* Termination Data Strobe: When enabled (1b1) TDQS provides add ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR1_TDQS (1 << 11)
/* Output Enable/Disable: When 1b0, all outputs function normal; ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR1_QOFF (1 << 12)
/* Reserved */
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR1_RSVD_15_13_MASK 0x0000E000
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR1_RSVD_15_13_SHIFT 13
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR1_RESERVED_31_16_MASK 0xFFFF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR1_RESERVED_31_16_SHIFT 16

/**** MR2 register ****/
/* Partial Array Self Refresh: Specifies that data located in ar ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR2_PASR_MASK 0x00000007
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR2_PASR_SHIFT 0
/* CAS Write Latency: The delay, in clock cycles, between when t ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR2_CWL_MASK 0x00000038
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR2_CWL_SHIFT 3
/* Auto Self-Refresh: When enabled (1b1), SDRAM automatically pr ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR2_ASR (1 << 6)
/* Self-Refresh Temperature Range: Selects either normal (1b0) o ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR2_SRT (1 << 7)
/* Reserved */
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR2_RSVD_8 (1 << 8)
/* Dynamic ODT: Selects RTT for dynamic ODT */
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR2_RTTWR_MASK 0x00000600
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR2_RTTWR_SHIFT 9
/* These are JEDEC reserved bits and are recommended by JEDEC to ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR2_RSVD_15_11_MASK 0x0000F800
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR2_RSVD_15_11_SHIFT 11
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR2_RSVD_31_16_MASK 0xFFFF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR2_RSVD_31_16_SHIFT 16

/**** MR3 register ****/
/* Multi-Purpose Register (MPR) Location: Selects MPR data locat ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR3_MPRLOC_MASK 0x00000003
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR3_MPRLOC_SHIFT 0
/* Multi-Purpose Register Enable: Enables, if set, that read dat ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR3_MPR (1 << 2)
/* These are JEDEC reserved bits and are recommended by JEDEC to ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR3_RSVD_MASK 0x0000FFF8
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR3_RSVD_SHIFT 3
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR3_RESERVED_31_16_MASK 0xFFFF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR3_RESERVED_31_16_SHIFT 16

/**** MR4 register ****/
/* These are JEDEC reserved bits and are recommended by JEDEC to ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR4_RSVD_15_0_MASK 0x0000FFFF
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR4_RSVD_15_0_SHIFT 0
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR4_RESERVED_31_16_MASK 0xFFFF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR4_RESERVED_31_16_SHIFT 16

/**** MR5 register ****/
/* C/A Parity Latency Mode */
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR5_RSVD_15_0_MASK 0x0000FFFF
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR5_RSVD_15_0_SHIFT 0
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR5_RESERVED_31_16_MASK 0xFFFF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR5_RESERVED_31_16_SHIFT 16

/**** MR6 register ****/
/* VrefDQ Training ValuesRefer to the JEDEC spec for more inform ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR6_RSVD_15_0_MASK 0x0000FFFF
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR6_RSVD_15_0_SHIFT 0
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR6_RESERVED_31_16_MASK 0xFFFF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR6_RESERVED_31_16_SHIFT 16

/**** MR7 register ****/
/* Reserve for future use. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR7_RSVD_15_0_MASK 0x0000FFFF
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR7_RSVD_15_0_SHIFT 0
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR7_RESERVED_31_16_MASK 0xFFFF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR7_RESERVED_31_16_SHIFT 16

/**** MR11 register ****/
/* On Die Termination */
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR11_RSVD_15_0_MASK 0x0000FFFF
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR11_RSVD_15_0_SHIFT 0
/* Power Down control */
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR11_RESERVED_31_16_MASK 0xFFFF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_MR11_RESERVED_31_16_SHIFT 16

/**** DTCR0 register ****/
/* Data Training Repeat Number: Repeat number used to confirm st ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTCR0_DTRPTN_MASK 0x0000000F
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTCR0_DTRPTN_SHIFT 0
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTCR0_RESERVED_5_4_MASK 0x00000030
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTCR0_RESERVED_5_4_SHIFT 4
/* Data Training Using MPR (DDR3 Only): Specifies, if set, that  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTCR0_DTMPR (1 << 6)
/* Data Training Compare Data: Specifies, if set, that DQS gate  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTCR0_DTCMPD (1 << 7)
/* Refreshes Issued During Entry to Training: */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTCR0_RESERVED_10_8_MASK 0x00000700
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTCR0_RESERVED_10_8_SHIFT 8
/* Data Training Debug Byte Select 4. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTCR0_DTDBS4 (1 << 11)
/* Data Training Write Bit Deskew Data Mask */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTCR0_DTWBDDM (1 << 12)
/* Data Training Bit Deskew Centering: Enables, if set, eye cent ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTCR0_DTBDC (1 << 13)
/* Data Training read DBI deskewing configuration. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTCR0_DTRDBITR_MASK 0x0000C000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTCR0_DTRDBITR_SHIFT 14

#define ALPINE_V3_DWC_DDR_PHY_REGS_DTCR0_DTRDBITR_DIS		\
	(0x0 << ALPINE_V3_DWC_DDR_PHY_REGS_DTCR0_DTRDBITR_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTCR0_DTRDBITR_NO_RDQ		\
	(0x1 << ALPINE_V3_DWC_DDR_PHY_REGS_DTCR0_DTRDBITR_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTCR0_DTRDBITR_DIS2		\
	(0x2 << ALPINE_V3_DWC_DDR_PHY_REGS_DTCR0_DTRDBITR_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTCR0_DTRDBITR_WITH_RDQ		\
	(0x3 << ALPINE_V3_DWC_DDR_PHY_REGS_DTCR0_DTRDBITR_SHIFT)

/* Data Training Debug Byte Select: Selects the byte during data ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTCR0_DTDBS_MASK 0x000F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTCR0_DTDBS_SHIFT 16
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTCR0_DTDBS(i)			\
	((i) << ALPINE_V3_DWC_DDR_PHY_REGS_DTCR0_DTDBS_SHIFT)
/* Data Training Debug Enable: Enables, if set, the data trainin ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTCR0_DTDEN (1 << 20)
/* Data Training Debug Step: A write of 1b1 to this bit steps th ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTCR0_DTDSTP (1 << 21)
/* Data Training Extended Write DQS: Enables, if set, an extende ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTCR0_DTEXD (1 << 22)
/* Data Training with Early/Extended Gate: Specifies if set that ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTCR0_DTEXG (1 << 23)
/* Data Training Debug Rank Select: Select the rank during train ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTCR0_DTDRS_MASK 0x03000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTCR0_DTDRS_SHIFT 24
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTCR0_RESERVED_27_26_MASK 0x0C000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTCR0_RESERVED_27_26_SHIFT 26
/* Refresh During Training: A non-zero value specifies that a bu ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTCR0_RFSHDT_MASK 0xF0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTCR0_RFSHDT_SHIFT 28

/**** DTCR1 register ****/
/* Basic Gate Training Enable: runs a trial and error algorithm  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTCR1_BSTEN (1 << 0)
/* Read Leveling Enable: runs a DQS sampling scheme using the ga ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTCR1_RDLVLEN (1 << 1)
/* Read Preamble Training enable: engages read preamble training ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTCR1_RDPRMVL_TRN (1 << 2)
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTCR1_RESERVED_3 (1 << 3)
/* Read Leveling Gate Shift: delay reduction to apply to gate af ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTCR1_RDLVLGS_MASK 0x00000070
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTCR1_RDLVLGS_SHIFT 4
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTCR1_RESERVED_7 (1 << 7)
/* Read Leveling Gate Sampling Difference: width of DQS sampling ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTCR1_RDLVLGDIFF_MASK 0x00000700
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTCR1_RDLVLGDIFF_SHIFT 8
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTCR1_RESERVED_11 (1 << 11)
/* Data Training Rank: Selects the SDRAM rank to be used during  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTCR1_DTRANK_MASK 0x00003000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTCR1_DTRANK_SHIFT 12
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTCR1_RESERVED_15_14_MASK 0x0000C000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTCR1_RESERVED_15_14_SHIFT 14
/* Rank Enable: Specifies the ranks that are enabled for data-tr ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTCR1_RANKEN_MASK 0xFFFF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTCR1_RANKEN_SHIFT 16

/**** DTAR0 register ****/
/* Data Training Row Address: Selects the SDRAM row address to b ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTAR0_DTROW_MASK 0x0003FFFF
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTAR0_DTROW_SHIFT 0
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTAR0_RESERVED_19_18_MASK 0x000C0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTAR0_RESERVED_19_18_SHIFT 18
/* Data Training Bank Group and Bank Address: Selects the SDRAM  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTAR0_DTBGBK0_MASK 0x00F00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTAR0_DTBGBK0_SHIFT 20
/* Data Training Bank Group and Bank Address: Selects the SDRAM  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTAR0_DTBGBK1_MASK 0x0F000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTAR0_DTBGBK1_SHIFT 24
/* Multi-Purpose Register (MPR) Location: Selects MPR data locat ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTAR0_MPRLOC_MASK 0x30000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTAR0_MPRLOC_SHIFT 28
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTAR0_RESERVED_31_30_MASK 0xC0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTAR0_RESERVED_31_30_SHIFT 30

/**** DTAR1 register ****/
/* Data Training Column Address: Selects the SDRAM column addres ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTAR1_DTCOL0_MASK 0x000001FF
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTAR1_DTCOL0_SHIFT 0
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTAR1_RESERVED_15_9_MASK 0x0000FE00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTAR1_RESERVED_15_9_SHIFT 9
/* Data Training Column Address: Selects the SDRAM column addres ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTAR1_DTCOL1_MASK 0x01FF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTAR1_DTCOL1_SHIFT 16
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTAR1_RESERVED_31_25_MASK 0xFE000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTAR1_RESERVED_31_25_SHIFT 25

/**** DTAR2 register ****/
/* Data Training Column Address: Selects the SDRAM column addres ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTAR2_DTCOL2_MASK 0x000001FF
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTAR2_DTCOL2_SHIFT 0
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTAR2_RESERVED_15_9_MASK 0x0000FE00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTAR2_RESERVED_15_9_SHIFT 9
/* Data Training Column Address: Selects the SDRAM column addres ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTAR2_DTCOL3_MASK 0x01FF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTAR2_DTCOL3_SHIFT 16
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTAR2_RESERVED_31_25_MASK 0xFE000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTAR2_RESERVED_31_25_SHIFT 25

/**** DTDR0 register ****/
/* Data Training Data: The first 4 bytes of data used during dat ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTDR0_DTBYTE0_MASK 0x000000FF
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTDR0_DTBYTE0_SHIFT 0
/* Data Training Data: The first 4 bytes of data used during dat ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTDR0_DTBYTE1_MASK 0x0000FF00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTDR0_DTBYTE1_SHIFT 8
/* Data Training Data: The first 4 bytes of data used during dat ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTDR0_DTBYTE2_MASK 0x00FF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTDR0_DTBYTE2_SHIFT 16
/* Data Training Data: The first 4 bytes of data used during dat ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTDR0_DTBYTE3_MASK 0xFF000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTDR0_DTBYTE3_SHIFT 24

/**** DTDR1 register ****/
/* Data Training Data: The second 4 bytes of data used during da ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTDR1_DTBYTE4_MASK 0x000000FF
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTDR1_DTBYTE4_SHIFT 0
/* Data Training Data: The second 4 bytes of data used during da ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTDR1_DTBYTE5_MASK 0x0000FF00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTDR1_DTBYTE5_SHIFT 8
/* Data Training Data: The second 4 bytes of data used during da ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTDR1_DTBYTE6_MASK 0x00FF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTDR1_DTBYTE6_SHIFT 16
/* Data Training Data: The second 4 bytes of data used during da ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTDR1_DTBYTE7_MASK 0xFF000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTDR1_DTBYTE7_SHIFT 24

/**** UDDR0 register ****/
/* User Defined Data Beat 0. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_UDDR0_UDBEAT0_MASK 0x000000FF
#define ALPINE_V3_DWC_DDR_PHY_REGS_UDDR0_UDBEAT0_SHIFT 0
/* User Defined Data Beat 1. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_UDDR0_UDBEAT1_MASK 0x0000FF00
#define ALPINE_V3_DWC_DDR_PHY_REGS_UDDR0_UDBEAT1_SHIFT 8
/* User Defined Data Beat 2. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_UDDR0_UDBEAT2_MASK 0x00FF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_UDDR0_UDBEAT2_SHIFT 16
/* User Defined Data Beat 3. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_UDDR0_UDBEAT3_MASK 0xFF000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_UDDR0_UDBEAT3_SHIFT 24

/**** UDDR1 register ****/
/* User Defined Data Beat 4. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_UDDR1_UDBEAT4_MASK 0x000000FF
#define ALPINE_V3_DWC_DDR_PHY_REGS_UDDR1_UDBEAT4_SHIFT 0
/* User Defined Data Beat 5. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_UDDR1_UDBEAT5_MASK 0x0000FF00
#define ALPINE_V3_DWC_DDR_PHY_REGS_UDDR1_UDBEAT5_SHIFT 8
/* User Defined Data Beat 6. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_UDDR1_UDBEAT6_MASK 0x00FF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_UDDR1_UDBEAT6_SHIFT 16
/* User Defined Data Beat 7. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_UDDR1_UDBEAT7_MASK 0xFF000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_UDDR1_UDBEAT7_SHIFT 24

/**** DTEDR0 register ****/
/* Data Training WDQ LCDL Minimum. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTEDR0_DTWLMN_MASK 0x000001FF
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTEDR0_DTWLMN_SHIFT 0
/* Data Training WDQ LCDL Maximum. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTEDR0_DTWLMX_MASK 0x0003FE00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTEDR0_DTWLMX_SHIFT 9
/* Data Training Write BDL Shift Minimum. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTEDR0_DTWBMN_MASK 0x00FC0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTEDR0_DTWBMN_SHIFT 18
/* Data Training Write BDL Shift Maximum. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTEDR0_DTWBMX_MASK 0xFF000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTEDR0_DTWBMX_SHIFT 24

/**** DTEDR1 register ****/
/* Data Training RDQS LCDL Minimum. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTEDR1_DTRLMN_MASK 0x000001FF
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTEDR1_DTRLMN_SHIFT 0
/* Data Training RDQS LCDL Maximum. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTEDR1_DTRLMX_MASK 0x0003FE00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTEDR1_DTRLMX_SHIFT 9
/* Data Training Read BDL Shift Minimum. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTEDR1_DTRBMN_MASK 0x00FC0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTEDR1_DTRBMN_SHIFT 18
/* Data Training Read BDL Shift Maximum. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTEDR1_DTRBMX_MASK 0xFF000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTEDR1_DTRBMX_SHIFT 24

/**** DTEDR2 register ****/
/* Data Training RDQSN LCDL Minimum. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTEDR2_DTRLMN_MASK 0x000001FF
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTEDR2_DTRLMN_SHIFT 0
/* Data Training RDQSN LCDL Maximum. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTEDR2_DTRLMX_MASK 0x0003FE00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTEDR2_DTRLMX_SHIFT 9
/* Data Training Read BDL Shift Minimum. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTEDR2_DTRBMN_MASK 0x00FC0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTEDR2_DTRBMN_SHIFT 18
/* Data Training Read BDL Shift Maximum. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTEDR2_DTRBMX_MASK 0xFF000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DTEDR2_DTRBMX_SHIFT 24

/**** VTDR register ****/
/* DRAM DQ VREF Minimum. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTDR_DVREFMN_MASK 0x0000003F
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTDR_DVREFMN_SHIFT 0
/* Reserved. Returns zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTDR_RESERVED_7_6_MASK 0x000000C0
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTDR_RESERVED_7_6_SHIFT 6
/* DRAM DQ VREF Maximum. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTDR_DVREFMX_MASK 0x00003F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTDR_DVREFMX_SHIFT 8
/* Reserved. Returns zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTDR_RESERVED_15_14_MASK 0x0000C000
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTDR_RESERVED_15_14_SHIFT 14
/* DRAM DQ VREF Minimum. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTDR_HVREFMN_MASK 0x003F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTDR_HVREFMN_SHIFT 16
/* Reserved. Returns zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTDR_RESERVED_23_22_MASK 0x00C00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTDR_RESERVED_23_22_SHIFT 22
/* DRAM DQ VREF Maximum. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTDR_HVREFMX_MASK 0x3F000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTDR_HVREFMX_SHIFT 24
/* Reserved. Returns zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTDR_RESERVED_31_30_MASK 0xC0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTDR_RESERVED_31_30_SHIFT 30

/**** CATR0 register ****/
/* CA_1 Response Byte Lane 0;  Indicates PHY byte lane number  t ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_CATR0_CA1BYTE0_MASK 0x0000000F
#define ALPINE_V3_DWC_DDR_PHY_REGS_CATR0_CA1BYTE0_SHIFT 0
/* CA_1 Response Byte Lane 1;  Indicates PHY byte lane number  t ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_CATR0_CA1BYTE1_MASK 0x000000F0
#define ALPINE_V3_DWC_DDR_PHY_REGS_CATR0_CA1BYTE1_SHIFT 4
/* Minimum time  (in terms of number of dram clocks)   PUB shoul ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_CATR0_CAADR_MASK 0x00001F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_CATR0_CAADR_SHIFT 8
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_CATR0_RESERVED_15_13_MASK 0x0000E000
#define ALPINE_V3_DWC_DDR_PHY_REGS_CATR0_RESERVED_15_13_SHIFT 13
/* Minimum time  (in terms of number of dram clocks)  between tw ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_CATR0_CACD_MASK 0x001F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_CATR0_CACD_SHIFT 16
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_CATR0_RESERVED_31_21_MASK 0xFFE00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_CATR0_RESERVED_31_21_SHIFT 21

/**** CATR1 register ****/
/* Minimum time  (in terms of number of dram clocks)  for first  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_CATR1_CAENT_MASK 0x0000000F
#define ALPINE_V3_DWC_DDR_PHY_REGS_CATR1_CAENT_SHIFT 0
/* Minimum time  (in terms of number of dram clocks)  for CA cal ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_CATR1_CAEXT_MASK 0x000000F0
#define ALPINE_V3_DWC_DDR_PHY_REGS_CATR1_CAEXT_SHIFT 4
/* Minimum time  (in terms of number of dram clocks)  for CKE go ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_CATR1_CACKEL_MASK 0x00000F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_CATR1_CACKEL_SHIFT 8
/* Minimum time  (in terms of number of dram clocks)  for CKE  h ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_CATR1_CACKEH_MASK 0x0000F000
#define ALPINE_V3_DWC_DDR_PHY_REGS_CATR1_CACKEH_SHIFT 12
/* Minimum time  (in terms of number of dram clocks)  for DRAM D ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_CATR1_CAMRZ_MASK 0x000F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_CATR1_CAMRZ_SHIFT 16
/* CA_0 Response Byte Lane 0;  Indicates PHY byte lane number  t ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_CATR1_CA0BYTE0_MASK 0x00F00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_CATR1_CA0BYTE0_SHIFT 20
/* CA_0 Response Byte Lane 1;  Indicates PHY byte lane number  t ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_CATR1_CA0BYTE1_MASK 0x0F000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_CATR1_CA0BYTE1_SHIFT 24
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_CATR1_RESERVED_31_28_MASK 0xF0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_CATR1_RESERVED_31_28_SHIFT 28

/**** DQSDR0 register ****/
/* DQS Drift Detection Enable: Indicates when set that  the DQS  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DQSDR0_DFTDTEN (1 << 0)
/* DQS Drift Detection Mode: Specifies the DQS detection mode to ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DQSDR0_DFTDTMODE (1 << 1)
/* DQS Drift Update Mode: Specifies the DQS update modeto use */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DQSDR0_DFTUPMODE_MASK 0x0000000C
#define ALPINE_V3_DWC_DDR_PHY_REGS_DQSDR0_DFTUPMODE_SHIFT 2
/* Gate Pulse Enable:  Specifies the DDR clocks when the qs_gate ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DQSDR0_DFTGPULSE (1 << 4)
/* DQS Drift Self-Refresh Mode. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DQSDR0_DFTSRMODE (1 << 5)
/* DQS Drift Power Down Mode. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DQSDR0_DFTPDMODE (1 << 6)
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DQSDR0_RESERVED_7 (1 << 7)
/* Drift Update Dummy Reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DQSDR0_DFTUPDRD_MASK 0x00000F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DQSDR0_DFTUPDRD_SHIFT 8
/* PHY Update Dummy Reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DQSDR0_PHYUPDRD_MASK 0x0000F000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DQSDR0_PHYUPDRD_SHIFT 12
/* Controller Update Dummy Reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DQSDR0_CTLUPDRD_MASK 0x000F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DQSDR0_CTLUPDRD_SHIFT 16
/* Drift Read Spacing */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DQSDR0_DFTRDSPC_MASK 0x00300000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DQSDR0_DFTRDSPC_SHIFT 20
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DQSDR0_DFTIDLRD_MASK 0x03C00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DQSDR0_DFTIDLRD_SHIFT 22
/* Drift DDL Update: Specifies if set that the PUB should also u ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DQSDR0_DFTDDLUP (1 << 26)
/* Drift Impedance Update: Specifies if set that the PUB should  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DQSDR0_DFTZQUP (1 << 27)
/* Number of delay taps by which the DQS gate LCDL will be updat ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DQSDR0_DFTDLY_MASK 0xF0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DQSDR0_DFTDLY_SHIFT 28

/**** DQSDR1 register ****/
/* Drift Idle Read Cycles: Specifies the number of clock cycles  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DQSDR1_DFTRDIDLC_MASK 0x000000FF
#define ALPINE_V3_DWC_DDR_PHY_REGS_DQSDR1_DFTRDIDLC_SHIFT 0
/* Drift Back-to-Back Read Cycles: Specifies the number of conti ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DQSDR1_DFTRDB2BC_MASK 0x0000FF00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DQSDR1_DFTRDB2BC_SHIFT 8
/* Drift Idle Read Cycles Factor: Specifies the multiplication f ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DQSDR1_DFTRDIDLF_MASK 0x000F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DQSDR1_DFTRDIDLF_SHIFT 16
/* Drift Back-to-Back Read Cycles Factor: Specifies the multipli ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DQSDR1_DFTRDB2BF_MASK 0x00F00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DQSDR1_DFTRDB2BF_SHIFT 20
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DQSDR1_PUDFTUP (1 << 24)
/* Controller Update DQS Drift Update. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DQSDR1_CUDFTUP (1 << 25)
/* Drift Back-to-Back Reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DQSDR1_DFTB2BRD_MASK 0x3C000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DQSDR1_DFTB2BRD_SHIFT 26
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DQSDR1_RESERVED_31_30_MASK 0xC0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DQSDR1_RESERVED_31_30_SHIFT 30

/**** DQSDR2 register ****/
/* Drift Monitor Period: Specifies the minimum number of clock c ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DQSDR2_DFTMNTPRD_MASK 0x0000FFFF
#define ALPINE_V3_DWC_DDR_PHY_REGS_DQSDR2_DFTMNTPRD_SHIFT 0
/* Drift Threshold: Specifies the minimum number of DQS drift de ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DQSDR2_DFTTHRSH_MASK 0x00FF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DQSDR2_DFTTHRSH_SHIFT 16
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DQSDR2_RESERVED_31_24_MASK 0xFF000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DQSDR2_RESERVED_31_24_SHIFT 24

/**** DCUAR register ****/
/* Cache Word Address: Address of the cache word to be written. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCUAR_CWADDR_W_MASK 0x0000000F
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCUAR_CWADDR_W_SHIFT 0
/* Cache Slice Address: Address of the cache slice to be written */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCUAR_CSADDR_W_MASK 0x000000F0
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCUAR_CSADDR_W_SHIFT 4
/* Cache Select: Selects the cache to be accessed */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCUAR_CSEL_MASK 0x00000300
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCUAR_CSEL_SHIFT 8
/* Increment Address: Specifies, if set, that the cache address  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCUAR_INCA (1 << 10)
/* Access Type: Specifies the type of access to be performed usi ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCUAR_ATYPE (1 << 11)
/* Cache Word Address: Address of the cache word to be read. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCUAR_CWADDR_R_MASK 0x0000F000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCUAR_CWADDR_R_SHIFT 12
/* Cache Slice Address: Address of the cache slice to be read. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCUAR_CSADDR_R_MASK 0x000F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCUAR_CSADDR_R_SHIFT 16
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCUAR_RESERVED_31_20_MASK 0xFFF00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCUAR_RESERVED_31_20_SHIFT 20

/**** DCURR register ****/
/* DCU Instruction: Selects the DCU command to be executed: Vali ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCURR_DINST_MASK 0x0000000F
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCURR_DINST_SHIFT 0
/* Start Address: Cache word address where the execution of comm ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCURR_SADDR_MASK 0x000000F0
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCURR_SADDR_SHIFT 4
/* End Address: Cache word address where the execution of comman ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCURR_EADDR_MASK 0x00000F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCURR_EADDR_SHIFT 8
/* Number of Failures: Specifies the number of failures after wh ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCURR_NFAIL_MASK 0x000FF000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCURR_NFAIL_SHIFT 12
/* Stop On Nth Fail: Specifies if set that the execution of comm ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCURR_SONF (1 << 20)
/* Stop Capture On Full: Specifies if set that the capture of re ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCURR_SCOF (1 << 21)
/* Read Capture Enable: Indicates if set that read data coming b ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCURR_RCEN (1 << 22)
/* Expected Compare Enable: Indicates if set that read data comi ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCURR_XCEN (1 << 23)
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCURR_RESERVED_31_24_MASK 0xFF000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCURR_RESERVED_31_24_SHIFT 24

/**** DCULR register ****/
/* Loop Start Address: Command cache word address where the loop ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCULR_LSADDR_MASK 0x0000000F
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCULR_LSADDR_SHIFT 0
/* Loop End Address: Command cache word address where the loop s ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCULR_LEADDR_MASK 0x000000F0
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCULR_LEADDR_SHIFT 4
/* Loop Count: The number of times that the loop should be execu ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCULR_LCNT_MASK 0x0000FF00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCULR_LCNT_SHIFT 8
/* Loop Infinite: Indicates if set that the loop should be execu ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCULR_LINF (1 << 16)
/* Increment DRAM Address: Indicates if set that DRAM addresses  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCULR_IDA (1 << 17)
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCULR_RESERVED_27_18_MASK 0x0FFC0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCULR_RESERVED_27_18_SHIFT 18
/* Expected Data Loop End Address: The last expected data cache  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCULR_XLEADDR_MASK 0xF0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCULR_XLEADDR_SHIFT 28

/**** DCUGCR register ****/
/* Read Capture Start Word: The capture and compare of read data ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCUGCR_RCSW_MASK 0x0000FFFF
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCUGCR_RCSW_SHIFT 0
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCUGCR_RESERVED_31_16_MASK 0xFFFF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCUGCR_RESERVED_31_16_SHIFT 16

/**** DCUTPR register ****/
/* DCU Generic Timing Parameter 0. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCUTPR_TDCUT0_MASK 0x000000FF
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCUTPR_TDCUT0_SHIFT 0
/* DCU Generic Timing Parameter 1. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCUTPR_TDCUT1_MASK 0x0000FF00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCUTPR_TDCUT1_SHIFT 8
/* DCU Generic Timing Parameter 2. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCUTPR_TDCUT2_MASK 0x00FF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCUTPR_TDCUT2_SHIFT 16
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCUTPR_RESERVED_31_24_MASK 0xFF000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCUTPR_RESERVED_31_24_SHIFT 24

/**** DCUSR0 register ****/
/* Run Done: Indicates if set that the DCU has finished executin ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCUSR0_RDONE (1 << 0)
/* Capture Fail: Indicates if set that that at least one read da ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCUSR0_CFAIL (1 << 1)
/* Capture Full: Indicates if set that the capture cache is full */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCUSR0_CFULL (1 << 2)
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCUSR0_RESERVED_31_3_MASK 0xFFFFFFF8
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCUSR0_RESERVED_31_3_SHIFT 3

/**** DCUSR1 register ****/
/* Read Count: Number of read words returned from the SDRAM. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCUSR1_RDCNT_MASK 0x0000FFFF
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCUSR1_RDCNT_SHIFT 0
/* Fail Count: Number of read words that have failed. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCUSR1_FLCNT_MASK 0x00FF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCUSR1_FLCNT_SHIFT 16
/* Loop Count: Indicates the value of the loop count */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCUSR1_LPCNT_MASK 0xFF000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DCUSR1_LPCNT_SHIFT 24

/**** BISTRR register ****/
/* BIST Instruction: Selects the BIST instruction to be executed ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTRR_BINST_MASK 0x00000007
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTRR_BINST_SHIFT 0

#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTRR_BINST_NOP		\
	(0x0 << ALPINE_V3_DWC_DDR_PHY_REGS_BISTRR_BINST_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTRR_BINST_RUN		\
	(0x1 << ALPINE_V3_DWC_DDR_PHY_REGS_BISTRR_BINST_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTRR_BINST_STOP		\
	(0x2 << ALPINE_V3_DWC_DDR_PHY_REGS_BISTRR_BINST_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTRR_BINST_RESET		\
	(0x3 << ALPINE_V3_DWC_DDR_PHY_REGS_BISTRR_BINST_SHIFT)

/* BIST Mode: Selects the mode in which BIST is run */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTRR_BMODE (1 << 3)
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTRR_BMODE_SHIFT	3

#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTRR_BMODE_LOOPBACK		\
	(0x0 << ALPINE_V3_DWC_DDR_PHY_REGS_BISTRR_BMODE_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTRR_BMODE_DRAM		\
	(0x1 << ALPINE_V3_DWC_DDR_PHY_REGS_BISTRR_BMODE_SHIFT)

/* BIST Infinite Run: Specifies if set that the BIST should be r ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTRR_BINF (1 << 4)
/* Number of Failures: Specifies the number of failures after wh ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTRR_NFAIL_MASK 0x00001FE0
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTRR_NFAIL_SHIFT 5
/* BIST Stop On Nth Fail: Specifies if set that the BIST should  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTRR_BSONF (1 << 13)
/* BIST DATX8 Enable: Enables the running of BIST on the data by ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTRR_BDXEN (1 << 14)
/* BIST AC Enable: Enables the running of BIST on the address/co ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTRR_BACEN (1 << 15)
/* BIST Data Mask Enable: Enables if set that the data mask BIST ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTRR_BDMEN (1 << 16)
/* BIST Data Pattern: Selects the data pattern used during BIST */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTRR_BDPAT_MASK 0x003E0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTRR_BDPAT_SHIFT 17

#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTRR_BDPAT_WALKING_0		\
	(0xD << ALPINE_V3_DWC_DDR_PHY_REGS_BISTRR_BDPAT_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTRR_BDPAT_WALKING_1		\
	(0xC << ALPINE_V3_DWC_DDR_PHY_REGS_BISTRR_BDPAT_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTRR_BDPAT_LFSR		\
	(0xF << ALPINE_V3_DWC_DDR_PHY_REGS_BISTRR_BDPAT_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTRR_BDPAT_USER		\
	(0xE << ALPINE_V3_DWC_DDR_PHY_REGS_BISTRR_BDPAT_SHIFT)

#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTRR_BDXSEL_MASK 0x03C00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTRR_BDXSEL_SHIFT 22

#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTRR_BCKSEL_MASK 0x0C000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTRR_BCKSEL_SHIFT 26
/* BIST DATX8 Select: Select the byte lane for comparison of loo ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTRR_BCCSEL_MASK 0x30000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTRR_BCCSEL_SHIFT 28
/* BIST CK Select: Selects the CK that should be used to registe ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTRR_BSOMA (1 << 30)
/* BIST Clock Cycle Select: Selects the clock numbers on which t ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTRR_RESERVED_31 (1 << 31)

/**** BISTWCR register ****/
/* BIST Word Count: Indicates the number of words to generate du ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTWCR_BWCNT_MASK 0x0000FFFF
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTWCR_BWCNT_SHIFT 0
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTWCR_RESERVED_MASK 0xFFFF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTWCR_RESERVED_SHIFT 16

/**** BISTMSKR0 register ****/
/* Mask bit for each of the up to 16 address bits. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTMSKR0_AMSK_MASK 0x0003FFFF
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTMSKR0_AMSK_SHIFT 0
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTMSKR0_RESERVED_18 (1 << 18)
/* Mask bit for the RAS. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTMSKR0_ACTMSK (1 << 19)
/* Mask bit for each of the up to 12 CS_N bits. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTMSKR0_CSMSK_MASK 0x00F00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTMSKR0_CSMSK_SHIFT 20
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTMSKR0_CSMSK_RSVD_MASK 0xFF000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTMSKR0_CSMSK_RSVD_SHIFT 24

/**** BISTMSKR1 register ****/
/* Mask bit for the data mask (DM) bit for nibble 1. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTMSKR1_X4DMMSK_MASK 0x0000000F
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTMSKR1_X4DMMSK_SHIFT 0
/* Mask bit for each of the up to 4 bank address bits. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTMSKR1_BAMSK_MASK 0x000000F0
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTMSKR1_BAMSK_SHIFT 4
/* Mask bit for each of the up to 8 CKE bits. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTMSKR1_CKEMSK_MASK 0x00000F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTMSKR1_CKEMSK_SHIFT 8
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTMSKR1_CKEMSK_RSVD_MASK 0x0000F000
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTMSKR1_CKEMSK_RSVD_SHIFT 12
/* Mask bit for each of the up to 8 ODT bits. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTMSKR1_ODTMSK_MASK 0x000F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTMSKR1_ODTMSK_SHIFT 16
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTMSKR1_ODTMSK_RSVD_MASK 0x00F00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTMSKR1_ODTMSK_RSVD_SHIFT 20
/* Mask bits for each of the up to 3 Chip IP bits. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTMSKR1_CIDMSK_MASK 0x03000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTMSKR1_CIDMSK_SHIFT 24
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTMSKR1_CIDMSK_RSVD (1 << 26)
/* Mask bit for the PAR_IN. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTMSKR1_PARINMSK (1 << 27)
/* Mask bit for the data mask (DM) bit. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTMSKR1_DMMSK_MASK 0xF0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTMSKR1_DMMSK_SHIFT 28

/**** BISTAR0 register ****/
/* BIST Column Address: Selects the SDRAM column address to be u ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTAR0_BCOL_MASK 0x00000FFF
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTAR0_BCOL_SHIFT 0
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTAR0_RESERVED_27_12_MASK 0x0FFFF000
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTAR0_RESERVED_27_12_SHIFT 12
/* BIST Bank Address: Selects the SDRAM bank address to be used  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTAR0_BBANK_MASK 0xF0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTAR0_BBANK_SHIFT 28

/**** BISTAR1 register ****/
/* BIST Rank: Selects the SDRAM rank to be used during BIST */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTAR1_BRANK_MASK 0x0000000F
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTAR1_BRANK_SHIFT 0
/* BIST Address Increment: Selects the value by which the SDRAM  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTAR1_BAINC_MASK 0x0000FFF0
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTAR1_BAINC_SHIFT 4
/* BIST Maximum Rank. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTAR1_BMRANK_MASK 0x000F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTAR1_BMRANK_SHIFT 16
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTAR1_RESERVED_31_20_MASK 0xFFF00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTAR1_RESERVED_31_20_SHIFT 20

/**** BISTAR2 register ****/
/* BIST Maximum Column Address: Specifies the maximum SDRAM colu ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTAR2_BMCOL_MASK 0x00000FFF
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTAR2_BMCOL_SHIFT 0
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTAR2_RESERVED_27_12_MASK 0x0FFFF000
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTAR2_RESERVED_27_12_SHIFT 12
/* BIST Maximum Bank Address: Specifies the maximum SDRAM bank a ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTAR2_BMBANK_MASK 0xF0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTAR2_BMBANK_SHIFT 28

/**** BISTAR3 register ****/
/* BIST Row Address: Selects the SDRAM row address to be used du ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTAR3_BROW_MASK 0x0003FFFF
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTAR3_BROW_SHIFT 0
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTAR3_RESERVED_31_18_MASK 0xFFFC0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTAR3_RESERVED_31_18_SHIFT 18

/**** BISTAR4 register ****/
/* BIST Maximum Row Address: Specifies the maximum SDRAM row add ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTAR4_BMROW_MASK 0x0003FFFF
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTAR4_BMROW_SHIFT 0
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTAR4_RESERVED_31_18_MASK 0xFFFC0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTAR4_RESERVED_31_18_SHIFT 18

/**** BISTUDPR register ****/
/* BIST User Data Pattern 0: Data to be applied on even DQ pins  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTUDPR_BUDP0_MASK 0x0000FFFF
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTUDPR_BUDP0_SHIFT 0

#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTUDPR_BUDP0(val)		\
	((val) << ALPINE_V3_DWC_DDR_PHY_REGS_BISTUDPR_BUDP0_SHIFT)

/* BIST User Data Pattern 1: Data to be applied on odd DQ pins d ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTUDPR_BUDP1_MASK 0xFFFF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTUDPR_BUDP1_SHIFT 16

#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTUDPR_BUDP1(val)		\
	((val) << ALPINE_V3_DWC_DDR_PHY_REGS_BISTUDPR_BUDP1_SHIFT)

/**** BISTGSR register ****/
/* BIST Done: Indicates if set that the BIST has finished execut ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTGSR_BDONE (1 << 0)
/* BIST Address/Command Error: indicates if set that there is a  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTGSR_BACERR (1 << 1)
/* BIST Data Error: indicates if set that there is a data compar ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTGSR_BDXERR_MASK 0x000007FC
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTGSR_BDXERR_SHIFT 2
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTGSR_X4DMBER_MASK 0x00007800
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTGSR_X4DMBER_SHIFT 11
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTGSR_RESERVED_19_15_MASK 0x000F8000
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTGSR_RESERVED_19_15_SHIFT 15
/* DM Bit Error. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTGSR_DMBER_MASK 0x00F00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTGSR_DMBER_SHIFT 20
/* RAS_n/ACT_n Bit Error: Indicates the number of bit errors on  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTGSR_RESERVED_27_24_MASK 0x0F000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTGSR_RESERVED_27_24_SHIFT 24
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTGSR_RASBER (1 << 28)
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTGSR_RESERVED_29 (1 << 29)
/* PAR Bit Error. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTGSR_PARBER (1 << 30)
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTGSR_RESERVED_31 (1 << 31)

/**** BISTWER0 register ****/
/* Address/Command Word Error: Indicates the number of word erro ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTWER0_ACWER_MASK 0x0003FFFF
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTWER0_ACWER_SHIFT 0
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTWER0_RESERVED_31_18_MASK 0xFFFC0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTWER0_RESERVED_31_18_SHIFT 18

/**** BISTWER1 register ****/
/* Byte Word Error: Indicates the number of word errors on the b ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTWER1_DXWER_MASK 0x0000FFFF
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTWER1_DXWER_SHIFT 0
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTWER1_RESERVED_31_16_MASK 0xFFFF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTWER1_RESERVED_31_16_SHIFT 16

/**** BISTBER0 register ****/
/* Bank Address Bit Error: Each group of two bits indicate the b ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER0_ABER_MASK 0x0000FFFF
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER0_ABER_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER0_RESERVED_31_16_MASK 0xFFFF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER0_RESERVED_31_16_SHIFT 16

/**** BISTBER1 register ****/
/* Bank Address Bit Error. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER1_BABER_MASK 0x0000000F
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER1_BABER_SHIFT 0
/* CS_N Bit Error. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER1_RESERVED_7_4_MASK 0x000000F0
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER1_RESERVED_7_4_SHIFT 4
/* CS_N Bit Error. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER1_CSBER_MASK 0x00000F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER1_CSBER_SHIFT 8
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER1_CSBER_RSVD_MASK 0x000FF000
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER1_CSBER_RSVD_SHIFT 12

/**** BISTBER2 register ****/
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER2_DQBER0_MASK 0x0000FFFF
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER2_DQBER0_SHIFT 0
/* Address Bit Error: Each group of two bits indicate the bit er ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER2_RESERVED_31_16_MASK 0xFFFF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER2_RESERVED_31_16_SHIFT 16

/*
 * Data Bit Error: The error count for even DQS cycles. The first 8 bits
 * indicate the error count for the first data beat (i.e. the data driven out
 * on DQ[7:0] on the rising edge of DQS). The second 8 bits indicate the
 * error on the second data beat (i.e. the error count of the data driven
 * out on DQ[7:0] on the falling edge of DQS).
 * For each of the 8-bit group, bit [0] is for DQ[0], [1] for DQ[1], and so on.
 */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER2_DQBER0_RISG_DQ_I_SHIFT(i)	((i))
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER2_DQBER0_RISG_DQ_I_MASK(i)	\
	(0x3 << ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER2_DQBER0_RISG_DQ_I_SHIFT(i))
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER2_DQBER0_FALL_DQ_I_SHIFT(i)	((i) + 16)
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER2_DQBER0_FALL_DQ_I_MASK(i)	\
	(0x3 << ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER2_DQBER0_FALL_DQ_I_SHIFT(i))

/**** BISTBER3 register ****/
/* Data Bit Error. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER3_DQBER1_MASK 0x0000FFFF
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER3_DQBER1_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER3_RESERVED_31_16_MASK 0xFFFF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER3_RESERVED_31_16_SHIFT 16

/*
 * Data Bit Error: The error count for odd DQS cycles. The first 8 bits
 * indicate the error count for the first data beat (i.e. the data driven out
 * on DQ[7:0] on the rising edge of DQS). The second 8 bits indicate the
 * error on the second data beat (i.e. the error count of the data driven
 * out on DQ[7:0] on the falling edge of DQS).
 * For each of the 8-bit group, bit [0] is for DQ[0], [1] for DQ[1], and so on.
 */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER3_DQBER1_RISG_DQ_I_SHIFT(i)	((i))
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER3_DQBER1_RISG_DQ_I_MASK(i)	\
	(0x3 << ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER3_DQBER1_RISG_DQ_I_SHIFT(i))
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER3_DQBER1_FALL_DQ_I_SHIFT(i)	((i) + 16)
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER3_DQBER1_FALL_DQ_I_MASK(i)	\
	(0x3 << ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER3_DQBER1_FALL_DQ_I_SHIFT(i))

/**** BISTBER4 register ****/
/* Address Bit Error. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER4_ABER_MASK 0x00000003
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER4_ABER_SHIFT 0
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER4_RESERVED_7_2_MASK 0x000000FC
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER4_RESERVED_7_2_SHIFT 2
/* Chip ID Bit Error. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER4_CIDBER_MASK 0x00000300
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER4_CIDBER_SHIFT 8
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER4_CIDBER_RSVD (1 << 10)
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER4_RESERVED_31_11_MASK 0xFFFFF800
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER4_RESERVED_31_11_SHIFT 11

/**** BISTWCSR register ****/
/* Address/Command Word Count: Indicates the number of words rec ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTWCSR_ACWCNT_MASK 0x0000FFFF
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTWCSR_ACWCNT_SHIFT 0
/* Byte Word Count: Indicates the number of words received from  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTWCSR_DXWCNT_MASK 0xFFFF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTWCSR_DXWCNT_SHIFT 16

/**** BISTFWR0 register ****/
/* Bit status during a word error for each of the up to 16 addre ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTFWR0_AWEBS_MASK 0x0003FFFF
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTFWR0_AWEBS_SHIFT 0
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTFWR0_ACTWEBS (1 << 18)
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTFWR0_RESERVED_19 (1 << 19)
/* Bit status during a word error for each of the up to 12 CS_N bits. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTFWR0_CSWEBS_MASK 0x00F00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTFWR0_CSWEBS_SHIFT 20
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTFWR0_CSWEBS_RSVD_MASK 0xFF000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTFWR0_CSWEBS_RSVD_SHIFT 24

/**** BISTFWR1 register ****/
/* Bit status during a word error for each of the up to 8 CKE bits. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTFWR1_CKEWEBS_MASK 0x0000000F
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTFWR1_CKEWEBS_SHIFT 0
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTFWR1_CKEWEBS_RSVD_MASK 0x000000F0
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTFWR1_CKEWEBS_RSVD_SHIFT 4
/* Bit status during a word error for each of the up to 8 ODT bits. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTFWR1_ODTWEBS_MASK 0x00000F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTFWR1_ODTWEBS_SHIFT 8
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTFWR1_ODTWEBS_RSVD_MASK 0x0000F000
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTFWR1_ODTWEBS_RSVD_SHIFT 12
/* Bit status during a word error for each of the up to 3 bank address bits. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTFWR1_BAWEBS_MASK 0x000F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTFWR1_BAWEBS_SHIFT 16
/* Bit status during a word error for each of the up to 3 chip ID bits. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTFWR1_CIDWEBS_MASK 0x00300000
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTFWR1_CIDWEBS_SHIFT 20
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTFWR1_CIDWEBS_RSVD (1 << 22)
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTFWR1_RESERVED_23 (1 << 23)
/* Bit status during a word error for the data mask (DM) bit on nibble 1. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTFWR1_X4DMWEBS_MASK 0x0F000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTFWR1_X4DMWEBS_SHIFT 24
/* Bit status during a word error for the data mask (DM) bit. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTFWR1_DMWEBS_MASK 0xF0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTFWR1_DMWEBS_SHIFT 28

/* Register BISTFWR2 */
/*
 * Bit status during a word error for each of the 8 data (DQ) bits.
 * The first 8 bits indicate the status of the first data beat
 * (i.e. the status of the data driven out on DQ[7:0] on the rising
 * edge of DQS).
 * The second 8 bits indicate the status of the second data beat
 * (i.e. the status of the data driven out on DQ[7:0] on the falling
 * edge of DQS),
 * and so on. For each of the 8-bit group, the first bit is for DQ[0],
 * the second bit is for DQ[1], and so on.
 */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTFWR2_DQWEBS_1ST_DQ_I(i)		(i)
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTFWR2_DQWEBS_2ND_DQ_I(i)		((i) + 8)
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTFWR2_DQWEBS_3RD_DQ_I(i)		((i) + 16)
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTFWR2_DQWEBS_4TH_DQ_I(i)		((i) + 24)

/**** BISTBER5 register ****/
/* CKE Bit Error. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER5_CKEBER_MASK 0x000000FF
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER5_CKEBER_SHIFT 0
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER5_RESERVED_15_8_MASK 0x0000FF00
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER5_RESERVED_15_8_SHIFT 8
/* ODT Bit Error. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER5_ODTBER_MASK 0x00FF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER5_ODTBER_SHIFT 16
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER5_RESERVED_31_24_MASK 0xFF000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_BISTBER5_RESERVED_31_24_SHIFT 24

/**** RANKIDR register ****/
/* Rank Write ID: Selects one of the up to 12 rank registers tha ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RANKIDR_RANKWID_MASK 0x0000000F
#define ALPINE_V3_DWC_DDR_PHY_REGS_RANKIDR_RANKWID_SHIFT 0
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RANKIDR_RESERVED_15_4_MASK 0x0000FFF0
#define ALPINE_V3_DWC_DDR_PHY_REGS_RANKIDR_RESERVED_15_4_SHIFT 4
/* Rank Read ID: Selects one of the up to 12 rank registers that ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RANKIDR_RANKRID_MASK 0x000F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_RANKIDR_RANKRID_SHIFT 16
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RANKIDR_RESERVED_31_20_MASK 0xFFF00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_RANKIDR_RESERVED_31_20_SHIFT 20

/**** RIOCR0 register ****/
/* SDRAM CS# On-Die Termination: Enables, when set, the on-die t ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR0_CSODT_MASK 0x0000000F
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR0_CSODT_SHIFT 0
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR0_CSODT_RSVD_MASK 0x00000FF0
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR0_CSODT_RSVD_SHIFT 4
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR0_RESERVED_15_12_MASK 0x0000F000
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR0_RESERVED_15_12_SHIFT 12
/* SDRAM CS# Power Down Receiver: Powers down, when set, the inp ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR0_CSPDR_MASK 0x000F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR0_CSPDR_SHIFT 16
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR0_CSPDR_RSVD_MASK 0x0FF00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR0_CSPDR_RSVD_SHIFT 20
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR0_RESERVED_31_28_MASK 0xF0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR0_RESERVED_31_28_SHIFT 28

/**** RIOCR1 register ****/
/* Rank On-Die Termination: Enables, when set, the on-die termin ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR1_CKEODT_MASK 0x0000000F
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR1_CKEODT_SHIFT 0
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR1_CKEODT_RSVD_MASK 0x000000F0
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR1_CKEODT_RSVD_SHIFT 4
/* Rank Power Down Receiver: Powers down, when set, the input re ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR1_CKEPDR_MASK 0x00000F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR1_CKEPDR_SHIFT 8
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR1_CKEPDR_RSVD_MASK 0x0000F000
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR1_CKEPDR_RSVD_SHIFT 12
/* Rank On-Die Termination: Enables, when set, the on-die termin ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR1_ODTODT_MASK 0x000F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR1_ODTODT_SHIFT 16
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR1_ODTODT_RSVD_MASK 0x00F00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR1_ODTODT_RSVD_SHIFT 20
/* Rank Power Down Receiver: Powers down, when set, the input re ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR1_ODTPDR_MASK 0x0F000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR1_ODTPDR_SHIFT 24
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR1_ODTPDR_RSVD_MASK 0xF0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR1_ODTPDR_RSVD_SHIFT 28

/**** RIOCR2 register ****/
/* SDRAM CS_n Output Enable (OE) Mode Selection */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR2_CSOEMODE_MASK 0x000000FF
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR2_CSOEMODE_SHIFT 0
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR2_CSOEMODE_RSVD_MASK 0x00FFFF00
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR2_CSOEMODE_RSVD_SHIFT 8
/* SDRAM C Output Enable (OE) Mode Selection */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR2_COEMODE_MASK 0x0F000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR2_COEMODE_SHIFT 24
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR2_COEMODE_RSVD_MASK 0x30000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR2_COEMODE_RSVD_SHIFT 28
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR2_RESERVED_31_30_MASK 0xC0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR2_RESERVED_31_30_SHIFT 30

/**** RIOCR4 register ****/
/* SDRAM CKE Output Enable (OE) Mode Selection */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR4_CKEOEMODE_MASK 0x000000FF
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR4_CKEOEMODE_SHIFT 0
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR4_CKEOEMODE_RSVD_MASK 0x0000FF00
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR4_CKEOEMODE_RSVD_SHIFT 8
/* SDRAM CKE Power Down Driver (PDD) Mode Selection. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR4_RESERVED_31_16_MASK 0xFFFF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR4_RESERVED_31_16_SHIFT 16

/**** RIOCR5 register ****/
/* SDRAM On-die Termination Output Enable (OE) Mode Selection. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR5_ODTOEMODE_MASK 0x000000FF
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR5_ODTOEMODE_SHIFT 0
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR5_ODTOEMODE_RSVD_MASK 0x0000FF00
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR5_ODTOEMODE_RSVD_SHIFT 8
/* SDRAM On-die Termination Power Down Driver (PDD) Mode Selection. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR5_RESERVED_31_16_MASK 0xFFFF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_RIOCR5_RESERVED_31_16_SHIFT 16

/**** ACIOCR0 register ****/
/* Address/Command I/O Mode: Selects SSTL mode (when set to 0) o ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACIOCR0_ACIOM (1 << 0)
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACIOCR0_RESERVED_1 (1 << 1)
/* Address/Command On-Die Termination: Enables, when set, the on ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACIOCR0_ACODT (1 << 2)
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACIOCR0_RESERVED_3 (1 << 3)
/* AC Power Down Receiver: Powers down, when set, the input rece ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACIOCR0_ACPDR (1 << 4)
/* CK On-Die Termination: Enables, when set, the on-die terminat ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACIOCR0_CKODT_MASK 0x000001E0
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACIOCR0_CKODT_SHIFT 5
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACIOCR0_RESERVED_9 (1 << 9)
/* CK Power Down Receiver: Powers down, when set, the input rece ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACIOCR0_CKPDR_MASK 0x00003C00
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACIOCR0_CKPDR_SHIFT 10
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACIOCR0_RESERVED_25_14_MASK 0x03FFC000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACIOCR0_RESERVED_25_14_SHIFT 14
/* SDRAM Reset On-Die Termination: Enables, when set, the on-die ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACIOCR0_RSTODT (1 << 26)
/* SDRAM Reset Power Down Driver: Powers down, when set, the out ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACIOCR0_RESERVED_27 (1 << 27)
/* SDRAM Reset Power Down Receiver: Powers down, when set, the i ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACIOCR0_RSTPDR (1 << 28)
/* SDRAM Reset I/O Mode: Selects SSTL mode (when set to 0) or CM ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACIOCR0_RSTIOM (1 << 29)
/* Address/Command Slew Rate (D3F I/O Only): Selects slew rate o ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACIOCR0_ACSR_MASK 0xC0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACIOCR0_ACSR_SHIFT 30

/**** ACIOCR3 register ****/
/* SDRAM CK Output Enable (OE) Mode Selection */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACIOCR3_CKOEMODE_MASK 0x000000FF
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACIOCR3_CKOEMODE_SHIFT 0
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACIOCR3_RESERVED_15_8_MASK 0x0000FF00
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACIOCR3_RESERVED_15_8_SHIFT 8
/* SDRAM ACT_n Output Enable (OE) Mode Selection (DDR4 only) */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACIOCR3_ACTOEMODE_MASK 0x00030000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACIOCR3_ACTOEMODE_SHIFT 16
/* SDRAM A[16] / RAS_n Output Enable (OE) Mode Selection */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACIOCR3_A16OEMODE_MASK 0x000C0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACIOCR3_A16OEMODE_SHIFT 18
/* SDRAM A[17] Output Enable (OE) Mode Selection */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACIOCR3_A17OEMODE_MASK 0x00300000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACIOCR3_A17OEMODE_SHIFT 20
/* SDRAM Bank Address Output Enable (OE) Mode Selection */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACIOCR3_BAOEMODE_MASK 0x03C00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACIOCR3_BAOEMODE_SHIFT 22
/* SDRAM Bank Group Output Enable (OE) Mode Selection */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACIOCR3_BGOEMODE_MASK 0x3C000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACIOCR3_BGOEMODE_SHIFT 26
/* SDRAM Parity Output Enable (OE) Mode Selection */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACIOCR3_PAROEMODE_MASK 0xC0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACIOCR3_PAROEMODE_SHIFT 30

/**** IOVCR0 register ****/
/* REFSEL Control for internal AC IOs:  Selects the genereated V ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_IOVCR0_ACVREFISEL_MASK 0x0000003F
#define ALPINE_V3_DWC_DDR_PHY_REGS_IOVCR0_ACVREFISEL_SHIFT 0
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_IOVCR0_RESERVED_7_6_MASK 0x000000C0
#define ALPINE_V3_DWC_DDR_PHY_REGS_IOVCR0_RESERVED_7_6_SHIFT 6
/* Address/command lane Single-End VREF Select: Selects the gene ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_IOVCR0_ACREFSSEL_MASK 0x00003F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_IOVCR0_ACREFSSEL_SHIFT 8
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_IOVCR0_RESERVED_15_14_MASK 0x0000C000
#define ALPINE_V3_DWC_DDR_PHY_REGS_IOVCR0_RESERVED_15_14_SHIFT 14
/* Address/command lane External VREF Select: Selects the genera ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_IOVCR0_ACREFESEL_MASK 0x003F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_IOVCR0_ACREFESEL_SHIFT 16
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_IOVCR0_RESERVED_23_22_MASK 0x00C00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_IOVCR0_RESERVED_23_22_SHIFT 22
/* Address/command lane Internal VREF Enable: Enables the genera ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_IOVCR0_ACREFIEN (1 << 24)
/* Address/command lane Single-End VREF Enable: Enables the gene ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_IOVCR0_ACREFSEN (1 << 25)
/* Address/command lane Internal VREF Enable: Enables the genera ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_IOVCR0_ACREFEEN_MASK 0x0C000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_IOVCR0_ACREFEEN_SHIFT 26
/* Address/command lane VREF Pad Enable: Enables the pass gate b ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_IOVCR0_ACREFPEN (1 << 28)
/* Address/command lane VREF IOM (Used only by D4MU IOs) */
#define ALPINE_V3_DWC_DDR_PHY_REGS_IOVCR0_ACREFIOM_MASK 0xE0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_IOVCR0_ACREFIOM_SHIFT 29

/**** IOVCR1 register ****/
/* ZQ Internal VREF Select: Selects the generated VREF value for ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_IOVCR1_ZQREFISEL_MASK 0x0000003F
#define ALPINE_V3_DWC_DDR_PHY_REGS_IOVCR1_ZQREFISEL_SHIFT 0
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_IOVCR1_RESERVED_7_6_MASK 0x000000C0
#define ALPINE_V3_DWC_DDR_PHY_REGS_IOVCR1_RESERVED_7_6_SHIFT 6
/* ZQ Internal VREF Enable: Enables the generation of VREF for Z ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_IOVCR1_ZQREFIEN (1 << 8)
/* ZQ VREF Pad Enable: Enables the pass gate between (to connect ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_IOVCR1_ZQREFPEN (1 << 9)
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_IOVCR1_RESERVED_31_10_MASK 0xFFFFFC00
#define ALPINE_V3_DWC_DDR_PHY_REGS_IOVCR1_RESERVED_31_10_SHIFT 10

/**** VTCR0 register ****/
/* Initial DRAM DQ VREF value used during DRAM VREF training. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTCR0_DVINIT_MASK 0x0000003F
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTCR0_DVINIT_SHIFT 0
/* Minimum VREF limit value used during DRAM VREF training. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTCR0_DVMIN_MASK 0x00000FC0
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTCR0_DVMIN_SHIFT 6
/* Maximum VREF limit value used during DRAM VREF training. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTCR0_DVMAX_MASK 0x0003F000
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTCR0_DVMAX_SHIFT 12
/* DRAM DQ VREF step size used during DRAM VREF training */
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTCR0_DVSS_MASK 0x003C0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTCR0_DVSS_SHIFT 18
/* Reserved. Returns zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTCR0_RESERVED_26_22_MASK 0x07C00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTCR0_RESERVED_26_22_SHIFT 22
/* Per Device Addressability Enable: When Enabled, each device w ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTCR0_PDAEN (1 << 27)
/* DRM DQ VREF training Enable: When set, DQ VREF training will  ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTCR0_DVEN (1 << 28)
/* Number of ctl_clk required to meet (> 150ns) timing requireme ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTCR0_TVREF_MASK 0xE0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTCR0_TVREF_SHIFT 29

/**** VTCR1 register ****/
/* Host IO Type Control:  Controls whether IO VREF value will be ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTCR1_HVIO (1 << 0)
/* HOST (IO) internal VREF training Enable: When Set, IO VREF tr ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTCR1_HVEN (1 << 1)
/* Number of LCDL Eye points for which VREF training is repeated */
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTCR1_ENUM (1 << 2)
/* Eye LCDL Offset value for VREF training:  Valid values are:2b ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTCR1_EOFF_MASK 0x00000018
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTCR1_EOFF_SHIFT 3
/* Number of ctl_clk required to meet (> 200ns) VREF Settling ti ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTCR1_TVREFIO_MASK 0x000000E0
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTCR1_TVREFIO_SHIFT 5
/* Static Host Vref Rank Enable */
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTCR1_SHREN (1 << 8)
/* Static Host Vref Rank Value: When SHREN is enabled, SHRNK [1: ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTCR1_SHRNK_MASK 0x00000600
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTCR1_SHRNK_SHIFT 9
/* Reserved. Returns zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTCR1_RESERVED_11 (1 << 11)
/* VREF Word Count:  The number of times same memory location ra ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTCR1_VWCR_MASK 0x0000F000
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTCR1_VWCR_SHIFT 12
/* Minimum VREF limit value used during DRAM VREF training. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTCR1_HVMIN_MASK 0x003F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTCR1_HVMIN_SHIFT 16
/* Maximum VREF limit value used during DRAM VREF training. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTCR1_HVMAX_MASK 0x0FC00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTCR1_HVMAX_SHIFT 22
/* Host VREF step size used during VREF training */
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTCR1_HVSS_MASK 0xF0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_VTCR1_HVSS_SHIFT 28

/**** ACBDLR0 register ****/
/* CK0 Bit Delay: Delay select for the BDL on CK0. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR0_CK0BD_MASK 0x0000003F
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR0_CK0BD_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR0_RESERVED_7_6_MASK 0x000000C0
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR0_RESERVED_7_6_SHIFT 6
/* CK1 Bit Delay: Delay select for the BDL on CK1. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR0_CK1BD_MASK 0x00003F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR0_CK1BD_SHIFT 8
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR0_RESERVED_15_14_MASK 0x0000C000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR0_RESERVED_15_14_SHIFT 14
/* CK2 Bit Delay: Delay select for the BDL on CK2. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR0_CK2BD_MASK 0x003F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR0_CK2BD_SHIFT 16
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR0_RESERVED_23_22_MASK 0x00C00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR0_RESERVED_23_22_SHIFT 22
/* CK3 Bit Delay: Delay select for the BDL on CK3. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR0_CK3BD_MASK 0x3F000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR0_CK3BD_SHIFT 24
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR0_RESERVED_31_30_MASK 0xC0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR0_RESERVED_31_30_SHIFT 30

/**** ACBDLR1 register ****/
/* Delay select for the BDL on ACTN. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR1_ACTBD_MASK 0x0000003F
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR1_ACTBD_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR1_RESERVED_7_6_MASK 0x000000C0
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR1_RESERVED_7_6_SHIFT 6
/* Delay select for the BDL on Address A[17] */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR1_A17BD_MASK 0x00003F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR1_A17BD_SHIFT 8
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR1_RESERVED_15_14_MASK 0x0000C000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR1_RESERVED_15_14_SHIFT 14
/* Delay select for the BDL on Address A[16] */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR1_A16BD_MASK 0x003F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR1_A16BD_SHIFT 16
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR1_RESERVED_23_22_MASK 0x00C00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR1_RESERVED_23_22_SHIFT 22
/* Delay select for the BDL on Parity. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR1_PARBD_MASK 0x3F000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR1_PARBD_SHIFT 24
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR1_RESERVED_31_30_MASK 0xC0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR1_RESERVED_31_30_SHIFT 30

/**** ACBDLR2 register ****/
/* Delay select for the BDL on BA[0]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR2_BA0BD_MASK 0x0000003F
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR2_BA0BD_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR2_RESERVED_7_6_MASK 0x000000C0
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR2_RESERVED_7_6_SHIFT 6
/* Delay select for the BDL on BA[1]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR2_BA1BD_MASK 0x00003F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR2_BA1BD_SHIFT 8
/* Reser.ved Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR2_RESERVED_15_14_MASK 0x0000C000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR2_RESERVED_15_14_SHIFT 14
/* Delay select for the BDL on BG[0]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR2_BG0BD_MASK 0x003F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR2_BG0BD_SHIFT 16
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR2_RESERVED_23_22_MASK 0x00C00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR2_RESERVED_23_22_SHIFT 22
/* Delay select for the BDL on BG[1]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR2_BG1BD_MASK 0x3F000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR2_BG1BD_SHIFT 24
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR2_RESERVED_31_30_MASK 0xC0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR2_RESERVED_31_30_SHIFT 30

/**** ACBDLR3 register ****/
/* Delay select for the BDL on CS[0]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR3_CS0BD_MASK 0x0000003F
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR3_CS0BD_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR3_RESERVED_7_6_MASK 0x000000C0
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR3_RESERVED_7_6_SHIFT 6
/* Delay select for the BDL on CS[1]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR3_CS1BD_MASK 0x00003F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR3_CS1BD_SHIFT 8
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR3_RESERVED_15_14_MASK 0x0000C000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR3_RESERVED_15_14_SHIFT 14
/* Delay select for the BDL on CS[2]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR3_CS2BD_MASK 0x003F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR3_CS2BD_SHIFT 16
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR3_RESERVED_23_22_MASK 0x00C00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR3_RESERVED_23_22_SHIFT 22
/* Delay select for the BDL on CS[3]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR3_CS3BD_MASK 0x3F000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR3_CS3BD_SHIFT 24
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR3_RESERVED_31_30_MASK 0xC0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR3_RESERVED_31_30_SHIFT 30

/**** ACBDLR4 register ****/
/* Delay select for the BDL on ODT[0]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR4_ODT0BD_MASK 0x0000003F
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR4_ODT0BD_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR4_RESERVED_7_6_MASK 0x000000C0
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR4_RESERVED_7_6_SHIFT 6
/* Delay select for the BDL on ODT[1]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR4_ODT1BD_MASK 0x00003F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR4_ODT1BD_SHIFT 8
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR4_RESERVED_15_14_MASK 0x0000C000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR4_RESERVED_15_14_SHIFT 14
/* Delay select for the BDL on ODT[2]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR4_ODT2BD_MASK 0x003F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR4_ODT2BD_SHIFT 16
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR4_RESERVED_23_22_MASK 0x00C00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR4_RESERVED_23_22_SHIFT 22
/* Delay select for the BDL on ODT[3]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR4_ODT3BD_MASK 0x3F000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR4_ODT3BD_SHIFT 24
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR4_RESERVED_31_30_MASK 0xC0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR4_RESERVED_31_30_SHIFT 30

/**** ACBDLR5 register ****/
/* Delay select for the BDL on CKE[0]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR5_CKE0BD_MASK 0x0000003F
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR5_CKE0BD_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR5_RESERVED_7_6_MASK 0x000000C0
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR5_RESERVED_7_6_SHIFT 6
/* Delay select for the BDL on CKE[1]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR5_CKE1BD_MASK 0x00003F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR5_CKE1BD_SHIFT 8
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR5_RESERVED_15_14_MASK 0x0000C000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR5_RESERVED_15_14_SHIFT 14
/* Delay select for the BDL on CKE[2]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR5_CKE2BD_MASK 0x003F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR5_CKE2BD_SHIFT 16
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR5_RESERVED_23_22_MASK 0x00C00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR5_RESERVED_23_22_SHIFT 22
/* Delay select for the BDL on CKE[3]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR5_CKE3BD_MASK 0x3F000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR5_CKE3BD_SHIFT 24
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR5_RESERVED_31_30_MASK 0xC0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR5_RESERVED_31_30_SHIFT 30

/**** ACBDLR6 register ****/
/* Delay select for the BDL on Address A[0]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR6_A00BD_MASK 0x0000003F
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR6_A00BD_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR6_RESERVED_7_6_MASK 0x000000C0
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR6_RESERVED_7_6_SHIFT 6
/* Delay select for the BDL on Address A[1]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR6_A01BD_MASK 0x00003F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR6_A01BD_SHIFT 8
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR6_RESERVED_15_14_MASK 0x0000C000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR6_RESERVED_15_14_SHIFT 14
/* Delay select for the BDL on Address A[2]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR6_A02BD_MASK 0x003F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR6_A02BD_SHIFT 16
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR6_RESERVED_23_22_MASK 0x00C00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR6_RESERVED_23_22_SHIFT 22
/* Delay select for the BDL on Address A[3]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR6_A03BD_MASK 0x3F000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR6_A03BD_SHIFT 24
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR6_RESERVED_31_30_MASK 0xC0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR6_RESERVED_31_30_SHIFT 30

/**** ACBDLR7 register ****/
/* Delay select for the BDL on Address A[4]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR7_A04BD_MASK 0x0000003F
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR7_A04BD_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR7_RESERVED_7_6_MASK 0x000000C0
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR7_RESERVED_7_6_SHIFT 6
/* Delay select for the BDL on Address A[5]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR7_A05BD_MASK 0x00003F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR7_A05BD_SHIFT 8
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR7_RESERVED_15_14_MASK 0x0000C000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR7_RESERVED_15_14_SHIFT 14
/* Delay select for the BDL on Address A[6]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR7_A06BD_MASK 0x003F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR7_A06BD_SHIFT 16
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR7_RESERVED_23_22_MASK 0x00C00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR7_RESERVED_23_22_SHIFT 22
/* Delay select for the BDL on Address A[7]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR7_A07BD_MASK 0x3F000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR7_A07BD_SHIFT 24
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR7_RESERVED_31_30_MASK 0xC0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR7_RESERVED_31_30_SHIFT 30

/**** ACBDLR8 register ****/
/* Delay select for the BDL on Address A[8]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR8_A08BD_MASK 0x0000003F
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR8_A08BD_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR8_RESERVED_7_6_MASK 0x000000C0
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR8_RESERVED_7_6_SHIFT 6
/* Delay select for the BDL on Address A[9]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR8_A09BD_MASK 0x00003F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR8_A09BD_SHIFT 8
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR8_RESERVED_15_14_MASK 0x0000C000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR8_RESERVED_15_14_SHIFT 14
/* Delay select for the BDL on Address A[10]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR8_A10BD_MASK 0x003F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR8_A10BD_SHIFT 16
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR8_RESERVED_23_22_MASK 0x00C00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR8_RESERVED_23_22_SHIFT 22
/* Delay select for the BDL on Address A[11]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR8_A11BD_MASK 0x3F000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR8_A11BD_SHIFT 24
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR8_RESERVED_31_30_MASK 0xC0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR8_RESERVED_31_30_SHIFT 30

/**** ACBDLR9 register ****/
/* Delay select for the BDL on Address A[12]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR9_A12BD_MASK 0x0000003F
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR9_A12BD_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR9_RESERVED_7_6_MASK 0x000000C0
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR9_RESERVED_7_6_SHIFT 6
/* Delay select for the BDL on Address A[13]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR9_A13BD_MASK 0x00003F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR9_A13BD_SHIFT 8
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR9_RESERVED_15_14_MASK 0x0000C000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR9_RESERVED_15_14_SHIFT 14
/* Delay select for the BDL on Address A[14]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR9_A14BD_MASK 0x003F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR9_A14BD_SHIFT 16
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR9_RESERVED_23_22_MASK 0x00C00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR9_RESERVED_23_22_SHIFT 22
/* Delay select for the BDL on Address A[15]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR9_A15BD_MASK 0x3F000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR9_A15BD_SHIFT 24
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR9_RESERVED_31_30_MASK 0xC0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR9_RESERVED_31_30_SHIFT 30

/**** ACBDLR10 register ****/
/* Delay select for the BDL on AC PDD. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR10_RESERVED_7_0_MASK 0x000000FF
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR10_RESERVED_7_0_SHIFT 0
/* Delay select for the BDL on Chip ID CID[0]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR10_CID0BD_MASK 0x00003F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR10_CID0BD_SHIFT 8
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR10_RESERVED_15_14_MASK 0x0000C000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR10_RESERVED_15_14_SHIFT 14
/* Delay select for the BDL on Chip ID CID[1]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR10_CID1BD_MASK 0x003F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR10_CID1BD_SHIFT 16
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR10_RESERVED_23_22_MASK 0x00C00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR10_RESERVED_23_22_SHIFT 22
/* Delay select for the BDL on Chip ID CID[2]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR10_CID2BD_MASK 0x3F000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR10_CID2BD_SHIFT 24
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR10_RESERVED_31_30_MASK 0xC0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR10_RESERVED_31_30_SHIFT 30

/**** ACBDLR11 register ****/
/* Delay select for the BDL on CS[4]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR11_CS4BD_MASK 0x0000003F
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR11_CS4BD_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR11_RESERVED_7_6_MASK 0x000000C0
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR11_RESERVED_7_6_SHIFT 6
/* Delay select for the BDL on CS[5]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR11_CS5BD_MASK 0x00003F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR11_CS5BD_SHIFT 8
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR11_RESERVED_15_14_MASK 0x0000C000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR11_RESERVED_15_14_SHIFT 14
/* Delay select for the BDL on CS[6]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR11_CS6BD_MASK 0x003F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR11_CS6BD_SHIFT 16
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR11_RESERVED_23_22_MASK 0x00C00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR11_RESERVED_23_22_SHIFT 22
/* Delay select for the BDL on CS[7]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR11_CS7BD_MASK 0x3F000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR11_CS7BD_SHIFT 24
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR11_RESERVED_31_30_MASK 0xC0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR11_RESERVED_31_30_SHIFT 30

/**** ACBDLR12 register ****/
/* Delay select for the BDL on CS[8]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR12_CS8BD_MASK 0x0000003F
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR12_CS8BD_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR12_RESERVED_7_6_MASK 0x000000C0
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR12_RESERVED_7_6_SHIFT 6
/* Delay select for the BDL on CS[9]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR12_CS9BD_MASK 0x00003F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR12_CS9BD_SHIFT 8
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR12_RESERVED_15_14_MASK 0x0000C000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR12_RESERVED_15_14_SHIFT 14
/* Delay select for the BDL on CS[10]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR12_CS10BD_MASK 0x003F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR12_CS10BD_SHIFT 16
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR12_RESERVED_23_22_MASK 0x00C00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR12_RESERVED_23_22_SHIFT 22
/* Delay select for the BDL on CS[11]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR12_CS11BD_MASK 0x3F000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR12_CS11BD_SHIFT 24
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR12_RESERVED_31_30_MASK 0xC0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR12_RESERVED_31_30_SHIFT 30

/**** ACBDLR13 register ****/
/* Delay select for the BDL on ODT[4]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR13_ODT4BD_MASK 0x0000003F
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR13_ODT4BD_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR13_RESERVED_7_6_MASK 0x000000C0
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR13_RESERVED_7_6_SHIFT 6
/* Delay select for the BDL on ODT[5]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR13_ODT5BD_MASK 0x00003F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR13_ODT5BD_SHIFT 8
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR13_RESERVED_15_14_MASK 0x0000C000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR13_RESERVED_15_14_SHIFT 14
/* Delay select for the BDL on ODT[6]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR13_ODT6BD_MASK 0x003F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR13_ODT6BD_SHIFT 16
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR13_RESERVED_23_22_MASK 0x00C00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR13_RESERVED_23_22_SHIFT 22
/* Delay select for the BDL on ODT[7]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR13_ODT7BD_MASK 0x3F000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR13_ODT7BD_SHIFT 24
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR13_RESERVED_31_30_MASK 0xC0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR13_RESERVED_31_30_SHIFT 30

/**** ACBDLR14 register ****/
/* Delay select for the BDL on CKE[4]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR14_CKE4BD_MASK 0x0000003F
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR14_CKE4BD_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR14_RESERVED_7_6_MASK 0x000000C0
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR14_RESERVED_7_6_SHIFT 6
/* Delay select for the BDL on CKE[5]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR14_CKE5BD_MASK 0x00003F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR14_CKE5BD_SHIFT 8
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR14_RESERVED_15_14_MASK 0x0000C000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR14_RESERVED_15_14_SHIFT 14
/* Delay select for the BDL on CKE[6]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR14_CKE6BD_MASK 0x003F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR14_CKE6BD_SHIFT 16
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR14_RESERVED_23_22_MASK 0x00C00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR14_RESERVED_23_22_SHIFT 22
/* Delay select for the BDL on CKE[7]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR14_CKE7BD_MASK 0x3F000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR14_CKE7BD_SHIFT 24
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR14_RESERVED_31_30_MASK 0xC0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACBDLR14_RESERVED_31_30_SHIFT 30

/**** ACLCDLR register ****/
/* Address/Command Delay: Delay select for the address/command ( ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACLCDLR_ACD_MASK 0x000001FF
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACLCDLR_ACD_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACLCDLR_RESERVED_31_9_MASK 0xFFFFFE00
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACLCDLR_RESERVED_31_9_SHIFT 9

/**** ACMDLR0 register ****/
/* Initial Period: Initial period measured by the master delay l ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACMDLR0_IPRD_MASK 0x000001FF
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACMDLR0_IPRD_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACMDLR0_RESERVED_15_9_MASK 0x0000FE00
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACMDLR0_RESERVED_15_9_SHIFT 9
/* Target Period: Target period measured by the master delay lin ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACMDLR0_TPRD_MASK 0x01FF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACMDLR0_TPRD_SHIFT 16
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACMDLR0_RESERVED_31_25_MASK 0xFE000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACMDLR0_RESERVED_31_25_SHIFT 25

/**** ACMDLR1 register ****/
/* MDL Delay: Delay select for the LCDL for the Master Delay Lin ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACMDLR1_MDLD_MASK 0x000001FF
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACMDLR1_MDLD_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACMDLR1_RESERVED_31_9_MASK 0xFFFFFE00
#define ALPINE_V3_DWC_DDR_PHY_REGS_ACMDLR1_RESERVED_31_9_SHIFT 9

/**** ZQCR register ****/
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQCR_RESERVED_0 (1 << 0)
/* Termination OFF. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQCR_TERM_OFF (1 << 1)
/* ZQ Power Down. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQCR_ZQPD (1 << 2)
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQCR_RESERVED_3 (1 << 3)
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQCR_RESERVED_7_4_MASK 0x000000F0
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQCR_RESERVED_7_4_SHIFT 4
/* Programmable Wait: specifies the number of clock cycles to re ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQCR_PGWAIT_MASK 0x00000700
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQCR_PGWAIT_SHIFT 8
/* Impedance Calibration Type: Valid decoding of the calibration ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQCR_ZCALT_MASK 0x00003800
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQCR_ZCALT_SHIFT 11
/* Maximum number of averaging rounds to be used by averaging al ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQCR_AVGMAX_MASK 0x0000C000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQCR_AVGMAX_SHIFT 14

#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQCR_AVGMAX_2		\
	(0 << ALPINE_V3_DWC_DDR_PHY_REGS_ZQCR_AVGMAX_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQCR_AVGMAX_4		\
	(1 << ALPINE_V3_DWC_DDR_PHY_REGS_ZQCR_AVGMAX_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQCR_AVGMAX_8		\
	(2 << ALPINE_V3_DWC_DDR_PHY_REGS_ZQCR_AVGMAX_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQCR_AVGMAX_16		\
	(3 << ALPINE_V3_DWC_DDR_PHY_REGS_ZQCR_AVGMAX_SHIFT)

/* Averaging algorithm enable, if set, enables averaging algorit ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQCR_AVGEN (1 << 16)
/* IO VT Drift Limit: Specifies the minimum change in the Impeda ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQCR_IODLMT_MASK 0x00FE0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQCR_IODLMT_SHIFT 17
/* Enable Asynmmetric drive strength calibration for all segment ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQCR_ASYM_DRV_EN (1 << 24)
/* Enable pullup only ODT calibration for all segments */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQCR_PU_ODT_ONLY (1 << 25)
/* Disable non-linear compensation for single ended termination */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQCR_DIS_NON_LIN_COMP (1 << 26)
/* When set to 1b1, forces a ZCAL VT update to the impedance cal ... */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQCR_FORCE_ZCAL_VT_UPDATE (1 << 27)
/* Bits 31 - 28 of ZCTRL bus going to PZCTRL cell (for D4MU IOs). */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQCR_ZCTRL_UPPER_MASK 0xF0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQCR_ZCTRL_UPPER_SHIFT 28


/**** ZQnPR register ****/
/* Impedance Divide Ratio. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_MASK 0x000000FF
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_SHIFT 0

#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_MASK 0x0000000F
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_SHIFT 0
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_MASK 0x000000F0
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_SHIFT 4

#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_80OHM		\
	(0x5 << ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_68OHM		\
	(0x6 << ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_60OHM		\
	(0x7 << ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_53OHM		\
	(0x8 << ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_48OHM		\
	(0x9 << ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_44OHM		\
	(0xA << ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_40OHM		\
	(0xB << ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_37OHM		\
	(0xC << ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_34OHM		\
	(0xD << ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_32OHM		\
	(0xE << ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_30OHM		\
	(0xF << ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_SHIFT)

#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_200OHM		\
	(0x1 << ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_133OHM		\
	(0x2 << ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_100OHM		\
	(0x3 << ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_77OHM		\
	(0x4 << ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_66OHM		\
	(0x5 << ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_56OHM		\
	(0x6 << ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_50OHM		\
	(0x7 << ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_44OHM		\
	(0x8 << ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_40OHM		\
	(0x9 << ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_36OHM		\
	(0xA << ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_33OHM		\
	(0xB << ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_30OHM		\
	(0xC << ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_28OHM		\
	(0xD << ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_26OHM		\
	(0xE << ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_SHIFT)
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_25OHM		\
	(0xF << ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_SHIFT)

/* Impedance Divide Ratio. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZPROG_ASYM_DRV_PU_MASK 0x00000F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZPROG_ASYM_DRV_PU_SHIFT 8
/* Impedance Divide Ratio. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZPROG_ASYM_DRV_PD_MASK 0x0000F000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZPROG_ASYM_DRV_PD_SHIFT 12
/* Impedance Divide Ratio. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZPROG_PU_ODT_ONLY_MASK 0x000F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZPROG_PU_ODT_ONLY_SHIFT 16
/* Pullup drive strength adjustment. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_PU_DRV_ADJUST_MASK 0x00300000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_PU_DRV_ADJUST_SHIFT 20
/* Pulldown drive strength adjustment. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_PD_DRV_ADJUST_MASK 0x00C00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_PD_DRV_ADJUST_SHIFT 22
/* Bits 31:28 of ZCTRL bus going to PZCTRL cell (for D4MU IOs). */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZCTRL_UPPER_MASK 0x0F000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_ZCTRL_UPPER_SHIFT 24
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_RESERVED_31_28_MASK 0xF0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnPR_RESERVED_31_28_SHIFT 28

/**** ZQnDR register ****/
/* Impedance Controller Impedance Data: */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnDR_ZDATA_MASK 0x0FFFFFFF
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnDR_ZDATA_SHIFT 0
/* Reserved.  Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnDR_RESERVED_28 (1 << 28)
/* Calibration segment bypass. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnDR_ZSEGBYP (1 << 29)
/* Termination over-ride Enable. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnDR_ODT_ZDEN (1 << 30)
/* Drive strength over-ride enable. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnDR_DRV_ZDEN (1 << 31)

/**** ZQnSR register ****/
/* Output impedance pull-down calibration status. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnSR_ZPD_MASK 0x00000003
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnSR_ZPD_SHIFT 0
/* Output impedance pull-up calibration status. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnSR_ZPU_MASK 0x0000000C
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnSR_ZPU_SHIFT 2
/* On-die termination (ODT) pull-down calibration status. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnSR_OPD_MASK 0x00000030
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnSR_OPD_SHIFT 4
/* On-die termination (ODT) pull-up calibration status. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnSR_OPU_MASK 0x000000C0
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnSR_OPU_SHIFT 6
/* Impedance Calibration Error. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnSR_ZERR (1 << 8)
/* Impedance Calibration Done. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnSR_ZDONE (1 << 9)
/* Pullup drive strength code saturated due to drive strength adjustment setting in ZQnPR register.
 */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnSR_PU_DRV_SAT (1 << 10)
/* Pulldown drive strength code saturated due to drive strength adjustment setting in ZQnPR
 * register. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnSR_PD_DRV_SAT (1 << 11)
/* Reserved. Return zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnSR_RESERVED_31_12_MASK 0xFFFFF000
#define ALPINE_V3_DWC_DDR_PHY_REGS_ZQnSR_RESERVED_31_12_SHIFT 12

/**** DXNGCR0 register ****/
/* Data Byte Enable. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR0_DXEN (1 << 0)
/* Data I/O Mode. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR0_DXIOM (1 << 1)
/* DQSG Output Enable. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR0_DQSGOE (1 << 2)
/* DQSG On-Die Termination. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR0_DQSGODT (1 << 3)
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR0_RESERVED_4 (1 << 4)
/* DQSG Power Down Receiver. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR0_DQSGPDR (1 << 5)
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR0_RESERVED_6 (1 << 6)
/* PDR Additive Latency. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR0_PDRAL_MASK 0x00000180
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR0_PDRAL_SHIFT 7
/* RTT Output Hold. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR0_RTTOH_MASK 0x00000600
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR0_RTTOH_SHIFT 9
/* RTT On Additive Latency. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR0_RTTOAL (1 << 11)
/* DQSSE Power Down Receiver. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR0_DQSSEPDR (1 << 12)
/* DQSNSE Power Down Receiver. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR0_DQSNSEPDR (1 << 13)
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR0_RESERVED_15_14_MASK 0x0000C000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR0_RESERVED_15_14_SHIFT 14
/* PLL Reset. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR0_PLLRST (1 << 16)
/* PLL Power Down. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR0_PLLPD (1 << 17)
/* Gear Shift. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR0_GSHIFT (1 << 18)
/* PLL Bypass. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR0_PLLBYP (1 << 19)
/* Number of Cycles (in terms of ctl_clk) to generate ctl_dx_get_static_rd input for the respective
 * bypte lane of the PHY. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR0_RDDLY_MASK 0x00F00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR0_RDDLY_SHIFT 20
/* Reserved. Returns zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR0_RESERVED_29_24_MASK 0x3F000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR0_RESERVED_29_24_SHIFT 24
/* Master Delay Line Enable. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR0_MDLEN (1 << 30)
/* Calibration Bypass. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR0_CALBYP (1 << 31)

/**** DXNGCR1 register ****/
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR1_RESERVED_15_0_MASK 0x0000FFFF
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR1_RESERVED_15_0_SHIFT 0
/* Enables the PDR mode values for DQ[7:0]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR1_DXPDRMODE_MASK 0xFFFF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR1_DXPDRMODE_SHIFT 16

/**** DXNGCR2 register ****/
/* Enables the TE (ODT) mode values for DQ[7:0]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR2_DXTEMODE_MASK 0x0000FFFF
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR2_DXTEMODE_SHIFT 0
/* Enables the OE mode values for DQ[7:0]. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR2_DXOEMODE_MASK 0xFFFF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR2_DXOEMODE_SHIFT 16

/**** DXNGCR3 register ****/
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR3_RESERVED_1_0_MASK 0x00000003
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR3_RESERVED_1_0_SHIFT 0
/* Enables the PDR mode values for DQS. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR3_DSPDRMODE_MASK 0x0000000C
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR3_DSPDRMODE_SHIFT 2
/* Enables the TE mode values for DQS. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR3_DSTEMODE_MASK 0x00000030
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR3_DSTEMODE_SHIFT 4
/* Enables the OE mode values for DQS. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR3_DSOEMODE_MASK 0x000000C0
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR3_DSOEMODE_SHIFT 6
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR3_RESERVED_9_8_MASK 0x00000300
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR3_RESERVED_9_8_SHIFT 8
/* Enables the PDR mode values for DM. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR3_DMPDRMODE_MASK 0x00000C00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR3_DMPDRMODE_SHIFT 10
/* Enables the TE mode values for DM. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR3_DMTEMODE_MASK 0x00003000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR3_DMTEMODE_SHIFT 12
/* Enables the OE mode values for DM. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR3_DMOEMODE_MASK 0x0000C000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR3_DMOEMODE_SHIFT 14
/* Reserved. Returns zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR3_RESERVED_17_16_MASK 0x00030000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR3_RESERVED_17_16_SHIFT 16
/* Output Enable BDL VT Compensation. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR3_OEBVT (1 << 18)
/* Power Down Receiver BDL Delay VT Compensation. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR3_PDRBVT (1 << 19)
/* Termination Enable BDL Delay VT Compensation. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR3_TEBVT (1 << 20)
/* Write Data Strobe BDL Delay VT Compensation. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR3_WDSBVT (1 << 21)
/* Read Data Strobe BDL Delay VT Compensation. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR3_RDSBVT (1 << 22)
/* Read DQS Gating Status  LCDL Delay VT Compensation. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR3_RGSLVT (1 << 23)
/* Write Leveling LCDL Delay VT Compensation. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR3_WLLVT (1 << 24)
/* Write DQ LCDL Delay VT Compensation. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR3_WDLVT (1 << 25)
/* Read DQS LCDL Delay VT Compensation. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR3_RDLVT (1 << 26)
/* Read DQS Gating LCDL Delay VT Compensation. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR3_RGLVT (1 << 27)
/* Write Data BDL VT Compensation. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR3_WDBVT (1 << 28)
/* Read Data BDL VT Compensation. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR3_RDBVT (1 << 29)
/* Write Data Mask  BDL VT Compensation. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR3_WDMBVT (1 << 30)
/* Read Data Mask  BDL VT Compensation. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR3_RDMBVT (1 << 31)

/**** DXNGCR4 register ****/
/* VRMON control for DQ IO (Single Ended) buffers of a byte lane. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR4_DXREFIMON_MASK 0x00000003
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR4_DXREFIMON_SHIFT 0
/* VREF Enable control for DQ IO (Single Ended) buffers of a byte lane. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR4_DXREFIEN_MASK 0x0000003C
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR4_DXREFIEN_SHIFT 2
/* Reserved. Returns zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR4_RESERVED_7_6_MASK 0x000000C0
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR4_RESERVED_7_6_SHIFT 6
/* Byte Lane Single-End VREF Select. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR4_DXREFSSEL_MASK 0x00003F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR4_DXREFSSEL_SHIFT 8
/* Reserved. Returns zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR4_RESERVED_15_14_MASK 0x0000C000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR4_RESERVED_15_14_SHIFT 14
/* Byte Lane External VREF Select. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR4_DXREFESEL_MASK 0x003F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR4_DXREFESEL_SHIFT 16
/* Reserved. Returns zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR4_RESERVED_24_22_MASK 0x01C00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR4_RESERVED_24_22_SHIFT 22
/* Byte Lane Single-End VREF Enable. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR4_DXREFSEN (1 << 25)
/* Byte Lane Internal VREF Enable. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR4_DXREFEEN_MASK 0x0C000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR4_DXREFEEN_SHIFT 26
/* Byte Lane VREF Pad Enable. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR4_DXREFPEN (1 << 28)
/* Byte lane VREF IOM (Used only by D4MU IOs) */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR4_DXREFIOM_MASK 0xE0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR4_DXREFIOM_SHIFT 29

/**** DXNGCR5 register ****/
/* Byte Lane internal VREF Select for Rank0. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR5_DXREFISELR0_MASK 0x0000003F
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR5_DXREFISELR0_SHIFT 0
/* Reserved. Returns zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR5_RESERVED_7_6_MASK 0x000000C0
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR5_RESERVED_7_6_SHIFT 6
/* Byte Lane internal VREF Select for Rank 1. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR5_DXREFISELR1_MASK 0x00003F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR5_DXREFISELR1_SHIFT 8
/* Reserved. Returns zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR5_RESERVED_15_14_MASK 0x0000C000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR5_RESERVED_15_14_SHIFT 14
/* Byte Lane internal VREF Select for Rank2. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR5_DXREFISELR2_MASK 0x003F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR5_DXREFISELR2_SHIFT 16
/* Reserved. Returns zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR5_RESERVED_23_22_MASK 0x00C00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR5_RESERVED_23_22_SHIFT 22
/* Byte Lane internal VREF Select for Rank3. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR5_DXREFISELR3_MASK 0x3F000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR5_DXREFISELR3_SHIFT 24
/* Reserved. Returns zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR5_RESERVED_31_30_MASK 0xC0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR5_RESERVED_31_30_SHIFT 30

/**** DXNGCR6 register ****/
/* DRAM DQ VREF Select for Rank0. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR6_DXDQVREFR0_MASK 0x0000003F
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR6_DXDQVREFR0_SHIFT 0
/* Reserved. Returns zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR6_RESERVED_7_6_MASK 0x000000C0
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR6_RESERVED_7_6_SHIFT 6
/* DRAM DQ VREF Select for Rank1. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR6_DXDQVREFR1_MASK 0x00003F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR6_DXDQVREFR1_SHIFT 8
/* Reserved. Returns zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR6_RESERVED_15_14_MASK 0x0000C000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR6_RESERVED_15_14_SHIFT 14
/* DRAM DQ VREF Select for Rank2. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR6_DXDQVREFR2_MASK 0x003F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR6_DXDQVREFR2_SHIFT 16
/* Reserved. Returns zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR6_RESERVED_23_22_MASK 0x00C00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR6_RESERVED_23_22_SHIFT 22
/* DRAM DQ VREF Select for Rank3. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR6_DXDQVREFR3_MASK 0x3F000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR6_DXDQVREFR3_SHIFT 24
/* Reserved. Returns zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR6_RESERVED_31_30_MASK 0xC0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR6_RESERVED_31_30_SHIFT 30

/**** DXNGCR7 register ****/
/* Reserved. Returns zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR7_RESERVED_1_0_MASK 0x00000003
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR7_RESERVED_1_0_SHIFT 0
/* Enables the PDR mode values for DQS[1] when using  macro. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR7_X4DSPDRMODE_MASK 0x0000000C
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR7_X4DSPDRMODE_SHIFT 2
/* Enables the TE mode values for DQS[1] when using DATX4X2 macro. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR7_X4DSTEMODE_MASK 0x00000030
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR7_X4DSTEMODE_SHIFT 4
/* Enables the OE mode values for DQS[1] when using DATX4X2 macro. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR7_X4DSOEMODE_MASK 0x000000C0
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR7_X4DSOEMODE_SHIFT 6
/* Reserved. Returns zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR7_RESERVED_9_8_MASK 0x00000300
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR7_RESERVED_9_8_SHIFT 8
/* Enables the PDR mode values for DQ[9] when using DATX4X2 macro. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR7_X4DXPDRMODE_MASK 0x00000C00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR7_X4DXPDRMODE_SHIFT 10
/* Enables the TE mode values for DQ[9] when using DATX4X2 macro. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR7_X4DXTEMODE_MASK 0x00003000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR7_X4DXTEMODE_SHIFT 12
/* Enables the OE mode values for DQ[9] when using DATX4X2 macro. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR7_X4DXOEMODE_MASK 0x0000C000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR7_X4DXOEMODE_SHIFT 14
/* DQSG Output Enable. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR7_X4DQSGOE (1 << 16)
/* DQSG On-Die Termination. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR7_X4DQSGODT (1 << 17)
/* Reserved. Returns zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR7_RESERVED_18 (1 << 18)
/* DQSG Power Down Receiver. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR7_X4DQSGPDR (1 << 19)
/* Reserved. Returns zeros on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR7_RESERVED_20 (1 << 20)
/* DQSSE Power Down Receiver. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR7_X4DQSSEPDR (1 << 21)
/* DQSNSE Power Down Receiver. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR7_X4DQSNSEPDR (1 << 22)
/* RTT Output Hold. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR7_X4RTTOH_MASK 0x01800000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR7_X4RTTOH_SHIFT 23
/* RTT On Additive Latency. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR7_X4RTTOAL (1 << 25)
/* Number of Cycles (in terms of ctl_clk) to generate ctl_dx_get_static_rd input for the respective
 * byte lane second nibble of the PHY. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR7_X4RDDLY_MASK 0x3C000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR7_X4RDDLY_SHIFT 26
/* Reserved. Returns zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR7_RESERVED_31_30_MASK 0xC0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR7_RESERVED_31_30_SHIFT 30

/**** DXNGCR8 register ****/
/* Byte Lane (upper nibble) internal VREF Select for Rank0. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR8_X4DXREFISELR0_MASK 0x0000003F
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR8_X4DXREFISELR0_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR8_RESERVED_7_6_MASK 0x000000C0
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR8_RESERVED_7_6_SHIFT 6
/* Byte Lane (upper nibble) internal VREF Select for Rank 1. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR8_X4DXREFISELR1_MASK 0x00003F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR8_X4DXREFISELR1_SHIFT 8
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR8_RESERVED_15_14_MASK 0x0000C000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR8_RESERVED_15_14_SHIFT 14
/* Byte Lane (upper nibble) internal VREF Select for Rank 2. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR8_X4DXREFISELR2_MASK 0x003F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR8_X4DXREFISELR2_SHIFT 16
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR8_RESERVED_23_22_MASK 0x00C00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR8_RESERVED_23_22_SHIFT 22
/* Byte Lane (upper nibble) internal VREF Select for Rank 3. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR8_X4DXREFISELR3_MASK 0x3F000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR8_X4DXREFISELR3_SHIFT 24
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR8_RESERVED_31_30_MASK 0xC0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR8_RESERVED_31_30_SHIFT 30

/**** DXNGCR9 register ****/
/* DRAM DQ (upper nibble) VREF Select for Rank0. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR9_X4DXDQVREFR0_MASK 0x0000003F
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR9_X4DXDQVREFR0_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR9_RESERVED_7_6_MASK 0x000000C0
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR9_RESERVED_7_6_SHIFT 6
/* DRAM DQ (upper nibble) VREF Select for Rank1. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR9_X4DXDQVREFR1_MASK 0x00003F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR9_X4DXDQVREFR1_SHIFT 8
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR9_RESERVED_15_14_MASK 0x0000C000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR9_RESERVED_15_14_SHIFT 14
/* DRAM DQ (upper nibble) VREF Select for Rank2. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR9_X4DXDQVREFR2_MASK 0x003F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR9_X4DXDQVREFR2_SHIFT 16
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR9_RESERVED_23_22_MASK 0x00C00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR9_RESERVED_23_22_SHIFT 22
/* DRAM DQ (upper nibble) VREF Select for Rank3. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR9_X4DXDQVREFR3_MASK 0x3F000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR9_X4DXDQVREFR3_SHIFT 24
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR9_RESERVED_31_30_MASK 0xC0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGCR9_RESERVED_31_30_SHIFT 30

/**** DXNBDLR0 register ****/
/* DQ0 Write Bit Delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR0_DQ0WBD_MASK 0x0000003F
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR0_DQ0WBD_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR0_RESERVED_7_6_MASK 0x000000C0
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR0_RESERVED_7_6_SHIFT 6
/* DQ1 Write Bit Delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR0_DQ1WBD_MASK 0x00003F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR0_DQ1WBD_SHIFT 8
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR0_RESERVED_15_14_MASK 0x0000C000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR0_RESERVED_15_14_SHIFT 14
/* DQ2 Write Bit Delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR0_DQ2WBD_MASK 0x003F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR0_DQ2WBD_SHIFT 16
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR0_RESERVED_23_22_MASK 0x00C00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR0_RESERVED_23_22_SHIFT 22
/* DQ3 Write Bit Delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR0_DQ3WBD_MASK 0x3F000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR0_DQ3WBD_SHIFT 24
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR0_RESERVED_31_30_MASK 0xC0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR0_RESERVED_31_30_SHIFT 30

/**** DXNBDLR1 register ****/
/* DQ4 Write Bit Delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR1_DQ4WBD_MASK 0x0000003F
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR1_DQ4WBD_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR1_RESERVED_7_6_MASK 0x000000C0
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR1_RESERVED_7_6_SHIFT 6
/* DQ5 Write Bit Delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR1_DQ5WBD_MASK 0x00003F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR1_DQ5WBD_SHIFT 8
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR1_RESERVED_15_14_MASK 0x0000C000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR1_RESERVED_15_14_SHIFT 14
/* DQ6 Write Bit Delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR1_DQ6WBD_MASK 0x003F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR1_DQ6WBD_SHIFT 16
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR1_RESERVED_23_22_MASK 0x00C00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR1_RESERVED_23_22_SHIFT 22
/* DQ7 Write Bit Delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR1_DQ7WBD_MASK 0x3F000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR1_DQ7WBD_SHIFT 24
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR1_RESERVED_31_30_MASK 0xC0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR1_RESERVED_31_30_SHIFT 30

/**** DXNBDLR2 register ****/
/* DM Write Bit Delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR2_DMWBD_MASK 0x0000003F
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR2_DMWBD_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR2_RESERVED_7_6_MASK 0x000000C0
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR2_RESERVED_7_6_SHIFT 6
/* DQS Write Bit Delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR2_DSWBD_MASK 0x00003F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR2_DSWBD_SHIFT 8
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR2_RESERVED_15_14_MASK 0x0000C000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR2_RESERVED_15_14_SHIFT 14
/* DQS/DM/DQ Output Enable Bit Delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR2_OEBD_MASK 0x003F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR2_OEBD_SHIFT 16
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR2_RESERVED_31_22_MASK 0xFFC00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR2_RESERVED_31_22_SHIFT 22

/**** DXNBDLR3 register ****/
/* DQ0 Read Bit Delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR3_DQ0RBD_MASK 0x0000003F
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR3_DQ0RBD_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR3_RESERVED_7_6_MASK 0x000000C0
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR3_RESERVED_7_6_SHIFT 6
/* DQ1 Read Bit Delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR3_DQ1RBD_MASK 0x00003F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR3_DQ1RBD_SHIFT 8
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR3_RESERVED_15_14_MASK 0x0000C000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR3_RESERVED_15_14_SHIFT 14
/* DQ2 Read Bit Delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR3_DQ2RBD_MASK 0x003F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR3_DQ2RBD_SHIFT 16
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR3_RESERVED_23_22_MASK 0x00C00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR3_RESERVED_23_22_SHIFT 22
/* DQ3 Read Bit Delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR3_DQ3RBD_MASK 0x3F000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR3_DQ3RBD_SHIFT 24
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR3_RESERVED_31_30_MASK 0xC0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR3_RESERVED_31_30_SHIFT 30

/**** DXNBDLR4 register ****/
/* DQ4 Read Bit Delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR4_DQ4RBD_MASK 0x0000003F
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR4_DQ4RBD_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR4_RESERVED_7_6_MASK 0x000000C0
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR4_RESERVED_7_6_SHIFT 6
/* DQ5 Read Bit Delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR4_DQ5RBD_MASK 0x00003F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR4_DQ5RBD_SHIFT 8
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR4_RESERVED_15_14_MASK 0x0000C000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR4_RESERVED_15_14_SHIFT 14
/* DQ6 Read Bit Delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR4_DQ6RBD_MASK 0x003F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR4_DQ6RBD_SHIFT 16
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR4_RESERVED_23_22_MASK 0x00C00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR4_RESERVED_23_22_SHIFT 22
/* DQ7 Read Bit Delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR4_DQ7RBD_MASK 0x3F000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR4_DQ7RBD_SHIFT 24
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR4_RESERVED_31_30_MASK 0xC0000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR4_RESERVED_31_30_SHIFT 30

/**** DXNBDLR5 register ****/
/* DM Read Bit Delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR5_DMRBD_MASK 0x0000003F
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR5_DMRBD_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR5_RESERVED_7_6_MASK 0x000000C0
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR5_RESERVED_7_6_SHIFT 6
/* DQS Read Bit Delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR5_DSRBD_MASK 0x00003F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR5_DSRBD_SHIFT 8
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR5_RESERVED_15_14_MASK 0x0000C000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR5_RESERVED_15_14_SHIFT 14
/* DQSN Read Bit Delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR5_DSNRBD_MASK 0x003F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR5_DSNRBD_SHIFT 16
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR5_RESERVED_31_22_MASK 0xFFC00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR5_RESERVED_31_22_SHIFT 22

/**** DXNBDLR6 register ****/
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR6_RESERVED_7_0_MASK 0x000000FF
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR6_RESERVED_7_0_SHIFT 0
/* Power down receiver Bit Delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR6_PDRBD_MASK 0x00003F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR6_PDRBD_SHIFT 8
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR6_RESERVED_15_14_MASK 0x0000C000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR6_RESERVED_15_14_SHIFT 14
/* Termination Enable Bit Delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR6_TERBD_MASK 0x003F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR6_TERBD_SHIFT 16
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR6_RESERVED_31_22_MASK 0xFFC00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR6_RESERVED_31_22_SHIFT 22

/**** DXNBDLR7 register ****/
/* DM Write Bit Delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR7_X4DMWBD_MASK 0x0000003F
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR7_X4DMWBD_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR7_RESERVED_7_6_MASK 0x000000C0
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR7_RESERVED_7_6_SHIFT 6
/* DQS Write Bit Delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR7_X4DSWBD_MASK 0x00003F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR7_X4DSWBD_SHIFT 8
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR7_RESERVED_15_14_MASK 0x0000C000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR7_RESERVED_15_14_SHIFT 14
/* DQS/DM/DQ Output Enable Bit Delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR7_X4OEBD_MASK 0x003F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR7_X4OEBD_SHIFT 16
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR7_RESERVED_31_22_MASK 0xFFC00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR7_RESERVED_31_22_SHIFT 22

/**** DXNBDLR8 register ****/
/* DM Read Bit Delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR8_X4DMRBD_MASK 0x0000003F
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR8_X4DMRBD_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR8_RESERVED_7_6_MASK 0x000000C0
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR8_RESERVED_7_6_SHIFT 6
/* DQS Read Bit Delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR8_X4DSRBD_MASK 0x00003F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR8_X4DSRBD_SHIFT 8
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR8_RESERVED_15_14_MASK 0x0000C000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR8_RESERVED_15_14_SHIFT 14
/* DQSN Read Bit Delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR8_X4DSNRBD_MASK 0x003F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR8_X4DSNRBD_SHIFT 16
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR8_RESERVED_31_22_MASK 0xFFC00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR8_RESERVED_31_22_SHIFT 22

/**** DXNBDLR9 register ****/
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR9_RESERVED_7_0_MASK 0x000000FF
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR9_RESERVED_7_0_SHIFT 0
/* Power down receiver Bit Delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR9_X4PDRBD_MASK 0x00003F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR9_X4PDRBD_SHIFT 8
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR9_RESERVED_15_14_MASK 0x0000C000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR9_RESERVED_15_14_SHIFT 14
/* Termination Enable Bit Delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR9_X4TERBD_MASK 0x003F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR9_X4TERBD_SHIFT 16
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR9_RESERVED_31_22_MASK 0xFFC00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR9_RESERVED_31_22_SHIFT 22

/**** DXNLCDLR0 register ****/
/* Write Leveling Delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR0_WLD_MASK 0x000001FF
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR0_WLD_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR0_RESERVED_15_9_MASK 0x0000FE00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR0_RESERVED_15_9_SHIFT 9
/* X4 Write Leveling Delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR0_X4WLD_MASK 0x01FF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR0_X4WLD_SHIFT 16
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR0_RESERVED_31_25_MASK 0xFE000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR0_RESERVED_31_25_SHIFT 25

/**** DXNLCDLR1 register ****/
/* Write Data Delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR1_WDQD_MASK 0x000001FF
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR1_WDQD_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR1_RESERVED_15_9_MASK 0x0000FE00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR1_RESERVED_15_9_SHIFT 9
/* X4 Write Data Delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR1_X4WDQD_MASK 0x01FF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR1_X4WDQD_SHIFT 16
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR1_RESERVED_31_25_MASK 0xFE000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR1_RESERVED_31_25_SHIFT 25

/**** DXNLCDLR2 register ****/
/* Read DQS Gating Delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR2_DQSGD_MASK 0x000001FF
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR2_DQSGD_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR2_RESERVED_15_9_MASK 0x0000FE00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR2_RESERVED_15_9_SHIFT 9
/* X4 Read DQS Gating Delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR2_X4DQSGD_MASK 0x01FF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR2_X4DQSGD_SHIFT 16
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR2_RESERVED_31_25_MASK 0xFE000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR2_RESERVED_31_25_SHIFT 25

/**** DXNLCDLR3 register ****/
/* Read DQS Delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR3_RDQSD_MASK 0x000001FF
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR3_RDQSD_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR3_RESERVED_15_9_MASK 0x0000FE00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR3_RESERVED_15_9_SHIFT 9
/* X4 Read DQS Delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR3_X4RDQSD_MASK 0x01FF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR3_X4RDQSD_SHIFT 16
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR3_RESERVED_31_25_MASK 0xFE000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR3_RESERVED_31_25_SHIFT 25

/**** DXNLCDLR4 register ****/
/* Read DQSN Delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR4_RDQSND_MASK 0x000001FF
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR4_RDQSND_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR4_RESERVED_15_9_MASK 0x0000FE00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR4_RESERVED_15_9_SHIFT 9
/* X4 Read DQSN Delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR4_X4RDQSND_MASK 0x01FF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR4_X4RDQSND_SHIFT 16
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR4_RESERVED_31_25_MASK 0xFE000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR4_RESERVED_31_25_SHIFT 25

/**** DXNLCDLR5 register ****/
/* DQS Gating Status Delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR5_DQSGSD_MASK 0x000001FF
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR5_DQSGSD_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR5_RESERVED_15_9_MASK 0x0000FE00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR5_RESERVED_15_9_SHIFT 9
/* X4 DQS Gating Status Delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR5_X4DQSGSD_MASK 0x01FF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR5_X4DQSGSD_SHIFT 16
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR5_RESERVED_31_25_MASK 0xFE000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR5_RESERVED_31_25_SHIFT 25

/**** DXNMDLR0 register ****/
/* Initial Period. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNMDLR0_IPRD_MASK 0x000001FF
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNMDLR0_IPRD_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNMDLR0_RESERVED_15_9_MASK 0x0000FE00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNMDLR0_RESERVED_15_9_SHIFT 9
/* Target Period. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNMDLR0_TPRD_MASK 0x01FF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNMDLR0_TPRD_SHIFT 16
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNMDLR0_RESERVED_31_25_MASK 0xFE000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNMDLR0_RESERVED_31_25_SHIFT 25

/**** DXNMDLR1 register ****/
/* MDL Delay. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNMDLR1_MDLD_MASK 0x000001FF
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNMDLR1_MDLD_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNMDLR1_RESERVED_31_9_MASK 0xFFFFFE00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNMDLR1_RESERVED_31_9_SHIFT 9

/**** DXNGTR0 register ****/
/* DQS Gating System Latency. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGTR0_DGSL_MASK 0x0000001F
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGTR0_DGSL_SHIFT 0
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGTR0_RESERVED_7_5_MASK 0x000000E0
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGTR0_RESERVED_7_5_SHIFT 5
/* X4 DQS Gating System Latency. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGTR0_X4DGSL_MASK 0x00001F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGTR0_X4DGSL_SHIFT 8
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGTR0_RESERVED_15_13_MASK 0x0000E000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGTR0_RESERVED_15_13_SHIFT 13
/* Write Leveling System Latency. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGTR0_WLSL_MASK 0x000F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGTR0_WLSL_SHIFT 16
/* X4 Write Leveling System Latency. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGTR0_X4WLSL_MASK 0x00F00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGTR0_X4WLSL_SHIFT 20
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGTR0_RESERVED_31_24_MASK 0xFF000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGTR0_RESERVED_31_24_SHIFT 24

/**** DXNRSR0 register ****/
/* DQS Gate Training Error. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNRSR0_QSGERR_MASK 0x0000FFFF
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNRSR0_QSGERR_SHIFT 0
/* X4 DQS Gate Training Error. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNRSR0_X4QSGERR_MASK 0xFFFF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNRSR0_X4QSGERR_SHIFT 16

/**** DXNRSR1 register ****/
/* Read Leveling Error. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNRSR1_RDLVLERR_MASK 0x0000FFFF
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNRSR1_RDLVLERR_SHIFT 0
/* X4 Read Leveling Error. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNRSR1_X4RDLVLERR_MASK 0xFFFF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNRSR1_X4RDLVLERR_SHIFT 16

/**** DXNRSR2 register ****/
/* Write Latency Adjustment (DQS off on some DQ lines) Warning. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNRSR2_WLAWN_MASK 0x0000FFFF
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNRSR2_WLAWN_SHIFT 0
/* X4 Write Latency Adjustment (DQS off on some DQ lines) Warning. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNRSR2_X4WLAWN_MASK 0xFFFF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNRSR2_X4WLAWN_SHIFT 16

/**** DXNRSR3 register ****/
/* Write Leveling Adjustment Error. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNRSR3_WLAERR_MASK 0x0000FFFF
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNRSR3_WLAERR_SHIFT 0
/* X4 Write Leveling Adjustment Error. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNRSR3_X4WLAERR_MASK 0xFFFF0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNRSR3_X4WLAERR_SHIFT 16

/**** DXNGSR0 register ****/
/* Write DQ Calibration. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR0_WDQCAL (1 << 0)
/* Read DQS Calibration. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR0_RDQSCAL (1 << 1)
/* Read DQS_N Calibration. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR0_RDQSNCAL (1 << 2)
/* Read DQS gating Calibration. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR0_GDQSCAL (1 << 3)
/* Write Leveling Calibration. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR0_WLCAL (1 << 4)
/* Write Leveling Done. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR0_WLDONE (1 << 5)
/* Write Leveling Error. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR0_WLERR (1 << 6)
/* Write Leveling Period. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR0_WLPRD_MASK 0x0000FF80
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR0_WLPRD_SHIFT 7
/* DATX8 PLL Lock. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR0_DPLOCK (1 << 16)
/* Read DQS gating Period. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR0_GDQSPRD_MASK 0x03FE0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR0_GDQSPRD_SHIFT 17
/* Reserved. Returns zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR0_RESERVED_29_26_MASK 0x3C000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR0_RESERVED_29_26_SHIFT 26
/* Write Leveling DQ Status. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR0_WLDQ (1 << 30)
/* Reserved. Returns zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR0_RESERVED_31 (1 << 31)

/**** DXNGSR1 register ****/
/* Delay Line Test Done. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR1_DLTDONE (1 << 0)
/* Delay Line Test Code. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR1_DLTCODE_MASK 0x01FFFFFE
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR1_DLTCODE_SHIFT 1
/* Reserved. Returns zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR1_RESERVED_31_25_MASK 0xFE000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR1_RESERVED_31_25_SHIFT 25

/**** DXNGSR2 register ****/
/* Read Bit Deskew Error. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR2_RDERR (1 << 0)
/* Read Bit Deskew Warning. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR2_RDWN (1 << 1)
/* Write Bit Deskew Error. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR2_WDERR (1 << 2)
/* Write Bit Deskew Warning. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR2_WDWN (1 << 3)
/* Read Eye Centering Error. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR2_REERR (1 << 4)
/* Read Eye Centering Warning. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR2_REWN (1 << 5)
/* Write Eye Centering Error. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR2_WEERR (1 << 6)
/* Write Eye Centering Warning. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR2_WEWN (1 << 7)
/* Error Status. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR2_ESTAT_MASK 0x00000F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR2_ESTAT_SHIFT 8
/* Reserved. Returns zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR2_RESERVED_19_12_MASK 0x000FF000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR2_RESERVED_19_12_SHIFT 12
/* Static Read Error. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR2_SRDERR (1 << 20)
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR2_RESERVED_21 (1 << 21)
/* Read DQS Gating Status Calibration. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR2_GSDQSCAL (1 << 22)
/* Read DQS gating Status Period. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR2_GSDQSPRD_MASK 0xFF800000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR2_GSDQSPRD_SHIFT 23

/**** DXNGSR3 register ****/
/* Static Read Delay Pass Count. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR3_SRDPC_MASK 0x00000003
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR3_SRDPC_SHIFT 0
/* X4 Static Read Delay Pass Count. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR3_X4SRDPC_MASK 0x0000000C
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR3_X4SRDPC_SHIFT 2
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR3_RESERVED_7_4_MASK 0x000000F0
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR3_RESERVED_7_4_SHIFT 4
/* Host VREF Training Error. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR3_HVERR_MASK 0x00000F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR3_HVERR_SHIFT 8
/* Host VREF Training Warning. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR3_HVWRN_MASK 0x0000F000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR3_HVWRN_SHIFT 12
/* DRAM VREF Training Error. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR3_DVERR_MASK 0x000F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR3_DVERR_SHIFT 16
/* DRAM VREF Training Warning. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR3_DVWRN_MASK 0x00F00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR3_DVWRN_SHIFT 20
/* VREF Training Error Status Code. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR3_ESTAT_MASK 0x07000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR3_ESTAT_SHIFT 24
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR3_RESERVED_31_27_MASK 0xF8000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR3_RESERVED_31_27_SHIFT 27

/**** DXNGSR4 register ****/
/* Write DQ Calibration. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR4_X4WDQCAL (1 << 0)
/* Read DQS Calibration. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR4_X4RDQSCAL (1 << 1)
/* Read DQSb Calibration. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR4_X4RDQSNCAL (1 << 2)
/* Read DQS gating Calibration. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR4_X4GDQSCAL (1 << 3)
/* Write Leveling Calibration. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR4_X4WLCAL (1 << 4)
/* Write Leveling Done. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR4_X4WLDONE (1 << 5)
/* Write Leveling Error. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR4_X4WLERR (1 << 6)
/* Write Leveling Period. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR4_X4WLPRD_MASK 0x0000FF80
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR4_X4WLPRD_SHIFT 7
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR4_RESERVED_16 (1 << 16)
/* Read DQS gating Period. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR4_X4GDQSPRD_MASK 0x03FE0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR4_X4GDQSPRD_SHIFT 17
/* Reserved. Returns zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR4_RESERVED_31_26_MASK 0xFC000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR4_RESERVED_31_26_SHIFT 26

/**** DXNGSR5 register ****/
/* Read Bit Deskew Error. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR5_X4RDERR (1 << 0)
/* Read Bit Deskew Warning. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR5_X4RDWN (1 << 1)
/* Write Bit Deskew Error. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR5_X4WDERR (1 << 2)
/* Write Bit Deskew Warning. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR5_X4WDWN (1 << 3)
/* Read Eye Centering Error. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR5_X4REERR (1 << 4)
/* Read Eye Centering Warning. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR5_X4REWN (1 << 5)
/* Write Eye Centering Error. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR5_X4WEERR (1 << 6)
/* Write Eye Centering Warning. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR5_X4WEWN (1 << 7)
/* Error Status. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR5_X4ESTAT_MASK 0x00000F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR5_X4ESTAT_SHIFT 8
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR5_RESERVED_19_12_MASK 0x000FF000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR5_RESERVED_19_12_SHIFT 12
/* Static Read Error. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR5_X4SRDERR (1 << 20)
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR5_RESERVED_21 (1 << 21)
/* Read DQS Gating Status Calibration. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR5_X4GSDQSCAL (1 << 22)
/* Read DQS gating Status Period. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR5_X4GSDQSPRD_MASK 0xFF800000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR5_X4GSDQSPRD_SHIFT 23

/**** DXNGSR6 register ****/
/* Reserved. Return zeroes on reads. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR6_RESERVED_1_0_MASK 0x00000003
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR6_RESERVED_1_0_SHIFT 0
/* X4 Static Read Delay Pass Count. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR6_X4SRDPC_MASK 0x0000000C
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR6_X4SRDPC_SHIFT 2
/* X4 Read Leveling Error. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR6_X4RDLVLERR_MASK 0x000000F0
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR6_X4RDLVLERR_SHIFT 4
/* X4 Host VREF Training Error. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR6_X4HVERR_MASK 0x00000F00
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR6_X4HVERR_SHIFT 8
/* X4 Host VREF Training Warning. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR6_X4HVWRN_MASK 0x0000F000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR6_X4HVWRN_SHIFT 12
/* X4 DRAM VREF Training Error. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR6_X4DVERR_MASK 0x000F0000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR6_X4DVERR_SHIFT 16
/* X4 DRAM VREF Training Warning. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR6_X4DVWRN_MASK 0x00F00000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR6_X4DVWRN_SHIFT 20
/* Capture DB DQ[7:0], bits[31:28] is for DQ[7:4] for upper nibble, bits[27:24] is for DQ[3:0] for
 * lower nibble. */
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR6_DBDQ_MASK 0xFF000000
#define ALPINE_V3_DWC_DDR_PHY_REGS_DXNGSR6_DBDQ_SHIFT 24


#ifdef __cplusplus
}
#endif

#endif

/** @} end of DDR group */


