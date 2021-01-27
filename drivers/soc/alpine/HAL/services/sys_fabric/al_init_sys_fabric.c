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

#if ((defined(AL_DEV_ID) && (AL_DEV_ID <= AL_DEV_ID_ALPINE_V2)) ||\
	(defined(AL_DEV_ID) && defined(AL_DEV_REV_ID)))

#include <al_hal_common.h>
#include <al_hal_reg_utils.h>
#include <al_hal_nb_regs.h>
#include "al_hal_ccu_regs.h"
#include <al_init_sys_fabric.h>
#include "al_init_smmu_regs.h"
#include "al_init_gicv2_regs.h"
#include "al_init_gicv3_regs.h"
#include "al_hal_iomap.h"
#include "al_sysreg_aarch64.h"

/* CPU memory syndrome register definitions */
#define CPUMERRSR_HI_FATAL_SHIFT		31
#define CPUMERRSR_HI_ERR_CNT_OTHER_MASK		AL_FIELD_MASK(15, 8)
#define CPUMERRSR_HI_ERR_CNT_OTHER_SHIFT	8
#define CPUMERRSR_HI_ERR_CNT_REPEAT_MASK	AL_FIELD_MASK(7, 0)
#define CPUMERRSR_HI_ERR_CNT_REPEAT_SHIFT	0

#define CPUMERRSR_LO_VALID_SHIFT		31
#define CPUMERRSR_LO_RAM_ID_MASK		AL_FIELD_MASK(30, 24)
#define CPUMERRSR_LO_RAM_ID_SHIFT		24
#define CPUMERRSR_LO_BANK_WAY_MASK		AL_FIELD_MASK(22, 18)
#define CPUMERRSR_LO_BANK_WAY_SHIFT		18
#define CPUMERRSR_LO_INDEX_MASK			AL_FIELD_MASK(17, 0)
#define CPUMERRSR_LO_INDEX_SHIFT		0

/* L2 memory syndrome register definitions */
#define L2MERRSR_HI_FATAL_SHIFT			31
#define L2MERRSR_HI_ERR_CNT_OTHER_MASK		AL_FIELD_MASK(15, 8)
#define L2MERRSR_HI_ERR_CNT_OTHER_SHIFT		8
#define L2MERRSR_HI_ERR_CNT_REPEAT_MASK	AL_FIELD_MASK(7, 0)
#define L2MERRSR_HI_ERR_CNT_REPEAT_SHIFT	0

#define L2MERRSR_LO_VALID_SHIFT			31
#define L2MERRSR_LO_RAM_ID_MASK			AL_FIELD_MASK(30, 24)
#define L2MERRSR_LO_RAM_ID_SHIFT		24
#define L2MERRSR_LO_CPUID_WAY_MASK		AL_FIELD_MASK(21, 18)
#define L2MERRSR_LO_CPUID_WAY_SHIFT		18
#define L2MERRSR_LO_INDEX_MASK			AL_FIELD_MASK(17, 0)
#define L2MERRSR_LO_INDEX_SHIFT			0

/* definition currently missing from nb_regs */
#define AL_NB_ACF_MISC_READ_BYPASS (1 << 30)

#define UINT32_OFFSET(base, offset) (uint32_t *)((uint8_t *)(base) + (offset))

void al_gic_cpu_sleepnotify(void)
{
#if defined(AL_DEV_ID) && (AL_DEV_ID >= AL_DEV_ID_ALPINE_V2)
	/* signal to redist. CPU is asleep - only for main gic*/
	uint32_t __iomem *gicr_waker = UINT32_OFFSET((uintptr_t)AL_NB_GIC_REDIST_BASE(
					NB_GIC_MAIN, al_get_cpu_id()),
					GICR_WAKER);
	al_reg_write32(gicr_waker, GICR_WAKER_PROCESSOR_SLEEP);
	while (!(al_reg_read32(gicr_waker) & GICR_WAKER_CHILDREN_ASLEEP))
		;
#endif
}

/*******************************************************************************
 ** API Implementations
 ******************************************************************************/

/* initialization of different units */
void al_nbservice_init(void __iomem *nb_regs_address,
		al_bool dev_ord_relax)
{
	struct al_nb_regs __iomem *nb_regs = (struct al_nb_regs *)nb_regs_address;

	/* allow reads to bypass writes to different addresses */
	al_reg_write32_masked(
			&(nb_regs->global.acf_misc),
			AL_NB_ACF_MISC_READ_BYPASS,
			(dev_ord_relax) ? AL_NB_ACF_MISC_READ_BYPASS : 0);
}

