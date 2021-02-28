#!/bin/bash

set -e

export OUTPUT_DIR=`mktemp -d`
export INCLUDES=
export INCLUDES="$INCLUDES -I./"
export INCLUDES="$INCLUDES -I$OUTPUT_DIR/"
export INCLUDES="$INCLUDES -I./include/"
export INCLUDES="$INCLUDES -I./include/pbs"
export INCLUDES="$INCLUDES -I./include/common/"
export INCLUDES="$INCLUDES -I./platform/alpine_v3/include/"
export INCLUDES="$INCLUDES -I./include/plat_api/sample/"
export INCLUDES="$INCLUDES -I./ddr/include/"
export INCLUDES="$INCLUDES -I./include/iofic/"
export INCLUDES="$INCLUDES -I./include/sys_services/"
export DEFINES=
export DEFINES="$DEFINES -DAL_DEV_ID_ALPINE_V1=0"
export DEFINES="$DEFINES -DAL_DEV_ID_ALPINE_V2=0"
export DEFINES="$DEFINES -DAL_DEV_ID_ALPINE_V3=0"
export DEFINES="$DEFINES -DAL_DEV_ID=AL_DEV_ID_ALPINE_V3"
export DEFINES="$DEFINES -DAL_DEV_REV_ID=1"

mkdir -p $OUTPUT_DIR/
echo Temporary output directory: $OUTPUT_DIR

export SOURCES=

./reg_xref.pl include/pbs/al_hal_pbs_regs.h al_pbs_regs pbs > $OUTPUT_DIR/reg_print_offsets_pbs.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_pbs.c"

./reg_xref.pl drivers/pbs/al_hal_i2c_regs.h al_i2c_regs i2c_pld > $OUTPUT_DIR/reg_print_offsets_i2c_pld.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_i2c_pld.c"

./reg_xref.pl drivers/pbs/al_hal_i2c_regs.h al_i2c_regs i2c_gen > $OUTPUT_DIR/reg_print_offsets_i2c_gen.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_i2c_gen.c"

./reg_xref.pl drivers/pbs/al_hal_nand_regs.h al_nand_regs nand_ctrl > $OUTPUT_DIR/reg_print_offsets_nand_ctrl.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_nand_ctrl.c"

./reg_xref.pl drivers/pbs/al_hal_nand_regs.h al_nand_wrap_regs nand_wrapper > $OUTPUT_DIR/reg_print_offsets_nand_wrapper.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_nand_wrapper.c"

./reg_xref.pl drivers/pbs/al_hal_spi_regs.h al_spi_regs spi_slave > $OUTPUT_DIR/reg_print_offsets_spi_slave.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_spi_slave.c"

./reg_xref.pl drivers/pbs/al_hal_spi_regs.h al_spi_regs spi_master > $OUTPUT_DIR/reg_print_offsets_spi_master.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_spi_master.c"

./reg_xref.pl drivers/pbs/al_hal_uart_regs.h al_uart_regs uart_0 > $OUTPUT_DIR/reg_print_offsets_uart_0.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_uart_0.c"

./reg_xref.pl drivers/pbs/al_hal_uart_regs.h al_uart_regs uart_1 > $OUTPUT_DIR/reg_print_offsets_uart_1.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_uart_1.c"

./reg_xref.pl drivers/pbs/al_hal_uart_regs.h al_uart_regs uart_2 > $OUTPUT_DIR/reg_print_offsets_uart_2.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_uart_2.c"

./reg_xref.pl drivers/pbs/al_hal_uart_regs.h al_uart_regs uart_3 > $OUTPUT_DIR/reg_print_offsets_uart_3.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_uart_3.c"

./reg_xref.pl drivers/pbs/al_hal_gpio_regs.h al_gpio_regs gpio_7_0 > $OUTPUT_DIR/reg_print_offsets_gpio_7_0.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_gpio_7_0.c"

./reg_xref.pl drivers/pbs/al_hal_gpio_regs.h al_gpio_regs gpio_15_8 > $OUTPUT_DIR/reg_print_offsets_gpio_15_8.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_gpio_15_8.c"

./reg_xref.pl drivers/pbs/al_hal_gpio_regs.h al_gpio_regs gpio_23_16 > $OUTPUT_DIR/reg_print_offsets_gpio_23_16.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_gpio_23_16.c"

./reg_xref.pl drivers/pbs/al_hal_gpio_regs.h al_gpio_regs gpio_31_24 > $OUTPUT_DIR/reg_print_offsets_gpio_31_24.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_gpio_31_24.c"

./reg_xref.pl drivers/pbs/al_hal_gpio_regs.h al_gpio_regs gpio_39_32 > $OUTPUT_DIR/reg_print_offsets_gpio_39_32.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_gpio_39_32.c"

./reg_xref.pl drivers/pbs/al_hal_gpio_regs.h al_gpio_regs gpio_47_40 > $OUTPUT_DIR/reg_print_offsets_gpio_47_40.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_gpio_47_40.c"

cpp drivers/pbs/al_hal_sgpo_regs.h $INCLUDES -D__AL_HAL_SGPO_H__ -D AL_SGPO_NUM_OF_GROUPS=4 | sed "s/^#.*//g" > $OUTPUT_DIR/sgpo_regs.h
./reg_xref.pl $OUTPUT_DIR/sgpo_regs.h al_sgpo_regs sgpo > $OUTPUT_DIR/reg_print_offsets_sgpo.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_sgpo.c"

