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
 * @file   al_hal_ddr_ctrl_regs_common.h
 *
 * @brief  DDR controller common register header file
 *
 */

#ifndef __AL_HAL_DDR_CTRL_REGS_DEFS_H__
#define __AL_HAL_DDR_CTRL_REGS_DEFS_H__

#include "al_hal_ddr_ctrl_regs_alpine_v1.h"
#include "al_hal_ddr_ctrl_regs_alpine_v2.h"
#include "al_hal_ddr_ctrl_regs_alpine_v3.h"

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


/*
* Registers Fields
*/
union al_ddr_ctrl_regs {
	struct al_ddr_ctrl_regs_alpine_v1 alpine_v1;
	struct al_ddr_ctrl_regs_alpine_v2 alpine_v2;
	struct al_ddr_ctrl_regs_alpine_v3 alpine_v3;
};


/**** MSTR register ****/
/*
 * Select DDR3 SDRAM
 *  - 1 - DDR3 SDRAM device in use
 *  - 0 - non-DDR3 SDRAM device in use
 * Only present in designs that support DDR3.
 */
#define DWC_DDR_UMCTL2_REGS_MSTR_DDR3 (1 << 0)
/*
 * Select DDR4 SDRAM
 *  - 1 - DDR4 SDRAM device in use.
 *  - 0 - non-DDR4 device in use
 * Present only in designs configured to support DDR4.
 */
#define DWC_DDR_UMCTL2_REGS_MSTR_DDR4 (1 << 4)
/* Indicates burst mode */
#define DWC_DDR_UMCTL2_REGS_MSTR_BURST_MODE (1 << 8)
/* When set, enable burst-chop in DDR3/DDR4. */
#define DWC_DDR_UMCTL2_REGS_MSTR_BURSTCHOP (1 << 9)
/*
 * If 1, then uMCTL2 uses 2T timing. Otherwise, uses 1T timing. In 2T timing, all command signals
 * (except chip select) are held for 2 clocks on the SDRAM bus. Chip select is asserted on the
 * second cycle of the command<br>Note: 2T timing is not supported in LPDDR2/LPDDR3 mode<br>Note: 2T
 * timing is not supported if the configuration parameter MEMC_CMD_RTN2IDLE is set<br>Note: 2T
 * timing is not supported in DDR4 geardown mode.
 */
#define DWC_DDR_UMCTL2_REGS_MSTR_EN_2T_TIMING_MODE (1 << 10)
/*
 * 1 indicates put the DRAM in geardown mode (2N) and 0 indicates put the DRAM in normal mode (1N).
 * This register can be changed, only when the Controller is in self-refresh mode.<br>This signal
 * must be set the same value as MR3 bit A3.<br>Note: Geardown mode is not supported if the
 * configuration parameter MEMC_CMD_RTN2IDLE is set
 */
#define DWC_DDR_UMCTL2_REGS_MSTR_GEARDOWN_MODE (1 << 11)
/*
 * Selects proportion of DQ bus width that is used by the SDRAM
 *  - 00 - Full DQ bus width to SDRAM
 *  - 01 - Half DQ bus width to SDRAM
 *  - 10 - Quarter DQ bus width to SDRAM
 *  - 11 - Reserved.
 * Note that half bus width mode is only supported when the SDRAM bus width is a multiple of 16, and
 * quarter bus width mode is only supported when the SDRAM bus width is a multiple of 32 and the
 * configuration parameter MEMC_QBUS_SUPPORT is set. Bus width refers to DQ bus width (excluding any
 * ECC width).
 */
#define DWC_DDR_UMCTL2_REGS_MSTR_DATA_BUS_WIDTH_MASK 0x00003000
#define DWC_DDR_UMCTL2_REGS_MSTR_DATA_BUS_WIDTH_SHIFT 12

#define DWC_DDR_UMCTL2_REGS_MSTR_DATA_BUS_WIDTH_64	\
	(0 << DWC_DDR_UMCTL2_REGS_MSTR_DATA_BUS_WIDTH_SHIFT)
#define DWC_DDR_UMCTL2_REGS_MSTR_DATA_BUS_WIDTH_32	\
	(1 << DWC_DDR_UMCTL2_REGS_MSTR_DATA_BUS_WIDTH_SHIFT)
#define DWC_DDR_UMCTL2_REGS_MSTR_DATA_BUS_WIDTH_16	\
	(2 << DWC_DDR_UMCTL2_REGS_MSTR_DATA_BUS_WIDTH_SHIFT)

/*
 * Set to 1 when the uMCTL2 and DRAM has to be put in DLL-off mode for low frequency operation.<br>
 * Set to 0 to put uMCTL2 and DRAM in DLL-on mode for normal frequency operation.<br>
 * If DDR4 CRC/parity retry is enabled (CRCPARCTL1.crc_parity_retry_enable = 1), dll_off_mode is not
 * supported, and this bit must be set to '0'.
 */
#define DWC_DDR_UMCTL2_REGS_MSTR_DLL_OFF_MODE (1 << 15)
/*
 * SDRAM burst length used:
 *  - 0001 - Burst length of 2 (only supported for mDDR)
 *  - 0010 - Burst length of 4
 *  - 0100 - Burst length of 8
 *  - 1000 - Burst length of 16 (only supported for mDDR and LPDDR2)
 * All other values are reserved.<br>This controls the burst size used to access the SDRAM. This
 * must match the burst length mode register setting in the SDRAM. Burst length of 2 is not
 * supported with AXI ports when MEMC_BURST_LENGTH is 8.
 */
#define DWC_DDR_UMCTL2_REGS_MSTR_BURST_RDWR_MASK 0x000F0000
#define DWC_DDR_UMCTL2_REGS_MSTR_BURST_RDWR_SHIFT 16
#define DWC_DDR_UMCTL2_REGS_MSTR_ACTIVE_LOGICAL_RANKS_MASK 0x00300000
#define DWC_DDR_UMCTL2_REGS_MSTR_ACTIVE_LOGICAL_RANKS_SHIFT 20
/*
 * Only present for multi-rank configurations.<br>Each bit represents one rank. For two-rank
 * configurations, only bits[25:24] are present.
 *  - 1 - populated
 *  - 0 - unpopulated
 * LSB is the lowest rank number.<br>For 2 ranks following combinations are legal:
 *  - 01 - One rank
 *  - 11 - Two ranks
 *  - Others - Reserved.
 * For 4 ranks following combinations are legal:
 *  - 0001 - One rank
 *  - 0011 - Two ranks
 *  - 1111 - Four ranks
 */
#define DWC_DDR_UMCTL2_REGS_MSTR_ACTIVE_RANKS_MASK 0x0F000000
#define DWC_DDR_UMCTL2_REGS_MSTR_ACTIVE_RANKS_SHIFT 24
/*
 * Indicates the configuration of the device used in the system.
 *  - 00 - x4 device
 *  - 01 - x8 device
 *  - 10 - x16 device
 *  - 11 - x32 device
 */
#define DWC_DDR_UMCTL2_REGS_MSTR_DEVICE_CONFIG_MASK 0xC0000000
#define DWC_DDR_UMCTL2_REGS_MSTR_DEVICE_CONFIG_SHIFT 30

#define DWC_DDR_UMCTL2_REGS_MSTR_DEVICE_CONFIG_X4	\
	(0 << DWC_DDR_UMCTL2_REGS_MSTR_DEVICE_CONFIG_SHIFT)
#define DWC_DDR_UMCTL2_REGS_MSTR_DEVICE_CONFIG_X8	\
	(1 << DWC_DDR_UMCTL2_REGS_MSTR_DEVICE_CONFIG_SHIFT)
#define DWC_DDR_UMCTL2_REGS_MSTR_DEVICE_CONFIG_X16	\
	(2 << DWC_DDR_UMCTL2_REGS_MSTR_DEVICE_CONFIG_SHIFT)
#define DWC_DDR_UMCTL2_REGS_MSTR_DEVICE_CONFIG_X32	\
	(3 << DWC_DDR_UMCTL2_REGS_MSTR_DEVICE_CONFIG_SHIFT)

/**** STAT register ****/
/*
 * Operating mode. This is 3-bits wide in configurations with mDDR/LPDDR2/LPDDR3/DDR4 support and
 * 2-bits in all other configurations.<br><b>non-mDDR/LPDDR2/LPDDR3 and non-DDR4 designs:</b>
 *  - 00 - Init
 *  - 01 - Normal
 *  - 10 - Power-down
 *  - 11 - Self refresh
 * <b>mDDR/LPDDR2/LPDDR3 or DDR4 designs:</b>
 *  - 000 - Init
 *  - 001 - Normal
 *  - 010 - Power-down
 *  - 011 - Self refresh
 *  - 1XX - Deep power-down / Maximum Power Saving Mode
 */
#define DWC_DDR_UMCTL2_REGS_STAT_OPERATING_MODE_MASK 0x00000007
#define DWC_DDR_UMCTL2_REGS_STAT_OPERATING_MODE_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_STAT_OPERATING_MODE_INIT		\
	(0 << DWC_DDR_UMCTL2_REGS_STAT_OPERATING_MODE_SHIFT)
#define DWC_DDR_UMCTL2_REGS_STAT_OPERATING_MODE_NORMAL		\
	(1 << DWC_DDR_UMCTL2_REGS_STAT_OPERATING_MODE_SHIFT)
#define DWC_DDR_UMCTL2_REGS_STAT_OPERATING_MODE_POWER_DOWN	\
	(2 << DWC_DDR_UMCTL2_REGS_STAT_OPERATING_MODE_SHIFT)
#define DWC_DDR_UMCTL2_REGS_STAT_OPERATING_MODE_SELF_REFRESH	\
	(3 << DWC_DDR_UMCTL2_REGS_STAT_OPERATING_MODE_SHIFT)

/*
 * Flags if Self Refresh is entered and if it was under Automatic Self Refresh control only or not.
 * - 00 - SDRAM is not in Self Refresh. If retry is enabled by CRCPARCTRL1.crc_parity_retry_enable,
 * this also indicates SRE command is still in parity error window or retry is in-progress.
 * - 11 - SDRAM is in Self Refresh and Self Refresh was caused by Automatic Self Refresh only. If
 * retry is enabled, this guarantees SRE command is executed correctly without parity error.
 * - 10 - SDRAM is in Self Refresh and Self Refresh was not caused solely under Automatic Self
 * Refresh control. It could have been caused by Hardware Low Power Interface and/or Software
 * (reg_ddrc_selfref_sw). If retry is enabled, this guarantees SRE command is executed correctly
 * without parity error.
 */
#define DWC_DDR_UMCTL2_REGS_STAT_SELFREF_TYPE_MASK 0x00000030
#define DWC_DDR_UMCTL2_REGS_STAT_SELFREF_TYPE_SHIFT 4

/**** MSTR1 register ****/
#define DWC_DDR_UMCTL2_REGS_MSTR1_RANK_TMGREG_SEL_MASK 0x0000000F
#define DWC_DDR_UMCTL2_REGS_MSTR1_RANK_TMGREG_SEL_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_MSTR1_ALT_ADDRMAP_EN (1 << 16)


/**** MRCTRL0 register ****/
/*
 * Indicates whether the mode register operation is read or write. Only used for LPDDR2/LPDDR3/DDR4.
 *  - 0 - Write
 *  - 1 - Read
 */
#define DWC_DDR_UMCTL2_REGS_MRCTRL0_MR_TYPE (1 << 0)

#define DWC_DDR_UMCTL2_REGS_MRCTRL0_MR_TYPE_READ (1 << 0)
#define DWC_DDR_UMCTL2_REGS_MRCTRL0_MR_TYPE_WRITE (0 << 0)

/*
 * Indicates whether the mode register operation is MRS or WR/RD for MPR (only supported for DDR4)
 *  - 0 - MRS
 *  - 1 - WR/RD for MPR
 */
#define DWC_DDR_UMCTL2_REGS_MRCTRL0_MPR_EN (1 << 1)
/*
 * Indicates whether the mode register operation is MRS in PDA mode or not
 *  - 0 - MRS
 *  - 1 - MRS in Per DRAM Addressability mode
 */
#define DWC_DDR_UMCTL2_REGS_MRCTRL0_PDA_EN (1 << 2)
/*
 * Indicates whether the mode register operation is MRS to the DDR4 RCD (MR7) before automatic SDRAM
 * initialization routine or not.<br>Note: This must be cleared to 0 after RCD initialization.
 * Otherwise, SDRAM initialization routine will not re-start.
 *  - 0 - MRS
 *  - 1 - MRS to the DDR4 RCD (MR7) before SDRAM initialization routine
 */
#define DWC_DDR_UMCTL2_REGS_MRCTRL0_RCD_INIT_EN (1 << 3)
/*
 * Controls which rank is accessed by MRCTRL0.mr_wr. Normally, it is desired to access all ranks, so
 * all bits should be set to 1. However, for multi-rank UDIMMs/RDIMMs which implement address
 * mirroring, it may be necessary to access ranks individually.<br>Examples (assume uMCTL2 is
 * configured for 4 ranks):
 *	- 0x1 - select rank 0 only
 *	- 0x2 - select rank 1 only
 *	- 0x5 - select ranks 0 and 2
 *	- 0xA - select ranks 1 and 3
 *	- 0xF - select ranks 0, 1, 2 and 3
 */
#define DWC_DDR_UMCTL2_REGS_MRCTRL0_MR_RANK_MASK 0x000000F0
#define DWC_DDR_UMCTL2_REGS_MRCTRL0_MR_RANK_SHIFT 4
/*
 * Address of the mode register that is to be written to.
 *  - 0000 - MR0
 *  - 0001 - MR1
 *  - 0010 - MR2
 *  - 0011 - MR3
 *  - 0100 - MR4
 *  - 0101 - MR5
 *  - 0110 - MR6
 *  - 0111 - MR7
 * Don't Care for LPDDR2/LPDDR3 (see MRCTRL1.mr_data for mode register addressing in
 * LPDDR2/LPDDR3)<br>
 * This signal is also used for writing to control words of RDIMMs. In that case, it corresponds to
 * the bank address bits sent to the RDIMM<br>
 * In case of DDR4, the bit[3:2] corresponds to the bank group bits. Therefore, the bit[3] as well
 * as the bit[2:0] must be set to an appropriate value which is considered both the Address
 * Mirroring of UDIMMs/RDIMMs and the Output Inversion of RDIMMs.
 */
#define DWC_DDR_UMCTL2_REGS_MRCTRL0_MR_ADDR_MASK 0x0000F000
#define DWC_DDR_UMCTL2_REGS_MRCTRL0_MR_ADDR_SHIFT 12
/*
 * Chip ID value for Read/Write command during MPR access (DDR4 3DS).
 *  This register must be set to 0 when DDR4 3DS feature is not used.
 *  This register must be set up while the controller is in reset.
 */
#define DWC_DDR_UMCTL2_REGS_MRCTRL0_MR_CID_MASK 0x00030000
#define DWC_DDR_UMCTL2_REGS_MRCTRL0_MR_CID_SHIFT 16
/*
 * Indicates whether PBA access is executed. When setting this bit to 1 along with setting pda_en to
 * 1, uMCTL2 initiates PBA access instead of PDA access.
 *  - 0 - Per DRAM Addressability mode
 *  - 1 - Per Buffer Addressability mode
 * The completion of PBA access is confirmed by MRSTAT.pda_done in the same way as PDA.
 */
#define DWC_DDR_UMCTL2_REGS_MRCTRL0_PBA_MODE (1 << 30)
/*
 * Setting this register bit to 1 triggers a mode register read or write operation. When the MR
 * operation is complete, the uMCTL2 automatically clears this bit. The other register fields of
 * this register must be written in a separate APB transaction, before setting this mr_wr bit. It is
 * recommended NOT to set this signal if in Init, Deep power-down or MPSM operating modes.
 */
#define DWC_DDR_UMCTL2_REGS_MRCTRL0_MR_WR (1 << 31)

/**** MRCTRL1 register ****/
/*
 * Mode register write data for all non-LPDDR2/non-LPDDR3 modes.<br>For LPDDR2/LPDDR3, MRCTRL1[15:0]
 * are interpreted as<br>[15:8] MR Address<br>[7:0] MR data for writes, don't care for reads. This
 * is 18-bits wide in configurations with DDR4 support and 16-bits in all other configurations.
 */
#define DWC_DDR_UMCTL2_REGS_MRCTRL1_MR_DATA_MASK 0x0003FFFF
#define DWC_DDR_UMCTL2_REGS_MRCTRL1_MR_DATA_SHIFT 0

/**** MRSTAT register ****/
/*
 * The SoC core may initiate a MR write operation only if this signal is low. This signal goes high
 * in the clock after the uMCTL2 accepts the MRW/MRR request. It goes low when the MRW/MRR command
 * is issued to the SDRAM. It is recommended not to perform MRW/MRR commands when
 * 'MRSTAT.mr_wr_busy' is high.
 *  - 0 - Indicates that the SoC core can initiate a mode register write operation
 *  - 1 - Indicates that mode register write operation is in progress
 */
#define DWC_DDR_UMCTL2_REGS_MRSTAT_MR_WR_BUSY (1 << 0)
/*
 * The SoC core may initiate a MR write operation in PDA mode only if this signal is low. This
 * signal goes high when three consecutive MRS commands related to the PDA mode are issued to the
 * SDRAM. This signal goes low when MRCTRL0.pda_en becomes 0. Therefore, it is recommended to write
 * MRCTRL0.pda_en to 0 after this signal goes high in order to prepare to perform PDA operation next
 * time.
 * - 0 - Indicates that mode register write operation related to PDA is in progress or has not
 * started yet.
 *  - 1 - Indicates that mode register write operation related to PDA has competed.
 */
#define DWC_DDR_UMCTL2_REGS_MRSTAT_PDA_DONE (1 << 8)

/**** PWRCTL register ****/
/*
 * If true then the uMCTL2 puts the SDRAM into Self Refresh after a programmable number of cycles
 * "maximum idle clocks before Self Refresh (PWRTMG.selfref_to_x32)". This register bit may be
 * re-programmed during the course of normal operation.
 */
#define DWC_DDR_UMCTL2_REGS_PWRCTL_SELFREF_EN (1 << 0)
/*
 * If true then the uMCTL2 goes into power-down after a programmable number of cycles "maximum idle
 * clocks before power down" (PWRTMG.powerdown_to_x32).<br>This register bit may be re-programmed
 * during the course of normal operation.
 */
#define DWC_DDR_UMCTL2_REGS_PWRCTL_POWERDOWN_EN (1 << 1)
/*
 * Enable the assertion of dfi_dram_clk_disable whenever a clock is not required by the SDRAM.<br>
 * If set to 0, dfi_dram_clk_disable is never asserted.<br>
 * Assertion of dfi_dram_clk_disable is as follows:<br>
 * In DDR2/DDR3, can only be asserted in Self Refresh.<br>
 * In DDR4, can be asserted in following:
 *   - in Self Refresh.
 *   - in Maximum Power Saving Mode
 * In mDDR/LPDDR2/LPDDR3, can be asserted in following:
 *   - in Self Refresh
 *   - in Power Down
 *   - in Deep Power Down
 *   - during Normal operation (Clock Stop)
 */
#define DWC_DDR_UMCTL2_REGS_PWRCTL_EN_DFI_DRAM_CLK_DISABLE (1 << 3)
/*
 * When this is 1, the uMCTL2 puts the SDRAM into maximum power saving mode when the transaction
 * store is empty.<br>This register must be reset to '0' to bring uMCTL2 out of maximum power saving
 * mode.<br>Present only in designs configured to support DDR4.<br>Note that MPSM is not supported
 * when using a Synopsys DWC DDR PHY, if the PHY parameter DWC_AC_CS_USE is disabled, as the MPSM
 * exit sequence requires the chip-select signal to toggle.<br>FOR PERFORMANCE ONLY.
 */
#define DWC_DDR_UMCTL2_REGS_PWRCTL_MPSM_EN (1 << 4)
/*
 * A value of 1 to this register causes system to move to Self Refresh state immediately, as long as
 * it is not in INIT or DPD/MPSM operating_mode. This is referred to as Software Entry/Exit to Self
 * Refresh.
 *   - 1 - Software Entry to Self Refresh
 *   - 0 - Software Exit from Self Refresh
 */
#define DWC_DDR_UMCTL2_REGS_PWRCTL_SELFREF_SW (1 << 5)

/**** PWRTMG register ****/
/*
 * After this many clocks of NOP or deselect the uMCTL2 automatically puts the SDRAM into
 * power-down. This must be enabled in the PWRCTL.powerdown_en.<br>Unit: Multiples of 32
 * clocks<br>FOR PERFORMANCE ONLY.
 */
#define DWC_DDR_UMCTL2_REGS_PWRTMG_POWERDOWN_TO_X32_MASK 0x0000001F
#define DWC_DDR_UMCTL2_REGS_PWRTMG_POWERDOWN_TO_X32_SHIFT 0
/*
 * After this many clocks of NOP or deselect the uMCTL2 automatically puts the SDRAM into Self
 * Refresh. This must be enabled in the PWRCTL.selfref_en.<br>Unit: Multiples of 32 clocks.<br>FOR
 * PERFORMANCE ONLY.
 */
#define DWC_DDR_UMCTL2_REGS_PWRTMG_SELFREF_TO_X32_MASK 0x00FF0000
#define DWC_DDR_UMCTL2_REGS_PWRTMG_SELFREF_TO_X32_SHIFT 16

/**** HWLPCTL register ****/
/*
 * Enable for Hardware Low Power Interface.
 */
#define DWC_DDR_UMCTL2_REGS_HWLPCTL_HW_LP_EN (1 << 0)
/*
 * When this bit is programmed to 1 the cactive_in_ddrc pin of the DDRC can be used to exit from the
 * automatic clock stop, automatic power down or automatic self-refresh modes. Note, it will not
 * cause exit of Self-Refresh that was caused by Hardware Low Power Interface and/or Software
 * (PWRCTL.selfref_sw).
 */
#define DWC_DDR_UMCTL2_REGS_HWLPCTL_HW_LP_EXIT_IDLE_EN (1 << 1)
/*
 * Hardware idle period. The cactive_ddrc output is driven low if the system is idle for hw_lp_idle
 * * 32 cycles if not in INIT or DPD/MPSM operating_mode. The hardware idle function is disabled
 * when hw_lp_idle_x32=0.<br>Unit: Multiples of 32 clocks.<br>FOR PERFORMANCE ONLY.
 */
#define DWC_DDR_UMCTL2_REGS_HWLPCTL_HW_LP_IDLE_X32_MASK 0x0FFF0000
#define DWC_DDR_UMCTL2_REGS_HWLPCTL_HW_LP_IDLE_X32_SHIFT 16

/**** RFSHCTL0 register ****/
/*
 * The programmed value + 1 is the number of refresh timeouts that is allowed to accumulate before
 * traffic is blocked and the refreshes are forced to execute. Closing pages to perform a refresh is
 * a one-time penalty that must be paid for each group of refreshes. Therefore, performing refreshes
 * in a burst reduces the per-refresh penalty of these page closings. Higher numbers for
 * RFSHCTL.refresh_burst slightly increases utilization; lower numbers decreases the worst-case
 * latency associated with refreshes.
 *  - 0 - single refresh
 *  - 1 - burst-of-2 refresh
 *  - 7 - burst-of-8 refresh
 * For information on burst refresh feature refer to section 3.9 of DDR2 JEDEC specification -
 * JESD79-2F.pdf.<br>For DDR2/3, the refresh is always per-rank and not per-bank. The rank refresh
 * can be accumulated over 8*tREFI cycles using the burst refresh feature.
 * In DDR4 mode, according to Fine Granularity feature, 8 refreshes can be postponed in 1X mode, 16
 * refreshes in 2X mode and 32 refreshes in 4X mode. If using PHY-initiated updates, care must be
 * taken in the setting of RFSHCTL0.refresh_burst, to ensure that t<sub>RFCmax</sub> is not violated
 * due to a PHY-initiated update occurring shortly before a refresh burst was due. In this
 * situation, the refresh burst will be delayed until the PHY-initiated update is complete.
 */

#define DWC_DDR_UMCTL2_REGS_RFSHCTL0_REFRESH_BURST_MASK_ALPINE_V1 0x00000700
#define DWC_DDR_UMCTL2_REGS_RFSHCTL0_REFRESH_BURST_SHIFT_ALPINE_V1 8

#define DWC_DDR_UMCTL2_REGS_RFSHCTL0_REFRESH_BURST_ALPINE_V1(n)	\
	(((n) - 1) << DWC_DDR_UMCTL2_REGS_RFSHCTL0_REFRESH_BURST_SHIFT_ALPINE_V1)

#define DWC_DDR_UMCTL2_REGS_RFSHCTL0_REFRESH_BURST_MASK_ALPINE_V2_V3 0x000001F0
#define DWC_DDR_UMCTL2_REGS_RFSHCTL0_REFRESH_BURST_SHIFT_ALPINE_V2_V3 4

#define DWC_DDR_UMCTL2_REGS_RFSHCTL0_REFRESH_BURST_ALPINE_V2_V3(n)	\
	(((n) - 1) << DWC_DDR_UMCTL2_REGS_RFSHCTL0_REFRESH_BURST_SHIFT_ALPINE_V2_V3)

/*
 * If the refresh timer (t<sub>RFC</sub>nom, also known as t<sub>REFI</sub>) has expired at least
 * once, but it has not expired (RFSHCTL0.refresh_burst+1) times yet, then a speculative refresh may
 * be performed. A speculative refresh is a refresh performed at a time when refresh would be
 * useful, but before it is absolutely required. When the SDRAM bus is idle for a period of time
 * determined by this RFSHCTL0.refresh_to_x32 and the refresh timer has expired at least once since
 * the last refresh, then a speculative refresh is performed. Speculative refreshes continues
 * successively until there are no refreshes pending or until new reads or writes are issued to the
 * uMCTL2.<br>FOR PERFORMANCE ONLY.
 */
#define DWC_DDR_UMCTL2_REGS_RFSHCTL0_REFRESH_TO_X32_MASK 0x0001F000
#define DWC_DDR_UMCTL2_REGS_RFSHCTL0_REFRESH_TO_X32_SHIFT 12
/*
 * Threshold value in number of clock cycles before the critical refresh or page timer expires. A
 * critical refresh is to be issued before this threshold is reached. It is recommended that this
 * not be changed from the default value, currently shown as 0x2. It must always be less than
 * internally used t_rfc_nom_x32. Note that, in LPDDR2/LPDDR3, internally used t_rfc_nom_x32 may be
 * equal to RFSHTMG.t_rfc_nom_x32>>2 if derating is enabled (DERATEEN.derate_enable=1). Otherwise,
 * internally used t_rfc_nom_x32 will be equal to RFSHTMG.t_rfc_nom_x32. <br>Unit: Multiples of 32
 * clocks.
 */
#define DWC_DDR_UMCTL2_REGS_RFSHCTL0_REFRESH_MARGIN_MASK 0x00F00000
#define DWC_DDR_UMCTL2_REGS_RFSHCTL0_REFRESH_MARGIN_SHIFT 20

/**** RFSHCTL1 register ****/
/*
 * Refresh timer start for rank 0 (only present in multi-rank configurations). This is useful in
 * staggering the refreshes to multiple ranks to help traffic to proceed. This is explained in
 * Refresh Controls section of architecture chapter.<br>Unit: Multiples of 32 clocks.<br>FOR
 * PERFORMANCE ONLY.
 */
#define DWC_DDR_UMCTL2_REGS_RFSHCTL1_REFRESH_TIMER0_START_VALUE_X32_MASK 0x00000FFF
#define DWC_DDR_UMCTL2_REGS_RFSHCTL1_REFRESH_TIMER0_START_VALUE_X32_SHIFT 0
/*
 * Refresh timer start for rank 1 (only present in multi-rank configurations). This is useful in
 * staggering the refreshes to multiple ranks to help traffic to proceed. This is explained in
 * Refresh Controls section of architecture chapter.<br>Unit: Multiples of 32 clocks.<br>FOR
 * PERFORMANCE ONLY.
 */
#define DWC_DDR_UMCTL2_REGS_RFSHCTL1_REFRESH_TIMER1_START_VALUE_X32_MASK 0x0FFF0000
#define DWC_DDR_UMCTL2_REGS_RFSHCTL1_REFRESH_TIMER1_START_VALUE_X32_SHIFT 16

/**** RFSHCTL2 register ****/
/*
 * Refresh timer start for rank 2 (only present in 4-rank configurations). This is useful in
 * staggering the refreshes to multiple ranks to help traffic to proceed. This is explained in
 * Refresh Controls section of architecture chapter.<br>Unit: Multiples of 32 clocks<br>FOR
 * PERFORMANCE ONLY.
 */
#define DWC_DDR_UMCTL2_REGS_RFSHCTL2_REFRESH_TIMER2_START_VALUE_X32_MASK 0x00000FFF
#define DWC_DDR_UMCTL2_REGS_RFSHCTL2_REFRESH_TIMER2_START_VALUE_X32_SHIFT 0
/*
 * Refresh timer start for rank 3 (only present in 4-rank configurations). This is useful in
 * staggering the refreshes to multiple ranks to help traffic to proceed. This is explained in
 * Refresh Controls section of architecture chapter.<br>Unit: Multiples of 32 clocks<br>FOR
 * PERFORMANCE ONLY.
 */
#define DWC_DDR_UMCTL2_REGS_RFSHCTL2_REFRESH_TIMER3_START_VALUE_X32_MASK 0x0FFF0000
#define DWC_DDR_UMCTL2_REGS_RFSHCTL2_REFRESH_TIMER3_START_VALUE_X32_SHIFT 16

/**** RFSHCTL3 register ****/
/*
 * When '1', disable auto-refresh generated by the uMCTL2. When auto-refresh is disabled, the SoC
 * core must generate refreshes using the registers reg_ddrc_rank0_refresh, reg_ddrc_rank1_refresh,
 * reg_ddrc_rank2_refresh and reg_ddrc_rank3_refresh.<br>When dis_auto_refresh transitions from 0 to
 * 1, any pending refreshes are immediately scheduled by the uMCTL2.<br>If DDR4 CRC/parity retry is
 * enabled (CRCPARCTL1.crc_parity_retry_enable = 1), disable auto-refresh is not supported, and this
 * bit must be set to '0'.<br>This register field is changeable on the fly.
 */
#define DWC_DDR_UMCTL2_REGS_RFSHCTL3_DIS_AUTO_REFRESH (1 << 0)
/*
 * Toggle this signal (either from 0 to 1 or from 1 to 0) to indicate that the refresh register(s)
 * have been updated.<br>The value is automatically updated when exiting reset, so it does not need
 * to be toggled initially.
 */
#define DWC_DDR_UMCTL2_REGS_RFSHCTL3_REFRESH_UPDATE_LEVEL (1 << 1)
/*
 * Fine Granularity Refresh Mode
 *   - 000 - Fixed 1x (Normal mode)
 *   - 001 - Fixed 2x
 *   - 010 - Fixed 4x
 *   - 101 - Enable on the fly 2x (not supported)
 *   - 110 - Enable on the fly 4x (not supported)
 *   - Everything else - reserved
 * Note: The on-the-fly modes is not supported in this version of the uMCTL2.<br>Note: This must be
 * set up while the Controller is in reset or while the Controller is in self-refresh mode. Changing
 * this during normal operation is not allowed. Making this a dynamic register will be supported in
 * future version of the uMCTL2.
 */
#define DWC_DDR_UMCTL2_REGS_RFSHCTL3_REFRESH_MODE_MASK 0x00000070
#define DWC_DDR_UMCTL2_REGS_RFSHCTL3_REFRESH_MODE_SHIFT 4

#define DWC_DDR_UMCTL2_REGS_RFSHCTL3_REFRESH_MODE_1X \
	(0 << DWC_DDR_UMCTL2_REGS_RFSHCTL3_REFRESH_MODE_SHIFT)
#define DWC_DDR_UMCTL2_REGS_RFSHCTL3_REFRESH_MODE_2X \
	(1 << DWC_DDR_UMCTL2_REGS_RFSHCTL3_REFRESH_MODE_SHIFT)
#define DWC_DDR_UMCTL2_REGS_RFSHCTL3_REFRESH_MODE_4X \
	(2 << DWC_DDR_UMCTL2_REGS_RFSHCTL3_REFRESH_MODE_SHIFT)

#define DWC_DDR_UMCTL2_REGS_RFSHCTL3_RANK_DIS_REFRESH_MASK 0x000F0000
#define DWC_DDR_UMCTL2_REGS_RFSHCTL3_RANK_DIS_REFRESH_SHIFT 16

/**** RFSHCTL4 register ****/
#define DWC_DDR_UMCTL2_REGS_RFSHCTL4_REFRESH_TIMER_LR_OFFSET_X32_MASK 0x000007FF
#define DWC_DDR_UMCTL2_REGS_RFSHCTL4_REFRESH_TIMER_LR_OFFSET_X32_SHIFT 0

/**** RFSHTMG register ****/
/*
 * t<sub>RFC</sub> (min): Minimum time from refresh to refresh or activate.<br>
 * For MEMC_FREQ_RATIO=1 configurations, t_rfc_min should be set to
 * RoundUp(t<sub>RFCmin</sub>/t<sub>CK</sub>).<br>
 * For MEMC_FREQ_RATIO=2 configurations, t_rfc_min should be set to
 * RoundUp(RoundUp(t<sub>RFCmin</sub>/t<sub>CK</sub>)/2).<br><br>
 * In LPDDR2/LPDDR3 mode:
 * - if using all-bank refreshes, the t<sub>RFCmin</sub> value in the above equations is equal to
 * t<sub>RFCab</sub>
 * - if using per-bank refreshes, the t<sub>RFCmin</sub> value in the above equations is equal to
 * t<sub>RFCpb</sub><br>
 * In DDR4 mode, the t<sub>RFCmin</sub> value in the above equations is different depending on the
 * refresh mode (fixed 1X,2X,4X) and the device density. The user should program the appropriate
 * value from the spec based on the 'refresh_mode' and the device density that is used.<br>
 * Unit: Clocks.
 */
