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
#if (AL_DEV_ID == AL_DEV_ID_ALPINE_V2)

#include "al_hal_ddr_init.h"
#include "al_hal_ddr_ctrl_regs_common.h"
#include "al_hal_ddr_phy_regs_alpine_v2.h"
#include "al_hal_nb_regs_v1_v2.h"
#include "al_hal_ddrc_regs.h"
#include "al_hal_ddr_jedec_ddr3.h"
#include "al_hal_ddr_jedec_ddr4.h"
#include "al_hal_ddr_utils.h"

#define STRUCT_CTRL_REGS struct al_dwc_ddr_umctl2_regs_alpine_v2
#define STRUCT_CTRL_MP_REGS struct al_dwc_ddr_umctl2_mp_alpine_v2
#define CTRL_REGS(ddr_ctrl_regs_base) &((union al_ddr_ctrl_regs __iomem *)ddr_ctrl_regs_base)->alpine_v2.umctl2_regs;
#define CTRL_MP_REGS(ddr_ctrl_regs_base) &((union al_ddr_ctrl_regs __iomem *)ddr_ctrl_regs_base)->alpine_v2.umctl2_mp;
#define STRUCT_PHY_REGS struct al_dwc_ddrphy_top_dwc_ddrphy_pub_alpine_v2
#define STRUCT_PHY_DATX8_REGS struct al_ddr_phy_datx8_regs_alpine_v2
#define PHY_REGS(ddr_phy_regs_base)	\
	(&((struct al_dwc_ddrphy_top_regs_alpine_v2 __iomem *)ddr_phy_regs_base)->dwc_ddrphy_pub)

/**** Reorder_ID_Mask register ****/
/* DDR Read Reorder buffer ID mask.
If incoming read transaction ID ANDed with mask is equal Reorder_ID_Value, then the transaction is
mapped to the DDR controller bypass channel.
Setting this register to 0 will disable the check */
#define NB_DRAM_CHANNELS_REORDER_ID_MASK_MASK_MASK 0x003FFFFF
#define NB_DRAM_CHANNELS_REORDER_ID_MASK_MASK_SHIFT 0

/**** Reorder_ID_Value register ****/
/* DDR Read Reorder buffer ID value
If incoming read transaction ID ANDed with Reorder_ID_Mask is equal to this register, then the
transaction is mapped to the DDR controller bypass channel */
#define NB_DRAM_CHANNELS_REORDER_ID_VALUE_VALUE_MASK 0x003FFFFF
#define NB_DRAM_CHANNELS_REORDER_ID_VALUE_VALUE_SHIFT 0

/* Definitions for making the code more compact */
#define REF_CLK_FREQ_MHZ	(cfg->tmg.ref_clk_freq_mhz)
#define AL			(cfg->tmg.al)
#define CL			(cfg->tmg.cl)
#define CWL			(cfg->tmg.cwl)
#define PL			(cfg->calc.t_pl)

/* Refresh interval [ns] */
#define TREFI_NS		7800

/* Burst Length */
#define BL			8

/* The size of the request reordering queue */
#define REORDER_QUEUE_SIZE	63

/* The size of the crc/parity retry queue */
#define RETRY_CMD_FIFO_DEPTH	24

/**
 * Fetch the maximum out of nCK and a number of picoseconds.
 * The (nCK, picoseconds) pair is obtained from a table according to the
 * DDR frequency.
 */
#define AL_DDR_TMG_TBL_MAX_CK_PS_GET_VAL(table)	\
	al_ddr_max_ck_ps(\
		REF_CLK_FREQ_MHZ,\
		(table)[cfg->tmg.ddr_freq].nck,\
		(table)[cfg->tmg.ddr_freq].ps)

/* Default Rank Read DQS Gating Delay - Used when training is disabled */
#define AL_DDR_RNK_RD_DQS_GATING_DLY	0x6e
#define AL_DDR_RNK_RD_DQS_GATING_DLY_ALPINE_V2	0x136
#define AL_DDR_RNK_RD_DQS_GATING_DLY_ALPINE_V3	0x0FA

/* Default Rank Write Leveling Delay - Used when training is disabled */
#define AL_DDR_RNK_WR_LVL_DLY		0x2C

/* Max DDR controller read ODT delay */
#define AL_DDR_CTRL_MAX_READ_ODT_DELAY 15

/* Max DDR PHY read ODT delay */
#define AL_DDR_PHY_MAX_READ_ODT_DELAY 32

/* (nCK, picoseconds) pair */
struct al_ddr_max_ck_ps_ent {
	unsigned short nck;
	unsigned short ps;	/* [pico seconds] */
};

/* Single data lane manual configuration - used when training is disabled */
static inline void al_ddr_phy_data_lane_cfg(
	struct al_ddr_init_cfg	*cfg,
	int			byte);

/* ddr initialization - print initialization parameters */
static inline void al_ddr_init_print_params(
	struct al_ddr_init_cfg	*cfg);

/* DDR initialization - Mark active byte lanes based on ECC and bus width */
static inline int al_ddr_active_byte_lanes_mark(
	struct al_ddr_init_cfg	*cfg);

/* DDR initialization - Configure the DDR controller */
static inline int al_ddr_ctrl_cfg(
	struct al_ddr_init_cfg	*cfg);

/* DDR initialization - Configure the DDR controller address mapping */
static inline int al_ddr_ctrl_cfg_addr_map(
	struct al_ddr_init_cfg	*cfg);

/* DDR initialization - PHY initialization */
static inline int al_ddr_phy_init(
	struct al_ddr_init_cfg	*cfg);

/* DDR initialization - PHY unused DQs power down */
static inline int al_ddr_phy_unused_dq_pd(
	struct al_ddr_init_cfg	*cfg);

/* DDR initialization - DDR controller and PHY reset */
static inline void al_ddr_reset(
	struct al_ddr_init_cfg	*cfg);

/* DDR initialization - DDR controller and PHY reset releasing */
static inline int al_ddr_unit_reset_release(
	struct al_ddr_init_cfg	*cfg);

/* DDR initialization - DDR controller-phy interface initialization */
static inline int al_ddr_ctrl_dfi_init(
	struct al_ddr_init_cfg	*cfg);

/* DDR initialization - DDR PHY training */
static inline int al_ddr_phy_training(
	struct al_ddr_init_cfg	*cfg);

/* DDR initialization - DDR PHY switch to normal operating mode */
static inline int al_ddr_phy_run(
	struct al_ddr_init_cfg	*cfg);

/* DDR initialization - DDR controller switch to normal operating mode */
static inline int al_ddr_ctrl_run(
	struct al_ddr_init_cfg	*cfg);

/* DDR initialization - print training failure */
static void al_ddr_phy_training_failure_print(
	struct al_ddr_init_cfg	*cfg);

/* Divide by 2 (round up) */
static unsigned int al_ddr_div2_rnd_up(
	unsigned int value);

static unsigned int al_rd_dbi_extra_cas_lat_get(
	unsigned int clk_freq_mhz);

/* Convert nanoseconds to reference clock cycles (round up) */
static unsigned int al_ddr_ns_to_ck_rnd_up(
	unsigned int clk_freq_mhz,
	unsigned int ns);

/* Convert nanoseconds to reference clock cycles (round down) */
static unsigned int al_ddr_ns_to_ck_rnd_dn(
	unsigned int clk_freq_mhz,
	unsigned int ns);

/**
 * Return the maximum out of:
 * - A given number of reference clock cycles
 * - A given number of nanoseconds converted to reference clock cycles
 */
static unsigned int al_ddr_max_ck_ns(
	unsigned int clk_freq_mhz,
	unsigned int ck,
	unsigned int ns);

/* Convert nanoseconds to 1024 reference clock cycles (round up) */
static unsigned int al_ddr_ns_to_ck_x1024_rnd_up(
	unsigned int clk_freq_mhz,
	unsigned int ns);

/* Convert nanoseconds to 1024 reference clock cycles (round down) */
static unsigned int al_ddr_ns_to_ck_x1024_rnd_dn(
	unsigned int clk_freq_mhz,
	unsigned int ns);

/* Convert nanoseconds to reference clock cycles (round up) */
static unsigned int al_ddr_ps_to_ck_rnd_up(
	unsigned int clk_freq_mhz,
	unsigned int ps);

/**
 * Return the maximum out of:
 * - A given number of reference clock cycles
 * - A given number of nanoseconds converted to reference clock cycles
 */
static unsigned int al_ddr_max_ck_ps(
	unsigned int clk_freq_mhz,
	unsigned int ck,
	unsigned int ps);

/* Decrease 'dec_odd' if 'val' is odd, decrease 'dec_even' if 'val' is even */

/**
 * Return the maximum out of:
 * - A given number of reference clock cycles
 * - A given number of nanoseconds converted to reference clock cycles
 *
 * The result is converted to units of 32 reference clock cycles and rounded up.
 */
static inline unsigned int al_ddr_max_ck_ns_x32_rnd_up(
	unsigned int clk_freq_mhz,
	unsigned int ck,
	unsigned int ns);

/* Convert nanoseconds to 32 reference clock cycles (round up) */
static inline unsigned int al_ddr_ns_to_ck_x32_rnd_up(
	unsigned int clk_freq_mhz,
	unsigned int ns);

/* Convert picoseconds to 32 reference clock cycles (round up) */
static inline unsigned int al_ddr_ps_to_ck_x32_rnd_up(
	unsigned int clk_freq_mhz,
	unsigned int ps);

/* Set each bit to 1 if rank is active*/
static inline unsigned int active_ranks_calc(
	unsigned int dimms,
	unsigned int ranks);

/* Set CID offset if rank is active*/
static inline unsigned int active_ranks_calc_phy(
	unsigned int active_ranks_no_cid);

/* Wait for PHY stage initialization to be done with check status option*/
static int al_ddr_phy_wait_for_init_done_ext(
	STRUCT_PHY_REGS __iomem *phy_regs,
	al_bool check_status);

/* Wait for PHY stage initialization to be done */
static int al_ddr_phy_wait_for_init_done(
	STRUCT_PHY_REGS __iomem *phy_regs);

static const al_bool ddr_ctrl_sdr_mode = 0;

/**
 * Rise-to-Rise Mode: When asserted, the PHY aligns the rising edge of
 * its clocks relative to the rising edge of the controller clock,
 * resulting in the PHY sampling and sourcing signals from and to the
 * controller relative to the rising edge of ctl_clk. Otherwise
 * rise-to-fall clocking scheme is used in which the PHY samples and
 * sources signals from and to the controller relative to the falling
 * edge of ctl_clk.
 *
 * Required value: 1
 */
static const unsigned int rr_mode = 1;

/**
 * PHY RX FIFO Read Mode
 *
 * Required value: DWC_DDR_PHY_REGS_PGCR3_RDMODE_STATIC_RD_RSP_OFF
 */
static const uint32_t phy_rdmode =
	ALPINE_V2_DWC_DDR_PHY_REGS_PGCR3_RDMODE_STATIC_RD_RSP_OFF;

/**
 * If 1, then controller uses 2T timing. Otherwise, uses 1T timing. In
 * 2T timing, all command signals (except chip select) are held for 2
 * clocks on the SDRAM bus. Chip select is asserted on the second cycle
 * of the command
 *
 * Required value: 1
 */
static const unsigned int en_2t_timing_mode = 1;

/**
 * Indicates burst mode.
 *
 * 1 - Sequential burst mode
 * 0 - Interleaved burst mode
 *
 * Related to MR0
 *
 * Suggested value: 1
 */
static const al_bool burst_mode_seq = 1;

/**
 * Specifies the maximum number of DFI clock cycles that the
 * phy_dfi_phyupd_req signal may remain asserted after the assertion of
 * the ddrc_dfi_phyupd_ack signal for phy_dfi_phyupd_type = 2'b00. The
 * phy_dfi_phyupd_req signal may de-assert at any cycle after the
 * assertion of the ddrc_dfi_phyupd_ack signal.
 *
 * Required value: 0x10
 */
static const unsigned int dfi_phyupd_type0 = 0x10;

/**
 * Specifies the maximum number of DFI clock cycles that the
 * phy_dfi_phyupd_req signal may remain asserted after the assertion of
 * the ddrc_dfi_phyupd_ack signal for phy_dfi_phyupd_type = 2'b01. The
 * phy_dfi_phyupd_req signal may de-assert at any cycle after the
 * assertion of the ddrc_dfi_phyupd_ack signal.
 *
 * Required value: 0x640
 */
static const unsigned int dfi_phyupd_type1 = 0x640;

/**
 * Cycles to hold ODT for a read command. When 0x0, ODT signal is ON
 * for 1 cycle. When 0x1, it is ON for 2 cycles, and so on.
 *
 * Required value: 5 (assuming mstr.burst_rdwr = 4)
 */

/**
 * Cycles to hold ODT for a write command. When 0x0, ODT signal is ON
 * for 1 cycle. When 0x1, it is ON for 2 cycles, and so on.
 *
 * Required value: 5 (assuming mstr.burst_rdwr = 4)
 */

/**
 * Extra Cycles to hold ODT for a PHY command. When 0x0, ODT signal is ON
 * for 4 cycles. When 0x1, it is ON for 6 cycles, and so on.
 *
 * Suggested value: 1
 */
static const unsigned int phy_odt_hold = 1;

/**
 * If set to 1, read transactions with ID not covered by any of the
 * virtual channel ID mapping registers are not reordered.
 *
 * Required value: 1
 */

/**
 * Determines the mask used in the ID mapping function
 *
 * Required value: 0x18
 */
static const unsigned int id_mask = 0x18;
static const unsigned int id_mask_0 = 0x61e;
static const unsigned int id_mask_1 = 0x79e;

/**
 * Determines the value used in the ID mapping function
 *
 * Required value: 0x08
 */
static const unsigned int id_value = 0x08;
static const unsigned int id_value_0 = 0xc;
static const unsigned int id_value_1 = 0x70c;


/**
 * Specifies the number of DFI clock cycles after an assertion or
 * de-assertion of the DFI control signals that the control signals at
 * the PHY-DRAM interface reflect the assertion or de-assertion.
 *
 * Required value: 7
 */

/**
 * Specifies the number of clock cycles between when dfi_wrdata_en is
 * asserted to when the associated write data is driven on the
 * dfi_wrdata signal. This corresponds to the DFI timing parameter
 * tphy_wrdata.
 *
 * Required value: 0
 */

/**
 * Specifies the number of DFI clock cycles from the assertion of the
 * ddrc_dfi_dram_clk_disable signal on the DFI until the clock to the
 * DRAM memory devices, at the PHY-DRAM boundary, maintains a low value.
 *
 * Required value: 4
 */

/**
 * Specifies the number of DFI clock cycles from the de-assertion of the
 * ddrc_dfi_dram_clk_disable signal on the DFI until the first valid
 * rising edge of the clock to the DRAM memory devices, at the PHY-DRAM
 * boundary.
 *
 * Required value: 4
 */

/**
 * Disable controller initiated DLL calibrations.
 *
 * Suggested value: 1
 */
static const unsigned int dis_auto_ctrlupd = 1;

/**
 * Burst chop is supported only when using full bus-width mode. Only
 * reads make use of burst chop. Writes still complete a full write
 * burst every time because of write combine support. When burst chop is
 * in use, short reads make use of burst chop. Note that per DDR3
 * specifications, a rank is unavailable for two cycles following a
 * burst chop, so the implementation of burst chop provides an advantage
 * only when switching between ranks.
 *
 * Suggested value: 0
 */

/*
 * Addressing RMN: 11710
 *
 * RMN description:
 * Max Rank Read Feature does not Provide an Opportunity to Reschedule all Rank with BC4
 * Software flow:
 * Set burstchop = 0
 */
static const al_bool burstchop;

/**
 * Exit Power Down with DLL on to any valid command; Exit Precharge
 * Power Down with DLL frozen to commands not requiring a locked DLL
 *
 * Required value: According to the following JEDEC table:
 *                    DDR freq | t_xp
 *                 ------------------------------
 *                      800MHz | max(3nCK, 7.5ns)
 *                     1066Mhz | max(3nCK, 7.5ns)
 *                     1333MHz | max(3nCK, 6ns)
 *                     1600MHz | max(3nCK, 6ns)
 *                     1866MHz | max(3nCK, 6ns)
 *                     2133MHz | max(3nCK, 6ns)
 *                     2400MHz | max(3nCK, 6ns)
 *                     2666MHz | max(3nCK, 6ns)
 */
static const struct al_ddr_max_ck_ps_ent t_xp_table[AL_DDR_FREQ_ENUM_SIZE] = {
	{ 3, 7500 },	/* AL_DDR_FREQ_800 */
	{ 3, 7500 },	/* AL_DDR_FREQ_1066 */
	{ 3, 6000 },	/* AL_DDR_FREQ_1333 */
	{ 3, 6000 },	/* AL_DDR_FREQ_1600 */
	{ 3, 6000 },	/* AL_DDR_FREQ_1866 */
	{ 3, 6000 },	/* AL_DDR_FREQ_2133 */
	{ 3, 6000 },	/* AL_DDR_FREQ_2400 */
	{ 3, 6000 },	/* AL_DDR_FREQ_2666 */
};

static const struct al_ddr_max_ck_ps_ent t_xp_table_ddr4[AL_DDR_FREQ_ENUM_SIZE] = {
	{ 4, 6000 },	/* AL_DDR_FREQ_800 */
	{ 4, 6000 },	/* AL_DDR_FREQ_1066 */
	{ 4, 6000 },	/* AL_DDR_FREQ_1333 */
	{ 4, 6000 },	/* AL_DDR_FREQ_1600 */
	{ 4, 6000 },	/* AL_DDR_FREQ_1866 */
	{ 4, 6000 },	/* AL_DDR_FREQ_2133 */
	{ 4, 6000 },	/* AL_DDR_FREQ_2400 */
	{ 4, 6000 },	/* AL_DDR_FREQ_2666 */

};

/**
 * Minimum number of cycles of CKE HIGH/LOW during power-down and self
 * refresh
 *
 * Required value: According to the following JEDEC table:
 *                    DDR freq | t_cke
 *                 ---------------------------------------------
 *                      800MHz | MAX(3nCK, 7.5ns)
 *                     1066Mhz | MAX(3nCK, 5.625ns)
 *                     1333MHz | MAX(3nCK, 5.625ns)
 *                     1600MHz | MAX(3nCK, 5ns)
 *                     1866MHz | MAX(3nCK, 5ns)
 *                     2133MHz | MAX(3nCK, 5ns)
 */
static const struct al_ddr_max_ck_ps_ent t_cke_table[
	AL_DDR_FREQ_ENUM_SIZE] = {
	{ 3,	7500 },	/* AL_DDR_FREQ_800 */
	{ 3,	5625 },	/* AL_DDR_FREQ_1066 */
	{ 3,	5625 },	/* AL_DDR_FREQ_1333 */
	{ 3,	5000 },	/* AL_DDR_FREQ_1600 */
	{ 3,	5000 },	/* AL_DDR_FREQ_1866 */
	{ 3,	5000 },	/* AL_DDR_FREQ_2133 */
};

/**
 * DLL locking time
 *
 * Required value: According to the following JEDEC table:
 *                    DDR freq | t_dllk
 *                 ------------------------------
 *                     1600MHz | 597
 *                     1866MHz | 597
 *                     2133MHz | 768
 *                     2400MHz | 768
 *                     2666MHz | 854
 */
static const unsigned short t_dllk_table[AL_DDR_FREQ_ENUM_SIZE] = {
	597,	/* AL_DDR_FREQ_800 */
	597,	/* AL_DDR_FREQ_1066 */
	597,	/* AL_DDR_FREQ_1333 */
	597,	/* AL_DDR_FREQ_1600 */
	597,	/* AL_DDR_FREQ_1866 */
	768,	/* AL_DDR_FREQ_2133 */
	768,	/* AL_DDR_FREQ_2400 */
	854,	/* AL_DDR_FREQ_2666 */

};

/**
 * Parity Latency : added parity latency when C/A parity is enabled
 *
 * Required value: According to the following JEDEC table:
 *                    DDR freq | t_pl
 *                 ------------------------------
 *                     1600MHz | 4
 *                     1866MHz | 4
 *                     2133MHz | 4
 *                     2400MHz | 5
 *                     2666MHz | 6 - (from Soma model)
 */
static const unsigned short t_pl_table[AL_DDR_FREQ_ENUM_SIZE] = {
	4,	/* AL_DDR_FREQ_800 */
	4,	/* AL_DDR_FREQ_1066 */
	4,	/* AL_DDR_FREQ_1333 */
	4,	/* AL_DDR_FREQ_1600 */
	4,	/* AL_DDR_FREQ_1866 */
	4,	/* AL_DDR_FREQ_2133 */
	5,	/* AL_DDR_FREQ_2400 */
	6,	/* AL_DDR_FREQ_2666 */

};

/**
 * Write CMD Latency when CRC and DM are enabled
 *
 * Required value: According to the following JEDEC table:
 *                    DDR freq | t_crc_dm_wcl
 *                 ------------------------------
 *                     1600MHz | 4
 *                     1866MHz | 5
 *                     2133MHz | 5
 *                     2400MHz | 5
 *                     2666MHz | 6
 */
static const unsigned short t_crc_dm_wcl_table[AL_DDR_FREQ_ENUM_SIZE] = {
	4,	/* AL_DDR_FREQ_800 */
	4,	/* AL_DDR_FREQ_1066 */
	4,	/* AL_DDR_FREQ_1333 */
	4,	/* AL_DDR_FREQ_1600 */
	5,	/* AL_DDR_FREQ_1866 */
	5,	/* AL_DDR_FREQ_2133 */
	5,	/* AL_DDR_FREQ_2400 */
	6,	/* AL_DDR_FREQ_2666 */
};

/**
 * Pulse width of ALERT_n signal when asserted
 *
 * Required value: According to the following JEDEC table:
 *                    DDR freq | t_par_alert_pw_max
 *                 ------------------------------
 *                     1600MHz | 96
 *                     1866MHz | 112
 *                     2133MHz | 128
 *                     2400MHz | 144
 *                     2666MHz | 160 - (from Soma model)
 */
static const unsigned short t_par_alert_pw_max_table[AL_DDR_FREQ_ENUM_SIZE] = {
	96,		/* AL_DDR_FREQ_800  */
	96,		/* AL_DDR_FREQ_1066 */
	96,		/* AL_DDR_FREQ_1333 */
	96,		/* AL_DDR_FREQ_1600 */
	112,	/* AL_DDR_FREQ_1866 */
	128,	/* AL_DDR_FREQ_2133 */
	144,	/* AL_DDR_FREQ_2400 */
	160,	/* AL_DDR_FREQ_2666 */

};

/**
 * tMRS- Mode Register Set command cycle time
 *
 * Required value: According to the following JEDEC table:
 *                    DDR freq | tmrd
 *                 ------------------------------
 *                     1600MHz | 8
 *                     1866MHz | 8
 *                     2133MHz | 8
 *                     2400MHz | 8
 *                     2666MHz | 9
 */
static const unsigned short t_mrd_table[AL_DDR_FREQ_ENUM_SIZE] = {
	8,	/* AL_DDR_FREQ_800 */
	8,	/* AL_DDR_FREQ_1066 */
	8,	/* AL_DDR_FREQ_1333 */
	8,	/* AL_DDR_FREQ_1600 */
	8,	/* AL_DDR_FREQ_1866 */
	8,	/* AL_DDR_FREQ_2133 */
	8,	/* AL_DDR_FREQ_2400 */
	9,	/* AL_DDR_FREQ_2666 */
};

/**
 * RDIMM control word operating speed
 *
 * Required value: According to the following JEDEC table:
 *                    DDR freq | tmrd
 *                 ------------------------------
 *                     1600MHz | 0
 *                     1866MHz | 1
 *                     2133MHz | 2
 *                     2400MHz | 3
 *                     2666MHz | 4
 */

/**
 * If 1, enables DDR4 C/A parity and CRC retry
 *
 * Suggested value: 1
 */
static const unsigned int crc_parity_retry_enable = 1;

/**
 * Selects arqos value to seperate between low/high priority reads
 * inside DDR controller
 *
 * Required value: User defined
 */
static const unsigned int rqos_map_level1 = 0x7;

/**
 * Read BDL's pre seed value for Alpine V2 RMN 6951 SW WA
 *
 * Suggested value: 0x20
 */
static const unsigned int pre_seed = 0x20;

/*******************************************************************************
 ******************************************************************************/
static inline void al_ddr_phy_data_lane_cfg(
	struct al_ddr_init_cfg	*cfg,
	int			byte)
{
	STRUCT_PHY_REGS __iomem *phy_regs = PHY_REGS(cfg->ddr_cfg.ddr_phy_regs_base);
	STRUCT_PHY_DATX8_REGS __iomem *datx8_regs = &phy_regs->datx8[byte];
	unsigned int i;
	register unsigned int ranks = cfg->org.ranks;
	unsigned int rd_dqs_gating_dly;

	rd_dqs_gating_dly = AL_DDR_RNK_RD_DQS_GATING_DLY_ALPINE_V2;

	for (i = 0; i < ranks; i++) {
		/* Select rank */
		al_reg_write32(
			&phy_regs->rankidr,
			(i <<
			ALPINE_V2_DWC_DDR_PHY_REGS_RANKIDR_RANKWID_SHIFT) |
			(i <<
			ALPINE_V2_DWC_DDR_PHY_REGS_RANKIDR_RANKRID_SHIFT));

		/* set Read DQS gating */
		al_reg_write32(
			&datx8_regs->lcdlr[2],
			(rd_dqs_gating_dly <<
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNLCDLR2_DQSGD_SHIFT) |
			(rd_dqs_gating_dly <<
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNLCDLR2_X4DQSGD_SHIFT));
		/* set Write Leveling */
		al_reg_write32(
			&datx8_regs->lcdlr[0],
			(AL_DDR_RNK_WR_LVL_DLY <<
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNLCDLR0_WLD_SHIFT) |
			(AL_DDR_RNK_WR_LVL_DLY <<
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNLCDLR0_X4WLD_SHIFT));
	}

}


/*******************************************************************************
 ******************************************************************************/
static inline void al_ddr_init_print_params(
	struct al_ddr_init_cfg	*cfg)
{
	int i;

	al_dbg("-----------------------------------------------------------\n");
	al_dbg("DDR configuration\n");
	al_dbg("-----------------------------------------------------------\n");
	al_dbg("Organization\n");
	al_dbg("\tData Width: %u bits\n",
		(cfg->org.data_width == AL_DDR_DATA_WIDTH_16_BITS) ? 16 :
		(cfg->org.data_width == AL_DDR_DATA_WIDTH_32_BITS) ? 32 :
		(cfg->org.data_width == AL_DDR_DATA_WIDTH_64_BITS) ? 64 :
		0);
	al_dbg("\tNum ranks: %u\n", cfg->org.ranks);
	al_dbg("\tNum DIMMs: %u\n", cfg->org.dimms);
	al_dbg("\trdimm = %s\n", cfg->org.rdimm ? "yes" : "no");
	al_dbg("\tudimm_addr_mirroring = %s\n", cfg->org.udimm_addr_mirroring ? "yes" : "no");
	al_dbg("\tecc_is_supported = %s\n", cfg->org.ecc_is_supported ? "yes" : "no");
	al_dbg("\tddr_type = ddr%u\n",
		(cfg->org.ddr_type == AL_DDR_TYPE_DDR3) ? 3 :
		(cfg->org.ddr_type == AL_DDR_TYPE_DDR4) ? 4 :
		0);
	al_dbg("\tddr_device = x%u\n",
		(cfg->org.ddr_device == AL_DDR_DEVICE_X4) ? 4 :
		(cfg->org.ddr_device == AL_DDR_DEVICE_X8) ? 8 :
		(cfg->org.ddr_device == AL_DDR_DEVICE_X16) ? 16 :
		0);
	al_dbg("\tdq_rank_swap_enabled = %s\n",
		cfg->org.dq_rank_swap_enabled ? "yes" : "no");
	al_dbg("\tdq_nibble_map = 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x "
		"0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",
		cfg->org.dq_nibble_map[0],
		cfg->org.dq_nibble_map[1],
		cfg->org.dq_nibble_map[2],
		cfg->org.dq_nibble_map[3],
		cfg->org.dq_nibble_map[4],
		cfg->org.dq_nibble_map[5],
		cfg->org.dq_nibble_map[6],
		cfg->org.dq_nibble_map[7],
		cfg->org.dq_nibble_map[8],
		cfg->org.dq_nibble_map[9],
		cfg->org.dq_nibble_map[10],
		cfg->org.dq_nibble_map[11],
		cfg->org.dq_nibble_map[12],
		cfg->org.dq_nibble_map[13],
		cfg->org.dq_nibble_map[14],
		cfg->org.dq_nibble_map[15]);
	al_dbg("\tcb_nibble_map = 0x%02x 0x%02x\n",
		cfg->org.cb_nibble_map[0],
		cfg->org.cb_nibble_map[1]);

	al_dbg("Address mapping\n");
	al_dbg("\tcol[3..9, 11..13] = %u %u %u %u %u %u %u %u %u %u\n",
		cfg->addrmap.col_b3_9_b11_13[0],
		cfg->addrmap.col_b3_9_b11_13[1],
		cfg->addrmap.col_b3_9_b11_13[2],
		cfg->addrmap.col_b3_9_b11_13[3],
		cfg->addrmap.col_b3_9_b11_13[4],
		cfg->addrmap.col_b3_9_b11_13[5],
		cfg->addrmap.col_b3_9_b11_13[6],
		cfg->addrmap.col_b3_9_b11_13[7],
		cfg->addrmap.col_b3_9_b11_13[8],
		cfg->addrmap.col_b3_9_b11_13[9]);
	al_dbg("\tbank[0..2] = %u %u %u\n",
		cfg->addrmap.bank_b0_2[0],
		cfg->addrmap.bank_b0_2[1],
		cfg->addrmap.bank_b0_2[2]);
	al_dbg("\tbg[0..1] = %u %u\n",
		cfg->addrmap.bg_b0_1[0],
		cfg->addrmap.bg_b0_1[1]);
	al_dbg("\trow[0..17] = %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u\n",
		cfg->addrmap.row_b0_17[0],
		cfg->addrmap.row_b0_17[1],
		cfg->addrmap.row_b0_17[2],
		cfg->addrmap.row_b0_17[3],
		cfg->addrmap.row_b0_17[4],
		cfg->addrmap.row_b0_17[5],
		cfg->addrmap.row_b0_17[6],
		cfg->addrmap.row_b0_17[7],
		cfg->addrmap.row_b0_17[8],
		cfg->addrmap.row_b0_17[9],
		cfg->addrmap.row_b0_17[10],
		cfg->addrmap.row_b0_17[11],
		cfg->addrmap.row_b0_17[12],
		cfg->addrmap.row_b0_17[13],
		cfg->addrmap.row_b0_17[14],
		cfg->addrmap.row_b0_17[15],
		cfg->addrmap.row_b0_17[16],
		cfg->addrmap.row_b0_17[17]);
	al_dbg("\tcs[0..1] = %u %u\n",
		cfg->addrmap.cs_b0_1[0],
		cfg->addrmap.cs_b0_1[1]);
	al_dbg("\tswap addr[16..14] with addr[19..17] = %s\n",
		cfg->addrmap.swap_14_16_with_17_19 ? "yes" : "no");

