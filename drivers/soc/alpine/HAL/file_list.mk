#sources of the HAL drivers
HAL_DRIVER_SOURCES = \
	$(HAL_TOP)/drivers/iofic/al_hal_iofic.c \
	$(HAL_TOP)/drivers/udma/al_hal_udma_main.c \
	$(HAL_TOP)/drivers/udma/al_hal_udma_config.c \
	$(HAL_TOP)/drivers/udma/al_hal_udma_iofic.c \
	$(HAL_TOP)/drivers/udma/al_hal_m2m_udma.c \
	$(HAL_TOP)/drivers/udma/al_hal_udma_debug.c \
	$(HAL_TOP)/drivers/udma/al_hal_msg_ipc.c	\
	$(HAL_TOP)/drivers/eth/al_hal_eth_main.c \
	$(HAL_TOP)/drivers/eth/al_hal_eth_mac.c \
	$(HAL_TOP)/drivers/eth/al_hal_eth_mac_internal.c \
	$(HAL_TOP)/drivers/eth/al_hal_eth_mac_v1_v2.c \
	$(HAL_TOP)/drivers/eth/al_hal_eth_mac_v3.c \
	$(HAL_TOP)/drivers/eth/al_hal_eth_mac_v4.c \
	$(HAL_TOP)/drivers/eth/al_hal_eth_kr.c \
	$(HAL_TOP)/drivers/eth/al_hal_eth_epe.c \
	$(HAL_TOP)/drivers/eth/al_hal_eth_common.c \
	$(HAL_TOP)/drivers/eth/al_hal_eth_field.c \
	$(HAL_TOP)/drivers/eth/al_hal_eth_rfw.c \
	$(HAL_TOP)/drivers/ssm/al_hal_ssm.c \
	$(HAL_TOP)/drivers/ssm/al_hal_ssm_raid.c	\
	$(HAL_TOP)/drivers/ssm/al_hal_ssm_crypto.c \
	$(HAL_TOP)/drivers/ssm/al_hal_ssm_crypto_cfg.c \
	$(HAL_TOP)/drivers/ssm/al_hal_ssm_crc_memcpy.c \
	$(HAL_TOP)/drivers/serdes/al_hal_serdes_hssp.c \
	$(HAL_TOP)/drivers/serdes/al_hal_serdes_25g.c \
	$(HAL_TOP)/drivers/serdes/al_hal_serdes_avg.c \
	$(HAL_TOP)/drivers/serdes/al_hal_serdes_hssp_init.c \
	$(HAL_TOP)/drivers/serdes/al_hal_serdes_25g_init.c \
	$(HAL_TOP)/drivers/serdes/al_hal_serdes_avg_init.c \
	$(HAL_TOP)/drivers/serdes/al_hal_serdes_avg_utils.c \
	$(HAL_TOP)/drivers/serdes/al_hal_serdes_snps.c \
	$(HAL_TOP)/drivers/serdes/al_hal_serdes_snps_init.c \
	$(HAL_TOP)/drivers/pcie/al_hal_pcie.c \
	$(HAL_TOP)/drivers/pcie/al_hal_pcie_reg_ptr_set_rev1.c \
	$(HAL_TOP)/drivers/pcie/al_hal_pcie_reg_ptr_set_rev2.c \
	$(HAL_TOP)/drivers/pcie/al_hal_pcie_reg_ptr_set_rev3.c \
	$(HAL_TOP)/drivers/pcie/al_hal_pcie_reg_ptr_set_rev4.c \
	$(HAL_TOP)/drivers/pcie/al_hal_pcie_reg_ptr_set_rev5.c \
	$(HAL_TOP)/drivers/pcie/al_hal_pcie_interrupts.c \
	$(HAL_TOP)/ddr/src/al_hal_ddr.c \
	$(HAL_TOP)/ddr/src/al_hal_ddr_alpine_v1.c \
	$(HAL_TOP)/ddr/src/al_hal_ddr_alpine_v2.c \
	$(HAL_TOP)/ddr/src/al_hal_ddr_alpine_v3.c \
	$(HAL_TOP)/ddr/src/al_hal_ddr_init_alpine_v1.c \
	$(HAL_TOP)/ddr/src/al_hal_ddr_init_alpine_v2.c \
	$(HAL_TOP)/ddr/src/al_hal_ddr_init_alpine_v3.c \
	$(HAL_TOP)/ddr/src/al_hal_ddr_pmu.c \
	$(HAL_TOP)/drivers/pbs/al_hal_pbs_utils.c \
	$(HAL_TOP)/drivers/pbs/al_hal_muio_mux.c \
	$(HAL_TOP)/drivers/pbs/al_hal_spi.c \
	$(HAL_TOP)/drivers/pbs/al_hal_nand.c \
	$(HAL_TOP)/drivers/pbs/al_hal_nand_dma.c \
	$(HAL_TOP)/drivers/pbs/al_hal_bootstrap.c \
	$(HAL_TOP)/drivers/pbs/al_hal_gpio.c \
	$(HAL_TOP)/drivers/pbs/al_hal_sgpo.c \
	$(HAL_TOP)/drivers/pbs/al_hal_i2c.c \
	$(HAL_TOP)/drivers/pbs/al_hal_uart.c \
	$(HAL_TOP)/drivers/pbs/al_hal_addr_map.c \
	$(HAL_TOP)/drivers/pbs/al_hal_tdm.c \
	$(HAL_TOP)/drivers/pbs/al_hal_sbus.c \
	$(HAL_TOP)/drivers/ring/al_hal_pll.c \
	$(HAL_TOP)/drivers/sys_services/al_hal_timer.c \
	$(HAL_TOP)/drivers/sys_services/al_hal_thermal_sensor.c \
	$(HAL_TOP)/drivers/sys_services/al_hal_vt_sensor.c \
	$(HAL_TOP)/drivers/sys_services/al_hal_watchdog.c \
	$(HAL_TOP)/drivers/sys_services/al_hal_otp.c \
	$(HAL_TOP)/drivers/sys_fabric/al_hal_sys_fabric_utils.c \
	$(HAL_TOP)/drivers/sys_fabric/al_hal_sys_fabric_utils_v1_v2.c \
	$(HAL_TOP)/drivers/sys_fabric/al_hal_sys_fabric_utils_v3.c \
	$(HAL_TOP)/drivers/sys_fabric/al_hal_sys_fabric_pasw.c \
	$(HAL_TOP)/drivers/sys_fabric/al_hal_ccu_pmu.c \
	$(HAL_TOP)/drivers/ring/al_hal_cmos.c \
	$(HAL_TOP)/drivers/udma_fast/al_hal_udma_fast.c \
	$(HAL_TOP)/drivers/io_fabric/al_hal_unit_adapter.c \
	$(HAL_TOP)/drivers/pmdt/al_hal_pmdt_pmu.c \
	$(HAL_TOP)/drivers/pmdt/al_hal_pmdt_axi_mon.c \
	$(HAL_TOP)/drivers/pmdt/al_hal_pmdt_ela.c \
	$(HAL_TOP)/drivers/pmdt/al_hal_pmdt_ela_config.c \
	$(HAL_TOP)/drivers/pmdt/al_hal_pmdt_cctm.c \

