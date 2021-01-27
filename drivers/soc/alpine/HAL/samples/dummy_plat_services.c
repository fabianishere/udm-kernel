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
 * @brief  dummy platform services implementation
 *
 */

#include <al_hal_common.h>
#include <al_hal_iomap.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "al_hal_sample.h"
static int dump_file_dsc = -1;
static char path[100];

#define IS_FD_SET(fd) ((fd) >= 0)
#define MAX_LINE_SIZE 512
#define OFFLINE_UNSUPPORTED_ERR_MSG "Unsupported offline action\n"
#define OFFLINE_UNSUPPORTED_ERR \
	al_err("%s %s\n", __func__, OFFLINE_UNSUPPORTED_ERR_MSG)

static uint32_t __al_read_val_from_dump_file(uintptr_t reg_addr)
{
	FILE *cmd;
	char result[MAX_LINE_SIZE];
	char cmd_str[256];
	char *val_p;
	char *res;

	sprintf(cmd_str, "grep %08" PRIxPTR " %s", reg_addr, path);
	cmd = popen(cmd_str, "r");
	if (cmd == NULL) {
		perror("popen");
		exit(EXIT_FAILURE);
	}

	res = fgets(result, sizeof(result), cmd);
	pclose(cmd);

	if (res == NULL) {
		al_warn("No info for reg_addr 0x%08" PRIxPTR "\n", reg_addr);
		return 0;
	}

	/* Extract the value */
	val_p =  strtok(result, ": ");
	val_p = strtok(NULL, ": ");

	if (val_p == NULL) {
		al_warn("Wrong format of address and value\n");
		return 0;
	}

	return (uint32_t)strtoul(val_p, NULL, 16);
}

uint8_t al_reg_read8(uint8_t *offset)
{
	uint32_t reg;
	unsigned char  byte_index = (uintptr_t)offset & (uintptr_t)0x3;

	if (IS_FD_SET(dump_file_dsc)) {
		/* file descriptor was initialized,
		 * therefore we should retrieve the value offline */
		reg = __al_read_val_from_dump_file((uintptr_t) ((uintptr_t)offset &
				((uintptr_t)~0x3)));
		return (uint8_t)AL_REG_FIELD_GET(reg,
			(0xff << (byte_index << 3)),
			(byte_index << 3));
	} else {
		al_dbg("read8 from %p\n", offset);
		return 0;
	}
}

uint16_t al_reg_read16(uint16_t *offset)
{
	uint32_t reg;
	unsigned char  half_word_index = (uintptr_t)offset & (uintptr_t)0x3;

	if (IS_FD_SET(dump_file_dsc)) {
		/* file descriptor was initialized,
		 * therefore we should retrieve the value offline */
		reg = __al_read_val_from_dump_file((uintptr_t) ((uintptr_t)offset &
				((uintptr_t)~0x3)));
		return (uint16_t)AL_REG_FIELD_GET(reg,
			(0xffff << (half_word_index << 3)),
			(half_word_index << 3));
	} else {
		al_dbg("read16 from %p\n", offset);
		return 0;
	}
}

uint32_t al_reg_read32(uint32_t *offset)
{
	if (IS_FD_SET(dump_file_dsc)) {
		/* file descriptor was initialized,
		 * therefore we should retrieve the value offline */
		return __al_read_val_from_dump_file((uintptr_t)offset);
	} else {
		al_dbg("read32 from %p\n", offset);
		return 0;
	}
}

uint32_t al_reg_read32_relaxed(uint32_t *offset)
{
	if (IS_FD_SET(dump_file_dsc)) {
		/* file descriptor was initialized,
		 * therefore we should retrieve the value offline */
		return __al_read_val_from_dump_file((uintptr_t)offset);
	} else {
		al_dbg("relaxed read32 from %p\n", offset);
		return 0;
	}
}

uint64_t al_reg_read64(uint64_t *offset)
{
	uint64_t reg;
	uint32_t bottom_bits;
	uint32_t top_bits;

	if (IS_FD_SET(dump_file_dsc)) {
		bottom_bits = al_reg_read32((uint32_t *)offset);
		top_bits = al_reg_read32(((uint32_t *)offset) + 1);
		reg = (uint64_t)((((uint64_t)top_bits) << 32) | (uint64_t)bottom_bits);
		return reg;
	} else {
		al_dbg("read64 from %p\n", offset);
		return 0;
	}
}

void al_reg_write8(uint8_t *offset, uint8_t val)
{
	if (IS_FD_SET(dump_file_dsc))
		OFFLINE_UNSUPPORTED_ERR;
	else
		al_dbg("write8 0x%02x to %p\n", val, offset);
}

void al_reg_write16(uint16_t *offset, uint16_t val)
{
	if (IS_FD_SET(dump_file_dsc))
		OFFLINE_UNSUPPORTED_ERR;
	else
		al_dbg("write16 0x%04x to %p\n", val, offset);
}

void al_reg_write32(uint32_t *offset, uint32_t val)
{
	if (IS_FD_SET(dump_file_dsc))
		OFFLINE_UNSUPPORTED_ERR;
	else
		al_dbg("write32 0x%08x to %p\n", val, offset);
}

void al_reg_write32_relaxed(uint32_t *offset, uint32_t val)
{
	if (IS_FD_SET(dump_file_dsc))
			OFFLINE_UNSUPPORTED_ERR;
	else
		al_dbg("relaxed write32 0x%08x to %p\n", val, offset);
}

void al_reg_write64(uint64_t *offset, uint64_t val)
{
	if (IS_FD_SET(dump_file_dsc))
			OFFLINE_UNSUPPORTED_ERR;
	else
		al_dbg("write64 0x%08x%08x to %p\n", AL_UINT64_HIGH(val),
				AL_UINT64_LOW(val), offset);
}

void al_data_memory_barrier(void) {};

void al_local_data_memory_barrier(void) {};

void al_smp_data_memory_barrier(void) {};

void al_udelay(unsigned long loops __attribute__((__unused__))) {};

void al_msleep(unsigned long loops __attribute__((__unused__))) {};

/************ offline services ****************/
al_bool al_reg_dump_reg_is_in_dump(uintptr_t reg_addr)
{
	FILE *cmd;
	char result[MAX_LINE_SIZE];
	char cmd_str[256];
	char *res;

	sprintf(cmd_str, "grep %08" PRIxPTR " %s", reg_addr, path);
	cmd = popen(cmd_str, "r");

	if (cmd == NULL) {
		perror("popen");
		exit(EXIT_FAILURE);
	}

	res = fgets(result, sizeof(result), cmd);

	pclose(cmd);

	if (res == NULL)
		return AL_FALSE;

	return AL_TRUE;
}

int al_reg_dump_fd_init(const char *dump_file_path)
{
	strncpy(path, dump_file_path, sizeof(path));
	path[sizeof(path) - 1] = '\0';

	dump_file_dsc = open(path, O_RDONLY);
	if (dump_file_dsc < 0) {
		al_err("Error: there was a problem opening the dump file at %s\n", path);
		return dump_file_dsc;
	}
	return 0;
}
