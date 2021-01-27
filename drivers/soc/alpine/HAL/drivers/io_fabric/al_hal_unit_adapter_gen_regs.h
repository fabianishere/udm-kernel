/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

/**
 *  @{
 * @file   al_hal_unit_adapter_regs.h
 *
 * @brief Unit_Adapter registers
 *
 */

#ifndef __AL_HAL_UNIT_ADAPTER_REGS_H__
#define __AL_HAL_UNIT_ADAPTER_REGS_H__

#include "al_hal_plat_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Unit Registers
 */

struct al_unit_adapter_pci_e_gen {
	/* [0x0] Device Id and Vendor Register */
	uint32_t device_id_vendor_id;
	/* [0x4] Command and Status Register */
	uint32_t pci_command_and_status;
	/* [0x8] Device Id and Vendor Id Register */
	uint32_t revision_and_class;
	/* [0xc] Device Id and Vendor Id Register */
	uint32_t pci_register_c;
	/* [0x10] PF BAR 0 Register */
	uint32_t pf_bar_0;
	/*
	 * [0x14] PF BAR 0 High Register. This register is RO 0 if associate BAR_Type = 0
	 * BAR high Base.
	 */
	uint32_t pf_bar_0_high;
	/* [0x18] PF BAR 2 Register */
	uint32_t pf_bar_2;
	/*
	 * [0x1c] PF BAR 2 High Register. This register is RO 0 if associate BAR_Type = 0
	 * BAR high Base.
	 */
	uint32_t pf_bar_2_high;
	/*
	 * [0x20] PF BAR 4 Register
	 */
	uint32_t pf_bar_4;
	/*
	 * [0x24] PF BAR 4 High Register.
	 * IN SATA Adapaters all the BARS are disable, except of this BAR, in case of SATA, the
	 * fields of this registers are:
	 * [0] -- 1'b0 (memory space)
	 * [2:1]  -- 2'b00 (32 bits cell)
	 * [3] -- 1'b0 not Prefetchable.
	 * [31:4] -- base address + size information
	 * for NON-SATA this register is RW, PF_BAR_4_High
	 */
	uint32_t pf_bar_4_high;
	/*
	 * [0x28] Card Bus Pointer register
	 * Unused
	 */
	uint32_t cardbus_cis_pointer;
	/* [0x2c] Sub System Id Register */
	uint32_t subsystem_id;
	uint32_t rsrvd_0;
	/* [0x34] CAP Pointer Register */
	uint32_t cap_pointer;
	uint32_t rsrvd_1;
	/* [0x3c] Interrupt Line PIN register */
	uint32_t interrupt_line_pin;
};

struct al_unit_adapter_pci_e_cap {
	/* [0x0] PCI Express cap Register */
	uint32_t pci_e_cap;
	/* [0x4] PCIe cap register */
	uint32_t pci_e_device_cap;
	/* [0x8] PCI Express Capability Register */
	uint32_t pci_e_control_and_status;
	uint32_t rsrvd[5];
};

struct al_unit_adapter_pf_msi_x_cap {
	/* [0x0] MSI-X Register 0 */
	uint32_t msi_x_reg_0;
	/* [0x4] MSI-X Table Offset Register */
	uint32_t msi_x_table_offset;
	/* [0x8] MSI-X PBA Offset Register */
	uint32_t msi_x_pba_offset;
};

struct al_unit_adapter_sub_master_cfg_ctrl {
	/*
	 * [0x0] AXI Sub Master Configuration
	 * Determines the specific parameters to override the unit AXI master attributes. Dedicated
	 * register per sub master.
	 */
	uint32_t configuration;
	/*
	 * [0x4] AXI Sub Master Configuration 2
	 */
	uint32_t configuration_2;
	/*
	 * [0x8] AXI Sub Master remap Register
	 * This register determines whether and how to remap the 32-bit Intermediate Physical
	 * Address (aka, IPA address typically-seen by the OS or guest OS) to a new window within
	 * the 64-bit address space per sub-master.
	 */
	uint32_t remap;
	/*
	 * [0xc] AXI Sub Master Translated Base register determines the Translated Address window.
	 * Dedicated register per sub-master.
	 */
	uint32_t translated_base;
	/*
	 * [0x10] AXI Translated Base High register determines the high address of the Translated
	 * Address window.
	 * Dedicated register per sub master
	 * Determines bit [63:32] of new window
	 */
	uint32_t translated_high_base;
	uint32_t rsrvd[3];
};

struct al_unit_adapter_gen_ctrl_reg_0 {
	/*
	 * [0x0] Generic Control Register
	 * Generic Control field
	 * Bit[0] -- Disable clock gating or not.
	 * Bit[1] -- Reflect OR between all the interrupts coming from the functions in the
	 * Interrupt Status bit.
	 * Bit[2] -- Parity protection disable for units without GDMA.
	 * Bit[5:3]: Mask the VF FLR to the GDMA engine due to VF_FLR.
	 * Bit[6]: When set, all transactions through the PCI Conf BAR and Mem BAR get timeout.
	 * Bit[11:8] : Mask the pf_flr bit function.
	 * Bit[12] : Mask the adapter reset when the pf_flr.
	 * Bt[23:16] - Soft_reset_app.
	 * Bt[31:24] - Enable_FLR.
	 * Bit[25] in the CRC engine sets this bit to 1 and resets the FIFO between the CRYPTO and
	 * CRC engines.
	 */
	uint32_t data;
};

struct al_unit_adapter_gen_ctrl_reg_1 {
	/*
	 * [0x0] Generic Control Register
	 * Generic Control field
	 * Bit[15:0] -- MSI_X_En_overide.
	 * Enable legacy interrupt in MSI-X enable. Bit per function & group
	 */
	uint32_t data;
};

struct al_unit_adapter_gen_ctrl_reg_2 {
	/*
	 * [0x0] Generic Control Register
	 * Generic Control field
	 */
	uint32_t data;
};

struct al_unit_adapter_gen_ctrl_reg_3 {
	/*
	 * [0x0] Generic Control Register
	 * Generic Control field
	 */
	uint32_t data;
};

struct al_unit_adapter_generic_status_registers {
	/*
	 * [0x0] Generic Status Register
	 * Generic Status Field
	 */
	uint32_t generic_status;
};

struct al_unit_adapter_metal_fix_registers {
	/*
	 * [0x0] Metal fix Register
	 * Metal Fix
	 */
	uint32_t metal_fix;
};

struct al_unit_adapter_gen_ctrl_reg_4 {
	/*
	 * [0x0] Generic Control Register
	 * Generic Control Field
	 */
	uint32_t data;
};

struct al_unit_adapter_gen_ctrl_reg_5 {
	/*
	 * [0x0] Generic Control Register
	 * Generic Control Field
	 */
	uint32_t data;
};

struct al_unit_adapter_gen_ctrl_reg_6 {
	/*
	 * [0x0] Generic Control Register
	 * Generic Control Field
	 */
	uint32_t data;
};

struct al_unit_adapter_gen_ctrl_reg_7 {
	/*
	 * [0x0] Generic Control Regisiter
	 * Generic Control Field
	 */
	uint32_t data;
};

struct al_unit_adapter_gen_ctrl_reg_8 {
	/*
	 * [0x0] Generic Control Register
	 * Generic Control Field
	 */
	uint32_t data;
};

struct al_unit_adapter_gen_ctrl_reg_9 {
	/*
	 * [0x0] Generic Control Register
	 * Generic Control Field
	 */
	uint32_t data;
};

struct al_unit_adapter_gen_ctrl_reg_10 {
	/*
	 * [0x0] Generic Control Register
	 * Generic Control field
	 */
	uint32_t data;
};

struct al_unit_adapter_gen_ctrl_reg_11 {
	/* [0x0] Generic Control Register */
	uint32_t data;
};

struct al_unit_adapter_gen_ctrl_reg_12 {
	/*
	 * [0x0] Generic Control Regsiter
	 * generic Control field.
	 */
	uint32_t data;
};

struct al_unit_adapter_gen_ctrl_reg_13 {
	/*
	 * [0x0] Generic Control Regsiter
	 * generic Control field.
	 */
	uint32_t data;
};

struct al_unit_adapter_gen_ctrl_reg_14 {
	/*
	 * [0x0] Generic Control Regsiter
	 * generic Control field.
	 */
	uint32_t data;
};

struct al_unit_adapter_gen_ctrl_reg_15 {
	/*
	 * [0x0] Generic Control Regsiter
	 * generic Control field.
	 */
	uint32_t data;
};

struct al_unit_adapter_gen_ctrl_reg_16 {
	/*
	 * [0x0] Generic Control Regsiter
	 * generic Control field.
	 */
	uint32_t data;
};

struct al_unit_adapter_gen_ctrl_reg_17 {
	/*
	 * [0x0] Generic Control Regsiter
	 * generic Control field.
	 */
	uint32_t data;
};

struct al_unit_adapter_gen_ctrl_reg_18 {
	/*
	 * [0x0] Generic Control Regsiter
	 * generic Control field.
	 */
	uint32_t data;
};

struct al_unit_adapter_gen_ctrl_reg_19 {
	/*
	 * [0x0] Generic Control Regsiter
	 * generic Control field.
	 */
	uint32_t data;
};

struct al_unit_adapter_sriov_cap {
	/* [0x0] SRIOV ID and_ ext Offset Register */
	uint32_t sriov_id_and_next_offset;
	/* [0x4] SRIOV Capabilities Register */
	uint32_t sriov_caps;
	/* [0x8] SRIOV Status and Control Register */
	uint32_t sriov_status_and_control;
	/* [0xc] SRIOV Total and Initial VFs Register */
	uint32_t sriov_total_and_initial_vfs;
	/* [0x10] SRIOV Register 10 */
	uint32_t sriov_register_10;
	/* [0x14] SRIOV Register 14 */
	uint32_t sriov_register_14;
	/* [0x18] SRIOV VF Device ID Register */
	uint32_t sriov_vf_device_id;
	/*
	 * [0x1c] SRIOV Supported Pages Sizes Register
	 * SRIOV Supported Page Sizes
	 */
	uint32_t sriov_supported_page_sizes;
	/*
	 * [0x20] SRIOV System Page Register
	 * SRIOV System page sizes.
	 * If this value is larger than the actual VF Mem BAR, the size of the BAR will be the page
	 * size.
	 */
	uint32_t sriov_system_page_size;
	/* [0x24] VF BAR 0 Register */
	uint32_t vf_bar_0;
	/*
	 * [0x28] VF BAR 0 High
	 * BAR Base
	 */
	uint32_t vf_bar_0_high;
	/* [0x2c] VF BAR 2 Register */
	uint32_t vf_bar_2;
	/*
	 * [0x30] VF BAR 2 High
	 * BAR Base
	 */
	uint32_t vf_bar_2_high;
	/* [0x34] VF BAR 4 Register */
	uint32_t vf_bar_4;
	/*
	 * [0x38] VF BAR 4 high
	 * BAR Base
	 */
	uint32_t vf_bar_4_high;
	uint32_t rsrvd;
};

