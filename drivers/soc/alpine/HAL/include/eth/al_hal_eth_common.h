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
 * @defgroup group_eth_common_api API
 * Ethernet Controllers Common HAL driver API
 * @ingroup group_eth
 * @{
 * @file   al_hal_eth_common.h
 *
 * @brief Header file for Ethernet Controllers Common HAL driver API
 *
 */

#ifndef __AL_HAL_ETH_COMMON_H__
#define __AL_HAL_ETH_COMMON_H__

#include "al_hal_types.h"
#include "al_hal_eth_mac.h"

/**
 * Ethernet Block Topology
 *
 *                +--------+   +--------+   +--------+   +--------+
 *                | Unit 0 |   | Unit 1 |   | Unit 2 |   | Unit 3 |
 *                +--------+   +--------+   +--------+   +--------+
 *
 *                +-----------------------------------------------+
 *                |                APB + AXI Mux                  |
 *                +-----------------------------------------------+
 *
 *                +--------+   +--------+   +--------+   +--------+
 *                | UDMA 0 |   | UDMA 1 |   | UDMA 2 |   | UDMA 3 |
 *                +--------+   +--------+   +--------+   +--------+
 *
 *                +-----------------------------------------------+
 *                |               AXI Stream Mux                  |
 *                +-----------------------------------------------+
 *
 *                +--------+   +--------+   +--------+   +--------+
 *                |  EC 0  |   |  EC 1  |   |  EC 2  |   |  EC 3  |
 *                +--------+   +--------+   +--------+   +--------+
 *
 *   +--------+   +--------+   +--------+   +--------+   +--------+
 *   |  MAC   |   |  MAC   |   |  MAC   |   |  MAC   |   |  MAC   |
 *   |        |   | 10/25/ |   | 10/25/ |   | 10/25/ |   | 10/25/ |
 *   |  100G  |   | 40/50G |   | 40/50G |   | 40/50G |   | 40/50G |
 *   +--------+   +--------+   +--------+   +--------+   +--------+
 *
 */

#define AL_ETH_COMMON_ADAPTER_NUM	4

/**
 * Ethernet modes of operation
 *
 * +------------------+----------+-------------------+--------------+
 * | Mode \ Features  | Adapters | UDMAs per adapter | ECs per UDMA |
 * +------------------+----------+-------------------+--------------+
 * | 4 x 25G          |     4    |         1         |       1      |
 * +------------------+----------+-------------------+--------------+
 * | 2 x 50G          |     2    |         2         |       1      |
 * +------------------+----------+-------------------+--------------+
 * | 1 x 100G         |     1    |         4         |       1      |
 * +------------------+----------+-------------------+--------------+
 * | 1 x 100G Agg.    |     1    |         4         |  TX 1, RX 4  |
 * +------------------+----------+-------------------+--------------+
 * | 1 x 40G          |     1    |         4         |       1      |
 * +------------------+----------+-------------------+--------------+
 * | 1 x 50G, 2 x 25G |     3    |        2, 1       |       1      |
 * +------------------+----------+-------------------+--------------+
 *
 * Features per EC according to mode:
 * +------------------+------------------+---------------------------+---------------------------+
 * |                  |                  |         RX FIFO           |      Internal RX FIFO     |
 * +------------------+------------------+---------------------------+---------------------------+
 * | Mode \ Features  |       MACs       |    Data   |    Info       |    Data    |     Info     |
 * +------------------+------------------+-----------+---------------+------------+--------------+
 * | 4 x 25G          |      1 x 25G     |    4 KB   |     2.5 KB    |            |              |
 * +------------------+------------------+-----------+---------------+------------+--------------+
 * | 2 x 50G          |      1 x 50G     |   10 KB   |       5 KB    |            |              |
 * +------------------+------------------+-----------+---------------+------------+--------------+
 * | 1 x 100G         |     1 x 100G     |   20 KB   |      10 KB    |            |              |
 * +------------------+------------------+-----------+---------------+------------+--------------+
 * | 1 x 100G Agg.    |      4 x 25G     | 4 KB/MAC  |  2.5 KB/MAC   |            |              |
 * +------------------+------------------+-----------+---------------+------------+--------------+
 * | 1 x 40G          |     1 x 100G     |   20 KB   |      10 KB    |            |              |
 * +------------------+------------------+-----------+---------------+------------+--------------+
 * | 1 x 50G, 2 x 25G | 1 x 50G, 1 x 25G | 10KB, 4KB |  5KB, 2.5KB   |            |              |
 * +------------------+------------------+-----------+---------------+------------+--------------+
 *
 */
#define AL_ETH_HAS_COMMON_MODE
enum al_eth_common_mode {
	/** Invalid */
	AL_ETH_COMMON_MODE_INVALID,
	/** 4 x 25G */
	AL_ETH_COMMON_MODE_4X25G,
	/** 2 x 50G */
	AL_ETH_COMMON_MODE_2X50G,
	/** 1 x 100G */
	AL_ETH_COMMON_MODE_1X100G,
	/** 1 x 100G Aggregated */
	AL_ETH_COMMON_MODE_1X100G_AGG,
	/** 1 x 40G */
	AL_ETH_COMMON_MODE_1X40G,
	/** 1 x 50G, 2 x 25G */
	AL_ETH_COMMON_MODE_1X50G_2X25G,
	/** 2 x 25G, 2 x 10G */
	AL_ETH_COMMON_MODE_2X25G_2X10G,
	/** 4 x 10G */
	AL_ETH_COMMON_MODE_4X10G,
	/** Max number of eth common modes */
	AL_ETH_COMMON_MODE_NUM,
};

/** Indexing of AN LT lanes */
enum al_eth_an_lt_lane {
	AL_ETH_AN__LT_LANE_0,
	AL_ETH_AN__LT_LANE_1,
	AL_ETH_AN__LT_LANE_2,
	AL_ETH_AN__LT_LANE_3,
	AL_ETH_AN__LT_LANE_NUM,
};

/**
 * Ethernet performance modes
 *
 * Applicable since ETHv4
 */
enum al_eth_perf_mode {
	/**
	 * Hardware reset value
	 *
	 * As configured by HW after reset.
	 *
	 * FIFO read threshold:         0x120 beats
	 * FIFO saturation:             0x130 beats
	 * Store and forward:           Disabled
	 * Credits increment interval:  0x4 clocks
	 * Credits increment value:     0x1 beats
	 * Negative credits:            Enabled
	 */
	AL_ETH_PERF_MODE_HW_RESET_VALUE,
	/**
	 * Mode A
	 *
	 * Optimized on October 2018 for working with all 4 ports.
	 *
	 * FIFO read threshold:         0x120 beats
	 * FIFO saturation:             0x168 beats
	 * Store and forward:           Enabled
	 * Credits increment interval:  0x80 clocks
	 * Credits increment value:     0x19 beats
	 * Negative credits:            Enabled
	 */
	AL_ETH_PERF_MODE_A,
	/**
	 * Default
	 *
	 * Performance mode as configured during Ethernet Common initialization
	 * and each Ethernet adapter initialization.
	 *
	 * Notice that Ethernet Common initialization usually happens outside of application context
	 * (unlike Ethernet adapter initialization), so it might use an older/newer HAL,
	 * while the definition of default mode might change over time.
	 * So if explicit tuning mode is needed - the Ethernet Common performance mode should be
	 * configured explicitly by application (given the application is the owner
	 * of all Ethernet adapters).
	 */
	AL_ETH_PERF_MODE_DEFAULT = AL_ETH_PERF_MODE_A,
};

/**
 * Get a common mode string name
 *
 * @param mode
 *		Common mode in use
 * @returns common mode name
 */
const char *al_eth_common_mode_to_str(enum al_eth_common_mode mode);

/**
 * Get a common mode number of adapters
 *
 * @param mode Common mode
 * @returns number of adapters
 */
unsigned int al_eth_common_mode_num_adapters_get(enum al_eth_common_mode mode);

/** Ethernet common handle init params */
struct al_eth_common_handle_init_params {
	/**
	 * Ethernet Common registers base address
	 */
	void __iomem *eth_common_regs_base;

	/**
	 * Ethernet Controller 0 registers base address - Should only be used for accessing
	 * RX resources since only the RX pipe is shared among the ports.
	 */
	void __iomem *eth_ec_0_regs_base;

};

/** Ethernet common handle */
struct al_eth_common_handle {
	/**
	 * Ethernet Common registers base address
	 */
	void __iomem *eth_common_regs_base;

	/**
	 * Ethernet Controller 0 registers base address - Should only be used for accessing
	 * RX resources since only the RX pipe is shared among the ports.
	 */
	void __iomem *eth_ec_0_regs_base;

};

struct al_eth_common_init_params {
	/**
	 * Ethernet Common registers base address
	 */
	void __iomem *eth_common_regs_base;

	/**
	 * Ethernet Controler 0 registers base address
	 */
	void __iomem *eth_ec_0_regs_base;

	/**
	 * Ethernet Common base mode
	 */
	enum al_eth_common_mode mode;

	/**
	 * Even if mode should turn off some of the adapters from discovery,
	 * they still will be used by the UDMA matching their index for
	 * transferring data.
	 */
	al_bool use_all_adapters;

	/**
	 * Skip Adapters, UDMAs & ECs initialization
	 */
	al_bool skip_adapters_udmas_ecs_init;

	/**
	 * Skip MACs initialization
	 */
	al_bool skip_macs_init;

	/**
	 * When turned on- configure base and depth for rxf
	 * data_fifo_2 && info_fifo_2 in case of 2x50, 1x100, 1x40
	 */
	al_bool internal_switching;
};

/** Ethernet Common General Storage params */
struct al_eth_common_gen_storage_params {
	/* This is bit field used by higher level drivers (e.g. AL_ETH in linux), to save
	 * which ETH ports [represented by function numbers (F of BDF as seen by RC)]
	 * have been probed (or removed) by the driver.
	 */
	uint32_t eth_func_probe_state;
};

/** Adapter to UDMA FLR muxing regs struct */
struct al_eth_common_adp2func_flr_regs {
	uint32_t flr;
	uint32_t flr_bar;
};

/** Each adapter exports 4 flr bit lines (one for each func it supports).
 *  Basically, each line is triggered by the PCI_EXP_DEVCTL_BCR_FLR bit in
 *  the PCI_EXP_DEVCTL reg of the corresponding PCI config space:
 *    flr_bit[0] - PF
 *    flr_bit[1] - VF0
 *    flr_bit[2] - VF1
 *    flr_bit[3] - VF2
 */
struct al_eth_common_adp2func_flr_udma_attr {
	/* The Physical UDMA num we want to configure */
	unsigned int udma_num;
	/* Chooses from which adapter to take the 4bit group of flr bits. */
	unsigned int adapter_num;
	/* Chooses the single flr bit out of the 4bit group (from the adapter
	 * which was connected to it according to the 'adapter_num' param).
	 */
	unsigned int flr_bit;
};

/** The following params struct enables configuring the operation of the different HW modules
 *  in the RX EC pipe. For each module, there exists the following:
 *     - 'valid' field - allows to enable/disable this HW module
 *     - 'enable/disable' field - the actual command (only relevant if 'valid' is true)
 */
struct al_eth_common_rx_pipe_modules_ctrl_params {
	/* Rx multi-stream write controller operation */
	al_bool ec_gen_msw_in_valid;
	al_bool ec_gen_msw_in;
};

/** UDMA conversion table */

/** This table maps UDMA number to adapter port and must be restored on FLR */
struct al_eth_common_udma_select_params {
	uint32_t udma_select;
	uint32_t rfw_v4_udma_select[64];
};

/**
 * Get udma conversion table from ec register file
 *
 * @param regs_base
 *		Ethernet Controler registers base address
 * @param params
 *		Struct to save params to
 * @returns void
 */
void al_eth_common_udma_select_config_get(
	void __iomem *regs_base,
	struct al_eth_common_udma_select_params *params);

/**
 * Set udma conversion table from saved parameters
 *
 * @param regs_base
 *		Ethernet Controler registers base address
 * @param params
 *		Struct to restore params from
 * @returns void
 */
void al_eth_common_udma_select_config_set(
	void __iomem *regs_base,
	struct al_eth_common_udma_select_params *params);

/**
 * Initializes Ethernet Common handle
 *
 * @param handle
 *		Pointer to handle
 * @param handle_init_params
 *		Pointer to handle initialization parameters
 * @returns void
 */
void al_eth_common_handle_init(struct al_eth_common_handle *handle,
	struct al_eth_common_handle_init_params *handle_init_params);

/**
 * Write Ethernet Common General Storage params to scratch pad registers
 *
 * @param handle
 *		Ethernet common handle
 * @param params
 *		pointer to general storage params with write data
 * @returns void
 */
void al_eth_common_gen_storage_params_set(const struct al_eth_common_handle *handle,
					  const struct al_eth_common_gen_storage_params *params);

/**
 * Read Ethernet Common General Storage params from scratch pad registers
 *
 * @param handle
 *		Ethernet common handle
 * @param params
 *		pointer to general storage params to fill
 * @returns void
 */
void al_eth_common_gen_storage_params_get(const struct al_eth_common_handle *handle,
					  struct al_eth_common_gen_storage_params *params);

/**
 * Initializes Ethernet Common block
 * Configures Unit Adapters of Ethernet controllers
 *
 * @param init_params
 *		Pointer to init params
 *
 * @return 0 on success. negative errno on failure.
 */
int al_eth_common_init(struct al_eth_common_init_params *init_params);

/**
 * Initializes Ethernet FEC, only relevent for MAC v4 this configures
 * the common registers on MAC v4 and should be done prior to configuring
 * the mac registers for FEC
 *
 * @param mac_obj
 *		Pointer to mac private object
 * @param lane
 *		MAC serdes lane index
 * @param fec_enable
 *		Enable/disable FEC
 * @param fec_type
 *		Currently only FEC91 / FEC74 support
 *
 * @return 0 on success. negative errno on failure.
 */
int al_eth_mac_v4_fec_ctrl(struct al_eth_mac_obj *mac_obj,
	enum al_eth_mac_common_mac_v4_lane lane, enum al_eth_fec_type fec_type, al_bool fec_enable);

/**
 * Ethernet Common performance mode configuration
 *
 * @param handle
 *		Ethernet Common handle
 * @param mode
 *		Performance mode
 *
 * @return none
 */
void al_eth_common_perf_mode_config(struct al_eth_common_handle *handle,
	enum al_eth_perf_mode mode);

/**
 * Read Ethernet Common adp2func flr and flr_bar regs
 *
 * @param handle
 *		Ethernet Common handle
 * @param flr_regs
 *		Pointer to var in which to store the value of the flr regs
 *
 * @return none
 */
void al_eth_common_adp2func_flr_read_raw(struct al_eth_common_handle *handle,
					 struct al_eth_common_adp2func_flr_regs *flr_regs);

/**
 * Write to Ethernet Common adp2func flr and flr_bar regs
 *
 * @param handle
 *		Ethernet Common handle
 * @param flr_regs
 *		Pointer to struct which holds the values to write to the flr regs
 *
 * @return none
 */
void al_eth_common_adp2func_flr_write_raw(struct al_eth_common_handle *handle,
					  struct al_eth_common_adp2func_flr_regs *flr_regs);

/**
 * Per UDMA flr and flr_bar configuration - This basically configures for the
 * specific UDMA, which adapter (and in the that adapter which flr line) will
 * be connected to it's sw reset line
 *
 * @param handle
 *		Ethernet Common handle
 * @param udma_attr
 *		Pointer to udma_attr params
 *
 * @return none
 *
 * @note This function shouldn't be called concurrently from 2 (or more)
 *       Port/UDMA contexts
 */
void al_eth_common_adp2func_flr_udma_config(struct al_eth_common_handle *handle,
					    struct al_eth_common_adp2func_flr_udma_attr *udma_attr);

/**
 * Enable/Disable HW modules in the shared EC RX pipe. For example:
 *  - Rx multi-stream write controller operation
 *  - Rx FIFO input controller 1 operation
 *  - Rx FIFO output controller operation
 *
 * @param handle
 *		Ethernet Common handle
 * @param params
 *		Pointer to rx_pipe_modules_en_params
 *
 * @return none
 *
 * @note This function shouldn't be called concurrently from 2 (or more)
 *       Port contexts
 */
void al_eth_common_rx_pipe_modules_ctrl(struct al_eth_common_handle *handle,
					struct al_eth_common_rx_pipe_modules_ctrl_params *params);

#endif /* __AL_HAL_ETH_COMMON_H__ */

/** @} */