	al_dbg("Timing\n");
	al_dbg("\tref_clk_freq_mhz = %u\n", cfg->tmg.ref_clk_freq_mhz);
	al_dbg("\tddr_freq = %u MHz\n",
		(cfg->tmg.ddr_freq == AL_DDR_FREQ_800) ? 800 :
		(cfg->tmg.ddr_freq == AL_DDR_FREQ_1066) ? 1066 :
		(cfg->tmg.ddr_freq == AL_DDR_FREQ_1333) ? 1333 :
		(cfg->tmg.ddr_freq == AL_DDR_FREQ_1600) ? 1600 :
		(cfg->tmg.ddr_freq == AL_DDR_FREQ_1866) ? 1866 :
		(cfg->tmg.ddr_freq == AL_DDR_FREQ_2133) ? 2133 :
		(cfg->tmg.ddr_freq == AL_DDR_FREQ_2400) ? 2400 :
		(cfg->tmg.ddr_freq == AL_DDR_FREQ_2666) ? 2666 :
		0);
	al_dbg("\tt_faw_ps = %u ps\n", cfg->tmg.t_faw_ps);
	al_dbg("\tt_ras_min_ps = %u ps\n", cfg->tmg.t_ras_min_ps);
	al_dbg("\tt_rc_ps = %u ps\n", cfg->tmg.t_rc_ps);
	al_dbg("\tt_rcd_ps = %u ps\n", cfg->tmg.t_rcd_ps);
	al_dbg("\tt_rrd_ps = %u ps\n", cfg->tmg.t_rrd_ps);
	al_dbg("\tt_rp_ps = %u ps\n", cfg->tmg.t_rp_ps);
	al_dbg("\tt_rfc_min_ps = %u ps\n", cfg->tmg.t_rfc_min_ps);
	al_dbg("\tt_rfc2_ps = %u ps\n", cfg->tmg.t_rfc2_ps);
	al_dbg("\tt_rfc4_ps = %u ps\n", cfg->tmg.t_rfc4_ps);
	al_dbg("\tt_wtr_ps = %u ps\n", cfg->tmg.t_wtr_ps);
	al_dbg("\tt_rtp_ps = %u ps\n", cfg->tmg.t_rtp_ps);
	al_dbg("\tt_wr_ps = %u ps\n", cfg->tmg.t_wr_ps);
	al_dbg("\tcl = %u ck\n", cfg->tmg.cl);
	al_dbg("\tcwl = %u ck\n", cfg->tmg.cwl);
	al_dbg("\tal = %u ck\n", cfg->tmg.al);
	al_dbg("\tt_ccd_ps = %u ps\n", cfg->tmg.t_ccd_ps);
	al_dbg("\tt_rrd_s_ps = %u ps\n", cfg->tmg.t_rrd_s_ps);
	al_dbg("Impedance Control\n");
	al_dbg("\tdic = %u\n",
		(cfg->impedance_ctrl.dic == AL_DDR_DIC_RZQ6) ? 6 :
		(cfg->impedance_ctrl.dic == AL_DDR_DIC_RZQ7) ? 7 :
		(cfg->impedance_ctrl.dic == AL_DDR_DIC_RZQ5) ? 5 :
		0);
	al_dbg("\todt = %d\n",
		(cfg->impedance_ctrl.odt == AL_DDR_ODT_DIS) ? 0 :
		(cfg->impedance_ctrl.odt == AL_DDR_ODT_RZQ2) ? 2 :
		(cfg->impedance_ctrl.odt == AL_DDR_ODT_RZQ4) ? 4 :
		(cfg->impedance_ctrl.odt == AL_DDR_ODT_RZQ6) ? 6 :
		(cfg->impedance_ctrl.odt == AL_DDR_ODT_RZQ8) ? 8 :
		(cfg->impedance_ctrl.odt == AL_DDR_ODT_RZQ12) ? 12 :
		(cfg->impedance_ctrl.odt == AL_DDR_ODT_RZQ1) ? 1 :
		(cfg->impedance_ctrl.odt == AL_DDR_ODT_RZQ5) ? 5 :
		(cfg->impedance_ctrl.odt == AL_DDR_ODT_RZQ3) ? 3 :
		(cfg->impedance_ctrl.odt == AL_DDR_ODT_RZQ7) ? 7 :
		-1);
	al_dbg("\todt_dyn = %s\n",
		(cfg->impedance_ctrl.odt_dyn == AL_DDR_ODT_DYN_DIS) ? "Dis" :
		(cfg->impedance_ctrl.odt_dyn == AL_DDR_ODT_DYN_RZQ2) ? "RZQ2" :
		(cfg->impedance_ctrl.odt_dyn == AL_DDR_ODT_DYN_RZQ4) ? "RZQ4" :
		(cfg->impedance_ctrl.odt_dyn == AL_DDR_ODT_DYN_RZQ1) ? "RZQ1" :
		(cfg->impedance_ctrl.odt_dyn == AL_DDR_ODT_DYN_HI_Z) ? "HI Z" :
		"N/A");
	al_dbg("\trtt_park = %d\n",
		(cfg->impedance_ctrl.rtt_park == AL_DDR_RTT_PARK_DIS) ? 0 :
		(cfg->impedance_ctrl.rtt_park == AL_DDR_RTT_PARK_RZQ4) ? 4 :
		(cfg->impedance_ctrl.rtt_park == AL_DDR_RTT_PARK_RZQ2) ? 2 :
		(cfg->impedance_ctrl.rtt_park == AL_DDR_RTT_PARK_RZQ6) ? 6 :
		(cfg->impedance_ctrl.rtt_park == AL_DDR_RTT_PARK_RZQ1) ? 1 :
		(cfg->impedance_ctrl.rtt_park == AL_DDR_RTT_PARK_RZQ5) ? 5 :
		(cfg->impedance_ctrl.rtt_park == AL_DDR_RTT_PARK_RZQ3) ? 3 :
		(cfg->impedance_ctrl.rtt_park == AL_DDR_RTT_PARK_RZQ7) ? 7 :
		-1);
	al_dbg("\thost_initial_vref = %u\n", cfg->impedance_ctrl.host_initial_vref);
	al_dbg("\tvrefdq = %u\n", cfg->impedance_ctrl.vrefdq);
	if (cfg->impedance_ctrl.hv_min != AL_DDR_HV_INVALID_VAL)
		al_dbg("\thv_min = %u\n", cfg->impedance_ctrl.hv_min);
	if (cfg->impedance_ctrl.hv_max != AL_DDR_HV_INVALID_VAL)
		al_dbg("\thv_max = %u\n", cfg->impedance_ctrl.hv_max);
	al_dbg("\tphy_rout_pu = ");
	for (i = 0; i < AL_DDR_PHY_NUM_ZQ_SEGMENTS_ALPINE_V2; i++)
		al_dbg("%u ", cfg->impedance_ctrl.phy_rout_pu[i]);
	al_dbg("\n");
	al_dbg("\tphy_rout_pd = ");
	for (i = 0; i < AL_DDR_PHY_NUM_ZQ_SEGMENTS_ALPINE_V2; i++)
		al_dbg("%u ", cfg->impedance_ctrl.phy_rout_pd[i]);
	al_dbg("\n");
	al_dbg("\tphy_pu_odt = ");
	for (i = 0; i < AL_DDR_PHY_NUM_ZQ_SEGMENTS_ALPINE_V2; i++)
		al_dbg("%u ", cfg->impedance_ctrl.phy_pu_odt[i]);
	al_dbg("\n");

	al_dbg("\twr_odt_map = %u %u %u %u\n",
		cfg->impedance_ctrl.wr_odt_map[0],
		cfg->impedance_ctrl.wr_odt_map[1],
		cfg->impedance_ctrl.wr_odt_map[2],
		cfg->impedance_ctrl.wr_odt_map[3]);
	al_dbg("\trd_odt_map = %u %u %u %u\n",
		cfg->impedance_ctrl.rd_odt_map[0],
		cfg->impedance_ctrl.rd_odt_map[1],
		cfg->impedance_ctrl.rd_odt_map[2],
		cfg->impedance_ctrl.rd_odt_map[3]);
	al_dbg("\tphy_rout [ohm] = ");
	for (i = 0; i < AL_DDR_PHY_NUM_ZQ_SEGMENTS_ALPINE_V2; i++)
		al_dbg("%u ",
			(cfg->impedance_ctrl.phy_rout[i] == AL_DDR_PHY_ROUT_80OHM) ? 80 :
			(cfg->impedance_ctrl.phy_rout[i] == AL_DDR_PHY_ROUT_68OHM) ? 68 :
			(cfg->impedance_ctrl.phy_rout[i] == AL_DDR_PHY_ROUT_60OHM) ? 60 :
			(cfg->impedance_ctrl.phy_rout[i] == AL_DDR_PHY_ROUT_53OHM) ? 53 :
			(cfg->impedance_ctrl.phy_rout[i] == AL_DDR_PHY_ROUT_48OHM) ? 48 :
			(cfg->impedance_ctrl.phy_rout[i] == AL_DDR_PHY_ROUT_44OHM) ? 44 :
			(cfg->impedance_ctrl.phy_rout[i] == AL_DDR_PHY_ROUT_40OHM) ? 40 :
			(cfg->impedance_ctrl.phy_rout[i] == AL_DDR_PHY_ROUT_37OHM) ? 37 :
			(cfg->impedance_ctrl.phy_rout[i] == AL_DDR_PHY_ROUT_34OHM) ? 34 :
			(cfg->impedance_ctrl.phy_rout[i] == AL_DDR_PHY_ROUT_32OHM) ? 32 :
			(cfg->impedance_ctrl.phy_rout[i] == AL_DDR_PHY_ROUT_30OHM) ? 30 :
			0);
	al_dbg("\n");

	al_dbg("\tphy_odt [ohm] = ");
	for (i = 0; i < AL_DDR_PHY_NUM_ZQ_SEGMENTS_ALPINE_V2; i++)
		al_dbg("%u ",
			(cfg->impedance_ctrl.phy_odt[i] == AL_DDR_PHY_ODT_200OHM) ? 200 :
			(cfg->impedance_ctrl.phy_odt[i] == AL_DDR_PHY_ODT_133OHM) ? 133 :
			(cfg->impedance_ctrl.phy_odt[i] == AL_DDR_PHY_ODT_100OHM) ? 100 :
			(cfg->impedance_ctrl.phy_odt[i] == AL_DDR_PHY_ODT_77OHM) ? 77 :
			(cfg->impedance_ctrl.phy_odt[i] == AL_DDR_PHY_ODT_66OHM) ? 66 :
			(cfg->impedance_ctrl.phy_odt[i] == AL_DDR_PHY_ODT_56OHM) ? 56 :
			(cfg->impedance_ctrl.phy_odt[i] == AL_DDR_PHY_ODT_50OHM) ? 50 :
			(cfg->impedance_ctrl.phy_odt[i] == AL_DDR_PHY_ODT_44OHM) ? 44 :
			(cfg->impedance_ctrl.phy_odt[i] == AL_DDR_PHY_ODT_40OHM) ? 40 :
			(cfg->impedance_ctrl.phy_odt[i] == AL_DDR_PHY_ODT_36OHM) ? 36 :
			(cfg->impedance_ctrl.phy_odt[i] == AL_DDR_PHY_ODT_33OHM) ? 33 :
			(cfg->impedance_ctrl.phy_odt[i] == AL_DDR_PHY_ODT_30OHM) ? 30 :
			(cfg->impedance_ctrl.phy_odt[i] == AL_DDR_PHY_ODT_28OHM) ? 28 :
			(cfg->impedance_ctrl.phy_odt[i] == AL_DDR_PHY_ODT_26OHM) ? 26 :
			(cfg->impedance_ctrl.phy_odt[i] == AL_DDR_PHY_ODT_25OHM) ? 25 :
			0);
	al_dbg("\n");
	al_dbg("\tdqs_res = pull %s ohm\n",
		(cfg->impedance_ctrl.dqs_res == AL_DDR_DQS_RES_PULL_DOWN_500OHM) ? "down 500" :
		(cfg->impedance_ctrl.dqs_res == AL_DDR_DQS_RES_PULL_UP_611OHM) ? "up 611" :
		"N/A");
	al_dbg("\tdqsn_res = pull %s ohm\n",
		(cfg->impedance_ctrl.dqsn_res == AL_DDR_DQSN_RES_PULL_UP_458OHM) ? "up 458" :
		(cfg->impedance_ctrl.dqsn_res == AL_DDR_DQSN_RES_PULL_UP_500OHM) ? "up 500" :
		"N/A");
	al_dbg("Misc\n");
	al_dbg("\tecc_is_enabled = %s\n", cfg->misc.ecc_is_enabled ? "yes" : "no");
	al_dbg("\tecc_scrub_dis = %s\n", cfg->misc.ecc_scrub_dis ? "yes" : "no");
	al_dbg("\ttraining_en = %s\n", cfg->misc.training_en ? "yes" : "no");
	al_dbg("\tphy_dll_en = %s\n", cfg->misc.phy_dll_en ? "yes" : "no");
	al_dbg("\tcrc_enabled = %s\n", cfg->misc.crc_enabled ? "yes" : "no");
	al_dbg("\tparity_enabled = %s\n", cfg->misc.parity_enabled ? "yes" : "no");
	al_dbg("\tocpar_enabled = %s\n", cfg->misc.ocpar_enabled ? "yes" : "no");
	al_dbg("\tread_dbi_enabled = %s\n", cfg->misc.read_dbi_enabled ? "yes" : "no");
	al_dbg("\twrite_dbi_enabled = %s\n", cfg->misc.write_dbi_enabled ? "yes" : "no");
	al_dbg("-----------------------------------------------------------\n");
}

/*******************************************************************************
 ******************************************************************************/
static inline int al_ddr_active_byte_lanes_mark(
	struct al_ddr_init_cfg	*cfg)
{
	unsigned int val_active_byte_lanes_64_bits =
		(cfg->org.data_width == AL_DDR_DATA_WIDTH_64_BITS) ? 1 : 0;
	unsigned int val_active_byte_lanes_32_bits =
		(cfg->org.data_width == AL_DDR_DATA_WIDTH_32_BITS) ? 1 : 0;
	int i;

	/** Lane muxing
		DRAM Data	Alpine V2
		DQ[7:0]   (LSB)	Octet^0
		DQ[15:8]	Octet^1
		DQ[23:16]	Octet^2
		DQ[31:24]	Octet^3
		DQ[39:32]	Octet^4
		DQ[47:40]	Octet^5
		DQ[55:48]	Octet^6
		DQ[63:56] (MSB)	Octet^7
		DQ[71:64] - ECC	Octet^8
	*/
	for (i = 0; i < AL_DDR_PHY_16BIT_BYTE_LANES_ALPINE_V2; i++)
		cfg->calc.active_byte_lanes[i] = 1;

	for (; i < AL_DDR_PHY_32BIT_BYTE_LANES_ALPINE_V2; i++)
		cfg->calc.active_byte_lanes[i] = val_active_byte_lanes_32_bits
					| val_active_byte_lanes_64_bits;

	for (; i < AL_DDR_PHY_ECC_BYTE_LANE_INDEX_ALPINE_V2; i++)
		cfg->calc.active_byte_lanes[i] = val_active_byte_lanes_64_bits;

	cfg->calc.active_byte_lanes[i++] = cfg->org.ecc_is_supported;

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
static inline int al_ddr_ctrl_cfg(
	struct al_ddr_init_cfg	*cfg)
{
	uint16_t mr0;
	uint16_t mr1;
	uint16_t mr2;

	uint16_t mr3 = 0;
	uint16_t mr4 = 0;
	uint16_t mr5 = 0;
	uint16_t mr6 = 0;

	unsigned int active_ranks = active_ranks_calc(cfg->org.dimms, cfg->org.ranks);

	struct al_nb_regs __iomem *nb_regs =
		(struct al_nb_regs __iomem *)cfg->ddr_cfg.nb_regs_base;

	STRUCT_CTRL_REGS __iomem *ctrl_regs = CTRL_REGS(cfg->ddr_cfg.ddr_ctrl_regs_base);
	STRUCT_CTRL_MP_REGS __iomem *ctrl_mp_regs = CTRL_MP_REGS(cfg->ddr_cfg.ddr_ctrl_regs_base);

	uint32_t reg_val;

	/**
	 * If 1, enables DDR4 C/A parity
	 */
	unsigned int en_ca_parity = (cfg->org.ddr_type == AL_DDR_TYPE_DDR3) ?
					0 :
					cfg->misc.parity_enabled;

	/**
	 * If 1, enables DDR4 DM
	 */
	unsigned int dm_enabled =
			(((cfg->org.ddr_device == AL_DDR_DEVICE_X8) ||
			 (cfg->org.ddr_device == AL_DDR_DEVICE_X16))
			 && cfg->misc.write_dbi_enabled == 0) ? 1 : 0;

	/**
	 * PL (Parity latency) is added to Data output delay when C/A parity latency mode is enabled.
	 *
	 * Valid values: 0, 4-5
	 *
	 * Required value: t_pl_table[cfg->tmg.ddr_freq]
	 */
	unsigned int t_pl = en_ca_parity ? t_pl_table[cfg->tmg.ddr_freq] : 0;

	/**
	* If set to 1, read transactions with ID not covered by any of the
	* virtual channel ID mapping registers are not reordered.
	*
	* Required value: 0 for DDR4 32bit DDR, 1 otherwise
	*/
	/**
	 * Addressing RMN: 5193
	 *
	 * RMN description:
	 * Performance degradation when using DDR4 in half-bus (32 bit) mode
	 * Software flow:
	 * Set PCCFG.bl_exp_mode=1 and disable read_reorder_bypass_en
	 */
	unsigned int read_reorder_bypass_en =
			((cfg->org.ddr_type == AL_DDR_TYPE_DDR4) &&
			 (cfg->org.data_width == AL_DDR_DATA_WIDTH_32_BITS)) ?
			0 : 1;


	/**
	 * ZQ initial calibration, tZQINIT [32 nCK]
	 *
	 * Required value:
	 * DDR3 : max(512nCK, 640ns)
	 * DDR4 : 1024nCK
	 */
	unsigned int dev_zqinit_x32 = (cfg->org.ddr_type == AL_DDR_TYPE_DDR3) ?
		al_ddr_max_ck_ns_x32_rnd_up(REF_CLK_FREQ_MHZ, 512, 640) :
		1024/32 + 1;

	/**
	 * tZQOPER: Number of cycles of NOP required after a ZQCL (ZQ
	 * calibration long) command is issued to SDRAM. [nCK]
	 *
	 * Required value:
	 * DDR3 : Max(256ck,320ns)
	 * DDR4 : 512ck
	 */
	unsigned int t_zq_long_nop = (cfg->org.ddr_type == AL_DDR_TYPE_DDR3) ?
		al_ddr_max_ck_ns(REF_CLK_FREQ_MHZ, 256, 320) :
		512;

	/**
	 * tZQCS: Number of cycles of NOP required after a ZQCS (ZQ
	 * calibration short) command is issued to SDRAM. [nCK]
	 *
	 * Required value:
	 * DDR3 : Max(64ck,80ns)
	 * DDR4 : 128ck
	 */
	unsigned int t_zq_short_nop = (cfg->org.ddr_type == AL_DDR_TYPE_DDR3) ?
		al_ddr_max_ck_ns(REF_CLK_FREQ_MHZ, 64, 80) :
		128;
	/**
	 * Average time between refreshes for a given rank. The actual time
	 * between any two refresh commands may be larger or smaller than this.
	 * This represents the maximum time allowed between refresh commands to
	 * a given rank when averaged over a large period of time. [32 nCK]
	 *
	 * Required value: TREFI_X32
	 *
	 * Addressing RMN: 2153
	 *
	 * RMN description:
	 * On specific dimms it seems t_rfc_nom_x32 is not enough.
	 *
	 * Software flow:
	 * Reduce by 800ns
	 */
	unsigned int t_rfc_nom_x32 =
		al_ddr_ns_to_ck_x32_rnd_up(REF_CLK_FREQ_MHZ, TREFI_NS - 800);

	/**
	 * Activate to precharge command period - maximal value [1024 nCK]
	 *
	 * Required value: TREFI_9_X1024
	 */
	unsigned int t_ras_max =
		al_ddr_ns_to_ck_x1024_rnd_dn(REF_CLK_FREQ_MHZ, 9*TREFI_NS);

	/* WRITE recovery time */
	unsigned int t_wr =
		al_ddr_ps_to_ck_rnd_up(REF_CLK_FREQ_MHZ,
				(cfg->org.ddr_type == AL_DDR_TYPE_DDR4) ?
				15000 :
				cfg->tmg.t_wr_ps);

	/* WRITE recovery time */
	unsigned int t_wr_crc_dm = t_wr +
		((cfg->misc.crc_enabled && dm_enabled && (cfg->org.ddr_type == AL_DDR_TYPE_DDR4)) ?
		 al_ddr_max_ck_ps(REF_CLK_FREQ_MHZ, 5, 3750) : 0);

	/**
	 * Delay from start of internal write transaction to internal read
	 * command. [nCK]
	 */
	unsigned int t_wtr = (cfg->org.ddr_type == AL_DDR_TYPE_DDR3) ?
		al_ddr_max_ck_ps(REF_CLK_FREQ_MHZ, 4, cfg->tmg.t_wtr_ps) :
		al_ddr_max_ck_ps(REF_CLK_FREQ_MHZ, 4, 7500) +
			((cfg->misc.crc_enabled && dm_enabled) ?
			 al_ddr_max_ck_ps(REF_CLK_FREQ_MHZ, 5, 3750) : 0);

	/**
	 * Delay from start of internal write transaction to internal read
	 * command for different bank group. [nCK]
	 * Applicable to DDR4 only
	 */
	unsigned int t_wtr_s = al_ddr_max_ck_ps(REF_CLK_FREQ_MHZ, 2, 2500) +
		((cfg->misc.crc_enabled && dm_enabled) ?
		 al_ddr_max_ck_ps(REF_CLK_FREQ_MHZ, 5, 3750) : 0);
;

	/* Internal READ Command to PRECHARGE Command delay [nCK]. */
	unsigned int t_rtp =
		(cfg->org.ddr_type == AL_DDR_TYPE_DDR4) ?
		al_ddr_max_ck_ps(REF_CLK_FREQ_MHZ, 4, 7500) :
		al_ddr_ps_to_ck_rnd_up(REF_CLK_FREQ_MHZ, cfg->tmg.t_rtp_ps);

	CL = CL +
		(cfg->misc.read_dbi_enabled == 1 ?
		 al_rd_dbi_extra_cas_lat_get(REF_CLK_FREQ_MHZ) :
		 0);
	/* Read latency */
	unsigned int rl = (AL + CL + t_pl);

	/* Write latency */
	unsigned int wl = (AL + CWL + t_pl);

	/**
	 * Number of clocks of gap in data responses when performing
	 * consecutive writes to different ranks.
	 * This is used to switch the delays in the PHY to match the rank
	 * requirements.
	 *
	 * Suggested value: 6
	 *
	 * Addressing RMN: 1968
	 * Workaround: Relaxed timing by 8
	*/
	unsigned int diff_rank_wr_gap = 14;

	/**
	 * Number of clocks of gap in data responses when performing
	 * consecutive reads to different ranks.
	 * This is used to switch the delays in the PHY to match the rank
	 * requirements.
	 *
	 * Suggested value: 6
	 *
	 * Addressing RMN: 1968
	 * Workaround: Relaxed timing by 8
	*/
	unsigned int diff_rank_rd_gap = 14;

	/**
	 * Minimum time between write and precharge to same bank. [nCK]
	 *
	 * Required value: (wl + (BL/2) + t_wr_crc_dm)
	 */
	unsigned int wr2pre = (wl + (BL/2) + t_wr_crc_dm);


	/**
	 * Minimum time from a read command to a precharge command to the same
	 * bank [nCK]
	 *
	 * Required value: (AL + MAX(t_rtp, 4))
	 */
	unsigned int rd2pre =
		(AL + al_max_t(unsigned long, t_rtp, 4));

	/**
	 * Minimum time to allow between issuing any read command and issuing
	 * any write command [nCK]
	 *
	 * Required value: (rl + (BL/2) + 2 - wl)
	 */
	/* Workaround: Relaxed read to write timing by 2 */
	unsigned int rd2wr = (rl + (BL/2) + 2 - wl + 2);

	/**
	 * Minimum time to allow between issuing any write command and issuing
	 * any read command. [nCK]
	 *
	 * Required value: (wl + (BL/2) + t_wtr)
	 */
	/* Workaround: Relaxed write to read timing by 2 */
	unsigned int wr2rd = (wl + (BL/2) + t_wtr + 2);

	/**
	 * Minimum time to allow between issuing any write command and issuing
	 * any read command for different bank group. [nCK]
	 * Applicable to DDR4 only
	 *
	 * Required value: (wl + (BL/2) + t_wtr_s)
	 */
	/* Workaround: Relaxed write to read timing by 2 */
	unsigned int wr2rd_s = (wl + (BL/2) + t_wtr_s + 2);

	unsigned int t_cke = (cfg->org.ddr_type == AL_DDR_TYPE_DDR3) ?
		AL_DDR_TMG_TBL_MAX_CK_PS_GET_VAL(t_cke_table) :
		al_ddr_max_ck_ps(REF_CLK_FREQ_MHZ,3,5000);

	unsigned int t_faw = al_ddr_ps_to_ck_rnd_up(
		REF_CLK_FREQ_MHZ,
		cfg->tmg.t_faw_ps);

	unsigned int t_ras_min = al_ddr_ps_to_ck_rnd_up(
		REF_CLK_FREQ_MHZ,
		cfg->tmg.t_ras_min_ps);

	unsigned int t_xp = (cfg->org.ddr_type == AL_DDR_TYPE_DDR3) ?
		AL_DDR_TMG_TBL_MAX_CK_PS_GET_VAL(t_xp_table) :
		AL_DDR_TMG_TBL_MAX_CK_PS_GET_VAL(t_xp_table_ddr4) + t_pl;
	unsigned int t_rc = al_ddr_ps_to_ck_rnd_up(
		REF_CLK_FREQ_MHZ,
		cfg->tmg.t_rc_ps);

	/* tRCD - AL */
	unsigned int t_rcd = al_ddr_ps_to_ck_rnd_up(
		REF_CLK_FREQ_MHZ,
		cfg->tmg.t_rcd_ps) - AL;

	unsigned int t_rrd = al_ddr_max_ck_ps(
		REF_CLK_FREQ_MHZ,
		4,
		cfg->tmg.t_rrd_ps);

	unsigned int t_rrd_s = al_ddr_max_ck_ps(
		REF_CLK_FREQ_MHZ,
		4,
		cfg->tmg.t_rrd_s_ps);

	unsigned int t_rp = al_ddr_ps_to_ck_rnd_up(
		REF_CLK_FREQ_MHZ,
		cfg->tmg.t_rp_ps);

	/**
	 * Minimum CKE low width for Self Refresh entry to exit timing [nCK]
	 *
	 * Required value: t_cke + 1
	 */
	unsigned int t_ckesr = t_cke + 1;

	/**
	 * Mode Register Set command update delay [nCK]
	 *
	 * Required value:
	 * DDR3 : al_ddr_max_ck_ns(REF_CLK_FREQ_MHZ, 12, 15)
	 * DDR4 : al_ddr_max_ck_ns(REF_CLK_FREQ_MHZ, 24, 15)
	 */
	unsigned int t_mod = al_ddr_max_ck_ns(REF_CLK_FREQ_MHZ,
			((cfg->org.ddr_type == AL_DDR_TYPE_DDR3) ? 12 : 24), 15)
			 + cfg->org.rdimm
			 + (en_ca_parity ? t_pl : 0);

	/**
	* Mode Register Set command cycle time
	*
	* Required value: 4 for DDR3, 8 or 9 for DDR4
	*/
	unsigned int t_mrd = (cfg->org.ddr_type == AL_DDR_TYPE_DDR3) ? 4 :
				(en_ca_parity ? t_mod : t_mrd_table[cfg->tmg.ddr_freq]);

	/**
	 * Valid Clock Requirement after Self Refresh Entry (SRE) or Power-Down
	 * Entry (PDE)
	 * Specifies the number of DFI clock cycles from the de-assertion of the
	 * ddrc_dfi_dram_clk_disable signal on the DFI until the first valid
	 * rising edge of the clock to the DRAM memory devices, at the PHY-DRAM
	 * boundary. [nCK]
	 *
	 * Required value: al_ddr_max_ck_ns(REF_CLK_FREQ_MHZ, 5, 10)
	 */
	unsigned int t_cksre = al_ddr_max_ck_ns(REF_CLK_FREQ_MHZ, 5, 10) + rr_mode;

	/**
	 * Valid Clock Requirement before Self Refresh Exit (SRX) or Power-Down
	 * Exit (PDX) or Reset Exit [nCK]
	 *
	 * Required value: al_ddr_max_ck_ns(REF_CLK_FREQ_MHZ, 5, 10)
	 */
	unsigned int t_cksrx = al_ddr_max_ck_ns(REF_CLK_FREQ_MHZ, 5, 10);

	/**
	 * Time from the assertion of a read command on the DFI interface to the
	 * assertion of the ddrc_dfi_rddata_en signal. [nCK]
	 *
	 * Required value: AL_DDR_ODD_EVEN_DEC(rl + cfg->org.rdimm, 3, 4)
	 */
	unsigned int dfi_t_rddata_en =
		/* PHY is compensating for RDIMM extra
		* read cycle if running training
		*/
		rl + (cfg->misc.training_en == 1 ? 0 : cfg->org.rdimm) - 4;

	/**
	 * Number of clocks from the write command to write data enable. [nCK]
	 *
	 * Required value: AL_DDR_ODD_EVEN_DEC(wl + cfg->org.rdimm, 3, 4)
	 */
	unsigned int dfi_tphy_wrlat =
		/* PHY is compensating for RDIMM extra write
		* cycle if running training
		*/
		wl +
		(cfg->misc.training_en == 1 ?
		  0 :
		  cfg->org.rdimm)
		- 2;


	/**
	 * Time from read command to read data on SDRAM interface
	 *
	 * Required value: rl + cfg->org.rdimm
	 */
	unsigned int read_latency =
		/* PHY is compensating for RDIMM extra
		* read cycle it running training
		*/
		rl + (cfg->misc.training_en == 1 ? 0 : cfg->org.rdimm);

	/**
	 * Time from write command to write data on SDRAM interface
	 *
	 * Required value: wl + cfg->org.rdimm
	 */
	unsigned int write_latency =
		/* PHY is compensating for RDIMM extra write
		* cycle it running training with DDR3
		*/
		wl +
		(((cfg->misc.training_en == 1) &&
		 (cfg->org.ddr_type == AL_DDR_TYPE_DDR3)) ?
		  0 :
		  cfg->org.rdimm);

	unsigned int t_rfc_min =
		al_ddr_ps_to_ck_rnd_up(REF_CLK_FREQ_MHZ, cfg->tmg.t_rfc_min_ps);

	/**
	 * Self Refresh to commands not requiring a locked DLL [nCK]
	 *
	 * Required value:
	 * DDR3 : MAX(5nCK, tRFC(min) + 10ns)
	 * DDR4 : tRFC(min) + 10ns
	 */
	unsigned int t_xs = (cfg->org.ddr_type == AL_DDR_TYPE_DDR3) ?
		al_max_t(unsigned int, 5, al_ddr_ps_to_ck_rnd_up(
			REF_CLK_FREQ_MHZ, cfg->tmg.t_rfc_min_ps + 10000)) :
		al_ddr_ps_to_ck_rnd_up(
			REF_CLK_FREQ_MHZ, cfg->tmg.t_rfc_min_ps + 10000);
	unsigned int t_xs_x32 = al_ddr_ps_to_ck_x32_rnd_up(
		REF_CLK_FREQ_MHZ,cfg->tmg.t_rfc_min_ps + 10000);


	/**
	 * SRX to commands not requiring a locked DLL in Self Refresh ABORT
	 *
	 * Required value: tRFC4(min) + 10ns
	 */
	unsigned int t_xs_abort_x32 = al_ddr_ps_to_ck_x32_rnd_up(
			REF_CLK_FREQ_MHZ, cfg->tmg.t_rfc4_ps + 10000);

	/**
	 * Exit Self Refresh to ZQCL,ZQCS and MRS (CL,CWL,WR,RTP and Gear Down)
	 *
	 * Required value: tRFC4(min) + 10ns
	 */
	unsigned int t_xs_fast_x32 = al_ddr_ps_to_ck_x32_rnd_up(
			REF_CLK_FREQ_MHZ, cfg->tmg.t_rfc4_ps + 10000);

	/**
	 * Exit Self Refresh to commands requiring a locked DLL
	 *
	 * Required value: tDLLK
	 */
	unsigned int t_dllk =  (cfg->org.ddr_type == AL_DDR_TYPE_DDR3) ?
				512 :
				t_dllk_table[cfg->tmg.ddr_freq];
	unsigned int t_xs_dll_x32 = (t_dllk / 32) + 1;

	unsigned int t_ck_ps = 1000000 / REF_CLK_FREQ_MHZ;
	/**
	 * Cycles to wait after driving CKE high to start the SDRAM
	 * initialization sequence. [1024 nCK]
	 *
	 * Required value:
	 * MAX(t_xs, 5 x tCK)
	 */
	unsigned int post_cke_x1024 =
		((al_max_t(unsigned int, t_xs, al_ddr_ps_to_ck_rnd_up(
		REF_CLK_FREQ_MHZ, 5 * t_ck_ps)) + 1023) / 1024) + 1;

	/**
	 * Cycles to wait after reset before driving CKE high to start the SDRAM
	 * initialization sequence. [1024 nCK]
	 *
	 * Required value:
	 * al_ddr_ns_to_ck_x1024_rnd_up(REF_CLK_FREQ_MHZ, 500000)
	 */
	unsigned int pre_cke_x1024 =
		al_ddr_ns_to_ck_x1024_rnd_up(REF_CLK_FREQ_MHZ, 500000);
	/* TODO : this value should increase for DDR RDIMM - need to find the correct value */

	/**
	 * Number of cycles to assert SDRAM reset signal during init sequence.
	 * [1024 nCK]
	 *
	 * Required value:
	 * al_ddr_ns_to_ck_x1024_rnd_up(REF_CLK_FREQ_MHZ, 200000)
	 */
	unsigned int dram_rstn_x1024 =
		al_ddr_ns_to_ck_x1024_rnd_up(REF_CLK_FREQ_MHZ, 200000);

	/**
	 * Enable controller acknowledgements for PHY DLL calibrations requests.
	 * When the PHY initiates a DLL calbration it requests the controller
	 * to hold incoming traffic and waits for its acknowledgement.
	 *
	 * Related to MR1
	 *
	 * Suggested value: 1
	 */
	unsigned int dfi_phyupd_en = cfg->misc.phy_dll_en;

	/**
	 * The delay, in clock cycles, from issuing a read command to setting ODT values
	 * associated with that command.
	 *
	 * Valid values: 0-15
	 *
	 * Suggested value: CL - WL
	 */
	unsigned int rd_odt_delay = (cfg->org.ddr_type == AL_DDR_TYPE_DDR3) ?
					rl - wl :
					(rl - wl > 0 ? rl - wl - 1 : 0);

	/**
	* Cycles to hold ODT for a read command. When 0x0, ODT signal is ON
	* for 1 cycle. When 0x1, it is ON for 2 cycles, and so on.
	*
	* Required value: 6 (assuming mstr.burst_rdwr = 4)
	*/
	/* TODO : CDNS insist read ODT should care about CRC. Need to check on silicon */
	unsigned int rd_odt_hold = 6 + cfg->misc.crc_enabled;
	/**
	* Cycles to hold ODT for a write command. When 0x0, ODT signal is ON
	* for 1 cycle. When 0x1, it is ON for 2 cycles, and so on.
	*
	* Required value: 6 (assuming mstr.burst_rdwr = 4)
	*/
	unsigned int wr_odt_hold = 6 + cfg->misc.crc_enabled;

	/**
	 * CAS^ to CAS^ command delay same bank group
	 *
	 * Valid values: 4-8
	 *
	 * Required value:
	 * DDR3 : 4
	 * DDR4: al_ddr_ps_to_ck_rnd_up(
	 *	REF_CLK_FREQ_MHZ,
	 *	cfg->tmg.t_ccd_ps);
	 */
	unsigned int t_ccd_l_raw = (cfg->org.ddr_type == AL_DDR_TYPE_DDR3) ? 4 :
			al_ddr_ps_to_ck_rnd_up(REF_CLK_FREQ_MHZ,
			cfg->tmg.t_ccd_ps);
	unsigned int t_ccd = (cfg->misc.crc_enabled && (t_ccd_l_raw == 4)) ? 5 : t_ccd_l_raw;

