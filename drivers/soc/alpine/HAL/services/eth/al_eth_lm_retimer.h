/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#ifndef __AL_ETH_LM_RETIMER_H__
#define __AL_ETH_LM_RETIMER_H__

#include "al_hal_common.h"
#include "al_hal_eth.h"
#include "al_hal_i2c.h"

/*******************************************************************************
 ** Initialization Structures
 ******************************************************************************/

/**
 * Supported retimers
 */
enum al_eth_lm_retimer_type {
	/** Indicating no retimer is used */
	AL_ETH_LM_RETIMER_TYPE_NONE,
	/** TI DS100BR210 */
	AL_ETH_LM_RETIMER_TYPE_BR_210,
	/** TI DS100BR410 */
	AL_ETH_LM_RETIMER_TYPE_BR_410,
	/** TI DS25 */
	AL_ETH_LM_RETIMER_TYPE_DS_25,
	AL_ETH_LM_RETIMER_TYPE_NUM,
};

/**
 * Retimer speed
 */
enum al_eth_lm_retimer_speed {
	/** Invalid speed */
	AL_ETH_LM_RETIMER_SPEED_INVALID,
	/** Auto speed (where applicable) */
	AL_ETH_LM_RETIMER_SPEED_AUTO,
	/** 10G */
	AL_ETH_LM_RETIMER_SPEED_10G,
	/** 25G */
	AL_ETH_LM_RETIMER_SPEED_25G,
	AL_ETH_LM_RETIMER_SPEED_MAX,
};

/**
 * Retimer channel direction
 */
enum al_eth_lm_retimer_ch_dir {
	/** Invalid direction */
	AL_ETH_LM_RETIMER_CH_DIR_INVALID,
	/** TX */
	AL_ETH_LM_RETIMER_CH_DIR_TX,
	/** RX */
	AL_ETH_LM_RETIMER_CH_DIR_RX,
	AL_ETH_LM_RETIMER_CH_DIR_MAX,
};

/**
 * Retimer parameters
 */
struct al_eth_lm_retimer;

struct al_eth_lm_retimer_params {
	/** Retimer type */
	enum al_eth_lm_retimer_type	type;

	/** Retimer I2C master */
	enum al_i2c_master_t		i2c_master;
	/** Retimer I2C bus */
	unsigned int			i2c_bus;
	/** Retimer I2C address */
	unsigned int			i2c_addr;

	/** I2C context for I2C functions */
	void				*i2c_context;

	/**
	 * I2C read single byte function
	 *
	 * @param i2c_context
	 *	I2C context
	 * @param i2c_master
	 *	I2C master
	 * @param i2c_bus
	 *	I2C bus
	 * @param i2c_addr
	 *	I2C address
	 * @param reg_addr
	 *	I2C register address
	 * @param val
	 *	Value returned from I2C register on success
	 *
	 * @return
	 *	0 on success, errno otherwise
	 */
	int				(*i2c_read)(void *i2c_context,
						enum al_i2c_master_t i2c_master,
						unsigned int i2c_bus, unsigned int i2c_addr,
						unsigned int reg_addr, uint8_t *val);

	/**
	 * I2C write single byte function
	 *
	 * @param i2c_context
	 *	I2C context
	 * @param i2c_master
	 *	I2C master
	 * @param i2c_bus
	 *	I2C bus
	 * @param i2c_addr
	 *	I2C address
	 * @param reg_addr
	 *	I2C register address
	 * @param val
	 *	Value to write into I2C register
	 *
	 * @return
	 *	0 on success, errno otherwise
	 */
	int				(*i2c_write)(void *i2c_context,
						enum al_i2c_master_t i2c_master,
						unsigned int i2c_bus, unsigned int i2c_addr,
						unsigned int reg_addr, uint8_t val);

	/**
	 * Retimer lock access
	 *
	 * @param lock_context
	 *	Retimer lock context
	 *
	 * @return
	 *	0 on success, errno otherwise
	 */
	int				(*retimer_lock)(void *lock_context);

	/**
	 * Retimer free access
	 *
	 * @param lock_context
	 *	Retimer lock context
	 *
	 * @return
	 *	0 on success, errno otherwise
	 */
	int				(*retimer_unlock)(void *lock_context);

	/** Retimer lock context to avoid channels mixing  */
	void				*retimer_lock_context;

	/** Debug prints will be printed in higher level */
	al_bool				debug;
};

/**
 * Retimer channel configuration parameters
 */
struct al_eth_lm_retimer_config_params {
	/** Direction */
	enum al_eth_lm_retimer_ch_dir	dir;

	/** Speed */
	enum al_eth_lm_retimer_speed	speed;

	/** Direct attach cable length (if not a DA cable, set to 0) */
	unsigned int			da_len;
};

/*******************************************************************************
 ** Internal Usage Structures
 ******************************************************************************/

struct al_eth_lm_retimer {
	/** Retimer type */
	enum al_eth_lm_retimer_type	type;

	/** Retimer I2C master */
	enum al_i2c_master_t		i2c_master;
	/** Retimer I2C bus */
	unsigned int			i2c_bus;
	/** Retimer I2C address */
	unsigned int			i2c_addr;

