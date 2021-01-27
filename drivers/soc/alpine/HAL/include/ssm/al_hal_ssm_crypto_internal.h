/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#ifndef __AL_HAL_SSM_CRYPTO_INTERNAL_H__
#define __AL_HAL_SSM_CRYPTO_INTERNAL_H__

/**
 *  Crypto regs base address get
 *
 *  Calculates and return crypto function address based on rev_id
 *
 * @param	app_regs
 *		SSMAE application registers
 *
 * @param rev_id
 *		Adapter revision ID
 *
 * @return Crypto registers structure
 */
struct crypto_regs __iomem *al_ssm_crypto_regs_get(void __iomem *app_regs, uint8_t rev_id);

#endif
