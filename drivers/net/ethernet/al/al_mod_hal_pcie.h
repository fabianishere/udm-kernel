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

/**
 * @defgroup grouppcie PCI Express Controller
 *  @{
 * @section overview Overview
 * This header file provide API for the HAL driver of the pcie port, the driver
 * provides the following functionalities:
 * - Port initialization
 * - Link operation
 * - Interrupts transactions generation (Endpoint mode).
 * - Configuration Access management functions
 * - Internal Translation Unit programming
 *
 * This API does not provide the following:
 * - PCIe transactions generation and reception (except interrupts as mentioned
 *   above) as this functionality is done by the port without need for sw
 *   intervention.
 * - Configuration Access: those transactions are generated automatically by
 *   the port (ECAM or ATU mode) when the CPU issues memory transaction
 *   through the fabric toward the PCIe port. This API provides management
 *   function for controlling the Configuration Access type and bus destination
 * - Interrupt Handling.
 * - Message Generation: common used messages are automatically generated, also,
 *   the ATU generic mechanism for generating various kind of messages.
 * - PCIe Port Management: both link and port power management features can be
 *   managed using the PCI/PCIe standard power management and PCIe capabilities
 *   registers.
 * - PCIe link and protocol error handling: the feature can be managed using
 *   the Advanced Error Handling PCIe capability registers.
 *
 * @section flows Software Flows
 * @subsection init Initialization
 *   - allocation and set zeros al_mod_pcie_port and al_mod_pcie_pf structures handles
 *   - call al_mod_pcie_port_handle_init() with pointer to the allocated
 *     al_mod_pcie_port handle, address of the port internal registers space, and
 *     port id.
 *   - call al_mod_pcie_pf_handle_init() with pointer to the al_mod_pcie_port handle
 *     and pf_number.
 *   - set the port mode, End-Point or Root-Compex (default).
 *   - set number of lanes connected to the controller.
 *   - enable the controller using the al_mod_pcie_port_enable(). note that this
 *     function expect the virtual address of the PBS Functional Registers.
 *   - wait for 2000 South-bridge cycles.
 *   - prepare al_mod_pcie_port_config_params and al_mod_pcie_pf_config_params
 *     structures depending on chip, board and system configuration.
 *     for example, when using the port as root complex, the operating_mode
 *     field should be set to AL_PCIE_OPERATING_MODE_RC. In this example we
 *     prepare the following configuration:
 *     For port configuration
 *     - Root Complex mode
 *     - Set the Max Link Speed to Gen2
 *     - Set the max lanes width to 2 (x2)
 *     - Enable Snoops to support I/O Hardware cache coherency
 *     - Enable pcie core RAM parity
 *     - Enable pcie core AXI parity
 *     - Keep transaction layer default credits
 *     For pf configuration
 *     - No EP parameters
 *     - No SR-IOV parameters
 *     so the structures we prepare:
 *     @code
 *     - struct al_mod_pcie_link_params link_params = {
 *		AL_PCIE_LINK_SPEED_GEN2,
 *		AL_PCIE_MPS_DEFAULT};
 *
 *     - struct al_mod_pcie_port_config_params config_params = {
 *		&link_params,
 *		AL_TRUE, // enable Snoop for inbound memory transactions
 *		AL_TRUE, // enable pcie port RAM parity
 *		AL_TRUE, // enable pcie port AXI parity
 *		NULL, // use default latency/replay timers
 *		NULL, // use default gen2 pipe params
 *		NULL, // gen3_params not needed when max speed set to Gen2
 *		NULL, // don't change TL credits
 *		NULL, // end point params not needed
 *		AL_FALSE, //no fast link
 *		AL_FALSE};	//return 0xFFFFFFFF for read transactions with
 *				//pci target error
 *	@endcode
 *	- now call al_mod_pcie_port_config() with pcie_port and port_config_params
 * @subsection link-init Link Initialization
 *  - once the port configured, we can start PCIe link:
 *  - call al_mod_pcie_link_start()
 *  - call al_mod_pcie_link_up_wait()
 *  - allocate al_mod_pcie_link_status struct and call al_mod_pcie_link_status() and
 *    check the link is established.
 *
 *  @subsection  cap Configuration Access Preparation
 *  - Once the link is established, we can prepare the port for pci
 *  configuration access, this stage requires system knowledge about the PCI
 *  buses enumeration. For example, if 5 buses were discovered on previously
 *  scanned root complex port, then we should start enumeration from bus 5 (PCI
 *  secondary bus), the sub-ordinary bus will be temporarily set to maximum
 *  value (255) until the scan process under this bus is finished, then it will
 *  updated to the maximum bus value found. So we use the following sequence:
 *  - call al_mod_pcie_secondary_bus_set() with sec-bus = 5
 *  - call al_mod_pcie_subordinary_bus_set() with sub-bus = 255
 *
 *  @subsection cfg Configuration (Cfg) Access Generation
 *  - we assume using ECAM method, in this method, the software issues pcie Cfg
 *  access by accessing the ECAM memory space of the pcie port. For example, to
 *  issue 4 byte Cfg Read from bus B, Device D, Function F and register R, the
 *  software issues 4 byte read access to the following physical address
 *  ECAM base address of the port + (B << 20) + (D << 15) + (F << 12) + R.
 *  But, as the default size of the ECAM address space is less than
 *  needed full range (256MB), we modify the target_bus value prior to Cfg
 *  access in order make the port generate Cfg access with bus value set to the
 *  value of the target_bus rather than bits 27:20 of the physical address.
 *  - call al_mod_pcie_target_bus_set() with target_bus set to the required bus of
 *   the next Cfg access to be issued, mask_target_bus will be set to 0xff.
 *   no need to call that function if the next Cfg access bus equals to the last
 *   value set to target_bus.
 *
 *      @file  al_mod_hal_pcie.h
 *      @brief HAL Driver Header for the Annapurna Labs PCI Express port.
 */

#ifndef _AL_HAL_PCIE_H_
#define _AL_HAL_PCIE_H_

#include "al_mod_hal_common.h"
#include "al_mod_hal_pcie_regs.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/********************************* Constants **********************************/
/******************************************************************************/

/**
 * PCIe Core revision IDs:
 *     ID_1: Alpine V1
 *     ID_2: Alpine V2 x4
 *     ID_3: Alpine V2 x8
 *     ID_4: Alpine V3 x16/x8/x4
 *     ID_5: Alpine V4 x8
 */
#define AL_PCIE_REV_ID_1			1
#define AL_PCIE_REV_ID_2			2
#define AL_PCIE_REV_ID_3			3
#define AL_PCIE_REV_ID_4			4
#define AL_PCIE_REV_ID_5			5
#define AL_PCIE_REV_ID_MAX			AL_PCIE_REV_ID_5

/** Number of extended registers */
#define AL_PCIE_EX_REGS_NUM				40

/** Number of cfg space registers */
#define AL_PCIE_CFG_SPACE_REGS_NUM		(0x1000 / 4)

/*******************************************************************************
 * The inbound flow control for headers is programmable per P, NP and CPL
 * transactions types. The following parameters define the total number of
 * available header flow controls for all types.
 ******************************************************************************/
/** Inbound header credits sum - rev1/2 */
#define AL_PCIE_REV_1_2_IB_HCRD_SUM			97
/** Inbound header credits sum - rev3 */
#define AL_PCIE_REV3_IB_HCRD_SUM			259
/** Inbound header credits sum - rev4 */
#define AL_PCIE_REV4_X4_IB_HCRD_SUM			147
#define AL_PCIE_REV4_X8_IB_HCRD_SUM			419
#define AL_PCIE_REV4_X16_IB_HCRD_SUM			266
/** Inbound header credits sum - rev5 */
#define AL_PCIE_REV5_X8_IB_HCRD_SUM			266


/*******************************************************************************
 * PCIe AER uncorrectable error bits
 * To be used with the following functions:
 * - al_mod_pcie_aer_config
 * - al_mod_pcie_aer_uncorr_get_and_clear
 ******************************************************************************/
/** Data Link Protocol Error */
#define AL_PCIE_AER_UNCORR_DLP_ERR			AL_BIT(4)
/** Poisoned TLP */
#define AL_PCIE_AER_UNCORR_POISIONED_TLP		AL_BIT(12)
/** Flow Control Protocol Error */
#define AL_PCIE_AER_UNCORR_FLOW_CTRL_ERR		AL_BIT(13)
/** Completion Timeout */
#define AL_PCIE_AER_UNCORR_COMPL_TO			AL_BIT(14)
/** Completer Abort */
#define AL_PCIE_AER_UNCORR_COMPL_ABT			AL_BIT(15)
/** Unexpected Completion */
#define AL_PCIE_AER_UNCORR_UNEXPCTED_COMPL		AL_BIT(16)
/** Receiver Overflow */
#define AL_PCIE_AER_UNCORR_RCV_OVRFLW			AL_BIT(17)
/** Malformed TLP */
#define AL_PCIE_AER_UNCORR_MLFRM_TLP			AL_BIT(18)
/** ECRC Error */
#define AL_PCIE_AER_UNCORR_ECRC_ERR			AL_BIT(19)
/** Unsupported Request Error */
#define AL_PCIE_AER_UNCORR_UNSUPRT_REQ_ERR		AL_BIT(20)
/** Uncorrectable Internal Error */
#define AL_PCIE_AER_UNCORR_INT_ERR			AL_BIT(22)
/** AtomicOp Egress Blocked */
#define AL_PCIE_AER_UNCORR_ATOMIC_EGRESS_BLK		AL_BIT(24)

