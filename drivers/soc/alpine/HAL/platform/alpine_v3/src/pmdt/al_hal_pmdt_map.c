/*******************************************************************************
Copyright (C) 2016 Annapurna Labs Ltd.

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

#include "al_hal_iomap.h"
#include "al_hal_pmdt_map.h"
#include "al_hal_pcie.h"

/* */
#define AL_PMDT_DDR_READ_ACCESS_THRHLD_BIN_0		20
#define AL_PMDT_DDR_READ_ACCESS_THRHLD_BIN_1		60
#define AL_PMDT_DDR_READ_ACCESS_THRHLD_BIN_2		100
#define AL_PMDT_DDR_READ_ACCESS_THRHLD_BIN_3		150
#define AL_PMDT_DDR_READ_ACCESS_THRHLD_BIN_4		200

#define AL_PMDT_DDR_WRITE_ACCESS_THRHLD_BIN_0		20
#define AL_PMDT_DDR_WRITE_ACCESS_THRHLD_BIN_1		60
#define AL_PMDT_DDR_WRITE_ACCESS_THRHLD_BIN_2		100
#define AL_PMDT_DDR_WRITE_ACCESS_THRHLD_BIN_3		150
#define AL_PMDT_DDR_WRITE_ACCESS_THRHLD_BIN_4		200

#define AL_PMDT_PCIE_READ_ACCESS_THRHLD_BIN_0		400
#define AL_PMDT_PCIE_READ_ACCESS_THRHLD_BIN_1		800
#define AL_PMDT_PCIE_READ_ACCESS_THRHLD_BIN_2		1200
#define AL_PMDT_PCIE_READ_ACCESS_THRHLD_BIN_3		1600
#define AL_PMDT_PCIE_READ_ACCESS_THRHLD_BIN_4		2000

#define AL_PMDT_PCIE_WRITE_ACCESS_THRHLD_BIN_0		100
#define AL_PMDT_PCIE_WRITE_ACCESS_THRHLD_BIN_1		200
#define AL_PMDT_PCIE_WRITE_ACCESS_THRHLD_BIN_2		300
#define AL_PMDT_PCIE_WRITE_ACCESS_THRHLD_BIN_3		400
#define AL_PMDT_PCIE_WRITE_ACCESS_THRHLD_BIN_4		500

#define AL_PMDT_SB_AXI_TRANS_DEFAULT_TIMEOUT		25000 /* 50usec */
#define AL_PMDT_NB_AXI_TRANS_DEFAULT_TIMEOUT		100000 /* 50usec */

#define AL_PMDT_CPU_AXI_TRANS_RESP_BITS_START_POSITION 112
#define AL_PMDT_PCIE_ADAPT_AXI_TRANS_RESP_BITS_START_POSITION 119
#define AL_PMDT_PCIE_MASTER_X8_xX16_UNIT_AXI_TRANS_RESP_BITS_START_POSITION 119
#define AL_PMDT_PCIE_MASTER_X4_UNIT_AXI_TRANS_RESP_BITS_START_POSITION 120
#define AL_PMDT_PCIE_SLAVE_X8_xX16_UNIT_AXI_TRANS_RESP_BITS_START_POSITION 132
#define AL_PMDT_PCIE_SLAVE_X4_UNIT_AXI_TRANS_RESP_BITS_START_POSITION 133

/* According to HW design , PCIE_0_2 AXI units get reference clock of 750MHz
 * All other PCIe units get SB clock as reference */

#define AL_PMDT_PCIE_0_2_AXI_UNIT_REF_CLOCK		750000000 /* 750MHz*/

static struct al_pmdt_feature_timeout def_timeout_config = {
		.read_timeout_us = 0,
		.write_timeout_us = 0,
		.direction = AL_PMDT_MONITOR_READ_WRITE_DIR,
};

static struct al_pmdt_feature_latency def_latency_config = {
		.direction = AL_PMDT_MONITOR_READ_WRITE_DIR,
		.read_bar_thr = {
			[0 ... (AL_PMDT_AXI_MON_BAR_NUM-2)] = 0
		},
		.write_bar_thr = {
			[0 ... (AL_PMDT_AXI_MON_BAR_NUM-2)] = 0
		},
};

static struct al_pmdt_feature_trap def_trap_config = {
		.direction = AL_PMDT_MONITOR_READ_WRITE_DIR,
		.trap_event = AL_PMDT_TRAP_SLV_DEC_ERR_EVENT,
		.trap_mask = {
			.mask = {
				[0 ... (AL_PMDT_AXI_MON_TRACKER_WIDTH - 1)] = 0
			},
			.comp =	{
				[0 ... (AL_PMDT_AXI_MON_TRACKER_WIDTH - 1)] = 0
			},
		},
		.axi_resp_field_position = AL_PMDT_PCIE_ADAPT_AXI_TRANS_RESP_BITS_START_POSITION,
		.is_trace = AL_TRUE,
};

static struct al_pmdt_feature_track def_track_config = {
		.direction = AL_PMDT_MONITOR_READ_WRITE_DIR,
		.is_track_all = AL_TRUE,
		.all_track_filter_num = -1,
		.rtrack_mask = {
			.mask = {
				[0 ... (AL_PMDT_AXI_MON_TRACKER_WIDTH - 1)] = 0
			},
			.comp = {
				[0 ... (AL_PMDT_AXI_MON_TRACKER_WIDTH - 1)] = 0
			},
		},
		.wtrack_mask = {
			.mask = {
				[0 ... (AL_PMDT_AXI_MON_TRACKER_WIDTH - 1)] = 0
			},
			.comp = {
				[0 ... (AL_PMDT_AXI_MON_TRACKER_WIDTH - 1)] = 0
			},
		},
		.track_type = AL_PMDT_TRACK_BANDWIDTH,
		.upper_bound = AL_BIT_64(63),
		.lower_bound = 0,
		/*In case trap and track features are are requested together
		 * then the maximal window for track will be  AL_PMDT_PMU_WINDOW_TRAP_SAMPLE_TIME
		 * instead below value*/
		.sampling_window_us = AL_PMDT_PMU_WINDOW_DEFAULT_SAMPLE_TIME,
		.stop_on_cross = AL_TRUE,
};

static int al_pmdt_map_entry_default_init(
		enum al_pmdt_unit pmdtu, struct al_pmdt_config *config);

/******************************************************************************
 ******************************************************************************/
int al_pmdt_map_entry_get(
		unsigned int region __attribute__((__unused__)),
		enum al_pmdt_unit id,
		struct al_bootstrap *bs,
		struct al_pmdt_map_entry *entry)
{
	int rc;

	al_assert(entry);

