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

#include "al_mod_hal_common.h"
#include "al_mod_hal_iomap_dynamic.h"

/******************************************************************************
 ******************************************************************************/
__attribute__((weak)) al_mod_phys_addr_t al_mod_iomap_v1_offset_idx_sub_idx_get(
	enum al_mod_iomap_type type,
	unsigned int idx,
	unsigned int sub_idx);

al_mod_phys_addr_t al_mod_iomap_v1_offset_idx_sub_idx_get(
	enum al_mod_iomap_type type __attribute__((unused)),
	unsigned int idx __attribute__((unused)),
	unsigned int sub_idx __attribute__((unused)))
{
	al_mod_err("Alpine V1 I/O offsets are unsupported\n");

	return 0;
}

/******************************************************************************
 ******************************************************************************/
__attribute__((weak)) al_mod_phys_addr_t al_mod_iomap_v2_offset_idx_sub_idx_get(
	enum al_mod_iomap_type type,
	unsigned int idx,
	unsigned int sub_idx);

al_mod_phys_addr_t al_mod_iomap_v2_offset_idx_sub_idx_get(
	enum al_mod_iomap_type type __attribute__((unused)),
	unsigned int idx __attribute__((unused)),
	unsigned int sub_idx __attribute__((unused)))
{
	al_mod_err("Alpine V2 I/O offsets are unsupported\n");

	return 0;
}

/******************************************************************************
 ******************************************************************************/
__attribute__((weak)) al_mod_phys_addr_t al_mod_iomap_v3_offset_idx_sub_idx_get(
	enum al_mod_iomap_type type,
	unsigned int idx,
	unsigned int sub_idx);

al_mod_phys_addr_t al_mod_iomap_v3_offset_idx_sub_idx_get(
	enum al_mod_iomap_type type __attribute__((unused)),
	unsigned int idx __attribute__((unused)),
	unsigned int sub_idx __attribute__((unused)))
{
	al_mod_err("Alpine V3 I/O offsets are unsupported\n");

	return 0;
}

/******************************************************************************
 ******************************************************************************/
al_mod_phys_addr_t al_mod_iomap_offset_idx_sub_idx_get(
	unsigned int dev_id,
	enum al_mod_iomap_type type,
	unsigned int idx,
	unsigned int sub_idx)
{
	switch (dev_id) {
	case AL_DEV_ID_ALPINE_V1:
		return al_mod_iomap_v1_offset_idx_sub_idx_get(type, idx, sub_idx);
	case AL_DEV_ID_ALPINE_V2:
		return al_mod_iomap_v2_offset_idx_sub_idx_get(type, idx, sub_idx);
	case AL_DEV_ID_ALPINE_V3:
		return al_mod_iomap_v3_offset_idx_sub_idx_get(type, idx, sub_idx);
	default:
		al_mod_err("Unsupported platform\n");
		al_mod_assert(0);
	}

	return 0;
}

