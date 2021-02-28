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

#include <al_hal_common.h>
#include <al_hal_reg_utils.h>
#include <al_hal_nb_regs.h>
#include <al_init_sys_fabric.h>
#include "al_init_ccu_regs.h"
#include "al_init_smmu_regs.h"
#include "al_init_gicv2_regs.h"
#include "al_init_gicv3_regs.h"
#include "al_init_sys_fabric_offsets.h"
#include "al_hal_iomap.h"

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

/*******************************************************************************
 ** Internal functions
 ******************************************************************************/

/* secure initialization of different units.
 * Must be called from a secure context, to allow non-secure access
 * to the unit
 */

static void al_ccu_security_init(void __iomem *ccu_address)
{
	/*enable non-secure access to CCU*/
	al_reg_write32(UINT32_OFFSET(ccu_address, AL_CCU_SECURE_ACCESS_OFFSET), 1);
}

#if (AL_DEV_ID == AL_DEV_ID_ALPINE_V1)
static void al_smmu_security_init(void __iomem *smmu_address)
{
	/* convert all secure transactions to non-secure
	 * Addressing RMN: 804
	 */
	al_reg_write32(UINT32_OFFSET(smmu_address, SMMU_CFG_R0_OFFSET),
			SMMU_CFG_R0_BYPASS | SMMU_CFG_R0_NON_SECURE);
	/* assign all SMMU resources to non-secure translation */
	al_reg_write32(UINT32_OFFSET(smmu_address, SMMU_CFG_R1_OFFSET),
			SMMU_CFG_R1_NSNUMCBO_MASK | SMMU_CFG_R1_NSNUMSMRGO_MASK
			| SMMU_CFG_R1_NSNUMIRPTO_MASK);
}
#endif

static void al_gic_security_init(enum al_nb_gic_id gic)
{
	int irq, irq_num;
	uint32_t temp;

	temp = al_reg_read32(UINT32_OFFSET((uintptr_t)AL_NB_GIC_DIST_BASE(gic), GIC_DIST_CTR));
	irq_num = ((temp & 0x1f) + 1) * 32;
	/* set non-private interrupts to group 1. 1 bit per interrupt */
	for (irq = 32; irq < irq_num; irq += 32) {
		al_reg_write32(UINT32_OFFSET((uintptr_t)AL_NB_GIC_DIST_BASE(gic),
				GIC_DIST_IGROUPR + irq/8),
				0xffffffff);
	}

	/* set non-secure priority. 8 bit per interrupt */
	for (irq = 32; irq < irq_num; irq += 4) {
		al_reg_write32(UINT32_OFFSET((uintptr_t)AL_NB_GIC_DIST_BASE(gic),
				GIC_DIST_PRI + irq),
				0x80808080);
	}

	/* Main Distributor */
#if (CONFIG_GIC500_ARE == 1)
	/* Turn on EnableGrp0 and ARE_S */
	temp = 0x11;
#else
	/* Turn on EnableGrp0 */
	temp = 0x1;
#endif

	al_reg_write32_masked(UINT32_OFFSET((uintptr_t)AL_NB_GIC_DIST_BASE(gic),
			GIC_DIST_CTRL),
			temp, temp);

}

static void al_gic_percpu_security_init(enum al_nb_gic_id gic, int enable_secure_interrupts)
{
	int irq;
	uint32_t temp;
	uint8_t __iomem *private_dist_base;
#if (CONFIG_GIC500_ARE == 1)
	int cpu_id;

	if (gic == NB_GIC_MAIN) {
		cpu_id = al_get_cpu_id();
		private_dist_base =
			(uint8_t __iomem *)(uintptr_t)AL_NB_GIC_REDIST_SGI_BASE(gic, cpu_id);
	} else
#endif
	{
		private_dist_base = (uint8_t __iomem *)(uintptr_t)AL_NB_GIC_DIST_BASE(gic);
	}

	/* set private interrupts to group 1 */
	al_reg_write32(UINT32_OFFSET(private_dist_base, GIC_DIST_IGROUPR), 0xffffffff);

	/* set non-secure priority. 8 bit per interrupt */
	for (irq = 0; irq < 32; irq += 4) {
		al_reg_write32(UINT32_OFFSET(private_dist_base, GIC_DIST_PRI + irq), 0x80808080);
	}

	/* accept all priorities (accept non-secure priorities) */
	al_reg_write32(UINT32_OFFSET((uintptr_t)AL_NB_GIC_CPU_BASE(gic), GIC_CPU_PRIMASK),
				0xff);

	/* CPU interface Control Register-
	 * bit	val	meaning
	 * 10	p	EOIR for non-secure
	 * 9	0	EOIR does both  priority drop and int disable for secure
	 * 7:8	p	bypass for insecure
	 * 6	1	don't use bypass-irq for secure
	 * 5	1	don't use bypass-fiq for secure
	 * 4	1	cbpr secure+non-secure together
	 * 3	1	signal secure interrupts via fiqs
	 * 2	0	dont ack non-secure interrupts from secure world
	 * 1	p	enable grp 1
	 * 0	?	enable grp 0
	 * */
	temp = 0x78;
	if (enable_secure_interrupts)
		temp |= 1;

	al_reg_write32_masked(UINT32_OFFSET((uintptr_t)AL_NB_GIC_CPU_BASE(gic), GIC_CPU_CTRL),
		0x27d, temp);
}

