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

#include "../../include/al_hal_iomap.h"
#include "../../include/al_hal_pmdt_map.h"

/******************************************************************************
 ******************************************************************************/
int al_pmdt_map_entry_get(
		unsigned int region __attribute__((__unused__)),
		enum al_pmdt_unit id,
		struct al_bootstrap *bs,
		struct al_pmdt_map_entry *entry)
{
	if (id >= AL_PMDT_UNIT_MAX || entry == NULL)
		return -1;

	switch (id) {
	case AL_PMDT_UNIT_ADAPT_SSMAE_0:
		entry->name = "ADAPT_SSMAE_0";
		entry->core_freq = bs->sb_clk_freq;
		entry->regs_base = (void __iomem *)AL_SSM_PMDT_BASE(0);
		entry->cctm_grp = 0;
		entry->cctm_pmu_bit = AL_CCTM_GRP_A_PMU_ADAPT_SSMAE_0;
		entry->cctm_ela_bit = AL_CCTM_GRP_A_ELA_0_ADAPT_SSMAE_0;
		entry->default_axi_read = AL_TRUE;
		break;
	case AL_PMDT_UNIT_ADAPT_SSMAE_1:
		entry->name = "ADAPT_SSMAE_1";
		entry->core_freq = bs->sb_clk_freq;
		entry->regs_base = (void __iomem *)AL_SSM_PMDT_BASE(1);
		entry->cctm_grp = 0;
		entry->cctm_pmu_bit = AL_CCTM_GRP_A_PMU_ADAPT_SSMAE_1;
		entry->cctm_ela_bit = AL_CCTM_GRP_A_ELA_0_ADAPT_SSMAE_1;
		entry->default_axi_read = AL_TRUE;
		break;
	case AL_PMDT_UNIT_UDMA_0_SSMAE_0:
		entry->name = "UDMA_0_SSMAE_0";
		entry->core_freq = bs->sb_clk_freq;
		entry->regs_base = (void __iomem *)AL_SSM_UDMA_PMDT_BASE(0, 0);
		entry->cctm_grp = 0;
		entry->cctm_pmu_bit = AL_CCTM_GRP_A_PMU_UDMA_0_SSMAE_0;
		entry->cctm_ela_bit = AL_CCTM_GRP_A_ELA_UDMA_0_SSMA_0;
		entry->default_axi_read = AL_TRUE;
		break;
	case AL_PMDT_UNIT_UDMA_1_SSMAE_0:
		entry->name = "UDMA_1_SSMAE_0";
		entry->core_freq = bs->sb_clk_freq;
		entry->regs_base = (void __iomem *)AL_SSM_UDMA_PMDT_BASE(0, 1);
		entry->cctm_grp = 0;
		entry->cctm_pmu_bit = AL_CCTM_GRP_A_PMU_UDMA_1_SSMAE_0;
		entry->cctm_ela_bit = AL_CCTM_GRP_A_ELA_UDMA_1_SSMA_0;
		entry->default_axi_read = AL_TRUE;
		break;
	case AL_PMDT_UNIT_UDMA_2_SSMAE_0:
		entry->name = "UDMA_2_SSMAE_0";
		entry->core_freq = bs->sb_clk_freq;
		entry->regs_base = (void __iomem *)AL_SSM_UDMA_PMDT_BASE(0, 2);
		entry->cctm_grp = 0;
		entry->cctm_pmu_bit = AL_CCTM_GRP_A_PMU_UDMA_2_SSMAE_0;
		entry->cctm_ela_bit = AL_CCTM_GRP_A_ELA_UDMA_2_SSMA_0;
		entry->default_axi_read = AL_TRUE;
		break;
	case AL_PMDT_UNIT_UDMA_3_SSMAE_0:
		entry->name = "UDMA_3_SSMAE_0";
		entry->core_freq = bs->sb_clk_freq;
		entry->regs_base = (void __iomem *)AL_SSM_UDMA_PMDT_BASE(0, 3);
		entry->cctm_grp = 0;
		entry->cctm_pmu_bit = AL_CCTM_GRP_A_PMU_UDMA_3_SSMAE_0;
		entry->cctm_ela_bit = AL_CCTM_GRP_A_ELA_UDMA_3_SSMA_0;
		entry->default_axi_read = AL_TRUE;
		break;
	case AL_PMDT_UNIT_UDMA_0_SSMAE_1:
		entry->name = "UDMA_0_SSMAE_1";
		entry->core_freq = bs->sb_clk_freq;
		entry->regs_base = (void __iomem *)AL_SSM_UDMA_PMDT_BASE(1, 0);
		entry->cctm_grp = 0;
		entry->cctm_pmu_bit = AL_CCTM_GRP_A_PMU_UDMA_0_SSMAE_1;
		entry->cctm_ela_bit = AL_CCTM_GRP_A_ELA_UDMA_0_SSMA_1;
		entry->default_axi_read = AL_TRUE;
		break;
	case AL_PMDT_UNIT_UDMA_1_SSMAE_1:
		entry->name = "UDMA_1_SSMAE_1";
		entry->core_freq = bs->sb_clk_freq;
		entry->regs_base = (void __iomem *)AL_SSM_UDMA_PMDT_BASE(1, 1);
		entry->cctm_grp = 0;
		entry->cctm_pmu_bit = AL_CCTM_GRP_A_PMU_UDMA_1_SSMAE_1;
		entry->cctm_ela_bit = AL_CCTM_GRP_A_ELA_UDMA_1_SSMA_1;
		entry->default_axi_read = AL_TRUE;
		break;
	case AL_PMDT_UNIT_UDMA_2_SSMAE_1:
		entry->name = "UDMA_2_SSMAE_1";
		entry->core_freq = bs->sb_clk_freq;
		entry->regs_base = (void __iomem *)AL_SSM_UDMA_PMDT_BASE(1, 2);
		entry->cctm_grp = 0;
		entry->cctm_pmu_bit = AL_CCTM_GRP_A_PMU_UDMA_2_SSMAE_1;
		entry->cctm_ela_bit = AL_CCTM_GRP_A_ELA_UDMA_2_SSMA_1;
		entry->default_axi_read = AL_TRUE;
		break;
	case AL_PMDT_UNIT_UDMA_3_SSMAE_1:
		entry->name = "UDMA_3_SSMAE_1";
		entry->core_freq = bs->sb_clk_freq;
		entry->regs_base = (void __iomem *)AL_SSM_UDMA_PMDT_BASE(1, 3);
		entry->cctm_grp = 0;
		entry->cctm_pmu_bit = AL_CCTM_GRP_A_PMU_UDMA_3_SSMAE_1;
		entry->cctm_ela_bit = AL_CCTM_GRP_A_ELA_UDMA_3_SSMA_1;
		entry->default_axi_read = AL_TRUE;
		break;
	case AL_PMDT_UNIT_ADAPT_ETH_10_0:
		entry->name = "ADAPT_ETH_10_0";
		entry->core_freq = bs->sb_clk_freq;
		entry->regs_base = (void __iomem *)AL_ETH_PMDT_BASE(1);
		entry->cctm_grp = 0;
		entry->cctm_pmu_bit = AL_CCTM_GRP_A_PMU_ADAPT_ETH_10_0;
		entry->cctm_ela_bit = AL_CCTM_GRP_A_ELA_0_ADAPT_ETH_10_0;
		entry->default_axi_read = AL_TRUE;
		break;
	case AL_PMDT_UNIT_ADAPT_ETH_10_1:
		entry->name = "ADAPT_ETH_10_1";
		entry->core_freq = bs->sb_clk_freq;
		entry->regs_base = (void __iomem *)AL_ETH_PMDT_BASE(3);
		entry->cctm_grp = 0;
		entry->cctm_pmu_bit = AL_CCTM_GRP_A_PMU_ADAPT_ETH_10_1;
		entry->cctm_ela_bit = AL_CCTM_GRP_A_ELA_0_ADAPT_ETH_10_1;
		entry->default_axi_read = AL_TRUE;
		break;
	case AL_PMDT_UNIT_NBC:
		return -1;
	case AL_PMDT_UNIT_DDR_CONTROLER_0:
		return -1;
	case AL_PMDT_UNIT_DDR_CONTROLER_1:
		return -1;
	case AL_PMDT_UNIT_ADAPT_ETH_100_0:
		entry->name = "ADAPT_ETH_100_0";
		entry->core_freq = bs->sb_clk_freq;
		entry->regs_base = (void __iomem *)AL_ETH_PMDT_BASE(0);
		entry->cctm_grp = 1;
		entry->cctm_pmu_bit = AL_CCTM_GRP_B_PMU_ADAPT_ETH_100_0;
		entry->cctm_ela_bit = AL_CCTM_GRP_B_ELA_0_ADAPT_ETH_100_0;
		entry->default_axi_read = AL_TRUE;
		break;
	case AL_PMDT_UNIT_ADAPT_ETH_100_1:
		entry->name = "ADAPT_ETH_100_1";
		entry->core_freq = bs->sb_clk_freq;
		entry->regs_base = (void __iomem *)AL_ETH_PMDT_BASE(2);
		entry->cctm_grp = 1;
		entry->cctm_pmu_bit = AL_CCTM_GRP_B_PMU_ADAPT_ETH_100_1;
		entry->cctm_ela_bit = AL_CCTM_GRP_B_ELA_0_ADAPT_ETH_100_1;
		entry->default_axi_read = AL_TRUE;
		break;
	case AL_PMDT_UNIT_ADAPT_ETH_100_2:
		entry->name = "ADAPT_ETH_100_2";
		entry->core_freq = bs->sb_clk_freq;
		entry->regs_base = (void __iomem *)AL_ETH_PMDT_BASE(4);
		entry->cctm_grp = 1;
		entry->cctm_pmu_bit = AL_CCTM_GRP_B_PMU_ADAPT_ETH_100_2;
		entry->cctm_ela_bit = AL_CCTM_GRP_B_ELA_0_ADAPT_ETH_100_2;
		entry->default_axi_read = AL_TRUE;
		break;
	case AL_PMDT_UNIT_ADAPT_ETH_100_3:
		entry->name = "ADAPT_ETH_100_3";
		entry->core_freq = bs->sb_clk_freq;
		entry->regs_base = (void __iomem *)AL_ETH_PMDT_BASE(5);
		entry->cctm_grp = 1;
		entry->cctm_pmu_bit = AL_CCTM_GRP_B_PMU_ADAPT_ETH_100_3;
		entry->cctm_ela_bit = AL_CCTM_GRP_B_ELA_0_ADAPT_ETH_100_3;
		entry->default_axi_read = AL_TRUE;
		break;
	case AL_PMDT_UNIT_UDMA_ETH_100_0:
		entry->name = "UDMA_ETH_100_0";
		entry->core_freq = bs->sb_clk_freq;
		entry->regs_base = (void __iomem *)AL_ETH_UDMA_PMDT_BASE(0, 0);
		entry->cctm_grp = 1;
		entry->cctm_pmu_bit = AL_CCTM_GRP_B_PMU_UDMA_ETH_100_0;
		entry->cctm_ela_bit = AL_CCTM_GRP_B_ELA_UDMA_ETH_100_0;
		entry->default_axi_read = AL_TRUE;
		break;
	case AL_PMDT_UNIT_UDMA_ETH_100_1:
		entry->name = "UDMA_ETH_100_1";
		entry->core_freq = bs->sb_clk_freq;
		entry->regs_base = (void __iomem *)AL_ETH_UDMA_PMDT_BASE(2, 0);
		entry->cctm_grp = 1;
		entry->cctm_pmu_bit = AL_CCTM_GRP_B_PMU_UDMA_ETH_100_1;
		entry->cctm_ela_bit = AL_CCTM_GRP_B_ELA_UDMA_ETH_100_1;
		entry->default_axi_read = AL_TRUE;
		break;
	case AL_PMDT_UNIT_UDMA_ETH_100_2:
		entry->name = "UDMA_ETH_100_2";
		entry->core_freq = bs->sb_clk_freq;
		entry->regs_base = (void __iomem *)AL_ETH_UDMA_PMDT_BASE(4, 0);
		entry->cctm_grp = 1;
		entry->cctm_pmu_bit = AL_CCTM_GRP_B_PMU_UDMA_ETH_100_2;
		entry->cctm_ela_bit = AL_CCTM_GRP_B_ELA_UDMA_ETH_100_2;
		entry->default_axi_read = AL_TRUE;
		break;
	case AL_PMDT_UNIT_UDMA_ETH_100_3:
		entry->name = "UDMA_ETH_100_3";
		entry->core_freq = bs->sb_clk_freq;
		entry->regs_base = (void __iomem *)AL_ETH_UDMA_PMDT_BASE(5, 0);
		entry->cctm_grp = 1;
		entry->cctm_pmu_bit = AL_CCTM_GRP_B_PMU_UDMA_ETH_100_3;
		entry->cctm_ela_bit = AL_CCTM_GRP_B_ELA_UDMA_ETH_100_3;
		entry->default_axi_read = AL_TRUE;
		break;
	case AL_PMDT_UNIT_EC_ETH_100_0:
		return -1;
	case AL_PMDT_UNIT_SHARED_RESOURCE_ETH_100_0:
		return -1;
	case AL_PMDT_UNIT_SHARED_CACHE_ETH_100_0:
		return -1;
	case AL_PMDT_UNIT_CPU_CLUSTER_0:
		return -1;
	case AL_PMDT_UNIT_CPU_CLUSTER_1:
		return -1;
	case AL_PMDT_UNIT_CPU_CLUSTER_2:
		return -1;
	case AL_PMDT_UNIT_CPU_CLUSTER_3:
		return -1;
	case AL_PMDT_UNIT_ADAPT_SATA_0:
		entry->name = "ADAPT_SATA_0";
		entry->core_freq = bs->sb_clk_freq;
		entry->regs_base = (void __iomem *)AL_SATA_PMDT_BASE(0);
		entry->cctm_grp = 2;
		entry->cctm_pmu_bit = AL_CCTM_GRP_C_PMU_ADAPT_SATA_0;
		entry->cctm_ela_bit = AL_CCTM_GRP_C_ELA_ADAPT_SATA_0;
		entry->default_axi_read = AL_TRUE;
		break;
	case AL_PMDT_UNIT_ADAPT_SATA_1:
		entry->name = "ADAPT_SATA_1";
		entry->core_freq = bs->sb_clk_freq;
		entry->regs_base = (void __iomem *)AL_SATA_PMDT_BASE(1);
		entry->cctm_grp = 2;
		entry->cctm_pmu_bit = AL_CCTM_GRP_C_PMU_ADAPT_SATA_1;
		entry->cctm_ela_bit = AL_CCTM_GRP_C_ELA_ADAPT_SATA_1;
		entry->default_axi_read = AL_TRUE;
		break;
	case AL_PMDT_UNIT_ADAPT_SATA_2:
		entry->name = "ADAPT_SATA_2";
		entry->core_freq = bs->sb_clk_freq;
		entry->regs_base = (void __iomem *)AL_SATA_PMDT_BASE(2);
		entry->cctm_grp = 2;
		entry->cctm_pmu_bit = AL_CCTM_GRP_C_PMU_ADAPT_SATA_2;
		entry->cctm_ela_bit = AL_CCTM_GRP_C_ELA_ADAPT_SATA_2;
		entry->default_axi_read = AL_TRUE;
		break;
	case AL_PMDT_UNIT_ADAPT_SATA_3:
		entry->name = "ADAPT_SATA_3";
		entry->core_freq = bs->sb_clk_freq;
		entry->regs_base = (void __iomem *)AL_SATA_PMDT_BASE(3);
		entry->cctm_grp = 2;
		entry->cctm_pmu_bit = AL_CCTM_GRP_C_PMU_ADAPT_SATA_3;
		entry->cctm_ela_bit = AL_CCTM_GRP_C_ELA_ADAPT_SATA_3;
		entry->default_axi_read = AL_TRUE;
		break;
	case AL_PMDT_UNIT_ADAPT_SATA_4:
		entry->name = "ADAPT_SATA_4";
		entry->core_freq = bs->sb_clk_freq;
		entry->regs_base = (void __iomem *)AL_SATA_PMDT_BASE(4);
		entry->cctm_grp = 2;
		entry->cctm_pmu_bit = AL_CCTM_GRP_C_PMU_ADAPT_SATA_4;
		entry->cctm_ela_bit = AL_CCTM_GRP_C_ELA_ADAPT_SATA_4;
		entry->default_axi_read = AL_TRUE;
		break;
	case AL_PMDT_UNIT_ADAPT_SATA_5:
		entry->name = "ADAPT_SATA_5";
		entry->core_freq = bs->sb_clk_freq;
		entry->regs_base = (void __iomem *)AL_SATA_PMDT_BASE(5);
		entry->cctm_grp = 2;
		entry->cctm_pmu_bit = AL_CCTM_GRP_C_PMU_ADAPT_SATA_5;
		entry->cctm_ela_bit = AL_CCTM_GRP_C_ELA_ADAPT_SATA_5;
		entry->default_axi_read = AL_TRUE;
		break;
	case AL_PMDT_UNIT_ADAPT_SATA_6:
		entry->name = "ADAPT_SATA_6";
		entry->core_freq = bs->sb_clk_freq;
		entry->regs_base = (void __iomem *)AL_SATA_PMDT_BASE(6);
		entry->cctm_grp = 2;
		entry->cctm_pmu_bit = AL_CCTM_GRP_C_PMU_ADAPT_SATA_6;
		entry->cctm_ela_bit = AL_CCTM_GRP_C_ELA_ADAPT_SATA_6;
		entry->default_axi_read = AL_TRUE;
		break;
	case AL_PMDT_UNIT_ADAPT_SATA_7:
		entry->name = "ADAPT_SATA_7";
		entry->core_freq = bs->sb_clk_freq;
		entry->regs_base = (void __iomem *)AL_SATA_PMDT_BASE(7);
		entry->cctm_grp = 2;
		entry->cctm_pmu_bit = AL_CCTM_GRP_C_PMU_ADAPT_SATA_7;
		entry->cctm_ela_bit = AL_CCTM_GRP_C_ELA_ADAPT_SATA_7;
		entry->default_axi_read = AL_TRUE;
		break;
	case AL_PMDT_UNIT_PCIECORE_0:
		return -1;
	case AL_PMDT_UNIT_PCIECORE_1:
		return -1;
	case AL_PMDT_UNIT_PCIECORE_2:
		return -1;
	case AL_PMDT_UNIT_PCIECORE_3:
		return -1;
	case AL_PMDT_UNIT_PCIECORE_4:
		return -1;
	case AL_PMDT_UNIT_PCIECORE_5:
		return -1;
	case AL_PMDT_UNIT_PCIECORE_6:
		return -1;
	case AL_PMDT_UNIT_PCIECORE_7:
		return -1;
	case AL_PMDT_UNIT_PCIe_SLVAXI_0:
		return -1;
	case AL_PMDT_UNIT_PCIe_SLVAXI_1:
		return -1;
	case AL_PMDT_UNIT_PCIe_SLVAXI_2:
		return -1;
	case AL_PMDT_UNIT_PCIe_SLVAXI_3:
		return -1;
	case AL_PMDT_UNIT_PCIe_SLVAXI_4:
		return -1;
	case AL_PMDT_UNIT_PCIe_SLVAXI_5:
		return -1;
	case AL_PMDT_UNIT_PCIe_SLVAXI_6:
		return -1;
	case AL_PMDT_UNIT_PCIe_SLVAXI_7:
		return -1;
	case AL_PMDT_UNIT_PCIe_MSTRAXI_0:
		return -1;
	case AL_PMDT_UNIT_PCIe_MSTRAXI_1:
		return -1;
	case AL_PMDT_UNIT_PCIe_MSTRAXI_2:
		return -1;
	case AL_PMDT_UNIT_PCIe_MSTRAXI_3:
		return -1;
	case AL_PMDT_UNIT_PCIe_MSTRAXI_4:
		return -1;
	case AL_PMDT_UNIT_PCIe_MSTRAXI_5:
		return -1;
	case AL_PMDT_UNIT_PCIe_MSTRAXI_6:
		return -1;
	case AL_PMDT_UNIT_PCIe_MSTRAXI_7:
		return -1;
	default:
		return -1;
	}

