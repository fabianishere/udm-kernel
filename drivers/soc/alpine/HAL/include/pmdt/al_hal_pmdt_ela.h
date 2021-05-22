/*******************************************************************************
Copyright (C) 2016 Annapurna Labs Ltd.

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
 * @defgroup group_pmdt_ela_api API
 * Performance Monitoring Debug and Trace - ELA, Embedded Logic Analyzer
 * @ingroup group_pmdt_ela
 * @{
 * The ELA is part of a single PMU complex instance, embedded in a
 * specific HW core (there can be several ELAs in a single complex, e.g. 2).
 * ELA is composed of several trigger states (4/5). Each state can monitor a
 * specific signal group with a specific pattern filter at specific trigger
 * conditions. Comparison events can be counted in a state counter.
 * The user can choose whether to issue comparison trigger on signal comparison
 * or counter comparison. The comparison trigger leads to capturing selected
 * data into trace RAM. In addition, it may set specific values on output action
 * wires and jump to a different state. More details can be found at:
 * http://infocenter.arm.com/help/topic/com.arm.doc.100127_0100_00_en/index.html
 *
 * @file   al_hal_pmdt_ela.h
 */

#ifndef __AL_HAL_PMDT_ELA_H__
#define __AL_HAL_PMDT_ELA_H__

#include "al_hal_common.h"

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

/** Max interval of timestamp insertion into SRAM */
#define AL_PMDT_ELA_MAX_TS_INSERT_INT 15
/** Max signal width in words (128 bit) */
#define AL_PMDT_ELA_SIG_WIDTH 4
/** Max signal width in words (128 bit) */
#define AL_PMDT_ELA_RAM_MAX_ENTRIES  512
/** Number of ELA states */
#define AL_PMDT_ELA_STATE_NUM 5

/** trigger counter source */
enum al_pmdt_ela_counter_src {
	AL_ELA_CS_TRIG_MATCH = 0, /** counter incremented on trigger signal match */
	AL_ELA_CS_EVERY_CYCLE = 1, /** counter incremented every clock cycle */
};
/** trace capture control */
enum al_pmdt_ela_trace_cap_ctrl {
	AL_ELA_TCC_SIG_MATCH = 0, /** trace captured on trigger signal match */
	AL_ELA_TCC_CNT_MATCH = 1, /** trace captured on trigger counter match */
	AL_ELA_TCC_EVERY_CYCLE = 2, /** trace captured every clock cycle */
};
/** capture transaction id */
enum al_pmdt_ela_captid {
	AL_ELA_CAPTID_DISABLE = 0, /** disable use of captid for signal comparison*/
	AL_ELA_CAPTID_ON_MATCH = 1, /** capture ID on trigger signal match*/
	AL_ELA_CAPTID_COMP_TGT = 2, /** use captid as comparison target */
	AL_ELA_CAPTID_COMP_VAL = 3, /** use captid as comparison value */
};
/** comparison mode */
enum al_pmdt_ela_comp_mode {
	AL_ELA_COMP_MODE_SIGNAL = 0, /** compare selected signal */
	AL_ELA_COMP_MODE_COUNTER = 1, /** compare counter value */
};
/** comparison type */
enum al_pmdt_ela_comp_type {
	AL_ELA_COMP_DISABLE = 0, /** disable comparison*/
	AL_ELA_COMP_EQUAL = 1,			/** == */
	AL_ELA_COMP_GREATER = 2,		/** >  */
	AL_ELA_COMP_GREATER_EQUAL = 3,	/** >= */
	AL_ELA_COMP_NOT_EQUAL = 5,		/** != */
	AL_ELA_COMP_LESS = 6,			/** <  */
	AL_ELA_COMP_LESS_EQUAL = 7,	/** <= */
};
/** trigger states */
enum al_pmdt_ela_trigger_state {
	AL_ELA_TRIG_STATE_0 = 0x1,
	AL_ELA_TRIG_STATE_1 = 0x2,
	AL_ELA_TRIG_STATE_2 = 0x4,
	AL_ELA_TRIG_STATE_3 = 0x8,
	AL_ELA_TRIG_STATE_4 = 0x10,
	AL_ELA_TRIG_STATE_FINAL = 0x0,
};

/** ELA action */
struct al_pmdt_ela_action {
	unsigned int cttrigout; /** value to be driven on CTTRIGOUT[1:0] */
	al_bool stopclock; /** level to be driven on STOPCLOCK */
	al_bool trace_en; /** enable or disable trace */
	unsigned int elaoutput; /** value to be driven on ELAOUTPUT[3:0] */
};

/** ELA configuration parameters */
struct al_pmdt_ela_config {
	al_bool timestamp_en; /** enable timestamp insertion into SRAM trace */
	unsigned int ts_int;/**select bit from 16bit-interval-counter to insert ts*/
	unsigned int tcsel0; /**select bit from counter to log @ SRAM header tc[0]*/
	unsigned int tcsel1; /**select bit from counter to log @ SRAM header tc[1]*/
	al_bool st4_trace_en; /** enable independent trace for trigger state 4 */
	struct al_pmdt_ela_action init_action; /* initial action after ela_enable*/
};

