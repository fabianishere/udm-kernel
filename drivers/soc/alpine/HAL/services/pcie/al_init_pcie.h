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
/**
 * @ingroup grouppcie
 *  @{
 *  The PCIe Initialization HAL can be used in order to enable/configure/start
 *  PCIe port and physical functions.
 *
 * Common operation example:
 * @code
 *
 * // create init_pcie_handle
 * struct al_init_pcie_handle init_pcie_handle;
 *
 * // create link status for
 * struct al_pcie_link_status link_status = {0};
 *
 * // do some changes to the init_pcie_params defaults...
 * default_init_pcie_params.port_id = 0;
 * default_init_pcie_params.mode = AL_PCIE_OPERATING_MODE_EP;
 * default_init_pcie_params.max_lanes = 4;
 * default_init_pcie_params.max_num_of_pfs = 1;
 * default_init_pcie_params.port_params->link_params->max_speed =
 *							AL_PCIE_LINK_SPEED_GEN3;
 * // print the parameters
 * al_init_pcie_print_params(&default_init_pcie_params);
 *
 * // init the pcie port according to the init_pcie_params
 * al_init_pcie(&init_pcie_handle, &default_init_pcie_params, &link_status);
 *
 * @endcode
 *
 * @file   al_init_pcie.h
 * @brief C Header file for the PCIe initialization
 */

#ifndef AL_INIT_PCIE_H__
#define AL_INIT_PCIE_H__

