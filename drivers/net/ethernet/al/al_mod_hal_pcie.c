/*******************************************************************************
Copyright (C) 2015 Annapurna Labs Ltd.

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

#include "al_mod_hal_pcie.h"
#include "al_mod_hal_pbs_utils.h"
#include "al_mod_hal_unit_adapter_regs.h"

#if (!defined(AL_DEV_ID)) || (AL_DEV_ID == AL_DEV_ID_ALPINE_V1) || (defined(AL_HAL_PCIE_REV_ID_ALL))
#define REV1_SUPPORTED	1
#else
#define REV1_SUPPORTED	0
#endif
#if (!defined(AL_DEV_ID)) || (AL_DEV_ID == AL_DEV_ID_ALPINE_V2) || (defined(AL_HAL_PCIE_REV_ID_ALL))
#define REV2_SUPPORTED	1
#define REV3_SUPPORTED	1
#else
#define REV2_SUPPORTED	0
#define REV3_SUPPORTED	0
#endif
#if (!defined(AL_DEV_ID)) || (AL_DEV_ID == AL_DEV_ID_ALPINE_V3) || (defined(AL_HAL_PCIE_REV_ID_ALL))
#define REV4_SUPPORTED	1
#else
#define REV4_SUPPORTED	0
#endif
#if (!defined(AL_DEV_ID)) || (AL_DEV_ID == AL_DEV_ID_ALPINE_V4) || (defined(AL_HAL_PCIE_REV_ID_ALL))
#define REV5_SUPPORTED	1
#else
#define REV5_SUPPORTED	0
#endif

#if (REV1_SUPPORTED)
#include "al_mod_hal_pcie_reg_ptr_set_rev1.h"
#endif
#if (REV2_SUPPORTED)
#include "al_mod_hal_pcie_reg_ptr_set_rev2.h"
#endif
#if (REV3_SUPPORTED)
#include "al_mod_hal_pcie_reg_ptr_set_rev3.h"
#endif
#if (REV4_SUPPORTED)
#include "al_mod_hal_pcie_reg_ptr_set_rev4.h"
#endif
#if (REV5_SUPPORTED)
#include "al_mod_hal_pcie_reg_ptr_set_rev5.h"
#endif

#include "al_mod_hal_pcie_regs_fields.h"

/**
 * Parameter definitions
 */
#define AL_PCIE_AXI_REGS_OFFSET			0x0

#define AL_PCIE_DEVCTL_PAYLOAD_128B		0x00
#define AL_PCIE_DEVCTL_PAYLOAD_256B		0x20

#define AL_PCIE_SECBUS_DEFAULT			0x1
#define AL_PCIE_SUBBUS_DEFAULT			0x1
#define AL_PCIE_LINKUP_WAIT_INTERVAL		50	/* measured in usec */
#define AL_PCIE_LINKUP_WAIT_INTERVALS_PER_SEC	20

#define AL_PCIE_LINKUP_RETRIES			8

#define AL_PCIE_MAX_32_MEMORY_BAR_SIZE		(0x100000000ULL)
#define AL_PCIE_MIN_MEMORY_BAR_SIZE		(1 << 12)
#define AL_PCIE_MIN_IO_BAR_SIZE			(1 << 8)

#define AL_PCIE_CLK_GEN2_PIPE_HZ		(250 * 1000000) /* 250MHZ */
#define AL_PCIE_CLK_GEN1_PIPE_HZ		(125 * 1000000) /* 125MHZ */
#define AL_PCIE_CLK_DELAY_AFTER_RESET_EN_HZ	(0x50)		/* 0x50HZ */
#define AL_PCIE_CLK_DELAY_BEFORE_RATE_CHANGE_HZ	(0x20)		/* 0x20HZ */
#define AL_PCIE_CLK_DELAY_AFTER_RATE_CHANGE_HZ	(0x20)		/* 0x20HZ */
#define AL_PCIE_CLK_REF_MAX			(3)

#define AL_PCIE_PORT_GEN3_EQ_FMDC_MAX_POST_CUSROR_DELTA_MAX	15
#define AL_PCIE_PORT_GEN3_EQ_FMDC_MAX_PRE_CUSROR_DELTA_MAX	15
#define AL_PCIE_PORT_GEN3_EQ_FMDC_N_EVALS_MAX			16
#define AL_PCIE_PORT_GEN3_EQ_FMDC_T_MIN_PHAS_MAX		24

#define AL_PCIE_INT_FWD_UNIT_ETH_IDX_MAX 6

#define AL_PCIE_ATU_ADDRESS_MATCH_EXT_MODE_THRESHOLD	(1ULL << 32)

/**
 * MACROS
 */
#define AL_PCIE_PARSE_LANES(v)		(((1 << v) - 1) << \
		PCIE_REVX_AXI_MISC_PCIE_GLOBAL_CONF_NOF_ACT_LANES_SHIFT)

#define AL_PCIE_FLR_DONE_INTERVAL		10

/**
 * Inbound posted/non-posted header credits and outstanding outbound reads
 * completion header HW settings
 *
 * Programmers are not expected to modify these setting except for rare cases,
 */
struct al_mod_pcie_ib_hcrd_os_ob_reads_hw_config {

	/**
	 * This value holds the hardware limit for
	 * number of outstanding outbound reads
	 */
	uint8_t max_nof_outstanding_ob_reads;

	/**
	 * This value holds the hardware limit for the sum of
	 * nof_cpl_hdr, nof_np_hdr and nof_p_hdr
	 */
	unsigned int total_hdr_limit;

	/**
	 * This value state if it is advised to use hardware
	 * default of values of header credits
	 */
	al_mod_bool ib_hcrd_use_hw_default;

	/**
	 * This value state if current hardware support
	 * dynamic header credits allocation
	 */
	al_mod_bool ib_hcrd_supported;
};

static const struct al_mod_pcie_ib_hcrd_os_ob_reads_config
ib_hcrd_os_ob_reads_defaults[AL_PCIE_REV_ID_MAX + 1][PCIE_AXI_DEVICE_ID_REG_REV_ID_MAX + 1][2]
	 = {
	/* REV1 EP */
	[AL_PCIE_REV_ID_1][PCIE_AXI_DEVICE_ID_REG_REV_ID_NA][AL_PCIE_OPERATING_MODE_EP] = {
		.nof_outstanding_ob_reads = 15,
		.nof_cpl_hdr = 75,
		.nof_np_hdr = 6,
		.nof_p_hdr = 15,
	},
	/* REV1 RC */
	[AL_PCIE_REV_ID_1][PCIE_AXI_DEVICE_ID_REG_REV_ID_NA][AL_PCIE_OPERATING_MODE_RC] = {
		.nof_outstanding_ob_reads = 8,
		.nof_cpl_hdr = 41,
		.nof_np_hdr = 25,
		.nof_p_hdr = 31,
	},
	/* REV2 EP */
	[AL_PCIE_REV_ID_2][PCIE_AXI_DEVICE_ID_REG_REV_ID_NA][AL_PCIE_OPERATING_MODE_EP] = {
		.nof_outstanding_ob_reads = 15,
		.nof_cpl_hdr = 75,
		.nof_np_hdr = 6,
		.nof_p_hdr = 15,
	},
	/* REV2 RC */
	[AL_PCIE_REV_ID_2][PCIE_AXI_DEVICE_ID_REG_REV_ID_NA][AL_PCIE_OPERATING_MODE_RC] = {
		.nof_outstanding_ob_reads = 8,
		.nof_cpl_hdr = 41,
		.nof_np_hdr = 25,
		.nof_p_hdr = 31,
	},
	/* REV3 EP */
	[AL_PCIE_REV_ID_3][PCIE_AXI_DEVICE_ID_REG_REV_ID_NA][AL_PCIE_OPERATING_MODE_EP] = {
		.nof_outstanding_ob_reads = 32,
		.nof_cpl_hdr = 161,
		.nof_np_hdr = 38,
		.nof_p_hdr = 60,
	},
	/* REV3 RC */
	[AL_PCIE_REV_ID_3][PCIE_AXI_DEVICE_ID_REG_REV_ID_NA][AL_PCIE_OPERATING_MODE_RC] = {
		.nof_outstanding_ob_reads = 32,
		.nof_cpl_hdr = 161,
		.nof_np_hdr = 38,
		.nof_p_hdr = 60,
	},
	/* REV4 EP X4*/
	[AL_PCIE_REV_ID_4][PCIE_AXI_DEVICE_ID_REG_REV_ID_X4][AL_PCIE_OPERATING_MODE_EP] = {
		.nof_outstanding_ob_reads = 16,
		.nof_cpl_hdr = 81,
		.nof_np_hdr = 33,
		.nof_p_hdr = 33,
	},
	/* REV4 RC X4*/
	[AL_PCIE_REV_ID_4][PCIE_AXI_DEVICE_ID_REG_REV_ID_X4][AL_PCIE_OPERATING_MODE_RC] = {
		.nof_outstanding_ob_reads = 16,
		.nof_cpl_hdr = 81,
		.nof_np_hdr = 33,
		.nof_p_hdr = 33,
	},
	/* REV4 EP X8*/
	[AL_PCIE_REV_ID_4][PCIE_AXI_DEVICE_ID_REG_REV_ID_X8][AL_PCIE_OPERATING_MODE_EP] = {
		.nof_outstanding_ob_reads = 64,
		.nof_cpl_hdr = 321,
		.nof_np_hdr = 33,
		.nof_p_hdr = 65,
	},
	/* REV4 RC X8*/
	[AL_PCIE_REV_ID_4][PCIE_AXI_DEVICE_ID_REG_REV_ID_X8][AL_PCIE_OPERATING_MODE_RC] = {
		.nof_outstanding_ob_reads = 64,
		.nof_cpl_hdr = 321,
		.nof_np_hdr = 33,
		.nof_p_hdr = 65,
	},
	/* REV4 EP X16*/
	[AL_PCIE_REV_ID_4][PCIE_AXI_DEVICE_ID_REG_REV_ID_X16][AL_PCIE_OPERATING_MODE_EP] = {
		.nof_outstanding_ob_reads = 80,
		.nof_cpl_hdr = 200,
		.nof_np_hdr = 33,
		.nof_p_hdr = 33,
	},
	/* REV4 RC X16*/
	[AL_PCIE_REV_ID_4][PCIE_AXI_DEVICE_ID_REG_REV_ID_X16][AL_PCIE_OPERATING_MODE_RC] = {
		.nof_outstanding_ob_reads = 80,
		.nof_cpl_hdr = 200,
		.nof_np_hdr = 33,
		.nof_p_hdr = 33,
	},
	/* REV5 RC X8*/
	[AL_PCIE_REV_ID_5][PCIE_AXI_DEVICE_ID_REG_REV_ID_REV5_X8][AL_PCIE_OPERATING_MODE_RC] = {
		.nof_outstanding_ob_reads = 64,
		.nof_cpl_hdr = 200,
		.nof_np_hdr = 33,
		.nof_p_hdr = 33,
	},
};

/**
 * inbound header credits and outstanding outbound reads HW config
 */
static const struct al_mod_pcie_ib_hcrd_os_ob_reads_hw_config
	ib_hcrd_os_ob_reads_hw_config[AL_PCIE_REV_ID_MAX + 1][PCIE_AXI_DEVICE_ID_REG_REV_ID_MAX + 1]
	 = {
	/* REV1 */
	[AL_PCIE_REV_ID_1][PCIE_AXI_DEVICE_ID_REG_REV_ID_NA] = {
		.max_nof_outstanding_ob_reads = 16,
		.total_hdr_limit = AL_PCIE_REV_1_2_IB_HCRD_SUM,
		.ib_hcrd_use_hw_default = 0,
		.ib_hcrd_supported = 1,
	},
	/* REV2 */
	[AL_PCIE_REV_ID_2][PCIE_AXI_DEVICE_ID_REG_REV_ID_NA] = {
		.max_nof_outstanding_ob_reads = 16,
		.total_hdr_limit = AL_PCIE_REV_1_2_IB_HCRD_SUM,
		.ib_hcrd_use_hw_default = 0,
		.ib_hcrd_supported = 1,
	},
	/* REV3 */
	[AL_PCIE_REV_ID_3][PCIE_AXI_DEVICE_ID_REG_REV_ID_NA] = {
		.max_nof_outstanding_ob_reads = 32,
		.total_hdr_limit = AL_PCIE_REV3_IB_HCRD_SUM,
		.ib_hcrd_use_hw_default = 0,
		.ib_hcrd_supported = 1,
	},
	/* REV4 X4*/
	[AL_PCIE_REV_ID_4][PCIE_AXI_DEVICE_ID_REG_REV_ID_X4] = {
		.max_nof_outstanding_ob_reads = 32,
		.total_hdr_limit = AL_PCIE_REV4_X4_IB_HCRD_SUM,
		.ib_hcrd_use_hw_default = 1,
		.ib_hcrd_supported = 1,
	},
	/* REV4 X8*/
	[AL_PCIE_REV_ID_4][PCIE_AXI_DEVICE_ID_REG_REV_ID_X8] = {
		.max_nof_outstanding_ob_reads = 64,
		.total_hdr_limit = AL_PCIE_REV4_X8_IB_HCRD_SUM,
		.ib_hcrd_use_hw_default = 1,
		.ib_hcrd_supported = 1,
	},
	/* REV4 X16*/
	[AL_PCIE_REV_ID_4][PCIE_AXI_DEVICE_ID_REG_REV_ID_X16] = {
		.max_nof_outstanding_ob_reads = 128,
		.total_hdr_limit = AL_PCIE_REV4_X16_IB_HCRD_SUM,
		.ib_hcrd_use_hw_default = 1,
		.ib_hcrd_supported = 0,
	},
	/* REV5 X8*/
	[AL_PCIE_REV_ID_5][PCIE_AXI_DEVICE_ID_REG_REV_ID_REV5_X8] = {
		.max_nof_outstanding_ob_reads = 128,
		.total_hdr_limit = AL_PCIE_REV5_X8_IB_HCRD_SUM,
		.ib_hcrd_use_hw_default = 1,
		.ib_hcrd_supported = 1,
	},
};

static unsigned int lanes_num[AL_PCIE_REV_ID_MAX + 1] = {
	[AL_PCIE_REV_ID_1] = 4,
	[AL_PCIE_REV_ID_2] = 4,
	[AL_PCIE_REV_ID_3] = 8,
	[AL_PCIE_REV_ID_4] = 16,
	[AL_PCIE_REV_ID_5] = 8,
};

/**
 * Static functions
 */
static void
al_mod_pcie_port_wr_to_ro_set(struct al_mod_pcie_port *pcie_port, al_mod_bool enable)
{
	/* when disabling writes to RO, make sure any previous writes to
	 * config space were committed
	 */
	if (enable == AL_FALSE)
		al_mod_local_data_memory_barrier();

	al_mod_reg_write32(&pcie_port->regs->port_regs->rd_only_wr_en,
		       (enable == AL_TRUE) ? 1 : 0);

	/* when enabling writes to RO, make sure it is committed before trying
	 * to write to RO config space
	 */
	if (enable == AL_TRUE)
		al_mod_local_data_memory_barrier();
}

/** helper function to access dbi_cs2 registers */
static void al_mod_reg_write32_dbi_cs2(
	struct al_mod_pcie_port	*pcie_port,
	uint32_t		*offset,
	uint32_t		val)
{
	uintptr_t cs2_bit =
		(pcie_port->rev_id >= AL_PCIE_REV_ID_3) ? 0x4000 : 0x1000;

	al_mod_reg_write32((uint32_t *)((uintptr_t)offset | cs2_bit), val);
}

static unsigned int al_mod_pcie_speed_gen_code(enum al_mod_pcie_link_speed speed)
{
	if (speed == AL_PCIE_LINK_SPEED_GEN1)
		return 1;
	if (speed == AL_PCIE_LINK_SPEED_GEN2)
		return 2;
	if (speed == AL_PCIE_LINK_SPEED_GEN3)
		return 3;
	/* must not be reached */
	return 0;
}

static inline void al_mod_pcie_port_link_speed_ctrl_set(
	struct al_mod_pcie_port *pcie_port,
	enum al_mod_pcie_link_speed max_speed)
{
	struct al_mod_pcie_regs *regs = pcie_port->regs;

	al_mod_pcie_port_wr_to_ro_set(pcie_port, AL_TRUE);

	if (max_speed != AL_PCIE_LINK_SPEED_DEFAULT) {
		uint16_t max_speed_val = (uint16_t)al_mod_pcie_speed_gen_code(max_speed);
		al_mod_reg_write32_masked(
			(uint32_t __iomem *)(regs->core_space[0].pcie_link_cap_base),
			0xF, max_speed_val);
		al_mod_reg_write32_masked(
			(uint32_t __iomem *)(regs->core_space[0].pcie_cap_base
			+ (AL_PCI_EXP_LNKCTL2 >> 2)),
			0xF, max_speed_val);
	}

	al_mod_pcie_port_wr_to_ro_set(pcie_port, AL_FALSE);
}

static int al_mod_pcie_port_link_config(
	struct al_mod_pcie_port *pcie_port,
	const struct al_mod_pcie_link_params *link_params)
{
	struct al_mod_pcie_regs *regs = pcie_port->regs;
	uint8_t max_lanes = pcie_port->max_lanes;

	if ((link_params->max_payload_size != AL_PCIE_MPS_DEFAULT)	&&
	    (link_params->max_payload_size != AL_PCIE_MPS_128)		&&
	    (link_params->max_payload_size != AL_PCIE_MPS_256)) {
		al_mod_err("PCIe %d: unsupported Max Payload Size (%u)\n",
		       pcie_port->port_id, link_params->max_payload_size);
		return -EINVAL;
	}

	al_mod_pcie_port_link_speed_ctrl_set(pcie_port, link_params->max_speed);

	/* Change Max Payload Size, if needed.
	 * The Max Payload Size is only valid for PF0.
	 */
	if (link_params->max_payload_size != AL_PCIE_MPS_DEFAULT)
		al_mod_reg_write32_masked(regs->core_space[0].pcie_dev_ctrl_status,
				      PCIE_PORT_DEV_CTRL_STATUS_MPS_MASK,
				      link_params->max_payload_size <<
					PCIE_PORT_DEV_CTRL_STATUS_MPS_SHIFT);

	/** Snap from PCIe core spec:
	 * Link Mode Enable. Sets the number of lanes in the link that you want
	 * to connect to the link partner. When you have unused lanes in your
	 * system, then you must change the value in this register to reflect
	 * the number of lanes. You must also change the value in the
	 * "Predetermined Number of Lanes" field of the "Link Width and Speed
	 * Change Control Register".
	 * 000001: x1
	 * 000011: x2
	 * 000111: x4
	 * 001111: x8
	 * 011111: x16
	 * 111111: x32 (not supported)
	 */
	al_mod_reg_write32_masked(&regs->port_regs->gen2_ctrl,
				PCIE_PORT_GEN2_CTRL_NUM_OF_LANES_MASK,
				max_lanes << PCIE_PORT_GEN2_CTRL_NUM_OF_LANES_SHIFT);
	al_mod_reg_write32_masked(&regs->port_regs->port_link_ctrl,
				PCIE_PORT_LINK_CTRL_LINK_CAPABLE_MASK,
				(max_lanes + (max_lanes-1))
				<< PCIE_PORT_LINK_CTRL_LINK_CAPABLE_SHIFT);
	return 0;
}

static void al_mod_pcie_port_relaxed_pcie_ordering_config(
	struct al_mod_pcie_port *pcie_port,
	struct al_mod_pcie_relaxed_ordering_params *relaxed_ordering_params)
{
	struct al_mod_pcie_regs *regs = pcie_port->regs;
	enum al_mod_pcie_operating_mode op_mode = al_mod_pcie_operating_mode_get(pcie_port);
	/**
	 * Default:
	 *  - RC: Rx relaxed ordering only
	 *  - EP: TX relaxed ordering only
	 */
	al_mod_bool tx_relaxed_ordering = (op_mode == AL_PCIE_OPERATING_MODE_RC ? AL_FALSE : AL_TRUE);
	al_mod_bool rx_relaxed_ordering = (op_mode == AL_PCIE_OPERATING_MODE_RC ? AL_TRUE : AL_FALSE);

	if (relaxed_ordering_params) {
		tx_relaxed_ordering = relaxed_ordering_params->enable_tx_relaxed_ordering;
		rx_relaxed_ordering = relaxed_ordering_params->enable_rx_relaxed_ordering;
	}

	/** PCIe ordering:
	 *  - disable outbound completion must be stalled behind outbound write
	 *    ordering rule enforcement is disabled for root-port
	 *  - disables read completion on the master port push slave writes for end-point
	 */
	al_mod_reg_write32_masked(
		regs->axi.ordering.pos_cntl,
		PCIE_AXI_POS_ORDER_BYPASS_CMPL_AFTER_WR_FIX |
		PCIE_AXI_POS_ORDER_EP_CMPL_AFTER_WR_DIS |
		PCIE_AXI_POS_ORDER_EP_CMPL_AFTER_WR_SUPPORT_INTERLV_DIS |
		PCIE_AXI_POS_ORDER_SEGMENT_BUFFER_DONT_WAIT_FOR_P_WRITES,
		(tx_relaxed_ordering ?
		(PCIE_AXI_POS_ORDER_BYPASS_CMPL_AFTER_WR_FIX |
		PCIE_AXI_POS_ORDER_SEGMENT_BUFFER_DONT_WAIT_FOR_P_WRITES) : 0) |
		(rx_relaxed_ordering ?
		(PCIE_AXI_POS_ORDER_EP_CMPL_AFTER_WR_DIS |
		PCIE_AXI_POS_ORDER_EP_CMPL_AFTER_WR_SUPPORT_INTERLV_DIS) : 0));
}

static int al_mod_pcie_dev_id_get(struct al_mod_pcie_revx_regs *pcie_reg_base)
{
	return al_mod_reg_read32(&pcie_reg_base->axi.device_id.device_rev_id) &
		PCIE_AXI_DEVICE_ID_REG_DEV_ID_MASK;
}

/* For Alpine V3+ return the PCIe revision id, all other revisions return NA */
static int al_mod_pcie_subrev_id_get(struct al_mod_pcie_revx_regs *pcie_reg_base, uint8_t rev_id)
{
	int subrev_id = PCIE_AXI_DEVICE_ID_REG_REV_ID_NA;

	if (rev_id >= AL_PCIE_REV_ID_4) {
		subrev_id = al_mod_reg_read32(&pcie_reg_base->axi.device_id.device_rev_id) &
			    PCIE_AXI_DEVICE_ID_REG_REV_ID_MASK;

		al_mod_assert((subrev_id >= PCIE_AXI_DEVICE_ID_REG_REV_ID_X4) &&
			  (subrev_id <= PCIE_AXI_DEVICE_ID_REG_REV_ID_X16));
	}

	return subrev_id;
}

static int al_mod_pcie_rev_id_get(
	void __iomem *pbs_reg_base,
	void __iomem *pcie_reg_base)
{
	unsigned int chip_id_dev;
	unsigned int rev_id;

	/*
	 * Addressing RMN: 4022, 10069
	 *
	 * RMN description:
	 * The revision ID of the PCIe Core is placed in the LSB of the PCI Class field, which
	 * creates two problems:
	 * - The port must be enabled to be able to determine the revision ID
	 * - The wrong revision ID might be placed in the field as it can be programmed by the
	 *   application in EP mode
	 *
	 * Software flow:
	 * Read the revision ID from the PBS chip ID
	 */

	/* get revision ID from the PBS */
	chip_id_dev = al_mod_pbs_dev_id_get(pbs_reg_base);

	if (chip_id_dev == PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V1) {
		rev_id = AL_PCIE_REV_ID_1;
	} else if (chip_id_dev == PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V2) {
		uint32_t dev_id = al_mod_pcie_dev_id_get(pcie_reg_base);

		if (dev_id == PCIE_AXI_DEVICE_ID_REG_DEV_ID_X4) {
			rev_id = AL_PCIE_REV_ID_2;
		} else if (dev_id == PCIE_AXI_DEVICE_ID_REG_DEV_ID_X8) {
			rev_id = AL_PCIE_REV_ID_3;
		} else {
			al_mod_warn("%s: Revision ID is unknown\n",
				__func__);
			return -EINVAL;
		}
	} else if (chip_id_dev == PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V3) {
		uint32_t dev_id = al_mod_pcie_dev_id_get(pcie_reg_base);

		if (dev_id == PCIE_AXI_DEVICE_ID_REG_DEV_ID_X16) {
			rev_id = AL_PCIE_REV_ID_4;
		} else {
			al_mod_warn("%s: Revision ID is unknown\n",
				__func__);
			return -EINVAL;
		}
	} else if (chip_id_dev == PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V4) {
		uint32_t dev_id = al_mod_pcie_dev_id_get(pcie_reg_base);

		if (dev_id == PCIE_AXI_DEVICE_ID_REG_DEV_ID_REV5) {
			rev_id = AL_PCIE_REV_ID_5;
		} else {
			al_mod_warn("%s: Revision ID is unknown\n",
				__func__);
			return -EINVAL;
		}
	} else {
		al_mod_warn("%s: Revision ID is unknown\n",
			__func__);
		return -EINVAL;
	}
	return rev_id;
}

static int
al_mod_pcie_port_lat_rply_timers_config(
	struct al_mod_pcie_port *pcie_port,
	const struct al_mod_pcie_latency_replay_timers  *lat_rply_timers)
{
	struct al_mod_pcie_regs *regs = pcie_port->regs;
	uint32_t	reg = 0;

	AL_REG_FIELD_SET(reg, 0xFFFF, 0, lat_rply_timers->round_trip_lat_limit);
	AL_REG_FIELD_SET(reg, 0xFFFF0000, 16, lat_rply_timers->replay_timer_limit);

	al_mod_reg_write32(&regs->port_regs->ack_lat_rply_timer, reg);
	return 0;
}