./reg_xref.pl drivers/sys_services/al_hal_watchdog_regs.h al_watchdog_regs watchdog_0 > $OUTPUT_DIR/reg_print_offsets_watchdog_0.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_watchdog_0.c"

./reg_xref.pl drivers/sys_services/al_hal_watchdog_regs.h al_watchdog_regs watchdog_1 > $OUTPUT_DIR/reg_print_offsets_watchdog_1.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_watchdog_1.c"

./reg_xref.pl drivers/sys_services/al_hal_watchdog_regs.h al_watchdog_regs watchdog_2 > $OUTPUT_DIR/reg_print_offsets_watchdog_2.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_watchdog_2.c"

./reg_xref.pl drivers/sys_services/al_hal_watchdog_regs.h al_watchdog_regs watchdog_3 > $OUTPUT_DIR/reg_print_offsets_watchdog_3.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_watchdog_3.c"

./reg_xref.pl drivers/sys_services/al_hal_timer_regs.h al_timer_regs timer_0_0 > $OUTPUT_DIR/reg_print_offsets_timer_0_0.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_timer_0_0.c"

./reg_xref.pl drivers/sys_services/al_hal_timer_regs.h al_timer_regs timer_0_1 > $OUTPUT_DIR/reg_print_offsets_timer_0_1.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_timer_0_1.c"

./reg_xref.pl drivers/sys_services/al_hal_timer_regs.h al_timer_regs timer_1_0 > $OUTPUT_DIR/reg_print_offsets_timer_1_0.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_timer_1_0.c"

./reg_xref.pl drivers/sys_services/al_hal_timer_regs.h al_timer_regs timer_1_1 > $OUTPUT_DIR/reg_print_offsets_timer_1_1.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_timer_1_1.c"

./reg_xref.pl drivers/sys_services/al_hal_timer_regs.h al_timer_regs timer_2_0 > $OUTPUT_DIR/reg_print_offsets_timer_2_0.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_timer_2_0.c"

./reg_xref.pl drivers/sys_services/al_hal_timer_regs.h al_timer_regs timer_2_1 > $OUTPUT_DIR/reg_print_offsets_timer_2_1.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_timer_2_1.c"

./reg_xref.pl drivers/sys_services/al_hal_timer_regs.h al_timer_regs timer_3_0 > $OUTPUT_DIR/reg_print_offsets_timer_3_0.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_timer_3_0.c"

./reg_xref.pl drivers/sys_services/al_hal_timer_regs.h al_timer_regs timer_3_1 > $OUTPUT_DIR/reg_print_offsets_timer_3_1.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_timer_3_1.c"

cpp include/ring/al_hal_cmos_regs.h $INCLUDES | sed "s/^#.*//g" > $OUTPUT_DIR/cmos_regs.h
./reg_xref.pl $OUTPUT_DIR/cmos_regs.h al_cmos_regs cmos > $OUTPUT_DIR/reg_print_offsets_cmos.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_cmos.c"

./reg_xref.pl drivers/ring/al_hal_pll_regs.h al_pll_regs pll_sb > $OUTPUT_DIR/reg_print_offsets_pll_sb.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_pll_sb.c"

./reg_xref.pl drivers/ring/al_hal_pll_regs.h al_pll_regs pll_nb > $OUTPUT_DIR/reg_print_offsets_pll_nb.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_pll_nb.c"

./reg_xref.pl drivers/ring/al_hal_pll_regs.h al_pll_regs pll_cpu > $OUTPUT_DIR/reg_print_offsets_pll_cpu.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_pll_cpu.c"

tmp_output=$OUTPUT_DIR/reg_print_offsets_nb_service.c
./reg_xref.pl include/sys_fabric/al_hal_nb_regs_v3.h al_nb_regs nb_service > ${tmp_output}
mv ${tmp_output} ${tmp_output}.bk; grep -v "\->pmu" ${tmp_output}.bk > ${tmp_output}
export SOURCES="$SOURCES ${tmp_output}"

tmp_output=$OUTPUT_DIR/reg_print_offsets_nb_anpa_0.c
./reg_xref.pl include/sys_fabric/al_hal_anpa_regs.h al_anpa_regs nb_anpa_0 > ${tmp_output}
mv ${tmp_output} ${tmp_output}.bk; grep -v "\->pmu" ${tmp_output}.bk > ${tmp_output}
export SOURCES="$SOURCES ${tmp_output}"

tmp_output=$OUTPUT_DIR/reg_print_offsets_nb_anpa_1.c
./reg_xref.pl include/sys_fabric/al_hal_anpa_regs.h al_anpa_regs nb_anpa_1 > ${tmp_output}
mv ${tmp_output} ${tmp_output}.bk; grep -v "\->pmu" ${tmp_output}.bk > ${tmp_output}
export SOURCES="$SOURCES ${tmp_output}"

tmp_output=$OUTPUT_DIR/reg_print_offsets_nb_anpa_2.c
./reg_xref.pl include/sys_fabric/al_hal_anpa_regs.h al_anpa_regs nb_anpa_2 > ${tmp_output}
mv ${tmp_output} ${tmp_output}.bk; grep -v "\->pmu" ${tmp_output}.bk > ${tmp_output}
export SOURCES="$SOURCES ${tmp_output}"

