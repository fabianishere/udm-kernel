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
 * @defgroup group_eth_alu_api API
 * Ethernet Controller generic ALU API
 * @ingroup group_eth
 * @{
 * @file   al_mod_hal_eth_alu.h
 *
 * @brief Header file for control parameters for the generic ALU unit in the Ethernet Datapath for Advanced Ethernet port.
 *
 */

#ifndef __AL_HAL_ETH_ALU_H__
#define __AL_HAL_ETH_ALU_H__

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

/** Ethernet ALU input selector  */
enum al_mod_eth_alu_in {
	/** Input 0 */
	ETH_ALU_IN_0,
	/** Input 1 */
	ETH_ALU_IN_1,
	/** Input 2 */
	ETH_ALU_IN_2,
	/** Input 3 */
	ETH_ALU_IN_3,
	ETH_ALU_IN_MAX
};

/** Ethernet ALU value  */
enum al_mod_eth_alu_val {
	/** ALU value 0 */
	ETH_ALU_VAL_0,
	/** ALU value 1 */
	ETH_ALU_VAL_1,
	ETH_ALU_VAL_MAX
};

/** Ethernet ALU operation selector  */
enum al_mod_eth_alu_op {
	/** Operation on the first 2 inputs */
	ETH_ALU_OP_0_1,
	/** Operation on the last 2 inputs */
	ETH_ALU_OP_2_3,
	/** Operation on the outputs of previous two operations */
	ETH_ALU_OP_FINAL,
	ETH_ALU_OP_MAX
};

/** Ethernet ALU operation */
enum AL_ETH_ALU_OPCODE {
	/** A */
	AL_ALU_FWD_A = 0,
	/** A + B */
	AL_ALU_ARITHMETIC_ADD = 1,
	/** A - B */
	AL_ALU_ARITHMETIC_SUBTRACT = 2,
	/** A & B */
	AL_ALU_BITWISE_AND = 3,
	/** A | B */
	AL_ALU_BITWISE_OR = 4,
	/** A >> B */
	AL_ALU_SHIFT_RIGHT_A_BY_B = 5,
	/** A << B */
	AL_ALU_SHIFT_LEFT_A_BY_B = 6,
	/** A XOR B */
	AL_ALU_BITWISE_XOR = 7,
	/** Clear least significant bits of A (A[b-1:0]=0) */
	AL_ALU_MASK_LSBS = 8,
	/** Clear most significant bits of A (A[N-1:N-b]=0) */
	AL_ALU_MASK_MSBS = 9,
	/** ~A */
	AL_ALU_FWD_INV_A = 16,
	/** (~A) + B */
	AL_ALU_ARITHMETIC_ADD_INV_A_AND_B = 17,
	/** (~A) - B */
	AL_ALU_ARITHMETIC_SUBTRACT_INV_A_AND_B = 18,
	/** (~A) & B */
	AL_ALU_BITWISE_AND_INV_A_AND_B = 19,
	/** (~A) | B */
	AL_ALU_BITWISE_OR_INV_A_AND_B = 20,
	/** (~A) >> B */
	AL_ALU_SHIFT_RIGHT_INV_A_BY_B = 21,
	/** (~A) << B */
	AL_ALU_SHIFT_LEFT_INV_A_BY_B = 22,
	/** (~A) XOR B */
	AL_ALU_BITWISE_XOR_INV_A_AND_B = 23,
	/** A + (~B) */
	AL_ALU_ARITHMETIC_ADD_A_AND_INV_B = 33,
	/** A - (~B) */
	AL_ALU_ARITHMETIC_SUBTRACT_A_AND_INV_B = 34,
	/** A & (~B) */
	AL_ALU_BITWISE_AND_A_AND_INV_B = 35,
	/** A | (~B) */
	AL_ALU_BITWISE_OR_A_AND_INV_B = 36,
	/** A >> (~B) */
	AL_ALU_SHIFT_RIGHT_A_BY_INV_B = 37,
	/** A << (~B) */
	AL_ALU_SHIFT_LEFT_A_BY_INV_B = 38,
	/** A XOR (~B) */
	AL_ALU_BITWISE_XOR_A_AND_INV_B = 39,
	/** (~A) + (~B) */
	AL_ALU_ARITHMETIC_ADD_INV_A_AND_INV_B = 49,
	/** (~A) - (~B) */
	AL_ALU_ARITHMETIC_SUBTRACT_INV_A_AND = 50,
	/** (~A) & (~B) */
	AL_ALU_BITWISE_AND_INV_A_AND_INV_B = 51,
	/** (~A) | (~B) */
	AL_ALU_BITWISE_OR_INV_A_AND_INV_B = 52,
	/** (~A) >> (~B) */
	AL_ALU_SHIFT_RIGHT_INV_A_BY_INV_B = 53,
	/** (~A) << (~B) */
	AL_ALU_SHIFT_LEFT_INV_A_BY_INV_B = 54,
	/** (~A) XOR (~B) */
	AL_ALU_BITWISE_XOR_INV_A_AND_INV_B = 55,
};

/** Single ALU */
struct al_mod_eth_alu {
	/** Inputs selector */
	uint8_t input[ETH_ALU_IN_MAX];

	/** Where value can be selected as one of the inputs */
	uint32_t val[ETH_ALU_VAL_MAX];

	/** Operations on inputs */
	enum AL_ETH_ALU_OPCODE op[ETH_ALU_OP_MAX];
};

/** Triple ALU - ALU select */
enum al_mod_eth_alu_x3_alu {
	/** First stage - ALU 0 */
	ETH_ALU_X3_ALU_STAGE_1_0,
	/** First stage - ALU 1 */
	ETH_ALU_X3_ALU_STAGE_1_1,
	/** Second stage ALU */
	ETH_ALU_X3_ALU_STAGE_2,
	ETH_ALU_X3_ALU_MAX
};

/** Triple ALU */
struct al_mod_eth_alu_x3 {
	struct al_mod_eth_alu alu[ETH_ALU_X3_ALU_MAX];
};

#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */
#endif /* __AL_HAL_ETH_ALU_H__ */
/** @} end of Ethernet group */