static void al_gic_percpu_perwakeup_security_init(enum al_nb_gic_id gic
#if (AL_DEV_ID == AL_DEV_ID_ALPINE_V1)
	__attribute__((__unused__))
#endif
)
{
#if defined(AL_DEV_ID) && (AL_DEV_ID == AL_DEV_ID_ALPINE_V2)
	if (gic == NB_GIC_MAIN) {
		uint8_t __iomem *gic_redist_base =
			(uint8_t __iomem *)(uintptr_t)AL_NB_GIC_REDIST_BASE(gic, al_get_cpu_id());
		/* signal to redist. CPU is awake - only for main gic */
		al_reg_write32(UINT32_OFFSET(gic_redist_base, GICR_WAKER), 0);
		while (al_reg_read32(UINT32_OFFSET(gic_redist_base, GICR_WAKER))
				& GICR_WAKER_CHILDREN_ASLEEP)
			;

		/* accept all priorities (accept non-secure priorities) */
		al_reg_write32(UINT32_OFFSET((uintptr_t)AL_NB_GIC_CPU_BASE(gic), GIC_CPU_PRIMASK),
					0xff);
	}
#endif
}

void al_gic_cpu_sleepnotify(void)
{
#if defined(AL_DEV_ID) && (AL_DEV_ID == AL_DEV_ID_ALPINE_V2)
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

void al_ccu_init(void __iomem *ccu_address, al_bool iocc)
{
	/* enable snoop */
	if (iocc) {
		al_reg_write32(UINT32_OFFSET(ccu_address, AL_CCU_SNOOP_CONTROL_IOFAB_0_OFFSET), 1);
		al_reg_write32(UINT32_OFFSET(ccu_address, AL_CCU_SNOOP_CONTROL_IOFAB_1_OFFSET), 1);
	}
	/* disable speculative fetches from masters */
	al_reg_write32(UINT32_OFFSET(ccu_address, AL_CCU_SPECULATION_CONTROL_OFFSET), 7);
}

void al_ccu_cluster_init(void __iomem *ccu_address,
			 al_bool iocc,
			 unsigned int cluster)
{
	al_assert(cluster == 0);
	al_ccu_init(ccu_address, iocc);
}

void al_nbservice_clear_settings(void __iomem *nb_regs_address)
{
	struct al_nb_regs __iomem *nb_regs = (struct al_nb_regs *)nb_regs_address;

	al_reg_write32_masked(
		&(nb_regs->global.acf_misc),
		AL_NB_ACF_MISC_READ_BYPASS,
		0);
}

void al_ccu_clear_settings(void __iomem *ccu_address)
{
	al_reg_write32(UINT32_OFFSET(ccu_address, AL_CCU_SNOOP_CONTROL_IOFAB_0_OFFSET), 0);
	al_reg_write32(UINT32_OFFSET(ccu_address, AL_CCU_SNOOP_CONTROL_IOFAB_1_OFFSET), 0);
	al_reg_write32(UINT32_OFFSET(ccu_address, AL_CCU_SPECULATION_CONTROL_OFFSET), 0);
}

void al_sys_fabric_security_init(void __iomem *nb_base_address)
{
	al_ccu_security_init(UINT32_OFFSET(nb_base_address, AL_NB_CCU_OFFSET));
#if (AL_DEV_ID == AL_DEV_ID_ALPINE_V1)
	al_smmu_security_init(UINT32_OFFSET(nb_base_address, AL_NB_SMMU0_OFFSET));
	al_smmu_security_init(UINT32_OFFSET(nb_base_address, AL_NB_SMMU1_OFFSET));
#endif
#if (AL_DEV_ID == AL_DEV_ID_ALPINE_V2)
	al_gic_security_init(
			NB_GIC_MAIN);
#endif
	al_gic_security_init(
			NB_GIC_SECONDARY);
}

void al_sys_fabric_cpu_cluster_security_init(void)
{
#if (AL_DEV_ID == AL_DEV_ID_ALPINE_V1)
	al_gic_security_init(
			NB_GIC_MAIN);
#endif
}

void al_sys_fabric_percpu_security_init(void)
{
	al_gic_percpu_security_init(
			NB_GIC_MAIN, 1);
	al_gic_percpu_security_init(
			NB_GIC_SECONDARY, 0);
}

void al_sys_fabric_percpu_perwakeup_security_init(void)
{
	al_gic_percpu_perwakeup_security_init(
			NB_GIC_MAIN);
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
	asm volatile("MRS %0, S3_1_c15_c2_2" : "=r" (val));

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
	uint64_t val = 0;

	asm volatile("MSR S3_1_c15_c2_2, %0" : "=r" (val));
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
	asm volatile("MRS %0, S3_1_c15_c2_3" : "=r" (val));

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
	uint64_t val = 0;

	asm volatile("MSR S3_1_c15_c2_3, %0" : "=r" (val));
#endif
}

