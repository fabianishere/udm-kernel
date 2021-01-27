#include "al_hal_sys_fabric_utils.h"
#include "al_hal_anpa_regs.h"
#include "al_hal_iofic.h"
#include "al_hal_iofic_regs.h"
#include "al_hal_nb_regs_v3.h"

#if (!defined(AL_DEV_ID)) || (AL_DEV_ID >= AL_DEV_ID_ALPINE_V3)

#if (defined(AL_DEV_ID))
#define UTILS_STATIC
#else
#define UTILS_STATIC	static
#endif

#define NUM_CORES_PER_CLUSTER		4
#define NUM_SEMAPHORES			64

#define ANPA_GLOBAL_CPUS_SOFTWARE_RESET_LEVEL_CPU_PORESET \
		(0x1 << ANPA_GLOBAL_CPUS_SOFTWARE_RESET_LEVEL_SHIFT)

#if (defined(AL_DEV_ID) && (AL_DEV_ID == AL_DEV_ID_ALPINE_V3)) && \
		(defined(AL_DEV_REV_ID) && (AL_DEV_REV_ID == 0))
#define ALPINE_V3_TEST_CHIP 1
#else
#define ALPINE_V3_TEST_CHIP 0
#endif

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_handle_init(
	struct al_sys_fabric_handle	*handle,
	void __iomem			*nb_regs_base)
{
	struct al_nb_regs __iomem *nb_regs =
		(struct al_nb_regs __iomem *)nb_regs_base;

	al_assert(handle);

	handle->ver = (al_reg_read32(&nb_regs->nb_version.version) &
		NB_NB_VERSION_VERSION_RELEASE_NUM_MAJOR_MASK) >>
		NB_NB_VERSION_VERSION_RELEASE_NUM_MAJOR_SHIFT;

	handle->nb_regs_base = nb_regs_base;
#if (ALPINE_V3_TEST_CHIP == 0)
	handle->error_sfic_base = &nb_regs->error_intc;
	handle->nmi_sfic_base = &nb_regs->nmi_intc;
#endif
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_cluster_handle_init(
	struct al_sys_fabric_cluster_handle	*handle,
	const struct al_sys_fabric_handle	*fabric_handle,
	void __iomem				*anpa_regs_base)
{
	al_assert(handle);
	al_assert(fabric_handle);
	al_assert(
		(fabric_handle->ver >= NB_NB_VERSION_VERSION_RELEASE_NUM_MAJOR_VAL_ALPINE_V3) &&
		(anpa_regs_base));

	handle->fabric_handle = fabric_handle;
	handle->anpa_regs_base = anpa_regs_base;
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_int_local_unmask(
	const struct al_sys_fabric_handle	*handle,
	unsigned int			idx __attribute__((unused)),
	unsigned int			mask __attribute__((unused)))
{
	al_assert(handle);

	al_err("%s: not supported!\n", __func__);
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_int_local_mask(
	const struct al_sys_fabric_handle	*handle,
	unsigned int			idx __attribute__((unused)),
	unsigned int			mask __attribute__((unused)))
{
	al_assert(handle);

	al_err("%s: not supported!\n", __func__);
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_int_clear(
	const struct al_sys_fabric_handle	*handle,
	unsigned int			mask __attribute__((unused)))
{
	al_assert(handle);

	al_err("%s: not supported!\n", __func__);
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_int_cause_read(
	const struct al_sys_fabric_handle	*handle,
	uint32_t *cause)
{
	al_assert(handle);
	al_assert(cause);

	al_err("%s: not supported!\n", __func__);
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_error_cause_read(
	const struct al_sys_fabric_handle	*handle,
	uint32_t *cause)
{
	al_assert(handle);
	al_assert(cause);

	al_err("%s: not supported!\n", __func__);
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_error_int_unmask(
	const struct al_sys_fabric_handle	*handle,
	unsigned int			mask __attribute__((unused)))
{
	al_assert(handle);

	al_err("%s: not supported!\n", __func__);
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_error_int_mask(
	const struct al_sys_fabric_handle	*handle,
	unsigned int			mask __attribute__((unused)))
{
	al_assert(handle);

	al_err("%s: not supported!\n", __func__);
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_error_int_clear(
	const struct al_sys_fabric_handle	*handle,
	unsigned int			mask __attribute__((unused)))
{
	al_assert(handle);

	al_err("%s: not supported!\n", __func__);
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_sb_pos_info_get_and_clear(
	const struct al_sys_fabric_handle		*handle,
	struct al_sys_fabric_sb_pos_error_info	*pos_info)
{
	struct al_nb_regs __iomem *nb_regs;
	uint32_t error_log_1;

	al_assert(handle);
	al_assert(pos_info);

	nb_regs = (struct al_nb_regs __iomem *)handle->nb_regs_base;

	/* SB PoS error low address bits */
	pos_info->address = al_reg_read32(&nb_regs->global.sb_pos_error_log_0);

	/* this clears the interrupt */
	error_log_1 = al_reg_read32(&nb_regs->global.sb_pos_error_log_1);

	/*
	 * Addressing RMN: 10628
	 *
	 * RMN description:
	 * on V2 there is a HW synchronization bug, which
	 * Nondeterministically results in interrupt not being
	 * cleared after reading the log_1 register
	 *
	 * This is applicable to Alpine V3 as well
	 *
	 * Software flow:
	 * read log_1 multiple times until interrupt is cleared
	 */
	if (handle->ver == NB_NB_VERSION_VERSION_RELEASE_NUM_MAJOR_VAL_ALPINE_V3) {
		int i;
		uint32_t nb_int_cause;

		for (i = 0; i < 1000; i++) {
			nb_int_cause = al_iofic_read_cause(handle->error_sfic_base, AL_INT_GROUP_A);
			if (!(nb_int_cause & AL_SYS_FABRIC_SFIC_ERR_GROUP_A_FABRIC_POS_ERR_IRQ))
				break;

			al_reg_read32(&nb_regs->global.sb_pos_error_log_1);
			al_iofic_clear_cause(handle->error_sfic_base, AL_INT_GROUP_A,
				AL_SYS_FABRIC_SFIC_ERR_GROUP_A_FABRIC_POS_ERR_IRQ);
		}

		if (i == 1000)
			al_err("%s: tried to clear SB PoS error %d times, giving up...\n",
				__func__,
				i);
	}

	/* Error Log 1
	 * [7:0] address_high
	 * [16:8] request id
	 * [18:17] bresp
	 */
	pos_info->address |= ((uint64_t)(error_log_1 & AL_FIELD_MASK(7, 0)) << 32);
	pos_info->request_id = (error_log_1 & AL_FIELD_MASK(16, 8)) >> 8;
	pos_info->bresp = (error_log_1 & AL_FIELD_MASK(18, 17)) >> 17;
	pos_info->valid = (error_log_1 & NB_GLOBAL_SB_POS_ERROR_LOG_1_VALID) ? AL_TRUE : AL_FALSE;

	/*
	 * clear valid bit so that
	 * Subsequent errors will be captured
	 * and information will be valid
	 */
	if (pos_info->valid)
		al_reg_write32(&nb_regs->global.sb_pos_error_log_1, 0);
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_core_msg_set(
	const struct al_sys_fabric_cluster_handle	*handle,
	unsigned int				core,
	unsigned int				msg,
	al_bool					concat_reg_addr)
{
	struct al_anpa_regs __iomem *anpa_regs;
	uint32_t *msg_reg;
	uintptr_t address;

	al_assert(handle);
	al_assert(core < NUM_CORES_PER_CLUSTER);

	anpa_regs = (struct al_anpa_regs __iomem *)handle->anpa_regs_base;
	msg_reg = &anpa_regs->per_core[core].msg_out;

	if (concat_reg_addr) {
		address = (uintptr_t)msg_reg;
		al_assert((msg & 0xffff) == msg);
		msg |= ((uint32_t)address << 16);
	}

	al_reg_write32(msg_reg, msg);
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_core_msg_get(
	const struct al_sys_fabric_cluster_handle	*handle,
	unsigned int				core,
	unsigned int				*msg_valid,
	unsigned int				*msg)
{
	struct al_anpa_regs __iomem *anpa_regs;
	unsigned int val;

	al_assert(handle);
	al_assert(core < NUM_CORES_PER_CLUSTER);

	anpa_regs = (struct al_anpa_regs __iomem *)handle->anpa_regs_base;
	val = al_reg_read32(&anpa_regs->per_core[core].msg_in);

	*msg_valid = !!(val & ANPA_PER_CORE_MSG_IN_VALID);
	*msg = (val & ANPA_PER_CORE_MSG_IN_DATA_MASK) >> ANPA_PER_CORE_MSG_IN_DATA_SHIFT;
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC unsigned int _al_sys_fabric_sys_counter_freq_get(
	const struct al_sys_fabric_handle	*handle)
{
	struct al_nb_regs __iomem *nb_regs = (struct al_nb_regs __iomem *)handle->nb_regs_base;

	al_assert(handle);

	return al_reg_read32(&nb_regs->system_counter.cnt_base_freq);
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC uint64_t _al_sys_fabric_sys_counter_get_64(
	const struct al_sys_fabric_handle	*handle)
{
	struct al_nb_regs __iomem *nb_regs = (struct al_nb_regs __iomem *)handle->nb_regs_base;
	uint32_t timer_low;
	uint32_t timer_high_presample;
	uint32_t timer_high_postsample;
	uint64_t timer;

	al_assert(handle);

	do {
		timer_high_presample = al_reg_read32(&nb_regs->system_counter.cnt_high);
		timer_low = al_reg_read32(&nb_regs->system_counter.cnt_low);
		timer_high_postsample = al_reg_read32(&nb_regs->system_counter.cnt_high);
	/* if cnt_high changed during sampling, re-sample the value */
	} while (timer_high_presample != timer_high_postsample);

	timer = (((uint64_t)timer_high_postsample << 32) | (uint64_t)timer_low);

	return timer;
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC uint32_t _al_sys_fabric_sys_counter_get_32(
	const struct al_sys_fabric_handle	*handle)
{
	struct al_nb_regs __iomem *nb_regs = (struct al_nb_regs __iomem *)handle->nb_regs_base;

	al_assert(handle);

	return al_reg_read32(&nb_regs->system_counter.cnt_low);
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC int _al_sys_fabric_hw_semaphore_lock(
	const struct al_sys_fabric_handle	*handle,
	unsigned int			id)
{
	struct al_nb_regs __iomem *nb_regs = (struct al_nb_regs __iomem *)handle->nb_regs_base;
	struct al_nb_semaphores *regs = &nb_regs->semaphores[id];
	uint32_t val;

	al_assert(handle);
	al_assert(id < NUM_SEMAPHORES);

	val = al_reg_read32(&regs->lockn);
	/* ensure memory barrier */
	if (val)
		return -EIO;

	return 0;
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_hw_semaphore_unlock(
	const struct al_sys_fabric_handle	*handle,
	unsigned int			id)
{
	struct al_nb_regs __iomem *nb_regs = (struct al_nb_regs __iomem *)handle->nb_regs_base;
	struct al_nb_semaphores *regs = &nb_regs->semaphores[id];

	al_assert(handle);
	al_assert(id < NUM_SEMAPHORES);

	al_reg_write32(&regs->lockn, 0);
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_cluster_pd_pu_timer_set(
	const struct al_sys_fabric_cluster_handle	*handle,
	unsigned int				pd,
	unsigned int				pu)
{
	struct al_anpa_regs __iomem *anpa_regs;

	al_assert(handle);

	anpa_regs = (struct al_anpa_regs __iomem *)handle->anpa_regs_base;

	al_reg_write32(&anpa_regs->global.core_max_pd_timer, pd);
	al_reg_write32(&anpa_regs->global.core_max_pu_timer, pu);
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_cluster_power_ctrl(
	const struct al_sys_fabric_cluster_handle	*handle,
	unsigned int				ctrl)
{
	struct al_anpa_regs __iomem *anpa_regs;

	al_assert(handle);

	anpa_regs = (struct al_anpa_regs __iomem *)handle->anpa_regs_base;

	al_reg_write32(&anpa_regs->global.cpus_power_ctrl, ctrl);
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_core_power_ctrl(
	const struct al_sys_fabric_cluster_handle	*handle,
	unsigned int				core,
	unsigned int				ctrl)
{
	struct al_anpa_regs __iomem *anpa_regs;

	al_assert(handle);
	al_assert(core < NUM_CORES_PER_CLUSTER);

	anpa_regs = (struct al_anpa_regs __iomem *)handle->anpa_regs_base;

	al_reg_write32(&anpa_regs->per_core[core].power_ctrl, ctrl);
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_cluster_sw_reset(
	const struct al_sys_fabric_cluster_handle	*handle)
{
	struct al_anpa_regs __iomem *anpa_regs;

	al_assert(handle);

	anpa_regs = (struct al_anpa_regs __iomem *)handle->anpa_regs_base;

	al_reg_write32(
		&anpa_regs->global.cpus_software_reset,
		(3 << ANPA_GLOBAL_CPUS_SOFTWARE_RESET_LEVEL_SHIFT) |
		ANPA_GLOBAL_CPUS_SOFTWARE_RESET_SWRESET_REQ);
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_core_sw_reset(
	const struct al_sys_fabric_cluster_handle	*handle,
	unsigned int				core)
{
	struct al_anpa_regs __iomem *anpa_regs;

	al_assert(handle);
	al_assert(core < NUM_CORES_PER_CLUSTER);

	anpa_regs = (struct al_anpa_regs __iomem *)handle->anpa_regs_base;

	al_reg_write32(
		&anpa_regs->global.cpus_software_reset,
		(1 << (ANPA_GLOBAL_CPUS_SOFTWARE_RESET_CORES_SHIFT + core)) |
		ANPA_GLOBAL_CPUS_SOFTWARE_RESET_SWRESET_REQ);
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_core_power_on_reset(
	const struct al_sys_fabric_cluster_handle	*handle,
	unsigned int				core)
{
	struct al_anpa_regs __iomem *anpa_regs;

	al_assert(handle);
	al_assert(core < NUM_CORES_PER_CLUSTER);

	anpa_regs = (struct al_anpa_regs __iomem *)handle->anpa_regs_base;

	al_reg_write32(
		&anpa_regs->global.cpus_software_reset,
		(1 << (ANPA_GLOBAL_CPUS_SOFTWARE_RESET_CORES_SHIFT + core)) |
		ANPA_GLOBAL_CPUS_SOFTWARE_RESET_LEVEL_CPU_PORESET |
		ANPA_GLOBAL_CPUS_SOFTWARE_RESET_SWRESET_REQ);
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_core_reset_deassert(
	const struct al_sys_fabric_cluster_handle	*handle,
	unsigned int				core)
{
	struct al_anpa_regs __iomem *anpa_regs;

	al_assert(handle);
	al_assert(core < NUM_CORES_PER_CLUSTER);

	anpa_regs = (struct al_anpa_regs __iomem *)handle->anpa_regs_base;

	al_reg_write32_masked(
		&anpa_regs->global.cpus_init_control, AL_BIT(core), AL_BIT(core));
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_core_aarch64_setup(
	const struct al_sys_fabric_cluster_handle	*handle,
	unsigned int				core,
	uint32_t				entry_high,
	uint32_t				entry_low)
{
	struct al_anpa_regs __iomem *anpa_regs;

	al_assert(handle);
	al_assert(core < NUM_CORES_PER_CLUSTER);
	al_assert(entry_high || entry_low);

	anpa_regs = (struct al_anpa_regs __iomem *)handle->anpa_regs_base;

	al_reg_write32(
		&anpa_regs->per_core[core].rvbar_low,
		entry_low);
	al_reg_write32(
		&anpa_regs->per_core[core].rvbar_high,
		entry_high);
	al_reg_write32(
		&anpa_regs->per_core[core].aarch64,
		ANPA_PER_CORE_AARCH64_AA64_NAA32);
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_core_aarch32_setup(
	const struct al_sys_fabric_cluster_handle	*handle,
	unsigned int				core,
	uint32_t				entry_high,
	uint32_t				entry_low)
{
	struct al_anpa_regs __iomem *anpa_regs;

	al_assert(handle);
	al_assert(core < NUM_CORES_PER_CLUSTER);
	al_assert(entry_high || entry_low);

	anpa_regs = (struct al_anpa_regs __iomem *)handle->anpa_regs_base;

	al_reg_write32(
		&anpa_regs->per_core[core].resume_addr_l,
		entry_low);
	al_reg_write32(
		&anpa_regs->per_core[core].resume_addr_h,
		entry_high);
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_core_aarch32_setup_get(
	const struct al_sys_fabric_cluster_handle	*handle,
	unsigned int				core,
	uint32_t				*entry_high,
	uint32_t				*entry_low)
{
	struct al_anpa_regs __iomem *anpa_regs;

	al_assert(handle);
	al_assert(core < NUM_CORES_PER_CLUSTER);
	al_assert(entry_high || entry_low);

	anpa_regs = (struct al_anpa_regs __iomem *)handle->anpa_regs_base;

	*entry_low = al_reg_read32(&anpa_regs->per_core[core].resume_addr_l);
	*entry_high = al_reg_read32(&anpa_regs->per_core[core].resume_addr_h);
}

/**************************************************************************************************/
/**************************************************************************************************/
#if ((AL_DEV_ID == AL_DEV_ID_ALPINE_V3) && (AL_DEV_REV_ID == 0))
UTILS_STATIC struct al_iofic_regs __iomem *al_sys_fabric_iofic_regs_get(
	const struct al_sys_fabric_handle	*fabric_handle __attribute__((unused)),
	enum al_sys_fabric_iofic_id	id __attribute__((unused)))
{
	al_err("%s: not supported!\n", __func__);

	return NULL;
}
#else
UTILS_STATIC struct al_iofic_regs __iomem *_al_sys_fabric_iofic_regs_get(
	const struct al_sys_fabric_handle	*fabric_handle,
	enum al_sys_fabric_iofic_id	id)
{
	struct al_nb_regs __iomem *nb_regs;

	al_assert(fabric_handle);

	nb_regs = (struct al_nb_regs __iomem *)fabric_handle->nb_regs_base;

	switch (id) {
	case AL_SYS_FABRIC_IOFIC_ID_ERR:
		return (struct al_iofic_regs __iomem *)&nb_regs->error_intc[0];
	case AL_SYS_FABRIC_IOFIC_ID_PMU:
		return (struct al_iofic_regs __iomem *)&nb_regs->pmu_intc[0];
	case AL_SYS_FABRIC_IOFIC_ID_CORESIGHT:
		return (struct al_iofic_regs __iomem *)&nb_regs->coresight_intc[0];
	case AL_SYS_FABRIC_IOFIC_ID_IOMMU:
		return (struct al_iofic_regs __iomem *)&nb_regs->iommu_intc[0];
	case AL_SYS_FABRIC_IOFIC_ID_EXTERNAL:
		return (struct al_iofic_regs __iomem *)&nb_regs->external_intc_0[0];
	case AL_SYS_FABRIC_IOFIC_ID_NMI:
		return (struct al_iofic_regs __iomem *)&nb_regs->nmi_intc[0];
	}

	return NULL;
}
#endif

/**************************************************************************************************/
/**************************************************************************************************/
#if ((AL_DEV_ID == AL_DEV_ID_ALPINE_V3) && (AL_DEV_REV_ID == 0))
UTILS_STATIC void _al_sys_fabric_iocache_usage_cfg_set(
	const struct al_sys_fabric_handle		*fabric_handle __attribute__((unused)),
	const struct al_sys_fabric_iocache_usage_cfg	*cfg __attribute__((unused)))
{
	al_err("%s: not supported!\n", __func__);
}
#else
UTILS_STATIC void _al_sys_fabric_iocache_usage_cfg_set(
	const struct al_sys_fabric_handle		*fabric_handle,
	const struct al_sys_fabric_iocache_usage_cfg	*cfg)
{
	struct al_nb_regs __iomem *nb_regs;

	al_assert(fabric_handle);

	nb_regs = (struct al_nb_regs __iomem *)fabric_handle->nb_regs_base;

	al_reg_write32_masked(&nb_regs->address_map.config,
		NB_ADDRESS_MAP_CONFIG_SRAM_OVRD,
		(cfg->addr_map_sram_ovrd ? NB_ADDRESS_MAP_CONFIG_SRAM_OVRD : 0));

	al_reg_write32_masked(&nb_regs->acf.misc,
		NB_ACF_MISC_CPU_IOCACHE_PAD_EN |
		NB_ACF_MISC_CPU_IOCACHE_ALLOC_EN,
		(cfg->pad_en ? NB_ACF_MISC_CPU_IOCACHE_PAD_EN : 0) |
		(cfg->alloc_en ? NB_ACF_MISC_CPU_IOCACHE_ALLOC_EN : 0));

	al_reg_write32_masked(&nb_regs->acf.iocache,
		NB_ACF_IOCACHE_CPU_RD_MASK_MASK |
		NB_ACF_IOCACHE_CPU_RD_VALUE_MASK |
		NB_ACF_IOCACHE_CPU_WR_MASK_MASK |
		NB_ACF_IOCACHE_CPU_WR_VALUE_MASK,
		(cfg->rd_mask << NB_ACF_IOCACHE_CPU_RD_MASK_SHIFT) |
		(cfg->rd_value << NB_ACF_IOCACHE_CPU_RD_VALUE_SHIFT) |
		(cfg->wr_mask << NB_ACF_IOCACHE_CPU_WR_MASK_SHIFT) |
		(cfg->wr_value << NB_ACF_IOCACHE_CPU_WR_VALUE_SHIFT));
}
#endif

/**************************************************************************************************/
/**************************************************************************************************/
#define AL_SYS_FABRIC_SFIC_ERR_GROUP_A_SUM \
	(AL_SYS_FABRIC_SFIC_ERR_GROUP_A_CPU_AXIERRIRQ_N_0 | \
	AL_SYS_FABRIC_SFIC_ERR_GROUP_A_CPU_AXIERRIRQ_N_1 | \
	AL_SYS_FABRIC_SFIC_ERR_GROUP_A_CPU_AXIERRIRQ_N_2 | \
	AL_SYS_FABRIC_SFIC_ERR_GROUP_A_CPU_AXIERRIRQ_N_3 | \
	AL_SYS_FABRIC_SFIC_ERR_GROUP_A_CPU_RD_DATA_PAR_ERR_0 | \
	AL_SYS_FABRIC_SFIC_ERR_GROUP_A_CPU_RD_DATA_PAR_ERR_1 | \
	AL_SYS_FABRIC_SFIC_ERR_GROUP_A_CPU_RD_DATA_PAR_ERR_2 | \
	AL_SYS_FABRIC_SFIC_ERR_GROUP_A_CPU_RD_DATA_PAR_ERR_3 | \
	AL_SYS_FABRIC_SFIC_ERR_GROUP_A_CCI_ERRO_IRQ_N | \
	AL_SYS_FABRIC_SFIC_ERR_GROUP_A_CORESIGHT_TSCNT_OVFL_INTR | \
	AL_SYS_FABRIC_SFIC_ERR_GROUP_A_GIC_ECC_FATAL | \
	AL_SYS_FABRIC_SFIC_ERR_GROUP_A_FABRIC_MSIX_ERR_INTR | \
	AL_SYS_FABRIC_SFIC_ERR_GROUP_A_FABRIC_POS_ERR_IRQ | \
	AL_SYS_FABRIC_SFIC_ERR_GROUP_A_FABRIC_PP_SRAM_PARITY_ERR | \
	AL_SYS_FABRIC_SFIC_ERR_GROUP_A_FABRIC_C2SWB_PARITY_ERR | \
	AL_SYS_FABRIC_SFIC_ERR_GROUP_A_FABRIC_SB_ARREQ_PAR_ERR_0 | \
	AL_SYS_FABRIC_SFIC_ERR_GROUP_A_FABRIC_SB_ARREQ_PAR_ERR_1 | \
	AL_SYS_FABRIC_SFIC_ERR_GROUP_A_FABRIC_SB_ARREQ_PAR_ERR_2 | \
	AL_SYS_FABRIC_SFIC_ERR_GROUP_A_FABRIC_SB_ARREQ_PAR_ERR_3 | \
	AL_SYS_FABRIC_SFIC_ERR_GROUP_A_IO_MMU_PTW_RD_DATA_PAR_ERR | \
	AL_SYS_FABRIC_SFIC_ERR_GROUP_A_IO_CACHE_MEMORY_UNCORR_ECC_0 | \
	AL_SYS_FABRIC_SFIC_ERR_GROUP_A_IO_CACHE_MEMORY_UNCORR_ECC_1 | \
	AL_SYS_FABRIC_SFIC_ERR_GROUP_A_IO_CACHE_MEMORY_CORR_ECC_0 | \
	AL_SYS_FABRIC_SFIC_ERR_GROUP_A_IO_CACHE_MEMORY_CORR_ECC_1 | \
	AL_SYS_FABRIC_SFIC_ERR_GROUP_A_IO_CACHE_PARITY_ERROR_0 | \
	AL_SYS_FABRIC_SFIC_ERR_GROUP_A_IO_CACHE_PARITY_ERROR_1)

UTILS_STATIC void _al_sys_fabric_error_ints_mask_get(
	const struct al_sys_fabric_handle		*fabric_handle,
	uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d)
{
	al_assert(fabric_handle);
	al_assert(a);
	al_assert(b);
	al_assert(c);
	al_assert(d);

	*a = AL_SYS_FABRIC_SFIC_ERR_GROUP_A_SUM;
	*b = 0;
	*c = 0;
	*d = 0;
}

/**
 * The NMI SFIC has 2 groups of bits A & B
 * which are identical - the use case is for the SW to register 2 handlers
 * one is fatal (higher prio) & the 2nd only for error statistics collection
 */
#define AL_SYS_FABRIC_SFIC_NMI_GROUP_A_SUM \
	(AL_SYS_FABRIC_SFIC_NMI_GROUP_A_DDR_CHANNEL_0_UNCORRECTABLE_ECC | \
	AL_SYS_FABRIC_SFIC_NMI_GROUP_A_DDR_CHANNEL_0_DFI_ALERT_N_CRC | \
	AL_SYS_FABRIC_SFIC_NMI_GROUP_A_DDR_CHANNEL_0_MAX_ALERT_N_REACHED_CRC | \
	AL_SYS_FABRIC_SFIC_NMI_GROUP_A_DDR_CHANNEL_1_UNCORRECTABLE_ECC | \
	AL_SYS_FABRIC_SFIC_NMI_GROUP_A_DDR_CHANNEL_1_DFI_ALERT_N_CRC | \
	AL_SYS_FABRIC_SFIC_NMI_GROUP_A_DDR_CHANNEL_1_MAX_ALERT_N_REACHED_CRC | \
	AL_SYS_FABRIC_SFIC_NMI_GROUP_A_ERROR_SFIC_SUMMARY | \
	AL_SYS_FABRIC_SFIC_NMI_GROUP_A_CCI_SF_PARERR | \
	AL_SYS_FABRIC_SFIC_NMI_GROUP_A_DDR_CHANNEL_0_PAR_WDATA_OUT_ERR | \
	AL_SYS_FABRIC_SFIC_NMI_GROUP_A_DDR_CHANNEL_1_PAR_WDATA_OUT_ERR)

UTILS_STATIC void _al_sys_fabric_nmi_ints_mask_get(
	const struct al_sys_fabric_handle		*fabric_handle,
	uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d)
{
	al_assert(fabric_handle);
	al_assert(a);
	al_assert(b);
	al_assert(c);
	al_assert(d);

	*a = AL_SYS_FABRIC_SFIC_NMI_GROUP_A_SUM;
	*b = 0; /** Keep group B masked - currently no need for 2 handlers */
	*c = 0;
	*d = 0;
}

#if ALPINE_V3_TEST_CHIP
UTILS_STATIC void _al_sys_fabric_error_unmask(const struct al_sys_fabric_handle *fabric_handle)
{
	al_assert(fabric_handle);
}

UTILS_STATIC void _al_sys_fabric_nmi_unmask(const struct al_sys_fabric_handle *fabric_handle)
{
	al_assert(fabric_handle);
}

UTILS_STATIC void __iomem *_al_sys_fabric_nmi_sfic_regs_base_get(
	const struct al_sys_fabric_handle *handle)
{
	al_assert(handle);

	al_assert_msg(0, "%s: No SFIC support\n", __func__);

	return NULL;
}

UTILS_STATIC void __iomem *_al_sys_fabric_err_sfic_regs_base_get(
	const struct al_sys_fabric_handle *handle)
{
	al_assert(handle);

	al_assert_msg(0, "%s: No SFIC support\n", __func__);

	return NULL;
}
#else
UTILS_STATIC void _al_sys_fabric_error_unmask(const struct al_sys_fabric_handle *fabric_handle)
{
	uint32_t a, b, c, d;
	struct al_nb_regs __iomem *regs_base;

	al_assert(fabric_handle);

	regs_base = fabric_handle->nb_regs_base;

	_al_sys_fabric_error_ints_mask_get(fabric_handle, &a, &b, &c, &d);

	al_iofic_config(&regs_base->error_intc,
			AL_INT_GROUP_A,
			INT_CONTROL_GRP_MASK_MSI_X);

	/* unmask interrupts */
	al_iofic_unmask(&regs_base->error_intc, AL_INT_GROUP_A, a);
}

UTILS_STATIC void _al_sys_fabric_nmi_unmask(const struct al_sys_fabric_handle *fabric_handle)
{
	uint32_t a, b, c, d;
	struct al_nb_regs __iomem *regs_base;

	al_assert(fabric_handle);

	regs_base = fabric_handle->nb_regs_base;

	_al_sys_fabric_nmi_ints_mask_get(fabric_handle, &a, &b, &c, &d);

	al_iofic_config(&regs_base->nmi_intc,
			AL_INT_GROUP_A,
			INT_CONTROL_GRP_MASK_MSI_X);

	/* unmask interrupts */
	al_iofic_unmask(&regs_base->nmi_intc, AL_INT_GROUP_A, a);
}

UTILS_STATIC void __iomem *_al_sys_fabric_nmi_sfic_regs_base_get(
	const struct al_sys_fabric_handle *handle)
{
	al_assert(handle);

	return handle->nmi_sfic_base;
}

UTILS_STATIC void __iomem *_al_sys_fabric_err_sfic_regs_base_get(
	const struct al_sys_fabric_handle *handle)
{
	al_assert(handle);

	return handle->error_sfic_base;
}
#endif

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_qos_cfg_set(
	const struct al_sys_fabric_handle	*handle,
	const struct al_sys_fabric_qos_cfg	*cfg)
{
	al_assert(handle);
	al_assert(cfg);

	al_err("%s: Not implemented!\n", __func__);
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_qos_cfg_get(
	const struct al_sys_fabric_handle	*handle,
	struct al_sys_fabric_qos_cfg		*cfg)
{
	al_assert(handle);
	al_assert(cfg);

	al_err("%s: Not implemented!\n", __func__);
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_iof_2_iof_path_en(
	const struct al_sys_fabric_handle	*handle,
	al_bool					en)
{
	al_assert(handle);

	if (!en)
		al_err("%s: Not supported in Alpine V3!\n", __func__);
}

#endif

#if (!defined(AL_DEV_ID))
void al_hal_sys_fabric_utils_imp_v3(struct al_sys_fabric_imp *imp);

void al_hal_sys_fabric_utils_imp_v3(struct al_sys_fabric_imp *imp)
{
	al_memset(imp, 0, sizeof(struct al_sys_fabric_imp));

	imp->al_sys_fabric_handle_init = _al_sys_fabric_handle_init;
	imp->al_sys_fabric_cluster_handle_init = _al_sys_fabric_cluster_handle_init;
	imp->al_sys_fabric_int_local_unmask = _al_sys_fabric_int_local_unmask;
	imp->al_sys_fabric_int_local_mask = _al_sys_fabric_int_local_mask;
	imp->al_sys_fabric_int_clear = _al_sys_fabric_int_clear;
	imp->al_sys_fabric_int_cause_read = _al_sys_fabric_int_cause_read;
	imp->al_sys_fabric_error_cause_read = _al_sys_fabric_error_cause_read;
	imp->al_sys_fabric_error_int_unmask = _al_sys_fabric_error_int_unmask;
	imp->al_sys_fabric_error_int_mask = _al_sys_fabric_error_int_mask;
	imp->al_sys_fabric_error_int_clear = _al_sys_fabric_error_int_clear;
	imp->al_sys_fabric_sb_pos_info_get_and_clear = _al_sys_fabric_sb_pos_info_get_and_clear;
	imp->al_sys_fabric_core_msg_set = _al_sys_fabric_core_msg_set;
	imp->al_sys_fabric_core_msg_get = _al_sys_fabric_core_msg_get;
	imp->al_sys_fabric_sys_counter_freq_get = _al_sys_fabric_sys_counter_freq_get;
	imp->al_sys_fabric_sys_counter_get_64 = _al_sys_fabric_sys_counter_get_64;
	imp->al_sys_fabric_sys_counter_get_32 = _al_sys_fabric_sys_counter_get_32;
	imp->al_sys_fabric_hw_semaphore_lock = _al_sys_fabric_hw_semaphore_lock;
	imp->al_sys_fabric_hw_semaphore_unlock = _al_sys_fabric_hw_semaphore_unlock;
	imp->al_sys_fabric_cluster_pd_pu_timer_set = _al_sys_fabric_cluster_pd_pu_timer_set;
	imp->al_sys_fabric_cluster_power_ctrl = _al_sys_fabric_cluster_power_ctrl;
	imp->al_sys_fabric_core_power_ctrl = _al_sys_fabric_core_power_ctrl;
	imp->al_sys_fabric_cluster_sw_reset = _al_sys_fabric_cluster_sw_reset;
	imp->al_sys_fabric_core_sw_reset = _al_sys_fabric_core_sw_reset;
	imp->al_sys_fabric_core_power_on_reset = _al_sys_fabric_core_power_on_reset;
	imp->al_sys_fabric_core_reset_deassert = _al_sys_fabric_core_reset_deassert;
	imp->al_sys_fabric_core_aarch64_setup = _al_sys_fabric_core_aarch64_setup;
	imp->al_sys_fabric_core_aarch32_setup = _al_sys_fabric_core_aarch32_setup;
	imp->al_sys_fabric_core_aarch32_setup_get = _al_sys_fabric_core_aarch32_setup_get;
	imp->al_sys_fabric_iofic_regs_get = _al_sys_fabric_iofic_regs_get;
	imp->al_sys_fabric_iocache_usage_cfg_set = _al_sys_fabric_iocache_usage_cfg_set;
	imp->al_sys_fabric_error_ints_mask_get = _al_sys_fabric_error_ints_mask_get;
	imp->al_sys_fabric_error_unmask = _al_sys_fabric_error_unmask;
	imp->al_sys_fabric_nmi_ints_mask_get = _al_sys_fabric_nmi_ints_mask_get;
	imp->al_sys_fabric_nmi_unmask = _al_sys_fabric_nmi_unmask;
	imp->al_sys_fabric_nmi_sfic_regs_base_get = _al_sys_fabric_nmi_sfic_regs_base_get;
	imp->al_sys_fabric_err_sfic_regs_base_get = _al_sys_fabric_err_sfic_regs_base_get;
	imp->al_sys_fabric_qos_cfg_set = _al_sys_fabric_qos_cfg_set;
	imp->al_sys_fabric_qos_cfg_get = _al_sys_fabric_qos_cfg_get;
	imp->al_sys_fabric_iof_2_iof_path_en = _al_sys_fabric_iof_2_iof_path_en;
}

#endif
