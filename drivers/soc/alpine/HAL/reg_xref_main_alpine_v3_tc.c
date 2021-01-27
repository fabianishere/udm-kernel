#include "al_hal_plat_types.h"
#include "al_hal_iomap.h"

int main()
{
	reg_print_offsets_pbs((void *)AL_PBS_REGFILE_BASE);
	reg_print_offsets_i2c_pld((void *)AL_I2C_PLD_BASE);
	reg_print_offsets_i2c_gen((void *)AL_I2C_GEN_BASE);
	reg_print_offsets_nand_ctrl((void *)AL_PBS_UFC_CNTL_BASE);
	reg_print_offsets_nand_wrapper((void *)AL_PBS_UFC_WRAP_BASE);
	reg_print_offsets_spi_slave((void *)AL_SPI_SLAVE_BASE);
	reg_print_offsets_spi_master((void *)AL_SPI_MASTER_BASE);
	reg_print_offsets_uart_0((void *)AL_UART_BASE(0));
	reg_print_offsets_uart_1((void *)AL_UART_BASE(1));
	reg_print_offsets_uart_2((void *)AL_UART_BASE(2));
	reg_print_offsets_uart_3((void *)AL_UART_BASE(3));
	reg_print_offsets_gpio_7_0((void *)AL_GPIO_BASE(0));
	reg_print_offsets_gpio_15_8((void *)AL_GPIO_BASE(1));
	reg_print_offsets_gpio_23_16((void *)AL_GPIO_BASE(2));
	reg_print_offsets_gpio_31_24((void *)AL_GPIO_BASE(3));
	reg_print_offsets_gpio_39_32((void *)AL_GPIO_BASE(4));
	reg_print_offsets_gpio_47_40((void *)AL_GPIO_BASE(5));
	reg_print_offsets_watchdog_0((void *)AL_WD_BASE(0));
	reg_print_offsets_watchdog_1((void *)AL_WD_BASE(1));
	reg_print_offsets_watchdog_2((void *)AL_WD_BASE(2));
	reg_print_offsets_watchdog_3((void *)AL_WD_BASE(3));
	reg_print_offsets_timer_0_0((void *)AL_TIMER_BASE(0, 0));
	reg_print_offsets_timer_0_1((void *)AL_TIMER_BASE(0, 1));
	reg_print_offsets_timer_1_0((void *)AL_TIMER_BASE(1, 0));
	reg_print_offsets_timer_1_1((void *)AL_TIMER_BASE(1, 1));
	reg_print_offsets_timer_2_0((void *)AL_TIMER_BASE(2, 0));
	reg_print_offsets_timer_2_1((void *)AL_TIMER_BASE(2, 1));
	reg_print_offsets_timer_3_0((void *)AL_TIMER_BASE(3, 0));
	reg_print_offsets_timer_3_1((void *)AL_TIMER_BASE(3, 1));
	reg_print_offsets_cmos((void *)AL_CMOS_GROUP_BASE(0));
	reg_print_offsets_pll_sb((void *)AL_PLL_BASE(AL_PLL_SB));
	reg_print_offsets_pll_nb((void *)AL_PLL_BASE(AL_PLL_NB));
	reg_print_offsets_pll_cpu((void *)AL_PLL_BASE(AL_PLL_CPU));
	reg_print_offsets_nb_service((void *)AL_NB_SERVICE_BASE);
	reg_print_offsets_nb_anpa_0((void *)AL_NB_ANPA_BASE(0));
	reg_print_offsets_ddr_ctrl((void *)AL_NB_DDR_CTL_BASE);
	reg_print_offsets_ddr_phy((void *)AL_NB_DDR_PHY_BASE);
	reg_print_offsets_pcie4((void *)AL_SB_PCIE_BASE(4));
	reg_print_offsets_srds_cmplx_main((void *)AL_SB_SRD_CMPLX_MAIN_BASE);
	reg_print_offsets_srds_cmplx_main_sbus_master((void *)AL_SRD_CMPLX_MAIN_SBUS_MASTER_BASE);
	reg_print_offsets_srds_cmplx_main_lane0((void *)AL_SRD_CMPLX_MAIN_LANE_BASE(0));
	reg_print_offsets_srds_cmplx_main_lane1((void *)AL_SRD_CMPLX_MAIN_LANE_BASE(1));
	reg_print_offsets_srds_cmplx_main_lane2((void *)AL_SRD_CMPLX_MAIN_LANE_BASE(2));

	return 0;
}