void al_ccu_cluster_init(void __iomem *ccu_address, al_bool iocc, unsigned int cluster)
{
	struct al_ccu_regs __iomem *regs = (struct al_ccu_regs __iomem *)ccu_address;

	al_assert(cluster < AL_NB_ANPA_NUM_CLUSTERS);

	/* enable snoop */
	if (iocc) {
		unsigned int slave_num = AL_CCU_SLAVE_IDX_CLUSTER(cluster);
		uint32_t *snoop_control_reg = &regs->slaves[slave_num].snoop_control_register;

		al_reg_write32(
			snoop_control_reg,
			CCU_SNOOP_CONTROL_REGISTER_SX_ENABLE_SNOOP_REQUESTS |
			((AL_NB_ANPA_NUM_CLUSTERS > 1) ?
			CCU_SNOOP_CONTROL_REGISTER_SX_ENABLE_DVMS : 0));

		/* Wait for previous write to complete - to avoid potential
		 * snoop usage before activation of its controls
		 */
		al_reg_read32(snoop_control_reg);
		al_data_memory_barrier();
	}

	/* disable speculative fetches from masters */
#if (AL_DEV_ID == AL_DEV_ID_ALPINE_V1) || (AL_DEV_ID == AL_DEV_ID_ALPINE_V2)
	al_reg_write32(&regs->speculation_ctrl_register_v1_v2, 7);
#endif
}

void al_ccu_init(void __iomem *ccu_address, al_bool iocc)
{
	int i;

	for (i = 0; i < AL_NB_ANPA_NUM_CLUSTERS; i++)
		al_ccu_cluster_init(ccu_address, iocc, i);
}

void al_nbservice_clear_settings(void __iomem *nb_regs_address)
{
	struct al_nb_regs __iomem *nb_regs = (struct al_nb_regs *)nb_regs_address;

	al_reg_write32_masked(
		&(nb_regs->global.acf_misc),
		AL_NB_ACF_MISC_READ_BYPASS,
		0);
}

void al_ccu_cluster_clear_settings(void __iomem *ccu_address, unsigned int cluster)
{
	struct al_ccu_regs __iomem *regs = (struct al_ccu_regs __iomem *)ccu_address;
	unsigned int slave_num = AL_CCU_SLAVE_IDX_CLUSTER(cluster);
	uint32_t *snoop_control_reg = &regs->slaves[slave_num].snoop_control_register;

	al_assert(cluster < AL_NB_ANPA_NUM_CLUSTERS);

	al_reg_write32(snoop_control_reg, 0);

	/* Wait for previous write to complete - to avoid potential
		* snoop usage before deactivation of its controls
		*/
	al_reg_read32(snoop_control_reg);
	al_data_memory_barrier();
}

void al_ccu_clear_settings(void __iomem *ccu_address)
{
#if (AL_DEV_ID == AL_DEV_ID_ALPINE_V1) || (AL_DEV_ID == AL_DEV_ID_ALPINE_V2)
	struct al_ccu_regs __iomem *regs = (struct al_ccu_regs __iomem *)ccu_address;
#endif
	int i;

	for (i = 0; i < AL_NB_ANPA_NUM_CLUSTERS; i++)
		al_ccu_cluster_clear_settings(ccu_address, i);

#if (AL_DEV_ID == AL_DEV_ID_ALPINE_V1) || (AL_DEV_ID == AL_DEV_ID_ALPINE_V2)
	al_reg_write32(&regs->speculation_ctrl_register_v1_v2, 0);
#endif
}

/*******************************************************************************
 ******************************************************************************/
void al_cpu_mem_err_syndrome_get(
	struct al_cpu_mem_err_syndrome *syndrome)
{
	uint32_t vall = 0;
	uint32_t valh = 0;
#if defined(__aarch64__)
	uint64_t val = 0;
#endif

	al_assert(syndrome);

#ifdef __arm__
#if (AL_DEV_ID == AL_DEV_ID_ALPINE_V1)
	asm volatile("mrrc p15, 0, %0, %1, c15" : "=r" (vall), "=r" (valh));
#else
	asm volatile("mrrc p15, 2, %0, %1, c15" : "=r" (vall), "=r" (valh));
#endif
#elif defined(__aarch64__)
	val = al_sysreg_read(CPUMERRSR_EL1);
	valh = (uint32_t)(val >> 32);
	vall = (uint32_t)(val & 0xffffffff);
#endif

