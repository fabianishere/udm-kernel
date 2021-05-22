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

#ifndef __AL_HAL_UNIT_ADAPTER_REGS_H__
#define __AL_HAL_UNIT_ADAPTER_REGS_H__

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *                       PCIe offsets/masks (by spec)                         *
 ******************************************************************************/

/**
 * Device-ID/Vendor-ID
 */
#define AL_PCI_DEV_ID_VEN_ID			0x00
#define AL_PCI_DEV_ID_VEN_ID_DEV_ID_MASK	0xffff0000
#define AL_PCI_DEV_ID_VEN_ID_DEV_ID_SHIFT	16
#define AL_PCI_DEV_ID_VEN_ID_VEN_ID_MASK	0x0000ffff
#define AL_PCI_DEV_ID_VEN_ID_VEN_ID_SHIFT	0

/**
 * Command
 */
#define AL_PCI_COMMAND				0x04       /* 16 bits */
#define AL_PCI_COMMAND_IO			0x1        /* Enable response in I/O space */
#define AL_PCI_COMMAND_MEMORY			0x2        /* Enable response in Memory space */
#define AL_PCI_COMMAND_MASTER			0x4        /* Enable bus mastering */
#define AL_PCI_COMMAND_PARITY_ERROR		0x40       /* Parity Error Response */
#define AL_PCI_COMMAND_SERR			0x100      /* SERR enables reporting of errors */
#define AL_PCI_COMMAND_INTR_DISABLE		0x400      /* Adapter Interrupt disable */

/**
 * Status
 */
#define AL_PCI_STATUS				0x06       /* 16 bits */
#define AL_PCI_STATUS_SHIFT			16         /* status shift */
#define  AL_PCI_STATUS_IS			0x8        /* Interrupt Status */
#define  AL_PCI_STATUS_CL			0x10       /* Capabilities List */
#define  AL_PCI_STATUS_MDPE			0x100      /* Master Data Parity Error */
#define  AL_PCI_STATUS_STA			0x800      /* Signaled Target Abort */
#define  AL_PCI_STATUS_RTA			0x1000     /* Received Target Abort */
#define  AL_PCI_STATUS_RMA			0x2000     /* Received Master Abort */
#define  AL_PCI_STATUS_SSE			0x4000     /* Signaled System Error */
#define  AL_PCI_STATUS_DPE			0x8000     /* Detected Parity Error */

/**
 * Class
 */
#define PCI_CLASS_REVISION			0x08 /* High 24 bits are class, low 8 revision */
#define PCI_CLASS_REVISION_CLASS_MASK		0xffffff00
#define PCI_CLASS_REVISION_CLASS_SHIFT		8
#define PCI_CLASS_REVISION_REVISION_MASK	0x000000ff
#define PCI_CLASS_REVISION_REVISION_SHIFT	0

/** Cache line size, latency timer, header type, BIST */
#define PCI_CLS_LT_HT_BIST			0x0c

/*
 * Latency
 */
#define PCI_LATENCY				0xc

/*
 * Header Type
 */
#define AL_PCI_HEADER_TYPE			0xe  /* Header type reg */
#define  AL_PCI_HEADER_TYPE_LAYOUT		0    /* Header type layout shift */
#define  AL_PCI_HEADER_TYPE_LAYOUT_MASK		0x7f /* Header type layout mask */
#define  AL_PCI_HEADER_TYPE_MF			0x80 /* Header type multi function */

/**
 * Bars (Base Address Registers)
 */
#define  AL_PCI_BASE_ADDRESS_SPACE_IO		0x01
#define  AL_PCI_BASE_ADDRESS_MEM_TYPE_64	0x04 /* 64 bit address */
#define  AL_PCI_BASE_ADDRESS_MEM_PREFETCH	0x08 /* prefetchable? */
#define  AL_PCI_BASE_ADDRESS_DEVICE_ID		0x0c

#define  AL_PCI_BASE_ADDRESS_0			0x10
#define  AL_PCI_BASE_ADDRESS_0_HI		0x14
#define  AL_PCI_BASE_ADDRESS_2			0x18
#define  AL_PCI_BASE_ADDRESS_2_HI		0x1c
#define  AL_PCI_BASE_ADDRESS_4			0x20
#define  AL_PCI_BASE_ADDRESS_4_HI		0x24

/**
 * Expansion ROM
 */
#define	AL_PCI_EXP_ROM_BASE_ADDRESS		0x30     /* Expansion ROM BAR reg */
#define AL_PCI_EXP_ROM_EN			0x1      /* Expansion ROM BAR enable */

/**
 * Subsystem-ID/Vendor-ID
 */
#define  AL_PCI_SUBSYSTEM_VENDOR_ID		0x2c
#define  AL_PCI_SUBSYSTEM_ID			0x2e

/**
 * Interrupt line/pin
 */
#define AL_PCI_INTERRUPT_LINE			0x3c    /* 8 bits */
#define AL_PCI_INTERRUPT_PIN			0x3d    /* 8 bits */

/**
 * PCI Express capability
 */
