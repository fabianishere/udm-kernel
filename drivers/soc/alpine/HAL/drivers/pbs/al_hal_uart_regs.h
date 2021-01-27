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
 * @defgroup group_uart UART
 * @ingroup group_pbs
 *  @{
 *
 * @file   al_hal_uart_regs.h
 * @brief Header file for the UART registers definition
 *
 */

#ifndef _AL_HAL_UART_REGS_H_
#define _AL_HAL_UART_REGS_H_

#include "al_hal_plat_types.h"

/*
 * Unit Registers
 */

struct al_uart_regs {
	uint32_t	rbr_thr_dll;	/*
					 * RBR - read only
					 * (can be accessed if LCR[7] bit = 0)
					 * THR - write only
					 * (can be accessed if LCR[7] bit = 0)
					 * DLL - R/W
					 * (can be accessed if LCR[7] bit = 1)
					 * all registers have same offset 0x00
					 */
	uint32_t	dlh_ier;	/*
					 * DLH - R/W
					 * (can be accessed if LCR[7] bit = 1)
					 * IER - R/W
					 * (can be accessed if LCR[7] bit = 0)
					 * all registers have same offset 0x04
					 */
	uint32_t	iir_fcr;	/*
					 * IIR - read only
					 * FCR - write only
					 * all registers have same offset 0x08
					 */
	uint32_t	lcr;		/*0x0C*/
	uint32_t	mcr;		/*0x10*/
	uint32_t	lsr;		/*0x14*/
	uint32_t	msr;		/*0x18*/
	uint32_t	scr;		/*0x1C*/
	uint32_t	lpdll;		/*
					 * can be accessed if LCR[7] bit = 1
					 * offset 0x20
					 */
	uint32_t	lpdlh;		/*
					 * can be accessed if LCR[7] bit = 1
					 * offset 0x24
					 */
	uint32_t	rsrvd[2];	/*0x28 -0x2C*/
	uint32_t	srbr_sthr[16];	/*
					 * SRBR - read only
					 * (can be accessed if LCR[7] bit = 0)
					 * STHR - write only
					 * (can be accessed if LCR[7] bit = 0)
					 * all registers have same
					 * offsets 0x30 - 0x6C
					 */
	uint32_t	far;		/*0x70*/
	uint32_t	tfr;		/*0x74*/
	uint32_t	rfw;		/*0x78*/
	uint32_t	usr;		/*0x7C*/
	uint32_t	tfl;		/*0x80*/
	uint32_t	rfl;		/*0x84*/
	uint32_t	srr;		/*0x88*/
	uint32_t	srts;		/*0x8C*/
	uint32_t	sbcr;		/*0x90*/
	uint32_t	sdmam;		/*0x94*/
	uint32_t	sfe;		/*0x98*/
	uint32_t	srt;		/*0x9C*/
	uint32_t	stet;		/*0xA0*/
	uint32_t	htx;		/*0xA4*/
	uint32_t	dmasa;		/*0xA8*/
	uint32_t	rsrvd1[18];	/*0xAC - 0xF0*/
	uint32_t	cpr;		/*0xF4*/
	uint32_t	ucv;		/*0xF8*/
	uint32_t	ctr;		/*0xFC*/
};

/*
 * Registers Fields
 */
/**** Interrupt Enable Register ****/
#define AL_UART_IER_PTIME_SHIFT					0x7
#define AL_UART_IER_PTIME_MASK					(0x1 << \
		AL_UART_IER_PTIME_SHIFT)
#define AL_UART_IER_PTIME_DISABLE				(0x0 << \
		AL_UART_IER_PTIME_SHIFT)
#define AL_UART_IER_PTIME_ENABLE				(0x1 << \
		AL_UART_IER_PTIME_SHIFT)
/*Modem Status Interrupt*/
#define AL_UART_IER_EDSSI_SHIFT					0x3
#define AL_UART_IER_EDSSI_MASK					(0x1 << \
		AL_UART_IER_EDSSI_SHIFT)
#define AL_UART_IER_EDSSI_DISABLE				(0x0 << \
		AL_UART_IER_EDSSI_SHIFT)
#define AL_UART_IER_EDSSI_ENABLE				(0x1 << \
		AL_UART_IER_EDSSI_SHIFT)
