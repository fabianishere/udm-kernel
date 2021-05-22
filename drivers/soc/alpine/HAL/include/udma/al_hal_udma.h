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
 * @defgroup group_udma_api API
 * @ingroup group_udma
 * UDMA API
 * @{
 * @}
 *
 * @defgroup group_udma_main UDMA Main
 * @ingroup group_udma_api
 * UDMA main API
 * @{
 * @file   al_hal_udma.h
 *
 * @brief C Header file for the Universal DMA HAL driver
 *
 */

#ifndef __AL_HAL_UDMA_H__
#define __AL_HAL_UDMA_H__

#include "al_hal_common.h"
#include "al_hal_udma_regs.h"

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

#define AL_UDMA_API_VER		5

#define DMA_MAX_Q_V3		4
#define DMA_MAX_Q_V4		16
#define DMA_MAX_Q_MAX		DMA_MAX_Q_V4

/* Use maximal possible number of queues */
#define AL_UDMA_NUM_QUEUES_MAX	0xff

/* Maximal number of completion descriptors per cache line */
#define AL_UDMA_MAX_NUM_CDESC_PER_CACHE_LINE	16

#define AL_UDMA_MIN_Q_SIZE	32
#define AL_UDMA_MAX_Q_SIZE 	(1 << 16) /* hw can do more, but we limit it */

/* Default Max number of descriptors supported per action */
#define AL_UDMA_DEFAULT_MAX_ACTN_DESCS	16
#define AL_UDMA_DEFAULT_MAX_ACTN_DESCS_V1	16
#define AL_UDMA_DEFAULT_MAX_ACTN_DESCS_V2	16
#define AL_UDMA_DEFAULT_MAX_ACTN_DESCS_V4	60
#define AL_UDMA_DEFAULT_MAX_ACTN_DESCS_MAX	AL_UDMA_DEFAULT_MAX_ACTN_DESCS_V4

#define AL_UDMA_REV_ID_1	1
#define AL_UDMA_REV_ID_2	2
#define AL_UDMA_REV_ID_4	4

#define DMA_RING_ID_MASK	0x3
/* New registers ?? */
/* Statistics - TBD */

/** UDMA submission descriptor */
union al_udma_desc {
	/* TX */
	struct {
		uint32_t len_ctrl;
		uint32_t meta_ctrl;
		uint64_t buf_ptr;
	} tx;
	/* TX Meta, used by upper layer */
	struct {
		uint32_t len_ctrl;
		uint32_t meta_ctrl;
		uint32_t meta1;
		uint32_t meta2;
	} tx_meta;
	/* RX */
	struct {
		uint32_t len_ctrl;
		uint32_t buf2_ptr_lo;
		uint64_t buf1_ptr;
	} rx;
	/* Used for fast initialization */
	struct {
		uint64_t qword0;
		uint64_t qword1;
	} raw;
} __packed_a16;

/* TX desc length and control fields */

#define AL_M2S_DESC_CONCAT			AL_BIT(31)	/* concatenate */
#define AL_M2S_DESC_DMB				AL_BIT(30)
						/** Data Memory Barrier */
#define AL_M2S_DESC_NO_SNOOP_H			AL_BIT(29)
#define AL_M2S_DESC_INT_EN			AL_BIT(28)	/** enable interrupt */
#define AL_M2S_DESC_LAST			AL_BIT(27)
#define AL_M2S_DESC_FIRST			AL_BIT(26)
#define AL_M2S_DESC_RING_ID_SHIFT		24
#define AL_M2S_DESC_RING_ID_MASK		(0x3 << AL_M2S_DESC_RING_ID_SHIFT)
#define AL_M2S_DESC_RING_SHIFT			UDMA_M2S_Q_TDRBP_LOW_ADDR_SHIFT
#define AL_M2S_DESC_META_DATA			AL_BIT(23)
#define AL_M2S_DESC_DUMMY			AL_BIT(22) /* for Metdata only */
#define AL_M2S_DESC_LEN_SHIFT			0
#define AL_M2S_DESC_LEN_MASK			(0xffff << AL_M2S_DESC_LEN_SHIFT)

#define AL_S2M_DESC_DUAL_BUF			AL_BIT(31)
#define AL_S2M_DESC_NO_SNOOP_H			AL_BIT(29)
#define AL_S2M_DESC_INT_EN			AL_BIT(28)	/** enable interrupt */
#define AL_S2M_DESC_RING_ID_SHIFT		24
#define AL_S2M_DESC_RING_ID_MASK		(0x3 << AL_S2M_DESC_RING_ID_SHIFT)
#define AL_S2M_DESC_RING_SHIFT			UDMA_S2M_Q_RDRBP_LOW_ADDR_SHIFT
#define AL_S2M_DESC_LEN_SHIFT			0
#define AL_S2M_DESC_LEN_MASK			(0xffff << AL_S2M_DESC_LEN_SHIFT)
#define AL_S2M_DESC_LEN2_SHIFT			16
#define AL_S2M_DESC_LEN2_MASK			(0x3fff << AL_S2M_DESC_LEN2_SHIFT)
#define AL_S2M_DESC_LEN2_GRANULARITY_SHIFT	6
#define AL_S2M_DESC_ERR				AL_BIT(16)

/* TX/RX descriptor Target-ID field (in the buffer address 64 bit field) */
#define AL_UDMA_DESC_TGTID_SHIFT		48

/** UDMA completion descriptor */
union al_udma_cdesc {
	/* TX completion */
	struct {
		uint32_t ctrl_meta;
	} al_desc_comp_tx;
	/* RX completion */
	struct {
		/* TBD */
		uint32_t ctrl_meta;
	} al_desc_comp_rx;
} __packed_a4;

/* TX/RX common completion desc ctrl_meta feilds */
#define AL_UDMA_CDESC_ERROR		AL_BIT(31)
#define AL_UDMA_CDESC_BUF1_USED		AL_BIT(30)
#define AL_UDMA_CDESC_LAST		AL_BIT(27)
#define AL_UDMA_CDESC_FIRST		AL_BIT(26)
/* word 2 */
#define AL_UDMA_CDESC_BUF2_USED			AL_BIT(31)
#define AL_UDMA_CDESC_BUF2_LEN_SHIFT		16
#define AL_UDMA_CDESC_BUF2_LEN_MASK		AL_FIELD_MASK(29, 16)
#define AL_UDMA_CDESC_RING_SHIFT		UDMA_S2M_Q_RCRBP_LOW_ADDR_SHIFT

/** Basic Buffer structure */
struct al_buf {
	al_phys_addr_t addr; /**< Buffer physical address */
	uint32_t len; /**< Buffer lenght in bytes */
};

/** Block is a set of buffers that belong to same source or destination */
struct al_block {
	struct al_buf *bufs; /**< The buffers of the block */
	uint32_t num; /**< Number of buffers of the block */

	/**<
	 * Target-ID to be assigned to the block descriptors
	 * Requires Target-ID in descriptor to be enabled for the specific UDMA
	 * queue.
	 */
	uint16_t tgtid;
};

/** UDMA type */
enum al_udma_type {
	UDMA_TX,
	UDMA_RX,
	UDMA_TYPE_MAX,
};

/** UDMA state */
enum al_udma_state {
	UDMA_DISABLE = 0,
	UDMA_IDLE,
	UDMA_NORMAL,
	UDMA_ABORT,
	UDMA_RESET,
	UDMA_STATE_MAX,
};

extern const char *const al_udma_states_name[];

/** UDMA Q specific parameters from upper layer */
struct al_udma_q_params {
	uint32_t size;		/**< ring size (in descriptors), submission and
				 * completion rings must have same size
				 */
	union al_udma_desc *desc_base; /**< cpu address for submission ring
					 * descriptors
					 */
	al_phys_addr_t desc_phy_base;	/**< submission ring descriptors
					 * physical base address
					 */
	uint8_t *cdesc_base;	/**< completion descriptors pointer, NULL */
				/* means no completion update */
	al_phys_addr_t cdesc_phy_base;	/**< completion descriptors ring
					 * physical base address
					 */
	uint32_t cdesc_size;	/**< size (in bytes) of a single dma completion
					* descriptor
					*/

	uint8_t adapter_rev_id; /**<PCI adapter revision ID */
};

/** UDMA parameters from upper layer */
struct al_udma_params {
	void __iomem *udma_regs_base;
	enum al_udma_type type;	/**< Tx or Rx */
	/**
	 * number of queues used by the UDMA
	 * Use 'AL_UDMA_NUM_QUEUES_MAX' to use all available queues
	 */
	uint8_t num_of_queues;
	const char *name; /**< the upper layer must keep the string area */
};

/* Fordward declaration */
struct al_udma;

/** SW status of a queue */
enum al_udma_queue_status {
	AL_QUEUE_DISABLED = 1,
	AL_QUEUE_ENABLED,
	AL_QUEUE_ABORTED,
	AL_QUEUE_STATUS_MAX,
};

/** UDMA Queue private data structure */
struct __cache_aligned al_udma_q {
	uint16_t size_mask;		/**< mask used for pointers wrap around
					 * equals to size - 1
					 */
	union udma_q_regs __iomem *q_regs; /**< pointer to the per queue UDMA
					   * registers
					   */
	union al_udma_desc *desc_base_ptr; /**< base address submission ring
						* descriptors
						*/
	uint16_t next_desc_idx; /**< index to the next available submission
				      * descriptor
				      */

	uint32_t desc_ring_id;	/**< current submission ring id */

	uint8_t *cdesc_base_ptr;/**< completion descriptors pointer, NULL */
				/* means no completion */
	uint32_t cdesc_size;	/**< size (in bytes) of the udma completion ring
				 * descriptor
				 */
	uint16_t next_cdesc_idx; /**< index in descriptors for next completing
			      * ring descriptor
			      */
	uint8_t *end_cdesc_ptr;	/**< used for wrap around detection */
	uint16_t comp_head_idx; /**< completion ring head pointer register
				 *shadow
				 */
	volatile union al_udma_cdesc *comp_head_ptr; /**< when working in get_packet mode
				       * we maintain pointer instead of the
				       * above idx
				       */

	uint32_t pkt_crnt_descs; /**< holds the number of processed descriptors
				  * of the current packet
				  */
	uint32_t comp_ring_id;	/**< current completion Ring Id */


	al_phys_addr_t desc_phy_base; /**< submission desc. physical base */
	al_phys_addr_t cdesc_phy_base; /**< completion desc. physical base */

	uint32_t flags; /**< flags used for completion modes */
	uint32_t size;		/**< ring size in descriptors  */
	enum al_udma_queue_status status;
	struct al_udma *udma;	/**< pointer to parent UDMA */
	uint32_t qid;		/**< the index number of the queue */

	/*
	 * The following fields are duplicated from the UDMA parent adapter
	 * due to performance considerations.
	 */
	uint8_t adapter_rev_id; /**<PCI adapter revision ID */
};

/* UDMA */
struct al_udma {
	const char *name;
	enum al_udma_type type;	/* Tx or Rx */
	enum al_udma_state state;
	uint8_t num_of_queues_max; /* max number of queues supported by the UDMA */
	uint8_t num_of_queues; /* number of queues used by the UDMA */
	void __iomem *unit_regs_base; /** udma unit (RX & TX) regs base */
	union udma_regs __iomem *udma_regs; /* pointer to the UDMA registers */
	void *gen_regs;		/* pointer to the Gen registers*/
	struct udma_gen_axi *gen_axi_regs;	/* pointer to the gen axi regs */
	struct udma_iofic_regs *gen_int_regs;	/* pointer to the gen iofic regs */
	void *gen_ex_regs;		/* pointer to the Gen ex registers*/
	struct al_udma_q udma_q[DMA_MAX_Q_MAX];	/* Array of UDMA Qs pointers */
	unsigned int rev_id; /* UDMA revision ID */
};


/*
 * Configurations
 */

/* UDMA get revision */
/**
 * Get the UDMA revision
 *
 * @param regs_base pointer to the UDMA registers
 *
 * @return revision id
 */
unsigned int al_udma_revision_get(void __iomem *regs_base);

/* Initializations functions */
/**
 * Initialize the udma engine handle
 *
 * @param udma udma data structure
 * @param udma_params udma parameters from upper layer
 *
 * @return 0 on success. -EINVAL otherwise.
 */
int al_udma_handle_init(struct al_udma *udma, struct al_udma_params *udma_params);

/**
 * Get UDMA revision ID
 *
 * @param udma udma data structure
 *
 * @return udma rev ID
 */
unsigned int al_udma_rev_id_get(struct al_udma *udma);

/**
 * UDMA performance parameters printout
 *
 * @param	m2s_udma
 *		M2S UDMA initialized handle
 * @param	s2m_udma
 *		S2M UDMA initialized handle
 */
void al_udma_perf_params_print(
	struct al_udma		*m2s_udma,
	struct al_udma		*s2m_udma);

/**
 * Get number of available UDMA queues
 *
 * @param	udma
 *		Initialize UDMA handle
 *
 * @returns	Number of available queues
 */
unsigned int al_udma_num_queues_get(
	const struct al_udma *udma);

/**
 * Initialize the udma engine handle and defaults
 *
 * @param udma udma data structure
 * @param udma_params udma parameters from upper layer
 *
 * @return 0 on success. -EINVAL otherwise.
 */
int al_udma_init(struct al_udma *udma, struct al_udma_params *udma_params);

/**
 * Reset (and disable) all UDMA queues
 *
 * @param	udma
 *		Initialized UDMA handle
 *
 * @returns	0 upon success
 */
int al_udma_q_reset_all(struct al_udma *udma);

/**
 * Initialize the udma queue data structure
 *
 * @param udma
 * @param qid
 * @param q_params
 *
 * @return 0 if no error found.
 *	   -EINVAL if the qid is out of range
 *	   -EIO if queue was already initialized
 */

int al_udma_q_init(struct al_udma *udma, uint32_t qid,
		   struct al_udma_q_params *q_params);

/**
 * pause a udma queue
 *
 * The queue can be resumes again using 'al_udma_q_resume'
 *
 * @param udma_q
 *
 * @return 0 if no error found.
 */

int al_udma_q_pause(struct al_udma_q *udma_q);

/**
 * resume a udma queue
 *
 * resume a udma queue that was previously paused
 *
 * @param udma_q
 *
 * @return 0 if no error found.
 */

int al_udma_q_resume(struct al_udma_q *udma_q);

/**
 * Reset a udma queue
 *
 * Prior to calling this function make sure:
 * 1. Queue interrupts are masked
 * 2. No additional descriptors are written to the descriptor ring of the queue
 * 3. No completed descriptors are being fetched
 *
 * The queue can be initialized again using 'al_udma_q_init'
 *
 * @param udma_q
 *
 * @return 0 if no error found.
 */

int al_udma_q_reset(struct al_udma_q *udma_q);

/**
 * enable/disable udma queue
 *
 * @param udma_q udma queue data structure
 * @param enable none zero value enables the queue, zero means disable
 */
void al_udma_q_enable(struct al_udma_q *udma_q, int enable);

/**
 * check if hardware state indicated a queue is enabled
 *
 * @param udma_q
 *
 * @return AL_TRUE if queue is enabled, AL_FALSE otherwise
 */
al_bool al_udma_q_is_enabled(struct al_udma_q *udma_q);

/**
 * return (by reference) a pointer to a specific queue date structure.
 * this pointer needed for calling functions (i.e. al_udma_desc_action_add) that
 * require this pointer as input argument.
 *
 * @param udma udma data structure
 * @param qid queue index
 * @param q_handle pointer to the location where the queue structure pointer
 * written to.
 *
 * @return  0 on success. -EINVAL otherwise.
 */
int al_udma_q_handle_get(struct al_udma *udma, uint32_t qid,
		      struct al_udma_q **q_handle);

/**
 * Change the UDMA's state
 *
 * @param udma udma data structure
 * @param state the target state
 *
 * @return 0
 */
int al_udma_state_set(struct al_udma *udma, enum al_udma_state state);

/**
 * return the current UDMA hardware state
 *
 * @param udma udma handle
 *
 * @return the UDMA state as reported by the hardware.
 */
enum al_udma_state al_udma_state_get(struct al_udma *udma);

/**
 * change and wait udma state
 *
 * @param dma the udma to change its state
 * @param new_state
 * @param flush_stream flush the stream during state change
 *              (Needed for state while UDMA recieves traffic)
 *
 * @return 0 on success. otherwise on failure.
 */
int al_udma_state_set_wait(struct al_udma *dma, enum al_udma_state new_state, al_bool flush_stream);

/*
 * Action handling
 */

/**
 * get number of descriptors that can be submitted to the udma.
 *
 * keep enough free descriptors to make sure that completion descriptor padding will not overrun
 * yet not acknowleged completion descriptors
 *
 * @param udma_q queue handle
 *
 * @return num of free descriptors.
 */
static INLINE uint32_t al_udma_available_get(struct al_udma_q *udma_q)
{
	uint16_t tmp = udma_q->next_cdesc_idx -
		(udma_q->next_desc_idx + AL_UDMA_MAX_NUM_CDESC_PER_CACHE_LINE);
	tmp &= udma_q->size_mask;

	return (uint32_t) tmp;
}

/**
 * check if queue has pending descriptors
 *
 * @param udma_q queue handle
 *
 * @return AL_TRUE if descriptors are submitted to completion ring and still
 * not completed (with ack). AL_FALSE otherwise.
 */
static INLINE al_bool al_udma_is_empty(struct al_udma_q *udma_q)
{
	if (((udma_q->next_cdesc_idx - udma_q->next_desc_idx) &
	     udma_q->size_mask) == 0)
		return AL_TRUE;

	return AL_FALSE;
}

/**
 * get next available descriptor
 * @param udma_q queue handle
 *
 * @return pointer to the next available descriptor
 */
static INLINE union al_udma_desc *al_udma_desc_get(struct al_udma_q *udma_q)
{
	union al_udma_desc *desc;
	uint16_t next_desc_idx;

	al_assert(udma_q);

	next_desc_idx = udma_q->next_desc_idx;
	desc = udma_q->desc_base_ptr + next_desc_idx;

	next_desc_idx++;

	/* if reached end of queue, wrap around */
	udma_q->next_desc_idx = next_desc_idx & udma_q->size_mask;

	return desc;
}

/**
 * get ring id for the last allocated descriptor
 * @param udma_q
 *
 * @return ring id for the last allocated descriptor
 * this function must be called each time a new descriptor is allocated
 * by the al_udma_desc_get(), unless ring id is ignored.
 */
static INLINE uint32_t al_udma_ring_id_get(struct al_udma_q *udma_q)
{
	uint32_t ring_id;

	al_assert(udma_q);

	ring_id = udma_q->desc_ring_id;

	/* calculate the ring id of the next desc */
	/* if next_desc points to first desc, then queue wrapped around */
	if (unlikely(udma_q->next_desc_idx) == 0)
		udma_q->desc_ring_id = (udma_q->desc_ring_id + 1) &
			DMA_RING_ID_MASK;
	return ring_id;
}

/* add DMA action - trigger the engine */
/**
 * add num descriptors to the submission queue.
 *
 * @param udma_q queue handle
 * @param num number of descriptors to add to the queues ring.
 *
 * @return 0;
 */
static INLINE int al_udma_desc_action_add(struct al_udma_q *udma_q,
					  uint32_t num)
{
	uint32_t *addr;

	al_assert(udma_q);
	al_assert((num > 0) && (num <= udma_q->size));

	addr = &udma_q->q_regs->rings.drtp_inc;
	/* make sure data written to the descriptors will be visible by the */
	/* DMA */
	al_local_data_memory_barrier();

	/*
	 * As we explicitly invoke the synchronization function
	 * (al_data_memory_barrier()), then we can use the relaxed version.
	 */
	al_reg_write32_relaxed(addr, num);

	return 0;
}

#define cdesc_is_first(flags) ((flags) & AL_UDMA_CDESC_FIRST)
#define cdesc_is_last(flags) ((flags) & AL_UDMA_CDESC_LAST)

/**
 * return pointer to the cdesc + offset desciptors. wrap around when needed.
 *
 * @param udma_q queue handle
 * @param cdesc pointer that set by this function
 * @param offset offset desciptors
 *
 */
static INLINE volatile union al_udma_cdesc *al_cdesc_next(
	struct al_udma_q		*udma_q,
	volatile union al_udma_cdesc	*cdesc,
	uint32_t			offset)
{
	volatile uint8_t *tmp = (volatile uint8_t *) cdesc + offset * udma_q->cdesc_size;
	al_assert(udma_q);
	al_assert(cdesc);

	/* if wrap around */
	if (unlikely((tmp > udma_q->end_cdesc_ptr)))
		return (union al_udma_cdesc *)
			(udma_q->cdesc_base_ptr +
			(tmp - udma_q->end_cdesc_ptr - udma_q->cdesc_size));

	return (volatile union al_udma_cdesc *) tmp;
}

/**
 * Return pointer to the cdesc + offset descriptors.
 * Wrap around when needed.
 *
 * Same as 'al_cdesc_next', but without volatile requirement.
 * Can be used for optimization, where there is a known range of descriptors
 * ready by the HW, and waiting for the SW to read.
 *
 * Notice: This function is unsafe, as it doesn't treat descriptors as volatile.
 *         To be used in cases where the user knows that these descriptors are ready.
 *         For instance, descriptors that are in the bounds of 'al_udma_cdesc_packet_get'
 *         returned value.
 *
 * @param udma_q queue handle
 * @param cdesc pointer that set by this function
 * @param offset offset descriptors
 *
 */
static INLINE union al_udma_cdesc *al_cdesc_next_unsafe(
	struct al_udma_q		*udma_q,
	union al_udma_cdesc		*cdesc,
	uint32_t			offset)
{
	uint8_t *tmp = (uint8_t *) cdesc + offset * udma_q->cdesc_size;

	al_assert(udma_q);
	al_assert(cdesc);

	/* if wrap around */
	if (unlikely((tmp > udma_q->end_cdesc_ptr)))
		return (union al_udma_cdesc *)
			(udma_q->cdesc_base_ptr +
			(tmp - udma_q->end_cdesc_ptr - udma_q->cdesc_size));

	return (union al_udma_cdesc *) tmp;
}

/**
 * check if the flags of the descriptor indicates that is new one
 * the function uses the ring id from the descriptor flags to know whether it
 * new one by comparing it with the curring ring id of the queue
 *
 * @param udma_q queue handle
 * @param flags the flags of the completion descriptor
 *
 * @return AL_TRUE if the completion descriptor is new one.
 * 	AL_FALSE if it old one.
 */
static INLINE al_bool al_udma_new_cdesc(struct al_udma_q *udma_q,
								uint32_t flags)
{
	if (((flags & AL_M2S_DESC_RING_ID_MASK) >> AL_M2S_DESC_RING_ID_SHIFT)
	    == udma_q->comp_ring_id)
		return AL_TRUE;
	return AL_FALSE;
}

/**
 * get next completion descriptor
 * this function will also increment the completion ring id when the ring wraps
 * around
 *
 * @param udma_q queue handle
 * @param cdesc current completion descriptor
 *
 * @return pointer to the completion descriptor that follows the one pointed by
 * cdesc
 */
static INLINE volatile union al_udma_cdesc *al_cdesc_next_update(
	struct al_udma_q		*udma_q,
	volatile union al_udma_cdesc	*cdesc)
{
	/* if last desc, wrap around */
	if (unlikely(((volatile uint8_t *) cdesc == udma_q->end_cdesc_ptr))) {
		udma_q->comp_ring_id =
		    (udma_q->comp_ring_id + 1) & DMA_RING_ID_MASK;
		return (union al_udma_cdesc *) udma_q->cdesc_base_ptr;
	}
	return (volatile union al_udma_cdesc *) ((volatile uint8_t *) cdesc + udma_q->cdesc_size);
}

/**
 * get next completed packet from completion ring of the queue
 *
 * @param udma_q udma queue handle
 * @param desc pointer that set by this function to the first descriptor
 * note: desc is valid only when return value is not zero
 * @return number of descriptors that belong to the packet. 0 means no completed
 * full packet was found.
 * If the descriptors found in the completion queue don't form full packet (no
 * desc with LAST flag), then this function will do the following:
 * (1) save the number of processed descriptors.
 * (2) save last processed descriptor, so next time it called, it will resume
 *     from there.
 * (3) return 0.
 * note: the descriptors that belong to the completed packet will still be
 * considered as used, that means the upper layer is safe to access those
 * descriptors when this function returns. the al_udma_cdesc_ack() should be
 * called to inform the udma driver that those descriptors are freed.
 */
uint32_t al_udma_cdesc_packet_get(
	struct al_udma_q		*udma_q,
	volatile union al_udma_cdesc	**desc);

/** get completion descriptor pointer from its index */
#define al_udma_cdesc_idx_to_ptr(udma_q, idx)				\
	((volatile union al_udma_cdesc *) ((udma_q)->cdesc_base_ptr +	\
				(idx) * (udma_q)->cdesc_size))


/**
 * return number of all completed descriptors in the completion ring
 *
 * @param udma_q udma queue handle
 * @param cdesc pointer that set by this function to the first descriptor
 * note: desc is valid only when return value is not zero
 * note: pass NULL if not interested
 * @return number of descriptors. 0 means no completed descriptors were found.
 * note: the descriptors that belong to the completed packet will still be
 * considered as used, that means the upper layer is safe to access those
 * descriptors when this function returns. the al_udma_cdesc_ack() should be
 * called to inform the udma driver that those descriptors are freed.
 */
static INLINE uint32_t al_udma_cdesc_get_all(
	struct al_udma_q		*udma_q,
	volatile union al_udma_cdesc	**cdesc)
{
	uint16_t count = 0;

	al_assert(udma_q);

	udma_q->comp_head_idx = (uint16_t)
				(al_reg_read32(&udma_q->q_regs->rings.crhp) &
						0xFFFF);

	count = (udma_q->comp_head_idx - udma_q->next_cdesc_idx) &
		udma_q->size_mask;

	if (cdesc)
		*cdesc = al_udma_cdesc_idx_to_ptr(udma_q, udma_q->next_cdesc_idx);

	return (uint32_t)count;
}

/**
 * acknowledge the driver that the upper layer completed processing completion
 * descriptors
 *
 * @param udma_q udma queue handle
 * @param num number of descriptors to acknowledge
 *
 * @return 0
 */
static INLINE int al_udma_cdesc_ack(struct al_udma_q *udma_q, uint32_t num)
{
	al_assert(udma_q);

	udma_q->next_cdesc_idx += num;
	udma_q->next_cdesc_idx &= udma_q->size_mask;

	return 0;
}


/**
 * get udma unit regs base
 *
 * @param udma udma handle
 *
 * @return udma regs base pointer
 */
static INLINE void __iomem *al_udma_unit_regs_base_get(struct al_udma *udma)
{
	al_assert(udma);

	return udma->unit_regs_base;
}

/**
 * Read value from UDMA mailbox
 *
 * @param	udma udma data structure
 * @param	mailbox_id UDMA mailbox number to read from
 * @param	val pointer to variable which read to
 *
 */
void al_udma_mailbox_read(struct al_udma *udma, unsigned int mailbox_id, uint32_t *val);

/**
 * Write value to UDMA mailbox
 *
 * @param	udma udma data structure
 * @param	mailbox_id UDMA mailbox number to write to
 * @param	val value to write
 *
 */
void al_udma_mailbox_write(struct al_udma *udma, unsigned int mailbox_id, uint32_t val);

/* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */

#endif /* __AL_HAL_UDMA_H__ */
/** @} end of UDMA group */