#define AL_PCIE_EXP_CAPS			2         /* PCI Express Capabilities */
#define  AL_PCIE_EXP_CAPS_CAP_VER_SHIFT		0         /* Capability Version shift */
#define  AL_PCIE_EXP_CAPS_CAP_VER_MASK		0xf       /* Capability Version */
#define  AL_PCIE_EXP_CAPS_DEV_TYPE_SHIFT	4         /* Device/Port Type shift */
#define  AL_PCIE_EXP_CAPS_DEV_TYPE_MASK		0xf0      /* Device/Port Type */
#define  AL_PCIE_EXP_CAPS_SLOT_IMPL		0x100     /* Slot Implemented */
#define  AL_PCIE_EXP_CAPS_INT_NUM_SHIFT		9         /* Interrupt Message Number shift */
#define  AL_PCIE_EXP_CAPS_INT_NUM_MASK		0x3e00    /* Interrupt Message Number */
#define AL_PCI_EXP_DEVCAP			4         /* Device capabilities */
#define  AL_PCI_EXP_DEVCAP_PAYLOAD_SHIFT	0         /* Max_Payload_Size field shift */
#define  AL_PCI_EXP_DEVCAP_PAYLOAD		0x07      /* Max_Payload_Size */
#define  AL_PCI_EXP_DEVCAP_PHANTOM		0x18      /* Phantom functions */
#define  AL_PCI_EXP_DEVCAP_EXT_TAG		0x20      /* Extended tags */
#define  AL_PCI_EXP_DEVCAP_L0S			0x1c0     /* L0s Acceptable Latency */
#define  AL_PCI_EXP_DEVCAP_L1			0xe00     /* L1 Acceptable Latency */
#define  AL_PCI_EXP_DEVCAP_ATN_BUT		0x1000    /* Attention Button Present */
#define  AL_PCI_EXP_DEVCAP_ATN_IND		0x2000    /* Attention Indicator Present */
#define  AL_PCI_EXP_DEVCAP_PWR_IND		0x4000    /* Power Indicator Present */
#define  AL_PCI_EXP_DEVCAP_RBER			0x8000    /* Role-Based Error Reporting */
#define  AL_PCI_EXP_DEVCAP_PWR_VAL		0x3fc0000 /* Slot Power Limit Value */
#define  AL_PCI_EXP_DEVCAP_PWR_SCL		0xc000000 /* Slot Power Limit Scale */
#define  AL_PCI_EXP_DEVCAP_FLR			0x10000000 /* Function Level Reset */
#define AL_PCI_EXP_DEVCTL			8         /* Device Control */
#define  AL_PCI_EXP_DEVCTL_CERE			0x0001    /* Correctable Error Reporting En */
#define  AL_PCI_EXP_DEVCTL_NFERE		0x0002    /* Non-Fatal Error Reporting Enable */
#define  AL_PCI_EXP_DEVCTL_FERE			0x0004    /* Fatal Error Reporting Enable */
#define  AL_PCI_EXP_DEVCTL_URRE			0x0008    /* Unsupported Request Reporting En */
#define  AL_PCI_EXP_DEVCTL_RELAX_EN		0x0010    /* Enable relaxed ordering */
#define  AL_PCI_EXP_DEVCTL_PAYLOAD_SHIFT	5         /* Max_Payload_Size field shift */
#define  AL_PCI_EXP_DEVCTL_PAYLOAD		0x00e0    /* Max_Payload_Size */
#define  AL_PCI_EXP_DEVCTL_EXT_TAG		0x0100    /* Extended Tag Field Enable */
#define  AL_PCI_EXP_DEVCTL_PHANTOM		0x0200    /* Phantom Functions Enable */
#define  AL_PCI_EXP_DEVCTL_AUX_PME		0x0400    /* Auxiliary Power PM Enable */
#define  AL_PCI_EXP_DEVCTL_NOSNOOP_EN		0x0800    /* Enable No Snoop */
#define  AL_PCI_EXP_DEVCTL_READRQ_SHIFT		12        /* Max_Read_Request_Size field shift */
#define  AL_PCI_EXP_DEVCTL_READRQ		0x7000    /* Max_Read_Request_Size */
#define  AL_PCI_EXP_DEVCTL_BCR_FLR		0x8000    /* Bridge Configuration Retry / FLR */
#define AL_PCI_EXP_DEVSTA			0xA       /* Device Status */
#define AL_PCI_EXP_DEVSTA_SHIFT			16        /* Device Status field shift*/
#define AL_PCI_EXP_DEVSTA_MASK			0xFFFF0000 /* Device Status field mask*/
#define  AL_PCI_EXP_DEVSTA_CED			0x01      /* Correctable Error Detected */
#define  AL_PCI_EXP_DEVSTA_NFED			0x02      /* Non-Fatal Error Detected */
#define  AL_PCI_EXP_DEVSTA_FED			0x04      /* Fatal Error Detected */
#define  AL_PCI_EXP_DEVSTA_URD			0x08      /* Unsupported Request Detected */
#define  AL_PCI_EXP_DEVSTA_AUXPD		0x10      /* AUX Power Detected */
#define  AL_PCI_EXP_DEVSTA_TRPND		0x20      /* Transactions Pending */
#define AL_PCI_EXP_LNKCAP			0xC	  /* Link Capabilities */
#define  AL_PCI_EXP_LNKCAP_SLS_SHIFT		0	  /* Supported Link Speeds field shift */
#define  AL_PCI_EXP_LNKCAP_SLS			0xf	  /* Supported Link Speeds */
#define  AL_PCI_EXP_LNKCAP_SLS_2_5GB		0x1       /* LNKCAP2 SLS Vector bit 0 (2.5GT/s) */
#define  AL_PCI_EXP_LNKCAP_SLS_5_0GB		0x2       /* LNKCAP2 SLS Vector bit 1 (5.0GT/s) */
#define  AL_PCI_EXP_LNKCAP_MLW_SHIFT		4         /* Maximum Link Width field shift */
#define  AL_PCI_EXP_LNKCAP_MLW			0x3f0     /* Maximum Link Width */
#define  AL_PCI_EXP_LNKCAP_ASPMS		0xc00     /* ASPM Support */
#define  AL_PCI_EXP_LNKCAP_ASPMS_SHIFT		10        /* ASPM Support shift */
#define  AL_PCI_EXP_LNKCAP_L0SEL		0x7000    /* L0s Exit Latency */
#define  AL_PCI_EXP_LNKCAP_L1EL			0x38000   /* L1 Exit Latency */
#define  AL_PCI_EXP_LNKCAP_CLKPM		0x40000   /* L1 Clock Power Management */
#define  AL_PCI_EXP_LNKCAP_SDERC		0x80000   /* Surprise Down Err Reporting Capable */
#define  AL_PCI_EXP_LNKCAP_DLLLARC		0x100000  /* DLL Link Active Reporting Capable */
#define  AL_PCI_EXP_LNKCAP_LBNC			0x200000  /* Link BW Notification Capability */
#define  AL_PCI_EXP_LNKCAP_PN_SHIFT		24        /* Port Number field shift */
#define  AL_PCI_EXP_LNKCAP_PN			0xff000000 /* Port Number */
#define AL_PCI_EXP_LNKCTL			0x10      /* Link Control */
#define  AL_PCI_EXP_LNKCTL_ASPMC		0x3       /* ASPM Control */
#define  AL_PCI_EXP_LNKCTL_RCB			0x8       /* Read Completion Boundary (RCB) */
#define  AL_PCI_EXP_LNKCTL_LNK_DIS		0x10      /* Link Disable Status */
#define  AL_PCI_EXP_LNKCTL_LNK_RTRN		0x20      /* Link Retrain Status */
#define  AL_PCI_EXP_LNKCTL_CCC			0x40      /* Common Clock Configuration */
#define  AL_PCI_EXP_LNKCTL_ES			0x80      /* Extended Synch */
#define  AL_PCI_EXP_LNKCTK_ECPM			0x100     /* Enable Clock Power Management */
#define  AL_PCI_EXP_LNKCTK_HAWD			0x200     /* Hardware Autonomous Width Disable */
#define  AL_PCI_EXP_LNKCTL_LBMIE		0x400     /* Link Bandwidth Management Int Enable */
#define  AL_PCI_EXP_LNKCTL_LABIE		0x800     /* Link Autonomous Bandwidth Int Enable */
#define AL_PCI_EXP_LNKSTA			0x12      /* Link Status */
#define AL_PCI_EXP_LNKSTA_SHIFT			16        /* Link Status shift */
#define  AL_PCI_EXP_LNKSTA_CLS_SHIFT		0         /* Current Link Speed field shift */
#define  AL_PCI_EXP_LNKSTA_CLS			0x000f    /* Current Link Speed */
#define  AL_PCI_EXP_LNKSTA_CLS_2_5GB		0x01      /* Current Link Speed 2.5GT/s */
#define  AL_PCI_EXP_LNKSTA_CLS_5_0GB		0x02      /* Current Link Speed 5.0GT/s */
#define  AL_PCI_EXP_LNKSTA_CLS_8_0GB		0x03      /* Current Link Speed 8.0GT/s */
#define  AL_PCI_EXP_LNKSTA_NLW			0x03f0    /* Negotiated Link Width */
#define  AL_PCI_EXP_LNKSTA_NLW_SHIFT		4         /* start of NLW mask in link status */
#define  AL_PCI_EXP_LNKSTA_LT			0x0800    /* Link Training */
#define  AL_PCI_EXP_LNKSTA_SLC			0x1000    /* Slot Clock Configuration */
#define  AL_PCI_EXP_LNKSTA_DLLLA		0x2000    /* Data Link Layer Link Active */
#define  AL_PCI_EXP_LNKSTA_LBMS			0x4000    /* Link Bandwidth Management Status */
#define  AL_PCI_EXP_LNKSTA_LABS			0x8000    /* Link Autonomous Bandwidth Status */
#define AL_PCI_EXP_SLTCAP			0x14      /* Slot Capabilities */
#define  AL_PCI_EXP_SLTCAP_ABP			0x1       /* Attention Button Present */
#define  AL_PCI_EXP_SLTCAP_PCP			0x2       /* Power Controller Present */
#define  AL_PCI_EXP_SLTCAP_MSP			0x4       /* MRL Sensor Present */
#define  AL_PCI_EXP_SLTCAP_AIP			0x8       /* Attention Indicator Present */
#define  AL_PCI_EXP_SLTCAP_PIP			0x10      /* Power Indicator Present */
#define  AL_PCI_EXP_SLTCAP_HPS			0x20      /* Hot-Plug Surprise */
#define  AL_PCI_EXP_SLTCAP_HPC			0x40      /* Hot-Plug Capable */
#define  AL_PCI_EXP_SLTCAP_SPLV_SHIFT		7         /* Slot Power Limit Value shift */
#define  AL_PCI_EXP_SLTCAP_SPLV_MASK		0x7f80    /* Slot Power Limit Value mask */
#define  AL_PCI_EXP_SLTCAP_SPLS_SHIFT		15        /* Slot Power Limit Scale shift */
#define  AL_PCI_EXP_SLTCAP_SPLS_MASK		0x18000   /* Slot Power Limit Scale mask */
#define  AL_PCI_EXP_SLTCAP_EIP			0x20000   /* Electromechanical Interlock Present */
#define  AL_PCI_EXP_SLTCAP_NCCS			0x40000   /* No Command Completed Support */
#define  AL_PCI_EXP_SLTCAP_PSN			19        /* Physical Slot Number shift */
#define  AL_PCI_EXP_SLTCAP_PSN_MASK		0xfff80000  /* Physical Slot Number mask */
#define AL_PCI_EXP_SLTCTRL			0x18      /* Slot Control */
#define  AL_PCI_EXP_SLTCTRL_ABPE		0x1       /* Attention Button Pressed Enable */
#define  AL_PCI_EXP_SLTCTRL_PFDE		0x2       /* Power Fault Detected Enable */
#define  AL_PCI_EXP_SLTCTRL_MSCE		0x4       /* MRL Sensor Changed Enable */
#define  AL_PCI_EXP_SLTCTRL_PDCE		0x8       /* Presence Detect Changed Enable */
#define  AL_PCI_EXP_SLTCTRL_CCIE		0x10      /* Command Completed Interrupt Enable */
#define  AL_PCI_EXP_SLTCTRL_HPIE		0x20      /* Hot-Plug Interrupt Enable */
#define  AL_PCI_EXP_SLTCTRL_AIC_SHIFT		6         /* Attention Ind Control field shift */
#define  AL_PCI_EXP_SLTCTRL_AIC_MASK		0xC0      /* Attention Indicator Control */
#define  AL_PCI_EXP_SLTCTRL_PIC_SHIFT		8         /* Power Indicator Control field shift */
#define  AL_PCI_EXP_SLTCTRL_PIC_MASK		0x300     /* Power Indicator Control */
#define  AL_PCI_EXP_SLTCTRL_PIC_OFF		0x3       /* Power Indicator Control off */
#define  AL_PCI_EXP_SLTCTRL_PIC_ON		0x1       /* Power Indicator Control on */
#define  AL_PCI_EXP_SLTCTRL_PCC			0x400     /* Power Controller Control */
#define  AL_PCI_EXP_SLTCTRL_EIC			0x800     /* Electromechanical Interlock Control */
#define  AL_PCI_EXP_SLTCTRL_DLLSCE		0x1000    /* Data Link Layer State Changed Enable */
#define AL_PCI_EXP_SLTSTS			0x1a      /* Slot Status */
#define  AL_PCI_EXP_SLTSTS_ABP			0x1       /* Attention Button Pressed */
#define  AL_PCI_EXP_SLTSTS_PFD			0x2       /* Power Fault Detected */
#define  AL_PCI_EXP_SLTSTS_MSC			0x4       /* MRL Sensor Changed */
#define  AL_PCI_EXP_SLTSTS_PDC			0x8       /* Presence Detect Changed */
#define  AL_PCI_EXP_SLTSTS_CC			0x10      /* Command Completed */
#define  AL_PCI_EXP_SLTSTS_MST			0x20      /* MRL Sensor State */
#define  AL_PCI_EXP_SLTSTS_PDS			0x40      /* Presence Detect State */
#define  AL_PCI_EXP_SLTSTS_EIS			0x80      /* Electromechanical Interlock Status */
#define  AL_PCI_EXP_SLTSTS_DLLSC		0x100     /* Data Link Layer State Changed */
#define AL_PCI_EXP_DEVCAP2			0x24      /* Device Capabilities 2 */
#define  AL_PCI_EXP_DEVCAP2_CTRS		0xf       /* Completion Timeout Ranges Supported */
#define  AL_PCI_EXP_DEVCAP2_CTDS		0x10      /* Completion Timeout Disable Supported */
#define  AL_PCI_EXP_DEVCAP2_ARI_FS		0x20      /* ARI Forwarding Supported */
#define  AL_PCI_EXP_DEVCAP2_AORS		0x40      /* AtomicOp Routing Supported */
#define  AL_PCI_EXP_DEVCAP2_32AOCS		0x80      /* 32-bit AtomicOp Completer Supported */
#define  AL_PCI_EXP_DEVCAP2_64AOCS		0x100     /* 64-bit AtomicOp Completer Supported */
#define  AL_PCI_EXP_DEVCAP2_128CCS		0x200     /* 128-bit CAS Completer Supported */
#define  AL_PCI_EXP_DEVCAP2_NREPP		0x400     /* No RO-enabled PR-PR Passing */
#define  AL_PCI_EXP_DEVCAP2_LTRMS		0x800     /* LTR Mechanism Supported */
#define  AL_PCI_EXP_DEVCAP2_TCS_SHIFT		12        /* TPH Completer Supported field shift */
#define  AL_PCI_EXP_DEVCAP2_TCS_MASK		0x3000    /* TPH Completer Supported */
#define  AL_PCI_EXP_DEVCAP2_OBFFS_SHIFT		18        /* OBFF Supported field shift */
#define  AL_PCI_EXP_DEVCAP2_OBFFS_MASK		0xc0000   /* OBFF Supported */
#define  AL_PCI_EXP_DEVCAP2_EFFS		0x100000  /* Extended Fmt Field Supported */
#define  AL_PCI_EXP_DEVCAP2_EETPS		0x200000  /* End-End TLP Prefix Supported */
#define  AL_PCI_EXP_DEVCAP2_MEETP_SHIFT		22        /* Max End-End TLP Prefixes field shift */
#define  AL_PCI_EXP_DEVCAP2_MEETP_MASK		0xc00000  /* Max End-End TLP Prefixes */
#define AL_PCI_EXP_DEVCTL2			0x28      /* Device Control 2 */
#define  AL_PCI_EXP_DEVCTL2_CMPLT_17S_64S	0xe	  /* Compl Timeout of 17[sec] - 64[sec] */
#define AL_PCI_EXP_LNKCAP2			0x2c      /* Link Capabilities 2 */
#define  AL_PCI_EXP_LNKCAP2_SLSV		0xfe      /* Supported Link Speeds Vector */
#define  AL_PCI_EXP_LNKCAP2_CS			0x100     /* Crosslink Supported */
#define AL_PCI_EXP_LNKCTL2			0x30      /* Link Control 2 */
#define   AL_PCI_EXP_LNKCTL2_TLS_SHIFT		0         /* Target Link Speed field shift */
#define   AL_PCI_EXP_LNKCTL2_TLS		0xf       /* Target Link Speed */
#define   AL_PCI_EXP_LNKCTL2_EC			0x10      /* Enter Compliance */
#define   AL_PCI_EXP_LNKCTL2_HASD		0x20      /* Hardware Autonomous Speed Disable */
#define   AL_PCI_EXP_LNKCTL2_TM			0x380     /* Transmit Margin */
#define   AL_PCI_EXP_LNKCTL2_EMC		0x400     /* Enter Modified Compliance */
#define   AL_PCI_EXP_LNKCTL2_CSOS		0x800     /* Compliance SOS */
#define   AL_PCI_EXP_LNKCTL2_CPDE		0xf000    /* Compliance Preset/De-emphasis */
#define AL_PCI_EXP_LNKSTS2			0x32      /* Link Status 2 */
#define AL_PCI_EXP_LNKSTS2_SHIFT		16        /* Link Status 2 shift */
#define   AL_PCI_EXP_LNKSTS2_CDEL		0x1       /* Current De-emphasis Level */
#define   AL_PCI_EXP_LNKSTS2_EQC		0x2       /* Equalization Complete */
#define   AL_PCI_EXP_LNKSTS2_EQ_P1_S		0x4       /* Equalization Phase 1 Successful */
#define   AL_PCI_EXP_LNKSTS2_EQ_P2_S		0x8       /* Equalization Phase 2 Successful */
#define   AL_PCI_EXP_LNKSTS2_EQ_P3_S		0x10      /* Equalization Phase 3 Successful */
#define   AL_PCI_EXP_LNKSTS2_EQ_REQ		0x20      /* Link Equalization Request */
#define AL_PCI_EXP_LNKCTL3			0x4       /* Link Control 3 */
#define  AL_PCI_EXP_LNKCTL3_PRFRM_EQ		AL_BIT(0) /* Bit0: Perform Eq */
#define  AL_PCI_EXP_LNKCTL3_EQ_INT_EN		AL_BIT(1) /* Bit1: Link Eq Request Int Enable */
#define AL_PCI_EXP_LANE_EQ_CTRL(lane)		(0xc + (2 * (lane))) /* Lane Equalization
									Control Register */
