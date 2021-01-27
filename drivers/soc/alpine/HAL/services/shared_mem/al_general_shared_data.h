/*******************************************************************************
Copyright (C) 2015 Annapurna Labs Ltd.

This file is licensed under the terms of the Annapurna Labs' Commercial License
Agreement distributed with the file or available on the software download site.
Recipient shall use the content of this file only on semiconductor devices or
systems developed by or for Annapurna Labs.

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
 * This file includes general shared data,
 * commonly will reside in some SRAM memory.
 */
#ifndef __AL_GENERAL_SHARED_DATA_H__
#define __AL_GENERAL_SHARED_DATA_H__

#include "al_hal_plat_types.h"

/* The expected magic number for validating the shared data */
#define AL_GENERAL_SHARED_DATA_MN		0xA7
#define CURRENT_SENSOR_MAX_CHANNELS		2
#define AL_GENERAL_SHARED_MAC_ADDR_LEN		6
#define AL_GENERAL_SHARED_MAC_ADDR_NUM		4

/* stage2 secure_fail_reason definition */
#define AL_STG2_SECURE_FAIL_REASON_TPM_ACCESS_ERROR		AL_BIT(0)
#define AL_STG2_SECURE_FAIL_REASON_UC_COMM_ERROR		AL_BIT(1)
#define AL_STG2_SECURE_FAIL_REASON_EEPROM_UPDATE_ERROR		AL_BIT(2)
/* u-boot secure_fail_reason definition */
#define AL_STG2_SECURE_FAIL_REASON_UBOOT_TPM_ACCESS_ERROR		AL_BIT(7)

#define AL_UC_SHARED_HAS_FRU				1

/* The expected magic number for validating al temp sensors */
#define AL_UC_SHARED_AL_TEMP_MN				0x4747
#define AL_UC_SHARED_NUM_OF_AL_TEMP_SENSORS		4
#define AL_UC_SHARED_NUM_OF_AL_VOLT_SENSORS		3
#define AL_UC_SHARED_NUM_OF_DRAM_TEMP_SENSORS		3
#define AL_UC_SHARED_NUM_OF_BUS_VOLT			3
#define AL_UC_SHARED_NUM_OF_SHUNT_VOLT			3

/* Length of fru shared struct arrays */
#define AL_UC_SHARED_FRU_MN				0x1C361C36
#define AL_UC_SHARED_MAC_ADDR_LEN_BYTES			6
#define AL_UC_SHARED_PCB_REV_ID_LEN_BYTES		7
#define AL_UC_SHARED_BOARD_SN_LEN_BYTES			16
#define AL_UC_SHARED_SYSTEM_PN_LEN_BYTES		15
#define AL_UC_SHARED_NUM_OF_MAC_ADDR			8

/* Telemetry shared struct, aligned to 4 bytes */
struct al_uc_shared_telemetry_data {
	/* Sanity check parameters */
	uint16_t al_temp_magic_num;
	uint16_t al_temp_liveness_counter;

	/* Alpine temp sensors */
	/* local, remote0, remote1, remote2 */
	uint8_t al_temp_sensors[AL_UC_SHARED_NUM_OF_AL_TEMP_SENSORS];

	/* Alpine temp sensors for cpu, core */
	uint8_t al_cpu_temp;
	uint8_t al_core_temp;

	uint8_t al_telemetry_reserved_1[2];

	/* Alpine volt sensors for cpu, core */
	uint16_t al_cpu_volt;
	uint16_t al_core_volt;

	/* Alpine current sensors for cpu,core */
	uint16_t al_cpu_curr;
	uint16_t al_core_curr;

	/* Alpine volt sensor for avdd */
	uint16_t al_avdd;

	uint8_t al_telemetry_reserved_2[2];

	/* Dram temp sensors */
	/* dram a, dram b, dram pcb */
	uint8_t dram_temp_sensors[AL_UC_SHARED_NUM_OF_DRAM_TEMP_SENSORS];
	uint8_t temp_dram_reserved;

