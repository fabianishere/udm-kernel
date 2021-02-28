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
 * @defgroup group_udma_fast_samples Code Samples
 * @ingroup group_udma_fast
 * @{
 * udma_fast.c: this file can be found in samples directory.
 *
 * @code */
#include "al_hal_udma_fast.h"

static struct al_ssm_dma_params ssm_dma_params;
static struct al_ssm_dma ssm_dma;
static struct al_udma_q *tx_udma_q[2];
static struct al_udma_q *rx_udma_q[2];

/* Initialize RAID controller queue 0 */
int ssm_init(void)
{
	int rc;
	struct al_udma_q_params tx_params;
	struct al_udma_q_params rx_params;
	void *tx_dma_desc_virt, *rx_dma_desc_virt, *rx_dma_cdesc_virt;
	al_phys_addr_t tx_dma_desc_phys, rx_dma_desc_phys, rx_dma_cdesc_phys;
	int descs_num = 8;
	int tx_cdesc_size = 8; /* Validate with documentation! */
	int rx_cdesc_size = 8; /* Validate with documentation! */

	/*
	 * Replace with actual regs base after PCI resource allocation
	 * Device ID is extracted from the PCI configuration space address 0, bits 31:16
	 * Revision ID is extracted from the PCI configuration space address 8, bits 7:0
	 */
	ssm_dma_params.rev_id = 0; /* Change according to adapter */
	ssm_dma_params.udma_regs_base = NULL;

	strcpy(ssm_dma_params.name, "ssm_dev_ame");
	ssm_dma_params.num_of_queues = 4;

	/* Init ssm dma */
	rc = al_ssm_dma_init(&ssm_dma, &ssm_dma_params);
	if (!rc)
		return rc;

	/* Start dma engine */
	rc = al_ssm_dma_state_set(&ssm_dma, UDMA_NORMAL);

	/* Allocate dma queue memory */
	/* Allocate memory for Tx submission descriptors */
	tx_dma_desc_virt = malloc(descs_num * sizeof(union al_udma_desc));
	/* Use os dma_map service to map tx_dma_desc_phys
	 * Here we just copy */
	tx_dma_desc_phys = (al_phys_addr_t)(unsigned long)tx_dma_desc_virt;

	/* Allocate memory for Rx submission descriptors */
	rx_dma_desc_virt = malloc(descs_num * sizeof(union al_udma_desc));
	/* Use os dma_map service to map rx_dma_desc_phys
	 * Here we just copy */
	rx_dma_desc_phys = (al_phys_addr_t)(unsigned long)rx_dma_desc_virt;

	/* Allocate memory for Rx completion descriptors */
	rx_dma_cdesc_virt = malloc(descs_num * rx_cdesc_size);
	/* Use os dma_map service to map rx_dma_desc_phys
	 * Here we just copy */
	rx_dma_cdesc_phys = (al_phys_addr_t)(unsigned long)rx_dma_cdesc_virt;

	/* Fill in dma queue params */
	tx_params.size = descs_num;
	tx_params.desc_base = (union al_udma_desc *)tx_dma_desc_virt;
	tx_params.desc_phy_base = tx_dma_desc_phys;
	tx_params.cdesc_base = NULL; /* don't use Tx completion ring */
	tx_params.cdesc_phy_base = 0;
	tx_params.cdesc_size = tx_cdesc_size;

	rx_params.size = descs_num;
	rx_params.desc_base = (union al_udma_desc *)rx_dma_desc_virt;
	rx_params.desc_phy_base = rx_dma_desc_phys;
	rx_params.cdesc_base = (uint8_t *)rx_dma_cdesc_virt;
	rx_params.cdesc_phy_base = rx_dma_cdesc_phys;
	rx_params.cdesc_size = rx_cdesc_size;

	/* Init dma queue 0 using the params above */
	rc = al_ssm_dma_q_init(&ssm_dma, 0, &tx_params, &rx_params, AL_MEM_CRC_MEMCPY_Q);

	/* Allocate dma queue memory */
	/* Allocate memory for Tx submission descriptors */
	tx_dma_desc_virt = malloc(descs_num * sizeof(union al_udma_desc));
	/* Use os dma_map service to map tx_dma_desc_phys
	 * Here we just copy */
	tx_dma_desc_phys = (al_phys_addr_t)(unsigned long)tx_dma_desc_virt;

	/* Allocate memory for Rx submission descriptors */
	rx_dma_desc_virt = malloc(descs_num * sizeof(union al_udma_desc));
	/* Use os dma_map service to map rx_dma_desc_phys
	 * Here we just copy */
	rx_dma_desc_phys = (al_phys_addr_t)(unsigned long)rx_dma_desc_virt;

	/* Allocate memory for Rx completion descriptors */
	rx_dma_cdesc_virt = malloc(descs_num * rx_cdesc_size);
	/* Use os dma_map service to map rx_dma_desc_phys
	 * Here we just copy */
	rx_dma_cdesc_phys = (al_phys_addr_t)(unsigned long)rx_dma_cdesc_virt;

	/* Fill in dma queue params */
	tx_params.size = descs_num;
	tx_params.desc_base = (union al_udma_desc *)tx_dma_desc_virt;
	tx_params.desc_phy_base = tx_dma_desc_phys;
	tx_params.cdesc_base = NULL; /* don't use Tx completion ring */
	tx_params.cdesc_phy_base = 0;
	tx_params.cdesc_size = tx_cdesc_size;

	rx_params.size = descs_num;
	rx_params.desc_base = (union al_udma_desc *)rx_dma_desc_virt;
	rx_params.desc_phy_base = rx_dma_desc_phys;
	rx_params.cdesc_base = (uint8_t *)rx_dma_cdesc_virt;
	rx_params.cdesc_phy_base = rx_dma_cdesc_phys;
	rx_params.cdesc_size = rx_cdesc_size;

	rc = al_ssm_dma_q_init(&ssm_dma, 1, &tx_params, &rx_params, AL_RAID_Q);

	return rc;
}

/* Prepare queue 0 for fast memcpy */
void ssm_udma_fast_init_memcpy(void)
{
	struct al_memcpy_transaction xaction;

	tx_udma_q[0] = al_ssm_dma_tx_queue_handle_get(&ssm_dma, 0);

	rx_udma_q[0] = al_ssm_dma_rx_queue_handle_get(&ssm_dma, 0);

	memset(&xaction, 0, sizeof(struct al_memcpy_transaction));

	al_udma_fast_memcpy_q_prepare(tx_udma_q[0], rx_udma_q[0], &xaction);
}

/* Prepare queue 1 for fast memset */
void ssm_udma_fast_init_memset(void)
{
	struct al_memset_transaction xaction;

	tx_udma_q[1] = al_ssm_dma_tx_queue_handle_get(&ssm_dma, 1);

	rx_udma_q[1] = al_ssm_dma_rx_queue_handle_get(&ssm_dma, 1);

	memset(&xaction, 0, sizeof(xaction));
	memset(xaction.data, 0, sizeof(xaction.data));
	xaction.default_len = 1024;

	al_udma_fast_memset_q_prepare(tx_udma_q[1], rx_udma_q[1], &xaction);
}

/* Fast memcopy submission */
int udma_fast_memcpy(int len, al_phys_addr_t src, al_phys_addr_t dst)
{
	union al_udma_desc *tx_desc;
	union al_udma_desc *rx_desc;
	int completed = 0;
	uint32_t flags;

	/* prepare rx desc */
	rx_desc = al_udma_desc_get(rx_udma_q[0]);

	flags = al_udma_ring_id_get(rx_udma_q[0]) <<
			AL_M2S_DESC_RING_ID_SHIFT;

	al_udma_fast_desc_flags_set(rx_desc, flags, AL_M2S_DESC_RING_ID_MASK);
	al_udma_fast_desc_len_set(rx_desc, len);
	al_udma_fast_desc_buf_set(rx_desc, dst, 0);

	/* submit rx desc */
	al_udma_desc_action_add(rx_udma_q[0], 1);

	/* prepare tx desc */
	tx_desc = al_udma_desc_get(tx_udma_q[0]);

	flags = al_udma_ring_id_get(tx_udma_q[0]) <<
			AL_M2S_DESC_RING_ID_SHIFT;

	al_udma_fast_desc_flags_set(tx_desc, flags, AL_M2S_DESC_RING_ID_MASK);
	al_udma_fast_desc_len_set(tx_desc, len);
	al_udma_fast_desc_buf_set(tx_desc, src, 0);

	/* submit tx desc */
	al_udma_desc_action_add(tx_udma_q[0], 1);

	/* wait for completion using polling */
	while (completed < 1)
		completed += al_udma_fast_completion(rx_udma_q[0], 1, 0);

	return 0;
}

/* Fast memset submission */
int udma_fast_memset(int len, al_phys_addr_t dst)
{
	union al_udma_desc *tx_desc;
	union al_udma_desc *rx_desc;
	int completed = 0;
	uint32_t flags;

	/* prepare rx desc */
	rx_desc = al_udma_desc_get(rx_udma_q[1]);

	flags = al_udma_ring_id_get(rx_udma_q[1]) <<
			AL_M2S_DESC_RING_ID_SHIFT;

	al_udma_fast_desc_flags_set(rx_desc, flags, AL_M2S_DESC_RING_ID_MASK);
	al_udma_fast_desc_len_set(rx_desc, len);
	al_udma_fast_desc_buf_set(rx_desc, dst, 0);

	/* submit rx desc */
	al_udma_desc_action_add(rx_udma_q[1], 1);

	/* prepare tx desc */
	tx_desc = al_udma_desc_get(tx_udma_q[1]);

	flags = al_udma_ring_id_get(tx_udma_q[1]) <<
			AL_M2S_DESC_RING_ID_SHIFT;

	al_udma_fast_desc_flags_set(tx_desc, flags, AL_M2S_DESC_RING_ID_MASK);
	al_udma_fast_desc_len_set(tx_desc, len);

	/* submit tx desc */
	al_udma_desc_action_add(tx_udma_q[1], 1);

	/* wait for completion using polling */
	while(completed < 1)
		completed += al_udma_fast_completion(rx_udma_q[1], 1, 0);

	return 0;
}

/** @endcode */

/** @} */
