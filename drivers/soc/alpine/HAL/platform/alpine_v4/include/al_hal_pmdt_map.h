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

/**
 * This file contains PMDT device mapping definitions
 */

#ifndef __AL_HAL_PMDT_MAP_H__
#define __AL_HAL_PMDT_MAP_H__

#include "al_hal_reg_utils.h"
#include "al_hal_bootstrap.h"

#define AL_PMDT_REGION_NUM				1

/* CCTM Mapping */
/* Group A */
#define AL_CCTM_GRP_A_PMU_ADAPT_SSMAE_0					AL_BIT(0)
#define AL_CCTM_GRP_A_PMU_ADAPT_SSMAE_1					AL_BIT(1)
#define AL_CCTM_GRP_A_ELA_0_ADAPT_SSMAE_0				AL_BIT(2)
#define AL_CCTM_GRP_A_ELA_0_ADAPT_SSMAE_1				AL_BIT(3)
#define AL_CCTM_GRP_A_PMU_UDMA_0_SSMAE_0				AL_BIT(4)
#define AL_CCTM_GRP_A_PMU_UDMA_1_SSMAE_0				AL_BIT(5)
#define AL_CCTM_GRP_A_PMU_UDMA_2_SSMAE_0				AL_BIT(6)
#define AL_CCTM_GRP_A_PMU_UDMA_3_SSMAE_0				AL_BIT(7)
#define AL_CCTM_GRP_A_PMU_UDMA_0_SSMAE_1				AL_BIT(8)
#define AL_CCTM_GRP_A_PMU_UDMA_1_SSMAE_1				AL_BIT(9)
#define AL_CCTM_GRP_A_PMU_UDMA_2_SSMAE_1				AL_BIT(10)
#define AL_CCTM_GRP_A_PMU_UDMA_3_SSMAE_1				AL_BIT(11)
#define AL_CCTM_GRP_A_ELA_UDMA_0_SSMA_0					AL_BIT(12)
#define AL_CCTM_GRP_A_ELA_UDMA_1_SSMA_0					AL_BIT(13)
#define AL_CCTM_GRP_A_ELA_UDMA_2_SSMA_0					AL_BIT(14)
#define AL_CCTM_GRP_A_ELA_UDMA_3_SSMA_0					AL_BIT(15)
#define AL_CCTM_GRP_A_ELA_UDMA_0_SSMA_1					AL_BIT(16)
#define AL_CCTM_GRP_A_ELA_UDMA_1_SSMA_1					AL_BIT(17)
#define AL_CCTM_GRP_A_ELA_UDMA_2_SSMA_1					AL_BIT(18)
#define AL_CCTM_GRP_A_ELA_UDMA_3_SSMA_1					AL_BIT(19)
#define AL_CCTM_GRP_A_PMU_ADAPT_ETH_10_0				AL_BIT(20)
#define AL_CCTM_GRP_A_ELA_0_ADAPT_ETH_10_0				AL_BIT(21)
#define AL_CCTM_GRP_A_PMU_ADAPT_ETH_10_1				AL_BIT(22)
#define AL_CCTM_GRP_A_ELA_0_ADAPT_ETH_10_1				AL_BIT(23)
#define AL_CCTM_GRP_A_PMU_NBC							AL_BIT(24)
/* skip 3 bits */
#define AL_CCTM_GRP_A_PMU_DDR_CONTROLER_0				AL_BIT(28)
#define AL_CCTM_GRP_A_PMU_DDR_CONTROLER_1				AL_BIT(29)
#define AL_CCTM_GRP_A_ELA_DDR_CONTROLER_0				AL_BIT(30)
#define AL_CCTM_GRP_A_ELA_DDR_CONTROLER_1				AL_BIT(31)

