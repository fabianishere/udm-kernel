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
 * @file   al_hal_udma_main.c
 *
 * @brief  Universal DMA HAL driver for main functions (initialization, data path)
 *
 */

#include <al_hal_udma.h>
#include <al_hal_udma_config.h>
#include <al_hal_udma_iofic.h>
#ifdef AL_ETH_EX
#include <al_hal_udma_config_ex.h>
#endif

#define AL_UDMA_Q_QUIESCE_TOUT	10000	/* Queue quiesce timeout [uSecs] */

#define UDMA_STATE_IDLE		0x0
#define UDMA_STATE_NORMAL	0x1
#define UDMA_STATE_ABORT	0x2
#define UDMA_STATE_RESERVED	0x3

const char *const al_udma_states_name[] = {
	"Disable",
	"Idle",
	"Normal",
	"Abort",
	"Reset"
};

#define AL_UDMA_INITIAL_RING_ID	1

/*  dma_q flags */
#define AL_UDMA_Q_FLAGS_NO_COMP_UPDATE	AL_BIT(1)

static void al_udma_set_defaults(struct al_udma *udma)
{
	uint8_t rev_id = udma->rev_id;

	if (udma->type == UDMA_TX) {
		struct al_udma_m2s_pkt_len_conf conf = {
			.encode_64k_as_zero = AL_TRUE,
			.max_pkt_size = UDMA_M2S_CFG_LEN_MAX_PKT_SIZE_MASK,
		};

		/*
		 * UDMA V4:
		 * Setting the data fifo depth to 32K (1024 beats of 256 bits)
		 * This allows the UDMA to have 128 outstanding writes
		 *
		 * UDMA V2:
		 * Setting the data fifo depth to 4K (256 beats of 128 bits)
		 * This allows the UDMA to have 16 outstanding writes
		 */
		if (rev_id >= AL_UDMA_REV_ID_2) {
			unsigned int num_beats = (rev_id >= AL_UDMA_REV_ID_4) ? 1024 : 256;

			al_reg_write32_masked(&udma->udma_regs->m2s.m2s_rd.data_cfg,
			      UDMA_M2S_RD_DATA_CFG_DATA_FIFO_DEPTH_MASK,
			      num_beats << UDMA_M2S_RD_DATA_CFG_DATA_FIFO_DEPTH_SHIFT);
		}

		/* Set M2S max number of outstanding transactions */
		if (rev_id >= AL_UDMA_REV_ID_4) {
			al_reg_write32_masked(&udma->udma_regs->m2s.axi_m2s.ostand_cfg,
				UDMA_AXI_M2S_OSTAND_CFG_MAX_DATA_RD_MASK |
				UDMA_AXI_M2S_OSTAND_CFG_MAX_DESC_RD_MASK |
				UDMA_AXI_M2S_OSTAND_CFG_MAX_COMP_REQ_MASK,
				(128 << UDMA_AXI_M2S_OSTAND_CFG_MAX_DATA_RD_SHIFT) |
				(128 << UDMA_AXI_M2S_OSTAND_CFG_MAX_DESC_RD_SHIFT) |
				(128 << UDMA_AXI_M2S_OSTAND_CFG_MAX_COMP_REQ_SHIFT));
		}

		/* set AXI timeout to 5M (~10 ms) */
		al_reg_write32(&udma->gen_axi_regs->cfg_1, 5000000);

		/* Ack time out */
		al_reg_write32(&udma->udma_regs->m2s.m2s_comp.cfg_application_ack, 0);

		/* set max packet size to maximum */
		al_udma_m2s_packet_size_cfg_set(udma, &conf);

		/* Set addr_hi selectors */
		if (rev_id == AL_UDMA_REV_ID_4) {
			struct udma_gen_ex_regs *gen_ex_regs =
				(struct udma_gen_ex_regs *)udma->gen_ex_regs;
			unsigned int i;

			for (i = 0; i < DMA_MAX_Q_V4; i++)
				al_reg_write32(&gen_ex_regs->vmpr_v4[i].tx_sel, 0xffffffff);
		}
	}

	if (udma->type == UDMA_RX) {
		/* Ack time out */
		al_reg_write32(&udma->udma_regs->s2m.s2m_comp.cfg_application_ack, 0);

		/* Set addr_hi selectors */
		if (rev_id == AL_UDMA_REV_ID_4) {
			struct udma_gen_ex_regs *gen_ex_regs =
				(struct udma_gen_ex_regs *)udma->gen_ex_regs;
			unsigned int i;

			for (i = 0; i < DMA_MAX_Q_V4; i++) {
				al_reg_write32(&gen_ex_regs->vmpr_v4[i].rx_sel[0], 0xffffffff);
				al_reg_write32(&gen_ex_regs->vmpr_v4[i].rx_sel[1], 0xffffffff);
				al_reg_write32(&gen_ex_regs->vmpr_v4[i].rx_sel[2], 0xffffffff);
			}
		}

		/* Set S2M max number of outstanding transactions */
		if (rev_id >= AL_UDMA_REV_ID_4) {
			al_reg_write32_masked(&udma->udma_regs->s2m.axi_s2m.ostand_cfg_rd,
				UDMA_AXI_S2M_OSTAND_CFG_RD_MAX_DESC_RD_OSTAND_MASK,
				(128 << UDMA_AXI_S2M_OSTAND_CFG_RD_MAX_DESC_RD_OSTAND_SHIFT));

			al_reg_write32_masked(&udma->udma_regs->s2m.axi_s2m.ostand_cfg_wr,
				UDMA_AXI_S2M_OSTAND_CFG_WR_MAX_DATA_WR_OSTAND_MASK |
				UDMA_AXI_S2M_OSTAND_CFG_WR_MAX_COMP_REQ_MASK,
				(128 << UDMA_AXI_S2M_OSTAND_CFG_WR_MAX_DATA_WR_OSTAND_SHIFT) |
				(128 << UDMA_AXI_S2M_OSTAND_CFG_WR_MAX_COMP_REQ_SHIFT));
		}
	}
}
/**
 * misc queue configurations
 *
 * @param udma_q udma queue data structure
 *
 * @return 0
 */
