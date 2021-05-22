#include "al_dram_margins.h"
#include "al_hal_iomap.h"
#include "al_hal_unit_adapter_regs.h"

#if (AL_DEV_ID == AL_DEV_ID_ALPINE_V1)
#include "al_hal_ddr_phy_regs_alpine_v1.h"
#elif (AL_DEV_ID == AL_DEV_ID_ALPINE_V2)
#include "al_hal_ddr_phy_regs_alpine_v2.h"
#include "al_dram_margins_agent_arr_alpine_v2.h"
#elif ((AL_DEV_ID == AL_DEV_ID_ALPINE_V3) && (AL_DEV_REV_ID == 0))
#include "al_hal_ddr_phy_regs_alpine_v2.h"
#include "al_dram_margins_agent_arr_alpine_v3.h"
#elif (AL_DEV_ID >= AL_DEV_ID_ALPINE_V3)
#include "al_hal_ddr_phy_regs_alpine_v3.h"
#include "al_dram_margins_agent_arr_alpine_v3.h"
#endif

#if (AL_DEV_ID == AL_DEV_ID_ALPINE_V1)
#define DELAY_START		(-38)
#define DELAY_END		8
#define DELAY_SKIP		1
#elif (AL_DEV_ID == AL_DEV_ID_ALPINE_V2)
#define DELAY_START		(-95)
#define DELAY_END		31
#define DELAY_SKIP		1
#elif (AL_DEV_ID >= AL_DEV_ID_ALPINE_V3)
#define DELAY_START		(-127)
#define DELAY_END		31
#define DELAY_SKIP		1
#endif

#define DELAYS_NUM		((DELAY_END - DELAY_START + 1) / DELAY_SKIP)

#define MAX_WC			0xFFFC

#define WRITE_BDLR		2
#define READ_BDLR		5

#if (AL_DEV_ID == AL_DEV_ID_ALPINE_V1)
#define STRUCT_PHY_REGS struct al_dwc_ddrphy_top_dwc_ddrphy_pub_alpine_v1
#define PHY_REGS(ddr_phy_regs_base)	\
	(&((struct al_dwc_ddrphy_top_regs_alpine_v1 __iomem *)ddr_phy_regs_base)->dwc_ddrphy_pub)
#define BDLR_REG(phy_regs, lane, i)								\
	(&(phy_regs->datx8[lane].BDLR[i]))
#elif ((AL_DEV_ID == AL_DEV_ID_ALPINE_V2) || \
	((AL_DEV_ID == AL_DEV_ID_ALPINE_V3) && (AL_DEV_REV_ID == 0)))
#define STRUCT_PHY_REGS struct al_dwc_ddrphy_top_dwc_ddrphy_pub_alpine_v2
#define PHY_REGS(ddr_phy_regs_base)	\
	(&((struct al_dwc_ddrphy_top_regs_alpine_v2 __iomem *)ddr_phy_regs_base)->dwc_ddrphy_pub)
#define BDLR_REG(phy_regs, lane, i)								\
	((((i) < 3) ? &(phy_regs)->datx8[lane].bdlr0_2[i] :		\
	(((i) < 6) ? &(phy_regs)->datx8[lane].bdlr3_5[(i) - 3] :	\
	&(phy_regs)->datx8[lane].bdlr6_9[(i) - 6])))
#elif (AL_DEV_ID >= AL_DEV_ID_ALPINE_V3)
#define STRUCT_PHY_REGS struct al_dwc_ddrphy_top_dwc_ddrphy_pub_alpine_v3
#define PHY_REGS(ddr_phy_regs_base)	\
	(&((struct al_dwc_ddrphy_top_regs_alpine_v3 __iomem *)ddr_phy_regs_base)->dwc_ddrphy_pub)
#define BDLR_REG(phy_regs, lane, i)								\
	((((i) < 3) ? &(phy_regs)->datx8[lane].bdlr0_2[i] :		\
	(((i) < 6) ? &(phy_regs)->datx8[lane].bdlr3_5[(i) - 3] :	\
	&(phy_regs)->datx8[lane].bdlr6_9[(i) - 6])))
#endif

#define BDLR_REGS		6

#define TIMEOUT_LIMIT		10

#define READ			0
#define WRITE			1

#if (AL_DEV_ID == AL_DEV_ID_ALPINE_V2)
#define ETH0_ADAPTER_BASE	AL_ETH_BASE(0)
#define ETH0_BIG_SRAM_EN_REG	(AL_ETH_BASE(0) + 0x0000d030)
#define ETH0_BIG_SRAM_EN_MASK	0x3
#define ETH0_BIG_SRAM_EN_VAL	0x3
#define AGENT_SRAM_BASE		AL_ADV_ETH_SRAM_BASE
#endif

#if (AL_DEV_ID > AL_DEV_ID_ALPINE_V2)
#define AGENT_SRAM_BASE		SRAM_AGENT_ADDRESS
#endif

#if (AL_DEV_ID >= AL_DEV_ID_ALPINE_V2)
#define AGENT_WORK_AREA_PTR	(void *)(uintptr_t)(AGENT_SRAM_BASE + 0x00)
#define AGENT_RESULTS_PTR	(void *)(uintptr_t)(AGENT_SRAM_BASE + 0x04)

#define AGENT_FLAGS		(void *)(uintptr_t)(AGENT_SRAM_BASE + 0x08)
#define AGENT_FLAGS_FIRST	AL_BIT(0)
#define AGENT_FLAGS_LAST	AL_BIT(1)
#define AGENT_FLAGS_PRINT	AL_BIT(2)
#define AGENT_FLAGS_PRINT_CSV	AL_BIT(3)
#define AGENT_FLAGS_PAT_W0	AL_BIT(4)
#define AGENT_FLAGS_PAT_W1	AL_BIT(5)
#define AGENT_FLAGS_PAT_LFSR	AL_BIT(6)
#define AGENT_FLAGS_PAT_USER	AL_BIT(7)