tmp_output=$OUTPUT_DIR/reg_print_offsets_nb_anpa_3.c
./reg_xref.pl include/sys_fabric/al_hal_anpa_regs.h al_anpa_regs nb_anpa_3 > ${tmp_output}
mv ${tmp_output} ${tmp_output}.bk; grep -v "\->pmu" ${tmp_output}.bk > ${tmp_output}
export SOURCES="$SOURCES ${tmp_output}"

cpp drivers/ddr/al_hal_ddrc_regs.h $INCLUDES | sed "s/^#.*//g" > $OUTPUT_DIR/ddrc_regs.h
./reg_xref.pl $OUTPUT_DIR/ddrc_regs.h al_ddrc_regs ddrc_0 > $OUTPUT_DIR/reg_print_offsets_ddrc_0.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_ddrc_0.c"

cpp drivers/ddr/al_hal_ddr_ctrl_regs_alpine_v3.h $INCLUDES | sed "s/^#.*//g" > $OUTPUT_DIR/ddr_ctrl_regs.h
./reg_xref.pl $OUTPUT_DIR/ddr_ctrl_regs.h al_ddr_ctrl_regs_alpine_v3 ddr_ctrl_0 > $OUTPUT_DIR/reg_print_offsets_ddr_ctrl_0.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_ddr_ctrl_0.c"

cpp drivers/ddr/al_hal_ddr_phy_regs_alpine_v3.h $INCLUDES | sed "s/^#.*//g" > $OUTPUT_DIR/ddr_phy_regs.h
./reg_xref.pl $OUTPUT_DIR/ddr_phy_regs.h al_dwc_ddrphy_top_regs_alpine_v3 ddr_phy_0 > $OUTPUT_DIR/reg_print_offsets_ddr_phy_0.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_ddr_phy_0.c"

cpp drivers/ddr/al_hal_ddrc_regs.h $INCLUDES | sed "s/^#.*//g" > $OUTPUT_DIR/ddrc_regs.h
./reg_xref.pl $OUTPUT_DIR/ddrc_regs.h al_ddrc_regs ddrc_1 > $OUTPUT_DIR/reg_print_offsets_ddrc_1.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_ddrc_1.c"

cpp drivers/ddr/al_hal_ddr_ctrl_regs_alpine_v3.h $INCLUDES | sed "s/^#.*//g" > $OUTPUT_DIR/ddr_ctrl_regs.h
./reg_xref.pl $OUTPUT_DIR/ddr_ctrl_regs.h al_ddr_ctrl_regs_alpine_v3 ddr_ctrl_1 > $OUTPUT_DIR/reg_print_offsets_ddr_ctrl_1.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_ddr_ctrl_1.c"

cpp drivers/ddr/al_hal_ddr_phy_regs_alpine_v3.h $INCLUDES | sed "s/^#.*//g" > $OUTPUT_DIR/ddr_phy_regs.h
./reg_xref.pl $OUTPUT_DIR/ddr_phy_regs.h al_dwc_ddrphy_top_regs_alpine_v3 ddr_phy_1 > $OUTPUT_DIR/reg_print_offsets_ddr_phy_1.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_ddr_phy_1.c"

./reg_xref.pl drivers/io_fabric/al_hal_unit_adapter_gen_regs.h al_unit_adapter_regs eth0_adapter > $OUTPUT_DIR/reg_print_offsets_eth0_adapter.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_eth0_adapter.c"

tmp_output=$OUTPUT_DIR/reg_print_offsets_eth0_ec.c
./reg_xref.pl drivers/eth/al_hal_eth_ec_regs.h al_ec_regs eth0_ec > ${tmp_output}
mv ${tmp_output} ${tmp_output}.bk; grep -v "\->rmp_cfg" ${tmp_output}.bk > ${tmp_output}
mv ${tmp_output} ${tmp_output}.bk; grep -v "\->pmu_cfg" ${tmp_output}.bk > ${tmp_output}
mv ${tmp_output} ${tmp_output}.bk; grep -v "\->rfw_shared_cfg" ${tmp_output}.bk > ${tmp_output}
export SOURCES="$SOURCES ${tmp_output}"

./reg_xref.pl drivers/eth/al_hal_eth_mac_v4_regs.h al_eth_mac_v4_regs eth0_mac > $OUTPUT_DIR/reg_print_offsets_eth0_mac.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_eth0_mac.c"

cpp include/udma/al_hal_udma_regs.h $INCLUDES | sed "s/^#.*//g" > $OUTPUT_DIR/udma_regs.h

./reg_xref.pl $OUTPUT_DIR/udma_regs.h unit_regs_v4 eth0_udma0 > $OUTPUT_DIR/reg_print_offsets_eth0_udma0.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_eth0_udma0.c"

./reg_xref.pl drivers/io_fabric/al_hal_unit_adapter_gen_regs.h al_unit_adapter_regs eth1_adapter > $OUTPUT_DIR/reg_print_offsets_eth1_adapter.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_eth1_adapter.c"