#define AL_PCI_EXP_LANE_EQ_CTRL_SIZE		2         /* Lane Equalization Control Reg size */
#define  AL_PCI_EXP_LANE_EQ_CTRL_DPTP		0xf       /* Downstream Port Transmitter Preset */
#define  AL_PCI_EXP_LANE_EQ_CTRL_DPRPH		0x70      /* Downstream Port Receiver Preset Hint */
#define  AL_PCI_EXP_LANE_EQ_CTRL_UPTP		0xf00     /* Upstream Port Transmitter Preset */
#define  AL_PCI_EXP_LANE_EQ_CTRL_UPRPH		0x7000    /* Upstream Port Receiver Preset Hint */

/**
 * MSI-x capability
 */
#define AL_PCI_MSIX_MSGCTRL			0          /* MSIX message control reg */
#define  AL_PCI_MSIX_MSGCTRL_TBL_SIZE		0x7ff      /* MSIX table size */
#define  AL_PCI_MSIX_MSGCTRL_TBL_SIZE_SHIFT	16         /* MSIX table size shift */
#define  AL_PCI_MSIX_MSGCTRL_EN			0x80000000 /* MSIX enable */
#define  AL_PCI_MSIX_MSGCTRL_MASK		0x40000000 /* MSIX mask */
#define AL_PCI_MSIX_TABLE			0x4        /* MSIX table offset and bar reg */
#define  AL_PCI_MSIX_TABLE_OFFSET		0xfffffff8 /* MSIX table offset */
#define  AL_PCI_MSIX_TABLE_BAR			0x7        /* MSIX table BAR */
#define AL_PCI_MSIX_PBA				0x8        /* MSIX pba offset and bar reg */
#define  AL_PCI_MSIX_PBA_OFFSET			0xfffffff8 /* MSIX pba offset */
#define  AL_PCI_MSIX_PBA_BAR			0x7        /* MSIX pba BAR */

