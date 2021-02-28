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
 * @defgroup group_spi	SPI
 *  @{
 *
 * @file   al_hal_spi.h
 * @brief Header file for the SPI HAL driver
 *
 */

#include "al_hal_spi.h"
#include "al_hal_spi_regs.h"
#include "al_hal_pbs_utils.h"

/* The size of RX/TX FIFO Queues */
#define TX_FIFO_DEPTH		32
#define RX_FIFO_DEPTH		32

/* RX, TX flr timeout */
#define RX_TX_FLR_TIMEOUT_US	1000
#define RX_TX_FLR_TIMEOUT_DELAY_FACTOR	100

/* SPI service functions */
/*
 * Check if the SPI device is currently enabled
 *
 * @param [i/o]	spi_if
 *		The SPI instance we are working with
 *
 * @return	int
 *		1 if enabled
 *		0 otherwise
 */
static inline int al_spi_is_enabled(struct al_spi_interface *spi_if)
{
	return al_reg_read16(&spi_if->regs->ssienr);
}

/*
 * Enable the SPI device
 *
 * @param [i/o]	spi_if
 *		The SPI instance we are working with
 *
 */
static inline void al_spi_enable(struct al_spi_interface *spi_if)
{
	al_reg_write16(&spi_if->regs->ssienr,
				AL_SPI_SSIENR_ENABLE);
}

/*
 * Disable the SPI device
 *
 * Note that disabling the device during ongoing transaction will result with
 * the transaction being terminated (all tx configuration will be lost)
 *
 * @param [i/o]	spi_if
 *		The SPI instance we are working with
 *
 */
static inline void al_spi_disable(struct al_spi_interface *spi_if)
{
	al_reg_write16(&spi_if->regs->ssienr,
				AL_SPI_SSIENR_DISABLE);
}

/*
 * waits until reg&mask==val.
 * returns 0 if function succeeds, -ETIME if timer expired.
 */
static int al_spi_wait_on_sr(
	struct al_spi_regs __iomem	*regs,
	uint16_t			mask,
	uint16_t			val,
	uint32_t			timeout)
{
	uint16_t __iomem *reg = &regs->sr;
	uint32_t watchdog = 1;
	uint16_t reg_val;

	while (1) {
		reg_val = al_reg_read16(reg);

		if (reg_val & AL_SPI_SR_RFF_FULL) {
			al_err("%s: RX fifo full!\n", __func__);
			return -ETIME;
		}

		if (val == (reg_val & mask))
			break;

		if (watchdog < timeout) {
			al_udelay(1); /* wait fot 1us */
			++watchdog;
		} else {
			al_err("%s: Reading %p encountered timeout\n",
					__func__, reg);
			al_err("%s: val=%08X ,mask=%08X ,timeout=%08X\n"
					, __func__, val, mask, timeout);
			return -ETIME;
		}
	}

	return 0;
}

static inline int al_spi_wait_for_rx_fifo_not_empty(
		struct al_spi_interface *spi_if,
		uint32_t timeout)
{
	return al_spi_wait_on_sr(
		spi_if->regs,
		AL_SPI_SR_RFNE_MASK,
		AL_SPI_SR_RFNE_NOT_EMPTY,
		timeout);
}

static inline int al_spi_wait_for_tx_fifo_not_full(
		struct al_spi_interface *spi_if,
		uint32_t timeout)
{
	return al_spi_wait_on_sr(
		spi_if->regs,
		AL_SPI_SR_TFNF_MASK,
		AL_SPI_SR_TFNF_NOT_FULL,
		timeout);
}

static inline int al_spi_wait_for_tx_fifo_empty(
		struct al_spi_interface *spi_if,
		uint32_t timeout)
{
	return al_spi_wait_on_sr(
		spi_if->regs,
		AL_SPI_SR_TFE_MASK,
		AL_SPI_SR_TFE_EMPTY,
		timeout);
}

static inline int al_spi_wait_for_sr_idle(
		struct al_spi_interface *spi_if,
		uint32_t timeout)
{
	return al_spi_wait_on_sr(
		spi_if->regs,
		AL_SPI_SR_BUSY_MASK,
		AL_SPI_SR_BUSY_IDLE,
		timeout);
}

static void al_spi_tmod_set_aux_aux(struct al_spi_regs __iomem	*spi_regs, unsigned int tmod)
{
	uint16_t ctrl0_reg_val;

	ctrl0_reg_val = al_reg_read16(&spi_regs->ctrlr0);
	ctrl0_reg_val = (ctrl0_reg_val & ~AL_SPI_CTRLR0_TMOD_MASK) |
			(tmod & AL_SPI_CTRLR0_TMOD_MASK);
	al_reg_write16(&spi_regs->ctrlr0, ctrl0_reg_val);
}

static void al_spi_tmod_set_aux(struct al_spi_interface *spi_if, unsigned int tmod)
{
	al_spi_tmod_set_aux_aux(spi_if->regs, tmod);
}

/*
 * Set the baud rate of the SPI device
 *
 * The baud rate determines the SPI device's clk frequency, according to this
 * formula: spi_clk = input_clk / baud_rate
 * Note that the baud rate must be even, so if the divisor is odd, we round it
 * up to an even number (round up, so the spi frequency would be lower than
 * requested - rather than higher)
 * SPI device must be disabled when changing the baud rate
 *
 * @param [i/o]	spi_if
 *		The SPI instance we are working with
 * @param [in]	spi_freq
 *		The device frequency we'd like to have, measured in Hz
 */
static void al_spi_set_baudr_aux(
	struct al_spi_regs __iomem	*spi_regs,
	unsigned int			input_clk_freq,
	unsigned int			spi_freq)
{
	uint32_t clk_divider;

	clk_divider = (input_clk_freq % spi_freq == 0) ?
		(input_clk_freq / spi_freq) : ((input_clk_freq / spi_freq) + 1);
	/* if the clk_divider is odd, even it up */
	if ((clk_divider & 0x1) != 0x0)
		clk_divider += 0x1;

	al_reg_write16(&spi_regs->baudr, clk_divider);
}

static void al_spi_set_baudr(struct al_spi_interface *spi_if, uint32_t spi_freq)
{
	al_spi_set_baudr_aux(spi_if->regs, spi_if->input_clk_freq, spi_freq);
}

/* SPI API functions */

void al_spi_init(struct al_spi_interface *spi_if,
		void __iomem *spi_regs_base,
		uint32_t input_clk_freq)
{
	al_dbg("%s: Entered..\n", __func__);
	/* Initialize the internal database */
	spi_if->regs = (struct al_spi_regs __iomem *)spi_regs_base;
	spi_if->input_clk_freq = input_clk_freq;
	spi_if->curr_bus_owner = AL_SPI_CS_NONE;

	/* HW initialization */
	al_spi_disable(spi_if);

	/*
	 * Define the basic properties of our SPI device:
	 *   TMOD	the transfer mode of the device (4 modes, according to
	 *		the ssi protocol)
	 *   SCPOL	clock polarity, determines whether the inactive state
	 *		of the serial clock is high or low
	 *   SCPH	clock phase, determines whether the serial transfer
	 *		begins with the falling edge of the slave select signal,
	 *		or the first edge of the serial clock
	 *   FRF	defines clock phase available directly after reset:
	 *		0- SPI serial data captured on the first clock edge
	 *		1- SPI serial data captured on the second clock edge
	 *   DFS	data frame size
	 */
	al_reg_write16(&spi_if->regs->ctrlr0,
			AL_SPI_CTRLR0_TMOD_EEPROM
			| AL_SPI_CTRLR0_SCPOL_LOW
			| AL_SPI_CTRLR0_SCPH_SS
			| AL_SPI_CTRLR0_FRF_MOTOROLA
			| AL_SPI_CTRLR0_DFS_8_BITS);
}

int al_spi_claim_bus(struct al_spi_interface *spi_if,
			uint32_t freq,
			enum al_spi_phase_t phase,
			enum al_spi_polarity_t polarity,
			uint32_t cs)
{
	int ret = 0;
	uint16_t ctrl0_reg_val;

	if (spi_if->curr_bus_owner != AL_SPI_CS_NONE) {
		al_err("%s: bus is not free.\n", __func__);
		return -EINVAL;
	}

	/* Disable the device, in order to configure the necessary registers */
	al_spi_disable(spi_if);

	/* Config CTRLR0 according to input parameters */
	ctrl0_reg_val = al_reg_read16(&spi_if->regs->ctrlr0);

	ctrl0_reg_val &= ~AL_SPI_CTRLR0_SCPOL_MASK;
	ctrl0_reg_val |= (polarity<<AL_SPI_CTRLR0_SCPOL_SHIFT);

	ctrl0_reg_val &= ~AL_SPI_CTRLR0_SCPH_MASK;
	ctrl0_reg_val |= (phase<<AL_SPI_CTRLR0_SCPH_SHIFT);

	al_reg_write16(&spi_if->regs->ctrlr0, ctrl0_reg_val);

	/* Config BAUD RATE according to input parameters */
	al_spi_set_baudr(spi_if, freq);

	spi_if->curr_bus_owner = SPI_CS_INDEX_TO_VAL(cs);

	return ret;
}

void al_spi_release_bus(struct al_spi_interface *spi_if, uint32_t cs_index)
{
	if (spi_if->curr_bus_owner != SPI_CS_INDEX_TO_VAL(cs_index)) {
		al_err("%s: Failed. Input slave isn't the current bus holder\n",
			__func__);
		al_err("\tCurrent bus holder = 0x%04X, slave CS = 0x%04X\n",
			spi_if->curr_bus_owner, SPI_CS_INDEX_TO_VAL(cs_index));
		return;
	}

	al_spi_disable(spi_if);
	spi_if->curr_bus_owner = AL_SPI_CS_NONE;
}

unsigned int al_spi_get_baudr(
	struct al_spi_interface	*spi_if)
{
	uint32_t clk_divider = al_reg_read16(&spi_if->regs->baudr);

	/* if the clk_divider is odd, even it up */
	if ((clk_divider & 0x1) != 0x0)
		clk_divider += 0x1;

	return spi_if->input_clk_freq / clk_divider;
}

void al_spi_tmod_set(struct al_spi_interface *spi_if, enum al_spi_tmod_t tmod)
{
	unsigned int tmod_val;

	switch(tmod) {
	case AL_SPI_TMOD_TRANS_RECV:
		tmod_val = AL_SPI_CTRLR0_TMOD_TRANS_RECV;
		break;
	case AL_SPI_TMOD_TRANS:
		tmod_val = AL_SPI_CTRLR0_TMOD_TRANS;
		break;
	case AL_SPI_TMOD_RECV:
		tmod_val = AL_SPI_CTRLR0_TMOD_RECV;
		break;
	case AL_SPI_TMOD_EEPROM:
		tmod_val = AL_SPI_CTRLR0_TMOD_EEPROM;
		break;
	default:
		al_err("%s: received an invalid tmod value: %x\n", __func__, tmod);
		return;
	}

	al_spi_tmod_set_aux(spi_if, tmod_val);
}

int al_spi_read(struct al_spi_interface *spi_if,
		uint8_t const *cmd,
		uint8_t cmd_len,
		uint8_t *din,
		uint32_t bytelen,
		uint32_t cs,
		uint32_t timeout)
{
	int ret = 0;
	int cmd_idx = 0;
	int rx_avail;
	int watchdog = 0;
	/*
	 * The DW SPI core requires 'dummy' data to be inserted into the TX
	 * FIFO, in order to perform the read procedure. The data will be
	 * ignored during the read command, as its only purpose is enabling
	 * the interface to perform read transaction.
	 */
	int dummy_tx = bytelen;

	al_dbg("%s: Entered with params:\n\t", __func__);
	al_dbg("cmd[0]=0x%02X | cmd_len=0x%02X | bytelen=0x%08X\n",
			cmd[0], cmd_len, bytelen);

	al_spi_disable(spi_if);

	/* Set SPI mode */
	al_spi_tmod_set_aux(spi_if, AL_SPI_CTRLR0_TMOD_TRANS_RECV);

	/* -- Command Phase -- */

	/*
	 * Enable the SPI interface, and disable the SER:
	 * once the FIFO is empty, the transfer is terminated - therefore we
	 * must make sure that the fifo isn't empty during the transfer
	 */
	al_reg_write16(&spi_if->regs->ser, AL_SPI_CS_NONE);
	al_spi_enable(spi_if);

	for (cmd_idx = 0 ; cmd_idx < cmd_len ; ++cmd_idx) {
		/* Check that the transmit FIFO isn't full per each byte tx */
		ret = al_spi_wait_for_tx_fifo_not_full(spi_if, timeout);
		if (ret != 0) {
			al_spi_disable(spi_if);
			return ret;
		}
		/* FIFO not full -> write command byte*/
		al_reg_write16(&spi_if->regs->dr[0], cmd[cmd_idx]);
	}

	/* Fill the TX FIFO as much as possible before selecting the slave */
	while (dummy_tx) {
		if (((al_reg_read16(&spi_if->regs->sr)&AL_SPI_SR_TFNF_MASK)
			== AL_SPI_SR_TFNF_FULL))
			break;

		al_reg_write16(&spi_if->regs->dr[0], 0);
		dummy_tx--;
	}

	/* -- Read Data Phase -- */

	al_reg_write16(&spi_if->regs->ser, SPI_CS_INDEX_TO_VAL(cs));

	while (bytelen > 0) {
		/* Get number of pending tx bytes */
		int tx_used = al_reg_read16(&spi_if->regs->txflr);
		int rx_used = rx_avail = al_reg_read16(&spi_if->regs->rxflr);

		if (rx_avail) {
			watchdog = 0;
		} else {
			watchdog++;
			if (!(watchdog % RX_TX_FLR_TIMEOUT_DELAY_FACTOR))
				al_udelay(1);
		}

		if (watchdog == (RX_TX_FLR_TIMEOUT_US * RX_TX_FLR_TIMEOUT_DELAY_FACTOR)) {
			ret = -ETIME;
			break;
		}

		/* Fill the FIFO with dummy data (1 byte for each read byte)
		 * Make sure RX FIFO have enough space for incoming data.
		 * Reading the dummy bytes we also have to make sure we transmit
		 * 0xFF bytes back to prevent the slave confusion.
		 */
		while (dummy_tx && (tx_used < TX_FIFO_DEPTH)
				&& (rx_used < RX_FIFO_DEPTH)) {
			al_reg_write16(&spi_if->regs->dr[0], 0xFF);
			dummy_tx--;
			tx_used++;
			rx_used++;
		}

		while (rx_avail) {
			*din = al_reg_read16(&spi_if->regs->dr[0]);

			if (!cmd_len) {
				din++;
				bytelen--;
			} else if (cmd_len) {
				cmd_len--;
			}

			rx_avail--;
		}
	}

	/* Wait until SPI transaction is done */
	if (!ret)
		ret = al_spi_wait_for_sr_idle(spi_if, timeout);

	/* Disable the SPI interface */
	al_reg_write16(&spi_if->regs->ser, AL_SPI_CS_NONE);
	al_spi_disable(spi_if);

	return ret;
}

int al_spi_write(struct al_spi_interface *spi_if,
		uint8_t const *cmd,
		uint8_t cmd_len,
		uint8_t const *dout,
		uint32_t bytelen,
		uint32_t cs,
		uint32_t timeout)
{
	int ret = 0;
	int ser_set = 0, cmd_idx = 0;
	int watchdog = 0;

	al_dbg("%s: Entered with params:\n\t", __func__);
	al_dbg("cmd[0]=0x%02X | bytelen=0x%08X | cs = 0x%04X\n",
			cmd_len > 0 ? cmd[0] : 0xff, bytelen, cs);

	al_spi_disable(spi_if);

	/* Set SPI mode */
	al_spi_tmod_set_aux(spi_if, AL_SPI_CTRLR0_TMOD_TRANS);

	/* -- Command Phase -- */

	/*
	 * Enable the SPI interface, and disable the SER:
	 * once the FIFO is empty, the transfer is terminated - therefore we
	 * must make sure that the fifo isn't empty during the transfer
	 */
	al_reg_write16(&spi_if->regs->ser, AL_SPI_CS_NONE);
	al_spi_enable(spi_if);

	for (cmd_idx = 0 ; cmd_idx < cmd_len ; ++cmd_idx) {
		/* Check that the transmit FIFO isn't full per each byte tx */
		ret = al_spi_wait_for_tx_fifo_not_full(spi_if, timeout);
		if (ret != 0) {
			al_spi_disable(spi_if);
			return ret;
		}
		/* FIFO not full -> write command byte*/
		al_reg_write16(&spi_if->regs->dr[0], cmd[cmd_idx]);
	}

	/* -- Write Data Phase -- */

	while (bytelen > 0) {
		/* Get number of pending tx bytes */
		int tx_used = al_reg_read16(&spi_if->regs->txflr);

		if (tx_used < TX_FIFO_DEPTH) {
			watchdog = 0;
		} else {
			watchdog++;
			if (!(watchdog % RX_TX_FLR_TIMEOUT_DELAY_FACTOR))
				al_udelay(1);
		}

		if (watchdog == (RX_TX_FLR_TIMEOUT_US * RX_TX_FLR_TIMEOUT_DELAY_FACTOR)) {
			ret = -ETIME;
			break;
		}

		/* Fill the FIFO with data to be written */
		while (bytelen && (tx_used < TX_FIFO_DEPTH)) {
			al_reg_write16(&spi_if->regs->dr[0], *dout);
			dout++;
			tx_used++;
			bytelen--;
		}

		/* Set the Slave Enable Register on the first iteration */
		if (ser_set == 0) {
			al_reg_write16(&spi_if->regs->ser,
					SPI_CS_INDEX_TO_VAL(cs));
			ser_set = 1;
		}
	}

	/* Set the Slave Enable Register if it wasn't already set */
	if (ser_set == 0)
		al_reg_write16(&spi_if->regs->ser,
				SPI_CS_INDEX_TO_VAL(cs));

	/* Wait until SPI transaction is done */
	if (!ret) {
		ret = al_spi_wait_for_tx_fifo_empty(spi_if, timeout);
		if (ret) {
			al_err("%s: al_spi_wait_for_tx_fifo_empty failed!\n", __func__);
			return ret;
		}
	}

	if (!ret) {
		ret = al_spi_wait_for_sr_idle(spi_if, timeout);
		if (ret) {
			al_err("%s: al_spi_wait_for_sr_idle failed!\n", __func__);
			return ret;
		}
	}

	/* Disable the SPI interface */
	al_reg_write16(&spi_if->regs->ser, AL_SPI_CS_NONE);
	al_spi_disable(spi_if);


	return ret;
}

unsigned int al_spi_raw_intr_get(
		struct al_spi_interface *spi_if)
{
	struct al_spi_regs *regs = (struct al_spi_regs *)spi_if->regs;

	return al_reg_read16(&regs->risr);
}

unsigned int al_spi_masked_intr_get(
		struct al_spi_interface *spi_if)
{
	struct al_spi_regs *regs = (struct al_spi_regs *)spi_if->regs;

	return al_reg_read16(&regs->isr);
}

void al_spi_intr_enable(
		struct al_spi_interface *spi_if,
		unsigned int enable_val)
{
	struct al_spi_regs *regs = (struct al_spi_regs *)spi_if->regs;
	uint16_t curr_val;

	curr_val = al_reg_read16(&regs->imr);

	al_reg_write16(&regs->imr, curr_val | enable_val);
}

void al_spi_intr_disable(
		struct al_spi_interface *spi_if,
		unsigned int disable_val)
{
	struct al_spi_regs *regs = (struct al_spi_regs *)spi_if->regs;
	uint16_t curr_val;

	curr_val = al_reg_read16(&regs->imr);

	al_reg_write16(&regs->imr, curr_val & ~disable_val);
}

void al_spi_intr_clear(
		struct al_spi_interface *spi_if,
		unsigned int clear_val)
{
	struct al_spi_regs *regs;

	al_assert(spi_if);

	regs = (struct al_spi_regs *)spi_if->regs;

	/* SW can't clear 'tx empty' and 'rx full' interrupts */
	al_assert((clear_val & AL_SPI_INTR_TXEIM) == 0);
	al_assert((clear_val & AL_SPI_INTR_RXFIM) == 0);

	if (clear_val & AL_SPI_INTR_TXOIM)
		al_reg_read16(&regs->txoicr);
	if (clear_val & AL_SPI_INTR_RXUIM)
		al_reg_read16(&regs->rxuicr);
	if (clear_val & AL_SPI_INTR_RXOIM)
		al_reg_read16(&regs->rxoicr);
	if (clear_val & AL_SPI_INTR_MSTIM)
		al_reg_read16(&regs->msticr);
}

void al_spi_cs_override_set(
		void __iomem		*spi_regs_base)
{
	struct al_spi_regs *regs = (struct al_spi_regs __iomem *)spi_regs_base;

	al_reg_write16(&regs->ssi_ovr,
			AL_SPI_SSI_OVR_CS_0
			| AL_SPI_SSI_OVR_CS_1
			| AL_SPI_SSI_OVR_CS_2
			| AL_SPI_SSI_OVR_CS_3);
}

void al_spi_cs_override_clear(
		void __iomem		*spi_regs_base)
{
	struct al_spi_regs *regs = (struct al_spi_regs __iomem *)spi_regs_base;

	al_reg_write16(&regs->ssi_ovr, 0);
}

void al_spi_cs_set(
		void __iomem		*spi_regs_base,
		unsigned int		cs)
{
	struct al_spi_regs *regs = (struct al_spi_regs __iomem *)spi_regs_base;

	al_reg_write16(&regs->ser, SPI_CS_INDEX_TO_VAL(cs));
}

int al_spi_slave_cfg(
		struct al_spi_interface	*spi_if,
		enum al_spi_phase_t	phase,
		enum al_spi_polarity_t	polarity,
		al_bool enable_back)
{
	int ret = 0;
	uint16_t ctrl0_reg_val;
	int spi_enabled;

	spi_enabled = al_spi_is_enabled(spi_if);

	/* If originally enabled - Disable the device, to configure the necessary registers */
	if (spi_enabled)
		al_spi_disable(spi_if);

	/* Config CTRLR0 according to input parameters */
	ctrl0_reg_val = al_reg_read16(&spi_if->regs->ctrlr0);

	ctrl0_reg_val &= ~AL_SPI_CTRLR0_SCPOL_MASK;
	ctrl0_reg_val |= (polarity<<AL_SPI_CTRLR0_SCPOL_SHIFT);

	ctrl0_reg_val &= ~AL_SPI_CTRLR0_SCPH_MASK;
	ctrl0_reg_val |= (phase<<AL_SPI_CTRLR0_SCPH_SHIFT);

	al_reg_write16(&spi_if->regs->ctrlr0, ctrl0_reg_val);

	/* If requested: - Enable the device back */
	if (enable_back)
		al_spi_enable(spi_if);

	return ret;
}

void al_spi_master_glue_enable(
	void __iomem		*spi_regs_base,
	unsigned int		cs)
{
	struct al_spi_regs __iomem *spi_regs = (struct al_spi_regs __iomem *)spi_regs_base;
	uint16_t ctrl0_reg_val;

	al_reg_write16(&spi_regs->ssienr, AL_SPI_SSIENR_DISABLE);

	/* Reset state machines of DMA and of Anpa-AXI-to-SPI-controller ("glue logic") in case we
	 * are in the middle of some old transaction. For this we send pulse
	 * "disable_everything->enable_everything" to DMA control register:
	 */
	al_reg_write16(&spi_regs->dmacr, 0);
	al_reg_write16(&spi_regs->dmacr, AL_SPI_DMACR_RDMAE | AL_SPI_DMACR_TDMAE);

	ctrl0_reg_val = al_reg_read16(&spi_regs->ctrlr0);
	ctrl0_reg_val = (ctrl0_reg_val & ~AL_SPI_CTRLR0_TMOD_MASK) |
			(AL_SPI_CTRLR0_TMOD_EEPROM & AL_SPI_CTRLR0_TMOD_MASK);
	al_reg_write16(&spi_regs->ctrlr0, ctrl0_reg_val);

	al_reg_write16(&spi_regs->ser, SPI_CS_INDEX_TO_VAL(cs));

	al_data_memory_barrier();
}

void al_spi_master_glue_qpi_config_pins(
	void __iomem		*pbs_regs_base,
	unsigned int		mask,
	unsigned int		drive_mask,
	unsigned int		val_mask)
{
	struct al_pbs_regs __iomem *pbs_regs = (struct al_pbs_regs __iomem *)pbs_regs_base;
	unsigned int val;
	unsigned int dev_id = al_pbs_dev_id_get(pbs_regs_base);
	al_assert(dev_id >= PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V3);
	val = al_reg_read32(&pbs_regs->qspi.config);
	val &= ~((PBS_QSPI_CONFIG_TXD_VALUE_MASK &
		((mask >> 2) << PBS_QSPI_CONFIG_TXD_VALUE_SHIFT)) |
		(PBS_QSPI_CONFIG_TXD_DRV_EN_MASK &
		(mask >> 2) << PBS_QSPI_CONFIG_TXD_DRV_EN_SHIFT));
	val |= ((drive_mask & mask) >> 2) << PBS_QSPI_CONFIG_TXD_DRV_EN_SHIFT;
	val |= ((val_mask & mask) >> 2) << PBS_QSPI_CONFIG_TXD_VALUE_SHIFT;
	al_reg_write32(&pbs_regs->qspi.config, val);
}

static void al_spi_master_glue_qpi_en_aux(
	void __iomem		*spi_regs_base,
	void __iomem		*pbs_regs_base,
	al_bool			en)
{
	struct al_spi_regs __iomem *spi_regs = (struct al_spi_regs __iomem *)spi_regs_base;
	struct al_pbs_regs __iomem *pbs_regs = (struct al_pbs_regs __iomem *)pbs_regs_base;

	unsigned int dev_id = al_pbs_dev_id_get(pbs_regs_base);
	unsigned int dev_rev_id = al_pbs_dev_rev_id_get(pbs_regs_base);
	al_bool wa = (dev_id == PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V3) &&
			(dev_rev_id == PBS_UNIT_CHIP_ID_DEV_REV_ID_ALPINE_V3_TC);

	if ((!en) && (dev_id < PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V3))
		return;

	al_assert(dev_id >= PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V3);

	if (en) {
		al_reg_write32_masked(&pbs_regs->qspi.enable,
			PBS_QSPI_ENABLE_ENABLE,
			PBS_QSPI_ENABLE_ENABLE);

		if (wa)
			al_spi_tmod_set_aux_aux(spi_regs, AL_SPI_CTRLR0_TMOD_TRANS);

		al_reg_write32_masked(&pbs_regs->qspi.start_trig,
			PBS_QSPI_START_TRIG_START,
			PBS_QSPI_START_TRIG_START);
		al_reg_read32(&pbs_regs->qspi.start_trig);
		while (al_reg_read16(&spi_regs->sr) & AL_SPI_SR_BUSY_ACTIVE)
			;

		if (wa) {
			al_reg_write16(&spi_regs->ssienr, AL_SPI_SSIENR_DISABLE);
			al_spi_tmod_set_aux_aux(spi_regs, AL_SPI_CTRLR0_TMOD_EEPROM);
		}
	} else if (al_reg_read32(&pbs_regs->qspi.enable) & PBS_QSPI_ENABLE_ENABLE) {
		if (wa)
			al_spi_tmod_set_aux_aux(spi_regs, AL_SPI_CTRLR0_TMOD_TRANS);

		al_reg_write32_masked(&pbs_regs->qspi.stop_trig,
			PBS_QSPI_STOP_TRIG_STOP,
			PBS_QSPI_STOP_TRIG_STOP);
		al_reg_read32(&pbs_regs->qspi.stop_trig);
		while (al_reg_read16(&spi_regs->sr) & AL_SPI_SR_BUSY_ACTIVE)
			;

		if (wa) {
			al_reg_write16(&spi_regs->ssienr, AL_SPI_SSIENR_DISABLE);
			al_spi_tmod_set_aux_aux(spi_regs, AL_SPI_CTRLR0_TMOD_EEPROM);
		}

		al_reg_write32_masked(&pbs_regs->qspi.enable, PBS_QSPI_ENABLE_ENABLE, 0);
	}
}

void al_spi_master_glue_qpi_en(
	void __iomem		*spi_regs_base,
	void __iomem		*pbs_regs_base,
	al_bool			en)
{
	al_spi_master_glue_qpi_en_aux(spi_regs_base, pbs_regs_base, en);
	al_data_memory_barrier();
}

#define SPI_SPD_THR_RX_SAMPLE_DLY	50
#define SPI_SPD_THR_FAST_READ		25
#define SPI_OPCODE_READ_FAST		0x0B
#define SPI_OPCODE_READ_NORMAL		0x03

void al_spi_master_glue_speed_set(
	void __iomem		*spi_regs_base,
	void __iomem		*pbs_regs_base,
	al_bool			qpi,
	unsigned int		ref_clk_freq_hz,
	unsigned int		speed_mhz)
{
	struct al_spi_regs __iomem *spi_regs = (struct al_spi_regs __iomem *)spi_regs_base;
	struct al_pbs_regs __iomem *pbs_regs = (struct al_pbs_regs __iomem *)pbs_regs_base;

	al_reg_write16(&spi_regs->ssienr, AL_SPI_SSIENR_DISABLE);
	while (al_reg_read16(&spi_regs->sr) & AL_SPI_SR_BUSY_ACTIVE)
		;

	al_reg_write16(&spi_regs->rx_sample_dly, (speed_mhz < SPI_SPD_THR_RX_SAMPLE_DLY) ? 0 : 3);

	al_spi_set_baudr_aux(spi_regs, ref_clk_freq_hz, speed_mhz * 1000000);

	al_reg_write32_masked(&pbs_regs->unit.spi_mst_conf_1,
		PBS_UNIT_SPI_MST_CONF_1_CFG_SPI_MST_FAST_RD,
		((speed_mhz > SPI_SPD_THR_FAST_READ) || qpi) ?
		PBS_UNIT_SPI_MST_CONF_1_CFG_SPI_MST_FAST_RD : 0);

	al_reg_write32_masked(&pbs_regs->unit.spi_mst_conf_0,
		PBS_UNIT_SPI_MST_CONF_0_CFG_SPI_MST_RD_CMD_MASK,
		((speed_mhz > SPI_SPD_THR_FAST_READ) ?
		SPI_OPCODE_READ_FAST : SPI_OPCODE_READ_NORMAL) <<
		PBS_UNIT_SPI_MST_CONF_0_CFG_SPI_MST_RD_CMD_SHIFT);

	al_spi_master_glue_qpi_en_aux(spi_regs_base, pbs_regs_base, qpi);

	al_data_memory_barrier();
}

/** @} end of SPI group */
