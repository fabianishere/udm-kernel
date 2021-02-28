/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#ifndef __AL_HAL_PCIE_REGS_FIELDS_H__
#define __AL_HAL_PCIE_REGS_FIELDS_H__

/* When set holds the assertion of reset to serdes and to pcie core in case of link down event */
#define PCIE_APP_LINK_DOWN_RESET_DELAY_CNTL_RST_ASSERT_HOLD		AL_BIT(31)
/* When set holds the deassertion of reset to serdes and to pcie core till this bit is clear */
#define PCIE_APP_LINK_DOWN_RESET_DELAY_CNTL_RST_DEASSERT_HOLD		AL_BIT(30)
/*
 * Set the minimum delay between link down event and the assertion of reset to serdes and to pcie
 * core [Core clock - 8ns]
 */
#define PCIE_APP_LINK_DOWN_RESET_DELAY_CNTL_RST_ASSERT_MIN_DELAY_MASK	AL_FIELD_MASK(29, 0)
#define PCIE_APP_LINK_DOWN_RESET_DELAY_CNTL_RST_ASSERT_MIN_DELAY_SHIFT	0

/* when set causes reset assertion to serdes and to pcie core */
#define PCIE_AXI_LINK_DOWN_RESET_EXTEND_CNTL_RST_ASSERT			AL_BIT(31)
/*
 * set the duration of the reset assertion to the serdes and to pcie core in case of link down
 * event [SB clock]
 */
#define PCIE_AXI_LINK_DOWN_RESET_EXTEND_CNTL_RST_ASSERT_DURATION_MASK	AL_FIELD_MASK(30, 0)
#define PCIE_AXI_LINK_DOWN_RESET_EXTEND_CNTL_RST_ASSERT_DURATION_SHIFT	0

#define PCIE_AXI_MISC_PCIE_GLOBAL_CONF_DEV_TYPE_EP	0
#define PCIE_AXI_MISC_PCIE_GLOBAL_CONF_DEV_TYPE_RC	4

#define PCIE_AXI_CFG_QUALIFIED_ATU_ERROR_WITH_FLT_Q_HV	AL_BIT(29)

#define PCIE_PORT_GEN2_CTRL_DIRECT_SPEED_CHANGE		AL_BIT(17)
#define PCIE_PORT_GEN2_CTRL_TX_SWING_LOW_SHIFT		18
#define PCIE_PORT_GEN2_CTRL_TX_COMPLIANCE_RCV_SHIFT	19
#define PCIE_PORT_GEN2_CTRL_DEEMPHASIS_SET_SHIFT	20
#define PCIE_PORT_GEN2_CTRL_NUM_OF_LANES_MASK		AL_FIELD_MASK(12, 8)
#define PCIE_PORT_GEN2_CTRL_NUM_OF_LANES_SHIFT		8

#define PCIE_PORT_GEN3_CTRL_EQ_PHASE_2_3_DISABLE_SHIFT		9
#define PCIE_PORT_GEN3_CTRL_EQ_EIEOS_COUNT_RESET_DISABLE_SHIFT	10
#define PCIE_PORT_GEN3_CTRL_EQ_REDO_BYPASS_SHIFT				11
#define PCIE_PORT_GEN3_CTRL_EQ_DISABLE_SHIFT			16

#define PCIE_PORT_GEN3_EQ_LF_SHIFT			0
#define PCIE_PORT_GEN3_EQ_LF_MASK			0x3f
#define PCIE_PORT_GEN3_EQ_FS_SHIFT			6
#define PCIE_PORT_GEN3_EQ_FS_MASK			(0x3f << PCIE_PORT_GEN3_EQ_FS_SHIFT)

#define PCIE_PORT_GEN3_EQ_CTRL_PSET_REQ_VEC_SHIFT	8
#define PCIE_PORT_GEN3_EQ_CTRL_PSET_REQ_VEC_MASK	AL_FIELD_MASK(23, 8)

#define PCIE_PORT_GEN3_EQ_CTRL_FB_MODE_SHIFT		0
#define PCIE_PORT_GEN3_EQ_CTRL_FB_MODE_MASK		AL_FIELD_MASK(3, 0)

