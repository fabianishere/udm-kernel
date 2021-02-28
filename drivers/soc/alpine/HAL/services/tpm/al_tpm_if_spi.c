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

#include "al_tpm_if_spi.h"

#define TPM_SPI_TIMEOUT 1000000
#define TIS_BASE_OFFSET 0xD4
#define DEFAULT_FREQ	1000000

static int al_tpm_if_spi_read(
	struct al_tpm_if *tpm_if, unsigned int offset, uint8_t *data, unsigned int data_size)
{
	int err = 0;
	struct al_tpm_if_spi *tpm_if_spi;
	al_assert(tpm_if);
	al_assert(tpm_if->priv);

	tpm_if_spi = tpm_if->priv;

	err = al_spi_claim_bus(
		tpm_if_spi->spi_if, tpm_if_spi->freq,  tpm_if_spi->phase,
		tpm_if_spi->polarity, tpm_if_spi->cs);
	if (err) {
		al_err("%s: TPM SPI claim bus failed!\n", __func__);
		*data = 0;
		return err;
	}

	al_memset(tpm_if_spi->TX, 0, TPM_MAX_COMMAND_SIZE);
	al_memset(tpm_if_spi->RX, 0, TPM_MAX_COMMAND_SIZE);
	tpm_if_spi->TX[0] = 0x80 | (data_size-1); /*0x80 = read  | 0=1byte*/
	/* Legacy base offset from memory mapped TPM driver
	requires by NationZ, ignored by Infenion*/
	tpm_if_spi->TX[1] = TIS_BASE_OFFSET;
	tpm_if_spi->TX[2] = (offset>>8)  & 0xFF;
	tpm_if_spi->TX[3] = (offset)     & 0xFF;
	err = al_spi_read(
			tpm_if_spi->spi_if,
			tpm_if_spi->TX,
			4,
			tpm_if_spi->RX,
			data_size,
			tpm_if_spi->cs,
			TPM_SPI_TIMEOUT);
	if (!err)
		al_memcpy(data, tpm_if_spi->RX, data_size);
	else
		*data = 0;

	al_spi_release_bus(tpm_if_spi->spi_if, tpm_if_spi->cs);

	return err;
}

static uint8_t al_tpm_if_spi_read1(struct al_tpm_if *tpm_if, unsigned int offset)
{
	uint8_t data;
	int err;

	err = al_tpm_if_spi_read(tpm_if, offset, &data, 1);
	if (err)
		al_err("TPM SPI access failed\n");
	return data;
}

static uint32_t al_tpm_if_spi_read4(struct al_tpm_if *tpm_if, unsigned int offset)
{
	uint32_t data;
	int err = 0;
	err = al_tpm_if_spi_read(tpm_if, offset, (uint8_t *)&data, 4);
	if (err)
		al_err("TPM SPI read access failed\n");

	return data;
}

static int al_tpm_if_spi_write(
	struct al_tpm_if *tpm_if, unsigned int offset, uint8_t *data, unsigned int data_size)
{
	int err = 0;
	struct al_tpm_if_spi *tpm_if_spi;
	al_assert(tpm_if);
	al_assert(tpm_if->priv);
	tpm_if_spi = tpm_if->priv;

	err = al_spi_claim_bus(
		tpm_if_spi->spi_if, tpm_if_spi->freq, tpm_if_spi->phase,
		tpm_if_spi->polarity, tpm_if_spi->cs);
	if (err) {
		al_err("TPM SPI claim bus failed!\n");
		return err;
	}

	al_memset(tpm_if_spi->TX, 0, TPM_MAX_COMMAND_SIZE);
	tpm_if_spi->TX[0] = 0x00 | (data_size-1); /*0x00 = write | 0 = 1byte*/
    /* Legacy base offset from memory mapped TPM driver
    requires by NationZ, ignored by Infenion*/
	tpm_if_spi->TX[1] = TIS_BASE_OFFSET;
	tpm_if_spi->TX[2] = (offset>>8)  & 0xFF;
	tpm_if_spi->TX[3] = (offset)     & 0xFF;


	err = al_spi_write(
		tpm_if_spi->spi_if ,
		tpm_if_spi->TX,
		4,
		data,
		data_size,
		tpm_if_spi->cs,
		TPM_SPI_TIMEOUT);
	if (err)
		al_err("TPM SPI write access failed\n");

	al_spi_release_bus(tpm_if_spi->spi_if, tpm_if_spi->cs);

	return err;
}

static void al_tpm_if_spi_write1(struct al_tpm_if *tpm_if, unsigned int offset, uint8_t val)
{
	int err = 0;
	err = al_tpm_if_spi_write(tpm_if, offset, &val, 1);
	if (err)
		al_err("TPM SPI access failed\n");

}

static void al_tpm_if_spi_write4(struct al_tpm_if *tpm_if, unsigned int offset, uint32_t val)
{
	int err = 0;
	err = al_tpm_if_spi_write(tpm_if, offset, (uint8_t *)&val, 4);
	if (err)
		al_err("TPM SPI access failed\n");
}

void al_tpm_if_spi_handle_init_ex(struct al_tpm_if *tpm_if, struct al_tpm_if_spi *tpm_if_spi,
	struct al_tpm_if_spi_init_params *init_params)
{
	al_assert(tpm_if);
	al_assert(tpm_if_spi);
	al_assert(init_params);
	al_assert(init_params->spi_if);

	al_memset(tpm_if, 0, sizeof(struct al_tpm_if));
	al_memset(tpm_if_spi, 0, sizeof(struct al_tpm_if_spi));

	tpm_if_spi->spi_if = init_params->spi_if;
	tpm_if_spi->cs = init_params->cs;
	tpm_if_spi->polarity = init_params->polarity;
	tpm_if_spi->phase = init_params->phase;
	tpm_if_spi->freq = init_params->freq;
	tpm_if->priv = tpm_if_spi;

	tpm_if->read = al_tpm_if_spi_read;
	tpm_if->read1 = al_tpm_if_spi_read1;
	tpm_if->read4 = al_tpm_if_spi_read4;
	tpm_if->write = al_tpm_if_spi_write;
	tpm_if->write1 = al_tpm_if_spi_write1;
	tpm_if->write4 = al_tpm_if_spi_write4;
}

void al_tpm_if_spi_handle_init(struct al_tpm_if *tpm_if, struct al_tpm_if_spi *tpm_if_spi,
		struct al_spi_interface	*spi_if, unsigned int cs)
{
	al_assert(tpm_if);
	al_assert(tpm_if_spi);
	al_assert(spi_if);

	al_memset(tpm_if, 0, sizeof(struct al_tpm_if));
	al_memset(tpm_if_spi, 0, sizeof(struct al_tpm_if_spi));

	tpm_if_spi->spi_if = spi_if;
	tpm_if_spi->cs = cs;
	tpm_if_spi->polarity = AL_SPI_POLARITY_INACTIVE_LOW;
	tpm_if_spi->phase = AL_SPI_PHASE_SLAVE_SELECT;
	tpm_if_spi->freq = DEFAULT_FREQ;
	tpm_if->priv = tpm_if_spi;

	tpm_if->read = al_tpm_if_spi_read;
	tpm_if->read1 = al_tpm_if_spi_read1;
	tpm_if->read4 = al_tpm_if_spi_read4;
	tpm_if->write = al_tpm_if_spi_write;
	tpm_if->write1 = al_tpm_if_spi_write1;
	tpm_if->write4 = al_tpm_if_spi_write4;
}
