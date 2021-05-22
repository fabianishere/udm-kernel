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

/**
 *  @{
 * @file   al_hal_eth_mac.c
 *
 * @brief  Ethernet MAC HAL
 *
 */

#include "al_hal_eth.h"
#include "al_hal_eth_mac.h"
#include "al_hal_eth_mac_internal.h"

/** MAC iofic offset for Eth rev 1 - 3 */
#define AL_ETH_MAC_IOFIC_OFFSET_V1_V3 0x0800

/** Check if implementation exists in mac object  */
#define MAC_OBJ_FUNC_EXISTS(x)\
	do { \
		al_assert(adapter);\
		if (!adapter->mac_obj.x) { \
			al_warn("%s: Not supported for rev id %u\n", __func__, adapter->rev_id);\
			return -ENOSYS;\
		} \
	} while (0)

#define MAC_OBJ_FUNC_EXISTS_HANDLE(x)\
	do { \
		al_assert(handle);\
		if (!handle->mac_obj->x) { \
			al_warn("%s: Not supported for mac obj\n", __func__);\
			return -ENOSYS;\
		} \
	} while (0)

#define MAC_OBJ_FUNC_EXISTS_SELF(obj, foo)\
	do { \
		al_assert(obj);\
		if (!obj->foo) { \
			al_warn("%s: " #obj "doesnt have implementation of " #foo "\n", __func__);\
			return -ENOSYS;\
		} \
	} while (0)

static size_t al_eth_mac_iofic_regs_offset(int rev_id)
{
	switch (rev_id) {

	case AL_ETH_REV_ID_1:
	case AL_ETH_REV_ID_2:
	case AL_ETH_REV_ID_3:
		return AL_ETH_MAC_IOFIC_OFFSET_V1_V3;
	case AL_ETH_REV_ID_4:
		return al_offsetof(struct al_eth_mac_v4_regs, int_ctrl_mem);
	default:
		al_warn("%s: Not supported for rev id %u\n", __func__, rev_id);
		al_assert(0);
		return 0;
	}
}

int al_eth_mac_handle_init(struct al_eth_mac_obj *obj, struct al_eth_mac_obj_init_params *params)
{
	al_assert(params->mac_regs_base);

	al_memset(obj, 0, sizeof(*obj));

	obj->mac_regs_base = params->mac_regs_base;
	obj->mac_common_regs = params->mac_common_regs;
	obj->mac_common_regs_lock = params->mac_common_regs_lock;
	obj->mac_common_regs_lock_handle = params->mac_common_regs_lock_handle;
	obj->serdes_lane = params->serdes_lane;
	obj->name = params->name;
	obj->mac_iofic_regs_base = (void __iomem *)((uint8_t *)obj->mac_regs_base +
		al_eth_mac_iofic_regs_offset(params->eth_rev_id));
	obj->dme_conf = params->dme_conf;

	if (params->eth_original_rev_id) {
		obj->eth_original_rev_id = params->eth_original_rev_id;
	} else {
		al_warn("%s: eth_original_rev_id not supplied. assuming rev id %u\n",
			__func__, params->eth_rev_id);
		obj->eth_original_rev_id = params->eth_rev_id;
	}

	switch (params->eth_rev_id) {
	case AL_ETH_REV_ID_1:
	case AL_ETH_REV_ID_2:
		al_eth_mac_v1_v2_handle_init(obj, params);
		break;
	case AL_ETH_REV_ID_3:
		obj->fec_type = AL_ETH_FEC_TYPE_CLAUSE_74;
		al_eth_mac_v3_handle_init(obj, params);
		break;
	case AL_ETH_REV_ID_4:
		al_eth_mac_v4_handle_init(obj, params);
		break;
	default:
		al_warn("%s: Not supported for rev id %u\n", __func__, params->eth_rev_id);
		return -EINVAL;
	}

	return 0;
}

void __iomem *al_eth_mac_iofic_regs_get(
	struct al_eth_mac_obj *obj)
{
	al_assert(obj);
	al_assert(obj->mac_iofic_regs_base);
	return obj->mac_iofic_regs_base;
}

const char *al_eth_mdio_type_str(enum al_eth_mdio_type type_name)
{
	switch (type_name) {
	case AL_ETH_MDIO_TYPE_CLAUSE_22:
		return "Clause 22";
	case AL_ETH_MDIO_TYPE_CLAUSE_45:
		return "Clause 45";
	default:
		return "N/A";
	}
}

const char *al_eth_mac_mode_str(enum al_eth_mac_mode mode)
{
	switch (mode) {
	case AL_ETH_MAC_MODE_RGMII:
		return "RGMII";
	case AL_ETH_MAC_MODE_SGMII:
		return "SGMII";
	case AL_ETH_MAC_MODE_SGMII_2_5G:
		return "SGMII_2_5G";
	case AL_ETH_MAC_MODE_10GbE_Serial:
		return "KR";
	case AL_ETH_MAC_MODE_KR_LL_25G:
		return "KR_LL_25G";
	case AL_ETH_MAC_MODE_10G_SGMII:
		return "10G_SGMII";
	case AL_ETH_MAC_MODE_XLG_LL_40G:
		return "40G_LL";
	case AL_ETH_MAC_MODE_XLG_LL_50G:
		return "50G_LL";
	case AL_ETH_MAC_MODE_XLG_LL_25G:
		return "25G_LL";
	case AL_ETH_MAC_MODE_CG_100G:
		return "100G";
	default:
		return "N/A";
	}
}

const char *al_eth_fec_type_str(enum al_eth_fec_type type)
{
	switch (type) {
	case AL_ETH_FEC_TYPE_CLAUSE_74:
		return "Clause 74";
	case AL_ETH_FEC_TYPE_CLAUSE_91:
		return "Clause 91";
	default:
		return "N/A";
	}
}

int al_eth_mac_mode_set(struct al_hal_eth_adapter *adapter, enum al_eth_mac_mode mode)
{
	int rc;

	MAC_OBJ_FUNC_EXISTS(mode_set);

	rc = adapter->mac_obj.mode_set(&adapter->mac_obj, mode);

	if (rc)
		return rc;

	adapter->mac_mode = mode;

	return 0;
}

int al_eth_mac_config(struct al_hal_eth_adapter *adapter, enum al_eth_mac_mode mode)
{
	int ret;

	MAC_OBJ_FUNC_EXISTS(config);

	ret = adapter->mac_obj.config(&adapter->mac_obj, mode);
	if (ret == -EPERM) {
		al_err("%s: unsupported MAC mode %s (%d)\n",
			__func__, al_eth_mac_mode_str(mode), mode);
		return ret;
	} else if (ret) {
		return ret;
	}

	adapter->mac_mode = mode;

	al_info("Configured MAC to %s mode\n", al_eth_mac_mode_str(mode));

	return 0;
}

int al_eth_mac_stop(struct al_hal_eth_adapter *adapter)
{
	MAC_OBJ_FUNC_EXISTS(mac_start_stop_adv);

	return adapter->mac_obj.mac_start_stop_adv(&adapter->mac_obj,
						   AL_TRUE,
						   AL_TRUE,
						   AL_FALSE,
						   AL_FALSE);
}

int al_eth_mac_start(struct al_hal_eth_adapter *adapter)
{
	MAC_OBJ_FUNC_EXISTS(mac_start_stop_adv);

	return adapter->mac_obj.mac_start_stop_adv(&adapter->mac_obj,
						   AL_TRUE,
						   AL_TRUE,
						   AL_TRUE,
						   AL_TRUE);
}

int al_eth_mac_loopback_config(struct al_hal_eth_adapter *adapter, al_bool enable)
{
	int ret;

	MAC_OBJ_FUNC_EXISTS(loopback_config);

	al_dbg("eth [%s]: loopback %s\n", adapter->name, (enable ? "enable" : "disable"));

	ret = adapter->mac_obj.loopback_config(&adapter->mac_obj, enable);
	if (ret == -EPERM) {
		al_err("%s: MAC loopback not supported in this mode %s (%d)\n",
			__func__, al_eth_mac_mode_str(adapter->mac_mode), adapter->mac_mode);
		return ret;
	}

	return ret;
}

int al_eth_mac_stats_get(struct al_hal_eth_adapter *adapter, struct al_eth_mac_stats *stats)
{
	int ret;

	MAC_OBJ_FUNC_EXISTS(stats_get);

	ret = adapter->mac_obj.stats_get(&adapter->mac_obj, stats);
	if (ret == -EPERM) {
		al_err("%s: MAC statistics not supported in this mode %s (%d)\n",
			__func__, al_eth_mac_mode_str(adapter->mac_mode), adapter->mac_mode);
		return ret;
	}

	return ret;
}

int al_eth_fec_enable(struct al_hal_eth_adapter *adapter,
	enum al_eth_fec_type type, al_bool enable)
{
	int ret;

	MAC_OBJ_FUNC_EXISTS(fec_enable);

	ret = adapter->mac_obj.fec_enable(&adapter->mac_obj, type, enable);
	if (ret == -EPERM) {
		al_err("%s: FEC type %s (%d) is not supported\n",
			__func__, al_eth_fec_type_str(type), type);
		return ret;
	}

	if (!ret)
		adapter->mac_obj.fec_type = type;

	return ret;
}

al_bool al_eth_fec_is_enabled(struct al_hal_eth_adapter *adapter)
{
	MAC_OBJ_FUNC_EXISTS(fec_is_enabled);

	return adapter->mac_obj.fec_is_enabled(&adapter->mac_obj);
}

int al_eth_fec_stats_get(struct al_hal_eth_adapter *adapter,
	uint32_t *corrected, uint32_t *uncorrectable)
{
	int ret;

	MAC_OBJ_FUNC_EXISTS(fec_stats_get);

	ret = adapter->mac_obj.fec_stats_get(&adapter->mac_obj, corrected, uncorrectable);
	if (ret == -EPERM) {
		al_err("%s: FEC statistics get for type %s (%d) is not supported\n",
			__func__,
			al_eth_fec_type_str(adapter->mac_obj.fec_type), adapter->mac_obj.fec_type);
		return ret;
	}

	return ret;
}


int al_eth_gearbox_reset(struct al_hal_eth_adapter *adapter,
	al_bool tx_reset, al_bool rx_reset)
{
	MAC_OBJ_FUNC_EXISTS(gearbox_reset);

	return adapter->mac_obj.gearbox_reset(&adapter->mac_obj, tx_reset, rx_reset);
}

int al_eth_mac_link_config(struct al_hal_eth_adapter *adapter,
			   al_bool force_1000_base_x, al_bool an_enable,
			   uint32_t speed, al_bool full_duplex)
{
	MAC_OBJ_FUNC_EXISTS(link_config);

	return adapter->mac_obj.link_config(&adapter->mac_obj, force_1000_base_x, an_enable,
			speed, full_duplex);
}

int al_eth_link_status_clear(struct al_hal_eth_adapter *adapter)
{
	MAC_OBJ_FUNC_EXISTS(link_status_clear);

	return adapter->mac_obj.link_status_clear(&adapter->mac_obj);
}

int al_eth_link_status_get(struct al_hal_eth_adapter *adapter,
	struct al_eth_link_status *status)
{
	int ret;

	MAC_OBJ_FUNC_EXISTS(link_status_get);

	ret = adapter->mac_obj.link_status_get(&adapter->mac_obj, status);

	al_dbg("[%s]: mac %s port. link_status: %s.\n", adapter->name,
		al_eth_mac_mode_str(adapter->mac_mode),
		(status->link_up == AL_TRUE) ? "LINK_UP" : "LINK_DOWN");

	return ret;
}

int al_eth_mdio_config(struct al_hal_eth_adapter *adapter, enum al_eth_mdio_type mdio_type,
	al_bool	shared_mdio_if, enum al_eth_ref_clk_freq ref_clk_freq,
	unsigned int mdio_clk_freq_khz)
{
	int ret;

	MAC_OBJ_FUNC_EXISTS(mdio_config);

	ret = adapter->mac_obj.mdio_config(&adapter->mac_obj, mdio_type, shared_mdio_if,
		ref_clk_freq, mdio_clk_freq_khz, AL_FALSE);

	if (!ret) {
		/** Set mdio values to eth adapter */
		adapter->mdio_if = adapter->mac_obj.mdio_if;
		adapter->shared_mdio_if = adapter->mac_obj.shared_mdio_if;
		adapter->mdio_type = adapter->mac_obj.mdio_type;
	}

	return ret;
}
int al_eth_mdio_config_ex(struct al_hal_eth_adapter *adapter,
	struct al_eth_mdio_config_params *params)
{
	int ret;

	MAC_OBJ_FUNC_EXISTS(mdio_config);

	ret = adapter->mac_obj.mdio_config(&adapter->mac_obj,
		params->mdio_type,
		params->shared_mdio_if,
		params->ref_clk_freq,
		params->mdio_clk_freq_khz,
		params->ignore_read_status);

	if (!ret) {
		/** Set mdio values to eth adapter */
		adapter->mdio_if = adapter->mac_obj.mdio_if;
		adapter->shared_mdio_if = adapter->mac_obj.shared_mdio_if;
		adapter->mdio_type = adapter->mac_obj.mdio_type;
	}

	return ret;
}

int al_eth_mdio_pol_config(struct al_hal_eth_adapter *adapter, enum al_eth_mdc_mdio_pol mdio_pol)
{
	MAC_OBJ_FUNC_EXISTS(mdio_pol_config);

	return adapter->mac_obj.mdio_pol_config(&adapter->mac_obj, mdio_pol);
}

int al_eth_mdio_read(struct al_hal_eth_adapter *adapter, uint32_t phy_addr, uint32_t device,
	uint32_t reg, uint16_t *val)
{
	MAC_OBJ_FUNC_EXISTS(mdio_read);

	return adapter->mac_obj.mdio_read(&adapter->mac_obj, phy_addr, device,
			reg, val);

}

int al_eth_mdio_write(struct al_hal_eth_adapter *adapter, uint32_t phy_addr, uint32_t device,
		uint32_t reg, uint16_t val)
{
	MAC_OBJ_FUNC_EXISTS(mdio_write);

	return adapter->mac_obj.mdio_write(&adapter->mac_obj, phy_addr, device,
			reg, val);
}

int al_eth_led_set(struct al_hal_eth_adapter *adapter, al_bool link_is_up)
{
	MAC_OBJ_FUNC_EXISTS(led_set);

	return adapter->mac_obj.led_set(&adapter->mac_obj, link_is_up);
}

int al_eth_mac_tx_flush_config(struct al_hal_eth_adapter *adapter, al_bool enable)
{
	int ret;

	MAC_OBJ_FUNC_EXISTS(tx_flush_config);

	al_dbg("eth [%s]: tx_flush %s\n", adapter->name, enable ? "enable" : "disable");

	ret = adapter->mac_obj.tx_flush_config(&adapter->mac_obj, enable);
	if (ret == -EPERM)
		al_err("eth [%s]: tx flush not supported in this mode %d\n",
			adapter->name, adapter->mac_mode);

	return ret;
}

int al_eth_mac_rate_counter_handle_init(struct al_hal_eth_adapter *adapter,
	struct al_eth_mac_rate_counter_handle *handle,
	struct al_eth_mac_rate_counter_handle_init_params *handle_init_params)
{
	int ret;

	MAC_OBJ_FUNC_EXISTS(rate_counter_handle_init);

	ret = adapter->mac_obj.rate_counter_handle_init(handle, handle_init_params);
	if (ret != 0)
		al_err("eth [%s]: rate counter handle init failed\n", __func__);

	return ret;
}

int al_eth_mac_rate_counter_config(struct al_eth_mac_rate_counter_handle *handle)
{
	int ret;

	MAC_OBJ_FUNC_EXISTS_HANDLE(rate_counter_config);

	ret = handle->mac_obj->rate_counter_config(handle);
	if (ret != 0)
		al_err("eth [%s]: rate counter config failed\n", __func__);

	return ret;
}

int al_eth_mac_rate_counter_start(struct al_eth_mac_rate_counter_handle *handle)
{
	int ret;

	MAC_OBJ_FUNC_EXISTS_HANDLE(rate_counter_start);

	ret = handle->mac_obj->rate_counter_start(handle);
	if (ret != 0)
		al_err("eth [%s]: rate counter start failed\n", __func__);

	return ret;
}

int al_eth_mac_rate_counter_stop(struct al_eth_mac_rate_counter_handle *handle)
{
	int ret;

	MAC_OBJ_FUNC_EXISTS_HANDLE(rate_counter_stop);

	ret = handle->mac_obj->rate_counter_stop(handle);
	if (ret != 0)
		al_err("eth [%s]: rate counter stop failed\n", __func__);

	return ret;
}

int al_eth_mac_rate_counter_get(struct al_eth_mac_rate_counter_handle *handle,
	uint32_t *bytes, uint32_t *frames)
{
	int ret;

	MAC_OBJ_FUNC_EXISTS_HANDLE(rate_counter_get);

	ret = handle->mac_obj->rate_counter_get(handle, bytes, frames);
	if (ret != 0)
		al_err("eth [%s]: rate counter get failed\n", __func__);

	return ret;
}

int al_eth_mac_rate_counter_print(struct al_eth_mac_rate_counter_handle *handle)
{
	int ret;

	MAC_OBJ_FUNC_EXISTS_HANDLE(rate_counter_print);

	ret = handle->mac_obj->rate_counter_print(handle);
	if (ret != 0)
		al_err("eth [%s]: rate counter print failed\n", __func__);

	return ret;
}

int al_eth_mac_iofic_attrs_get(struct al_eth_mac_obj *obj, int flags,
			       uint32_t *mac_a_mask, uint32_t *mac_b_mask,
			       uint32_t *mac_c_mask, uint32_t *mac_d_mask)
{
	uint32_t a_mask, b_mask, c_mask, d_mask;
	al_assert(obj);

	MAC_OBJ_FUNC_EXISTS_SELF(obj, iofic_attrs_get_hw);

	/* Always start with bits valid for the specific HW revision */
	obj->iofic_attrs_get_hw(obj, mac_a_mask, mac_b_mask,
				mac_c_mask, mac_d_mask);

	/* If requested, clear bits which belong to different software
	 * configurations */
	if (flags & AL_ETH_IOFIC_ATTRS_RUNTIME_CONFIG) {
		MAC_OBJ_FUNC_EXISTS_SELF(obj, iofic_attrs_get_runtime_inval);

		obj->iofic_attrs_get_runtime_inval(obj, &a_mask, &b_mask,
						   &c_mask, &d_mask);
		*mac_a_mask &= ~a_mask;
		*mac_b_mask &= ~b_mask;
		*mac_c_mask &= ~c_mask;
		*mac_d_mask &= ~d_mask;
	}

	/* If requested, clear bits which are unstable at startup */
	if (flags & AL_ETH_IOFIC_ATTRS_FILTER_TRANSIENT) {
		MAC_OBJ_FUNC_EXISTS_SELF(obj, iofic_attrs_get_transient);

		obj->iofic_attrs_get_transient(obj, &a_mask, &b_mask,
					       &c_mask, &d_mask);
		*mac_a_mask &= ~a_mask;
		*mac_b_mask &= ~b_mask;
		*mac_c_mask &= ~c_mask;
		*mac_d_mask &= ~d_mask;
	}

	/* If requested, clear bits which are intended to be queried only by
	 * polling (non fatal bits) */
	if (flags & AL_ETH_IOFIC_ATTRS_FATAL) {
		MAC_OBJ_FUNC_EXISTS_SELF(obj, iofic_attrs_get_non_fatal);

		obj->iofic_attrs_get_non_fatal(obj, &a_mask, &b_mask,
					       &c_mask, &d_mask);
		*mac_a_mask &= ~a_mask;
		*mac_b_mask &= ~b_mask;
		*mac_c_mask &= ~c_mask;
		*mac_d_mask &= ~d_mask;
	}

	return 0;
}

int al_eth_mac_start_stop_adv(struct al_hal_eth_adapter *adapter,
			      al_bool tx,
			      al_bool rx,
			      al_bool tx_start,
			      al_bool rx_start)
{
	MAC_OBJ_FUNC_EXISTS(mac_start_stop_adv);

	return adapter->mac_obj.mac_start_stop_adv(&adapter->mac_obj, tx, rx, tx_start, rx_start);
}

/** @} end of Ethernet group */
