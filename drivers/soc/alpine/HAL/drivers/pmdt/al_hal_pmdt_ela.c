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
 *  @{
 * @file   al_hal_pmdt_ela.c
 *
 * @brief  PMDT Embedded Logic Analyzer HAL functions
 *
 */

#include "al_hal_pmdt_pmu_complex_regs.h"
#include "al_hal_pmdt_ela.h"
#include "al_hal_cxela500_regs.h"
#include "al_hal_pmdt_common.h"

/* RAM entry header fields */
#define AL_PMDT_ELA_RAM_HEADER_TCSEL1 (1<<7)
#define AL_PMDT_ELA_RAM_HEADER_TCSEL0 (1<<6)
#define AL_PMDT_ELA_RAM_HEADER_ST4_OVRD (1<<5)
#define AL_PMDT_ELA_RAM_HEADER_STATE_MASK 0x1C
#define AL_PMDT_ELA_RAM_HEADER_STATE_SHIFT 2
#define AL_PMDT_ELA_RAM_HEADER_IS_TIMESTAMP (1<<1)

/* write lock status bit in LSR register */
enum al_pmdt_ela_write_lock_status {
	AL_PMDT_ELA_WRITE_PERMITTED = 0x1,
	AL_PMDT_ELA_WRITE_LOCKED = 0x3,
};

/**
 * Set action output signals
 *
 * @param action_reg
 *		action register address
 * @param
 *		action parameters
 */
static void set_action(
		uint32_t *action_reg,
		const struct al_pmdt_ela_action *action)
{
	uint32_t val = 0;

	al_assert(AL_REG_FIELD_RANGE_VALID(action->elaoutput,
			CXELA500_TRIGSTATE_ACTION_ELAOUTPUT_MASK,
			CXELA500_TRIGSTATE_ACTION_ELAOUTPUT_SHIFT));
	al_assert(AL_REG_FIELD_RANGE_VALID(action->cttrigout,
			CXELA500_TRIGSTATE_ACTION_CTTRIGOUT_MASK,
			CXELA500_TRIGSTATE_ACTION_CTTRIGOUT_SHIFT));

	AL_REG_FIELD_SET(val,
			CXELA500_TRIGSTATE_ACTION_CTTRIGOUT_MASK,
			CXELA500_TRIGSTATE_ACTION_CTTRIGOUT_SHIFT,
			action->cttrigout);
	AL_REG_MASK_SET_VAL(val,
			CXELA500_TRIGSTATE_ACTION_STOPCLOCK,
			action->stopclock);
	AL_REG_MASK_SET_VAL(val,
			CXELA500_TRIGSTATE_ACTION_TRACE,
			action->trace_en);
	AL_REG_FIELD_SET(val,
			CXELA500_TRIGSTATE_ACTION_ELAOUTPUT_MASK,
			CXELA500_TRIGSTATE_ACTION_ELAOUTPUT_SHIFT,
			action->elaoutput);
	al_reg_write32(action_reg, val);
}

/**
 * Initialize the ELA handle
 */
int al_pmdt_ela_handle_init(
		struct al_pmdt_ela *pmdt_ela,
		const char *core_name,
		unsigned int id,
		void __iomem *pmu_cmplx_base)
{
	struct al_pmdt_pmu_complex_regs *cmplx_regs;
	struct al_cxela500_regs *regs;

	al_assert(pmdt_ela);
	al_assert(pmu_cmplx_base);
	al_assert(id < AL_PMDT_PMU_CMPLX_ELA_NUM);

	if (core_name)
		pmdt_ela->name = core_name;
	else
		pmdt_ela->name = "";
	pmdt_ela->id = id;

	cmplx_regs =  (struct al_pmdt_pmu_complex_regs *)pmu_cmplx_base;
	pmdt_ela->regs = id == 0 ?
			(struct al_cxela500_regs *)&cmplx_regs->ela_0 :
			(struct al_cxela500_regs *)&cmplx_regs->ela_1;

	/* check enabled & locked status */
	regs = (struct al_cxela500_regs *)pmdt_ela->regs;
	pmdt_ela->stopped = !(al_reg_read32(&regs->ctrl.ctrl) &
			CXELA500_CONTROL_CTRL_RUN);
	pmdt_ela->locked = (al_reg_read32(&regs->sw_lock.lsr) ==
			AL_PMDT_ELA_WRITE_LOCKED);
	/* retrieve device info */
	al_pmdt_ela_device_info_get(pmdt_ela, &pmdt_ela->info);

	al_dbg("pmdt ela [%s:%d]: init\n",
			pmdt_ela->name, pmdt_ela->id);

	return 0;
}

/**
 * Configure the ELA
 */
int al_pmdt_ela_config(
		struct al_pmdt_ela *pmdt_ela,
		const struct al_pmdt_ela_config *config)
{
	struct al_cxela500_regs *regs;
	uint32_t val = 0;

	al_assert(pmdt_ela);
	al_assert(config);
	al_assert(AL_REG_FIELD_RANGE_VALID(config->ts_int,
			CXELA500_CONTROL_TIMECTRL_TSINT_MASK,
			CXELA500_CONTROL_TIMECTRL_TSINT_SHIFT));
	al_assert(AL_REG_FIELD_RANGE_VALID(config->tcsel0,
			CXELA500_CONTROL_TIMECTRL_TCSEL0_MASK,
			CXELA500_CONTROL_TIMECTRL_TCSEL0_SHIFT));
	al_assert(AL_REG_FIELD_RANGE_VALID(config->tcsel1,
			CXELA500_CONTROL_TIMECTRL_TCSEL1_MASK,
			CXELA500_CONTROL_TIMECTRL_TCSEL1_SHIFT));
	if (pmdt_ela->locked || !pmdt_ela->stopped) {
		al_err("pmdt ela [%s:%d] %s: illegal state\n",
				pmdt_ela->name, pmdt_ela->id, __func__);
		return -EPERM;
	}

	regs = (struct al_cxela500_regs *)pmdt_ela->regs;

	/* configure timestamp insertions */
	AL_REG_MASK_SET_VAL(val,
			CXELA500_CONTROL_TIMECTRL_TSEN,
			config->timestamp_en);
	AL_REG_FIELD_SET(val,
			CXELA500_CONTROL_TIMECTRL_TSINT_MASK,
			CXELA500_CONTROL_TIMECTRL_TSINT_SHIFT,
			config->ts_int);
	AL_REG_FIELD_SET(val,
			CXELA500_CONTROL_TIMECTRL_TCSEL0_MASK,
			CXELA500_CONTROL_TIMECTRL_TCSEL0_SHIFT,
			config->tcsel0);
	AL_REG_FIELD_SET(val,
			CXELA500_CONTROL_TIMECTRL_TCSEL1_MASK,
			CXELA500_CONTROL_TIMECTRL_TCSEL1_SHIFT,
			config->tcsel1);
	al_reg_write32(&regs->ctrl.timectrl, val);

	/* configure state 4 independent trace */
	al_reg_write32(&regs->ctrl.tssr, config->st4_trace_en ?
			AL_ELA_TRIG_STATE_4 : 0);

	/* configure pre-trigger action */
	set_action(&regs->ctrl.ptaction, &config->init_action);

	return 0;
}

/**
 * Configure a specific trigger state
 */
int al_pmdt_ela_trigger_state_config(
		struct al_pmdt_ela *pmdt_ela,
		unsigned int trig_state_num,
		const struct al_pmdt_ela_trig_config *config)
{
	struct al_cxela500_regs *regs;
	struct al_cxela500_trig_state *ts_reg;
	uint32_t val = 0;
	int i;

	al_assert(pmdt_ela);
	al_assert(AL_BIT(trig_state_num) <=
			(pmdt_ela->info.num_trig_states == 5 ?
					AL_ELA_TRIG_STATE_4 : AL_ELA_TRIG_STATE_3));
	al_assert(config);
	al_assert(config->signal_group < AL_BIT(pmdt_ela->info.sig_grps_num));
	if (pmdt_ela->locked || !pmdt_ela->stopped) {
		al_err("pmdt ela [%s:%d] %s: illegal state\n",
				pmdt_ela->name, pmdt_ela->id, __func__);
		return -EPERM;
	}

	regs = (struct al_cxela500_regs *)pmdt_ela->regs;
	ts_reg = &regs->trig_state[trig_state_num];

	/* select signal groups */
	al_reg_write32(&ts_reg->sigsel, config->signal_group);

	/* set trigger control */
	AL_REG_FIELD_SET(val,
			CXELA500_TRIGSTATE_TRIGCTRL_COMP_MASK,
			CXELA500_TRIGSTATE_TRIGCTRL_COMP_SHIFT,
			config->trig_ctrl.comp_type);
	AL_REG_MASK_SET_VAL(val,
			CXELA500_TRIGSTATE_TRIGCTRL_COMPSEL,
			config->trig_ctrl.comp_mode);
	AL_REG_MASK_SET_VAL(val,
			CXELA500_TRIGSTATE_TRIGCTRL_WATCHRST,
			config->trig_ctrl.counter_rst);
	AL_REG_MASK_SET_VAL(val,
			CXELA500_TRIGSTATE_TRIGCTRL_COUNTSRC,
			config->trig_ctrl.counter_src);
	AL_REG_FIELD_SET(val,
			CXELA500_TRIGSTATE_TRIGCTRL_TRACE_MASK,
			CXELA500_TRIGSTATE_TRIGCTRL_TRACE_SHIFT,
			config->trig_ctrl.trace_cap_ctrl);
	AL_REG_MASK_SET_VAL(val,
			CXELA500_TRIGSTATE_TRIGCTRL_COUNTCLR,
			config->trig_ctrl.counter_clr);
	AL_REG_MASK_SET_VAL(val,
			CXELA500_TRIGSTATE_TRIGCTRL_COUNTBRK,
			config->trig_ctrl.counter_brk);
	AL_REG_FIELD_SET(val,
			CXELA500_TRIGSTATE_TRIGCTRL_CAPTID_MASK,
			CXELA500_TRIGSTATE_TRIGCTRL_CAPTID_SHIFT,
			config->trig_ctrl.captid);
	AL_REG_FIELD_SET(val,
			CXELA500_TRIGSTATE_TRIGCTRL_ALTCOMP_MASK,
			CXELA500_TRIGSTATE_TRIGCTRL_ALTCOMP_SHIFT,
			config->trig_ctrl.alt_comp_type);
	AL_REG_MASK_SET_VAL(val,
			CXELA500_TRIGSTATE_TRIGCTRL_ALTCOMPSEL,
			config->trig_ctrl.alt_comp_mode);
	al_reg_write32(&ts_reg->trigctrl, val);
	al_reg_write32(&ts_reg->countcomp, config->trig_ctrl.counter_comp);

	/* set next states & actions */
	al_reg_write32(&ts_reg->nextstate, config->next_state);
	set_action(&ts_reg->action, &config->action);
	al_reg_write32(&ts_reg->altnextstate, config->alt_next_state);
	set_action(&ts_reg->altaction, &config->alt_action);

	/* set external triggers pattern */
	al_reg_write32(&ts_reg->extmask, (uint32_t)config->ext_trig_mask);
	al_reg_write32(&ts_reg->extcomp, (uint32_t)config->ext_trig_comp);

	/* set signal pattern */
	for (i = 0; i < AL_PMDT_ELA_SIG_WIDTH; i++) {
		al_reg_write32(&ts_reg->sigmask[i], config->sig_mask[i]);
		al_reg_write32(&ts_reg->sigcomp[i], config->sig_comp[i]);
	}

	return 0;
}

/**
 * Start/stop ELA
 */
int al_pmdt_ela_start(
		struct al_pmdt_ela *pmdt_ela,
		al_bool is_start)
{
	struct al_cxela500_regs *regs;

	al_assert(pmdt_ela);

	if (pmdt_ela->locked) {
		al_err("pmdt ela [%s:%d] %s: illegal state\n",
				pmdt_ela->name, pmdt_ela->id, __func__);
		return -EPERM;
	}

	regs = (struct al_cxela500_regs *)pmdt_ela->regs;
	al_reg_write32(&regs->ctrl.ctrl, is_start);

	/* change state */
	pmdt_ela->stopped = !is_start;

	return 0;
}

/**
 * Lock writing into ELA registers
 */
void al_pmdt_ela_write_lock(
		struct al_pmdt_ela *pmdt_ela,
		al_bool is_lock)
{
	struct al_cxela500_regs *regs;

	al_assert(pmdt_ela);

	regs = (struct al_cxela500_regs *)pmdt_ela->regs;
	/* to unlock we use 0xC5ACCE55, any other value locks other registers */
	al_reg_write32(&regs->sw_lock.lar, is_lock ? 0x1 : 0xC5ACCE55);

	/* change state */
	pmdt_ela->locked = is_lock;
}

/**
 * Retrieve write lock state
 */
al_bool al_pmdt_ela_is_locked(struct al_pmdt_ela *pmdt_ela)
{
	struct al_cxela500_regs *regs;
	al_bool is_lock;

	al_assert(pmdt_ela);

	regs = (struct al_cxela500_regs *)pmdt_ela->regs;
	/* to unlock we use 0xC5ACCE55, any other value locks other registers */
	is_lock = (al_reg_read32(&regs->sw_lock.lsr) ==
			AL_PMDT_ELA_WRITE_LOCKED);

	/* verify state */
	al_assert(pmdt_ela->locked == is_lock);

	return is_lock;
}

/**
 * Retrieve current triggering mechanism state
 */
int al_pmdt_ela_trig_state_get(
		struct al_pmdt_ela *pmdt_ela,
		struct al_pmdt_ela_trig_state *trig_state)
{
	struct al_cxela500_regs *regs;
	uint32_t val;

	al_assert(pmdt_ela);
	al_assert(trig_state);

	regs = (struct al_cxela500_regs *)pmdt_ela->regs;

	val = al_reg_read32(&regs->cur_state.ctsr);
	trig_state->current_state = AL_REG_FIELD_GET(val,
			CXELA500_CURSTATE_CTSR_CTSR_MASK,
			CXELA500_CURSTATE_CTSR_CTSR_SHIFT);
	trig_state->is_final = AL_REG_MASK_IS_SET(val,
			CXELA500_CURSTATE_CTSR_FINAL_STATE);

	/* CCVR & CAVR are latched after CTSR reading, hence order is important */
	al_data_memory_barrier();

	trig_state->counter = al_reg_read32(&regs->cur_state.ccvr);

	val = al_reg_read32(&regs->cur_state.cavr);
	trig_state->action.cttrigout = AL_REG_FIELD_GET(val,
			CXELA500_TRIGSTATE_ACTION_CTTRIGOUT_MASK,
			CXELA500_TRIGSTATE_ACTION_CTTRIGOUT_SHIFT);
	trig_state->action.stopclock = AL_REG_MASK_IS_SET(val,
			CXELA500_TRIGSTATE_ACTION_STOPCLOCK);
	trig_state->action.trace_en = AL_REG_MASK_IS_SET(val,
			CXELA500_TRIGSTATE_ACTION_TRACE);
	trig_state->action.elaoutput = AL_REG_FIELD_GET(val,
			CXELA500_TRIGSTATE_ACTION_ELAOUTPUT_MASK,
			CXELA500_TRIGSTATE_ACTION_ELAOUTPUT_SHIFT);

	return 0;
}

/**
 * Retrieve the transaction id on the trigger signal match of an
 * address request in a trigger state
 */
int al_pmdt_ela_captid_get(
		struct al_pmdt_ela *pmdt_ela,
		unsigned int *captid)
{
	struct al_cxela500_regs *regs;

	al_assert(pmdt_ela);
	al_assert(captid);
	al_assert(pmdt_ela->info.id_capt_size);

	if (!pmdt_ela->stopped) {
		al_err("pmdt ela [%s:%d] %s: illegal state\n",
				pmdt_ela->name, pmdt_ela->id, __func__);
		return -EPERM;
	}

	regs = (struct al_cxela500_regs *)pmdt_ela->regs;
	*captid = al_reg_read32(&regs->cur_state.rdcaptid);

	return 0;
}

/**
 * Retrieve RAM read pointer
 */
int al_pmdt_ela_ram_read_ptr_get(
		struct al_pmdt_ela *pmdt_ela,
		unsigned int *addr)
{
	struct al_cxela500_regs *regs;

	al_assert(pmdt_ela);
	al_assert(addr);

	if (!pmdt_ela->stopped) {
		al_err("pmdt ela [%s:%d] %s: illegal state\n",
				pmdt_ela->name, pmdt_ela->id, __func__);
		return -EPERM;
	}

	regs = (struct al_cxela500_regs *)pmdt_ela->regs;
	*addr = al_reg_read32(&regs->ram.rrar);

	return 0;
}

/**
 * Retrieve RAM write pointer
 */
int al_pmdt_ela_ram_write_ptr_get(
		struct al_pmdt_ela *pmdt_ela,
		unsigned int *addr,
		al_bool *is_wrap)
{
	struct al_cxela500_regs *regs;
	uint32_t val;

	al_assert(pmdt_ela);
	al_assert(addr);
	al_assert(is_wrap);

	if (!pmdt_ela->stopped) {
		al_err("pmdt ela [%s:%d] %s: illegal state\n",
				pmdt_ela->name, pmdt_ela->id, __func__);
		return -EPERM;
	}

	regs = (struct al_cxela500_regs *)pmdt_ela->regs;
	val = al_reg_read32(&regs->ram.rwar);

	*addr = AL_REG_FIELD_GET(val,
			CXELA500_RAM_RWAR_RWA_MASK,
			CXELA500_RAM_RWAR_RWA_SHIFT);
	*is_wrap = AL_REG_MASK_IS_SET(val,
			CXELA500_RAM_RWAR_WRAP);

	return 0;
}

/**
 * Read a single RAM entry
 */
int al_pmdt_ela_ram_read(
		struct al_pmdt_ela *pmdt_ela,
		al_bool is_next,
		unsigned int addr,
		struct al_pmdt_ela_ram_entry *entry)
{
	struct al_cxela500_regs *regs;
	uint32_t val;

	al_assert(pmdt_ela);
	al_assert(addr < AL_BIT(pmdt_ela->info.ram_addr_size));

	if (pmdt_ela->locked || !pmdt_ela->stopped) {
		al_err("pmdt ela [%s:%d] %s: illegal state\n",
				pmdt_ela->name, pmdt_ela->id, __func__);
		return -EPERM;
	}

	regs = (struct al_cxela500_regs *)pmdt_ela->regs;
	/* read sequence (order is important):
	 * - write address to rrar
	 * - read once rrdr: header byte
	 * - read twice rrdr: 2 payload words (start from LSW)
	 * - if group_width = 128, read two more times rrdr for 2 upper payload */
	if (!is_next) {
		al_reg_write32(&regs->ram.rrar, addr);
		al_data_memory_barrier();
	}

	if (!entry)
		return 0;

	val = al_reg_read32(&regs->ram.rrdr);
	/* extract header fields */
	entry->tcsel0 = AL_REG_MASK_IS_SET(val, AL_PMDT_ELA_RAM_HEADER_TCSEL0);
	entry->tcsel1 = AL_REG_MASK_IS_SET(val, AL_PMDT_ELA_RAM_HEADER_TCSEL1);
	entry->st4_ovrd = AL_REG_MASK_IS_SET(val, AL_PMDT_ELA_RAM_HEADER_ST4_OVRD);
	entry->ts = AL_REG_MASK_IS_SET(val, AL_PMDT_ELA_RAM_HEADER_IS_TIMESTAMP);
	entry->trig_state_num = AL_REG_FIELD_GET(val,
			AL_PMDT_ELA_RAM_HEADER_STATE_MASK,
			AL_PMDT_ELA_RAM_HEADER_STATE_SHIFT);
	al_data_memory_barrier();

	entry->payload_0 =
			al_hal_pmdt_read64_latched(&regs->ram.rrdr, &regs->ram.rrdr);
	al_data_memory_barrier();

	if (pmdt_ela->info.sig_grp_width == 128)
		entry->payload_1 =
				al_hal_pmdt_read64_latched(&regs->ram.rrdr, &regs->ram.rrdr);

	return 0;
}

/**
 * Write a single RAM entry
 */
int al_pmdt_ela_ram_write(
		struct al_pmdt_ela *pmdt_ela,
		al_bool is_next,
		unsigned int addr,
		const struct al_pmdt_ela_ram_entry *entry)
{
	struct al_cxela500_regs *regs;
	uint32_t val = 0;

	al_assert(pmdt_ela);
	al_assert(addr < AL_BIT(pmdt_ela->info.ram_addr_size));

	if (pmdt_ela->locked || !pmdt_ela->stopped) {
		al_err("pmdt ela [%s:%d] %s: illegal state\n",
				pmdt_ela->name, pmdt_ela->id, __func__);
		return -EPERM;
	}

	regs = (struct al_cxela500_regs *)pmdt_ela->regs;
	/* write sequence (order is important):
	 * - write address to rwar
	 * - write once rwdr: header byte
	 * - write twice rwdr: 2 payload words (start from LSW)
	 * - if group_width = 128, write two more times rwdr for 2 upper payload */
	if (!is_next) {
		al_reg_write32(&regs->ram.rwar, addr);
		al_data_memory_barrier();
	}

	if (!entry)
		return 0;

	AL_REG_MASK_SET_VAL(val, AL_PMDT_ELA_RAM_HEADER_TCSEL0, entry->tcsel0);
	AL_REG_MASK_SET_VAL(val, AL_PMDT_ELA_RAM_HEADER_TCSEL1, entry->tcsel1);
	AL_REG_MASK_SET_VAL(val, AL_PMDT_ELA_RAM_HEADER_ST4_OVRD, entry->st4_ovrd);
	AL_REG_MASK_SET_VAL(val, AL_PMDT_ELA_RAM_HEADER_IS_TIMESTAMP, entry->ts);
	AL_REG_FIELD_SET(val,
				AL_PMDT_ELA_RAM_HEADER_STATE_MASK,
				AL_PMDT_ELA_RAM_HEADER_STATE_SHIFT,
				entry->trig_state_num);
	al_reg_write32(&regs->ram.rwdr, val);
	al_data_memory_barrier();

	al_hal_pmdt_write64_latched(entry->payload_0,
			&regs->ram.rwdr, &regs->ram.rwdr);
	al_data_memory_barrier();

	if (pmdt_ela->info.sig_grp_width == 128)
		al_hal_pmdt_write64_latched(entry->payload_1,
				&regs->ram.rwdr, &regs->ram.rwdr);

	return 0;
}

/**
 * Set a specific value on action wires (meant for integration testing)
 */
int al_pmdt_ela_action_set(
		struct al_pmdt_ela *pmdt_ela,
		const struct al_pmdt_ela_action *action)
{
	struct al_cxela500_regs *regs;

	al_assert(pmdt_ela);
	al_assert(action);

	if (pmdt_ela->locked || !pmdt_ela->stopped) {
		al_err("pmdt ela [%s:%d] %s: illegal state\n",
				pmdt_ela->name, pmdt_ela->id, __func__);
		return -EPERM;
	}

	regs = (struct al_cxela500_regs *)pmdt_ela->regs;
	set_action(&regs->intg_mode.ittrigout, action);

	return 0;
}

/**
 * Get the current external trigger mask value (meant for integration testing)
 */
int al_pmdt_ela_ext_trig_get(
		struct al_pmdt_ela *pmdt_ela,
		char *trig_mask)
{
	struct al_cxela500_regs *regs;

	al_assert(pmdt_ela);
	al_assert(trig_mask);

	if (pmdt_ela->locked || !pmdt_ela->stopped) {
		al_err("pmdt ela [%s:%d] %s: illegal state\n",
				pmdt_ela->name, pmdt_ela->id, __func__);
		return -EPERM;
	}

	regs = (struct al_cxela500_regs *)pmdt_ela->regs;
	*trig_mask = al_reg_read32(&regs->intg_mode.ittrigin);

	return 0;
}

/**
 * Retrieve device information
 */
void al_pmdt_ela_device_info_get(
		struct al_pmdt_ela *pmdt_ela,
		struct al_pmdt_ela_dev_info *info)
{
	struct al_cxela500_regs *regs;
	uint32_t val;

	al_assert(pmdt_ela);
	al_assert(info);

	regs = (struct al_cxela500_regs *)pmdt_ela->regs;

	/* compose revision, peripheral & component ids */
	info->revision =
			(uint8_t)al_reg_read32(&regs->device.devid) |
			(uint8_t)al_reg_read32(&regs->device.devtype) << 8 |
			(uint16_t)al_reg_read32(&regs->device.devarch) << 16;
	info->peripheral_id =
			(uint8_t)al_reg_read32(&regs->id.PIDR0) |
			(uint8_t)al_reg_read32(&regs->id.PIDR1) << 8 |
			(uint8_t)al_reg_read32(&regs->id.PIDR2) << 16 |
			(uint8_t)al_reg_read32(&regs->id.PIDR3) << 24;
	info->component_id =
				(uint8_t)al_reg_read32(&regs->id.CIDR0) |
				(uint8_t)al_reg_read32(&regs->id.CIDR1) << 8 |
				(uint8_t)al_reg_read32(&regs->id.CIDR2) << 16 |
				(uint8_t)al_reg_read32(&regs->id.CIDR3) << 24;

	/* get debug capabilities */
	val = al_reg_read32(&regs->authstatus);
	info->nsid_dbg = AL_REG_FIELD_GET(val,
			CXELA500_AUTHSTATUS_AUTHSTATUS_NSID_MASK,
			CXELA500_AUTHSTATUS_AUTHSTATUS_NSID_SHIFT) & 0x1;
	info->nsnid_dbg = AL_REG_FIELD_GET(val,
			CXELA500_AUTHSTATUS_AUTHSTATUS_NSNID_MASK,
			CXELA500_AUTHSTATUS_AUTHSTATUS_NSNID_SHIFT) & 0x1;
	info->sid_dbg = AL_REG_FIELD_GET(val,
			CXELA500_AUTHSTATUS_AUTHSTATUS_SID_MASK,
			CXELA500_AUTHSTATUS_AUTHSTATUS_SID_SHIFT) & 0x1;
	info->snid_dbg = AL_REG_FIELD_GET(val,
			CXELA500_AUTHSTATUS_AUTHSTATUS_SNID_MASK,
			CXELA500_AUTHSTATUS_AUTHSTATUS_SNID_SHIFT) & 0x1;

	/* get HW configuration parameters */
	val = al_reg_read32(&regs->device.devid1);
	info->counter_width = AL_REG_FIELD_GET(val,
			CXELA500_DEVICE_DEVID1_COUNTWIDTH_MASK,
			CXELA500_DEVICE_DEVID1_COUNTWIDTH_SHIFT);
	info->num_trig_states = AL_REG_FIELD_GET(val,
			CXELA500_DEVICE_DEVID1_NUMTRIGSTATES_MASK,
			CXELA500_DEVICE_DEVID1_NUMTRIGSTATES_SHIFT);
	info->sig_grp_width = 8 * (1 + AL_REG_FIELD_GET(val,
			CXELA500_DEVICE_DEVID1_SIGGRPWIDTH_MASK,
			CXELA500_DEVICE_DEVID1_SIGGRPWIDTH_SHIFT));
	info->sig_grps_num = AL_REG_FIELD_GET(val,
			CXELA500_DEVICE_DEVID1_NUMSIGGRPS_MASK,
			CXELA500_DEVICE_DEVID1_NUMSIGGRPS_SHIFT);

	val = al_reg_read32(&regs->device.devid2);
	info->st4_trace_en = (AL_REG_FIELD_GET(val,
			CXELA500_DEVICE_DEVID2_ALTTS_MASK,
			CXELA500_DEVICE_DEVID2_ALTTS_SHIFT) & AL_ELA_TRIG_STATE_4) ?
			AL_TRUE : AL_FALSE;

	val = al_reg_read32(&regs->device.devid);
	info->cond_trig_en = AL_REG_FIELD_GET(val,
			CXELA500_DEVICE_DEVID_COND_TRIG_MASK,
			CXELA500_DEVICE_DEVID_COND_TRIG_SHIFT) & 0x1;
	info->id_capt_size = AL_REG_FIELD_GET(val,
			CXELA500_DEVICE_DEVID_ID_CAPTURE_SIZE_MASK,
			CXELA500_DEVICE_DEVID_ID_CAPTURE_SIZE_SHIFT);
	info->ram_addr_size = AL_REG_FIELD_GET(val,
			CXELA500_DEVICE_DEVID_SRAM_ADDR_SIZE_MASK,
			CXELA500_DEVICE_DEVID_SRAM_ADDR_SIZE_SHIFT);
}

/** @} end of PMDT ELA group */
