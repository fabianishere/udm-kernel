/*
 * Copyright 2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 */

#include <linux/clk.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/rawnand.h>
#include <linux/of_address.h>
#include <linux/platform_device.h>

#include "al_hal_nand.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Annapurna Labs");

#define WAIT_EMPTY_CMD_FIFO_TIME_OUT 1000000
#define AL_NAND_NAME "al-nand"
#define AL_NAND_MAX_ONFI_TIMING_MODE 1

#define AL_NAND_MAX_BIT_FLIPS 4
#define AL_NAND_MAX_OOB_SIZE SZ_1K

#define AL_NAND_ECC_SUPPORT

static const char * const part_probes[] = {
	"cmdlinepart", "RedBoot", "ofpart", NULL };

static struct mtd_partition nand_partitions[] = {
        {
                .name           = "main",
                .offset         = 0x00000000,
                .size           = MTDPART_SIZ_FULL,
        },
};

struct nand_data {
	struct nand_controller controller;
	struct nand_chip chip;
	struct device *dev;

	struct al_nand_ctrl_obj nand_obj;
	uint8_t word_cache[4];
	int cache_pos;
	struct al_nand_ecc_config ecc_config;

	/*** interrupts ***/
	struct completion complete;
	int irq;

	uint8_t oob[AL_NAND_MAX_OOB_SIZE];

	struct al_nand_extra_dev_properties dev_ext_props;
	struct al_nand_dev_properties device_properties;
};

/*
 * Addressing RMN: 2903
 *
 * RMN description:
 * NAND timing parameters that are used in the non-manual mode are wrong and
 * reduce performance.
 * Replacing with the manual parameters to increase speed
 */
static unsigned long clk_freq_khz = 375000;
#define NAND_CLK_CYCLES(nsec) ((nsec) * clk_freq_khz / 1000000)

static void al_nand_timing_params_set(struct al_nand_device_timing *timing, uint8_t mode)
{
	switch (mode) {
	case 0:
		timing->tSETUP = NAND_CLK_CYCLES(14);
		timing->tHOLD = NAND_CLK_CYCLES(22);
		timing->tWRP = NAND_CLK_CYCLES(54);
		timing->tRR = NAND_CLK_CYCLES(43);
		timing->tWB = NAND_CLK_CYCLES(206);
		timing->tWH = NAND_CLK_CYCLES(32);
		timing->tINTCMD = NAND_CLK_CYCLES(86);
		timing->readDelay = NAND_CLK_CYCLES(3);
		break;
	case 1:
		timing->tSETUP = NAND_CLK_CYCLES(14);
		timing->tHOLD = NAND_CLK_CYCLES(14);
		timing->tWRP = NAND_CLK_CYCLES(27);
		timing->tRR = NAND_CLK_CYCLES(22);
		timing->tWB = NAND_CLK_CYCLES(104);
		timing->tWH = NAND_CLK_CYCLES(19);
		timing->tINTCMD = NAND_CLK_CYCLES(43);
		timing->readDelay = NAND_CLK_CYCLES(3);
		break;
	case 2:
		timing->tSETUP = NAND_CLK_CYCLES(14);
		timing->tHOLD = NAND_CLK_CYCLES(14);
		timing->tWRP = NAND_CLK_CYCLES(19);
		timing->tRR = NAND_CLK_CYCLES(22);
		timing->tWB = NAND_CLK_CYCLES(104);
		timing->tWH = NAND_CLK_CYCLES(16);
		timing->tINTCMD = NAND_CLK_CYCLES(43);
		timing->readDelay = NAND_CLK_CYCLES(3);
		break;
	case 3:
		timing->tSETUP = NAND_CLK_CYCLES(14);
		timing->tHOLD = NAND_CLK_CYCLES(14);
		timing->tWRP = NAND_CLK_CYCLES(16);
		timing->tRR = NAND_CLK_CYCLES(22);
		timing->tWB = NAND_CLK_CYCLES(104);
		timing->tWH = NAND_CLK_CYCLES(11);
		timing->tINTCMD = NAND_CLK_CYCLES(27);
		timing->readDelay = NAND_CLK_CYCLES(3);
		break;
	case 4:
		timing->tSETUP = NAND_CLK_CYCLES(14);
		timing->tHOLD = NAND_CLK_CYCLES(14);
		timing->tWRP = NAND_CLK_CYCLES(14);
		timing->tRR = NAND_CLK_CYCLES(22);
		timing->tWB = NAND_CLK_CYCLES(104);
		timing->tWH = NAND_CLK_CYCLES(11);
		timing->tINTCMD = NAND_CLK_CYCLES(27);
		timing->readDelay = NAND_CLK_CYCLES(3);
		break;
	case 5:
		timing->tSETUP = NAND_CLK_CYCLES(14);
		timing->tHOLD = NAND_CLK_CYCLES(14);
		timing->tWRP = NAND_CLK_CYCLES(14);
		timing->tRR = NAND_CLK_CYCLES(22);
		timing->tWB = NAND_CLK_CYCLES(104);
		timing->tWH = NAND_CLK_CYCLES(11);
		timing->tINTCMD = NAND_CLK_CYCLES(27);
		timing->readDelay = NAND_CLK_CYCLES(3);
		break;
	default:
		pr_err("%s: Invalid timing mode %d", __func__, mode);
		BUG();
	}
}