#define DWC_DDR_UMCTL2_REGS_RFSHTMG_T_RFC_MIN_MASK 0x000003FF
#define DWC_DDR_UMCTL2_REGS_RFSHTMG_T_RFC_MIN_SHIFT 0
/*
 * t<sub>REFI</sub>: Average time interval between refreshes per rank (Specification: 7.8us for
 * DDR2, DDR3 and DDR4. See JEDEC specification for mDDR, LPDDR2 and LPDDR3).<br>For LPDDR2/LPDDR3:
 * - if using all-bank refreshes (RFSHCTL0.per_bank_refresh = 0), this register should be set to
 * t<sub>REFIab</sub>
 * - if using per-bank refreshes (RFSHCTL0.per_bank_refresh = 1), this register should be set to
 * t<sub>REFIpb</sub><br>
 * For configurations with MEMC_FREQ_RATIO=2, program this to (t<sub>REFI</sub>/2), no rounding
 * up.<br>
 * In DDR4 mode, t<sub>REFI</sub> value is different depending on the refresh mode. The user should
 * program the appropriate value from the spec based on the value programmed in the refresh mode
 * register.<br>Note that RFSHTMG.t_rfc_nom_x32 * 32 must be greater than RFSHTMG.t_rfc_min.<br>
 * Unit: Multiples of 32 clocks.
 */
#define DWC_DDR_UMCTL2_REGS_RFSHTMG_T_RFC_NOM_X32_MASK 0x0FFF0000
#define DWC_DDR_UMCTL2_REGS_RFSHTMG_T_RFC_NOM_X32_SHIFT 16

/**** RFSHTMG1 register ****/
#define DWC_DDR_UMCTL2_REGS_RFSHTMG1_T_RFC_MIN_DLR_MASK 0x000000FF
#define DWC_DDR_UMCTL2_REGS_RFSHTMG1_T_RFC_MIN_DLR_SHIFT 0

/**** ECCCFG0 register ****/
/*
 * ECC mode indicator
 *  - 000 - ECC disabled
 *  - 100 - ECC enabled - SEC/DED over 1 beat
 *  - 101 - ECC enabled - Advanced ECC
 *  - all other settings are reserved for future use
 */
#define DWC_DDR_UMCTL2_REGS_ECCCFG0_ECC_MODE_MASK 0x00000007
#define DWC_DDR_UMCTL2_REGS_ECCCFG0_ECC_MODE_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_ECCCFG0_ECC_MODE_DIS	\
	(0 << DWC_DDR_UMCTL2_REGS_ECCCFG0_ECC_MODE_SHIFT)
#define DWC_DDR_UMCTL2_REGS_ECCCFG0_ECC_MODE_EN		\
	(4 << DWC_DDR_UMCTL2_REGS_ECCCFG0_ECC_MODE_SHIFT)

/* Disable ECC scrubs. Valid only when ECCCFG0.ecc_mode = 3'b100 and MEMC_USE_RMW is defined */
#define DWC_DDR_UMCTL2_REGS_ECCCFG0_DIS_SCRUB (1 << 4)

/**** ECCCFG1 register ****/
/*
 * Enable ECC data poisoning - introduces ECC errors on writes to address specified by the
 * ECCPOISONADDR0/1 registers
 */
#define DWC_DDR_UMCTL2_REGS_ECCCFG1_DATA_POISON_EN (1 << 0)
/*
 * Selects whether to poison 1 or 2 bits - if 0 -> 2-bit (uncorrectable) data poisoning, if 1 ->
 * 1-bit (correctable) data poisoning, if ECCCFG1.data_poison_en=1
 */
#define DWC_DDR_UMCTL2_REGS_ECCCFG1_DATA_POISON_BIT (1 << 1)

/**** ECCSTAT register ****/
/*
 * Bit number corrected by single-bit ECC error. See ECC section of architecture chapter for
 * encoding of this field. If more than one data lane has an error, the lower data lane is selected.
 * This register is 7 bits wide in order to handle 72 bits of the data present in a single lane.
 */
#define DWC_DDR_UMCTL2_REGS_ECCSTAT_ECC_CORRECTED_BIT_NUM_MASK 0x0000007F
#define DWC_DDR_UMCTL2_REGS_ECCSTAT_ECC_CORRECTED_BIT_NUM_SHIFT 0
/*
 * Single-bit error indicators, 1 per ECC lane. If MEMC_FREQ_RATIO=1, there are only two lanes, so
 * only the lower two bits are used
 */
#define DWC_DDR_UMCTL2_REGS_ECCSTAT_ECC_CORRECTED_ERR_MASK 0x00000F00
#define DWC_DDR_UMCTL2_REGS_ECCSTAT_ECC_CORRECTED_ERR_SHIFT 8
/*
 * Double-bit error indicators, 1 per ECC lane. If MEMC_FREQ_RATIO=1, there are only two lanes, so
 * only the lower two bits are used
 */
#define DWC_DDR_UMCTL2_REGS_ECCSTAT_ECC_UNCORRECTED_ERR_MASK 0x000F0000
#define DWC_DDR_UMCTL2_REGS_ECCSTAT_ECC_UNCORRECTED_ERR_SHIFT 16

/**** ECCCLR register ****/
/*
 * Setting this register bit to 1 clears the currently stored corrected ECC error. The
 * ECCSTAT.ecc_corrected_err, ECCSTAT.ecc_corrected_bit_num, ECCCADDR0, ECCCADDR1, ECCCSYN0,
 * ECCCSYN1, ECCCSYN2, ECCBITMASK0, ECCBITMASK1 and ECCBITMASK2 registers are cleared by this
 * operation. When the clear operation is complete, the uMCTL2 automatically clears this bit.
 */
#define DWC_DDR_UMCTL2_REGS_ECCCLR_ECC_CLR_CORR_ERR (1 << 0)
/*
 * Setting this register bit to 1 clears the currently stored uncorrected ECC error. The
 * ECCSTAT.ecc_uncorrected_err, ECCUADDR0, ECCUADDR1, ECCUSYN0, ECCUSYN1 and ECCUSYN2 registers are
 * cleared by this operation. When the clear operation is complete, the uMCTL2 automatically clears
 * this bit.
 */
#define DWC_DDR_UMCTL2_REGS_ECCCLR_ECC_CLR_UNCORR_ERR (1 << 1)
/*
 * Setting this register bit to 1 clears the currently stored corrected ECC error count. The
 * ECCERRCNT.ecc_corr_err_cnt register is cleared by this operation. When the clear operation is
 * complete, the uMCTL2 automatically clears this bit.
 */
#define DWC_DDR_UMCTL2_REGS_ECCCLR_ECC_CLR_CORR_ERR_CNT (1 << 2)
/*
 * Setting this register bit to 1 clears the currently stored uncorrected ECC error count. The
 * ECCSTAT.ecc_uncorr_err_cnt register is cleared by this operation. When the clear operation is
 * complete, the uMCTL2 automatically clears this bit.
 */
#define DWC_DDR_UMCTL2_REGS_ECCCLR_ECC_CLR_UNCORR_ERR_CNT (1 << 3)

/**** ECCERRCNT register ****/
/* Number of correctable ECC errors detected */
#define DWC_DDR_UMCTL2_REGS_ECCERRCNT_ECC_CORR_ERR_CNT_MASK 0x0000FFFF
#define DWC_DDR_UMCTL2_REGS_ECCERRCNT_ECC_CORR_ERR_CNT_SHIFT 0
/* Number of uncorrectable ECC errors detected */
#define DWC_DDR_UMCTL2_REGS_ECCERRCNT_ECC_UNCORR_ERR_CNT_MASK 0xFFFF0000
#define DWC_DDR_UMCTL2_REGS_ECCERRCNT_ECC_UNCORR_ERR_CNT_SHIFT 16

/**** ECCCADDR0 register ****/
/*
 * Page/row number of a read resulting in a corrected ECC error. This is 18-bits wide in
 * configurations with DDR4 support and 16-bits in all other configurations.
 */
#define DWC_DDR_UMCTL2_REGS_ECCCADDR0_ECC_CORR_ROW_MASK 0x0003FFFF
#define DWC_DDR_UMCTL2_REGS_ECCCADDR0_ECC_CORR_ROW_SHIFT 0
/* Rank number of a read resulting in a corrected ECC error */
#define DWC_DDR_UMCTL2_REGS_ECCCADDR0_ECC_CORR_RANK_MASK 0x03000000
#define DWC_DDR_UMCTL2_REGS_ECCCADDR0_ECC_CORR_RANK_SHIFT 24

/**** ECCCADDR1 register ****/
/* Block number of a read resulting in a corrected ECC error (lowest bit not assigned here) */
#define DWC_DDR_UMCTL2_REGS_ECCCADDR1_ECC_CORR_COL_MASK 0x00000FFF
#define DWC_DDR_UMCTL2_REGS_ECCCADDR1_ECC_CORR_COL_SHIFT 0
/* Bank number of a read resulting in a corrected ECC error */
#define DWC_DDR_UMCTL2_REGS_ECCCADDR1_ECC_CORR_BANK_MASK 0x00070000
#define DWC_DDR_UMCTL2_REGS_ECCCADDR1_ECC_CORR_BANK_SHIFT 16
/* Bank Group number of a read resulting in a corrected ECC error */
#define DWC_DDR_UMCTL2_REGS_ECCCADDR1_ECC_CORR_BG_MASK 0x03000000
#define DWC_DDR_UMCTL2_REGS_ECCCADDR1_ECC_CORR_BG_SHIFT 24
#define DWC_DDR_UMCTL2_REGS_ECCCADDR1_ECC_CORR_CID_MASK 0x30000000
#define DWC_DDR_UMCTL2_REGS_ECCCADDR1_ECC_CORR_CID_SHIFT 28

/**** ECCCSYN2 register ****/
/*
 * Data pattern that resulted in a corrected error one for each ECC lane, all concatenated together
 * This register refers to the ECC byte, which is bits [71:64] for 64-bit ECC, [39:32] for 32-bit
 * ECC, or [23:16] for 16-bit ECC
 */
#define DWC_DDR_UMCTL2_REGS_ECCCSYN2_ECC_CORR_SYNDROMES_71_64_MASK 0x000000FF
#define DWC_DDR_UMCTL2_REGS_ECCCSYN2_ECC_CORR_SYNDROMES_71_64_SHIFT 0

/**** ECCBITMASK2 register ****/
/*
 * Mask for the corrected data portion
 *	- 1 on any bit indicates that the bit has been corrected by the ECC logic
 *	- 0 on any bit indicates that the bit has not been corrected by the ECC logic
 * This register accumulates data over multiple ECC errors, to give an overall indication of which
 * bits are being fixed. It is cleared by writing a 1 to ECCCLR.ecc_clr_corr_err.
 * This register refers to the ECC byte, which is bits [71:64] for 64-bit ECC, [39:32] for 32-bit
 * ECC, or [23:16] for 16-bit ECC
 */
#define DWC_DDR_UMCTL2_REGS_ECCBITMASK2_ECC_CORR_BIT_MASK_71_64_MASK 0x000000FF
#define DWC_DDR_UMCTL2_REGS_ECCBITMASK2_ECC_CORR_BIT_MASK_71_64_SHIFT 0

/**** ECCUADDR0 register ****/
/*
 * Page/row number of a read resulting in an uncorrected ECC error. This is 18-bits wide in
 * configurations with DDR4 support and 16-bits in all other configurations.
 */
#define DWC_DDR_UMCTL2_REGS_ECCUADDR0_ECC_UNCORR_ROW_MASK 0x0003FFFF
#define DWC_DDR_UMCTL2_REGS_ECCUADDR0_ECC_UNCORR_ROW_SHIFT 0
/* Rank number of a read resulting in an uncorrected ECC error */
#define DWC_DDR_UMCTL2_REGS_ECCUADDR0_ECC_UNCORR_RANK_MASK 0x03000000
#define DWC_DDR_UMCTL2_REGS_ECCUADDR0_ECC_UNCORR_RANK_SHIFT 24

/**** ECCUADDR1 register ****/
/* Block number of a read resulting in an uncorrected ECC error (lowest bit not assigned here) */
#define DWC_DDR_UMCTL2_REGS_ECCUADDR1_ECC_UNCORR_COL_MASK 0x00000FFF
#define DWC_DDR_UMCTL2_REGS_ECCUADDR1_ECC_UNCORR_COL_SHIFT 0
/* Bank number of a read resulting in an uncorrected ECC error */
#define DWC_DDR_UMCTL2_REGS_ECCUADDR1_ECC_UNCORR_BANK_MASK 0x00070000
#define DWC_DDR_UMCTL2_REGS_ECCUADDR1_ECC_UNCORR_BANK_SHIFT 16
/* Bank Group number of a read resulting in an uncorrected ECC error */
#define DWC_DDR_UMCTL2_REGS_ECCUADDR1_ECC_UNCORR_BG_MASK 0x03000000
#define DWC_DDR_UMCTL2_REGS_ECCUADDR1_ECC_UNCORR_BG_SHIFT 24
/* CID number of a read resulting in an uncorrected ECC error */
#define DWC_DDR_UMCTL2_REGS_ECCUADDR1_ECC_UNCORR_CID_MASK 0x30000000
#define DWC_DDR_UMCTL2_REGS_ECCUADDR1_ECC_UNCORR_CID_SHIFT 28

/**** ECCUSYN2 register ****/
/*
 * Data pattern that resulted in an uncorrected error one for each ECC lane, all concatenated
 * together.
 * This register refers to the ECC byte, which is bits [71:64] for 64-bit ECC, [39:32] for 32-bit
 * ECC, or [23:16] for 16-bit ECC
 */
#define DWC_DDR_UMCTL2_REGS_ECCUSYN2_ECC_UNCORR_SYNDROMES_71_64_MASK 0x000000FF
#define DWC_DDR_UMCTL2_REGS_ECCUSYN2_ECC_UNCORR_SYNDROMES_71_64_SHIFT 0

/**** ECCPOISONADDR0 register ****/
/*
 * Column address for ECC poisoning. Note that this column address must be burst aligned:
 * - In full bus width mode, ecc_poison_col[2:0] must be set to 0
 * - In half bus width mode, ecc_poison_col[3:0] must be set to 0
 * - In quarter bus width mode, ecc_poison_col[4:0] must be set to 0
 */
#define DWC_DDR_UMCTL2_REGS_ECCPOISONADDR0_ECC_POISON_COL_MASK 0x00000FFF
#define DWC_DDR_UMCTL2_REGS_ECCPOISONADDR0_ECC_POISON_COL_SHIFT 0
/*
 * Chip ID for ECC poisoning (DDR4 3DS only)
 *  This register must be set to 0 when DDR4 3DS feature is not used.
 */
#define DWC_DDR_UMCTL2_REGS_ECCPOISONADDR0_ECC_POISON_CID_MASK 0x00030000
#define DWC_DDR_UMCTL2_REGS_ECCPOISONADDR0_ECC_POISON_CID_SHIFT 16
/* Rank address for ECC poisoning */
#define DWC_DDR_UMCTL2_REGS_ECCPOISONADDR0_ECC_POISON_RANK_MASK 0x03000000
#define DWC_DDR_UMCTL2_REGS_ECCPOISONADDR0_ECC_POISON_RANK_SHIFT 24

/**** ECCPOISONADDR1 register ****/
/*
 * Row address for ECC poisoning. This is 18-bits wide in configurations with DDR4 support and
 * 16-bits in all other configurations.
 */
#define DWC_DDR_UMCTL2_REGS_ECCPOISONADDR1_ECC_POISON_ROW_MASK 0x0003FFFF
#define DWC_DDR_UMCTL2_REGS_ECCPOISONADDR1_ECC_POISON_ROW_SHIFT 0
/* Bank address for ECC poisoning */
#define DWC_DDR_UMCTL2_REGS_ECCPOISONADDR1_ECC_POISON_BANK_MASK 0x07000000
#define DWC_DDR_UMCTL2_REGS_ECCPOISONADDR1_ECC_POISON_BANK_SHIFT 24
/* Bank Group address for ECC poisoning */
#define DWC_DDR_UMCTL2_REGS_ECCPOISONADDR1_ECC_POISON_BG_MASK 0x30000000
#define DWC_DDR_UMCTL2_REGS_ECCPOISONADDR1_ECC_POISON_BG_SHIFT 28

/**** CRCPARCTL0 register ****/
/*
 * Interrupt enable bit for DFI alert error. If this bit is set, any parity/CRC error detected on
 * the dfi_alert_n input will result in an interrupt being set on CRCPARSTAT.dfi_alert_err_int.
 */
#define DWC_DDR_UMCTL2_REGS_CRCPARCTL0_DFI_ALERT_ERR_INT_EN (1 << 0)
/*
 * Interrupt clear bit for DFI alert error. If this bit is set, the alert error interrupt on
 * CRCPARSTAT.dfi_alert_err_int will be cleared. When the clear operation is complete, the uMCTL2
 * automatically clears this bit.
 */
#define DWC_DDR_UMCTL2_REGS_CRCPARCTL0_DFI_ALERT_ERR_INT_CLR (1 << 1)
/*
 * DFI alert error count clear. Clear bit for DFI alert error counter. Asserting this bit will clear
 * the DFI alert error counter, CRCPARSTAT.dfi_alert_err_cnt. When the clear operation is complete,
 * the uMCTL2 automatically clears this bit.
 */
#define DWC_DDR_UMCTL2_REGS_CRCPARCTL0_DFI_ALERT_ERR_CNT_CLR (1 << 2)
/*
 * Interrupt clear bit for dfi_alert_err_fatl_int. If this bit is set, the alert error interrupt on
 * CRCPARSTAT.dfi_alert_err_fatl_int is cleared. When the clear operation is complete, the uMCTL2
 * automatically clears this bit.
 */
#define DWC_DDR_UMCTL2_REGS_CRCPARCTL0_DFI_ALERT_ERR_FATL_INT_CLR (1 << 4)
/*
 * Interrupt clear bit for DFI alert counter saturation. If this bit is set, the alert error
 * interrupt on CRCPARSTAT.dfi_alert_err_max_reached_int is cleared. When the clear operation is
 * complete, the uMCTL2 automatically clears this bit.
 */
#define DWC_DDR_UMCTL2_REGS_CRCPARCTL0_DFI_ALERT_ERR_MAX_REACHED_INT_CLR (1 << 8)
/*
 * dfi_ctrlupd_req enable for retry.
 *	- 1: Enable to issue dfi_ctrlupd_req before starting retry sequence
 *	- 0: Disable to issue dfi_ctrlupd_req before starting retry sequence
 *   The DFI controller update can be used to reset PHY FIFO pointers.
 *
 * If both CRCPARCTL0.retry_cfrlupd_enable and CRCPARCTL1.alert_wait_for_sw are enabled,
 * dfi_ctrlupd_req is issued just before software intervention time.
 *
 * If CRCPARCTL0.retry_cfrlupd_enable is enabled, dfi_ctrlupd_req will be issued regardless of
 * DFIUPD0.dis_auto_ctrlupd.
 */
#define DWC_DDR_UMCTL2_REGS_CRCPARCTL0_RETRY_CTRLUPD_ENABLE (1 << 15)

/**** CRCPARCTL1 register ****/
/*
 * C/A Parity enable register<br>
 *	- 1: Enable generation of C/A parity and detection of C/A parity error<br>
 *	- 0: Disable generation of C/A parity and disable detection of C/A parity error<br>
 * If RCD's parity error detection or SDRAM's parity detection is enabled, this register should be
 * 1.
 */
#define DWC_DDR_UMCTL2_REGS_CRCPARCTL1_PARITY_ENABLE (1 << 0)
/*
 * CRC enable Register<br>
 *	- 1: Enable generation of CRC<br>
 *	- 0: Disable generation of CRC<br>
 *	The setting of this register should match the CRC mode register setting in the DRAM.
 */
#define DWC_DDR_UMCTL2_REGS_CRCPARCTL1_CRC_ENABLE (1 << 4)
/*
 * CRC Calculation setting register<br>
 *	- 1: CRC includes DM signal<br>
 *	- 0: CRC not includes DM signal<br>
 *	Present only in designs configured to support DDR4.
 */
#define DWC_DDR_UMCTL2_REGS_CRCPARCTL1_CRC_INC_DM (1 << 7)
/*
 *
 *	- 1: Enable command retry mechanism in case of C/A Parity or CRC error<br>
 *	- 0: Disable command retry mechanism when C/A Parity or CRC features are enabled.<br>
 * Note that retry functionality is not supported if burst chop is enabled (MSTR.burstchop = 1)
 * and/or disable auto-refresh is enabled (RFSHCTL3.dis_auto_refresh = 1)
 */
#define DWC_DDR_UMCTL2_REGS_CRCPARCTL1_CRC_PARITY_RETRY_ENABLE (1 << 8)
/*
 * After a Parity or CRC error is flagged on dfi_alert_n signal, the software has an option to read
 * the mode registers in the DRAM before the hardware begins the retry process<br>
 * - 1: Wait for software to read/write the mode registers before hardware begins the retry. After
 * software is done with its operations, it will clear the alert interrupt register bit<br>
 *	- 0: Hardware can begin the retry right away after the dfi_alert_n pulse goes away.<br>
 * The value on this register is valid only when retry is enabled (PARCTRL.crc_parity_retry_enable =
 * 1)<br><br>
 * If this register is set to 1 and if the software doesn't clear the interrupt register after
 * handling the parity/CRC error, then the hardware will not begin the retry process and the system
 * will hang.
 * In the case of Parity/CRC error, there are two possibilities when the software doesn't reset
 * MR5[4] to 0.<br>
 * - (i) If 'Persistent parity' mode register bit is NOT set: the commands sent during retry and
 * normal operation are executed without parity checking. The value in the Parity error log register
 * MPR Page 1 is valid.<br>
 * - (ii) If 'Persistent parity' mode register bit is SET: Parity checking is done for commands sent
 * during retry and normal operation. If multiple errors occur before MR5[4] is cleared, the error
 * log in MPR Page 1 should be treated as 'Don't care'.
 */
#define DWC_DDR_UMCTL2_REGS_CRCPARCTL1_ALERT_WAIT_FOR_SW (1 << 9)

#define DWC_DDR_UMCTL2_REGS_CRCPARCTL1_CAPARITY_DISABLE_BEFORE_SR (1 << 12)

#define DWC_DDR_UMCTL2_REGS_CRCPARCTL1_DFI_T_PHY_RDLAT_MASK 0x3F000000
#define DWC_DDR_UMCTL2_REGS_CRCPARCTL1_DFI_T_PHY_RDLAT_SHIFT 24

/**** CRCPARCTL2 register ****/
/*
 *
 * Indicates the maximum duration in number of Controller clock cycles for which a command should be
 * held in the Command Retry FIFO before it is popped out. Every location in the Command Retry FIFO
 * has an associated down counting timer that will use this register as the start value.
 * The down counting starts when a command is loaded into the FIFO. The timer counts down every 4
 * DRAM cycles.
 * When the counter reaches zero, the entry is popped from the FIFO. All the counters are frozen, if
 * a C/A Parity or CRC error occurs before the counter reaches zero.
 * The counter is reset to 0, after all the commands in the FIFO are retried.<br>
 *
 *	Recommended values:<br>
 *	   - Only C/A Parity is enabled.<br>
 * (PHY Command Latency(DRAM CLK) + CAL + tPAR_ALERT_PW + tPAR_UNKNOWN + PHY Alert Latency(DRAM
 * CLK)) / 4 + 1<br>
 *
 *	   - Both C/A Parity and CRC is enabled/ Only CRC is enabled.
 * (PHY Command Latency(DRAM CLK) + CAL + WL + 5 + tCRC_ALERT_ON.max + PHY Alert Latency(DRAM CLK))
 * / 4 + 1<br>
 *
 *	Max value can be set to this register is defined below: <br>
 *
 *		- UMCTL2_RETRY_CMD_FIFO_DEPTH == 40 <br>
 *			Max value = 31 <br>
 *		- UMCTL2_RETRY_CMD_FIFO_DEPTH < 40 <br>
 * Full bus Mode (CRC=OFF) Max value = UMCTL2_RETRY_CMD_FIFO_DEPTH - 1 <br>
 * Full bus Mode (CRC=ON) Max value = UMCTL2_RETRY_CMD_FIFO_DEPTH - 2 <br>
 * Half bus Mode (CRC=OFF) Max value = UMCTL2_RETRY_CMD_FIFO_DEPTH - 2 <br>
 * Half bus Mode (CRC=ON) Max value = UMCTL2_RETRY_CMD_FIFO_DEPTH - 3 <br>
 * Quarter bus Mode (CRC=OFF) Max value = UMCTL2_RETRY_CMD_FIFO_DEPTH - 4 <br>
 * Quarter bus Mode (CRC=ON) Max value = UMCTL2_RETRY_CMD_FIFO_DEPTH - 6 <br>
 *
 *	Values of 0, 1 and 2 are illegal.
 */
#define DWC_DDR_UMCTL2_REGS_CRCPARCTL2_RETRY_FIFO_MAX_HOLD_TIMER_X4_MASK 0x0000003F
#define DWC_DDR_UMCTL2_REGS_CRCPARCTL2_RETRY_FIFO_MAX_HOLD_TIMER_X4_SHIFT 0
/*
 * Value from the DRAM spec indicating the maximum width of the dfi_alert_n pulse when a CRC error
 * occurs.<br><br>
 *	Recommended values:<br>
 *	   - tCRC_ALERT_PW.MAX<br>
 * For configurations with MEMC_FREQ_RATIO=2, program this to tCRC_ALERT_PW.MAX/2 and round up to
 * next integer value.<br>
 *	   Values of 0, 1 and 2 are illegal.<br>
 *	This value must be less than CRCPARCTL2.t_par_alert_pw_max.
 *
 */
#define DWC_DDR_UMCTL2_REGS_CRCPARCTL2_T_CRC_ALERT_PW_MAX_MASK 0x00001F00
#define DWC_DDR_UMCTL2_REGS_CRCPARCTL2_T_CRC_ALERT_PW_MAX_SHIFT 8
/*
 * Value from the DRAM spec indicating the maximum width of the dfi_alert_n pulse when a parity
 * error occurs.<br><br>
 *	Recommended values:<br>
 *	   - tPAR_ALERT_PW.MAX<br>
 * For configurations with MEMC_FREQ_RATIO=2, program this to tPAR_ALERT_PW.MAX/2 and round up to
 * next integer value.<br>
 *	   Values of 0, 1 and 2 are illegal.<br>
 *	This value must be greater than CRCPARCTL2.t_crc_alert_pw_max.
 */
#define DWC_DDR_UMCTL2_REGS_CRCPARCTL2_T_PAR_ALERT_PW_MAX_MASK 0x01FF0000
#define DWC_DDR_UMCTL2_REGS_CRCPARCTL2_T_PAR_ALERT_PW_MAX_SHIFT 16

/**** CRCPARSTAT register ****/
/*
 * DFI alert error count.<br> If a parity/CRC error is detected on dfi_alert_n, this counter be
 * incremented. This is independent of the setting of CRCPARCTL0.dfi_alert_err_int_en. It will
 * saturate at 0xFFFF, and can be cleared by asserting CRCPARCTL0.dfi_alert_err_cnt_clr.
 */
#define DWC_DDR_UMCTL2_REGS_CRCPARSTAT_DFI_ALERT_ERR_CNT_MASK 0x0000FFFF
#define DWC_DDR_UMCTL2_REGS_CRCPARSTAT_DFI_ALERT_ERR_CNT_SHIFT 0
/*
 * DFI alert error interrupt.<br> If a parity/CRC error is detected on dfi_alert_n, and the
 * interrupt is enabled by CRCPARCTL0.dfi_alert_err_int_en, this interrupt bit will be set. It will
 * remain set until cleared by CRCPARCTL0.dfi_alert_err_int_clr
 */
#define DWC_DDR_UMCTL2_REGS_CRCPARSTAT_DFI_ALERT_ERR_INT (1 << 16)
/*
 *
 *	Fatal parity error interrupt.<br>
 *	One or more these situation below happens, this interrupt bit is set.
 * - PDA operation is in progress/started during retry is in control <br>
 *				- Parity error happens again during dfi_alert_err_inrt=1 <br>
 * - MRS was in retry_fifo_max_hold_timer_x4 window from alert_n=0.<br>
 *	It remains set until cleared by CRCPARCTL0.dfi_alert_err_fatl_clr.
 *	If this interrupt is asserted, system reset is strongly recommended.
 */
#define DWC_DDR_UMCTL2_REGS_CRCPARSTAT_DFI_ALERT_ERR_FATL_INT (1 << 17)
/*
 * DFI alert error counter max reached interrupt. <br> If the CRCPARSTAT.dfi_alert_err_cnt reaches
 * it maximum value, this interrupt bit is set. It remains set until cleared by
 * CRCPARCTL0.dfi_alert_err_int_clr
 */
#define DWC_DDR_UMCTL2_REGS_CRCPARSTAT_DFI_ALERT_ERR_MAX_REACHED_INT (1 << 18)
/*
 * Indicate whether software can perform MRS/MPR/PDA during software intervention time.
 * - 0 - MRS/MPR/PDA can be performed during software intervention time
 * - 1 - MRS/MPR/PDA can NOT be performed during software intervention time
 * If CRCPARCTL1.alert_wait_for_sw=1 and dfi_alert_err_no_sw=1, software can not perform MRS/MPR/PDA
 * until dfi_alert_err_int is cleared by dfi_alert_err_int_clr.<br>
 *	It remains set until cleared by CRCPARCTL0.dfi_alert_err_int_clr. <br>
 */
#define DWC_DDR_UMCTL2_REGS_CRCPARSTAT_DFI_ALERT_ERR_NO_SW (1 << 19)
/*
 * Indicate reason of dfi_alert_err_fatl_int assertion <br>
 * - [22] MPSMX caused parity error. (RCD's parity error detection only) <br>
 * - [21] Parity error happens again during software intervention time<br>
 * - [20] MRS was in retry_fifo_max_hold_timer_x4 window from alert_n=0 or STAT.operating_mode is
 * Init.<br>
 *
 *			Two or more reason can be available at the same time <br>
 *
 *	It remains set until cleared by CRCPARCTL0.dfi_alert_err_fatl_clr. <br>
 */
#define DWC_DDR_UMCTL2_REGS_CRCPARSTAT_DFI_ALERT_ERR_FATL_CODE_MASK 0x00700000
#define DWC_DDR_UMCTL2_REGS_CRCPARSTAT_DFI_ALERT_ERR_FATL_CODE_SHIFT 20
/*
 * Indicate current retry state for debug purposes only
 *				- 0000 - IDLE:Retry is not enabled
 *				- 0001 - MON_DFI:Retry is enabled and monitoring DFI
 *				- 0010 - DETECTED ALERT:Detected dfi_alert_n
 *				- 0011 - WAKE_UP:Waking up from self-refresh state
 *				- 0100 - PRECHRGING:Pre-charging banks
 * - 0101 - SEND_REF1:Sending extra REFs before software intervention time
 *				- 0110 - CTRLUPD:Issuing control update from retry logic
 *				- 0111 - WAIT_SW:Software intervention time
 *				- 1000 - SEND_REF2:Sending extra REFs before retrying commands
 *				- 1001 - RETRY_COMMANDS:Retrying commands
 * - 1010 - RESTART_RETRY:Restarting retry due to alert_n detection during retry
 *				- 1111 - FATL_ERR:Fatal error detected
 *
 *
 *
 * FOR DEBUG PURPOSE ONLY
 *
 */
#define DWC_DDR_UMCTL2_REGS_CRCPARSTAT_RETRY_CURRENT_STATE_MASK 0x0F000000
#define DWC_DDR_UMCTL2_REGS_CRCPARSTAT_RETRY_CURRENT_STATE_SHIFT 24
/*
 * Operating mode of retry
 *				- 0 - Normal
 *				- 1 - Retry due to CRC/Parity error is in progress
 */
#define DWC_DDR_UMCTL2_REGS_CRCPARSTAT_RETRY_OPERATING_MODE (1 << 28)
/*
 * Indicate if commands are in the parity/crc error window.
 *  - 0 - No command is in the parity/crc error window.
 *  - 1 - One or more commands are in the parity/crc error window or retry is in progress.
 *	This register is valid when CRCPARCTRL1.crc_parity_retry_enable=1.
 */
#define DWC_DDR_UMCTL2_REGS_CRCPARSTAT_CMD_IN_ERR_WINDOW (1 << 29)

/**** INIT0 register ****/
/*
 * Cycles to wait after reset before driving CKE high to start the SDRAM initialization
 * sequence.<br>Unit: 1024 clock cycles.<br>DDR2 specifications typically require this to be
 * programmed for a delay of >= 200 us.<br>LPDDR2/LPDDR3: t<sub>INIT1</sub> of 100 ns (min) <br> For
 * configurations with MEMC_FREQ_RATIO=2, program this to JEDEC spec value divided by 2, and round
 * it up to next integer value.
 */
#define DWC_DDR_UMCTL2_REGS_INIT0_PRE_CKE_X1024_MASK 0x00000FFF
#define DWC_DDR_UMCTL2_REGS_INIT0_PRE_CKE_X1024_SHIFT 0
/*
 * Cycles to wait after driving CKE high to start the SDRAM initialization sequence.<br>Unit: 1024
 * clocks.<br>DDR2 typically requires a 400 ns delay, requiring this value to be programmed to 2 at
 * all clock speeds.<br>LPDDR2/LPDDR3 typically requires this to be programmed for a delay of 200
 * us.<br>For configurations with MEMC_FREQ_RATIO=2, program this to JEDEC spec value divided by 2,
 * and round it up to next integer value.
 */
