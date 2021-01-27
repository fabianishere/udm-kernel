/*
 * Copyright 2016, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */
#include "al_perf_params.h"
#include "al_bootstrap.h"
#include "al_hal_ccu_regs.h"
#include "al_hal_ddr.h"
#include "al_hal_sys_fabric_pasw.h"
#ifdef AL_HAL_EX
#include "al_hal_iommu.h"
#endif
#include "al_hal_udma_config.h"
#include "al_hal_unit_adapter.h"
#include "al_hal_pcie.h"
#if defined(__arm__)
#include "aarch32/monitor_mgmt.h"
#endif
#if defined(__aarch64__)
#include "aarch64/monitor_mgmt.h"
#endif

#if defined(__arm__) || defined(__aarch64__)
uint32_t __iomem *_secure_reg_read32_reg;
volatile uint32_t _secure_reg_read32_val;


static int _secure_reg_read32(void)
{
	_secure_reg_read32_val = al_reg_read32(_secure_reg_read32_reg);

	return 0;
}

static uint32_t secure_reg_read32(uint32_t __iomem *reg)
{
	_secure_reg_read32_reg = reg;
	monitor_run_secure(_secure_reg_read32);
	return _secure_reg_read32_val;
}
#else
static uint32_t secure_reg_read32(uint32_t __iomem *reg)
{
	return al_reg_read32(reg);
}
#endif

static void ddr_addrmap_bit_get(
	struct al_ddr_addrmap	*addrmap,
	unsigned int		sys_addr_bit,
	const char		**name,
	int			*idx)
{
	unsigned int i;

	if (addrmap->swap_14_16_with_17_19) {
		switch (sys_addr_bit) {
		case 14:
			sys_addr_bit = 17;
			break;
		case 15:
			sys_addr_bit = 18;
			break;
		case 16:
			sys_addr_bit = 19;
			break;
		case 17:
			sys_addr_bit = 14;
			break;
		case 18:
			sys_addr_bit = 15;
			break;
		case 19:
			sys_addr_bit = 16;
			break;
		}
	}

	for (i = 0; i < sizeof(addrmap->col_b3_9_b11_13); i++) {
		if (addrmap->col_b3_9_b11_13[i] == sys_addr_bit) {
			*name = "cl";
			*idx = (i <= 7) ? (i + 3) : (i + 4);
			return;
		}
	}
	for (i = 0; i < sizeof(addrmap->bank_b0_2); i++) {
		if (addrmap->bank_b0_2[i] == sys_addr_bit) {
			*name = "bk";
			*idx = i;
			return;
		}
	}
	for (i = 0; i < sizeof(addrmap->bg_b0_1); i++) {
		if (addrmap->bg_b0_1[i] == sys_addr_bit) {
			*name = "bg";
			*idx = i;
			return;
		}
	}
	for (i = 0; i < sizeof(addrmap->row_b0_17); i++) {
		if (addrmap->row_b0_17[i] == sys_addr_bit) {
			*name = "rw";
			*idx = i;
			return;
		}
	}
	for (i = 0; i < sizeof(addrmap->cs_b0_1); i++) {
		if (addrmap->cs_b0_1[i] == sys_addr_bit) {
			*name = "cs";
			*idx = i;
			return;
		}
	}
	for (i = 0; i < sizeof(addrmap->cid_b0_1); i++) {
		if (addrmap->cid_b0_1[i] == sys_addr_bit) {
			*name = "ci";
			*idx = i;
			return;
		}
	}

	*name = "N/A";
	*idx = -1;
}

#if defined(__arm__)
static uint32_t l2actlr_get(void)
{
	uint32_t reg_val;

	asm volatile("MRC p15, 1, %0, c15, c0, 0" : "=r" (reg_val));

	return reg_val;
}

static uint32_t l2ctr_get(void)
{
	uint32_t reg_val;

	asm volatile("MRC p15, 1, %0, c9, c0, 2" : "=r" (reg_val));

	return reg_val;
}
#elif defined(__aarch64__)
static uint32_t l2actlr_get(void)
{
	uint32_t reg_val;

	asm volatile("MRS %0, s3_1_c15_c0_0" : "=r" (reg_val));

	return reg_val;
}

