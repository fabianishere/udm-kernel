#include "samples.h"
#include "al_hal_addr_map.h"
#include "al_hal_iomap.h"

static const struct al_addr_map_tgt_enforcement_slave_cfg_entry tgt_enforcement_slv_cfg[] = {
	{
		.slave = AL_ADDR_MAP_TGT_ENFORCEMENT_SLV_ADDR_DEC,
		.axuser_19_16_val = 0,
		.axuser_19_16_mask = 0,
	},
	{
		.slave = AL_ADDR_MAP_TGT_ENFORCEMENT_SLV_PCIE_0,
		.axuser_19_16_val = 0x8,
		.axuser_19_16_mask = 0xe,
	},
	{
		.slave = AL_ADDR_MAP_TGT_ENFORCEMENT_SLV_PCIE_1,
		.axuser_19_16_val = 0xa,
		.axuser_19_16_mask = 0xe,
	},
	{
		.slave = AL_ADDR_MAP_TGT_ENFORCEMENT_SLV_PCIE_2,
		.axuser_19_16_val = 0xc,
		.axuser_19_16_mask = 0xe,
	},
	{
		.slave = AL_ADDR_MAP_TGT_ENFORCEMENT_SLV_PCIE_3,
		.axuser_19_16_val = 0xe,
		.axuser_19_16_mask = 0xe,
	},
	{
		.slave = AL_ADDR_MAP_TGT_ENFORCEMENT_SLV_PCIE_4,
		.axuser_19_16_val = 0,
		.axuser_19_16_mask = 0,
	},
	{
		.slave = AL_ADDR_MAP_TGT_ENFORCEMENT_SLV_PCIE_5,
		.axuser_19_16_val = 0,
		.axuser_19_16_mask = 0,
	},
	{
		.slave = AL_ADDR_MAP_TGT_ENFORCEMENT_SLV_PCIE_6,
		.axuser_19_16_val = 0,
		.axuser_19_16_mask = 0,
	},
	{
		.slave = AL_ADDR_MAP_TGT_ENFORCEMENT_SLV_PCIE_7,
		.axuser_19_16_val = 0,
		.axuser_19_16_mask = 0,
	},
	{
		.slave = AL_ADDR_MAP_TGT_ENFORCEMENT_SLV_DRAM,
		.axuser_19_16_val = 0x4,
		.axuser_19_16_mask = 0xc,
	},
};

static const struct al_addr_map_tgt_enforcement_master_cfg_entry tgt_enforcement_mst_cfg_eth0[] = {
	{
		.master = AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_ETH_FAST_0,
		.slv_cfg_entries = tgt_enforcement_slv_cfg,
		.slv_cfg_entries_num = AL_ARR_SIZE(tgt_enforcement_slv_cfg),
	},
	{
		.master = AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_ETH_FAST_1,
		.slv_cfg_entries = tgt_enforcement_slv_cfg,
		.slv_cfg_entries_num = AL_ARR_SIZE(tgt_enforcement_slv_cfg),
	},
};

static void pbs_addr_map_target_enforcement_sample(void)
{
	al_addr_map_tgt_enforcement_cfg_set(
		(void __iomem *)AL_PBS_REGFILE_BASE,
		tgt_enforcement_mst_cfg_eth0,
		AL_ARR_SIZE(tgt_enforcement_mst_cfg_eth0));
}

int main(void)
{
	pbs_addr_map_target_enforcement_sample();

	return 0;
}