/*******************************************************************************
 * PCIe AER correctable error bits
 * To be used with the following functions:
 * - al_mod_pcie_aer_config
 * - al_mod_pcie_aer_corr_get_and_clear
 ******************************************************************************/
/** Receiver Error */
#define AL_PCIE_AER_CORR_RCV_ERR			AL_BIT(0)
/** Bad TLP */
#define AL_PCIE_AER_CORR_BAD_TLP			AL_BIT(6)
/** Bad DLLP */
#define AL_PCIE_AER_CORR_BAD_DLLP			AL_BIT(7)
/** REPLAY_NUM Rollover */
#define AL_PCIE_AER_CORR_RPLY_NUM_ROLL_OVR		AL_BIT(8)
/** Replay Timer Timeout */
#define AL_PCIE_AER_CORR_RPLY_TMR_TO			AL_BIT(12)
/** Advisory Non-Fatal Error */
#define AL_PCIE_AER_CORR_ADVISORY_NON_FTL_ERR		AL_BIT(13)
/** Corrected Internal Error */
#define AL_PCIE_AER_CORR_INT_ERR			AL_BIT(14)

/** The AER erroneous TLP header length [num DWORDs] */
#define AL_PCIE_AER_ERR_TLP_HDR_NUM_DWORDS		4

/*******************************************************************************
 * PCIe ATU's (Outbound and Inbound)
 ******************************************************************************/
#define AL_PCIE_REV_1_2_ATUS_IB_NUM			12
#define AL_PCIE_REV_3_ATUS_IB_NUM			24
#define AL_PCIE_REV_4_X4_ATUS_IB_NUM			24
#define AL_PCIE_REV4_X8_X16_ATUS_IB_NUM			32

#define AL_PCIE_REV_1_2_ATUS_OB_NUM			12
#define AL_PCIE_REV_3_4_ATUS_OB_NUM			16

/******************************************************************************/
/************************* Data Structures and Types **************************/
/******************************************************************************/

/**
 * al_mod_pcie_ib_hcrd_config: data structure internally used in order to config
 * inbound posted/non-posted parameters.
 * Note: this is a private member in pcie_port handle and MUST NOT be modified
 *       by the user.
 */
struct al_mod_pcie_ib_hcrd_config {
	/* Internally used - see 'al_mod_pcie_ib_hcrd_os_ob_reads_config' */
	unsigned int	nof_np_hdr;

	/* Internally used - see 'al_mod_pcie_ib_hcrd_os_ob_reads_config' */
	unsigned int	nof_p_hdr;

	/* Internally used - see 'al_mod_pcie_ib_hcrd_os_ob_reads_config' */
	al_mod_bool crdt_update_required;
};

/* The Max Payload Size. Measured in bytes.
 *   DEFAULT: do not change the current MPS
 */
enum al_mod_pcie_max_payload_size {
	AL_PCIE_MPS_DEFAULT,
	AL_PCIE_MPS_128		= 0,
	AL_PCIE_MPS_256		= 1,
};

/**
 * al_mod_pcie_port: data structure used by the HAL to handle a specific pcie port.
 * this structure is allocated and set to zeros by the upper layer, then it is
 * initialized by the al_mod_pcie_port_handle_init() that should be called before any
 * other function of this API. later, this handle passed to the API functions.
 */
struct al_mod_pcie_port {
	void __iomem		*pcie_reg_base;
	struct al_mod_pcie_regs 	regs_ptrs;
	struct al_mod_pcie_regs	*regs;
	uint32_t		*ex_regs_ptrs[AL_PCIE_EX_REGS_NUM];
	void			*ex_regs;
	void __iomem		*pbs_regs;

	/* Rev ID */
	uint8_t		rev_id;
	/* Subrevision ID */
	uint8_t		subrev_id;
	unsigned int	port_id;
	uint8_t		max_lanes;

	/* For EP mode only */
	uint8_t		max_num_of_pfs;

	/* Internally used */
	struct al_mod_pcie_ib_hcrd_config ib_hcrd_config;
	unsigned int num_retrains;
};

/**
 * al_mod_pcie_pf: the pf handle, a data structure used to handle PF specific
 * functionality. Initialized using "al_mod_pcie_pf_handle_init()"
 *
 * Note: This structure should be used for EP mode only
 */
struct al_mod_pcie_pf {
	unsigned int		pf_num;
	struct al_mod_pcie_port	*pcie_port;
};

struct al_mod_pcie_clk_params {
	/* SB clock frequency in Hz */
	unsigned int	sb_clk_freq;
	/**
	 * Reference clock:
	 * - for RC: 0
	 * - for EP: there are 3 EP reference clocks, each port can be attached to one clock
	 *   - 1 for EP ref_clk #1
	 *   - 2 for EP ref_clk #2
	 *   - 3 for EP ref_clk #3
	 */
	unsigned int	ref_clk;
};

/** Operating mode (endpoint, root complex) */
enum al_mod_pcie_operating_mode {
	AL_PCIE_OPERATING_MODE_EP,
	AL_PCIE_OPERATING_MODE_RC,
	AL_PCIE_OPERATING_MODE_UNKNOWN
};

/* The maximum link speed, measured GT/s (Giga transfer / second)
 *   DEFAULT: do not change the current speed
 *   GEN1: 2.5 GT/s
 *   GEN2: 5 GT/s
 *   GEN3: 8GT/s
 *
 *   Note: The values of this enumerator are important for proper behavior
 */
enum al_mod_pcie_link_speed {
	AL_PCIE_LINK_SPEED_DEFAULT,
	AL_PCIE_LINK_SPEED_GEN1 = 1,
	AL_PCIE_LINK_SPEED_GEN2 = 2,
	AL_PCIE_LINK_SPEED_GEN3 = 3
};

/** PCIe capabilities that supported by a specific port */
struct al_mod_pcie_max_capability {
	al_mod_bool		end_point_mode_supported;
	al_mod_bool		root_complex_mode_supported;
	enum al_mod_pcie_link_speed	max_speed;
	uint8_t		max_lanes;
	uint8_t		atu_regions_num;
	uint32_t	atu_min_size; /* Size granularity: 4 Kbytes */
};

/** PCIe link related parameters */
struct al_mod_pcie_link_params {
	enum al_mod_pcie_link_speed		max_speed;
	enum al_mod_pcie_max_payload_size	max_payload_size;

};

/** PCIe gen2 link parameters */
struct al_mod_pcie_gen2_params {
	al_mod_bool	tx_swing_low; /* set tx swing low when true, and tx swing full when false */
	al_mod_bool	tx_compliance_receive_enable;
	al_mod_bool	set_deemphasis;
};

/** PCIe gen 3 standard per lane equalization parameters */
struct al_mod_pcie_gen3_lane_eq_params {
	uint8_t		downstream_port_transmitter_preset;
	uint8_t		downstream_port_receiver_preset_hint;
	uint8_t		upstream_port_transmitter_preset;
	uint8_t		upstream_port_receiver_preset_hint;
};

/** Gen 3 feedback mode */
enum al_mod_pcie_gen3_fb_mode {
	/** Direction change */
	AL_PCIE_GEN3_FB_MODE_DIR_CHG = 0,
	/** Figure of merrit */
	AL_PCIE_GEN3_FB_MODE_FOM,
};

/** Direction change feedback mode params */
struct al_mod_pcie_gen3_fmdc {
	/**
	 * Convergence Window Aperture for C+1.
	 * Post-cursor coefficients maximum delta within the convergence window depth.
	 * Allowed range: 0,1,2,..15
	 */
	unsigned int max_post_cusror_delta;
	/**
	 * Convergence Window Aperture for C-1.
	 * Pre-cursor coefficients maximum delta within the convergence window depth.
	 * Allowed range: 0,1,2,..15
	 */
	unsigned int max_pre_cusror_delta;
	/**
	 * Convergence Window Depth.
	 * Number of consecutive evaluations considered in Phase 2/3 when determining if optimal
	 * coefficients have been found.
	 * Allowed range: 0,1,2,..16
	 * When set to 0, EQ Master is performed without sending any requests to the remote partner
	 * in Phase 2 for USP and Phase 3 for DSP. Therefore, the remote partner will not change its
	 * transmitter coefficients and will move to the next state.
	 */
	unsigned int n_evals;
	/**
	 * Minimum Time (in ms) To Remain in EQ Master Phase.
	 * The LTSSM stays in EQ Master phase for at least this amount of time, before starting to
	 * check for convergence of the coefficients.
	 * Allowed values 0,1,...,24
	 */
	unsigned int t_min_phas;
};

/** PCIe gen 3 equalization parameters */
struct al_mod_pcie_gen3_params {
	al_mod_bool	perform_eq;
	al_mod_bool	interrupt_enable_on_link_eq_request;
	struct al_mod_pcie_gen3_lane_eq_params *eq_params; /* array of lanes params */
	unsigned int eq_params_elements; /* number of elements in the eq_params array */

	al_mod_bool	eq_disable; /* disables the equalization feature */
	al_mod_bool eq_phase2_3_disable; /* Equalization Phase 2 and Phase 3 */
				     /* Disable (RC mode only) */
	al_mod_bool eq_redo_bypass; /* Equalization re-do bypassing */
	uint8_t local_lf; /* Full Swing (FS) Value for Gen3 Transmit Equalization */
			  /* Value Range: 12 through 63 (decimal).*/

