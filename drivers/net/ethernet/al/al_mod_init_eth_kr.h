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
 *  Ethernet
 *  @{
 * @file   al_mod_init_eth_kr.h
 *
 * @brief auto-negotiation and link training activation sequence
 *
 *
 */

#ifndef __AL_INIT_ETH_KR_H__
#define __AL_INIT_ETH_KR_H__

#include "al_mod_hal_eth_kr.h"
#include "al_mod_serdes.h"

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

/* timeout in mSec before auto-negotiation will be terminated */
#define AL_ETH_KR_AN_TIMEOUT		(500)
#define QARRAY_SIZE	3 /**< how many entries we want in our history array. */

/* mac algorithm state machine */
enum al_mod_eth_kr_mac_lt_state {
	TX_INIT = 0,	/* start of all */
	WAIT_BEGIN,	/* wait for initial training lock */
	DO_PRESET,	/* issue PRESET to link partner */
	DO_HOLD,	/* issue HOLD to link partner */
	/* preparation is done, start testing the coefficient. */
	QMEASURE,	/* EyeQ measurement. */
	QCHECK,		/* Check if measurement shows best value. */
	DO_NEXT_TRY,	/* issue DEC command to coeff for next measurement. */
	END_STEPS,	/* perform last steps to go back to optimum. */
	END_STEPS_HOLD,	/* perform last steps HOLD command. */
	COEFF_DONE,	/* done with the current coefficient updates.
			 * Check if another should be done. */
	/* end of training to all coefficients */
	SET_READY,	/* indicate local receiver ready */
	TX_DONE		/* transmit process completed, training can end. */
};

/* constants used for the measurement. */
enum al_mod_eth_kr_coef {
	AL_ETH_KR_COEF_C_MINUS,
	AL_ETH_KR_COEF_C_ZERO,
	AL_ETH_KR_COEF_C_PLUS,
};

struct al_mod_eth_kr_data {
	struct al_mod_hal_eth_adapter	*adapter;
	struct al_mod_serdes_grp_obj	*serdes_obj;
	enum al_mod_serdes_lane		lane;

	/* AutoNegotiation parameters */
	uint32_t bp_fec_mode;
	uint32_t np_fec_mode;
	uint32_t pause;

	/* receiver side data */
	struct al_mod_eth_kr_status_report_data status_report; /* report to response */
	struct al_mod_eth_kr_coef_up_data last_lpcoeff; /* last coeff received */

	/* transmitter side data */
	enum al_mod_eth_kr_mac_lt_state algo_state;	/* statemachine. */
	unsigned int qarray[QARRAY_SIZE];	/*< EyeQ measurements history */
	unsigned int qarray_cnt;        /*< how many entries in the array are
					    valid for compares yet. */
	enum al_mod_eth_kr_coef curr_coeff;
	unsigned int coeff_status_step; /*< status of coefficient during the
					    last DEC/INC command (before issuing
					     HOLD again). */
	unsigned int end_steps_cnt;     /*< number of end steps needed */
};

/**
 * execute Auto-negotiation process
 *
 * @param adapter pointer to the private structure
 * @param serdes_obj pointer to serdes group private structure
 * @param lane serdes's lane
 * @param an_adv pointer to the AN Advertisement Registers structure
 *        when NULL, the registers will not be updated.
 * @param partner_adv pointer to the AN Advertisement received from the lp
 *
 * @return 0 on success. otherwise on failure.
 */
int al_mod_eth_an_lt_execute(struct al_mod_hal_eth_adapter *adapter,
			 struct al_mod_serdes_grp_obj *serdes_obj,
			 enum al_mod_serdes_lane	lane,
			 struct al_mod_eth_an_adv	*an_adv,
			 struct al_mod_eth_an_adv   *partner_adv);

#ifdef __cplusplus
}
#endif
#endif /*__AL_INIT_ETH_KR_H__*/
