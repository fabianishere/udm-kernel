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
 * @addtogroup group_muio_mux
 *
 *  @{
 * @file   al_hal_muio_mux.c
 *
 * @brief  MUIO mux HAL driver
 *
 */

#include "al_hal_muio_mux.h"
#include "al_hal_muio_mux_map.h"
#include "al_hal_pbs_utils.h"

#define FUNC_NUM_BITS_ALPNIE		2
#define FUNC_NUM_BITS_ALPINE_V2		4

#define FUNC_MASK_ALP			0x03
#define FUNC_MASK_ALPINE_V2		0x0F

#define NUM_MUIO_PER_REG_ALP		(32 / (FUNC_NUM_BITS_ALPNIE))
#define NUM_MUIO_PER_REG_ALPINE_V2	(32 / (FUNC_NUM_BITS_ALPINE_V2))

#define REGS_NUM_ALP			3
#define REGS_NUM_ALPINE_V2		6
#define REGS_NUM_ALPINE_V3		8

enum func_mode {
	ALLOC_MODE,
	FREE_MODE
};

#define PIN_REG(pin, dev_id) ((pin) / (((dev_id) ==\
	PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V1) ? \
	NUM_MUIO_PER_REG_ALP : NUM_MUIO_PER_REG_ALPINE_V2))
#define PIN_NUM_IN_REG(pin, dev_id) ((pin) % (((dev_id) ==\
	PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V1) ? \
	NUM_MUIO_PER_REG_ALP : NUM_MUIO_PER_REG_ALPINE_V2))

/**
 * Extended function for allocating or freeing an interface,
 * with and without forcing
 */
static int iface_upd_ex_s(
	struct al_muio_mux_obj	*obj,
	enum al_muio_mux_if	iface,
	int			arg,
	int			force,
	enum func_mode          func_mode);

/******************************************************************************/
/******************************************************************************/
int al_muio_mux_init(
	void __iomem		*regs_base,
	const char		*name,
	struct al_muio_mux_obj	*obj)
{
	int status = 0;

	int pin;

	unsigned int dev_id;
	unsigned int rev_id;

	al_dbg("%s(%p, %s)\n", __func__, regs_base, name);

	al_assert(regs_base);
	al_assert(name);
	al_assert(obj);

	obj->regs_base = (struct al_pbs_regs *)regs_base;

	obj->name = name;

	for (pin = 0; pin < AL_MUIO_MUX_NUM_MUIO; pin++)
		obj->pins_iface[pin] = AL_MUIO_MUX_IF_NONE;

	dev_id = al_pbs_dev_id_get(obj->regs_base);
	rev_id = al_pbs_dev_rev_id_get(obj->regs_base);

	if (dev_id == PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V1) {
		obj->dev_prop.regs_num = REGS_NUM_ALP;
		obj->dev_prop.func_mask = FUNC_MASK_ALP;
		obj->dev_prop.func_num_bits = FUNC_NUM_BITS_ALPNIE;
	} else if ((dev_id == PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V2) ||
		((dev_id == PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V3) &&
		(rev_id == PBS_UNIT_CHIP_ID_DEV_REV_ID_ALPINE_V3_TC))) {
		obj->dev_prop.regs_num = REGS_NUM_ALPINE_V2;
		obj->dev_prop.func_mask = FUNC_MASK_ALPINE_V2;
		obj->dev_prop.func_num_bits = FUNC_NUM_BITS_ALPINE_V2;
	} else {
		obj->dev_prop.regs_num = REGS_NUM_ALPINE_V3;
		obj->dev_prop.func_mask = FUNC_MASK_ALPINE_V2;
		obj->dev_prop.func_num_bits = FUNC_NUM_BITS_ALPINE_V2;
	}

	obj->dev_prop.dev_id = dev_id;

	return status;
}

/******************************************************************************/
/******************************************************************************/
int al_muio_mux_terminate(
	struct al_muio_mux_obj	*obj)
{
	int status = 0;

	al_dbg("%s()\n", __func__);

	al_assert(obj);

	return status;
}

/******************************************************************************/
/******************************************************************************/
static int iface_upd_ex_s(
	struct al_muio_mux_obj	*obj,
	enum al_muio_mux_if	iface,
	int			arg,
	int			force,
	enum func_mode          func_mode)
{
	int status = 0;
	int i;
	int gpio_pin = arg;

	const struct al_muio_mux_iface_pins gpio_pins = {
		AL_MUIO_MUX_IF_GPIO,
		AL_MUIO_MUX_FUNC_GPIO,
		1,
		&gpio_pin,
		AL_MUIO_MUX_CHIP_VER_MIN,
		AL_MUIO_MUX_CHIP_VER_MAX,
	};

	const struct al_muio_mux_iface_pins *iface_pins = NULL;
	 /*
	  * REGS_NUM_ALPINE_V3 > REGS_NUM_ALPINE_V2
	  * REGS_NUM_ALPINE_V2 > REGS_NUM_ALP,
	  * so we declare on the array with maximum size possible.
	  */
	uint32_t *const pbs_mux_sel_addr[REGS_NUM_ALPINE_V3] = {
			&obj->regs_base->unit.pbs_mux_sel_0,
			&obj->regs_base->unit.pbs_mux_sel_1,
			&obj->regs_base->unit.pbs_mux_sel_2,
			&obj->regs_base->unit.pbs_mux_sel_3,
			&obj->regs_base->unit.pbs_mux_sel_4,
			&obj->regs_base->unit.pbs_mux_sel_5,
			&obj->regs_base->unit.pbs_mux_sel_6,
			&obj->regs_base->unit.pbs_mux_sel_7,
	};
	uint32_t mux_sel_reg_val[REGS_NUM_ALPINE_V3];


