/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */
#ifndef __AL_PERF_PARAMS_H__
#define __AL_PERF_PARAMS_H__

#include "al_hal_common.h"

/** DDR performance params print parameter structure */
struct al_perf_params_print_params_ddr {
	/** System fabric service register base */
	void __iomem	*nb_regs_base;
	/** Channel 0 DDR core register base */
	void __iomem	*ch0_ddr_core_regs_base;
	/** Channel 0 DDR PHY register base */
	void __iomem	*ch0_ddr_phy_regs_base;
	/** Channel 0 DDR wrapper register base */
	void __iomem	*ch0_ddrc_regs_base;
	/** Max number of channels */
	unsigned int	max_num_channels;
	/** VPW mechanism availability */
	al_bool		vpw_avail;
};

/** Types of generic unit performance params print parameter structures */
enum al_perf_params_print_params_generic_type {
	/** I/O MMU */
	AL_PERF_PARAMS_PRINT_PARAMS_GENERIC_TYPE_IOMMU = 0,
	/** CCU */
	AL_PERF_PARAMS_PRINT_PARAMS_GENERIC_TYPE_CCU,
	/** Ethernet adapter */
	AL_PERF_PARAMS_PRINT_PARAMS_GENERIC_TYPE_ETH_ADAPTER,
	/** SSM adapter */
	AL_PERF_PARAMS_PRINT_PARAMS_GENERIC_TYPE_SSM_ADAPTER,
	/** SATA adapter */
	AL_PERF_PARAMS_PRINT_PARAMS_GENERIC_TYPE_SATA_ADAPTER,
	/** UDMA */
	AL_PERF_PARAMS_PRINT_PARAMS_GENERIC_TYPE_UDMA,
	/** PCIe */
	AL_PERF_PARAMS_PRINT_PARAMS_GENERIC_TYPE_PCIE,
};

/** Generic unit performance params print parameter structure */
struct al_perf_params_print_params_generic {
	/** Type */
	enum al_perf_params_print_params_generic_type	type;
	/** Bus number */
	unsigned int					bus;
	/** Device number */
	unsigned int					dev;
	/** Function number */
	unsigned int					func;
	/** Name */
	const char					*name;
	/** Register base */
	void __iomem					*regs_base;
};

/** Performance params print parameter structure */
struct al_perf_params_print_params {
	/** PBS register base */
	void __iomem						*pbs_regs_base;
	/** DDR performance params print parameter structure */
	const struct al_perf_params_print_params_ddr		*ddr;
	/** Number of generic unit performance params print parameter structures */
	unsigned int						generic_num;
	/** Generic unit performance params print parameter structure array */
	const struct al_perf_params_print_params_generic	*generic;
};

/**
 * Performance parameters prinout
 *
 * @param	params
 *		Printout parameters
 *
 * @returns	0 upon success, <0 upon failure
 */
int al_perf_params_print(
	const struct al_perf_params_print_params *params);

#endif

