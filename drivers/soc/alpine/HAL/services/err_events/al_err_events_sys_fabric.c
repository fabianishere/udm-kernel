/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#if defined(AL_DEV_ID)

#include "al_err_events_internal.h"
#include "al_hal_pbs_utils.h"
#include "al_err_events_sys_fabric.h"
#include "al_hal_nb_regs.h"
#include "al_hal_iofic.h"
#include "al_hal_sys_fabric_utils.h"

static void err_events_l1_field_print(struct al_err_events_field *field, al_bool print_header);
static void err_events_l2_field_print(struct al_err_events_field *field, al_bool print_header);

static void err_events_sf_ddr_corr_print(struct al_err_events_field *field, al_bool print_header);
static void err_events_sf_ddr_uncorr_print(struct al_err_events_field *field, al_bool print_header);
static void err_events_sf_pos_error_print(struct al_err_events_field *field, al_bool print_header);
static void err_events_sf_ddr_on_chip_parity_print(struct al_err_events_field *field,
						   al_bool print_header);
static void sys_fabric_ddr_on_chip_parity_collect(struct al_err_events_field *field,
						  uint32_t cause);

static void sys_fabric_ddr_correctable_collect(struct al_err_events_field *field, uint32_t cause);
static void sys_fabric_ddr_uncorrectable_collect(struct al_err_events_field *field, uint32_t cause);
static void sys_fabric_pos_error_collect(struct al_err_events_field *field, uint32_t cause);
static void err_events_l2_nb_int_mask(struct al_err_events_field *field);

static void err_events_sys_fabric_common_print(struct al_err_events_module *module,
					       struct al_err_events_field *field,
					       al_bool print_header);

#if (AL_DEV_ID <= AL_DEV_ID_ALPINE_V2)
static void sys_fabric_error_int_collect(struct al_err_events_field *field, uint32_t cause);
static void sys_fabric_field_nb_int_error_collect(struct al_err_events_field *field,
						  uint32_t cause);
static void err_events_sys_fabric_nb_int_mask(struct al_err_events_field *field);
static void err_events_sys_fabric_error_cause_mask(struct al_err_events_field *field);
static void sys_fabric_ddr_parity_collect(struct al_err_events_field *field, uint32_t cause);
#endif

/*******************************************************************************
 ** Static error fields
 ******************************************************************************/
static const struct al_err_events_field al_err_events_l1_fields[AL_ERR_EVENTS_L1_MAX_ERRORS] = {
	{
		.name = "Correctable Error",
		.id = AL_ERR_EVENTS_L1_CORRECTABLE_ERROR,
		.sev = AL_ERR_EVENTS_CORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_POLLING,
		.print = err_events_l1_field_print,
		.collect = al_err_events_common_field_empty_collect,
		.mask = al_err_events_common_field_empty_mask,
	},
	{
		.name = "Uncorrectable Error",
		.id = AL_ERR_EVENTS_L1_UNCORRECTABLE_ERROR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_POLLING,
		.print = err_events_l1_field_print,
		.collect = al_err_events_common_field_empty_collect,
		.mask = al_err_events_common_field_empty_mask,
	},
	{
		.name = "Instruction Tag RAM Correctable Error",
		.id = AL_ERR_EVENTS_L1_ITAG_CORRECTABLE_ERROR,
		.sev = AL_ERR_EVENTS_CORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_POLLING,
		.print = err_events_l1_field_print,
		.collect = al_err_events_common_field_empty_collect,
		.mask = al_err_events_common_field_empty_mask,
	},
	{
		.name = "Instruction Data RAM Correctable Error",
		.id = AL_ERR_EVENTS_L1_IDATA_CORRECTABLE_ERROR,
		.sev = AL_ERR_EVENTS_CORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_POLLING,
		.print = err_events_l1_field_print,
		.collect = al_err_events_common_field_empty_collect,
		.mask = al_err_events_common_field_empty_mask,
	},
	{
		.name = "Data Tag RAM Correctable Error",
		.id = AL_ERR_EVENTS_L1_DTAG_CORRECTABLE_ERROR,
		.sev = AL_ERR_EVENTS_CORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_POLLING,
		.print = err_events_l1_field_print,
		.collect = al_err_events_common_field_empty_collect,
		.mask = al_err_events_common_field_empty_mask,
	},
	{
		.name = "Data-D RAM Correctable Error",
		.id = AL_ERR_EVENTS_L1_DDATA_CORRECTABLE_ERROR,
		.sev = AL_ERR_EVENTS_CORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_POLLING,
		.print = err_events_l1_field_print,
		.collect = al_err_events_common_field_empty_collect,
		.mask = al_err_events_common_field_empty_mask,
	},
	{
		.name = "L2 TLB RAM Correctable Error",
		.id = AL_ERR_EVENTS_L1_L2_TLB_CORRECTABLE_ERROR,
		.sev = AL_ERR_EVENTS_CORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_POLLING,
		.print = err_events_l1_field_print,
		.collect = al_err_events_common_field_empty_collect,
		.mask = al_err_events_common_field_empty_mask,
	},
	{
		.name = "Instruction Tag RAM non fatal Error",
		.id = AL_ERR_EVENTS_L1_ITAG_UNCORRECTABLE_ERROR,
		/*
		 * this error is marked as correctable since the HW
		 * auto recover by fetching the data from next cache level
		 */
		.sev = AL_ERR_EVENTS_CORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_POLLING,
		.print = err_events_l1_field_print,
		.collect = al_err_events_common_field_empty_collect,
		.mask = al_err_events_common_field_empty_mask,
	},
	{
		.name = "Instruction Data RAM non fatal Error",
		.id = AL_ERR_EVENTS_L1_IDATA_UNCORRECTABLE_ERROR,
		/*
		 * this error is marked as correctable since the HW
		 * auto recover by fetching the data from next cache level
		 */
		.sev = AL_ERR_EVENTS_CORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_POLLING,
		.print = err_events_l1_field_print,
		.collect = al_err_events_common_field_empty_collect,
		.mask = al_err_events_common_field_empty_mask,
	},
	{
		.name = "Data Tag RAM Uncorrectable Error",
		.id = AL_ERR_EVENTS_L1_DTAG_UNCORRECTABLE_ERROR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_POLLING,
		.print = err_events_l1_field_print,
		.collect = al_err_events_common_field_empty_collect,
		.mask = al_err_events_common_field_empty_mask,
	},
	{
		.name = "Data-D RAM Uncorrectable Error",
		.id = AL_ERR_EVENTS_L1_DDATA_UNCORRECTABLE_ERROR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_POLLING,
		.print = err_events_l1_field_print,
		.collect = al_err_events_common_field_empty_collect,
		.mask = al_err_events_common_field_empty_mask,
	},
	{
		.name = "L2 TLB RAM non fatal Error",
		.id = AL_ERR_EVENTS_L1_L2_TLB_UNCORRECTABLE_ERROR,
		/*
		 * this error is marked as correctable since the HW
		 * auto recover by fetching the data from next cache level
		 */
		.sev = AL_ERR_EVENTS_CORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_POLLING,
		.print = err_events_l1_field_print,
		.collect = al_err_events_common_field_empty_collect,
		.mask = al_err_events_common_field_empty_mask,
	},
	{
		.name = "Unknown Error",
		.id = AL_ERR_EVENTS_L1_UNKNOWN_ERROR,
		.sev = AL_ERR_EVENTS_CORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_POLLING,
		.print = err_events_l1_field_print,
		.collect = al_err_events_common_field_empty_collect,
		.mask = al_err_events_common_field_empty_mask,
	},
};

static const struct al_err_events_field al_err_events_l2_fields[AL_ERR_EVENTS_L2_MAX_ERRORS] = {
	{
		.name = "Correctable Error",
		.id = AL_ERR_EVENTS_L2_CORRECTABLE_ERROR,
		.sev = AL_ERR_EVENTS_CORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_POLLING,
		.print = err_events_l2_field_print,
		.collect = al_err_events_common_field_empty_collect,
		.mask = al_err_events_common_field_empty_mask,
	},
	{
		.name = "Uncorrectable Error",
		.id = AL_ERR_EVENTS_L2_UNCORRECTABLE_ERROR,
#if (AL_DEV_ID <= AL_DEV_ID_ALPINE_V2)
		.iofic_bit = NB_GLOBAL_NB_INT_CAUSE_CPU_INTERRIRQ,
#endif
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = err_events_l2_field_print,
		.mask = err_events_l2_nb_int_mask,
		.collect = al_err_events_common_field_empty_collect,
	},
	{
		.name = "Tag RAM Correctable Error",
		.id = AL_ERR_EVENTS_L2_TAG_CORRECTABLE_ERROR,
		.sev = AL_ERR_EVENTS_CORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_POLLING,
		.print = err_events_l2_field_print,
		.collect = al_err_events_common_field_empty_collect,
		.mask = al_err_events_common_field_empty_mask,
	},
	{
		.name = "Data RAM Correctable Error",
		.id = AL_ERR_EVENTS_L2_DATA_CORRECTABLE_ERROR,
		.sev = AL_ERR_EVENTS_CORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_POLLING,
		.print = err_events_l2_field_print,
		.collect = al_err_events_common_field_empty_collect,
		.mask = al_err_events_common_field_empty_mask,
	},
	{
		.name = "SCU snoop filter RAM Correctable Error",
		.id = AL_ERR_EVENTS_L2_SNOOP_FILTER_CORRECTABLE_ERROR,
		.sev = AL_ERR_EVENTS_CORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_POLLING,
		.print = err_events_l2_field_print,
		.collect = al_err_events_common_field_empty_collect,
		.mask = al_err_events_common_field_empty_mask,
	},
	{
		.name = "Tag RAM Uncorrectable Error",
		.id = AL_ERR_EVENTS_L2_TAG_UNCORRECTABLE_ERROR,
#if (AL_DEV_ID <= AL_DEV_ID_ALPINE_V2)
		.iofic_bit = NB_GLOBAL_NB_INT_CAUSE_CPU_INTERRIRQ,
#endif
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = err_events_l2_field_print,
		.mask = err_events_l2_nb_int_mask,
		.collect = al_err_events_common_field_empty_collect,
	},
	{
		.name = "Data RAM Uncorrectable Error",
		.id = AL_ERR_EVENTS_L2_DATA_UNCORRECTABLE_ERROR,
#if (AL_DEV_ID <= AL_DEV_ID_ALPINE_V2)
		.iofic_bit = NB_GLOBAL_NB_INT_CAUSE_CPU_INTERRIRQ,
#endif
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = err_events_l2_field_print,
		.mask = err_events_l2_nb_int_mask,
		.collect = al_err_events_common_field_empty_collect,
	},
	{
		.name = "SCU snoop filter RAM Uncorrectable Error",
		.id = AL_ERR_EVENTS_L2_SNOOP_FILTER_UNORRECTABLE_ERROR,
#if (AL_DEV_ID <= AL_DEV_ID_ALPINE_V2)
		.iofic_bit = NB_GLOBAL_NB_INT_CAUSE_CPU_INTERRIRQ,
#endif
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = err_events_l2_field_print,
		.mask = err_events_l2_nb_int_mask,
		.collect = al_err_events_common_field_empty_collect,
	},
	{
		.name = "Unknown Error",
		.id = AL_ERR_EVENTS_L2_UNKNOWN_ERROR,
		.sev = AL_ERR_EVENTS_CORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_POLLING,
		.print = err_events_l2_field_print,
		.collect = al_err_events_common_field_empty_collect,
		.mask = al_err_events_common_field_empty_mask,
	},
};