static int al_udma_q_config(struct al_udma_q *udma_q)
{
	uint32_t *reg_addr;
	uint32_t val;

	if (udma_q->udma->type == UDMA_TX) {
		reg_addr = &udma_q->q_regs->m2s_q.rlimit.mask;

		val = al_reg_read32(reg_addr);
		// enable DMB
		val &= ~UDMA_M2S_Q_RATE_LIMIT_MASK_INTERNAL_PAUSE_DMB;
		al_reg_write32(reg_addr, val);
	}
	return 0;
}

/**
 * set the queue's completion configuration register
 *
 * @param udma_q udma queue data structure
 *
 * @return 0
 */
static int al_udma_q_config_compl(struct al_udma_q *udma_q)
{
	uint32_t *reg_addr;
	uint32_t val;

	if (udma_q->udma->type == UDMA_TX)
		reg_addr = &udma_q->q_regs->m2s_q.comp_cfg;
	else
		reg_addr = &udma_q->q_regs->s2m_q.comp_cfg;

	val = al_reg_read32(reg_addr);

	if (udma_q->flags & AL_UDMA_Q_FLAGS_NO_COMP_UPDATE)
		val &= ~UDMA_M2S_Q_COMP_CFG_EN_COMP_RING_UPDATE;
	else
		val |= UDMA_M2S_Q_COMP_CFG_EN_COMP_RING_UPDATE;

	val |= UDMA_M2S_Q_COMP_CFG_DIS_COMP_COAL;

	al_reg_write32(reg_addr, val);

	/* set the completion queue size */
	if (udma_q->udma->type == UDMA_RX) {
		val = al_reg_read32(
				&udma_q->udma->udma_regs->s2m.s2m_comp.cfg_1c);
		val &= ~UDMA_S2M_COMP_CFG_1C_DESC_SIZE_MASK;
		/* the register expects it to be in words */
		val |= (udma_q->cdesc_size >> 2)
				& UDMA_S2M_COMP_CFG_1C_DESC_SIZE_MASK;
		al_reg_write32(&udma_q->udma->udma_regs->s2m.s2m_comp.cfg_1c
							, val);
	}
	return 0;
}

/**
 * reset the queues pointers (Head, Tail, etc) and set the base addresses
 *
 * @param udma_q udma queue data structure
 */
static int al_udma_q_set_pointers(struct al_udma_q *udma_q)
{
	/* reset the descriptors ring pointers */
	/* assert descriptor base address aligned. */
	al_assert((AL_ADDR_LOW(udma_q->desc_phy_base) &
		   ~UDMA_M2S_Q_TDRBP_LOW_ADDR_MASK) == 0);
	al_reg_write32(&udma_q->q_regs->rings.drbp_low,
		       AL_ADDR_LOW(udma_q->desc_phy_base));
	al_reg_write32(&udma_q->q_regs->rings.drbp_high,
		       AL_ADDR_HIGH(udma_q->desc_phy_base));

	al_reg_write32(&udma_q->q_regs->rings.drl, udma_q->size);

	/* if completion ring update disabled */
	if (udma_q->cdesc_base_ptr == NULL) {
		udma_q->flags |= AL_UDMA_Q_FLAGS_NO_COMP_UPDATE;
	} else {
		/* reset the completion descriptors ring pointers */
		/* assert completion base address aligned. */
		al_assert((AL_ADDR_LOW(udma_q->cdesc_phy_base) &
			   ~UDMA_M2S_Q_TCRBP_LOW_ADDR_MASK) == 0);
		al_reg_write32(&udma_q->q_regs->rings.crbp_low,
			       AL_ADDR_LOW(udma_q->cdesc_phy_base));
		al_reg_write32(&udma_q->q_regs->rings.crbp_high,
			       AL_ADDR_HIGH(udma_q->cdesc_phy_base));
	}
	al_udma_q_config_compl(udma_q);
	return 0;
}

/** enable/disable udma queue */
void al_udma_q_enable(struct al_udma_q *udma_q, int enable)
{
	uint32_t reg;

	al_assert(udma_q);

	reg = al_reg_read32(&udma_q->q_regs->rings.cfg);
	if (enable) {
		reg |= (UDMA_M2S_Q_CFG_EN_PREF | UDMA_M2S_Q_CFG_EN_SCHEDULING);
		udma_q->status = AL_QUEUE_ENABLED;
	} else {
		reg &= ~(UDMA_M2S_Q_CFG_EN_PREF | UDMA_M2S_Q_CFG_EN_SCHEDULING);
		udma_q->status = AL_QUEUE_DISABLED;
	}
	al_reg_write32(&udma_q->q_regs->rings.cfg, reg);
}

static enum al_udma_queue_status al_udma_q_status_get(struct al_udma_q *udma_q)
{
	if (al_udma_q_is_enabled(udma_q))
		return AL_QUEUE_ENABLED;

	return AL_QUEUE_DISABLED;

}

/************************ API functions ***************************************/


/* UDMA get revision */
unsigned int al_udma_revision_get(void __iomem *regs_base)
{
	struct udma_m2s_regs_v4 __iomem *m2s_regs = (struct udma_m2s_regs_v4 __iomem *)regs_base;
	uint32_t dma_version;

	al_assert(regs_base);

	dma_version = al_reg_read32(&m2s_regs->m2s_feature.dma_version);

	if (dma_version	< AL_UDMA_REV_ID_REV4) {
		struct unit_regs_v3 __iomem *unit_regs =
		(struct unit_regs_v3 __iomem *)regs_base;

		dma_version = (al_reg_read32(&unit_regs->gen.dma_misc.revision)
			& UDMA_GEN_DMA_MISC_REVISION_PROGRAMMING_ID_MASK) >>
			UDMA_GEN_DMA_MISC_REVISION_PROGRAMMING_ID_SHIFT;
	}

	return dma_version;
}

/* Initializations functions */

/**
 * Initialize UDMA handle and allow to read current statuses from registers
 */
