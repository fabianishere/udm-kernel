/*
 * Copyright 2016, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#include <al_hal_pbs_utils.h>
#include <al_hal_reg_utils.h>

/**************************************************************************************************/
/**************************************************************************************************/
unsigned int al_pbs_dev_id_get(
	void __iomem *pbs_regs)
{
	struct al_pbs_regs __iomem *regs = (struct al_pbs_regs __iomem *)pbs_regs;

	return (al_reg_read32(&regs->unit.chip_id) & PBS_UNIT_CHIP_ID_DEV_ID_MASK) >>
		PBS_UNIT_CHIP_ID_DEV_ID_SHIFT;
}

/**************************************************************************************************/
/**************************************************************************************************/
unsigned int al_pbs_dev_rev_id_get(
	void __iomem *pbs_regs)
{
	struct al_pbs_regs __iomem *regs = (struct al_pbs_regs __iomem *)pbs_regs;

	return (al_reg_read32(&regs->unit.chip_id) & PBS_UNIT_CHIP_ID_DEV_REV_ID_MASK) >>
		PBS_UNIT_CHIP_ID_DEV_REV_ID_SHIFT;
}

/**************************************************************************************************/
/**************************************************************************************************/
void al_pbs_axi_timeout_set(
	void __iomem	*pbs_regs,
	unsigned int	num_cyc,
	unsigned int	num_cyc_spi)
{
	struct al_pbs_regs __iomem *regs = (struct al_pbs_regs __iomem *)pbs_regs;
	unsigned int dev_id = al_pbs_dev_id_get(pbs_regs);
	uint16_t num_cyc_hi = num_cyc >> 16;
	uint16_t num_cyc_low = num_cyc & 0xffff;
	uint16_t num_cyc_spi_hi = num_cyc_spi >> 24;
	uint16_t num_cyc_spi_med = (num_cyc_spi >> 16) & 0xff;
	uint16_t num_cyc_spi_low = num_cyc_spi & 0xffff;

	if (dev_id == PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V1) {
		al_reg_write32(&regs->unit.cfg_axi_conf_3, num_cyc);
		return;
	}

	al_assert(num_cyc_spi_low == num_cyc_low);
	al_assert((num_cyc_hi & (PBS_UNIT_CFG_AXI_CONF_3_TIMEOUT_HI_MASK >>
			PBS_UNIT_CFG_AXI_CONF_3_TIMEOUT_HI_SHIFT)) == num_cyc_hi);
	al_assert((num_cyc_low & (PBS_UNIT_CFG_AXI_CONF_3_TIMEOUT_LOW_MASK >>
			PBS_UNIT_CFG_AXI_CONF_3_TIMEOUT_LOW_SHIFT)) == num_cyc_low);
	al_assert((num_cyc_spi_hi & (PBS_UNIT_CFG_AXI_CONF_3_TIMEOUT_SPI_HI_MASK >>
			PBS_UNIT_CFG_AXI_CONF_3_TIMEOUT_SPI_HI_SHIFT)) == num_cyc_spi_hi);
	al_assert(num_cyc_spi_med == 0);

	al_reg_write32_masked(
		&regs->unit.cfg_axi_conf_3,
		PBS_UNIT_CFG_AXI_CONF_3_TIMEOUT_LOW_MASK |
		PBS_UNIT_CFG_AXI_CONF_3_TIMEOUT_HI_MASK |
		PBS_UNIT_CFG_AXI_CONF_3_TIMEOUT_SPI_HI_MASK,
		(num_cyc_low << PBS_UNIT_CFG_AXI_CONF_3_TIMEOUT_LOW_SHIFT) |
		(num_cyc_hi << PBS_UNIT_CFG_AXI_CONF_3_TIMEOUT_HI_SHIFT) |
		(num_cyc_spi_hi << PBS_UNIT_CFG_AXI_CONF_3_TIMEOUT_SPI_HI_SHIFT));
}

/**************************************************************************************************/
/**************************************************************************************************/
void al_pbs_winit_access_en(
	void __iomem	*pbs_regs,
	al_bool		en)
{
	struct al_pbs_regs __iomem *regs = (struct al_pbs_regs __iomem *)pbs_regs;
	unsigned int dev_id = al_pbs_dev_id_get(pbs_regs);

	al_reg_write32_masked(
		(dev_id <= PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V2) ?
		&regs->unit.winit_cntl_v2 :
		&regs->bar_config.winit_cntl,
		PBS_UNIT_WINIT_CNTL_ENABLE_WINIT_REGS_ACCESS,
		en ? PBS_UNIT_WINIT_CNTL_ENABLE_WINIT_REGS_ACCESS : 0);
}

/**************************************************************************************************/
/**************************************************************************************************/
void al_pbs_apb_mem_config_set(
	void __iomem			*pbs_regs,
	struct al_pbs_apb_mem_config	*cfg)
{
	struct al_pbs_regs __iomem *regs = (struct al_pbs_regs __iomem *)pbs_regs;

	al_assert(pbs_regs);
	al_assert(cfg);

	al_reg_write32_masked(
		&regs->unit.apb_mem_conf_int,
		PBS_UNIT_APB_MEM_CONF_INT_CFG_BOOTROM_PARITY_EN |
		PBS_UNIT_APB_MEM_CONF_INT_CFG_RD_SLV_ERR_EN |
		PBS_UNIT_APB_MEM_CONF_INT_CFG_RD_DEC_ERR_EN |
		PBS_UNIT_APB_MEM_CONF_INT_CFG_WR_SLV_ERR_EN |
		PBS_UNIT_APB_MEM_CONF_INT_CFG_WR_DEC_ERR_EN,
		(cfg->bootrom_parity_en ? PBS_UNIT_APB_MEM_CONF_INT_CFG_BOOTROM_PARITY_EN : 0) |
		(cfg->rd_slv_err_en ? PBS_UNIT_APB_MEM_CONF_INT_CFG_RD_SLV_ERR_EN : 0) |
		(cfg->rd_dec_err_en ? PBS_UNIT_APB_MEM_CONF_INT_CFG_RD_DEC_ERR_EN : 0) |
		(cfg->wr_slv_err_en ? PBS_UNIT_APB_MEM_CONF_INT_CFG_WR_SLV_ERR_EN : 0) |
		(cfg->wr_dec_err_en ? PBS_UNIT_APB_MEM_CONF_INT_CFG_WR_DEC_ERR_EN : 0));
}

