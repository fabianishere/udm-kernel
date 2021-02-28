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
 *  @{
 * @file   al_hal_ssm.c
 *
 */

#include "al_hal_ssm.h"
#include "al_hal_ssm_crc_memcpy.h"
#include "al_hal_ssm_crypto.h"
#include "al_hal_ssm_raid.h"
#include "al_hal_udma_iofic.h"
#include "al_hal_unit_adapter.h"

/* Crypto register base offset */
#define AL_CRYPTO_V1_REGS_BASE_OFFSET			0x800
#define AL_CRYPTO_V1_CRC_REGS_BASE_OFFSET(crc_idx)	(0x200 * (crc_idx))

/* Raid registers base address offsets */
#define AL_RAID_V1_REGS_BASE_OFFSET			0x0
#define AL_RAID_V1_CRC_REGS_BASE_OFFSET(crc_idx)	(0x400 + (0x400 * (crc_idx)))

/* SSM registers base address offsets */
#define AL_SSM_V2_CRYPTO_REGS_BASE_OFFSET		0x4000
#define AL_SSM_V2_RAID_REGS_BASE_OFFSET			0x5000
#define AL_SSM_V2_CRC_REGS_BASE_OFFSET(crc_idx)		(0x1000 * (crc_idx))

#define AL_SSM_V3_CRYPTO_REGS_BASE_OFFSET		0x10000
#define AL_SSM_V3_RAID_REGS_BASE_OFFSET			0x30000
#define AL_SSM_V3_CRC_REGS_BASE_OFFSET(crc_idx)		AL_SSM_V2_CRC_REGS_BASE_OFFSET(crc_idx)

/******************************************************************************
 ******************************************************************************/
static int al_ssm_dma_init_aux(
	struct al_ssm_dma		*ssm_dma,
	struct al_ssm_dma_params	*params,
	al_bool                         init_device)
{
	struct al_m2m_udma_params m2m_params;
	int rc;
	unsigned int udma_rev_id;

	udma_rev_id = al_udma_revision_get(params->udma_regs_base);

	ssm_dma->rev_id = params->rev_id;
	ssm_dma->ssm_max_src_descs = AL_SSM_MAX_SRC_DESCS_REV(udma_rev_id);
	ssm_dma->ssm_max_dst_descs = AL_SSM_MAX_DST_DESCS_REV(udma_rev_id);

	m2m_params.name = params->name;
	m2m_params.udma_regs_base = params->udma_regs_base;
	m2m_params.num_of_queues = params->num_of_queues;
	m2m_params.max_m2s_descs_per_pkt = ssm_dma->ssm_max_src_descs;
	m2m_params.max_s2m_descs_per_pkt = ssm_dma->ssm_max_dst_descs;

	if (init_device) {
		/* if adapter handler is provided configure the adapter for pf only*/
		if (params->unit_adapter && (!params->func_num) && (!params->skip_adapter_init))
			al_unit_adapter_init(params->unit_adapter);
		else if (!params->unit_adapter)
			al_warn("%s: non optimal adapter configuration\n", __func__);

		/* initialize the udma  */
		rc =  al_m2m_udma_init(&ssm_dma->m2m_udma, &m2m_params);
		if (rc != 0)
			al_err("failed to initialize udma, error %d\n", rc);

		/* unmask S2M no descriptor timeout error.
		 * this error is not opened in al_udma_init
		 * because in eth case its not a fatal error
		 * as UDMA will back pressure the MAC to drop packets
		 */
		al_udma_iofic_unmask_adv(al_ssm_dma_rx_udma_handle_get(ssm_dma),
				AL_UDMA_IOFIC_LEVEL_SECONDARY,
				AL_INT_GROUP_B,
				AL_INT_2ND_GROUP_B_S2M_NO_DESC_TO);
		al_iofic_abort_mask_clear(al_udma_iofic_reg_base_get_adv(
			al_ssm_dma_rx_udma_handle_get(ssm_dma),
			AL_UDMA_IOFIC_LEVEL_SECONDARY),
			AL_INT_GROUP_B,
			AL_INT_2ND_GROUP_B_S2M_NO_DESC_TO);

		/*
		 * Addressing RMN: 12057
		 *
		 * RMN description:
		 * Compression false interrupt assertion aborts SSMAE
		 * During AL7 Compression Device driver development and test with Alpine Linux, DMA hang was found.
		 * Specific compression/decompression falsely triggers an interrupt.The interrupt propagates to
		 * application status bits in the S2M completion, and from there to the DMA secondary interrupt
		 * controller, issuing an abort.
		 * Software flow:
		 * Mask the relevant interrupts:
		 * AL_INT_2ND_GROUP_A_M2S_STRM_RESP
		 * AL_INT_2ND_GROUP_B_S2M_STRM_DATA
		 */
		if (ssm_dma->rev_id == AL_SSM_REV_ID_REV3) { /* Relevant just for ALPINE_V3 */
			al_udma_iofic_mask_adv(al_ssm_dma_tx_udma_handle_get(ssm_dma),
				AL_UDMA_IOFIC_LEVEL_SECONDARY,
				AL_INT_GROUP_A,
				AL_INT_2ND_GROUP_A_M2S_STRM_RESP);
			al_udma_iofic_mask_adv(al_ssm_dma_rx_udma_handle_get(ssm_dma),
				AL_UDMA_IOFIC_LEVEL_SECONDARY,
				AL_INT_GROUP_B,
				AL_INT_2ND_GROUP_B_S2M_STRM_DATA);
		}

		return rc;
	} else {
		/* initialize the udma  */
		rc =  al_m2m_udma_handle_init(&ssm_dma->m2m_udma, &m2m_params);
		if (rc != 0)
			al_err("failed to initialize udma handle, error %d\n", rc);

		return rc;
	}
}

/******************************************************************************
 ******************************************************************************/
int al_ssm_dma_init(
	struct al_ssm_dma		*ssm_dma,
	struct al_ssm_dma_params	*params)
{
	al_dbg("ssm [%s]: initialize DMA\n", params->name);

	return al_ssm_dma_init_aux(ssm_dma, params, AL_TRUE);
}

/******************************************************************************
 ******************************************************************************/
int al_ssm_dma_handle_init(
	struct al_ssm_dma		*ssm_dma,
	struct al_ssm_dma_params	*params)
{
	al_dbg("ssm [%s]: initialize DMA handle\n", params->name);

	return al_ssm_dma_init_aux(ssm_dma, params, AL_FALSE);
}

/******************************************************************************
 ******************************************************************************/
int al_ssm_dma_q_init(struct al_ssm_dma		*ssm_dma,
		      uint32_t			qid,
		      struct al_udma_q_params	*tx_params,
		      struct al_udma_q_params	*rx_params,
		      enum al_ssm_q_type	q_type)
{
	int rc;

	al_dbg("ssm [%s]: Initialize queue %d\n",
		 ssm_dma->m2m_udma.name, qid);

	tx_params->adapter_rev_id = ssm_dma->rev_id;
	rx_params->adapter_rev_id = ssm_dma->rev_id;

	rc = al_m2m_udma_q_init(&ssm_dma->m2m_udma, qid, tx_params, rx_params);
	if (rc != 0)
		al_err("ssm [%s]: failed to initialize q %d, error %d\n",
			 ssm_dma->m2m_udma.name, qid, rc);
	else
		ssm_dma->q_types[qid] = q_type;

	return rc;
}

/******************************************************************************
 ******************************************************************************/
int al_ssm_dma_state_set(
	struct al_ssm_dma	*ssm_dma,
	enum al_udma_state	dma_state)
{
	int rc;

	rc =  al_m2m_udma_state_set(&ssm_dma->m2m_udma, dma_state);
	if (rc != 0)
		al_err("ssm [%s]: failed to change state, error %d\n",
			 ssm_dma->m2m_udma.name, rc);
	return rc;
}

/******************************************************************************
 ******************************************************************************/
int al_ssm_dma_handle_get(
	struct al_ssm_dma	*ssm_dma,
	enum al_udma_type	type,
	struct al_udma		**udma)
{
	return al_m2m_udma_handle_get(&ssm_dma->m2m_udma, type, udma);
}

/******************************************************************************
 ******************************************************************************/
struct al_udma *al_ssm_dma_tx_udma_handle_get(
	struct al_ssm_dma *ssm_dma)
{
	struct al_udma *udma;
	int err;

	err = al_m2m_udma_handle_get(&ssm_dma->m2m_udma, UDMA_TX, &udma);
	if (err)
		return NULL;

	return udma;
}

/******************************************************************************
 ******************************************************************************/
struct al_udma_q *al_ssm_dma_tx_queue_handle_get(
	struct al_ssm_dma	*ssm_dma,
	unsigned int		qid)
{
	struct al_udma *udma;
	int err;

	err = al_m2m_udma_handle_get(&ssm_dma->m2m_udma, UDMA_TX, &udma);
	if (err)
		return NULL;

	return &udma->udma_q[qid];
}

/******************************************************************************
 ******************************************************************************/
struct al_udma *al_ssm_dma_rx_udma_handle_get(
	struct al_ssm_dma *ssm_dma)
{
	struct al_udma *udma;
	int err;

	err = al_m2m_udma_handle_get(&ssm_dma->m2m_udma, UDMA_RX, &udma);
	if (err)
		return NULL;

	return udma;
}

/******************************************************************************
 ******************************************************************************/
struct al_udma_q *al_ssm_dma_rx_queue_handle_get(
	struct al_ssm_dma	*ssm_dma,
	unsigned int		qid)
{
	struct al_udma *udma;
	int err;

	err = al_m2m_udma_handle_get(&ssm_dma->m2m_udma, UDMA_RX, &udma);
	if (err)
		return NULL;

	return &udma->udma_q[qid];
}

/******************************************************************************
 ******************************************************************************/
int al_ssm_dma_action(struct al_ssm_dma *dma, uint32_t qid, int tx_descs)
{
	struct al_udma_q *tx_udma_q;
	int rc;

	rc = al_udma_q_handle_get(&dma->m2m_udma.tx_udma, qid, &tx_udma_q);
	al_assert(!rc); /* assert valid tx q handle */

	/* add tx descriptors */
	al_udma_desc_action_add(tx_udma_q, tx_descs);
	return 0;
}

/******************************************************************************
 ******************************************************************************/
void al_ssm_unit_regs_info_get(void *bars[6],
			       unsigned int dev_id,
			       unsigned int rev_id __attribute__((__unused__)),
			       struct al_ssm_unit_regs_info *unit_info)
{
	char *app_bar;
	int crc_idx;

	al_assert(unit_info);
	al_assert(bars);
	al_assert(bars[0]);
	al_assert(bars[4]);

	al_memset(unit_info, 0, sizeof(*unit_info));

	/* UDMA regs base is always BAR 0 address */
	unit_info->udma_regs_base = bars[0];

	app_bar = bars[4];

	/* Application regs base is always BAR 4 address */
	switch (dev_id) {
	/** V2 & V3 share the same device ID */
	case AL_CRYPTO_ALPINE_V2_DEV_ID:
		if (rev_id <= AL_SSM_REV_ID_REV2) {
			unit_info->crypto_regs_base =
				app_bar + AL_SSM_V2_CRYPTO_REGS_BASE_OFFSET;
			unit_info->raid_regs_base =
				app_bar + AL_SSM_V2_RAID_REGS_BASE_OFFSET;
			for (crc_idx = 0; crc_idx < AL_SSM_V1_V2_CRC_NUM; crc_idx++)
				unit_info->crc_regs_base[crc_idx] = app_bar +
						AL_SSM_V2_CRC_REGS_BASE_OFFSET(crc_idx);
		} else {
			unit_info->crypto_regs_base =
				app_bar + AL_SSM_V3_CRYPTO_REGS_BASE_OFFSET;
			unit_info->raid_regs_base =
				app_bar + AL_SSM_V3_RAID_REGS_BASE_OFFSET;
			for (crc_idx = 0; crc_idx < AL_SSM_V3_CRC_NUM; crc_idx++)
				unit_info->crc_regs_base[crc_idx] = app_bar +
						AL_SSM_V3_CRC_REGS_BASE_OFFSET(crc_idx);
		}

		break;
	case AL_CRYPTO_ALPINE_V1_DEV_ID:
		unit_info->crypto_regs_base =
			app_bar + AL_CRYPTO_V1_REGS_BASE_OFFSET;
		for (crc_idx = 0; crc_idx < AL_SSM_V1_V2_CRC_NUM; crc_idx++)
			unit_info->crc_regs_base[crc_idx] = app_bar +
					AL_CRYPTO_V1_CRC_REGS_BASE_OFFSET(crc_idx);
		break;
	case AL_RAID_DEV_ID:
		unit_info->raid_regs_base =
			app_bar + AL_RAID_V1_REGS_BASE_OFFSET;
		for (crc_idx = 0; crc_idx < AL_SSM_V1_V2_CRC_NUM; crc_idx++)
			unit_info->crc_regs_base[crc_idx] = app_bar +
					AL_RAID_V1_CRC_REGS_BASE_OFFSET(crc_idx);
		break;
	default:
		al_err("%s: unknown device ID %d\n", __func__, dev_id);
		al_assert(0);
	}
}

/******************************************************************************
 ******************************************************************************/
void al_ssm_error_ints_unmask(struct al_ssm_unit_regs_info *unit_info, unsigned int rev_id)
{
	if (unit_info->crypto_regs_base) {
		al_crypto_error_ints_unmask(rev_id,
			unit_info->crypto_regs_base,
			unit_info->udma_regs_base);
	}

	if (unit_info->raid_regs_base) {
		al_raid_error_ints_unmask(rev_id,
			unit_info->raid_regs_base,
			unit_info->udma_regs_base);
	}

	al_crc_memcpy_error_ints_unmask(rev_id,
			AL_SSM_V1_V2_CRC_NUM,
			unit_info->crc_regs_base,
			unit_info->udma_regs_base);
}