#include "al_hal_pcie.h"
#include "al_hal_serdes_avg.h"
#ifdef __cplusplus
extern "C" {
#endif

#define AL_PCIE_PORT_NUM_MAX			8

#define AL_PCIE_PORT_PF_INIT_PARAMS_NUM_BARS	6

/** PCIE Port config space params */
struct al_pcie_port_pf_init_params {
	al_bool status;
	struct al_pcie_ep_id_params cfg_space; /**< config data */
	al_bool msix_enable;
	struct al_pcie_msix_params msix;
	struct al_pcie_ep_bar_params bars[AL_PCIE_PORT_PF_INIT_PARAMS_NUM_BARS];
	uint64_t target_addr[AL_PCIE_PORT_PF_INIT_PARAMS_NUM_BARS];
};

/** PCIe port Configuration */
struct al_pcie_port_init_params {
	int port_idx; /**< Pcie port number */
	al_bool status; /**< port status: if enabled or disabled */
	int present; /**< Card Connected */
	enum al_pcie_link_speed max_speed; /**< Max speed */
	int num_lanes; /**< Number of lanes */
	al_bool ep; /**< Is endpoint */
	al_bool iof_regs_access_en; /**< Enable access to I/O fabric regs */
	unsigned int ref_clk_idx; /** < ref clock index */
	const char *link_init_stage; /**< link init stage */
	const char *port_init_stage; /**< port init stage */
	al_bool port_init_found; /**< if port_init_stage found */
	struct al_serdes_grp_obj *serdes_lanes[AL_SRDS_LANES_PER_COMPLEX_MAX]; /**< serdes groups */
	unsigned int serdes_num_lanes; /**< num serdes lanes */

	const char *cfg_space_init_stage; /**< config init stage */
	/**< PCIE config pf data */
	struct al_pcie_port_pf_init_params pf_params[AL_MAX_NUM_OF_PFS];

	al_bool sris_config_status; /**< sris init status */
	const char *sris_config_stage; /**< sris params init stage */
	/**< SRIS params */
	struct al_pcie_sris_params pcie_sris_params;
};

/** PCIe configuration */
struct al_pcie_init_params_adv {
	al_bool pcie_rc_manual_init; /**< if to manually init PCIE RC */
	unsigned int pcie_rc_link_pre_delay; /**< PCIe RC delay before link up */
	unsigned int pcie_rc_link_up_timeout; /**< Wait for link timeout */
	unsigned int pcie_rc_link_post_delay; /**< PCIe delay after link up */
	al_bool pcie_rc_link_retry; /**< PCIe RC retry link */
	al_bool pcie_rc_retrain_if_not_full_width; /**< PCIe RC retrain link */

	struct al_pcie_port_init_params port_params[AL_PCIE_PORT_NUM_MAX]; /**< PCIe port params */

	al_bool teardown; /**< teardown to allow investigation of PCIe core and/or SerDes */
	al_bool reinit; /**< if to reinit link if already started */
	al_bool skip_serdes_ical; /**< skip triggering ical process in serdes */
};

/**
 * @brief	Init PCIe parameters include all parameters that can be
 *		configured as part of the enable/configure/start PCIe port and
 *		physical/virtual functions
 *
 * @param	port_id: the port id
 *			(default: 0)
 * @param	pcie_reg_base: pcie base registers
 *			(default: AL_SB_PCIE_BASE(port_id))
 * @param	pbs_reg_base: pbs base registers
 *			(default: AL_PBS_REGFILE_BASE)
 * @param	mode: operating mode
 *			(default: EP)
 * @param	max_lanes: number of max lanes supported
 *			(default: 8)
 * @param	max_num_of_pfs: number of PFs supported
 *			(default: 1)
 * @param	reads_config: inbound header credit and outstanding outbound reads
 *			(default: NULL - does no read configuration)
 * @param	port_params: port configuration params used to config the port
 *			(default: see al_init_pcie_params.h)
 * @param	pf_params: an array of PF params to config each PF
 *			(default: see al_init_pcie_params.h)
 * @param	start_link: AL_TRUE to start link after configuration
 *			(default: AL_TRUE)
 * @param	wait_for_link_timeout_ms: milliseconds to wait after link is started
 *			(default: 1ms)
 * @param	wait_for_link_silent_fail: don't print an error upon failure to establish link
 *			(default: AL_FALSE)
 * @param	crrs_en: enable deferring incoming configuration requests
 *			(default: AL_FALSE)
 * @param	ex_params: extended params
 * 			(default: NULL)
 * @param	reinit_link: enforce link re-initialization if link already started/up
 *			(default: AL_FALSE)
 * @param	skip_teardown: skip teardown to allow investigation of PCIe core and/or SerDes
 *			(default: AL_FALSE)
 * @param	serdes_grp_objs: an array of serdes group pointers to be initialized
 *			(default: NULL)
 *			Note: relevant only for Alpine V3
 * @param	serdes_grp_objs_num: serdes groups array length
 *			(default: 0)
 *			Note: relevant only for Alpine V3
 * @param	skip_serdes_ical : skip triggering ical process in serdes
 *			(default: AL_FALSE)
 *			Note: relevant only for Alpine V3
 * @param	retrain_if_not_full_width
 *		retrigger training if link doesn't include all detected lanes
 *		(default: AL_FALSE)
 * @param	nb_regs_base: nb base registers
 *			(default: AL_NB_SERVICE_BASE)
 * @param	iof_regs_access_en: enable access to io_fabric registers for port without direct
 *				    access to them (currently allowed only for port3).
 *				    RESTRICTIONS:
 *				    - this can be set to true only when operating mode is EP mode.
 *				    - when enabling this feature it is prohibted to send outbound
 *				      PCIe transactions directly from CPU (and not via UDMA) while
 *				      an inbound transaction to an io_fabric reg is received.
 *			(default: AL_FALSE)
 */
struct al_init_pcie_params {
	unsigned int					port_id;
	void						*pcie_reg_base;
	void						*pbs_reg_base;
	enum al_pcie_operating_mode			mode;
	unsigned int					max_lanes;
	unsigned int					max_num_of_pfs;
	struct al_pcie_ib_hcrd_os_ob_reads_config	*reads_config;
	struct al_pcie_port_config_params		*port_params;
	struct al_pcie_pf_config_params			*pf_params[AL_MAX_NUM_OF_PFS];
	al_bool						start_link;
	unsigned int					wait_for_link_timeout_ms;
	al_bool						wait_for_link_silent_fail;
	al_bool						crrs_en;
	void						*ex_params;
	al_bool						reinit_link;
	al_bool						skip_teardown;
	struct al_serdes_grp_obj			**serdes_grp_objs;
	unsigned int					serdes_grp_objs_num;
	al_bool						skip_serdes_ical;
#define AL_PCIE_INIT_PARAMS_HAS_RETRAIN_IF_NOT_FULL_WIDTH
	al_bool						retrain_if_not_full_width;
	void						*nb_regs_base;
	al_bool						iof_regs_access_en;
};

/**
 * A single structure that has both port and physical functions handles
 */
struct al_init_pcie_handle {
	struct al_pcie_port pcie_port;
	struct al_pcie_pf pcie_pf[AL_MAX_NUM_OF_PFS];
};

/**
 * The default initialization parameters
 */
extern struct al_init_pcie_params default_init_pcie_params;

/**
 * Initialize PCIe Port and PFs
 * @param  init_pcie_handle	the pcie init handle
 * @param  init_pcie_params	pcie init Port and PFs params
 * @param  link_status		the link status result
 * @return			0 if no error found
 */
int al_init_pcie(struct al_init_pcie_handle *init_pcie_handle,
		struct al_init_pcie_params *init_pcie_params,
		struct al_pcie_link_status *link_status);

/**
 * Initialize PCIE port and PFs
 * @param init_pcie_handle	the pcie init handle
 * @param pcie_params		pcie init port and pfs params from DT
 * @param link_status		the link status result
 * @param port_idx		pcie port index
 * @param current_stage		Current stage, could be: UBOOT, STAGE2, STAGE3, BOOT_APP
 * @param performed		Mark if we performed init
 *
 * @return			0 if nor error found
 */
int al_pcie_init_adv(struct al_init_pcie_handle *init_pcie_handle,
		 struct al_pcie_init_params_adv *pcie_params,
		 struct al_pcie_link_status *link_status,
		 int port_idx,
		 char *current_stage,
		 al_bool *performed);
#ifdef __cplusplus
}
#endif

#endif /* AL_INIT_PCIE_H__ */

/**  @} */