#define DWC_DDR_UMCTL2_REGS_INIT0_POST_CKE_X1024_MASK 0x03FF0000
#define DWC_DDR_UMCTL2_REGS_INIT0_POST_CKE_X1024_SHIFT 16
/*
 * If lower bit is enabled the SDRAM initialization routine is skipped. The upper bit decides what
 * state the controller starts up in when reset is removed
 *  - 00 - SDRAM Intialization routine is run after power-up
 * - 01 - SDRAM Intialization routine is skipped after power-up. Controller starts up in Normal Mode
 * - 11 - SDRAM Intialization routine is skipped after power-up. Controller starts up in
 * Self-refresh Mode
 *  - 10 - SDRAM Intialization routine is run after power-up.
 */
#define DWC_DDR_UMCTL2_REGS_INIT0_SKIP_DRAM_INIT_MASK 0xC0000000
#define DWC_DDR_UMCTL2_REGS_INIT0_SKIP_DRAM_INIT_SHIFT 30

/**** INIT1 register ****/
/*
 * Wait period before driving the OCD complete command to SDRAM.<br>Unit: Counts of a global timer
 * that pulses every 32 clock cycles.<br>There is no known specific requirement for this; it may be
 * set to zero.
 */
#define DWC_DDR_UMCTL2_REGS_INIT1_PRE_OCD_X32_MASK 0x0000000F
#define DWC_DDR_UMCTL2_REGS_INIT1_PRE_OCD_X32_SHIFT 0
/*
 * Cycles to wait after completing the SDRAM initialization sequence before starting the dynamic
 * scheduler.<br>Unit: Counts of a global timer that pulses every 32 clock cycles.<br>There is no
 * known specific requirement for this; it may be set to zero.
 */
#define DWC_DDR_UMCTL2_REGS_INIT1_FINAL_WAIT_X32_MASK 0x00007F00
#define DWC_DDR_UMCTL2_REGS_INIT1_FINAL_WAIT_X32_SHIFT 8
/*
 * Number of cycles to assert SDRAM reset signal during init sequence.<br>This is only present for
 * designs supporting DDR3/DDR4 devices. For use with a Synopsys DDR PHY, this should be set to a
 * minimum of 1
 */
#define DWC_DDR_UMCTL2_REGS_INIT1_DRAM_RSTN_X1024_MASK 0x01FF0000
#define DWC_DDR_UMCTL2_REGS_INIT1_DRAM_RSTN_X1024_SHIFT 16

/**** INIT3 register ****/
/*
 * DDR2: Value to write to EMR register. Bits 9:7 are for OCD and the setting in this register is
 * ignored. The uMCTL2 sets those bits appropriately.<br>DDR3/DDR4: Value to write to MR1 register
 * Set bit 7 to 0. If PHY-evaluation mode training is enabled, this bit is set appropriately by the
 * uMCTL2 during write leveling.<br> mDDR: Value to write to EMR register.<br>LPDDR2/LPDDR3 - Value
 * to write to MR2 register
 */
#define DWC_DDR_UMCTL2_REGS_INIT3_EMR_MASK 0x0000FFFF
#define DWC_DDR_UMCTL2_REGS_INIT3_EMR_SHIFT 0
/*
 * DDR2: Value to write to MR register. Bit 8 is for DLL and the setting here is ignored. The uMCTL2
 * sets this bit appropriately.<br>DDR3/DDR4: Value loaded into MR0 register.<br>mDDR: Value to
 * write to MR register.<br>LPDDR2/LPDDR3 - Value to write to MR1 register
 */
#define DWC_DDR_UMCTL2_REGS_INIT3_MR_MASK 0xFFFF0000
#define DWC_DDR_UMCTL2_REGS_INIT3_MR_SHIFT 16

/**** INIT4 register ****/
/*
 * DDR2: Value to write to EMR3 register.<br>DDR3/DDR4: Value to write to MR3
 * register<br>mDDR/LPDDR2/LPDDR3: Unused
 */
#define DWC_DDR_UMCTL2_REGS_INIT4_EMR3_MASK 0x0000FFFF
#define DWC_DDR_UMCTL2_REGS_INIT4_EMR3_SHIFT 0
/*
 * DDR2: Value to write to EMR2 register.<br>DDR3/DDR4: Value to write to MR2
 * register<br>LPDDR2/LPDDR3: Value to write to MR3 register<br>mDDR: Unused
 */
#define DWC_DDR_UMCTL2_REGS_INIT4_EMR2_MASK 0xFFFF0000
#define DWC_DDR_UMCTL2_REGS_INIT4_EMR2_SHIFT 16

/**** INIT5 register ****/
/*
 * ZQ initial calibration, t<sub>ZQINIT</sub>. Present only in designs configured to support DDR3 or
 * DDR4 or LPDDR2/LPDDR3.<br>Unit: 32 clock cycles.<br>DDR3 typically requires 512 clocks.<br>DDR4
 * requires 1024 clocks.<br>LPDDR2/LPDDR3 requires 1 us.
 */
#define DWC_DDR_UMCTL2_REGS_INIT5_DEV_ZQINIT_X32_MASK 0x00FF0000
#define DWC_DDR_UMCTL2_REGS_INIT5_DEV_ZQINIT_X32_SHIFT 16

/**** INIT6 register ****/
/* DDR4- Value to be loaded into SDRAM MR5 registers.<br>Used in DDR4 designs only. */
#define DWC_DDR_UMCTL2_REGS_INIT6_MR5_MASK 0x0000FFFF
#define DWC_DDR_UMCTL2_REGS_INIT6_MR5_SHIFT 0
/* DDR4- Value to be loaded into SDRAM MR4 registers.<br>Used in DDR4 designs only. */
#define DWC_DDR_UMCTL2_REGS_INIT6_MR4_MASK 0xFFFF0000
#define DWC_DDR_UMCTL2_REGS_INIT6_MR4_SHIFT 16

/**** INIT7 register ****/
/* DDR4- Value to be loaded into SDRAM MR6 registers.<br>Used in DDR4 designs only. */
#define DWC_DDR_UMCTL2_REGS_INIT7_MR6_MASK 0xFFFF0000
#define DWC_DDR_UMCTL2_REGS_INIT7_MR6_SHIFT 16

/**** DIMMCTL register ****/
/*
 * Staggering enable for multi-rank accesses (for multi-rank UDIMM and RDIMM implementations only).
 * This is not supported for mDDR, LPDDR2 or LPDDR3 SDRAMs.<br>Note: Even if this bit is set it does
 * not take care of software driven MR commands (via MRCTRL0/MRCTRL1), where software is responsible
 * to send them to seperate ranks as appropriate.
 *	- 1 - (DDR4) Send MRS commands to each ranks seperately
 *	- 1 - (non-DDR4) Send all commands to even and odd ranks seperately
 *	- 0 - Do not stagger accesses
 */
#define DWC_DDR_UMCTL2_REGS_DIMMCTL_DIMM_STAGGER_CS_EN (1 << 0)
/*
 * Address Mirroring Enable (for multi-rank UDIMM implementations and multi-rank DDR4 RDIMM
 * implementations).<br>Some UDIMMs and DDR4 RDIMMs implement address mirroring for odd ranks, which
 * means that the following address, bank address and bank group bits are swapped: (A3, A4), (A5,
 * A6), (A7, A8), (BA0, BA1) and also (A11, A13), (BG0, BG1) for the DDR4. Setting this bit ensures
 * that, for mode register accesses during the automatic initialization routine, these bits are
 * swapped within the uMCTL2 to compensate for this UDIMM/RDIMM swapping. In addition to the
 * automatic initialization routine, in case of DDR4 UDIMM/RDIMM, they are swapped during the
 * automatic MRS access to enable/disable of a particular DDR4 feature.<br>Note: This has no effect
 * on the address of any other memory accesses, or of software-driven mode register
 * accesses.<br>This is not supported for mDDR, LPDDR2 or LPDDR3 SDRAMs.<br>Note: In case of x16
 * DDR4 DIMMs, BG1 output of MRS for the odd ranks is same as BG0 because BG1 is invalid, hence
 * dimm_dis_bg_mirroring register must be set to 1.
 * - 1 - For odd ranks, implement address mirroring for MRS commands to during initialization and
 * for any automatic DDR4 MRS commands (to be used if UDIMM/RDIMM implements address mirroring)
 *	- 0 - Do not implement address mirroring
 */
#define DWC_DDR_UMCTL2_REGS_DIMMCTL_DIMM_ADDR_MIRR_EN (1 << 1)
/*
 * Output Inversion Enable (for DDR4 RDIMM implementations only).<br>DDR4 RDIMM implements the
 * Output Inversion feature by default, which means that the following address, bank address and
 * bank group bits of B-side DRAMs are inverted: A3-A9, A11, A13, A17, BA0-BA1, BG0-BG1. Setting
 * this bit ensures that, for mode register accesses generated by the uMCTL2 during the automatic
 * initialization routine and enabling of a particular DDR4 feature, separate A-side and B-side mode
 * register accesses are generated. For B-side mode register accesses, these bits are inverted
 * within the uMCTL2 to compensate for this RDIMM inversion.<br>Note: This has no effect on the
 * address of any other memory accesses, or of software-driven mode register accesses.
 *	- 1 - Implement output inversion for B-side DRAMs.
 *	- 0 - Do not implement output inversion for B-side DRAMs.
 */
#define DWC_DDR_UMCTL2_REGS_DIMMCTL_DIMM_OUTPUT_INV_EN (1 << 2)
/*
 * Enable for A17 bit of MRS command.<br>A17 bit of the mode register address is specified as RFU
 * (Reserved for Future Use) and must be programmed to 0 during MRS. In case where DRAMs which do
 * not have A17 are attached and the Output Inversion are enabled, this must be set to 0, so that
 * the calculation of CA parity will not include A17 bit.<br>Note: This has no effect on the address
 * of any other memory accesses, or of software-driven mode register accesses.
 *	- 1 - Enabled
 *	- 0 - Disabled
 */
#define DWC_DDR_UMCTL2_REGS_DIMMCTL_MRS_A17_EN (1 << 3)
/*
 * Enable for BG1 bit of MRS command.<br>BG1 bit of the mode register address is specified as RFU
 * (Reserved for Future Use) and must be programmed to 0 during MRS. In case where DRAMs which do
 * not have BG1 are attached and both the CA parity and the Output Inversion are enabled, this must
 * be set to 0, so that the calculation of CA parity will not include BG1 bit.<br>Note: This has no
 * effect on the address of any other memory accesses, or of software-driven mode register
 * accesses.<br>If address mirroring is enabled, this is applied to BG1 of even ranks and BG0 of odd
 * ranks.
 *	- 1 - Enabled
 *	- 0 - Disabled
 */
#define DWC_DDR_UMCTL2_REGS_DIMMCTL_MRS_BG1_EN (1 << 4)
/*
 * Disabling Address Mirroring for BG bits. When this is set to 1, BG0 and BG1 are NOT swapped even
 * if Address Mirroring is enabled. This will be required for DDR4 DIMMs with x16 devices.
 *	- 1 - BG0 and BG1 are NOT swapped.
 *	- 0 - BG0 and BG1 are swapped if address mirroring is enabled.
 */
#define DWC_DDR_UMCTL2_REGS_DIMMCTL_DIMM_DIS_BG_MIRRORING (1 << 5)
#define DWC_DDR_UMCTL2_REGS_DIMMCTL_LRDIMM_BCOM_CMD_PROT (1 << 6)

/**** RANKCTL register ****/
/*
 * Only present for multi-rank configurations.<br>Background: Reads to the same rank can be
 * performed back-to-back. Reads to different ranks require additional gap dictated by the register
 * RANKCTL.diff_rank_rd_gap. This is to avoid possible data bus contention as well as to give PHY
 * enough time to switch the delay when changing ranks. The uMCTL2 arbitrates for bus access on a
 * cycle-by-cycle basis; therefore after a read is scheduled, there are few clock cycles (determined
 * by the value on RANKCTL.diff_rank_rd_gap register) in which only reads from the same rank are
 * eligible to be scheduled. This prevents reads from other ranks from having fair access to the
 * data bus.<br>This parameter represents the maximum number of reads that can be scheduled
 * consecutively to the same rank. After this number is reached, a delay equal to
 * RANKCTL.diff_rank_rd_gap is inserted by the scheduler to allow all ranks a fair opportunity to be
 * scheduled. Higher numbers increase bandwidth utilization, lower numbers increase
 * fairness.<br>This feature can be DISABLED by setting this register to 0. When set to 0, the
 * Controller will stay on the same rank as long as commands are available for it.<br>Minimum
 * programmable value is 0 (feature disabled) and maximum programmable value is 0xF.<br>FOR
 * PERFORMANCE ONLY.
 */
#define DWC_DDR_UMCTL2_REGS_RANKCTL_MAX_RANK_RD_MASK 0x0000000F
#define DWC_DDR_UMCTL2_REGS_RANKCTL_MAX_RANK_RD_SHIFT 0
/*
 * Only present for multi-rank configurations.<br>Indicates the number of clocks of gap in data
 * responses when performing consecutive reads to different ranks.<br>This is used to switch the
 * delays in the PHY to match the rank requirements.<br>The value programmed in this register takes
 * care of the ODT switch off timing requirement when switching ranks during reads.<br>For
 * configurations with MEMC_FREQ_RATIO=2, program this to (N/2) and round it up to the next integer
 * value. N is value required by PHY, in terms of PHY clocks. <br> If read preamble is set to
 * 2tCK(DDR4 only), should be increased by 1.<br> For configurations with MEMC_FREQ_RATIO=1, program
 * this to N+1.<br> For configurations with MEMC_FREQ_RATIO=2, program this to ((N+1)/2) and round
 * it up to the next integer value.
 */
#define DWC_DDR_UMCTL2_REGS_RANKCTL_DIFF_RANK_RD_GAP_MASK 0x000000F0
#define DWC_DDR_UMCTL2_REGS_RANKCTL_DIFF_RANK_RD_GAP_SHIFT 4
/*
 * Only present for multi-rank configurations.<br>Indicates the number of clocks of gap in data
 * responses when performing consecutive writes to different ranks.<br>This is used to switch the
 * delays in the PHY to match the rank requirements.<br>The value programmed in this register takes
 * care of the ODT switch off timing requirement when switching ranks during writes.<br>For
 * configurations with MEMC_FREQ_RATIO=2, program this to (N/2) and round it up to the next integer
 * value. N is value required by PHY, in terms of PHY clocks. <br>If write preamble is set to
 * 2tCK(DDR4 only), should be increased by 1.<br> For configurations with MEMC_FREQ_RATIO=1, program
 * this to N+1.<br> For configurations with MEMC_FREQ_RATIO=2, program this to ((N+1)/2) and round
 * it up to the next integer value.
 */
#define DWC_DDR_UMCTL2_REGS_RANKCTL_DIFF_RANK_WR_GAP_MASK 0x00000F00
#define DWC_DDR_UMCTL2_REGS_RANKCTL_DIFF_RANK_WR_GAP_SHIFT 8

#define DWC_DDR_UMCTL2_REGS_RANKCTL_MAX_LOGICAL_RANK_RD_MASK 0x000F0000
#define DWC_DDR_UMCTL2_REGS_RANKCTL_MAX_LOGICAL_RANK_RD_SHIFT 16

/**** DRAMTMG0 register ****/
/*
 * t<sub>RAS</sub>(min): Minimum time between activate and precharge to the same bank.<br>For
 * configurations with MEMC_FREQ_RATIO=2, 1T mode, program this to t<sub>RAS</sub>(min)/2. No
 * rounding up.<br>For configurations with MEMC_FREQ_RATIO=2, 2T mode, program this to
 * (t<sub>RAS</sub>(min)/2 + 1). No rounding up of the division operation.<br>Unit: Clocks
 */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG0_T_RAS_MIN_MASK 0x0000003F
#define DWC_DDR_UMCTL2_REGS_DRAMTMG0_T_RAS_MIN_SHIFT 0
/*
 * t<sub>RAS</sub>(max): Maximum time between activate and precharge to same bank. This is the
 * maximum time that a page can be kept open<br>Minimum value of this register is 1. Zero is
 * invalid.<br>For configurations with MEMC_FREQ_RATIO=2, program this to
 * (t<sub>RAS</sub>(max)-1)/2. No rounding up. <br>Unit: Multiples of 1024 clocks.
 */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG0_T_RAS_MAX_MASK 0x00007F00
#define DWC_DDR_UMCTL2_REGS_DRAMTMG0_T_RAS_MAX_SHIFT 8
/*
 * t<sub>FAW</sub> Valid only when 8 or more banks(or banks x bank groups) are present.<br>In 8-bank
 * design, at most 4 banks must be activated in a rolling window of t<sub>FAW</sub> cycles.<br>For
 * configurations with MEMC_FREQ_RATIO=2, program this to (t<sub>FAW</sub>/2) and round up to next
 * integer value.<br>In a 4-bank design, set this register to 0x1 independent of the MEMC_FREQ_RATIO
 * configuration.<br>Unit: Clocks
 */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG0_T_FAW_MASK 0x003F0000
#define DWC_DDR_UMCTL2_REGS_DRAMTMG0_T_FAW_SHIFT 16
/*
 *
 * Minimum time between write and precharge to same bank. <br>Unit: Clocks<br>Specifications: WL +
 * BL/2 + t<sub>WR</sub> = approximately 8 cycles + 15 ns = 14 clocks @400MHz and less for lower
 * frequencies<br>where:
 *  - WL = write latency
 * - BL = burst length. This must match the value programmed in the BL bit of the mode register to
 * the SDRAM. BST (burst terminate) is not supported at present.
 *  - t<sub>WR</sub> = Write recovery time. This comes directly from the SDRAM specification.
 * Add one extra cycle for LPDDR2/LPDDR3 for this parameter.<br>For configurations with
 * MEMC_FREQ_RATIO=2, 1T mode, divide the above value by 2. No rounding up.<br>For configurations
 * with MEMC_FREQ_RATIO=2, 2T mode, divide the above value by 2 and add 1. No rounding up.
 */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG0_WR2PRE_MASK 0x7F000000
#define DWC_DDR_UMCTL2_REGS_DRAMTMG0_WR2PRE_SHIFT 24

/**** DRAMTMG1 register ****/
/*
 * t<sub>RC</sub>: Minimum time between activates to same bank.<br>For configurations with
 * MEMC_FREQ_RATIO=2, program this to (t<sub>RC</sub>/2) and round up to next integer
 * value.<br>Unit: Clocks.
 */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG1_T_RC_MASK 0x0000007F
#define DWC_DDR_UMCTL2_REGS_DRAMTMG1_T_RC_SHIFT 0
/*
 * t<sub>RTP</sub>:  Minimum time from read to precharge of same bank.
 *  - DDR2: t<sub>AL</sub> + BL/2 + max(t<sub>RTP</sub>, 2) - 2
 *  - DDR3: t<sub>AL</sub> + max (t<sub>RTP</sub>, 4)
 *  - DDR4: Max of following two equations:
 *		   t<sub>AL</sub> + max (t<sub>RTP</sub>, 4) or,
 *		   RL + BL/2 - t<sub>RP</sub>.
 *  - mDDR: BL/2
 *  - LPDDR2: Depends on if it's LPDDR2-S2 or LPDDR2-S4:
 *		   LPDDR2-S2: BL/2 + t<sub>RTP</sub> - 1.
 *		   LPDDR2-S4: BL/2 + max(t<sub>RTP</sub>,2) - 2.
 *  - LPDDR3: BL/2 +  max(t<sub>RTP</sub>,4) - 4
 * For configurations with MEMC_FREQ_RATIO=2, 1T mode, divide the above value by 2. No rounding up.
 * <br>For configurations with MEMC_FREQ_RATIO=2, 2T mode, divide the above value by 2 and add 1. No
 * rounding up of division operation. <br>Unit: Clocks.
 */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG1_RD2PRE_MASK 0x00003F00
#define DWC_DDR_UMCTL2_REGS_DRAMTMG1_RD2PRE_SHIFT 8
/*
 * t<sub>XP</sub>: Minimum time after power-down exit to any operation. For DDR3, this should be
 * programmed to t<sub>XPDLL</sub> if slow powerdown exit is selected in MR0[12]. <br>If C/A parity
 * for DDR4 is used, set to (t<sub>XP</sub>+PL) instead. <br>For configurations with
 * MEMC_FREQ_RATIO=2, program this to (t<sub>XP</sub>/2) and round it up to the next integer
 * value.<br>Units: Clocks
 */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG1_T_XP_MASK 0x001F0000
#define DWC_DDR_UMCTL2_REGS_DRAMTMG1_T_XP_SHIFT 16

/**** DRAMTMG2 register ****/
/*
 *
 * DDR4: CWL + PL + BL/2 + t<sub>WTR_L</sub><br>Others: CWL + BL/2 + t<sub>WTR</sub><br>In DDR4,
 * minimum time from write command to read command for same bank group. In others, minimum time from
 * write command to read command. Includes time for bus turnaround, recovery times, and all
 * per-bank, per-rank, and global constraints. <br>Unit: Clocks.<br>Where:
 *  - CWL = CAS write latency
 *  - PL = Parity latency
 * - BL = burst length. This must match the value programmed in the BL bit of the mode register to
 * the SDRAM
 * - t<sub>WTR_L</sub> = internal write to read command delay for same bank group. This comes
 * directly from the SDRAM specification.
 * - t<sub>WTR</sub> = internal write to read command delay. This comes directly from the SDRAM
 * specification.
 * Add one extra cycle for LPDDR2/LPDDR3 operation. <br><br>For configurations with
 * MEMC_FREQ_RATIO=2, divide the value calculated using the above equation by 2, and round it up to
 * next integer.
 */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG2_WR2RD_MASK 0x0000003F
#define DWC_DDR_UMCTL2_REGS_DRAMTMG2_WR2RD_SHIFT 0
/*
 *
 * DDR2/3/mDDR: RL + BL/2 + 2 - WL<br>DDR4: RL + BL/2 + 1 + WR_PREAMBLE - WL<br>LPDDR2/LPDDR3: RL +
 * BL/2 + RU(t<sub>DQSCK</sub>max/t<sub>CK</sub>) + 1 - WL.<br>Minimum time from read command to
 * write command. Include time for bus turnaround and all per-bank, per-rank, and global
 * constraints.<br>Unit: Clocks.<br>Where:
 *  - WL = write latency
 * - BL = burst length. This must match the value programmed in the BL bit of the mode register to
 * the SDRAM
 *  - RL = read latency = CAS latency
 *  - WR_PREAMBLE = write preamble. This is unique to DDR4.
 * For configurations with MEMC_FREQ_RATIO=2, divide the value calculated using the above equation
 * by 2, and round it up to next integer.
 */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG2_RD2WR_MASK 0x00003F00
#define DWC_DDR_UMCTL2_REGS_DRAMTMG2_RD2WR_SHIFT 8
/*
 * Set to RL<br>Time from read command to read data on SDRAM interface. This must be set to RL. Note
 * that, depending on the PHY, if using RDIMM, it mat be necessary to use a value of RL + 1 to
 * compensate for the extra cycle of latency through the RDIMM<br>For configurations with
 * MEMC_FREQ_RATIO=2, divide the value calculated using the above equation by 2, and round it up to
 * next integer.<br>This register field is not required for DDR2 and DDR3 (except if MEMC_TRAINING
 * is set), as the DFI read and write latencies defined in DFITMG0 and DFITMG1 are sufficient for
 * those protocols<br>Unit: clocks
 */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG2_READ_LATENCY_MASK 0x003F0000
#define DWC_DDR_UMCTL2_REGS_DRAMTMG2_READ_LATENCY_SHIFT 16
/*
 * Set to WL<br>Time from write command to write data on SDRAM interface. This must be set to
 * WL.<br>For mDDR, it should normally be set to 1.<br>Note that, depending on the PHY, if using
 * RDIMM, it may be necessary to use a value of WL + 1 to compensate for the extra cycle of latency
 * through the RDIMM<br>For configurations with MEMC_FREQ_RATIO=2, divide the value calculated using
 * the above equation by 2, and round it up to next integer.<br>This register field is not required
 * for DDR2 and DDR3 (except if MEMC_TRAINING is set), as the DFI read and write latencies defined
 * in DFITMG0 and DFITMG1 are sufficient for those protocols<br>Unit: clocks
 */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG2_WRITE_LATENCY_MASK 0x3F000000
#define DWC_DDR_UMCTL2_REGS_DRAMTMG2_WRITE_LATENCY_SHIFT 24

/**** DRAMTMG3 register ****/
/*
 * t<sub>MOD</sub>: Present if MEMC_DDR3_OR_4 = 1. Cycles between load mode command and following
 * non-load mode command. This is required to be programmed even when a design that supports DDR3/4
 * is running in DDR2 mode.<br>If C/A parity for DDR4 is used, set to
 * t<sub>MOD_PAR</sub>(t<sub>MOD</sub>+PL) instead<br>Set to t<sub>MOD</sub> if MEMC_FREQ_RATIO=1,
 * or t<sub>MOD</sub>/2 (rounded up to next integer) if MEMC_FREQ_RATIO=2. Note that if using RDIMM,
 * depending on the PHY, it may be necessary to use a value of t<sub>MOD</sub> + 1 or
 * (t<sub>MOD</sub> + 1)/2 to compensate for the extra cycle of latency applied to mode register
 * writes by the RDIMM chip
 */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG3_T_MOD_MASK 0x000003FF
#define DWC_DDR_UMCTL2_REGS_DRAMTMG3_T_MOD_SHIFT 0
/*
 * t<sub>MRD</sub>: Cycles between load mode commands. If MEMC_DDR3_OR_4 = 0, this parameter is also
 * used to define the cycles between load mode command and following non-load mode command.<br>For
 * configurations with MEMC_FREQ_RATIO=2, program this to (t<sub>MRD</sub>/2) and round it up to the
 * next integer value.<br>If C/A parity for DDR4 is used, set to
 * t<sub>MRD_PAR</sub>(t<sub>MOD</sub>+PL) instead
 */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG3_T_MRD_MASK 0x0003F000
#define DWC_DDR_UMCTL2_REGS_DRAMTMG3_T_MRD_SHIFT 12

/**** DRAMTMG4 register ****/
/*
 * t<sub>RP</sub>:  Minimum time from precharge to activate of same bank.<br>
 * For MEMC_FREQ_RATIO=1 configurations, t_rp should be set to
 * RoundUp(t<sub>RP</sub>/t<sub>CK</sub>).<br>
 * For MEMC_FREQ_RATIO=2 configurations, t_rp should be set to
 * RoundDown(RoundUp(t<sub>RP</sub>/t<sub>CK</sub>)/2) + 1.<br>
 *	<br>Unit: Clocks.
 */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG4_T_RP_MASK 0x0000001F
#define DWC_DDR_UMCTL2_REGS_DRAMTMG4_T_RP_SHIFT 0
/*
 * DDR4: t<sub>RRD_L</sub>: Minimum time between activates from bank "a" to bank "b" for same bank
 * group.<br>Others: t<sub>RRD</sub>: Minimum time between activates from bank "a" to bank
 * "b"<br>For configurations with MEMC_FREQ_RATIO=2, program this to (t<sub>RRD_L</sub>/2 or
 * t<sub>RRD</sub>/2) and round it up to the next integer value.<br>Unit: Clocks.
 */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG4_T_RRD_MASK 0x00000F00
#define DWC_DDR_UMCTL2_REGS_DRAMTMG4_T_RRD_SHIFT 8
/*
 * DDR4: t<sub>CCD_L</sub>: This is the minimum time between two reads or two writes for same bank
 * group.<br>Others: t<sub>CCD</sub>: This is the minimum time between two reads or two
 * writes.<br>For configurations with MEMC_FREQ_RATIO=2, program this to (t<sub>CCD_L</sub>/2 or
 * t<sub>CCD</sub>/2) and round it up to the next integer value.<br>Unit: clocks.
 */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG4_T_CCD_MASK 0x000F0000
#define DWC_DDR_UMCTL2_REGS_DRAMTMG4_T_CCD_SHIFT 16
/*
 * t<sub>RCD</sub> - t<sub>AL</sub>: Minimum time from activate to read or write command to same
 * bank.<br>For configurations with MEMC_FREQ_RATIO=2, program this to ((t<sub>RCD</sub> -
 * t<sub>AL</sub>)/2) and round it up to the next integer value. <br>Minimum value allowed for this
 * register is 1, which implies minimum (t<sub>RCD</sub> - t<sub>AL</sub>) value to be 2 in
 * configurations with MEMC_FREQ_RATIO=2. <br>Unit: Clocks.
 */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG4_T_RCD_MASK 0x1F000000
#define DWC_DDR_UMCTL2_REGS_DRAMTMG4_T_RCD_SHIFT 24

/**** DRAMTMG5 register ****/
/*
 * Minimum number of cycles of CKE HIGH/LOW during power-down and self refresh.
 *  - LPDDR2/LPDDR3 mode: Set this to the larger of t<sub>CKE</sub> or t<sub>CKESR</sub>
 *  - Non-LPDDR2/non-LPDDR3 designs: Set this to t<sub>CKE</sub> value.
 * For configurations with MEMC_FREQ_RATIO=2, program this to (value described above)/2 and round it
 * up to the next integer value. <br>Unit: Clocks.
 */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG5_T_CKE_MASK 0x0000001F
#define DWC_DDR_UMCTL2_REGS_DRAMTMG5_T_CKE_SHIFT 0
/*
 * Minimum CKE low width for Self refresh entry to exit timing in memory clock cycles. <br>
 * Recommended settings:
 *	- mDDR: tRFC
 *	- LPDDR2: tCKESR
 *	- LPDDR3: tCKESR
 *	- LPDDR4: max(tCKELPD, tSR)
 *	- DDR2: tCKE
 *	- DDR3: tCKE + 1
 *	- DDR4: tCKE + 1
 * For configurations with MEMC_FREQ_RATIO=2, program this to recommended value divided by two and
 * round it up to next integer.
 */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG5_T_CKESR_MASK 0x00003F00
#define DWC_DDR_UMCTL2_REGS_DRAMTMG5_T_CKESR_SHIFT 8
/*
 * This is the time after Self Refresh Down Entry that CK is maintained as a valid clock. Specifies
 * the clock disable delay after SRE. <br>
 *    Recommended settings:
 *	- mDDR: 0
 *	- LPDDR2: 2
 *	- LPDDR3: 2
 *	- DDR2: 1
 *	- DDR3: max (10 ns, 5 tCK)
 *	- DDR4: max (10 ns, 5 tCK)
 * For configurations with MEMC_FREQ_RATIO=2, program this to recommended value divided by two and
 * round it up to next integer.
 */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG5_T_CKSRE_MASK 0x000F0000
#define DWC_DDR_UMCTL2_REGS_DRAMTMG5_T_CKSRE_SHIFT 16
/*
 * This is the time before Self Refresh Exit that CK is maintained as a valid clock before issuing
 * SRX. Specifies the clock stable time before SRX. <br>
 *	Recommended settings:
 *	- mDDR: 1
 *	- LPDDR2: 2
 *	- LPDDR3: 2
 *	- DDR2: 1
 *	- DDR3: tCKSRX
 *	- DDR4: tCKSRX
 * For configurations with MEMC_FREQ_RATIO=2, program this to recommended value divided by two and
 * round it up to next integer.
 */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG5_T_CKSRX_MASK 0x0F000000
#define DWC_DDR_UMCTL2_REGS_DRAMTMG5_T_CKSRX_SHIFT 24

/**** DRAMTMG8 register ****/
/* Minimum time to wait after coming out of self refresh before  ... */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG8_POST_SELFREF_GAP_X32_MASK 0x0000007F
#define DWC_DDR_UMCTL2_REGS_DRAMTMG8_POST_SELFREF_GAP_X32_SHIFT 0

/*
 * t<sub>XS</sub>: Exit Self Refresh to commands not requiring a locked DLL.<br>For configurations
 * with MEMC_FREQ_RATIO=2, program this to the above value divided by 2 and round up to next integer
 * value.<br>Unit: Multiples of 32 clocks.<br>Note: In LPDDR2/LPDDR3/Mobile DDR mode, t_xs_x32 and
 * t_xs_dll_x32 must be set the same values derived from tXSR.
 */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG8_T_XS_X32_MASK 0x0000007F
#define DWC_DDR_UMCTL2_REGS_DRAMTMG8_T_XS_X32_SHIFT 0
/*
 * t<sub>XSDLL</sub>: Exit Self Refresh to commands requiring a locked DLL.<br>For configurations
 * with MEMC_FREQ_RATIO=2, program this to the above value divided by 2 and round up to next integer
 * value.<br>Unit: Multiples of 32 clocks.<br>Note: In LPDDR2/LPDDR3/Mobile DDR mode, t_xs_x32 and
 * t_xs_dll_x32 must be set the same values derived from tXSR.
 */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG8_T_XS_DLL_X32_MASK 0x00007F00
#define DWC_DDR_UMCTL2_REGS_DRAMTMG8_T_XS_DLL_X32_SHIFT 8
/*
 * t<sub>XS_ABORT</sub>: Exit Self Refresh to commands not requiring a locked DLL in Self Refresh
 * Abort.<br>For configurations with MEMC_FREQ_RATIO=2, program this to the above value divided by 2
 * and round up to next integer value.<br>Unit: Multiples of 32 clocks.<br>Note: Ensure this is less
 * than or equal to t_xs_x32.
 */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG8_T_XS_ABORT_X32_MASK 0x007F0000