#define AGENT_USER_PATTERN	(void *)(uintptr_t)(AGENT_SRAM_BASE + 0x0c)
#define AGENT_ITER_PER_SAMPLE	(void *)(uintptr_t)(AGENT_SRAM_BASE + 0x10)
#define AGENT_DRAM_CH		(void *)(uintptr_t)(AGENT_SRAM_BASE + 0x14)
#define AGENT_ENTRY_POINT	(void *)(uintptr_t)(AGENT_SRAM_BASE + 0x20)
#endif

struct test_params {
	al_bool				pattern_walk_0;
	al_bool				pattern_walk_1;
	al_bool				pattern_lfsr;
	al_bool				pattern_user;
	unsigned int			iterations_per_sample;
	struct al_ddr_bist_params	*bist_params;
};

static struct al_dram_margins_result	margins_result_temp;

/**************************************************************************************************/
/**************************************************************************************************/
static void update_dq_current(
	struct al_ddr_cfg			*ddr_cfg,
	unsigned int				lane,
	struct al_dram_margins_result_per_lane	*result)
{
	STRUCT_PHY_REGS __iomem *phy_regs =
		(STRUCT_PHY_REGS __iomem *)PHY_REGS(ddr_cfg->ddr_phy_regs_base);
	int i;
	uint32_t reg_bdlr0 = al_reg_read32(BDLR_REG(phy_regs, lane, 0));
	uint32_t reg_bdlr1 = al_reg_read32(BDLR_REG(phy_regs, lane, 1));
	uint32_t reg_bdlr3 = al_reg_read32(BDLR_REG(phy_regs, lane, 3));
	uint32_t reg_bdlr4 = al_reg_read32(BDLR_REG(phy_regs, lane, 4));

	/* get current delay */
#if (AL_DEV_ID == AL_DEV_ID_ALPINE_V1)
	uint32_t reg = al_reg_read32(&phy_regs->datx8[lane].LCDLR[1]);
	int curr_dqs_read = ((reg & DWC_DDR_PHY_REGS_DXNLCDLR1_RDQSD_MASK) >>
			DWC_DDR_PHY_REGS_DXNLCDLR1_RDQSD_SHIFT);
	int curr_dqs_write = ((reg & DWC_DDR_PHY_REGS_DXNLCDLR1_WDQD_MASK) >>
			DWC_DDR_PHY_REGS_DXNLCDLR1_WDQD_SHIFT);
#elif ((AL_DEV_ID == AL_DEV_ID_ALPINE_V2) || \
	((AL_DEV_ID == AL_DEV_ID_ALPINE_V3) && (AL_DEV_REV_ID == 0)))
	uint32_t lcdlr1 = al_reg_read32(&phy_regs->datx8[lane].lcdlr[1]);
	uint32_t lcdlr3 = al_reg_read32(&phy_regs->datx8[lane].lcdlr[3]);
	int curr_dqs_read = ((lcdlr3 & ALPINE_V2_DWC_DDR_PHY_REGS_DXNLCDLR3_RDQSD_MASK) >>
		ALPINE_V2_DWC_DDR_PHY_REGS_DXNLCDLR3_RDQSD_SHIFT);
	int curr_dqs_write = ((lcdlr1 & ALPINE_V2_DWC_DDR_PHY_REGS_DXNLCDLR1_WDQD_MASK) >>
		ALPINE_V2_DWC_DDR_PHY_REGS_DXNLCDLR1_WDQD_SHIFT);
#elif (AL_DEV_ID >= AL_DEV_ID_ALPINE_V3)
	uint32_t lcdlr1 = al_reg_read32(&phy_regs->datx8[lane].lcdlr[1]);
	uint32_t lcdlr3 = al_reg_read32(&phy_regs->datx8[lane].lcdlr[3]);
	int curr_dqs_read = ((lcdlr3 & ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR3_RDQSD_MASK) >>
		ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR3_RDQSD_SHIFT);
	int curr_dqs_write = ((lcdlr1 & ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR1_WDQD_MASK) >>
		ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR1_WDQD_SHIFT);
#endif

	for (i = 0; i < AL_DDR_BIST_DQ_BITS; i++) {
#if (AL_DEV_ID == AL_DEV_ID_ALPINE_V1)
		int mask = 0x1F;
#elif (AL_DEV_ID >= AL_DEV_ID_ALPINE_V2)
		int mask = 0x3F;
#endif
		int shift = 8 * (i%4);

		int dq_write = ((((i < 4) ? reg_bdlr0 : reg_bdlr1) >> shift) & mask);
		int dq_read = ((((i < 4) ? reg_bdlr3 : reg_bdlr4) >> shift) & mask);

		result->bits[i].rd_dqs = curr_dqs_read;
		result->bits[i].rd_dq = dq_read;
		result->bits[i].rd_min = DELAY_START;
		result->bits[i].rd_curr = dq_read - curr_dqs_read;
		result->bits[i].rd_max = DELAY_END;
		result->bits[i].wr_dqs = curr_dqs_write;
		result->bits[i].wr_dq = dq_write;
		result->bits[i].wr_min = DELAY_START;
		result->bits[i].wr_curr = -dq_write - curr_dqs_write;
		result->bits[i].wr_max = DELAY_END;
	}
}