/** ELA trigger state control */
struct al_pmdt_ela_trig_ctrl {
	enum al_pmdt_ela_comp_mode comp_mode;/**in signal mode counter is disabled*/
	enum al_pmdt_ela_comp_type comp_type;
	enum al_pmdt_ela_comp_mode alt_comp_mode; /** alternative comparison mode */
	enum al_pmdt_ela_comp_type alt_comp_type; /** alternative comparison type */
	enum al_pmdt_ela_counter_src counter_src;
	al_bool counter_rst; /** reset counter after trigger signal match */
	al_bool counter_clr; /** clear counter when moving to next state */
	al_bool counter_brk; /**counter match breaks states-loop and move to final*/
	unsigned int counter_comp; /** counter value to compare against */
	enum al_pmdt_ela_trace_cap_ctrl trace_cap_ctrl;
	enum al_pmdt_ela_captid captid;
};

/** ELA trigger state configuration parameters (for a specific state) */
struct al_pmdt_ela_trig_config {
	unsigned int signal_group; /** mask of monitored signal groups */
	struct al_pmdt_ela_trig_ctrl trig_ctrl;
	enum al_pmdt_ela_trigger_state next_state;
	struct al_pmdt_ela_action action;
	enum al_pmdt_ela_trigger_state alt_next_state;
	struct al_pmdt_ela_action alt_action;
	char ext_trig_mask; /** external trigger 8-bit mask value */
	char ext_trig_comp; /** external trigger 8-bit compare value */
	uint32_t sig_mask[AL_PMDT_ELA_SIG_WIDTH]; /**observed signal mask value */
	uint32_t sig_comp[AL_PMDT_ELA_SIG_WIDTH]; /**observed signal compare value*/
};

/** ELA trigger state info */
struct al_pmdt_ela_trig_state {
	enum al_pmdt_ela_trigger_state current_state;
	al_bool is_final; /* if is_final=true then state before final is reported */
	struct al_pmdt_ela_action action; /* last taken action */
	unsigned int counter; /* last counter value */
};

/** RAM entry params */
struct al_pmdt_ela_ram_entry {
	al_bool tcsel0; /** al_pmdt_ela_config.tcsel0 bit value */
	al_bool tcsel1; /** al_pmdt_ela_config.tcsel1 bit value */
	al_bool st4_ovrd; /** state 4 trace data overwritten data from other state*/
	unsigned int trig_state_num; /* trigger state that generate this entry */
	al_bool ts; /* timestamp entry. (0 - regular state payload) */
	uint64_t payload_0; /** payload[0:63] */
	uint64_t payload_1; /** payload[64:127]-relevant only if sig_grp_width=128*/
};

/** device info */
struct al_pmdt_ela_dev_info {
	unsigned int revision; /**rev[16bit] | type[8bit] | trace header rev[8bit]*/
	unsigned int peripheral_id;
	unsigned int component_id;
	al_bool snid_dbg; /** secure non-invasive debug enabled */
	al_bool sid_dbg; /** secure invasive debug enabled */
	al_bool nsnid_dbg; /** non-secure non-invasive debug enabled */
	al_bool nsid_dbg; /** non-secure invasive debug enabled */
	unsigned int counter_width; /** e.g. 32 */
	unsigned int num_trig_states; /** number of trigger states - 4 or 5 */
	al_bool st4_trace_en; /** state 4 independent trace is implemented */
	unsigned int sig_grp_width; /** 64 or 128-bit signal width */
	unsigned int sig_grps_num; /** number of signal groups, e.g. 12 */
	al_bool cond_trig_en; /** conditional trigger state feature enables */
	unsigned int id_capt_size; /** captured transaction id bit-width 2-32 */
	unsigned int ram_addr_size; /**2-30bits. RAM size = 2^ram_addr_size entries
							   entry = 1 header byte + (sig_grp_width/8) bytes*/
};

/** ELA context */
struct al_pmdt_ela {
	const char *name;
	unsigned int id;
	struct al_pmdt_ela_dev_info info;
	al_bool stopped;
	al_bool locked;
	void __iomem *regs;
};

/**
 * Initialize the ELA handle
 *
 * @param pmdt_ela
 *		An allocated, non-initialized instance
 * @param core_name
 *		The name of the core ELA is integrated into
 *		Upper layer must keep string area
 * @param id
 *      The ELA instance ID
 * @param pmu_cmplx_base
 *		The base address of the PMU complex registers
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_pmdt_ela_handle_init(
		struct al_pmdt_ela *pmdt_ela,
		const char *core_name,
		unsigned int id,
		void __iomem *pmu_cmplx_base);

/**
 * Configure the ELA
 *
 * Note. ELA can be configured only when it is stopped
 *
 * @param pmdt_ela
 *		ELA handle
 * @param config
 *		ELA config parameters
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_pmdt_ela_config(
		struct al_pmdt_ela *pmdt_ela,
		const struct al_pmdt_ela_config *config);

/**
 * Configure a specific trigger state
 *
 * Note. trigger state can be configured only when ELA is stopped
 *
 * @param pmdt_ela
 *		ELA handle
 * @param trig_state_num
 *		state number to be configured
 * @param config
 *		trigger state config parameters
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_pmdt_ela_trigger_state_config(
		struct al_pmdt_ela *pmdt_ela,
		unsigned int trig_state_num,
		const struct al_pmdt_ela_trig_config *config);

/**
 * Start/stop ELA
 *
 * @param pmdt_ela
 *		ELA handle
 * @param is_start
 *		0/1 - stop/start
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_pmdt_ela_start(
		struct al_pmdt_ela *pmdt_ela,
		al_bool is_start);

/**
 * Lock writing into ELA registers
 *
 * @param pmdt_ela
 *		ELA handle
 * @param is_lock
 *		true/false - lock/unlock
 */
