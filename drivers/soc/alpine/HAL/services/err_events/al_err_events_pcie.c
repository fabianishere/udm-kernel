/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#include "al_err_events_internal.h"
#include "al_err_events_pcie.h"
#include "al_hal_pcie_interrupts.h"
#include "al_hal_udma_iofic.h"

static void pcie_app_parity_error_print(struct al_err_events_field *field,
					al_bool print_header);
static void pcie_app_parity_collect(struct al_err_events_field *field, uint32_t cause);
static void pcie_axi_read_data_parity_print(struct al_err_events_field *field,
					    al_bool print_header);
static void pcie_axi_read_compl_error_print(struct al_err_events_field *field,
					    al_bool print_header);
static void pcie_axi_write_compl_error_print(struct al_err_events_field *field,
					     al_bool print_header);
static void pcie_axi_read_compl_timeout_print(struct al_err_events_field *field,
					      al_bool print_header);
static void pcie_axi_write_compl_timeout_print(struct al_err_events_field *field,
					       al_bool print_header);
static void pcie_axi_pos_error_print(struct al_err_events_field *field,
				     al_bool print_header);
static void pcie_axi_parity_error_print(struct al_err_events_field *field,
					al_bool print_header);
static void pcie_axi_error_collect(struct al_err_events_field *field, uint32_t cause);

/*******************************************************************************
 ** Static error fields
 ******************************************************************************/