#define PCIE_PORT_GEN3_EQ_FMDC_MAX_POST_CUSROR_DELTA_SHIFT	14
#define PCIE_PORT_GEN3_EQ_FMDC_MAX_POST_CUSROR_DELTA_MASK	AL_FIELD_MASK(17, 14)
#define PCIE_PORT_GEN3_EQ_FMDC_MAX_PRE_CUSROR_DELTA_SHIFT	10
#define PCIE_PORT_GEN3_EQ_FMDC_MAX_PRE_CUSROR_DELTA_MASK	AL_FIELD_MASK(13, 10)
#define PCIE_PORT_GEN3_EQ_FMDC_N_EVALS_SHIFT			5
#define PCIE_PORT_GEN3_EQ_FMDC_N_EVALS_MASK			AL_FIELD_MASK(9, 5)
#define PCIE_PORT_GEN3_EQ_FMDC_T_MIN_PHAS_SHIFT			0
#define PCIE_PORT_GEN3_EQ_FMDC_T_MIN_PHAS_MASK			AL_FIELD_MASK(4, 0)

#define PCIE_PORT_LINK_CTRL_LB_EN_SHIFT			2
#define PCIE_PORT_LINK_CTRL_FAST_LINK_EN_SHIFT		7
#define PCIE_PORT_LINK_CTRL_LINK_CAPABLE_MASK		AL_FIELD_MASK(21, 16)
#define PCIE_PORT_LINK_CTRL_LINK_CAPABLE_SHIFT		16

#define PCIE_PORT_PIPE_LOOPBACK_CTRL_PIPE_LB_EN_SHIFT	31

#define PCIE_PORT_AXI_SLAVE_ERR_RESP_ALL_MAPPING_SHIFT	0

/** timer_ctrl_max_func_num register
 * Max physical function number (for example: 0 for 1PF, 3 for 4PFs)
 */
#define PCIE_PORT_GEN3_MAX_FUNC_NUM			AL_FIELD_MASK(7, 0)

/* filter_mask_reg_1 register */
/**
 * SKP Interval Value.
 * The number of symbol times to wait between transmitting SKP ordered sets
 */
#define PCIE_FLT_MASK_SKP_INT_VAL_MASK			AL_FIELD_MASK(10, 0)

/*
 * 0: Treat Function MisMatched TLPs as UR
 * 1: Treat Function MisMatched TLPs as Supported
 */
#define CX_FLT_MASK_UR_FUNC_MISMATCH			AL_BIT(16)

/*
 * 0: Treat CFG type1 TLPs as UR for EP; Supported for RC
 * 1: Treat CFG type1 TLPs as Supported for EP; UR for RC
 */
#define CX_FLT_MASK_CFG_TYPE1_RE_AS_UR			AL_BIT(19)

/*
 * 0: Enforce requester id match for received CPL TLPs.
 *    A violation results in cpl_abort, and possibly AER of unexp_cpl_err,
 *    cpl_rcvd_ur, cpl_rcvd_ca
 * 1: Mask requester id match for received CPL TLPs
 */
#define CX_FLT_MASK_CPL_REQID_MATCH			AL_BIT(22)

/*
 * 0: Enforce function match for received CPL TLPs.
 *    A violation results in cpl_abort, and possibly AER of unexp_cpl_err,
 *    cpl_rcvd_ur, cpl_rcvd_ca
 * 1: Mask function match for received CPL TLPs
 */
#define CX_FLT_MASK_CPL_FUNC_MATCH			AL_BIT(23)

/* vc0_posted_rcv_q_ctrl register */
#define RADM_PQ_HCRD_VC0_MASK				AL_FIELD_MASK(19, 12)
#define RADM_PQ_HCRD_VC0_SHIFT				12

/* vc0_non_posted_rcv_q_ctrl register */
#define RADM_NPQ_HCRD_VC0_MASK				AL_FIELD_MASK(19, 12)
#define RADM_NPQ_HCRD_VC0_SHIFT				12

/* vc0_comp_rcv_q_ctrl register */
#define RADM_CPLQ_HCRD_VC0_MASK				AL_FIELD_MASK(19, 12)
#define RADM_CPLQ_HCRD_VC0_SHIFT			12

