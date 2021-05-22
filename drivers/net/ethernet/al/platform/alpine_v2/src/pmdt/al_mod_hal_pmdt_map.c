/**
 * Copyright (C) 2018 Annapurna Labs Ltd.
 * This file may be licensed under the terms of the Annapurna Labs Commercial
 * License Agreement.
 */

#include "al_mod_hal_pmdt_map.h"

int al_mod_pmdt_map_entry_get(
		unsigned int region __attribute__((__unused__)),
		enum al_mod_pmdt_unit id __attribute__((__unused__)),
		struct al_mod_bootstrap *bs __attribute__((__unused__)),
		struct al_mod_pmdt_map_entry *entry __attribute__((__unused__)))
{
	al_mod_err("PMDT unsupported for V2\n");
	return -1;
}

int al_mod_pmdt_map_unit_default_config_get(
		enum al_mod_pmdt_unit id __attribute__((__unused__)),
		struct al_mod_pmdt_config *config __attribute__((__unused__)))
{
	al_mod_err("PMDT unsupported for V2\n");
	return -1;
}

int al_mod_pmdt_map_trig_num_get(
		unsigned int axi_mon_num __attribute__((__unused__)),
		unsigned int axi_mon_instance __attribute__((__unused__)),
		enum al_mod_pmdt_pmu_int_trig_type type __attribute__((__unused__)),
		unsigned int index __attribute__((__unused__)))
{
	al_mod_err("PMDT unsupported for V2\n");
	return -1;
}

int al_mod_pmdt_map_trace_sig_get(
		unsigned int axi_mon_num __attribute__((__unused__)),
		unsigned int axi_mon_instance __attribute__((__unused__)))
{
	al_mod_err("PMDT unsupported for V2\n");
	return -1;
}

int al_mod_pmdt_map_pmu_irq_num_get(
		unsigned int axi_mon_num __attribute__((__unused__)),
		unsigned int axi_mon_instance __attribute__((__unused__)),
		enum al_mod_pmdt_axi_mon_irq_type type __attribute__((__unused__)))
{
	al_mod_err("PMDT unsupported for V2\n");
	return -1;
}

int al_mod_pmdt_map_cpu_clust_num_get(
		struct al_mod_pmdt_complex_handle *cmplx __attribute__((__unused__)))
{
	al_mod_err("PMDT unsupported for V2\n");
	return -1;
}

int al_mod_pmdt_unit_components_info_get(
		enum al_mod_pmdt_unit id __attribute__((__unused__)),
		struct al_mod_pmdt_components_info *info __attribute__((__unused__)))
{
	al_mod_err("PMDT unsupported for V2\n");
	return -1;
}
