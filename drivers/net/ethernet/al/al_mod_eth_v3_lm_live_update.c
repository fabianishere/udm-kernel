/*
 * Copyright 2018, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */
#include "al_mod_hal_live_update.h"
#include "al_mod_hal_live_update_internal.h"
#include "al_mod_eth_v3_lm_live_update.h"
#include "al_mod_init_eth_lm.h"

#define AL_LU_ETH_PACKED_MAGIC		(0xCAFE)
#define AL_LU_ETH_LM_V3_PACKED_ID	(0x220)

struct al_mod_serdes_adv_tx_params_packed_v0 {
	int	override;
	uint8_t	amp;
	uint8_t	total_driver_units;
	uint8_t	c_plus_1;
	uint8_t	c_plus_2;
	uint8_t	c_minus_1;
	uint8_t	slew_rate;
} __packed;

struct al_mod_serdes_adv_rx_params_packed_v0 {
	int	override;
	uint8_t	dcgain;
	uint8_t	dfe_3db_freq;
	uint8_t	dfe_gain;
	uint8_t	dfe_first_tap_ctrl;
	uint8_t	dfe_secound_tap_ctrl;
	uint8_t	dfe_third_tap_ctrl;
	uint8_t	dfe_fourth_tap_ctrl;
	uint8_t	low_freq_agc_gain;
	uint8_t	precal_code_sel;
	uint8_t	high_freq_agc_boost;
} __packed;

struct al_mod_eth_an_adv_packed_v0 {
	uint8_t		selector_field;
	uint8_t		echoed_nonce;
	uint8_t		capability;
	uint8_t		remote_fault;
	uint8_t		acknowledge;
	uint8_t		next_page;
	uint8_t		transmitted_nonce;
	uint32_t	technology;
	uint8_t		fec_capability;
} __packed;

struct al_mod_eth_lm_step_data_packed_v0 {
	int		detection_state;
	int		establish_state;
	int		rx_adap_state;
	int		slc_state;
	int		wfl_state;
	unsigned int	wfl_last_time_checked;
	unsigned int	start_time;
} __packed;

struct al_mod_eth_lm_retimer_channel_status_packed_v0 {
	int	signal_detect;
	int	cdr_lock;
	int	first_check;
} __packed;

struct al_mod_hal_eth_lm_v3_packed_v0 {
	int				lane; /* enum al_mod_serdes_lane */
	uint32_t			link_training_failures;
	int				tx_param_dirty;
	int				serdes_tx_params_valid;
	struct al_mod_serdes_adv_tx_params_packed_v0	tx_params_override;
	int				rx_param_dirty;
	int				serdes_rx_params_valid;
	struct al_mod_serdes_adv_rx_params_packed_v0	rx_params_override;
	struct al_mod_eth_an_adv_packed_v0	local_adv;
	struct al_mod_eth_an_adv_packed_v0	partner_adv;
	int				mode; /* enum al_mod_eth_lm_link_mode */
	uint8_t				da_len;
	int				debug;
	int				sfp_detection;
	uint8_t				sfp_bus_id;
	uint8_t				sfp_i2c_addr;
	int				default_mode; /* enum al_mod_eth_lm_link_mode */
	uint8_t				default_dac_len;
	int				link_training;
	int				rx_equal;
	int				static_values;
	int				retimer_channel; /* enum al_mod_eth_retimer_channel */
	int				auto_fec_enable;
	unsigned int			auto_fec_initial_timeout;
	unsigned int			auto_fec_toggle_timeout;
	uint32_t			gpio_present;
	int				retimer_tx_channel; /* enum al_mod_eth_retimer_channel */
	int				retimer_configured;
	int				max_speed; /* enum al_mod_eth_lm_max_speed */
	int				sfp_detect_force_mode;
	int				speed_detection;
	int				link_state; /* enum al_mod_eth_lm_link_state */
	int				auto_fec_state; /* enum al_mod_auto_fec_state */
	unsigned int			fec_state_last_time_toggled;
	unsigned int			auto_fec_wait_to_toggle;
	int				link_prev_up;
	int				last_detected_mode; /* enum al_mod_eth_lm_link_mode */
	int				speed_change;
	struct al_mod_eth_lm_step_data_packed_v0	step_data;
	struct al_mod_eth_lm_retimer_channel_status_packed_v0	retimer_rx_channel_last_status;
	struct al_mod_eth_lm_retimer_channel_status_packed_v0	retimer_tx_channel_last_status;
} __packed;