static uint32_t wait_for_irq(struct nand_data *nand, uint32_t irq_mask);

static inline struct nand_data *nand_data_get(
				struct mtd_info *mtd)
{
	struct nand_chip *nand = mtd_to_nand(mtd);

	return nand_get_controller_data(nand);
}

static void nand_cw_size_get(
			int		num_bytes,
			uint32_t	*cw_size,
			uint32_t	*cw_count)
{
	num_bytes = AL_ALIGN_UP(num_bytes, 4);

	if (num_bytes < *cw_size)
		*cw_size = num_bytes;

	if (0 != (num_bytes % *cw_size))
		*cw_size = num_bytes / 4;

	BUG_ON(num_bytes % *cw_size);

	*cw_count = num_bytes / *cw_size;
}

static void nand_send_byte_count_command(
			struct al_nand_ctrl_obj		*nand_obj,
			enum al_nand_command_type	cmd_id,
			uint16_t			len)
{
	uint32_t cmd;

	cmd = AL_NAND_CMD_SEQ_ENTRY(
			cmd_id,
			(len & 0xff));

	al_nand_cmd_single_execute(nand_obj, cmd);

	cmd = AL_NAND_CMD_SEQ_ENTRY(
			cmd_id,
			((len & 0xff00) >> 8));

	al_nand_cmd_single_execute(nand_obj, cmd);
}

static void nand_wait_cmd_fifo_empty(
			struct nand_data	*nand)
{
	int cmd_buff_empty;
	uint32_t i = WAIT_EMPTY_CMD_FIFO_TIME_OUT;

	while (i > 0) {
		cmd_buff_empty = al_nand_cmd_buff_is_empty(&nand->nand_obj);
		if (cmd_buff_empty)
			break;

		udelay(1);
		i--;
	}

	if (i == 0)
		dev_err(nand->dev,
			"Wait for empty cmd fifo for more than a sec!\n");
}

void nand_cmd_ctrl(struct mtd_info *mtd, int dat, unsigned int ctrl)
{
	uint32_t cmd;
	enum al_nand_command_type type;
	struct nand_data *nand;

	if ((ctrl & (NAND_CLE | NAND_ALE)) == 0)
		return;

	nand = nand_data_get(mtd);
	nand->cache_pos = -1;

	type = ((ctrl & NAND_CTRL_CLE) == NAND_CTRL_CLE) ?
					AL_NAND_COMMAND_TYPE_CMD :
					AL_NAND_COMMAND_TYPE_ADDRESS;

	cmd = AL_NAND_CMD_SEQ_ENTRY(type, (dat & 0xff));
	dev_dbg(nand->dev, "dat=0x%x, ctrl=0x%x, cmd=0x%x\n", dat, ctrl, cmd);

	al_nand_cmd_single_execute(&nand->nand_obj, cmd);

	nand_wait_cmd_fifo_empty(nand);

	if ((dat == NAND_CMD_PAGEPROG) && (ctrl & NAND_CLE)) {
		cmd = AL_NAND_CMD_SEQ_ENTRY(
				AL_NAND_COMMAND_TYPE_WAIT_FOR_READY,
				0);

		dev_dbg(nand->dev, "pagepro. send cmd = 0x%x\n", cmd);
		al_nand_cmd_single_execute(&nand->nand_obj, cmd);

		nand_wait_cmd_fifo_empty(nand);

		al_nand_wp_set_enable(&nand->nand_obj, 1);
		al_nand_tx_set_enable(&nand->nand_obj, 0);
	}
}

