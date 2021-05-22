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
 *  @{
 * @file   al_bootstrap.c
 *
 * @brief Bootstrap processing service
 *
 */
#include "al_bootstrap.h"
#include "al_hal_iomap.h"
#include "al_hal_pll.h"

int al_bootstrap_get(struct al_bootstrap *bootstrap)
{
	enum al_pll_freq pll_freq;
	unsigned int pll_freq_val;
	unsigned int chan_freq;
	struct al_pll_obj obj;
	int err;

	err = al_bootstrap_parse((void *)AL_PBS_REGFILE_BASE, bootstrap);
	if (err)
		return err;

	err = al_pll_init(
		(void __iomem *)AL_PLL_BASE(AL_PLL_NB),
		"NB PLL",
		(bootstrap->pll_ref_clk_freq == 25000000) ?
		AL_PLL_REF_CLK_FREQ_25_MHZ :
		AL_PLL_REF_CLK_FREQ_100_MHZ,
		&obj);
	if (err)
		return err;

	err = al_pll_freq_get(
		&obj,
		&pll_freq,
		&pll_freq_val);
	if (err)
		return err;

	err = al_pll_channel_freq_get(&obj, 0, &chan_freq);
	if (err)
		return err;

	bootstrap->ddr_pll_freq = chan_freq * 1000 + chan_freq % 1000;

	err = al_pll_init(
		(void __iomem *)AL_PLL_BASE(AL_PLL_CPU),
		"CPU PLL",
		(bootstrap->pll_ref_clk_freq == 25000000) ?
		AL_PLL_REF_CLK_FREQ_25_MHZ :
		AL_PLL_REF_CLK_FREQ_100_MHZ,
		&obj);
	if (err)
		return err;

	err = al_pll_freq_get(
		&obj,
		&pll_freq,
		&pll_freq_val);
	if (err)
		return err;

	err = al_pll_channel_freq_get(&obj, 0, &chan_freq);
	if (err)
		return err;

	bootstrap->cpu_pll_freq = chan_freq * 1000UL;

	return 0;
}

/**
 * @}
 */