struct al_mod_hal_eth_lm_v3_packed {
	int32_t		ver; /* enum al_mod_lu_hal_packed_ver */
	uint16_t	magic;
	uint16_t	id;
	uint16_t	idx;
	uint16_t	data_len;
	int8_t		active_buff;  /* Indicates which of the 2 buffers in the union is active */
	uint8_t		pad[3];
	uint8_t		reserved[16];
	char packed[AL_LU_ETH_LM_V3_PACKED_BUFFER_COUNT][AL_LU_ETH_LM_V3_PACKED_DATA_LEN];
} __packed;

/*
 * These functions have to check what is the current active buffer,
 * update the not active, and set it to active
 * Note: The version in the tuple, is the source version to convert from
 */
static struct al_mod_lu_ver_to_func_tuple eth_lm_v3_convert_up_funcs[0] = {
};
static struct al_mod_lu_ver_to_func_tuple eth_lm_v3_convert_down_funcs[0] = {
};

static enum al_mod_lu_hal_packed_ver eth_lm_v3_get_ver_func(void *packed)
{
	return ((struct al_mod_hal_eth_lm_v3_packed *)packed)->ver;
}

/* Internal structs packing/unpacking */
static void
al_mod_serdes_adv_tx_params_pack_v0(struct al_mod_serdes_adv_tx_params_packed_v0 *packed,
				const struct al_mod_serdes_adv_tx_params *src)
{
	packed->override = src->override;
	packed->amp = src->amp;
	packed->total_driver_units = src->total_driver_units;
	packed->c_plus_1 = src->c_plus_1;
	packed->c_plus_2 = src->c_plus_2;
	packed->c_minus_1 = src->c_minus_1;
	packed->slew_rate = src->slew_rate;
}

static void
al_mod_serdes_adv_tx_params_unpack_v0(struct al_mod_serdes_adv_tx_params *dst,
				  const struct al_mod_serdes_adv_tx_params_packed_v0 *packed)
{
	dst->override = !!packed->override;
	dst->amp = packed->amp;
	dst->total_driver_units = packed->total_driver_units;
	dst->c_plus_1 = packed->c_plus_1;
	dst->c_plus_2 = packed->c_plus_2;
	dst->c_minus_1 = packed->c_minus_1;
	dst->slew_rate = packed->slew_rate;
}

static void
al_mod_serdes_adv_rx_params_pack_v0(struct al_mod_serdes_adv_rx_params_packed_v0 *packed,
				const struct al_mod_serdes_adv_rx_params *src)
{
	packed->override = src->override;
	packed->dcgain = src->dcgain;
	packed->dfe_3db_freq = src->dfe_3db_freq;
	packed->dfe_gain = src->dfe_gain;
	packed->dfe_first_tap_ctrl = src->dfe_first_tap_ctrl;
	packed->dfe_secound_tap_ctrl = src->dfe_secound_tap_ctrl;
	packed->dfe_third_tap_ctrl = src->dfe_third_tap_ctrl;
	packed->dfe_fourth_tap_ctrl = src->dfe_fourth_tap_ctrl;
	packed->low_freq_agc_gain = src->low_freq_agc_gain;
	packed->precal_code_sel = src->precal_code_sel;
	packed->high_freq_agc_boost = src->high_freq_agc_boost;
}