void nand_dev_select(struct mtd_info *mtd, int chipnr)
{
	struct nand_data *nand;

	if (chipnr < 0)
		return;

	nand = nand_data_get(mtd);
	al_nand_dev_select(&nand->nand_obj, chipnr);

	dev_dbg(nand->dev, "chipnr = %d\n", chipnr);
}

int nand_dev_ready(struct mtd_info *mtd)
{
	int is_ready = 0;
	struct nand_data *nand;

	nand = nand_data_get(mtd);
	is_ready = al_nand_dev_is_ready(&nand->nand_obj);

	dev_dbg(nand->dev, "ready = %d\n", is_ready);

	return is_ready;
}

/*
 * read len bytes from the nand device.
 */
void nand_read_buff(struct mtd_info *mtd, uint8_t *buf, int len)
{
	uint32_t cw_size;
	uint32_t cw_count;
	struct nand_data *nand;
	uint32_t intr_status;
	void __iomem *data_buff;
	struct nand_chip *nand_chip = mtd_to_nand(mtd);

	nand = nand_data_get(mtd);

	dev_dbg(nand->dev, "read len = %d\n", len);

	cw_size = nand_chip->ecc.size;

	BUG_ON(len & 3);
	BUG_ON(nand->cache_pos != -1);

	nand_cw_size_get(len, &cw_size, &cw_count);

	al_nand_cw_config(
			&nand->nand_obj,
			cw_size,
			cw_count);

	while (cw_count--)
		nand_send_byte_count_command(&nand->nand_obj,
				AL_NAND_COMMAND_TYPE_DATA_READ_COUNT,
				cw_size);
	while (len > 0) {
		intr_status = wait_for_irq(nand, AL_NAND_INTR_STATUS_BUF_RDRDY);

		data_buff = al_nand_data_buff_base_get(&nand->nand_obj);
		memcpy(buf, data_buff, cw_size);
		buf += cw_size;
		len -= cw_size;
	}
}

/*
 * read byte from the device.
 * read byte is not supported by the controller so this function reads
 * 4 bytes as a cache and use it in the next calls.
 */
uint8_t nand_read_byte_from_fifo(struct mtd_info *mtd)
{
	uint8_t ret_val;
	struct nand_data *nand;

	nand = nand_data_get(mtd);

	dev_dbg(nand->dev, "cache_pos = %d", nand->cache_pos);

	if (nand->cache_pos == -1) {
		nand_read_buff(mtd, nand->word_cache, 4);
		nand->cache_pos = 0;
	}

	ret_val = nand->word_cache[nand->cache_pos];
	nand->cache_pos++;
	if (nand->cache_pos == 4)
		nand->cache_pos = -1;

	dev_dbg(nand->dev, "return = 0x%x\n", ret_val);
	return ret_val;
}

u16 nand_read_word(struct mtd_info *mtd)
{
	/* shouldn't be called */
	BUG();

	return 0;
}
/*
 * writing buffer to the nand device.
 * this func will wait for the write to be complete
 */
void nand_write_buff(struct mtd_info *mtd, const uint8_t *buf, int len)
{
	struct nand_chip *nand_chip = mtd_to_nand(mtd);

	uint32_t cw_size = nand_chip->ecc.size;
	uint32_t cw_count;
	struct nand_data *nand;
	void __iomem *data_buff;

	nand = nand_data_get(mtd);

	dev_dbg(nand->dev, "len = %d start: 0x%x%x%x\n", len, buf[0], buf[1],
		buf[2]);

	al_nand_tx_set_enable(&nand->nand_obj, 1);
	al_nand_wp_set_enable(&nand->nand_obj, 0);

	nand_cw_size_get(len, &cw_size, &cw_count);

	al_nand_cw_config(
			&nand->nand_obj,
			cw_size,
			cw_count);

	while (cw_count--)
		nand_send_byte_count_command(&nand->nand_obj,
				AL_NAND_COMMAND_TYPE_DATA_WRITE_COUNT,
				cw_size);


	while (len > 0) {
		wait_for_irq(nand, AL_NAND_INTR_STATUS_BUF_WRRDY);

		data_buff = al_nand_data_buff_base_get(&nand->nand_obj);
		memcpy(data_buff, buf, cw_size);

		buf += cw_size;
		len -= cw_size;
	}

	/* enable wp and disable tx will be executed after commands
	 * NAND_CMD_PAGEPROG and AL_NAND_COMMAND_TYPE_WAIT_FOR_READY will be
	 * sent to make sure all data were written.
	 */
}

