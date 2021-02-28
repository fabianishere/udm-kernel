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
#ifndef __AL_TRACE_H__
#define __AL_TRACE_H__

#include "al_hal_plat_types.h"

/**
 * Trace control block - located at the beginning of the actual trace buffer
 */
struct al_trace_ctrl {
	uint64_t	start;			/* 64 bit address of start of the log buffer */
	uint64_t	end;			/* 64 bit address of the end of the log buffer */
	uint32_t	head;			/* index where producer doing to write next char */
	uint32_t	tail;			/* index where the consumer stopped consuming */
	uint32_t	reset_request;		/* log reset request flag */
	uint32_t	wrap_around;		/* log wrap around flag */
	uint32_t	mn;			/* Magic number */
	char		reserved[28];
} __attribute__((__packed__));


/**
 * Trace handle - to be used as context by the various functions
 */
struct al_trace_handle {
	char				*base;
	unsigned int			size;
	al_bool				en;
	unsigned int			(*time_stamp_str_get)(char *, unsigned int);

	char				*head;
	char				*start;
	char				*end;
	int				add_timestamp;
	volatile struct al_trace_ctrl	*ctrl;
};

/**
 * Trace initialization
 *
 * @param	trace
 *		Trace uninitialized handle
 * @param	base
 *		Trace buffer base
 * @param	size
 *		Trace buffer size
 * @param	sync
 *		Sync to existing trace buffer (if exists)
 * @param	time_stamp_str_get
 *		Function for getting current time stamp string
 */
void al_trace_init(
	struct al_trace_handle	*trace,
	void			*base,
	unsigned int		size,
	al_bool			sync,
	unsigned int		(*time_stamp_str_get)(char *, unsigned int));

/**
 * Trace append - append a trace to another trace
 *
 * @param	trace
 *		The target trace
 * @param	addition
 *		The trace whos contents are to be appended to 'trace'
 */
void al_trace_append(
	struct al_trace_handle	*trace,
	struct al_trace_handle	*addition);

/**
 * Trace enable/disable
 *
 * @param	trace
 *		Trace handle
 * @param	en
 *		AL_TRUE - enable, AL_FALSE - disable
 */
void al_trace_en(
	struct al_trace_handle	*trace,
	al_bool			en);

/**
 * Trace add character
 *
 * @param	trace
 *		Trace handle
 * @param	c
 *		Character to be added
 */
void al_trace_putchar(
	struct al_trace_handle	*trace,
	int			c);

/**
 * Trace dumping
 *
 * @param	trace
 *		Trace handle
 * @param	num_of_chars
 *		Number of characters to be dumped from the end, or 0 to dump the entire contents
 */
void al_trace_dump(
	struct al_trace_handle *trace,
	unsigned int		num_of_chars);

#endif

