/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

/**
 *  @{
 * @file   al_hal_serdes_snps_quad_regs.h
 *
 * @brief serdes_snps_quad registers
 *
 */

#ifndef __AL_HAL_SERDES_SNPS_QUAD_REGS_H__
#define __AL_HAL_SERDES_SNPS_QUAD_REGS_H__

#include "al_hal_plat_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Unit Registers
 */

struct al_serdes_snps_quad_gen {
	/* [0x0] SERDES registers Version */
	uint32_t version;
};

struct al_serdes_snps_quad_phy_cr {
	/* [0x0] Address and Command */
	uint32_t para;
	/* [0x4] Read Write Data */
	uint32_t para_data;
};

struct al_serdes_snps_quad_glb {
	/* [0x0] */
	uint32_t config;
	/* [0x4] */
	uint32_t clk;
	/* [0x8] */
	uint32_t term;
	/* [0xc] */
	uint32_t dfx;
};

struct al_serdes_snps_quad_mpllx {
	/* [0x0] */
	uint32_t ctrl;
	/* [0x4] */
	uint32_t bw;
	/* [0x8] */
	uint32_t misc2;
	/* [0xc] */
	uint32_t misc3;
};

struct al_serdes_snps_quad_override {
	/* [0x0] */
	uint32_t rx_gx_1;
	/* [0x4] */
	uint32_t rx_gx_2;
	/* [0x8] */
	uint32_t rx_vco_g1_g2;
	/* [0xc] */
	uint32_t rx_vco_g3_g4;
	/* [0x10] */
	uint32_t sup_misc_gx;
	/* [0x14] ` */
	uint32_t gernal_misc;
	uint32_t rsrvd[2];
};

struct al_serdes_snps_quad_override_lanex {
	/* [0x0] */
	uint32_t g1_rx_eq;
	/* [0x4] */
	uint32_t g1_tx_eq;
	/* [0x8] */
	uint32_t g1_misc;
	uint32_t rsrvd_0;
	/* [0x10] */
	uint32_t g2_rx_eq;
	/* [0x14] */
	uint32_t g2_tx_eq;
	/* [0x18] */
	uint32_t g2_misc;
	uint32_t rsrvd_1;
	/* [0x20] */
	uint32_t g3_rx_eq;
	/* [0x24] */
	uint32_t g3_tx_eq;
	/* [0x28] */
	uint32_t g3_misc;
	uint32_t rsrvd_2;
	/* [0x30] */
	uint32_t g4_rx_eq;
	/* [0x34] */
	uint32_t g4_tx_eq;
	/* [0x38] */
	uint32_t g4_misc;
	/* [0x3c] */
	uint32_t misc;
};

struct al_serdes_snps_quad_regs {
	/* [0x0] 4 groups of interrupts */
	uint32_t int_ctrl_mem[32];
	/* [0x80] */
	uint32_t rsrvd_0[32];
	/* [0x100] */
	struct al_serdes_snps_quad_gen gen;
	/* [0x104] */
	uint32_t rsrvd_1[7];
	/* [0x120] */
	struct al_serdes_snps_quad_phy_cr phy_cr;
	/* [0x128] */
	uint32_t rsrvd_2[6];
	/* [0x140] */
	struct al_serdes_snps_quad_glb glb;
	/* [0x150] */
	struct al_serdes_snps_quad_mpllx mpllx[2];
	/* [0x170] */
	struct al_serdes_snps_quad_override override;
	/* [0x190] */
	uint32_t rsrvd_3[28];
	/* [0x200] */
	struct al_serdes_snps_quad_override_lanex override_lanex[4];
};


/*
 * Registers Fields
 */

/**** version register ****/
/*  Revision number (Minor) */
#define SERDES_SNPS_QUAD_GEN_VERSION_RELEASE_NUM_MINOR_MASK 0x000000FF
#define SERDES_SNPS_QUAD_GEN_VERSION_RELEASE_NUM_MINOR_SHIFT 0
/*  Revision number (Major) */
#define SERDES_SNPS_QUAD_GEN_VERSION_RELEASE_NUM_MAJOR_MASK 0x0000FF00
#define SERDES_SNPS_QUAD_GEN_VERSION_RELEASE_NUM_MAJOR_SHIFT 8
/*  date of release */
#define SERDES_SNPS_QUAD_GEN_VERSION_DATE_DAY_MASK 0x001F0000
#define SERDES_SNPS_QUAD_GEN_VERSION_DATE_DAY_SHIFT 16
/*  month of release */
#define SERDES_SNPS_QUAD_GEN_VERSION_DATA_MONTH_MASK 0x01E00000
#define SERDES_SNPS_QUAD_GEN_VERSION_DATA_MONTH_SHIFT 21
/*  year of release (starting from 2000) */
#define SERDES_SNPS_QUAD_GEN_VERSION_DATE_YEAR_MASK 0x3E000000
#define SERDES_SNPS_QUAD_GEN_VERSION_DATE_YEAR_SHIFT 25
/*  Reserved */
#define SERDES_SNPS_QUAD_GEN_VERSION_RESERVED_MASK 0xC0000000
#define SERDES_SNPS_QUAD_GEN_VERSION_RESERVED_SHIFT 30

/**** para register ****/
/* address */
#define SERDES_SNPS_QUAD_PHY_CR_PARA_ADDR_MASK 0x0000FFFF
#define SERDES_SNPS_QUAD_PHY_CR_PARA_ADDR_SHIFT 0
/* when set use the interface for read access */
#define SERDES_SNPS_QUAD_PHY_CR_PARA_RD_EN (1 << 29)
/* when set use the interface for write access */
#define SERDES_SNPS_QUAD_PHY_CR_PARA_WR_EN (1 << 30)
/* when set data is valid for read */
#define SERDES_SNPS_QUAD_PHY_CR_PARA_ACK (1 << 31)

