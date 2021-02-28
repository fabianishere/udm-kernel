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
 * @brief  DDR PHY registers
 *
 */
#ifndef __AL_HAL_DDR_PHY_REGS_REGS_ALPINE_V1_H__
#define __AL_HAL_DDR_PHY_REGS_REGS_ALPINE_V1_H__

#include "al_hal_ddr_cfg.h"
#include "al_hal_plat_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* The index of the ECC byte lane */
#define AL_DDR_PHY_ECC_BYTE_LANE_INDEX_ALPINE_V1		4

/* The clock frequency on which the PLL frequency select need to be changed */
#define AL_DDR_PHY_PLL_FREQ_SEL_MHZ_ALPINE_V1		700

struct al_ddr_phy_zq_regs_alpine_v1 {
	uint32_t PR;		/* ZQ Impedance Control Program Register */
	uint32_t DR;		/* ZQ Impedance Control Data Register */
	uint32_t SR;		/* ZQ Impedance Control Status Register */
	uint32_t reserved;
};

struct al_ddr_phy_datx8_regs_alpine_v1 {
	uint32_t GCR[4];	/* General Configuration Registers 0-3 */
	uint32_t GSR[3];	/* General Status Registers 0-2 */
	uint32_t BDLR[7];	/* Bit Delay Line Registers 0-6 */
	uint32_t LCDLR[3];	/* Local Calibrated Delay Line Registers 0-2 */
	uint32_t MDLR;		/* Master Delay Line Register */
	uint32_t GTR;		/* General Timing Register */
	uint32_t reserved[13];
};

struct al_dwc_ddrphy_top_dwc_ddrphy_pub_alpine_v1 {
	uint32_t RIDR;		/* Revision Identification Reg */
	uint32_t PIR;		/* PHY Initialization Reg */
	uint32_t PGCR[4];	/* PHY General Configuration Regs 0-3 */
	uint32_t PGSR[2];	/* PHY General Status Regs 0-1 */
	uint32_t PLLCR;		/* PLL Control Reg */
	uint32_t PTR[5];	/* PHY Timing Regs 0-4 */
	uint32_t ACMDLR;	/* AC Master Delay Line Reg */
	uint32_t ACLCDLR;	/* AC Local Calibrated Delay Line Reg */
	uint32_t ACBDLR[10];	/* AC Bit Delay Line Regs 0-9 */
	uint32_t ACIOCR[6];	/* AC I/O Configuration Regs 0-5 */
	uint32_t DXCCR;		/* DATX8 Common Configuration Reg */
	uint32_t DSGCR;		/* DDR System General Configuration Reg */
	uint32_t DCR;		/* DRAM Configuration Reg */
	uint32_t DTPR[4];	/* DRAM Timing Parameters Registesr 0-3 */
	uint32_t mr[4];		/* Mode Regs 0-3 */
	uint32_t ODTCR;		/* ODT Configuration Reg */
	uint32_t DTCR;		/* Data Training Configuration Reg */
	uint32_t DTAR[4];	/* Data Training Address Register 0-3 */
	uint32_t DTDR[2];	/* Data Training Data Register 0-1 */
	uint32_t DTEDR[2];	/* Data Training Eye Data Register 0-1 */
	uint32_t RDIMMGCR[2];	/* RDIMM General Configuration Register 0-1 */
	uint32_t RDIMMCR[2];	/* RDIMM Control Register 0-1 */
	uint32_t reserved1[0x3D - 0x39];
	uint32_t ODTCTLR;	/* ODT Control Reg */
	uint32_t reserved2[0x70 - 0x3E];
	uint32_t BISTRR;	/* BIST Run Register */
	uint32_t BISTWCR;	/* BIST Word Count Register */
	uint32_t BISTMSKR[3];	/* BIST Mask Register 0-2 */
	uint32_t BISTLSR;	/* BIST LFSR Seed Register */
	uint32_t BISTAR[3];	/* BIST Address Register 0-2 */
	uint32_t BISTUDPR;	/* BIST User Data Pattern Register */
	uint32_t BISTGSR;	/* BIST General Status Register */
	uint32_t BISTWER;	/* BIST Word Error Register */
	uint32_t BISTBER[4];	/* BIST Bit Error Register 0-3 */
	uint32_t BISTWCSR;	/* BIST Word Count Status Register */
	uint32_t BISTFWR[3];	/* BIST Fail Word Register 0-2 */
	uint32_t reserved3[0x8E - 0x84];
	uint32_t IOVCR[2];	/* IO VREF Control Register 0-1 */
	uint32_t ZQCR;		/* ZQ Impedance Control Register */
	struct al_ddr_phy_zq_regs_alpine_v1 ZQ[AL_DDR_PHY_NUM_ZQ_SEGMENTS_ALPINE_V1];
	uint32_t reserved4[0xA0 - 0x9D];
	struct al_ddr_phy_datx8_regs_alpine_v1 datx8[AL_DDR_PHY_NUM_BYTE_LANES];
};


struct al_dwc_ddrphy_top_regs_alpine_v1 {
	struct al_dwc_ddrphy_top_dwc_ddrphy_pub_alpine_v1 dwc_ddrphy_pub;
};

/* Register PIR */
/*
 * Initialization Trigger,
 * A write of 1 to this bit triggers the DDR system initialization,
 * including PHY initialization, DRAM initialization, and PHY training.
 * The exact initialization steps to be executed are specified
 * in bits 1 to 15 of this register. A bit setting of 1 means the step will
 * be executed as part of the initialization sequence, while a setting
 * of 0 means the step will be bypassed. The initialization trigger
 * bit is self-clearing.
 */
#define DWC_DDR_PHY_REGS_PIR_INIT			0x01

/*
 * PHY Reset
 * Resets the AC and DATX8 modules by asserting the AC/DATX8 reset pin.
 */
#define DWC_DDR_PHY_REGS_PIR_PHYRST			0x40

/*
 * Write Leveling (DDR3 Only)
 * Executes a PUBm2 write leveling routine.
 */
#define DWC_DDR_PHY_REGS_PIR_WL				AL_BIT(9)

/*
 * Read DQS Gate Training
 * Executes a PUBm2 training routine to determine the optimum position of the read data DQS strobe
 * for maximum system timing margins
 */
#define DWC_DDR_PHY_REGS_PIR_QSGATE			AL_BIT(10)

/*
 * Write Leveling Adjust (DDR3 Only)
 * Executes a PUBm2 training routine that readjusts the write latency used during write in case the
 * write leveling routine changed the expected latency.
 */
#define DWC_DDR_PHY_REGS_PIR_WLADJ			AL_BIT(11)

/*
 * Read Data Bit Deskew
 * Executes a PUBm2 training routine to deskew the DQ bits during read
 */
#define DWC_DDR_PHY_REGS_PIR_RDDSKW			AL_BIT(12)

/*
 * Write Data Bit Deskew
 * Executes a PUBm2 training routine to deskew the DQ bits during write
 */
#define DWC_DDR_PHY_REGS_PIR_WRDSKW			AL_BIT(13)

/*
 * Read Data Eye Training
 * Executes a PUBm2 training routine to maximize the read data eye.
 */
#define DWC_DDR_PHY_REGS_PIR_RDEYE			AL_BIT(14)

/*
 * Write Data Eye Training
 * Executes a PUBm2 training routine to maximize the write data eye.
 */
#define DWC_DDR_PHY_REGS_PIR_WREYE			AL_BIT(15)

/**
 * Digital Delay Line (DDL) Calibration Bypass: Bypasses or stops, if set, DDL
 * calibration that automatically triggers after reset. DDL calibration may be
 * triggered manually using INIT and DCAL bits of the PIR register.
 * This bit is self clearing.
 */
#define DWC_DDR_PHY_REGS_PIR_DCALBYP			0x20000000

/* Register PGSR0 field iDONE */
/**
 * Initialization Done: Indicates if set that the DDR system initialization has
 * completed. This bit is set after all the selected initialization routines in
 * PIR register have completed.
 */
#define DWC_DDR_PHY_REGS_PGSR0_IDONE			0x00000001

/* Register PGSR0 field ZCERR */
/**
 * Impedance Calibration Error: Indicates if set that there is an error in
 * impedance calibration.
 */
#define DWC_DDR_PHY_REGS_PGSR0_ZCERR			0x00100000

/* Register PGSR0 field WLERR */
/**
 * Write Leveling Error: Indicates if set that there is an error in write
 * leveling.
 */
#define DWC_DDR_PHY_REGS_PGSR0_WLERR			0x00200000

/* Register PGSR0 field QSGERR */
/**
 * DQS Gate Training Error: Indicates if set that there is an error in DQS gate
 * training.
 */
#define DWC_DDR_PHY_REGS_PGSR0_QSGERR			0x00400000

/* Register PGSR0 field WLAERR */
/**
 * Write Leveling Adjustment Error: Indicates if set that there is an error in
 * write leveling adjustment.
 */
#define DWC_DDR_PHY_REGS_PGSR0_WLAERR			0x00800000

/* Register PGSR0 field RDERR */
/**
 * Read Bit Deskew Error: Indicates if set that there is an error in read bit
 * deskew.
 */
#define DWC_DDR_PHY_REGS_PGSR0_RDERR			0x01000000

/* Register PGSR0 field WDERR */
/**
 * Write Bit Deskew Error: Indicates if set that there is an error in write bit
 * deskew.
 */
#define DWC_DDR_PHY_REGS_PGSR0_WDERR			0x02000000

/* Register PGSR0 field REERR */
/**
 * Read Eye Training Error: Indicates if set that there is an error in read eye
 * training.
 */
#define DWC_DDR_PHY_REGS_PGSR0_REERR			0x04000000

/* Register PGSR0 field WEERR */
/**
 * Write Eye Training Error: Indicates if set that there is an error in write
 * eye training.
 */
#define DWC_DDR_PHY_REGS_PGSR0_WEERR			0x08000000

/* Register PGSR0 field VTDONE */
/**
 * AC VT Done: Indicates if set that VT compensation calculation has
 * been completed for all enabled AC BDLs and LCDL.
 */
#define DWC_DDR_PHY_REGS_PGSR0_VTDONE			0x40000000

/* Register PGSR1 field VTSTOP */
/**
 * VT Stop: Indicates if set that the VT calculation logic has stopped
 * computing the next values for the VT compensated delay line values. After
 * assertion of the PGCR.INHVT, the VTSTOP bit should be read to ensure all VT
 * compensation logic has stopped computations before writing to the delay line
 * registers.
 */
#define DWC_DDR_PHY_REGS_PGSR1_VTSTOP			0x40000000

/* Register PGCR0 field PHYFRST */
/**
 * A write of ‘0’ to this bit resets the AC and DATX8 FIFOs without
 * resetting PUB RTL logic. This bit is not self-clearing and a ‘1’
 * must be written to deassert the reset.
 */
#define DWC_DDR_PHY_REGS_PGCR0_PHYFRST			0x04000000

/* Register PGCR1 field DLBYPMODE */
/**
 * Controls DDL Bypass Modes. Valid values are:
 * 00 = Normal dynamic control
 * 01 = All DDLs bypassed
 * 10 = No DDLs bypassed
 * 11 = Reserved
 */
