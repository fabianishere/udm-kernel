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

#include "al_mod_serdes.h"
#include "al_mod_hal_serdes_hssp.h"
#include "al_mod_hal_serdes_25g.h"

static int(*handle_init[AL_SRDS_NUM_GROUPS])(void __iomem *, struct al_mod_serdes_grp_obj *) = {
	al_mod_serdes_hssp_handle_init,
	al_mod_serdes_hssp_handle_init,
	al_mod_serdes_hssp_handle_init,
	al_mod_serdes_hssp_handle_init,
#if CHECK_ALPINE_V2
#if !defined(AL_SERDES_PCIE_ONLY) || (AL_SERDES_PCIE_ONLY == 0)
	al_mod_serdes_25g_handle_init,
#else
	NULL,
#endif
#endif
};

int al_mod_serdes_handle_grp_init(
	void __iomem			*serdes_regs_base,
	enum al_mod_serdes_group		grp,
	struct al_mod_serdes_grp_obj	*obj)
{
	al_mod_assert(grp < AL_SRDS_NUM_GROUPS);

	if (handle_init[grp])
		handle_init[grp](serdes_regs_base, obj);

	return 0;
}

void  al_mod_serdes_temperature_get_cb_set(struct al_mod_serdes_grp_obj *obj,
					int (*temperature_get)(void))
{
	obj->temperature_get_cb = temperature_get;
}

void  al_mod_serdes_udelay_cb_set(struct al_mod_serdes_grp_obj *obj,
					void (*udelay_cb)(void *context, unsigned int delay),
					void *udelay_cb_context)
{
	obj->udelay_cb = udelay_cb;
	obj->udelay_cb_context = udelay_cb_context;
}

void  al_mod_serdes_msleep_cb_set(struct al_mod_serdes_grp_obj *obj,
					void (*msleep_cb)(void *context, unsigned int delay),
					void *msleep_cb_context)
{
	obj->msleep_cb = msleep_cb;
	obj->msleep_cb_context = msleep_cb_context;
}
