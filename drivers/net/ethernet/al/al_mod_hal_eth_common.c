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

#include "al_mod_hal_common.h"
#include "al_mod_hal_eth.h"
#include "al_mod_hal_eth_common.h"
#include "al_mod_hal_eth_common_regs.h"
#include "al_mod_hal_eth_mac_v4_common_regs.h"
#include "al_mod_hal_eth_shared_resource_regs.h"
#include "al_mod_hal_eth_ec_regs.h"

#define AL_ETH_COMMON_UDMA_NUM		4
#define AL_ETH_COMMON_EC_NUM		4
#define AL_ETH_COMMON_PORT_NUM		4
#define AL_ETH_COMMON_ADAPTER_FUNC_NUM	4

/** Ethernet bus width (aka as beat) */
#define AL_ETH_COMMON_BUS_WIDTH 32
/** Ethernet bus max packet size */
#define AL_ETH_COMMON_FIFO_SIZE_IN_BEATS 1024

#define AL_ETH_COMMON_DATA_RXF_SIZE		20480	/* 20KB */
#define AL_ETH_COMMON_DATA_RXF_SIZE_CLEAR_MASK	AL_BIT_MASK(11)	/* 2KB */

#define AL_ETH_COMMON_INFO_RXF_SIZE		10240	/* 10KB */
#define AL_ETH_COMMON_INFO_RXF_SIZE_CLEAR_MASK	AL_BIT_MASK(9)	/* 0.5 KB */

#define AL_ETH_UDMA_CONVERSION_TABLE_SIZE       512
#define AL_ETH_UDMA2UDMA_MAP_VAL_NA		4

#define AL_ETH_COMMON_FEC_ENABLE_MAC_MODE_XLG_LL_25G 0x00000001
#define AL_ETH_COMMON_FEC_ENABLE_MAC_MODE_XLG_LL_50G 0x00000003
#define AL_ETH_COMMON_FEC_ENABLE_MAC_MODE_CG_100G 0x0000000F
#define AL_ETH_COMMON_FEC_ENABLE_MAC_MODE_XLG_LL_40G 0x0000000F

#define ETH_MAC_V4_COMMON_GEN_V4_LANE(idx) (1 << (idx))

/*
 * The CAM TMG_MODE mapping is {SM[5:0],SMEN,MAW,MAR,MAEN,RM[1:0],RMEN}.
 * 'h4 = RM=10, RMEN=0. Default.
 * 'h1 = RM=00, RMEN=1 (Vmin)
 * 'h3 = RM=01, RMEN=1 (Slow)
 * 'h5 = RM=10, RMEN=1 (Default)
 * 'h7 = RM=11, RMEN=1 (Fast)
 *
 * As result of the ceiling issue (electrical problem) of the TPR's TCAM,
 * it was approved by AVG to configure RMEN=0x1, RM=0x3.
 */
#define AL_ETH_COMMON_TCAM_TMG_MODE_CORE_MEM_HDCAM_VAL	0x7

/* Adapter helper macros */
#define SEL_ADAPTER_BITS			2

#define SEL_ADAPTER_MASK(adapter) \
	(AL_FIELD_MASK(\
			SEL_ADAPTER_BITS * ((adapter) + 1) - 1,\
			SEL_ADAPTER_BITS * (adapter)))

#define SEL_ADAPTER_SHIFT(adapter)	(SEL_ADAPTER_BITS * (adapter))

/* EC/UDMA selection helper macros */
#define SEL_MASK(name, idx, suffix)	(name##_##idx##suffix##_MASK)