static uint32_t l2ctr_get(void)
{
	uint32_t reg_val;

	asm volatile("MRS %0, s3_1_c11_c0_2" : "=r" (reg_val));

	return reg_val;
}
#endif

static void al_perf_params_print_cpu_l2_info(void)
{
#if defined(__arm__) || defined(__aarch64__)
	uint32_t l2actlr;
	uint32_t l2ctr;
	l2actlr = l2actlr_get();
	l2ctr = l2ctr_get();

	al_print("- l2actlr: %08x\n", l2actlr);
	al_print("  * Disable clean/evict push to external: %u\n", !!(l2actlr & AL_BIT(3)));
	al_print("  * Disable WriteUnique and WriteLineUnique transactions from master: %u\n",
		!!(l2actlr & AL_BIT(4)));
	al_print("  * Disable DVM and cache maintenance operation message broadcast: %u\n",
		!!(l2actlr & AL_BIT(8)));
	al_print("  * L2 PLRU insertion point: %u\n", (l2actlr >> 30) & 3);
	al_print("- l2ctr: %08x\n", l2ctr);
	al_print("  * Data ram latency: %u\n", (l2ctr >> 0) & 7);
	al_print("  * Tag ram latency: %u\n", (l2ctr >> 6) & 7);
	al_print("  * ECC enable: %u\n", !!(l2ctr & AL_BIT(21)));
#else
	al_print("- l2actlr & l2ctr are not available\n");
#endif
}

static int al_perf_params_print_cpu(void)
{
	struct al_bootstrap bootstrap;
	int err;

	err = al_bootstrap_get(&bootstrap);
	if (err) {
		al_err("%s: al_bootstrap_get failed!\n", __func__);
		return err;
	}

	al_print("CPU:\n");
	al_print("- CPU speed: %u MHz\n", bootstrap.cpu_pll_freq / 1000000);
	al_perf_params_print_cpu_l2_info();
	al_print("- Chicken bits: N/A\n");

	return 0;
}


__attribute__((weak, alias("__al_sys_fabric_pasw_dram_intrlv_stripe_bit_get")))
unsigned int al_sys_fabric_pasw_dram_intrlv_stripe_bit_get(
	void __iomem    *nb_regs_base __attribute__ ((unused)));

static unsigned int __al_sys_fabric_pasw_dram_intrlv_stripe_bit_get(
	void __iomem    *nb_regs_base __attribute__ ((unused)))
{
	return 0;
}