tmp_output=$OUTPUT_DIR/reg_print_offsets_eth1_ec.c
./reg_xref.pl drivers/eth/al_hal_eth_ec_regs.h al_ec_regs eth1_ec > ${tmp_output}
mv ${tmp_output} ${tmp_output}.bk; grep -v "\->rmp_cfg" ${tmp_output}.bk > ${tmp_output}
mv ${tmp_output} ${tmp_output}.bk; grep -v "\->pmu_cfg" ${tmp_output}.bk > ${tmp_output}
mv ${tmp_output} ${tmp_output}.bk; grep -v "\->rfw_shared_cfg" ${tmp_output}.bk > ${tmp_output}
export SOURCES="$SOURCES ${tmp_output}"

./reg_xref.pl drivers/eth/al_hal_eth_mac_regs.h al_eth_mac_regs eth1_mac > $OUTPUT_DIR/reg_print_offsets_eth1_mac.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_eth1_mac.c"

./reg_xref.pl $OUTPUT_DIR/udma_regs.h unit_regs_v3 eth1_udma0 > $OUTPUT_DIR/reg_print_offsets_eth1_udma0.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_eth1_udma0.c"

./reg_xref.pl drivers/io_fabric/al_hal_unit_adapter_gen_regs.h al_unit_adapter_regs eth2_adapter > $OUTPUT_DIR/reg_print_offsets_eth2_adapter.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_eth2_adapter.c"

tmp_output=$OUTPUT_DIR/reg_print_offsets_eth2_ec.c
./reg_xref.pl drivers/eth/al_hal_eth_ec_regs.h al_ec_regs eth2_ec > ${tmp_output}
mv ${tmp_output} ${tmp_output}.bk; grep -v "\->rmp_cfg" ${tmp_output}.bk > ${tmp_output}
mv ${tmp_output} ${tmp_output}.bk; grep -v "\->pmu_cfg" ${tmp_output}.bk > ${tmp_output}
mv ${tmp_output} ${tmp_output}.bk; grep -v "\->rfw_shared_cfg" ${tmp_output}.bk > ${tmp_output}
export SOURCES="$SOURCES ${tmp_output}"

./reg_xref.pl drivers/eth/al_hal_eth_mac_v4_regs.h al_eth_mac_v4_regs eth2_mac > $OUTPUT_DIR/reg_print_offsets_eth2_mac.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_eth2_mac.c"

./reg_xref.pl $OUTPUT_DIR/udma_regs.h unit_regs_v4 eth2_udma0 > $OUTPUT_DIR/reg_print_offsets_eth2_udma0.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_eth2_udma0.c"

./reg_xref.pl drivers/io_fabric/al_hal_unit_adapter_gen_regs.h al_unit_adapter_regs eth3_adapter > $OUTPUT_DIR/reg_print_offsets_eth3_adapter.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_eth3_adapter.c"

tmp_output=$OUTPUT_DIR/reg_print_offsets_eth3_ec.c
./reg_xref.pl drivers/eth/al_hal_eth_ec_regs.h al_ec_regs eth3_ec > ${tmp_output}
mv ${tmp_output} ${tmp_output}.bk; grep -v "\->rmp_cfg" ${tmp_output}.bk > ${tmp_output}
mv ${tmp_output} ${tmp_output}.bk; grep -v "\->pmu_cfg" ${tmp_output}.bk > ${tmp_output}
mv ${tmp_output} ${tmp_output}.bk; grep -v "\->rfw_shared_cfg" ${tmp_output}.bk > ${tmp_output}
export SOURCES="$SOURCES ${tmp_output}"

./reg_xref.pl drivers/eth/al_hal_eth_mac_regs.h al_eth_mac_regs eth3_mac > $OUTPUT_DIR/reg_print_offsets_eth3_mac.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_eth3_mac.c"

./reg_xref.pl $OUTPUT_DIR/udma_regs.h unit_regs_v3 eth3_udma0 > $OUTPUT_DIR/reg_print_offsets_eth3_udma0.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_eth3_udma0.c"

./reg_xref.pl drivers/io_fabric/al_hal_unit_adapter_gen_regs.h al_unit_adapter_regs eth4_adapter > $OUTPUT_DIR/reg_print_offsets_eth4_adapter.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_eth4_adapter.c"

tmp_output=$OUTPUT_DIR/reg_print_offsets_eth4_ec.c
./reg_xref.pl drivers/eth/al_hal_eth_ec_regs.h al_ec_regs eth4_ec > ${tmp_output}
mv ${tmp_output} ${tmp_output}.bk; grep -v "\->rmp_cfg" ${tmp_output}.bk > ${tmp_output}
mv ${tmp_output} ${tmp_output}.bk; grep -v "\->pmu_cfg" ${tmp_output}.bk > ${tmp_output}
mv ${tmp_output} ${tmp_output}.bk; grep -v "\->rfw_shared_cfg" ${tmp_output}.bk > ${tmp_output}
export SOURCES="$SOURCES ${tmp_output}"

./reg_xref.pl drivers/eth/al_hal_eth_mac_v4_regs.h al_eth_mac_v4_regs eth4_mac > $OUTPUT_DIR/reg_print_offsets_eth4_mac.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_eth4_mac.c"

./reg_xref.pl $OUTPUT_DIR/udma_regs.h unit_regs_v4 eth4_udma0 > $OUTPUT_DIR/reg_print_offsets_eth4_udma0.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_eth4_udma0.c"

