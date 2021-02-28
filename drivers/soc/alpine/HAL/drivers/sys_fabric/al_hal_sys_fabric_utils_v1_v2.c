#include "al_hal_nb_regs_v1_v2.h"
#include "al_hal_sys_fabric_utils.h"
#include "al_hal_anpa_regs.h"

#if (!defined(AL_DEV_ID)) || (AL_DEV_ID <= AL_DEV_ID_ALPINE_V2)

#if (defined(AL_DEV_ID))
#define UTILS_STATIC
#else
#define UTILS_STATIC	static
#endif

#define NUM_CORES_PER_CLUSTER		4
#define NUM_LOCAL_CAUSE			4
#define NUM_SEMAPHORES			64

#define NB_GLOBAL_CPUS_SOFTWARE_RESET_LEVEL_CPU_PORESET \
		(0x1 << NB_GLOBAL_CPUS_SOFTWARE_RESET_LEVEL_SHIFT)

#define SB_MSTR_FORCE_SAME_ID_SEL_0_VAL_ORDERED		0xffffffff
#define SB_MSTR_FORCE_SAME_ID_SEL_0_VAL_NOT_ORDERED	0x00000022

#define SB_MSTR_FORCE_SAME_ID_SEL_1_VAL_ORDERED		0xffffffff
#define SB_MSTR_FORCE_SAME_ID_SEL_1_VAL_NOT_ORDERED	0x00000041

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
		(fabric_handle->ver <= NB_NB_VERSION_VERSION_RELEASE_NUM_MAJOR_VAL_ALPINE_V2) &&
		(!anpa_regs_base));

	handle->fabric_handle = fabric_handle;
	handle->anpa_regs_base = anpa_regs_base;
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC uint32_t _al_sys_fabric_int_local_mask_read(
	const struct al_sys_fabric_handle	*handle,
	unsigned int			idx)
{
	struct al_nb_regs __iomem *nb_regs;

	al_assert(handle);
	al_assert(idx < NUM_LOCAL_CAUSE);

	nb_regs = (struct al_nb_regs __iomem *)handle->nb_regs_base;

	return al_reg_read32(&nb_regs->cpun_config_status[idx].local_cause_mask);
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_int_local_unmask(
	const struct al_sys_fabric_handle	*handle,
	unsigned int			idx,
	unsigned int			mask)
{
	struct al_nb_regs __iomem *nb_regs = (struct al_nb_regs __iomem *)handle->nb_regs_base;

	al_assert(handle);
	al_assert(idx < NUM_LOCAL_CAUSE);

	al_reg_write32_masked(&nb_regs->cpun_config_status[idx].local_cause_mask, mask, mask);
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_int_local_mask(
	const struct al_sys_fabric_handle	*handle,
	unsigned int			idx,
	unsigned int			mask)
{
	struct al_nb_regs __iomem *nb_regs = (struct al_nb_regs __iomem *)handle->nb_regs_base;

	al_assert(handle);
	al_assert(idx < NUM_LOCAL_CAUSE);

	al_reg_write32_masked(&nb_regs->cpun_config_status[idx].local_cause_mask, mask, ~mask);
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_int_clear(
	const struct al_sys_fabric_handle	*handle,
	unsigned int			mask)
{
	struct al_nb_regs __iomem *nb_regs = (struct al_nb_regs __iomem *)handle->nb_regs_base;

	al_assert(handle);

	al_reg_write32(&nb_regs->global.nb_int_cause, mask);
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_int_cause_read(
	const struct al_sys_fabric_handle	*handle,
	uint32_t				*cause)
{
	struct al_nb_regs __iomem *nb_regs = (struct al_nb_regs __iomem *)handle->nb_regs_base;

	al_assert(handle);
	al_assert(cause);

	*cause = al_reg_read32(&nb_regs->global.nb_int_cause);
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_error_cause_read(const struct al_sys_fabric_handle	*handle,
						 uint32_t				*cause)
{
	struct al_nb_regs __iomem *nb_regs = (struct al_nb_regs __iomem *)handle->nb_regs_base;

	al_assert(handle);
	al_assert(cause);

	*cause = al_reg_read32(&nb_regs->global.error_cause);
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC uint32_t _al_sys_fabric_error_int_mask_read(
	const struct al_sys_fabric_handle	*handle)
{
	struct al_nb_regs __iomem *nb_regs;

	al_assert(handle);

	nb_regs = (struct al_nb_regs __iomem *)handle->nb_regs_base;

	return al_reg_read32(&nb_regs->global.error_mask);
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_error_int_unmask(
	const struct al_sys_fabric_handle	*handle,
	unsigned int			mask)
{
	struct al_nb_regs __iomem *nb_regs;

	al_assert(handle);

	nb_regs = (struct al_nb_regs __iomem *)handle->nb_regs_base;

	al_reg_write32_masked(&nb_regs->global.error_mask, mask, mask);
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_error_int_mask(
	const struct al_sys_fabric_handle	*handle,
	unsigned int			mask)
{
	struct al_nb_regs __iomem *nb_regs;

	al_assert(handle);

	nb_regs = (struct al_nb_regs __iomem *)handle->nb_regs_base;

	al_reg_write32_masked(&nb_regs->global.error_mask, mask, ~mask);
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_error_int_clear(
	const struct al_sys_fabric_handle	*handle,
	unsigned int			mask)
{
	struct al_nb_regs __iomem *nb_regs;

	al_assert(handle);

	nb_regs = (struct al_nb_regs __iomem *)handle->nb_regs_base;

	al_reg_write32(&nb_regs->global.error_cause, ~mask);
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_sb_pos_info_get_and_clear(
	const struct al_sys_fabric_handle	*handle,
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
	 * Software flow:
	 * read log_1 multiple times until interrupt is cleared
	 */
	if (handle->ver == NB_NB_VERSION_VERSION_RELEASE_NUM_MAJOR_VAL_ALPINE_V2) {
		int i;
		uint32_t nb_int_cause;

		for (i = 0; i < 1000; i++) {
			nb_int_cause = al_reg_read32(&nb_regs->global.nb_int_cause);
			if (!(nb_int_cause & NB_GLOBAL_NB_INT_CAUSE_SB_POS_ERR))
				break;

			al_reg_read32(&nb_regs->global.sb_pos_error_log_1);
			al_sys_fabric_int_clear(handle, ~NB_GLOBAL_NB_INT_CAUSE_SB_POS_ERR);
		}

		if (i == 1000)
			al_err("%s: tried to clear SB PoS error %d times, giving up...\n",
				__func__,
				i);
	} else {
		al_sys_fabric_int_clear(handle, ~NB_GLOBAL_NB_INT_CAUSE_SB_POS_ERR);
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
	struct al_nb_regs __iomem *nb_regs;
	uint32_t *msg_reg;
	uintptr_t address;

	al_assert(handle);
	al_assert(core < NUM_CORES_PER_CLUSTER);

	nb_regs = (struct al_nb_regs __iomem *)handle->fabric_handle->nb_regs_base;

	switch (core) {
	case 0:
		msg_reg = &nb_regs->debug.cpu_msg;
		break;
	case 1:
		msg_reg = &nb_regs->debug.cpu_msg1;
		break;
	case 2:
		msg_reg = &nb_regs->debug.cpu_msg2;
		break;
	case 3:
		msg_reg = &nb_regs->debug.cpu_msg3;
		break;
	default:
		al_assert(0);
		return;
	}

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
	struct al_nb_regs __iomem *nb_regs;
	unsigned int val;

	al_assert(handle);
	al_assert(core < NUM_CORES_PER_CLUSTER);

	nb_regs = (struct al_nb_regs __iomem *)handle->fabric_handle->nb_regs_base;

	val = al_reg_read32(&nb_regs->cpun_config_status[core].cpu_msg_in);

	*msg_valid = !!(val & NB_CPUN_CONFIG_STATUS_CPU_MSG_IN_VALID);
	*msg = (val & NB_CPUN_CONFIG_STATUS_CPU_MSG_IN_DATA_MASK) >>
		NB_CPUN_CONFIG_STATUS_CPU_MSG_IN_DATA_SHIFT;
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
	struct al_nb_regs __iomem *nb_regs;

	al_assert(handle);

	nb_regs = (struct al_nb_regs __iomem *)handle->fabric_handle->nb_regs_base;

	al_reg_write32(&nb_regs->global.cpu_max_pd_timer, pd);
	al_reg_write32(&nb_regs->global.cpu_max_pu_timer, pu);
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_cluster_power_ctrl(
	const struct al_sys_fabric_cluster_handle	*handle,
	unsigned int				ctrl)
{
	struct al_nb_regs __iomem *nb_regs;

	al_assert(handle);

	nb_regs = (struct al_nb_regs __iomem *)handle->fabric_handle->nb_regs_base;

	al_reg_write32(&nb_regs->global.cpus_power_ctrl, ctrl);
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_core_power_ctrl(
	const struct al_sys_fabric_cluster_handle	*handle,
	unsigned int				core,
	unsigned int				ctrl)
{
	struct al_nb_regs __iomem *nb_regs;

	al_assert(handle);
	al_assert(core < NUM_CORES_PER_CLUSTER);

	nb_regs = (struct al_nb_regs __iomem *)handle->fabric_handle->nb_regs_base;

	al_reg_write32(&nb_regs->cpun_config_status[core].power_ctrl, ctrl);
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_cluster_sw_reset(
	const struct al_sys_fabric_cluster_handle	*handle)
{
	struct al_nb_regs __iomem *nb_regs;

	al_assert(handle);

	nb_regs = (struct al_nb_regs __iomem *)handle->fabric_handle->nb_regs_base;

	al_reg_write32(
		&nb_regs->global.cpus_software_reset,
		(3 << NB_GLOBAL_CPUS_SOFTWARE_RESET_LEVEL_SHIFT) |
		NB_GLOBAL_CPUS_SOFTWARE_RESET_SWRESET_REQ);
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_core_sw_reset(
	const struct al_sys_fabric_cluster_handle	*handle,
	unsigned int				core)
{
	struct al_nb_regs __iomem *nb_regs;

	al_assert(handle);
	al_assert(core < NUM_CORES_PER_CLUSTER);

	nb_regs = (struct al_nb_regs __iomem *)handle->fabric_handle->nb_regs_base;

	al_reg_write32(
		&nb_regs->global.cpus_software_reset,
		(1 << (NB_GLOBAL_CPUS_SOFTWARE_RESET_CORES_SHIFT + core)) |
		NB_GLOBAL_CPUS_SOFTWARE_RESET_SWRESET_REQ);
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_core_power_on_reset(
	const struct al_sys_fabric_cluster_handle	*handle,
	unsigned int				core)
{
	struct al_nb_regs __iomem *nb_regs;

	al_assert(handle);
	al_assert(core < NUM_CORES_PER_CLUSTER);

	nb_regs = (struct al_nb_regs __iomem *)handle->fabric_handle->nb_regs_base;

	al_reg_write32(
		&nb_regs->global.cpus_software_reset,
		(1 << (NB_GLOBAL_CPUS_SOFTWARE_RESET_CORES_SHIFT + core)) |
		NB_GLOBAL_CPUS_SOFTWARE_RESET_LEVEL_CPU_PORESET |
		NB_GLOBAL_CPUS_SOFTWARE_RESET_SWRESET_REQ);
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_core_reset_deassert(
	const struct al_sys_fabric_cluster_handle	*handle,
	unsigned int				core)
{
	struct al_nb_regs __iomem *nb_regs;

	al_assert(handle);
	al_assert(core < NUM_CORES_PER_CLUSTER);

	nb_regs = (struct al_nb_regs __iomem *)handle->fabric_handle->nb_regs_base;

	al_reg_write32_masked(
		&nb_regs->global.cpus_init_control, AL_BIT(core), AL_BIT(core));
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_core_aarch64_setup(
	const struct al_sys_fabric_cluster_handle	*handle,
	unsigned int				core,
	uint32_t				entry_high,
	uint32_t				entry_low)
{
	struct al_nb_regs __iomem *nb_regs;

	al_assert(handle);
	al_assert(core < NUM_CORES_PER_CLUSTER);
	al_assert(entry_high || entry_low);

	nb_regs = (struct al_nb_regs __iomem *)handle->fabric_handle->nb_regs_base;

	al_reg_write32(&nb_regs->cpun_config_status[core].rvbar_low, entry_low);
	al_reg_write32(&nb_regs->cpun_config_status[core].rvbar_high, entry_high);
	al_reg_write32(&nb_regs->cpun_config_status[core].config_aarch64,
		NB_CPUN_CONFIG_STATUS_CONFIG_AARCH64_AA64_NAA32);
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_core_aarch32_setup(
	const struct al_sys_fabric_cluster_handle	*handle,
	unsigned int				core,
	uint32_t				entry_high,
	uint32_t				entry_low)
{
	struct al_nb_regs __iomem *nb_regs;

	al_assert(handle);
	al_assert(core < NUM_CORES_PER_CLUSTER);
	al_assert(entry_high || entry_low);

	nb_regs = (struct al_nb_regs __iomem *)handle->fabric_handle->nb_regs_base;

	al_reg_write32(&nb_regs->cpun_config_status[core].resume_addr_l, entry_low);
	al_reg_write32(&nb_regs->cpun_config_status[core].resume_addr_h, entry_high);
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_core_aarch32_setup_get(
	const struct al_sys_fabric_cluster_handle	*handle,
	unsigned int				core,
	uint32_t				*entry_high,
	uint32_t				*entry_low)
{
	struct al_nb_regs __iomem *nb_regs;

	al_assert(handle);
	al_assert(core < NUM_CORES_PER_CLUSTER);
	al_assert(entry_high || entry_low);

	nb_regs = (struct al_nb_regs __iomem *)handle->fabric_handle->nb_regs_base;

	*entry_low = al_reg_read32(&nb_regs->cpun_config_status[core].resume_addr_l);
	*entry_high = al_reg_read32(&nb_regs->cpun_config_status[core].resume_addr_h);
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC struct al_iofic_regs __iomem *_al_sys_fabric_iofic_regs_get(
	const struct al_sys_fabric_handle	*fabric_handle __attribute__((unused)),
	enum al_sys_fabric_iofic_id	id __attribute__((unused)))
{
	al_err("%s: not supported!\n", __func__);

	return NULL;
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_iocache_usage_cfg_set(
	const struct al_sys_fabric_handle		*fabric_handle __attribute__((unused)),
	const struct al_sys_fabric_iocache_usage_cfg	*cfg __attribute__((unused)))
{
	al_err("%s: not supported!\n", __func__);
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_wr_split_mode_set(
		const struct al_sys_fabric_handle		*handle,
		enum al_sys_fabric_wr_split_mode	mode)
{
	al_assert(handle);

	if (handle->ver == NB_NB_VERSION_VERSION_RELEASE_NUM_MAJOR_VAL_ALPINE_V2) {
		struct al_nb_regs __iomem *nb_regs =
			(struct al_nb_regs __iomem *)handle->nb_regs_base;

		if (mode == SYS_FABRIC_WR_SPLIT_MODE_ALPINE_V1) {
			al_reg_write32(&nb_regs->global.sb_mstr_force_same_id_sel_0,
					SB_MSTR_FORCE_SAME_ID_SEL_0_VAL_ORDERED);

			al_reg_write32(&nb_regs->global.sb_mstr_force_same_id_sel_1,
					SB_MSTR_FORCE_SAME_ID_SEL_1_VAL_ORDERED);

			al_reg_write32_masked(&nb_regs->global.acf_misc,
					NB_GLOBAL_ACF_MISC_WRSPLT_ALPINE_M0_MODE,
					NB_GLOBAL_ACF_MISC_WRSPLT_ALPINE_M0_MODE);
		} else if (mode == SYS_FABRIC_WR_SPLIT_MODE_ALPINE_V2) {
			al_reg_write32_masked(&nb_regs->global.acf_misc,
					NB_GLOBAL_ACF_MISC_WRSPLT_ALPINE_M0_MODE,
					0);

			al_reg_write32(&nb_regs->global.sb_mstr_force_same_id_sel_0,
					SB_MSTR_FORCE_SAME_ID_SEL_0_VAL_NOT_ORDERED);

			al_reg_write32(&nb_regs->global.sb_mstr_force_same_id_sel_1,
					SB_MSTR_FORCE_SAME_ID_SEL_1_VAL_NOT_ORDERED);
		} else {
			al_err("%s: unsupported mode (%d)\n", __func__, mode);
			al_assert(0);
		}
	} else if (handle->ver == NB_NB_VERSION_VERSION_RELEASE_NUM_MAJOR_VAL_ALPINE_V1) {
		if (mode == SYS_FABRIC_WR_SPLIT_MODE_ALPINE_V1) {
			return;
		} else {
			al_err("%s: unsupported mode (%d)\n", __func__, mode);
			al_assert(0);
		}
	} else {
		al_err("%s: unsupported NB version (%u)\n", __func__, handle->ver);
		al_assert(0);
	}
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_wr_split_mode_get(
		const struct al_sys_fabric_handle	*handle,
		enum al_sys_fabric_wr_split_mode	*mode)
{
	al_assert(handle);
	al_assert(mode);

	if (handle->ver == NB_NB_VERSION_VERSION_RELEASE_NUM_MAJOR_VAL_ALPINE_V2) {
		uint32_t sb_mstr_force_same_id_sel_0;
		uint32_t sb_mstr_force_same_id_sel_1;
		uint32_t acf_misc;
		struct al_nb_regs __iomem *nb_regs =
			(struct al_nb_regs __iomem *)handle->nb_regs_base;

		acf_misc = al_reg_read32(&nb_regs->global.acf_misc);
		sb_mstr_force_same_id_sel_0 =
				al_reg_read32(&nb_regs->global.sb_mstr_force_same_id_sel_0);
		sb_mstr_force_same_id_sel_1 =
				al_reg_read32(&nb_regs->global.sb_mstr_force_same_id_sel_1);

		if ((acf_misc & NB_GLOBAL_ACF_MISC_WRSPLT_ALPINE_M0_MODE) &&
				(sb_mstr_force_same_id_sel_0 ==
						SB_MSTR_FORCE_SAME_ID_SEL_0_VAL_ORDERED) &&
				(sb_mstr_force_same_id_sel_1 ==
						SB_MSTR_FORCE_SAME_ID_SEL_1_VAL_ORDERED)) {
			*mode = SYS_FABRIC_WR_SPLIT_MODE_ALPINE_V1;
		} else if (!(acf_misc & NB_GLOBAL_ACF_MISC_WRSPLT_ALPINE_M0_MODE) &&
				(sb_mstr_force_same_id_sel_0 ==
						SB_MSTR_FORCE_SAME_ID_SEL_0_VAL_NOT_ORDERED) &&
				(sb_mstr_force_same_id_sel_1 ==
						SB_MSTR_FORCE_SAME_ID_SEL_1_VAL_NOT_ORDERED)) {
			*mode = SYS_FABRIC_WR_SPLIT_MODE_ALPINE_V2;
		} else {
			*mode = SYS_FABRIC_WR_SPLIT_MODE_UNKNOWN;
		}
	} else if (handle->ver == NB_NB_VERSION_VERSION_RELEASE_NUM_MAJOR_VAL_ALPINE_V1) {
		*mode = SYS_FABRIC_WR_SPLIT_MODE_ALPINE_V1;
	} else {
		al_err("%s: unsupported NB version (%u)\n", __func__, handle->ver);
		al_assert(0);
	}
}

UTILS_STATIC void _al_sys_fabric_error_ints_mask_get(
	const struct al_sys_fabric_handle		*fabric_handle,
	uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d)
{
	al_assert(fabric_handle);
	al_assert(a);
	al_assert(b);
	al_assert(c);
	al_assert(d);

	al_assert_msg(0, "%s: No SFIC support\n", __func__);

	*a = 0;
	*b = 0;
	*c = 0;
	*d = 0;
}

UTILS_STATIC void _al_sys_fabric_error_unmask(const struct al_sys_fabric_handle *fabric_handle)
{
	al_assert(fabric_handle);
	al_assert_msg(0, "%s: No SFIC support\n", __func__);
}

UTILS_STATIC void _al_sys_fabric_nmi_ints_mask_get(
	const struct al_sys_fabric_handle		*fabric_handle,
	uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d)
{
	al_assert(fabric_handle);
	al_assert(a);
	al_assert(b);
	al_assert(c);
	al_assert(d);

	al_assert_msg(0, "%s: No SFIC support\n", __func__);

	*a = 0;
	*b = 0;
	*c = 0;
	*d = 0;
}

UTILS_STATIC void _al_sys_fabric_nmi_unmask(const struct al_sys_fabric_handle *fabric_handle)
{
	al_assert(fabric_handle);

	al_assert_msg(0, "%s: No SFIC support\n", __func__);
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

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_qos_cfg_set(
	const struct al_sys_fabric_handle	*handle,
	const struct al_sys_fabric_qos_cfg	*cfg)
{
	struct al_nb_regs __iomem *nb_regs;

	al_assert(handle);
	al_assert(cfg);

	nb_regs = (struct al_nb_regs __iomem *)handle->nb_regs_base;

	al_reg_write32_masked(&nb_regs->global.qos_set_control,
		NB_GLOBAL_QOS_SET_CONTROL_CPU_LP_ARQOS_MASK |
		NB_GLOBAL_QOS_SET_CONTROL_CPU_HP_ARQOS_MASK |
		NB_GLOBAL_QOS_SET_CONTROL_CPU_LP_AWQOS_MASK |
		NB_GLOBAL_QOS_SET_CONTROL_CPU_HP_AWQOS_MASK |
		NB_GLOBAL_QOS_SET_CONTROL_SB_LP_ARQOS_MASK |
		NB_GLOBAL_QOS_SET_CONTROL_SB_LP_AWQOS_MASK,
		(cfg->cpu_lp_arqos << NB_GLOBAL_QOS_SET_CONTROL_CPU_LP_ARQOS_SHIFT) |
		(cfg->cpu_hp_arqos << NB_GLOBAL_QOS_SET_CONTROL_CPU_HP_ARQOS_SHIFT) |
		(cfg->cpu_lp_awqos << NB_GLOBAL_QOS_SET_CONTROL_CPU_LP_AWQOS_SHIFT) |
		(cfg->cpu_hp_awqos << NB_GLOBAL_QOS_SET_CONTROL_CPU_HP_AWQOS_SHIFT) |
		(cfg->sb_lp_arqos << NB_GLOBAL_QOS_SET_CONTROL_SB_LP_ARQOS_SHIFT) |
		(cfg->sb_lp_awqos << NB_GLOBAL_QOS_SET_CONTROL_SB_LP_AWQOS_SHIFT));
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_qos_cfg_get(
	const struct al_sys_fabric_handle	*handle,
	struct al_sys_fabric_qos_cfg		*cfg)
{
	struct al_nb_regs __iomem *nb_regs;
	uint32_t reg_val;

	al_assert(handle);
	al_assert(cfg);

	nb_regs = (struct al_nb_regs __iomem *)handle->nb_regs_base;

	reg_val = al_reg_read32(&nb_regs->global.qos_set_control);

	cfg->cpu_lp_arqos = AL_REG_FIELD_GET(reg_val,
		NB_GLOBAL_QOS_SET_CONTROL_CPU_LP_ARQOS_MASK,
		NB_GLOBAL_QOS_SET_CONTROL_CPU_LP_ARQOS_SHIFT);
	cfg->cpu_hp_arqos = AL_REG_FIELD_GET(reg_val,
		NB_GLOBAL_QOS_SET_CONTROL_CPU_HP_ARQOS_MASK,
		NB_GLOBAL_QOS_SET_CONTROL_CPU_HP_ARQOS_SHIFT);
	cfg->cpu_lp_awqos = AL_REG_FIELD_GET(reg_val,
		NB_GLOBAL_QOS_SET_CONTROL_CPU_LP_AWQOS_MASK,
		NB_GLOBAL_QOS_SET_CONTROL_CPU_LP_AWQOS_SHIFT);
	cfg->cpu_hp_awqos = AL_REG_FIELD_GET(reg_val,
		NB_GLOBAL_QOS_SET_CONTROL_CPU_HP_AWQOS_MASK,
		NB_GLOBAL_QOS_SET_CONTROL_CPU_HP_AWQOS_SHIFT);
	cfg->sb_lp_arqos = AL_REG_FIELD_GET(reg_val,
		NB_GLOBAL_QOS_SET_CONTROL_SB_LP_ARQOS_MASK,
		NB_GLOBAL_QOS_SET_CONTROL_SB_LP_ARQOS_SHIFT);
	cfg->sb_lp_awqos = AL_REG_FIELD_GET(reg_val,
		NB_GLOBAL_QOS_SET_CONTROL_SB_LP_AWQOS_MASK,
		NB_GLOBAL_QOS_SET_CONTROL_SB_LP_AWQOS_SHIFT);
}

/**************************************************************************************************/
/**************************************************************************************************/
UTILS_STATIC void _al_sys_fabric_iof_2_iof_path_en(
	const struct al_sys_fabric_handle	*handle,
	al_bool					en)
{
	struct al_nb_regs __iomem *nb_regs;

	al_assert(handle);

	nb_regs = (struct al_nb_regs __iomem *)handle->nb_regs_base;

	al_reg_write32_masked(&nb_regs->global.acf_misc,
		NB_GLOBAL_ACF_MISC_SB2SB_PATH_DIS,
		en ? 0 : NB_GLOBAL_ACF_MISC_SB2SB_PATH_DIS);
}

#endif

#if (!defined(AL_DEV_ID))
void al_hal_sys_fabric_utils_imp_v1_v2(struct al_sys_fabric_imp *imp);

void al_hal_sys_fabric_utils_imp_v1_v2(struct al_sys_fabric_imp *imp)
{
	al_memset(imp, 0, sizeof(struct al_sys_fabric_imp));

	imp->al_sys_fabric_handle_init = _al_sys_fabric_handle_init;
	imp->al_sys_fabric_cluster_handle_init = _al_sys_fabric_cluster_handle_init;
	imp->al_sys_fabric_int_local_mask_read = _al_sys_fabric_int_local_mask_read;
	imp->al_sys_fabric_int_local_unmask = _al_sys_fabric_int_local_unmask;
	imp->al_sys_fabric_int_local_mask = _al_sys_fabric_int_local_mask;
	imp->al_sys_fabric_int_clear = _al_sys_fabric_int_clear;
	imp->al_sys_fabric_int_cause_read = _al_sys_fabric_int_cause_read;
	imp->al_sys_fabric_error_cause_read = _al_sys_fabric_error_cause_read;
	imp->al_sys_fabric_error_int_mask_read = _al_sys_fabric_error_int_mask_read;
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
	imp->al_sys_fabric_wr_split_mode_set = _al_sys_fabric_wr_split_mode_set;
	imp->al_sys_fabric_wr_split_mode_get = _al_sys_fabric_wr_split_mode_get;
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

