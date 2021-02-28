/*******************************************************************************
Copyright (C) 2015 Annapurna Labs Ltd.

This file is licensed under the terms of the Annapurna Labs' Commercial License
Agreement distributed with the file or available on the software download site.
Recipient shall use the content of this file only on semiconductor devices or
systems developed by or for Annapurna Labs.

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
 * @defgroup group_serdes_init SerDes Initialization
 * @ingroup group_serdes SerDes
 * @{
 *
 * @file   al_hal_serdes_avg_init.h
 *
 */

#ifndef __AL_HAL_SERDES_AVG_INIT_H__
#define __AL_HAL_SERDES_AVG_INIT_H__

#include "al_hal_serdes_avg.h"

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

enum al_serdes_avg_mode {
	/*
	 * Lane muxing skipped
	 * Supported by lanes: All
	 */
	AL_SRDS_AVG_MODE_SKIP,

	/*
	 * Lane is part of PCIE 0
	 * Supported by Alpine V3 main complex lanes: 0 - 15
	 */
	AL_SRDS_AVG_MODE_PCIE_0,

	/*
	 * Lane is part of PCIE 1
	 * Supported by Alpine V3 main complex lanes: 0 - 7
	 */
	AL_SRDS_AVG_MODE_PCIE_1,

	/*
	 * Lane is part of PCIE 4
	 * Supported by Alpine V3 main complex lanes: 8 - 11
	 * Supported by Alpine V3 TC main complex lanes: 0 - 1
	 */
	AL_SRDS_AVG_MODE_PCIE_4,

	/*
	 * Lane is part of PCIE 5
	 * Supported by Alpine V3 main complex lanes: 12 - 15
	 */
	AL_SRDS_AVG_MODE_PCIE_5,

	/*
	 * Lane is part of PCIE 2
	 * Supported by Alpine V3 secondary complex lanes: 0 - 7
	 */
	AL_SRDS_AVG_MODE_PCIE_2,

	/*
	 * Lane is part of PCIE 3
	 * Supported by Alpine V3 secondary complex lanes: 0 - 3
	 */
	AL_SRDS_AVG_MODE_PCIE_3,

	/*
	 * Lane is part of PCIE 6
	 * Supported by Alpine V3 secondary complex lanes: 4 - 7
	 */
	AL_SRDS_AVG_MODE_PCIE_6,

	/*
	 * Lane is part of PCIE 7
	 * Supported by Alpine V3 secondary complex lanes: 12 - 15
	 */
	AL_SRDS_AVG_MODE_PCIE_7,

	/*
	 * Lane is part of SATA 0
	 * Supported by Alpine V3 main complex lanes: 0 - 3
	 */
	AL_SRDS_AVG_MODE_SATA_0,

	/*
	 * Lane is part of SATA 1
	 * Supported by Alpine V3 main complex lanes: 4 - 7
	 * Supported by Alpine V3 TC main complex lanes: 0 - 1
	 */
	AL_SRDS_AVG_MODE_SATA_1,

	/*
	 * Lane is part of SATA 2
	 * Supported by Alpine V3 main complex lanes: 8 - 11
	 */
	AL_SRDS_AVG_MODE_SATA_2,

	/*
	 * Lane is part of SATA 3
	 * Supported by Alpine V3 main complex lanes: 12 - 15
	 */
	AL_SRDS_AVG_MODE_SATA_3,

	/*
	 * Lane is part of SATA 4
	 * Supported by Alpine V3 secondary complex lanes: 0 - 3
	 */
	AL_SRDS_AVG_MODE_SATA_4,

	/*
	 * Lane is part of SATA 5
	 * Supported by Alpine V3 secondary complex lanes: 4 - 7
	 */
	AL_SRDS_AVG_MODE_SATA_5,

	/*
	 * Lane is part of SATA 6
	 * Supported by Alpine V3 secondary complex lanes: 8 - 11
	 */
	AL_SRDS_AVG_MODE_SATA_6,

