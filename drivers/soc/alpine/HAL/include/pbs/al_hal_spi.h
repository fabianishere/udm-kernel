/*
 * Copyright 2018, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */


/**
 * @defgroup group_spi SPI
 * @ingroup group_pbs
 *  @{
 *
 * @file   al_hal_spi.h
 * @brief Header file for the SPI HAL driver
 *
 */

#ifndef _AL_HAL_SPI_H_
#define _AL_HAL_SPI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "al_hal_common.h"


struct al_spi_interface {
	struct al_spi_regs __iomem *regs;
	uint32_t input_clk_freq;
	/* bus owner will be saved as value, not index (e.g. owner#2 = 0x4) */
	uint16_t curr_bus_owner;
};

/*- Following enum values should NOT be changed, as they are used internally -*/

/*
 * Serial Clock Phase, used to select the relationship of the serial clock with
 * the slave select signal.
 * When SCPH = 0, data is captured on the first edge of the serial clock.
 * When SCPH = 1, the serial clock starts toggling one cycle after the slave
 * select line is activated, and data is captured on the second edge of the
 * serial clock.
 */
enum al_spi_phase_t {
	AL_SPI_PHASE_SLAVE_SELECT	= 0,
	AL_SPI_PHASE_CLOCK		= 1
};

/*
 * Serial Clock Polarity, used to select the polarity of the inactive serial
 * clock, which is held inactive when the DW_apb_ssi master is not actively
 * transferring data on the serial bus.
 */
enum al_spi_polarity_t {
	AL_SPI_POLARITY_INACTIVE_LOW	= 0,
	AL_SPI_POLARITY_INACTIVE_HIGH	= 1
};

enum al_spi_tmod_t {
	AL_SPI_TMOD_TRANS_RECV	= 0,
	AL_SPI_TMOD_TRANS	= 1,
	AL_SPI_TMOD_RECV	= 2,
	AL_SPI_TMOD_EEPROM	= 3
};

enum al_spi_intr_t {
	AL_SPI_INTR_TXEIM = (1 << 0),	/* transmit FIFO empty */
	AL_SPI_INTR_TXOIM = (1 << 1),	/* transmit FIFO overflow */
	AL_SPI_INTR_RXUIM = (1 << 2),	/* receive FIFO underflow */
	AL_SPI_INTR_RXOIM = (1 << 3),	/* receive FIFO overflow */
	AL_SPI_INTR_RXFIM = (1 << 4),	/* receive FIFO full */
	AL_SPI_INTR_MSTIM = (1 << 5),	/* multi-master contention */
};

/**
 * Initialize the SPI controller
 *
 * @param	spi_if
 *		The SPI instance we are working with
 * @param	spi_regs_base
 *		The base address of the SPI configuration registers
 * @param	input_clk_freq
 *		The SPI controller reference clock frequency, measured in Hz.
 *		will be used in order to set the SPI device clock rate
 *		(using the baudr register)
 */
void al_spi_init(
		struct al_spi_interface	*spi_if,
		void __iomem		*spi_regs_base,
		uint32_t		input_clk_freq);


/**
 * Claiming the bus for a specific slave
 *
 * This API function will config the polarity and phase bits of the SPI device
 * according to the claiming slave's configuration.
 *
 * @param	spi_if
 *		The SPI instance we are working with
 * @param	freq
 *		The frequency of the current bus claimer, measured in Hz
 * @param	phase
 *		Determines whether the serial transfer begins with the falling
 *		edge of the slave select signal, or the first edge of the serial
 *		clock
 * @param	polarity
 *		Determines whether the inactive state of the serial clock
 *		is high or low
 * @param	cs
 *		The bus claimer's chip select index
 *
 * @return	int
 *		AL_SUCCESS if all is okay
 *		AL_ERROR if timed out
 */
int al_spi_claim_bus(
		struct al_spi_interface	*spi_if,
		uint32_t		freq,
		enum al_spi_phase_t	phase,
		enum al_spi_polarity_t	polarity,
		uint32_t		cs);

/**
 * Disable the SPI device
 *
 * @param	spi_if
 *		The SPI instance we are working with
 * @param	cs_index
 *		The CS index who is supposed to be the current bus claimer
 */
void al_spi_release_bus(
		struct al_spi_interface	*spi_if,
		uint32_t		cs_index);

/**
 * Get current baud rate
 *
 * @param	spi_if
 *		The SPI interface we are working with
 *
 * @returns	The current baud rate
 */
unsigned int al_spi_get_baudr(
	struct al_spi_interface	*spi_if);