void al_pmdt_ela_write_lock(
		struct al_pmdt_ela *pmdt_ela,
		al_bool is_lock);

/**
 * Retrieve write lock state
 *
 * @param pmdt_ela
 *		ELA handle
 *
 * @return	write lock state
 */
al_bool al_pmdt_ela_is_locked(struct al_pmdt_ela *pmdt_ela);

/**
 * Retrieve current triggering mechanism state
 *
 * @param pmdt_ela
 *		ELA handle
 * @param trig_state
 *		Allocated trig_state instance (out)
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_pmdt_ela_trig_state_get(
		struct al_pmdt_ela *pmdt_ela,
		struct al_pmdt_ela_trig_state *trig_state);

/**
 * Retrieve the transaction id on the trigger signal match of an
 * address request in a trigger state
 *
 * Note. can be called only when al_pmdt_ela_dev_info.id_capt_size > 0
 * and ELA is stopped
 *
 * @param pmdt_ela
 *		ELA handle
 * @param captid
 *		captured transaction id
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_pmdt_ela_captid_get(
		struct al_pmdt_ela *pmdt_ela,
		unsigned int *captid);

/**
 * Retrieve RAM read pointer
 *
 * Note. can be called only when ELA is stopped
 *
 * @param pmdt_ela
 *		ELA handle
 * @param addr
 *		current RAM read address
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_pmdt_ela_ram_read_ptr_get(
		struct al_pmdt_ela *pmdt_ela,
		unsigned int *addr);

/**
 * Retrieve RAM write pointer
 *
 * Note. can be called only when ELA is stopped
 *
 * @param pmdt_ela
 *		ELA handle
 * @param addr
 *		current RAM write address
 * @param is_wrap
 *		indicates whether collected trace data caused RAM wrap around
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_pmdt_ela_ram_write_ptr_get(
		struct al_pmdt_ela *pmdt_ela,
		unsigned int *addr,
		al_bool *is_wrap);

/**
 * Read a single RAM entry
 *
 * Note. can be called only when ELA is stopped. Read pointer auto-incremented.
 *
 * @param pmdt_ela
 *		ELA handle
 * @param is_next
 *		use current pointer location (addr is ignored)
 * @param addr
 *		specific entry address (is_next should be false)
 * @param entry
 *		Allocated instance of entry (out), if NULL the function will only set
 *		read pointer to addr
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_pmdt_ela_ram_read(
		struct al_pmdt_ela *pmdt_ela,
		al_bool is_next,
		unsigned int addr,
		struct al_pmdt_ela_ram_entry *entry);

/**
 * Write a single RAM entry
 *
 * Note. can be called only when ELA is stopped. Write pointer auto-incremented.
 *
 * @param pmdt_ela
 *		ELA handle
 * @param is_next
 *		use current pointer location (addr is ignored)
 * @param addr
 *		specific entry address (is_next should be false)
 * @param entry
 *		entry params, if NULL the function will only set write pointer to addr
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_pmdt_ela_ram_write(
		struct al_pmdt_ela *pmdt_ela,
		al_bool is_next,
		unsigned int addr,
		const struct al_pmdt_ela_ram_entry *entry);

/**
 * Set a specific value on action wires (meant for integration testing)
 *
 * Note. can be called only when ELA is stopped
 *
 * @param pmdt_ela
 *		ELA handle
 * @param action
 *		action value. 'trace_en' field is ignored
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_pmdt_ela_action_set(
		struct al_pmdt_ela *pmdt_ela,
		const struct al_pmdt_ela_action *action);

/**
 * Get the current external trigger mask value (meant for integration testing)
 *
 * Note. can be called only when ELA is stopped
 *
 * @param pmdt_ela
 *		ELA handle
 * @param trig_mask
 *		current trigger mask value (out)
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_pmdt_ela_ext_trig_get(
		struct al_pmdt_ela *pmdt_ela,
		char *trig_mask);

/**
 * Retrieve device information
 *
 * @param pmdt_ela
 *		ELA handle
 * @param info
 *		Allocated device info instance (out)
 */
void al_pmdt_ela_device_info_get(
		struct al_pmdt_ela *pmdt_ela,
		struct al_pmdt_ela_dev_info *info);

/* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */

#endif /* __AL_HAL_PMDT_ELA_H__ */
/** @} end of group_pmdt_ela */