#sources of the init files compiled in the HAL itself
HAL_INIT_SOURCES_GENERIC = \
	$(HAL_TOP)/services/serdes/al_serdes.c \
	$(HAL_TOP)/services/serdes/al_serdes_init.c \
	$(HAL_TOP)/services/shared_mem/al_general_shared_data.c \
	$(HAL_TOP)/services/eth/al_eth_lm_retimer.c \
	$(HAL_TOP)/services/eth/internal/al_eth_lm_retimer_br210.c \
	$(HAL_TOP)/services/eth/internal/al_eth_lm_retimer_br410.c \
	$(HAL_TOP)/services/eth/internal/al_eth_lm_retimer_ds25.c \
	$(HAL_TOP)/services/eth/al_init_eth_lm.c \
	$(HAL_TOP)/services/eth/al_init_eth_kr.c \
	$(HAL_TOP)/services/eth/al_eth_group_lm.c \
	$(HAL_TOP)/services/eth/al_eth_switch.c \
	$(HAL_TOP)/services/eth/al_eth_v4_lm.c \
	$(HAL_TOP)/services/eth/al_eth_aux.c \
	$(HAL_TOP)/services/pcie/al_init_pcie.c \
	$(HAL_TOP)/services/pcie/al_init_pcie_debug.c \
	$(HAL_TOP)/services/sys_fabric/al_init_sys_fabric.c \
	$(HAL_TOP)/services/sys_fabric/al_init_l2cache_aarch32.S \
	$(HAL_TOP)/services/sys_fabric/al_init_l2cache_aarch64.S \
	$(HAL_TOP)/services/sys_fabric/al_init_neon_aarch32.S \
	$(HAL_TOP)/services/flash_contents/al_flash_contents.c \
	$(HAL_TOP)/services/dram_margins/al_dram_margins.c \
	$(HAL_TOP)/services/bootstrap/al_bootstrap.c \
	$(HAL_TOP)/services/iomap_dynamic/al_hal_iomap_dynamic.c \
	$(HAL_TOP)/services/trace/al_trace.c \
	$(HAL_TOP)/services/err_events/al_err_events_common.c \
	$(HAL_TOP)/services/err_events/al_err_events.c \
	$(HAL_TOP)/services/err_events/al_err_events_udma.c \
	$(HAL_TOP)/services/err_events/al_err_events_io_fabric.c \
	$(HAL_TOP)/services/err_events/al_err_events_ssm.c \
	$(HAL_TOP)/services/err_events/al_err_events_eth.c \
	$(HAL_TOP)/services/err_events/al_err_events_eth_epe.c \
	$(HAL_TOP)/services/err_events/al_err_events_sys_fabric.c \
	$(HAL_TOP)/services/err_events/al_err_events_pcie.c \
	$(HAL_TOP)/services/tpm/al_tpm.c \
	$(HAL_TOP)/services/tpm/al_tpm_tis12.c \
	$(HAL_TOP)/services/tpm/al_tpm_tis20.c \
	$(HAL_TOP)/services/tpm/al_tpm_if_i2c.c \
	$(HAL_TOP)/services/tpm/al_tpm_if_spi.c \
	$(HAL_TOP)/services/nand/al_nand_early.c \
	$(HAL_TOP)/services/otp_storage/al_otp_storage.c \
	$(HAL_TOP)/services/otp_storage/al_otp_storage_layout_get_v2.c \
	$(HAL_TOP)/services/otp_storage/al_otp_storage_layout_get_v3.c \
	$(HAL_TOP)/services/perf_params/al_perf_params.c \
	$(HAL_TOP)/services/monitor_mgmt/aarch32/monitor_mgmt.S \
	$(HAL_TOP)/services/monitor_mgmt/aarch64/monitor_mgmt.S \
	$(HAL_TOP)/services/uc_shared/al_uc_shared.c \
	$(HAL_TOP)/services/uc_shared/al_uc_cmds.c \
	$(HAL_TOP)/services/dt_parse/al_dt_parse.c \
	$(HAL_TOP)/services/dt_parse/al_dt_parse_internal.c \
	$(HAL_TOP)/services/dt_parse/al_dt_parse_eth.c \
	$(HAL_TOP)/services/dt_parse/al_dt_parse_pcie.c \
	$(HAL_TOP)/services/crc/al_crc.c \