/* Group B */
#define AL_CCTM_GRP_B_PMU_ADAPT_ETH_100_0				AL_BIT(0)
#define AL_CCTM_GRP_B_PMU_ADAPT_ETH_100_1				AL_BIT(1)
#define AL_CCTM_GRP_B_PMU_ADAPT_ETH_100_2				AL_BIT(2)
#define AL_CCTM_GRP_B_PMU_ADAPT_ETH_100_3				AL_BIT(3)
#define AL_CCTM_GRP_B_ELA_0_ADAPT_ETH_100_0				AL_BIT(4)
#define AL_CCTM_GRP_B_ELA_0_ADAPT_ETH_100_1				AL_BIT(5)
#define AL_CCTM_GRP_B_ELA_0_ADAPT_ETH_100_2				AL_BIT(6)
#define AL_CCTM_GRP_B_ELA_0_ADAPT_ETH_100_3				AL_BIT(7)
#define AL_CCTM_GRP_B_PMU_UDMA_ETH_100_0				AL_BIT(8)
#define AL_CCTM_GRP_B_PMU_UDMA_ETH_100_1				AL_BIT(9)
#define AL_CCTM_GRP_B_PMU_UDMA_ETH_100_2				AL_BIT(10)
#define AL_CCTM_GRP_B_PMU_UDMA_ETH_100_3				AL_BIT(11)
#define AL_CCTM_GRP_B_ELA_UDMA_ETH_100_0				AL_BIT(12)
#define AL_CCTM_GRP_B_ELA_UDMA_ETH_100_1				AL_BIT(13)
#define AL_CCTM_GRP_B_ELA_UDMA_ETH_100_2				AL_BIT(14)
#define AL_CCTM_GRP_B_ELA_UDMA_ETH_100_3				AL_BIT(15)
#define AL_CCTM_GRP_B_PMU_EC_ETH_100_0					AL_BIT(16)
#define AL_CCTM_GRP_B_ELA_EC_ETH_100_0					AL_BIT(17)
#define AL_CCTM_GRP_B_PMU_SHARED_RESOURCE_ETH_100_0		AL_BIT(18)
#define AL_CCTM_GRP_B_ELA_SHARED_RESOURCE_ETH_100_0		AL_BIT(19)
#define AL_CCTM_GRP_B_PMU_SHARED_CACHE_ETH_100_0		AL_BIT(20)
#define AL_CCTM_GRP_B_ELA_SHARED_CACHE_ETH_100_0		AL_BIT(21)
/* skip 2 bits */
#define AL_CCTM_GRP_B_PMU_CPU_CLUSTER_0					AL_BIT(24)
#define AL_CCTM_GRP_B_PMU_CPU_CLUSTER_1					AL_BIT(25)
#define AL_CCTM_GRP_B_PMU_CPU_CLUSTER_2					AL_BIT(26)
#define AL_CCTM_GRP_B_PMU_CPU_CLUSTER_3					AL_BIT(27)
#define AL_CCTM_GRP_B_ELA_CPU_CLUSTER_0					AL_BIT(28)
#define AL_CCTM_GRP_B_ELA_CPU_CLUSTER_1					AL_BIT(29)
#define AL_CCTM_GRP_B_ELA_CPU_CLUSTER_2					AL_BIT(30)
#define AL_CCTM_GRP_B_ELA_CPU_CLUSTER_3					AL_BIT(31)

/* Group C */
#define AL_CCTM_GRP_C_PMU_ADAPT_SATA_0					AL_BIT(0)
#define AL_CCTM_GRP_C_PMU_ADAPT_SATA_1					AL_BIT(1)
#define AL_CCTM_GRP_C_ELA_ADAPT_SATA_0					AL_BIT(2)
#define AL_CCTM_GRP_C_ELA_ADAPT_SATA_1					AL_BIT(3)
#define AL_CCTM_GRP_C_PMU_ADAPT_SATA_2					AL_BIT(4)
#define AL_CCTM_GRP_C_PMU_ADAPT_SATA_3					AL_BIT(5)
#define AL_CCTM_GRP_C_ELA_ADAPT_SATA_2					AL_BIT(6)
#define AL_CCTM_GRP_C_ELA_ADAPT_SATA_3					AL_BIT(7)
#define AL_CCTM_GRP_C_PMU_ADAPT_SATA_4					AL_BIT(8)
#define AL_CCTM_GRP_C_PMU_ADAPT_SATA_5					AL_BIT(9)
#define AL_CCTM_GRP_C_ELA_ADAPT_SATA_4					AL_BIT(10)
#define AL_CCTM_GRP_C_ELA_ADAPT_SATA_5					AL_BIT(11)
#define AL_CCTM_GRP_C_PMU_ADAPT_SATA_6					AL_BIT(12)
#define AL_CCTM_GRP_C_PMU_ADAPT_SATA_7					AL_BIT(13)
#define AL_CCTM_GRP_C_ELA_ADAPT_SATA_6					AL_BIT(14)
#define AL_CCTM_GRP_C_ELA_ADAPT_SATA_7					AL_BIT(15)
#define AL_CCTM_GRP_C_PMU_PCIECORE_0					AL_BIT(16)
#define AL_CCTM_GRP_C_PMU_PCIECORE_1					AL_BIT(17)
#define AL_CCTM_GRP_C_PMU_PCIECORE_2					AL_BIT(18)
#define AL_CCTM_GRP_C_PMU_PCIECORE_3					AL_BIT(19)
#define AL_CCTM_GRP_C_PMU_PCIECORE_4					AL_BIT(20)
#define AL_CCTM_GRP_C_PMU_PCIECORE_5					AL_BIT(21)
#define AL_CCTM_GRP_C_PMU_PCIECORE_6					AL_BIT(22)
#define AL_CCTM_GRP_C_PMU_PCIECORE_7					AL_BIT(23)
#define AL_CCTM_GRP_C_ELA_PCIECORE_0					AL_BIT(24)
#define AL_CCTM_GRP_C_ELA_PCIECORE_1					AL_BIT(25)
#define AL_CCTM_GRP_C_ELA_PCIECORE_2					AL_BIT(26)
#define AL_CCTM_GRP_C_ELA_PCIECORE_3					AL_BIT(27)
#define AL_CCTM_GRP_C_ELA_PCIECORE_4					AL_BIT(28)
#define AL_CCTM_GRP_C_ELA_PCIECORE_5					AL_BIT(29)
#define AL_CCTM_GRP_C_ELA_PCIECORE_6					AL_BIT(30)
#define AL_CCTM_GRP_C_ELA_PCIECORE_7					AL_BIT(31)