	return 0;
}

int al_pmdt_map_trig_num_get(
		unsigned int axi_mon_num,
		unsigned int axi_mon_instance,
		enum al_pmdt_pmu_int_trig_type type,
		unsigned int index)
{
	int trig_num = -1;

	if (axi_mon_num < 1 ||
		axi_mon_num > 2 ||
		axi_mon_instance >= axi_mon_num)
		return -1;

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
		if (index >= 6)
			return -1;
		trig_num = (axi_mon_instance == 0) ? 64 + index : 70 + index;
		break;
	case AL_PMDT_INT_TRIG_TRACKER_INC_1:
		if (index >= 4)
			return -1;
		if (axi_mon_num == 1)
			trig_num = 70 + index;
		else /* axi_mon_num == 2 */
			trig_num = (axi_mon_instance == 0) ? 76 + index : 80 + index;
		break;
	case AL_PMDT_INT_TRIG_TRACKER_INC_VAL:
		if (index >= 4)
			return -1;
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

	if (axi_mon_num < 1 ||
		axi_mon_num > 2 ||
		axi_mon_instance >= axi_mon_num)
		return -1;

	/*
	 * Single AXI monitor:
	 *	AXI_MON_0 - 10
	 * Dual AXI monitors:
	 *	AXI_MON_0 - 9
	 *	AXI_MON_1 - 10
	 */

	if (axi_mon_num == 1)
		signal_group = AL_BIT(10);
	else
		if (axi_mon_instance == 0)
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

	if (axi_mon_num < 1 ||
		axi_mon_num > 2 ||
		axi_mon_instance >= axi_mon_num)
		return -1;

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
		if (axi_mon_num == 1)
			irq_num = (axi_mon_instance == 0) ? 0 : 1;
		else
			irq_num = 0;
		break;
	case AL_PMDT_AXIMON_OVERFLOW_IRQ:
		if (axi_mon_num == 1)
			irq_num = (axi_mon_instance == 0) ? 2 : 3;
		else
			irq_num = 1;
		break;
	case AL_PMDT_AXIMON_UNDERFLOW_IRQ:
		if (axi_mon_num == 1)
			irq_num = (axi_mon_instance == 0) ? 4 : 5;
		else
			irq_num = 3;
		break;
	}

	return irq_num;
}