#define DWC_DDR_UMCTL2_REGS_DRAMTMG8_T_XS_ABORT_X32_SHIFT 16
/*
 * t<sub>XS_FAST</sub>: Exit Self Refresh to ZQCL, ZQCS and MRS (only CL, WR, RTP and Geardown
 * mode).<br>For configurations with MEMC_FREQ_RATIO=2, program this to the above value divided by 2
 * and round up to next integer value.<br>Unit: Multiples of 32 clocks.<br>Note: This is applicable
 * to only ZQCL/ZQCS commands. <br>Note: Ensure this is less than or equal to t_xs_x32.
 */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG8_T_XS_FAST_X32_MASK 0x7F000000
#define DWC_DDR_UMCTL2_REGS_DRAMTMG8_T_XS_FAST_X32_SHIFT 24

/**** DRAMTMG9 register ****/
/*
 *
 * CWL + PL + BL/2 + t<sub>WTR_S</sub><br>Minimum time from write command to read command for
 * different bank group. Includes time for bus turnaround, recovery times, and all per-bank,
 * per-rank, and global constraints.<br>Present only in designs configured to support DDR4.<br>Unit:
 * Clocks.<br>Where:
 *  - CWL = CAS write latency
 *  - PL = Parity latency
 * - BL = burst length. This must match the value programmed in the BL bit of the mode register to
 * the SDRAM
 * - t<sub>WTR_S</sub> = internal write to read command delay for different bank group. This comes
 * directly from the SDRAM specification.
 * <br><br>For configurations with MEMC_FREQ_RATIO=2, divide the value calculated using the above
 * equation by 2, and round it up to next integer.
 */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG9_WR2RD_S_MASK 0x0000003F
#define DWC_DDR_UMCTL2_REGS_DRAMTMG9_WR2RD_S_SHIFT 0
/*
 * t<sub>RRD_S</sub>: Minimum time between activates from bank "a" to bank "b" for different bank
 * group.<br>For configurations with MEMC_FREQ_RATIO=2, program this to (t<sub>RRD_S</sub>/2) and
 * round it up to the next integer value.<br>Present only in designs configured to support
 * DDR4.<br>Unit: Clocks.
 */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG9_T_RRD_S_MASK 0x00000F00
#define DWC_DDR_UMCTL2_REGS_DRAMTMG9_T_RRD_S_SHIFT 8
/*
 * t<sub>CCD_S</sub>: This is the minimum time between two reads or two writes for different bank
 * group. For bank switching (from bank "a" to bank "b"), the minimum time is this value + 1.<br>For
 * configurations with MEMC_FREQ_RATIO=2, program this to (t<sub>CCD_S</sub>/2) and round it up to
 * the next integer value.<br>Present only in designs configured to support DDR4.<br>Unit: clocks.
 */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG9_T_CCD_S_MASK 0x00070000
#define DWC_DDR_UMCTL2_REGS_DRAMTMG9_T_CCD_S_SHIFT 16

#define DWC_DDR_UMCTL2_REGS_DRAMTMG9_DDR4_WR_PREAMBLE (1 << 30)

/**** DRAMTMG10 register ****/
/*
 * Geardown hold time.<br>For DDR4-2666 and DDR4-3200, this parameter is defined as 2 clks<br>For
 * configurations with MEMC_FREQ_RATIO=2, program this to (tGEAR_hold/2) and round it up to the next
 * integer value.
 */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG10_T_GEAR_HOLD_MASK 0x00000003
#define DWC_DDR_UMCTL2_REGS_DRAMTMG10_T_GEAR_HOLD_SHIFT 0
/*
 * Geardown setup time.<br>For DDR4-2666 and DDR4-3200, this parameter is defined as 2 clks<br>For
 * configurations with MEMC_FREQ_RATIO=2, program this to (tGEAR_setup/2) and round it up to the
 * next integer value.
 */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG10_T_GEAR_SETUP_MASK 0x0000000C
#define DWC_DDR_UMCTL2_REGS_DRAMTMG10_T_GEAR_SETUP_SHIFT 2
/*
 * Sync pulse to first valid command.<br><br>For DDR4-2666 and DDR4-3200, this parameter is defined
 * as tMOD(min)<br><br>tMOD(min) is greater of 12nCK or 15ns<br>15ns / .625ns = 24<br><br>Max value
 * for this register is 24<br>For configurations with MEMC_FREQ_RATIO=2, program this to
 * (tCMD_GEAR/2) and round it up to the next integer value.
 */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG10_T_CMD_GEAR_MASK 0x00001F00
#define DWC_DDR_UMCTL2_REGS_DRAMTMG10_T_CMD_GEAR_SHIFT 8
/*
 * Indicates the time between MRS command and the sync pulse time.<br><br>This must be even number
 * of clocks.<br><br>For DDR4-2666 and DDR4-3200, this parameter is defined as
 * tMOD(min)+4nCK<br><br>tMOD(min) is greater of 12nCK or 15ns<br><br>15ns / .625ns = 24<br>Max
 * value for this register is 24+4 = 28<br>For configurations with MEMC_FREQ_RATIO=2, program this
 * to (tSYNC_GEAR/2) and round it up to the next integer value.
 */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG10_T_SYNC_GEAR_MASK 0x001F0000
#define DWC_DDR_UMCTL2_REGS_DRAMTMG10_T_SYNC_GEAR_SHIFT 16

/**** DRAMTMG11 register ****/
/*
 * t<sub>CKMPE</sub>: Minimum valid clock requirement after MPSM entry.<br>Present only in designs
 * configured to support DDR4.<br>Unit: Clocks.<br>
 * For configurations with MEMC_FREQ_RATIO=2, divide the value calculated using the above equation
 * by 2, and round it up to next integer.
 */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG11_T_CKMPE_MASK 0x0000001F
#define DWC_DDR_UMCTL2_REGS_DRAMTMG11_T_CKMPE_SHIFT 0
/*
 * t<sub>MPX_S</sub>: Minimum time CS setup time to CKE.<br>For configurations with
 * MEMC_FREQ_RATIO=2, program this to (t<sub>MPX_S</sub>/2) and round it up to the next integer
 * value.<br>Present only in designs configured to support DDR4.<br>Unit: Clocks.
 */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG11_T_MPX_S_MASK 0x00000300
#define DWC_DDR_UMCTL2_REGS_DRAMTMG11_T_MPX_S_SHIFT 8
/*
 * t<sub>MPX_LH</sub>: This is the minimum CS_n Low hold time to CKE rising edge.<br>For
 * configurations with MEMC_FREQ_RATIO=2, program this to (t<sub>MPX_LH</sub>/2) and round it up to
 * the next integer value.<br>Present only in designs configured to support DDR4.<br>Unit: clocks.
 */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG11_T_MPX_LH_MASK 0x001F0000
#define DWC_DDR_UMCTL2_REGS_DRAMTMG11_T_MPX_LH_SHIFT 16
/*
 * t<sub>XMPDLL</sub>: This is the minimum Exit MPSM to commands requiring a locked DLL.<br>For
 * configurations with MEMC_FREQ_RATIO=2, program this to (t<sub>XMPDLL</sub>/2) and round it up to
 * the next integer value.<br>Present only in designs configured to support DDR4.<br>Unit: Multiples
 * of 32 clocks.
 */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG11_POST_MPSM_GAP_X32_MASK 0x7F000000
#define DWC_DDR_UMCTL2_REGS_DRAMTMG11_POST_MPSM_GAP_X32_SHIFT 24

/**** DRAMTMG12 register ****/
/*
 * t<sub>MRD_PDA</sub>: This is the Mode Register Set command cycle time in PDA mode.<br>For
 * configurations with MEMC_FREQ_RATIO=2, program this to (tMRD_PDA/2) and round it up to next
 * integer value.
 */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG12_T_MRD_PDA_MASK 0x0000001F
#define DWC_DDR_UMCTL2_REGS_DRAMTMG12_T_MRD_PDA_SHIFT 0

/**** DRAMTMG15 register ****/
#define DWC_DDR_UMCTL2_REGS_DRAMTMG15_T_STAB_X32_MASK 0x000000FF
#define DWC_DDR_UMCTL2_REGS_DRAMTMG15_T_STAB_X32_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_DRAMTMG15_EN_DFI_LP_T_STAB (1 << 31)


/**** DRAMTMG16 register ****/
#define DWC_DDR_UMCTL2_REGS_DRAMTMG16_T_CCD_DLR_MASK 0x00000007
#define DWC_DDR_UMCTL2_REGS_DRAMTMG16_T_CCD_DLR_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_DRAMTMG16_T_RRD_DLR_MASK 0x00000700
#define DWC_DDR_UMCTL2_REGS_DRAMTMG16_T_RRD_DLR_SHIFT 8

#define DWC_DDR_UMCTL2_REGS_DRAMTMG16_T_FAW_DLR_MASK 0x001F0000
#define DWC_DDR_UMCTL2_REGS_DRAMTMG16_T_FAW_DLR_SHIFT 16

#define DWC_DDR_UMCTL2_REGS_DRAMTMG16_T_RP_CA_PARITY_MASK 0xFF000000
#define DWC_DDR_UMCTL2_REGS_DRAMTMG16_T_RP_CA_PARITY_SHIFT 24

/**** MRAMTMG0 register ****/
#define DWC_DDR_UMCTL2_REGS_MRAMTMG0_T_RAS_MIN_MRAM_MASK 0x0000007F
#define DWC_DDR_UMCTL2_REGS_MRAMTMG0_T_RAS_MIN_MRAM_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_MRAMTMG0_T_FAW_MRAM_MASK 0x00FF0000
#define DWC_DDR_UMCTL2_REGS_MRAMTMG0_T_FAW_MRAM_SHIFT 16


/**** MRAMTMG1 register ****/
#define DWC_DDR_UMCTL2_REGS_MRAMTMG1_T_RC_MRAM_MASK 0x000000FF
#define DWC_DDR_UMCTL2_REGS_MRAMTMG1_T_RC_MRAM_SHIFT 0


/**** MRAMTMG4 register ****/
#define DWC_DDR_UMCTL2_REGS_MRAMTMG4_T_RP_MRAM_MASK 0x0000007F
#define DWC_DDR_UMCTL2_REGS_MRAMTMG4_T_RP_MRAM_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_MRAMTMG4_T_RRD_MRAM_MASK 0x00003F00
#define DWC_DDR_UMCTL2_REGS_MRAMTMG4_T_RRD_MRAM_SHIFT 8

#define DWC_DDR_UMCTL2_REGS_MRAMTMG4_T_RCD_MRAM_MASK 0x7F000000
#define DWC_DDR_UMCTL2_REGS_MRAMTMG4_T_RCD_MRAM_SHIFT 24

/**** MRAMTMG9 register ****/
#define DWC_DDR_UMCTL2_REGS_MRAMTMG9_T_RRD_S_MRAM_MASK 0x00003F00
#define DWC_DDR_UMCTL2_REGS_MRAMTMG9_T_RRD_S_MRAM_SHIFT 8

/**** ZQCTL0 register ****/
/*
 * t<sub>ZQCS</sub>: Number of cycles of NOP required after a ZQCS (ZQ calibration short) command is
 * issued to SDRAM.<br>For configurations with MEMC_FREQ_RATIO=2, program this to t<sub>ZQCS</sub>/2
 * and round it up to the next integer value.<br>Unit: Clock cycles.<br>This is only present for
 * designs supporting DDR3/DDR4 or LPDDR2/LPDDR3 devices.
 */
#define DWC_DDR_UMCTL2_REGS_ZQCTL0_T_ZQ_SHORT_NOP_MASK 0x000003FF
#define DWC_DDR_UMCTL2_REGS_ZQCTL0_T_ZQ_SHORT_NOP_SHIFT 0
/*
 * t<sub>ZQoper</sub> for DDR3/DDR4, t<sub>ZQCL</sub> for LPDDR2/LPDDR3: Number of cycles of NOP
 * required after a ZQCL (ZQ calibration long) command is issued to SDRAM.<br>For configurations
 * with MEMC_FREQ_RATIO=2:<br>DDR3/DDR4: program this to t<sub>ZQoper</sub>/2 and round it up to the
 * next integer value.<br>LPDDR2/LPDDR3: program this to t<sub>ZQCL</sub>/2 and round it up to the
 * next integer value.<br>Unit: Clock cycles.<br>This is only present for designs supporting
 * DDR3/DDR4 or LPDDR2/LPDDR3 devices.
 */
#define DWC_DDR_UMCTL2_REGS_ZQCTL0_T_ZQ_LONG_NOP_MASK 0x07FF0000
#define DWC_DDR_UMCTL2_REGS_ZQCTL0_T_ZQ_LONG_NOP_SHIFT 16
/*
 *
 * - 1 - Disable issuing of ZQCL command at Maximum Power Saving Mode exit. Only applicable when run
 * in DDR4 mode.
 * - 0 - Enable issuing of ZQCL command at Maximum Power Saving Mode exit. Only applicable when run
 * in DDR4 mode.
 * This is only present for designs supporting DDR4 devices.
 */
#define DWC_DDR_UMCTL2_REGS_ZQCTL0_DIS_MPSMX_ZQCL (1 << 28)
/*
 *
 * - 1 - Denotes that ZQ resistor is shared between ranks. Means ZQinit/ZQCL/ZQCS commands are sent
 * to one rank at a time with tZQinit/tZQCL/tZQCS timing met between commands so that commands to
 * different ranks do not overlap.
 *  - 0 - ZQ resistor is not shared.
 * This is only present for designs supporting DDR3/DDR4 or LPDDR2/LPDDR3 devices.
 */
#define DWC_DDR_UMCTL2_REGS_ZQCTL0_ZQ_RESISTOR_SHARED (1 << 29)
/*
 *
 * - 1 - Disable issuing of ZQCL command at Self-Refresh exit. Only applicable when run in DDR3 or
 * DDR4 or LPDDR2 or LPDDR3 mode.
 * - 0 - Enable issuing of ZQCL command at Self-Refresh exit. Only applicable when run in DDR3 or
 * DDR4 or LPDDR2 or LPDDR3 mode.
 * This is only present for designs supporting DDR3/DDR4 or LPDDR2/LPDDR3 devices.
 */
#define DWC_DDR_UMCTL2_REGS_ZQCTL0_DIS_SRX_ZQCL (1 << 30)
/*
 *
 * - 1 - Disable uMCTL2 generation of ZQCS command. Register DBGCMD.zq_calib_short can be used
 * instead to issue ZQ calibration request from APB module.
 *  - 0 - Internally generate ZQCS commands based on ZQCTL1.t_zq_short_interval_x1024.
 * This is only present for designs supporting DDR3/DDR4 or LPDDR2/LPDDR3 devices.
 */
#define DWC_DDR_UMCTL2_REGS_ZQCTL0_DIS_AUTO_ZQ (1 << 31)

/**** ZQCTL1 register ****/
/*
 * Average interval to wait between automatically issuing ZQCS (ZQ calibration short) commands to
 * DDR3/DDR4/LPDDR2/LPDDR3 devices.<br>Meaningless, if ZQCTL0.dis_auto_zq=1.<br>Unit: 1024 clock
 * cycles.<br>This is only present for designs supporting DDR3/DDR4 or LPDDR2/LPDDR3 devices.
 */
#define DWC_DDR_UMCTL2_REGS_ZQCTL1_T_ZQ_SHORT_INTERVAL_X1024_MASK 0x000FFFFF
#define DWC_DDR_UMCTL2_REGS_ZQCTL1_T_ZQ_SHORT_INTERVAL_X1024_SHIFT 0

/**** ZQCTL2 register ****/
#define DWC_DDR_UMCTL2_REGS_ZQCTL2_ZQ_CID_MASK 0x00000300
#define DWC_DDR_UMCTL2_REGS_ZQCTL2_ZQ_CID_SHIFT 8

/**** DFITMG0 register ****/
/*
 * Write latency<br>Number of clocks from the write command to write data enable (dfi_wrdata_en).
 * This corresponds to the DFI timing parameter t<sub>phy_wrlat</sub>.<br>Refer to PHY specification
 * for correct value.Note that, depending on the PHY, if using RDIMM, it may be necessary to use the
 * value (CL + 1) in the calculation of t<sub>phy_wrlat</sub>. This is to compensate for the extra
 * cycle of latency through the RDIMM.
 */
#define DWC_DDR_UMCTL2_REGS_DFITMG0_DFI_TPHY_WRLAT_MASK 0x0000003F
#define DWC_DDR_UMCTL2_REGS_DFITMG0_DFI_TPHY_WRLAT_SHIFT 0
/*
 * Specifies the number of clock cycles between when dfi_wrdata_en is asserted to when the
 * associated write data is driven on the dfi_wrdata signal. This corresponds to the DFI timing
 * parameter tphy_wrdata. Refer to PHY specification for correct value. Note, max supported value is
 * 8. <br>Unit: Clocks
 */
#define DWC_DDR_UMCTL2_REGS_DFITMG0_DFI_TPHY_WRDATA_MASK 0x00003F00
#define DWC_DDR_UMCTL2_REGS_DFITMG0_DFI_TPHY_WRDATA_SHIFT 8
/*
 * Defines whether dfi_wrdata_en/dfi_wrdata/dfi_wrdata_mask is generated using HDR or SDR values
 * Selects whether value in DFITMG0.dfi_tphy_wrlat  is in terms of SDR or HDR clock cycles
 * Selects whether value in DFITMG0.dfi_tphy_wrdata is in terms of SDR or HDR clock cycles
 *  - 0 in terms of HDR clock cycles
 *  - 1 in terms of SDR clock cycles
 * Refer to PHY specification for correct value.
 */
#define DWC_DDR_UMCTL2_REGS_DFITMG0_DFI_WRDATA_USE_SDR (1 << 15)
/*
 * Time from the assertion of a read command on the DFI interface to the assertion of the
 * dfi_rddata_en signal.<br>Refer to PHY specification for correct value.<br>This corresponds to the
 * DFI parameter t<sub>rddata_en</sub>. Note that, depending on the PHY, if using RDIMM, it may be
 * necessary to use the value (CL + 1) in the calculation of t<sub>rddata_en</sub>. This is to
 * compensate for the extra cycle of latency through the RDIMM.<br>Unit: Clocks
 */
#define DWC_DDR_UMCTL2_REGS_DFITMG0_DFI_T_RDDATA_EN_MASK 0x007F0000
#define DWC_DDR_UMCTL2_REGS_DFITMG0_DFI_T_RDDATA_EN_SHIFT 16
/*
 * Defines whether dfi_rddata_en/dfi_rddata/dfi_rddata_valid is generated using HDR or SDR values
 * Selects whether value in DFITMG0.dfi_t_rddata_en is in terms of SDR or HDR clock cycles:
 *  - 0 in terms of HDR clock cycles
 *  - 1 in terms of SDR clock cycles
 * Refer to PHY specification for correct value.
 */
#define DWC_DDR_UMCTL2_REGS_DFITMG0_DFI_RDDATA_USE_SDR (1 << 23)
/*
 * Specifies the number of DFI clock cycles after an assertion or de-assertion of the DFI control
 * signals that the control signals at the PHY-DRAM interface reflect the assertion or de-assertion.
 * If the DFI clock and the memory clock are not phase-aligned, this timing parameter should be
 * rounded up to the next integer value. Note that if using RDIMM, depending on the PHY, it may be
 * necessary to increment this parameter by 1. This is to compensate for the extra cycle of latency
 * through the RDIMM
 */
#define DWC_DDR_UMCTL2_REGS_DFITMG0_DFI_T_CTRL_DELAY_MASK 0x1F000000
#define DWC_DDR_UMCTL2_REGS_DFITMG0_DFI_T_CTRL_DELAY_SHIFT 24

/**** DFITMG1 register ****/
/*
 * Specifies the number of DFI clock cycles from the de-assertion of the dfi_dram_clk_disable signal
 * on the DFI until the first valid rising edge of the clock to the DRAM memory devices, at the
 * PHY-DRAM boundary. If the DFI clock and the memory clock are not phase aligned, this timing
 * parameter should be rounded up to the next integer value.
 */
#define DWC_DDR_UMCTL2_REGS_DFITMG1_DFI_T_DRAM_CLK_ENABLE_MASK 0x0000001F
#define DWC_DDR_UMCTL2_REGS_DFITMG1_DFI_T_DRAM_CLK_ENABLE_SHIFT 0
/*
 * Specifies the number of DFI clock cycles from the assertion of the dfi_dram_clk_disable signal on
 * the DFI until the clock to the DRAM memory devices, at the PHY-DRAM boundary, maintains a low
 * value. If the DFI clock and the memory clock are not phase aligned, this timing parameter should
 * be rounded up to the next integer value.
 */
#define DWC_DDR_UMCTL2_REGS_DFITMG1_DFI_T_DRAM_CLK_DISABLE_MASK 0x00001F00
#define DWC_DDR_UMCTL2_REGS_DFITMG1_DFI_T_DRAM_CLK_DISABLE_SHIFT 8
/*
 * Specifies the number of DFI clocks between when the dfi_wrdata_en
 * signal is asserted and when the corresponding write data transfer is completed on the DRAM bus.
 * This corresponds to the DFI timing parameter twrdata_delay. Refer to PHY specification for
 * correct value.
 * For DFI 3.0 PHY, set to twrdata_delay, a new timing parameter introduced in DFI 3.0.
 * For DFI 2.1 PHY, set to tphy_wrdata + (delay of DFI write data to the DRAM).
 * Value to be programmed is in terms of DFI clocks, not PHY clocks.
 * In FREQ_RATIO=2, divide PHY's value by 2 and round up to next integer.
 * If using DFITMG0.dfi_wrdata_use_sdr=1, add 1 to the value.<br>Unit: Clocks
 */
#define DWC_DDR_UMCTL2_REGS_DFITMG1_DFI_T_WRDATA_DELAY_MASK 0x001F0000
#define DWC_DDR_UMCTL2_REGS_DFITMG1_DFI_T_WRDATA_DELAY_SHIFT 16
/*
 * Specifies the number of DFI PHY clocks between when the dfi_cs_n signal is asserted and when the
 * associated dfi_parity_in signal is driven.
 */
#define DWC_DDR_UMCTL2_REGS_DFITMG1_DFI_T_PARIN_LAT_MASK 0x03000000
#define DWC_DDR_UMCTL2_REGS_DFITMG1_DFI_T_PARIN_LAT_SHIFT 24
/*
 * Specifies the number of DFI PHY clocks between when the dfi_cs_n signal is asserted and when the
 * associated command is driven.
 * This field is used for CAL mode, should be set to '0' or the value which matches the CAL mode
 * register setting in the DRAM.<br>
 * If the PHY can add the latency for CAL mode, this should be set to '0'.<br>Valid Range: 0, 3, 4,
 * 5, 6, and 8
 */
#define DWC_DDR_UMCTL2_REGS_DFITMG1_DFI_T_CMD_LAT_MASK 0xF0000000
#define DWC_DDR_UMCTL2_REGS_DFITMG1_DFI_T_CMD_LAT_SHIFT 28

/**** DFILPCFG0 register ****/
/*
 * Enables DFI Low Power interface handshaking during Power Down Entry/Exit.
 *   - 0 - Disabled
 *   - 1 - Enabled
 */
#define DWC_DDR_UMCTL2_REGS_DFILPCFG0_DFI_LP_EN_PD (1 << 0)
/*
 * Value to drive on dfi_lp_wakeup signal when Power Down mode is entered. <br>Determines the DFI's
 * tlp_wakeup time:
 *   - 0x0 - 16 cycles
 *   - 0x1 - 32 cycles
 *   - 0x2 - 64 cycles
 *   - 0x3 - 128 cycles
 *   - 0x4 - 256 cycles
 *   - 0x5 - 512 cycles
 *   - 0x6 - 1024 cycles
 *   - 0x7 - 2048 cycles
 *   - 0x8 - 4096 cycles
 *   - 0x9 - 8192 cycles
 *   - 0xA - 16384 cycles
 *   - 0xB - 32768 cycles
 *   - 0xC - 65536 cycles
 *   - 0xD - 131072 cycles
 *   - 0xE - 262144 cycles
 *   - 0xF - Unlimited
 */
#define DWC_DDR_UMCTL2_REGS_DFILPCFG0_DFI_LP_WAKEUP_PD_MASK 0x000000F0
#define DWC_DDR_UMCTL2_REGS_DFILPCFG0_DFI_LP_WAKEUP_PD_SHIFT 4
/*
 * Enables DFI Low Power interface handshaking during Self Refresh Entry/Exit.
 *   - 0 - Disabled
 *   - 1 - Enabled
 */
#define DWC_DDR_UMCTL2_REGS_DFILPCFG0_DFI_LP_EN_SR (1 << 8)
/*
 * Value to drive on dfi_lp_wakeup signal when Self Refresh mode is entered. <br>Determines the
 * DFI's tlp_wakeup time:
 *   - 0x0 - 16 cycles
 *   - 0x1 - 32 cycles
 *   - 0x2 - 64 cycles
 *   - 0x3 - 128 cycles
 *   - 0x4 - 256 cycles
 *   - 0x5 - 512 cycles
 *   - 0x6 - 1024 cycles
 *   - 0x7 - 2048 cycles
 *   - 0x8 - 4096 cycles
 *   - 0x9 - 8192 cycles
 *   - 0xA - 16384 cycles
 *   - 0xB - 32768 cycles
 *   - 0xC - 65536 cycles
 *   - 0xD - 131072 cycles
 *   - 0xE - 262144 cycles
 *   - 0xF - Unlimited
 */
#define DWC_DDR_UMCTL2_REGS_DFILPCFG0_DFI_LP_WAKEUP_SR_MASK 0x0000F000
#define DWC_DDR_UMCTL2_REGS_DFILPCFG0_DFI_LP_WAKEUP_SR_SHIFT 12
/*
 * Setting for DFI's tlp_resp time. <br>Same value is used for both Power Down, Self Refresh, Deep
 * Power Down and Maximum Power Saving modes. <br>DFI 2.1 specification onwards, recommends using a
 * fixed value of 7 always.
 */
#define DWC_DDR_UMCTL2_REGS_DFILPCFG0_DFI_TLP_RESP_MASK 0x1F000000
#define DWC_DDR_UMCTL2_REGS_DFILPCFG0_DFI_TLP_RESP_SHIFT 24

/**** DFILPCFG1 register ****/
/*
 * Enables DFI Low Power interface handshaking during Maximum Power Saving Mode Entry/Exit.
 *   - 0 - Disabled
 *   - 1 - Enabled
 * This is only present for designs supporting DDR4 devices.
 */
#define DWC_DDR_UMCTL2_REGS_DFILPCFG1_DFI_LP_EN_MPSM (1 << 0)
/*
 * Value to drive on dfi_lp_wakeup signal when Maximum Power Saving Mode is entered. <br>Determines
 * the DFI's tlp_wakeup time:
 *   - 0x0 - 16 cycles
 *   - 0x1 - 32 cycles
 *   - 0x2 - 64 cycles
 *   - 0x3 - 128 cycles
 *   - 0x4 - 256 cycles
 *   - 0x5 - 512 cycles
 *   - 0x6 - 1024 cycles
 *   - 0x7 - 2048 cycles
 *   - 0x8 - 4096 cycles
 *   - 0x9 - 8192 cycles
 *   - 0xA - 16384 cycles
 *   - 0xB - 32768 cycles
 *   - 0xC - 65536 cycles
 *   - 0xD - 131072 cycles
 *   - 0xE - 262144 cycles
 *   - 0xF - Unlimited
 * This is only present for designs supporting DDR4 devices.
 */
#define DWC_DDR_UMCTL2_REGS_DFILPCFG1_DFI_LP_WAKEUP_MPSM_MASK 0x000000F0
#define DWC_DDR_UMCTL2_REGS_DFILPCFG1_DFI_LP_WAKEUP_MPSM_SHIFT 4

/**** DFIUPD0 register ****/
/*
 * Specifies the minimum number of clock cycles that the dfi_ctrlupd_req signal must be asserted.
 * The uMCTL2 expects the PHY to respond within this time. If the PHY does not respond, the uMCTL2
 * will de-assert dfi_ctrlupd_req after dfi_t_ctrlup_min + 2 cycles. Lowest value to assign to this
 * variable is 0x3.<br>Unit: Clocks
 */
#define DWC_DDR_UMCTL2_REGS_DFIUPD0_DFI_T_CTRLUP_MIN_MASK 0x000003FF
#define DWC_DDR_UMCTL2_REGS_DFIUPD0_DFI_T_CTRLUP_MIN_SHIFT 0
/*
 * Specifies the maximum number of clock cycles that the dfi_ctrlupd_req signal can assert. Lowest
 * value to assign to this variable is 0x40.<br>Unit: Clocks
 */
#define DWC_DDR_UMCTL2_REGS_DFIUPD0_DFI_T_CTRLUP_MAX_MASK 0x03FF0000
#define DWC_DDR_UMCTL2_REGS_DFIUPD0_DFI_T_CTRLUP_MAX_SHIFT 16

#define DWC_DDR_UMCTL2_REGS_DFIUPD0_CTRLUPD_PRE_SRX (1 << 29)
/*
 * When '1', disable the automatic dfi_ctrlupd_req generation by the uMCTL2 following a self-refresh
 * exit. The core must issue the dfi_ctrlupd_req signal using register reg_ddrc_ctrlupd. This
 * register field is changeable on the fly.<br>When '0', uMCTL2 issues a dfi_ctrlupd_req after
 * exiting self-refresh.
 */
#define DWC_DDR_UMCTL2_REGS_DFIUPD0_DIS_AUTO_CTRLUPD_SRX (1 << 30)
/*
 * When '1', disable the automatic dfi_ctrlupd_req generation by the uMCTL2. The core must issue the
 * dfi_ctrlupd_req signal using register reg_ddrc_ctrlupd. This register field is changeable on the
 * fly.<br>When '0', uMCTL2 issues dfi_ctrlupd_req periodically.
 */
#define DWC_DDR_UMCTL2_REGS_DFIUPD0_DIS_AUTO_CTRLUPD (1 << 31)

/**** DFIUPD1 register ****/
/*
 * This is the maximum amount of time between uMCTL2 initiated DFI update requests. This timer
 * resets with each update request; when the timer expires dfi_ctrlupd_req is sent and traffic is
 * blocked until the dfi_ctrlupd_ackx is received. PHY can use this idle time to recalibrate the
 * delay lines to the DLLs. The DFI controller update is also used to reset PHY FIFO pointers in
 * case of data capture errors. Updates are required to maintain calibration over PVT, but frequent
 * updates may impact performance.<br>Note: Value programmed for
 * DFIUPD1.dfi_t_ctrlupd_interval_max_x1024 must be greater than
 * DFIUPD1.dfi_t_ctrlupd_interval_min_x1024.<br>Unit: 1024 clocks
 */
#define DWC_DDR_UMCTL2_REGS_DFIUPD1_DFI_T_CTRLUPD_INTERVAL_MAX_X1024_MASK 0x000000FF
#define DWC_DDR_UMCTL2_REGS_DFIUPD1_DFI_T_CTRLUPD_INTERVAL_MAX_X1024_SHIFT 0
/*
 * This is the minimum amount of time between uMCTL2 initiated DFI update requests (which is
 * executed whenever the uMCTL2 is idle). Set this number higher to reduce the frequency of update
 * requests, which can have a small impact on the latency of the first read request when the uMCTL2
 * is idle.<br>Unit: 1024 clocks
 */
#define DWC_DDR_UMCTL2_REGS_DFIUPD1_DFI_T_CTRLUPD_INTERVAL_MIN_X1024_MASK 0x00FF0000
#define DWC_DDR_UMCTL2_REGS_DFIUPD1_DFI_T_CTRLUPD_INTERVAL_MIN_X1024_SHIFT 16

/**** DFIUPD2 register ****/
/*
 * Specifies the maximum number of DFI clock cycles that the dfi_phyupd_req signal may remain
 * asserted after the assertion of the dfi_phyupd_ack signal for dfi_phyupd_type = 2'b00. Is used in
 * conjunction with value defined via DFIUPD4.dfi_phyupd_type0_mult. The dfi_phyupd_req signal may
 * de-assert at any cycle after the assertion of the dfi_phyupd_ack signal.
 */
#define DWC_DDR_UMCTL2_REGS_DFIUPD2_DFI_PHYUPD_TYPE0_MASK 0x00000FFF
#define DWC_DDR_UMCTL2_REGS_DFIUPD2_DFI_PHYUPD_TYPE0_SHIFT 0
/*
 * Specifies the maximum number of DFI clock cycles that the dfi_phyupd_req signal may remain
 * asserted after the assertion of the dfi_phyupd_ack signal for dfi_phyupd_type = 2'b01. Is used in
 * conjunction with value defined via DFIUPD4.dfi_phyupd_type1_mult. The dfi_phyupd_req signal may
 * de-assert at any cycle after the assertion of the dfi_phyupd_ack signal.
 */
#define DWC_DDR_UMCTL2_REGS_DFIUPD2_DFI_PHYUPD_TYPE1_MASK 0x0FFF0000
#define DWC_DDR_UMCTL2_REGS_DFIUPD2_DFI_PHYUPD_TYPE1_SHIFT 16
/*
 * Enables the support for acknowledging PHY-initiated updates:
 *	- 0 - Disabled
 *	- 1 - Enabled
 */
#define DWC_DDR_UMCTL2_REGS_DFIUPD2_DFI_PHYUPD_EN (1 << 31)

/**** DFIUPD3 register ****/
/*
 * Specifies the maximum number of DFI clock cycles that the dfi_phyupd_req signal may remain
 * asserted after the assertion of the dfi_phyupd_ack signal for dfi_phyupd_type = 2'b10. Is used in
 * conjunction with value defined via DFIUPD4.dfi_phyupd_type2_mult. The dfi_phyupd_req signal may
 * de-assert at any cycle after the assertion of the dfi_phyupd_ack signal.
 */