	/**
	* CAS^ to CAS^ command delay for different bank group.
	* Applicable only to DDR4
	*
	* Required value: 4
	*/
	unsigned int t_ccd_s = cfg->misc.crc_enabled ? 5 : 4;

	/**
	 * Write CMD Latency when CRC and DM are enabled
	 *
	 * Valid values: 4-6
	 *
	 * Required value: t_crc_dm_wcl_table[cfg->tmg.ddr_freq]
	 */
	unsigned int t_crc_dm_wcl = t_crc_dm_wcl_table[cfg->tmg.ddr_freq];

	/**
	* Specifies the number of DFI clock cycles after an assertion or
	* de-assertion of the DFI control signals that the control signals at
	* the PHY-DRAM interface reflect the assertion or de-assertion.
	*
	* Required value: 7 for SDR CTRL, 3 for HDR
	*/
	unsigned int dfi_t_ctrl_delay = ddr_ctrl_sdr_mode ? 7 : 3;

	/**
	* Specifies the number of clock cycles between when dfi_wrdata_en is
	* asserted to when the associated write data is driven on the
	* dfi_wrdata signal. This corresponds to the DFI timing parameter
	* tphy_wrdata.
	*
	* Required value: 0 for SDR CTRL, 2 for HDR
	*/
	unsigned int dfi_tphy_wrdata = ddr_ctrl_sdr_mode ? 0 : 2;

	/**
	* Specifies the number of DFI clock cycles from the assertion of the
	* ddrc_dfi_dram_clk_disable signal on the DFI until the clock to the
	* DRAM memory devices, at the PHY-DRAM boundary, maintains a low value.
	*
	* Required value: 3 for SDR CTRL, 2 for HDR
	*/
	unsigned int dfi_t_dram_clk_disable = ddr_ctrl_sdr_mode ? 3 : 2;

	/**
	* Specifies the number of DFI clock cycles from the de-assertion of the
	* ddrc_dfi_dram_clk_disable signal on the DFI until the first valid
	* rising edge of the clock to the DRAM memory devices, at the PHY-DRAM
	* boundary.
	*
	* Required value: 4 for SDR CTRL, 2 for HDR
	*/
	unsigned int dfi_t_dram_clk_enable = ddr_ctrl_sdr_mode ? 4 : 2;

	/**
	* CRC ALERT_n pulse width max
	*
	* Required value: 10
	*/
	unsigned int t_crc_alert_pw_max = 10;

	/**
	* Indicates the maximum duration in number of Controller clock cycles
	* for which a command should be held in the Command Retry FIFO before
	* it is popped out.
	*
	* Recommended values:
	* - Only C/A Parity is enabled
	* (PHY Command Latency(DRAM CLK) + CAL + tPAR_ALERT_PW + tPAR_UNKNOWN
	*	+ PHY Alert Latency(DRAM CLK)) / 4 + 1
	* - Both C/A Parity and CRC is enabled/ Only CRC is enabled.
	* (PHY Command Latency(DRAM CLK) + CAL + WL + 5 + tCRC_ALERT_ON.max
	*	+ PHY Alert Latency(DRAM CLK)) / 4 + 1
	*
	* Max value can be set to this register is defined below:
	* Full bus Mode (CRC=OFF)     Max value = RETRY_CMD_FIFO_DEPTH - 1
	* Full bus Mode (CRC=ON)      Max value = RETRY_CMD_FIFO_DEPTH - 2
	* Half bus Mode (CRC=OFF)     Max value = RETRY_CMD_FIFO_DEPTH - 2
	* Half bus Mode (CRC=ON)      Max value = RETRY_CMD_FIFO_DEPTH - 3
	* Quarter bus Mode (CRC=OFF)  Max value = RETRY_CMD_FIFO_DEPTH - 4
	* Quarter bus Mode (CRC=ON)   Max value = RETRY_CMD_FIFO_DEPTH - 6
	*/
	unsigned int retry_fifo_max_hold_timer_x4 =
		((cfg->org.data_width == AL_DDR_DATA_WIDTH_64_BITS) ?
			RETRY_CMD_FIFO_DEPTH - 2 :
		((cfg->org.data_width == AL_DDR_DATA_WIDTH_32_BITS) ?
			RETRY_CMD_FIFO_DEPTH - 3 :
			RETRY_CMD_FIFO_DEPTH - 6));

	/**
	* Pulse width of parity ALERT_n signal when asserted
	*
	* Required value: t_par_alert_pw_max_table[cfg->tmg.ddr_freq];
	*/
	/**
	 * Addressing RMN: 5585
	 *
	 * RMN description:
	 * DDR4 Retry: A PREA Command can be Sent Before tXS is Satisfied After tXS_fast
	 * Software flow:
	 * Set CRCPARCTL2.t_par_alert_pw_max to the larger of the following two calculations:
	 * 1. tPAR_ALERT_PW.max (JEDEC).
	 * 2. (DRAMTMG8.t_xs_x32 - DRAMTMG8.t_xs_fast_x32)*32 -
	 *	(CRCPARCTL2.retry_fifo_max_hold_timer_x4*2)
	 */
	unsigned int t_par_alert_pw_max =
		al_max_t(long, t_par_alert_pw_max_table[cfg->tmg.ddr_freq],
			(long)((t_xs_x32 - t_xs_fast_x32)*32 - retry_fifo_max_hold_timer_x4*2));


	/* DDR controller 1:2 freq mode timing params */
	unsigned int t_rfc_nom_x32_ctrl = ddr_ctrl_sdr_mode ?
						t_rfc_nom_x32 :
						t_rfc_nom_x32/2;
	unsigned int t_rfc_min_ctrl = ddr_ctrl_sdr_mode ?
					t_rfc_min :
					al_ddr_div2_rnd_up(t_rfc_min);

	unsigned int post_cke_x1024_ctrl = ddr_ctrl_sdr_mode ?
					post_cke_x1024 :
					al_ddr_div2_rnd_up(post_cke_x1024) + 1;
	unsigned int pre_cke_x1024_ctrl = ddr_ctrl_sdr_mode ?
					pre_cke_x1024 :
					al_ddr_div2_rnd_up(pre_cke_x1024) + 1;
	unsigned int dram_rstn_x1024_ctrl = ddr_ctrl_sdr_mode ?
					dram_rstn_x1024 :
					al_ddr_div2_rnd_up(dram_rstn_x1024) + 1;

	unsigned int diff_rank_wr_gap_ctrl = ddr_ctrl_sdr_mode ?
					diff_rank_wr_gap :
					al_ddr_div2_rnd_up(diff_rank_wr_gap);
	unsigned int diff_rank_rd_gap_ctrl = ddr_ctrl_sdr_mode ?
					diff_rank_rd_gap :
					al_ddr_div2_rnd_up(diff_rank_rd_gap);

	unsigned int wr2pre_ctrl = ddr_ctrl_sdr_mode ?
					wr2pre :
					(en_2t_timing_mode ?
					 wr2pre/2 + 1:
					 wr2pre/2);

	unsigned int t_faw_ctrl = ddr_ctrl_sdr_mode ?
					t_faw :
					al_ddr_div2_rnd_up(t_faw);
	unsigned int t_ras_max_ctrl = ddr_ctrl_sdr_mode ?
					t_ras_max :
					(t_ras_max - 1)/2;
	unsigned int t_ras_min_ctrl = ddr_ctrl_sdr_mode ?
					t_ras_min :
					(en_2t_timing_mode ?
					 t_ras_min/2 + 1 :
					 t_ras_min/2);
	unsigned int t_xp_ctrl = ddr_ctrl_sdr_mode ?
					t_xp :
					al_ddr_div2_rnd_up(t_xp);
	unsigned int rd2pre_ctrl = ddr_ctrl_sdr_mode ?
					rd2pre :
					(en_2t_timing_mode ?
					 rd2pre/2 + 1 :
					 rd2pre/2);
	unsigned int t_rc_ctrl = ddr_ctrl_sdr_mode ?
					t_rc :
					al_ddr_div2_rnd_up(t_rc);

	unsigned int write_latency_ctrl = ddr_ctrl_sdr_mode ?
					write_latency :
					al_ddr_div2_rnd_up(write_latency);
	unsigned int read_latency_ctrl = ddr_ctrl_sdr_mode ?
					read_latency :
					al_ddr_div2_rnd_up(read_latency);
	unsigned int rd2wr_ctrl = ddr_ctrl_sdr_mode ?
					rd2wr :
					al_ddr_div2_rnd_up(rd2wr);
	unsigned int wr2rd_ctrl = ddr_ctrl_sdr_mode ?
					wr2rd :
					al_ddr_div2_rnd_up(wr2rd);

	unsigned int t_mrd_ctrl = ddr_ctrl_sdr_mode ?
					t_mrd :
					al_ddr_div2_rnd_up(t_mrd);
	unsigned int t_mod_ctrl = ddr_ctrl_sdr_mode ?
					t_mod:
					al_ddr_div2_rnd_up(t_mod);
	unsigned int t_rcd_ctrl = ddr_ctrl_sdr_mode ?
					t_rcd :
					al_ddr_div2_rnd_up(t_rcd);
	unsigned int t_ccd_ctrl = ddr_ctrl_sdr_mode ?
					t_ccd :
					al_ddr_div2_rnd_up(t_ccd);
	unsigned int t_rrd_ctrl = ddr_ctrl_sdr_mode ?
					t_rrd :
					al_ddr_div2_rnd_up(t_rrd);
	unsigned int t_rp_ctrl = ddr_ctrl_sdr_mode ?
					t_rp :
					t_rp/2 + 1;
	unsigned int t_cksrx_ctrl = ddr_ctrl_sdr_mode ?
					t_cksrx :
					al_ddr_div2_rnd_up(t_cksrx);
	unsigned int t_cksre_ctrl = ddr_ctrl_sdr_mode ?
					t_cksre :
					al_ddr_div2_rnd_up(t_cksre);
	unsigned int t_ckesr_ctrl = ddr_ctrl_sdr_mode ?
					t_ckesr :
					al_ddr_div2_rnd_up(t_ckesr);
	unsigned int t_cke_ctrl = ddr_ctrl_sdr_mode ?
					t_cke :
					al_ddr_div2_rnd_up(t_cke);

	unsigned int t_xs_fast_x32_ctrl = ddr_ctrl_sdr_mode ?
					t_xs_fast_x32 :
					al_ddr_div2_rnd_up(t_xs_fast_x32) + 1;
	unsigned int t_xs_abort_x32_ctrl = ddr_ctrl_sdr_mode ?
					t_xs_abort_x32 :
					al_ddr_div2_rnd_up(t_xs_abort_x32) + 1;
	unsigned int t_xs_dll_x32_ctrl = ddr_ctrl_sdr_mode ?
					t_xs_dll_x32 :
					al_ddr_div2_rnd_up(t_xs_dll_x32) + 1;
	unsigned int t_xs_x32_ctrl = ddr_ctrl_sdr_mode ?
					t_xs_x32 :
					al_ddr_div2_rnd_up(t_xs_x32) + 1;

	unsigned int t_ccd_s_ctrl = ddr_ctrl_sdr_mode ?
					t_ccd_s :
					al_ddr_div2_rnd_up(t_ccd_s);
	unsigned int t_rrd_s_ctrl = ddr_ctrl_sdr_mode ?
					t_rrd_s :
					al_ddr_div2_rnd_up(t_rrd_s);
	unsigned int wr2rd_s_ctrl = ddr_ctrl_sdr_mode ?
					wr2rd_s :
					al_ddr_div2_rnd_up(wr2rd_s);

	unsigned int t_zq_long_nop_ctrl = ddr_ctrl_sdr_mode ?
					t_zq_long_nop :
					al_ddr_div2_rnd_up(t_zq_long_nop);
	unsigned int t_zq_short_nop_ctrl = ddr_ctrl_sdr_mode ?
					t_zq_short_nop :
					al_ddr_div2_rnd_up(t_zq_short_nop);

	unsigned int t_par_alert_pw_max_ctrl = ddr_ctrl_sdr_mode ?
					t_par_alert_pw_max :
					al_ddr_div2_rnd_up(t_par_alert_pw_max);

	unsigned int t_crc_alert_pw_max_ctrl = ddr_ctrl_sdr_mode ?
					t_crc_alert_pw_max :
					al_ddr_div2_rnd_up(t_crc_alert_pw_max);

	/* TODO : add dfi_t_wrdata_delay configuration */


	al_assert(rd_odt_delay <= AL_DDR_CTRL_MAX_READ_ODT_DELAY);

	/**
	 * Addressing RMN: 11375
	 *
	 * RMN description:
	 * ACT/WRA or ACT/RDA in same HDR cycle may result in JEDEC protocol violation when DQS
	 * Drift Detection is Enabled
	 * Software flow:
	 * Ensure that tRCD - AL > 1, which will force any ACT/WRA or ACT/RDA to be in different
	 * HDR clock cycles.
	 */
	al_assert(t_rcd > 1);

	/*
	* Addressing RMN: 5771
	*
	* RMN description:
	* Precharge command sent incorrectly during read/write command burst expansion
	* Software flow:
	* Ensure that DRAMTMG1.rd2pre >= DRAMTMG4.t_ccd
	*/
	al_assert(rd2pre >= t_ccd);


	cfg->calc.t_rp = t_rp;
	cfg->calc.t_ras_min = t_ras_min;
	cfg->calc.t_rc = t_rc;
	cfg->calc.t_rrd = t_rrd;
	cfg->calc.t_rcd = t_rcd;
	cfg->calc.t_rtp = t_rtp;
	cfg->calc.t_wtr = t_wtr;
	cfg->calc.rd_odt_delay = rd_odt_delay;
	cfg->calc.t_pl = t_pl;
	cfg->calc.t_ccd = t_ccd;
	cfg->calc.t_rfc_min = t_rfc_min;
	cfg->calc.t_dllk = t_dllk;

#if (!defined(AL_DDR_PHY_DISABLED) || (AL_DDR_PHY_DISABLED == 0))
#if (defined(AL_DDR_VERIF_SHORT_TIME) && (AL_DDR_VERIF_SHORT_TIME == 1))
	{
		STRUCT_PHY_REGS __iomem *phy_regs = PHY_REGS(cfg->ddr_cfg.ddr_phy_regs_base);

		pre_cke_x1024_ctrl = 1;
		post_cke_x1024_ctrl = 1;
		dram_rstn_x1024_ctrl = 1;

		/* PTR0  */
		/* to reduce simulation time */
		reg_val = 0;
		reg_val |= (0x10 << ALPINE_V2_DWC_DDR_PHY_REGS_PTR0_TPHYRST_SHIFT);
		reg_val |= (0x10 << ALPINE_V2_DWC_DDR_PHY_REGS_PTR0_TPLLGS_SHIFT);
		reg_val |= (0x80 << ALPINE_V2_DWC_DDR_PHY_REGS_PTR0_TPLLPD_SHIFT);
		al_reg_write32(&phy_regs->ptr[0], reg_val);

		/* PTR1 */
		/* to reduce simulation time */
		reg_val = 0;
		reg_val |= (0x80 << ALPINE_V2_DWC_DDR_PHY_REGS_PTR1_TPLLRST_SHIFT);
		reg_val |= (0x80 << ALPINE_V2_DWC_DDR_PHY_REGS_PTR1_TPLLLOCK_SHIFT);
		al_reg_write32(&phy_regs->ptr[1], reg_val);

		/* PGCR1 */
		/* to reduce simulation time */
		al_reg_write32_masked(
			&phy_regs->pgcr[1],
			ALPINE_V2_DWC_DDR_PHY_REGS_PGCR1_FDEPTH_MASK,
			ALPINE_V2_DWC_DDR_PHY_REGS_PGCR1_FDEPTH_2);

		/* ZQCR */
		/* to reduce simulation time */
		al_reg_write32_masked(
			&phy_regs->zqcr,
			ALPINE_V2_DWC_DDR_PHY_REGS_ZQCR_AVGMAX_MASK,
			ALPINE_V2_DWC_DDR_PHY_REGS_ZQCR_AVGMAX_2);
	}
#endif /* AL_DDR_VERIF_SHORT_TIME */
#endif

/* TODO : trigger sw_done after dfi_init done? */
	/* Enable Quasi-Dynamic registers change */
	al_reg_write32(&ctrl_regs->swctl, 0);

	al_reg_write32_masked(
		&ctrl_regs->dfimisc,
		DWC_DDR_UMCTL2_REGS_DFIMISC_DFI_INIT_COMPLETE_EN,
		0);

	al_reg_write32_masked(
		&ctrl_regs->mstr,
		(DWC_DDR_UMCTL2_REGS_MSTR_DEVICE_CONFIG_MASK |
		DWC_DDR_UMCTL2_REGS_MSTR_ACTIVE_RANKS_MASK |
		DWC_DDR_UMCTL2_REGS_MSTR_DATA_BUS_WIDTH_MASK |
		DWC_DDR_UMCTL2_REGS_MSTR_EN_2T_TIMING_MODE |
		DWC_DDR_UMCTL2_REGS_MSTR_BURSTCHOP |
		DWC_DDR_UMCTL2_REGS_MSTR_BURST_MODE |
		DWC_DDR_UMCTL2_REGS_MSTR_DDR3 |
		DWC_DDR_UMCTL2_REGS_MSTR_DDR4),
		(((cfg->org.ddr_device == AL_DDR_DEVICE_X4) ?
		DWC_DDR_UMCTL2_REGS_MSTR_DEVICE_CONFIG_X4 :
		(cfg->org.ddr_device == AL_DDR_DEVICE_X8) ?
		DWC_DDR_UMCTL2_REGS_MSTR_DEVICE_CONFIG_X8 :
		DWC_DDR_UMCTL2_REGS_MSTR_DEVICE_CONFIG_X16) |
		(active_ranks << DWC_DDR_UMCTL2_REGS_MSTR_ACTIVE_RANKS_SHIFT) |
		((cfg->org.data_width == AL_DDR_DATA_WIDTH_16_BITS) ?
		DWC_DDR_UMCTL2_REGS_MSTR_DATA_BUS_WIDTH_16 :
		(cfg->org.data_width == AL_DDR_DATA_WIDTH_32_BITS) ?
		DWC_DDR_UMCTL2_REGS_MSTR_DATA_BUS_WIDTH_32 :
		DWC_DDR_UMCTL2_REGS_MSTR_DATA_BUS_WIDTH_64) |
		(en_2t_timing_mode ?
		 DWC_DDR_UMCTL2_REGS_MSTR_EN_2T_TIMING_MODE : 0) |
		(burstchop ?
		 DWC_DDR_UMCTL2_REGS_MSTR_BURSTCHOP : 0) |
		(burst_mode_seq ?
		 0 : DWC_DDR_UMCTL2_REGS_MSTR_BURST_MODE) |
		((cfg->org.ddr_type == AL_DDR_TYPE_DDR3) ?
		DWC_DDR_UMCTL2_REGS_MSTR_DDR3 :
		DWC_DDR_UMCTL2_REGS_MSTR_DDR4)));


	if (cfg->perf) {
		/*
		* Addressing RMN: 4400
		*
		* RMN description:
		* Critical Refreshes Take Extra Time in DDR3 Stagger CS Mode
		* Software flow:
		* Using single refresh (RFSHCTL0.refresh_burst=0) avoids the issue
		*/
		if ((cfg->org.ddr_type == AL_DDR_TYPE_DDR3) &&
			(cfg->org.udimm_addr_mirroring || cfg->org.rdimm) &&
			(cfg->perf->refresh_burst != 1)) {
			al_err("invld refresh_burst (%d) when using DDR3 Stagger CS Mode!\n",
				cfg->perf->refresh_burst);
			return -EINVAL;
		}

		al_reg_write32_masked(
			&ctrl_regs->rfshctl0,
			DWC_DDR_UMCTL2_REGS_RFSHCTL0_REFRESH_BURST_MASK_ALPINE_V2_V3,
			DWC_DDR_UMCTL2_REGS_RFSHCTL0_REFRESH_BURST_ALPINE_V2_V3(
				cfg->perf->refresh_burst));
	}

	al_reg_write32_masked(
		&ctrl_regs->rfshtmg,
		(DWC_DDR_UMCTL2_REGS_RFSHTMG_T_RFC_NOM_X32_MASK |
		DWC_DDR_UMCTL2_REGS_RFSHTMG_T_RFC_MIN_MASK),
		((t_rfc_nom_x32_ctrl <<
		  DWC_DDR_UMCTL2_REGS_RFSHTMG_T_RFC_NOM_X32_SHIFT) |
		(t_rfc_min_ctrl <<
		 DWC_DDR_UMCTL2_REGS_RFSHTMG_T_RFC_MIN_SHIFT)));

	al_reg_write32_masked(
		&ctrl_regs->ecccfg0,
		DWC_DDR_UMCTL2_REGS_ECCCFG0_ECC_MODE_MASK |
		DWC_DDR_UMCTL2_REGS_ECCCFG0_DIS_SCRUB,
		(cfg->misc.ecc_is_enabled ?
		DWC_DDR_UMCTL2_REGS_ECCCFG0_ECC_MODE_EN :
		DWC_DDR_UMCTL2_REGS_ECCCFG0_ECC_MODE_DIS) |
		(cfg->misc.ecc_scrub_dis ?
		DWC_DDR_UMCTL2_REGS_ECCCFG0_DIS_SCRUB :
		0));

	al_reg_write32_masked(
		&ctrl_regs->crcparctl0,
		DWC_DDR_UMCTL2_REGS_CRCPARCTL0_DFI_ALERT_ERR_INT_EN,
		(cfg->misc.parity_enabled || cfg->misc.crc_enabled) ?
			DWC_DDR_UMCTL2_REGS_CRCPARCTL0_DFI_ALERT_ERR_INT_EN :
			0);

	/**
	 * Addressing RMN: 11437
	 *
	 * RMN description:
	 * DDR4 tPAR_ALERT_RSP Violation May Occur With Persistent CA Parity Checking
	 * in Quarter Bus Mode
	 * Software flow:
	 * Full WA detailed below. Current WA is disabled Parity feature.
	 * 1) If CRCPARCTL1.alert_wait_for_sw=1, the software must clear Parity error status bit
	 * by issuing MRS to resume parity checking before clearing dfi_alert_err_int as normal.
	 * 2) If CRCPARCTL1.alert_wait_for_sw=0, the software can use the following options:
	 * a. The software issues MRS to clear the Parity error status bit when it detects
	 * dfi_parity_err_int. Parity checking is disabled until the software issues MRS to clear
	 * the Parity error status bit
	 *
	 * b. The software sets PWRCTL.selfref_sw=1 when it detects dfi_parity_err_int.
	 * The uMCTL2 Controller issues MRS to clear the Parity error status bit automatically
	 * before entering Self-Refresh or after exiting Self-Refresh.
	 * Parity checking is disabled until uMCTL2 enters Self-Refresh.
	 *
	 * c. The software does not do anything explicitly.
	 * Parity checking is disabled until uMCTL2 enters Self-Refresh.
	 * Self-Refresh can occur either through Automatic-Self-Refresh (enabled by
	 * PWRCTL.selfref_en) or hardware low power interface (enabled by HWLPCTL.hw_lp_en)
	 */
	al_assert(((cfg->org.data_width == AL_DDR_DATA_WIDTH_16_BITS) &&
		  (cfg->misc.parity_enabled == AL_FALSE)) ||
		  (cfg->org.data_width != AL_DDR_DATA_WIDTH_16_BITS));

	/**
	* Addressing RMN: 5110
	*
	* RMN description:
	* A ZQCS can be issued without retry protection
	* Software flow:
	* Set CRCPARCTL1.alert_wait_for_sw==0
	*/
	al_reg_write32_masked(
		&ctrl_regs->crcparctl1,
		DWC_DDR_UMCTL2_REGS_CRCPARCTL1_PARITY_ENABLE |
		DWC_DDR_UMCTL2_REGS_CRCPARCTL1_CRC_ENABLE |
		DWC_DDR_UMCTL2_REGS_CRCPARCTL1_CRC_INC_DM |
		DWC_DDR_UMCTL2_REGS_CRCPARCTL1_CRC_PARITY_RETRY_ENABLE |
		DWC_DDR_UMCTL2_REGS_CRCPARCTL1_ALERT_WAIT_FOR_SW,
		((cfg->misc.parity_enabled ?
			DWC_DDR_UMCTL2_REGS_CRCPARCTL1_PARITY_ENABLE :
			0) |
		 (cfg->misc.crc_enabled ?
			DWC_DDR_UMCTL2_REGS_CRCPARCTL1_CRC_ENABLE :
			0) |
		 DWC_DDR_UMCTL2_REGS_CRCPARCTL1_CRC_INC_DM |	/* TODO : check with CDNS */
		 (crc_parity_retry_enable ?
			DWC_DDR_UMCTL2_REGS_CRCPARCTL1_CRC_PARITY_RETRY_ENABLE :
			0)));

	al_reg_write32_masked(
		&ctrl_regs->crcparctl2,
		(DWC_DDR_UMCTL2_REGS_CRCPARCTL2_RETRY_FIFO_MAX_HOLD_TIMER_X4_MASK |
		 DWC_DDR_UMCTL2_REGS_CRCPARCTL2_T_PAR_ALERT_PW_MAX_MASK |
		 DWC_DDR_UMCTL2_REGS_CRCPARCTL2_T_CRC_ALERT_PW_MAX_MASK),
		((t_par_alert_pw_max_ctrl <<
			DWC_DDR_UMCTL2_REGS_CRCPARCTL2_T_PAR_ALERT_PW_MAX_SHIFT) |
		 (t_crc_alert_pw_max_ctrl <<
			DWC_DDR_UMCTL2_REGS_CRCPARCTL2_T_CRC_ALERT_PW_MAX_SHIFT) |
		 (retry_fifo_max_hold_timer_x4 <<
			DWC_DDR_UMCTL2_REGS_CRCPARCTL2_RETRY_FIFO_MAX_HOLD_TIMER_X4_SHIFT)));

	if (cfg->org.ddr_type == AL_DDR_TYPE_DDR4) {
		al_reg_write32(
			&ctrl_regs->dqmap0,
			((cfg->org.dq_nibble_map[0] <<
				DWC_DDR_UMCTL2_REGS_DQMAP0_DQ_NIBBLE_MAP_0_3_SHIFT) |
			 (cfg->org.dq_nibble_map[1] <<
				DWC_DDR_UMCTL2_REGS_DQMAP0_DQ_NIBBLE_MAP_4_7_SHIFT) |
			 (cfg->org.dq_nibble_map[2] <<
				DWC_DDR_UMCTL2_REGS_DQMAP0_DQ_NIBBLE_MAP_8_11_SHIFT) |
			 (cfg->org.dq_nibble_map[3] <<
				DWC_DDR_UMCTL2_REGS_DQMAP0_DQ_NIBBLE_MAP_12_15_SHIFT)));

		al_reg_write32(
			&ctrl_regs->dqmap1,
			((cfg->org.dq_nibble_map[4] <<
				DWC_DDR_UMCTL2_REGS_DQMAP1_DQ_NIBBLE_MAP_16_19_SHIFT) |
			 (cfg->org.dq_nibble_map[5] <<
				DWC_DDR_UMCTL2_REGS_DQMAP1_DQ_NIBBLE_MAP_20_23_SHIFT) |
			 (cfg->org.dq_nibble_map[6] <<
				DWC_DDR_UMCTL2_REGS_DQMAP1_DQ_NIBBLE_MAP_24_27_SHIFT) |
			 (cfg->org.dq_nibble_map[7] <<
				DWC_DDR_UMCTL2_REGS_DQMAP1_DQ_NIBBLE_MAP_28_31_SHIFT)));

		al_reg_write32(
			&ctrl_regs->dqmap2,
			((cfg->org.dq_nibble_map[8] <<
				DWC_DDR_UMCTL2_REGS_DQMAP2_DQ_NIBBLE_MAP_32_35_SHIFT) |
			 (cfg->org.dq_nibble_map[9] <<
				DWC_DDR_UMCTL2_REGS_DQMAP2_DQ_NIBBLE_MAP_36_39_SHIFT) |
			 (cfg->org.dq_nibble_map[10] <<
				DWC_DDR_UMCTL2_REGS_DQMAP2_DQ_NIBBLE_MAP_40_43_SHIFT) |
			 (cfg->org.dq_nibble_map[11] <<
				DWC_DDR_UMCTL2_REGS_DQMAP2_DQ_NIBBLE_MAP_44_47_SHIFT)));

		al_reg_write32(
			&ctrl_regs->dqmap3,
			((cfg->org.dq_nibble_map[12] <<
				DWC_DDR_UMCTL2_REGS_DQMAP3_DQ_NIBBLE_MAP_48_51_SHIFT) |
			 (cfg->org.dq_nibble_map[13] <<
				DWC_DDR_UMCTL2_REGS_DQMAP3_DQ_NIBBLE_MAP_52_55_SHIFT) |
			 (cfg->org.dq_nibble_map[14] <<
				DWC_DDR_UMCTL2_REGS_DQMAP3_DQ_NIBBLE_MAP_56_59_SHIFT) |
			 (cfg->org.dq_nibble_map[15] <<
				DWC_DDR_UMCTL2_REGS_DQMAP3_DQ_NIBBLE_MAP_60_63_SHIFT)));

		al_reg_write32(
			&ctrl_regs->dqmap4,
			((cfg->org.cb_nibble_map[0] <<
				DWC_DDR_UMCTL2_REGS_DQMAP4_DQ_NIBBLE_MAP_CB_0_3_SHIFT) |
			 (cfg->org.cb_nibble_map[1] <<
				DWC_DDR_UMCTL2_REGS_DQMAP4_DQ_NIBBLE_MAP_CB_4_7_SHIFT)));
	}

	al_reg_write32_masked(
		&ctrl_regs->dqmap5,
		DWC_DDR_UMCTL2_REGS_DQMAP5_DIS_DQ_RANK_SWAP,
		(cfg->org.dq_rank_swap_enabled ? 0 :
		 DWC_DDR_UMCTL2_REGS_DQMAP5_DIS_DQ_RANK_SWAP));


	al_reg_write32_masked(
		&ctrl_regs->init0,
		(DWC_DDR_UMCTL2_REGS_INIT0_PRE_CKE_X1024_MASK |
		DWC_DDR_UMCTL2_REGS_INIT0_POST_CKE_X1024_MASK),
		((pre_cke_x1024_ctrl <<
		  DWC_DDR_UMCTL2_REGS_INIT0_PRE_CKE_X1024_SHIFT) |
		(post_cke_x1024_ctrl <<
		 DWC_DDR_UMCTL2_REGS_INIT0_POST_CKE_X1024_SHIFT)));

	al_reg_write32_masked(
		&ctrl_regs->init1,
		DWC_DDR_UMCTL2_REGS_INIT1_DRAM_RSTN_X1024_MASK,
		dram_rstn_x1024_ctrl <<
		DWC_DDR_UMCTL2_REGS_INIT1_DRAM_RSTN_X1024_SHIFT);

