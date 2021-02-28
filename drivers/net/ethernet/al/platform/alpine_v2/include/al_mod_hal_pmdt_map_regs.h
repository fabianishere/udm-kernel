#ifndef __AL_HAL_PMDT_MAP_REGS_H__
#define __AL_HAL_PMDT_MAP_REGS_H__

#define AL_PMDT_AXI_MON_MAX_NUM	2
#define AL_PMDT_ELA_MAX_NUM	2

/* dummy values */
#define AL_PMDT_REGION_NUM	0
#define AL_PMDT_PMU_WINDOW_DEFAULT_EXP		0
#define AL_PMDT_PMU_WINDOW_DEFAULT_SAMPLE_TIME	0
#define AL_PMDT_PMU_WINDOW_TRAP_SAMPLE_TIME	0
#define AL_PMDT_EXT_TRIG_NUM 1	/* It should be positive value */

enum al_mod_pmdt_unit {
	AL_PMDT_UNIT_MAX
};

#endif