/*
 * MSI capability
 */
#define AL_PCI_MSI_MSGCTRL			2          /* MSI message control reg */
#define  AL_PCI_MSI_MSGCTRL_EN			0x1        /* MSI enable */
#define  AL_PCI_MSI_MSGCTRL_MMC			0xe        /* MSI Multiple Message Capable */
#define  AL_PCI_MSI_MSGCTRL_MME			0x70       /* MSI Multiple Message Enable */
#define  AL_PCI_MSI_MSGCTRL_64_BAC		0x80       /* MSI 64 bit address capable */
#define  AL_PCI_MSI_MSGCTRL_PVMC		0x100      /* MSI Per-vector masking capable */
#define AL_PCI_MSI_MSGADR			0x4        /* MSI Message Address reg */
#define AL_PCI_MSI_MSG_UPPER_ADR		0x8        /* MSI Message Upper Address reg */
#define AL_PCI_MSI_MSGDATA			0xc        /* MSI Message Data reg */
#define AL_PCI_MSI_MASK_BITS			0x10       /* MSI Mask Bits reg */
#define AL_PCI_MSI_PENDING_BITS			0x14       /* MSI Pending Bits reg */

/**
 * TPH capability
 */
#define AL_PCI_TPH_REQ_CAP			4     /* TPH Requester Capability reg */
#define  AL_PCI_TPH_REQ_CAP_NSMS		0x1   /* TPH No ST Mode Supported */
#define  AL_PCI_TPH_REQ_CAP_IVMS		0x2   /* TPH Interrupt Vector Mode Supported */
#define  AL_PCI_TPH_REQ_CAP_DSMS		0x4   /* TPH Device Specific Mode Supported */
#define  AL_PCI_TPH_REQ_CAP_ETRS		0x100 /* TPH Extended TPH Requester Supported */
#define  AL_PCI_TPH_REQ_CAP_STL			9     /* TPH ST Table Location */
#define  AL_PCI_TPH_REQ_CAP_STL_MASK		0x600 /* TPH ST Table Location mask */
#define  AL_PCI_TPH_REQ_CAP_STS			16    /* TPH ST Table Size */
#define  AL_PCI_TPH_REQ_CAP_STS_MASK		0x7FF0000 /* TPH ST Table Size mask */
#define AL_PCI_TPH_REQ_CTRL_OFFSET		0x8   /* offset from TPH cap */
#define AL_PCI_TPH_REQ_ENABLE_MASK		0x300 /* req enable mask in request control reg */
#define AL_PCI_TPH_REQ_ENABLE_SHIFT		0x8   /* req enable shift in request control reg */
#define AL_PCI_TPH_REQ_ENABLE_REG		0x01  /* regular req control enable */
#define AL_PCI_TPH_REQ_ENABLE_REG_EXT		0x11  /* regular/extended req control enable */

 /**
 * Device Serial Number capability
 */
#define  AL_PCI_EXP_DSN_DW1	4	/* Device Serial Number lower dword */
#define  AL_PCI_EXP_DSN_DW2	8	/* Device Serial Number upper dword*/

/**
 * PM capability
 */
#define AL_ADAPTER_PM_PMC			2	/* Power Management Capabilities reg */
#define AL_ADAPTER_PM_PMC_SHIFT			16	/* Power Management Capabilities shift */
#define AL_ADAPTER_PM_PMCSR			4	/* Power Management Control/Status reg */
#define AL_ADAPTER_PM_0_PM_NEXT_CAP_MASK	0xff00
#define AL_ADAPTER_PM_0_PM_NEXT_CAP_SHIFT	8
#define AL_ADAPTER_PM_0_PM_NEXT_CAP_VAL_MSIX	0x90
#define AL_ADAPTER_PM_1_PME_EN			0x100	/* PM enable */
#define AL_ADAPTER_PM_1_PWR_STATE_MASK		0x3	/* PM state mask */
#define AL_ADAPTER_PM_1_PWR_STATE_D0		0x0	/* PM D0 state */
#define AL_ADAPTER_PM_1_PWR_STATE_D1		0x1	/* PM D1 state */
#define AL_ADAPTER_PM_1_PWR_STATE_D2		0x2	/* PM D2 state */
#define AL_ADAPTER_PM_1_PWR_STATE_D3		0x3	/* PM D3 state */
#define AL_ADAPTER_PM_1_NO_SOFT_RESET_MASK	0x8	/* 0 - cfg space is reset upon D3 --> D0 */

/**
 * SR-IOV capability
 */
#define AL_PCI_SRIOV_CAPABILTIES		4          /* SR-IOV Capabilities reg */
#define  AL_PCI_SRIOV_CAPABILTIES_VFMC		0x1        /* SR-IOV VF Migration Capable */
#define  AL_PCI_SRIOV_CAPABILTIES_ACHP		0x2        /* SR-IOV ARI Capable Hier Preserved */
#define  AL_PCI_SRIOV_CAPABILTIES_VFMIMN	21         /* SR-IOV VF Mig Int Msg Num shift */
#define  AL_PCI_SRIOV_CAPABILTIES_VFMIMN_MASK	0xffe00000 /* SR-IOV VF Mig Int Msg Num mask */
#define AL_PCI_SRIOV_CONTROL			8          /* SR-IOV Control reg */
#define  AL_PCI_SRIOV_CONTROL_VE		0x1        /* SR-IOV VF Enable */
#define  AL_PCI_SRIOV_CONTROL_VME		0x2        /* SR-IOV VF Migration Enable */
#define  AL_PCI_SRIOV_CONTROL_VMIE		0x4        /* SR-IOV VF Migration Int Enable */
#define  AL_PCI_SRIOV_CONTROL_VM		0x8        /* SR-IOV VF MSE */
#define  AL_PCI_SRIOV_CONTROL_ACH		0x10       /* SR-IOV ARI Capable Hierarchy */
#define AL_PCI_SRIOV_INITIAL_VFS		12         /* SR-IOV InitialVFs reg */
#define AL_PCI_SRIOV_TOTAL_VFS			14         /* SR-IOV TotalVFs reg */
#define AL_PCI_SRIOV_NUM_VFS			16         /* SR-IOV NumVFs reg */
#define AL_PCI_SRIOV_SUP_SYSTEM_PAGE_SIZE	28         /* SR-IOV Supported Page Sizes reg */
#define AL_PCI_SRIOV_SYSTEM_PAGE_SIZE		32         /* SR-IOV System Page Size reg */

/**
 * ARI capability
 */
#define AL_PCI_ARI_CAPABILITY			4          /* ARI Capability reg */
#define  AL_PCI_ARI_CAPABILITY_MFGC		0x1        /* ARI MFVC Function Groups Capability */
#define  AL_PCI_ARI_CAPABILITY_AFGC		0x2        /* ARI ACS Function Groups Capability */
#define  AL_PCI_ARI_CAPABILITY_NFN		8          /* ARI Next Function Number shift */
#define  AL_PCI_ARI_CAPABILITY_NFN_MASK		0xff00     /* ARI Next Function Number mask */

/**
 * VPD capability
 */
#define AL_PCI_VPD_CTRL				2          /* VPD control reg */
#define AL_PCI_VPD_CTRL_SHIFT			16         /* VPD control shift */
#define  AL_PCI_VPD_CTRL_ADDRESS		0x7fff     /* VPD address */
#define  AL_PCI_VPD_CTRL_F			0x8000     /* VPD F */
#define AL_PCI_VPD_DATA				4          /* VPD data reg */

/**
 * ACS capability
 */
#define AL_PCI_ACS_CAPABILITY			4         /* ACS Capability reg */
#define  AL_PCI_ACS_CAPABILITY_SV		0x1       /* ACS Source Validation */
#define  AL_PCI_ACS_CAPABILITY_TB		0x2       /* ACS Translation Blocking */
#define  AL_PCI_ACS_CAPABILITY_P2PRR		0x4       /* ACS P2P Request Redirect */
#define  AL_PCI_ACS_CAPABILITY_P2PCR		0x8       /* ACS P2P Completion Redirect */
#define  AL_PCI_ACS_CAPABILITY_UF		0x10      /* ACS Upstream Forwarding */
#define  AL_PCI_ACS_CAPABILITY_P2PEC		0x20      /* ACS P2P Egress Control */
#define  AL_PCI_ACS_CAPABILITY_DTP2P		0x40      /* ACS Direct Translated P2P */
#define  AL_PCI_ACS_CAPABILITY_ECVS		8         /* ACS Egress Control Vector Size shift */
#define  AL_PCI_ACS_CAPABILITY_ECVS_MASK	0xff00    /* ACS Egress Control Vector Size mask */
#define AL_PCI_ACS_CONTROL			6         /* ACS Control reg */
#define AL_PCI_ACS_CONTROL_SHIFT		16        /* ACS Control shift */
#define  AL_PCI_ACS_CONTROL_SVE			0x1       /* ACS Source Validation Enable */
#define  AL_PCI_ACS_CONTROL_TBE			0x2       /* ACS Translation Blocking Enable */
#define  AL_PCI_ACS_CONTROL_P2PRRE		0x4       /* ACS P2P Request Redirect Enable */
#define  AL_PCI_ACS_CONTROL_P2PCRE		0x8       /* ACS P2P Completion Redirect Enable */
#define  AL_PCI_ACS_CONTROL_UFE			0x10      /* ACS ACS Upstream Forwarding Enable */
#define  AL_PCI_ACS_CONTROL_P2PECE		0x20      /* ACS P2P Egress Control Enable */
#define  AL_PCI_ACS_CONTROL_DTP2P		0x40      /* ACS Direct Translated P2P Enable */
#define AL_PCI_ACS_EGRESS_CONTROL_VEC		8         /* ACS Egress Control Vector reg */