static int al_udma_handle_init_aux(struct al_udma *udma, struct al_udma_params *udma_params,
	al_bool read_regs)
{
	int i;

	udma->rev_id = al_udma_revision_get(udma_params->udma_regs_base);
	if (udma->rev_id < AL_UDMA_REV_ID_4)
		udma->num_of_queues_max = DMA_MAX_Q_V3;
	else
		udma->num_of_queues_max = DMA_MAX_Q_V4;

	udma->type = udma_params->type;

	if (udma_params->num_of_queues == AL_UDMA_NUM_QUEUES_MAX)
		udma->num_of_queues = udma->num_of_queues_max;
	else
		udma->num_of_queues = udma_params->num_of_queues;

	if (udma->num_of_queues > udma->num_of_queues_max) {
		al_err("udma: invalid num_of_queues parameter\n");
		return -EINVAL;
	}

	if (udma->rev_id < AL_UDMA_REV_ID_4) {
		struct unit_regs_v3 __iomem *unit_regs =
			(struct unit_regs_v3 __iomem *)udma_params->udma_regs_base;

		udma->unit_regs_base = (void __iomem *)unit_regs;
		udma->gen_regs = &unit_regs->gen;
		udma->gen_axi_regs = &unit_regs->gen.axi;
		udma->gen_int_regs = &unit_regs->gen.interrupt_regs;
		udma->gen_ex_regs = &unit_regs->gen_ex;
		if (udma->type == UDMA_TX)
			udma->udma_regs = (union udma_regs *)&unit_regs->m2s;
		else
			udma->udma_regs = (union udma_regs *)&unit_regs->s2m;
	} else {
		struct unit_regs_v4 __iomem *unit_regs =
			(struct unit_regs_v4 __iomem *)udma_params->udma_regs_base;

		udma->unit_regs_base = (void __iomem *)unit_regs;
		udma->gen_regs = &unit_regs->gen;
		udma->gen_axi_regs = &unit_regs->gen.axi;
		udma->gen_int_regs = &unit_regs->gen.interrupt_regs;
		udma->gen_ex_regs = &unit_regs->gen_ex;
		if (udma->type == UDMA_TX)
			udma->udma_regs = (union udma_regs *)&unit_regs->m2s;
		else
			udma->udma_regs = (union udma_regs *)&unit_regs->s2m;
	}

	if (udma_params->name == NULL)
		udma->name = "";
	else
		udma->name = udma_params->name;

	for (i = 0; i < udma->num_of_queues_max; i++) {
		udma->udma_q[i].q_regs = (udma->type == UDMA_TX) ?
			(union udma_q_regs __iomem *)&udma->udma_regs->m2s.m2s_q[i] :
			(union udma_q_regs __iomem *)&udma->udma_regs->s2m.s2m_q[i];
		udma->udma_q[i].udma = udma;
		udma->udma_q[i].status = (read_regs ?
			al_udma_q_status_get(&udma->udma_q[i]) : AL_QUEUE_DISABLED);
	}

	udma->state = (read_regs ? al_udma_state_get(udma) : UDMA_DISABLE);

	return 0;
}

/*
 * Initialize the udma engine handle
 */
int al_udma_handle_init(struct al_udma *udma, struct al_udma_params *udma_params)
{
	al_assert(udma);
	al_assert(udma_params);

	return al_udma_handle_init_aux(udma, udma_params, AL_TRUE);
}

unsigned int al_udma_rev_id_get(struct al_udma *udma)
{
	al_assert(udma);

	return udma->rev_id;
}

/* Performance params printout */
void al_udma_perf_params_print(
	struct al_udma		*m2s_udma,
	struct al_udma		*s2m_udma)
{
	struct al_udma_m2s_desc_pref_conf m2s_conf;
	struct al_udma_s2m_desc_pref_conf s2m_conf;
	unsigned int m2s_ostand_max_data_read;
	unsigned int m2s_ostand_max_desc_read;
	unsigned int m2s_ostand_max_comp_req;
	unsigned int s2m_ostand_max_desc_read;
	unsigned int s2m_ostand_max_data_req;
	unsigned int s2m_ostand_max_comp_req;
	uint32_t reg;
	int err;
	unsigned int i;

	err = al_udma_m2s_pref_get(m2s_udma, &m2s_conf);
	if (err) {
		al_err("%s: al_udma_handle_init failed!\n", __func__);
		return;
	}

	reg = al_reg_read32(&m2s_udma->udma_regs->m2s.axi_m2s.ostand_cfg);
	m2s_ostand_max_data_read = AL_REG_FIELD_GET(reg,
		UDMA_AXI_M2S_OSTAND_CFG_MAX_DATA_RD_MASK,
		UDMA_AXI_M2S_OSTAND_CFG_MAX_DATA_RD_SHIFT);
	m2s_ostand_max_desc_read = AL_REG_FIELD_GET(reg,
		UDMA_AXI_M2S_OSTAND_CFG_MAX_DESC_RD_MASK,
		UDMA_AXI_M2S_OSTAND_CFG_MAX_DESC_RD_SHIFT);
	m2s_ostand_max_comp_req = AL_REG_FIELD_GET(reg,
		UDMA_AXI_M2S_OSTAND_CFG_MAX_COMP_REQ_MASK,
		UDMA_AXI_M2S_OSTAND_CFG_MAX_COMP_REQ_SHIFT);

	err = al_udma_s2m_pref_get(s2m_udma, &s2m_conf);
	if (err) {
		al_err("%s: al_udma_handle_init failed!\n", __func__);
		return;
	}

	reg = al_reg_read32(&s2m_udma->udma_regs->s2m.axi_s2m.ostand_cfg_rd);
	s2m_ostand_max_desc_read = AL_REG_FIELD_GET(reg,
		UDMA_AXI_S2M_OSTAND_CFG_RD_MAX_DESC_RD_OSTAND_MASK,
		UDMA_AXI_S2M_OSTAND_CFG_RD_MAX_DESC_RD_OSTAND_SHIFT);