static void al_mod_pcie_ib_hcrd_os_ob_reads_config_default(
	struct al_mod_pcie_port *pcie_port)
{
	struct al_mod_pcie_ib_hcrd_os_ob_reads_config ib_hcrd_os_ob_reads_config;
	enum al_mod_pcie_operating_mode op_mode = al_mod_pcie_operating_mode_get(pcie_port);
	uint8_t	rev_id = pcie_port->rev_id;
	int subrev_id = pcie_port->subrev_id;

	al_mod_assert(ib_hcrd_os_ob_reads_hw_config[rev_id][subrev_id].ib_hcrd_supported);

	ib_hcrd_os_ob_reads_config.nof_outstanding_ob_reads =
	  ib_hcrd_os_ob_reads_defaults[rev_id][subrev_id][op_mode].nof_outstanding_ob_reads;
	ib_hcrd_os_ob_reads_config.nof_cpl_hdr =
	  ib_hcrd_os_ob_reads_defaults[rev_id][subrev_id][op_mode].nof_cpl_hdr;
	ib_hcrd_os_ob_reads_config.nof_np_hdr =
	  ib_hcrd_os_ob_reads_defaults[rev_id][subrev_id][op_mode].nof_np_hdr;
	ib_hcrd_os_ob_reads_config.nof_p_hdr =
	  ib_hcrd_os_ob_reads_defaults[rev_id][subrev_id][op_mode].nof_p_hdr;

	al_mod_pcie_port_ib_hcrd_os_ob_reads_config(pcie_port, &ib_hcrd_os_ob_reads_config);
};

/** return AL_TRUE if link is up, AL_FALSE otherwise */
static al_mod_bool al_mod_pcie_check_link(
	struct al_mod_pcie_port *pcie_port,
	uint8_t *ltssm_ret)
{
	struct al_mod_pcie_regs *regs = (struct al_mod_pcie_regs *)pcie_port->regs;
	uint32_t info_0;
	uint8_t	ltssm_state;

	info_0 = al_mod_reg_read32(regs->app.debug.info_0);

	ltssm_state = AL_REG_FIELD_GET(info_0,
			PCIE_W_DEBUG_INFO_0_LTSSM_STATE_MASK,
			PCIE_W_DEBUG_INFO_0_LTSSM_STATE_SHIFT);

	al_mod_dbg("PCIe %d: Port Debug 0: 0x%08x. LTSSM state :0x%x\n",
		pcie_port->port_id, info_0, ltssm_state);

	if (ltssm_ret)
		*ltssm_ret = ltssm_state;

	if ((ltssm_state == AL_PCIE_LTSSM_STATE_L0) ||
			(ltssm_state == AL_PCIE_LTSSM_STATE_L0S))
		return AL_TRUE;
	return AL_FALSE;
}

static int
al_mod_pcie_port_gen2_params_config(struct al_mod_pcie_port *pcie_port,
				const struct al_mod_pcie_gen2_params *gen2_params)
{
	struct al_mod_pcie_regs *regs = pcie_port->regs;
	uint32_t gen2_ctrl;

	al_mod_dbg("PCIe %d: Gen2 params config: Tx Swing %s, interrupt on link Eq %s, set Deemphasis %s\n",
	       pcie_port->port_id,
	       gen2_params->tx_swing_low ? "Low" : "Full",
	       gen2_params->tx_compliance_receive_enable? "enable" : "disable",
	       gen2_params->set_deemphasis? "enable" : "disable");

	gen2_ctrl = al_mod_reg_read32(&regs->port_regs->gen2_ctrl);

	if (gen2_params->tx_swing_low)
		AL_REG_BIT_SET(gen2_ctrl, PCIE_PORT_GEN2_CTRL_TX_SWING_LOW_SHIFT);
	else
		AL_REG_BIT_CLEAR(gen2_ctrl, PCIE_PORT_GEN2_CTRL_TX_SWING_LOW_SHIFT);

	if (gen2_params->tx_compliance_receive_enable)
		AL_REG_BIT_SET(gen2_ctrl, PCIE_PORT_GEN2_CTRL_TX_COMPLIANCE_RCV_SHIFT);
	else
		AL_REG_BIT_CLEAR(gen2_ctrl, PCIE_PORT_GEN2_CTRL_TX_COMPLIANCE_RCV_SHIFT);

	if (gen2_params->set_deemphasis)
		AL_REG_BIT_SET(gen2_ctrl, PCIE_PORT_GEN2_CTRL_DEEMPHASIS_SET_SHIFT);
	else
		AL_REG_BIT_CLEAR(gen2_ctrl, PCIE_PORT_GEN2_CTRL_DEEMPHASIS_SET_SHIFT);

	al_mod_reg_write32(&regs->port_regs->gen2_ctrl, gen2_ctrl);

	return 0;
}


static uint16_t
gen3_lane_eq_param_to_val(const struct al_mod_pcie_gen3_lane_eq_params *eq_params)
{
	uint16_t eq_control = 0;

	eq_control = eq_params->downstream_port_transmitter_preset & 0xF;
	eq_control |= (eq_params->downstream_port_receiver_preset_hint & 0x7) << 4;
	eq_control |= (eq_params->upstream_port_transmitter_preset & 0xF) << 8;
	eq_control |= (eq_params->upstream_port_receiver_preset_hint & 0x7) << 12;

	return eq_control;
}

static int
al_mod_pcie_port_gen3_params_config(struct al_mod_pcie_port *pcie_port,
				enum al_mod_pcie_operating_mode op_mode,
				const struct al_mod_pcie_gen3_params *gen3_params)
{
	struct al_mod_pcie_regs *regs = pcie_port->regs;
	uint32_t reg = 0;
	uint16_t __iomem *lanes_eq_base = (uint16_t __iomem *)(regs->core_space[0].pcie_sec_ext_cap_base + (0xC >> 2));
	unsigned int i;

	al_mod_dbg("PCIe %d: Gen3 params config: Equalization %s, interrupt on link Eq %s\n",
	       pcie_port->port_id,
	       gen3_params->perform_eq ? "enable" : "disable",
	       gen3_params->interrupt_enable_on_link_eq_request? "enable" : "disable");

	if (gen3_params->perform_eq)
		AL_REG_BIT_SET(reg, 0);
	if (gen3_params->interrupt_enable_on_link_eq_request)
		AL_REG_BIT_SET(reg, 1);

	al_mod_reg_write32(regs->core_space[0].pcie_sec_ext_cap_base + (4 >> 2),
		       reg);

	al_mod_pcie_port_wr_to_ro_set(pcie_port, AL_TRUE);

	for (i = 0; i < gen3_params->eq_params_elements; i += 2) {
		uint32_t eq_control =
			(uint32_t)gen3_lane_eq_param_to_val(gen3_params->eq_params + i) |
			(uint32_t)gen3_lane_eq_param_to_val(gen3_params->eq_params + i + 1) << 16;

		al_mod_dbg("PCIe %d: Set EQ (0x%08x) for lane %d, %d\n", pcie_port->port_id, eq_control, i, i + 1);
		al_mod_reg_write32((uint32_t *)(lanes_eq_base + i), eq_control);
	}

	al_mod_pcie_port_wr_to_ro_set(pcie_port, AL_FALSE);

	reg = al_mod_reg_read32(&regs->port_regs->gen3_ctrl);
	if (gen3_params->eq_disable)
		AL_REG_BIT_SET(reg, PCIE_PORT_GEN3_CTRL_EQ_DISABLE_SHIFT);
	else
		AL_REG_BIT_CLEAR(reg, PCIE_PORT_GEN3_CTRL_EQ_DISABLE_SHIFT);

	if (gen3_params->eq_phase2_3_disable)
		AL_REG_BIT_SET(reg, PCIE_PORT_GEN3_CTRL_EQ_PHASE_2_3_DISABLE_SHIFT);
	else
		AL_REG_BIT_CLEAR(reg, PCIE_PORT_GEN3_CTRL_EQ_PHASE_2_3_DISABLE_SHIFT);

	if (gen3_params->eq_redo_bypass)
		AL_REG_BIT_SET(reg, PCIE_PORT_GEN3_CTRL_EQ_REDO_BYPASS_SHIFT);
	else
		AL_REG_BIT_CLEAR(reg, PCIE_PORT_GEN3_CTRL_EQ_REDO_BYPASS_SHIFT);

	al_mod_reg_write32(&regs->port_regs->gen3_ctrl, reg);

	reg = 0;
	AL_REG_FIELD_SET(reg, PCIE_PORT_GEN3_EQ_LF_MASK,
			 PCIE_PORT_GEN3_EQ_LF_SHIFT,
			 gen3_params->local_lf);
	AL_REG_FIELD_SET(reg, PCIE_PORT_GEN3_EQ_FS_MASK,
			 PCIE_PORT_GEN3_EQ_FS_SHIFT,
			 gen3_params->local_fs);

	al_mod_reg_write32(&regs->port_regs->gen3_eq_fs_lf, reg);

	reg = 0;
	AL_REG_FIELD_SET(reg, PCIE_AXI_MISC_ZERO_LANEX_PHY_MAC_LOCAL_LF_MASK,
			 PCIE_AXI_MISC_ZERO_LANEX_PHY_MAC_LOCAL_LF_SHIFT,
			 gen3_params->local_lf);
	AL_REG_FIELD_SET(reg, PCIE_AXI_MISC_ZERO_LANEX_PHY_MAC_LOCAL_FS_MASK,
			 PCIE_AXI_MISC_ZERO_LANEX_PHY_MAC_LOCAL_FS_SHIFT,
			 gen3_params->local_fs);

	for (i = 0; i < lanes_num[pcie_port->rev_id]; i++)
		al_mod_reg_write32(regs->axi.conf.zero_lane[i], reg);

	/*
	 * Gen3 EQ Control Register:
	 * - Behavior After 24 ms Timeout (when optimal settings are not
	 *   found): Recovery.Speed
	 * - Phase2_3 2 ms Timeout Disable
	 */
	reg = 0x00000020;

	/* Set preset request vector */
	AL_REG_FIELD_SET(reg, PCIE_PORT_GEN3_EQ_CTRL_PSET_REQ_VEC_MASK,
		PCIE_PORT_GEN3_EQ_CTRL_PSET_REQ_VEC_SHIFT,
		(op_mode == AL_PCIE_OPERATING_MODE_RC) ?
		gen3_params->pset_req_vec_rc :
		gen3_params->pset_req_vec_ep);
	/* Set feedback mode */
	AL_REG_FIELD_SET(reg, PCIE_PORT_GEN3_EQ_CTRL_FB_MODE_MASK,
		PCIE_PORT_GEN3_EQ_CTRL_FB_MODE_SHIFT, gen3_params->fb_mode);

	al_mod_reg_write32(&regs->port_regs->gen3_eq_ctrl, reg);

	/* Set direction change feedback mode params */
	if (gen3_params->fb_mode == AL_PCIE_GEN3_FB_MODE_DIR_CHG) {
		al_mod_assert(gen3_params->fmdc);
		al_mod_assert(gen3_params->fmdc->max_post_cusror_delta <=
			AL_PCIE_PORT_GEN3_EQ_FMDC_MAX_POST_CUSROR_DELTA_MAX);
		al_mod_assert(gen3_params->fmdc->max_pre_cusror_delta <=
			AL_PCIE_PORT_GEN3_EQ_FMDC_MAX_PRE_CUSROR_DELTA_MAX);
		al_mod_assert(gen3_params->fmdc->n_evals <=
			AL_PCIE_PORT_GEN3_EQ_FMDC_N_EVALS_MAX);
		al_mod_assert(gen3_params->fmdc->t_min_phas <=
			AL_PCIE_PORT_GEN3_EQ_FMDC_T_MIN_PHAS_MAX);

		al_mod_reg_write32_masked(&regs->port_regs->gen3_eq_fb_mode_dir_chg,
			PCIE_PORT_GEN3_EQ_FMDC_MAX_POST_CUSROR_DELTA_MASK |
			PCIE_PORT_GEN3_EQ_FMDC_MAX_PRE_CUSROR_DELTA_MASK |
			PCIE_PORT_GEN3_EQ_FMDC_N_EVALS_MASK |
			PCIE_PORT_GEN3_EQ_FMDC_T_MIN_PHAS_MASK,
			(gen3_params->fmdc->max_post_cusror_delta <<
			PCIE_PORT_GEN3_EQ_FMDC_MAX_POST_CUSROR_DELTA_SHIFT) |
			(gen3_params->fmdc->max_pre_cusror_delta <<
			PCIE_PORT_GEN3_EQ_FMDC_MAX_PRE_CUSROR_DELTA_SHIFT) |
			(gen3_params->fmdc->n_evals <<
			PCIE_PORT_GEN3_EQ_FMDC_N_EVALS_SHIFT) |
			(gen3_params->fmdc->t_min_phas <<
			PCIE_PORT_GEN3_EQ_FMDC_T_MIN_PHAS_SHIFT));
	}

	return 0;
}

static int
al_mod_pcie_port_pf_params_config(struct al_mod_pcie_pf *pcie_pf,
			      const struct al_mod_pcie_pf_config_params *pf_params)
{
	struct al_mod_pcie_port *pcie_port = pcie_pf->pcie_port;
	struct al_mod_pcie_regs *regs = pcie_port->regs;
	unsigned int pf_num = pcie_pf->pf_num;
	int bar_idx;
	int ret;

	al_mod_pcie_port_wr_to_ro_set(pcie_port, AL_TRUE);

	/* Disable D1 and D3hot capabilities */
	if (pf_params->cap_d1_d3hot_dis)
		al_mod_reg_write32_masked(
			regs->core_space[pf_num].pcie_pm_cap_base,
			AL_FIELD_MASK(26, 25) | AL_FIELD_MASK(31, 28), 0);

	/* Set/Clear FLR bit */
	if (pf_params->cap_flr_dis)
		al_mod_reg_write32_masked(
			regs->core_space[pf_num].pcie_dev_cap_base,
			AL_PCI_EXP_DEVCAP_FLR, 0);
	else
		al_mod_reg_write32_masked(
			regs->core_space[pcie_pf->pf_num].pcie_dev_cap_base,
			AL_PCI_EXP_DEVCAP_FLR, AL_PCI_EXP_DEVCAP_FLR);

	/* Disable ASPM capability */
	if (pf_params->cap_aspm_dis) {
		al_mod_reg_write32_masked(
			regs->core_space[pf_num].pcie_cap_base + (AL_PCI_EXP_LNKCAP >> 2),
			AL_PCI_EXP_LNKCAP_ASPMS, 0);
	}

	if (pf_params->id_params) {
		if (pf_params->id_params->vendor_id_override)
			al_mod_reg_write32_masked(
				(uint32_t __iomem *)(&regs->core_space[pf_num].config_header[0]),
				AL_PCI_DEV_ID_VEN_ID_VEN_ID_MASK,
				pf_params->id_params->vendor_id <<
				AL_PCI_DEV_ID_VEN_ID_VEN_ID_SHIFT);
		if (pf_params->id_params->device_id_override)
			al_mod_reg_write32_masked(
				(uint32_t __iomem *)(&regs->core_space[pf_num].config_header[0]),
				AL_PCI_DEV_ID_VEN_ID_DEV_ID_MASK,
				pf_params->id_params->device_id <<
				AL_PCI_DEV_ID_VEN_ID_DEV_ID_SHIFT);
		if (pf_params->id_params->revision_id_override)
			al_mod_reg_write32_masked(
				(uint32_t __iomem *)(&regs->core_space[pf_num].config_header[0]
				+ (PCI_CLASS_REVISION >> 2)),
				PCI_CLASS_REVISION_REVISION_MASK,
				pf_params->id_params->revision_id <<
				PCI_CLASS_REVISION_REVISION_SHIFT);
		if (pf_params->id_params->class_code_override)
			al_mod_reg_write32_masked(
				(uint32_t __iomem *)(&regs->core_space[pf_num].config_header[0]
				+ (PCI_CLASS_REVISION >> 2)),
				PCI_CLASS_REVISION_CLASS_MASK,
				pf_params->id_params->class_code <<
				PCI_CLASS_REVISION_CLASS_SHIFT);
	}

	if (!pf_params->bar_params_valid) {
		ret = 0;
		goto done;
	}

	for (bar_idx = 0; bar_idx < 6;){ /* bar_idx will be incremented depending on bar type */
		const struct al_mod_pcie_ep_bar_params *params = pf_params->bar_params + bar_idx;
		uint32_t mask = 0;
		uint32_t ctrl = 0;
		uint32_t __iomem *bar_addr = &regs->core_space[pf_num].config_header[(AL_PCI_BASE_ADDRESS_0 >> 2) + bar_idx];

		if (params->enable) {
			uint64_t size = params->size;

			if (params->memory_64_bit) {
				const struct al_mod_pcie_ep_bar_params *next_params = params + 1;
				/* 64 bars start at even index (BAR0, BAR 2 or BAR 4) */
				if (bar_idx & 1) {
					ret = -EINVAL;
					goto done;
				}

				/* next BAR must be disabled */
				if (next_params->enable) {
					ret = -EINVAL;
					goto done;
				}

				/* 64 bar must be memory bar */
				if (!params->memory_space) {
					ret = -EINVAL;
					goto done;
				}
			} else {
				if (size > AL_PCIE_MAX_32_MEMORY_BAR_SIZE)
					return -EINVAL;
				/* 32 bit space can't be prefetchable */
				if (params->memory_is_prefetchable) {
					ret = -EINVAL;
					goto done;
				}
			}

			if (params->memory_space) {
				if (size < AL_PCIE_MIN_MEMORY_BAR_SIZE) {
					al_mod_err("PCIe %d: memory BAR %d: size (0x%" PRIx64 ")"
							" less that minimal allowed value\n",
							pcie_port->port_id, bar_idx, size);
					ret = -EINVAL;
					goto done;
				}
			} else {
				/* IO can't be prefetchable */
				if (params->memory_is_prefetchable) {
					ret = -EINVAL;
					goto done;
				}

				if (size < AL_PCIE_MIN_IO_BAR_SIZE) {
					al_mod_err("PCIe %d: IO BAR %d: size (0x%" PRIx64 ")"
							" less that minimal allowed value\n",
							pcie_port->port_id, bar_idx, size);
					ret = -EINVAL;
					goto done;
				}
			}

			/* size must be power of 2 */
			if (size & (size - 1)) {
				al_mod_err("PCIe %d: BAR %d:size (0x%" PRIx64 ") must be "
					"power of 2\n",
					pcie_port->port_id, bar_idx, size);
				ret = -EINVAL;
				goto done;
			}

			/* If BAR is 64-bit, disable the next BAR before
			 * configuring this one
			 */
			if (params->memory_64_bit)
				al_mod_reg_write32_dbi_cs2(pcie_port, bar_addr + 1, 0);

			mask = 1; /* enable bit*/
			mask |= (params->size - 1) & 0xFFFFFFFF;

			al_mod_reg_write32_dbi_cs2(pcie_port, bar_addr , mask);

			if (params->memory_space == AL_FALSE)
				ctrl = AL_PCI_BASE_ADDRESS_SPACE_IO;
			if (params->memory_64_bit)
				ctrl |= AL_PCI_BASE_ADDRESS_MEM_TYPE_64;
			if (params->memory_is_prefetchable)
				ctrl |= AL_PCI_BASE_ADDRESS_MEM_PREFETCH;
			al_mod_reg_write32(bar_addr, ctrl);

			if (params->memory_64_bit) {
				mask = ((params->size - 1) >> 32) & 0xFFFFFFFF;
				al_mod_reg_write32_dbi_cs2(pcie_port, bar_addr + 1, mask);
			}

		} else {
			al_mod_reg_write32_dbi_cs2(pcie_port, bar_addr , mask);
		}
		if (params->enable && params->memory_64_bit)
			bar_idx += 2;
		else
			bar_idx += 1;
	}

	if (pf_params->exp_bar_params.enable) {
		if (pcie_port->rev_id < AL_PCIE_REV_ID_3) {
			al_mod_err("PCIe %d: Expansion BAR enable not supported\n", pcie_port->port_id);
			ret = -ENOSYS;
			goto done;
		} else {
			/* Enable exp ROM */
			uint32_t __iomem *exp_rom_bar_addr =
			&regs->core_space[pf_num].config_header[AL_PCI_EXP_ROM_BASE_ADDRESS >> 2];
			uint32_t mask = 1; /* enable bit*/
			mask |= (pf_params->exp_bar_params.size - 1) & 0xFFFFFFFF;
			al_mod_reg_write32_dbi_cs2(pcie_port, exp_rom_bar_addr , mask);
		}
	} else if (pcie_port->rev_id >= AL_PCIE_REV_ID_3) {
		/* Disable exp ROM */
		uint32_t __iomem *exp_rom_bar_addr =
			&regs->core_space[pf_num].config_header[AL_PCI_EXP_ROM_BASE_ADDRESS >> 2];
		al_mod_reg_write32_dbi_cs2(pcie_port, exp_rom_bar_addr , 0);
	}

	/* Open CPU generated msi and legacy interrupts in pcie wrapper logic */
	if (pcie_port->rev_id == AL_PCIE_REV_ID_1) {
		al_mod_reg_write32(regs->app.soc_int[pf_num].mask_inta_leg_0, AL_BIT(21));
	} else if ((pcie_port->rev_id == AL_PCIE_REV_ID_2) ||
		   (pcie_port->rev_id == AL_PCIE_REV_ID_3)) {
		al_mod_reg_write32(regs->app.soc_int[pf_num].mask_inta_leg_3, AL_BIT(18));
	} else if (pcie_port->rev_id >= AL_PCIE_REV_ID_4) {
		al_mod_reg_write32(regs->app.soc_int[pf_num].mask_inta_leg_6, AL_BIT(0));
	} else {
		al_mod_assert(0);
		ret = -ENOSYS;
		goto done;
	}

	/**
	 * Addressing RMN: 1547
	 *
	 * RMN description:
	 * 1. Whenever writing to 0x2xx offset, the write also happens to
	 * 0x3xx address, meaning two registers are written instead of one.
	 * 2. Read and write from 0x3xx work ok.
	 *
	 * Software flow:
	 * Backup the value of the app.int_grp_a.mask_a register, because
	 * app.int_grp_a.mask_clear_a gets overwritten during the write to
	 * app.soc.mask_msi_leg_0 register.
	 * Restore the original value after the write to app.soc.mask_msi_leg_0
	 * register.
	 */
	if (pcie_port->rev_id == AL_PCIE_REV_ID_1) {
		al_mod_reg_write32(regs->app.soc_int[pf_num].mask_msi_leg_0, AL_BIT(22));
	} else if ((pcie_port->rev_id == AL_PCIE_REV_ID_2) ||
		   (pcie_port->rev_id == AL_PCIE_REV_ID_3)) {
		al_mod_reg_write32(regs->app.soc_int[pf_num].mask_msi_leg_3, AL_BIT(19));
	} else if (pcie_port->rev_id >= AL_PCIE_REV_ID_4) {
		al_mod_reg_write32(regs->app.soc_int[pf_num].mask_msi_leg_6, AL_BIT(1));
	} else {
		al_mod_assert(0);
		ret = -ENOSYS;
		goto done;
	}

	ret = 0;

done:
	al_mod_pcie_port_wr_to_ro_set(pcie_port, AL_FALSE);

	return ret;
}

static int al_mod_pcie_port_sris_config(
	struct al_mod_pcie_port *pcie_port,
	struct al_mod_pcie_sris_params *sris_params,
	enum al_mod_pcie_link_speed link_speed)
{
	int rc = 0;
	struct al_mod_pcie_regs *regs = pcie_port->regs;

	if (sris_params->use_defaults) {
		sris_params->kp_counter_gen3 = (pcie_port->rev_id > AL_PCIE_REV_ID_1) ?
						PCIE_SRIS_KP_COUNTER_GEN3_DEFAULT_VAL : 0;
		sris_params->kp_counter_gen21 = PCIE_SRIS_KP_COUNTER_GEN21_DEFAULT_VAL;

		al_mod_dbg("PCIe %d: configuring SRIS with default values kp_gen3[%d] kp_gen21[%d]\n",
			pcie_port->port_id,
			sris_params->kp_counter_gen3,
			sris_params->kp_counter_gen21);
	}

	switch (pcie_port->rev_id) {
	case AL_PCIE_REV_ID_5:
	case AL_PCIE_REV_ID_4:
	case AL_PCIE_REV_ID_3:
		al_mod_reg_write32_masked(regs->app.cfg_func_ext.cfg,
				PCIE_W_CFG_FUNC_EXT_CFG_APP_SRIS_MODE,
				PCIE_W_CFG_FUNC_EXT_CFG_APP_SRIS_MODE);
		/* fall through */
	case AL_PCIE_REV_ID_2:
		al_mod_reg_write32_masked(regs->app.global_ctrl.sris_kp_counter,
			PCIE_W_GLOBAL_CTRL_SRIS_KP_COUNTER_VALUE_GEN3_SRIS_MASK |
			PCIE_W_GLOBAL_CTRL_SRIS_KP_COUNTER_VALUE_GEN21_SRIS_MASK |
			PCIE_W_GLOBAL_CTRL_SRIS_KP_COUNTER_VALUE_PCIE_X4_SRIS_EN,
			(sris_params->kp_counter_gen3 <<
				PCIE_W_GLOBAL_CTRL_SRIS_KP_COUNTER_VALUE_GEN3_SRIS_SHIFT) |
			(sris_params->kp_counter_gen21 <<
				PCIE_W_GLOBAL_CTRL_SRIS_KP_COUNTER_VALUE_GEN21_SRIS_SHIFT) |
			PCIE_W_GLOBAL_CTRL_SRIS_KP_COUNTER_VALUE_PCIE_X4_SRIS_EN);
		break;

	case AL_PCIE_REV_ID_1:
		if ((link_speed == AL_PCIE_LINK_SPEED_GEN3) && (sris_params->kp_counter_gen3)) {
			al_mod_err("PCIe %d: cannot config Gen%d SRIS with rev_id[%d]\n",
				pcie_port->port_id, al_mod_pcie_speed_gen_code(link_speed),
				pcie_port->rev_id);
			return -EINVAL;
		}

		al_mod_reg_write32_masked(&regs->port_regs->filter_mask_reg_1,
			PCIE_FLT_MASK_SKP_INT_VAL_MASK,
			sris_params->kp_counter_gen21);
		break;

	default:
		al_mod_err("PCIe %d: SRIS config is not supported in rev_id[%d]\n",
			pcie_port->port_id, pcie_port->rev_id);
		al_mod_assert(0);
		return -EINVAL;
	}

