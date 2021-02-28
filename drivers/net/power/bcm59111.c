/*
 * bcm59111.c BRCM59111 PSE driver
 *
 */

#include <error.h>
#include <linux/i2c.h>
#include <socket.h>
#include <linux/netlink.h>
#include <types.h>
#include <unistd.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/of_gpio.h>

#ifdef CONFIG_UBNT_POE_DEV
#include "ubnt_poe_internal.h"
#endif

#include "al_hal_common.h"
#include "bcm59111.h"
#include "BCM59111_POETEC_VER_21_RELEASE.h"
#include "BCM59121_POETEC_VER_15_RELEASE.h"

struct bcm59111_chip {
	struct i2c_client *cl;
	struct mutex lock; /* lock for user-space interface */
	u8 mode;
	int en_gpio;
	const struct firmware *fw;
#ifdef CONFIG_UBNT_POE_DEV
	struct ubnt_poe_dev ubnt_poe;
#endif
};

#define I2C_POE_SADDR1 0x29

#ifdef ARRAY_SIZE
#undef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#endif

#define BCM59111_P1_CONNECT 0x40
#define BCM59111_P2_CONNECT 0x10
#define BCM59111_P3_CONNECT 0x04
#define BCM59111_P4_CONNECT 0x01

#define BCM59111_P1_DISCONNECT 0x3f
#define BCM59111_P2_DISCONNECT 0xcf
#define BCM59111_P3_DISCONNECT 0xf3
#define BCM59111_P4_DISCONNECT 0xfc

#define BCM59111_RST_IOCTL_CMD 0x0
#define BCM59111_INIT_IOCTL_CMD 0x1
#define BCM59111_CFG_IOCTL_CMD 0x2
#define BCM59111_CRC_IOCTL_CMD 0x3
#define BCM59111_LED_INIT_IOCTL_CMD 0x4
#define BCM59111_PROBE_IOCTL_CMD 0x5

#define BCM59111_POE0_MUX_MASK (0x1 << 4)
#define BCM59111_POE1_MUX_MASK (0x1 << 5)
#define BCM59111_RST_GPIO 9

#define CHIPCOMMONA_GPIOOUTEN 0x18000068
#define CHIPCOMMONA_GPIOOUT 0x18000064

#define DEVICE_NUM 1
#define I2C_POE_SADDR0 0x30
#define SOC_E_NONE 0
enum {
	BCM59111_EVT_P1 = 0x1,
	BCM59111_EVT_P2 = 0x2,
	BCM59111_EVT_P3 = 0x4,
	BCM59111_EVT_P4 = 0x8,
};

//extern int bcm59111_netlink_init(int unit);

#define BCM59111_CURR_LSB 0
#define BCM59111_CURR_MSB 1
#define BCM59111_VOLT_LSB 2
#define BCM59111_VOLT_MSB 3
#define D14 (100000000000000UL)
#define D11 (100000000000UL)
#define POWER_UNIT (unsigned long)(12207*5835)

#define BCM59111_POWER_ADDR_BASE 0x30
#define BCM59111_POWER_CLASS_BASE 0x0C
#define BCM59111_PORT_OFFSET 4
#define BCM59111_PORT_NUMBER 4

#define IS_AF(class) \
	((class > 0 && class <= 3) || class == 6)

#define BCM59111_PORT_STATUS_GOOD 4

int soc_i2c_read_byte_data(int unit, uint8_t saddr, uint16_t addr, uint8_t *data)
{
	return 0;
}

static int bcm59111_firmware_load(struct i2c_client *client, const uint8_t *firmware, int size);
static void bcm59111_post_init(struct i2c_client *client);
static int bcm59111_detect(struct i2c_client *client, struct i2c_board_info *info);

static int bcm59111_firmware_crc(struct i2c_client *client);

void bcm59111_board_alert(void)
{
	/* Not Yet Implement */
}

void (*bcm59111_board_handler[])(void) = {
	bcm59111_board_alert,
};