/**** para_data register ****/
/* write data */
#define SERDES_SNPS_QUAD_PHY_CR_PARA_DATA_WRITE_MASK 0x0000FFFF
#define SERDES_SNPS_QUAD_PHY_CR_PARA_DATA_WRITE_SHIFT 0
/* read data */
#define SERDES_SNPS_QUAD_PHY_CR_PARA_DATA_READ_MASK 0xFFFF0000
#define SERDES_SNPS_QUAD_PHY_CR_PARA_DATA_READ_SHIFT 16

/**** config register ****/
/* SW reset for the PHY */
#define SERDES_SNPS_QUAD_GLB_CONFIG_PHY_RESET (1 << 0)
/* Set for overriding PHY defualt values  */
#define SERDES_SNPS_QUAD_GLB_CONFIG_PHY_EXT_CTRL_SEL (1 << 1)
/* Resistor Tune Acknowledge  /Function: Indicates that a resistor tune has completed */
#define SERDES_SNPS_QUAD_GLB_CONFIG_PHY_RTUNE_ACK (1 << 2)
/*
 * Resistor Tune Request /Function: Assertion triggers a resistor tune request (if one is not
 * already in progress)
 */
#define SERDES_SNPS_QUAD_GLB_CONFIG_PHY_RTUNE_REQ (1 << 3)
/*
 * SRAM external load done / Function: Signal asserted by user after any updates to the SRAM have
 * been
 *  loaded
 */
#define SERDES_SNPS_QUAD_GLB_CONFIG_SRAM_EXT_LD_DONE (1 << 4)
/*
 * SRAM Initialization done/ Function: Signal indicating that the SRAM has been initialized by the
 * boot loader in the Raw PCS This signal does not assert if sram_bypass is asserted.
 */
#define SERDES_SNPS_QUAD_GLB_CONFIG_SRAM_INIT_DONE (1 << 5)
/*
 * SRAM bypass/ Function: Control signal when asserted, bypasses the SRAM interface. In this case,
 * the adaptation and calibration algorithms are executed from the hard wired values within the Raw
 * PCS. If SRAM is not bypassed, the internal algorithms are first loaded by Raw PCS into the SRAM
 * at which point user
 * can change the contents of the SRAM. The updated SRAM contents are
 * used for the adaptation and calibration routines.
 * This signal is meant to be used only for debugging purposes and must not
 * change after phy_reset is negated.
 */
#define SERDES_SNPS_QUAD_GLB_CONFIG_SRAM_BYPASS (1 << 6)
/*
 * VPH Nominal Selection /Function: Indicates the VPH voltage level supplied:
 * - 2'b00: Reserved
 * - 2'b01: Reserved
 * - 2'b10: 1.5 V
 * - 2'b11: 1.8 V
 */
#define SERDES_SNPS_QUAD_GLB_CONFIG_NOMINAL_VPH_SEL_MASK 0x00000180
#define SERDES_SNPS_QUAD_GLB_CONFIG_NOMINAL_VPH_SEL_SHIFT 7
/*
 * VP Nominal Selection/ Function: Indicates the VP voltage level supplied:
 * - 2'b00: Reserved
 * - 2'b01: Reserved
 * - 2'b10: 0.75 V
 * - 2'b11: 0.85 V
 */
#define SERDES_SNPS_QUAD_GLB_CONFIG_NOMINAL_VP_SEL_MASK 0x00000600
#define SERDES_SNPS_QUAD_GLB_CONFIG_NOMINAL_VP_SEL_SHIFT 9
/*
 * PCS PIPE configuration /Function: When upcs_pipe_config[0] is set to 1, the PCS ignores lane-off
 * via PIPE specification method (TxElecIdle = 1 and TxCompliance = 1) and responds to
 * power-down/rate/width changes. Otherwise, until the MAC
 * deasserts the "turned off” signaling, the PCS ignores any commands to change
 * power-down/rate/width after being turned off. Note: Any change to this input must be followed by
 * phy_reset assertion.
 */
#define SERDES_SNPS_QUAD_GLB_CONFIG_UPCS_PIPE_MASK 0xFFFF0000
#define SERDES_SNPS_QUAD_GLB_CONFIG_UPCS_PIPE_SHIFT 16

/**** clk register ****/
/*
 * Spread Spectrum Enable/ Function: Enables spread-spectrum clock (SSC) generation on the MPLL
 * clocks outputs (mpll{a,b}_word_clk, mpll{a,b}_div_clk). If the reference clock already has spread
 * spectrum applied, mpll{a,b}_ssc_en must be deasserted. These inputs can be changed only when the
 * txX_mpll_en input for all lanes is de-asserted Note:
 * - Use of built-in SSC generation results in increased deterministic jitter (DJ).
 * - The SSC profile is not synchronized across aggregated PHYs.
 */
#define SERDES_SNPS_QUAD_GLB_CLK_MPLLA_SSC_EN (1 << 0)
/*
 * MPLL{A,B} Force Enable /Function: When asserted, the corresponding MPLL is forced to be powered
 * up, irrespective of the txX_mpll_en input. This input is used for applications where a
 * free-running MPLL clock output is required. The mpll{a,b}_force_en signal must be kept
 * de-asserted from the negation of phy_reset until the txX_ack/rxX_ack signals de-assert. If MPLL
 * is not powered up, Synopsys recommends that you follow the txX_mpll_en controls as shown in
 * Figure 5-7 on page 101 ("MPLLA Configuration Settings Update Followed By a TX Power-State
 * Change”).
 */
#define SERDES_SNPS_QUAD_GLB_CLK_MPLLA_FORCE_EN (1 << 1)
/*
 * MPLL{A,B} Force Acknowledge / Function: Acknowledge output for mpll{a,b}_force_en. This output
 * asserts when the MPLL is powered up in response to mpll{a,b}_force_en assertion. This output
 * de-asserts as a response to mpll{a,b}_force_en de-assertion, and at that point the MPLL is
 * powered down if txX_mpll_en of other lanes is de-asserted. Note: A four-way level handshake must
 * be followed between mpll{a,b}_force_en and mpll{a,b}_force_ack.
 */
#define SERDES_SNPS_QUAD_GLB_CLK_MPLLA_FORCE_ACK (1 << 2)
/*
 * MPLL{A,B} State Indicator Function: Indicates the state of MPLL{A,B}. This signal is asserted
 * when MPLL{A,B} is powered up and phase-locked.
 */