/*Receiver Line Status Interrupt*/
#define AL_UART_IER_ELSI_SHIFT					0x2
#define AL_UART_IER_ELSI_MASK					(0x1 << \
		AL_UART_IER_ELSI_SHIFT)
#define AL_UART_IER_ELSI_DISABLE				(0x0 << \
		AL_UART_IER_ELSI_SHIFT)
#define AL_UART_IER_ELSI_ENABLE					(0x1 << \
		AL_UART_IER_ELSI_SHIFT)
/*Transmit Holding Register Empty Interrupt*/
#define AL_UART_IER_ETBEI_SHIFT					0x1
#define AL_UART_IER_ETBEI_MASK					(0x1 << \
		AL_UART_IER_ETBEI_SHIFT)
#define AL_UART_IER_ETBEI_DISABLE				(0x0 << \
		AL_UART_IER_ETBEI_SHIFT)
#define AL_UART_IER_ETBEI_ENABLE				(0x1 << \
		AL_UART_IER_ETBEI_SHIFT)
/* Received Data Available Interrupt */
#define AL_UART_IER_ERBFI_SHIFT					0x0
#define AL_UART_IER_ERBFI_MASK					(0x1 << \
		AL_UART_IER_ERBFI_SHIFT)
#define AL_UART_IER_ERBFI_DISABLE				(0x0 << \
		AL_UART_IER_ERBFI_SHIFT)
#define AL_UART_IER_ERBFI_ENABLE				(0x1 << \
		AL_UART_IER_ERBFI_SHIFT)


/**** Interrupt Identity Register ****/
/* READ ONLY REGISTER */
/* FIFOs Enabled status */
#define AL_UART_IIR_FIFOSE_SHIFT				0x6
#define AL_UART_IIR_FIFOSE_MASK					(0x3 << \
		AL_UART_IIR_FIFOSE_SHIFT)
/* Interrupt ID */
#define AL_UART_IIR_IID_SHIFT					0x0
#define AL_UART_IIR_ID_MASK					(0xF << \
		AL_UART_IIR_IID_SHIFT)
#define AL_UART_IIR_MODEM_STATUS_INTR				(0x0 << \
		AL_UART_IIR_IID_SHIFT)
#define AL_UART_IIR_THR_EMPTY_INTR				(0x2 << \
		AL_UART_IIR_IID_SHIFT)
#define AL_UART_IIR_RX_DATA_AVAIL_INTR				(0x4 << \
		AL_UART_IIR_IID_SHIFT)
#define AL_UART_IIR_RX_LINE_STAT_INTR				(0x2 << \
		AL_UART_IIR_IID_SHIFT)
#define AL_UART_IIR_BUSY_DETECT_INTR				(0x7 << \
		AL_UART_IIR_IID_SHIFT)
#define AL_UART_IIR_CHAR_TIMEOUT_INTR				(0xC << \
		AL_UART_IIR_IID_SHIFT)


/**** FIFO Control Register ****/
/* WRITE ONLY REGISTER */
/* RX FIFO Trigger */
#define AL_UART_FCR_RCVR_SHIFT					0x6
#define AL_UART_FCR_RCVR_MASK					(0x3 << \
		AL_UART_FCR_RCVR_SHIFT)
#define AL_UART_FCR_RCVR_ONE_CHAR_IN_FIFO			(0x0 << \
		AL_UART_FCR_RCVR_SHIFT)
#define AL_UART_FCR_RCVR_FIFO_QUARTER_FULL			(0x1 << \
		AL_UART_FCR_RCVR_SHIFT)
#define AL_UART_FCR_RCVR_FIFO_HALF_FULL				(0x2 << \
		AL_UART_FCR_RCVR_SHIFT)
#define AL_UART_FCR_RCVR_FIFO_2_LESS_THAN_FULL			(0x3 << \
		AL_UART_FCR_RCVR_SHIFT)
/* TX Empty Trigger */
#define AL_UART_FCR_TET_SHIFT					0x4
#define AL_UART_FCR_TET_MASK					(0x3 << \
		AL_UART_FCR_TET_SHIFT)

#define AL_UART_FCR_TET_FIFO_EMPTY				(0x0 << \
		AL_UART_FCR_TET_SHIFT)
