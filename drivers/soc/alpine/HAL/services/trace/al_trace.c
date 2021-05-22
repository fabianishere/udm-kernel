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
#include "al_trace.h"
#include "al_hal_plat_services.h"

#define AL_TRACE_DUMP_LINE_LEN_MAX	80

#define AL_TRACE_CTRL_MN		0x57ACE123

#define AL_TRACE_TIME_STR_LEN		14

static void al_trace_sync(
	struct al_trace_handle *trace)
{
	trace->head = (char *)(uintptr_t)(trace->start + trace->ctrl->head);
	trace->add_timestamp = 1;
}

static void al_trace_reset(
	struct al_trace_handle *trace,
	al_bool			first)
{
	trace->ctrl->head = 0;
	trace->ctrl->tail = 0;
	trace->ctrl->wrap_around = 0;
	trace->ctrl->reset_request = 0;
	if (first) {
		trace->ctrl->start = (uintptr_t)trace->base + sizeof(struct al_trace_ctrl);
		trace->ctrl->end = (uintptr_t)trace->base + trace->size;
		trace->ctrl->mn = AL_TRACE_CTRL_MN;
	}

	al_trace_sync(trace);
}

static inline void al_trace_increment(
	struct al_trace_handle *trace)
{
	++(trace->head);

	if (trace->head == trace->end) {
		trace->head = trace->start;
		trace->ctrl->head = 0;
		trace->ctrl->wrap_around = 1;
	} else {
		trace->ctrl->head++;
	}
}

static void al_trace_add_timestamp(
	struct al_trace_handle *trace)
{
	char timestr[AL_TRACE_TIME_STR_LEN];
	unsigned int i;
	unsigned int time_stamp_len;

	if (!trace->time_stamp_str_get)
		return;

	time_stamp_len = trace->time_stamp_str_get(timestr, AL_TRACE_TIME_STR_LEN);

	for (i = 0; i < time_stamp_len; i++) {
		*(trace->head) = timestr[i];
		al_trace_increment(trace);
	}
}

void al_trace_init(
	struct al_trace_handle	*trace,
	void			*base,
	unsigned int		size,
	al_bool			sync,
	unsigned int		(*time_stamp_str_get)(char *, unsigned int))
{
	trace->base = (char *)base;
	trace->size = size;
	trace->start = trace->base + sizeof(struct al_trace_ctrl);
	trace->end = trace->base + trace->size;
	trace->ctrl = (struct al_trace_ctrl *)trace->base;
	trace->time_stamp_str_get = time_stamp_str_get;
	trace->en = 1;

	if (sync && (trace->ctrl->mn == AL_TRACE_CTRL_MN))
		al_trace_sync(trace);
	else
		al_trace_reset(trace, AL_TRUE);
}

void al_trace_append(
	struct al_trace_handle	*trace,
	struct al_trace_handle	*addition)
{
	char *ptr;
	char *end;

	ptr = addition->ctrl->wrap_around ?
		((addition->head == addition->end) ? addition->start : addition->head + 1) :
		addition->start;
	end = addition->head;

	while (ptr != end) {
		*(trace->head) = *ptr;
		al_trace_increment(trace);
		++ptr;
		if (ptr == addition->end)
			ptr = addition->start;
	}
}

void al_trace_en(
	struct al_trace_handle	*trace,
	al_bool			en)
{
	trace->en = en;
}

void al_trace_putchar(
	struct al_trace_handle	*trace,
	int			c)
{
	if (!trace->en)
		return;

	if (trace->ctrl->reset_request)
		al_trace_reset(trace, AL_FALSE);

	if (!c)
		return;

	if (trace->add_timestamp) {
		al_trace_add_timestamp(trace);
		trace->add_timestamp = 0;
	}

	*(trace->head) = (unsigned char)c;
	al_trace_increment(trace);

	if (c == '\n')
		trace->add_timestamp = 1;
}

void al_trace_dump(
	struct al_trace_handle *trace,
	unsigned int		num_of_chars)
{
	char *ptr;
	char *end;
	char line[AL_TRACE_DUMP_LINE_LEN_MAX + 1];
	unsigned int line_idx = 0;
	unsigned int en_orig = trace->en;

	trace->en = 0;

	al_print("********************************************************\n");
	al_print("* TRACE DUMP: BEGIN\n");
	al_print("********************************************************\n");
	al_print("- base = %p\n", trace->base);
	al_print("- size = %x\n", trace->size);
	al_print("- en = %d\n", en_orig);
	al_print("- head = %p\n", trace->head);
	al_print("- start = %p\n", trace->start);
	al_print("- end = %p\n", trace->end);
	al_print("- add_timestamp = %d\n", trace->add_timestamp);
	al_print("- ctrl = %p\n", trace->ctrl);
	al_print("- ctrl->head = %x\n", trace->ctrl->head);
	al_print("- ctrl->tail = %x\n", trace->ctrl->tail);
	al_print("- ctrl->reset_request = %d\n", trace->ctrl->reset_request);
	al_print("- ctrl->wrap_around = %d\n", trace->ctrl->wrap_around);
	al_print("********************************************************\n");

	ptr = (trace->ctrl->wrap_around) ?
		((trace->head == trace->end) ? trace->start : (trace->head + 1)) :
		trace->start;
	end = trace->head;

	if (num_of_chars && (num_of_chars < (unsigned int)(trace->end - trace->start))) {
		if (trace->ctrl->wrap_around) {
			if ((unsigned int)(end - trace->start) > num_of_chars)
				ptr = end - num_of_chars;
			else
				ptr = trace->end - (num_of_chars - (ptr - trace->start));
		} else {
			if ((unsigned int)(end - ptr) > num_of_chars)
				ptr = end - num_of_chars;
		}
	}

	while (ptr != end) {
		int c = *ptr;

		if (c == '\n') {
			line[line_idx] = 0;
			line_idx = 0;
			al_print("%s\n", line);
		} else if (line_idx < AL_TRACE_DUMP_LINE_LEN_MAX) {
			line[line_idx] = c;
			line_idx++;
		}
		++ptr;
		if (ptr == trace->end)
			ptr = trace->start;
	}
	line[line_idx] = 0;
	al_print("%s", line);
	al_print("\n********************************************************\n");

	trace->en = en_orig;
}

