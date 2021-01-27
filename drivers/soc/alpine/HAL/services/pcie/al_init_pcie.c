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
 *  PCIe init
 *  @{
 * @file   al_init_pcie.c
 *
 * @brief  PCIe initialization
 */

#include "al_init_pcie_params.h"
#include "al_hal_iomap.h"
#include "al_hal_serdes_interface.h"
#include "al_hal_pcie_interrupts.h"
#include "al_hal_pbs_utils.h"
#include "al_init_pcie_debug.h"
#include "al_hal_unit_adapter_regs.h"
#include "al_hal_sys_fabric_utils.h"
#include "al_hal_addr_map.h"

/*
 * time to wait after port disable/enable
 * On Alpine V3:
 * The delay is used to init the memory, the longest memory is 1280 in the pcie, to it takes 8ns
 * (pcie core clock before pcie_exist) * 1280 ~10.5usec.
 * However there's internal 2000 cycle delay of the core reset (to be on the safe side) so
 * additionalwe 8ns * 2000 = 16us.
 * The value of 2000 can be controlled through register field in the PCIe AXI wrapper regfile
 * CFG_DELAY_AFTER_PCIE_EXIST[10:0].
 * It is safe to wait 20us.
 */
#define PORT_DIS_EN_WAIT_US								\
	((init_pcie_params->port_params->fast_link_mode) ? 20 :				\
	((al_pbs_dev_id_get(init_pcie_params->pbs_reg_base) <=				\
	PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V2) ? 500 : 20))

#define LINK_FINAL_WAIT_MS (48) /* max time to wait for link up */

#define INIT_PCIE_CHECK_ADV(tear_down_label, print_err, ...)				\
	do {										\
		if (rc) {								\
			if (print_err) {						\
				al_err("%s: PCIe[%d] ", __func__, pcie_port->port_id);	\
				al_err(__VA_ARGS__);					\
				al_err("\n");						\
			}								\
			if (init_pcie_params->skip_teardown) {				\
				al_err("%s: PCIe[%d] teardown skipped!\n",		\
					__func__, pcie_port->port_id);			\
				return rc;						\
			} else {							\
				goto tear_down_label;					\
			}								\
		}									\
	} while (0)

#define INIT_PCIE_CHECK(tear_down_label, ...)						\
	INIT_PCIE_CHECK_ADV(tear_down_label, AL_TRUE, __VA_ARGS__)

__attribute__((weak, alias("__al_init_pcie_ex"))) int al_init_pcie_ex(
	struct al_init_pcie_handle *init_pcie_handle __attribute__ ((unused)),
	struct al_init_pcie_params *init_pcie_params);

static int __al_init_pcie_ex(
	struct al_init_pcie_handle *init_pcie_handle __attribute__ ((unused)),
	struct al_init_pcie_params *init_pcie_params)
{
	return init_pcie_params->ex_params != NULL;
}

/*
 * This function is relevant for ports which don't have direct access to io_fabric regs
 * (in ALPINE_V2 these are ports 2 & 3). We assert that the port_id is 3, since currently it
 * has been tested only for it.
 */
static void al_init_pcie_iof_regs_access_force(struct al_pcie_port *pcie_port,
					       void __iomem *nb_regs_base)
{
	struct al_sys_fabric_handle sys_fabric_handle;
	struct al_addr_map_tgt_enforcement_master_cfg_entry tgt_enforcement_master_cfg_entry;
	struct al_addr_map_tgt_enforcement_slave_cfg_entry tgt_enforcement_slave_cfg_entry;

	al_assert(pcie_port);
	al_assert(pcie_port->port_id == 3);
	al_assert(nb_regs_base);

	tgt_enforcement_master_cfg_entry.master = AL_ADDR_MAP_TGT_ENFORCEMENT_MASTER_PCIE_3;
	tgt_enforcement_master_cfg_entry.slv_cfg_entries = &tgt_enforcement_slave_cfg_entry;
	tgt_enforcement_master_cfg_entry.slv_cfg_entries_num = 1;

	tgt_enforcement_slave_cfg_entry.slave = AL_ADDR_MAP_TGT_ENFORCEMENT_SLV_DRAM;
	tgt_enforcement_slave_cfg_entry.axuser_19_16_val = 0x0;
	tgt_enforcement_slave_cfg_entry.axuser_19_16_mask = 0xf;

	al_sys_fabric_handle_init(&sys_fabric_handle, nb_regs_base);
	al_sys_fabric_iof_2_iof_path_en(&sys_fabric_handle, AL_TRUE);

	/* Force all inbound transactions to pcie port3 to be forwarded to the sys fabric */
	al_addr_map_tgt_enforcement_cfg_set(pcie_port->pbs_regs,
					    &tgt_enforcement_master_cfg_entry,
					    1);
}

int al_init_pcie(struct al_init_pcie_handle *init_pcie_handle,
		struct al_init_pcie_params *init_pcie_params,
		struct al_pcie_link_status *link_status)
{
	int rc = 0;
	unsigned int i;
	struct al_pcie_port *pcie_port = &(init_pcie_handle->pcie_port);
	struct al_serdes_grp_obj *serdes_grp;

	if (!init_pcie_params->pcie_reg_base)
		init_pcie_params->pcie_reg_base =
			(void *)AL_SB_PCIE_BASE((long)init_pcie_params->port_id);
	if (!init_pcie_params->pbs_reg_base)
		init_pcie_params->pbs_reg_base = (void *)AL_PBS_REGFILE_BASE;
	if (!init_pcie_params->nb_regs_base)
		init_pcie_params->nb_regs_base = (void *)AL_NB_SERVICE_BASE;

	rc = al_pcie_port_handle_init(pcie_port,
		init_pcie_params->pcie_reg_base,
		init_pcie_params->pbs_reg_base,
		init_pcie_params->port_id);
	INIT_PCIE_CHECK(init_done, "could not initialize port handle");

	rc = al_pcie_link_status(pcie_port, link_status);
	INIT_PCIE_CHECK(init_done, "could not check link status upon init");

	if (link_status->ltssm_state) {
		if (init_pcie_params->reinit_link) {
			rc = al_pcie_link_stop(pcie_port);
			INIT_PCIE_CHECK(init_shutdown_mem, "could not stop link upon init");

			al_data_memory_barrier();
			rc = al_pcie_port_memory_shutdown_set(pcie_port, AL_TRUE);
			INIT_PCIE_CHECK(init_port_disable, "could not shutdown memory upon init");
			al_data_memory_barrier();

			al_pcie_port_disable(pcie_port);
			al_udelay(PORT_DIS_EN_WAIT_US);
		} else {
			rc = -EINVAL;
			INIT_PCIE_CHECK(init_done, "link already started");
		}
	}

	rc = al_pcie_port_enable(pcie_port);
	INIT_PCIE_CHECK(init_done, "could not enable port before shutdown");
	al_udelay(PORT_DIS_EN_WAIT_US);

	al_data_memory_barrier();
	rc = al_pcie_port_memory_shutdown_set(pcie_port, AL_FALSE);
	INIT_PCIE_CHECK(init_port_disable, "could not disable shutdown memory");
	al_data_memory_barrier();

	al_pcie_port_disable(pcie_port);

	rc = al_pcie_port_operating_mode_config(pcie_port, init_pcie_params->mode);
	INIT_PCIE_CHECK(init_shutdown_mem, "could not set mode[%d]", init_pcie_params->mode);

	rc = al_pcie_port_max_lanes_set(pcie_port, init_pcie_params->max_lanes);
	INIT_PCIE_CHECK(init_shutdown_mem,
		"could not set max_lanes[%d]", init_pcie_params->max_lanes);

	rc = al_pcie_port_max_num_of_pfs_set(pcie_port, init_pcie_params->max_num_of_pfs);
	INIT_PCIE_CHECK(init_shutdown_mem,
		"could not set max_num_of_pfs[%d]", init_pcie_params->max_num_of_pfs);

	if (init_pcie_params->reads_config) {
		rc = al_pcie_port_ib_hcrd_os_ob_reads_config(pcie_port,
			init_pcie_params->reads_config);
		INIT_PCIE_CHECK(init_shutdown_mem, "could not configure IB/OB reads");
	}

	rc = al_pcie_port_enable(pcie_port);
	INIT_PCIE_CHECK(init_shutdown_mem, "could not enable port");

	/* Wait more than 2000 clock cycles */
	al_udelay(PORT_DIS_EN_WAIT_US);

	/* serdes groups interrupt enable */
	if (init_pcie_params->serdes_grp_objs_num && init_pcie_params->serdes_grp_objs) {
		al_bool ical_config = init_pcie_params->skip_serdes_ical == AL_FALSE;

		for (i = 0; i < init_pcie_params->serdes_grp_objs_num; i++) {
			serdes_grp = init_pcie_params->serdes_grp_objs[i];
			al_assert(serdes_grp);
			serdes_grp->fw_init_status_get(serdes_grp, NULL);
			if (ical_config)
				serdes_grp->pcie_ical_config_begin(serdes_grp);
		}

		while (ical_config) {
			ical_config = AL_FALSE;
			for (i = 0; i < init_pcie_params->serdes_grp_objs_num; i++) {
				serdes_grp = init_pcie_params->serdes_grp_objs[i];
				if (serdes_grp->pcie_ical_config_iterate(serdes_grp))
					ical_config = AL_TRUE;
			}
		}

		for (i = 0; i < init_pcie_params->serdes_grp_objs_num; i++) {
			serdes_grp = init_pcie_params->serdes_grp_objs[i];
			serdes_grp->pcs_interrupt_enable_set(serdes_grp, AL_TRUE);
		}
	}

	rc = al_pcie_port_config(pcie_port, init_pcie_params->port_params);
	INIT_PCIE_CHECK(init_shutdown_mem, "could not configure port");

	if (init_pcie_params->iof_regs_access_en) {
		/* iof_regs_access_en can be set to true only when in EP mode */
		al_assert(init_pcie_params->mode == AL_PCIE_OPERATING_MODE_EP);
		al_init_pcie_iof_regs_access_force(pcie_port,
						   init_pcie_params->nb_regs_base);
	}

	if (init_pcie_params->mode == AL_PCIE_OPERATING_MODE_RC) {
		for (i = 0; i < AL_MAX_NUM_OF_PFS; i++) {
			rc = (init_pcie_params->pf_params[i] != NULL);
			INIT_PCIE_CHECK(init_shutdown_mem,
				"pf_params[%d] should be NULL in RC mode", i);
		}
		rc = (init_pcie_params->ex_params != NULL);
		INIT_PCIE_CHECK(init_shutdown_mem, "extended params are not supported in RC mode");
	} else if (init_pcie_params->mode == AL_PCIE_OPERATING_MODE_EP) {
		struct al_pcie_pf *pcie_pf;

		for (i = 0; i < init_pcie_params->max_num_of_pfs; i++) {
			al_assert(init_pcie_params->pf_params[i] != NULL);

			pcie_pf = &init_pcie_handle->pcie_pf[i];
			rc = al_pcie_pf_handle_init(pcie_pf, pcie_port, i);
			INIT_PCIE_CHECK(init_shutdown_mem, "could not initialize pf[%d] handle", i);

			rc = al_pcie_pf_config(pcie_pf, init_pcie_params->pf_params[i]);
			INIT_PCIE_CHECK(init_shutdown_mem, "could not configure pf[%d]", i);
		}

		rc = al_init_pcie_ex(init_pcie_handle, init_pcie_params);
		INIT_PCIE_CHECK(init_shutdown_mem, "could not perform extended init");
	} else {
		al_assert(0);
	}

	if (init_pcie_params->crrs_en)
		al_pcie_app_req_retry_set(pcie_port, AL_TRUE);

	if (!init_pcie_params->start_link)
		return rc;

	rc = al_pcie_link_start(pcie_port);
	INIT_PCIE_CHECK(init_shutdown_mem, "could not start link");

	if (!init_pcie_params->wait_for_link_timeout_ms)
		return rc;

	rc = al_pcie_link_up_wait_ex(pcie_port, init_pcie_params->wait_for_link_timeout_ms,
		init_pcie_params->retrain_if_not_full_width);
	INIT_PCIE_CHECK_ADV(
		init_stop_link, !init_pcie_params->wait_for_link_silent_fail, "link is not up");

	/* Wait till link speed is the max speed */
	for (i = 0; i < (LINK_FINAL_WAIT_MS * 20); i++) {
		rc = al_pcie_link_status(pcie_port, link_status);
		INIT_PCIE_CHECK(init_stop_link, "could not check link status");

		if (link_status->speed == init_pcie_params->port_params->link_params->max_speed)
			break;
		else
			al_udelay(50);
	}

	if (link_status->link_up) {
		return rc;
	} else {
		rc = -EINVAL;
		INIT_PCIE_CHECK(init_stop_link, "could not link up port");
	}

init_stop_link:
	al_pcie_link_stop(pcie_port);
init_shutdown_mem:
	al_pcie_port_memory_shutdown_set(pcie_port, AL_TRUE);
init_port_disable:
	al_pcie_port_disable(pcie_port);
init_done:
	return rc;
}

static void al_pcie_port_cfg_space_write(struct al_init_pcie_handle *pcie_handle,
		unsigned int pf_idx,
		struct al_pcie_port_pf_init_params *pf_params)
{
	/* pcie_pf_handle_init already called in al_init_pcie */
	struct al_pcie_pf *pcie_pf = &pcie_handle->pcie_pf[pf_idx];
	struct al_pcie_ep_id_params *cfg_data = &pf_params->cfg_space;

	if (!pf_params->status)
		return;

	al_pcie_local_cfg_space_write(pcie_pf,
		AL_PCI_DEV_ID_VEN_ID >> 2,
		(cfg_data->device_id <<
			AL_PCI_DEV_ID_VEN_ID_DEV_ID_SHIFT) |
		(cfg_data->vendor_id <<
			AL_PCI_DEV_ID_VEN_ID_VEN_ID_SHIFT),
		AL_FALSE,
		AL_TRUE);

	al_pcie_local_cfg_space_write(
		pcie_pf,
		PCI_CLASS_REVISION >> 2,
		(cfg_data->class_code <<
			PCI_CLASS_REVISION_CLASS_SHIFT) |
		(cfg_data->revision_id <<
			PCI_CLASS_REVISION_REVISION_SHIFT),
		AL_FALSE,
		AL_TRUE);
}

static int al_pcie_port_bar_atu_init(
	struct al_pcie_port *pcie_port,
	uint64_t target_addr,
	unsigned int bar_idx,
	unsigned int pf_idx,
	uint8_t atu_idx)
{
	struct al_pcie_atu_region atu_region;
	int err = 0;

	al_dbg("PF %d Configuring bar %d\n", pf_idx, bar_idx);

	/* Inbound ATUs */
	al_memset(&atu_region, 0, sizeof(atu_region));
	/**
	 * The priority of the window match is arbitrarily set by the bar number.
	 * bar0 is the most prioritize.
	 * This way ensure unique index per bar.
	 */
	atu_region.index = atu_idx;
	atu_region.bar_number = bar_idx;
	atu_region.enable = AL_TRUE;
	atu_region.direction = AL_PCIE_ATU_DIR_INBOUND;
	atu_region.invert_matching = AL_FALSE;
	atu_region.tlp_type = AL_PCIE_TLP_TYPE_MEM;
	atu_region.match_mode = 1; /* BAR match mode */
	atu_region.attr = 0; /* not used */
	atu_region.msg_code = 0; /* not used */
	atu_region.cfg_shift_mode = AL_FALSE;
	atu_region.target_addr = target_addr;
	atu_region.enable_attr_match_mode = AL_FALSE;
	atu_region.enable_msg_match_mode = AL_FALSE;
	atu_region.function_match_bypass_mode = AL_TRUE;
	atu_region.function_match_bypass_mode_number = pf_idx;

	err = al_pcie_atu_region_set(pcie_port, &atu_region);
	if (err)
		al_err("al_pcie_atu_region_set() failed\n");

	return err;
}

static int al_pcie_port_atu_init(
	struct al_pcie_port *pcie_port,
	unsigned int pf_idx,
	struct al_pcie_port_pf_init_params *pf_param,
	uint8_t *atu_idx)
{
	unsigned int bar;
	int ret;

	for (bar = 0; bar < AL_PCIE_PORT_PF_INIT_PARAMS_NUM_BARS; bar++) {
		/* Initialize ATUs */
		if (!pf_param->bars[bar].enable)
			continue;

		ret = al_pcie_port_bar_atu_init(pcie_port, pf_param->target_addr[bar], bar, pf_idx,
						*atu_idx);
		if (ret)
			return ret;

		(*atu_idx)++;
	}

	return 0;
}

static void al_pcie_port_pf_init(struct al_pcie_pf_config_params *pf_params,
				 struct al_pcie_port_pf_init_params *pf_init_params,
				 unsigned int port_idx,
				 unsigned int pf,
				 unsigned int *pf_count)
{
	unsigned int bar;

	if (!pf_init_params->status) {
		al_dbg("%s: PCIE port %d pf %d disabled\n", __func__, port_idx, pf);
		pf_params->bar_params_valid = AL_FALSE;
		return;
	}

	(*pf_count)++;

	for (bar = 0; bar < AL_PCIE_PORT_PF_INIT_PARAMS_NUM_BARS; bar++) {
		al_dbg("%s: PCIE port %d pf %d bar %d: ", __func__, port_idx, pf, bar);
		if (!pf_init_params->bars[bar].enable) {
			al_dbg("disabled\n");
			pf_params->bar_params[bar].enable = AL_FALSE;
		} else {
			al_dbg("enabled\n");
			pf_params->bar_params_valid = AL_TRUE;
			pf_params->bar_params[bar] = pf_init_params->bars[bar];
		}
	}
}

int al_pcie_init_adv(struct al_init_pcie_handle *pcie_handle,
		 struct al_pcie_init_params_adv *pcie_params,
		 struct al_pcie_link_status *link_status,
		 int port_idx,
		 char *current_stage,
		 al_bool *performed)
{
	struct al_init_pcie_params init_pcie_params;
	unsigned int i;
	unsigned int j;
	unsigned int pf;
	int ret;
	struct al_pcie_port_pf_init_params pf_param;
	unsigned int pf_count = 0;

	al_assert(pcie_handle);
	al_assert(pcie_params);
	al_assert(link_status);
	al_assert(current_stage);

	/* Assuming we will perform init */
	*performed = AL_TRUE;

	if ((!pcie_params->port_params[port_idx].port_init_found) &&
	    (pcie_params->port_params[port_idx].ep == AL_TRUE)) {
		/* If we didn't find port_init_stage and we are ep, we don't want to init port */
		*performed = AL_FALSE;
		return 0;
	}

	if (al_strcmp(pcie_params->port_params[port_idx].port_init_stage, current_stage) != 0) {
		/* If port-init-stage isn't equal to current stage, we don't want to init port */
		*performed = AL_FALSE;
		return 0;
	}

	init_pcie_params = default_init_pcie_params;
	init_pcie_params.port_id = port_idx;
	init_pcie_params.mode = pcie_params->port_params[port_idx].ep ?
				AL_PCIE_OPERATING_MODE_EP : AL_PCIE_OPERATING_MODE_RC;
	init_pcie_params.iof_regs_access_en = pcie_params->port_params[port_idx].iof_regs_access_en;
	init_pcie_params.port_params->link_params->max_speed =
				pcie_params->port_params[port_idx].max_speed;
	init_pcie_params.max_lanes = pcie_params->port_params[port_idx].num_lanes;
#if (AL_DEV_ID == AL_DEV_ID_ALPINE_V3)
	init_pcie_params.serdes_grp_objs = pcie_params->port_params[port_idx].serdes_lanes;
	init_pcie_params.serdes_grp_objs_num = init_pcie_params.max_lanes;
#endif
	if (pcie_params->port_params[port_idx].ep) {
		init_pcie_params.reinit_link = AL_FALSE;
		init_pcie_params.wait_for_link_timeout_ms = 0;
		init_pcie_params.crrs_en = AL_FALSE;
		init_pcie_params.pcie_reg_base = (void *)(uintptr_t)AL_SB_PCIE_BASE(port_idx);
		init_pcie_params.pbs_reg_base = (void *)AL_PBS_REGFILE_BASE;
		init_pcie_params.port_params->gen3_params->eq_redo_bypass = AL_TRUE;

		/* Check if cfg-space init stage is not null */
		if ((pcie_params->port_params[port_idx].cfg_space_init_stage != NULL) &&
		    (al_strcmp(pcie_params->port_params[port_idx].cfg_space_init_stage,
			   current_stage) == 0)) {
			for (pf = 0; pf < AL_MAX_NUM_OF_PFS; pf++) {
				struct al_pcie_port_pf_init_params pf_param =
					pcie_params->port_params[port_idx].pf_params[pf];

				al_pcie_port_pf_init(init_pcie_params.pf_params[pf],
						     &pf_param, port_idx, pf, &pf_count);
			}
		}
		init_pcie_params.max_num_of_pfs = pf_count;
	} else {
		init_pcie_params.wait_for_link_timeout_ms = pcie_params->pcie_rc_link_up_timeout;
		init_pcie_params.wait_for_link_silent_fail = AL_TRUE;
		init_pcie_params.skip_teardown = !pcie_params->teardown;
#if (AL_DEV_ID == AL_DEV_ID_ALPINE_V3)
		init_pcie_params.skip_serdes_ical = pcie_params->skip_serdes_ical;
#endif
		init_pcie_params.reinit_link = pcie_params->reinit;
		init_pcie_params.retrain_if_not_full_width =
				pcie_params->pcie_rc_retrain_if_not_full_width;

		for (j = 0; j < AL_MAX_NUM_OF_PFS; j++)
			init_pcie_params.pf_params[j] = NULL;
	}

	al_init_pcie_print_params(&init_pcie_params);

	ret = al_init_pcie(pcie_handle, &init_pcie_params, link_status);
	if (ret) {
		al_dbg("%s: al_init_pcie failed for PCIe port %d\n", __func__, port_idx);
		return ret;
	}

	if ((init_pcie_params.mode == AL_PCIE_OPERATING_MODE_EP) &&
	    (al_strcmp(pcie_params->port_params[port_idx].cfg_space_init_stage,
		       current_stage) == 0)) {
		uint8_t atu_idx = 0;

		for (i = 0; i < AL_MAX_NUM_OF_PFS; i++) {
			pf_param = pcie_params->port_params[port_idx].pf_params[i];
			if (!pf_param.status)
				continue;

			/* Initialize PCIE port config space */
			al_pcie_port_cfg_space_write(pcie_handle, i, &pf_param);

			ret = al_pcie_port_atu_init(&pcie_handle->pcie_port, i, &pf_param,
						    &atu_idx);
			if (ret) {
				al_err("%s: failed to init ATUs for PCIE port %d PF %d\n",
				       __func__, port_idx, i);
				return ret;
			}

			if (!pf_param.msix_enable)
				continue;

			ret = al_pcie_msix_config(&pcie_handle->pcie_pf[i], &pf_param.msix);
			if (ret) {
				al_err("%s: failed to configure the MSIX for PCIE port %d PF %d\n",
				       __func__, port_idx, i);
				return ret;
			}
		}
	}

	return 0;
}

