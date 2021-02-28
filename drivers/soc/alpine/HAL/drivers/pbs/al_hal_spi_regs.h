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

#ifndef _AL_SPI_REGS_H_
#define _AL_SPI_REGS_H_

#include "al_hal_plat_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SPI_CS_INDEX_TO_VAL(v)	(0x1<<(v))


/*
 * Unit Registers
 */

struct al_spi_regs {
	uint16_t	ctrlr0;		/*0x00*/
	uint16_t	reserved0;
	uint16_t	ctrlr1;
	uint16_t	reserved1;
	uint16_t	ssienr;
	uint16_t	reserved2;
	uint16_t	mwcr;
	uint16_t	reserved3;
	uint16_t	ser;		/*0x10*/
	uint16_t	reserved4;
	uint16_t	baudr;
	uint16_t	reserved5;
	uint16_t	txftlr;
	uint16_t	reserved6;
	uint16_t	rxftlr;
	uint16_t	reserved7;
	uint16_t	txflr;		/*0x20*/
	uint16_t	reserved8;
	uint16_t	rxflr;
	uint16_t	reserved9;
	uint16_t	sr;
	uint16_t	reservedA;
	uint16_t	imr;
	uint16_t	reservedB;
	uint16_t	isr;		/*0x30*/
	uint16_t	reservedC;
	uint16_t	risr;
	uint16_t	reservedD;
	uint16_t	txoicr;
	uint16_t	reservedE;
	uint16_t	rxoicr;
	uint16_t	reservedF;
	uint16_t	rxuicr;		/*0x40*/
	uint16_t	reserved10;
	uint16_t	msticr;
	uint16_t	reserved11;
	uint16_t	icr;
	uint16_t	reserved12;
	uint16_t	dmacr;
	uint16_t	reserved13;
	uint16_t	dmatdlr;	/*0x50*/
	uint16_t	reserved14;
	uint16_t	dmardlr;
	uint16_t	reserved15;
	uint32_t	idr;
	uint32_t	ssi_comp_version;
	uint16_t	dr[72];		/*0x60*/
	uint16_t	rx_sample_dly;	/*0XF0*/
	uint16_t	reserved16;
	/*
	 * spi core deasserts CS when TX FIFO is empty, this behavior can
	 * damage ongoing transactions.
	 * i.o. to fix this behavior, new register have been added - ssi_ovr:
	 *   (*) ssi_ovr bits 3:0 control the behavior of corresponding cs 3:0:
	 *     - when bit_x is set, cs_x will be set according to the ser reg
	 *     - when bit_x is clear, cs_x will behave according to spi core
	 *   (*) ssi_ovr bit 7:6 control the output enable signal
	 *     - bit 6 controls the value of output enable signal
	 *     - bit 7 disables/enables the use of bit 6
	 */
	uint16_t	ssi_ovr;
	uint16_t	reserved17;

};


/*
 * Registers Fields
 */

#define AL_SPI_CS_SHIFT			0 /* chip select - select slave number i
					by activating the i-th bit */
#define AL_SPI_CS_MASK			(0xF << AL_SPI_CS_SHIFT)
#define AL_SPI_CS_NONE			(0x0 << AL_SPI_CS_SHIFT)
#define AL_SPI_CS_0			(0x1 << AL_SPI_CS_SHIFT)
#define AL_SPI_CS_1			(0x2 << AL_SPI_CS_SHIFT)
#define AL_SPI_CS_2			(0x4 << AL_SPI_CS_SHIFT)
#define AL_SPI_CS_3			(0x8 << AL_SPI_CS_SHIFT)

#define AL_SPI_CTRLR0_TMOD_SHIFT	8
#define	AL_SPI_CTRLR0_TMOD_MASK		(0x3 << AL_SPI_CTRLR0_TMOD_SHIFT)
#define AL_SPI_CTRLR0_TMOD_TRANS_RECV	(0x0 << AL_SPI_CTRLR0_TMOD_SHIFT)
#define AL_SPI_CTRLR0_TMOD_TRANS	(0x1 << AL_SPI_CTRLR0_TMOD_SHIFT)
#define AL_SPI_CTRLR0_TMOD_RECV		(0x2 << AL_SPI_CTRLR0_TMOD_SHIFT)
#define AL_SPI_CTRLR0_TMOD_EEPROM	(0x3 << AL_SPI_CTRLR0_TMOD_SHIFT)