/**************************************************************************************************/
/**************************************************************************************************/
static void run_bist(
	struct al_ddr_cfg	*ddr_cfg,
	struct test_params	*test_params,
	uint8_t			*dq_error)
{
	struct al_ddr_bist_params *bist_params = test_params->bist_params;
	enum al_ddr_bist_pat		pattern;
	struct al_ddr_bist_err_status	bist_err_status;
	int i;
	int to_cnt = 0;
	unsigned int iter;
	int err = 0;

	for (i = 0; i < AL_DDR_BIST_DQ_BITS; i++)
		dq_error[i] = 0;

	for (iter = 0; (!err) && (iter < test_params->iterations_per_sample); iter++) {
		for (pattern = AL_DDR_BIST_PATTERN_WALK_0;
			(!err) && (pattern <= AL_DDR_BIST_PATTERN_USER); pattern++) {
			if ((pattern == AL_DDR_BIST_PATTERN_WALK_0) &&
					(!test_params->pattern_walk_0))
				continue;
			if ((pattern == AL_DDR_BIST_PATTERN_WALK_1) &&
					(!test_params->pattern_walk_1))
				continue;
			if ((pattern == AL_DDR_BIST_PATTERN_LFSR) &&
					(!test_params->pattern_lfsr))
				continue;
			if ((pattern == AL_DDR_BIST_PATTERN_USER) &&
					(!test_params->pattern_user))
				continue;

			bist_params->pat = pattern;
again:
			if (al_ddr_phy_datx_bist(
					ddr_cfg,
					bist_params,
					&bist_err_status)) {
				if ((bist_err_status.timeout == AL_TRUE) &&
						(to_cnt < TIMEOUT_LIMIT)) {
					to_cnt++;
					al_udelay(1000);
					goto again;
				}
				to_cnt = 0;

				err = 0;
				for (i = 0; i < AL_DDR_BIST_DQ_BITS; i++) {
					if (bist_err_status.timeout == AL_TRUE) {
						dq_error[i] += TIMEOUT_LIMIT;
						err = -ETIME;
					} else if (
						bist_err_status.even_fall_err[i] != 0 ||
						bist_err_status.even_risg_err[i] != 0 ||
						bist_err_status.odd_fall_err[i]  != 0 ||
						bist_err_status.odd_risg_err[i]  != 0) {
							dq_error[i]++;
							err = -EIO;
					}
				}
			}
		}
	}
}