/**** iATU, index Register ****/
#define PCIE_IATU_INDEX_REGION_DIR_MASK			AL_BIT(31)
#define PCIE_IATU_INDEX_REGION_DIR_SHIFT		31
#define PCIE_IATU_INDEX_REGION_INDEX_MASK		AL_FIELD_MASK(4, 0)
#define PCIE_IATU_INDEX_REGION_INDEX_SHIFT		0

/**** iATU, Control Register 1 ****/
/**
 * When the Address and BAR matching logic in the core indicate that a MEM-I/O
 * transaction matches a BAR in the function corresponding to this value, then
 * address translation proceeds. This check is only performed if the "Function
 * Number Match Enable" bit of the "iATU Control 2 Register" is set
 */
#define PCIE_IATU_CR1_FUNC_NUM_MASK			AL_FIELD_MASK(24, 20)
#define PCIE_IATU_CR1_FUNC_NUM_SHIFT			20
#define PCIE_IATU_CR1_ATTER_MASK			AL_FIELD_MASK(10, 9)
#define PCIE_IATU_CR1_ATTER_SHIFT			9
#define PCIE_IATU_CR1_TYPE_MASK				AL_FIELD_MASK(4, 0)
#define PCIE_IATU_CR1_TYPE_SHIFT			0

/**** iATU, Control Register 2 ****/
/** For outbound regions, the Function Number Translation Bypass mode enables
 *  taking the function number of the translated TLP from the PCIe core
 *  interface and not from the "Function Number" field of CR1.
 *  For inbound regions, this bit should be asserted when physical function
 *  match mode needs to be enabled
 */
#define PCIE_IATU_CR2_ENABLE						AL_BIT(31)
#define PCIE_IATU_CR2_MATCH_MODE					AL_BIT(30)
#define PCIE_IATU_CR2_INVERT_MODE					AL_BIT(29)
#define PCIE_IATU_CR2_CFG_SHIFT_MODE					AL_BIT(28)
#define PCIE_IATU_CR2_RESPONSE_CODE_MASK				AL_FIELD_MASK(25, 24)
#define PCIE_IATU_CR2_RESPONSE_CODE_SHIFT				24
#define PCIE_IATU_CR2_MSG_CODE_MATCH_EN					AL_BIT(21)
#define PCIE_IATU_CR2_FUNC_NUM_TRANS_BYPASS_FUNC_MATCH_ENABLE_MASK	AL_BIT(19)
#define PCIE_IATU_CR2_FUNC_NUM_TRANS_BYPASS_FUNC_MATCH_ENABLE_SHIFT	19
#define PCIE_IATU_CR2_ATTR_MATCH_EN					AL_BIT(16)
#define PCIE_IATU_CR2_MSG_BAR_NUM_MASK					AL_FIELD_MASK(10, 8)
#define PCIE_IATU_CR2_MSG_BAR_NUM_SHIFT					8
#define PCIE_IATU_CR2_MSG_CODE_MASK					AL_FIELD_MASK(7, 0)
#define PCIE_IATU_CR2_MSG_CODE_SHIFT					0

/* pcie_dev_ctrl_status register */
#define PCIE_PORT_DEV_CTRL_STATUS_CORR_ERR_REPORT_EN	AL_BIT(0)
#define PCIE_PORT_DEV_CTRL_STATUS_NON_FTL_ERR_REPORT_EN	AL_BIT(1)
#define PCIE_PORT_DEV_CTRL_STATUS_FTL_ERR_REPORT_EN	AL_BIT(2)
#define PCIE_PORT_DEV_CTRL_STATUS_UNSUP_REQ_REPORT_EN	AL_BIT(3)

#define PCIE_PORT_DEV_CTRL_STATUS_MPS_MASK		AL_FIELD_MASK(7, 5)
#define PCIE_PORT_DEV_CTRL_STATUS_MPS_SHIFT		5
#define PCIE_PORT_DEV_CTRL_STATUS_MPS_VAL_256		(1 << PCIE_PORT_DEV_CTRL_STATUS_MPS_SHIFT)