#define SERDES_SNPS_QUAD_GLB_CLK_MPLLA_STATE (1 << 3)
/*
 * MPLL{A,B} Re-calibration Bank Select/ Function: Selects the bank to store the MPLL calibration
 * value Note: This input can be changed only when the MPLL is powered down.
 */
#define SERDES_SNPS_QUAD_GLB_CLK_MPLLA_RECAL_BANK_SEL_MASK 0x00000030
#define SERDES_SNPS_QUAD_GLB_CLK_MPLLA_RECAL_BANK_SEL_SHIFT 4
/*
 * Spread Spectrum Enable/ Function: Enables spread-spectrum clock (SSC) generation on the MPLL
 * clocks outputs (mpll{a,b}_word_clk, mpll{a,b}_div_clk). If the reference clock already has spread
 * spectrum applied, mpll{a,b}_ssc_en must be deasserted. These inputs can be changed only when the
 * txX_mpll_en input for all lanes is de-asserted Note:
 * - Use of built-in SSC generation results in increased deterministic jitter (DJ).
 * - The SSC profile is not synchronized across aggregated PHYs.
 */
#define SERDES_SNPS_QUAD_GLB_CLK_MPLLB_SSC_EN (1 << 6)
/*
 * MPLL{A,B} Force Enable /Function: When asserted, the corresponding MPLL is forced to be powered
 * up, irrespective of the txX_mpll_en input. This input is used for applications where a
 * free-running MPLL clock output is required. The mpll{a,b}_force_en signal must be kept
 * de-asserted from the negation of phy_reset until the txX_ack/rxX_ack signals de-assert. If MPLL
 * is not powered up, Synopsys recommends that you follow the txX_mpll_en controls as shown in
 * Figure 5-7 on page 101 ("MPLLA Configuration Settings Update Followed By a TX Power-State
 * Change”).
 */
#define SERDES_SNPS_QUAD_GLB_CLK_MPLLB_FORCE_EN (1 << 7)
/*
 * MPLL{A,B} Force Acknowledge / Function: Acknowledge output for mpll{a,b}_force_en. This output
 * asserts when the MPLL is powered up in response to mpll{a,b}_force_en assertion. This output
 * de-asserts as a response to mpll{a,b}_force_en de-assertion, and at that point the MPLL is
 * powered down if txX_mpll_en of other lanes is de-asserted. Note: A four-way level handshake must
 * be followed between mpll{a,b}_force_en and mpll{a,b}_force_ack.
 */
#define SERDES_SNPS_QUAD_GLB_CLK_MPLLB_FORCE_ACK (1 << 8)
/*
 * MPLL{A,B} State Indicator Function: Indicates the state of MPLL{A,B}. This signal is asserted
 * when MPLL{A,B} is powered up and phase-locked.
 */
#define SERDES_SNPS_QUAD_GLB_CLK_MPLLB_STATE (1 << 9)
/*
 * MPLL{A,B} Re-calibration Bank Select/ Function: Selects the bank to store the MPLL calibration
 * value Note: This input can be changed only when the MPLL is powered down.
 */
#define SERDES_SNPS_QUAD_GLB_CLK_MBLLB_RECAL_BANK_SEL_MASK 0x00000C00
#define SERDES_SNPS_QUAD_GLB_CLK_MBLLB_RECAL_BANK_SEL_SHIFT 10
/*
 * Repeat Reference Clock Enable / Function: Enables the CML output clocks ref_repeat_clk_{p,m}.
 * This pair of clocks can be used as reference clocks for other on-chip PHYs
 */
#define SERDES_SNPS_QUAD_GLB_CLK_PHY_REF_REPEAT_CLK_EN (1 << 12)
/*
 * Select Reference Clock Connected to ref_pad_clk_p/ref_pad_clk_m Function: Selects the external
 * ref_pad_clk_p and ref_pad_clk_m inputs as the reference clock source when asserted. When
 * de-asserted, ref_alt_clk_p and ref_alt_clk_m are the sources of the eference clock. Any change in
 * this input must be followed by phy_reset assertion. Note: A transition on this input must be
 * followed by the assertion of phy_reset
 */
#define SERDES_SNPS_QUAD_GLB_CLK_PHY_REF_USE_PAD (1 << 13)
/*
 * Reference Clock Detection result /Function: Indicates the presence of the reference clock on the
 * pads input The result is only valid when ref_clkdet_en is asserted.
 */
#define SERDES_SNPS_QUAD_GLB_CLK_REF_CLKDET_EN (1 << 14)
/*
 * Enable Reference Clock Detection /Function: Enables detection of the reference clock on the pads
 * input
 */
#define SERDES_SNPS_QUAD_GLB_CLK_REF_CLKDET_RESULT (1 << 15)
/* ?????? Don't find that one at all */
#define SERDES_SNPS_QUAD_GLB_CLK_TX_REF_CLK_EN (1 << 16)
/*
 * Input Reference Clock Divider Control/ Function: The reference clock is divided by 2 when
 * asserted. Any change in this input must be followed by phy_reset assertion.
 */
#define SERDES_SNPS_QUAD_GLB_CLK_REF_CLK_DIV2_EN (1 << 17)

/**** term register ****/
/*
 * Offset for TX Down Termination /Function: Specifies an additional fixed offset to calibrated TX
 * down termination value. This is a signed input with 2's complement encoding.
 */
#define SERDES_SNPS_QUAD_GLB_TERM_PHY_TXDN_TERM_OFFSET_MASK 0x000001FF
#define SERDES_SNPS_QUAD_GLB_TERM_PHY_TXDN_TERM_OFFSET_SHIFT 0
/*
 * Offset for TX Up Termination/ Function: Specifies an additional fixed offset to calibrated TX up
 * termination value. This is a signed input with 2's complement encoding.
 */