#define AL_SPI_CTRLR0_SCPOL_SHIFT	7 /* When is the clock INACTIVE */
#define	AL_SPI_CTRLR0_SCPOL_MASK	(0x1 << AL_SPI_CTRLR0_SCPOL_SHIFT)
#define	AL_SPI_CTRLR0_SCPOL_LOW		(0x0 << AL_SPI_CTRLR0_SCPOL_SHIFT)
#define	AL_SPI_CTRLR0_SCPOL_HIGH	(0x1 << AL_SPI_CTRLR0_SCPOL_SHIFT)

#define AL_SPI_CTRLR0_SCPH_SHIFT	6 /* When is the clock INACTIVE */
#define	AL_SPI_CTRLR0_SCPH_MASK		(0x1 << AL_SPI_CTRLR0_SCPH_SHIFT)
#define	AL_SPI_CTRLR0_SCPH_SS		(0x0 << AL_SPI_CTRLR0_SCPH_SHIFT)
#define	AL_SPI_CTRLR0_SCPH_CLK		(0x1 << AL_SPI_CTRLR0_SCPH_SHIFT)

#define AL_SPI_CTRLR0_FRF_SHIFT		4 /* Select the serial protocol */
#define	AL_SPI_CTRLR0_FRF_MASK		(0x3 << AL_SPI_CTRLR0_FRF_SHIFT)
#define	AL_SPI_CTRLR0_FRF_MOTOROLA	(0x0 << AL_SPI_CTRLR0_FRF_SHIFT)
#define	AL_SPI_CTRLR0_FRF_TI		(0x1 << AL_SPI_CTRLR0_FRF_SHIFT)
#define	AL_SPI_CTRLR0_FRF_NSM		(0x2 << AL_SPI_CTRLR0_FRF_SHIFT)
#define	AL_SPI_CTRLR0_FRF_RESERVED	(0x3 << AL_SPI_CTRLR0_FRF_SHIFT)

#define AL_SPI_CTRLR0_DFS_SHIFT		0 /* Data Frame size */
#define	AL_SPI_CTRLR0_DFS_MASK		(0xF << AL_SPI_CTRLR0_DFS_SHIFT)
#define	AL_SPI_CTRLR0_DFS_4_BITS	(0x3 << AL_SPI_CTRLR0_DFS_SHIFT)
#define	AL_SPI_CTRLR0_DFS_5_BITS	(0x4 << AL_SPI_CTRLR0_DFS_SHIFT)
#define	AL_SPI_CTRLR0_DFS_6_BITS	(0x5 << AL_SPI_CTRLR0_DFS_SHIFT)
#define	AL_SPI_CTRLR0_DFS_7_BITS	(0x6 << AL_SPI_CTRLR0_DFS_SHIFT)
#define	AL_SPI_CTRLR0_DFS_8_BITS	(0x7 << AL_SPI_CTRLR0_DFS_SHIFT)
#define	AL_SPI_CTRLR0_DFS_9_BITS	(0x8 << AL_SPI_CTRLR0_DFS_SHIFT)
#define	AL_SPI_CTRLR0_DFS_10_BITS	(0x9 << AL_SPI_CTRLR0_DFS_SHIFT)
#define	AL_SPI_CTRLR0_DFS_11_BITS	(0xA << AL_SPI_CTRLR0_DFS_SHIFT)
#define	AL_SPI_CTRLR0_DFS_12_BITS	(0xB << AL_SPI_CTRLR0_DFS_SHIFT)
#define	AL_SPI_CTRLR0_DFS_13_BITS	(0xC << AL_SPI_CTRLR0_DFS_SHIFT)
#define	AL_SPI_CTRLR0_DFS_14_BITS	(0xD << AL_SPI_CTRLR0_DFS_SHIFT)
#define	AL_SPI_CTRLR0_DFS_15_BITS	(0xE << AL_SPI_CTRLR0_DFS_SHIFT)
#define	AL_SPI_CTRLR0_DFS_16_BITS	(0xF << AL_SPI_CTRLR0_DFS_SHIFT)


