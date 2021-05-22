#include <linux/init.h>
#include <linux/module.h>
#include <linux/spi/spi.h>
#include <linux/list.h>
#include <linux/delay.h>
#include <linux/of.h>
#include <linux/of_gpio.h>

#include "ubnt_poe_internal.h"

/* OF attributes */
/* Storage Register Clock Input */
#define UBNT_POE_OF_ATT_GPIO_STCP "gpio-stcp"

/* level shifter have 8 outputs*/
#define UBNT_POE_S2P_PORT_NUM 8

/* STCP is active-high signal */
#define GPIO_STCP_ACTIVE 1
#define GPIO_STCP_INACTIVE 0

static DEFINE_MUTEX(s2p_list_lock);
static struct list_head s2p_list;

/**
 * @brief Helpers
 */
#define ubnt_poe_s2p_lock() mutex_lock(&s2p_list_lock)
#define ubnt_poe_s2p_unlock() mutex_unlock(&s2p_list_lock)

/**
 * @brief Write PoE state to level shifter
 *
 * @param s2p - s2p control structure
 * @return int 0 on success, errno otherwise
 */
static int ubnt_poe_s2p_sync(struct ubnt_poe_s2p *s2p)
{
	int rc = 0;
	struct spi_message	m;
	struct spi_transfer	t = {
			.tx_buf		= &s2p->state,
			.len		= sizeof(s2p->state),
		};

	spi_message_init(&m);
	spi_message_add_tail(&t, &m);

	gpio_set_value(s2p->gpio_stcp, GPIO_STCP_ACTIVE);
	rc = spi_sync_locked(s2p->spi_device, &m);
	if (rc < 0) {
		pr_warn("%s transfer failed with status %d\n", __func__, rc);
	}
	gpio_set_value(s2p->gpio_stcp, GPIO_STCP_INACTIVE);

	return 0;
}

/**
 * @brief Enable/Disable port
 *
 * @param en - s2p control structure
 * @param port - port number
 * @param mode - poe mode
 * @return int 0 on success, errno otherwise
 */
void ubnt_poe_s2p_mode_set(struct ubnt_poe_s2p *s2p, uint32_t port, uint32_t mode)
{
	if (NULL == s2p || port >= UBNT_POE_S2P_PORT_NUM) {
		return;
	}

	if (POE_MODE_OFF == mode) {
		s2p->state &= ~(1 << port);
	} else {
		s2p->state |= (1 << port);
	}

	ubnt_poe_s2p_sync(s2p);
}
EXPORT_SYMBOL(ubnt_poe_s2p_mode_set);

/**
 * @brief Lock & get PoE s2p
 *
 * NOTE: ubnt_poe_s2p_put should be called after work is done to free the s2p
 * @param s2p - pointer ubnt-poe s2p structure
 */
static void ubnt_poe_s2p_get(struct ubnt_poe_s2p *s2p)
{
	if (s2p) {
		mutex_lock(&s2p->lock);
	}
}

/**
 * @brief Unlock PoE s2p after ubnt_poe_s2p_get
 *
 * NOTE: ubnt_poe_s2p_put should be called after work is done to free the s2p
 * @param s2p - ubnt-poe s2p structure
 */
void ubnt_poe_s2p_put(struct ubnt_poe_s2p *s2p)
{
	if (s2p) {
		mutex_unlock(&s2p->lock);
	}
}
EXPORT_SYMBOL(ubnt_poe_s2p_put);

/**
 * @brief Lock & get PoE s2p by its address
 *
 * NOTE: ubnt_poe_s2p_put should be called after work is done to free the s2p
 * @param uaddr - s2p uaddr
 * @return struct ubnt_poe_s2p*, pointer ubnt-poe s2p structure, NULL on fail
 */
struct ubnt_poe_s2p *ubnt_poe_s2p_get_by_uaddr(int uaddr)
{
	struct ubnt_poe_s2p *s2p = NULL;
	struct ubnt_poe_s2p *s2p_it;

	ubnt_poe_s2p_lock();

	list_for_each_entry (s2p_it, &s2p_list, list) {
		if (uaddr != s2p_it->uaddr) {
			continue;
		}
		s2p = s2p_it;
		break;
	}

	ubnt_poe_s2p_get(s2p);

	ubnt_poe_s2p_unlock();
	return s2p;
}
EXPORT_SYMBOL(ubnt_poe_s2p_get_by_uaddr);

/**
 * @brief Acquire GPIO pin (STCP)
 *
 * @param stcp - GPIO number
 * @return int 0 on success, errno otherwise
 */
static int ubnt_poe_s2p_stcp_setup(uint32_t stcp)
{
	int rc = 0;

	if (!gpio_is_valid(stcp)) {
		pr_err("%s Invalid gpio %d", __func__, stcp);
		return -EINVAL;
	}

	rc = gpio_request(stcp, "ubnt_poe_s2p");
	if (rc) {
		pr_err("%s unable to request gpio (%x)", __func__, rc);
		return rc;
	}

	rc = gpio_direction_output(stcp, GPIO_STCP_ACTIVE);
	if (rc) {
		pr_err("%s unable to set gpio as output (%x)", __func__, rc);
		goto error;
	}

	return 0;

error:
	gpio_free(stcp);
	return rc;
}