HAL_INIT_SOURCES_ARM = \

HAL_INIT_SOURCES_AARCH64 = \

#include path that a HAL user needs
HAL_USER_INCLUDE_PATH = \
	-I$(HAL_TOP)/include/common \
	-I$(HAL_TOP)/include/io_fabric \
	-I$(HAL_TOP)/include/iofic \
	-I$(HAL_TOP)/include/udma\
	-I$(HAL_TOP)/include/ssm \
	-I$(HAL_TOP)/include/eth \
	-I$(HAL_TOP)/include/pbs \
	-I$(HAL_TOP)/include/ring \
	-I$(HAL_TOP)/include/sys_services \
	-I$(HAL_TOP)/include/serdes \
	-I$(HAL_TOP)/include/pcie \
	-I$(HAL_TOP)/include/sys_fabric \
	-I$(HAL_TOP)/ddr/include \
	-I$(HAL_TOP)/include/udma_fast \
	-I$(HAL_TOP)/include/pmdt \

#include path additioons for compiling the drivers
HAL_DRIVER_INCLUDE_PATH = \
	-I$(HAL_TOP)/ddr/src \
	-I$(HAL_TOP)/drivers/eth \
	-I$(HAL_TOP)/drivers/pbs \
	-I$(HAL_TOP)/drivers/pcie \
	-I$(HAL_TOP)/drivers/pmdt \
	-I$(HAL_TOP)/drivers/ring \
	-I$(HAL_TOP)/drivers/serdes/ \
	-I$(HAL_TOP)/drivers/ssm \
	-I$(HAL_TOP)/drivers/sys_fabric \
	-I$(HAL_TOP)/drivers/sys_services \