	switch (id) {
	case AL_PMDT_UNIT_ADAPT_SSMAE_0:
		entry->name = "ADAPT_SSMAE_0";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_SSM_PMDT_BASE(0);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 0;
		entry->cctm_pmu_bit = AL_CCTM_GRP_A_PMU_ADAPT_SSMAE_0;
		entry->cctm_ela_bit = AL_CCTM_GRP_A_ELA_0_ADAPT_SSMAE_0;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_ADAPT_SSMAE_1:
		entry->name = "ADAPT_SSMAE_1";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_SSM_PMDT_BASE(1);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 0;
		entry->cctm_pmu_bit = AL_CCTM_GRP_A_PMU_ADAPT_SSMAE_1;
		entry->cctm_ela_bit = AL_CCTM_GRP_A_ELA_0_ADAPT_SSMAE_1;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_UDMA_0_SSMAE_0:
		entry->name = "UDMA_0_SSMAE_0";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_SSM_UDMA_PMDT_BASE(0, 0);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 0;
		entry->cctm_pmu_bit = AL_CCTM_GRP_A_PMU_UDMA_0_SSMAE_0;
		entry->cctm_ela_bit = AL_CCTM_GRP_A_ELA_UDMA_0_SSMA_0;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_UDMA_1_SSMAE_0:
		entry->name = "UDMA_1_SSMAE_0";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_SSM_UDMA_PMDT_BASE(0, 1);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 0;
		entry->cctm_pmu_bit = AL_CCTM_GRP_A_PMU_UDMA_1_SSMAE_0;
		entry->cctm_ela_bit = AL_CCTM_GRP_A_ELA_UDMA_1_SSMA_0;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_UDMA_2_SSMAE_0:
		entry->name = "UDMA_2_SSMAE_0";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_SSM_UDMA_PMDT_BASE(0, 2);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 0;
		entry->cctm_pmu_bit = AL_CCTM_GRP_A_PMU_UDMA_2_SSMAE_0;
		entry->cctm_ela_bit = AL_CCTM_GRP_A_ELA_UDMA_2_SSMA_0;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_UDMA_3_SSMAE_0:
		entry->name = "UDMA_3_SSMAE_0";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_SSM_UDMA_PMDT_BASE(0, 3);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 0;
		entry->cctm_pmu_bit = AL_CCTM_GRP_A_PMU_UDMA_3_SSMAE_0;
		entry->cctm_ela_bit = AL_CCTM_GRP_A_ELA_UDMA_3_SSMA_0;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_UDMA_0_SSMAE_1:
		entry->name = "UDMA_0_SSMAE_1";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_SSM_UDMA_PMDT_BASE(1, 0);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 0;
		entry->cctm_pmu_bit = AL_CCTM_GRP_A_PMU_UDMA_0_SSMAE_1;
		entry->cctm_ela_bit = AL_CCTM_GRP_A_ELA_UDMA_0_SSMA_1;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_UDMA_1_SSMAE_1:
		entry->name = "UDMA_1_SSMAE_1";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_SSM_UDMA_PMDT_BASE(1, 1);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 0;
		entry->cctm_pmu_bit = AL_CCTM_GRP_A_PMU_UDMA_1_SSMAE_1;
		entry->cctm_ela_bit = AL_CCTM_GRP_A_ELA_UDMA_1_SSMA_1;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_UDMA_2_SSMAE_1:
		entry->name = "UDMA_2_SSMAE_1";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_SSM_UDMA_PMDT_BASE(1, 2);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 0;
		entry->cctm_pmu_bit = AL_CCTM_GRP_A_PMU_UDMA_2_SSMAE_1;
		entry->cctm_ela_bit = AL_CCTM_GRP_A_ELA_UDMA_2_SSMA_1;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_UDMA_3_SSMAE_1:
		entry->name = "UDMA_3_SSMAE_1";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_SSM_UDMA_PMDT_BASE(1, 3);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 0;
		entry->cctm_pmu_bit = AL_CCTM_GRP_A_PMU_UDMA_3_SSMAE_1;
		entry->cctm_ela_bit = AL_CCTM_GRP_A_ELA_UDMA_3_SSMA_1;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_ADAPT_ETH_10_0:
		entry->name = "ADAPT_ETH_10_0";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_ETH_PMDT_BASE(1);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 0;
		entry->cctm_pmu_bit = AL_CCTM_GRP_A_PMU_ADAPT_ETH_10_0;
		entry->cctm_ela_bit = AL_CCTM_GRP_A_ELA_0_1_ADAPT_ETH_10_0_1;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_ADAPT_ETH_10_1:
		entry->name = "ADAPT_ETH_10_1";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_ETH_PMDT_BASE(3);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 0;
		entry->cctm_pmu_bit = AL_CCTM_GRP_A_PMU_ADAPT_ETH_10_1;
		entry->cctm_ela_bit = AL_CCTM_GRP_A_ELA_0_1_ADAPT_ETH_10_0_1;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_NBC:
		entry->name = "NBC";
		entry->core_freq = bs ? bs->ddr_pll_freq : 0;
		entry->regs_base = (void __iomem *)AL_NB_PMDT_BASE;
		entry->bus_base = (void __iomem *)AL_NB_BASE;
		entry->cctm_grp = 0;
		entry->cctm_pmu_bit = AL_CCTM_GRP_A_PMU_NBC;
		entry->cctm_ela_bit = -1;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_DDR_CONTROLER_0:
		entry->name = "DDR_CONTROLER_0";
		entry->core_freq = bs ? bs->ddr_pll_freq : 0;
		entry->regs_base = (void __iomem *)AL_DDRC_PMDT_BASE(0);
		entry->bus_base = (void __iomem *)AL_NB_BASE;
		entry->cctm_grp = 0;
		entry->cctm_pmu_bit = AL_CCTM_GRP_A_PMU_DDR_CONTROLER_0;
		entry->cctm_ela_bit = AL_CCTM_GRP_A_ELA_DDR_CONTROLER_0;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_DDR_CONTROLER_1:
		entry->name = "DDR_CONTROLER_1";
		entry->core_freq = bs ? bs->ddr_pll_freq : 0;
		entry->regs_base = (void __iomem *)AL_DDRC_PMDT_BASE(1);
		entry->bus_base = (void __iomem *)AL_NB_BASE;
		entry->cctm_grp = 0;
		entry->cctm_pmu_bit = AL_CCTM_GRP_A_PMU_DDR_CONTROLER_1;
		entry->cctm_ela_bit = AL_CCTM_GRP_A_ELA_DDR_CONTROLER_1;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_ADAPT_ETH_100_0:
		entry->name = "ADAPT_ETH_100_0";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_ETH_PMDT_BASE(0);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 1;
		entry->cctm_pmu_bit = AL_CCTM_GRP_B_PMU_ADAPT_ETH_100_0;
		entry->cctm_ela_bit = AL_CCTM_GRP_B_ELA_0_ADAPT_ETH_100_0;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_ADAPT_ETH_100_1:
		entry->name = "ADAPT_ETH_100_1";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_ETH_PMDT_BASE(2);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 1;
		entry->cctm_pmu_bit = AL_CCTM_GRP_B_PMU_ADAPT_ETH_100_1;
		entry->cctm_ela_bit = AL_CCTM_GRP_B_ELA_0_ADAPT_ETH_100_1;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_ADAPT_ETH_100_2:
		entry->name = "ADAPT_ETH_100_2";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_ETH_PMDT_BASE(4);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 1;
		entry->cctm_pmu_bit = AL_CCTM_GRP_B_PMU_ADAPT_ETH_100_2;
		entry->cctm_ela_bit = AL_CCTM_GRP_B_ELA_0_ADAPT_ETH_100_2;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_ADAPT_ETH_100_3:
		entry->name = "ADAPT_ETH_100_3";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_ETH_PMDT_BASE(5);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 1;
		entry->cctm_pmu_bit = AL_CCTM_GRP_B_PMU_ADAPT_ETH_100_3;
		entry->cctm_ela_bit = AL_CCTM_GRP_B_ELA_0_ADAPT_ETH_100_3;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_UDMA_ETH_100_0:
		entry->name = "UDMA_ETH_100_0";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_ETH_UDMA_PMDT_BASE(0, 0);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 1;
		entry->cctm_pmu_bit = AL_CCTM_GRP_B_PMU_UDMA_ETH_100_0;
		entry->cctm_ela_bit = AL_CCTM_GRP_B_ELA_UDMA_ETH_100_0;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_UDMA_ETH_100_1:
		entry->name = "UDMA_ETH_100_1";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_ETH_UDMA_PMDT_BASE(2, 0);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 1;
		entry->cctm_pmu_bit = AL_CCTM_GRP_B_PMU_UDMA_ETH_100_1;
		entry->cctm_ela_bit = AL_CCTM_GRP_B_ELA_UDMA_ETH_100_1;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_UDMA_ETH_100_2:
		entry->name = "UDMA_ETH_100_2";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_ETH_UDMA_PMDT_BASE(4, 0);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 1;
		entry->cctm_pmu_bit = AL_CCTM_GRP_B_PMU_UDMA_ETH_100_2;
		entry->cctm_ela_bit = AL_CCTM_GRP_B_ELA_UDMA_ETH_100_2;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_UDMA_ETH_100_3:
		entry->name = "UDMA_ETH_100_3";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_ETH_UDMA_PMDT_BASE(5, 0);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 1;
		entry->cctm_pmu_bit = AL_CCTM_GRP_B_PMU_UDMA_ETH_100_3;
		entry->cctm_ela_bit = AL_CCTM_GRP_B_ELA_UDMA_ETH_100_3;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_EC_ETH_100_0:
		entry->name = "ETH_CTRL_ETH_100_0";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_ETH_EC_PMDT_BASE(0);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 0;
		entry->cctm_pmu_bit = AL_CCTM_GRP_A_PMU_EC_ETH_100_0;
		entry->cctm_ela_bit = AL_CCTM_GRP_A_ELA_EC_ETH_100_0;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
		return -1;
	case AL_PMDT_UNIT_EC_ETH_100_1:
		entry->name = "ETH_CTRL_ETH_100_1";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_ETH_EC_PMDT_BASE(1);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 0;
		entry->cctm_pmu_bit = AL_CCTM_GRP_A_PMU_EC_ETH_100_1;
		entry->cctm_ela_bit = AL_CCTM_GRP_A_ELA_EC_ETH_100_1;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_EC_ETH_100_2:
		entry->name = "ETH_CTRL_ETH_100_2";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_ETH_EC_PMDT_BASE(2);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 1;
		entry->cctm_pmu_bit = AL_CCTM_GRP_B_PMU_EC_ETH_100_2;
		entry->cctm_ela_bit = AL_CCTM_GRP_B_ELA_EC_ETH_100_2;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_EC_ETH_100_3:
		entry->name = "ETH_CTRL_ETH_100_3";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_ETH_EC_PMDT_BASE(3);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 1;
		entry->cctm_pmu_bit = AL_CCTM_GRP_B_PMU_EC_ETH_100_3;
		entry->cctm_ela_bit = AL_CCTM_GRP_B_ELA_EC_ETH_100_3;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_SHARED_RESOURCE_ETH_100_0:
		entry->name = "SHARED_RESOURCE_ETH_100_3";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_ETH_SHARED_RESOURCE_PMDT_BASE;
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 1;
		entry->cctm_pmu_bit = AL_CCTM_GRP_B_PMU_SHARED_RESOURCE_ETH_100_0;
		entry->cctm_ela_bit = AL_CCTM_GRP_B_ELA_SHARED_RESOURCE_ETH_100_0;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_SHARED_CACHE_ETH_100_0:
		entry->name = "SHARED_CACHE_ETH_100_0";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_ETH_SHARED_CACHE_PMDT_BASE;
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 1;
		entry->cctm_pmu_bit = AL_CCTM_GRP_B_PMU_SHARED_CACHE_ETH_100_0;
		entry->cctm_ela_bit = AL_CCTM_GRP_B_ELA_SHARED_CACHE_ETH_100_0;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_CPU_CLUSTER_0:
		entry->name = "CPU_CLUSTER_0";
		entry->core_freq = bs ? bs->cpu_pll_freq : 0;
		entry->regs_base = (void __iomem *)AL_NB_ANPA_PMDT_BASE(0);
		entry->bus_base = (void __iomem *)AL_NB_BASE;
		entry->cctm_grp = 1;
		entry->cctm_pmu_bit = AL_CCTM_GRP_B_PMU_CPU_CLUSTER_0;
		entry->cctm_ela_bit = AL_CCTM_GRP_B_ELA_CPU_CLUSTER_0;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_CPU_CLUSTER_1:
		entry->name = "CPU_CLUSTER_1";
		entry->core_freq = bs ? bs->cpu_pll_freq : 0;
		entry->regs_base = (void __iomem *)AL_NB_ANPA_PMDT_BASE(1);
		entry->bus_base = (void __iomem *)AL_NB_BASE;
		entry->cctm_grp = 1;
		entry->cctm_pmu_bit = AL_CCTM_GRP_B_PMU_CPU_CLUSTER_1;
		entry->cctm_ela_bit = AL_CCTM_GRP_B_ELA_CPU_CLUSTER_1;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_CPU_CLUSTER_2:
		entry->name = "CPU_CLUSTER_2";
		entry->core_freq = bs ? bs->cpu_pll_freq : 0;
		entry->regs_base = (void __iomem *)AL_NB_ANPA_PMDT_BASE(2);
		entry->bus_base = (void __iomem *)AL_NB_BASE;
		entry->cctm_grp = 1;
		entry->cctm_pmu_bit = AL_CCTM_GRP_B_PMU_CPU_CLUSTER_2;
		entry->cctm_ela_bit = AL_CCTM_GRP_B_ELA_CPU_CLUSTER_2;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_CPU_CLUSTER_3:
		entry->name = "CPU_CLUSTER_3";
		entry->core_freq = bs ? bs->cpu_pll_freq : 0;
		entry->regs_base = (void __iomem *)AL_NB_ANPA_PMDT_BASE(3);
		entry->bus_base = (void __iomem *)AL_NB_BASE;
		entry->cctm_grp = 1;
		entry->cctm_pmu_bit = AL_CCTM_GRP_B_PMU_CPU_CLUSTER_3;
		entry->cctm_ela_bit = AL_CCTM_GRP_B_ELA_CPU_CLUSTER_3;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_ADAPT_SATA_0:
		entry->name = "ADAPT_SATA_0";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_SATA_PMDT_BASE(0);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 2;
		entry->cctm_pmu_bit = AL_CCTM_GRP_C_PMU_ADAPT_SATA_0;
		entry->cctm_ela_bit = AL_CCTM_GRP_C_ELA_ADAPT_SATA_0;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_ADAPT_SATA_1:
		entry->name = "ADAPT_SATA_1";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_SATA_PMDT_BASE(1);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 2;
		entry->cctm_pmu_bit = AL_CCTM_GRP_C_PMU_ADAPT_SATA_1;
		entry->cctm_ela_bit = AL_CCTM_GRP_C_ELA_ADAPT_SATA_1;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_ADAPT_SATA_2:
		entry->name = "ADAPT_SATA_2";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_SATA_PMDT_BASE(2);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 2;
		entry->cctm_pmu_bit = AL_CCTM_GRP_C_PMU_ADAPT_SATA_2;
		entry->cctm_ela_bit = AL_CCTM_GRP_C_ELA_ADAPT_SATA_2;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_ADAPT_SATA_3:
		entry->name = "ADAPT_SATA_3";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_SATA_PMDT_BASE(3);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 2;
		entry->cctm_pmu_bit = AL_CCTM_GRP_C_PMU_ADAPT_SATA_3;
		entry->cctm_ela_bit = AL_CCTM_GRP_C_ELA_ADAPT_SATA_3;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_ADAPT_SATA_4:
		entry->name = "ADAPT_SATA_4";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_SATA_PMDT_BASE(4);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 2;
		entry->cctm_pmu_bit = AL_CCTM_GRP_C_PMU_ADAPT_SATA_4;
		entry->cctm_ela_bit = AL_CCTM_GRP_C_ELA_ADAPT_SATA_4;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_ADAPT_SATA_5:
		entry->name = "ADAPT_SATA_5";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_SATA_PMDT_BASE(5);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 2;
		entry->cctm_pmu_bit = AL_CCTM_GRP_C_PMU_ADAPT_SATA_5;
		entry->cctm_ela_bit = AL_CCTM_GRP_C_ELA_ADAPT_SATA_5;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_ADAPT_SATA_6:
		entry->name = "ADAPT_SATA_6";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_SATA_PMDT_BASE(6);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 2;
		entry->cctm_pmu_bit = AL_CCTM_GRP_C_PMU_ADAPT_SATA_6;
		entry->cctm_ela_bit = AL_CCTM_GRP_C_ELA_ADAPT_SATA_6;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_ADAPT_SATA_7:
		entry->name = "ADAPT_SATA_7";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_SATA_PMDT_BASE(7);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 2;
		entry->cctm_pmu_bit = AL_CCTM_GRP_C_PMU_ADAPT_SATA_7;
		entry->cctm_ela_bit = AL_CCTM_GRP_C_ELA_ADAPT_SATA_7;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_PCIECORE_0:
		entry->name = "CORE_PCIE_0";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_PCIE_CORE_PMDT_BASE(0);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 2;
		entry->cctm_pmu_bit = AL_CCTM_GRP_C_PMU_PCIECORE_0;
		entry->cctm_ela_bit = AL_CCTM_GRP_C_ELA_PCIECORE_0;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_PCIECORE_1:
		entry->name = "CORE_PCIE_1";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_PCIE_CORE_PMDT_BASE(1);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 2;
		entry->cctm_pmu_bit = AL_CCTM_GRP_C_PMU_PCIECORE_1;
		entry->cctm_ela_bit = AL_CCTM_GRP_C_ELA_PCIECORE_1;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_PCIECORE_2:
		entry->name = "CORE_PCIE_2";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_PCIE_CORE_PMDT_BASE(2);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 2;
		entry->cctm_pmu_bit = AL_CCTM_GRP_C_PMU_PCIECORE_2;
		entry->cctm_ela_bit = AL_CCTM_GRP_C_ELA_PCIECORE_2;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_PCIECORE_3:
		entry->name = "CORE_PCIE_3";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_PCIE_CORE_PMDT_BASE(3);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 2;
		entry->cctm_pmu_bit = AL_CCTM_GRP_C_PMU_PCIECORE_3;
		entry->cctm_ela_bit = AL_CCTM_GRP_C_ELA_PCIECORE_3;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_PCIECORE_4:
		entry->name = "CORE_PCIE_4";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_PCIE_CORE_PMDT_BASE(4);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 2;
		entry->cctm_pmu_bit = AL_CCTM_GRP_C_PMU_PCIECORE_4;
		entry->cctm_ela_bit = AL_CCTM_GRP_C_ELA_PCIECORE_4;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_PCIECORE_5:
		entry->name = "CORE_PCIE_5";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_PCIE_CORE_PMDT_BASE(5);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 2;
		entry->cctm_pmu_bit = AL_CCTM_GRP_C_PMU_PCIECORE_5;
		entry->cctm_ela_bit = AL_CCTM_GRP_C_ELA_PCIECORE_5;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_PCIECORE_6:
		entry->name = "CORE_PCIE_6";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_PCIE_CORE_PMDT_BASE(6);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 2;
		entry->cctm_pmu_bit = AL_CCTM_GRP_C_PMU_PCIECORE_6;
		entry->cctm_ela_bit = AL_CCTM_GRP_C_ELA_PCIECORE_6;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_PCIECORE_7:
		entry->name = "CORE_PCIE_7";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_PCIE_CORE_PMDT_BASE(7);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 2;
		entry->cctm_pmu_bit = AL_CCTM_GRP_C_PMU_PCIECORE_7;
		entry->cctm_ela_bit = AL_CCTM_GRP_C_ELA_PCIECORE_7;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_PCIe_SLVAXI_0:
		entry->name = "SLVAXI_PCIE_0";
		entry->core_freq = AL_PMDT_PCIE_0_2_AXI_UNIT_REF_CLOCK;
		entry->regs_base = (void __iomem *)AL_PCIE_SLV_PMDT_BASE(0);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 3;
		entry->cctm_pmu_bit = AL_CCTM_GRP_D_PMU_PCIe_SLVAXI_0;
		entry->cctm_ela_bit = AL_CCTM_GRP_D_ELA_PCIe_SLVAXI_0;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_PCIe_SLVAXI_1:
		entry->name = "SLVAXI_PCIE_1";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_PCIE_SLV_PMDT_BASE(1);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 3;
		entry->cctm_pmu_bit = AL_CCTM_GRP_D_PMU_PCIe_SLVAXI_1;
		entry->cctm_ela_bit = AL_CCTM_GRP_D_ELA_PCIe_SLVAXI_1;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_PCIe_SLVAXI_2:
		entry->name = "SLVAXI_PCIE_2";
		entry->core_freq = AL_PMDT_PCIE_0_2_AXI_UNIT_REF_CLOCK;
		entry->regs_base = (void __iomem *)AL_PCIE_SLV_PMDT_BASE(2);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 3;
		entry->cctm_pmu_bit = AL_CCTM_GRP_D_PMU_PCIe_SLVAXI_2;
		entry->cctm_ela_bit = AL_CCTM_GRP_D_ELA_PCIe_SLVAXI_2;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_PCIe_SLVAXI_3:
		entry->name = "SLVAXI_PCIE_3";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_PCIE_SLV_PMDT_BASE(3);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 3;
		entry->cctm_pmu_bit = AL_CCTM_GRP_D_PMU_PCIe_SLVAXI_3;
		entry->cctm_ela_bit = AL_CCTM_GRP_D_ELA_PCIe_SLVAXI_3;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_PCIe_SLVAXI_4:
		entry->name = "SLVAXI_PCIE_4";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_PCIE_SLV_PMDT_BASE(4);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 3;
		entry->cctm_pmu_bit = AL_CCTM_GRP_D_PMU_PCIe_SLVAXI_4;
		entry->cctm_ela_bit = AL_CCTM_GRP_D_ELA_PCIe_SLVAXI_4;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_PCIe_SLVAXI_5:
		entry->name = "SLVAXI_PCIE_5";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_PCIE_SLV_PMDT_BASE(5);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 3;
		entry->cctm_pmu_bit = AL_CCTM_GRP_D_PMU_PCIe_SLVAXI_5;
		entry->cctm_ela_bit = AL_CCTM_GRP_D_ELA_PCIe_SLVAXI_5;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_PCIe_SLVAXI_6:
		entry->name = "SLVAXI_PCIE_6";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_PCIE_SLV_PMDT_BASE(6);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 3;
		entry->cctm_pmu_bit = AL_CCTM_GRP_D_PMU_PCIe_SLVAXI_6;
		entry->cctm_ela_bit = AL_CCTM_GRP_D_ELA_PCIe_SLVAXI_6;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_PCIe_SLVAXI_7:
		entry->name = "SLVAXI_PCIE_7";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_PCIE_SLV_PMDT_BASE(7);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 3;
		entry->cctm_pmu_bit = AL_CCTM_GRP_D_PMU_PCIe_SLVAXI_7;
		entry->cctm_ela_bit = AL_CCTM_GRP_D_ELA_PCIe_SLVAXI_7;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_PCIe_MSTRAXI_0:
		entry->name = "MSTR_AXI_PCIE_0";
		entry->core_freq = AL_PMDT_PCIE_0_2_AXI_UNIT_REF_CLOCK;
		entry->regs_base = (void __iomem *)AL_PCIE_MSTR_PMDT_BASE(0);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 3;
		entry->cctm_pmu_bit = AL_CCTM_GRP_D_PMU_PCIe_MSTRAXI_0;
		entry->cctm_ela_bit = AL_CCTM_GRP_D_ELA_PCIe_MSTRAXI_0;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_PCIe_MSTRAXI_1:
		entry->name = "MSTR_AXI_PCIE_1";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_PCIE_MSTR_PMDT_BASE(1);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 3;
		entry->cctm_pmu_bit = AL_CCTM_GRP_D_PMU_PCIe_MSTRAXI_1;
		entry->cctm_ela_bit = AL_CCTM_GRP_D_ELA_PCIe_MSTRAXI_1;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_PCIe_MSTRAXI_2:
		entry->name = "MSTR_AXI_PCIE_2";
		entry->core_freq = AL_PMDT_PCIE_0_2_AXI_UNIT_REF_CLOCK;
		entry->regs_base = (void __iomem *)AL_PCIE_MSTR_PMDT_BASE(2);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 3;
		entry->cctm_pmu_bit = AL_CCTM_GRP_D_PMU_PCIe_MSTRAXI_2;
		entry->cctm_ela_bit = AL_CCTM_GRP_D_ELA_PCIe_MSTRAXI_2;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_PCIe_MSTRAXI_3:
		entry->name = "MSTR_AXI_PCIE_3";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_PCIE_MSTR_PMDT_BASE(3);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 3;
		entry->cctm_pmu_bit = AL_CCTM_GRP_D_PMU_PCIe_MSTRAXI_3;
		entry->cctm_ela_bit = AL_CCTM_GRP_D_ELA_PCIe_MSTRAXI_3;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_PCIe_MSTRAXI_4:
		entry->name = "MSTR_AXI_PCIE_4";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_PCIE_MSTR_PMDT_BASE(4);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 3;
		entry->cctm_pmu_bit = AL_CCTM_GRP_D_PMU_PCIe_MSTRAXI_4;
		entry->cctm_ela_bit = AL_CCTM_GRP_D_ELA_PCIe_MSTRAXI_4;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_PCIe_MSTRAXI_5:
		entry->name = "MSTR_AXI_PCIE_5";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_PCIE_MSTR_PMDT_BASE(5);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 3;
		entry->cctm_pmu_bit = AL_CCTM_GRP_D_PMU_PCIe_MSTRAXI_5;
		entry->cctm_ela_bit = AL_CCTM_GRP_D_ELA_PCIe_MSTRAXI_5;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_PCIe_MSTRAXI_6:
		entry->name = "MSTR_AXI_PCIE_6";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_PCIE_MSTR_PMDT_BASE(6);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 3;
		entry->cctm_pmu_bit = AL_CCTM_GRP_D_PMU_PCIe_MSTRAXI_6;
		entry->cctm_ela_bit = AL_CCTM_GRP_D_ELA_PCIe_MSTRAXI_6;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	case AL_PMDT_UNIT_PCIe_MSTRAXI_7:
		entry->name = "MSTR_AXI_PCIE_7";
		entry->core_freq = bs ? bs->sb_clk_freq : 0;
		entry->regs_base = (void __iomem *)AL_PCIE_MSTR_PMDT_BASE(7);
		entry->bus_base = (void __iomem *)AL_SB_BASE;
		entry->cctm_grp = 3;
		entry->cctm_pmu_bit = AL_CCTM_GRP_D_PMU_PCIe_MSTRAXI_7;
		entry->cctm_ela_bit = AL_CCTM_GRP_D_ELA_PCIe_MSTRAXI_7;
		rc = al_pmdt_map_entry_default_init(id, &entry->pmdtu_config);
		break;
	default:
		al_err(" Illegal PMDTU id (%d)\n", id);
		rc = -1;
	}

