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

#include "al_hal_otp.h"
#include "al_hal_otp_regs.h"
#include "al_hal_pbs_utils.h"

#define OTP_SIM			0

#define OTP_MAGIC_NUM_VAL	0xAFBEAFBE

#define AL_OTP_WORD_IDX_LOCK(w)	\
	((otp_handle->num_words - (otp_handle->num_words / AL_OTP_NUM_WORDS_PER_LOCK_WORD)) +	\
	(w) / AL_OTP_NUM_WORDS_PER_LOCK_WORD)

#define AL_OTP_WORD_IDX_BUSY	(otp_handle->num_words - 1)
#define AL_OTP_BIT_IDX_BUSY	31

#if OTP_SIM
static uint32_t otpw[AL_OTP_NUM_WORDS_V3];
static uint32_t otpr[AL_OTP_NUM_WORDS_V3];
static al_bool write_en;
#endif

/******************************************************************************
 ******************************************************************************/
int al_otp_handle_init(
	struct al_otp_handle	*otp_handle,
	void __iomem		*otp_regs_base,
	void __iomem		*pbs_regs_base)
{
	al_assert(otp_handle);
	al_assert(otp_regs_base);
	al_assert(pbs_regs_base);

	otp_handle->pbs_regs_base = pbs_regs_base;

	if (al_pbs_dev_id_get(pbs_regs_base) <= PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V2) {
		otp_handle->otpw = ((struct al_otp_regs_v2 __iomem *)otp_regs_base)->otpw;
		otp_handle->otpr = ((struct al_otp_regs_v2 __iomem *)otp_regs_base)->otpr;
		otp_handle->num_words = AL_OTP_NUM_WORDS_V2;
	} else {
		otp_handle->otpw = ((struct al_otp_regs_v3 __iomem *)otp_regs_base)->otpw;
		otp_handle->otpr = ((struct al_otp_regs_v3 __iomem *)otp_regs_base)->otpr;
		otp_handle->num_words = AL_OTP_NUM_WORDS_V3;
	}

#if OTP_SIM
	otp_handle->otpw = otpw;
	otp_handle->otpr = otpr;
#endif

	return 0;
}

/******************************************************************************
 ******************************************************************************/
uint32_t al_otp_read_word(
	const struct al_otp_handle	*otp_handle,
	unsigned int			word_idx)
{
	al_assert(otp_handle);
	al_assert(word_idx < AL_OTP_WORD_IDX_LOCK(0));

	return al_reg_read32(&otp_handle->otpr[word_idx]);
}

/******************************************************************************
 ******************************************************************************/
uint32_t al_otp_read_word_direct(
	const struct al_otp_handle	*otp_handle,
	unsigned int			word_idx)
{
	al_assert(otp_handle);
	al_assert(word_idx < AL_OTP_WORD_IDX_LOCK(0));

	return al_reg_read32(&otp_handle->otpw[word_idx]);
}

/******************************************************************************
 ******************************************************************************/
void al_otp_write_enable(
	const struct al_otp_handle	*otp_handle)
{
	struct al_pbs_regs __iomem *pbs_regs;

	al_assert(otp_handle);

	pbs_regs = (struct al_pbs_regs __iomem *)otp_handle->pbs_regs_base;

#if OTP_SIM
	write_en = AL_TRUE;
#else
	al_reg_write32(&pbs_regs->unit.otp_magic_num, OTP_MAGIC_NUM_VAL);
#endif
}

/******************************************************************************
 ******************************************************************************/
void al_otp_write_disable(
	const struct al_otp_handle	*otp_handle)
{
	struct al_pbs_regs __iomem *pbs_regs;

	al_assert(otp_handle);

	pbs_regs = (struct al_pbs_regs __iomem *)otp_handle->pbs_regs_base;

#if OTP_SIM
	write_en = AL_FALSE;
#else
	al_reg_write32(&pbs_regs->unit.otp_magic_num, 0);
#endif
}

/******************************************************************************
 ******************************************************************************/