#include path additioons for using init functions
HAL_INIT_INCLUDE_PATH = \
	-I$(HAL_TOP)/services/sysreg/ \
	-I$(HAL_TOP)/services/gic/ \
	-I$(HAL_TOP)/services/eth/ \
	-I$(HAL_TOP)/services/eth/auto_gen/ \
	-I$(HAL_TOP)/services/flash_contents \
	-I$(HAL_TOP)/services/sys_fabric/ \
	-I$(HAL_TOP)/services/cpu_resume/ \
	-I$(HAL_TOP)/services/pcie/ \
	-I$(HAL_TOP)/services/serdes/ \
	-I$(HAL_TOP)/services/dram_margins/ \
	-I$(HAL_TOP)/services/shared_mem/ \
	-I$(HAL_TOP)/services/eeprom/ \
	-I$(HAL_TOP)/services/bootstrap/ \
	-I$(HAL_TOP)/services/iomap_dynamic/ \
	-I$(HAL_TOP)/services/trace/ \
	-I$(HAL_TOP)/services/err_events/ \
	-I$(HAL_TOP)/services/tpm/ \
	-I$(HAL_TOP)/services/nand/ \
	-I$(HAL_TOP)/services/otp_storage/ \
	-I$(HAL_TOP)/services/monitor_mgmt/ \
	-I$(HAL_TOP)/services/perf_params/ \
	-I$(HAL_TOP)/services/uc_shared \
	-I$(HAL_TOP)/services/dt_parse \
	-I$(HAL_TOP)/services/crc \

#include path additions for using init functions - aarch32
HAL_INIT_INCLUDE_PATH_AARCH32 = \
	-I$(HAL_TOP)/services/monitor_mgmt/aarch32/ \

#include path additions for using init functions - aarch64
HAL_INIT_INCLUDE_PATH_AARCH64 = \
	-I$(HAL_TOP)/services/monitor_mgmt/aarch64/ \

HAL_PLATFORM_INCLUDE_PATH_ALPINE_V1 = \
	-I$(HAL_TOP)/platform/alpine_v1/include

HAL_PLATFORM_SOURCES_ALPINE_V1 = \

HAL_PLATFORM_INCLUDE_PATH_ALPINE_V2 = \
	-I$(HAL_TOP)/platform/alpine_v2/include

HAL_PLATFORM_SOURCES_ALPINE_V2 = \

HAL_PLATFORM_INCLUDE_PATH_ALPINE_V3 = \
        -I$(HAL_TOP)/platform/alpine_v3/include/ \
        -I$(HAL_TOP)/services/pmdt/ \

HAL_PLATFORM_SOURCES_ALPINE_V3 = \
	$(HAL_TOP)/platform/alpine_v3/src/pmdt/al_hal_pmdt_map.c \
	$(HAL_TOP)/services/pmdt/al_pmdt.c \
	$(HAL_TOP)/services/pmdt/al_pmdt_config.c \

HAL_PLATFORM_INCLUDE_PATH_ALPINE_V4 = \
        -I$(HAL_TOP)/platform/alpine_v4/include/ \

HAL_PLATFORM_SOURCES_ALPINE_V4 = \

# Platform specific that supports multi-platform compile
HAL_DRIVER_SOURCES += \
	$(HAL_TOP)/platform/alpine_v1/src/iomap_dynamic/al_hal_iomap_dynamic.c \
	$(HAL_TOP)/platform/alpine_v2/src/iomap_dynamic/al_hal_iomap_dynamic.c \
	$(HAL_TOP)/platform/alpine_v3/src/iomap_dynamic/al_hal_iomap_dynamic.c \
	$(HAL_TOP)/platform/alpine_v4/src/iomap_dynamic/al_hal_iomap_dynamic.c \

# include proprietary file list if needed
ifneq ($(AL_HAL_EX),)
include $(HAL_TOP)/proprietary/file_list_ex.mk
endif