./reg_xref.pl drivers/io_fabric/al_hal_unit_adapter_gen_regs.h al_unit_adapter_regs eth5_adapter > $OUTPUT_DIR/reg_print_offsets_eth5_adapter.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_eth5_adapter.c"

tmp_output=$OUTPUT_DIR/reg_print_offsets_eth5_ec.c
./reg_xref.pl drivers/eth/al_hal_eth_ec_regs.h al_ec_regs eth5_ec > ${tmp_output}
mv ${tmp_output} ${tmp_output}.bk; grep -v "\->rmp_cfg" ${tmp_output}.bk > ${tmp_output}
mv ${tmp_output} ${tmp_output}.bk; grep -v "\->pmu_cfg" ${tmp_output}.bk > ${tmp_output}
mv ${tmp_output} ${tmp_output}.bk; grep -v "\->rfw_shared_cfg" ${tmp_output}.bk > ${tmp_output}
export SOURCES="$SOURCES ${tmp_output}"

./reg_xref.pl drivers/eth/al_hal_eth_mac_v4_regs.h al_eth_mac_v4_regs eth5_mac > $OUTPUT_DIR/reg_print_offsets_eth5_mac.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_eth5_mac.c"

./reg_xref.pl $OUTPUT_DIR/udma_regs.h unit_regs_v4 eth5_udma0 > $OUTPUT_DIR/reg_print_offsets_eth5_udma0.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_eth5_udma0.c"

./reg_xref.pl drivers/io_fabric/al_hal_unit_adapter_gen_regs.h al_unit_adapter_regs ssm0_adapter > $OUTPUT_DIR/reg_print_offsets_ssm0_adapter.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_ssm0_adapter.c"

./reg_xref.pl drivers/ssm/al_hal_ssm_crypto_regs.h crypto_regs ssm0_crypto > $OUTPUT_DIR/reg_print_offsets_ssm0_crypto.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_ssm0_crypto.c"

./reg_xref.pl drivers/ssm/al_hal_ssm_raid_regs.h raid_accelerator_regs ssm0_raid > $OUTPUT_DIR/reg_print_offsets_ssm0_raid.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_ssm0_raid.c"

./reg_xref.pl $OUTPUT_DIR/udma_regs.h unit_regs_v4 ssm0_udma0 > $OUTPUT_DIR/reg_print_offsets_ssm0_udma0.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_ssm0_udma0.c"

./reg_xref.pl drivers/io_fabric/al_hal_unit_adapter_gen_regs.h al_unit_adapter_regs ssm1_adapter > $OUTPUT_DIR/reg_print_offsets_ssm1_adapter.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_ssm1_adapter.c"

./reg_xref.pl drivers/ssm/al_hal_ssm_crypto_regs.h crypto_regs ssm1_crypto > $OUTPUT_DIR/reg_print_offsets_ssm1_crypto.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_ssm1_crypto.c"

./reg_xref.pl drivers/ssm/al_hal_ssm_raid_regs.h raid_accelerator_regs ssm1_raid > $OUTPUT_DIR/reg_print_offsets_ssm1_raid.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_ssm1_raid.c"

./reg_xref.pl $OUTPUT_DIR/udma_regs.h unit_regs_v4 ssm1_udma0 > $OUTPUT_DIR/reg_print_offsets_ssm1_udma0.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_ssm1_udma0.c"

./reg_xref.pl drivers/io_fabric/al_hal_unit_adapter_gen_regs.h al_unit_adapter_regs sata0_adapter > $OUTPUT_DIR/reg_print_offsets_sata0_adapter.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_sata0_adapter.c"

./reg_xref.pl drivers/io_fabric/al_hal_unit_adapter_gen_regs.h al_unit_adapter_regs sata1_adapter > $OUTPUT_DIR/reg_print_offsets_sata1_adapter.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_sata1_adapter.c"

./reg_xref.pl drivers/io_fabric/al_hal_unit_adapter_gen_regs.h al_unit_adapter_regs sata2_adapter > $OUTPUT_DIR/reg_print_offsets_sata2_adapter.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_sata2_adapter.c"

./reg_xref.pl drivers/io_fabric/al_hal_unit_adapter_gen_regs.h al_unit_adapter_regs sata3_adapter > $OUTPUT_DIR/reg_print_offsets_sata3_adapter.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_sata3_adapter.c"

./reg_xref.pl drivers/io_fabric/al_hal_unit_adapter_gen_regs.h al_unit_adapter_regs sata4_adapter > $OUTPUT_DIR/reg_print_offsets_sata4_adapter.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_sata4_adapter.c"

./reg_xref.pl drivers/io_fabric/al_hal_unit_adapter_gen_regs.h al_unit_adapter_regs sata5_adapter > $OUTPUT_DIR/reg_print_offsets_sata5_adapter.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_sata5_adapter.c"

./reg_xref.pl drivers/io_fabric/al_hal_unit_adapter_gen_regs.h al_unit_adapter_regs sata6_adapter > $OUTPUT_DIR/reg_print_offsets_sata6_adapter.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_sata6_adapter.c"