	reg = al_reg_read32(&s2m_udma->udma_regs->s2m.axi_s2m.ostand_cfg_wr);
	s2m_ostand_max_data_req = AL_REG_FIELD_GET(reg,
		UDMA_AXI_S2M_OSTAND_CFG_WR_MAX_DATA_WR_OSTAND_MASK,
		UDMA_AXI_S2M_OSTAND_CFG_WR_MAX_DATA_WR_OSTAND_SHIFT);
	s2m_ostand_max_comp_req = AL_REG_FIELD_GET(reg,
		UDMA_AXI_S2M_OSTAND_CFG_WR_MAX_COMP_REQ_MASK,
		UDMA_AXI_S2M_OSTAND_CFG_WR_MAX_COMP_REQ_SHIFT);

	al_print("- M2S\n");
	al_print("  * outstanding reads:\n");
	al_print("    > ostand_max_data_read = %u\n", m2s_ostand_max_data_read);
	al_print("    > ostand_max_desc_read = %u\n", m2s_ostand_max_desc_read);
	al_print("  * outstanding writes:\n");
	al_print("    > ostand_max_comp_req = %u\n", m2s_ostand_max_comp_req);
	al_print("  * prefetch:\n");
	al_print("    > desc_fifo_depth = %u\n", m2s_conf.desc_fifo_depth);
	al_print("    > max_desc_per_packet = %u\n", m2s_conf.max_desc_per_packet);
	al_print("    > pref_thr = %u\n", m2s_conf.pref_thr);
	al_print("    > min_burst_above_thr = %u\n", m2s_conf.min_burst_above_thr);
	al_print("    > min_burst_below_thr = %u\n", m2s_conf.min_burst_below_thr);
	al_print("    > max_pkt_limit = %u\n", m2s_conf.max_pkt_limit);
	al_print("    > data_fifo_depth = %u\n", m2s_conf.data_fifo_depth);

	al_print("- S2M\n");
	al_print("  * outstanding reads:\n");
	al_print("    > ostand_max_desc_read = %u\n", s2m_ostand_max_desc_read);
	al_print("  * outstanding writes:\n");
	al_print("    > ostand_max_data_req = %u\n", s2m_ostand_max_data_req);
	al_print("    > ostand_max_comp_req = %u\n", s2m_ostand_max_comp_req);
	al_print("  * prefetch:\n");
	al_print("    > desc_fifo_depth = %u\n", s2m_conf.desc_fifo_depth);
	al_print("    > pref_thr = %u\n", s2m_conf.pref_thr);
	al_print("    > min_burst_above_thr = %u\n", s2m_conf.min_burst_above_thr);
	al_print("    > min_burst_below_thr = %u\n", s2m_conf.min_burst_below_thr);

	for (i = 0; i < al_udma_num_queues_get(s2m_udma); i++) {
		struct al_udma_q *s2m_udma_q;
		struct al_udma_s2m_q_comp_conf comp_conf;

		err = al_udma_q_handle_get(s2m_udma, i, &s2m_udma_q);
		if (err) {
			al_err("%s: al_udma_q_handle_get failed!\n", __func__);
			return;
		}

		if (!al_udma_q_is_enabled(s2m_udma_q)) {
			al_print("  * Queue %u: Disabled\n", i);
			continue;
		}

		al_udma_s2m_q_comp_get(s2m_udma_q, &comp_conf);

		al_print("  * Queue %u\n", i);
		al_print("    > dis_comp_coal = %s\n",
			comp_conf.dis_comp_coal ? "enabled" : "disabled");
		al_print("    > en_comp_ring_update = %s\n",
			comp_conf.en_comp_ring_update ? "enabled" : "disabled");
		al_print("    > comp_timer = %u\n", comp_conf.comp_timer);
		al_print("    > q_qos = %u\n", comp_conf.q_qos);
	}
}

/* Num available queues */
unsigned int al_udma_num_queues_get(
	const struct al_udma *udma)
{
	al_assert(udma);

	return udma->num_of_queues;
}

/*
 * Initialize the udma engine
 */
int al_udma_init(struct al_udma *udma, struct al_udma_params *udma_params)
{
	int ret;

	al_assert(udma);
	al_assert(udma_params);

	ret = al_udma_handle_init_aux(udma, udma_params, AL_FALSE);
	if (ret)
		return ret;

	/* initialize configuration registers to correct values */
	al_udma_set_defaults(udma);

	/* unmask error interrupts */
	if (udma->type == UDMA_TX)
		al_udma_iofic_m2s_error_ints_unmask(udma);
	else
		al_udma_iofic_s2m_error_ints_unmask(udma	);

	al_dbg("udma [%s] initialized. base %p\n", udma->name,
		udma->udma_regs);
	return 0;
}

/* Reset (and disable) all UDMA queues */
int al_udma_q_reset_all(struct al_udma *udma)
{
	unsigned int i;
	int err;

	al_assert(udma);

	for (i = 0; i < udma->num_of_queues; i++) {
		struct al_udma_q *q = NULL;

		err = al_udma_q_handle_get(udma, i, &q);
		if (err) {
			al_err("%s: al_udma_q_handle_get failed(%u)!\n", __func__, i);
			return err;
		}

		if (!al_udma_q_is_enabled(q))
			continue;

		err = al_udma_q_reset(q);
		if (err) {
			al_err("%s: al_udma_q_reset failed(%u)!\n", __func__, i);
			return err;
		}
	}

	return 0;
}

/*
 * Initialize the udma queue data structure
 */
int al_udma_q_init(struct al_udma *udma, uint32_t qid,
					struct al_udma_q_params *q_params)
{
	struct al_udma_q *udma_q;

	al_assert(udma);
	al_assert(q_params);

	if (qid >= udma->num_of_queues) {
		al_err("udma: invalid queue id (%d)\n", qid);
		return -EINVAL;
	}

	if (udma->udma_q[qid].status == AL_QUEUE_ENABLED) {
		al_err("udma: queue (%d) already enabled!\n", qid);
		return -EIO;
	}