#if (AL_DEV_ID <= AL_DEV_ID_ALPINE_V2)
static const struct al_err_events_field
	al_err_events_sys_fabric_fields[AL_ERR_EVENTS_SYS_FABRIC_MAX_ERRORS] = {
	{
		.name = "DDR ECC Correctable Error",
		.id = AL_ERR_EVENTS_SYS_FABRIC_DDR_CORRECTABLE_ERROR,
		.iofic_bit = NB_GLOBAL_NB_INT_CAUSE_MCTL_ECC_CORR_ERR,
		.sev = AL_ERR_EVENTS_CORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_POLLING,
		.print = err_events_sf_ddr_corr_print,
		.collect = sys_fabric_ddr_correctable_collect,
		.mask = err_events_sys_fabric_nb_int_mask,
	},
	{
		.name = "DDR ECC Uncorrectable Error",
		.id = AL_ERR_EVENTS_SYS_FABRIC_DDR_UNCORRECTABLE_ERROR,
		.iofic_bit = NB_GLOBAL_NB_INT_CAUSE_MCTL_ECC_UNCORR_ERR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = err_events_sf_ddr_uncorr_print,
		.collect = sys_fabric_ddr_uncorrectable_collect,
		.mask = err_events_sys_fabric_nb_int_mask,
	},
	{
		.name = "DDR Parity Uncorrectable Error",
		.id = AL_ERR_EVENTS_SYS_FABRIC_DDR_PARITY_ERROR,
		.iofic_bit = NB_GLOBAL_NB_INT_CAUSE_MCTL_PARITY_ERR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = sys_fabric_ddr_parity_collect,
		.mask = err_events_sys_fabric_nb_int_mask,
	},
	{
		.name = "DDR On Chip Parity Error",
		.id = AL_ERR_EVENTS_SYS_FABRIC_DDR_ON_CHIP_PARITY_ERROR,
		.iofic_bit = NB_GLOBAL_NB_INT_CAUSE_ERR_CAUSE_SUM,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = err_events_sf_ddr_on_chip_parity_print,
		.collect = sys_fabric_ddr_on_chip_parity_collect,
		.mask = err_events_sys_fabric_error_cause_mask,
	},
	{
		.name = "AXI error",
		.id = AL_ERR_EVENTS_SYS_FABRIC_AXI_ERROR,
		.iofic_bit = NB_GLOBAL_NB_INT_CAUSE_CPU_AXIERRIRQ,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = sys_fabric_field_nb_int_error_collect,
		.mask = err_events_sys_fabric_nb_int_mask,
	},
	{
		.name = "Local GIC Error",
		.id = AL_ERR_EVENTS_SYS_FABRIC_LOCAL_GIC_ERROR,
		.iofic_bit = NB_GLOBAL_NB_INT_CAUSE_ERR_CAUSE_SUM,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = sys_fabric_error_int_collect,
		.mask = err_events_sys_fabric_error_cause_mask,
	},
	{
		.name = "MSIX Error",
		.id = AL_ERR_EVENTS_SYS_FABRIC_MSIX_ERROR,
		.iofic_bit = NB_GLOBAL_NB_INT_CAUSE_ERR_CAUSE_SUM,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = sys_fabric_error_int_collect,
		.mask = err_events_sys_fabric_error_cause_mask,
	},
	{
		.name = "SB PoS Error",
		.id = AL_ERR_EVENTS_SYS_FABRIC_SB_POS_ERROR,
		.iofic_bit = NB_GLOBAL_NB_INT_CAUSE_SB_POS_ERR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = err_events_sf_pos_error_print,
		.collect = sys_fabric_pos_error_collect,
		.mask = err_events_sys_fabric_nb_int_mask,
	},
	{
		.name = "CCI Error",
		.id = AL_ERR_EVENTS_SYS_FABRIC_CCI_ERROR,
		.iofic_bit = NB_GLOBAL_NB_INT_CAUSE_ACF_ERRORIRQ,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = sys_fabric_field_nb_int_error_collect,
		.mask = err_events_sys_fabric_nb_int_mask,
	},
};
#else

enum sys_fabric_v3_sfic_id {
	SYS_FABRIC_V3_SFIC_ID_ERR = 0,
	SYS_FABRIC_V3_SFIC_ID_NMI,
};