struct al_unit_adapter_vf_regs {
	/* [0x0] Device ID and Vendor Register */
	uint32_t device_id_vendor_id_vf;
	/* [0x4] Command and Status Register */
	uint32_t command_and_status_vf;
	/* [0x8] Device ID and Vendor ID Register */
	uint32_t revision_and_class_vf;
	/* [0xc] Device ID and Vendor ID Register */
	uint32_t pci_register_c_vf;
	uint32_t rsrvd_0[9];
	/*
	 * [0x34] CAP Pointer Register
	 * Capability Pointer
	 */
	uint32_t cap_pointer_vf;
	uint32_t rsrvd_1[2];
	/* [0x40] PCI Express Capability Register */
	uint32_t pci_e_cap_vf;
	/* [0x44] PCI Express Capability Register */
	uint32_t pci_e_device_cap_vf;
	/* [0x48] PCI Express Capability Register */
	uint32_t pci_e_control_and_status_vf;
	uint32_t rsrvd_2[17];
	/* [0x90] MSI-X Register 0 */
	uint32_t msi_x_reg_0_vf;
	/* [0x94] MSI-X Table Offset Register */
	uint32_t msi_x_table_offset_vf;
	/* [0x98] MSI-X PBA Offset Register */
	uint32_t msi_x_pba_offset_vf;
	uint32_t rsrvd[985];
};

struct al_unit_adapter_regs {
	/* [0x0] */
	struct al_unit_adapter_pci_e_gen pci_e_gen;
	/* [0x40] */
	struct al_unit_adapter_pci_e_cap pci_e_cap;
	/* [0x60] */
	uint32_t rsrvd_1[12];
	/* [0x90] */
	struct al_unit_adapter_pf_msi_x_cap pf_msi_x_cap;
	/* [0x9c] */
	uint32_t rsrvd_2[29];
	/* [0x110] */
	struct al_unit_adapter_sub_master_cfg_ctrl sub_master_cfg_ctrl[4];
	/* [0x190] */
	uint32_t rsrvd_3[20];
	/* [0x1e0] */
	struct al_unit_adapter_gen_ctrl_reg_0 gen_ctrl_reg_0;
	/* [0x1e4] */
	struct al_unit_adapter_gen_ctrl_reg_1 gen_ctrl_reg_1;
	/* [0x1e8] */
	struct al_unit_adapter_gen_ctrl_reg_2 gen_ctrl_reg_2;
	/* [0x1ec] */
	struct al_unit_adapter_gen_ctrl_reg_3 gen_ctrl_reg_3;
	/* [0x1f0] */
	struct al_unit_adapter_generic_status_registers generic_status_registers;
	/* [0x1f4] */
	uint32_t rsrvd_4[3];
	/* [0x200] */
	struct al_unit_adapter_metal_fix_registers metal_fix_registers;
	/* [0x204] */
	struct al_unit_adapter_gen_ctrl_reg_4 gen_ctrl_reg_4;
	/* [0x208] */
	struct al_unit_adapter_gen_ctrl_reg_5 gen_ctrl_reg_5;
	/* [0x20c] */
	struct al_unit_adapter_gen_ctrl_reg_6 gen_ctrl_reg_6;
	/* [0x210] */
	struct al_unit_adapter_gen_ctrl_reg_7 gen_ctrl_reg_7;
	/* [0x214] */
	struct al_unit_adapter_gen_ctrl_reg_8 gen_ctrl_reg_8;
	/* [0x218] */
	struct al_unit_adapter_gen_ctrl_reg_9 gen_ctrl_reg_9;
	/* [0x21c] */
	struct al_unit_adapter_gen_ctrl_reg_10 gen_ctrl_reg_10;
	/* [0x220] */
	struct al_unit_adapter_gen_ctrl_reg_11 gen_ctrl_reg_11;
	/* [0x224] */
	struct al_unit_adapter_gen_ctrl_reg_12 gen_ctrl_reg_12;
	/* [0x228] */
	struct al_unit_adapter_gen_ctrl_reg_13 gen_ctrl_reg_13;
	/* [0x22c] */
	struct al_unit_adapter_gen_ctrl_reg_14 gen_ctrl_reg_14;
	/* [0x230] */
	struct al_unit_adapter_gen_ctrl_reg_15 gen_ctrl_reg_15;
	/* [0x234] */
	struct al_unit_adapter_gen_ctrl_reg_16 gen_ctrl_reg_16;
	/* [0x238] */
	struct al_unit_adapter_gen_ctrl_reg_17 gen_ctrl_reg_17;
	/* [0x23c] */
	struct al_unit_adapter_gen_ctrl_reg_18 gen_ctrl_reg_18;
	/* [0x240] */
	struct al_unit_adapter_gen_ctrl_reg_19 gen_ctrl_reg_19;
	/* [0x244] */
	uint32_t rsrvd_5[47];
	/* [0x300] */
	struct al_unit_adapter_sriov_cap sriov_cap;
	/* [0x340] */
	uint32_t rsrvd_6[816];
	/* [0x1000] */
	struct al_unit_adapter_vf_regs vf_regs[3];
};


/*
 * Registers Fields
 */

/**** Device_ID_Vendor_ID register ****/
/* Vendor ID */
#define UNIT_ADAPTER__DEVICE_ID_VENDOR_ID_VENDOR_ID_MASK 0x0000FFFF
#define UNIT_ADAPTER__DEVICE_ID_VENDOR_ID_VENDOR_ID_SHIFT 0
/* Device ID */
#define UNIT_ADAPTER__DEVICE_ID_VENDOR_ID_DEVICE_ID_MASK 0xFFFF0000
#define UNIT_ADAPTER__DEVICE_ID_VENDOR_ID_DEVICE_ID_SHIFT 16

/**** PCI_Command_and_Status register ****/
/* I/O space enable - Unused */
#define UNIT_ADAPTER__PCI_COMMAND_AND_STATUS_IOSE (1 << 0)
/* Memory space enable */
#define UNIT_ADAPTER__PCI_COMMAND_AND_STATUS_MSE (1 << 1)
/* Bus master enable */
#define UNIT_ADAPTER__PCI_COMMAND_AND_STATUS_BME (1 << 2)
/* Special cycle enable - Unused */
#define UNIT_ADAPTER__PCI_COMMAND_AND_STATUS_SCE (1 << 3)
/* Master write and invalidate - Unused */
#define UNIT_ADAPTER__PCI_COMMAND_AND_STATUS_MWIE (1 << 4)
/* Unused */
#define UNIT_ADAPTER__PCI_COMMAND_AND_STATUS_VGA (1 << 5)
/* Parity error response */
#define UNIT_ADAPTER__PCI_COMMAND_AND_STATUS_PER (1 << 6)
/* Unused */
#define UNIT_ADAPTER__PCI_COMMAND_AND_STATUS_RESERVED_7 (1 << 7)
/* SER enable */
#define UNIT_ADAPTER__PCI_COMMAND_AND_STATUS_SEE (1 << 8)
/* Fast Back-to-Back Enable transactions - Unused */
#define UNIT_ADAPTER__PCI_COMMAND_AND_STATUS_FBE (1 << 9)
/* Interrupt Disable */
#define UNIT_ADAPTER__PCI_COMMAND_AND_STATUS_INTERRUPT_DISABLE (1 << 10)
/* Unused */
#define UNIT_ADAPTER__PCI_COMMAND_AND_STATUS_RESERVED_15_11_MASK 0x0000F800
#define UNIT_ADAPTER__PCI_COMMAND_AND_STATUS_RESERVED_15_11_SHIFT 11
/* Unused */
#define UNIT_ADAPTER__PCI_COMMAND_AND_STATUS_STATUS_RESERVED_18_16_MASK 0x00070000
#define UNIT_ADAPTER__PCI_COMMAND_AND_STATUS_STATUS_RESERVED_18_16_SHIFT 16
/*
 * Interrupt Status. 0 by default. If enabled by chicken bit, reflects the unit INT signal before it
 * is masked by the Interrupt_disable bit
 */
#define UNIT_ADAPTER__PCI_COMMAND_AND_STATUS_STATUS_IS (1 << 19)
/* Capability List */
#define UNIT_ADAPTER__PCI_COMMAND_AND_STATUS_STATUS_CL (1 << 20)
/* 66 MHz Capable */
#define UNIT_ADAPTER__PCI_COMMAND_AND_STATUS_STATUS_C66 (1 << 21)
/* Reserved */
#define UNIT_ADAPTER__PCI_COMMAND_AND_STATUS_STATUS_RESERVED_22 (1 << 22)
/* Fast Back-2-Back Capability */
#define UNIT_ADAPTER__PCI_COMMAND_AND_STATUS_STATUS_FBC (1 << 23)
/* Master data parity error */
#define UNIT_ADAPTER__PCI_COMMAND_AND_STATUS_STATUS_DPD (1 << 24)
/* Devsel Timing - Unused */
#define UNIT_ADAPTER__PCI_COMMAND_AND_STATUS_STATUS_DEVSEL_MASK 0x06000000
#define UNIT_ADAPTER__PCI_COMMAND_AND_STATUS_STATUS_DEVSEL_SHIFT 25
/* Signaled target abort - Unused */
#define UNIT_ADAPTER__PCI_COMMAND_AND_STATUS_STATUS_STA (1 << 27)
/* Received target abort - Unused */
#define UNIT_ADAPTER__PCI_COMMAND_AND_STATUS_STATUS_RTA (1 << 28)
/* Received master abort - Unused */
#define UNIT_ADAPTER__PCI_COMMAND_AND_STATUS_STATUS_RMA (1 << 29)
/* Signaled System error */
#define UNIT_ADAPTER__PCI_COMMAND_AND_STATUS_STATUS_SSE (1 << 30)
/* Detected Parity Error */
#define UNIT_ADAPTER__PCI_COMMAND_AND_STATUS_STATUS_DPE (1 << 31)

/**** Revision_and_Class register ****/
/* Revision ID */
#define UNIT_ADAPTER__REVISION_AND_CLASS_REV_ID_MASK 0x000000FF
#define UNIT_ADAPTER__REVISION_AND_CLASS_REV_ID_SHIFT 0
/* Specific register level program interface */
#define UNIT_ADAPTER__REVISION_AND_CLASS_CLASS_PI_MASK 0x0000FF00
#define UNIT_ADAPTER__REVISION_AND_CLASS_CLASS_PI_SHIFT 8
/* Sub class code */
#define UNIT_ADAPTER__REVISION_AND_CLASS_CLASS_SCC_MASK 0x00FF0000
#define UNIT_ADAPTER__REVISION_AND_CLASS_CLASS_SCC_SHIFT 16
/* Base class code */
#define UNIT_ADAPTER__REVISION_AND_CLASS_CLASS_BCC_MASK 0xFF000000
#define UNIT_ADAPTER__REVISION_AND_CLASS_CLASS_BCC_SHIFT 24

/**** PCI_Register_C register ****/
/* System cache line size */
#define UNIT_ADAPTER__PCI_REGISTER_C_CACH_LINE_SIZE_MASK 0x000000FF
#define UNIT_ADAPTER__PCI_REGISTER_C_CACH_LINE_SIZE_SHIFT 0
/* Latency timer - Unused */
#define UNIT_ADAPTER__PCI_REGISTER_C_LATENCY_TIMER_UNUSED_MASK 0x0000FF00
#define UNIT_ADAPTER__PCI_REGISTER_C_LATENCY_TIMER_UNUSED_SHIFT 8
/* Header type - Indicates a single-function device */
#define UNIT_ADAPTER__PCI_REGISTER_C_HEADER_TYPE_MASK 0x00FF0000
#define UNIT_ADAPTER__PCI_REGISTER_C_HEADER_TYPE_SHIFT 16
/* Built-in self-test - Unused */
#define UNIT_ADAPTER__PCI_REGISTER_C_BIST_MASK 0xFF000000
#define UNIT_ADAPTER__PCI_REGISTER_C_BIST_SHIFT 24