/**
 * AER capability
 */
#define AL_PCI_AER_UCORR_ERR_STATUS	4         /* Uncorrectable Error Status reg */
#define AL_PCI_AER_UCORR_ERR_MASK	8         /* Uncorrectable Error Mask reg */
#define  AL_PCI_AER_UCORR_ERR_MASK_DPEM	0x10      /* Data Link Protocol Error Mask */
#define  AL_PCI_AER_UCORR_ERR_MASK_SDEM	0x20      /* Surprise Down Error Mask */
#define  AL_PCI_AER_UCORR_ERR_MASK_PTM	0x1000    /* Poisoned TLP Mask */
#define  AL_PCI_AER_UCORR_ERR_MASK_FPEM	0x2000    /* Flow Control Protocol Error Mask */
#define  AL_PCI_AER_UCORR_ERR_MASK_CTM  0x4000    /* Completion Timeout Mask */
#define  AL_PCI_AER_UCORR_ERR_MASK_CAM	0x8000    /* Completer Abort Mask */
#define  AL_PCI_AER_UCORR_ERR_MASK_UCM	0x10000   /* Unexpected Completion Mask */
#define  AL_PCI_AER_UCORR_ERR_MASK_ROM	0x20000   /* Receiver Overflow Mask */
#define  AL_PCI_AER_UCORR_ERR_MASK_MTM	0x40000   /* Malformed TLP Mask */
#define  AL_PCI_AER_UCORR_ERR_MASK_EEM	0x80000   /* ECRC Error Mask */
#define  AL_PCI_AER_UCORR_ERR_MASK_UREM	0x100000  /* Unsupported Request Error Mask */
#define  AL_PCI_AER_UCORR_ERR_MASK_AVM	0x200000  /* ACS Violation Mask */
#define  AL_PCI_AER_UCORR_ERR_MASK_UIEM	0x400000  /* Uncorrectable Internal Error Mask */
#define  AL_PCI_AER_UCORR_ERR_MASK_MBTM	0x800000  /* MC Blocked TLP Mask */
#define  AL_PCI_AER_UCORR_ERR_MASK_AEBM	0x1000000 /* AtomicOp Egress Blocked Mask */
#define  AL_PCI_AER_UCORR_ERR_MASK_TPBE	0x2000000 /* TLP Prefix Blocked Error Mask */
#define AL_PCI_AER_UCORR_ERR_SEV	12        /* Uncorrectable Error Severity reg */
#define  AL_PCI_AER_UCORR_ERR_SEV_DLPES	0x10      /* Data Link Protocol Error Severity */
#define  AL_PCI_AER_UCORR_ERR_SEV_SDES	0x20      /* Surprise Down Error Severity */
#define  AL_PCI_AER_UCORR_ERR_SEV_PTS	0x1000    /* Poisoned TLP Severity */
#define  AL_PCI_AER_UCORR_ERR_SEV_FCPES	0x2000    /* Flow Control Protocol Error Severity */
#define  AL_PCI_AER_UCORR_ERR_SEV_CTES	0x4000    /* Completion Timeout Error Severity */
#define  AL_PCI_AER_UCORR_ERR_SEV_CAES	0x8000    /* Completer Abort Error Severity */
#define  AL_PCI_AER_UCORR_ERR_SEV_UCES	0x10000   /* Unexpected Completion Error Severity */
#define  AL_PCI_AER_UCORR_ERR_SEV_ROES	0x20000   /* Receiver Overflow Error Severity */
#define  AL_PCI_AER_UCORR_ERR_SEV_MTS	0x40000   /* Malformed TLP Severity */
#define  AL_PCI_AER_UCORR_ERR_SEV_EES	0x80000   /* ECRC Error Severity */
#define  AL_PCI_AER_UCORR_ERR_SEV_URES	0x100000  /* Unsupported Request Error Severity */
#define  AL_PCI_AER_UCORR_ERR_SEV_AVS	0x200000  /* ACS Violation Severity */
#define  AL_PCI_AER_UCORR_ERR_SEV_UIES	0x400000  /* Uncorrectable Internal Error Severity */
#define  AL_PCI_AER_UCORR_ERR_SEV_MBTS	0x800000  /* MC Blocked TLP Severity */
#define  AL_PCI_AER_UCORR_ERR_SEV_AEBS	0x1000000 /* AtomicOp Egress Blocked Severity */
#define  AL_PCI_AER_UCORR_ERR_SEV_TPBES	0x2000000 /* TLP Prefix Blocked Error Severity */
#define AL_PCI_AER_CORR_ERR_STATUS	16        /* Correctable Error Status reg */
#define AL_PCI_AER_CORR_ERR_MASK	20        /* Correctable Error Mask reg */
#define  AL_PCI_AER_CORR_ERR_MASK_REM	0x1       /* Correctable Receiver Error Mask */
#define  AL_PCI_AER_CORR_ERR_MASK_BTM	0x40      /* Correctable Bad TLP Mask */
#define  AL_PCI_AER_CORR_ERR_MASK_BDM	0x80      /* Correctable Bad DLLP Mask */
#define  AL_PCI_AER_CORR_ERR_MASK_RRM	0x100     /* Correctable REPLAY_NUM Rollover Mask */
#define  AL_PCI_AER_CORR_ERR_MASK_RTTM	0x1000    /* Correctable Replay Timer Timeout Mask */
#define  AL_PCI_AER_CORR_ERR_MASK_ANEM	0x2000    /* Correctable Advisory Non-Fatal Error Mask */
#define  AL_PCI_AER_CORR_ERR_MASK_CIEM	0x4000    /* Correctable Corrected Internal Error Mask */
#define  AL_PCI_AER_CORR_ERR_MASK_HLOM	0x8000    /* Correctable Header Log Overflow Mask */

/**
 * Secondary PCI Express Extended Capability
 */
#define AL_PCI_SEC_LINK_CTRL_3			4      /* Link Control 3 reg */
#define  AL_PCI_SEC_LINK_CTRL_3_PE		0x1    /* Perform Equalization */
#define  AL_PCI_SEC_LINK_CTRL_3_LERIE		0x2    /* Link Equal Request Int Enable */
#define AL_PCI_SEC_LANE_ERR_STATUS		8      /* Lane Error Status reg */
#define AL_PCI_SEC_LANE_EQ_CTRL			12     /* Lane Equalization Control reg */
#define  AL_PCI_SEC_LANE_EQ_CTRL_DPTP		0      /* DSP Transmitter Preset */
#define  AL_PCI_SEC_LANE_EQ_CTRL_DPTP_MASK	0xf    /* DSP Transmitter Preset mask */
#define  AL_PCI_SEC_LANE_EQ_CTRL_DPRPH		4      /* DSP Receiver Preset Hint */
#define  AL_PCI_SEC_LANE_EQ_CTRL_DPRPH_MASK	0x70   /* DSP Receiver Preset Hint mask */
#define  AL_PCI_SEC_LANE_EQ_CTRL_UPTP		8      /* Upstream Port Transmitter Preset */
#define  AL_PCI_SEC_LANE_EQ_CTRL_UPTP_MASK	0xf00  /* Upstream Port Transmitter Preset mask */
#define  AL_PCI_SEC_LANE_EQ_CTRL_UPRPH		12     /* Upstream Port Receiver Preset Hint */
#define  AL_PCI_SEC_LANE_EQ_CTRL_UPRPH_MASK	0x7000 /* Upstream Port Receiver Preset Hint mask */

/******************************************************************************
 *                       PCIe bridge offsets/masks (by spec)                  *
 ******************************************************************************/
