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
#include "samples.h"
#include "stdlib.h"

/**
 * @defgroup group_msg_ipc_samples Code Samples
 * @ingroup group_udma
 * @{
 * msg_ipc.c: this file can be found in samples directory.
 *
 * This sample shows how one OS/Process could use the message IPC mechanism to
 * send and receive.
 * this code by itself should be running in two different processes or
 * operating systems in order to work.
 *
 * @code */
#include "al_hal_msg_ipc.h"
#include "al_hal_ssm_raid.h"

#define LOCAL_QID	3 /* Local Queue ID */

#define PACKET_SIZE	100

static struct al_ssm_dma raid_dma;

static struct pkt_queue tx_pkt_queue; /* TX packet SW queue */
static struct pkt_queue rx_pkt_queue; /* RX packet SW queue */

static unsigned char tx_bufs[MAX_PACKETS][PACKET_SIZE];
static unsigned char rx_bufs[MAX_PACKETS][PACKET_SIZE];

/* SW packet queue initialization */
static void msg_ipc_pkt_queue_init(struct pkt_queue *pkt_queue)
{
	pkt_queue->head = 0;
	pkt_queue->tail = 0;
}

/* SW packet queue get number of vacant entries */
static unsigned int msg_ipc_pkt_queue_num_vacant_get(
	struct pkt_queue *pkt_queue)
{
	unsigned int num =
		(MAX_PACKETS + pkt_queue->tail - pkt_queue->head - 1) %
		MAX_PACKETS;

	return num;
}

/* SW packet queue get number of pending entries */
unsigned int msg_ipc_pkt_queue_num_pending_get(
	struct pkt_queue *pkt_queue)
{
	unsigned int num =
		(MAX_PACKETS + pkt_queue->head - pkt_queue->tail) % MAX_PACKETS;

	return num;
}

/* SW packet queue get queue head packet (next to be filled) */
static struct al_msg_ipc_pkt *msg_ipc_pkt_queue_head_get(
	struct pkt_queue *pkt_queue)
{
	return &pkt_queue->pkts[pkt_queue->head];
}

/* SW packet queue get queue head index */
static unsigned int msg_ipc_pkt_queue_head_idx_get(
	struct pkt_queue *pkt_queue)
{
	return pkt_queue->head;
}

/* SW packet queue head index increment */
static void msg_ipc_pkt_queue_head_inc(
	struct pkt_queue *pkt_queue)
{
	pkt_queue->head = (pkt_queue->head + 1) % MAX_PACKETS;
}

/* SW packet queue get queue tail packet (last used) */
struct al_msg_ipc_pkt *msg_ipc_pkt_queue_tail_get(
	struct pkt_queue *pkt_queue)
{
	return &pkt_queue->pkts[pkt_queue->tail];
}

/* SW packet queue get queue tail index */
static unsigned int msg_ipc_pkt_queue_tail_idx_get(
	struct pkt_queue *pkt_queue)
{
	return pkt_queue->tail;
}

/* SW packet queue tail index increment */
static void msg_ipc_pkt_queue_tail_inc(
	struct pkt_queue *pkt_queue)
{
	pkt_queue->tail = (pkt_queue->tail + 1) % MAX_PACKETS;
}

/*
 * Message-based IPC initialization, based on the RAID acceleration UDMA, which
 * is the one supporting message-based IPC in Alpine
 */