	if (q_params->size < AL_UDMA_MIN_Q_SIZE) {
		al_err("udma: queue (%d) size (%d) too small\n",
			qid, q_params->size);
		return -EINVAL;
	}

	if (q_params->size > AL_UDMA_MAX_Q_SIZE) {
		al_err("udma: queue (%d) size (%d) too large\n",
			qid, q_params->size);
		return -EINVAL;
	}

	if (q_params->size & (q_params->size - 1)) {
		al_err("udma: queue (%d) size (%d) must be power of 2\n",
			 qid, q_params->size);
		return -EINVAL;
	}

	udma_q = &udma->udma_q[qid];
	udma_q->adapter_rev_id = q_params->adapter_rev_id;
	udma_q->size = q_params->size;
	udma_q->size_mask = q_params->size - 1;
	udma_q->desc_base_ptr = q_params->desc_base;
	udma_q->desc_phy_base = q_params->desc_phy_base;
	udma_q->cdesc_base_ptr = q_params->cdesc_base;
	udma_q->cdesc_phy_base = q_params->cdesc_phy_base;
	udma_q->cdesc_size = q_params->cdesc_size;

	udma_q->next_desc_idx = 0;
	udma_q->next_cdesc_idx = 0;
	udma_q->end_cdesc_ptr = (uint8_t *) udma_q->cdesc_base_ptr +
	    (udma_q->size - 1) * udma_q->cdesc_size;
	udma_q->comp_head_idx = 0;
	udma_q->comp_head_ptr = (union al_udma_cdesc *)udma_q->cdesc_base_ptr;
	udma_q->desc_ring_id = AL_UDMA_INITIAL_RING_ID;
	udma_q->comp_ring_id = AL_UDMA_INITIAL_RING_ID;
#if 0
	udma_q->desc_ctrl_bits = AL_UDMA_INITIAL_RING_ID <<
						AL_M2S_DESC_RING_ID_SHIFT;
#endif
	udma_q->pkt_crnt_descs = 0;
	udma_q->flags = 0;
	udma_q->status = AL_QUEUE_DISABLED;
	udma_q->udma = udma;
	udma_q->qid = qid;

	/* start hardware configuration: */
	al_udma_q_config(udma_q);
	/* reset the queue pointers */
	al_udma_q_set_pointers(udma_q);

	/* enable the q */
	al_udma_q_enable(udma_q, 1);

	al_dbg("udma [%s %d]: %s q init. size 0x%x\n"
			"  desc ring info: phys base 0x%" PRIx64 " virt base %p\n",
			udma_q->udma->name, udma_q->qid,
			udma->type == UDMA_TX ? "Tx" : "Rx",
			q_params->size,
			q_params->desc_phy_base,
			q_params->desc_base);
	al_dbg("  cdesc ring info: phys base 0x%" PRIx64 " virt base %p entry size 0x%x\n",
			q_params->cdesc_phy_base,
			q_params->cdesc_base,
			q_params->cdesc_size);

	return 0;
}

al_bool al_udma_q_is_enabled(struct al_udma_q *udma_q)
{
	uint32_t reg;

	al_assert(udma_q);

	reg = al_reg_read32(&udma_q->q_regs->rings.cfg);
	if (reg & (UDMA_M2S_Q_CFG_EN_PREF | UDMA_M2S_Q_CFG_EN_SCHEDULING))
		return AL_TRUE;

	return AL_FALSE;
}

/*
 * pause a udma queue
 */
int al_udma_q_pause(struct al_udma_q *udma_q)
{
	unsigned int remaining_time = AL_UDMA_Q_QUIESCE_TOUT;
	uint32_t *status_reg;
	uint32_t *dcp_reg;
	uint32_t *crhp_reg;

	al_assert(udma_q);

	/* De-assert scheduling and prefetch */
	al_udma_q_enable(udma_q, 0);

	/* Wait for scheduling and prefetch to stop */
	status_reg = &udma_q->q_regs->rings.status;

	while (remaining_time) {
		uint32_t status = al_reg_read32(status_reg);

		if (!(status & (UDMA_M2S_Q_STATUS_PREFETCH |
						UDMA_M2S_Q_STATUS_SCHEDULER)))
			break;

		remaining_time--;
		al_udelay(1);
	}

	if (!remaining_time) {
		al_err("udma [%s %d]: %s timeout waiting for prefetch and "
			"scheduler disable\n", udma_q->udma->name, udma_q->qid,
			__func__);
		return -ETIME;
	}

	/* Wait for the completion queue to reach to the same pointer as the
	 * prefetch stopped at ([TR]DCP == [TR]CRHP) */
	dcp_reg = &udma_q->q_regs->rings.dcp;
	crhp_reg = &udma_q->q_regs->rings.crhp;

	while (remaining_time) {
		uint32_t dcp = al_reg_read32(dcp_reg);
		uint32_t crhp = al_reg_read32(crhp_reg);

		if (dcp == crhp)
			break;

		remaining_time--;
		al_udelay(1);
	};

	if (!remaining_time) {
		al_err("udma [%s %d]: %s timeout waiting for dcp==crhp\n",
			udma_q->udma->name, udma_q->qid, __func__);
		return -ETIME;
	}

	return 0;
}

int al_udma_q_resume(struct al_udma_q *udma_q)
{
	al_assert(udma_q);

	al_udma_q_enable(udma_q, 1);

	return 0;
}

/*
 * Reset a udma queue
 */
int al_udma_q_reset(struct al_udma_q *udma_q)
{
	uint32_t *q_sw_ctrl_reg;
	int rc;

	al_assert(udma_q);

	rc = al_udma_q_pause(udma_q);

	if (rc)
		return rc;

	/* Assert the queue reset */
	if (udma_q->udma->type == UDMA_TX)
		q_sw_ctrl_reg = &udma_q->q_regs->m2s_q.q_sw_ctrl;
	else
		q_sw_ctrl_reg = &udma_q->q_regs->s2m_q.q_sw_ctrl;

	al_reg_write32(q_sw_ctrl_reg, UDMA_M2S_Q_SW_CTRL_RST_Q);

	return 0;
}