#define AL_PCI_PRIMARY_BUS_NUM			0x18    /* Primary Bus number */
#define AL_PCI_SECONDARY_BUS_NUM		0x19    /* Secondary Bus number */
#define AL_PCI_SUBORDINATE_BUS_NUM		0x1a    /* Subordinate Bus number */
#define AL_PCI_SEC_LAT_TIMER			0x1b    /* Secondary Latency Timer */
#define AL_PCI_IO_BASE				0x1c    /* I/O Base */
#define AL_PCI_IO_LIMIT				0x1d    /* I/O limit */
#define AL_PCI_SEC_STATUS			0x1e    /* Secondary Status */
#define AL_PCI_MEM_BASE				0x20    /* memory base */
#define  AL_PCI_MEM_BASE_RO_SHIFT		0       /* memory base RO shift */
#define  AL_PCI_MEM_BASE_RO_MASK		0xf     /* memory base RO mask */
#define  AL_PCI_MEM_BASE_ADDR_SHIFT		4       /* memory base address shift */
#define  AL_PCI_MEM_BASE_ADDR_MASK		0xfff0  /* memory base address mask */
#define AL_PCI_MEM_LIMIT			0x22    /* memory limit */
#define  AL_PCI_MEM_LIMIT_RO_SHIFT		0       /* memory limit RO shift */
#define  AL_PCI_MEM_LIMIT_RO_MASK		0xf     /* memory limit RO mask */
#define  AL_PCI_MEM_LIMIT_ADDR_SHIFT		4       /* memory limit address shift */
#define  AL_PCI_MEM_LIMIT_ADDR_MASK		0xfff0  /* memory limit address mask */
#define AL_PCI_PREFETCH_MEM_BASE		0x24    /* Prefetchable Memory Base */
#define AL_PCI_PREFETCH_MEM_BASE_RO_SHIFT	0       /* Prefetchable Memory Base RO */
#define AL_PCI_PREFETCH_MEM_BASE_RO_MASK	0xf     /* Prefetchable Memory Base Ro */
#define AL_PCI_PREFETCH_MEM_BASE_ADDR_SHIFT	4       /* Prefetchable Memory Base address */
#define AL_PCI_PREFETCH_MEM_BASE_ADDR_MASK	0xfff0  /* Prefetchable Memory Base address */
#define AL_PCI_PREFETCH_MEM_LIMIT		0x26    /* Prefetchable Memory limit */
#define AL_PCI_PREFETCH_MEM_LIMIT_RO_SHIFT	0       /* Prefetchable Memory limit RO */
#define AL_PCI_PREFETCH_MEM_LIMIT_RO_MASK	0xf     /* Prefetchable Memory limit RO */
#define AL_PCI_PREFETCH_MEM_LIMIT_ADDR_SHIFT	4       /* Prefetchable Memory limit address */
#define AL_PCI_PREFETCH_MEM_LIMIT_ADDR_MASK	0xfff0  /* Prefetchable Memory limit address */
#define AL_PCI_PREFETCH_BASE_UPPER_32B		0x28    /* Prefetchable Base Upper 32 Bits */
#define AL_PCI_PREFETCH_LIMIT_UPPER_32B		0x2c    /* Prefetchable limit Upper 32 Bits */
#define AL_PCI_IO_BASE_UPPER_16B		0x30    /* I/O Base Upper 16 Bits */
#define AL_PCI_IO_LIMIT_UPPER_16B		0x32    /* I/O Limit Upper 16 Bits */
#define AL_PCI_BRIDGE_CTRL			0x3e    /* bridge control */
#define  AL_PCI_BRIDGE_CTRL_PERE		0x1     /* Parity Error Response Enable */
#define  AL_PCI_BRIDGE_CTRL_SERR_EN		0x2     /* SERR# Enable */
#define  AL_PCI_BRIDGE_CTRL_ISA_EN		0x4     /* ISA Enable */
#define  AL_PCI_BRIDGE_CTRL_VGA_EN		0x8     /* VGA Enable */
#define  AL_PCI_BRIDGE_CTRL_VGA_16_DECODE	0x10    /* VGA 16-bit decode */
#define  AL_PCI_BRIDGE_CTRL_MAM			0x20    /* Master-Abort Mode */
#define  AL_PCI_BRIDGE_CTRL_SBR			0x40    /* Secondary Bus Reset */
#define  AL_PCI_BRIDGE_CTRL_FBTBE		0x80    /* Fast Back-to-Back Enable */
#define  AL_PCI_BRIDGE_CTRL_PDT			0x100   /* Primary Discard Timer */
#define  AL_PCI_BRIDGE_CTRL_SDT			0x200   /* Secondary Discard Timer */
#define  AL_PCI_BRIDGE_CTRL_DTS			0x400   /* Discard Timer Status */
#define  AL_PCI_BRIDGE_CTRL_DTSE		0x800   /* Discard Timer SERR# Enable */

/******************************************************************************
 *          AL specific PCIe controller config space offsets/masks            *
 ******************************************************************************/

/**
 * Capabilities offsets
 */
#define AL_PCIE_CFG_EXP_CAP_BASE_OFFSET		0x70
#define AL_PCIE_CFG_MSI_CAP_BASE_OFFSET		0xb0
#define AL_PCIE_CFG_EXP_DSN			0x140
#define AL_PCIE_CFG_TPH_CAP_BASE_OFFSET		0x178
#define AL_PCIE_CFG_TPH_CAP_BASE_OFFSET_V3	0x188

/******************************************************************************
 *          AL Specific Embedded PCIe Adapter config space offset/masks       *
 ******************************************************************************/

/**
 * Capabilities offsets
 */
#define AL_PCI_EXP_CAP_BASE			0x40
#define AL_PCI_MSI_CAP_BASE			0x50
#define AL_PCI_MSIX_CAP_BASE			0xb0
#define AL_ADAPTER_PM_0				0x80
#define AL_ADAPTER_PM_1				0x84

/**
 * AXI configuration and control
 */
#define  AL_PCI_AXI_CFG_AND_CTR_0		0x110
#define  AL_PCI_AXI_CFG_AND_CTR_1		0x130
#define  AL_PCI_AXI_CFG_AND_CTR_2		0x150
#define  AL_PCI_AXI_CFG_AND_CTR_3		0x170

/**
 * Adapter Application control
 */
#define  AL_PCI_APP_CONTROL			0x220

/**
 * SR/IOV
 */
#define  AL_PCI_SRIOV_TOTAL_AND_INITIAL_VFS	0x30c
#define  AL_PCI_VF_BASE_ADDRESS_0		0x324

/* Sub Master Configuration & Control registers */
/* AXI_Sub_Master_Configuration */
#define AL_ADAPTER_SMCC					0x110
#define AL_ADAPTER_SMCC_BUNDLE_SIZE			0x20
#define AL_ADAPTER_SMCC_BUNDLE_NUM			0x4
#define AL_ADAPTER_SMCC_CONF_SNOOP_OVR		AL_BIT(0) /* Snoop Override*/
#define AL_ADAPTER_SMCC_CONF_SNOOP_ENABLE	AL_BIT(1) /* Snoop Enable*/

/* AXI_Sub_Master_Configuration_2 */
#define AL_ADAPTER_SMCC_CONF_2					0x114
#define AL_ADAPTER_SMCC_CONF_2_DIS_ERROR_TRACK		AL_BIT(8)

/* Interrupt_Cause register */
#define AL_ADAPTER_INT_CAUSE			0x1B0
#define AL_ADAPTER_INT_CAUSE_WR_ERR		AL_BIT(1)
#define AL_ADAPTER_INT_CAUSE_RD_ERR		AL_BIT(0)
#define AL_ADAPTER_INT_CAUSE_APP_PARITY_ERR	AL_BIT(3)

/* AXI_Master_Write_Error_Attribute_Latch register */
/* AXI_Master_Read_Error_Attribute_Latch register */
#define AL_ADAPTER_AXI_MSTR_WR_ERR_ATTR			0x1B4
#define AL_ADAPTER_AXI_MSTR_RD_ERR_ATTR			0x1B8

#define AL_ADAPTER_AXI_MSTR_RD_WR_ERR_ATTR_COMP_STAT_MASK	AL_FIELD_MASK(1, 0)
#define AL_ADAPTER_AXI_MSTR_RD_WR_ERR_ATTR_COMP_STAT_SHIFT	0
#define AL_ADAPTER_AXI_MSTR_RD_WR_ERR_ATTR_MSTR_ID_MASK		AL_FIELD_MASK(4, 2)
#define AL_ADAPTER_AXI_MSTR_RD_WR_ERR_ATTR_MSTR_ID_SHIFT	2
#define AL_ADAPTER_AXI_MSTR_RD_WR_ERR_ATTR_ADDR_TO		AL_BIT(8)
#define AL_ADAPTER_AXI_MSTR_RD_WR_ERR_ATTR_COMP_ERR		AL_BIT(9)
#define AL_ADAPTER_AXI_MSTR_RD_WR_ERR_ATTR_COMP_TO		AL_BIT(10)
#define AL_ADAPTER_AXI_MSTR_RD_WR_ERR_ATTR_ERR_BLK		AL_BIT(11)
#define AL_ADAPTER_AXI_MSTR_RD_ERR_ATTR_RD_PARITY_ERR		AL_BIT(12)