#define DWC_DDR_PHY_REGS_PGCR1_DLBYPMODE_MASK		0x00000030
#define DWC_DDR_PHY_REGS_PGCR1_DLBYPMODE_SHIFT		4

#define DWC_DDR_PHY_REGS_PGCR1_DLBYPMODE_DYNAMIC	\
	(0 << DWC_DDR_PHY_REGS_PGCR1_DLBYPMODE_SHIFT)

#define DWC_DDR_PHY_REGS_PGCR1_DLBYPMODE_BYPASS	\
	(1 << DWC_DDR_PHY_REGS_PGCR1_DLBYPMODE_SHIFT)

#define DWC_DDR_PHY_REGS_PGCR1_DLBYPMODE_NO_BYPASS	\
	(2 << DWC_DDR_PHY_REGS_PGCR1_DLBYPMODE_SHIFT)

/* Register PGCR1 field IODDRM */
/**
 * I/O DDR Mode (D3F I/O Only): Selects the DDR mode for the I/Os.
 * These bits connect to bits [2:1] of the IOM pin of the SSTL I/O.
 * I/O Mode: I/O Mode select
 * 000 = DDR2 mode
 * 001 = LVCMOS mode
 * 010 = DDR3 mode
 * 011 = Reserved
 * 100 = DDR3L mode
 * 101 = Reserved
 * 110 = Reserved
 * 111 = Reserved
 */
#define DWC_DDR_PHY_REGS_PGCR1_IODDRM_MASK		AL_FIELD_MASK(8, 7)
#define DWC_DDR_PHY_REGS_PGCR1_IODDRM_SHIFT		7
#define DWC_DDR_PHY_REGS_PGCR1_IODDRM_VAL_DDR3		\
	(1 << DWC_DDR_PHY_REGS_PGCR1_IODDRM_SHIFT)

/* Register PGCR1 field PHYHRST */
/**
 * PHY High-Speed Reset: A write of ‘0’ to this bit resets the AC and DATX8
 * macros without resetting PUBm2 RTL logic. This bit is not self-clearing
 * and a ‘1’ must be written to de-assert the reset.
 */
#define DWC_DDR_PHY_REGS_PGCR1_PHYHRST			0x02000000

/* Register PGCR1 field INHVT */
/**
 * VT Calculation Inhibit: Inhibits calculation of the next VT compensated
 * delay line values. A value of 1 will initiate a stop of the VT compensation
 * logic. The bit PGSR1[30] (VSTOP) will be set to a logic 1 when VT
 * compensation has stopped. This bit should be set to 1 during writes to the
 * delay line registers. A value of 0 will re-enable the VT compensation
 * logic.
 */
#define DWC_DDR_PHY_REGS_PGCR1_INHVT			0x04000000

/* Register PGCR1 field IOLB */
/**
 * I/O Loop-Back Select: Selects where inside the I/O the loop-back of signals
 * happens. Valid values are:
 * 0 = Loopback is after output buffer; output enable must be asserted
 * 1 = Loopback is before output buffer; output enable is don’t care
 */
#define DWC_DDR_PHY_REGS_PGCR1_IOLB			0x08000000

/* Write Leveling Step: Specifies the number of delay step-size  ... */
#define DWC_DDR_PHY_REGS_PGCR1_WLSTEP (1 << 2)

/* Register PGCR2 field T_REFPRD */
/**
 * Refresh Period: Indicates the period in clock cycles after which the PUBm2 has to
 * issue a refresh command to the SDRAM. This is derived from the maximum refresh
 * interval from the datasheet, tRFC (max) or REFI, divided by the clock cycle time. A
 * further 400 clocks must be subtracted from the derived number to account for
 * command flow and missed slots of refreshes in the internal PUBm2 blocks. The
 * default corresponds to DDR3 9*7.8us at 1066MHz when a burst of 9 refreshes are
 * issued at every refresh interval.
 */
#define DWC_DDR_PHY_REGS_PGCR2_T_REFPRD_MASK		0x0003ffff
#define DWC_DDR_PHY_REGS_PGCR2_T_REFPRD_SHIFT		0

/* Register PGCR3 field RDMODE */
/**
 */
#define DWC_DDR_PHY_REGS_PGCR3_RDMODE_MASK		0x00000018
#define DWC_DDR_PHY_REGS_PGCR3_RDMODE_SHIFT		3

#define DWC_DDR_PHY_REGS_PGCR3_RDMODE_STATIC_RD_RSP_OFF	\
	(0 << DWC_DDR_PHY_REGS_PGCR3_RDMODE_SHIFT)

#define DWC_DDR_PHY_REGS_PGCR3_RDMODE_STATIC_RD_RSP_CMP	\
	(2 << DWC_DDR_PHY_REGS_PGCR3_RDMODE_SHIFT)

#define DWC_DDR_PHY_REGS_PGCR3_RDMODE_STATIC_RD_RSP_PRG	\
	(3 << DWC_DDR_PHY_REGS_PGCR3_RDMODE_SHIFT)

/* Register PGCR3 field RDDLY */
/**
 */
#define DWC_DDR_PHY_REGS_PGCR3_RDDLY_MASK		0x000001e0
#define DWC_DDR_PHY_REGS_PGCR3_RDDLY_SHIFT		5

/* Register PGCR3 field GATEDXCTLCLK */
/**
Enable Clock Gating for DX ctl_clk: Enables, when set, clock gating for power
saving. Valid values are:
0 = Clock gating is disabled .
1 = Clock gating is enabled
 */
#define DWC_DDR_PHY_REGS_PGCR3_GATEDXCTLCLK			0x00002000

/* Register PGCR3 field GATEDXDDRCLK */
/**
Enable Clock Gating for DX ddr_clk: Enables, when set, clock gating for power
saving. Valid values are:
0 = Clock gating is disabled .
1 = Clock gating is enabled
 */
#define DWC_DDR_PHY_REGS_PGCR3_GATEDXDDRCLK			0x00004000

/* Register PGCR3 field GATEDXRDCLK */
/**
Enable Clock Gating for DX rd_clk: Enables, when set, clock gating for power
saving. Valid values are:
0 = Clock gating is disabled .
1 = Clock gating is enabled
 */
#define DWC_DDR_PHY_REGS_PGCR3_GATEDXRDCLK			0x00008000

/* Register PLLCR field FRQSEL */
/**
 * PLL Frequency Select: Selects the operating range of the PLL.
 * 00 = PLL reference clock (ctl_clk/REF_CLK) ranges from 335MHz to 533MHz
 * 01 = PLL reference clock (ctl_clk/REF_CLK) ranges from 225MHz to 385MHz
 * 10 = Reserved
 * 11 = PLL reference clock (ctl_clk/REF_CLK) ranges from 166MHz to 275MHz
 */
#define DWC_DDR_PHY_REGS_PLLCR_FRQSEL_MASK			0x00180000
#define DWC_DDR_PHY_REGS_PLLCR_FRQSEL_SHIFT			19
#define DWC_DDR_PHY_REGS_PLLCR_FRQSEL_335MHZ_533MHz		\
	(0x0 << DWC_DDR_PHY_REGS_PLLCR_FRQSEL_SHIFT)
#define DWC_DDR_PHY_REGS_PLLCR_FRQSEL_225MHZ_385MHz		\
	(0x1 << DWC_DDR_PHY_REGS_PLLCR_FRQSEL_SHIFT)
#define DWC_DDR_PHY_REGS_PLLCR_FRQSEL_166MHZ_275MHz		\
	(0x3 << DWC_DDR_PHY_REGS_PLLCR_FRQSEL_SHIFT)

/* Register ACIOCR0 field ACPDR */
/**
 * AC Power Down Receiver: Powers down, when set, the input receiver on the I/O for
 * RAS#, CAS#, WE#, BA[2:0], and A[15:0] pins.
 */
#define DWC_DDR_PHY_REGS_ACIOCR0_ACPDR			0x00000010

/* Register ACIOCR0 field CKPDR */
/**
 * CK Power Down Receiver: Powers down, when set, the input receiver on the I/O for
 * CK[0], CK[1], CK[2], and CK[3] pins, respectively.
 */
#define DWC_DDR_PHY_REGS_ACIOCR0_CKPDR_MASK			0x00003c00
#define DWC_DDR_PHY_REGS_ACIOCR0_CKPDR_SHIFT			10

/* Register ACIOCR0 field RANKPDR */
/**
 * Rank Power Down Receiver: Powers down, when set, the input receiver on the I/O
 * CKE[3:0], ODT[3:0], and CS#[3:0] pins. RANKPDR[0] controls the power down for
 * CKE[0], ODT[0], and CS#[0], RANKPDR[1] controls the power down for CKE[1],
 * ODT[1], and CS#[1], and so on.
 */
#define DWC_DDR_PHY_REGS_ACIOCR0_RANKPDR_MASK			0x03c00000
#define DWC_DDR_PHY_REGS_ACIOCR0_RANKPDR_SHIFT			22

/* Register ACIOCR0 field RSTPDR */
/**
 * SDRAM Reset Power Down Receiver: Powers down, when set, the input receiver
 * on the I/O for SDRAM RST# pin.
 */
#define DWC_DDR_PHY_REGS_ACIOCR0_RSTPDR			0x10000000

/* Register DXCCR field DXODT */
/**
 * Data On-Die Termination: Enables, when set, the on-die termination on the I/O for DQ, DM, and
 * DQS/DQS# pins of all DATX8 macros. This bit is ORed with the ODT configuration bit of the
 * individual DATX8
 */
#define DWC_DDR_PHY_REGS_DXCCR_DXODT_EN			0x00000001

/* Register DXCCR field DQSRES */
/**
 * DQS Resistor: Selects the on-die pull-down/pull-up resistor for DQS pins.
 * DQSRES[3] selects pull-down (when set to 0) or pull-up (when set to 1). DQSRES[2:0] selects the
 * resistor value as follows:
 * 000 = Open: On-die resistor disconnected
 * 001 = 688 ohms
 * 010 = 611 ohms
 * 011 = 550 ohms
 * 100 = 500 ohms
 * 101 = 458 ohms
 * 110 = 393 ohms
 * 111 = 344 ohms
 */
#define DWC_DDR_PHY_REGS_DXCCR_DQSRES_MASK			0x000001e0
#define DWC_DDR_PHY_REGS_DXCCR_DQSRES_SHIFT			5
#define DWC_DDR_PHY_REGS_DXCCR_DQSRES_VAL_PULL_DOWN_500OHM	\
	(0x4 << DWC_DDR_PHY_REGS_DXCCR_DQSRES_SHIFT)
#define DWC_DDR_PHY_REGS_DXCCR_DQSRES_VAL_PULL_UP_611OHM	\
	(0xA << DWC_DDR_PHY_REGS_DXCCR_DQSRES_SHIFT)

/* Register DXCCR field DQSNRES */
/**
 * DQS# Resistor: Selects the on-die pull-down/pull-up resistor for DQS# pins.
 * DQSNRES[3] selects pull-down (when set to 0) or pull-up (when set to 1). DQSNRES[2:0] selects the
 * resistor value as follows:
 * 000 = Open: On-die resistor disconnected
 * 001 = 688 ohms
 * 010 = 611 ohms
 * 011 = 550 ohms
 * 100 = 500 ohms
 * 101 = 458 ohms
 * 110 = 393 ohms
 * 111 = 344 ohms
 */