/******************************************************************************/
/**************************** ecc functions ***********************************/
/******************************************************************************/
#ifdef AL_NAND_ECC_SUPPORT
static inline int is_empty_oob(uint8_t *oob, int len)
{
	int flips = 0;
	int i;
	int j;

	for (i = 0; i < len; i++) {
		if (oob[i] == 0xff)
			continue;

		for (j = 0; j < 8; j++) {
			if ((oob[i] & BIT(j)) == 0) {
				flips++;
				if (flips >= AL_NAND_MAX_BIT_FLIPS)
					break;
			}
		}
	}

	if (flips < AL_NAND_MAX_BIT_FLIPS)
		return 1;

	return 0;
}
/*
 * read page with HW ecc support (corrected and uncorrected stat will be
 * updated).
 */
int ecc_read_page(struct mtd_info *mtd, struct nand_chip *chip,
			uint8_t *buf, int oob_required, int page)
{
	struct mtd_oob_region oobregion;
	struct nand_data *nand;
	int uncorr_err_count = 0;
	int corr_err_count = 0;

	nand = nand_data_get(mtd);

	dev_dbg(nand->dev, "read page %d\n", page);

	/* Clear TX/RX ECC state machine */
	al_nand_tx_set_enable(&nand->nand_obj, 1);
	al_nand_tx_set_enable(&nand->nand_obj, 0);

	al_nand_uncorr_err_clear(&nand->nand_obj);
	al_nand_corr_err_clear(&nand->nand_obj);

	al_nand_ecc_set_enabled(&nand->nand_obj, 1);

	BUG_ON(oob_required);

	mtd_ooblayout_ecc(mtd, 0, &oobregion);

	/* First need to read the OOB to the controller to calc the ecc */
	chip->cmdfunc(mtd, NAND_CMD_READOOB,
			oobregion.offset, page);

	nand_send_byte_count_command(&nand->nand_obj,
				AL_NAND_COMMAND_TYPE_SPARE_READ_COUNT,
				oobregion.length);

	/* move to the start of the page to read the data */
	chip->cmdfunc(mtd, NAND_CMD_RNDOUT, 0x00, -1);

	/* read the buffer (after ecc correction) */
	nand_read_page_op(chip, page, 0, buf, mtd->writesize);

	uncorr_err_count = al_nand_uncorr_err_get(&nand->nand_obj);
	corr_err_count = al_nand_corr_err_get(&nand->nand_obj);

	al_nand_ecc_set_enabled(&nand->nand_obj, 0);

	/* update statistics*/
	if (0 != uncorr_err_count) {
		bool uncorr_err = true;
		if (nand->ecc_config.algorithm == AL_NAND_ECC_ALGORITHM_BCH) {
			/* the ECC in BCH algorithm will find an uncorrected
			 * errors while trying to read an empty page.
			 * to avoid error messages and failures in the upper
			 * layer, don't update the statistics in this case */
			chip->read_buf(mtd, nand->oob, mtd->oobsize);

			if (is_empty_oob(nand->oob, mtd->oobsize)) {
				uncorr_err = false;
				memset(buf, 0xff, mtd->writesize);
			}
		}

		if (uncorr_err) {
			mtd->ecc_stats.failed++;
			dev_err(nand->dev,
				"Uncorrected errors found in page %d! (increased to %d)\n",
				page, mtd->ecc_stats.failed);
		}
	}

	if (0 != corr_err_count) {
		mtd->ecc_stats.corrected++;
		dev_dbg(nand->dev, "corrected increased to %d\n",
			mtd->ecc_stats.corrected);
	}

	dev_dbg(nand->dev, "corrected = %d\n", mtd->ecc_stats.corrected);

	return 0;
}

int ecc_read_subpage(struct mtd_info *mtd, struct nand_chip *chip,
			uint32_t offs, uint32_t len, uint8_t *buf, int page)
{
	struct nand_data *nand;

	nand = nand_data_get(mtd);
	dev_err(nand->dev, "ECC read subpage not supported!\n");

	return -1;
}
/*
 * program page with HW ecc support.
 * this function is called after the commands and adderess for this page sent.
 */
