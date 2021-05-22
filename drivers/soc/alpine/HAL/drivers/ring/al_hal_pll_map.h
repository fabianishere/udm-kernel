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
 * @defgroup grouppll PLL control
 * @ingroup group_ring
 *
 *  @{
 * @file   al_hal_pll_map.h
 *
 * @brief Header file for the PLL HAL driver frequency mapping
 *
 */

#ifndef __AL_HAL_PLL_MAP_H__
#define __AL_HAL_PLL_MAP_H__

#include "al_hal_pll.h"

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

struct al_pll_freq_map_ent {
	enum al_pll_freq	freq;
	uint32_t		freq_val;
	union {
		struct {
			uint32_t	nf;
			uint32_t	nr;
			uint32_t	od;
			uint32_t	bwadj;
		} v1;
#if (!defined(AL_DEV_ID) || (AL_DEV_ID > AL_DEV_ID_ALPINE_V2))
		struct {
			uint32_t	ref_clk_div;
			uint32_t	fb_pre_scalar;
			uint32_t	fb_post_scalar;
			uint32_t	out_div;
		} v2;
		struct {
			uint32_t	ref_div;
			uint32_t	fb_div;
			uint32_t	frac;
			uint32_t	post_div_1;
			uint32_t	post_div_2;
		} v3;
#endif
	} params;
};

static const struct al_pll_freq_map_ent al_pll_freq_map_v1_100[] = {
	/* freq,		freq_val,			nf,	nr,	od,	bwadj */
	{ AL_PLL_FREQ_533_333,	533333,		.params.v1 = { 31,	0,	5,	15 } },
	{ AL_PLL_FREQ_800_000,	800000,		.params.v1 = { 31,	0,	3,	15 } },
	{ AL_PLL_FREQ_1000_000,	1000000,	.params.v1 = { 19,	0,	1,	9 } },
	{ AL_PLL_FREQ_1200_000,	1200000,	.params.v1 = { 23,	0,	1,	11 } },
	{ AL_PLL_FREQ_1500_000,	1500000,	.params.v1 = { 29,	0,	1,	14 } },
	{ AL_PLL_FREQ_1600_000,	1600000,	.params.v1 = { 31,	0,	1,	15 } },
	{ AL_PLL_FREQ_1700_000,	1700000,	.params.v1 = { 33,	0,	1,	16 } },
	{ AL_PLL_FREQ_1800_000,	1800000,	.params.v1 = { 35,	0,	1,	17 } },
	{ AL_PLL_FREQ_1900_000,	1900000,	.params.v1 = { 37,	0,	1,	18 } },
	{ AL_PLL_FREQ_2000_000,	2000000,	.params.v1 = { 19,	0,	0,	9 } },
	{ AL_PLL_FREQ_2100_000,	2100000,	.params.v1 = { 20,	0,	0,	9 } },
	{ AL_PLL_FREQ_2200_000,	2200000,	.params.v1 = { 21,	0,	0,	10 } },
	{ AL_PLL_FREQ_2300_000,	2300000,	.params.v1 = { 22,	0,	0,	10 } },
	{ AL_PLL_FREQ_2400_000,	2400000,	.params.v1 = { 23,	0,	0,	11 } },
	{ AL_PLL_FREQ_2500_000,	2500000,	.params.v1 = { 24,	0,	0,	11 } },
	{ AL_PLL_FREQ_2600_000,	2600000,	.params.v1 = { 25,	0,	0,	12 } },
	{ AL_PLL_FREQ_2700_000,	2700000,	.params.v1 = { 26,	0,	0,	12 } },
	{ AL_PLL_FREQ_2800_000,	2800000,	.params.v1 = { 27,	0,	0,	13 } },
	{ AL_PLL_FREQ_3000_000,	3000000,	.params.v1 = { 29,	0,	0,	14 } },
	{ AL_PLL_FREQ_3200_000,	3200000,	.params.v1 = { 31,	0,	0,	15 } },
};

static const struct al_pll_freq_map_ent al_pll_freq_map_v1_125[] = {
	/* freq,		freq_val,			nf,	nr,	od,	bwadj */
	{ AL_PLL_FREQ_1250_000,	1250000,	.params.v1 = { 9,	0,	0,	4 } },
	{ AL_PLL_FREQ_1500_000,	1500000,	.params.v1 = { 11,	0,	0,	5 } },
	{ AL_PLL_FREQ_1625_000,	1625000,	.params.v1 = { 12,	0,	0,	6 } },
	{ AL_PLL_FREQ_1750_000,	1750000,	.params.v1 = { 13,	0,	0,	6 } },
	{ AL_PLL_FREQ_2000_000,	2000000,	.params.v1 = { 15,	0,	0,	7 } },
	{ AL_PLL_FREQ_2125_000,	2125000,	.params.v1 = { 16,	0,	0,	8 } },
	{ AL_PLL_FREQ_2250_000,	2250000,	.params.v1 = { 17,	0,	0,	8 } },
	{ AL_PLL_FREQ_2375_000,	2375000,	.params.v1 = { 18,	0,	0,	9 } },
	{ AL_PLL_FREQ_2500_000,	2500000,	.params.v1 = { 19,	0,	0,	9 } },
	{ AL_PLL_FREQ_2625_000,	2625000,	.params.v1 = { 20,	0,	0,	10 } },
	{ AL_PLL_FREQ_2750_000,	2750000,	.params.v1 = { 21,	0,	0,	10 } },
	{ AL_PLL_FREQ_2875_000,	2875000,	.params.v1 = { 22,	0,	0,	11 } },
	{ AL_PLL_FREQ_3000_000,	3000000,	.params.v1 = { 23,	0,	0,	11 } },
	{ AL_PLL_FREQ_533_333,	533333,		.params.v1 = { 127,	14,	1,	63 } },
	{ AL_PLL_FREQ_666_666,	666666,		.params.v1 = { 63,	2,	3,	31 } },
	{ AL_PLL_FREQ_916_666,	916666,		.params.v1 = { 43,	2,	1,	21 } },
	{ AL_PLL_FREQ_933_333,	933333,		.params.v1 = { 223,	14,	1,	111 } },
	{ AL_PLL_FREQ_1062_500,	1062500,	.params.v1 = { 84,	4,	1,	42 } },
	{ AL_PLL_FREQ_800_000,	800000,		.params.v1 = { 63,	4,	1,	31 } },
};

static const struct al_pll_freq_map_ent al_pll_freq_map_v1_25[] = {
	/* freq,		freq_val,			nf,	nr,	od,	bwadj */
	{ AL_PLL_FREQ_1250_000,	1250000,	.params.v1 = { 49,	0,	0,	24 } },
	{ AL_PLL_FREQ_1500_000,	1500000,	.params.v1 = { 59,	0,	0,	29 } },
	{ AL_PLL_FREQ_1625_000,	1625000,	.params.v1 = { 64,	0,	0,	32 } },
	{ AL_PLL_FREQ_1750_000,	1750000,	.params.v1 = { 69,	0,	0,	34 } },
	{ AL_PLL_FREQ_2000_000,	2000000,	.params.v1 = { 79,	0,	0,	39 } },
	{ AL_PLL_FREQ_2125_000,	2125000,	.params.v1 = { 84,	0,	0,	42 } },
	{ AL_PLL_FREQ_2250_000,	2250000,	.params.v1 = { 89,	0,	0,	44 } },
	{ AL_PLL_FREQ_2375_000,	2375000,	.params.v1 = { 94,	0,	0,	47 } },
	{ AL_PLL_FREQ_2500_000,	2500000,	.params.v1 = { 99,	0,	0,	49 } },
	{ AL_PLL_FREQ_2625_000,	2625000,	.params.v1 = { 104,	0,	0,	52 } },
	{ AL_PLL_FREQ_2750_000,	2750000,	.params.v1 = { 109,	0,	0,	54 } },
	{ AL_PLL_FREQ_2875_000,	2875000,	.params.v1 = { 114,	0,	0,	57 } },
	{ AL_PLL_FREQ_3000_000,	3000000,	.params.v1 = { 119,	0,	0,	59 } },
	{ AL_PLL_FREQ_533_333,	533333,		.params.v1 = { 127,	2,	1,	63 } },
	{ AL_PLL_FREQ_666_666,	666666,		.params.v1 = { 159,	2,	1,	79 } },
	{ AL_PLL_FREQ_916_666,	916666,		.params.v1 = { 219,	2,	1,	109 } },
	{ AL_PLL_FREQ_933_333,	933333,		.params.v1 = { 223,	2,	1,	111 } },
	{ AL_PLL_FREQ_1062_500,	1062500,	.params.v1 = { 84,	0,	1,	42 } },
	{ AL_PLL_FREQ_800_000,	800000,		.params.v1 = { 63,	0,	1,	31 } },
};
#if (!defined(AL_DEV_ID) || (AL_DEV_ID > AL_DEV_ID_ALPINE_V2))
static const struct al_pll_freq_map_ent al_pll_freq_map_v2_100[] = {
	/* freq,		freq_val,		       ref_div, pre,    post,   out_divj */
	{ AL_PLL_FREQ_500_000,	500000,		.params.v2 = { 1,	2,	25,	10, } },
	{ AL_PLL_FREQ_533_333,	533333,		.params.v2 = { 1,	2,	24,	9, } },
	{ AL_PLL_FREQ_666_666,	666666,		.params.v2 = { 1,	3,	20,	9, } },
	{ AL_PLL_FREQ_800_000,	800000,		.params.v2 = { 1,	2,	24,	6, } },
	{ AL_PLL_FREQ_900_000,	900000,		.params.v2 = { 1,	2,	27,	6, } },
	{ AL_PLL_FREQ_933_333,	933333,		.params.v2 = { 1,	2,	28,	6, } },
	{ AL_PLL_FREQ_1000_000,	1000000,	.params.v2 = { 1,	2,	25,	5, } },
	{ AL_PLL_FREQ_1050_000,	1050000,	.params.v2 = { 1,	2,	21,	4, } },
	{ AL_PLL_FREQ_1066_666,	1066666,	.params.v2 = { 1,	2,	26,	5, } },
	{ AL_PLL_FREQ_1200_000,	1200000,	.params.v2 = { 1,	2,	24,	4, } },
	{ AL_PLL_FREQ_1300_000,	1300000,	.params.v2 = { 1,	2,	26,	4, } },
	{ AL_PLL_FREQ_1333_333,	1333333,	.params.v2 = { 1,	2,	20,	3, } },
	{ AL_PLL_FREQ_1500_000,	1500000,	.params.v2 = { 1,	3,	15,	3, } },
	{ AL_PLL_FREQ_1600_000,	1600000,	.params.v2 = { 1,	2,	24,	3, } },
	{ AL_PLL_FREQ_1700_000,	1700000,	.params.v2 = { 1,	3,	17,	3, } },
	{ AL_PLL_FREQ_1800_000,	1800000,	.params.v2 = { 1,	3,	18,	3, } },
	{ AL_PLL_FREQ_1900_000,	1900000,	.params.v2 = { 1,	3,	19,	3, } },
	{ AL_PLL_FREQ_2000_000,	2000000,	.params.v2 = { 1,	3,	20,	3, } },
	{ AL_PLL_FREQ_2100_000,	2100000,	.params.v2 = { 1,	2,	21,	2, } },
	{ AL_PLL_FREQ_2200_000,	2200000,	.params.v2 = { 1,	2,	22,	2, } },
	{ AL_PLL_FREQ_2300_000,	2300000,	.params.v2 = { 1,	2,	23,	2, } },
	{ AL_PLL_FREQ_2400_000,	2400000,	.params.v2 = { 1,	2,	24,	2, } },
	{ AL_PLL_FREQ_2500_000,	2500000,	.params.v2 = { 1,	2,	25,	2, } },
	{ AL_PLL_FREQ_2600_000,	2600000,	.params.v2 = { 1,	2,	26,	2, } },
	{ AL_PLL_FREQ_2700_000,	2700000,	.params.v2 = { 1,	2,	27,	2, } },
	{ AL_PLL_FREQ_2800_000,	2800000,	.params.v2 = { 1,	2,	28,	2, } },
	{ AL_PLL_FREQ_2900_000,	2900000,	.params.v2 = { 1,	2,	29,	2, } },
	{ AL_PLL_FREQ_3000_000,	3000000,	.params.v2 = { 1,	2,	30,	2, } },
};

static const struct al_pll_freq_map_ent al_pll_freq_map_v3_100[] = {
/* freq,		freq_val,		ref_div,fb_div,	frac,	pd1,  pd2 */
{ AL_PLL_FREQ_500_000,	500000,	 .params.v3 = { 1,	30,	0,	6,	1, } },
{ AL_PLL_FREQ_533_333,	533333,	 .params.v3 = { 1,	32,	0,	6,	1, } },
{ AL_PLL_FREQ_666_666,	666666,	 .params.v3 = { 1,	20,	0,	3,	1, } },
{ AL_PLL_FREQ_800_000,	800000,	 .params.v3 = { 1,	32,	0,	4,	1, } },
{ AL_PLL_FREQ_933_333,	933333,	 .params.v3 = { 1,	28,	0,	3,	1, } },
{ AL_PLL_FREQ_1000_000, 1000000, .params.v3 = { 1,	20,	0,	2,	1, } },
{ AL_PLL_FREQ_1066_666, 1066666, .params.v3 = { 1,	32,	0,	3,	1, } },
{ AL_PLL_FREQ_1100_000, 1100000, .params.v3 = { 1,	22,	0,	2,	1, } },
{ AL_PLL_FREQ_1200_000, 1200000, .params.v3 = { 1,	24,	0,	2,	1, } },
{ AL_PLL_FREQ_1300_000, 1300000, .params.v3 = { 1,	26,	0,	2,	1, } },
{ AL_PLL_FREQ_1333_333, 1333333, .params.v3 = { 1,	26,	11184810, 2,	1, } },
{ AL_PLL_FREQ_1400_000, 1400000, .params.v3 = { 1,	28,	0,	2,	1, } },
{ AL_PLL_FREQ_1500_000, 1500000, .params.v3 = { 1,	30,	0,	2,	1, } },
{ AL_PLL_FREQ_1600_000, 1600000, .params.v3 = { 1,	32,	0,	2,	1, } },
{ AL_PLL_FREQ_1700_000, 1700000, .params.v3 = { 1,	17,	0,	1,	1, } },
{ AL_PLL_FREQ_1800_000, 1800000, .params.v3 = { 1,	18,	0,	1,	1, } },
{ AL_PLL_FREQ_1900_000, 1900000, .params.v3 = { 1,	19,	0,	1,	1, } },
{ AL_PLL_FREQ_2000_000, 2000000, .params.v3 = { 1,	20,	0,	1,	1, } },
{ AL_PLL_FREQ_2100_000, 2100000, .params.v3 = { 1,	21,	0,	1,	1, } },
{ AL_PLL_FREQ_2200_000, 2200000, .params.v3 = { 1,	22,	0,	1,	1, } },
{ AL_PLL_FREQ_2300_000, 2300000, .params.v3 = { 1,	23,	0,	1,	1, } },
{ AL_PLL_FREQ_2400_000, 2400000, .params.v3 = { 1,	24,	0,	1,	1, } },
{ AL_PLL_FREQ_2500_000, 2500000, .params.v3 = { 1,	25,	0,	1,	1, } },
{ AL_PLL_FREQ_2600_000, 2600000, .params.v3 = { 1,	26,	0,	1,	1, } },
{ AL_PLL_FREQ_2700_000, 2700000, .params.v3 = { 1,	27,	0,	1,	1, } },
{ AL_PLL_FREQ_2800_000, 2800000, .params.v3 = { 1,	28,	0,	1,	1, } },
{ AL_PLL_FREQ_3000_000, 3000000, .params.v3 = { 1,	30,	0,	1,	1, } },
};

static const struct al_pll_freq_map_ent al_pll_freq_map_v2_50[] = {
	/* freq,		freq_val,		       ref_div, pre,    post,   out_divj */
	{ AL_PLL_FREQ_500_000,	500000,		.params.v2 = { 1,	2,	50,	9, } },
	{ AL_PLL_FREQ_533_333,	533333,		.params.v2 = { 1,	2,	48,	9, } },
	{ AL_PLL_FREQ_666_666,	666666,		.params.v2 = { 1,	3,	40,	9, } },
	{ AL_PLL_FREQ_800_000,	800000,		.params.v2 = { 1,	2,	48,	6, } },
	{ AL_PLL_FREQ_900_000,	900000,		.params.v2 = { 1,	2,	54,	6, } },
	{ AL_PLL_FREQ_933_333,	933333,		.params.v2 = { 1,	2,	56,	6, } },
	{ AL_PLL_FREQ_1000_000,	1000000,	.params.v2 = { 1,	2,	50,	5, } },
	{ AL_PLL_FREQ_1050_000,	1050000,	.params.v2 = { 1,	2,	42,	4, } },
	{ AL_PLL_FREQ_1066_666,	1066666,	.params.v2 = { 1,	2,	53,	5, } },
	{ AL_PLL_FREQ_1200_000,	1200000,	.params.v2 = { 1,	2,	48,	4, } },
	{ AL_PLL_FREQ_1300_000,	1300000,	.params.v2 = { 1,	2,	52,	4, } },
	{ AL_PLL_FREQ_1333_333,	1333333,	.params.v2 = { 1,	2,	40,	3, } },
	{ AL_PLL_FREQ_1500_000,	1500000,	.params.v2 = { 1,	3,	30,	3, } },
	{ AL_PLL_FREQ_1600_000,	1600000,	.params.v2 = { 1,	2,	48,	3, } },
	{ AL_PLL_FREQ_1700_000,	1700000,	.params.v2 = { 1,	3,	34,	3, } },
	{ AL_PLL_FREQ_1800_000,	1800000,	.params.v2 = { 1,	3,	36,	3, } },
	{ AL_PLL_FREQ_1900_000,	1900000,	.params.v2 = { 1,	3,	38,	3, } },
	{ AL_PLL_FREQ_2000_000,	2000000,	.params.v2 = { 1,	3,	40,	3, } },
	{ AL_PLL_FREQ_2100_000,	2100000,	.params.v2 = { 1,	2,	42,	2, } },
	{ AL_PLL_FREQ_2200_000,	2200000,	.params.v2 = { 1,	2,	44,	2, } },
	{ AL_PLL_FREQ_2300_000,	2300000,	.params.v2 = { 1,	2,	46,	2, } },
	{ AL_PLL_FREQ_2400_000,	2400000,	.params.v2 = { 1,	2,	48,	2, } },
	{ AL_PLL_FREQ_2500_000,	2500000,	.params.v2 = { 1,	2,	50,	2, } },
	{ AL_PLL_FREQ_2600_000,	2600000,	.params.v2 = { 1,	2,	52,	2, } },
	{ AL_PLL_FREQ_2700_000,	2700000,	.params.v2 = { 1,	2,	54,	2, } },
	{ AL_PLL_FREQ_2800_000,	2800000,	.params.v2 = { 1,	2,	56,	2, } },
	{ AL_PLL_FREQ_2900_000,	2900000,	.params.v2 = { 1,	2,	58,	2, } },
	{ AL_PLL_FREQ_3000_000,	3000000,	.params.v2 = { 1,	2,	60,	2, } },
};

static const struct al_pll_freq_map_ent al_pll_freq_map_v3_50[] = {
/* freq,		freq_val,		ref_div,fb_div,	frac,	pd1,  pd2 */
{ AL_PLL_FREQ_533_333,	533333,	 .params.v3 = { 1,	64,	0,	6,	1, } },
{ AL_PLL_FREQ_666_666,	666666,	 .params.v3 = { 1,	40,	0,	3,	1, } },
{ AL_PLL_FREQ_933_333,	933333,	 .params.v3 = { 1,	56,	0,	3,	1, } },
{ AL_PLL_FREQ_1000_000, 1000000, .params.v3 = { 1,	40,	0,	2,	1, } },
{ AL_PLL_FREQ_1066_666, 1066666, .params.v3 = { 1,	64,	0,	3,	1, } },
{ AL_PLL_FREQ_1100_000, 1100000, .params.v3 = { 1,	44,	0,	2,	1, } },
{ AL_PLL_FREQ_1200_000, 1200000, .params.v3 = { 1,	48,	0,	2,	1, } },
{ AL_PLL_FREQ_1300_000, 1300000, .params.v3 = { 1,	52,	0,	2,	1, } },
{ AL_PLL_FREQ_1333_333, 1333333, .params.v3 = { 1,	53,	5592405, 2,	1, } },
{ AL_PLL_FREQ_1400_000, 1400000, .params.v3 = { 1,	56,	0,	2,	1, } },
{ AL_PLL_FREQ_1500_000, 1500000, .params.v3 = { 1,	60,	0,	2,	1, } },
{ AL_PLL_FREQ_1600_000, 1600000, .params.v3 = { 1,	64,	0,	2,	1, } },
{ AL_PLL_FREQ_1700_000, 1700000, .params.v3 = { 1,	34,	0,	1,	1, } },
{ AL_PLL_FREQ_1800_000, 1800000, .params.v3 = { 1,	36,	0,	1,	1, } },
{ AL_PLL_FREQ_1900_000, 1900000, .params.v3 = { 1,	38,	0,	1,	1, } },
{ AL_PLL_FREQ_2000_000, 2000000, .params.v3 = { 1,	40,	0,	1,	1, } },
{ AL_PLL_FREQ_2100_000, 2100000, .params.v3 = { 1,	42,	0,	1,	1, } },
{ AL_PLL_FREQ_2200_000, 2200000, .params.v3 = { 1,	44,	0,	1,	1, } },
{ AL_PLL_FREQ_2300_000, 2300000, .params.v3 = { 1,	46,	0,	1,	1, } },
{ AL_PLL_FREQ_2400_000, 2400000, .params.v3 = { 1,	48,	0,	1,	1, } },
{ AL_PLL_FREQ_2500_000, 2500000, .params.v3 = { 1,	50,	0,	1,	1, } },
{ AL_PLL_FREQ_2600_000, 2600000, .params.v3 = { 1,	52,	0,	1,	1, } },
{ AL_PLL_FREQ_2700_000, 2700000, .params.v3 = { 1,	54,	0,	1,	1, } },
{ AL_PLL_FREQ_2800_000, 2800000, .params.v3 = { 1,	56,	0,	1,	1, } },
{ AL_PLL_FREQ_3000_000, 3000000, .params.v3 = { 1,	60,	0,	1,	1, } },
};

static const struct al_pll_freq_map_ent al_pll_freq_map_v3_25[] = {
/* freq,		freq_val,		ref_div,fb_div,	frac,	pd1,  pd2 */
{ AL_PLL_FREQ_533_333,	533333,	 .params.v3 = { 1,	128,	0,	6,	1, } },
{ AL_PLL_FREQ_666_666,	666666,	 .params.v3 = { 1,	80,	0,	3,	1, } },
{ AL_PLL_FREQ_933_333,	933333,	 .params.v3 = { 1,	112,	0,	3,	1, } },
{ AL_PLL_FREQ_1000_000, 1000000, .params.v3 = { 1,	80,	0,	2,	1, } },
{ AL_PLL_FREQ_1066_666, 1066666, .params.v3 = { 1,	128,	0,	3,	1, } },
{ AL_PLL_FREQ_1100_000, 1100000, .params.v3 = { 1,	88,	0,	2,	1, } },
{ AL_PLL_FREQ_1200_000, 1200000, .params.v3 = { 1,	96,	0,	2,	1, } },
{ AL_PLL_FREQ_1300_000, 1300000, .params.v3 = { 1,	104,	0,	2,	1, } },
{ AL_PLL_FREQ_1333_333, 1333333, .params.v3 = { 1,	106,	11184810, 2,	1, } },
{ AL_PLL_FREQ_1400_000, 1400000, .params.v3 = { 1,	112,	0,	2,	1, } },
{ AL_PLL_FREQ_1500_000, 1500000, .params.v3 = { 1,	120,	0,	2,	1, } },
{ AL_PLL_FREQ_1600_000, 1600000, .params.v3 = { 1,	128,	0,	2,	1, } },
{ AL_PLL_FREQ_1700_000, 1700000, .params.v3 = { 1,	68,	0,	1,	1, } },
{ AL_PLL_FREQ_1800_000, 1800000, .params.v3 = { 1,	72,	0,	1,	1, } },
{ AL_PLL_FREQ_1900_000, 1900000, .params.v3 = { 1,	76,	0,	1,	1, } },
{ AL_PLL_FREQ_2000_000, 2000000, .params.v3 = { 1,	80,	0,	1,	1, } },
{ AL_PLL_FREQ_2100_000, 2100000, .params.v3 = { 1,	84,	0,	1,	1, } },
{ AL_PLL_FREQ_2200_000, 2200000, .params.v3 = { 1,	88,	0,	1,	1, } },
{ AL_PLL_FREQ_2300_000, 2300000, .params.v3 = { 1,	92,	0,	1,	1, } },
{ AL_PLL_FREQ_2400_000, 2400000, .params.v3 = { 1,	96,	0,	1,	1, } },
{ AL_PLL_FREQ_2500_000, 2500000, .params.v3 = { 1,	100,	0,	1,	1, } },
{ AL_PLL_FREQ_2600_000, 2600000, .params.v3 = { 1,	104,	0,	1,	1, } },
{ AL_PLL_FREQ_2700_000, 2700000, .params.v3 = { 1,	108,	0,	1,	1, } },
{ AL_PLL_FREQ_2800_000, 2800000, .params.v3 = { 1,	112,	0,	1,	1, } },
{ AL_PLL_FREQ_3000_000, 3000000, .params.v3 = { 1,	120,	0,	1,	1, } },
};
#endif

/* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */
#endif

/** @} end of PLL group */