/**
 * Perform an SPI read transaction
 *
 * Note that the transaction is separated to two phases:
 *	1) Command phase: configuring the parameters for the spi transaction,
 *	then writing the command bytes to the transmit fifo
 *	2) Read data phase: reading the actual data bytes from the receive fifo
 * Note that once you begin writing the command bytes to the fifo, the
 * SSI Enable Register (SSIENR) should NOT be disabled, nor should the Slave
 * Enable Register (SER) - deasserting either one of these registers will
 * terminate the transaction.
 *
 * @param	spi_if
 *		The SPI instance we are working with
 * @param	cmd
 *		The buffer that holds the command data:
 *		command = {opcode,addrMSB,addr,addrLSB}
 * @param	cmd_len
 *		The length of the command
 * @param	din
 *		The buffer that accepts the incoming data
 * @param	bytelen
 *		The length of the read transaction, measured in bytes
 * @param	cs
 *		The bus claimer's chip select index
 * @param	timeout
 *		how much time should the HAL wait before declaring a timeout,
 *		measured in usec
 *
 * @return	int
 *		AL_SUCCESS if all is okay
 *		AL_ERROR if timed out
 */
int al_spi_read(
		struct al_spi_interface	*spi_if,
		uint8_t const		*cmd,
		uint8_t			cmd_len,
		uint8_t			*din,
		uint32_t		bytelen,
		uint32_t		cs,
		uint32_t		timeout);


/**
 * Perform an SPI write transaction
 *
 * Note that the transaction is separated to two phases:
 *	1) Command phase: configuring the parameters for the spi transaction,
 *	then writing the command bytes to the transmit fifo
 *	2) Write data phase: writing the actual data bytes to the transmit fifo
 * Note that once you begin writing the command bytes to the fifo, the
 * SSI Enable Register (SSIENR) should NOT be disabled, nor should the Slave
 * Enable Register (SER) - deasserting either one of these registers will
 * terminate the transaction.
 *
 * @param	spi_if
 *		The SPI instance we are working with
 * @param	cmd
 *		The buffer that holds the command data:
 *		command = {opcode,addrMSB,addr,addrLSB}
 * @param	cmd_len
 *		The length of the command
 * @param	dout
 *		The buffer that holds the outgoing data
 * @param	bytelen
 *		The length of the write data, measured in bytes
 * @param	cs
 *		The slave we are writing into, use al_spi_regs definitions
 * @param	timeout
 *		how much time should the HAL wait before declaring a timeout,
 *		measured in usec
 *
 * @return	int
 *		AL_SUCCESS if all is okay
 *		AL_ERROR if timed out
 */
int al_spi_write(
		struct al_spi_interface	*spi_if,
		uint8_t const		*cmd,
		uint8_t			cmd_len,
		uint8_t const		*dout,
		uint32_t		bytelen,
		uint32_t		cs,
		uint32_t		timeout);

/**
 * Get the RAW interrupt state
 *
 * Raw interrupt state is the value of the Interrupt State regardless of the
 * interrupt mask.
 *
 * Use the SPI interrupts enumeration i.o. to parse value into separate irqs.
 *
 * @param	spi_if
 *		The SPI instance we are working with
 *
 * @returns	RAW interrupt register value
 */
unsigned int al_spi_raw_intr_get(
		struct al_spi_interface	*spi_if);

/**
 * Get the MASKED interrupt state
 *
 * Masked interrupt state is the value of the Interrupt State considering the
 * interrupt mask: if the interrupt's corresponding mask bit is set, it will
 * be masked.
 *
 * Use the SPI interrupts enumeration i.o. to parse value into separate irqs.
 *
 * @param	spi_if
 *		The spi instance we are working with
 *
 * @returns	Interrupt register value
 */
unsigned int al_spi_masked_intr_get(
		struct al_spi_interface	*spi_if);

/**
 * Enables an interrupt
 *
 * Using the SPI interrupts enumeration in order to create the input value is
 * recommended.
 *
 * Example:
 *   al_spi_intr_enable(&spi_if_instance,
 *		(AL_SPI_INTR_TXOIM | AL_SPI_INTR_RXOIM));
 *
 * @param	spi_if
 *		The SPI instance we are working with
 * @param	enable_val
 *		Value holding all the interrupts to enable
 */