int ecc_write_page(struct mtd_info *mtd, struct nand_chip *chip,
			const uint8_t *buf, int oob_required, int page)
{
	struct mtd_oob_region oobregion;
	uint32_t cmd;
	struct nand_data *nand;

	nand = nand_data_get(mtd);

	dev_dbg(nand->dev, "Start ECC write page\n");

	BUG_ON(oob_required);

	al_nand_ecc_set_enabled(&nand->nand_obj, 1);

	nand_prog_page_begin_op(chip, page, 0, buf, mtd->writesize);

	mtd_ooblayout_ecc(mtd, 0, &oobregion);

	chip->cmdfunc(mtd, NAND_CMD_RNDIN,
			mtd->writesize + oobregion.offset, -1);

	cmd = AL_NAND_CMD_SEQ_ENTRY(
			AL_NAND_COMMAND_TYPE_WAIT_CYCLE_COUNT,
			0);

	al_nand_tx_set_enable(&nand->nand_obj, 1);
	al_nand_wp_set_enable(&nand->nand_obj, 0);

	al_nand_cmd_single_execute(&nand->nand_obj, cmd);

	dev_dbg(nand->dev, "spare bytes: %d\n", oobregion.length);
	nand_send_byte_count_command(&nand->nand_obj,
				AL_NAND_COMMAND_TYPE_SPARE_WRITE_COUNT,
				oobregion.length);

	nand_wait_cmd_fifo_empty(nand);

	al_nand_wp_set_enable(&nand->nand_obj, 1);
	al_nand_tx_set_enable(&nand->nand_obj, 0);

	al_nand_ecc_set_enabled(&nand->nand_obj, 0);

	return nand_prog_page_end_op(chip);
}
#endif

/******************************************************************************/
/****************************** interrupts ************************************/
/******************************************************************************/
static irqreturn_t al_nand_isr(int irq, void *dev_id);

static void nand_interrupt_init(struct nand_data *nand)
{
	int ret;

	init_completion(&nand->complete);
	al_nand_int_disable(&nand->nand_obj, 0xffff);

	ret = devm_request_irq(nand->dev, nand->irq, al_nand_isr, IRQF_SHARED,
			       AL_NAND_NAME, nand);
	if (ret)
		pr_info("failed to request irq. rc %d\n", ret);
}
/*
 * ISR for nand interrupts - save the interrupt status, disable this interrupts
 * and nodify the waiting proccess.
 */
static irqreturn_t al_nand_isr(int irq, void *dev_id)
{
	struct nand_data *nand = dev_id;
	uint32_t irq_status = 0x0;

	irq_status = al_nand_int_status_get(&nand->nand_obj);

	al_nand_int_disable(&nand->nand_obj, irq_status);
	complete(&nand->complete);

	return IRQ_HANDLED;

}

/*
 * Waiting for interrupt with status in irq_mask to occur.
 * return 0 when reach timeout of 1 sec.
 */
static uint32_t wait_for_irq(struct nand_data *nand, uint32_t irq_mask)
{
	unsigned long comp_res = 0;
	unsigned long timeout = msecs_to_jiffies(1000);

	al_nand_int_enable(&nand->nand_obj, irq_mask);
	comp_res = wait_for_completion_timeout(&nand->complete, timeout);

	if (comp_res == 0) {
		/* timeout */
		dev_err(nand->dev, "%s: timeout occurred, irq mask = 0x%x\n",
			__func__, irq_mask);
		return -EINVAL;
	}
	return 0;

}

/******************************************************************************/
/**************************** configuration ***********************************/
/******************************************************************************/
static enum al_nand_ecc_bch_num_corr_bits bch_num_bits_convert(
							unsigned int bits)
{
	switch (bits) {
	case 4:
		return AL_NAND_ECC_BCH_NUM_CORR_BITS_4;
	case 8:
		return AL_NAND_ECC_BCH_NUM_CORR_BITS_8;
	case 12:
		return AL_NAND_ECC_BCH_NUM_CORR_BITS_12;
	case 16:
		return AL_NAND_ECC_BCH_NUM_CORR_BITS_16;
	case 20:
		return AL_NAND_ECC_BCH_NUM_CORR_BITS_20;
	case 24:
		return AL_NAND_ECC_BCH_NUM_CORR_BITS_24;
	case 28:
		return AL_NAND_ECC_BCH_NUM_CORR_BITS_28;
	case 32:
		return AL_NAND_ECC_BCH_NUM_CORR_BITS_32;
	case 36:
		return AL_NAND_ECC_BCH_NUM_CORR_BITS_36;
	case 40:
		return AL_NAND_ECC_BCH_NUM_CORR_BITS_40;
	default:
		BUG();
	}

	return AL_NAND_ECC_BCH_NUM_CORR_BITS_8;
}

