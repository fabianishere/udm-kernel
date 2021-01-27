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

#include "al_hal_pcie_interrupts.h"
#include "al_hal_pcie_regs.h"
#include "al_hal_iofic_regs.h"

/**
 * get the interrupt group base registers
 * @param  pcie_port     port handle
 * @param  int_group     interrupt group
 * @return               regs base or NULL in case the group is not supported
 */
static void *al_pcie_app_int_grp_regs_base_get(
	const struct al_pcie_port *pcie_port,
	enum al_pcie_int_group int_group)
{
	switch (int_group) {
	case AL_PCIE_INT_GRP_A:
		return pcie_port->regs->app.int_grp_a;
	case AL_PCIE_INT_GRP_B:
		return pcie_port->regs->app.int_grp_b;
	case AL_PCIE_INT_GRP_C:
		if (pcie_port->rev_id < AL_PCIE_REV_ID_3) {
			al_err("PCIe %d: Group C not supported in rev %d\n",
				pcie_port->port_id, pcie_port->rev_id);
			return NULL;
		}
		return pcie_port->regs->app.int_grp_c;
	case AL_PCIE_INT_GRP_D:
		if (pcie_port->rev_id < AL_PCIE_REV_ID_3) {
			al_err("PCIe %d: Group D not supported in rev %d\n",
				pcie_port->port_id, pcie_port->rev_id);
			return NULL;
		}
		return pcie_port->regs->app.int_grp_d;
	case AL_PCIE_INT_GRP_AXI_A:
		return pcie_port->regs->axi.int_grp_a;
	default:
		al_err("PCIe %d: unsupported group[%d]\n",
			pcie_port->port_id, int_group);
		return NULL;
	}
}

/* Enable PCIe controller interrupts */
void al_pcie_ints_config(const struct al_pcie_port *pcie_port)
{
	al_iofic_config(pcie_port->regs->app.int_grp_a, 0,
			INT_CONTROL_GRP_SET_ON_POSEDGE);

	al_iofic_config(pcie_port->regs->app.int_grp_b, 0,
			INT_CONTROL_GRP_SET_ON_POSEDGE);

	if (pcie_port->rev_id >= AL_PCIE_REV_ID_3) {
		al_iofic_config(pcie_port->regs->app.int_grp_c, 0, 0);
		al_iofic_config(pcie_port->regs->app.int_grp_d, 0, 0);
	}

	al_iofic_config(pcie_port->regs->axi.int_grp_a, 0, 0);
}

void al_pcie_app_int_grp_unmask(
	const struct al_pcie_port *pcie_port,
	enum al_pcie_int_group int_group,
	uint32_t int_mask)
{
	void __iomem *int_regs_base = al_pcie_app_int_grp_regs_base_get(pcie_port, int_group);

	al_assert(int_regs_base);

	if (likely(int_regs_base))
		al_iofic_unmask(int_regs_base, 0, int_mask);
}

void al_pcie_app_int_grp_mask(
	const struct al_pcie_port *pcie_port,
	enum al_pcie_int_group int_group,
	uint32_t int_mask)
{
	void __iomem *int_regs_base = al_pcie_app_int_grp_regs_base_get(pcie_port, int_group);

	al_assert(int_regs_base);

	if (likely(int_regs_base))
		al_iofic_mask(int_regs_base, 0, int_mask);
}

void al_pcie_app_int_grp_cause_clear(
	const struct al_pcie_port *pcie_port,
	enum al_pcie_int_group int_group,
	uint32_t int_cause)
{
	void __iomem *int_regs_base = al_pcie_app_int_grp_regs_base_get(pcie_port, int_group);

	al_assert(int_regs_base);

	if (likely(int_regs_base))
		al_iofic_clear_cause(int_regs_base, 0, int_cause);
}

uint32_t al_pcie_app_int_grp_cause_read(
	const struct al_pcie_port *pcie_port,
	enum al_pcie_int_group int_group)
{
	void __iomem *int_regs_base = al_pcie_app_int_grp_regs_base_get(pcie_port, int_group);

	al_assert(int_regs_base);

	if (likely(int_regs_base))
		return al_iofic_read_cause(int_regs_base, 0);

	return 0;
}

void al_pcie_app_int_grp_cause_set(
	const struct al_pcie_port *pcie_port,
	enum al_pcie_int_group int_group,
	uint32_t int_cause)
{
	void __iomem *int_regs_base = al_pcie_app_int_grp_regs_base_get(pcie_port, int_group);

	al_assert(int_regs_base);

	al_iofic_set_cause(int_regs_base, 0, int_cause);
}

uint32_t al_pcie_app_int_cause_read_and_clear(
	struct al_pcie_port *pcie_port,
	enum al_pcie_int_group int_group,
	uint32_t int_mask)
{
	void __iomem *int_regs_base = al_pcie_app_int_grp_regs_base_get(pcie_port, int_group);

	al_assert(int_regs_base);

	if (likely(int_regs_base))
		return al_iofic_read_and_clear_cause(int_regs_base, 0, int_mask);

	return 0;
}