#define SERDES_SNPS_QUAD_GLB_TERM_PHY_TXUP_TERM_OFFSET_MASK 0x01FF0000
#define SERDES_SNPS_QUAD_GLB_TERM_PHY_TXUP_TERM_OFFSET_SHIFT 16
/*
 * Offset for RX Termination /Function: Specifies an additional fixed offset to calibrated RX
 * termination value. This is a signed input with 2's complement encoding.
 */
#define SERDES_SNPS_QUAD_GLB_TERM_PHY_RX_TERM_OFFSET_MASK 0x3E000000
#define SERDES_SNPS_QUAD_GLB_TERM_PHY_RX_TERM_OFFSET_SHIFT 25

/**** dfx register ****/
/*
 * Enable TX ref_clk Test Mode /Function: TX ref_clk test mode sends the reference clock to the TX
 * for outputting through the TX driver.
 */
#define SERDES_SNPS_QUAD_GLB_DFX_TEST_TX_REF_CLK_EN (1 << 0)
/*
 * Digital Test Bus Output /Function: Used in debug mode for probing the values of various internal
 * signals in the PHY
 */
#define SERDES_SNPS_QUAD_GLB_DFX_PHY_DTB_OUT_MASK 0x00000006
#define SERDES_SNPS_QUAD_GLB_DFX_PHY_DTB_OUT_SHIFT 1
/*
 * TX Boundary Scan Low Swing/ Function: Enables TX JTAG low-swing mode (otherwise, it is in
 * full-swing mode)
 */
#define SERDES_SNPS_QUAD_GLB_DFX_BS_TX_LOWSWING (1 << 3)
/*
 * RX Biasing Current Control / Function: Sets the RX biasing current for RX analog front end. The
 * recommended default setting for this input is 5'b10001.
 */
#define SERDES_SNPS_QUAD_GLB_DFX_BS_RX_BIGSWING (1 << 4)
/*
 * ACJTAG Receiver Sensitivity Level Control/ Function: Sets the internal thresholds for the ACJTAG
 * receiver. The ACJTAG threshold level should be set to trigger for the expected amplitude for the
 * given protocol. Threshold levels are provided in Table 5-20 on page 133 ("ACJTAG - Low Swing Mode
 * Trip Levels") and Table 5-21 on page 133 ("ACJTAG - Big Swing Mode Trip Levels”).
 */
#define SERDES_SNPS_QUAD_GLB_DFX_BS_RX_LEVEL_MASK 0x00001F00
#define SERDES_SNPS_QUAD_GLB_DFX_BS_RX_LEVEL_SHIFT 8

/**** ctrl register ****/
/*
 * MPLL{A,B} Divide Clock Enable /Function: When asserted, the frequency of the mpll{a,b}_div_clk
 * output clock is the MPLL{A,B} frequency divided by mpll{a,b}_div_multiplier[7:0]. Fractional
 * division and/or SSC is additionally applied to mpll{a,b}_div_clk depending on the settings of
 * mpll{a,b}_ssc_en and mpll{a,b}_frac_en. These inputs can only be changed when the respective
 * mpll{a,b}_ssc_en and txX_mpll_en inputs are de-asserted.
 */
#define SERDES_SNPS_QUAD_MPLLX_CTRL_DIV_CLK_EN (1 << 0)
/*
 * MPLL{A,B} DIV Clock Output Frequency Divider Control/ Function: Additional frequency division
 * factor used to generate mpll{a,b}_div_clk clock output from the MPLL{A,B} VCO. This signal
 * controls whether the MPLL VCO is divided by 2 (when pll{a,b}_fb_clk_div4_en is deasserted) or 4
 * (when mpll{a,b}_fb_clk_div4_en is asserted) before being divided down by
 * mpll{a,b}_div_multiplier[7:0]. This input can be changed only when the txX_mpll_en input for all
 * lanes is de-asserted.
 */
#define SERDES_SNPS_QUAD_MPLLX_CTRL_FB_CLK_DIV4_EN (1 << 1)
/*
 * MPLL{A,B} Divide by 16.5 Enable /Function: Enables the output clocks derived from MPLL{A,B} based
 * on 16.5, 33 and 66 division ratios. This input can be changed only when the txX_mpll_en inputs
 * for all lanes are de-asserted.
 */
#define SERDES_SNPS_QUAD_MPLLX_CTRL_DIV16P5_CLK_EN (1 << 2)
/* missing desc! */
#define SERDES_SNPS_QUAD_MPLLX_CTRL_BW_THRESHOLD_MASK 0x00000018
#define SERDES_SNPS_QUAD_MPLLX_CTRL_BW_THRESHOLD_SHIFT 3
/*
 * MPLL{A,B} Frequency Multiplier Control/ Function: Multiplies the reference clock to a frequency
 * suitable for intended operating speed. The reference clocks used by the MPLLs are generated after
 * applying ref_clk_mplla_div[2:0]/ref_clk_mpllb_div[2:0] divisions on the
 * ref_pad_clk_p/ref_pad_clk_m or ref_alt_clk_p/ref_alt_clk_m. Note:
 * - These inputs can be changed only when phy_reset is asserted.
 * - These inputs have 2 fractional bits and 10 integer bits, so the intended
 * multiplication factor must be multiplied by 4. As a result, for a multiplier value of 125, the
 * bus should be set to 12’d500.
 */
#define SERDES_SNPS_QUAD_MPLLX_CTRL_DIV_MULTIPLIER_MASK 0xFF000000
#define SERDES_SNPS_QUAD_MPLLX_CTRL_DIV_MULTIPLIER_SHIFT 24

/**** bw register ****/
/*
 * MPLL{A,B} Bandwidth Control/ Function: Bandwidth control for MPLLA and MPLLB, respectively. Set
 * this input as per the tables provided in Chapter 5, "PHY Usage and Configuration”, for all
 * protocols.
 */
#define SERDES_SNPS_QUAD_MPLLX_BW_LOW_MASK 0x0000FFFF
#define SERDES_SNPS_QUAD_MPLLX_BW_LOW_SHIFT 0
/*
 * MPLL{A,B} Bandwidth Control/ Function: Bandwidth control for MPLLA and MPLLB, respectively. Set
 * this input as per the tables provided in Chapter 5, "PHY Usage and Configuration”, for all
 * protocols.
 */