static int al_perf_params_print_ddr(
	const struct al_perf_params_print_params_ddr	*params)
{
	struct al_ddr_addrmap ddr_addrmap;
	struct al_ddr_ecc_cfg ddr_ecc_cfg;
	struct al_bootstrap bootstrap;
	struct al_ddr_cfg ddr_cfg;
	int err;
	int i;

	err = al_bootstrap_get(&bootstrap);
	if (err) {
		al_err("%s: al_bootstrap_get failed!\n", __func__);
		return err;
	}

	if (!params->ch0_ddrc_regs_base)
		al_ddr_cfg_init(
			params->nb_regs_base,
			params->ch0_ddr_core_regs_base,
			params->ch0_ddr_phy_regs_base,
			&ddr_cfg);
	else
		al_ddr_cfg_init_v3(
			params->nb_regs_base,
			params->ch0_ddr_core_regs_base,
			params->ch0_ddr_phy_regs_base,
			params->ch0_ddrc_regs_base,
			0,
			&ddr_cfg);

	al_ddr_address_map_get(&ddr_cfg, &ddr_addrmap);
	al_ddr_ecc_cfg_get(&ddr_cfg, &ddr_ecc_cfg);

	al_print("DDR:\n");
	al_print("- Reference clock freq: %u MHz\n", bootstrap.ddr_pll_freq / 1000000);
	al_print("- DDR ECC: %s\n", ddr_ecc_cfg.ecc_enabled ? "enabled" : "disabled");
	al_print("- Address map:\n");
	al_print("  ");
	for (i = 39; i >= 20; i--)
		al_print("%4u ", i);
	al_print("\n");
	al_print("  ");
	for (i = 39; i >= 20; i--) {
		const char *name;
		int idx;

		ddr_addrmap_bit_get(&ddr_addrmap, i, &name, &idx);
		if (idx < 0)
			al_print("N/A  ");
		else
			al_print("%s%02d ", name, idx);
	}
	al_print("\n");
	al_print("  ");
	for (i = 19; i >= 0; i--)
		al_print("%4u ", i);
	al_print("\n");
	al_print("  ");
	for (i = 19; i >= 0; i--) {
		const char *name;
		int idx;

		ddr_addrmap_bit_get(&ddr_addrmap, i, &name, &idx);
		if (idx < 0)
			al_print("N/A  ");
		else
			al_print("%s%02d ", name, idx);
	}
	al_print("\n");

	if (params->max_num_channels != 2)
		al_print("- DRAM interleaving stripe bit: N/A\n");
	else
		al_print("- DRAM interleaving stripe bit: %u\n",
			al_sys_fabric_pasw_dram_intrlv_stripe_bit_get(params->nb_regs_base));

#ifdef AL_DEV_ID
	if (!params->vpw_avail)
		al_print("- DDR VPW: N/A\n");
	else
		al_print("- DDR VPW timeout: %u\n", al_ddr_vpw_cfg_get(&ddr_cfg));
#endif
	return 0;
}

#ifdef AL_HAL_EX
static int al_perf_params_print_iommu(void __iomem *iommu_regs_base)
{
	struct al_iommu_handle_init_params handle_init_params = {
		.iommu_regs_base = iommu_regs_base, };
	struct al_iommu_obj iommu;

	al_iommu_handle_init(&iommu, &handle_init_params);

	al_print("- Status: %s\n", al_iommu_is_enabled(&iommu) ? "enabled" : "disabled");

	return 0;
}
#endif

static int al_perf_params_print_ccu(void __iomem *ccu_regs_base)
{
	struct al_ccu_regs __iomem *ccu_regs = (struct al_ccu_regs __iomem *)ccu_regs_base;

	al_print("- Write buffer: %s\n",
		((al_reg_read32(&ccu_regs->secure_access_register) &
		(CCU_CTRL_OVRD_WR_BUFF_EN | CCU_CTRL_OVRD_WR_BUFF_BYPASS_N)) ==
		(CCU_CTRL_OVRD_WR_BUFF_EN | CCU_CTRL_OVRD_WR_BUFF_BYPASS_N)) ?
		"enabled" : "disabled");
	al_print("- Snoop filter: %s\n",
		(secure_reg_read32(&ccu_regs->control_override_register) &
		CCU_CTRL_OVRD_DISABLE_SNOOP_FILTER) ?
		"disabled" : "enabled");

	return 0;
}

static int al_perf_params_print_adapter(
	void __iomem			*regs_base,
	enum al_unit_adapter_type	type)
{
	struct al_unit_adapter unit_adapter;
	int err;

	err = al_unit_adapter_handle_init(&unit_adapter, type, regs_base, NULL, NULL, NULL, NULL);
	if (err) {
		al_err("%s: al_unit_adapter_handle_init failed!\n", __func__);
		return err;
	}

	al_unit_adapter_perf_params_print(&unit_adapter);

	return 0;
}

static int al_perf_params_print_udma(
	void __iomem	*udma_regs_base)
{
	struct al_udma_params m2s_udma_params = {
		.udma_regs_base = udma_regs_base,
		.type = UDMA_TX,
		.num_of_queues = AL_UDMA_NUM_QUEUES_MAX,
		"N/A",
	};
	struct al_udma m2s_udma;
	struct al_udma_params s2m_udma_params = {
		.udma_regs_base = udma_regs_base,
		.type = UDMA_RX,
		.num_of_queues = AL_UDMA_NUM_QUEUES_MAX,
		"N/A",
	};
	struct al_udma s2m_udma;
	int err;

	err = al_udma_handle_init(&m2s_udma, &m2s_udma_params);
	if (err) {
		al_err("%s: al_udma_handle_init failed!\n", __func__);
		return err;
	}

	err = al_udma_handle_init(&s2m_udma, &s2m_udma_params);
	if (err) {
		al_err("%s: al_udma_handle_init failed!\n", __func__);
		return err;
	}

	al_udma_perf_params_print(&m2s_udma, &s2m_udma);

	return 0;
}