./reg_xref.pl drivers/io_fabric/al_hal_unit_adapter_gen_regs.h al_unit_adapter_regs sata7_adapter > $OUTPUT_DIR/reg_print_offsets_sata7_adapter.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_sata7_adapter.c"

echo "#define __iomem" > $OUTPUT_DIR/pcie_regs.h
cpp drivers/pcie/al_hal_pcie_regs.h $INCLUDES | sed "s/^#.*//g" >> $OUTPUT_DIR/pcie_regs.h

./reg_xref.pl $OUTPUT_DIR/pcie_regs.h al_pcie_rev4_regs pcie0 > $OUTPUT_DIR/reg_print_offsets_pcie0.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_pcie0.c"

./reg_xref.pl $OUTPUT_DIR/pcie_regs.h al_pcie_rev4_regs pcie1 > $OUTPUT_DIR/reg_print_offsets_pcie1.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_pcie1.c"

./reg_xref.pl $OUTPUT_DIR/pcie_regs.h al_pcie_rev4_regs pcie2 > $OUTPUT_DIR/reg_print_offsets_pcie2.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_pcie2.c"

./reg_xref.pl $OUTPUT_DIR/pcie_regs.h al_pcie_rev4_regs pcie3 > $OUTPUT_DIR/reg_print_offsets_pcie3.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_pcie3.c"

./reg_xref.pl $OUTPUT_DIR/pcie_regs.h al_pcie_rev4_regs pcie4 > $OUTPUT_DIR/reg_print_offsets_pcie4.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_pcie4.c"

./reg_xref.pl $OUTPUT_DIR/pcie_regs.h al_pcie_rev4_regs pcie5 > $OUTPUT_DIR/reg_print_offsets_pcie5.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_pcie5.c"

./reg_xref.pl $OUTPUT_DIR/pcie_regs.h al_pcie_rev4_regs pcie6 > $OUTPUT_DIR/reg_print_offsets_pcie6.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_pcie6.c"

./reg_xref.pl $OUTPUT_DIR/pcie_regs.h al_pcie_rev4_regs pcie7 > $OUTPUT_DIR/reg_print_offsets_pcie7.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_pcie7.c"

tmp_output=$OUTPUT_DIR/reg_print_offsets_srds_cmplx_main.c
./reg_xref.pl drivers/serdes/al_hal_serdes_avg_complex_regs.h al_srd_complex_regs srds_cmplx_main > ${tmp_output}
mv ${tmp_output} ${tmp_output}.bk; grep -v "\->srd_wrapper" ${tmp_output}.bk > ${tmp_output}
mv ${tmp_output} ${tmp_output}.bk; grep -v "\->sbm_wrapper" ${tmp_output}.bk > ${tmp_output}
export SOURCES="$SOURCES ${tmp_output}"

./reg_xref.pl drivers/pbs/al_hal_sbm_regs.h al_sbm_regs srds_cmplx_main_sbus_master > $OUTPUT_DIR/reg_print_offsets_srds_cmplx_main_sbus_master.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_srds_cmplx_main_sbus_master.c"

./reg_xref.pl drivers/serdes/al_hal_serdes_avg_lane_regs.h al_srd_1lane_regs srds_cmplx_main_lane0 > $OUTPUT_DIR/reg_print_offsets_srds_cmplx_main_lane0.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_srds_cmplx_main_lane0.c"

./reg_xref.pl drivers/serdes/al_hal_serdes_avg_lane_regs.h al_srd_1lane_regs srds_cmplx_main_lane1 > $OUTPUT_DIR/reg_print_offsets_srds_cmplx_main_lane1.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_srds_cmplx_main_lane1.c"

./reg_xref.pl drivers/serdes/al_hal_serdes_avg_lane_regs.h al_srd_1lane_regs srds_cmplx_main_lane2 > $OUTPUT_DIR/reg_print_offsets_srds_cmplx_main_lane2.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_srds_cmplx_main_lane2.c"

./reg_xref.pl drivers/serdes/al_hal_serdes_avg_lane_regs.h al_srd_1lane_regs srds_cmplx_main_lane3 > $OUTPUT_DIR/reg_print_offsets_srds_cmplx_main_lane3.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_srds_cmplx_main_lane3.c"

./reg_xref.pl drivers/serdes/al_hal_serdes_avg_lane_regs.h al_srd_1lane_regs srds_cmplx_main_lane4 > $OUTPUT_DIR/reg_print_offsets_srds_cmplx_main_lane4.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_srds_cmplx_main_lane4.c"

./reg_xref.pl drivers/serdes/al_hal_serdes_avg_lane_regs.h al_srd_1lane_regs srds_cmplx_main_lane5 > $OUTPUT_DIR/reg_print_offsets_srds_cmplx_main_lane5.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_srds_cmplx_main_lane5.c"

./reg_xref.pl drivers/serdes/al_hal_serdes_avg_lane_regs.h al_srd_1lane_regs srds_cmplx_main_lane6 > $OUTPUT_DIR/reg_print_offsets_srds_cmplx_main_lane6.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_srds_cmplx_main_lane6.c"

./reg_xref.pl drivers/serdes/al_hal_serdes_avg_lane_regs.h al_srd_1lane_regs srds_cmplx_main_lane7 > $OUTPUT_DIR/reg_print_offsets_srds_cmplx_main_lane7.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_srds_cmplx_main_lane7.c"