	/* Bus voltage */
	/* ch 0 1 2 */
	uint16_t bus_volt[AL_UC_SHARED_NUM_OF_BUS_VOLT];
	uint8_t bus_volt_reserved[2];

	/* Shunt voltage */
	/* ch 0 1 2 */
	uint16_t shunt_volt[AL_UC_SHARED_NUM_OF_SHUNT_VOLT];
	uint8_t shunt_volt_reserved[2];
};

struct al_uc_shared_mac_addr_data {
	uint8_t addr[AL_UC_SHARED_MAC_ADDR_LEN_BYTES];
};

/* Fru shared struct, aligned to 4 bytes */
struct al_uc_shared_fru_data {
	/* Magic Number */
	uint32_t magic_number;

	/* Customer revision id */
	uint8_t customer_rev_id;
	uint8_t customer_rev_id_reserved[3];

	/* Mac Addresses */
	struct al_uc_shared_mac_addr_data mac_addr[AL_UC_SHARED_NUM_OF_MAC_ADDR];

	/* Board assembly-ID */
	uint8_t pcb_rev_id[AL_UC_SHARED_PCB_REV_ID_LEN_BYTES];
	uint8_t pcb_rev_id_reserved;

	/* Specific Board S/N */
	uint8_t board_sn[AL_UC_SHARED_BOARD_SN_LEN_BYTES];

	/* System P/N */
	uint8_t system_pn[AL_UC_SHARED_SYSTEM_PN_LEN_BYTES];
	uint8_t system_pn_reserved;
};

struct al_uc_shared_sdr_data {
	uint16_t k2_ver_1;
	uint16_t k2_ver_2;
	uint16_t k2_ver_3;
	uint16_t k2_ver_4;
	uint16_t k2_generic_1;
	uint16_t k2_generic_2;
	uint16_t k2_generic_3;
	uint16_t k2_generic_4;
	uint16_t k2_generic_5;
	uint16_t k2_generic_6;
	uint16_t k2_crt_err;
	uint16_t k2_uncrt_err;
	uint16_t k2_mem_crr_err;
	uint16_t k2_mem_uncrr_err;
	uint16_t k2_pcie_crr_err;
	uint16_t k2_pcie_uncrr_err;
	uint16_t k2_boot_err;
	uint16_t k2_watchdog;
};

/* Micro controller v3 shared struct, aligned to 4 bytes */
struct al_uc_shared_data {
	struct al_uc_shared_fru_data fru;

	uint8_t reserved1[256 - sizeof(struct al_uc_shared_fru_data)];

	struct al_uc_shared_telemetry_data telemetry;

	uint8_t reserved2[256 - sizeof(struct al_uc_shared_telemetry_data)];

	struct al_uc_shared_sdr_data sdr;

	uint8_t reserved3[256 - sizeof(struct al_uc_shared_sdr_data)];
};

struct al_current_sensor_data {
	uint16_t electric_current;
	uint16_t electric_voltage;
};

enum al_stg2_shared_data_flag {
	AL_STG2_SHARED_DATA_FLAG_XMODEM_LOAD_ENFORCE,
	AL_STG2_SHARED_DATA_FLAG_SPI_LOAD_ENFORCE,
	AL_STG2_SHARED_DATA_FLAG_NITRO_MODE,
	AL_STG2_SHARED_DATA_FLAG_UBOOT_USER_BOOT_ABORT
};

struct al_stage2_shared_data {
	uint8_t magic_num;

	uint8_t flags;

	uint8_t secure_fail_reason;

	uint8_t reserved[29];
};

struct al_general_shared_mac_addr_data {
	uint8_t magic_num;
	uint8_t addr[AL_GENERAL_SHARED_MAC_ADDR_LEN];
	uint8_t reserved[1];
};

struct al_general_shared_data {
	uint8_t	magic_num;

	/* Boot app version */
	uint8_t boot_app_major_ver;
	uint8_t boot_app_minor_ver;
	uint8_t boot_app_fix_ver;

	/* U-Boot version */
	uint8_t uboot_major_ver;
	uint8_t uboot_minor_ver;
	uint8_t uboot_fix_ver;