#define AL_UART_FCR_TET_TWO_CHAR_IN_FIFO			(0x1 << \
		AL_UART_FCR_TET_SHIFT)
#define AL_UART_FCR_TET_FIFO_QUARTER_FULL			(0x2 << \
		AL_UART_FCR_TET_SHIFT)
#define AL_UART_FCR_TET_FIFO_HALF_FULL				(0x3 << \
		AL_UART_FCR_TET_SHIFT)
/* DMA Mode */
#define AL_UART_FCR_DMAM_SHIFT					0x3
#define AL_UART_FCR_DMAM_MASK					(0x3 << \
		AL_UART_FCR_DMAM_SHIFT)
#define AL_UART_FCR_DMAM_MODE_0					(0x0 << \
		AL_UART_FCR_DMAM_SHIFT)
#define AL_UART_FCR_DMAM_MODE_1					(0x1 << \
		AL_UART_FCR_DMAM_SHIFT)
/* XMIT FIFO Reset */
#define AL_UART_FCR_XFIFOR_SHIFT				0x2
#define AL_UART_FCR_XFIFOR_MASK					(0x1 << \
		AL_UART_FCR_XFIFOR_SHIFT)
#define AL_UART_FCR_XFIFOR_RESET				(0x1 << \
		AL_UART_FCR_XFIFOR_SHIFT)
/* RCVR FIFO Reset */
#define AL_UART_FCR_RFIFOR_SHIFT				0x1
#define AL_UART_FCR_RFIFOR_MASK					(0x1 << \
		AL_UART_FCR_RFIFOR_SHIFT)
#define AL_UART_FCR_RFIFOR_RESET				(0x1 << \
		AL_UART_FCR_RFIFOR_SHIFT)
/* FIFO Enable */
#define AL_UART_FCR_FIFO_ENABLE_SHIFT				0x0
#define AL_UART_FCR_FIFO_ENABLE_MASK				(0x1 << \
		AL_UART_FCR_FIFO_ENABLE_SHIFT)
#define AL_UART_FCR_FIFO_DISABLE				(0x0 << \
		AL_UART_FCR_FIFO_ENABLE_SHIFT)
#define AL_UART_FCR_FIFO_ENABLE					(0x1 << \
		AL_UART_FCR_FIFO_ENABLE_SHIFT)


/**** Line Control Register ****/
/* Divisor Latch Access Bit */
#define AL_UART_LCR_DLAB_SHIFT					0x7
#define AL_UART_LCR_DLAB_MASK					(0x1 << \
		AL_UART_LCR_DLAB_SHIFT)
#define AL_UART_LCR_DLAB_CLEAR					(0x0 << \
		AL_UART_LCR_DLAB_SHIFT)
#define AL_UART_LCR_DLAB_SET					(0x1 << \
		AL_UART_LCR_DLAB_SHIFT)
/* Break control bit */
#define AL_UART_LCR_BREAK_SHIFT					0x6
#define AL_UART_LCR_BREAK_MASK					(0x1 << \
		AL_UART_LCR_BREAK_SHIFT)
#define AL_UART_LCR_BREAK_DISABLE				(0x0 << \
		AL_UART_LCR_BREAK_SHIFT)
#define AL_UART_LCR_BREAK_ENABLE				(0x1 << \
		AL_UART_LCR_BREAK_SHIFT)
/* Stick Parity bit */
#define AL_UART_LCR_STICK_PARITY_SHIFT				0x5
#define AL_UART_LCR_STICK_PARITY_MASK				(0x1 << \
		AL_UART_LCR_STICK_PARITY_SHIFT)
#define AL_UART_LCR_STICK_PARITY_DISABLE			(0x0 << \
		AL_UART_LCR_STICK_PARITY_SHIFT)
#define AL_UART_LCR_STICK_PARITY_ENABLE				(0x1 << \
		AL_UART_LCR_STICK_PARITY_SHIFT)
/* Even Parity Select */
#define AL_UART_LCR_EVEN_PARITY_SELECT_SHIFT			0x4
#define AL_UART_LCR_EVEN_PARITY_SELECT_MASK			(0x1 << \
		AL_UART_LCR_EVEN_PARITY_SELECT_SHIFT)
