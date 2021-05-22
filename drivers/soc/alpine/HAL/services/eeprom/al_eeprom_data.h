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
 * This file includes EEPROM data layout
 */
#ifndef __AL_EEPROM_DATA_H__
#define __AL_EEPROM_DATA_H__

#include "al_hal_plat_types.h"

/* Offset length in bytes */
#define AL_EEPROM_OFFSET_LEN				2

/* Offset of board data inside EEPROM device */
#define AL_EEPROM_BOARD_DATA_OFFSET			0x600

/* Magic number for validating board data information */
#define AL_EEPROM_BOARD_DATA_MN				0xCC

#define AL_EEPROM_BOARD_DATA_PARAM_OFFSET(name)	\
		((uint16_t)(uintptr_t)(&(((struct al_eeprom_board_data *)(uintptr_t) \
				AL_EEPROM_BOARD_DATA_OFFSET)->name)))

#define AL_EEPROM_BOARD_DATA_PARAM_SIZE(name)	\
		(sizeof(((struct al_eeprom_board_data *)(uintptr_t) \
				AL_EEPROM_BOARD_DATA_OFFSET)->name))

struct al_eeprom_board_data {
	uint8_t magic;
	uint8_t customer_rev_id;
	uint8_t mac_addr[4][6];
	uint8_t pcb_rev_id[7];
	uint8_t sn[16];
	uint8_t pn[15];
	uint8_t mac_addr_4_7[4][6];
};

#endif /* __AL_EEPROM_DATA_H__ */