	if (cfg->org.ddr_type == AL_DDR_TYPE_DDR3) {
		/**
		* Value to be loaded into the SDRAM Mode register
		*
		* Required value:
		* Bitwise OR of the following:
		* - AL_DDR_JEDEC_DDR3_MR_BC_4_8 or AL_DDR_JEDEC_DDR3_MR_BL_8_FIXED -
		*   depending on 'burstchop' being enabled or not
		* - AL_DDR_JEDEC_DDR3_MR_BT_SEQ or AL_DDR_JEDEC_DDR3_MR_BT_INTRLVD -
		*   depending on 'burst_mode_seq'
		* - AL_DDR_JEDEC_DDR3_MR_CL_* - depending on CL
		* - AL_DDR_JEDEC_DDR3_MR_WR_* - depending on tWR in clock cycles
		* - AL_DDR_JEDEC_DDR3_MR_PD_SLOW_EXIT or
		*   AL_DDR_JEDEC_DDR3_MR_PD_FAST_EXIT - depending on whether the DRAM
		*   DLL is on or off - DRAM DLL should be on by default
		*/

		mr0 = (AL_DDR_JEDEC_DDR3_MR_PD_FAST_EXIT | AL_DDR_JEDEC_DDR3_MR_DR);

		mr0 |= (burstchop) ?
			AL_DDR_JEDEC_DDR3_MR_BC_4_8 : AL_DDR_JEDEC_DDR3_MR_BL_8_FIXED;
		mr0 |= (burst_mode_seq) ?
			AL_DDR_JEDEC_DDR3_MR_BT_SEQ : AL_DDR_JEDEC_DDR3_MR_BT_INTRLVD;

		switch (cfg->tmg.cl) {
		case 5:
			mr0 |= AL_DDR_JEDEC_DDR3_MR_CL_5;
			break;
		case 6:
			mr0 |= AL_DDR_JEDEC_DDR3_MR_CL_6;
			break;
		case 7:
			mr0 |= AL_DDR_JEDEC_DDR3_MR_CL_7;
			break;
		case 8:
			mr0 |= AL_DDR_JEDEC_DDR3_MR_CL_8;
			break;
		case 9:
			mr0 |= AL_DDR_JEDEC_DDR3_MR_CL_9;
			break;
		case 10:
			mr0 |= AL_DDR_JEDEC_DDR3_MR_CL_10;
			break;
		case 11:
			mr0 |= AL_DDR_JEDEC_DDR3_MR_CL_11;
			break;
		case 12:
			mr0 |= AL_DDR_JEDEC_DDR3_MR_CL_12;
			break;
		case 13:
			mr0 |= AL_DDR_JEDEC_DDR3_MR_CL_13;
			break;
		case 14:
			mr0 |= AL_DDR_JEDEC_DDR3_MR_CL_14;
			break;
		default:
			al_err("invld CL (%d)!\n", cfg->tmg.cl);
			return -EINVAL;
		};

		switch (t_wr) {
		case 5:
			mr0 |= AL_DDR_JEDEC_DDR3_MR_WR_5;
			break;
		case 6:
			mr0 |= AL_DDR_JEDEC_DDR3_MR_WR_6;
			break;
		case 7:
			mr0 |= AL_DDR_JEDEC_DDR3_MR_WR_7;
			break;
		case 8:
			mr0 |= AL_DDR_JEDEC_DDR3_MR_WR_8;
			break;
		case 10:
			mr0 |= AL_DDR_JEDEC_DDR3_MR_WR_10;
			break;
		case 12:
			mr0 |= AL_DDR_JEDEC_DDR3_MR_WR_12;
			break;
		case 14:
			mr0 |= AL_DDR_JEDEC_DDR3_MR_WR_14;
			break;
		case 16:
			mr0 |= AL_DDR_JEDEC_DDR3_MR_WR_16;
			break;
		default:
			al_err("invld t_wr (%d)!\n", t_wr);
			return -EINVAL;
		};


		/**
		* Value to be loaded into SDRAM EMR registers.
		*
		* Required value:
		* Bitwise OR of the following:
		* - AL_DDR_JEDEC_DDR3_MR1_DE_EN or AL_DDR_JEDEC_DDR3_MR1_DE_DIS -
		*   depends on whether the DRAM DLL is enabled or not
		* - AL_DDR_JEDEC_DDR3_MR1_DIC_* - depending on the required output
		*   driver impedance control
		* - AL_DDR_JEDEC_DDR3_MR1_RTT_* - depending on the board's on-die
		*   termination
		* - AL_DDR_JEDEC_DDR3_MR1_AL_* - depending on the board's additive
		*   latency (AL)
		*/
		mr1 = AL_DDR_JEDEC_DDR3_MR1_DE_EN;

		switch (cfg->impedance_ctrl.dic) {
		case AL_DDR_DIC_RZQ6:
			mr1 |= AL_DDR_JEDEC_DDR3_MR1_DIC_RZQ6;
			break;
		case AL_DDR_DIC_RZQ7:
			mr1 |= AL_DDR_JEDEC_DDR3_MR1_DIC_RZQ7;
			break;
		default:
			al_err("invld DIC (%d)!\n", cfg->impedance_ctrl.dic);
			return -EINVAL;
		};

		switch (cfg->impedance_ctrl.odt) {
		case AL_DDR_ODT_DIS:
			mr1 |= AL_DDR_JEDEC_DDR3_MR1_RTT_ODT_DIS;
			break;
		case AL_DDR_ODT_RZQ2:
			mr1 |= AL_DDR_JEDEC_DDR3_MR1_RTT_RZQ2;
			break;
		case AL_DDR_ODT_RZQ4:
			mr1 |= AL_DDR_JEDEC_DDR3_MR1_RTT_RZQ4;
			break;
		case AL_DDR_ODT_RZQ6:
			mr1 |= AL_DDR_JEDEC_DDR3_MR1_RTT_RZQ6;
			break;
		case AL_DDR_ODT_RZQ8:
			mr1 |= AL_DDR_JEDEC_DDR3_MR1_RTT_RZQ8;
			break;
		case AL_DDR_ODT_RZQ12:
			mr1 |= AL_DDR_JEDEC_DDR3_MR1_RTT_RZQ12;
			break;
		default:
			al_err("invld ODT (%d)!\n", cfg->impedance_ctrl.odt);
			return -EINVAL;
		};

		switch (cfg->tmg.al) {
		case 0:
			mr1 |= AL_DDR_JEDEC_DDR3_MR1_AL_DIS;
			break;
		case 1:
			mr1 |= AL_DDR_JEDEC_DDR3_MR1_AL_CL_MINUS_1;
			break;
		case 2:
			mr1 |= AL_DDR_JEDEC_DDR3_MR1_AL_CL_MINUS_2;
			break;
		default:
			al_err("invld AL (%d)!\n", cfg->tmg.al);
			return -EINVAL;
		};

		al_reg_write32_masked(
			&ctrl_regs->init3,
			(DWC_DDR_UMCTL2_REGS_INIT3_MR_MASK |
			DWC_DDR_UMCTL2_REGS_INIT3_EMR_MASK),
			((mr0 << DWC_DDR_UMCTL2_REGS_INIT3_MR_SHIFT) |
			(mr1 << DWC_DDR_UMCTL2_REGS_INIT3_EMR_SHIFT)));

		/**
		* Value to be loaded into SDRAM EMR2 registers
		*
		* Bitwise OR of the following:
		* - AL_DDR_JEDEC_DDR3_MR2_CWL_* - depending on the CWL
		* - AL_DDR_JEDEC_DDR3_MR2_RTTWR_* - depending on the board's write
		*   on-die termination
		*/
		mr2 = 0;

		switch (cfg->tmg.cwl) {
		case 5:
			mr2 |= AL_DDR_JEDEC_DDR3_MR2_CWL_5;
			break;
		case 6:
			mr2 |= AL_DDR_JEDEC_DDR3_MR2_CWL_6;
			break;
		case 7:
			mr2 |= AL_DDR_JEDEC_DDR3_MR2_CWL_7;
			break;
		case 8:
			mr2 |= AL_DDR_JEDEC_DDR3_MR2_CWL_8;
			break;
		case 9:
			mr2 |= AL_DDR_JEDEC_DDR3_MR2_CWL_9;
			break;
		case 10:
			mr2 |= AL_DDR_JEDEC_DDR3_MR2_CWL_10;
			break;
		case 11:
			mr2 |= AL_DDR_JEDEC_DDR3_MR2_CWL_11;
			break;
		case 12:
			mr2 |= AL_DDR_JEDEC_DDR3_MR2_CWL_12;
			break;
		default:
			al_err("invld CWL (%d)!\n", cfg->tmg.cwl);
			return -EINVAL;
		};

		switch (cfg->impedance_ctrl.odt_dyn) {
		case AL_DDR_ODT_DYN_DIS:
			mr2 |= AL_DDR_JEDEC_DDR3_MR2_RTTWR_ODT_DIS;
			break;
		case AL_DDR_ODT_DYN_RZQ2:
			mr2 |= AL_DDR_JEDEC_DDR3_MR2_RTTWR_RZQ2;
			break;
		case AL_DDR_ODT_DYN_RZQ4:
			mr2 |= AL_DDR_JEDEC_DDR3_MR2_RTTWR_RZQ4;
			break;
		default:
			al_err("invld ODT_DYN (%d)!\n", cfg->impedance_ctrl.odt_dyn);
			return -EINVAL;
		};

		al_reg_write32_masked(
			&ctrl_regs->init4,
			(DWC_DDR_UMCTL2_REGS_INIT4_EMR2_MASK |
			DWC_DDR_UMCTL2_REGS_INIT4_EMR3_MASK),
			((mr2 << DWC_DDR_UMCTL2_REGS_INIT4_EMR2_SHIFT) |
			(mr3 << DWC_DDR_UMCTL2_REGS_INIT4_EMR3_SHIFT)));


	}else{
		/**
		* Addressing RMN: 5010
		*
		* RMN description:
		* Controller Does not Correctly Control ODT in Certain Settings in DDR4
		* Software flow:
		* Do not allow CL < CWL
		*/
		if (cfg->tmg.cl < cfg->tmg.cwl) {
			al_err("invld CL (%d) smaller than CWL (%d) when using DDR4!\n",
				cfg->tmg.cl, cfg->tmg.cwl);
			return -EINVAL;
		}

		/**
		* Value to be loaded into the SDRAM Mode register 0
		*
		* Required value:
		* Bitwise OR of the following:
		* - AL_DDR_JEDEC_DDR4_MR0_BC_4_8 or AL_DDR_JEDEC_DDR4_MR0_BL_8_FIXED -
		*   depending on 'burstchop' being enabled or not
		* - AL_DDR_JEDEC_DDR4_MR0_BT_SEQ or AL_DDR_JEDEC_DDR4_MR0_BT_INTRLVD -
		*   depending on 'burst_mode_seq'
		* - AL_DDR_JEDEC_DDR4_MR0_CL_* - depending on CL
		* - AL_DDR_JEDEC_DDR4_MR0_WR_* - depending on tWR in clock cycles
		*/

		mr0 = AL_DDR_JEDEC_DDR4_MR0_DR;

		mr0 |= (burstchop) ?
			AL_DDR_JEDEC_DDR4_MR0_BC_4_8 : AL_DDR_JEDEC_DDR4_MR0_BL_8_FIXED;
		mr0 |= (burst_mode_seq) ?
			AL_DDR_JEDEC_DDR4_MR0_BT_SEQ : AL_DDR_JEDEC_DDR4_MR0_BT_INTRLVD;

		switch (cfg->tmg.cl) {
		case 9:
			mr0 |= AL_DDR_JEDEC_DDR4_MR0_CL_9;
			break;
		case 10:
			mr0 |= AL_DDR_JEDEC_DDR4_MR0_CL_10;
			break;
		case 11:
			mr0 |= AL_DDR_JEDEC_DDR4_MR0_CL_11;
			break;
		case 12:
			mr0 |= AL_DDR_JEDEC_DDR4_MR0_CL_12;
			break;
		case 13:
			mr0 |= AL_DDR_JEDEC_DDR4_MR0_CL_13;
			break;
		case 14:
			mr0 |= AL_DDR_JEDEC_DDR4_MR0_CL_14;
			break;
		case 15:
			mr0 |= AL_DDR_JEDEC_DDR4_MR0_CL_15;
			break;
		case 16:
			mr0 |= AL_DDR_JEDEC_DDR4_MR0_CL_16;
			break;
		case 18:
			mr0 |= AL_DDR_JEDEC_DDR4_MR0_CL_18;
			break;
		case 20:
			mr0 |= AL_DDR_JEDEC_DDR4_MR0_CL_20;
			break;
		case 22:
			mr0 |= AL_DDR_JEDEC_DDR4_MR0_CL_22;
			break;
		case 24:
			mr0 |= AL_DDR_JEDEC_DDR4_MR0_CL_24;
			break;
		case 23:
			mr0 |= AL_DDR_JEDEC_DDR4_MR0_CL_23;
			break;
		case 17:
			mr0 |= AL_DDR_JEDEC_DDR4_MR0_CL_17;
			break;
		case 19:
			mr0 |= AL_DDR_JEDEC_DDR4_MR0_CL_19;
			break;
		case 21:
			mr0 |= AL_DDR_JEDEC_DDR4_MR0_CL_21;
			break;
		default:
			al_err("invld CL (%d)!\n", cfg->tmg.cl);
			return -EINVAL;
		};

		switch (t_wr) {
		case 10:
			mr0 |= AL_DDR_JEDEC_DDR4_MR0_WR_10;
			break;
		case 12:
			mr0 |= AL_DDR_JEDEC_DDR4_MR0_WR_12;
			break;
		case 14:
			mr0 |= AL_DDR_JEDEC_DDR4_MR0_WR_14;
			break;
		case 16:
			mr0 |= AL_DDR_JEDEC_DDR4_MR0_WR_16;
			break;
		case 18:
			mr0 |= AL_DDR_JEDEC_DDR4_MR0_WR_18;
			break;
		case 20:
			mr0 |= AL_DDR_JEDEC_DDR4_MR0_WR_20;
			break;
		case 24:
			mr0 |= AL_DDR_JEDEC_DDR4_MR0_WR_24;
			break;
		default:
			al_err("invld t_wr (%d)!\n", t_wr);
			return -EINVAL;
		};

		/**
		* Value to be loaded into SDRAM MR1 register.
		*
		* Required value:
		* Bitwise OR of the following:
		* - AL_DDR_JEDEC_DDR4_MR1_DE_EN or AL_DDR_JEDEC_DDR4_MR1_DE_DIS -
		*   depends on whether the DRAM DLL is enabled or not
		* - AL_DDR_JEDEC_DDR4_MR1_DIC_* - depending on the required output
		*   driver impedance control
		* - AL_DDR_JEDEC_DDR4_MR1_RTT_* - depending on the board's on-die
		*   termination
		* - AL_DDR_JEDEC_DDR4_MR1_AL_* - depending on the board's additive
		*   latency (AL)
		*/
		mr1 = AL_DDR_JEDEC_DDR4_MR1_DE_EN;

		switch (cfg->impedance_ctrl.dic) {
		case AL_DDR_DIC_RZQ7:
			mr1 |= AL_DDR_JEDEC_DDR4_MR1_DIC_RZQ7;
			break;
		case AL_DDR_DIC_RZQ5:
			mr1 |= AL_DDR_JEDEC_DDR4_MR1_DIC_RZQ5;
			break;
		default:
			al_err("invld DIC (%d)!\n", cfg->impedance_ctrl.dic);
			return -EINVAL;
		};

		switch (cfg->impedance_ctrl.odt) {
		case AL_DDR_ODT_DIS:
			mr1 |= AL_DDR_JEDEC_DDR4_MR1_RTT_ODT_DIS;
			break;
		case AL_DDR_ODT_RZQ4:
			mr1 |= AL_DDR_JEDEC_DDR4_MR1_RTT_RZQ4;
			break;
		case AL_DDR_ODT_RZQ2:
			mr1 |= AL_DDR_JEDEC_DDR4_MR1_RTT_RZQ2;
			break;
		case AL_DDR_ODT_RZQ6:
			mr1 |= AL_DDR_JEDEC_DDR4_MR1_RTT_RZQ6;
			break;
		case AL_DDR_ODT_RZQ1:
			mr1 |= AL_DDR_JEDEC_DDR4_MR1_RTT_RZQ1;
			break;
		case AL_DDR_ODT_RZQ5:
			mr1 |= AL_DDR_JEDEC_DDR4_MR1_RTT_RZQ5;
			break;
		case AL_DDR_ODT_RZQ3:
			mr1 |= AL_DDR_JEDEC_DDR4_MR1_RTT_RZQ3;
			break;
		case AL_DDR_ODT_RZQ7:
			mr1 |= AL_DDR_JEDEC_DDR4_MR1_RTT_RZQ7;
			break;
		default:
			al_err("invld ODT (%d)!\n", cfg->impedance_ctrl.odt);
			return -EINVAL;
		};

		switch (cfg->tmg.al) {
		case 0:
			mr1 |= AL_DDR_JEDEC_DDR4_MR1_AL_DIS;
			break;
		case 1:
			mr1 |= AL_DDR_JEDEC_DDR4_MR1_AL_CL_MINUS_1;
			break;
		case 2:
			mr1 |= AL_DDR_JEDEC_DDR4_MR1_AL_CL_MINUS_2;
			break;
		default:
			al_err("invld AL (%d)!\n", cfg->tmg.al);
			return -EINVAL;
		};

		al_reg_write32_masked(
			&ctrl_regs->init3,
			(DWC_DDR_UMCTL2_REGS_INIT3_MR_MASK |
			DWC_DDR_UMCTL2_REGS_INIT3_EMR_MASK),
			((mr0 << DWC_DDR_UMCTL2_REGS_INIT3_MR_SHIFT) |
			(mr1 << DWC_DDR_UMCTL2_REGS_INIT3_EMR_SHIFT)));

		/**
		* Value to be loaded into SDRAM MR2 registers
		*
		* Bitwise OR of the following:
		* - AL_DDR_JEDEC_DDR4_MR2_CWL_* - depending on the CWL
		* - AL_DDR_JEDEC_DDR4_MR2_RTTWR_* - depending on the board's write
		*   on-die termination
		* - AL_DDR_JEDEC_DDR4_MR2_WRITE_CRC_DIS or AL_DDR_JEDEC_DDR4_MR2_WRITE_CRC_EN -
		*   depends on whether the DRAM write CRC is enabled or not
		*/
		mr2 = cfg->misc.crc_enabled ?
			AL_DDR_JEDEC_DDR4_MR2_WRITE_CRC_EN :
			AL_DDR_JEDEC_DDR4_MR2_WRITE_CRC_DIS;

		switch (cfg->tmg.cwl) {
		case 9:
			mr2 |= AL_DDR_JEDEC_DDR4_MR2_CWL_9;
			break;
		case 10:
			mr2 |= AL_DDR_JEDEC_DDR4_MR2_CWL_10;
			break;
		case 11:
			mr2 |= AL_DDR_JEDEC_DDR4_MR2_CWL_11;
			break;
		case 12:
			mr2 |= AL_DDR_JEDEC_DDR4_MR2_CWL_12;
			break;
		case 14:
			mr2 |= AL_DDR_JEDEC_DDR4_MR2_CWL_14;
			break;
		case 16:
			mr2 |= AL_DDR_JEDEC_DDR4_MR2_CWL_16;
			break;
		case 18:
			mr2 |= AL_DDR_JEDEC_DDR4_MR2_CWL_18;
			break;
		default:
			al_err("invld CWL (%d)!\n", cfg->tmg.cwl);
			return -EINVAL;
		};

		switch (cfg->impedance_ctrl.odt_dyn) {
		case AL_DDR_ODT_DYN_DIS:
			mr2 |= AL_DDR_JEDEC_DDR4_MR2_RTTWR_ODT_DIS;
			break;
		case AL_DDR_ODT_DYN_RZQ2:
			mr2 |= AL_DDR_JEDEC_DDR4_MR2_RTTWR_RZQ2;
			break;
		case AL_DDR_ODT_DYN_RZQ1:
			mr2 |= AL_DDR_JEDEC_DDR4_MR2_RTTWR_RZQ1;
			break;
		case AL_DDR_ODT_DYN_HI_Z:
			mr2 |= AL_DDR_JEDEC_DDR4_MR2_RTTWR_HI_Z;
			break;
		default:
			al_err("invld ODT_DYN (%d)!\n", cfg->impedance_ctrl.odt_dyn);
			return -EINVAL;
		};

		/**
		* Value to be loaded into SDRAM MR3 registers
		*
		* Bitwise OR of the following:
		* - AL_DDR_JEDEC_DDR4_MR3_PDA_DIS or AL_DDR_JEDEC_DDR4_MR3_PDA_EN -
		*   depends on whether the DRAM PDA is enabled or not
		* - AL_DDR_JEDEC_DDR4_MR3_CRC_DM_WCL_* - depending on the device speed bin
		*/

		mr3 = AL_DDR_JEDEC_DDR4_MR3_PDA_DIS;	/* TODO : change to param after SNPS add support for this feature */

		switch (t_crc_dm_wcl) {
		case 4:
			mr3 |= AL_DDR_JEDEC_DDR4_MR3_CRC_DM_WCL_4;
			break;
		case 5:
			mr3 |= AL_DDR_JEDEC_DDR4_MR3_CRC_DM_WCL_5;
			break;
		case 6:
			mr3 |= AL_DDR_JEDEC_DDR4_MR3_CRC_DM_WCL_6;
			break;
		default:
			al_err("invld CRC_DM_WCL (%d)!\n", t_crc_dm_wcl);
			return -EINVAL;
		};

		al_reg_write32_masked(
			&ctrl_regs->init4,
			(DWC_DDR_UMCTL2_REGS_INIT4_EMR2_MASK |
			DWC_DDR_UMCTL2_REGS_INIT4_EMR3_MASK),
			((mr2 << DWC_DDR_UMCTL2_REGS_INIT4_EMR2_SHIFT) |
			(mr3 << DWC_DDR_UMCTL2_REGS_INIT4_EMR3_SHIFT)));

		/**
		* Value to be loaded into SDRAM MR4 registers
		*
		* Bitwise OR of the following:
		* - AL_DDR_JEDEC_DDR4_MR4_MPDM_DIS or AL_DDR_JEDEC_DDR4_MR4_MPDM_EN -
		*   depends on whether the DRAM MPDM is enabled or not
		* - AL_DDR_JEDEC_DDR4_MR4_CAL_* - depending on the CAL
		* - AL_DDR_JEDEC_DDR4_MR4_RD_PREAMBLE_* - depending on the board specific read preamble required
		* - AL_DDR_JEDEC_DDR4_MR4_WR_PREAMBLE_* - depending on the board specific write preamble required
		*/

		/* TODO : change to params */
		mr4 = AL_DDR_JEDEC_DDR4_MR4_MPDM_DIS |
			AL_DDR_JEDEC_DDR4_MR4_CAL_DIS |
			AL_DDR_JEDEC_DDR4_MR4_RD_PREAMBLE_1CK |
			AL_DDR_JEDEC_DDR4_MR4_WR_PREAMBLE_1CK;

		/**
		* Value to be loaded into SDRAM MR5 registers
		*
		* Bitwise OR of the following:
		* - AL_DDR_JEDEC_DDR4_MR5_PL_* - depending on the PL
		* - AL_DDR_JEDEC_DDR4_MR5_RTT_PARK_* - depending on the board specific RTT_PARK
		* - AL_DDR_JEDEC_DDR4_MR5_CA_PAR_PERS_DIS or AL_DDR_JEDEC_DDR4_MR5_CA_PAR_PERS_EN -
		*   depends on whether the DRAM CA parity persistent error is enabled or not
		* - AL_DDR_JEDEC_DDR4_MR5_DM_DIS or AL_DDR_JEDEC_DDR4_MR5_DM_EN -
		*   depends on whether the DRAM DM is enabled or not
		* - AL_DDR_JEDEC_DDR4_MR5_WRITE_DBI_DIS or AL_DDR_JEDEC_DDR4_MR5_WRITE_DBI_EN -
		*   depends on whether the DRAM write DBI is enabled or not
		* - AL_DDR_JEDEC_DDR4_MR5_READ_DBI_DIS or AL_DDR_JEDEC_DDR4_MR5_READ_DBI_EN -
		*   depends on whether the DRAM read DBI is enabled or not
		*/

		mr5 = AL_DDR_JEDEC_DDR4_MR5_CA_PAR_PERS_EN;
		mr5 |= dm_enabled ?
			AL_DDR_JEDEC_DDR4_MR5_DM_EN :
			AL_DDR_JEDEC_DDR4_MR5_DM_DIS;
		mr5 |= cfg->misc.write_dbi_enabled ?
			AL_DDR_JEDEC_DDR4_MR5_WRITE_DBI_EN :
			AL_DDR_JEDEC_DDR4_MR5_WRITE_DBI_DIS;
		mr5 |= cfg->misc.read_dbi_enabled ?
			AL_DDR_JEDEC_DDR4_MR5_READ_DBI_EN :
			AL_DDR_JEDEC_DDR4_MR5_READ_DBI_DIS;

		switch (PL) {
		case 0:
			mr5 |= AL_DDR_JEDEC_DDR4_MR5_PL_DIS;
			break;
		case 4:
			mr5 |= AL_DDR_JEDEC_DDR4_MR5_PL_4;
			break;
		case 5:
			mr5 |= AL_DDR_JEDEC_DDR4_MR5_PL_5;
			break;
		case 6:
			mr5 |= AL_DDR_JEDEC_DDR4_MR5_PL_6;
			break;
		default:
			al_err("invld PL (%d)!\n", PL);
			return -EINVAL;
		};

		switch (cfg->impedance_ctrl.rtt_park) {
		case AL_DDR_RTT_PARK_DIS:
			mr5 |= AL_DDR_JEDEC_DDR4_MR5_RTT_PARK_DIS;
			break;
		case AL_DDR_RTT_PARK_RZQ4:
			mr5 |= AL_DDR_JEDEC_DDR4_MR5_RTT_PARK_RZQ4;
			break;
		case AL_DDR_RTT_PARK_RZQ2:
			mr5 |= AL_DDR_JEDEC_DDR4_MR5_RTT_PARK_RZQ2;
			break;
		case AL_DDR_RTT_PARK_RZQ6:
			mr5 |= AL_DDR_JEDEC_DDR4_MR5_RTT_PARK_RZQ6;
			break;
		case AL_DDR_RTT_PARK_RZQ1:
			mr5 |= AL_DDR_JEDEC_DDR4_MR5_RTT_PARK_RZQ1;
			break;
		case AL_DDR_RTT_PARK_RZQ5:
			mr5 |= AL_DDR_JEDEC_DDR4_MR5_RTT_PARK_RZQ5;
			break;
		case AL_DDR_RTT_PARK_RZQ3:
			mr5 |= AL_DDR_JEDEC_DDR4_MR5_RTT_PARK_RZQ3;
			break;
		case AL_DDR_RTT_PARK_RZQ7:
			mr5 |= AL_DDR_JEDEC_DDR4_MR5_RTT_PARK_RZQ7;
			break;
		default:
			al_err("invld RTT_PARK (%d)!\n", cfg->impedance_ctrl.rtt_park);
			return -EINVAL;
		};

		al_reg_write32_masked(
			&ctrl_regs->init6,
			(DWC_DDR_UMCTL2_REGS_INIT6_MR4_MASK |
			DWC_DDR_UMCTL2_REGS_INIT6_MR5_MASK),
			((mr4 << DWC_DDR_UMCTL2_REGS_INIT6_MR4_SHIFT) |
			(mr5 << DWC_DDR_UMCTL2_REGS_INIT6_MR5_SHIFT)));

		/**
		* Value to be loaded into SDRAM MR6 registers
		*
		* Bitwise OR of the following:
		* - AL_DDR_JEDEC_DDR4_MR6_CCDL_* - depending on the CCDL
		*/

		mr6 = (0x0 << AL_DDR_JEDEC_DDR4_MR6_VREFDQ_RANGE_SHIFT) |
			(cfg->impedance_ctrl.vrefdq << AL_DDR_JEDEC_DDR4_MR6_VREFDQ_VAL_SHIFT);

		switch (t_ccd) {
		case 4:
			mr6 |= AL_DDR_JEDEC_DDR4_MR6_CCDL_4;
			break;
		case 5:
			mr6 |= AL_DDR_JEDEC_DDR4_MR6_CCDL_5;
			break;
		case 6:
			mr6 |= AL_DDR_JEDEC_DDR4_MR6_CCDL_6;
			break;
		case 7:
			mr6 |= AL_DDR_JEDEC_DDR4_MR6_CCDL_7;
			break;
		case 8:
			mr6 |= AL_DDR_JEDEC_DDR4_MR6_CCDL_8;
			break;
		default:
			al_err("invld CCD_L (%d)!\n", t_ccd);
			return -EINVAL;
		};

		al_reg_write32_masked(
			&ctrl_regs->init7,
			DWC_DDR_UMCTL2_REGS_INIT7_MR6_MASK,
			(mr6 << DWC_DDR_UMCTL2_REGS_INIT7_MR6_SHIFT));

	}



	cfg->calc.mr0 = mr0;
	cfg->calc.mr1 = mr1;
	cfg->calc.mr2 = mr2;
	cfg->calc.mr3 = mr3;
	cfg->calc.mr4 = mr4;
	cfg->calc.mr5 = mr5;
	cfg->calc.mr6 = mr6;

	al_reg_write32_masked(
		&ctrl_regs->init5,
		DWC_DDR_UMCTL2_REGS_INIT5_DEV_ZQINIT_X32_MASK,
		(dev_zqinit_x32 <<
		 DWC_DDR_UMCTL2_REGS_INIT5_DEV_ZQINIT_X32_SHIFT));

	if (cfg->org.udimm_addr_mirroring)
		al_reg_write32_masked(
			&ctrl_regs->dimmctl,
			(DWC_DDR_UMCTL2_REGS_DIMMCTL_DIMM_ADDR_MIRR_EN |
			DWC_DDR_UMCTL2_REGS_DIMMCTL_DIMM_STAGGER_CS_EN),
			(DWC_DDR_UMCTL2_REGS_DIMMCTL_DIMM_ADDR_MIRR_EN |
			DWC_DDR_UMCTL2_REGS_DIMMCTL_DIMM_STAGGER_CS_EN));

	if (cfg->org.rdimm)
		al_reg_write32_masked(
			&ctrl_regs->dimmctl,
			DWC_DDR_UMCTL2_REGS_DIMMCTL_DIMM_STAGGER_CS_EN,
			DWC_DDR_UMCTL2_REGS_DIMMCTL_DIMM_STAGGER_CS_EN);

	if (cfg->org.rdimm && (cfg->org.ddr_type == AL_DDR_TYPE_DDR4))
		/*
		* Note : if A17 doesn't exist in device - must not use it in parity calculations
		*	 Currently using addrmap.row_b0_17[17] to decide if A17 exist,
		*	 this might not be correct in all cases.
		*/
		al_reg_write32_masked(
			&ctrl_regs->dimmctl,
			DWC_DDR_UMCTL2_REGS_DIMMCTL_DIMM_OUTPUT_INV_EN |
			DWC_DDR_UMCTL2_REGS_DIMMCTL_MRS_A17_EN |
			DWC_DDR_UMCTL2_REGS_DIMMCTL_MRS_BG1_EN,
			DWC_DDR_UMCTL2_REGS_DIMMCTL_DIMM_OUTPUT_INV_EN |
			((cfg->addrmap.row_b0_17[17] == AL_DDR_ADDRMAP_NC) ? 0
				: DWC_DDR_UMCTL2_REGS_DIMMCTL_MRS_A17_EN) |
			((cfg->org.ddr_device == AL_DDR_DEVICE_X16) ? 0
				: DWC_DDR_UMCTL2_REGS_DIMMCTL_MRS_BG1_EN));

	if ((cfg->org.ddr_device == AL_DDR_DEVICE_X16) &&
		(cfg->org.ddr_type == AL_DDR_TYPE_DDR4))
		al_reg_write32_masked(
			&ctrl_regs->dimmctl,
			DWC_DDR_UMCTL2_REGS_DIMMCTL_DIMM_DIS_BG_MIRRORING,
			DWC_DDR_UMCTL2_REGS_DIMMCTL_DIMM_DIS_BG_MIRRORING);

	al_reg_write32_masked(
		&ctrl_regs->rankctl,
		(DWC_DDR_UMCTL2_REGS_RANKCTL_DIFF_RANK_RD_GAP_MASK |
		DWC_DDR_UMCTL2_REGS_RANKCTL_DIFF_RANK_WR_GAP_MASK),
		((diff_rank_rd_gap_ctrl <<
		  DWC_DDR_UMCTL2_REGS_RANKCTL_DIFF_RANK_RD_GAP_SHIFT) |
		(diff_rank_wr_gap_ctrl <<
		 DWC_DDR_UMCTL2_REGS_RANKCTL_DIFF_RANK_WR_GAP_SHIFT)));

	al_reg_write32_masked(
		&ctrl_regs->dramtmg0,
		(DWC_DDR_UMCTL2_REGS_DRAMTMG0_WR2PRE_MASK |
		DWC_DDR_UMCTL2_REGS_DRAMTMG0_T_FAW_MASK |
		DWC_DDR_UMCTL2_REGS_DRAMTMG0_T_RAS_MIN_MASK |
		DWC_DDR_UMCTL2_REGS_DRAMTMG0_T_RAS_MAX_MASK),
		((wr2pre_ctrl <<
		  DWC_DDR_UMCTL2_REGS_DRAMTMG0_WR2PRE_SHIFT) |
		(t_faw_ctrl <<
		 DWC_DDR_UMCTL2_REGS_DRAMTMG0_T_FAW_SHIFT) |
		(t_ras_min_ctrl <<
		 DWC_DDR_UMCTL2_REGS_DRAMTMG0_T_RAS_MIN_SHIFT) |
		(t_ras_max_ctrl <<
		 DWC_DDR_UMCTL2_REGS_DRAMTMG0_T_RAS_MAX_SHIFT)));

	al_reg_write32_masked(
		&ctrl_regs->dramtmg1,
		(DWC_DDR_UMCTL2_REGS_DRAMTMG1_T_XP_MASK |
		DWC_DDR_UMCTL2_REGS_DRAMTMG1_RD2PRE_MASK |
		DWC_DDR_UMCTL2_REGS_DRAMTMG1_T_RC_MASK),
		((t_xp_ctrl <<
		  DWC_DDR_UMCTL2_REGS_DRAMTMG1_T_XP_SHIFT) |
		(rd2pre_ctrl <<
		 DWC_DDR_UMCTL2_REGS_DRAMTMG1_RD2PRE_SHIFT) |
		(t_rc_ctrl <<
		 DWC_DDR_UMCTL2_REGS_DRAMTMG1_T_RC_SHIFT)));

	al_reg_write32_masked(
		&ctrl_regs->dramtmg2,
		(DWC_DDR_UMCTL2_REGS_DRAMTMG2_RD2WR_MASK |
		DWC_DDR_UMCTL2_REGS_DRAMTMG2_WRITE_LATENCY_MASK |
		DWC_DDR_UMCTL2_REGS_DRAMTMG2_READ_LATENCY_MASK |
		DWC_DDR_UMCTL2_REGS_DRAMTMG2_WR2RD_MASK),
		((rd2wr_ctrl <<
		  DWC_DDR_UMCTL2_REGS_DRAMTMG2_RD2WR_SHIFT) |
		(write_latency_ctrl <<
		 DWC_DDR_UMCTL2_REGS_DRAMTMG2_WRITE_LATENCY_SHIFT) |
		(read_latency_ctrl <<
		 DWC_DDR_UMCTL2_REGS_DRAMTMG2_READ_LATENCY_SHIFT) |
		(wr2rd_ctrl <<
		 DWC_DDR_UMCTL2_REGS_DRAMTMG2_WR2RD_SHIFT)));

	al_reg_write32_masked(
		&ctrl_regs->dramtmg3,
		(DWC_DDR_UMCTL2_REGS_DRAMTMG3_T_MRD_MASK |
		DWC_DDR_UMCTL2_REGS_DRAMTMG3_T_MOD_MASK),
		((t_mrd_ctrl <<
		  DWC_DDR_UMCTL2_REGS_DRAMTMG3_T_MRD_SHIFT) |
		(t_mod_ctrl <<
		 DWC_DDR_UMCTL2_REGS_DRAMTMG3_T_MOD_SHIFT)));