/**************************************************************************************************/
/**************************************************************************************************/
static void set_delay(
	struct al_ddr_cfg	*ddr_cfg,
	int pval,
	int test,
	int lane)
{
	STRUCT_PHY_REGS __iomem *phy_regs =
		(STRUCT_PHY_REGS __iomem *)PHY_REGS(ddr_cfg->ddr_phy_regs_base);
	int bdlr_idx = (test == READ) ? READ_BDLR : WRITE_BDLR;

	if (pval <= 0) {
		al_local_data_memory_barrier();
		if (test == READ) {
#if (AL_DEV_ID == AL_DEV_ID_ALPINE_V1)
			al_reg_write32_masked(
				&phy_regs->datx8[lane].LCDLR[1],
				DWC_DDR_PHY_REGS_DXNLCDLR1_RDQSD_MASK,
				(-pval) << DWC_DDR_PHY_REGS_DXNLCDLR1_RDQSD_SHIFT);
			al_reg_read32(&phy_regs->datx8[lane].LCDLR[1]);
#elif ((AL_DEV_ID == AL_DEV_ID_ALPINE_V2) || \
	((AL_DEV_ID == AL_DEV_ID_ALPINE_V3) && (AL_DEV_REV_ID == 0)))
			al_reg_write32_masked(
				&phy_regs->datx8[lane].lcdlr[3],
				ALPINE_V2_DWC_DDR_PHY_REGS_DXNLCDLR3_RDQSD_MASK,
				(-pval) << ALPINE_V2_DWC_DDR_PHY_REGS_DXNLCDLR3_RDQSD_SHIFT);
			al_reg_read32(&phy_regs->datx8[lane].lcdlr[3]);
#elif (AL_DEV_ID >= AL_DEV_ID_ALPINE_V3)
			al_reg_write32_masked(
				&phy_regs->datx8[lane].lcdlr[3],
				ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR3_RDQSD_MASK |
				ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR3_X4RDQSD_MASK,
				((-pval) << ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR3_RDQSD_SHIFT) |
				((-pval) << ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR3_X4RDQSD_SHIFT));
			al_reg_read32(&phy_regs->datx8[lane].lcdlr[3]);
#endif
		} else {
#if (AL_DEV_ID == AL_DEV_ID_ALPINE_V1)
			al_reg_write32_masked(
				&phy_regs->datx8[lane].LCDLR[1],
				DWC_DDR_PHY_REGS_DXNLCDLR1_WDQD_MASK,
				(-pval) << DWC_DDR_PHY_REGS_DXNLCDLR1_WDQD_SHIFT);
			al_reg_read32(&phy_regs->datx8[lane].LCDLR[1]);
#elif ((AL_DEV_ID == AL_DEV_ID_ALPINE_V2) || \
	((AL_DEV_ID == AL_DEV_ID_ALPINE_V3) && (AL_DEV_REV_ID == 0)))
			al_reg_write32_masked(
				&phy_regs->datx8[lane].lcdlr[1],
				ALPINE_V2_DWC_DDR_PHY_REGS_DXNLCDLR1_WDQD_MASK,
				(-pval) << ALPINE_V2_DWC_DDR_PHY_REGS_DXNLCDLR1_WDQD_SHIFT);
			al_reg_read32(&phy_regs->datx8[lane].lcdlr[1]);
#elif (AL_DEV_ID >= AL_DEV_ID_ALPINE_V3)
			al_reg_write32_masked(
				&phy_regs->datx8[lane].lcdlr[1],
				ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR1_WDQD_MASK |
				ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR1_X4WDQD_MASK,
				((-pval) << ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR1_WDQD_SHIFT) |
				((-pval) << ALPINE_V3_DWC_DDR_PHY_REGS_DXNLCDLR1_X4WDQD_SHIFT));
			al_reg_read32(&phy_regs->datx8[lane].lcdlr[1]);
#endif
		}
		al_local_data_memory_barrier();
	} else {
		al_local_data_memory_barrier();
		if (test == WRITE) {
#if (AL_DEV_ID == AL_DEV_ID_ALPINE_V1)
			uint32_t dqs_mask =
				DWC_DDR_PHY_REGS_DXNBDLR2_DMWBD_MASK |
				DWC_DDR_PHY_REGS_DXNBDLR2_DSWBD_MASK |
				DWC_DDR_PHY_REGS_DXNBDLR2_OEBD_MASK;
			uint32_t reg_val =
				(pval << DWC_DDR_PHY_REGS_DXNBDLR2_DMWBD_SHIFT) |
				(pval << DWC_DDR_PHY_REGS_DXNBDLR2_DSWBD_SHIFT) |
				(pval << DWC_DDR_PHY_REGS_DXNBDLR2_OEBD_SHIFT);
#elif ((AL_DEV_ID == AL_DEV_ID_ALPINE_V2) || \
	((AL_DEV_ID == AL_DEV_ID_ALPINE_V3) && (AL_DEV_REV_ID == 0)))
			uint32_t dqs_mask =
				ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR2_DMWBD_MASK |
				ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR2_DSWBD_MASK |
				ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR2_DSOEBD_MASK;
			uint32_t reg_val =
				(pval << ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR2_DMWBD_SHIFT) |
				(pval << ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR2_DSWBD_SHIFT) |
				(pval << ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR2_DSOEBD_SHIFT);
#elif (AL_DEV_ID >= AL_DEV_ID_ALPINE_V3)
			uint32_t dqs_mask =
				ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR2_DMWBD_MASK |
				ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR2_DSWBD_MASK |
				ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR2_OEBD_MASK;
			uint32_t reg_val =
				(pval << ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR2_DMWBD_SHIFT) |
				(pval << ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR2_DSWBD_SHIFT) |
				(pval << ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR2_OEBD_SHIFT);
#endif
			al_reg_write32_masked(
				BDLR_REG(phy_regs, lane, bdlr_idx),
				dqs_mask,
				reg_val);
		} else {
			int i;
			for (i = 0; i < AL_DDR_BIST_DQ_BITS; i++) {
#if (AL_DEV_ID == AL_DEV_ID_ALPINE_V1)
				/* dq 0 - 3 */
				uint32_t dq_mask1 =
					DWC_DDR_PHY_REGS_DXNBDLR3_DQ0RBD_MASK |
					DWC_DDR_PHY_REGS_DXNBDLR3_DQ1RBD_MASK |
					DWC_DDR_PHY_REGS_DXNBDLR3_DQ2RBD_MASK |
					DWC_DDR_PHY_REGS_DXNBDLR3_DQ3RBD_MASK;

				uint32_t reg_val1 =
					(pval << DWC_DDR_PHY_REGS_DXNBDLR3_DQ0RBD_SHIFT) |
					(pval << DWC_DDR_PHY_REGS_DXNBDLR3_DQ1RBD_SHIFT) |
					(pval << DWC_DDR_PHY_REGS_DXNBDLR3_DQ2RBD_SHIFT) |
					(pval << DWC_DDR_PHY_REGS_DXNBDLR3_DQ3RBD_SHIFT);

				/* dq 4 - 7 */
				uint32_t dq_mask2 =
					DWC_DDR_PHY_REGS_DXNBDLR4_DQ4RBD_MASK |
					DWC_DDR_PHY_REGS_DXNBDLR4_DQ5RBD_MASK |
					DWC_DDR_PHY_REGS_DXNBDLR4_DQ6RBD_MASK |
					DWC_DDR_PHY_REGS_DXNBDLR4_DQ7RBD_MASK;

				uint32_t reg_val2 =
					(pval << DWC_DDR_PHY_REGS_DXNBDLR4_DQ4RBD_SHIFT) |
					(pval << DWC_DDR_PHY_REGS_DXNBDLR4_DQ5RBD_SHIFT) |
					(pval << DWC_DDR_PHY_REGS_DXNBDLR4_DQ6RBD_SHIFT) |
					(pval << DWC_DDR_PHY_REGS_DXNBDLR4_DQ7RBD_SHIFT);
#elif ((AL_DEV_ID == AL_DEV_ID_ALPINE_V2) || \
	((AL_DEV_ID == AL_DEV_ID_ALPINE_V3) && (AL_DEV_REV_ID == 0)))
				/* dq 0 - 3 */
				uint32_t dq_mask1 =
					ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR3_DQ0RBD_MASK |
					ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR3_DQ1RBD_MASK |
					ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR3_DQ2RBD_MASK |
					ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR3_DQ3RBD_MASK;

				uint32_t reg_val1 =
					(pval << ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR3_DQ0RBD_SHIFT) |
					(pval << ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR3_DQ1RBD_SHIFT) |
					(pval << ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR3_DQ2RBD_SHIFT) |
					(pval << ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR3_DQ3RBD_SHIFT);

				/* dq 4 - 7 */
				uint32_t dq_mask2 =
					ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR4_DQ4RBD_MASK |
					ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR4_DQ5RBD_MASK |
					ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR4_DQ6RBD_MASK |
					ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR4_DQ7RBD_MASK;

				uint32_t reg_val2 =
					(pval << ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR4_DQ4RBD_SHIFT) |
					(pval << ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR4_DQ5RBD_SHIFT) |
					(pval << ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR4_DQ6RBD_SHIFT) |
					(pval << ALPINE_V2_DWC_DDR_PHY_REGS_DXNBDLR4_DQ7RBD_SHIFT);
#elif (AL_DEV_ID >= AL_DEV_ID_ALPINE_V3)
				/* dq 0 - 3 */
				uint32_t dq_mask1 =
					ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR3_DQ0RBD_MASK |
					ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR3_DQ1RBD_MASK |
					ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR3_DQ2RBD_MASK |
					ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR3_DQ3RBD_MASK;

				uint32_t reg_val1 =
					(pval << ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR3_DQ0RBD_SHIFT) |
					(pval << ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR3_DQ1RBD_SHIFT) |
					(pval << ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR3_DQ2RBD_SHIFT) |
					(pval << ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR3_DQ3RBD_SHIFT);

				/* dq 4 - 7 */
				uint32_t dq_mask2 =
					ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR4_DQ4RBD_MASK |
					ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR4_DQ5RBD_MASK |
					ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR4_DQ6RBD_MASK |
					ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR4_DQ7RBD_MASK;

				uint32_t reg_val2 =
					(pval << ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR4_DQ4RBD_SHIFT) |
					(pval << ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR4_DQ5RBD_SHIFT) |
					(pval << ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR4_DQ6RBD_SHIFT) |
					(pval << ALPINE_V3_DWC_DDR_PHY_REGS_DXNBDLR4_DQ7RBD_SHIFT);
#endif

				al_reg_write32_masked(
					BDLR_REG(phy_regs, lane, 3),
					dq_mask1,
					reg_val1);
				al_reg_write32_masked(
					BDLR_REG(phy_regs, lane, 4),
					dq_mask2,
					reg_val2);
			}
		}
		al_local_data_memory_barrier();
	}
}

