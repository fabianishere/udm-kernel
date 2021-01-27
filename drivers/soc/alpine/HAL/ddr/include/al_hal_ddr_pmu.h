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
 * @defgroup group_ddr_pmu DDR performance monitor
 * @ingroup groupddr
 *  @{
 * @section overview Overview
 * The DDR performance monitor HAL can be used to configure the DDR
 * performance monitor counters in the system fabric.
 *
 * Common operation example:
 * @code
 *	int main()
 *	{
 *		unsigned int num_counters;
 *		uint64_t pmu_counter;
 *
 *		// Get number of DDR performance counters
 *		num_counters = al_ddr_pmu_num_counters_get((void __iomem *)AL_NB_SERVICE_BASE);
 *
 *		// Disable all counters
 *		al_ddr_pmu_disable((void __iomem *)AL_NB_SERVICE_BASE);
 *
 *		// Select event types to monitor and counter index
 *		al_ddr_pmu_event_sel((void __iomem *)AL_NB_SERVICE_BASE, ...);
 *
 *		// Enable all counters
 *		al_ddr_pmu_enable((void __iomem *)AL_NB_SERVICE_BASE);
 *
 *		// Run the code you wish to profile
 *		,,,
 *
 *		// Pause all counters
 *		al_ddr_pmu_pause((void __iomem *)AL_NB_SERVICE_BASE);
 *
 *		// Read performance monitor counters
 *		pmu_counter = al_ddr_pmu_counter_get((void __iomem *)AL_NB_SERVICE_BASE, ...);
 *	}
 * @endcode
 * @file   al_hal_ddr_pmu.h
 *
 * @brief Header file for the DDR PMU HAL driver
 */

#ifndef __AL_HAL_DDR_PMU_H__
#define __AL_HAL_DDR_PMU_H__

#include "al_hal_common.h"

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

/**
 * DDR Performance monitor types
 * Values must not change (as these are the values the HW expects)
*/
enum al_ddr_perf_types {
	/**
	 * Fabric write access
	 * Counts number of fabric write commands
	*/
	AL_DDR_PERFCNTR_HIF_WR                  = 0,

	/**
	 * Fabric read or write
	 * Counts number of fabric read and write commands
	*/
	AL_DDR_PERFCNTR_HIF_RD_OR_WR            = 1,

	/**
	 * ZQCS command
	 * Counts number of ZQ calibration short commands,
	 * not including init interval
	*/
	AL_DDR_PERFCNTR_OP_IS_ZQCS              = 2,

	/**
	 * ZQCL command
	 * Counts number of ZQ calibration long commands,
	 * not including init interval
	*/
	AL_DDR_PERFCNTR_OP_IS_ZQCL              = 3,

	/**
	 * Lode Mode register
	 * Counts number of Load Mode registers,
	 * not including init interval
	*/
	AL_DDR_PERFCNTR_OP_IS_LOAD_MODE         = 4,

	/**
	 * Refresh command
	 * Counts number of refresh commands,
	 * not including init interval
	*/
	AL_DDR_PERFCNTR_OP_IS_REFRESH           = 5,

	/**
	 * Self-refresh mode
	 * Counts number of cycles the DRAM was in Self-refresh mode
	*/
	AL_DDR_PERFCNTR_SELFREF_MODE            = 6,

	/**
	 * DRAM enters Power down mode
	 * Counts number of times the DRAM entered Power-down mode
	*/
	AL_DDR_PERFCNTR_OP_IS_ENTER_POWERDOWN   = 7,

	/**
	 * DRAM enters Self refresh mode
	 * Counts number of times the DRAM entered Self-refresh mode
	*/
	AL_DDR_PERFCNTR_OP_IS_ENTER_SELFREF     = 8,

	/**
	 * WAW hazard
	 * Counts number of WAW hazards if write_combine is disabled
	*/
	AL_DDR_PERFCNTR_WAW_HAZARD              = 9,

	/**
	 * RAW hazard
	 * Counts number of RAW hazards
	*/
	AL_DDR_PERFCNTR_RAW_HAZARD              = 10,

	/**
	 * WAR hazard
	 * Counts number of WAR hazards
	*/
	AL_DDR_PERFCNTR_WAR_HAZARD              = 11,

	/**
	 * Write combine
	 * Counts number of write combines in the DDR controller
	*/
	AL_DDR_PERFCNTR_WRITE_COMBINE           = 12,

	/**
	 * Read-write transition
	 * Counts number of times the DDR controller switched between read
	 * and write modes
	*/
	AL_DDR_PERFCNTR_RDWR_TRANSITIONS        = 13,

	/**
	 * Pre-charge commands because of others
	 * Counts number of pre-charge commands sent that were not required
	 * by a read / write (max open time, etc.)
	*/
	AL_DDR_PERFCNTR_PRECHARGE_FOR_OTHER     = 14,

	/**
	 * Pre-charge commands because of page miss
	 * Counts number of pre-charge commands sent because they were
	 * required by a read / write
	*/
	AL_DDR_PERFCNTR_PRECHARGE_FOR_RDWR      = 15,

	/**
	 * Pre-charge command
	 * Counts number of pre-charge commands
	*/
	AL_DDR_PERFCNTR_OP_IS_PRECHARGE         = 16,

	/**
	 * Write command
	 * Counts number of DRAM write commands
	*/
	AL_DDR_PERFCNTR_OP_IS_WR                = 17,

	/**
	 * Read command
	 * Counts number of DRAM read commands
	*/
	AL_DDR_PERFCNTR_OP_IS_RD                = 18,

	/**
	 * Read ACTIVATE command
	 * Counts number of ACTIVATE commands in read mode
	*/
	AL_DDR_PERFCNTR_OP_IS_RD_ACTIVATE       = 19,

	/**
	 * READ or WRITE command
	 * Counts number or READ and WRITE commands
	*/
	AL_DDR_PERFCNTR_OP_IS_RD_OR_WR          = 20,

	/**
	 * Activate command
	 * Counts number of ACTIVATE commands
	*/
	AL_DDR_PERFCNTR_OP_IS_ACTIVATE          = 21,

	/**
	 * WRITE command when critical
	 * Counts number of WRITE commands issued while the write queue was
	 * in a critical state
	*/
	AL_DDR_PERFCNTR_WR_XACT_WHEN_CRITICAL   = 22,

	/**
	 * Low priority READ when critical
	 * Counts number of low-priority READ commands issued while the low
	 * priority queue was in a critical state
	*/
	AL_DDR_PERFCNTR_LPR_XACT_WHEN_CRITICAL  = 23,

	/**
	 * High priority READ when critical
	 * Counts number of high-priority READ commands issued while the high
	 * priority queue was in a critical state
	*/
	AL_DDR_PERFCNTR_HPR_XACT_WHEN_CRITICAL  = 24,

	/**
	 * DFI read data valid
	 * Counts number of data reads on DFI
	*/
	AL_DDR_PERFCNTR_DFI_RD_DATA_CYCLES      = 25,

	/**
	 * DFI write data enable
	 * Counts number of data writes on DFI
	*/
	AL_DDR_PERFCNTR_DFI_WR_DATA_CYCLES      = 26,

	/**
	 * BYPASS activate command
	 * Counts number of ACTIVATE commands issued through the BYPASS mechanism
	*/
	AL_DDR_PERFCNTR_ACT_BYPASS              = 27,

	/**
	 * BYPASS high priority read command
	 * Counts number of high-priority READ commands issued through the
	 * BYPASS mechanism
	*/
	AL_DDR_PERFCNTR_READ_BYPASS             = 28,

	/**
	 * Fabric high priority READ
	 * Counts number of fabric high-priority READ commands.
	*/
	AL_DDR_PERFCNTR_HIF_HI_PRI_RD           = 29,

	/**
	 * RMW command
	 * Counts number of RMW (read-modify-write) commands
	*/
	AL_DDR_PERFCNTR_HIF_RMW                 = 30,

	/**
	 * Fabric READ command
	 * Counts number of Fabric READ commands (both high- and low-priority)
	*/
	AL_DDR_PERFCNTR_HIF_RD                  = 31,

	/**
	 * Total cycles on Low Priority reads credit
	 * Count cycles for not serving low-priority reads due to
	 * non-available credit
	*/
	AL_DDR_PERFCNTR_LPR_REQ_WITH_NOCREDIT   = 32,

	/**
	 * Total cycles on High Priority reads credit
	 * Count cycles for not serving high-priority reads due to
	 * non-available credit
	*/
	AL_DDR_PERFCNTR_HPR_REQ_WITH_NOCREDIT   = 33,

	/**
	 * DRAM enters Maximum power saving mode
	 * Counts number of times the DRAM entered Maximum power saving mode
         * Applicable to ALPINE V2 only.
	*/
	AL_DDR_PERFCNTR_OP_IS_ENTER_MPSM        = 34

};


/**
 * Get the number of DDR performance counters
 *
 * @param[in] nb_regs_base
 *	base address of the system fabric service registers
 *
 * @return number of DDR performance counters
 */
unsigned int al_ddr_pmu_num_counters_get(
	void __iomem			*nb_regs_base);

/**
 * Disable all DDR performance counters
 * Will reset the counters value to 0
 *
 * @param[in] nb_regs_base
 *	base address of the system fabric service registers
 */
void al_ddr_pmu_disable(
	void __iomem			*nb_regs_base);

/**
 * Pause all DDR performance counters
 * Resumed with al_ddr_pmu_enable
 *
 * @param[in] nb_regs_base
 *	base address of the system fabric service registers
 */
void al_ddr_pmu_pause(
	void __iomem			*nb_regs_base);

/**
 * Enable all DDR performance counters
 *
 * @param[in] nb_regs_base
 *	base address of the system fabric service registers
 */
void al_ddr_pmu_enable(
	void __iomem			*nb_regs_base);

/**
 * Select the event to be counted by the DDR performance counter
 *
 * @param[in] nb_regs_base
 *	base address of the system fabric service registers
 * @param[in] idx
 *	performance counter index
 *	must be smaller than al_ddr_pmu_num_counters_get()
 * @param[in] type
 *	event type to be counted
 */
void al_ddr_pmu_event_sel(
	void __iomem			*nb_regs_base,
	unsigned int			idx,
	enum al_ddr_perf_types		type);

/**
 * Get the value of the DDR performance counter
 *
 * @param[in] nb_regs_base
 *	base address of the system fabric service registers
 * @param[in] idx
 *	performance counter index
 *	must be smaller than al_ddr_pmu_num_counters_get()
 *
 * @return value of DDR performance counter (48 bit)
 */
uint64_t al_ddr_pmu_counter_get(
	void __iomem			*nb_regs_base,
	unsigned int			idx);

/**
 * Enable a specific DDR performance counter
 *
 * @param[in] nb_regs_base
 *	base address of the system fabric service registers
 * @param[in] idx
 *	performance counter index
 *	must be smaller than al_ddr_pmu_num_counters_get()
 */
void al_ddr_pmu_enable_counter(
	void __iomem			*nb_regs_base,
	unsigned int			idx);

/**
 * Disable a specific DDR performance counter
 * Will reset the counter value to 0
 *
 * @param[in] nb_regs_base
 *	base address of the system fabric service registers
 * @param[in] idx
 *	performance counter index
 *	must be smaller than al_ddr_pmu_num_counters_get()
 */
void al_ddr_pmu_disable_counter(
	void __iomem			*nb_regs_base,
	unsigned int			idx);

/**
 * Pause a specific DDR performance counter
 * Resumed with al_ddr_pmu_enable_counter
 *
 * @param[in] nb_regs_base
 *	base address of the system fabric service registers
 * @param[in] idx
 *	performance counter index
 *	must be smaller than al_ddr_pmu_num_counters_get()
 */
void al_ddr_pmu_pause_counter(
	void __iomem			*nb_regs_base,
	unsigned int			idx);


/* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */
/** @} end of DDR group */
#endif