#define DWC_DDR_PHY_REGS_DXCCR_DQSNRES_MASK			0x00001e00
#define DWC_DDR_PHY_REGS_DXCCR_DQSNRES_SHIFT			9
#define DWC_DDR_PHY_REGS_DXCCR_DQSNRES_VAL_PULL_UP_500OHM	\
	(0xC << DWC_DDR_PHY_REGS_DXCCR_DQSNRES_SHIFT)
#define DWC_DDR_PHY_REGS_DXCCR_DQSNRES_VAL_PULL_UP_458OHM	\
	(0xD << DWC_DDR_PHY_REGS_DXCCR_DQSNRES_SHIFT)

/* Register DSGCR field PUREN */
/**
 * PHY Update Request Enable: Specifies if set, that the PHY should issue
 * PHY-initiated update request when there is DDL VT drift.
 */
#define DWC_DDR_PHY_REGS_DSGCR_PUREN			0x00000001


/* Register DSGCR field DQSGX */
/**
 * DQSGX DQS Gate Extension: Specifies if set that the read DQS gating mustgate
 * will be extended. This should be set ONLY when used with DQS
 * pulldown and then re-centered, i.e. one clock cycle extension on either
 * side.DQSn pullup. Allowable settings are:
 * 00 = do not extend the gate
 * 01 = extend the gate by ½ tCK in both directions (but never earlier than
 * zero read latency)
 * 10 = extend the gate earlier by ½ tCK and later by 2 * tCK (to facilitate
 * LPDDR2/LPDDR3 usage without training for systems supporting upto
 * 800Mbps)
 * 11 = extend the gate earlier by ½ tCK and later by 3 * tCK (to facilitate
 * LPDDR2/LPDDR3 usage without training for systems supporting upto
 * 1600Mbps))
 */
#define DWC_DDR_PHY_REGS_DSGCR_DQSGX_MASK		0x000000c0
#define DWC_DDR_PHY_REGS_DSGCR_DQSGX_SHIFT		6

/* Register DSGCR field RRMODE */
/**
 * Rise-to-Rise Mode: Indicates if set that the PHY mission mode is configured
 * to run in rise-to-rise mode. Otherwise if not set the PHY mission mode is
 * running in rise-to-fall mode.
 */
#define DWC_DDR_PHY_REGS_DSGCR_RRMODE			0x00040000

/* Register DCR field NOSRA */
/**
 * No Simultaneous Rank Access: Specifies if set that simultaneous rank access
 * on the same clock cycle is not allowed. This means that multiple chip select
 * signals should not be asserted at the same time. This may be required on
 * some DIMM systems.
 */
#define DWC_DDR_PHY_REGS_DCR_NOSRA			0x08000000

/* Register DCR field DDR2T */
/**
 * DDR 2T Timing: Indicates if set that 2T timing should be used by PUBm2
 * internally generated SDRAM transactions.
 */
#define DWC_DDR_PHY_REGS_DCR_DDR2T			0x10000000

/* Register DCR field UDIMM */
/**
 * Un-buffered DIMM Address Mirroring: Indicates if set that there is address
 * mirroring on the second rank of an un-buffered DIMM (the rank connected to
 * CS#[1]). In this case, the PUBm2 re-scrambles the bank and address when
 * sending mode register commands to the second rank. This only applies to
 * PUBm2 internal SDRAM transactions. Transactions generated by the controller
 * must make its own adjustments when using an un-buffered DIMM. DCR[NOSRA]
 * must be set if address mirroring is enabled.
 */
#define DWC_DDR_PHY_REGS_DCR_UDIMM			0x20000000

/* Register DTPR0 field t_rtp */
/* Internal read to precharge command delay. Valid values are 2 to 15 */
#define DWC_DDR_PHY_REGS_DTPR0_T_RTP_MASK		0x0000000f
#define DWC_DDR_PHY_REGS_DTPR0_T_RTP_SHIFT		0

/* Register DTPR0 field t_wtr */
/* Internal write to read command delay. Valid values are 1 to 15 */
#define DWC_DDR_PHY_REGS_DTPR0_T_WTR_MASK		0x000000f0
#define DWC_DDR_PHY_REGS_DTPR0_T_WTR_SHIFT		4

/* Register DTPR0 field t_rp */
/* Precharge command period: The minimum time between a precharge command
and any other command. Note that the Controller automatically derives tRPA for
8-bank DDR2 devices by adding 1 to tRP. Valid values are 2 to 15.
In LPDDR3 mode, PUBm2 adds an offset of 8 to the register value, so valid range
is 8 to 2 */
#define DWC_DDR_PHY_REGS_DTPR0_T_RP_MASK		0x00000f00
#define DWC_DDR_PHY_REGS_DTPR0_T_RP_SHIFT		8

/* Register DTPR0 field t_rcd */
/* Activate to read or write delay. Minimum time from when an activate command
 * is issued to when a read or write to the activated row can be issued. Valid
 * values are 2 to 15. In LPDDR3 mode, PUBm2 adds an offset of 8 to the register
 * value, so valid range is 8 to 23.
 */
#define DWC_DDR_PHY_REGS_DTPR0_T_RCD_MASK		0x0000f000
#define DWC_DDR_PHY_REGS_DTPR0_T_RCD_SHIFT		12

/* Register DTPR0 field t_ras_min */
/* Activate to precharge command delay. Valid values are 2 to 63 */
#define DWC_DDR_PHY_REGS_DTPR0_T_RAS_MIN_MASK		0x003f0000
#define DWC_DDR_PHY_REGS_DTPR0_T_RAS_MIN_SHIFT		16

/* Register DTPR0 field t_rrd */
/* Activate to activate command delay (different banks). Valid values are 1 to
 * 15
 */
#define DWC_DDR_PHY_REGS_DTPR0_T_RRD_MASK		0x03c00000
#define DWC_DDR_PHY_REGS_DTPR0_T_RRD_SHIFT		22

/* Register DTPR0 field t_rc */
/* Activate to activate command delay (same bank). Valid values are 2 to 63 */
#define DWC_DDR_PHY_REGS_DTPR0_T_RC_MASK		0xfc000000
#define DWC_DDR_PHY_REGS_DTPR0_T_RC_SHIFT		26

/* Register DTPR1 field T_AOND */
/* Read ODT turn-on delay */
#define DWC_DDR_PHY_REGS_DTPR1_T_AOND_MASK		0xc0000000
#define DWC_DDR_PHY_REGS_DTPR1_T_AOND_SHIFT		30

/**** DTPR2 register ****/
/* Read to ODT delay (DDR3 only) */
#define DWC_DDR_PHY_REGS_DTPR2_TRTODT (1 << 29)

/* Read to Write command delay */
#define DWC_DDR_PHY_REGS_DTPR2_TRTW (1 << 30)

/* Register DTPR3 field T_OFDX */
/* ODT turn-on length (read and write) */
#define DWC_DDR_PHY_REGS_DTPR3_T_OFDX_MASK		0xe0000000
#define DWC_DDR_PHY_REGS_DTPR3_T_OFDX_SHIFT		29

/* Register ODTCR field RDODT0 */
/**
 * Read ODT: Specifies whether ODT should be enabled (‘1’) or disabled (‘0’) on
 * each of the up to four ranks when a read command is sent to rank n. RDODT0,
 * RDODT1, RDODT2, and RDODT3 specify ODT settings when a read is to rank 0,
 * rank 1, rank 2, and rank 3, respectively. The four bits of each field each
 * represent a rank, the LSB being rank 0 and the MSB being rank 3.
 * Default is to disable ODT during reads.
*/
#define DWC_DDR_PHY_REGS_ODTCR_RDODT0_MASK		0x0000000F
#define DWC_DDR_PHY_REGS_ODTCR_RDODT0_SHIFT		0

#define DWC_DDR_PHY_REGS_ODTCR_RDODT1_MASK		0x000000F0
#define DWC_DDR_PHY_REGS_ODTCR_RDODT1_SHIFT		4

#define DWC_DDR_PHY_REGS_ODTCR_RDODT2_MASK		0x00000F00
#define DWC_DDR_PHY_REGS_ODTCR_RDODT2_SHIFT		8

#define DWC_DDR_PHY_REGS_ODTCR_RDODT3_MASK		0x0000F000
#define DWC_DDR_PHY_REGS_ODTCR_RDODT3_SHIFT		12

/* Register ODTCR field WRODT0 */
/**
 * Write ODT: Specifies whether ODT should be enabled (‘1’) or disabled (‘0’) on
 * each of the up to four ranks when a write command is sent to rank n. WRODT0,
 * WRODT1, WRODT2, and WRODT3 specify ODT settings when a write is to rank 0,
 * rank 1, rank 2, and rank 3, respectively. The four bits of each field each
 * represent a rank, the LSB being rank 0 and the MSB being rank 3.
 * Default is to enable ODT only on rank being written to.
*/
#define DWC_DDR_PHY_REGS_ODTCR_WRODT0_MASK		0x000F0000
#define DWC_DDR_PHY_REGS_ODTCR_WRODT0_SHIFT		16

#define DWC_DDR_PHY_REGS_ODTCR_WRODT1_MASK		0x00F00000
#define DWC_DDR_PHY_REGS_ODTCR_WRODT1_SHIFT		20

#define DWC_DDR_PHY_REGS_ODTCR_WRODT2_MASK		0x0F000000
#define DWC_DDR_PHY_REGS_ODTCR_WRODT2_SHIFT		24

#define DWC_DDR_PHY_REGS_ODTCR_WRODT3_MASK		0xF0000000
#define DWC_DDR_PHY_REGS_ODTCR_WRODT3_SHIFT		28

/* Register DTCR field DTRPTN */
/**
 * Data Training Repeat Number: Repeat number used to confirm stability of DDR
 * write or read
*/
#define DWC_DDR_PHY_REGS_DTCR_DTRPTN_MASK		0x0000000f
#define DWC_DDR_PHY_REGS_DTCR_DTRPTN_SHIFT		0

/* Register DTCR field DTRANK */
/**
 * Data Training Rank: Selects the SDRAM rank to be used during data bit deskew
 * and eye centering.
*/
#define DWC_DDR_PHY_REGS_DTCR_DTRANK_MASK		0x00000030
#define DWC_DDR_PHY_REGS_DTCR_DTRANK_SHIFT		4

/* Register DTCR field DTMPR */
/**
 * Data Training Using MPR (DDR3 Only): Specifies, if set, that DQS gate
 * training should use the SDRAM Multi-Purpose Register (MPR) register.
 * Otherwise datatraining is performed by first writing to some locations in
 * the SDRAM and then reading them back.
 */
#define DWC_DDR_PHY_REGS_DTCR_DTMPR			0x00000040

/* Register DTCR field DTCMPD */
/**
 * Data Training Compare Data: Specifies, if set, that DQS gate training should also check if the
 * returning read data is correct. Otherwise data-training only checks if the correct number of DQS
 * edges were returned.
 */
#define DWC_DDR_PHY_REGS_DTCR_DTCMPD			0x00000080

/* Register DTCR field DTWBDDM */
/**
 * Data Training Write Bit Deskew Data Mask. If set it enables write bit deskew of the data mask
 */
#define DWC_DDR_PHY_REGS_DTCR_DTWBDDM			0x00001000