/* Interrupt_Cause_mask register */
#define AL_ADAPTER_INT_CAUSE_MASK			0x1BC
#define AL_ADAPTER_INT_CAUSE_MASK_WR_ERR		AL_BIT(1)
#define AL_ADAPTER_INT_CAUSE_MASK_RD_ERR		AL_BIT(0)
#define AL_ADAPTER_INT_CAUSE_MASK_APP_PARITY_ERR	AL_BIT(3)

/* AXI_Master_write_error_address_Latch register */
#define AL_ADAPTER_AXI_MSTR_WR_ERR_LO_LATCH	0x1C0

/* AXI_Master_write_error_address_high_Latch register */
#define AL_ADAPTER_AXI_MSTR_WR_ERR_HI_LATCH	0x1C4

/* AXI_Master_read_error_address_Latch register */
#define AL_ADAPTER_AXI_MSTR_RD_ERR_LO_LATCH	0x1C8

/* AXI_Master_read_error_address_high_Latch register */
#define AL_ADAPTER_AXI_MSTR_RD_ERR_HI_LATCH	0x1CC

/* AXI_Master_Timeout register */
#define AL_ADAPTER_AXI_MSTR_TO			0x1D0
#define AL_ADAPTER_AXI_MSTR_TO_WR_MASK		AL_FIELD_MASK(31, 16)
#define AL_ADAPTER_AXI_MSTR_TO_WR_SHIFT		16
#define AL_ADAPTER_AXI_MSTR_TO_RD_MASK		AL_FIELD_MASK(15, 0)
#define AL_ADAPTER_AXI_MSTR_TO_RD_SHIFT		0

/* Application_Parity_Status register */
#define AL_ADAPTER_APP_PARITY_STATUS				0x1D4
#define AL_ADAPTER_APP_PARITY_STATUS_SATA_PORT_0_TX_RAM		AL_BIT(0)
#define AL_ADAPTER_APP_PARITY_STATUS_SATA_PORT_0_RX_RAM		AL_BIT(1)
#define AL_ADAPTER_APP_PARITY_STATUS_SATA_PORT_0_FB_LOW_RAM	AL_BIT(2)
#define AL_ADAPTER_APP_PARITY_STATUS_SATA_PORT_0_FB_HIGH_RAM	AL_BIT(3)
#define AL_ADAPTER_APP_PARITY_STATUS_SATA_PORT_1_TX_RAM		AL_BIT(4)
#define AL_ADAPTER_APP_PARITY_STATUS_SATA_PORT_1_RX_RAM		AL_BIT(5)
#define AL_ADAPTER_APP_PARITY_STATUS_SATA_PORT_1_FB_LOW_RAM	AL_BIT(6)
#define AL_ADAPTER_APP_PARITY_STATUS_SATA_PORT_1_FB_HIGH_RAM	AL_BIT(7)
#define AL_ADAPTER_APP_PARITY_STATUS_SATA_PORT_2_TX_RAM		AL_BIT(8)
#define AL_ADAPTER_APP_PARITY_STATUS_SATA_PORT_2_RX_RAM		AL_BIT(9)
#define AL_ADAPTER_APP_PARITY_STATUS_SATA_PORT_2_FB_LOW_RAM	AL_BIT(10)
#define AL_ADAPTER_APP_PARITY_STATUS_SATA_PORT_2_FB_HIGH_RAM	AL_BIT(11)
#define AL_ADAPTER_APP_PARITY_STATUS_SATA_PORT_3_TX_RAM		AL_BIT(12)
#define AL_ADAPTER_APP_PARITY_STATUS_SATA_PORT_3_RX_RAM		AL_BIT(13)
#define AL_ADAPTER_APP_PARITY_STATUS_SATA_PORT_3_FB_LOW_RAM	AL_BIT(14)
#define AL_ADAPTER_APP_PARITY_STATUS_SATA_PORT_3_FB_HIGH_RAM	AL_BIT(15)
#define AL_ADAPTER_APP_PARITY_STATUS_DBG_AXI_UROB_SRAM_OUT	AL_BIT(30)
#define AL_ADAPTER_APP_PARITY_STATUS_DBG_AXI_UROB_SRAM_IN	AL_BIT(31)

/*
 * Generic control registers
 */

/* Control 0 */
#define AL_ADAPTER_GENERIC_CONTROL_0			0x1E0
/* Control 2 */
#define AL_ADAPTER_GENERIC_CONTROL_2			0x1E8
/* Control 3 */
#define AL_ADAPTER_GENERIC_CONTROL_3			0x1EC
/* Control 9 */
#define AL_ADAPTER_GENERIC_CONTROL_9			0x218
/* Control 10 */
#define AL_ADAPTER_GENERIC_CONTROL_10			0x21C
/* Control 11 */
#define AL_ADAPTER_GENERIC_CONTROL_11			0x220
/* Control 12 */
#define AL_ADAPTER_GENERIC_CONTROL_12			0x224
/* Control 13 */
#define AL_ADAPTER_GENERIC_CONTROL_13			0x228
/* Control 14 */
#define AL_ADAPTER_GENERIC_CONTROL_14			0x22C
/* Control 15 */
#define AL_ADAPTER_GENERIC_CONTROL_15			0x230
/* Control 16 */
#define AL_ADAPTER_GENERIC_CONTROL_16			0x234
/* Control 17 */
#define AL_ADAPTER_GENERIC_CONTROL_17			0x238
/* Control 18 */
#define AL_ADAPTER_GENERIC_CONTROL_18			0x23C
/* Control 19 */
#define AL_ADAPTER_GENERIC_CONTROL_19			0x240

/* Enable clock gating */
#define AL_ADAPTER_GENERIC_CONTROL_0_CLK_GATE_EN	0x01
/*
 * Mask the VF FLR to the UDMA engien due to VF_FLR bit[6]
 * When bit is set, all transactions through pci conf bar and mem bar will get timeout
 */
#define AL_ADAPTER_GENERIC_CONTROL_0_VF_UDMA_RST_MASK(vf)	AL_BIT(3 + (vf))
/* When set, all transactions through the PCI conf & mem BARs get timeout */
#define AL_ADAPTER_GENERIC_CONTROL_0_ADAPTER_DIS	0x40
/* Mask PF UDMA from FLR */
#define AL_ADAPTER_GENERIC_CONTROL_0_PF_RST_MASK	AL_BIT(8)
/* Mask VF UDMAs from FLR */
#define AL_ADAPTER_GENERIC_CONTROL_0_VF_RST_MASK(vf)	AL_BIT(9 + vf)
#define AL_ADAPTER_GENERIC_CONTROL_0_ETH_RESET_1GMAC	AL_BIT(18)
#define AL_ADAPTER_GENERIC_CONTROL_0_ETH_RESET_1GMAC_ON_FLR	AL_BIT(26)

