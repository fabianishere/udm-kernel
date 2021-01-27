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
export DEFINES="$DEFINES -DAL_DEV_REV_ID=0"

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

./reg_xref.pl include/sys_fabric/al_hal_nb_regs_v3_tc.h al_nb_regs nb_service > $OUTPUT_DIR/reg_print_offsets_nb_service.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_nb_service.c"

./reg_xref.pl include/sys_fabric/al_hal_anpa_regs.h al_anpa_regs nb_anpa_0 > $OUTPUT_DIR/reg_print_offsets_nb_anpa_0.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_nb_anpa_0.c"

cpp drivers/ddr/al_hal_ddr_ctrl_regs_alpine_v2.h $INCLUDES | sed "s/^#.*//g" > $OUTPUT_DIR/ddr_ctrl_regs.h
./reg_xref.pl $OUTPUT_DIR/ddr_ctrl_regs.h al_ddr_ctrl_regs_alpine_v2 ddr_ctrl > $OUTPUT_DIR/reg_print_offsets_ddr_ctrl.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_ddr_ctrl.c"

cpp drivers/ddr/al_hal_ddr_phy_regs_alpine_v3.h $INCLUDES | sed "s/^#.*//g" > $OUTPUT_DIR/ddr_phy_regs.h
./reg_xref.pl $OUTPUT_DIR/ddr_phy_regs.h al_dwc_ddrphy_top_regs_alpine_v3 ddr_phy > $OUTPUT_DIR/reg_print_offsets_ddr_phy.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_ddr_phy.c"

echo "#define __iomem" > $OUTPUT_DIR/pcie_regs.h
cpp drivers/pcie/al_hal_pcie_regs.h $INCLUDES | sed "s/^#.*//g" >> $OUTPUT_DIR/pcie_regs.h

./reg_xref.pl $OUTPUT_DIR/pcie_regs.h al_pcie_rev4_regs pcie4 > $OUTPUT_DIR/reg_print_offsets_pcie4.c
export SOURCES="$SOURCES $OUTPUT_DIR/reg_print_offsets_pcie4.c"

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

cp reg_xref_main_alpine_v3_tc.c $OUTPUT_DIR/
gcc -o $OUTPUT_DIR/reg_print $OUTPUT_DIR/reg_xref_main_alpine_v3_tc.c $SOURCES $INCLUDES $DEFINES -Wno-implicit-function-declaration
$OUTPUT_DIR/reg_print | sort -k 3 > reg_xref_alpine_v3_tc.txt
rm -rf $OUTPUT_DIR/