#define SERDES_SNPS_QUAD_MPLLX_BW_HIGH_MASK 0xFFFF0000
#define SERDES_SNPS_QUAD_MPLLX_BW_HIGH_SHIFT 16

/**** misc2 register ****/
/*
 * Spread Spectrum Peak Value /Function: Controls the ppm shift amplitude of the spread-spectrum
 * profile.
 */
#define SERDES_SNPS_QUAD_MPLLX_MISC2_SSC_PEAK_MASK 0x000FFFFF
#define SERDES_SNPS_QUAD_MPLLX_MISC2_SSC_PEAK_SHIFT 0
/*
 * MPLL{A,B} Frequency Multiplier Control/ Function: Multiplies the reference clock to a frequency
 * suitable for intended operating speed. The reference clocks used by the MPLLs are generated after
 * applying ref_clk_mplla_div[2:0]/ref_clk_mpllb_div[2:0] divisions on the
 * ref_pad_clk_p/ref_pad_clk_m or ref_alt_clk_p/ref_alt_clk_m. Note:
 * - These inputs can be changed only when phy_reset is asserted.
 * - These inputs have 2 fractional bits and 10 integer bits, so the intended multiplication factor
 * must be multiplied by 4. As a result, for a multiplier value of 125, the bus should be set to
 * 12’d500.
 */
#define SERDES_SNPS_QUAD_MPLLX_MISC2_MULTIPLIER_MASK 0xFFF00000
#define SERDES_SNPS_QUAD_MPLLX_MISC2_MULTIPLIER_SHIFT 20

/**** misc3 register ****/
/* Spread Spectrum Step Size Value/ Function: Controls the spread-spectrum profile step. */
#define SERDES_SNPS_QUAD_MPLLX_MISC3_SSC_STEP_SIZE_MASK 0x001FFFFF
#define SERDES_SNPS_QUAD_MPLLX_MISC3_SSC_STEP_SIZE_SHIFT 0
/*
 * MPLL{A,B} SSC Up Spread Enable/ Function: When asserted, the SSC is applied in upwards direction
 * (positive ppm) and when deasserted the SSC is applied downwards (negative ppm). Note: This input
 * can be changed only when the MPLL is powered down.
 */
#define SERDES_SNPS_QUAD_MPLLX_MISC3_SSC_UP_SPREAD (1 << 21)
/*
 * MPLL{A,B} Word Clock Divider Control / Function: Controls the MPLL{A,B} word clock dividers for
 * generating the mpll{a,b}_word_clk output. These inputs can be changed only when the txX_mpll_en
 * input is deasserted.
 * - 2’b00: Divide by 4
 * - 2’b01: Divide by 5
 * - 2’b10: Divide by 8
 * - 2’b11: Divide by 10
 */
#define SERDES_SNPS_QUAD_MPLLX_MISC3_WORD_CLK_DIV_MASK 0x00C00000
#define SERDES_SNPS_QUAD_MPLLX_MISC3_WORD_CLK_DIV_SHIFT 22
/*
 * MPLL{A,B} Word Clock Divider Control Function: Controls the MPLL{A,B} word clock dividers for
 * generating the mpll{a,b}_word_clk output. These inputs can be changed only when the txX_mpll_en
 * input is deasserted.
 * - 2’b00: Divide by 4
 * - 2’b01: Divide by 5
 * - 2’b10: Divide by 8
 * - 2’b11: Divide by 10
 */
#define SERDES_SNPS_QUAD_MPLLX_MISC3_TX_CLK_DIV_MASK 0x07000000
#define SERDES_SNPS_QUAD_MPLLX_MISC3_TX_CLK_DIV_SHIFT 24

/**** rx_gx_1 register ****/

#define SERDES_SNPS_QUAD_OVERRIDE_RX_GX_1_EQ_AFE_RATE_G1_MASK 0x00000007
#define SERDES_SNPS_QUAD_OVERRIDE_RX_GX_1_EQ_AFE_RATE_G1_SHIFT 0

#define SERDES_SNPS_QUAD_OVERRIDE_RX_GX_1_EQ_AFE_RATE_G2_MASK 0x00000070
#define SERDES_SNPS_QUAD_OVERRIDE_RX_GX_1_EQ_AFE_RATE_G2_SHIFT 4

#define SERDES_SNPS_QUAD_OVERRIDE_RX_GX_1_EQ_AFE_RATE_G3_MASK 0x00000700
#define SERDES_SNPS_QUAD_OVERRIDE_RX_GX_1_EQ_AFE_RATE_G3_SHIFT 8

#define SERDES_SNPS_QUAD_OVERRIDE_RX_GX_1_EQ_AFE_RATE_G4_MASK 0x00007000
#define SERDES_SNPS_QUAD_OVERRIDE_RX_GX_1_EQ_AFE_RATE_G4_SHIFT 12

#define SERDES_SNPS_QUAD_OVERRIDE_RX_GX_1_SIGDET_LF_THRESHOLD_G1_MASK 0x00070000
#define SERDES_SNPS_QUAD_OVERRIDE_RX_GX_1_SIGDET_LF_THRESHOLD_G1_SHIFT 16

#define SERDES_SNPS_QUAD_OVERRIDE_RX_GX_1_SIGDET_LF_THRESHOLD_G2_MASK 0x00700000
#define SERDES_SNPS_QUAD_OVERRIDE_RX_GX_1_SIGDET_LF_THRESHOLD_G2_SHIFT 20

#define SERDES_SNPS_QUAD_OVERRIDE_RX_GX_1_SIGDET_LF_THRESHOLD_G3_MASK 0x07000000
#define SERDES_SNPS_QUAD_OVERRIDE_RX_GX_1_SIGDET_LF_THRESHOLD_G3_SHIFT 24

#define SERDES_SNPS_QUAD_OVERRIDE_RX_GX_1_SIGDET_LF_THRESHOLD_G4_MASK 0x70000000
#define SERDES_SNPS_QUAD_OVERRIDE_RX_GX_1_SIGDET_LF_THRESHOLD_G4_SHIFT 28

