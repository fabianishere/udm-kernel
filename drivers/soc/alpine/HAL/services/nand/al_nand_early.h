/*******************************************************************************
Copyright (C) 2016 Annapurna Labs Ltd.

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
#ifndef __AL_NAND_EARLY_H__
#define __AL_NAND_EARLY_H__

#include "al_hal_plat_types.h"
#include "al_hal_nand_defs.h"

/**
 * Early NAND service parameters obtaining
 *
 * @param obj
 *	  Initialized NAND object
 * @param dev_properties
 *             NAND device properties (device specific)
 *
 * @param ecc_config
 *             ECC config (application requirements)
 *
 * @param dev_ext_props
 *             NAND device extra properties
 *
 * @returns	0 upon success, errno upon error
 */
int al_nand_early_params_obtain(
	struct al_nand_ctrl_obj			*obj,
	struct al_nand_dev_properties		*dev_properties,
	struct al_nand_ecc_config		*ecc_config,
	struct al_nand_extra_dev_properties	*dev_ext_props);

/**
 * Early NAND service initialization
 *
 * @returns	0 upon success, errno upon error
 */
int al_nand_early_init(void);

/**
 * Early NAND reading service
 *
 * @param	address
 *		Address in NAND
 * @param	buff_ptr
 *		User buffer to read to
 * @param	num_bytes
 *		Number of bytes to read
 *
 * @returns	0 upon success, errno upon error
 */
int al_nand_early_read(
	unsigned int	address,
	void		*buff_ptr,
	unsigned int	num_bytes);

/**
 * Early NAND writing service
 *
 * @param	address
 *		Address in NAND
 * @param	buff_ptr
 *		User buffer to write from
 * @param	num_bytes
 *		Number of bytes to write
 *
 * @returns	0 upon success, errno upon error
 */
int al_nand_early_write(
	unsigned int	address,
	void		*buff_ptr,
	unsigned int	num_bytes);

/*
 * Erasing the block in block_off.
 *
 * @param	block_off
 *		Block offset in nand.
 */
int al_nand_early_erase_block(
	unsigned int	block_off);

/**
 * Early NAND service get NAND object
 *
 * @return a pointer to the NAND object
 */
struct nand_db *al_nand_early_get_obj(void);

#endif