/**************************************************************************************************/
/**************************************************************************************************/
static void find_lane_limits(
	struct al_ddr_cfg			*ddr_cfg,
	struct test_params			*test_params,
	int					active_lane,
	struct al_dram_margins_result_per_lane	*result)
{
	STRUCT_PHY_REGS __iomem *phy_regs =
		(STRUCT_PHY_REGS __iomem *)PHY_REGS(ddr_cfg->ddr_phy_regs_base);
	uint32_t			lcdlr1_orig;
#if (AL_DEV_ID >= AL_DEV_ID_ALPINE_V2)
	uint32_t			lcdlr3_orig;
#endif
	uint32_t			bdlr_orig[BDLR_REGS];

	int8_t pval;
	int i;
	int test_iter;

	struct al_ddr_bist_params *bist_params = test_params->bist_params;

	/* set lane to test */
	bist_params->active_byte_lanes[active_lane] = 1;

	/* save original values*/
#if (AL_DEV_ID == AL_DEV_ID_ALPINE_V1)
	lcdlr1_orig = al_reg_read32(
		&phy_regs->datx8[active_lane].LCDLR[1]);
#elif (AL_DEV_ID >= AL_DEV_ID_ALPINE_V2)
	lcdlr1_orig = al_reg_read32(
		&phy_regs->datx8[active_lane].lcdlr[1]);
	lcdlr3_orig = al_reg_read32(
		&phy_regs->datx8[active_lane].lcdlr[3]);
#endif
	for (i = 0; i < BDLR_REGS; i++)
		bdlr_orig[i] = al_reg_read32(BDLR_REG(phy_regs, active_lane, i));

	for (test_iter = READ; test_iter <= WRITE; test_iter++) {
		/* reset DQ delays to 0 */
		for (i = 0; i < BDLR_REGS; i++)
			al_reg_write32(BDLR_REG(phy_regs, active_lane, i), 0);

		/* Upon read test, restore original write settings */
		if (test_iter == READ)
			for (i = 0; i <= 2; i++)
				al_reg_write32(
					BDLR_REG(phy_regs, active_lane, i),
					bdlr_orig[i]);
		/* Upon write test, restore original read settings */
		else if (test_iter == WRITE)
			for (i = 3; i <= 5; i++)
				al_reg_write32(
					BDLR_REG(phy_regs, active_lane, i),
					bdlr_orig[i]);

		for (pval = DELAY_START; pval <= DELAY_END; pval++) {
			uint8_t dq_error[AL_DDR_BIST_DQ_BITS];

			set_delay(ddr_cfg, pval, test_iter, active_lane);
			run_bist(ddr_cfg, test_params, dq_error);

			for (i = 0; i < AL_DDR_BIST_DQ_BITS; i++) {
				int curr = (test_iter == READ) ?
					result->bits[i].rd_curr :
					result->bits[i].wr_curr;

				if (pval < curr) {
					int8_t *val_to_set =
						(test_iter == READ) ?
						&result->bits[i].rd_min :
						&result->bits[i].wr_min;

					if (dq_error[i])
						*val_to_set = pval;
				} else {
					int8_t *val_to_set =
						(test_iter == READ) ?
						&result->bits[i].rd_max :
						&result->bits[i].wr_max;

					if (dq_error[i] && ((*val_to_set) > pval))
						*val_to_set = pval;
				}
			}
		}

		/* Restore settings */
		al_local_data_memory_barrier();
#if (AL_DEV_ID == AL_DEV_ID_ALPINE_V1)
		al_reg_write32(&phy_regs->datx8[active_lane].LCDLR[1],
			lcdlr1_orig);
		al_reg_read32(&phy_regs->datx8[active_lane].LCDLR[1]);
#elif (AL_DEV_ID >= AL_DEV_ID_ALPINE_V2)
		al_reg_write32(&phy_regs->datx8[active_lane].lcdlr[1],
			lcdlr1_orig);
		al_reg_read32(&phy_regs->datx8[active_lane].lcdlr[1]);
		al_reg_write32(&phy_regs->datx8[active_lane].lcdlr[3],
			lcdlr3_orig);
		al_reg_read32(&phy_regs->datx8[active_lane].lcdlr[3]);
#endif
		al_local_data_memory_barrier();
	}

	/* Restore original settings */
	for (i = 0; i < BDLR_REGS; i++)
		al_reg_write32(
			BDLR_REG(phy_regs, active_lane, i),
			bdlr_orig[i]);

	/* clear lane from test */
	bist_params->active_byte_lanes[active_lane] = 0;
}

/**************************************************************************************************/
/**************************************************************************************************/
static void al_dram_margins_result_print_lane(
	struct al_dram_margins_result_per_lane	*result,
	unsigned int				centi_taps_per_row)
{
	int i;
	int j;

	for (i = 0; i < AL_DDR_BIST_DQ_BITS; i++) {
		int mid_printed;

		al_print(" %u|", i);

		for (mid_printed = 0, j = (100 * DELAY_START); j <= (100 * DELAY_END);
				j += centi_taps_per_row) {
			int d = (j / 100) + result->bits[i].rd_dq;

			if (d > DELAY_END)
				al_print(".");
			else if ((d < result->bits[i].rd_min) || (d > result->bits[i].rd_max))
				al_print("*");
			else if ((d >= result->bits[i].rd_curr) && (!mid_printed)) {
				al_print("+");
				mid_printed = 1;
			} else
				al_print(" ");
		}

		al_print(" ");

		for (mid_printed = 0, j = (100 * DELAY_START); j <= (100 * DELAY_END);
				j += centi_taps_per_row) {
			int d = (j / 100) - result->bits[i].wr_dq;

			if (d < DELAY_START)
				al_print(".");
			else if ((d < result->bits[i].wr_min) || (d > result->bits[i].wr_max))
				al_print("*");
			else if ((d >= result->bits[i].wr_curr) && (!mid_printed)) {
				al_print("+");
				mid_printed = 1;
			} else
				al_print(" ");
		}

		al_print("| %2d %2d %2d | %2d %2d %2d |\n",
			result->bits[i].rd_curr - result->bits[i].rd_min,
			-result->bits[i].rd_curr,
			result->bits[i].rd_max - result->bits[i].rd_curr,
			result->bits[i].wr_curr - result->bits[i].wr_min,
			-result->bits[i].wr_curr,
			result->bits[i].wr_max - result->bits[i].wr_curr);
	}
}
/**************************************************************************************************/
/**************************************************************************************************/
static void al_dram_margins_result_print_lane_csv(
	struct al_dram_margins_result_per_lane	*result,
	unsigned int				rank,
	unsigned int				lane)
{
	int i;

	for (i = 0; i < AL_DDR_BIST_DQ_BITS; i++) {
		al_print("%4d, %5d, %6d, %6d, %6d, %5d, %6d, %6d, %6d, %6d, %5d, %6d\n",
			rank, lane,
			result->bits[i].rd_min,
			result->bits[i].rd_curr,
			result->bits[i].rd_dqs,
			result->bits[i].rd_dq,
			result->bits[i].rd_max,
			result->bits[i].wr_min,
			result->bits[i].wr_curr,
			result->bits[i].wr_dqs,
			result->bits[i].wr_dq,
			result->bits[i].wr_max);
	}
}

