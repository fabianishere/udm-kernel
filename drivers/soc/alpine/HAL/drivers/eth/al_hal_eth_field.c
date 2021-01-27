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

#include "al_hal_common.h"
#include "al_hal_eth_field.h"

/* Number of bits in 1 byte */
#define BITS_TO_BYTE					8

void al_eth_field_extractor_calc(const struct al_eth_field_extractor *fe,
	unsigned int *offset_p, unsigned int *size_p)
{
	unsigned int offset = 0;
	unsigned int size = 0;

	al_assert(fe);

	offset = fe->offset;
	size = fe->size;

	if (fe->field) {
		if (!size)
			size = fe->field->size - offset;

		offset += fe->field->offset;

		if (fe->field->section)
			offset += fe->field->section->offset;
	}

	if (offset_p)
		*offset_p = offset;

	if (size_p)
		*size_p = size;
}

void al_eth_field_extractor_calc_byte(const struct al_eth_field_extractor *fe,
	unsigned int *offset, unsigned int *size)
{
	al_eth_field_extractor_calc(fe, offset, size);

	if (offset) {
		al_assert_msg(((*offset) % BITS_TO_BYTE == 0),
			"Field extractor offset is not byte aligned\n");

		*offset /= BITS_TO_BYTE;
	}

	if (size) {
		al_assert_msg(((*size) % BITS_TO_BYTE == 0),
			"Field extractor size is not byte aligned\n");

		*size /= BITS_TO_BYTE;
	}
}
