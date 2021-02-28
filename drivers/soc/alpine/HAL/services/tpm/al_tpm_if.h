/*
 * Copyright 2018, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */


#ifndef __TPM_IF_H__
#define __TPM_IF_H__

#include "al_hal_plat_types.h"

#define TPM_MAX_COMMAND_SIZE		64
#define TPM_MAX_RESP_SIZE			4096

struct al_tpm_if {
	int (*read)(struct al_tpm_if *tpm_if, unsigned int offset, uint8_t *p, unsigned int size);
	uint8_t (*read1)(struct al_tpm_if *tpm_if, unsigned int offset);
	uint32_t (*read4)(struct al_tpm_if *tpm_if, unsigned int offset);
	int (*write)(struct al_tpm_if *tpm_if, unsigned int offset, uint8_t *p, unsigned int size);
	void (*write1)(struct al_tpm_if *tpm_if, unsigned int offset, uint8_t val);
	void (*write4)(struct al_tpm_if *tpm_if, unsigned int offset, uint32_t val);
	void *priv;
};

#endif