/**** rx_gx_2 register ****/

#define SERDES_SNPS_QUAD_OVERRIDE_RX_GX_2_REF_LD_VAL_G1_MASK 0x0000003F
#define SERDES_SNPS_QUAD_OVERRIDE_RX_GX_2_REF_LD_VAL_G1_SHIFT 0

#define SERDES_SNPS_QUAD_OVERRIDE_RX_GX_2_SIGDET_LFPS_FILTER_EN_G1 (1 << 7)

#define SERDES_SNPS_QUAD_OVERRIDE_RX_GX_2_REF_LD_VAL_G2_MASK 0x00003F00
#define SERDES_SNPS_QUAD_OVERRIDE_RX_GX_2_REF_LD_VAL_G2_SHIFT 8

#define SERDES_SNPS_QUAD_OVERRIDE_RX_GX_2_SIGDET_LFPS_FILTER_EN_G2 (1 << 15)

#define SERDES_SNPS_QUAD_OVERRIDE_RX_GX_2_REF_LD_VAL_G3_MASK 0x003F0000
#define SERDES_SNPS_QUAD_OVERRIDE_RX_GX_2_REF_LD_VAL_G3_SHIFT 16

#define SERDES_SNPS_QUAD_OVERRIDE_RX_GX_2_SIGDET_LFPS_FILTER_EN_G3 (1 << 23)

#define SERDES_SNPS_QUAD_OVERRIDE_RX_GX_2_REF_LD_VAL_G4_MASK 0x3F000000
#define SERDES_SNPS_QUAD_OVERRIDE_RX_GX_2_REF_LD_VAL_G4_SHIFT 24

#define SERDES_SNPS_QUAD_OVERRIDE_RX_GX_2_SIGDET_LFPS_FILTER_EN_G4 (1 << 31)

/**** rx_vco_g1_g2 register ****/

#define SERDES_SNPS_QUAD_OVERRIDE_RX_VCO_G1_G2_LD_VAL_G1_MASK 0x00001FFF
#define SERDES_SNPS_QUAD_OVERRIDE_RX_VCO_G1_G2_LD_VAL_G1_SHIFT 0

#define SERDES_SNPS_QUAD_OVERRIDE_RX_VCO_G1_G2_LD_VAL_G2_MASK 0x1FFF0000
#define SERDES_SNPS_QUAD_OVERRIDE_RX_VCO_G1_G2_LD_VAL_G2_SHIFT 16

/**** rx_vco_g3_g4 register ****/

#define SERDES_SNPS_QUAD_OVERRIDE_RX_VCO_G3_G4_LD_VAL_G3_MASK 0x00001FFF
#define SERDES_SNPS_QUAD_OVERRIDE_RX_VCO_G3_G4_LD_VAL_G3_SHIFT 0

#define SERDES_SNPS_QUAD_OVERRIDE_RX_VCO_G3_G4_LD_VAL_G4_MASK 0x1FFF0000
#define SERDES_SNPS_QUAD_OVERRIDE_RX_VCO_G3_G4_LD_VAL_G4_SHIFT 16

/**** sup_misc_gx register ****/
/*
 * Support Miscellaneous Controls
 * Function: This input controls miscellaneous settings in the Support block. This input must be set
 * to the values provided in Chapter 5, "PHY Usage and Configuration”.
 */
#define SERDES_SNPS_QUAD_OVERRIDE_SUP_MISC_GX_G1_MASK 0x000000FF
#define SERDES_SNPS_QUAD_OVERRIDE_SUP_MISC_GX_G1_SHIFT 0
/*
 * Support Miscellaneous Controls
 * Function: This input controls miscellaneous settings in the Support block. This input must be set
 * to the values provided in Chapter 5, "PHY Usage and Configuration”.
 */
#define SERDES_SNPS_QUAD_OVERRIDE_SUP_MISC_GX_G2_MASK 0x0000FF00
#define SERDES_SNPS_QUAD_OVERRIDE_SUP_MISC_GX_G2_SHIFT 8
/*
 * Support Miscellaneous Controls
 * Function: This input controls miscellaneous settings in the Support block. This input must be set
 * to the values provided in Chapter 5, "PHY Usage and Configuration”.
 */
#define SERDES_SNPS_QUAD_OVERRIDE_SUP_MISC_GX_G3_MASK 0x00FF0000
#define SERDES_SNPS_QUAD_OVERRIDE_SUP_MISC_GX_G3_SHIFT 16
/*
 * Support Miscellaneous Controls
 * Function: This input controls miscellaneous settings in the Support block. This input must be set
 * to the values provided in Chapter 5, "PHY Usage and Configuration”.
 */
#define SERDES_SNPS_QUAD_OVERRIDE_SUP_MISC_GX_G4_MASK 0xFF000000
#define SERDES_SNPS_QUAD_OVERRIDE_SUP_MISC_GX_G4_SHIFT 24

/**** gernal_misc register ****/

#define SERDES_SNPS_QUAD_OVERRIDE_GERNAL_MISC_RX_LOS_PWR_UP_CNT_MASK 0x000007FF
#define SERDES_SNPS_QUAD_OVERRIDE_GERNAL_MISC_RX_LOS_PWR_UP_CNT_SHIFT 0

#define SERDES_SNPS_QUAD_OVERRIDE_GERNAL_MISC_RX_VREF_CTRL_MASK 0x0000F800
#define SERDES_SNPS_QUAD_OVERRIDE_GERNAL_MISC_RX_VREF_CTRL_SHIFT 11
/*
 * Input Reference Clock frequency Range/ Function: Specifies the frequency range of the input
 * reference clock (post ref_clk_div2_en division if any). The code mapping of ref_range is as
 * follows:
 * - 3’b000: 20 - 26 MHz
 * - 3’b001: 26.1 - 52 MHz
 * - 3’b010: 52.1 - 78 MHz
 * - 3’b011: 78.1 - 104 MHz
 * - 3’b100: 104.1 - 130 MHz
 * - 3’b101: 130.1 - 156 MHz
 * - 3’b110: 156.1 - 182 MHz
 * - 3’b111: 182.1 - 200 MHz
 * Any change in this input must be followed by phy_reset assertion.
 */