	al_reg_write32_masked(
		&ctrl_regs->dramtmg4,
		(DWC_DDR_UMCTL2_REGS_DRAMTMG4_T_RCD_MASK |
		DWC_DDR_UMCTL2_REGS_DRAMTMG4_T_CCD_MASK |
		DWC_DDR_UMCTL2_REGS_DRAMTMG4_T_RRD_MASK |
		DWC_DDR_UMCTL2_REGS_DRAMTMG4_T_RP_MASK),
		((t_rcd_ctrl <<
		  DWC_DDR_UMCTL2_REGS_DRAMTMG4_T_RCD_SHIFT) |
		(t_ccd_ctrl <<
		 DWC_DDR_UMCTL2_REGS_DRAMTMG4_T_CCD_SHIFT) |
		(t_rrd_ctrl <<
		 DWC_DDR_UMCTL2_REGS_DRAMTMG4_T_RRD_SHIFT) |
		(t_rp_ctrl <<
		 DWC_DDR_UMCTL2_REGS_DRAMTMG4_T_RP_SHIFT)));

	al_reg_write32_masked(
		&ctrl_regs->dramtmg5,
		(DWC_DDR_UMCTL2_REGS_DRAMTMG5_T_CKSRX_MASK |
		DWC_DDR_UMCTL2_REGS_DRAMTMG5_T_CKSRE_MASK |
		DWC_DDR_UMCTL2_REGS_DRAMTMG5_T_CKESR_MASK |
		DWC_DDR_UMCTL2_REGS_DRAMTMG5_T_CKE_MASK),
		((t_cksrx_ctrl << DWC_DDR_UMCTL2_REGS_DRAMTMG5_T_CKSRX_SHIFT) |
		((t_cksre_ctrl) <<
		 DWC_DDR_UMCTL2_REGS_DRAMTMG5_T_CKSRE_SHIFT) |
		(t_ckesr_ctrl << DWC_DDR_UMCTL2_REGS_DRAMTMG5_T_CKESR_SHIFT) |
		(t_cke_ctrl << DWC_DDR_UMCTL2_REGS_DRAMTMG5_T_CKE_SHIFT)));

	al_reg_write32_masked(
		&ctrl_regs->dramtmg8,
		(DWC_DDR_UMCTL2_REGS_DRAMTMG8_T_XS_X32_MASK |
		 DWC_DDR_UMCTL2_REGS_DRAMTMG8_T_XS_DLL_X32_MASK |
		 DWC_DDR_UMCTL2_REGS_DRAMTMG8_T_XS_ABORT_X32_MASK |
		 DWC_DDR_UMCTL2_REGS_DRAMTMG8_T_XS_FAST_X32_MASK),
		((t_xs_x32_ctrl << DWC_DDR_UMCTL2_REGS_DRAMTMG8_T_XS_X32_SHIFT) |
		 (t_xs_dll_x32_ctrl << DWC_DDR_UMCTL2_REGS_DRAMTMG8_T_XS_DLL_X32_SHIFT) |
		 (t_xs_abort_x32_ctrl << DWC_DDR_UMCTL2_REGS_DRAMTMG8_T_XS_ABORT_X32_SHIFT) |
		 (t_xs_fast_x32_ctrl << DWC_DDR_UMCTL2_REGS_DRAMTMG8_T_XS_FAST_X32_SHIFT)));

	if (cfg->org.ddr_type == AL_DDR_TYPE_DDR4)
		al_reg_write32_masked(
			&ctrl_regs->dramtmg9,
			(DWC_DDR_UMCTL2_REGS_DRAMTMG9_WR2RD_S_MASK |
			DWC_DDR_UMCTL2_REGS_DRAMTMG9_T_RRD_S_MASK |
			DWC_DDR_UMCTL2_REGS_DRAMTMG9_T_CCD_S_MASK),
			((wr2rd_s_ctrl <<
			DWC_DDR_UMCTL2_REGS_DRAMTMG9_WR2RD_S_SHIFT) |
			(t_rrd_s_ctrl <<
			DWC_DDR_UMCTL2_REGS_DRAMTMG9_T_RRD_S_SHIFT) |
			(t_ccd_s_ctrl <<
			DWC_DDR_UMCTL2_REGS_DRAMTMG9_T_CCD_S_SHIFT)));

	/* TODO : set register DRAMTMG 10-12 */

	al_reg_write32_masked(
		&ctrl_regs->zqctl0,
		(DWC_DDR_UMCTL2_REGS_ZQCTL0_T_ZQ_LONG_NOP_MASK |
		 DWC_DDR_UMCTL2_REGS_ZQCTL0_T_ZQ_SHORT_NOP_MASK),
		(t_zq_long_nop_ctrl <<
		 DWC_DDR_UMCTL2_REGS_ZQCTL0_T_ZQ_LONG_NOP_SHIFT) |
		(t_zq_short_nop_ctrl <<
		 DWC_DDR_UMCTL2_REGS_ZQCTL0_T_ZQ_SHORT_NOP_SHIFT));

	al_reg_write32_masked(
		&ctrl_regs->dfitmg0,
		(DWC_DDR_UMCTL2_REGS_DFITMG0_DFI_T_CTRL_DELAY_MASK |
		DWC_DDR_UMCTL2_REGS_DFITMG0_DFI_T_RDDATA_EN_MASK |
		DWC_DDR_UMCTL2_REGS_DFITMG0_DFI_TPHY_WRDATA_MASK |
		DWC_DDR_UMCTL2_REGS_DFITMG0_DFI_WRDATA_USE_SDR |
		DWC_DDR_UMCTL2_REGS_DFITMG0_DFI_RDDATA_USE_SDR |
		DWC_DDR_UMCTL2_REGS_DFITMG0_DFI_TPHY_WRLAT_MASK),
		((dfi_t_ctrl_delay + rr_mode + cfg->org.rdimm) <<
		 DWC_DDR_UMCTL2_REGS_DFITMG0_DFI_T_CTRL_DELAY_SHIFT) |
		((dfi_t_rddata_en) <<
		 DWC_DDR_UMCTL2_REGS_DFITMG0_DFI_T_RDDATA_EN_SHIFT) |
		(dfi_tphy_wrdata <<
		 DWC_DDR_UMCTL2_REGS_DFITMG0_DFI_TPHY_WRDATA_SHIFT) |
		(ddr_ctrl_sdr_mode ?
		 0: DWC_DDR_UMCTL2_REGS_DFITMG0_DFI_WRDATA_USE_SDR) |
		(ddr_ctrl_sdr_mode ?
		 0: DWC_DDR_UMCTL2_REGS_DFITMG0_DFI_RDDATA_USE_SDR) |
		((dfi_tphy_wrlat) <<
		 DWC_DDR_UMCTL2_REGS_DFITMG0_DFI_TPHY_WRLAT_SHIFT));

	al_reg_write32_masked(
		&ctrl_regs->dfitmg1,
		(DWC_DDR_UMCTL2_REGS_DFITMG1_DFI_T_DRAM_CLK_DISABLE_MASK |
		DWC_DDR_UMCTL2_REGS_DFITMG1_DFI_T_DRAM_CLK_ENABLE_MASK),
		((dfi_t_dram_clk_disable + rr_mode + cfg->org.rdimm) <<
		 DWC_DDR_UMCTL2_REGS_DFITMG1_DFI_T_DRAM_CLK_DISABLE_SHIFT) |
		((dfi_t_dram_clk_enable + rr_mode + cfg->org.rdimm) <<
		 DWC_DDR_UMCTL2_REGS_DFITMG1_DFI_T_DRAM_CLK_ENABLE_SHIFT));

	al_reg_write32_masked(
		&ctrl_regs->dfiupd0,
		DWC_DDR_UMCTL2_REGS_DFIUPD0_DIS_AUTO_CTRLUPD,
		dis_auto_ctrlupd ?
		DWC_DDR_UMCTL2_REGS_DFIUPD0_DIS_AUTO_CTRLUPD :
		0);

	al_reg_write32_masked(
		&ctrl_regs->dfiupd2,
		(DWC_DDR_UMCTL2_REGS_DFIUPD2_DFI_PHYUPD_EN |
		DWC_DDR_UMCTL2_REGS_DFIUPD2_DFI_PHYUPD_TYPE1_MASK |
		DWC_DDR_UMCTL2_REGS_DFIUPD2_DFI_PHYUPD_TYPE0_MASK),
		((dfi_phyupd_en ?
		  DWC_DDR_UMCTL2_REGS_DFIUPD2_DFI_PHYUPD_EN : 0) |
		(dfi_phyupd_type1 <<
		 DWC_DDR_UMCTL2_REGS_DFIUPD2_DFI_PHYUPD_TYPE1_SHIFT) |
		(dfi_phyupd_type0 <<
		 DWC_DDR_UMCTL2_REGS_DFIUPD2_DFI_PHYUPD_TYPE0_SHIFT)));

	reg_val = 0;
	if (dm_enabled)
		reg_val |= DWC_DDR_UMCTL2_REGS_DBICTL_DM_EN;
	if (cfg->misc.read_dbi_enabled)
		reg_val |= DWC_DDR_UMCTL2_REGS_DBICTL_RD_DBI_EN;
	if (cfg->misc.write_dbi_enabled)
		reg_val |= DWC_DDR_UMCTL2_REGS_DBICTL_WR_DBI_EN;

	al_reg_write32(
		&ctrl_regs->dbictl,
		reg_val);

	al_ddr_ctrl_cfg_addr_map(cfg);

	al_reg_write32_masked(
		&ctrl_regs->odtcfg,
		DWC_DDR_UMCTL2_REGS_ODTCFG_RD_ODT_DELAY_MASK |
		DWC_DDR_UMCTL2_REGS_ODTCFG_RD_ODT_HOLD_MASK |
		DWC_DDR_UMCTL2_REGS_ODTCFG_WR_ODT_HOLD_MASK,
		((rd_odt_delay << DWC_DDR_UMCTL2_REGS_ODTCFG_RD_ODT_DELAY_SHIFT) |
		(rd_odt_hold << DWC_DDR_UMCTL2_REGS_ODTCFG_RD_ODT_HOLD_SHIFT) |
		(wr_odt_hold << DWC_DDR_UMCTL2_REGS_ODTCFG_WR_ODT_HOLD_SHIFT)));

	al_reg_write32(&ctrl_regs->odtmap,
			((cfg->impedance_ctrl.wr_odt_map[0] <<
			  DWC_DDR_UMCTL2_REGS_ODTMAP_RANK0_WR_ODT_SHIFT) |
			 (cfg->impedance_ctrl.rd_odt_map[0] <<
			  DWC_DDR_UMCTL2_REGS_ODTMAP_RANK0_RD_ODT_SHIFT) |
			 (cfg->impedance_ctrl.wr_odt_map[1] <<
			  DWC_DDR_UMCTL2_REGS_ODTMAP_RANK1_WR_ODT_SHIFT) |
			 (cfg->impedance_ctrl.rd_odt_map[1] <<
			  DWC_DDR_UMCTL2_REGS_ODTMAP_RANK1_RD_ODT_SHIFT) |
			 (cfg->impedance_ctrl.wr_odt_map[2] <<
			  DWC_DDR_UMCTL2_REGS_ODTMAP_RANK2_WR_ODT_SHIFT) |
			 (cfg->impedance_ctrl.rd_odt_map[2] <<
			  DWC_DDR_UMCTL2_REGS_ODTMAP_RANK2_RD_ODT_SHIFT) |
			 (cfg->impedance_ctrl.wr_odt_map[3] <<
			  DWC_DDR_UMCTL2_REGS_ODTMAP_RANK3_WR_ODT_SHIFT) |
			 (cfg->impedance_ctrl.rd_odt_map[3] <<
			  DWC_DDR_UMCTL2_REGS_ODTMAP_RANK3_RD_ODT_SHIFT)));

	if (cfg->perf) {
		al_reg_write32_masked(
			&nb_regs->global.ddr_qos,
			NB_GLOBAL_DDR_QOS_HIGH_PRIO_THRESHOLD_MASK,
			((REORDER_QUEUE_SIZE - cfg->perf->lpr_num_entries) <<
			 NB_GLOBAL_DDR_QOS_HIGH_PRIO_THRESHOLD_SHIFT));
		al_reg_write32_masked(
			&ctrl_regs->sched,
			(DWC_DDR_UMCTL2_REGS_SCHED_RDWR_IDLE_GAP_MASK |
			DWC_DDR_UMCTL2_REGS_SCHED_LPR_NUM_ENTRIES_MASK |
			DWC_DDR_UMCTL2_REGS_SCHED_PAGECLOSE_RD |
			DWC_DDR_UMCTL2_REGS_SCHED_PREFER_WRITE),
			((cfg->perf->rdwr_idle_gap <<
			  DWC_DDR_UMCTL2_REGS_SCHED_RDWR_IDLE_GAP_SHIFT) |
			 (cfg->perf->lpr_num_entries <<
			DWC_DDR_UMCTL2_REGS_SCHED_LPR_NUM_ENTRIES_SHIFT) |
			(cfg->perf->pageclose ?
			 DWC_DDR_UMCTL2_REGS_SCHED_PAGECLOSE_RD : 0)));

		al_reg_write32_masked(
			&ctrl_regs->sched1,
			DWC_DDR_UMCTL2_REGS_SCHED1_PAGECLOSE_RD_TIMER_MASK,
			(cfg->perf->pageclose_timer <<
			 DWC_DDR_UMCTL2_REGS_SCHED1_PAGECLOSE_RD_TIMER_SHIFT));


		al_reg_write32_masked(
			&ctrl_regs->perfhpr1,
			(DWC_DDR_UMCTL2_REGS_PERFHPR1_HPR_XACT_RUN_LENGTH_MASK |
			DWC_DDR_UMCTL2_REGS_PERFHPR1_HPR_MAX_STARVE_MASK),
			((cfg->perf->hpr_xact_run_length <<
			DWC_DDR_UMCTL2_REGS_PERFHPR1_HPR_XACT_RUN_LENGTH_SHIFT)
			 |(cfg->perf->hpr_max_starve <<
			 DWC_DDR_UMCTL2_REGS_PERFHPR1_HPR_MAX_STARVE_SHIFT)));


		al_reg_write32_masked(
			&ctrl_regs->perflpr1,
			(DWC_DDR_UMCTL2_REGS_PERFLPR1_LPR_XACT_RUN_LENGTH_MASK |
			DWC_DDR_UMCTL2_REGS_PERFLPR1_LPR_MAX_STARVE_MASK),
			((cfg->perf->lpr_xact_run_length <<
			DWC_DDR_UMCTL2_REGS_PERFLPR1_LPR_XACT_RUN_LENGTH_SHIFT)
			 |(cfg->perf->lpr_max_starve <<
			 DWC_DDR_UMCTL2_REGS_PERFLPR1_LPR_MAX_STARVE_SHIFT)));


		al_reg_write32_masked(
			&ctrl_regs->perfwr1,
			(DWC_DDR_UMCTL2_REGS_PERFWR1_W_XACT_RUN_LENGTH_MASK |
			DWC_DDR_UMCTL2_REGS_PERFWR1_W_MAX_STARVE_MASK),
			((cfg->perf->w_xact_run_length <<
			  DWC_DDR_UMCTL2_REGS_PERFWR1_W_XACT_RUN_LENGTH_SHIFT) |
			(cfg->perf->w_max_starve <<
			 DWC_DDR_UMCTL2_REGS_PERFWR1_W_MAX_STARVE_SHIFT)));
	}


	if (cfg->misc.ocpar_enabled) {
		al_reg_write32_masked(
			&ctrl_regs->ocparcfg0,
			DWC_DDR_UMCTL2_REGS_OCPARCFG0_OC_PARITY_EN |
			DWC_DDR_UMCTL2_REGS_OCPARCFG0_OC_PARITY_TYPE |
			DWC_DDR_UMCTL2_REGS_OCPARCFG0_PAR_WDATA_SLVERR_EN |
			DWC_DDR_UMCTL2_REGS_OCPARCFG0_ADDRESS_PARITY_MODE,
		DWC_DDR_UMCTL2_REGS_OCPARCFG0_OC_PARITY_EN);
	}

	/**
	 * Addressing RMN: 5193
	 *
	 * RMN description:
	 * Performance degradation when using DDR4 in half-bus (32 bit) mode
	 * Software flow:
	 * Set PCCFG.bl_exp_mode=1 and disable read_reorder_bypass_en
	 */
	/**
	 * Addressing RMN: 5301
	 *
	 * RMN description:
	 * Performance Issue for Writes if PCCFG.bl_exp_mode=1 Because of Same addr Collisions
	 * Software flow:
	 * Set DBG0.dis_wc=0
	 */
	if ((cfg->org.ddr_type == AL_DDR_TYPE_DDR4) &&
		(cfg->org.data_width == AL_DDR_DATA_WIDTH_32_BITS)) {
		/**
		* Addressing RMN: 5557
		*
		* RMN description:
		* DDR controller sends X's on DFI wrdata when bl_exp_mode=1 and write DBI enabled
		* Software flow:
		* Do not allow running with data mask disabled when bl_exp_mode=1
		*/
		if (dm_enabled == 0) {
			al_err("Data mask disabled is not allowed with DDR4 32bit!\n");
			return -EINVAL;
		}

		al_reg_write32_masked(
			&ctrl_mp_regs->pccfg,
			DWC_DDR_UMCTL2_MP_PCCFG_BL_EXP_MODE,
			DWC_DDR_UMCTL2_MP_PCCFG_BL_EXP_MODE);
		al_reg_write32_masked(
			&ctrl_regs->dbg0,
			DWC_DDR_UMCTL2_REGS_DBG0_DIS_WC,
			0);
	}

	al_reg_write32_masked(
		&ctrl_mp_regs->pcfgr_0,
		DWC_DDR_UMCTL2_MP_PCFGR_0_READ_REORDER_BYPASS_EN,
		read_reorder_bypass_en ?
		DWC_DDR_UMCTL2_MP_PCFGR_0_READ_REORDER_BYPASS_EN : 0);

	al_reg_write32_masked(
		&ctrl_mp_regs->pcfgidmaskch0_0,
		DWC_DDR_UMCTL2_MP_PCFGIDMASKCH0_0_ID_MASK_MASK,
		id_mask << DWC_DDR_UMCTL2_MP_PCFGIDMASKCH0_0_ID_MASK_SHIFT);

	al_reg_write32_masked(
		&ctrl_mp_regs->pcfgidvaluech0_0,
		DWC_DDR_UMCTL2_MP_PCFGIDVALUECH0_0_ID_VALUE_MASK,
		id_value << DWC_DDR_UMCTL2_MP_PCFGIDVALUECH0_0_ID_VALUE_SHIFT);

	/*
	* Addressing RMN: 10626, 6779
	*
	* RMN description:
	* DDR Controller read reorder queue bypass feature is not working for CPU transactions.
	* The bypass channel is selected using a mask on the ARID. Currently ARID[1] indicated
	* whether the transaction from CPU is unaligned wrap.
	*
	* Software flow:
	* Using this bit and only allowing aligned transactions to enter bypass channel
	*/

	/* Allow also aligned CPU L2 transactions to go to bypass channel */

	al_reg_write32(
		&nb_regs->dram_channels.reorder_id_mask_0,
		(id_mask_0 << NB_DRAM_CHANNELS_REORDER_ID_MASK_MASK_SHIFT));
	al_reg_write32(
		&nb_regs->dram_channels.reorder_id_value_0,
		(id_value_0 << NB_DRAM_CHANNELS_REORDER_ID_VALUE_VALUE_SHIFT));

	al_reg_write32(
		&nb_regs->dram_channels.reorder_id_mask_1,
		(id_mask_1 << NB_DRAM_CHANNELS_REORDER_ID_MASK_MASK_SHIFT));
	al_reg_write32(
		&nb_regs->dram_channels.reorder_id_value_1,
		(id_value_1 << NB_DRAM_CHANNELS_REORDER_ID_VALUE_VALUE_SHIFT));

	al_reg_write32(
		&ctrl_mp_regs->pcfgqos0_0,
		DWC_DDR_UMCTL2_MP_PCFGQOS0_0_RQOS_MAP_REGION1_HPR |
		DWC_DDR_UMCTL2_MP_PCFGQOS0_0_RQOS_MAP_REGION0_LPR |
		(rqos_map_level1 << DWC_DDR_UMCTL2_MP_PCFGQOS0_0_RQOS_MAP_LEVEL1_SHIFT));

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
static inline int al_ddr_ctrl_cfg_addr_map(
	struct al_ddr_init_cfg	*cfg)
{
	STRUCT_CTRL_REGS __iomem *ctrl_regs = CTRL_REGS(cfg->ddr_cfg.ddr_ctrl_regs_base);

	struct al_ddr_addrmap *addrmap =
		&cfg->addrmap;

	int addrmap_col_b2_11[10];
	int addrmap_bank_b0_2[3];
	int addrmap_row_b0_17[18];
	int addrmap_cs_b0_1[2];
	int addrmap_bg_b0_1[2];
	struct al_nb_regs __iomem *nb_regs =
		(struct al_nb_regs __iomem *)cfg->ddr_cfg.nb_regs_base;

	unsigned int i;
	unsigned int addrmap_offset =
			AL_DDR_ADDR_MAP_OFFSET_V1_V2;
	unsigned int col_offset =
			1;

	/* Column */
	for (i = 0; i < (AL_ARR_SIZE(addrmap->col_b3_9_b11_13) - col_offset); i++) {
		int user_val;

		if (cfg->org.data_width == AL_DDR_DATA_WIDTH_64_BITS)
			user_val = ((i == 0) && (col_offset == 0)) ?
					(addrmap_offset + AL_DDR_ADDR_MAP_COL_2_BASE) :
					addrmap->col_b3_9_b11_13[i + col_offset - 1];
		else if (cfg->org.data_width == AL_DDR_DATA_WIDTH_32_BITS)
			user_val = addrmap->col_b3_9_b11_13[i + col_offset];
		else
			user_val = (i >= (AL_ARR_SIZE(addrmap->col_b3_9_b11_13) - 2)) ?
					AL_DDR_ADDRMAP_NC :
					addrmap->col_b3_9_b11_13[i + 1 + col_offset];

		addrmap_col_b2_11[i] =
			(user_val == AL_DDR_ADDRMAP_NC) ?
			AL_DDR_ADDR_MAP_COL_DISABLED :
			(user_val - addrmap_offset -
			 AL_DDR_ADDR_MAP_COL_2_BASE - i);

		al_assert(addrmap_col_b2_11[i] >= 0);
	}

	  addrmap_col_b2_11[i] = AL_DDR_ADDR_MAP_COL_DISABLED;

	/* Bank */
	for (i = 0; i < AL_ARR_SIZE(addrmap->bank_b0_2); i++) {
		int user_val = addrmap->bank_b0_2[i];

		addrmap_bank_b0_2[i] =
			(user_val == AL_DDR_ADDRMAP_NC) ?
			AL_DDR_ADDR_MAP_BANK_DISABLED_ALPINE_V2 :
			(user_val - addrmap_offset -
			 AL_DDR_ADDR_MAP_BANK_0_BASE - i);

		al_assert(addrmap_bank_b0_2[i]  >= 0);
	}

	/* Bank Group */
	for (i = 0; i < AL_ARR_SIZE(addrmap->bg_b0_1); i++) {
		int user_val = addrmap->bg_b0_1[i];

		addrmap_bg_b0_1[i] =
			(user_val == AL_DDR_ADDRMAP_NC) ?
			AL_DDR_ADDR_MAP_BG_DISABLED :
			(user_val - addrmap_offset -
			 AL_DDR_ADDR_MAP_BG_0_BASE - i);

		al_assert(addrmap_bg_b0_1[i]  >= 0);
	}

	/* CS */
	for (i = 0; i < AL_ARR_SIZE(addrmap->cs_b0_1); i++) {
		int user_val = addrmap->cs_b0_1[i];

		addrmap_cs_b0_1[i] =
			(user_val == AL_DDR_ADDRMAP_NC) ?
			AL_DDR_ADDR_MAP_CS_DISABLED :
			(user_val - addrmap_offset -
			 AL_DDR_ADDR_MAP_CS_0_BASE - i);

		al_assert(addrmap_cs_b0_1[i] >= 0);
	}

	/* Row */
	for (i = 0; i < AL_ARR_SIZE(addrmap->row_b0_17); i++) {
		int user_val = addrmap->row_b0_17[i];

		addrmap_row_b0_17[i] =
			(user_val == AL_DDR_ADDRMAP_NC) ?
			AL_DDR_ADDR_MAP_ROW_DISABLED :
			(user_val - addrmap_offset -
			 AL_DDR_ADDR_MAP_ROW_0_BASE - i);

		al_assert(addrmap_row_b0_17[i] >= 0);
	}
	for (i = 7; i <= 10; i++)
		al_assert(addrmap_row_b0_17[i] == addrmap_row_b0_17[6]);

	/**
	 * CS address mapping
	 */
	al_reg_write32_masked(
		&ctrl_regs->addrmap0,
		(DWC_DDR_UMCTL2_REGS_ADDRMAP0_ADDRMAP_CS_BIT1_MASK |
		DWC_DDR_UMCTL2_REGS_ADDRMAP0_ADDRMAP_CS_BIT0_MASK),
		((addrmap_cs_b0_1[1] <<
		  DWC_DDR_UMCTL2_REGS_ADDRMAP0_ADDRMAP_CS_BIT1_SHIFT) |
		(addrmap_cs_b0_1[0] <<
		 DWC_DDR_UMCTL2_REGS_ADDRMAP0_ADDRMAP_CS_BIT0_SHIFT)));

	/**
	 * Bank address mapping
	 */
	al_reg_write32_masked(
		&ctrl_regs->addrmap1,
		(DWC_DDR_UMCTL2_REGS_ADDRMAP1_ADDRMAP_BANK_B2_MASK |
		DWC_DDR_UMCTL2_REGS_ADDRMAP1_ADDRMAP_BANK_B1_MASK |
		DWC_DDR_UMCTL2_REGS_ADDRMAP1_ADDRMAP_BANK_B0_MASK),
		((addrmap_bank_b0_2[2] <<
		  DWC_DDR_UMCTL2_REGS_ADDRMAP1_ADDRMAP_BANK_B2_SHIFT) |
		(addrmap_bank_b0_2[1] <<
		 DWC_DDR_UMCTL2_REGS_ADDRMAP1_ADDRMAP_BANK_B1_SHIFT) |
		(addrmap_bank_b0_2[0] <<
		 DWC_DDR_UMCTL2_REGS_ADDRMAP1_ADDRMAP_BANK_B0_SHIFT)));

	/**
	 * Column address mapping
	 */
	al_reg_write32_masked(
		&ctrl_regs->addrmap2,
		(DWC_DDR_UMCTL2_REGS_ADDRMAP2_ADDRMAP_COL_B5_MASK |
		DWC_DDR_UMCTL2_REGS_ADDRMAP2_ADDRMAP_COL_B4_MASK |
		DWC_DDR_UMCTL2_REGS_ADDRMAP2_ADDRMAP_COL_B3_MASK |
		DWC_DDR_UMCTL2_REGS_ADDRMAP2_ADDRMAP_COL_B2_MASK),
		((addrmap_col_b2_11[3] <<
		  DWC_DDR_UMCTL2_REGS_ADDRMAP2_ADDRMAP_COL_B5_SHIFT) |
		(addrmap_col_b2_11[2] <<
		 DWC_DDR_UMCTL2_REGS_ADDRMAP2_ADDRMAP_COL_B4_SHIFT) |
		(addrmap_col_b2_11[1] <<
		 DWC_DDR_UMCTL2_REGS_ADDRMAP2_ADDRMAP_COL_B3_SHIFT) |
		(addrmap_col_b2_11[0] <<
		 DWC_DDR_UMCTL2_REGS_ADDRMAP2_ADDRMAP_COL_B2_SHIFT)));

	al_reg_write32_masked(
		&ctrl_regs->addrmap3,
		(DWC_DDR_UMCTL2_REGS_ADDRMAP3_ADDRMAP_COL_B9_MASK |
		DWC_DDR_UMCTL2_REGS_ADDRMAP3_ADDRMAP_COL_B8_MASK |
		DWC_DDR_UMCTL2_REGS_ADDRMAP3_ADDRMAP_COL_B7_MASK |
		DWC_DDR_UMCTL2_REGS_ADDRMAP3_ADDRMAP_COL_B6_MASK),
		((addrmap_col_b2_11[7] <<
		  DWC_DDR_UMCTL2_REGS_ADDRMAP3_ADDRMAP_COL_B9_SHIFT) |
		(addrmap_col_b2_11[6] <<
		 DWC_DDR_UMCTL2_REGS_ADDRMAP3_ADDRMAP_COL_B8_SHIFT) |
		(addrmap_col_b2_11[5] <<
		 DWC_DDR_UMCTL2_REGS_ADDRMAP3_ADDRMAP_COL_B7_SHIFT) |
		(addrmap_col_b2_11[4] <<
		 DWC_DDR_UMCTL2_REGS_ADDRMAP3_ADDRMAP_COL_B6_SHIFT)));

	al_reg_write32_masked(
		&ctrl_regs->addrmap4,
		(DWC_DDR_UMCTL2_REGS_ADDRMAP4_ADDRMAP_COL_B11_MASK |
		DWC_DDR_UMCTL2_REGS_ADDRMAP4_ADDRMAP_COL_B10_MASK),
		((addrmap_col_b2_11[9] <<
		  DWC_DDR_UMCTL2_REGS_ADDRMAP4_ADDRMAP_COL_B11_SHIFT) |
		(addrmap_col_b2_11[8] <<
		 DWC_DDR_UMCTL2_REGS_ADDRMAP4_ADDRMAP_COL_B10_SHIFT)));

	/**
	 * Row address mapping
	 */
	al_reg_write32_masked(
		&ctrl_regs->addrmap5,
		(DWC_DDR_UMCTL2_REGS_ADDRMAP5_ADDRMAP_ROW_B11_MASK |
		DWC_DDR_UMCTL2_REGS_ADDRMAP5_ADDRMAP_ROW_B2_10_MASK |
		DWC_DDR_UMCTL2_REGS_ADDRMAP5_ADDRMAP_ROW_B1_MASK |
		DWC_DDR_UMCTL2_REGS_ADDRMAP5_ADDRMAP_ROW_B0_MASK),
		((addrmap_row_b0_17[11] <<
		  DWC_DDR_UMCTL2_REGS_ADDRMAP5_ADDRMAP_ROW_B11_SHIFT) |
		(addrmap_row_b0_17[6] <<
		 DWC_DDR_UMCTL2_REGS_ADDRMAP5_ADDRMAP_ROW_B2_10_SHIFT) |
		(addrmap_row_b0_17[1] <<
		 DWC_DDR_UMCTL2_REGS_ADDRMAP5_ADDRMAP_ROW_B1_SHIFT) |
		(addrmap_row_b0_17[0] <<
		 DWC_DDR_UMCTL2_REGS_ADDRMAP5_ADDRMAP_ROW_B0_SHIFT)));

	al_reg_write32_masked(
		&ctrl_regs->addrmap6,
		(DWC_DDR_UMCTL2_REGS_ADDRMAP6_ADDRMAP_ROW_B15_MASK |
		DWC_DDR_UMCTL2_REGS_ADDRMAP6_ADDRMAP_ROW_B14_MASK |
		DWC_DDR_UMCTL2_REGS_ADDRMAP6_ADDRMAP_ROW_B13_MASK |
		DWC_DDR_UMCTL2_REGS_ADDRMAP6_ADDRMAP_ROW_B12_MASK),
		((addrmap_row_b0_17[15] <<
		  DWC_DDR_UMCTL2_REGS_ADDRMAP6_ADDRMAP_ROW_B15_SHIFT) |
		(addrmap_row_b0_17[14] <<
		 DWC_DDR_UMCTL2_REGS_ADDRMAP6_ADDRMAP_ROW_B14_SHIFT) |
		(addrmap_row_b0_17[13] <<
		 DWC_DDR_UMCTL2_REGS_ADDRMAP6_ADDRMAP_ROW_B13_SHIFT) |
		(addrmap_row_b0_17[12] <<
		 DWC_DDR_UMCTL2_REGS_ADDRMAP6_ADDRMAP_ROW_B12_SHIFT)));

	al_reg_write32_masked(
		&ctrl_regs->addrmap7,
		(DWC_DDR_UMCTL2_REGS_ADDRMAP7_ADDRMAP_ROW_B17_MASK |
		DWC_DDR_UMCTL2_REGS_ADDRMAP7_ADDRMAP_ROW_B16_MASK),
		((addrmap_row_b0_17[17] <<
		  DWC_DDR_UMCTL2_REGS_ADDRMAP7_ADDRMAP_ROW_B17_SHIFT) |
		(addrmap_row_b0_17[16] <<
		 DWC_DDR_UMCTL2_REGS_ADDRMAP7_ADDRMAP_ROW_B16_SHIFT)));

	al_reg_write32(
		&nb_regs->dram_channels.address_map,
		((addrmap_row_b0_17[2] << NB_DRAM_CHANNELS_ADDRESS_MAP_ADDRMAP_ROW_B2_SHIFT) |
		 (addrmap_row_b0_17[3] << NB_DRAM_CHANNELS_ADDRESS_MAP_ADDRMAP_ROW_B3_SHIFT) |
		 (addrmap_row_b0_17[4] << NB_DRAM_CHANNELS_ADDRESS_MAP_ADDRMAP_ROW_B4_SHIFT) |
		 (addrmap_row_b0_17[5] << NB_DRAM_CHANNELS_ADDRESS_MAP_ADDRMAP_ROW_B5_SHIFT) |
		 NB_DRAM_CHANNELS_ADDRESS_MAP_ADDRMAP_PART_EN));
	/**
	 * Bank Group address mapping
	 */
	al_reg_write32_masked(
		&ctrl_regs->addrmap8,
		(DWC_DDR_UMCTL2_REGS_ADDRMAP8_ADDRMAP_BG_B1_MASK |
		DWC_DDR_UMCTL2_REGS_ADDRMAP8_ADDRMAP_BG_B0_MASK),
		((addrmap_bg_b0_1[1] <<
		  DWC_DDR_UMCTL2_REGS_ADDRMAP8_ADDRMAP_BG_B1_SHIFT) |
		(addrmap_bg_b0_1[0] <<
		 DWC_DDR_UMCTL2_REGS_ADDRMAP8_ADDRMAP_BG_B0_SHIFT)));