/* Register DTCR field DTBDC */
/**
 * Data Training Bit Deskew Centering: Enables, if set, eye centering capability during write and
 * read bit deskew training.
 */
#define DWC_DDR_PHY_REGS_DTCR_DTBDC			0x00002000

/* Register DTCR field DTDBS */
/**
 * Data Training Debug Byte Select: Selects the byte during data training debug
 * mode.
 */
#define DWC_DDR_PHY_REGS_DTCR_DTDBS_MASK		0x000f0000
#define DWC_DDR_PHY_REGS_DTCR_DTDBS_SHIFT		16
#define DWC_DDR_PHY_REGS_DTCR_DTDBS(i)			\
	((i) << DWC_DDR_PHY_REGS_DTCR_DTDBS_SHIFT)

/* Register DTCR field DTEXG */
/**
 * Data Training with Early/Extended Gate: Specifies if set that the DQS gate
 * training should be performed with an early/extended gate as specified in
 * DSGCR.DQSGX.
 */
#define DWC_DDR_PHY_REGS_DTCR_DTEXG			0x00800000

/* Register DTCR field RANKEN */
/**
 * Rank Enable: Specifies the ranks that are enabled for data-training. Bit 0
 * controls rank 0, bit 1 controls rank 1, bit 2 controls rank 2, and bit 3
 * controls rank 3. Setting the bit to '1' enables the rank, and setting it to
 * '0' disables the rank.
 */
#define DWC_DDR_PHY_REGS_DTCR_RANKEN_MASK		0x0f000000
#define DWC_DDR_PHY_REGS_DTCR_RANKEN_SHIFT		24

/* Register DTCR field RFSHDT */
/**
 * Refresh During Training: A non-zero value specifies that a burst of refreshes
 * equal to the number specified in this field should be sent to the SDRAM after
 * training each rank except the last rank.
 */
#define DWC_DDR_PHY_REGS_DTCR_RFSHDT_MASK		0xf0000000
#define DWC_DDR_PHY_REGS_DTCR_RFSHDT_SHIFT		28

/* Register DTEDR0 field DTWLMN */
/* Data Training WDQ LCDL Minimum */
#define DWC_DDR_PHY_REGS_DTEDR0_DTWLMN_MASK		0x000000ff
#define DWC_DDR_PHY_REGS_DTEDR0_DTWLMN_SHIFT		0

/* Register DTEDR0 field DTWLMX */
/* Data Training WDQ LCDL Maximum */
#define DWC_DDR_PHY_REGS_DTEDR0_DTWLMX_MASK		0x0000ff00
#define DWC_DDR_PHY_REGS_DTEDR0_DTWLMX_SHIFT		8

/* Register DTEDR0 field DTWBMN */
/* Data Training Write BDL Shift Minimum */
#define DWC_DDR_PHY_REGS_DTEDR0_DTWBMN_MASK		0x00ff0000
#define DWC_DDR_PHY_REGS_DTEDR0_DTWBMN_SHIFT		16

/* Register DTEDR0 field DTWBMX */
/* Data Training Write BDL Shift Minimum */
#define DWC_DDR_PHY_REGS_DTEDR0_DTWBMX_MASK		0xff000000
#define DWC_DDR_PHY_REGS_DTEDR0_DTWBMX_SHIFT		24

/* Register DTEDR1 field DTRLMN */
/* Data Training RDQS LCDL Minimum */
#define DWC_DDR_PHY_REGS_DTEDR1_DTRLMN_MASK		0x000000ff
#define DWC_DDR_PHY_REGS_DTEDR1_DTRLMN_SHIFT		0

/* Register DTEDR1 field DTRLMX */
/* Data Training RDQS LCDL Maximum */
#define DWC_DDR_PHY_REGS_DTEDR1_DTRLMX_MASK		0x0000ff00
#define DWC_DDR_PHY_REGS_DTEDR1_DTRLMX_SHIFT		8

/* Register DTEDR1 field DTRBMN */
/* Data Training Read BDL Shift Minimum */
#define DWC_DDR_PHY_REGS_DTEDR1_DTRBMN_MASK		0x00ff0000
#define DWC_DDR_PHY_REGS_DTEDR1_DTRBMN_SHIFT		16

/* Register DTEDR1 field DTRBMX */
/* Data Training Read BDL Shift Minimum */
#define DWC_DDR_PHY_REGS_DTEDR1_DTRBMX_MASK		0xff000000
#define DWC_DDR_PHY_REGS_DTEDR1_DTRBMX_SHIFT		24

/* Register RDIMMGCR0 field RDIMM */
/**
 * Registered DIMM: Indicates if set that a registered DIMM is used. In this
 * case, the PUBm2 increases the SDRAM write and read latencies (WL/RL) by 1
 * and also enforces that accesses adhere to RDIMM buffer chip. This only
 * applies to PUBm2 internal SDRAM transactions. Transactions generated by the
 * controller must make its own adjustments to WL/RL when using a registered
 * DIMM. The DCR.NOSRA register bit must be set to '1' if using the standard
 * RDIMM buffer chip so that normal DRAM accesses do not assert multiple chip
 * select bits at the same time.
 */
#define DWC_DDR_PHY_REGS_RDIMMGCR0_RDIMM		0x00000001

/* Register ODTCTLR field FRCEN */
/**
 * ODT force value enable : when this field is set, the ODT
 * value is taken from the FRCVAL field.
 * One bit for each rank.
 */
#define DWC_DDR_PHY_REGS_ODTCTLR_FRCEN_MASK		0x0000000f
#define DWC_DDR_PHY_REGS_ODTCTLR_FRCEN_SHIFT		0

/* Register ODTCTLR field FRCVAL */
/**
 * ODT force value : when FRCEN field is set, the ODT
 * value is taken from this field.
 * One bit for each rank.
 */
#define DWC_DDR_PHY_REGS_ODTCTLR_FRCVAL_MASK		0x000000f0
#define DWC_DDR_PHY_REGS_ODTCTLR_FRCVAL_SHIFT		4

/* Register BISTRR field BINST */
/**
Selects the BIST instruction to be executed: Valid values are:
000 = NOP: No operation
001 = Run: Triggers the running of the BIST.
010 = Stop: Stops the running of the BIST.
011 = Reset: Resets all BIST run-time registers, such as error counters.
100 – 111 Reserved
 */
#define DWC_DDR_PHY_REGS_BISTRR_BINST_MASK		0x00000007
#define DWC_DDR_PHY_REGS_BISTRR_BINST_SHIFT		0
#define DWC_DDR_PHY_REGS_BISTRR_BINST_NOP		\
	(0x0 << DWC_DDR_PHY_REGS_BISTRR_BINST_SHIFT)
#define DWC_DDR_PHY_REGS_BISTRR_BINST_RUN		\
	(0x1 << DWC_DDR_PHY_REGS_BISTRR_BINST_SHIFT)
#define DWC_DDR_PHY_REGS_BISTRR_BINST_STOP		\
	(0x2 << DWC_DDR_PHY_REGS_BISTRR_BINST_SHIFT)
#define DWC_DDR_PHY_REGS_BISTRR_BINST_RESET		\
	(0x3 << DWC_DDR_PHY_REGS_BISTRR_BINST_SHIFT)

/* Register BISTRR field BMODE */
/**
BIST Mode: Selects the mode in which BIST is run. Valid values are:
0 = Loopback mode: Address, commands and data loop back at the PHY I/Os.
1 = DRAM mode: Address, commands and data go to DRAM for normal memory
accesses.
*/
#define DWC_DDR_PHY_REGS_BISTRR_BMODE_MASK		0x00000008
#define DWC_DDR_PHY_REGS_BISTRR_BMODE_SHIFT		3
#define DWC_DDR_PHY_REGS_BISTRR_BMODE_LOOPBACK		\
	(0x0 << DWC_DDR_PHY_REGS_BISTRR_BMODE_SHIFT)
#define DWC_DDR_PHY_REGS_BISTRR_BMODE_DRAM		\
	(0x1 << DWC_DDR_PHY_REGS_BISTRR_BMODE_SHIFT)

/* Register BISTRR field BDXEN */
/**
 * DATX8 Enable: Enables the running of BIST on the data byte lane PHYs.
 * This bit is exclusive with BACEN, i.e. both cannot be set to '1' at the same
 * time.
 */
#define DWC_DDR_PHY_REGS_BISTRR_BDXEN			0x00004000

/* Register BISTRR field BACEN */
/**
 * BIST AC Enable: Enables the running of BIST on the address/command lane PHY.
 * This bit is exclusive with BDXEN, i.e. both cannot be set to '1' at the same
 * time.
 */
#define DWC_DDR_PHY_REGS_BISTRR_BACEN			0x00008000

/* Register BISTRR field BDPAT */
/**
BIST Data Pattern: Selects the data pattern used during BIST. Valid values are:
00 = Walking 0
01 = Walking 1
10 = LFSR-based pseudo-random
11 = User programmable (Not valid for AC loopback).
*/
#define DWC_DDR_PHY_REGS_BISTRR_BDPAT_MASK		0x00060000
#define DWC_DDR_PHY_REGS_BISTRR_BDPAT_SHIFT		17
#define DWC_DDR_PHY_REGS_BISTRR_BDPAT_WALKING_0		\
	(0x0 << DWC_DDR_PHY_REGS_BISTRR_BDPAT_SHIFT)
#define DWC_DDR_PHY_REGS_BISTRR_BDPAT_WALKING_1		\
	(0x1 << DWC_DDR_PHY_REGS_BISTRR_BDPAT_SHIFT)
#define DWC_DDR_PHY_REGS_BISTRR_BDPAT_LFSR		\
	(0x2 << DWC_DDR_PHY_REGS_BISTRR_BDPAT_SHIFT)
#define DWC_DDR_PHY_REGS_BISTRR_BDPAT_USER		\
	(0x3 << DWC_DDR_PHY_REGS_BISTRR_BDPAT_SHIFT)

/* Register BISTRR field BDXSEL */
/**
BIST DATX8 Select: Select the byte lane for comparison of loopback/read data.
Valid values are 0 to 8.
*/
#define DWC_DDR_PHY_REGS_BISTRR_BDXSEL_MASK		0x00780000
#define DWC_DDR_PHY_REGS_BISTRR_BDXSEL_SHIFT		19
#define DWC_DDR_PHY_REGS_BISTRR_BDXSEL(val)		\
	((val) << DWC_DDR_PHY_REGS_BISTRR_BDXSEL_SHIFT)

/* Register BISTWCR field BWCNT */
/**
BIST Word Count: Indicates the number of words to generate during BIST. This
must be a multiple of DRAM burst length (BL) divided by 2, e.g. for BL=8, valid
values are 4, 8, 12, 16, and so on.
*/
#define DWC_DDR_PHY_REGS_BISTWCR_BWCNT_MASK		0x0000ffff
#define DWC_DDR_PHY_REGS_BISTWCR_BWCNT_SHIFT		0
#define DWC_DDR_PHY_REGS_BISTWCR_BWCNT(cnt)	\
	((cnt) << DWC_DDR_PHY_REGS_BISTWCR_BWCNT_SHIFT)