#define PCIE_PORT_DEV_CTRL_STATUS_MRRS_MASK		AL_FIELD_MASK(14, 12)
#define PCIE_PORT_DEV_CTRL_STATUS_MRRS_SHIFT		12
#define PCIE_PORT_DEV_CTRL_STATUS_MRRS_VAL_128		(0 << PCIE_PORT_DEV_CTRL_STATUS_MRRS_SHIFT)
#define PCIE_PORT_DEV_CTRL_STATUS_MRRS_VAL_256		(1 << PCIE_PORT_DEV_CTRL_STATUS_MRRS_SHIFT)
#define PCIE_PORT_DEV_CTRL_STATUS_MRRS_VAL_512          (2 << PCIE_PORT_DEV_CTRL_STATUS_MRRS_SHIFT)

/******************************************************************************
 * AER registers
 ******************************************************************************/
/* PCI Express Extended Capability ID */
#define PCIE_AER_CAP_ID_MASK			AL_FIELD_MASK(15, 0)
#define PCIE_AER_CAP_ID_SHIFT			0
#define PCIE_AER_CAP_ID_VAL			1
/* Capability Version */
#define PCIE_AER_CAP_VER_MASK			AL_FIELD_MASK(19, 16)
#define PCIE_AER_CAP_VER_SHIFT			16
#define PCIE_AER_CAP_VER_VAL			2

/* First Error Pointer */
#define PCIE_AER_CTRL_STAT_FIRST_ERR_PTR_MASK		AL_FIELD_MASK(4, 0)
#define PCIE_AER_CTRL_STAT_FIRST_ERR_PTR_SHIFT		0
/* ECRC Generation Capability */
#define PCIE_AER_CTRL_STAT_ECRC_GEN_SUPPORTED		AL_BIT(5)
/* ECRC Generation Enable */
#define PCIE_AER_CTRL_STAT_ECRC_GEN_EN			AL_BIT(6)
/* ECRC Check Capable */
#define PCIE_AER_CTRL_STAT_ECRC_CHK_SUPPORTED		AL_BIT(7)
/* ECRC Check Enable */
#define PCIE_AER_CTRL_STAT_ECRC_CHK_EN			AL_BIT(8)

/* Correctable Error Reporting Enable */
#define PCIE_AER_ROOT_ERR_CMD_CORR_ERR_RPRT_EN		AL_BIT(0)
/* Non-Fatal Error Reporting Enable */
#define PCIE_AER_ROOT_ERR_CMD_NON_FTL_ERR_RPRT_EN	AL_BIT(1)
/* Fatal Error Reporting Enable */
#define PCIE_AER_ROOT_ERR_CMD_FTL_ERR_RPRT_EN		AL_BIT(2)

/* ERR_COR Received */
#define PCIE_AER_ROOT_ERR_STAT_CORR_ERR			AL_BIT(0)
/* Multiple ERR_COR Received */
#define PCIE_AER_ROOT_ERR_STAT_CORR_ERR_MULTI		AL_BIT(1)
/* ERR_FATAL/NONFATAL Received */
#define PCIE_AER_ROOT_ERR_STAT_FTL_NON_FTL_ERR		AL_BIT(2)
/* Multiple ERR_FATAL/NONFATAL Received */
#define PCIE_AER_ROOT_ERR_STAT_FTL_NON_FTL_ERR_MULTI	AL_BIT(3)
/* First Uncorrectable Fatal */
#define PCIE_AER_ROOT_ERR_STAT_FIRST_UNCORR_FTL		AL_BIT(4)
/* Non-Fatal Error Messages Received */
#define PCIE_AER_ROOT_ERR_STAT_NON_FTL_RCVD		AL_BIT(5)
/* Fatal Error Messages Received */
#define PCIE_AER_ROOT_ERR_STAT_FTL_RCVD			AL_BIT(6)
/* Advanced Error Interrupt Message Number */
#define PCIE_AER_ROOT_ERR_STAT_ERR_INT_MSG_NUM_MASK	AL_FIELD_MASK(31, 27)
#define PCIE_AER_ROOT_ERR_STAT_ERR_INT_MSG_NUM_SHIFT	27

