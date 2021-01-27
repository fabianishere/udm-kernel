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
 * @defgroup group_serdes_api API
 * SerDes HAL driver API
 * @ingroup group_serdes SerDes
 * @{
 *
 * @file   al_mod_hal_serdes_avg.h
 *
 * @brief Header file for the SerDes HAL driver
 *
 */

#ifndef __AL_HAL_SERDES_AVG_H__
#define __AL_HAL_SERDES_AVG_H__

#include "al_mod_hal_common.h"
#include "al_mod_hal_serdes_interface.h"
#include "al_mod_hal_sbus.h"

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

/* Number of lanes per complex type */
#define AL_SRDS_LANES_PER_COMPLEX_TNK_MAIN	6
#define AL_SRDS_LANES_PER_COMPLEX_TR_TC_MAIN	3
#define AL_SRDS_LANES_PER_COMPLEX_TR_MAIN	16
#define AL_SRDS_LANES_PER_COMPLEX_TR_SEC	16
#define AL_SRDS_LANES_PER_COMPLEX_TR_HS		4
#define AL_SRDS_LANES_PER_COMPLEX_MAX		16

/** IOFIC bits - each lane has its own iofic */
/* group A*/
#define AL_SRDS_AVG_INT_GROUP_A_SERDES_SPICO_INT_IN_PROGRESS			AL_BIT(0)
#define AL_SRDS_AVG_INT_GROUP_A_SINGLE_BIT_ERROR_INDICATION_SERDES_MEMORIES	AL_BIT(5)
#define AL_SRDS_AVG_INT_GROUP_A_DOUBLE_BIT_ERROR_INDICATION_IN_SERDES_MEMORY	AL_BIT(6)
#define AL_SRDS_AVG_INT_GROUP_A_COMMA_ALIGNED_IN_SATA_MODE			AL_BIT(7)
#define AL_SRDS_AVG_INT_GROUP_A_FSM_STATE						AL_BIT(8)
/* group B*/
#define AL_SRDS_AVG_INT_GROUP_B_SERDES_CORE_STATUS_0				AL_BIT(0)
#define AL_SRDS_AVG_INT_GROUP_B_SERDES_CORE_STATUS_1				AL_BIT(1)
#define AL_SRDS_AVG_INT_GROUP_B_SERDES_CORE_STATUS_2				AL_BIT(2)
#define AL_SRDS_AVG_INT_GROUP_B_SERDES_CORE_STATUS_3				AL_BIT(3)
#define AL_SRDS_AVG_INT_GROUP_B_SERDES_CORE_STATUS_4				AL_BIT(4)
#define AL_SRDS_AVG_INT_GROUP_B_SERDES_CORE_STATUS_5				AL_BIT(5)
#define AL_SRDS_AVG_INT_GROUP_B_SERDES_CORE_STATUS_6				AL_BIT(6)
#define AL_SRDS_AVG_INT_GROUP_B_SERDES_CORE_STATUS_7				AL_BIT(7)
#define AL_SRDS_AVG_INT_GROUP_B_SERDES_CORE_STATUS_8				AL_BIT(8)
#define AL_SRDS_AVG_INT_GROUP_B_SERDES_CORE_STATUS_9				AL_BIT(9)
#define AL_SRDS_AVG_INT_GROUP_B_SERDES_CORE_STATUS_10				AL_BIT(10)
#define AL_SRDS_AVG_INT_GROUP_B_SERDES_CORE_STATUS_11				AL_BIT(11)
#define AL_SRDS_AVG_INT_GROUP_B_SERDES_CORE_STATUS_12				AL_BIT(12)
#define AL_SRDS_AVG_INT_GROUP_B_SERDES_CORE_STATUS_13				AL_BIT(13)
#define AL_SRDS_AVG_INT_GROUP_B_SERDES_CORE_STATUS_14				AL_BIT(14)
#define AL_SRDS_AVG_INT_GROUP_B_SERDES_CORE_STATUS_15				AL_BIT(15)
#define AL_SRDS_AVG_INT_GROUP_B_SERDES_CORE_STATUS_16				AL_BIT(16)
#define AL_SRDS_AVG_INT_GROUP_B_SERDES_CORE_STATUS_17				AL_BIT(17)
#define AL_SRDS_AVG_INT_GROUP_B_SERDES_CORE_STATUS_18				AL_BIT(18)
#define AL_SRDS_AVG_INT_GROUP_B_SERDES_CORE_STATUS_19				AL_BIT(19)
#define AL_SRDS_AVG_INT_GROUP_B_SERDES_CORE_STATUS_20				AL_BIT(20)
#define AL_SRDS_AVG_INT_GROUP_B_SERDES_CORE_STATUS_21				AL_BIT(21)
#define AL_SRDS_AVG_INT_GROUP_B_SERDES_CORE_STATUS_22				AL_BIT(22)
#define AL_SRDS_AVG_INT_GROUP_B_SERDES_CORE_STATUS_23				AL_BIT(23)
#define AL_SRDS_AVG_INT_GROUP_B_SERDES_CORE_STATUS_24				AL_BIT(24)
#define AL_SRDS_AVG_INT_GROUP_B_SERDES_CORE_STATUS_25				AL_BIT(25)
#define AL_SRDS_AVG_INT_GROUP_B_SERDES_CORE_STATUS_26				AL_BIT(26)
#define AL_SRDS_AVG_INT_GROUP_B_SERDES_CORE_STATUS_27				AL_BIT(27)
#define AL_SRDS_AVG_INT_GROUP_B_SERDES_CORE_STATUS_28				AL_BIT(28)
#define AL_SRDS_AVG_INT_GROUP_B_SERDES_CORE_STATUS_29				AL_BIT(29)
#define AL_SRDS_AVG_INT_GROUP_B_SERDES_CORE_STATUS_30				AL_BIT(30)
#define AL_SRDS_AVG_INT_GROUP_B_SERDES_CORE_STATUS_31				AL_BIT(31)

/**
 * SBUS master interrupts
 * There are 3 SBUS masters (for each complex).
 * Each one has its own IRQ at the GIC, there is no indication about which lane triggered
 * the interrupt so when we receive an interrupt we can only know the serdes-complex it originated
 * from and we should look at the iofic of all the lanes
 */
#define AL_SRDS_SBUS_MASTER_INT_GROUP_A_SINGLE_BIT_ERR	AL_BIT(20)
#define AL_SRDS_SBUS_MASTER_INT_GROUP_A_DOUBLE_BIT_ERR	AL_BIT(21)
#define AL_SRDS_SBUS_MASTER_INT_GROUP_A_HIGH_TEMP	AL_BIT(22)

/* SerDes complex types */
enum al_mod_serdes_complex_type {
	/* Taranaki complex */
	AL_SRDS_COMPLEX_TYPE_TNK_MAIN,
	/* Alpine V3 test-chip main complex */
	AL_SRDS_COMPLEX_TYPE_TR_TC_MAIN,
	/* Alpine V3 main complex */
	AL_SRDS_COMPLEX_TYPE_TR_MAIN,
	/* Alpine V3 secondary complex */
	AL_SRDS_COMPLEX_TYPE_TR_SEC,
	/* Alpine V3 high-speed complex */
	AL_SRDS_COMPLEX_TYPE_TR_HS,
};

/* SATA speed */
enum al_mod_serdes_avg_sata_speed {
	AL_SRDS_AVG_SATA_SPEED_GEN_1,
	AL_SRDS_AVG_SATA_SPEED_GEN_2,
	AL_SRDS_AVG_SATA_SPEED_GEN_3,
};

/** Ethernet link training mode */
enum al_mod_serdes_avg_eth_lt_mode {
	/** The lane is a stand alone 10G Ethernet port */
	AL_SRDS_AVG_ETH_LT_MODE_10G,
	/** The lane is a stand alone 25G Ethernet port */
	AL_SRDS_AVG_ETH_LT_MODE_25G,
	/** The lane is part of 40G Ethernet port */
	AL_SRDS_AVG_ETH_LT_MODE_40G,
	/** The lane is part of 50G Ethernet port */
	AL_SRDS_AVG_ETH_LT_MODE_50G,
	/** The lane is part of 100G Ethernet port */
	AL_SRDS_AVG_ETH_LT_MODE_100G,
};

/** Ethernet lane speed */
enum al_mod_serdes_avg_eth_lane_speed {
	AL_SRDS_AVG_ETH_LANE_SPEED_10G,
	AL_SRDS_AVG_ETH_LANE_SPEED_10G_40BIT,
	AL_SRDS_AVG_ETH_LANE_SPEED_25G,
};

/** SerDes bit width */
enum al_mod_serdes_bit_width {
	AL_SRDS_BIT_WIDTH_10,
	AL_SRDS_BIT_WIDTH_20,
	AL_SRDS_BIT_WIDTH_32,
	AL_SRDS_BIT_WIDTH_40,
};

/** SerDes bit rate */
enum al_mod_serdes_bit_rate {
	AL_SRDS_BIT_RATE_6_0G,
	AL_SRDS_BIT_RATE_1_25G,
	AL_SRDS_BIT_RATE_10_3125G,
	AL_SRDS_BIT_RATE_12_89G,
	AL_SRDS_BIT_RATE_25_78G,
};

struct al_mod_serdes_complex_obj {
	void __iomem			*regs_base;
	enum al_mod_serdes_complex_type	complex_type;
	uint32_t			(*reg_read32)(uint32_t *p);
	void				(*reg_write32)(uint32_t *p, uint32_t v);
	struct al_mod_sbus_obj	sbus_obj;
	struct al_mod_serdes_grp_obj	*lanes[AL_SRDS_LANES_PER_COMPLEX_MAX];
};

/**
 * SerDes complex handle initialization
 *
 * @param	regs_base
 *		Complex register base
 * @param	sbus_regs_base
 *		SBUS master register base
 * @param	complex_type
 *		Complex type
 * @param	reg_read32
 *		32 bit register reading function (NULL for default implementation)
 * @param	reg_write32
 *		32 bit register writing function (NULL for default implementation)
 * @param	obj
 *		A pointer to a non initialized complex handle structure
 */
void al_mod_serdes_avg_complex_handle_init(
	void __iomem			*regs_base,
	void __iomem			*sbus_regs_base,
	enum al_mod_serdes_complex_type	complex_type,
	uint32_t			(*reg_read32)(uint32_t *p),
	void				(*reg_write32)(uint32_t *p, uint32_t v),
	struct al_mod_serdes_complex_obj	*obj);

/**
 * SerDes lane handle initialization
 *
 * @param	regs_base
 *		Lane register base
 * @param	complex_obj
 *		Parent complex initialized object
 * @param	lane_in_complex
 *		The number of the lane inside its parent complex
 * @param	obj
 *		A pointer to a non initialized lane handle structure
 */
void al_mod_serdes_avg_handle_init(
	void __iomem			*regs_base,
	struct al_mod_serdes_complex_obj	*complex_obj,
	unsigned int			lane_in_complex,
	struct al_mod_serdes_grp_obj	*obj);

/**
 * SerDes core interrupt - wait for completion of ongoing interrupt and get it's result
 *
 * @param	obj
 *		A pointer to an initialized lane handle structure
 *
 * @returns	Core interrupt result
 */
unsigned int al_mod_serdes_avg_core_interrupt_res_wait_and_get(
	struct al_mod_serdes_grp_obj	*obj);

/**
 * SerDes core interrupt trigger, without waiting for it's completion
 *
 * @param	obj
 *		A pointer to an initialized lane handle structure
 * @param	code
 *		Core interrupt code
 * @param	data
 *		Core interrupt data
 */
void al_mod_serdes_avg_core_interrupt_no_wait(
	struct al_mod_serdes_grp_obj	*obj,
	unsigned int			code,
	unsigned int			data);

/**
 * SerDes core interrupt trigger, with waiting for it's completion and getting it's result
 *
 * @param	obj
 *		A pointer to an initialized lane handle structure
 * @param	code
 *		Core interrupt code
 * @param	data
 *		Core interrupt data
 *
 * @returns	Core interrupt result
 */
unsigned int al_mod_serdes_avg_core_interrupt(
	struct al_mod_serdes_grp_obj	*obj,
	unsigned int			code,
	unsigned int			data);

/**
 * SerDes core interrupt debug enable setting
 *
 * @param	obj
 *		A pointer to an initialized lane handle structure
 * @param	en
 *		Debug enable/disable
 */
void al_mod_serdes_avg_core_interrupt_dbg_en_set(
	struct al_mod_serdes_grp_obj	*obj,
	al_mod_bool				en);

/**
 * Force specific SATA speed
 *
 * @param	obj
 *		A pointer to an initialized lane handle structure
 * @param	speed
 *		Required speed
 */
void al_mod_serdes_avg_sata_speed_force(
	struct al_mod_serdes_grp_obj	*obj,
	enum al_mod_serdes_avg_sata_speed	speed);

/**
 * Ethernet Link Training
 *
 * @param	obj
 *		A pointer to an initialized lane handle structure
 * @param	mode
 *		Link training mode
 *
 * @returns	0 upon success
 */
int al_mod_serdes_avg_eth_lt(
	struct al_mod_serdes_grp_obj	*obj,
	enum al_mod_serdes_avg_eth_lt_mode	mode);

/**
 * Change Ethernet lane speed
 *
 * @param	obj
 *		A pointer to an initialized lane handle structure
 * @param	speed
 *		Required speed
 */
void al_mod_serdes_avg_eth_lane_speed_change(
	struct al_mod_serdes_grp_obj		*obj,
	enum al_mod_serdes_avg_eth_lane_speed	speed);

/* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif

/* *INDENT-ON* */
#endif

/** @} end of SERDES group */