	return rc;
}

int al_pmdt_map_unit_default_config_get(
		enum al_pmdt_unit id,
		struct al_pmdt_config *config)
{
	al_assert(config);

	if (id >= AL_PMDT_UNIT_MAX) {
		al_err(" Illegal PMDTU id (%d)\n", id);
		return -1;
	}

	al_pmdt_map_entry_default_init(id, config);

	return 0;
}

int al_pmdt_map_trig_num_get(
		unsigned int axi_mon_num,
		unsigned int axi_mon_instance,
		enum al_pmdt_pmu_int_trig_type type,
		unsigned int index)
{
	int trig_num = -1;

	if (axi_mon_num < 1 || axi_mon_num > 2) {
		al_err("Illegal AXI Monitor number (%d)\n", axi_mon_num);
		return -1;
	}
	if (axi_mon_instance >= axi_mon_num) {
		al_err("Illegal AXI Monitor instance %d (max available %d)\n",
			axi_mon_instance, axi_mon_num - 1);
		return -1;
	}

	/*
	 * Single AXI monitor:
	 * 64-69: AXI_MONITOR_0 latency triggers
	 * 70-73: AXI_MONITOR_0 trackers triggers incremented by 1
	 * 74-77: AXI_MONITOR_0 trackers triggers incremented by value
	 * Dual AXI monitors:
	 * 64-69: AXI_MONITOR_0 latency triggers
	 * 70-75: AXI_MONITOR_1 latency triggers
	 * 76-79: AXI_MONITOR_0 trackers triggers incremented by 1
	 * 80-83: AXI_MONITOR_1 trackers triggers incremented by 1
	 * 84-87: AXI_MONITOR_0 trackers triggers incremented by value
	 * 88-91: AXI_MONITOR_1 trackers triggers incremented by value
	 */

	switch (type) {
	case AL_PMDT_INT_TRIG_LATENCY:
		if (index >= 6) {
			al_err("Required index %d is not allowed "
				"for requested AL_PMDT_INT_TRIG_LATENCY type\n",
					index);
			return -1;
		}
		trig_num = (axi_mon_instance == 0) ? 64 + index : 70 + index;
		break;
	case AL_PMDT_INT_TRIG_TRACKER_INC_1:
		if (index >= 4) {
			al_err("Required index %d is not allowed "
				"for requested AL_PMDT_INT_TRIG_TRACKER_INC_1 type\n",
					index);
			return -1;
		}
		if (axi_mon_num == 1)
			trig_num = 70 + index;
		else /* axi_mon_num == 2 */
			trig_num = (axi_mon_instance == 0) ? 76 + index : 80 + index;
		break;
	case AL_PMDT_INT_TRIG_TRACKER_INC_VAL:
		if (index >= 4) {
			al_err("Required index %d is not allowed "
				"for requested AL_PMDT_INT_TRIG_TRACKER_INC_VAL type\n",
					index);
			return -1;
		}
		if (axi_mon_num == 1)
			trig_num = 74 + index;
		else /* axi_mon_num == 2 */
			trig_num = (axi_mon_instance == 0) ? 84 + index : 88 + index;
		break;
	}

	return trig_num;
}