#define DWC_DDR_UMCTL2_REGS_DFIUPD3_DFI_PHYUPD_TYPE2_MASK 0x00000FFF
#define DWC_DDR_UMCTL2_REGS_DFIUPD3_DFI_PHYUPD_TYPE2_SHIFT 0
/*
 * Specifies the maximum number of DFI clock cycles that the dfi_phyupd_req signal may remain
 * asserted after the assertion of the dfi_phyupd_ack signal for dfi_phyupd_type = 2'b11. Is used in
 * conjunction with value defined via DFIUPD4.dfi_phyupd_type3_mult. The dfi_phyupd_req signal may
 * de-assert at any cycle after the assertion of the dfi_phyupd_ack signal.
 */
#define DWC_DDR_UMCTL2_REGS_DFIUPD3_DFI_PHYUPD_TYPE3_MASK 0x0FFF0000
#define DWC_DDR_UMCTL2_REGS_DFIUPD3_DFI_PHYUPD_TYPE3_SHIFT 16

/**** DFIMISC register ****/
/*
 * PHY initialization complete enable signal.<br>When asserted the dfi_init_complete signal can be
 * used to trigger SDRAM initialisation
 */
#define DWC_DDR_UMCTL2_REGS_DFIMISC_DFI_INIT_COMPLETE_EN (1 << 0)
/*
 * DBI implemented in DDRC or PHY.
 *   - 0 - DDRC implements DBI functionality.
 *   - 1 - PHY implements DBI functionality.
 *   Present only in designs configured to support DDR4.
 */
#define DWC_DDR_UMCTL2_REGS_DFIMISC_PHY_DBI_MODE (1 << 1)

#define DWC_DDR_UMCTL2_REGS_DFIMISC_CTL_IDLE_EN (1 << 4)

#define DWC_DDR_UMCTL2_REGS_DFIMISC_DFI_INIT_START (1 << 5)

#define DWC_DDR_UMCTL2_REGS_DFIMISC_DFI_FREQUENCY_MASK 0x00001F00
#define DWC_DDR_UMCTL2_REGS_DFIMISC_DFI_FREQUENCY_SHIFT 8

/**** DFITMG3 register ****/
#define DWC_DDR_UMCTL2_REGS_DFITMG3_DFI_T_GEARDOWN_DELAY_MASK 0x0000001F
#define DWC_DDR_UMCTL2_REGS_DFITMG3_DFI_T_GEARDOWN_DELAY_SHIFT 0


/**** DFISTAT register ****/
#define DWC_DDR_UMCTL2_REGS_DFISTAT_DFI_INIT_COMPLETE (1 << 0)

#define DWC_DDR_UMCTL2_REGS_DFISTAT_DFI_LP_ACK (1 << 1)

/**** DFIUPD4 register ****/
/*
 * Multiplier value to be used in conjunction with DFIUPD2.dfi_phyupd_type0. Total value for
 * tphyupd_type0 is DFIUPD2.dfi_phyupd_type0 * 2^(dfi_phyupd_type0_mult-1)(with option for an
 * infinite value when it is 0x00), Maximum is 0x17. 0x18...0x1F are not allowed
 */
#define DWC_DDR_UMCTL2_REGS_DFIUPD4_DFI_PHYUPD_TYPE0_MULT_MASK 0x0000001F
#define DWC_DDR_UMCTL2_REGS_DFIUPD4_DFI_PHYUPD_TYPE0_MULT_SHIFT 0
/*
 * Multiplier value to be used in conjunction with DFIUPD2.dfi_phyupd_type1. Total value for
 * tphyupd_type1 is DFIUPD2.dfi_phyupd_type1 * 2^(dfi_phyupd_type1_mult-1)(with option for an
 * infinite value when it is 0x00), Maximum is 0x17. 0x18...0x1F are not allowed
 */
#define DWC_DDR_UMCTL2_REGS_DFIUPD4_DFI_PHYUPD_TYPE1_MULT_MASK 0x00001F00
#define DWC_DDR_UMCTL2_REGS_DFIUPD4_DFI_PHYUPD_TYPE1_MULT_SHIFT 8
/*
 * Multiplier value to be used in conjunction with DFIUPD3.dfi_phyupd_type2. Total value for
 * tphyupd_type2 is DFIUPD3.dfi_phyupd_type2 * 2^(dfi_phyupd_type2_mult-1)(with option for an
 * infinite value when it is 0x00), Maximum is 0x17. 0x18...0x1F are not allowed
 */
#define DWC_DDR_UMCTL2_REGS_DFIUPD4_DFI_PHYUPD_TYPE2_MULT_MASK 0x001F0000
#define DWC_DDR_UMCTL2_REGS_DFIUPD4_DFI_PHYUPD_TYPE2_MULT_SHIFT 16
/*
 * Multiplier value to be used in conjunction with DFIUPD3.dfi_phyupd_type3. Total value for
 * tphyupd_type3 is DFIUPD3.dfi_phyupd_type3 * 2^(dfi_phyupd_type3_mult-1)(with option for an
 * infinite value when it is 0x00), Maximum is 0x17. 0x18...0x1F are not allowed
 */
#define DWC_DDR_UMCTL2_REGS_DFIUPD4_DFI_PHYUPD_TYPE3_MULT_MASK 0x1F000000
#define DWC_DDR_UMCTL2_REGS_DFIUPD4_DFI_PHYUPD_TYPE3_MULT_SHIFT 24

/**** DBICTL register ****/
/*
 * DM enable signal in DDRC. This signal must be set the same value as MR5 bit A10.<br>When x4
 * devices are used, this signal must be set to 0.
 *   - 0 - DM is disabled.
 *   - 1 - DM is enabled.
 */
#define DWC_DDR_UMCTL2_REGS_DBICTL_DM_EN (1 << 0)
/*
 * Write DBI enable signal in DDRC. This signal must be set the same value as MR5 bit A11.<br>When
 * x4 devices are used, this signal must be set to 0.
 *   - 0 - Write DBI is disabled.
 *   - 1 - Write DBI is enabled.
 */
#define DWC_DDR_UMCTL2_REGS_DBICTL_WR_DBI_EN (1 << 1)
/*
 * Read DBI enable signal in DDRC. This signal must be set the same value as MR5 bit A12.<br>When x4
 * devices are used, this signal must be set to 0.
 *   - 0 - Read DBI is disabled.
 *   - 1 - Read DBI is enabled.
 */
#define DWC_DDR_UMCTL2_REGS_DBICTL_RD_DBI_EN (1 << 2)

/* Address mapping bases */
#define AL_DDR_ADDR_MAP_CS_0_BASE	6

#define AL_DDR_ADDR_MAP_CS_DISABLED	31

#define AL_DDR_ADDR_MAP_CID_0_BASE	4

#define AL_DDR_ADDR_MAP_CID_DISABLED	31

#define AL_DDR_ADDR_MAP_BANK_0_BASE	2

#define AL_DDR_ADDR_MAP_BANK_DISABLED_ALPINE_V1	15
#define AL_DDR_ADDR_MAP_BANK_DISABLED_ALPINE_V2	31

#define AL_DDR_ADDR_MAP_BG_0_BASE	2

#define AL_DDR_ADDR_MAP_BG_DISABLED	31

#define AL_DDR_ADDR_MAP_COL_2_BASE	2

#define AL_DDR_ADDR_MAP_COL_DISABLED	15

#define AL_DDR_ADDR_MAP_ROW_0_BASE	6

#define AL_DDR_ADDR_MAP_ROW_DISABLED	15

#define AL_DDR_ADDR_MAP_OFFSET_V1_V2		4
#define AL_DDR_ADDR_MAP_OFFSET_V3		3
#define AL_DDR_ADDR_MAP_OFFSET_V4		3

/**** ADDRMAP0 register ****/
/*
 * Selects the HIF address bit used as rank address bit 0.<br>Valid Range: 0 to 27, and
 * 31<br>Internal Base: 6<br>The selected HIF address bit is determined by adding the internal base
 * to the value of this field.<br>If set to 31, rank address bit 0 is set to 0.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP0_ADDRMAP_CS_BIT0_MASK 0x0000001F
#define DWC_DDR_UMCTL2_REGS_ADDRMAP0_ADDRMAP_CS_BIT0_SHIFT 0
/*
 * Selects the HIF address bit used as rank address bit 1.<br>Valid Range: 0 to 26, and
 * 31<br>Internal Base: 7<br>The selected HIF address bit is determined by adding the internal base
 * to the value of this field.<br>If set to 31, rank address bit 1 is set to 0.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP0_ADDRMAP_CS_BIT1_MASK 0x00001F00
#define DWC_DDR_UMCTL2_REGS_ADDRMAP0_ADDRMAP_CS_BIT1_SHIFT 8

/**** ADDRMAP1 register ****/
/*
 * Selects the HIF address bits used as bank address bit 0.<br>Valid Range: 0 to 30<br>Internal
 * Base: 2<br>The selected HIF address bit for each of the bank address bits is determined by adding
 * the internal base to the value of this field.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP1_ADDRMAP_BANK_B0_MASK 0x0000001F
#define DWC_DDR_UMCTL2_REGS_ADDRMAP1_ADDRMAP_BANK_B0_SHIFT 0
/*
 * Selects the HIF address bits used as bank address bit 1.<br>Valid Range: 0 to 30<br>Internal
 * Base: 3<br>The selected HIF address bit for each of the bank address bits is determined by adding
 * the internal base to the value of this field.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP1_ADDRMAP_BANK_B1_MASK 0x00001F00
#define DWC_DDR_UMCTL2_REGS_ADDRMAP1_ADDRMAP_BANK_B1_SHIFT 8
/*
 * Selects the HIF address bit used as bank address bit 2.<br>Valid Range: 0 to 29 and
 * 31<br>Internal Base: 4<br>The selected HIF address bit is determined by adding the internal base
 * to the value of this field.<br>If set to 31, bank address bit 2 is set to 0.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP1_ADDRMAP_BANK_B2_MASK 0x001F0000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP1_ADDRMAP_BANK_B2_SHIFT 16

/**** ADDRMAP2 register ****/
/*
 *
 * - Full bus width mode: Selects the HIF address bit used as column address bit 2 (if
 * MEMC_BURST_LENGTH = 4) or 3 (if MEMC_BURST_LENGTH = 8).<br>
 * - Half bus width mode: Selects the HIF address bit used as column address bit 3 (if
 * MEMC_BURST_LENGTH = 4) or 4 (if MEMC_BURST_LENGTH = 8).<br>
 * - Quarter bus width mode: Selects the HIF address bit used as column address bit 4 (if
 * MEMC_BURST_LENGTH = 4) or 5 (if MEMC_BURST_LENGTH = 8).<br>Valid Range: 0 to 7<br>Internal Base:
 * 2<br>The selected HIF address bit is determined by adding the internal base to the value of this
 * field.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP2_ADDRMAP_COL_B2_MASK 0x0000000F
#define DWC_DDR_UMCTL2_REGS_ADDRMAP2_ADDRMAP_COL_B2_SHIFT 0
/*
 *
 * - Full bus width mode: Selects the HIF address bit used as column address bit 3 (if
 * MEMC_BURST_LENGTH = 4) or 4 (if MEMC_BURST_LENGTH = 8). <br>
 * - Half bus width mode: Selects the HIF address bit used as column address bit 4 (if
 * MEMC_BURST_LENGTH = 4) or 5 (if MEMC_BURST_LENGTH = 8).<br>
 * - Quarter bus width mode: Selects the HIF address bit used as column address bit 5 (if
 * MEMC_BURST_LENGTH = 4) or 6 (if MEMC_BURST_LENGTH = 8).<br>Valid Range: 0 to 7<br>Internal Base:
 * 3<br>The selected HIF address bit is determined by adding the internal base to the value of this
 * field.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP2_ADDRMAP_COL_B3_MASK 0x00000F00
#define DWC_DDR_UMCTL2_REGS_ADDRMAP2_ADDRMAP_COL_B3_SHIFT 8
/*
 *
 * - Full bus width mode: Selects the HIF address bit used as column address bit 4 (if
 * MEMC_BURST_LENGTH = 4) or 5 (if MEMC_BURST_LENGTH = 8).<br>
 * - Half bus width mode: Selects the HIF address bit used as column address bit 5 (if
 * MEMC_BURST_LENGTH = 4) or 6 (if MEMC_BURST_LENGTH = 8).<br>
 * - Quarter bus width mode: Selects the HIF address bit used as column address bit 6 (if
 * MEMC_BURST_LENGTH = 4) or 7 (if MEMC_BURST_LENGTH = 8).<br>Valid Range: 0 to 7, and
 * 15<br>Internal Base: 4<br>The selected HIF address bit is determined by adding the internal base
 * to the value of this field. If set to 15, this column address bit is set to 0.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP2_ADDRMAP_COL_B4_MASK 0x000F0000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP2_ADDRMAP_COL_B4_SHIFT 16
/*
 *
 * - Full bus width mode: Selects the HIF address bit used as column address bit 5 (if
 * MEMC_BURST_LENGTH = 4) or 6 (if MEMC_BURST_LENGTH = 8).<br>
 * - Half bus width mode: Selects the HIF address bit used as column address bit 6 (if
 * MEMC_BURST_LENGTH = 4) or 7 (if MEMC_BURST_LENGTH = 8).<br>
 * - Quarter bus width mode: Selects the HIF address bit used as column address bit 7 (if
 * MEMC_BURST_LENGTH = 4) or 8 (if MEMC_BURST_LENGTH = 8).<br>
 * Valid Range: 0 to 7, and 15<br>Internal Base: 5<br>The selected HIF address bit is determined by
 * adding the internal base to the value of this field. If set to 15, this column address bit is set
 * to 0.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP2_ADDRMAP_COL_B5_MASK 0x0F000000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP2_ADDRMAP_COL_B5_SHIFT 24

/**** ADDRMAP3 register ****/
/*
 *
 * - Full bus width mode: Selects the HIF address bit used as column address bit 6 (if
 * MEMC_BURST_LENGTH = 4) or 7 (if MEMC_BURST_LENGTH = 8).<br>
 * - Half bus width mode: Selects the HIF address bit used as column address bit 7 (if
 * MEMC_BURST_LENGTH = 4) or 8 (if MEMC_BURST_LENGTH = 8).<br>
 * - Quarter bus width mode: Selects the HIF address bit used as column address bit 8 (if
 * MEMC_BURST_LENGTH = 4) or 9 (if MEMC_BURST_LENGTH = 8).<br>Valid Range: 0 to 7, and
 * 15<br>Internal Base: 6<br>The selected HIF address bit is determined by adding the internal base
 * to the value of this field. If set to 15, this column address bit is set to 0.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP3_ADDRMAP_COL_B6_MASK 0x0000000F
#define DWC_DDR_UMCTL2_REGS_ADDRMAP3_ADDRMAP_COL_B6_SHIFT 0
/*
 *
 * - Full bus width mode: Selects the HIF address bit used as column address bit 7 (if
 * MEMC_BURST_LENGTH = 4) or 8 (if MEMC_BURST_LENGTH = 8).<br>
 * - Half bus width mode: Selects the HIF address bit used as column address bit 8 (if
 * MEMC_BURST_LENGTH = 4) or 9 (if MEMC_BURST_LENGTH = 8).<br>
 * - Quarter bus width mode: Selects the HIF address bit used as column address bit 9 (if
 * MEMC_BURST_LENGTH = 4) or 11 (10 in LPDDR2/LPDDR3 mode) (if MEMC_BURST_LENGTH = 8).<br>Valid
 * Range: 0 to 7, and 15<br>Internal Base: 7 <br>The selected HIF address bit is determined by
 * adding the internal base to the value of this field. If set to 15, this column address bit is set
 * to 0.<br>Note: Per JEDEC DDR2/3/mDDR specification, column address bit 10 is reserved for
 * indicating auto-precharge and hence no source address bit can be mapped to column address bit
 * 10.<br>In LPDDR2/LPDDR3, there is a dedicated bit for auto-precharge in the CA bus and hence
 * column bit 10 is used.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP3_ADDRMAP_COL_B7_MASK 0x00000F00
#define DWC_DDR_UMCTL2_REGS_ADDRMAP3_ADDRMAP_COL_B7_SHIFT 8
/*
 *
 * - Full bus width mode: Selects the HIF address bit used as column address bit 8 (if
 * MEMC_BURST_LENGTH = 4) or 9 (if MEMC_BURST_LENGTH = 8).<br>
 * - Half bus width mode: Selects the HIF address bit used as column address bit 9 (if
 * MEMC_BURST_LENGTH = 4) or 11 (10 in LPDDR2/LPDDR3 mode) (if MEMC_BURST_LENGTH = 8). <br>
 * - Quarter bus width mode: Selects the HIF address bit used as column address bit 11 (10 in
 * LPDDR2/LPDDR3 mode) (if MEMC_BURST_LENGTH = 4) or 13 (11 in LPDDR2/LPDDR3 mode) (if
 * MEMC_BURST_LENGTH = 8).<br>Valid Range: 0 to 7, and 15<br>Internal Base: 8<br>The selected HIF
 * address bit is determined by adding the internal base to the value of this field.<br>If set to
 * 15, this column address bit is set to 0.<br>Note: Per JEDEC DDR2/3/mDDR specification, column
 * address bit 10 is reserved for indicating auto-precharge, and hence no source address bit can be
 * mapped to column address bit 10.<br>In LPDDR2/LPDDR3, there is a dedicated bit for auto-precharge
 * in the CA bus and hence column bit 10 is used.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP3_ADDRMAP_COL_B8_MASK 0x000F0000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP3_ADDRMAP_COL_B8_SHIFT 16
/*
 *
 * - Full bus width mode: Selects the HIF address bit used as column address bit 9 (if
 * MEMC_BURST_LENGTH = 4) or 11 (10 in LPDDR2/LPDDR3 mode) (if MEMC_BURST_LENGTH = 8).<br>
 * - Half bus width mode: Selects the HIF address bit used as column address bit 11 (10 in
 * LPDDR2/LPDDR3 mode) (if MEMC_BURST_LENGTH = 4) or 13 (11 in LPDDR2/LPDDR3 mode) (if
 * MEMC_BURST_LENGTH = 8). (Column address bit 11 in LPDDR2/LPDDR3 mode)<br>
 * - Quarter bus width mode: Selects the HIF address bit used as column address bit 13 (11 in
 * LPDDR2/LPDDR3 mode) (if MEMC_BURST_LENGTH = 4) or UNUSED (if MEMC_BURST_LENGTH = 8).<br>Valid
 * Range: 0 to 7, and 15<br>Internal Base: 9<br>The selected HIF address bit is determined by adding
 * the internal base to the value of this field.<br>If set to 15, this column address bit is set to
 * 0.<br>Note: Per JEDEC DDR2/3/mDDR specification, column address bit 10 is reserved for indicating
 * auto-precharge, and hence no source address bit can be mapped to column address bit 10.<br>In
 * LPDDR2/LPDDR3, there is a dedicated bit for auto-precharge in the CA bus and hence column bit 10
 * is used.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP3_ADDRMAP_COL_B9_MASK 0x0F000000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP3_ADDRMAP_COL_B9_SHIFT 24

/**** ADDRMAP4 register ****/
/*
 *
 * - Full bus width mode: Selects the HIF address bit used as column address bit 11 (10 in
 * LPDDR2/LPDDR3 mode) (if MEMC_BURST_LENGTH = 4) or 13 (11 in LPDDR2/LPDDR3 mode) (if
 * MEMC_BURST_LENGTH = 8).<br>
 * - Half bus width mode: Selects the HIF address bit used as column address bit 13 (11 in
 * LPDDR2/LPDDR3 mode) (if MEMC_BURST_LENGTH = 4) or UNUSED (if MEMC_BURST_LENGTH = 8).<br>
 * - Quarter bus width mode: UNUSED. To make it unused, this must be tied to 4'hF.<br>Valid Range: 0
 * to 7, and 15<br>Internal Base: 10<br>The selected HIF address bit is determined by adding the
 * internal base to the value of this field.<br>If set to 15, this column address bit is set to 0.
 * <br>Note: Per JEDEC DDR2/3/mDDR specification, column address bit 10 is reserved for indicating
 * auto-precharge, and hence no source address bit can be mapped to column address bit 10.<br>In
 * LPDDR2/LPDDR3, there is a dedicated bit for auto-precharge in the CA bus and hence column bit 10
 * is used.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP4_ADDRMAP_COL_B10_MASK 0x0000000F
#define DWC_DDR_UMCTL2_REGS_ADDRMAP4_ADDRMAP_COL_B10_SHIFT 0
/*
 *
 * - Full bus width mode: Selects the HIF address bit used as column address bit 13 (11 in
 * LPDDR2/LPDDR3 mode) (if MEMC_BURST_LENGTH = 4) or UNUSED (if MEMC_BURST_LENGTH = 8).<br>
 *   - Half bus width mode: Unused. To make it unused, this should be tied to 4'hF.<br>
 * - Quarter bus width mode: Unused. To make it unused, this must be tied to 4'hF.<br>Valid Range: 0
 * to 7, and 15<br>Internal Base: 11<br>The selected HIF address bit is determined by adding the
 * internal base to the value of this field.<br>If set to 15, this column address bit is set to
 * 0.<br>Note: Per JEDEC DDR2/3/mDDR specification, column address bit 10 is reserved for indicating
 * auto-precharge, and hence no source address bit can be mapped to column address bit 10.<br>In
 * LPDDR2/LPDDR3, there is a dedicated bit for auto-precharge in the CA bus and hence column bit 10
 * is used.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP4_ADDRMAP_COL_B11_MASK 0x00000F00
#define DWC_DDR_UMCTL2_REGS_ADDRMAP4_ADDRMAP_COL_B11_SHIFT 8

/**** ADDRMAP5 register ****/
/*
 * Selects the HIF address bits used as row address bit 0.<br>Valid Range: 0 to 11<br>Internal Base:
 * 6<br>The selected HIF address bit for each of the row address bits is determined by adding the
 * internal base to the value of this field.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP5_ADDRMAP_ROW_B0_MASK 0x0000000F
#define DWC_DDR_UMCTL2_REGS_ADDRMAP5_ADDRMAP_ROW_B0_SHIFT 0
/*
 * Selects the HIF address bits used as row address bit 1.<br>Valid Range: 0 to 11<br>Internal Base:
 * 7<br>The selected HIF address bit for each of the row address bits is determined by adding the
 * internal base to the value of this field.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP5_ADDRMAP_ROW_B1_MASK 0x00000F00
#define DWC_DDR_UMCTL2_REGS_ADDRMAP5_ADDRMAP_ROW_B1_SHIFT 8
/*
 * Selects the HIF address bits used as row address bits 2 to 10.<br>Valid Range: 0 to
 * 11<br>Internal Base: 8 (for row address bit 2), 9 (for row address bit 3), 10 (for row address
 * bit 4) etc increasing to 16 (for row address bit 10)<br>The selected HIF address bit for each of
 * the row address bits is determined by adding the internal base to the value of this field.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP5_ADDRMAP_ROW_B2_10_MASK 0x000F0000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP5_ADDRMAP_ROW_B2_10_SHIFT 16
/*
 * Selects the HIF address bit used as row address bit 11.<br>Valid Range: 0 to 11, and
 * 15<br>Internal Base: 17<br>The selected HIF address bit is determined by adding the internal base
 * to the value of this field.<br>If set to 15, row address bit 11 is set to 0.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP5_ADDRMAP_ROW_B11_MASK 0x0F000000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP5_ADDRMAP_ROW_B11_SHIFT 24

/**** ADDRMAP6 register ****/
/*
 * Selects the HIF address bit used as row address bit 12.<br>Valid Range: 0 to 11, and
 * 15<br>Internal Base: 18<br>The selected HIF address bit is determined by adding the internal base
 * to the value of this field.<br>If set to 15, row address bit 12 is set to 0.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP6_ADDRMAP_ROW_B12_MASK 0x0000000F
#define DWC_DDR_UMCTL2_REGS_ADDRMAP6_ADDRMAP_ROW_B12_SHIFT 0
/*
 * Selects the HIF address bit used as row address bit 13.<br>Valid Range: 0 to 11, and
 * 15<br>Internal Base: 19<br>The selected HIF address bit is determined by adding the internal base
 * to the value of this field.<br>If set to 15, row address bit 13 is set to 0.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP6_ADDRMAP_ROW_B13_MASK 0x00000F00
#define DWC_DDR_UMCTL2_REGS_ADDRMAP6_ADDRMAP_ROW_B13_SHIFT 8
/*
 * Selects the HIF address bit used as row address bit 14.<br>Valid Range: 0 to 11, and
 * 15<br>Internal Base: 20<br>The selected HIF address bit is determined by adding the internal base
 * to the value of this field.<br>If set to 15, row address bit 14 is set to 0.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP6_ADDRMAP_ROW_B14_MASK 0x000F0000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP6_ADDRMAP_ROW_B14_SHIFT 16
/*
 * Selects the HIF address bit used as row address bit 15.<br>Valid Range: 0 to 11, and
 * 15<br>Internal Base: 21<br>The selected HIF address bit is determined by adding the internal base
 * to the value of this field.<br>If set to 15, row address bit 15 is set to 0.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP6_ADDRMAP_ROW_B15_MASK 0x0F000000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP6_ADDRMAP_ROW_B15_SHIFT 24

/**** ADDRMAP7 register ****/
/*
 * Selects the HIF address bit used as row address bit 16.<br>Valid Range: 0 to 11, and
 * 15<br>Internal Base: 22<br>The selected HIF address bit is determined by adding the internal base
 * to the value of this field.<br>If set to 15, row address bit 16 is set to 0.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP7_ADDRMAP_ROW_B16_MASK 0x0000000F
#define DWC_DDR_UMCTL2_REGS_ADDRMAP7_ADDRMAP_ROW_B16_SHIFT 0
/*
 * Selects the HIF address bit used as row address bit 17.<br>Valid Range: 0 to 10, and
 * 15<br>Internal Base: 23<br>The selected HIF address bit is determined by adding the internal base
 * to the value of this field.<br>If set to 15, row address bit 17 is set to 0.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP7_ADDRMAP_ROW_B17_MASK 0x00000F00
#define DWC_DDR_UMCTL2_REGS_ADDRMAP7_ADDRMAP_ROW_B17_SHIFT 8

/**** ADDRMAP8 register ****/
/*
 * Selects the HIF address bits used as bank group address bit 0.<br>Valid Range: 0 to
 * 30<br>Internal Base: 2<br>The selected HIF address bit for each of the bank group address bits is
 * determined by adding the internal base to the value of this field.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP8_ADDRMAP_BG_B0_MASK 0x0000001F
#define DWC_DDR_UMCTL2_REGS_ADDRMAP8_ADDRMAP_BG_B0_SHIFT 0
/*
 * Selects the HIF address bits used as bank group address bit 1.<br>Valid Range: 0 to 30, and
 * 31<br>Internal Base: 3<br>The selected HIF address bit for each of the bank group address bits is
 * determined by adding the internal base to the value of this field.<br>If set to 31, bank group
 * address bit 1 is set to 0.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP8_ADDRMAP_BG_B1_MASK 0x00003F00
#define DWC_DDR_UMCTL2_REGS_ADDRMAP8_ADDRMAP_BG_B1_SHIFT 8

/**** ADDRMAP9 register ****/
/*
 * Selects the HIF address bits used as row address bit 2.
 * Valid Range: 0 to 11
 * Internal Base: 8
 * The selected HIF address bit for each of the row address bits is determined by adding the
 * internal base to the value of this field. This register field is used only when
 * ADDRMAP5.addrmap_row_b2_10 is set to value 15.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP9_ADDRMAP_ROW_B2_MASK 0x0000000F
#define DWC_DDR_UMCTL2_REGS_ADDRMAP9_ADDRMAP_ROW_B2_SHIFT 0
/*
 * Selects the HIF address bits used as row address bit 3.
 * Valid Range: 0 to 11
 * Internal Base: 9
 * The selected HIF address bit for each of the row address bits is determined by adding the
 * internal base to the value of this field. This register field is used only when
 * ADDRMAP5.addrmap_row_b2_10 is set to value 15.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP9_ADDRMAP_ROW_B3_MASK 0x00000F00
#define DWC_DDR_UMCTL2_REGS_ADDRMAP9_ADDRMAP_ROW_B3_SHIFT 8
/*
 * Selects the HIF address bits used as row address bit 4.
 * Valid Range: 0 to 11
 * Internal Base: 10
 * The selected HIF address bit for each of the row address bits is determined by adding the
 * internal base to the value of this field. This register field is used only when
 * ADDRMAP5.addrmap_row_b2_10 is set to value 15.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP9_ADDRMAP_ROW_B4_MASK 0x000F0000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP9_ADDRMAP_ROW_B4_SHIFT 16
/*
 * Selects the HIF address bits used as row address bit 5.
 * Valid Range: 0 to 11
 * Internal Base: 11
 * The selected HIF address bit for each of the row address bits is determined by adding the
 * internal base to the value of this field. This register field is used only when
 * ADDRMAP5.addrmap_row_b2_10 is set to value 15.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP9_ADDRMAP_ROW_B5_MASK 0x0F000000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP9_ADDRMAP_ROW_B5_SHIFT 24

/**** ADDRMAP10 register ****/
/*
 * Selects the HIF address bits used as row address bit 6.
 * Valid Range: 0 to 11
 * Internal Base: 12
 * The selected HIF address bit for each of the row address bits is determined by adding the
 * internal base to the value of this field. This register field is used only when
 * ADDRMAP5.addrmap_row_b2_10 is set to value 15.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP10_ADDRMAP_ROW_B6_MASK 0x0000000F
#define DWC_DDR_UMCTL2_REGS_ADDRMAP10_ADDRMAP_ROW_B6_SHIFT 0
/*
 * Selects the HIF address bits used as row address bit 7.
 * Valid Range: 0 to 11
 * Internal Base: 13
 * The selected HIF address bit for each of the row address bits is determined by adding the
 * internal base to the value of this field. This register field is used only when
 * ADDRMAP5.addrmap_row_b2_10 is set to value 15.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP10_ADDRMAP_ROW_B7_MASK 0x00000F00
#define DWC_DDR_UMCTL2_REGS_ADDRMAP10_ADDRMAP_ROW_B7_SHIFT 8
/*
 * Selects the HIF address bits used as row address bit 8.
 * Valid Range: 0 to 11
 * Internal Base: 14
 * The selected HIF address bit for each of the row address bits is determined by adding the
 * internal base to the value of this field. This register field is used only when
 * ADDRMAP5.addrmap_row_b2_10 is set to value 15.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP10_ADDRMAP_ROW_B8_MASK 0x000F0000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP10_ADDRMAP_ROW_B8_SHIFT 16
/*
 * Selects the HIF address bits used as row address bit 9.
 * Valid Range: 0 to 11
 * Internal Base: 15
 * The selected HIF address bit for each of the row address bits is determined by adding the
 * internal base to the value of this field. This register field is used only when
 * ADDRMAP5.addrmap_row_b2_10 is set to value 15.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP10_ADDRMAP_ROW_B9_MASK 0x0F000000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP10_ADDRMAP_ROW_B9_SHIFT 24

/**** ADDRMAP11 register ****/
/*
 * Selects the HIF address bits used as row address bit 10.
 * Valid Range: 0 to 11
 * Internal Base: 16
 * The selected HIF address bit for each of the row address bits is determined by adding the
 * internal base to the value of this field. This register field is used only when
 * ADDRMAP5.addrmap_row_b2_10 is set to value 15.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP11_ADDRMAP_ROW_B10_MASK 0x0000000F
#define DWC_DDR_UMCTL2_REGS_ADDRMAP11_ADDRMAP_ROW_B10_SHIFT 0
/*
 * Selects the HIF address bit used as chip id bit 0.
 * Valid Range: 0 to 29
 * Internal Base: 4
 * The selected HIF address bit is determined by adding the internal base to the value of this
 * field. If set to 31, chip id bit 0 is set to 0.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP11_ADDRMAP_CID_B0_MASK 0x00001F00
#define DWC_DDR_UMCTL2_REGS_ADDRMAP11_ADDRMAP_CID_B0_SHIFT 8
/*
 * Selects the HIF address bit used as chip id bit 1.
 * Valid Range: 0 to 29
 * Internal Base: 5
 * The selected HIF address bit is determined by adding the internal base to the value of this
 * field. If set to 31, chip id bit 1 is set to 0.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP11_ADDRMAP_CID_B1_MASK 0x001F0000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP11_ADDRMAP_CID_B1_SHIFT 16

/*
 *
 * The delay, in clock cycles, from issuing a read command to setting ODT values associated with
 * that command. ODT setting must remain constant for the entire time that DQS is driven by the
 * uMCTL2. ODT is used only in DDR2, DDR3, DDR4 and LPDDR3 designs.<br>
 *  Recommended values:<br>
 *  DDR2
 *  - If (CL + AL < 4), then 0.
 *  - If (CL + AL >= 4), then (CL + AL - 4)
 *  DDR3
 *  - (CL - CWL)
 *  DDR4
 *  - If CAL mode is enabled, CL - CWL + DFITMG1.dfi_t_cmd_lat
 *  - If CAL mode is not enabled, CL - CWL -1, or 0 if CL - CWL < 1
 *  LPDDR3, MEMC_FREQ_RATIO=2
 *   - CL - RU(tODToffmax/tCK))
 */