	uint8_t	local_fs; /* Low Frequency (LF) Value for Gen3 Transmit Equalization */
	/**
	 * Gen 3 preset request vector (RC)
	 * Bit [15:0] =0x0: No preset is requested and evaluated in EQ Master Phase
	 * Bit [i] =1: "Preset=i" is requested and evaluated in EQ Master Phase.
	 */
	unsigned int pset_req_vec_rc;
	/**
	 * Gen 3 preset request vector (EP)
	 * Bit [15:0] =0x0: No preset is requested and evaluated in EQ Master Phase
	 * Bit [i] =1: "Preset=i" is requested and evaluated in EQ Master Phase.
	 */
	unsigned int pset_req_vec_ep;
	enum al_mod_pcie_gen3_fb_mode fb_mode;
	struct al_mod_pcie_gen3_fmdc *fmdc;
};

/**
 * Inbound posted/non-posted header credits and outstanding outbound reads
 * completion header configuration.
 *
 * This structure controls the resource partitioning of an important resource in
 * the PCIe port. This resource includes the PCIe TLP headers coming on the PCIe
 * port, and is shared between three types:
 *  - Inbound Non-posted, which are PCIe Reads as well as PCIe Config Cycles
 *  - Inbound Posted, i.e. PCIe Writes
 *  - Inbound Read-completion, which are the completions matching and outbound
 *    reads issued previously by the same core.
 * The programmer need to take into consideration that a given outbound read
 * request could be split on the return path into Ceiling[MPS_Size / 64] + 1
 * of Read Completions.
 * Programmers are not expected to modify these setting except for rare cases,
 * where a different ratio between Posted-Writes and Read-Completions is desired
 *
 * Constraints:
 * - nof_cpl_hdr + nof_np_hdr + nof_p_hdr ==
 *			AL_PCIE_REV_1_2_IB_HCRD_SUM/AL_PCIE_REV3_IB_HCRD_SUM
 * - nof_cpl_hdr > 0
 * - nof_p_hdr > 0
 * - nof_np_hdr > 0
 */
struct al_mod_pcie_ib_hcrd_os_ob_reads_config {
	/** Max number of outstanding outbound reads */
	uint8_t nof_outstanding_ob_reads;

	/**
	 * This value set the possible outstanding headers CMPLs , the core
	 * can get (the core always advertise infinite credits for CMPLs).
	 */
	unsigned int nof_cpl_hdr;

	/**
	 * This value set the possible outstanding headers reads (non-posted
	 * transactions), the core can get  (it set the value in the init FC
	 * process).
	 */
	unsigned int nof_np_hdr;

	/**
	 * This value set the possible outstanding headers writes (posted
	 * transactions), the core can get  (it set the value in the init FC
	 * process).
	 */
	unsigned int nof_p_hdr;
};

/**
 * PCIe Ack/Nak Latency and Replay timers
 *
 * Note: Programmer is not expected to modify these values unless working in
 *       very slow external devices like low-end FPGA or hardware devices
 *       emulated in software
 */
struct al_mod_pcie_latency_replay_timers {
	uint16_t	round_trip_lat_limit;
	uint16_t	replay_timer_limit;
};

/**
 * SRIS KP counter values
 *
 * Description: SRIS is PCI SIG ECN, that enables the two peers on a given PCIe
 * link to run with Separate Reference clock with Independent Spread spectrum
 * clock and requires inserting PCIe SKP symbols on the link in faster frequency
 * that original PCIe spec
 */
struct al_mod_pcie_sris_params {
	/** set to AL_TRUE to use defaults and ignore the other parameters */
	al_mod_bool		use_defaults;
	uint16_t	kp_counter_gen3;	/* only for Gen3 */
	uint16_t	kp_counter_gen21;
};

/**
 * Relaxed ordering params
 * Enable ordering relaxations for applications that does not require
 * enforcement of 'completion must not bypass posted' ordering rule.
 *
 * Recommendation:
 *  - For downstream port, set enable_tx_relaxed_ordering
 *  - For upstream port
 *     - set enable_rx_relaxed_ordering
 *     - set enable tx_relaxed_ordering for emulated EP.
 *
 * Defaults:
 *  - For Root-Complex:
 *     - tx_relaxed_ordering = AL_FALSE, rx_relaxed_ordering = AL_TRUE
 *  - For End-Point:
 *     - tx_relaxed_ordering = AL_TRUE, rx_relaxed_ordering = AL_FALSE
 */
struct al_mod_pcie_relaxed_ordering_params {
	al_mod_bool		enable_tx_relaxed_ordering;
	al_mod_bool		enable_rx_relaxed_ordering;
};

/** PCIe port configuration parameters
 * This structure includes the parameters that the HAL should apply to the port
 * (by al_mod_pcie_port_config()).
 * The fields that are pointers (e.g. link_params) can be set to NULL, in that
 * case, the al_mod_pcie_port_config() will keep the current HW settings.
 */
struct al_mod_pcie_port_config_params {
	struct al_mod_pcie_link_params		*link_params;
	al_mod_bool					enable_axi_snoop;
	al_mod_bool					enable_ram_parity_int;
	al_mod_bool					enable_axi_parity_int;
	struct al_mod_pcie_latency_replay_timers	*lat_rply_timers;
	struct al_mod_pcie_gen2_params		*gen2_params;
	struct al_mod_pcie_gen3_params		*gen3_params;
	/*
	 * Sets all internal timers to Fast Mode for speeding up simulation.
	 * this varible should be set always to AL_FALSE unless user is running
	 * on simulation setup
	 */
	al_mod_bool					fast_link_mode;
	/*
	 * when true, the PCI unit will return Slave Error/Decoding Error to any
	 * I/O Fabric master or Internal Processors in case of error.
	 * when false, the value 0xFFFFFFFF will be returned without error indication.
	 */
	al_mod_bool					enable_axi_slave_err_resp;
	struct al_mod_pcie_sris_params		*sris_params;
	struct al_mod_pcie_relaxed_ordering_params	*relaxed_ordering_params;
};

/**
 * PCIe port live configuration params
 */
struct al_mod_pcie_port_config_live_params {
};

/**
 * BAR register configuration parameters
 * Note: This structure should be used for EP mode only
 */
struct al_mod_pcie_ep_bar_params {
	al_mod_bool		enable;
	al_mod_bool		memory_space; /**< memory or io */
	al_mod_bool		memory_64_bit; /**< is memory space is 64 bit */
	al_mod_bool		memory_is_prefetchable;
	uint64_t	size; /* the bar size in bytes */
};

/**
 * ID registers configuration parameters ()
 * Note: This structure should be used for EP mode only
 */
struct al_mod_pcie_ep_id_params {
	al_mod_bool		device_id_override;
	unsigned int	device_id;
	al_mod_bool		vendor_id_override;
	unsigned int	vendor_id;
	al_mod_bool		class_code_override;
	unsigned int	class_code;
	al_mod_bool		revision_id_override;
	unsigned int	revision_id;
};

/**
 * PF config params (EP mode only)
 * Note: This structure should be used for EP mode only
 */
struct al_mod_pcie_pf_config_params {
	/**
	 * disable advertising D1 and D3hot state
	 * Recommended to be AL_TRUE
	 */
	al_mod_bool				cap_d1_d3hot_dis;
	/**
	 * disable advertising support for Function-Level-Reset
	 * Recommended to be AL_FALSE
	 */
	al_mod_bool				cap_flr_dis;
	/*
	 * disable advertising Advanced power management states
	 */
	al_mod_bool				cap_aspm_dis;
	al_mod_bool				bar_params_valid;
	/*
	 * Note: only bar_params[0], [2] and [4] can have memory_64_bit enabled
	 * and in such case, the next bar ([1], [3], or [5] respectively) is not used
	 */
	struct al_mod_pcie_ep_bar_params	bar_params[6];
	struct al_mod_pcie_ep_bar_params	exp_bar_params;/* expansion ROM BAR*/

	struct al_mod_pcie_ep_id_params	*id_params;
};

/** PCIe link status */
struct al_mod_pcie_link_status {
	al_mod_bool			link_up;
	enum al_mod_pcie_link_speed	speed;
	uint8_t			lanes; /* Number of lanes */
	uint8_t			ltssm_state;

	/**
	 * Number of retrains due to link width not matching detected lanes
	 *
	 * Note: relevant on when same handle was used for both establishing the link and checking
	 * the link status
	 */
	unsigned int		num_retrains;
};

/** PCIe lane status */
struct al_mod_pcie_lane_status {
	al_mod_bool			is_reset;
	enum al_mod_pcie_link_speed	requested_speed;
};

/**
 * PCIe MSIX capability configuration parameters
 * Note: This structure should be used for EP mode only
 */
struct al_mod_pcie_msix_params {
	/* Number of entries - size can be up to: 2024 */
	unsigned int	table_size;
	unsigned int	table_offset;
	unsigned int	table_bar;
	unsigned int	pba_offset;
	/* which bar to use when calculating the PBA table address and adding offset to */
	unsigned int	pba_bar;
};

