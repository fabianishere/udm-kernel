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

#ifndef _AL_HAL_UART_H_
#define _AL_HAL_UART_H_

#include "al_hal_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup group_uart UART
 * @ingroup group_pbs
 *  @{
 *
 * This UART Hardware Abstraction Layer provides Annapurna Labs' implementation
 * for initializing and communicating with an external UART device, using
 * convenient API functions that allow:
 *	- initialize the UART device
 *	- read data from other UART device,
 *	- write data to other UART device
 *	- enable/disable interrupts
 *	- get UART,modem and line statuses
 *	- perform UART,RX/TX FIFO`s software reset
 *
 * Common operation example:
 * @code
 *
 *	struct al_uart_if uart_if;
 *	uint8_t read_buffer[100];
 *	uint8_t write_buffer[100];
 *
 *	// Interrupt handler
 *	static void pbs_uart_summary_handler(
 *					uint32_t irq,
 *					enum al_nb_gic_id gic)
 *	{
 *		enum al_uart_intr_id intr_id = al_uart_intr_id_get(&uart_if);
 *		int ret;
 *
 *		uint16_t line_status = al_uart_line_status_get(&uart_if);
 *		unsigned int avail_data_to_read;
 *		struct al_uart_regs *regs = (struct al_uart_regs *)uart_if.regs;
 *
 *		if(intr_id == AL_UART_RX_DATA_AVAIL_INTR_ID ) {
 *			al_dbg("Received Data Available Interrupt\n");
 *			if (al_reg_read32(&regs->iir_fcr) &
 *							AL_UART_IIR_FIFOSE_MASK)
 *				avail_data_to_read =
 *					al_uart_rx_fifo_level_get(&uart_if);
 *			else
 *				avail_data_to_read = 1;
 *
 *			ret = al_uart_read(&uart_if,
 *					avail_data_to_read,	//bytelen
 *					read_buffer,		//input buffer
 *					4000000);
 *			if (ret != 0) {
 *				printf("Read transaction failed\n");
 *				return;
 *			}
 *			// Write back the same data that was received
 *			al_uart_write(&uart_if,
 *					avail_data_to_read,
 *					read_buffer,
 *					4000000);
 *		}
 *		if( intr_id == AL_UART_THR_EMPTY_INTR_ID ) {
 *			al_dbg("Transmit Holding Register Empty Interrupt\n");
 *			// Disable THR_EMPTY Interrupt
 *			al_uart_intr_mask(&uart_if,
 *					AL_UART_INTR_THR_EMPTY);
 *		}
 *		if( intr_id == AL_UART_BUSY_DETECT_INTR_ID ) {
 *			al_dbg("Busy Detection Interrupt\n");
 *			// Clearing Busy Detection Interrupt
 *			al_uart_status_get(&uart_if);
 *		}
 *		if(( intr_id == AL_UART_RX_LINE_STAT_INTR_ID) &&
 *			( line_status &
 *				AL_UART_LINE_STATUS_PARITY_ERROR_MASK )) {
 *			al_dbg("Receiver Line Status Interrupt - Parity Error\n");
 *		}
 *
 *		return;
 *	}
 *
 *      int main()
 *      {
 *		struct al_uart_cfg uart_config;
 *              int interrupt_val = 0, ret = 0;
 *
 *		// @pseudo code
 *		//
 *              // Configurate and registrate UART interrupts with:
 *		//	1.generic interrupt controller,
 *		//	2.irq number (depend from CPU that run code),
 *		//	3.CPU that should handle the interrupt,
 *		//	4.interrupts triggering level,
 *		//	5.handler function that will be invoked upon receiving
 *		//	  interrupt (in our case pbs_uart_summary_handler);
 *
 *		// initialize the uart interface
 *		//	configure a uart in UART_mode with
 *		//		115200 baud rate,
 *		//		8bit data lenth,
 *		//		1 stop bit ,
 *		//		without parity bit
 *		//		HALF_FULL triggers for TX/RX FIFO`s
 *		uart_config.input_clk_freq = 375000000; // 375MHz
 *		uart_config.baud_rate = 115200;
 *		uart_config.mode = AL_UART_MODE_UART;
 *		uart_config.data_length = AL_UART_DATA_LENGTH_8_BIT;
 *		uart_config.stop_bit_num = AL_UART_1_STOP_BIT;
 *		uart_config.parity = AL_UART_PARITY_NONE;
 *		uart_config.rx_trigger = AL_UART_RX_TRIG_FIFO_HALF_FULL;
 *		uart_config.tx_trigger = AL_UART_TX_TRIG_FIFO_HALF_FULL;
 *
 *		al_uart_handle_init(&uart_if, (void *)AL_UART_BASE(cpu_id);
 *
 *		al_uart_init(&uart_if, &uart_config);
 *
 *		// Configuration and registration
 *		 * of UART controller interrupts
 *		irq_config(gic,AL_IRQ_UART(test_in->port),1 << cpu_id ,0,
 *				pbs_uart_summary_handler);
 *
 *		// Enabling(unmasking) interrupts
 *		al_uart_intr_unmask(&uart_if, AL_UART_INTR_RX_DATA_AVAIL);
 *
 *		while(1);
 *
 *		return ret;
 *      }
 * @endcode
 *
 * @file   al_hal_uart.h
 * @brief Header file for the UART HAL driver
 *
 */

/*
 * The following enum values should NOT be changed, as they are being used
 * internally and are important for proper behavior
 */
enum al_uart_mode {
	AL_UART_MODE_UART = 0,
	AL_UART_MODE_SIR  = 1
};

 /*
  * This enumeration should be used to select the number of data bits
  * per character that the peripheral transmits and receives.
  *
  * The following enum values should NOT be changed, as they are being used
  * internally and are important for proper behavior
  */
enum al_uart_serial_data_length {
	AL_UART_SERIAL_DATA_LENGTH_5_BIT = 0,
	AL_UART_SERIAL_DATA_LENGTH_6_BIT = 1,
	AL_UART_SERIAL_DATA_LENGTH_7_BIT = 2,
	AL_UART_SERIAL_DATA_LENGTH_8_BIT = 3
};

/*
 * The following enum values should NOT be changed, as they are being used
 * internally and are important for proper behavior
 */
enum al_uart_stop_bit {
	AL_UART_1_STOP_BIT = 0,
	/*
	 * In this mode the number of stop bits is
	 * determined according to the data length:
	 *	lenght = 5 bit => 1.5stop bits
	 *	length > 5bits => 2 stop bits
	 */
	AL_UART_1_TO_2_STOP_BITS = 1
};

/*
 * The following enum values should NOT be changed, as they are being used
 * internally and are important for proper behavior
 */
enum al_uart_parity {
	AL_UART_PARITY_ODD  = 0,
	AL_UART_PARITY_EVEN = 1,
	AL_UART_PARITY_NONE = 2
};

/*
 * This enumeration should be used for definition
 * the TX FIFO empty threshold level at which
 * the Transmit Holding Register Empty Interrupt generated.
 *
 *
 * The following enum values should NOT be changed, as they are being used
 * internally and are important for proper behavior
 */
enum al_uart_tx_fifo_trigger {
	AL_UART_TX_TRIG_FIFO_EMPTY		= 0,
	AL_UART_TX_TRIG_FIFO_TWO_CHAR_IN_FIFO   = 1,
	AL_UART_TX_TRIG_FIFO_QUARTER_FULL	= 2,
	AL_UART_TX_TRIG_FIFO_HALF_FULL		= 3
};

/*
 * This enumeration should be used for definition the trigger level
 * in the receiver FIFO at which the Received Data Available Interrupt
 * is generated.
 *
 * The following enum values should NOT be changed, as they are being used
 * internally and are important for proper behavior
 */
enum al_uart_rx_fifo_trigger {
	AL_UART_RX_TRIG_FIFO_ONE_CHAR_IN_FIFO		= 0,
	AL_UART_RX_TRIG_FIFO_QUARTER_FULL		= 1,
	AL_UART_RX_TRIG_FIFO_HALF_FULL			= 2,
	AL_UART_RX_TRIG_FIFO_2_CHAR_LESS_THAN_FULL	= 3
};

/* This enumeration should be used for masking/unmasking interrupts
 *
 * The following enum values should NOT be changed, as they are being used
 * internally
 */
enum al_uart_intr_mask {
	/* Received Data Available Interrupt */
	AL_UART_INTR_RX_DATA_AVAIL	= AL_BIT(0),
	/* Transmit Holding Register Empty Interrupt */
	AL_UART_INTR_THR_EMPTY		= AL_BIT(1),
	/* Receiver Line Status Interrupt */
	AL_UART_INTR_RX_LINE_STAT	= AL_BIT(2),
	/* Modem Status Interrupt */
	AL_UART_INTR_MODEM_STAT		= AL_BIT(3)
};

/*
 * This enumeration should be used for interrupts identification.
 *
 * In order to get interrupt ID , use al_uart_intr_id_get(..) function
 *
 * The following enum values should NOT be changed, as they are being used
 * internally
 */
enum al_uart_intr_id {
	/* Modem Status Interrupt */
	AL_UART_MODEM_STATUS_INTR_ID	= 0,
	/*
	 * Transmit Holding Register Empty Interrupt is asserted when
	 * TX FIFO at or below threshold (trigger level)
	 */
	AL_UART_THR_EMPTY_INTR_ID	= 2,
	/*
	 * Received Data Available Interrupt is asserted when
	 * RX FIFO trigger level reached
	 */
	AL_UART_RX_DATA_AVAIL_INTR_ID	= 4,
	/*
	 * Receiver Line Status Interrupt is asserted when
	 * Overrun/parity/framing errors or break interrupt occur
	 */
	AL_UART_RX_LINE_STAT_INTR_ID	= 6,
	/*
	 * Busy interrupt is asserted in case of write operation
	 * to Line Control register is performed, while UART is busy
	 * No possibility to disable this interrupt
	 */
	AL_UART_BUSY_DETECT_INTR_ID	= 7,
	/*
	 * Character Timeout Interrupt asserted when no characters in or out
	 * of the RX FIFO during the last 4 character times and there is
	 * at least 1 character in it during this time
	 */
	AL_UART_CHAR_TIMEOUT_INTR_ID	= 12
};

/*
 * The following enum values should NOT be changed, as they are being used
 * internally
 */
enum al_uart_reset {
	AL_UART_RESET_TX_FIFO	= 0x3,
	AL_UART_RESET_RX_FIFO	= 0x5,
	AL_UART_RESET_UART	= 0x6
};

/*
 * The following enum values should NOT be changed, as they are being used
 * internally
 */
enum al_uart_status {
	 /* UART busy */
	AL_UART_STATUS_BUSY	= AL_BIT(0),
	 /* Transmit FIFO not full */
	AL_UART_STATUS_TFNF	= AL_BIT(1),
	 /* Transmit FIFO empty */
	AL_UART_STATUS_TFE	= AL_BIT(2),
	 /* Receive FIFO not empty */
	AL_UART_STATUS_RFNE	= AL_BIT(3),
	 /* Receive FIFO full */
	AL_UART_STATUS_RFF	= AL_BIT(4),
};

/*
 * The following enum values should NOT be changed, as they are being used
 * internally
 */
enum al_uart_line_status {
	/* Data Ready */
	AL_UART_LINE_STATUS_DR		= AL_BIT(0),
	/* Overrun error */
	AL_UART_LINE_STATUS_OE		= AL_BIT(1),
	/* Parity Error */
	AL_UART_LINE_STATUS_PE		= AL_BIT(2),
	/* Framing Error */
	AL_UART_LINE_STATUS_FE		= AL_BIT(3),
	/* Break Interrupt */
	AL_UART_LINE_STATUS_BI		= AL_BIT(4),
	/* Transmit Holding Register Empty */
	AL_UART_LINE_STATUS_THRE	= AL_BIT(5),
	/* Transmitter Empty */
	AL_UART_LINE_STATUS_TEMT	= AL_BIT(6),
	/* Receiver FIFO Error */
	AL_UART_LINE_STATUS_RFE		= AL_BIT(7),
};

/*
 * The following enum values should NOT be changed, as they are being used
 * internally
 */
enum al_uart_modem_status {
	/* Delta Clear to Send */
	AL_UART_MODEM_STATUS_DCTS	= AL_BIT(0),
	/* Delta Data Set Ready */
	AL_UART_MODEM_STATUS_DDSR	= AL_BIT(1),
	/* Trailing Edge of Ring Indicator */
	AL_UART_MODEM_STATUS_TERI	= AL_BIT(2),
	/* Delta Data Carrier Detect */
	AL_UART_MODEM_STATUS_DDCD	= AL_BIT(3),
	/* Clear to Send */
	AL_UART_MODEM_STATUS_CTS	= AL_BIT(4),
	/* Data Set Ready */
	AL_UART_MODEM_STATUS_DSR	= AL_BIT(5),
	/* Ring Indicator */
	AL_UART_MODEM_STATUS_RI		= AL_BIT(6),
	/* Data Carrier Detect */
	AL_UART_MODEM_STATUS_DCD	= AL_BIT(7),

};

struct al_uart_if {
	void __iomem *regs;
	unsigned int timeout;
};

/*
 * The following struct defines the UART serial character structure,
 * it is being used to UART initialization
 *
 * UART Serial Character looks as following:
 *  ***************************************************************************
 *  * | start bit | serial data (5-8 bits) | parity | stop bit(s) (1,1.5,2) | *
 *  ***************************************************************************
 *
 * serial_data_length
 *	The length of serial data that is transfered/received
 *	in each single transaction as part of serial character
 * stop_bit_num
 *	The number of STOP bits that should be transfered/received after
 *	serial data as part of the serial character
 * parity
 *	The Uart parity type ,
 *	see enum al_uart_parity for further explanations
 *	Parity bit ,if defined,  will be added after the last serial data bit
 *	and before the stop bit(s) in the serial character structure
 */
struct al_uart_serial_char {
	enum al_uart_serial_data_length serial_data_length;
	enum al_uart_stop_bit stop_bit_num;
	enum al_uart_parity parity;
};


/*
 * The following struct is the main uart configuration struct, it is being used
 * for passing arguments to the uart initialization function.
 *
 * serial_char
 *	Serial character structure
 * input_clk_freq
 *	The UART controller reference clock frequency, measured in Hz.
 *	will be used together with baud_arte in order to set the UART device
 *	baud rate divisor (using the DLL and DLH registers)
 * baud_rate
 *	UART baud rate
 * mode
 *	Two possible values:
 *	1 - Serial Infrared Mode
 *	0 - UART Mode
 * rx_trigger
 *	Uart RX FIFO trigger,
 *	see enum al_uart_rx_fifo_trigger for further explanations
 * tx_trigger
 *	Uart TX FIFO trigger
 *	see enum al_uart_tx_fifo_trigger for further explanations
 * enable_auto_flow_cntr
 *	1 - enable AUTO FLOW CONTROL MODE
 *	0 - disable
 *
 * NOTE: Auto Flow control is enable ONLY when RX and TX FIFO`s
 *	are implemented and enabled
 *
 * enable_fifos
 *	1 - enable RX/TX FIFO's
 *	0 - disable
 *
 */
struct al_uart_cfg {
	struct al_uart_serial_char serial_char;
	unsigned int input_clk_freq;
	unsigned int baud_rate;
	enum al_uart_mode mode;
	enum al_uart_rx_fifo_trigger rx_trigger;
	enum al_uart_tx_fifo_trigger tx_trigger;
	unsigned int enable_auto_flow_cntr;
	unsigned int enable_fifos;
};


/**
 * Initialize the UART device handle
 *
 * this API initializes the UART handle without touching the hardware.
 *
 * @param	uart_if
 *		An allocated, non-initialized instance.
 * @param	uart_regs_base
 *		The base address of the UART configuration registers
 */
void al_uart_handle_init(
		struct al_uart_if *uart_if,
		void __iomem *uart_regs_base);

/**
 * Initialize the UART device
 *
 * @param	uart_if
 *		An allocated, non-initialized instance.
 * @param	config
 *		The UART configuration, see the struct al_uart_configuration
 *		for further explanation.
 */
void al_uart_init(
		struct al_uart_if *uart_if,
		struct al_uart_cfg *config);

/**
 * Read from UART device
 *
 * Note that this is a polling implementation, meaning that this is a blocking
 * function.
 *
 * @param	uart_if
 *		The UART instance we are working with.
 * @param	bytelen
 *		The amount of bytes to be read.
 * @param	buffer
 *		A pointer to the read data.
 * @param	timeout
 *		Time to wait before declaring a timeout error, measured in usec
 *		timeout >= 0  - declare timeout time
 *		timeout = -1  - declare infinite timeout
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_uart_read(
		struct al_uart_if *uart_if,
		unsigned int bytelen,
		uint8_t *buffer,
		int timeout);

/**
 * Write to UART device
 *
 * @param	uart_if
 *		The UART instance we are working with.I2C
 * @param	bytelen
 *		The amount of bytes to be written.
 * @param	buffer
 *		A pointer to the data to be written.
 * @param	timeout
 *		Time to wait before declaring a timeout error, measured in usec
 *		timeout >= 0  - declare timeout time
 *		timeout = -1  - declare infinite timeout
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred (errno value)
 */
int al_uart_write(
		struct al_uart_if *uart_if,
		unsigned int bytelen,
		uint8_t *buffer,
		int timeout);

/**
 * Performing software reset
 *
 * In order to create the input value is recommended to use
 * the UART reset enumeration
 *
 * Example:
 *   al_uart_reset(&uart_if_instance,
 *		(AL_UART_RESET_RX_FIFO & AL_UART_RESET_TX_FIFO));
 *
 * @param	uart_if
 *		The UART instance we are working with
 * @param	reset_val
 *		Value holding UART parts that should be reseted
 *		(its may be one from following : TX_FIFO,RX_FIFO,UART CONTROLER)
 */
void al_uart_soft_reset(
		struct al_uart_if *uart_if,
		unsigned int reset_val);

/**
 * Get the line status state
 *
 * This register reflect the cause of Line Status interrupt, if was detected
 *
 * @param	uart_if
 *		The uart instance we are working with
 *
 * @returns	Line status register value
 */
unsigned int al_uart_line_status_get(
		struct al_uart_if *uart_if);

/**
 * Get the modem status state
 *
 * This register reflect the cause of Modem Status interrupt, if was detected
 *
 * Use the UART modem_status enumeration i.o. to parse value into separate irqs.
 *
 * @param	uart_if
 *		The uart instance we are working with
 *
 * @returns	Modem status register value
 */
unsigned int al_uart_modem_status_get(
		struct al_uart_if *uart_if);

/**
 * Get the interrupts status state
 *
 * Use the al_uart_intr_id enumeration i.o. for interrupts identification
 *
 * @param	uart_if
 *		The uart instance we are working with
 *
 * @returns	Interrupt identity register value
 */
unsigned int al_uart_intr_status_get(
		struct al_uart_if *uart_if);

/**
 * Get the interrupt identification
 *
 * @param	uart_if
 *		The uart instance we are working with
 *
 * @returns	Interrupt ID ,
 *		see enum al_uart_intr_id for further explanation
 */
enum al_uart_intr_id al_uart_intr_id_get(
		struct al_uart_if *uart_if);

/**
 * Get the UART status
 *
 * Use the uart_status enumeration i.o. to parse value
 * into separate statuses
 *
 * @param	uart_if
 *		The uart instance we are working with
 *
 * @returns	Uart status register value
 */

unsigned int al_uart_status_get(
		struct al_uart_if *uart_if);

/**
 * Set UART TX FIFO trigger
 *
 * @param	uart_if
 *		The UART instance we are working with
 * @param	tx_trigger
 *		The tx FIFO trigger to set
 */
void al_uart_tx_trig_set(
		struct al_uart_if *uart_if,
		enum al_uart_tx_fifo_trigger tx_trigger);

/**
 * Set UART RX FIFO trigger
 *
 * @param	uart_if
 *		The UART instance we are working with
 * @param	rx_trigger
 *		The rx FIFO trigger to set
 */
void al_uart_rx_trig_set(
		struct al_uart_if *uart_if,
		enum al_uart_rx_fifo_trigger rx_trigger);

/**
 * Unmask (enable) an interrupts
 *
 * In order to create the input value is recommended to use
 * the UART interrupts enumeration
 *
 * Example:
 *   al_uart_intr_unmask(&uart_if_instance,
 *			(AL_UART_INTR_THR_EMPTY | AL_UART_INTR_MODEM_STAT));
 *
 * NOTE: If Transmit Holding Register Empty interrupt is going to be unmasked
 *	 => Programmable THRE Interrupt Mode Enable MUST be enabled too using
 *	 al_uart_ptime_enable(...) function
 *
 * @param	uart_if
 *		The UART instance we are working with
 * @param	unmask_val
 *		Value holding all the interrupts to unmask
 */
void al_uart_intr_unmask(
		struct al_uart_if *uart_if,
		unsigned int unmask_val);

/**
 * Mask (disable) an interrupts
 *
 * When an interrupt is masked (disabled), the corresponding interrupt
 * will not trigger its interrupt handling function even if it has been set
 *
 * In order to create the input value is recommended to use
 * the UART interrupts enumeration
 *
 * Example:
 *   al_uart_intr_mask(&uart_if_instance,
 *			(AL_UART_INTR_THR_EMPTY | AL_UART_INTR_MODEM_STAT));
 *
 * NOTE: If Transmit Holding Register Empty interrupt is going to be masked
 *	 => Programmable THRE Interrupt Mode Enable may be disabled too, using
 *	 al_uart_ptime_disable(...) function
 *
 * @param	uart_if
 *		The UART instance we are working with
 * @param	mask_val
 *		Value holding all the interrupts to mask
 */
void al_uart_intr_mask(
		struct al_uart_if *uart_if,
		unsigned int mask_val);

/**
 * Get Receive FIFO Level.
 *
 * Receive FIFO Level indicates the number of
 * data entries  are available in the receive FIFO.
 *
 * @param	uart_if
 *		The UART instance we are working with
 *
 * @return	number of data entries in RX FIFO
 */

unsigned int al_uart_rx_fifo_level_get(
		struct al_uart_if *uart_if);

/**
 * Get Transmit FIFO Level.
 *
 * Transmit FIFO Level indicates the number of free
 * data entries are available in the transmit FIFO.
 *
 * @param	uart_if
 *		The UART instance we are working with
 *
 * @return	number of free data entries in TX FIFO
 */

unsigned int al_uart_tx_fifo_level_get(
		struct al_uart_if *uart_if);

/**
 * Enable Programmable THRE Interrupt Mode
 *
 * Programmable THRE Interrupt Mode MUST be enabled in order to allow
 * generation of Transmit Holding Register Empty interrupt,when it
 * is unmasked.
 *
 * @param	uart_if
 *		The UART instance we are working with
 */
void al_uart_ptime_enable(
		struct al_uart_if *uart_if);

/**
 * Disable Programmable THRE Interrupt Mode
 *
 * @param	uart_if
 *		The UART instance we are working with
 */
void al_uart_ptime_disable(
		struct al_uart_if *uart_if);

/**
 * Check if input data available
 *
 * @param	uart_if
 *		The UART instance we are working with
 *
 * @return	0 - no data available neither in RBR no in receiver FIFO
 *		1 - receiver contains at least one character in
 *		the RBR or the receiver FIFO.
 */
int al_uart_is_input_available(
		struct al_uart_if *uart_if);


#endif /* _AL_HAL_UART_H_ */

/** @} */