static enum al_nand_device_page_size page_size_bytes_convert(
							unsigned int bytes)
{
	switch (bytes) {
	case 2048:
		return AL_NAND_DEVICE_PAGE_SIZE_2K;
	case 4096:
		return AL_NAND_DEVICE_PAGE_SIZE_4K;
	case 8192:
		return AL_NAND_DEVICE_PAGE_SIZE_8K;
	case 16384:
		return AL_NAND_DEVICE_PAGE_SIZE_16K;
	default:
		BUG();
	}

	return AL_NAND_DEVICE_PAGE_SIZE_4K;
}

static int al_nand_set_features(struct mtd_info *mtd, struct nand_chip *chip,
				int addr, u8 *subfeature_param)
{
	struct nand_data *nand = nand_data_get(mtd);
	uint32_t cmds[] = {
		AL_NAND_CMD_SEQ_ENTRY(AL_NAND_COMMAND_TYPE_CMD,
				      NAND_CMD_SET_FEATURES),
		AL_NAND_CMD_SEQ_ENTRY(AL_NAND_COMMAND_TYPE_ADDRESS, addr),
		AL_NAND_CMD_SEQ_ENTRY(AL_NAND_COMMAND_TYPE_STATUS_WRITE,
				      subfeature_param[0]),
		AL_NAND_CMD_SEQ_ENTRY(AL_NAND_COMMAND_TYPE_STATUS_WRITE,
				      subfeature_param[1]),
		AL_NAND_CMD_SEQ_ENTRY(AL_NAND_COMMAND_TYPE_STATUS_WRITE,
				      subfeature_param[2]),
		AL_NAND_CMD_SEQ_ENTRY(AL_NAND_COMMAND_TYPE_STATUS_WRITE,
				      subfeature_param[3])
	};

	al_nand_cmd_seq_execute(&nand->nand_obj, cmds, ARRAY_SIZE(cmds));

	nand_wait_cmd_fifo_empty(nand);
	return 0;
}

static void nand_set_timing_mode(struct mtd_info *mtd,
				 struct nand_chip *chip,
				 enum al_nand_device_timing_mode timing)
{
	u8 subfeature_param[ONFI_SUBFEATURE_PARAM_LEN] = {
		(u8)timing, 0x00, 0x00, 0x00
	};

	al_nand_set_features(mtd, chip, ONFI_FEATURE_ADDR_TIMING_MODE,
			     subfeature_param);
}

static int nand_resources_get_and_map(struct device *dev,
				      void __iomem **nand_base,
				      void __iomem **pbs_base)
{
	struct device_node *np;

	np = of_find_compatible_node(
			NULL, NULL, "annapurna-labs,al-nand");

	*nand_base = of_iomap(np, 0);
	if (!(*nand_base)) {
		dev_err(dev, "Failed to map nand memory\n");
		return -ENOMEM;
	}

	np = of_find_compatible_node(
			NULL, NULL, "annapurna-labs,al-pbs");

	*pbs_base = of_iomap(np, 0);
	if (!(*pbs_base)) {
		dev_err(dev, "Failed to map PBS base\n");
		return -ENOMEM;
	}

	return 0;

}

static void nand_onfi_config_set(
		struct mtd_info *mtd,
		struct al_nand_dev_properties *device_properties,
		struct al_nand_ecc_config *ecc_config)
{
	struct nand_chip *nand = mtd_to_nand(mtd);

	enum al_nand_device_page_size onfi_page_size;
	int i;
	u16 async_timing_mode;

	/* Addressing RMN: 2903 */
	device_properties->timingMode = AL_NAND_DEVICE_TIMING_MODE_MANUAL;