	/** Retimer lock context to avoid channels mixing  */
	void				*retimer_lock_context;

	/** I2C context for I2C functions */
	void				*i2c_context;

	/**
	 * I2C read function
	 *
	 * @param context
	 *	Provided I2C context
	 * @param i2c_master
	 *	I2C master
	 * @param i2c_bus
	 *	I2C bus
	 * @param i2c_addr
	 *	I2C address
	 * @param reg_addr
	 *	I2C register address
	 * @param val
	 *	Value returned from I2C register on success
	 *
	 * @return
	 *	0 on success, errno otherwise
	 */
	int				(*i2c_read)(void *i2c_context,
						enum al_i2c_master_t i2c_master,
						unsigned int i2c_bus, unsigned int i2c_addr,
						unsigned int reg_addr, uint8_t *val);

	/**
	 * I2C write function
	 *
	 * @param context
	 *	Provided I2C context
	 * @param i2c_master
	 *	I2C master
	 * @param i2c_bus
	 *	I2C bus
	 * @param i2c_addr
	 *	I2C address
	 * @param reg_addr
	 *	I2C register address
	 * @param val
	 *	Value to write into I2C register
	 *
	 * @return
	 *	0 on success, errno otherwise
	 */
	int				(*i2c_write)(void *i2c_context,
						enum al_i2c_master_t i2c_master,
						unsigned int i2c_bus, unsigned int i2c_addr,
						unsigned int reg_addr, uint8_t val);

	/** Debug prints will be printed in higher level */
	al_bool				debug;

	/**
	 * Retimer model defined functions
	 */

	/**
	 * Retimer channel configuration
	 *
	 * @param handle
	 *	Retimer handle
	 * @param channel
	 *	Retimer channel
	 * @param params
	 *	Retimer channel configuration parameters
	 *
	 * @return
	 *	0 on success, errno otherwise
	 */
	int				(*config)(struct al_eth_lm_retimer *handle,
						unsigned int channel,
						struct al_eth_lm_retimer_config_params *params);

	/**
	 * Retimer channel reset
	 *
	 * @param handle
	 *	Retimer handle
	 * @param channel
	 *	Retimer channel
	 *
	 * @return
	 *	0 on success, errno otherwise
	 */
	int				(*reset)(struct al_eth_lm_retimer *handle,
						unsigned int channel);

	/**
	 * Retimer channel signal detect
	 *
	 * @param handle
	 *	Retimer handle
	 * @param channel
	 *	Retimer channel
	 * @param detected
	 *	Is detected?
	 *
	 * @return
	 *	0 on success, errno otherwise
	 */
	int				(*signal_detect)(struct al_eth_lm_retimer *handle,
						unsigned int channel,
						al_bool *detected);

	/**
	 * Retimer channel CDR lock
	 *
	 * @param handle
	 *	Retimer handle
	 * @param channel
	 *	Retimer channel
	 * @param locked
	 *	Is locked?
	 *
	 * @return
	 *	0 on success, errno otherwise
	 */
	int				(*cdr_lock)(struct al_eth_lm_retimer *handle,
						unsigned int channel,
						al_bool *locked);

	/**
	 * Retimer RX EQ adaption complete check
	 *
	 * @param handle
	 *	Retimer handle
	 * @param channel
	 *	Retimer channel
	 * @param done
	 *	Rx_EQ adaption has completed?
	 *
	 * @return
	 *	0 on success, errno otherwise
	 */
	int				(*rx_eq_done)(struct al_eth_lm_retimer *handle,
						unsigned int channel,
						al_bool *done);

	/**
	 * Retimer channel line speed detection
	 *
	 * @param handle
	 *	Retimer handle
	 * @param channel
	 *	Retimer channel
	 * @param speed
	 *	Detected line speed
	 *
	 * @return
	 *	0 on success, errno otherwise
	 */
	int				(*speed_detect)(struct al_eth_lm_retimer *handle,
						unsigned int channel,
						enum al_eth_lm_retimer_speed *speed);

	/**
	 * Retimer lock access
	 *
	 * @param lock_context
	 *	Retimer lock context
	 *
	 * @return
	 *	0 on success, errno otherwise
	 */
	int				(*retimer_lock)(void *lock_context);

	/**
	 * Retimer free access
	 *
	 * @param lock_context
	 *	Retimer lock context
	 *
	 * @return
	 *	0 on success, errno otherwise
	 */
	int				(*retimer_unlock)(void *lock_context);
};

/*******************************************************************************
 ** API Functions
 ******************************************************************************/

/**
 * Ethernet Link Management Retimer handle initialization
 *
 * @param handle
 *	Ethernet Link Management Retimer handle
 * @param params
 *	Ethernet Management Retimer handle parameters
 *
 * @return
 *	0 on success, errno otherwise
 */
int al_eth_lm_retimer_handle_init(
	struct al_eth_lm_retimer *handle,
	const struct al_eth_lm_retimer_params *params);

#endif /* __AL_ETH_LM_RETIMER_H__ */