	return rc;
}

static void
al_mod_pcie_port_ib_hcrd_config(struct al_mod_pcie_port *pcie_port)
{
	struct al_mod_pcie_regs *regs = pcie_port->regs;

	al_mod_reg_write32_masked(
		&regs->port_regs->vc0_posted_rcv_q_ctrl,
		RADM_PQ_HCRD_VC0_MASK,
		(pcie_port->ib_hcrd_config.nof_p_hdr - 1)
			<< RADM_PQ_HCRD_VC0_SHIFT);

	al_mod_reg_write32_masked(
		&regs->port_regs->vc0_non_posted_rcv_q_ctrl,
		RADM_NPQ_HCRD_VC0_MASK,
		(pcie_port->ib_hcrd_config.nof_np_hdr - 1)
			<< RADM_NPQ_HCRD_VC0_SHIFT);
}

static unsigned int
al_mod_pcie_port_max_num_of_pfs_get(struct al_mod_pcie_port *pcie_port)
{
	struct al_mod_pcie_regs *regs = pcie_port->regs;
	uint32_t max_func_num;
	uint32_t max_num_of_pfs;

	/**
	 * Only in REV3+, when port is already enabled, max_num_of_pfs is already
	 * initialized, return it. Otherwise, return default: 1 PF
	 */
	if ((pcie_port->rev_id >= AL_PCIE_REV_ID_3)
		&& al_mod_pcie_port_is_enabled(pcie_port)) {
		max_func_num = al_mod_reg_read32(&regs->port_regs->timer_ctrl_max_func_num);
		max_num_of_pfs = AL_REG_FIELD_GET(max_func_num, PCIE_PORT_GEN3_MAX_FUNC_NUM, 0) + 1;
		return max_num_of_pfs;
	}
	return 1;
}

/** Enable ecrc generation in outbound atu (Addressing RMN: 5119) */
static void al_mod_pcie_ecrc_gen_ob_atu_enable(struct al_mod_pcie_port *pcie_port, unsigned int pf_num)
{
	struct al_mod_pcie_regs *regs = pcie_port->regs;
	int max_ob_atu = (pcie_port->rev_id >= AL_PCIE_REV_ID_3) ?
		AL_PCIE_REV_3_4_ATU_NUM_OUTBOUND_REGIONS : AL_PCIE_REV_1_2_ATU_NUM_OUTBOUND_REGIONS;
	int i;
	for (i = 0; i < max_ob_atu; i++) {
		al_mod_bool enable = 0;
		uint32_t reg = 0;
		unsigned int func_num;
		AL_REG_FIELD_SET(reg, 0xF, 0, i);
		AL_REG_BIT_VAL_SET(reg, 31, AL_PCIE_ATU_DIR_OUTBOUND);
		al_mod_reg_write32(&regs->port_regs->iatu.index, reg);
		reg = al_mod_reg_read32(&regs->port_regs->iatu.cr2);
		enable = AL_REG_BIT_GET(reg, 31) ? AL_TRUE : AL_FALSE;
		reg = al_mod_reg_read32(&regs->port_regs->iatu.cr1);
		func_num = AL_REG_FIELD_GET(reg,
				PCIE_IATU_CR1_FUNC_NUM_MASK,
				PCIE_IATU_CR1_FUNC_NUM_SHIFT);
		if ((enable == AL_TRUE) && (pf_num == func_num)) {
			/* Set TD bit */
			AL_REG_BIT_SET(reg, 8);
			al_mod_reg_write32(&regs->port_regs->iatu.cr1, reg);
		}
	}
}

/******************************************************************************/
/***************************** API Implementation *****************************/
/******************************************************************************/

/*************************** PCIe Initialization API **************************/

/**
 * Initializes a PCIe port handle structure
 * Caution: this function should not read/write to any register except for
 * reading RO register (REV_ID for example)
 */
int al_mod_pcie_port_handle_init(
	struct al_mod_pcie_port 	*pcie_port,
	void __iomem		*pcie_reg_base,
	void __iomem		*pbs_reg_base,
	unsigned int		port_id)
{
	int ret;

	pcie_port->pcie_reg_base = pcie_reg_base;
	pcie_port->regs = &pcie_port->regs_ptrs;
	pcie_port->ex_regs = NULL;
	pcie_port->pbs_regs = pbs_reg_base;
	pcie_port->port_id = port_id;
	pcie_port->max_lanes = 0;

	ret = al_mod_pcie_rev_id_get(pbs_reg_base, pcie_reg_base);
	if (ret < 0)
		return ret;

	pcie_port->rev_id = ret;

	ret = al_mod_pcie_subrev_id_get(pcie_reg_base, pcie_port->rev_id);
	if (ret < 0)
		return ret;

	pcie_port->subrev_id = ret;

	/* Zero all regs */
	al_mod_memset(pcie_port->regs, 0, sizeof(struct al_mod_pcie_regs));

#if (REV1_SUPPORTED)
	if (pcie_port->rev_id == AL_PCIE_REV_ID_1) {
		al_mod_pcie_port_handle_init_reg_ptr_set_rev1(pcie_port, pcie_reg_base);
	} else
#endif
#if (REV2_SUPPORTED)
	if (pcie_port->rev_id == AL_PCIE_REV_ID_2) {
		al_mod_pcie_port_handle_init_reg_ptr_set_rev2(pcie_port, pcie_reg_base);
	} else
#endif
#if (REV3_SUPPORTED)
	if (pcie_port->rev_id == AL_PCIE_REV_ID_3) {
		al_mod_pcie_port_handle_init_reg_ptr_set_rev3(pcie_port, pcie_reg_base);
	} else
#endif
#if (REV4_SUPPORTED)
	if (pcie_port->rev_id == AL_PCIE_REV_ID_4) {
		al_mod_pcie_port_handle_init_reg_ptr_set_rev4(pcie_port, pcie_reg_base);
	} else
#endif
#if (REV5_SUPPORTED)
	if (pcie_port->rev_id == AL_PCIE_REV_ID_5) {
		al_mod_pcie_port_handle_init_reg_ptr_set_rev5(pcie_port, pcie_reg_base);
	} else
#endif
	{
		al_mod_warn("%s: Revision ID is unknown\n",
			__func__);
		return -EINVAL;
	}

	/* set maximum number of physical functions */
	pcie_port->max_num_of_pfs = al_mod_pcie_port_max_num_of_pfs_get(pcie_port);

	/* Clear 'nof_p_hdr' & 'nof_np_hdr' to later know if they where changed by the user */
	pcie_port->ib_hcrd_config.nof_np_hdr = 0;
	pcie_port->ib_hcrd_config.nof_p_hdr = 0;

	pcie_port->num_retrains = 0;

	al_mod_dbg("pcie port handle initialized. port id: %d, rev_id %d, regs base %p\n",
	       port_id, pcie_port->rev_id, pcie_reg_base);
	return 0;
}

void al_mod_pcie_port_perf_params_print(
	struct al_mod_pcie_port *pcie_port)
{
	struct al_mod_pcie_ib_hcrd_os_ob_reads_config ib_hcrd_os_ob_reads_config;
	struct al_mod_pcie_link_status status;
	enum al_mod_pcie_max_payload_size mps;
	struct al_mod_pcie_regs *regs;
	uint32_t reg_val;
	int err;
	int i;

	al_mod_assert(pcie_port);

	regs = pcie_port->regs;

	if (!al_mod_pcie_port_is_enabled(pcie_port)) {
		al_mod_print("- Port is disabled\n");
		return;
	}
	err = al_mod_pcie_link_status(pcie_port, &status);
	if (err) {
		al_mod_print("- al_mod_pcie_link_status failed!\n");
		return;
	}
	if (!status.link_up) {
		al_mod_print("- Link is down\n");
		return;
	}
	al_mod_print("- Link is up (Gen %ux%u)\n", status.speed, status.lanes);

	reg_val = al_mod_reg_read32(regs->core_space[0].pcie_dev_ctrl_status);
	mps = AL_REG_FIELD_GET(reg_val, PCIE_PORT_DEV_CTRL_STATUS_MPS_MASK,
		PCIE_PORT_DEV_CTRL_STATUS_MPS_SHIFT);
	al_mod_print("- Max payload size = %s\n",
		(mps == AL_PCIE_MPS_128) ? "128" :
		(mps == AL_PCIE_MPS_256) ? "256" :
		"N/A");
	for (i = 0; i < AL_MAX_NUM_OF_PFS; i++) {
		unsigned int mrrs;

		reg_val = al_mod_reg_read32(regs->core_space[i].pcie_dev_ctrl_status);
		mrrs = reg_val & PCIE_PORT_DEV_CTRL_STATUS_MRRS_MASK;
		al_mod_print("- PF[%d] max read request size = %s\n", i,
			(mrrs == PCIE_PORT_DEV_CTRL_STATUS_MRRS_VAL_128) ? "128" :
			(mrrs == PCIE_PORT_DEV_CTRL_STATUS_MRRS_VAL_256) ? "256" :
			(mrrs == PCIE_PORT_DEV_CTRL_STATUS_MRRS_VAL_512) ? "512" :
			"N/A");
		if (pcie_port->rev_id < AL_PCIE_REV_ID_3)
			break;
	}

	reg_val = al_mod_reg_read32(regs->axi.init_fc.cfg);
	if (pcie_port->rev_id >= AL_PCIE_REV_ID_3) {
		ib_hcrd_os_ob_reads_config.nof_p_hdr = AL_REG_FIELD_GET(reg_val,
			PCIE_AXI_REV3_4_INIT_FC_CFG_NOF_P_HDR_MASK,
			PCIE_AXI_REV3_4_INIT_FC_CFG_NOF_P_HDR_SHIFT);
		ib_hcrd_os_ob_reads_config.nof_np_hdr = AL_REG_FIELD_GET(reg_val,
			PCIE_AXI_REV3_4_INIT_FC_CFG_NOF_NP_HDR_MASK,
			PCIE_AXI_REV3_4_INIT_FC_CFG_NOF_NP_HDR_SHIFT);
		ib_hcrd_os_ob_reads_config.nof_cpl_hdr = AL_REG_FIELD_GET(reg_val,
			PCIE_AXI_REV3_4_INIT_FC_CFG_NOF_CPL_HDR_MASK,
			PCIE_AXI_REV3_4_INIT_FC_CFG_NOF_CPL_HDR_SHIFT);
	} else {
		ib_hcrd_os_ob_reads_config.nof_p_hdr = AL_REG_FIELD_GET(reg_val,
			PCIE_AXI_REV1_2_INIT_FC_CFG_NOF_P_HDR_MASK,
			PCIE_AXI_REV1_2_INIT_FC_CFG_NOF_P_HDR_SHIFT);
		ib_hcrd_os_ob_reads_config.nof_np_hdr = AL_REG_FIELD_GET(reg_val,
			PCIE_AXI_REV1_2_INIT_FC_CFG_NOF_NP_HDR_MASK,
			PCIE_AXI_REV1_2_INIT_FC_CFG_NOF_NP_HDR_SHIFT);
		ib_hcrd_os_ob_reads_config.nof_cpl_hdr = AL_REG_FIELD_GET(reg_val,
			PCIE_AXI_REV1_2_INIT_FC_CFG_NOF_CPL_HDR_MASK,
			PCIE_AXI_REV1_2_INIT_FC_CFG_NOF_CPL_HDR_SHIFT);
	}

	reg_val = al_mod_reg_read32(regs->axi.pre_configuration.pcie_core_setup);
	ib_hcrd_os_ob_reads_config.nof_outstanding_ob_reads = AL_REG_FIELD_GET(reg_val,
		PCIE_AXI_CORE_SETUP_NOF_READS_ONSLAVE_INTRF_PCIE_CORE_MASK,
		PCIE_AXI_CORE_SETUP_NOF_READS_ONSLAVE_INTRF_PCIE_CORE_SHIFT);

	al_mod_print("- Max outstanding reads = %u\n",
		ib_hcrd_os_ob_reads_config.nof_outstanding_ob_reads);
	al_mod_print("- Num completions headers = %u\n", ib_hcrd_os_ob_reads_config.nof_cpl_hdr);
	al_mod_print("- Num posted headers = %u\n", ib_hcrd_os_ob_reads_config.nof_p_hdr);
	al_mod_print("- Num non posted headers = %u\n", ib_hcrd_os_ob_reads_config.nof_np_hdr);

	reg_val = al_mod_reg_read32(regs->axi.ordering.pos_cntl);
	al_mod_print("- cfg_ep_cmpl_after_wr_dis = %u\n",
		!!(reg_val & PCIE_AXI_POS_ORDER_EP_CMPL_AFTER_WR_DIS));
	al_mod_print("- cfg_bypass_cmpl_after_write_fix = %u\n",
		!!(reg_val & PCIE_AXI_POS_ORDER_BYPASS_CMPL_AFTER_WR_FIX));
}

int al_mod_pcie_port_clk_init(struct al_mod_pcie_port *pcie_port,
			  struct al_mod_pcie_clk_params *pcie_clk_params)
{
	if (pcie_port->rev_id < AL_PCIE_REV_ID_4) {
		al_mod_err("pcie port %u does not support clk init\n", pcie_port->port_id);
		return -EINVAL;
	}

	/* set gen2/gen1 pipe clocks and delay after reset */
	al_mod_reg_write32_masked(pcie_port->regs->axi.cfg_pclk_generate.cntr_low,
		PCIE_AXI_CFG_PCLK_GENERATE_CNTR_LOW_GEN2_MHZ_MASK |
		PCIE_AXI_CFG_PCLK_GENERATE_CNTR_LOW_GEN1_MHZ_MASK |
		PCIE_AXI_CFG_PCLK_GENERATE_CNTR_LOW_RST_DELAY_AFTER_CLK_EN_MASK,
		(pcie_clk_params->sb_clk_freq / AL_PCIE_CLK_GEN2_PIPE_HZ)
			<< PCIE_AXI_CFG_PCLK_GENERATE_CNTR_LOW_GEN2_MHZ_SHIFT |
		(pcie_clk_params->sb_clk_freq / AL_PCIE_CLK_GEN1_PIPE_HZ)
			<< PCIE_AXI_CFG_PCLK_GENERATE_CNTR_LOW_GEN1_MHZ_SHIFT |
		AL_PCIE_CLK_DELAY_AFTER_RESET_EN_HZ
			<< PCIE_AXI_CFG_PCLK_GENERATE_CNTR_LOW_RST_DELAY_AFTER_CLK_EN_SHIFT);

	/* set delay before/after rate-change event */
	al_mod_reg_write32_masked(pcie_port->regs->axi.cfg_pclk_generate.cntr_high,
		PCIE_AXI_CFG_PCLK_GENERATE_CNTR_HIGH_DELAY_BEFORE_RATE_CHANGE_MASK |
		PCIE_AXI_CFG_PCLK_GENERATE_CNTR_HIGH_DELAY_AFTER_RATE_CHANGE_MASK,
		AL_PCIE_CLK_DELAY_BEFORE_RATE_CHANGE_HZ
			<< PCIE_AXI_CFG_PCLK_GENERATE_CNTR_HIGH_DELAY_BEFORE_RATE_CHANGE_SHIFT |
		AL_PCIE_CLK_DELAY_AFTER_RATE_CHANGE_HZ
			<< PCIE_AXI_CFG_PCLK_GENERATE_CNTR_HIGH_DELAY_AFTER_RATE_CHANGE_SHIFT);

	/* 1 usec delay for the clock to bubble */
	al_mod_udelay(1);

	/* choose the correct clock for this pcie unit */
	al_mod_assert(pcie_clk_params->ref_clk <= AL_PCIE_CLK_REF_MAX);
	al_mod_reg_write32_masked(pcie_port->regs->axi.cfg_pclk_generate.cntr_low,
		PCIE_AXI_CFG_PCLK_GENERATE_CNTR_LOW_UNIT_PCLK_MASK,
		pcie_clk_params->ref_clk << PCIE_AXI_CFG_PCLK_GENERATE_CNTR_LOW_UNIT_PCLK_SHIFT);

	/* 1 usec delay for the clock to bubble */
	al_mod_udelay(1);

	/* enable main clock gate and gen2/gen1 dividers */
	al_mod_reg_write32_masked(pcie_port->regs->axi.cfg_pclk_generate.cntr_low,
		PCIE_AXI_CFG_PCLK_GENERATE_CNTR_LOW_MAIN_CLK_GATE_EN |
		PCIE_AXI_CFG_PCLK_GENERATE_CNTR_LOW_CLK_GEN2_DIV_EN |
		PCIE_AXI_CFG_PCLK_GENERATE_CNTR_LOW_CLK_GEN1_DIV_EN,
		PCIE_AXI_CFG_PCLK_GENERATE_CNTR_LOW_MAIN_CLK_GATE_EN |
		PCIE_AXI_CFG_PCLK_GENERATE_CNTR_LOW_CLK_GEN2_DIV_EN |
		PCIE_AXI_CFG_PCLK_GENERATE_CNTR_LOW_CLK_GEN1_DIV_EN);

	/* 1 usec delay for the clock to bubble */
	al_mod_udelay(1);

	/* reset internal clock logic */
	al_mod_reg_write32_masked(pcie_port->regs->axi.cfg_pclk_generate.cntr_low,
		PCIE_AXI_CFG_PCLK_GENERATE_CNTR_LOW_MAIN_INTERNAL_CLK_LOGIC_RST_EN,
		PCIE_AXI_CFG_PCLK_GENERATE_CNTR_LOW_MAIN_INTERNAL_CLK_LOGIC_RST_EN);

	/* 2 usec delay for the reset to take effect*/
	al_mod_udelay(2);

	return 0;
}

/**
 * Initializes a PCIe Physical function handle structure
 * Caution: this function should not read/write to any register except for
 * reading RO register (REV_ID for example)
 */
int al_mod_pcie_pf_handle_init(
	struct al_mod_pcie_pf *pcie_pf,
	struct al_mod_pcie_port *pcie_port,
	unsigned int pf_num)
{
	enum al_mod_pcie_operating_mode op_mode = al_mod_pcie_operating_mode_get(pcie_port);
	al_mod_assert(pf_num < pcie_port->max_num_of_pfs);

	if (op_mode != AL_PCIE_OPERATING_MODE_EP) {
		al_mod_err("PCIe %d: can't init PF handle with operating mode [%d]\n",
			pcie_port->port_id, op_mode);
		return -EINVAL;
	}

	pcie_pf->pf_num = pf_num;
	pcie_pf->pcie_port = pcie_port;

	al_mod_dbg("PCIe %d: pf handle initialized. pf number: %d, rev_id %d, regs %p\n",
	       pcie_port->port_id, pcie_pf->pf_num, pcie_port->rev_id,
	       pcie_port->regs);
	return 0;
}

/** Get port revision ID */
int al_mod_pcie_port_rev_id_get(struct al_mod_pcie_port *pcie_port)
{
	return pcie_port->rev_id;
}

/************************** Pre PCIe Port Enable API **************************/

/** configure pcie operating mode (root complex or endpoint) */
int al_mod_pcie_port_operating_mode_config(
	struct al_mod_pcie_port *pcie_port,
	enum al_mod_pcie_operating_mode mode)
{
	struct al_mod_pcie_regs *regs = pcie_port->regs;
	uint32_t reg, device_type, new_device_type;

	if (al_mod_pcie_port_is_enabled(pcie_port)) {
		al_mod_err("PCIe %d: already enabled, cannot set operating mode\n",
			pcie_port->port_id);
		return -EINVAL;
	}

	reg = al_mod_reg_read32(regs->axi.pcie_global.conf);

	device_type = AL_REG_FIELD_GET(reg,
			PCIE_AXI_MISC_PCIE_GLOBAL_CONF_DEV_TYPE_MASK,
			PCIE_AXI_MISC_PCIE_GLOBAL_CONF_DEV_TYPE_SHIFT);
	if (mode == AL_PCIE_OPERATING_MODE_EP) {
		new_device_type = PCIE_AXI_MISC_PCIE_GLOBAL_CONF_DEV_TYPE_EP;
	} else if (mode == AL_PCIE_OPERATING_MODE_RC) {
		new_device_type = PCIE_AXI_MISC_PCIE_GLOBAL_CONF_DEV_TYPE_RC;

		if (pcie_port->rev_id >= AL_PCIE_REV_ID_3) {
			/* config 1 PF in RC mode */
			al_mod_reg_write32_masked(regs->axi.axi_attr_ovrd.pf_sel,
				PCIE_AXI_AXI_ATTR_OVRD_PF_SEL_PF_BIT0_OVRD_FROM_AXUSER |
				PCIE_AXI_AXI_ATTR_OVRD_PF_SEL_PF_BIT0_OVRD_FROM_REG |
				PCIE_AXI_AXI_ATTR_OVRD_PF_SEL_PF_BIT0_ADDR_OFFSET_MASK |
				PCIE_AXI_AXI_ATTR_OVRD_PF_SEL_CFG_PF_BIT0_OVRD |
				PCIE_AXI_AXI_ATTR_OVRD_PF_SEL_PF_BIT1_OVRD_FROM_AXUSER |
				PCIE_AXI_AXI_ATTR_OVRD_PF_SEL_PF_BIT1_OVRD_FROM_REG |
				PCIE_AXI_AXI_ATTR_OVRD_PF_SEL_PF_BIT1_ADDR_OFFSET_MASK |
				PCIE_AXI_AXI_ATTR_OVRD_PF_SEL_CFG_PF_BIT1_OVRD,
				PCIE_AXI_AXI_ATTR_OVRD_PF_SEL_PF_BIT0_OVRD_FROM_REG |
				PCIE_AXI_AXI_ATTR_OVRD_PF_SEL_PF_BIT1_OVRD_FROM_REG);
		}
	} else {
		al_mod_err("PCIe %d: unknown operating mode: %d\n", pcie_port->port_id, mode);
		return -EINVAL;
	}

	if (new_device_type == device_type) {
		al_mod_dbg("PCIe %d: operating mode already set to %s\n",
		       pcie_port->port_id, (mode == AL_PCIE_OPERATING_MODE_EP) ?
		       "EndPoint" : "Root Complex");
		return 0;
	}
	al_mod_dbg("PCIe %d: set operating mode to %s\n",
		pcie_port->port_id, (mode == AL_PCIE_OPERATING_MODE_EP) ?
		"EndPoint" : "Root Complex");
	AL_REG_FIELD_SET(reg, PCIE_AXI_MISC_PCIE_GLOBAL_CONF_DEV_TYPE_MASK,
			 PCIE_AXI_MISC_PCIE_GLOBAL_CONF_DEV_TYPE_SHIFT,
			 new_device_type);

	al_mod_reg_write32(regs->axi.pcie_global.conf, reg);

	return 0;
}

int al_mod_pcie_port_max_lanes_set(struct al_mod_pcie_port *pcie_port, uint8_t lanes)
{
	struct al_mod_pcie_regs *regs = pcie_port->regs;
	uint32_t active_lanes_val;

	if (al_mod_pcie_port_is_enabled(pcie_port)) {
		al_mod_err("PCIe %d: already enabled, cannot set max lanes\n",
			pcie_port->port_id);
		return -EINVAL;
	}

	/* convert to bitmask format (4 ->'b1111, 2 ->'b11, 1 -> 'b1) */
	active_lanes_val = AL_PCIE_PARSE_LANES(lanes);

	al_mod_reg_write32_masked(regs->axi.pcie_global.conf,
		(pcie_port->rev_id >= AL_PCIE_REV_ID_3) ?
		PCIE_REV3_4_AXI_MISC_PCIE_GLOBAL_CONF_NOF_ACT_LANES_MASK :
		PCIE_REV1_2_AXI_MISC_PCIE_GLOBAL_CONF_NOF_ACT_LANES_MASK,
		active_lanes_val);

	pcie_port->max_lanes = lanes;
	return 0;
}

int al_mod_pcie_port_max_lanes_get(struct al_mod_pcie_port *pcie_port, uint8_t *lanes)
{
	uint32_t act_lanes;

	act_lanes = AL_REG_FIELD_GET(al_mod_reg_read32(pcie_port->regs->axi.pcie_global.conf),
		(pcie_port->rev_id >= AL_PCIE_REV_ID_3) ?
		PCIE_REV3_4_AXI_MISC_PCIE_GLOBAL_CONF_NOF_ACT_LANES_MASK :
		PCIE_REV1_2_AXI_MISC_PCIE_GLOBAL_CONF_NOF_ACT_LANES_MASK,
		PCIE_REVX_AXI_MISC_PCIE_GLOBAL_CONF_NOF_ACT_LANES_SHIFT);

	switch (act_lanes) {
	case 0x1:
		*lanes = 1;
		break;
	case 0x3:
		*lanes = 2;
		break;
	case 0xf:
		*lanes = 4;
		break;
	case 0xff:
		*lanes = 8;
		break;
	case 0xffff:
		*lanes = 16;
		break;
	default:
		*lanes = 0;
		al_mod_err("PCIe %d: invalid max lanes val (0x%x)\n", pcie_port->port_id, act_lanes);
		return -EINVAL;
	}

	return 0;
}