/**** PF_BAR_0 register ****/
/* 0 - Memory space */
#define UNIT_ADAPTER__PF_BAR_0_MEMORY_SPACE_INDICATOR (1 << 0)
/*
 * BAR Type
 * 00 - 32b address
 * 01 - Reserved
 * 10 - 64b address
 * 11 - Reserved
 */
#define UNIT_ADAPTER__PF_BAR_0_BAR_TYPE_MASK 0x00000006
#define UNIT_ADAPTER__PF_BAR_0_BAR_TYPE_SHIFT 1
/* Prefetchable, no side effects on reads */
#define UNIT_ADAPTER__PF_BAR_0_PREFETCHABLE (1 << 3)
/* BAR Base */
#define UNIT_ADAPTER__PF_BAR_0_BASE_MASK 0xFFFFFFF0
#define UNIT_ADAPTER__PF_BAR_0_BASE_SHIFT 4

/**** PF_BAR_2 register ****/
/* 0 - Memory Space */
#define UNIT_ADAPTER__PF_BAR_2_MEMORY_SPACE_INDICATOR (1 << 0)
/*
 * BAR Type
 * 00 - 32b address
 * 01 - Reserved
 * 10 - 64b address
 * 11 - Reserved
 */
#define UNIT_ADAPTER__PF_BAR_2_BAR_TYPE_MASK 0x00000006
#define UNIT_ADAPTER__PF_BAR_2_BAR_TYPE_SHIFT 1
/* Prefetchable, no side effects on reads */
#define UNIT_ADAPTER__PF_BAR_2_PREFETCHABLE (1 << 3)
/* BAR Base */
#define UNIT_ADAPTER__PF_BAR_2_BASE_MASK 0xFFFFFFF0
#define UNIT_ADAPTER__PF_BAR_2_BASE_SHIFT 4

/**** PF_BAR_4 register ****/
/* 0 - Memory Space */
#define UNIT_ADAPTER__PF_BAR_4_MEMORY_SPACE_INDICATOR (1 << 0)
/*
 * BAR Type
 * 00 - 32b address
 * 01 - Reserved
 * 10 - 64b address
 * 11 - Reserved
 */
#define UNIT_ADAPTER__PF_BAR_4_BAR_TYPE_MASK 0x00000006
#define UNIT_ADAPTER__PF_BAR_4_BAR_TYPE_SHIFT 1
/* Prefetchable, no side effects on reads */
#define UNIT_ADAPTER__PF_BAR_4_PREFETCHABLE (1 << 3)
/* BAR Base */
#define UNIT_ADAPTER__PF_BAR_4_BASE_MASK 0xFFFFFFF0
#define UNIT_ADAPTER__PF_BAR_4_BASE_SHIFT 4

/**** CAP_Pointer register ****/
/* Capability Pointer */
#define UNIT_ADAPTER__CAP_POINTER_CAP_PTR_MASK 0x000000FF
#define UNIT_ADAPTER__CAP_POINTER_CAP_PTR_SHIFT 0
/* Capability Pointer */
#define UNIT_ADAPTER__CAP_POINTER_RESERVED_MASK 0xFFFFFF00
#define UNIT_ADAPTER__CAP_POINTER_RESERVED_SHIFT 8

/**** Interrupt_Line_Pin register ****/
/* Interrupt Line */
#define UNIT_ADAPTER__INTERRUPT_LINE_PIN_INTERRUPT_LINE_MASK 0x000000FF
#define UNIT_ADAPTER__INTERRUPT_LINE_PIN_INTERRUPT_LINE_SHIFT 0
/* Interrupt Pin */
#define UNIT_ADAPTER__INTERRUPT_LINE_PIN_INTERRUPT_PIN_MASK 0x0000FF00
#define UNIT_ADAPTER__INTERRUPT_LINE_PIN_INTERRUPT_PIN_SHIFT 8
/* Interrupt Line Pin */
#define UNIT_ADAPTER__INTERRUPT_LINE_PIN_INT_LINE_PIN_RESERVED_MASK 0xFFFF0000
#define UNIT_ADAPTER__INTERRUPT_LINE_PIN_INT_LINE_PIN_RESERVED_SHIFT 16

/**** PCI_E_Capability register ****/
/* PCIe cap ID */
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_CAP_PCI_E_CAP_ID_MASK 0x000000FF
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_CAP_PCI_E_CAP_ID_SHIFT 0
/* PCIe next cap pointer */
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_CAP_PCI_E_NEXT_CAP_POINTER_MASK 0x0000FF00
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_CAP_PCI_E_NEXT_CAP_POINTER_SHIFT 8
/* PCIe compatibility version */
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_CAP_PCI_E_COMPATIBILITY_VERSION_MASK 0x000F0000
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_CAP_PCI_E_COMPATIBILITY_VERSION_SHIFT 16
/* Represents root complex integrated PCI Express Endpoint */
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_CAP_PCI_E_DEVICE_TYPE_MASK 0x00F00000
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_CAP_PCI_E_DEVICE_TYPE_SHIFT 20
/* PCI Express slot implemented */
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_CAP_PCI_E_SLOT_IMP (1 << 24)
/* Interrupt message generated in association with status bit of this cap */
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_CAP_PCI_E_IMN_MASK 0x3E000000
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_CAP_PCI_E_IMN_SHIFT 25
/* Undefined */
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_CAP_UNDEFINED (1 << 30)
/* Reserved */
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_CAP_RESERVED (1 << 31)

/**** PCI_E_Device_Capability register ****/
/* Max Payload size = 256B */
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_DEVICE_CAP_PCI_E_MPSS_MASK 0x00000007
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_DEVICE_CAP_PCI_E_MPSS_SHIFT 0
/* Unused */
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_DEVICE_CAP_PCI_E_PFS_MASK 0x00000018
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_DEVICE_CAP_PCI_E_PFS_SHIFT 3
/* Unused */
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_DEVICE_CAP_PCI_E_ETFS (1 << 5)
/* Unused */
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_DEVICE_CAP_PCI_E_ENDPONT_L0_AL_MASK 0x000001C0
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_DEVICE_CAP_PCI_E_ENDPONT_L0_AL_SHIFT 6
/* Unused */
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_DEVICE_CAP_PCI_E_ENDPONT_L1_AL_MASK 0x00000E00
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_DEVICE_CAP_PCI_E_ENDPONT_L1_AL_SHIFT 9
/* Undefined */
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_DEVICE_CAP_UNDEFINED_MASK 0x00007000
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_DEVICE_CAP_UNDEFINED_SHIFT 12
/* Unused */
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_DEVICE_CAP_PCI_E_RBER (1 << 15)
/* Reserved */
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_DEVICE_CAP_RESERVED_17_16_MASK 0x00030000
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_DEVICE_CAP_RESERVED_17_16_SHIFT 16
/* Unused */
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_DEVICE_CAP_PCI_E_CSPLV_MASK 0x03FC0000
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_DEVICE_CAP_PCI_E_CSPLV_SHIFT 18
/* Unused */
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_DEVICE_CAP_PCI_E_CSPLS_MASK 0x0C000000
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_DEVICE_CAP_PCI_E_CSPLS_SHIFT 26
/* Function level reset support */
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_DEVICE_CAP_FLR_CAP (1 << 28)
/* Reserved */
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_DEVICE_CAP_RESERVED_31_29_MASK 0xE0000000
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_DEVICE_CAP_RESERVED_31_29_SHIFT 29

/**** PCI_E_Control_and_Status register ****/
/* Unused */
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_CONTROL_AND_STATUS_PCI_E_CERE (1 << 0)
/* Unused */
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_CONTROL_AND_STATUS_PCI_E_NFERE (1 << 1)
/* Unused */
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_CONTROL_AND_STATUS_PCI_E_FERE (1 << 2)
/* Unused */
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_CONTROL_AND_STATUS_PCI_E_URRE (1 << 3)
/* Unused */
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_CONTROL_AND_STATUS_PCI_E_ENABLED_RELAXED_ORDERING (1 << 4)
/* Unused */
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_CONTROL_AND_STATUS_PCI_E_MPS_MASK 0x000000E0
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_CONTROL_AND_STATUS_PCI_E_MPS_SHIFT 5
/* Unused */
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_CONTROL_AND_STATUS_MSI_E_ETFE (1 << 8)
/* Unused */
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_CONTROL_AND_STATUS_PCI_E_PFE (1 << 9)
/* Unused */
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_CONTROL_AND_STATUS_PCI_E_AUX_PWR_PM_EN (1 << 10)
/*
 * Enable No snoop. When this bit is set to 1, it is recommended that the coherency subsystem not
 * perform snoop, as it may be overwritten down the path by other system configurations.
 */
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_CONTROL_AND_STATUS_PCI_E_EN_NO_SNOOP (1 << 11)
/* Unused */
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_CONTROL_AND_STATUS_PCI_E_MRRS_MASK 0x00007000
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_CONTROL_AND_STATUS_PCI_E_MRRS_SHIFT 12
/* Function level reset */
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_CONTROL_AND_STATUS_PF_FLR (1 << 15)
/* Undefined */
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_CONTROL_AND_STATUS_PCI_E_CED (1 << 16)
/* Unused */
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_CONTROL_AND_STATUS_PCI_E_NFED (1 << 17)
/* Reserved */
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_CONTROL_AND_STATUS_PCI_E_FED (1 << 18)
/* Unused */
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_CONTROL_AND_STATUS_PCI_E_UNSUPPORTED_REQUEST (1 << 19)
/* Unused */
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_CONTROL_AND_STATUS_PCI_E_AUX_PWR_DETECTED (1 << 20)
/* Unused */
#define UNIT_ADAPTER_PCI_E_CAP_PCI_E_CONTROL_AND_STATUS_PCI_E_TP (1 << 21)

/**** USB_Specific register ****/
/* USB_Serial_bus_rls_num */
#define UNIT_ADAPTER__USB_SPECIFIC_USB_SERIAL_BUS_RLS_NUM_MASK 0x000000FF
#define UNIT_ADAPTER__USB_SPECIFIC_USB_SERIAL_BUS_RLS_NUM_SHIFT 0
/* USB_Frm_len_adj */
#define UNIT_ADAPTER__USB_SPECIFIC_USB_FRM_LEN_ADJ_MASK 0x0000FF00
#define UNIT_ADAPTER__USB_SPECIFIC_USB_FRM_LEN_ADJ_SHIFT 8
/* Reserved */
#define UNIT_ADAPTER__USB_SPECIFIC_RESERVED_MASK 0xFFFF0000
#define UNIT_ADAPTER__USB_SPECIFIC_RESERVED_SHIFT 16