/* ERR_COR Source Identification */
#define PCIE_AER_SRC_ID_CORR_ERR_MASK			AL_FIELD_MASK(15, 0)
#define PCIE_AER_SRC_ID_CORR_ERR_SHIFT			0
/* ERR_FATAL/NONFATAL Source Identification */
#define PCIE_AER_SRC_ID_CORR_ERR_FTL_NON_FTL_MASK	AL_FIELD_MASK(31, 16)
#define PCIE_AER_SRC_ID_CORR_ERR_FTL_NON_FTL_SHIFT	16
/* Unsupported request bit in Uncorrectable error status/severity/mask registers */
#define PCIE_AER_UNCOR_UR				AL_BIT(20)
#define PCIE_AER_UNCOR_ERR_STA_OFFSET			0x4
/* Non fatal error in Uncorrectable error status/mask registers */
#define PCIE_AER_COR_STA_NON_FATAL			AL_BIT(13)
#define PCIE_AER_COR_ERR_STA_OFFSET			0x10

/* AER message */
#define PCIE_AER_MSG_REQID_MASK				AL_FIELD_MASK(31, 16)
#define PCIE_AER_MSG_REQID_SHIFT			16
#define PCIE_AER_MSG_TYPE_MASK				AL_FIELD_MASK(15, 8)
#define PCIE_AER_MSG_TYPE_SHIFT				8
#define PCIE_AER_MSG_RESERVED				AL_FIELD_MASK(7, 1)
#define PCIE_AER_MSG_VALID				AL_BIT(0)
/* AER message ack */
#define PCIE_AER_MSG_ACK				AL_BIT(0)
/* AER errors definitions */
#define AL_PCIE_AER_TYPE_CORR				(0x30)
#define AL_PCIE_AER_TYPE_NON_FATAL			(0x31)
#define AL_PCIE_AER_TYPE_FATAL				(0x33)
/* Requester ID Bus */
#define AL_PCIE_REQID_BUS_NUM_SHIFT			(8)

/******************************************************************************
 * TPH registers
 ******************************************************************************/
#define PCIE_TPH_NEXT_POINTER				AL_FIELD_MASK(31, 20)

/******************************************************************************
 * Config Header registers
 ******************************************************************************/
/**
 * see BIST_HEADER_TYPE_LATENCY_CACHE_LINE_SIZE_REG in core spec
 * Note: valid only for EP mode
 */
#define PCIE_BIST_HEADER_TYPE_BASE		0xc
#define PCIE_BIST_HEADER_TYPE_MULTI_FUNC_MASK	AL_BIT(23)

/******************************************************************************
 * Parity error registers
 ******************************************************************************/