	syndrome->fatal = AL_REG_BIT_GET(
		valh,
		CPUMERRSR_HI_FATAL_SHIFT);
	syndrome->err_cnt_repeat = AL_REG_FIELD_GET(
		valh,
		CPUMERRSR_HI_ERR_CNT_OTHER_MASK,
		CPUMERRSR_HI_ERR_CNT_OTHER_SHIFT);
	syndrome->err_cnt_other = AL_REG_FIELD_GET(
		valh,
		CPUMERRSR_HI_ERR_CNT_REPEAT_MASK,
		CPUMERRSR_HI_ERR_CNT_REPEAT_SHIFT);
	syndrome->valid = AL_REG_BIT_GET(
		vall,
		CPUMERRSR_LO_VALID_SHIFT);
	syndrome->ram_id = AL_REG_FIELD_GET(
		vall,
		CPUMERRSR_LO_RAM_ID_MASK,
		CPUMERRSR_LO_RAM_ID_SHIFT);
	syndrome->bank_way = AL_REG_FIELD_GET(
		vall,
		CPUMERRSR_LO_BANK_WAY_MASK,
		CPUMERRSR_LO_BANK_WAY_SHIFT);
	syndrome->index = AL_REG_FIELD_GET(
		vall,
		CPUMERRSR_LO_INDEX_MASK,
		CPUMERRSR_LO_INDEX_SHIFT);
}

/*******************************************************************************
 ******************************************************************************/
void al_cpu_mem_err_syndrome_clear(
	void)
{
#ifdef __arm__
	uint32_t vall = 0;
	uint32_t valh = 0;

#if (AL_DEV_ID == AL_DEV_ID_ALPINE_V1)
	asm volatile("mcrr p15, 0, %0, %1, c15" : "=r" (vall), "=r" (valh));
#else
	asm volatile("mcrr p15, 2, %0, %1, c15" : "=r" (vall), "=r" (valh));
#endif
#elif defined(__aarch64__)
	al_sysreg_write(CPUMERRSR_EL1, 0);
#endif
}

/*******************************************************************************
 ******************************************************************************/
void al_l2_mem_err_syndrome_get(
	struct al_l2_mem_err_syndrome *syndrome)
{
	uint32_t vall = 0;
	uint32_t valh = 0;
#if defined(__aarch64__)
	uint64_t val = 0;
#endif

	al_assert(syndrome);

#ifdef __arm__
#if (AL_DEV_ID == AL_DEV_ID_ALPINE_V1)
	asm volatile("mrrc p15, 1, %0, %1, c15" : "=r" (vall), "=r" (valh));
#else
	asm volatile("mrrc p15, 3, %0, %1, c15" : "=r" (vall), "=r" (valh));
#endif
#elif defined(__aarch64__)
	val = al_sysreg_read(L2MERRSR_EL1);
	valh = (uint32_t)(val >> 32);
	vall = (uint32_t)(val & 0xffffffff);
#endif

	syndrome->fatal = AL_REG_BIT_GET(
		valh,
		L2MERRSR_HI_FATAL_SHIFT);
	syndrome->err_cnt_repeat = AL_REG_FIELD_GET(
		valh,
		L2MERRSR_HI_ERR_CNT_OTHER_MASK,
		L2MERRSR_HI_ERR_CNT_OTHER_SHIFT);
	syndrome->err_cnt_other = AL_REG_FIELD_GET(
		valh,
		L2MERRSR_HI_ERR_CNT_REPEAT_MASK,
		L2MERRSR_HI_ERR_CNT_REPEAT_SHIFT);
	syndrome->valid = AL_REG_BIT_GET(
		vall,
		L2MERRSR_LO_VALID_SHIFT);
	syndrome->ram_id = AL_REG_FIELD_GET(
		vall,
		L2MERRSR_LO_RAM_ID_MASK,
		L2MERRSR_LO_RAM_ID_SHIFT);
	syndrome->cpuid_way = AL_REG_FIELD_GET(
		vall,
		L2MERRSR_LO_CPUID_WAY_MASK,
		L2MERRSR_LO_CPUID_WAY_SHIFT);
	syndrome->index = AL_REG_FIELD_GET(
		vall,
		L2MERRSR_LO_INDEX_MASK,
		L2MERRSR_LO_INDEX_SHIFT);
}

/*******************************************************************************
 ******************************************************************************/
void al_l2_mem_err_syndrome_clear(
	void)
{
#ifdef __arm__
	uint32_t vall = 0;
	uint32_t valh = 0;

#if (AL_DEV_ID == AL_DEV_ID_ALPINE_V1)
	asm volatile("mcrr p15, 1, %0, %1, c15" : "=r" (vall), "=r" (valh));
#else
	asm volatile("mcrr p15, 3, %0, %1, c15" : "=r" (vall), "=r" (valh));
#endif
#elif defined(__aarch64__)
	al_sysreg_write(L2MERRSR_EL1, 0);
#endif
}

#endif