/* Group D */
#define AL_CCTM_GRP_D_PMU_PCIe_SLVAXI_0					AL_BIT(0)
#define AL_CCTM_GRP_D_PMU_PCIe_SLVAXI_1					AL_BIT(1)
#define AL_CCTM_GRP_D_PMU_PCIe_SLVAXI_2					AL_BIT(2)
#define AL_CCTM_GRP_D_PMU_PCIe_SLVAXI_3					AL_BIT(3)
#define AL_CCTM_GRP_D_PMU_PCIe_SLVAXI_4					AL_BIT(4)
#define AL_CCTM_GRP_D_PMU_PCIe_SLVAXI_5					AL_BIT(5)
#define AL_CCTM_GRP_D_PMU_PCIe_SLVAXI_6					AL_BIT(6)
#define AL_CCTM_GRP_D_PMU_PCIe_SLVAXI_7					AL_BIT(7)
#define AL_CCTM_GRP_D_ELA_PCIe_SLVAXI_0					AL_BIT(8)
#define AL_CCTM_GRP_D_ELA_PCIe_SLVAXI_1					AL_BIT(9)
#define AL_CCTM_GRP_D_ELA_PCIe_SLVAXI_2					AL_BIT(10)
#define AL_CCTM_GRP_D_ELA_PCIe_SLVAXI_3					AL_BIT(11)
#define AL_CCTM_GRP_D_ELA_PCIe_SLVAXI_4					AL_BIT(12)
#define AL_CCTM_GRP_D_ELA_PCIe_SLVAXI_5					AL_BIT(13)
#define AL_CCTM_GRP_D_ELA_PCIe_SLVAXI_6					AL_BIT(14)
#define AL_CCTM_GRP_D_ELA_PCIe_SLVAXI_7					AL_BIT(15)
#define AL_CCTM_GRP_D_PMU_PCIe_MSTRAXI_0				AL_BIT(16)
#define AL_CCTM_GRP_D_PMU_PCIe_MSTRAXI_1				AL_BIT(17)
#define AL_CCTM_GRP_D_PMU_PCIe_MSTRAXI_2				AL_BIT(18)
#define AL_CCTM_GRP_D_PMU_PCIe_MSTRAXI_3				AL_BIT(19)
#define AL_CCTM_GRP_D_PMU_PCIe_MSTRAXI_4				AL_BIT(20)
#define AL_CCTM_GRP_D_PMU_PCIe_MSTRAXI_5				AL_BIT(21)
#define AL_CCTM_GRP_D_PMU_PCIe_MSTRAXI_6				AL_BIT(22)
#define AL_CCTM_GRP_D_PMU_PCIe_MSTRAXI_7				AL_BIT(23)
#define AL_CCTM_GRP_D_ELA_PCIe_MSTRAXI_0				AL_BIT(24)
#define AL_CCTM_GRP_D_ELA_PCIe_MSTRAXI_1				AL_BIT(25)
#define AL_CCTM_GRP_D_ELA_PCIe_MSTRAXI_2				AL_BIT(26)
#define AL_CCTM_GRP_D_ELA_PCIe_MSTRAXI_3				AL_BIT(27)
#define AL_CCTM_GRP_D_ELA_PCIe_MSTRAXI_4				AL_BIT(28)
#define AL_CCTM_GRP_D_ELA_PCIe_MSTRAXI_5				AL_BIT(29)
#define AL_CCTM_GRP_D_ELA_PCIe_MSTRAXI_6				AL_BIT(30)
#define AL_CCTM_GRP_D_ELA_PCIe_MSTRAXI_7				AL_BIT(31)

