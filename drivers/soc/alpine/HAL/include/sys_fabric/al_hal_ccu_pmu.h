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
 * @defgroup group_ccu_pmu CCU performance monitor
 * @ingroup groupccu
 *  @{
 * @section overview Overview
 * The CCU performance monitor HAL can be used to configure the CCU
 * performance monitor counters in the system fabric.
 *
 * Common operation example:
 * @code
 *
 * // initialize ccu pmu handle
 *
 * al_ccu_pmu_handle_init(handle, AL_NB_CCI_BASE);
 *
 * // set the counter
 * al_ccu_pmu_set_counter(handle, counter_index,
 *                        CCU_PMU_REQ_EVENT_0, CCU_PMU_SRC_SLV_0);
 *
 * // reset all performance counters
 * al_ccu_pmu_cntrs_reset(handle)
 *
 * // enable PMU
 * al_ccu_pmu_enable(handle)
 *
 * // enable counter
 * al_ccu_pmu_counter_enable(handle, counter_index)
 *
 * start_value = al_ccu_pmu_counter_read(handle, counter_index)
 *
 * <section of code to monitor>
 *
 * end_value = al_ccu_pmu_counter_read(handle, counter_index)
 * @endcode
 * @file   al_hal_ccu_pmu.h
 *
 * @brief API for CCU PMU
 **/
#ifndef __AL_HAL_CCU_PMU_H__
#define __AL_HAL_CCU_PMU_H__


/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */


enum AL_CCU_PMU_EVENT_TYPE {

	/*---------------------------------------------------------------
	 * Slave interface
	 *---------------------------------------------------------------*/

	/**
	 * Read request handshake: any.
	 */
	AL_CCU_PMU_SLV_READ_ANY = 0x00,

	/**
	 * Read request handshake: device transaction.
	 */
	AL_CCU_PMU_SLV_READ_DEV_TRANSAC  = 0x01,

	/**
	 * Read request handshake:
	 * normal, non-shareable or system-shareable,
	 * but not barrier or cache maintenance operation.
	 */
	AL_CCU_PMU_SLV_READ_NORM = 0x02,

	/**
	 *  Read request handshake: inner- or outer-shareable,
	 *  but not barrier, DVM message or cache maintenance
	 *  operation.
	 */
	AL_CCU_PMU_SLV_READ_SHARE = 0x03,

	/**
	 *  Read request handshake: cache maintenance operation,
	 *  CleanInvalid, CleanShared, MakeInvalid.
	 */
	AL_CCU_PMU_SLV_READ_CACHE_MAINT = 0x04,

	/**
	 * Read request handshake: memory barrier.
	 */
	AL_CCU_PMU_SLV_READ_MB = 0x05,

	/**
	 *  Read request handshake: synchronization barrier.
	 */
	AL_CCU_PMU_SLV_READ_SMB = 0x06,

	/**
	 *  Read request handshake: DVM message, not synchronization.
	 */
	AL_CCU_PMU_SLV_READ_DVM_NOT_SYNC = 0x07,

	/**
	 *  Read request handshake: DVM message, synchronization.
	 */
	AL_CCU_PMU_SLV_READ_DVM_SYNC = 0x08,

	/**
	 * Read request stall cycle because
	 * the transaction tracker is full.
	 * Increase SIx_R_MAX to avoid this stall.
	 */
	AL_CCU_PMU_SLV_READ_STALL_CYCLE_TT_FULL = 0x09,

	/**
	 *  Read data last handshake: data returned from the snoop
	 *  instead of from downstream.
	 */
	AL_CCU_PMU_SLV_READ_DATA_FROM_SNOOP = 0x0A,

	/**
	 *  Read data stall cycle: RVALIDS is HIGH, RREADYS is LOW.
	 */
	AL_CCU_PMU_SLV_READ_STALL_CYCLE_RVALIDS_HIGH_RREADYS_LOW = 0x0B,

	/**
	 *  Write request handshake: any.
	 */
	AL_CCU_PMU_SLV_WRITE = 0x0C,

	/**
	 *  Write request handshake: device transaction.
	 */
	AL_CCU_PMU_SLV_WRITE_DEVICE_TRANSACTION = 0x0D,

	/**
	 * Write request handshake: normal, non-shareable,
	 * or system-shareable, but not barrier.
	 */
	AL_CCU_PMU_SLV_WRITE_NORMAL = 0x0E,

	/**
	 *  Write request handshake: inner- or outer-shareable,
	 *  WriteBack or WriteClean.
	 */
	AL_CCU_PMU_SLV_WRITE_SHARE = 0x0F,

	/**
	 * Write request handshake: WriteUnique.
	 */
	AL_CCU_PMU_SLV_WRITE_UNIQUE = 0x10,

	/**
	 * Write request handshake: WriteLineUnique.
	 */
	AL_CCU_PMU_SLV_WRITE_LINE_UNIQUE = 0x11,

	/**
	 *  Write request handshake: Evict.
	 */
	AL_CCU_PMU_SLV_WRITE_EVICT = 0x12,

	/**
	 * Write request stall cycle because the transaction tracker is full.
	 * Increase SIx_W_MAX to avoid this stall.
	 */
	AL_CCU_PMU_SLV_WRITE_STALL_CYCLE_TT_FULL = 0x13,

	/*---------------------------------------------------------------
	 * Master interface
	 *---------------------------------------------------------------*/

	/**
	 * RETRY of speculative fetch transaction.
	 */
	AL_CCU_PMU_MST_RETRY_SPEC_FETCH_TRANSACTION = 0x14,

	/**
	 *  Read request stall cycle because of an address hazard.
	 */
	AL_CCU_PMU_MST_READ_STALL_CYCLE_ADDR_HAZARD = 0x15,

	/**
	 * Read request stall cycle because of an ID hazard.
	 */
	AL_CCU_PMU_MST_READ_STALL_CYCLE_ID_HAZARD = 0x16,

	/**
	 *  Read request stall cycle because the transaction tracker is full.
	 *  Increase MIx_R_MAX to avoid this stall. See the CoreLink AL_CCU-400
	 *  Cache Coherent Interconnect Integration Manual.
	 */
	AL_CCU_PMU_MST_READ_STALL_CYCLE_TT_FULL = 0x17,

	/**
	 * Read request stall cycle because of a barrier hazard.
	 */
	AL_CCU_PMU_MST_READ_STALL_CYCLE_BARRIER_HAZARD = 0x18,

	/**
	 *  Write request stall cycle because of a barrier hazard.
	 */
	AL_CCU_PMU_MST_WRITE_STALL_CYCLE_BARRIER_HAZARD = 0x19,

	/**
	 *  Write request stall cycle because the transaction tracker is full.
	 *  Increase MIx_W_MAX to avoid this stall. See the CoreLink AL_CCU-400
	 *  Cache Coherent Interconnect Integration Manual.
	 */
	AL_CCU_PMU_MST_WRITE_STALL_CYCLE_TT_FULL = 0x1A,

#if (AL_DEV_ID < AL_DEV_ID_ALPINE_V3)
	AL_CCU_PMU_EVENT_LAST = 0x1B,
#else
	AL_CCU_PMU_EVENT_LAST = 0x20,
#endif
};

enum AL_CCU_PMU_SRC_INTERFACE {
	AL_CCU_PMU_SRC_SLV_SB_0 = 0x0,       /* Slave interface 0, S0 - Southbridge 0 */
	AL_CCU_PMU_SRC_SLV_SB_1 = 0x1,       /* Slave interface 1, S1 - Southbridge 1 */
	AL_CCU_PMU_SRC_SLV_DEBUG = 0x2,      /* Slave interface 2, S2 - Debug */
	AL_CCU_PMU_SRC_SLV_CPU = 0x3,        /* Slave interface 3, S3 - CPU */
	AL_CCU_PMU_SRC_SLV_NOT_USED_4 = 0x4, /* Slave interface 4, S4 - Not Used */
	AL_CCU_PMU_SRC_MST_SB = 0x5,         /* Master interface 0, M0 - Southbridge */
	AL_CCU_PMU_SRC_MST_DDR = 0x6,        /* Master interface 1, M1 - DDR */
	AL_CCU_PMU_SRC_MST_NOT_USED_2 = 0x7, /* Master interface 2, M2 - Not Used*/
#if (AL_DEV_ID < AL_DEV_ID_ALPINE_V3)
	AL_CCU_PMU_SRC_LAST  = 0x8,
#else
	AL_CCU_PMU_SRC_LAST  = 0x10,
#endif
};

struct al_hal_ccu_pmu_handle {
	struct al_ccu_regs *ccu_regs;
};

/**
 * @brief Initialize ccu pmu handle
 *
 * @param  pmu_handle
 *	   CCU pmu handle struct
 *
 * @param  ccu_regs_base
 *         ccu registers base address
 */
void al_ccu_pmu_handle_init(struct al_hal_ccu_pmu_handle *pmu_handle, void *ccu_regs_base);

/**
 * @brief Enable all PMU counters
 *
 * @param  pmu_handle
 *	   CCU pmu handle struct
 */
void al_ccu_pmu_enable(struct al_hal_ccu_pmu_handle *pmu_handle);

/**
 * @brief Disable all PMU counters
 *
 * @param  pmu_handle
 *	   CCU pmu handle struct
 */
void al_ccu_pmu_disable(struct al_hal_ccu_pmu_handle *pmu_handle);

/**
 * @brief Resets al PMU counters
 *
 * @param  pmu_handle
 *	   CCU pmu handle struct
 */
void al_ccu_pmu_cntrs_reset(struct al_hal_ccu_pmu_handle *pmu_handle);

/**
 * @brief Set type of counter
 *
 * @param       pmu_handle
 *	        CCU pmu handle struct
 *
 * @param	index
 *		Index of the counter
 *
 * @param       event
 *              Type of the counter
 *
 * @param       src
 *              Source of the counter
 *
 */
void al_ccu_pmu_set_counter(struct al_hal_ccu_pmu_handle *pmu_handle,
			    int index,
			    enum AL_CCU_PMU_EVENT_TYPE event,
			    enum AL_CCU_PMU_SRC_INTERFACE src);

/**
 * @brief Enable PMU counter
 *
 * @param       pmu_handle
 *	        CCU pmu handle struct
 *
 * @param	index
 *		Index of the counter
 */
void al_ccu_pmu_counter_enable(struct al_hal_ccu_pmu_handle *pmu_handle,
			       int index);

/**
 * @brief Disable PMU counter
 *
 * @param       pmu_handle
 *	        CCU pmu handle struct
 *
 * @param	index
 *		Index of the counter
 */
void al_ccu_pmu_counter_disable(struct al_hal_ccu_pmu_handle *pmu_handle,
				int index);

/**
 * @brief Read PMU counter
 *
 * @param       pmu_handle
 *	        CCU pmu handle struct
 *
 * @param	index
 *		Index of the counter
 *
 * @returns	Value of the counter
 */
uint32_t al_ccu_pmu_counter_read(struct al_hal_ccu_pmu_handle *pmu_handle,
				 int index);


/* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */
/** @} end of CCU group */

#endif