/* Register BISTAR0 field BCOL */
/**
 * BIST Column Address: Selects the SDRAM column address to be used during
 * BIST. The lower bits of this address must be "0000" for BL16, "000" for BL8,
 * "00" for BL4 and "0" for BL2.
 */
#define DWC_DDR_PHY_REGS_BISTAR0_BCOL_MASK		0x00000fff
#define DWC_DDR_PHY_REGS_BISTAR0_BCOL_SHIFT		0
#define DWC_DDR_PHY_REGS_BISTAR0_BCOL(val)		\
	((val) << DWC_DDR_PHY_REGS_BISTAR0_BCOL_SHIFT)

/* Register BISTAR0 field BROW */
/**
BIST Row Address: Selects the SDRAM row address to be used during BIST
*/
#define DWC_DDR_PHY_REGS_BISTAR0_BROW_MASK		0x0ffff000
#define DWC_DDR_PHY_REGS_BISTAR0_BROW_SHIFT		12
#define DWC_DDR_PHY_REGS_BISTAR0_BROW(val)		\
	((val) << DWC_DDR_PHY_REGS_BISTAR0_BROW_SHIFT)

/* Register BISTAR0 field BBANK */
/**
BIST Bank Address: Selects the SDRAM bank address to be used during BIST.
*/
#define DWC_DDR_PHY_REGS_BISTAR0_BBANK_MASK		0x70000000
#define DWC_DDR_PHY_REGS_BISTAR0_BBANK_SHIFT		28
#define DWC_DDR_PHY_REGS_BISTAR0_BBANK(val)		\
	((val) << DWC_DDR_PHY_REGS_BISTAR0_BBANK_SHIFT)

/* Register BISTAR1 field BRANK */
/**
BIST Rank: Selects the SDRAM rank to be used during BIST. Valid values range
from 0 to maximum ranks minus 1.
*/
#define DWC_DDR_PHY_REGS_BISTAR1_BRANK_MASK		0x00000003
#define DWC_DDR_PHY_REGS_BISTAR1_BRANK_SHIFT		0
#define DWC_DDR_PHY_REGS_BISTAR1_BRANK(val)		\
	((val) << DWC_DDR_PHY_REGS_BISTAR1_BRANK_SHIFT)

/* Register BISTAR1 field BMRANK */
/**
BIST Maximum Rank: Specifies the maximum SDRAM rank to be used during BIST.
The default value is set to maximum ranks minus 1.
*/
#define DWC_DDR_PHY_REGS_BISTAR1_BMRANK_MASK		0x0000000c
#define DWC_DDR_PHY_REGS_BISTAR1_BMRANK_SHIFT		2
#define DWC_DDR_PHY_REGS_BISTAR1_BMRANK(val)		\
	((val) << DWC_DDR_PHY_REGS_BISTAR1_BMRANK_SHIFT)

/* Register BISTAR1 field BAINC */
/**
 * BIST Address Increment: Selects the value by which the SDRAM address is
 * incremented for each write/read access. This value must be at the beginning
 * of a burst boundary, i.e. the lower bits must be "0000" for BL16, "000" for
 * BL8, "00" for BL4 and "0" for BL2.
*/
#define DWC_DDR_PHY_REGS_BISTAR1_BAINC_MASK		0x0000fff0
#define DWC_DDR_PHY_REGS_BISTAR1_BAINC_SHIFT		4
#define DWC_DDR_PHY_REGS_BISTAR1_BAINC(val)		\
	((val) << DWC_DDR_PHY_REGS_BISTAR1_BAINC_SHIFT)

/* Register BISTAR2 field BMCOL */
/**
BIST Maximum Column Address: Specifies the maximum SDRAM column address
to be used during BIST before the address increments to the next row.
*/
#define DWC_DDR_PHY_REGS_BISTAR2_BMCOL_MASK		0x00000fff
#define DWC_DDR_PHY_REGS_BISTAR2_BMCOL_SHIFT		0
#define DWC_DDR_PHY_REGS_BISTAR2_BMCOL(val)		\
	((val) << DWC_DDR_PHY_REGS_BISTAR2_BMCOL_SHIFT)

/* Register BISTAR2 field BMROW */
/**
BIST Maximum Row Address: Specifies the maximum SDRAM row address to be
used during BIST before the address increments to the next bank.
*/
#define DWC_DDR_PHY_REGS_BISTAR2_BMROW_MASK		0x0ffff000
#define DWC_DDR_PHY_REGS_BISTAR2_BMROW_SHIFT		12
#define DWC_DDR_PHY_REGS_BISTAR2_BMROW(val)		\
	((val) << DWC_DDR_PHY_REGS_BISTAR2_BMROW_SHIFT)

/* Register BISTAR2 field BMBANK */
/**
BIST Maximum Bank Address: Specifies the maximum SDRAM bank address to be
used during BIST before the address increments to the next rank.
*/
#define DWC_DDR_PHY_REGS_BISTAR2_BMBANK_MASK		0x70000000
#define DWC_DDR_PHY_REGS_BISTAR2_BMBANK_SHIFT		28
#define DWC_DDR_PHY_REGS_BISTAR2_BMBANK(val)		\
	((val) << DWC_DDR_PHY_REGS_BISTAR2_BMBANK_SHIFT)

/* Register BISTUDPR field BUDP0 */
/**
 * BIST User Data Pattern 0: Data to be applied on even DQ pins during BIST.
 */
#define DWC_DDR_PHY_REGS_BISTUDPR_BUDP0_MASK		0x0000ffff
#define DWC_DDR_PHY_REGS_BISTUDPR_BUDP0_SHIFT		0
#define DWC_DDR_PHY_REGS_BISTUDPR_BUDP0(val)		\
	((val) << DWC_DDR_PHY_REGS_BISTUDPR_BUDP0_SHIFT)

/* Register BISTUDPR field BUDP1 */
/**
 * BIST User Data Pattern 1: Data to be applied on odd DQ pins during BIST.
 */
#define DWC_DDR_PHY_REGS_BISTUDPR_BUDP1_MASK		0xffff0000
#define DWC_DDR_PHY_REGS_BISTUDPR_BUDP1_SHIFT		16
#define DWC_DDR_PHY_REGS_BISTUDPR_BUDP1(val)		\
	((val) << DWC_DDR_PHY_REGS_BISTUDPR_BUDP1_SHIFT)

/* Register BISTGSR field BDONE */
/**
BIST Done: Indicates if set that the BIST has finished executing. This bit is reset to
zero when BIST is triggered.
*/
#define DWC_DDR_PHY_REGS_BISTGSR_BDONE			0x00000001

/* Register BISTGSR field BACERR */
/**
BIST Address/Command Error: indicates if set that there is a data comparison error
in the address/command lane.
*/
#define DWC_DDR_PHY_REGS_BISTGSR_BACERR			0x00000002

/* Register BISTGSR field BDXERR */
/**
BIST Data Error: indicates if set that there is a data comparison error in the byte
lane.
*/
#define DWC_DDR_PHY_REGS_BISTGSR_BDXERR			0x00000004


/**** ZQCR register ****/
/* Impedance Calibration Bypass: Disables, if set, impedance cal ... */
#define DWC_DDR_PHY_REGS_ZQCR_ZCALBYP (1 << 0)
/* Impedance Calibration Trigger: A write of 1b1 to this bit tri ... */
#define DWC_DDR_PHY_REGS_ZQCR_ZCAL (1 << 1)
/* ZQ Power Down; Powers down, if set, all PZQ cells */
#define DWC_DDR_PHY_REGS_ZQCR_ZQPD (1 << 2)
/* Programmable Wait: specifies the number of clock cycles to re ... */
#define DWC_DDR_PHY_REGS_ZQCR_PGWAIT_MASK 0x00000700
#define DWC_DDR_PHY_REGS_ZQCR_PGWAIT_SHIFT 8
/* Impedance Calibration Type: Valid decoding of the calibration ... */
#define DWC_DDR_PHY_REGS_ZQCR_ZCALT_MASK 0x00003800
#define DWC_DDR_PHY_REGS_ZQCR_ZCALT_SHIFT 11
/* Maximum number of averaging rounds to be used by averaging al ... */
#define DWC_DDR_PHY_REGS_ZQCR_AVGMAX_MASK 0x0000C000
#define DWC_DDR_PHY_REGS_ZQCR_AVGMAX_SHIFT 14
/* Averaging algorithm enable, if set, enables averaging algorit ... */
#define DWC_DDR_PHY_REGS_ZQCR_AVGEN (1 << 16)
/* IO VT Drift Limit: Specifies the minimum change in the Impeda ... */
#define DWC_DDR_PHY_REGS_ZQCR_IODLMT_MASK 0x00FE0000
#define DWC_DDR_PHY_REGS_ZQCR_IODLMT_SHIFT 17

/**** ZQnPR register ****/
/* Impedance Divide Ratio: Selects the external resistor divide  ... */
#define DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_MASK 0x000000FF
#define DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_SHIFT 0
#define DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_MASK 0x0000000F
#define DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_SHIFT 0
#define DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_MASK 0x000000F0
#define DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_SHIFT 4

#define DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_80OHM		\
	(0x5 << DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_SHIFT)
#define DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_68OHM		\
	(0x6 << DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_SHIFT)
#define DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_60OHM		\
	(0x7 << DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_SHIFT)
#define DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_53OHM		\
	(0x8 << DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_SHIFT)
#define DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_48OHM		\
	(0x9 << DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_SHIFT)
#define DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_44OHM		\
	(0xA << DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_SHIFT)
#define DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_40OHM		\
	(0xB << DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_SHIFT)
#define DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_37OHM		\
	(0xC << DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_SHIFT)
#define DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_34OHM		\
	(0xD << DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_SHIFT)
#define DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_32OHM		\
	(0xE << DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_SHIFT)
#define DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_30OHM		\
	(0xF << DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_SHIFT)

#define DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_200OHM		\
	(0x1 << DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_SHIFT)
#define DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_133OHM		\
	(0x2 << DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_SHIFT)
#define DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_100OHM		\
	(0x3 << DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_SHIFT)
#define DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_77OHM		\
	(0x4 << DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_SHIFT)
#define DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_66OHM		\
	(0x5 << DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_SHIFT)
#define DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_56OHM		\
	(0x6 << DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_SHIFT)
#define DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_50OHM		\
	(0x7 << DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_SHIFT)
#define DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_44OHM		\
	(0x8 << DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_SHIFT)
#define DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_40OHM		\
	(0x9 << DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_SHIFT)
#define DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_36OHM		\
	(0xA << DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_SHIFT)
#define DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_33OHM		\
	(0xB << DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_SHIFT)
#define DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_30OHM		\
	(0xC << DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_SHIFT)
#define DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_28OHM		\
	(0xD << DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_SHIFT)
#define DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_26OHM		\
	(0xE << DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_SHIFT)
#define DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_25OHM		\
	(0xF << DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_SHIFT)

