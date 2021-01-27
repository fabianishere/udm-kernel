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
 *  Ethernet
 *  @{
 * @file   al_mod_init_eth_lm.h
 *
 * @brief ethernet link management common utilities
 *
 * Common operation example:
 * @code
 *      int main()
 *      {
 *		struct al_mod_eth_lm_context lm_context;
 *		struct al_mod_eth_lm_init_params lm_params;
 *		enum al_mod_eth_lm_link_mode old_mode;
 *		enum al_mod_eth_lm_link_mode new_mode;
 *		al_mod_bool	fault;
 *		al_mod_bool				link_up;
 *		int rc = 0;
 *
 *		lm_params.adapter = hal_adapter;
 *		lm_params.serdes_obj = serdes;
 *		lm_params.grp = grp;
 *		lm_params.lane = lane;
 *		lm_params.sfp_detection = true;
 *		lm_params.link_training = true;
 *		lm_params.rx_equal = true
 *		lm_params.static_values = true;
 *		lm_params.kr_fec_enable = false;
 *		lm_params.eeprom_read = &my_eeprom_read;
 *		lm_params.eeprom_context = context;
 *		lm_params.get_random_byte = &my_rand_byte;
 *		lm_params.default_mode = AL_ETH_LM_MODE_10G_DA;
 *
 *		al_mod_eth_lm_init(&lm_context, &lm_params);
 *
 *		rc = al_mod_eth_lm_link_detection(&lm_context, &fault, &old_mode, &new_mode);
 *		if (fault == false)
 *			return; // in this case the link is still up
 *
 *		if (rc) {
 *			printf("link detection failed on error\n");
 *			return;
 *		}
 *
 *		if (old_mode != new_mode) {
 *			 // perform serdes configuration if needed
 *
 *			 // mac stop / start / config if needed
 *		}
 *
 *		spin_lock(lock);
 *		rc = al_mod_eth_lm_link_establish($lm_context, &link_up);
 *		spin_unlock(lock);
 *		if (rc) {
 *			printf("establish link failed\n");
 *			return;
 *		}
 *
 *		if (link_up)
 *			printf("Link established successfully\n");
 *		else
 *			printf("No signal found. probably the link partner is disconnected\n");
 *      }
 * @endcode
 *
 */

#ifndef __AL_INIT_ETH_LM_H__
#define __AL_INIT_ETH_LM_H__

#include <linux/sfp.h>

#include "al_mod_serdes.h"
#include "al_mod_hal_eth.h"
#include "al_mod_init_eth_kr.h"
#include "al_mod_eth_lm_retimer.h"

#define SFP_I2C_ADDR			(0xA0 >> 1)
#define SFP_I2C_ADDR_A2			(0xA2 >> 1)
#define SFP_I2C_ADDR_PHY		(0xAC >> 1)

enum al_mod_eth_lm_link_mode {
	AL_ETH_LM_MODE_DISCONNECTED,
	AL_ETH_LM_MODE_10G_OPTIC,
	AL_ETH_LM_MODE_10G_DA,
	AL_ETH_LM_MODE_1G,
	AL_ETH_LM_MODE_25G,
	AL_ETH_LM_MODE_1G_DA
};

enum al_mod_eth_lm_max_speed {
	AL_ETH_LM_MAX_SPEED_MAX,
	AL_ETH_LM_MAX_SPEED_25G,
	AL_ETH_LM_MAX_SPEED_10G,
	AL_ETH_LM_MAX_SPEED_1G,
};

enum al_mod_eth_lm_link_state {
	AL_ETH_LM_LINK_DOWN,
	AL_ETH_LM_LINK_DOWN_RF,
	AL_ETH_LM_LINK_UP,
};

enum al_mod_eth_lm_led_config_speed {
	AL_ETH_LM_LED_CONFIG_1G,
	AL_ETH_LM_LED_CONFIG_10G,
	AL_ETH_LM_LED_CONFIG_25G,
};

struct al_mod_eth_lm_led_config_data {
	enum al_mod_eth_lm_led_config_speed	speed;
};

enum al_mod_auto_fec_state {
	AL_ETH_LM_AUTO_FEC_DISABLED,
	AL_ETH_LM_AUTO_FEC_ENABLED,
	AL_ETH_LM_AUTO_FEC_INIT,		/* state we init Auto FEC internal variables
						 * (also after SFP disconnect)
						 */
	AL_ETH_LM_AUTO_FEC_INIT_ENABLED,	/* the actual initial cycle of the toggling
						 * mechanism, where the FEC should be enabled
						 */
};

/**
 * allow to turn off/on probing of different SFP modules
 */
enum al_mod_eth_lm_sfp_probe_type {
	AL_ETH_LM_SFP_PROBE_1G,
	AL_ETH_LM_SFP_PROBE_10G,
};

/**
 * @brief List of quirks
 */
enum al_mod_sfp_quirks_e {
	/* module does not support sequential reading */
	AL_MOD_SFP_QUIRK_NO_SEQ_READING = 0,
	/* limit access to module's EEPROM */
	AL_MOD_SFP_QUIRK_NO_EEPROM_ACCESS,

	AL_MOD_SFP_QUIRK_COUNT
};

struct al_mod_sfp_fixup_entry {
	const char *pn;
	const unsigned long sfp_quirk_flags;
	const int delay_init_s;
};

/**
 * @brief List of modules with quirks
 *
 * UF-INSTANT - doesn't support sequential reading
 * Alcatel 3FE46541AA & Huawei MA5671A - periodic reading of eeprom leads to jamming i2c bus
 * VSOL 1GE (SFP) ONU - doesn't support sequential reading
 *
 * X (product number, quirk flags, number of seconds to delay SFP initialization)
 */
#define AL_MOD_SFP_FIXUP_LIST(X) \
	X("MA5671A",		BIT(AL_MOD_SFP_QUIRK_NO_EEPROM_ACCESS),	10) \
	X("3FE46541AA",		BIT(AL_MOD_SFP_QUIRK_NO_EEPROM_ACCESS),	10) \
	X("UF-INSTANT",		BIT(AL_MOD_SFP_QUIRK_NO_SEQ_READING),	0)  \
	X("V2801F",			BIT(AL_MOD_SFP_QUIRK_NO_SEQ_READING),	0)

struct al_mod_eth_lm_step_retimer_data {
	int		rx_adap_state;
	int		slc_state;
	int		wfl_state;
	unsigned int	wfl_last_time_checked;
};

struct al_mod_eth_lm_step_data {
	int	detection_state;
	int	establish_state;
	struct al_mod_eth_lm_step_retimer_data retimer_data;
	/* start_time field is used for all stages since they are sequential (for the same port)*/
	unsigned int start_time; /*in msec*/
};

struct al_mod_eth_lm_retimer_channel_status {
	al_mod_bool signal_detect;
	al_mod_bool cdr_lock;
	al_mod_bool first_check;
};

struct al_mod_eth_lm_link_config {
	/* desired duplex */
	int	duplex;
	/* desired speed */
	int	speed;
	/* autoneg */
	al_mod_bool	autoneg;
};

struct al_mod_eth_lm_context {
	struct al_mod_hal_eth_adapter	*adapter;
	struct al_mod_serdes_grp_obj	*serdes_obj;
	enum al_mod_serdes_lane		lane;

	uint32_t			link_training_failures;

	al_mod_bool				tx_param_dirty;
	al_mod_bool				serdes_tx_params_valid;
	struct al_mod_serdes_adv_tx_params	tx_params_override;
	al_mod_bool				rx_param_dirty;
	al_mod_bool				serdes_rx_params_valid;
	struct al_mod_serdes_adv_rx_params	rx_params_override;

	struct al_mod_eth_an_adv		local_adv;
	struct al_mod_eth_an_adv		partner_adv;

	enum al_mod_eth_lm_link_mode	mode;
	uint8_t				da_len;
	al_mod_bool				debug;

	/* configurations */
	al_mod_bool				sfp_detection;
	uint8_t				sfp_bus_id;
	uint8_t				sfp_i2c_addr;

	enum al_mod_eth_lm_link_mode	default_mode;
	uint8_t				default_dac_len;
	al_mod_bool				link_training;
	al_mod_bool				rx_equal;
	al_mod_bool				static_values;
	al_mod_bool 			sfp_probe_1g;
	al_mod_bool 			sfp_probe_10g;
	al_mod_bool 			sfp_has_phyreg;

	/** Obsolete field; not used, left to keep structure */
	al_mod_bool				retimer_exist;
	/** Obsolete field; not used, left to keep structure */
	enum al_mod_eth_retimer_type	retimer_type;
	/** Obsolete field; not used, left to keep structure */
	uint8_t				retimer_bus_id;
	/** Obsolete field; not used, left to keep structure */
	uint8_t				retimer_i2c_addr;
	enum al_mod_eth_retimer_channel	retimer_channel;

	al_mod_bool				auto_fec_enable;
	unsigned int			auto_fec_initial_timeout;
	unsigned int			auto_fec_toggle_timeout;

	/* services */
	int (*i2c_read_data)(void *handle, uint8_t bus_id, uint8_t i2c_addr,
			uint8_t reg_addr, uint8_t *val, size_t len, al_mod_bool seq);
	int (*i2c_read)(void *handle, uint8_t bus_id, uint8_t i2c_addr,
			uint8_t reg_addr, uint8_t *val);
	int (*i2c_write_data)(void *handle, uint8_t bus_id, uint8_t i2c_addr,
			 uint8_t reg_addr, uint8_t *val, size_t len, al_mod_bool seq);
	int (*i2c_write)(void *handle, uint8_t bus_id, uint8_t i2c_addr,
			 uint8_t reg_addr, uint8_t val);
	void				*i2c_context;
	uint8_t (*get_random_byte)(void);

	int (*gpio_get)(unsigned int gpio);
	uint32_t			gpio_present;

	enum al_mod_eth_retimer_channel	retimer_tx_channel;
	al_mod_bool				retimer_configured;

	enum al_mod_eth_lm_max_speed	max_speed;

	al_mod_bool				sfp_detect_force_mode;
	al_mod_bool				speed_detection;

	enum al_mod_eth_lm_link_state	link_state;

	al_mod_bool (*lm_pause)(void *handle);

	void (*led_config)(void *handle, struct al_mod_eth_lm_led_config_data *data);

	unsigned int (*get_msec)(void);

	/* Auto-FEC workaround internal variables */
	/* saves the current FEC state */
	enum al_mod_auto_fec_state		auto_fec_state;
	/* saves the last time (in msec) the FEC state was toggled */
	unsigned int			fec_state_last_time_toggled;
	/* saves the time (in msec) to wait unit next toggle */
	unsigned int			auto_fec_wait_to_toggle;

	/* Used in Auto-FEC to remember that a link was previously up
	 * so we know to start the toggling from the previous FEC state
	 * instead for first toggling to opposite FEC state
	 */
	al_mod_bool				link_prev_up;

	enum al_mod_eth_lm_link_mode	last_detected_mode;
	al_mod_bool				speed_change;

	struct al_mod_eth_lm_step_data	step_data;

	struct al_mod_eth_lm_retimer_channel_status retimer_rx_channel_last_status;
	struct al_mod_eth_lm_retimer_channel_status retimer_tx_channel_last_status;

	struct al_mod_eth_lm_retimer	retimer;

	/* desired link configuration */
	struct al_mod_eth_lm_link_config	link_conf;
	/* Allows to apply a special treatment for SFP module */
	unsigned long sfp_quirk_flags;
	/* SFP A0 */
	struct sfp_eeprom_id sfp_id;
	/* lm context lock */
	struct mutex  lock;
};

struct al_mod_eth_lm_init_params {
	/* pointer to HAL context */
	struct al_mod_hal_eth_adapter	*adapter;
	/* pointer to serdes object */
	struct al_mod_serdes_grp_obj	*serdes_obj;
	/* serdes lane for this port */
	enum al_mod_serdes_lane		lane;

	/*
	 * set to true to perform sfp detection if the link is down.
	 * when set to true, eeprom_read below should NOT be NULL.
	 */
	al_mod_bool				sfp_detection;
	/* i2c bus id of the SFP for this port */
	uint8_t				sfp_bus_id;
	/* i2c addr of the SFP for this port */
	uint8_t				sfp_i2c_addr;
	/*
	 * default mode, and dac length will be used in case sfp_detection
	 * is not set or in case the detection failed.
	 */
	enum al_mod_eth_lm_link_mode	default_mode;
	uint8_t				default_dac_len;

	/* the i2c bus id and addr of the retimer in case it exist */
	uint8_t				retimer_bus_id;
	uint8_t				retimer_i2c_addr;
	/* retimer channel connected to this port */
	enum al_mod_eth_retimer_channel	retimer_channel;
	enum al_mod_eth_retimer_channel	retimer_tx_channel;
	/* retimer type if exist */
	enum al_mod_eth_retimer_type	retimer_type;

	/*
	 * the following parameters control what mechanisms to run
	 * on link_establish with the following steps:
	 * - if retimer_exist is set, the retimer will be configured based on DA len.
	 * - if link_training is set and DA detected run link training. if succeed return 0
	 * - if rx_equal is set serdes equalization will be run to configure the rx parameters.
	 * - if static_values is set, tx and rx values will be set based on static values.
	 */
	al_mod_bool				retimer_exist;
	al_mod_bool				link_training;
	al_mod_bool				rx_equal;
	al_mod_bool				static_values;

	/* Allows to disable probing of 1G & 10G modules */
	al_mod_bool				sfp_probe_1g;
	al_mod_bool				sfp_probe_10g;

	/* enable / disable fec capabilities in AN */
	al_mod_bool				kr_fec_enable;

	/*
	 * pointer to function that's read 1 byte from eeprom
	 * in case no eeprom is connected should return -ETIMEDOUT
	 */
	int (*i2c_read_data)(void *handle, uint8_t bus_id, uint8_t i2c_addr,
			uint8_t reg_addr, uint8_t *val, size_t len, al_mod_bool seq);
	int (*i2c_read)(void *handle, uint8_t bus_id, uint8_t i2c_addr,
			uint8_t reg_addr, uint8_t *val);
	int (*i2c_write_data)(void *handle, uint8_t bus_id, uint8_t i2c_addr,
			 uint8_t reg_addr, uint8_t *val, size_t len, al_mod_bool seq);
	int (*i2c_write)(void *handle, uint8_t bus_id, uint8_t i2c_addr,
			 uint8_t reg_addr, uint8_t val);
	void				*i2c_context;
	/* pointer to function that return 1 rand byte */
	uint8_t (*get_random_byte)(void);

	/* pointer to function that gets GPIO value - if NULL gpio present won't be used */
	int (*gpio_get)(unsigned int gpio);
	/* gpio number connected to the SFP present pin */
	uint32_t			gpio_present;

	enum al_mod_eth_lm_max_speed	max_speed;

	/* in case force mode is true - the default mode will be set regardless to
	 * the SFP EEPROM content */
	al_mod_bool				sfp_detect_force_mode;

	/* if true and speed detection is supported in the retimer the speed will be
	 * changed according to the link partner speed */
	al_mod_bool				speed_detection;

	/* lm pause callback - in case it return true the LM will try to preserve
	 * the current link status and will not try to establish new link (and will not
	 * access to i2c bus) */
	al_mod_bool (*lm_pause)(void *handle);

	/* config ethernet LEDs according to data. can be NULL if no configuration needed */
	void (*led_config)(void *handle, struct al_mod_eth_lm_led_config_data *data);

	/* pointer to a function which returns the value of a system clock in millisecond.
	 * NOTICE: the time is REQUIRED to wraparound over the full range of uint, or else the
	 * behavior is undefined.
	 */
	unsigned int (*get_msec)(void);

	/*
	 * the following parameters are used for Auto-FEC workaround mode
	 * time units for timeout parameters are 1msec
	 */
	/* enable/disable Auto-FEC mode */
	al_mod_bool				auto_fec_enable;
	/* Initial timeout (in msec) to wait in FEC_ENABLED mode for link establishment */
	unsigned int			auto_fec_initial_timeout;
	/* Toggle timeout (in msec) to wait before toggling FEC state */
	unsigned int			auto_fec_toggle_timeout;
};

/**
 * initialize link management context and set configuration
 *
 * @param  lm_context pointer to link management context
 * @param  params  parameters passed from upper layer
 *
 * @return 0 in case of success. otherwise on failure.
 */
int al_mod_eth_lm_init(struct al_mod_eth_lm_context	*lm_context,
		   struct al_mod_eth_lm_init_params	*params);

/**
 * perform link status check. in case link is down perform sfp detection
 *
 * @param lm_context pointer to link management context
 * @param link_fault indicate if the link is down
 * @param old_mode the last working mode
 * @param new_mode the new mode detected in this call
 *
 * @return  0 in case of success. otherwise on failure.
 */
int al_mod_eth_lm_link_detection(struct al_mod_eth_lm_context	*lm_context,
			     al_mod_bool			*link_fault,
			     enum al_mod_eth_lm_link_mode	*old_mode,
			     enum al_mod_eth_lm_link_mode	*new_mode);

/**
 * run LT, rx equalization and static values override according to configuration
 * This function MUST be called inside a lock as it using common serdes registers
 *
 * @param lm_context pointer to link management context
 * @param link_up set to true in case link is establish successfully
 *
 * @return < 0 in case link was failed to be established
 */
int al_mod_eth_lm_link_establish(struct al_mod_eth_lm_context	*lm_context,
			     al_mod_bool			*link_up);

/**
 * override the default static parameters
 *
 * @param lm_context pointer to link management context
 * @param tx_params pointer to new tx params
 * @param rx_params pointer to new rx params
 *
 * @return  0 in case of success. otherwise on failure.
 **/
int al_mod_eth_lm_static_parameters_override(struct al_mod_eth_lm_context	*lm_context,
					 struct al_mod_serdes_adv_tx_params *tx_params,
					 struct al_mod_serdes_adv_rx_params *rx_params);

/**
 * disable serdes parameters override
 *
 * @param lm_context pointer to link management context
 * @param tx_params set to true to disable override of tx params
 * @param rx_params set to true to disable override of rx params
 *
 * @return  0 in case of success. otherwise on failure.
 **/
int al_mod_eth_lm_static_parameters_override_disable(
					struct al_mod_eth_lm_context *lm_context,
					al_mod_bool			 tx_params,
					al_mod_bool			 rx_params);

/**
 * get the static parameters that are being used
 * if the parameters was override - return the override values
 * else return the current values of the parameters
 *
 * @param  lm_context pointer to link management context
 * @param  tx_params  pointer to new tx params
 * @param  rx_params  pointer to new rx params
 *
 * @return  0 in case of success. otherwise on failure.
 */
int al_mod_eth_lm_static_parameters_get(struct al_mod_eth_lm_context	*lm_context,
				    struct al_mod_serdes_adv_tx_params *tx_params,
				    struct al_mod_serdes_adv_rx_params *rx_params);

/**
 * convert link management mode to string
 *
 * @param  val link management mode
 *
 * @return     string of the mode
 */
const char *al_mod_eth_lm_mode_convert_to_str(enum al_mod_eth_lm_link_mode val);

/**
 * print all debug messages
 *
 * @param lm_context pointer to link management context
 * @param enable     set to true to enable debug mode
 */
void al_mod_eth_lm_debug_mode_set(struct al_mod_eth_lm_context	*lm_context,
			      al_mod_bool			enable);

/**
 * return the state of retimer tx cdr lock
 *
 * @param  lm_context pointer to link management context
 *
 * @return  retimer tx cdr lock status. AL_FALSE if retimer not available or no cdr_lock implementation
 */
al_mod_bool al_mod_eth_lm_retimer_tx_cdr_lock_get(struct al_mod_eth_lm_context *lm_context);

/**
 * perform link status check. in case link is down perform sfp detection
 * uses an internal state machine to overcome long delays which can degrade performance.
 * NOTICE: MUST retain old_mode and new_mode values until done is true
 *
 * @param lm_context pointer to link management context
 * @param link_fault indicate if the link is down
 * @param old_mode the last working mode
 * @param new_mode the new mode detected in this call
 *
 * @return  0 in case of success. -EINPROGRESS (-115) if the state machine is still in progress. otherwise on failure.
 */
int al_mod_eth_lm_link_detection_step(struct al_mod_eth_lm_context	*lm_context,
				al_mod_bool				*link_fault,
				enum al_mod_eth_lm_link_mode	*old_mode,
				enum al_mod_eth_lm_link_mode	*new_mode);

/**
 * run LT, rx equalization and static values override according to configuration
 * uses an internal state machine to overcome long delays which can degrade performance
 * This function MUST be called inside a lock as it uses common serdes registers
 *
 * @param lm_context pointer to link management context
 * @param link_up set to true in case link is establish successfully
 *
 * @return 0 in case of success. -EINPROGRESS (-115) if the state machine is still in progress. otherwise in case
 * link was failed to be established
 */
int al_mod_eth_lm_link_establish_step(struct al_mod_eth_lm_context	*lm_context,
				al_mod_bool			*link_up);

/**
 * getter function for link state
 *
 * @param lm_context pointer to link management context
 *
 * @return link state
 */
enum al_mod_eth_lm_link_state al_mod_eth_lm_link_state_get(struct al_mod_eth_lm_context *lm_context);

/**
 * checks new link status and returns it through a parameter.
 * NOTICE: the internal state of lm_context isn't updated when calling this function.
 *
 * @param lm_context pointer to link management context
 * @param link_state pointer to link_state
 *
 * @return 0 in case of success reading the link status. otherwise on failure
 */
int al_mod_eth_lm_link_check(struct al_mod_eth_lm_context *lm_context,
			enum al_mod_eth_lm_link_state *link_state);

/**
 * check lm_pause current status.
 *
 * @param lm_context pointer to link management context
 *
 * @return TRUE indicates we must pause. otherwise FALSE.
 */
al_mod_bool al_mod_eth_lm_pause_check(struct al_mod_eth_lm_context *lm_context);

/**
 * ethtool get_module_info handler
 *
 * @param lm_context pointer to link management context
 * @param modinfo pointer to struct ethtool_modinfo
 *
 * @return 0 in case of success reading the link status. otherwise on failure
 */
int al_mod_eth_lm_get_module_info(struct al_mod_eth_lm_context *lm_context,
		   struct ethtool_modinfo *modinfo);

/**
 * ethtool get_module_eeprom handler
 *
 * @param lm_context pointer to link management context
 * @param eeprom pointer to struct ethtool_eeprom
 * @param data pointer to eeprom data
 *
 * @return 0 in case of success reading the link status. otherwise on failure
 */
int al_mod_eth_lm_get_module_eeprom(struct al_mod_eth_lm_context *lm_context,
		struct ethtool_eeprom *eeprom, uint8_t *data);

#endif