int al_mod_pcie_port_max_num_of_pfs_set(
	struct al_mod_pcie_port *pcie_port,
	uint8_t max_num_of_pfs)
{
	struct al_mod_pcie_regs *regs = pcie_port->regs;

	if (pcie_port->rev_id == AL_PCIE_REV_ID_5)
		al_mod_assert(max_num_of_pfs <= REV5_MAX_NUM_OF_PFS);
	else if (pcie_port->rev_id == AL_PCIE_REV_ID_4)
		al_mod_assert(max_num_of_pfs <= REV4_MAX_NUM_OF_PFS);
	else if (pcie_port->rev_id == AL_PCIE_REV_ID_3)
		al_mod_assert(max_num_of_pfs <= REV3_MAX_NUM_OF_PFS);
	else
		al_mod_assert(max_num_of_pfs == REV1_2_MAX_NUM_OF_PFS);

	pcie_port->max_num_of_pfs = max_num_of_pfs;

	if (al_mod_pcie_port_is_enabled(pcie_port) && (pcie_port->rev_id >= AL_PCIE_REV_ID_3)) {
		enum al_mod_pcie_operating_mode op_mode = al_mod_pcie_operating_mode_get(pcie_port);

		al_mod_bool is_multi_pf =
			((op_mode == AL_PCIE_OPERATING_MODE_EP) && (pcie_port->max_num_of_pfs > 1));

		/* Set maximum physical function numbers */
		al_mod_reg_write32_masked(
			&regs->port_regs->timer_ctrl_max_func_num,
			PCIE_PORT_GEN3_MAX_FUNC_NUM,
			pcie_port->max_num_of_pfs - 1);

		al_mod_pcie_port_wr_to_ro_set(pcie_port, AL_TRUE);

		/**
		 * in EP mode, when we have more than 1 PF we need to assert
		 * multi-pf support so the host scan all PFs
		 */
		al_mod_reg_write32_masked((uint32_t __iomem *)
			(&regs->core_space[0].config_header[0] +
			(PCIE_BIST_HEADER_TYPE_BASE >> 2)),
			PCIE_BIST_HEADER_TYPE_MULTI_FUNC_MASK,
			is_multi_pf ? PCIE_BIST_HEADER_TYPE_MULTI_FUNC_MASK : 0);

		al_mod_pcie_port_wr_to_ro_set(pcie_port, AL_FALSE);
	}

	return 0;
}

/* Inbound header credits and outstanding outbound reads configuration */
int al_mod_pcie_port_ib_hcrd_os_ob_reads_config(
	struct al_mod_pcie_port *pcie_port,
	struct al_mod_pcie_ib_hcrd_os_ob_reads_config *ib_hcrd_os_ob_reads_config)
{
	struct al_mod_pcie_regs *regs = pcie_port->regs;
	uint8_t	rev_id = pcie_port->rev_id;
	int subrev_id = pcie_port->subrev_id;
	unsigned int credit_sum;
	unsigned int total_hdr_limit;


	/* This version doesn't support dynamic credit allocation */
	if (ib_hcrd_os_ob_reads_hw_config[rev_id][subrev_id].ib_hcrd_supported == AL_FALSE) {
		al_mod_err("PCIe %d: this port doesn't support dynamic header allocation\n",
				pcie_port->port_id);
		return -EINVAL;
	}

	if (al_mod_pcie_port_is_enabled(pcie_port)) {
		al_mod_err("PCIe %d: already enabled, cannot configure IB credits and OB OS reads\n",
			pcie_port->port_id);
		return -EINVAL;
	}

	al_mod_assert(ib_hcrd_os_ob_reads_config->nof_np_hdr > 0);

	al_mod_assert(ib_hcrd_os_ob_reads_config->nof_p_hdr > 0);

	al_mod_assert(ib_hcrd_os_ob_reads_config->nof_cpl_hdr > 0);

	credit_sum = ib_hcrd_os_ob_reads_config->nof_cpl_hdr +
			ib_hcrd_os_ob_reads_config->nof_np_hdr +
			ib_hcrd_os_ob_reads_config->nof_p_hdr;

	total_hdr_limit = ib_hcrd_os_ob_reads_hw_config[rev_id][subrev_id].total_hdr_limit;

	al_mod_assert(credit_sum <= total_hdr_limit);


	if (pcie_port->rev_id >= AL_PCIE_REV_ID_4) {
		al_mod_reg_write32_masked(
			regs->axi.init_fc.cfg,
			PCIE_AXI_REV3_4_INIT_FC_CFG_NOF_P_HDR_MASK |
			PCIE_AXI_REV3_4_INIT_FC_CFG_NOF_NP_HDR_MASK |
			PCIE_AXI_REV3_4_INIT_FC_CFG_NOF_CPL_HDR_MASK,
			(ib_hcrd_os_ob_reads_config->nof_p_hdr <<
			 PCIE_AXI_REV3_4_INIT_FC_CFG_NOF_P_HDR_SHIFT) |
			(ib_hcrd_os_ob_reads_config->nof_np_hdr <<
			 PCIE_AXI_REV3_4_INIT_FC_CFG_NOF_NP_HDR_SHIFT) |
			(ib_hcrd_os_ob_reads_config->nof_cpl_hdr <<
			 PCIE_AXI_REV3_4_INIT_FC_CFG_NOF_CPL_HDR_SHIFT));
	} else if (pcie_port->rev_id == AL_PCIE_REV_ID_3) {
		al_mod_reg_write32_masked(
			regs->axi.init_fc.cfg,
			PCIE_AXI_REV3_4_INIT_FC_CFG_NOF_P_HDR_MASK |
			PCIE_AXI_REV3_4_INIT_FC_CFG_NOF_NP_HDR_MASK |
			PCIE_AXI_REV3_4_INIT_FC_CFG_NOF_CPL_HDR_MASK,
			(ib_hcrd_os_ob_reads_config->nof_p_hdr <<
			 PCIE_AXI_REV3_4_INIT_FC_CFG_NOF_P_HDR_SHIFT) |
			(ib_hcrd_os_ob_reads_config->nof_np_hdr <<
			 PCIE_AXI_REV3_4_INIT_FC_CFG_NOF_NP_HDR_SHIFT) |
			(ib_hcrd_os_ob_reads_config->nof_cpl_hdr <<
			 PCIE_AXI_REV3_4_INIT_FC_CFG_NOF_CPL_HDR_SHIFT));
	} else {
		al_mod_reg_write32_masked(
			regs->axi.init_fc.cfg,
			PCIE_AXI_REV1_2_INIT_FC_CFG_NOF_P_HDR_MASK |
			PCIE_AXI_REV1_2_INIT_FC_CFG_NOF_NP_HDR_MASK |
			PCIE_AXI_REV1_2_INIT_FC_CFG_NOF_CPL_HDR_MASK,
			(ib_hcrd_os_ob_reads_config->nof_p_hdr <<
			 PCIE_AXI_REV1_2_INIT_FC_CFG_NOF_P_HDR_SHIFT) |
			(ib_hcrd_os_ob_reads_config->nof_np_hdr <<
			 PCIE_AXI_REV1_2_INIT_FC_CFG_NOF_NP_HDR_SHIFT) |
			(ib_hcrd_os_ob_reads_config->nof_cpl_hdr <<
			 PCIE_AXI_REV1_2_INIT_FC_CFG_NOF_CPL_HDR_SHIFT));
	}

	if (ib_hcrd_os_ob_reads_hw_config[rev_id][subrev_id].ib_hcrd_use_hw_default == AL_FALSE) {
		al_mod_reg_write32_masked(
				regs->axi.pre_configuration.pcie_core_setup,
				PCIE_AXI_CORE_SETUP_NOF_READS_ONSLAVE_INTRF_PCIE_CORE_MASK,
				ib_hcrd_os_ob_reads_config->nof_outstanding_ob_reads <<
				PCIE_AXI_CORE_SETUP_NOF_READS_ONSLAVE_INTRF_PCIE_CORE_SHIFT);

		/* Store 'nof_p_hdr' and 'nof_np_hdr' to be set in the core later */
		pcie_port->ib_hcrd_config.nof_np_hdr =
			ib_hcrd_os_ob_reads_config->nof_np_hdr;
		pcie_port->ib_hcrd_config.nof_p_hdr =
			ib_hcrd_os_ob_reads_config->nof_p_hdr;
		pcie_port->ib_hcrd_config.crdt_update_required = 1;
	} else {
		/* Disable proprietary mechanism limiting number of outstanding outbound reads */
		al_mod_reg_write32_masked(
				regs->axi.pre_configuration.pcie_core_setup,
				PCIE_AXI_CORE_SETUP_NOF_READS_ONSLAVE_INTRF_PCIE_CORE_MASK,
				0);
	}

	return 0;
}

enum al_mod_pcie_operating_mode al_mod_pcie_operating_mode_get(
	struct al_mod_pcie_port *pcie_port)
{
	struct al_mod_pcie_regs *regs;
	uint32_t reg, device_type;

	al_mod_assert(pcie_port);
	regs = pcie_port->regs;

	reg = al_mod_reg_read32(regs->axi.pcie_global.conf);

	device_type = AL_REG_FIELD_GET(reg,
			PCIE_AXI_MISC_PCIE_GLOBAL_CONF_DEV_TYPE_MASK,
			PCIE_AXI_MISC_PCIE_GLOBAL_CONF_DEV_TYPE_SHIFT);

	switch (device_type) {
	case PCIE_AXI_MISC_PCIE_GLOBAL_CONF_DEV_TYPE_EP:
		return AL_PCIE_OPERATING_MODE_EP;
	case PCIE_AXI_MISC_PCIE_GLOBAL_CONF_DEV_TYPE_RC:
		return AL_PCIE_OPERATING_MODE_RC;
	default:
		al_mod_err("PCIe %d: unknown device type (%d) in global conf register.\n",
			pcie_port->port_id, device_type);
	}
	return AL_PCIE_OPERATING_MODE_UNKNOWN;
}

/* PCIe AXI quality of service configuration */
void al_mod_pcie_axi_qos_config(
	struct al_mod_pcie_port	*pcie_port,
	unsigned int		arqos,
	unsigned int		awqos)
{
	struct al_mod_pcie_regs *regs;

	al_mod_assert(pcie_port);
	regs = pcie_port->regs;
	al_mod_assert(arqos <= PCIE_AXI_CTRL_MASTER_ARCTL_ARQOS_VAL_MAX);
	al_mod_assert(awqos <= PCIE_AXI_CTRL_MASTER_AWCTL_AWQOS_VAL_MAX);

	al_mod_reg_write32_masked(
		regs->axi.ctrl.master_arctl,
		PCIE_AXI_CTRL_MASTER_ARCTL_ARQOS_MASK,
		arqos << PCIE_AXI_CTRL_MASTER_ARCTL_ARQOS_SHIFT);
	al_mod_reg_write32_masked(
		regs->axi.ctrl.master_awctl,
		PCIE_AXI_CTRL_MASTER_AWCTL_AWQOS_MASK,
		awqos << PCIE_AXI_CTRL_MASTER_AWCTL_AWQOS_SHIFT);
}

/**************************** PCIe Port Enable API ****************************/

/** Enable PCIe port (deassert reset) */
int al_mod_pcie_port_enable(struct al_mod_pcie_port *pcie_port)
{
	struct al_mod_pbs_regs *pbs_reg_base =
				(struct al_mod_pbs_regs *)pcie_port->pbs_regs;
	struct al_mod_pcie_regs *regs = pcie_port->regs;
	unsigned int port_id = pcie_port->port_id;
	uint8_t	rev_id = pcie_port->rev_id;
	int subrev_id = pcie_port->subrev_id;

	/* pre-port-enable default functionality should be here */

	/**
	 * Set inbound header credit and outstanding outbound reads defaults
	 * if the port initiator doesn't set it & port support it
	 * Must be called before port enable (PCIE_EXIST)
	 */
	if (((pcie_port->ib_hcrd_config.nof_np_hdr == 0) ||
	     (pcie_port->ib_hcrd_config.nof_p_hdr == 0)) &&
	    (ib_hcrd_os_ob_reads_hw_config[rev_id][subrev_id].ib_hcrd_supported == AL_TRUE))
		al_mod_pcie_ib_hcrd_os_ob_reads_config_default(pcie_port);

	if (rev_id == AL_PCIE_REV_ID_4) {
		if (subrev_id == PCIE_AXI_DEVICE_ID_REG_REV_ID_X16) {
			/*
			 * Addressing RMN: 11478
			 *
			 * RMN description:
			 * Completion header buffer size, apply to X16 only.
			 * This port has place for 400 headers.
			 * due to fact each read request can be split to up to 5 parts
			 * (how many parts depend of max_read_size / RCB) there is a need
			 * to set the proper max value.
			 *
			 * Software flow:
			 * set limit NOF_OUT_RD = 'd80
			 * The max setting can be slightly different,
			 * depends on system setup (max_read_size / RCB), we took the worse case
			 */
			al_mod_reg_write32_masked(
				regs->axi.pre_configuration.pcie_core_setup,
				PCIE_AXI_CORE_SETUP_NOF_READS_ONSLAVE_INTRF_PCIE_CORE_MASK,
				80 << PCIE_AXI_CORE_SETUP_NOF_READS_ONSLAVE_INTRF_PCIE_CORE_SHIFT);
		}

		if ((subrev_id == PCIE_AXI_DEVICE_ID_REG_REV_ID_X16) ||
		    (subrev_id == PCIE_AXI_DEVICE_ID_REG_REV_ID_X8)) {
			/*
			 * Addressing RMN: 11478
			 *
			 * RMN description:
			 * Completion DATA buffer size apply to X16 and X8.
			 * This ports have a smaller completion DATA buffer size than required
			 * to accommodate all the outstanding reads data.
			 *
			 * Software flow:
			 * set the RD_LIMITER in value that will prevent it from accepting new read
			 * request when there is not sufficient place
			 */
			al_mod_reg_write32(regs->axi.cfg_outbound_rd_len.len_shaper,
				       (subrev_id == PCIE_AXI_DEVICE_ID_REG_REV_ID_X16) ?
					640 : 320);
		}
	}

	/*
	 * Disable ATS capability
	 * - must be done before core reset deasserted
	 * - rev_id 0 - no effect, but no harm
	 */
	if ((pcie_port->rev_id == AL_PCIE_REV_ID_1) ||
		(pcie_port->rev_id == AL_PCIE_REV_ID_2)) {
		al_mod_reg_write32_masked(
			regs->axi.ordering.pos_cntl,
			PCIE_AXI_CORE_SETUP_ATS_CAP_DIS,
			PCIE_AXI_CORE_SETUP_ATS_CAP_DIS);
	}

	/* Deassert core reset */
	al_mod_reg_write32_masked(
		&pbs_reg_base->unit.pcie_conf_1,
		1 << (port_id + PBS_UNIT_PCIE_CONF_1_PCIE_EXIST_SHIFT),
		1 << (port_id + PBS_UNIT_PCIE_CONF_1_PCIE_EXIST_SHIFT));

	return 0;
}

/** Disable PCIe port (assert reset) */
void al_mod_pcie_port_disable(struct al_mod_pcie_port *pcie_port)
{
	struct al_mod_pbs_regs *pbs_reg_base =
				(struct al_mod_pbs_regs *)pcie_port->pbs_regs;
	unsigned int port_id = pcie_port->port_id;

	if (!al_mod_pcie_port_is_enabled(pcie_port)) {
		al_mod_warn("PCIe %d: trying to disable a non-enabled port\n",
			pcie_port->port_id);
	}

	/* Unset values given in port_enable */
	pcie_port->ib_hcrd_config.nof_np_hdr = 0;
	pcie_port->ib_hcrd_config.nof_p_hdr = 0;
	pcie_port->ib_hcrd_config.crdt_update_required = 0;

	/* Assert core reset */
	al_mod_reg_write32_masked(
		&pbs_reg_base->unit.pcie_conf_1,
		1 << (port_id + PBS_UNIT_PCIE_CONF_1_PCIE_EXIST_SHIFT),
		0);
}

int al_mod_pcie_port_memory_shutdown_set(
	struct al_mod_pcie_port	*pcie_port,
	al_mod_bool			enable)
{
	struct al_mod_pcie_regs *regs = pcie_port->regs;
	uint32_t mask = (pcie_port->rev_id >= AL_PCIE_REV_ID_3) ?
		PCIE_REV3_4_AXI_MISC_PCIE_GLOBAL_CONF_MEM_SHUTDOWN :
		PCIE_REV1_2_AXI_MISC_PCIE_GLOBAL_CONF_MEM_SHUTDOWN;

	if (!al_mod_pcie_port_is_enabled(pcie_port)) {
		al_mod_err("PCIe %d: not enabled, cannot shutdown memory\n",
			pcie_port->port_id);
		return -EINVAL;
	}

	al_mod_reg_write32_masked(regs->axi.pcie_global.conf,
		mask, enable == AL_TRUE ? mask : 0);

	return 0;
}

al_mod_bool al_mod_pcie_port_memory_is_shutdown(struct al_mod_pcie_port *pcie_port)
{
	struct al_mod_pcie_regs *regs = pcie_port->regs;
	uint32_t mask;

	al_mod_assert(pcie_port);

	if (!al_mod_pcie_port_is_enabled(pcie_port))
		return AL_TRUE;

	switch (pcie_port->rev_id) {
	case AL_PCIE_REV_ID_5:
	case AL_PCIE_REV_ID_4:
	case AL_PCIE_REV_ID_3:
		mask = PCIE_REV3_4_AXI_MISC_PCIE_GLOBAL_CONF_MEM_SHUTDOWN;
		break;
	case AL_PCIE_REV_ID_2:
	case AL_PCIE_REV_ID_1:
		mask = PCIE_REV1_2_AXI_MISC_PCIE_GLOBAL_CONF_MEM_SHUTDOWN;
		break;
	default:
		al_mod_err("%s: unsupported PCIe revision ID %d\n", __func__, pcie_port->rev_id);
		al_mod_assert(0);
		return AL_FALSE;
	}

	if (al_mod_reg_read32(regs->axi.pcie_global.conf) & mask)
		return AL_TRUE;

	return AL_FALSE;
}

al_mod_bool al_mod_pcie_port_is_enabled_raw(
	void __iomem	*pbs_reg_base,
	unsigned int	port_id)
{
	struct al_mod_pbs_regs *pbs_regs = (struct al_mod_pbs_regs *)pbs_reg_base;
	uint32_t pcie_exist = al_mod_reg_read32(&pbs_regs->unit.pcie_conf_1);

	uint32_t ports_enabled = AL_REG_FIELD_GET(pcie_exist,
		PBS_UNIT_PCIE_CONF_1_PCIE_EXIST_MASK,
		PBS_UNIT_PCIE_CONF_1_PCIE_EXIST_SHIFT);

	return (AL_REG_FIELD_GET(ports_enabled, AL_BIT(port_id), port_id) == 1);
}

al_mod_bool al_mod_pcie_port_is_enabled(struct al_mod_pcie_port *pcie_port)
{
	struct al_mod_pbs_regs *pbs_reg_base = (struct al_mod_pbs_regs *)pcie_port->pbs_regs;

	return al_mod_pcie_port_is_enabled_raw(pbs_reg_base, pcie_port->port_id);
}

/*************************** PCIe Configuration API ***************************/

/** configure pcie port (link params, etc..) */
int al_mod_pcie_port_config(struct al_mod_pcie_port *pcie_port,
			const struct al_mod_pcie_port_config_params *params)
{
	struct al_mod_pcie_port_config_live_params live_params;
	struct al_mod_pcie_regs *regs;
	enum al_mod_pcie_operating_mode op_mode;
	int status = 0;
	int i;
	unsigned int pf;

	al_mod_assert(pcie_port);
	al_mod_assert(params);

	regs = pcie_port->regs;

	if (!al_mod_pcie_port_is_enabled(pcie_port)) {
		al_mod_err("PCIe %d: port not enabled, cannot configure port\n",
			pcie_port->port_id);
		return -EINVAL;
	}

	if (al_mod_pcie_is_link_started(pcie_port)) {
		al_mod_err("PCIe %d: link already started, cannot configure port\n",
			pcie_port->port_id);
		return -EINVAL;
	}

	al_mod_dbg("PCIe %d: port config\n", pcie_port->port_id);

	op_mode = al_mod_pcie_operating_mode_get(pcie_port);

	/* Addressing RMN: 10181
	 *
	 * RMN description:
	 * BAR4/5 Registers Not Properly Disabled in RC mode
	 * Software flow:
	 * Disable BAR's 4/5 in HW when in RC mode
	 */
	if (op_mode == AL_PCIE_OPERATING_MODE_RC) {
		for (i = 0; i < pcie_port->max_num_of_pfs; i++) {
			uint32_t __iomem *bar_addr;

			bar_addr =
				&regs->core_space[i].config_header[(AL_PCI_BASE_ADDRESS_4 >> 2)];
			al_mod_reg_write32_dbi_cs2(pcie_port, bar_addr, 0);

			bar_addr =
				&regs->core_space[i].config_header[(AL_PCI_BASE_ADDRESS_4_HI >> 2)];
			al_mod_reg_write32_dbi_cs2(pcie_port, bar_addr, 0);
		}
	}

	/* if max lanes not specifies, read it from register */
	if (pcie_port->max_lanes == 0) {
		status = al_mod_pcie_port_max_lanes_get(pcie_port, &pcie_port->max_lanes);
		if (status) {
			pcie_port->max_lanes = 0;
			al_mod_err("PCIe %d: invalid max lanes val\n", pcie_port->port_id);
		}
	}

	if (params->link_params)
		status = al_mod_pcie_port_link_config(pcie_port, params->link_params);
	if (status)
		goto done;

	status = al_mod_pcie_port_snoop_config(pcie_port, params->enable_axi_snoop);
	if (status)
		goto done;

	al_mod_pcie_port_max_num_of_pfs_set(pcie_port, pcie_port->max_num_of_pfs);

	al_mod_pcie_port_ram_parity_int_config(pcie_port, params->enable_ram_parity_int);

	al_mod_pcie_port_axi_parity_int_config(pcie_port, params->enable_axi_parity_int);

	al_mod_pcie_port_relaxed_pcie_ordering_config(pcie_port, params->relaxed_ordering_params);

	if (params->lat_rply_timers)
		status = al_mod_pcie_port_lat_rply_timers_config(pcie_port, params->lat_rply_timers);
	if (status)
		goto done;

	if (params->gen2_params)
		status = al_mod_pcie_port_gen2_params_config(pcie_port, params->gen2_params);
	if (status)
		goto done;

	if (params->gen3_params)
		status = al_mod_pcie_port_gen3_params_config(pcie_port, op_mode, params->gen3_params);
	if (status)
		goto done;

	if (params->sris_params)
		status = al_mod_pcie_port_sris_config(pcie_port, params->sris_params,
						params->link_params->max_speed);
	if (status)
		goto done;

	if (pcie_port->ib_hcrd_config.crdt_update_required) {
		al_mod_pcie_port_ib_hcrd_config(pcie_port);
	}

	if (params->fast_link_mode) {
		if (pcie_port->rev_id >= AL_PCIE_REV_ID_4)
			al_mod_reg_write32_masked(
				regs->app.cfg_blockalign.cfg_ext_cntl_1,
				PCIE_W_CFG_EXT_CNTL_1_FAST_MODE_CNT_SLOWDOWN_FACTOR_MASK,
				PCIE_W_CFG_EXT_CNTL_1_FAST_MODE_CNT_SLOWDOWN_FACTOR_VAL_FAST <<
				PCIE_W_CFG_EXT_CNTL_1_FAST_MODE_CNT_SLOWDOWN_FACTOR_SHIFT);

		al_mod_reg_write32_masked(&regs->port_regs->port_link_ctrl,
			      1 << PCIE_PORT_LINK_CTRL_FAST_LINK_EN_SHIFT,
			      1 << PCIE_PORT_LINK_CTRL_FAST_LINK_EN_SHIFT);
	}

	if (params->enable_axi_slave_err_resp)
		al_mod_reg_write32_masked(&regs->port_regs->axi_slave_err_resp,
				1 << PCIE_PORT_AXI_SLAVE_ERR_RESP_ALL_MAPPING_SHIFT,
				1 << PCIE_PORT_AXI_SLAVE_ERR_RESP_ALL_MAPPING_SHIFT);

	/**
	 * Addressing RMN: 5477
	 *
	 * RMN description:
	 * address-decoder logic performs sub-target decoding even for transactions
	 * which undergo target enforcement. thus, in case transaction's address is
	 * inside any ECAM bar, the sub-target decoding will be set to ECAM, which
	 * causes wrong handling by PCIe unit
	 *
	 * Software flow:
	 * on EP mode only, turning on the iATU-enable bit (with the relevant mask
	 * below) allows the PCIe unit to discard the ECAM bit which was asserted
	 * by-mistake in the address-decoder
	 */
	if (op_mode == AL_PCIE_OPERATING_MODE_EP) {
		al_mod_reg_write32_masked(regs->axi.ob_ctrl.cfg_target_bus,
			PCIE_AXI_MISC_OB_CTRL_CFG_TARGET_BUS_MASK_MASK,
			(0) << PCIE_AXI_MISC_OB_CTRL_CFG_TARGET_BUS_MASK_SHIFT);
		al_mod_reg_write32_masked(regs->axi.ob_ctrl.cfg_control,
			PCIE_AXI_MISC_OB_CTRL_CFG_CONTROL_IATU_EN,
			PCIE_AXI_MISC_OB_CTRL_CFG_CONTROL_IATU_EN);
	}

	if (op_mode == AL_PCIE_OPERATING_MODE_RC) {
		/**
		 * enable memory and I/O access from port when in RC mode
		 * in RC mode, only core_space[0] is valid.
		 */
		al_mod_reg_write16_masked(
			(uint16_t __iomem *)(&regs->core_space[0].config_header[0] + (0x4 >> 2)),
			0x7, /* Mem, MSE, IO */
			0x7);

		/* change the class code to match pci bridge */
		al_mod_pcie_port_wr_to_ro_set(pcie_port, AL_TRUE);

		al_mod_reg_write32_masked(
			(uint32_t __iomem *)(&regs->core_space[0].config_header[0]
			+ (PCI_CLASS_REVISION >> 2)),
			0xFFFFFF00,
			0x06040000);

		al_mod_pcie_port_wr_to_ro_set(pcie_port, AL_FALSE);

		/**
		 * Addressing RMN: 5702
		 *
		 * RMN description:
		 * target bus mask default value in HW is: 0xFE, this enforces
		 * setting the target bus for ports 1 and 3 when running on RC
		 * mode since bit[20] in ECAM address in these cases is set
		 *
		 * Software flow:
		 * on RC mode only, set target-bus value to 0xFF to prevent this
		 * enforcement
		 */
		al_mod_reg_write32_masked(regs->axi.ob_ctrl.cfg_target_bus,
			PCIE_AXI_MISC_OB_CTRL_CFG_TARGET_BUS_MASK_MASK,
			PCIE_AXI_MISC_OB_CTRL_CFG_TARGET_BUS_MASK_MASK);
	}

	if (pcie_port->rev_id >= AL_PCIE_REV_ID_3) {
		/* Disable referring to PM state of non-enable functions */
		for (i = pcie_port->max_num_of_pfs; i < AL_MAX_NUM_OF_PFS; i++) {
			al_mod_reg_write32_masked(regs->app.pm_state_per_func[i].pm_state_per_func,
			PCIE_W_PM_STATE_PER_FUNC_PM_STATE_PER_FUNC_ASPM_PF_ENABLE_MAX_FUNC_NUMBER |
			PCIE_W_PM_STATE_PER_FUNC_PM_STATE_PER_FUNC_DSATE_PF_ENABLE_MAX_FUNC_NUMBER,
			0);
		}
	}

	/* disable resetting the EIEOS count which may cause equalization to
	 * take longer than needed
	 */
	al_mod_reg_write32_masked(&regs->port_regs->gen3_ctrl,
			      AL_BIT(PCIE_PORT_GEN3_CTRL_EQ_EIEOS_COUNT_RESET_DISABLE_SHIFT),
			      AL_BIT(PCIE_PORT_GEN3_CTRL_EQ_EIEOS_COUNT_RESET_DISABLE_SHIFT));

	al_mod_pcie_port_wr_to_ro_set(pcie_port, AL_TRUE);

	for (pf = 0; pf < pcie_port->max_num_of_pfs; pf++) {
		al_mod_reg_write32_masked(
			regs->core_space[pf].pcie_link_cap_base,
			AL_PCI_EXP_LNKCAP_MLW,
			pcie_port->max_lanes << AL_PCI_EXP_LNKCAP_MLW_SHIFT);

		/*
		 * in EP mode we dont want to expose that we support TPH as completer
		 */
		if (op_mode == AL_PCIE_OPERATING_MODE_EP)
			al_mod_reg_write32_masked(
				regs->core_space[pf].pcie_cap_base + (AL_PCI_EXP_DEVCAP2 >> 2),
				AL_PCI_EXP_DEVCAP2_TCS_MASK,
				0);
	}

	al_mod_pcie_port_wr_to_ro_set(pcie_port, AL_TRUE);

	/* Default live configuration */
	if (sizeof(live_params))
		al_mod_memset(&live_params, 0, sizeof(live_params));

	al_mod_pcie_port_config_live(pcie_port, &live_params);

done:


	al_mod_dbg("PCIe %d: port config %s\n", pcie_port->port_id, status? "failed": "done");

	return status;
}