#define SERDES_SNPS_QUAD_OVERRIDE_GERNAL_MISC_REF_RANGE_MASK 0x00070000
#define SERDES_SNPS_QUAD_OVERRIDE_GERNAL_MISC_REF_RANGE_SHIFT 16
/*
 * TX Voltage Boost Maximum Level/ Function: Sets the maximum achievable TX swing. The recommended
 * default setting for this input is 3’b101 (tx_vboost_vref = 0.275 V). Note: This signal does not
 * set the actual TX swing, it only sets the maximum limit.
 */
#define SERDES_SNPS_QUAD_OVERRIDE_GERNAL_MISC_TX_VBOOST_LVL_MASK 0x00380000
#define SERDES_SNPS_QUAD_OVERRIDE_GERNAL_MISC_TX_VBOOST_LVL_SHIFT 19

#define SERDES_SNPS_QUAD_OVERRIDE_GERNAL_MISC_PHY_NOMINAL_VP_SEL_MASK 0x00C00000
#define SERDES_SNPS_QUAD_OVERRIDE_GERNAL_MISC_PHY_NOMINAL_VP_SEL_SHIFT 22

#define SERDES_SNPS_QUAD_OVERRIDE_GERNAL_MISC_PHY_NOMINAL_VPH_SEL_MASK 0x03000000
#define SERDES_SNPS_QUAD_OVERRIDE_GERNAL_MISC_PHY_NOMINAL_VPH_SEL_SHIFT 24

/**** g1_rx_eq register ****/

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G1_RX_EQ_CTLE_BOOST_MASK 0x0000001F
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G1_RX_EQ_CTLE_BOOST_SHIFT 0

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G1_RX_EQ_ATT_LVL_MASK 0x000000E0
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G1_RX_EQ_ATT_LVL_SHIFT 5

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G1_RX_EQ_DFE_TAP1_MASK 0x0000FF00
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G1_RX_EQ_DFE_TAP1_SHIFT 8

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G1_RX_EQ_CTLE_POLE_MASK 0x00030000
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G1_RX_EQ_CTLE_POLE_SHIFT 16

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G1_RX_EQ_VGA1_GAIN_MASK 0x001C0000
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G1_RX_EQ_VGA1_GAIN_SHIFT 18

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G1_RX_EQ_VGA2_GAIN_MASK 0x00E00000
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G1_RX_EQ_VGA2_GAIN_SHIFT 21
/* not related to eq */
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G1_RX_EQ_MISC_MASK 0xFF000000
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G1_RX_EQ_MISC_SHIFT 24

/**** g1_tx_eq register ****/

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G1_TX_EQ_OVRD (1 << 0)

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G1_TX_EQ_MAIN_MASK 0x000000FC
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G1_TX_EQ_MAIN_SHIFT 2

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G1_TX_EQ_PRE_MASK 0x00000F00
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G1_TX_EQ_PRE_SHIFT 8

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G1_TX_EQ_POST_MASK 0x0000F000
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G1_TX_EQ_POST_SHIFT 12
/*
 * TX Miscellaneous Controls (not related to EQ) / Function: This input controls miscellaneous
 * settings in the TX analog block. This input must be set according to the values provided in
 * Chapter 5, "PHYUsage and Configuration”.
 */
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G1_TX_EQ_MISC_MASK 0xFF000000
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G1_TX_EQ_MISC_SHIFT 24

/**** g1_misc register ****/

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G1_MISC_RX_DFE_BYPASS (1 << 0)

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G1_MISC_RX_ADAPT_SEL (1 << 2)

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G1_MISC_RX_CDR_VCO_CONFIG_MASK 0x00000038
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G1_MISC_RX_CDR_VCO_CONFIG_SHIFT 3

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G1_MISC_RX_ADAPT_MODE_MASK 0x000000C0
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G1_MISC_RX_ADAPT_MODE_SHIFT 6

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G1_MISC_RX_CDR_PPM_MAX_MASK 0x00001F00
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G1_MISC_RX_CDR_PPM_MAX_SHIFT 8

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G1_MISC_RX_DELTA_IQ_MASK 0x000F0000
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G1_MISC_RX_DELTA_IQ_SHIFT 16

/**** g2_rx_eq register ****/

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G2_RX_EQ_CTLE_BOOST_MASK 0x0000001F
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G2_RX_EQ_CTLE_BOOST_SHIFT 0

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G2_RX_EQ_ATT_LVL_MASK 0x000000E0
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G2_RX_EQ_ATT_LVL_SHIFT 5

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G2_RX_EQ_DFE_TAP1_MASK 0x0000FF00
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G2_RX_EQ_DFE_TAP1_SHIFT 8

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G2_RX_EQ_CTLE_POLE_MASK 0x00030000
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G2_RX_EQ_CTLE_POLE_SHIFT 16

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G2_RX_EQ_VGA1_GAIN_MASK 0x001C0000
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G2_RX_EQ_VGA1_GAIN_SHIFT 18

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G2_RX_EQ_VGA2_GAIN_MASK 0x00E00000
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G2_RX_EQ_VGA2_GAIN_SHIFT 21
/* not related to eq */
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G2_RX_EQ_MISC_MASK 0xFF000000
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G2_RX_EQ_MISC_SHIFT 24

/**** g2_tx_eq register ****/

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G2_TX_EQ_OVRD (1 << 0)

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G2_TX_EQ_MAIN_MASK 0x000000FC
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G2_TX_EQ_MAIN_SHIFT 2

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G2_TX_EQ_PRE_MASK 0x00000F00
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G2_TX_EQ_PRE_SHIFT 8

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G2_TX_EQ_POST_MASK 0x0000F000
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G2_TX_EQ_POST_SHIFT 12
/* not related to eq */
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G2_TX_EQ_MISC_MASK 0xFF000000
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G2_TX_EQ_MISC_SHIFT 24