static const struct al_err_events_field
	al_err_events_sys_fabric_error_fields[AL_ERR_EVENTS_SYS_FABRIC_V3_SFIC_MAX_ERRORS] = {
	{
		.name = "CPU AXI Error 0",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_CPU_AXIERRIRQ_N_0,
		.iofic_id = SYS_FABRIC_V3_SFIC_ID_ERR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_ERR_GROUP_A_CPU_AXIERRIRQ_N_0,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "CPU AXI Error 1",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_CPU_AXIERRIRQ_N_1,
		.iofic_id = SYS_FABRIC_V3_SFIC_ID_ERR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_ERR_GROUP_A_CPU_AXIERRIRQ_N_1,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "CPU AXI Error 2",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_CPU_AXIERRIRQ_N_2,
		.iofic_id = SYS_FABRIC_V3_SFIC_ID_ERR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_ERR_GROUP_A_CPU_AXIERRIRQ_N_2,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "CPU AXI Error 3",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_CPU_AXIERRIRQ_N_3,
		.iofic_id = SYS_FABRIC_V3_SFIC_ID_ERR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_ERR_GROUP_A_CPU_AXIERRIRQ_N_3,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "CPU read data parity error 0",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_CPU_RD_DATA_PAR_ERR_0,
		.iofic_id = SYS_FABRIC_V3_SFIC_ID_ERR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_ERR_GROUP_A_CPU_RD_DATA_PAR_ERR_0,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "CPU read data parity error 1",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_CPU_RD_DATA_PAR_ERR_1,
		.iofic_id = SYS_FABRIC_V3_SFIC_ID_ERR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_ERR_GROUP_A_CPU_RD_DATA_PAR_ERR_1,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "CPU read data parity error 2",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_CPU_RD_DATA_PAR_ERR_2,
		.iofic_id = SYS_FABRIC_V3_SFIC_ID_ERR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_ERR_GROUP_A_CPU_RD_DATA_PAR_ERR_2,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "CPU read data parity error 3",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_CPU_RD_DATA_PAR_ERR_3,
		.iofic_id = SYS_FABRIC_V3_SFIC_ID_ERR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_ERR_GROUP_A_CPU_RD_DATA_PAR_ERR_3,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "CCI Error",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_CCI_ERRO_IRQ_N,
		.iofic_id = SYS_FABRIC_V3_SFIC_ID_ERR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_ERR_GROUP_A_CCI_ERRO_IRQ_N,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Coresight overflow interrupt",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_CORESIGHT_TSCNT_OVFL_INTR,
		.iofic_id = SYS_FABRIC_V3_SFIC_ID_ERR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_ERR_GROUP_A_CORESIGHT_TSCNT_OVFL_INTR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "GIC ECC fatal interrupt",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_GIC_ECC_FATAL,
		.iofic_id = SYS_FABRIC_V3_SFIC_ID_ERR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_ERR_GROUP_A_GIC_ECC_FATAL,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "MSIX error",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_FABRIC_MSIX_ERR_INTR,
		.iofic_id = SYS_FABRIC_V3_SFIC_ID_ERR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_ERR_GROUP_A_FABRIC_MSIX_ERR_INTR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "POS error",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_FABRIC_POS_ERR_IRQ,
		.iofic_id = SYS_FABRIC_V3_SFIC_ID_ERR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_ERR_GROUP_A_FABRIC_POS_ERR_IRQ,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = err_events_sf_pos_error_print,
		.collect = sys_fabric_pos_error_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "pp SRAM parity error",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_FABRIC_PP_SRAM_PARITY_ERR,
		.iofic_id = SYS_FABRIC_V3_SFIC_ID_ERR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_ERR_GROUP_A_FABRIC_PP_SRAM_PARITY_ERR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "CPU to SB write buffer parity error",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_FABRIC_C2SWB_PARITY_ERR,
		.iofic_id = SYS_FABRIC_V3_SFIC_ID_ERR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_ERR_GROUP_A_FABRIC_C2SWB_PARITY_ERR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "AXI read request parity error 0",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_FABRIC_SB_ARREQ_PAR_ERR_0,
		.iofic_id = SYS_FABRIC_V3_SFIC_ID_ERR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_ERR_GROUP_A_FABRIC_SB_ARREQ_PAR_ERR_0,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "AXI read request parity error 1",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_FABRIC_SB_ARREQ_PAR_ERR_1,
		.iofic_id = SYS_FABRIC_V3_SFIC_ID_ERR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_ERR_GROUP_A_FABRIC_SB_ARREQ_PAR_ERR_1,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "AXI read request parity error 2",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_FABRIC_SB_ARREQ_PAR_ERR_2,
		.iofic_id = SYS_FABRIC_V3_SFIC_ID_ERR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_ERR_GROUP_A_FABRIC_SB_ARREQ_PAR_ERR_2,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "AXI read request parity error 3",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_FABRIC_SB_ARREQ_PAR_ERR_3,
		.iofic_id = SYS_FABRIC_V3_SFIC_ID_ERR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_ERR_GROUP_A_FABRIC_SB_ARREQ_PAR_ERR_3,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "IOMMU read data parity error",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_IO_MMU_PTW_RD_DATA_PAR_ERR,
		.iofic_id = SYS_FABRIC_V3_SFIC_ID_ERR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_ERR_GROUP_A_IO_MMU_PTW_RD_DATA_PAR_ERR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "IOCACHE uncorrectable ECC error 0",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_IO_CACHE_MEMORY_UNCORR_ECC_0,
		.iofic_id = SYS_FABRIC_V3_SFIC_ID_ERR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_ERR_GROUP_A_IO_CACHE_MEMORY_UNCORR_ECC_0,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "IOCACHE uncorrectable ECC error 1",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_IO_CACHE_MEMORY_UNCORR_ECC_1,
		.iofic_id = SYS_FABRIC_V3_SFIC_ID_ERR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_ERR_GROUP_A_IO_CACHE_MEMORY_UNCORR_ECC_1,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "IOCACHE correctable ECC error 0",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_IO_CACHE_MEMORY_CORR_ECC_0,
		.iofic_id = SYS_FABRIC_V3_SFIC_ID_ERR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_ERR_GROUP_A_IO_CACHE_MEMORY_CORR_ECC_0,
		.sev = AL_ERR_EVENTS_CORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "IOCACHE correctable ECC error 1",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_IO_CACHE_MEMORY_CORR_ECC_1,
		.iofic_id = SYS_FABRIC_V3_SFIC_ID_ERR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_ERR_GROUP_A_IO_CACHE_MEMORY_CORR_ECC_1,
		.sev = AL_ERR_EVENTS_CORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "IOCACHE parity error 0",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_IO_CACHE_PARITY_ERROR_0,
		.iofic_id = SYS_FABRIC_V3_SFIC_ID_ERR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_ERR_GROUP_A_IO_CACHE_PARITY_ERROR_0,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "IOCACHE parity error 1",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_IO_CACHE_PARITY_ERROR_1,
		.iofic_id = SYS_FABRIC_V3_SFIC_ID_ERR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_ERR_GROUP_A_IO_CACHE_PARITY_ERROR_1,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "DDR channel 0 onchip error",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_DDR_ONCHIP_PARITY_0,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_ERR_GROUP_A_DDR_CAUSE_SUM_0,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = err_events_sf_ddr_on_chip_parity_print,
		.collect = sys_fabric_ddr_on_chip_parity_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "DDR channel 1 onchip error",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_DDR_ONCHIP_PARITY_1,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_ERR_GROUP_A_DDR_CAUSE_SUM_1,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = err_events_sf_ddr_on_chip_parity_print,
		.collect = sys_fabric_ddr_on_chip_parity_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "DDR channel 0 uncorrectable ECC",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_DDR_CHANNEL_0_UNCORRECTABLE_ECC,
		.iofic_id = SYS_FABRIC_V3_SFIC_ID_NMI,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_NMI_GROUP_A_DDR_CHANNEL_0_UNCORRECTABLE_ECC,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = err_events_sf_ddr_uncorr_print,
		.collect = sys_fabric_ddr_uncorrectable_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "DDR channel 0 correctable ECC",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_DDR_CHANNEL_0_CORRECTABLE_ECC,
		.iofic_id = SYS_FABRIC_V3_SFIC_ID_NMI,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_NMI_GROUP_A_DDR_CHANNEL_0_CORRECTABLE_ECC,
		.sev = AL_ERR_EVENTS_CORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_POLLING,
		.print = err_events_sf_ddr_corr_print,
		.collect = sys_fabric_ddr_correctable_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "DDR channel 0 dfi_alert_n CRC",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_DDR_CHANNEL_0_DFI_ALERT_N_CRC,
		.iofic_id = SYS_FABRIC_V3_SFIC_ID_NMI,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_NMI_GROUP_A_DDR_CHANNEL_0_DFI_ALERT_N_CRC,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "DDR channel 0 max alert_n reached CRC",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_DDR_CHANNEL_0_MAX_ALERT_N_REACHED_CRC,
		.iofic_id = SYS_FABRIC_V3_SFIC_ID_NMI,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_NMI_GROUP_A_DDR_CHANNEL_0_MAX_ALERT_N_REACHED_CRC,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "DDR channel 1 uncorrectable ECC",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_DDR_CHANNEL_1_UNCORRECTABLE_ECC,
		.iofic_id = SYS_FABRIC_V3_SFIC_ID_NMI,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_NMI_GROUP_A_DDR_CHANNEL_1_UNCORRECTABLE_ECC,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = err_events_sf_ddr_uncorr_print,
		.collect = sys_fabric_ddr_uncorrectable_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "DDR channel 1 correctable ECC",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_DDR_CHANNEL_1_CORRECTABLE_ECC,
		.iofic_id = SYS_FABRIC_V3_SFIC_ID_NMI,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_NMI_GROUP_A_DDR_CHANNEL_1_CORRECTABLE_ECC,
		.sev = AL_ERR_EVENTS_CORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_POLLING,
		.print = err_events_sf_ddr_corr_print,
		.collect = sys_fabric_ddr_correctable_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "DDR channel 1 dfi_alert_n CRC",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_DDR_CHANNEL_1_DFI_ALERT_N_CRC,
		.iofic_id = SYS_FABRIC_V3_SFIC_ID_NMI,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_NMI_GROUP_A_DDR_CHANNEL_1_DFI_ALERT_N_CRC,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "DDR channel 1 max alert_n reached CRC",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_DDR_CHANNEL_1_MAX_ALERT_N_REACHED_CRC,
		.iofic_id = SYS_FABRIC_V3_SFIC_ID_NMI,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_NMI_GROUP_A_DDR_CHANNEL_1_MAX_ALERT_N_REACHED_CRC,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "CCI sf_parerr",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_CCI_SF_PARERR,
		.iofic_id = SYS_FABRIC_V3_SFIC_ID_NMI,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_NMI_GROUP_A_CCI_SF_PARERR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "DDR channel 0 par_wdata_out_err",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_DDR_CHANNEL_0_PAR_WDATA_OUT_ERR,
		.iofic_id = SYS_FABRIC_V3_SFIC_ID_NMI,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_NMI_GROUP_A_DDR_CHANNEL_0_PAR_WDATA_OUT_ERR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "DDR channel 1 par_wdata_out_err",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_DDR_CHANNEL_1_PAR_WDATA_OUT_ERR,
		.iofic_id = SYS_FABRIC_V3_SFIC_ID_NMI,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_NMI_GROUP_A_DDR_CHANNEL_1_PAR_WDATA_OUT_ERR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "PCIe DEV 0 fatal 0",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_PCIE_DEV_0_FATAL_0,
		.iofic_id = SYS_FABRIC_V3_SFIC_ID_NMI,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_NMI_GROUP_A_PCIE_DEV_0_FATAL_0,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "PCIe DEV 0 fatal 1",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_PCIE_DEV_0_FATAL_1,
		.iofic_id = SYS_FABRIC_V3_SFIC_ID_NMI,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_NMI_GROUP_A_PCIE_DEV_0_FATAL_1,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "PCIe DEV 0 fatal 2",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_PCIE_DEV_0_FATAL_2,
		.iofic_id = SYS_FABRIC_V3_SFIC_ID_NMI,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_NMI_GROUP_A_PCIE_DEV_0_FATAL_2,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "PCIe DEV 0 fatal 3",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_PCIE_DEV_0_FATAL_3,
		.iofic_id = SYS_FABRIC_V3_SFIC_ID_NMI,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_NMI_GROUP_A_PCIE_DEV_0_FATAL_3,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "PCIe DEV 0 fatal 4",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_PCIE_DEV_0_FATAL_4,
		.iofic_id = SYS_FABRIC_V3_SFIC_ID_NMI,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_NMI_GROUP_A_PCIE_DEV_0_FATAL_4,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "PCIe DEV 0 fatal 5",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_PCIE_DEV_0_FATAL_5,
		.iofic_id = SYS_FABRIC_V3_SFIC_ID_NMI,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_NMI_GROUP_A_PCIE_DEV_0_FATAL_5,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "PCIe DEV 0 fatal 6",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_PCIE_DEV_0_FATAL_6,
		.iofic_id = SYS_FABRIC_V3_SFIC_ID_NMI,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_NMI_GROUP_A_PCIE_DEV_0_FATAL_6,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "PCIe DEV 0 fatal 7",
		.id = AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_PCIE_DEV_0_FATAL_7,
		.iofic_id = SYS_FABRIC_V3_SFIC_ID_NMI,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SYS_FABRIC_SFIC_NMI_GROUP_A_PCIE_DEV_0_FATAL_7,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
};
#endif

/*******************************************************************************
 ** L1
 ******************************************************************************/
static int err_events_l1_collect(struct al_err_events_module *module,
				 enum al_err_events_collect collect)
{
	struct al_err_events_field *field;
	struct al_err_events_l1_data *data = (struct al_err_events_l1_data *)module->private_data;
	struct al_cpu_mem_err_syndrome *syndrome = &data->syndrome;
	unsigned int cpu_id = al_get_cpu_id();

	if (collect == AL_ERR_EVENTS_COLLECT_INTERRUPT)
		return 0;

	if (cpu_id != module->primary_index)
		return 0;

	al_cpu_mem_err_syndrome_get(syndrome);

	if (!syndrome->valid)
		return 0;

	al_cpu_mem_err_syndrome_clear();

	switch (syndrome->ram_id) {
	case AL_CPU_SYS_FABRIC_CPU_RAM_ID_L1_I_TAG:
		if (!syndrome->fatal)
			field = &module->fields[AL_ERR_EVENTS_L1_ITAG_CORRECTABLE_ERROR];
		else
			field = &module->fields[AL_ERR_EVENTS_L1_ITAG_UNCORRECTABLE_ERROR];
		break;
	case AL_CPU_SYS_FABRIC_CPU_RAM_ID_L1_I_DATA:
		if (!syndrome->fatal)
			field = &module->fields[AL_ERR_EVENTS_L1_IDATA_CORRECTABLE_ERROR];
		else
			field = &module->fields[AL_ERR_EVENTS_L1_IDATA_UNCORRECTABLE_ERROR];
		break;
	case AL_CPU_SYS_FABRIC_CPU_RAM_ID_L1_D_TAG:
		if (!syndrome->fatal)
			field = &module->fields[AL_ERR_EVENTS_L1_DTAG_CORRECTABLE_ERROR];
		else
			field = &module->fields[AL_ERR_EVENTS_L1_DTAG_UNCORRECTABLE_ERROR];
		break;
	case AL_CPU_SYS_FABRIC_CPU_RAM_ID_L1_D_DATA:
		if (!syndrome->fatal)
			field = &module->fields[AL_ERR_EVENTS_L1_DDATA_CORRECTABLE_ERROR];
		else
			field = &module->fields[AL_ERR_EVENTS_L1_DDATA_UNCORRECTABLE_ERROR];
		break;
	case AL_CPU_SYS_FABRIC_CPU_RAM_ID_L2_TLB:
		if (!syndrome->fatal)
			field = &module->fields[AL_ERR_EVENTS_L1_L2_TLB_CORRECTABLE_ERROR];
		else
			field = &module->fields[AL_ERR_EVENTS_L1_L2_TLB_UNCORRECTABLE_ERROR];
		break;
	default:
		al_err("%s: unknown ram_id [%d]\n", __func__, syndrome->ram_id);
		return -EFAULT;
	}

	/* First error is counted by 'valid', while 'repeat' can be 0 */
	if (field->valid)
		al_err_events_common_field_error(field, 1 + syndrome->err_cnt_repeat);

	/*
	 * Other error count:
	 * error that does not match the RAMID, bank, way, or index information
	 * in this register while the sticky Valid bit is set
	 *
	 * since we have no way of knowing what is the error cause we mark it as unknown
	 */
	if (syndrome->err_cnt_other) {
		field = &module->fields[AL_ERR_EVENTS_L1_UNKNOWN_ERROR];

		if (field->valid)
			al_err_events_common_field_error(field, syndrome->err_cnt_other);
	}

	if (syndrome->fatal)
		field = &module->fields[AL_ERR_EVENTS_L1_UNCORRECTABLE_ERROR];
	else
		field = &module->fields[AL_ERR_EVENTS_L1_CORRECTABLE_ERROR];

	/* First error is counted by 'valid', while 'repeat' and 'other' can be 0 */
	if (field->valid)
		al_err_events_common_field_error(field,
						 1 + syndrome->err_cnt_repeat +
						 syndrome->err_cnt_other);

	return 0;
}

static void err_events_l1_clear(struct al_err_events_module *module)
{
	struct al_err_events_l1_data *data = (struct al_err_events_l1_data *)module->private_data;

	al_err_events_common_module_clear(module);

	al_memset(&data->syndrome, 0, sizeof(struct al_cpu_mem_err_syndrome));
}

static void err_events_l1_print_syndrome(al_err_events_print_cb print_cb,
					 struct al_cpu_mem_err_syndrome *syndrome)
{
	if (syndrome->valid) {
		print_cb("\tFatal: %d\n", syndrome->fatal);
		print_cb("\tRepeat: %d\n", syndrome->err_cnt_repeat);
		print_cb("\tOther: %d\n", syndrome->err_cnt_other);
		print_cb("\tRAM Id: %d\n", syndrome->ram_id);
		print_cb("\tBank/Way: %d\n", syndrome->bank_way);
		print_cb("\tIndex: %d\n", syndrome->index);
	}
}

static void err_events_l1_field_print(struct al_err_events_field *field, al_bool print_header)
{
	struct al_err_events_module *module = field->parent_module;
	struct al_err_events_l1_data *data = (struct al_err_events_l1_data *)module->private_data;
	al_err_events_print_cb print_cb = module->handle->print_cb;

	if (print_header) {
		if (field->valid) {
			print_cb("%s %s: %d\n", module->name, field->name, field->counter);
			err_events_l1_print_syndrome(print_cb, &data->syndrome);
		} else
			print_cb("%s %s: Ignored\n", module->name, field->name);
	} else {
		if (field->valid)
			print_cb("\t%s: %d\n", field->name, field->counter);
		else
			print_cb("\t%s: Ignored\n", field->name);
	}
}

static void err_events_l1_print(struct al_err_events_module *module)
{
	struct al_err_events_l1_data *data = (struct al_err_events_l1_data *)module->private_data;
	struct al_cpu_mem_err_syndrome *syndrome = &data->syndrome;
	al_err_events_print_cb print_cb = module->handle->print_cb;

	al_err_events_common_module_print(module);

	print_cb("\tLast syndrome: %s\n", (syndrome->valid ? "Valid" : "Invalid"));

	err_events_l1_print_syndrome(print_cb, syndrome);
}

int al_err_events_l1_init(struct al_err_events_handle *handle,
			  struct al_err_events_l1_data *data,
			  struct al_err_events_l1_init_params *params)
{
	struct al_err_events_common_module_params module_params;
	int i;

	al_assert(handle);
	al_assert(data);
	al_assert(params);
	al_assert(sizeof(al_err_events_l1_fields) == sizeof(data->fields));
	al_assert(sizeof(al_err_events_l1_fields) ==
		 (AL_ERR_EVENTS_L1_MAX_ERRORS * sizeof(struct al_err_events_field)));

	al_memset(&module_params, 0, sizeof(module_params));

	/* init module */
	al_sprintf(module_params.name, "CPU(%d)-L1-Cache", params->cpu_id);
	module_params.primary_module = AL_ERR_EVENTS_MODULE_L1;
	module_params.secondary_module = AL_ERR_EVENTS_SEC_MODULE_NONE;
	module_params.primary_index = params->cpu_id;
	module_params.secondary_index = 0;
	module_params.collect_mode = params->collect_mode;
	for (i = 0; i < AL_ERR_EVENTS_IC_MAX; i++)
		module_params.ic_regs_bases[i] = NULL;
	module_params.ic_size = 0;
	module_params.fields_size = AL_ERR_EVENTS_L1_MAX_ERRORS;
	module_params.fields = data->fields;
	module_params.fields_template = al_err_events_l1_fields;
	module_params.private_data = data;
	module_params.collect = err_events_l1_collect;
	module_params.print = err_events_l1_print;
	module_params.clear = err_events_l1_clear;
	module_params.test = NULL;

	al_err_events_common_module_init(handle, &data->module, &module_params);

	al_err_events_common_module_post_init(&data->module);

	return 0;
}

/*******************************************************************************
 ** L2
 ******************************************************************************/
#if AL_DEV_ID <= AL_DEV_ID_ALPINE_V2
static void err_events_l2_nb_int_mask(struct al_err_events_field *field)
{
	unsigned int i;
	struct al_err_events_module *module = field->parent_module;
	struct al_err_events_l2_data *data = (struct al_err_events_l2_data *)module->private_data;
	uint32_t local_int_mask = data->local_int_mask;

	for (i = 0; local_int_mask; i++, local_int_mask >>= 1) {
		if (!(local_int_mask & 1))
			continue;

		al_sys_fabric_int_local_mask(&data->handle, i, field->iofic_bit);
	}
}
#else
static void err_events_l2_nb_int_mask(struct al_err_events_field *field)
{
	struct al_err_events_module *module = field->parent_module;
	struct al_err_events_l2_data *data =
			(struct al_err_events_l2_data *)module->private_data;
	void __iomem *regs_base = al_sys_fabric_nmi_sfic_regs_base_get(&data->handle);

	al_iofic_mask(regs_base, AL_INT_GROUP_A, data->nmi_sfic_bit);
}
#endif

static int err_events_l2_collect_syndrome(struct al_err_events_module *module)
{
	struct al_err_events_field *field;
	struct al_err_events_l2_data *data = (struct al_err_events_l2_data *)module->private_data;
	struct al_l2_mem_err_syndrome *syndrome = &data->syndrome;

	al_l2_mem_err_syndrome_get(syndrome);

	if (!syndrome->valid)
		return -EINVAL;

	al_l2_mem_err_syndrome_clear();

	switch (syndrome->ram_id) {
	case AL_L2_SYS_FABRIC_L2_RAM_ID_L2_TAG:
		if (!syndrome->fatal)
			field = &module->fields[AL_ERR_EVENTS_L2_TAG_CORRECTABLE_ERROR];
		else
			field = &module->fields[AL_ERR_EVENTS_L2_TAG_UNCORRECTABLE_ERROR];
		break;
	case AL_L2_SYS_FABRIC_L2_RAM_ID_L2_DATA:
		if (!syndrome->fatal)
			field = &module->fields[AL_ERR_EVENTS_L2_DATA_CORRECTABLE_ERROR];
		else
			field = &module->fields[AL_ERR_EVENTS_L2_DATA_UNCORRECTABLE_ERROR];
		break;
	case AL_L2_SYS_FABRIC_L2_RAM_ID_L2_SNP_TAG:
		if (!syndrome->fatal)
			field = &module->fields[AL_ERR_EVENTS_L2_SNOOP_FILTER_CORRECTABLE_ERROR];
		else
			field = &module->fields[AL_ERR_EVENTS_L2_SNOOP_FILTER_UNORRECTABLE_ERROR];
		break;
	default:
		al_err("%s: unknown ram_id [%d]\n", __func__, syndrome->ram_id);
		return -EFAULT;
	}

	/* First error is counted by 'valid', while 'repeat' can be 0 */
	if (field->valid)
		al_err_events_common_field_error(field, 1 + syndrome->err_cnt_repeat);

	/*
	 * Other error count:
	 * memory error that does not match the RAMID and Bank/Way information
	 * in this register while the sticky Valid bit is set
	 *
	 * since we have no way of knowing what is the error cause we mark it as unknown
	 */
	if (syndrome->err_cnt_other) {
		field = &module->fields[AL_ERR_EVENTS_L2_UNKNOWN_ERROR];

		if (field->valid)
			al_err_events_common_field_error(field, syndrome->err_cnt_other);
	}

	if (syndrome->fatal)
		field = &module->fields[AL_ERR_EVENTS_L2_UNCORRECTABLE_ERROR];
	else
		field = &module->fields[AL_ERR_EVENTS_L2_CORRECTABLE_ERROR];

	/* First error is counted by 'valid', while 'repeat' and 'other' can be 0 */
	if (field->valid)
		al_err_events_common_field_error(field,
						 1 + syndrome->err_cnt_repeat +
						 syndrome->err_cnt_other);

	return 0;
}

static struct al_err_events_field *l2_find_uncorr_en_field(struct al_err_events_module *module,
							   enum al_err_events_collect collect)
{
	unsigned int i;
	enum al_err_events_l2 uncorr_l2_types[] = {
		AL_ERR_EVENTS_L2_UNCORRECTABLE_ERROR,
		AL_ERR_EVENTS_L2_TAG_UNCORRECTABLE_ERROR,
		AL_ERR_EVENTS_L2_DATA_UNCORRECTABLE_ERROR,
		AL_ERR_EVENTS_L2_SNOOP_FILTER_UNORRECTABLE_ERROR,
		AL_ERR_EVENTS_L2_UNKNOWN_ERROR,
	};

	for (i = 0; i < AL_ARR_SIZE(uncorr_l2_types); i++) {
		enum al_err_events_l2 l2_type = uncorr_l2_types[i];
		struct al_err_events_field *field =  &module->fields[l2_type];

		if (field->valid && field->collect_mode == collect)
			return field;
	}

	return NULL;
}

#if AL_DEV_ID < AL_DEV_ID_ALPINE_V3
static int l2_collect_intr_ctx(struct al_err_events_module *module,
				 enum al_err_events_collect collect)
{
	struct al_err_events_l2_data *data =
		(struct al_err_events_l2_data *)module->private_data;
	unsigned int i;
	struct al_err_events_field *field;
	uint32_t nb_cause;
	uint32_t local_int_mask = 0;
	uint32_t local_mask = data->local_int_mask;

	field = l2_find_uncorr_en_field(module, collect);
	if (!field)
		return 0;

	for (i = 0; local_mask; i++, local_mask >>= 1) {
		if (!(local_mask & 1))
			continue;

		local_int_mask |= al_sys_fabric_int_local_mask_read(&data->handle, i);
	}

	if (!(local_int_mask & field->iofic_bit))
		return 0;

	al_sys_fabric_int_cause_read(&data->handle, &nb_cause);
	if (nb_cause & field->iofic_bit) {
		int err = err_events_l2_collect_syndrome(module);
		if (err)
			al_err("%s: ERROR got L2 interrupt and syndrome is not valid\n",
			       __func__);

		al_sys_fabric_int_clear(&data->handle,
					~field->iofic_bit);
	}

	return 0;
}
#else
static int l2_collect_intr_ctx(struct al_err_events_module *module,
				 enum al_err_events_collect collect)
{
	int err = 0;
	struct al_err_events_l2_data *data =
		(struct al_err_events_l2_data *)module->private_data;
	void __iomem *nmi_sfic = al_sys_fabric_nmi_sfic_regs_base_get(&data->handle);
	unsigned int cluster_id = data->cluster_id;
	struct al_err_events_field *field;

	/** Interrupt (abort handler) flow - collect the uncorrectable error */

	if (cluster_id != module->primary_index) {
		al_err("%s: ERROR Can't collect L2 for cluster %d from cluster %d on "
			"interrupt collect mode\n",
			__func__, module->primary_index, cluster_id);
		return -EINVAL;
	}

	field = l2_find_uncorr_en_field(module, collect);
	if (!field)
		return 0;

	if (al_iofic_read_cause(nmi_sfic, AL_INT_GROUP_A) & data->nmi_sfic_bit) {
		err = err_events_l2_collect_syndrome(module);
		if (err)
			al_err("%s: ERROR got L2 interrupt and syndrome is not valid\n", __func__);

		al_iofic_clear_cause(nmi_sfic, AL_INT_GROUP_A, data->nmi_sfic_bit);
	}

	return err;
}
#endif

static int err_events_l2_collect(struct al_err_events_module *module,
				 enum al_err_events_collect collect)
{
#if AL_DEV_ID >= AL_DEV_ID_ALPINE_V3
	unsigned int cluster_id = al_get_cluster_id();

	/**
	 * Polling flow - polling collect function defines what modules to collect on.
	 *
	 * Interrupt flow - There are 2 main use cases
	 * 1. L2 module interrupt handler
	 * 2. Abort handler - typically calls al_err_events_handle_collect(), iterating
	 *    over all modules registered on the handle and collecting errors
	 *
	 * In both cases should ensure that collecting CPU is collecting its own cluster errors
	 */

	if (cluster_id != module->primary_index)
		return 0;
#endif

	if (collect == AL_ERR_EVENTS_COLLECT_INTERRUPT)
		return l2_collect_intr_ctx(module, collect);
	else
		err_events_l2_collect_syndrome(module);

	return 0;
}

static void err_events_l2_print_syndrome(al_err_events_print_cb print_cb,
					 struct al_l2_mem_err_syndrome *syndrome)
{
	if (syndrome->valid) {
		print_cb("\tFatal: %d\n", syndrome->fatal);
		print_cb("\tRepeat: %d\n", syndrome->err_cnt_repeat);
		print_cb("\tOther: %d\n", syndrome->err_cnt_other);
		print_cb("\tRAM Id: %d\n", syndrome->ram_id);
		print_cb("\tCPU Id Way: %d\n", syndrome->cpuid_way);
		print_cb("\tIndex: %d\n", syndrome->index);
	}
}

static void err_events_l2_field_print(struct al_err_events_field *field, al_bool print_header)
{
	struct al_err_events_module *module = field->parent_module;
	struct al_err_events_l2_data *data = (struct al_err_events_l2_data *)module->private_data;
	al_err_events_print_cb print_cb = module->handle->print_cb;

	if (print_header) {
		if (field->valid) {
			print_cb("%s %s: %d\n", module->name, field->name, field->counter);
			err_events_l2_print_syndrome(print_cb, &data->syndrome);
		} else
			print_cb("%s %s: Ignored\n", module->name, field->name);
	} else {
		if (field->valid)
			print_cb("\t%s: %d\n", field->name, field->counter);
		else
			print_cb("\t%s: Ignored\n", field->name);
	}
}

static void err_events_l2_print(struct al_err_events_module *module)
{
	struct al_err_events_l2_data *data = (struct al_err_events_l2_data *)module->private_data;
	struct al_l2_mem_err_syndrome *syndrome = &data->syndrome;
	al_err_events_print_cb print_cb = module->handle->print_cb;

	al_err_events_common_module_print(module);

	print_cb("\tLast syndrome: %s\n", (syndrome->valid ? "Valid" : "Invalid"));

	err_events_l2_print_syndrome(print_cb, syndrome);
}

static void err_events_l2_clear(struct al_err_events_module *module)
{
	struct al_err_events_l2_data *data = (struct al_err_events_l2_data *)module->private_data;

	al_err_events_common_module_clear(module);

	al_memset(&data->syndrome, 0, sizeof(struct al_l2_mem_err_syndrome));
}

#define AL_ERR_EVENTS_V3_L2_NUM_CLUSTERS 4

int al_err_events_l2_init(struct al_err_events_handle *handle,
			  struct al_err_events_l2_data *data,
			  struct al_err_events_l2_init_params *params)
{
	struct al_err_events_common_module_params module_params;
	unsigned int i;
	unsigned int cluster_id;
	uint32_t iofic_bit[AL_ERR_EVENTS_V3_L2_NUM_CLUSTERS] = {
		AL_SYS_FABRIC_SFIC_NMI_GROUP_A_CPU_INTR_ERR_N0,
		AL_SYS_FABRIC_SFIC_NMI_GROUP_A_CPU_INTR_ERR_N1,
		AL_SYS_FABRIC_SFIC_NMI_GROUP_A_CPU_INTR_ERR_N2,
		AL_SYS_FABRIC_SFIC_NMI_GROUP_A_CPU_INTR_ERR_N3,
	};

	al_assert(handle);
	al_assert(data);
	al_assert(params);
	al_assert(sizeof(al_err_events_l2_fields) == sizeof(data->fields));
	al_assert(sizeof(al_err_events_l2_fields) ==
		 (AL_ERR_EVENTS_L2_MAX_ERRORS * sizeof(struct al_err_events_field)));
#if AL_DEV_ID >= AL_DEV_ID_ALPINE_V3
	al_assert(params->cluster_id < AL_ERR_EVENTS_V3_L2_NUM_CLUSTERS);
#endif
	/* init data */
	al_sys_fabric_handle_init(&data->handle, params->nb_regs_base);

	/* init module */
	al_memset(&module_params, 0, sizeof(module_params));
#if AL_DEV_ID < AL_DEV_ID_ALPINE_V3
	cluster_id = 0;
	al_sprintf(module_params.name, "L2-Cache");
#else
	cluster_id = params->cluster_id;
	al_sprintf(module_params.name, "Cluster(%d) L2-Cache", params->cluster_id);
#endif
	data->nmi_sfic_bit = iofic_bit[params->cluster_id];

	module_params.primary_module = AL_ERR_EVENTS_MODULE_L2;
	module_params.secondary_module = AL_ERR_EVENTS_SEC_MODULE_NONE;
	module_params.primary_index = cluster_id;
	module_params.secondary_index = 0;
	module_params.collect_mode = params->collect_mode;
	for (i = 0; i < AL_ERR_EVENTS_IC_MAX; i++)
		module_params.ic_regs_bases[i] = NULL;
	module_params.ic_size = 0;
	module_params.fields_size = AL_ERR_EVENTS_L2_MAX_ERRORS;
	module_params.fields = data->fields;
	module_params.fields_template = al_err_events_l2_fields;
	module_params.private_data = data;
	module_params.collect = err_events_l2_collect;
	module_params.print = err_events_l2_print;
	module_params.clear = err_events_l2_clear;
	module_params.test = NULL;

	al_err_events_common_module_init(handle, &data->module, &module_params);

	al_err_events_common_module_post_init(&data->module);

	return 0;
}

int al_err_events_l2_ints_enable(struct al_err_events_l2_data *data,
				 unsigned int local_int_mask __attribute__((__unused__)))
{
#if AL_DEV_ID < AL_DEV_ID_ALPINE_V3
	unsigned int i;
	uint32_t int_mask = 0;
	uint32_t int_unmask = 0;
	struct al_err_events_module *module;
	struct al_err_events_field *field;

	al_assert(data);

	module = &data->module;

	if (!module->enabled)
		return -EINVAL;

	field = &module->fields[AL_ERR_EVENTS_L2_UNCORRECTABLE_ERROR];
	if (field->valid && (field->collect_mode == AL_ERR_EVENTS_COLLECT_INTERRUPT)) {
		int_unmask = field->iofic_bit;

		/* on V1 this interrupts bits are on after reset */
		if (data->handle.ver == NB_NB_VERSION_VERSION_RELEASE_NUM_MAJOR_VAL_ALPINE_V1)
			al_sys_fabric_int_clear(&data->handle, ~field->iofic_bit);
	} else {
		int_mask = field->iofic_bit;
	}

	data->local_int_mask = local_int_mask;

	/* mask/unmask sys fabric local interrupts */
	for (i = 0; local_int_mask; i++, local_int_mask >>= 1) {
		if (!(local_int_mask & 1))
			continue;

		al_sys_fabric_int_local_mask(&data->handle, i, int_mask);
		al_sys_fabric_int_local_unmask(&data->handle, i, int_unmask);
	}
#else
	struct al_err_events_module *module;

	al_assert(data);

	module = &data->module;

	if (!module->enabled)
		return -EINVAL;

	/* unmask interrupts for this module cluster */
	al_iofic_unmask(data->handle.nmi_sfic_base, AL_INT_GROUP_A, data->nmi_sfic_bit);
#endif
	return 0;
}

/*******************************************************************************
 ** System Fabric
 ******************************************************************************/
#if (AL_DEV_ID <= AL_DEV_ID_ALPINE_V2)
static uint32_t al_err_events_sys_fabric_get_error_bit(struct al_err_events_field *field)
{
	switch (field->id) {
	case AL_ERR_EVENTS_SYS_FABRIC_MSIX_ERROR:
		return  NB_GLOBAL_ERROR_CAUSE_MSIX_ERR_INT;
	case AL_ERR_EVENTS_SYS_FABRIC_LOCAL_GIC_ERROR:
		return NB_GLOBAL_ERROR_CAUSE_LOCAL_GIC_ECC_FATAL;
	case  AL_ERR_EVENTS_SYS_FABRIC_DDR_ON_CHIP_PARITY_ERROR:
		return NB_GLOBAL_ERROR_CAUSE_DDR_CAUSE_SUM;
	default:
		return 0;
	}
}


static void sys_fabric_ddr_parity_collect(struct al_err_events_field *field,
					  uint32_t cause)
{
	if (cause & field->iofic_bit) {
		struct al_err_events_sys_fabric_data *data =
			(struct al_err_events_sys_fabric_data *)field->parent_module->private_data;
		unsigned int counter = al_ddr_parity_count_get(data->ddr_cfg[0]);

		al_ddr_parity_count_clear(data->ddr_cfg[0]);

		al_err_events_common_field_error(field, counter);

		al_ddr_parity_int_clear(data->ddr_cfg[0]);
	}
}

static void sys_fabric_error_int_collect(struct al_err_events_field *field,
					 uint32_t cause)
{
	uint32_t error_bit = al_err_events_sys_fabric_get_error_bit(field);

	if (cause & error_bit) {
		struct al_err_events_sys_fabric_data *data;

		al_err_events_common_field_error(field, 1);

		data = (struct al_err_events_sys_fabric_data *)field->parent_module->private_data;

		al_sys_fabric_error_int_clear(&data->handle,
					      error_bit);

		al_sys_fabric_int_clear(&data->handle,
					~field->iofic_bit);
	}
}

static void sys_fabric_field_nb_int_error_collect(struct al_err_events_field *field,
						  uint32_t cause)
{
	if (field->iofic_bit & cause) {
		struct al_err_events_sys_fabric_data *data;

		al_err_events_common_field_error(field, 1);

		data = (struct al_err_events_sys_fabric_data *)field->parent_module->private_data;

		al_sys_fabric_int_clear(&data->handle,
					~field->iofic_bit);
	}
}

static void err_events_sys_fabric_nb_int_mask(struct al_err_events_field *field)
{
	unsigned int i;
	struct al_err_events_module *module = field->parent_module;
	struct al_err_events_sys_fabric_data *data =
			(struct al_err_events_sys_fabric_data *)module->private_data;
	uint32_t local_int_mask = data->local_int_mask;

	for (i = 0; local_int_mask; i++, local_int_mask >>= 1) {
		if (!(local_int_mask & 1))
			continue;

		al_sys_fabric_int_local_mask(&data->handle, i, field->iofic_bit);
	}
}

static void err_events_sys_fabric_error_cause_mask(struct al_err_events_field *field)
{
	struct al_err_events_module *module = field->parent_module;
	struct al_err_events_sys_fabric_data *data =
			(struct al_err_events_sys_fabric_data *)module->private_data;

	al_sys_fabric_error_int_mask(&data->handle, al_err_events_sys_fabric_get_error_bit(field));
}

#else
static uint32_t al_err_events_sys_fabric_get_error_bit(struct al_err_events_field *field)
{
	return field->iofic_bit;
}

#endif

static unsigned int sys_fabric_ddr_channel_get(struct al_err_events_field *field)
{
	struct al_err_events_module *module = field->parent_module;
	struct al_err_events_sys_fabric_data *data = module->private_data;

	if (data->supported_ddr_ch == 1)
		return 0;

#if AL_DEV_ID >= AL_DEV_ID_ALPINE_V3
	switch (field->id) {
	case AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_DDR_CHANNEL_0_UNCORRECTABLE_ECC:
	case AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_DDR_CHANNEL_0_CORRECTABLE_ECC:
	case AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_DDR_CHANNEL_0_DFI_ALERT_N_CRC:
	case AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_DDR_CHANNEL_0_MAX_ALERT_N_REACHED_CRC:
	case AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_DDR_CHANNEL_0_PAR_WDATA_OUT_ERR:
		return 0;
	case AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_DDR_CHANNEL_1_UNCORRECTABLE_ECC:
	case AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_DDR_CHANNEL_1_CORRECTABLE_ECC:
	case AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_DDR_CHANNEL_1_DFI_ALERT_N_CRC:
	case AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_DDR_CHANNEL_1_MAX_ALERT_N_REACHED_CRC:
	case AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_DDR_CHANNEL_1_PAR_WDATA_OUT_ERR:
		return 1;
	}
#endif
	return 0;
}

static void sys_fabric_ddr_correctable_collect(struct al_err_events_field *field,
					       uint32_t cause)
{
	if (cause & field->iofic_bit) {
		unsigned int counter;
		unsigned int ch = sys_fabric_ddr_channel_get(field);
		struct al_err_events_sys_fabric_data *data =
			(struct al_err_events_sys_fabric_data *)field->parent_module->private_data;

		al_ddr_ecc_status_get(data->ddr_cfg[ch],
				      &data->ddr_corr_status,
				      NULL);
		al_ddr_ecc_corr_count_clear(data->ddr_cfg[ch]);

		counter = data->ddr_corr_status.err_cnt;

		al_err_events_common_field_error(field, counter);

		al_ddr_ecc_corr_int_clear(data->ddr_cfg[ch]);
	}
}

static void sys_fabric_ddr_uncorrectable_collect(struct al_err_events_field *field,
						 uint32_t cause)
{
	if (cause & field->iofic_bit) {
		unsigned int counter;
		unsigned int ch = sys_fabric_ddr_channel_get(field);
		struct al_err_events_sys_fabric_data *data =
			(struct al_err_events_sys_fabric_data *)field->parent_module->private_data;

		al_ddr_ecc_status_get(data->ddr_cfg[ch],
				      NULL,
				      &data->ddr_uncorr_status);
		al_ddr_ecc_uncorr_count_clear(data->ddr_cfg[ch]);

		counter = data->ddr_uncorr_status.err_cnt;

		al_err_events_common_field_error(field, counter);

		al_ddr_ecc_uncorr_int_clear(data->ddr_cfg[ch]);
	}
}

static void sys_fabric_ddr_on_chip_parity_collect(struct al_err_events_field *field,
						  uint32_t cause)
{
	if (cause & al_err_events_sys_fabric_get_error_bit(field)) {
		unsigned int ch = sys_fabric_ddr_channel_get(field);
		struct al_err_events_sys_fabric_data *data =
			(struct al_err_events_sys_fabric_data *)field->parent_module->private_data;

		al_ddr_onchip_parity_status_get(data->ddr_cfg[ch],
						&data->ddr_on_chip_status);

		al_err_events_common_field_error(field, 1);

		al_ddr_onchip_parity_int_clear(data->ddr_cfg[ch]);
	}
}

static void sys_fabric_pos_error_collect(struct al_err_events_field *field,
					 uint32_t cause)
{

	if (field->iofic_bit & cause) {
		struct al_err_events_sys_fabric_data *data =
			(struct al_err_events_sys_fabric_data *)field->parent_module->private_data;

		al_sys_fabric_sb_pos_info_get_and_clear(&data->handle,
							&data->sb_pos_status);

		al_err_events_common_field_error(field, 1);
	}
}

static void err_events_ddr_ecc_stats_print(struct al_err_events_field *field,
					   struct al_ddr_ecc_status *ddr_ecc_status,
					   al_bool is_corr)
{
	al_phys_addr_t sys_addr;
	struct al_err_events_module *module = field->parent_module;
	struct al_err_events_sys_fabric_data *data =
		(struct al_err_events_sys_fabric_data *)module->private_data;
	al_err_events_print_cb print_cb = module->handle->print_cb;
	unsigned int ch = sys_fabric_ddr_channel_get(field);
	struct al_ddr_cfg *ddr_cfg = data->ddr_cfg[ch];
	struct al_ddr_addr addr_params = {
		.bg = ddr_ecc_status->bg,
		.bank = ddr_ecc_status->bank,
		.rank = ddr_ecc_status->rank,
		.cid = ddr_ecc_status->cid,
		.col = ddr_ecc_status->col,
		.row = ddr_ecc_status->row,
	};

	al_ddr_address_translate_dram2sys_ext(ddr_cfg, &sys_addr, &addr_params);

	print_cb("\tLast status:\n");
	print_cb("\tCount: %u\n", ddr_ecc_status->err_cnt);
	print_cb("\tRank: %u\n", ddr_ecc_status->rank);
	print_cb("\tBank: %u\n", ddr_ecc_status->bank);
	print_cb("\tBank group: %u\n", ddr_ecc_status->bg);
	print_cb("\tRow: %u\n", ddr_ecc_status->row);
	print_cb("\tCol: %u\n", ddr_ecc_status->col);
	print_cb("\tSyndromes: %08x:%08x:%02x\n",
			ddr_ecc_status->syndromes_63_32,
			ddr_ecc_status->syndromes_31_0,
			ddr_ecc_status->syndromes_ecc);
	print_cb("\tSystem address: 0x%" PRIx64 "\n", sys_addr);

	if (is_corr) {
		print_cb("\tCorrected bit mask: %08x:%08x:%02x\n",
				ddr_ecc_status->corr_bit_mask_63_32,
				ddr_ecc_status->corr_bit_mask_31_0,
				ddr_ecc_status->corr_bit_mask_ecc);
		print_cb("\tCorrected bit num: %u\n",
				ddr_ecc_status->ecc_corrected_bit_num);
	}
}

static void err_events_sys_fabric_common_print(struct al_err_events_module *module,
					       struct al_err_events_field *field,
					       al_bool print_header)
{
	al_err_events_print_cb print_cb = module->handle->print_cb;

	if (!field->valid) {
		if (print_header)
			print_cb("%s %s: Ignored\n", module->name, field->name);
		else
			print_cb("\t%s: Ignored\n", field->name);
		return;
	}

	if (print_header)
		print_cb("%s %s: %d\n", module->name, field->name, field->counter);
	else
		print_cb("\t%s: %d\n", field->name, field->counter);
}

static void err_events_sf_ddr_corr_print(struct al_err_events_field *field, al_bool print_header)
{
	struct al_err_events_module *module = field->parent_module;
	struct al_err_events_sys_fabric_data *data =
			(struct al_err_events_sys_fabric_data *)module->private_data;

	err_events_sys_fabric_common_print(module, field, print_header);

	if (field->counter > 0)
		err_events_ddr_ecc_stats_print(field, &data->ddr_corr_status, AL_TRUE);
}

static void err_events_sf_ddr_uncorr_print(struct al_err_events_field *field, al_bool print_header)
{
	struct al_err_events_module *module = field->parent_module;
	struct al_err_events_sys_fabric_data *data =
			(struct al_err_events_sys_fabric_data *)module->private_data;

	err_events_sys_fabric_common_print(module, field, print_header);

	if (field->counter > 0)
		err_events_ddr_ecc_stats_print(field, &data->ddr_uncorr_status, AL_FALSE);
}

static void err_events_sf_pos_error_print(struct al_err_events_field *field, al_bool print_header)
{
	struct al_err_events_module *module = field->parent_module;
	struct al_err_events_sys_fabric_data *data =
			(struct al_err_events_sys_fabric_data *)module->private_data;
	al_err_events_print_cb print_cb = module->handle->print_cb;

	err_events_sys_fabric_common_print(module, field, print_header);

	if (field->counter > 0) {
		print_cb("\tSB PoS Error Address: 0x%" PRIx64 "\n", data->sb_pos_status.address);
		print_cb("\tSB PoS Error Response: 0x%x (%s)\n",
			 data->sb_pos_status.bresp,
			 (data->sb_pos_status.bresp == 0x2) ? "Slave Error" : "Decode Error");
	}
}

static void err_events_sf_ddr_on_chip_parity_print(struct al_err_events_field *field,
						   al_bool print_header)
{
	struct al_err_events_module *module = field->parent_module;
	struct al_err_events_sys_fabric_data *data =
			(struct al_err_events_sys_fabric_data *)module->private_data;
	al_err_events_print_cb print_cb = module->handle->print_cb;

	err_events_sys_fabric_common_print(module, field, print_header);

	if (field->counter > 0) {
		print_cb("\tRead Address On-Chip Parity Error: %d\n",
			 data->ddr_on_chip_status.raddr_err);
		print_cb("\tWrite Address On-Chip Parity Error: %d\n",
			 data->ddr_on_chip_status.waddr_err);
		print_cb("\tRead Data On-Chip Parity Error : %d\n",
			 data->ddr_on_chip_status.rdata_err);
		print_cb("\tWrite Data On-Chip Parity Error on controller input port: %d\n",
			 data->ddr_on_chip_status.wdata_in_err);
		print_cb("\tRead Address On-Chip Parity Error on controller output port: %d\n",
			 data->ddr_on_chip_status.wdata_out_err);
		print_cb("\tWrite Rank: %u\n", data->ddr_on_chip_status.wdata_out_rank);
		print_cb("\tWrite Bank: %u\n", data->ddr_on_chip_status.wdata_out_bank);
		print_cb("\tWrite Bank Group: %u\n", data->ddr_on_chip_status.wdata_out_bg);
		print_cb("\tWrite Row: %u\n", data->ddr_on_chip_status.wdata_out_row);
		print_cb("\tWrite Col: %u\n", data->ddr_on_chip_status.wdata_out_col);
		print_cb("\tWrite Byte: %u\n", data->ddr_on_chip_status.wdata_out_byte_loc);
		print_cb("\tWrite Addr: 0x" PRIx64 "\n",
			(uintptr_t)data->ddr_on_chip_status.waddr_address);
		print_cb("\tRead Byte: %u\n", data->ddr_on_chip_status.rdata_byte_loc);
		print_cb("\tRead Addr: 0x" PRIx64 "\n",
			(uintptr_t)data->ddr_on_chip_status.raddr_address);
	}
}

static void err_events_sys_fabric_clear(struct al_err_events_module *module)
{
	struct al_err_events_sys_fabric_data *data =
			(struct al_err_events_sys_fabric_data *)module->private_data;

	al_err_events_common_module_clear(module);

	al_memset(&data->ddr_corr_status, 0, sizeof(struct al_ddr_ecc_status));
	al_memset(&data->ddr_uncorr_status, 0, sizeof(struct al_ddr_ecc_status));
	al_memset(&data->sb_pos_status, 0, sizeof(struct al_sys_fabric_sb_pos_error_info));
	al_memset(&data->ddr_on_chip_status, 0, sizeof(struct al_ddr_onchip_par_status));
}

#if AL_DEV_ID <= AL_DEV_ID_ALPINE_V2
static int err_events_sys_fabric_collect(struct al_err_events_module *module,
					 enum al_err_events_collect collect)
{
	unsigned int i;
	uint32_t nb_cause;
	struct al_err_events_sys_fabric_data *data =
			(struct al_err_events_sys_fabric_data *)module->private_data;

	al_sys_fabric_int_cause_read(&data->handle, &nb_cause);
	if (nb_cause) {
		uint32_t local_mask = data->local_int_mask;
		uint32_t local_int_mask = 0;
		uint32_t error_bit;
		uint32_t error_mask;
		uint32_t error_cause;

		al_sys_fabric_error_cause_read(&data->handle, &error_cause);
		error_mask = al_sys_fabric_error_int_mask_read(&data->handle);
		for (i = 0; local_mask; i++, local_mask >>= 1) {
			if (!(local_mask & 1))
				continue;

			local_int_mask |= al_sys_fabric_int_local_mask_read(&data->handle, i);
		}

		for (i = 0; i < module->fields_size; i++) {
			struct al_err_events_field *field = &module->fields[i];

			if (!field->valid || (field->collect_mode != collect))
				continue;

			if ((collect == AL_ERR_EVENTS_COLLECT_INTERRUPT) &&
			    !(local_int_mask & field->iofic_bit))
				continue;

			error_bit = al_err_events_sys_fabric_get_error_bit(field);
			if (error_bit) {
				if ((collect == AL_ERR_EVENTS_COLLECT_INTERRUPT) &&
				    !(error_mask & error_bit))
					continue;

				field->collect(field, error_cause);
			} else
				field->collect(field, nb_cause);
		}
	}

	return 0;
}

int al_err_events_sys_fabric_init(struct al_err_events_handle *handle,
				  struct al_err_events_sys_fabric_data *data,
				  struct al_err_events_sys_fabric_init_params *params)
{
	struct al_err_events_common_module_params module_params;
	struct al_err_events_module *module;
	unsigned int i;

	al_assert(handle);
	al_assert(data);
	al_assert(params);
	al_assert(sizeof(al_err_events_sys_fabric_fields) <= sizeof(data->fields));
	al_assert(sizeof(al_err_events_sys_fabric_fields) ==
		 (AL_ERR_EVENTS_SYS_FABRIC_MAX_ERRORS * sizeof(struct al_err_events_field)));

	module = &data->module;

	/* init data */
	data->ddr_cfg[0] = params->ddr_cfg[0];
	data->supported_ddr_ch = (handle->dev_id <= PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V2) ? 1 : 2;
	data->local_int_mask = params->local_int_mask;
	al_sys_fabric_handle_init(&data->handle, params->nb_regs_base);

	/* init module */
	al_memset(&module_params, 0, sizeof(module_params));
	al_sprintf(module_params.name, "System-Fabric");
	module_params.primary_module = AL_ERR_EVENTS_MODULE_SYS_FABRIC;
	module_params.secondary_module = AL_ERR_EVENTS_SEC_MODULE_NONE;
	module_params.primary_index = 0;
	module_params.secondary_index = 0;
	module_params.collect_mode = params->collect_mode;
	for (i = 0; i < AL_ERR_EVENTS_IC_MAX; i++)
		module_params.ic_regs_bases[i] = NULL;
	module_params.ic_size = 0;
	module_params.fields_size = AL_ERR_EVENTS_SYS_FABRIC_MAX_ERRORS;
	module_params.fields = data->fields;
	module_params.fields_template = al_err_events_sys_fabric_fields;
	module_params.private_data = data;
	module_params.collect = err_events_sys_fabric_collect;
	module_params.print = al_err_events_common_module_print;
	module_params.clear = err_events_sys_fabric_clear;
	module_params.test = NULL;

	al_err_events_common_module_init(handle, module, &module_params);

	if (data->handle.ver == NB_NB_VERSION_VERSION_RELEASE_NUM_MAJOR_VAL_ALPINE_V1) {
		module->fields[AL_ERR_EVENTS_SYS_FABRIC_DDR_ON_CHIP_PARITY_ERROR].valid = AL_FALSE;
		module->fields[AL_ERR_EVENTS_SYS_FABRIC_LOCAL_GIC_ERROR].valid = AL_FALSE;
	}

	al_err_events_common_module_post_init(module);

	return 0;
}

int al_err_events_sys_fabric_ints_enable(struct al_err_events_sys_fabric_data *data)
{
	unsigned int i;
	unsigned int local_int_mask;
	al_bool ddr_on_chip_cfg = AL_FALSE;
	uint32_t error_mask = 0;
	uint32_t int_unmask = 0;
	struct al_err_events_module *module;

	al_assert(data);

	module = &data->module;

	if (!module->enabled)
		return -EINVAL;

	for (i = 0; i < module->fields_size; i++) {
		struct al_err_events_field *field = &module->fields[i];

		if (field->valid && (field->collect_mode == AL_ERR_EVENTS_COLLECT_INTERRUPT)) {
			int_unmask |= field->iofic_bit;

			if (i == AL_ERR_EVENTS_SYS_FABRIC_DDR_ON_CHIP_PARITY_ERROR)
				ddr_on_chip_cfg = AL_TRUE;
			else if ((i == AL_ERR_EVENTS_SYS_FABRIC_MSIX_ERROR) ||
				 (i == AL_ERR_EVENTS_SYS_FABRIC_LOCAL_GIC_ERROR))
				error_mask |= al_err_events_sys_fabric_get_error_bit(field);

		}
	}

	al_sys_fabric_int_clear(&data->handle,
				~NB_GLOBAL_NB_INT_CAUSE_CPU_AXIERRIRQ);

	al_sys_fabric_error_int_unmask(&data->handle,
				       error_mask);

	local_int_mask = data->local_int_mask;

	for (i = 0; local_int_mask; i++, local_int_mask >>= 1) {
		if (!(local_int_mask & 1))
			continue;

		al_sys_fabric_int_local_mask(&data->handle, i,
			NB_GLOBAL_NB_INT_CAUSE_MCTL_ECC_CORR_ERR);

		al_sys_fabric_int_local_unmask(&data->handle, i, int_unmask);

		if (ddr_on_chip_cfg)
			al_ddr_onchip_parity_int_unmask(data->ddr_cfg[0], i);
	}

	return 0;
}

#else
static int err_events_sys_fabric_err_sfic_collect(struct al_err_events_module *module,
				 enum al_err_events_collect collect)
{
	int rc;
	al_bool should_collect = AL_FALSE;
	uint32_t cause = 0;

	if (collect == AL_ERR_EVENTS_COLLECT_POLLING) {
		should_collect = AL_TRUE;
	} else if (collect == AL_ERR_EVENTS_COLLECT_INTERRUPT) {
		cause = al_iofic_read_cause(
			module->ic_regs_bases[SYS_FABRIC_V3_SFIC_ID_NMI], AL_INT_GROUP_A);
		should_collect = !!(cause & AL_SYS_FABRIC_SFIC_NMI_GROUP_A_ERROR_SFIC_SUMMARY);
	}

	if (!should_collect)
		return 0;

	rc = al_err_events_common_module_iofic_collect(module, collect, SYS_FABRIC_V3_SFIC_ID_ERR,
										AL_INT_GROUP_A);
	if (rc)
		al_err("%s: Failed to collect error sfic errors (rc = %d)\n", __func__, rc);

	if (collect == AL_ERR_EVENTS_COLLECT_INTERRUPT)
		al_iofic_clear_cause(
			module->ic_regs_bases[SYS_FABRIC_V3_SFIC_ID_NMI], AL_INT_GROUP_A,
			AL_SYS_FABRIC_SFIC_NMI_GROUP_A_ERROR_SFIC_SUMMARY);

	return 0;
}

static int err_events_sys_fabric_error_collect(struct al_err_events_module *module,
				 enum al_err_events_collect collect)
{
	int rc;

	/** Collect ERR SFIC first */
	rc = err_events_sys_fabric_err_sfic_collect(module, collect);
	if (rc)
		return rc;

	/** collect NMI errors */
	rc = al_err_events_common_module_iofic_collect(module, collect, SYS_FABRIC_V3_SFIC_ID_NMI,
		AL_INT_GROUP_A);
	if (rc) {
		al_err("%s: Failed to collect error for module %s, NMI SFIC\n",
			__func__, module->name);
		return rc;
	}

	return 0;
}

static al_bool al_err_events_sys_fabric_v3_module_test(struct al_err_events_module *module)
{
	/**
	 * Needs to invalidate DDR ECC errors as we dont need to read them on self test
	 * The self test is only to test the SFIC / IOFIC
	 * and there is a dedicated test for ECC
	 */
	unsigned int i;
	al_bool res;
	enum al_err_events_sys_fabric_v3_error ids[] = {
		AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_DDR_CHANNEL_0_UNCORRECTABLE_ECC,
		AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_DDR_CHANNEL_0_CORRECTABLE_ECC,
		AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_DDR_CHANNEL_1_UNCORRECTABLE_ECC,
		AL_ERR_EVENTS_SYS_FABRIC_V3_NMI_SFIC_DDR_CHANNEL_1_CORRECTABLE_ECC,
		AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_DDR_ONCHIP_PARITY_0,
		AL_ERR_EVENTS_SYS_FABRIC_V3_ERROR_SFIC_DDR_ONCHIP_PARITY_1,
	};
	al_bool valids[AL_ARR_SIZE(ids)];

	for (i = 0; i < AL_ARR_SIZE(ids); i++) {
		valids[i] = module->fields[ids[i]].valid;
		module->fields[ids[i]].valid = AL_FALSE;
	}

	res = al_err_events_common_module_test(module);

	for (i = 0; i < AL_ARR_SIZE(ids); i++)
		module->fields[ids[i]].valid = valids[i];

	return res;
}

int al_err_events_sys_fabric_init(struct al_err_events_handle *handle,
				  struct al_err_events_sys_fabric_data *data,
				  struct al_err_events_sys_fabric_init_params *params)
{
	struct al_err_events_common_module_params module_params;
	struct al_err_events_module *module;

	al_assert(handle);
	al_assert(data);
	al_assert(params);
	al_assert(sizeof(al_err_events_sys_fabric_error_fields) == sizeof(data->fields));
	al_assert(sizeof(al_err_events_sys_fabric_error_fields) ==
		 (AL_ERR_EVENTS_SYS_FABRIC_V3_SFIC_MAX_ERRORS *
			sizeof(struct al_err_events_field)));
	al_assert(params->ddr_cfg[0]);
	al_assert(params->ddr_cfg[1]);

	module = &data->module;
	data->pcie_enable_map = params->pcie_enable_map;
	data->supported_ddr_ch = (handle->dev_id <= PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V2) ? 1 : 2;
	data->ddr_cfg[0] = params->ddr_cfg[0];
	data->ddr_cfg[1] = params->ddr_cfg[1];

	/* init data */
	al_sys_fabric_handle_init(&data->handle, params->nb_regs_base);

	/* init module */
	al_memset(&module_params, 0, sizeof(struct al_err_events_common_module_params));
	al_sprintf(module_params.name, "System-Fabric");
	module_params.primary_module = AL_ERR_EVENTS_MODULE_SYS_FABRIC;
	module_params.secondary_module = AL_ERR_EVENTS_SEC_MODULE_NONE;
	module_params.primary_index = 0;
	module_params.secondary_index = 0;
	module_params.collect_mode = params->collect_mode;
	module_params.ic_regs_bases[SYS_FABRIC_V3_SFIC_ID_ERR] = data->handle.error_sfic_base;
	module_params.ic_regs_bases[SYS_FABRIC_V3_SFIC_ID_NMI] = data->handle.nmi_sfic_base;
	module_params.ic_size = 2;
	module_params.fields_size = AL_ERR_EVENTS_SYS_FABRIC_V3_SFIC_MAX_ERRORS;
	module_params.fields = data->fields;
	module_params.fields_template = al_err_events_sys_fabric_error_fields;
	module_params.private_data = data;
	module_params.collect = err_events_sys_fabric_error_collect;
	module_params.print = al_err_events_common_module_print;
	module_params.clear = err_events_sys_fabric_clear;
	module_params.test = al_err_events_sys_fabric_v3_module_test;

	al_err_events_common_module_init(handle, module, &module_params);

	al_err_events_common_module_post_init(module);

	return 0;
}

int al_err_events_sys_fabric_ints_enable(struct al_err_events_sys_fabric_data *data)
{
	unsigned int i;
	uint32_t enabled_pcie = 0;
	uint32_t disabled_pcie = 0;
	uint32_t pcie_bits[AL_ERR_EVENTS_PCIE_NUM] = {
		AL_SYS_FABRIC_SFIC_NMI_GROUP_A_PCIE_DEV_0_FATAL_0,
		AL_SYS_FABRIC_SFIC_NMI_GROUP_A_PCIE_DEV_0_FATAL_1,
		AL_SYS_FABRIC_SFIC_NMI_GROUP_A_PCIE_DEV_0_FATAL_2,
		AL_SYS_FABRIC_SFIC_NMI_GROUP_A_PCIE_DEV_0_FATAL_3,
		AL_SYS_FABRIC_SFIC_NMI_GROUP_A_PCIE_DEV_0_FATAL_4,
		AL_SYS_FABRIC_SFIC_NMI_GROUP_A_PCIE_DEV_0_FATAL_5,
		AL_SYS_FABRIC_SFIC_NMI_GROUP_A_PCIE_DEV_0_FATAL_6,
		AL_SYS_FABRIC_SFIC_NMI_GROUP_A_PCIE_DEV_0_FATAL_7,
	};

	al_assert(data);

	/**
	 *		( NMI SFIC )
	 *                  ||
	 * (L2 errs 3:0) ..........(ERR SFIC)
	 *                           ||
	 *                     (DDR CAUSE 1:0)
	 *
	 * Start unmasking from the buttom
	 *
	 */

	al_sys_fabric_error_unmask(&data->handle);

	al_sys_fabric_nmi_unmask(&data->handle);

	/**
	 * Adressing RMN 11482
	 * Interrupts - PCIe fatal cause in NMI is not valid when pcie_exist = 0
	 *
	 * SW W/A : Pass a mask of enabled pcie devices
	 */
	/**
	 * Adressing RMN 11157
	 * Interrupts - PCIe fatal cause in NMI is not valid when pcie_exist = 0
	 *
	 * SW W/A : Pass a mask of enabled pcie devices
	 */
	for (i = 0; i < AL_ERR_EVENTS_PCIE_NUM; i++) {
		if (AL_BIT(i) & data->pcie_enable_map)
			enabled_pcie |= pcie_bits[i];
		else
			disabled_pcie |= pcie_bits[i];
	}
	al_iofic_mask(data->handle.nmi_sfic_base,
		      AL_INT_GROUP_A,
		      disabled_pcie);
	al_iofic_clear_cause(data->handle.nmi_sfic_base,
			     AL_INT_GROUP_A,
			     disabled_pcie);

	/**
	 * Clear before unmasking as this bit is an indication of pcie link-down and might
	 * be set from pcie init phase
	 */
	al_iofic_clear_cause(data->handle.nmi_sfic_base,
			     AL_INT_GROUP_A,
			     enabled_pcie);
	al_iofic_unmask(data->handle.nmi_sfic_base,
			AL_INT_GROUP_A,
			enabled_pcie);

	return 0;
}
#endif

#endif