static void
al_mod_serdes_adv_rx_params_unpack_v0(struct al_mod_serdes_adv_rx_params *dst,
				  const struct al_mod_serdes_adv_rx_params_packed_v0 *packed)
{
	dst->override = !!packed->override;
	dst->dcgain = packed->dcgain;
	dst->dfe_3db_freq = packed->dfe_3db_freq;
	dst->dfe_gain = packed->dfe_gain;
	dst->dfe_first_tap_ctrl = packed->dfe_first_tap_ctrl;
	dst->dfe_secound_tap_ctrl = packed->dfe_secound_tap_ctrl;
	dst->dfe_third_tap_ctrl = packed->dfe_third_tap_ctrl;
	dst->dfe_fourth_tap_ctrl = packed->dfe_fourth_tap_ctrl;
	dst->low_freq_agc_gain = packed->low_freq_agc_gain;
	dst->precal_code_sel = packed->precal_code_sel;
	dst->high_freq_agc_boost = packed->high_freq_agc_boost;
}

static void
al_mod_eth_an_adv_pack_v0(struct al_mod_eth_an_adv_packed_v0 *packed,
		      const struct al_mod_eth_an_adv *src)
{
	packed->selector_field = src->selector_field;
	packed->echoed_nonce = src->echoed_nonce;
	packed->capability = src->capability;
	packed->remote_fault = src->remote_fault;
	packed->acknowledge = src->acknowledge;
	packed->next_page = src->next_page;
	packed->transmitted_nonce = src->transmitted_nonce;
	packed->technology = src->technology;
	packed->fec_capability = src->fec_capability;
}

static void
al_mod_eth_an_adv_unpack_v0(struct al_mod_eth_an_adv *dst,
			const struct al_mod_eth_an_adv_packed_v0 *packed)
{
	dst->selector_field = packed->selector_field;
	dst->echoed_nonce = packed->echoed_nonce;
	dst->capability = packed->capability;
	dst->remote_fault = packed->remote_fault;
	dst->acknowledge = packed->acknowledge;
	dst->next_page = packed->next_page;
	dst->transmitted_nonce = packed->transmitted_nonce;
	dst->technology = packed->technology;
	dst->fec_capability = packed->fec_capability;
}

static void
al_mod_eth_lm_step_data_pack_v0(struct al_mod_eth_lm_step_data_packed_v0 *packed,
			    const struct al_mod_eth_lm_step_data *src)
{
	packed->detection_state = src->detection_state;
	packed->establish_state = src->establish_state;
	packed->rx_adap_state = src->retimer_data.rx_adap_state;
	packed->slc_state = src->retimer_data.slc_state;
	packed->wfl_state = src->retimer_data.wfl_state;
	packed->wfl_last_time_checked = src->retimer_data.wfl_last_time_checked;
	packed->start_time = src->start_time;
}

static void
al_mod_eth_lm_step_data_unpack_v0(struct al_mod_eth_lm_step_data *dst,
			      const struct al_mod_eth_lm_step_data_packed_v0 *packed)
{
	dst->detection_state = packed->detection_state;
	dst->establish_state = packed->establish_state;
	dst->retimer_data.rx_adap_state = packed->rx_adap_state;
	dst->retimer_data.slc_state = packed->slc_state;
	dst->retimer_data.wfl_state = packed->wfl_state;
	dst->retimer_data.wfl_last_time_checked = packed->wfl_last_time_checked;
	dst->start_time = packed->start_time;
}

static void
al_mod_eth_lm_retimer_channel_status_pack_v0(struct al_mod_eth_lm_retimer_channel_status_packed_v0 *packed,
					 const struct al_mod_eth_lm_retimer_channel_status *src)
{
	packed->signal_detect = src->signal_detect;
	packed->cdr_lock = src->cdr_lock;
	packed->first_check = src->first_check;
}

static void
al_mod_eth_lm_retimer_channel_status_unpack_v0(struct al_mod_eth_lm_retimer_channel_status *dst,
					   const struct al_mod_eth_lm_retimer_channel_status_packed_v0 *packed)
{
	dst->signal_detect = packed->signal_detect;
	dst->cdr_lock = packed->cdr_lock;
	dst->first_check = packed->first_check;
}

/*
 * Pack a struct al_mod_eth_lm_context into a struct al_mod_hal_eth_lm_v3_packed_v0
 */