int msg_ipc_init(
	void __iomem *udma_regs_base)
{
	struct al_ssm_dma_params raid_dma_params;

	int err;
	struct al_udma_q_params tx_params;
	struct al_udma_q_params rx_params;
	void *tx_dma_desc_virt;
	void *tx_dma_cdesc_virt;
	void *rx_dma_desc_virt;
	void *rx_dma_cdesc_virt;
	al_phys_addr_t tx_dma_desc_phys;
	al_phys_addr_t tx_dma_cdesc_phys;
	al_phys_addr_t rx_dma_desc_phys;
	al_phys_addr_t rx_dma_cdesc_phys;
	int descs_num = 8;
	int tx_cdesc_size = 8; /* Validate with documentation! */
	int rx_cdesc_size = 8; /* Validate with documentation! */
	struct al_udma_q *rx_dma_q;
	int i;

	/* Initialize SW queues */
	msg_ipc_pkt_queue_init(&tx_pkt_queue);
	msg_ipc_pkt_queue_init(&rx_pkt_queue);

	/* Initialize hardware */
	/* Replace with actual regs base after PCI resource allocation
	 * Device ID is extracted from the PCI configuration space address 0, bits 31:16
	 * Revision ID is extracted from the PCI configuration space address 8, bits 7:0
	 */
	raid_dma_params.rev_id = AL_RAID_REV_ID_0;
	raid_dma_params.udma_regs_base = udma_regs_base;

	raid_dma_params.name = (char *)malloc(strlen("raid_dev_name")+1);
	strcpy(raid_dma_params.name, "raid_dev_name");
	raid_dma_params.num_of_queues = 4;

	/* Init raid dma */
	err = al_ssm_dma_init(&raid_dma, &raid_dma_params);
	if (!err)
		return err;

	/* Start dma engine */
	err = al_ssm_dma_state_set(&raid_dma, UDMA_NORMAL);

	/* Allocate dma queue memory */

	/* Allocate memory for Tx submission descriptors */
	tx_dma_desc_virt = malloc(descs_num * sizeof(union al_udma_desc));
	/* Use os dma_map service to map tx_dma_desc_phys
	 * Here we just copy */
	tx_dma_desc_phys = (al_phys_addr_t)(uintptr_t)tx_dma_desc_virt;

	/* Allocate memory for Tx completion descriptors */
	tx_dma_cdesc_virt = malloc(descs_num * tx_cdesc_size);
	/* Use os dma_map service to map tx_dma_desc_phys
	 * Here we just copy */
	tx_dma_cdesc_phys = (al_phys_addr_t)(uintptr_t)tx_dma_cdesc_virt;

	/* Allocate memory for Rx submission descriptors */
	rx_dma_desc_virt = malloc(descs_num * sizeof(union al_udma_desc));
	/* Use os dma_map service to map rx_dma_desc_phys
	 * Here we just copy */
	rx_dma_desc_phys = (al_phys_addr_t)(uintptr_t)rx_dma_desc_virt;

	/* Allocate memory for Rx completion descriptors */
	rx_dma_cdesc_virt = malloc(descs_num * rx_cdesc_size);
	/* Use os dma_map service to map rx_dma_desc_phys
	 * Here we just copy */
	rx_dma_cdesc_phys = (al_phys_addr_t)(uintptr_t)rx_dma_cdesc_virt;

	/* Fill in dma queue params */
	tx_params.size = descs_num;
	tx_params.desc_base = (union al_udma_desc *)tx_dma_desc_virt;
	tx_params.desc_phy_base = tx_dma_desc_phys;
	tx_params.cdesc_base = (uint8_t *)tx_dma_cdesc_virt;
	tx_params.cdesc_phy_base = tx_dma_cdesc_phys;
	tx_params.cdesc_size = tx_cdesc_size;

	rx_params.size = descs_num;
	rx_params.desc_base = (union al_udma_desc *)rx_dma_desc_virt;
	rx_params.desc_phy_base = rx_dma_desc_phys;
	rx_params.cdesc_base = (uint8_t *)rx_dma_cdesc_virt;
	rx_params.cdesc_phy_base = rx_dma_cdesc_phys;
	rx_params.cdesc_size = rx_cdesc_size;

	/* Init dma queue using the params above */
	err = al_ssm_dma_q_init(&raid_dma, LOCAL_QID, &tx_params, &rx_params, AL_RAID_Q);

	rx_dma_q = al_ssm_dma_rx_queue_handle_get(&raid_dma, LOCAL_QID);

	/* Allocate RX packets */
	for (i = 0; i < MAX_PACKETS; i++) {
		struct al_msg_ipc_pkt *pkt;
		unsigned int head_idx;

		pkt = msg_ipc_pkt_queue_head_get(&rx_pkt_queue);
		head_idx = msg_ipc_pkt_queue_head_idx_get(&rx_pkt_queue);

		pkt->flags = AL_MSG_IPC_RX_FLAGS_INT;
		pkt->num_of_bufs = 1;
		pkt->bufs[0].addr =
			(al_phys_addr_t)(uintptr_t)rx_bufs[head_idx];
		pkt->bufs[0].len = PACKET_SIZE;

		msg_ipc_pkt_queue_head_inc(&rx_pkt_queue);

		err = al_msg_ipc_rx_buffer_add(
			rx_dma_q, &pkt->bufs[0], pkt->flags);
		if (err) {
			/* Err handling */
			return -ENOMEM;
		}
	}

	al_msg_ipc_rx_buffer_action(rx_dma_q, MAX_PACKETS);

	return 0;
}