/**************************************************************************************************/
/**************************************************************************************************/
static void _al_dram_margins(
	unsigned int			dram_ch,
	al_phys_addr_t			work_area,
	al_bool				first,
	al_bool				last,
	al_bool				pattern_walk_0,
	al_bool				pattern_walk_1,
	al_bool				pattern_lfsr,
	al_bool				pattern_user,
	uint16_t			pattern_user_even,
	uint16_t			pattern_user_odd,
	unsigned int			iterations_per_sample,
	struct al_dram_margins_result	*margins_result_final)
{
	int err;
	struct al_ddr_cfg ddr_cfg;
	struct al_ddr_bist_params bist_params;
	struct al_dram_margins_result *margins_result = &margins_result_temp;
	struct test_params test_params = {
			.pattern_walk_0 = pattern_walk_0,
			.pattern_walk_1 = pattern_walk_1,
			.pattern_lfsr = pattern_lfsr,
			.pattern_user = pattern_user,
			.iterations_per_sample = iterations_per_sample,
			.bist_params = &bist_params,
		};
	int active_byte_lanes[AL_DDR_PHY_NUM_BYTE_LANES];
	unsigned int rank_mask;
	unsigned int num_columns;

	int i;
	int lane;
	unsigned int rank;
	unsigned int bank;
	unsigned int bg;
	unsigned int col;
	unsigned int row;

#if (AL_DEV_ID <= AL_DEV_ID_ALPINE_V2)
	al_assert(dram_ch == 0);
	al_ddr_cfg_init(
		(void __iomem *)AL_NB_SERVICE_BASE,
		(void __iomem *)AL_NB_DDR_CTL_BASE,
		(void __iomem *)AL_NB_DDR_PHY_BASE,
		&ddr_cfg);
#else
	al_ddr_cfg_init_v3(
		(void __iomem *)AL_NB_SERVICE_BASE,
		(void __iomem *)AL_DDR_CORE_BASE(dram_ch),
		(void __iomem *)AL_DDR_PHY_BASE(dram_ch),
		(void __iomem *)AL_DDRC_BASE(dram_ch),
		dram_ch,
		&ddr_cfg);
#endif

	err = al_ddr_address_translate_sys2dram(&ddr_cfg, work_area, &rank, &bank, &bg, &col, &row);
	if (err)
		return;

	al_ddr_active_byte_lanes_get(&ddr_cfg, active_byte_lanes);
	num_columns = al_ddr_active_columns_get(&ddr_cfg);

	al_memset(&bist_params, 0, sizeof(struct al_ddr_bist_params));
	bist_params.mode = AL_DDR_BIST_MODE_DRAM;
	bist_params.inc  = 8;	/* 8 is the smallest increment */
	bist_params.col_min  = 0;
	bist_params.col_max  = num_columns - bist_params.inc;
	bist_params.row_min  = row;
	bist_params.row_max  = row;
	bist_params.bank_min = bank;
	bist_params.bank_max = bank;

	bist_params.wc = 2 * (bist_params.col_max / bist_params.inc);
	if (bist_params.wc > MAX_WC)
		bist_params.wc = MAX_WC;

	/* clear active lane */
	for (i = 0; i < AL_DDR_PHY_NUM_BYTE_LANES; i++)
		bist_params.active_byte_lanes[i] = 0;

	/* set user pattern */
	bist_params.user_pat_even = pattern_user_even;
	bist_params.user_pat_odd  = pattern_user_odd;

	/* disable in order to play with params, enable again in the end */
	al_ddr_phy_datx_bist_pre_adv(&ddr_cfg, first);

	rank_mask = al_ddr_active_ranks_mask_get(&ddr_cfg);
	margins_result->rank_mask = rank_mask;
	al_memcpy(margins_result->active_byte_lanes, active_byte_lanes,
		sizeof(margins_result->active_byte_lanes));

	for (rank = 0; rank_mask; rank++, rank_mask >>= 1) {
		unsigned int logical_rank = al_ddr_logical_rank_from_phys(&ddr_cfg, rank);
#if (AL_DEV_ID >= AL_DEV_ID_ALPINE_V2)
		STRUCT_PHY_REGS __iomem *phy_regs =
			(STRUCT_PHY_REGS __iomem *)PHY_REGS(ddr_cfg.ddr_phy_regs_base);
#endif

		if (!(rank_mask & AL_BIT(0)))
			continue;

		bist_params.rank_min = logical_rank;
		bist_params.rank_max = logical_rank;

#if ((AL_DEV_ID == AL_DEV_ID_ALPINE_V2) || \
	((AL_DEV_ID == AL_DEV_ID_ALPINE_V3) && (AL_DEV_REV_ID == 0)))
		/* Select rank */
		al_reg_write32(
			&phy_regs->rankidr,
			(rank << ALPINE_V2_DWC_DDR_PHY_REGS_RANKIDR_RANKWID_SHIFT) |
			(rank << ALPINE_V2_DWC_DDR_PHY_REGS_RANKIDR_RANKRID_SHIFT));
#elif (AL_DEV_ID >= AL_DEV_ID_ALPINE_V3)
		/* Select rank */
		al_reg_write32(
			&phy_regs->rankidr,
			(rank << ALPINE_V3_DWC_DDR_PHY_REGS_RANKIDR_RANKWID_SHIFT) |
			(rank << ALPINE_V3_DWC_DDR_PHY_REGS_RANKIDR_RANKRID_SHIFT));
#endif

