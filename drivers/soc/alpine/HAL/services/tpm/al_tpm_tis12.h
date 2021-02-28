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

#ifndef __AL_TPM_TIS12_H__
#define __AL_TPM_TIS12_H__

#include "al_tpm_if.h"

struct al_tpm_tis12_handle {
	struct al_tpm_if	*tpm_if;
	uint8_t			stat;
};

/**
 * Initialize a handle for TPM TIS 1.2 (TPM interface specification) services
 *
 * @param	handle
 *		Uninitialized handle
 * @param	tpm_if
 *		TPM interface initialized handle
 */
void al_tpm_tis12_handle_init(
	struct al_tpm_tis12_handle	*handle,
	struct al_tpm_if		*tpm_if);

/**
 * TPM vendor ID & device ID get
 *
 * @param	handle
 *		Initialized handle
 * @param	vid
 *		Vendor ID
 * @param	did
 *		Device ID
 */
void al_tpm_tis12_id_get(
	struct al_tpm_tis12_handle	*handle,
	unsigned int			*vid,
	unsigned int			*did);

/**
 * Setup TPM
 *
 * @param	handle
 *		Initialized handle
 *
 * @returns	0 upon success
 *		errno otherwise
 */
int al_tpm_tis12_init(
	struct al_tpm_tis12_handle *handle);

/**
 * TPM start transaction
 *
 * @param	handle
 *		Initialized handle
 * @param	is_write
 *		AL_TRUE if starting a write transaction
 *
 * @returns	0 upon success
 *		errno otherwise
 */
int al_tpm_tis12_start(
	struct al_tpm_tis12_handle	*handle,
	al_bool				is_write);

/**
 * Read from TPM
 *
 * @param	handle
 *		Initialized handle
 * @param	buf
 *		Buffer to read to
 * @param	len
 *		How many bytes to read
 * @param	count
 *		How many bytes actually have been read
 * @param	flags
 *		Read flags
 *
 * @returns	0 upon success
 *		errno otherwise
 */
int al_tpm_tis12_read(
	struct al_tpm_tis12_handle	*handle,
	void				*buf,
	int				len,
	int				*count,
	int				flags);

/**
 * Write to TPM
 *
 * @param	handle
 *		Initialized handle
 * @param	buf
 *		Buffer to write
 * @param	len
 *		How many bytes to write
 *
 * @returns	0 upon success
 *		errno otherwise
 */
int al_tpm_tis12_write(
	struct al_tpm_tis12_handle	*handle,
	void				*buf,
	int				len);

/**
 * TPM finish transaction
 *
 * @param	handle
 *		Initialized handle
 * @param	is_write
 *		AL_TRUE if finishing a write transaction
 * @param	is_err
 *		AL_TRUE if recovering from error
 *
 * @returns	0 upon success
 *		errno otherwise
 */
int al_tpm_tis12_end(
	struct al_tpm_tis12_handle	*handle,
	al_bool				is_write,
	al_bool				is_err);

#endif