./reg_xref.pl drivers/serdes/al_hal_serdes_avg_lane_regs.h al_srd_1lane_regs srds_cmplx_main_lane8 > $OUTPUT_DIR/reg_print_offsets_srds_cmplx_main_lane8.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_srds_cmplx_main_lane8.c"

./reg_xref.pl drivers/serdes/al_hal_serdes_avg_lane_regs.h al_srd_1lane_regs srds_cmplx_main_lane9 > $OUTPUT_DIR/reg_print_offsets_srds_cmplx_main_lane9.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_srds_cmplx_main_lane9.c"

./reg_xref.pl drivers/serdes/al_hal_serdes_avg_lane_regs.h al_srd_1lane_regs srds_cmplx_main_lane10 > $OUTPUT_DIR/reg_print_offsets_srds_cmplx_main_lane10.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_srds_cmplx_main_lane10.c"

./reg_xref.pl drivers/serdes/al_hal_serdes_avg_lane_regs.h al_srd_1lane_regs srds_cmplx_main_lane11 > $OUTPUT_DIR/reg_print_offsets_srds_cmplx_main_lane11.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_srds_cmplx_main_lane11.c"

./reg_xref.pl drivers/serdes/al_hal_serdes_avg_lane_regs.h al_srd_1lane_regs srds_cmplx_main_lane12 > $OUTPUT_DIR/reg_print_offsets_srds_cmplx_main_lane12.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_srds_cmplx_main_lane12.c"

./reg_xref.pl drivers/serdes/al_hal_serdes_avg_lane_regs.h al_srd_1lane_regs srds_cmplx_main_lane13 > $OUTPUT_DIR/reg_print_offsets_srds_cmplx_main_lane13.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_srds_cmplx_main_lane13.c"

./reg_xref.pl drivers/serdes/al_hal_serdes_avg_lane_regs.h al_srd_1lane_regs srds_cmplx_main_lane14 > $OUTPUT_DIR/reg_print_offsets_srds_cmplx_main_lane14.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_srds_cmplx_main_lane14.c"

./reg_xref.pl drivers/serdes/al_hal_serdes_avg_lane_regs.h al_srd_1lane_regs srds_cmplx_main_lane15 > $OUTPUT_DIR/reg_print_offsets_srds_cmplx_main_lane15.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_srds_cmplx_main_lane15.c"

tmp_output=$OUTPUT_DIR/reg_print_offsets_srds_cmplx_sec.c
./reg_xref.pl drivers/serdes/al_hal_serdes_avg_complex_regs.h al_srd_complex_regs srds_cmplx_sec > ${tmp_output}
mv ${tmp_output} ${tmp_output}.bk; grep -v "\->srd_wrapper" ${tmp_output}.bk > ${tmp_output}
mv ${tmp_output} ${tmp_output}.bk; grep -v "\->sbm_wrapper" ${tmp_output}.bk > ${tmp_output}
export SOURCES="$SOURCES ${tmp_output}"

./reg_xref.pl drivers/pbs/al_hal_sbm_regs.h al_sbm_regs srds_cmplx_sec_sbus_master > $OUTPUT_DIR/reg_print_offsets_srds_cmplx_sec_sbus_master.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_srds_cmplx_sec_sbus_master.c"

./reg_xref.pl drivers/serdes/al_hal_serdes_avg_lane_regs.h al_srd_1lane_regs srds_cmplx_sec_lane0 > $OUTPUT_DIR/reg_print_offsets_srds_cmplx_sec_lane0.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_srds_cmplx_sec_lane0.c"

./reg_xref.pl drivers/serdes/al_hal_serdes_avg_lane_regs.h al_srd_1lane_regs srds_cmplx_sec_lane1 > $OUTPUT_DIR/reg_print_offsets_srds_cmplx_sec_lane1.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_srds_cmplx_sec_lane1.c"

./reg_xref.pl drivers/serdes/al_hal_serdes_avg_lane_regs.h al_srd_1lane_regs srds_cmplx_sec_lane2 > $OUTPUT_DIR/reg_print_offsets_srds_cmplx_sec_lane2.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_srds_cmplx_sec_lane2.c"

./reg_xref.pl drivers/serdes/al_hal_serdes_avg_lane_regs.h al_srd_1lane_regs srds_cmplx_sec_lane3 > $OUTPUT_DIR/reg_print_offsets_srds_cmplx_sec_lane3.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_srds_cmplx_sec_lane3.c"

./reg_xref.pl drivers/serdes/al_hal_serdes_avg_lane_regs.h al_srd_1lane_regs srds_cmplx_sec_lane4 > $OUTPUT_DIR/reg_print_offsets_srds_cmplx_sec_lane4.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_srds_cmplx_sec_lane4.c"

./reg_xref.pl drivers/serdes/al_hal_serdes_avg_lane_regs.h al_srd_1lane_regs srds_cmplx_sec_lane5 > $OUTPUT_DIR/reg_print_offsets_srds_cmplx_sec_lane5.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_srds_cmplx_sec_lane5.c"

./reg_xref.pl drivers/serdes/al_hal_serdes_avg_lane_regs.h al_srd_1lane_regs srds_cmplx_sec_lane6 > $OUTPUT_DIR/reg_print_offsets_srds_cmplx_sec_lane6.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_srds_cmplx_sec_lane6.c"