/** PCIE AER capability parameters */
struct al_mod_pcie_aer_params {
	/** ECRC Generation Enable
	 *  while this feature is powerful, all known Chip-sets and processors
	 *  do not support it as of 2015
	 */
	al_mod_bool		ecrc_gen_en;
	/** ECRC Check Enable */
	al_mod_bool		ecrc_chk_en;

	/**
	 * Enabled reporting of correctable errors (bit mask)
	 * See 'AL_PCIE_AER_CORR_*' for details
	 * 0 - no reporting at all
	 */
	unsigned int	enabled_corr_err;
	/**
	 * Enabled reporting of non-fatal uncorrectable errors (bit mask)
	 * See 'AL_PCIE_AER_UNCORR_*' for details
	 * 0 - no reporting at all
	 */
	unsigned int	enabled_uncorr_non_fatal_err;
	/**
	 * Enabled reporting of fatal uncorrectable errors (bit mask)
	 * See 'AL_PCIE_AER_UNCORR_*' for details
	 * 0 - no reporting at all
	 */
	unsigned int	enabled_uncorr_fatal_err;
};

/******************************************************************************/
/********************************** PCIe API **********************************/
/******************************************************************************/

/*************************** PCIe Initialization API **************************/

/**
 * Initializes a PCIe port handle structure.
 *
 * @param   pcie_port		an allocated, non-initialized instance.
 * @param   pcie_reg_base	the virtual base address of the port internal
 *				registers
 * @param   pbs_reg_base	the virtual base address of the pbs functional
 *				registers
 * @param   port_id		the port id (used mainly for debug messages)
 *
 * @return 0 if no error found.
 */
int al_mod_pcie_port_handle_init(struct al_mod_pcie_port *pcie_port,
			 void __iomem *pcie_reg_base,
			 void __iomem *pbs_reg_base,
			 unsigned int port_id);

/**
 * Print a PCIe port performance parameters
 *
 * @param	pcie_port
 *		An initialized instance
 */
void al_mod_pcie_port_perf_params_print(
	struct al_mod_pcie_port *pcie_port);

/**
 * Initialize the PCIe clock
 * Note: relevant only for REV4+ ports
 * @param  pcie_port       pcie port handle
 * @param  pcie_clk_params pcie clock params
 * @return                 0 if no error found
 */
int al_mod_pcie_port_clk_init(struct al_mod_pcie_port *pcie_port,
			  struct al_mod_pcie_clk_params *pcie_clk_params);

/**
 * Initializes a PCIe pf handle structure
 * @param  pcie_pf   an allocated, non-initialized instance of pf handle
 * @param  pcie_port pcie port handle
 * @param  pf_num    physical function number
 * @return           0 if no error found
 */
int al_mod_pcie_pf_handle_init(
	struct al_mod_pcie_pf *pcie_pf,
	struct al_mod_pcie_port *pcie_port,
	unsigned int pf_num);

/**
 * Get port revision ID
 * @param  pcie_port pcie port handle
 * @return           Port rev_id
 */
int al_mod_pcie_port_rev_id_get(struct al_mod_pcie_port *pcie_port);

/************************** Pre PCIe Port Enable API **************************/

/**
 * @brief set current pcie operating mode (root complex or endpoint)
 * This function can be called only before enabling the controller using
 * al_mod_pcie_port_enable().
 *
 * @param pcie_port pcie port handle
 * @param mode pcie operating mode
 *
 * @return 0 if no error found.
 */
int al_mod_pcie_port_operating_mode_config(struct al_mod_pcie_port *pcie_port,
				  enum al_mod_pcie_operating_mode mode);

/**
 * Configure number of lanes connected to this port.
 * This function can be called only before enabling the controller using al_mod_pcie_port_enable().
 *
 * @param pcie_port pcie port handle
 * @param lanes number of lanes  (must be 1,2,4,8,16  and not any other value)
 *
 * Note: this function must be called before any al_mod_pcie_port_config() calls
 *
 * @return 0 if no error found.
 */
int al_mod_pcie_port_max_lanes_set(struct al_mod_pcie_port *pcie_port, uint8_t lanes);

/**
 * Get number of lanes connected to this port.
 *
 * @param pcie_port pcie port handle
 * @param lanes pointer to the return number of lanes
 *
 * @return 0 if no error found.
 */
int al_mod_pcie_port_max_lanes_get(struct al_mod_pcie_port *pcie_port, uint8_t *lanes);

/**
 * Set maximum physical function numbers
 * @param pcie_port      pcie port handle
 * @param max_num_of_pfs number of physical functions
 *
 * Notes:
 *  - this function must be called before any al_mod_pcie_pf_config() calls
 *  - exposed on a given PCIe Endpoint port
 *  - PCIe rev1/rev2 supports only single Endpoint
 *  - PCIe rev3 can support up to 4
 */
int al_mod_pcie_port_max_num_of_pfs_set(
	struct al_mod_pcie_port *pcie_port,
	uint8_t max_num_of_pfs);

/**
 * @brief Inbound posted/non-posted header credits and outstanding outbound
 *        reads completion header configuration
 *
 * @param	pcie_port pcie port handle
 * @param	ib_hcrd_os_ob_reads_config
 * 		Inbound header credits and outstanding outbound reads
 * 		configuration
 */
int al_mod_pcie_port_ib_hcrd_os_ob_reads_config(
	struct al_mod_pcie_port *pcie_port,
	struct al_mod_pcie_ib_hcrd_os_ob_reads_config *ib_hcrd_os_ob_reads_config);

/** return PCIe operating mode
 * @param pcie_port	pcie port handle
 * @return		operating mode
 */
enum al_mod_pcie_operating_mode al_mod_pcie_operating_mode_get(
	struct al_mod_pcie_port *pcie_port);

/**
 * PCIe AXI quality of service configuration
 *
 * @param	pcie_port
 *		Initialized PCIe port handle
 * @param	arqos
 *		AXI read quality of service (0 - 15)
 * @param	awqos
 *		AXI write quality of service (0 - 15)
 */
void al_mod_pcie_axi_qos_config(
	struct al_mod_pcie_port	*pcie_port,
	unsigned int		arqos,
	unsigned int		awqos);

/**************************** PCIe Port Enable API ****************************/

/**
 *  Enable PCIe unit (deassert reset)
 *  This function only enables the port, without any configuration/link
 *  functionality. Should be called before starting any configuration/link API
 *
 * @param   pcie_port pcie port handle
 *
 * @return 0 if no error found.
 */
int al_mod_pcie_port_enable(struct al_mod_pcie_port *pcie_port);

/** Disable PCIe unit (assert reset)
 *
 * @param   pcie_port pcie port handle
 */
void al_mod_pcie_port_disable(struct al_mod_pcie_port *pcie_port);

/**
 * Port memory shutdown/up
 * Memory shutdown should be called for an unused ports for power-saving
 *
 * Caution: This function can be called only when the controller is disabled
 *
 * @param pcie_port pcie port handle
 * @param enable memory shutdown enable or disable
 *
 */
int al_mod_pcie_port_memory_shutdown_set(
	struct al_mod_pcie_port	*pcie_port,
	al_mod_bool			enable);

/**
 * Check if port enabled or not
 * @param  pcie_port pcie port handle
 * @return           AL_TRUE of port enabled and AL_FALSE otherwise
 */
al_mod_bool al_mod_pcie_port_is_enabled(struct al_mod_pcie_port *pcie_port);

/**
 * Check if port enabled or not (without a handle)
 * @param   pbs_reg_base	the virtual base address of the pbs functional
 *				registers
 * @param   port_id		the port id
 *
 * @return           AL_TRUE of port enabled and AL_FALSE otherwise
 */
al_mod_bool al_mod_pcie_port_is_enabled_raw(
	void __iomem	*pbs_reg_base,
	unsigned int	port_id);

/**
 * Check if port memory enabled or not
 * @param  pcie_port pcie port handle
 * @return           AL_TRUE of port memory enabled and AL_FALSE otherwise
 */
al_mod_bool al_mod_pcie_port_memory_is_shutdown(struct al_mod_pcie_port *pcie_port);

/*************************** PCIe Configuration API ***************************/

/**
 * @brief   configure pcie port (mode, link params, etc..)
 * this function must be called before initializing the link
 *
 * @param pcie_port pcie port handle
 * @param params configuration structure.
 *
 * @return  0 if no error found
 */
int al_mod_pcie_port_config(struct al_mod_pcie_port *pcie_port,
			const struct al_mod_pcie_port_config_params *params);

/**
 * PCIe port live configuration (when link is already up)
 *
 * @param	pcie_port
 *		PCIe port handle
 * @param	params
 *		Configuration params
 */
void al_mod_pcie_port_config_live(
	struct al_mod_pcie_port				*pcie_port,
	const struct al_mod_pcie_port_config_live_params	*params);

/**
 * @brief Configure a specific PF
 * this function must be called before any datapath transactions
 *
 * @param pcie_pf	pcie pf handle
 * @param params	configuration structure.
 *
 * @return		0 if no error found
 */
int al_mod_pcie_pf_config(
	struct al_mod_pcie_pf *pcie_pf,
	const struct al_mod_pcie_pf_config_params *params);

/*************************** PCIe PM Related API ***************************/

/** The port supports L0s ASPM */
#define AL_PCIE_PM_ASPM_CFG_L0S		AL_BIT(0)
/** The port supports L1 ASPM */
#define AL_PCIE_PM_ASPM_CFG_L1		AL_BIT(1)

