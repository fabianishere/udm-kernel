/*******************************************************************************
Copyright (C) 2013 Annapurna Labs Ltd.

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
 * @file   al_serdes_init.h
 *
 */

#ifndef __AL_SERDES_INIT_H__
#define __AL_SERDES_INIT_H__

#include "al_serdes.h"
#include "al_hal_serdes_interface.h"

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

struct al_serdes_group_cfg {
	enum al_serdes_group_mode	mode;

	/* Group active lanes - e.g. { AL_TRUE, AL_TRUE, AL_FALSE, AL_FALSE } */
	al_bool				active_lanes[AL_SRDS_NUM_LANES];

	/* R2L output clock source */
	enum al_serdes_clk_src		clk_src_r2l;

	/* L2R output clock source */
	enum al_serdes_clk_src		clk_src_l2r;

	/* SERDES core clock source */
	enum al_serdes_clk_src		clk_src_core;

	/* Reference clock frequency */
	enum al_serdes_clk_freq	ref_clk_freq;

	/*
	 * For group A - R2L clock frequency
	 * For groups B, C, D - Reserved
	 */
	enum al_serdes_clk_freq	r2l_clk_freq;

	/*
	 * For group D - L2R clock frequency
	 * For groups A, B, C - Reserved
	 */
	enum al_serdes_clk_freq	l2r_clk_freq;

	/*
	 * Spread spectrum clock enabled
	 * Relevant to mode == AL_SRDS_CFG_SATA_A / AL_SRDS_CFG_SATA_B
	 */
	al_bool				ssc_en;

	/* Inverted Tx lanes polarity - e.g. { AL_TRUE, AL_TRUE, ... } */
	al_bool				inv_tx_lanes[AL_SRDS_NUM_LANES];

	/* Inverted Rx lanes polarity - e.g. { AL_TRUE, AL_TRUE, ... } */
	al_bool				inv_rx_lanes[AL_SRDS_NUM_LANES];

	/*
	 * Mode set advanced parameters (ignored if NULL)
	 * Currently supported only for 10G and 25G on 25G SerDes
	 *
	 * Notice: 'group_mode' parameter of 'mode_set_adv_params' MUST match
	 *         this struct 'mode' parameter.
	 */
	struct al_serdes_mode_set_adv_params *mode_set_adv_params;

	/* Override the default AGC AC boost - relevant for PCIe only */
	al_bool				override_agc_ac_boost;
	uint8_t				override_agc_ac_boost_val;
};

struct al_serdes_cfg {
	struct al_serdes_group_cfg	grp_cfg[AL_SRDS_NUM_GROUPS];

	/**
	 * An indication for skipping core initialization - should only be
	 * used when initialization is done through RTL simulation forces.
	 */
	int				skip_core_init;

#if CHECK_ALPINE_V2
	/** SerDes 25g FW - if NULL used default FW */
	const char			*serdes_25g_fw;
	unsigned int			serdes_25g_fw_size;
#endif
};

/**
 * Initialize the SERDES complex
 *
 * @param  obj
 *             List of AL_SRDS_NUM_GROUPS group objects
 *
 * @param  pbs_regs_base
 *             The PBS register file base pointer
 *
 * @param  cfg
 *             The required SERDES configuration
 *
 * @return 0 if no error found.
 *
 */
int al_serdes_init(
	struct al_serdes_grp_obj	*obj,
	void __iomem			*pbs_regs_base,
	const struct al_serdes_cfg	*cfg);

/* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif

/* *INDENT-ON* */
#endif

/** @} end of SERDES group */