int al_pmdt_map_trace_sig_get(
		unsigned int axi_mon_num,
		unsigned int axi_mon_instance)
{
	int signal_group;

	if (axi_mon_num < 1 || axi_mon_num > 2) {
		al_err("Illegal AXI Monitor number (%d)\n", axi_mon_num);
		return -1;
	}
	if (axi_mon_instance >= axi_mon_num) {
		al_err("Illegal AXI Monitor instance %d (max available %d)\n",
				axi_mon_instance, axi_mon_num - 1);
		return -1;
	}

	/*
	 * Single AXI monitor:
	 *	AXI_MON_0 - 10
	 * Dual AXI monitors:
	 *	AXI_MON_0 - 9
	 *	AXI_MON_1 - 10
	 */

	if (axi_mon_num == 2 && axi_mon_instance == 0)
		signal_group = AL_BIT(9);
	else
		signal_group = AL_BIT(10);

	return signal_group;
}

int al_pmdt_map_pmu_irq_num_get(
		unsigned int axi_mon_num,
		unsigned int axi_mon_instance,
		enum al_pmdt_axi_mon_irq_type type)
{
	int irq_num = -1;

	if (axi_mon_num < 1 || axi_mon_num > 2) {
		al_err("Illegal AXI Monitor number (%d)\n", axi_mon_num);
		return -1;
	}
	if (axi_mon_instance >= axi_mon_num) {
		al_err("Illegal AXI Monitor instance %d (max available %d)\n",
				axi_mon_instance, axi_mon_num - 1);
		return -1;
	}

