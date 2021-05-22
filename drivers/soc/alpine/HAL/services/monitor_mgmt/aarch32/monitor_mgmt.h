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

#ifndef __MONITOR_MGMT_H__
#define __MONITOR_MGMT_H__

#include "al_init_gicv2_regs.h"

/*******************************************************************************
 ** Secure monitor API
 ******************************************************************************/
#define MONITOR_MGMT_REQ_RUN_SECURE	0
#define MONITOR_MGMT_REQ_REGISTER_FIQ	1
#define MONITOR_MGMT_REQ_REGISTER_SMC	2
#define MONITOR_MGMT_REQ_CHANGE_SP	3

#define MONITOR_MGMT_REQ_MASK		0xff

#ifndef __ASSEMBLER__

#define AL_MONITOR_HAS_FIQ_HANDLER
typedef void (*al_monitor_fiq_handler)(uint32_t irq, enum al_nb_gic_id gic);

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

void smc_init(void);

#endif /*!__ASSEMBLER__*/

#endif /* __MONITOR_MGMT_H__ */