static const struct al_err_events_field
	al_err_events_pcie_app_fields[AL_ERR_EVENTS_PCIE_APP_MAX_ERRORS] = {
	{
		.name = "Core Internal Decompose Logic Error",
		.id = AL_ERR_EVENTS_PCIE_APP_CORE_INTERNAL_DECOMPOSE_LOGIC_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_PCIE_APP_INT_GRP_B_SLV_RESP_COMP_LKUP_ERR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Parity Error",
		.id = AL_ERR_EVENTS_PCIE_APP_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_PCIE_APP_INT_GRP_B_PARITY_ERR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = pcie_app_parity_error_print,
		.collect = pcie_app_parity_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Write Overflow Error",
		.id = AL_ERR_EVENTS_PCIE_APP_WRITE_OVERFLOW_ERROR,
		.iofic_group = AL_INT_GROUP_B,
		.iofic_bit = AL_PCIE_APP_INT_GRP_B_WRITE_OVERFLOW,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Internal Overflow Error",
		.id = AL_ERR_EVENTS_PCIE_APP_INTERNAL_OVERFLOW_ERROR,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_PCIE_APP_INT_GRP_C_INTERNAL_ERR_OVRFLW,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "System err function 0",
		.id = AL_ERR_EVENTS_PCIE_APP_SYS_ERR_FUNC_0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_PCIE_APP_INT_GRP_C_SYS_ERR_FUNC_0,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "System err function 1",
		.id = AL_ERR_EVENTS_PCIE_APP_SYS_ERR_FUNC_1,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_PCIE_APP_INT_GRP_C_SYS_ERR_FUNC_1,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "System err function 2",
		.id = AL_ERR_EVENTS_PCIE_APP_SYS_ERR_FUNC_2,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_PCIE_APP_INT_GRP_C_SYS_ERR_FUNC_2,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "System err function 3",
		.id = AL_ERR_EVENTS_PCIE_APP_SYS_ERR_FUNC_3,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_PCIE_APP_INT_GRP_C_SYS_ERR_FUNC_3,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "AER err interrupt function 0",
		.id = AL_ERR_EVENTS_PCIE_APP_SYS_ERR_FUNC_0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_PCIE_APP_INT_GRP_C_AER_ERR_INT_FUNC_0,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "AER err interrupt function 1",
		.id = AL_ERR_EVENTS_PCIE_APP_SYS_ERR_FUNC_1,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_PCIE_APP_INT_GRP_C_AER_ERR_INT_FUNC_1,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "AER err interrupt function 2",
		.id = AL_ERR_EVENTS_PCIE_APP_SYS_ERR_FUNC_2,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_PCIE_APP_INT_GRP_C_AER_ERR_INT_FUNC_2,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "AER err interrupt function 3",
		.id = AL_ERR_EVENTS_PCIE_APP_SYS_ERR_FUNC_3,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_PCIE_APP_INT_GRP_C_AER_ERR_INT_FUNC_3,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "AER err msi function 0",
		.id = AL_ERR_EVENTS_PCIE_APP_AER_ERR_MSI_0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_PCIE_APP_INT_GRP_C_AER_ERR_MSI_FUNC_0,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "AER err msi function 1",
		.id = AL_ERR_EVENTS_PCIE_APP_AER_ERR_MSI_1,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_PCIE_APP_INT_GRP_C_AER_ERR_MSI_FUNC_1,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "AER err msi function 2",
		.id = AL_ERR_EVENTS_PCIE_APP_AER_ERR_MSI_2,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_PCIE_APP_INT_GRP_C_AER_ERR_MSI_FUNC_2,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "AER err msi function 3",
		.id = AL_ERR_EVENTS_PCIE_APP_AER_ERR_MSI_3,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_PCIE_APP_INT_GRP_C_AER_ERR_MSI_FUNC_3,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "AP correctable error status function 0",
		.id = AL_ERR_EVENTS_PCIE_APP_AP_CRCTBL_STAT_0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_PCIE_APP_INT_GRP_D_AP_CRCTBL_STAT_FUNC_0,
		.sev = AL_ERR_EVENTS_CORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "AP correctable error status function 1",
		.id = AL_ERR_EVENTS_PCIE_APP_AP_CRCTBL_STAT_0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_PCIE_APP_INT_GRP_D_AP_CRCTBL_STAT_FUNC_1,
		.sev = AL_ERR_EVENTS_CORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "AP correctable error status function 2",
		.id = AL_ERR_EVENTS_PCIE_APP_AP_CRCTBL_STAT_0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_PCIE_APP_INT_GRP_D_AP_CRCTBL_STAT_FUNC_2,
		.sev = AL_ERR_EVENTS_CORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "AP correctable error status function 3",
		.id = AL_ERR_EVENTS_PCIE_APP_AP_CRCTBL_STAT_0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_PCIE_APP_INT_GRP_D_AP_CRCTBL_STAT_FUNC_3,
		.sev = AL_ERR_EVENTS_CORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "AP uncorrectable error status function 0",
		.id = AL_ERR_EVENTS_PCIE_APP_AP_UNCRCTBL_STAT_0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_PCIE_APP_INT_GRP_D_AP_UNCRCTBL_STAT_FUNC_0,
		.sev = AL_ERR_EVENTS_CORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "AP uncorrectable error status function 1",
		.id = AL_ERR_EVENTS_PCIE_APP_AP_UNCRCTBL_STAT_0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_PCIE_APP_INT_GRP_D_AP_UNCRCTBL_STAT_FUNC_1,
		.sev = AL_ERR_EVENTS_CORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "AP uncorrectable error status function 2",
		.id = AL_ERR_EVENTS_PCIE_APP_AP_UNCRCTBL_STAT_0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_PCIE_APP_INT_GRP_D_AP_UNCRCTBL_STAT_FUNC_2,
		.sev = AL_ERR_EVENTS_CORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "AP uncorrectable error status function 3",
		.id = AL_ERR_EVENTS_PCIE_APP_AP_UNCRCTBL_STAT_0,
		.iofic_group = AL_INT_GROUP_C,
		.iofic_bit = AL_PCIE_APP_INT_GRP_D_AP_UNCRCTBL_STAT_FUNC_3,
		.sev = AL_ERR_EVENTS_CORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
};

static const struct al_err_events_field
	al_err_events_pcie_axi_fields[AL_ERR_EVENTS_PCIE_AXI_MAX_ERRORS] = {
	{
		.name = "Parity Error",
		.id = AL_ERR_EVENTS_PCIE_AXI_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_PCIE_AXI_INT_AXI_DOM_PARITY_ERR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = pcie_axi_parity_error_print,
		.collect = pcie_axi_error_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Core Internal Composer Logic Error",
		.id = AL_ERR_EVENTS_PCIE_AXI_CORE_INTERNAL_DECOMPOSE_LOGIC_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_PCIE_AXI_INT_MSTR_RESP_COMP_LKUP_ERR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Master Read Data Parity Error",
		.id = AL_ERR_EVENTS_PCIE_AXI_MASTER_READ_DATA_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_PCIE_AXI_INT_PARITY_ERR_MSTR_DATA_RD_CHNL,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = pcie_axi_read_data_parity_print,
		.collect = pcie_axi_error_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Slave Read Address Parity",
		.id = AL_ERR_EVENTS_PCIE_AXI_SLAVE_READ_ADDR_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_PCIE_AXI_INT_PARITY_ERR_SLV_ADDR_RD_CHNL,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Slave Write Address Parity",
		.id = AL_ERR_EVENTS_PCIE_AXI_SLAVE_WRITE_ADDR_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_PCIE_AXI_INT_PARITY_ERR_SLV_ADDR_WR_CHNL,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Slave Write Data Parity",
		.id = AL_ERR_EVENTS_PCIE_AXI_SLAVE_WRITE_DATA_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_PCIE_AXI_INT_PARITY_ERR_SLV_DATA_WR_CHNL,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Read Completion Error",
		.id = AL_ERR_EVENTS_PCIE_AXI_READ_COMPLETION_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_PCIE_AXI_INT_RD_AXI_COMPL_ERR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = pcie_axi_read_compl_error_print,
		.collect = pcie_axi_error_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Write Completion Error",
		.id = AL_ERR_EVENTS_PCIE_AXI_WRITE_COMPLETION_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_PCIE_AXI_INT_WR_AXI_COMPL_ERR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = pcie_axi_write_compl_error_print,
		.collect = pcie_axi_error_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Read Completion Timeout",
		.id = AL_ERR_EVENTS_PCIE_AXI_READ_COMPLETION_TIMEOUT,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_PCIE_AXI_INT_RD_AXI_COMPL_TO,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = pcie_axi_read_compl_timeout_print,
		.collect = pcie_axi_error_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Write Completion Timeout",
		.id = AL_ERR_EVENTS_PCIE_AXI_WRITE_COMPLETION_TIMEOUT,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_PCIE_AXI_INT_WR_AXI_COMPL_TO,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = pcie_axi_write_compl_timeout_print,
		.collect = pcie_axi_error_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "PoS Error",
		.id = AL_ERR_EVENTS_PCIE_AXI_POS_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_PCIE_AXI_INT_POS_ERR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = pcie_axi_pos_error_print,
		.collect = pcie_axi_error_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
	{
		.name = "Write Respond Error",
		.id = AL_ERR_EVENTS_PCIE_AXI_WRITE_RESPONED_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_PCIE_AXI_INT_WR_AXI_RESPOND_ERR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_err_events_common_field_print,
		.collect = al_err_events_common_field_iofic_collect,
		.mask = al_err_events_common_field_iofic_mask,
	},
};

/*******************************************************************************
 ** PCIe App
 ******************************************************************************/
static void err_events_pcie_app_parity_v3_collect(struct al_err_events_pcie_app_data *data,
						  struct al_pcie_core_parity_stats *core_status)
{
	if (core_status->ram_1p_rbuf)
		data->app_v3_parity.ram_1p_rbuf++;
	if (core_status->ram_2p_sotbuf)
		data->app_v3_parity.ram_2p_sotbuf++;
	if (core_status->u0_ram_radm_qbuffer_hdr)
		data->app_v3_parity.u0_ram_radm_qbuffer_hdr++;
	if (core_status->u3_ram_radm_qbuffer_data_0)
		data->app_v3_parity.u3_ram_radm_qbuffer_data_0++;
	if (core_status->u3_ram_radm_qbuffer_data_1)
		data->app_v3_parity.u3_ram_radm_qbuffer_data_1++;
	if (core_status->u10_ram2p_0)
		data->app_v3_parity.u10_ram2p_0++;
	if (core_status->u10_ram2p_1)
		data->app_v3_parity.u10_ram2p_1++;
	if (core_status->u8_ram2p)
		data->app_v3_parity.u8_ram2p++;
	if (core_status->u7_ram2p)
		data->app_v3_parity.u7_ram2p++;
	if (core_status->u6_ram)
		data->app_v3_parity.u6_ram++;
	if (core_status->u11_ram2p)
		data->app_v3_parity.u11_ram2p++;
	if (core_status->u1_ram2p)
		data->app_v3_parity.u1_ram2p++;
	if (core_status->u0_ram2p)
		data->app_v3_parity.u0_ram2p++;
}

static void err_events_pcie_app_parity_v1_v2_collect(struct al_err_events_pcie_app_data *data,
						     struct al_pcie_core_parity_stats *core_status)
{
	if (core_status->u_ram_1p_sotbuf)
		data->app_v1_v2_parity.u_ram_1p_sotbuf++;
	if (core_status->u0_ram_radm_qbuffer)
		data->app_v1_v2_parity.u0_ram_radm_qbuffer++;
	if (core_status->u3_qbuffer_0)
		data->app_v1_v2_parity.u3_qbuffer_0++;
	if (core_status->u3_qbuffer_1)
		data->app_v1_v2_parity.u3_qbuffer_1++;
	if (core_status->u9_decomp)
		data->app_v1_v2_parity.u9_decomp++;
	if (core_status->u8_ram2p)
		data->app_v1_v2_parity.u8_ram2p++;
	if (core_status->u7_ram2p)
		data->app_v1_v2_parity.u7_ram2p++;
	if (core_status->u6_ram2p)
		data->app_v1_v2_parity.u6_ram2p++;
	if (core_status->u11_ram2p)
		data->app_v1_v2_parity.u11_ram2p++;
	if (core_status->u1_ram2p)
		data->app_v1_v2_parity.u1_ram2p++;
	if (core_status->u0_ram2p)
		data->app_v1_v2_parity.u0_ram2p++;
	if (core_status->u0_rbuf)
		data->app_v1_v2_parity.u0_rbuf++;
	if (core_status->u3_qbuffer_2)
		data->app_v1_v2_parity.u3_qbuffer_2++;
}

static void pcie_app_parity_collect(struct al_err_events_field *field, uint32_t cause)
{
	if (cause & field->iofic_bit) {
		struct al_pcie_core_parity_stats core_status;
		struct al_err_events_module *module = field->parent_module;
		struct al_err_events_pcie_app_data *data =
				(struct al_err_events_pcie_app_data *)module->private_data;
		struct al_pcie_port *pcie_port = data->pcie_port;

		al_pcie_core_parity_stats_get(pcie_port, &core_status);
		if (core_status.num_of_errors) {
			switch (pcie_port->rev_id) {
			case AL_PCIE_REV_ID_4:
			case AL_PCIE_REV_ID_3:
				err_events_pcie_app_parity_v3_collect(data,
								      &core_status);
				break;
			case AL_PCIE_REV_ID_2:
			case AL_PCIE_REV_ID_1:
				err_events_pcie_app_parity_v1_v2_collect(data,
									 &core_status);
				break;
			default:
				al_err("%s: ERROR unsupported PCIe revision ID %d\n",
					__func__,
					pcie_port->rev_id);
				return;
			}
		}

		al_err_events_common_field_iofic_collect(field, cause);
	}
}

static int err_events_pcie_app_collect(struct al_err_events_module *module,
				       enum al_err_events_collect collect_mode)
{
	int ret;
	struct al_err_events_pcie_app_data *data =
			(struct al_err_events_pcie_app_data *)module->private_data;

	ret = al_err_events_common_module_iofic_collect(module, collect_mode, 0, AL_INT_GROUP_B);
	if (ret)
		return ret;

	if (data->pcie_port->rev_id >= AL_PCIE_REV_ID_4)
		al_err_events_common_module_iofic_collect(
			module, collect_mode, 0, AL_INT_GROUP_C);

	return 0;
}

static void pcie_app_parity_error_print(struct al_err_events_field *field,
					al_bool print_header)
{
	struct al_err_events_module *module = field->parent_module;
	al_err_events_print_cb print_cb = module->handle->print_cb;
	struct al_err_events_pcie_app_data *data =
			(struct al_err_events_pcie_app_data *)module->private_data;

	al_err_events_common_field_print(field, print_header);

	if (!field->valid || (field->counter == 0))
		return;

	if (data->pcie_port->rev_id <= AL_PCIE_REV_ID_2) {
		print_cb("\tApp u_ram_1p_sotbuf SRAM Parity Error: %d\n",
			 data->app_v1_v2_parity.u_ram_1p_sotbuf);
		print_cb("\tApp u0_ram_radm_qbuffer SRAM Parity Error: %d\n",
			 data->app_v1_v2_parity.u0_ram_radm_qbuffer);
		print_cb("\tApp u3_qbuffer_0 SRAM Parity Error: %d\n",
			 data->app_v1_v2_parity.u3_qbuffer_0);
		print_cb("\tApp u3_qbuffer_1 SRAM Parity Error: %d\n",
			 data->app_v1_v2_parity.u3_qbuffer_1);
		print_cb("\tApp u9_decomp SRAM Parity Error: %d\n",
			 data->app_v1_v2_parity.u9_decomp);
		print_cb("\tApp u8_ram2p SRAM Parity Error: %d\n", data->app_v1_v2_parity.u8_ram2p);
		print_cb("\tApp u7_ram2p SRAM Parity Error: %d\n", data->app_v1_v2_parity.u7_ram2p);
		print_cb("\tApp u6_ram2p SRAM Parity Error: %d\n", data->app_v1_v2_parity.u6_ram2p);
		print_cb("\tApp u11_ram2p SRAM Parity Error: %d\n",
			 data->app_v1_v2_parity.u11_ram2p);
		print_cb("\tApp u1_ram2p SRAM Parity Error: %d\n", data->app_v1_v2_parity.u1_ram2p);
		print_cb("\tApp u0_ram2p SRAM Parity Error: %d\n", data->app_v1_v2_parity.u0_ram2p);
		print_cb("\tApp u0_rbuf SRAM Parity Error: %d\n", data->app_v1_v2_parity.u0_rbuf);
		print_cb("\tApp u3_qbuffer_2 SRAM Parity Error: %d\n",
			 data->app_v1_v2_parity.u3_qbuffer_2);
	} else {
		print_cb("\tApp ram_1p_rbuf SRAM Parity Error: %d\n",
			 data->app_v3_parity.ram_1p_rbuf);
		print_cb("\tApp ram_2p_sotbuf SRAM Parity Error: %d\n",
			 data->app_v3_parity.ram_2p_sotbuf);
		print_cb("\tApp u0_ram_radm_qbuffer_hdr SRAM Parity Error: %d\n",
			 data->app_v3_parity.u0_ram_radm_qbuffer_hdr);
		print_cb("\tApp u3_ram_radm_qbuffer_data_0 SRAM Parity Error: %d\n",
			 data->app_v3_parity.u3_ram_radm_qbuffer_data_0);
		print_cb("\tApp u3_ram_radm_qbuffer_data_1 SRAM Parity Error: %d\n",
			 data->app_v3_parity.u3_ram_radm_qbuffer_data_1);
		print_cb("\tApp u10_ram2p_0 SRAM Parity Error: %d\n",
			 data->app_v3_parity.u10_ram2p_0);
		print_cb("\tApp u10_ram2p_1 SRAM Parity Error: %d\n",
			 data->app_v3_parity.u10_ram2p_1);
		print_cb("\tApp u8_ram2p SRAM Parity Error: %d\n", data->app_v3_parity.u8_ram2p);
		print_cb("\tApp u7_ram2p SRAM Parity Error: %d\n", data->app_v3_parity.u7_ram2p);
		print_cb("\tApp u6_ram SRAM Parity Error: %d\n", data->app_v3_parity.u6_ram);
		print_cb("\tApp u11_ram2p SRAM Parity Error: %d\n", data->app_v3_parity.u11_ram2p);
		print_cb("\tApp u1_ram2p SRAM Parity Error: %d\n", data->app_v3_parity.u1_ram2p);
		print_cb("\tApp u0_ram2p SRAM Parity Error: %d\n", data->app_v3_parity.u0_ram2p);
	}
}

static void err_events_pcie_app_clear(struct al_err_events_module *module)
{
	struct al_err_events_pcie_app_data *data =
			(struct al_err_events_pcie_app_data *)module->private_data;

	al_err_events_common_module_clear(module);

	al_memset(&data->app_v1_v2_parity, 0, sizeof(data->app_v1_v2_parity));
	al_memset(&data->app_v3_parity, 0, sizeof(data->app_v3_parity));
}

int al_err_events_pcie_app_init(struct al_err_events_handle *handle,
				struct al_err_events_pcie_app_data *data,
				struct al_err_events_pcie_init_params *params)
{
	struct al_err_events_common_module_params module_params;

	al_assert(handle);
	al_assert(data);
	al_assert(params);
	al_assert(sizeof(al_err_events_pcie_app_fields) == sizeof(data->fields));
	al_assert(sizeof(al_err_events_pcie_app_fields) ==
		 (AL_ERR_EVENTS_PCIE_APP_MAX_ERRORS * sizeof(struct al_err_events_field)));

	al_memset(&module_params, 0, sizeof(module_params));

	/* init data */
	data->pcie_port = params->pcie_port;

	/* init module */
	al_sprintf(module_params.name, "PCIe-App-Port(%d)", params->pcie_port->port_id);
	module_params.primary_module = AL_ERR_EVENTS_MODULE_PCIE;
	module_params.secondary_module = AL_ERR_EVENTS_SEC_MODULE_PCIE_APP;
	module_params.primary_index = params->pcie_port->port_id;
	module_params.secondary_index = 0;
	module_params.collect_mode = params->collect_mode;
	module_params.ic_regs_bases[0] = params->pcie_port->regs->app.int_grp_a;
	module_params.ic_size = 1;
	module_params.fields_size = AL_ERR_EVENTS_PCIE_APP_MAX_ERRORS;
	module_params.fields = data->fields;
	module_params.fields_template = al_err_events_pcie_app_fields;
	module_params.private_data = data;
	module_params.collect = err_events_pcie_app_collect;
	module_params.print = al_err_events_common_module_print;
	module_params.clear = err_events_pcie_app_clear;
	module_params.test = al_err_events_common_module_test;

	al_err_events_common_module_init(handle, &data->module, &module_params);

	if (data->pcie_port->rev_id < AL_PCIE_REV_ID_4) {
		unsigned int i;
		enum al_err_events_pcie_app ids[] = {
			AL_ERR_EVENTS_PCIE_APP_INTERNAL_OVERFLOW_ERROR,
			AL_ERR_EVENTS_PCIE_APP_SYS_ERR_FUNC_0,
			AL_ERR_EVENTS_PCIE_APP_SYS_ERR_FUNC_1,
			AL_ERR_EVENTS_PCIE_APP_SYS_ERR_FUNC_2,
			AL_ERR_EVENTS_PCIE_APP_SYS_ERR_FUNC_3,
			AL_ERR_EVENTS_PCIE_APP_AER_ERR_INT_0,
			AL_ERR_EVENTS_PCIE_APP_AER_ERR_INT_1,
			AL_ERR_EVENTS_PCIE_APP_AER_ERR_INT_2,
			AL_ERR_EVENTS_PCIE_APP_AER_ERR_INT_3,
			AL_ERR_EVENTS_PCIE_APP_AER_ERR_MSI_0,
			AL_ERR_EVENTS_PCIE_APP_AER_ERR_MSI_1,
			AL_ERR_EVENTS_PCIE_APP_AER_ERR_MSI_2,
			AL_ERR_EVENTS_PCIE_APP_AER_ERR_MSI_3,
			AL_ERR_EVENTS_PCIE_APP_AP_CRCTBL_STAT_0,
			AL_ERR_EVENTS_PCIE_APP_AP_CRCTBL_STAT_1,
			AL_ERR_EVENTS_PCIE_APP_AP_CRCTBL_STAT_2,
			AL_ERR_EVENTS_PCIE_APP_AP_CRCTBL_STAT_3,
			AL_ERR_EVENTS_PCIE_APP_AP_UNCRCTBL_STAT_0,
			AL_ERR_EVENTS_PCIE_APP_AP_UNCRCTBL_STAT_1,
			AL_ERR_EVENTS_PCIE_APP_AP_UNCRCTBL_STAT_2,
			AL_ERR_EVENTS_PCIE_APP_AP_UNCRCTBL_STAT_3,
		};

		for (i = 0; i < AL_ARR_SIZE(ids); i++)
			data->module.fields[ids[i]].valid = AL_FALSE;
	}

	al_err_events_common_module_post_init(&data->module);

	return 0;
}

int al_err_events_pcie_app_int_enable(struct al_err_events_pcie_app_data *data)
{
	uint32_t mask;
	struct al_pcie_port *pcie_port;
	struct al_err_events_module *module;

	al_assert(data);

	module = &data->module;

	if (!module->enabled)
		return -EINVAL;

	pcie_port = data->pcie_port;

	al_pcie_ints_config(pcie_port);

	mask = al_err_events_common_group_mask_get(module, 0, AL_INT_GROUP_B);
	al_iofic_unmask(module->ic_regs_bases[0],
			AL_INT_GROUP_B,
			mask);

	if (data->pcie_port->rev_id >= AL_PCIE_REV_ID_4) {
		mask = al_err_events_common_group_mask_get(module, 0, AL_INT_GROUP_C);
		al_iofic_unmask(module->ic_regs_bases[0],
				AL_INT_GROUP_C,
				mask);
	}

	al_pcie_port_ram_parity_int_config(pcie_port, AL_TRUE);

	return 0;
}

/*******************************************************************************
 ** PCIe AXI
 ******************************************************************************/
static void pcie_axi_error_collect(struct al_err_events_field *field, uint32_t cause)
{
	if (cause & field->iofic_bit) {
		struct al_pcie_axi_parity_stats axi_status;
		struct al_err_events_module *module = field->parent_module;
		struct al_err_events_pcie_axi_data *data =
				(struct al_err_events_pcie_axi_data *)module->private_data;
		struct al_pcie_port *pcie_port = data->pcie_port;

		switch (field->id) {
		case AL_ERR_EVENTS_PCIE_AXI_MASTER_READ_DATA_PARITY_ERROR:
			data->axi_read_data_parity_address =
					al_pcie_axi_read_data_parity_error_addr_get(pcie_port);
			break;
		case AL_ERR_EVENTS_PCIE_AXI_READ_COMPLETION_ERROR:
			data->axi_read_completion_error_address =
					al_pcie_axi_read_compl_error_addr_get(pcie_port);
			break;
		case AL_ERR_EVENTS_PCIE_AXI_WRITE_COMPLETION_ERROR:
			data->axi_write_completion_error_address =
					al_pcie_axi_write_compl_error_addr_get(pcie_port);
			break;
		case AL_ERR_EVENTS_PCIE_AXI_READ_COMPLETION_TIMEOUT:
			data->axi_read_compl_timeout_address =
					al_pcie_axi_read_compl_error_addr_get(pcie_port);
			break;
		case AL_ERR_EVENTS_PCIE_AXI_WRITE_COMPLETION_TIMEOUT:
			data->axi_write_cmpl_timeout_address =
					al_pcie_axi_write_to_error_addr_get(pcie_port);
			break;
		case AL_ERR_EVENTS_PCIE_AXI_POS_ERROR:
			data->axi_pos_error_addr = al_pcie_axi_pos_error_addr_get(pcie_port);
			break;
		case AL_ERR_EVENTS_PCIE_AXI_PARITY_ERROR:
			al_pcie_axi_parity_stats_get(pcie_port, &axi_status);

			if (axi_status.num_of_errors) {
				if (axi_status.u5_ram2p)
					data->axi_parity.u5_ram2p++;
				if (axi_status.u4_ram2p)
					data->axi_parity.u4_ram2p++;
				if (axi_status.u3_ram2p)
					data->axi_parity.u3_ram2p++;
				if (axi_status.u2_ram2p)
					data->axi_parity.u2_ram2p++;

				if (pcie_port->rev_id <= AL_PCIE_REV_ID_2) {
					if (axi_status.u12_ram2p)
						data->axi_parity.u12_ram2p++;
					if (axi_status.u10_ram2p)
						data->axi_parity.u12_ram2p++;
				}
			}
			break;
		default:
			al_err("%s: ERROR unknown field ID %d\n", __func__, field->id);
		}

		al_err_events_common_field_iofic_collect(field, cause);
	}
}

static int err_events_pcie_axi_collect(struct al_err_events_module *module,
				       enum al_err_events_collect collect_mode)
{
	return al_err_events_common_module_iofic_collect(module, collect_mode, 0, AL_INT_GROUP_A);
}

static void pcie_axi_read_data_parity_print(struct al_err_events_field *field,
					    al_bool print_header)
{
	struct al_err_events_module *module = field->parent_module;
	al_err_events_print_cb print_cb = module->handle->print_cb;
	struct al_err_events_pcie_axi_data *data =
			(struct al_err_events_pcie_axi_data *)module->private_data;

	al_err_events_common_field_print(field, print_header);

	if (field->valid && (field->counter > 0))
		print_cb("\tAXI Master Read Data Parity Address: 0x%" PRIx64 "\n",
			 data->axi_read_data_parity_address);
}

static void pcie_axi_read_compl_error_print(struct al_err_events_field *field,
					    al_bool print_header)
{
	struct al_err_events_module *module = field->parent_module;
	al_err_events_print_cb print_cb = module->handle->print_cb;
	struct al_err_events_pcie_axi_data *data =
			(struct al_err_events_pcie_axi_data *)module->private_data;

	al_err_events_common_field_print(field, print_header);

	if (field->valid && (field->counter > 0))
		print_cb("\tAXI Read Completion Error Address: 0x%" PRIx64 "\n",
			 data->axi_read_completion_error_address);
}

static void pcie_axi_write_compl_error_print(struct al_err_events_field *field,
					     al_bool print_header)
{
	struct al_err_events_module *module = field->parent_module;
	al_err_events_print_cb print_cb = module->handle->print_cb;
	struct al_err_events_pcie_axi_data *data =
			(struct al_err_events_pcie_axi_data *)module->private_data;

	al_err_events_common_field_print(field, print_header);

	if (field->valid && (field->counter > 0))
		print_cb("\tAXI Write Completion Error Address: 0x%" PRIx64 "\n",
			 data->axi_write_completion_error_address);
}

static void pcie_axi_read_compl_timeout_print(struct al_err_events_field *field,
					      al_bool print_header)
{
	struct al_err_events_module *module = field->parent_module;
	al_err_events_print_cb print_cb = module->handle->print_cb;
	struct al_err_events_pcie_axi_data *data =
			(struct al_err_events_pcie_axi_data *)module->private_data;

	al_err_events_common_field_print(field, print_header);

	if (field->valid && (field->counter > 0))
		print_cb("\tAXI Read Completion Timeout Address: 0x%" PRIx64 "\n",
			 data->axi_read_compl_timeout_address);
}

static void pcie_axi_write_compl_timeout_print(struct al_err_events_field *field,
					       al_bool print_header)
{
	struct al_err_events_module *module = field->parent_module;
	al_err_events_print_cb print_cb = module->handle->print_cb;
	struct al_err_events_pcie_axi_data *data =
			(struct al_err_events_pcie_axi_data *)module->private_data;

	al_err_events_common_field_print(field, print_header);

	if (field->valid && (field->counter > 0))
		print_cb("\tAXI Write Completion Timeout Address: 0x%" PRIx64 "\n",
			 data->axi_write_cmpl_timeout_address);
}

static void pcie_axi_pos_error_print(struct al_err_events_field *field,
				     al_bool print_header)
{
	struct al_err_events_module *module = field->parent_module;
	al_err_events_print_cb print_cb = module->handle->print_cb;
	struct al_err_events_pcie_axi_data *data =
			(struct al_err_events_pcie_axi_data *)module->private_data;

	al_err_events_common_field_print(field, print_header);

	if (field->valid && (field->counter > 0))
		print_cb("\tAXI PoS Error Address: 0x%" PRIx64 "\n",
			 data->axi_pos_error_addr);
}

static void pcie_axi_parity_error_print(struct al_err_events_field *field,
					al_bool print_header)
{
	struct al_err_events_module *module = field->parent_module;
	al_err_events_print_cb print_cb = module->handle->print_cb;
	struct al_err_events_pcie_axi_data *data =
			(struct al_err_events_pcie_axi_data *)module->private_data;

	al_err_events_common_field_print(field, print_header);

	if (!field->valid || (field->counter == 0))
		return;

	print_cb("\tAXI u5_ram2p SRAM Parity Error: %d\n", data->axi_parity.u5_ram2p);
	print_cb("\tAXI u4_ram2p SRAM Parity Error: %d\n", data->axi_parity.u4_ram2p);
	print_cb("\tAXI u3_ram2p SRAM Parity Error: %d\n", data->axi_parity.u3_ram2p);
	print_cb("\tAXI u2_ram2p SRAM Parity Error: %d\n", data->axi_parity.u2_ram2p);
	if (data->pcie_port->rev_id > AL_PCIE_REV_ID_2) {
		print_cb("\tAXI u12_ram2p SRAM Parity Error: %d\n", data->axi_parity.u12_ram2p);
		print_cb("\tAXI u10_ram2p SRAM Parity Error: %d\n", data->axi_parity.u10_ram2p);
	}
}

static void err_events_pcie_axi_clear(struct al_err_events_module *module)
{
	struct al_err_events_pcie_axi_data *data =
			(struct al_err_events_pcie_axi_data *)module->private_data;

	al_err_events_common_module_clear(module);

	data->axi_read_data_parity_address = 0;
	data->axi_read_completion_error_address = 0;
	data->axi_write_completion_error_address = 0;
	data->axi_read_compl_timeout_address = 0;
	data->axi_write_cmpl_timeout_address = 0;
	data->axi_pos_error_addr = 0;

	al_memset(&data->axi_parity, 0, sizeof(data->axi_parity));
}

int al_err_events_pcie_axi_init(struct al_err_events_handle *handle,
				struct al_err_events_pcie_axi_data *data,
				struct al_err_events_pcie_init_params *params)
{
	struct al_err_events_common_module_params module_params;
	struct al_err_events_module *module;

	al_assert(handle);
	al_assert(data);
	al_assert(params);
	al_assert(sizeof(al_err_events_pcie_axi_fields) == sizeof(data->fields));
	al_assert(sizeof(al_err_events_pcie_axi_fields) ==
		 (AL_ERR_EVENTS_PCIE_AXI_MAX_ERRORS * sizeof(struct al_err_events_field)));

	module = &data->module;

	/* init data */
	data->pcie_port = params->pcie_port;

	/* init module */
	al_memset(&module_params, 0, sizeof(module_params));
	al_sprintf(module_params.name, "PCIe-AXI-Port(%d)", params->pcie_port->port_id);
	module_params.primary_module = AL_ERR_EVENTS_MODULE_PCIE;
	module_params.secondary_module = AL_ERR_EVENTS_SEC_MODULE_PCIE_AXI;
	module_params.primary_index = params->pcie_port->port_id;
	module_params.secondary_index = 0;
	module_params.collect_mode = params->collect_mode;
	module_params.ic_regs_bases[0] = params->pcie_port->regs->axi.int_grp_a;
	module_params.ic_size = 1;
	module_params.fields_size = AL_ERR_EVENTS_PCIE_AXI_MAX_ERRORS;
	module_params.fields = data->fields;
	module_params.fields_template = al_err_events_pcie_axi_fields;
	module_params.private_data = data;
	module_params.collect = err_events_pcie_axi_collect;
	module_params.print = al_err_events_common_module_print;
	module_params.clear = err_events_pcie_axi_clear;
	module_params.test = al_err_events_common_module_test;

	al_err_events_common_module_init(handle, module, &module_params);

	if (data->pcie_port->rev_id > AL_PCIE_REV_ID_2)
		module->fields[AL_ERR_EVENTS_PCIE_AXI_CORE_INTERNAL_DECOMPOSE_LOGIC_ERROR].valid =
				AL_FALSE;

	/**
	 * Addressing RMN: 11096
	 *
	 * RMN description:
	 * False indication of AXI slave data parity interrupt
	 *
	 * Software flow:
	 * Dont use this field on V1
	 */
	if (data->pcie_port->rev_id == AL_PCIE_REV_ID_1)
		module->fields[AL_ERR_EVENTS_PCIE_AXI_SLAVE_WRITE_DATA_PARITY_ERROR].valid =
				AL_FALSE;

	al_err_events_common_module_post_init(module);

	return 0;
}

int al_err_events_pcie_axi_int_enable(struct al_err_events_pcie_axi_data *data)
{
	uint32_t mask;
	struct al_pcie_port *pcie_port;
	struct al_err_events_module *module;

	al_assert(data);

	module = &data->module;

	if (!module->enabled)
		return -EINVAL;

	pcie_port = data->pcie_port;

	al_pcie_ints_config(pcie_port);

	mask = al_err_events_common_group_mask_get(module, 0, AL_INT_GROUP_A);
	al_iofic_unmask(module->ic_regs_bases[0],
			AL_INT_GROUP_A,
			mask);

	al_pcie_port_axi_parity_int_config(pcie_port, AL_TRUE);

	return 0;
}
