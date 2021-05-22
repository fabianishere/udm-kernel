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
 * @ingroup group_pbs
 *  @{
 *
 * I2C is a serial open-drain multi-drop bus that is used to attach low-speed
 * peripherals to an embedded system.
 *
 * This I2C Hardware Abstraction Layer provides Annapurna Labs' implementation
 * for initializing and communicating with an external I2C device, using
 * convinient API functions.
 *
 * Common operation example:
 * @code
 *      int main()
 *      {
 *              struct al_i2c_interface i2c_master_if;
 *		struct al_i2c_configuration i2c_master_config;
 *		uint8_t offset[4];
 *		uint8_t read_buffer[256];
 *		uint8_t write_buffer[256];
 *              int interrupt_val = 0, ret = 0;
 *
 *		// initialize the i2c interface
 *		//	configure a master using default threshold arguments,
 *		//	7 bit address space and standard speed.
 *		i2c_master_config.mmode = AL_I2C_MASTER_MODE_MASTER;
 *		i2c_master_config.configure_thresholds = 0;
 *		i2c_master_config.slv_addr = I2C_MASTER_SLV_ADDR;
 *		i2c_master_config.timeout = 1 * 1000 * 1000 // 1 second
 *		i2c_master_config.addr_size = AL_I2C_ADDR_SIZE_7BIT;
 *		i2c_master_config.speed = AL_I2C_SPEED_STANDARD;
 *
 *		ret = al_i2c_init(&i2c_master_if,
 *				&i2c_master_config,
 *				(void *)I2C_MASTER_REG_BASE);
 *		if (ret != 0) {
 *			printf("Initialization failed\n");
 *			return ret;
 *		}
 *
 *		// read from device 0x0010
 *		// memory device doesn't use offset.
 *		// read 4 bytes into read_buffer
 *		ret = al_i2c_read(&i2c_if,
 *				0x0010,		//target address
 *				0,		//internal address length
 *				0,		//internal address
 *				4,		//transaction byte length
 *				read_buffer);	//input buffer
 *		if (ret != 0) {
 *			printf("Read transaction failed\n");
 *			return ret;
 *		}
 *
 *		// read from device 0x0040
 *		// memory device uses 4-byte MSB-first offset.
 *		// read from offset 0x00123400
 *		// read 8 bytes into read_buffer
 *		offset[0] = 0x00;
 *		offset[1] = 0x12;
 *		offset[2] = 0x34;
 *		offset[3] = 0x00;
 *		ret = al_i2c_read(&i2c_if,
 *				0x0010,		//target address
 *				4,		//internal address length
 *				offset,	//internal address
 *				8,		//transaction byte length
 *				read_buffer);	//input buffer
 *		if (ret != 0) {
 *			printf("Read transaction failed\n");
 *			return ret;
 *		}
 *
 *		// write to device 0x0040
 *		// memory device uses 4-byte MSB-first offset.
 *		// write 0xABCD1234 to offset 0x00008000
 *		offset[0] = 0x00;
 *		offset[1] = 0x00;
 *		offset[2] = 0x80;
 *		offset[3] = 0x00;
 *
 *		write_buffer[0] = 0x34;
 *		write_buffer[1] = 0x12;
 *		write_buffer[2] = 0xCD;
 *		write_buffer[3] = 0xAB;
 *
 *		ret = al_i2c_write(&i2c_if,
 *				0x0010,		//target address
 *				4,		//internal address length
 *				offset,	//internal address
 *				4,		//transaction byte length
 *				write_buffer);	//input buffer
 *		if (ret != 0) {
 *			printf("Write transaction failed\n");
 *			return ret;
 *		}
 *
 *
 *
 *		return ret;
 *      }
 * @endcode
 *
 * @file   al_hal_i2c.h
 * @brief Header file for the I2C HAL driver
 *
 */

#ifndef __AL_HAL_I2C_H__
#define __AL_HAL_I2C_H__

#include "al_hal_common.h"


/* --->>> Data structures <<<--- */

struct al_i2c_interface {
	void __iomem *regs;
	unsigned int timeout;
	void (*udelay_cb)(void *context, unsigned int delay);
	void *udelay_cb_context;
};

enum al_i2c_master_t {
	AL_I2C_MASTER_PRIMARY,
	/** Pre loader */
	AL_I2C_MASTER_PLD = AL_I2C_MASTER_PRIMARY,
	AL_I2C_MASTER_SECONDARY,
	/** General */
	AL_I2C_MASTER_GEN = AL_I2C_MASTER_SECONDARY,
	AL_I2C_MASTER_NUM
};

enum al_i2c_mode_t {
	AL_I2C_MODE_SLAVE,
	AL_I2C_MODE_MASTER
};

enum al_i2c_addr_size_t {
	AL_I2C_ADDR_SIZE_7BIT,
	AL_I2C_ADDR_SIZE_10BIT
};

/* Values are important for proper behavior */
enum al_i2c_speed_t {
	AL_I2C_SPEED_STANDARD	= (1 << 1),	/* <100kbit/sec */
	AL_I2C_SPEED_FAST	= (2 << 1),	/* <400kbit/sec */
	AL_I2C_SPEED_HIGH	= (3 << 1),	/* <3.4Mbit/sec */
};

/*
 * The following struct is the main I2C configuration struct, it is being used
 * for passing arguments to the i2c initialization function.
 *
 * mode
 *	Master Mode, configures the device to operate as an
 *	I2C Master, or I2C Slave.
 * configure_thresholds
 *	Two possible values:
 *	1 - use the thresholds arguments to configure the I2C device
 *	0 - ignore thresholds arguments, use the I2C device's default values
 *  Choose 0 unless you have specific needs
 * rx/tx_threshold
 *	The amount of bytes in the rx/tx FIFO that assert the rx/tx_tl
 *	(Threshold Level) interrupt.
 * local_slv_addr
 *	Holds the slave address when the I2C is operating as a slave.
 *	For 7-bit addressing, only bits [6:0] are used.
 *      The default values cannot be any of the reserved address locations
 *      0x00 to 0x07, or 0x78 to 0x7f.
 * timeout
 *	Time to wait before declaring a timeout error, measured in usec
 * addr_size	[MASTER ONLY]
 *	Enable/Disable the 10 bit address mode - relevant only for
 *	master mode.
 * speed	[MASTER ONLY]
 *	The I2C device speed - relevant only for Master Mode.
*/
struct al_i2c_configuration {
	/* parameters for both slave and master */
	enum al_i2c_mode_t mode;
	int configure_thresholds;
	unsigned int rx_threshold;
	unsigned int tx_threshold;
	unsigned int local_slv_addr;
	unsigned long timeout;
	/* parameters only for master (slave ignores this parameters) */
	enum al_i2c_addr_size_t addr_size;
	enum al_i2c_speed_t speed;
};

/* The following enum values should NOT be changed, as they are being used
   internally */
enum al_i2c_intr_t {
	AL_I2C_INTR_RX_UNDER	= (1 << 0),	/* receive FIFO is empty */
	AL_I2C_INTR_RX_OVER	= (1 << 1),	/* receive FIFO is almost full
						   (above IC_RX_BUFFER_DEPTH) */
	AL_I2C_INTR_RX_FULL	= (1 << 2),	/* receive FIFO is above threshold */
	AL_I2C_INTR_TX_OVER	= (1 << 3),	/* transmit FIFO is almost full
						   (above IC_TR_BUFFER_DEPTH) */
	AL_I2C_INTR_TX_EMPTY	= (1 << 4),	/* transmit FIFO is almost empty
					   	   (below IC_TX_CL) */
	AL_I2C_INTR_RD_REQ	= (1 << 5),	/* read request received */
	AL_I2C_INTR_TX_ABRT	= (1 << 6),	/* xfer was aborted */
	AL_I2C_INTR_RX_DONE	= (1 << 7),	/* master didn't ACK last xfer */
	AL_I2C_INTR_ACTIVITY	= (1 << 8),	/* detected activity on the bus */
	AL_I2C_INTR_STOP_DET	= (1 << 9),	/* STOP condition detected */
	AL_I2C_INTR_START_DET	= (1 << 10),	/* START/RESTART condition detected */
	AL_I2C_INTR_GEN_CALL	= (1 << 11)	/* general call received */
};


/* --->>> API functions <<<--- */

/**
 * Initialize the I2C device
 *
 * @param i2c_if
 *		An allocated, non-initialized instance.
 * @param	config
 *		The I2C configuration, see the struct al_i2c_configuration
 *		for further explanation.
 * @param	i2c_regs_base
 *		The base address of the I2C configuration registers
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_i2c_init(
		struct al_i2c_interface *i2c_if,
		const struct al_i2c_configuration *config,
		void __iomem *i2c_regs_base);

/**
 * Set hold time for I2C device
 *
 * @param i2c_if
 *		The I2C instance we are working with.
 * @param	ref_clk_freq_mhz
 *		reference clock frequency in Mhz
 * @param	hold_time_ns
 *		hold time value in nanoseconds
 */
void al_i2c_hold_time_set(
		struct al_i2c_interface *i2c_if,
		unsigned int ref_clk_freq_mhz,
		unsigned int hold_time_ns);

/**
 * Replace al_udelay with custom callback for udelay function
 *
 * @param i2c_if
 *		The I2C instance we are working with.
 * @param	udelay_cb
 *		udelay callback function
 * @param	udelay_cb_context
 *		udelay callback context
 */
void al_i2c_udelay_cb_set(
		struct al_i2c_interface *i2c_if,
		void (*udelay_cb)(void *context, unsigned int delay),
		void *udelay_cb_context);

/**
 * Read from i2c memory device
 *
 * Note that this is a polling implementation, meaning that this is a blocking
 * function.
 *
 * @param i2c_if
 *		The I2C instance we are working with.
 * @param	target_i2c_addr
 *		The target address we are reading from.
 * @param	address_len
 *		The length of the address byte array.
 *		For a read that doesn't require address, use 0.
 * @param	address
 *		Address to read from (mapped inside the slave).
 *		The address need to be represented as a byte array, MSB to LSB.
 * @param	bytelen
 *		The amount of bytes to be read.
 * @param	buffer
 *		A pointer to the read data.
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_i2c_read(
		struct al_i2c_interface *i2c_if,
		unsigned int target_i2c_addr,
		uint8_t address_len,
		uint8_t *address,
		unsigned int bytelen,
		uint8_t *buffer);

/**
 * Write to i2c memory device
 *
 * @param i2c_if
 *		The I2C instance we are working with.
 * @param	target_i2c_addr
 *		The target address we are writing to.
 * @param	address_len
 *		The length of the address byte array.
 *		For a write that doesn't require address, use 0.
 * @param	address
 *		Address to write to (mapped inside the Slave).
 *		The address need to be represented as a byte array, MSB to LSB.
 * @param	bytelen
 *		The amount of bytes to be written.
 * @param	buffer
 *		A pointer to the data to be written.
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_i2c_write(
		struct al_i2c_interface *i2c_if,
		unsigned int target_i2c_addr,
		uint8_t address_len,
		uint8_t *address,
		unsigned int bytelen,
		uint8_t *buffer);

/**
 * Get the RAW interrupt state
 *
 * Raw interrupt state is the value of the Interrupt State regardless of the
 * interrupt mask.
 *
 * Use the I2C interrupts enumeration i.o. to parse value into separate irqs.
 *
 * @param	i2c_if
 *		The i2c instance we are working with.
 *
 * @return	Interrupt register value
 */
unsigned int al_i2c_raw_intr_get(
		struct al_i2c_interface *i2c_if);

/**
 * Get the MASKED interrupt state
 *
 * Masked interrupt state is the value of the Interrupt State considering the
 * interrupt mask: if the interrupt's corresponding mask bit is set, it will
 * be masked.
 *
 * Use the I2C interrupts enumeration i.o. to parse value into separate irqs.
 *
 * @param	i2c_if
 *		The i2c instance we are working with.
 *
 * @return	Interrupt register value
 */
unsigned int al_i2c_masked_intr_get(
		struct al_i2c_interface *i2c_if);

/**
 * Enables an interrupt
 *
 * Using the I2C interrupts enumeration in order to create the input value is
 * recommended.
 *
 * Example:
 *   al_i2c_intr_enable(&i2c_if_instance,
 *		(AL_I2C_INTR_RX_UNDER | AL_I2C_INTR_RX_OVER));
 *
 * @param	i2c_if
 *		The I2C instance we are working with
 * @param	enable_val
 *		Value holding all the interrupts to enable
 */
void al_i2c_intr_enable(
		struct al_i2c_interface	*i2c_if,
		unsigned int		enable_val);

/**
 * Disables an interrupt
 *
 * When an interrupt is disabled, the corresponding interrupt will not
 * trigger its interrupt handling function even if it has been set
 *
 * Using the SPI interrupts enumeration in order to create the input value is
 * recommended.
 *
 * Example:
 *   al_i2c_intr_disable(&i2c_if_instance,
 *		(AL_I2C_INTR_RX_UNDER | AL_I2C_INTR_RX_OVER));
 *
 * @param	i2c_if
 *		The I2C instance we are working with
 * @param	disable_val
 *		Value holding all the interrupts to disable
 */
void al_i2c_intr_disable(
		struct al_i2c_interface	*i2c_if,
		unsigned int		disable_val);

/**
 * Clear I2C interrupts
 *
 * Using the I2C interrupts enumeration in order to create the input value is
 * recommended.
 *
 * Example:
 *   al_i2c_intr_clear(&i2c_if_instance,
 *		(AL_I2C_INTR_RX_UNDER | AL_I2C_INTR_RX_OVER));
 *
 * @param	i2c_if
 *		The i2c instance we are working with.
 * @param	clear_val
 *		Value holding all the interrupts to clear
 */
void al_i2c_intr_clear(
		struct al_i2c_interface *i2c_if,
		unsigned int clear_val);

/**
 * Clear all interrupts
 *
 * @param	i2c_if
 *		The i2c instance we are working with
 */
void al_i2c_intr_clear_all(
		struct al_i2c_interface *i2c_if);

/**
 * I2C slave read
 *
 * @param	i2c_slave_if
 *		The i2c_slave instance we are working with.
  * @param	buffer
 *		A buffer to which the data is read
 * @param	buffer_size
 *		The size of the buffer
 * @param	read_done
 *		Used for notification about end of receive
 *
 * @return	unsigned int
 *		actual number of bytes that were received by the slave
 */
unsigned int al_i2c_slave_read(
		struct al_i2c_interface *i2c_slave_if,
		uint8_t *buffer,
		unsigned int buffer_size,
		al_bool *read_done);

/**
 * I2C slave write
 *
 * @param	i2c_slave_if
 *		The i2c_slave instance we are working with.
 * @param	buffer
 *		A buffer from which the data is written
 * @param	buffer_size
 *		The size of the buffer
 * @param	write_done
 *		Used for notification about end of transmit
 *
 * @return	unsigned int
 *		actual number of bytes that were transmitted by the slave
 */
unsigned int al_i2c_slave_write(
		struct al_i2c_interface *i2c_slave_if,
		uint8_t *buffer,
		unsigned int buffer_size,
		al_bool *write_done);


#endif /* __AL_HAL_I2C_H__*/

/** @} end of I2C group */