/**** PCI_Power_Management_Reg0 register ****/
/* PM Capability ID */
#define UNIT_ADAPTER__PCI_POWER_MANAGEMENT_REG0_PM_CAP_ID_MASK 0x000000FF
#define UNIT_ADAPTER__PCI_POWER_MANAGEMENT_REG0_PM_CAP_ID_SHIFT 0
/* PM Next Capability */
#define UNIT_ADAPTER__PCI_POWER_MANAGEMENT_REG0_PM_NEXT_CAP_MASK 0x0000FF00
#define UNIT_ADAPTER__PCI_POWER_MANAGEMENT_REG0_PM_NEXT_CAP_SHIFT 8
/* Version */
#define UNIT_ADAPTER__PCI_POWER_MANAGEMENT_REG0_VERSION_MASK 0x00070000
#define UNIT_ADAPTER__PCI_POWER_MANAGEMENT_REG0_VERSION_SHIFT 16
/* PME CLOCK unused */
#define UNIT_ADAPTER__PCI_POWER_MANAGEMENT_REG0_PME_CLOCK_UNUSED (1 << 19)
/* Reserved */
#define UNIT_ADAPTER__PCI_POWER_MANAGEMENT_REG0_PM_REG0_RESERVED (1 << 20)
/* DSI */
#define UNIT_ADAPTER__PCI_POWER_MANAGEMENT_REG0_DEVICE_SPECIFIC_INIT (1 << 21)
/* AUX Current */
#define UNIT_ADAPTER__PCI_POWER_MANAGEMENT_REG0_AUXCURRENT_MASK 0x01C00000
#define UNIT_ADAPTER__PCI_POWER_MANAGEMENT_REG0_AUXCURRENT_SHIFT 22
/* D1 Support */
#define UNIT_ADAPTER__PCI_POWER_MANAGEMENT_REG0_D1_SUPPORT (1 << 25)
/* D2 Support */
#define UNIT_ADAPTER__PCI_POWER_MANAGEMENT_REG0_D2_SUPPORT (1 << 26)
/* PME support. The value is changed per unit. */
#define UNIT_ADAPTER__PCI_POWER_MANAGEMENT_REG0_PME_SUPPORT_MASK 0xF8000000
#define UNIT_ADAPTER__PCI_POWER_MANAGEMENT_REG0_PME_SUPPORT_SHIFT 27

/**** PCI_Power_Management_Reg1 register ****/
/*
 * Power State. This field is used to set and report the power state of a functions as follows:
 * 0 = D0
 * 1 = D1
 * 2 = D2
 * 3 = D3
 * When in the D3HOT state, the HBA configuration space is available, but the register memory spaces
 * are not.
 */
#define UNIT_ADAPTER__PCI_POWER_MANAGEMENT_REG1_POWERSTATE_MASK 0x00000003
#define UNIT_ADAPTER__PCI_POWER_MANAGEMENT_REG1_POWERSTATE_SHIFT 0
/* Reserved */
#define UNIT_ADAPTER__PCI_POWER_MANAGEMENT_REG1_PM_REG1_RESERVED_2 (1 << 2)
/* Reserved */
#define UNIT_ADAPTER__PCI_POWER_MANAGEMENT_REG1_PM_NO_SOFT_RESET (1 << 3)
/* Reserved */
#define UNIT_ADAPTER__PCI_POWER_MANAGEMENT_REG1_PM_REG1_RESERVED_1_MASK 0x000000F0
#define UNIT_ADAPTER__PCI_POWER_MANAGEMENT_REG1_PM_REG1_RESERVED_1_SHIFT 4
/* PME En */
#define UNIT_ADAPTER__PCI_POWER_MANAGEMENT_REG1_PME_EN (1 << 8)
/* Data register is not supported. */
#define UNIT_ADAPTER__PCI_POWER_MANAGEMENT_REG1_DATA_SELECT_MASK 0x00001E00
#define UNIT_ADAPTER__PCI_POWER_MANAGEMENT_REG1_DATA_SELECT_SHIFT 9
/* Data register is not supported. */
#define UNIT_ADAPTER__PCI_POWER_MANAGEMENT_REG1_DATA_SCALE_MASK 0x00006000
#define UNIT_ADAPTER__PCI_POWER_MANAGEMENT_REG1_DATA_SCALE_SHIFT 13
/* PME Status */
#define UNIT_ADAPTER__PCI_POWER_MANAGEMENT_REG1_PME_STATUS (1 << 15)
/* Bridge Support Extensions - Unused */
#define UNIT_ADAPTER__PCI_POWER_MANAGEMENT_REG1_BRIDGE_SUPPORT_EXTENTIONS_MASK 0x00FF0000
#define UNIT_ADAPTER__PCI_POWER_MANAGEMENT_REG1_BRIDGE_SUPPORT_EXTENTIONS_SHIFT 16
/* PM Data */
#define UNIT_ADAPTER__PCI_POWER_MANAGEMENT_REG1_PM_DATA_MASK 0xFF000000
#define UNIT_ADAPTER__PCI_POWER_MANAGEMENT_REG1_PM_DATA_SHIFT 24

/**** MSI_X_Reg_0 register ****/
/* MSI-X Capability ID */
#define UNIT_ADAPTER_PF_MSI_X_CAP_MSI_X_REG_0_MSI_X_CAP_ID_MASK 0x000000FF
#define UNIT_ADAPTER_PF_MSI_X_CAP_MSI_X_REG_0_MSI_X_CAP_ID_SHIFT 0
/* MSI-X Next Pointer */
#define UNIT_ADAPTER_PF_MSI_X_CAP_MSI_X_REG_0_MSI_X_NEXT_POINTER_MASK 0x0000FF00
#define UNIT_ADAPTER_PF_MSI_X_CAP_MSI_X_REG_0_MSI_X_NEXT_POINTER_SHIFT 8
/*
 * Table Size.
 * Supports up to 64 different interrupt vectors per function.
 */
#define UNIT_ADAPTER_PF_MSI_X_CAP_MSI_X_REG_0_TABLE_SIZE_MASK 0x07FF0000
#define UNIT_ADAPTER_PF_MSI_X_CAP_MSI_X_REG_0_TABLE_SIZE_SHIFT 16
/* Reserved */
#define UNIT_ADAPTER_PF_MSI_X_CAP_MSI_X_REG_0_RESERVED_MASK 0x38000000
#define UNIT_ADAPTER_PF_MSI_X_CAP_MSI_X_REG_0_RESERVED_SHIFT 27
/* Function Mask */
#define UNIT_ADAPTER_PF_MSI_X_CAP_MSI_X_REG_0_FUNCTION_MASK (1 << 30)
/* MSI-X Enable */
#define UNIT_ADAPTER_PF_MSI_X_CAP_MSI_X_REG_0_MSI_X_EN (1 << 31)

/**** MSI_X_Table_Offset register ****/
/*
 * Table Base ID Indicator Register
 * Indicates that it is offset from BAR0.
 */
#define UNIT_ADAPTER_PF_MSI_X_CAP_MSI_X_TABLE_OFFSET_TABLE_BIR_MASK 0x00000007
#define UNIT_ADAPTER_PF_MSI_X_CAP_MSI_X_TABLE_OFFSET_TABLE_BIR_SHIFT 0
/* Table_Offset, 8B aligned */
#define UNIT_ADAPTER_PF_MSI_X_CAP_MSI_X_TABLE_OFFSET_TABLE_OFFSET_MASK 0xFFFFFFF8
#define UNIT_ADAPTER_PF_MSI_X_CAP_MSI_X_TABLE_OFFSET_TABLE_OFFSET_SHIFT 3

/**** MSI_X_PBA_Offset register ****/
/*
 * PBA Base Indicator Register
 * Indicates that it is offset from BAR0.
 */
#define UNIT_ADAPTER_PF_MSI_X_CAP_MSI_X_PBA_OFFSET_PBA_BIR_MASK 0x00000007
#define UNIT_ADAPTER_PF_MSI_X_CAP_MSI_X_PBA_OFFSET_PBA_BIR_SHIFT 0
/* PBA Offset, 8B aligned */
#define UNIT_ADAPTER_PF_MSI_X_CAP_MSI_X_PBA_OFFSET_PBA_OFFSET_MASK 0xFFFFFFF8
#define UNIT_ADAPTER_PF_MSI_X_CAP_MSI_X_PBA_OFFSET_PBA_OFFSET_SHIFT 3

/**** Vendor_Specific_Extended_Capability register ****/
/* VSEC CAP ID */
#define UNIT_ADAPTER__VENDOR_SPECIFIC_EXTENDED_CAP_VSEC_CAP_ID_MASK 0x0000FFFF
#define UNIT_ADAPTER__VENDOR_SPECIFIC_EXTENDED_CAP_VSEC_CAP_ID_SHIFT 0
/* VSEC CAP Version */
#define UNIT_ADAPTER__VENDOR_SPECIFIC_EXTENDED_CAP_VSEC_CAP_VERSION_MASK 0x000F0000
#define UNIT_ADAPTER__VENDOR_SPECIFIC_EXTENDED_CAP_VSEC_CAP_VERSION_SHIFT 16
/* VSEC next CAP Offset */
#define UNIT_ADAPTER__VENDOR_SPECIFIC_EXTENDED_CAP_VSEC_NEXT_CAP_OFFSET_MASK 0xFFF00000
#define UNIT_ADAPTER__VENDOR_SPECIFIC_EXTENDED_CAP_VSEC_NEXT_CAP_OFFSET_SHIFT 20

/**** Vendor_Specific_Header register ****/
/* VSEC  ID */
#define UNIT_ADAPTER__VENDOR_SPECIFIC_HEADER_VSEC_ID_MASK 0x0000FFFF
#define UNIT_ADAPTER__VENDOR_SPECIFIC_HEADER_VSEC_ID_SHIFT 0
/* VSEC Revision */
#define UNIT_ADAPTER__VENDOR_SPECIFIC_HEADER_VSEC_REV_MASK 0x000F0000
#define UNIT_ADAPTER__VENDOR_SPECIFIC_HEADER_VSEC_REV_SHIFT 16
/* VSEC CAP Length */
#define UNIT_ADAPTER__VENDOR_SPECIFIC_HEADER_VSEC_LEN_MASK 0xFFF00000
#define UNIT_ADAPTER__VENDOR_SPECIFIC_HEADER_VSEC_LEN_SHIFT 20

/**** AXI_Sub_Master_Configuration register ****/
/* See Snoop description. */
#define UNIT_ADAPTER_SUB_MASTER_CFG_CTRL_CONFIGURATION_SNOOP_OVERRIDE (1 << 0)
/*
 * When Enable id snoop_override is set, this bit determines whether or not the requests from the
 * master will be snoopable. This signal overrides the PCI_E_en_no_snoop impact and the indication
 * that comes from the master.
 */
#define UNIT_ADAPTER_SUB_MASTER_CFG_CTRL_CONFIGURATION_SNOOP (1 << 1)
/* See AR_id description */
#define UNIT_ADAPTER_SUB_MASTER_CFG_CTRL_CONFIGURATION_AR_ID_OVERRIDE (1 << 2)
/*
 * When a bit in AR_id_override is set, associate bit in AR_id overrides associate AXI sub master
 * ARID[7:0].
 */
#define UNIT_ADAPTER_SUB_MASTER_CFG_CTRL_CONFIGURATION_AR_ID_MASK 0x000000F8
#define UNIT_ADAPTER_SUB_MASTER_CFG_CTRL_CONFIGURATION_AR_ID_SHIFT 3
/* See AW_id description */
#define UNIT_ADAPTER_SUB_MASTER_CFG_CTRL_CONFIGURATION_AW_ID_OVERRIDE (1 << 8)
/*
 * When a bit in AW_id_override is set, the associated bit in AW_id overrides the associated AXI sub
 * master AWID[7:0].
 */