		for (lane = 0; lane < AL_DDR_PHY_NUM_BYTE_LANES; lane++) {
			if (!active_byte_lanes[lane])
				continue;

			update_dq_current(
				&ddr_cfg,
				lane,
				&margins_result->ranks[rank].lanes[lane]);

			find_lane_limits(
				&ddr_cfg,
				&test_params,
				lane,
				&margins_result->ranks[rank].lanes[lane]);
		}
	}

	if (last) {
		struct al_ddr_ecc_cfg ecc_cfg;

		al_ddr_ecc_cfg_get(&ddr_cfg, &ecc_cfg);

		rank_mask = al_ddr_active_ranks_mask_get(&ddr_cfg);
		for (rank = 0; (ecc_cfg.ecc_enabled) && (rank_mask); rank++, rank_mask >>= 1) {
			unsigned int logical_rank = al_ddr_logical_rank_from_phys(&ddr_cfg, rank);
			struct al_ddr_bist_err_status bist_err_status;

			if (!(rank_mask & AL_BIT(0)))
				continue;

			bist_params.rank_min = logical_rank;
			bist_params.rank_max = logical_rank;
			bist_params.pat = AL_DDR_BIST_PATTERN_USER;
#if (AL_DEV_ID <= AL_DEV_ID_ALPINE_V2)
			bist_params.user_pat_even = 0xaaaa;
			bist_params.user_pat_odd  = 0x5555;
#else
			bist_params.user_pat_even = 0x0000;
			bist_params.user_pat_odd  = 0xffff;
#endif
			al_ddr_active_byte_lanes_get(&ddr_cfg, bist_params.active_byte_lanes);
			bist_params.all_lanes_active = AL_TRUE;
			al_ddr_phy_datx_bist(&ddr_cfg, &bist_params, &bist_err_status);
		}
	}

	al_ddr_phy_datx_bist_post_adv(&ddr_cfg, last);
	al_local_data_memory_barrier();

	if (margins_result_final)
		al_memcpy(margins_result_final, margins_result,
			sizeof(struct al_dram_margins_result));
}

/**************************************************************************************************/
/**************************************************************************************************/
void al_dram_margins_ex(
	unsigned int				dram_ch,
	const struct al_dram_margins_params	*params)
{
	al_assert(params);

	_al_dram_margins(
		dram_ch,
		params->work_area,
		params->first,
		params->last,
		params->pattern_walk_0,
		params->pattern_walk_1,
		params->pattern_lfsr,
		params->pattern_user,
		params->pattern_user_even,
		params->pattern_user_odd,
		params->iterations_per_sample,
		params->margins_result);
}

/**************************************************************************************************/
/**************************************************************************************************/
void al_dram_margins(
	al_phys_addr_t			work_area,
	al_bool				first,
	al_bool				last,
	al_bool				pattern_walk_0,
	al_bool				pattern_walk_1,
	al_bool				pattern_lfsr,
	al_bool				pattern_user,
	uint16_t			pattern_user_even,
	uint16_t			pattern_user_odd,
	unsigned int			iterations_per_sample,
	struct al_dram_margins_result	*margins_result_final)
{
	_al_dram_margins(
		0,
		work_area,
		first,
		last,
		pattern_walk_0,
		pattern_walk_1,
		pattern_lfsr,
		pattern_user,
		pattern_user_even,
		pattern_user_odd,
		iterations_per_sample,
		margins_result_final);
}

/**************************************************************************************************/
/**************************************************************************************************/
void al_dram_margins_result_print(
	struct al_dram_margins_result	*result,
	unsigned int			width)
{
	int lane;
	unsigned int rank;
	unsigned int rank_mask = result->rank_mask;
	unsigned int i;
	unsigned int centi_taps_per_row =
		(width < 40) ? (100 * width) : (100 * DELAYS_NUM) / ((width - 27) / 2);

	for (rank = 0; rank_mask; rank++, rank_mask >>= 1) {
		int j;
		unsigned int net_width;

		if (!(rank_mask & AL_BIT(0)))
			continue;

		al_print("Rank #%d:\n", rank);

		for (lane = 0; lane < AL_DDR_PHY_NUM_BYTE_LANES; lane++) {
#if (AL_DEV_ID == AL_DEV_ID_ALPINE_V1)
			int lane_idx_ecc = AL_DDR_PHY_ECC_BYTE_LANE_INDEX_ALPINE_V1;
#elif ((AL_DEV_ID == AL_DEV_ID_ALPINE_V2) || \
	((AL_DEV_ID == AL_DEV_ID_ALPINE_V3) && (AL_DEV_REV_ID == 0)))
			int lane_idx_ecc = AL_DDR_PHY_ECC_BYTE_LANE_INDEX_ALPINE_V2;
#elif (AL_DEV_ID >= AL_DEV_ID_ALPINE_V3)
			int lane_idx_ecc = AL_DDR_PHY_ECC_BYTE_LANE_INDEX_ALPINE_V3;
#endif

			if (!result->active_byte_lanes[lane])
				continue;

			al_print("%s #%u:", (lane == lane_idx_ecc) ? "ECC  " : "OCTET", lane);
			for (net_width = 0, j = (100 * DELAY_START); j <= (100 * DELAY_END);
				j += centi_taps_per_row, net_width++)
					;
			for (i = 0; i <= ((2 * net_width) - 4); i++)
				al_print(" ");
			al_print("<< rd >> | << wr >>\n");
			al_dram_margins_result_print_lane(
				&result->ranks[rank].lanes[lane], centi_taps_per_row);
		}
	}
}