static const struct reg_init pse_init_cmds[] = {
	{
		.reg = BCM59111_INT_MASK_REG, //0x01
		.val = BCM59111_INT_F_PWR_GOOD | BCM59111_INT_F_DIS,
	},
	{
		.reg = BCM59111_OP_MOD_REG, //0x12
#ifdef CONFIG_UBNT_POE_DEV
		.val = 0x00,
#else
		.val = 0xFF,
#endif /* CONFIG_UBNT_POE_DEV */
	},
	{
		.reg = BCM59111_DIS_SEN_EN_REG, //0x13
		.val = 0xf,
	},
	{
		.reg = BCM59111_DET_CLASS_EN_REG, //0x14
		.val = 0xff,
	},

	/* disable global high power feature */
	{
		.reg = BCM59111_HP_EN_REG, //0x44
		.val = 0x00,
	},

	/* Port 1 high power feature */
	{
		.reg = BCM59111_HP_P1_MOD_REG, //0x46
		.val = 0x01,
	},
	{
		.reg = BCM59111_HP_P1_OV_CUT_REG, //0x47
		.val = 0x50,
	},
	{
		.reg = BCM59111_HP_P1_CL_FBC_REG, //0x48
		.val = 0x0,
	},

	/* Port 2 high power feature */
	{
		.reg = BCM59111_HP_P2_MOD_REG,
		.val = 0x01,
	},
	{
		.reg = BCM59111_HP_P2_OV_CUT_REG,
		.val = 0x50,
	},
	{
		.reg = BCM59111_HP_P2_CL_FBC_REG,
		.val = 0x0,
	},

	/* Port 3 high power feature */
	{
		.reg = BCM59111_HP_P3_MOD_REG,
		.val = 0x01,
	},
	{
		.reg = BCM59111_HP_P3_OV_CUT_REG,
		.val = 0x50,
	},

	{
		.reg = BCM59111_HP_P3_CL_FBC_REG,
		.val = 0x0,
	},

	/* Port 4 high power feature */
	{
		.reg = BCM59111_HP_P4_MOD_REG,
		.val = 0x01,
	},
	{
		.reg = BCM59111_HP_P4_OV_CUT_REG,
		.val = 0x50,
	},
	{
		.reg = BCM59111_HP_P4_CL_FBC_REG,
		.val = 0x0,
	},

};

#ifdef CONFIG_UBNT_POE_DEV
/* QA : Do we need to support passive POE ? */
#define BCM59111_CAP (POE_CAP_ACTV_3AT | POE_CAP_OFF)
#define BCM59111_MODE_VALID(mode) (POE_MODE_ACTV_3AT == mode || POE_MODE_OFF == mode)
#define BCM59111_BYTE_READ(_reg, _res)                                                  \
	do {                                                                                \
		_res = i2c_smbus_read_byte_data(client, (_reg));                                \
		if ((_res) < 0) {                                                               \
			dev_err(&client->dev, "%s : i2s smbus read error %d\n", __func__, (_res));  \
			return (_res);                                                              \
		}                                                                               \
	} while (0)

#define BCM59111_BYTE_WRITE(_reg, _data, _res)                                          \
	do {                                                                                \
		_res = i2c_smbus_write_byte_data(client, (_reg), (_data));                      \
		if ((_res) < 0) {                                                               \
			dev_err(&client->dev, "%s : i2s smbus write error %d\n", __func__, (_res)); \
			return (_res);                                                              \
		}                                                                               \
	} while (0)

/**
 * @brief Get port capabilities
 *
 * @param dev - ubnt-poe device structure
 * @param port - port number
 * @param cap_out [out] - supported modes
 * @return int 0 on success, errno otherwise
 */
static int ubnt_poe_bcm59111_port_cap_get(struct ubnt_poe_dev *dev, uint32_t port, uint32_t *cap_out)
{
	if (port >= BCM59111_PORT_NUMBER || NULL == cap_out) {
		return -EINVAL;
	}
	*cap_out = BCM59111_CAP;
	return 0;
}

/**
 * @brief Set status port handler - retrieve status of PoE port
 *
 * @param dev - ubnt-poe device structure
 * @param port - port number
 * @param mode [in] - poe mode
 * @return int 0 on success, errno otherwise
 */
static int ubnt_poe_bcm59111_port_mode_set(struct ubnt_poe_dev *dev, uint32_t port, uint32_t mode)
{
	struct i2c_client *client = to_i2c_client(dev->dev);
	int mod_new, mod_old, rc = 0;

	if (!BCM59111_MODE_VALID(mode)) {
		return -EOPNOTSUPP;
	}

	if (port >= BCM59111_PORT_NUMBER) {
		return -EINVAL;
	}

	/* read ongoing mode */
	BCM59111_BYTE_READ(BCM59111_OP_MOD_REG, rc);

	mod_old = BCM59111_DEV_GET_PORT_OPMODE(port, rc);
	mod_new = (POE_MODE_ACTV_3AT == mode) ? BCM59111_OP_MOD_AUTO : BCM59111_OP_MOD_SHUT;

	/* do nothing if the modes are the same*/
	if (mod_old == mod_new) {
		return 0;
	}
	/* set mode */
	BCM59111_BYTE_WRITE(BCM59111_OP_MOD_REG, BCM59111_DEV_SET_PORT_OPMODE(port, rc, mod_new), rc);
	/* Enable detection and classification */
	BCM59111_BYTE_WRITE(BCM59111_DET_CLASS_EN_REG, 0xff, rc);

	return rc;
}