#define UNIT_ADAPTER_SUB_MASTER_CFG_CTRL_CONFIGURATION_AW_ID_MASK 0x00003E00
#define UNIT_ADAPTER_SUB_MASTER_CFG_CTRL_CONFIGURATION_AW_ID_SHIFT 9
/* See VM_id description */
#define UNIT_ADAPTER_SUB_MASTER_CFG_CTRL_CONFIGURATION_VM_ID_IPA_OVERRIDE (1 << 14)
/*
 * When the VM_id_IPA_override bit is set, the VM_id field overrides the associated AXI sub master
 * TGTID[15:0]
 */
#define UNIT_ADAPTER_SUB_MASTER_CFG_CTRL_CONFIGURATION_VM_ID_MASK 0x7FFF8000
#define UNIT_ADAPTER_SUB_MASTER_CFG_CTRL_CONFIGURATION_VM_ID_SHIFT 15
/* When VM_id_IPA_override bit is set, the IPA indication is overriden by this bit value. */
#define UNIT_ADAPTER_SUB_MASTER_CFG_CTRL_CONFIGURATION_IPA (1 << 31)

/**** AXI_Sub_Master_Configuration_2 register ****/
/* See QoS. */
#define UNIT_ADAPTER_SUB_MASTER_CFG_CTRL_CONFIGURATION_2_QOS_OVERRIDE (1 << 0)
/* If QoS_override is set, the QoS value overrides the Master value. */
#define UNIT_ADAPTER_SUB_MASTER_CFG_CTRL_CONFIGURATION_2_QOS_MASK 0x0000000E
#define UNIT_ADAPTER_SUB_MASTER_CFG_CTRL_CONFIGURATION_2_QOS_SHIFT 1
/* Only the value for Master 0 is used. This field sets the QoS policy inside the adapter */
#define UNIT_ADAPTER_SUB_MASTER_CFG_CTRL_CONFIGURATION_2_AXI_QOS_PROMO_MASK 0x000000F0
#define UNIT_ADAPTER_SUB_MASTER_CFG_CTRL_CONFIGURATION_2_AXI_QOS_PROMO_SHIFT 4
/*
 * Only the value for Master 0 is used. If set to 1'b1, error track inside the adapter is disabled.
 */
#define UNIT_ADAPTER_SUB_MASTER_CFG_CTRL_CONFIGURATION_2_DIS_ERROR_TRACK (1 << 8)
/* Reserved */
#define UNIT_ADAPTER_SUB_MASTER_CFG_CTRL_CONFIGURATION_2_RESERVED_MASK 0xFFFFFE00
#define UNIT_ADAPTER_SUB_MASTER_CFG_CTRL_CONFIGURATION_2_RESERVED_SHIFT 9

/**** AXI_Sub_Master_Remap register ****/
/*
 * Window Size
 * This field determines the window size.
 * 1: Window size = 4KB
 * 2: Window size = 8KB
 * S: Window size = 2^(S+11) KB.
 * S<22
 * A value of 0 disables remapping.
 * A value of 21 remaps the entire 32b or 4GB window
 */
#define UNIT_ADAPTER_SUB_MASTER_CFG_CTRL_REMAP_WINDOW_SIZE_MASK 0x0000001F
#define UNIT_ADAPTER_SUB_MASTER_CFG_CTRL_REMAP_WINDOW_SIZE_SHIFT 0
/*
 * Base Address window. Bits [31:S+11] are checked for a hit. Minimum window size 4KB.
 * AXI address [31:S+11] is compared with AXI sub-master N Remap Register [31:S+11] for a window
 * hit. If matched:
 * AXI address [63:32] is replaced with AXI Sub Master N Translated Base High Register [31:0] and
 * AXI address[31:S+11] is replaced with AXI Sub Master N Translated Base Register[31:S+11]
 */
#define UNIT_ADAPTER_SUB_MASTER_CFG_CTRL_REMAP_BASE_ADDRESS_WINDOW_MASK 0xFFFFF000
#define UNIT_ADAPTER_SUB_MASTER_CFG_CTRL_REMAP_BASE_ADDRESS_WINDOW_SHIFT 12

/**** AXI_Sub_Master_Translated_Base register ****/
/* Reserved */
#define UNIT_ADAPTER_SUB_MASTER_CFG_CTRL_TRANSLATED_BASE_RESERVED_MASK 0x00000FFF
#define UNIT_ADAPTER_SUB_MASTER_CFG_CTRL_TRANSLATED_BASE_RESERVED_SHIFT 0
/* Determines Bit [31:S+11] of new window */
#define UNIT_ADAPTER_SUB_MASTER_CFG_CTRL_TRANSLATED_BASE_TRANSLATED_BASE_MASK 0xFFFFF000
#define UNIT_ADAPTER_SUB_MASTER_CFG_CTRL_TRANSLATED_BASE_TRANSLATED_BASE_SHIFT 12

/**** Interrupt_Cause register ****/
/* Hardware sets this bit to 1 on Master Read error. */
#define UNIT_ADAPTER__INTERRUPT_CAUSE_AXI_MASTER_RD_ERR (1 << 0)
/* Hardware sets this bit to 1 on Master Write error. */
#define UNIT_ADAPTER__INTERRUPT_CAUSE_AXI_MASTER_WR_ERR (1 << 1)
/*
 * Interrupt is set whenever a wakeup request is needed.
 * Clear this interrupt by changing the pme_state.
 */
#define UNIT_ADAPTER__INTERRUPT_CAUSE_WAKEUP_INT (1 << 2)
/* application_parity_error, only for USB/SATA application */
#define UNIT_ADAPTER__INTERRUPT_CAUSE_APPLICATION_PARITY_ERR (1 << 3)

/**** AXI_Master_Write_Error_Attribute_Latch register ****/
/*
 * When a write transaction is completed with an error, this field captures the error completion
 * status.
 */
#define UNIT_ADAPTER__AXI_MASTER_WR_ERR_ATTRIBUTE_LATCH_WR_ERR_COMPLETION_STATUS_MASK 0x00000003
#define UNIT_ADAPTER__AXI_MASTER_WR_ERR_ATTRIBUTE_LATCH_WR_ERR_COMPLETION_STATUS_SHIFT 0
/* ID of the sub-master that initiated the erroneous write transaction. */
#define UNIT_ADAPTER__AXI_MASTER_WR_ERR_ATTRIBUTE_LATCH_WR_ERR_MASTER_ID_MASK 0x0000001C
#define UNIT_ADAPTER__AXI_MASTER_WR_ERR_ATTRIBUTE_LATCH_WR_ERR_MASTER_ID_SHIFT 2
/*
 * This bit is set to 1 when the AXI sub master does not get a write address acknowledge for time
 * determined in AXI_master_write_timeout field.
 */
#define UNIT_ADAPTER__AXI_MASTER_WR_ERR_ATTRIBUTE_LATCH_WRITE_ADDRESS_TIMEOUT (1 << 8)
/* This bit is set to 1 when the bus response of the write returns with an error.   */
#define UNIT_ADAPTER__AXI_MASTER_WR_ERR_ATTRIBUTE_LATCH_WRITE_COMPLETION_ERROR (1 << 9)
/*
 * This bit is set to 1 when the AXI sub-master does not get a write acknowledge completion for time
 * determined in the AXI_master_write_timeout field.
 */
#define UNIT_ADAPTER__AXI_MASTER_WR_ERR_ATTRIBUTE_LATCH_WRITE_COMPLETION_TIMEOUT (1 << 10)
/*
 * This bit is set to 1 when the AXI sub-master tries to initiate a write transaction when bit BME
 * (Bus Master Enable) is cleared to 0.
 */
#define UNIT_ADAPTER__AXI_MASTER_WR_ERR_ATTRIBUTE_LATCH_WR_ERR_BLOCKED (1 << 11)

/**** AXI_Master_Read_Error_Attribute_Latch register ****/
/*
 * When a read transaction is completed with an error, this field captures the error completion
 * status.
 */
#define UNIT_ADAPTER__AXI_MASTER_RD_ERR_ATTRIBUTE_LATCH_RD_ERR_COMPLETION_STATUS_MASK 0x00000003
#define UNIT_ADAPTER__AXI_MASTER_RD_ERR_ATTRIBUTE_LATCH_RD_ERR_COMPLETION_STATUS_SHIFT 0
/* ID of the sub-master that intiated the error read transaction */
#define UNIT_ADAPTER__AXI_MASTER_RD_ERR_ATTRIBUTE_LATCH_RD_ERR_MASTER_ID_MASK 0x0000001C
#define UNIT_ADAPTER__AXI_MASTER_RD_ERR_ATTRIBUTE_LATCH_RD_ERR_MASTER_ID_SHIFT 2
/*
 * This bit is set to 1 when the AXI sub-master does not get a read address acknowledge for time
 * determined in the AXI_master_write_timout field.
 */
#define UNIT_ADAPTER__AXI_MASTER_RD_ERR_ATTRIBUTE_LATCH_READ_ADDRESS_TIMEOUT (1 << 8)
/* This bit is set to 1 when the bus response of the read returns with an error.   */
#define UNIT_ADAPTER__AXI_MASTER_RD_ERR_ATTRIBUTE_LATCH_READ_COMPLETION_ERROR (1 << 9)
/*
 * This bit is set to 1 when the AXI sub-master does not get read acknowledge completion for time
 * determined in the AXI_master_write_timout field.
 */
#define UNIT_ADAPTER__AXI_MASTER_RD_ERR_ATTRIBUTE_LATCH_READ_COMPLETION_TIMEOUT (1 << 10)
/*
 * This bit is set to 1 when the AXI sub-master tries to initiate a read transaction when bit BME
 * (Bus Master Enable) is cleared to 0.
 */
#define UNIT_ADAPTER__AXI_MASTER_RD_ERR_ATTRIBUTE_LATCH_RD_ERR_BLOCKED (1 << 11)
/* For units that do not check parity themselves, the parity is checked inside the adaptor. */
#define UNIT_ADAPTER__AXI_MASTER_RD_ERR_ATTRIBUTE_LATCH_READ_PARITY_ERROR (1 << 12)

/**** Interrupt_Cause_mask register ****/
/* Hardware sets this bit to 1 on master read error. */
#define UNIT_ADAPTER__INTERRUPT_CAUSE_MASK_AXI_MASTER_RD_ERR_MASK (1 << 0)
/* Hardware sets this bit to 1 on master write error. */
#define UNIT_ADAPTER__INTERRUPT_CAUSE_MASK_AXI_MASTER_WR_ERR_MASK (1 << 1)
/* Interrupt is set whenever a wakeup request is needed. */
#define UNIT_ADAPTER__INTERRUPT_CAUSE_MASK_WAKEUP_INT_MASK (1 << 2)
/*
 * application_parity_error
 * Only for USB/SATA application
 */
#define UNIT_ADAPTER__INTERRUPT_CAUSE_MASK_APPLICATION_PARITY_ERR (1 << 3)

/**** AXI_Master_Timeout register ****/
/*
 * AXI Master read timeout threshold (in SB clock cycles). A value of 0 disables the timer. This
 * value is multiplied by 4.
 */
#define UNIT_ADAPTER__AXI_MASTER_TIMEOUT_AXI_MASTER_READ_TIMEOUT_THRESHOLD_MASK 0x0000FFFF
#define UNIT_ADAPTER__AXI_MASTER_TIMEOUT_AXI_MASTER_READ_TIMEOUT_THRESHOLD_SHIFT 0
/*
 * AXI Master write timeout threshold (in SB clock cycles). A value of 0 disables the timer. This
 * value is multiplied by 4.
 */