	return 0;
}

/*******************************************************************************
 ******************************************************************************/
static inline int al_ddr_phy_init(
	struct al_ddr_init_cfg	*cfg)
{
	int i;
	uint32_t reg_val;

	STRUCT_PHY_REGS __iomem *phy_regs = PHY_REGS(cfg->ddr_cfg.ddr_phy_regs_base);

	unsigned int active_ranks_no_cid = active_ranks_calc(cfg->org.dimms, cfg->org.ranks);

	unsigned int active_ranks_with_cid = active_ranks_calc_phy(active_ranks_no_cid);

	/**
	 * Refresh Period
	 * 20000 clocks must be subtracted from the derived number to account for
	 * command flow and missed slots of refreshes
	 *
	 * Required value: TREFI_9 - 20000
	 */
	unsigned int t_refprd =
		al_ddr_ns_to_ck_rnd_dn(REF_CLK_FREQ_MHZ, 9*TREFI_NS - 20000);

	/* PLLCR -  */
	if (REF_CLK_FREQ_MHZ < AL_DDR_PHY_PLL_FREQ_SEL_MHZ_ALPINE_V2)
		al_reg_write32_masked(
			&phy_regs->pllcr_typ_a,
			ALPINE_V2_DWC_DDR_PHY_REGS_PLLCR_FRQSEL_MASK,
			ALPINE_V2_DWC_DDR_PHY_REGS_PLLCR_FRQSEL_225MHZ_490MHz);

	/* DXCCR -  */
	al_reg_write32_masked(
		&phy_regs->dxccr,
		(ALPINE_V2_DWC_DDR_PHY_REGS_DXCCR_X4DQSMD |
		 ALPINE_V2_DWC_DDR_PHY_REGS_DXCCR_X4MODE),
		(cfg->org.ddr_device == AL_DDR_DEVICE_X4 ?
		 ALPINE_V2_DWC_DDR_PHY_REGS_DXCCR_X4MODE :
		 ALPINE_V2_DWC_DDR_PHY_REGS_DXCCR_X4DQSMD));

	if (cfg->org.ddr_type == AL_DDR_TYPE_DDR4)
		/* Disable DQS Pull up/down for DDR4 */
		al_reg_write32_masked(
			&phy_regs->dxccr,
			ALPINE_V2_DWC_DDR_PHY_REGS_DXCCR_DQSRES_MASK |
			ALPINE_V2_DWC_DDR_PHY_REGS_DXCCR_DQSNRES_MASK,
			(0x0 << ALPINE_V2_DWC_DDR_PHY_REGS_DXCCR_DQSRES_SHIFT) |
			(0x8 << ALPINE_V2_DWC_DDR_PHY_REGS_DXCCR_DQSNRES_SHIFT));

	/* DSGCR -  */
	/* DSGCR -  */
	reg_val = 0x0;

	if (rr_mode)
		reg_val |= ALPINE_V2_DWC_DDR_PHY_REGS_DSGCR_WRRMODE | ALPINE_V2_DWC_DDR_PHY_REGS_DSGCR_RRRMODE;

	reg_val |= ALPINE_V2_DWC_DDR_PHY_REGS_DSGCR_SDRMODE_HDR;

	al_reg_write32_masked(
		&phy_regs->dsgcr,
		ALPINE_V2_DWC_DDR_PHY_REGS_DSGCR_PUREN |
		ALPINE_V2_DWC_DDR_PHY_REGS_DSGCR_WRRMODE |
		ALPINE_V2_DWC_DDR_PHY_REGS_DSGCR_SDRMODE_MASK |
		ALPINE_V2_DWC_DDR_PHY_REGS_DSGCR_RRRMODE,
		reg_val);


	/* DCR -  */
	reg_val = 0x0;

	if (en_2t_timing_mode)
		reg_val |= ALPINE_V2_DWC_DDR_PHY_REGS_DCR_DDR2T;

	if (cfg->org.udimm_addr_mirroring)
		reg_val |= ALPINE_V2_DWC_DDR_PHY_REGS_DCR_UDIMM |
			ALPINE_V2_DWC_DDR_PHY_REGS_DCR_NOSRA;

	if (cfg->org.rdimm)
		reg_val |= ALPINE_V2_DWC_DDR_PHY_REGS_DCR_NOSRA;

	if (cfg->org.ddr_type == AL_DDR_TYPE_DDR3)
		reg_val |= ALPINE_V2_DWC_DDR_PHY_REGS_DCR_DDRMD_DDR3;
	else
		reg_val |= ALPINE_V2_DWC_DDR_PHY_REGS_DCR_DDRMD_DDR4;

	if (cfg->org.ddr_device == AL_DDR_DEVICE_X16)
		reg_val |= ALPINE_V2_DWC_DDR_PHY_REGS_DCR_UBG;

	al_reg_write32_masked(
		&phy_regs->dcr,
		ALPINE_V2_DWC_DDR_PHY_REGS_DCR_DDRMD_MASK |
		ALPINE_V2_DWC_DDR_PHY_REGS_DCR_DDR2T |
		ALPINE_V2_DWC_DDR_PHY_REGS_DCR_UDIMM |
		ALPINE_V2_DWC_DDR_PHY_REGS_DCR_NOSRA |
		ALPINE_V2_DWC_DDR_PHY_REGS_DCR_UBG |
		ALPINE_V2_DWC_DDR_PHY_REGS_DCR_DDRTYPE_MASK,
		reg_val);


	/* MR0 -  */
	reg_val = cfg->calc.mr0;
	if (cfg->org.ddr_type == AL_DDR_TYPE_DDR4)
		AL_REG_MASK_CLEAR(reg_val, AL_DDR_JEDEC_DDR4_MR0_DR);
	al_reg_write32(&phy_regs->mr[0], reg_val);

	/* MR1 -  */
	reg_val = cfg->calc.mr1;
	al_reg_write32(&phy_regs->mr[1], reg_val);

	/* MR2 -  */
	reg_val = cfg->calc.mr2;
	al_reg_write32(&phy_regs->mr[2], reg_val);

	/* MR3 -  */
	reg_val = cfg->calc.mr3;
	al_reg_write32(&phy_regs->mr[3], reg_val);

	/* MR4 -  */
	reg_val = cfg->calc.mr4;
	al_reg_write32(&phy_regs->mr[4], reg_val);

	/* MR5 -  */
	reg_val = cfg->calc.mr5;
	al_reg_write32(&phy_regs->mr[5], reg_val);

	/* MR6 -  */
	reg_val = cfg->calc.mr6;
	al_reg_write32(&phy_regs->mr[6], reg_val);

	/* ZQ - change of SoC Rout/ODT settings */

	/* Stop ZQ Calibration */
	al_reg_write32_masked(
			&phy_regs->pir,
			ALPINE_V2_DWC_DDR_PHY_REGS_PIR_ZCAL,
			0);
	al_reg_write32_masked(
			&phy_regs->zqcr,
			ALPINE_V2_DWC_DDR_PHY_REGS_ZQCR_ZCALT_MASK,
			0);

	/* Allow ~1mSec for potential ongoing cal to stop */
#if (defined(AL_DDR_VERIF_SHORT_TIME) && (AL_DDR_VERIF_SHORT_TIME == 1))
	al_udelay(1);
#else
	al_udelay(1000);
#endif
	/* Change ZQ Cal mode */
	for (i = 0; i < AL_DDR_PHY_NUM_ZQ_SEGMENTS_ALPINE_V2; i++) {
		if (cfg->org.ddr_type == AL_DDR_TYPE_DDR3) {
			reg_val = 0;

			switch (cfg->impedance_ctrl.phy_rout[i]) {
			case AL_DDR_PHY_ROUT_80OHM:
				reg_val |= ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_80OHM;
				break;
			case AL_DDR_PHY_ROUT_68OHM:
				reg_val |= ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_68OHM;
				break;
			case AL_DDR_PHY_ROUT_60OHM:
				reg_val |= ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_60OHM;
				break;
			case AL_DDR_PHY_ROUT_53OHM:
				reg_val |= ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_53OHM;
				break;
			case AL_DDR_PHY_ROUT_48OHM:
				reg_val |= ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_48OHM;
				break;
			case AL_DDR_PHY_ROUT_44OHM:
				reg_val |= ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_44OHM;
				break;
			case AL_DDR_PHY_ROUT_40OHM:
				reg_val |= ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_40OHM;
				break;
			case AL_DDR_PHY_ROUT_37OHM:
				reg_val |= ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_37OHM;
				break;
			case AL_DDR_PHY_ROUT_34OHM:
				reg_val |= ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_34OHM;
				break;
			case AL_DDR_PHY_ROUT_32OHM:
				reg_val |= ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_32OHM;
				break;
			case AL_DDR_PHY_ROUT_30OHM:
				reg_val |= ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ROUT_30OHM;
				break;
			default:
				al_err("invld PHY ROUT (%d)!\n", cfg->impedance_ctrl.phy_rout[i]);
				return -EINVAL;
			};

			switch (cfg->impedance_ctrl.phy_odt[i]) {
			case AL_DDR_PHY_ODT_200OHM:
				reg_val |= ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_200OHM;
				break;
			case AL_DDR_PHY_ODT_133OHM:
				reg_val |= ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_133OHM;
				break;
			case AL_DDR_PHY_ODT_100OHM:
				reg_val |= ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_100OHM;
				break;
			case AL_DDR_PHY_ODT_77OHM:
				reg_val |= ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_77OHM;
				break;
			case AL_DDR_PHY_ODT_66OHM:
				reg_val |= ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_66OHM;
				break;
			case AL_DDR_PHY_ODT_56OHM:
				reg_val |= ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_56OHM;
				break;
			case AL_DDR_PHY_ODT_50OHM:
				reg_val |= ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_50OHM;
				break;
			case AL_DDR_PHY_ODT_44OHM:
				reg_val |= ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_44OHM;
				break;
			case AL_DDR_PHY_ODT_40OHM:
				reg_val |= ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_40OHM;
				break;
			case AL_DDR_PHY_ODT_36OHM:
				reg_val |= ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_36OHM;
				break;
			case AL_DDR_PHY_ODT_33OHM:
				reg_val |= ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_33OHM;
				break;
			case AL_DDR_PHY_ODT_30OHM:
				reg_val |= ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_30OHM;
				break;
			case AL_DDR_PHY_ODT_28OHM:
				reg_val |= ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_28OHM;
				break;
			case AL_DDR_PHY_ODT_26OHM:
				reg_val |= ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_26OHM;
				break;
			case AL_DDR_PHY_ODT_25OHM:
				reg_val |= ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_ODT_25OHM;
				break;
			default:
				al_err("invld PHY ODT (%d)!\n", cfg->impedance_ctrl.phy_odt[i]);
				return -EINVAL;
			};

			al_reg_write32_masked(&phy_regs->zq[i].pr,
				ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZQDIV_MASK,
				reg_val);

		} else { /* DDR4 */

			/* Use Asymmetric drive */
			al_reg_write32_masked(
				&phy_regs->gpr[0],
				ALPINE_V2_DWC_DDR_PHY_REGS_GPR0_ASYM_DRV_FIX_DIS,
				0);

			al_reg_write32_masked(
				&phy_regs->zqcr,
				ALPINE_V2_DWC_DDR_PHY_REGS_ZQCR_ASYM_DRV_EN |
				ALPINE_V2_DWC_DDR_PHY_REGS_ZQCR_PU_ODT_ONLY |
				ALPINE_V2_DWC_DDR_PHY_REGS_ZQCR_DIS_NON_LIN_COMP |
				ALPINE_V2_DWC_DDR_PHY_REGS_ZQCR_PGWAIT_MASK,
				ALPINE_V2_DWC_DDR_PHY_REGS_ZQCR_ASYM_DRV_EN |
				ALPINE_V2_DWC_DDR_PHY_REGS_ZQCR_PU_ODT_ONLY |
				(0x7 << ALPINE_V2_DWC_DDR_PHY_REGS_ZQCR_PGWAIT_SHIFT));

			/* Segment 0 is for A/C - no need for PD_DRV_ADJUST */
			al_reg_write32_masked(&phy_regs->zq[i].pr,
				ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZPROG_ASYM_DRV_PU_MASK |
				ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZPROG_ASYM_DRV_PD_MASK |
				ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZPROG_PU_ODT_ONLY_MASK |
				ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_PD_DRV_ADJUST_MASK,
				(cfg->impedance_ctrl.phy_rout_pu[i] <<
				 ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZPROG_ASYM_DRV_PU_SHIFT) |
				(cfg->impedance_ctrl.phy_rout_pd[i] <<
				 ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZPROG_ASYM_DRV_PD_SHIFT) |
				(cfg->impedance_ctrl.phy_pu_odt[i] <<
				 ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_ZPROG_PU_ODT_ONLY_SHIFT) |
				((i == 0 ? 0x0 : 0x1) <<
				 ALPINE_V2_DWC_DDR_PHY_REGS_ZQnPR_PD_DRV_ADJUST_SHIFT));
		}
	}

	/* Restart Live ZQ Calibration */
	/* Triggering of the ZQ calibration is done later during PHY init */
	al_reg_write32_masked(
		&phy_regs->zqcr,
		ALPINE_V2_DWC_DDR_PHY_REGS_ZQCR_ZCALT_MASK,
		(0x1 << ALPINE_V2_DWC_DDR_PHY_REGS_ZQCR_ZCALT_SHIFT));
	/* Allow ~1mSec delay for IO to re-calibrate to new settings */
#if (defined(AL_DDR_VERIF_SHORT_TIME) && (AL_DDR_VERIF_SHORT_TIME == 1))
	al_udelay(1);
#else
	al_udelay(1000);
#endif

	/* ODTCR */
	for (i = 0; i < (int)cfg->org.ranks; i++) {
		/* Select rank */
		al_reg_write32(&phy_regs->rankidr,
				((i << ALPINE_V2_DWC_DDR_PHY_REGS_RANKIDR_RANKWID_SHIFT) |
				(i << ALPINE_V2_DWC_DDR_PHY_REGS_RANKIDR_RANKRID_SHIFT)));
		/* Set ODT */
		al_reg_write32(&phy_regs->odtcr,
				((cfg->impedance_ctrl.wr_odt_map[i] <<
				ALPINE_V2_DWC_DDR_PHY_REGS_ODTCR_WRODT_SHIFT) |
				(cfg->impedance_ctrl.rd_odt_map[i] <<
				ALPINE_V2_DWC_DDR_PHY_REGS_ODTCR_RDODT_SHIFT)));
	}

	/* DTCR  */
	if (cfg->misc.training_en) {
		al_reg_write32_masked(
			&phy_regs->dtcr[0],
			ALPINE_V2_DWC_DDR_PHY_REGS_DTCR0_DTMPR |
			ALPINE_V2_DWC_DDR_PHY_REGS_DTCR0_DTWBDDM |
			ALPINE_V2_DWC_DDR_PHY_REGS_DTCR0_DTRDBITR_MASK,
			ALPINE_V2_DWC_DDR_PHY_REGS_DTCR0_DTMPR |
			(((cfg->org.ddr_device == AL_DDR_DEVICE_X4) ?
			 0 : ALPINE_V2_DWC_DDR_PHY_REGS_DTCR0_DTWBDDM) |
			 (cfg->misc.read_dbi_enabled ?
			 ALPINE_V2_DWC_DDR_PHY_REGS_DTCR0_DTRDBITR_WITH_RDQ :
			 ALPINE_V2_DWC_DDR_PHY_REGS_DTCR0_DTRDBITR_DIS2)));
		al_reg_write32_masked(
			&phy_regs->dtcr[1],
			ALPINE_V2_DWC_DDR_PHY_REGS_DTCR1_RANKEN_MASK |
			ALPINE_V2_DWC_DDR_PHY_REGS_DTCR1_RANKEN_RSVD_MASK,
			(active_ranks_with_cid << ALPINE_V2_DWC_DDR_PHY_REGS_DTCR1_RANKEN_SHIFT));

		if (cfg->org.ddr_type == AL_DDR_TYPE_DDR4) {
			for (i = 0; i < AL_DDR_PHY_NUM_BYTE_LANES; i++) {
				al_reg_write32_masked(
					&phy_regs->datx8[i].gcr[5],
					ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR5_DXREFISELR0_MASK |
					ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR5_DXREFISELR1_MASK |
					ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR5_DXREFISELR2_MASK |
					ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR5_DXREFISELR3_MASK,
					(cfg->impedance_ctrl.host_initial_vref <<
					 ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR5_DXREFISELR0_SHIFT) |
					(cfg->impedance_ctrl.host_initial_vref <<
					 ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR5_DXREFISELR1_SHIFT) |
					(cfg->impedance_ctrl.host_initial_vref <<
					 ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR5_DXREFISELR2_SHIFT) |
					(cfg->impedance_ctrl.host_initial_vref <<
					 ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR5_DXREFISELR3_SHIFT));

				al_reg_write32_masked(
					&phy_regs->datx8[i].gcr[8],
					ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR8_X4DXREFISELR0_MASK |
					ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR8_X4DXREFISELR1_MASK |
					ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR8_X4DXREFISELR2_MASK |
					ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR8_X4DXREFISELR3_MASK,
					(cfg->impedance_ctrl.host_initial_vref <<
					 ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR8_X4DXREFISELR0_SHIFT) |
					(cfg->impedance_ctrl.host_initial_vref <<
					 ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR8_X4DXREFISELR1_SHIFT) |
					(cfg->impedance_ctrl.host_initial_vref <<
					 ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR8_X4DXREFISELR2_SHIFT) |
					(cfg->impedance_ctrl.host_initial_vref <<
					 ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR8_X4DXREFISELR3_SHIFT));
			}

			al_reg_write32_masked(
				&phy_regs->iovcr[1],
				ALPINE_V2_DWC_DDR_PHY_REGS_IOVCR1_ZQREFISEL_MASK,
				(0x9 << ALPINE_V2_DWC_DDR_PHY_REGS_IOVCR1_ZQREFISEL_SHIFT));

#if (!defined(AL_DDR_VERIF_SHORT_TIME) || (AL_DDR_VERIF_SHORT_TIME == 0))
			/*
			 * Addressing RMN: 11189 - DRAM VREFDQ Programming Required before
			 * running Data Training
			 *
			 * RMN description:
			 * Initial VREFDQ value may not be programmed into the DRAM (MR6)
			 * during DRAM initialization. JESD79-4B requires two consecutive
			 * MRS commands to MR6; the first to enable VREF training and the
			 * second to provide the initial VREF value. Currently the PHY
			 * issues a single MRS command to MR6 during DRAM initialization
			 * which enables VREF training and provides the initial VREF value.
			 * This initial VREF value may not be programmed in some DRAMs
			 * Software flow:
			 * configure the start vrefdq value at VTCR0.DVINIT
			 */
			al_reg_write32_masked(
				&phy_regs->vtcr[0],
				(ALPINE_V2_DWC_DDR_PHY_REGS_VTCR0_DVINIT_MASK |
				ALPINE_V2_DWC_DDR_PHY_REGS_VTCR0_PDAEN),
				((cfg->impedance_ctrl.vrefdq <<
				  ALPINE_V2_DWC_DDR_PHY_REGS_VTCR0_DVINIT_SHIFT) |
				 ALPINE_V2_DWC_DDR_PHY_REGS_VTCR0_PDAEN));

			al_reg_write32_masked(
				&phy_regs->vtcr[1],
				ALPINE_V2_DWC_DDR_PHY_REGS_VTCR1_ENUM |
				ALPINE_V2_DWC_DDR_PHY_REGS_VTCR1_VWCR_MASK |
				ALPINE_V2_DWC_DDR_PHY_REGS_VTCR1_HVSS_MASK,
				ALPINE_V2_DWC_DDR_PHY_REGS_VTCR1_ENUM |
				(0xf << ALPINE_V2_DWC_DDR_PHY_REGS_VTCR1_VWCR_SHIFT) |
				(0x0 << ALPINE_V2_DWC_DDR_PHY_REGS_VTCR1_HVSS_SHIFT));

			if (cfg->impedance_ctrl.hv_min != AL_DDR_HV_INVALID_VAL) {
				al_reg_write32_masked(
					&phy_regs->vtcr[1],
					ALPINE_V2_DWC_DDR_PHY_REGS_VTCR1_HVMIN_MASK,
					(cfg->impedance_ctrl.hv_min <<
						ALPINE_V2_DWC_DDR_PHY_REGS_VTCR1_HVMIN_SHIFT));
			}
			if (cfg->impedance_ctrl.hv_max != AL_DDR_HV_INVALID_VAL) {
				al_reg_write32_masked(
					&phy_regs->vtcr[1],
					ALPINE_V2_DWC_DDR_PHY_REGS_VTCR1_HVMAX_MASK,
					(cfg->impedance_ctrl.hv_max <<
						ALPINE_V2_DWC_DDR_PHY_REGS_VTCR1_HVMAX_SHIFT));
			}

#endif
		}

	}

	/* DTPR0 -  */
	reg_val =
		((cfg->calc.t_rtp << ALPINE_V2_DWC_DDR_PHY_REGS_DTPR0_TRTP_SHIFT) |
		(cfg->calc.t_rp << ALPINE_V2_DWC_DDR_PHY_REGS_DTPR0_TRP_SHIFT) |
		(cfg->calc.t_ras_min << ALPINE_V2_DWC_DDR_PHY_REGS_DTPR0_TRAS_SHIFT) |
		(cfg->calc.t_rrd << ALPINE_V2_DWC_DDR_PHY_REGS_DTPR0_TRRD_SHIFT));

	al_reg_write32(&phy_regs->dtpr[0], reg_val);

	/* GPR1 -  */
	al_reg_write32_masked(
		&phy_regs->gpr[1],
		(ALPINE_V2_DWC_DDR_PHY_REGS_GPR1_RDODT_DLY_VAL_MASK |
		 ALPINE_V2_DWC_DDR_PHY_REGS_GPR1_RDODT_DLY_EN),
		(((cfg->calc.rd_odt_delay <= AL_DDR_PHY_MAX_READ_ODT_DELAY ?
		  cfg->calc.rd_odt_delay : AL_DDR_PHY_MAX_READ_ODT_DELAY) <<
		  ALPINE_V2_DWC_DDR_PHY_REGS_GPR1_RDODT_DLY_VAL_SHIFT) |
		  ALPINE_V2_DWC_DDR_PHY_REGS_GPR1_RDODT_DLY_EN));

	/* DTPR3 -  */
	al_reg_write32_masked(
		&phy_regs->dtpr[3],
		(ALPINE_V2_DWC_DDR_PHY_REGS_DTPR3_TOFDX_MASK |
		ALPINE_V2_DWC_DDR_PHY_REGS_DTPR3_TDLLK_MASK),
		((phy_odt_hold << ALPINE_V2_DWC_DDR_PHY_REGS_DTPR3_TOFDX_SHIFT) |
		(cfg->calc.t_dllk << ALPINE_V2_DWC_DDR_PHY_REGS_DTPR3_TDLLK_SHIFT)));

	/* DTPR4 -  */
	al_reg_write32_masked(
		&phy_regs->dtpr[4],
		ALPINE_V2_DWC_DDR_PHY_REGS_DTPR4_TRFC_MASK,
		(cfg->calc.t_rfc_min << ALPINE_V2_DWC_DDR_PHY_REGS_DTPR4_TRFC_SHIFT));

	/* DTPR5 -  */
	reg_val =
		((cfg->calc.t_wtr << ALPINE_V2_DWC_DDR_PHY_REGS_DTPR5_TWTR_SHIFT) |
		(cfg->calc.t_rcd << ALPINE_V2_DWC_DDR_PHY_REGS_DTPR5_TRCD_SHIFT) |
		(cfg->calc.t_rc << ALPINE_V2_DWC_DDR_PHY_REGS_DTPR5_TRC_SHIFT));

	al_reg_write32(&phy_regs->dtpr[5], reg_val);

	/* RDIMMGCR */
	al_reg_write32_masked(
		&phy_regs->rdimmgcr[0],
		ALPINE_V2_DWC_DDR_PHY_REGS_RDIMMGCR0_RDIMM,
		cfg->org.rdimm ? ALPINE_V2_DWC_DDR_PHY_REGS_RDIMMGCR0_RDIMM : 0);

	if (cfg->org.rdimm && (cfg->org.ddr_type == AL_DDR_TYPE_DDR4)) {
		if (cfg->addrmap.row_b0_17[17] == AL_DDR_ADDRMAP_NC) {
			al_reg_write32_masked(
				&phy_regs->rdimmgcr[1],
				ALPINE_V2_DWC_DDR_PHY_REGS_RDIMMGCR1_A17BID,
				ALPINE_V2_DWC_DDR_PHY_REGS_RDIMMGCR1_A17BID);
			al_reg_write32_masked(
				&phy_regs->rdimmcr[1],
				ALPINE_V2_DWC_DDR_PHY_REGS_RDIMMCR1_RC8_MASK,
				0x8 << ALPINE_V2_DWC_DDR_PHY_REGS_RDIMMCR1_RC8_SHIFT);
		}
		if (cfg->org.udimm_addr_mirroring)
			al_reg_write32_masked(
				&phy_regs->rdimmcr[1],
				ALPINE_V2_DWC_DDR_PHY_REGS_RDIMMCR1_RC13_MASK,
				0x8 << ALPINE_V2_DWC_DDR_PHY_REGS_RDIMMCR1_RC13_SHIFT);
	}

	/*
	* Addressing RMN: 11052
	*
	* RMN description:
	* Write-leveling might align DQS to negative CK edge
	* Software flow:
	* Use a PGCR1.WLSTEP of 32 taps (WLSTEP = 0)
	*/
	/* PGCR1 */
	al_reg_write32_masked(
		&phy_regs->pgcr[1],
		ALPINE_V2_DWC_DDR_PHY_REGS_PGCR1_DDLBYPMODE_MASK |
		ALPINE_V2_DWC_DDR_PHY_REGS_PGCR1_WLSTEP |
		ALPINE_V2_DWC_DDR_PHY_REGS_PGCR1_IODDRM_MASK,
		ALPINE_V2_DWC_DDR_PHY_REGS_PGCR1_DDLBYPMODE_DYNAMIC |
		ALPINE_V2_DWC_DDR_PHY_REGS_PGCR1_IODDRM_D4M);

	/* PGCR2 */
	al_reg_write32_masked(
		&phy_regs->pgcr[2],
		ALPINE_V2_DWC_DDR_PHY_REGS_PGCR2_TREFPRD_MASK,
		(t_refprd << ALPINE_V2_DWC_DDR_PHY_REGS_PGCR2_TREFPRD_SHIFT));

	/* Addressing RMN: 11212
	*
	* RMN description:
	* Use of the DDL Duty Cycle Correction (DCC) feature should be avoided
	* Software flow:
	* For PUB 3.00 or older:
	* Disable DCC by writing 1'b1 to PGCR4.ACDCCBYP and DXCCR.DXDCCBYP
	*/
	/* PGCR4 */
	al_reg_write32_masked(
		&phy_regs->pgcr[4],
		ALPINE_V2_DWC_DDR_PHY_REGS_PGCR4_ACDCCBYP,
		ALPINE_V2_DWC_DDR_PHY_REGS_PGCR4_ACDCCBYP);
	/* DXCCR */
	al_reg_write32_masked(
		&phy_regs->dxccr,
		ALPINE_V2_DWC_DDR_PHY_REGS_DXCCR_DXDCCBYP,
		ALPINE_V2_DWC_DDR_PHY_REGS_DXCCR_DXDCCBYP);

	/* Do PHY init */
	al_reg_write32(&phy_regs->pir,
				ALPINE_V2_DWC_DDR_PHY_REGS_PIR_INIT |
				ALPINE_V2_DWC_DDR_PHY_REGS_PIR_ZCAL |
				ALPINE_V2_DWC_DDR_PHY_REGS_PIR_PLLINIT |
				ALPINE_V2_DWC_DDR_PHY_REGS_PIR_DCAL |
				ALPINE_V2_DWC_DDR_PHY_REGS_PIR_PHYRST);

	/* Read PGSR0 for initialization done status */

	al_data_memory_barrier();

	al_ddr_phy_wait_for_init_done(phy_regs);

	/* Force ZQ update */
	al_reg_write32_masked(
			&phy_regs->zqcr,
			ALPINE_V2_DWC_DDR_PHY_REGS_ZQCR_FORCE_ZCAL_VT_UPDATE,
			ALPINE_V2_DWC_DDR_PHY_REGS_ZQCR_FORCE_ZCAL_VT_UPDATE);
	al_reg_write32_masked(
			&phy_regs->zqcr,
			ALPINE_V2_DWC_DDR_PHY_REGS_ZQCR_FORCE_ZCAL_VT_UPDATE,
			0);

	/*
	 * Addressing RMN: 11688
	 *
	 * RMN description:
	 * The duration of RESET_n applied to the DRAM during initialization is determined by
	 * the reset mode used (`define DWC_RST_DFLT) and the value of PTR4[17:0]:tINIT2 in
	 * clock cycles. The width of PTR4[17:0]:tINIT2 is only sufficient to specify sufficient
	 * clock cycles to achieve the JEDEC required 200 us reset duration when operating below
	 * 1310 MHz (2620 Mbps). If DWC_RST_DFLT is 0, the PUB will perform additional tasks
	 * such as PLL lock and DDL calibration (minimum of 50 us) before de-asserting reset_n,
	 * and the 200 us JEDEC requirement will be met. If DWC_RST_DFLT = 1, RESET_n will be
	 * asserted only for the number of clock cycles specified in PTR4[17:0]:tINIT2 and if
	 * above 1310 MHz, the duration will be less than the required 200 us.
	 *
	 * Software flow:
	 * No need any action (we're using controller initialization, and this RMN is not
	 * relevant for controller initialization).
	 */
	/**
	 * PIR - set DRAM initialization by controller and triger init sequence
	 */
	al_reg_write32(&phy_regs->pir,
				ALPINE_V2_DWC_DDR_PHY_REGS_PIR_INIT |
				ALPINE_V2_DWC_DDR_PHY_REGS_PIR_CTLDINIT);

	/* Read PGSR0 for initialization done status */
	/* PGSR0 */

	al_data_memory_barrier();

	al_ddr_phy_wait_for_init_done(phy_regs);

	if (cfg->misc.training_en) {
	} else {
		al_ddr_phy_vt_calc_disable(&cfg->ddr_cfg);

		/* Data lanes config */
		for (i = 0; i < AL_DDR_PHY_NUM_BYTE_LANES; i++)
			al_ddr_phy_data_lane_cfg(cfg, i);

		al_ddr_phy_vt_calc_enable(&cfg->ddr_cfg);
	}

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
static inline void al_ddr_reset(
	struct al_ddr_init_cfg	*cfg)
{
	struct al_nb_regs __iomem *nb_regs =
		(struct al_nb_regs __iomem *)cfg->ddr_cfg.nb_regs_base;
	uint32_t *rst_reg = &nb_regs->global.ddrc_hold_reset;

	/* Reset DDR complex */
	al_reg_write32(rst_reg,
			NB_GLOBAL_DDRC_HOLD_RESET_APB_SYNC_RESET |
			NB_GLOBAL_DDRC_HOLD_RESET_CORE_SYNC_RESET |
			NB_GLOBAL_DDRC_HOLD_RESET_AXI_SYNC_RESET |
			NB_GLOBAL_DDRC_HOLD_RESET_PUB_CTL_SYNC_RESET |
			NB_GLOBAL_DDRC_HOLD_RESET_PUB_SDR_SYNC_RESET |
			NB_GLOBAL_DDRC_HOLD_RESET_PHY_SYNC_RESET);

#if (defined(AL_DDR_VERIF_SHORT_TIME) && (AL_DDR_VERIF_SHORT_TIME == 1))
	al_udelay(1);
#else
	al_udelay(100);
#endif

	al_reg_write32_masked(
		rst_reg,
		(NB_GLOBAL_DDRC_HOLD_RESET_APB_SYNC_RESET |
		NB_GLOBAL_DDRC_HOLD_RESET_PUB_CTL_SYNC_RESET |
		NB_GLOBAL_DDRC_HOLD_RESET_PUB_SDR_SYNC_RESET),
		0);

	al_data_memory_barrier();

	/*
	 * Addressing RMN: 11766
	 *
	 * RMN description:
	 * After the de-assertion of presetn (APB reset), need to allow a certain time for
	 * synchronization of presetn to core_ddrc_core_clk and aclk domains and to allow
	 * initialization of end logic.
	 * Software flow:
	 * Time required is 128 pclk cycles, which was previously documented
	 * as 10 pclk cycles.
	 */

	/* Wait for reset to propagate to DDR (around 50ns) */
	al_udelay(1);
}

/*******************************************************************************
 ******************************************************************************/
static inline int al_ddr_unit_reset_release(
	struct al_ddr_init_cfg	*cfg)
{
	struct al_nb_regs __iomem *nb_regs =
		(struct al_nb_regs __iomem *)cfg->ddr_cfg.nb_regs_base;
	uint32_t *rst_reg = &nb_regs->global.ddrc_hold_reset;

	/* Take the controller out of reset, at this stage all ddr phy and */
	/* controller resets are de-asserted */

	al_reg_write32_masked(
		rst_reg,
		(NB_GLOBAL_DDRC_HOLD_RESET_CORE_SYNC_RESET |
		NB_GLOBAL_DDRC_HOLD_RESET_AXI_SYNC_RESET |
		NB_GLOBAL_DDRC_HOLD_RESET_PHY_SYNC_RESET),
		0);

	/* Making sure the next read/write from controller will not happen */
	/* before reset is deasserted */
	al_data_memory_barrier();

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
static inline int al_ddr_ctrl_dfi_init(
	struct al_ddr_init_cfg	*cfg)
{
	STRUCT_CTRL_REGS __iomem *ctrl_regs = CTRL_REGS(cfg->ddr_cfg.ddr_ctrl_regs_base);

	int err;