/**
 * @brief Configure ASPM capabilities of the port
 *
 * @param pcie_port	PCIe port handle
 * @param aspm_cfg	desired aspm configuration
 *			Combination of AL_PCIE_PM_ASPM_CFG_*
 */
void al_mod_pcie_pm_aspm_cfg_set(
	struct al_mod_pcie_port	*pcie_port,
	unsigned int		aspm_cfg);

/**
 * @brief Transition the link to or from L2 state
 *
 * @param pcie_port	PCIe port handle
 * @param enable	AL_TRUE to enter L2, AL_FALSE to exit
 *	Only RC is allowed to initiate transition to L2
 *	Only EP is allowed to wake the link up from L2
 *
 * @return	0 if no error found
 */

void al_mod_pcie_pm_l2_trigger(
	struct al_mod_pcie_port	*pcie_port,
	al_mod_bool			enable);

/************************** PCIe Link Operations API **************************/

/**
 * @brief   start pcie link
 * This function starts the link and should be called only after port is enabled
 * and pre port-enable and configurations are done
 * @param   pcie_port pcie port handle
 *
 * @return  0 if no error found
 */
int al_mod_pcie_link_start(struct al_mod_pcie_port *pcie_port);

/**
 * @brief   stop pcie link
 *
 * @param   pcie_port pcie port handle
 *
 * @return  0 if no error found
 */
int al_mod_pcie_link_stop(struct al_mod_pcie_port *pcie_port);

/**
 * @brief   check if pcie link is started
 * Note that this function checks if link is started rather than link is up
 * @param  pcie_port pcie port handle
 * @return           AL_TRUE if link is started and AL_FALSE otherwise
 */
al_mod_bool al_mod_pcie_is_link_started(struct al_mod_pcie_port *pcie_port);

/**
 * @brief   trigger link-disable
 *
 * @param   pcie_port pcie port handle
 * @param   disable   AL_TRUE to disable the link and AL_FALSE to enable it
 *
 * Note: this functionality differs from "al_mod_pcie_link_stop" as it's a spec
 *       functionality where both sides of the PCIe agrees to disable the link
 * @return  0 if no error found
 */
int al_mod_pcie_link_disable(struct al_mod_pcie_port *pcie_port, al_mod_bool disable);

/**
 * @brief   wait for link up indication (with extended features)
 * this function waits for link up indication, it polls LTSSM state until link is ready
 *
 * @param   pcie_port pcie port handle
 * @param   timeout_ms maximum timeout in milli-seconds to wait for link up
 * @param   retrain_if_not_full_width retrigger training if link doesn't include all detected lanes
 *
 * @return  0 if link up indication detected
 *          -ETIME if not.
 */
int al_mod_pcie_link_up_wait_ex(struct al_mod_pcie_port *pcie_port, uint32_t timeout_ms,
	al_mod_bool retrain_if_not_full_width);

/**
 * @brief   wait for link up indication
 * this function waits for link up indication, it polls LTSSM state until link is ready
 *
 * @param   pcie_port pcie port handle
 * @param   timeout_ms maximum timeout in milli-seconds to wait for link up
 *
 * @return  0 if link up indication detected
 * 	    -ETIME if not.
 */
int al_mod_pcie_link_up_wait(struct al_mod_pcie_port *pcie_port, uint32_t timeout_ms);

/**
 * @brief get number of active lanes
 * @param  pcie_port pcie port handle
 * @return           number of active lanes
 */
int al_mod_pcie_link_active_lanes_get(struct al_mod_pcie_port *pcie_port);

/**
 * @brief   get link status
 *
 * @param   pcie_port pcie port handle
 * @param   status structure for link status
 *
 * @return  0 if no error found
 */
int al_mod_pcie_link_status(struct al_mod_pcie_port *pcie_port, struct al_mod_pcie_link_status *status);

/**
 * @brief   get lane status
 *
 * @param	pcie_port
 *		pcie port handle
 * @param	lane
 *		PCIe lane
 * @param	status
 *		Pointer to returned structure for lane status
 *
 */
void al_mod_pcie_lane_status_get(
	struct al_mod_pcie_port		*pcie_port,
	unsigned int			lane,
	struct al_mod_pcie_lane_status	*status);

/**
 * @brief   trigger hot reset
 * this function initiates In-Band reset while link is up.
 * to initiate hot reset: call this function with AL_TRUE
 * to exit from hos reset: call this function with AL_FALSE
 * Note: This function should be called in RC mode only
 *
 * @param   pcie_port pcie port handle
 * @param   enable   AL_TRUE to enable hot-reset and AL_FALSE to disable it
 *
 * @return  0 if no error found
 */
int al_mod_pcie_link_hot_reset(struct al_mod_pcie_port *pcie_port, al_mod_bool enable);

/**
 * @brief   trigger link-retain
 * this function initiates Link retraining by directing the Physical Layer LTSSM
 * to the Recovery state. If the LTSSM is already in Recovery or Configuration,
 * re-entering Recovery is permitted but not required.
 * Note: This function should be called in RC mode only. There's no need to disable
 * link-retrain
 *
 * @param   pcie_port pcie port handle
 *
 * @return  0 if no error found
 */
int al_mod_pcie_link_retrain(struct al_mod_pcie_port *pcie_port);

/**
 * @brief   set redo equalization configuration.
 * will be performed with next link retrain sequence
 * Note: This function should be called in RC mode only

 * @param   pcie_port pcie port handle
 * @param   enable   AL_TRUE to enable redo equalization with next link retrain
 * and AL_FALSE to disable it
 *
 * @return  0 if no error found
 */
int al_mod_pcie_link_retrain_eq_redo_en(struct al_mod_pcie_port *pcie_port, al_mod_bool enable);

/**
 * @brief   change port speed
 * this function changes the port speed, it doesn't wait for link re-establishment
 *
 * @param   pcie_port pcie port handle
 * @param   new_speed the new speed gen to set
 *
 * @return  0 if no error found
 */
int al_mod_pcie_link_change_speed(struct al_mod_pcie_port *pcie_port, enum al_mod_pcie_link_speed new_speed);

/* TODO: check if this function needed */
int al_mod_pcie_link_change_width(struct al_mod_pcie_port *pcie_port, uint8_t width);

/** LTSSM state */
enum al_mod_pcie_ltssm_state {
	AL_PCIE_LTSSM_STATE_DETECT_QUIET	= 0x0,
	AL_PCIE_LTSSM_STATE_DETECT_ACT		= 0x1,
	AL_PCIE_LTSSM_STATE_POLL_ACTIVE		= 0x2,
	AL_PCIE_LTSSM_STATE_POLL_COMPLIANCE	= 0x3,
	AL_PCIE_LTSSM_STATE_POLL_CONFIG		= 0x4,
	AL_PCIE_LTSSM_STATE_PRE_DETECT_QUIET	= 0x5,
	AL_PCIE_LTSSM_STATE_DETECT_WAIT		= 0x6,
	AL_PCIE_LTSSM_STATE_CFG_LINKWD_START	= 0x7,
	AL_PCIE_LTSSM_STATE_CFG_LINKWD_ACEPT	= 0x8,
	AL_PCIE_LTSSM_STATE_CFG_LANENUM_WAIT	= 0x9,
	AL_PCIE_LTSSM_STATE_CFG_LANENUM_ACEPT	= 0xa,
	AL_PCIE_LTSSM_STATE_CFG_COMPLETE	= 0xb,
	AL_PCIE_LTSSM_STATE_CFG_IDLE		= 0xc,
	AL_PCIE_LTSSM_STATE_RCVRY_LOCK		= 0xd,
	AL_PCIE_LTSSM_STATE_RCVRY_SPEED		= 0xe,
	AL_PCIE_LTSSM_STATE_RCVRY_RCVRCFG	= 0xf,
	AL_PCIE_LTSSM_STATE_RCVRY_IDLE		= 0x10,
	AL_PCIE_LTSSM_STATE_L0			= 0x11,
	AL_PCIE_LTSSM_STATE_L0S			= 0x12,
	AL_PCIE_LTSSM_STATE_L123_SEND_EIDLE	= 0x13,
	AL_PCIE_LTSSM_STATE_L1_IDLE		= 0x14,
	AL_PCIE_LTSSM_STATE_L2_IDLE		= 0x15,
	AL_PCIE_LTSSM_STATE_L2_WAKE		= 0x16,
	AL_PCIE_LTSSM_STATE_DISABLED_ENTRY	= 0x17,
	AL_PCIE_LTSSM_STATE_DISABLED_IDLE	= 0x18,
	AL_PCIE_LTSSM_STATE_DISABLED		= 0x19,
	AL_PCIE_LTSSM_STATE_LPBK_ENTRY		= 0x1a,
	AL_PCIE_LTSSM_STATE_LPBK_ACTIVE		= 0x1b,
	AL_PCIE_LTSSM_STATE_LPBK_EXIT		= 0x1c,
	AL_PCIE_LTSSM_STATE_LPBK_EXIT_TIMEOUT	= 0x1d,
	AL_PCIE_LTSSM_STATE_HOT_RESET_ENTRY	= 0x1e,
	AL_PCIE_LTSSM_STATE_HOT_RESET		= 0x1f,
	AL_PCIE_LTSSM_RCVRY_EQ0			= 0x20,
	AL_PCIE_LTSSM_RCVRY_EQ1			= 0x21,
	AL_PCIE_LTSSM_RCVRY_EQ2			= 0x22,
	AL_PCIE_LTSSM_RCVRY_EQ3			= 0x23,
};

