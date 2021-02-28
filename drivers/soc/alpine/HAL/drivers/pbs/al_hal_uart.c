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
 * @defgroup group_uart	UART
 *  @{
 *
 * @file   al_hal_uart.c
 * @brief Implementation file for the UART HAL driver
 *
 */
#include "al_hal_uart.h"
#include "al_hal_uart_regs.h"

#define UART_FIFO_DEPTH		64
#define UART_BAUD_RATE_DIVISOR(ref_clock, baud_rate) \
	((ref_clock) / (16 * (baud_rate)))

void al_uart_handle_init(
		struct al_uart_if	*uart_if,
		void __iomem	*uart_regs_base)
{
	al_assert(uart_regs_base);
	/* Initialize the register address field */
	uart_if->regs = (struct al_uart_regs __iomem *)uart_regs_base;
}

void al_uart_init(
		struct al_uart_if	*uart_if,
		struct al_uart_cfg	*config)
{
	struct al_uart_regs *regs;
	uint32_t mcr_value = 0, lcr_value = 0;
	uint16_t divisor;

	al_assert(uart_if);
	al_assert(config);

	regs = (struct al_uart_regs *)uart_if->regs;

	/* HW initialization */
	if (config->mode)
		mcr_value |=  AL_UART_MCR_SIRE_ENABLE;
	if (config->enable_auto_flow_cntr)
		/* Enable Auto Flow Control */
		mcr_value |= (AL_UART_MCR_AUTO_FLOW_CNTRL_ENABLE |
				AL_UART_MCR_REQUEST_TO_SEND_SET);
	al_reg_write32(&regs->mcr, mcr_value);

	/*
	 * Set DLAB bit in order to access DLL and DLH registers
	 * to set up divisor for required baud rate
	 */
	al_reg_write32(&regs->lcr, AL_UART_LCR_DLAB_SET);
	divisor = UART_BAUD_RATE_DIVISOR(config->input_clk_freq,
					 config->baud_rate);
	al_reg_write32(&regs->dlh_ier, (uint32_t)(divisor >> 8));
	al_reg_write32(&regs->rbr_thr_dll, (uint32_t)(divisor & 0xFF));

	/* Clear DLAB bit in order to access other registers.*/
	al_reg_write32(&regs->lcr, AL_UART_LCR_DLAB_CLEAR);

	if (config->serial_char.parity != AL_UART_PARITY_NONE) {
		lcr_value = AL_UART_LCR_PARITY_ENABLE |
			    (config->serial_char.parity <<
					AL_UART_LCR_EVEN_PARITY_SELECT_SHIFT);
	}
	/* Setting transfer data_length and number of stop bits */
	lcr_value |= (config->serial_char.stop_bit_num <<
			AL_UART_LCR_STOP_BITS_NUMBER_SHIFT) |
				(config->serial_char.serial_data_length);

	al_reg_write32(&regs->lcr, lcr_value);

	if (config->enable_fifos) {
		/* Enable FIFO Mode and set rx/tx FIFO triggers*/
		al_reg_write32(&regs->iir_fcr, AL_UART_FCR_FIFO_ENABLE |
				(config->rx_trigger << AL_UART_FCR_RCVR_SHIFT) |
				(config->tx_trigger << AL_UART_FCR_TET_SHIFT));
	}
}

int al_uart_read(
		struct al_uart_if *uart_if,
		unsigned int bytelen,
		uint8_t *buffer,
		int timeout)
{
	int ret = 0;
	int timer = 0;
	unsigned int num_data_avail;
	struct al_uart_regs *regs;
	unsigned int fifo_enable;
	al_bool infinite_loop = (timeout == -1) ? AL_TRUE : AL_FALSE;

	al_assert(uart_if);
	al_assert(buffer);
	al_assert(bytelen);

	regs = (struct al_uart_regs *)uart_if->regs;
	fifo_enable = al_reg_read32(&regs->iir_fcr) & AL_UART_IIR_FIFOSE_MASK;

	while (bytelen) {
		/*
		 * If RX FIFO is enabled,
		 * read number of available data in FIFO
		 */
		if (fifo_enable) {
			num_data_avail = al_reg_read32(&regs->rfl);
		} else {
			if (al_reg_read32(&regs->lsr) &
					AL_UART_LINE_STATUS_DATA_READY_MASK) {
				num_data_avail = 1;
			} else {
				num_data_avail = 0;
			}
		}
		if (num_data_avail) {
			timer = 0;
			while (num_data_avail && bytelen) {
				*buffer = (uint8_t)al_reg_read32(
						&regs->rbr_thr_dll);
				++buffer;
				--bytelen;
				--num_data_avail;
			}
		} else if (!infinite_loop) {
			al_udelay(1); /* wait for 1us */
			++timer;
		}

		if ((!infinite_loop) && (timer > timeout)) {
				al_err("%s: Timed out.\n", __func__);
				return -ETIME;
		}
	}

	return ret;
}

int al_uart_write(
		struct al_uart_if *uart_if,
		unsigned int bytelen,
		uint8_t *buffer,
		int timeout)
{
	int ret = 0;
	int timer = 0;
	unsigned int num_free_entry_avail;
	struct al_uart_regs *regs;
	unsigned int fifo_enable;
	al_bool infinite_loop = (timeout == -1) ? AL_TRUE : AL_FALSE;

	al_assert(uart_if);
	al_assert(buffer);
	al_assert(bytelen);

	regs = (struct al_uart_regs *)uart_if->regs;
	fifo_enable = al_reg_read32(&regs->iir_fcr) & AL_UART_IIR_FIFOSE_MASK;

	while (bytelen) {
		/*
		 * If TX FIFO is enabled, make sure there's room in it
		 * prior to writing
		 */
		if (fifo_enable) {
			num_free_entry_avail = UART_FIFO_DEPTH -
						al_reg_read32(&regs->tfl);
		} else {
			if (al_reg_read32(&regs->lsr) &
					AL_UART_LINE_STATUS_THRE_MASK) {
				num_free_entry_avail = 1;
			} else {
				num_free_entry_avail = 0;
			}
		}
		if (num_free_entry_avail) {
			timer = 0;
			while (num_free_entry_avail && bytelen) {
				al_reg_write32(&regs->rbr_thr_dll,
						(uint32_t)*buffer);
				++buffer;
				--bytelen;
				--num_free_entry_avail;
			}
		} else if (!infinite_loop) {
			al_udelay(1); /* wait for 1us */
			++timer;
		}

		if ((!infinite_loop) && (timer > timeout)) {
			al_err("%s: Timed out.\n", __func__);
			return -ETIME;
		}
	}
	return ret;
}


void al_uart_soft_reset(
		struct al_uart_if *uart_if,
		unsigned int reset_val)
{
	struct al_uart_regs *regs;

	al_assert(uart_if);

	regs = (struct al_uart_regs *)uart_if->regs;

	al_reg_write32(&regs->srr, reset_val);
}

unsigned int al_uart_line_status_get(
		struct al_uart_if *uart_if)
{
	uint32_t line_stat;
	struct al_uart_regs *regs;

	al_assert(uart_if);

	regs = (struct al_uart_regs *)uart_if->regs;

	line_stat = al_reg_read32(&regs->lsr);

	return  line_stat;
}

unsigned int al_uart_modem_status_get(
		struct al_uart_if *uart_if)
{
	uint32_t modem_stat;
	struct al_uart_regs *regs;

	al_assert(uart_if);

	regs = (struct al_uart_regs *)uart_if->regs;

	modem_stat = al_reg_read32(&regs->msr);

	return modem_stat;
}

unsigned int al_uart_intr_status_get(
		struct al_uart_if *uart_if)
{
	uint32_t intr_stat;
	struct al_uart_regs *regs;

	al_assert(uart_if);

	regs = (struct al_uart_regs *)uart_if->regs;

	intr_stat = al_reg_read32(&regs->iir_fcr);

	return intr_stat;
}

enum al_uart_intr_id al_uart_intr_id_get(
		struct al_uart_if *uart_if)
{
	uint32_t intr_id;
	struct al_uart_regs *regs;

	al_assert(uart_if);

	regs = (struct al_uart_regs *)uart_if->regs;

	intr_id = al_reg_read32(&regs->iir_fcr) & AL_UART_IIR_ID_MASK;

	return (enum al_uart_intr_id)intr_id;
}

unsigned int al_uart_status_get(
		struct al_uart_if *uart_if)
{
	uint32_t uart_stat;
	struct al_uart_regs *regs;

	al_assert(uart_if);

	regs = (struct al_uart_regs *)uart_if->regs;

	uart_stat = al_reg_read32(&regs->usr);

	return uart_stat;
}

void al_uart_tx_trig_set(
		struct al_uart_if		*uart_if,
		enum al_uart_tx_fifo_trigger	tx_trigger)
{
	struct al_uart_regs *regs;

	al_assert(uart_if);

	regs = (struct al_uart_regs *)uart_if->regs;

	al_reg_write32(&regs->stet, (uint32_t)tx_trigger);
}

void al_uart_rx_trig_set(
		struct al_uart_if		*uart_if,
		enum al_uart_rx_fifo_trigger	rx_trigger)
{
	struct al_uart_regs *regs;

	al_assert(uart_if);

	regs = (struct al_uart_regs *)uart_if->regs;

	al_reg_write32(&regs->srt, (uint32_t)rx_trigger);
}

void al_uart_intr_unmask(
		struct al_uart_if *uart_if,
		unsigned int unmask_val)
{
	uint32_t ier_value;
	struct al_uart_regs *regs;

	al_assert(uart_if);

	regs = (struct al_uart_regs *)uart_if->regs;

	ier_value = al_reg_read32(&regs->dlh_ier);

	al_reg_write32(&regs->dlh_ier, ier_value | unmask_val);
}

void al_uart_intr_mask(
		struct al_uart_if *uart_if,
		unsigned int mask_val)
{
	uint32_t ier_value;
	struct al_uart_regs *regs;

	al_assert(uart_if);

	regs = (struct al_uart_regs *)uart_if->regs;

	ier_value = al_reg_read32(&regs->dlh_ier);

	al_reg_write32(&regs->dlh_ier, ier_value & ~mask_val);
}

unsigned int al_uart_rx_fifo_level_get(
		struct al_uart_if *uart_if)
{
	uint32_t rx_fifo_level;
	struct al_uart_regs *regs;

	al_assert(uart_if);

	regs = (struct al_uart_regs *)uart_if->regs;

	rx_fifo_level = al_reg_read32(&regs->rfl);

	return rx_fifo_level;
}

unsigned int al_uart_tx_fifo_level_get(
		struct al_uart_if *uart_if)
{
	uint32_t tx_fifo_free_entr;
	struct al_uart_regs *regs;

	al_assert(uart_if);

	regs = (struct al_uart_regs *)uart_if->regs;

	tx_fifo_free_entr = UART_FIFO_DEPTH -
			(uint8_t)al_reg_read32(&regs->tfl);

	return tx_fifo_free_entr;
}

void al_uart_ptime_enable(
		struct al_uart_if *uart_if)
{
	struct al_uart_regs *regs;
	uint32_t ier_value;

	al_assert(uart_if);

	regs = (struct al_uart_regs *)uart_if->regs;
	ier_value = al_reg_read32(&regs->dlh_ier);
	AL_REG_BIT_SET(ier_value, AL_UART_IER_PTIME_SHIFT);

	/*
	 * PTIME bit is writtable ONLY when THRE_MODE_USER parammeter
	 * is enabled in the system => before write this bit check
	 * if it possible at all
	 */
	if (al_reg_read32(&regs->cpr) & AL_UART_CPR_THRE_MODE_MASK)
		al_reg_write32(&regs->dlh_ier, ier_value);
}

void al_uart_ptime_disable(
		struct al_uart_if *uart_if)
{
	struct al_uart_regs *regs;
	uint32_t ier_value;

	al_assert(uart_if);

	regs = (struct al_uart_regs *)uart_if->regs;
	ier_value = al_reg_read32(&regs->dlh_ier);
	AL_REG_BIT_CLEAR(ier_value, AL_UART_IER_PTIME_SHIFT);

	/*
	 * PTIME bit is writtable ONLY when THRE_MODE_USER parammeter
	 * is enabled in the system => before write this bit check
	 * if it possible at all
	 */
	if (al_reg_read32(&regs->cpr) & AL_UART_CPR_THRE_MODE_MASK)
		al_reg_write32(&regs->dlh_ier, ier_value);
}

int al_uart_is_input_available(
		struct al_uart_if *uart_if)
{
	int is_input_available;

	struct al_uart_regs *regs;

	al_assert(uart_if);

	regs = (struct al_uart_regs *)uart_if->regs;

	is_input_available = al_reg_read32(&regs->lsr) &
				AL_UART_LINE_STATUS_DATA_READY_MASK;

	return is_input_available;
}

/** @} */