#define AL_PMDT_UNIT_NAME_LEN 30
#define AL_PMDT_AXI_MON_MAX_NUM	2
#define AL_PMDT_ELA_MAX_NUM	2

enum al_pmdt_unit {
	AL_PMDT_UNIT_ADAPT_SSMAE_0,
	AL_PMDT_UNIT_ADAPT_SSMAE_1,
	AL_PMDT_UNIT_UDMA_0_SSMAE_0,
	AL_PMDT_UNIT_UDMA_1_SSMAE_0,
	AL_PMDT_UNIT_UDMA_2_SSMAE_0,
	AL_PMDT_UNIT_UDMA_3_SSMAE_0,
	AL_PMDT_UNIT_UDMA_0_SSMAE_1,
	AL_PMDT_UNIT_UDMA_1_SSMAE_1,
	AL_PMDT_UNIT_UDMA_2_SSMAE_1,
	AL_PMDT_UNIT_UDMA_3_SSMAE_1,
	AL_PMDT_UNIT_ADAPT_ETH_10_0,
	AL_PMDT_UNIT_ADAPT_ETH_10_1,
	AL_PMDT_UNIT_NBC,
	AL_PMDT_UNIT_DDR_CONTROLER_0,
	AL_PMDT_UNIT_DDR_CONTROLER_1,
	AL_PMDT_UNIT_ADAPT_ETH_100_0,
	AL_PMDT_UNIT_ADAPT_ETH_100_1,
	AL_PMDT_UNIT_ADAPT_ETH_100_2,
	AL_PMDT_UNIT_ADAPT_ETH_100_3,
	AL_PMDT_UNIT_UDMA_ETH_100_0,
	AL_PMDT_UNIT_UDMA_ETH_100_1,
	AL_PMDT_UNIT_UDMA_ETH_100_2,
	AL_PMDT_UNIT_UDMA_ETH_100_3,
	AL_PMDT_UNIT_EC_ETH_100_0,
	AL_PMDT_UNIT_SHARED_RESOURCE_ETH_100_0,
	AL_PMDT_UNIT_SHARED_CACHE_ETH_100_0,
	AL_PMDT_UNIT_CPU_CLUSTER_0,
	AL_PMDT_UNIT_CPU_CLUSTER_1,
	AL_PMDT_UNIT_CPU_CLUSTER_2,
	AL_PMDT_UNIT_CPU_CLUSTER_3,
	AL_PMDT_UNIT_ADAPT_SATA_0,
	AL_PMDT_UNIT_ADAPT_SATA_1,
	AL_PMDT_UNIT_ADAPT_SATA_2,
	AL_PMDT_UNIT_ADAPT_SATA_3,
	AL_PMDT_UNIT_ADAPT_SATA_4,
	AL_PMDT_UNIT_ADAPT_SATA_5,
	AL_PMDT_UNIT_ADAPT_SATA_6,
	AL_PMDT_UNIT_ADAPT_SATA_7,
	AL_PMDT_UNIT_PCIECORE_0,
	AL_PMDT_UNIT_PCIECORE_1,
	AL_PMDT_UNIT_PCIECORE_2,
	AL_PMDT_UNIT_PCIECORE_3,
	AL_PMDT_UNIT_PCIECORE_4,
	AL_PMDT_UNIT_PCIECORE_5,
	AL_PMDT_UNIT_PCIECORE_6,
	AL_PMDT_UNIT_PCIECORE_7,
	AL_PMDT_UNIT_PCIe_SLVAXI_0,
	AL_PMDT_UNIT_PCIe_SLVAXI_1,
	AL_PMDT_UNIT_PCIe_SLVAXI_2,
	AL_PMDT_UNIT_PCIe_SLVAXI_3,
	AL_PMDT_UNIT_PCIe_SLVAXI_4,
	AL_PMDT_UNIT_PCIe_SLVAXI_5,
	AL_PMDT_UNIT_PCIe_SLVAXI_6,
	AL_PMDT_UNIT_PCIe_SLVAXI_7,
	AL_PMDT_UNIT_PCIe_MSTRAXI_0,
	AL_PMDT_UNIT_PCIe_MSTRAXI_1,
	AL_PMDT_UNIT_PCIe_MSTRAXI_2,
	AL_PMDT_UNIT_PCIe_MSTRAXI_3,
	AL_PMDT_UNIT_PCIe_MSTRAXI_4,
	AL_PMDT_UNIT_PCIe_MSTRAXI_5,
	AL_PMDT_UNIT_PCIe_MSTRAXI_6,
	AL_PMDT_UNIT_PCIe_MSTRAXI_7,
	AL_PMDT_UNIT_MAX
};