/* Parity Enable */
#define AL_UART_LCR_PARITY_SHIFT				0x3
#define AL_UART_LCR_PARITY_MASK					(0x1 << \
		AL_UART_LCR_PARITY_SHIFT)
#define AL_UART_LCR_PARITY_DISABLE				(0x0 << \
		AL_UART_LCR_PARITY_SHIFT)
#define AL_UART_LCR_PARITY_ENABLE				(0x1 << \
		AL_UART_LCR_PARITY_SHIFT)
/* Number of stop bits */
#define AL_UART_LCR_STOP_BITS_NUMBER_SHIFT			0x2
#define AL_UART_LCR_STOP_BITS_NUMBER_MASK			(0x1 << \
		AL_UART_LCR_STOP_BITS_NUMBER_SHIFT)
/* Data Length Select */
#define AL_UART_LCR_DATA_LENGTH_SHIFT				0x0
#define AL_UART_LCR_DATA_LENGTH_MASK				(0x3 << \
		AL_UART_LCR_DATA_LENGTH_SHIFT)


/**** Modem Control Register ****/
/* Serial InfraRed mode enable*/
#define AL_UART_MCR_SIRE_MODE_SHIFT				0x6
#define AL_UART_MCR_SIRE_MODE_MASK				(0x1 << \
		AL_UART_MCR_SIRE_MODE_SHIFT)
#define AL_UART_MCR_SIRE_DISABLE				(0x0 << \
		AL_UART_MCR_SIRE_MODE_SHIFT)
#define AL_UART_MCR_SIRE_ENABLE					(0x1 << \
		AL_UART_MCR_SIRE_MODE_SHIFT)
/* Auto Flow Control Enable */
#define AL_UART_MCR_AUTO_FLOW_CNTRL_SHIFT			0x5
#define AL_UART_MCR_AUTO_FLOW_CNTRL_MASK			(0x1 << \
		AL_UART_MCR_AUTO_FLOW_CNTRL_SHIFT)
#define AL_UART_MCR_AUTO_FLOW_CNTRL_DISABLE			(0x0 << \
		AL_UART_MCR_AUTO_FLOW_CNTRL_SHIFT)
#define AL_UART_MCR_AUTO_FLOW_CNTRL_ENABLE			(0x1 << \
		AL_UART_MCR_AUTO_FLOW_CNTRL_SHIFT)
/* Loopback Mode*/
#define AL_UART_MCR_LOOPBACK_SHIFT				0x4
#define AL_UART_MCR_LOOPBACK_MASK				(0x1 << \
		AL_UART_MCR_LOOPBACK_SHIFT)
#define AL_UART_MCR_LOOPBACK_DISABLE				(0x0 << \
		AL_UART_MCR_LOOPBACK_SHIFT)
#define AL_UART_MCR_LOOPBACK_ENABLE				(0x1 << \
		AL_UART_MCR_LOOPBACK_SHIFT)
#define AL_UART_MCR_OUT2_SHIFT					0x3
	#define AL_UART_MCR_OUT2_MASK				(0x1 << \
		AL_UART_MCR_OUT2_SHIFT)
/* set out2 to logic 1*/
#define AL_UART_MCR_OUT2_DEASSERT				(0x0 << \
		AL_UART_MCR_OUT2_SHIFT)
/* set out2 to logic 0*/
#define AL_UART_MCR_OUT2_ASSERT					(0x1 << \
		AL_UART_MCR_OUT2_SHIFT)
#define AL_UART_MCR_OUT1_SHIFT					0x2
#define AL_UART_MCR_OUT1_MASK					(0x1 << \
		AL_UART_MCR_OUT1_SHIFT)
/* set out1_n to logic 1*/
#define AL_UART_MCR_OUT1_DEASSERT				(0x0 << \
		AL_UART_MCR_OUT1_SHIFT)
/* set out1_n to logic 0*/
#define AL_UART_MCR_OUT1_ASSERT					(0x1 << \
		AL_UART_MCR_OUT1_SHIFT)
#define AL_UART_MCR_REQUEST_TO_SEND_SHIFT			0x1
#define AL_UART_MCR_REQUEST_TO_SEND_MASK			(0x1 << \
		AL_UART_MCR_REQUEST_TO_SEND_SHIFT)
