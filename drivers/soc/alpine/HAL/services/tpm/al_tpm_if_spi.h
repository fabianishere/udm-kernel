/*
 * Copyright 2018, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#ifndef __TPM_IF_SPI_H__
#define __TPM_IF_SPI_H__

#include "al_tpm_if.h"
#include "al_hal_spi.h"

struct al_tpm_if_spi_init_params {
	struct al_spi_interface	*spi_if;
	unsigned int cs;
	enum al_spi_polarity_t polarity;
	enum al_spi_phase_t phase;
	unsigned int freq;
};

struct al_tpm_if_spi {
	unsigned char TX[TPM_MAX_COMMAND_SIZE];
	unsigned char RX[TPM_MAX_COMMAND_SIZE];
	struct al_spi_interface *spi_if;
	unsigned int cs;
	enum al_spi_polarity_t polarity;
	enum al_spi_phase_t phase;
	unsigned int freq;
};

/**
 * TPM SPI interface handle initialization using extended initialization structure
 *
 * @param	tpm_if
 *		An uninitialized TPM interface handle
 * @param	tpm_if_spi
 *		TPM SPI interface
 * @param	init_params
 *		Basic parameters for initialization
 */
void al_tpm_if_spi_handle_init_ex(
	struct al_tpm_if			*tpm_if,
	struct al_tpm_if_spi			*tpm_if_spi,
	struct al_tpm_if_spi_init_params	*init_params);

/**
 * TPM SPI interface handle initialization using initialization structure
 *
 * @param	tpm_if
 *		An uninitialized TPM interface handle
 * @param	tpm_if_spi
 *		TPM SPI interface
 * @param	spi_if
 *		SPI interface
 * @param	cs
 *		Chip Select
 */
void al_tpm_if_spi_handle_init(
	struct al_tpm_if	*tpm_if,
	struct al_tpm_if_spi	*tpm_if_spi,
	struct al_spi_interface	*spi_if,
	unsigned int		cs);
#endif