/**
 * @brief Get status port handler - retrieve status of PoE port
 *
 * @param dev - ubnt-poe device structure
 * @param port - port number
 * @param mode_out [out] - poe mode
 * @return int 0 on success, errno otherwise
 */
static int ubnt_poe_bcm59111_port_mode_get(struct ubnt_poe_dev *dev, uint32_t port, uint32_t *mode_out)
{
	struct i2c_client *client = to_i2c_client(dev->dev);
	int rc = 0, mode;

	if (port >= BCM59111_PORT_NUMBER || NULL == mode_out) {
		return -EINVAL;
	}

	/* read ongoing mode */
	BCM59111_BYTE_READ(BCM59111_OP_MOD_REG, rc);

	mode = BCM59111_DEV_GET_PORT_OPMODE(port, rc);

	switch (mode) {
	case BCM59111_OP_MOD_SHUT:
		*mode_out = POE_MODE_OFF;
		break;
	case BCM59111_OP_MOD_AUTO:
		*mode_out = POE_MODE_ACTV_3AT;
		break;
	default:
		dev_err(&client->dev, "%s : unknown operation mode %d\n", __func__, mode);
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Get status port handler - retrieve status of PoE port
 *
 * @param dev - ubnt-poe device structure
 * @param port - port number
 * @param status_out [out] - port status
 * @return int 0 on success, errno otherwise
 */
static int ubnt_poe_bcm59111_port_status_get(struct ubnt_poe_dev *dev, uint32_t port,
						 struct ubnt_poe_status *status_out)
{
	struct i2c_client *client = to_i2c_client(dev->dev);
	uint16_t volt = 0, curr = 0;
	int v,i, offset;

	if (NULL == status_out || port >= BCM59111_PORT_NUMBER) {
		return -EINVAL;
	}

	offset = BCM59111_PORT_OFFSET * port;

	/**
	 * @note
	 * Voltage and current measurements are 16-bit words, divided into two bytes.
	 * The Most Significant Byte (MSB) contains the upper 8 bits; and the Least
	 * Significant Byte (LSB) contains the lower 8 bits. Reading the lower byte
	 * latches the upper byte to assure they are both from the same sample;
	 * therefore, the lower byte should always be read first. After concatenating
	 * the upper and lower bytes the following conversion factors are used: for
	 * current measurements multiply by 122.07 μA/count (typical) and for voltage
	 * measurements multiply by 5.835mV/count
	 */

	BCM59111_BYTE_READ(BCM59111_POWER_ADDR_BASE + BCM59111_CURR_LSB + offset, i);
	BCM59111_BYTE_READ(BCM59111_POWER_ADDR_BASE + BCM59111_VOLT_LSB + offset, v);
	curr = (i & 0xFF);
	volt = (v & 0xFF);
	BCM59111_BYTE_READ(BCM59111_POWER_ADDR_BASE + BCM59111_CURR_MSB + offset, i);
	BCM59111_BYTE_READ(BCM59111_POWER_ADDR_BASE + BCM59111_VOLT_MSB + offset, v);
	curr += ((i & 0xFF) << 8);
	volt += ((v & 0xFF) << 8);
	status_out->power_mw = (curr*volt*POWER_UNIT)/D11;

	return 0;
}

/**
 * @brief Setters/Getters
 *
 */
static const struct ubnt_poe_dev_ops bcm59111_ops = {
	.port_mode_set = ubnt_poe_bcm59111_port_mode_set,
	.port_mode_get = ubnt_poe_bcm59111_port_mode_get,
	.port_cap_get = ubnt_poe_bcm59111_port_cap_get,
	.port_status_get = ubnt_poe_bcm59111_port_status_get
};

/**
 * @brief Register PoE chip to ubnt-poe layer
 *
 * @param chip - drivers private data
 * @return int errno
 */
static int ubnt_poe_bcm59111_reg(struct bcm59111_chip* chip) {

	struct ubnt_poe_dev* dev = &chip->ubnt_poe;
	dev->dev = &chip->cl->dev;
	dev->ops = &bcm59111_ops;
	return ubnt_poe_dev_reg(dev);
}
#endif /* CONFIG_UBNT_POE_DEV */

static void bcm59111_post_init(struct i2c_client *client)
{
	int i, ret;
	char data[1];
	for (i = 0; i < ARRAY_SIZE(pse_init_cmds); i++) {
		data[0] = pse_init_cmds[i].val;
		ret = i2c_smbus_write_i2c_block_data(client, pse_init_cmds[i].reg, 1, data);
		dev_dbg(&client->dev, "%s post_init_cmd[%d] reg 0x%x data 0x%x ret %d\n", __func__,
			i, pse_init_cmds[i].reg, data[0], ret);
	}
}

#define BCM_59111_CRC_TIMEOUT_MSEC 1000

static int bcm59111_firmware_load(struct i2c_client *client, const uint8_t *firmware, int size)
{
	int i, timeout = BCM_59111_CRC_TIMEOUT_MSEC, crc_stat;
	int ret = 0;
	uint16_t pos = 0, tx_size = (SEGMENT_SIZE + BCM59111_HEADER_OFFSET);
	char segment[SEGMENT_SIZE + BCM59111_HEADER_OFFSET] = { 0 };
	char crc[1];
	/*  The follwing is firmware downlod format
	*  START <addr> 0x70 0x80 <img_off> <img_data> STOP
	*/
	segment[0] = BCM59111_FW_PROGRAM;

	while (size) {
		segment[1] = (pos >> 8) & 0xff;
		segment[2] = ((pos & 0xff));

		for (i = 0; i < SEGMENT_SIZE; i++)
			segment[i + BCM59111_HEADER_OFFSET] = firmware[pos + i];

		ret = i2c_smbus_write_i2c_block_data(client, BCM59111_FW_DWN_CTRL, tx_size,
							 segment);
		if (ret != 0)
			pr_err("%s size %d, ret %d\n", __func__, size, ret);
		size = size - SEGMENT_SIZE;
		pos = pos + SEGMENT_SIZE;
	}

	crc[0] = BCM59111_FW_FORCE_CRC;
	ret = i2c_smbus_write_i2c_block_data(client, BCM59111_FW_DWN_CTRL, 1, crc);
	if (ret != 0)
		dev_err(&client->dev, "FW_DWN_CTRL crc ret %d\n", ret);

	while (timeout > 0) {
		crc_stat = i2c_smbus_read_byte_data(client, BCM59111_CRC_STATUS);
		if (crc_stat < 0) {
			dev_err(&client->dev, "%s : i2s smbus read error %d\n", __func__, crc_stat);
			return crc_stat;
		}

		if (crc_stat == 0xaa) {
			dev_dbg(&client->dev, "FW succesfully loaded\n");
			return 0;
		}
		timeout -= 50;
		msleep(50);
	}

	return -EILSEQ;
}

static int bcm59111_firmware_crc(struct i2c_client *client)
{
	int i, firmware_status = 0;

	/* switch to poe board specific bus */
	for (i = 0; i < DEVICE_NUM; i++) {
		uint8_t crc_stat = 0xff;
		crc_stat = i2c_smbus_read_byte_data(client, BCM59111_CRC_STATUS);
		dev_dbg(&client->dev, "%s crc_stat %x\n", __func__, crc_stat);

		if (crc_stat == 0xaa) {
			firmware_status |= (0x1 << i);
		}
	}
	return firmware_status;
}

static int bcm59111_chips_detect(struct i2c_client *client)
{
	int i, probe_status = 0x0, dev_id = 0x0;
	struct bcm59111_chip *chip_data = i2c_get_clientdata(client);

	for (i = 0; i < DEVICE_NUM; i++) {
		dev_id = i2c_smbus_read_byte_data(client, BCM59111_DEV_ID_REG);
		chip_data->mode = dev_id;
		dev_dbg(&client->dev, "%s probe dev id %x\n", __func__, dev_id);

		if (dev_id != BCM59111_DEV_ID_REV && dev_id != BCM59121_DEV_ID_REV) {
			probe_status |= (0x1 << i);
		}
	}

	return probe_status;
}

enum bcm591xx_fw_state {
	BCM591XX_FW_STATE_LOADED_NOW,
	BCM591XX_FW_STATE_LOADED_BEFORE
};

static int bcm59111_loadfw(struct i2c_client *client)
{
	struct bcm59111_chip *chip_data = i2c_get_clientdata(client);
	int rc = 0, fw_exp;
	const unsigned char *fw_data;
	size_t fw_sz;

	if (NULL == chip_data) {
		return -EINVAL;
	}

	rc = i2c_smbus_read_byte_data(client, BCM59111_FW_REV_REG);
	dev_dbg(&client->dev, "%s read fw rev %x\n", __func__, rc);
	if(rc < 0) {
		return rc;
	}

	switch(chip_data->mode) {
		case BCM59111_DEV_ID_REV:
			fw_data = Kc_BCM59111_POETEC_VER_21_RELEASE;
			fw_sz	= SZ_BCM59111_POETEC_VER_21_RELEASE;
			fw_exp	= BCM59111_POETEC_VER;
			break;
		case BCM59121_DEV_ID_REV:
			fw_data = Kc_BCM59121_POETEC_VER_15_RELEASE;
			fw_sz	= SZ_BCM59121_POETEC_VER_15_RELEASE;
			fw_exp	= BCM59121_POETEC_VER;
			break;
		default:
			dev_err(&client->dev, "Unknown dev id %x\n", chip_data->mode);
			return -EOPNOTSUPP;
	}

	/* check if loaded FW version match the expected one */
	if(rc != fw_exp) {
		rc = bcm59111_firmware_load(client, fw_data, fw_sz);
		if(rc < 0) {
			return rc;
		}
		rc = BCM591XX_FW_STATE_LOADED_NOW;
	} else {
		rc = BCM591XX_FW_STATE_LOADED_BEFORE;
	}

	return rc;
}

static ssize_t firmware_enabled_store(struct device *dev, struct device_attribute *attr,
					  const char *buf, size_t size)
{
	struct i2c_client *client = to_i2c_client(dev);

	bcm59111_loadfw(client);

	return size;
}

static ssize_t firmware_enabled_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "0x%x\n", bcm59111_firmware_crc(to_i2c_client(dev)));
}