static int al_mod_eth_lm_v3_pack_v0(struct al_mod_hal_eth_lm_v3_packed *lm_packed,
				const struct al_mod_eth_lm_context *eth_lm)
{
	struct al_mod_hal_eth_lm_v3_packed_v0 *packed_v0 =
		(struct al_mod_hal_eth_lm_v3_packed_v0 *)lm_packed->packed[lm_packed->active_buff];

	/* Pack the fields */
	packed_v0->lane = eth_lm->lane;
	packed_v0->link_training_failures = eth_lm->link_training_failures;
	packed_v0->tx_param_dirty = eth_lm->tx_param_dirty;
	packed_v0->serdes_tx_params_valid = eth_lm->serdes_tx_params_valid;

	al_mod_serdes_adv_tx_params_pack_v0(&packed_v0->tx_params_override,
					&eth_lm->tx_params_override);

	packed_v0->rx_param_dirty = eth_lm->rx_param_dirty;
	packed_v0->serdes_rx_params_valid = eth_lm->serdes_rx_params_valid;

	al_mod_serdes_adv_rx_params_pack_v0(&packed_v0->rx_params_override,
					&eth_lm->rx_params_override);

	al_mod_eth_an_adv_pack_v0(&packed_v0->local_adv, &eth_lm->local_adv);
	al_mod_eth_an_adv_pack_v0(&packed_v0->partner_adv, &eth_lm->partner_adv);

	packed_v0->mode = eth_lm->mode;
	packed_v0->da_len = eth_lm->da_len;
	packed_v0->debug = eth_lm->debug;
	packed_v0->sfp_detection = eth_lm->sfp_detection;
	packed_v0->sfp_bus_id = eth_lm->sfp_bus_id;
	packed_v0->sfp_i2c_addr = eth_lm->sfp_i2c_addr;
	packed_v0->default_mode = eth_lm->default_mode;
	packed_v0->default_dac_len = eth_lm->default_dac_len;
	packed_v0->link_training = eth_lm->link_training;
	packed_v0->rx_equal = eth_lm->rx_equal;
	packed_v0->static_values = eth_lm->static_values;

	packed_v0->retimer_channel = eth_lm->retimer_channel;
	packed_v0->auto_fec_enable = eth_lm->auto_fec_enable;
	packed_v0->auto_fec_initial_timeout = eth_lm->auto_fec_initial_timeout;
	packed_v0->auto_fec_toggle_timeout = eth_lm->auto_fec_toggle_timeout;

	packed_v0->gpio_present = eth_lm->gpio_present;
	packed_v0->retimer_tx_channel = eth_lm->retimer_tx_channel;
	packed_v0->retimer_configured = eth_lm->retimer_configured;
	packed_v0->max_speed = eth_lm->max_speed;
	packed_v0->sfp_detect_force_mode = eth_lm->sfp_detect_force_mode;
	packed_v0->speed_detection = eth_lm->speed_detection;
	packed_v0->link_state = eth_lm->link_state;

	packed_v0->auto_fec_state = eth_lm->auto_fec_state;
	packed_v0->fec_state_last_time_toggled =
		eth_lm->fec_state_last_time_toggled;
	packed_v0->auto_fec_wait_to_toggle = eth_lm->auto_fec_wait_to_toggle;
	packed_v0->link_prev_up = eth_lm->link_prev_up;
	packed_v0->last_detected_mode = eth_lm->last_detected_mode;
	packed_v0->speed_change = eth_lm->speed_change;

	al_mod_eth_lm_step_data_pack_v0(&packed_v0->step_data, &eth_lm->step_data);

	al_mod_eth_lm_retimer_channel_status_pack_v0(&packed_v0->retimer_rx_channel_last_status,
						 &eth_lm->retimer_rx_channel_last_status);

	al_mod_eth_lm_retimer_channel_status_pack_v0(&packed_v0->retimer_tx_channel_last_status,
						 &eth_lm->retimer_tx_channel_last_status);

	return 0;
}

/*
 * Unpack a V0 packed struct into a struct al_mod_eth_lm_context
 */
