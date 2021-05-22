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

/**
 * @defgroup group_eth_field_api API
 * Ethernet Controller generic field API
 * @ingroup group_eth
 * @{
 * @file   al_hal_eth_field.h
 *
 * @brief Header file for control parameters for the generic field and field extractors
 *        in the Ethernet Datapath for Advanced Ethernet port.
 *
 */

#ifndef __AL_HAL_ETH_FIELD_H__
#define __AL_HAL_ETH_FIELD_H__

#include "al_hal_common.h"

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

/** Fields section */
struct al_eth_field_section {
	/** Offset in bits of the section in full vector */
	unsigned int offset;
	/** Size in bits of the section in full vector */
	unsigned int size;
};

/** Field */
struct al_eth_field {
	/** Section in full vector */
	struct al_eth_field_section *section;
	/** Offset in bits of the field inside section */
	unsigned int offset;
	/** Size in bits of the field inside section */
	unsigned int size;
};

/**
 * Field extractor
 * Set of features may vary between implementing components
 */
struct al_eth_field_extractor {
	/**
	 * Base vector field reference,
	 * if NULL consider as whole vector
	 */
	struct al_eth_field *field;
	/** Offset in bits inside the field */
	unsigned int offset;
	/**
	 * Size in bits inside the field,
	 * 0 for taking from offset to end of field
	 */
	unsigned int size;
	/** Merge offset */
	unsigned int merge_offset;
	/**
	 * Use indirection table
	 * Extracted value will be used as a table index
	 */
	al_bool use_table;
};

/**
 * Calculate vector field extractor offset & size in bits
 *
 * @param fe pointer to field extractor parameters
 * @param offset calculated offset in bits
 * @param size calculated size in bits
 */
void al_eth_field_extractor_calc(const struct al_eth_field_extractor *fe,
	unsigned int *offset, unsigned int *size);

/**
 * Calculate vector field extractor offset & size in bytes
 *
 * @param fe pointer to field extractor parameters
 * @param offset calculated offset in bytes
 * @param size calculated size in bytes
 */
void al_eth_field_extractor_calc_byte(const struct al_eth_field_extractor *fe,
	unsigned int *offset, unsigned int *size);

#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */
#endif /* __AL_HAL_ETH_FIELD_H__ */
/** @} */