#define DWC_DDR_UMCTL2_REGS_ODTCFG_RD_ODT_DELAY_MASK 0x0000007C
#define DWC_DDR_UMCTL2_REGS_ODTCFG_RD_ODT_DELAY_SHIFT 2
/*
 *
 * Cycles to hold ODT for a read command. The minimum supported value is 2.
 * Recommended values:<br>
 *  DDR2/DDR3
 *  - BL8 - 0x6
 *  - BL4 - 0x4
 *  DDR4 - 0x6, but needs to be reduced to 0x5 in CAL mode to avoid overlap of read and write ODT
 *  LPDDR3 - RU(tDQSCKmax/tCK) + 4 + 1
 */
#define DWC_DDR_UMCTL2_REGS_ODTCFG_RD_ODT_HOLD_MASK 0x00000F00
#define DWC_DDR_UMCTL2_REGS_ODTCFG_RD_ODT_HOLD_SHIFT 8
/*
 *
 * The delay, in clock cycles, from issuing a write command to setting ODT values associated with
 * that command. ODT setting must remain constant for the entire time that DQS is driven by the
 * uMCTL2. ODT is used only in DDR2, DDR3, DDR4 and LPDDR3 designs.<br>
 *  Recommended values:<br>
 *   DDR2
 *  - If (CWL + AL < 3), then 0.
 *  - If (CWL + AL >= 3), then (CWL + AL - 3)
 *  DDR3 - 0<br>
 *  DDR4 - DFITMG1.dfi_t_cmd_lat (to adjust for CAL mode)
 *  LPDDR3 - (CWL - RU(tODToffmax/tCK))
 */
#define DWC_DDR_UMCTL2_REGS_ODTCFG_WR_ODT_DELAY_MASK 0x001F0000
#define DWC_DDR_UMCTL2_REGS_ODTCFG_WR_ODT_DELAY_SHIFT 16
/*
 *
 * Cycles to hold ODT for a write command. The minimum supported value is 2.
 *  DDR2/DDR3/DDR4
 *  - BL8 - 0x6
 *  - BL4 - 0x4
 *  LPDDR3 - RU(tDQSSmax/tCK) + 4
 */
#define DWC_DDR_UMCTL2_REGS_ODTCFG_WR_ODT_HOLD_MASK 0x0F000000
#define DWC_DDR_UMCTL2_REGS_ODTCFG_WR_ODT_HOLD_SHIFT 24

/**** ODTMAP register ****/
/*
 * Indicates which remote ODTs must be turned on during a write to rank 0.<br>Each rank has a remote
 * ODT (in the SDRAM) which can be turned on by setting the appropriate bit here.<br>Rank 0 is
 * controlled by the LSB; rank 1 is controlled by bit next to the LSB, etc.<br>For each rank, set
 * its bit to 1 to enable its ODT.
 */
#define DWC_DDR_UMCTL2_REGS_ODTMAP_RANK0_WR_ODT_MASK 0x0000000F
#define DWC_DDR_UMCTL2_REGS_ODTMAP_RANK0_WR_ODT_SHIFT 0
/*
 * Indicates which remote ODTs must be turned on during a read from rank 0.<br>Each rank has a
 * remote ODT (in the SDRAM) which can be turned on by setting the appropriate bit here.<br>Rank 0
 * is controlled by the LSB; rank 1 is controlled by bit next to the LSB, etc.<br>For each rank, set
 * its bit to 1 to enable its ODT.
 */
#define DWC_DDR_UMCTL2_REGS_ODTMAP_RANK0_RD_ODT_MASK 0x000000F0
#define DWC_DDR_UMCTL2_REGS_ODTMAP_RANK0_RD_ODT_SHIFT 4
/*
 * Indicates which remote ODTs must be turned on during a write to rank 1.<br>Each rank has a remote
 * ODT (in the SDRAM) which can be turned on by setting the appropriate bit here.<br>Rank 0 is
 * controlled by the LSB; rank 1 is controlled by bit next to the LSB, etc.<br>For each rank, set
 * its bit to 1 to enable its ODT.<br>Present only in configurations that have 2 or more ranks
 */
#define DWC_DDR_UMCTL2_REGS_ODTMAP_RANK1_WR_ODT_MASK 0x00000F00
#define DWC_DDR_UMCTL2_REGS_ODTMAP_RANK1_WR_ODT_SHIFT 8
/*
 * Indicates which remote ODTs must be turned on during a read from rank 1.<br>Each rank has a
 * remote ODT (in the SDRAM) which can be turned on by setting the appropriate bit here.<br>Rank 0
 * is controlled by the LSB; rank 1 is controlled by bit next to the LSB, etc.<br>For each rank, set
 * its bit to 1 to enable its ODT.<br>Present only in configurations that have 2 or more ranks
 */
#define DWC_DDR_UMCTL2_REGS_ODTMAP_RANK1_RD_ODT_MASK 0x0000F000
#define DWC_DDR_UMCTL2_REGS_ODTMAP_RANK1_RD_ODT_SHIFT 12
/*
 * Indicates which remote ODTs must be turned on during a write to rank 2.<br>Each rank has a remote
 * ODT (in the SDRAM) which can be turned on by setting the appropriate bit here.<br>Rank 0 is
 * controlled by the LSB; rank 1 is controlled by bit next to the LSB, etc.<br>For each rank, set
 * its bit to 1 to enable its ODT.<br>Present only in configurations that have 4 ranks
 */
#define DWC_DDR_UMCTL2_REGS_ODTMAP_RANK2_WR_ODT_MASK 0x000F0000
#define DWC_DDR_UMCTL2_REGS_ODTMAP_RANK2_WR_ODT_SHIFT 16
/*
 * Indicates which remote ODTs must be turned on during a read from rank 2.<br>Each rank has a
 * remote ODT (in the SDRAM) which can be turned on by setting the appropriate bit here.<br>Rank 0
 * is controlled by the LSB; rank 1 is controlled by bit next to the LSB, etc.<br>For each rank, set
 * its bit to 1 to enable its ODT.<br>Present only in configurations that have 4 ranks
 */
#define DWC_DDR_UMCTL2_REGS_ODTMAP_RANK2_RD_ODT_MASK 0x00F00000
#define DWC_DDR_UMCTL2_REGS_ODTMAP_RANK2_RD_ODT_SHIFT 20
/*
 * Indicates which remote ODTs must be turned on during a write to rank 3.<br>Each rank has a remote
 * ODT (in the SDRAM) which can be turned on by setting the appropriate bit here.<br>Rank 0 is
 * controlled by the LSB; rank 1 is controlled by bit next to the LSB, etc. For each rank, set its
 * bit to 1 to enable its ODT.<br>Present only in configurations that have 4 ranks
 */
#define DWC_DDR_UMCTL2_REGS_ODTMAP_RANK3_WR_ODT_MASK 0x0F000000
#define DWC_DDR_UMCTL2_REGS_ODTMAP_RANK3_WR_ODT_SHIFT 24
/*
 * Indicates which remote ODTs must be turned on during a read from rank 3.<br>Each rank has a
 * remote ODT (in the SDRAM) which can be turned on by setting the appropriate bit here.<br>Rank 0
 * is controlled by the LSB; rank 1 is controlled by bit next to the LSB, etc. For each rank, set
 * its bit to 1 to enable its ODT.<br>Present only in configurations that have 4 ranks
 */
#define DWC_DDR_UMCTL2_REGS_ODTMAP_RANK3_RD_ODT_MASK 0xF0000000
#define DWC_DDR_UMCTL2_REGS_ODTMAP_RANK3_RD_ODT_SHIFT 28

/**** SCHED register ****/
/*
 * Active low signal. When asserted ('0'), all incoming transactions are forced to low priority.
 * This implies that all High Priority Read (HPR) and Variable Priority Read commands (VPR) will be
 * treated as Low Priority Read (LPR) commands. On the write side, all Variable Priority Write (VPW)
 * commands will be treated as Normal Priority Write (NPW) commands. Forcing the incoming
 * transactions to low priority implicitly turns off Bypass path for read commands.<br>FOR
 * PERFORMANCE ONLY.
 */
#define DWC_DDR_UMCTL2_REGS_SCHED_FORCE_LOW_PRI_N (1 << 0)
/* If set then the bank selector prefers writes over reads.<br>FOR DEBUG ONLY. */
#define DWC_DDR_UMCTL2_REGS_SCHED_PREFER_WRITE (1 << 1)
/*
 * If true, bank is kept open only while there are page hit transactions available in the CAM to
 * that bank. The last read or write command in the CAM with a bank and page hit will be executed
 * with auto-precharge if SCHED1.pageclose_timer=0. Even if this register set to 1 and
 * SCHED1.pageclose_timer is set to 0, explicit precharge (and not auto-precharge) may be issued in
 * some cases where there is a mode switch between Write and Read or between LPR and HPR. The Read
 * and Write commands that are executed as part of the ECC scrub requests are also executed without
 * auto-precharge.</br>
 * If false, the bank remains open until there is a need to close it (to open a different page, or
 * for page timeout or refresh timeout) - also known as open page policy. The open page policy can
 * be overridden by setting the per-command-autopre bit on the HIF interface
 * (hif_cmd_autopre).<br>The pageclose feature provids a midway between Open and Close page
 * policies.</br>FOR PERFORMANCE ONLY.
 */
#define DWC_DDR_UMCTL2_REGS_SCHED_PAGECLOSE_RD (1 << 2)
#define DWC_DDR_UMCTL2_REGS_SCHED_PAGECLOSE_WR (1 << 3)
/*
 * Number of entries in the low priority transaction store is this value + 1.<br>(MEMC_NO_OF_ENTRY -
 * (SCHED.lpr_num_entries + 1)) is the number of entries available for the high priority transaction
 * store.<br>Setting this to maximum value allocates all entries to low priority transaction
 * store.<br>Setting this to 0 allocates 1 entry to low priority transaction store and the rest to
 * high priority transaction store.<br><b>Note:</b> In ECC configurations, the numbers of write and
 * low priority read credits issued is one less than in the non-ECC case. One entry each is reserved
 * in the write and low-priority read CAMs for storing the RMW requests arising out of single bit
 * error correction RMW operation.
 */
#define DWC_DDR_UMCTL2_REGS_SCHED_LPR_NUM_ENTRIES_MASK 0x00003F00
#define DWC_DDR_UMCTL2_REGS_SCHED_LPR_NUM_ENTRIES_SHIFT 8
/* Describes the number of cycles that co_gs_go2critical_rd or c ... */
#define DWC_DDR_UMCTL2_REGS_SCHED_GO2CRITICAL_HYSTERESIS_MASK 0x00FF0000
#define DWC_DDR_UMCTL2_REGS_SCHED_GO2CRITICAL_HYSTERESIS_SHIFT 16
/*
 * When the preferred transaction store is empty for these many clock cycles, switch to the
 * alternate transaction store if it is non-empty.<br>The read transaction store (both high and low
 * priority) is the default preferred transaction store and the write transaction store is the
 * alternative store.<br>When prefer write over read is set this is reversed.<br>0x0 is a legal
 * value for this register. When set to 0x0, the transaction store switching will happen immediately
 * when the switching conditions become true.<br>FOR PERFORMANCE ONLY
 */
#define DWC_DDR_UMCTL2_REGS_SCHED_RDWR_IDLE_GAP_MASK 0x7F000000
#define DWC_DDR_UMCTL2_REGS_SCHED_RDWR_IDLE_GAP_SHIFT 24

/**** SCHED1 register ****/
/*
 * This field works in conjunction with SCHED.pageclose.
 * It only has meaning if SCHED.pageclose==1.
 *
 * If SCHED.pageclose==1 and pageclose_timer==0, then an auto-precharge may be scheduled for last
 * read
 * or write command in the CAM with a bank and page hit.
 * Note, sometimes an explicit precharge is scheduled instead of the auto-precharge. See
 * SCHED.pageclose for details of when this may happen.
 *
 * If SCHED.pageclose==1 and pageclose_timer>0, then an auto-precharge is not scheduled for last
 * read
 * or write command in the CAM with a bank and page hit.
 * Instead, a timer is started, with pageclose_timer as the initial value.
 * There is a timer on a per bank basis.
 * The timer decrements unless the next read or write in the CAM to a bank is a page hit.
 * It gets reset to pageclose_timer value if the next read or write in the CAM to a bank is a page
 * hit.
 * Once the timer has reached zero, an explcit precharge will be attempted to be scheduled.
 */
#define DWC_DDR_UMCTL2_REGS_SCHED1_PAGECLOSE_RD_TIMER_MASK 0x000000FF
#define DWC_DDR_UMCTL2_REGS_SCHED1_PAGECLOSE_RD_TIMER_SHIFT 0
#define DWC_DDR_UMCTL2_REGS_SCHED1_PAGECLOSE_WR_TIMER_MASK 0x0000FF00
#define DWC_DDR_UMCTL2_REGS_SCHED1_PAGECLOSE_WR_TIMER_SHIFT 8

/**** PERFHPR0 register ****/
/* Number of clocks that the HPR queue is guaranteed to stay in  ... */
#define DWC_DDR_UMCTL2_REGS_PERFHPR0_HPR_MIN_NON_CRITICAL_MASK 0x0000FFFF
#define DWC_DDR_UMCTL2_REGS_PERFHPR0_HPR_MIN_NON_CRITICAL_SHIFT 0

/**** PERFHPR1 register ****/
/*
 * Number of clocks that the HPR queue can be starved before it goes critical. The minimum valid
 * functional value for this register is 0x1. Programming it to 0x0 will disable the starvation
 * functionality; during normal operation, this function should not be disabled as it will cause
 * excessive latencies.<br>Unit: Clock cycles.<br>FOR PERFORMANCE ONLY.
 */
#define DWC_DDR_UMCTL2_REGS_PERFHPR1_HPR_MAX_STARVE_MASK 0x0000FFFF
#define DWC_DDR_UMCTL2_REGS_PERFHPR1_HPR_MAX_STARVE_SHIFT 0
/*
 *
 * Number of transactions that are serviced once the HPR queue goes critical is the smaller of:
 *  - (a) This number
 *  - (b) Number of transactions available.
 *  Unit: Transaction.<br>FOR PERFORMANCE ONLY.
 */
#define DWC_DDR_UMCTL2_REGS_PERFHPR1_HPR_XACT_RUN_LENGTH_MASK 0xFF000000
#define DWC_DDR_UMCTL2_REGS_PERFHPR1_HPR_XACT_RUN_LENGTH_SHIFT 24

/**** PERFLPR0 register ****/
/* Number of clocks that the LPR queue is guaranteed to be non-c ... */
#define DWC_DDR_UMCTL2_REGS_PERFLPR0_LPR_MIN_NON_CRITICAL_MASK 0x0000FFFF
#define DWC_DDR_UMCTL2_REGS_PERFLPR0_LPR_MIN_NON_CRITICAL_SHIFT 0

/**** PERFLPR1 register ****/
/*
 * Number of clocks that the LPR queue can be starved before it goes critical. The minimum valid
 * functional value for this register is 0x1. Programming it to 0x0 will disable the starvation
 * functionality; during normal operation, this function should not be disabled as it will cause
 * excessive latencies.<br>Unit: Clock cycles.<br>FOR PERFORMANCE ONLY.
 */
#define DWC_DDR_UMCTL2_REGS_PERFLPR1_LPR_MAX_STARVE_MASK 0x0000FFFF
#define DWC_DDR_UMCTL2_REGS_PERFLPR1_LPR_MAX_STARVE_SHIFT 0
/*
 *
 * Number of transactions that are serviced once the LPR queue goes critical is the smaller of:
 *  - (a) This number
 *  - (b) Number of transactions available.
 * Unit: Transaction.<br>FOR PERFORMANCE ONLY.
 */
#define DWC_DDR_UMCTL2_REGS_PERFLPR1_LPR_XACT_RUN_LENGTH_MASK 0xFF000000
#define DWC_DDR_UMCTL2_REGS_PERFLPR1_LPR_XACT_RUN_LENGTH_SHIFT 24

/**** PERFWR0 register ****/
/* Number of clocks that the write queue is guaranteed to be non ... */
#define DWC_DDR_UMCTL2_REGS_PERFWR0_W_MIN_NON_CRITICAL_MASK 0x0000FFFF
#define DWC_DDR_UMCTL2_REGS_PERFWR0_W_MIN_NON_CRITICAL_SHIFT 0

/**** PERFWR1 register ****/
/*
 * Number of clocks that the WR queue can be starved before it goes critical. The minimum valid
 * functional value for this register is 0x1. Programming it to 0x0 will disable the starvation
 * functionality; during normal operation, this function should not be disabled as it will cause
 * excessive latencies.<br>Unit: Clock cycles.<br>FOR PERFORMANCE ONLY.
 */
#define DWC_DDR_UMCTL2_REGS_PERFWR1_W_MAX_STARVE_MASK 0x0000FFFF
#define DWC_DDR_UMCTL2_REGS_PERFWR1_W_MAX_STARVE_SHIFT 0
/*
 *
 * Number of transactions that are serviced once the WR queue goes critical is the smaller of:
 *  - (a) This number
 *  - (b) Number of transactions available.
 * Unit: Transaction.<br>FOR PERFORMANCE ONLY.
 */
#define DWC_DDR_UMCTL2_REGS_PERFWR1_W_XACT_RUN_LENGTH_MASK 0xFF000000
#define DWC_DDR_UMCTL2_REGS_PERFWR1_W_XACT_RUN_LENGTH_SHIFT 24

/**** DQMAP0 register ****/
/* DQ nibble map for DQ bits [0-3]<br>Present only in designs configured to support DDR4. */
#define DWC_DDR_UMCTL2_REGS_DQMAP0_DQ_NIBBLE_MAP_0_3_MASK 0x000000FF
#define DWC_DDR_UMCTL2_REGS_DQMAP0_DQ_NIBBLE_MAP_0_3_SHIFT 0
/* DQ nibble map for DQ bits [4-7]<br>Present only in designs configured to support DDR4. */
#define DWC_DDR_UMCTL2_REGS_DQMAP0_DQ_NIBBLE_MAP_4_7_MASK 0x0000FF00
#define DWC_DDR_UMCTL2_REGS_DQMAP0_DQ_NIBBLE_MAP_4_7_SHIFT 8
/* DQ nibble map for DQ bits [8-11]<br>Present only in designs configured to support DDR4. */
#define DWC_DDR_UMCTL2_REGS_DQMAP0_DQ_NIBBLE_MAP_8_11_MASK 0x00FF0000
#define DWC_DDR_UMCTL2_REGS_DQMAP0_DQ_NIBBLE_MAP_8_11_SHIFT 16
/* DQ nibble map for DQ bits [12-15]<br>Present only in designs configured to support DDR4. */
#define DWC_DDR_UMCTL2_REGS_DQMAP0_DQ_NIBBLE_MAP_12_15_MASK 0xFF000000
#define DWC_DDR_UMCTL2_REGS_DQMAP0_DQ_NIBBLE_MAP_12_15_SHIFT 24

/**** DQMAP1 register ****/
/* DQ nibble map for DQ bits [16-19]<br>Present only in designs configured to support DDR4. */
#define DWC_DDR_UMCTL2_REGS_DQMAP1_DQ_NIBBLE_MAP_16_19_MASK 0x000000FF
#define DWC_DDR_UMCTL2_REGS_DQMAP1_DQ_NIBBLE_MAP_16_19_SHIFT 0
/* DQ nibble map for DQ bits [20-23]<br>Present only in designs configured to support DDR4. */
#define DWC_DDR_UMCTL2_REGS_DQMAP1_DQ_NIBBLE_MAP_20_23_MASK 0x0000FF00
#define DWC_DDR_UMCTL2_REGS_DQMAP1_DQ_NIBBLE_MAP_20_23_SHIFT 8
/* DQ nibble map for DQ bits [24-27]<br>Present only in designs configured to support DDR4. */
#define DWC_DDR_UMCTL2_REGS_DQMAP1_DQ_NIBBLE_MAP_24_27_MASK 0x00FF0000
#define DWC_DDR_UMCTL2_REGS_DQMAP1_DQ_NIBBLE_MAP_24_27_SHIFT 16
/* DQ nibble map for DQ bits [28-31]<br>Present only in designs configured to support DDR4. */
#define DWC_DDR_UMCTL2_REGS_DQMAP1_DQ_NIBBLE_MAP_28_31_MASK 0xFF000000
#define DWC_DDR_UMCTL2_REGS_DQMAP1_DQ_NIBBLE_MAP_28_31_SHIFT 24

/**** DQMAP2 register ****/
/* DQ nibble map for DQ bits [32-35]<br>Present only in designs configured to support DDR4. */
#define DWC_DDR_UMCTL2_REGS_DQMAP2_DQ_NIBBLE_MAP_32_35_MASK 0x000000FF
#define DWC_DDR_UMCTL2_REGS_DQMAP2_DQ_NIBBLE_MAP_32_35_SHIFT 0
/* DQ nibble map for DQ bits [36-39]<br>Present only in designs configured to support DDR4. */
#define DWC_DDR_UMCTL2_REGS_DQMAP2_DQ_NIBBLE_MAP_36_39_MASK 0x0000FF00
#define DWC_DDR_UMCTL2_REGS_DQMAP2_DQ_NIBBLE_MAP_36_39_SHIFT 8
/* DQ nibble map for DQ bits [40-43]<br>Present only in designs configured to support DDR4. */
#define DWC_DDR_UMCTL2_REGS_DQMAP2_DQ_NIBBLE_MAP_40_43_MASK 0x00FF0000
#define DWC_DDR_UMCTL2_REGS_DQMAP2_DQ_NIBBLE_MAP_40_43_SHIFT 16
/* DQ nibble map for DQ bits [44-47]<br>Present only in designs configured to support DDR4. */
#define DWC_DDR_UMCTL2_REGS_DQMAP2_DQ_NIBBLE_MAP_44_47_MASK 0xFF000000
#define DWC_DDR_UMCTL2_REGS_DQMAP2_DQ_NIBBLE_MAP_44_47_SHIFT 24

/**** DQMAP3 register ****/
/* DQ nibble map for DQ bits [48-51]<br>Present only in designs configured to support DDR4. */
#define DWC_DDR_UMCTL2_REGS_DQMAP3_DQ_NIBBLE_MAP_48_51_MASK 0x000000FF
#define DWC_DDR_UMCTL2_REGS_DQMAP3_DQ_NIBBLE_MAP_48_51_SHIFT 0
/* DQ nibble map for DQ bits [52-55]<br>Present only in designs configured to support DDR4. */
#define DWC_DDR_UMCTL2_REGS_DQMAP3_DQ_NIBBLE_MAP_52_55_MASK 0x0000FF00
#define DWC_DDR_UMCTL2_REGS_DQMAP3_DQ_NIBBLE_MAP_52_55_SHIFT 8
/* DQ nibble map for DQ bits [56-59]<br>Present only in designs configured to support DDR4. */
#define DWC_DDR_UMCTL2_REGS_DQMAP3_DQ_NIBBLE_MAP_56_59_MASK 0x00FF0000
#define DWC_DDR_UMCTL2_REGS_DQMAP3_DQ_NIBBLE_MAP_56_59_SHIFT 16
/* DQ nibble map for DQ bits [60-63]<br>Present only in designs configured to support DDR4. */
#define DWC_DDR_UMCTL2_REGS_DQMAP3_DQ_NIBBLE_MAP_60_63_MASK 0xFF000000
#define DWC_DDR_UMCTL2_REGS_DQMAP3_DQ_NIBBLE_MAP_60_63_SHIFT 24

/**** DQMAP4 register ****/
/*
 * DQ nibble map for DIMM ECC check bits [0-3]<br>Present only in designs configured to support
 * DDR4.
 */
#define DWC_DDR_UMCTL2_REGS_DQMAP4_DQ_NIBBLE_MAP_CB_0_3_MASK 0x000000FF
#define DWC_DDR_UMCTL2_REGS_DQMAP4_DQ_NIBBLE_MAP_CB_0_3_SHIFT 0
/*
 * DQ nibble map for DIMM ECC check bits [4-7]<br>Present only in designs configured to support
 * DDR4.
 */
#define DWC_DDR_UMCTL2_REGS_DQMAP4_DQ_NIBBLE_MAP_CB_4_7_MASK 0x0000FF00
#define DWC_DDR_UMCTL2_REGS_DQMAP4_DQ_NIBBLE_MAP_CB_4_7_SHIFT 8

/**** DQMAP5 register ****/
/*
 * <br>All even ranks have the same DQ mapping controled by DQMAP0-4 register as rank 0. This
 * register provides DQ swap function for all odd ranks to support CRC feature.<br>rank based DQ
 * swapping is:<br> swap bit 0 with 1, swap bit 2 with 3, swap bit 4 with 5 and swap bit 6 with
 * 7.<br>1: Disable rank based DQ swapping<br>0: Enable rank based DQ swapping<br>Present only in
 * designs configured to support DDR4.
 */
#define DWC_DDR_UMCTL2_REGS_DQMAP5_DIS_DQ_RANK_SWAP (1 << 0)

/**** DBG0 register ****/
/* When 1, disable write combine.<br>FOR DEBUG ONLY */
#define DWC_DDR_UMCTL2_REGS_DBG0_DIS_WC (1 << 0)
/*
 * Only present in designs supporting read bypass.<br>When 1, disable bypass path for high priority
 * read page hits<br>FOR DEBUG ONLY.
 */
#define DWC_DDR_UMCTL2_REGS_DBG0_DIS_RD_BYPASS (1 << 1)
/*
 * Only present in designs supporting activate bypass.<br>When 1, disable bypass path for high
 * priority read activates<br>FOR DEBUG ONLY.
 */
#define DWC_DDR_UMCTL2_REGS_DBG0_DIS_ACT_BYPASS (1 << 2)
/*
 * When this is set to '0', auto-precharge is disabled for the flushed command in a collision case.
 * Collision cases are write followed by read to same address, read followed by write to same
 * address, or write followed by write to same address with DBG0.dis_wc bit = 1 (where same address
 * comparisons exclude the two address bits representing critical word).<br>FOR DEBUG ONLY.
 */
#define DWC_DDR_UMCTL2_REGS_DBG0_DIS_COLLISION_PAGE_OPT (1 << 4)

/**** DBG1 register ****/
/*
 * When 1, uMCTL2 will not de-queue any transactions from the CAM. Bypass is also disabled. All
 * transactions are queued in the CAM. No reads or writes are issued to SDRAM as long as this is
 * asserted.<br>This bit may be used to prevent reads or writes being issued by the uMCTL2, which
 * makes it safe to modify certain register fields associated with reads and writes (see User Guide
 * for details). After setting this bit, it is strongly recommended to poll
 * DBGCAM.wr_data_pipeline_empty and DBGCAM.rd_data_pipeline_empty, before making changes to any
 * registers which affect reads and writes. This will ensure that the relevant logic in the DDRC is
 * idle.<br>This bit is intended to be switched on-the-fly.
 */
#define DWC_DDR_UMCTL2_REGS_DBG1_DIS_DQ (1 << 0)
/*
 * When 1, uMCTL2 asserts the HIF command signal hif_cmd_stall. uMCTL2 will ignore the hif_cmd_valid
 * and all other associated request signals.<br>This bit is intended to be switched on-the-fly.
 */
#define DWC_DDR_UMCTL2_REGS_DBG1_DIS_HIF (1 << 1)

/**** DBGCAM register ****/
/* High priority read queue depth<br>FOR DEBUG ONLY */
#define DWC_DDR_UMCTL2_REGS_DBGCAM_DBG_HPR_Q_DEPTH_MASK 0x0000007F
#define DWC_DDR_UMCTL2_REGS_DBGCAM_DBG_HPR_Q_DEPTH_SHIFT 0
/* Low priority read queue depth<br>FOR DEBUG ONLY */
#define DWC_DDR_UMCTL2_REGS_DBGCAM_DBG_LPR_Q_DEPTH_MASK 0x00007F00
#define DWC_DDR_UMCTL2_REGS_DBGCAM_DBG_LPR_Q_DEPTH_SHIFT 8
/* Write queue depth<br>FOR DEBUG ONLY */
#define DWC_DDR_UMCTL2_REGS_DBGCAM_DBG_W_Q_DEPTH_MASK 0x007F0000
#define DWC_DDR_UMCTL2_REGS_DBGCAM_DBG_W_Q_DEPTH_SHIFT 16
/* Stall<br>FOR DEBUG ONLY */
#define DWC_DDR_UMCTL2_REGS_DBGCAM_DBG_STALL (1 << 24)
/*
 * When 1, all the Read command queues and Read data buffers inside DDRC are empty. This register is
 * to be used for debug purpose. <br>An example use-case scenario: When Controller enters
 * Self-Refresh using the Low-Power entry sequence, Controller is expected to have executed all the
 * commands in its queues and the write and read data drained. Hence this register should be 1 at
 * that time.<br>FOR DEBUG ONLY
 */
#define DWC_DDR_UMCTL2_REGS_DBGCAM_DBG_RD_Q_EMPTY (1 << 25)
/*
 * When 1, all the Write command queues and Write data buffers inside DDRC are empty. This register
 * is to be used for debug purpose. <br>An example use-case scenario: When Controller enters
 * Self-Refresh using the Low-Power entry sequence, Controller is expected to have executed all the
 * commands in its queues and the write and read data drained. Hence this register should be 1 at
 * that time.<br>FOR DEBUG ONLY
 */
#define DWC_DDR_UMCTL2_REGS_DBGCAM_DBG_WR_Q_EMPTY (1 << 26)
/*
 * This bit indicates that the read data pipeline on the DFI interface is empty. This register is
 * intended to be polled after setting DBG1.dis_dq, to ensure that all remaining commands/data have
 * completed.
 */
#define DWC_DDR_UMCTL2_REGS_DBGCAM_RD_DATA_PIPELINE_EMPTY (1 << 28)
/*
 * This bit indicates that the write data pipeline on the DFI interface is empty. This register is
 * intended to be polled after setting DBG1.dis_dq, to ensure that all remaining commands/data have
 * completed.
 */
#define DWC_DDR_UMCTL2_REGS_DBGCAM_WR_DATA_PIPELINE_EMPTY (1 << 29)

#define DWC_DDR_UMCTL2_REGS_DBGCAM_DBG_STALL_WR (1 << 30)

#define DWC_DDR_UMCTL2_REGS_DBGCAM_DBG_STALL_RD (1 << 31)

/**** DBGCMD register ****/
/*
 *
 * Setting this register bit to 1 indicates to the uMCTL2 to issue a refresh to rank 0. Writing to
 * this bit causes DBGSTAT.rank0_refresh_busy to be set. When DBGSTAT.rank0_refresh_busy is cleared,
 * the command has been stored in uMCTL2. This operation can be performed only when
 * RFSHCTL3.dis_auto_refresh=1. It is recommended NOT to set this register bit if in Init or Deep
 * power-down operating modes or Maximum Power Saving Mode.
 */
#define DWC_DDR_UMCTL2_REGS_DBGCMD_RANK0_REFRESH (1 << 0)
/*
 *
 * Setting this register bit to 1 indicates to the uMCTL2 to issue a refresh to rank 1. Writing to
 * this bit causes DBGSTAT.rank1_refresh_busy to be set. When DBGSTAT.rank1_refresh_busy is cleared,
 * the command has been stored in uMCTL2. This operation can be performed only when
 * RFSHCTL3.dis_auto_refresh=1. It is recommended NOT to set this register bit if in Init or Deep
 * power-down operating modes or Maximum Power Saving Mode.
 */
#define DWC_DDR_UMCTL2_REGS_DBGCMD_RANK1_REFRESH (1 << 1)
/*
 *
 * Setting this register bit to 1 indicates to the uMCTL2 to issue a refresh to rank 2. Writing to
 * this bit causes DBGSTAT.rank2_refresh_busy to be set. When DBGSTAT.rank2_refresh_busy is cleared,
 * the command has been stored in uMCTL2. This operation can be performed only when
 * RFSHCTL3.dis_auto_refresh=1. It is recommended NOT to set this register bit if in Init or Deep
 * power-down operating modes or Maximum Power Saving Mode.
 */
#define DWC_DDR_UMCTL2_REGS_DBGCMD_RANK2_REFRESH (1 << 2)
/*
 *
 * Setting this register bit to 1 indicates to the uMCTL2 to issue a refresh to rank 3. Writing to
 * this bit causes DBGSTAT.rank3_refresh_busy to be set. When DBGSTAT.rank3_refresh_busy is cleared,
 * the command has been stored in uMCTL2. This operation can be performed only when
 * RFSHCTL3.dis_auto_refresh=1. It is recommended NOT to set this register bit if in Init or Deep
 * power-down operating modes or Maximum Power Saving Mode.
 */
#define DWC_DDR_UMCTL2_REGS_DBGCMD_RANK3_REFRESH (1 << 3)
/*
 *
 * Setting this register bit to 1 indicates to the uMCTL2 to issue a ZQCS (ZQ calibration short)
 * command to the SDRAM. When this request is stored in the uMCTL2, the bit is automatically
 * cleared. This operation can be performed only when ZQCTL0.dis_auto_zq=1. It is recommended NOT to
 * set this register bit if in Init operating mode. This register bit is ignored when in
 * Self-Refresh and Deep power-down operating modes and Maximum Power Saving Mode.
 */
#define DWC_DDR_UMCTL2_REGS_DBGCMD_ZQ_CALIB_SHORT (1 << 4)
/*
 *
 * Setting this register bit to 1 indicates to the uMCTL2 to issue a dfi_ctrlupd_req to the PHY.
 * When this request is stored in the uMCTL2, the bit is automatically cleared. This operation must
 * only be performed when DFIUPD0.dis_auto_ctrlupd=1.
 */
#define DWC_DDR_UMCTL2_REGS_DBGCMD_CTRLUPD (1 << 5)

#define DWC_DDR_UMCTL2_REGS_DBGCMD_RANK4_REFRESH (1 << 8)

#define DWC_DDR_UMCTL2_REGS_DBGCMD_RANK5_REFRESH (1 << 9)

