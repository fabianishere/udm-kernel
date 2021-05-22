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

#include "al_tpm_if_i2c.h"

#define MAX_NUM_ATTEMPTS		3
#define INTER_ATTEMPT_DELAY_USEC	5000
#define GUARD_TIME_DEFAULT_USEC		250

struct al_tpm_if_i2c_reg_map_entry {
	unsigned int	tcg_offset;
	uint8_t		i2c_offset;
};

static struct al_tpm_if_i2c_reg_map_entry reg_map[] = {
	{ 0, 0, },		/* TPM_ACCESS_0 */
	{ 0x18, 1, },		/* TPM_STS_0 */
	{ 0x19, 2, },		/* TPM_STS_0 */
	{ 0x1a, 3, },		/* TPM_STS_0 */
	{ 0x1b, 4, },		/* TPM_STS_0 */
	{ 0x24, 5, },		/* TPM_DATA_FIFO_0 */
	{ 0xF00, 6, },		/* TPM_DID_VID_0 */
	{ 0xF01, 7, },		/* TPM_DID_VID_0 */
	{ 0xF02, 8, },		/* TPM_DID_VID_0 */
	{ 0xF03, 9, },};	/* TPM_DID_VID_0 */

static uint8_t reg_offset_get(unsigned int tcg_offset)
{
	unsigned int i;
	unsigned int locality_idx = tcg_offset >> 12;

	for (i = 0; i < AL_ARR_SIZE(reg_map); i++) {
		if (reg_map[i].tcg_offset == (tcg_offset & 0xfff))
			return (0x10 * locality_idx) + reg_map[i].i2c_offset;
	}

	al_err("%s: couldn't find i2c offset for tcg_offset %u\n", __func__, tcg_offset);

	return 0;
}

static int al_tpm_if_i2c_read(
	struct al_tpm_if *tpm_if, unsigned int offset, uint8_t *data, unsigned int data_size)
{
	struct al_tpm_if_i2c_priv *priv;
	uint8_t addr = reg_offset_get(offset);
	int err;
	int i;

	al_assert(tpm_if);
	al_assert(tpm_if->priv);

	priv = (struct al_tpm_if_i2c_priv *)tpm_if->priv;

	for (i = 0; i < MAX_NUM_ATTEMPTS; i++) {
		err = al_i2c_read(priv->i2c_if, priv->tpm_i2c_addr, 1, &addr, data_size, data);
		if (!err)
			break;

		al_dbg("%s: retrying reading %u bytes from %02x!\n", __func__, data_size, offset);
		al_udelay(INTER_ATTEMPT_DELAY_USEC);
	}

	al_udelay(priv->guard_time);

	if (err) {
		al_err("%s: failed reading %u bytes from %02x!\n", __func__, data_size, offset);
		al_memset(data, 0xff, data_size);
	}

	return err;
}

static uint8_t al_tpm_if_i2c_read1(struct al_tpm_if *tpm_if, unsigned int offset)
{
	uint8_t data;

	al_tpm_if_i2c_read(tpm_if, offset, &data, 1);

	return data;
}

static uint32_t al_tpm_if_i2c_read4(struct al_tpm_if *tpm_if, unsigned int offset)
{
	uint32_t data;

	al_tpm_if_i2c_read(tpm_if, offset, (uint8_t *)&data, 4);

	return data;
}

static int al_tpm_if_i2c_write(
	struct al_tpm_if *tpm_if, unsigned int offset, uint8_t *data, unsigned int data_size)
{
	struct al_tpm_if_i2c_priv *priv;
	uint8_t addr = reg_offset_get(offset);
	int err;
	int i;

	al_assert(tpm_if);
	al_assert(tpm_if->priv);

	priv = (struct al_tpm_if_i2c_priv *)tpm_if->priv;

	for (i = 0; i < MAX_NUM_ATTEMPTS; i++) {
		err = al_i2c_write(priv->i2c_if, priv->tpm_i2c_addr, 1, &addr, data_size, data);
		if (!err)
			break;

		al_dbg("%s: retrying writing %u bytes to %02x!\n", __func__, data_size, offset);
		al_udelay(INTER_ATTEMPT_DELAY_USEC);
	}

	al_udelay(priv->guard_time);

	if (err)
		al_err("%s: failed writing %u bytes to %02x!\n", __func__, data_size, offset);

	return err;
}

static void al_tpm_if_i2c_write1(struct al_tpm_if *tpm_if, unsigned int offset, uint8_t val)
{
	al_tpm_if_i2c_write(tpm_if, offset, &val, 1);
}

static void al_tpm_if_i2c_write4(struct al_tpm_if *tpm_if, unsigned int offset, uint32_t val)
{
	al_tpm_if_i2c_write(tpm_if, offset, (uint8_t *)&val, 4);
}

void al_tpm_if_i2c_handle_init_aux(
	struct al_tpm_if		*tpm_if,
	struct al_tpm_if_i2c_priv	*tpm_if_priv,
	struct al_tpm_if_i2c_params	*al_tpm_if_i2c_params)
{
	al_assert(tpm_if);
	al_assert(tpm_if_priv);
	al_assert(al_tpm_if_i2c_params);
	al_assert(al_tpm_if_i2c_params->i2c_if);

	al_memset(tpm_if, 0, sizeof(struct al_tpm_if));
	al_memset(tpm_if_priv, 0, sizeof(struct al_tpm_if_i2c_priv));

	tpm_if_priv->i2c_if = al_tpm_if_i2c_params->i2c_if;
	tpm_if_priv->tpm_i2c_addr = al_tpm_if_i2c_params->tpm_i2c_addr;
	if (al_tpm_if_i2c_params->guard_time == 0)
		tpm_if_priv->guard_time = GUARD_TIME_DEFAULT_USEC;
	else
		tpm_if_priv->guard_time = al_tpm_if_i2c_params->guard_time;

	tpm_if->priv = tpm_if_priv;

	tpm_if->read = al_tpm_if_i2c_read;
	tpm_if->read1 = al_tpm_if_i2c_read1;
	tpm_if->read4 = al_tpm_if_i2c_read4;
	tpm_if->write = al_tpm_if_i2c_write;
	tpm_if->write1 = al_tpm_if_i2c_write1;
	tpm_if->write4 = al_tpm_if_i2c_write4;
}


void al_tpm_if_i2c_handle_init(
	struct al_tpm_if		*tpm_if,
	struct al_tpm_if_i2c_priv	*tpm_if_priv,
	struct al_i2c_interface	*i2c_if,
	unsigned int		tpm_i2c_addr)
{
	struct al_tpm_if_i2c_params params;

	al_memset(&params, 0, sizeof(struct al_tpm_if_i2c_params));

	params.i2c_if = i2c_if;
	params.tpm_i2c_addr = tpm_i2c_addr;
	params.guard_time = 0; /* use default */

	al_tpm_if_i2c_handle_init_aux(tpm_if, tpm_if_priv, &params);
}