void al_mod_pcie_port_config_live(
	struct al_mod_pcie_port				*pcie_port,
	const struct al_mod_pcie_port_config_live_params	*params)
{
	struct al_mod_pcie_regs *regs;

	al_mod_assert(pcie_port);
	al_mod_assert(al_mod_pcie_port_is_enabled(pcie_port));
	al_mod_assert(params);

	regs = pcie_port->regs;

	/* disable the mechanism that allows direct message generation by CPU or DMA address
	   matching Msg region defined by Msg_Start and Msg_Limit region (this mechanism
	 * is currently not in use)
	 */
	al_mod_reg_write32_masked(regs->axi.ctrl.slv_wctl,
		PCIE_AXI_CTRL_SLV_WCTRL_MSG_GENERATE_MASK,
		PCIE_AXI_CTRL_SLV_WCTRL_MSG_GENERATE_MASK);

	/**
	 * Addressing RMN: 12107
	 *
	 * RMN description:
	 * When setting in-bound ATU to return bad response, such as UR,
	 * error indication might be asserted for in-bound request not hitting this ATU.
	 *
	 * Software flow:
	 * set bit rsrvd1 in pre_configuration_pcie_core_setup register.
	 * This will add the missing qualifier : cfg_qualified_atu_error_with_flt_q_hv.
	 */
	if (pcie_port->rev_id > AL_PCIE_REV_ID_1)
		al_mod_reg_write32_masked(regs->axi.pre_configuration.pcie_core_setup,
				      PCIE_AXI_CFG_QUALIFIED_ATU_ERROR_WITH_FLT_Q_HV,
				      PCIE_AXI_CFG_QUALIFIED_ATU_ERROR_WITH_FLT_Q_HV);
}

int al_mod_pcie_pf_config(
	struct al_mod_pcie_pf *pcie_pf,
	const struct al_mod_pcie_pf_config_params *params)
{
	struct al_mod_pcie_port *pcie_port;
	int status = 0;

	al_mod_assert(pcie_pf);
	al_mod_assert(params);

	pcie_port = pcie_pf->pcie_port;

	if (!al_mod_pcie_port_is_enabled(pcie_port)) {
		al_mod_err("PCIe %d: port not enabled, cannot configure port\n", pcie_port->port_id);
		return -EINVAL;
	}

	al_mod_dbg("PCIe %d: pf %d config\n", pcie_port->port_id, pcie_pf->pf_num);

	if (params)
		status = al_mod_pcie_port_pf_params_config(pcie_pf, params);
	if (status)
		goto done;

done:
	al_mod_dbg("PCIe %d: pf %d config %s\n",
		pcie_port->port_id, pcie_pf->pf_num, status ? "failed" : "done");

	return status;
}

/*************************** PCIe PM Related API ***************************/

/* Configure ASPM capabilities of the port */
void al_mod_pcie_pm_aspm_cfg_set(
	struct al_mod_pcie_port	*pcie_port,
	unsigned int		aspm_cfg)
{
	struct al_mod_pcie_regs *regs;
	enum al_mod_pcie_operating_mode op_mode;

	al_mod_assert(!(aspm_cfg &
		~(AL_PCIE_PM_ASPM_CFG_L0S |
		AL_PCIE_PM_ASPM_CFG_L1)));
	al_mod_assert(pcie_port);
	al_mod_assert(al_mod_pcie_port_is_enabled(pcie_port));

	regs = pcie_port->regs;
	op_mode = al_mod_pcie_operating_mode_get(pcie_port);

	al_mod_reg_write32_masked(regs->core_space[0].pcie_link_cap_base,
		AL_PCI_EXP_LNKCAP_ASPMS,
		aspm_cfg << AL_PCI_EXP_LNKCAP_ASPMS_SHIFT);

	/* Enable entrance to L1 without receiver in L0s, if L0s not supported */
	if (aspm_cfg == AL_PCIE_PM_ASPM_CFG_L1)
		al_mod_reg_write32_masked(&regs->port_regs->ack_f_aspm_ctrl,
			PCIE_PORT_ACK_F_ASPM_CTRL_ENTER_ASPM,
			PCIE_PORT_ACK_F_ASPM_CTRL_ENTER_ASPM);

	if (op_mode == AL_PCIE_OPERATING_MODE_RC)
		al_mod_reg_write32_masked(regs->core_space[0].pcie_link_ctrl_status,
			AL_PCI_EXP_LNKCTL_ASPMC,
			aspm_cfg);
}

/* Transition the link to or from L2 state */
void al_mod_pcie_pm_l2_trigger(
	struct al_mod_pcie_port *pcie_port,
	al_mod_bool enable)
{
	enum al_mod_pcie_operating_mode op_mode;
	struct al_mod_pcie_regs *regs;

	al_mod_assert(pcie_port);
	al_mod_assert(al_mod_pcie_port_is_enabled(pcie_port));

	regs = pcie_port->regs;
	op_mode = al_mod_pcie_operating_mode_get(pcie_port);

	al_mod_assert(((op_mode == AL_PCIE_OPERATING_MODE_RC) && enable) ||
		((op_mode == AL_PCIE_OPERATING_MODE_EP) && (!enable)));

	if (enable) {
		unsigned int mask = (pcie_port->rev_id >= AL_PCIE_REV_ID_3) ?
			PCIE_W_REV3_4_GLOBAL_CTRL_PM_CONTROL_PM_XMT_TURNOFF :
			PCIE_W_REV1_2_GLOBAL_CTRL_PM_CONTROL_PM_XMT_TURNOFF;
		/*
		 * In order to create the TurnOff MSG TLP,
		 * the msg_arbitration module requires an edge on the relevant bit.
		 * Thus, we need to make sure it is 1'b0 before asserting it to 1'b1
		 */
		al_mod_reg_write32_masked(regs->app.global_ctrl.pm_control, mask, 0);
		al_mod_reg_write32_masked(regs->app.global_ctrl.pm_control, mask, mask);
	} else {
		unsigned int mask = (pcie_port->rev_id >= AL_PCIE_REV_ID_3) ?
			PCIE_W_REV3_4_GLOBAL_CTRL_PM_CONTROL_PM_XMT_PME_FUNC_MASK :
			PCIE_W_REV1_2_GLOBAL_CTRL_PM_CONTROL_PM_XMT_PME;
		/*
		 * In order to create the PM_PME MSG TLP,
		 * the msg_arbitration module requires an edge on the all relevant bits.
		 * Thus, we need to make sure each one of them is 1'b0
		 * before asserting it to 1'b1
		 */
		al_mod_reg_write32_masked(regs->app.global_ctrl.pm_control, mask, 0);
		al_mod_reg_write32_masked(regs->app.global_ctrl.pm_control, mask, mask);
	}
}

/************************** PCIe Link Operations API **************************/

/* start pcie link */
int al_mod_pcie_link_start(struct al_mod_pcie_port *pcie_port)
{
	struct al_mod_pcie_regs *regs = (struct al_mod_pcie_regs *)pcie_port->regs;

	if (!al_mod_pcie_port_is_enabled(pcie_port)) {
		al_mod_err("PCIe %d: port not enabled, cannot start link\n",
			pcie_port->port_id);
		return -EINVAL;
	}

	al_mod_dbg("PCIe_%d: start port link.\n", pcie_port->port_id);

	al_mod_reg_write32_masked(
			regs->app.global_ctrl.port_init,
			PCIE_W_GLOBAL_CTRL_PORT_INIT_APP_LTSSM_EN_MASK,
			PCIE_W_GLOBAL_CTRL_PORT_INIT_APP_LTSSM_EN_MASK);

	return 0;
}

/* stop pcie link */
int al_mod_pcie_link_stop(struct al_mod_pcie_port *pcie_port)
{
	struct al_mod_pcie_regs *regs = (struct al_mod_pcie_regs *)pcie_port->regs;

	if (!al_mod_pcie_is_link_started(pcie_port)) {
		al_mod_warn("PCIe %d: trying to stop a non-started link\n",
			pcie_port->port_id);
	}

	al_mod_dbg("PCIe_%d: stop port link.\n", pcie_port->port_id);

	al_mod_reg_write32_masked(
			regs->app.global_ctrl.port_init,
			PCIE_W_GLOBAL_CTRL_PORT_INIT_APP_LTSSM_EN_MASK,
			~PCIE_W_GLOBAL_CTRL_PORT_INIT_APP_LTSSM_EN_MASK);

	return 0;
}

/** return AL_TRUE is link started (LTSSM enabled) and AL_FALSE otherwise */
al_mod_bool al_mod_pcie_is_link_started(struct al_mod_pcie_port *pcie_port)
{
	uint32_t port_init;
	uint8_t ltssm_en;
	struct al_mod_pcie_regs *regs = (struct al_mod_pcie_regs *)pcie_port->regs;

	if (!al_mod_pcie_port_is_enabled(pcie_port))
		return AL_FALSE;

	port_init = al_mod_reg_read32(regs->app.global_ctrl.port_init);
	ltssm_en = AL_REG_FIELD_GET(port_init,
		PCIE_W_GLOBAL_CTRL_PORT_INIT_APP_LTSSM_EN_MASK,
		PCIE_W_GLOBAL_CTRL_PORT_INIT_APP_LTSSM_EN_SHIFT);

	return ltssm_en;
}

static int _al_pcie_link_up_wait_retrain_if_not_full_width(
	struct al_mod_pcie_port	*pcie_port,
	int			timeout_usec,
	unsigned int		*time_remaining_usec,
	unsigned int		*retrain_cnt)
{
	unsigned int count = 0;
	int state_counted = -1;
	unsigned int retrain_count = 0;

	struct state_cnt {
		unsigned int ltssm_state;
		unsigned int timeout;
	};

	static const struct state_cnt state_cnt[] = {
		{
			.ltssm_state = AL_PCIE_LTSSM_STATE_POLL_ACTIVE,
			.timeout = 50,
		},
		{
			.ltssm_state = AL_PCIE_LTSSM_STATE_CFG_LINKWD_START,
			.timeout = 5,
		},
		{
			.ltssm_state = AL_PCIE_LTSSM_STATE_CFG_LINKWD_ACEPT,
			.timeout = 5,
		},
		{
			.ltssm_state = AL_PCIE_LTSSM_STATE_CFG_LANENUM_WAIT,
			.timeout = 5,
		},
		{
			.ltssm_state = AL_PCIE_LTSSM_STATE_CFG_LANENUM_ACEPT,
			.timeout = 5,
		},
	};
	static const unsigned int num_state_types = AL_ARR_SIZE(state_cnt);

	do {
		al_mod_bool retrain_required = AL_FALSE;
		uint8_t ltssm_state;

		al_mod_pcie_check_link(pcie_port, &ltssm_state);

		if (ltssm_state >= AL_PCIE_LTSSM_STATE_L0) {
				break;
		} else {
			unsigned int i;

			for (i = 0; i < num_state_types; i++) {
				if (ltssm_state == state_cnt[i].ltssm_state) {
					if (state_counted != ltssm_state)
						count = 1;
					else
						count++;

					state_counted = ltssm_state;

					if (count == state_cnt[i].timeout)
						retrain_required = AL_TRUE;
					break;
				}
			}

			if (i == num_state_types)
				count = 0;
		}

		if (retrain_required) {
			al_mod_pcie_link_stop(pcie_port);
			al_mod_udelay(1000);
			timeout_usec -= 1000;
			retrain_count++;
			al_mod_pcie_link_start(pcie_port);
		}

		al_mod_udelay(100);
		timeout_usec -= 100;
	} while (timeout_usec > 0);

	*retrain_cnt = retrain_count;

	if (timeout_usec <= 0)
		return -ETIME;

	*time_remaining_usec = (unsigned int)timeout_usec;

	return 0;
}

static int _al_pcie_link_up_wait(struct al_mod_pcie_port *pcie_port, uint32_t timeout_ms,
	al_mod_bool retrain_if_not_full_width)
{
	unsigned int retrain_cnt = 0;
	int wait_count;

	if (retrain_if_not_full_width) {
		unsigned int time_remaining_usec;
		int err;

		err = _al_pcie_link_up_wait_retrain_if_not_full_width(
			pcie_port, timeout_ms * 1000, &time_remaining_usec, &retrain_cnt);
		if (err) {
			al_mod_err("PCIE_%d: link retrain timeout (%u retrains)\n",
				pcie_port->port_id, retrain_cnt);
			return err;
		}

		al_mod_print("PCIE_%d: link retrain ok (%u retrains, %u usec remaining)\n",
			pcie_port->port_id, retrain_cnt, time_remaining_usec);

		timeout_ms = time_remaining_usec / 1000;
	}

	pcie_port->num_retrains = retrain_cnt;

	wait_count = timeout_ms * AL_PCIE_LINKUP_WAIT_INTERVALS_PER_SEC;

	while (wait_count-- > 0)	{
		if (al_mod_pcie_check_link(pcie_port, NULL)) {
			al_mod_dbg("PCIe_%d: <<<<<<<<< Link up >>>>>>>>>\n", pcie_port->port_id);
			return 0;
		} else
			al_mod_dbg("PCIe_%d: No link up, %d attempts remaining\n",
				pcie_port->port_id, wait_count);

		al_mod_udelay(AL_PCIE_LINKUP_WAIT_INTERVAL);
	}
	al_mod_dbg("PCIE_%d: link is not established in time\n",
				pcie_port->port_id);

	return -ETIME;
}

int al_mod_pcie_link_up_wait(struct al_mod_pcie_port *pcie_port, uint32_t timeout_ms)
{
	return _al_pcie_link_up_wait(pcie_port, timeout_ms, AL_FALSE);
}

int al_mod_pcie_link_up_wait_ex(struct al_mod_pcie_port *pcie_port, uint32_t timeout_ms,
	al_mod_bool retrain_if_not_full_width)
{
	return _al_pcie_link_up_wait(pcie_port, timeout_ms, retrain_if_not_full_width);
}

int al_mod_pcie_link_active_lanes_get(struct al_mod_pcie_port *pcie_port)
{
	struct al_mod_pcie_regs *regs = pcie_port->regs;
	uint16_t pcie_lnksta = al_mod_reg_read16((uint16_t __iomem *)regs->core_space[0].pcie_cap_base
				+ (AL_PCI_EXP_LNKSTA >> 1));

	return (pcie_lnksta & AL_PCI_EXP_LNKSTA_NLW) >> AL_PCI_EXP_LNKSTA_NLW_SHIFT;
}

/** get link status */
int al_mod_pcie_link_status(struct al_mod_pcie_port *pcie_port,
			struct al_mod_pcie_link_status *status)
{
	struct al_mod_pcie_regs *regs = pcie_port->regs;
	uint16_t	pcie_lnksta;

	al_mod_assert(status);

	if (!al_mod_pcie_port_is_enabled(pcie_port)) {
		al_mod_dbg("PCIe %d: port not enabled, no link.\n", pcie_port->port_id);
		status->link_up = AL_FALSE;
		status->speed = AL_PCIE_LINK_SPEED_DEFAULT;
		status->lanes = 0;
		status->ltssm_state = 0;
		status->num_retrains = 0;
		return 0;
	}

	status->link_up = al_mod_pcie_check_link(pcie_port, &status->ltssm_state);

	if (!status->link_up) {
		status->speed = AL_PCIE_LINK_SPEED_DEFAULT;
		status->lanes = 0;
		status->num_retrains = 0;
		return 0;
	}

	pcie_lnksta = al_mod_reg_read16((uint16_t __iomem *)regs->core_space[0].pcie_cap_base + (AL_PCI_EXP_LNKSTA >> 1));

	switch(pcie_lnksta & AL_PCI_EXP_LNKSTA_CLS) {
		case AL_PCI_EXP_LNKSTA_CLS_2_5GB:
			status->speed = AL_PCIE_LINK_SPEED_GEN1;
			break;
		case AL_PCI_EXP_LNKSTA_CLS_5_0GB:
			status->speed = AL_PCIE_LINK_SPEED_GEN2;
			break;
		case AL_PCI_EXP_LNKSTA_CLS_8_0GB:
			status->speed = AL_PCIE_LINK_SPEED_GEN3;
			break;
		default:
			status->speed = AL_PCIE_LINK_SPEED_DEFAULT;
			al_mod_err("PCIe %d: unknown link speed indication. PCIE LINK STATUS %x\n",
				pcie_port->port_id, pcie_lnksta);
	}
	status->lanes = (pcie_lnksta & AL_PCI_EXP_LNKSTA_NLW) >> AL_PCI_EXP_LNKSTA_NLW_SHIFT;
	status->num_retrains = pcie_port->num_retrains;
	al_mod_dbg("PCIe %d: Link up. speed gen%d negotiated width %d (%u retrains)\n",
		pcie_port->port_id, status->speed, status->lanes, status->num_retrains);

	return 0;
}

/** get lane status */
void al_mod_pcie_lane_status_get(
	struct al_mod_pcie_port		*pcie_port,
	unsigned int			lane,
	struct al_mod_pcie_lane_status	*status)
{
	struct al_mod_pcie_regs *regs;
	uint32_t lane_status;
	uint32_t *reg_ptr;

	al_mod_assert(pcie_port);
	al_mod_assert(status);
	al_mod_assert(lane < lanes_num[pcie_port->rev_id]);

	regs = pcie_port->regs;
	reg_ptr = regs->axi.status.lane[lane];

	/* Reset field is valid only when same value is read twice */
	do {
		lane_status = al_mod_reg_read32(reg_ptr);
		status->is_reset = !!(lane_status & PCIE_AXI_STATUS_LANE_IS_RESET);
	} while (status->is_reset != (!!(al_mod_reg_read32(reg_ptr) & PCIE_AXI_STATUS_LANE_IS_RESET)));

	status->requested_speed =
		(lane_status & PCIE_AXI_STATUS_LANE_REQUESTED_SPEED_MASK) >>
		PCIE_AXI_STATUS_LANE_REQUESTED_SPEED_SHIFT;
}

/** trigger hot reset */
int al_mod_pcie_link_hot_reset(struct al_mod_pcie_port *pcie_port, al_mod_bool enable)
{
	struct al_mod_pcie_regs *regs = pcie_port->regs;
	uint32_t events_gen;
	al_mod_bool app_reset_state;
	enum al_mod_pcie_operating_mode op_mode = al_mod_pcie_operating_mode_get(pcie_port);

	if (op_mode != AL_PCIE_OPERATING_MODE_RC) {
		al_mod_err("PCIe %d: hot-reset is applicable only for RC mode\n", pcie_port->port_id);
		return -EINVAL;
	}

	if (!al_mod_pcie_is_link_started(pcie_port)) {
		al_mod_err("PCIe %d: link not started, cannot trigger hot-reset\n", pcie_port->port_id);
		return -EINVAL;
	}

	events_gen = al_mod_reg_read32(regs->app.global_ctrl.events_gen[0]);
	app_reset_state = events_gen & PCIE_W_GLOBAL_CTRL_EVENTS_GEN_APP_RST_INIT;

	if (enable && app_reset_state) {
		al_mod_err("PCIe %d: link is already in hot-reset state\n", pcie_port->port_id);
		return -EINVAL;
	} else if ((!enable) && (!(app_reset_state))) {
		al_mod_err("PCIe %d: link is already in non-hot-reset state\n", pcie_port->port_id);
		return -EINVAL;
	} else {
		al_mod_dbg("PCIe %d: %s hot-reset\n", pcie_port->port_id,
			(enable ? "enabling" : "disabling"));
		/* hot-reset functionality is implemented only for function 0 */
		al_mod_reg_write32_masked(regs->app.global_ctrl.events_gen[0],
			PCIE_W_GLOBAL_CTRL_EVENTS_GEN_APP_RST_INIT,
			(enable ? PCIE_W_GLOBAL_CTRL_EVENTS_GEN_APP_RST_INIT
				: ~PCIE_W_GLOBAL_CTRL_EVENTS_GEN_APP_RST_INIT));
		return 0;
	}
}

/** disable port link */
int al_mod_pcie_link_disable(struct al_mod_pcie_port *pcie_port, al_mod_bool disable)
{
	struct al_mod_pcie_regs *regs = pcie_port->regs;
	uint32_t pcie_lnkctl;
	al_mod_bool link_disable_state;
	enum al_mod_pcie_operating_mode op_mode = al_mod_pcie_operating_mode_get(pcie_port);

	if (op_mode != AL_PCIE_OPERATING_MODE_RC) {
		al_mod_err("PCIe %d: hot-reset is applicable only for RC mode\n", pcie_port->port_id);
		return -EINVAL;
	}

	if (!al_mod_pcie_is_link_started(pcie_port)) {
		al_mod_err("PCIe %d: link not started, cannot disable link\n", pcie_port->port_id);
		return -EINVAL;
	}

	pcie_lnkctl = al_mod_reg_read32(regs->core_space[0].pcie_cap_base + (AL_PCI_EXP_LNKCTL >> 2));
	link_disable_state = pcie_lnkctl & AL_PCI_EXP_LNKCTL_LNK_DIS;

	if (disable && link_disable_state) {
		al_mod_err("PCIe %d: link is already in disable state\n", pcie_port->port_id);
		return -EINVAL;
	} else if ((!disable) && (!(link_disable_state))) {
		al_mod_err("PCIe %d: link is already in enable state\n", pcie_port->port_id);
		return -EINVAL;
	}

	al_mod_dbg("PCIe %d: %s port\n", pcie_port->port_id, (disable ? "disabling" : "enabling"));
	al_mod_reg_write32_masked(regs->core_space[0].pcie_cap_base + (AL_PCI_EXP_LNKCTL >> 2),
		AL_PCI_EXP_LNKCTL_LNK_DIS,
		(disable ? AL_PCI_EXP_LNKCTL_LNK_DIS : ~AL_PCI_EXP_LNKCTL_LNK_DIS));
	return 0;
}