#define AL_SPI_SR_BUSY_SHIFT		0
#define	AL_SPI_SR_BUSY_MASK		(0x1 << AL_SPI_SR_BUSY_SHIFT)
#define	AL_SPI_SR_BUSY_IDLE		(0x0 << AL_SPI_SR_BUSY_SHIFT)
#define	AL_SPI_SR_BUSY_ACTIVE		(0x1 << AL_SPI_SR_BUSY_SHIFT)

#define AL_SPI_SR_TFNF_SHIFT		1 /* transmit fifo not full */
#define	AL_SPI_SR_TFNF_MASK		(0x1 << AL_SPI_SR_TFNF_SHIFT)
#define	AL_SPI_SR_TFNF_FULL		(0x0 << AL_SPI_SR_TFNF_SHIFT)
#define	AL_SPI_SR_TFNF_NOT_FULL		(0x1 << AL_SPI_SR_TFNF_SHIFT)

#define AL_SPI_SR_TFE_SHIFT		2 /* transmit fifo empty */
#define	AL_SPI_SR_TFE_MASK		(0x1 << AL_SPI_SR_TFE_SHIFT)
#define	AL_SPI_SR_TFE_NOT_EMPTY		(0x0 << AL_SPI_SR_TFE_SHIFT)
#define	AL_SPI_SR_TFE_EMPTY		(0x1 << AL_SPI_SR_TFE_SHIFT)

#define AL_SPI_SR_RFNE_SHIFT		3 /* receive fifo not empty */
#define	AL_SPI_SR_RFNE_MASK		(0x1 << AL_SPI_SR_RFNE_SHIFT)
#define	AL_SPI_SR_RFNE_EMPTY		(0x0 << AL_SPI_SR_RFNE_SHIFT)
#define	AL_SPI_SR_RFNE_NOT_EMPTY	(0x1 << AL_SPI_SR_RFNE_SHIFT)

#define AL_SPI_SR_RFF_SHIFT		4 /* receive fifo full */
#define	AL_SPI_SR_RFF_MASK		(0x1 << AL_SPI_SR_RFF_SHIFT)
#define	AL_SPI_SR_RFF_NOT_FULL		(0x0 << AL_SPI_SR_RFF_SHIFT)
#define	AL_SPI_SR_RFF_FULL		(0x1 << AL_SPI_SR_RFF_SHIFT)

#define AL_SPI_SR_TXE_SHIFT		5 /* tramsmit error */
#define	AL_SPI_SR_TXE_MASK		(0x1 << AL_SPI_SR_TXE_SHIFT)
#define	AL_SPI_SR_TXE_OKAY		(0x0 << AL_SPI_SR_TXE_SHIFT)
#define	AL_SPI_SR_TXE_TRANS_ERROR	(0x1 << AL_SPI_SR_TXE_SHIFT)

#define AL_SPI_SR_DCOL_SHIFT		6 /* data collision error */
#define	AL_SPI_SR_DCOL_MASK		(0x1 << AL_SPI_SR_DCOL_SHIFT)
#define	AL_SPI_SR_DCOL_OKAY		(0x0 << AL_SPI_SR_DCOL_SHIFT)
#define	AL_SPI_SR_DCOL_DATA_COL_ERROR	(0x1 << AL_SPI_SR_DCOL_SHIFT)


#define AL_SPI_SSIENR_SHIFT		0 /* ssi protocol enable register */
#define	AL_SPI_SSIENR_MASK		(0x1 << AL_SPI_SSIENR_SHIFT)
#define AL_SPI_SSIENR_DISABLE		(0x0 << AL_SPI_SSIENR_SHIFT)
#define AL_SPI_SSIENR_ENABLE		(0x1 << AL_SPI_SSIENR_SHIFT)

#define AL_SPI_SSI_OVR_CS_0		(0x1 << 0)
#define AL_SPI_SSI_OVR_CS_1		(0x1 << 1)
#define AL_SPI_SSI_OVR_CS_2		(0x1 << 2)
#define AL_SPI_SSI_OVR_CS_3		(0x1 << 3)
#define AL_SPI_SSI_OVR_OE_VAL		(0x1 << 6)
#define AL_SPI_SSI_OVR_OE_EN		(0x1 << 7)

#define AL_SPI_DMACR_RDMAE		(0x01 << 0)
#define AL_SPI_DMACR_TDMAE		(0x01 << 1)

#ifdef __cplusplus
}
#endif

#endif /* _AL_SPI_REGS_H_ */