./reg_xref.pl drivers/serdes/al_hal_serdes_avg_lane_regs.h al_srd_1lane_regs srds_cmplx_sec_lane7 > $OUTPUT_DIR/reg_print_offsets_srds_cmplx_sec_lane7.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_srds_cmplx_sec_lane7.c"

./reg_xref.pl drivers/serdes/al_hal_serdes_avg_lane_regs.h al_srd_1lane_regs srds_cmplx_sec_lane8 > $OUTPUT_DIR/reg_print_offsets_srds_cmplx_sec_lane8.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_srds_cmplx_sec_lane8.c"

./reg_xref.pl drivers/serdes/al_hal_serdes_avg_lane_regs.h al_srd_1lane_regs srds_cmplx_sec_lane9 > $OUTPUT_DIR/reg_print_offsets_srds_cmplx_sec_lane9.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_srds_cmplx_sec_lane9.c"

./reg_xref.pl drivers/serdes/al_hal_serdes_avg_lane_regs.h al_srd_1lane_regs srds_cmplx_sec_lane10 > $OUTPUT_DIR/reg_print_offsets_srds_cmplx_sec_lane10.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_srds_cmplx_sec_lane10.c"

./reg_xref.pl drivers/serdes/al_hal_serdes_avg_lane_regs.h al_srd_1lane_regs srds_cmplx_sec_lane11 > $OUTPUT_DIR/reg_print_offsets_srds_cmplx_sec_lane11.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_srds_cmplx_sec_lane11.c"

./reg_xref.pl drivers/serdes/al_hal_serdes_avg_lane_regs.h al_srd_1lane_regs srds_cmplx_sec_lane12 > $OUTPUT_DIR/reg_print_offsets_srds_cmplx_sec_lane12.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_srds_cmplx_sec_lane12.c"

./reg_xref.pl drivers/serdes/al_hal_serdes_avg_lane_regs.h al_srd_1lane_regs srds_cmplx_sec_lane13 > $OUTPUT_DIR/reg_print_offsets_srds_cmplx_sec_lane13.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_srds_cmplx_sec_lane13.c"

./reg_xref.pl drivers/serdes/al_hal_serdes_avg_lane_regs.h al_srd_1lane_regs srds_cmplx_sec_lane14 > $OUTPUT_DIR/reg_print_offsets_srds_cmplx_sec_lane14.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_srds_cmplx_sec_lane14.c"

./reg_xref.pl drivers/serdes/al_hal_serdes_avg_lane_regs.h al_srd_1lane_regs srds_cmplx_sec_lane15 > $OUTPUT_DIR/reg_print_offsets_srds_cmplx_sec_lane15.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_srds_cmplx_sec_lane15.c"

tmp_output=$OUTPUT_DIR/reg_print_offsets_srds_cmplx_hs.c
./reg_xref.pl drivers/serdes/al_hal_serdes_avg_complex_regs.h al_srd_complex_regs srds_cmplx_hs > ${tmp_output}
mv ${tmp_output} ${tmp_output}.bk; grep -v "\->srd_wrapper" ${tmp_output}.bk > ${tmp_output}
mv ${tmp_output} ${tmp_output}.bk; grep -v "\->sbm_wrapper" ${tmp_output}.bk > ${tmp_output}
export SOURCES="$SOURCES ${tmp_output}"

./reg_xref.pl drivers/pbs/al_hal_sbm_regs.h al_sbm_regs srds_cmplx_hs_sbus_master > $OUTPUT_DIR/reg_print_offsets_srds_cmplx_hs_sbus_master.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_srds_cmplx_hs_sbus_master.c"

./reg_xref.pl drivers/serdes/al_hal_serdes_avg_lane_regs.h al_srd_1lane_regs srds_cmplx_hs_lane0 > $OUTPUT_DIR/reg_print_offsets_srds_cmplx_hs_lane0.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_srds_cmplx_hs_lane0.c"

./reg_xref.pl drivers/serdes/al_hal_serdes_avg_lane_regs.h al_srd_1lane_regs srds_cmplx_hs_lane1 > $OUTPUT_DIR/reg_print_offsets_srds_cmplx_hs_lane1.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_srds_cmplx_hs_lane1.c"

./reg_xref.pl drivers/serdes/al_hal_serdes_avg_lane_regs.h al_srd_1lane_regs srds_cmplx_hs_lane2 > $OUTPUT_DIR/reg_print_offsets_srds_cmplx_hs_lane2.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_srds_cmplx_hs_lane2.c"

./reg_xref.pl drivers/serdes/al_hal_serdes_avg_lane_regs.h al_srd_1lane_regs srds_cmplx_hs_lane3 > $OUTPUT_DIR/reg_print_offsets_srds_cmplx_hs_lane3.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_srds_cmplx_hs_lane3.c"

cp reg_xref_main_alpine_v3.c $OUTPUT_DIR/
gcc -o $OUTPUT_DIR/reg_print $OUTPUT_DIR/reg_xref_main_alpine_v3.c $SOURCES $INCLUDES -Wno-implicit-function-declaration
$OUTPUT_DIR/reg_print | sort -k 3 > reg_xref_alpine_v3.txt
rm -rf $OUTPUT_DIR/