#define AL_UART_MCR_REQUEST_TO_SEND_SET				(0x1 << \
		AL_UART_MCR_REQUEST_TO_SEND_SHIFT)
#define AL_UART_MCR_REQUEST_TO_SEND_CLEAR			(0x0 << \
		AL_UART_MCR_REQUEST_TO_SEND_SHIFT)
/* Data Terminal Ready */
#define AL_UART_MCR_DATA_TERMINAL_READY_SHIFT			0
#define AL_UART_MCR_DATA_TERMINAL_READY_MASK			(0x3 << \
		AL_UART_MCR_DATA_TERMINAL_READY_SHIFT)
#define AL_UART_MCR_DATA_TERMINAL_READY_DEASSERT		(0x0 << \
		AL_UART_MCR_DATA_TERMINAL_READY_SHIFT)
#define AL_UART_MCR_DATA_TERMINAL_READY_ASSERT			(0x1 << \
		AL_UART_MCR_DATA_TERMINAL_READY_SHIFT)


/**** Line Status Register ****/
#define AL_UART_LINE_STATUS_RX_FIFO_ERROR_SHIFT			0x7
#define AL_UART_LINE_STATUS_RX_FIFO_ERROR_MASK			(0x1 << \
		AL_UART_LINE_STATUS_RX_FIFO_ERROR_SHIFT)
#define AL_UART_LINE_STATUS_TRANSMIT_EMPTY_SHIFT		0x6
#define AL_UART_LINE_STATUS_TRANSMIT_EMPTY_MASK			(0x1 << \
		AL_UART_LINE_STATUS_TRANSMIT_EMPTY_SHIFT)
/*Transmit Holding Register Empty */
#define AL_UART_LINE_STATUS_THRE_SHIFT				0x5
#define AL_UART_LINE_STATUS_THRE_MASK				(0x1 << \
		AL_UART_LINE_STATUS_THRE_SHIFT)
#define AL_UART_LINE_STATUS_BREAK_INTR_SHIFT			0x4
#define AL_UART_LINE_STATUS_BREAK_INTR_MASK			(0x1 << \
		AL_UART_LINE_STATUS_BREAK_INTR_SHIFT)
#define AL_UART_LINE_STATUS_FRAMING_ERROR_SHIFT			0x3
#define AL_UART_LINE_STATUS_FRAMING_ERROR_MASK			(0x1 << \
		AL_UART_LINE_STATUS_FRAMING_ERROR_SHIFT)
#define AL_UART_LINE_STATUS_PARITY_ERROR_SHIFT			0x2
#define AL_UART_LINE_STATUS_PARITY_ERROR_MASK			(0x1 << \
		AL_UART_LINE_STATUS_PARITY_ERROR_SHIFT)
#define AL_UART_LINE_STATUS_OVERRUN_ERROR_SHIFT			0x1
#define AL_UART_LINE_STATUS_OVERRUN_ERROR_MASK			(0x1 << \
		AL_UART_LINE_STATUS_OVERRUN_ERROR_SHIFT)
#define AL_UART_LINE_STATUS_DATA_READY_SHIFT			0x0
#define AL_UART_LINE_STATUS_DATA_READY_MASK			(0x1 << \
		AL_UART_LINE_STATUS_DATA_READY_SHIFT)



/**** Modem Status Register ****/
/* Data Carrier Detect */
#define AL_UART_MODEM_STATUS_DCD_SHIFT				0x7
#define AL_UART_MODEM_STATUS_DCD_MASK				(0x1 << \
		AL_UART_MODEM_STATUS_DCD_SHIFT)
/* Ring Indicator */
#define AL_UART_MODEM_STATUS_RI_SHIFT				0x6
#define AL_UART_MODEM_STATUS_RI_MASK				(0x1 << \
		AL_UART_MODEM_STATUS_RI_SHIFT)
/* Data Set Ready */
#define AL_UART_MODEM_STATUS_DSR_SHIFT				0x5
#define AL_UART_MODEM_STATUS_DSR_MASK				(0x1 << \
		AL_UART_MODEM_STATUS_DSR_SHIFT)