/** retrain link */
int al_mod_pcie_link_retrain(struct al_mod_pcie_port *pcie_port)
{
	struct al_mod_pcie_regs *regs = pcie_port->regs;
	enum al_mod_pcie_operating_mode op_mode = al_mod_pcie_operating_mode_get(pcie_port);

	if (op_mode != AL_PCIE_OPERATING_MODE_RC) {
		al_mod_err("PCIe %d: link-retrain is applicable only for RC mode\n",
			pcie_port->port_id);
		return -EINVAL;
	}

	if (!al_mod_pcie_is_link_started(pcie_port)) {
		al_mod_err("PCIe %d: link not started, cannot link-retrain\n", pcie_port->port_id);
		return -EINVAL;
	}

	al_mod_reg_write32_masked(regs->core_space[0].pcie_cap_base + (AL_PCI_EXP_LNKCTL >> 2),
	AL_PCI_EXP_LNKCTL_LNK_RTRN, AL_PCI_EXP_LNKCTL_LNK_RTRN);

	return 0;
}

/** redo equalization */
int al_mod_pcie_link_retrain_eq_redo_en(struct al_mod_pcie_port *pcie_port, al_mod_bool enable)
{
	struct al_mod_pcie_regs *regs = pcie_port->regs;
	enum al_mod_pcie_operating_mode op_mode = al_mod_pcie_operating_mode_get(pcie_port);

	if (op_mode != AL_PCIE_OPERATING_MODE_RC) {
		al_mod_err("PCIe %d: link-retrain redoing equalization is applicable only for RC mode\n",
			pcie_port->port_id);
		return -EINVAL;
	}

	al_mod_reg_write32_masked(
			regs->core_space[0].pcie_sec_ext_cap_base + (AL_PCI_EXP_LNKCTL3 >> 2),
			AL_PCI_EXP_LNKCTL3_PRFRM_EQ | AL_PCI_EXP_LNKCTL3_EQ_INT_EN,
			(enable == AL_TRUE) ?
			(AL_PCI_EXP_LNKCTL3_PRFRM_EQ | AL_PCI_EXP_LNKCTL3_EQ_INT_EN) : 0);

	return 0;
}

/* trigger speed change */
int al_mod_pcie_link_change_speed(struct al_mod_pcie_port *pcie_port,
			      enum al_mod_pcie_link_speed new_speed)
{
	struct al_mod_pcie_regs *regs = pcie_port->regs;

	if (!al_mod_pcie_is_link_started(pcie_port)) {
		al_mod_err("PCIe %d: link not started, cannot change speed\n", pcie_port->port_id);
		return -EINVAL;
	}

	al_mod_dbg("PCIe %d: changing speed to %d\n", pcie_port->port_id, new_speed);

	al_mod_pcie_port_link_speed_ctrl_set(pcie_port, new_speed);

	al_mod_reg_write32_masked(&regs->port_regs->gen2_ctrl,
		PCIE_PORT_GEN2_CTRL_DIRECT_SPEED_CHANGE,
		PCIE_PORT_GEN2_CTRL_DIRECT_SPEED_CHANGE);

	return 0;
}

/* TODO: check if this function needed */
int al_mod_pcie_link_change_width(struct al_mod_pcie_port *pcie_port,
			      uint8_t width __attribute__((__unused__)))
{
	al_mod_err("PCIe %d: link change width not implemented\n",
		pcie_port->port_id);

	return -ENOSYS;
}

void al_mod_pcie_link_ltssm_state_vec0_cmp_enable(
	struct al_mod_pcie_port *pcie_port,
	enum al_mod_pcie_ltssm_state state)
{
	struct al_mod_pcie_regs *regs;

	al_mod_assert(pcie_port);
	al_mod_assert(pcie_port->rev_id >= AL_PCIE_REV_ID_4);
	al_mod_assert(state <= AL_PCIE_LTSSM_RCVRY_EQ3);

	regs = (struct al_mod_pcie_regs *)pcie_port->regs;

	al_mod_reg_write32_masked(regs->app.global_ctrl.port_init,
			      PCIE_W_GLOBAL_CTRL_PORT_INIT_LTSSM_STATE_CMP_0_MASK,
			      state << PCIE_W_GLOBAL_CTRL_PORT_INIT_LTSSM_STATE_CMP_0_SHIFT);
}

/**************************** Post Link Start API *****************************/

/************************** Snoop Configuration API ***************************/

int
al_mod_pcie_port_snoop_config(struct al_mod_pcie_port *pcie_port, al_mod_bool enable_axi_snoop)
{
	struct al_mod_pcie_regs *regs = pcie_port->regs;

	/* Set snoop mode */
	al_mod_dbg("PCIE_%d: snoop mode %s\n",
			pcie_port->port_id, enable_axi_snoop ? "enable" : "disable");

	if (enable_axi_snoop) {
		al_mod_reg_write32_masked(regs->axi.ctrl.master_arctl,
			PCIE_AXI_CTRL_MASTER_ARCTL_OVR_SNOOP | PCIE_AXI_CTRL_MASTER_ARCTL_SNOOP,
			PCIE_AXI_CTRL_MASTER_ARCTL_OVR_SNOOP | PCIE_AXI_CTRL_MASTER_ARCTL_SNOOP);

		al_mod_reg_write32_masked(regs->axi.ctrl.master_awctl,
			PCIE_AXI_CTRL_MASTER_AWCTL_OVR_SNOOP | PCIE_AXI_CTRL_MASTER_AWCTL_SNOOP,
			PCIE_AXI_CTRL_MASTER_AWCTL_OVR_SNOOP | PCIE_AXI_CTRL_MASTER_AWCTL_SNOOP);
	} else {
		al_mod_reg_write32_masked(regs->axi.ctrl.master_arctl,
			PCIE_AXI_CTRL_MASTER_ARCTL_OVR_SNOOP | PCIE_AXI_CTRL_MASTER_ARCTL_SNOOP,
			PCIE_AXI_CTRL_MASTER_ARCTL_OVR_SNOOP);

		al_mod_reg_write32_masked(regs->axi.ctrl.master_awctl,
			PCIE_AXI_CTRL_MASTER_AWCTL_OVR_SNOOP | PCIE_AXI_CTRL_MASTER_AWCTL_SNOOP,
			PCIE_AXI_CTRL_MASTER_AWCTL_OVR_SNOOP);
	}
	return 0;
}

/************************** Configuration Space API ***************************/

/** get base address of pci configuration space header */
int al_mod_pcie_config_space_get(struct al_mod_pcie_pf *pcie_pf,
			     uint8_t __iomem **addr)
{
	struct al_mod_pcie_regs *regs = pcie_pf->pcie_port->regs;

	*addr = (uint8_t __iomem *)&regs->core_space[pcie_pf->pf_num].config_header[0];
	return 0;
}

/* Read data from the local configuration space */
uint32_t al_mod_pcie_local_cfg_space_read(
	struct al_mod_pcie_pf	*pcie_pf,
	unsigned int		reg_offset)
{
	struct al_mod_pcie_regs *regs = pcie_pf->pcie_port->regs;
	uint32_t data;

	al_mod_assert(reg_offset < AL_PCIE_CFG_SPACE_REGS_NUM);

	data = al_mod_reg_read32(&regs->core_space[pcie_pf->pf_num].config_header[reg_offset]);

	return data;
}

/* Write data to the local configuration space */
void al_mod_pcie_local_cfg_space_write(
	struct al_mod_pcie_pf	*pcie_pf,
	unsigned int		reg_offset,
	uint32_t		data,
	al_mod_bool			cs2,
	al_mod_bool			allow_ro_wr)
{
	struct al_mod_pcie_port *pcie_port = pcie_pf->pcie_port;
	struct al_mod_pcie_regs *regs = pcie_port->regs;
	unsigned int pf_num = pcie_pf->pf_num;
	uint32_t *offset = &regs->core_space[pf_num].config_header[reg_offset];

	al_mod_assert(reg_offset < AL_PCIE_CFG_SPACE_REGS_NUM);

	if (allow_ro_wr)
		al_mod_pcie_port_wr_to_ro_set(pcie_port, AL_TRUE);

	if (cs2 == AL_FALSE)
		al_mod_reg_write32(offset, data);
	else
		al_mod_reg_write32_dbi_cs2(pcie_port, offset, data);

	if (allow_ro_wr)
		al_mod_pcie_port_wr_to_ro_set(pcie_port, AL_FALSE);
}

/** set target_bus and mask_target_bus */
int al_mod_pcie_target_bus_set(
	struct al_mod_pcie_port *pcie_port,
	uint8_t target_bus,
	uint8_t mask_target_bus)
{
	struct al_mod_pcie_regs *regs = (struct al_mod_pcie_regs *)pcie_port->regs;
	uint32_t reg;

	reg = al_mod_reg_read32(regs->axi.ob_ctrl.cfg_target_bus);
	AL_REG_FIELD_SET(reg, PCIE_AXI_MISC_OB_CTRL_CFG_TARGET_BUS_MASK_MASK,
			PCIE_AXI_MISC_OB_CTRL_CFG_TARGET_BUS_MASK_SHIFT,
			mask_target_bus);
	AL_REG_FIELD_SET(reg, PCIE_AXI_MISC_OB_CTRL_CFG_TARGET_BUS_BUSNUM_MASK,
			PCIE_AXI_MISC_OB_CTRL_CFG_TARGET_BUS_BUSNUM_SHIFT,
			target_bus);
	al_mod_reg_write32(regs->axi.ob_ctrl.cfg_target_bus, reg);
	return 0;
}

/** get target_bus and mask_target_bus */
int al_mod_pcie_target_bus_get(
	struct al_mod_pcie_port *pcie_port,
	uint8_t *target_bus,
	uint8_t *mask_target_bus)
{
	struct al_mod_pcie_regs *regs = (struct al_mod_pcie_regs *)pcie_port->regs;
	uint32_t reg;

	al_mod_assert(target_bus);
	al_mod_assert(mask_target_bus);

	reg = al_mod_reg_read32(regs->axi.ob_ctrl.cfg_target_bus);

	*mask_target_bus = AL_REG_FIELD_GET(reg,
				PCIE_AXI_MISC_OB_CTRL_CFG_TARGET_BUS_MASK_MASK,
				PCIE_AXI_MISC_OB_CTRL_CFG_TARGET_BUS_MASK_SHIFT);
	*target_bus = AL_REG_FIELD_GET(reg,
			PCIE_AXI_MISC_OB_CTRL_CFG_TARGET_BUS_BUSNUM_MASK,
			PCIE_AXI_MISC_OB_CTRL_CFG_TARGET_BUS_BUSNUM_SHIFT);
	return 0;
}

/** Set secondary bus number */
int al_mod_pcie_secondary_bus_set(struct al_mod_pcie_port *pcie_port, uint8_t secbus)
{
	struct al_mod_pcie_regs *regs = pcie_port->regs;

	uint32_t secbus_val = (secbus <<
			PCIE_AXI_MISC_OB_CTRL_CFG_CONTROL_SEC_BUS_SHIFT);

	al_mod_reg_write32_masked(
		regs->axi.ob_ctrl.cfg_control,
		PCIE_AXI_MISC_OB_CTRL_CFG_CONTROL_SEC_BUS_MASK,
		secbus_val);
	return 0;
}

/** Set sub-ordinary bus number */
int al_mod_pcie_subordinary_bus_set(struct al_mod_pcie_port *pcie_port, uint8_t subbus)
{
	struct al_mod_pcie_regs *regs = pcie_port->regs;

	uint32_t subbus_val = (subbus <<
			PCIE_AXI_MISC_OB_CTRL_CFG_CONTROL_SUBBUS_SHIFT);

	al_mod_reg_write32_masked(
		regs->axi.ob_ctrl.cfg_control,
		PCIE_AXI_MISC_OB_CTRL_CFG_CONTROL_SUBBUS_MASK,
		subbus_val);
	return 0;
}

/* Enable/disable deferring incoming configuration requests */
void al_mod_pcie_app_req_retry_set(
	struct al_mod_pcie_port	*pcie_port,
	al_mod_bool			en)
{
	struct al_mod_pcie_regs *regs = pcie_port->regs;
	uint32_t mask = (pcie_port->rev_id >= AL_PCIE_REV_ID_3) ?
		PCIE_W_REV3_4_GLOBAL_CTRL_PM_CONTROL_APP_REQ_RETRY_EN :
		PCIE_W_REV1_2_GLOBAL_CTRL_PM_CONTROL_APP_REQ_RETRY_EN;

	al_mod_reg_write32_masked(regs->app.global_ctrl.pm_control,
		mask, (en == AL_TRUE) ? mask : 0);
}

/* Check if deferring incoming configuration requests is enabled or not */
al_mod_bool al_mod_pcie_app_req_retry_get_status(struct al_mod_pcie_port	*pcie_port)
{
	struct al_mod_pcie_regs *regs = pcie_port->regs;
	uint32_t pm_control;
	uint32_t mask = (pcie_port->rev_id >= AL_PCIE_REV_ID_3) ?
		PCIE_W_REV3_4_GLOBAL_CTRL_PM_CONTROL_APP_REQ_RETRY_EN :
		PCIE_W_REV1_2_GLOBAL_CTRL_PM_CONTROL_APP_REQ_RETRY_EN;

	pm_control = al_mod_reg_read32(regs->app.global_ctrl.pm_control);
	return (pm_control & mask) ? AL_TRUE : AL_FALSE;
}

/* set configuration space reset on link down event */
void al_mod_pcie_port_cfg_rst_on_link_down_set(struct al_mod_pcie_port *pcie_port, al_mod_bool enable)
{
	struct al_mod_pcie_regs *regs = pcie_port->regs;

	al_mod_assert(pcie_port->rev_id >= AL_PCIE_REV_ID_3);

	al_mod_reg_write32_masked(regs->axi.ctrl.global,
		PCIE_REV3_4_AXI_CTRL_GLOBAL_CFG_ALLOW_NONSTICKY_RESET_WHEN_LNKDOWN_CLK_RST,
		enable ?
		PCIE_REV3_4_AXI_CTRL_GLOBAL_CFG_ALLOW_NONSTICKY_RESET_WHEN_LNKDOWN_CLK_RST :
		0);
}

/*************** Internal Address Translation Unit (ATU) API ******************/

unsigned int al_mod_pcie_atu_max_num_get(
	struct al_mod_pcie_port *pcie_port,
	enum al_mod_pcie_atu_dir direction)
{
	al_mod_assert(pcie_port);

	switch (direction) {
	case AL_PCIE_ATU_DIR_OUTBOUND:
		switch (pcie_port->rev_id) {
		case AL_PCIE_REV_ID_1:
		case AL_PCIE_REV_ID_2:
			return AL_PCIE_REV_1_2_ATUS_OB_NUM;
		case AL_PCIE_REV_ID_3:
		case AL_PCIE_REV_ID_4:
			return AL_PCIE_REV_3_4_ATUS_OB_NUM;
		default:
			al_mod_assert_msg(0,
				      "%s: invalid port rev ID [%d]\n",
				      __func__,
				      pcie_port->rev_id);
		}
		break;
	case AL_PCIE_ATU_DIR_INBOUND:
		switch (pcie_port->rev_id) {
		case AL_PCIE_REV_ID_1:
		case AL_PCIE_REV_ID_2:
			return AL_PCIE_REV_1_2_ATUS_IB_NUM;
		case AL_PCIE_REV_ID_3:
			return AL_PCIE_REV_3_ATUS_IB_NUM;
		case AL_PCIE_REV_ID_4:
			if (pcie_port->subrev_id == PCIE_AXI_DEVICE_ID_REG_REV_ID_X4)
				return AL_PCIE_REV_4_X4_ATUS_IB_NUM;
			else
				return AL_PCIE_REV4_X8_X16_ATUS_IB_NUM;
		default:
			al_mod_assert_msg(0,
				      "%s: invalid port rev ID [%d]\n",
				      __func__,
				      pcie_port->rev_id);
		}
		break;
	default:
		al_mod_assert_msg(0,
			      "%s: invalid direction [%d]\n",
			      __func__,
			      direction);
	}

	return 0;
}

al_mod_bool al_mod_pcie_atu_region_is_valid(struct al_mod_pcie_atu_region *atu_region)
{
	al_mod_assert(atu_region);

	if (!atu_region->enable)
		return AL_TRUE;

	/*
	 * address match mode
	 */
	if (atu_region->match_mode == 0) {
		const uint64_t limit_sz_mask = atu_region->limit - atu_region->base_addr;

		if (atu_region->limit < atu_region->base_addr)
			return AL_FALSE;

		if ((atu_region->limit >= (1ULL << 32)) &&
		    (atu_region->base_addr < (1ULL << 32))) {
			al_mod_err("%s: limit and base must both be greater or smaller than 32b\n",
				__func__);
			return AL_FALSE;
		}

		/*
		 * if working in extended mode
		 */
		if (limit_sz_mask > AL_PCIE_ATU_ADDRESS_MATCH_EXT_MODE_THRESHOLD) {
			/**
			 * - size most be a power of 2
			 * - base address must be aligned to size
			 * - target address must be aligned to size
			 */
			if (!AL_IS_POW_OF_TWO(limit_sz_mask + 1)) {
				al_mod_err("%s: size (0x%" PRIx64 ") "
					"must be a power of 2\n",
					__func__,
					limit_sz_mask + 1);
				return AL_FALSE;
			}

			if (atu_region->target_addr & limit_sz_mask) {
				al_mod_err("%s: target address (0x%" PRIx64 ") "
					"not aligned to size (0x%" PRIx64 ")\n",
					__func__,
					atu_region->target_addr,
					limit_sz_mask + 1);
				return AL_FALSE;
			}

			if (atu_region->base_addr & limit_sz_mask) {
				al_mod_err("%s: base address (0x%" PRIx64 ") "
					"not aligned to size (0x%" PRIx64 ")\n",
					__func__,
					atu_region->base_addr,
					limit_sz_mask + 1);
				return AL_FALSE;
			}
		} else {
			/**
			 * if working in regular mode:
			 * - base address most be aligned to 4k
			 * - limit address + 1 must be aligned to 4k
			 * - target address must be aligned to 4k
			 */
			if (atu_region->base_addr % 0x1000) {
				al_mod_err("%s: base address (0x%" PRIx64 ") "
					"not aligned to 4k\n",
					__func__,
					atu_region->base_addr);
				return AL_FALSE;
			}

			if ((atu_region->limit + 1) % 0x1000) {
				al_mod_err("%s: limit address (0x%" PRIx64 ") + 1 not aligned to 4k\n",
					__func__,
					atu_region->limit);
				return AL_FALSE;
			}

			if (atu_region->target_addr % 0x1000) {
				al_mod_err("%s: target address (0x%" PRIx64 ") "
					"not aligned to 4k\n",
					__func__,
					atu_region->target_addr);
				return AL_FALSE;
			}
		}
	} else {
		/*
		 * bar match mode
		 */
		if (atu_region->target_addr % 0x1000) {
			al_mod_err("%s: target address (0x%" PRIx64 ") "
				"not aligned to 4k\n",
				__func__,
				atu_region->target_addr);
			return AL_FALSE;
		}
	}

	return AL_TRUE;
}

/** program internal ATU region entry */
int al_mod_pcie_atu_region_set(
	struct al_mod_pcie_port *pcie_port,
	struct al_mod_pcie_atu_region *atu_region)
{
	struct al_mod_pcie_regs *regs = pcie_port->regs;
	enum al_mod_pcie_operating_mode op_mode = al_mod_pcie_operating_mode_get(pcie_port);
	unsigned int max_atus = al_mod_pcie_atu_max_num_get(pcie_port, atu_region->direction);
	uint32_t reg;

	/*
	 * assert in case index in out of bounds
	 * since HW will accept wrong index write
	 * and other ATU register writes will have no affect.
	 */
	al_mod_assert(max_atus > atu_region->index);

	/**
	 * Addressing RMN: 5384
	 *
	 * RMN description:
	 * From SNPS (also included in the data book) Dynamic iATU Programming
	 * With AHB/AXI Bridge Module When the bridge slave interface clock
	 * (hresetn or slv_aclk) is asynchronous to the PCIe native core clock
	 * (core_clk), you must not update the iATU registers while operations
	 * are in progress on the AHB/AXI bridge slave interface. The iATU
	 * registers are in the core_clk clock domain. The register outputs are
	 * used in the AHB/AXI bridge slave interface clock domain. There is no
	 * synchronization logic between these registers and the AHB/AXI bridge
	 * slave interface.
	 *
	 * Software flow:
	 * Do not allow configuring Outbound iATU after link is started
	 */
	if ((atu_region->direction == AL_PCIE_ATU_DIR_OUTBOUND)
		&& (al_mod_pcie_is_link_started(pcie_port))) {
		if (!atu_region->enforce_ob_atu_region_set) {
			al_mod_err("PCIe %d: setting OB iATU after link is started is not allowed\n",
				pcie_port->port_id);
			al_mod_assert(AL_FALSE);
			return -EINVAL;
		} else {
			al_mod_info("PCIe %d: setting OB iATU even after link is started\n",
				pcie_port->port_id);
		}
	}

	/* set atu index and direction */
	reg = 0;
	AL_REG_FIELD_SET(reg,
			 PCIE_IATU_INDEX_REGION_INDEX_MASK,
			 PCIE_IATU_INDEX_REGION_INDEX_SHIFT,
			 atu_region->index);
	AL_REG_BIT_VAL_SET(reg, PCIE_IATU_INDEX_REGION_DIR_SHIFT, atu_region->direction);
	al_mod_reg_write32(&regs->port_regs->iatu.index, reg);

	/* in case of disable no need validate/write to configuration/address/limit registers */
	if (!atu_region->enable) {
		/* enable = 0 */
		al_mod_reg_write32(&regs->port_regs->iatu.cr2, 0);

		return 0;
	}

	/**
	 * iATU base/target addresses must be aligned to the size if:
	 * - address match-mode
	 * - size is larger than 4GB
	 */
	if (atu_region->match_mode == 0) {
		const uint64_t limit_sz_mask = atu_region->limit - atu_region->base_addr;
		if (limit_sz_mask > (1ULL << 32) /* 4GB */) {
			if (atu_region->target_addr & limit_sz_mask) {
				al_mod_err("PCIe %d: target addr (0x%" PRIx64 ") "
						"not aligned to size (0x%" PRIx64 ")\n",
					pcie_port->port_id, atu_region->target_addr, limit_sz_mask);
				return -EINVAL;
			}

			if (atu_region->base_addr & limit_sz_mask) {
				al_mod_err("PCIe %d: base addr (0x%" PRIx64 ") "
						"not aligned to size (0x%" PRIx64 ")\n",
					pcie_port->port_id, atu_region->base_addr, limit_sz_mask);
				return -EINVAL;
			}
		}
	}

	al_mod_reg_write32(&regs->port_regs->iatu.lower_base_addr,
			(uint32_t)(atu_region->base_addr & 0xFFFFFFFF));
	al_mod_reg_write32(&regs->port_regs->iatu.upper_base_addr,
			(uint32_t)((atu_region->base_addr >> 32)& 0xFFFFFFFF));
	al_mod_reg_write32(&regs->port_regs->iatu.lower_target_addr,
			(uint32_t)(atu_region->target_addr & 0xFFFFFFFF));
	al_mod_reg_write32(&regs->port_regs->iatu.upper_target_addr,
			(uint32_t)((atu_region->target_addr >> 32)& 0xFFFFFFFF));

	/* configure the limit, not needed when working in BAR match mode */
	if (atu_region->match_mode == 0) {
		uint32_t limit_reg_val;
		uint32_t *limit_ext_reg =
			(atu_region->direction == AL_PCIE_ATU_DIR_OUTBOUND) ?
			&regs->app.atu.out_mask_pair[atu_region->index / 2] :
			&regs->app.atu.in_mask_pair[atu_region->index / 2];
		uint32_t limit_ext_reg_mask =
			(atu_region->index % 2) ?
			PCIE_W_ATU_MASK_EVEN_ODD_ATU_MASK_40_32_ODD_MASK :
			PCIE_W_ATU_MASK_EVEN_ODD_ATU_MASK_40_32_EVEN_MASK;
		unsigned int limit_ext_reg_shift =
			(atu_region->index % 2) ?
			PCIE_W_ATU_MASK_EVEN_ODD_ATU_MASK_40_32_ODD_SHIFT :
			PCIE_W_ATU_MASK_EVEN_ODD_ATU_MASK_40_32_EVEN_SHIFT;
		uint64_t limit_sz_msk =
			atu_region->limit - atu_region->base_addr;
		uint32_t limit_ext_reg_val = (uint32_t)(((limit_sz_msk) >>
					32) & 0xFFFFFFFF);

		if (limit_ext_reg_val) {
			limit_reg_val =	(uint32_t)((limit_sz_msk) & 0xFFFFFFFF);
			al_mod_assert(limit_reg_val == 0xFFFFFFFF);
		} else {
			limit_reg_val = (uint32_t)(atu_region->limit &
					0xFFFFFFFF);
		}

		al_mod_reg_write32_masked(
				limit_ext_reg,
				limit_ext_reg_mask,
				limit_ext_reg_val << limit_ext_reg_shift);

		al_mod_reg_write32(&regs->port_regs->iatu.limit_addr,
				limit_reg_val);
	}