#define UNIT_ADAPTER__AXI_MASTER_TIMEOUT_AXI_MASTER_WRITE_TIMEOUT_THRESHOLD_MASK 0xFFFF0000
#define UNIT_ADAPTER__AXI_MASTER_TIMEOUT_AXI_MASTER_WRITE_TIMEOUT_THRESHOLD_SHIFT 16

/**** Generic_Control_11 register ****/
/* Override the arcache value. */
#define UNIT_ADAPTER_GEN_CTRL_11_ARCACHE_OVRD (1 << 0)
/* Override the arcache value. */
#define UNIT_ADAPTER_GEN_CTRL_11_ARCACHE_MASK 0x0000001E
#define UNIT_ADAPTER_GEN_CTRL_11_ARCACHE_SHIFT 1
/* Override the awcache value. */
#define UNIT_ADAPTER_GEN_CTRL_11_AWCACHE_OVRD (1 << 5)
/* Override the awcache value. */
#define UNIT_ADAPTER_GEN_CTRL_11_AWCACHE_MASK 0x000003C0
#define UNIT_ADAPTER_GEN_CTRL_11_AWCACHE_SHIFT 6
/*
 * Bit[16] - AXPROT_Override_Disable, the APROT. For AXI read/writes buses, for all AXI masters.
 * Bits[19:17] - When AXPROT_Override set, the value of the AXPROT bus, is set tas the following:
 * AXPROT[0] = BIT[17]
 * AXPROT[1] = Not(BIT[18])
 * AXPROT[2] = BIT[19]
 */
#define UNIT_ADAPTER_GEN_CTRL_11_RESERVED_MASK 0xFFFFFC00
#define UNIT_ADAPTER_GEN_CTRL_11_RESERVED_SHIFT 10

/**** SRIOV_ID_and_Next_Offset register ****/
/* SRIOV CAP ID */
#define UNIT_ADAPTER_SRIOV_CAP_SRIOV_ID_AND_NEXT_OFFSET_SRIOV_CAP_ID_MASK 0x0000FFFF
#define UNIT_ADAPTER_SRIOV_CAP_SRIOV_ID_AND_NEXT_OFFSET_SRIOV_CAP_ID_SHIFT 0
/* SRIOV CAP Version */
#define UNIT_ADAPTER_SRIOV_CAP_SRIOV_ID_AND_NEXT_OFFSET_SRIOV_CAP_VERSION_MASK 0x000F0000
#define UNIT_ADAPTER_SRIOV_CAP_SRIOV_ID_AND_NEXT_OFFSET_SRIOV_CAP_VERSION_SHIFT 16
/* SRIOV Next CAP Offset */
#define UNIT_ADAPTER_SRIOV_CAP_SRIOV_ID_AND_NEXT_OFFSET_SRIOV_NEXT_CAP_OFFSET_MASK 0xFFF00000
#define UNIT_ADAPTER_SRIOV_CAP_SRIOV_ID_AND_NEXT_OFFSET_SRIOV_NEXT_CAP_OFFSET_SHIFT 20

/**** SRIOV_Capabilities register ****/
/* No VF Migration Capable */
#define UNIT_ADAPTER_SRIOV_CAP_SRIOV_CAPS_SRIOV_VF_MIGRATION_CAP (1 << 0)
/* SRIOV ARI Capable Hierarchy */
#define UNIT_ADAPTER_SRIOV_CAP_SRIOV_CAPS_SRIOV_ARI_CAPABLE_HIERARCHY (1 << 1)
/* SRIOV Capabilities Reserved */
#define UNIT_ADAPTER_SRIOV_CAP_SRIOV_CAPS_SRIOV_CAPS_RESERVED_MASK 0x001FFFFC
#define UNIT_ADAPTER_SRIOV_CAP_SRIOV_CAPS_SRIOV_CAPS_RESERVED_SHIFT 2
/* SRIOV VF Migration Interrupt Number. Unused */
#define UNIT_ADAPTER_SRIOV_CAP_SRIOV_CAPS_SRIOV_VF_MIGRATION_INTERRUPT_NUMBER_MASK 0xFFE00000
#define UNIT_ADAPTER_SRIOV_CAP_SRIOV_CAPS_SRIOV_VF_MIGRATION_INTERRUPT_NUMBER_SHIFT 21

/**** SRIOV_Status_and_Control register ****/
/* Enable/Disable VFs */
#define UNIT_ADAPTER_SRIOV_CAP_SRIOV_STATUS_AND_CONTROL_VF_ENABLE (1 << 0)
/* Enable VF Migration */
#define UNIT_ADAPTER_SRIOV_CAP_SRIOV_STATUS_AND_CONTROL_VF_MIGRATION_EN (1 << 1)
/* VF Migration Interrupt Enable */
#define UNIT_ADAPTER_SRIOV_CAP_SRIOV_STATUS_AND_CONTROL_VF_MIGRATION_INTERRUPT_EN (1 << 2)
/* Memory Space Enable for Virtual Functions  */
#define UNIT_ADAPTER_SRIOV_CAP_SRIOV_STATUS_AND_CONTROL_VF_MSE (1 << 3)
/* Permitted to locate VFs in function number 8-255 */
#define UNIT_ADAPTER_SRIOV_CAP_SRIOV_STATUS_AND_CONTROL_SRIOV_CONTROL_ARI_CAP (1 << 4)
/* Reserved */
#define UNIT_ADAPTER_SRIOV_CAP_SRIOV_STATUS_AND_CONTROL_SRIOV_CONTROL_RESERVED_MASK 0x0000FFE0
#define UNIT_ADAPTER_SRIOV_CAP_SRIOV_STATUS_AND_CONTROL_SRIOV_CONTROL_RESERVED_SHIFT 5
/* VF Migration Status. Unused */
#define UNIT_ADAPTER_SRIOV_CAP_SRIOV_STATUS_AND_CONTROL_SRIOV_STATUS_VF_MIGRATION_STATUS (1 << 16)
/* Reserved */
#define UNIT_ADAPTER_SRIOV_CAP_SRIOV_STATUS_AND_CONTROL_SRIOV_STATUS_RESERVED_MASK 0xFFFE0000
#define UNIT_ADAPTER_SRIOV_CAP_SRIOV_STATUS_AND_CONTROL_SRIOV_STATUS_RESERVED_SHIFT 17

/**** SRIOV_Total_and_Initial_VFs register ****/
/* Initial Number of VFs */
#define UNIT_ADAPTER_SRIOV_CAP_SRIOV_TOTAL_AND_INITIAL_VFS_SRIOV_INITIAL_VFS_MASK 0x0000FFFF
#define UNIT_ADAPTER_SRIOV_CAP_SRIOV_TOTAL_AND_INITIAL_VFS_SRIOV_INITIAL_VFS_SHIFT 0
/* Total Number of VFs. For RAID and CRYPTO this field is 16'h1. */
#define UNIT_ADAPTER_SRIOV_CAP_SRIOV_TOTAL_AND_INITIAL_VFS_SRIOV_TOTAL_VFS_MASK 0xFFFF0000
#define UNIT_ADAPTER_SRIOV_CAP_SRIOV_TOTAL_AND_INITIAL_VFS_SRIOV_TOTAL_VFS_SHIFT 16

/**** SRIOV_Register_10 register ****/
/*
 * Function Number of virtual functions. Defines how many VF(s) are visible. (Always starts from
 * VF0.)
 */
#define UNIT_ADAPTER_SRIOV_CAP_SRIOV_REGISTER_10_SRIOV_NUM_VFS_MASK 0x0000FFFF
#define UNIT_ADAPTER_SRIOV_CAP_SRIOV_REGISTER_10_SRIOV_NUM_VFS_SHIFT 0
/* Function Dependency List - Unused */
#define UNIT_ADAPTER_SRIOV_CAP_SRIOV_REGISTER_10_SRIOV_FUNCTION_DEP_LINK_MASK 0x00FF0000
#define UNIT_ADAPTER_SRIOV_CAP_SRIOV_REGISTER_10_SRIOV_FUNCTION_DEP_LINK_SHIFT 16
/* Reserved */
#define UNIT_ADAPTER_SRIOV_CAP_SRIOV_REGISTER_10_SRIOV_RESERVED_MASK 0xFF000000
#define UNIT_ADAPTER_SRIOV_CAP_SRIOV_REGISTER_10_SRIOV_RESERVED_SHIFT 24

/**** SRIOV_Register_14 register ****/
/* 4KB First VF Offset */
#define UNIT_ADAPTER_SRIOV_CAP_SRIOV_REGISTER_14_SRIOV_FIRST_VF_OFFSET_MASK 0x0000FFFF
#define UNIT_ADAPTER_SRIOV_CAP_SRIOV_REGISTER_14_SRIOV_FIRST_VF_OFFSET_SHIFT 0
/* 4KB VF Stride */
#define UNIT_ADAPTER_SRIOV_CAP_SRIOV_REGISTER_14_SRIOV_VF_STRIDE_MASK 0xFFFF0000
#define UNIT_ADAPTER_SRIOV_CAP_SRIOV_REGISTER_14_SRIOV_VF_STRIDE_SHIFT 16

/**** SRIOV_VF_Device_ID register ****/
/* Reserved */
#define UNIT_ADAPTER_SRIOV_CAP_SRIOV_VF_DEVICE_ID_SRIOV_VF_DEV_ID_RESERVED_MASK 0x0000FFFF
#define UNIT_ADAPTER_SRIOV_CAP_SRIOV_VF_DEVICE_ID_SRIOV_VF_DEV_ID_RESERVED_SHIFT 0
/* SRIOV VF Device ID */
#define UNIT_ADAPTER_SRIOV_CAP_SRIOV_VF_DEVICE_ID_SRIOV_VF_DEV_ID_MASK 0xFFFF0000
#define UNIT_ADAPTER_SRIOV_CAP_SRIOV_VF_DEVICE_ID_SRIOV_VF_DEV_ID_SHIFT 16

/**** VF_BAR_0 register ****/
/* 0 - Memory Space */
#define UNIT_ADAPTER_SRIOV_CAP_VF_BAR_0_MEMORY_SPACE_INDICATOR (1 << 0)
/*
 * BAR Type
 * 00 - 32b address
 * 01 - Reserved
 * 10 - 64b address
 * 11 - Reserved
 */
#define UNIT_ADAPTER_SRIOV_CAP_VF_BAR_0_BAR_TYPE_MASK 0x00000006
#define UNIT_ADAPTER_SRIOV_CAP_VF_BAR_0_BAR_TYPE_SHIFT 1
/* Prefetchable, no side effects on reads */
#define UNIT_ADAPTER_SRIOV_CAP_VF_BAR_0_PREFETCHABLE (1 << 3)
/* BAR Base */
#define UNIT_ADAPTER_SRIOV_CAP_VF_BAR_0_BASE_MASK 0xFFFFFFF0
#define UNIT_ADAPTER_SRIOV_CAP_VF_BAR_0_BASE_SHIFT 4

/**** VF_BAR_2 register ****/
/* 0 - Memory Space */
#define UNIT_ADAPTER_SRIOV_CAP_VF_BAR_2_MEMORY_SPACE_INDICATOR (1 << 0)
/*
 * BAR Type
 * 00 - 32b address
 * 01 - Reserved
 * 10 - 64b address
 * 11 - Reserved
 */