static ssize_t post_init_store(struct device *dev, struct device_attribute *attr, const char *buf,
				   size_t size)
{
	struct i2c_client *client = to_i2c_client(dev);

	if (bcm59111_firmware_crc(to_i2c_client(dev))) {
		bcm59111_post_init(client);
	}

	return size;
}

static ssize_t report_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	int i;
	for (i = 0; i < BCM59111_PORT_NUMBER; i++) {
		unsigned short volt, curr, class, detect;
		unsigned long ints1 = 0, ints2 = 0, power;
		curr = i2c_smbus_read_byte_data(client, BCM59111_POWER_ADDR_BASE +
								BCM59111_CURR_MSB +
								BCM59111_PORT_OFFSET * i);
		curr = (curr << 8) + i2c_smbus_read_byte_data(
						 client, BCM59111_POWER_ADDR_BASE + BCM59111_CURR_LSB +
								 BCM59111_PORT_OFFSET * i);

		volt = i2c_smbus_read_byte_data(client, BCM59111_POWER_ADDR_BASE +
								BCM59111_VOLT_MSB +
								BCM59111_PORT_OFFSET * i);
		volt = (volt << 8) + i2c_smbus_read_byte_data(
						 client, BCM59111_POWER_ADDR_BASE + BCM59111_VOLT_LSB +
								 BCM59111_PORT_OFFSET * i);

		class = i2c_smbus_read_byte_data(client, BCM59111_POWER_CLASS_BASE + i) >> 4;
		detect = i2c_smbus_read_byte_data(client, BCM59111_POWER_CLASS_BASE + i) & 0x0F;
		if (detect != BCM59111_PORT_STATUS_GOOD) {
			class = 0;
		}

		power = curr * volt * POWER_UNIT;
		ints1 = power / D14;
		ints2 = (power - ints1 * D14) / D11;

		sprintf(buf + strlen(buf), "\tP%d: %lu.%03lu W %s\n", i + 1, ints1, ints2,
			class == 4 ? "AT" : (IS_AF(class) ? "AF" : "N/A"));
	}
	return strlen(buf);
}