/** PMU internal trigger types */
enum al_pmdt_pmu_int_trig_type {
	AL_PMDT_INT_TRIG_LATENCY, /** AXI_MON latency bar triggers */
	AL_PMDT_INT_TRIG_TRACKER_INC_1, /** tracker match trigger, +1 per event */
	AL_PMDT_INT_TRIG_TRACKER_INC_VAL, /** tracker match trigger, +inc_val */
};

/** PMU interrupts triggered by AXI Monitor (Group B) */
enum al_pmdt_axi_mon_irq_type {
	AL_PMDT_AXIMON_TIMEOUT_IRQ,
	AL_PMDT_AXIMON_OVERFLOW_IRQ,
	AL_PMDT_AXIMON_UNDERFLOW_IRQ,
};

struct al_pmdt_map_entry {
	enum al_pmdt_unit id;
	const char *name; /* unit name - up to AL_PMDT_UNIT_NAME_LEN chars */
	void __iomem *regs_base; /* unit regiter base address */
	void __iomem *bus_base; /* units bus base address (e.g. NB_BASE/SB_BASE */
	unsigned int core_freq; /* unit clock frequency */
	unsigned int cctm_grp; /* unit's cctm group */
	unsigned int cctm_pmu_bit; /* unit's cctm pmu bit (in the above group) */
	unsigned int cctm_ela_bit; /* unit's cctm ela bit (in the above group) */
	al_bool default_axi_read; /* default AXI_MON monitoring direction */
};

/**
 * Retrieve map entry for specific pmdt unit
 *
 * @param region
 *		pmdt unit region on multi-regions systems, ignored in single region systems
 * @param id
 *		pmdt unit ID
 * @param bs
 *		device bootstrap info
 * @param entry
 *		map entry (out)
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_pmdt_map_entry_get(
		unsigned int region,
		enum al_pmdt_unit id,
		struct al_bootstrap *bs,
		struct al_pmdt_map_entry *entry);


/**
 * Retrieve internal trigger number to be used
 * with al_pmdt_pmu_event_config.trig_num
 *
 * @param axi_mon_num
 *		number of axi_mon in this pmdt
 * @param axi_mon_instance
 *		axi monitor instance from which the trigger is fired
 * @param type
 *		trigger type
 * @param index
 *		trigger index (per type)
 *
 * @return	trigger number if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_pmdt_map_trig_num_get(
		unsigned int axi_mon_num,
		unsigned int axi_mon_instance,
		enum al_pmdt_pmu_int_trig_type type,
		unsigned int index);

/**
 * Retrieve axi_mon trace signal group in the ela.
 * To be used with al_pmdt_ela_trig_config.signal_group
 *
 * @param axi_mon_num
 *		number of axi_mon in this pmdt
 * @param axi_mon_instance
 *		axi monitor instance from which the signal is fired
 * @return	signal_group if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_pmdt_map_trace_sig_get(
		unsigned int axi_mon_num,
		unsigned int axi_mon_instance);

/**
 * Retrieve pmu group B interrupt bit number
 *
 * @param axi_mon_num
 *		number of axi_mon in this pmdt
 * @param axi_mon_instance
 *		axi monitor instance from which the trigger is fired
 * @param type
 *		irq type
 *
 * @return	trigger number if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_pmdt_map_pmu_irq_num_get(
		unsigned int axi_mon_num,
		unsigned int axi_mon_instance,
		enum al_pmdt_axi_mon_irq_type type);

#endif
