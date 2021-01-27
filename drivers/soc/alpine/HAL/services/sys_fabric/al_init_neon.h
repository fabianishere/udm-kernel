/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#ifndef __AL_INIT_NEON_H__
#define __AL_INIT_NEON_H__

/**
 * Enable ARM NEON support
 *
 * This function must be called from secure context and prior to any NEON instruction
 *
 * @param	neon_regs_init
 *		Initialize NEON registers (d0-d31) to zero
 */
void al_neon_enable(al_bool neon_regs_init);

#endif