#define DWC_DDR_UMCTL2_REGS_DBGCMD_RANK6_REFRESH (1 << 10)

#define DWC_DDR_UMCTL2_REGS_DBGCMD_RANK7_REFRESH (1 << 11)

/**** DBGSTAT register ****/
/*
 * SoC core may initiate a rank0_refresh operation (refresh operation to rank 0) only if this signal
 * is low. This signal goes high in the clock after DBGCMD.rank0_refresh is set to one. It goes low
 * when the rank0_refresh operation is stored in the uMCTL2. It is recommended not to perform
 * rank0_refresh operations when this signal is high.
 *  - 0 - Indicates that the SoC core can initiate a rank0_refresh operation
 *  - 1 - Indicates that rank0_refresh operation has not been stored yet in the uMCTL2
 */
#define DWC_DDR_UMCTL2_REGS_DBGSTAT_RANK0_REFRESH_BUSY (1 << 0)
/*
 * SoC core may initiate a rank1_refresh operation (refresh operation to rank 1) only if this signal
 * is low. This signal goes high in the clock after DBGCMD.rank1_refresh is set to one. It goes low
 * when the rank1_refresh operation is stored in the uMCTL2. It is recommended not to perform
 * rank1_refresh operations when this signal is high.
 *  - 0 - Indicates that the SoC core can initiate a rank1_refresh operation
 *  - 1 - Indicates that rank1_refresh operation has not been stored yet in the uMCTL2
 */
#define DWC_DDR_UMCTL2_REGS_DBGSTAT_RANK1_REFRESH_BUSY (1 << 1)
/*
 * SoC core may initiate a rank2_refresh operation (refresh operation to rank 2) only if this signal
 * is low. This signal goes high in the clock after DBGCMD.rank2_refresh is set to one. It goes low
 * when the rank2_refresh operation is stored in the uMCTL2. It is recommended not to perform
 * rank2_refresh operations when this signal is high.
 *  - 0 - Indicates that the SoC core can initiate a rank2_refresh operation
 *  - 1 - Indicates that rank2_refresh operation has not been stored yet in the uMCTL2
 */
#define DWC_DDR_UMCTL2_REGS_DBGSTAT_RANK2_REFRESH_BUSY (1 << 2)
/*
 * SoC core may initiate a rank3_refresh operation (refresh operation to rank 3) only if this signal
 * is low. This signal goes high in the clock after DBGCMD.rank3_refresh is set to one. It goes low
 * when the rank3_refresh operation is stored in the uMCTL2. It is recommended not to perform
 * rank3_refresh operations when this signal is high.
 *  - 0 - Indicates that the SoC core can initiate a rank3_refresh operation
 *  - 1 - Indicates that rank3_refresh operation has not been stored yet in the uMCTL2
 */
#define DWC_DDR_UMCTL2_REGS_DBGSTAT_RANK3_REFRESH_BUSY (1 << 3)
/*
 * SoC core may initiate a ZQCS (ZQ calibration short) operation only if this signal is low. This
 * signal goes high in the clock after the uMCTL2 accepts the ZQCS request. It goes low when the
 * ZQCS operation is initiated in the uMCTL2. It is recommended not to perform ZQCS operations when
 * this signal is high.
 *  - 0 - Indicates that the SoC core can initiate a ZQCS operation
 *  - 1 - Indicates that ZQCS operation has not been initiated yet in the uMCTL2
 */
#define DWC_DDR_UMCTL2_REGS_DBGSTAT_ZQ_CALIB_SHORT_BUSY (1 << 4)
/*
 * SoC core may initiate a ctrlupd operation only if this signal is low. This signal goes high in
 * the clock after the uMCTL2 accepts the ctrlupd request. It goes low when the ctrlupd operation is
 * initiated in the uMCTL2. It is recommended not to perform ctrlupd operations when this signal is
 * high.
 *  - 0 - Indicates that the SoC core can initiate a ctrlupd operation
 *  - 1 - Indicates that ctrlupd operation has not been initiated yet in the uMCTL2
 */
#define DWC_DDR_UMCTL2_REGS_DBGSTAT_CTRLUPD_BUSY (1 << 5)

#define DWC_DDR_UMCTL2_REGS_DBGSTAT_RANK4_REFRESH_BUSY (1 << 8)

#define DWC_DDR_UMCTL2_REGS_DBGSTAT_RANK5_REFRESH_BUSY (1 << 9)

#define DWC_DDR_UMCTL2_REGS_DBGSTAT_RANK6_REFRESH_BUSY (1 << 10)

#define DWC_DDR_UMCTL2_REGS_DBGSTAT_RANK7_REFRESH_BUSY (1 << 11)

/**** SWCTL register ****/
/*
 * Enable quasi-dynamic register programming outside reset. Program register to 0 to enable
 * quasi-dynamic programming. Set back register to 1 once programming is done.
 */
#define DWC_DDR_UMCTL2_REGS_SWCTL_SW_DONE (1 << 0)

/**** SWSTAT register ****/
/*
 * Register programming done. This register is the echo of SWCTL.sw_done. Wait for sw_done value 1
 * to propagate to sw_done_ack at the end of the programming sequence to ensure that the correct
 * registers values are propagated to the destination clock domains.
 */
#define DWC_DDR_UMCTL2_REGS_SWSTAT_SW_DONE_ACK (1 << 0)

/**** OCPARCFG0 register ****/
/* Parity enable register. Enables On-Chip parity for all interfaces. */
#define DWC_DDR_UMCTL2_REGS_OCPARCFG0_OC_PARITY_EN (1 << 0)
/*
 * Parity type: <br>
 *   - 0: Even parity
 *   - 1: Odd parity
 */
#define DWC_DDR_UMCTL2_REGS_OCPARCFG0_OC_PARITY_TYPE (1 << 1)
/*
 * Operating mode for write data parity: <br>
 *   - 2'b00: Write data parity checking is disabled.
 *   - 2'b01: Write data parity checking at AXI is enabled.
 *   - 2'b10: Write data parity checking at DFI is enabled.
 *   - 2'b11: Write data parity checking at AXI and DFI is enabled.
 */
#define DWC_DDR_UMCTL2_REGS_OCPARCFG0_WRITE_DATA_PARITY_MODE_MASK 0x00000030
#define DWC_DDR_UMCTL2_REGS_OCPARCFG0_WRITE_DATA_PARITY_MODE_SHIFT 4
/*
 * Enables interrupt generation, if set to 1, for all ports, on signal par_wdata_in_err_intr_n upon
 * detection of parity error at the AXI interface.
 */
#define DWC_DDR_UMCTL2_REGS_OCPARCFG0_PAR_WDATA_IN_ERR_INTR_EN (1 << 6)
/*
 * Enables interrupt generation, if set to 1, on signal par_wdata_out_err_intr upon detection of
 * parity error at the DFI interface.
 */
#define DWC_DDR_UMCTL2_REGS_OCPARCFG0_PAR_WDATA_OUT_ERR_INTR_EN (1 << 7)
/*
 * Enables SLVERR generation on write response when write data parity error is detected at the AXI
 * interface.
 */
#define DWC_DDR_UMCTL2_REGS_OCPARCFG0_PAR_WDATA_SLVERR_EN (1 << 8)
/*
 * Interrupt clear bit for all par_wdata_in_err_intr_n and par_wdata_out_err_intr. At the end of the
 * operation, the uMCTL2 automatically clears this bit.
 */
#define DWC_DDR_UMCTL2_REGS_OCPARCFG0_PAR_WDATA_ERR_INTR_CLR (1 << 9)
/*
 * The operating mode for read data parity. <br>
 *   - 1'b0: Read data parity checking is disabled.
 *   - 1'b1: Read data parity checking at AXI is enabled.
 */
#define DWC_DDR_UMCTL2_REGS_OCPARCFG0_READ_DATA_PARITY_MODE (1 << 12)
/*
 * In case of an uncorrectable 2-bit ECC error, there can be two options to update the parity value
 * with respect to the bad ECC data. If this register is set to 0, parity is corrupted; If set to 1,
 * do nothing. Note that if the parity is poisoned, the read data parity check will flag it as a
 * parity error.
 */
#define DWC_DDR_UMCTL2_REGS_OCPARCFG0_READ_DATA_PARITY_WITH_UNCORR_ECC (1 << 13)
/*
 * Enables SLVERR generation on read response when read data parity error is detected at the AXI
 * interface.
 */
#define DWC_DDR_UMCTL2_REGS_OCPARCFG0_PAR_RDATA_SLVERR_EN (1 << 14)
/*
 * Enables interrupt generation, if set to 1, for all ports, on signal par_rdata_err_intr_n upon
 * detection of parity error at the AXI interface.
 */
#define DWC_DDR_UMCTL2_REGS_OCPARCFG0_PAR_RDATA_ERR_INTR_EN (1 << 15)
/*
 * Interrupt clear bit for all par_rdata_err_intr_n. At the end of the operation, the uMCTL2
 * automatically clears this bit.
 */
#define DWC_DDR_UMCTL2_REGS_OCPARCFG0_PAR_RDATA_ERR_INTR_CLR (1 << 16)
/*
 * The operating mode for address parity.<br>
 *   - 1'b0: Address parity (for both read and write) checking is disabled.
 *   - 1'b1: Address parity checking is enabled.
 */
#define DWC_DDR_UMCTL2_REGS_OCPARCFG0_ADDRESS_PARITY_MODE (1 << 24)
/*
 * Enables SLVERR generation on read response or write response when address parity error is
 * detected at the AXI interface.
 */
#define DWC_DDR_UMCTL2_REGS_OCPARCFG0_PAR_ADDR_SLVERR_EN (1 << 25)
/*
 * Enables interrupt generation, if set to 1, for all ports, on signal par_waddr_err_intr_n upon
 * detection of parity error on the AXI interface.
 */
#define DWC_DDR_UMCTL2_REGS_OCPARCFG0_PAR_WADDR_ERR_INTR_EN (1 << 26)
/*
 * Interrupt clear bit for all par_waddr_err_intr_n. At the end of the operation, the uMCTL2
 * automatically clears this bit.
 */
#define DWC_DDR_UMCTL2_REGS_OCPARCFG0_PAR_WADDR_ERR_INTR_CLR (1 << 27)
/*
 * Enables interrupt generation, if set to 1, for all ports, on signal par_raddr_err_intr_n upon
 * detection of parity error on the AXI interface.
 */
#define DWC_DDR_UMCTL2_REGS_OCPARCFG0_PAR_RADDR_ERR_INTR_EN (1 << 28)
/*
 * Interrupt clear bit for all par_raddr_err_intr_n. At the end of the operation, the uMCTL2
 * automatically clears this bit.
 */
#define DWC_DDR_UMCTL2_REGS_OCPARCFG0_PAR_RADDR_ERR_INTR_CLR (1 << 29)

/**** OCPARCFG1 register ****/
/* Enables on-chip parity poisoning on the data interfaces. */
#define DWC_DDR_UMCTL2_REGS_OCPARCFG1_PAR_POISON_EN (1 << 0)
/*
 *
 * - On-chip parity poison mode. Continuous mode: If this register is set to 0, parity error is
 * injected continuously on every read or write data beat passing through the interfaces determined
 * by par_poison_loc* registers as long as par_poison_en register is enabled. <br>
 * - Trigger mode: If this register is set to 1, parity error is injected for one data beat when a
 * certain group of data patterns are detected on the selected interfaces determined by
 * par_poison_loc* registers. The patterns are specified in par_poison_data_pattern* registers.
 */
#define DWC_DDR_UMCTL2_REGS_OCPARCFG1_PAR_POISON_MODE (1 << 1)
/* Enables parity poisoning on read data at the DFI interface after the parity generation logic. */
#define DWC_DDR_UMCTL2_REGS_OCPARCFG1_PAR_POISON_LOC_RD_DFI (1 << 2)
/*
 * Enables parity poisoning on read data at the AXI interface after the parity check logic. The
 * value specifies the binary encoded port number of the AXI interface to be injected with parity
 * error. Error can be injected to one port at a time. An error injected here is not logged and does
 * not trigger SLVERR or interrupt by the controller.
 */
#define DWC_DDR_UMCTL2_REGS_OCPARCFG1_PAR_POISON_LOC_RD_PORT_MASK 0x000000F0
#define DWC_DDR_UMCTL2_REGS_OCPARCFG1_PAR_POISON_LOC_RD_PORT_SHIFT 4
/*
 * Enables parity poisoning on write data at the AXI interface before the input parity check logic.
 * The value specifies the binary encoded port number of the AXI interface to be injected with
 * parity error.
 */
#define DWC_DDR_UMCTL2_REGS_OCPARCFG1_PAR_POISON_LOC_WR_PORT_MASK 0x00000F00
#define DWC_DDR_UMCTL2_REGS_OCPARCFG1_PAR_POISON_LOC_WR_PORT_SHIFT 8
/*
 * Byte number (binary encoded) where the parity error is to be injected. Error can be injected one
 * byte at a time.
 */
#define DWC_DDR_UMCTL2_REGS_OCPARCFG1_PAR_POISON_BYTE_NUM_MASK 0x001F0000
#define DWC_DDR_UMCTL2_REGS_OCPARCFG1_PAR_POISON_BYTE_NUM_SHIFT 16

/**** OCPARSTAT0 register ****/
/*
 * Write address parity error interrupt for port 0. This interrupt is asserted when an on-chip write
 * address parity error occurred on the corresponding AXI port's write address channel.
 */
#define DWC_DDR_UMCTL2_REGS_OCPARSTAT0_PAR_WADDR_ERR_INTR_0 (1 << 0)
/*
 * Read address parity error interrupt for port 0. This interrupt is asserted when an on-chip read
 * address parity error occurred on the corresponding AXI port's read address channel.
 */
#define DWC_DDR_UMCTL2_REGS_OCPARSTAT0_PAR_RADDR_ERR_INTR_0 (1 << 16)

/**** OCPARSTAT1 register ****/
/*
 * Write data parity error interrupt on input for port 0. This interrupt is asserted when an on-chip
 * write data parity error occurred on the corresponding AXI port's write data channel. Bit 0
 * corresponds to Port 0, and so on. Cleared by register par_wdata_err_intr_clr.
 */
#define DWC_DDR_UMCTL2_REGS_OCPARSTAT1_PAR_WDATA_IN_ERR_INTR_0 (1 << 0)
/*
 * Read data parity error interrupt for port 0. This interrupt is asserted when an on-chip read data
 * parity error occurred on the corresponding AXI port's read data channel. Bit 0 corresponds to
 * Port 0, and so on. Cleared by register par_rdata_err_intr_clr.
 */
#define DWC_DDR_UMCTL2_REGS_OCPARSTAT1_PAR_RDATA_ERR_INTR_0 (1 << 16)

/**** OCPARWLOG0 register ****/
/* Write data parity error interrupt on output. */
#define DWC_DDR_UMCTL2_REGS_OCPARWLOG0_PAR_WDATA_OUT_ERR_INTR (1 << 0)
/*
 * Rank address of the last write transaction resulting in on-chip parity error on write data path
 * at the DFI interface (output).
 */
#define DWC_DDR_UMCTL2_REGS_OCPARWLOG0_PAR_WDATA_LOG_RANK_MASK 0x00000030
#define DWC_DDR_UMCTL2_REGS_OCPARWLOG0_PAR_WDATA_LOG_RANK_SHIFT 4
/*
 * Row address of the last write transaction resulting in on-chip parity error on write data path at
 * the DFI interface (output).
 */
#define DWC_DDR_UMCTL2_REGS_OCPARWLOG0_PAR_WDATA_LOG_ROW_MASK 0x03FFFF00
#define DWC_DDR_UMCTL2_REGS_OCPARWLOG0_PAR_WDATA_LOG_ROW_SHIFT 8
/*
 * Bank number of the last write transaction resulting in on-chip parity error on write data path at
 * the DFI interface (output).
 */
#define DWC_DDR_UMCTL2_REGS_OCPARWLOG0_PAR_WDATA_LOG_BANK_MASK 0x70000000
#define DWC_DDR_UMCTL2_REGS_OCPARWLOG0_PAR_WDATA_LOG_BANK_SHIFT 28

/**** OCPARWLOG1 register ****/
/*
 * Column address of the last write transaction resulting in on-chip parity error on write data path
 * at the DFI interface (output).
 */
#define DWC_DDR_UMCTL2_REGS_OCPARWLOG1_PAR_WDATA_LOG_COL_MASK 0x00000FFF
#define DWC_DDR_UMCTL2_REGS_OCPARWLOG1_PAR_WDATA_LOG_COL_SHIFT 0
/*
 * Bank group number of the last write transaction resulting in on-chip parity error on write data
 * path at the DFI interface (output).
 */
#define DWC_DDR_UMCTL2_REGS_OCPARWLOG1_PAR_WDATA_LOG_BG_MASK 0x00030000
#define DWC_DDR_UMCTL2_REGS_OCPARWLOG1_PAR_WDATA_LOG_BG_SHIFT 16

#define DWC_DDR_UMCTL2_REGS_OCPARWLOG1_PAR_WDATA_LOG_CID_MASK 0x03000000
#define DWC_DDR_UMCTL2_REGS_OCPARWLOG1_PAR_WDATA_LOG_CID_SHIFT 24

/**** OCPARAWLOG1 register ****/
/*
 * AXI system address [59:32] of the last write transaction resulting in on-chip parity error on
 * write address path at the AXI interface.
 */
#define DWC_DDR_UMCTL2_REGS_OCPARAWLOG1_PAR_WADDR_LOG_HIGH_MASK 0x00000FFF
#define DWC_DDR_UMCTL2_REGS_OCPARAWLOG1_PAR_WADDR_LOG_HIGH_SHIFT 0
/*
 * Failing port number (binary encoded) of the last write address transaction which resulted in
 * on-chip parity error at the AXI interface. If there are more than one simultaneous port failures,
 * the lower-indexed port is captured.
 */
#define DWC_DDR_UMCTL2_REGS_OCPARAWLOG1_PAR_WADDR_LOG_PORT_NUM_MASK 0xF0000000
#define DWC_DDR_UMCTL2_REGS_OCPARAWLOG1_PAR_WADDR_LOG_PORT_NUM_SHIFT 28

/**** OCPARRLOG1 register ****/
/*
 * Failing port number (binary encoded) of the last read data beat which resulted in on-chip parity
 * error at the AXI interface. If there are more than one simultaneous port failures, the
 * lower-indexed port is captured.
 */
#define DWC_DDR_UMCTL2_REGS_OCPARRLOG1_PAR_RDATA_LOG_PORT_NUM_MASK 0x0000000F
#define DWC_DDR_UMCTL2_REGS_OCPARRLOG1_PAR_RDATA_LOG_PORT_NUM_SHIFT 0

/**** OCPARARLOG1 register ****/
/*
 * AXI system address [59:32] of the last read transaction resulting in on-chip parity error on read
 * address path at the AXI interface.
 */
#define DWC_DDR_UMCTL2_REGS_OCPARARLOG1_PAR_RADDR_LOG_HIGH_MASK 0x00000FFF
#define DWC_DDR_UMCTL2_REGS_OCPARARLOG1_PAR_RADDR_LOG_HIGH_SHIFT 0
/*
 * Failing port number (binary encoded) of the last read address transaction which resulted in
 * on-chip parity error at the AXI interface. If there are more than one simultaneous port failures,
 * the lower-indexed port is captured.
 */
#define DWC_DDR_UMCTL2_REGS_OCPARARLOG1_PAR_RADDR_LOG_PORT_NUM_MASK 0xF0000000
#define DWC_DDR_UMCTL2_REGS_OCPARARLOG1_PAR_RADDR_LOG_PORT_NUM_SHIFT 28

/**** POISONCFG register ****/
/* If set to 1, enables SLVERR response for write transaction poisoning */
#define DWC_DDR_UMCTL2_REGS_POISONCFG_WR_POISON_SLVERR_EN (1 << 0)
/* If set to 1, enables interrupts for write transaction poisoning */
#define DWC_DDR_UMCTL2_REGS_POISONCFG_WR_POISON_INTR_EN (1 << 4)
/*
 * Interrupt clear for write transaction poisoning. Allow 2/3 clock cycles for correct value to
 * propagate to core logic and clear the interrupts.
 */
#define DWC_DDR_UMCTL2_REGS_POISONCFG_WR_POISON_INTR_CLR (1 << 8)
/* If set to 1, enables SLVERR response for read transaction poisoning */
#define DWC_DDR_UMCTL2_REGS_POISONCFG_RD_POISON_SLVERR_EN (1 << 16)
/* If set to 1, enables interrupts for read transaction poisoning */
#define DWC_DDR_UMCTL2_REGS_POISONCFG_RD_POISON_INTR_EN (1 << 20)
/*
 * Interrupt clear for read transaction poisoning. Allow 2/3 clock cycles for correct value to
 * propagate to core logic and clear the interrupts.
 */
#define DWC_DDR_UMCTL2_REGS_POISONCFG_RD_POISON_INTR_CLR (1 << 24)

/**** POISONSTAT register ****/
/*
 * Write transaction poisoning error interrupt for port 0. This register is a APB clock copy (double
 * register synchronizer) of the interrupt asserted when a transaction is poisoned on the
 * corresponding AXI port's write address channel. Bit 0 corresponds to Port 0, and so on. Interrupt
 * is cleared by register wr_poison_intr_clr, then value propagated to APB clock.
 */
#define DWC_DDR_UMCTL2_REGS_POISONSTAT_WR_POISON_INTR_0 (1 << 0)
/*
 * Read transaction poisoning error interrupt for port 0. This register is a APB clock copy (double
 * register synchronizer) of the interrupt asserted when a transaction is poisoned on the
 * corresponding AXI port's read address channel. Bit 0 corresponds to Port 0, and so on. Interrupt
 * is cleared by register rd_poison_intr_clr, then value propagated to APB clock.
 */
#define DWC_DDR_UMCTL2_REGS_POISONSTAT_RD_POISON_INTR_0 (1 << 16)

/**** ADVECCINDEX register ****/
/*
 * Selector of which DRAM beat data output to ECCCSYN0/1/2 as well as ECCUCSYN.
 *
 * In Advanced ECC, the syndrome consist of number of DRAM beats. This register select which beats
 * of data will be output to ECCCSYN0/1/2 and ECCUCSYN0/1/2 registers.
 */
#define DWC_DDR_UMCTL2_REGS_ADVECCINDEX_ECC_SYNDROME_SEL_MASK 0x00000007
#define DWC_DDR_UMCTL2_REGS_ADVECCINDEX_ECC_SYNDROME_SEL_SHIFT 0
/*
 * Selector of which error symbol's status output to ADVECCSTAT.advecc_err_symbol_pos and
 * advecc_err_symbol_bits. The default is first error symbol.
 *
 *   The value should be less than ADVECCSTAT.advecc_num_err_symbol.
 */
#define DWC_DDR_UMCTL2_REGS_ADVECCINDEX_ECC_ERR_SYMBOL_SEL_MASK 0x00000018
#define DWC_DDR_UMCTL2_REGS_ADVECCINDEX_ECC_ERR_SYMBOL_SEL_SHIFT 3
/*
 * Selector of which DRAM beat's poison pattern will be set by ECCPOISONPAT0/1/2 registers.
 *
 * For frequency ratio 1:1 mode, 2 DRAM beats can be poisoned. Set ecc_poison_beats_sel to 0 and
 * given ECCPOISONPAT0/1/2 to set 1st beat's poison pattern; set ecc_poison_beats_sel to 1 and given
 * ECCPOISONPAT0/1/2 to set 2nd beat's poison pattern.
 *
 * For frequency ratio 1:2 mode, 4 DRAM beats can be poisoned. Set ecc_poison_beats_sel from 0~3 to
 * set 1st to 4th beat's poison pattern.
 *
 *   The other value is reserved.
 */
#define DWC_DDR_UMCTL2_REGS_ADVECCINDEX_ECC_POISON_BEATS_SEL_MASK 0x000001E0
#define DWC_DDR_UMCTL2_REGS_ADVECCINDEX_ECC_POISON_BEATS_SEL_SHIFT 5

/**** ECCPOISONPAT2 register ****/
/*
 * Poison pattern for DRAM data[71:64].
 *   Only bit is set to 1 indicate poison (invert) corresponding DRAM bit.
 *
 *   It is indirect register. Selector is ADVECCINDEX.ecc_poison_beats_sel.
 */
#define DWC_DDR_UMCTL2_REGS_ECCPOISONPAT2_ECC_POISON_DATA_71_64_MASK 0x000000FF
#define DWC_DDR_UMCTL2_REGS_ECCPOISONPAT2_ECC_POISON_DATA_71_64_SHIFT 0

/**** PSTAT register ****/
/* Indicates if there are outstanding reads for port 0. */
#define DWC_DDR_UMCTL2_MP_PSTAT_RD_PORT_BUSY_0 (1 << 0)
/* Indicates if there are outstanding writes for port 0. */
#define DWC_DDR_UMCTL2_MP_PSTAT_WR_PORT_BUSY_0 (1 << 16)

/**** PCCFG register ****/
/*
 * If set to 1 (enabled), sets co_gs_go2critical_wr and co_gs_go2critical_lpr/co_gs_go2critical_hpr
 * signals going to DDRC based on urgent input (awurgent, arurgent) coming from AXI master. If set
 * to 0 (disabled), co_gs_go2critical_wr and co_gs_go2critical_lpr/co_gs_go2critical_hpr signals at
 * DDRC are driven to 1b'0.
 */
#define DWC_DDR_UMCTL2_MP_PCCFG_GO2CRITICAL_EN (1 << 0)
/*
 * Page match four limit. If set to 1, limits the number of consecutive same page DDRC transactions
 * that can be granted by the Port Arbiter to four when Page Match feature is enabled. If set to 0,
 * there is no limit imposed on number of consecutive same page DDRC transactions.
 */
#define DWC_DDR_UMCTL2_MP_PCCFG_PAGEMATCH_LIMIT (1 << 4)
#define DWC_DDR_UMCTL2_MP_PCCFG_BL_EXP_MODE (1 << 8)

/**** PCFGR_0 register ****/
/*
 * Determines the initial load value of read aging counters. These counters will be parallel loaded
 * after reset, or after each grant to the corresponding port. The aging counters down-count every
 * clock cycle where the port is requesting but not granted. The higher significant 5-bits of the
 * read aging counter sets the priority of the read channel of a given port. Port's priority will
 * increase as the higher significant 5-bits of the counter starts to decrease. When the aging
 * counter becomes 0, the corresponding port channel will have the highest priority level (timeout
 * condition - Priority0). For multi-port configurations, the aging counters cannot be used to set
 * port priorities when external dynamic priority inputs (arqos) are enabled (timeout is still
 * applicable). For single port configurations, the aging counters are only used when they timeout
 * (become 0) to force read-write direction switching. In this case, external dynamic priority
 * input, arqos (for reads only) can still be used to set the DDRC read priority (2 priority levels:
 * low priority read - LPR, high priority read - HPR) on a command by command basis. <br>Note: The
 * two LSBs of this register field are tied internally to 2'b00.
 */
#define DWC_DDR_UMCTL2_MP_PCFGR_0_RD_PORT_PRIORITY_MASK 0x000003FF
#define DWC_DDR_UMCTL2_MP_PCFGR_0_RD_PORT_PRIORITY_SHIFT 0
/*
 * If set to 1, read transactions with ID not covered by any of the virtual channel ID mapping
 * registers are not reordered.
 */
#define DWC_DDR_UMCTL2_MP_PCFGR_0_READ_REORDER_BYPASS_EN (1 << 11)
/* If set to 1, enables aging function for the read channel of the port. */
#define DWC_DDR_UMCTL2_MP_PCFGR_0_RD_PORT_AGING_EN (1 << 12)
/*
 * If set to 1, enables the AXI urgent sideband signal (arurgent). When enabled and arurgent is
 * asserted by the master, that port becomes the highest priority and
 * co_gs_go2critical_lpr/co_gs_go2critical_hpr signal to DDRC is asserted if enabled in
 * PCCFG.go2critical_en register. Note that arurgent signal can be asserted anytime and as long as
 * required which is independent of address handshaking (it is not associated with any particular
 * command).
 */
#define DWC_DDR_UMCTL2_MP_PCFGR_0_RD_PORT_URGENT_EN (1 << 13)
/*
 * If set to 1, enables the Page Match feature. If enabled, once a requesting port is granted, the
 * port is continued to be granted if the following immediate commands are to the same memory page
 * (same bank and same row). See also related PCCFG.pagematch_limit register.
 */
#define DWC_DDR_UMCTL2_MP_PCFGR_0_RD_PORT_PAGEMATCH_EN (1 << 14)
/* If set to 1, enables reads to be generated as "High Priority  ... */
#define DWC_DDR_UMCTL2_MP_PCFGR_0_RD_PORT_HPR_EN (1 << 15)

/**** PCFGW_0 register ****/
/*
 * Determines the initial load value of write aging counters. These counters will be parallel loaded
 * after reset, or after each grant to the corresponding port. The aging counters down-count every
 * clock cycle where the port is requesting but not granted. The higher significant 5-bits of the
 * write aging counter sets the initial priority of the write channel of a given port. Port's
 * priority will increase as the higher significant 5-bits of the counter starts to decrease. When
 * the aging counter becomes 0, the corresponding port channel will have the highest priority level.
 * <br>For multi-port configurations, the aging counters cannot be used to set port priorities when
 * external dynamic priority inputs (awqos) are enabled (timeout is still applicable). <br>For
 * single port configurations, the aging counters are only used when they timeout (become 0) to
 * force read-write direction switching. <br>Note: The two LSBs of this register field are tied
 * internally to 2'b00.
 */
#define DWC_DDR_UMCTL2_MP_PCFGW_0_WR_PORT_PRIORITY_MASK 0x000003FF
#define DWC_DDR_UMCTL2_MP_PCFGW_0_WR_PORT_PRIORITY_SHIFT 0
/* If set to 1, enables aging function for the write channel of the port. */
#define DWC_DDR_UMCTL2_MP_PCFGW_0_WR_PORT_AGING_EN (1 << 12)
/*
 * If set to 1, enables the AXI urgent sideband signal (awurgent). When enabled and awurgent is
 * asserted by the master, that port becomes the highest priority and co_gs_go2critical_wr signal to
 * DDRC is asserted if enabled in PCCFG.go2critical_en register.<br> Note that awurgent signal can
 * be asserted anytime and as long as required which is independent of address handshaking (it is
 * not associated with any particular command).
 */
#define DWC_DDR_UMCTL2_MP_PCFGW_0_WR_PORT_URGENT_EN (1 << 13)
/*
 * If set to 1, enables the Page Match feature. If enabled, once a requesting port is granted, the
 * port is continued to be granted if the following immediate commands are to the same memory page
 * (same bank and same row). See also related PCCFG.pagematch_limit register.
 */
#define DWC_DDR_UMCTL2_MP_PCFGW_0_WR_PORT_PAGEMATCH_EN (1 << 14)

/**** PCFGIDMASKCH0_0 register ****/
/* Determines the mask used in the ID mapping function for virtual channel m. */
#define DWC_DDR_UMCTL2_MP_PCFGIDMASKCH0_0_ID_MASK_MASK 0x1FFFFFFF
#define DWC_DDR_UMCTL2_MP_PCFGIDMASKCH0_0_ID_MASK_SHIFT 0

/**** PCFGIDVALUECH0_0 register ****/
/* Determines the value used in the ID mapping function for virtual channel m. */
#define DWC_DDR_UMCTL2_MP_PCFGIDVALUECH0_0_ID_VALUE_MASK 0x1FFFFFFF
#define DWC_DDR_UMCTL2_MP_PCFGIDVALUECH0_0_ID_VALUE_SHIFT 0

/**** PCFGIDMASKCH1_0 register ****/
/* Determines the mask used in the ID mapping function for virtual channel m. */
#define DWC_DDR_UMCTL2_MP_PCFGIDMASKCH1_0_ID_MASK_MASK 0x1FFFFFFF
#define DWC_DDR_UMCTL2_MP_PCFGIDMASKCH1_0_ID_MASK_SHIFT 0

/**** PCFGIDVALUECH1_0 register ****/
/* Determines the value used in the ID mapping function for virtual channel m. */
#define DWC_DDR_UMCTL2_MP_PCFGIDVALUECH1_0_ID_VALUE_MASK 0x1FFFFFFF
#define DWC_DDR_UMCTL2_MP_PCFGIDVALUECH1_0_ID_VALUE_SHIFT 0

/**** PCFGIDMASKCH2_0 register ****/
/* Determines the mask used in the ID mapping function for virtual channel m. */
#define DWC_DDR_UMCTL2_MP_PCFGIDMASKCH2_0_ID_MASK_MASK 0x1FFFFFFF
#define DWC_DDR_UMCTL2_MP_PCFGIDMASKCH2_0_ID_MASK_SHIFT 0

/**** PCFGIDVALUECH2_0 register ****/
/* Determines the value used in the ID mapping function for virtual channel m. */
#define DWC_DDR_UMCTL2_MP_PCFGIDVALUECH2_0_ID_VALUE_MASK 0x1FFFFFFF
#define DWC_DDR_UMCTL2_MP_PCFGIDVALUECH2_0_ID_VALUE_SHIFT 0

/**** PCFGIDMASKCH3_0 register ****/
/* Determines the mask used in the ID mapping function for virtual channel m. */
#define DWC_DDR_UMCTL2_MP_PCFGIDMASKCH3_0_ID_MASK_MASK 0x1FFFFFFF
#define DWC_DDR_UMCTL2_MP_PCFGIDMASKCH3_0_ID_MASK_SHIFT 0