/**** ZQnSR register ****/
/* Output impedance pull-down calibration status */
#define DWC_DDR_PHY_REGS_ZQnSR_ZPD_MASK 0x00000003
#define DWC_DDR_PHY_REGS_ZQnSR_ZPD_SHIFT 0
/* Output impedance pull-up calibration status */
#define DWC_DDR_PHY_REGS_ZQnSR_ZPU_MASK 0x0000000C
#define DWC_DDR_PHY_REGS_ZQnSR_ZPU_SHIFT 2
/* On-die termination (ODT) pull-down calibration status */
#define DWC_DDR_PHY_REGS_ZQnSR_OPD_MASK 0x00000030
#define DWC_DDR_PHY_REGS_ZQnSR_OPD_SHIFT 4
/* On-die termination (ODT) pull-up calibration status */
#define DWC_DDR_PHY_REGS_ZQnSR_OPU_MASK 0x000000C0
#define DWC_DDR_PHY_REGS_ZQnSR_OPU_SHIFT 6
/* Impedance Calibration Error: If set, indicates that there was ... */
#define DWC_DDR_PHY_REGS_ZQnSR_ZERR (1 << 8)
/* Impedance Calibration Done: Indicates that the first round of ... */
#define DWC_DDR_PHY_REGS_ZQnSR_ZDONE (1 << 9)
/* Reserved.  Return zeros on reads. */
#define DWC_DDR_PHY_REGS_ZQnSR_RESERVED_31_10_MASK 0xFFFFFC00
#define DWC_DDR_PHY_REGS_ZQnSR_RESERVED_31_10_SHIFT 10

/**** DXNGSR0 register ****/
/* Write DQ Calibration: Indicates, if set, that the DATX8 has f ... */
#define DWC_DDR_PHY_REGS_DXNGSR0_WDQCAL (1 << 0)
/* Read DQS Calibration: Indicates, if set, that the DATX8 has f ... */
#define DWC_DDR_PHY_REGS_DXNGSR0_RDQSCAL (1 << 1)
/* Read DQS# Calibration: Indicates, if set, that the DATX8 has  ... */
#define DWC_DDR_PHY_REGS_DXNGSR0_RDQSNCAL (1 << 2)
/* Read DQS gating Calibration: Indicates, if set, that the DATX ... */
#define DWC_DDR_PHY_REGS_DXNGSR0_GDQSCAL (1 << 3)
/* Write Leveling Calibration: Indicates, if set, that the DATX8 ... */
#define DWC_DDR_PHY_REGS_DXNGSR0_WLCAL (1 << 4)
/* Write Leveling Done: Indicates, if set, that the DATX8 has co ... */
#define DWC_DDR_PHY_REGS_DXNGSR0_WLDONE (1 << 5)
/* Write Leveling Error: Indicates, if set, that there is a writ ... */
#define DWC_DDR_PHY_REGS_DXNGSR0_WLERR (1 << 6)
/* Write Leveling Period: Returns the DDR clock period measured  ... */
#define DWC_DDR_PHY_REGS_DXNGSR0_WLPRD_MASK 0x00007F80
#define DWC_DDR_PHY_REGS_DXNGSR0_WLPRD_SHIFT 7
/* DATX8 PLL Lock: Indicates, if set, that that DATX8 PLL has lo ... */
#define DWC_DDR_PHY_REGS_DXNGSR0_DPLOCK (1 << 15)
/* Read DQS gating Period: Returns the DDR clock period measured ... */
#define DWC_DDR_PHY_REGS_DXNGSR0_GDQSPRD_MASK 0x00FF0000
#define DWC_DDR_PHY_REGS_DXNGSR0_GDQSPRD_SHIFT 16
/* DQS Gate Training Error: Indicates if set that there is an er ... */
#define DWC_DDR_PHY_REGS_DXNGSR0_QSGERR_MASK 0x0F000000
#define DWC_DDR_PHY_REGS_DXNGSR0_QSGERR_SHIFT 24
/* Write Leveling DQ Status: Captures the write leveling DQ stat ... */
#define DWC_DDR_PHY_REGS_DXNGSR0_WLDQ (1 << 28)

/**** DXNGSR2 register ****/
/* Read Bit Deskew Error: Indicates, if set, that the DATX8 has  ... */
#define DWC_DDR_PHY_REGS_DXNGSR2_RDERR (1 << 0)
/* Read Bit Deskew Warning: Indicates, if set, that the DATX8 ha ... */
#define DWC_DDR_PHY_REGS_DXNGSR2_RDWN (1 << 1)
/* Write Bit Deskew Error: Indicates, if set, that the DATX8 has ... */
#define DWC_DDR_PHY_REGS_DXNGSR2_WDERR (1 << 2)
/* Write Bit Deskew Warning: Indicates, if set, that the DATX8 h ... */
#define DWC_DDR_PHY_REGS_DXNGSR2_WDWN (1 << 3)
/* Read Eye Centering Error: Indicates, if set, that the DATX8 h ... */
#define DWC_DDR_PHY_REGS_DXNGSR2_REERR (1 << 4)
/* Read Eye Centering Warning: Indicates, if set, that the DATX8 ... */
#define DWC_DDR_PHY_REGS_DXNGSR2_REWN (1 << 5)
/* Write Eye Centering Error: Indicates, if set, that the DATX8  ... */
#define DWC_DDR_PHY_REGS_DXNGSR2_WEERR (1 << 6)
/* Write Eye Centering Warning: Indicates, if set, that the DATX ... */
#define DWC_DDR_PHY_REGS_DXNGSR2_WEWN (1 << 7)
/* Error Status:  If an error occurred for this lane as indicate ... */
#define DWC_DDR_PHY_REGS_DXNGSR2_ESTAT_MASK 0x00000F00
#define DWC_DDR_PHY_REGS_DXNGSR2_ESTAT_SHIFT 8

/* Register DXnGCR0 field DXEN */
/**
 * Data Byte Enable: Enables if set the data byte. Setting this bit to ‘0’
 * disables the byte, i.e. the byte is not used in PHY initialization or
 * training and is ignored during SDRAM read/write operations.
 */
#define DWC_DDR_PHY_REGS_DXNGCR0_DXEN			0x00000001

/* Register DXnGCR0 field DQSGOE */
/**
DQSG Output Enable: Enables, when set, the output driver (OE pin)on the I/O for
DQS gate.
 */
#define DWC_DDR_PHY_REGS_DXNGCR0_DQSGOE			0x00000004

/* Register DXnGCR0 field DQSGODT */
/**
DQSG On-Die Termination: Enables, when set, the on-die termination (TE pin)on
the I/O for DQS gate. Note that in typical usage, DQSGOE will always be on,
rendering this control bit meaningless.
 */
#define DWC_DDR_PHY_REGS_DXNGCR0_DQSGODT		0x00000008

/* Register DXnGCR0 field DQSGPDD */
/**
DQSG Power Down Driver: Powers down, if set, the output driver on the I/O for
DQS gate. This bit is ORed with the common PDD configuration bit.
 */
#define DWC_DDR_PHY_REGS_DXNGCR0_DQSGPDD		0x00000010

/* Register DXnGCR0 field DQSGPDR */
/**
DQSG Power Down Receiver: Powers down, if set, the input receiver on the I/O for
DQS gate. This bit is ORed with the common PDR configuration bit.
 */
#define DWC_DDR_PHY_REGS_DXNGCR0_DQSGPDR		0x00000020

/* Register DXnGCR0 field DQSRPD */
/**
DQSR Power Down: Powers down, if set, the PDQSR cell. This bit is ORed with the
common PDR configuration bit
 */
#define DWC_DDR_PHY_REGS_DXNGCR0_DQSRPD		0x00000040

/* Register DXnGCR0 field DSEN */
/**
 * Write DQS Enable: Controls whether the write DQS going to the SDRAM is enabled (toggling) or
 * disabled (static value) and whether the DQS is inverted.  DQS# is always the inversion of DQS.
 * These values are valid only when DQS/DQS# output enable is on, otherwise the DQS/DQS# is
 * tristated. Valid settings are:
 * 00 = DQS disabled (Driven to constant 0)
 * 01 = DQS toggling with inverted polarity
 * 10 = DQS toggling with normal polarity (This should be the default setting)
 * 11 = DQS disabled (Driven to constant 1)
 */
#define DWC_DDR_PHY_REGS_DXNGCR0_DSEN_MASK		0x00000180
#define DWC_DDR_PHY_REGS_DXNGCR0_DSEN_SHIFT		7
#define DWC_DDR_PHY_REGS_DXNGCR0_DSEN_VAL_DISABLED_0	\
	(0 << DWC_DDR_PHY_REGS_DXNGCR0_DSEN_SHIFT)
#define DWC_DDR_PHY_REGS_DXNGCR0_DSEN_VAL_INVERTED	\
	(1 << DWC_DDR_PHY_REGS_DXNGCR0_DSEN_SHIFT)
#define DWC_DDR_PHY_REGS_DXNGCR0_DSEN_VAL_NORMAL	\
	(2 << DWC_DDR_PHY_REGS_DXNGCR0_DSEN_SHIFT)
#define DWC_DDR_PHY_REGS_DXNGCR0_DSEN_VAL_DISABLED_1	\
	(3 << DWC_DDR_PHY_REGS_DXNGCR0_DSEN_SHIFT)

/* Register DXnGCR0 field RTTOH */
/**
 * RTT Output Hold: Indicates the number of clock cycles (from 0 to 3) after the read data postamble
 * for which ODT control should remain set to DQSODT for DQS or DQODT for DQ/DM before disabling it
 * (setting it to '0') when using dynamic ODT control. ODT is disabled almost RTTOH clock cycles
 * after the read postamble.
 */
#define DWC_DDR_PHY_REGS_DXNGCR0_RTTOH_MASK	0x00000600
#define DWC_DDR_PHY_REGS_DXNGCR0_RTTOH_SHIFT	9

/* Register DXnGCR0 field PLLPD */
/**
 * PLL Power Down: Puts the byte PLL in power down mode by driving the PLL
 * power down pin. This bit is not self-clearing and a '0' must be written to
 * de-assert the power-down. This bit is ORed with the global PLLPD
 * configuration bit
 */
#define DWC_DDR_PHY_REGS_DXNGCR0_PLLPD		0x00020000

/* Register DXnGCR0 field WLRKEN */
/* Write Level Rank Enable: Specifies the ranks that should be write leveled
 * for this byte. Write leveling responses from ranks that are not enabled for
 * write leveling for a particular byte are ignored and write leveling is
 * flagged as done for these ranks. WLRKEN[0] enables rank 0, [1] enables rank
 * 1, [2] enables rank 2, and [3] enables rank 3.
 */
#define DWC_DDR_PHY_REGS_DXNGCR0_WLRKEN_MASK		0x3c000000
#define DWC_DDR_PHY_REGS_DXNGCR0_WLRKEN_SHIFT		26

/* Register DXnGCR0 field MDLEN */
/**
Master Delay Line Enable: Enables, if set, the DATX8 master delay line calibration
to perform subsequent period measurements following the initial period
measurements that are performed after reset or on when calibration is manually
triggered. These additional measurements are accumulated and filtered as long as
this bit remains high. This bit is combined with the common DATX8 MDL enable bit
 */
#define DWC_DDR_PHY_REGS_DXNGCR0_MDLEN		0x40000000

/* Register DXnGCR0 field CALBYP */
/**
Calibration Bypass: Prevents, if set, period measurement calibration from
automatically triggering after PHY initialization.
 */
#define DWC_DDR_PHY_REGS_DXNGCR0_CALBYP		0x80000000