/**
 * @brief Free PoE s2p resources
 *
 * @param s2p - s2p control structure
 */
static void ubnt_poe_s2p_free(struct ubnt_poe_s2p *s2p)
{
	if (s2p) {
		gpio_free(s2p->gpio_stcp);
		kfree(s2p);
	}
}

/**
 * @brief Setup level shifter
 *
 * @param spi - spi device structure
 * @return struct ubnt_poe_s2p*, on success pointer to s2p structure,
 * NULL otherwise
 */
static struct ubnt_poe_s2p *ubnt_poe_s2p_setup(struct spi_device *spi)
{
	struct device_node *dev;
	struct ubnt_poe_s2p *s2p;
	uint32_t uaddr, stcp;
	int rc = 0;

	if (NULL == spi) {
		return NULL;
	}

	dev = spi->dev.of_node;

	rc = of_property_read_u32(dev, UBNT_POE_OF_ATT_UADDR, &uaddr);
	if (rc) {
		pr_err("%s Unable to obtain unit-address", __func__);
		goto error;
	}

	rc = of_property_read_u32(dev, UBNT_POE_OF_ATT_GPIO_STCP, &stcp);
	if (rc) {
		pr_err("%s Unable to obtain gpio cs", __func__);
		goto error;
	}

	rc = ubnt_poe_s2p_stcp_setup(stcp);
	if (rc) {
		pr_err("%s unable to setup gpio (%x)", __func__, rc);
		goto error;
	}

	s2p = kzalloc(sizeof(*s2p), GFP_KERNEL);
	if (NULL == s2p) {
		pr_err("%s Unable to allocate ubnt s2p control structure", __func__);
		gpio_free(stcp);
		goto error;
	}

	/* Initialization */
	INIT_LIST_HEAD(&s2p->list);
	mutex_init(&s2p->lock);
	s2p->uaddr = uaddr;
	s2p->gpio_stcp = stcp;
	s2p->spi_device = spi;
	s2p->state = 0x00;
	return s2p;

error:
	return NULL;
}

/**
 * @brief SPI probe function
 *
 * @param spi - spi device structure
 * @return int 0 on success, errno otherwise
 */
static int ubnt_poe_s2p_probe(struct spi_device *spi)
{
	struct ubnt_poe_s2p *s2p;
	int rc = 0;

	s2p = ubnt_poe_s2p_setup(spi);
	if (NULL == s2p) {
		/* no match */
		pr_err("%s Unable to setup ubnt poe s2p.", __func__);
		return -ENODEV;
	}

	/** Setup SPI */
	spi->bits_per_word = 8;
	rc = spi_setup(spi);
	if (rc < 0) {
		pr_err("%s Unable to setup spi for s2p ID %d", __func__, s2p->uaddr);
		ubnt_poe_s2p_free(s2p);
		return rc;
	}

	ubnt_poe_s2p_lock();
	list_add_tail(&s2p->list, &s2p_list);
	ubnt_poe_s2p_unlock();

	pr_err("Successfully registered PoE s2p with uaddr %d", s2p->uaddr);

	return 0;
}

/**
 * @brief SPI remove function
 *
 * @param spi - spi device structure
 * @return int 0 on success, errno otherwise
 */
static int ubnt_poe_s2p_remove(struct spi_device *spi)
{
	struct list_head *c, *n;
	struct ubnt_poe_s2p *s2p;

	ubnt_poe_s2p_lock();
	list_for_each_safe (c, n, &s2p_list) {
		s2p = list_entry(c, struct ubnt_poe_s2p, list);
		if (s2p->spi_device == spi) {
			list_del(c);
			gpio_free(s2p->gpio_stcp);
			ubnt_poe_s2p_free(s2p);
		}
	}
	ubnt_poe_s2p_unlock();

	return 0;
}

static const struct of_device_id ubnt_poe_s2p_of_match[] =
{
	{
		.compatible = "alpine,74hc595",
	},
	{ /* end of table */ }
};

MODULE_DEVICE_TABLE(of, ubnt_poe_s2p_of_match);

static struct spi_driver ubnt_poe_s2p_driver = {
	.driver = {
		.name = "ubnt_s2p_shift_register",
		.owner = THIS_MODULE,
		.of_match_table = ubnt_poe_s2p_of_match
	},
	.probe = ubnt_poe_s2p_probe,
	.remove = ubnt_poe_s2p_remove,
};

static int __init ubnt_poe_s2p_init(void)
{
	INIT_LIST_HEAD(&s2p_list);
	return spi_register_driver(&ubnt_poe_s2p_driver);
}
module_init(ubnt_poe_s2p_init);

static void __exit ubnt_poe_s2p_exit(void)
{
	spi_unregister_driver(&ubnt_poe_s2p_driver);
}
module_exit(ubnt_poe_s2p_exit);

MODULE_DESCRIPTION("Ubnt PoE serial-parallel shift register");