	/**
	* Addressing RMN: 3186
	*
	* RMN description:
	* Bug in SNPS IP (versions 4.21 , 4.10a-ea02)
	* In CFG request created via outbound atu (shift mode) bits [27:12] go to
	* [31:16] , the shifting is correct , however the ATU leaves bit [15:12]
	* to their original values, this is then transmited in the tlp .
	* Those bits are currently reserved ,bit might be non-resv. in future generations .
	*
	* Software flow:
	* Enable HW fix
	* rev=REV1,REV2 set bit 15 in corresponding app_reg.atu.out_mask
	* rev>REV2 set corresponding bit is app_reg.atu.reg_out_mask
	*/
	if ((atu_region->cfg_shift_mode == AL_TRUE) &&
		(atu_region->direction == AL_PCIE_ATU_DIR_OUTBOUND)) {
		if (pcie_port->rev_id > AL_PCIE_REV_ID_2) {
			al_mod_reg_write32_masked(regs->app.atu.reg_out_mask,
			1 << (atu_region->index) ,
			1 << (atu_region->index));
		} else {
			uint32_t *limit_ext_reg =
				(atu_region->direction == AL_PCIE_ATU_DIR_OUTBOUND) ?
				&regs->app.atu.out_mask_pair[atu_region->index / 2] :
				&regs->app.atu.in_mask_pair[atu_region->index / 2];
			uint32_t limit_ext_reg_mask =
				(atu_region->index % 2) ?
				PCIE_W_ATU_MASK_EVEN_ODD_ATU_MASK_40_32_ODD_MASK :
				PCIE_W_ATU_MASK_EVEN_ODD_ATU_MASK_40_32_EVEN_MASK;
			unsigned int limit_ext_reg_shift =
				(atu_region->index % 2) ?
				PCIE_W_ATU_MASK_EVEN_ODD_ATU_MASK_40_32_ODD_SHIFT :
				PCIE_W_ATU_MASK_EVEN_ODD_ATU_MASK_40_32_EVEN_SHIFT;

			al_mod_reg_write32_masked(
				limit_ext_reg,
				limit_ext_reg_mask,
				(AL_BIT(15)) << limit_ext_reg_shift);
		}
	}

	reg = 0;
	AL_REG_FIELD_SET(reg, 0x1F, 0, atu_region->tlp_type);
	AL_REG_FIELD_SET(reg, 0x3 << 9, 9, atu_region->attr);


	if ((pcie_port->rev_id >= AL_PCIE_REV_ID_3)
		&& (op_mode == AL_PCIE_OPERATING_MODE_EP)
		&& (((atu_region->function_match_bypass_mode) &&
		     (atu_region->direction == AL_PCIE_ATU_DIR_INBOUND)) ||
		    ((!atu_region->function_match_bypass_mode) &&
		     (atu_region->direction == AL_PCIE_ATU_DIR_OUTBOUND)))) {
		AL_REG_FIELD_SET(reg,
			PCIE_IATU_CR1_FUNC_NUM_MASK,
			PCIE_IATU_CR1_FUNC_NUM_SHIFT,
			atu_region->function_match_bypass_mode_number);
	}

	al_mod_reg_write32(&regs->port_regs->iatu.cr1, reg);

	/* Enable/disable the region. */
	reg = 0;
	AL_REG_FIELD_SET(reg, 0xFF, 0, atu_region->msg_code);
	AL_REG_FIELD_SET(reg, 0x700, 8, atu_region->bar_number);
	AL_REG_FIELD_SET(reg, 0x3 << 24, 24, atu_region->response);
	AL_REG_BIT_VAL_SET(reg, 16, atu_region->enable_attr_match_mode == AL_TRUE);
	AL_REG_BIT_VAL_SET(reg, 21, atu_region->enable_msg_match_mode == AL_TRUE);
	AL_REG_BIT_VAL_SET(reg, 28, atu_region->cfg_shift_mode == AL_TRUE);
	AL_REG_BIT_VAL_SET(reg, 29, atu_region->invert_matching == AL_TRUE);
	if (atu_region->tlp_type == AL_PCIE_TLP_TYPE_MEM || atu_region->tlp_type == AL_PCIE_TLP_TYPE_IO)
		AL_REG_BIT_VAL_SET(reg, 30, !!atu_region->match_mode);
	AL_REG_BIT_SET(reg, 31);

	/* In outbound, enable function bypass
	 * In inbound, enable function match mode
	 * Note: this is the same bit, has different meanings in ob/ib ATUs
	 */
	if (op_mode == AL_PCIE_OPERATING_MODE_EP)
		AL_REG_FIELD_SET(reg,
			PCIE_IATU_CR2_FUNC_NUM_TRANS_BYPASS_FUNC_MATCH_ENABLE_MASK,
			PCIE_IATU_CR2_FUNC_NUM_TRANS_BYPASS_FUNC_MATCH_ENABLE_SHIFT,
			atu_region->function_match_bypass_mode ? 0x1 : 0x0);

	al_mod_reg_write32(&regs->port_regs->iatu.cr2, reg);

	return 0;
}

void al_mod_pcie_atu_region_get(
	struct al_mod_pcie_port *pcie_port,
	enum al_mod_pcie_atu_dir direction,
	uint8_t index,
	struct al_mod_pcie_atu_region *atu_region)
{
	struct al_mod_pcie_regs *regs = pcie_port->regs;
	unsigned int max_atus = al_mod_pcie_atu_max_num_get(pcie_port, direction);
	uint32_t reg = 0;

	al_mod_assert(atu_region);

	/*
	 * assert in case index in out of bounds
	 * since HW will accept wrong index write
	 * and other ATU register writes will have no affect.
	 */
	al_mod_assert(max_atus > index);

	/*
	 * set desired ATU
	 */
	AL_REG_FIELD_SET(reg,
			 PCIE_IATU_INDEX_REGION_INDEX_MASK,
			 PCIE_IATU_INDEX_REGION_INDEX_SHIFT,
			 index);
	AL_REG_BIT_VAL_SET(reg, PCIE_IATU_INDEX_REGION_DIR_SHIFT, direction);
	al_mod_reg_write32(&regs->port_regs->iatu.index, reg);

	reg = al_mod_reg_read32(&regs->port_regs->iatu.upper_base_addr);
	atu_region->base_addr = (uint64_t)reg << 32;
	atu_region->base_addr |= al_mod_reg_read32(&regs->port_regs->iatu.lower_base_addr);

	atu_region->target_addr =
			(uint64_t)al_mod_reg_read32(&regs->port_regs->iatu.upper_target_addr) << 32;
	atu_region->target_addr |= al_mod_reg_read32(&regs->port_regs->iatu.lower_target_addr);

	/*
	 * limit high address bits are the same as base address high bits
	 */
	atu_region->limit = (uint64_t)reg << 32;
	atu_region->limit |= al_mod_reg_read32(&regs->port_regs->iatu.limit_addr);

	reg = al_mod_reg_read32(&regs->port_regs->iatu.cr1);
	atu_region->function_match_bypass_mode_number =
			AL_REG_FIELD_GET(reg,
					 PCIE_IATU_CR1_FUNC_NUM_MASK,
					 PCIE_IATU_CR1_FUNC_NUM_SHIFT);
	atu_region->attr = AL_REG_FIELD_GET(reg,
					    PCIE_IATU_CR1_ATTER_MASK,
					    PCIE_IATU_CR1_ATTER_SHIFT);
	atu_region->tlp_type = AL_REG_FIELD_GET(reg,
						PCIE_IATU_CR1_TYPE_MASK,
						PCIE_IATU_CR1_TYPE_SHIFT);

	reg = al_mod_reg_read32(&regs->port_regs->iatu.cr2);
	atu_region->enable = AL_REG_MASK_IS_SET(reg, PCIE_IATU_CR2_ENABLE);
	atu_region->match_mode = AL_REG_MASK_IS_SET(reg, PCIE_IATU_CR2_MATCH_MODE);
	atu_region->invert_matching = AL_REG_MASK_IS_SET(reg, PCIE_IATU_CR2_INVERT_MODE);
	atu_region->msg_code =  AL_REG_FIELD_GET(reg,
						 PCIE_IATU_CR2_MSG_CODE_MASK,
						 PCIE_IATU_CR2_MSG_CODE_SHIFT);
	atu_region->bar_number =  AL_REG_FIELD_GET(reg,
						   PCIE_IATU_CR2_MSG_BAR_NUM_MASK,
						   PCIE_IATU_CR2_MSG_BAR_NUM_SHIFT);
	atu_region->response =  AL_REG_FIELD_GET(reg,
						 PCIE_IATU_CR2_RESPONSE_CODE_MASK,
						 PCIE_IATU_CR2_RESPONSE_CODE_SHIFT);
	atu_region->enable_attr_match_mode = AL_REG_MASK_IS_SET(reg, PCIE_IATU_CR2_ATTR_MATCH_EN);
	atu_region->enable_msg_match_mode =
			AL_REG_MASK_IS_SET(reg, PCIE_IATU_CR2_MSG_CODE_MATCH_EN);
	atu_region->cfg_shift_mode = AL_REG_MASK_IS_SET(reg, PCIE_IATU_CR2_CFG_SHIFT_MODE);
	atu_region->function_match_bypass_mode =
		AL_REG_MASK_IS_SET(reg, PCIE_IATU_CR2_FUNC_NUM_TRANS_BYPASS_FUNC_MATCH_ENABLE_MASK);
}

/** obtains internal ATU region base/target addresses */
void al_mod_pcie_atu_region_get_fields(
	struct al_mod_pcie_port *pcie_port,
	enum al_mod_pcie_atu_dir direction, uint8_t index,
	al_mod_bool *enable, uint64_t *base_addr, uint64_t *target_addr)
{
	struct al_mod_pcie_atu_region atu_region;

	al_mod_pcie_atu_region_get(pcie_port, direction, index, &atu_region);

	*enable = atu_region.enable;
	*base_addr = atu_region.base_addr;
	*target_addr = atu_region.target_addr;
}

void al_mod_pcie_axi_io_config(
	struct al_mod_pcie_port *pcie_port,
	al_mod_phys_addr_t start,
	al_mod_phys_addr_t end)
{
	struct al_mod_pcie_regs *regs = pcie_port->regs;

	al_mod_reg_write32(regs->axi.ob_ctrl.io_start_h,
			(uint32_t)((start >> 32) & 0xFFFFFFFF));

	al_mod_reg_write32(regs->axi.ob_ctrl.io_start_l,
			(uint32_t)(start & 0xFFFFFFFF));

	al_mod_reg_write32(regs->axi.ob_ctrl.io_limit_h,
			(uint32_t)((end >> 32) & 0xFFFFFFFF));

	al_mod_reg_write32(regs->axi.ob_ctrl.io_limit_l,
			(uint32_t)(end & 0xFFFFFFFF));

	al_mod_reg_write32_masked(regs->axi.ctrl.slv_ctl,
			      PCIE_AXI_CTRL_SLV_CTRL_IO_BAR_EN,
			      PCIE_AXI_CTRL_SLV_CTRL_IO_BAR_EN);
}

/************** Interrupt and Event generation (Endpoint mode Only) API *****************/

int al_mod_pcie_pf_flr_done_gen(struct al_mod_pcie_pf		*pcie_pf)
{
	struct al_mod_pcie_regs *regs = pcie_pf->pcie_port->regs;
	unsigned int pf_num = pcie_pf->pf_num;

	al_mod_reg_write32_masked(regs->app.global_ctrl.events_gen[pf_num],
			PCIE_W_GLOBAL_CTRL_EVENTS_GEN_FLR_PF_DONE,
			PCIE_W_GLOBAL_CTRL_EVENTS_GEN_FLR_PF_DONE);
	al_mod_udelay(AL_PCIE_FLR_DONE_INTERVAL);
	al_mod_reg_write32_masked(regs->app.global_ctrl.events_gen[pf_num],
			PCIE_W_GLOBAL_CTRL_EVENTS_GEN_FLR_PF_DONE, 0);
	return 0;
}


/** generate INTx Assert/DeAssert Message */
int al_mod_pcie_legacy_int_gen(
	struct al_mod_pcie_pf		*pcie_pf,
	al_mod_bool				assert,
	enum al_mod_pcie_legacy_int_type	type)
{
	struct al_mod_pcie_regs *regs = pcie_pf->pcie_port->regs;
	unsigned int pf_num = pcie_pf->pf_num;
	uint32_t reg;

	al_mod_assert(type == AL_PCIE_LEGACY_INTA); /* only INTA supported */
	reg = al_mod_reg_read32(regs->app.global_ctrl.events_gen[pf_num]);
	AL_REG_BIT_VAL_SET(reg, 3, !!assert);
	al_mod_reg_write32(regs->app.global_ctrl.events_gen[pf_num], reg);

	return 0;
}

/** generate MSI interrupt */
int al_mod_pcie_msi_int_gen(struct al_mod_pcie_pf *pcie_pf, uint8_t vector)
{
	struct al_mod_pcie_regs *regs = pcie_pf->pcie_port->regs;
	unsigned int pf_num = pcie_pf->pf_num;
	uint32_t reg;

	/* set msi vector and clear MSI request */
	reg = al_mod_reg_read32(regs->app.global_ctrl.events_gen[pf_num]);
	AL_REG_BIT_CLEAR(reg, 4);
	AL_REG_FIELD_SET(reg,
			PCIE_W_GLOBAL_CTRL_EVENTS_GEN_MSI_VECTOR_MASK,
			PCIE_W_GLOBAL_CTRL_EVENTS_GEN_MSI_VECTOR_SHIFT,
			vector);
	al_mod_reg_write32(regs->app.global_ctrl.events_gen[pf_num], reg);
	/* set MSI request */
	AL_REG_BIT_SET(reg, 4);
	al_mod_reg_write32(regs->app.global_ctrl.events_gen[pf_num], reg);

	return 0;
}

/** configure MSIX capability */
int al_mod_pcie_msix_config(
	struct al_mod_pcie_pf *pcie_pf,
	struct al_mod_pcie_msix_params *msix_params)
{
	struct al_mod_pcie_regs *regs = pcie_pf->pcie_port->regs;
	unsigned int pf_num = pcie_pf->pf_num;
	uint32_t msix_reg0;

	al_mod_pcie_port_wr_to_ro_set(pcie_pf->pcie_port, AL_TRUE);

	msix_reg0 = al_mod_reg_read32(regs->core_space[pf_num].msix_cap_base);

	msix_reg0 &= ~(AL_PCI_MSIX_MSGCTRL_TBL_SIZE << AL_PCI_MSIX_MSGCTRL_TBL_SIZE_SHIFT);
	msix_reg0 |= ((msix_params->table_size - 1) & AL_PCI_MSIX_MSGCTRL_TBL_SIZE) <<
			AL_PCI_MSIX_MSGCTRL_TBL_SIZE_SHIFT;
	al_mod_reg_write32(regs->core_space[pf_num].msix_cap_base, msix_reg0);

	/* Table offset & BAR */
	al_mod_reg_write32(regs->core_space[pf_num].msix_cap_base + (AL_PCI_MSIX_TABLE >> 2),
		       (msix_params->table_offset & AL_PCI_MSIX_TABLE_OFFSET) |
			       (msix_params->table_bar & AL_PCI_MSIX_TABLE_BAR));
	/* PBA offset & BAR */
	al_mod_reg_write32(regs->core_space[pf_num].msix_cap_base + (AL_PCI_MSIX_PBA >> 2),
		       (msix_params->pba_offset & AL_PCI_MSIX_PBA_OFFSET) |
			       (msix_params->pba_bar & AL_PCI_MSIX_PBA_BAR));

	al_mod_pcie_port_wr_to_ro_set(pcie_pf->pcie_port, AL_FALSE);

	return 0;
}

/** check whether MSIX is enabled */
al_mod_bool al_mod_pcie_msix_enabled(struct al_mod_pcie_pf	*pcie_pf)
{
	struct al_mod_pcie_regs *regs = pcie_pf->pcie_port->regs;
	uint32_t msix_reg0 = al_mod_reg_read32(regs->core_space[pcie_pf->pf_num].msix_cap_base);

	if (msix_reg0 & AL_PCI_MSIX_MSGCTRL_EN)
		return AL_TRUE;
	return AL_FALSE;
}

/** check whether MSIX is masked */
al_mod_bool al_mod_pcie_msix_masked(struct al_mod_pcie_pf *pcie_pf)
{
	struct al_mod_pcie_regs *regs = pcie_pf->pcie_port->regs;
	uint32_t msix_reg0 = al_mod_reg_read32(regs->core_space[pcie_pf->pf_num].msix_cap_base);

	if (msix_reg0 & AL_PCI_MSIX_MSGCTRL_MASK)
		return AL_TRUE;
	return AL_FALSE;
}

/******************** Advanced Error Reporting (AER) API **********************/
/************************* Auxiliary functions ********************************/
/* configure AER capability */
static int al_mod_pcie_aer_config_aux(
		struct al_mod_pcie_port		*pcie_port,
		unsigned int	pf_num,
		struct al_mod_pcie_aer_params	*params)
{
	struct al_mod_pcie_regs *regs = pcie_port->regs;
	struct al_mod_pcie_core_aer_regs *aer_regs = regs->core_space[pf_num].aer;
	uint32_t reg_val;

	reg_val = al_mod_reg_read32(&aer_regs->header);

	if (((reg_val & PCIE_AER_CAP_ID_MASK) >> PCIE_AER_CAP_ID_SHIFT) !=
		PCIE_AER_CAP_ID_VAL)
		return -EIO;

	if (((reg_val & PCIE_AER_CAP_VER_MASK) >> PCIE_AER_CAP_VER_SHIFT) !=
		PCIE_AER_CAP_VER_VAL)
		return -EIO;

	al_mod_reg_write32(&aer_regs->corr_err_mask, ~params->enabled_corr_err);

	al_mod_reg_write32(&aer_regs->uncorr_err_mask,
		~((params->enabled_uncorr_non_fatal_err) |
		  (params->enabled_uncorr_fatal_err)));

	al_mod_reg_write32(&aer_regs->uncorr_err_severity,
		params->enabled_uncorr_fatal_err);

	al_mod_reg_write32(&aer_regs->cap_and_ctrl,
		(params->ecrc_gen_en ? PCIE_AER_CTRL_STAT_ECRC_GEN_EN : 0) |
		(params->ecrc_chk_en ? PCIE_AER_CTRL_STAT_ECRC_CHK_EN : 0));

	/**
	 * Addressing RMN: 5119
	 *
	 * RMN description:
	 * ECRC generation for outbound request translated by iATU is effected
	 * by iATU setting instead of ecrc_gen_bit in AER
	 *
	 * Software flow:
	 * When enabling ECRC generation, set the outbound iATU to generate ECRC
	 */
	if (params->ecrc_gen_en == AL_TRUE) {
		al_mod_pcie_ecrc_gen_ob_atu_enable(pcie_port, pf_num);
	}

	al_mod_reg_write32_masked(
		regs->core_space[pf_num].pcie_dev_ctrl_status,
		PCIE_PORT_DEV_CTRL_STATUS_CORR_ERR_REPORT_EN |
		PCIE_PORT_DEV_CTRL_STATUS_NON_FTL_ERR_REPORT_EN |
		PCIE_PORT_DEV_CTRL_STATUS_FTL_ERR_REPORT_EN |
		PCIE_PORT_DEV_CTRL_STATUS_UNSUP_REQ_REPORT_EN,
		(params->enabled_corr_err ?
		 PCIE_PORT_DEV_CTRL_STATUS_CORR_ERR_REPORT_EN : 0) |
		(params->enabled_uncorr_non_fatal_err ?
		 PCIE_PORT_DEV_CTRL_STATUS_NON_FTL_ERR_REPORT_EN : 0) |
		(params->enabled_uncorr_fatal_err ?
		 PCIE_PORT_DEV_CTRL_STATUS_FTL_ERR_REPORT_EN : 0) |
		((params->enabled_uncorr_non_fatal_err &
		  AL_PCIE_AER_UNCORR_UNSUPRT_REQ_ERR) ?
		 PCIE_PORT_DEV_CTRL_STATUS_UNSUP_REQ_REPORT_EN : 0) |
		((params->enabled_uncorr_fatal_err &
		  AL_PCIE_AER_UNCORR_UNSUPRT_REQ_ERR) ?
		 PCIE_PORT_DEV_CTRL_STATUS_UNSUP_REQ_REPORT_EN : 0));

	return 0;
}

/** AER uncorrectable errors get and clear */
static unsigned int al_mod_pcie_aer_uncorr_get_and_clear_aux(
		struct al_mod_pcie_port		*pcie_port,
		unsigned int	pf_num)
{
	struct al_mod_pcie_regs *regs = pcie_port->regs;
	struct al_mod_pcie_core_aer_regs *aer_regs = regs->core_space[pf_num].aer;
	uint32_t reg_val;

	reg_val = al_mod_reg_read32(&aer_regs->uncorr_err_stat);
	al_mod_reg_write32(&aer_regs->uncorr_err_stat, reg_val);

	return reg_val;
}

/** AER correctable errors get and clear */
static unsigned int al_mod_pcie_aer_corr_get_and_clear_aux(
		struct al_mod_pcie_port		*pcie_port,
		unsigned int	pf_num)
{
	struct al_mod_pcie_regs *regs = pcie_port->regs;
	struct al_mod_pcie_core_aer_regs *aer_regs = regs->core_space[pf_num].aer;
	uint32_t reg_val;

	reg_val = al_mod_reg_read32(&aer_regs->corr_err_stat);
	al_mod_reg_write32(&aer_regs->corr_err_stat, reg_val);

	return reg_val;
}

#if (AL_PCIE_AER_ERR_TLP_HDR_NUM_DWORDS != 4)
#error Wrong assumption!
#endif

/** AER get the header for the TLP corresponding to a detected error */
static void al_mod_pcie_aer_err_tlp_hdr_get_aux(
		struct al_mod_pcie_port		*pcie_port,
		unsigned int	pf_num,
	uint32_t hdr[AL_PCIE_AER_ERR_TLP_HDR_NUM_DWORDS])
{
	struct al_mod_pcie_regs *regs = pcie_port->regs;
	struct al_mod_pcie_core_aer_regs *aer_regs = regs->core_space[pf_num].aer;
	int i;

	for (i = 0; i < AL_PCIE_AER_ERR_TLP_HDR_NUM_DWORDS; i++)
		hdr[i] = al_mod_reg_read32(&aer_regs->header_log[i]);
}

/******************** EP AER functions **********************/
/** configure EP physical function AER capability */
int al_mod_pcie_aer_config(
		struct al_mod_pcie_pf *pcie_pf,
		struct al_mod_pcie_aer_params	*params)
{
	al_mod_assert(pcie_pf);
	al_mod_assert(params);

	return al_mod_pcie_aer_config_aux(
			pcie_pf->pcie_port, pcie_pf->pf_num, params);
}

/** EP physical function AER uncorrectable errors get and clear */
unsigned int al_mod_pcie_aer_uncorr_get_and_clear(struct al_mod_pcie_pf *pcie_pf)
{
	al_mod_assert(pcie_pf);

	return al_mod_pcie_aer_uncorr_get_and_clear_aux(
			pcie_pf->pcie_port, pcie_pf->pf_num);
}

/** EP physical function AER correctable errors get and clear */
unsigned int al_mod_pcie_aer_corr_get_and_clear(struct al_mod_pcie_pf *pcie_pf)
{
	al_mod_assert(pcie_pf);

	return al_mod_pcie_aer_corr_get_and_clear_aux(
			pcie_pf->pcie_port, pcie_pf->pf_num);
}

/**
 * EP physical function AER get the header for
 * the TLP corresponding to a detected error
 * */
void al_mod_pcie_aer_err_tlp_hdr_get(
		struct al_mod_pcie_pf *pcie_pf,
		uint32_t hdr[AL_PCIE_AER_ERR_TLP_HDR_NUM_DWORDS])
{
	al_mod_assert(pcie_pf);
	al_mod_assert(hdr);

	al_mod_pcie_aer_err_tlp_hdr_get_aux(
			pcie_pf->pcie_port, pcie_pf->pf_num, hdr);
}

/******************** RC AER functions **********************/
/** configure RC port AER capability */
int al_mod_pcie_port_aer_config(
		struct al_mod_pcie_port		*pcie_port,
		struct al_mod_pcie_aer_params	*params)
{
	al_mod_assert(pcie_port);
	al_mod_assert(params);

	/**
	* For RC mode there's no PFs (neither PF handles),
	* therefore PF#0 is used
	* */
	return al_mod_pcie_aer_config_aux(pcie_port, 0, params);
}

/** RC port AER uncorrectable errors get and clear */
unsigned int al_mod_pcie_port_aer_uncorr_get_and_clear(
		struct al_mod_pcie_port		*pcie_port)
{
	al_mod_assert(pcie_port);

	/**
	* For RC mode there's no PFs (neither PF handles),
	* therefore PF#0 is used
	* */
	return al_mod_pcie_aer_uncorr_get_and_clear_aux(pcie_port, 0);
}

/** RC port AER correctable errors get and clear */
unsigned int al_mod_pcie_port_aer_corr_get_and_clear(
		struct al_mod_pcie_port		*pcie_port)
{
	al_mod_assert(pcie_port);

	/**
	* For RC mode there's no PFs (neither PF handles),
	* therefore PF#0 is used
	* */
	return al_mod_pcie_aer_corr_get_and_clear_aux(pcie_port, 0);
}

/** RC port AER get the header for the TLP corresponding to a detected error */
void al_mod_pcie_port_aer_err_tlp_hdr_get(
		struct al_mod_pcie_port		*pcie_port,
		uint32_t hdr[AL_PCIE_AER_ERR_TLP_HDR_NUM_DWORDS])
{
	al_mod_assert(pcie_port);
	al_mod_assert(hdr);

	/**
	* For RC mode there's no PFs (neither PF handles),
	* therefore PF#0 is used
	* */
	al_mod_pcie_aer_err_tlp_hdr_get_aux(pcie_port, 0, hdr);
}

/********************** Loopback mode (RC and Endpoint modes) ************/