static int al_perf_params_print_pcie(
	void __iomem	*pcie_regs_base,
	void __iomem	*pbs_regs_base,
	unsigned int	port_idx)
{
	struct al_pcie_port pcie_port;
	int err;

	err = al_pcie_port_handle_init(
		&pcie_port, pcie_regs_base, pbs_regs_base, port_idx);
	if (err) {
		al_err("%s: al_pcie_port_handle_init failed!\n", __func__);
		return err;
	}

	al_pcie_port_perf_params_print(&pcie_port);

	return 0;
}

static int al_perf_params_print_generic(
	void __iomem						*pbs_regs_base,
	const struct al_perf_params_print_params_generic	*params)
{
	int err = 0;

	switch (params->type) {
#ifdef AL_HAL_EX
	case AL_PERF_PARAMS_PRINT_PARAMS_GENERIC_TYPE_IOMMU:
		al_print("%s:\n", params->name);
		err = al_perf_params_print_iommu(params->regs_base);
		break;
#endif
	case AL_PERF_PARAMS_PRINT_PARAMS_GENERIC_TYPE_CCU:
		al_print("%s:\n", params->name);
		err = al_perf_params_print_ccu(params->regs_base);
		break;
	case AL_PERF_PARAMS_PRINT_PARAMS_GENERIC_TYPE_ETH_ADAPTER:
		al_print("%s (%u:%u:%u):\n", params->name, params->bus, params->dev, params->func);
		err = al_perf_params_print_adapter(params->regs_base, AL_UNIT_ADAPTER_TYPE_ETH);
		break;
	case AL_PERF_PARAMS_PRINT_PARAMS_GENERIC_TYPE_SSM_ADAPTER:
		al_print("%s (%u:%u:%u):\n", params->name, params->bus, params->dev, params->func);
		err = al_perf_params_print_adapter(params->regs_base, AL_UNIT_ADAPTER_TYPE_SSM);
		break;
	case AL_PERF_PARAMS_PRINT_PARAMS_GENERIC_TYPE_SATA_ADAPTER:
		al_print("%s (%u:%u:%u):\n", params->name, params->bus, params->dev, params->func);
		err = al_perf_params_print_adapter(params->regs_base, AL_UNIT_ADAPTER_TYPE_SATA);
		break;
	case AL_PERF_PARAMS_PRINT_PARAMS_GENERIC_TYPE_UDMA:
		al_print("%s (%u:%u:%u):\n", params->name, params->bus, params->dev, params->func);
		err = al_perf_params_print_udma(params->regs_base);
		break;
	case AL_PERF_PARAMS_PRINT_PARAMS_GENERIC_TYPE_PCIE:
		al_print("%s %u:\n", params->name, params->dev);
		err = al_perf_params_print_pcie(params->regs_base, pbs_regs_base, params->dev);
		break;
	default:
		al_err("%s: type %u not supported!\n", __func__, params->type);
		err = -EINVAL;
		break;
	}

	return err;
}

int al_perf_params_print(
	const struct al_perf_params_print_params *params)
{
	unsigned int i;

	al_print("-----------------------------------------------------------------------------\n");
	al_print("System performance parameters:\n");
	al_print("-----------------------------------------------------------------------------\n");

	al_perf_params_print_cpu();
	if (params->ddr)
		al_perf_params_print_ddr(params->ddr);
	for (i = 0; i < params->generic_num; i++)
		al_perf_params_print_generic(params->pbs_regs_base, &params->generic[i]);

	al_print("-----------------------------------------------------------------------------\n");

	return 0;
}