	/*
	 * Addressing RMN: 11030
	 *
	 * RMN description:
	 * The register MRSTAT.mr_wr_busy is not cleared after
	 * completing the software-driven MR access
	 * Software flow:
	 * Disable the sources of hardware-driven MR accesses by writing the following
	 * registers before initiating the software-driven MR access.
	 * PWRCTL.mpsm_en = 0
	 * PWRCTL.selfref_sw = 0
	 * PWRCTL.selfref_en = 0
	 * HWLPCTL.hw_lp_en = 0
	 */
	uint32_t pwrctl = al_reg_read32(&ctrl_regs->pwrctl);
	uint32_t hwlpctl = al_reg_read32(&ctrl_regs->hwlpctl);

	al_reg_write32_masked(
		&ctrl_regs->pwrctl,
		(DWC_DDR_UMCTL2_REGS_PWRCTL_MPSM_EN |
		DWC_DDR_UMCTL2_REGS_PWRCTL_SELFREF_SW |
		DWC_DDR_UMCTL2_REGS_PWRCTL_SELFREF_EN),
		0);
	al_reg_write32_masked(
		&ctrl_regs->hwlpctl,
		DWC_DDR_UMCTL2_REGS_HWLPCTL_HW_LP_EN,
		0);

	/* For DDR4 RDIMM need to init the RCD */
	if (cfg->org.rdimm && (cfg->org.ddr_type == AL_DDR_TYPE_DDR4))
		al_reg_write32_masked(
			&ctrl_regs->mrctrl0,
			DWC_DDR_UMCTL2_REGS_MRCTRL0_RCD_INIT_EN,
			DWC_DDR_UMCTL2_REGS_MRCTRL0_RCD_INIT_EN);

	al_reg_write32_masked(
		&ctrl_regs->dfimisc,
		DWC_DDR_UMCTL2_REGS_DFIMISC_DFI_INIT_COMPLETE_EN,
		DWC_DDR_UMCTL2_REGS_DFIMISC_DFI_INIT_COMPLETE_EN);

	al_data_memory_barrier();

	if (cfg->org.rdimm && (cfg->org.ddr_type == AL_DDR_TYPE_DDR4)) {

		/* TODO : should wait for RCD init to arrive and write the mode register */
		al_udelay(10);

		/*
		* DDR4 RDIMM register write
		* writes are to MRS7: {BG[1:0], BA[1:0]} = 4'b0111
		* For 4-bit registers (addr < 16), data of the RDIMM register is driven A[ 3:0]
		*                                  address of the RDIMM register is driven A[ 7:4]
		* For 8-bit registers (addr > 16), address of the RDIMM register is driven A[12:8]
		*                                  data of the RDIMM register is driven on A[ 7:0]
		*/
		if (cfg->addrmap.row_b0_17[17] == AL_DDR_ADDRMAP_NC) {
			/* Set bit 3 in RC8 */
			err = al_ddr_mode_register_set(
				&cfg->ddr_cfg,
				((cfg->org.dimms == 2) ? 0x5 : 0x1),	/* handle both DIMMs */
				7,
				0x8 | (0x8 << 4));
			if (err) {
				al_err("%s: al_ddr_mode_register_set failed!\n", __func__);
				return err;
			}
		}

		if (cfg->org.udimm_addr_mirroring) {
			/* Set bit 3 in RC13 - enable address mirroring during MRS */
			err = al_ddr_mode_register_set(
				&cfg->ddr_cfg,
				((cfg->org.dimms == 2) ? 0x5 : 0x1),	/* handle both DIMMs */
				7,
				0x8 | (13 << 4));
			if (err) {
				al_err("%s: al_ddr_mode_register_set failed!\n", __func__);
				return err;
			}
		}

		al_reg_write32_masked(
			&ctrl_regs->mrctrl0,
			DWC_DDR_UMCTL2_REGS_MRCTRL0_RCD_INIT_EN,
			0);
	}

	al_ddr_ctrl_wait_for_normal_operating_mode(&cfg->ddr_cfg);

	al_reg_write32_masked(
		&ctrl_regs->pwrctl,
		(DWC_DDR_UMCTL2_REGS_PWRCTL_MPSM_EN |
		DWC_DDR_UMCTL2_REGS_PWRCTL_SELFREF_SW |
		DWC_DDR_UMCTL2_REGS_PWRCTL_SELFREF_EN),
		pwrctl);
	al_reg_write32_masked(
		&ctrl_regs->hwlpctl,
		DWC_DDR_UMCTL2_REGS_HWLPCTL_HW_LP_EN,
		hwlpctl);

	return 0;
}

static int al_ddr_phy_training_step_ext(
	struct al_ddr_init_cfg	*cfg,
	uint32_t		dtcr_mask,
	uint32_t		dtcr_val,
	uint32_t		pir_val,
	al_bool			check_status)
{
	STRUCT_PHY_REGS __iomem *phy_regs = PHY_REGS(cfg->ddr_cfg.ddr_phy_regs_base);

	al_reg_write32_masked(&phy_regs->dtcr[0], dtcr_mask, dtcr_val);
	al_reg_write32(&phy_regs->pir, pir_val);
	al_data_memory_barrier();
	return al_ddr_phy_wait_for_init_done_ext(phy_regs, check_status);
}

static int al_ddr_phy_training_step(
	struct al_ddr_init_cfg	*cfg,
	uint32_t		dtcr_mask,
	uint32_t		dtcr_val,
	uint32_t		pir_val)
{
	return al_ddr_phy_training_step_ext(cfg, dtcr_mask, dtcr_val, pir_val, AL_TRUE);
}

/*******************************************************************************
 ******************************************************************************/
static inline int al_ddr_phy_rmn_10651_wa(
	struct al_ddr_init_cfg	*cfg)
{
	int err = 0;

	STRUCT_PHY_REGS __iomem *phy_regs = PHY_REGS(cfg->ddr_cfg.ddr_phy_regs_base);

	if (cfg->org.ddr_type == AL_DDR_TYPE_DDR4) {
		al_reg_write32_masked(
			&phy_regs->dtcr[0],
			(ALPINE_V2_DWC_DDR_PHY_REGS_DTCR0_DTCMPD |
			ALPINE_V2_DWC_DDR_PHY_REGS_DTCR0_DTRDBITR_MASK),
			(ALPINE_V2_DWC_DDR_PHY_REGS_DTCR0_DTCMPD |
			ALPINE_V2_DWC_DDR_PHY_REGS_DTCR0_DTRDBITR_DIS));
		al_reg_write32_masked(
			&phy_regs->dtcr[1],
			(ALPINE_V2_DWC_DDR_PHY_REGS_DTCR1_RDPRMVL_TRN |
			 ALPINE_V2_DWC_DDR_PHY_REGS_DTCR1_BSTEN |
			 ALPINE_V2_DWC_DDR_PHY_REGS_DTCR1_RDLVLEN),
			(ALPINE_V2_DWC_DDR_PHY_REGS_DTCR1_RDPRMVL_TRN |
			 ALPINE_V2_DWC_DDR_PHY_REGS_DTCR1_BSTEN |
			 ALPINE_V2_DWC_DDR_PHY_REGS_DTCR1_RDLVLEN));
	} else if (cfg->org.ddr_type == AL_DDR_TYPE_DDR3) {
		/* Run basic gate training only */
		al_reg_write32_masked(
			&phy_regs->dtcr[0],
			ALPINE_V2_DWC_DDR_PHY_REGS_DTCR0_DTCMPD,
			ALPINE_V2_DWC_DDR_PHY_REGS_DTCR0_DTCMPD);
		al_reg_write32_masked(
			&phy_regs->dtcr[1],
			(ALPINE_V2_DWC_DDR_PHY_REGS_DTCR1_RDPRMVL_TRN |
			 ALPINE_V2_DWC_DDR_PHY_REGS_DTCR1_BSTEN |
			 ALPINE_V2_DWC_DDR_PHY_REGS_DTCR1_RDLVLEN),
			 ALPINE_V2_DWC_DDR_PHY_REGS_DTCR1_BSTEN);
	}

	al_reg_write32_masked(
		&phy_regs->dxccr,
		ALPINE_V2_DWC_DDR_PHY_REGS_DXCCR_QSCNTEN,
		0);

	err = al_ddr_phy_training_step(cfg, 0, 0,
		ALPINE_V2_DWC_DDR_PHY_REGS_PIR_INIT |
		ALPINE_V2_DWC_DDR_PHY_REGS_PIR_QSGATE);

	if (cfg->org.ddr_type == AL_DDR_TYPE_DDR3) {
		/* Run read leveling only */
		al_reg_write32_masked(
			&phy_regs->dtcr[1],
			(ALPINE_V2_DWC_DDR_PHY_REGS_DTCR1_RDPRMVL_TRN |
			 ALPINE_V2_DWC_DDR_PHY_REGS_DTCR1_BSTEN |
			 ALPINE_V2_DWC_DDR_PHY_REGS_DTCR1_RDLVLEN),
			 ALPINE_V2_DWC_DDR_PHY_REGS_DTCR1_RDLVLEN);
		err = al_ddr_phy_training_step(cfg, 0, 0,
			ALPINE_V2_DWC_DDR_PHY_REGS_PIR_INIT |
			ALPINE_V2_DWC_DDR_PHY_REGS_PIR_QSGATE);
	}

	al_reg_write32_masked(
		&phy_regs->dxccr,
		ALPINE_V2_DWC_DDR_PHY_REGS_DXCCR_QSCNTEN,
		ALPINE_V2_DWC_DDR_PHY_REGS_DXCCR_QSCNTEN);

	return err;
}


/*******************************************************************************
 ******************************************************************************/
static inline int al_ddr_phy_training_wla_wa(
	struct al_ddr_init_cfg	*cfg)
{
	int i, j;
	uint32_t reg_val;
	int err = 0;
	uint8_t		failed_byte_lanes[AL_DDR_PHY_NUM_BYTE_LANES];
	unsigned int	active_ranks = active_ranks_calc(cfg->org.dimms, cfg->org.ranks);
	STRUCT_PHY_REGS __iomem *phy_regs = PHY_REGS(cfg->ddr_cfg.ddr_phy_regs_base);

	al_dbg("Entered WLA WA\n");
	for (i = 0; i < AL_DDR_PHY_NUM_BYTE_LANES; i++) {
		failed_byte_lanes[i] = 0;
		if (!(cfg->calc.active_byte_lanes[i]))
			continue;
		reg_val = al_reg_read32(&phy_regs->datx8[i].rsr[0]);
		if (reg_val &
			(ALPINE_V2_DWC_DDR_PHY_REGS_DXNRSR0_QSGERR_MASK |
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNRSR0_X4QSGERR_MASK)) {
			failed_byte_lanes[i] = 1;
		}
		reg_val = al_reg_read32(&phy_regs->datx8[i].rsr[3]);
		if (reg_val &
			(ALPINE_V2_DWC_DDR_PHY_REGS_DXNRSR3_WLAERR_MASK |
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNRSR3_X4WLAERR_MASK)) {
			failed_byte_lanes[i] = 1;
		}
		if (failed_byte_lanes[i]) {
			al_dbg("Seeding byte %d Read BDL's\n", i);
			/* Seed Read BDL's */
			al_reg_write32(
				&phy_regs->datx8[i].bdlr3_5[0],
				(pre_seed << ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR3_DQ0RBD_SHIFT) |
				(pre_seed << ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR3_DQ1RBD_SHIFT) |
				(pre_seed << ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR3_DQ2RBD_SHIFT) |
				(pre_seed << ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR3_DQ3RBD_SHIFT));
			al_reg_write32(
				&phy_regs->datx8[i].bdlr3_5[1],
				(pre_seed << ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR4_DQ4RBD_SHIFT) |
				(pre_seed << ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR4_DQ5RBD_SHIFT) |
				(pre_seed << ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR4_DQ6RBD_SHIFT) |
				(pre_seed << ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR4_DQ7RBD_SHIFT));
			al_reg_write32(
				&phy_regs->datx8[i].bdlr3_5[2],
				(pre_seed << ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR5_DMRBD_SHIFT) |
				(pre_seed << ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR5_DSRBD_SHIFT) |
				(pre_seed << ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR5_DSNRBD_SHIFT));
		} else {
			/* Disable bytes which do not need to be re-trained */
			al_reg_write32_masked(
				&phy_regs->datx8[i].gcr[0],
				ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR0_DXEN,
				0);
		}
	}

	/* Reset PHY FIFO's */
	al_reg_write32_masked(
		&phy_regs->pgcr[0],
		ALPINE_V2_DWC_DDR_PHY_REGS_PGCR0_PHYFRST,
		0);
	al_udelay(1);	/* Requires 8 cycles delay before deasserting reset*/
	al_reg_write32_masked(
		&phy_regs->pgcr[0],
		ALPINE_V2_DWC_DDR_PHY_REGS_PGCR0_PHYFRST,
		ALPINE_V2_DWC_DDR_PHY_REGS_PGCR0_PHYFRST);

	/* Rerun Read DQS Gating Training */

	/*
	 * Addressing RMN: 10651
	 *
	 * RMN description:
	 * Read DQS gate training may result in non-ideal DQS gate placement
	 * Software flow:
	 * Disable QSCNTEN when running read DQS gate training.
	 * Enable data training data compare.
	 */
	err = al_ddr_phy_rmn_10651_wa(cfg);
	if (err)
		goto remove_bdl_seed;

	/* Rerun Write Leveling adjust Training */
	err = al_ddr_phy_training_step(cfg, 0, 0,
		ALPINE_V2_DWC_DDR_PHY_REGS_PIR_INIT |
		ALPINE_V2_DWC_DDR_PHY_REGS_PIR_WLADJ);

remove_bdl_seed:
	/* Remove BDL seed and resume disabled bytes */
	for (i = 0; i < AL_DDR_PHY_NUM_BYTE_LANES; i++) {
		uint8_t rdqsgd;
		uint8_t x4rdqsgd;
		uint8_t rdqsgd_extra;
		uint8_t x4rdqsgd_extra;
		unsigned int gdqsprd;

		if (!cfg->calc.active_byte_lanes[i])
			continue;
		if (!failed_byte_lanes[i]) {
			/* Re-enable byte */
			al_reg_write32_masked(
				&phy_regs->datx8[i].gcr[0],
				ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR0_DXEN,
				ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR0_DXEN);
			continue;
		}

		/* Remove Read BDL's seed*/
		al_reg_write32(
			&phy_regs->datx8[i].bdlr3_5[0],
			(0 << ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR3_DQ0RBD_SHIFT) |
			(0 << ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR3_DQ1RBD_SHIFT) |
			(0 << ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR3_DQ2RBD_SHIFT) |
			(0 << ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR3_DQ3RBD_SHIFT));
		al_reg_write32(
			&phy_regs->datx8[i].bdlr3_5[1],
			(0 << ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR4_DQ4RBD_SHIFT) |
			(0 << ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR4_DQ5RBD_SHIFT) |
			(0 << ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR4_DQ6RBD_SHIFT) |
			(0 << ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR4_DQ7RBD_SHIFT));
		al_reg_write32(
			&phy_regs->datx8[i].bdlr3_5[2],
			(0 << ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR5_DMRBD_SHIFT) |
			(0 << ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR5_DSRBD_SHIFT) |
			(0 << ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR5_DSNRBD_SHIFT));

		/* Adjust read gate */
		for (j = 0; j < AL_DDR_NUM_RANKS; j++) {
			if (!(AL_REG_BIT_GET(active_ranks, j)))
				continue;

			/* Select rank */
			al_reg_write32(
				&phy_regs->rankidr,
				(j << ALPINE_V2_DWC_DDR_PHY_REGS_RANKIDR_RANKWID_SHIFT) |
				(j << ALPINE_V2_DWC_DDR_PHY_REGS_RANKIDR_RANKRID_SHIFT));

			reg_val = al_reg_read32(
				&phy_regs->datx8[i].lcdlr[2]);
			rdqsgd =
				(reg_val &
				 ALPINE_V2_DWC_DDR_PHY_REGS_DXNLCDLR2_DQSGD_MASK) >>
				ALPINE_V2_DWC_DDR_PHY_REGS_DXNLCDLR2_DQSGD_SHIFT;
			x4rdqsgd =
				(reg_val &
				 ALPINE_V2_DWC_DDR_PHY_REGS_DXNLCDLR2_X4DQSGD_MASK) >>
				ALPINE_V2_DWC_DDR_PHY_REGS_DXNLCDLR2_X4DQSGD_SHIFT;

			reg_val = al_reg_read32(
				&phy_regs->datx8[i].gtr);

			rdqsgd_extra =
				(reg_val &
				 ALPINE_V2_DWC_DDR_PHY_REGS_DXNGTR0_DGSL_MASK) >>
				ALPINE_V2_DWC_DDR_PHY_REGS_DXNGTR0_DGSL_SHIFT;
			x4rdqsgd_extra =
				(reg_val &
				 ALPINE_V2_DWC_DDR_PHY_REGS_DXNGTR0_X4DGSL_MASK) >>
				ALPINE_V2_DWC_DDR_PHY_REGS_DXNGTR0_X4DGSL_SHIFT;

			reg_val = al_reg_read32(
				&phy_regs->datx8[i].gsr[0]);

			gdqsprd =
				(reg_val &
				 ALPINE_V2_DWC_DDR_PHY_REGS_DXNGSR0_GDQSPRD_MASK) >>
				ALPINE_V2_DWC_DDR_PHY_REGS_DXNGSR0_GDQSPRD_SHIFT;

			if (pre_seed <= rdqsgd) {
				rdqsgd -= pre_seed;

			} else {
				rdqsgd = gdqsprd - (pre_seed - rdqsgd);
				if (rdqsgd_extra == 0) {
					err = -EIO;
					al_err("Could not dec gate delay for octet %d\n", i);
				}
				rdqsgd_extra -= 1;
			}
			if (pre_seed <= x4rdqsgd) {
				x4rdqsgd -= pre_seed;
			} else {
				x4rdqsgd = gdqsprd - (pre_seed - x4rdqsgd);
				if (x4rdqsgd_extra == 0) {
					err = -EIO;
					al_err("Could not dec x4 gate delay for octet %d\n", i);
				}
				x4rdqsgd_extra -= 1;
			}

			al_reg_write32_masked(
				&phy_regs->datx8[i].lcdlr[2],
				ALPINE_V2_DWC_DDR_PHY_REGS_DXNLCDLR2_DQSGD_MASK,
				(rdqsgd <<
				 ALPINE_V2_DWC_DDR_PHY_REGS_DXNLCDLR2_DQSGD_SHIFT));
			al_reg_write32_masked(
				&phy_regs->datx8[i].gtr,
				ALPINE_V2_DWC_DDR_PHY_REGS_DXNGTR0_DGSL_MASK,
				(rdqsgd_extra <<
				 ALPINE_V2_DWC_DDR_PHY_REGS_DXNGTR0_DGSL_SHIFT));
			al_reg_write32_masked(
				&phy_regs->datx8[i].lcdlr[2],
				ALPINE_V2_DWC_DDR_PHY_REGS_DXNLCDLR2_X4DQSGD_MASK,
				(x4rdqsgd <<
				 ALPINE_V2_DWC_DDR_PHY_REGS_DXNLCDLR2_X4DQSGD_SHIFT));
			al_reg_write32_masked(
				&phy_regs->datx8[i].gtr,
				ALPINE_V2_DWC_DDR_PHY_REGS_DXNGTR0_X4DGSL_MASK,
				(x4rdqsgd_extra <<
				 ALPINE_V2_DWC_DDR_PHY_REGS_DXNGTR0_X4DGSL_SHIFT));
		}
	}

	return err;
}


static void al_ddr_phy_wa_rmn_11481(
	struct al_ddr_init_cfg	*cfg,
	unsigned int active_ranks)
{
	int rank, byte, side;
	uint32_t data;
	int num_of_sides = cfg->org.rdimm ? 2 : 1;
	static const unsigned int dxngcr6_dxdqverfr_mask[AL_DDR_NUM_RANKS] = {
		ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR6_DXDQVREFR0_MASK,
		ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR6_DXDQVREFR1_MASK,
		ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR6_DXDQVREFR2_MASK,
		ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR6_DXDQVREFR3_MASK};
	static const unsigned int dxngcr6_dxdqverfr_shift[AL_DDR_NUM_RANKS] = {
		ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR6_DXDQVREFR0_SHIFT,
		ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR6_DXDQVREFR1_SHIFT,
		ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR6_DXDQVREFR2_SHIFT,
		ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR6_DXDQVREFR3_SHIFT};
	STRUCT_PHY_REGS __iomem *phy_regs = PHY_REGS(cfg->ddr_cfg.ddr_phy_regs_base);
	/* Enter PUB mode */
	al_reg_write32_masked(&phy_regs->pgcr[1],
			      ALPINE_V2_DWC_DDR_PHY_REGS_PGCR1_PUBMODE,
			      ALPINE_V2_DWC_DDR_PHY_REGS_PGCR1_PUBMODE);
	for (rank = 0; rank < AL_DDR_NUM_RANKS; rank++) {
		if (!(AL_REG_BIT_GET(active_ranks, rank)))
				continue;
		/* Enter PDA mode by executing MR3 Set command using the Scheduler
		 * command registers*/
		/* Sent also side-B MRS for RDIMM */
		for (side = 0; side < num_of_sides; side++) {
			data = 0;
			AL_REG_FIELD_SET(data,
					 ALPINE_V2_DWC_DDR_PHY_REGS_SCHCR1_SCRNK_MASK,
					 ALPINE_V2_DWC_DDR_PHY_REGS_SCHCR1_SCRNK_SHIFT,
					 rank);
			AL_REG_FIELD_SET(data,
					 ALPINE_V2_DWC_DDR_PHY_REGS_SCHCR1_SCADDR_MASK,
					 ALPINE_V2_DWC_DDR_PHY_REGS_SCHCR1_SCADDR_SHIFT,
					 (cfg->calc.mr3 | 0x0010));
			AL_REG_FIELD_SET(data,
					 ALPINE_V2_DWC_DDR_PHY_REGS_SCHCR1_SCBG_MASK,
					 ALPINE_V2_DWC_DDR_PHY_REGS_SCHCR1_SCBG_SHIFT,
					 ((side == 0) ? 0x0 : 0x2));
			AL_REG_FIELD_SET(data,
					 ALPINE_V2_DWC_DDR_PHY_REGS_SCHCR1_SCBK_MASK,
					 ALPINE_V2_DWC_DDR_PHY_REGS_SCHCR1_SCBK_SHIFT,
					 0x3);
			al_reg_write32(&phy_regs->schcr[1], data);

			data = 0;
			AL_REG_FIELD_SET(data,
					 ALPINE_V2_DWC_DDR_PHY_REGS_SCHCR0_CMD_MASK,
					 ALPINE_V2_DWC_DDR_PHY_REGS_SCHCR0_CMD_SHIFT,
					 ALPINE_V2_DWC_DDR_PHY_REGS_SCHCR0_CMD_LOAD_MODE);
			AL_REG_FIELD_SET(data,
					 ALPINE_V2_DWC_DDR_PHY_REGS_SCHCR0_SCHTRIG_MASK,
					 ALPINE_V2_DWC_DDR_PHY_REGS_SCHCR0_SCHTRIG_SHIFT,
					 0x1);
			al_reg_write32(&phy_regs->schcr[0], data);
		}

		/* Should wait for command to be done (t_mrd) */
		al_udelay(1);

		for (side = 0; side < num_of_sides; side++) {
			for (byte = 0; byte < AL_DDR_PHY_NUM_BYTE_LANES; byte++) {
				/* Read Register DXnGCR6 to get Trained VREF value
				 * for current rank */
				int rd_reg = al_reg_read32(&phy_regs->datx8[byte].gcr[6]);
				int vref_mask = dxngcr6_dxdqverfr_mask[rank];
				int vref_shift = dxngcr6_dxdqverfr_shift[rank];
				int vref;

				vref = AL_REG_FIELD_GET(rd_reg, vref_mask, vref_shift);

				/* Write the vref value to the DRAM using the scheduler
				 * command registers */
				data = 0;
				AL_REG_FIELD_SET(data,
					 ALPINE_V2_DWC_DDR_PHY_REGS_SCHCR1_SCRNK_MASK,
					 ALPINE_V2_DWC_DDR_PHY_REGS_SCHCR1_SCRNK_SHIFT,
					 rank);
				AL_REG_FIELD_SET(data,
					 ALPINE_V2_DWC_DDR_PHY_REGS_SCHCR1_SCADDR_MASK,
					 ALPINE_V2_DWC_DDR_PHY_REGS_SCHCR1_SCADDR_SHIFT,
					 ((cfg->calc.mr6 & 0xFFC0) | vref));
				AL_REG_FIELD_SET(data,
					 ALPINE_V2_DWC_DDR_PHY_REGS_SCHCR1_SCBG_MASK,
					 ALPINE_V2_DWC_DDR_PHY_REGS_SCHCR1_SCBG_SHIFT,
					 ((side == 0) ? 0x1 : 0x3));
				AL_REG_FIELD_SET(data,
					 ALPINE_V2_DWC_DDR_PHY_REGS_SCHCR1_SCBK_MASK,
					 ALPINE_V2_DWC_DDR_PHY_REGS_SCHCR1_SCBK_SHIFT,
					 0x2);
				al_reg_write32(&phy_regs->schcr[1], data);

				data = 0;
				AL_REG_FIELD_SET(data,
					 ALPINE_V2_DWC_DDR_PHY_REGS_SCHCR0_CMD_MASK,
					 ALPINE_V2_DWC_DDR_PHY_REGS_SCHCR0_CMD_SHIFT,
					 ALPINE_V2_DWC_DDR_PHY_REGS_SCHCR0_CMD_LOAD_MODE);
				AL_REG_FIELD_SET(data,
					 ALPINE_V2_DWC_DDR_PHY_REGS_SCHCR0_SCHTRIG_MASK,
					 ALPINE_V2_DWC_DDR_PHY_REGS_SCHCR0_SCHTRIG_SHIFT,
					 0x1);
				AL_REG_FIELD_SET(data,
					 ALPINE_V2_DWC_DDR_PHY_REGS_SCHCR0_SCHDQV_MASK,
					 ALPINE_V2_DWC_DDR_PHY_REGS_SCHCR0_SCHDQV_SHIFT,
					 (0x1FF & ~(AL_BIT(byte))));
				al_reg_write32(&phy_regs->schcr[0], data);

				al_udelay(1);
			}
		}
		/* Exit PDA mode for current rank */
		for (side = 0; side < num_of_sides; side++) {
			data = 0;
			AL_REG_FIELD_SET(data,
				 ALPINE_V2_DWC_DDR_PHY_REGS_SCHCR1_SCRNK_MASK,
				 ALPINE_V2_DWC_DDR_PHY_REGS_SCHCR1_SCRNK_SHIFT,
				 rank);
			AL_REG_FIELD_SET(data,
				 ALPINE_V2_DWC_DDR_PHY_REGS_SCHCR1_SCADDR_MASK,
				 ALPINE_V2_DWC_DDR_PHY_REGS_SCHCR1_SCADDR_SHIFT,
				 (cfg->calc.mr3 & ~(AL_BIT(4))));
			AL_REG_FIELD_SET(data,
				 ALPINE_V2_DWC_DDR_PHY_REGS_SCHCR1_SCBG_MASK,
				 ALPINE_V2_DWC_DDR_PHY_REGS_SCHCR1_SCBG_SHIFT,
				 ((side == 0) ? 0x0 : 0x2));
			AL_REG_FIELD_SET(data,
				 ALPINE_V2_DWC_DDR_PHY_REGS_SCHCR1_SCBK_MASK,
				 ALPINE_V2_DWC_DDR_PHY_REGS_SCHCR1_SCBK_SHIFT,
				 0x3);
			al_reg_write32(&phy_regs->schcr[1], data);

			data = 0;
			AL_REG_FIELD_SET(data,
				 ALPINE_V2_DWC_DDR_PHY_REGS_SCHCR0_CMD_MASK,
				 ALPINE_V2_DWC_DDR_PHY_REGS_SCHCR0_CMD_SHIFT,
				 ALPINE_V2_DWC_DDR_PHY_REGS_SCHCR0_CMD_LOAD_MODE);
			AL_REG_FIELD_SET(data,
				 ALPINE_V2_DWC_DDR_PHY_REGS_SCHCR0_SCHTRIG_MASK,
				 ALPINE_V2_DWC_DDR_PHY_REGS_SCHCR0_SCHTRIG_SHIFT,
				 0x1);
			al_reg_write32(&phy_regs->schcr[0], data);

			al_udelay(1);
		}
	}
	al_reg_write32_masked(&phy_regs->pgcr[1],
			      ALPINE_V2_DWC_DDR_PHY_REGS_PGCR1_PUBMODE,
			      0);

	/* TODO : should add check here that vref was set correctly using DCU */
}

/*******************************************************************************
 ******************************************************************************/
static inline int al_ddr_phy_training(
	struct al_ddr_init_cfg	*cfg)
{
	int i;
	int err = 0;
	uint32_t reg_val;
	unsigned int active_ranks = active_ranks_calc(cfg->org.dimms, cfg->org.ranks);
	int j;
	unsigned int dtrank;
	unsigned int rddly;
	unsigned int rddly_x4;
	unsigned int rddly_max[AL_DDR_PHY_NUM_BYTE_LANES] = { 0 };
	unsigned int rddly_x4_max[AL_DDR_PHY_NUM_BYTE_LANES] = { 0 };

	STRUCT_PHY_REGS __iomem *phy_regs = PHY_REGS(cfg->ddr_cfg.ddr_phy_regs_base);

	al_ddr_ctrl_stop(&cfg->ddr_cfg);

	al_ddr_phy_vt_calc_disable(&cfg->ddr_cfg);

	/*
	* Addressing RMN: 6951
	*
	* RMN description:
	* Trainings fail due to Q Valid bubbles
	* Software flow:
	* Run the following training: Write Leveling, Gate Training and Write Level Adjustment
	* Step 2. If a failure is encountered in step 1, use the Read BDL delay lines to move
	*         the incoming DQS clocks away from alignment with the internal FIFO read clock
	*         during gate training and Write Leveling Adjust (WLA).
	* Step 3. Run all subsequent training steps in Static Read Mode which bypasses the issue
	*         with staggering and bubbles in the data valid signals.
	*/

	/*********** Write Leveling Training ***********/
	err = al_ddr_phy_training_step(cfg, 0, 0,
		ALPINE_V2_DWC_DDR_PHY_REGS_PIR_INIT |
		ALPINE_V2_DWC_DDR_PHY_REGS_PIR_WL);

	/*********** Read DQS Gating Training ***********/
	/*
	* Addressing RMN: 11048
	*
	* RMN description:
	* When Read DBI is used, PHY initialization may not complete during
	* Read Bit Deskew Training
	* Software flow:
	* Read DBI deskewing must be disabled by setting DTCR0.DTRDBITR to 2'b00 at all times.
	* It will also be necessary to set an appropriate value in DXnBDLR5[5:0].DMRBD,
	* following the training, since the training FSM may alter this field when it is run
	* with the Read DBI deskewing dis abled. In our case, need to clear DMRBD after training.
	*/

	/*
	* Addressing RMN: 11656
	*
	* RMN description:
	* BL / CL Settings Changed after Read DBI Training if run with Read DBI Not Enabled
	* Software flow:
	* Make sure not to enable DTCR0.DTRDBITR while mr5.RDBI is disable.
	*
	* NOTE: In our case DTCR0.DTRDBITR always disable due to RMN 11048
	*/

	/*
	* Addressing RMN: 11198
	*
	* RMN description:
	* DDR PHY STAR 9001111914: Read/Write bit deskew algorithm may not complete if
	* triggered a second (or more) time
	* Read Bit Deskew Training
	* Software flow:
	* The only way to re-training is to run the whole ddr_init sequence which already
	* reset the phy.
	*/

	if (cfg->org.ddr_type == AL_DDR_TYPE_DDR4) {
		al_reg_write32_masked(
			&phy_regs->dtcr[0],
			ALPINE_V2_DWC_DDR_PHY_REGS_DTCR0_DTRDBITR_MASK,
			ALPINE_V2_DWC_DDR_PHY_REGS_DTCR0_DTRDBITR_DIS);
	}

	/*
	 * Addressing RMN: 10651
	 *
	 * RMN description:
	 * Read DQS gate training may result in non-ideal DQS gate placement
	 * Software flow:
	 * Disable QSCNTEN when running read DQS gate training.
	 * Enable data training data compare.
	 */
	err = al_ddr_phy_rmn_10651_wa(cfg);
	if (err) {
		reg_val = al_reg_read32(&phy_regs->pgsr[0]);
		if (reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_PGSR0_QSGERR) {
			err = al_ddr_phy_training_wla_wa(cfg);
			if (err)
				goto done;
		}
	}

	err = al_ddr_phy_training_step(cfg, 0, 0,
		ALPINE_V2_DWC_DDR_PHY_REGS_PIR_INIT |
		ALPINE_V2_DWC_DDR_PHY_REGS_PIR_WLADJ);

	if (err) {
		reg_val = al_reg_read32(&phy_regs->pgsr[0]);
		if (reg_val &
			(ALPINE_V2_DWC_DDR_PHY_REGS_PGSR0_QSGERR |
			ALPINE_V2_DWC_DDR_PHY_REGS_PGSR0_WLAERR))
			err = al_ddr_phy_training_wla_wa(cfg);
	}

	if (err)
		goto done;

	/*********** Static Read Training ***********/
	/* Must enable static read mode before launching static read training */
	al_reg_write32_masked(
		&phy_regs->pgcr[0],
		ALPINE_V2_DWC_DDR_PHY_REGS_PGCR0_PHYFRST,
		0);

	al_reg_write32_masked(
		&phy_regs->pgcr[3],
		ALPINE_V2_DWC_DDR_PHY_REGS_PGCR3_RDMODE_MASK,
		ALPINE_V2_DWC_DDR_PHY_REGS_PGCR3_RDMODE_STATIC_RD_RSP);