static DEVICE_ATTR_RW(firmware_enabled);
static DEVICE_ATTR_WO(post_init);
static DEVICE_ATTR_RO(report);

/*
 * ATTRIBUTES:
 *
 */

static struct attribute *bcm59111_attrs[] = {
	&dev_attr_firmware_enabled.attr,
	&dev_attr_post_init.attr,
	&dev_attr_report.attr,
	NULL,
};

static struct attribute_group bcm59111_attr_group = {
	.attrs = bcm59111_attrs,
};

static int bcm59111_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int status = 0, ret = 0;
	struct bcm59111_chip *chip_data;
	struct device_node *np = client->dev.of_node;

	status = sysfs_create_group(&client->dev.kobj, &bcm59111_attr_group);

	if (status) {
		dev_err(&client->dev, "Unable to export , error: %d\n", status);
	}

	chip_data = devm_kzalloc(&client->dev, sizeof(struct bcm59111_chip), GFP_KERNEL);
	if (!chip_data)
		return -ENOMEM;

	i2c_set_clientdata(client, chip_data);
	chip_data->cl = client;

	if (!of_property_read_u32(np, "en-gpio", &chip_data->en_gpio)) {
		dev_err(&client->dev, "en-gpio%d\n", chip_data->en_gpio);

		if (gpio_is_valid(chip_data->en_gpio)) {
			ret = devm_gpio_request(&client->dev, chip_data->en_gpio,
						dev_name(&client->dev));
			if (ret) {
				dev_err(&client->dev, "unable to req gpio%d: %d\n",
					chip_data->en_gpio, ret);
				goto err_probe;
			}
			ret = gpio_direction_output(chip_data->en_gpio, 1);
			if (ret)
				dev_err(&client->dev, "unable to enable gpio%d: %d\n",
					chip_data->en_gpio, ret);
			devm_gpio_free(&client->dev, chip_data->en_gpio);
		}
	}
	status = bcm59111_detect(client, NULL);