/* Clear to Send */
#define AL_UART_MODEM_STATUS_CTS_SHIFT				0x4
#define AL_UART_MODEM_STATUS_CTS_MASK				(0x1 << \
		AL_UART_MODEM_STATUS_CTS_SHIFT)
/* Delta Data Carrier Detect */
#define AL_UART_MODEM_STATUS_DDCD_SHIFT				0x3
#define AL_UART_MODEM_STATUS_DDCD_MASK				(0x1 << \
		AL_UART_MODEM_STATUS_DDCD_SHIFT)
/* Trailing Edge of Ring Indicator */
#define AL_UART_MODEM_STATUS_TERI_SHIFT				0x2
#define AL_UART_MODEM_STATUS_TERI_MASK				(0x1 << \
		AL_UART_MODEM_STATUS_TERI_SHIFT)
/* Delta Data Set Ready */
#define AL_UART_MODEM_STATUS_DDSR_SHIFT				0x1
#define AL_UART_MODEM_STATUS_DDSR_MASK				(0x1 << \
		AL_UART_MODEM_STATUS_DDSR_SHIFT)
/* Delta Clear to Send */
#define AL_UART_MODEM_STATUS_DCTS_SHIFT				0x0
#define AL_UART_MODEM_STATUS_DCTS_MASK				(0x1 << \
		AL_UART_MODEM_STATUS_DCTS_SHIFT)

/**** UART Status Register ****/
#define AL_UART_STATUS_RX_FIFO_FULL_SHIFT			0x4
#define AL_UART_STATUS_RX_FIFO_FULL_MASK			(0x1 << \
		AL_UART_STATUS_RX_FIFO_FULL_SHIFT)
#define AL_UART_STATUS_RX_FIFO_NOT_EMPTY_SHIFT			0x3
#define AL_UART_STATUS_RX_FIFO_NOT_EMPTY_MASK			(0x1 << \
		AL_UART_STATUS_RX_FIFO_NOT_EMPTY_SHIFT)
#define AL_UART_STATUS_TX_FIFO_EMPTY_SHIFT			0x2
#define AL_UART_STATUS_TX_FIFO_EMPTY_MASK			(0x1 << \
		AL_UART_STATUS_TX_FIFO_EMPTY_SHIFT)
#define AL_UART_STATUS_TX_FIFO_NOT_FULL_SHIFT			0x1
#define AL_UART_STATUS_TX_FIFO_NOT_FULL_MASK			(0x1 << \
		AL_UART_STATUS_TX_FIFO_NOT_FULL_SHIFT)
#define AL_UART_STATUS_BUSY_SHIFT				0x0
#define AL_UART_STATUS_BUSY_MASK				(0x1 << \
		AL_UART_STATUS_BUSY_SHIFT)


/**** Software Reset Register ****/
/* XMIT FIFO Reset */
#define AL_UART_SRR_XFR_SHIFT					0x2
#define AL_UART_SRR_XFR_MASK					(0x1 << \
		AL_UART_SRR_XFR_SHIFT)
#define AL_UART_SRR_XFR_RESET					(0x1 << \
		AL_UART_SRR_XFR_SHIFT)
/* RCVR FIFO Reset */
#define AL_UART_SRR_RFR_SHIFT					0x1
#define AL_UART_SRR_RFR_MASK					(0x1 << \
		AL_UART_SRR_RFR_SHIFT)
#define AL_UART_SRR_RFR_RESET					(0x1 << \
		AL_UART_SRR_RFR_SHIFT)
/* UART Reset */
#define AL_UART_SRR_UR_SHIFT					0x0
#define AL_UART_SRR_UR_MASK					(0x1 << \
		AL_UART_SRR_UR_SHIFT)
#define AL_UART_SRR_UR_RESET					(0x1 << \
		AL_UART_SRR_UR_SHIFT)


/**** FIFO Access Register ****/
#define AL_UART_FAR_FIFO_ACCESS_SHIFT				0x0
#define AL_UART_FAR_FIFO_ACCESS_MASK				(0x1 << \
		AL_UART_FAR_FIFO_ACCESS_SHIFT)
#define AL_UART_FAR_FIFO_ACCESS_DISABLE				(0x0 << \
		AL_UART_FAR_FIFO_ACCESS_SHIFT)