	al_reg_write32_masked(
		&phy_regs->pgcr[0],
		ALPINE_V2_DWC_DDR_PHY_REGS_PGCR0_PHYFRST,
		ALPINE_V2_DWC_DDR_PHY_REGS_PGCR0_PHYFRST);

	/* Store original rank to train */
	reg_val = al_reg_read32(&phy_regs->dtcr[1]);
	dtrank = (reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DTCR1_DTRANK_MASK) >>
			ALPINE_V2_DWC_DDR_PHY_REGS_DTCR1_DTRANK_SHIFT;

	/*
	* Run static training per rank as current design only run for a single rank
	* Take Maximum value as it represent the max possible delay before
	* the read FIFO should be popped.
	*/
	for (j = 0; j < AL_DDR_NUM_RANKS; j++) {
		if (!(AL_REG_BIT_GET(active_ranks, j)))
			continue;

		/* Set DTRANK to set the rank to be trained */
		al_reg_write32_masked(
			&phy_regs->dtcr[1],
			ALPINE_V2_DWC_DDR_PHY_REGS_DTCR1_DTRANK_MASK,
			(j << ALPINE_V2_DWC_DDR_PHY_REGS_DTCR1_DTRANK_SHIFT));

		err = al_ddr_phy_training_step(cfg, 0, 0,
			ALPINE_V2_DWC_DDR_PHY_REGS_PIR_INIT |
			ALPINE_V2_DWC_DDR_PHY_REGS_PIR_SRD);
		if (err)
			goto after_static_train;

		/* take the maximum for all ranks per byte */
		for (i = 0; i < AL_DDR_PHY_NUM_BYTE_LANES; i++) {
			if (!(cfg->calc.active_byte_lanes[i]))
				continue;

			reg_val = al_reg_read32(&phy_regs->datx8[i].gcr[0]);
			rddly = (reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR0_RDDLY_MASK) >>
					ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR0_RDDLY_SHIFT;
			if (rddly > rddly_max[i])
				rddly_max[i] = rddly;

			reg_val = al_reg_read32(&phy_regs->datx8[i].gcr[7]);
			rddly_x4 = (reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR7_X4RDDLY_MASK) >>
					ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR7_X4RDDLY_SHIFT;
			if (rddly_x4 > rddly_x4_max[i])
				rddly_x4_max[i] = rddly_x4;
		}
	}

	/* Set maximum value to registers */
	for (i = 0; i < AL_DDR_PHY_NUM_BYTE_LANES; i++) {
		if (!(cfg->calc.active_byte_lanes[i]))
			continue;

		al_reg_write32_masked(
			&phy_regs->datx8[i].gcr[0],
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR0_RDDLY_MASK,
			(rddly_max[i] << ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR0_RDDLY_SHIFT));
		al_reg_write32_masked(
			&phy_regs->datx8[i].gcr[7],
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR7_X4RDDLY_MASK,
			(rddly_x4_max[i] << ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR7_X4RDDLY_SHIFT));
	}

after_static_train:

	/* Resume DTRANK */
	al_reg_write32_masked(
		&phy_regs->dtcr[1],
		ALPINE_V2_DWC_DDR_PHY_REGS_DTCR1_DTRANK_MASK,
		(dtrank << ALPINE_V2_DWC_DDR_PHY_REGS_DTCR1_DTRANK_SHIFT));

	/* Remain in static read mode for next training steps */
	if (err)
		goto done;

	/*********** Bit Deskew/Eye Centering Training ***********/
	err = al_ddr_phy_training_step(cfg, 0, 0,
		ALPINE_V2_DWC_DDR_PHY_REGS_PIR_INIT |
		ALPINE_V2_DWC_DDR_PHY_REGS_PIR_RDDSKW |
		ALPINE_V2_DWC_DDR_PHY_REGS_PIR_WRDSKW |
		ALPINE_V2_DWC_DDR_PHY_REGS_PIR_RDEYE |
		ALPINE_V2_DWC_DDR_PHY_REGS_PIR_WREYE);

	if (err)
		goto done;

	/*********** VREF Training ***********/
	if (cfg->org.ddr_type == AL_DDR_TYPE_DDR4) {
		unsigned int rfshdt;
		/*
		* Addressing RMN: 6012
		*
		* RMN description:
		* DDR4 VREF training issue when using certain BIST data patterns
		* Software flow:
		* Setup the BIST to use user programmable data pattern (BDPAT set to 11)
		* prior to running VREF training
		*/
		/* BISTRR */
		al_reg_write32_masked(
			&phy_regs->bistrr,
			ALPINE_V2_DWC_DDR_PHY_REGS_BISTRR_BDPAT_MASK,
			ALPINE_V2_DWC_DDR_PHY_REGS_BISTRR_BDPAT_USER);

		/* BISTUDPR */
		al_reg_write32_masked(
			&phy_regs->bistudpr,
			ALPINE_V2_DWC_DDR_PHY_REGS_BISTUDPR_BUDP0_MASK |
			ALPINE_V2_DWC_DDR_PHY_REGS_BISTUDPR_BUDP1_MASK,
			ALPINE_V2_DWC_DDR_PHY_REGS_BISTUDPR_BUDP0(0xA5A5) |
			ALPINE_V2_DWC_DDR_PHY_REGS_BISTUDPR_BUDP1(0xA5A5));

		/* BISTAR1 */
		al_reg_write32_masked(
			&phy_regs->bistar[1],
			ALPINE_V2_DWC_DDR_PHY_REGS_BISTAR1_BAINC_MASK,
			(0x8 << ALPINE_V2_DWC_DDR_PHY_REGS_BISTAR1_BAINC_SHIFT));

		/*
		 * Addressing RMN: 11481
		 *
		 * RMN description:
		 * DRAM Vref training might fail when DATX4X2 is in x8 mode and PDA mode.
		 *
		 * Software flow:
		 * 1. Run DRAM vref training
		 * 2. Set final results using scheduler commands
		 * 3. Run HOST vref training
		 */
		/*  Run DRAM vref training */
		al_reg_write32_masked(&phy_regs->vtcr[0],
		      ALPINE_V2_DWC_DDR_PHY_REGS_VTCR0_DVEN,
		      ALPINE_V2_DWC_DDR_PHY_REGS_VTCR0_DVEN);
		al_reg_write32_masked(&phy_regs->vtcr[1],
		      ALPINE_V2_DWC_DDR_PHY_REGS_VTCR1_HVEN,
		      0);

		/*
		* Addressing RMN: 5720
		*
		* RMN description:
		* VREF training hangs when servicing refresh request
		* Software flow:
		* Disable refreshes when running VREF training
		*/
		rfshdt = AL_REG_FIELD_GET(al_reg_read32(&phy_regs->dtcr[0]),
					ALPINE_V2_DWC_DDR_PHY_REGS_DTCR0_RFSHDT_MASK,
					ALPINE_V2_DWC_DDR_PHY_REGS_DTCR0_RFSHDT_SHIFT);

		err = al_ddr_phy_training_step_ext(
			cfg,
			ALPINE_V2_DWC_DDR_PHY_REGS_DTCR0_RFSHDT_MASK,
			(0x0 << ALPINE_V2_DWC_DDR_PHY_REGS_DTCR0_RFSHDT_SHIFT),
			ALPINE_V2_DWC_DDR_PHY_REGS_PIR_INIT |
			ALPINE_V2_DWC_DDR_PHY_REGS_PIR_VREF,
			(((cfg->org.ddr_device == AL_DDR_DEVICE_X8) ||
			  (cfg->org.ddr_device == AL_DDR_DEVICE_X16)) ? AL_FALSE : AL_TRUE));

		if ((cfg->org.ddr_device == AL_DDR_DEVICE_X8) ||
		    (cfg->org.ddr_device == AL_DDR_DEVICE_X16))
			al_ddr_phy_wa_rmn_11481(cfg, active_ranks);

		/* Run HOST VREF training */
		al_reg_write32_masked(&phy_regs->vtcr[0],
			ALPINE_V2_DWC_DDR_PHY_REGS_VTCR0_DVEN,
			0);
		al_reg_write32_masked(&phy_regs->vtcr[1],
			ALPINE_V2_DWC_DDR_PHY_REGS_VTCR1_HVEN,
			ALPINE_V2_DWC_DDR_PHY_REGS_VTCR1_HVEN);
		err = al_ddr_phy_training_step(cfg, 0, 0,
			ALPINE_V2_DWC_DDR_PHY_REGS_PIR_INIT |
			ALPINE_V2_DWC_DDR_PHY_REGS_PIR_VREF);


		/* Re-enable refreshes */
		al_reg_write32_masked(&phy_regs->dtcr[0],
			ALPINE_V2_DWC_DDR_PHY_REGS_DTCR0_RFSHDT_MASK,
			(rfshdt << ALPINE_V2_DWC_DDR_PHY_REGS_DTCR0_RFSHDT_SHIFT));

		if (err)
			goto done;
	}

done:

	/*
	 * Addressing RMN: 11048
	 *
	 * RMN description:
	 * When Read DBI is used, PHY initialization may not complete during
	 * Read Bit Deskew Training
	 * Software flow:
	 * Read DBI deskewing must be disabled by setting DTCR0.DTRDBITR to 2'b00 at all times.
	 * It will also be necessary to set an appropriate value in DXnBDLR5[5:0].DMRBD,
	 * following the training, since the training FSM may alter this field when it is run
	 * with the Read DBI deskewing dis abled. In our case, need to clear DMRBD after training.
	 */
	for (j = 0; j < AL_DDR_NUM_RANKS; j++) {
		if (!(AL_REG_BIT_GET(active_ranks, j)))
			continue;

		/* Select rank */
		al_reg_write32(
			&phy_regs->rankidr,
			(j << ALPINE_V2_DWC_DDR_PHY_REGS_RANKIDR_RANKWID_SHIFT) |
			(j << ALPINE_V2_DWC_DDR_PHY_REGS_RANKIDR_RANKRID_SHIFT));
		al_reg_write32_masked(
			&phy_regs->dtcr[0],
			ALPINE_V2_DWC_DDR_PHY_REGS_DTCR0_DTDRS_MASK,
			(j << ALPINE_V2_DWC_DDR_PHY_REGS_DTCR0_DTDRS_SHIFT));

		for (i = 0; i < AL_DDR_PHY_NUM_BYTE_LANES; i++) {
			al_reg_write32_masked(
				&phy_regs->datx8[i].bdlr3_5[2],
				ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR5_DMRBD_MASK,
				(0 << ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR5_DMRBD_SHIFT));
		}
	}

	if (err)
		al_ddr_phy_training_failure_print(cfg);

	al_ddr_phy_vt_calc_enable(&cfg->ddr_cfg);

	al_ddr_ctrl_resume(&cfg->ddr_cfg);

	return err;
}

/*******************************************************************************
 ******************************************************************************/
static inline int al_ddr_phy_run(
	struct al_ddr_init_cfg	*cfg)
{
	uint32_t reg_val;

	STRUCT_PHY_REGS __iomem *phy_regs = PHY_REGS(cfg->ddr_cfg.ddr_phy_regs_base);

	/* DSGCR -  */
	reg_val = 0x0;

	/*
	* Addressing RMN: 4401
	*
	* RMN description:
	* Controller may Deassert dfi_phyupd_ack Early due to dfi_ctrlupd_req
	* Software flow:
	* Do not enable controller initiated updates and phy initiated updates
	* at the same time
	*/
	/*
	* Addressing RMN: 5008
	*
	* RMN description:
	* I/O update issue in certain configurations
	* Software flow:
	* Do not allow the following configuration :
	* DSGCR.PUREN=1
	* DSGCR.CTLZUEN=1
	* DSGCR.PHYZUEN=0
	*/
	if (dis_auto_ctrlupd)
		reg_val |= ALPINE_V2_DWC_DDR_PHY_REGS_DSGCR_PUREN |
				ALPINE_V2_DWC_DDR_PHY_REGS_DSGCR_PHYZUEN;
	else
		reg_val |= ALPINE_V2_DWC_DDR_PHY_REGS_DSGCR_CTLZUEN;
	if (rr_mode)
		reg_val |= ALPINE_V2_DWC_DDR_PHY_REGS_DSGCR_WRRMODE |
				ALPINE_V2_DWC_DDR_PHY_REGS_DSGCR_RRRMODE;
	reg_val |= ALPINE_V2_DWC_DDR_PHY_REGS_DSGCR_CUAEN;
	reg_val |= ALPINE_V2_DWC_DDR_PHY_REGS_DSGCR_SDRMODE_HDR;

	al_reg_write32_masked(
		&phy_regs->dsgcr,
		ALPINE_V2_DWC_DDR_PHY_REGS_DSGCR_PUREN |
		ALPINE_V2_DWC_DDR_PHY_REGS_DSGCR_CTLZUEN |
		ALPINE_V2_DWC_DDR_PHY_REGS_DSGCR_CUAEN |
		ALPINE_V2_DWC_DDR_PHY_REGS_DSGCR_WRRMODE |
		ALPINE_V2_DWC_DDR_PHY_REGS_DSGCR_SDRMODE_MASK |
		ALPINE_V2_DWC_DDR_PHY_REGS_DSGCR_RRRMODE |
		ALPINE_V2_DWC_DDR_PHY_REGS_DSGCR_PHYZUEN,
		reg_val);

	/* Change read mode */
	al_reg_write32_masked(
		&phy_regs->pgcr[0],
		ALPINE_V2_DWC_DDR_PHY_REGS_PGCR0_PHYFRST,
		0);

	al_reg_write32_masked(
		&phy_regs->pgcr[3],
		ALPINE_V2_DWC_DDR_PHY_REGS_PGCR3_RDMODE_MASK,
		phy_rdmode);

	al_reg_write32_masked(
		&phy_regs->pgcr[0],
		ALPINE_V2_DWC_DDR_PHY_REGS_PGCR0_PHYFRST,
		ALPINE_V2_DWC_DDR_PHY_REGS_PGCR0_PHYFRST);


	return 0;
}

/*******************************************************************************
 ******************************************************************************/
static inline int al_ddr_ctrl_run(
	struct al_ddr_init_cfg	*cfg
	)
{
	STRUCT_CTRL_MP_REGS __iomem *ctrl_mp_regs = CTRL_MP_REGS(cfg->ddr_cfg.ddr_ctrl_regs_base);

	/* Enable DDR controller AXI port */
	al_reg_write32(
		&ctrl_mp_regs->pctrl_0,
		DWC_DDR_UMCTL2_MP_PCTRL_0_PORT_EN);

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
static inline int al_ddr_phy_unused_dq_pd(
	struct al_ddr_init_cfg	*cfg)
{
	int i;

	STRUCT_PHY_REGS __iomem *phy_regs = PHY_REGS(cfg->ddr_cfg.ddr_phy_regs_base);

	for (i = 0; i < AL_DDR_PHY_NUM_BYTE_LANES; i++) {
		if (cfg->calc.active_byte_lanes[i])
			continue;

		/* Power down lane */
		al_reg_write32_masked(
			&phy_regs->datx8[i].gcr[0],
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR0_DXEN |
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR0_DQSGOE |
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR0_DQSGPDD |
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR0_DQSGPDR |
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR0_DQSRPD |
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR0_PLLPD |
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR0_MDLEN |
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR0_CALBYP,
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR0_DQSGPDD |
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR0_DQSGPDR |
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR0_DQSRPD |
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR0_PLLPD |
			ALPINE_V2_DWC_DDR_PHY_REGS_DXNGCR0_CALBYP);

	}

	al_data_memory_barrier();

	/**
	 * Disbale and re-enable DDL calibration so calibration FSM will
	 * record which lanes were disabled
	*/
	al_reg_write32_masked(
		&phy_regs->pir,
		ALPINE_V2_DWC_DDR_PHY_REGS_PIR_DCALPSE,
		ALPINE_V2_DWC_DDR_PHY_REGS_PIR_DCALPSE);
	al_reg_write32_masked(
		&phy_regs->pir,
		ALPINE_V2_DWC_DDR_PHY_REGS_PIR_DCALPSE,
		0);

	/* PGCR3 */
	al_reg_write32_masked(
		&phy_regs->pgcr[3],
		ALPINE_V2_DWC_DDR_PHY_REGS_PGCR3_GATEDXCTLCLK |
		ALPINE_V2_DWC_DDR_PHY_REGS_PGCR3_GATEDXDDRCLK |
		ALPINE_V2_DWC_DDR_PHY_REGS_PGCR3_GATEDXRDCLK,
		ALPINE_V2_DWC_DDR_PHY_REGS_PGCR3_GATEDXCTLCLK |
		ALPINE_V2_DWC_DDR_PHY_REGS_PGCR3_GATEDXDDRCLK |
		ALPINE_V2_DWC_DDR_PHY_REGS_PGCR3_GATEDXRDCLK);

	/* TODO : maybe disable unused ranks? PGCR3.CKEN */

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
static unsigned int al_ddr_div2_rnd_up(
	unsigned int value)
{
	return (value + 1) / 2;
}

static unsigned int al_rd_dbi_extra_cas_lat_get(
	unsigned int clk_freq_mhz)
{
	return clk_freq_mhz > 934 ? 3 : 2;
}

/*******************************************************************************
 ******************************************************************************/
static unsigned int al_ddr_ns_to_ck_rnd_up(
	unsigned int clk_freq_mhz,
	unsigned int ns)
{
	return (((ns) * (clk_freq_mhz)) + 999) / 1000;
}

/*******************************************************************************
 ******************************************************************************/
static unsigned int al_ddr_ns_to_ck_rnd_dn(
	unsigned int clk_freq_mhz,
	unsigned int ns)
{
	return ((ns) * (clk_freq_mhz)) / 1000;
}

/*******************************************************************************
 ******************************************************************************/
static unsigned int al_ddr_max_ck_ns(
	unsigned int clk_freq_mhz,
	unsigned int ck,
	unsigned int ns)
{
	return (al_ddr_ns_to_ck_rnd_up(clk_freq_mhz, ns) > (ck) ?
	(al_ddr_ns_to_ck_rnd_up(clk_freq_mhz, ns)) : (ck));
}

/*******************************************************************************
 ******************************************************************************/
static unsigned int al_ddr_ns_to_ck_x1024_rnd_up(
	unsigned int clk_freq_mhz,
	unsigned int ns)
{
	return (al_ddr_ns_to_ck_rnd_up(clk_freq_mhz, ns) + 1023) / 1024;
}

/*******************************************************************************
 ******************************************************************************/
static unsigned int al_ddr_ns_to_ck_x1024_rnd_dn(
	unsigned int clk_freq_mhz,
	unsigned int ns)
{
	return ((ns) * (clk_freq_mhz)) / (1000 * 1024);
}

/*******************************************************************************
 ******************************************************************************/
static unsigned int al_ddr_ps_to_ck_rnd_up(
	unsigned int clk_freq_mhz,
	unsigned int ps)
{
	return (((ps) * (clk_freq_mhz)) + 999999) / 1000000;
}

/*******************************************************************************
 ******************************************************************************/
static unsigned int al_ddr_max_ck_ps(
	unsigned int clk_freq_mhz,
	unsigned int ck,
	unsigned int ps)
{
	return (al_ddr_ps_to_ck_rnd_up(clk_freq_mhz, ps) > (ck) ?
	(al_ddr_ps_to_ck_rnd_up(clk_freq_mhz, ps)) : (ck));
}

/*******************************************************************************
 ******************************************************************************/

/*******************************************************************************
 ******************************************************************************/
static inline unsigned int al_ddr_max_ck_ns_x32_rnd_up(
	unsigned int clk_freq_mhz,
	unsigned int ck,
	unsigned int ns)
{
	return ((al_ddr_max_ck_ns(clk_freq_mhz, ck, ns) + 31) / 32) + 1;
}

/*******************************************************************************
 ******************************************************************************/
static inline unsigned int al_ddr_ns_to_ck_x32_rnd_up(
	unsigned int clk_freq_mhz,
	unsigned int ns)
{
	return ((al_ddr_ns_to_ck_rnd_up(clk_freq_mhz, ns) + 31) / 32) + 1;
}

/*******************************************************************************
 ******************************************************************************/
static inline unsigned int al_ddr_ps_to_ck_x32_rnd_up(
	unsigned int clk_freq_mhz,
	unsigned int ps)
{
	return ((al_ddr_ps_to_ck_rnd_up(clk_freq_mhz, ps) + 31) / 32) + 1;
}

/*******************************************************************************
 ******************************************************************************/
static inline unsigned int active_ranks_calc(
	unsigned int dimms,
	unsigned int ranks)
{

	return ((dimms == 2) && (ranks == 2)) ? 0x5 : (1 << ranks) - 1;

}


/*******************************************************************************
 ******************************************************************************/
static inline unsigned int active_ranks_calc_phy(
	unsigned int active_ranks_no_cid)
{

	unsigned int active_ranks = active_ranks_no_cid;


	return active_ranks;
}

/*******************************************************************************
 ******************************************************************************/
static int al_ddr_phy_wait_for_init_done_ext(
	STRUCT_PHY_REGS __iomem *phy_regs,
	al_bool check_status)
{
	uint32_t reg_val;
	int err;
	int i;

	al_udelay(1);

	err = al_ddr_reg_poll32(
		&phy_regs->pgsr[0],
		ALPINE_V2_DWC_DDR_PHY_REGS_PGSR0_IDONE,
		ALPINE_V2_DWC_DDR_PHY_REGS_PGSR0_IDONE,
		DEFAULT_TIMEOUT);

	if (err) {
		al_err("%s: poll failed!\n", __func__);
		return err;
	}

	al_udelay(1);

	if (!check_status)
		return 0;

	reg_val = al_reg_read32(&phy_regs->pgsr[0]);

	/* Make sure no init errors */
	if (reg_val &
		(ALPINE_V2_DWC_DDR_PHY_REGS_PGSR0_VERR |
		ALPINE_V2_DWC_DDR_PHY_REGS_PGSR0_ZCERR |
		ALPINE_V2_DWC_DDR_PHY_REGS_PGSR0_WLERR |
		ALPINE_V2_DWC_DDR_PHY_REGS_PGSR0_QSGERR |
		ALPINE_V2_DWC_DDR_PHY_REGS_PGSR0_WLAERR |
		ALPINE_V2_DWC_DDR_PHY_REGS_PGSR0_RDERR |
		ALPINE_V2_DWC_DDR_PHY_REGS_PGSR0_WDERR |
		ALPINE_V2_DWC_DDR_PHY_REGS_PGSR0_REERR |
		ALPINE_V2_DWC_DDR_PHY_REGS_PGSR0_WEERR |
		ALPINE_V2_DWC_DDR_PHY_REGS_PGSR0_SRDERR)) {
#if (!defined(AL_DDR_PHY_INIT_ERR_MSG_SURPRESS)) || (AL_DDR_PHY_INIT_ERR_MSG_SURPRESS == 0)
		al_err("PHY init error (%X)!\n", reg_val);
#else
		al_dbg("PHY init error (%X)!\n", reg_val);
#endif
		return -EIO;
	}

	/**
	 * Addressing RMN: 10654
	 *
	 * RMN description:
	 * PGCR0.ZCERR does not reflect status from all ZCTRL segments
	 * Software flow:
	 * ZQnSR.ZERR can be used instead of PGSR0.ZCERR
	 */
	for (i = 0; i < AL_DDR_PHY_NUM_ZQ_SEGMENTS_ALPINE_V2; i++) {
		reg_val = al_reg_read32(&phy_regs->zq[i].sr);
		if (reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_ZQnSR_ZERR) {
#if (!defined(AL_DDR_PHY_INIT_ERR_MSG_SURPRESS)) || (AL_DDR_PHY_INIT_ERR_MSG_SURPRESS == 0)
			al_err("PHY ZQ[%X].SR init error (%X)!\n", i, reg_val);
#else
			al_dbg("PHY ZQ[%X].SR init error (%X)!\n", i, reg_val);
#endif
			return -EIO;
		}
	}

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
static int al_ddr_phy_wait_for_init_done(
	STRUCT_PHY_REGS __iomem *phy_regs)
{
	return al_ddr_phy_wait_for_init_done_ext(phy_regs, AL_TRUE);
}


/*******************************************************************************
 ******************************************************************************/
static void al_ddr_phy_training_failure_print(
	struct al_ddr_init_cfg *cfg
)
{
	STRUCT_PHY_REGS __iomem *phy_regs = PHY_REGS(cfg->ddr_cfg.ddr_phy_regs_base);
	uint32_t reg_val;
	int i;
	unsigned int dtrank;
	al_bool wlerr;
	unsigned int qsgerr;
	unsigned int rdlvlerr;
	unsigned int wlaerr;
	al_bool srderr;
	al_bool rderr;
	al_bool wderr;
	al_bool reerr;
	al_bool weerr;
	unsigned int hverr;
	unsigned int dverr;

	al_bool wlerr_x4;
	unsigned int qsgerr_x4;
	unsigned int rdlvlerr_x4;
	unsigned int wlaerr_x4;
	al_bool srderr_x4;
	al_bool rderr_x4;
	al_bool wderr_x4;
	al_bool reerr_x4;
	al_bool weerr_x4;
	unsigned int hverr_x4;
	unsigned int dverr_x4;

	/* Get trained rank */
	reg_val = al_reg_read32(&phy_regs->dtcr[1]);
	dtrank = (reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DTCR1_DTRANK_MASK) >>
			ALPINE_V2_DWC_DDR_PHY_REGS_DTCR1_DTRANK_SHIFT;

	for (i = 0; i < AL_DDR_PHY_NUM_BYTE_LANES; i++) {
		if (!(cfg->calc.active_byte_lanes[i]))
			continue;

		reg_val = al_reg_read32(&phy_regs->datx8[i].gsr[0]);
		wlerr = (reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNGSR0_WLERR) ? 1 : 0;

		reg_val = al_reg_read32(&phy_regs->datx8[i].gsr[2]);
		rderr = (reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNGSR2_RDERR) ? 1 : 0;
		wderr = (reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNGSR2_WDERR) ? 1 : 0;
		reerr = (reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNGSR2_REERR) ? 1 : 0;
		weerr = (reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNGSR2_WEERR) ? 1 : 0;
		srderr = (reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNGSR2_SRDERR) ? 1 : 0;

		reg_val = al_reg_read32(&phy_regs->datx8[i].gsr[3]);
		hverr = AL_REG_FIELD_GET(reg_val,
				ALPINE_V2_DWC_DDR_PHY_REGS_DXNGSR3_HVERR_MASK,
				ALPINE_V2_DWC_DDR_PHY_REGS_DXNGSR3_HVERR_SHIFT);
		dverr = AL_REG_FIELD_GET(reg_val,
				ALPINE_V2_DWC_DDR_PHY_REGS_DXNGSR3_DVERR_MASK,
				ALPINE_V2_DWC_DDR_PHY_REGS_DXNGSR3_DVERR_SHIFT);

		reg_val = al_reg_read32(&phy_regs->datx8[i].gsr[4]);
		wlerr_x4 = (reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNGSR4_X4WLERR) ? 1 : 0;

		reg_val = al_reg_read32(&phy_regs->datx8[i].gsr[5]);
		rderr_x4 = (reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNGSR5_X4RDERR) ? 1 : 0;
		wderr_x4 = (reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNGSR5_X4WDERR) ? 1 : 0;
		reerr_x4 = (reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNGSR5_X4REERR) ? 1 : 0;
		weerr_x4 = (reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNGSR5_X4WEERR) ? 1 : 0;
		srderr_x4 = (reg_val & ALPINE_V2_DWC_DDR_PHY_REGS_DXNGSR5_X4SRDERR) ? 1 : 0;

		reg_val = al_reg_read32(&phy_regs->datx8[i].gsr[6]);
		hverr_x4 = AL_REG_FIELD_GET(reg_val,
				ALPINE_V2_DWC_DDR_PHY_REGS_DXNGSR6_X4HVERR_MASK,
				ALPINE_V2_DWC_DDR_PHY_REGS_DXNGSR6_X4HVERR_SHIFT);
		dverr_x4 = AL_REG_FIELD_GET(reg_val,
				ALPINE_V2_DWC_DDR_PHY_REGS_DXNGSR6_X4DVERR_MASK,
				ALPINE_V2_DWC_DDR_PHY_REGS_DXNGSR6_X4DVERR_SHIFT);

		reg_val = al_reg_read32(&phy_regs->datx8[i].rsr[0]);
		qsgerr = AL_REG_FIELD_GET(reg_val,
				ALPINE_V2_DWC_DDR_PHY_REGS_DXNRSR0_QSGERR_MASK,
				ALPINE_V2_DWC_DDR_PHY_REGS_DXNRSR0_QSGERR_SHIFT);
		qsgerr_x4 = AL_REG_FIELD_GET(reg_val,
				ALPINE_V2_DWC_DDR_PHY_REGS_DXNRSR0_X4QSGERR_MASK,
				ALPINE_V2_DWC_DDR_PHY_REGS_DXNRSR0_X4QSGERR_SHIFT);

		reg_val = al_reg_read32(&phy_regs->datx8[i].rsr[1]);
		rdlvlerr = AL_REG_FIELD_GET(reg_val,
				ALPINE_V2_DWC_DDR_PHY_REGS_DXNRSR1_RDLVLERR_MASK,
				ALPINE_V2_DWC_DDR_PHY_REGS_DXNRSR1_RDLVLERR_SHIFT);
		rdlvlerr_x4 = AL_REG_FIELD_GET(reg_val,
				ALPINE_V2_DWC_DDR_PHY_REGS_DXNRSR1_X4RDLVLERR_MASK,
				ALPINE_V2_DWC_DDR_PHY_REGS_DXNRSR1_X4RDLVLERR_SHIFT);

		reg_val = al_reg_read32(&phy_regs->datx8[i].rsr[3]);
		wlaerr = AL_REG_FIELD_GET(reg_val,
				ALPINE_V2_DWC_DDR_PHY_REGS_DXNRSR3_WLAERR_MASK,
				ALPINE_V2_DWC_DDR_PHY_REGS_DXNRSR3_WLAERR_SHIFT);
		wlaerr_x4 = AL_REG_FIELD_GET(reg_val,
				ALPINE_V2_DWC_DDR_PHY_REGS_DXNRSR3_X4WLAERR_MASK,
				ALPINE_V2_DWC_DDR_PHY_REGS_DXNRSR3_X4WLAERR_SHIFT);

		al_info("DX%d: ", i);
		if (wlerr)
			al_info("Write Leveling Error");
		else if (qsgerr)
			al_info("Read DQS Gate Error, Failing ranks bitwise 0x%02x", qsgerr);
		else if (rdlvlerr)
			al_info("Read Leveling Error, Failing ranks bitwise 0x%02x", rdlvlerr);
		else if (wlaerr)
			al_info("Write Level Adjust Error, Failing ranks bitwise 0x%02x", wlaerr);
		else if (srderr)
			al_info("Static Read Error, Failing rank %d", dtrank);
		else if (rderr)
			al_info("Read Deskew Error, Failing rank %d", dtrank);
		else if (wderr)
			al_info("Write Deskew Error, Failing rank %d", dtrank);
		else if (reerr)
			al_info("Read Eye Centering Error");
		else if (weerr)
			al_info("Write Eye Centering Error");
		else if (dverr)
			al_info("DRAM Vref Error, Failing ranks bitwise 0x%02x", dverr);
		else if (hverr)
			al_info("Host Vref Error, Failing ranks bitwise 0x%02x", hverr);
		else
			al_info("PASS");

		al_info("\n");

		if (cfg->org.ddr_device == AL_DDR_DEVICE_X4) {
			al_info("DX%d_x4: ", i);
			if (wlerr_x4)
				al_info("Write Leveling Error");
			else if (qsgerr_x4)
				al_info("Read DQS Gate Error, Failing ranks bitwise 0x%02x", qsgerr_x4);
			else if (rdlvlerr_x4)
				al_info("Read Leveling Error, Failing ranks bitwise 0x%02x", rdlvlerr_x4);
			else if (wlaerr_x4)
				al_info("Write Level Adjust Error, Failing ranks bitwise 0x%02x", wlaerr_x4);
			else if (srderr_x4)
				al_info("Static Read Error, Failing rank %d", dtrank);
			else if (rderr_x4)
				al_info("Read Deskew Error, Failing rank %d", dtrank);
			else if (wderr_x4)
				al_info("Write Deskew Error, Failing rank %d", dtrank);
			else if (reerr_x4)
				al_info("Read Eye Centering Error");
			else if (weerr_x4)
				al_info("Write Eye Centering Error");
			else if (dverr_x4)
				al_info("DRAM Vref Error, Failing ranks bitwise 0x%02x", dverr_x4);
			else if (hverr_x4)
				al_info("Host Vref Error, Failing ranks bitwise 0x%02x", hverr_x4);
			else
				al_info("PASS");

			al_info("\n");
		}
	}
}

/*******************************************************************************
 * API functions
 ******************************************************************************/
int al_ddr_init(
	struct al_ddr_init_cfg	*cfg)
{
	int err = 0;


	al_ddr_init_print_params(cfg);

	al_ddr_reset(cfg);

	err = al_ddr_active_byte_lanes_mark(cfg);
	if (err)
		return err;

	err = al_ddr_ctrl_cfg(cfg);
	if (err)
		return err;

#if (!defined(AL_DDR_PHY_DISABLED) || (AL_DDR_PHY_DISABLED == 0))
	err = al_ddr_phy_init(cfg);
	if (err)
		return err;

	err = al_ddr_phy_unused_dq_pd(cfg);
	if (err)
		return err;
#endif

	err = al_ddr_unit_reset_release(cfg);
	if (err)
		return err;

	err = al_ddr_ctrl_dfi_init(cfg);
	if (err)
		return err;

#if (!defined(AL_DDR_PHY_DISABLED) || (AL_DDR_PHY_DISABLED == 0))
	if (cfg->misc.training_en) {
		err = al_ddr_phy_training(cfg);
		if (err)
			return err;
	}

	err = al_ddr_phy_run(cfg);
	if (err)
		return err;
#endif

	err = al_ddr_ctrl_run(cfg);

	return err;
}

#endif

/** @} end of DDR group */

