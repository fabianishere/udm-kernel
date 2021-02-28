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
 * @defgroup group_i2c I2C
 *
 *  @{
 * @file   al_mod_hal_i2c.c
 *
 * @brief  I2C HAL driver
 *
 */

#include "al_mod_hal_i2c.h"
#include "al_mod_hal_i2c_regs.h"

#define AL_I2C_UDELAY(usec) (i2c_if->udelay_cb(i2c_if->udelay_cb_context, (usec)))

/* --->>> static functions <<<--- */
static void al_mod_i2c_udelay(
		void *context  __attribute__((unused)),
		unsigned int delay)
{
	al_mod_udelay(delay);
}

static int al_mod_i2c_perform_write(
		struct al_mod_i2c_interface *i2c_if,
		unsigned int bytelen,
		uint8_t *buffer,
		int is_init)
{
	struct al_mod_i2c_regs *regs = (struct al_mod_i2c_regs *)i2c_if->regs;
	unsigned long timer = 0;
	uint16_t write_data;

	while (bytelen) {
		/* Make sure there's room in the tx FIFO prior to writing */
		if (al_mod_reg_read32(&regs->status) &
				AL_I2C_STATUS_TFNF_NOT_FULL) {
			if ((is_init == 1) || bytelen > 1)
				write_data = (uint16_t)*buffer;
			else /* On last DATA write (not init), raise the
				stop interrupt */
				write_data = (uint16_t)(*buffer |
						AL_I2C_DATA_CMD_STOP);
			al_mod_reg_write16(&regs->data_cmd, write_data);
			++buffer;
			--bytelen;
			timer = 0;
		} else if (timer > i2c_if->timeout) {
			al_mod_err("%s: Timed out. i2c write failed\n",
					__func__);
			return -ETIME;
		} else {
			AL_I2C_UDELAY(1); /* wait for 1us */
			++timer;
		}
	}

	return 0;
}

static inline void al_mod_i2c_enable_set(struct al_mod_i2c_interface *i2c_if, int en)
{
	struct al_mod_i2c_regs *regs = (struct al_mod_i2c_regs *)i2c_if->regs;
	uint16_t i2c_enable_val;

	i2c_enable_val = al_mod_reg_read16(&regs->enable);
	AL_REG_BIT_VAL_SET(i2c_enable_val, AL_I2C_ENABLE_SHIFT, en);
	al_mod_reg_write16(&regs->enable, i2c_enable_val);
}


static inline void al_mod_i2c_enable(struct al_mod_i2c_interface *i2c_if)
{
	al_mod_i2c_enable_set(i2c_if, 0x1);
}

static inline void al_mod_i2c_disable(struct al_mod_i2c_interface *i2c_if)
{
	al_mod_i2c_enable_set(i2c_if, 0x0);
}

static inline al_mod_bool al_mod_i2c_enable_get(struct al_mod_i2c_interface *i2c_if)
{
	struct al_mod_i2c_regs *regs = (struct al_mod_i2c_regs *)i2c_if->regs;
	uint16_t i2c_enable_val;

	i2c_enable_val = al_mod_reg_read16(&regs->enable);
	if (((i2c_enable_val & AL_I2C_ENABLE_MASK) >> AL_I2C_ENABLE_SHIFT) == 0x1)
		return AL_TRUE;

	return AL_FALSE;
}

/**
 * Wait until the activity interrupt bit is clear
 */
static int al_mod_i2c_wait_for_idle(struct al_mod_i2c_interface *i2c_if)
{
	struct al_mod_i2c_regs *regs = (struct al_mod_i2c_regs *)i2c_if->regs;
	uint32_t status;
	unsigned long timer = 0;

	status = al_mod_reg_read32(&regs->status);
	while (status & AL_I2C_STATUS_ACTIVITY_ACTIVE) {

		/* Evaluate timeout */
		if (timer > i2c_if->timeout)
			return -ETIME;
		AL_I2C_UDELAY(1); /* wait for 1us */
		++timer;
		status = al_mod_reg_read32(&regs->status);
	}

	return 0;
}

/**
 * Initiate transfer
 *
 * Both write and read transfers begin with programing the Target device
 * address (target), then programing the internal operation address (address)
 */
static int al_mod_i2c_xfer_init(
		struct al_mod_i2c_interface *i2c_if,
		uint16_t target,
		uint8_t *address,
		uint8_t address_len)
{
	int ret = 0;
	struct al_mod_i2c_regs *regs = (struct al_mod_i2c_regs *)i2c_if->regs;
	uint16_t target_val;

	target_val = al_mod_reg_read16(&regs->tar);
	target_val = ((target_val & ~AL_I2C_TAR_TARGET_MASK) | target);
	al_mod_reg_write16(&regs->tar, target_val);

	al_mod_i2c_enable(i2c_if);

	if (address_len > 0) /* write address offset */
		ret = al_mod_i2c_perform_write(i2c_if, address_len, address, 1);

	return ret;
}


static int al_mod_i2c_xfer_finish(struct al_mod_i2c_interface *i2c_if)
{
	struct al_mod_i2c_regs *regs = (struct al_mod_i2c_regs *)i2c_if->regs;
	unsigned long timer = 0;
	int err = 0;

	/* wait until a stop tx interrupt is asserted */
	while (1) {
		if ((al_mod_reg_read16(&regs->raw_intr_stat) &
					AL_I2C_INTR_MASK_STOP_DET_MASK)) {
			al_mod_reg_read16(&regs->clr_stop_det);
			break;
		} else if (timer > i2c_if->timeout) {
			al_mod_err("%s: Timed out waiting for stop detection\n",
					__func__);
			err = -ETIME;
			break;
		} else {
			AL_I2C_UDELAY(1);
			++timer;
		}
	}

	/* wait until device is in IDLE state */
	if (al_mod_i2c_wait_for_idle(i2c_if)) {
		al_mod_err("%s: Timed out waiting for IDLE state\n", __func__);
		err = -ETIME;
	}

	al_mod_i2c_disable(i2c_if);

	return err;
}

/* --->>> API functions <<<--- */
int al_mod_i2c_init(
		struct al_mod_i2c_interface *i2c_if,
		const struct al_mod_i2c_configuration *config,
		void __iomem *i2c_regs_base)
{
	int ret = 0;
	struct al_mod_i2c_regs *regs;
	/* default threshold values */
	uint16_t rx_tl_val = 0, tx_tl_val = 0;
	uint16_t con_val = 0;

	al_mod_assert(i2c_if);
	al_mod_assert(config);
	al_mod_assert(i2c_regs_base);

	/* Initialize the register address field */
	i2c_if->regs = i2c_regs_base;
	regs = (struct al_mod_i2c_regs *)i2c_if->regs;

	al_mod_i2c_disable(i2c_if);

	/* Perform Master Mode configurations */
	if (config->mode == AL_I2C_MODE_MASTER) {
		/*
		 * The RESTART condition must be enabled in order to support
		 * High Speed and 10bit Addressing modes
		 */
		con_val = AL_I2C_CON_MASTER_MODE_MASTER |
			AL_I2C_CON_SLAVE_DISABLE_DISABLE |
			AL_I2C_CON_RESTART_EN_MASK |
			config->speed;
		/* Configure 10bit address mode */
		if (config->addr_size == AL_I2C_ADDR_SIZE_10BIT) {
			con_val |= AL_I2C_CON_10BITADDR_MASTER_ENABLE;
			al_mod_reg_write16(&regs->tar, AL_I2C_TAR_10BIT_ADDR_ENABLE);
		} else {
			al_mod_reg_write16(&regs->tar, 0);
		}
	} else {
		/* Configure the slave address register */
		al_mod_reg_write16(&regs->sar, (uint16_t) config->local_slv_addr);
		if (config->addr_size == AL_I2C_ADDR_SIZE_10BIT)
			con_val |= AL_I2C_CON_10BITADDR_SLAVE_MASK;
	}

	al_mod_reg_write16(&regs->con, con_val);

	/* Configure thresholds */
	if (config->configure_thresholds) {
		rx_tl_val = (uint16_t)config->rx_threshold;
		tx_tl_val = (uint16_t)config->tx_threshold;
	}
	al_mod_reg_write16(&regs->rx_tl, rx_tl_val);
	al_mod_reg_write16(&regs->tx_tl, tx_tl_val);

	i2c_if->timeout = config->timeout;
	i2c_if->udelay_cb = al_mod_i2c_udelay;
	i2c_if->udelay_cb_context = NULL;

	return ret;
}

void al_mod_i2c_hold_time_set(
		struct al_mod_i2c_interface *i2c_if,
		unsigned int ref_clk_freq_mhz,
		unsigned int hold_time_ns)
{
	struct al_mod_i2c_regs *regs;
	uint32_t hold_time_val = (hold_time_ns * ref_clk_freq_mhz) / 1000;

	al_mod_assert(i2c_if);
	al_mod_assert(al_mod_i2c_enable_get(i2c_if) == AL_FALSE);

	regs = (struct al_mod_i2c_regs *)i2c_if->regs;

	al_mod_reg_write32(&regs->sda_hold, hold_time_val);
}

void al_mod_i2c_udelay_cb_set(
		struct al_mod_i2c_interface *i2c_if,
		void (*udelay_cb)(void *context, unsigned int delay),
		void *udelay_cb_context)
{
	i2c_if->udelay_cb = udelay_cb;
	i2c_if->udelay_cb_context = udelay_cb_context;
}

int al_mod_i2c_read(
		struct al_mod_i2c_interface *i2c_if,
		unsigned int target_i2c_addr,
		uint8_t address_len,
		uint8_t *address,
		unsigned int bytelen,
		uint8_t *buffer)
{
	int ret = 0;
	struct al_mod_i2c_regs *regs;
	int nbyte = bytelen;
	unsigned long timer = 0;
	uint16_t data_cmd_val = 0;
	unsigned int tx_bytes_cnt = bytelen;

	al_mod_assert(i2c_if);
	regs = (struct al_mod_i2c_regs *)i2c_if->regs;

	/*
	 * Initialize the transfer - write the target address,
	 * then write the internal address to read from
	 * in case address_len > 0
	 */
	ret = al_mod_i2c_xfer_init(
		i2c_if, (uint16_t)target_i2c_addr, address, address_len);
	if (ret)
		return ret;

	while (bytelen) {
		uint32_t status = al_mod_reg_read32(&regs->status);
		/* For new read xfer: issue read command */
		if ((tx_bytes_cnt != 0) &&
			((status & AL_I2C_STATUS_TFNF_MASK) ==
					AL_I2C_STATUS_TFNF_NOT_FULL) &&
			((status & AL_I2C_STATUS_RFF_MASK) ==
					AL_I2C_STATUS_RFF_NOT_FULL)) {
			data_cmd_val = AL_I2C_DATA_CMD_READ;
			/* On last byte, issue a STOP intr as well */
			if (tx_bytes_cnt-- == 1)
				data_cmd_val |= AL_I2C_DATA_CMD_STOP;
			/* Config the data cmd register for read xfer */
			al_mod_reg_write16(&regs->data_cmd, data_cmd_val);
		}

		/* Make sure the rx FIFO isn't empty prior to reading */
		if (status & AL_I2C_STATUS_RFNE_NOT_EMPTY) {
			*buffer = (uint8_t)al_mod_reg_read16(
					&regs->data_cmd);
			++buffer;
			--bytelen;
			timer = 0;
		} else if (timer > (nbyte * i2c_if->timeout)) {
			al_mod_i2c_xfer_finish(i2c_if);
			return -ETIME;
		} else {
			AL_I2C_UDELAY(1); /* wait for 1us */
			++timer;
		}
	}

	ret = al_mod_i2c_xfer_finish(i2c_if);

	return ret;
}


int al_mod_i2c_write(
		struct al_mod_i2c_interface *i2c_if,
		unsigned int target_i2c_addr,
		uint8_t address_len,
		uint8_t *address,
		unsigned int bytelen,
		uint8_t *buffer)
{
	int ret = 0;

	al_mod_assert(i2c_if);

	/*
	 * Initialize the transfer - write the target address,
	 * then write the internal address to write to
	 */
	ret = al_mod_i2c_xfer_init(
		i2c_if, (uint16_t)target_i2c_addr, address, address_len);
	if (ret)
		return ret;

	ret = al_mod_i2c_perform_write(i2c_if, bytelen, buffer, 0);
	if (ret) {
		al_mod_i2c_xfer_finish(i2c_if);
		return ret;
	}

	ret = al_mod_i2c_xfer_finish(i2c_if);

	return ret;
}


unsigned int al_mod_i2c_raw_intr_get(struct al_mod_i2c_interface *i2c_if)
{
	struct al_mod_i2c_regs *regs;

	al_mod_assert(i2c_if);

	regs = (struct al_mod_i2c_regs *)i2c_if->regs;

	return (al_mod_reg_read16(&regs->raw_intr_stat));
}


unsigned int al_mod_i2c_masked_intr_get(struct al_mod_i2c_interface *i2c_if)
{
	struct al_mod_i2c_regs *regs;

	al_mod_assert(i2c_if);

	regs = (struct al_mod_i2c_regs *)i2c_if->regs;

	return (al_mod_reg_read16(&regs->intr_stat));
}


void al_mod_i2c_intr_enable(
		struct al_mod_i2c_interface *i2c_if,
		unsigned int enable_val)
{
	struct al_mod_i2c_regs *regs;
	uint16_t curr_val;

	al_mod_assert(i2c_if);

	regs = (struct al_mod_i2c_regs *)i2c_if->regs;

	curr_val = al_mod_reg_read16(&regs->intr_mask);

	al_mod_reg_write16(&regs->intr_mask, curr_val | enable_val);
}

void al_mod_i2c_intr_disable(
		struct al_mod_i2c_interface *i2c_if,
		unsigned int disable_val)
{
	struct al_mod_i2c_regs *regs;
	uint16_t curr_val;

	al_mod_assert(i2c_if);

	regs = (struct al_mod_i2c_regs *)i2c_if->regs;

	curr_val = al_mod_reg_read16(&regs->intr_mask);

	al_mod_reg_write16(&regs->intr_mask, curr_val & ~disable_val);
}


void al_mod_i2c_intr_clear(
		struct al_mod_i2c_interface *i2c_if,
		unsigned int clear_val)
{
	struct al_mod_i2c_regs *regs;

	al_mod_assert(i2c_if);
	al_mod_assert(!(clear_val & AL_I2C_INTR_RX_FULL));
	al_mod_assert(!(clear_val & AL_I2C_INTR_TX_EMPTY));

	regs = (struct al_mod_i2c_regs *)i2c_if->regs;

	if(clear_val & AL_I2C_INTR_RX_UNDER)
		al_mod_reg_read16(&regs->clr_rx_under);
	if(clear_val & AL_I2C_INTR_RX_OVER)
		al_mod_reg_read16(&regs->clr_rx_over);
	if(clear_val & AL_I2C_INTR_TX_OVER)
		al_mod_reg_read16(&regs->clr_tx_over);
	if(clear_val & AL_I2C_INTR_RD_REQ)
		al_mod_reg_read16(&regs->clr_rd_req);
	if(clear_val & AL_I2C_INTR_TX_ABRT)
		al_mod_reg_read16(&regs->clr_tx_abrt);
	if(clear_val & AL_I2C_INTR_RX_DONE)
		al_mod_reg_read16(&regs->clr_rx_done);
	if(clear_val & AL_I2C_INTR_ACTIVITY)
		al_mod_reg_read16(&regs->clr_activity);
	if(clear_val & AL_I2C_INTR_STOP_DET)
		al_mod_reg_read16(&regs->clr_stop_det);
	if(clear_val & AL_I2C_INTR_START_DET)
		al_mod_reg_read16(&regs->clr_start_det);
	if(clear_val & AL_I2C_INTR_GEN_CALL)
		al_mod_reg_read16(&regs->clr_gen_call);
}


void al_mod_i2c_intr_clear_all(
		struct al_mod_i2c_interface *i2c_if)
{
	struct al_mod_i2c_regs *regs;

	al_mod_assert(i2c_if);

	regs = (struct al_mod_i2c_regs *)i2c_if->regs;

	al_mod_reg_read16(&regs->clr_intr);
}


unsigned int al_mod_i2c_slave_write(
		struct al_mod_i2c_interface *i2c_slave_if,
		uint8_t *buffer,
		unsigned int buffer_size,
		al_mod_bool *write_done)
{
	unsigned int num_of_write_bytes = 0;
	unsigned int i = 0;
	uint16_t raw_intr_value  = 0;
	struct al_mod_i2c_regs *slave_regs;

	al_mod_assert(i2c_slave_if);
	al_mod_assert(buffer);
	al_mod_assert(buffer_size);

	slave_regs = (struct al_mod_i2c_regs *)i2c_slave_if->regs;

	*write_done = AL_FALSE;

	/* Clear TX_ABRT interrupt */
	al_mod_i2c_intr_clear(i2c_slave_if, AL_I2C_INTR_TX_ABRT);

	/*
	 * Transmit data ,while I2C Master acknowledge a transmitted byte .
	 * On the last byte of transaction,Master does not acknowledge
	 * and RX_DONE interrupt is asserted
	 */
	while(!((raw_intr_value = al_mod_reg_read16(&slave_regs->raw_intr_stat)) &
			AL_I2C_INTR_MASK_RX_DONE_MASK)) {
		/* Make sure the tx FIFO isn't full prior to writting */
		if ((al_mod_reg_read32(&slave_regs->status) & AL_I2C_STATUS_TFNF_NOT_FULL) &&
				(raw_intr_value & AL_I2C_INTR_MASK_RD_REQ_MASK)) {
			al_mod_reg_write16(&slave_regs->data_cmd, buffer[i++]);
			num_of_write_bytes++;

			/* Clear RD_REQ interrupt */
			al_mod_i2c_intr_clear(i2c_slave_if, AL_I2C_INTR_RD_REQ);

			/*
			 * If we reach end of mem_space and write transaction
			 * is continued return with actual bytelen
			 * that was transmited till now
			 */
			if (i == buffer_size)
				return num_of_write_bytes;
		}
	}
	al_mod_i2c_intr_clear(i2c_slave_if,AL_I2C_INTR_RX_DONE);
	*write_done = AL_TRUE;

	return num_of_write_bytes;
}

unsigned int al_mod_i2c_slave_read(
		struct al_mod_i2c_interface *i2c_slave_if,
		uint8_t *buffer,
		unsigned int buffer_size,
		al_mod_bool *read_done)
{
	unsigned int num_of_read_bytes = 0;
	unsigned int i = 0;
	uint16_t raw_intr_value  = 0;
	struct al_mod_i2c_regs *slave_regs;

	al_mod_assert(i2c_slave_if);
	al_mod_assert(buffer);
	al_mod_assert(buffer_size);

	slave_regs = (struct al_mod_i2c_regs *)i2c_slave_if->regs;
	*read_done = AL_FALSE;

	/* Receive data untill I2C Master asserts the STOP bit */
	while(!((raw_intr_value = al_mod_reg_read16(&slave_regs->raw_intr_stat))&
			AL_I2C_INTR_MASK_STOP_DET_MASK)) {
		if(raw_intr_value & AL_I2C_INTR_MASK_RX_FULL_MASK) {
			/*
			 * Read the byte from the rx FIFO and
			 * write it to slave memory buffer
			 */
			buffer[i++] = (uint8_t)al_mod_reg_read16(&slave_regs->data_cmd);
			num_of_read_bytes++;

			/*
			 * If we reach end of mem_space and read transaction
			 * is continued return with actual bytelen that was
			 * received till now
			 */
			if (i == buffer_size)
				return num_of_read_bytes;
		}
	}
	al_mod_i2c_intr_clear(i2c_slave_if, AL_I2C_INTR_STOP_DET);
	*read_done = AL_TRUE;

	return num_of_read_bytes;
}
/** @} end of I2C group */