	uint8_t reserved1;

	/* Port status should be aligned to 4 bytes boundary */
	uint8_t port_status[4]; /* 0 link down ,1 link up */

	uint32_t board_rev_id;

	/* PCI VendorID & DeviceID */
	uint16_t vendor_id;
	uint16_t device_id;

	/* APCEA version */
	uint8_t apcea_major_ver;
	uint8_t apcea_minor_ver;
	uint8_t apcea_fix_ver;

	/* iPXE version */
	uint8_t ipxe_major_ver;
	uint8_t ipxe_minor_ver;
	uint8_t ipxe_fix_ver;

	uint8_t reserved2[6];

	/* Diagnostics application */
	uint32_t diag_indication_mn;
	uint32_t diag_indication_status;

	/* Current sensor readings, filled by app, read by uC for exposing to BMC over SMBus */
	struct al_current_sensor_data current_sensor_ch[CURRENT_SENSOR_MAX_CHANNELS];

	/* Temperature sensor readings, filled by app, read by uC for exposing to BMC over SMBus */
	uint16_t temp_sensor;

	uint8_t reserved3[2];

	/* uc shared data, filled using err event service
	 * read by uC for exposing to BMC over SMBUS */
	uint8_t critical_err;
	uint8_t uncritical_err;
	uint8_t boot_err;
	uint8_t watchdog;

	uint8_t mem_corr_err;
	uint8_t mem_uncorr_err;
	uint8_t pcie_corr_err;
	uint8_t pcie_uncorr_err;

	uint8_t reserved4[4];

	/* Shared data that is initialized in stage2 */
	struct al_stage2_shared_data stage2_shared_data;

	struct al_general_shared_mac_addr_data mac_addr_data[AL_GENERAL_SHARED_MAC_ADDR_NUM];
};

/**
 * Initialize the shared data struct
 *
 * All the fields but "magic_num" will be set to zero.
 * "magic_num" will be set to AL_GENERAL_SHARED_DATA_MN
 *
 * @param	shared_data
 *		A pointer to the shared data to be initialized
 */
void al_general_shared_data_init(
	struct al_general_shared_data		*shared_data);

/**
 * Initialize the stage2_shared_data struct
 *
 * @param	shared_data
 *		A pointer to an initialized shared data
 */
void al_stg2_shared_data_init(
	struct al_general_shared_data		*shared_data);

/**
 * Get a flag value from the stage2_shared_data struct
 *
 * @param	shared_data
 *		A pointer to an initialized shared data
 * @param	flag
 *		The flag for which the value is required
 *
 * @returns	The flag value if it is valid, AL_FALSE otherwise
 */
al_bool al_stg2_shared_data_flag_get(
	struct al_general_shared_data		*shared_data,
	enum al_stg2_shared_data_flag		flag);

/**
 * Set a stage2_shared_data flag value
 *
 * @param	shared_data
 *		A pointer to an initialized shared data
 * @param	flag
 *		The flag for which the value will be set
 * @param	flag_value
 *		The flag value to be set
 */
void al_stg2_shared_data_flag_set(
	struct al_general_shared_data		*shared_data,
	enum al_stg2_shared_data_flag		flag,
	al_bool					flag_value);

/**
 * Set MAC address
 * @param	shared_data
 *		A pointer to an initialized shared data
 * @param	mac_addr
 *		A pointer to mac address
 * @param	mac_addr_num
 *		Num of the mac address to set
 */
void al_general_shared_data_mac_addr_set(
	struct al_general_shared_data		*shared_data,
	uint8_t					*mac_addr,
	unsigned int				mac_addr_num);

/**
 * Get MAC address
 * @param	shared_data
 *		A pointer to an initialized shared data
 * @param	mac_addr_num
 *		Number of mac address we want to get
 * @param	mac_addr
 *		Pointer where to write the mac addr
 * @return	true if mac address exists, false otherwise
 */
al_bool al_general_shared_data_mac_addr_get(
	struct al_general_shared_data		*shared_data,
	unsigned int				mac_addr_num,
	uint8_t					*mac_addr);

#endif