/*
 * Message transmitter task - sends (3 * MAX_PACKETS) to UDMA #dst_udma_id,
 * queue #dst_qid
 */
int msg_ipc_transmitter(
	unsigned int dst_udma_id,
	unsigned int dst_qid)
{
	struct al_udma_q *tx_dma_q = al_ssm_dma_tx_queue_handle_get(
		&raid_dma,
		LOCAL_QID);
	int i;

	for (i = 0; i < 3 * MAX_PACKETS; i++) {
		struct al_msg_ipc_pkt *pkt;
		unsigned int head_idx;
		int num_descs;

		while (!msg_ipc_pkt_queue_num_vacant_get(&tx_pkt_queue))
			;

		pkt = msg_ipc_pkt_queue_head_get(&tx_pkt_queue);
		head_idx = msg_ipc_pkt_queue_head_idx_get(&tx_pkt_queue);

		pkt->udma_id = dst_udma_id;
		pkt->queue_id = dst_qid;
		pkt->flags = AL_MSG_IPC_TX_FLAGS_INT;
		pkt->num_of_bufs = 1;
		pkt->bufs[0].addr =
			(al_phys_addr_t)(uintptr_t)tx_bufs[head_idx];
		pkt->bufs[0].len = PACKET_SIZE;

		/* TODO: Fill buffer */

		msg_ipc_pkt_queue_head_inc(&tx_pkt_queue);

		num_descs = al_msg_ipc_tx_pkt_prepare(
			tx_dma_q, pkt);
		if (!num_descs) {
			/* Err handling */
			return -ENOMEM;
		}

		al_msg_ipc_tx_dma_action(tx_dma_q, num_descs);
	}

	return 0;
}

/* TX interrupt handler - in this example simply dequeues */
void msg_ipc_tx_int_handler(void)
{
	struct al_udma_q *tx_dma_q = al_ssm_dma_tx_queue_handle_get(
		&raid_dma,
		LOCAL_QID);

	int num_comps;

	do {
		num_comps = al_msg_ipc_comp_tx_get(tx_dma_q);

		for (; num_comps; num_comps--)
			msg_ipc_pkt_queue_tail_inc(&tx_pkt_queue);
	} while (num_comps);
}

/**
 * RX interrupt handler - on most operating systems it should only schedule
 * a high priority task (e.g. Linux tasklet) - in this sample handling is done
 * in interrupt context.
 */
void msg_ipc_rx_int_handler(void)
{
	struct al_udma_q *rx_dma_q = al_ssm_dma_rx_queue_handle_get(
		&raid_dma, LOCAL_QID);
	struct al_msg_ipc_pkt pkt;
	unsigned int head_idx;
	unsigned int tail_idx;
	unsigned int num_descs;
	int err;

	do {
		num_descs = al_msg_ipc_pkt_rx(rx_dma_q, &pkt);

		al_assert(num_descs <= 1);

		if (num_descs) {
			tail_idx = msg_ipc_pkt_queue_tail_idx_get(
				&rx_pkt_queue);

			al_assert(rx_bufs[tail_idx]);

			/* TODO: check flags and use received data */

			msg_ipc_pkt_queue_tail_inc(&rx_pkt_queue);

			/* Add a new RX buffer */

			head_idx = msg_ipc_pkt_queue_head_idx_get(
				&rx_pkt_queue);

			pkt.flags = AL_MSG_IPC_RX_FLAGS_INT;
			pkt.num_of_bufs = 1;
			pkt.bufs[0].addr =
				(al_phys_addr_t)(uintptr_t)rx_bufs[head_idx];
			pkt.bufs[0].len = PACKET_SIZE;

			msg_ipc_pkt_queue_head_inc(&rx_pkt_queue);

			err = al_msg_ipc_rx_buffer_add(
				rx_dma_q, &pkt.bufs[0], pkt.flags);
			if (err) {
				/* Err handling */
				return;
			}

			al_msg_ipc_rx_buffer_action(rx_dma_q, 1);
		}
	} while (num_descs);
}

/** @endcode */

/** @} */