/* Register BISTWER */
/*
 * BIST Word Error Register is used to return the number of errors on the
 * full word (all bits) of the address/command lane and data byte lane.
 * An error on any bit of the address/command bus will increment
 * the address/command word error count.
 * Similarly any error on the data bits or data mask bits will increment
 * the data byte lane word error count.
 */

/*
 * Address/Command Word Error: Indicates the number of word errors on the
 * address/command lane. An error on any bit of the address/command bus
 * increments the error count.
 */
#define DWC_DDR_PHY_REGS_BISTWER_ACWER_MASK 		0x0000FFFF
#define DWC_DDR_PHY_REGS_BISTWER_ACWER_SHIFT		0
/*
 * Byte Word Error: Indicates the number of word errors on the byte lane.
 * An error on any bit of the data bus including the data mask bit increments
 * the error count.
 */
#define DWC_DDR_PHY_REGS_BISTWER_DXWER_MASK 		0xFFFF0000
#define DWC_DDR_PHY_REGS_BISTWER_DXWER_SHIFT		16

/* Register BISTBER2 */
/*
 * Data Bit Error: The error count for even DQS cycles.
 * The first 16 bits indicate the error count for the first data beat
 * (i.e. the data driven out on DQ[7:0] on the rising edge of DQS).
 * The second 16 bits indicate the error on the second data beat
 * (i.e. the error count of the data driven out on DQ[7:0] on
 * the falling edge of DQS).
 * For each of the 16-bit group, the first 2 bits are for DQ[0],
 * the second for DQ[1], and so on
 */
#define DWC_DDR_PHY_REGS_BISTBER2_DQBER0_RISG_DQ_I_SHIFT(i)	(2*(i))
#define DWC_DDR_PHY_REGS_BISTBER2_DQBER0_RISG_DQ_I_MASK(i)	\
	(0x3 << DWC_DDR_PHY_REGS_BISTBER2_DQBER0_RISG_DQ_I_SHIFT(i))
#define DWC_DDR_PHY_REGS_BISTBER2_DQBER0_FALL_DQ_I_SHIFT(i)	(2*(i) + 16)
#define DWC_DDR_PHY_REGS_BISTBER2_DQBER0_FALL_DQ_I_MASK(i)	\
	(0x3 << DWC_DDR_PHY_REGS_BISTBER2_DQBER0_FALL_DQ_I_SHIFT(i))

/* Register BISTBER3 */
/*
 * Data Bit Error: The error count for odd DQS cycles.
 * The first 16 bits indicate the error count for the first data beat
 * (i.e. the data driven out on DQ[7:0] on the rising edge of DQS).
 * The second 16 bits indicate the error on the second data beat
 * (i.e. the error count of the data driven out on DQ[7:0] on
 * the falling edge of DQS).
 * For each of the 16-bit group, the first 2 bits are for DQ[0],
 * the second for DQ[1], and so on
 */
#define DWC_DDR_PHY_REGS_BISTBER3_DQBER1_RISG_DQ_I_SHIFT(i)	(2*(i))
#define DWC_DDR_PHY_REGS_BISTBER3_DQBER1_RISG_DQ_I_MASK(i)	\
	(0x3 << DWC_DDR_PHY_REGS_BISTBER3_DQBER1_RISG_DQ_I_SHIFT(i))
#define DWC_DDR_PHY_REGS_BISTBER3_DQBER1_FALL_DQ_I_SHIFT(i)	(2*(i) + 16)
#define DWC_DDR_PHY_REGS_BISTBER3_DQBER1_FALL_DQ_I_MASK(i)	\
	(0x3 << DWC_DDR_PHY_REGS_BISTBER3_DQBER1_FALL_DQ_I_SHIFT(i))

/* Register BISTWCSR */
/*
 * BIST Word Count Status Register is used to return the number of
 * words received from the address/command lane and data byte lane.
 */
/*
 * Address/Command Word Count: Indicates the number of words
 * received from the address/command lane.
 */
#define DWC_DDR_PHY_REGS_BISTWCSR_ACWCNT_MASK 		0x0000FFFF
#define DWC_DDR_PHY_REGS_BISTWCSR_ACWCNT_SHIFT		0

/*
 * Byte Word Count: Indicates the number of words received from the byte lane.
 */
#define DWC_DDR_PHY_REGS_BISTWCSR_DXWCNT_MASK 		0xFFFF0000
#define DWC_DDR_PHY_REGS_BISTWCSR_DXWCNT_SHIFT		16

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
#define DWC_DDR_PHY_REGS_BISTFWR2_DQWEBS_1ST_DQ_I(i)		(i)
#define DWC_DDR_PHY_REGS_BISTFWR2_DQWEBS_2ND_DQ_I(i)		((i) + 8)
#define DWC_DDR_PHY_REGS_BISTFWR2_DQWEBS_3RD_DQ_I(i)		((i) + 16)
#define DWC_DDR_PHY_REGS_BISTFWR2_DQWEBS_4TH_DQ_I(i)		((i) + 24)

/* Register DXnGCR3 field DSPDRMODE */
/**
 * Enables the PDR mode values for DQS.
 * 00 : PDR Dynamic
 * 01 : PDR always ON
 * 10 : PDR always OFF
 * 11 : Reserved
 */
#define DWC_DDR_PHY_REGS_DXNGCR3_DSPDRMODE_MASK		0x0000000c
#define DWC_DDR_PHY_REGS_DXNGCR3_DSPDRMODE_SHIFT	2
#define DWC_DDR_PHY_REGS_DXNGCR3_DSPDRMODE_PDR_DYNAMIC		\
	(0x0 << DWC_DDR_PHY_REGS_DXNGCR3_DSPDRMODE_SHIFT)
#define DWC_DDR_PHY_REGS_DXNGCR3_DSPDRMODE_PDR_ALWAYS_ON	\
	(0x1 << DWC_DDR_PHY_REGS_DXNGCR3_DSPDRMODE_SHIFT)
#define DWC_DDR_PHY_REGS_DXNGCR3_DSPDRMODE_PDR_ALWAYS_OFF	\
	(0x2 << DWC_DDR_PHY_REGS_DXNGCR3_DSPDRMODE_SHIFT)

/*
 * The DATX8 bit delay line registers are used to select the delay value on
 * the BDLs used in the DATX8 macros.
 * A single BDL field in the BDLR register connects to a corresponding BDL.
 * The following tables describe the bits of the DATX8 BDLR register.
 */

/* Register DXnBDLR2 */
#define DWC_DDR_PHY_REGS_DXNBDLR2_DMWBD_MASK		0x0000001F
#define DWC_DDR_PHY_REGS_DXNBDLR2_DMWBD_SHIFT		0
#define DWC_DDR_PHY_REGS_DXNBDLR2_DSWBD_MASK		0x00001F00
#define DWC_DDR_PHY_REGS_DXNBDLR2_DSWBD_SHIFT		8
#define DWC_DDR_PHY_REGS_DXNBDLR2_OEBD_MASK		0x001F0000
#define DWC_DDR_PHY_REGS_DXNBDLR2_OEBD_SHIFT		16

/* Register DXnBDLR3 */
#define DWC_DDR_PHY_REGS_DXNBDLR3_DQ0RBD_MASK		0x0000001F
#define DWC_DDR_PHY_REGS_DXNBDLR3_DQ0RBD_SHIFT		0
#define DWC_DDR_PHY_REGS_DXNBDLR3_DQ1RBD_MASK		0x00001F00
#define DWC_DDR_PHY_REGS_DXNBDLR3_DQ1RBD_SHIFT		8
#define DWC_DDR_PHY_REGS_DXNBDLR3_DQ2RBD_MASK		0x001F0000
#define DWC_DDR_PHY_REGS_DXNBDLR3_DQ2RBD_SHIFT		16
#define DWC_DDR_PHY_REGS_DXNBDLR3_DQ3RBD_MASK		0x1F000000
#define DWC_DDR_PHY_REGS_DXNBDLR3_DQ3RBD_SHIFT		24

/* Register DXnBDLR4 */
#define DWC_DDR_PHY_REGS_DXNBDLR4_DQ4RBD_MASK		0x0000001F
#define DWC_DDR_PHY_REGS_DXNBDLR4_DQ4RBD_SHIFT		0
#define DWC_DDR_PHY_REGS_DXNBDLR4_DQ5RBD_MASK		0x00001F00
#define DWC_DDR_PHY_REGS_DXNBDLR4_DQ5RBD_SHIFT		8
#define DWC_DDR_PHY_REGS_DXNBDLR4_DQ6RBD_MASK		0x001F0000
#define DWC_DDR_PHY_REGS_DXNBDLR4_DQ6RBD_SHIFT		16
#define DWC_DDR_PHY_REGS_DXNBDLR4_DQ7RBD_MASK		0x1F000000
#define DWC_DDR_PHY_REGS_DXNBDLR4_DQ7RBD_SHIFT		24

/* Register DXnLCDLR0 field RnWLD */
#define DWC_DDR_PHY_REGS_DXNLCDLR0_RNWLD_MASK		0x000000ff
#define DWC_DDR_PHY_REGS_DXNLCDLR0_RNWLD_SHIFT		0
#define DWC_DDR_PHY_REGS_DXNLCDLR0_RNWLD_NUM_BITS	8

/* Register DXnLCDLR0 field R0WLD */
/** Rank 0 Write Leveling Delay: Rank 0 delay select for the write leveling
 * (WL) LCDL
 */
#define DWC_DDR_PHY_REGS_DXNLCDLR0_R0WLD_MASK		0x000000ff
#define DWC_DDR_PHY_REGS_DXNLCDLR0_R0WLD_SHIFT		0

/* Register DXnLCDLR0 field R1WLD */
/** Rank 0 Write Leveling Delay: Rank 1 delay select for the write leveling
 * (WL) LCDL
 */
#define DWC_DDR_PHY_REGS_DXNLCDLR0_R1WLD_MASK		0x0000ff00
#define DWC_DDR_PHY_REGS_DXNLCDLR0_R1WLD_SHIFT		8

/* Register DXnLCDLR0 field R2WLD */
/** Rank 0 Write Leveling Delay: Rank 2 delay select for the write leveling
 * (WL) LCDL
 */
#define DWC_DDR_PHY_REGS_DXNLCDLR0_R2WLD_MASK		0x00ff0000
#define DWC_DDR_PHY_REGS_DXNLCDLR0_R2WLD_SHIFT		16

/* Register DXnLCDLR0 field R3WLD */
/** Rank 0 Write Leveling Delay: Rank 3 delay select for the write leveling
 * (WL) LCDL
 */
#define DWC_DDR_PHY_REGS_DXNLCDLR0_R3WLD_MASK		0xff000000
#define DWC_DDR_PHY_REGS_DXNLCDLR0_R3WLD_SHIFT		24

/* Register DXnLCDLR1 field WDQD */
/* Write Data Delay: Delay select for the write data (WDQ) LCDL. */
#define DWC_DDR_PHY_REGS_DXNLCDLR1_WDQD_MASK		0x000000ff
#define DWC_DDR_PHY_REGS_DXNLCDLR1_WDQD_SHIFT		0

/* Register DXnLCDLR1 field RDQSD */
/* Read DQS Delay: Delay select for the read DQS (RDQS) LCDL. */
#define DWC_DDR_PHY_REGS_DXNLCDLR1_RDQSD_MASK		0x0000ff00
#define DWC_DDR_PHY_REGS_DXNLCDLR1_RDQSD_SHIFT		8