#define UNIT_ADAPTER_SRIOV_CAP_VF_BAR_2_BAR_TYPE_MASK 0x00000006
#define UNIT_ADAPTER_SRIOV_CAP_VF_BAR_2_BAR_TYPE_SHIFT 1
/* Prefetchable, no side effects on reads */
#define UNIT_ADAPTER_SRIOV_CAP_VF_BAR_2_PREFETCHABLE (1 << 3)
/* BAR Base */
#define UNIT_ADAPTER_SRIOV_CAP_VF_BAR_2_BASE_MASK 0xFFFFFFF0
#define UNIT_ADAPTER_SRIOV_CAP_VF_BAR_2_BASE_SHIFT 4

/**** VF_BAR_4 register ****/
/* 0 - Memory Space */
#define UNIT_ADAPTER_SRIOV_CAP_VF_BAR_4_MEMORY_SPACE_INDICATOR (1 << 0)
/*
 * BAR Type
 * 00 - 32b address
 * 01 - Reserved
 * 10 - 64b address
 * 11 - Reserved
 */
#define UNIT_ADAPTER_SRIOV_CAP_VF_BAR_4_BAR_TYPE_MASK 0x00000006
#define UNIT_ADAPTER_SRIOV_CAP_VF_BAR_4_BAR_TYPE_SHIFT 1
/* Prefetchable, no side effects on reads */
#define UNIT_ADAPTER_SRIOV_CAP_VF_BAR_4_PREFETCHABLE (1 << 3)
/* BAR Base */
#define UNIT_ADAPTER_SRIOV_CAP_VF_BAR_4_BASE_MASK 0xFFFFFFF0
#define UNIT_ADAPTER_SRIOV_CAP_VF_BAR_4_BASE_SHIFT 4

/**** Device_ID_Vendor_ID_VF register ****/
/* Vendor ID */
#define UNIT_ADAPTER_VF_REGS_DEVICE_ID_VENDOR_ID_VF_VENDOR_ID_MASK 0x0000FFFF
#define UNIT_ADAPTER_VF_REGS_DEVICE_ID_VENDOR_ID_VF_VENDOR_ID_SHIFT 0
/* Device ID */
#define UNIT_ADAPTER_VF_REGS_DEVICE_ID_VENDOR_ID_VF_DEVICE_ID_MASK 0xFFFF0000
#define UNIT_ADAPTER_VF_REGS_DEVICE_ID_VENDOR_ID_VF_DEVICE_ID_SHIFT 16

/**** Command_and_Status_VF register ****/
/* I/O Space Enable - Not applied to VF.  */
#define UNIT_ADAPTER_VF_REGS_COMMAND_AND_STATUS_VF_COMMAND_IOSE_UNUSED (1 << 0)
/* Memory Space Enable. Not applied to VF. */
#define UNIT_ADAPTER_VF_REGS_COMMAND_AND_STATUS_VF_COMMAND_MSE_UNUSED (1 << 1)
/* Bus Master Enable */
#define UNIT_ADAPTER_VF_REGS_COMMAND_AND_STATUS_VF_COMMAND_BME (1 << 2)
/* Special Cycle Enable - Unused */
#define UNIT_ADAPTER_VF_REGS_COMMAND_AND_STATUS_VF_COMMAND_SCE (1 << 3)
/* Master Write and Invalidate - Unused */
#define UNIT_ADAPTER_VF_REGS_COMMAND_AND_STATUS_VF_COMMAND_MWIE_UNUSED (1 << 4)
/* Unused */
#define UNIT_ADAPTER_VF_REGS_COMMAND_AND_STATUS_VF_COMMAND_VGA_UNUSED (1 << 5)
/* Parity Error Response */
#define UNIT_ADAPTER_VF_REGS_COMMAND_AND_STATUS_VF_COMMAND_PER (1 << 6)
/* Unused */
#define UNIT_ADAPTER_VF_REGS_COMMAND_AND_STATUS_VF_COMMAND_RESERVED_0 (1 << 7)
/* SER Enable */
#define UNIT_ADAPTER_VF_REGS_COMMAND_AND_STATUS_VF_COMMAND_SEE (1 << 8)
/* Fast Back-to-Back Enable transactions - Unused */
#define UNIT_ADAPTER_VF_REGS_COMMAND_AND_STATUS_VF_COMMAND_FBE_UNUSED (1 << 9)
/* Interrupt Disable. Not applied to VF. */
#define UNIT_ADAPTER_VF_REGS_COMMAND_AND_STATUS_VF_COMMAND_ID_UNUSED (1 << 10)
/* Unused */
#define UNIT_ADAPTER_VF_REGS_COMMAND_AND_STATUS_VF_COMMAND_RESERVED_1_MASK 0x0000F800
#define UNIT_ADAPTER_VF_REGS_COMMAND_AND_STATUS_VF_COMMAND_RESERVED_1_SHIFT 11
/* Unused */
#define UNIT_ADAPTER_VF_REGS_COMMAND_AND_STATUS_VF_STATUS_RESERVED_0_MASK 0x00070000
#define UNIT_ADAPTER_VF_REGS_COMMAND_AND_STATUS_VF_STATUS_RESERVED_0_SHIFT 16
/* Interrupt Status. Not applied to VF  */
#define UNIT_ADAPTER_VF_REGS_COMMAND_AND_STATUS_VF_STATUS_IS_UNUSED (1 << 19)
/* Capability List */
#define UNIT_ADAPTER_VF_REGS_COMMAND_AND_STATUS_VF_STATUS_CL (1 << 20)
/* Reserved (66 MHz Capable) - Unused */
#define UNIT_ADAPTER_VF_REGS_COMMAND_AND_STATUS_VF_STATUS_RESERVED_1_MASK 0x00E00000
#define UNIT_ADAPTER_VF_REGS_COMMAND_AND_STATUS_VF_STATUS_RESERVED_1_SHIFT 21
/* Master Data Parity Error */
#define UNIT_ADAPTER_VF_REGS_COMMAND_AND_STATUS_VF_STATUS_DPD (1 << 24)
/* Devsel Timing - Unused */
#define UNIT_ADAPTER_VF_REGS_COMMAND_AND_STATUS_VF_STATUS_DEVSEL_MASK 0x06000000
#define UNIT_ADAPTER_VF_REGS_COMMAND_AND_STATUS_VF_STATUS_DEVSEL_SHIFT 25
/* Signaled Target Abort - Unused */
#define UNIT_ADAPTER_VF_REGS_COMMAND_AND_STATUS_VF_STATUS_STA (1 << 27)
/* Received Target Abort - Unused */
#define UNIT_ADAPTER_VF_REGS_COMMAND_AND_STATUS_VF_STATUS_RTA (1 << 28)
/* Received Master Abort - Unused */
#define UNIT_ADAPTER_VF_REGS_COMMAND_AND_STATUS_VF_STATUS_RMA (1 << 29)
/* Signaled System Error */
#define UNIT_ADAPTER_VF_REGS_COMMAND_AND_STATUS_VF_STATUS_SSE (1 << 30)
/* Detected Parity Error */
#define UNIT_ADAPTER_VF_REGS_COMMAND_AND_STATUS_VF_STATUS_DPE (1 << 31)

/**** Revision_and_Class_VF register ****/
/* VF Rev_id. Reflects Rev_id field value from PF Revision_and_Class Register. */
#define UNIT_ADAPTER_VF_REGS_REVISION_AND_CLASS_VF_REV_ID_MASK 0x000000FF
#define UNIT_ADAPTER_VF_REGS_REVISION_AND_CLASS_VF_REV_ID_SHIFT 0
/* VF Class_PI. Reflects Class_PI field value from PF Revision_and_Class Register. */
#define UNIT_ADAPTER_VF_REGS_REVISION_AND_CLASS_VF_CLASS_PI_MASK 0x0000FF00
#define UNIT_ADAPTER_VF_REGS_REVISION_AND_CLASS_VF_CLASS_PI_SHIFT 8
/* VF Class_SCC. Reflects Class_SCC field value from PF Revision_and_Class Register. */
#define UNIT_ADAPTER_VF_REGS_REVISION_AND_CLASS_VF_CLASS_SCC_MASK 0x00FF0000
#define UNIT_ADAPTER_VF_REGS_REVISION_AND_CLASS_VF_CLASS_SCC_SHIFT 16
/* VF Class_BCC. Reflects Class_BCC field value from PF Revision_and_Class Register. */
#define UNIT_ADAPTER_VF_REGS_REVISION_AND_CLASS_VF_CLASS_BCC_MASK 0xFF000000
#define UNIT_ADAPTER_VF_REGS_REVISION_AND_CLASS_VF_CLASS_BCC_SHIFT 24

/**** PCI_Register_C_VF register ****/
/* System Cache Line Size. Not applied to VF. */
#define UNIT_ADAPTER_VF_REGS_PCI_REGISTER_C_VF_CACH_LINE_SIZE_UNUSED_MASK 0x000000FF
#define UNIT_ADAPTER_VF_REGS_PCI_REGISTER_C_VF_CACH_LINE_SIZE_UNUSED_SHIFT 0
/* Latency Timer - Unused */
#define UNIT_ADAPTER_VF_REGS_PCI_REGISTER_C_VF_LATENCY_TIMER_UNUSED_MASK 0x0000FF00
#define UNIT_ADAPTER_VF_REGS_PCI_REGISTER_C_VF_LATENCY_TIMER_UNUSED_SHIFT 8
/* Header Type - Unused */
#define UNIT_ADAPTER_VF_REGS_PCI_REGISTER_C_VF_HEADER_TYPE_MASK 0x00FF0000
#define UNIT_ADAPTER_VF_REGS_PCI_REGISTER_C_VF_HEADER_TYPE_SHIFT 16
/* Built In Self-test - Unused */
#define UNIT_ADAPTER_VF_REGS_PCI_REGISTER_C_VF_BIST_MASK 0xFF000000
#define UNIT_ADAPTER_VF_REGS_PCI_REGISTER_C_VF_BIST_SHIFT 24

/**** PCI_E_Capability_VF register ****/
/* PCI Express Capability ID */
#define UNIT_ADAPTER_VF_REGS_PCI_E_CAP_VF_PCI_E_CAP_ID_MASK 0x000000FF
#define UNIT_ADAPTER_VF_REGS_PCI_E_CAP_VF_PCI_E_CAP_ID_SHIFT 0
/* PCI Express Next Capability Pointer */
#define UNIT_ADAPTER_VF_REGS_PCI_E_CAP_VF_PCI_E_NEXT_CAP_POINTER_MASK 0x0000FF00
#define UNIT_ADAPTER_VF_REGS_PCI_E_CAP_VF_PCI_E_NEXT_CAP_POINTER_SHIFT 8
/* Undefined */
#define UNIT_ADAPTER_VF_REGS_PCI_E_CAP_VF_PCI_E_COMPATIBILITY_VERSION_MASK 0x000F0000
#define UNIT_ADAPTER_VF_REGS_PCI_E_CAP_VF_PCI_E_COMPATIBILITY_VERSION_SHIFT 16
/* Represents PCI Express Endpoint. */
#define UNIT_ADAPTER_VF_REGS_PCI_E_CAP_VF_PCI_E_DEVICE_TYPE_MASK 0x00F00000
#define UNIT_ADAPTER_VF_REGS_PCI_E_CAP_VF_PCI_E_DEVICE_TYPE_SHIFT 20
/* PCIe slot implemented. */
#define UNIT_ADAPTER_VF_REGS_PCI_E_CAP_VF_PCI_E_SLOT_IMP (1 << 24)
/* Interrupt message generated in association with status bit of this cap. */
#define UNIT_ADAPTER_VF_REGS_PCI_E_CAP_VF_PCI_E_IMN_MASK 0x3E000000
#define UNIT_ADAPTER_VF_REGS_PCI_E_CAP_VF_PCI_E_IMN_SHIFT 25
/* Undefined */
#define UNIT_ADAPTER_VF_REGS_PCI_E_CAP_VF_UNDEFINED (1 << 30)
/* Reserved */
#define UNIT_ADAPTER_VF_REGS_PCI_E_CAP_VF_RESERVED (1 << 31)