	async_timing_mode = onfi_get_async_timing_mode(nand);
	/*
	 * If the device contains async timing mode information, use it to find the max timing mode
	 * supported by both the device and AL_NAND_MAX_ONFI_TIMING_MODE. Otherwise (non ONFI mode)
	 * use AL_NAND_MAX_ONFI_TIMING_MODE.
	 */
	for (i = AL_NAND_MAX_ONFI_TIMING_MODE;
		async_timing_mode && i && (!((1 << i) & async_timing_mode)); i--)
		;
	al_nand_timing_params_set(&device_properties->timing, i);

	nand_set_timing_mode(mtd, nand, device_properties->timingMode);

	device_properties->num_col_cyc = nand->page_shift;
	device_properties->num_row_cyc = nand->chip_shift;
	onfi_page_size = page_size_bytes_convert(mtd->writesize);
	device_properties->pageSize = onfi_page_size;

#ifdef AL_NAND_ECC_SUPPORT
	if (nand->ecc_strength_ds == 1) {
		ecc_config->algorithm = AL_NAND_ECC_ALGORITHM_HAMMING;
	} else if (nand->ecc_strength_ds > 1) {
		ecc_config->algorithm = AL_NAND_ECC_ALGORITHM_BCH;
		ecc_config->num_corr_bits =
			bch_num_bits_convert(nand->ecc_strength_ds);
	}
#endif
}

static int al_nand_ooblayout_ecc(struct mtd_info *mtd, int section,
				  struct mtd_oob_region *oobregion)
{
	struct nand_chip *nand = mtd_to_nand(mtd);
	struct nand_data *nand_dat = nand_get_controller_data(nand);

	int ecc_loc = nand_dat->ecc_config.spareAreaOffset -
			mtd->writesize;

	if (section)
		return -ERANGE;

	oobregion->length = mtd->oobsize - ecc_loc;
	oobregion->offset = ecc_loc;

	return 0;
}

static int al_nand_ooblayout_free(struct mtd_info *mtd, int section,
				   struct mtd_oob_region *oobregion)
{
	struct nand_chip *nand = mtd_to_nand(mtd);
	struct nand_data *nand_dat = nand_get_controller_data(nand);

	int ecc_loc = nand_dat->ecc_config.spareAreaOffset -
			mtd->writesize;

	if (section)
		return -ERANGE;

	oobregion->offset = 2;
	oobregion->length = ecc_loc - 2 ;

	return 0;
}

static const struct mtd_ooblayout_ops al_nand_ooblayout_ops = {
	.ecc = al_nand_ooblayout_ecc,
	.free = al_nand_ooblayout_free,
};

static void nand_ecc_config(
		struct nand_chip *nand,
		struct nand_ecc_ctrl *ecc,
		uint32_t hw_ecc_enabled)
{
#ifdef AL_NAND_ECC_SUPPORT
	if (hw_ecc_enabled != 0) {
		ecc->mode = NAND_ECC_HW;

		ecc->read_page = ecc_read_page;
		ecc->read_subpage = ecc_read_subpage;
		ecc->write_page = ecc_write_page;

		ecc->strength = nand->ecc_strength_ds;
		ecc->size = nand->ecc_step_ds;
	} else {
		ecc->mode = NAND_ECC_NONE;
	}
#else
	ecc->mode = NAND_ECC_NONE;
#endif
}

static int al_nand_attach_chip(struct nand_chip *chip)
{
	struct nand_data *nand_dat;
	struct mtd_info *mtd;
	int ret;

	mtd = nand_to_mtd(chip);
	nand_dat = nand_get_controller_data(chip);

	BUG_ON(mtd->oobsize > AL_NAND_MAX_OOB_SIZE);

	nand_onfi_config_set(mtd, &nand_dat->device_properties,
			     &nand_dat->ecc_config);

	mtd_set_ooblayout(mtd, &al_nand_ooblayout_ops);

	nand_ecc_config(chip, &chip->ecc, nand_dat->dev_ext_props.eccIsEnabled);

	ret = al_nand_dev_config(&nand_dat->nand_obj,
				 &nand_dat->device_properties,
				 &nand_dat->ecc_config);
	if (ret) {
		dev_err(nand_dat->dev, "Device configuration failed\n");
		return -EIO;
	}

	return 0;
}

static const struct nand_controller_ops al_nand_controller_ops = {
	.attach_chip = al_nand_attach_chip,
};