/*
 * return (by reference) a pointer to a specific queue date structure.
 */
int al_udma_q_handle_get(struct al_udma *udma, uint32_t qid,
						struct al_udma_q **q_handle)
{

	al_assert(udma);
	al_assert(q_handle);

	if (unlikely(qid >= udma->num_of_queues)) {
		al_err("udma [%s]: invalid queue id (%d)\n", udma->name, qid);
		return -EINVAL;
	}
	*q_handle = &udma->udma_q[qid];
	return 0;
}

/*
 * Change the UDMA's state
 */
int al_udma_state_set(struct al_udma *udma, enum al_udma_state state)
{
	uint32_t reg;

	al_assert(udma != NULL);
	if (state == udma->state)
		al_dbg("udma [%s]: requested state identical to "
			"current state (%d)\n", udma->name, state);

	al_dbg("udma [%s]: change state from (%s) to (%s)\n",
		 udma->name, al_udma_states_name[udma->state],
		 al_udma_states_name[state]);

	reg = 0;
	switch (state) {
	case UDMA_DISABLE:
		reg |= UDMA_M2S_CHANGE_STATE_DIS;
		break;
	case UDMA_NORMAL:
		reg |= UDMA_M2S_CHANGE_STATE_NORMAL;
		break;
	case UDMA_ABORT:
		reg |= UDMA_M2S_CHANGE_STATE_ABORT;
		break;
	default:
		al_err("udma: invalid state (%d)\n", state);
		return -EINVAL;
	}

	if (udma->type == UDMA_TX)
		al_reg_write32(&udma->udma_regs->m2s.m2s.change_state, reg);
	else
		al_reg_write32(&udma->udma_regs->s2m.s2m.change_state, reg);

	udma->state = state;
	return 0;
}

static void al_udma_s2m_fifos_flush(struct al_udma *udma)
{
	al_assert(udma);
	al_assert(udma->type == UDMA_RX);

	/** FIFO reset - toggle enable bit*/
	al_reg_write32(&udma->udma_regs->s2m.s2m.fifo_en, 0);
	/** restart udma control state machines */
	al_reg_write32(&udma->udma_regs->s2m.s2m.clear_ctrl, 0xFFFFFFFF);
	al_udelay(1);

	/** Re enable fifo's */
	al_reg_write32(&udma->udma_regs->s2m.s2m.fifo_en, 0xFFFFFFFF);
	/** restart udma state machines */
	al_reg_write32(&udma->udma_regs->s2m.s2m.clear_ctrl, 0xFFFFFFFF);
	al_udelay(1);
}

#define AL_UDMA_S2M_STREAM_FLUSH \
				(UDMA_S2M_STREAM_CFG_DISABLE	| \
				UDMA_S2M_STREAM_CFG_FLUSH	| \
				UDMA_S2M_STREAM_CFG_STOP_PREFETCH)

static void udma_s2m_stream_set_mode(struct al_udma *udma, al_bool state)
{
	uint32_t stream_dis = AL_UDMA_S2M_STREAM_FLUSH;

	al_reg_write32_masked(&udma->udma_regs->s2m.s2m.stream_cfg,
		stream_dis,
		state ? 0 : stream_dis);
}

static void udma_s2m_stream_queues_set_mode(struct al_udma *udma, al_bool state)
{
	int i = 0;
	int rc;

	for (i = 0; i < udma->num_of_queues; i++) {
		struct al_udma_q *dma_q = NULL;
		uint32_t *reg_addr;
		rc = al_udma_q_handle_get(udma, i, &dma_q);
		if (rc != 0) {
			al_assert_msg(!rc, "%s : Failed at al_udma_q_handle_get (rc = %d)\n",
				__func__, rc);
			return;
		}

		reg_addr = &dma_q->q_regs->s2m_q.cfg;
		al_reg_write32_masked(reg_addr, UDMA_S2M_Q_CFG_EN_STREAM,
						state ? UDMA_S2M_Q_CFG_EN_STREAM : 0);
	}
}

static void al_udma_s2m_stream_configure(struct al_udma *udma, al_bool state)
{
	al_assert(udma);
	al_assert(udma->type == UDMA_RX);

	if (state == AL_TRUE) {
		udma_s2m_stream_queues_set_mode(udma, state);
		udma_s2m_stream_set_mode(udma, state);
	} else {
		udma_s2m_stream_set_mode(udma, state);
		udma_s2m_stream_queues_set_mode(udma, state);
	}
}

/**
 * Determine if stream is enabled or disbled (flushing new packets)
 */
static al_bool al_udma_s2m_stream_status_get(struct al_udma *udma)
{
	unsigned int i;
	uint32_t stream_cfg = 0;
	int rc = 0;
	al_bool queue_stream_status;
	al_bool queue_stream_status_valid = AL_FALSE;
	al_bool stream_status = AL_TRUE;

	al_assert(udma);
	al_assert(udma->type == UDMA_RX);

	stream_cfg = al_reg_read32(&udma->udma_regs->s2m.s2m.stream_cfg);
	stream_cfg &= AL_UDMA_S2M_STREAM_FLUSH;

	if (stream_cfg == AL_UDMA_S2M_STREAM_FLUSH)
		stream_status = AL_FALSE; /** stream is disabled */

	queue_stream_status = AL_FALSE;
	for (i = 0; i < udma->num_of_queues; i++) {
		struct al_udma_q *dma_q = NULL;
		uint32_t *reg_addr;
		uint32_t reg_val;

		rc = al_udma_q_handle_get(udma, i, &dma_q);
		if (rc != 0)
			al_err("%s : Failed at al_udma_q_handle_get (rc = %d)\n", __func__, rc);

		if (!al_udma_q_is_enabled(dma_q))
			continue;

		queue_stream_status_valid = AL_TRUE;

		reg_addr = &dma_q->q_regs->s2m_q.cfg;
		reg_val = al_reg_read32(reg_addr);

		/** Need all queues stream interface disabled */
		if (reg_val & UDMA_S2M_Q_CFG_EN_STREAM) {
			queue_stream_status = AL_TRUE;
			break;
		}
	}

	if (queue_stream_status_valid && (queue_stream_status != stream_status)) {
		al_warn("%s: Bad configurations, stream & queue stream interface status are"
			" different! assuming strem is enabled\n",
			__func__);
		/** Need both interfaces disabled to stop stream */
		return AL_TRUE;
	}

	return stream_status;
}

