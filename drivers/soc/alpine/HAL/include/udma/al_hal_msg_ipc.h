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
 * @defgroup group_msg_ipc Message-based IPC API
 * @ingroup group_udma
 * @{
 * @file   al_hal_msg_ipc.h
 *
 * @brief Header file for the message based IPC HAL
 *
 * This Message-based Inter-Process-Communication HAL provide IPC services
 * between multiple Operating Systems, or Multiple Processes, that do not share
 * memory regions.
 * In Alpine, it utilizes and shares same UDMA engines as the RAID controller.
 * Each side of the IPC "owns" a UDMA, i.e. the UDMA register space is owned by
 * the specific Operating-System or Process, and it sends a message (packet)
 * while identifying the target UDMA that could be owned by a different
 * Operating-System or Process. The source and destination UDMA are completely
 * isolated and each could have access to different memory region.
 * The IPC mechanism assumes all sides of the communication channel are up and
 * running, and its up to the higher level software to set that up (for
 * example, using the UDMA mailbox mechanism).
 * As this is based on the UDMA architecture, it benefits from all the UDMA
 * capability including linked-list of source and destination buffers, flexible
 * buffer size and address alignment, message-signaled interrupts etc.
 *
 */

#ifndef __AL_HAL_MSG_IPC_H__
#define __AL_HAL_MSG_IPC_H__

#include "al_hal_udma.h"

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

#define AL_MSG_IPC_PKT_MAX_BUFS		AL_UDMA_DEFAULT_MAX_ACTN_DESCS_MAX

/* Packet Tx flags */
#define AL_MSG_IPC_TX_FLAGS_INT		AL_M2S_DESC_INT_EN
#define AL_MSG_IPC_TX_FLAGS_NO_SNOOP	AL_M2S_DESC_NO_SNOOP_H

/* Packet Rx flags when adding buffer to receive queue */
#define AL_MSG_IPC_RX_FLAGS_INT		AL_M2S_DESC_INT_EN
#define AL_MSG_IPC_RX_FLAGS_NO_SNOOP	AL_M2S_DESC_NO_SNOOP_H

/* Packet Rx flags set by HW when receiving packet */
#define AL_MSG_IPC_RX_ERROR		AL_BIT(31)

/** packet structure. used for packet transmission and reception */
struct __cache_aligned al_msg_ipc_pkt {
	uint32_t flags; /**< see flags above, depends on context(tx or rx) */
	uint8_t num_of_bufs;

	/*
	 * When a packet is transmitted, the udma_id and queue_id will indicate
	 * the target udma to send it to. It's up to the upper layer to make
	 * sure the receive udma is up and running.
	 * When the packet is received, the udma_id and queue_id indicate the
	 * source of udma channel that sent the packet.
	 */
	unsigned int udma_id;
	unsigned int queue_id;
	
	struct al_buf bufs[AL_MSG_IPC_PKT_MAX_BUFS];
};

/* TX */
/**
 * prepare packet descriptors in tx queue.
 *
 * This functions prepares the descriptors for the given packet in the tx
 * submission ring. the caller must call al_msg_ipc_tx_pkt_action() below
 * in order to notify the hardware about the new descriptors.
 *
 * @param tx_dma_q pointer to UDMA tx queue
 * @param pkt the packet to transmit, including its destination
 *
 * @return number of descriptors used for this packet, 0 if no free room in the
 *         descriptors ring
 */
int al_msg_ipc_tx_pkt_prepare(
	struct al_udma_q	*tx_dma_q,
	struct al_msg_ipc_pkt	*pkt);

/**
 * Trigger the DMA about previously added tx descriptors.
 *
 * @param tx_dma_q pointer to UDMA tx queue
 * @param tx_descs number of descriptors to notify the DMA about.
 * the tx_descs can be sum of descriptor numbers of multiple prepared packets,
 * this way the caller can use this function to notify the DMA about multiple
 * packets.
 */
static inline void al_msg_ipc_tx_dma_action(
	struct al_udma_q	*tx_dma_q,
	unsigned int		tx_descs)
{
	al_udma_desc_action_add(tx_dma_q, tx_descs);
}

/**
 * get the number of completed tx descriptors, upper layer should derive from
 * this information which packets were completed.
 *
 * @param tx_dma_q pointer to UDMA tx queue
 *
 * @return number of completed tx descriptors.
 */
int al_msg_ipc_comp_tx_get(struct al_udma_q *tx_dma_q);

/* RX */
/**
 * add unused buffer to receive queue
 *
 * @param rx_dma_q pointer to UDMA rx queue
 * @param buf pointer to data buffer
 * @param flags bitwise of AL_MSG_IPC_RX_FLAGS
 * NULL.
 *
 * @return 0 on success. otherwise on failure.
 */
int al_msg_ipc_rx_buffer_add(
	struct al_udma_q	*rx_dma_q,
	struct al_buf		*buf,
	unsigned int		flags);

/**
 * notify the hw engine about rx descriptors that were added to the receive queue
 *
 * @param rx_dma_q pointer to UDMA rx queue
 * @param descs_num number of rx descriptors
 */
static inline void al_msg_ipc_rx_buffer_action(
	struct al_udma_q	*rx_dma_q,
	unsigned int		descs_num)
{
	al_udma_desc_action_add(rx_dma_q, descs_num);
}

/**
 * get packet from RX completion ring
 *
 * @param rx_dma_q pointer to UDMA rx queue
 * @param pkt pointer to a packet data structure, this function fills this
 * structure with the information about the received packet. the buffers
 * structures filled only with the length of the data written into the buffer,
 * the address fields are not updated as the upper layer can retrieve this
 * information by itself because the hardware uses the buffers in the same order
 * were those buffers inserted into the ring of the receive queue.
 * this structure should be allocated by the caller function.
 *
 * @return return number of descriptors or 0 if no completed packet found.
 */
unsigned int al_msg_ipc_pkt_rx(
	struct al_udma_q		*rx_dma_q,
	struct al_msg_ipc_pkt	*pkt);

/* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */
/** @} */
#endif

