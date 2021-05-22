/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#include "al_mod_err_events_internal.h"
#include "al_mod_err_events_io_fabric.h"
#include "al_mod_hal_pbs_iofic.h"
#include "al_mod_hal_udma_iofic.h"
#include "al_mod_hal_pbs_utils.h"
#include "al_mod_hal_serdes_25g.h"
#include "al_mod_hal_serdes_avg.h"
#include "al_mod_hal_unit_adapter_regs.h"

static void err_events_ua_axi_print(struct al_mod_err_events_field *field, al_mod_bool print_header);
static void err_events_ua_axi_parity_collect(struct al_mod_err_events_field *field, uint32_t cause);
static void err_events_ua_axi_error_collect(struct al_mod_err_events_field *field, uint32_t cause);
static void err_events_ua_axi_mask(struct al_mod_err_events_field *field);

/*******************************************************************************
 ** Static error fields
 ******************************************************************************/
static const struct al_mod_err_events_field al_mod_err_events_pbs_fields[AL_ERR_EVENTS_PBS_MAX_ERRORS] = {
	{
		.name = "SW SRAM Parity Error",
		.id = AL_ERR_EVENTS_PBS_SW_SRAM_PARITY,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_PBS_IOFIC_INT_ID_BOOTROOM_PARITY_ERROR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.print = al_mod_err_events_common_field_print,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "UFC SRAM Parity Error",
		.id = AL_ERR_EVENTS_PBS_UFC_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_PBS_IOFIC_INT_ID_UFC_PARITY_ERROR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.print = al_mod_err_events_common_field_print,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "APB PREADY Without PSEL Error",
		.id = AL_ERR_EVENTS_PBS_APB_PREADY_NO_PSEL_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_PBS_IOFIC_INT_ID_APB_PREADY_NO_PSEL_ERROR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.print = al_mod_err_events_common_field_print,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "APB Double PREADY Error",
		.id = AL_ERR_EVENTS_PBS_APB_DOUBLE_PREADY_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_PBS_IOFIC_INT_ID_APB_DOUBLE_PREADY_ERROR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.print = al_mod_err_events_common_field_print,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "TDM TX SRAM Parity Error",
		.id = AL_ERR_EVENTS_PBS_TDM_TX_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_PBS_IOFIC_INT_ID_TDM_TX_PARITY_ERROR,
		.sev = AL_ERR_EVENTS_CORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.print = al_mod_err_events_common_field_print,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "TDM RX SRAM Parity Error",
		.id = AL_ERR_EVENTS_PBS_TDM_RX_PARITY_ERROR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_PBS_IOFIC_INT_ID_TDM_RX_PARITY_ERROR,
		.sev = AL_ERR_EVENTS_CORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.print = al_mod_err_events_common_field_print,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
};

static const struct al_mod_err_events_field
	al_mod_err_events_serdes_fields[AL_ERR_EVENTS_SERDES_MAX_ERRORS] = {
	{
		.name = "Internal SRAM Parity",
		.id = AL_ERR_EVENTS_SERDES_INTERNAL_SRAM_PARITY,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SERDES_25G_IOFIC_INT_ID_INTERNAL_SRAM_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.print = al_mod_err_events_common_field_print,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "Data Memory Parity",
		.id = AL_ERR_EVENTS_SERDES_DATA_MEMORY_PARITY,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SERDES_25G_IOFIC_INT_ID_DATA_MEM_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.print = al_mod_err_events_common_field_print,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "Program Memory Parity",
		.id = AL_ERR_EVENTS_SERDES_DATA_PROGRAM_PARITY,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit = AL_SERDES_25G_IOFIC_INT_ID_PROG_MEM_PARITY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.print = al_mod_err_events_common_field_print,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
};

static const struct al_mod_err_events_field
	al_mod_err_events_serdes_avg_fields[AL_ERR_EVENTS_SERDES_AVG_MAX_ERRORS] = {
	{
		.name = "single bit error indication in the SerDes memories (corrected by ecc)",
		.id = AL_ERR_EVENTS_SERDES_AVG_SINGLE_BIT_ERR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit =
			AL_SRDS_AVG_INT_GROUP_A_SINGLE_BIT_ERROR_INDICATION_SERDES_MEMORIES,
		.sev = AL_ERR_EVENTS_CORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.print = al_mod_err_events_common_field_print,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
	{
		.name = "Double bit error indication in the SerDes memory",
		.id = AL_ERR_EVENTS_SERDES_AVG_DOUBLE_BIT_ERR,
		.iofic_group = AL_INT_GROUP_A,
		.iofic_bit =
			AL_SRDS_AVG_INT_GROUP_A_DOUBLE_BIT_ERROR_INDICATION_IN_SERDES_MEMORY,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.collect = al_mod_err_events_common_field_iofic_collect,
		.print = al_mod_err_events_common_field_print,
		.mask = al_mod_err_events_common_field_iofic_mask,
	},
};

static const struct al_mod_err_events_field
	al_mod_err_events_ua_fields[AL_ERR_EVENTS_UNIT_ADAPTER_MAX_ERRORS] = {
	{
		.name = "AXI Read Parity Error",
		.id = AL_ERR_EVENTS_UNIT_ADAPTER_AXI_READ_PARITY_ERROR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.iofic_group = AL_INT_GROUP_A,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = err_events_ua_axi_print,
		.collect = err_events_ua_axi_error_collect,
		.mask = err_events_ua_axi_mask,
	},
	{
		.name = "AXI Read BME Blocked Error",
		.id = AL_ERR_EVENTS_UNIT_ADAPTER_AXI_READ_ERROR_BLOCKED,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.iofic_group = AL_INT_GROUP_A,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = err_events_ua_axi_print,
		.collect = err_events_ua_axi_error_collect,
		.mask = err_events_ua_axi_mask,
	},
	{
		.name = "AXI Read Completion Timeout",
		.id = AL_ERR_EVENTS_UNIT_ADAPTER_AXI_READ_COMPLETION_TIMEOUT,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.iofic_group = AL_INT_GROUP_A,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = err_events_ua_axi_print,
		.collect = err_events_ua_axi_error_collect,
		.mask = err_events_ua_axi_mask,
	},
	{
		.name = "AXI Read Completion Error",
		.id = AL_ERR_EVENTS_UNIT_ADAPTER_AXI_READ_COMPLETION_ERROR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.iofic_group = AL_INT_GROUP_A,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = err_events_ua_axi_print,
		.collect = err_events_ua_axi_error_collect,
		.mask = err_events_ua_axi_mask,
	},
	{
		.name = "AXI Read Address Timeout",
		.id = AL_ERR_EVENTS_UNIT_ADAPTER_AXI_READ_ADDRESS_TIMEOUT,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.iofic_group = AL_INT_GROUP_A,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = err_events_ua_axi_print,
		.collect = err_events_ua_axi_error_collect,
		.mask = err_events_ua_axi_mask,
	},
	{
		.name = "AXI Write BME Blocked Error",
		.id = AL_ERR_EVENTS_UNIT_ADAPTER_AXI_WRITE_ERROR_BLOCKED,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.iofic_group = AL_INT_GROUP_B,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = err_events_ua_axi_print,
		.collect = err_events_ua_axi_error_collect,
		.mask = err_events_ua_axi_mask,
	},
	{
		.name = "AXI Write Completion Timeout",
		.id = AL_ERR_EVENTS_UNIT_ADAPTER_AXI_WRITE_COMPLETION_TIMEOUT,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.iofic_group = AL_INT_GROUP_B,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = err_events_ua_axi_print,
		.collect = err_events_ua_axi_error_collect,
		.mask = err_events_ua_axi_mask,
	},
	{
		.name = "AXI Write Completion Error",
		.id = AL_ERR_EVENTS_UNIT_ADAPTER_AXI_WRITE_COMPLETION_ERROR,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.iofic_group = AL_INT_GROUP_B,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = err_events_ua_axi_print,
		.collect = err_events_ua_axi_error_collect,
		.mask = err_events_ua_axi_mask,
	},
	{
		.name = "AXI Write Address Timeout",
		.id = AL_ERR_EVENTS_UNIT_ADAPTER_AXI_WRITE_ADDRESS_TIMEOUT,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.iofic_group = AL_INT_GROUP_B,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = err_events_ua_axi_print,
		.collect = err_events_ua_axi_error_collect,
		.mask = err_events_ua_axi_mask,
	},
	{
		.name = "AXI ROB SRAM Out Parity",
		.id = AL_ERR_EVENTS_UNIT_ADAPTER_AXI_ROB_OUT_SRAM_PARITY,
		.iofic_bit = AL_ADAPTER_APP_PARITY_STATUS_DBG_AXI_UROB_SRAM_OUT,
		.iofic_group = AL_INT_GROUP_C,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = err_events_ua_axi_parity_collect,
		.mask = err_events_ua_axi_mask,
	},
	{
		.name = "AXI ROB SRAM In Parity",
		.id = AL_ERR_EVENTS_UNIT_ADAPTER_AXI_ROB_IN_SRAM_PARITY,
		.iofic_bit = AL_ADAPTER_APP_PARITY_STATUS_DBG_AXI_UROB_SRAM_IN,
		.iofic_group = AL_INT_GROUP_C,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = err_events_ua_axi_parity_collect,
		.mask = err_events_ua_axi_mask,
	},
	{
		.name = "SATA Port 0 TX RAM Parity",
		.id = AL_ERR_EVENTS_UNIT_ADAPTER_SATA_PORT_0_TX_RAM_PARITY,
		.iofic_bit = AL_ADAPTER_APP_PARITY_STATUS_SATA_PORT_0_TX_RAM,
		.iofic_group = AL_INT_GROUP_C,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = err_events_ua_axi_parity_collect,
		.mask = err_events_ua_axi_mask,
	},

	{
		.name = "SATA Port 1 TX RAM Parity",
		.id = AL_ERR_EVENTS_UNIT_ADAPTER_SATA_PORT_1_TX_RAM_PARITY,
		.iofic_bit = AL_ADAPTER_APP_PARITY_STATUS_SATA_PORT_1_TX_RAM,
		.iofic_group = AL_INT_GROUP_C,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = err_events_ua_axi_parity_collect,
		.mask = err_events_ua_axi_mask,
	},
	{
		.name = "SATA Port 2 TX RAM Parity",
		.id = AL_ERR_EVENTS_UNIT_ADAPTER_SATA_PORT_2_TX_RAM_PARITY,
		.iofic_bit = AL_ADAPTER_APP_PARITY_STATUS_SATA_PORT_2_TX_RAM,
		.iofic_group = AL_INT_GROUP_C,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = err_events_ua_axi_parity_collect,
		.mask = err_events_ua_axi_mask,
	},
	{
		.name = "SATA Port 3 TX RAM Parity",
		.id = AL_ERR_EVENTS_UNIT_ADAPTER_SATA_PORT_3_TX_RAM_PARITY,
		.iofic_bit = AL_ADAPTER_APP_PARITY_STATUS_SATA_PORT_3_TX_RAM,
		.iofic_group = AL_INT_GROUP_C,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = err_events_ua_axi_parity_collect,
		.mask = err_events_ua_axi_mask,
	},
	{
		.name = "SATA Port 0 RX RAM Parity",
		.id = AL_ERR_EVENTS_UNIT_ADAPTER_SATA_PORT_0_RX_RAM_PARITY,
		.iofic_bit = AL_ADAPTER_APP_PARITY_STATUS_SATA_PORT_0_RX_RAM,
		.iofic_group = AL_INT_GROUP_C,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = err_events_ua_axi_parity_collect,
		.mask = err_events_ua_axi_mask,
	},
	{
		.name = "SATA Port 1 RX RAM Parity",
		.id = AL_ERR_EVENTS_UNIT_ADAPTER_SATA_PORT_1_RX_RAM_PARITY,
		.iofic_bit = AL_ADAPTER_APP_PARITY_STATUS_SATA_PORT_1_RX_RAM,
		.iofic_group = AL_INT_GROUP_C,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = err_events_ua_axi_parity_collect,
		.mask = err_events_ua_axi_mask,
	},
	{
		.name = "SATA Port 2 RX RAM Parity",
		.id = AL_ERR_EVENTS_UNIT_ADAPTER_SATA_PORT_2_RX_RAM_PARITY,
		.iofic_bit = AL_ADAPTER_APP_PARITY_STATUS_SATA_PORT_2_RX_RAM,
		.iofic_group = AL_INT_GROUP_C,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = err_events_ua_axi_parity_collect,
		.mask = err_events_ua_axi_mask,
	},
	{
		.name = "SATA Port 3 RX RAM Parity",
		.id = AL_ERR_EVENTS_UNIT_ADAPTER_SATA_PORT_3_RX_RAM_PARITY,
		.iofic_bit = AL_ADAPTER_APP_PARITY_STATUS_SATA_PORT_3_RX_RAM,
		.iofic_group = AL_INT_GROUP_C,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = err_events_ua_axi_parity_collect,
		.mask = err_events_ua_axi_mask,
	},
	{
		.name = "SATA Port 0 FB Low RAM Parity",
		.id = AL_ERR_EVENTS_UNIT_ADAPTER_SATA_PORT_0_FB_LOW_RAM_PARITY,
		.iofic_bit = AL_ADAPTER_APP_PARITY_STATUS_SATA_PORT_0_FB_LOW_RAM,
		.iofic_group = AL_INT_GROUP_C,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = err_events_ua_axi_parity_collect,
		.mask = err_events_ua_axi_mask,
	},
	{
		.name = "SATA Port 1 FB Low RAM Parity",
		.id = AL_ERR_EVENTS_UNIT_ADAPTER_SATA_PORT_1_FB_LOW_RAM_PARITY,
		.iofic_bit = AL_ADAPTER_APP_PARITY_STATUS_SATA_PORT_1_FB_LOW_RAM,
		.iofic_group = AL_INT_GROUP_C,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = err_events_ua_axi_parity_collect,
		.mask = err_events_ua_axi_mask,
	},
	{
		.name = "SATA Port 2 FB Low RAM Parity",
		.id = AL_ERR_EVENTS_UNIT_ADAPTER_SATA_PORT_2_FB_LOW_RAM_PARITY,
		.iofic_bit = AL_ADAPTER_APP_PARITY_STATUS_SATA_PORT_2_FB_LOW_RAM,
		.iofic_group = AL_INT_GROUP_C,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = err_events_ua_axi_parity_collect,
		.mask = err_events_ua_axi_mask,
	},
	{
		.name = "SATA Port 3 FB Low RAM Parity",
		.id = AL_ERR_EVENTS_UNIT_ADAPTER_SATA_PORT_3_FB_LOW_RAM_PARITY,
		.iofic_bit = AL_ADAPTER_APP_PARITY_STATUS_SATA_PORT_3_FB_LOW_RAM,
		.iofic_group = AL_INT_GROUP_C,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = err_events_ua_axi_parity_collect,
		.mask = err_events_ua_axi_mask,
	},
	{
		.name = "SATA Port 0 FB High RAM Parity",
		.id = AL_ERR_EVENTS_UNIT_ADAPTER_SATA_PORT_0_FB_HIGH_RAM_PARITY,
		.iofic_bit = AL_ADAPTER_APP_PARITY_STATUS_SATA_PORT_0_FB_HIGH_RAM,
		.iofic_group = AL_INT_GROUP_C,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = err_events_ua_axi_parity_collect,
		.mask = err_events_ua_axi_mask,
	},
	{
		.name = "SATA Port 1 FB High RAM Parity",
		.id = AL_ERR_EVENTS_UNIT_ADAPTER_SATA_PORT_1_FB_HIGH_RAM_PARITY,
		.iofic_bit = AL_ADAPTER_APP_PARITY_STATUS_SATA_PORT_1_FB_HIGH_RAM,
		.iofic_group = AL_INT_GROUP_C,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = err_events_ua_axi_parity_collect,
		.mask = err_events_ua_axi_mask,
	},
	{
		.name = "SATA Port 2 FB High RAM Parity",
		.id = AL_ERR_EVENTS_UNIT_ADAPTER_SATA_PORT_2_FB_HIGH_RAM_PARITY,
		.iofic_bit = AL_ADAPTER_APP_PARITY_STATUS_SATA_PORT_2_FB_HIGH_RAM,
		.iofic_group = AL_INT_GROUP_C,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = err_events_ua_axi_parity_collect,
		.mask = err_events_ua_axi_mask,
	},
	{
		.name = "SATA Port 3 FB High RAM Parity",
		.id = AL_ERR_EVENTS_UNIT_ADAPTER_SATA_PORT_3_FB_HIGH_RAM_PARITY,
		.iofic_bit = AL_ADAPTER_APP_PARITY_STATUS_SATA_PORT_3_FB_HIGH_RAM,
		.iofic_group = AL_INT_GROUP_C,
		.sev = AL_ERR_EVENTS_UNCORRECTABLE,
		.collect_mode = AL_ERR_EVENTS_COLLECT_INTERRUPT,
		.print = al_mod_err_events_common_field_print,
		.collect = err_events_ua_axi_parity_collect,
		.mask = err_events_ua_axi_mask,
	},
};

/*******************************************************************************
 ** PBS
 ******************************************************************************/
static int err_events_pbs_collect(struct al_mod_err_events_module *module,
				  enum al_mod_err_events_collect collect)
{
	return al_mod_err_events_common_module_iofic_collect(module, collect, 0, AL_INT_GROUP_A);
}

int al_mod_err_events_pbs_init(struct al_mod_err_events_handle *handle,
			   struct al_mod_err_events_pbs_data *data,
			   struct al_mod_err_events_pbs_init_params *params)
{
	unsigned int rev_id;
	struct al_mod_err_events_common_module_params module_params;

	al_mod_assert(handle);
	al_mod_assert(data);
	al_mod_assert(params);
	al_mod_assert(sizeof(al_mod_err_events_pbs_fields) == sizeof(data->fields));
	al_mod_assert(sizeof(al_mod_err_events_pbs_fields) ==
		 (AL_ERR_EVENTS_PBS_MAX_ERRORS * sizeof(struct al_mod_err_events_field)));

	al_mod_memset(&module_params, 0, sizeof(module_params));

	/* init module */
	al_mod_sprintf(module_params.name, "PBS");
	module_params.primary_module = AL_ERR_EVENTS_MODULE_PBS;
	module_params.secondary_module = AL_ERR_EVENTS_SEC_MODULE_NONE;
	module_params.primary_index = 0;
	module_params.secondary_index = 0;
	module_params.collect_mode = params->collect_mode;
	module_params.ic_regs_bases[0] = al_mod_pbs_iofic_regs_get(handle->pbs_regs_base);
	module_params.ic_size = 1;
	module_params.fields_size = AL_ERR_EVENTS_PBS_MAX_ERRORS;
	module_params.fields = data->fields;
	module_params.fields_template = al_mod_err_events_pbs_fields;
	module_params.private_data = data;
	module_params.collect = err_events_pbs_collect;
	module_params.print = al_mod_err_events_common_module_print;
	module_params.clear = al_mod_err_events_common_module_clear;
	module_params.test = al_mod_err_events_common_module_test;
	module_params.set_cause = al_mod_err_events_common_field_set_cause;
	module_params.get_attrs = al_mod_err_events_common_field_get_attrs;

	al_mod_err_events_common_module_init(handle, &data->module, &module_params);

	rev_id = al_mod_pbs_dev_id_get(handle->pbs_regs_base);
	if (rev_id == PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V1) {
		data->module.fields[AL_ERR_EVENTS_PBS_TDM_TX_PARITY_ERROR].valid = AL_FALSE;
		data->module.fields[AL_ERR_EVENTS_PBS_TDM_RX_PARITY_ERROR].valid = AL_FALSE;
	}

	al_mod_err_events_common_module_post_init(&data->module);

	return 0;
}

int al_mod_err_events_pbs_ints_enable(struct al_mod_err_events_pbs_data *data)
{
	struct al_mod_err_events_module *module;

	al_mod_assert(data);

	module = &data->module;

	if (!module->enabled)
		return -EINVAL;

	al_mod_iofic_config(module->ic_regs_bases[0],
			AL_INT_GROUP_A,
			INT_CONTROL_GRP_MASK_MSI_X);

	al_mod_iofic_unmask(module->ic_regs_bases[0],
			AL_INT_GROUP_A,
			al_mod_err_events_common_group_mask_get(module, 0, AL_INT_GROUP_A));


	return 0;
}

/*******************************************************************************
 ** Serdes 25G
 ******************************************************************************/
static int err_events_serdes_25g_collect(struct al_mod_err_events_module *module,
					 enum al_mod_err_events_collect collect_mode)
{
	return al_mod_err_events_common_module_iofic_collect(module, collect_mode, 0, AL_INT_GROUP_A);
}

int al_mod_err_events_serdes_25g_init(struct al_mod_err_events_handle *handle,
				  struct al_mod_err_events_serdes_25g_data *data,
				  struct al_mod_err_events_serdes_25g_init_params *params)
{

	unsigned int rev_id;
	struct al_mod_err_events_common_module_params module_params;

	al_mod_assert(handle);
	al_mod_assert(data);
	al_mod_assert(params);
	al_mod_assert(sizeof(al_mod_err_events_serdes_fields) == sizeof(data->fields));
	al_mod_assert(sizeof(al_mod_err_events_serdes_fields) ==
		 (AL_ERR_EVENTS_SERDES_MAX_ERRORS * sizeof(struct al_mod_err_events_field)));

	rev_id = al_mod_pbs_dev_id_get(handle->pbs_regs_base);
	al_mod_assert(rev_id == PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V2);

	/* init module */
	al_mod_memset(&module_params, 0, sizeof(module_params));
	al_mod_sprintf(module_params.name, "SERDES-25G");
	module_params.primary_module = AL_ERR_EVENTS_MODULE_SERDES_25G;
	module_params.secondary_module = AL_ERR_EVENTS_SEC_MODULE_NONE;
	module_params.primary_index = 0;
	module_params.secondary_index = 0;
	module_params.ic_regs_bases[0] = params->unit_regs_base;
	module_params.ic_size = 1;
	module_params.collect_mode = params->collect_mode;
	module_params.fields_size = AL_ERR_EVENTS_SERDES_MAX_ERRORS;
	module_params.fields = data->fields;
	module_params.fields_template = al_mod_err_events_serdes_fields;
	module_params.private_data = data;
	module_params.collect = err_events_serdes_25g_collect;
	module_params.print = al_mod_err_events_common_module_print;
	module_params.clear = al_mod_err_events_common_module_clear;
	module_params.test = al_mod_err_events_common_module_test;
	module_params.set_cause = al_mod_err_events_common_field_set_cause;
	module_params.get_attrs = al_mod_err_events_common_field_get_attrs;

	al_mod_err_events_common_module_init(handle, &data->module, &module_params);

	al_mod_err_events_common_module_post_init(&data->module);

	return 0;
}

int al_mod_err_events_serdes_25g_ints_enable(struct al_mod_err_events_serdes_25g_data *data)
{
	struct al_mod_err_events_module *module;

	al_mod_assert(data);

	module = &data->module;

	if (!module->enabled)
		return -EINVAL;

	al_mod_iofic_config(module->ic_regs_bases[0],
			AL_INT_GROUP_A,
			INT_CONTROL_GRP_MASK_MSI_X);

	/* Clear program memory parity indication because
	 * of early false positive parity that
	 * happen before serdes FW init
	 */
	al_mod_iofic_clear_cause(
			module->ic_regs_bases[0],
			AL_INT_GROUP_A,
			module->fields[AL_ERR_EVENTS_SERDES_DATA_PROGRAM_PARITY].iofic_bit);

	al_mod_iofic_unmask(module->ic_regs_bases[0],
			AL_INT_GROUP_A,
			al_mod_err_events_common_group_mask_get(module, 0, AL_INT_GROUP_A));

	return 0;
}

/*******************************************************************************
 ** AVG Serdes
 ******************************************************************************/
static int err_events_serdes_avg_collect(struct al_mod_err_events_module *module,
					 enum al_mod_err_events_collect collect_mode)
{
	return al_mod_err_events_common_module_iofic_collect(module, collect_mode, 0, AL_INT_GROUP_A);
}

int al_mod_err_events_serdes_avg_init(struct al_mod_err_events_handle *handle,
				  struct al_mod_err_events_serdes_avg_data *data,
				  struct al_mod_err_events_serdes_avg_init_params *params)
{

	unsigned int rev_id;
	struct al_mod_err_events_common_module_params module_params;

	al_mod_assert(handle);
	al_mod_assert(data);
	al_mod_assert(params);
	al_mod_assert(params->complex_obj);
	al_mod_assert(params->regs_base);
	al_mod_assert(sizeof(al_mod_err_events_serdes_fields) == sizeof(data->fields));
	al_mod_assert(sizeof(al_mod_err_events_serdes_fields) ==
		 (AL_ERR_EVENTS_SERDES_AVG_MAX_ERRORS * sizeof(struct al_mod_err_events_field)));

	rev_id = al_mod_pbs_dev_id_get(handle->pbs_regs_base);
	al_mod_assert(rev_id == PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V3);

	al_mod_serdes_avg_handle_init(
		params->regs_base, params->complex_obj, params->lane_in_complex, &data->obj);

	/* init module */
	al_mod_memset(&module_params, 0, sizeof(struct al_mod_err_events_common_module_params));
	al_mod_sprintf(module_params.name, "SERDES-AVG");
	module_params.primary_module = AL_ERR_EVENTS_MODULE_SERDES_AVG;
	module_params.secondary_module = AL_ERR_EVENTS_SEC_MODULE_NONE;
	module_params.primary_index = 0;
	module_params.secondary_index = 0;
	module_params.ic_regs_bases[0] = data->obj.int_mem_base;
	module_params.ic_size = 1;
	module_params.collect_mode = params->collect_mode;
	module_params.fields_size = AL_ERR_EVENTS_SERDES_AVG_MAX_ERRORS;
	module_params.fields = data->fields;
	module_params.fields_template = al_mod_err_events_serdes_avg_fields;
	module_params.private_data = data;
	module_params.collect = err_events_serdes_avg_collect;
	module_params.print = al_mod_err_events_common_module_print;
	module_params.clear = al_mod_err_events_common_module_clear;
	module_params.test = al_mod_err_events_common_module_test;
	module_params.set_cause = al_mod_err_events_common_field_set_cause;
	module_params.get_attrs = al_mod_err_events_common_field_get_attrs;

	al_mod_err_events_common_module_init(handle, &data->module, &module_params);

	al_mod_err_events_common_module_post_init(&data->module);

	return 0;
}

int al_mod_err_events_serdes_avg_ints_enable(struct al_mod_err_events_serdes_avg_data *data)
{
	struct al_mod_err_events_module *module;
	uint32_t a, b, c, d;

	al_mod_assert(data);

	module = &data->module;

	if (!module->enabled)
		return -EINVAL;

	/** Unmask interrupts */
	data->obj.errors_unmask(&data->obj);
	/** Update valid fields */
	data->obj.error_masks_get(&data->obj, &a, &b, &c, &d);
	al_mod_err_event_common_update_valid_fields(&data->module, 0, a, b, c, d);

	return 0;
}

/*******************************************************************************
 ** Unit adapter
 ******************************************************************************/
static void err_events_ua_axi_mask(struct al_mod_err_events_field *field)
{
	struct al_mod_err_events_module *module = field->parent_module;
	struct al_mod_err_events_unit_adapter_data *data =
			(struct al_mod_err_events_unit_adapter_data *)module->private_data;

	switch (field->iofic_group) {
	case AL_INT_GROUP_A:
		data->int_fields.rd_err = AL_FALSE;
		break;
	case AL_INT_GROUP_B:
		data->int_fields.wr_err = AL_FALSE;
		break;
	case AL_INT_GROUP_C:
		data->int_fields.app_parity_error = AL_FALSE;
		break;
	}

	al_mod_unit_adapter_int_cause_mask_set_ex(&data->unit_adapter, &data->int_fields);
}

static void err_events_ua_axi_error_collect(struct al_mod_err_events_field *field,
					    uint32_t cause __attribute__((unused)))
{
	struct al_mod_err_events_module *module = field->parent_module;
	struct al_mod_err_events_unit_adapter_data *data =
			(struct al_mod_err_events_unit_adapter_data *)module->private_data;
	struct al_mod_unit_adapter_err_attr *axi_err = &data->axi_err;

	switch (field->id) {
	case AL_ERR_EVENTS_UNIT_ADAPTER_AXI_WRITE_ERROR_BLOCKED:
	case AL_ERR_EVENTS_UNIT_ADAPTER_AXI_READ_ERROR_BLOCKED:
		if (axi_err->error_blocked)
			goto error_found;
		break;
	case AL_ERR_EVENTS_UNIT_ADAPTER_AXI_WRITE_COMPLETION_TIMEOUT:
	case AL_ERR_EVENTS_UNIT_ADAPTER_AXI_READ_COMPLETION_TIMEOUT:
		if (axi_err->completion_timeout)
			goto error_found;
		break;
	case AL_ERR_EVENTS_UNIT_ADAPTER_AXI_WRITE_COMPLETION_ERROR:
	case AL_ERR_EVENTS_UNIT_ADAPTER_AXI_READ_COMPLETION_ERROR:
		if (axi_err->completion_error)
			goto error_found;
		break;
	case AL_ERR_EVENTS_UNIT_ADAPTER_AXI_WRITE_ADDRESS_TIMEOUT:
	case AL_ERR_EVENTS_UNIT_ADAPTER_AXI_READ_ADDRESS_TIMEOUT:
		if (axi_err->address_timeout)
			goto error_found;
		break;
	case AL_ERR_EVENTS_UNIT_ADAPTER_AXI_READ_PARITY_ERROR:
		if (axi_err->read_parity_error)
			goto error_found;
		break;
	default:
		al_mod_err("%s: ERROR unknown field ID %d\n", __func__, field->id);
	}

	return;

error_found:
	al_mod_err_events_common_field_error(field, 1);
}

static void err_events_ua_axi_parity_collect(struct al_mod_err_events_field *field, uint32_t cause)
{
	if (cause & field->iofic_bit)
		al_mod_err_events_common_field_error(field, 1);
}

static void err_events_ua_fields_collect(struct al_mod_err_events_module *module,
					 enum al_mod_err_events_collect collect_mode,
					 unsigned int iofic_group,
					 uint32_t cause)
{
	unsigned int i;

	for (i = 0; i < module->fields_size; i++) {
		struct al_mod_err_events_field *field = &module->fields[i];

		if (!field->valid ||
		    (field->collect_mode != collect_mode) ||
		    (field->iofic_group != iofic_group))
			continue;

		if (collect_mode == AL_ERR_EVENTS_COLLECT_INTERRUPT) {
			struct al_mod_err_events_unit_adapter_data *data =
				(struct al_mod_err_events_unit_adapter_data *)module->private_data;

			switch (field->iofic_group) {
			case AL_INT_GROUP_A:
				if (!data->int_fields.rd_err)
					continue;
				break;
			case AL_INT_GROUP_B:
				if (!data->int_fields.wr_err)
					continue;
				break;
			case AL_INT_GROUP_C:
				if (!data->int_fields.app_parity_error)
					continue;
				break;
			}
		}

		field->collect(field, cause);
	}
}

static int err_events_unit_adapter_collect(struct al_mod_err_events_module *module,
					   enum al_mod_err_events_collect collect_mode)
{
	uint32_t cause;
	struct al_mod_err_events_unit_adapter_data *data =
			(struct al_mod_err_events_unit_adapter_data *)module->private_data;
	struct al_mod_unit_adapter_int_fields int_fields;

	al_mod_unit_adapter_int_cause_get_and_clear_ex(&data->unit_adapter, &int_fields);

	if (int_fields.wr_err) {
		data->axi_err_type = AL_ERR_EVENTS_AXI_ERR_TYPE_WRITE;
		al_mod_unit_adapter_axi_master_wr_err_attr_get_and_clear(&data->unit_adapter,
								     &data->axi_err);
		err_events_ua_fields_collect(module, collect_mode, AL_INT_GROUP_B, 0);
	}

	if (int_fields.rd_err) {
		data->axi_err_type = AL_ERR_EVENTS_AXI_ERR_TYPE_READ;
		al_mod_unit_adapter_axi_master_rd_err_attr_get_and_clear(&data->unit_adapter,
								     &data->axi_err);
		err_events_ua_fields_collect(module, collect_mode, AL_INT_GROUP_A, 0);
	}

	if (int_fields.app_parity_error) {
		cause = al_mod_unit_adapter_app_parity_status_get_and_clear(&data->unit_adapter);
		err_events_ua_fields_collect(module, collect_mode, AL_INT_GROUP_C, cause);
	}

	return 0;
}

static void err_events_ua_axi_err_print(struct al_mod_unit_adapter_err_attr *axi_err,
	al_mod_err_events_print_cb print_cb, unsigned int alpine_dev_id)
{
	print_cb("\tMaster ID: 0x%x\n", axi_err->error_master_id);
	print_cb("\tCompletion Status: 0x%x\n", axi_err->error_completion_status);
	/** Address logging is available on Alpine V1 & V2 */
	if (alpine_dev_id < PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V3)
		print_cb("\tAddress: 0x%08x%08x\n",
			axi_err->error_address_latch_hi,
			axi_err->error_address_latch_lo);
}

static void err_events_ua_axi_print(struct al_mod_err_events_field *field, al_mod_bool print_header)
{
	struct al_mod_err_events_module *module = field->parent_module;
	al_mod_err_events_print_cb print_cb = module->handle->print_cb;
	struct al_mod_err_events_unit_adapter_data *data =
			(struct al_mod_err_events_unit_adapter_data *)module->private_data;

	if (!field->valid) {
		if (print_header)
			print_cb("%s %s: Ignored\n", module->name, field->name);
		else
			print_cb("\t%s: Ignored\n", field->name);
		return;
	}

	if (print_header) {
		print_cb("%s %s: %d\n", module->name, field->name, field->counter);
		err_events_ua_axi_err_print(&data->axi_err, print_cb, module->handle->dev_id);
	} else
		print_cb("\t%s: %d\n", field->name, field->counter);
}

static void al_mod_err_events_unit_adapter_print(struct al_mod_err_events_module *module)
{
	al_mod_err_events_print_cb print_cb = module->handle->print_cb;
	struct al_mod_err_events_unit_adapter_data *data =
			(struct al_mod_err_events_unit_adapter_data *)module->private_data;

	al_mod_err_events_common_module_print(module);

	if (data->axi_err_type != AL_ERR_EVENTS_AXI_ERR_TYPE_NONE) {
		print_cb("\tLast AXI bus master error (%s):\n",
			 (data->axi_err_type == AL_ERR_EVENTS_AXI_ERR_TYPE_READ) ?
			 "read" : "write");
		err_events_ua_axi_err_print(&data->axi_err, print_cb, module->handle->dev_id);
	}
}

static void al_mod_err_events_unit_adapter_clear(struct al_mod_err_events_module *module)
{
	struct al_mod_err_events_unit_adapter_data *data =
			(struct al_mod_err_events_unit_adapter_data *)module->private_data;

	al_mod_err_events_common_module_clear(module);

	al_mod_memset(&data->axi_err, 0, sizeof(data->axi_err));

	data->axi_err_type = AL_ERR_EVENTS_AXI_ERR_TYPE_NONE;
}

int al_mod_err_events_unit_adapter_init(struct al_mod_err_events_handle *handle,
				    struct al_mod_err_events_unit_adapter_data *data,
				    struct al_mod_err_events_ua_init_params *params)
{
	unsigned int i;
	struct al_mod_err_events_common_module_params module_params;

	al_mod_assert(handle);
	al_mod_assert(data);
	al_mod_assert(params);
	al_mod_assert(sizeof(al_mod_err_events_ua_fields) == sizeof(data->fields));
	al_mod_assert(sizeof(al_mod_err_events_ua_fields) ==
		 (AL_ERR_EVENTS_UNIT_ADAPTER_MAX_ERRORS * sizeof(struct al_mod_err_events_field)));

	/* init data */
	al_mod_unit_adapter_handle_init(&data->unit_adapter,
				    params->type,
				    params->ua_regs_base,
				    NULL, NULL, NULL, NULL);
	al_mod_memset(&data->int_fields, 0, sizeof(struct al_mod_unit_adapter_int_fields));

	/* init module */
	al_mod_memset(&module_params, 0, sizeof(module_params));

	switch (params->type) {
	case AL_UNIT_ADAPTER_TYPE_ETH:
		al_mod_sprintf(module_params.name, "ETH(%d)-Unit-Adapter", params->index);
		module_params.primary_module = AL_ERR_EVENTS_MODULE_ETH;
		break;
	case AL_UNIT_ADAPTER_TYPE_SSM:
		al_mod_sprintf(module_params.name, "SSM(%d)-Unit-Adapter", params->index);
		module_params.primary_module = AL_ERR_EVENTS_MODULE_SSM;
		break;
	case AL_UNIT_ADAPTER_TYPE_SATA:
		al_mod_sprintf(module_params.name, "SATA(%d)-Unit-Adapter", params->index);
		module_params.primary_module = AL_ERR_EVENTS_MODULE_SATA;
		break;
	default:
		al_mod_assert(0);
	}

	module_params.secondary_module = AL_ERR_EVENTS_SEC_MODULE_UNIT_ADAPTER;
	module_params.primary_index = params->index;
	module_params.secondary_index = 0;
	module_params.collect_mode = params->collect_mode;
	for (i = 0; i < AL_ERR_EVENTS_IC_MAX; i++)
		module_params.ic_regs_bases[i] = NULL;
	module_params.ic_size = 0;
	module_params.fields_size = AL_ERR_EVENTS_UNIT_ADAPTER_MAX_ERRORS;
	module_params.fields = data->fields;
	module_params.fields_template = al_mod_err_events_ua_fields;
	module_params.private_data = data;
	module_params.collect = err_events_unit_adapter_collect;
	module_params.print = al_mod_err_events_unit_adapter_print;
	module_params.clear = al_mod_err_events_unit_adapter_clear;
	module_params.test = NULL;

	al_mod_err_events_common_module_init(handle, &data->module, &module_params);

	if (params->type != AL_UNIT_ADAPTER_TYPE_SATA)
		for (i = AL_ERR_EVENTS_UNIT_ADAPTER_SATA_PORT_0_TX_RAM_PARITY;
		     i <= AL_ERR_EVENTS_UNIT_ADAPTER_SATA_PORT_3_FB_HIGH_RAM_PARITY; i++)
			data->module.fields[i].valid = AL_FALSE;

	al_mod_err_events_common_module_post_init(&data->module);

	return 0;
}

int al_mod_err_events_ua_ints_enable(struct al_mod_err_events_unit_adapter_data *data)
{
	al_mod_assert(data);

	if (!data->module.enabled)
		return -EINVAL;

	al_mod_unit_adapter_int_enable(&data->unit_adapter, AL_TRUE);

	data->int_fields.wr_err = AL_TRUE;
	data->int_fields.rd_err = AL_TRUE;
	data->int_fields.app_parity_error = AL_TRUE;
	al_mod_unit_adapter_int_cause_mask_set_ex(&data->unit_adapter, &data->int_fields);

	return 0;
}

void al_mod_err_events_ua_ints_disable(struct al_mod_err_events_unit_adapter_data *data)
{
	al_mod_assert(data);

	if (!data->module.enabled)
		return;

	data->int_fields.wr_err = AL_FALSE;
	data->int_fields.rd_err = AL_FALSE;
	data->int_fields.app_parity_error = AL_FALSE;
	al_mod_unit_adapter_int_cause_mask_set_ex(&data->unit_adapter, &data->int_fields);

	/*
	 * Not disabling INTX here as it may affect other users (e.g. Ethernet
	 * driver using INTX mode)
	 */
}