static int al_mod_eth_lm_v3_unpack_v0(struct al_mod_eth_lm_context *eth_lm,
				  const struct al_mod_hal_eth_lm_v3_packed *lm_packed)
{
	const struct al_mod_hal_eth_lm_v3_packed_v0 *packed_v0 =
		(const struct al_mod_hal_eth_lm_v3_packed_v0 *)lm_packed->packed[lm_packed->active_buff];

	al_mod_static_assert(sizeof(struct al_mod_hal_eth_lm_v3_packed_v0) <
			AL_LU_ETH_LM_V3_PACKED_DATA_LEN, "Packed v0 struct to big");

	eth_lm->lane = packed_v0->lane;
	eth_lm->link_training_failures = packed_v0->link_training_failures;
	eth_lm->tx_param_dirty = !!packed_v0->tx_param_dirty;
	eth_lm->serdes_tx_params_valid = !!packed_v0->serdes_tx_params_valid;

	al_mod_serdes_adv_tx_params_unpack_v0(&eth_lm->tx_params_override,
					  &packed_v0->tx_params_override);

	eth_lm->rx_param_dirty = !!packed_v0->rx_param_dirty;
	eth_lm->serdes_rx_params_valid = !!packed_v0->serdes_rx_params_valid;

	al_mod_serdes_adv_rx_params_unpack_v0(&eth_lm->rx_params_override,
					  &packed_v0->rx_params_override);

	al_mod_eth_an_adv_unpack_v0(&eth_lm->local_adv, &packed_v0->local_adv);
	al_mod_eth_an_adv_unpack_v0(&eth_lm->partner_adv, &packed_v0->partner_adv);

	eth_lm->mode = packed_v0->mode;
	eth_lm->da_len = packed_v0->da_len;
	eth_lm->debug = !!packed_v0->debug;
	eth_lm->sfp_detection = !!packed_v0->sfp_detection;
	eth_lm->sfp_bus_id = packed_v0->sfp_bus_id;
	eth_lm->sfp_i2c_addr = packed_v0->sfp_i2c_addr;
	eth_lm->default_mode = packed_v0->default_mode;
	eth_lm->default_dac_len = packed_v0->default_dac_len;
	eth_lm->link_training = !!packed_v0->link_training;
	eth_lm->rx_equal = !!packed_v0->rx_equal;
	eth_lm->static_values = !!packed_v0->static_values;
	eth_lm->retimer_channel = packed_v0->retimer_channel;
	eth_lm->auto_fec_enable = !!packed_v0->auto_fec_enable;
	eth_lm->auto_fec_initial_timeout = packed_v0->auto_fec_initial_timeout;
	eth_lm->auto_fec_toggle_timeout = packed_v0->auto_fec_toggle_timeout;

	eth_lm->gpio_present = packed_v0->gpio_present;
	eth_lm->retimer_tx_channel = packed_v0->retimer_tx_channel;
	eth_lm->retimer_configured = !!packed_v0->retimer_configured;
	eth_lm->max_speed = packed_v0->max_speed;
	eth_lm->sfp_detect_force_mode = !!packed_v0->sfp_detect_force_mode;
	eth_lm->speed_detection = !!packed_v0->speed_detection;
	eth_lm->link_state = packed_v0->link_state;

	eth_lm->auto_fec_state = packed_v0->auto_fec_state;
	eth_lm->fec_state_last_time_toggled =
		packed_v0->fec_state_last_time_toggled;
	eth_lm->auto_fec_wait_to_toggle = packed_v0->auto_fec_wait_to_toggle;
	eth_lm->link_prev_up = !!packed_v0->link_prev_up;
	eth_lm->last_detected_mode = packed_v0->last_detected_mode;
	eth_lm->speed_change = !!packed_v0->speed_change;

	al_mod_eth_lm_step_data_unpack_v0(&eth_lm->step_data, &packed_v0->step_data);

	al_mod_eth_lm_retimer_channel_status_unpack_v0(&eth_lm->retimer_rx_channel_last_status,
						   &packed_v0->retimer_rx_channel_last_status);

	al_mod_eth_lm_retimer_channel_status_unpack_v0(&eth_lm->retimer_tx_channel_last_status,
						   &packed_v0->retimer_tx_channel_last_status);

	return 0;
}