/**************************************************************************************************/
/**************************************************************************************************/
void al_dram_margins_result_print_csv(
	struct al_dram_margins_result	*result)
{
	int lane;
	unsigned int rank;
	unsigned int rank_mask = result->rank_mask;

	al_print("rank, octet, rd_min, rd_cur, rd_dqs, rd_dq, rd_max, wr_min, wr_cur, wr_dqs, "
		"wr_dq, wr_max\n");

	for (rank = 0; rank_mask; rank++, rank_mask >>= 1) {
		if (!(rank_mask & AL_BIT(0)))
			continue;

		for (lane = 0; lane < AL_DDR_PHY_NUM_BYTE_LANES; lane++) {
			if (!result->active_byte_lanes[lane])
				continue;

			al_dram_margins_result_print_lane_csv(
				&result->ranks[rank].lanes[lane], rank, lane);
		}
	}
}

#if (AL_DEV_ID >= AL_DEV_ID_ALPINE_V2)
/**************************************************************************************************/
/**************************************************************************************************/
int al_dram_margins_agent_init(void)
{
	int err;

#if (AL_DEV_ID == AL_DEV_ID_ALPINE_V2)
	/* Ethernet 0 powerup, if required */
	if (al_reg_read32((uint32_t *)(ETH0_ADAPTER_BASE + AL_ADAPTER_GENERIC_CONTROL_0)) &
		AL_ADAPTER_GENERIC_CONTROL_0_ADAPTER_DIS) {
		/* Disable clock gating */
		al_reg_write32_masked(
			(uint32_t *)(ETH0_ADAPTER_BASE + AL_ADAPTER_GENERIC_CONTROL_0),
			AL_ADAPTER_GENERIC_CONTROL_0_CLK_GATE_EN, 0);
		/* Switch to D0 */
		al_reg_write32_masked(
			(uint32_t *)(ETH0_ADAPTER_BASE + AL_ADAPTER_PM_1),
			AL_ADAPTER_PM_1_PWR_STATE_MASK, AL_ADAPTER_PM_1_PWR_STATE_D0);
		/* Unit specific power-up */
		al_reg_write32_masked(
			(uint32_t *)(ETH0_ADAPTER_BASE + AL_ADAPTER_GENERIC_CONTROL_3),
			0x8f000000, 0);
		/* Function level reset */
		al_reg_write32_masked(
			(uint32_t *)(ETH0_ADAPTER_BASE + AL_PCI_EXP_CAP_BASE + AL_PCI_EXP_DEVCTL),
			AL_PCI_EXP_DEVCTL_BCR_FLR, AL_PCI_EXP_DEVCTL_BCR_FLR);
		al_udelay(1000);
	}

	al_reg_write32_masked(
		(uint32_t *)ETH0_BIG_SRAM_EN_REG,
		ETH0_BIG_SRAM_EN_MASK,
		ETH0_BIG_SRAM_EN_VAL);
	al_reg_read32((uint32_t *)ETH0_BIG_SRAM_EN_REG);
#endif

	al_memcpy(
		AGENT_ENTRY_POINT,
		aarch64_dram_margins_agent_arr,
		sizeof(aarch64_dram_margins_agent_arr));

	err = al_memcmp(AGENT_ENTRY_POINT, aarch64_dram_margins_agent_arr,
			sizeof(aarch64_dram_margins_agent_arr));
	if (err)
		return -ENOMEM;

	return 0;
}

/**************************************************************************************************/
/**************************************************************************************************/
int al_dram_margins_agent_run(
	al_phys_addr_t			work_area,
	al_bool				first,
	al_bool				last,
	al_bool				pattern_walk_0,
	al_bool				pattern_walk_1,
	al_bool				pattern_lfsr,
	al_bool				pattern_user,
	uint16_t			pattern_user_even,
	uint16_t			pattern_user_odd,
	unsigned int			iterations_per_sample,
	struct al_dram_margins_result	*results)
{
	struct al_dram_margins_params params = {
		.work_area = work_area,
		.first = first,
		.last = last,
		.pattern_walk_0 = pattern_walk_0,
		.pattern_walk_1 = pattern_walk_1,
		.pattern_lfsr = pattern_lfsr,
		.pattern_user = pattern_user,
		.pattern_user_even = pattern_user_even,
		.pattern_user_odd = pattern_user_odd,
		.iterations_per_sample = iterations_per_sample,
		.margins_result = results
	};

	return al_dram_margins_agent_run_ex(0, &params);
}

/**************************************************************************************************/
/**************************************************************************************************/
int al_dram_margins_agent_run_ex(
	unsigned int				dram_ch,
	const struct al_dram_margins_params	*params)
{
	int (*agent_run)(void) = (int (*)(void))AGENT_ENTRY_POINT;
	int err;

	al_assert(params);

	*(uint32_t *)AGENT_WORK_AREA_PTR = (uint32_t)params->work_area;
	*(uint32_t *)AGENT_RESULTS_PTR = (uint32_t)(uintptr_t)params->margins_result;

	*(uint32_t *)AGENT_FLAGS =
		(params->first ? AGENT_FLAGS_FIRST : 0) |
		(params->last ? AGENT_FLAGS_LAST : 0) |
		(params->pattern_walk_0 ? AGENT_FLAGS_PAT_W0 : 0) |
		(params->pattern_walk_1 ? AGENT_FLAGS_PAT_W1 : 0) |
		(params->pattern_lfsr ? AGENT_FLAGS_PAT_LFSR : 0) |
		(params->pattern_user ? AGENT_FLAGS_PAT_USER : 0);

	*(uint32_t *)AGENT_USER_PATTERN =
		(((uint32_t)params->pattern_user_even) << 16) |
		((uint32_t)params->pattern_user_odd);

	*(uint32_t *)AGENT_ITER_PER_SAMPLE = params->iterations_per_sample;

	*(uint32_t *)AGENT_DRAM_CH = dram_ch;

	al_local_data_memory_barrier();
	err = agent_run();
	al_local_data_memory_barrier();

	return err;
}
#endif