void al_spi_intr_enable(
		struct al_spi_interface	*spi_if,
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
 *   al_spi_intr_disable(&spi_if_instance,
 *		(AL_SPI_INTR_TXOIM | AL_SPI_INTR_RXOIM));
 *
 * @param	spi_if
 *		The SPI instance we are working with
 * @param	disable_val
 *		Value holding all the interrupts to disable
 */
void al_spi_intr_disable(
		struct al_spi_interface	*spi_if,
		unsigned int		disable_val);

/**
 * Clear SPI interrupts
 *
 * Using the SPI interrupts enumeration in order to create the input value is
 * recommended.
 *
 * Example:
 *   al_spi_intr_clear(&spi_if_instance,
 *		(AL_SPI_INTR_TXOIM | AL_SPI_INTR_RXOIM));
 *
 * @param	spi_if
 *		The SPI instance we are working with
 * @param	clear_val
 *		Value holding all the interrupts to clear
 */
void al_spi_intr_clear(
		struct al_spi_interface	*spi_if,
		unsigned int		clear_val);

/**
 * Set SPI transfer mode
 *
 * @param	spi_if
 *		The SPI instance we are working with
 * @param	tmod
 *		The transfer mode to set
 */
void al_spi_tmod_set(
		struct al_spi_interface	*spi_if,
		enum al_spi_tmod_t	tmod);

/**
 * Set spi chip select override
 *
 * Set HW CS control Override, so that SER will control the CS mechanism
 * Using this will prevent the deassertion of CS upon empty TX FIFO
 *
 * @param	spi_regs_base
 *		The base address of the SPI configuration registers
 *		(passing regs base address enables to invoke this API prior to
 *		initialization)
 */
void al_spi_cs_override_set(
		void __iomem		*spi_regs_base);

/**
 * Clear spi chip select override
 *
 * Clear HW CS control Override
 *
 * @param	spi_regs_base
 *		The base address of the SPI configuration registers
 *		(passing regs base address enables to invoke this API prior to
 *		initialization)
 */
void al_spi_cs_override_clear(
		void __iomem		*spi_regs_base);

/**
 * Set spi chip select
 *
 * Set SER register to replace between different CS
 * This is useful for direct read access via glue logic, not via al_spi_read
 *
 * @param	spi_regs_base
 *		The base address of the SPI configuration registers
 *		(passing regs base address enables to invoke this API prior to
 *		initialization)
 * @param	cs
 *		The desired chip select index
 */
void al_spi_cs_set(
		void __iomem		*spi_regs_base,
		unsigned int		cs);

/**
 * Configure SPI slave parameters
 *
 * This API function will config the polarity and phase bits of the SPI device
 * according to the claiming slave's configuration.
 *
 * @param	spi_if
 *		The SPI instance we are working with
 * @param	phase
 *		Determines whether the serial transfer begins with the falling
 *		edge of the slave select signal, or the first edge of the serial
 *		clock
 * @param	polarity
 *		Determines whether the inactive state of the serial clock
 *		is high or low
 * @param	enable_back
 *		When TRUE - complete config action by enabling the device
		When FALSE - keep device disabled after config
 * @return	int
 *		Zero upon success
 *		Non-zero upon failure
 */
int al_spi_slave_cfg(
		struct al_spi_interface	*spi_if,
		enum al_spi_phase_t	phase,
		enum al_spi_polarity_t	polarity,
		al_bool			enable_back);

/**
 * Enable the SPI master glue logic
 *
 * Required after the SPI master was accessed through the SPI HAL
 *
 * @param	spi_regs_base
 *		The base address of the SPI configuration registers
 * @param	cs
 *		The desired chip select index
 */
void al_spi_master_glue_enable(
	void __iomem		*spi_regs_base,
	unsigned int		cs);

/**
 * Configure SPI master glue logic speed
 *
 * @param	spi_regs_base
 *		The base address of the SPI configuration registers
 * @param	pbs_regs_base
 *		The base address of the PBS registers
 * @param	qpi
 *		AL_TRUE - QPI mode, AL_FALSE - SPI mode
 * @param	ref_clk_freq_hz
 *		Reference clock frequency [Hz]
 * @param	speed_mhz
 *		Required baud rate [MHz]
 */
void al_spi_master_glue_speed_set(
	void __iomem		*spi_regs_base,
	void __iomem		*pbs_regs_base,
	al_bool			qpi,
	unsigned int		ref_clk_freq_hz,
	unsigned int		speed_mhz);

/**
 * Enable SPI master glue logic QPI mode, without changing baud rate
 *
 * @param	spi_regs_base
 *		The base address of the SPI configuration registers
 * @param	pbs_regs_base
 *		The base address of the PBS registers
 * @param	en
 *		AL_TRUE - enable, AL_FALSE - disable
 */
void al_spi_master_glue_qpi_en(
	void __iomem		*spi_regs_base,
	void __iomem		*pbs_regs_base,
	al_bool			en);

/**
 * Configure QPI IOs, unused in SPI mode to No Drive or Drive a specific value
 *
 * @param	pbs_regs_base
 *		The base address of the PBS registers
 * @param	mask
 *		bit 3 -  1 means QPI IO3 related values are allowed to modify
 *		bit 2 -  1 means QPI IO2 related values are allowed to modify
 * @param	drive_mask
 *		bit 3 - represents QPI IO3, 1 means drive, 0 - not drive
 *		bit 2 - represents QPI IO2, 1 means drive, 0 - not drive
 * @param	val_mask
 *		bit 3 - reflected in QPI IO3
 *		bit 2 - reflected in QPI IO2
 */
void al_spi_master_glue_qpi_config_pins(
	void __iomem		*pbs_regs_base,
	unsigned int		mask,
	unsigned int		drive_mask,
	unsigned int		val_mask);

#ifdef __cplusplus
}
#endif

#endif

/** @} end of SPI group */

