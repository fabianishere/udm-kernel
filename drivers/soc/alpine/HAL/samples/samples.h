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

#include "al_hal_types.h"
#include "al_hal_msg_ipc.h"
#include "al_hal_eth.h"

/**
 * Crypto Sample
 */
int crypto_init(void);
int crypto_xaction_submit(const uint8_t *key, size_t keylen,
		al_phys_addr_t buf, uint32_t nbytes);

/**
 * msg IPC Sample
 */
#define MAX_PACKETS	10 /* Number of packets in the SW ring */
/* Packet SW queue */
struct pkt_queue {
	struct al_msg_ipc_pkt pkts[MAX_PACKETS];
	unsigned int head;
	unsigned int tail;
};

unsigned int msg_ipc_pkt_queue_num_pending_get(
	struct pkt_queue *pkt_queue);
struct al_msg_ipc_pkt *msg_ipc_pkt_queue_tail_get(
	struct pkt_queue *pkt_queue);
int msg_ipc_init(
	void __iomem *udma_regs_base);
int msg_ipc_transmitter(
	unsigned int dst_udma_id,
	unsigned int dst_qid);
void msg_ipc_tx_int_handler(void);
void msg_ipc_rx_int_handler(void);

/**
 * SSM and UDMA fast samples
 */
int ssm_init(void);
void ssm_udma_fast_init_memcpy(void);
void ssm_udma_fast_init_memset(void);
int udma_fast_memcpy(int len, al_phys_addr_t src, al_phys_addr_t dst);
int udma_fast_memset(int len, al_phys_addr_t dst);

/**
 * ETH sample
 */
int eth_init_rx_flow_steering(struct al_hal_eth_adapter *eth_adapter);

/**
 * Statistics sample
 */
int stats_init(void);
void stats_loop_cpu_0(void);
void stats_loop_cpu_other(void);
void stats_sys_fabric_handler(void);
void stats_udma_ssmae0_handler(void);
void stats_unit_adapter_ssmae0_handler(void);
void stats_udma_eth0_handler(void);
void stats_unit_adapter_eth0_handler(void);
void stats_pbs_handler(void);
void stats_eth0_handler(void);
void stats_ssmae0_handler(void);

/** Thermal sensor sample */
int thermal_sensor_sample(void);

/**
 * PMDT sample
 */
int pmdt_config_test(void);
void pmdt_run_test(void);
void pmdt_service_run(void);