#define AL_UART_FAR_FIFO_ACCESS_ENABLE				(0x1 << \
		AL_UART_FAR_FIFO_ACCESS_SHIFT)

/**** Receive FIFO Write ****/
/* Receive FIFO Framing Error */
#define AL_UART_RFW_RFFE_SHIFT					0x9
#define AL_UART_RFW_RFFE_MASK					(0x1 << \
		AL_UART_RFW_RFFE_SHIFT)
#define AL_UART_RFW_FIFO_ACCESS_DISABLE				(0x0 << \
		AL_UART_RFW_FIFO_ACCESS_SHIFT)
#define AL_UART_RFW_FIFO_ACCESS_ENABLE				(0x1 << \
		AL_UART_RFW_FIFO_ACCESS_SHIFT)

/**** Shadow FIFO Enable Register ****/
#define AL_UART_SFE__SHIFT					0x0
#define AL_UART_SFE__MASK					(0x1 << \
		AL_UART_SFE_SHIFT)
#define AL_UART_SFE_SHADOW_FIFO_DISABLE				(0x0 << \
		AL_UART_SFE_SHIFT)
#define AL_UART_SFE_SHADOW_FIFO_ENABLE				(0x1 << \
		AL_UART_SFE_SHIFT)

/**** Component Parameter Register ****/
#define AL_UART_CPR_FIFO_MODE_SHIFT				16
#define AL_UART_CPR_FIFO_MODE_MASK				(0xFF << \
		AL_UART_CPR_FIFO_MODE_SHIFT)
#define AL_UART_CPR_DMA_EXTRA_SHIFT				13
#define AL_UART_CPR_DMA_EXTRA_MASK				(0x1 << \
		AL_UART_CPR_DMA_EXTRA_SHIFT)
#define AL_UART_CPR_ADD_ENCODED_PARAMS_SHIFT			12
#define AL_UART_CPR_ADD_ENCODED_PARAMS_MASK			(0x1 << \
		AL_UART_CPR_ADD_ENCODED_PARAMS_SHIFT)
#define AL_UART_CPR_SHADOW_SHIFT				11
#define AL_UART_CPR_SHADOW_MASK					(0x1 << \
		AL_UART_CPR_SHADOW_SHIFT)
#define AL_UART_CPR_FIFO_STAT_SHIFT				10
#define AL_UART_CPR_FIFO_STAT_MASK				(0x1 << \
		AL_UART_CPR_FIFO_STAT_SHIFT)
#define AL_UART_CPR_FIFO_ACCESS_SHIFT				0x9
#define AL_UART_CPR_FIFO_ACCESS_MASK				(0x1 << \
		AL_UART_CPR_FIFO_ACCESS_SHIFT)
#define AL_UART_CPR_NEW_FEAT_SHIFT				0x8
#define AL_UART_CPR_NEW_FEAT_MASK				(0x1 << \
		AL_UART_CPR_NEW_FEAT_SHIFT)
#define AL_UART_CPR_SIR_LP_MODE_SHIFT				0x7
#define AL_UART_CPR_SIR_LP_MODE_MASK				(0x1 << \
		AL_UART_CPR_SIR_LP_MODE_SHIFT)
#define AL_UART_CPR_SIR_MODE_SHIFT				0x6
#define AL_UART_CPR_SIR_MODE_MASK				(0x1 << \
		AL_UART_CPR_SIR_MODE_SHIFT)
#define AL_UART_CPR_THRE_MODE_SHIFT				0x5
#define AL_UART_CPR_THRE_MODE_MASK				(0x1 << \
		AL_UART_CPR_THRE_MODE_SHIFT)
#define AL_UART_CPR_AFCE_MODE_SHIFT				0x4
#define AL_UART_CPR_AFCE_MODE_MASK				(0x1 << \
		AL_UART_CPR_AFCE_MODE_SHIFT)
#define AL_UART_CPR_APB_DATA_WIDTH_SHIFT			0x0
#define AL_UART_CPR_APB_DATA_WIDTH_MASK				(0x3 << \
		AL_UART_CPR_APB_DATA_WIDTH_SHIFT)


#endif /* _AL_HAL_UART_REGS_H_ */

/** @} */