/*****************************API Functions  **********************************/
int al_mod_eth_lm_v3_handle_pack(struct al_mod_hal_eth_lm_v3_packed_container *lm_packed_cont,
			     struct al_mod_eth_lm_context *eth_lm,
			     int idx,
			     enum al_mod_lu_hal_packed_ver dst_ver)
{
	struct al_mod_hal_eth_lm_v3_packed *lm_packed =
				(struct al_mod_hal_eth_lm_v3_packed *)lm_packed_cont;

	al_mod_static_assert(sizeof(struct al_mod_hal_eth_lm_v3_packed_v0) <
			AL_LU_ETH_LM_V3_PACKED_DATA_LEN, "Packed v0 struct to big");
	al_mod_static_assert(sizeof(struct al_mod_hal_eth_lm_v3_packed_container) >=
		sizeof(struct al_mod_hal_eth_lm_v3_packed), "Packed struct larger than container");
	al_mod_static_assert(sizeof(struct al_mod_hal_eth_lm_v3_packed) >=
			(AL_LU_ETH_LM_V3_PACKED_BUFFER_COUNT *
			 sizeof(struct al_mod_hal_eth_lm_v3_packed_v0)),
			"2*Packed_v0 larger than packed");

	al_mod_memset(lm_packed_cont, 0, sizeof(struct al_mod_hal_eth_lm_v3_packed_container));

	/* Put the last HAL version that changed the eth */
	if (AL_ARR_SIZE(eth_lm_v3_convert_down_funcs) > 0) {
		size_t index = AL_ARR_SIZE(eth_lm_v3_convert_down_funcs) - 1;

		lm_packed->ver = eth_lm_v3_convert_down_funcs[index].ver;
	} else {
		lm_packed->ver = AL_LU_HAL_VERSION_V0;
	}

	lm_packed->magic = AL_LU_ETH_PACKED_MAGIC;
	lm_packed->id = AL_LU_ETH_LM_V3_PACKED_ID;
	lm_packed->idx = idx;
	lm_packed->active_buff = 0;
	lm_packed->data_len = sizeof(struct al_mod_hal_eth_lm_v3_packed_v0);

	al_mod_eth_lm_v3_pack_v0(lm_packed, eth_lm);

	if (al_mod_lu_packed_convert(lm_packed,
				 dst_ver,
				 eth_lm_v3_convert_up_funcs,
				 eth_lm_v3_convert_down_funcs,
				 eth_lm_v3_get_ver_func,
				 AL_ARR_SIZE(eth_lm_v3_convert_up_funcs)) != 0)
		return -EINVAL;

	return 0;
}

int al_mod_eth_lm_v3_handle_unpack(struct al_mod_eth_lm_context *eth_lm,
			       struct al_mod_hal_eth_lm_v3_packed_container *lm_packed_cont)
{
	struct al_mod_hal_eth_lm_v3_packed *lm_packed =
		(struct al_mod_hal_eth_lm_v3_packed *)lm_packed_cont;
	int rc;

	/* Assertions for checking enum differences between HAL versions.
	 * When changing an ENUM, create a new HAL version, and implement a
	 * convert function
	 */
	al_mod_static_assert(AL_SRDS_LANE_0 == 0, "Enum mismatch");
	al_mod_static_assert(AL_SRDS_LANE_1 == 1, "Enum mismatch");
	al_mod_static_assert(AL_SRDS_LANE_2 == 2, "Enum mismatch");
	al_mod_static_assert(AL_SRDS_LANE_3 == 3, "Enum mismatch");
	al_mod_static_assert(AL_SRDS_NUM_LANES == 4, "Enum mismatch");
	al_mod_static_assert(AL_SRDS_LANES_0123 == 7, "Enum mismatch");