	al_dbg("%s(%d, %d)\n", __func__, iface, arg);

	if (iface != AL_MUIO_MUX_IF_GPIO) {
		for (i = 0; i < AL_MUIO_MUX_NUM_IFACES; i++)
			if ((_al_muio_mux_iface_pins[i].iface == iface) &&
				(_al_muio_mux_iface_pins[i].chip_version_min <=
				obj->dev_prop.dev_id) &&
				(_al_muio_mux_iface_pins[i].chip_version_max >=
				obj->dev_prop.dev_id)) {
				iface_pins = &_al_muio_mux_iface_pins[i];
				break;
			}
	} else {
		iface_pins = &gpio_pins;
	}

	if (!iface_pins) {
		al_err("%s: invalid iface (%d, %d)!\n",	__func__, iface, arg);
		status = -EINVAL;
		goto done;
	}

	if (!force) {
		for (i = 0; i < iface_pins->num_pins; i++) {
			int pin = iface_pins->pins[i];
			enum al_muio_mux_if pin_if = obj->pins_iface[pin];

			if (func_mode == ALLOC_MODE) {
				if (pin_if != AL_MUIO_MUX_IF_NONE) {
					al_err(
						"%s(%d): pin %d taken by iface %d!\n",
						__func__,
						iface,
						pin,
						obj->pins_iface[pin]);
					status = -EBUSY;
					goto done;
				}
			} else { /* FREE_MODE */
				if (pin_if != iface) {
					al_err(
						"%s(%d): not fully allocated!\n",
						__func__,
						iface);

					status = -EIO;
					goto done;
				}
			}
		}
	}

	for (i = 0; i < obj->dev_prop.regs_num; ++i)
		mux_sel_reg_val[i] = al_reg_read32(pbs_mux_sel_addr[i]);

	for (i = 0; i < iface_pins->num_pins; i++) {
		int pin = iface_pins->pins[i];
		int mux_func = (func_mode == ALLOC_MODE) ?
				iface_pins->mux_func : AL_MUIO_MUX_FUNC_GPIO;

		/* the relevant reg for the current pin */
		unsigned int pin_reg = PIN_REG(pin, obj->dev_prop.dev_id);
		/* the pin shift in the relevant reg */
		unsigned int pin_reg_shift = PIN_NUM_IN_REG(pin, obj->dev_prop.dev_id);

		al_dbg(
			"%s: pin %d, func %d\n",
			__func__,
			pin,
			iface_pins->mux_func);

		obj->pins_iface[pin] = (func_mode == ALLOC_MODE) ?
						iface : AL_MUIO_MUX_IF_NONE;

		mux_sel_reg_val[pin_reg] &=
				~(obj->dev_prop.func_mask <<
						(obj->dev_prop.func_num_bits * pin_reg_shift));

		mux_sel_reg_val[pin_reg] |=
				(mux_func << (obj->dev_prop.func_num_bits * pin_reg_shift));
	}

	for (i = 0; i < obj->dev_prop.regs_num; ++i)
		al_reg_write32(pbs_mux_sel_addr[i], mux_sel_reg_val[i]);

done:

	return status;
}

/******************************************************************************/
/******************************************************************************/
int al_muio_mux_iface_alloc(
	struct al_muio_mux_obj	*obj,
	enum al_muio_mux_if	iface,
	int			arg)
{
	al_dbg(
		"%s(%d, %d)\n",
		__func__,
		iface,
		arg);

	al_assert(obj);

	return iface_upd_ex_s(obj, iface, arg, 0, ALLOC_MODE);
}

/******************************************************************************/
/******************************************************************************/
int al_muio_mux_iface_alloc_force(
	struct al_muio_mux_obj	*obj,
	enum al_muio_mux_if	iface,
	int			arg)
{
	al_dbg(
		"%s(%d, %d)\n",
		__func__,
		iface,
		arg);

	al_assert(obj);

	return iface_upd_ex_s(obj, iface, arg, 1, ALLOC_MODE);
}

/******************************************************************************/
/******************************************************************************/
int al_muio_mux_iface_alloc_multi(
	struct al_muio_mux_obj			*obj,
	const struct al_muio_mux_if_and_arg	*ifaces_and_args,
	int					ifaces_cnt)
{
	int status = 0;
	int i;

	al_dbg(
		"%s(%p, %d)\n",
		__func__,
		ifaces_and_args,
		ifaces_cnt);

	al_assert(obj);
	al_assert(ifaces_and_args);

	for (i = 0; i < ifaces_cnt; i++) {
		status = iface_upd_ex_s(
			obj,
			ifaces_and_args[i].iface,
			ifaces_and_args[i].arg,
			0,
			ALLOC_MODE);

		if (status != 0) {
			al_err(
				"%s: al_muio_mux_iface_alloc(%d, %d) failed!\n",
				__func__,
				ifaces_and_args[i].iface,
				ifaces_and_args[i].arg);
			i--;
			goto fail_free_allocated;
		}
	}

	goto done;

fail_free_allocated:

	for (; i >= 0; i--)
		al_muio_mux_iface_free(
			obj,
			ifaces_and_args[i].iface,
			ifaces_and_args[i].arg);

done:

	return status;
}

/******************************************************************************/
/******************************************************************************/
int al_muio_mux_iface_free(
	struct al_muio_mux_obj	*obj,
	enum al_muio_mux_if	iface,
	int			arg)
{
	al_dbg(
		"%s(%d, %d)\n",
		__func__,
		iface,
		arg);

	al_assert(obj);

	return iface_upd_ex_s(obj, iface, arg, 0, FREE_MODE);
}

/** @} end of MUIO mux group */

