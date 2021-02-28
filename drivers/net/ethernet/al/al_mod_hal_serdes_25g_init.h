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
 * @file   al_mod_hal_serdes_25g_init.h
 *
 */

#ifndef __AL_HAL_SERDES_25G_INIT_H__
#define __AL_HAL_SERDES_25G_INIT_H__

#include "al_mod_hal_serdes_interface.h"

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

int al_mod_serdes_25g_fw_init(
	struct al_mod_serdes_grp_obj	*obj,
	const char			*fw,
	unsigned int			fw_size);

int al_mod_serdes_25g_group_cfg_25g_mode(struct al_mod_serdes_grp_obj *obj);
int al_mod_serdes_25g_group_cfg_10g_mode(struct al_mod_serdes_grp_obj *obj);

int al_mod_serdes_25g_group_ictl_pma_val_set(
	struct al_mod_serdes_grp_obj	*grp_obj,
	uint32_t			*ictl_pma_val,
	enum al_mod_serdes_clk_src		clk_src_r2l,
	enum al_mod_serdes_clk_src		clk_src_l2r,
	enum al_mod_serdes_clk_src		clk_src_core);

void al_mod_serdes_25g_group_lanes_tx_rx_pol_inv(
	struct al_mod_serdes_grp_obj	*grp_obj,
	al_mod_bool				*inv_tx_lanes,
	al_mod_bool				*inv_rx_lanes);

/* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif

/* *INDENT-ON* */
#endif

/** @} end of SERDES group */