int al_otp_write_word(
	const struct al_otp_handle	*otp_handle,
	unsigned int			word_idx,
	uint32_t			val)
{
	uint32_t read_val;

	al_assert(otp_handle);
	al_assert(word_idx < AL_OTP_WORD_IDX_LOCK(0));

#if OTP_SIM
	if (write_en)
		al_reg_write32(&otp_handle->otpw[word_idx], val);
#else
	al_reg_write32(&otp_handle->otpw[word_idx], val);

	while (al_reg_read32(&otp_handle->otpr[AL_OTP_WORD_IDX_BUSY]) & AL_BIT(AL_OTP_BIT_IDX_BUSY))
		;
#endif

	read_val = al_reg_read32(&otp_handle->otpw[word_idx]);
	if (read_val != val) {
		al_err("%s(%u, %08x) failed! (read %08x)\n",
				__func__, word_idx, val, read_val);
		return -EIO;
	}

	return 0;
}

/******************************************************************************
 ******************************************************************************/
int al_otp_write_word_shadow(
	const struct al_otp_handle	*otp_handle,
	unsigned int			word_idx,
	uint32_t			val)
{
	uint32_t read_val;

	al_assert(otp_handle);
	al_assert(word_idx < AL_OTP_WORD_IDX_LOCK(0));

	al_reg_write32(&otp_handle->otpr[word_idx], val);

	read_val = al_reg_read32(&otp_handle->otpr[word_idx]);
	if (read_val != val) {
		al_err("%s(%u, %08x) failed! (read %08x)\n",
				__func__, word_idx, val, read_val);
		return -EIO;
	}

	return 0;
}

/******************************************************************************
 ******************************************************************************/
int al_otp_lock_word(
	const struct al_otp_handle	*otp_handle,
	unsigned int			word_idx)
{
	uint32_t val;
	uint32_t read_val;

	al_assert(otp_handle);
	al_assert(word_idx < AL_OTP_WORD_IDX_LOCK(0));

	val = al_reg_read32(&otp_handle->otpw[AL_OTP_WORD_IDX_LOCK(word_idx)]);
	val |= (1 << (word_idx % AL_OTP_NUM_WORDS_PER_LOCK_WORD));

#if OTP_SIM
	if (write_en)
		al_reg_write32(&otp_handle->otpw[AL_OTP_WORD_IDX_LOCK(word_idx)], val);
#else
	al_reg_write32(&otp_handle->otpw[AL_OTP_WORD_IDX_LOCK(word_idx)], val);

	while (al_reg_read32(&otp_handle->otpr[AL_OTP_WORD_IDX_BUSY]) & AL_BIT(AL_OTP_BIT_IDX_BUSY))
		;
#endif

	read_val = al_reg_read32(&otp_handle->otpw[AL_OTP_WORD_IDX_LOCK(word_idx)]);
	if (read_val != val) {
		al_err("%s(%u) failed! (read %08x, expected %08x)\n",
				__func__, word_idx, read_val, val);
		return -EIO;
	}

	return 0;
}

/******************************************************************************
 ******************************************************************************/
al_bool al_otp_word_is_locked(
		const struct al_otp_handle	*otp_handle,
		unsigned int			word_idx)
{
	uint32_t read_val;
	al_bool ret_val;

	al_assert(otp_handle);
	al_assert(word_idx < AL_OTP_WORD_IDX_LOCK(0));

	read_val = al_reg_read32(&otp_handle->otpr[AL_OTP_WORD_IDX_LOCK(word_idx)]);

	ret_val = ((read_val & (1 << (word_idx % AL_OTP_NUM_WORDS_PER_LOCK_WORD)))
		? AL_TRUE : AL_FALSE);

	return ret_val;
}

/******************************************************************************
 ******************************************************************************/
al_bool al_otp_word_is_locked_direct(
		const struct al_otp_handle	*otp_handle,
		unsigned int			word_idx)
{
	uint32_t read_val;
	al_bool ret_val;

	al_assert(otp_handle);
	al_assert(word_idx < AL_OTP_WORD_IDX_LOCK(0));

	read_val = al_reg_read32(&otp_handle->otpw[AL_OTP_WORD_IDX_LOCK(word_idx)]);

	ret_val = ((read_val & (1 << word_idx)) ? AL_TRUE : AL_FALSE);

	return ret_val;
}

