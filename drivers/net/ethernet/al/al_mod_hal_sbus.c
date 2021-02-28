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
 *  @{
 * @file   al_mod_hal_sbus.c
 *
 * @brief  includes SBUS Master HAL implementation
 *
 */

#include "al_mod_hal_sbus.h"
#include "al_mod_hal_sbm_regs.h"
#include "al_mod_hal_reg_utils.h"
#include "al_mod_hal_common.h"

#ifndef AL_SBUS_MASTER_EXTERNAL_FW
#define AL_SBUS_MASTER_EXTERNAL_FW	0
#endif

#if (AL_SBUS_MASTER_EXTERNAL_FW == 0)
#include "al_mod_hal_sbus_master_fw.h"
#endif

#if !defined(AL_SBUS_USE_CORE)
#define AL_SBUS_USE_CORE		1
#endif

/* SBUS command types */
#if AL_SBUS_USE_CORE
#define AL_SBUS_CMD_WRITE			0x21
#define AL_SBUS_CMD_READ			0x22
#else
#define AL_SBUS_CMD_WRITE			0x61
#define AL_SBUS_CMD_READ			0x62
#endif

#define AL_SBUS_CMD_FLAG_CONTROLLER		AL_BIT(7)

#define CORE_SBUS_WRITE_RESULT_CODE_EXPECTED	1
#define CORE_SBUS_WRITE_RESULT_CODE_FAILED	3
#define CORE_SBUS_READ_RESULT_CODE_EXPECTED	4
#define CORE_SBUS_READ_RESULT_CODE_FAILED	6

#define SBUS_CTRLR_REG_CLK_DIV_SETTING_REG_NUM	0xa

#define NUM_MUTEX_USERS				64
#define MUTEX_LOCK_TIMEOUT_USEC			5000

#define ROM_DL_START_TO_USEC			1000
#define ROM_DL_DONE_TO_USEC			10000

#define SBUS_MASTER_INT_REV_GET_REG_NUM		0

#define SBUS_MASTER_INT_BLD_GET_REG_NUM		1

#define SBUS_MASTER_INT_CRC_CHECK_REG_NUM	2
#define SBUS_MASTER_INT_CRC_CHECK_VAL_NA	0x0000
#define SBUS_MASTER_INT_CRC_CHECK_VAL_VALID	0x0001
#define SBUS_MASTER_INT_CRC_CHECK_VAL_FAIL	0xffff

static unsigned int al_mod_sbus_slave_addr_to_mutex_user_idx(
	unsigned int slave_addr)
{
	if (slave_addr >= (NUM_MUTEX_USERS - 2))
		return NUM_MUTEX_USERS - 2;

	return slave_addr;
}

static void al_mod_sbus_mutex_lock(
	const struct al_mod_sbus_obj	*obj,
	unsigned int			mutex_user_idx)
{
	unsigned int timeout;
	uint32_t *mutex_reg = &obj->regs->sbm_mutex[mutex_user_idx].mutex_lane;

	al_mod_reg_write32(mutex_reg, SBM_SBM_MUTEX_MUTEX_LANE_REQUEST);
	for (timeout = MUTEX_LOCK_TIMEOUT_USEC; timeout > 0; timeout--) {
		if (al_mod_reg_read32(mutex_reg) & SBM_SBM_MUTEX_MUTEX_LANE_GRANT)
			break;
		al_mod_udelay(1);
	}

	if (!timeout)
		al_mod_err("%s: failed for mutex %u\n", __func__, mutex_user_idx);
}

static void al_mod_sbus_mutex_unlock(
	const struct al_mod_sbus_obj	*obj,
	unsigned int			mutex_user_idx)
{
	uint32_t *mutex_reg = &obj->regs->sbm_mutex[mutex_user_idx].mutex_lane;

	al_mod_reg_write32(mutex_reg, SBM_SBM_MUTEX_MUTEX_LANE_DONE);
}

#if !defined(AL_SBUS_WAIT_FOR_VALID_TOGGLE)
#define AL_SBUS_WAIT_FOR_VALID_TOGGLE	1
#endif

#if (!defined(AL_SBUS_TIMEOUT_SUPPORT)) || (AL_SBUS_TIMEOUT_SUPPORT == 1)
#define WAIT_FOR_VALID_TIMEOUT		200	/* [uSec] */
#define WAIT_FOR_CORE_DONE_TIMEOUT	1000	/* [uSec] */
#else
#define WAIT_FOR_VALID_TIMEOUT		0
#define WAIT_FOR_CORE_DONE_TIMEOUT	0
#endif
#define WAIT_FOR_VALID_NUM_RETRIES	10
#define WAIT_FOR_VALID_RETRY_DELAY	1	/* [uSec] */

#if AL_SBUS_USE_CORE
static int al_mod_sbus_cmd_issue(
	const struct al_mod_sbus_obj	*obj,
	unsigned int			cmd,
	unsigned int			receiver_addr,
	unsigned int			data_addr,
	unsigned int			data,
	al_mod_bool				wait_vaild)
{
	unsigned int i;
	unsigned int time_remaining;
	unsigned int retries = 0;
	unsigned int reg_val;
#if AL_SBUS_WAIT_FOR_VALID_TOGGLE
	al_mod_bool	core_sbus_rcv_data_valid_prev;

	/* Switch to rcv_data_valid edge detection */
	reg_val = al_mod_reg_read32(&obj->regs->sbus.cfg);
	if (!(reg_val & SBM_SBUS_CFG_CORE_RCV_DATA_VALID_SEL)) {
		reg_val |= SBM_SBUS_CFG_CORE_RCV_DATA_VALID_SEL;
		al_mod_reg_write32(&obj->regs->sbus.cfg, reg_val);
		al_mod_reg_read32(&obj->regs->sbus.cfg);
		al_mod_data_memory_barrier();
	}
#endif

retry:
	/* Setting the data address within the Sbus receiver */
	al_mod_reg_write32(&obj->regs->sbus.core_add, data_addr);
	/*
	* 1. Setting the Sbus receiver address
	* 2. Setting up the sbus command for request
	*/
	al_mod_reg_write32(&obj->regs->sbus.core_cmd,
		(receiver_addr << SBM_SBUS_CORE_CMD_RCV_ADDR_SHIFT) |
		(cmd << SBM_SBUS_CORE_CMD_COMMAND_SHIFT));
#if AL_SBUS_WAIT_FOR_VALID_TOGGLE
	core_sbus_rcv_data_valid_prev =
		!!(al_mod_reg_read32(&obj->regs->sbus.core_rcv) & SBM_SBUS_CORE_RCV_VALID);
	al_mod_data_memory_barrier();
#endif
	/* Setting the data for request- this is what will transition the core_sbus_execute */
	al_mod_reg_write32(&obj->regs->sbus.core_data, data);

	/* Wait for command to be received and command processing to start */
#if WAIT_FOR_CORE_DONE_TIMEOUT
	for (i = 1; i <= (WAIT_FOR_CORE_DONE_TIMEOUT * 100); i++) {
#else
	while (1)
#endif
		if (al_mod_reg_read32(&obj->regs->sbus.sticky) & SBM_SBUS_STICKY_CORE_DONE)
			break;

#if WAIT_FOR_CORE_DONE_TIMEOUT
		if ((i % 100) == 0)
			al_mod_udelay(1);
	}

	if (i > (WAIT_FOR_CORE_DONE_TIMEOUT * 100)) {
		al_mod_err("%s: Timed out waiting for core sbus done to be asserted!\n", __func__);
		return -ETIME;
	}
#endif
	al_mod_dbg("core sbus done is asserted\n");

	/* Wait for command processing to finish */
#if WAIT_FOR_CORE_DONE_TIMEOUT
	for (i = 1; i <= (WAIT_FOR_CORE_DONE_TIMEOUT * 100); i++) {
#else
	while (1)
#endif
		if (!(al_mod_reg_read32(&obj->regs->sbus.sticky) & SBM_SBUS_STICKY_CORE_DONE))
			break;

#if WAIT_FOR_CORE_DONE_TIMEOUT
		if ((i % 100) == 0)
			al_mod_udelay(1);
	}

	if (i > (WAIT_FOR_CORE_DONE_TIMEOUT * 100)) {
		al_mod_err("%s: Timed out waiting for core sbus done to be de-asserted!\n", __func__);
		return -ETIME;
	}
#endif
	al_mod_dbg("core sbus done is de-asserted\n");

	/* if it is not an Sbus controller, but an sbus receiver command */
	if (wait_vaild && (!(cmd & AL_SBUS_CMD_FLAG_CONTROLLER))) {
		al_mod_bool	core_sbus_rcv_data_valid;
		unsigned int core_sbus_result_code;

		unsigned int expected_result_code =
			(cmd == AL_SBUS_CMD_WRITE) ?
			CORE_SBUS_WRITE_RESULT_CODE_EXPECTED :
			CORE_SBUS_READ_RESULT_CODE_EXPECTED;
		unsigned int failed_result_code =
			(cmd == AL_SBUS_CMD_WRITE) ?
			CORE_SBUS_WRITE_RESULT_CODE_FAILED :
			CORE_SBUS_READ_RESULT_CODE_FAILED;

		al_mod_dbg("expected result code is %x\n", expected_result_code);
		al_mod_dbg("Sbus_command is %x\n", cmd);

#if WAIT_FOR_VALID_TIMEOUT
		for (time_remaining = WAIT_FOR_VALID_TIMEOUT; time_remaining; time_remaining--)
#else
		while (1)
#endif
		{
			reg_val = al_mod_reg_read32(&obj->regs->sbus.core_rcv);

			core_sbus_rcv_data_valid =
				!!(reg_val & SBM_SBUS_CORE_RCV_VALID);
#if AL_SBUS_WAIT_FOR_VALID_TOGGLE
			core_sbus_rcv_data_valid = core_sbus_rcv_data_valid !=
				core_sbus_rcv_data_valid_prev;
#endif

			core_sbus_result_code =
				(reg_val & SBM_SBUS_CORE_RCV_RES_CODE_MASK) >>
				SBM_SBUS_CORE_RCV_RES_CODE_SHIFT;
			al_mod_dbg("core_sbus_result_code is %x\n", core_sbus_result_code);
			if (core_sbus_rcv_data_valid &&
				((core_sbus_result_code == expected_result_code) ||
				(core_sbus_result_code == failed_result_code)))
				break;

#if WAIT_FOR_VALID_TIMEOUT
			al_mod_udelay(1);
#endif
		}
#if WAIT_FOR_VALID_TIMEOUT
		if (!time_remaining) {
			if (retries < WAIT_FOR_VALID_NUM_RETRIES) {
				retries++;
				al_mod_dbg("%s: Timed out waiting for valid, retrying %u/%u...\n",
					__func__, retries, WAIT_FOR_VALID_NUM_RETRIES);
				al_mod_udelay(WAIT_FOR_VALID_RETRY_DELAY);
				goto retry;
			}

			al_mod_err("%s: Timed out waiting for valid!\n", __func__);

			return -ETIME;
		}
#endif

		if (core_sbus_result_code == expected_result_code) {
			al_mod_dbg("%s: result valid\n", __func__);
		} else if (core_sbus_result_code == failed_result_code) {
			al_mod_err("%s: operation failed!\n", __func__);
			return -EIO;
		} else {
			al_mod_err("%s: result invalid (%08x)\n", __func__, core_sbus_result_code);
			return -EIO;
		}
	}

	return 0;
}
#else
static int al_mod_sbus_cmd_issue(
	const struct al_mod_sbus_obj	*obj,
	unsigned int			cmd,
	unsigned int			receiver_addr,
	unsigned int			data_addr,
	unsigned int			data,
	al_mod_bool				wait_vaild)
{
	unsigned int time_remaining;
	unsigned int retries = 0;
	unsigned int reg_val;
#if AL_SBUS_WAIT_FOR_VALID_TOGGLE
	al_mod_bool	spare_sbus_rcv_data_valid_prev;

	/* Switch to rcv_data_valid edge detection */
	reg_val = al_mod_reg_read32(&obj->regs->sbus.cfg);
	if (!(reg_val & SBM_SBUS_CFG_SPARE_RCV_DATA_VALID_SEL)) {
		reg_val |= SBM_SBUS_CFG_SPARE_ENABLE;
		reg_val |= SBM_SBUS_CFG_SPARE_RCV_DATA_VALID_SEL;
		al_mod_reg_write32(&obj->regs->sbus.cfg, reg_val);
		al_mod_reg_read32(&obj->regs->sbus.cfg);
		al_mod_data_memory_barrier();
	}
#else
	al_mod_reg_write32(&obj->regs->sbus.cfg, SBM_SBUS_CFG_SPARE_ENABLE);
#endif

retry:
	/* Setting the data address within the Sbus receiver */
	al_mod_reg_write32(&obj->regs->sbus.spare_add, data_addr);
	/*
	* 1. Setting the Sbus receiver address
	* 2. Setting up the sbus command for request
	*/
	al_mod_reg_write32(&obj->regs->sbus.spare_int_cmd,
		(receiver_addr << SBM_SBUS_SPARE_INT_CMD_RCV_ADDR_SHIFT) |
		(cmd << SBM_SBUS_SPARE_INT_CMD_COMMAND_SHIFT));
#if AL_SBUS_WAIT_FOR_VALID_TOGGLE
	spare_sbus_rcv_data_valid_prev =
		!!(al_mod_reg_read32(&obj->regs->sbus.spare_rcv_res) & SBM_SBUS_SPARE_RCV_RES_VALID);
	al_mod_data_memory_barrier();
#endif
	/* Setting the data for request */
	al_mod_reg_write32(&obj->regs->sbus.spare_data, data);
	/* Assert execute */
	al_mod_reg_write32(&obj->regs->sbus.spare_int_cmd,
		(receiver_addr << SBM_SBUS_SPARE_INT_CMD_RCV_ADDR_SHIFT) |
		(cmd << SBM_SBUS_SPARE_INT_CMD_COMMAND_SHIFT) |
		SBM_SBUS_SPARE_INT_CMD_EXECUTE);
	/* Wait for command to be received and command processing to start */
	while (!(al_mod_reg_read32(&obj->regs->sbus.sticky) & SBM_SBUS_STICKY_SPARE_DONE))
		;
	/* Deassert execute */
	al_mod_reg_write32(&obj->regs->sbus.spare_int_cmd,
		(receiver_addr << SBM_SBUS_SPARE_INT_CMD_RCV_ADDR_SHIFT) |
		(cmd << SBM_SBUS_SPARE_INT_CMD_COMMAND_SHIFT));
	/* Wait for command processing to finish */
	while (al_mod_reg_read32(&obj->regs->sbus.sticky) & SBM_SBUS_STICKY_SPARE_DONE)
		;

	/* if it is not an Sbus controller, but an sbus receiver command */
	if (wait_vaild && (!(cmd & AL_SBUS_CMD_FLAG_CONTROLLER))) {
		al_mod_bool	spare_sbus_rcv_data_valid;
		unsigned int spare_sbus_result_code;

		unsigned int expected_result_code =
			(cmd == AL_SBUS_CMD_WRITE) ?
			CORE_SBUS_WRITE_RESULT_CODE_EXPECTED :
			CORE_SBUS_READ_RESULT_CODE_EXPECTED;
		unsigned int failed_result_code =
			(cmd == AL_SBUS_CMD_WRITE) ?
			CORE_SBUS_WRITE_RESULT_CODE_FAILED :
			CORE_SBUS_READ_RESULT_CODE_FAILED;

		al_mod_dbg("expected result code is %x\n", expected_result_code);
		al_mod_dbg("Sbus_command is %x\n", cmd);

#if WAIT_FOR_VALID_TIMEOUT
		for (time_remaining = WAIT_FOR_VALID_TIMEOUT; time_remaining; time_remaining--)
#else
		while (1)
#endif
		{
			reg_val = al_mod_reg_read32(&obj->regs->sbus.spare_rcv_res);

			spare_sbus_rcv_data_valid = !!(reg_val & SBM_SBUS_SPARE_RCV_RES_VALID);
#if AL_SBUS_WAIT_FOR_VALID_TOGGLE
			spare_sbus_rcv_data_valid = spare_sbus_rcv_data_valid !=
				spare_sbus_rcv_data_valid_prev;
#endif

			spare_sbus_result_code =
				(reg_val & SBM_SBUS_SPARE_RCV_RES_CODE_MASK) >>
				SBM_SBUS_SPARE_RCV_RES_CODE_SHIFT;
			al_mod_dbg("spare_sbus_result_code is %x\n", spare_sbus_result_code);
			if (spare_sbus_rcv_data_valid &&
				((spare_sbus_result_code == expected_result_code) ||
				(spare_sbus_result_code == failed_result_code)))
				break;

#if WAIT_FOR_VALID_TIMEOUT
			al_mod_udelay(1);
#endif
		}
#if WAIT_FOR_VALID_TIMEOUT
		if (!time_remaining) {
			if (retries < WAIT_FOR_VALID_NUM_RETRIES) {
				retries++;
				al_mod_dbg("%s: Timed out waiting for valid, retrying %u/%u...\n",
					__func__, retries, WAIT_FOR_VALID_NUM_RETRIES);
				al_mod_udelay(WAIT_FOR_VALID_RETRY_DELAY);
				goto retry;
			}

			al_mod_err("%s: Timed out waiting for valid!\n", __func__);

			return -ETIME;
		}
#endif

		if (spare_sbus_result_code == expected_result_code) {
			al_mod_dbg("%s: result valid\n", __func__);
		} else if (spare_sbus_result_code == failed_result_code) {
			al_mod_err("%s: operation failed!\n", __func__);
			return -EIO;
		} else {
			al_mod_err("%s: result invalid (%08x)\n", __func__, spare_sbus_result_code);
			return -EIO;
		}
	}

	return 0;
}
#endif

static void al_mod_sbus_master_reset_ctl(
	const struct al_mod_sbus_obj	*obj,
	al_mod_bool				assert,
	al_mod_bool				deassert)
{
	unsigned int mutex_user_idx = al_mod_sbus_slave_addr_to_mutex_user_idx(AL_SBUS_ADDR_SBUS_CTRLR);

	al_mod_assert(obj);
	al_mod_assert(obj->regs);

	/* Lock SBUS master */
	al_mod_sbus_mutex_lock(obj, mutex_user_idx);

	if (assert)
		al_mod_reg_write32(&obj->regs->sbus.ctl,
			SBM_SBUS_CTL_SBM_RESET | SBM_SBUS_CTL_SBUS_IN_RESET);
	al_mod_udelay(1);
	if (deassert)
		al_mod_reg_write32(&obj->regs->sbus.ctl, 0);
	/* 10 SBUS clocks */
	al_mod_udelay(1);

	/* Unlock SBUS master */
	al_mod_sbus_mutex_unlock(obj, mutex_user_idx);
}

void al_mod_sbus_handle_init(
	struct al_mod_sbus_obj	*obj,
	void __iomem			*regs)
{
	al_mod_assert(obj);
	al_mod_assert(regs);

	obj->regs = (struct al_mod_sbm_regs __iomem *)regs;
}

void al_mod_sbus_master_reset_assert(
	const struct al_mod_sbus_obj	*obj)
{
	al_mod_sbus_master_reset_ctl(obj, AL_TRUE, AL_FALSE);
}

void al_mod_sbus_master_reset_deassert(
	const struct al_mod_sbus_obj	*obj)
{
	al_mod_sbus_master_reset_ctl(obj, AL_FALSE, AL_TRUE);
}

void al_mod_sbus_master_clk_out_div_set(
	const struct al_mod_sbus_obj	*obj,
	unsigned int			div_log_2)
{
	unsigned int mutex_user_idx = al_mod_sbus_slave_addr_to_mutex_user_idx(AL_SBUS_ADDR_SBUS_CTRLR);

	al_mod_assert(obj);
	al_mod_assert(obj->regs);
	al_mod_assert(div_log_2);

	/* Lock SBUS master */
	al_mod_sbus_mutex_lock(obj, mutex_user_idx);

	al_mod_sbus_cmd_issue(obj,  AL_SBUS_CMD_WRITE,
		AL_SBUS_ADDR_BCAST(AL_SBUS_ADDR_SBUS_CTRLR),
		SBUS_CTRLR_REG_CLK_DIV_SETTING_REG_NUM, div_log_2, AL_TRUE);

	/* Unlock SBUS master */
	al_mod_sbus_mutex_unlock(obj, mutex_user_idx);
}

int al_mod_sbus_write(
	const struct al_mod_sbus_obj	*obj,
	unsigned int			receiver_addr,
	unsigned int			data_addr,
	unsigned int			data,
	al_mod_bool				wait_vaild)
{
	unsigned int mutex_user_idx = al_mod_sbus_slave_addr_to_mutex_user_idx(receiver_addr);
	int err;

	al_mod_assert(obj);
	al_mod_assert(obj->regs);

	/* Lock SBUS master */
	al_mod_sbus_mutex_lock(obj, mutex_user_idx);

	err = al_mod_sbus_cmd_issue(
		obj, AL_SBUS_CMD_WRITE, receiver_addr, data_addr, data, wait_vaild);

	/* Unlock SBUS master */
	al_mod_sbus_mutex_unlock(obj, mutex_user_idx);

	return err;
}

int al_mod_sbus_write_no_lock(
	const struct al_mod_sbus_obj	*obj,
	unsigned int			receiver_addr,
	unsigned int			data_addr,
	unsigned int			data,
	al_mod_bool				wait_vaild)
{
	int err;

	al_mod_assert(obj);
	al_mod_assert(obj->regs);

	err = al_mod_sbus_cmd_issue(
		obj, AL_SBUS_CMD_WRITE, receiver_addr, data_addr, data, wait_vaild);

	return err;
}

int al_mod_sbus_read(
	const struct al_mod_sbus_obj	*obj,
	unsigned int			receiver_addr,
	unsigned int			data_addr,
	unsigned int			*data)
{
	unsigned int mutex_user_idx = al_mod_sbus_slave_addr_to_mutex_user_idx(receiver_addr);
	int err;

	al_mod_assert(obj);
	al_mod_assert(obj->regs);
	al_mod_assert(data);

	/* Lock SBUS master */
	al_mod_sbus_mutex_lock(obj, mutex_user_idx);

	err = al_mod_sbus_cmd_issue(obj, AL_SBUS_CMD_READ, receiver_addr, data_addr, 0, AL_TRUE);

#if AL_SBUS_USE_CORE
	if (!err)
		*data = al_mod_reg_read32(&obj->regs->sbus.rcv);
#else
	if (!err)
		*data = al_mod_reg_read32(&obj->regs->sbus.spare_rcv);
#endif

	/* Unlock SBUS master */
	al_mod_sbus_mutex_unlock(obj, mutex_user_idx);

	return err;
}

int al_mod_sbus_read_no_lock(
	const struct al_mod_sbus_obj	*obj,
	unsigned int			receiver_addr,
	unsigned int			data_addr,
	unsigned int			*data)
{
	int err;

	al_mod_assert(obj);
	al_mod_assert(obj->regs);
	al_mod_assert(data);

	err = al_mod_sbus_cmd_issue(obj, AL_SBUS_CMD_READ, receiver_addr, data_addr, 0, AL_TRUE);
#if AL_SBUS_USE_CORE
	if (!err)
		*data = al_mod_reg_read32(&obj->regs->sbus.rcv);
#else
	if (!err)
		*data = al_mod_reg_read32(&obj->regs->sbus.spare_rcv);
#endif

	return err;
}

int al_mod_sbus_write_masked(
	const struct al_mod_sbus_obj	*obj,
	unsigned int			receiver_addr,
	unsigned int			data_addr,
	unsigned int			data_mask,
	unsigned int			data,
	al_mod_bool				wait_vaild)
{
	unsigned int mutex_user_idx = al_mod_sbus_slave_addr_to_mutex_user_idx(receiver_addr);
	int err;

	al_mod_assert(obj);
	al_mod_assert(obj->regs);
	al_mod_assert((data & data_mask) == data);

	/* Lock SBUS master */
	al_mod_sbus_mutex_lock(obj, mutex_user_idx);

	err = al_mod_sbus_cmd_issue(obj, AL_SBUS_CMD_READ, receiver_addr, data_addr, 0, AL_TRUE);
	if (err)
		return err;

#if AL_SBUS_USE_CORE
	data |= al_mod_reg_read32(&obj->regs->sbus.rcv) & (~data_mask);
#else
	data |= al_mod_reg_read32(&obj->regs->sbus.spare_rcv) & (~data_mask);
#endif
	err = al_mod_sbus_cmd_issue(
		obj, AL_SBUS_CMD_WRITE, receiver_addr, data_addr, data, wait_vaild);

	/* Unlock SBUS master */
	al_mod_sbus_mutex_unlock(obj, mutex_user_idx);

	return err;
}

int al_mod_sbus_rom_download(
	const struct al_mod_sbus_obj	*obj)
{
	unsigned int mutex_user_idx = al_mod_sbus_slave_addr_to_mutex_user_idx(AL_SBUS_ADDR_SBUS_CTRLR);
	unsigned int timeout;
	int err = 0;

	al_mod_assert(obj);
	al_mod_assert(obj->regs);

	/* Lock SBUS master */
	al_mod_sbus_mutex_lock(obj, mutex_user_idx);

	/* Download enable */
	al_mod_reg_write32_masked(&obj->regs->rom.ctrl, SBM_ROM_CTRL_ENABLE, SBM_ROM_CTRL_ENABLE);
	/* Wait for download to start */
	for (timeout = ROM_DL_START_TO_USEC; timeout; timeout--) {
		if (al_mod_reg_read32(&obj->regs->rom.ctrl) & SBM_ROM_CTRL_OUT_EN)
			break;
		al_mod_udelay(1);
	}
	if (!timeout) {
		al_mod_err("%s: timeout waiting for download to begin!\n", __func__);
		err = -ETIME;
		goto done;
	}
	/* Wait for download to complete */
	for (timeout = ROM_DL_DONE_TO_USEC; timeout; timeout--) {
		if (!(al_mod_reg_read32(&obj->regs->rom.ctrl) & SBM_ROM_CTRL_OUT_EN))
			break;
		al_mod_udelay(1);
	}
	if (!timeout) {
		al_mod_err("%s: timeout waiting for download to complete!\n", __func__);
		err = -ETIME;
		goto done;
	}

done:
	/* Unlock SBUS master */
	al_mod_sbus_mutex_unlock(obj, mutex_user_idx);

	return err;
}

void al_mod_sbus_lock(
	const struct al_mod_sbus_obj	*obj,
	unsigned int			receiver_addr)
{
	unsigned int mutex_user_idx = al_mod_sbus_slave_addr_to_mutex_user_idx(receiver_addr);

	al_mod_assert(obj);
	al_mod_assert(obj->regs);

	/* Lock SBUS master */
	al_mod_sbus_mutex_lock(obj, mutex_user_idx);
}

void al_mod_sbus_unlock(
	const struct al_mod_sbus_obj	*obj,
	unsigned int			receiver_addr)
{
	unsigned int mutex_user_idx = al_mod_sbus_slave_addr_to_mutex_user_idx(receiver_addr);

	al_mod_assert(obj);
	al_mod_assert(obj->regs);

	/* Unlock SBUS master */
	al_mod_sbus_mutex_unlock(obj, mutex_user_idx);
}

int al_mod_sbus_master_fw_init(
	const struct al_mod_sbus_obj	*sbus,
	const uint32_t			*fw,
	unsigned int			fw_size)
{
	unsigned int receiver_addr = 0xfd;
	unsigned int i;

	al_mod_assert(sbus);

	if (!fw) {
#if (AL_SBUS_MASTER_EXTERNAL_FW == 0)
		al_mod_assert(!fw_size);
		fw = al_mod_serdes_avg_fw_sbus_master;
		fw_size = AL_ARR_SIZE(al_mod_serdes_avg_fw_sbus_master);
#else
		al_mod_err("%s: FW not provided!\n", __func__);
		return -EINVAL;
#endif
	} else {
		al_mod_assert(fw_size);
	}

	/* Assert Spico reset */
	al_mod_sbus_write(sbus, receiver_addr, 0x01, AL_BIT(6) | AL_BIT(7), 1);
	/* Deassert Spico reset, but keep it disabled */
	al_mod_sbus_write(sbus, receiver_addr, 0x01, AL_BIT(6) | 0, 1);
	/* Assert IMEM override */
	al_mod_sbus_write(sbus, receiver_addr, 0x01, AL_BIT(6) | AL_BIT(9), 1);

	al_mod_sbus_lock(sbus, receiver_addr);

	/*  Note: A delayed version of the write-enable signal is used to write data */
	/*	into rom after address and data are set up.  This allows loading one */
	/*	new word for each sbus write. */
	for (i = 0; i < fw_size; i++) {
		unsigned int k = (i % fw_size);
		unsigned int sbus_data;

		if (!(i % (1024 / sizeof(uint32_t))))
			al_mod_info("FW DL %u/%u\n", (unsigned int)(i * sizeof(uint32_t)),
				(unsigned int)(sizeof(uint32_t) * fw_size));
		sbus_data = (3 << 30) | swap32_from_le(fw[k]);

		al_mod_dbg("Sbus burst data is %x\n", sbus_data);
		al_mod_sbus_write_no_lock(sbus, receiver_addr, 0x14, sbus_data, 1);
	}

	al_mod_sbus_unlock(sbus, receiver_addr);

	/*
	 * Pad with 0's - required for Verilog simulations, but may be bypassed when loading
	 * firmware onto chip.
	 */
	al_mod_sbus_write(sbus, receiver_addr, 0x14, 0xc000, 1);
	al_mod_sbus_write(sbus, receiver_addr, 0x14, 0xc000, 1);
	al_mod_sbus_write(sbus, receiver_addr, 0x14, 0xc000, 1);

	/* IMEM override off */
	al_mod_sbus_write(sbus, receiver_addr, 0x01, AL_BIT(6) | 0, 1);
	/* Turn ECC on */
	al_mod_sbus_write(sbus, receiver_addr, 0x16, 0x000c0000, 1);
	/* Turn SPICO Enable on */
	al_mod_sbus_write(sbus, receiver_addr, 0x01, AL_BIT(6) | AL_BIT(8), 1);

	return 0;
}

void al_mod_sbus_master_fw_revision_print(
	const struct al_mod_sbus_obj	*sbus)
{
	unsigned int status;
	unsigned int rev;
	unsigned int bld;
	unsigned int crc;
	int err;

	err = al_mod_sbus_master_interrupt(sbus, SBUS_MASTER_INT_REV_GET_REG_NUM, 0, &status, &rev);
	if (err || (status != 1))
		rev = 0;
	err = al_mod_sbus_master_interrupt(sbus, SBUS_MASTER_INT_BLD_GET_REG_NUM, 0, &status, &bld);
	if (err || (status != 1))
		bld = 0;
	err = al_mod_sbus_master_interrupt(sbus, SBUS_MASTER_INT_CRC_CHECK_REG_NUM, 0, &status, &crc);
	if (err || (status != 1))
		crc = SBUS_MASTER_INT_CRC_CHECK_VAL_NA;

	al_mod_print("SBUS master FW rev %02x bld %02x crc %s\n", rev, bld,
		crc == SBUS_MASTER_INT_CRC_CHECK_VAL_VALID ? "Ok" :
		(crc == SBUS_MASTER_INT_CRC_CHECK_VAL_FAIL ? "Failed" : "N/A"));
}

int al_mod_sbus_master_interrupt(
	const struct al_mod_sbus_obj	*sbus,
	unsigned int			code,
	unsigned int			data,
	unsigned int			*res_status,
	unsigned int			*res_data)
{
	unsigned int receiver_addr = 0xfd;
	unsigned int reg_val;
	int err;

	al_mod_assert(sbus);
	al_mod_assert(res_status);
	al_mod_assert(res_data);

	al_mod_sbus_write(sbus, receiver_addr, 0x02, (data << 16) | code, 1);
	err = al_mod_sbus_read(sbus, receiver_addr, 0x07, &reg_val);
	if (err)
		return err;
	al_mod_sbus_write(sbus, receiver_addr, 0x07, reg_val | 1, 1);
	al_mod_sbus_write(sbus, receiver_addr, 0x07, reg_val, 1);

	do {
		err = al_mod_sbus_read(sbus, receiver_addr, 0x08, &reg_val);
		if (err)
			return err;
	} while ((reg_val & AL_BIT(15)) && (!(reg_val & 0x7fff)));

	*res_status = reg_val & 0x7fff;
	*res_data = reg_val >> 16;

	return 0;
}

/** @} */