/*
 * return the current UDMA hardware state
 */
enum al_udma_state al_udma_state_get(struct al_udma *udma)
{
	uint32_t state_reg;
	uint32_t comp_ctrl;
	uint32_t stream_if;
	uint32_t data_rd;
	uint32_t desc_pref;
	al_bool stream_enabled;

	if (udma->type == UDMA_TX) {
		state_reg = al_reg_read32(&udma->udma_regs->m2s.m2s.state);
		stream_enabled = AL_TRUE;
	} else {
		state_reg = al_reg_read32(&udma->udma_regs->s2m.s2m.state);
		stream_enabled = al_udma_s2m_stream_status_get(udma);
	}

	comp_ctrl = AL_REG_FIELD_GET(state_reg,
				     UDMA_M2S_STATE_COMP_CTRL_MASK,
				     UDMA_M2S_STATE_COMP_CTRL_SHIFT);
	stream_if = AL_REG_FIELD_GET(state_reg,
				     UDMA_M2S_STATE_STREAM_IF_MASK,
				     UDMA_M2S_STATE_STREAM_IF_SHIFT);
	data_rd = AL_REG_FIELD_GET(state_reg,
				   UDMA_M2S_STATE_DATA_RD_CTRL_MASK,
				   UDMA_M2S_STATE_DATA_RD_CTRL_SHIFT);
	desc_pref = AL_REG_FIELD_GET(state_reg,
				     UDMA_M2S_STATE_DESC_PREF_MASK,
				     UDMA_M2S_STATE_DESC_PREF_SHIFT);

	al_assert(comp_ctrl != UDMA_STATE_RESERVED);
	al_assert(stream_if != UDMA_STATE_RESERVED);
	al_assert(data_rd != UDMA_STATE_RESERVED);
	al_assert(desc_pref != UDMA_STATE_RESERVED);

	/**
	 * Due to a HW bug, in case stream is disabled but there are packets waiting to enter
	 * the UDMA, the stream_if might be "stuck" at "1"
	 *
	 * We can ignore the stream_if indication if :
	 * 1. The stream is disabled for UDMA and for each queue -
	 *    new packets cannot enter the UDMA or its queues.
	 * 3. We waited at least 1us for the FIFO's to be emptied
	 */

	/* if any of the states is abort then return abort */
	if (stream_enabled) {
		if ((comp_ctrl == UDMA_STATE_ABORT) || (stream_if == UDMA_STATE_ABORT)
			|| (data_rd == UDMA_STATE_ABORT) || (desc_pref == UDMA_STATE_ABORT))
				return UDMA_ABORT;
	} else {
		if ((comp_ctrl == UDMA_STATE_ABORT) || (desc_pref == UDMA_STATE_ABORT) ||
			(data_rd == UDMA_STATE_ABORT))
				return UDMA_ABORT;
	}

	if (stream_enabled) {
		/* if any of the states is normal then return normal */
		if ((comp_ctrl == UDMA_STATE_NORMAL) || (stream_if == UDMA_STATE_NORMAL)
			|| (data_rd == UDMA_STATE_NORMAL) || (desc_pref == UDMA_STATE_NORMAL))
				return UDMA_NORMAL;
	} else {
		if ((comp_ctrl == UDMA_STATE_NORMAL) || (desc_pref == UDMA_STATE_NORMAL) ||
			(data_rd == UDMA_STATE_NORMAL))
				return UDMA_NORMAL;
	}

	return UDMA_IDLE;
}

int al_udma_state_set_wait(struct al_udma *dma, enum al_udma_state new_state, al_bool flush_stream)
{
	enum al_udma_state state;
	enum al_udma_state expected_state = new_state;
	int count = 1000;
	int rc;

	/**
	 * When we wish to move to DISABLE or NORMAL state & the UDMA can receive traffic,
	 * we might never change state & the UDMA will be busy processing new requests.
	 * Therefore we perform the following operations :
	 * 1. disable the stream of the UDMA & stream enable of each queue
	 * 2. wait untill all FIFO's are emptied - 1us is enough.
	 * 3. Issue FIFO's flush for "stuck" packets - This can occur if there are "garbage" packets
	 *    For example : if the EC hasn't been initialized but transferred traffic
	 */
	if (flush_stream) {
		al_assert(dma->type == UDMA_RX);
		al_assert((new_state == UDMA_DISABLE) || (new_state == UDMA_NORMAL));

		al_udma_s2m_stream_configure(dma, AL_FALSE);	/** stop stream */
		al_udelay(1);					/** wait until empty */
		al_udma_s2m_fifos_flush(dma);			/** flush remaining packets */
	}

	rc = al_udma_state_set(dma, new_state);
	if (rc != 0) {
		al_warn("[%s] warn: failed to change state, error %d\n", dma->name, rc);
		return rc;
	}

	if ((new_state == UDMA_NORMAL) || (new_state == UDMA_DISABLE))
		expected_state = UDMA_IDLE;

	do {
		state = al_udma_state_get(dma);
		if (state == expected_state)
			break;
		al_udelay(1);
		if (count-- == 0) {
			al_warn("[%s] warn: dma state didn't change to %s\n",
				 dma->name, al_udma_states_name[new_state]);
			return -ETIMEDOUT;
		}
	} while (1);

	if (flush_stream) {
		if ((new_state == UDMA_NORMAL) && (dma->rev_id >= AL_UDMA_REV_ID_4)) {
			al_udma_ttt_default_config(dma); /** Configure TTT table */
#ifdef AL_ETH_EX
			al_udma_ex_ttt_default_config(dma);
#endif
		}
		al_udma_s2m_stream_configure(dma, AL_TRUE); /** Enable the stream */
	}

	return 0;
}