/**** g2_misc register ****/

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G2_MISC_RX_DFE_BYPASS (1 << 0)

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G2_MISC_RX_ADAPT_SEL (1 << 2)

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G2_MISC_RX_CDR_VCO_CONFIG_MASK 0x00000038
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G2_MISC_RX_CDR_VCO_CONFIG_SHIFT 3

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G2_MISC_RX_ADAPT_MODE_MASK 0x000000C0
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G2_MISC_RX_ADAPT_MODE_SHIFT 6

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G2_MISC_RX_CDR_PPM_MAX_MASK 0x00001F00
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G2_MISC_RX_CDR_PPM_MAX_SHIFT 8

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G2_MISC_RX_DELTA_IQ_MASK 0x000F0000
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G2_MISC_RX_DELTA_IQ_SHIFT 16

/**** g3_rx_eq register ****/

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G3_RX_EQ_CTLE_BOOST_MASK 0x0000001F
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G3_RX_EQ_CTLE_BOOST_SHIFT 0

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G3_RX_EQ_ATT_LVL_MASK 0x000000E0
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G3_RX_EQ_ATT_LVL_SHIFT 5

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G3_RX_EQ_DFE_TAP1_MASK 0x0000FF00
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G3_RX_EQ_DFE_TAP1_SHIFT 8

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G3_RX_EQ_CTLE_POLE_MASK 0x00030000
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G3_RX_EQ_CTLE_POLE_SHIFT 16

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G3_RX_EQ_VGA1_GAIN_MASK 0x001C0000
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G3_RX_EQ_VGA1_GAIN_SHIFT 18

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G3_RX_EQ_VGA2_GAIN_MASK 0x00E00000
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G3_RX_EQ_VGA2_GAIN_SHIFT 21
/* not related to eq */
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G3_RX_EQ_MISC_MASK 0xFF000000
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G3_RX_EQ_MISC_SHIFT 24

/**** g3_tx_eq register ****/

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G3_TX_EQ_OVRD (1 << 0)

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G3_TX_EQ_MAIN_MASK 0x000000FC
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G3_TX_EQ_MAIN_SHIFT 2

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G3_TX_EQ_PRE_MASK 0x00000F00
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G3_TX_EQ_PRE_SHIFT 8

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G3_TX_EQ_POST_MASK 0x0000F000
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G3_TX_EQ_POST_SHIFT 12
/* not related to eq */
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G3_TX_EQ_MISC_MASK 0xFF000000
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G3_TX_EQ_MISC_SHIFT 24

/**** g3_misc register ****/

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G3_MISC_RX_DFE_BYPASS (1 << 0)

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G3_MISC_RX_ADAPT_SEL (1 << 2)

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G3_MISC_RX_CDR_VCO_CONFIG_MASK 0x00000038
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G3_MISC_RX_CDR_VCO_CONFIG_SHIFT 3

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G3_MISC_RX_ADAPT_MODE_MASK 0x000000C0
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G3_MISC_RX_ADAPT_MODE_SHIFT 6

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G3_MISC_RX_CDR_PPM_MAX_MASK 0x00001F00
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G3_MISC_RX_CDR_PPM_MAX_SHIFT 8

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G3_MISC_RX_DELTA_IQ_MASK 0x000F0000
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G3_MISC_RX_DELTA_IQ_SHIFT 16

/**** g4_rx_eq register ****/

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G4_RX_EQ_CTLE_BOOST_MASK 0x0000001F
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G4_RX_EQ_CTLE_BOOST_SHIFT 0

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G4_RX_EQ_ATT_LVL_MASK 0x000000E0
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G4_RX_EQ_ATT_LVL_SHIFT 5

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G4_RX_EQ_DFE_TAP1_MASK 0x0000FF00
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G4_RX_EQ_DFE_TAP1_SHIFT 8

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G4_RX_EQ_CTLE_POLE_MASK 0x00030000
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G4_RX_EQ_CTLE_POLE_SHIFT 16

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G4_RX_EQ_VGA1_GAIN_MASK 0x001C0000
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G4_RX_EQ_VGA1_GAIN_SHIFT 18

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G4_RX_EQ_VGA2_GAIN_MASK 0x00E00000
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G4_RX_EQ_VGA2_GAIN_SHIFT 21
/* not related to eq */
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G4_RX_EQ_MISC_MASK 0xFF000000
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G4_RX_EQ_MISC_SHIFT 24

/**** g4_tx_eq register ****/

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G4_TX_EQ_OVRD (1 << 0)

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G4_TX_EQ_MAIN_MASK 0x000000FC
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G4_TX_EQ_MAIN_SHIFT 2

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G4_TX_EQ_PRE_MASK 0x00000F00
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G4_TX_EQ_PRE_SHIFT 8

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G4_TX_EQ_POST_MASK 0x0000F000
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G4_TX_EQ_POST_SHIFT 12
/* not related to eq */
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G4_TX_EQ_MISC_MASK 0xFF000000
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G4_TX_EQ_MISC_SHIFT 24

/**** g4_misc register ****/

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G4_MISC_RX_DFE_BYPASS (1 << 0)

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G4_MISC_RX_ADAPT_SEL (1 << 2)

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G4_MISC_RX_CDR_VCO_CONFIG_MASK 0x00000038
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G4_MISC_RX_CDR_VCO_CONFIG_SHIFT 3

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G4_MISC_RX_ADAPT_MODE_MASK 0x000000C0
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G4_MISC_RX_ADAPT_MODE_SHIFT 6

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G4_MISC_RX_CDR_PPM_MAX_MASK 0x00001F00
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G4_MISC_RX_CDR_PPM_MAX_SHIFT 8

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G4_MISC_RX_DELTA_IQ_MASK 0x000F0000
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_G4_MISC_RX_DELTA_IQ_SHIFT 16

/**** misc register ****/

#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_MISC_TX_IBOOST_LVL_MASK 0x0000000F
#define SERDES_SNPS_QUAD_OVERRIDE_LANEX_MISC_TX_IBOOST_LVL_SHIFT 0

#ifdef __cplusplus
}
#endif

#endif

/** @} */