#ifdef CONFIG_UBNT_POE_DEV
	if (!status) {
		/* load FW - could take a while */
		status = bcm59111_loadfw(client);
		switch(status) {
			case BCM591XX_FW_STATE_LOADED_NOW:
				/* load default values */
				bcm59111_post_init(client);
				/* fallthrough */
			case BCM591XX_FW_STATE_LOADED_BEFORE:
				/* don't touch configuration on warm reboot */
				status = 0;
			break;
			default:
				dev_err(&client->dev, "unable to load FW (CRC error?) : %d\n", status);
				return status;
		}
		/* register chip in ubnt-poe */
		ret = ubnt_poe_bcm59111_reg(chip_data);
		if (ret) {
			dev_err(&client->dev, "unable to register to ubnt-poe proxy\n");
		}
	}
#endif /* CONFIG_UBNT_POE_DEV */

	return status;
err_probe:
	return ret;
}

static int bcm59111_remove(struct i2c_client *client)
{
	struct bcm59111_chip *chip_data = i2c_get_clientdata(client);
	#ifdef CONFIG_UBNT_POE_DEV
	if(NULL != chip_data) {
		ubnt_poe_dev_unreg(&chip_data->ubnt_poe);
	}
	#endif /* CONFIG_UBNT_POE_DEV */
	kfree(chip_data);
	sysfs_remove_group(&client->dev.kobj, &bcm59111_attr_group);
	return 0;
}

/* Return 0 if detection is successful, -ENODEV otherwise */
static int bcm59111_detect(struct i2c_client *client, struct i2c_board_info *info)
{
	int status = 0;
	status = bcm59111_chips_detect(client);
	dev_dbg(&client->dev, "%s status = %d", __func__, status);
	return status ? -ENODEV : status;
}
static const struct i2c_device_id bcm59111_id[] = { { "bcm59111", 0 }, {} };
MODULE_DEVICE_TABLE(i2c, bcm59111_id);

#ifdef CONFIG_OF
static const struct of_device_id of_bcm59111_pse_match[] = {
	{
		.compatible = "bcm59111",
	},
	{
		.compatible = "bcm59121",
	},
	{},
};

static struct i2c_driver bcm59111_driver = {
	.driver =
		{
			.name = "bcm59111",
			.of_match_table = of_match_ptr(of_bcm59111_pse_match),
		},
	.probe = bcm59111_probe,
	.detect = bcm59111_detect,
	.remove = bcm59111_remove,
	.id_table = bcm59111_id,
};

#endif

static int __init bcm59111_chip_init(void)
{
	return i2c_add_driver(&bcm59111_driver);
}

module_init(bcm59111_chip_init);

static void __exit bcm59111_chip_exit(void)
{
	i2c_del_driver(&bcm59111_driver);
}
module_exit(bcm59111_chip_exit);

MODULE_DESCRIPTION("BCM59111 PSE driver");
MODULE_LICENSE("GPL v2");