/**** PCI_E_Device_Capability_VF register ****/
/* Unused */
#define UNIT_ADAPTER_VF_REGS_PCI_E_DEVICE_CAP_VF_PCI_E_MPSS_MASK 0x00000007
#define UNIT_ADAPTER_VF_REGS_PCI_E_DEVICE_CAP_VF_PCI_E_MPSS_SHIFT 0
/* Unused */
#define UNIT_ADAPTER_VF_REGS_PCI_E_DEVICE_CAP_VF_PCI_E_PFS_MASK 0x00000018
#define UNIT_ADAPTER_VF_REGS_PCI_E_DEVICE_CAP_VF_PCI_E_PFS_SHIFT 3
/* Unused */
#define UNIT_ADAPTER_VF_REGS_PCI_E_DEVICE_CAP_VF_PCI_E_ETFS (1 << 5)
/* Unused */
#define UNIT_ADAPTER_VF_REGS_PCI_E_DEVICE_CAP_VF_PCI_E_ENDPOINT_L0_AL_MASK 0x000001C0
#define UNIT_ADAPTER_VF_REGS_PCI_E_DEVICE_CAP_VF_PCI_E_ENDPOINT_L0_AL_SHIFT 6
/* Unused */
#define UNIT_ADAPTER_VF_REGS_PCI_E_DEVICE_CAP_VF_PCI_E_ENDPOINT_L1_AL_MASK 0x00000E00
#define UNIT_ADAPTER_VF_REGS_PCI_E_DEVICE_CAP_VF_PCI_E_ENDPOINT_L1_AL_SHIFT 9
/* Undefined */
#define UNIT_ADAPTER_VF_REGS_PCI_E_DEVICE_CAP_VF_RESERVED_14_12_MASK 0x00007000
#define UNIT_ADAPTER_VF_REGS_PCI_E_DEVICE_CAP_VF_RESERVED_14_12_SHIFT 12
/* Unused */
#define UNIT_ADAPTER_VF_REGS_PCI_E_DEVICE_CAP_VF_PCI_E_RBER (1 << 15)
/* Reserved */
#define UNIT_ADAPTER_VF_REGS_PCI_E_DEVICE_CAP_VF_RESERVED_17_16_MASK 0x00030000
#define UNIT_ADAPTER_VF_REGS_PCI_E_DEVICE_CAP_VF_RESERVED_17_16_SHIFT 16
/* Unused */
#define UNIT_ADAPTER_VF_REGS_PCI_E_DEVICE_CAP_VF_PCI_E_CSPLV_MASK 0x03FC0000
#define UNIT_ADAPTER_VF_REGS_PCI_E_DEVICE_CAP_VF_PCI_E_CSPLV_SHIFT 18
/* Unused */
#define UNIT_ADAPTER_VF_REGS_PCI_E_DEVICE_CAP_VF_PCI_E_CSPLS_MASK 0x0C000000
#define UNIT_ADAPTER_VF_REGS_PCI_E_DEVICE_CAP_VF_PCI_E_CSPLS_SHIFT 26
/* Function Level Reset Support */
#define UNIT_ADAPTER_VF_REGS_PCI_E_DEVICE_CAP_VF_FLR_CAP (1 << 28)
/* Reserved */
#define UNIT_ADAPTER_VF_REGS_PCI_E_DEVICE_CAP_VF_RESERVED_31_29_MASK 0xE0000000
#define UNIT_ADAPTER_VF_REGS_PCI_E_DEVICE_CAP_VF_RESERVED_31_29_SHIFT 29

/**** PCI_E_Control_and_status_VF register ****/
/* Unused */
#define UNIT_ADAPTER_VF_REGS_PCI_E_CONTROL_AND_STATUS_VF_PCI_E_CERE (1 << 0)
/* Unused */
#define UNIT_ADAPTER_VF_REGS_PCI_E_CONTROL_AND_STATUS_VF_PCI_E_NFERE (1 << 1)
/* unused */
#define UNIT_ADAPTER_VF_REGS_PCI_E_CONTROL_AND_STATUS_VF_PCI_E_FERE (1 << 2)
/* Unused */
#define UNIT_ADAPTER_VF_REGS_PCI_E_CONTROL_AND_STATUS_VF_PCI_E_URRE (1 << 3)
/* Unused */
#define UNIT_ADAPTER_VF_REGS_PCI_E_CONTROL_AND_STATUS_VF_PCI_E_ENABLED_RELAXED_ORDERING (1 << 4)
/* Unused */
#define UNIT_ADAPTER_VF_REGS_PCI_E_CONTROL_AND_STATUS_VF_PCI_E_MPS_MASK 0x000000E0
#define UNIT_ADAPTER_VF_REGS_PCI_E_CONTROL_AND_STATUS_VF_PCI_E_MPS_SHIFT 5
/* Unused */
#define UNIT_ADAPTER_VF_REGS_PCI_E_CONTROL_AND_STATUS_VF_MSI_E_ETFE (1 << 8)
/* Unused */
#define UNIT_ADAPTER_VF_REGS_PCI_E_CONTROL_AND_STATUS_VF_PCI_E_PFE (1 << 9)
/* Unused */
#define UNIT_ADAPTER_VF_REGS_PCI_E_CONTROL_AND_STATUS_VF_PCI_E_AUX_PWR_PM_EN (1 << 10)
/*
 * Enable No Snoop.
 * If this bit is set to 1, all transactions must be snoopable. This bit is not in use. Use
 * snoop_ovrd instead.
 */
#define UNIT_ADAPTER_VF_REGS_PCI_E_CONTROL_AND_STATUS_VF_PCI_E_EN_NO_SNOOP (1 << 11)
/* Unused */
#define UNIT_ADAPTER_VF_REGS_PCI_E_CONTROL_AND_STATUS_VF_PCI_E_MRRS_MASK 0x00007000
#define UNIT_ADAPTER_VF_REGS_PCI_E_CONTROL_AND_STATUS_VF_PCI_E_MRRS_SHIFT 12
/* Function Level Reset */
#define UNIT_ADAPTER_VF_REGS_PCI_E_CONTROL_AND_STATUS_VF_VF_FLR (1 << 15)
/* Unused */
#define UNIT_ADAPTER_VF_REGS_PCI_E_CONTROL_AND_STATUS_VF_PCI_E_CED (1 << 16)
/* Unused */
#define UNIT_ADAPTER_VF_REGS_PCI_E_CONTROL_AND_STATUS_VF_PCI_E_NFED (1 << 17)
/* Reserved */
#define UNIT_ADAPTER_VF_REGS_PCI_E_CONTROL_AND_STATUS_VF_PCI_E_FED (1 << 18)
/* Unused */
#define UNIT_ADAPTER_VF_REGS_PCI_E_CONTROL_AND_STATUS_VF_PCI_E_UNSUPPORTED_REQUEST (1 << 19)
/* Unused */
#define UNIT_ADAPTER_VF_REGS_PCI_E_CONTROL_AND_STATUS_VF_PCI_E_AUX_PWR_DETECTED (1 << 20)
/* Unused */
#define UNIT_ADAPTER_VF_REGS_PCI_E_CONTROL_AND_STATUS_VF_PCI_E_TP (1 << 21)

/**** MSI_X_Reg_0_VF register ****/
/* MSI-X Capability ID */
#define UNIT_ADAPTER_VF_REGS_MSI_X_REG_0_VF_MSI_X_CAP_ID_MASK 0x000000FF
#define UNIT_ADAPTER_VF_REGS_MSI_X_REG_0_VF_MSI_X_CAP_ID_SHIFT 0
/* MSI-X Next Pointer */
#define UNIT_ADAPTER_VF_REGS_MSI_X_REG_0_VF_MSI_X_NEXT_POINTER_MASK 0x0000FF00
#define UNIT_ADAPTER_VF_REGS_MSI_X_REG_0_VF_MSI_X_NEXT_POINTER_SHIFT 8
/* Table Size */
#define UNIT_ADAPTER_VF_REGS_MSI_X_REG_0_VF_TABLE_SIZE_MASK 0x07FF0000
#define UNIT_ADAPTER_VF_REGS_MSI_X_REG_0_VF_TABLE_SIZE_SHIFT 16
/* Table Size. Supports up to 64 different interrupt vectors per function. */
#define UNIT_ADAPTER_VF_REGS_MSI_X_REG_0_VF_RESERVED_MASK 0x38000000
#define UNIT_ADAPTER_VF_REGS_MSI_X_REG_0_VF_RESERVED_SHIFT 27
/* Function Mask */
#define UNIT_ADAPTER_VF_REGS_MSI_X_REG_0_VF_FUNCTION_MASK_VF (1 << 30)
/* MSI X EN */
#define UNIT_ADAPTER_VF_REGS_MSI_X_REG_0_VF_MSI_X_EN_VF (1 << 31)

/**** MSI_X_Table_Offset_VF register ****/
/*
 * Table Base ID Indicator Register
 * Indicates that it is offset from BAR0.
 */
#define UNIT_ADAPTER_VF_REGS_MSI_X_TABLE_OFFSET_VF_TABLE_BIR_MASK 0x00000007
#define UNIT_ADAPTER_VF_REGS_MSI_X_TABLE_OFFSET_VF_TABLE_BIR_SHIFT 0
/* Table_Offset, 8B aligned */
#define UNIT_ADAPTER_VF_REGS_MSI_X_TABLE_OFFSET_VF_TABLE_OFFSET_MASK 0xFFFFFFF8
#define UNIT_ADAPTER_VF_REGS_MSI_X_TABLE_OFFSET_VF_TABLE_OFFSET_SHIFT 3

/**** MSI_X_PBA_Offset_VF register ****/
/*
 * PBA Base Indicator Register
 * Indicates that it is offset from BAR0.
 */
#define UNIT_ADAPTER_VF_REGS_MSI_X_PBA_OFFSET_VF_PBA_BIR_MASK 0x00000007
#define UNIT_ADAPTER_VF_REGS_MSI_X_PBA_OFFSET_VF_PBA_BIR_SHIFT 0
/* PBA Offset, 8B aligned */
#define UNIT_ADAPTER_VF_REGS_MSI_X_PBA_OFFSET_VF_PBA_OFFSET_MASK 0xFFFFFFF8
#define UNIT_ADAPTER_VF_REGS_MSI_X_PBA_OFFSET_VF_PBA_OFFSET_SHIFT 3

#ifdef __cplusplus
}
#endif

#endif

/** @} */