/*
 * Action handling
 */

/*
 * get next completed packet from completion ring of the queue
 *
 * This implementation assumes that udma drop mode is set - if it is removed we should remove
 * the marked code segment as its not needed.
 */
uint32_t al_udma_cdesc_packet_get(
	struct al_udma_q		*udma_q,
	volatile union al_udma_cdesc	**cdesc)
{
	uint32_t count;
	volatile union al_udma_cdesc *curr;
	uint32_t comp_flags;

	/* this function requires the completion ring update */
	al_assert(!(udma_q->flags & AL_UDMA_Q_FLAGS_NO_COMP_UPDATE));

	/* comp_head points to the last comp desc that was processed */
	curr = udma_q->comp_head_ptr;
	comp_flags = swap32_from_le(curr->al_desc_comp_tx.ctrl_meta);

	/* check if the completion descriptor is new */
	if (unlikely(al_udma_new_cdesc(udma_q, comp_flags) == AL_FALSE))
		return 0;

	/**
	 * If continue to process a packet, check it was not "trimmed" by udma drop
	 * i.e, making sure processed desc is not a "first" desc
	 */
	if (unlikely(udma_q->pkt_crnt_descs && cdesc_is_first(comp_flags))) {
		uint32_t last_desc =
			(udma_q->next_cdesc_idx + udma_q->pkt_crnt_descs - 1) & (udma_q->size_mask);
		curr = al_udma_cdesc_idx_to_ptr(udma_q, last_desc);
		curr->al_desc_comp_rx.ctrl_meta |=
			swap32_to_le(AL_S2M_DESC_ERR | AL_UDMA_CDESC_LAST);
		count = udma_q->pkt_crnt_descs;
		goto done;
	}

	/* if new desc found, increment the current packets descriptors */
	count = udma_q->pkt_crnt_descs + 1;

	/**
	 * Stopping condition is end of packet - indication of last descriptor
	 */
	while (!cdesc_is_last(comp_flags)) {
		curr = al_cdesc_next_update(udma_q, curr);
		comp_flags = swap32_from_le(curr->al_desc_comp_tx.ctrl_meta);
		if (unlikely(al_udma_new_cdesc(udma_q, comp_flags)
								== AL_FALSE)) {
			/* the current packet here doesn't have all  */
			/* descriptors completed. log the current desc */
			/* location and number of completed descriptors so */
			/*  far. then return */
			udma_q->pkt_crnt_descs = count;
			udma_q->comp_head_ptr = curr;
			return 0;
		}

		/**
		 * If udma drop mode is on (call to al_udma_s2m_no_desc_cfg_set())
		 * "middle/last" descs can be dropped.
		 * A possible error scenarios is :
		 * F, M, M, F, L : In that case we will pass on 2 packets as one.
		 * F, F , F, FL     : Pass 4 packets as one.
		 *
		 * Therefore we should make sure we are not processing a new "F/FL" desc.
		 */
		if (unlikely(cdesc_is_first(comp_flags))) {
			/*
			 * Return to actual last desc and mark it as last with L2 err, then stop
			 * F, M, M, F => last desc is at : first + 2 => first + (count - 1)
			 * F, F       => last desc is at : first + 0
			 * count >= 1
			 */
			uint32_t last_desc =
				(udma_q->next_cdesc_idx + count - 1) & (udma_q->size_mask);
			curr = al_udma_cdesc_idx_to_ptr(udma_q, last_desc);
			curr->al_desc_comp_rx.ctrl_meta |= swap32_to_le(
				AL_S2M_DESC_ERR | AL_UDMA_CDESC_LAST);
			break;
		}

		count++;
		/* check against max descs per packet. */
		al_assert(count <= udma_q->size);
	}

done:
	/* return back the first descriptor of the packet */
	*cdesc = al_udma_cdesc_idx_to_ptr(udma_q, udma_q->next_cdesc_idx);
	udma_q->pkt_crnt_descs = 0;
	udma_q->comp_head_ptr = al_cdesc_next_update(udma_q, curr);

	al_dbg("udma [%s %d]: packet completed. first desc %p (ixd 0x%x)"
		 " descs %d\n", udma_q->udma->name, udma_q->qid, *cdesc,
		 udma_q->next_cdesc_idx, count);

	return count;
}

void al_udma_mailbox_read(struct al_udma *udma, unsigned int mailbox_id, uint32_t *val)
{
	uint32_t *reg;

	if (udma->rev_id < AL_UDMA_REV_ID_4) {
		struct udma_gen_regs_v3 *udma_gen_regs = udma->gen_regs;

		al_assert(mailbox_id < AL_ARR_SIZE(udma_gen_regs->mailbox));

		reg = &udma_gen_regs->mailbox[mailbox_id].msg_in;
	} else {
		struct udma_gen_regs_v4 *udma_gen_regs = udma->gen_regs;

		al_assert(mailbox_id < AL_ARR_SIZE(udma_gen_regs->mailbox));

		reg = &udma_gen_regs->mailbox[mailbox_id].msg_in;
	}

	*val = al_reg_read32(reg);
}

void al_udma_mailbox_write(struct al_udma *udma, unsigned int mailbox_id, uint32_t val)
{
	uint32_t *reg;

	if (udma->rev_id < AL_UDMA_REV_ID_4) {
		struct udma_gen_regs_v3 *udma_gen_regs = udma->gen_regs;

		al_assert(mailbox_id < AL_ARR_SIZE(udma_gen_regs->mailbox));

		reg = &udma_gen_regs->mailbox[mailbox_id].msg_out;
	} else {
		struct udma_gen_regs_v4 *udma_gen_regs = udma->gen_regs;

		al_assert(mailbox_id < AL_ARR_SIZE(udma_gen_regs->mailbox));

		reg = &udma_gen_regs->mailbox[mailbox_id].msg_out;
	}

	al_reg_write32(reg, val);
}


/** @} end of UDMA group */