/* status_core register */
/* v1/2 defines */
#define PCIE_W_V1_V2_CORE_PARITY_CORE_U_RAM_1P_SOTBUF		AL_BIT(0)
#define PCIE_W_V1_V2_CORE_PARITY_CORE_U0_RAM_RADM_QBUFFER	AL_BIT(1)
#define PCIE_W_V1_V2_CORE_PARITY_CORE_U3_QBUFFER_0		AL_BIT(2)
#define PCIE_W_V1_V2_CORE_PARITY_CORE_U3_QBUFFER_1		AL_BIT(3)
#define PCIE_W_V1_V2_CORE_PARITY_CORE_U9_DECOMP			AL_BIT(4)
#define PCIE_W_V1_V2_CORE_PARITY_CORE_U8_RAM2P			AL_BIT(5)
#define PCIE_W_V1_V2_CORE_PARITY_CORE_U7_RAM2P			AL_BIT(6)
#define PCIE_W_V1_V2_CORE_PARITY_CORE_U6_RAM2P			AL_BIT(7)
#define PCIE_W_V1_V2_CORE_PARITY_CORE_U11_RAM2P			AL_BIT(8)
#define PCIE_W_V1_V2_CORE_PARITY_CORE_U1_RAM2P			AL_BIT(9)
#define PCIE_W_V1_V2_CORE_PARITY_CORE_U0_RAM2P			AL_BIT(10)
#define PCIE_W_V1_V2_CORE_PARITY_CORE_U0_RBUF			AL_BIT(11)
#define PCIE_W_V1_V2_CORE_PARITY_CORE_U3_QBUFFER_2		AL_BIT(12)
/* v3 defines */
#define PCIE_W_V3_CORE_PARITY_CORE_RAM_1P_RBUF			AL_BIT(0)
#define PCIE_W_V3_CORE_PARITY_CORE_RAM_2P_SOTBUF		AL_BIT(1)
#define PCIE_W_V3_CORE_PARITY_CORE_U0_RAM_RADM_QBUFFER_HDR	AL_BIT(2)
#define PCIE_W_V3_CORE_PARITY_CORE_U3_RAM_RADM_QBUFFER_DATA_0	AL_BIT(3)
#define PCIE_W_V3_CORE_PARITY_CORE_U3_RAM_RADM_QBUFFER_DATA_1	AL_BIT(4)
#define PCIE_W_V3_CORE_PARITY_CORE_U10_RAM2P_0			AL_BIT(5)
#define PCIE_W_V3_CORE_PARITY_CORE_U10_RAM2P_1			AL_BIT(6)
#define PCIE_W_V3_CORE_PARITY_CORE_U8_RAM2P			AL_BIT(7)
#define PCIE_W_V3_CORE_PARITY_CORE_U7_RAM2P			AL_BIT(8)
#define PCIE_W_V3_CORE_PARITY_CORE_U6_RAM			AL_BIT(9)
#define PCIE_W_V3_CORE_PARITY_CORE_U11_RAM2P			AL_BIT(10)
#define PCIE_W_V3_CORE_PARITY_CORE_U1_RAM2P			AL_BIT(11)
#define PCIE_W_V3_CORE_PARITY_CORE_U0_RAM2P			AL_BIT(12)

/*status_axi register */
#define PCIE_AXI_PARITY_AXI_U5_RAM2P			AL_BIT(0)
#define PCIE_AXI_PARITY_AXI_U4_RAM2P			AL_BIT(1)
#define PCIE_AXI_PARITY_AXI_U3_RAM2P			AL_BIT(2)
/* v1/2 defines */
#define PCIE_V1_V2_AXI_PARITY_AXI_U12_RAM2P		AL_BIT(3)
#define PCIE_V1_V2_AXI_PARITY_AXI_U2_RAM2P		AL_BIT(4)
#define PCIE_V1_V2_AXI_PARITY_AXI_U10_RAM2P		AL_BIT(5)
/* v3 defines */
#define PCIE_V3_AXI_PARITY_AXI_U2_RAM2P			AL_BIT(3)

/******************************************************************************
 * SRIS KP counters default values
 ******************************************************************************/
#define PCIE_SRIS_KP_COUNTER_GEN3_DEFAULT_VAL	(0x24)
#define PCIE_SRIS_KP_COUNTER_GEN21_DEFAULT_VAL	(0x4B)

/******************************************************************************
 * Interrupt forwarding
 ******************************************************************************/
#define PCIE_INTERRUPT_FORWARDING_ETH_0_MASK AL_FIELD_MASK(16, 0)

#define PCIE_INTERRUPT_FORWARDING_ETH_1_MASK_A AL_FIELD_MASK(31, 17)
#define PCIE_INTERRUPT_FORWARDING_ETH_1_MASK_B AL_FIELD_MASK(1, 0)

#define PCIE_INTERRUPT_FORWARDING_ETH_2_MASK AL_FIELD_MASK(18, 2)

#define PCIE_INTERRUPT_FORWARDING_ETH_3_MASK_A AL_FIELD_MASK(31, 19)
#define PCIE_INTERRUPT_FORWARDING_ETH_3_MASK_B AL_FIELD_MASK(3, 0)

#define PCIE_INTERRUPT_FORWARDING_ETH_4_MASK AL_FIELD_MASK(20, 4)

#define PCIE_INTERRUPT_FORWARDING_ETH_5_MASK_A AL_FIELD_MASK(31, 21)
#define PCIE_INTERRUPT_FORWARDING_ETH_5_MASK_B AL_FIELD_MASK(5, 0)

#endif
