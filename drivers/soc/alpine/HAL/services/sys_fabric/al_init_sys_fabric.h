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
 * The fabric consists of CCU, SMMU(*2), GIC(*2), and NB registers.
 *
 * The fabric hal provides simple api for fabric initialization, and not a
 * complete coverage of the unit's functionality.
 * main objective: to keep consistent fabric initialization between
 * different environments. Only functions used by all environments added.
 *
 * GIC and SMMU are only handled when initializing the secure context.
 * Both internal and external gic are initialized in that aspect.
 */

#ifndef __AL_HAL_SYS_FABRIC_H__
#define __AL_HAL_SYS_FABRIC_H__

#include <al_hal_common.h>

#ifdef __cplusplus
extern "C" {
#endif

/** CPU RAM ID */
enum al_cpu_ram_id {
	AL_CPU_SYS_FABRIC_CPU_RAM_ID_L1_I_TAG	= 0x00, /** L1-I tag RAM */
	AL_CPU_SYS_FABRIC_CPU_RAM_ID_L1_I_DATA	= 0x01, /** L1-I data RAM */
	AL_CPU_SYS_FABRIC_CPU_RAM_ID_L1_I_BTB	= 0x02, /** L1-I BTB RAM */
	AL_CPU_SYS_FABRIC_CPU_RAM_ID_L1_D_TAG	= 0x08, /** L1-D tag RAM */
	AL_CPU_SYS_FABRIC_CPU_RAM_ID_L1_D_DATA	= 0x09, /** L1-D data RAM */
	AL_CPU_SYS_FABRIC_CPU_RAM_ID_L2_TLB	= 0x18, /** L2 TLB RAM */
};

/** L2 RAM ID */
enum al_l2_ram_id {
	AL_L2_SYS_FABRIC_L2_RAM_ID_L2_TAG	= 0x10, /** L2 tag RAM */
	AL_L2_SYS_FABRIC_L2_RAM_ID_L2_DATA	= 0x11, /** L2 data RAM */
	AL_L2_SYS_FABRIC_L2_RAM_ID_L2_SNP_TAG	= 0x12, /** L2 snoop tag RAM */
	AL_L2_SYS_FABRIC_L2_RAM_ID_L2_DIRTY	= 0x14, /** L2 dirty RAM */
};

/** CPU memory error syndrome structure */
struct al_cpu_mem_err_syndrome {
	/**
	 * Valid flag. This flag is set to 1 on the first memory error.
	 * It is a sticky flag such that after it is set, it remains set until
	 * 'al_cpu_mem_err_syndrome_clear' is called.
	 */
	int				valid;

	/**
	 * Fatal flag. This flag is set to 1 on the first memory error that
	 * caused a data abort. It is a sticky flag such that after it is set,
	 * it remains set until 'al_cpu_mem_err_syndrome_clear' is
	 * called.
	 */
	int				fatal;

	/**
	 * This field is set to 0 on the first memory error and is incremented
	 * on any memory error that exactly matches the RAMID, bank, way or
	 * index information in this structure while the sticky Valid flag is
	 * set.
	 */
	int				err_cnt_repeat;

	/**
	 * This field is set to 0 on the first memory error and is incremented
	 * on any memory error that does not match the RAMID, bank, way, or
	 * index information in this structure while the sticky Valid flag is
	 * set.
	 */
	int				err_cnt_other;

	/**
	 * RAM Identifier
	 * Indicates the RAM, the first memory error occurred in.
	 */
	enum al_cpu_ram_id	ram_id;

	/**
	 * Indicates the bank or way of the RAM where the first memory error
	 * occurred.
	 */
	int				bank_way;

	/**
	 * Indicates the index address of the first memory error
	 */
	int				index;
};

/** L2 memory error syndrome structure */
struct al_l2_mem_err_syndrome {
	/**
	 * Valid flag. This flag is set to 1 on the first memory error.
	 * It is a sticky flag such that after it is set, it remains set until
	 * 'al_cpu_mem_err_syndrome_clear' is called.
	 */
	int				valid;

	/**
	 * Fatal flag. This flag is set to 1 on the first memory error that
	 * caused a data abort. It is a sticky flag such that after it is set,
	 * it remains set until 'al_cpu_mem_err_syndrome_clear' is
	 * called.
	 */
	int				fatal;

	/**
	 * This field is set to 0 on the first memory error and is incremented
	 * on any memory error that exactly matches the RAMID, bank, way or
	 * index information in this structure while the sticky Valid flag is
	 * set.
	 */
	int				err_cnt_repeat;

	/**
	 * This field is set to 0 on the first memory error and is incremented
	 * on any memory error that does not match the RAMID, bank, way, or
	 * index information in this structure while the sticky Valid flag is
	 * set.
	 */
	int				err_cnt_other;

	/**
	 * RAM Identifier
	 * Indicates the RAM, the first memory error occurred in.
	 */
	enum al_l2_ram_id	ram_id;

	/**
	 * Indicates which processor and way of the RAM where the first memory error occurred.
	 * For L2 tag, data, and dirty RAMs, indicates one of 16 ways, from way 0 to way 15.
	 */
	int				cpuid_way;

	/**
	 * Indicates the index address of the first memory error
	 */
	int				index;
};

/**
 * Initialize NB service regs
 *
 * @param nb_regs_address
 *	address of nb service registers
 * @param dev_ord_relax
 *	optimization: relax ordering between device-access reads and
 *	writes to different addresses.
 */
void al_nbservice_init(void __iomem *nb_regs_address,
		al_bool dev_ord_relax);

/**
 * Initialize CCU for a specific cluster
 *
 * @param ccu_address
 *	address of ccu registers
 * @param iocc
 *	enable I/O cache coherence
 * @param cluster
 *	cluster number
 */
void al_ccu_cluster_init(void __iomem *ccu_address, al_bool iocc, unsigned int cluster);

/**
 * Initialize CCU
 *
 * @param ccu_address
 *	address of ccu registers
 * @param iocc
 *	enable I/O cache coherency
 */
void al_ccu_init(void __iomem *ccu_address, al_bool iocc);

/**
 * Clear NB service regs settings
 *
 * @param nb_regs_address
 *	address of nb service registers
 */
void al_nbservice_clear_settings(void __iomem *nb_regs_address);

/**
 * Clear ccu settings for a specific cluster
 *
 * @param ccu_address
 *	address of ccu registers
 * @param cluster
 *	cluster number
 */
void al_ccu_cluster_clear_settings(void __iomem *ccu_address, unsigned int cluster);

/**
 * Clear ccu settings
 *
 * @param ccu_address
 *	address of ccu registers
 */
void al_ccu_clear_settings(void __iomem *ccu_address);

/**
 * Secure initialization for system fabric
 *
 * This function must be called from secure context.
 * It sets the fabric to allow further setup from
 * non-secure context.
 *
 * This function must be called once only per init.
 *
 * @param nb_base_address
 *	base address of the north-bridge
 */
void al_sys_fabric_security_init(void __iomem *nb_base_address);

/**
 * Secure initialization for the CPU cluster
 *
 * This function must be called from secure context.
 * It sets the CPU cluster to allow further setup from
 * non-secure context.
 *
 * This function must be called once only per init, or after CPU cluster
 * power-down
 */
void al_sys_fabric_cpu_cluster_security_init(void);

/**
 * Per-CPU Secure initialization for system fabric
 *
 * This function must be called from secure context.
 * It sets the fabric to allow further setup from
 * non-secure context.
 *
 * This function must be called once from each CPU. Can be done either
 * before or after al_fabric_security_init.
 */
void al_sys_fabric_percpu_security_init(void);

/**
 * Per-CPU Per-wakeup Secure initialization for system fabric
 *
 * This function must be called from secure context.
 * It sets the fabric to allow further setup from
 * non-secure context.
 *
 * This function must be called once from each CPU per init, or after
 * CPU power-down
 */
void al_sys_fabric_percpu_perwakeup_security_init(void);

/**
 * Per-CPU Secure notify sleep
 *
 * This function must be called from a secure context.
 * It notifies the appropriate GIC that the cpu is about to be powered down.
 * For platforms using gicv2 - it will do nothing.
 *
 * This function must be called from each CPU before each CPU powerdown.
 */
void al_gic_cpu_sleepnotify(void);

/**
 * @brief Current CPU memory error syndrome get
 *
 * @param	syndrome
 *		Current CPU memory error syndrome
 */
void al_cpu_mem_err_syndrome_get(
	struct al_cpu_mem_err_syndrome *syndrome);

/**
 * @brief Current CPU memory error syndrome clear
 */
void al_cpu_mem_err_syndrome_clear(
	void);

/**
 * @brief L2 memory error syndrome get
 *
 * @param	syndrome
 *		L2 memory error syndrome
 */
void al_l2_mem_err_syndrome_get(
	struct al_l2_mem_err_syndrome *syndrome);

/**
 * @brief L2 memory error syndrome clear
 */
void al_l2_mem_err_syndrome_clear(
	void);

#ifdef __cplusplus
}
#endif

#endif /* AL_HAL_SYS_FABRIC_H_ */