/* Register DXnLCDLR1 field RDQSND */
/* Read DQSN Delay: Delay select for the read DQSN (RDQS) LCDL. */
#define DWC_DDR_PHY_REGS_DXNLCDLR1_RDQSND_MASK		0x00ff0000
#define DWC_DDR_PHY_REGS_DXNLCDLR1_RDQSND_SHIFT		16

/* Register DXnLCDLR0 field RnWLD */
#define DWC_DDR_PHY_REGS_DXNLCDLR2_RNDQSGD_MASK		0x000000ff
#define DWC_DDR_PHY_REGS_DXNLCDLR2_RNDQSGD_SHIFT	0
#define DWC_DDR_PHY_REGS_DXNLCDLR2_RNDQSGD_NUM_BITS	8

/* Register DXnLCDLR2 field R0DQSGD */
/** Rank 0 Read DQS Gating Delay: Rank 0 delay select for the read DQS gating
 * (DQSG) LCDL.
 */
#define DWC_DDR_PHY_REGS_DXNLCDLR2_R0DQSGD_MASK		0x000000ff
#define DWC_DDR_PHY_REGS_DXNLCDLR2_R0DQSGD_SHIFT	0

/* Register DXnLCDLR2 field R1DQSGD */
/** Rank 1 Read DQS Gating Delay: Rank 1 delay select for the read DQS gating
 * (DQSG) LCDL.
 */
#define DWC_DDR_PHY_REGS_DXNLCDLR2_R1DQSGD_MASK		0x0000ff00
#define DWC_DDR_PHY_REGS_DXNLCDLR2_R1DQSGD_SHIFT	8

/* Register DXnLCDLR2 field R2DQSGD */
/** Rank 2 Read DQS Gating Delay: Rank 2 delay select for the read DQS gating
 * (DQSG) LCDL.
 */
#define DWC_DDR_PHY_REGS_DXNLCDLR2_R2DQSGD_MASK		0x00ff0000
#define DWC_DDR_PHY_REGS_DXNLCDLR2_R2DQSGD_SHIFT	16

/* Register DXnLCDLR2 field R3DQSGD */
/** Rank 3 Read DQS Gating Delay: Rank 3 delay select for the read DQS gating
 * (DQSG) LCDL.
 */
#define DWC_DDR_PHY_REGS_DXNLCDLR2_R3DQSGD_MASK		0xff000000
#define DWC_DDR_PHY_REGS_DXNLCDLR2_R3DQSGD_SHIFT	24

/* Register DXnMDLR field IPRD */
/** Initial Period: Initial period measured by the master delay line
 * calibration for VT drift compensation.
 * This value is used as the denominator when calculating the ratios of updates
 * during VT compensation.
 */
#define DWC_DDR_PHY_REGS_DXNMDLR_IPRD_MASK		0x000000ff
#define DWC_DDR_PHY_REGS_DXNMDLR_IPRD_SHIFT		0

/* Register DXnMDLR field TPRD */
/** Target Period: Target period measured by the master delay line calibration
 * for VT drift compensation.  This is the current measured value of the period
 * and is continuously updated if the MDL is enabled to do so.
 */
#define DWC_DDR_PHY_REGS_DXNMDLR_TPRD_MASK		0x0000ff00
#define DWC_DDR_PHY_REGS_DXNMDLR_TPRD_SHIFT		8

/* Register DXnGTR field RnDGSL */
#define DWC_DDR_PHY_REGS_DXNGTR_RNDGSL_MASK		0x00000007
#define DWC_DDR_PHY_REGS_DXNGTR_RNDGSL_SHIFT		0
#define DWC_DDR_PHY_REGS_DXNGTR_RNDGSL_NUM_BITS		3

/* Register DXnGTR field R0DGSL */
/** Rank n DQS Gating System Latency: This is used to increase the number of clock
 * cycles needed to expect valid DDR read data by up to seven extra clock cycles.
 * This is used to compensate for board delays and other system delays. Power-up
 * default is 000 (i.e. no extra clock cycles required). The SL fields are initially set by
 * the PUBm2 during automatic DQS data training but these values can be
 * overwritten by a direct write to this register. Every three bits of this register control
 * the latency of each of the (up to) four ranks. R0DGSL controls the latency of rank
 * 0, R1DGSL controls rank 1, and so on. Valid values are 0 to 7:
 */
#define DWC_DDR_PHY_REGS_DXNGTR_R0DGSL_MASK		0x00000007
#define DWC_DDR_PHY_REGS_DXNGTR_R0DGSL_SHIFT		0

/* Register DXnGTR field R1DGSL */
/** Rank n DQS Gating System Latency: This is used to increase the number of clock
 * cycles needed to expect valid DDR read data by up to seven extra clock cycles.
 * This is used to compensate for board delays and other system delays. Power-up
 * default is 000 (i.e. no extra clock cycles required). The SL fields are initially set by
 * the PUBm2 during automatic DQS data training but these values can be
 * overwritten by a direct write to this register. Every three bits of this register control
 * the latency of each of the (up to) four ranks. R0DGSL controls the latency of rank
 * 0, R1DGSL controls rank 1, and so on. Valid values are 0 to 7:
 */
#define DWC_DDR_PHY_REGS_DXNGTR_R1DGSL_MASK		0x00000038
#define DWC_DDR_PHY_REGS_DXNGTR_R1DGSL_SHIFT		3

/* Register DXnGTR field R2DGSL */
/** Rank n DQS Gating System Latency: This is used to increase the number of clock
 * cycles needed to expect valid DDR read data by up to seven extra clock cycles.
 * This is used to compensate for board delays and other system delays. Power-up
 * default is 000 (i.e. no extra clock cycles required). The SL fields are initially set by
 * the PUBm2 during automatic DQS data training but these values can be
 * overwritten by a direct write to this register. Every three bits of this register control
 * the latency of each of the (up to) four ranks. R0DGSL controls the latency of rank
 * 0, R1DGSL controls rank 1, and so on. Valid values are 0 to 7:
 */
#define DWC_DDR_PHY_REGS_DXNGTR_R2DGSL_MASK		0x000001c0
#define DWC_DDR_PHY_REGS_DXNGTR_R2DGSL_SHIFT		6

/* Register DXnGTR field R3DGSL */
/** Rank n DQS Gating System Latency: This is used to increase the number of clock
 * cycles needed to expect valid DDR read data by up to seven extra clock cycles.
 * This is used to compensate for board delays and other system delays. Power-up
 * default is 000 (i.e. no extra clock cycles required). The SL fields are initially set by
 * the PUBm2 during automatic DQS data training but these values can be
 * overwritten by a direct write to this register. Every three bits of this register control
 * the latency of each of the (up to) four ranks. R0DGSL controls the latency of rank
 * 0, R1DGSL controls rank 1, and so on. Valid values are 0 to 7:
 */
#define DWC_DDR_PHY_REGS_DXNGTR_R3DGSL_MASK		0x00000e00
#define DWC_DDR_PHY_REGS_DXNGTR_R3DGSL_SHIFT		9

/* Register DXnGTR field RnWLSL */
#define DWC_DDR_PHY_REGS_DXNGTR_RNWLSL_MASK		0x00003000
#define DWC_DDR_PHY_REGS_DXNGTR_RNWLSL_SHIFT		12
#define DWC_DDR_PHY_REGS_DXNGTR_RNWLSL_NUM_BITS		2
#define DWC_DDR_PHY_REGS_DXNGTR_RNWLSL_VAL_WL_M1	0
#define DWC_DDR_PHY_REGS_DXNGTR_RNWLSL_VAL_WL		1
#define DWC_DDR_PHY_REGS_DXNGTR_RNWLSL_VAL_WL_P1	2

/* Register DXnGTR field R0WLSL */
/** Rank n Write Leveling System Latency: This is used to adjust the write latency
 * after write leveling. Power-up default is 01 (i.e. no extra clock cycles required). The
 * SL fields are initially set by the PUBm2 during automatic write leveling but these
 * values can be overwritten by a direct write to this register. Every two bits of this
 * register control the latency of each of the (up to) four ranks. R0WLSL controls the
 * latency of rank 0, R1WLSL controls rank 1, and so on. Valid values:
 * 00 = Write latency = WL - 1
 * 01 = Write latency = WL
 * 10 = Write latency = WL + 1
 * 11 = Reserved
 */
#define DWC_DDR_PHY_REGS_DXNGTR_R0WLSL_MASK		0x00003000
#define DWC_DDR_PHY_REGS_DXNGTR_R0WLSL_SHIFT		12

/* Register DXnGTR field R1WLSL */
/** Rank n Write Leveling System Latency: This is used to adjust the write latency
 * after write leveling. Power-up default is 01 (i.e. no extra clock cycles required). The
 * SL fields are initially set by the PUBm2 during automatic write leveling but these
 * values can be overwritten by a direct write to this register. Every two bits of this
 * register control the latency of each of the (up to) four ranks. R0WLSL controls the
 * latency of rank 0, R1WLSL controls rank 1, and so on. Valid values:
 * 00 = Write latency = WL - 1
 * 01 = Write latency = WL
 * 10 = Write latency = WL + 1
 * 11 = Reserved
 */
#define DWC_DDR_PHY_REGS_DXNGTR_R1WLSL_MASK		0x0000c000
#define DWC_DDR_PHY_REGS_DXNGTR_R1WLSL_SHIFT		14

/* Register DXnGTR field R2WLSL */
/** Rank n Write Leveling System Latency: This is used to adjust the write latency
 * after write leveling. Power-up default is 01 (i.e. no extra clock cycles required). The
 * SL fields are initially set by the PUBm2 during automatic write leveling but these
 * values can be overwritten by a direct write to this register. Every two bits of this
 * register control the latency of each of the (up to) four ranks. R0WLSL controls the
 * latency of rank 0, R1WLSL controls rank 1, and so on. Valid values:
 * 00 = Write latency = WL - 1
 * 01 = Write latency = WL
 * 10 = Write latency = WL + 1
 * 11 = Reserved
 */
#define DWC_DDR_PHY_REGS_DXNGTR_R2WLSL_MASK		0x00030000
#define DWC_DDR_PHY_REGS_DXNGTR_R2WLSL_SHIFT		16

/* Register DXnGTR field R3WLSL */
/** Rank n Write Leveling System Latency: This is used to adjust the write latency
 * after write leveling. Power-up default is 01 (i.e. no extra clock cycles required). The
 * SL fields are initially set by the PUBm2 during automatic write leveling but these
 * values can be overwritten by a direct write to this register. Every two bits of this
 * register control the latency of each of the (up to) four ranks. R0WLSL controls the
 * latency of rank 0, R1WLSL controls rank 1, and so on. Valid values:
 * 00 = Write latency = WL - 1
 * 01 = Write latency = WL
 * 10 = Write latency = WL + 1
 * 11 = Reserved
 */
#define DWC_DDR_PHY_REGS_DXNGTR_R3WLSL_MASK		0x000c0000
#define DWC_DDR_PHY_REGS_DXNGTR_R3WLSL_SHIFT		18

#ifdef __cplusplus
}
#endif

#endif

/** @} end of DDR group */