	/*
	 * Lane is part of SATA 7
	 * Supported by Alpine V3 secondary complex lanes: 12 - 15
	 */
	AL_SRDS_AVG_MODE_SATA_7,

	/*
	 * Lane is STD ETH 0 - 1G
	 * Supported by Alpine V3 main or secondary complex lane 14
	 */
	AL_SRDS_AVG_MODE_STD_ETH_0_1G,

	/*
	 * Lane is STD ETH 0 - 10G
	 * Supported by Alpine V3 main or secondary complex lane 14
	 */
	AL_SRDS_AVG_MODE_STD_ETH_0_10G,

	/*
	 * Lane is STD ETH 1 - 1G
	 * Supported by Alpine V3 main or secondary complex lane 15
	 */
	AL_SRDS_AVG_MODE_STD_ETH_1_1G,

	/*
	 * Lane is STD ETH 1 - 10G
	 * Supported by Alpine V3 main or secondary complex lane 15
	 */
	AL_SRDS_AVG_MODE_STD_ETH_1_10G,

	/*
	 * Lane is HS ETH 0 - 10G
	 * Supported by Alpine V3 high-speed complex lane 0
	 * Supported by Alpine V3 TC main complex lane 2
	 */
	AL_SRDS_AVG_MODE_HS_ETH_0_10G,

	/*
	 * Lane is HS ETH 1 - 10G
	 * Supported by Alpine V3 high-speed complex lane 1
	 */
	AL_SRDS_AVG_MODE_HS_ETH_1_10G,

	/*
	 * Lane is HS ETH 2 - 10G
	 * Supported by Alpine V3 high-speed complex lane 2
	 */
	AL_SRDS_AVG_MODE_HS_ETH_2_10G,

	/*
	 * Lane is HS ETH 3 - 10G
	 * Supported by Alpine V3 high-speed complex lane 3
	 */
	AL_SRDS_AVG_MODE_HS_ETH_3_10G,

	/*
	 * Lane is HS ETH 0 - 25G
	 * Supported by Alpine V3 high-speed complex lane 0
	 * Supported by Alpine V3 TC main complex lane 2
	 */
	AL_SRDS_AVG_MODE_HS_ETH_0_25G,

	/*
	 * Lane is HS ETH 1 - 25G
	 * Supported by Alpine V3 high-speed complex lane 1
	 */
	AL_SRDS_AVG_MODE_HS_ETH_1_25G,

	/*
	 * Lane is HS ETH 2 - 25G
	 * Supported by Alpine V3 high-speed complex lane 2
	 */
	AL_SRDS_AVG_MODE_HS_ETH_2_25G,

	/*
	 * Lane is HS ETH 3 - 25G
	 * Supported by Alpine V3 high-speed complex lane 3
	 */
	AL_SRDS_AVG_MODE_HS_ETH_3_25G,

	/*
	 * Lane is part of HS ETH 0 - 50G
	 * Supported by Alpine V3 high-speed complex lanes 0 - 1
	 */
	AL_SRDS_AVG_MODE_HS_ETH_0_50G,

	/*
	 * Lane is part of HS ETH 2 - 50G
	 * Supported by Alpine V3 high-speed complex lanes 2 - 3
	 */
	AL_SRDS_AVG_MODE_HS_ETH_2_50G,

	/*
	 * Lane is part of HS ETH 0 - 100G
	 * Supported by Alpine V3 high-speed complex lanes 0 - 3
	 */
	AL_SRDS_AVG_MODE_HS_ETH_0_100G,
};

enum al_serdes_ref_clk_sel {
	AL_SRDS_REF_CLK_SEL_0,
	AL_SRDS_REF_CLK_SEL_1,
	AL_SRDS_REF_CLK_SEL_2,
	AL_SRDS_REF_CLK_SEL_3,
	AL_SRDS_REF_CLK_SEL_4,
	AL_SRDS_REF_CLK_SEL_5,
};