/**
 * bit mapping of the AL_ADAPTER_GENERIC_CONTROL_0 register in eth v4 adv unit :
 * [0]	clock disable enable
 * [1]	PF status is enabled
 * [2]	parity protect disabled
 * ############# Following bits logic operates when they are set to '0'
 * ## ** vf_flr_trig is the FLR bit according to PCIE spec
 * ##    (bit 15 in control & status reg of VF config space)
 * ##    They are not auto cleared, but if vf_presetn_int[func] = 0 the bit is reset.
 * ##    hence, if bits[15:13] in these register (below) are set to 0 we achieve auto-clear.
 * ## ** see adp2func[i] effect below at bits[11:8]
 * [3]	vf_flr_trig => flr_per_func[1] == 1 && adp2func_flr[1] == 1
 * [4]	vf_flr_trig => flr_per_func[2] == 1 && adp2func_flr[2] == 1
 * [5]	vf_flr_trig => flr_per_func[3] == 1 && adp2func_flr[3] == 1
 * ##
 * [6]	pci_conf_dev_not exists, can be override from common
 * ## (*) preset_int = 0 resets the pcie cfg space registers (connected to adapter regblock reset n)
 * ##     by masking this bit an FLR wont reset the pcie config space registers
 * ##     and so FLR must be issued as a pulse over the FLR bit.
 * ## (*) pf_flr_trig activated by the FLR bit ([15] at pcie ctrl & status reg of the PR)
 * ##     this relation can change in use_all_adapter flags is unit during al_eth_common_init(...)
 * [7]	pf_flr_trig => presetn_int == 0
 * ## adp2func_flr[i] will cause :
 * ##   (*) reset of UDMA func [i]
 * ##   (*) reset of applications according to bits[29:24]
 * [8]	pf_flr_trig => flr_per_func[0] == 1 && adp2func_flr[0] == 1
 * [9]	pf_flr_trig => flr_per_func[1] == 1 && adp2func_flr[1] == 1
 * [10]	pf_flr_trig => flr_per_func[2] == 1 && adp2func_flr[2] == 1
 * [11] pf_flr_trig => flr_per_func[3] == 1 && adp2func_flr[3] == 1
 * [12]	pf_flr_trig =>  presetn_int_out == 0. reset_n inside adpt_axi_arb
 * ## See vf_flr_trig above at bits[5:3]
 * ## vf_preset_int[func] will only reset the following :
 * ## (*) VF BME bit
 * ## (*) VF FLR bit ([15] at pcie ctrl & status)
 * ## (*) VF func mask
 * ## (*) VF msix XX en
 * [13]	pf_flr_trig || vf_flr_trig[0]  =>  vf_presetn_int[0] == 0
 * [14]	pf_flr_trig || vf_flr_trig[0]  =>  vf_presetn_int[0] == 0
 * [15]	pf_flr_trig || vf_flr_trig[0]  =>  vf_presetn_int[0] == 0
 * ############# Following bits logic operates when they are set to '1'
 * ## bits [21:16] resets the application without relation to the PCIE FLR, each adapter resets
 * ## its own app, there is NO auto clear
 * [16]	app_reset (EC)
 * [17]	app_reset (EC regs)
 * [18]	app_reset (MAC)
 * [19]	RESERVED
 * [20]	app_reset (shared_cache_reset)
 * [21]	app_reset (shared_resource_reset)
 * [22]	RESERVED
 * [23]	RESERVED
 * ## use of the following bits makes sense only if we perform PCIE FLR, bit[8] is set to 0
 * ## and we must make sure that the FLR bit be cleared
 * [24]	adp2func_flr_mux_out[i] -> app_reset[i]
 * [25]	adp2func_flr_mux_out[i] -> app_reg_reset[i]
 * [26]	adp2func_flr_mux_out[i] -> mac_reset[i]
 * [27]	RESERVED
 * [28]	adp2func_flr_mux_out[0] -> shared_cache_reset
 * [29]	adp2func_flr_mux_out[0] -> shared_resource_reset
 * [30]	RESERVED
 * [31]	RESERVED
 */
/* Make sure this bit is at "0" for auto-clear of FLR bit */
#define AL_ADAPTER_GENERIC_CONTROL_0_CFG_REGS_RESET_MASK		AL_BIT(7)
#define AL_ADAPTER_GENERIC_CONTROL_0_AXI_ARB_RESET_MASK			AL_BIT(12)
#define AL_ADAPTER_GENERIC_CONTROL_0_VF_PCIE_CFG_RST_MASK(i)		AL_BIT(13 + (i))

/* This is the reset value of this reg in the HW */
#define AL_ADAPTER_GENERIC_CONTROL_0_ETH_V4_HW_RESET_VAL		0xFF000000

/* EC(0-3) reset to respective UDMA (0-3) */
#define AL_ADAPTER_GENERIC_CONTROL_0_ETH_V4_EC_RST_EN_SELF		AL_BIT(16)
/* EC reg(0-3) reset to respective UDMA (0-3) FLR */
#define AL_ADAPTER_GENERIC_CONTROL_0_ETH_V4_EC_REG_RST_EN_SELF		AL_BIT(17)
/* MAC(0-3) FLR to respective UDMA (0-3) */
#define AL_ADAPTER_GENERIC_CONTROL_0_ETH_V4_MAC_RST_EN_SELF		AL_BIT(18)
/* eth_shared_cache reset */
#define AL_ADAPTER_GENERIC_CONTROL_0_ETH_V4_SCACHE_RST_EN_SELF		AL_BIT(20)
/* eth_shared_resource reset */
#define AL_ADAPTER_GENERIC_CONTROL_0_ETH_V4_SRESOURCE_RST_EN_SELF	AL_BIT(21)
/* couple EC(0-3) FLR to respective UDMA (0-3) FLR */
#define AL_ADAPTER_GENERIC_CONTROL_0_ETH_V4_EC_RST_EN		AL_BIT(24)
/* Couple EC reg(0-3) FLR to respective UDMA (0-3) FLR */
#define AL_ADAPTER_GENERIC_CONTROL_0_ETH_V4_EC_REG_RST_EN	AL_BIT(25)
/* Couple MAC(0-3) FLR to respective UDMA (0-3) FLR */
#define AL_ADAPTER_GENERIC_CONTROL_0_ETH_V4_MAC_RST_EN		AL_BIT(26)
/* Couple eth_shared_cache FLR to UDMA 0 FLR */
#define AL_ADAPTER_GENERIC_CONTROL_0_ETH_V4_SCACHE_RST_EN	AL_BIT(28)
/* Couple eth_shared_resource FLR to UDMA 0 FLR */
#define AL_ADAPTER_GENERIC_CONTROL_0_ETH_V4_SRESOURCE_RST_EN	AL_BIT(29)

/*
 * SATA registers only
 */
/* Select 125MHz free running clock from IOFAB main PLL as SATA OOB clock
 * instead of using power management ref clock
 */
#define AL_ADAPTER_GENERIC_CONTROL_10_SATA_OOB_CLK_SEL	AL_BIT(26)
/* AXUSER selection and value per bit (1 = address, 0 = register) */
/* Rx */
#define AL_ADPTR_GEN_CTL_12_SATA_AWUSER_VAL_MASK	AL_FIELD_MASK(15, 0)
#define AL_ADPTR_GEN_CTL_12_SATA_AWUSER_VAL_SHIFT	0
#define AL_ADPTR_GEN_CTL_12_SATA_AWUSER_SEL_MASK	AL_FIELD_MASK(31, 16)
#define AL_ADPTR_GEN_CTL_12_SATA_AWUSER_SEL_SHIFT	16
/* Tx */
#define AL_ADPTR_GEN_CTL_13_SATA_ARUSER_VAL_MASK	AL_FIELD_MASK(15, 0)
#define AL_ADPTR_GEN_CTL_13_SATA_ARUSER_VAL_SHIFT	0
#define AL_ADPTR_GEN_CTL_13_SATA_ARUSER_SEL_MASK	AL_FIELD_MASK(31, 16)
#define AL_ADPTR_GEN_CTL_13_SATA_ARUSER_SEL_SHIFT	16
/* Central Target-ID enabler. If set, then each entry will be used as programmed */
#define AL_ADPTR_GEN_CTL_14_SATA_MSIX_TGTID_SEL		AL_BIT(0)
/* Allow access to store Target-ID values per entry */
#define AL_ADPTR_GEN_CTL_14_SATA_MSIX_TGTID_ACCESS_EN	AL_BIT(1)
/* Target-ID Address select */
/* Tx */
#define AL_ADPTR_GEN_CTL_14_SATA_VM_ARADDR_SEL_MASK	AL_FIELD_MASK(13, 8)
#define AL_ADPTR_GEN_CTL_14_SATA_VM_ARADDR_SEL_SHIFT	8
/* Rx */
#define AL_ADPTR_GEN_CTL_14_SATA_VM_AWADDR_SEL_MASK	AL_FIELD_MASK(21, 16)
#define AL_ADPTR_GEN_CTL_14_SATA_VM_AWADDR_SEL_SHIFT	16
/* Address Value */
/* Rx */
#define AL_ADPTR_GEN_CTL_15_SATA_VM_AWDDR_HI	AL_FIELD_MASK(31, 0)
/* Tx */
#define AL_ADPTR_GEN_CTL_16_SATA_VM_ARDDR_HI	AL_FIELD_MASK(31, 0)

/*
 * ROB registers
 */
/* Read ROB Enable, when disabled the read ROB is bypassed */
#define AL_ADPTR_GEN_CTL_19_READ_ROB_EN			AL_BIT(0)
/* Read force in-order of every read transaction */
#define AL_ADPTR_GEN_CTL_19_READ_ROB_FORCE_INORDER	AL_BIT(1)
/* Read software reset */
#define AL_ADPTR_GEN_CTL_19_READ_ROB_SW_RESET		AL_BIT(15)
/* Write ROB Enable, when disabled the Write ROB is bypassed */
#define AL_ADPTR_GEN_CTL_19_WRITE_ROB_EN		AL_BIT(16)
/* Write force in-order of every write transaction */
#define AL_ADPTR_GEN_CTL_19_WRITE_ROB_FORCE_INORDER	AL_BIT(17)
/* Write software reset */
#define AL_ADPTR_GEN_CTL_19_WRITE_ROB_SW_RESET		AL_BIT(31)

#define AL_ADPTR_VF_BASE(vf_num)			(0x1000 + ((vf_num) * 0x1000))
#define AL_ADPTR_VF_CTRL_STATUS(vf_num)			(AL_ADPTR_VF_BASE(vf_num) + 0x48)
#define AL_ADPTR_VF_CTRL_STATUS_FLR			AL_BIT(15)

#ifdef __cplusplus
}
#endif

#endif