/**
 * Enable vector 0 LTSSM interrupt when LTSSM changes state machine to requested state
 * @param  pcie_port pcie port handle
 * @param  state LTSSM state
 */
void al_mod_pcie_link_ltssm_state_vec0_cmp_enable(
	struct al_mod_pcie_port *pcie_port,
	enum al_mod_pcie_ltssm_state state);

/**
 * @brief   get LTSSM state
 *
 * @param   pcie_port pcie port handle
 * @param   ltssm_state LTSSM state
 * @param   skip_port_enabled_check skip the check if port is enabled
 *
 * @return  0 if no error found
 */
int al_mod_pcie_ltssm_state_get(struct al_mod_pcie_port *pcie_port,
		uint8_t *ltssm_state,
		al_mod_bool skip_port_enabled_check);

/**************************** Post Link Start API *****************************/

/************************** Snoop Configuration API ***************************/

/**
 * @brief configure pcie port axi snoop
 * This enable the inbound PCIe posted write data or the Read completion data to
 * snoop the internal processor caches for I/O cache coherency
 *
 * @param pcie_port pcie port handle
 * @param enable_axi_snoop enable snoop.
 *
 * @return  0 if no error found
 */
/* TODO: Can this API be called after port enable? */
int al_mod_pcie_port_snoop_config(struct al_mod_pcie_port *pcie_port,
				al_mod_bool enable_axi_snoop);

/************************** Configuration Space API ***************************/

/**
 * Configuration Space Access Through PCI-E_ECAM_Ext PASW
 * This feature enables the internal processors to generate configuration cycles
 * on the PCIe ports by writing to part of the processor memory space marked by
 * the PCI-E_EXCAM_Ext address window
 */

/**
 * @brief   get base address of pci configuration space header
 * @param   pcie_pf	pcie pf handle
 * @param   addr	pointer for returned address;
 * @return              0 if no error found
 */
int al_mod_pcie_config_space_get(
	struct al_mod_pcie_pf *pcie_pf,
	uint8_t __iomem **addr);

/**
 * Read data from the local configuration space
 *
 * @param	pcie_pf	pcie	pf handle
 * @param	reg_offset	Configuration space register offset
 * @return	Read data
 */
uint32_t al_mod_pcie_local_cfg_space_read(
	struct al_mod_pcie_pf	*pcie_pf,
	unsigned int		reg_offset);

/**
 * Write data to the local configuration space
 *
 * @param	pcie_pf		PCIe pf handle
 * @param	reg_offset	Configuration space register offset
 * @param	data		Data to write
 * @param	cs2		Should be AL_TRUE if dbi_cs2 must be asserted
 *				to enable writing to this register, according to
 *				the PCIe Core specifications
 * @param	allow_ro_wr	AL_TRUE to allow writing into read-only regs
 *
 */
void al_mod_pcie_local_cfg_space_write(
	struct al_mod_pcie_pf	*pcie_pf,
	unsigned int		reg_offset,
	uint32_t		data,
	al_mod_bool			cs2,
	al_mod_bool			allow_ro_wr);

/**
 * @brief   set target_bus and mask_target_bus
 *
 * Call this function with target_bus set to the required bus of the next
 * outbound config access to be issued. No need to call that function if the
 * next config access bus equals to the last one.
 *
 * @param   pcie_port pcie port handle
 * @param   target_bus
 * @param   mask_target_bus
 * @return  0 if no error found
 */
int al_mod_pcie_target_bus_set(struct al_mod_pcie_port *pcie_port,
			   uint8_t target_bus,
			   uint8_t mask_target_bus);

/**
 * @brief   get target_bus and mask_target_bus
 * @param   pcie_port pcie port handle
 * @param   target_bus
 * @param   mask_target_bus
 * @return  0 if no error found
 */
int al_mod_pcie_target_bus_get(struct al_mod_pcie_port *pcie_port,
			   uint8_t *target_bus,
			   uint8_t *mask_target_bus);

/**
 * Set secondary bus number
 *
 * Same as al_mod_pcie_target_bus_set but with secondary bus
 *
 * @param pcie_port pcie port handle
 * @param secbus pci secondary bus number
 *
 * @return 0 if no error found.
 */
int al_mod_pcie_secondary_bus_set(struct al_mod_pcie_port *pcie_port, uint8_t secbus);

/**
 * Set subordinary bus number
 *
 * Same as al_mod_pcie_target_bus_set but with subordinary bus
 *
 * @param   pcie_port pcie port handle
 * @param   subbus the highest bus number of all of the buses that can be reached
 *		downstream of the PCIE instance.
 *
 * @return 0 if no error found.
 */
int al_mod_pcie_subordinary_bus_set(struct al_mod_pcie_port *pcie_port,uint8_t subbus);

/**
 * @brief Enable/disable deferring incoming configuration requests until
 * initialization is complete. When enabled, the core completes incoming
 * configuration requests with a Configuration Request Retry Status.
 * Other incoming non-configuration Requests complete with Unsupported Request status.
 *
 * Note: This function should be used for EP mode only
 *
 * @param pcie_port pcie port handle
 * @param en enable/disable
 */
void al_mod_pcie_app_req_retry_set(struct al_mod_pcie_port *pcie_port, al_mod_bool en);

/**
 * @brief  Check if deferring incoming configuration requests is enabled or not
 * @param  pcie_port pcie port handle
 * @return           AL_TRUE is it's enabled and AL_FALSE otherwise
 */
al_mod_bool al_mod_pcie_app_req_retry_get_status(struct al_mod_pcie_port	*pcie_port);

/**
 * @brief   set configuration space reset on link down
 *
 * @param   pcie_port pcie port handle
 * @param   enable enable reset on link down
 */
void al_mod_pcie_port_cfg_rst_on_link_down_set(struct al_mod_pcie_port *pcie_port, al_mod_bool enable);

/*************** Internal Address Translation Unit (ATU) API ******************/

enum al_mod_pcie_atu_dir {
	AL_PCIE_ATU_DIR_OUTBOUND = 0,
	AL_PCIE_ATU_DIR_INBOUND = 1,
};

/** decoding of the PCIe TLP Type as appears on the wire */
enum al_mod_pcie_atu_tlp {
	AL_PCIE_TLP_TYPE_MEM = 0,
	AL_PCIE_TLP_TYPE_IO = 2,
	AL_PCIE_TLP_TYPE_CFG0 = 4,
	AL_PCIE_TLP_TYPE_CFG1 = 5,
	AL_PCIE_TLP_TYPE_MSG = 0x10,
	AL_PCIE_TLP_TYPE_RESERVED = 0x1f
};

/** default response types */
enum al_mod_pcie_atu_response {
	AL_PCIE_RESPONSE_NORMAL = 0,
	AL_PCIE_RESPONSE_UR = 1, /* UR == Unsupported Request */
	AL_PCIE_RESPONSE_CA = 2  /* CA == Completion Abort    */
};

struct al_mod_pcie_atu_region {

	/**********************************************************************
	 * General Parameters                                                 *
	 **********************************************************************/

	al_mod_bool			enable;
	/* outbound or inbound */
	enum al_mod_pcie_atu_dir	direction;
	/* region index */
	uint8_t			index;
	/* the 64-bit address that get matched with the 64-bit address incoming
	 * on the PCIe TLP
	 * Note: base_addr must be aligned to iATU size (limit - base_addr) if:
	 *  - match_mode = Address Match Mode
	 *  - size > 4GB
	 */
	uint64_t		base_addr;
	/**
	 * limit marks the region's end address.
	 * For Alpine V1 (PCIe rev1): only bits [39:0] are valid
	 * For Alpine V2 (PCIe rev2/rev3): only bits [47:0] are valid
	 * an access is a hit in iATU if the:
	 *   - address >= base_addr
	 *   - address <= limit
	 */
	uint64_t		limit;
	/**
	 * the address that matches (hit) will be translated to:
	 * target_addr + offset
	 * the target address must be aligned to the ATU region's size
	 *
	 * Example: accessing (base_addr + 0x1000) will be translated to:
	 *                    (target_addr + 0x1000) in case limit >= 0x1000
	 *
	 * Note: target_addr must be aligned to iATU size (limit - base_addr) if:
	 *  - match_mode = Address Match Mode
	 *  - size > 4GB
	 */
	uint64_t		target_addr;
	/**
	 * When the Invert feature is activated, an address match occurs when
	 * the untranslated address is not in the region bounded by the Base
	 * address and Limit address. Match occurs when the untranslated address
	 * is not in the region bounded by the base address and limit address
	 */
	al_mod_bool			invert_matching;
	/**
	 * PCIe TLP type
	 * Can be: Mem, IO, CGF0, CFG1 or MSG
	 */
	enum al_mod_pcie_atu_tlp	tlp_type;
	/**
	 * PCIe frame header attr field.
	 * When the address of a TLP is matched to this region, then the ATTR
	 * field of the TLP is changed to the value in this register.
	 */
	uint8_t			attr;

	/**********************************************************************
	 * Outbound specific Parameters                                       *
	 **********************************************************************/