#define SEL_VAL(name, idx, suffix, val)	\
	(((val) << (name##_##idx##suffix##_SHIFT)) & SEL_MASK(name, idx, suffix))

#define SEL_4_MASK_SUFFIX(name, suffix)	\
	((SEL_MASK(name, 0, suffix)) \
	| (SEL_MASK(name, 1, suffix)) \
	| (SEL_MASK(name, 2, suffix)) \
	| (SEL_MASK(name, 3, suffix)))

#define SEL_4_VAL_SUFFIX(name, suffix, udma0, udma1, udma2, udma3)	\
	((SEL_VAL(name, 0, suffix, udma0)) \
	| (SEL_VAL(name, 1, suffix, udma1)) \
	| (SEL_VAL(name, 2, suffix, udma2)) \
	| (SEL_VAL(name, 3, suffix, udma3)))

#define SEL_4_MASK(name)	SEL_4_MASK_SUFFIX(name, /* Nothing */)

#define SEL_4_VAL(name, udma0, udma1, udma2, udma3)	\
	SEL_4_VAL_SUFFIX(name, , udma0, udma1, udma2, udma3)

#define SEL_4_VAL_ARR_SUFFIX(name, suffix, udma_arr)	\
	(SEL_4_VAL_SUFFIX(name, suffix, \
			udma_arr[0], udma_arr[1], udma_arr[2], udma_arr[3]))

#define SEL_4_VAL_ARR(name, udma_arr)	\
	SEL_4_VAL_ARR_SUFFIX(name, /* Nothing */, udma_arr)

/** FEC */
#define ETH_MAC_GEN_V4_RS_FEC_CONTROL_CHAN0_ADDR	PCS_ADDR(0x00000000)
#define ETH_MAC_GEN_V4_RS_FEC_ENABLE			AL_BIT(2)

/** General Storage Params
 *
 * This can be used to save dynamic state information (similar to board_params
 * which exist in in al_mod_hal_eth_main.c and are used on a per ETH unit basis)
 */
/** Total number of scratch regs available for board params */
#define AL_ETH_COMMON_GEN_STORAGE_PARAMS_SCRATCH_REGS_NUM		1
/** Params in scratch_pad1 */
#define AL_ETH_COMMON_GEN_STORAGE_PARAMS_ETH_FUNC_PROBE_STATE_MASK	AL_FIELD_MASK(31, 0)
#define AL_ETH_COMMON_GEN_STORAGE_PARAMS_ETH_FUNC_PROBE_STATE_SHIFT	0
#define AL_ETH_COMMON_GEN_STORAGE_PARAMS_ETH_FUNC_PROBE_STATE_REG_IDX	0

/** Common mode to str */
const char *al_mod_eth_common_mode_to_str(enum al_mod_eth_common_mode mode)
{
	switch (mode) {
	/** 4 x 25G */
	case AL_ETH_COMMON_MODE_4X25G:
		return "4x25G";
	/** 2 x 50G */
	case AL_ETH_COMMON_MODE_2X50G:
		return "2x50G";
	/** 1 x 100G */
	case AL_ETH_COMMON_MODE_1X100G:
		return "1x100G";
	/** 1 x 100G Aggregated */
	case AL_ETH_COMMON_MODE_1X100G_AGG:
		return "4x100G Aggregate";
	/** 1 x 40G */
	case AL_ETH_COMMON_MODE_1X40G:
		return "1x40G";
	/** 1 x 50G, 2 x 25G */
	case AL_ETH_COMMON_MODE_1X50G_2X25G:
		return "1x50G + 2x25G";
	/** 2 x 25G, 2 x 10G */
	case AL_ETH_COMMON_MODE_2X25G_2X10G:
		return "2x25G + 2x10G";
	/** 4 x 10G */
	case AL_ETH_COMMON_MODE_4X10G:
		return "4x10G";
	default:
		break;
	}

	return "N/A";
}

unsigned int al_mod_eth_common_mode_num_adapters_get(enum al_mod_eth_common_mode mode)
{
	switch (mode) {
	case AL_ETH_COMMON_MODE_4X10G:
	case AL_ETH_COMMON_MODE_4X25G:
	case AL_ETH_COMMON_MODE_2X25G_2X10G:
		return 4;
	case AL_ETH_COMMON_MODE_2X50G:
		return 2;
	case AL_ETH_COMMON_MODE_1X100G:
	case AL_ETH_COMMON_MODE_1X100G_AGG:
	case AL_ETH_COMMON_MODE_1X40G:
		return 1;
	case AL_ETH_COMMON_MODE_1X50G_2X25G:
		return 3;
	default:
		break;
	}

	al_mod_err("%s: Unknown common mode (%d), Cannot get num of adapters\n", __func__, mode);

	return 0;
}

/** Handle init */
void al_mod_eth_common_handle_init(struct al_mod_eth_common_handle *handle,
	struct al_mod_eth_common_handle_init_params *handle_init_params)
{
	al_mod_assert(handle);
	al_mod_assert(handle_init_params);
	al_mod_assert(handle_init_params->eth_common_regs_base);
	/* eth_ec_0_regs_base shouldn't be asserted, Mainly for 2 reasons:
	 * - Breaks backward comaptibility
	 * - It isn't required for most of the APIs provided (Future APIs which
	 *   will use it must assert)
	 */

	handle->eth_common_regs_base = handle_init_params->eth_common_regs_base;
	handle->eth_ec_0_regs_base = handle_init_params->eth_ec_0_regs_base;
}

void al_mod_eth_common_gen_storage_params_set(const struct al_mod_eth_common_handle *handle,
					  const struct al_mod_eth_common_gen_storage_params *params)
{
	struct al_mod_eth_common_regs __iomem *regs_base;
	uint32_t scratch_pad_regs[AL_ETH_COMMON_GEN_STORAGE_PARAMS_SCRATCH_REGS_NUM] = {0};
	uint32_t *scratch_pad_ptr[AL_ETH_COMMON_GEN_STORAGE_PARAMS_SCRATCH_REGS_NUM];
	int i;

	al_mod_assert(handle);
	al_mod_assert(params);
	al_mod_assert(handle->eth_common_regs_base);

	regs_base = handle->eth_common_regs_base;
	scratch_pad_ptr[0] = &regs_base->gen.scratch_pad_1;

	AL_REG_FIELD_SET(scratch_pad_regs[AL_ETH_COMMON_GEN_STORAGE_PARAMS_ETH_FUNC_PROBE_STATE_REG_IDX],
			 AL_ETH_COMMON_GEN_STORAGE_PARAMS_ETH_FUNC_PROBE_STATE_MASK,
			 AL_ETH_COMMON_GEN_STORAGE_PARAMS_ETH_FUNC_PROBE_STATE_SHIFT,
			 params->eth_func_probe_state);

	/** Write scratch regs to hw */
	for (i = 0; i < AL_ETH_COMMON_GEN_STORAGE_PARAMS_SCRATCH_REGS_NUM; i++)
		al_mod_reg_write32(scratch_pad_ptr[i], scratch_pad_regs[i]);
}

void al_mod_eth_common_gen_storage_params_get(const struct al_mod_eth_common_handle *handle,
					  struct al_mod_eth_common_gen_storage_params *params)
{
	struct al_mod_eth_common_regs __iomem *regs_base;
	uint32_t scratch_pad_regs[AL_ETH_COMMON_GEN_STORAGE_PARAMS_SCRATCH_REGS_NUM] = {0};
	uint32_t *scratch_pad_ptr[AL_ETH_COMMON_GEN_STORAGE_PARAMS_SCRATCH_REGS_NUM];
	int i;

	al_mod_assert(handle);
	al_mod_assert(params);
	al_mod_assert(handle->eth_common_regs_base);

	regs_base = handle->eth_common_regs_base;
	scratch_pad_ptr[0] = &regs_base->gen.scratch_pad_1;

	/** Read scratch regs from hw */
	for (i = 0; i < AL_ETH_COMMON_GEN_STORAGE_PARAMS_SCRATCH_REGS_NUM; i++)
		scratch_pad_regs[i] = al_mod_reg_read32(scratch_pad_ptr[i]);

	params->eth_func_probe_state = AL_REG_FIELD_GET(
		scratch_pad_regs[AL_ETH_COMMON_GEN_STORAGE_PARAMS_ETH_FUNC_PROBE_STATE_REG_IDX],
		AL_ETH_COMMON_GEN_STORAGE_PARAMS_ETH_FUNC_PROBE_STATE_MASK,
		AL_ETH_COMMON_GEN_STORAGE_PARAMS_ETH_FUNC_PROBE_STATE_SHIFT);
}

/* Helper functions */
static void apb_mux_config(struct al_mod_eth_common_regs *regs_base)
{
	/**
	 * APB MUX configuration matrix
	 *
	 * One static configuration to support both internal and external addressing modes
	 * for all supported Ethernet modes.
	 *
	 * Row - UDMA, Column - Adapter, Value - Adapter BAR
	 * Column shouldn't have same value twice.
	 *
	 * Internal in all scenarios:
	 * - Adapter 0 takes UDMA 0 via BAR 0 (Cell 0, 0)
	 * - Adapter 1 takes UDMA 1 via BAR 0 (Cell 1, 1)
	 * - Adapter 2 takes UDMA 2 via BAR 0 (Cell 2, 2)
	 * - Adapter 3 takes UDMA 3 via BAR 0 (Cell 3, 3)
	 *
	 * External in 1x100G / 1x40G scenario:
	 * - Adapter 0 takes all UDMAs 0-3 via BARs 0-3 (Column 0)
	 *
	 * External in 2x50G scenario:
	 * - Adapter 0 takes UDMAs 0-1 via BARs 0-1 (Cell 0, 0 & Cell 1, 0)
	 * - Adapter 2 takes UDMAs 2-3 via BARs 0-1 (Cell 2, 2 & Cell 3, 2)
	 *
	 * External in 4x25G scenario:
	 * - Adapter 0 takes UDMA 0 via BAR 0 (Cell 0, 0)
	 * - Adapter 1 takes UDMA 1 via BAR 0 (Cell 1, 1)
	 * - Adapter 2 takes UDMA 2 via BAR 0 (Cell 2, 2)
	 * - Adapter 3 takes UDMA 3 via BAR 0 (Cell 3, 3)
	 *
	 * External in 1x50G & 2x25G scenario:
	 * - Adapter 0 takes UDMAs 0-1 via BARs 0-1 (Cell 0, 0 & Cell 1, 0)
	 * - Adapter 2 takes UDMA 2 via BAR 0 (Cell 2, 2)
	 * - Adapter 3 takes UDMA 3 via BAR 0 (Cell 3, 3)
	 */
	uint8_t mux_matrix[] = {
		/* Adapter 0 | 1 | 2 | 3 */
		/* UDMA 0 */
		0, 1, 2, 3,
		/* UDMA 1 */
		1, 0, 3, 2,
		/* UDMA 2 */
		2, 3, 0, 1,
		/* UDMA 3 */
		3, 2, 1, 0
	};
	uint8_t apb_mux[AL_ETH_COMMON_UDMA_NUM] = {0};
	unsigned int i;

	al_mod_assert(sizeof(mux_matrix) == (AL_ETH_COMMON_ADAPTER_NUM * AL_ETH_COMMON_UDMA_NUM));

	for (i = 0; i < AL_ETH_COMMON_UDMA_NUM; i++) {
		AL_REG_FIELD_SET(apb_mux[i], SEL_ADAPTER_MASK(0), SEL_ADAPTER_SHIFT(0),
			mux_matrix[i * AL_ETH_COMMON_ADAPTER_NUM]);
		AL_REG_FIELD_SET(apb_mux[i], SEL_ADAPTER_MASK(1), SEL_ADAPTER_SHIFT(1),
			mux_matrix[i * AL_ETH_COMMON_ADAPTER_NUM + 1]);
		AL_REG_FIELD_SET(apb_mux[i], SEL_ADAPTER_MASK(2), SEL_ADAPTER_SHIFT(2),
			mux_matrix[i * AL_ETH_COMMON_ADAPTER_NUM + 2]);
		AL_REG_FIELD_SET(apb_mux[i], SEL_ADAPTER_MASK(3), SEL_ADAPTER_SHIFT(3),
			mux_matrix[i * AL_ETH_COMMON_ADAPTER_NUM + 3]);
	}

	al_mod_reg_write32(&regs_base->gen.apb_mux,
			SEL_4_VAL_ARR(ETH_COMMON_GEN_APB_MUX_SEL, apb_mux));
}

static void shared_rx_fifo_config(void __iomem *eth_shared_resource_regs_base,
		unsigned short rxf_parts[AL_ETH_COMMON_EC_NUM], al_mod_bool internal_switching)
{
	struct al_mod_eth_shared_resource_regs __iomem *regs_base =
		(struct al_mod_eth_shared_resource_regs __iomem *) eth_shared_resource_regs_base;
	unsigned short parts = 0;
	unsigned short curr_part = 0;
	uint16_t data_part_size, info_part_size;
	unsigned int i;

	/* Sum the amount of fifos of all of the ECs */
	for (i = 0; i < AL_ETH_COMMON_EC_NUM; i++)
		parts += rxf_parts[i];

	/* In 4x25 mode internal switching isn't availalbe.
	 * If (parts > 3) we are in 4x25
	 * If not, than the amount of fifos is doubled (Original + Loopback)
	 */
	if ((internal_switching == AL_TRUE) && (parts <= 2))
		parts *= 2;
	else
		internal_switching = AL_FALSE;

	data_part_size = AL_ETH_COMMON_DATA_RXF_SIZE / parts;
	info_part_size = AL_ETH_COMMON_INFO_RXF_SIZE / parts;

	al_mod_assert((data_part_size * parts) == AL_ETH_COMMON_DATA_RXF_SIZE);
	al_mod_assert((info_part_size * parts) == AL_ETH_COMMON_INFO_RXF_SIZE);

	for (i = 0; i < AL_ETH_COMMON_EC_NUM; i++) {
		uint16_t depth_base, depth;
		uint32_t reg;

		/* Data FIFO */
		if (rxf_parts[i]) {
			depth_base = curr_part * data_part_size;
			depth = rxf_parts[i] * data_part_size;
		} else {
			depth_base = AL_ETH_COMMON_DATA_RXF_SIZE;
			depth = 0;
		}

		AL_REG_MASK_CLEAR(depth_base, AL_ETH_COMMON_DATA_RXF_SIZE_CLEAR_MASK);
		AL_REG_MASK_CLEAR(depth, AL_ETH_COMMON_DATA_RXF_SIZE_CLEAR_MASK);

		reg = (depth_base << ETH_SHARED_RESOURCE_RXF_DATA_FIFO_1_DEPTH_BASE_SHIFT)
				& ETH_SHARED_RESOURCE_RXF_DATA_FIFO_1_DEPTH_BASE_MASK;
		reg |= (depth << ETH_SHARED_RESOURCE_RXF_DATA_FIFO_1_DEPTH_SHIFT)
				& ETH_SHARED_RESOURCE_RXF_DATA_FIFO_1_DEPTH_MASK;
		al_mod_reg_write32(&regs_base->rxf[i].data_fifo_1, reg);

		/* Info FIFO */
		if (rxf_parts[i]) {
			depth_base = curr_part * info_part_size;
			depth = rxf_parts[i] * info_part_size;
		} else {
			depth_base = AL_ETH_COMMON_INFO_RXF_SIZE;
			depth = 0;
		}

		AL_REG_MASK_CLEAR(depth_base, AL_ETH_COMMON_INFO_RXF_SIZE_CLEAR_MASK);
		AL_REG_MASK_CLEAR(depth, AL_ETH_COMMON_INFO_RXF_SIZE_CLEAR_MASK);

		reg = (depth_base << ETH_SHARED_RESOURCE_RXF_INFO_FIFO_1_DEPTH_BASE_SHIFT)
				& ETH_SHARED_RESOURCE_RXF_INFO_FIFO_1_DEPTH_BASE_MASK;
		reg |= (depth << ETH_SHARED_RESOURCE_RXF_INFO_FIFO_1_DEPTH_SHIFT)
				& ETH_SHARED_RESOURCE_RXF_INFO_FIFO_1_DEPTH_MASK;
		al_mod_reg_write32(&regs_base->rxf[i].info_fifo_1, reg);

		if (internal_switching == AL_TRUE) {
			/* Data FIFO 2 */
			if (rxf_parts[i]) {
				depth_base = (curr_part + 1) * data_part_size;
				depth = rxf_parts[i] * data_part_size;
			} else {
				depth_base = AL_ETH_COMMON_DATA_RXF_SIZE;
				depth = 0;
			}

			AL_REG_MASK_CLEAR(depth_base, AL_ETH_COMMON_DATA_RXF_SIZE_CLEAR_MASK);
			AL_REG_MASK_CLEAR(depth, AL_ETH_COMMON_DATA_RXF_SIZE_CLEAR_MASK);

			reg = (depth_base << ETH_SHARED_RESOURCE_RXF_DATA_FIFO_2_DEPTH_BASE_SHIFT)
					& ETH_SHARED_RESOURCE_RXF_DATA_FIFO_2_DEPTH_BASE_MASK;
			reg |= (depth << ETH_SHARED_RESOURCE_RXF_DATA_FIFO_2_DEPTH_SHIFT)
					& ETH_SHARED_RESOURCE_RXF_DATA_FIFO_2_DEPTH_MASK;
			al_mod_reg_write32(&regs_base->rxf[i].data_fifo_2, reg);

			/* Info FIFO 2 */
			if (rxf_parts[i]) {
				depth_base = (curr_part + 1) * info_part_size;
				depth = rxf_parts[i] * info_part_size;
			} else {
				depth_base = AL_ETH_COMMON_INFO_RXF_SIZE;
				depth = 0;
			}

			AL_REG_MASK_CLEAR(depth_base, AL_ETH_COMMON_INFO_RXF_SIZE_CLEAR_MASK);
			AL_REG_MASK_CLEAR(depth, AL_ETH_COMMON_INFO_RXF_SIZE_CLEAR_MASK);

			reg = (depth_base << ETH_SHARED_RESOURCE_RXF_INFO_FIFO_2_DEPTH_BASE_SHIFT)
					& ETH_SHARED_RESOURCE_RXF_INFO_FIFO_2_DEPTH_BASE_MASK;
			reg |= (depth << ETH_SHARED_RESOURCE_RXF_INFO_FIFO_2_DEPTH_SHIFT)
					& ETH_SHARED_RESOURCE_RXF_INFO_FIFO_2_DEPTH_MASK;
			al_mod_reg_write32(&regs_base->rxf[i].info_fifo_2, reg);
		}

		if (internal_switching == AL_TRUE)
			curr_part += 2*rxf_parts[i];
		else
			curr_part += rxf_parts[i];
	}
}

void al_mod_eth_common_udma_select_config_get(
	void __iomem *regs_base,
	struct al_mod_eth_common_udma_select_params *params)
{
	unsigned int i = 0;
	struct al_mod_ec_regs *ec_0_regs_base = (struct al_mod_ec_regs *)regs_base;

	al_mod_assert(ec_0_regs_base);
	al_mod_assert(params);

	params->udma_select = al_mod_reg_read32(&ec_0_regs_base->rfw_v4.udma_select);

	for (i = 0; i < AL_ARR_SIZE(params->rfw_v4_udma_select); i++) {
		params->rfw_v4_udma_select[i] =
			al_mod_reg_read32(&ec_0_regs_base->rfw_v4_udma_select[i].port_map);
	}
}

void al_mod_eth_common_udma_select_config_set(
	void __iomem *regs_base,
	struct al_mod_eth_common_udma_select_params *params)
{
	unsigned int i = 0;
	struct al_mod_ec_regs *ec_0_regs_base = (struct al_mod_ec_regs *)regs_base;

	al_mod_assert(ec_0_regs_base);
	al_mod_assert(params);

	al_mod_reg_write32(&ec_0_regs_base->rfw_v4.udma_select, params->udma_select);

	for (i = 0; i < AL_ARR_SIZE(params->rfw_v4_udma_select); i++) {
		al_mod_reg_write32(&ec_0_regs_base->rfw_v4_udma_select[i].port_map,
			params->rfw_v4_udma_select[i]);
	}
}

static void al_mod_eth_common_udma_select_config(
	struct al_mod_eth_common_init_params *init_params,
	uint8_t udma2udma_map[AL_ETH_COMMON_PORT_NUM][AL_ETH_COMMON_UDMA_NUM])
{
	uint32_t reg_val = 0;
	unsigned int i;
	struct al_mod_ec_regs *ec_0_regs_base = init_params->eth_ec_0_regs_base;

	if ((init_params->mode == AL_ETH_COMMON_MODE_1X100G) ||
		(init_params->mode == AL_ETH_COMMON_MODE_1X40G)) {
		/** Zero port_map_en, If no need in udma select table */
		al_mod_reg_write32(&ec_0_regs_base->rfw_v4.udma_select, 0);
		return;
	} else {
		al_mod_reg_write32(&ec_0_regs_base->rfw_v4.udma_select,
						EC_RFW_V4_UDMA_SELECT_PORT_MAP_EN);
	}

	for (i = 0; i < AL_ETH_UDMA_CONVERSION_TABLE_SIZE; i++) {
		unsigned int udma_conversion_entry = 0;
		unsigned int port_num;
		unsigned int j;

		/*
		 * source_port is i[1:0] and no LB
		 * lb_port is i[3:2] and LB
		 */
		if ((i & 0x10) == 0)
			port_num = (i & 0x3);
		else
			port_num = ((i >> 2) & 0x3);

		for (j = 0; j < AL_ETH_COMMON_UDMA_NUM; j++)
			if (((i >> (5+j)) & 0x1) == 0x1)
				udma_conversion_entry |= (1 << udma2udma_map[port_num][j]);

		/* 4 bits for entry, 8 entries per register */
		reg_val |= ((udma_conversion_entry & 0xf) << ((i % 8) * 4));
		if ((i % 8) == 7) {
			al_mod_reg_write32(&ec_0_regs_base->rfw_v4_udma_select[i/8].port_map, reg_val);
			reg_val = 0;
		}
	}
}

static void al_mod_eth_common_rmn(struct al_mod_eth_common_init_params *init_params)
{
	unsigned int i;
	struct al_mod_eth_common_regs __iomem *regs_base;
	struct al_mod_eth_shared_resource_regs __iomem *shared_regs_base;

	al_mod_assert(init_params);

	regs_base = (struct al_mod_eth_common_regs __iomem *)
		init_params->eth_common_regs_base;
	shared_regs_base =
		(struct al_mod_eth_shared_resource_regs __iomem *)&regs_base->eth_sr[0];

	/**
	 * Addressing RMN 11457
	 *
	 * RMN description:
	 * EC table access missing ec_id interface for HW accesses
	 * access protection feature isn't working since input ec_id
	 * is not presented as it should be.
	 *
	 * Software flow :
	 * Feature is disabled by default, prevent SW from enabling it
	 */
	for (i = 0; i < AL_ARR_SIZE(shared_regs_base->rfw_table_ctl); i++)
		/** Disable HW protection */
		al_mod_reg_write32_masked(&shared_regs_base->rfw_table_ctl[i].table_cfg,
			ETH_SHARED_RESOURCE_RFW_TABLE_CTL_TABLE_CFG_HW_PROTECTION_EN, 0);
}

static int al_mod_eth_common_adapters_udmas_ecs_init(struct al_mod_eth_common_init_params *init_params)
{
	struct al_mod_eth_common_regs __iomem *regs_base = (struct al_mod_eth_common_regs __iomem *)
		init_params->eth_common_regs_base;
	struct al_mod_eth_shared_resource_regs __iomem *shared_regs_base =
		(struct al_mod_eth_shared_resource_regs __iomem *)&regs_base->eth_sr[0];

	uint8_t udma2ec_tx[AL_ETH_COMMON_UDMA_NUM] = {0};
	uint8_t udma2ec_rx[AL_ETH_COMMON_UDMA_NUM] = {0};
	uint8_t ec_num_of_udmas_mask[AL_ETH_COMMON_EC_NUM] = {0};
	al_mod_bool ec_disabled[AL_ETH_COMMON_EC_NUM] = {0};
	uint8_t udma2adp[AL_ETH_COMMON_UDMA_NUM] = {0};
	uint8_t adp2func[AL_ETH_COMMON_UDMA_NUM] = {0};
	uint8_t adp2func_bar[AL_ETH_COMMON_UDMA_NUM] = {0};
	uint8_t func2adp_bar_oh[AL_ETH_COMMON_UDMA_NUM][AL_ETH_COMMON_UDMA_NUM] = { {0} };
	uint8_t num_of_vf[AL_ETH_COMMON_ADAPTER_NUM] = {0};
	al_mod_bool adapter_exists[AL_ETH_COMMON_ADAPTER_NUM] = {0};
	uint8_t bme_flr_slaves[AL_ETH_COMMON_ADAPTER_NUM] = {0};
	al_mod_bool bme_flr_slaves_override[AL_ETH_COMMON_ADAPTER_NUM] = {0};
	uint8_t udma2udma_map[AL_ETH_COMMON_PORT_NUM][AL_ETH_COMMON_UDMA_NUM];
	unsigned short rxf_part[AL_ETH_COMMON_EC_NUM] = {0};
	unsigned short ec_pause_sel[AL_ETH_COMMON_EC_NUM] = {0};
	unsigned short udma_pause_sel[AL_ETH_COMMON_UDMA_NUM] = {0};
	unsigned short udma_to_ec_int[AL_ETH_COMMON_UDMA_NUM] = {0};
	unsigned int i;
	unsigned int j;
	unsigned int threshold_beats;
	unsigned int max_packet_size_beats;
	uint32_t reg;

	for (i = 0; i < AL_ETH_COMMON_PORT_NUM; i++)
		for (j = 0; j < AL_ETH_COMMON_UDMA_NUM; j++)
			udma2udma_map[i][j] = AL_ETH_UDMA2UDMA_MAP_VAL_NA;

	switch (init_params->mode) {
	case AL_ETH_COMMON_MODE_1X100G:
		/* All UDMAs to adapter 0, each takes a bar.
		 * All UDMAs goes to unit 0
		 * EC 0 (100G) takes all 4 UDMAs */
		for (i = 0; i < AL_ETH_COMMON_UDMA_NUM; i++) {
			/* All UDMAs to EC 0 */
			udma2ec_tx[i] = udma2ec_rx[i] = 0;

			/* EC 0 (only) accepts all UDMAs */
			ec_num_of_udmas_mask[0] |= 1 << i;

			/* All UDMAs to adapter 0 */
			udma2adp[i] = 0;

			adp2func[i] = 0;
			adp2func_bar[i] = i;

			/* Only adapter 0 is relevant */
			func2adp_bar_oh[i][0] = 1 << i;

			/* map udma[i] to udma[i] */
			udma2udma_map[0][i] = i;

			/* Only EC 0 is in use. there are 4 udma's (3 VF's) per EC */
			ec_pause_sel[i] = 0;
			udma_pause_sel[i] = i;
		}

		/* Adapter 0 gets 3 VFs */
		num_of_vf[0] = 3;
		adapter_exists[0] = AL_TRUE;
		/* Disable Unused Ecs */
		ec_disabled[1] = ec_disabled[2] = ec_disabled[3] = AL_TRUE;

		/* Adapters 1-3 are slaves 1-3 to adapter 0 */
		if (init_params->use_all_adapters)
			for (i = 1; i < AL_ETH_COMMON_ADAPTER_NUM; i++) {
				bme_flr_slaves[i] = 0*4 + i;
				bme_flr_slaves_override[i] = AL_TRUE;
			}

		/* EC 0 gets all RX FIFO */
		rxf_part[0] = 1;
		/* UDMA 0 interrupt to EC0 */
		udma_to_ec_int[0] = AL_BIT(0);

		break;
	case AL_ETH_COMMON_MODE_1X100G_AGG:
		/* All UDMAs to adapter 0, each takes a bar.
		 * EC 0-3 takes 1 UDMA each on TX
		 * EC 0 takes 4 UDMAs on RX */
		for (i = 0; i < AL_ETH_COMMON_UDMA_NUM; i++) {
			/* UDMA X to EC X on TX */
			udma2ec_tx[i] = i;
			/* UDMA X from EC 0 on RX */
			udma2ec_rx[i] = 0;

			/* EC X takes 1 UDMA */
			ec_num_of_udmas_mask[i] = 1;

			/* All UDMAs to adapter 0 */
			udma2adp[i] = 0;

			adp2func[i] = 0;
			adp2func_bar[i] = i;

			/* Only adapter 0 is relevant */
			func2adp_bar_oh[i][0] = 1 << i;

			/* map port[j] to udma[j] */
			for (j = 0; j < AL_ETH_COMMON_PORT_NUM; j++)
				udma2udma_map[j][0] = j;

			ec_pause_sel[i] = i;
			udma_pause_sel[i] = 0;
		}

		/* Adapter 0 gets 3 VFs */
		num_of_vf[0] = 3;
		adapter_exists[0] = AL_TRUE;

		/* Adapters 1-3 are slaves 1-3 to adapter 0 */
		if (init_params->use_all_adapters)
			for (i = 1; i < AL_ETH_COMMON_ADAPTER_NUM; i++) {
				bme_flr_slaves[i] = 0*4 + i;
				bme_flr_slaves_override[i] = AL_TRUE;
			}

		/* Each EC gets quarter of RX FIFO */
		for (i = 0; i < AL_ETH_COMMON_EC_NUM; i++)
			rxf_part[i] = 1;

		udma_to_ec_int[0] = AL_BIT(0);
		break;
	case AL_ETH_COMMON_MODE_2X50G:
		/* UDMAs 0-1 to adapter 0, bars 0-1
		 * UDMAs 2-3 to adapter 2, bars 0-1
		 * EC 0/2 takes 2 UDMAs each */

		/* M2S UDMAs 0-1 to EC 0
		 * M2S UDMAs 2-3 to EC 2
		 * S2M UDMAs 0-3 to EC 0 */
		udma2ec_tx[0] = udma2ec_tx[1] = 0;
		udma2ec_tx[2] = udma2ec_tx[3] = 2;
		udma2ec_rx[0] = udma2ec_rx[1] = udma2ec_rx[2] = udma2ec_rx[3] = 0;

		/* EC 0/2 takes 2 UDMAs each */
		ec_num_of_udmas_mask[0] = ec_num_of_udmas_mask[2] = 0x3;

		/* UDMAs 0-1 to adapter 0
		 * UDMAs 2-3 to adapter 2 */
		udma2adp[0] = udma2adp[1] = 0;
		udma2adp[2] = udma2adp[3] = 2;

		adp2func[0] = adp2func[1] = 0;
		adp2func[2] = adp2func[3] = 2;
		adp2func_bar[0] = adp2func_bar[2] = 0;
		adp2func_bar[1] = adp2func_bar[3] = 1;

		/* BAR 0 on adapter 0/2 goes to UDMA 0/2 */
		func2adp_bar_oh[0][0] = 1 << 0;
		func2adp_bar_oh[0][2] = 1 << 2;

		/* BAR 1 on adapter 0/2 goes to UDMA 1/3 */
		func2adp_bar_oh[1][0] = 1 << 1;
		func2adp_bar_oh[1][2] = 1 << 3;

		/* map port 0 to UDMA 0/1 */
		udma2udma_map[0][0] = 0;
		udma2udma_map[0][1] = 1;
		/* map port 2 to UDMA 2/3 */
		udma2udma_map[2][0] = 2;
		udma2udma_map[2][1] = 3;

		/* Adapters 0/2 gets 1 VF */
		num_of_vf[0] = num_of_vf[2] = 1;
		adapter_exists[0] = adapter_exists[2] = AL_TRUE;
		/* Disable Unused Ecs */
		ec_disabled[1] = ec_disabled[3] = AL_TRUE;

		/* Adapters 1/3 are slave 1 to adapters 0/2 */
		if (init_params->use_all_adapters) {
			bme_flr_slaves[1] = 0*4 + 1;
			bme_flr_slaves[3] = 2*4 + 1;
			bme_flr_slaves_override[1] = bme_flr_slaves_override[3] = AL_TRUE;
		}

		/** UDMA's are attached to ports 0 & 2 */
		ec_pause_sel[0] = 0;
		ec_pause_sel[1] = 0;
		ec_pause_sel[2] = 2;
		ec_pause_sel[3] = 2;
		/** EC's has UDMA's 0 & 1 */
		udma_pause_sel[0] = 0;
		udma_pause_sel[1] = 1;

		/* ECs 0/2 gets half of RX FIFO each */
		rxf_part[0] = rxf_part[2] = 1;
		/** UDMA0 & EC0, UDMA2 & EC2 */
		udma_to_ec_int[0] = AL_BIT(0);
		udma_to_ec_int[2] = AL_BIT(2);

		break;
	case AL_ETH_COMMON_MODE_1X40G:
		/* All UDMAs to adapter 0, each takes a bar.
		 * UDMA X goes to unit X
		 * EC 0-3 takes 1 UDMA each */
		for (i = 0; i < AL_ETH_COMMON_UDMA_NUM; i++) {
			/* EC X to UDMA X */
			udma2ec_tx[i] = udma2ec_rx[i] = i;

			/* EC X takes 1 UDMA */
			ec_num_of_udmas_mask[i] = 1;

			/* All UDMAs to adapter 0 */
			udma2adp[i] = 0;

			adp2func[i] = 0;
			adp2func_bar[i] = i;

			/* Only adapter 0 is relevant */
			func2adp_bar_oh[i][0] = 1 << i;

			/* map udma[i] to udma[i] */
			udma2udma_map[0][i] = i;

			/* Only EC 0 is in use. there are 4 udma's (3 VF's) */
			ec_pause_sel[i] = 0;
			udma_pause_sel[i] = i;
		}

		/* Adapter 0 gets 3 VFs */
		num_of_vf[0] = 3;
		adapter_exists[0] = AL_TRUE;
		/* Disable Unused Ecs */
		ec_disabled[1] = ec_disabled[2] = ec_disabled[3] = AL_TRUE;

		/* Adapters 1-3 are slaves 1-3 to adapter 0 */
		if (init_params->use_all_adapters)
			for (i = 1; i < AL_ETH_COMMON_ADAPTER_NUM; i++) {
				bme_flr_slaves[i] = 0*4 + i;
				bme_flr_slaves_override[i] = AL_TRUE;
			}

		/* EC 0 gets all RX FIFO */
		rxf_part[0] = 1;

		udma_to_ec_int[0] = AL_BIT(0);
	break;
	case AL_ETH_COMMON_MODE_4X25G:
		/* UDMA per adapter - UDMA X to adapter X, bar 0
		 * EC 0-3 takes 1 UDMA each
		 * No VFs */
		for (i = 0; i < AL_ETH_COMMON_UDMA_NUM; i++) {
			/* M2S UDMAs X to EC X
			 * S2M UDMAs 0-3 to EC 0 */
			udma2ec_tx[i] = i;
			udma2ec_rx[i] = 0;

			/* EC X takes 1 UDMA */
			ec_num_of_udmas_mask[i] = 1;

			/* UDMA X to adapter X */
			udma2adp[i] = i;

			adp2func[i] = i;
			adp2func_bar[i] = 0;

			/* Only BAR 0 is relevant */
			func2adp_bar_oh[0][i] = 1 << i;

			adapter_exists[i] = AL_TRUE;

			/* map port[j] to udma[j] */
			for (j = 0; j < AL_ETH_COMMON_PORT_NUM; j++)
				udma2udma_map[j][0] = j;

			/* 4 EC's are in use - EC's has only UDMA #0 */
			ec_pause_sel[i] = i;
			udma_pause_sel[i] = 0;

			udma_to_ec_int[i] = AL_BIT(i);
		}

		/* Each EC gets quarter of RX FIFO */
		for (i = 0; i < AL_ETH_COMMON_EC_NUM; i++)
			rxf_part[i] = 1;
	break;
	case AL_ETH_COMMON_MODE_1X50G_2X25G:
		/* UDMAs 0-1 to adapter 0, bars 0-1
		 * UDMAs 2/3 to adapter 2/3, bar 0
		 * EC 0 takes 2 UDMAs each
		 * EC 2/3 takes 1 UDMA each */

		/* M2S UDMAs 0-1 to EC 0
		 * M2S UDMAs 2   to EC 2
		 * M2S UDMAs 3   to EC 3
		 * S2M UDMAs 0-3 to EC 0 */
		udma2ec_tx[0] = udma2ec_tx[1] = 0;
		udma2ec_tx[2] = 2;
		udma2ec_tx[3] = 3;
		udma2ec_rx[0] = udma2ec_rx[1] = udma2ec_rx[2] = udma2ec_rx[3] = 0;

		/* EC 0 takes 2 UDMAs
		 * EC 2/3 takes 1 UDMA each */
		ec_num_of_udmas_mask[0] = 0x3;
		ec_num_of_udmas_mask[2] = ec_num_of_udmas_mask[3] = 1;

		/* UDMAs 0-1 to adapter 0
		 * UDMAs 2/3 to adapter 2/3 */
		udma2adp[0] = udma2adp[1] = 0;
		udma2adp[2] = 2;
		udma2adp[3] = 3;

		adp2func[0] = adp2func[1] = 0;
		adp2func[2] = 2;
		adp2func[3] = 3;
		adp2func_bar[0] = adp2func_bar[2] = adp2func_bar[3] = 0;
		adp2func_bar[1] = 1;

		/* BAR 0 on adapter 0/2/3 goes to UDMA 0/2/3 */
		func2adp_bar_oh[0][0] = 1 << 0;
		func2adp_bar_oh[0][2] = 1 << 2;
		func2adp_bar_oh[0][3] = 1 << 3;

		/* BAR 1 on adapter 0 goes to UDMA 1 */
		func2adp_bar_oh[1][0] = 1 << 1;

		/* map port 0 to UDMA 0/1 */
		udma2udma_map[0][0] = 0;
		udma2udma_map[0][1] = 1;
		/* map port 2 to UDMA 2 */
		udma2udma_map[2][0] = 2;
		udma2udma_map[2][1] = 2;
		/* map port 3 to UDMA 3 */
		udma2udma_map[3][0] = 3;
		udma2udma_map[3][1] = 3;

		udma_pause_sel[0] = 0;
		udma_pause_sel[1] = 0;
		udma_pause_sel[2] = 2;
		udma_pause_sel[3] = 3;
		ec_pause_sel[0] = 0;
		ec_pause_sel[1] = 1;
		ec_pause_sel[2] = 0;
		ec_pause_sel[3] = 0;

		udma_to_ec_int[0] = AL_BIT(0);
		udma_to_ec_int[2] = AL_BIT(2);
		udma_to_ec_int[3] = AL_BIT(3);

		/* Adapters 0 gets 1 VF */
		num_of_vf[0] = 1;

		/* Adapters 0/2/3 exists */
		adapter_exists[0] = adapter_exists[2] = adapter_exists[3] = AL_TRUE;
		/* Disable Unused Ecs */
		ec_disabled[1] = AL_TRUE;

		/* Adapters 1 is slave 1 to adapters 0 */
		if (init_params->use_all_adapters) {
			bme_flr_slaves[1] = 0*4 + 1;
			bme_flr_slaves_override[1] = AL_TRUE;
		}

		/* ECs 0 gets half of RX FIFO each
		 * ECs 2/3 gets quarter of RX FIFO each */
		rxf_part[0] = 2;
		rxf_part[2] = rxf_part[3] = 1;
		break;
	default:
		al_mod_err("%s: Unknown ETH Adapters mode %d\n", __func__, init_params->mode);
		return -EINVAL;
	}

	/* Override UDMA to Unit Adapter assignment,
	 * so each UDMA goes through single Adapter */
	if (init_params->use_all_adapters)
		for (i = 0; i < AL_ETH_COMMON_UDMA_NUM; i++)
			udma2adp[i] = i;

	/* APB (Adapter bar to UDMA) Mux */
	apb_mux_config(regs_base);

	/* UDMA to EC Mux (same for RX & TX) */
	al_mod_reg_write32_masked(&regs_base->gen.ec_udma_axs_mux,
			SEL_4_MASK(ETH_COMMON_GEN_EC_UDMA_AXS_MUX_RX_SEL),
			SEL_4_VAL_ARR(ETH_COMMON_GEN_EC_UDMA_AXS_MUX_RX_SEL, udma2ec_rx));
	al_mod_reg_write32_masked(&regs_base->gen.ec_udma_axs_mux,
			SEL_4_MASK(ETH_COMMON_GEN_EC_UDMA_AXS_MUX_TX_SEL),
			SEL_4_VAL_ARR(ETH_COMMON_GEN_EC_UDMA_AXS_MUX_TX_SEL, udma2ec_tx));

	/* Number of UDMAs per EC */
	al_mod_reg_write32_masked(&regs_base->gen.ec_udma_axs_mux,
			SEL_4_MASK_SUFFIX(ETH_COMMON_GEN_EC_UDMA_AXS_MUX_EC, _UDMA_ENABLE),
			SEL_4_VAL_ARR_SUFFIX(ETH_COMMON_GEN_EC_UDMA_AXS_MUX_EC,
				_UDMA_ENABLE, ec_num_of_udmas_mask));

	/* UDMA to Adapter Mux */
	al_mod_reg_write32(&regs_base->gen.udma_adpt_axi_mux,
			SEL_4_VAL_ARR(ETH_COMMON_GEN_UDMA_ADPT_AXI_MUX_SEL, udma2adp));

	/* Adapter to Func */
	/* MSIX Func mask */
	al_mod_reg_write32(&regs_base->adp2func.msix_func_mask_vec,
			SEL_4_VAL_ARR(ETH_COMMON_ADP2FUNC_MSIX_FUNC_MASK_VEC_SEL, adp2func));
	al_mod_reg_write32(&regs_base->adp2func.msix_func_mask_vec_bar,
			SEL_4_VAL_ARR(ETH_COMMON_ADP2FUNC_MSIX_FUNC_MASK_VEC_BAR_SEL,
				adp2func_bar));

	/* MSIX enable */
	al_mod_reg_write32(&regs_base->adp2func.msix_enable_vec,
			SEL_4_VAL_ARR(ETH_COMMON_ADP2FUNC_MSIX_ENABLE_VEC_SEL, adp2func));
	al_mod_reg_write32(&regs_base->adp2func.msix_enable_vec_bar,
			SEL_4_VAL_ARR(ETH_COMMON_ADP2FUNC_MSIX_ENABLE_VEC_BAR_SEL, adp2func_bar));

	/* FLR */
	al_mod_reg_write32(&regs_base->adp2func.flr,
			SEL_4_VAL_ARR(ETH_COMMON_ADP2FUNC_FLR_SEL, adp2func));
	al_mod_reg_write32(&regs_base->adp2func.flr_bar,
			SEL_4_VAL_ARR(ETH_COMMON_ADP2FUNC_FLR_BAR_SEL, adp2func_bar));

	/* Conf */
	al_mod_reg_write32(&regs_base->adp2func.conf,
			SEL_4_VAL_ARR(ETH_COMMON_ADP2FUNC_CONF_SEL, adp2func));
	al_mod_reg_write32(&regs_base->adp2func.conf_bar,
			SEL_4_VAL_ARR(ETH_COMMON_ADP2FUNC_CONF_BAR_SEL, adp2func_bar));

	/* Int. disable */
	al_mod_reg_write32(&regs_base->adp2func.int_disable_vec,
			SEL_4_VAL_ARR(ETH_COMMON_ADP2FUNC_INT_DISABLE_VEC_SEL, adp2func));
	al_mod_reg_write32(&regs_base->adp2func.int_disable_vec_bar,
			SEL_4_VAL_ARR(ETH_COMMON_ADP2FUNC_INT_DISABLE_VEC_BAR_SEL, adp2func_bar));

	/* APS control */
	al_mod_reg_write32(&regs_base->adp2func.aps_control,
			SEL_4_VAL_ARR(ETH_COMMON_ADP2FUNC_APS_CONTROL_SEL, adp2func));
	al_mod_reg_write32(&regs_base->adp2func.aps_control_bar,
			SEL_4_VAL_ARR(ETH_COMMON_ADP2FUNC_APS_CONTROL_BAR_SEL, adp2func_bar));

	/* Func to Adapter */
	/* Int. vector One-Hot per BAR */
	al_mod_reg_write32(&regs_base->func2adp.int_vec_oh_bar0,
			SEL_4_VAL_ARR(ETH_COMMON_FUNC2ADP_INT_VEC_OH_BAR0_SEL,
				func2adp_bar_oh[0]));
	al_mod_reg_write32(&regs_base->func2adp.int_vec_oh_bar1,
			SEL_4_VAL_ARR(ETH_COMMON_FUNC2ADP_INT_VEC_OH_BAR1_SEL,
				func2adp_bar_oh[1]));
	al_mod_reg_write32(&regs_base->func2adp.int_vec_oh_bar2,
			SEL_4_VAL_ARR(ETH_COMMON_FUNC2ADP_INT_VEC_OH_BAR2_SEL,
				func2adp_bar_oh[2]));
	al_mod_reg_write32(&regs_base->func2adp.int_vec_oh_bar3,
			SEL_4_VAL_ARR(ETH_COMMON_FUNC2ADP_INT_VEC_OH_BAR3_SEL,
				func2adp_bar_oh[3]));

	/* Adapter to Unit */
	/* Unit adapter can disable clock for its EC/UDMA */
	al_mod_reg_write32_masked(&regs_base->adp2unit.clock_disable,
			SEL_4_MASK(ETH_COMMON_ADP2UNIT_CLOCK_DISABLE_SEL),
			SEL_4_VAL_ARR(ETH_COMMON_ADP2UNIT_CLOCK_DISABLE_SEL, udma2ec_tx));

	al_mod_reg_write32_masked(&regs_base->adp2unit.clock_disable,
			ETH_COMMON_ADP2UNIT_CLOCK_DISABLE_EC_MASK,
			((ec_disabled[3] << 3) | (ec_disabled[2] << 2) |
			 (ec_disabled[1] << 1) | (ec_disabled[0] << 0))
			<< ETH_COMMON_ADP2UNIT_CLOCK_DISABLE_EC_SHIFT);

	/**
	 * EC UDMA pause mux
	 * Each EC has an EFC (EC flow control) unit that can stop the TX datapath
	 * This mux selects the right EC & UDMA number pair
	 *
	 * This mux configures two types of behaviours - stop UDMA queue & stop of UDMA stream
	 * For each common mode they are identical
	 */
	reg = 0;
	AL_REG_FIELD_SET(reg,
		ETH_COMMON_GEN_EC_UDMA_PAUSE_MUX_SEL_0_EC_QUEUE_MASK,
		ETH_COMMON_GEN_EC_UDMA_PAUSE_MUX_SEL_0_EC_QUEUE_SHIFT,
		ec_pause_sel[0]);
	AL_REG_FIELD_SET(reg,
		ETH_COMMON_GEN_EC_UDMA_PAUSE_MUX_SEL_1_EC_QUEUE_MASK,
		ETH_COMMON_GEN_EC_UDMA_PAUSE_MUX_SEL_1_EC_QUEUE_SHIFT,
		ec_pause_sel[1]);
	AL_REG_FIELD_SET(reg,
		ETH_COMMON_GEN_EC_UDMA_PAUSE_MUX_SEL_2_EC_QUEUE_MASK,
		ETH_COMMON_GEN_EC_UDMA_PAUSE_MUX_SEL_2_EC_QUEUE_SHIFT,
		ec_pause_sel[2]);
	AL_REG_FIELD_SET(reg,
		ETH_COMMON_GEN_EC_UDMA_PAUSE_MUX_SEL_3_EC_QUEUE_MASK,
		ETH_COMMON_GEN_EC_UDMA_PAUSE_MUX_SEL_3_EC_QUEUE_SHIFT,
		ec_pause_sel[3]);
	AL_REG_FIELD_SET(reg,
		ETH_COMMON_GEN_EC_UDMA_PAUSE_MUX_SEL_0_UDMA_QUEUE_MASK,
		ETH_COMMON_GEN_EC_UDMA_PAUSE_MUX_SEL_0_UDMA_QUEUE_SHIFT,
		udma_pause_sel[0]);
	AL_REG_FIELD_SET(reg,
		ETH_COMMON_GEN_EC_UDMA_PAUSE_MUX_SEL_1_UDMA_QUEUE_MASK,
		ETH_COMMON_GEN_EC_UDMA_PAUSE_MUX_SEL_1_UDMA_QUEUE_SHIFT,
		udma_pause_sel[1]);
	AL_REG_FIELD_SET(reg,
		ETH_COMMON_GEN_EC_UDMA_PAUSE_MUX_SEL_2_UDMA_QUEUE_MASK,
		ETH_COMMON_GEN_EC_UDMA_PAUSE_MUX_SEL_2_UDMA_QUEUE_SHIFT,
		udma_pause_sel[2]);
	AL_REG_FIELD_SET(reg,
		ETH_COMMON_GEN_EC_UDMA_PAUSE_MUX_SEL_3_UDMA_QUEUE_MASK,
		ETH_COMMON_GEN_EC_UDMA_PAUSE_MUX_SEL_3_UDMA_QUEUE_SHIFT,
		udma_pause_sel[3]);
	/** identical configurations */
	reg |= (reg >> ETH_COMMON_GEN_EC_UDMA_PAUSE_MUX_SEL_0_EC_QUEUE_SHIFT);
	al_mod_reg_write32(&regs_base->gen.ec_udma_pause_mux, reg);

	/**
	 * EC to UDMA external application interrupt mux
	 */
	reg = 0;
	AL_REG_FIELD_SET(reg,
		ETH_COMMON_GEN_UDMA_EC_INT_MASK_0_MASK,
		ETH_COMMON_GEN_UDMA_EC_INT_MASK_0_SHIFT,
		udma_to_ec_int[0]);
	AL_REG_FIELD_SET(reg,
		ETH_COMMON_GEN_UDMA_EC_INT_MASK_1_MASK,
		ETH_COMMON_GEN_UDMA_EC_INT_MASK_1_SHIFT,
		udma_to_ec_int[1]);
	AL_REG_FIELD_SET(reg,
		ETH_COMMON_GEN_UDMA_EC_INT_MASK_2_MASK,
		ETH_COMMON_GEN_UDMA_EC_INT_MASK_2_SHIFT,
		udma_to_ec_int[2]);
	AL_REG_FIELD_SET(reg,
		ETH_COMMON_GEN_UDMA_EC_INT_MASK_3_MASK,
		ETH_COMMON_GEN_UDMA_EC_INT_MASK_3_SHIFT,
		udma_to_ec_int[3]);
	al_mod_reg_write32(&regs_base->gen.udma_ec_int, reg);

	/* enable multi port non-blocking logic */
	/** Set the maximum packet size in beats */
	max_packet_size_beats = AL_ETH_MAX_FRAME_LEN / AL_ETH_COMMON_BUS_WIDTH;
	al_mod_reg_write32(&regs_base->udma_rx_fifos.max0, max_packet_size_beats);
	al_mod_reg_write32(&regs_base->udma_rx_fifos.max1, max_packet_size_beats);
	al_mod_reg_write32(&regs_base->udma_rx_fifos.max2, max_packet_size_beats);
	al_mod_reg_write32(&regs_base->udma_rx_fifos.max3, max_packet_size_beats);
	/**
	 * Set the threshold in beats, equals to :
	 *   FIFO SIZE IN BEATS - MAX PACKET SIZE IN BEATS - 10
	 */
	threshold_beats = AL_ETH_COMMON_FIFO_SIZE_IN_BEATS -
				(AL_ETH_MAX_FRAME_LEN / AL_ETH_COMMON_BUS_WIDTH) - 10;
	al_mod_reg_write32(&regs_base->udma_rx_fifos.rx_data_path0, threshold_beats);
	al_mod_reg_write32(&regs_base->udma_rx_fifos.rx_data_path1, threshold_beats);
	al_mod_reg_write32(&regs_base->udma_rx_fifos.rx_data_path2, threshold_beats);
	al_mod_reg_write32(&regs_base->udma_rx_fifos.rx_data_path3, threshold_beats);

	al_mod_reg_write32(&regs_base->udma_rx_fifos.afull,
			(((1 << udma2ec_tx[3]) & 0xf) << (AL_ETH_COMMON_UDMA_NUM * 3)) |
			(((1 << udma2ec_tx[2]) & 0xf) << (AL_ETH_COMMON_UDMA_NUM * 2)) |
			(((1 << udma2ec_tx[1]) & 0xf) << (AL_ETH_COMMON_UDMA_NUM * 1)) |
			(((1 << udma2ec_tx[0]) & 0xf) << 0));

	 /* more than one port is enabled */
	if (adapter_exists[1] || adapter_exists[2] || adapter_exists[3]) {
		al_mod_reg_write32(&shared_regs_base->rxf_maping.rx_udma_fifo,
			ETH_SHARED_RESOURCE_RXF_MAPING_RX_UDMA_FIFO_MODE_EN);
	}

	/* Adapters Config */
	for (i = 0; i < AL_ETH_COMMON_ADAPTER_NUM; i++) {
		uint32_t reg;

		/* Number of VFs override */
		reg = (num_of_vf[i] << ETH_COMMON_ADPT_CFG_NOF_VF_OVRD_VALUE_SHIFT)
				& ETH_COMMON_ADPT_CFG_NOF_VF_OVRD_VALUE_MASK;
		reg |= ETH_COMMON_ADPT_CFG_NOF_VF_OVRD;
		al_mod_reg_write32(&regs_base->adpt_cfg[i].nof_vf, reg);

		/* Enable/disable adapter */
		reg = ETH_COMMON_ADPT_CFG_CONF_DEV_NOT_EXIST_OVRD;
		if (!adapter_exists[i])
			reg |= ETH_COMMON_ADPT_CFG_CONF_DEV_NOT_EXIST_OVRD_VALUE;

		al_mod_reg_write32(&regs_base->adpt_cfg[i].conf_dev, reg);

		/* Bus master enable / FLR - configure slaves */
		if (bme_flr_slaves_override[i]) {
			reg = (bme_flr_slaves[i] << ETH_COMMON_ADPT_CFG_BME_OVRD_SEL_SHIFT)
					& ETH_COMMON_ADPT_CFG_BME_OVRD_SEL_MASK;
			reg |= ETH_COMMON_ADPT_CFG_BME_OVRD;

			al_mod_reg_write32(&regs_base->adpt_cfg[i].bme, reg);

			reg = (bme_flr_slaves[i] << ETH_COMMON_ADPT_CFG_FLR_CFG_OVRD_SEL_SHIFT)
					& ETH_COMMON_ADPT_CFG_FLR_CFG_OVRD_SEL_MASK;
			reg |= ETH_COMMON_ADPT_CFG_FLR_CFG_OVRD;

			al_mod_reg_write32(&regs_base->adpt_cfg[i].flr_cfg, reg);
		}
	}

	/* Shared RX FIFO config */
	shared_rx_fifo_config(shared_regs_base, rxf_part,
			init_params->internal_switching);

	/* Port UDMA mapping */
	al_mod_eth_common_udma_select_config(init_params, udma2udma_map);

	al_mod_eth_common_rmn(init_params);

	return 0;
}

static int al_mod_eth_common_macs_init(struct al_mod_eth_common_init_params *init_params)
{
	struct al_mod_eth_common_regs __iomem *regs_base = (struct al_mod_eth_common_regs __iomem *)
		init_params->eth_common_regs_base;
	struct al_mod_eth_mac_v4_common_regs __iomem *mac_common_regs_base =
		(struct al_mod_eth_mac_v4_common_regs __iomem *)&regs_base->mac_shared_resource[0];
	uint32_t reg = 0;
	uint32_t sd_n2 = 0;
	uint32_t sd_top20_gate = 0;
	uint32_t an_lt_ipregs_sel = 0;

	/* Write 1's to all clock enable bits */
	/* BOZO: Do specific clock gating based on MAC selection */
	reg = ETH_MAC_V4_COMMON_GBL_CLOCK_MAC_50G_CLK_EN_MASK |
		ETH_MAC_V4_COMMON_GBL_CLOCK_MAC_100G_CLK_EN |
		ETH_MAC_V4_COMMON_GBL_CLOCK_PCS_CLK_EN |
		ETH_MAC_V4_COMMON_GBL_CLOCK_AN_LT_CLK_EN_MASK;
	al_mod_reg_write32(&mac_common_regs_base->gbl.clock, reg);

	reg = 0;
	/* Write PCS_100G_PIN_CFG based on mode */
	switch (init_params->mode) {

	case AL_ETH_COMMON_MODE_4X10G:
		sd_n2 = ETH_MAC_V4_COMMON_GEN_V4_LANE(0) |
			ETH_MAC_V4_COMMON_GEN_V4_LANE(1) |
			ETH_MAC_V4_COMMON_GEN_V4_LANE(2) |
			ETH_MAC_V4_COMMON_GEN_V4_LANE(3);
		sd_top20_gate =
			/** Set active width of 40 bit serdeses to 20 bit (10G rate) */
			ETH_MAC_V4_COMMON_GEN_V4_SERDES_40_TX_GATE_20B_SERDES_3 |
			ETH_MAC_V4_COMMON_GEN_V4_SERDES_40_TX_GATE_20B_SERDES_2 |
			ETH_MAC_V4_COMMON_GEN_V4_SERDES_40_TX_GATE_20B_SERDES_1 |
			ETH_MAC_V4_COMMON_GEN_V4_SERDES_40_TX_GATE_20B_SERDES_0;

		/** Set AN_LT regs channel select & registers */
		AL_REG_FIELD_SET(an_lt_ipregs_sel,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL0_MASK,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL0_SHIFT,
			AL_ETH_AN__LT_LANE_0);
		AL_REG_FIELD_SET(an_lt_ipregs_sel,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL1_MASK,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL1_SHIFT,
			AL_ETH_AN__LT_LANE_1);
		AL_REG_FIELD_SET(an_lt_ipregs_sel,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL2_MASK,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL2_SHIFT,
			AL_ETH_AN__LT_LANE_2);
		AL_REG_FIELD_SET(an_lt_ipregs_sel,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL3_MASK,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL3_SHIFT,
			AL_ETH_AN__LT_LANE_3);

		break;
	case AL_ETH_COMMON_MODE_4X25G:
		/** Set AN_LT regs channel select & registers */
		AL_REG_FIELD_SET(an_lt_ipregs_sel,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL0_MASK,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL0_SHIFT,
			AL_ETH_AN__LT_LANE_0);
		AL_REG_FIELD_SET(an_lt_ipregs_sel,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL1_MASK,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL1_SHIFT,
			AL_ETH_AN__LT_LANE_1);
		AL_REG_FIELD_SET(an_lt_ipregs_sel,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL2_MASK,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL2_SHIFT,
			AL_ETH_AN__LT_LANE_2);
		AL_REG_FIELD_SET(an_lt_ipregs_sel,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL3_MASK,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL3_SHIFT,
			AL_ETH_AN__LT_LANE_3);

		break;
	case AL_ETH_COMMON_MODE_1X40G:
		reg = ETH_MAC_V4_COMMON_GEN_V4_PCS_ALL_PIN_CFG_MODE40_ENA_IN;
		sd_n2 = ETH_MAC_V4_COMMON_GEN_V4_LANE(0) |
			ETH_MAC_V4_COMMON_GEN_V4_LANE(1) |
			ETH_MAC_V4_COMMON_GEN_V4_LANE(2) |
			ETH_MAC_V4_COMMON_GEN_V4_LANE(3);
		sd_top20_gate =
			ETH_MAC_V4_COMMON_GEN_V4_SERDES_40_TX_GATE_20B_SERDES_3 |
			ETH_MAC_V4_COMMON_GEN_V4_SERDES_40_TX_GATE_20B_SERDES_2 |
			ETH_MAC_V4_COMMON_GEN_V4_SERDES_40_TX_GATE_20B_SERDES_1 |
			ETH_MAC_V4_COMMON_GEN_V4_SERDES_40_TX_GATE_20B_SERDES_0;

		/** Set all AN LT lanes to channel 0 registers */
		AL_REG_FIELD_SET(an_lt_ipregs_sel,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL0_MASK,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL0_SHIFT,
			AL_ETH_AN__LT_LANE_0);
		AL_REG_FIELD_SET(an_lt_ipregs_sel,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL1_MASK,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL1_SHIFT,
			AL_ETH_AN__LT_LANE_0);
		AL_REG_FIELD_SET(an_lt_ipregs_sel,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL2_MASK,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL2_SHIFT,
			AL_ETH_AN__LT_LANE_0);
		AL_REG_FIELD_SET(an_lt_ipregs_sel,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL3_MASK,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL3_SHIFT,
			AL_ETH_AN__LT_LANE_0);

		break;
	case AL_ETH_COMMON_MODE_2X50G:
		reg = ETH_MAC_V4_COMMON_GEN_V4_PCS_ALL_PIN_CFG_RXLAUI_ENA_IN0 |
			ETH_MAC_V4_COMMON_GEN_V4_PCS_ALL_PIN_CFG_RXLAUI_ENA_IN2;

		/** Connect AN_LT registers 2,3 to channel regs 2 and AN LT 0,1 to channel regs 0 */
		AL_REG_FIELD_SET(an_lt_ipregs_sel,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL0_MASK,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL0_SHIFT,
			AL_ETH_AN__LT_LANE_0);
		AL_REG_FIELD_SET(an_lt_ipregs_sel,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL1_MASK,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL1_SHIFT,
			AL_ETH_AN__LT_LANE_0);
		AL_REG_FIELD_SET(an_lt_ipregs_sel,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL2_MASK,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL2_SHIFT,
			AL_ETH_AN__LT_LANE_2);
		AL_REG_FIELD_SET(an_lt_ipregs_sel,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL3_MASK,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL3_SHIFT,
			AL_ETH_AN__LT_LANE_2);

		break;
	case AL_ETH_COMMON_MODE_1X100G:
		reg = ETH_MAC_V4_COMMON_GEN_V4_PCS_ALL_PIN_CFG_PCS100_ENA_IN;
		AL_REG_FIELD_SET(an_lt_ipregs_sel,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL0_MASK,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL0_SHIFT,
			AL_ETH_AN__LT_LANE_0);
		AL_REG_FIELD_SET(an_lt_ipregs_sel,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL1_MASK,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL1_SHIFT,
			AL_ETH_AN__LT_LANE_0);
		AL_REG_FIELD_SET(an_lt_ipregs_sel,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL2_MASK,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL2_SHIFT,
			AL_ETH_AN__LT_LANE_0);
		AL_REG_FIELD_SET(an_lt_ipregs_sel,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL3_MASK,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL3_SHIFT,
			AL_ETH_AN__LT_LANE_0);

		break;
	case AL_ETH_COMMON_MODE_2X25G_2X10G:
		sd_n2 = ETH_MAC_V4_COMMON_GEN_V4_LANE(2) |
			ETH_MAC_V4_COMMON_GEN_V4_LANE(3);
		sd_top20_gate =
			ETH_MAC_V4_COMMON_GEN_V4_SERDES_40_TX_GATE_20B_SERDES_3 |
			ETH_MAC_V4_COMMON_GEN_V4_SERDES_40_TX_GATE_20B_SERDES_2;
		AL_REG_FIELD_SET(an_lt_ipregs_sel,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL0_MASK,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL0_SHIFT,
			AL_ETH_AN__LT_LANE_0);
		AL_REG_FIELD_SET(an_lt_ipregs_sel,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL1_MASK,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL1_SHIFT,
			AL_ETH_AN__LT_LANE_1);
		AL_REG_FIELD_SET(an_lt_ipregs_sel,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL2_MASK,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL2_SHIFT,
			AL_ETH_AN__LT_LANE_2);
		AL_REG_FIELD_SET(an_lt_ipregs_sel,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL3_MASK,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL3_SHIFT,
			AL_ETH_AN__LT_LANE_3);

		break;
	case AL_ETH_COMMON_MODE_1X50G_2X25G:
		reg = ETH_MAC_V4_COMMON_GEN_V4_PCS_ALL_PIN_CFG_RXLAUI_ENA_IN0;

		/** AN LT 0,1 registers connects to ch 0 & 2,3 to 2,3 respectively */
		AL_REG_FIELD_SET(an_lt_ipregs_sel,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL0_MASK,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL0_SHIFT,
			AL_ETH_AN__LT_LANE_0);
		AL_REG_FIELD_SET(an_lt_ipregs_sel,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL1_MASK,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL1_SHIFT,
			AL_ETH_AN__LT_LANE_0);
		AL_REG_FIELD_SET(an_lt_ipregs_sel,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL2_MASK,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL2_SHIFT,
			AL_ETH_AN__LT_LANE_2);
		AL_REG_FIELD_SET(an_lt_ipregs_sel,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL3_MASK,
			ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_CH_SEL3_SHIFT,
			AL_ETH_AN__LT_LANE_3);

		break;
	default:
		return -EINVAL;
		break;
	}
	AL_REG_FIELD_SET(reg,
		ETH_MAC_V4_COMMON_GEN_V4_PCS_ALL_PIN_CFG_SD_N2_MASK,
		ETH_MAC_V4_COMMON_GEN_V4_PCS_ALL_PIN_CFG_SD_N2_SHIFT,
		sd_n2);
	al_mod_reg_write32(&mac_common_regs_base->gen_v4.pcs_all_pin_cfg, reg);

	al_mod_reg_write32(&mac_common_regs_base->gen_v4.serdes_40_tx_gate_20b, sd_top20_gate);

	/** Connect each AN LT lane to channel */
	AL_REG_FIELD_SET(an_lt_ipregs_sel,
		ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_LANE_SEL0_MASK,
		ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_LANE_SEL0_SHIFT,
		AL_ETH_AN__LT_LANE_0);
	AL_REG_FIELD_SET(an_lt_ipregs_sel,
		ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_LANE_SEL1_MASK,
		ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_LANE_SEL1_SHIFT,
		AL_ETH_AN__LT_LANE_1);
	AL_REG_FIELD_SET(an_lt_ipregs_sel,
		ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_LANE_SEL2_MASK,
		ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_LANE_SEL2_SHIFT,
		AL_ETH_AN__LT_LANE_2);
	AL_REG_FIELD_SET(an_lt_ipregs_sel,
		ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_LANE_SEL3_MASK,
		ETH_MAC_V4_COMMON_GEN_V4_AN_LT_IPREGS_SEL_LANE_SEL3_SHIFT,
		AL_ETH_AN__LT_LANE_3);
	al_mod_reg_write32(&mac_common_regs_base->gen_v4.an_lt_ipregs_sel, an_lt_ipregs_sel);

	return 0;
}

static void al_mod_eth_common_tcam_init(struct al_mod_eth_common_init_params *init_params)
{
	struct al_mod_eth_common_regs __iomem *regs_base = (struct al_mod_eth_common_regs __iomem *)
		init_params->eth_common_regs_base;
	struct al_mod_eth_shared_resource_regs __iomem *shared_regs_base =
		(struct al_mod_eth_shared_resource_regs __iomem *)&regs_base->eth_sr[0];
	uint32_t  i;

	for (i = 0; i < AL_ARR_SIZE(shared_regs_base->reg_tcam_core_ctl); i++) {
		al_mod_reg_write32_masked(&shared_regs_base->reg_tcam_core_ctl[i].bist_tmg_mode,
			ETH_SHARED_RESOURCE_REG_TCAM_CORE_CTL_BIST_TMG_MODE_CORE_MEM_HDCAM_MASK,
			(AL_ETH_COMMON_TCAM_TMG_MODE_CORE_MEM_HDCAM_VAL <<
			 ETH_SHARED_RESOURCE_REG_TCAM_CORE_CTL_BIST_TMG_MODE_CORE_MEM_HDCAM_SHIFT));
	}
}

int al_mod_eth_mac_v4_fec_ctrl(struct al_mod_eth_mac_obj *mac_obj,
	enum al_mod_eth_mac_common_mac_v4_lane lane, enum al_mod_eth_fec_type fec_type, al_mod_bool fec_enable)
{
	struct al_mod_eth_mac_v4_common_regs *mac_v4_common_regs =
			mac_obj->mac_common_regs;
	struct al_mod_eth_mac_v4_common_gen_v4 *mac_v4_common_gen_v4 = &mac_v4_common_regs->gen_v4;

	uint32_t reg = 0;
	uint32_t fec_ena = 0;
	uint32_t fec91_ena_in = 0;

	reg = al_mod_reg_read32(&mac_v4_common_gen_v4->pcs_fec_pin_cfg);
	if (fec_type == AL_ETH_FEC_TYPE_CLAUSE_91) {
		fec91_ena_in = AL_REG_FIELD_GET(reg,
			ETH_MAC_V4_COMMON_GEN_V4_PCS_FEC_PIN_CFG_FEC91_ENA_IN_MASK,
			ETH_MAC_V4_COMMON_GEN_V4_PCS_FEC_PIN_CFG_FEC91_ENA_IN_SHIFT);

		switch (mac_obj->mac_mode) {
		case AL_ETH_MAC_MODE_XLG_LL_25G:
			/* Set pin-based fec91_ena_in based on current lane */
			if (fec_enable) {
				fec91_ena_in |=
					AL_ETH_COMMON_FEC_ENABLE_MAC_MODE_XLG_LL_25G << lane;

				/* Additionally, set 1lane_in bit as special case if lane 0 / 2 */
				if (lane == AL_ETH_MAC_V4_LANE_0)
					reg |=
					ETH_MAC_V4_COMMON_GEN_V4_PCS_FEC_PIN_CFG_FEC91_1LANE_IN0;
				else if (lane == AL_ETH_MAC_V4_LANE_2)
					reg |=
					ETH_MAC_V4_COMMON_GEN_V4_PCS_FEC_PIN_CFG_FEC91_1LANE_IN2;
			} else
				fec91_ena_in &=
					~(AL_ETH_COMMON_FEC_ENABLE_MAC_MODE_XLG_LL_25G << lane);
			break;
		case AL_ETH_MAC_MODE_XLG_LL_50G:
			if (fec_enable) {
				fec91_ena_in |=
					AL_ETH_COMMON_FEC_ENABLE_MAC_MODE_XLG_LL_50G << lane;
			} else {
				fec91_ena_in &=
					~(AL_ETH_COMMON_FEC_ENABLE_MAC_MODE_XLG_LL_50G << lane);

				/* If disabled, also clear fec91_1lane_in field! */
				if (lane == AL_ETH_MAC_V4_LANE_0)
					reg &=
					~ETH_MAC_V4_COMMON_GEN_V4_PCS_FEC_PIN_CFG_FEC91_1LANE_IN0;
				else if (lane == AL_ETH_MAC_V4_LANE_2)
					reg &=
					~ETH_MAC_V4_COMMON_GEN_V4_PCS_FEC_PIN_CFG_FEC91_1LANE_IN2;
			}

			break;
		case AL_ETH_MAC_MODE_CG_100G:
			if (fec_enable)
				fec91_ena_in |= AL_ETH_COMMON_FEC_ENABLE_MAC_MODE_CG_100G;
			else
				fec91_ena_in &= ~AL_ETH_COMMON_FEC_ENABLE_MAC_MODE_CG_100G;
			break;
		default:
			/** TODO print out the common mac mode aswell ? */
			al_mod_err("%s: fec type: %d is not supported on this MAC mode %d\n",
				__func__, fec_type, mac_obj->mac_mode);
			return -EPERM;
		}
		AL_REG_FIELD_SET(reg,
			ETH_MAC_V4_COMMON_GEN_V4_PCS_FEC_PIN_CFG_FEC91_ENA_IN_MASK,
			ETH_MAC_V4_COMMON_GEN_V4_PCS_FEC_PIN_CFG_FEC91_ENA_IN_SHIFT,
			fec91_ena_in);
		al_mod_reg_write32(&mac_v4_common_gen_v4->pcs_fec_pin_cfg, reg);

	} else if (fec_type == AL_ETH_FEC_TYPE_CLAUSE_74) {
		fec_ena = AL_REG_FIELD_GET(reg,
			ETH_MAC_V4_COMMON_GEN_V4_PCS_FEC_PIN_CFG_FEC_ENA_MASK,
			ETH_MAC_V4_COMMON_GEN_V4_PCS_FEC_PIN_CFG_FEC_ENA_SHIFT);

		switch (mac_obj->mac_mode) {
		case AL_ETH_MAC_MODE_10GbE_Serial:
		case AL_ETH_MAC_MODE_XLG_LL_25G:
			if (fec_enable)
				fec_ena |= AL_ETH_COMMON_FEC_ENABLE_MAC_MODE_XLG_LL_25G << lane;
			else
				fec_ena &= ~(AL_ETH_COMMON_FEC_ENABLE_MAC_MODE_XLG_LL_25G << lane);
			break;

		case AL_ETH_MAC_MODE_XLG_LL_40G:
			if (fec_enable)
				fec_ena |= AL_ETH_COMMON_FEC_ENABLE_MAC_MODE_XLG_LL_40G;
			else
				fec_ena &= ~AL_ETH_COMMON_FEC_ENABLE_MAC_MODE_XLG_LL_40G;
			break;
		case AL_ETH_MAC_MODE_XLG_LL_50G:
			if (fec_enable)
				fec_ena |= AL_ETH_COMMON_FEC_ENABLE_MAC_MODE_XLG_LL_50G << lane;
			else
				fec_ena &= ~(AL_ETH_COMMON_FEC_ENABLE_MAC_MODE_XLG_LL_50G << lane);
			break;
		default:
			/** TODO print out the common mac mode aswell ? */
			al_mod_err("%s: fec type: %d is not supported on this MAC mode %d\n",
				__func__, fec_type, mac_obj->mac_mode);
			return -EPERM;
		}
		AL_REG_FIELD_SET(reg,
			ETH_MAC_V4_COMMON_GEN_V4_PCS_FEC_PIN_CFG_FEC_ENA_MASK,
			ETH_MAC_V4_COMMON_GEN_V4_PCS_FEC_PIN_CFG_FEC_ENA_SHIFT,
			fec_ena);
		AL_REG_FIELD_SET(reg,
			ETH_MAC_V4_COMMON_GEN_V4_PCS_FEC_PIN_CFG_FEC_ERR_ENA_MASK,
			ETH_MAC_V4_COMMON_GEN_V4_PCS_FEC_PIN_CFG_FEC_ERR_ENA_SHIFT,
			fec_ena);
		al_mod_reg_write32(&mac_v4_common_gen_v4->pcs_fec_pin_cfg, reg);
	} else {
		al_mod_err("%s: No such fec type as %d\n", __func__, fec_type);
		return -EPERM;
	}

return 0;
}

/* API */
int al_mod_eth_common_init(struct al_mod_eth_common_init_params *init_params)
{
	struct al_mod_eth_common_handle handle = { NULL };
	struct al_mod_eth_common_handle_init_params handle_init = { NULL };
	int ret = 0;

	al_mod_assert(init_params);

	handle_init.eth_common_regs_base = init_params->eth_common_regs_base;
	handle_init.eth_ec_0_regs_base = init_params->eth_ec_0_regs_base;
	al_mod_eth_common_handle_init(&handle, &handle_init);

	if (!init_params->skip_adapters_udmas_ecs_init) {
		ret = al_mod_eth_common_adapters_udmas_ecs_init(init_params);

		if (ret)
			return ret;
	}

	if (!init_params->skip_macs_init) {
		ret = al_mod_eth_common_macs_init(init_params);

		if (ret)
			return ret;
	}

	al_mod_eth_common_tcam_init(init_params);

	al_mod_eth_common_perf_mode_config(&handle, AL_ETH_PERF_MODE_DEFAULT);

	return 0;
}

void al_mod_eth_common_perf_mode_config(struct al_mod_eth_common_handle *handle,
	enum al_mod_eth_perf_mode mode)
{
	struct al_mod_eth_common_regs __iomem *regs_base;
	struct al_mod_eth_shared_resource_regs __iomem *shared_regs_base;
	al_mod_bool crdt_neg_en; /* Negative credit enable */
	uint32_t crdt_inc_int; /* Credit increment interval */
	uint32_t crdt_lvl_th; /* Credit level threshold */
	uint32_t crdt_lvl_sat; /* Credit level saturation */
	uint32_t crdt_inc_val; /* Credit increment value */
	uint32_t reg;

	al_mod_assert(handle);

	regs_base = (struct al_mod_eth_common_regs __iomem *)handle->eth_common_regs_base;
	shared_regs_base = (struct al_mod_eth_shared_resource_regs __iomem *)&regs_base->eth_sr[0];

	switch (mode) {
	case AL_ETH_PERF_MODE_HW_RESET_VALUE:
		crdt_neg_en = AL_TRUE;
		crdt_inc_int = 0x4;
		crdt_lvl_th = 0x120;
		crdt_lvl_sat = 0x130;
		crdt_inc_val = 0x1;
		break;
	case AL_ETH_PERF_MODE_A:
		crdt_neg_en = AL_TRUE;
		crdt_inc_int = 0x80;
		crdt_lvl_th = 0x120;
		crdt_lvl_sat = 0x168;
		crdt_inc_val = 0x19;
		break;
	default:
		al_mod_assert_msg(AL_FALSE, "Unknown Ethernet Common performance mode: %d\n", mode);
		return;
	}

	/**
	 * Dirty fix to make Fortify pass.
	 * Fortify claims that this variable is written but never read, which is false claim,
	 * as it's used for a comparison operation.
	 */
	(void)crdt_neg_en;

	reg = al_mod_reg_read32(&shared_regs_base->axs_dwrr_arb_ctrl.arb_config);
	AL_REG_FIELD_SET(reg,
		ETH_SHARED_RESOURCE_AXS_DWRR_ARB_CTRL_ARB_CONFIG_MODE_NEG_CRED_EN_MASK,
		ETH_SHARED_RESOURCE_AXS_DWRR_ARB_CTRL_ARB_CONFIG_MODE_NEG_CRED_EN_SHIFT,
		(crdt_neg_en == AL_TRUE ? 0xF : 0));
	AL_REG_FIELD_SET(reg,
		ETH_SHARED_RESOURCE_AXS_DWRR_ARB_CTRL_ARB_CONFIG_CREDIT_GRANT_INTERVAL_MASK,
		ETH_SHARED_RESOURCE_AXS_DWRR_ARB_CTRL_ARB_CONFIG_CREDIT_GRANT_INTERVAL_SHIFT,
		crdt_inc_int);
	al_mod_reg_write32(&shared_regs_base->axs_dwrr_arb_ctrl.arb_config, reg);

	reg = al_mod_reg_read32(&shared_regs_base->axs_dwrr_arb_ctrl.cred_level);
	AL_REG_FIELD_SET(reg,
		ETH_SHARED_RESOURCE_AXS_DWRR_ARB_CTRL_CRED_LEVEL_CREDIT_THRESHOLD_MASK,
		ETH_SHARED_RESOURCE_AXS_DWRR_ARB_CTRL_CRED_LEVEL_CREDIT_THRESHOLD_SHIFT,
		crdt_lvl_th);
	AL_REG_FIELD_SET(reg,
		ETH_SHARED_RESOURCE_AXS_DWRR_ARB_CTRL_CRED_LEVEL_CREDIT_SATURATION_MASK,
		ETH_SHARED_RESOURCE_AXS_DWRR_ARB_CTRL_CRED_LEVEL_CREDIT_SATURATION_SHIFT,
		crdt_lvl_sat);
	al_mod_reg_write32(&shared_regs_base->axs_dwrr_arb_ctrl.cred_level, reg);

	reg = al_mod_reg_read32(&shared_regs_base->axs_dwrr_arb_ctrl.cred_grant);
	AL_REG_FIELD_SET(reg,
		ETH_SHARED_RESOURCE_AXS_DWRR_ARB_CTRL_CRED_GRANT_MASTER0_GRANT_MASK,
		ETH_SHARED_RESOURCE_AXS_DWRR_ARB_CTRL_CRED_GRANT_MASTER0_GRANT_SHIFT,
		crdt_inc_val);
	AL_REG_FIELD_SET(reg,
		ETH_SHARED_RESOURCE_AXS_DWRR_ARB_CTRL_CRED_GRANT_MASTER1_GRANT_MASK,
		ETH_SHARED_RESOURCE_AXS_DWRR_ARB_CTRL_CRED_GRANT_MASTER1_GRANT_SHIFT,
		crdt_inc_val);
	AL_REG_FIELD_SET(reg,
		ETH_SHARED_RESOURCE_AXS_DWRR_ARB_CTRL_CRED_GRANT_MASTER2_GRANT_MASK,
		ETH_SHARED_RESOURCE_AXS_DWRR_ARB_CTRL_CRED_GRANT_MASTER2_GRANT_SHIFT,
		crdt_inc_val);
	AL_REG_FIELD_SET(reg,
		ETH_SHARED_RESOURCE_AXS_DWRR_ARB_CTRL_CRED_GRANT_MASTER3_GRANT_MASK,
		ETH_SHARED_RESOURCE_AXS_DWRR_ARB_CTRL_CRED_GRANT_MASTER3_GRANT_SHIFT,
		crdt_inc_val);
	al_mod_reg_write32(&shared_regs_base->axs_dwrr_arb_ctrl.cred_grant, reg);
}

void al_mod_eth_common_adp2func_flr_read_raw(struct al_mod_eth_common_handle *handle,
					 struct al_mod_eth_common_adp2func_flr_regs *flr_regs)
{
	struct al_mod_eth_common_regs __iomem *regs_base;

	al_mod_assert(handle);
	al_mod_assert(handle->eth_common_regs_base);
	al_mod_assert(flr_regs);

	regs_base = handle->eth_common_regs_base;

	flr_regs->flr = al_mod_reg_read32(&regs_base->adp2func.flr);
	flr_regs->flr_bar = al_mod_reg_read32(&regs_base->adp2func.flr_bar);
}

void al_mod_eth_common_adp2func_flr_write_raw(struct al_mod_eth_common_handle *handle,
					  struct al_mod_eth_common_adp2func_flr_regs *flr_regs)
{
	struct al_mod_eth_common_regs __iomem *regs_base;

	al_mod_assert(handle);
	al_mod_assert(handle->eth_common_regs_base);
	al_mod_assert(flr_regs);

	regs_base = handle->eth_common_regs_base;

	al_mod_reg_write32(&regs_base->adp2func.flr, flr_regs->flr);
	al_mod_reg_write32(&regs_base->adp2func.flr_bar, flr_regs->flr_bar);
}

void al_mod_eth_common_adp2func_flr_udma_config(struct al_mod_eth_common_handle *handle,
					    struct al_mod_eth_common_adp2func_flr_udma_attr *udma_attr)

{
	struct al_mod_eth_common_regs __iomem *regs_base;
	struct al_mod_eth_common_adp2func_flr_regs flr_regs;
	struct al_mod_eth_common_adp2func_flr_regs flr_mask;
	struct al_mod_eth_common_adp2func_flr_regs flr_shift;

	al_mod_assert(handle);
	al_mod_assert(udma_attr);

	al_mod_assert_msg(udma_attr->adapter_num < AL_ETH_COMMON_ADAPTER_NUM,
		      "Invalid adapter_num (%u)! Must be between 0-%d\n",
		      udma_attr->adapter_num,
		      AL_ETH_COMMON_ADAPTER_NUM);

	al_mod_assert_msg(udma_attr->flr_bit < AL_ETH_COMMON_ADAPTER_FUNC_NUM,
		      "Invalid flr_bit (%u)! Must be between 0-%d\n",
		      udma_attr->flr_bit,
		      AL_ETH_COMMON_ADAPTER_FUNC_NUM);

	regs_base = handle->eth_common_regs_base;

	flr_regs.flr = al_mod_reg_read32(&regs_base->adp2func.flr);
	flr_regs.flr_bar = al_mod_reg_read32(&regs_base->adp2func.flr_bar);

	switch (udma_attr->udma_num) {
	case 0:
		flr_mask.flr = ETH_COMMON_ADP2FUNC_FLR_SEL_0_MASK;
		flr_shift.flr = ETH_COMMON_ADP2FUNC_FLR_SEL_0_SHIFT;
		flr_mask.flr_bar = ETH_COMMON_ADP2FUNC_FLR_BAR_SEL_0_MASK;
		flr_shift.flr_bar = ETH_COMMON_ADP2FUNC_FLR_BAR_SEL_0_SHIFT;
		break;
	case 1:
		flr_mask.flr = ETH_COMMON_ADP2FUNC_FLR_SEL_1_MASK;
		flr_shift.flr = ETH_COMMON_ADP2FUNC_FLR_SEL_1_SHIFT;
		flr_mask.flr_bar = ETH_COMMON_ADP2FUNC_FLR_BAR_SEL_1_MASK;
		flr_shift.flr_bar = ETH_COMMON_ADP2FUNC_FLR_BAR_SEL_1_SHIFT;
		break;
	case 2:
		flr_mask.flr = ETH_COMMON_ADP2FUNC_FLR_SEL_2_MASK;
		flr_shift.flr = ETH_COMMON_ADP2FUNC_FLR_SEL_2_SHIFT;
		flr_mask.flr_bar = ETH_COMMON_ADP2FUNC_FLR_BAR_SEL_2_MASK;
		flr_shift.flr_bar = ETH_COMMON_ADP2FUNC_FLR_BAR_SEL_2_SHIFT;
		break;
	case 3:
		flr_mask.flr = ETH_COMMON_ADP2FUNC_FLR_SEL_3_MASK;
		flr_shift.flr = ETH_COMMON_ADP2FUNC_FLR_SEL_3_SHIFT;
		flr_mask.flr_bar = ETH_COMMON_ADP2FUNC_FLR_BAR_SEL_3_MASK;
		flr_shift.flr_bar = ETH_COMMON_ADP2FUNC_FLR_BAR_SEL_3_SHIFT;
		break;
	default:
		al_mod_assert_msg(0,
			      "Invalid udma_num (%u)! Must be between 0-%d\n",
			      udma_attr->udma_num,
			      AL_ETH_COMMON_UDMA_NUM);
		return;
	};

	AL_REG_FIELD_SET(flr_regs.flr,
			 flr_mask.flr,
			 flr_shift.flr,
			 udma_attr->adapter_num);
	AL_REG_FIELD_SET(flr_regs.flr_bar,
			 flr_mask.flr_bar,
			 flr_shift.flr_bar,
			 udma_attr->flr_bit);

	al_mod_reg_write32(&regs_base->adp2func.flr, flr_regs.flr);
	al_mod_reg_write32(&regs_base->adp2func.flr_bar, flr_regs.flr_bar);
}

void al_mod_eth_common_rx_pipe_modules_ctrl(struct al_mod_eth_common_handle *handle,
					struct al_mod_eth_common_rx_pipe_modules_ctrl_params *params)
{
	struct al_mod_ec_regs __iomem *ec_0_regs_base;
	uint32_t gen_en;

	al_mod_assert(handle);
	al_mod_assert(handle->eth_ec_0_regs_base);
	al_mod_assert(params);

	ec_0_regs_base = handle->eth_ec_0_regs_base;

	gen_en = al_mod_reg_read32(&ec_0_regs_base->gen.en);

	al_mod_dbg("%s: ec_0_regs_base:%p, gen_en:0x%08x\n",
	       __func__, handle->eth_ec_0_regs_base, gen_en);
	al_mod_dbg("\t ec_gen_msw_in_valid:%d\tec_gen_msw_in:%d\n",
	       params->ec_gen_msw_in_valid, params->ec_gen_msw_in);

	if (params->ec_gen_msw_in_valid)
		AL_REG_FIELD_BIT_SET(gen_en, EC_GEN_EN_MSW_IN, params->ec_gen_msw_in);

	al_mod_reg_write32(&ec_0_regs_base->gen.en, gen_en);
}