	/*
	 * Single AXI monitor:
	 * 0: AL_PMDT_AXIMON_TIMEOUT_IRQ
	 * 1: AL_PMDT_AXIMON_OVERFLOW_IRQ
	 * 2: AL_PMDT_AXIMON_UNDERFLOW_IRQ
	 * Dual AXI monitors:
	 * 0: AL_PMDT_AXIMON_TIMEOUT_IRQ of AXIMON 0
	 * 1: AL_PMDT_AXIMON_TIMEOUT_IRQ of AXIMON 1
	 * 2: AL_PMDT_AXIMON_OVERFLOW_IRQ of AXIMON 0
	 * 3: AL_PMDT_AXIMON_OVERFLOW_IRQ of AXIMON 1
	 * 4: AL_PMDT_AXIMON_UNDERFLOW_IRQ of AXIMON 0
	 * 5: AL_PMDT_AXIMON_UNDERFLOW_IRQ of AXIMON 1
	 */

	switch (type) {
	case AL_PMDT_AXIMON_TIMEOUT_IRQ:
		if (axi_mon_num == 2)
			irq_num = (axi_mon_instance == 0) ? 0 : 1;
		else
			irq_num = 0;
		break;
	case AL_PMDT_AXIMON_OVERFLOW_IRQ:
		if (axi_mon_num == 2)
			irq_num = (axi_mon_instance == 0) ? 2 : 3;
		else
			irq_num = 1;
		break;
	case AL_PMDT_AXIMON_UNDERFLOW_IRQ:
		if (axi_mon_num == 2)
			irq_num = (axi_mon_instance == 0) ? 4 : 5;
		else
			irq_num = 2;
		break;
	}

