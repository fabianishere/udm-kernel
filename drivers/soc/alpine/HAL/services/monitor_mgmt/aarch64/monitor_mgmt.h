/*******************************************************************************
Copyright (C) 2014 Annapurna Labs Ltd.

This software file is triple licensed: you can use it either under the terms of
Commercial, the GPL, or the BSD license, at your option.

a) If you received this File from Annapurna Labs and you have entered into a
   commercial license agreement (a "Commercial License") with Annapurna Labs,
   the File is licensed to you under the terms of the applicable Commercial
   License.

Alternatively,

b) This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public
   License along with this library; if not, write to the Free
   Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
   MA 02110-1301 USA

Alternatively,

c) Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

    *   Redistributions of source code must retain the above copyright notice,
	this list of conditions and the following disclaimer.

    *   Redistributions in binary form must reproduce the above copyright
	notice, this list of conditions and the following disclaimer in the
	documentation and/or other materials provided with the distribution.

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

#ifndef __MONITOR_MGMT_H__
#define __MONITOR_MGMT_H__

#include "al_init_gicv2_regs.h"

/*******************************************************************************
 ** Secure monitor API
 ******************************************************************************/
#define MONITOR_MGMT_REQ_RUN_SECURE	0x82000000
#define MONITOR_MGMT_REQ_REGISTER_FIQ	0x82000001
#define MONITOR_MGMT_REQ_REGISTER_SMC	0x82000002
#define MONITOR_MGMT_REQ_CHANGE_SP	0x82000003
#define MONITOR_MGMT_REQ_REGISTER_ABORT 0x82000004

#ifndef __ASSEMBLER__

#define AL_MONITOR_HAS_FIQ_HANDLER
typedef void (*al_monitor_fiq_handler)(uint32_t irq, enum al_nb_gic_id gic);
#define AL_MONITOR_HAS_ABORT_HANDLER
typedef void (*al_monitor_abort_handler)(void *);

int __monitor_run_secure(int, void* function);
#define monitor_run_secure(func) 					\
	({ int _tmp = __monitor_run_secure(MONITOR_MGMT_REQ_RUN_SECURE, func); _tmp; })


#define REGISTER_FIQ_FLAG_ENABLE (1 << 0)
#define REGISTER_FIQ_FLAG_EDGE   (1 << 1)
int __monitor_register_fiq(int, al_monitor_fiq_handler handler, int fiq, int flags);
#define  monitor_register_fiq(handler, fiq, flags) 			\
	({ int _tmp = __monitor_register_fiq(MONITOR_MGMT_REQ_REGISTER_FIQ, handler, fiq, flags); _tmp; })

int __monitor_register_smc(int, void* handler, int smc);
#define monitor_register_smc(handler, smc) \
	({ int _tmp = __monitor_register_smc(MONITOR_MGMT_REQ_REGISTER_SMC, handler, smc); _tmp; })

/* DO NOT call this from inside the monitor */
int __monitor_replace_sp(int, void* new_sp);
#define monitor_replace_sp(new_sp) \
	({ int _tmp = __monitor_replace_sp(MONITOR_MGMT_REQ_CHANGE_SP, new_sp); _tmp; })

int __monitor_register_abort(int, al_monitor_abort_handler handler);
#define monitor_register_abort(handler)	\
	({ int _tmp =				\
		__monitor_register_abort(MONITOR_MGMT_REQ_REGISTER_ABORT, handler); _tmp; })

void smc_init(void);

#endif /*!__ASSEMBLER__*/

#endif /* __MONITOR_MGMT_H__ */