	/**
	 * PCIe Message code
	 * MSG TLPs (Message Code). When the address of an outbound TLP is
	 * matched to this region, and the translated TLP TYPE field is Msg
	 * then the message field of the TLP is changed to the value in this
	 * register.
	 */
	uint8_t			msg_code;
	/**
	 * CFG Shift Mode. This is useful for CFG transactions where the PCIe
	 * configuration mechanism maps bits [27:12] of the address to the
	 * bus/device and function number. This allows a CFG configuration space
	 * to be located in any 256MB window of your application memory space
	 * using a 28-bit effective address.Shifts bits [27:12] of the
	 * untranslated address to form bits [31:16] of the translated address.
	 */
	al_mod_bool			cfg_shift_mode;

	/**********************************************************************
	 * Inbound specific Parameters                                        *
	 **********************************************************************/

	uint8_t			bar_number;
	/**
	 * Match Mode. Determines Inbound matching mode for TLPs. The mode
	 * depends on the type of TLP that is received as follows:
	 * MEM-I/O: 0 = Address Match Mode
	 *          1 = BAR Match Mode
	 * CFG0   : 0 = Routing ID Match Mode
	 *          1 = Accept Mode
	 * MSG    : 0 = Address Match Mode
	 *          1 = Vendor ID Match Mode
	 */
	uint8_t			match_mode;
	/**
	 * For outbound:
	 *  - AL_TRUE : enables taking the function number of the translated TLP
	 *              from the PCIe core
	 *  - AL_FALSE: no function number is taken from PCIe core
	 * For inbound:
	 *  - AL_TRUE : enables ATU function match mode
	 *  - AL_FALSE: no function match mode applied to transactions
	 *
	 * Note: this boolean is ignored in RC mode
	 */
	al_mod_bool			function_match_bypass_mode;
	/**
	 * The function number to match/bypass (see previous parameter)
	 * Note: this parameter is ignored when previous parameter is AL_FALSE
	 */
	uint8_t			function_match_bypass_mode_number;
	/**
	 * setting up what is the default response for an inbound transaction
	 * that matches the iATU
	 */
	enum al_mod_pcie_atu_response response;
	/**
	 * Attr Match Enable. Ensures that a successful AT TLP field comparison
	 * match (see attr above) occurs for address translation to proceed
	 */
	al_mod_bool			enable_attr_match_mode;
	/**
	 * Message Code Match Enable(Msg TLPS). Ensures that a successful
	 * message Code TLP field comparison match (see Message msg_code)occurs
	 * (in MSG transactions) for address translation to proceed.
	 */
	al_mod_bool			enable_msg_match_mode;
	/**
	 * USE WITH CAUTION: setting this boolean to AL_TRUE allows setting the
	 * outbound ATU even after link is already started. DO NOT SET this
	 * boolean to AL_TRUE unless there have been NO traffic before calling
	 * al_mod_pcie_atu_region_set function
	 */
	al_mod_bool			enforce_ob_atu_region_set;
};

/**
 * @brief   get max number of ATU's
 * @param   pcie_port	pcie port handle
 * @param   direction	input: iATU direction (IB/OB)
 * @return  max number of ATU's
 */
unsigned int al_mod_pcie_atu_max_num_get(
	struct al_mod_pcie_port *pcie_port,
	enum al_mod_pcie_atu_dir direction);

/**
 * @brief   check if ATU region is valid
 * @param   atu_region	data structure that contains the region index and the
 *          translation parameters
 * @return  AL_TRUE if region is valid, AL_FALSE otherwise
 */
#define AL_PCIE_HAS_ATU_REGION_IS_VALID
al_mod_bool al_mod_pcie_atu_region_is_valid(struct al_mod_pcie_atu_region *atu_region);

/**
 * @brief   program internal ATU region entry
 * @param   pcie_port	pcie port handle
 * @param   atu_region	data structure that contains the region index and the
 *          translation parameters
 * @return  0 if no error
 */
int al_mod_pcie_atu_region_set(
	struct al_mod_pcie_port *pcie_port,
	struct al_mod_pcie_atu_region *atu_region);

/**
 * @brief  get internal ATU is enabled and base/target addresses
 * @param  pcie_port   pcie port handle
 * @param  direction   input: iATU direction (IB/OB)
 * @param  index       input: iATU index
 * @param  enable      output: AL_TRUE if the iATU is enabled
 * @param  base_addr   output: the iATU base address
 * @param  target_addr output: the iATU target address
 */
void al_mod_pcie_atu_region_get_fields(
	struct al_mod_pcie_port *pcie_port,
	enum al_mod_pcie_atu_dir direction, uint8_t index,
	al_mod_bool *enable, uint64_t *base_addr, uint64_t *target_addr);

/**
 * @brief  get internal ATU region fields
 * @param  pcie_port   pcie port handle
 * @param  direction   input: iATU direction (IB/OB)
 * @param  index       input: iATU index
 * @param  atu_region output: the iATU region fields struct
 */
void al_mod_pcie_atu_region_get(
	struct al_mod_pcie_port *pcie_port,
	enum al_mod_pcie_atu_dir direction,
	uint8_t index,
	struct al_mod_pcie_atu_region *atu_region);

/**
 * @brief   Configure axi io bar.
 *
 * This is an EP feature, enabling PCIe IO transaction to be captured if it fits
 * within start and end address, and then mapped to internal 4-byte
 * memRead/memWrite. Every hit to this bar will override size to 4 bytes.
 *
 * @param   pcie_port pcie port handle
 * @param   start the first address of the memory
 * @param   end the last address of the memory
 * @return
 */
void al_mod_pcie_axi_io_config(
	struct al_mod_pcie_port *pcie_port,
	al_mod_phys_addr_t start,
	al_mod_phys_addr_t end);

/************** Interrupt generation (Endpoint mode Only) API *****************/

enum al_mod_pcie_legacy_int_type{
	AL_PCIE_LEGACY_INTA = 0,
	AL_PCIE_LEGACY_INTB,
	AL_PCIE_LEGACY_INTC,
	AL_PCIE_LEGACY_INTD
};


/* @brief		generate FLR_PF_DONE message
 * @param  pcie_pf	pcie pf handle
 * @return		0 if no error found
 */
int al_mod_pcie_pf_flr_done_gen(struct al_mod_pcie_pf *pcie_pf);

/**
 * @brief		generate INTx Assert/DeAssert Message
 * @param  pcie_pf	pcie pf handle
 * @param  assert	when true, Assert Message is sent
 * @param  type		type of message (INTA, INTB, etc)
 * @return		0 if no error found
 */
int al_mod_pcie_legacy_int_gen(
	struct al_mod_pcie_pf		*pcie_pf,
	al_mod_bool				assert,
	enum al_mod_pcie_legacy_int_type	type);

/**
 * @brief		generate MSI interrupt
 * @param  pcie_pf	pcie pf handle
 * @param  vector	the vector index to send interrupt for.
 * @return		0 if no error found
 */
int al_mod_pcie_msi_int_gen(struct al_mod_pcie_pf *pcie_pf, uint8_t vector);

/**
 * @brief   configure MSIX capability
 * @param   pcie_pf	pcie pf handle
 * @param   msix_params	MSIX capability configuration parameters
 * @return  0 if no error found
 */
int al_mod_pcie_msix_config(
		struct al_mod_pcie_pf		*pcie_pf,
		struct al_mod_pcie_msix_params	*msix_params);

/**
 * @brief   check whether MSIX capability is enabled
 * @param   pcie_pf	pcie pf handle
 * @return  AL_TRUE if MSIX capability is enabled, AL_FALSE otherwise
 */
al_mod_bool al_mod_pcie_msix_enabled(struct al_mod_pcie_pf	*pcie_pf);

/**
 * @brief   check whether MSIX capability is masked
 * @param   pcie_pf	pcie pf handle
 * @return  AL_TRUE if MSIX capability is masked, AL_FALSE otherwise
 */
al_mod_bool al_mod_pcie_msix_masked(struct al_mod_pcie_pf *pcie_pf);

/******************** Advanced Error Reporting (AER) API **********************/

/**
 * @brief   configure EP physical function AER capability
 * @param   pcie_pf	pcie pf handle
 * @param   params	AER capability configuration parameters
 * @return  0 if no error found
 */
int al_mod_pcie_aer_config(
	struct al_mod_pcie_pf		*pcie_pf,
	struct al_mod_pcie_aer_params	*params);

/**
 * @brief   EP physical function AER uncorrectable errors get and clear
 * @param   pcie_pf	pcie pf handle
 * @return  bit mask of uncorrectable errors - see 'AL_PCIE_AER_UNCORR_*' for
 *          details
 */
unsigned int al_mod_pcie_aer_uncorr_get_and_clear(struct al_mod_pcie_pf	*pcie_pf);

/**
 * @brief   EP physical function AER correctable errors get and clear
 * @param   pcie_pf	pcie pf handle
 * @return  bit mask of correctable errors - see 'AL_PCIE_AER_CORR_*' for
 *          details
 */
unsigned int al_mod_pcie_aer_corr_get_and_clear(struct al_mod_pcie_pf	*pcie_pf);

/**
 * @brief   EP physical function AER get the header for
 *			the TLP corresponding to a detected error
 * @param   pcie_pf	pcie pf handle
 * @param   hdr		pointer to an array for getting the header
 */
void al_mod_pcie_aer_err_tlp_hdr_get(
	struct al_mod_pcie_pf	*pcie_pf,
	uint32_t		hdr[AL_PCIE_AER_ERR_TLP_HDR_NUM_DWORDS]);