/**** PCFGIDVALUECH3_0 register ****/
/* Determines the value used in the ID mapping function for virtual channel m. */
#define DWC_DDR_UMCTL2_MP_PCFGIDVALUECH3_0_ID_VALUE_MASK 0x1FFFFFFF
#define DWC_DDR_UMCTL2_MP_PCFGIDVALUECH3_0_ID_VALUE_SHIFT 0

/**** PCFGIDMASKCH4_0 register ****/
/* Determines the mask used in the ID mapping function for virtual channel m. */
#define DWC_DDR_UMCTL2_MP_PCFGIDMASKCH4_0_ID_MASK_MASK 0x1FFFFFFF
#define DWC_DDR_UMCTL2_MP_PCFGIDMASKCH4_0_ID_MASK_SHIFT 0

/**** PCFGIDVALUECH4_0 register ****/
/* Determines the value used in the ID mapping function for virtual channel m. */
#define DWC_DDR_UMCTL2_MP_PCFGIDVALUECH4_0_ID_VALUE_MASK 0x1FFFFFFF
#define DWC_DDR_UMCTL2_MP_PCFGIDVALUECH4_0_ID_VALUE_SHIFT 0

/**** PCFGIDMASKCH5_0 register ****/
/* Determines the mask used in the ID mapping function for virtual channel m. */
#define DWC_DDR_UMCTL2_MP_PCFGIDMASKCH5_0_ID_MASK_MASK 0x1FFFFFFF
#define DWC_DDR_UMCTL2_MP_PCFGIDMASKCH5_0_ID_MASK_SHIFT 0

/**** PCFGIDVALUECH5_0 register ****/
/* Determines the value used in the ID mapping function for virtual channel m. */
#define DWC_DDR_UMCTL2_MP_PCFGIDVALUECH5_0_ID_VALUE_MASK 0x1FFFFFFF
#define DWC_DDR_UMCTL2_MP_PCFGIDVALUECH5_0_ID_VALUE_SHIFT 0

/**** PCFGIDMASKCH6_0 register ****/
/* Determines the mask used in the ID mapping function for virtual channel m. */
#define DWC_DDR_UMCTL2_MP_PCFGIDMASKCH6_0_ID_MASK_MASK 0x1FFFFFFF
#define DWC_DDR_UMCTL2_MP_PCFGIDMASKCH6_0_ID_MASK_SHIFT 0

/**** PCFGIDVALUECH6_0 register ****/
/* Determines the value used in the ID mapping function for virtual channel m. */
#define DWC_DDR_UMCTL2_MP_PCFGIDVALUECH6_0_ID_VALUE_MASK 0x1FFFFFFF
#define DWC_DDR_UMCTL2_MP_PCFGIDVALUECH6_0_ID_VALUE_SHIFT 0

/**** PCFGIDMASKCH7_0 register ****/
/* Determines the mask used in the ID mapping function for virtual channel m. */
#define DWC_DDR_UMCTL2_MP_PCFGIDMASKCH7_0_ID_MASK_MASK 0x1FFFFFFF
#define DWC_DDR_UMCTL2_MP_PCFGIDMASKCH7_0_ID_MASK_SHIFT 0

/**** PCFGIDVALUECH7_0 register ****/
/* Determines the value used in the ID mapping function for virtual channel m. */
#define DWC_DDR_UMCTL2_MP_PCFGIDVALUECH7_0_ID_VALUE_MASK 0x1FFFFFFF
#define DWC_DDR_UMCTL2_MP_PCFGIDVALUECH7_0_ID_VALUE_SHIFT 0

/**** PCTRL_0 register ****/
/* Enables port n. */
#define DWC_DDR_UMCTL2_MP_PCTRL_0_PORT_EN (1 << 0)

/**** PCFGQOS0_0 register ****/
/*
 * Separation level1 indicating the end of region0 mapping; start of region0 is 0. Possible values
 * for level1 are 0 to 13 (for dual RAQ) or 0 to 14 (for single RAQ) which corresponds to arqos.
 * <br>Note that for PA, arqos values are used directly as port priorities, where the higher the
 * value corresponds to higher port priority.<br> All of the map_level* registers must be set to
 * distinct values.
 */
#define DWC_DDR_UMCTL2_MP_PCFGQOS0_0_RQOS_MAP_LEVEL1_MASK 0x0000000F
#define DWC_DDR_UMCTL2_MP_PCFGQOS0_0_RQOS_MAP_LEVEL1_SHIFT 0
/*
 * This bitfield indicates the traffic class of region 0.<br> Valid values are:<br> 0: LPR, 1: VPR,
 * 2: HPR.<br> For dual address queue configurations, region 0 maps to the blue address queue.<br>
 * In this case, valid values are: <br>0: LPR and 1: VPR only.<br> When VPR support is disabled
 * (UMCTL2_VPR_EN = 0) and traffic class of region0 is set to 1 (VPR), VPR traffic is aliased to LPR
 * traffic.
 */
#define DWC_DDR_UMCTL2_MP_PCFGQOS0_0_RQOS_MAP_REGION0_MASK 0x00030000
#define DWC_DDR_UMCTL2_MP_PCFGQOS0_0_RQOS_MAP_REGION0_SHIFT 16

#define DWC_DDR_UMCTL2_MP_PCFGQOS0_0_RQOS_MAP_REGION0_LPR	\
	(0 << DWC_DDR_UMCTL2_MP_PCFGQOS0_0_RQOS_MAP_REGION0_SHIFT)
#define DWC_DDR_UMCTL2_MP_PCFGQOS0_0_RQOS_MAP_REGION0_VPR	\
	(1 << DWC_DDR_UMCTL2_MP_PCFGQOS0_0_RQOS_MAP_REGION0_SHIFT)
#define DWC_DDR_UMCTL2_MP_PCFGQOS0_0_RQOS_MAP_REGION0_HPR	\
	(2 << DWC_DDR_UMCTL2_MP_PCFGQOS0_0_RQOS_MAP_REGION0_SHIFT)

/*
 * This bitfield indicates the traffic class of region 1.<br> Valid values are:<br>0 : LPR, 1: VPR,
 * 2: HPR.<br> For dual address queue configurations, region1 maps to the blue address queue.<br> In
 * this case, valid values are <br>0: LPR and 1: VPR only.<br> When VPR support is disabled
 * (UMCTL2_VPR_EN = 0) and traffic class of region 1 is set to 1 (VPR), VPR traffic is aliased to
 * LPR traffic.
 */
#define DWC_DDR_UMCTL2_MP_PCFGQOS0_0_RQOS_MAP_REGION1_MASK 0x00300000
#define DWC_DDR_UMCTL2_MP_PCFGQOS0_0_RQOS_MAP_REGION1_SHIFT 20

#define DWC_DDR_UMCTL2_MP_PCFGQOS0_0_RQOS_MAP_REGION1_LPR	\
	(0 << DWC_DDR_UMCTL2_MP_PCFGQOS0_0_RQOS_MAP_REGION1_SHIFT)
#define DWC_DDR_UMCTL2_MP_PCFGQOS0_0_RQOS_MAP_REGION1_VPR	\
	(1 << DWC_DDR_UMCTL2_MP_PCFGQOS0_0_RQOS_MAP_REGION1_SHIFT)
#define DWC_DDR_UMCTL2_MP_PCFGQOS0_0_RQOS_MAP_REGION1_HPR	\
	(2 << DWC_DDR_UMCTL2_MP_PCFGQOS0_0_RQOS_MAP_REGION1_SHIFT)

/**** PCFGQOS1_0 register ****/
/* Specifies the timeout value for transactions mapped to the blue address queue. */
#define DWC_DDR_UMCTL2_MP_PCFGQOS1_0_RQOS_MAP_TIMEOUTB_MASK 0x000007FF
#define DWC_DDR_UMCTL2_MP_PCFGQOS1_0_RQOS_MAP_TIMEOUTB_SHIFT 0
/* Specifies the timeout value for transactions mapped to the red address queue. */
#define DWC_DDR_UMCTL2_MP_PCFGQOS1_0_RQOS_MAP_TIMEOUTR_MASK 0x07FF0000
#define DWC_DDR_UMCTL2_MP_PCFGQOS1_0_RQOS_MAP_TIMEOUTR_SHIFT 16

/**** PCFGWQOS0_0 register ****/
/*
 * Separation level indicating the end of region0 mapping; start of region0 is 0. Possible values
 * for level1 are 0 to 14 which corresponds to awqos.
 * Note that for PA, awqos values are used directly as port priorities, where the higher the value
 * corresponds to higher port priority.
 */
#define DWC_DDR_UMCTL2_MP_PCFGWQOS0_0_WQOS_MAP_LEVEL_MASK 0x0000000F
#define DWC_DDR_UMCTL2_MP_PCFGWQOS0_0_WQOS_MAP_LEVEL_SHIFT 0
/*
 * This bitfield indicates the traffic class of region 0.
 *
 *	  Valid values are:
 *
 *	  0: NPW, 1: VPW.
 *
 * When VPW support is disabled (UMCTL2_VPW_EN = 0) and traffic class of region0 is set to 1 (VPW),
 * VPW traffic is aliased to NPW traffic.
 */
#define DWC_DDR_UMCTL2_MP_PCFGWQOS0_0_WQOS_MAP_REGION0_MASK 0x00030000
#define DWC_DDR_UMCTL2_MP_PCFGWQOS0_0_WQOS_MAP_REGION0_SHIFT 16

#define DWC_DDR_UMCTL2_MP_PCFGWQOS0_0_WQOS_MAP_REGION0_NPW	\
	(0 << DWC_DDR_UMCTL2_MP_PCFGWQOS0_0_WQOS_MAP_REGION0_SHIFT)
#define DWC_DDR_UMCTL2_MP_PCFGWQOS0_0_WQOS_MAP_REGION0_VPW	\
	(1 << DWC_DDR_UMCTL2_MP_PCFGWQOS0_0_WQOS_MAP_REGION0_SHIFT)

/*
 * This bitfield indicates the traffic class of region 1.
 *  Valid values are:
 *  0: NPW, 1: VPW.
 * When VPW support is disabled (UMCTL2_VPW_EN = 0) and traffic class of region 1 is set to 1 (VPW),
 * VPW traffic is aliased to LPW traffic.
 */
#define DWC_DDR_UMCTL2_MP_PCFGWQOS0_0_WQOS_MAP_REGION1_MASK 0x00300000
#define DWC_DDR_UMCTL2_MP_PCFGWQOS0_0_WQOS_MAP_REGION1_SHIFT 20

#define DWC_DDR_UMCTL2_MP_PCFGWQOS0_0_WQOS_MAP_REGION1_NPW	\
	(0 << DWC_DDR_UMCTL2_MP_PCFGWQOS0_0_WQOS_MAP_REGION1_SHIFT)
#define DWC_DDR_UMCTL2_MP_PCFGWQOS0_0_WQOS_MAP_REGION1_VPW	\
	(1 << DWC_DDR_UMCTL2_MP_PCFGWQOS0_0_WQOS_MAP_REGION1_SHIFT)

/**** PCFGWQOS1_0 register ****/
/* Specifies the timeout value for write transactions. */
#define DWC_DDR_UMCTL2_MP_PCFGWQOS1_0_WQOS_MAP_TIMEOUT_MASK 0x000007FF
#define DWC_DDR_UMCTL2_MP_PCFGWQOS1_0_WQOS_MAP_TIMEOUT_SHIFT 0

/**** ADDRMAP0_ALT register ****/
/*
 * Selects the HIF address bit used as rank address bit 0.
 * Valid Range: 0 to 28, and 31
 * Internal Base: 6
 * The selected HIF address bit is determined by adding the internal base to the value of this
 * field.
 * If set to 31, rank address bit 0 is set to 0.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP0_ALT_ADDRMAP_CS_BIT0_ALT_MASK 0x0000001F
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP0_ALT_ADDRMAP_CS_BIT0_ALT_SHIFT 0

/**** ADDRMAP1_ALT register ****/
/*
 * Selects the HIF address bits used as bank address bit 0.
 * Valid Range: 0 to 31
 * Internal Base: 2
 * The selected HIF address bit for each of the bank address bits is determined by adding the
 * internal base to the value of this field.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP1_ALT_ADDRMAP_BANK_B0_ALT_MASK 0x0000001F
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP1_ALT_ADDRMAP_BANK_B0_ALT_SHIFT 0
/*
 * Selects the HIF address bits used as bank address bit 1.
 * Valid Range: 0 to 31
 * Internal Base: 3
 * The selected HIF address bit for each of the bank address bits is determined by adding the
 * internal base to the value of this field.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP1_ALT_ADDRMAP_BANK_B1_ALT_MASK 0x00001F00
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP1_ALT_ADDRMAP_BANK_B1_ALT_SHIFT 8
/*
 * Selects the HIF address bit used as bank address bit 2.
 * Valid Range: 0 to 30 and 31
 * Internal Base: 4
 * The selected HIF address bit is determined by adding the internal base to the value of this
 * field.
 * If set to 31, bank address bit 2 is set to 0.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP1_ALT_ADDRMAP_BANK_B2_ALT_MASK 0x001F0000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP1_ALT_ADDRMAP_BANK_B2_ALT_SHIFT 16

/**** ADDRMAP2_ALT register ****/
/*
 *
 *   - Full bus width mode: Selects the HIF address bit used as column address bit 2.
 *
 *   - Half bus width mode: Selects the HIF address bit used as column address bit 3.
 *
 *   - Quarter bus width mode: Selects the HIF address bit used as column address bit 4.
 * Valid Range: 0 to 7
 * Internal Base: 2
 * The selected HIF address bit is determined by adding the internal base to the value of this
 * field.
 *
 * Note, if UMCTL2_INCL_ARB=1 and MEMC_BURST_LENGTH=8, it is required to program this to 0 unless:
 *   - in Half or Quarter bus width (MSTR.data_bus_width!=00) and
 *   - PCCFG.bl_exp_mode==1 and either
 *   - In DDR4   and ADDRMAP8.addrmap_bg_b0==0 or
 *   - In LPDDR4 and ADDRMAP1.addrmap_bank_b0==0
 *
 *
 * If UMCTL2_INCL_ARB=1 and MEMC_BURST_LENGTH=16, it is required to program this to 0 unless:
 *   - in Half or Quarter bus width (MSTR.data_bus_width!=00) and
 *   - PCCFG.bl_exp_mode==1 and
 *   - In DDR4 and ADDRMAP8.addrmap_bg_b0==0
 *
 * Otherwise, if MEMC_BURST_LENGTH=8 and Full Bus Width (MSTR.data_bus_width==00), it is recommended
 * to program this to 0 so that HIF[2] maps to column address bit 2.
 *
 * If MEMC_BURST_LENGTH=16 and Full Bus Width (MSTR.data_bus_width==00), it is recommended to
 * program this to 0 so that HIF[2] maps to column address bit 2.
 *
 * If MEMC_BURST_LENGTH=16 and Half Bus Width (MSTR.data_bus_width==01), it is recommended to
 * program this to 0 so that HIF[2] maps to column address bit 3.
 *
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP2_ALT_ADDRMAP_COL_B2_ALT_MASK 0x0000000F
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP2_ALT_ADDRMAP_COL_B2_ALT_SHIFT 0
/*
 *
 *   - Full bus width mode: Selects the HIF address bit used as column address bit 3.
 *
 *   - Half bus width mode: Selects the HIF address bit used as column address bit 4.
 *
 *   - Quarter bus width mode: Selects the HIF address bit used as column address bit 5.
 * Valid Range: 0 to 7
 * Internal Base: 3
 * The selected HIF address bit is determined by adding the internal base to the value of this
 * field.
 *
 *
 * Note, if UMCTL2_INCL_ARB=1, MEMC_BURST_LENGTH=16, Full bus width (MSTR.data_bus_width=00) and
 * BL16 (MSTR.burst_rdwr=1000), it is recommended to program this to 0.
 *
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP2_ALT_ADDRMAP_COL_B3_ALT_MASK 0x00000F00
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP2_ALT_ADDRMAP_COL_B3_ALT_SHIFT 8
/*
 *
 *   - Full bus width mode: Selects the HIF address bit used as column address bit 4.
 *
 *   - Half bus width mode: Selects the HIF address bit used as column address bit 5.
 *
 *   - Quarter bus width mode: Selects the HIF address bit used as column address bit 6.
 * Valid Range: 0 to 7, and 15
 * Internal Base: 4
 * The selected HIF address bit is determined by adding the internal base to the value of this
 * field. If set to 15, this column address bit is set to 0.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP2_ALT_ADDRMAP_COL_B4_ALT_MASK 0x000F0000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP2_ALT_ADDRMAP_COL_B4_ALT_SHIFT 16
/*
 *
 *   - Full bus width mode: Selects the HIF address bit used as column address bit 5.
 *
 *   - Half bus width mode: Selects the HIF address bit used as column address bit 6.
 *
 *   - Quarter bus width mode: Selects the HIF address bit used as column address bit 7 .
 * Valid Range: 0 to 7, and 15
 * Internal Base: 5
 * The selected HIF address bit is determined by adding the internal base to the value of this
 * field. If set to 15, this column address bit is set to 0.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP2_ALT_ADDRMAP_COL_B5_ALT_MASK 0x0F000000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP2_ALT_ADDRMAP_COL_B5_ALT_SHIFT 24

/**** ADDRMAP3_ALT register ****/
/*
 *
 *   - Full bus width mode: Selects the HIF address bit used as column address bit 6.
 *
 *   - Half bus width mode: Selects the HIF address bit used as column address bit 7.
 *
 *   - Quarter bus width mode: Selects the HIF address bit used as column address bit 8.
 * Valid Range: 0 to 7, and 15
 * Internal Base: 6
 * The selected HIF address bit is determined by adding the internal base to the value of this
 * field. If set to 15, this column address bit is set to 0.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP3_ALT_ADDRMAP_COL_B6_ALT_MASK 0x0000000F
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP3_ALT_ADDRMAP_COL_B6_ALT_SHIFT 0
/*
 *
 *   - Full bus width mode: Selects the HIF address bit used as column address bit 7.
 *
 *   - Half bus width mode: Selects the HIF address bit used as column address bit 8.
 *
 *   - Quarter bus width mode: Selects the HIF address bit used as column address bit 9.
 * Valid Range: 0 to 7, and 15
 * Internal Base: 7
 * The selected HIF address bit is determined by adding the internal base to the value of this
 * field. If set to 15, this column address bit is set to 0.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP3_ALT_ADDRMAP_COL_B7_ALT_MASK 0x00000F00
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP3_ALT_ADDRMAP_COL_B7_ALT_SHIFT 8
/*
 *
 *   - Full bus width mode: Selects the HIF address bit used as column address bit 8.
 *
 *   - Half bus width mode: Selects the HIF address bit used as column address bit 9.
 *
 * - Quarter bus width mode: Selects the HIF address bit used as column address bit 11 (10 in
 * LPDDR2/LPDDR3 mode).
 * Valid Range: 0 to 7, and 15
 * Internal Base: 8
 * The selected HIF address bit is determined by adding the internal base to the value of this
 * field.
 * If set to 15, this column address bit is set to 0.
 * Note: Per JEDEC DDR2/3/mDDR specification, column address bit 10 is reserved for indicating
 * auto-precharge, and hence no source address bit can be mapped to column address bit 10.
 * In LPDDR2/LPDDR3, there is a dedicated bit for auto-precharge in the CA bus and hence column bit
 * 10 is used.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP3_ALT_ADDRMAP_COL_B8_ALT_MASK 0x000F0000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP3_ALT_ADDRMAP_COL_B8_ALT_SHIFT 16
/*
 *
 *   - Full bus width mode: Selects the HIF address bit used as column address bit 9.
 *
 * - Half bus width mode: Selects the HIF address bit used as column address bit 11 (10 in
 * LPDDR2/LPDDR3 mode).
 *
 * - Quarter bus width mode: Selects the HIF address bit used as column address bit 13 (11 in
 * LPDDR2/LPDDR3 mode).
 * Valid Range: 0 to 7, and 15
 * Internal Base: 9
 * The selected HIF address bit is determined by adding the internal base to the value of this
 * field.
 * If set to 15, this column address bit is set to 0.
 * Note: Per JEDEC DDR2/3/mDDR specification, column address bit 10 is reserved for indicating
 * auto-precharge, and hence no source address bit can be mapped to column address bit 10.
 * In LPDDR2/LPDDR3, there is a dedicated bit for auto-precharge in the CA bus and hence column bit
 * 10 is used.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP3_ALT_ADDRMAP_COL_B9_ALT_MASK 0x0F000000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP3_ALT_ADDRMAP_COL_B9_ALT_SHIFT 24

/**** ADDRMAP4_ALT register ****/
/*
 *
 * - Full bus width mode: Selects the HIF address bit used as column address bit 11 (10 in
 * LPDDR2/LPDDR3 mode).
 *
 * - Half bus width mode: Selects the HIF address bit used as column address bit 13 (11 in
 * LPDDR2/LPDDR3 mode).
 *
 *   - Quarter bus width mode: UNUSED. To make it unused, this must be tied to 4'hF.
 * Valid Range: 0 to 7, and 15
 * Internal Base: 10
 * The selected HIF address bit is determined by adding the internal base to the value of this
 * field.
 * If set to 15, this column address bit is set to 0.
 * Note: Per JEDEC DDR2/3/mDDR specification, column address bit 10 is reserved for indicating
 * auto-precharge, and hence no source address bit can be mapped to column address bit 10.
 * In LPDDR2/LPDDR3, there is a dedicated bit for auto-precharge in the CA bus and hence column bit
 * 10 is used.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP4_ALT_ADDRMAP_COL_B10_ALT_MASK 0x0000000F
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP4_ALT_ADDRMAP_COL_B10_ALT_SHIFT 0
/*
 *
 * - Full bus width mode: Selects the HIF address bit used as column address bit 13 (11 in
 * LPDDR2/LPDDR3 mode).
 *
 *   - Half bus width mode: Unused. To make it unused, this should be tied to 4'hF.
 *
 *   - Quarter bus width mode: Unused. To make it unused, this must be tied to 4'hF.
 * Valid Range: 0 to 7, and 15
 * Internal Base: 11
 * The selected HIF address bit is determined by adding the internal base to the value of this
 * field.
 * If set to 15, this column address bit is set to 0.
 * Note: Per JEDEC DDR2/3/mDDR specification, column address bit 10 is reserved for indicating
 * auto-precharge, and hence no source address bit can be mapped to column address bit 10.
 * In LPDDR2/LPDDR3, there is a dedicated bit for auto-precharge in the CA bus and hence column bit
 * 10 is used.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP4_ALT_ADDRMAP_COL_B11_ALT_MASK 0x00000F00
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP4_ALT_ADDRMAP_COL_B11_ALT_SHIFT 8

/**** ADDRMAP5_ALT register ****/
/*
 * Selects the HIF address bits used as row address bit 0.
 * Valid Range: 0 to 11
 * Internal Base: 6
 * The selected HIF address bit for each of the row address bits is determined by adding the
 * internal base to the value of this field.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP5_ALT_ADDRMAP_ROW_B0_ALT_MASK 0x0000000F
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP5_ALT_ADDRMAP_ROW_B0_ALT_SHIFT 0
/*
 * Selects the HIF address bits used as row address bit 1.
 * Valid Range: 0 to 11
 * Internal Base: 7
 * The selected HIF address bit for each of the row address bits is determined by adding the
 * internal base to the value of this field.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP5_ALT_ADDRMAP_ROW_B1_ALT_MASK 0x00000F00
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP5_ALT_ADDRMAP_ROW_B1_ALT_SHIFT 8
/*
 * Selects the HIF address bits used as row address bits 2 to 10.
 * Valid Range: 0 to 11, and 15
 * Internal Base: 8 (for row address bit 2), 9 (for row address bit 3), 10 (for row address bit 4)
 * etc increasing to 16 (for row address bit 10)
 * The selected HIF address bit for each of the row address bits is determined by adding the
 * internal base to the value of this field. When value 15 is used the values of row address bits 2
 * to 10 are defined by registers ADDRMAP9, ADDRMAP10, ADDRMAP11.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP5_ALT_ADDRMAP_ROW_B2_10_ALT_MASK 0x000F0000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP5_ALT_ADDRMAP_ROW_B2_10_ALT_SHIFT 16
/*
 * Selects the HIF address bit used as row address bit 11.
 * Valid Range: 0 to 11, and 15
 * Internal Base: 17
 * The selected HIF address bit is determined by adding the internal base to the value of this
 * field.
 * If set to 15, row address bit 11 is set to 0.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP5_ALT_ADDRMAP_ROW_B11_ALT_MASK 0x0F000000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP5_ALT_ADDRMAP_ROW_B11_ALT_SHIFT 24

/**** ADDRMAP6_ALT register ****/
/*
 * Selects the HIF address bit used as row address bit 12.
 * Valid Range: 0 to 11, and 15
 * Internal Base: 18
 * The selected HIF address bit is determined by adding the internal base to the value of this
 * field.
 * If set to 15, row address bit 12 is set to 0.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP6_ALT_ADDRMAP_ROW_B12_ALT_MASK 0x0000000F
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP6_ALT_ADDRMAP_ROW_B12_ALT_SHIFT 0
/*
 * Selects the HIF address bit used as row address bit 13.
 * Valid Range: 0 to 11, and 15
 * Internal Base: 19
 * The selected HIF address bit is determined by adding the internal base to the value of this
 * field.
 * If set to 15, row address bit 13 is set to 0.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP6_ALT_ADDRMAP_ROW_B13_ALT_MASK 0x00000F00
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP6_ALT_ADDRMAP_ROW_B13_ALT_SHIFT 8
/*
 * Selects the HIF address bit used as row address bit 14.
 * Valid Range: 0 to 11, and 15
 * Internal Base: 20
 * The selected HIF address bit is determined by adding the internal base to the value of this
 * field.
 * If set to 15, row address bit 14 is set to 0.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP6_ALT_ADDRMAP_ROW_B14_ALT_MASK 0x000F0000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP6_ALT_ADDRMAP_ROW_B14_ALT_SHIFT 16
/*
 * Selects the HIF address bit used as row address bit 15.
 * Valid Range: 0 to 11, and 15
 * Internal Base: 21
 * The selected HIF address bit is determined by adding the internal base to the value of this
 * field.
 * If set to 15, row address bit 15 is set to 0.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP6_ALT_ADDRMAP_ROW_B15_ALT_MASK 0x0F000000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP6_ALT_ADDRMAP_ROW_B15_ALT_SHIFT 24

/**** ADDRMAP7_ALT register ****/
/*
 * Selects the HIF address bit used as row address bit 16.
 * Valid Range: 0 to 11, and 15
 * Internal Base: 22
 * The selected HIF address bit is determined by adding the internal base to the value of this
 * field.
 * If set to 15, row address bit 16 is set to 0.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP7_ALT_ADDRMAP_ROW_B16_ALT_MASK 0x0000000F
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP7_ALT_ADDRMAP_ROW_B16_ALT_SHIFT 0
/*
 * Selects the HIF address bit used as row address bit 17.
 * Valid Range: 0 to 11, and 15
 * Internal Base: 23
 * The selected HIF address bit is determined by adding the internal base to the value of this
 * field.
 * If set to 15, row address bit 17 is set to 0.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP7_ALT_ADDRMAP_ROW_B17_ALT_MASK 0x00000F00
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP7_ALT_ADDRMAP_ROW_B17_ALT_SHIFT 8

/**** ADDRMAP8_ALT register ****/
/*
 * Selects the HIF address bits used as bank group address bit 0.
 * Valid Range: 0 to 31
 * Internal Base: 2
 * The selected HIF address bit for each of the bank group address bits is determined by adding the
 * internal base to the value of this field.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP8_ALT_ADDRMAP_BG_B0_ALT_MASK 0x0000001F
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP8_ALT_ADDRMAP_BG_B0_ALT_SHIFT 0
/*
 * Selects the HIF address bits used as bank group address bit 1.
 * Valid Range: 0 to 31, and 63
 * Internal Base: 3
 * The selected HIF address bit for each of the bank group address bits is determined by adding the
 * internal base to the value of this field.
 * If set to 63, bank group address bit 1 is set to 0.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP8_ALT_ADDRMAP_BG_B1_ALT_MASK 0x00003F00
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP8_ALT_ADDRMAP_BG_B1_ALT_SHIFT 8

/**** ADDRMAP9_ALT register ****/
/*
 * Selects the HIF address bits used as row address bit 2.
 * Valid Range: 0 to 11
 * Internal Base: 8
 * The selected HIF address bit for each of the row address bits is determined by adding the
 * internal base to the value of this field. This register field is used only when
 * ADDRMAP5.addrmap_row_b2_10 is set to value 15.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP9_ALT_ADDRMAP_ROW_B2_ALT_MASK 0x0000000F
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP9_ALT_ADDRMAP_ROW_B2_ALT_SHIFT 0
/*
 * Selects the HIF address bits used as row address bit 3.
 * Valid Range: 0 to 11
 * Internal Base: 9
 * The selected HIF address bit for each of the row address bits is determined by adding the
 * internal base to the value of this field. This register field is used only when
 * ADDRMAP5.addrmap_row_b2_10 is set to value 15.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP9_ALT_ADDRMAP_ROW_B3_ALT_MASK 0x00000F00
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP9_ALT_ADDRMAP_ROW_B3_ALT_SHIFT 8
/*
 * Selects the HIF address bits used as row address bit 4.
 * Valid Range: 0 to 11
 * Internal Base: 10
 * The selected HIF address bit for each of the row address bits is determined by adding the
 * internal base to the value of this field. This register field is used only when
 * ADDRMAP5.addrmap_row_b2_10 is set to value 15.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP9_ALT_ADDRMAP_ROW_B4_ALT_MASK 0x000F0000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP9_ALT_ADDRMAP_ROW_B4_ALT_SHIFT 16
/*
 * Selects the HIF address bits used as row address bit 5.
 * Valid Range: 0 to 11
 * Internal Base: 11
 * The selected HIF address bit for each of the row address bits is determined by adding the
 * internal base to the value of this field. This register field is used only when
 * ADDRMAP5.addrmap_row_b2_10 is set to value 15.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP9_ALT_ADDRMAP_ROW_B5_ALT_MASK 0x0F000000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP9_ALT_ADDRMAP_ROW_B5_ALT_SHIFT 24

/**** ADDRMAP10_ALT register ****/
/*
 * Selects the HIF address bits used as row address bit 6.
 * Valid Range: 0 to 11
 * Internal Base: 12
 * The selected HIF address bit for each of the row address bits is determined by adding the
 * internal base to the value of this field. This register field is used only when
 * ADDRMAP5.addrmap_row_b2_10 is set to value 15.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP10_ALT_ADDRMAP_ROW_B6_ALT_MASK 0x0000000F
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP10_ALT_ADDRMAP_ROW_B6_ALT_SHIFT 0
/*
 * Selects the HIF address bits used as row address bit 7.
 * Valid Range: 0 to 11
 * Internal Base: 13
 * The selected HIF address bit for each of the row address bits is determined by adding the
 * internal base to the value of this field. This register field is used only when
 * ADDRMAP5.addrmap_row_b2_10 is set to value 15.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP10_ALT_ADDRMAP_ROW_B7_ALT_MASK 0x00000F00
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP10_ALT_ADDRMAP_ROW_B7_ALT_SHIFT 8
/*
 * Selects the HIF address bits used as row address bit 8.
 * Valid Range: 0 to 11
 * Internal Base: 14
 * The selected HIF address bit for each of the row address bits is determined by adding the
 * internal base to the value of this field. This register field is used only when
 * ADDRMAP5.addrmap_row_b2_10 is set to value 15.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP10_ALT_ADDRMAP_ROW_B8_ALT_MASK 0x000F0000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP10_ALT_ADDRMAP_ROW_B8_ALT_SHIFT 16
/*
 * Selects the HIF address bits used as row address bit 9.
 * Valid Range: 0 to 11
 * Internal Base: 15
 * The selected HIF address bit for each of the row address bits is determined by adding the
 * internal base to the value of this field. This register field is used only when
 * ADDRMAP5.addrmap_row_b2_10 is set to value 15.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP10_ALT_ADDRMAP_ROW_B9_ALT_MASK 0x0F000000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP10_ALT_ADDRMAP_ROW_B9_ALT_SHIFT 24

/**** ADDRMAP11_ALT register ****/
/*
 * Selects the HIF address bits used as row address bit 10.
 * Valid Range: 0 to 11
 * Internal Base: 16
 * The selected HIF address bit for each of the row address bits is determined by adding the
 * internal base to the value of this field. This register field is used only when
 * ADDRMAP5.addrmap_row_b2_10 is set to value 15.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP11_ALT_ADDRMAP_ROW_B10_ALT_MASK 0x0000000F
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP11_ALT_ADDRMAP_ROW_B10_ALT_SHIFT 0
/*
 * Selects the HIF address bit used as chip id bit 0.
 * Valid Range: 0 to 29
 * Internal Base: 4
 * The selected HIF address bit is determined by adding the internal base to the value of this
 * field. If set to 31, chip id bit 0 is set to 0.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP11_ALT_ADDRMAP_CID_B0_ALT_MASK 0x00001F00
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP11_ALT_ADDRMAP_CID_B0_ALT_SHIFT 8
/*
 * Selects the HIF address bit used as chip id bit 1.
 * Valid Range: 0 to 29
 * Internal Base: 5
 * The selected HIF address bit is determined by adding the internal base to the value of this
 * field. If set to 31, chip id bit 1 is set to 0.
 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP11_ALT_ADDRMAP_CID_B1_ALT_MASK 0x001F0000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP_ALT_ADDRMAP11_ALT_ADDRMAP_CID_B1_ALT_SHIFT 16

#ifdef __cplusplus
}
#endif

#endif /* __AL_HAL_DDR_CTRL_REGS_DEFS_H__ */

/** @} end of DDR group */


