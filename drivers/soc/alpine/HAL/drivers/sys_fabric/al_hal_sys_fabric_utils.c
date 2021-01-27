/*
 * Copyright 2018, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#include "al_hal_sys_fabric_utils.h"

#if (!defined(AL_DEV_ID))

#define IMP(func, ...)							\
	do {								\
		if (!handle->imp.func)					\
			return -EINVAL;					\
									\
		return handle->imp.func(__VA_ARGS__);			\
	} while (0)

#define IMP_RET_POINTER(func, ...)					\
	do {								\
		if (!handle->imp.func)					\
			return NULL;					\
									\
		return handle->imp.func(__VA_ARGS__);			\
	} while (0)

#define IMP_NO_RET(func, ...)						\
	do {								\
		if (!handle->imp.func)					\
			return;						\
									\
		handle->imp.func(__VA_ARGS__);				\
	} while (0)

#define IMP_CLUSTER(func, ...)						\
	do {								\
		if (!handle->fabric_handle->imp.func)			\
			return -EINVAL;					\
									\
		return handle->fabric_handle->imp.func(__VA_ARGS__);	\
	} while (0)

#define IMP_CLUSTER_NO_RET(func, ...)					\
	do {								\
		if (!handle->fabric_handle->imp.func)			\
			return;						\
									\
		return handle->fabric_handle->imp.func(__VA_ARGS__);	\
	} while (0)

void al_sys_fabric_handle_init(
	struct al_sys_fabric_handle		*handle,
	void __iomem				*nb_regs_base)
{
	IMP_NO_RET(al_sys_fabric_handle_init, handle, nb_regs_base);
}

void al_sys_fabric_cluster_handle_init(
	struct al_sys_fabric_cluster_handle	*handle,
	const struct al_sys_fabric_handle	*fabric_handle,
	void __iomem				*anpa_regs_base)
{
	IMP_CLUSTER_NO_RET(al_sys_fabric_cluster_handle_init,
			   handle, fabric_handle, anpa_regs_base);
}

uint32_t al_sys_fabric_int_local_mask_read(
	const struct al_sys_fabric_handle	*handle,
	unsigned int				idx)
{
	IMP(al_sys_fabric_int_local_mask_read, handle, idx);
}

void al_sys_fabric_int_local_unmask(
	const struct al_sys_fabric_handle	*handle,
	unsigned int				idx,
	unsigned int				mask)
{
	IMP_NO_RET(al_sys_fabric_int_local_unmask, handle, idx, mask);
}

void al_sys_fabric_int_local_mask(
	const struct al_sys_fabric_handle	*handle,
	unsigned int				idx,
	unsigned int				mask)
{
	IMP_NO_RET(al_sys_fabric_int_local_mask, handle, idx, mask);
}

void al_sys_fabric_int_clear(
	const struct al_sys_fabric_handle	*handle,
	unsigned int				mask)
{
	IMP_NO_RET(al_sys_fabric_int_clear, handle, mask);
}

void al_sys_fabric_int_cause_read(
	const struct al_sys_fabric_handle	*handle,
	uint32_t				*cause)
{
	IMP_NO_RET(al_sys_fabric_int_cause_read, handle, cause);
}

void al_sys_fabric_error_cause_read(
	const struct al_sys_fabric_handle	*handle,
	uint32_t				*cause)
{
	IMP_NO_RET(al_sys_fabric_error_cause_read, handle, cause);
}

uint32_t al_sys_fabric_error_int_mask_read(
	const struct al_sys_fabric_handle	*handle)
{
	IMP(al_sys_fabric_error_int_mask_read, handle);
}

void al_sys_fabric_error_int_unmask(
	const struct al_sys_fabric_handle	*handle,
	unsigned int				mask)
{
	IMP_NO_RET(al_sys_fabric_error_int_unmask, handle, mask);
}

void al_sys_fabric_error_int_mask(
	const struct al_sys_fabric_handle	*handle,
	unsigned int				mask)
{
	IMP_NO_RET(al_sys_fabric_error_int_mask, handle, mask);
}

void al_sys_fabric_error_int_clear(
	const struct al_sys_fabric_handle	*handle,
	unsigned int				mask)
{
	IMP_NO_RET(al_sys_fabric_error_int_clear, handle, mask);
}

void al_sys_fabric_sb_pos_info_get_and_clear(
	const struct al_sys_fabric_handle	*handle,
	struct al_sys_fabric_sb_pos_error_info	*pos_info)
{
	IMP_NO_RET(al_sys_fabric_sb_pos_info_get_and_clear, handle, pos_info);
}

void al_sys_fabric_core_msg_set(
	const struct al_sys_fabric_cluster_handle	*handle,
	unsigned int					core,
	unsigned int					msg,
	al_bool						concat_reg_addr)
{
	IMP_CLUSTER_NO_RET(al_sys_fabric_core_msg_set, handle, core, msg, concat_reg_addr);
}

void al_sys_fabric_core_msg_get(
	const struct al_sys_fabric_cluster_handle	*handle,
	unsigned int					core,
	unsigned int					*msg_valid,
	unsigned int					*msg)
{
	IMP_CLUSTER_NO_RET(al_sys_fabric_core_msg_get, handle, core, msg_valid, msg);
}

unsigned int al_sys_fabric_sys_counter_freq_get(
	const struct al_sys_fabric_handle	*handle)
{
	IMP(al_sys_fabric_sys_counter_freq_get, handle);
}

uint64_t al_sys_fabric_sys_counter_get_64(
	const struct al_sys_fabric_handle	*handle)
{
	IMP(al_sys_fabric_sys_counter_get_64, handle);
}

uint32_t al_sys_fabric_sys_counter_get_32(
	const struct al_sys_fabric_handle	*handle)
{
	IMP(al_sys_fabric_sys_counter_get_32, handle);
}

int al_sys_fabric_hw_semaphore_lock(
	const struct al_sys_fabric_handle	*handle,
	unsigned int				id)
{
	IMP(al_sys_fabric_hw_semaphore_lock, handle, id);
}

void al_sys_fabric_hw_semaphore_unlock(
	const struct al_sys_fabric_handle	*handle,
	unsigned int				id)
{
	IMP_NO_RET(al_sys_fabric_hw_semaphore_unlock, handle, id);
}

void al_sys_fabric_cluster_pd_pu_timer_set(
	const struct al_sys_fabric_cluster_handle	*handle,
	unsigned int					pd,
	unsigned int					pu)
{
	IMP_CLUSTER_NO_RET(al_sys_fabric_cluster_pd_pu_timer_set, handle, pd, pu);
}

void al_sys_fabric_cluster_power_ctrl(
	const struct al_sys_fabric_cluster_handle	*handle,
	unsigned int					ctrl)
{
	IMP_CLUSTER_NO_RET(al_sys_fabric_cluster_power_ctrl, handle, ctrl);
}

void al_sys_fabric_core_power_ctrl(
	const struct al_sys_fabric_cluster_handle	*handle,
	unsigned int					core,
	unsigned int					ctrl)
{
	IMP_CLUSTER_NO_RET(al_sys_fabric_core_power_ctrl, handle, core, ctrl);
}

void al_sys_fabric_cluster_sw_reset(
	const struct al_sys_fabric_cluster_handle	*handle)
{
	IMP_CLUSTER_NO_RET(al_sys_fabric_cluster_sw_reset, handle);
}

void al_sys_fabric_core_sw_reset(
	const struct al_sys_fabric_cluster_handle	*handle,
	unsigned int					core)
{
	IMP_CLUSTER_NO_RET(al_sys_fabric_core_sw_reset, handle, core);
}

void al_sys_fabric_core_power_on_reset(
	const struct al_sys_fabric_cluster_handle	*handle,
	unsigned int					core)
{
	IMP_CLUSTER_NO_RET(al_sys_fabric_core_power_on_reset, handle, core);
}

void al_sys_fabric_core_reset_deassert(
	const struct al_sys_fabric_cluster_handle	*handle,
	unsigned int					core)
{
	IMP_CLUSTER_NO_RET(al_sys_fabric_core_reset_deassert, handle, core);
}

void al_sys_fabric_core_aarch64_setup(
	const struct al_sys_fabric_cluster_handle	*handle,
	unsigned int					core,
	uint32_t					entry_high,
	uint32_t					entry_low)
{
	IMP_CLUSTER_NO_RET(al_sys_fabric_core_aarch64_setup, handle, core, entry_high, entry_low);
}

void al_sys_fabric_core_aarch32_setup(
	const struct al_sys_fabric_cluster_handle	*handle,
	unsigned int					core,
	uint32_t					entry_high,
	uint32_t					entry_low)
{
	IMP_CLUSTER_NO_RET(al_sys_fabric_core_aarch32_setup, handle, core, entry_high, entry_low);
}

void al_sys_fabric_core_aarch32_setup_get(
	const struct al_sys_fabric_cluster_handle	*handle,
	unsigned int					core,
	uint32_t					*entry_high,
	uint32_t					*entry_low)
{
	IMP_CLUSTER_NO_RET(al_sys_fabric_core_aarch32_setup_get,
			   handle, core, entry_high, entry_low);
}

struct al_iofic_regs __iomem *al_sys_fabric_iofic_regs_get(
	const struct al_sys_fabric_handle		*handle,
	enum al_sys_fabric_iofic_id			id)
{
	if (!handle->imp.al_sys_fabric_iofic_regs_get)
		return NULL;

	return handle->imp.al_sys_fabric_iofic_regs_get(handle, id);
}

void al_sys_fabric_iocache_usage_cfg_set(
	const struct al_sys_fabric_handle		*handle,
	const struct al_sys_fabric_iocache_usage_cfg	*cfg)
{
	IMP_NO_RET(al_sys_fabric_iocache_usage_cfg_set, handle, cfg);
}

void al_sys_fabric_wr_split_mode_set(
		const struct al_sys_fabric_handle	*handle,
		enum al_sys_fabric_wr_split_mode	mode)
{
	IMP_NO_RET(al_sys_fabric_wr_split_mode_set, handle, mode);
}

void al_sys_fabric_wr_split_mode_get(
		const struct al_sys_fabric_handle	*handle,
		enum al_sys_fabric_wr_split_mode	*mode)
{
	IMP_NO_RET(al_sys_fabric_wr_split_mode_get, handle, mode);
}

void al_sys_fabric_error_ints_mask_get(
	const struct al_sys_fabric_handle	*handle,
	uint32_t				*a,
	uint32_t				*b,
	uint32_t				*c,
	uint32_t				*d)
{
	IMP_NO_RET(al_sys_fabric_error_ints_mask_get, handle, a, b, c, d);
}

void al_sys_fabric_error_unmask(
	const struct al_sys_fabric_handle	*handle)
{
	IMP_NO_RET(al_sys_fabric_error_unmask, handle);
}

void al_sys_fabric_nmi_ints_mask_get(
	const struct al_sys_fabric_handle	*handle,
	uint32_t				*a,
	uint32_t				*b,
	uint32_t				*c,
	uint32_t				*d)
{
	IMP_NO_RET(al_sys_fabric_nmi_ints_mask_get, handle, a, b, c, d);
}

void al_sys_fabric_nmi_unmask(
	const struct al_sys_fabric_handle	*handle)
{
	IMP_NO_RET(al_sys_fabric_nmi_unmask, handle);
}


void __iomem *al_sys_fabric_nmi_sfic_regs_base_get(
	const struct al_sys_fabric_handle *handle)
{
	IMP_RET_POINTER(al_sys_fabric_nmi_sfic_regs_base_get, handle);
}

void __iomem *al_sys_fabric_err_sfic_regs_base_get(
	const struct al_sys_fabric_handle *handle)
{
	IMP_RET_POINTER(al_sys_fabric_err_sfic_regs_base_get, handle);
}

void al_sys_fabric_qos_cfg_set(
	const struct al_sys_fabric_handle	*handle,
	const struct al_sys_fabric_qos_cfg	*cfg)
{
	IMP_NO_RET(al_sys_fabric_qos_cfg_set, handle, cfg);
}

void al_sys_fabric_qos_cfg_get(
	const struct al_sys_fabric_handle	*handle,
	struct al_sys_fabric_qos_cfg		*cfg)
{
	IMP_NO_RET(al_sys_fabric_qos_cfg_get, handle, cfg);
}

void al_sys_fabric_iof_2_iof_path_en(
	const struct al_sys_fabric_handle	*handle,
	al_bool					en)
{
	IMP_NO_RET(al_sys_fabric_iof_2_iof_path_en, handle, en);
}

#endif