/**
 * @brief   configure RC port AER capability
 * @param   pcie_port pcie port handle
 * @param   params	AER capability configuration parameters
 * @return  0 if no error found
 */
int al_mod_pcie_port_aer_config(
		struct al_mod_pcie_port		*pcie_port,
		struct al_mod_pcie_aer_params	*params);

/**
 * @brief   RC port AER uncorrectable errors get and clear
 * @param   pcie_port pcie port handle
 * @return  bit mask of uncorrectable errors - see 'AL_PCIE_AER_UNCORR_*' for
 *          details
 */
unsigned int al_mod_pcie_port_aer_uncorr_get_and_clear(
		struct al_mod_pcie_port		*pcie_port);

/**
 * @brief   RC port AER correctable errors get and clear
 * @param   pcie_port pcie port handle
 * @return  bit mask of correctable errors - see 'AL_PCIE_AER_CORR_*' for
 *          details
 */
unsigned int al_mod_pcie_port_aer_corr_get_and_clear(
		struct al_mod_pcie_port		*pcie_port);

/**
 * @brief   RC port AER get the header for
 *			the TLP corresponding to a detected error
 * @param   pcie_port pcie port handle
 * @param   hdr		pointer to an array for getting the header
 */
void al_mod_pcie_port_aer_err_tlp_hdr_get(
	struct al_mod_pcie_port		*pcie_port,
	uint32_t		hdr[AL_PCIE_AER_ERR_TLP_HDR_NUM_DWORDS]);

/******************** Loop-Back mode (RC and Endpoint modes) ******************/

/**
 * @brief   enter local pipe loop-back mode
 *  This mode will connect the pipe RX signals to TX.
 *  no need to start link when using this mode.
 *  Gen3 equalization must be disabled before enabling this mode
 *  The caller must make sure the port is ready to accept the TLPs it sends to
 *  itself. for example, BARs should be initialized before sending memory TLPs.
 *
 * @param   pcie_port pcie port handle
 * @return  0 if no error found
 */
int al_mod_pcie_local_pipe_loopback_enter(struct al_mod_pcie_port *pcie_port);

/**
 * @brief   exit local pipe loopback mode
 *
 * @param   pcie_port pcie port handle
 * @return  0 if no error found
 */
int al_mod_pcie_local_pipe_loopback_exit(struct al_mod_pcie_port *pcie_port);

/**
 * @brief   enter master remote loopback mode
 *  No need to configure the link partner to enter slave remote loopback mode
 *  as this should be done as response to special training sequence directives
 *  when master works in remote loopback mode.
 *  The caller must make sure the port is ready to accept the TLPs it sends to
 *  itself. for example, BARs should be initialized before sending memory TLPs.
 *
 * @param   pcie_port pcie port handle
 * @return  0 if no error found
 */
int al_mod_pcie_remote_loopback_enter(struct al_mod_pcie_port *pcie_port);

/**
 * @brief   exit remote loopback mode
 *
 * @param   pcie_port pcie port handle
 * @return  0 if no error found
 */
int al_mod_pcie_remote_loopback_exit(struct al_mod_pcie_port *pcie_port);

/******************** Interrupt forwarding ******************/
#define AL_HAL_HAS_PCIE_INT_FWD_ENABLE_DISABLE

enum al_mod_pcie_int_fwd_unit {
	AL_PCIE_INT_FWD_UNIT_ETH,
};

/**
 * @brief   disable legacy interrupt forwarding between a unit (in PCIE EP mode)
 * and the host - required when working with MSIX based interrupts
 *
 * @param   pcie_port			pcie port handle
 * @param   pf_num			PF number
 * @param   unit			type of unit
 * @param   unit_sub_index		index of the sub unit
 * @return  0 if no error found
 */
int al_mod_pcie_interrupt_forwarding_disable(struct al_mod_pcie_port *pcie_port, int pf_num,
	enum al_mod_pcie_int_fwd_unit unit, int unit_sub_index);

/**
 * @brief   enable legacy interrupt forwarding between a unit (in PCIE EP mode) and the host
 *
 * @param   pcie_port			pcie port handle
 * @param   pf_num			PF number
 * @param   unit			type of unit
 * @param   unit_sub_index		index of the sub unit
 *
 * @return  0 if no error found
 */
int al_mod_pcie_interrupt_forwarding_enable(struct al_mod_pcie_port *pcie_port, int pf_num,
	enum al_mod_pcie_int_fwd_unit unit, int unit_sub_index);

/********************** Parity Errors ************/

struct al_mod_pcie_core_parity_stats {
	unsigned int num_of_errors;

	/* PCIe SRAM's parity errors */
	al_mod_bool u0_ram2p;
	al_mod_bool u1_ram2p;
	al_mod_bool u7_ram2p;
	al_mod_bool u8_ram2p;
	al_mod_bool u11_ram2p;
	/*
	 * PCIe x4 SRAM's parity errors
	 * Relevant only in Rev1/2
	 */
	al_mod_bool u_ram_1p_sotbuf;
	al_mod_bool u0_ram_radm_qbuffer;
	al_mod_bool u3_qbuffer_0;
	al_mod_bool u3_qbuffer_1;
	al_mod_bool u9_decomp;
	al_mod_bool u6_ram2p;
	al_mod_bool u0_rbuf;
	al_mod_bool u3_qbuffer_2;
	/*
	 * PCIe x8 SRAM's parity errors
	 * Relevant only in Rev3
	 */
	al_mod_bool ram_1p_rbuf;
	al_mod_bool ram_2p_sotbuf;
	al_mod_bool u0_ram_radm_qbuffer_hdr;
	al_mod_bool u3_ram_radm_qbuffer_data_0;
	al_mod_bool u3_ram_radm_qbuffer_data_1;
	al_mod_bool u10_ram2p_0;
	al_mod_bool u10_ram2p_1;
	al_mod_bool u6_ram;
};

struct al_mod_pcie_axi_parity_stats {
	unsigned int num_of_errors;

	/* PCIe SRAM's parity errors */
	al_mod_bool u5_ram2p;
	al_mod_bool u4_ram2p;
	al_mod_bool u3_ram2p;
	al_mod_bool u2_ram2p;

	/*
	 * PCIe x4 SRAM's parity errors
	 * Relevant only in Rev1/2
	 */
	al_mod_bool u12_ram2p;
	al_mod_bool u10_ram2p;
};

/**
 * @brief   config pcie port ram parity interrupts
 *
 * @param   pcie_port pcie port handle
 * @param   enable True to unmask interrupts, false to mask
 * @return  0 if no error found
 */
int al_mod_pcie_port_ram_parity_int_config(
	struct al_mod_pcie_port *pcie_port,
	al_mod_bool enable);

/**
 * @brief   config pcie port AXI parity interrupts
 *
 * @param   pcie_port pcie port handle
 * @param   enable True to unmask interrupts, false to mask
 * @return  0 if no error found
 */
int al_mod_pcie_port_axi_parity_int_config(
	struct al_mod_pcie_port *pcie_port,
	al_mod_bool enable);

/**
 * @brief   read core parity error status
 *
 * @param   pcie_port pcie port handle
 * @param   stats pcie parity stats
 * @return  0 if no error found
 */
void al_mod_pcie_core_parity_stats_get(struct al_mod_pcie_port *pcie_port,
				   struct al_mod_pcie_core_parity_stats *stats);

/**
 * @brief   read axi parity error status
 *
 * @param   pcie_port pcie port handle
 * @param   stats pcie axi parity stats
 * @return  0 if no error found
 */
void al_mod_pcie_axi_parity_stats_get(struct al_mod_pcie_port *pcie_port,
				  struct al_mod_pcie_axi_parity_stats *stats);

/**
 * @brief   Get axi POS error latched address
 *
 * @param   pcie_port pcie port handle
 * @return  error address
 */
uint64_t al_mod_pcie_axi_pos_error_addr_get(struct al_mod_pcie_port *pcie_port);

/**
 * @brief   Get axi read data parity error latched address
 *
 * @param   pcie_port pcie port handle
 * @return  error address
 */
uint64_t al_mod_pcie_axi_read_data_parity_error_addr_get(struct al_mod_pcie_port *pcie_port);

/**
 * @brief   Get axi read completion error latched address
 *
 * @param   pcie_port pcie port handle
 * @return  error address
 */
uint64_t al_mod_pcie_axi_read_compl_error_addr_get(struct al_mod_pcie_port *pcie_port);

/**
 * @brief   Get axi read timeout error latched address
 *
 * @param   pcie_port pcie port handle
 * @return  error address
 */
uint64_t al_mod_pcie_axi_read_to_error_addr_get(struct al_mod_pcie_port *pcie_port);

/**
 * @brief   Get axi write completion error latched address
 *
 * @param   pcie_port pcie port handle
 * @return  error address
 */
uint64_t al_mod_pcie_axi_write_compl_error_addr_get(struct al_mod_pcie_port *pcie_port);

/**
 * @brief   Get axi write timeout error latched address
 *
 * @param   pcie_port pcie port handle
 * @return  error address
 */
uint64_t al_mod_pcie_axi_write_to_error_addr_get(struct al_mod_pcie_port *pcie_port);

#ifdef __cplusplus
}
#endif

#endif
/** @} end of grouppcie group */