/* Lane configuration */
struct al_serdes_avg_lane_cfg {
	enum al_serdes_avg_mode		mode;
	/* Reference clock selection - relevant only for PCIe */
	enum al_serdes_ref_clk_sel	ref_clk_sel;
	enum al_serdes_clk_freq		ref_clk_freq;
	/** An indication whether or not the initialization is aimed for running PRBS tests */
	al_bool				init_for_prbs;
#define AL_SERDES_AVG_COMPLEX_CFG_HAS_TX_EQ_PARAMS
	/* Relevant just for Ethernet*/
	struct al_serdes_avg_tx_eq_params	*tx_eq_params;
};

/* TX EQ TAP params */
struct al_serdes_avg_tx_eq_params {
	/* TX EQ precursor */
	unsigned int pre1;
	/* TX EQ attenuation */
	unsigned int atten;
	/* TX EQ postcursor */
	unsigned int post1;
};

/* Complex configuration */
struct al_serdes_avg_complex_cfg {
	struct al_serdes_complex_obj	*complex_obj;
	struct al_serdes_avg_lane_cfg	*lane_cfg;
	unsigned int			num_lanes;
};

/** Serdes initialization params */
struct al_serdes_avg_init_params {
	/** A list of complex configuration pointers - the first one must be the master complex */
	struct al_serdes_avg_complex_cfg	*complex_cfgs;
	/** The number of complexes in the list */
	unsigned int				num_complexes;
	/** An indication whether or not to download the PCIe FW */
	al_bool					fw_pcie_download;
	/** PCIe FW buffer, or NULL for using the default FW */
	const uint32_t				*fw_pcie;
	/** PCIe FW size (number of 32 bit words), if 'fw_pcie' was not NULL */
	unsigned int				fw_pcie_size;
	/** PCIe FW read callback function, or NULL for using direct buffer or default FW */
	void					(*fw_pcie_rd_cb)(
							uint32_t	*buff,
							unsigned int	size);
	/** PCIe FW temp buffer, if 'fw_pcie_rd_cb' was not NULL */
	uint32_t				*fw_pcie_temp_buff;
	/** PCIe FW temp buffer size (number of 32 bit words), if 'fw_pcie_rd_cb' was not NULL */
	unsigned int				fw_pcie_temp_buff_size;
	/** An indication whether or not to download the non PCIe FW */
	al_bool					fw_non_pcie_download;
	/** Non PCIe FW buffer, or NULL for using the default FW */
	const uint32_t				*fw_non_pcie;
	/** Non PCIe FW size (number of 32 bit words), if 'fw_non_pcie' was not NULL */
	unsigned int				fw_non_pcie_size;
	/** An indication whether or not to perform SBUS master ROM download */
	al_bool					sbus_master_rom_download;
};

/** Lane configuration retrieve */
struct al_serdes_avg_lane_cfg_retrieve {
	enum al_serdes_avg_mode		mode;
	/* Reference clock selection - relevant only for PCIe */
	enum al_serdes_ref_clk_sel	ref_clk_sel;
	enum al_serdes_bit_rate		bit_rate;
	enum al_serdes_bit_width	bit_width;
};

/**
 * AVG SerDes init (FW download, muxing, clock selection, and mode configuration)
 *
 * @param	init_params
 *		Initialization parameters
 *
 * @returns	0 upon success
 */
int al_serdes_avg_init(
	struct al_serdes_avg_init_params *init_params);

/**
 * AVG SerDes retrieve previously configured parameters for a single lane
 *
 * Gets configuration from internal registers, so one software application can initialize
 * the lane, while another software application can retrieve the configured lane mode.
 *
 * @param	obj
 *		SerDes lane object
 * @param	retrieve
 *		AVG SerDes lane configuration retrieve
 *
 * @returns	none
 */
void al_serdes_avg_lane_cfg_retrieve(
	struct al_serdes_grp_obj		*obj,
	struct al_serdes_avg_lane_cfg_retrieve	*retrieve);

/* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif

/* *INDENT-ON* */
#endif

/** @} end of SERDES group */

