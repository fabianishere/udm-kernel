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
 * @defgroup group_sbus SBUS Master
 * @ingroup group_pbs
 *
 *  @{
 * @file   al_mod_hal_sbus.h
 *
 * @brief  includes SBUS Master API
 *
 */

#ifndef __AL_HAL_SBUS_H__
#define __AL_HAL_SBUS_H__

#include "al_mod_hal_types.h"

/* SBUS addresses */
#define AL_SBUS_ADDR_BCAST(addr)		(0x100 - (addr))
#define AL_SBUS_ADDR_BCAST_PCIE			0xe0
#define AL_SBUS_ADDR_BCAST_NON_PCIE		0xe1
#define AL_SBUS_ADDR_SERDES			0x01
#define AL_SBUS_ADDR_SBUS_CTRLR			0x02
#define AL_SBUS_ADDR_SERDES_TNK_MAIN(i)		(0x01 + (i))		/* TODO */
#define AL_SBUS_ADDR_SERDES_TR_TC_MAIN(i)	(0x01 + ((i) * 2))
#define AL_SBUS_ADDR_SERDES_TR_MAIN(i)		(0x01 + ((i) * 2))
#define AL_SBUS_ADDR_SERDES_TR_SEC(i)		(0x01 + ((i) * 2))
#define AL_SBUS_ADDR_SERDES_TR_HS(i)		(0x01 + (i))

/**
 * SBUS handle (to be used as context to below functions)
 */
struct al_mod_sbus_obj {
	struct al_mod_sbm_regs __iomem	*regs;
};

/**
 * SBUS handle initialization
 *
 * @param	obj
 *		SBUS uninitialized handle
 * @param	regs
 *		SBUS master base address
 */
void al_mod_sbus_handle_init(
	struct al_mod_sbus_obj	*obj,
	void __iomem		*regs);

/**
 * SBUS master clock output divider set
 *
 * @param	obj
 *		SBUS initialized handle
 * @param	div_log_2
 *		Divider value (log 2)
 */
void al_mod_sbus_master_clk_out_div_set(
	const struct al_mod_sbus_obj	*obj,
	unsigned int			div_log_2);

/**
 * SBUS master reset assert
 *
 * @param	obj
 *		SBUS initialized handle
 */
void al_mod_sbus_master_reset_assert(
	const struct al_mod_sbus_obj	*obj);

/**
 * SBUS master reset de-assert
 *
 * @param	obj
 *		SBUS initialized handle
 */
void al_mod_sbus_master_reset_deassert(
	const struct al_mod_sbus_obj	*obj);

/**
 * SBUS write
 *
 * @param	obj
 *		SBUS initialized handle
 * @param	receiver_addr
 *		SBUS slave address
 * @param	data_addr
 *		Data address within the slave (slave specific)
 * @param	data
 *		Data to be written to slave (slave specific)
 * @param	wait_vaild
 *		An indication whether or not to wait for issue completion
 *
 * @returns	0 upon success
 */
int al_mod_sbus_write(
	const struct al_mod_sbus_obj	*obj,
	unsigned int			receiver_addr,
	unsigned int			data_addr,
	unsigned int			data,
	al_mod_bool				wait_vaild);

/**
 * SBUS write without SBUS locking (unsafe, but quicker)
 *
 * @param	obj
 *		SBUS initialized handle
 * @param	receiver_addr
 *		SBUS slave address
 * @param	data_addr
 *		Data address within the slave (slave specific)
 * @param	data
 *		Data to be written to slave (slave specific)
 * @param	wait_vaild
 *		An indication whether or not to wait for issue completion
 *
 * @returns	0 upon success
 */
int al_mod_sbus_write_no_lock(
	const struct al_mod_sbus_obj	*obj,
	unsigned int			receiver_addr,
	unsigned int			data_addr,
	unsigned int			data,
	al_mod_bool				wait_vaild);

/**
 * SBUS read
 *
 * @param	obj
 *		SBUS initialized handle
 * @param	receiver_addr
 *		SBUS slave address
 * @param	data_addr
 *		Data address within the slave (slave specific)
 * @param	data
 *		Data read from slave (slave specific)
 *
 * @returns	0 upon success
 */
int al_mod_sbus_read(
	const struct al_mod_sbus_obj	*obj,
	unsigned int			receiver_addr,
	unsigned int			data_addr,
	unsigned int			*data);

/**
 * SBUS read without SBUS locking (unsafe, but quicker)
 *
 * @param	obj
 *		SBUS initialized handle
 * @param	receiver_addr
 *		SBUS slave address
 * @param	data_addr
 *		Data address within the slave (slave specific)
 * @param	data
 *		Data read from slave (slave specific)
 *
 * @returns	0 upon success
 */
int al_mod_sbus_read_no_lock(
	const struct al_mod_sbus_obj	*obj,
	unsigned int			receiver_addr,
	unsigned int			data_addr,
	unsigned int			*data);

/**
 * SBUS masked write
 *
 * @param	obj
 *		SBUS initialized handle
 * @param	receiver_addr
 *		SBUS slave address
 * @param	data_addr
 *		Data address within the slave (slave specific)
 * @param	data_mask
 *		Mask of data to be written to slave
 * @param	data
 *		Data to be written to slave (slave specific)
 * @param	wait_vaild
 *		An indication whether or not to wait for issue completion
 *
 * @returns	0 upon success
 */
int al_mod_sbus_write_masked(
	const struct al_mod_sbus_obj	*obj,
	unsigned int			receiver_addr,
	unsigned int			data_addr,
	unsigned int			data_mask,
	unsigned int			data,
	al_mod_bool				wait_vaild);

/**
 * SBUS ROM download
 *
 * A ROM internal to the SBus Master be read continuously starting from address '0' for 48-bit data,
 * which contains SBus write packet (8-bit Rx address, 8-bit data address, 32-bit data).
 * The SBus controller will write the 48-bit data packet to SBus receivers.
 * When a 48-bit data packet is all '0' data (48'h0), both reading from ROM and writing to SBus
 * receivers will stop.
 *
 * @param	obj
 *		SBUS initialized handle
 *
 * @returns	0 upon success
 */
int al_mod_sbus_rom_download(
	const struct al_mod_sbus_obj	*obj);

/**
 * Lock SBUS for specific slave
 *
 * @param	obj
 *		SBUS initialized handle
 * @param	receiver_addr
 *		SBUS slave address
 */
void al_mod_sbus_lock(
	const struct al_mod_sbus_obj	*obj,
	unsigned int			receiver_addr);

/**
 * Unlock SBUS from previous lock to specific slave
 *
 * @param	obj
 *		SBUS initialized handle
 * @param	receiver_addr
 *		SBUS slave address
 */
void al_mod_sbus_unlock(
	const struct al_mod_sbus_obj	*obj,
	unsigned int			receiver_addr);

/**
 * SBUS master FW init
 *
 * @param	sbus
 *		SBUS initialized handle
 * @param	fw
 *		Firmware buffer
 *		Note: Use NULL for default FW
 * @param	fw_size
 *		Firmware size (32 bit words)
 *		Note: Use 0 when fw is NULL
 *
 * @returns	0 upon success
 */
int al_mod_sbus_master_fw_init(
	const struct al_mod_sbus_obj	*sbus,
	const uint32_t			*fw,
	unsigned int			fw_size);

/**
 * SBUS master FW revision printout
 *
 * @param	sbus
 *		SBUS initialized handle
 */
void al_mod_sbus_master_fw_revision_print(
	const struct al_mod_sbus_obj	*sbus);

/**
 * SBUS master interrupt
 *
 * @param	sbus
 *		SBUS initialized handle
 * @param	code
 *		Core interrupt code
 * @param	data
 *		Core interrupt data
 * @param	res_status
 *		Result status
 * @param	res_data
 *		Result data
 *
 * @returns	0 upon success
 */
int al_mod_sbus_master_interrupt(
	const struct al_mod_sbus_obj	*sbus,
	unsigned int			code,
	unsigned int			data,
	unsigned int			*res_status,
	unsigned int			*res_data);

#endif

/** @} */
