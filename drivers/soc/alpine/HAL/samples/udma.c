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
 * @defgroup group_udma_samples Code Samples
 * @ingroup group_udma
 * @{
 * udma.c: this file can be found in samples directory.
 *
 * @code */
#include "al_hal_udma.h"

#define QUEUES_NUM 1
#define QUEUE_0 0

/* DESCS_NUM defines the size of each descriptor ring */
#define DESCS_NUM (1 << 12)

/*
 * CDESC_SIZE defines the size in bytes of the completion descriptor
 * in the Rx path (S2M)
 */
#define CDESC_SIZE 8

struct al_udma udma;

int udma_init()
{
	int ret;
	struct al_udma_params params;
	struct al_udma_q_params tx_q_params;
	dma_addr_t* tx_dma_desc, tx_dma_cdesc;

	/*
	 * Pointer to UDMA registers- should point to
	 * UDMA BAR
	 */
	/* params.udma_reg = 0x...; */
	params.type = UDMA_TX;
	params.num_of_queues = QUEUES_NUM;
	params.name = "UDMA Example";

	ret = al_udma_init(&udma, &params);
	if (ret < 0)
		return ret;

	/* Set up Q */
	/*
	 * dma_alloc_coherent is a Linux Kernel DMA API function
	 * capable of allocating a consistent region of memory that
	 * can be written by the HW device/cpu and read immediately
	 * by the other with no caching effects (See DMA-API.txt in the
	 * Linux Kernel documentation for more info).
	 *
	 * The function returns a pointer in virtual address space
	 * through its return value and a pointer in physical (HW)
	 * address space through its third parameter (tx_dma_desc below)
	 */
	tx_q_params.rev_id = 0;
	tx_q_params.size = DESCS_NUM;
	tx_q_params.desc_base =
		dma_alloc_coherent(NULL,
	 					   DESCS_NUM*sizeof(union al_udma_desc),
	 					   &tx_dma_desc,
	 					   GFP_KERNEL);
	tx_q_params.desc_phy_base = tx_dma_desc;
	tx_q_params.cdesc_base =
		dma_alloc_coherent(NULL,
	 					   DESCS_NUM*CDESC_SIZE,
	 					   &tx_dma_cdesc,
	 					   GFP_KERNEL);
	tx_q_params.cdesc_phy_base = tx_dma_cdesc;
	tx_q_params.cdesc_size = CDESC_SIZE;


	ret = al_udma_q_init(&udma, QUEUE_0, &tx_q_params);
	if (ret < 0)
		return ret;

	/* Enable the UDMA */
	return al_udma_state_set(&udma, UDMA_NORMAL);
}

int udma_action()
{
	/* Submit a new TX descriptor to the UDMA HW */
	struct al_udma_q* tx_q;
	union al_udma_desc* tx_desc;
	uint32_t flags_len = 0;
	int ret;

	ret = al_udma_q_handle_get(&udma, QUEUE_0, &tx_q);
	if (ret < 0)
		return ret;

	tx_desc = al_udma_desc_get(tx_q);

	/* Get the ring id */
	flags_len |= al_udma_ring_id_get(tx_q)
				<< AL_M2S_DESC_RING_ID_SHIFT;

	/* Set this flag for the first descriptor */
	flags_len |= AL_M2S_DESC_FIRST;

	/* Set this flag for the last descriptor */
	flags_len |= AL_M2S_DESC_LAST;

	/*
	 * Pass a data buffer- suppose it's called bufa
	 * and its length is buf_len
	 *
	 */
	flags_len |= buf_len & AL_M2S_DESC_LEN_MASK;

	tx_desc->tx.len_ctrl = swap32_to_le(flags_len);
	tx_desc->tx.buf_ptr = swap64_to_le(buf);
	tx_desc->tx.meta_ctrl = 0;

	/* Prepare more TX descriptors here, if necessary... */

	/* Finally- add descriptors to submission queue */
	return al_udma_desc_action_add(tx_q, 1);
}

int udma_complete()
{
	/*
	 * We're here because we have a new descriptor
	 * waiting for us in the completion ring
	 * This is typically called from within the
	 * interrupt routine or could be used in a polling
	 * mechanism.
	 */

	struct al_udma_q* q;
	union al_udma_cdesc* cdesc;
	uint32_t cdesc_count;
	int ret;

	ret = al_udma_q_handle_get(&udma, QUEUE_0, &q);
	if (ret < 0)
		return ret;

	cdesc_count = al_udma_cdesc_packet_get(q, &cdesc);

	/*
	 * Handle all cdescs above one by one...
	 * Use al_cdesc_next() to iterate through all of them
	 */

	return al_udma_cdesc_ack(q, cdesc_count);
}

/** @endcode */
/** @} */