	return irq_num;
}

static int al_pmdt_map_entry_default_init(
		enum al_pmdt_unit pmdtu,
		struct al_pmdt_config *config)
{
	al_memset(config, 0 , sizeof(struct al_pmdt_config));

	config->feature_mask =
			AL_PMDT_FEATURE_AXI_TIMEOUT |
			AL_PMDT_FEATURE_AXI_LATENCY_HISTOGRAM |
			AL_PMDT_FEATURE_AXI_TRACK;

	al_memcpy(&config->timeout_config, &def_timeout_config,
			sizeof(struct al_pmdt_feature_timeout));
	al_memcpy(&config->latency_config, &def_latency_config,
			sizeof(struct al_pmdt_feature_latency));
	al_memcpy(&config->trap_config, &def_trap_config,
			sizeof(struct al_pmdt_feature_trap));
	al_memcpy(&config->track_config, &def_track_config,
			sizeof(struct al_pmdt_feature_track));

	switch (pmdtu) {
	case (AL_PMDT_UNIT_CPU_CLUSTER_0):
	case (AL_PMDT_UNIT_CPU_CLUSTER_1):
	case (AL_PMDT_UNIT_CPU_CLUSTER_2):
	case (AL_PMDT_UNIT_CPU_CLUSTER_3):
		config->timeout_config.read_timeout_us = AL_PMDT_NB_AXI_TRANS_DEFAULT_TIMEOUT;
		config->timeout_config.write_timeout_us = AL_PMDT_NB_AXI_TRANS_DEFAULT_TIMEOUT;
		/* Initialize default latency bins */
		config->latency_config.read_bar_thr[0] = AL_PMDT_DDR_READ_ACCESS_THRHLD_BIN_0;
		config->latency_config.write_bar_thr[0] = AL_PMDT_DDR_WRITE_ACCESS_THRHLD_BIN_0;
		config->latency_config.read_bar_thr[1] = AL_PMDT_DDR_READ_ACCESS_THRHLD_BIN_1;
		config->latency_config.write_bar_thr[1] = AL_PMDT_DDR_WRITE_ACCESS_THRHLD_BIN_1;
		config->latency_config.read_bar_thr[2] = AL_PMDT_DDR_READ_ACCESS_THRHLD_BIN_2;
		config->latency_config.write_bar_thr[2] = AL_PMDT_DDR_WRITE_ACCESS_THRHLD_BIN_2;
		config->latency_config.read_bar_thr[3] = AL_PMDT_DDR_READ_ACCESS_THRHLD_BIN_3;
		config->latency_config.write_bar_thr[3] = AL_PMDT_DDR_WRITE_ACCESS_THRHLD_BIN_3;
		config->latency_config.read_bar_thr[4] = AL_PMDT_DDR_READ_ACCESS_THRHLD_BIN_4;
		config->latency_config.write_bar_thr[4] = AL_PMDT_DDR_WRITE_ACCESS_THRHLD_BIN_4;
		/*
		 * Initialize starting position of AXI transaction response bits
		 * in AXI transaction vector
		 * */
		config->trap_config.axi_resp_field_position =
				AL_PMDT_CPU_AXI_TRANS_RESP_BITS_START_POSITION;
		break;
	case (AL_PMDT_UNIT_ADAPT_SSMAE_0):
	case (AL_PMDT_UNIT_ADAPT_SSMAE_1):
	case (AL_PMDT_UNIT_ADAPT_ETH_10_0):
	case (AL_PMDT_UNIT_ADAPT_ETH_10_1):
	case (AL_PMDT_UNIT_ADAPT_ETH_100_0):
	case (AL_PMDT_UNIT_ADAPT_ETH_100_1):
	case (AL_PMDT_UNIT_ADAPT_ETH_100_2):
	case (AL_PMDT_UNIT_ADAPT_ETH_100_3):
	case (AL_PMDT_UNIT_ADAPT_SATA_0):
	case (AL_PMDT_UNIT_ADAPT_SATA_1):
	case (AL_PMDT_UNIT_ADAPT_SATA_2):
	case (AL_PMDT_UNIT_ADAPT_SATA_3):
	case (AL_PMDT_UNIT_ADAPT_SATA_4):
	case (AL_PMDT_UNIT_ADAPT_SATA_5):
	case (AL_PMDT_UNIT_ADAPT_SATA_6):
	case (AL_PMDT_UNIT_ADAPT_SATA_7):
	case (AL_PMDT_UNIT_SHARED_CACHE_ETH_100_0):
		config->track_config.direction = AL_PMDT_MONITOR_READ_WRITE_DIR;
		config->timeout_config.read_timeout_us = AL_PMDT_SB_AXI_TRANS_DEFAULT_TIMEOUT;
		config->timeout_config.write_timeout_us = AL_PMDT_SB_AXI_TRANS_DEFAULT_TIMEOUT;
		/* Initialize default latency bins */
		config->latency_config.read_bar_thr[0] = AL_PMDT_DDR_READ_ACCESS_THRHLD_BIN_0;
		config->latency_config.write_bar_thr[0] = AL_PMDT_DDR_WRITE_ACCESS_THRHLD_BIN_0;
		config->latency_config.read_bar_thr[1] = AL_PMDT_DDR_READ_ACCESS_THRHLD_BIN_1;
		config->latency_config.write_bar_thr[1] = AL_PMDT_DDR_WRITE_ACCESS_THRHLD_BIN_1;
		config->latency_config.read_bar_thr[2] = AL_PMDT_DDR_READ_ACCESS_THRHLD_BIN_2;
		config->latency_config.write_bar_thr[2] = AL_PMDT_DDR_WRITE_ACCESS_THRHLD_BIN_2;
		config->latency_config.read_bar_thr[3] = AL_PMDT_DDR_READ_ACCESS_THRHLD_BIN_3;
		config->latency_config.write_bar_thr[3] = AL_PMDT_DDR_WRITE_ACCESS_THRHLD_BIN_3;
		config->latency_config.read_bar_thr[4] = AL_PMDT_DDR_READ_ACCESS_THRHLD_BIN_4;
		config->latency_config.write_bar_thr[4] = AL_PMDT_DDR_WRITE_ACCESS_THRHLD_BIN_4;
		/*
		 * Initialize starting position of AXI transaction response bits
		 * in AXI transaction vector
		 * */
		config->trap_config.axi_resp_field_position =
				AL_PMDT_PCIE_ADAPT_AXI_TRANS_RESP_BITS_START_POSITION;
		break;
	case (AL_PMDT_UNIT_PCIe_SLVAXI_0):
	case (AL_PMDT_UNIT_PCIe_SLVAXI_1):
	case (AL_PMDT_UNIT_PCIe_SLVAXI_2):
	case (AL_PMDT_UNIT_PCIe_SLVAXI_3):
	case (AL_PMDT_UNIT_PCIe_SLVAXI_4):
	case (AL_PMDT_UNIT_PCIe_SLVAXI_5):
	case (AL_PMDT_UNIT_PCIe_SLVAXI_6):
	case (AL_PMDT_UNIT_PCIe_SLVAXI_7):
		config->timeout_config.direction = AL_PMDT_MONITOR_READ_DIR;
		config->trap_config.direction = AL_PMDT_MONITOR_READ_DIR;
		config->track_config.direction = AL_PMDT_MONITOR_READ_DIR;
		config->latency_config.direction = AL_PMDT_MONITOR_READ_DIR;

		config->timeout_config.read_timeout_us = AL_PMDT_SB_AXI_TRANS_DEFAULT_TIMEOUT;
		/* Initialize default latency bins */
		config->latency_config.read_bar_thr[0] = AL_PMDT_PCIE_READ_ACCESS_THRHLD_BIN_0;
		config->latency_config.read_bar_thr[1] = AL_PMDT_PCIE_READ_ACCESS_THRHLD_BIN_1;
		config->latency_config.read_bar_thr[2] = AL_PMDT_PCIE_READ_ACCESS_THRHLD_BIN_2;
		config->latency_config.read_bar_thr[3] = AL_PMDT_PCIE_READ_ACCESS_THRHLD_BIN_3;
		config->latency_config.read_bar_thr[4] = AL_PMDT_PCIE_READ_ACCESS_THRHLD_BIN_4;
		/*TODO: AXI response field in PCIE depends on it configuration
		 * For PCIex4 Slave position it is 133, and for PCIeX8,16 it is 132. Right way how
		 * to define position according to PCIE configuration should be found
		 */
		config->trap_config.axi_resp_field_position =
				AL_PMDT_PCIE_SLAVE_X4_UNIT_AXI_TRANS_RESP_BITS_START_POSITION;
		break;
	case (AL_PMDT_UNIT_PCIe_MSTRAXI_0):
	case (AL_PMDT_UNIT_PCIe_MSTRAXI_1):
	case (AL_PMDT_UNIT_PCIe_MSTRAXI_2):
	case (AL_PMDT_UNIT_PCIe_MSTRAXI_3):
	case (AL_PMDT_UNIT_PCIe_MSTRAXI_4):
	case (AL_PMDT_UNIT_PCIe_MSTRAXI_5):
	case (AL_PMDT_UNIT_PCIe_MSTRAXI_6):
	case (AL_PMDT_UNIT_PCIe_MSTRAXI_7):
		config->trap_config.direction = AL_PMDT_MONITOR_WRITE_DIR;
		config->track_config.direction = AL_PMDT_MONITOR_WRITE_DIR;
		config->timeout_config.direction = AL_PMDT_MONITOR_WRITE_DIR;
		config->latency_config.direction = AL_PMDT_MONITOR_WRITE_DIR;
		config->timeout_config.write_timeout_us = AL_PMDT_SB_AXI_TRANS_DEFAULT_TIMEOUT;
		/* Initialize default latency bins */
		config->latency_config.write_bar_thr[0] = AL_PMDT_PCIE_WRITE_ACCESS_THRHLD_BIN_0;
		config->latency_config.write_bar_thr[1] = AL_PMDT_PCIE_WRITE_ACCESS_THRHLD_BIN_1;
		config->latency_config.write_bar_thr[2] = AL_PMDT_PCIE_WRITE_ACCESS_THRHLD_BIN_2;
		config->latency_config.write_bar_thr[3] = AL_PMDT_PCIE_WRITE_ACCESS_THRHLD_BIN_3;
		config->latency_config.write_bar_thr[4] = AL_PMDT_PCIE_WRITE_ACCESS_THRHLD_BIN_4;
		/*TODO: AXI response field in PCIE depends on it configuration
		 * For PCIex4 Master position it is 120 and for PCIeX8,16 it is 119.
		 * Right way how to define position according to PCIE configuration should be found
		 */
		config->trap_config.axi_resp_field_position =
				AL_PMDT_PCIE_MASTER_X4_UNIT_AXI_TRANS_RESP_BITS_START_POSITION;
		break;
	case (AL_PMDT_UNIT_UDMA_0_SSMAE_0):
	case (AL_PMDT_UNIT_UDMA_1_SSMAE_0):
	case (AL_PMDT_UNIT_UDMA_2_SSMAE_0):
	case (AL_PMDT_UNIT_UDMA_3_SSMAE_0):
	case (AL_PMDT_UNIT_UDMA_0_SSMAE_1):
	case (AL_PMDT_UNIT_UDMA_1_SSMAE_1):
	case (AL_PMDT_UNIT_UDMA_2_SSMAE_1):
	case (AL_PMDT_UNIT_UDMA_3_SSMAE_1):
	case (AL_PMDT_UNIT_NBC):
	case (AL_PMDT_UNIT_DDR_CONTROLER_0):
	case (AL_PMDT_UNIT_DDR_CONTROLER_1):
	case (AL_PMDT_UNIT_PCIECORE_0):
	case (AL_PMDT_UNIT_PCIECORE_1):
	case (AL_PMDT_UNIT_PCIECORE_2):
	case (AL_PMDT_UNIT_PCIECORE_3):
	case (AL_PMDT_UNIT_PCIECORE_4):
	case (AL_PMDT_UNIT_PCIECORE_5):
	case (AL_PMDT_UNIT_PCIECORE_6):
	case (AL_PMDT_UNIT_PCIECORE_7):
	case (AL_PMDT_UNIT_EC_ETH_100_0):
	case (AL_PMDT_UNIT_EC_ETH_100_1):
	case (AL_PMDT_UNIT_EC_ETH_100_2):
	case (AL_PMDT_UNIT_EC_ETH_100_3):
	case (AL_PMDT_UNIT_SHARED_RESOURCE_ETH_100_0):
	case (AL_PMDT_UNIT_UDMA_ETH_100_0):
	case (AL_PMDT_UNIT_UDMA_ETH_100_1):
	case (AL_PMDT_UNIT_UDMA_ETH_100_2):
	case (AL_PMDT_UNIT_UDMA_ETH_100_3):
		config->feature_mask = 0;
		break;
	default:
		al_err(" Illegal PMDTU id (%d)\n", pmdtu);
		return -1;
	}

	return 0;
}