	al_mod_static_assert(AL_ETH_LM_MODE_DISCONNECTED == 0, "Enum mismatch");
	al_mod_static_assert(AL_ETH_LM_MODE_10G_OPTIC == 1, "Enum mismatch");
	al_mod_static_assert(AL_ETH_LM_MODE_10G_DA == 2, "Enum mismatch");
	al_mod_static_assert(AL_ETH_LM_MODE_1G == 3, "Enum mismatch");
	al_mod_static_assert(AL_ETH_LM_MODE_25G == 4, "Enum mismatch");

	al_mod_static_assert(AL_ETH_RETIMER_CHANNEL_A == 0, "Enum mismatch");
	al_mod_static_assert(AL_ETH_RETIMER_CHANNEL_B == 1, "Enum mismatch");
	al_mod_static_assert(AL_ETH_RETIMER_CHANNEL_C == 2, "Enum mismatch");
	al_mod_static_assert(AL_ETH_RETIMER_CHANNEL_D == 3, "Enum mismatch");
	al_mod_static_assert(AL_ETH_RETIMER_CHANNEL_E == 4, "Enum mismatch");
	al_mod_static_assert(AL_ETH_RETIMER_CHANNEL_F == 5, "Enum mismatch");
	al_mod_static_assert(AL_ETH_RETIMER_CHANNEL_G == 6, "Enum mismatch");
	al_mod_static_assert(AL_ETH_RETIMER_CHANNEL_H == 7, "Enum mismatch");
	al_mod_static_assert(AL_ETH_RETIMER_CHANNEL_MAX == 8, "Enum mismatch");

	al_mod_static_assert(AL_ETH_LM_MAX_SPEED_MAX == 0, "Enum mismatch");
	al_mod_static_assert(AL_ETH_LM_MAX_SPEED_25G == 1, "Enum mismatch");
	al_mod_static_assert(AL_ETH_LM_MAX_SPEED_10G == 2, "Enum mismatch");
	al_mod_static_assert(AL_ETH_LM_MAX_SPEED_1G == 3, "Enum mismatch");

	al_mod_static_assert(AL_ETH_LM_LINK_DOWN == 0, "Enum mismatch");
	al_mod_static_assert(AL_ETH_LM_LINK_DOWN_RF == 1, "Enum mismatch");
	al_mod_static_assert(AL_ETH_LM_LINK_UP == 2, "Enum mismatch");

	al_mod_static_assert(AL_ETH_LM_AUTO_FEC_DISABLED == 0, "Enum mismatch");
	al_mod_static_assert(AL_ETH_LM_AUTO_FEC_ENABLED == 1, "Enum mismatch");
	al_mod_static_assert(AL_ETH_LM_AUTO_FEC_INIT == 2, "Enum mismatch");
	al_mod_static_assert(AL_ETH_LM_AUTO_FEC_INIT_ENABLED == 3, "Enum mismatch");

	al_mod_assert(lm_packed->magic == AL_LU_ETH_PACKED_MAGIC);

	/* Convert the packed structure to a format we know how to work with */
	if (al_mod_lu_packed_convert(lm_packed,
				 AL_LU_HAL_CURR_VER,
				 eth_lm_v3_convert_up_funcs,
				 eth_lm_v3_convert_down_funcs,
				 eth_lm_v3_get_ver_func,
				 AL_ARR_SIZE(eth_lm_v3_convert_up_funcs)) != 0)
		return -EINVAL;

	/* Unpacks the lm struct, and all of the structs under it */
	rc = al_mod_eth_lm_v3_unpack_v0(eth_lm, lm_packed);
	if (rc != 0) {
		al_mod_err("Failed unpacked lm_packed struct\n");
		return -EINVAL;
	}

	return 0;
}

unsigned int al_mod_eth_lm_v3_packed_idx_get(struct al_mod_hal_eth_lm_v3_packed_container *lm_packed_cont)
{
	struct al_mod_hal_eth_lm_v3_packed *eth_lm_packed =
		(struct al_mod_hal_eth_lm_v3_packed *)lm_packed_cont;

	return eth_lm_packed->idx;
}