static int al_nand_probe(struct platform_device *pdev)
{
	struct mtd_info *mtd;
	struct nand_chip *nand;
	struct nand_data *nand_dat;
	int ret = 0;
	void __iomem *nand_base;
	void __iomem *pbs_base;
	struct device *dev = &pdev->dev;
	struct clk *clk;

	nand_dat = devm_kzalloc(dev, sizeof(*nand_dat), GFP_KERNEL);
	if (!nand_dat)
		return -ENOMEM;

	pr_info("%s: AnnapurnaLabs nand driver\n", __func__);

	nand = &nand_dat->chip;
	mtd = nand_to_mtd(nand);
	mtd->dev.parent = dev;

	nand_dat->cache_pos = -1;
	nand_set_controller_data(nand, nand_dat);
	nand_dat->dev = dev;
	nand_controller_init(&nand_dat->controller);

	dev_set_drvdata(dev, nand_dat);

	nand_set_flash_node(nand, dev->of_node);

	mtd->name = AL_NAND_NAME;

	ret = nand_resources_get_and_map(dev, &nand_base, &pbs_base);
	if (ret != 0)
		goto err;

	ret = al_nand_init(&nand_dat->nand_obj,	nand_base, NULL, 0);
	if (ret != 0) {
		dev_err(dev, "Failed to init NAND\n");
		goto err;
	}

	if (0 != al_nand_dev_config_basic(&nand_dat->nand_obj)) {
		dev_err(dev, "Failed to config basic device\n");
		ret = -EIO;
		goto err;
	}

	nand_dat->irq = platform_get_irq(pdev, 0);
	if (nand_dat->irq < 0) {
		dev_err(dev, "No irq defined\n");
		return -ENXIO;
	}
	nand_interrupt_init(nand_dat);

	clk = clk_get(dev, NULL);

	if (IS_ERR(clk))
		dev_dbg(dev,
			"clock doesn't appear in the device tree. setting to default %ld",
			clk_freq_khz);
	else
		clk_freq_khz = clk_get_rate(clk) / 1000;

	nand->options = NAND_NO_SUBPAGE_WRITE;

	nand->cmd_ctrl = nand_cmd_ctrl;
	nand->read_byte = nand_read_byte_from_fifo;
	nand->read_word = nand_read_word;
	nand->read_buf = nand_read_buff;
	nand->dev_ready = nand_dev_ready;
	nand->write_buf = nand_write_buff;
	nand->select_chip = nand_dev_select;

	nand->set_features = al_nand_set_features;

	if (0 != al_nand_properties_decode(
					pbs_base,
					&nand_dat->device_properties,
					&nand_dat->ecc_config,
					&nand_dat->dev_ext_props)) {
		dev_err(dev, "Failed to decode properties\n");
		ret = -EIO;
		goto err;
	}

	/* must be set before scan_ident cause it uses read_buff */
	nand->ecc.size = 512 << nand_dat->ecc_config.messageSize;

	nand_dat->controller.ops = &al_nand_controller_ops;
	nand->controller = &nand_dat->controller;

	ret = nand_scan(nand, 1);
	if (ret) {
		dev_err(dev, "Failed to scan nand\n");
		goto err;
	}

	/* Register the partitions */
	ret = mtd_device_parse_register(mtd, part_probes, NULL, nand_partitions,
					ARRAY_SIZE(nand_partitions));
	if (ret) {
		dev_err(dev, "Failed to register mtd device\n");
		goto cleanup_nand;
	}

	return 0;

cleanup_nand:
	nand_cleanup(nand);

err:
	return ret;
}

static int al_nand_remove(struct platform_device *pdev)
{
	struct nand_data *nand_dat = dev_get_drvdata(&pdev->dev);

	dev_dbg(nand_dat->dev, "nand driver removed\n");

	nand_release(&nand_dat->chip);

	return 0;
}

static const struct of_device_id al_nand_match[] = {
	{ .compatible = "annapurna-labs,al-nand", },
	{}
};

static struct platform_driver al_nand_driver = {
	.driver = {
		.name = "annapurna-labs,al-nand",
		.owner = THIS_MODULE,
		.of_match_table = al_nand_match,
	},
	.probe = al_nand_probe,
	.remove = al_nand_remove,
};

static int __init nand_init(void)
{
	return platform_driver_register(&al_nand_driver);
}

static void __exit nand_exit(void)
{
	platform_driver_unregister(&al_nand_driver);
}

module_init(nand_init);
module_exit(nand_exit);

