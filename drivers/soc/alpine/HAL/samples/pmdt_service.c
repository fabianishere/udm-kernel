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
#include "stdlib.h"

/**
 * @defgroup group_pmdt_service_samples Code Samples
 * @ingroup group_pmdt_service
 * @{
 * pmdt_service.c: this file can be found in samples directory.
 *
 * Example of pmdt service operation
 *
 * @code */
#include "samples.h"
#include "al_pmdt.h"

static struct al_pmdt_handle pmdt;

/***********************************************************
 * stubs for irq registration demonstration                */
#define AL_IRQ_PMDTU 0
enum al_nb_gic_id {NB_GIC_MAIN = 0, NB_GIC_SECONDARY = 1};
typedef void (*irq_handler)(uint32_t irq, enum al_nb_gic_id gic);
static int irq_config(
		enum al_nb_gic_id gic,
		uint32_t irq,
		unsigned int cpu_mask,
		al_bool edge_triggered,
		irq_handler handler)
{
	al_info("%d %d %d %d %p\n",
			gic, irq, cpu_mask, edge_triggered, handler);
	return 0;
}
/***********************************************************/


static void pmdt_irq_handler(uint32_t irq, enum al_nb_gic_id gic)
{
	struct al_pmdt_isr_info info;
	uint64_t read_hist[AL_PMDT_AXI_MON_BAR_NUM];
	uint64_t write_hist[AL_PMDT_AXI_MON_BAR_NUM];
	unsigned int i, bin_num;
	int rc = 1;

	al_info("irq %d, gic %d\n", irq, gic);

	while (rc > 0) {
		rc = al_pmdt_isr(&pmdt, &info);

		if (rc < 0) {
			al_info("pmdt isr error\n");
			return;
		}

		al_info("pmdt %d trapped axi error of type %d\n",
				info.pmdt_unit, info.err);

		al_latency_histograms_get(
				&pmdt,
				info.pmdt_unit,
				&bin_num,
				&read_hist,
				&write_hist);

		for (i = 0; i < bin_num; i++)
			al_info("bar %d: read latency %d, write latency %d\n",
					i, (int)read_hist[i], (int)write_hist[i]);
	}
}

int main(void)
{
	int rc;

	/* register irq */
	rc = irq_config(NB_GIC_MAIN, AL_IRQ_PMDTU, 1<<0, AL_TRUE, pmdt_irq_handler);

	if (rc < 0)
		al_info("irq registration failure\n");

	rc = al_pmdt_monitor(&pmdt, NULL, AL_TRUE, NULL);

	if (rc < 0)
		al_info("pmdt service error\n");
	else
		al_info("pmdt service start\n");

	return 0;
}

/** @endcode */

/** @} */

