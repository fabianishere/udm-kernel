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
 * @defgroup group_otp OTP
 * @ingroup group_sys_services
 *
 * The One Time Programmable Memory (OTMP) provides the necessary information
 * for Secure Boot, e.g., Secure Boot enforcement, secure key, etc.
 * The OTPM also provides manufacturing information, as well as the software
 * revision ID, etc.
 *
 *  @{
 *	Common Operation Example:
 * @code
 * void main()
 * {
 *	struct al_otp_handle handle;
 *	int err;
 *
 *	// Warning!!!! Irreversible!!!
 *	err = al_otp_handle_init(&handle, (void __iomem*)0xfd896000,
 *		(void __iome*)0xfd8a8000);
 *	if (err)
 *		return err;
 *
 *	al_otp_write_enable(&handle);
 *
 *	err = al_otp_write_word(&handle, 0, 0x12345678);
 *	if (err)
 *		printf("Unable to write OTP word!\n");
 *
 *	al_otp_write_disable(&handle);
 * }
 * @endcode
 *      @file  al_hal_otp.h
 *      @brief HAL Driver Header for the Annapurna Labs OTP unit
 *
 */

#ifndef _AL_HAL_OTP_H_
#define _AL_HAL_OTP_H_

#include "al_hal_common.h"

/*********************** Data Structures and Types ***************************/

/**
 * al_otp_handle: data structure used by the HAL to handle OTP functionality.
 * this structure is allocated and set to zeros by the upper layer, then it is
 * initialized by the al_otp_init() that should be called before any other
 * function of this API. later, this handle passed to the API functions.
 */
struct al_otp_handle {
	void __iomem		*pbs_regs_base;
	uint32_t __iomem	*otpw;
	uint32_t __iomem	*otpr;
	unsigned int		num_words;
};

/************************************* API ***********************************/

/**
 * @brief Initialize a OTP handle structure
 *
 * @param otp_handle
 *		an allocated, non-initialized instance.
 * @param otp_regs_base
 *		the virtual base address of the OTP internal registers
 * @param pbs_regs_base
 *		the virtual base address of the PBS internal registers
 *
 * @return 0 if no error found.
 */
int al_otp_handle_init(
	struct al_otp_handle	*otp_handle,
	void __iomem		*otp_regs_base,
	void __iomem		*pbs_regs_base);

/**
 * OTP read word from the eFuse shadow
 *
 * @param	otp_handle
 *		otp handle
 *
 * @param	word_idx
 *		Index of word to read:
 *		Alpine V1, V2: 0 - 30
 *		Alpine V3: 0 - 123
 *
 * @returns
 */
uint32_t al_otp_read_word(
	const struct al_otp_handle	*otp_handle,
	unsigned int			word_idx);

/**
 * OTP read word directly from the eFuse
 *
 * @param	otp_handle
 *		otp handle
 *
 * @param	word_idx
 *		Index of word to read:
 *		Alpine V1, V2: 0 - 30
 *		Alpine V3: 0 - 123
 *
 * @returns
 */
uint32_t al_otp_read_word_direct(
	const struct al_otp_handle	*otp_handle,
	unsigned int			word_idx);

/**
 * OTP write enable
 *
 * @param	otp_handle
 *		otp handle
 */
void al_otp_write_enable(
	const struct al_otp_handle	*otp_handle);

/**
 * OTP write disable
 *
 * @param	otp_handle
 *		otp handle
 */
void al_otp_write_disable(
	const struct al_otp_handle	*otp_handle);

/**
 * !!! NOTICE: Irreversible operation
 *
 * OTP write word to the eFuse
 *
 * @param	otp_handle
 *		otp handle
 *
 * @param	word_idx
 *		Index of word to be written:
 *		Alpine V1, V2: 0 - 30
 *		Alpine V3: 0 - 123
 *
 * @param	val
 *		value to be written
 *
 * @return 0 if no error found.
 */
int al_otp_write_word(
	const struct al_otp_handle	*otp_handle,
	unsigned int			word_idx,
	uint32_t			val);

/**
 * OTP write word to the eFuse shadow
 *
 * @param	otp_handle
 *		otp handle
 *
 * @param	word_idx
 *		Index of word to be written:
 *		Alpine V1, V2: 0 - 30
 *		Alpine V3: 0 - 123
 *
 * @param	val
 *		value to be written
 *
 * @return 0 if no error found.
 */
int al_otp_write_word_shadow(
	const struct al_otp_handle	*otp_handle,
	unsigned int			word_idx,
	uint32_t			val);

/**
 * !!! NOTICE: Irreversible operation
 *
 * OTP lock eFuse word
 *
 * @param	otp_handle
 *		otp handle
 *
 * @param	word_idx
 *		Index of word to be locked:
 *		Alpine V1, V2: 0 - 30
 *		Alpine V3: 0 - 123
 *
 * @return 0 if no error found.
 */
int al_otp_lock_word(
	const struct al_otp_handle	*otp_handle,
	unsigned int			word_idx);

/**
 *
 * OTP check if OTP eFuse word is locked
 *
 * @param	otp_handle
 *		otp handle
 *
 * @param	word_idx
 *		Index of word to be checked:
 *		Alpine V1, V2: 0 - 30
 *		Alpine V3: 0 - 123
 *
 * @return AL_TRUE if word is locked, AL_FALSE if not locked
 */
al_bool al_otp_word_is_locked(
		const struct al_otp_handle	*otp_handle,
		unsigned int			word_idx);

/**
 *
 * OTP check if OTP eFuse word is locked (directly in eFuse)
 *
 * @param	otp_handle
 *		otp handle
 *
 * @param	word_idx
 *		Index of word to be checked (0 - 30)
 *
 * @return AL_TRUE if word is locked, AL_FALSE if not locked
 */
al_bool al_otp_word_is_locked_direct(
		const struct al_otp_handle	*otp_handle,
		unsigned int			word_idx);

#endif
/** @} end of groupotp group */