/** enter local pipe loopback mode */
int al_mod_pcie_local_pipe_loopback_enter(struct al_mod_pcie_port *pcie_port)
{
	struct al_mod_pcie_regs *regs = pcie_port->regs;

	al_mod_dbg("PCIe %d: Enter LOCAL PIPE Loopback mode\n", pcie_port->port_id);

	al_mod_reg_write32_masked(&regs->port_regs->pipe_loopback_ctrl,
			      1 << PCIE_PORT_PIPE_LOOPBACK_CTRL_PIPE_LB_EN_SHIFT,
			      1 << PCIE_PORT_PIPE_LOOPBACK_CTRL_PIPE_LB_EN_SHIFT);

	al_mod_reg_write32_masked(&regs->port_regs->port_link_ctrl,
			      1 << PCIE_PORT_LINK_CTRL_LB_EN_SHIFT,
			      1 << PCIE_PORT_LINK_CTRL_LB_EN_SHIFT);

	return 0;
}

/**
 * @brief exit local pipe loopback mode
 *
 * @param pcie_port	pcie port handle
 * @return		0 if no error found
 */
int al_mod_pcie_local_pipe_loopback_exit(struct al_mod_pcie_port *pcie_port)
{
	struct al_mod_pcie_regs *regs = pcie_port->regs;

	al_mod_dbg("PCIe %d: Exit LOCAL PIPE Loopback mode\n", pcie_port->port_id);

	al_mod_reg_write32_masked(&regs->port_regs->pipe_loopback_ctrl,
			      1 << PCIE_PORT_PIPE_LOOPBACK_CTRL_PIPE_LB_EN_SHIFT,
			      0);

	al_mod_reg_write32_masked(&regs->port_regs->port_link_ctrl,
			      1 << PCIE_PORT_LINK_CTRL_LB_EN_SHIFT,
			      0);
	return 0;
}

/** enter remote loopback mode */
int al_mod_pcie_remote_loopback_enter(struct al_mod_pcie_port *pcie_port)
{
	struct al_mod_pcie_regs *regs = pcie_port->regs;

	al_mod_dbg("PCIe %d: Enter REMOTE Loopback mode\n", pcie_port->port_id);

	al_mod_reg_write32_masked(&regs->port_regs->port_link_ctrl,
			      1 << PCIE_PORT_PIPE_LOOPBACK_CTRL_PIPE_LB_EN_SHIFT,
			      1 << PCIE_PORT_PIPE_LOOPBACK_CTRL_PIPE_LB_EN_SHIFT);

	return 0;
}

/**
 * @brief   exit remote loopback mode
 *
 * @param   pcie_port pcie port handle
 * @return  0 if no error found
 */
int al_mod_pcie_remote_loopback_exit(struct al_mod_pcie_port *pcie_port)
{
	struct al_mod_pcie_regs *regs = pcie_port->regs;

	al_mod_dbg("PCIe %d: Exit REMOTE Loopback mode\n", pcie_port->port_id);

	al_mod_reg_write32_masked(&regs->port_regs->port_link_ctrl,
			      1 << PCIE_PORT_LINK_CTRL_LB_EN_SHIFT,
			      0);
	return 0;
}

static const char *al_mod_pcie_interrupt_forwarding_unit_to_str(enum al_mod_pcie_int_fwd_unit unit)
{
	switch (unit) {
	case AL_PCIE_INT_FWD_UNIT_ETH:
		return "Ethernet";
	default:
		return "unknown unit";
	}
}

static void al_mod_pcie_interrupt_forwarding_eth_registers(struct al_mod_pcie_port *pcie_port,
	int pf_num, int unit_sub_index, uint32_t **inta_a, uint32_t **inta_b,
	uint32_t *mask_a, uint32_t *mask_b)
{
	struct al_mod_pcie_regs *regs = pcie_port->regs;
	struct al_mod_pcie_reg_ptrs_app_soc_int *soc_int = &regs->app.soc_int[pf_num];

	al_mod_assert(unit_sub_index < AL_PCIE_INT_FWD_UNIT_ETH_IDX_MAX);

	*mask_a = 0;
	*mask_b = 0;

	switch (unit_sub_index) {
	case 0:
		*inta_a = soc_int->mask_inta_leg_0;
		*mask_a = PCIE_INTERRUPT_FORWARDING_ETH_0_MASK;
		break;
	case 1:
		*inta_a = soc_int->mask_inta_leg_0;
		*mask_a = PCIE_INTERRUPT_FORWARDING_ETH_1_MASK_A;
		*inta_b = soc_int->mask_inta_leg_1;
		*mask_b = PCIE_INTERRUPT_FORWARDING_ETH_1_MASK_B;
		break;
	case 2:
		*inta_a = soc_int->mask_inta_leg_1;
		*mask_a = PCIE_INTERRUPT_FORWARDING_ETH_2_MASK;
		break;
	case 3:
		*inta_a = soc_int->mask_inta_leg_1;
		*mask_a = PCIE_INTERRUPT_FORWARDING_ETH_3_MASK_A;
		*inta_b = soc_int->mask_inta_leg_2;
		*mask_b = PCIE_INTERRUPT_FORWARDING_ETH_3_MASK_B;
		break;
	case 4:
		*inta_a = soc_int->mask_inta_leg_2;
		*mask_a = PCIE_INTERRUPT_FORWARDING_ETH_4_MASK;
		break;
	case 5:
		*inta_a = soc_int->mask_inta_leg_2;
		*mask_a = PCIE_INTERRUPT_FORWARDING_ETH_5_MASK_A;
		*inta_b = soc_int->mask_inta_leg_3;
		*mask_b = PCIE_INTERRUPT_FORWARDING_ETH_5_MASK_B;
		break;
	}

	return;
}

static void interrupt_forwarding_eth_disable(struct al_mod_pcie_port *pcie_port,
	int pf_num, int unit_sub_index)
{
	uint32_t *inta_reg_a = NULL;
	uint32_t *inta_reg_b = NULL;
	uint32_t mask_a, mask_b;

	/**
	 * Each eth unit has 17 bit wide mask to control interrupt forwarding
	 * 4 bit per UDMA (bits 0:15) and 1 bit for the adapter
	 */

	al_mod_pcie_interrupt_forwarding_eth_registers(pcie_port, pf_num, unit_sub_index,
		&inta_reg_a, &inta_reg_b, &mask_a, &mask_b);

	/** Mask the interrupt ack bits */
	al_mod_reg_write32_masked(inta_reg_a, mask_a, mask_a);
	if (inta_reg_b)
		al_mod_reg_write32_masked(inta_reg_b, mask_b, mask_b);
}

static void interrupt_forwarding_eth_enable(struct al_mod_pcie_port *pcie_port,
	int pf_num, int unit_sub_index)
{
	uint32_t *inta_reg_a = NULL;
	uint32_t *inta_reg_b = NULL;
	uint32_t mask_a, mask_b;

	al_mod_pcie_interrupt_forwarding_eth_registers(pcie_port, pf_num, unit_sub_index,
		&inta_reg_a, &inta_reg_b, &mask_a, &mask_b);

	/** Unmask the interrupt ack bits */
	al_mod_reg_write32_masked(inta_reg_a, mask_a, 0);
	if (inta_reg_b)
		al_mod_reg_write32_masked(inta_reg_b, mask_b, 0);
}

int al_mod_pcie_interrupt_forwarding_disable(struct al_mod_pcie_port *pcie_port, int pf_num,
	enum al_mod_pcie_int_fwd_unit unit, int unit_sub_index)
{
	al_mod_assert(pcie_port);
	al_mod_assert(al_mod_pcie_port_is_enabled(pcie_port));

	switch (unit) {
	case AL_PCIE_INT_FWD_UNIT_ETH:
		interrupt_forwarding_eth_disable(pcie_port, pf_num, unit_sub_index);
		break;
	default:
		al_mod_err("%s: No support for unit %s (%d)\n",
			__func__, al_mod_pcie_interrupt_forwarding_unit_to_str(unit) , unit);
	}

	return 0;
}

int al_mod_pcie_interrupt_forwarding_enable(struct al_mod_pcie_port *pcie_port, int pf_num,
	enum al_mod_pcie_int_fwd_unit unit, int unit_sub_index)
{
	al_mod_assert(pcie_port);
	al_mod_assert(al_mod_pcie_port_is_enabled(pcie_port));

	switch (unit) {
	case AL_PCIE_INT_FWD_UNIT_ETH:
		interrupt_forwarding_eth_enable(pcie_port, pf_num, unit_sub_index);
		break;
	default:
		al_mod_err("%s: No support for unit %s (%d)\n",
			__func__, al_mod_pcie_interrupt_forwarding_unit_to_str(unit) , unit);
	}

	return 0;
}

/********************** Parity Errors ************/
int al_mod_pcie_port_ram_parity_int_config(
	struct al_mod_pcie_port *pcie_port,
	al_mod_bool enable)
{
	struct al_mod_pcie_regs *regs = pcie_port->regs;

	al_mod_reg_write32(regs->app.parity.en_core,
		(enable == AL_TRUE) ? 0xffffffff : 0x0);

	al_mod_reg_write32_masked(&regs->app.int_grp_b->mask,
	      PCIE_W_INT_GRP_B_CAUSE_B_PARITY_ERROR_CORE,
	      (enable != AL_TRUE) ?
	      PCIE_W_INT_GRP_B_CAUSE_B_PARITY_ERROR_CORE : 0);

	return 0;
}

int al_mod_pcie_port_axi_parity_int_config(
	struct al_mod_pcie_port *pcie_port,
	al_mod_bool enable)
{
	struct al_mod_pcie_regs *regs = pcie_port->regs;
	uint32_t parity_enable_value;
	uint32_t parity_enable_mask = 0xffffffff;

	/**
	 * Addressing RMN: 5603
	 *
	 * RMN description:
	 * u4_ram2p signal false parity error
	 *
	 * Software flow:
	 * Disable parity check for this memory
	 */
	if (pcie_port->rev_id >= AL_PCIE_REV_ID_3)
		parity_enable_mask &= ~PCIE_AXI_PARITY_EN_AXI_U4_RAM2P;

	/**
	 * Addressing RMN: 11118
	 *
	 * RMN description:
	 * u5_ram2p signal false parity error
	 *
	 * Software flow:
	 * Disable parity check for this memory
	 */
	if (pcie_port->rev_id == AL_PCIE_REV_ID_3)
		parity_enable_mask &= ~PCIE_AXI_PARITY_AXI_U5_RAM2P;

	al_mod_reg_write32(regs->axi.parity.en_axi,
		       (enable == AL_TRUE) ? parity_enable_mask : 0x0);

	if (pcie_port->rev_id >= AL_PCIE_REV_ID_3) {
		al_mod_reg_write32_masked(regs->axi.ctrl.global,
			PCIE_REV3_4_AXI_CTRL_GLOBAL_PARITY_CALC_EN_MSTR |
			PCIE_REV3_4_AXI_CTRL_GLOBAL_PARITY_ERR_EN_RD |
			PCIE_REV3_4_AXI_CTRL_GLOBAL_PARITY_CALC_EN_SLV |
			PCIE_REV3_4_AXI_CTRL_GLOBAL_PARITY_ERR_EN_WR,
			(enable == AL_TRUE) ?
			PCIE_REV3_4_AXI_CTRL_GLOBAL_PARITY_CALC_EN_MSTR |
			PCIE_REV3_4_AXI_CTRL_GLOBAL_PARITY_ERR_EN_RD |
			PCIE_REV3_4_AXI_CTRL_GLOBAL_PARITY_CALC_EN_SLV |
			PCIE_REV3_4_AXI_CTRL_GLOBAL_PARITY_ERR_EN_WR :
			PCIE_REV3_4_AXI_CTRL_GLOBAL_PARITY_CALC_EN_MSTR |
			PCIE_REV3_4_AXI_CTRL_GLOBAL_PARITY_CALC_EN_SLV);
	} else {
		al_mod_reg_write32_masked(regs->axi.ctrl.global,
			PCIE_REV1_2_AXI_CTRL_GLOBAL_PARITY_CALC_EN_MSTR |
			PCIE_REV1_2_AXI_CTRL_GLOBAL_PARITY_ERR_EN_RD |
			PCIE_REV1_2_AXI_CTRL_GLOBAL_PARITY_CALC_EN_SLV |
			PCIE_REV1_2_AXI_CTRL_GLOBAL_PARITY_ERR_EN_WR,
			(enable == AL_TRUE) ?
			PCIE_REV1_2_AXI_CTRL_GLOBAL_PARITY_CALC_EN_MSTR |
			PCIE_REV1_2_AXI_CTRL_GLOBAL_PARITY_ERR_EN_RD |
			PCIE_REV1_2_AXI_CTRL_GLOBAL_PARITY_CALC_EN_SLV |
			PCIE_REV1_2_AXI_CTRL_GLOBAL_PARITY_ERR_EN_WR :
			PCIE_REV1_2_AXI_CTRL_GLOBAL_PARITY_CALC_EN_MSTR |
			PCIE_REV1_2_AXI_CTRL_GLOBAL_PARITY_CALC_EN_SLV);
	}

	parity_enable_mask = (PCIE_AXI_INT_GRP_A_CAUSE_PARITY_ERR_DATA_PATH_RD |
			      PCIE_AXI_INT_GRP_A_CAUSE_PARITY_ERR_OUT_ADDR_RD |
			      PCIE_AXI_INT_GRP_A_CAUSE_PARITY_ERR_OUT_ADDR_WR |
			      PCIE_AXI_INT_GRP_A_CAUSE_PARITY_ERR_OUT_DATA_WR |
			      PCIE_AXI_INT_GRP_A_CAUSE_PARITY_ERROR_AXI);

	if (!enable)
		parity_enable_value = parity_enable_mask;
	else {
		parity_enable_value = 0;

		/**
		 * Addressing RMN: 11096
		 *
		 * RMN description:
		 * False indication of AXI slave data parity interrupt on V1
		 *
		 * Software flow:
		 * mask and ignore this interrupt
		 */
		if (pcie_port->rev_id == AL_PCIE_REV_ID_1)
			parity_enable_value |= PCIE_AXI_INT_GRP_A_CAUSE_PARITY_ERR_OUT_DATA_WR;

	}

	al_mod_reg_write32_masked(&regs->axi.int_grp_a->mask,
			      parity_enable_mask,
			      parity_enable_value);

	return 0;
}

void al_mod_pcie_core_parity_stats_get(struct al_mod_pcie_port *pcie_port,
				   struct al_mod_pcie_core_parity_stats *stats)
{
	uint32_t status;

	al_mod_assert(pcie_port);
	al_mod_assert(stats);

	al_mod_memset(stats, 0, sizeof(*stats));

	/* this clears all turned on bits */
	status = al_mod_reg_read32(pcie_port->regs->app.parity.status_core);

	if (status) {
		stats->num_of_errors = al_mod_popcount(status & AL_BIT_MASK(13));

		if (pcie_port->rev_id >= AL_PCIE_REV_ID_3) {
			if (status & PCIE_W_V3_CORE_PARITY_CORE_RAM_1P_RBUF)
				stats->ram_1p_rbuf = AL_TRUE;
			if (status & PCIE_W_V3_CORE_PARITY_CORE_RAM_2P_SOTBUF)
				stats->ram_2p_sotbuf = AL_TRUE;
			if (status & PCIE_W_V3_CORE_PARITY_CORE_U0_RAM_RADM_QBUFFER_HDR)
				stats->u0_ram_radm_qbuffer_hdr = AL_TRUE;
			if (status & PCIE_W_V3_CORE_PARITY_CORE_U3_RAM_RADM_QBUFFER_DATA_0)
				stats->u3_ram_radm_qbuffer_data_0 = AL_TRUE;
			if (status & PCIE_W_V3_CORE_PARITY_CORE_U3_RAM_RADM_QBUFFER_DATA_1)
				stats->u3_ram_radm_qbuffer_data_1 = AL_TRUE;
			if (status & PCIE_W_V3_CORE_PARITY_CORE_U10_RAM2P_0)
				stats->u10_ram2p_0 = AL_TRUE;
			if (status & PCIE_W_V3_CORE_PARITY_CORE_U10_RAM2P_1)
				stats->u10_ram2p_1 = AL_TRUE;
			if (status & PCIE_W_V3_CORE_PARITY_CORE_U8_RAM2P)
				stats->u8_ram2p = AL_TRUE;
			if (status & PCIE_W_V3_CORE_PARITY_CORE_U7_RAM2P)
				stats->u7_ram2p = AL_TRUE;
			if (status & PCIE_W_V3_CORE_PARITY_CORE_U6_RAM)
				stats->u6_ram = AL_TRUE;
			if (status & PCIE_W_V3_CORE_PARITY_CORE_U11_RAM2P)
				stats->u11_ram2p = AL_TRUE;
			if (status & PCIE_W_V3_CORE_PARITY_CORE_U1_RAM2P)
				stats->u1_ram2p = AL_TRUE;
			if (status & PCIE_W_V3_CORE_PARITY_CORE_U0_RAM2P)
				stats->u0_ram2p = AL_TRUE;
		} else {
			if (status & PCIE_W_V1_V2_CORE_PARITY_CORE_U_RAM_1P_SOTBUF)
				stats->u_ram_1p_sotbuf = AL_TRUE;
			if (status & PCIE_W_V1_V2_CORE_PARITY_CORE_U0_RAM_RADM_QBUFFER)
				stats->u0_ram_radm_qbuffer = AL_TRUE;
			if (status & PCIE_W_V1_V2_CORE_PARITY_CORE_U3_QBUFFER_0)
				stats->u3_qbuffer_0 = AL_TRUE;
			if (status & PCIE_W_V1_V2_CORE_PARITY_CORE_U3_QBUFFER_1)
				stats->u3_qbuffer_1 = AL_TRUE;
			if (status & PCIE_W_V1_V2_CORE_PARITY_CORE_U9_DECOMP)
				stats->u9_decomp = AL_TRUE;
			if (status & PCIE_W_V1_V2_CORE_PARITY_CORE_U8_RAM2P)
				stats->u8_ram2p = AL_TRUE;
			if (status & PCIE_W_V1_V2_CORE_PARITY_CORE_U7_RAM2P)
				stats->u7_ram2p = AL_TRUE;
			if (status & PCIE_W_V1_V2_CORE_PARITY_CORE_U6_RAM2P)
				stats->u6_ram2p = AL_TRUE;
			if (status & PCIE_W_V1_V2_CORE_PARITY_CORE_U11_RAM2P)
				stats->u11_ram2p = AL_TRUE;
			if (status & PCIE_W_V1_V2_CORE_PARITY_CORE_U1_RAM2P)
				stats->u1_ram2p = AL_TRUE;
			if (status & PCIE_W_V1_V2_CORE_PARITY_CORE_U0_RAM2P)
				stats->u0_ram2p = AL_TRUE;
			if (status & PCIE_W_V1_V2_CORE_PARITY_CORE_U0_RBUF)
				stats->u0_rbuf = AL_TRUE;
			if (status & PCIE_W_V1_V2_CORE_PARITY_CORE_U3_QBUFFER_2)
				stats->u3_qbuffer_2 = AL_TRUE;
		}
	}
}

void al_mod_pcie_axi_parity_stats_get(struct al_mod_pcie_port *pcie_port,
				  struct al_mod_pcie_axi_parity_stats *stats)
{
	uint32_t status, mask;

	al_mod_assert(pcie_port);
	al_mod_assert(stats);

	al_mod_memset(stats, 0, sizeof(*stats));

	/* this clears all turned on bits */
	status = al_mod_reg_read32(pcie_port->regs->axi.parity.status_axi);

	if (status) {
		mask = (PCIE_AXI_PARITY_AXI_U5_RAM2P |
			PCIE_AXI_PARITY_AXI_U4_RAM2P |
			PCIE_AXI_PARITY_AXI_U3_RAM2P);

		if (status & PCIE_AXI_PARITY_AXI_U5_RAM2P)
			stats->u5_ram2p = AL_TRUE;
		if (status & PCIE_AXI_PARITY_AXI_U4_RAM2P)
			stats->u4_ram2p = AL_TRUE;
		if (status & PCIE_AXI_PARITY_AXI_U3_RAM2P)
			stats->u3_ram2p = AL_TRUE;

		switch (pcie_port->rev_id) {
		case AL_PCIE_REV_ID_3:
			mask |= PCIE_V3_AXI_PARITY_AXI_U2_RAM2P;

			stats->num_of_errors = al_mod_popcount(status & mask);

			if (status & PCIE_V3_AXI_PARITY_AXI_U2_RAM2P)
				stats->u2_ram2p = AL_TRUE;
			break;
		case AL_PCIE_REV_ID_2:
		case AL_PCIE_REV_ID_1:
			mask |= (PCIE_V1_V2_AXI_PARITY_AXI_U2_RAM2P |
				 PCIE_V1_V2_AXI_PARITY_AXI_U10_RAM2P |
				 PCIE_V1_V2_AXI_PARITY_AXI_U12_RAM2P);

			stats->num_of_errors = al_mod_popcount(status & mask);

			if (status & PCIE_V1_V2_AXI_PARITY_AXI_U2_RAM2P)
				stats->u2_ram2p = AL_TRUE;
			if (status & PCIE_V1_V2_AXI_PARITY_AXI_U10_RAM2P)
				stats->u10_ram2p = AL_TRUE;
			if (status & PCIE_V1_V2_AXI_PARITY_AXI_U12_RAM2P)
				stats->u12_ram2p = AL_TRUE;
			break;
		default:
			al_mod_err("%s: unsupported PCIe revision ID %d\n",
				__func__,
				pcie_port->rev_id);
			al_mod_assert(0);
		}
	}
}

int al_mod_pcie_ltssm_state_get(
	struct al_mod_pcie_port *pcie_port,
	uint8_t *ltssm_state,
	al_mod_bool skip_port_enabled_check)
{
	if (skip_port_enabled_check != AL_TRUE) {
		if (!al_mod_pcie_port_is_enabled(pcie_port)) {
			al_mod_err("PCIe %d: not enabled, cannot get ltssm state\n",
				pcie_port->port_id);
			return -EINVAL;
		}
	}

	al_mod_pcie_check_link(pcie_port, ltssm_state);

	return 0;
}

uint64_t al_mod_pcie_axi_pos_error_addr_get(struct al_mod_pcie_port *pcie_port)
{
	uint32_t addr_low, addr_high;
	uint64_t addr;

	al_mod_assert(pcie_port);

	addr_low = al_mod_reg_read32(pcie_port->regs->axi.pos_logged.error_low);
	addr_high = al_mod_reg_read32(pcie_port->regs->axi.pos_logged.error_high);

	/*
	 * V1 address:
	 * error_low [31:0] => addr[31:0]
	 * error_high[11:0] => addr[44:32]
	 *
	 * V2/V3 address:
	 * error_low [31:0] => addr[45:14]
	 * error_high[21:0] => addr[67:46]
	 * NOTE: we ignore [67:64] as it fabric target ID
	 */
	switch (pcie_port->rev_id) {
	case AL_PCIE_REV_ID_1:
		addr = addr_low;
		addr |= (uint64_t)(addr_high & AL_FIELD_MASK(11, 0)) << 32;
		break;
	case AL_PCIE_REV_ID_2:
	case AL_PCIE_REV_ID_3:
		addr = (uint64_t)addr_low << 14;
		addr |= (uint64_t)(addr_high & AL_FIELD_MASK(17, 0)) << 46;
		break;
	default:
		al_mod_err("%s: unsupported PCIE revision ID %d\n", __func__, pcie_port->rev_id);
		addr = 0;
	}

	return addr;
}

uint64_t al_mod_pcie_axi_read_data_parity_error_addr_get(struct al_mod_pcie_port *pcie_port)
{
	uint64_t addr;

	al_mod_assert(pcie_port);

	addr = al_mod_reg_read32(pcie_port->regs->axi.rd_parity.log_low);
	addr |= ((uint64_t)al_mod_reg_read32(pcie_port->regs->axi.rd_parity.log_high)) << 32;

	return addr;
}

uint64_t al_mod_pcie_axi_read_compl_error_addr_get(struct al_mod_pcie_port *pcie_port)
{
	uint64_t addr;

	al_mod_assert(pcie_port);

	addr = al_mod_reg_read32(pcie_port->regs->axi.rd_cmpl.cmpl_log_low);
	addr |= ((uint64_t)al_mod_reg_read32(pcie_port->regs->axi.rd_cmpl.cmpl_log_high)) << 32;

	return addr;
}

uint64_t al_mod_pcie_axi_read_to_error_addr_get(struct al_mod_pcie_port *pcie_port)
{
	uint64_t addr;

	al_mod_assert(pcie_port);

	addr = al_mod_reg_read32(pcie_port->regs->axi.rd_to.to_log_low);
	addr |= ((uint64_t)al_mod_reg_read32(pcie_port->regs->axi.rd_to.to_log_high)) << 32;

	return addr;
}

uint64_t al_mod_pcie_axi_write_compl_error_addr_get(struct al_mod_pcie_port *pcie_port)
{
	uint64_t addr;

	al_mod_assert(pcie_port);

	addr = al_mod_reg_read32(pcie_port->regs->axi.wr_cmpl.wr_cmpl_log_low);
	addr |= ((uint64_t)al_mod_reg_read32(pcie_port->regs->axi.wr_cmpl.wr_cmpl_log_high)) << 32;

	return addr;
}

uint64_t al_mod_pcie_axi_write_to_error_addr_get(struct al_mod_pcie_port *pcie_port)
{
	uint64_t addr;

	al_mod_assert(pcie_port);

	addr = al_mod_reg_read32(pcie_port->regs->axi.wr_to.wr_to_log_low);
	addr |= ((uint64_t)al_mod_reg_read32(pcie_port->regs->axi.wr_to.wr_to_log_high)) << 32;

	return addr;
}
