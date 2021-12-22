/* al_eth.c: AnnapurnaLabs Unified 1GbE and 10GbE ethernet driver.
 *
 * Copyright (c) 2012 AnnapurnaLabs
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/moduleparam.h>

#include <linux/stringify.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/timer.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/pci.h>
#include <linux/init.h>
#include <linux/mdio.h>
#include <linux/mii.h>
#include <linux/phy.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/dma-mapping.h>
#include <linux/delay.h>
#include <linux/time.h>
#include <linux/ethtool.h>
#include <linux/if.h>
#include <linux/if_vlan.h>
#if defined(CONFIG_RFS_ACCEL) || defined(CONFIG_ARCH_ALPINE)
#include <linux/cpu_rmap.h>
#endif
#include <net/ip.h>
#include <net/tcp.h>
#include <net/checksum.h>
#include <linux/prefetch.h>
#include <linux/cache.h>
#include <linux/i2c.h>
#include <linux/u64_stats_sync.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#ifdef CONFIG_ARCH_ALPINE
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/crc32.h>
#endif

#include "al_mod_hal_eth.h"
#include "al_mod_eth_v4_proto_num_cam_config_auto_gen.h"
#include "al_mod_hal_serdes_25g.h"
#include "al_mod_init_eth_lm.h"
#include "al_mod_eth_group_lm.h"
#ifdef CONFIG_ARCH_ALPINE
#include "alpine_serdes.h"
#include "alpine_group_lm.h"
#ifdef CONFIG_ALPINE_SERDES_AVG
#include "alpine_avg_serdes.h"
#endif
#include "al_mod_eth_common.h"
#else
#include "al_mod_hal_serdes_hssp.h"
#endif

#include "al_eth.h"
#ifdef CONFIG_ARCH_ALPINE
#include <al_mod_hal_udma.h>
#include <al_mod_hal_udma_iofic.h>
#include <al_mod_hal_udma_debug.h>
#include <al_mod_hal_udma_config.h>
#else
#include "al_mod_hal_iomap_dynamic.h"
#include "al_mod_hal_udma.h"
#include "al_mod_hal_udma_iofic.h"
#include "al_mod_hal_udma_debug.h"
#include "al_mod_hal_udma_config.h"
#include "al_mod_hal_pcie.h"
#endif /* CONFIG_ARCH_ALPINE */
#include "al_mod_eth_sysfs.h"
#include "al_mod_hal_unit_adapter_regs.h"
#include "al_mod_hal_eth_ec_regs.h"
#include "al_mod_hal_unit_adapter.h"
#include "al_mod_hal_pbs_utils.h"
#include "al_mod_hal_pbs_regs.h"

#ifndef CONFIG_ARCH_ALPINE
#include "al_mod_tam.h"
#endif

#include "al_mod_eth_v3_lm_live_update.h"

#ifndef DRV_MODULE_NAME
#define DRV_MODULE_NAME		"al_eth"
#endif
#ifndef DRV_MODULE_VERSION
#define DRV_MODULE_VERSION	"3.5.3"
#endif
#define DRV_MODULE_RELDATE	"Mar 14, 2019"
#define DRV_MODULE_DESCRIPTION	"AnnapurnaLabs unified 1GbE/10GbE/25GbE Ethernet Driver with SR-IOV"

static char version[] = DRV_MODULE_DESCRIPTION " " DRV_MODULE_NAME " v" DRV_MODULE_VERSION " ("
			DRV_MODULE_RELDATE ")";

MODULE_AUTHOR("Saeed Bishara <saeed@annapurnaLabs.com>");
MODULE_DESCRIPTION(DRV_MODULE_DESCRIPTION);
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_MODULE_VERSION);

/* Time in jiffies before concluding the transmitter is hung. */
#define TX_TIMEOUT  (5*HZ)

/* Time in mSec to keep trying to read / write from MDIO in case of error */
#define MDIO_TIMEOUT_MSEC	100

/* Time in mSec to switch auto-FEC on init */
#define AUTO_FEC_INITIAL_TIMEOUT	1000

/* Time in mSec to switch auto-FEC on toggle */
#define AUTO_FEC_TOGGLE_TIMEOUT		500

static int disable_msi;

module_param(disable_msi, int, 0);
MODULE_PARM_DESC(disable_msi, "Disable Message Signaled Interrupt (MSI)");

#if defined(EMUL_PLAT) && (EMUL_PLAT == 1)
#define AL_ETH_PLAT_EMU 1
#endif

/** Default NIC mode port & udma numbers */
/** Linux kernel driver always uses port 0. On host driver it can be changes with loading of module in load.sh */
#define AL_ETH_DEFAULT_PORT  0

#if defined(CONFIG_ARCH_ALPINE) /** Linux kernerl driver uses udma 0 always */
#define AL_ETH_DEFAULT_UDMA  0
#else
#ifdef AL_ETH_PLAT_EMU
#define AL_ETH_DEFAULT_UDMA 0
#else
#define AL_ETH_DEFAULT_UDMA 1 /** host driver uses udma 1 by default, can be changed in load.sh */
#endif /** AL_ETH_PLAT_EMU */
#endif /** defined(CONFIG_ARCH_ALPINE) */

static int udma_num = AL_ETH_DEFAULT_UDMA;
module_param(udma_num, int, 0);
MODULE_PARM_DESC(udma_num, "udma number to use in V2 NIC mode");


static spinlock_t pcie_int_lock; /** Lock for interrupts disable/enable config */
static int pcie_int_lock_inited;

#ifdef CONFIG_ARCH_ALPINE
/** Port 0 should access all adv ports HAL adapter handles, save them in this array during probe */
#define AL_ETH_V4_PORTS_NUM	6	/** ADV & STD */
static struct al_mod_hal_eth_adapter *adapter_handles[AL_ETH_V4_PORTS_NUM];
static struct al_mod_eth_v4_lm_status eth_v4_lm_port_0_status;

#define AL_ETH_MAX_PORTS_NUM	AL_ETH_V4_PORTS_NUM

#ifdef CONFIG_ALPINE_SERDES_AVG
#define AL_ETH_V4_LM
#endif

#endif

#define DEFAULT_MSG_ENABLE (NETIF_MSG_DRV|NETIF_MSG_PROBE|NETIF_MSG_LINK)
static int debug = -1;
module_param(debug, int, 0);
MODULE_PARM_DESC(debug, "Debug level (0=none,...,16=all)");

#ifdef CONFIG_ARCH_ALPINE
#define AL_ETH_LIVE_UPDATE_RESTORE_DISABLE		0
#define AL_ETH_LIVE_UPDATE_RESTORE_ENABLE		1
#define AL_ETH_LIVE_UPDATE_RESTORE_USE_PRE_DEFINED	2
static int live_update_restore;
module_param(live_update_restore, int, 0);
MODULE_PARM_DESC(live_update_restore, "Live update restore (0=disable,1=restore state,2=use pre-defined state)");

#define AL_ETH_LIVE_UPDATE_SAVE_DISABLE		0
#define AL_ETH_LIVE_UPDATE_SAVE_ENABLE		1
static int live_update_save;
module_param(live_update_save, int, 0);
MODULE_PARM_DESC(live_update_save, "Live update save (0=disable,1=save state)");
#endif /* CONFIG_ARCH_ALPINE */

#define IS_V1_NIC(board_type) ((board_type) == ALPINE_NIC)

#define IS_V2_NIC(board_type) (((board_type) == ALPINE_NIC_V2_10) || \
			       ((board_type) == ALPINE_NIC_V2_25) || \
			       ((board_type) == ALPINE_NIC_V2_25_DUAL) || \
			       ((board_type) == ALPINE_NIC_V2_1G_TEST))

#define IS_V3_NIC(board_type) (((board_type) == ALPINE_NIC_V3_100) || \
			       ((board_type) == ALPINE_NIC_V3_4x25) || \
			       ((board_type) == ALPINE_NIC_V3_4x25_EX) || \
			       ((board_type) == ALPINE_NIC_V3_1G_TEST))

#define IS_NIC(board_type) (IS_V2_NIC(board_type) || IS_V1_NIC(board_type) || IS_V3_NIC(board_type))

#define AL_ETH_HAL_PKT_PROTO_NUM_IPV4(_pkt) (\
	((_pkt)->protocol_number == PROTO_NUM_IP_V4_TCP_OR_UDP) || \
	((_pkt)->protocol_number == PROTO_NUM_IP_V4))

#define AL_ETH_HAL_PKT_PROTO_NUM_IPV6(_pkt) (\
	((_pkt)->protocol_number == PROTO_NUM_IP_V6_TCP_OR_UDP) || \
	((_pkt)->protocol_number == PROTO_NUM_IP_V6))

#define AL_ETH_HAL_PKT_PROTO_NUM_TCP_UDP(_pkt) (\
	((_pkt)->protocol_number == PROTO_NUM_IP_V6_TCP_OR_UDP) || \
	((_pkt)->protocol_number == PROTO_NUM_IP_V4_TCP_OR_UDP))

#define IS_V2_ALPINE(_adapter) \
	(((_adapter)->board_type == ALPINE_INTEGRATED) && \
	 (((_adapter)->rev_id == AL_ETH_REV_ID_2) || \
	  ((_adapter)->rev_id == AL_ETH_REV_ID_3)))

#define AL_ETH_ERR_EVENTS_SUPPORTED(_adapter) \
	(IS_V2_NIC((_adapter)->board_type) || \
	 (IS_V2_ALPINE(_adapter) && pbs_regs_base))

/* Only UDMA0 can receive ec/mac interrupts */
#define EC_ERRS_COLLECTED_BY_INTR(_adapter) ((_adapter)->udma_num == 0)

/* 64bit bar devices use a different revision ID */
#define PCI_REV_ID_64B_BAR	0x01

/* indexed by board_t */
static struct {
	char *name;
	/* needed for NIC modes, newer modes expose a 64 bit BAR */
	unsigned int bar;
	unsigned int bar_64;
} board_info[ALPINE_BOARD_TYPE_MAX] = {
	[ALPINE_INTEGRATED] = {
		.name = "AnnapurnaLabs unified 1Gbe/10Gbe/25Gbe integrated",
	},
	[ALPINE_NIC] = {
		.name = "AnnapurnaLabs unified 1Gbe/10Gbe pcie NIC",
		.bar = 5,
		.bar_64 = 4,
	},
	[ALPINE_NIC_V2_10] = {
		.name = "AnnapurnaLabs unified 1Gbe/10Gbe pcie NIC",
		.bar = 5,
		.bar_64 = 4,
	},
	[ALPINE_NIC_V2_25] = {
		.name = "AnnapurnaLabs unified 10Gbe/25Gbe pcie NIC",
		.bar = 5,
		.bar_64 = 4,
	},
	[ALPINE_NIC_V2_25_DUAL] = {
		.name = "AnnapurnaLabs unified dual port 10Gbe/25Gbe pcie NIC",
		.bar = 5,
		.bar_64 = 4,
	},
	[ALPINE_NIC_V2_1G_TEST] = {
		.name = "AnnapurnaLabs V2 1G NIC",
		.bar = 5,
		.bar_64 = 4,
	},
	[ALPINE_NIC_V3_100] = {
		.name = "AnnapurnaLabs unified 100Gbe pcie NIC",
		.bar = 5,
		.bar_64 = 4,
	},
	[ALPINE_NIC_V3_4x25] = {
		.name = "AnnapurnaLabs unified 4x25Gbe pcie NIC",
		.bar = 5,
		.bar_64 = 4,
	},
	[ALPINE_NIC_V3_4x25_EX] = {
		.name = "AnnapurnaLabs unified 4x25Gbe pcie NIC (EX)",
		.bar = 5,
		.bar_64 = 4,
	},
	[ALPINE_NIC_V3_1G_TEST] = {
		.name = "AnnapurnaLabs V3 1G NIC",
		.bar = 5,
		.bar_64 = 4,
	},
};

/*
 * The sole puprose of this device_id, is to enable binding to ALPINE_NIC_V2_1G_TEST
 * at runtime, for testing the interface. This id is reserved for this use, and shouldn't
 * be used for a real device.
 */
#define PCI_DEVICE_ID_AL_ETH_NIC_V2_1G_TEST_DUMMY 0x6543
#define PCI_DEVICE_ID_AL_ETH_NIC_V3_1G_TEST_DUMMY 0x7654

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 8, 0)
static const struct pci_device_id al_mod_eth_pci_tbl[] = {
#else
static DEFINE_PCI_DEVICE_TABLE(al_mod_eth_pci_tbl) = {
#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(4, 8, 0) */
#ifdef PCI_DEVICE_ID_AL_ETH
	{ PCI_VENDOR_ID_ANNAPURNA_LABS, PCI_DEVICE_ID_AL_ETH,
	  PCI_ANY_ID, PCI_ANY_ID, 0, 0, ALPINE_INTEGRATED},
#endif
#ifdef PCI_DEVICE_ID_AL_ETH_ADVANCED
	{ PCI_VENDOR_ID_ANNAPURNA_LABS, PCI_DEVICE_ID_AL_ETH_ADVANCED,
	  PCI_ANY_ID, PCI_ANY_ID, 0, 0, ALPINE_INTEGRATED},
#endif
#ifdef PCI_DEVICE_ID_AL_ETH_NIC
	{ PCI_VENDOR_ID_ANNAPURNA_LABS, PCI_DEVICE_ID_AL_ETH_NIC,
	  PCI_ANY_ID, PCI_ANY_ID, 0, 0, ALPINE_NIC},
#endif
#ifdef PCI_DEVICE_ID_AL_ETH_NIC_V2_10
	{ PCI_VENDOR_ID_ANNAPURNA_LABS, PCI_DEVICE_ID_AL_ETH_NIC_V2_10,
	  PCI_ANY_ID, PCI_ANY_ID, 0, 0, ALPINE_NIC_V2_10},
#endif
#ifdef PCI_DEVICE_ID_AL_ETH_NIC_V2_25_R0A
	{ PCI_VENDOR_ID_ANNAPURNA_LABS, PCI_DEVICE_ID_AL_ETH_NIC_V2_25_R0A,
	  PCI_ANY_ID, PCI_ANY_ID, 0, 0, ALPINE_NIC_V2_25},
#endif
#ifdef PCI_DEVICE_ID_AL_ETH_NIC_V2_25_R0A1
	{ PCI_VENDOR_ID_ANNAPURNA_LABS, PCI_DEVICE_ID_AL_ETH_NIC_V2_25_R0A1,
	  PCI_ANY_ID, PCI_ANY_ID, 0, 0, ALPINE_NIC_V2_25},
#endif
#ifdef PCI_DEVICE_ID_AL_ETH_NIC_V2_K2C
	{ PCI_VENDOR_ID_ANNAPURNA_LABS, PCI_DEVICE_ID_AL_ETH_NIC_V2_K2C,
	  PCI_ANY_ID, PCI_ANY_ID, 0, 0, ALPINE_NIC_V2_25_DUAL},
#endif
#ifdef PCI_DEVICE_ID_AL_ETH_NIC_V2_1G_TEST_DUMMY
	{ PCI_VENDOR_ID_ANNAPURNA_LABS, PCI_DEVICE_ID_AL_ETH_NIC_V2_1G_TEST_DUMMY,
	  PCI_ANY_ID, PCI_ANY_ID, 0, 0, ALPINE_NIC_V2_1G_TEST},
#endif
#ifdef PCI_DEVICE_ID_AL_ETH_NIC_V3_100
	{ PCI_VENDOR_ID_ANNAPURNA_LABS, PCI_DEVICE_ID_AL_ETH_NIC_V3_100,
	  PCI_ANY_ID, PCI_ANY_ID, 0, 0, ALPINE_NIC_V3_100},
#endif
#ifdef PCI_DEVICE_ID_AL_ETH_NIC_V3_4x25
	{ PCI_VENDOR_ID_ANNAPURNA_LABS, PCI_DEVICE_ID_AL_ETH_NIC_V3_4x25,
	  PCI_ANY_ID, PCI_ANY_ID, 0, 0, ALPINE_NIC_V3_4x25},
#endif
#ifdef PCI_DEVICE_ID_AL_ETH_NIC_V3_4x25_EX
	{ PCI_VENDOR_ID_ANNAPURNA_LABS, PCI_DEVICE_ID_AL_ETH_NIC_V3_4x25_EX,
	  PCI_ANY_ID, PCI_ANY_ID, 0, 0, ALPINE_NIC_V3_4x25_EX},
#endif
#ifdef PCI_DEVICE_ID_AL_ETH_NIC_V3_1G_TEST_DUMMY
	{ PCI_VENDOR_ID_ANNAPURNA_LABS, PCI_DEVICE_ID_AL_ETH_NIC_V3_1G_TEST_DUMMY,
	  PCI_ANY_ID, PCI_ANY_ID, 0, 0, ALPINE_NIC_V3_1G_TEST},
#endif
	  { 0, }
};

MODULE_DEVICE_TABLE(pci, al_mod_eth_pci_tbl);

#ifdef CONFIG_AL_ETH_ALLOC_SKB
static DEFINE_PER_CPU(struct sk_buff_head, rx_recycle_cache);
#endif

/* the following defines should be moved to hal */
#define AL_ETH_CTRL_TABLE_PRIO_SEL_SHIFT	0
#define AL_ETH_CTRL_TABLE_PRIO_SEL_MASK		(0xF << AL_ETH_CTRL_TABLE_PRIO_SEL_SHIFT)
#define AL_ETH_CTRL_TABLE_PRIO_SEL_0		(12 << AL_ETH_CTRL_TABLE_PRIO_SEL_SHIFT)

#define AL_ETH_CTRL_TABLE_Q_SEL_SHIFT	4
#define AL_ETH_CTRL_TABLE_Q_SEL_MASK	(0xF << AL_ETH_CTRL_TABLE_Q_SEL_SHIFT)
#define AL_ETH_CTRL_TABLE_Q_SEL_THASH	(1 << AL_ETH_CTRL_TABLE_Q_SEL_SHIFT)

#define AL_ETH_CTRL_TABLE_Q_PRIO_SEL_SHIFT	8
#define AL_ETH_CTRL_TABLE_Q_PRIO_SEL_MASK	(0x3 << AL_ETH_CTRL_TABLE_Q_PRIO_SEL_SHIFT)
/* selected queue is hash output table */
#define AL_ETH_CTRL_TABLE_Q_PRIO_SEL_Q		(3 << AL_ETH_CTRL_TABLE_Q_PRIO_SEL_SHIFT)

#define AL_ETH_CTRL_TABLE_UDMA_SEL_SHIFT	10
#define AL_ETH_CTRL_TABLE_UDMA_SEL_MASK	(0xF << AL_ETH_CTRL_TABLE_UDMA_SEL_SHIFT)
/* select UDMA from rfw_default opt1 register */
#define AL_ETH_CTRL_TABLE_UDMA_SEL_DEF_1	(7 << AL_ETH_CTRL_TABLE_UDMA_SEL_SHIFT)
#define AL_ETH_CTRL_TABLE_UDMA_SEL_0	(15 << AL_ETH_CTRL_TABLE_UDMA_SEL_SHIFT)

#define AL_ETH_CTRL_TABLE_UDMA_SEL_MASK_INPUT	(1 << 14)

#define AL_ETH_CTRL_TABLE_USE_TABLE	(1 << 20)

#define AL_ETH_MAC_TABLE_UNICAST_IDX_BASE	0
#define AL_ETH_MAC_TABLE_UNICAST_MAX_COUNT	4
#define AL_ETH_MAC_TABLE_ALL_MULTICAST_IDX	(AL_ETH_MAC_TABLE_UNICAST_IDX_BASE + \
						AL_ETH_MAC_TABLE_UNICAST_MAX_COUNT)

#define AL_ETH_MAC_TABLE_DROP_IDX		(AL_ETH_FWD_MAC_NUM - 1)
#define AL_ETH_MAC_TABLE_BROADCAST_IDX		(AL_ETH_MAC_TABLE_DROP_IDX - 1)

#define MAC_ADDR_STR "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC_ADDR(addr) addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]

#define AL_ETH_SERDES_25G_OFFSET 0x2000 /** Offset into 25g serdes from serdes base */

/** MDIO clause 45 helpers */
#define AL_ETH_MDIO_C45_DEV_MASK	0x1f0000
#define AL_ETH_MDIO_C45_DEV_SHIFT	16
#define AL_ETH_MDIO_C45_REG_MASK	0xffff

#define AL_ETH_NUM_OF_DESCS_PRE_COMP_HEAD_PRINT	256

enum al_mod_eth_user_def_backup_command {
	AL_ETH_USER_DEF_BACKUP = 0,
	AL_ETH_USER_DEF_RESTORE
};

#define  AL_ETH_REF_CLK_FREQ_TO_HZ(ref_clk_freq)   \
	( ((ref_clk_freq) == AL_ETH_REF_FREQ_375_MHZ) ? 375000000 : (   \
	((ref_clk_freq) == AL_ETH_REF_FREQ_500_MHZ) ? 500000000 : (   \
	((ref_clk_freq) == AL_ETH_REF_FREQ_187_5_MHZ) ? 187500000 : ( \
	((ref_clk_freq) == AL_ETH_REF_FREQ_250_MHZ) ? 250000000 : (   \
	((ref_clk_freq) == AL_ETH_REF_FREQ_428_MHZ) ? 428000000 : 500000000)))))

/**
 * Interrupt moderation
 */

/* The interrupt coalescing resolution is (N+1) x 256 clock cycles of the IO Fabric (SB),
 * where N is the value of the mod_res field in the iofic register. sb_clk_freq is in KHz.
 * NOTICE: We use nsecs (instead of usecs) to avoid floating point arithmetic in the kernel.
 */
#define AL_ETH_INTR_MODERATION_RESOLUTION_NSECS(sb_clk_freq)	\
	((uint) (((AL_ETH_INTR_MODERATION_RESOLUTION + 1) * 256 * 1000000)	\
		/ (sb_clk_freq)))

#define AL_ETH_INTR_LOWEST_VALUE(sb_clk_freq)		((uint) (1000 * AL_ETH_INTR_LOWEST_USECS / \
	AL_ETH_INTR_MODERATION_RESOLUTION_NSECS(sb_clk_freq)))

#define AL_ETH_INTR_LOW_VALUE(sb_clk_freq)		((uint)(1000 * AL_ETH_INTR_LOW_USECS /	   \
	AL_ETH_INTR_MODERATION_RESOLUTION_NSECS(sb_clk_freq)))

#define AL_ETH_INTR_MID_VALUE(sb_clk_freq)		((uint)(1000 * AL_ETH_INTR_MID_USECS /	   \
	AL_ETH_INTR_MODERATION_RESOLUTION_NSECS(sb_clk_freq)))

#define AL_ETH_INTR_HIGH_VALUE(sb_clk_freq)		((uint)(1000 * AL_ETH_INTR_HIGH_USECS /	   \
	AL_ETH_INTR_MODERATION_RESOLUTION_NSECS(sb_clk_freq)))

#define AL_ETH_INTR_HIGHEST_VALUE(sb_clk_freq)		((uint)(1000 * AL_ETH_INTR_HIGHEST_USECS / \
	AL_ETH_INTR_MODERATION_RESOLUTION_NSECS(sb_clk_freq)))

/** Enable (AL_TRUE) / disable (AL_FALSE) interrupt moderation */
#ifdef CONFIG_ARCH_ALPINE
#define AL_ETH_DEFAULT_ADAPTIVE_INT_MODERATION	AL_FALSE
#else
#define AL_ETH_DEFAULT_ADAPTIVE_INT_MODERATION	AL_TRUE
#endif

static int adaptive_int_moderation = AL_ETH_DEFAULT_ADAPTIVE_INT_MODERATION;
module_param(adaptive_int_moderation, int, 0);
MODULE_PARM_DESC(adaptive_int_moderation,
	"Adaptive interrupt moderation Enable/Disable. 0 = Disable, 1 = Enable");

static struct al_mod_eth_intr_moderation_entry
default_moderation_table[AL_ETH_INTR_MAX_NUM_OF_LEVELS] = {
	{
		.packets_per_interval = AL_ETH_INTR_LOWEST_PKTS,
		.bytes_per_interval = AL_ETH_INTR_LOWEST_BYTES,
	},
	{
		.packets_per_interval =  AL_ETH_INTR_LOW_PKTS,
		.bytes_per_interval = AL_ETH_INTR_LOW_BYTES,
	},
	{
		.packets_per_interval = AL_ETH_INTR_MID_PKTS,
		.bytes_per_interval = AL_ETH_INTR_MID_BYTES,
	},
	{
		.packets_per_interval = AL_ETH_INTR_HIGH_PKTS,
		.bytes_per_interval = AL_ETH_INTR_HIGH_BYTES,
	},
	{
		.packets_per_interval = AL_ETH_INTR_HIGHEST_PKTS,
		.bytes_per_interval = AL_ETH_INTR_HIGHEST_BYTES,
	},
};

static struct al_mod_eth_intr_moderation_entry
initial_moderation_table[AL_ETH_INTR_MAX_NUM_OF_LEVELS] = {
	{
		.packets_per_interval = AL_ETH_INTR_LOWEST_PKTS,
		.bytes_per_interval = AL_ETH_INTR_LOWEST_BYTES,
	},
	{
		.packets_per_interval =  AL_ETH_INTR_LOW_PKTS,
		.bytes_per_interval = AL_ETH_INTR_LOW_BYTES,
	},
	{
		.packets_per_interval = AL_ETH_INTR_MID_PKTS,
		.bytes_per_interval = AL_ETH_INTR_MID_BYTES,
	},
	{
		.packets_per_interval = AL_ETH_INTR_HIGH_PKTS,
		.bytes_per_interval = AL_ETH_INTR_HIGH_BYTES,
	},
	{
		.packets_per_interval = AL_ETH_INTR_HIGHEST_PKTS,
		.bytes_per_interval = AL_ETH_INTR_HIGHEST_BYTES,
	},
};

struct al_mod_eth_epe_p_reg_entry parser_reg_udp_dis_end_of_parse_entry = {
	0x31100, 0x7FF00, 0x8000000D
};
struct al_mod_eth_epe_control_entry parser_control_udp_dis_end_of_parse_entry = {
	{0x2815042, 0x1F42000, 0x2042010, 0x800000, 0x1010000A, 0x40B800}
};

/* ports 1 & 3 are STD */
static const int eth_v4_adv_port_map[4] = {0, 2, 4, 5};

/** Forward declarations */
#ifdef CONFIG_ARCH_ALPINE
static void al_mod_eth_serdes_mode_set(struct al_mod_eth_adapter *adapter);
static void al_mod_eth_serdes_mode_restore(struct al_mod_eth_adapter *adapter);
#endif
static void al_mod_eth_down(struct al_mod_eth_adapter *adapter);
static int al_mod_eth_up(struct al_mod_eth_adapter *adapter);
static void al_mod_eth_ints_and_errors_print(struct al_mod_eth_adapter *adapter);
static int al_mod_eth_mdiobus_hw_init(struct al_mod_eth_adapter *adapter);
static int al_mod_eth_phy_init(struct al_mod_eth_adapter *adapter);

#ifdef CONFIG_ARCH_ALPINE
static int live_update_restore_internal;
static void __iomem *g_rsrvd_mem;

#define AL_ETH_RSRVD_MEM_HDR_MAGIC 0xdeadbaed

enum al_mod_eth_rsrvd_mem_type {
	AL_ETH_RSRVD_MEM_TYPE_GEN
};

#define AL_ETH_RSRVD_MEM_VER_0		0
#define AL_ETH_RSRVD_MEM_VER_CURR	AL_ETH_RSRVD_MEM_VER_0

#define AL_ETH_RSRVD_MEM_HDR_SIZE 128
struct al_mod_eth_rsrvd_mem_hdr {
	struct rsrvd_mem_gen {
		uint32_t magic;
		uint16_t type;
		uint16_t ver;
		uint32_t len; /* total length incl. this header */
		uint32_t crc;
	} hdr;
	uint8_t rsrvd[AL_ETH_RSRVD_MEM_HDR_SIZE -
		sizeof(struct rsrvd_mem_gen)];
};

#define AL_ETH_RSRVD_MEM_SECTION_HDR_MAGIC 0xbeeddeeb

enum al_mod_eth_rsrvd_mem_section_type {
	AL_ETH_RSRVD_MEM_SECTION_TYPE_PORT
};

#define AL_ETH_RSRVD_MEM_SECTION_VER_0		0
#define AL_ETH_RSRVD_MEM_SECTION_VER_CURR	AL_ETH_RSRVD_MEM_SECTION_VER_0

#define AL_ETH_RSRVD_MEM_SECTION_HDR_SIZE 128
struct al_mod_eth_rsrvd_mem_section_hdr {
	struct rsrvd_mem_gen_section_gen {
		uint32_t magic;
		uint16_t type;
		uint16_t ver;
		uint32_t len; /* total length incl. this header */
		uint32_t crc;
	} hdr;
	uint8_t rsrvd[AL_ETH_RSRVD_MEM_SECTION_HDR_SIZE -
		sizeof(struct rsrvd_mem_gen_section_gen)];
};

#define AL_ETH_RSRVD_MEM_SECTION_TYPE_PORT_DATA_SIZE 4096 /* in bytes */
#define AL_ETH_RSRVD_MEM_SECTION_TYPE_PORT_OFFSET(i)			\
	(AL_ETH_RSRVD_MEM_HDR_SIZE +					\
	((i) * (AL_ETH_RSRVD_MEM_SECTION_HDR_SIZE +			\
	AL_ETH_RSRVD_MEM_SECTION_TYPE_PORT_DATA_SIZE)))

#define AL_ETH_RSRVD_MEM_MIN_SIZE					\
	(AL_ETH_RSRVD_MEM_SECTION_TYPE_PORT_OFFSET(AL_ETH_MAX_PORTS_NUM) + \
	 AL_ETH_RSRVD_MEM_SECTION_TYPE_PORT_DATA_SIZE)

/* The following functions assume that the  crc field is zeroed */
uint32_t al_mod_eth_calculate_crc(unsigned char *buff, size_t len)
{
	uint32_t calculated_crc = ~0;

	calculated_crc = crc32_le(calculated_crc, buff, len);

	return calculated_crc;
}

uint32_t al_mod_eth_validate_crc(unsigned char *buff, size_t len,
			     uint32_t expected_crc)
{
	int rc = 0;
	uint32_t calculated_crc = ~0;

	calculated_crc = crc32_le(calculated_crc, buff, len);

	if (expected_crc != calculated_crc)
		rc = -EINVAL;

	return rc;
}
#endif /* CONFIG_ARCH_ALPINE */

#define AL_ETH_RTNL_LOCK_W_FLAGS_CHECK_PERIOD	1 /* in msec */

static inline int al_mod_eth_rtnl_lock_w_flags_check(struct al_mod_eth_adapter *adapter)
{
	while (!rtnl_trylock()) {
		if (adapter->flags & AL_ETH_FLAG_CLOSE_ONGOING)
			return -ESHUTDOWN;
		msleep(AL_ETH_RTNL_LOCK_W_FLAGS_CHECK_PERIOD);
	}

	return 0;
}

struct al_mod_udma *al_mod_eth_udma_get(struct al_mod_eth_adapter *adapter, int tx)
{
	if (tx)
		return &adapter->hal_adapter.tx_udma;
	return &adapter->hal_adapter.rx_udma;
}

static int
al_mod_eth_udma_queue_enable(struct al_mod_eth_adapter *adapter, enum al_mod_udma_type type,
	int qid)
{
	int rc = 0;
	char *name = (type == UDMA_TX) ? "Tx" : "Rx";
	struct al_mod_udma_q_params *q_params;

	if (type == UDMA_TX)
		q_params = &adapter->tx_ring[qid].q_params;
	else
		q_params = &adapter->rx_ring[qid].q_params;

	rc = al_mod_eth_queue_config(&adapter->hal_adapter, type, qid, q_params);
	if (rc < 0) {
		netdev_err(adapter->netdev, "config %s queue %u failed\n", name, qid);
		return rc;
	}
#if 0 /* queue enable not implemented yet */
	rc = al_mod_eth_queue_enable(&adapter->hal_adapter, type, qid);

	if (rc < 0)
		netdev_err(adapter->netdev, "enable %s queue %u failed\n", name,
			   qid);
#endif
	return rc;
}

static int al_mod_eth_udma_queues_enable_all(struct al_mod_eth_adapter *adapter)
{
	int i;
	int rc = 0;

	for (i = 0; i < adapter->num_tx_queues; i++) {
		rc = al_mod_eth_udma_queue_enable(adapter, UDMA_TX, i);
		if (rc != 0) {
			netdev_err(adapter->netdev,
				"%s: Failed al_mod_eth_udma_queue_enable (TX qid %d)", __func__, i);
			return rc;
		}
	}

	for (i = 0; i < adapter->num_rx_queues; i++) {
		rc = al_mod_eth_udma_queue_enable(adapter, UDMA_RX, i);
		if (rc != 0) {
			netdev_err(adapter->netdev,
				"%s: Failed al_mod_eth_udma_queue_enable (RX qid %d)", __func__, i);
			return rc;
		}
	}

	return 0;
}

static int al_mod_eth_udma_queues_reset_all(struct al_mod_eth_adapter *adapter)
{
	int i;

	for (i = 0; i < adapter->num_tx_queues; i++)
		if (adapter->tx_ring[i].dma_q && adapter->tx_ring[i].dma_q->q_regs)
			al_mod_udma_q_reset(adapter->tx_ring[i].dma_q);

	for (i = 0; i < adapter->num_rx_queues; i++)
		if (adapter->rx_ring[i].dma_q && adapter->rx_ring[i].dma_q->q_regs)
			al_mod_udma_q_reset(adapter->rx_ring[i].dma_q);
	return 0;
}

/* init FSM, no tunneling supported yet, if packet is tcp/udp over ipv4/ipv6, use 4 tuple hash */
static void
al_mod_eth_fsm_table_init(struct al_mod_eth_adapter *adapter)
{
	uint32_t val;
	int i;

	for (i = 0; i < AL_ETH_RX_FSM_TABLE_SIZE; i++) {
		uint8_t outer_type = AL_ETH_FSM_ENTRY_OUTER(i);
		switch (outer_type) {
		case AL_ETH_FSM_ENTRY_IPV4_TCP:
		case AL_ETH_FSM_ENTRY_IPV4_UDP:
		case AL_ETH_FSM_ENTRY_IPV6_TCP:
		case AL_ETH_FSM_ENTRY_IPV6_UDP:
			val = AL_ETH_FSM_DATA_OUTER_4_TUPLE | AL_ETH_FSM_DATA_HASH_SEL;
			break;
		case AL_ETH_FSM_ENTRY_IPV6_NO_UDP_TCP:
		case AL_ETH_FSM_ENTRY_IPV4_NO_UDP_TCP:
			val = AL_ETH_FSM_DATA_OUTER_2_TUPLE | AL_ETH_FSM_DATA_HASH_SEL;
			break;
		case AL_ETH_FSM_ENTRY_NOT_IP:
			if (AL_ETH_FSM_ENTRY_TUNNELED(i) && !AL_ETH_FSM_ENTRY_INNER(i)) /*PPPoE*/ {
				val = AL_ETH_FSM_DATA_INNER_4_TUPLE | AL_ETH_FSM_DATA_HASH_SEL;
				break;
			}
		default:
			val = (0 << AL_ETH_FSM_DATA_DEFAULT_Q_SHIFT |
				((1 << adapter->udma_num) << AL_ETH_FSM_DATA_DEFAULT_UDMA_SHIFT));
		}
		al_mod_eth_fsm_table_set(&adapter->hal_adapter, i, val);
	}
}

/**
 * MAC tables
 */

static void al_mod_eth_mac_table_unicast_add(
	struct al_mod_eth_adapter *adapter,
	uint8_t idx,
	uint8_t *addr,
	uint8_t udma_mask)
{
	struct al_mod_eth_fwd_mac_table_entry entry = { { 0 } };

	memcpy(entry.addr, addr, sizeof(entry.addr));

	memset(entry.mask, 0xff, sizeof(entry.mask));
	entry.rx_valid = true;
	entry.tx_valid = false;
	entry.udma_mask = udma_mask;
	entry.filter = false;

	netdev_dbg(adapter->netdev, "%s: [%d]: addr "MAC_ADDR_STR" mask "MAC_ADDR_STR"\n",
		__func__, idx, MAC_ADDR(entry.addr), MAC_ADDR(entry.mask));

	al_mod_eth_fwd_mac_table_set(&adapter->hal_adapter, idx, &entry);
}

static void al_mod_eth_mac_table_broadcast_add(
	struct al_mod_eth_adapter	*adapter,
	uint8_t idx,
	uint8_t udma_mask)
{
	struct al_mod_eth_fwd_mac_table_entry entry = { { 0 } };

	memset(entry.addr, 0xff, sizeof(entry.addr));
	memset(entry.mask, 0xff, sizeof(entry.mask));

	entry.rx_valid = true;
	entry.tx_valid = false;
	entry.udma_mask = udma_mask;
	entry.filter = false;

	netdev_dbg(adapter->netdev, "%s: [%d]: addr "MAC_ADDR_STR" mask "MAC_ADDR_STR"\n",
		__func__, idx, MAC_ADDR(entry.addr), MAC_ADDR(entry.mask));

	al_mod_eth_fwd_mac_table_set(&adapter->hal_adapter, idx, &entry);
}

#ifdef HAVE_SET_RX_MODE
static void al_mod_eth_mac_table_all_multicast_add(
	struct al_mod_eth_adapter *adapter,
	uint8_t idx,
	uint8_t udma_mask)
{
	struct al_mod_eth_fwd_mac_table_entry entry = { { 0 } };

	memset(entry.addr, 0x00, sizeof(entry.addr));
	memset(entry.mask, 0x00, sizeof(entry.mask));
	entry.mask[0] |= BIT(0);
	entry.addr[0] |= BIT(0);

	entry.rx_valid = true;
	entry.tx_valid = false;
	entry.udma_mask = udma_mask;
	entry.filter = false;

	netdev_dbg(adapter->netdev, "%s: [%d]: addr "MAC_ADDR_STR" mask "MAC_ADDR_STR"\n",
		__func__, idx, MAC_ADDR(entry.addr), MAC_ADDR(entry.mask));

	al_mod_eth_fwd_mac_table_set(&adapter->hal_adapter, idx, &entry);
}
#endif

static void al_mod_eth_mac_table_promiscuous_set(
	struct al_mod_eth_adapter *adapter,
	al_mod_bool promiscuous,
	uint8_t udma_mask)
{
	struct al_mod_eth_fwd_mac_table_entry entry = { { 0 } };

	memset(entry.addr, 0x00, sizeof(entry.addr));
	memset(entry.mask, 0x00, sizeof(entry.mask));

	entry.rx_valid = true;
	entry.tx_valid = false;
	entry.udma_mask = (promiscuous) ? udma_mask : 0;
	entry.filter = (promiscuous) ? false : true;

	netdev_dbg(adapter->netdev, "%s: %s promiscuous mode\n",
		__func__, (promiscuous) ? "enter" : "exit");

	al_mod_eth_fwd_mac_table_set(&adapter->hal_adapter,
		 AL_ETH_MAC_TABLE_DROP_IDX,
		 &entry);
}

static void al_mod_eth_mac_table_entry_clear(
	struct al_mod_eth_adapter *adapter,
	uint8_t idx)
{
	struct al_mod_eth_fwd_mac_table_entry entry = { { 0 } };

	netdev_dbg(adapter->netdev, "%s: clear entry %d\n", __func__, idx);

	al_mod_eth_fwd_mac_table_set(&adapter->hal_adapter, idx, &entry);
}

static int al_mod_eth_get_serdes_25g_speed(struct al_mod_eth_adapter *adapter, uint *speed)
{
#ifndef CONFIG_ARCH_ALPINE
	struct al_mod_serdes_grp_obj	*serdes_obj;
	enum al_mod_serdes_group_mode serdes_group_mode;
	uint32_t serdes_25g_gen_ctrl;

	if (unlikely(!adapter->serdes_base)) {
		netdev_err(adapter->netdev, "%s: serdes base isn't initialized\n", __func__);
		return -EINVAL;
	}

	if (unlikely(!adapter->serdes_obj)) {
		netdev_err(adapter->netdev, "%s: Not a valid serdes obj\n", __func__);
		return -EINVAL;
	}

	serdes_obj =  adapter->serdes_obj;

	serdes_25g_gen_ctrl = serdes_obj->serdes_mode_get(adapter->serdes_obj, &serdes_group_mode);

	if (serdes_group_mode == AL_SRDS_CFG_ETH_25G)
		*speed = 25000;
	else if (serdes_group_mode == AL_SRDS_CFG_KR)
		*speed = 10000;
	else
		return -EIO;

	return 0;
#else
	netdev_err(adapter->netdev, "%s: Invalid media type (%d)\n", __func__, adapter->board_type);
	return -EINVAL;
#endif
}

/**
 * @brief OF device id
 */
static const struct of_device_id al_mod_eth_of_table[] =
{
	{ .compatible = "annapurna-labs,al-eth", },
	{}
};

static struct device_node *of_find_child_by_name(struct device_node *node, const char* child_name)
{
	struct device_node *child;
	for_each_child_of_node(node, child) {
		if(!strcmp(child->name, child_name)) {
			return child;
		}
	}
	return NULL;
}

static int al_mod_eth_board_of_led(struct al_mod_eth_adapter *adapter,  struct al_mod_eth_board_params* params, struct device_node *np)
{
	struct device_node *np_leds = NULL;
	#define OF_NODE_NAME_LED_ROOT	"leds"
	#define OF_NODE_NAME_LED_1G	"sfp_1g"
	#define OF_NODE_NAME_LED_10G	"sfp_10g"
	#define OF_PROP_NAME_LED_GPIOS	"gpios"

	if (NULL == adapter || NULL == np || NULL == params) {
		return -EINVAL;
	}

	/** NOTE:
		We can't use of_find_node_by_name as the function interates over ALL nodes (not only children)
		and not every node has "leds" subnode. We want to find the node only in node's subtree.
	*/
	np_leds = of_find_child_by_name(np, OF_NODE_NAME_LED_ROOT);
	if (!np_leds) {
		netdev_dbg(adapter->netdev, "Unable to find matching node (%s)\n", OF_NODE_NAME_LED_ROOT);
		return -EINVAL;
	}

	np = of_find_child_by_name(np_leds, OF_NODE_NAME_LED_1G);
	if (np) {
		adapter->gpio_spd_1g = of_get_named_gpio(np, OF_PROP_NAME_LED_GPIOS, 0);

		of_node_put(np);
	} else {
		adapter->gpio_spd_1g = params->gpio_spd_1g;
	}

	np = of_find_child_by_name(np_leds, OF_NODE_NAME_LED_10G);
	if (np) {
		adapter->gpio_spd_10g = of_get_named_gpio(np, OF_PROP_NAME_LED_GPIOS, 0);

		of_node_put(np);
	} else {
		adapter->gpio_spd_10g = params->gpio_spd_10g;
	}

	if (adapter->gpio_spd_1g <= 0)
		adapter->gpio_spd_1g = GPIO_SPD_NOT_AVAILABLE;
	if (adapter->gpio_spd_10g <= 0)
		adapter->gpio_spd_10g = GPIO_SPD_NOT_AVAILABLE;
	if (adapter->gpio_spd_25g <= 0)
		adapter->gpio_spd_25g = GPIO_SPD_NOT_AVAILABLE;

	of_node_put(np_leds);
	return 0;
}

static int al_mod_eth_board_of_sfp_probing(struct al_mod_eth_adapter *adapter,  struct al_mod_eth_board_params* params, struct device_node *np)
{
	struct device_node *np_10g;
	#define OF_NODE_NAME_10G 	"10g-serial"
	#define OF_PROP_NAME_PROBE_1G	"sfp_probe_1g"
	#define OF_PROP_NAME_PROBE_10G	"sfp_probe_10g"

	if (NULL == adapter || NULL == np || NULL == params) {
		return -EINVAL;
	}

	np_10g = of_find_child_by_name(np, OF_NODE_NAME_10G);
	if (!np_10g) {
		netdev_dbg(adapter->netdev, "Unable to find matching node (%s)\n", OF_NODE_NAME_10G);
		return -EINVAL;
	}

	adapter->sfp_probe_1g = (of_property_match_string(np_10g, OF_PROP_NAME_PROBE_1G, "disabled") < 0);
	adapter->sfp_probe_10g = (of_property_match_string(np_10g, OF_PROP_NAME_PROBE_10G, "disabled") < 0);

	of_node_put(np_10g);
	return 0;
}

static int al_mod_eth_board_of_flow_ctrl(struct al_mod_eth_adapter *adapter, struct device_node *np)
{
	struct device_node *np_flow;
	#define OF_NODE_NAME_FLOW_CTRL                   "flow-ctrl"
	#define OF_PROP_NAME_PROBE_FLOW_CTRL_TX_PAUSE    "txpause"
	#define OF_PROP_NAME_PROBE_FLOW_CTRL_RX_PAUSE    "rxpause"


	if (NULL == adapter || NULL == np)
		return -EINVAL;

	np_flow = of_find_child_by_name(np, OF_NODE_NAME_FLOW_CTRL);
	if (!np_flow) {
		netdev_dbg(adapter->netdev, "Unable to find matching node (%s).", OF_NODE_NAME_FLOW_CTRL);
		return -EINVAL;
	}

	if (of_property_match_string(np_flow, OF_PROP_NAME_PROBE_FLOW_CTRL_TX_PAUSE, "disabled") == 0)
		adapter->link_config.flow_ctrl_supported &= ~AL_ETH_FLOW_CTRL_TX_PAUSE;

	if (of_property_match_string(np_flow, OF_PROP_NAME_PROBE_FLOW_CTRL_RX_PAUSE, "disabled") == 0)
		adapter->link_config.flow_ctrl_supported &= ~AL_ETH_FLOW_CTRL_RX_PAUSE;

	of_node_put(np_flow);
	return 0;
}

static void al_mod_eth_board_params_of_defaults(struct al_mod_eth_adapter *adapter) {

	/* No LED settings by default  */
	adapter->gpio_spd_1g = GPIO_SPD_NOT_AVAILABLE;
	adapter->gpio_spd_10g = GPIO_SPD_NOT_AVAILABLE;

	/* Enable 1G & 10G SFP probing by default */
	adapter->sfp_probe_1g = true;
	adapter->sfp_probe_10g = true;

}

static int al_mod_eth_board_params_of_init(struct al_mod_eth_adapter *adapter, struct al_mod_eth_board_params* params) {

	struct device_node *np_device, *np_port;
	char path[128];

	if (NULL == adapter || NULL == params) {
		return -EINVAL;
	}

	/* Set OF related setting to their default */
	al_mod_eth_board_params_of_defaults(adapter);

	/* Find compatible OF node */
	np_device = of_find_matching_node(NULL, al_mod_eth_of_table);
	if (!np_device) {
		netdev_warn(adapter->netdev, "Unable to find compatible OF node\n");
		return -EINVAL;
	}
	/* Find port's OF node */
	snprintf(path, sizeof(path), "port%d", adapter->port_num);
	/* vv  np_device is going to be consumed by of_find_node_by_name vv */
	np_port = of_find_node_by_name(np_device, path);
	if (!np_port) {
		netdev_dbg(adapter->netdev, "Unable to find port's OF node.");
		return -EINVAL;
	}

	/* LED setup */
	al_mod_eth_board_of_led(adapter, params, np_port);

	/* LM SFP probing */
	al_mod_eth_board_of_sfp_probing(adapter, params, np_port);

	/* Free resources */
	of_node_put(np_port);

	return 0;
}

static int al_mod_eth_board_params_init_nic(struct al_mod_eth_adapter *adapter)
{
	struct al_mod_eth_board_params params = {0};
	int rc;
	enum board_t board_type = adapter->board_type;

	adapter->sfp_detection_needed = false;
	adapter->phy_exist = false;
	adapter->an_en = false;
	adapter->lt_en = false;
	adapter->ref_clk_freq = (board_type == ALPINE_NIC) ? AL_ETH_REF_FREQ_375_MHZ :
			AL_ETH_REF_FREQ_500_MHZ;
	adapter->mdio_freq = AL_ETH_DEFAULT_MDIO_FREQ_KHZ;
	adapter->link_config.active_duplex = (board_type == ALPINE_NIC) ? DUPLEX_HALF : DUPLEX_FULL;
	adapter->link_config.autoneg = AUTONEG_DISABLE;

	switch (board_type) {
	case ALPINE_NIC:
		adapter->mac_mode = AL_ETH_MAC_MODE_10GbE_Serial;
		adapter->link_config.active_speed = 1000;
		return 0;
	case ALPINE_NIC_V2_10:
		adapter->mac_mode = AL_ETH_MAC_MODE_10GbE_Serial;
		adapter->link_config.active_speed = 10000;
		return 0;
	case ALPINE_NIC_V2_25:
	case ALPINE_NIC_V2_25_DUAL:
	case ALPINE_NIC_V3_100:
	case ALPINE_NIC_V3_4x25:
	case ALPINE_NIC_V3_4x25_EX:
		break;
	case ALPINE_NIC_V2_1G_TEST:
	case ALPINE_NIC_V3_1G_TEST:
		adapter->mac_mode = AL_ETH_MAC_MODE_RGMII;
		adapter->link_config.active_speed = SPEED_1000;
		adapter->link_config.force_1000_base_x = false;
		return 0;
	default:
		al_mod_assert(0);
	}

	switch (board_type) {
#ifdef AL_ETH_MAC_HAS_MAC_MODE_ETH_V4
	case ALPINE_NIC_V3_100:
		adapter->mac_mode = AL_ETH_MAC_MODE_CG_100G;
		break;
	case ALPINE_NIC_V3_4x25:
	case ALPINE_NIC_V3_4x25_EX:
		adapter->mac_mode = AL_ETH_MAC_MODE_XLG_LL_25G;
		adapter->serdes_lane = adapter->pf_num;
		break;
#endif
	case ALPINE_NIC_V2_25:
	case ALPINE_NIC_V2_25_DUAL:
		adapter->mac_mode = AL_ETH_MAC_MODE_KR_LL_25G;
		break;
	default:
		al_mod_assert(0);
	}
	dev_info(&adapter->pdev->dev, "%s: mac mode = %d\n", __func__, adapter->mac_mode);

	rc = al_mod_eth_board_params_get_ex(&adapter->hal_adapter, &params);
	if (rc) {
		dev_err(&adapter->pdev->dev, "board info not available\n");
		return -1;
	}
	adapter->phy_if = params.phy_if;
#ifdef AL_ETH_HAS_COMMON_MODE
	adapter->common_mode = params.common_mode;
#endif
	switch (params.media_type) {
	case AL_ETH_BOARD_MEDIA_TYPE_25G_10G_AUTO:
		rc = al_mod_eth_get_serdes_25g_speed(adapter, &adapter->link_config.active_speed);
		if (rc) {
			dev_err(&adapter->pdev->dev, "Invalid speed read from 25G serdes\n");
			return rc;
		}

		break;
	case AL_ETH_BOARD_MEDIA_TYPE_25G:
		adapter->link_config.active_speed = 25000;
		break;
#ifdef AL_ETH_BOARD_MEDIA_TYPE_HAS_100G
	case AL_ETH_BOARD_MEDIA_TYPE_100G:
		adapter->link_config.active_speed = 100000;
		break;
#endif
	case AL_ETH_BOARD_MEDIA_TYPE_10GBASE_SR:
	default:
		adapter->link_config.active_speed = 10000;
		break;
	}

	return 0;
}

static int al_mod_eth_board_params_init_integrated(struct al_mod_eth_adapter *adapter)
{
	struct al_mod_eth_board_params params;
	int rc;

	adapter->auto_speed = false;

	rc = al_mod_eth_board_params_get_ex(&adapter->hal_adapter, &params);
	if (rc) {
		dev_err(&adapter->pdev->dev, "board info not available\n");
		return -1;
	}
#ifdef AL_ETH_HAS_COMMON_MODE
		adapter->common_mode = params.common_mode;
#endif
	adapter->phy_exist = params.phy_exist == AL_TRUE;
	adapter->phy_addr = params.phy_mdio_addr;
	adapter->an_en = params.autoneg_enable;
	adapter->lt_en = params.kr_lt_enable;
	adapter->serdes_grp = params.serdes_grp;
	adapter->serdes_lane = params.serdes_lane;
	adapter->sfp_detection_needed = params.sfp_plus_module_exist;
	adapter->i2c_adapter_id = params.i2c_adapter_id;
	adapter->ref_clk_freq = params.ref_clk_freq;
	adapter->dont_override_serdes = params.dont_override_serdes;
	adapter->link_config.active_duplex = !params.half_duplex;
	adapter->link_config.autoneg = (adapter->phy_exist) ?
		(params.an_mode == AL_ETH_BOARD_AUTONEG_IN_BAND) :
		(!params.an_disable);
	adapter->link_config.force_1000_base_x = params.force_1000_base_x;
	adapter->retimer.exist = params.retimer_exist;
	adapter->retimer.type = params.retimer_type;
	adapter->retimer.bus_id = params.retimer_bus_id;
	adapter->retimer.i2c_addr = params.retimer_i2c_addr;
	adapter->retimer.channel = params.retimer_channel;
	adapter->retimer.tx_channel = params.retimer_tx_channel;
	adapter->phy_if = params.phy_if;
	adapter->kr_fec_enable = params.kr_fec_enable;
	adapter->auto_fec_enable = params.auto_fec_enable;
	adapter->gpio_spd_1g = params.gpio_spd_1g;
	adapter->gpio_spd_10g = params.gpio_spd_10g;
	adapter->gpio_spd_25g = params.gpio_spd_25g;
	adapter->gpio_sfp_present = params.gpio_sfp_present;

	/* OF init */
	al_mod_eth_board_params_of_init(adapter, &params);

	switch (params.speed) {
	default:
		dev_warn(&adapter->pdev->dev,
			"%s: invalid speed (%d)\n", __func__,
			params.speed);
	case AL_ETH_BOARD_1G_SPEED_1000M:
		adapter->link_config.active_speed = 1000;
		break;
	case AL_ETH_BOARD_1G_SPEED_100M:
		adapter->link_config.active_speed = 100;
		break;
	case AL_ETH_BOARD_1G_SPEED_10M:
		adapter->link_config.active_speed = 10;
		break;
	}

	switch (params.mdio_freq) {
	default:
		dev_warn(&adapter->pdev->dev,
		"%s: invalid mdio freq (%d)\n", __func__,
		params.mdio_freq);
	case AL_ETH_BOARD_MDIO_FREQ_2_5_MHZ:
		adapter->mdio_freq = 2500;
		break;
	case AL_ETH_BOARD_MDIO_FREQ_1_MHZ:
		adapter->mdio_freq = 1000;
		break;
	}

	switch (params.media_type) {
	case AL_ETH_BOARD_MEDIA_TYPE_RGMII:
		if (params.sfp_plus_module_exist == AL_TRUE)
			/* Backward compatibility */
			adapter->mac_mode = AL_ETH_MAC_MODE_SGMII;
		else
			adapter->mac_mode = AL_ETH_MAC_MODE_RGMII;

		adapter->use_lm = false;
		break;
	case AL_ETH_BOARD_MEDIA_TYPE_SGMII:
		adapter->mac_mode = AL_ETH_MAC_MODE_SGMII;
		adapter->max_speed = AL_ETH_LM_MAX_SPEED_1G;
		if (adapter->al_mod_chip_id <= PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V2)
			adapter->use_lm = true;
		else
			adapter->use_lm = false;
		break;
	case AL_ETH_BOARD_MEDIA_TYPE_SGMII_2_5G:
		adapter->mac_mode = AL_ETH_MAC_MODE_SGMII_2_5G;
		adapter->use_lm = false;
		break;
	case AL_ETH_BOARD_MEDIA_TYPE_10GBASE_SR:
		adapter->mac_mode = AL_ETH_MAC_MODE_10GbE_Serial;
		adapter->max_speed = AL_ETH_LM_MAX_SPEED_10G;
		adapter->use_lm = true;
		break;
	case AL_ETH_BOARD_MEDIA_TYPE_AUTO_DETECT:
		adapter->sfp_detection_needed = AL_TRUE;
		adapter->max_speed = AL_ETH_LM_MAX_SPEED_10G;
		adapter->auto_speed = false;
		adapter->use_lm = true;
		break;
	case AL_ETH_BOARD_MEDIA_TYPE_AUTO_DETECT_AUTO_SPEED:
		adapter->sfp_detection_needed = AL_TRUE;
		adapter->max_speed = AL_ETH_LM_MAX_SPEED_10G;
		adapter->auto_speed = true;
		adapter->mac_mode_set = false;
		adapter->use_lm = true;
		break;
	case AL_ETH_BOARD_MEDIA_TYPE_NBASE_T:
		adapter->mac_mode = AL_ETH_MAC_MODE_10GbE_Serial;
		adapter->max_speed = AL_ETH_LM_MAX_SPEED_10G;
		adapter->phy_fixup_needed = true;
		break;
	case AL_ETH_BOARD_MEDIA_TYPE_25G:
		adapter->sfp_detection_needed = AL_TRUE;
		adapter->mac_mode = (adapter->rev_id >= AL_ETH_REV_ID_4) ?
				AL_ETH_MAC_MODE_XLG_LL_25G : AL_ETH_MAC_MODE_KR_LL_25G;
		adapter->use_lm = true;
		adapter->max_speed = AL_ETH_LM_MAX_SPEED_25G;
		break;
	case AL_ETH_BOARD_MEDIA_TYPE_25G_10G_AUTO:
		adapter->sfp_detection_needed = AL_TRUE;
		adapter->mac_mode = AL_ETH_MAC_MODE_KR_LL_25G;
		adapter->use_lm = true;
		adapter->max_speed = AL_ETH_LM_MAX_SPEED_25G;
		adapter->speed_detection = true;
		adapter->auto_speed = true;
		break;
	default:
		dev_err(&adapter->pdev->dev,
			"%s: unsupported media type %d\n",
			__func__, params.media_type);
		return -1;
	}
	dev_info(&adapter->pdev->dev,
		"Board info: phy exist %s. phy addr %d. mdio freq %u Khz. SFP connected %s. media %d\n",
		params.phy_exist == AL_TRUE ? "Yes" : "No",
		params.phy_mdio_addr,
		adapter->mdio_freq,
		params.sfp_plus_module_exist == AL_TRUE ? "Yes" : "No",
		params.media_type);

		return 0;
}

static int al_mod_eth_board_params_init(struct al_mod_eth_adapter *adapter)
{
	int rc;

	if (IS_NIC(adapter->board_type)) {
		rc = al_mod_eth_board_params_init_nic(adapter);
	} else if (adapter->board_type == ALPINE_INTEGRATED) {
		rc = al_mod_eth_board_params_init_integrated(adapter);
	} else {
			dev_err(&adapter->pdev->dev,
				"%s: unsupported board type %d\n",
				__func__, adapter->board_type);
			return -EPERM;
	}

	al_mod_eth_mac_addr_read(adapter->ec_base, 0, adapter->mac_addr);

	return rc;
}

static inline void
al_mod_eth_flow_ctrl_init(struct al_mod_eth_adapter *adapter)
{
	uint8_t default_flow_ctrl;
	struct device_node *np_device, *np_port;
	char path[128];

	default_flow_ctrl = AL_ETH_FLOW_CTRL_TX_PAUSE;
	default_flow_ctrl |= AL_ETH_FLOW_CTRL_RX_PAUSE;

	adapter->link_config.flow_ctrl_supported = default_flow_ctrl;

	/* override the default flow control if the flow-ctrl node is exist in dts */
	/* Find compatible OF node */
	np_device = of_find_matching_node(NULL, al_mod_eth_of_table);
	if (!np_device) {
		netdev_info(adapter->netdev, "%s: Unable to find compatible OF node\n", __func__);
		return;
	}
	/* Find port's OF node */
	snprintf(path, sizeof(path), "port%d", adapter->port_num);
	/* vv  np_device is going to be consumed by of_find_node_by_name vv */
	np_port = of_find_node_by_name(np_device, path);
	if (!np_port) {
		netdev_info(adapter->netdev, "%s: Unable to find port's OF node.", __func__);
		return;
	}
	al_mod_eth_board_of_flow_ctrl(adapter, np_port);

	/* Free resources */
	of_node_put(np_port);
}

static int
al_mod_eth_flow_ctrl_config(struct al_mod_eth_adapter *adapter)
{
	struct al_mod_eth_flow_control_params *flow_ctrl_params;
	uint8_t active = adapter->link_config.flow_ctrl_active;
	int i;

	flow_ctrl_params = &adapter->flow_ctrl_params;
	memset(flow_ctrl_params, 0, sizeof(*flow_ctrl_params));

	flow_ctrl_params->type = AL_ETH_FLOW_CONTROL_TYPE_LINK_PAUSE;
	flow_ctrl_params->obay_enable =
		((active & AL_ETH_FLOW_CTRL_RX_PAUSE) != 0);
	flow_ctrl_params->gen_enable =
		((active & AL_ETH_FLOW_CTRL_TX_PAUSE) != 0);

	flow_ctrl_params->rx_fifo_th_high = AL_ETH_FLOW_CTRL_RX_FIFO_TH_HIGH;
	flow_ctrl_params->rx_fifo_th_low = AL_ETH_FLOW_CTRL_RX_FIFO_TH_LOW;
	flow_ctrl_params->quanta = AL_ETH_FLOW_CTRL_QUANTA;
	flow_ctrl_params->quanta_th = AL_ETH_FLOW_CTRL_QUANTA_TH;

	/*
	 * For link pause - use only priority 0 & specify queue bitmask
	 * For PFC - map priority to queue index, queue id = priority/2
	 */
	if (flow_ctrl_params->type == AL_ETH_FLOW_CONTROL_TYPE_LINK_PAUSE)
		flow_ctrl_params->prio_q_map[adapter->udma_num][0] =
			(1 << adapter->num_tx_queues) - 1;
	else if (flow_ctrl_params->type == AL_ETH_FLOW_CONTROL_TYPE_PFC)
		for (i = 0; i < AL_ETH_FWD_PRIO_TABLE_NUM; i++)
			flow_ctrl_params->prio_q_map[adapter->udma_num][i] = 1 << (i >> 1);

	al_mod_eth_flow_control_config(&adapter->hal_adapter, flow_ctrl_params);

	return 0;
}

static void
al_mod_eth_flow_ctrl_enable(struct al_mod_eth_adapter *adapter)
{
	/* change the active configuration to the default / force by ethtool
	 * and call to configure */
	adapter->link_config.flow_ctrl_active =
		adapter->link_config.flow_ctrl_supported;

	al_mod_eth_flow_ctrl_config(adapter);
}

static void
al_mod_eth_flow_ctrl_disable(struct al_mod_eth_adapter *adapter)
{
	adapter->link_config.flow_ctrl_active = 0;
	al_mod_eth_flow_ctrl_config(adapter);
}

#ifdef CONFIG_PHYLIB
static uint8_t al_mod_eth_flow_ctrl_mutual_cap_get(struct al_mod_eth_adapter *adapter)
{
	struct phy_device *phydev = adapter->phydev;
	struct al_mod_eth_link_config *link_config = &adapter->link_config;
	uint8_t peer_flow_ctrl = AL_ETH_FLOW_CTRL_AUTONEG;
	uint8_t new_flow_ctrl = AL_ETH_FLOW_CTRL_AUTONEG;

	if (phydev->pause)
		peer_flow_ctrl |= (AL_ETH_FLOW_CTRL_TX_PAUSE | AL_ETH_FLOW_CTRL_RX_PAUSE);
	if (phydev->asym_pause)
		peer_flow_ctrl ^= (AL_ETH_FLOW_CTRL_RX_PAUSE);

	/*
	 * in autoneg mode, supported flow ctrl is also
	 * the current advertising
	 */
	if ((peer_flow_ctrl & AL_ETH_FLOW_CTRL_TX_PAUSE) ==
	    (link_config->flow_ctrl_supported & AL_ETH_FLOW_CTRL_TX_PAUSE))
		new_flow_ctrl |= AL_ETH_FLOW_CTRL_TX_PAUSE;
	if ((peer_flow_ctrl & AL_ETH_FLOW_CTRL_RX_PAUSE) ==
		(link_config->flow_ctrl_supported & AL_ETH_FLOW_CTRL_RX_PAUSE))
		new_flow_ctrl |= AL_ETH_FLOW_CTRL_RX_PAUSE;

	return new_flow_ctrl;
}
#endif /* CONFIG_PHYLIB */

#ifndef CONFIG_ARCH_ALPINE
/** For now only for 100G */
static int al_mod_eth_tam_setup(struct al_mod_eth_adapter *adapter)
{
	struct al_mod_tam_cfg_params params;
	struct al_mod_tam_pcie_pf pcie_pf_1[] = {
			{.pf_num = 0},
	};
	struct al_mod_tam_pcie_pf pcie_pf_4[] = {
			{.pf_num = 0},
			{.pf_num = 1},
			{.pf_num = 2},
			{.pf_num = 3},
	};
	int port_num = adapter->pcie_ep_num;
	int rc;

	params.port_num = port_num;
	if (adapter->board_type == ALPINE_NIC_V3_100 ||
	    adapter->board_type == ALPINE_NIC_V3_4x25_EX) {
		params.pcie_pfs = pcie_pf_1;
		params.pcie_pfs_num = AL_ARR_SIZE(pcie_pf_1);
	} else if (adapter->board_type == ALPINE_NIC_V3_4x25) {
		params.pcie_pfs = pcie_pf_4;
		params.pcie_pfs_num = AL_ARR_SIZE(pcie_pf_4);
	}
	params.cfg_mode = AL_TAM_CFG_MODE_ALPINE_V3_DONGLE_LEGACY;
	params.pcie_port_grp = AL_TAM_PCIE_PORT_GRP_TYPE_A_0_2;
	params.pcie_pf_grp = AL_TAM_PCIE_PF_GRP_0_1_2_3;
	params.udma_queues_num = 0; /** configure pcie only */

	params.pbs_regfile_base = adapter->pbs_regfile_base;
	params.sb_pcie_base = adapter->internal_sb_base +
			al_mod_iomap_offset_idx_sub_idx_get(
			adapter->al_mod_chip_id, AL_IOMAP_SB_PCIE_OFFSET, adapter->pcie_ep_num ,0);

	rc = al_mod_tam_cfg_set(&params);
	if (rc != 0)
		netdev_err(adapter->netdev, "%s failed (%d)\n", __func__, rc);

	return rc;
}
#endif

/**
 * target ID's to force target traffic to a specific PCIE port
 * Values taken from IO fabric spec
 */
#define AL_ETH_TGTID_PCIE_EP_0	0x8000
#define AL_ETH_TGTID_PCIE_EP_1	0xA000
#define AL_ETH_TGTID_PCIE_EP_2	0xC000
#define AL_ETH_TGTID_PCIE_EP_3	0xE000

static int al_mod_eth_tgtid_calc(struct al_mod_eth_adapter *adapter)
{
	int tgtid = 0;

	if (IS_V1_NIC(adapter->board_type)) {
		tgtid = 0x100;
	} else if (IS_V2_NIC(adapter->board_type)) {
		if ((adapter->board_type == ALPINE_NIC_V2_25_DUAL) &&
				(PCI_FUNC(adapter->pdev->devfn) == 1))
			tgtid = 0x8001;
		else if (adapter->board_type == ALPINE_NIC_V2_1G_TEST)
					tgtid = 0x8000 | PCI_FUNC(adapter->pdev->devfn);
		else
			tgtid = 0x8000;
	} else if (IS_V3_NIC(adapter->board_type)) {
		static int target_id[4] = {
			AL_ETH_TGTID_PCIE_EP_0,
			AL_ETH_TGTID_PCIE_EP_1,
			AL_ETH_TGTID_PCIE_EP_2,
			AL_ETH_TGTID_PCIE_EP_3,
		};

		/** Eth rev id 4 */
		if ((adapter->board_type == ALPINE_NIC_V3_4x25) ||
		    (adapter->board_type == ALPINE_NIC_V3_4x25_EX) ||
		    (adapter->board_type == ALPINE_NIC_V3_1G_TEST)) {
			/** Assumes TAM mode used is AL_TAM_CFG_MODE_ALPINE_V3_DONGLE_LEGACY */
			unsigned int pf_num = adapter->pf_num;
			tgtid = target_id[adapter->pcie_ep_num] + pf_num;
		} else {
			tgtid = target_id[adapter->pcie_ep_num];
		}
	}
#ifdef CONFIG_AL_ETH_SRIOV
	if (!(adapter->pdev->is_physfn)) {

		/** SRIOV on ETHv4 not yet supported */
		al_mod_assert(adapter->rev_id < AL_ETH_REV_ID_4);

		netdev_err(adapter->netdev, "setting tgtid to 0x1\n");
		if (adapter->rev_id < AL_ETH_REV_ID_3)
			tgtid = 0x101;
		else
			tgtid = 0x8010;
	}
#endif
	dev_info(&adapter->pdev->dev, "%s: tgtid = 0x%x\n", __func__, tgtid);
	return tgtid;
}

static int al_mod_eth_read_pci_config(void *handle, int where, uint32_t *val)
{
	/* handle is a pointer to the pci_dev */
	pci_read_config_dword((struct pci_dev *)handle, where, val);
	return 0;
}

static int al_mod_eth_write_pci_config(void *handle, int where, uint32_t val)
{
	/* handle is a pointer to the pci_dev */
	pci_write_config_dword((struct pci_dev *)handle, where, val);
	return 0;
}

static int al_mod_eth_read_pcie_config_nic(void *handle, int where, uint32_t *val)
{
	struct al_mod_eth_adapter *adapter = (struct al_mod_eth_adapter *)handle;

	al_mod_assert(handle);

	*val = readl(adapter->internal_pcie_base + where);

	return 0;
}

static int al_mod_eth_write_pcie_config_nic(void *handle, int where, uint32_t val)
{
	struct al_mod_eth_adapter *adapter = (struct al_mod_eth_adapter *)handle;

	al_mod_assert(handle);

	writel(val, adapter->internal_pcie_base + where);

	return 0;
}

#ifdef AL_ETH_ADAPTER_PARAMS_HAS_UNIT_ADAPTER
static int al_mod_eth_function_reset(struct al_mod_eth_adapter *adapter);

static int al_mod_eth_pcie_flr(void *handle)
{
	struct al_mod_eth_adapter *adapter = pci_get_drvdata((struct pci_dev *)handle);

	return al_mod_eth_function_reset(adapter);
}

static int al_mod_eth_pcie_flr_nic(void *handle)
{
	return al_mod_eth_function_reset((struct al_mod_eth_adapter *)handle);
}
#endif /* AL_ETH_ADAPTER_PARAMS_HAS_UNIT_ADAPTER */

static void al_mod_eth_hw_generic_crc_init(struct al_mod_eth_adapter *adapter)
{
	al_mod_eth_tx_crc_chksum_replace_cmd_init(&adapter->hal_adapter);
	al_mod_eth_tx_generic_crc_table_init(&adapter->hal_adapter, 0);
	al_mod_eth_tx_protocol_detect_table_init(&adapter->hal_adapter);
	al_mod_eth_rx_generic_crc_table_init(&adapter->hal_adapter, 0);
	al_mod_eth_rx_protocol_detect_table_init(&adapter->hal_adapter);
}

static void al_mod_eth_tx_icrc_hw_offload_enable(struct al_mod_eth_adapter *adapter,
					     int entry_idx)
{
	struct al_mod_eth_tx_gpd_cam_entry icrc_tx_gpd_cam_entry = {
		/* USR_DEF over IPV4 and udp */
		8,	AL_ETH_PROTO_ID_USR_DEF,		0,		0,		1,
		0x1f,		0x1f,		0x0,		0x0
	};
	struct al_mod_eth_tx_gcp_table_entry icrc_tx_gcp_table_entry = {
		/* USR_DEF over IPV4 and udp */
		0,		1,		1,		0,		1,
		0,		4,		0,		0,		1,
		0,		0,		0,		0,		0,
		0,		0,		{0x3000cf00,	0x00000000,	0x00000000,
		0x00000000,	0x00000000,	0x00000000},	0xffffffff,	0x0,
		0
	};
	struct al_mod_eth_tx_crc_chksum_replace_cmd_for_protocol_num_entry
		icrc_tx_crc_chksum_replace_cmd = {
		/* USR_DEF over IPV4 and udp */
		0, 0, 1, 1,		0, 0, 0, 0,		0, 1, 0, 1
	};

	al_mod_assert(adapter->rev_id == AL_ETH_REV_ID_3);

	/* Set the new entry */
	al_mod_eth_tx_generic_crc_table_entry_set(&adapter->hal_adapter,
					      0, entry_idx,
					      &icrc_tx_gcp_table_entry);
	al_mod_eth_tx_crc_chksum_replace_cmd_entry_set(&adapter->hal_adapter,
						   entry_idx,
						   &icrc_tx_crc_chksum_replace_cmd);
	al_mod_eth_tx_crce_pub_crc_cks_ins_set(&adapter->hal_adapter, AL_TRUE, AL_FALSE);
	al_mod_eth_tx_protocol_detect_table_entry_set(&adapter->hal_adapter,
						  entry_idx,
						  &icrc_tx_gpd_cam_entry);

	netdev_info(adapter->netdev, "Configured iCRC TX HW offload in entry_idx %u (protocol_idx:0x%x)\n",
		    entry_idx, AL_ETH_PROTO_ID_USR_DEF);
}

static int al_mod_eth_hw_init_adapter(struct al_mod_eth_adapter *adapter)
{
	struct al_mod_eth_adapter_params *params = &adapter->eth_hal_params;
	struct al_mod_udma_gen_tgtid_conf	conf;
	struct al_mod_udma_gen_tgtid_msix_conf msix_conf;
	void __iomem *iofic_regs_base_tx_udma;
	void __iomem *iofic_regs_base_rx_udma;

	int rc;
	int i;
	int tgtid;

#ifdef AL_ETH_ADAPTER_PARAMS_HAS_UNIT_ADAPTER
	params->skip_adapter_init = AL_FALSE;
	params->func_num = adapter->udma_num;
	params->unit_adapter = &adapter->unit_adapter;
	params->force_adapter_init = IS_NIC(adapter->board_type);

	if (IS_NIC(adapter->board_type))
		rc = al_mod_unit_adapter_handle_init(&adapter->unit_adapter,
			AL_UNIT_ADAPTER_TYPE_ETH, NULL,
			al_mod_eth_read_pcie_config_nic, al_mod_eth_write_pcie_config_nic,
			al_mod_eth_pcie_flr_nic, (void *)adapter);
	else
		rc = al_mod_unit_adapter_handle_init(&adapter->unit_adapter,
			AL_UNIT_ADAPTER_TYPE_ETH, NULL,
			al_mod_eth_read_pci_config, al_mod_eth_write_pci_config,
			al_mod_eth_pcie_flr, adapter->pdev);

	if (rc) {
		netdev_err(adapter->netdev,
			   "%s failed at al_mod_unit_adapter_handle_init (%d)!\n",
			   __func__,
			   rc);
		return rc;
	}
#endif

	params->rev_id = adapter->rev_id;
	params->dev_id = adapter->dev_id;
	params->udma_id = adapter->udma_num;
	params->enable_rx_parser = 1; /* enable rx epe parser*/
	params->udma_regs_base = adapter->udma_base; /* UDMA register base address */
	params->ec_regs_base = adapter->ec_base; /* Ethernet controller registers base address */
	params->mac_regs_base = adapter->mac_base; /* Ethernet MAC registers base address */
	params->name = adapter->name;
	params->serdes_lane = adapter->serdes_lane;
	if (IS_NIC(adapter->board_type)) {
		params->force_ec_init = AL_TRUE;
		if (!EC_ERRS_COLLECTED_BY_INTR(adapter))
			params->app_int_unmask_bypass = AL_TRUE;
	}
#ifdef AL_ETH_HAS_COMMON_MODE
	if (IS_ETH_V4_ADV(adapter->rev_id, adapter->dev_id)) {
		params->common_mode = adapter->common_mode;
		params->eth_common_regs_base = adapter->eth_common_base;
#if defined(CONFIG_ARCH_ALPINE)
		params->mac_common_regs =
			al_mod_eth_common_resource_get(AL_ETH_COMMON_RESOURCE_TYPE_SHARED_MAC);
#endif
	}
#endif

	rc = al_mod_eth_adapter_init(&adapter->hal_adapter, params);
	if (rc) {
		netdev_err(adapter->netdev, "%s failed at al_mod_eth_adapter_init (%d)!\n", __func__, rc);
		return rc;
	}

	/* some services (e.g. err_events) rely on valid mac_mode, make sure it
	 * is set correctly */
	al_mod_eth_mac_mode_set(&adapter->hal_adapter, adapter->mac_mode);

	if (adapter->rev_id == AL_ETH_REV_ID_3)
		al_mod_eth_hw_generic_crc_init(adapter);
	else if (adapter->rev_id >= AL_ETH_REV_ID_4) {
		struct al_mod_eth_rfw_handle_init_params rx_fwd_handle_init_params = {0};
		rx_fwd_handle_init_params.cpu = 0;
		rx_fwd_handle_init_params.ec_regs_base = (void __iomem *)adapter->ec_base;
		al_mod_eth_rfw_handle_init(&adapter->rfw_handle, &rx_fwd_handle_init_params);
	}

	if (IS_NIC(adapter->board_type)) {
		/* in pcie NIC mode, force eth UDMA to access PCIE0 using the tgtid */
		if (adapter->rev_id >= AL_ETH_REV_ID_1) {
			tgtid = al_mod_eth_tgtid_calc(adapter);

#ifndef CONFIG_ARCH_ALPINE
			if ((IS_ETH_V4_ADV(adapter->rev_id, adapter->dev_id)))
				if (adapter->pf_num == 0)
					al_mod_eth_tam_setup(adapter);
#endif
			for (i = 0; i < adapter->num_tx_queues; i++) {
				conf.tx_q_conf[i].queue_en = AL_TRUE;
				conf.tx_q_conf[i].desc_en = AL_FALSE;
				conf.tx_q_conf[i].tgtid = tgtid; /* for access from PCIE0 */
				conf.rx_q_conf[i].queue_en = AL_TRUE;
				conf.rx_q_conf[i].desc_en = AL_FALSE;
				conf.rx_q_conf[i].tgtid = tgtid; /* for access from PCIE0 */
			}
			al_mod_udma_gen_tgtid_conf_set_adv(&adapter->hal_adapter.tx_udma,
				&conf);

			/* force MSIX to access PCIE 0 using the tgtid */
			iofic_regs_base_tx_udma = al_mod_udma_iofic_reg_base_get_adv(
				&(adapter->hal_adapter.tx_udma), AL_UDMA_IOFIC_LEVEL_PRIMARY);
			iofic_regs_base_rx_udma = al_mod_udma_iofic_reg_base_get_adv(
				&(adapter->hal_adapter.rx_udma), AL_UDMA_IOFIC_LEVEL_PRIMARY);

			msix_conf.access_en = AL_TRUE;
			msix_conf.sel = AL_TRUE;
			al_mod_udma_gen_tgtid_msix_conf_set_adv(&adapter->hal_adapter.tx_udma,
				&msix_conf);

			/* in NIC mode we assume working in irq mode
			 * AL_IOFIC_MODE_MSIX_PER_Q, so we configure group D
			 * summary bit of group A and all queue bits of groups
			 * B and C.
			 * No need to configure group D here as it doesn't
			 * trigger interrupts directly, only via group A */
			al_mod_iofic_msix_tgtid_attributes_config(
				iofic_regs_base_tx_udma,
				AL_INT_GROUP_A, 2, tgtid, AL_TRUE);
			for (i = 0; i < adapter->num_tx_queues; i++) {
				al_mod_iofic_msix_tgtid_attributes_config(
					iofic_regs_base_tx_udma,
					AL_INT_GROUP_B, i, tgtid, AL_TRUE);
				al_mod_iofic_msix_tgtid_attributes_config(
					iofic_regs_base_rx_udma,
					AL_INT_GROUP_C, i, tgtid, AL_TRUE);
			}
		}
	}
	return rc;
}

static bool al_mod_eth_flow_steer_user_def_exists(struct al_mod_eth_adapter *adapter)
{
	int i;

	for (i = 0; i < AL_ETH_EPE_ENTRIES_FREE_NUM; i++) {
		if (adapter->sw_flow_steer_rule[i].valid)
			return AL_TRUE;
	}

	return AL_FALSE;
}

/* This function assumes that after a BACKUP command, the following call is a RESTORE
 * (and vice versa). If the function is invoked more then once with the same command,
 * only the first invocation will actually backup (or restore). The main usecase is for backing up
 * the necessary HW entries before an FLR, and then restoring it (which is done in al_mod_eth_hw_init().
 * This behavior handles the case where al_mod_eth_function_reset() is invoked twice without calling
 * al_mod_eth_hw_init() in between, which would end in data loss */
static void al_mod_eth_user_def_rx_icrc_backup(struct al_mod_eth_adapter *adapter,
					   enum al_mod_eth_user_def_backup_command command)
{
	int i;

	al_mod_assert(IS_ETH_V3_ADV(adapter->rev_id, adapter->dev_id));

	if (command == AL_ETH_USER_DEF_BACKUP) {
		if ((adapter->user_def_rx_icrc_restored == AL_TRUE) &&
			(al_mod_eth_flow_steer_user_def_exists(adapter))) {
			for (i = 0; i < AL_ETH_RX_GPD_ENTRIES_FREE_NUM; i++) {
				al_mod_eth_rx_protocol_detect_table_entry_get(&adapter->hal_adapter,
							(i + AL_ETH_RX_PROTOCOL_DETECT_ENTRIES_NUM),
							&adapter->user_def_rx_icrc_gpd_entry[i]);
				al_mod_eth_rx_generic_crc_table_entry_get(&adapter->hal_adapter,
							(i + AL_ETH_RX_PROTOCOL_DETECT_ENTRIES_NUM),
							&adapter->user_def_rx_icrc_gcp_entry[i]);
			}

			adapter->user_def_rx_icrc_restored = AL_FALSE;
		}
	} else if (command == AL_ETH_USER_DEF_RESTORE) {
		if ((adapter->user_def_rx_icrc_restored == AL_FALSE) &&
			(al_mod_eth_flow_steer_user_def_exists(adapter))) {
			for (i = 0; i < AL_ETH_RX_GPD_ENTRIES_FREE_NUM; i++) {
						al_mod_eth_rx_protocol_detect_table_entry_set(
							&adapter->hal_adapter,
							(i + AL_ETH_RX_PROTOCOL_DETECT_ENTRIES_NUM),
							&adapter->user_def_rx_icrc_gpd_entry[i]);
						al_mod_eth_rx_generic_crc_table_entry_set(
							&adapter->hal_adapter,
							0,
							(i + AL_ETH_RX_PROTOCOL_DETECT_ENTRIES_NUM),
							&adapter->user_def_rx_icrc_gcp_entry[i]);
			}

			adapter->user_def_rx_icrc_restored = AL_TRUE;
		}
	} else {
		netdev_err(adapter->netdev, "%s: Unknown command! (%d)\n", __func__, command);
	}
}

static int al_mod_eth_hw_init(struct al_mod_eth_adapter *adapter)
{
	int rc;
	netdev_dbg(adapter->netdev, "Init adapter\n");
	rc = al_mod_eth_hw_init_adapter(adapter);
	if (rc)
		return rc;

	if (adapter->rev_id == AL_ETH_REV_ID_3) {
		al_mod_eth_tx_icrc_hw_offload_enable(adapter, AL_ETH_TX_GENERIC_CRC_ENTRIES_NUM);
		al_mod_eth_user_def_rx_icrc_backup(adapter, AL_ETH_USER_DEF_RESTORE);
	}

	/**
	 * In NIC mode, don't do mac_config for 25g ports to prevent reset of FEC settings
	 * since the FW does mac_config once on init. For 1g ports in NIC mode, the FW
	 * doesn't do mac_config, so the driver needs to do it
	 * this function must be called after FLR !!
	 **/
	if ((adapter->board_type == ALPINE_NIC_V2_1G_TEST) ||
	    (adapter->board_type == ALPINE_NIC_V3_1G_TEST) ||
	    !IS_NIC(adapter->board_type)) {
		if (adapter->mac_started) {
			netdev_dbg(adapter->netdev, "mac mode set\n");
			rc = al_mod_eth_mac_mode_set(&adapter->hal_adapter,
						 adapter->mac_mode);
		} else {
			netdev_dbg(adapter->netdev, "mac config\n");
			rc = al_mod_eth_mac_config(&adapter->hal_adapter,
					       adapter->mac_mode);
		}
		if (rc < 0) {
			netdev_err(adapter->netdev, "%s failed to configure mac!\n", __func__);
			return rc;
		}
	} else {
		al_mod_eth_mac_mode_set(&adapter->hal_adapter, adapter->mac_mode);
	}

	if (((adapter->mac_mode == AL_ETH_MAC_MODE_SGMII) ||
	    (adapter->mac_mode == AL_ETH_MAC_MODE_RGMII)) &&
	    adapter->phy_exist == AL_FALSE) {
		netdev_err(adapter->netdev, "link config (0x%x)\n", adapter->mac_mode);

		rc = al_mod_eth_mac_link_config(&adapter->hal_adapter,
			adapter->link_config.force_1000_base_x,
			adapter->link_config.autoneg,
			adapter->link_config.active_speed,
			adapter->link_config.active_duplex);
		if (rc) {
			netdev_err(adapter->netdev,
				"%s failed to configure link parameters!\n", __func__);
			return rc;
		}
	}

#ifndef AL_ETH_PLAT_EMU
	if (adapter->mdio_bus && adapter->phy_exist) {
		/* Reinit MDIO BUS - al_mod_eth_mac_config overwrites the MDIO bus settings */
		rc = al_mod_eth_mdiobus_hw_init(adapter);
		if(rc) {
			netdev_err(adapter->netdev, "%s: al_mod_eth_mdiobus_hw_init failed\n", __func__);
			return rc;
		}
	}

	netdev_dbg(adapter->netdev, "flow ctrl config\n");
	al_mod_eth_flow_ctrl_init(adapter);
#endif /* AL_ETH_PLAT_EMU */
	return rc;
}

static int al_mod_eth_mac_tx_stop(struct al_mod_eth_adapter *adapter)
{
	int rc;

	rc = al_mod_eth_mac_tx_flush_config(&adapter->hal_adapter, false);
	if (rc) {
		netdev_err(adapter->netdev,
			   "Failed to disable MAC TX flush (rc = %d)", rc);
	} else {
		netdev_dbg(adapter->netdev, "Disabled MAC TX flush");
	}

	rc = al_mod_eth_mac_start_stop_adv(&adapter->hal_adapter,
				       true,
				       false,
				       false,
				       false);
	if (rc) {
		netdev_err(adapter->netdev,
			   "Failed to stop MAC TX path (rc = %d)", rc);
	} else {
		netdev_dbg(adapter->netdev, "Stopped MAC TX path");
	}

	return rc;
}

static int al_mod_eth_mac_flush_state_config(struct al_mod_eth_adapter *adapter, bool enable)
{
	int rc;

	netdev_dbg(adapter->netdev, "%s: enable:%d", __func__, enable);

	if (enable) {
		rc = al_mod_eth_mac_tx_flush_config(&adapter->hal_adapter, true);
		if (rc)
			netdev_err(adapter->netdev, "Failed to enable MAC TX flush (rc = %d)",
				   rc);
		else
			netdev_dbg(adapter->netdev, "Enabled MAC TX flush");

		rc = al_mod_eth_mac_start_stop_adv(&adapter->hal_adapter,
					       false,
					       true,
					       false,
					       false);
		if (rc)
			netdev_err(adapter->netdev, "Failed to stop MAC RX path (rc = %d)",
				   rc);
		else
			netdev_dbg(adapter->netdev, "Stopped MAC RX path");
	} else {
		rc = al_mod_eth_mac_tx_flush_config(&adapter->hal_adapter, false);
		if (rc)
			netdev_err(adapter->netdev, "Failed to disable MAC TX flush (rc = %d)",
				   rc);
		else
			netdev_dbg(adapter->netdev, "Disabled MAC TX flush");

		rc = al_mod_eth_mac_start_stop_adv(&adapter->hal_adapter,
					       false,
					       true,
					       false,
					       true);
		if (rc)
			netdev_err(adapter->netdev, "Failed to start MAC RX path (rc = %d)",
				   rc);
		else
			netdev_dbg(adapter->netdev, "Started MAC RX path");
	}

	return rc;
}

static int al_mod_eth_hw_stop(struct al_mod_eth_adapter *adapter)
{
	int rc = 0;

	if (IS_ETH_V3_ADV(adapter->rev_id, adapter->dev_id)) {
		/*
		 * Since al_mod_eth_hw_stop() is invoked in al_mod_eth_probe() as well,
		 * we avoid configuring flush mode, since al_mod_eth_mac_start()
		 * has not been invoked yet (happens only in al_mod_eth_up()
		 */
		if (adapter->last_link) {
			rc = al_mod_eth_mac_flush_state_config(adapter, true);
			if (rc)
				goto err_mac_flush_state_config;
		}
	} else {
		al_mod_eth_mac_stop(&adapter->hal_adapter);
		netdev_dbg(adapter->netdev, "Stopped MAC");
	}

	if (adapter->rev_id <= AL_ETH_REV_ID_3)
		if (adapter->udma_num != 0) {
			/* if NIC mode, steer all packets to udma 0*/
			al_mod_eth_mac_table_unicast_add(adapter, AL_ETH_MAC_TABLE_UNICAST_IDX_BASE,
				adapter->mac_addr, 1);
			al_mod_eth_mac_table_broadcast_add(adapter, AL_ETH_MAC_TABLE_BROADCAST_IDX, 1);
			al_mod_eth_mac_table_promiscuous_set(adapter, true, 1);
		}

	/*
	 * wait till pending rx packets, which are already in the UDMA,
	 * are written and UDMA becomes idle,
	 */
	udelay(100);

	al_mod_eth_udma_queues_reset_all(adapter);
	al_mod_eth_adapter_stop(&adapter->hal_adapter);

	/*
	 * request UDMA-only FLR in next al_mod_eth_up if:
	 * - we were brought up using live update and al_mod_eth_up has not been
	 *   called yet.
	 * - relevant only for rev_id 3 advanced units in Alpine mode.
	 */
	adapter->flags |= AL_ETH_FLAG_RESET_REQUESTED;
	if (adapter->live_update_restore_state && !adapter->last_link)
		adapter->flags |= AL_ETH_FLAG_RESET_UDMA_EC;

	/* disable flow ctrl to avoid pause packets*/
	al_mod_eth_flow_ctrl_disable(adapter);

err_mac_flush_state_config:
	return rc;

}

static int al_mod_eth_change_mtu(struct net_device *dev, int new_mtu)
{
	struct al_mod_eth_adapter *adapter = netdev_priv(dev);
	int max_frame = new_mtu + ETH_HLEN + ETH_FCS_LEN + VLAN_HLEN;

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 10, 0)
	if ((new_mtu < AL_ETH_MIN_MTU) || (new_mtu > AL_ETH_MAX_MTU)) {
		netdev_err(dev, "Invalid MTU setting\n");
		return -EINVAL;
	}
#endif

	netdev_dbg(adapter->netdev, "changing MTU from %d to %d\n", dev->mtu, new_mtu);
	al_mod_eth_rx_pkt_limit_config(&adapter->hal_adapter,
		AL_ETH_MIN_FRAME_LEN, max_frame);

	dev->mtu = new_mtu;

	return 0;
}

/**
 * al_mod_eth_intr_msix_all - MSIX Interrupt Handler for all interrupts
 * @irq: interrupt number
 * @data: pointer to a network interface device structure
 **/
static irqreturn_t al_mod_eth_intr_msix_all(int irq, void *data)
{
	return IRQ_HANDLED;
}

/**
 * al_mod_eth_intr_msix_mgmt - MSIX Interrupt Handler for Management interrupts
 * @irq: interrupt number
 * @data: pointer to a network interface device structure
 **/
static irqreturn_t al_mod_eth_intr_msix_mgmt(int irq, void *data)
{
	/* Currently this interrupt is triggered due to the following events:
	 * 1. UDMA M2S error/hint aggregate
	 * 2. UDMA S2M error/hint aggregate
	 * Specific handling of each error/hint will be added in al_mod_eth_error_cb
	 */
	struct al_mod_eth_adapter *adapter = data;

	netdev_dbg(adapter->netdev, "%s: entered\n", __func__);

	if (adapter->err_events_enabled) {
		al_mod_err_events_module_collect(&adapter->udma_data.module,
					     AL_ERR_EVENTS_COLLECT_INTERRUPT);

		if (EC_ERRS_COLLECTED_BY_INTR(adapter)) {
			if (al_mod_err_events_udma_ext_app_int_read_and_clear(&adapter->udma_data)) {
				netdev_dbg(adapter->netdev,
					   "%s: collect ec/mac errors\n",
					   __func__);
				al_mod_err_events_module_collect(&adapter->ec_data.module,
							     AL_ERR_EVENTS_COLLECT_INTERRUPT);
			}
		}
	} else {
		al_mod_eth_ints_and_errors_print(adapter);
	}

	/* We only listen on AL_INT_GROUP_A_GROUP_D_SUM, and group A auto
	 * clears, so just need to unmask again */
	al_mod_udma_iofic_unmask_adv(&adapter->hal_adapter.tx_udma,
				 AL_UDMA_IOFIC_LEVEL_PRIMARY, AL_INT_GROUP_A,
				 AL_INT_GROUP_A_GROUP_D_SUM);

	return IRQ_HANDLED;
}

/**
 * al_mod_eth_intr_msix_tx - MSIX Interrupt Handler for Tx
 * @irq: interrupt number
 * @data: pointer to a network interface private napi device structure
 **/
static irqreturn_t al_mod_eth_intr_msix_tx(int irq, void *data)
{
	struct al_mod_eth_napi *al_mod_napi = data;

	pr_debug("%s\n", __func__);
	napi_schedule(&al_mod_napi->napi);

	return IRQ_HANDLED;
}

/**
 * al_mod_eth_intr_msix_rx - MSIX Interrupt Handler for Rx
 * @irq: interrupt number
 * @data: pointer to a network interface private napi device structure
 **/
static irqreturn_t al_mod_eth_intr_msix_rx(int irq, void *data)
{
	struct al_mod_eth_napi *al_mod_napi = data;

	pr_debug("%s\n", __func__);
	napi_schedule(&al_mod_napi->napi);
	return IRQ_HANDLED;
}

static int al_mod_init_rx_cpu_rmap(struct al_mod_eth_adapter *adapter)
{
#ifdef CONFIG_RFS_ACCEL
	unsigned int i;
	int rc;

	adapter->netdev->rx_cpu_rmap = alloc_irq_cpu_rmap(adapter->num_rx_queues);
	if (!adapter->netdev->rx_cpu_rmap)
		return -ENOMEM;
	for (i = 0; i < adapter->num_rx_queues; i++) {
		int	irq_idx = AL_ETH_RXQ_IRQ_IDX(adapter, i);

		rc = irq_cpu_rmap_add(adapter->netdev->rx_cpu_rmap,
				      adapter->msix_entries[irq_idx].vector);
		if (rc) {
			free_irq_cpu_rmap(adapter->netdev->rx_cpu_rmap);
			adapter->netdev->rx_cpu_rmap = NULL;
			return rc;
		}
	}
#endif
	return 0;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 12, 0)
static inline int al_mod_eth_pci_enable_msix(struct al_mod_eth_adapter *adapter,
					 int *msix_vecs)
{
	int rc;

	/*
	 * This functions returns the number of msix vectors successfully
	 * allocated (between the given min and max values), or a negative error
	 * code.
	 * We want exact number of vectors - either all or 1, so we always call
	 * this function with min==max
	 */
	rc = pci_enable_msix_range(adapter->pdev, adapter->msix_entries,
				   *msix_vecs, *msix_vecs);
	if (rc == *msix_vecs)
		return 0;

	*msix_vecs = 1; /* if we can't allocate all, then try only 1; */
	rc = pci_enable_msix_range(adapter->pdev, adapter->msix_entries,
				   *msix_vecs, *msix_vecs);
	if (rc == *msix_vecs)
		return 0;
	return rc;
}
#else
static inline int al_mod_eth_pci_enable_msix(struct al_mod_eth_adapter *adapter,
					 int *msix_vecs)
{
	int rc;

	rc = pci_enable_msix(adapter->pdev, adapter->msix_entries, *msix_vecs);
	if (rc <= 0)
		return rc;

	*msix_vecs = 1; /* if we can't allocate all, then try only 1; */
	return pci_enable_msix(adapter->pdev, adapter->msix_entries, *msix_vecs);
}
#endif

static void al_mod_eth_enable_msix(struct al_mod_eth_adapter *adapter)
{
	int i, msix_vecs, rc;

	msix_vecs  = 1 + adapter->num_rx_queues + adapter->num_tx_queues;

	dev_dbg(&adapter->pdev->dev, "Try to enable MSIX, vectors %d\n",
			msix_vecs);

	adapter->msix_entries = kcalloc(msix_vecs,
					sizeof(struct msix_entry), GFP_KERNEL);

	if (!adapter->msix_entries) {
		dev_err(&adapter->pdev->dev, "failed to allocate msix_entries, vectors %d\n",
			msix_vecs);

		return;
	}

	/* management vector (GROUP_A) @2*/
	adapter->msix_entries[AL_ETH_MGMT_IRQ_IDX].entry = AL_INT_MSIX_GROUP_A_SUM_D_IDX;
	adapter->msix_entries[AL_ETH_MGMT_IRQ_IDX].vector = 0;

	/* rx queues start @3 */
	for (i = 0; i < adapter->num_rx_queues; i++) {
		int	irq_idx = AL_ETH_RXQ_IRQ_IDX(adapter, i);

		adapter->msix_entries[irq_idx].entry = AL_INT_MSIX_RX_COMPLETION_START + i;
		adapter->msix_entries[irq_idx].vector = 0;
	}
	/* tx queues start @7 */
	for (i = 0; i < adapter->num_tx_queues; i++) {
		int	irq_idx = AL_ETH_TXQ_IRQ_IDX(adapter, i);
		/** TX msix entry is at different location - depends on num of HW queues */
		adapter->msix_entries[irq_idx].entry = AL_INT_MSIX_RX_COMPLETION_START +
						AL_ETH_UDMA_HW_QUEUES_BY_REV(adapter->rev_id) + i;
		adapter->msix_entries[irq_idx].vector = 0;
	}

	rc = al_mod_eth_pci_enable_msix(adapter, &msix_vecs);
	if (rc != 0) {
		dev_err(&adapter->pdev->dev, "failed to enable MSIX, vectors %d, rc %d\n",
			   msix_vecs, rc);
		adapter->msix_vecs = 0;
		kfree(adapter->msix_entries);
		adapter->msix_entries = NULL;

		return;
	}
	dev_dbg(&adapter->pdev->dev, "enable MSIX, vectors %d\n", msix_vecs);

	/* enable MSIX in the msix capability of the eth controller
	 * as the pci_enable_msix[_range] enables it in the pcie unit capability
	 */
	if (IS_NIC(adapter->board_type))
		writew(PCI_MSIX_FLAGS_ENABLE,
			adapter->internal_pcie_base + 0x1000 * adapter->udma_num + 0x92);

	if (msix_vecs >= 1) {
		if (al_mod_init_rx_cpu_rmap(adapter))
			dev_warn(&adapter->pdev->dev, "failed to map irqs to cpus\n");
	}

	adapter->msix_vecs = msix_vecs;
	adapter->flags |= AL_ETH_FLAG_MSIX_ENABLED;
}

/* This function initializes the initial interrupt moderation table (which can be changed through a
 * sysfs exposed interace (as opposed to the default interrtupt moderation table which cannot).
 */
static void al_mod_eth_configure_intr_moderation_table(struct al_mod_eth_intr_moderation_entry
						*intr_moderation_table)
{
	unsigned int i;

	for (i = 0; i < AL_ETH_INTR_MAX_NUM_OF_LEVELS; i++) {
		intr_moderation_table[i].intr_moderation_interval =
			initial_moderation_table[i].intr_moderation_interval;
		intr_moderation_table[i].packets_per_interval =
			initial_moderation_table[i].packets_per_interval;
		intr_moderation_table[i].bytes_per_interval =
			initial_moderation_table[i].bytes_per_interval;
	}
}


/* This function initializes the static default_moderation_table interval values according to the
 * actual SB clock of the chip we are running on
 */
static void al_mod_eth_init_intr_default_moderation_table_intervals(struct al_mod_eth_adapter *adapter)
{
	uint sb_clk_freq = AL_ETH_REF_CLK_FREQ_TO_HZ(adapter->ref_clk_freq) / 1000;

	default_moderation_table[AL_ETH_INTR_MODERATION_LOWEST].intr_moderation_interval =
		AL_ETH_INTR_LOWEST_VALUE(sb_clk_freq);
	default_moderation_table[AL_ETH_INTR_MODERATION_LOW].intr_moderation_interval =
		AL_ETH_INTR_LOW_VALUE(sb_clk_freq);
	default_moderation_table[AL_ETH_INTR_MODERATION_MID].intr_moderation_interval =
		AL_ETH_INTR_MID_VALUE(sb_clk_freq);
	default_moderation_table[AL_ETH_INTR_MODERATION_HIGH].intr_moderation_interval =
		AL_ETH_INTR_HIGH_VALUE(sb_clk_freq);
	default_moderation_table[AL_ETH_INTR_MODERATION_HIGHEST].intr_moderation_interval =
		AL_ETH_INTR_HIGHEST_VALUE(sb_clk_freq);
}


void al_mod_eth_initial_moderation_table_restore_default(void)
{
	int i;

	for (i = 0; i < AL_ETH_INTR_MAX_NUM_OF_LEVELS; i++)
		initial_moderation_table[i] = default_moderation_table[i];
}


void al_mod_eth_init_intr_moderation_entry(enum al_mod_eth_intr_moderation_level level,
		struct al_mod_eth_intr_moderation_entry *entry)
{
	if (level >= AL_ETH_INTR_MAX_NUM_OF_LEVELS)
		return;

	initial_moderation_table[level].intr_moderation_interval =
			entry->intr_moderation_interval;
	initial_moderation_table[level].packets_per_interval =
			entry->packets_per_interval;
	initial_moderation_table[level].bytes_per_interval =
			entry->bytes_per_interval;
}

void al_mod_eth_get_intr_moderation_entry(enum al_mod_eth_intr_moderation_level level,
	struct al_mod_eth_intr_moderation_entry *entry)
{
	if (level >= AL_ETH_INTR_MAX_NUM_OF_LEVELS)
		return;

	entry->intr_moderation_interval =
			initial_moderation_table[level].intr_moderation_interval;
	entry->packets_per_interval =
			initial_moderation_table[level].packets_per_interval;
	entry->bytes_per_interval =
			initial_moderation_table[level].bytes_per_interval;
}

void al_mod_eth_update_intr_moderation(struct al_mod_eth_adapter *adapter, unsigned int qid,
		enum al_mod_eth_direction direction)
{
	struct al_mod_eth_ring *ring;
	unsigned int packets, bytes;
	unsigned int smoothed_interval;
	enum al_mod_eth_intr_moderation_level current_moderation_indx, new_moderation_indx;
	struct al_mod_eth_intr_moderation_entry *intr_moderation_table = adapter->intr_moderation_table;
	struct al_mod_eth_intr_moderation_entry current_moderation_entry;
	struct al_mod_eth_intr_moderation_entry preceding_moderation_entry;
	struct al_mod_eth_intr_moderation_entry new_moderation_entry;
	void __iomem *iofic_regs_base_tx_udma;
	void __iomem *iofic_regs_base_rx_udma;

	if (direction == AL_ETH_TX)
		ring = &adapter->tx_ring[qid];
	else
		ring = &adapter->rx_ring[qid];
	pr_debug("qid=%d\n", qid);

	packets = ring->packets;
	bytes = ring->bytes;

	if (!packets || !bytes)
		return;

	current_moderation_indx = ring->moderation_table_indx;
	if (current_moderation_indx >=  AL_ETH_INTR_MAX_NUM_OF_LEVELS) {
		netdev_dbg(adapter->netdev, "Wrong moderation index\n");
		return;
	}
	current_moderation_entry = intr_moderation_table[current_moderation_indx];

	new_moderation_indx = current_moderation_indx;

	if (current_moderation_indx == AL_ETH_INTR_MODERATION_LOWEST) {
		if ((packets > current_moderation_entry.packets_per_interval) ||
				(bytes > current_moderation_entry.bytes_per_interval))
			new_moderation_indx = current_moderation_indx + 1;
	} else {
		preceding_moderation_entry = intr_moderation_table[current_moderation_indx - 1];

		if ((packets <= preceding_moderation_entry.packets_per_interval) ||
				(bytes <= preceding_moderation_entry.bytes_per_interval))
			new_moderation_indx = current_moderation_indx - 1;

		else if ((packets > current_moderation_entry.packets_per_interval) ||
				(bytes > current_moderation_entry.bytes_per_interval)) {
			if (current_moderation_indx != AL_ETH_INTR_MODERATION_HIGHEST)
				new_moderation_indx = current_moderation_indx + 1;
		}
	}
	new_moderation_entry = intr_moderation_table[new_moderation_indx];

	ring->packets = 0;
	ring->bytes = 0;

	smoothed_interval = ((new_moderation_entry.intr_moderation_interval * 4 +
			6 * ring->smoothed_interval) + 5) / 10;
	ring->smoothed_interval = smoothed_interval;
	ring->moderation_table_indx = new_moderation_indx;

	if (direction == AL_ETH_TX) {
		iofic_regs_base_tx_udma = al_mod_udma_iofic_reg_base_get_adv(
			&(adapter->hal_adapter.tx_udma), AL_UDMA_IOFIC_LEVEL_PRIMARY);
		al_mod_iofic_msix_moder_interval_config(iofic_regs_base_tx_udma,
				AL_INT_GROUP_C, qid, smoothed_interval);
	} else { /* RX */
		iofic_regs_base_rx_udma = al_mod_udma_iofic_reg_base_get_adv(
			&(adapter->hal_adapter.rx_udma), AL_UDMA_IOFIC_LEVEL_PRIMARY);
		al_mod_iofic_msix_moder_interval_config(iofic_regs_base_rx_udma,
				AL_INT_GROUP_B, qid, smoothed_interval);
	}
	return;
}

static int al_mod_eth_get_coalesce(struct net_device *net_dev,
				    struct ethtool_coalesce *coalesce)
{
	struct al_mod_eth_adapter *adapter = netdev_priv(net_dev);

	coalesce->tx_coalesce_usecs = adapter->tx_usecs;
	coalesce->tx_coalesce_usecs_irq = adapter->tx_usecs;
	coalesce->rx_coalesce_usecs = adapter->rx_usecs;
	coalesce->rx_coalesce_usecs_irq = adapter->rx_usecs;
	coalesce->use_adaptive_rx_coalesce = adapter->adaptive_intr_rate;

	return 0;
}

static void al_mod_eth_set_coalesce(
	struct al_mod_eth_adapter *adapter,
	unsigned int tx_usecs,
	unsigned int rx_usecs)
{
	void __iomem *iofic_regs_base;
	uint sb_clk_freq;
	uint interval;
	sb_clk_freq = AL_ETH_REF_CLK_FREQ_TO_HZ(adapter->ref_clk_freq) / 1000;

	if (adapter->tx_usecs != tx_usecs) {
		int qid;
		interval =
			(uint) (1000 * tx_usecs / AL_ETH_INTR_MODERATION_RESOLUTION_NSECS(
				sb_clk_freq));
		BUG_ON(interval > 255);
		adapter->tx_usecs =
			interval * AL_ETH_INTR_MODERATION_RESOLUTION_NSECS(
				sb_clk_freq) / 1000;

		iofic_regs_base = al_mod_udma_iofic_reg_base_get_adv(
				&(adapter->hal_adapter.tx_udma), AL_UDMA_IOFIC_LEVEL_PRIMARY);
		for (qid = 0; qid < adapter->num_tx_queues; qid++)
			al_mod_iofic_msix_moder_interval_config(
				iofic_regs_base,
				AL_INT_GROUP_C, qid, interval);
	}
	if (adapter->rx_usecs != rx_usecs) {
		int qid;
		interval =
			(uint) (1000 * rx_usecs / AL_ETH_INTR_MODERATION_RESOLUTION_NSECS(
				sb_clk_freq));
		BUG_ON(interval > 255);
		adapter->rx_usecs =
			interval * AL_ETH_INTR_MODERATION_RESOLUTION_NSECS(
				sb_clk_freq) / 1000;

		iofic_regs_base = al_mod_udma_iofic_reg_base_get_adv(
				&(adapter->hal_adapter.rx_udma), AL_UDMA_IOFIC_LEVEL_PRIMARY);
		for (qid = 0; qid < adapter->num_rx_queues; qid++)
			al_mod_iofic_msix_moder_interval_config(
				iofic_regs_base,
				AL_INT_GROUP_B, qid, interval);
	}
}

static int al_mod_eth_configure_int_mode(struct al_mod_eth_adapter *adapter)
{
	enum al_mod_iofic_mode int_mode;
	uint32_t	m2s_errors_disable = 0x480;
	uint32_t	m2s_aborts_disable = 0x480;
	uint32_t	s2m_errors_disable = 0x1E0;
	uint32_t	s2m_aborts_disable = 0x1E0;
	uint32_t	intr_mod_res;
	struct al_mod_udma_iofic_cfg iofic_cfg = {0};
	void __iomem *iofic_regs_base_tx_udma;
	void __iomem *iofic_regs_base_rx_udma;

	/* single INTX mode */
	if (adapter->msix_vecs == 0)
		int_mode = AL_IOFIC_MODE_LEGACY;
	else if (adapter->msix_vecs > 1)
		int_mode = AL_IOFIC_MODE_MSIX_PER_Q;
	else {
		netdev_err(adapter->netdev, "udma doesn't support single MSI-X mode yet.\n");
		return -EIO;
	}

	if (adapter->board_type != ALPINE_INTEGRATED) {
		/* TODO: When error events will be integrated, verify that these
		 * hardcoded values are indeed what we need, and in any case
		 * change to macros from al_mod_hal_udma_iofic.h
		 */
		/* M2S */
		m2s_errors_disable |= 0x3f << 25;
		m2s_aborts_disable |= 0x3f << 25;
		/* S2M */
		s2m_errors_disable |= 0x3f << 25;
		s2m_aborts_disable |= 0x3f << 25;
	}

	iofic_cfg.mode = int_mode;
	iofic_cfg.m2s_aborts_disable = m2s_aborts_disable;
	iofic_cfg.m2s_errors_disable = m2s_errors_disable;
	iofic_cfg.s2m_aborts_disable = s2m_aborts_disable;
	iofic_cfg.s2m_errors_disable = s2m_errors_disable;

	if (al_mod_udma_iofic_config_ex(&adapter->hal_adapter.tx_udma, &iofic_cfg)) {
			netdev_err(adapter->netdev, "al_mod_udma_unit_int_config failed!.\n");
			return -EIO;
	}
	adapter->int_mode = int_mode;
	netdev_info(adapter->netdev, "using %s interrupt mode",
		int_mode == AL_IOFIC_MODE_LEGACY ? "INTx" :
		int_mode == AL_IOFIC_MODE_MSIX_PER_Q ? "MSI-X per Queue" :
		"Unknown");
	/* set interrupt moderation resolution */
	intr_mod_res = AL_ETH_INTR_MODERATION_RESOLUTION;

	iofic_regs_base_tx_udma = al_mod_udma_iofic_reg_base_get_adv(
		&(adapter->hal_adapter.tx_udma), AL_UDMA_IOFIC_LEVEL_PRIMARY);
	iofic_regs_base_rx_udma = al_mod_udma_iofic_reg_base_get_adv(
		&(adapter->hal_adapter.rx_udma), AL_UDMA_IOFIC_LEVEL_PRIMARY);

	al_mod_iofic_moder_res_config(
			iofic_regs_base_rx_udma,
			AL_INT_GROUP_B, intr_mod_res);
	al_mod_iofic_moder_res_config(
			iofic_regs_base_tx_udma,
			AL_INT_GROUP_C, intr_mod_res);

	if (adapter->adaptive_intr_rate) {
		/* set initial moderation settings */
		al_mod_eth_set_coalesce(adapter, AL_ETH_INTR_INITIAL_TX_INTERVAL_USECS,
				AL_ETH_INTR_INITIAL_RX_INTERVAL_USECS);
		al_mod_eth_configure_intr_moderation_table(adapter->intr_moderation_table);
	}

	return 0;
}

static void al_mod_eth_interrupts_unmask(struct al_mod_eth_adapter *adapter)
{
	u32 group_a_mask = AL_INT_GROUP_A_GROUP_D_SUM; /* enable group D summery */
	u32 group_b_mask = (1 << adapter->num_rx_queues) - 1;/* bit per Rx q*/
	u32 group_c_mask = (1 << adapter->num_tx_queues) - 1;/* bit per Tx q*/
	u32 group_d_mask;

	group_d_mask = AL_INT_GROUP_D_M2S |
		       AL_INT_GROUP_D_S2M;

	if (adapter->rev_id >= AL_ETH_REV_ID_4)
		group_d_mask |= AL_INT_UDMA_V4_GROUP_D_2ND_IOFIC_GROUP_C;

	if (adapter->int_mode == AL_IOFIC_MODE_LEGACY)
		group_a_mask |= AL_INT_GROUP_A_GROUP_B_SUM |
				AL_INT_GROUP_A_GROUP_C_SUM |
				AL_INT_GROUP_A_GROUP_D_SUM;

	al_mod_udma_iofic_unmask_adv(&adapter->hal_adapter.tx_udma, AL_UDMA_IOFIC_LEVEL_PRIMARY,
			AL_INT_GROUP_A, group_a_mask);
	al_mod_udma_iofic_unmask_adv(&adapter->hal_adapter.rx_udma, AL_UDMA_IOFIC_LEVEL_PRIMARY,
			AL_INT_GROUP_B, group_b_mask);
	al_mod_udma_iofic_unmask_adv(&adapter->hal_adapter.tx_udma, AL_UDMA_IOFIC_LEVEL_PRIMARY,
			AL_INT_GROUP_C, group_c_mask);
	al_mod_udma_iofic_unmask_adv(&adapter->hal_adapter.tx_udma, AL_UDMA_IOFIC_LEVEL_PRIMARY,
			AL_INT_GROUP_D, group_d_mask);

	if (adapter->ua_irq.requested)
		al_mod_err_events_ua_ints_enable(&adapter->ua_data);
}

static void al_mod_eth_interrupts_mask(struct al_mod_eth_adapter *adapter)
{
	u32 group_b_mask = (1 << adapter->num_rx_queues) - 1;/* bit per Rx q*/
	u32 group_c_mask = (1 << adapter->num_tx_queues) - 1;/* bit per Tx q*/
	u32 group_a_mask = 0;

	group_a_mask |= AL_INT_GROUP_A_GROUP_B_SUM |
			AL_INT_GROUP_A_GROUP_C_SUM |
			AL_INT_GROUP_A_GROUP_D_SUM;

	if (adapter->ua_irq.requested)
		al_mod_err_events_ua_ints_disable(&adapter->ua_data);

	/* mask all interrupts */
	al_mod_udma_iofic_mask_adv(&adapter->hal_adapter.tx_udma, AL_UDMA_IOFIC_LEVEL_PRIMARY,
			AL_INT_GROUP_A, group_a_mask);
	al_mod_udma_iofic_mask_adv(&adapter->hal_adapter.rx_udma, AL_UDMA_IOFIC_LEVEL_PRIMARY,
			AL_INT_GROUP_B, group_b_mask);
	al_mod_udma_iofic_mask_adv(&adapter->hal_adapter.tx_udma, AL_UDMA_IOFIC_LEVEL_PRIMARY,
			AL_INT_GROUP_C, group_c_mask);
	al_mod_udma_iofic_mask_adv(&adapter->hal_adapter.tx_udma, AL_UDMA_IOFIC_LEVEL_PRIMARY,
			AL_INT_GROUP_D, 0xFFFFFFFF);
}

#ifndef CONFIG_ARCH_ALPINE
static int al_mod_eth_modify_pcie_ep_int(struct al_mod_eth_adapter *adapter, int enable)
{
#ifdef	AL_HAL_HAS_PCIE_INT_FWD_ENABLE_DISABLE
	struct al_mod_pcie_port pcie_port = {0};
	unsigned int pf_num = adapter->pf_num; /** TODO */
	int err = 0;
	unsigned int pcie_port_num = adapter->pcie_ep_num;

	void __iomem *sb_pcie_base = adapter->internal_sb_base +
			al_mod_iomap_offset_idx_sub_idx_get(
			adapter->al_mod_chip_id, AL_IOMAP_SB_PCIE_OFFSET, pcie_port_num, 0);
	void __iomem *pbs_regfile_base = adapter->pbs_regfile_base;

	al_mod_assert(IS_NIC(adapter->board_type));

	dev_info(&adapter->pdev->dev, "%s : %s pci ep interrupts\n",
		__func__, enable ? "Enabling" : "Disabling");

	spin_lock(&pcie_int_lock);

	err = al_mod_pcie_port_handle_init(&pcie_port, sb_pcie_base,
			pbs_regfile_base, pcie_port_num);
	if (err != 0) {
		dev_err(&adapter->pdev->dev, "Failed to init pcie port handle\n");
		goto error;
	}

	if (enable == 0)
		err = al_mod_pcie_interrupt_forwarding_disable(&pcie_port, pf_num,
			AL_PCIE_INT_FWD_UNIT_ETH, adapter->port_num);
	else
		err = al_mod_pcie_interrupt_forwarding_enable(&pcie_port, pf_num,
				AL_PCIE_INT_FWD_UNIT_ETH, adapter->port_num);

	if (err != 0)
		dev_err(&adapter->pdev->dev, "%s : Failed to %s pcie end point interrupts\n",
			__func__, enable ? "enable" : "disable");

error:
	spin_unlock(&pcie_int_lock);
	return err;
#else
	al_mod_assert(IS_NIC(adapter->board_type));

	/* Disable forwarding interrupts from eth through pci end point*/
	uint32_t val = enable ? 0x1FFFF : 0;
#if defined(CONFIG_AL_ETH_SRIOV)
	/** TODO - is just checking SRIOV enough? perhaps if we wish to use al_mod_eth on EVP + SRIOV this */
	/** will ruin it */
	if (adapter->pdev->is_physfn)
		/* enable forwarding interrupts from eth through pci end point*/
		writel(val, adapter->internal_pcie_base + 0x1800000 + 0x1210);
#else
	netdev_dbg(adapter->netdev, "disable int forwarding\n");
	if (adapter->rev_id <= AL_ETH_REV_ID_3)
		writel(val, adapter->internal_pcie_base + 0x1800000 + 0x1210);
	else
		writel(val, adapter->internal_pcie_base + 0x1800000 + 0x232c);

	return 0;
#endif /* defined(CONFIG_AL_ETH_SRIOV) */
#endif
}
#endif /* CONFIG_ARCH_ALPINE */

static void al_mod_eth_disable_int_sync(struct al_mod_eth_adapter *adapter)
{
	int i;

	if (!netif_running(adapter->netdev))
		return;
#ifndef CONFIG_ARCH_ALPINE
	{
		int err = 0;
		/* Disable forwarding interrupts from eth through pci end point*/
		err = al_mod_eth_modify_pcie_ep_int(adapter, 0);
		if (err != 0)
			netdev_err(adapter->netdev, "%s : Failed at al_mod_eth_modify_pcie_ep_int\n",
				__func__);
	}
#endif
	/* Mask hw interrupts */
	al_mod_eth_interrupts_mask(adapter);

	for (i = 0; i < adapter->irq_vecs; i++)
		synchronize_irq(adapter->irq_tbl[i].vector);
}

/**
 * al_mod_eth_intr_intx_all - Legacy Interrupt Handler for all interrupts
 * @irq: interrupt number
 * @data: pointer to a network interface device structure
 **/
static irqreturn_t al_mod_eth_intr_intx_all(int irq, void *data)
{
	struct al_mod_eth_adapter *adapter = data;
	uint32_t reg;

	reg = al_mod_udma_iofic_read_cause_adv(&adapter->hal_adapter.tx_udma,
			AL_UDMA_IOFIC_LEVEL_PRIMARY, AL_INT_GROUP_A);
	if (likely(reg))
		pr_debug("%s group A cause %x\n", __func__, reg);

	if (unlikely(reg & AL_INT_GROUP_A_GROUP_D_SUM)) {
		uint32_t cause_d =  al_mod_udma_iofic_read_cause_adv(&adapter->hal_adapter.tx_udma,
							     AL_UDMA_IOFIC_LEVEL_PRIMARY,
							     AL_INT_GROUP_D);

		if (cause_d) {
			pr_debug("got interrupt from group D. cause %x\n", cause_d);

			cause_d = al_mod_udma_iofic_read_cause_adv(&adapter->hal_adapter.tx_udma,
					AL_UDMA_IOFIC_LEVEL_SECONDARY,
					AL_INT_GROUP_A);
			pr_debug("secondary A cause %x\n", cause_d);

			cause_d = al_mod_udma_iofic_read_cause_adv(&adapter->hal_adapter.rx_udma,
					AL_UDMA_IOFIC_LEVEL_SECONDARY,
					AL_INT_GROUP_B);

			pr_debug("secondary B cause %x\n", cause_d);
		}
	}
	if (reg & AL_INT_GROUP_A_GROUP_B_SUM) {
		uint32_t cause_b = al_mod_udma_iofic_read_cause_adv(&adapter->hal_adapter.rx_udma,
							    AL_UDMA_IOFIC_LEVEL_PRIMARY,
							    AL_INT_GROUP_B);
		int qid;
		for (qid = 0; qid < adapter->num_rx_queues; qid++) {
			if (cause_b & (1 << qid)) {
				/* mask */
				al_mod_udma_iofic_mask_adv(
					&adapter->hal_adapter.rx_udma,
					AL_UDMA_IOFIC_LEVEL_PRIMARY,
					AL_INT_GROUP_B, 1 << qid);

				napi_schedule(
					&adapter->al_mod_napi[AL_ETH_RXQ_NAPI_IDX(adapter, qid)].napi);
			}
		}
	}
	if (reg & AL_INT_GROUP_A_GROUP_C_SUM) {
		uint32_t cause_c = al_mod_udma_iofic_read_cause_adv(&adapter->hal_adapter.tx_udma,
							    AL_UDMA_IOFIC_LEVEL_PRIMARY,
							    AL_INT_GROUP_C);
		int qid;
		for (qid = 0; qid < adapter->num_tx_queues; qid++) {
			if (cause_c & (1 << qid)) {
				/* mask */
				al_mod_udma_iofic_mask_adv(
					&adapter->hal_adapter.tx_udma,
					AL_UDMA_IOFIC_LEVEL_PRIMARY,
					AL_INT_GROUP_C, 1 << qid);

				napi_schedule(
					&adapter->al_mod_napi[AL_ETH_TXQ_NAPI_IDX(adapter, qid)].napi);
			}
		}
	}

	return IRQ_HANDLED;
}

static irqreturn_t al_mod_eth_ua_irq_handler(int irq, void *data)
{
	struct al_mod_eth_adapter *adapter = data;

	netdev_dbg(adapter->netdev, "%s: entered\n", __func__);

	al_mod_err_events_module_collect(&adapter->ua_data.module,
				     AL_ERR_EVENTS_COLLECT_INTERRUPT);

	return IRQ_HANDLED;
}

static int al_mod_eth_setup_int_mode(struct al_mod_eth_adapter *adapter, int dis_msi)
{
	int i;
	unsigned int cpu;

	netdev_dbg(adapter->netdev, " setup int %d %d %d\n",
		dis_msi, adapter->msix_vecs, adapter->pdev->irq);

	if (!dis_msi)
		al_mod_eth_enable_msix(adapter);

	adapter->irq_vecs = max(1, adapter->msix_vecs);

	/* single INTX mode */
	if (adapter->msix_vecs == 0) {
		snprintf(adapter->irq_tbl[AL_ETH_MGMT_IRQ_IDX].name, AL_ETH_IRQNAME_SIZE,
			 "al-eth-intx-all@pci:%s", pci_name(adapter->pdev));
		adapter->irq_tbl[AL_ETH_MGMT_IRQ_IDX].handler = al_mod_eth_intr_intx_all;
		adapter->irq_tbl[AL_ETH_MGMT_IRQ_IDX].vector = adapter->pdev->irq;
		adapter->irq_tbl[AL_ETH_MGMT_IRQ_IDX].data = adapter;

		cpu = cpumask_first(cpu_online_mask);
		cpumask_set_cpu(cpu, &adapter->irq_tbl[AL_ETH_MGMT_IRQ_IDX].affinity_hint_mask);

		return 0;
	}

	/* single MSI-X mode */
	if (adapter->msix_vecs == 1) {
		snprintf(adapter->irq_tbl[AL_ETH_MGMT_IRQ_IDX].name, AL_ETH_IRQNAME_SIZE,
			 "al-eth-msix-all@pci:%s", pci_name(adapter->pdev));
		adapter->irq_tbl[AL_ETH_MGMT_IRQ_IDX].handler = al_mod_eth_intr_msix_all;
		adapter->irq_tbl[AL_ETH_MGMT_IRQ_IDX].vector =
			adapter->msix_entries[AL_ETH_MGMT_IRQ_IDX].vector;
		adapter->irq_tbl[AL_ETH_MGMT_IRQ_IDX].data = adapter;

		cpu = cpumask_first(cpu_online_mask);
		cpumask_set_cpu(cpu, &adapter->irq_tbl[AL_ETH_MGMT_IRQ_IDX].affinity_hint_mask);

		return 0;
	}
	/* MSI-X per queue*/
	snprintf(adapter->irq_tbl[AL_ETH_MGMT_IRQ_IDX].name, AL_ETH_IRQNAME_SIZE,
		"al-eth-msix-mgmt@pci:%s", pci_name(adapter->pdev));
	adapter->irq_tbl[AL_ETH_MGMT_IRQ_IDX].handler = al_mod_eth_intr_msix_mgmt;

	adapter->irq_tbl[AL_ETH_MGMT_IRQ_IDX].data = adapter;
	adapter->irq_tbl[AL_ETH_MGMT_IRQ_IDX].vector =
		adapter->msix_entries[AL_ETH_MGMT_IRQ_IDX].vector;
	cpu = cpumask_first(cpu_online_mask);
	cpumask_set_cpu(cpu, &adapter->irq_tbl[AL_ETH_MGMT_IRQ_IDX].affinity_hint_mask);

	for (i = 0; i < adapter->num_rx_queues; i++) {
		int	irq_idx = AL_ETH_RXQ_IRQ_IDX(adapter, i);
		int	napi_idx = AL_ETH_RXQ_NAPI_IDX(adapter, i);

		snprintf(adapter->irq_tbl[irq_idx].name, AL_ETH_IRQNAME_SIZE,
			 "al-eth-rx-comp-%d@pci:%s", i,
			 pci_name(adapter->pdev));
		adapter->irq_tbl[irq_idx].handler = al_mod_eth_intr_msix_rx;
		adapter->irq_tbl[irq_idx].data = &adapter->al_mod_napi[napi_idx];
		adapter->irq_tbl[irq_idx].vector = adapter->msix_entries[irq_idx].vector;

		cpu = cpumask_next((i % num_online_cpus() - 1), cpu_online_mask);
		cpumask_set_cpu(cpu, &adapter->irq_tbl[irq_idx].affinity_hint_mask);
	}

	for (i = 0; i < adapter->num_tx_queues; i++) {
		int	irq_idx = AL_ETH_TXQ_IRQ_IDX(adapter, i);
		int	napi_idx = AL_ETH_TXQ_NAPI_IDX(adapter, i);

		snprintf(adapter->irq_tbl[irq_idx].name,
			 AL_ETH_IRQNAME_SIZE, "al-eth-tx-comp-%d@pci:%s", i,
			 pci_name(adapter->pdev));
		adapter->irq_tbl[irq_idx].handler = al_mod_eth_intr_msix_tx;
		adapter->irq_tbl[irq_idx].data = &adapter->al_mod_napi[napi_idx];
		adapter->irq_tbl[irq_idx].vector = adapter->msix_entries[irq_idx].vector;

		cpu = cpumask_next((i % num_online_cpus() - 1), cpu_online_mask);
		cpumask_set_cpu(cpu, &adapter->irq_tbl[irq_idx].affinity_hint_mask);
	}

	if (adapter->err_events_enabled && IS_V2_ALPINE(adapter)) {
		snprintf(adapter->ua_irq.name,
			 AL_ETH_IRQNAME_SIZE, "al-eth-unit@pci:%s",
			 pci_name(adapter->pdev));
		adapter->ua_irq.handler = al_mod_eth_ua_irq_handler;
		adapter->ua_irq.data = adapter;
		adapter->ua_irq.affinity_hint_mask =
			adapter->irq_tbl[AL_ETH_MGMT_IRQ_IDX].affinity_hint_mask;
	}

	return 0;
}

static inline void
al_mod_eth_vlan_rx_frag(struct al_mod_eth_adapter *adapter, struct sk_buff *skb, u8 *va, unsigned int *len)
{
	struct skb_frag_struct *frag;
	struct vlan_ethhdr *veh = (struct vlan_ethhdr *)va;

	if (!al_mod_eth_vlan_hwaccel_check_and_put(&adapter->netdev->features, skb, veh)) {
		/* remove vlan header and modify the length bookkeeping fields accordignly */
		memmove(va + VLAN_HLEN, va, 2 * ETH_ALEN);
		skb->len -= VLAN_HLEN;
		skb->data_len -= VLAN_HLEN;
		frag = skb_shinfo(skb)->frags;
		frag->page_offset += VLAN_HLEN;
		skb_frag_size_sub(frag, VLAN_HLEN);
		if (len)
			*len -= VLAN_HLEN;
	}
}

static inline void
al_mod_eth_vlan_rx_linear(struct al_mod_eth_adapter *adapter, struct sk_buff *skb, u8 **va,
		unsigned int *len)
{
	struct vlan_ethhdr *veh = (struct vlan_ethhdr *)*va;

	if (!al_mod_eth_vlan_hwaccel_check_and_put(&adapter->netdev->features, skb, veh)) {
		/* remove vlan header (length bookkeeping will be done by calling function) */
		memmove(*va + VLAN_HLEN, *va, 2 * ETH_ALEN);
		*va += VLAN_HLEN;
		if (len)
			*len -= VLAN_HLEN;
	}
}

#ifdef CONFIG_AL_ETH_ALLOC_PAGE
static	struct sk_buff *al_mod_eth_rx_skb(struct al_mod_eth_adapter *adapter,
				      struct al_mod_eth_ring *rx_ring,
				      struct al_mod_eth_pkt *hal_pkt,
				      unsigned int descs,
				      u16 *next_to_clean)
{
	struct sk_buff *skb;
	struct al_mod_eth_rx_buffer *rx_info =
		&rx_ring->rx_buffer_info[*next_to_clean];
	struct page *page = rx_info->page;
	unsigned int len, orig_len;
	unsigned int buf = 0;
	u8 *va;

	len = hal_pkt->bufs[0].len;
	dev_dbg(&adapter->pdev->dev, "rx_info %p page %p\n",
		rx_info, rx_info->page);

	page = rx_info->page;
	/* save virt address of first buffer */
	va = page_address(rx_info->page) + rx_info->page_offset;
	prefetch(va + AL_ETH_RX_OFFSET);

	if (len <= adapter->small_copy_len) {
		/** Smaller that this len will be copied to the skb header & dont use NAPI skb */
		skb = netdev_alloc_skb_ip_align(adapter->netdev, adapter->small_copy_len);
		if (unlikely(!skb)) {
			/*rx_ring->rx_stats.alloc_rx_buff_failed++;*/
			u64_stats_update_begin(&rx_ring->syncp);
			rx_ring->rx_stats.skb_alloc_fail++;
			u64_stats_update_end(&rx_ring->syncp);
			netdev_dbg(adapter->netdev, "Failed allocating skb\n");
			return NULL;
		}

		netdev_dbg(adapter->netdev, "rx skb allocated. len %d. data_len %d\n",
						skb->len, skb->data_len);

		netdev_dbg(adapter->netdev, "rx small packet. len %d\n", len);
		/* Need to use the same length when using the dma_* APIs */
		orig_len = len;
		/* sync this buffer for CPU use */
		dma_sync_single_for_cpu(rx_ring->dev, rx_info->dma, orig_len,
					DMA_FROM_DEVICE);
		if (hal_pkt->source_vlan_count > 0)
			/* This function alters len when the packet has a vlan tag */
			al_mod_eth_vlan_rx_linear(adapter, skb, &va, &len);

		skb_copy_to_linear_data(skb, va, len);

		dma_sync_single_for_device(rx_ring->dev, rx_info->dma, orig_len,
					DMA_FROM_DEVICE);

		skb_put(skb, len);
		skb->protocol = eth_type_trans(skb, adapter->netdev);
		*next_to_clean = AL_ETH_RX_RING_IDX_ADD(rx_ring, *next_to_clean, descs);
		/** Increase counters (relevent for adaptive interrupt modertaion only) */
		rx_ring->bytes += orig_len;
		return skb;
	}
	/**
	 * Use napi_get_frags, it will call __napi_alloc_skb that will allocate the head from
	 * a special region used only for NAPI Rx allocation.
	 */
	skb = napi_get_frags(rx_ring->napi);
	if (unlikely(!skb)) {
		u64_stats_update_begin(&rx_ring->syncp);
		rx_ring->rx_stats.skb_alloc_fail++;
		u64_stats_update_end(&rx_ring->syncp);
		netdev_dbg(adapter->netdev, "Failed allocating skb\n");
		return NULL;
	}

	do {
		dma_unmap_page(rx_ring->dev, dma_unmap_addr(rx_info, dma),
				PAGE_SIZE, DMA_FROM_DEVICE);

		skb_add_rx_frag(skb, skb_shinfo(skb)->nr_frags, rx_info->page,
				rx_info->page_offset, len, PAGE_SIZE);

		netdev_dbg(adapter->netdev, "rx skb updated. len %d. data_len %d\n",
					skb->len, skb->data_len);

		rx_info->page = NULL;
		*next_to_clean = AL_ETH_RX_RING_IDX_NEXT(rx_ring, *next_to_clean);
		if (likely(--descs == 0))
			break;
		rx_info = &rx_ring->rx_buffer_info[*next_to_clean];
		len += hal_pkt->bufs[++buf].len;
	} while (1);

	al_mod_eth_vlan_rx_frag(adapter, skb, va, &len);

	/** Increase counters (relevent for adaptive interrupt modertaion only) */
	rx_ring->bytes += len;

	return skb;
}
#elif defined(CONFIG_AL_ETH_ALLOC_FRAG)
static	struct sk_buff *al_mod_eth_rx_skb(struct al_mod_eth_adapter *adapter,
				      struct al_mod_eth_ring *rx_ring,
				      struct al_mod_eth_pkt *hal_pkt,
				      unsigned int descs,
				      u16 *next_to_clean)
{
	struct sk_buff *skb;
	struct al_mod_eth_rx_buffer *rx_info =
		&rx_ring->rx_buffer_info[*next_to_clean];
	u8 *va = rx_info->data + AL_ETH_RX_OFFSET;
	unsigned int len, orig_len;
	unsigned int buf = 0;

	len = hal_pkt->bufs[0].len;
	netdev_dbg(adapter->netdev, "rx_info %p data %p\n", rx_info,
		   rx_info->data);

	prefetch(va);

	if (len <= adapter->small_copy_len) {
		netdev_dbg(adapter->netdev, "rx small packet. len %d\n", len);

		skb = netdev_alloc_skb_ip_align(adapter->netdev,
				adapter->small_copy_len);
		if (unlikely(!skb)) {
			u64_stats_update_begin(&rx_ring->syncp);
			rx_ring->rx_stats.skb_alloc_fail++;
			u64_stats_update_end(&rx_ring->syncp);
			return NULL;
		}

		/* Need to use the same length when using the dma_* APIs */
		orig_len = len;

		pci_dma_sync_single_for_cpu(adapter->pdev, rx_info->dma,
					    orig_len, DMA_FROM_DEVICE);
		if (hal_pkt->source_vlan_count > 0)
			/* This function alters len when the packet has a vlan tag */
			al_mod_eth_vlan_rx_linear(adapter, skb, &va, &len);

		skb_copy_to_linear_data(skb, va, len);

		pci_dma_sync_single_for_device(adapter->pdev, rx_info->dma,
				orig_len, DMA_FROM_DEVICE);

		skb_put(skb, len);
		skb->protocol = eth_type_trans(skb, adapter->netdev);
		*next_to_clean = AL_ETH_RX_RING_IDX_NEXT(rx_ring, *next_to_clean);
		rx_ring->bytes += orig_len;
		return skb;
	}

	dma_unmap_single(rx_ring->dev, dma_unmap_addr(rx_info, dma),
			 rx_info->data_size, DMA_FROM_DEVICE);
#if 0
	skb = build_skb(rx_info->data, rx_ring->frag_size);
	if (unlikely(!skb))
		return NULL;
#else
	skb = napi_get_frags(rx_ring->napi);
	if (unlikely(!skb)) {
		u64_stats_update_begin(&rx_ring->syncp);
		rx_ring->rx_stats.skb_alloc_fail++;
		u64_stats_update_end(&rx_ring->syncp);
		return NULL;
	}

	skb_fill_page_desc(skb, skb_shinfo(skb)->nr_frags,
				rx_info->page,
				rx_info->page_offset + AL_ETH_RX_OFFSET, len);

	skb->len += len;
	skb->data_len += len;
	skb->truesize += len;
	/** Increase adaptive threshold (relevent only when adapter->adaptive_intr_rate is set */
	rx_ring->bytes += len;
#endif
#if 0
	skb_reserve(skb, AL_ETH_RX_OFFSET);
	skb_put(skb, len);
#endif
	al_mod_eth_vlan_rx_frag(adapter, skb, va, NULL);

	netdev_dbg(adapter->netdev, "rx skb updated. len %d. data_len %d\n",
				skb->len, skb->data_len);

	rx_info->data = NULL;
	*next_to_clean = AL_ETH_RX_RING_IDX_NEXT(rx_ring, *next_to_clean);

	while (--descs) {
		rx_info = &rx_ring->rx_buffer_info[*next_to_clean];
		len = hal_pkt->bufs[++buf].len;
		rx_ring->bytes += len;

		dma_unmap_single(rx_ring->dev, dma_unmap_addr(rx_info, dma),
				 rx_info->data_size, DMA_FROM_DEVICE);

		skb_add_rx_frag(skb, skb_shinfo(skb)->nr_frags,
				rx_info->page,
				rx_info->page_offset + AL_ETH_RX_OFFSET,
				len, rx_info->data_size);

		netdev_dbg(adapter->netdev, "rx skb updated. len %d. data_len %d\n",
			skb->len, skb->data_len);

		rx_info->data = NULL;

		*next_to_clean = AL_ETH_RX_RING_IDX_NEXT(rx_ring, *next_to_clean);
	}

	return skb;
}
#elif defined(CONFIG_AL_ETH_ALLOC_SKB)
static	struct sk_buff *al_mod_eth_rx_skb(struct al_mod_eth_adapter *adapter,
				      struct al_mod_eth_ring *rx_ring,
				      struct al_mod_eth_pkt *hal_pkt,
				      unsigned int descs,
				      u16 *next_to_clean)
{
	struct sk_buff *skb;
	struct al_mod_eth_rx_buffer *rx_info =
		&rx_ring->rx_buffer_info[*next_to_clean];
	unsigned int len;

	prefetch(rx_info->data + AL_ETH_RX_OFFSET);
	skb = rx_info->skb;
	prefetch(skb);
	prefetch(&skb->end);
	prefetch(&skb->dev);

	len = hal_pkt->bufs[0].len;

	dma_unmap_single(rx_ring->dev, dma_unmap_addr(rx_info, dma),
			rx_info->data_size, DMA_FROM_DEVICE);

	skb_reserve(skb, AL_ETH_RX_OFFSET);
	skb_put(skb, len);

	skb->protocol = eth_type_trans(skb, adapter->netdev);
	rx_info->skb = NULL;
	*next_to_clean = AL_ETH_RX_RING_IDX_NEXT(rx_ring, *next_to_clean);
	/* prefetch next packet */
	prefetch((rx_info + 1)->data + AL_ETH_RX_OFFSET);
	prefetch((rx_info + 1)->skb);

	return skb;
}
#endif

/*
 * This function assumes that after a BACKUP command, the following call is a RESTORE
 * (and vice versa). If the function is invoked more then once with the same command,
 * only the first invocation will actually backup (or restore).
 * The main usecase is for backing up the necessary HW entries before an FLR, and then
 * restoring it (which is done al_mod_eth_config_rx_fwd()).
 * This behavior handles the case where al_mod_eth_function_reset() is invoked twice without calling
 * al_mod_eth_config_rx_fwd() in between, which would end in data loss
 */
static void al_mod_eth_user_def_rx_flow_steering_backup(struct al_mod_eth_adapter *adapter,
						    enum al_mod_eth_user_def_backup_command command)
{
	struct al_mod_ec_regs __iomem *ec_regs_base = adapter->hal_adapter.ec_regs_base;
	int i;

	if (command == AL_ETH_USER_DEF_BACKUP) {
		if ((adapter->user_def_flow_steering_restored == AL_TRUE) &&
		    (al_mod_eth_flow_steer_user_def_exists(adapter) ||
		     (adapter->udma_steer_mask != 0x0) ||
		     (adapter->queue_steer_id != -1))) {
			/* save user defined EPE entries */
			for (i = 0; i < AL_ETH_EPE_ENTRIES_FREE_NUM; i++) {
				al_mod_eth_epe_entry_get(&adapter->hal_adapter,
						     (i + (AL_ETH_EPE_ENTRIES_NUM - 1)),
						     &adapter->user_def_epe_reg_entry[i],
						     &adapter->user_def_epe_control_entry[i],
						     0);
			}

			/*
			 * save default_reg confgurations of UDMA and queues forwarding, so we
			 * can restore any user-defined rules
			 */
			for (i = 0; i < 8; i++) { /* TODO: add define for this in HAL */
				adapter->rfw_default_backup[i] =
					al_mod_reg_read32(&ec_regs_base->rfw_default[i].opt_1);
			}

			/* save rx_ctrl_table so we can restore any user-defined rules */
			for (i = 0; i < AL_ETH_RX_CTRL_TABLE_SIZE; i++) {
				al_mod_eth_ctrl_table_raw_get(&adapter->hal_adapter,
							  i,
							  &adapter->rx_ctrl_table_backup[i]);
			}

			/* save the default ctrl entry in case udma/queue override is configured */
			al_mod_eth_ctrl_table_def_raw_get(&adapter->hal_adapter,
						      &adapter->rx_ctrl_table_def_backup);

			adapter->user_def_flow_steering_restored = AL_FALSE;
		}
	} else if (command == AL_ETH_USER_DEF_RESTORE) {
		if ((adapter->user_def_flow_steering_restored == AL_FALSE) &&
		    (al_mod_eth_flow_steer_user_def_exists(adapter) ||
		     (adapter->udma_steer_mask != 0x0) ||
		     (adapter->queue_steer_id != -1))) {
			for (i = 0; i < (AL_ETH_EPE_ENTRIES_FREE_NUM); i++) {
				al_mod_eth_epe_entry_set(&adapter->hal_adapter,
						(i + (AL_ETH_EPE_ENTRIES_NUM - 1)),
						&adapter->user_def_epe_reg_entry[i],
						&adapter->user_def_epe_control_entry[i]);
			}

			for (i = 0; i < 8; i++) { /* TODO: add define for this in HAL */
				al_mod_reg_write32(&ec_regs_base->rfw_default[i].opt_1,
					       adapter->rfw_default_backup[i]);
			}

			for (i = 0; i < AL_ETH_RX_CTRL_TABLE_SIZE; i++) {
				al_mod_eth_ctrl_table_raw_set(&adapter->hal_adapter,
							  i,
							  adapter->rx_ctrl_table_backup[i]);
			}

			al_mod_eth_epe_entry_set(&adapter->hal_adapter,
					     AL_ETH_PROTO_ID_UDP,
					     &parser_reg_udp_dis_end_of_parse_entry,
					     &parser_control_udp_dis_end_of_parse_entry);

			al_mod_eth_ctrl_table_def_raw_set(&adapter->hal_adapter,
						      adapter->rx_ctrl_table_def_backup);


			adapter->user_def_flow_steering_restored = AL_TRUE;
			netdev_dbg(adapter->netdev, "%s: Restored user defined Flow steering after FLR\n",
				   __func__);
		}
	} else {
		netdev_err(adapter->netdev, "%s: Unknown command! (%d)\n", __func__, command);
	}

}

/* configure the RX forwarding (UDMA/QUEUE.. selection)
 * we use the full control table, instead of using only the default configuration entry
 */

static void
al_mod_eth_config_rx_fwd(struct al_mod_eth_adapter *adapter)
{
	struct al_mod_eth_fwd_ctrl_table_index ctrl_table_index = {0};
	struct al_mod_eth_fwd_ctrl_table_entry entry;
	int i;

	/* let priority be equal to pbits */
	for (i = 0; i < AL_ETH_FWD_PBITS_TABLE_NUM; i++)
		al_mod_eth_fwd_pbits_table_set(&adapter->hal_adapter, i, i);

	/* map priority to queue index, queue id = priority/2 */
	for (i = 0; i < AL_ETH_FWD_PRIO_TABLE_NUM; i++)
		al_mod_eth_fwd_priority_table_set(&adapter->hal_adapter, i, i >> 1);

	ctrl_table_index.vlan_table_out = AL_ETH_FWD_CTRL_IDX_VLAN_TABLE_OUT_ANY;
	ctrl_table_index.tunnel_exist = AL_ETH_FWD_CTRL_IDX_TUNNEL_ANY;
	ctrl_table_index.vlan_exist = AL_ETH_FWD_CTRL_IDX_VLAN_ANY;
	ctrl_table_index.mac_table_match = AL_ETH_FWD_CTRL_IDX_MAC_TABLE_ANY;
	ctrl_table_index.protocol_id = AL_ETH_PROTO_ID_ANY;
	ctrl_table_index.mac_type = AL_ETH_FWD_CTRL_IDX_MAC_DA_TYPE_ANY;

	entry.prio_sel = AL_ETH_CTRL_TABLE_PRIO_SEL_VAL_0;
	entry.queue_sel_1 = AL_ETH_CTRL_TABLE_QUEUE_SEL_1_THASH_TABLE;
	entry.queue_sel_2 = AL_ETH_CTRL_TABLE_QUEUE_SEL_2_NO_PRIO;
	entry.udma_sel = AL_ETH_CTRL_TABLE_UDMA_SEL_MAC_TABLE;
	entry.filter = AL_FALSE;

#ifdef AL_ETH_PLAT_EMU
	entry.udma_sel = AL_ETH_CTRL_TABLE_UDMA_SEL_REG1; /** where to take UDMA number from */
	al_mod_eth_fwd_default_udma_config(&adapter->hal_adapter, 0, 1 << adapter->udma_num);
	al_mod_eth_ctrl_table_def_set(&adapter->hal_adapter, AL_FALSE, &entry);
#else
	al_mod_eth_ctrl_table_def_set(&adapter->hal_adapter, AL_TRUE, &entry);
	/* set steering control table entries: */
	al_mod_eth_ctrl_table_set(&adapter->hal_adapter,
			      &ctrl_table_index,
			      &entry);
#endif
	/*
	 * By default set the mac table to forward all unicast packets to our
	 * MAC address and all broadcast. all the rest will be dropped.
	 */
	al_mod_eth_mac_table_unicast_add(adapter, AL_ETH_MAC_TABLE_UNICAST_IDX_BASE,
				     adapter->mac_addr,
				     1 << adapter->default_rx_udma_num);
	al_mod_eth_mac_table_broadcast_add(adapter,
				       AL_ETH_MAC_TABLE_BROADCAST_IDX,
				       1 << adapter->default_rx_udma_num);

#ifdef AL_ETH_PLAT_EMU
	al_mod_eth_mac_table_promiscuous_set(adapter, true, 1 << adapter->udma_num);
#else
	/* Honor the netdev flags */
	al_mod_eth_mac_table_promiscuous_set(adapter, !!(adapter->netdev->flags & IFF_PROMISC), 1 << adapter->udma_num);
#endif

	/* set toeplitz hash keys */
	get_random_bytes(adapter->toeplitz_hash_key,
			 sizeof(adapter->toeplitz_hash_key));

	for (i = 0; i < AL_ETH_RX_HASH_KEY_NUM; i++)
		al_mod_eth_hash_key_set(&adapter->hal_adapter, i,
				    htonl(adapter->toeplitz_hash_key[i]));

	for (i = 0; i < adapter->rss_ind_tbl_size; i++)
		al_mod_eth_thash_table_set(&adapter->hal_adapter,
				       i,
				       adapter->default_rx_udma_num,
				       adapter->rss_ind_tbl[i]);

	al_mod_eth_fsm_table_init(adapter);

	al_mod_eth_user_def_rx_flow_steering_backup(adapter, AL_ETH_USER_DEF_RESTORE);
}

static void
al_mod_eth_config_rx_fwd_adv(struct al_mod_eth_adapter *adapter)
{
	int rc;
	struct al_mod_eth_rfw_lrss_params lrss_params = {
			.udma_num = adapter->default_rx_udma_num,
			.table_size = adapter->rss_ind_tbl_size,
			.indir_table = (uint8_t *)adapter->rss_ind_tbl,
	};

	rc = al_mod_eth_rfw_lrss_generic_config(&adapter->rfw_handle, &lrss_params);
	if (rc != 0)
		netdev_err(adapter->netdev,
			"%s: Error on al_mod_eth_rfw_lrss_generic_config (rc = %d)", __func__, rc);
}

#ifdef CONFIG_PHYLIB
/* MDIO */
static int al_mod_mdio_read(struct mii_bus *bp, int mii_id, int reg)
{
	struct al_mod_eth_adapter *adapter;
	struct al_mod_mdio_bus_shared_data *data = bp->priv;
	u16 value = 0;
	int rc;
	int timeout = MDIO_TIMEOUT_MSEC;

	al_mod_eth_mdio_lock(bp);
	adapter = data->mac_active;

	while (timeout > 0) {
		if (reg & MII_ADDR_C45) {
			/** Clause 45 */
			al_mod_dbg("%s [c45]: dev %x reg %x val %x\n",
				__func__,
				((reg & AL_ETH_MDIO_C45_DEV_MASK) >> AL_ETH_MDIO_C45_DEV_SHIFT),
				(reg & AL_ETH_MDIO_C45_REG_MASK), value);
			rc = al_mod_eth_mdio_read(&adapter->hal_adapter, mii_id,
				((reg & AL_ETH_MDIO_C45_DEV_MASK) >> AL_ETH_MDIO_C45_DEV_SHIFT),
				(reg & AL_ETH_MDIO_C45_REG_MASK), &value);
		} else {
			/** Clause 22 */
			rc = al_mod_eth_mdio_read(&adapter->hal_adapter, mii_id,
					      MDIO_DEVAD_NONE, reg, &value);
		}

		if (rc == 0) {
			rc = value;
			break;
		}

		netdev_dbg(adapter->netdev,
			   "mdio read failed. try again in 10 msec\n");

		timeout -= 10;
		msleep(10);
	}

	if (rc < 0)
		netdev_warn(adapter->netdev, "MDIO read failed on timeout\n");

	al_mod_eth_mdio_unlock(bp);

	return rc;
}

static int
al_mod_mdio_write(struct mii_bus *bp, int mii_id, int reg, u16 val)
{
	struct al_mod_eth_adapter *adapter;
	struct al_mod_mdio_bus_shared_data *data = bp->priv;
	int rc;
	int timeout = MDIO_TIMEOUT_MSEC;

	al_mod_eth_mdio_lock(bp);
	adapter = data->mac_active;

	while (timeout > 0) {
		if (reg & MII_ADDR_C45) {
			al_mod_dbg("%s [c45]: device %x reg %x val %x\n",
				__func__,
				((reg & AL_ETH_MDIO_C45_DEV_MASK) >> AL_ETH_MDIO_C45_DEV_SHIFT),
				(reg & AL_ETH_MDIO_C45_REG_MASK), val);
			rc = al_mod_eth_mdio_write(&adapter->hal_adapter, mii_id,
				((reg & AL_ETH_MDIO_C45_DEV_MASK) >> AL_ETH_MDIO_C45_DEV_SHIFT),
				(reg & AL_ETH_MDIO_C45_REG_MASK), val);
		} else {
			rc = al_mod_eth_mdio_write(&adapter->hal_adapter, mii_id,
				       MDIO_DEVAD_NONE, reg, val);
		}

		if (rc == 0)
			break;

		netdev_err(adapter->netdev,
			   "mdio write failed. try again in 10 msec\n");

		timeout -= 10;
		msleep(10);
	}


	if (rc)
		netdev_err(adapter->netdev, "MDIO write failed on timeout\n");

	al_mod_eth_mdio_unlock(bp);

	return rc;
}

/**
 * al_mod_eth_mdiobus_teardown - mdiobus unregister
 *
 *
 **/
static void al_mod_eth_mdiobus_teardown(struct al_mod_eth_adapter *adapter){

	struct al_mod_mdio_bus_shared_data *data;
	struct al_mod_mdio_bus_adapter *bus_adapter, *bus_adapter_temp;
	bool found = false;
	bool free_mdio = false;

	if (AL_FALSE == adapter->phy_exist || !adapter->mdio_bus)
		return;

	data = adapter->mdio_bus->priv;

	al_mod_eth_mdio_lock(adapter->mdio_bus);

	list_for_each_entry_safe(bus_adapter, bus_adapter_temp, &data->adapter_list, list) {
		list_del(&bus_adapter->list);
		kfree(bus_adapter);
		found = true;
		break;
	}

	if(!found) {
		netdev_warn(adapter->netdev, "al_mod_eth_mdiobus_teardown failed -> adapter has not been found in mdio_adapter list\n");
		al_mod_eth_mdio_unlock(adapter->mdio_bus);
		return;
	}

	data->use_counter--;

	/**
	 * mdiobus_unregister will free all phys for us later
	 */
	adapter->phydev = NULL;

	if (data->use_counter) {
		/* data->use_counter > 0 -> list is not empty */
		if (data->mac_active == adapter) {
			/* Take the first adapter in the list.*/
			bus_adapter = list_first_entry(&data->adapter_list, struct al_mod_mdio_bus_adapter, list);
			data->mac_active = bus_adapter->adapter;
		}
	}
	else
		free_mdio = true;

	al_mod_eth_mdio_unlock(adapter->mdio_bus);

	if (free_mdio) {
		mdiobus_unregister(adapter->mdio_bus);
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 5, 0)
		kfree(adapter->mdio_bus->irq);
#endif
		mdiobus_free(adapter->mdio_bus);
		adapter->mdio_bus = NULL;
	}
}

static int al_mod_eth_mdiobus_hw_init(struct al_mod_eth_adapter *adapter){

	int ret = 0;
	enum al_mod_eth_mdio_type mdio_type;

	if (AL_FALSE == adapter->phy_exist) {
		return 0;
	}

	if (adapter->phy_if == AL_ETH_BOARD_PHY_IF_XMDIO) {
#ifndef CONFIG_ARCH_ALPINE
		/** Host driver doesnt support xmdio */
		return -EOPNOTSUPP;
#else
		mdio_type = AL_ETH_MDIO_TYPE_CLAUSE_45;
#endif
	} else
		mdio_type = AL_ETH_MDIO_TYPE_CLAUSE_22;

	ret = al_mod_eth_mdio_config(&adapter->hal_adapter, mdio_type,
		AL_TRUE/*shared_mdio_if*/,
		adapter->ref_clk_freq, adapter->mdio_freq);
	if (ret) {
		netdev_err(adapter->netdev, "%s failed at mdio config!\n", __func__);
	}
	return ret;
}

/**
 * al_mod_eth_mdiobus_setup - initialize mdiobus and register to kernel
 *
 *
 **/
static int al_mod_eth_mdiobus_setup(struct al_mod_eth_adapter *adapter)
{
	struct phy_device *phydev;
	struct al_mod_mdio_bus_shared_data *data;
	struct al_mod_mdio_bus_adapter* bus_adapter;
	bool is_c45 = (adapter->phy_if == AL_ETH_BOARD_PHY_IF_XMDIO) ? true : false;

	int i;
	int ret;

	if (AL_FALSE == adapter->phy_exist) {
		return 0;
	}

	adapter->mdio_bus = mdiobus_find_by_name(MDIO_BUS_SHARED_NAME);
	if (adapter->mdio_bus == NULL) {
		adapter->mdio_bus = mdiobus_alloc();
		if (adapter->mdio_bus == NULL)
			return -ENOMEM;

		data = kzalloc(sizeof(*data), GFP_KERNEL);
		if (data == NULL) {
			mdiobus_free(adapter->mdio_bus);
			return -ENOMEM;
		}

		data->mac_active = adapter;
		data->use_counter = 0;
		INIT_LIST_HEAD(&data->adapter_list);
		adapter->mdio_bus->name = MDIO_BUS_SHARED_NAME;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 26)
		adapter->mdio_bus->id = (adapter->pdev->bus->number << 8) | adapter->pdev->devfn;
#else
		snprintf(adapter->mdio_bus->id, MII_BUS_ID_SIZE, "%s_%x", MDIO_BUS_SHARED_NAME,
			 (adapter->pdev->bus->number << 8) | adapter->pdev->devfn);
#endif
		adapter->mdio_bus->priv = data;
		al_mod_eth_mdio_lock_init(adapter->mdio_bus);

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 28)
		adapter->mdio_bus->dev = &adapter->pdev->dev;
#else
		adapter->mdio_bus->parent = &adapter->pdev->dev;
#endif
		adapter->mdio_bus->read = &al_mod_mdio_read;
		adapter->mdio_bus->write = &al_mod_mdio_write;
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 5, 0)
		adapter->mdio_bus->irq = kmalloc(sizeof(int) * PHY_MAX_ADDR, GFP_KERNEL);

		if (!adapter->mdio_bus->irq) {
			mdiobus_free(adapter->mdio_bus);
			kfree(data);
			return -ENOMEM;
		}

		/* Initialise the interrupts to polling */
		for (i = 0; i < PHY_MAX_ADDR; i++)
			adapter->mdio_bus->irq[i] = PHY_POLL;
#endif /* LINUX_VERSION_CODE < KERNEL_VERSION(4, 5, 0) */

#if defined(CONFIG_ARCH_ALPINE)
		/**
		 * Some switch chips doesn't support direct access to MII registers, we have to avoid
		 * auto probing. Mask out all devices and register it without PHY auto probing.
		 */
		adapter->mdio_bus->phy_mask = 0xffffffff;
		i = mdiobus_register(adapter->mdio_bus);
		if (i) {
			netdev_warn(adapter->netdev, "mdiobus_reg failed (0x%x)\n", i);
			mdiobus_free(adapter->mdio_bus);
			return i;
		}
#else
		ret = -EOPNOTSUPP;
		return ret;
#endif
	} else {
		data = (struct al_mod_mdio_bus_shared_data *)adapter->mdio_bus->priv;
	}

	/**
	 * @brief Get & register PHY device
	 */
	phydev = get_phy_device(adapter->mdio_bus, adapter->phy_addr, is_c45);
	if (IS_ERR(phydev) || NULL == phydev) {
		netdev_info(adapter->netdev,
				"%s: cannot get phy device - auto probing failed.\n", __func__);

		/* NOTE: There should probably be a list with all fake IDs and "native" support of
			* probing these fake IDs in PHY layer. For the simplicity keep it here and create
			* phy device directly.
			* */
		phydev = phy_device_create(adapter->mdio_bus, adapter->phy_addr,
						PHY_FAKE_ID_RTL8370, is_c45, NULL);

		if (IS_ERR(phydev) || NULL == phydev) {
			goto error;
		}
	}

	ret = phy_device_register(phydev);
	if (ret) {
		netdev_err(adapter->netdev, "%s: phy device register failed\n", __func__);
		goto error_xmdio_phy;
	}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 5, 0)
	if (!phydev) {
#else /* LINUX_VERSION_CODE >= KERNEL_VERSION(4, 5, 0) */
	if (!phydev || !phydev->drv) {
#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(4, 5, 0) */
		netdev_err(adapter->netdev, "%s: phy device is NULL\n", __func__);
		goto error;
	}

	/**
	 * @brief Connect addapter to PHY
	 */
	ret = al_mod_eth_phy_init(adapter);
	if (ret) {
		netdev_err(adapter->netdev, "failed to connect PHY!\n");
		return ret;
	}

	al_mod_eth_mdio_lock(adapter->mdio_bus);

	bus_adapter = kmalloc(sizeof(*bus_adapter), GFP_KERNEL);
	if( NULL == bus_adapter ) {
			al_mod_eth_mdio_unlock(adapter->mdio_bus);
			goto error;
	}

	INIT_LIST_HEAD(&bus_adapter->list);
	list_add_tail(&(bus_adapter->list), &(data->adapter_list));

	data->use_counter++;

	al_mod_eth_mdio_unlock(adapter->mdio_bus);

	return 0;

#if defined(CONFIG_ARCH_ALPINE)
error_xmdio_phy:
	phy_device_free(phydev);
#endif
error:
	netdev_warn(adapter->netdev, "No PHY devices\n");

	if( NULL != data && !data->use_counter ) {
		mdiobus_unregister(adapter->mdio_bus);
	#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 5, 0)
		kfree(adapter->mdio_bus->irq);
	#endif /* LINUX_VERSION_CODE < KERNEL_VERSION(4, 5, 0) */
		kfree(data);
		mdiobus_free(adapter->mdio_bus);
		adapter->mdio_bus = NULL;
	}
	return -ENODEV;
}

static void al_mod_eth_adjust_link(struct net_device *dev)
{
	struct al_mod_eth_adapter *adapter = netdev_priv(dev);
	struct al_mod_eth_link_config *link_config = &adapter->link_config;
	struct phy_device *phydev = adapter->phydev;
	enum al_mod_eth_mac_mode mac_mode_needed = AL_ETH_MAC_MODE_RGMII;
	int new_state = 0;
	int force_1000_base_x = false;

	if (phydev->link) {
		if (phydev->duplex != link_config->active_duplex) {
			new_state = 1;
			link_config->active_duplex = phydev->duplex;
		}

		if (phydev->speed != link_config->active_speed) {
			new_state = 1;
			switch (phydev->speed) {
			case SPEED_1000:
			case SPEED_100:
			case SPEED_10:
				if (adapter->mac_mode == AL_ETH_MAC_MODE_RGMII)
					mac_mode_needed = AL_ETH_MAC_MODE_RGMII;
				else
					mac_mode_needed = AL_ETH_MAC_MODE_SGMII;
				break;
			case SPEED_10000:
			case SPEED_2500:
				if (adapter->mac_mode == AL_ETH_MAC_MODE_SGMII_2_5G) {
					mac_mode_needed = AL_ETH_MAC_MODE_SGMII_2_5G;
				} else {
					mac_mode_needed = AL_ETH_MAC_MODE_10GbE_Serial;
				}
				break;
			default:
				if (netif_msg_link(adapter))
					netdev_warn(adapter->netdev,
					"Ack!  Speed (%d) is not 10M/100M/1G/2.5G/10G!",
						phydev->speed);
				break;
			}
			link_config->active_speed = phydev->speed;
		}

		if (!link_config->old_link) {
			new_state = 1;
			link_config->old_link = 1;
		}

		if (new_state) {
			int rc;

			if (adapter->mac_mode != mac_mode_needed) {
				rc = al_mod_eth_rtnl_lock_w_flags_check(adapter);
				if (rc < 0)
					return;
				al_mod_eth_down(adapter);
				adapter->mac_mode = mac_mode_needed;
#ifdef CONFIG_ARCH_ALPINE
				if (link_config->active_speed > 1000)
					al_mod_eth_serdes_mode_set(adapter);
				else {
					al_mod_eth_serdes_mode_set(adapter);
					force_1000_base_x = true;
				}
#else
				if (link_config->active_speed <= 1000)
					force_1000_base_x = true;
#endif /** CONFIG_ARCH_ALPINE */
				al_mod_eth_up(adapter);
				rtnl_unlock();
			}

			if (adapter->mac_mode != AL_ETH_MAC_MODE_10GbE_Serial &&
				adapter->mac_mode != AL_ETH_MAC_MODE_SGMII_2_5G) {

				al_mod_eth_mdio_lock(adapter->mdio_bus);

				/* change the MAC link configuration */
				rc = al_mod_eth_mac_link_config(&adapter->hal_adapter,
						force_1000_base_x,
						link_config->autoneg,
						link_config->active_speed,
						link_config->active_duplex ? AL_TRUE : AL_FALSE);
				if (rc) {
					netdev_warn(adapter->netdev,
					"Failed to config the mac with the new link settings!");
				}

				al_mod_eth_mdio_unlock(adapter->mdio_bus);

			}
		}

		if (link_config->flow_ctrl_supported & AL_ETH_FLOW_CTRL_AUTONEG) {
			uint8_t new_flow_ctrl =
				al_mod_eth_flow_ctrl_mutual_cap_get(adapter);

			if (new_flow_ctrl != link_config->flow_ctrl_active) {
				link_config->flow_ctrl_active = new_flow_ctrl;
				al_mod_eth_flow_ctrl_config(adapter);
			}

		}
	} else if (adapter->link_config.old_link) {
			new_state = 1;
			link_config->old_link = 0;
			link_config->active_duplex = DUPLEX_UNKNOWN;
			link_config->active_speed = SPEED_UNKNOWN;
	}

	if (new_state && netif_msg_link(adapter))
		phy_print_status(phydev);
}

static int al_mod_eth_phy_init(struct al_mod_eth_adapter *adapter)
{
	struct phy_device *phydev;
	phy_interface_t interface;
	int rc = 0;

	switch (adapter->mac_mode) {
	case AL_ETH_MAC_MODE_SGMII:
		interface = PHY_INTERFACE_MODE_SGMII;
		break;
	default:
		interface = PHY_INTERFACE_MODE_RGMII_ID;
		break;
	}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 5, 0)
	phydev = mdiobus_get_phy(adapter->mdio_bus, adapter->phy_addr);
#else /* LINUX_VERSION_CODE >= KERNEL_VERSION(4, 5, 0) */
	phydev = adapter->mdio_bus->phy_map[adapter->phy_addr];
#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(4, 5, 0) */

	adapter->link_config.old_link = 0;
	adapter->link_config.active_duplex = DUPLEX_UNKNOWN;
	adapter->link_config.active_speed = SPEED_UNKNOWN;

	/* Attach the MAC to the PHY. */
#if LINUX_VERSION_CODE > KERNEL_VERSION(3, 9, 0)
	rc = phy_connect_direct(adapter->netdev, phydev, al_mod_eth_adjust_link,
		interface);
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
	rc = phy_connect_direct(adapter->netdev, phydev, al_mod_eth_adjust_link,
			     0, interface);
#else
	rc = phy_connect_direct(adapter->netdev, phydev, al_mod_eth_adjust_link, 0);
#endif
	if (rc) {
		netdev_err(adapter->netdev, "Could not attach to PHY\n");
		return rc;
	}

	netdev_info(adapter->netdev, "phy[%d]: device %s, driver %s\n",
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 5, 0)
		    phydev->mdio.addr,
		    phydev_name(phydev),
#else
		    phydev->addr,
		    dev_name(&phydev->dev),
#endif
		    phydev->drv ? phydev->drv->name : "unknown");

	/* Mask with MAC supported features. */
	phydev->supported &= (PHY_GBIT_FEATURES |
		SUPPORTED_Pause |
		SUPPORTED_Asym_Pause);

	phydev->advertising = phydev->supported;

	netdev_info(adapter->netdev, "phy[%d]:supported %x adv %x\n",
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 5, 0)
		    phydev->mdio.addr,
#else
		    phydev->addr,
#endif
		    phydev->supported,
		    phydev->advertising);

	adapter->phydev = phydev;

	return 0;
}
#endif /* CONFIG_PHYLIB */

static int al_mod_eth_ioctl(struct net_device *netdev, struct ifreq *ifr, int cmd)
{
#if defined(CONFIG_PHYLIB)
	struct al_mod_eth_adapter *adapter = netdev_priv(netdev);
	struct mii_ioctl_data *mdio = if_mii(ifr);

	if (adapter->phy_exist == false)
		return -EOPNOTSUPP;

	netdev_dbg(adapter->netdev, "ioctl: phy id 0x%x, reg 0x%x, val_in 0x%x\n",
			mdio->phy_id, mdio->reg_num, mdio->val_in);

	if (adapter->mdio_bus && adapter->phydev)
		return phy_mii_ioctl(adapter->phydev, ifr, cmd);

	return -EOPNOTSUPP;
#else
	return -EOPNOTSUPP;
#endif
}

static void al_mod_eth_tx_timeout(struct net_device *dev)
{
	struct al_mod_eth_adapter *adapter = netdev_priv(dev);

	u64_stats_update_begin(&adapter->syncp);
	adapter->dev_stats.tx_timeout++;
	u64_stats_update_end(&adapter->syncp);

	if (netif_msg_tx_err(adapter))
		netdev_err(dev, "transmit timed out!!!!\n");
#ifdef AL_ETH_PLAT_EMU
	netdev_info(dev, "tx timeout - skip FLR on emulator\n");
#else
	schedule_work(&adapter->reset_task);
#endif
}

static void al_mod_eth_reset_task(struct work_struct *work)
{
	struct al_mod_eth_adapter *adapter;
	int rc;

	adapter = container_of(work, struct al_mod_eth_adapter, reset_task);
	netdev_err(adapter->netdev, "%s restarting interface\n", __func__);
	/*restart interface*/
	rc = al_mod_eth_rtnl_lock_w_flags_check(adapter);
	if (rc < 0)
		return;
	al_mod_eth_down(adapter);
	al_mod_eth_up(adapter);
	rtnl_unlock();
}

static int al_mod_eth_phys_function_reset(struct al_mod_eth_adapter *adapter,
				      bool force_all_udmas_flr)
{
	uint32_t cfg_reg_store[2];
	int rc = 0;
	int i = 0;

	dev_dbg(&adapter->pdev->dev, "dev_id:%d, rev_id:%d, force_all_udmas_flr:%d\n",
		adapter->dev_id, adapter->rev_id, force_all_udmas_flr);

	if (adapter->rev_id <= AL_ETH_REV_ID_3)
		al_mod_eth_user_def_rx_flow_steering_backup(adapter, AL_ETH_USER_DEF_BACKUP);
	if (IS_ETH_V3_ADV(adapter->rev_id, adapter->dev_id))
		al_mod_eth_user_def_rx_icrc_backup(adapter, AL_ETH_USER_DEF_BACKUP);

	if (adapter->dev_id == AL_ETH_DEV_ID_ADVANCED) {
		/*
		 * save VF_BARS in SR-IOV capability, so VFs can be
		 * accessed after FLR
		 */
		al_mod_eth_read_pci_config(adapter->pdev,
				       AL_PCI_VF_BASE_ADDRESS_0,
				       &cfg_reg_store[i++]);
		al_mod_eth_read_pci_config(adapter->pdev,
				       AL_PCI_VF_BASE_ADDRESS_0 + 4,
				       &cfg_reg_store[i++]);
	}

	{
#ifdef AL_ETH_HAS_FLR_PARAMS
		struct al_mod_eth_flr_params flr_params = {
#ifdef CONFIG_ARCH_ALPINE
			.pci_read_config_u32 = al_mod_eth_read_pci_config,
			.pci_write_config_u32 = al_mod_eth_write_pci_config,
			.handle = (void *)adapter->pdev,
#else
			.pci_read_config_u32 = al_mod_eth_read_pcie_config_nic,
			.pci_write_config_u32 = al_mod_eth_write_pcie_config_nic,
			.handle = (void *)adapter,
#endif
			.udma_num = adapter->udma_num,
		};

		if (force_all_udmas_flr &&
		    IS_ETH_V4_ADV(adapter->rev_id, adapter->dev_id))
			flr_params.type = AL_ETH_FLR_TYPE_ALL_UDMAS;
		else
			flr_params.type = (adapter->rev_id >= AL_ETH_REV_ID_4) ?
					   AL_ETH_FLR_TYPE_UDMA_QUEUES :
					   (adapter->flags & AL_ETH_FLAG_RESET_UDMA_EC) ?
					   AL_ETH_FLR_TYPE_UDMA_EC :
					   AL_ETH_FLR_TYPE_FULL;

		al_mod_eth_flr(&adapter->hal_adapter, &flr_params);
#else
		struct al_mod_eth_board_params params = {0};

		/* save board params so we restore it after reset */
		al_mod_eth_board_params_get_ex(&adapter->hal_adapter, &params);
		al_mod_eth_mac_addr_read(adapter->ec_base, 0, adapter->mac_addr);

		rc = al_mod_eth_flr_rmn(&al_mod_eth_read_pci_config,
				    &al_mod_eth_write_pci_config,
				    adapter->pdev, adapter->mac_base);

		/* restore params */
		al_mod_eth_board_params_set_ex(&adapter->hal_adapter, &params);
		al_mod_eth_mac_addr_store(adapter->ec_base, 0, adapter->mac_addr);
#endif
	}

	if (adapter->dev_id == AL_ETH_DEV_ID_ADVANCED) {
		i = 0;
		al_mod_eth_write_pci_config(adapter->pdev,
					AL_PCI_VF_BASE_ADDRESS_0,
					cfg_reg_store[i++]);
		al_mod_eth_write_pci_config(adapter->pdev,
					AL_PCI_VF_BASE_ADDRESS_0 + 4,
					cfg_reg_store[i++]);
	}

	return rc;
}

static int al_mod_eth_non_phys_function_reset(struct al_mod_eth_adapter *adapter)
{
	uint32_t adapter_pci_cmd;
	int rc = 0;

	/* FLR is different for VF (i.e. udma_num > 0) */
	if (adapter->udma_num) {
		writel(AL_PCI_EXP_DEVCTL_BCR_FLR,
		       adapter->internal_pcie_base + 0x1000 * adapter->udma_num +
		       AL_PCI_EXP_CAP_BASE + AL_PCI_EXP_DEVCTL);
		/* make sure flr is done. TODO: What is the expected value? */
		readl(adapter->internal_pcie_base + 0x1000 * adapter->udma_num + 0x48);
		mdelay(1);
		/* enable master/slave in the adapter conf */
		adapter_pci_cmd = readw(adapter->internal_pcie_base +
					0x1000 * (adapter->udma_num) +
					PCI_COMMAND);
		/*
		 * TODO: check if PCI_COMMAND_MEMORY is relevant, since AFAIK the MSE bit
		 * is Read-only for the VFs (it is controlled by SR-IOV capability in PF)
		 */
		adapter_pci_cmd |= PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER;
		writew(adapter_pci_cmd, adapter->internal_pcie_base +
		       0x1000 * (adapter->udma_num) +
		       PCI_COMMAND);
	} else {
		netdev_err(adapter->netdev, "Trying to FLR in NIC mode for unsupported config. udma_num:%d, board_type: %d\n",
			   adapter->udma_num, adapter->board_type);
		rc = -EOPNOTSUPP;
	}

	return rc;
}

static int
al_mod_eth_function_reset(struct al_mod_eth_adapter *adapter)
{
	int rc = -EINVAL;

	dev_info(&adapter->pdev->dev, "%s: performing FLR\n", __func__);

	if (IS_V3_NIC(adapter->board_type) ||
	    (adapter->board_type == ALPINE_INTEGRATED) ||
	    (adapter->board_type == ALPINE_NIC_V2_1G_TEST) ||
	    (adapter->board_type == ALPINE_NIC_V3_1G_TEST))
		rc = al_mod_eth_phys_function_reset(adapter, false);
	else if (IS_V2_NIC(adapter->board_type))
		rc = al_mod_eth_non_phys_function_reset(adapter);
	else
		dev_err(&adapter->pdev->dev, "%s : Unknown FLR configuration\n", __func__);

	return rc;
}

#ifndef CONFIG_ARCH_ALPINE
#define AL_ETH_V4_ADV_ALL_UDMAS_WA_DELAY 1 /* msec */
/*
 * This function MUST only be invoked in al_mod_eth_reset_workaround_handling()
 * See comment before that function for more details
 */
static int al_mod_eth_v4_all_udmas_reset(struct al_mod_eth_adapter *adapter)
{
	int i, q;
	int rc = 0;
	unsigned int hw_udma_queues_max;
	unsigned int num_of_udmas;
	struct al_mod_eth_common_rx_pipe_modules_ctrl_params rx_pipe_modules_ctrl_params = {0};

	/* 1. EC stop - gen.en (EC registers) */
	rx_pipe_modules_ctrl_params.ec_gen_msw_in_valid = 1;
	rx_pipe_modules_ctrl_params.ec_gen_msw_in = 0;
	al_mod_eth_common_rx_pipe_modules_ctrl(&adapter->hal_eth_common_handle,
					   &rx_pipe_modules_ctrl_params);

	/* 2. Short Delay */
	msleep(AL_ETH_V4_ADV_ALL_UDMAS_WA_DELAY);

	/* 3. Set UDMA to drop mode */
	/* We rely on the FW to perform this for each port (the latest HAL bump
	 * has it in al_mod_eth_adapter_init())
	 * There is still an issue for alpine_mode.
	 * TODO: Maybe add a check to make sure that indeed all UDMAs are
	 *       configured to drop mode
	 */

	/* 4. Short Delay */
	/* Leaving this here for documentation (Not required anymore since we
	 * are relying on the UDMA drop to be configured by the FW)
	 */

	/* 5. Stop prefetch in all RX queues of all UDMAs */
	hw_udma_queues_max = AL_ETH_UDMA_HW_QUEUES_BY_REV(adapter->rev_id);
	num_of_udmas = al_mod_iomap_offset_idx_get(adapter->al_mod_chip_id,
					       AL_IOMAP_ETH_UDMA_NUM,
					       adapter->port_num);
	for (i = 0; i < num_of_udmas; i++) {
		struct al_mod_udma_params udma_params = {0};
		struct al_mod_udma rx_udma;
		char name[32];
		struct udma_regs_base __iomem *udma_regs_base;

		udma_regs_base = adapter->internal_sb_base +
			al_mod_iomap_offset_idx_sub_idx_get(adapter->al_mod_chip_id,
							AL_IOMAP_ETH_OFFSET,
							eth_v4_adv_port_map[i],
							0) +
			al_mod_iomap_offset_idx_sub_idx_get(adapter->al_mod_chip_id,
							AL_IOMAP_ETH_UDMA_OFFSET,
							eth_v4_adv_port_map[i],
							0);
		snprintf(name, sizeof(name), "all_udmas_flr_udma_rx%d", i);

		udma_params.udma_regs_base = udma_regs_base;
		udma_params.type = UDMA_RX;
		udma_params.num_of_queues = hw_udma_queues_max;
		udma_params.name = name;

		rc = al_mod_udma_handle_init(&rx_udma, &udma_params);
		if (rc)
			return rc;

		for (q = 0; q < hw_udma_queues_max; q++) {
			rc = al_mod_udma_q_pause(&rx_udma.udma_q[i]);
			if (rc) {
				dev_err(&adapter->pdev->dev, "Failed at pausing q%d of UDMA%d\n",
					q, i);
				return rc;
			}
		}
	}

	/* 6. Short Delay */
	msleep(AL_ETH_V4_ADV_ALL_UDMAS_WA_DELAY);

	/* 7. Config Masks and Muxes */
	/* This following 2 are done for ALL_UDMAS FLR in al_mod_eth_flr():
	 * - Config FLR selector to take adapter(i)'s PF FLR to ALL udmas
	 * - Mask EC/MAC/Shared bits in the flr control reg
	 * The following is guaranteed due to the default reset value:
	 * - Enable clear ROB when doing UDMA FLR
	 */

	/* 8. ALL_UDMAS FLR */
	al_mod_eth_phys_function_reset(adapter, true);

	/* 9. Short Delay */
	msleep(AL_ETH_V4_ADV_ALL_UDMAS_WA_DELAY);

	/* 10. EC enable - gen.en (EC registers) */
	rx_pipe_modules_ctrl_params.ec_gen_msw_in_valid = 1;
	rx_pipe_modules_ctrl_params.ec_gen_msw_in = 1;
	al_mod_eth_common_rx_pipe_modules_ctrl(&adapter->hal_eth_common_handle,
					   &rx_pipe_modules_ctrl_params);

	return rc;
}

/*
 * In general, there is a HW bug (RMN 11527) which inhibits us from performing
 * a UDMA FLR for a specific port (in a SoC), without affecting all other ports.
 * Thus, we invoke an ALL_UDMAS FLR which resets all UDMAs at once.
 * We can't do this while other ports are up or being probed, so we do
 * this only for the first one and save the state in dedictated regs
 * of the specific SoC.
 * The per-port state is cleared for each port at is removed (either by explicit
 * unbinding or as part of driver unload).
 * Once all ports have been removed, the next time a port (the "first" in this
 * iteration) is probed, the flow will be performed again.
 *
 * This function must be called in al_mod_eth_probe() and al_mod_eth_remove() only,
 * and only once in each.
 */
static int al_mod_eth_reset_workaround_handling(struct al_mod_eth_adapter *adapter,
					    bool probe)
{
	struct al_mod_eth_common_gen_storage_params params = {0};
	int rc = 0;
	static DEFINE_SPINLOCK(reset_wa_handle_lock);

	dev_dbg(&adapter->pdev->dev, "%s: Entered\n", __func__);

	/* obtain lock here */
	spin_lock(&reset_wa_handle_lock);

	al_mod_eth_common_gen_storage_params_get(&adapter->hal_eth_common_handle,
					     &params);

	if (probe) {
		if (params.eth_func_probe_state == 0) {
			/*
			 * This port is the first to be probed on this SoC,
			 * so we run the ALL_UDMAS FLR workaround flow
			 */
			struct al_mod_unit_adapter unit_adapter;
			uint32_t reg;
			uint32_t check_bits;

			rc = al_mod_unit_adapter_handle_init(&unit_adapter,
							 AL_UNIT_ADAPTER_TYPE_ETH,
							 NULL,
							 al_mod_eth_read_pcie_config_nic,
							 al_mod_eth_write_pcie_config_nic,
							 al_mod_eth_pcie_flr_nic,
							 (void *)adapter);
			if (rc) {
				netdev_err(adapter->netdev,
					   "%s: Failed to initialize unit_adapter (rc=%d)!\n",
					   __func__,
					   rc);
				goto err_spin_unlock;
			}

			unit_adapter.pcie_read_config_u32(adapter,
					     AL_ADAPTER_GENERIC_CONTROL_0,
					     &reg);
			check_bits = reg &
				(AL_ADAPTER_GENERIC_CONTROL_0_ETH_V4_EC_RST_EN |
				 AL_ADAPTER_GENERIC_CONTROL_0_ETH_V4_EC_REG_RST_EN |
				 AL_ADAPTER_GENERIC_CONTROL_0_ETH_V4_MAC_RST_EN |
				 AL_ADAPTER_GENERIC_CONTROL_0_ETH_V4_SCACHE_RST_EN |
				 AL_ADAPTER_GENERIC_CONTROL_0_ETH_V4_SRESOURCE_RST_EN);
			if (check_bits) {
				dev_warn(&adapter->pdev->dev,
					 "Skipping ALL_UDMAS Reset - reg:0x%08x (most likley that FW version doesn't support this feature)\n",
					 reg);
			} else {
				rc = al_mod_eth_v4_all_udmas_reset(adapter);
				if (rc)
					goto err_spin_unlock;
				dev_info(&adapter->pdev->dev, "Performed ALL_UDMAS Reset\n");
			}
		}

		params.eth_func_probe_state |=
			1 << PCI_FUNC(adapter->pdev->devfn);
	} else {
		params.eth_func_probe_state &=
			~(1 << PCI_FUNC(adapter->pdev->devfn));
	}

	al_mod_eth_common_gen_storage_params_set(&adapter->hal_eth_common_handle,
					     &params);
err_spin_unlock:
	/* release lock here */
	spin_unlock(&reset_wa_handle_lock);

	dev_dbg(&adapter->pdev->dev, "%s: Exited\n", __func__);

	return rc;
}
#endif /* CONFIG_ARCH_ALPINE */

#ifndef AL_ETH_PLAT_EMU
static int
al_mod_eth_init_rings_handle(struct al_mod_eth_adapter *adapter)
{
	struct al_mod_hal_eth_adapter *hal_adapter = &adapter->hal_adapter;
	int rc;
	int i;

	/* init queue structure fields required for udma_q_reset */
	for (i = 0; i < adapter->num_tx_queues; i++) {
		struct al_mod_eth_ring *ring = &adapter->tx_ring[i];

		rc = al_mod_udma_q_handle_get(&hal_adapter->tx_udma, i, &ring->dma_q);
		if (rc != 0) {
			al_mod_err("%s: Failed at al_mod_udma_q_handle_get (tx_udma, rc = %d)\n",
					__func__, rc);
			return rc;
		}
		ring->dma_q->q_regs = (union udma_q_regs __iomem *)
				&hal_adapter->tx_udma.udma_regs->m2s.m2s_q[i];
		ring->dma_q->qid = i;
		ring->dma_q->udma = &hal_adapter->tx_udma;
	}

	for (i = 0; i < adapter->num_rx_queues; i++) {
		struct al_mod_eth_ring *ring = &adapter->rx_ring[i];

		rc = al_mod_udma_q_handle_get(&hal_adapter->rx_udma, i, &ring->dma_q);
		if (rc != 0) {
			al_mod_err("%s: Failed at al_mod_udma_q_handle_get (rx_udma, rc = %d)\n",
					__func__, rc);
			return rc;
		}
		ring->dma_q->q_regs = (union udma_q_regs __iomem *)
				&hal_adapter->rx_udma.udma_regs->s2m.s2m_q[i];
		ring->dma_q->qid = i;
		ring->dma_q->udma = &hal_adapter->rx_udma;
	}

	return 0;
}
#endif /* AL_ETH_PLAT_EMU */

static void
al_mod_eth_init_rings(struct al_mod_eth_adapter *adapter)
{
	int i;
	uint sb_clk_freq =  AL_ETH_REF_CLK_FREQ_TO_HZ(adapter->ref_clk_freq) / 1000;

	for (i = 0; i < adapter->num_tx_queues; i++) {
		struct al_mod_eth_ring *ring = &adapter->tx_ring[i];

		ring->dev = &adapter->pdev->dev;
		ring->netdev = adapter->netdev;
		al_mod_udma_q_handle_get(&adapter->hal_adapter.tx_udma, i, &ring->dma_q);
		ring->dma_q->udma = &adapter->hal_adapter.tx_udma;
		ring->sw_count = adapter->tx_ring_count;
		ring->hw_count = adapter->tx_descs_count;
		ring->unmask_reg_offset = al_mod_udma_iofic_unmask_offset_get_adv(
						&adapter->hal_adapter.tx_udma,
						AL_UDMA_IOFIC_LEVEL_PRIMARY,
						AL_INT_GROUP_C);
		ring->unmask_val = ~(1 << i);

		ring->moderation_table_indx = AL_ETH_INTR_MODERATION_LOWEST;
		ring->smoothed_interval = AL_ETH_INTR_LOWEST_VALUE(sb_clk_freq);
		ring->packets = 0;
		ring->bytes = 0;
		u64_stats_init(&ring->syncp);
	}

	for (i = 0; i < adapter->num_rx_queues; i++) {
		struct al_mod_eth_ring *ring = &adapter->rx_ring[i];

		ring->dev = &adapter->pdev->dev;
		ring->netdev = adapter->netdev;
		ring->napi = &adapter->al_mod_napi[AL_ETH_RXQ_NAPI_IDX(adapter,
				i)].napi;
		al_mod_udma_q_handle_get(&adapter->hal_adapter.rx_udma, i, &ring->dma_q);
		ring->dma_q->udma = &adapter->hal_adapter.rx_udma;
		ring->sw_count = adapter->rx_ring_count;
		ring->hw_count = adapter->rx_descs_count;
		ring->unmask_reg_offset = al_mod_udma_iofic_unmask_offset_get_adv(
						&adapter->hal_adapter.rx_udma,
						AL_UDMA_IOFIC_LEVEL_PRIMARY,
						AL_INT_GROUP_B);
		ring->unmask_val = ~(1 << i);

		ring->moderation_table_indx = AL_ETH_INTR_MODERATION_LOWEST;
		ring->smoothed_interval = AL_ETH_INTR_LOWEST_VALUE(sb_clk_freq);
		ring->packets = 0;
		ring->bytes = 0;
		u64_stats_init(&ring->syncp);
	}
}

/**
 * al_mod_eth_setup_tx_resources - allocate Tx resources (Descriptors)
 * @adapter: network interface device structure
 * @qid: queue index
 *
 * Return 0 on success, negative on failure
 **/
static int
al_mod_eth_setup_tx_resources(struct al_mod_eth_adapter *adapter, int qid)
{
	struct al_mod_eth_ring *tx_ring = &adapter->tx_ring[qid];
	struct device *dev = tx_ring->dev;
	struct al_mod_udma_q_params *q_params = &tx_ring->q_params;
	int size;

	size = sizeof(struct al_mod_eth_tx_buffer) * tx_ring->sw_count;

	tx_ring->tx_buffer_info = vzalloc(size);
	if (!tx_ring->tx_buffer_info)
		return -ENOMEM;

	/* TODO: consider ALIGN to page size */
	tx_ring->descs_size = tx_ring->hw_count * sizeof(union al_mod_udma_desc);
	q_params->size = tx_ring->hw_count;

	q_params->desc_base = dma_alloc_coherent(dev,
					tx_ring->descs_size,
					&q_params->desc_phy_base,
					GFP_KERNEL);

	if (!q_params->desc_base)
		return -ENOMEM;

	q_params->cdesc_base = NULL; /* completion queue not used for tx */
	q_params->cdesc_size = 8;

	/* Reset ethtool TX statistics */
	memset(&tx_ring->tx_stats, 0x0, sizeof(tx_ring->tx_stats));

	tx_ring->next_to_use = 0;
	tx_ring->next_to_clean = 0;
	return 0;
}

/**
 * al_mod_eth_free_tx_resources - Free Tx Resources per Queue
 * @adapter: network interface device structure
 * @qid: queue index
 *
 * Free all transmit software resources
 **/
static void
al_mod_eth_free_tx_resources(struct al_mod_eth_adapter *adapter, int qid)
{
	struct al_mod_eth_ring *tx_ring = &adapter->tx_ring[qid];
	struct al_mod_udma_q_params *q_params = &tx_ring->q_params;

	netdev_dbg(adapter->netdev, "%s qid %d\n", __func__, qid);

	vfree(tx_ring->tx_buffer_info);
	tx_ring->tx_buffer_info = NULL;

	/* if not set, then don't free */
	if (!q_params->desc_base)
		return;

	dma_free_coherent(tx_ring->dev, tx_ring->descs_size,
			  q_params->desc_base,
			  q_params->desc_phy_base);

	q_params->desc_base = NULL;
}


/**
 * al_mod_eth_setup_all_tx_resources - allocate all queues Tx resources
 * @adapter: private structure
 *
 * Return 0 on success, negative on failure
 **/
static int
al_mod_eth_setup_all_tx_resources(struct al_mod_eth_adapter *adapter)
{
	int i, rc = 0;

	for (i = 0; i < adapter->num_tx_queues; i++) {
		rc = al_mod_eth_setup_tx_resources(adapter, i);
		if (!rc)
			continue;

		netdev_err(adapter->netdev, "Allocation for Tx Queue %u failed\n", i);
		goto err_setup_tx;
	}

	return 0;
err_setup_tx:
	/* rewind the index freeing the rings as we go */
	while (i--)
		al_mod_eth_free_tx_resources(adapter, i);
	return rc;
}

/**
 * al_mod_eth_free_all_tx_resources - Free Tx Resources for All Queues
 * @adapter: board private structure
 *
 * Free all transmit software resources
 **/
static void
al_mod_eth_free_all_tx_resources(struct al_mod_eth_adapter *adapter)
{
	int i;

	for (i = 0; i < adapter->num_tx_queues; i++)
		if (adapter->tx_ring[i].q_params.desc_base)
			al_mod_eth_free_tx_resources(adapter, i);
}


/**
 * al_mod_eth_setup_rx_resources - allocate Rx resources (Descriptors)
 * @adapter: network interface device structure
 * @qid: queue index
 *
 * Returns 0 on success, negative on failure
 **/
static int
al_mod_eth_setup_rx_resources(struct al_mod_eth_adapter *adapter, unsigned int qid)
{
	struct al_mod_eth_ring *rx_ring = &adapter->rx_ring[qid];
	struct device *dev = rx_ring->dev;
	struct al_mod_udma_q_params *q_params = &rx_ring->q_params;
	int size;

	size = sizeof(struct al_mod_eth_rx_buffer) * rx_ring->sw_count;

	/* alloc extra element so in rx path we can always prefetch rx_info + 1*/
	size += 1;

	rx_ring->rx_buffer_info = vzalloc(size);
	if (!rx_ring->rx_buffer_info)
		return -ENOMEM;

	/* TODO: consider Round up to nearest 4K */
	rx_ring->descs_size = rx_ring->hw_count * sizeof(union al_mod_udma_desc);
	q_params->size = rx_ring->hw_count;

	q_params->desc_base = dma_alloc_coherent(dev, rx_ring->descs_size,
					&q_params->desc_phy_base,
					GFP_KERNEL);
	if (!q_params->desc_base)
		return -ENOMEM;

	if ((IS_ETH_V4_ADV(adapter->rev_id, adapter->dev_id)))
		q_params->cdesc_size = 32;
	else
		q_params->cdesc_size = 16;

	rx_ring->cdescs_size = rx_ring->hw_count * q_params->cdesc_size;
	q_params->cdesc_base = dma_alloc_coherent(dev,
					   rx_ring->cdescs_size,
					   &q_params->cdesc_phy_base,
					   GFP_KERNEL);
	if (!q_params->cdesc_base)
		return -ENOMEM;

	/* Zero out the descriptor ring */
	memset(q_params->cdesc_base, 0, rx_ring->cdescs_size);

	/* Reset ethtool RX statistics */
	memset(&rx_ring->rx_stats, 0x0, sizeof(rx_ring->rx_stats));

	rx_ring->next_to_clean = 0;
	rx_ring->next_to_use = 0;

	return 0;
}

/**
 * al_mod_eth_free_rx_resources - Free Rx Resources
 * @adapter: network interface device structure
 * @qid: queue index
 *
 * Free all receive software resources
 **/
static void
al_mod_eth_free_rx_resources(struct al_mod_eth_adapter *adapter, unsigned int qid)
{
	struct al_mod_eth_ring *rx_ring = &adapter->rx_ring[qid];
	struct al_mod_udma_q_params *q_params = &rx_ring->q_params;

	vfree(rx_ring->rx_buffer_info);
	rx_ring->rx_buffer_info = NULL;

	/* if not set, then don't free */
	if (!q_params->desc_base)
		return;

	dma_free_coherent(rx_ring->dev, rx_ring->descs_size,
			  q_params->desc_base,
			  q_params->desc_phy_base);

	q_params->desc_base = NULL;

	/* if not set, then don't free */
	if (!q_params->cdesc_base)
		return;

	dma_free_coherent(rx_ring->dev, rx_ring->cdescs_size,
			  q_params->cdesc_base,
			  q_params->cdesc_phy_base);

	q_params->cdesc_phy_base = 0;
}

/**
 * al_mod_eth_setup_all_rx_resources - allocate all queues Rx resources
 * @adapter: board private structure
 *
 * Return 0 on success, negative on failure
 **/
static int al_mod_eth_setup_all_rx_resources(struct al_mod_eth_adapter *adapter)
{
	int i, rc = 0;

	for (i = 0; i < adapter->num_rx_queues; i++) {
		rc = al_mod_eth_setup_rx_resources(adapter, i);
		if (!rc)
			continue;

		netdev_err(adapter->netdev, "Allocation for Rx Queue %u failed\n", i);
		goto err_setup_rx;
	}
	return 0;

err_setup_rx:
	/* rewind the index freeing the rings as we go */
	while (i--)
		al_mod_eth_free_rx_resources(adapter, i);
	return rc;
}

/**
 * al_mod_eth_free_all_rx_resources - Free Rx Resources for All Queues
 * @adapter: board private structure
 *
 * Free all receive software resources
 **/
static void al_mod_eth_free_all_rx_resources(struct al_mod_eth_adapter *adapter)
{
	int i;

	for (i = 0; i < adapter->num_rx_queues; i++)
		if (adapter->rx_ring[i].q_params.desc_base)
			al_mod_eth_free_rx_resources(adapter, i);
}

#ifdef CONFIG_AL_ETH_ALLOC_PAGE
static inline int
al_mod_eth_alloc_rx_page(struct al_mod_eth_adapter *adapter,
		     struct al_mod_eth_rx_buffer *rx_info, gfp_t gfp)
{
	struct al_mod_buf *al_mod_buf;
	struct page *page;
	dma_addr_t dma;
	struct al_mod_eth_ring *rx_ring = container_of(&rx_info, struct al_mod_eth_ring, rx_buffer_info);

	/* if previous allocated page is not used */
	if (rx_info->page != NULL)
		return 0;

	page = alloc_page(gfp);
	if (unlikely(!page)) {
		u64_stats_update_begin(&rx_ring->syncp);
		rx_ring->rx_stats.buf_alloc_fail++;
		u64_stats_update_end(&rx_ring->syncp);
		return -ENOMEM;
	}

	dma = dma_map_page(&adapter->pdev->dev, page, 0, PAGE_SIZE,
				DMA_FROM_DEVICE);
	if (unlikely(dma_mapping_error(&adapter->pdev->dev, dma))) {
		u64_stats_update_begin(&rx_ring->syncp);
		rx_ring->rx_stats.dma_mapping_err++;
		u64_stats_update_end(&rx_ring->syncp);

		__free_page(page);
		return -EIO;
	}
	dev_dbg(&adapter->pdev->dev, "alloc page %p, rx_info %p\n",
		page, rx_info);

	rx_info->page = page;
	rx_info->page_offset = 0;
	al_mod_buf = &rx_info->al_mod_buf;
	dma_unmap_addr_set(al_mod_buf, addr, dma);
	dma_unmap_addr_set(rx_info, dma, dma);
	dma_unmap_len_set(al_mod_buf, len, PAGE_SIZE);
	return 0;
}

static void
al_mod_eth_free_rx_page(struct al_mod_eth_adapter *adapter,
		    struct al_mod_eth_rx_buffer *rx_info)
{
	struct page *page = rx_info->page;
	struct al_mod_buf *al_mod_buf = &rx_info->al_mod_buf;

	if (!page)
		return;

	dma_unmap_page(&adapter->pdev->dev, dma_unmap_addr(al_mod_buf, addr),
		       PAGE_SIZE, DMA_FROM_DEVICE);

	__free_page(page);
	rx_info->page = NULL;
}

#elif defined(CONFIG_AL_ETH_ALLOC_FRAG)

static inline int
al_mod_eth_alloc_rx_frag(struct al_mod_eth_adapter *adapter,
		     struct al_mod_eth_ring *rx_ring,
		     struct al_mod_eth_rx_buffer *rx_info)
{
	struct al_mod_buf *al_mod_buf;
	dma_addr_t dma;
	u8 *data;

	/* if previous allocated frag is not used */
	if (rx_info->data != NULL)
		return 0;

	rx_info->data_size = min_t(unsigned int,
				  (rx_ring->netdev->mtu + ETH_HLEN + ETH_FCS_LEN + VLAN_HLEN),
				   adapter->max_rx_buff_alloc_size);

	rx_info->data_size = max_t(unsigned int,
				   rx_info->data_size,
				   AL_ETH_DEFAULT_MIN_RX_BUFF_ALLOC_SIZE);

	rx_info->frag_size = SKB_DATA_ALIGN(rx_info->data_size + AL_ETH_RX_OFFSET) +
			     SKB_DATA_ALIGN(sizeof(struct skb_shared_info));
	data = netdev_alloc_frag(rx_info->frag_size);

	if (!data) {
		u64_stats_update_begin(&rx_ring->syncp);
		rx_ring->rx_stats.buf_alloc_fail++;
		u64_stats_update_end(&rx_ring->syncp);
		return -ENOMEM;
	}

	dma = dma_map_single(rx_ring->dev, data + AL_ETH_RX_OFFSET,
			rx_info->data_size, DMA_FROM_DEVICE);
	if (unlikely(dma_mapping_error(rx_ring->dev, dma))) {
		u64_stats_update_begin(&rx_ring->syncp);
		rx_ring->rx_stats.dma_mapping_err++;
		u64_stats_update_end(&rx_ring->syncp);

		put_page(virt_to_head_page(data));
		return -EIO;
	}
	netdev_dbg(rx_ring->netdev, "alloc frag %p, rx_info %p len %x skb size %x\n",
		data, rx_info, rx_info->data_size, rx_info->frag_size);

	rx_info->data = data;

	BUG_ON(!virt_addr_valid(rx_info->data));
	rx_info->page = virt_to_head_page(rx_info->data);
	rx_info->page_offset = (uintptr_t)rx_info->data -
			       (uintptr_t)page_address(rx_info->page);
	al_mod_buf = &rx_info->al_mod_buf;
	dma_unmap_addr_set(al_mod_buf, addr, dma);
	dma_unmap_addr_set(rx_info, dma, dma);
	dma_unmap_len_set(al_mod_buf, len, rx_info->data_size);
	return 0;
}

static void
al_mod_eth_free_rx_frag(struct al_mod_eth_adapter *adapter,
		    struct al_mod_eth_rx_buffer *rx_info)
{
	u8 *data = rx_info->data;
	struct al_mod_buf *al_mod_buf = &rx_info->al_mod_buf;

	if (!data)
		return;

	dma_unmap_single(&adapter->pdev->dev, dma_unmap_addr(al_mod_buf, addr),
		       rx_info->data_size, DMA_FROM_DEVICE);

	put_page(virt_to_head_page(data));
	rx_info->data = NULL;
}

#elif defined(CONFIG_AL_ETH_ALLOC_SKB)

static inline int
al_mod_eth_alloc_rx_skb(struct al_mod_eth_adapter *adapter,
		     struct al_mod_eth_ring *rx_ring,
		     struct al_mod_eth_rx_buffer *rx_info)
{
	struct sk_buff *skb;
	struct al_mod_buf *al_mod_buf;
	dma_addr_t dma;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 19, 0)
	struct sk_buff_head *rx_rc = this_cpu_ptr(&rx_recycle_cache);
#else
	struct sk_buff_head *rx_rc = &__get_cpu_var(rx_recycle_cache);
#endif

	if (rx_info->skb)
		return 0;

	rx_info->data_size = rx_ring->netdev->mtu + ETH_HLEN + ETH_FCS_LEN + VLAN_HLEN;

	rx_info->data_size = max_t(unsigned int,
				   rx_info->data_size,
				   AL_ETH_DEFAULT_MIN_RX_BUFF_ALLOC_SIZE);

	skb = __skb_dequeue(rx_rc);
	if (skb == NULL)
		skb = __netdev_alloc_skb_ip_align(rx_ring->netdev, rx_info->data_size, GFP_DMA);

	if (!skb) {
		u64_stats_update_begin(&rx_ring->syncp);
		rx_ring->rx_stats.skb_alloc_fail++;
		u64_stats_update_end(&rx_ring->syncp);
		return -ENOMEM;
	}

	dma = dma_map_single(rx_ring->dev, skb->data + AL_ETH_RX_OFFSET,
			rx_info->data_size, DMA_FROM_DEVICE);
	if (unlikely(dma_mapping_error(rx_ring->dev, dma))) {
		u64_stats_update_begin(&rx_ring->syncp);
		rx_ring->rx_stats.dma_mapping_err++;
		u64_stats_update_end(&rx_ring->syncp);

		return -EIO;
	}

	rx_info->data = skb->data;
	rx_info->skb = skb;

	BUG_ON(!virt_addr_valid(rx_info->data));
	al_mod_buf = &rx_info->al_mod_buf;
	dma_unmap_addr_set(al_mod_buf, addr, dma);
	dma_unmap_addr_set(rx_info, dma, dma);
	dma_unmap_len_set(al_mod_buf, len, rx_info->data_size);
	return 0;
}

static void
al_mod_eth_free_rx_skb(struct al_mod_eth_adapter *adapter,
		    struct al_mod_eth_rx_buffer *rx_info)
{
	struct al_mod_buf *al_mod_buf = &rx_info->al_mod_buf;

	if (!rx_info->skb)
		return;

	dma_unmap_single(&adapter->pdev->dev, dma_unmap_addr(al_mod_buf, addr),
		       rx_info->data_size, DMA_FROM_DEVICE);
	dev_kfree_skb_any(rx_info->skb);
	rx_info->skb = NULL;
}

/* the following 3 functions taken from old kernels */
static bool skb_is_recycleable(const struct sk_buff *skb, int skb_size)
{
	if (irqs_disabled())
		return false;

	if (skb_shinfo(skb)->tx_flags & SKBTX_DEV_ZEROCOPY)
		return false;

	if (skb_is_nonlinear(skb) || skb->fclone != SKB_FCLONE_UNAVAILABLE)
		return false;

	skb_size = SKB_DATA_ALIGN(skb_size + NET_SKB_PAD);
	if (skb_end_offset(skb) < skb_size)
		return false;

	if (skb_shared(skb) || skb_cloned(skb))
		return false;

	return true;
}

/**
 *     skb_recycle - clean up an skb for reuse
 *     @skb: buffer
 *
 *     Recycles the skb to be reused as a receive buffer. This
 *     function does any necessary reference count dropping, and
 *     cleans up the skbuff as if it just came from __alloc_skb().
 */
void skb_recycle(struct sk_buff *skb)
{
	struct skb_shared_info *shinfo;

	skb_release_head_state(skb);
	shinfo = skb_shinfo(skb);
	memset(shinfo, 0, offsetof(struct skb_shared_info, dataref));
	atomic_set(&shinfo->dataref, 1);

	memset(skb, 0, offsetof(struct sk_buff, tail));
	skb->data = skb->head + NET_SKB_PAD;
	skb_reset_tail_pointer(skb);
}

/**
 *     skb_recycle_check - check if skb can be reused for receive
 *     @skb: buffer
 *     @skb_size: minimum receive buffer size
 *
 *     Checks that the skb passed in is not shared or cloned, and
 *     that it is linear and its head portion at least as large as
 *     skb_size so that it can be recycled as a receive buffer.
 *     If these conditions are met, this function does any necessary
 *     reference count dropping and cleans up the skbuff as if it
 *     just came from __alloc_skb().
*/
bool skb_recycle_check(struct sk_buff *skb, int skb_size)
{
	if (!skb_is_recycleable(skb, skb_size))
		return false;

	skb_recycle(skb);

	return true;
}
#endif	/* CONFIG_AL_ETH_ALLOC_SKB */

static int
al_mod_eth_refill_rx_bufs(struct al_mod_eth_adapter *adapter, unsigned int qid,
		      unsigned int num)
{
	struct al_mod_eth_ring *rx_ring = &adapter->rx_ring[qid];
	u16 next_to_use;
	unsigned int i;

	next_to_use = rx_ring->next_to_use;

	for (i = 0; i < num; i++) {
		int rc;
		struct al_mod_eth_rx_buffer *rx_info = &rx_ring->rx_buffer_info[next_to_use];

#ifdef CONFIG_AL_ETH_ALLOC_PAGE
		if (unlikely(al_mod_eth_alloc_rx_page(adapter, rx_info,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 15, 0)
						  GFP_ATOMIC | __GFP_COMP) < 0)) {
#else
						  __GFP_COLD | GFP_ATOMIC | __GFP_COMP) < 0)) {
#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(4, 15, 0) */
#elif defined(CONFIG_AL_ETH_ALLOC_FRAG)
		if (unlikely(al_mod_eth_alloc_rx_frag(adapter, rx_ring, rx_info) < 0)) {
#elif defined(CONFIG_AL_ETH_ALLOC_SKB)
		if (unlikely(al_mod_eth_alloc_rx_skb(adapter, rx_ring, rx_info) < 0)) {
#endif
			netdev_warn(adapter->netdev, "failed to alloc buffer for rx queue %d\n",
				    qid);
			break;
		}
		rc = al_mod_eth_rx_buffer_add(rx_ring->dma_q,
					 &rx_info->al_mod_buf, AL_ETH_RX_FLAGS_INT,
					 NULL);
		if (unlikely(rc)) {
			netdev_warn(adapter->netdev, "failed to add buffer for rx queue %d\n",
				    qid);
			break;

		}
		next_to_use = AL_ETH_RX_RING_IDX_NEXT(rx_ring, next_to_use);
	}

	if (unlikely(i < num)) {
		u64_stats_update_begin(&rx_ring->syncp);
		rx_ring->rx_stats.partial_rx_ring_refill++;
		u64_stats_update_end(&rx_ring->syncp);
		netdev_warn(rx_ring->netdev,
			    "refilled rx qid %d with only %d buffers (out of %d required)\n",
			    qid, i, num);
	}

	if (likely(i))
		al_mod_eth_rx_buffer_action(rx_ring->dma_q, i);

	rx_ring->next_to_use = next_to_use;

	return i;
}

static void
al_mod_eth_free_rx_bufs(struct al_mod_eth_adapter *adapter, unsigned int qid)
{
	struct al_mod_eth_ring *rx_ring = &adapter->rx_ring[qid];
	unsigned int i;

	for (i = 0; i < rx_ring->sw_count; i++) {
		struct al_mod_eth_rx_buffer *rx_info = &rx_ring->rx_buffer_info[i];

#ifdef CONFIG_AL_ETH_ALLOC_PAGE
		if (rx_info->page)
			al_mod_eth_free_rx_page(adapter, rx_info);
#elif defined(CONFIG_AL_ETH_ALLOC_FRAG)
		if (rx_info->data)
			al_mod_eth_free_rx_frag(adapter, rx_info);
#elif defined(CONFIG_AL_ETH_ALLOC_SKB)
		if (rx_info->skb)
			al_mod_eth_free_rx_skb(adapter, rx_info);
#endif
	}
}

/**
 * al_mod_eth_refill_all_rx_bufs - allocate all queues Rx buffers
 * @adapter: board private structure
 *
 **/
static void
al_mod_eth_refill_all_rx_bufs(struct al_mod_eth_adapter *adapter)
{
	int i;
	unsigned int num = 0;

	for (i = 0; i < adapter->num_rx_queues; i++) {
		num = al_mod_udma_available_get(adapter->rx_ring[i].dma_q);
		al_mod_eth_refill_rx_bufs(adapter, i, num);
	}
}

static void
al_mod_eth_free_all_rx_bufs(struct al_mod_eth_adapter *adapter)
{
	int i;

	for (i = 0; i < adapter->num_rx_queues; i++)
		al_mod_eth_free_rx_bufs(adapter, i);
}

static void al_mod_eth_ints_and_errors_print(struct al_mod_eth_adapter *adapter)
{
	struct al_mod_unit_adapter unit_adapter;
	uint32_t cause;
	uint32_t mask;
	void __iomem *ec_iofic_base_regs;
	void __iomem *mac_iofic_base_regs;
	void __iomem *udma_iofic_base_regs;
	struct al_mod_udma *udma;
	int grp;

	udma = &adapter->hal_adapter.rx_udma;

	cause = al_mod_udma_iofic_read_cause_adv(udma,
					     AL_UDMA_IOFIC_LEVEL_PRIMARY,
					     AL_INT_GROUP_D);
	netdev_info(adapter->netdev, "UDMA%d Primary GROUP D cause reg: 0x%08x\n",
		    adapter->udma_num, cause);

	/* UDMA Primary IOFIC */
	udma_iofic_base_regs = al_mod_udma_iofic_reg_base_get_adv(udma,
							      AL_UDMA_IOFIC_LEVEL_PRIMARY);
	mask = al_mod_iofic_read_mask(udma_iofic_base_regs,
				  AL_INT_GROUP_D);
	netdev_info(adapter->netdev, "UDMA%d Primary GROUP D mask reg: 0x%08x. masked cause 0x%08x\n",
		    adapter->udma_num, mask, ~mask & cause);

	cause = al_mod_udma_iofic_read_cause_adv(udma,
					     AL_UDMA_IOFIC_LEVEL_SECONDARY,
					     AL_INT_GROUP_A);
	netdev_info(adapter->netdev, "UDMA%d Secondary Group A (M2S error/hints) cause reg: 0x%08x\n",
		    adapter->udma_num, cause);

	/* UDMA Secondary IOFIC */
	udma_iofic_base_regs = al_mod_udma_iofic_reg_base_get_adv(udma,
							      AL_UDMA_IOFIC_LEVEL_SECONDARY);

	mask = al_mod_iofic_read_mask(udma_iofic_base_regs,
				  AL_INT_GROUP_A);
	netdev_info(adapter->netdev, "UDMA%d Secondary Group A (M2S error/hints) mask reg: 0x%08x. masked cause 0x%08x\n",
		    adapter->udma_num, mask, ~mask & cause);

	cause = al_mod_udma_iofic_read_cause_adv(udma,
					     AL_UDMA_IOFIC_LEVEL_SECONDARY,
					     AL_INT_GROUP_B);
	netdev_info(adapter->netdev, "UDMA%d Secondary Group B (S2M error/hints) cause reg: 0x%08x\n",
		    adapter->udma_num, cause);

	mask = al_mod_iofic_read_mask(udma_iofic_base_regs,
				  AL_INT_GROUP_B);
	netdev_info(adapter->netdev, "UDMA%d Secondary Group B (S2M error/hints) mask reg: 0x%08x. masked cause 0x%08x\n",
		    adapter->udma_num, mask, ~mask & cause);

	if (al_mod_udma_rev_id_get(udma) >= AL_UDMA_REV_ID_4) {
		cause = al_mod_udma_iofic_read_cause_adv(udma,
						     AL_UDMA_IOFIC_LEVEL_SECONDARY,
						     AL_INT_GROUP_C);
		netdev_info(adapter->netdev, "UDMA%d Secondary Group C (Additional M2S & S2M error/hints) cause reg: 0x%08x\n",
			    adapter->udma_num, cause);

		mask = al_mod_iofic_read_mask(udma_iofic_base_regs,
					  AL_INT_GROUP_C);
		netdev_info(adapter->netdev, "UDMA%d Secondary Group C (Additional M2S & S2M error/hints) mask reg: 0x%08x. masked cause 0x%08x\n",
			    adapter->udma_num, mask, ~mask & cause);
	}

	ec_iofic_base_regs = adapter->ec_base + AL_ETH_EC_IOFIC_OFFSET;
	for (grp = AL_INT_GROUP_A; grp <= AL_INT_GROUP_D; grp++) {
		cause = al_mod_iofic_read_cause(ec_iofic_base_regs,
					    grp);
		netdev_info(adapter->netdev, "EC iofic Group %c cause reg: 0x%08x\n",
			    'A' + grp, cause);

		mask = al_mod_iofic_read_mask(ec_iofic_base_regs,
					   grp);
		netdev_info(adapter->netdev, "EC iofic Group %c mask reg: 0x%08x. masked cause 0x%08x\n",
			    'A' + grp, mask, ~mask & cause);
	}

	mac_iofic_base_regs = adapter->mac_base + AL_ETH_MAC_IOFIC_OFFSET;
	for (grp = AL_INT_GROUP_A; grp <= AL_INT_GROUP_D; grp++) {
		cause = al_mod_iofic_read_cause(mac_iofic_base_regs,
					    grp);
		netdev_info(adapter->netdev, "MAC iofic Group %c cause reg: 0x%08x\n",
			    'A' + grp, cause);

		mask = al_mod_iofic_read_mask(mac_iofic_base_regs,
					   grp);
		netdev_info(adapter->netdev, "MAC iofic Group %c mask reg: 0x%08x. masked cause 0x%08x\n",
			    'A' + grp, mask, ~mask & cause);
	}

	if (IS_NIC(adapter->board_type)) {
		al_mod_unit_adapter_handle_init(&unit_adapter,
					    AL_UNIT_ADAPTER_TYPE_ETH,
					    adapter->internal_pcie_base,
					    NULL,
					    NULL,
					    NULL,
					    NULL);
	} else {
		al_mod_unit_adapter_handle_init(&unit_adapter,
					    AL_UNIT_ADAPTER_TYPE_ETH,
					    NULL,
					    &al_mod_eth_read_pci_config,
					    &al_mod_eth_write_pci_config,
					    NULL,
					    adapter->pdev);
	}

	al_mod_unit_adapter_err_attr_print(&unit_adapter, AL_TRUE);
	al_mod_unit_adapter_err_attr_print(&unit_adapter, AL_FALSE);
}

#define EC_STAT_UDMA_PRINT(ec_stat_udma, field)					\
	netdev_info(adapter->netdev, #field": 0x%08x\n", (ec_stat_udma)->field);

#define EC_STATS_PRINT(ec_stats, field)					\
	netdev_info(adapter->netdev, #field": 0x%08x\n", (ec_stats)->field);

void al_mod_eth_ec_counters_print(struct al_mod_eth_adapter *adapter, uint8_t udma_idx)
{
	struct al_mod_eth_ec_stat_udma ec_stat_udma;
	struct al_mod_eth_ec_stats ec_stats;
	int i;

	al_mod_eth_ec_stat_udma_get(&adapter->hal_adapter, udma_idx, &ec_stat_udma);

	netdev_info(adapter->netdev, "EC_STAT_UDMA of UDMA%u:\n", udma_idx);

	EC_STAT_UDMA_PRINT(&ec_stat_udma, rfw_out_rx_pkt);
	EC_STAT_UDMA_PRINT(&ec_stat_udma, rfw_out_drop);
	EC_STAT_UDMA_PRINT(&ec_stat_udma, msw_in_rx_pkt);
	EC_STAT_UDMA_PRINT(&ec_stat_udma, msw_drop_q_full);
	EC_STAT_UDMA_PRINT(&ec_stat_udma, msw_drop_sop);
	EC_STAT_UDMA_PRINT(&ec_stat_udma, msw_drop_eop);
	EC_STAT_UDMA_PRINT(&ec_stat_udma, msw_wr_eop);
	EC_STAT_UDMA_PRINT(&ec_stat_udma, msw_out_rx_pkt);
	EC_STAT_UDMA_PRINT(&ec_stat_udma, tso_no_tso_pkt);
	EC_STAT_UDMA_PRINT(&ec_stat_udma, tso_tso_pkt);
	EC_STAT_UDMA_PRINT(&ec_stat_udma, tso_seg_pkt);
	EC_STAT_UDMA_PRINT(&ec_stat_udma, tso_pad_pkt);
	EC_STAT_UDMA_PRINT(&ec_stat_udma, tpm_tx_spoof);
	EC_STAT_UDMA_PRINT(&ec_stat_udma, tmi_in_tx_pkt);
	EC_STAT_UDMA_PRINT(&ec_stat_udma, tmi_out_to_mac);
	EC_STAT_UDMA_PRINT(&ec_stat_udma, tmi_out_to_rx);

	for (i = 0; i < adapter->num_tx_queues; i++)
		EC_STAT_UDMA_PRINT(&ec_stat_udma, tx_q_bytes[i]);

	for (i = 0; i < adapter->num_tx_queues; i++)
		EC_STAT_UDMA_PRINT(&ec_stat_udma, tx_q_pkts[i]);

	al_mod_eth_ec_stats_get(&adapter->hal_adapter, &ec_stats);

	netdev_info(adapter->netdev, "EC STATS:\n");

	EC_STATS_PRINT(&ec_stats, faf_in_rx_pkt);
	EC_STATS_PRINT(&ec_stats, faf_in_rx_short);
	EC_STATS_PRINT(&ec_stats, faf_in_rx_long);
	EC_STATS_PRINT(&ec_stats, faf_out_rx_pkt);
	EC_STATS_PRINT(&ec_stats, faf_out_rx_short);
	EC_STATS_PRINT(&ec_stats, faf_out_rx_long);
	EC_STATS_PRINT(&ec_stats, faf_out_drop);
	EC_STATS_PRINT(&ec_stats, rxf_in_rx_pkt);
	EC_STATS_PRINT(&ec_stats, rxf_in_fifo_err);
	EC_STATS_PRINT(&ec_stats, lbf_in_rx_pkt);
	EC_STATS_PRINT(&ec_stats, lbf_in_fifo_err);
	EC_STATS_PRINT(&ec_stats, rxf_out_rx_1_pkt);
	EC_STATS_PRINT(&ec_stats, rxf_out_rx_2_pkt);
	EC_STATS_PRINT(&ec_stats, rxf_out_drop_1_pkt);
	EC_STATS_PRINT(&ec_stats, rxf_out_drop_2_pkt);
	EC_STATS_PRINT(&ec_stats, rpe_1_in_rx_pkt);
	EC_STATS_PRINT(&ec_stats, rpe_1_out_rx_pkt);
	EC_STATS_PRINT(&ec_stats, rpe_2_in_rx_pkt);
	EC_STATS_PRINT(&ec_stats, rpe_2_out_rx_pkt);
	EC_STATS_PRINT(&ec_stats, rpe_3_in_rx_pkt);
	EC_STATS_PRINT(&ec_stats, rpe_3_out_rx_pkt);
	EC_STATS_PRINT(&ec_stats, tpe_in_tx_pkt);
	EC_STATS_PRINT(&ec_stats, tpe_out_tx_pkt);
	EC_STATS_PRINT(&ec_stats, tpm_tx_pkt);
	EC_STATS_PRINT(&ec_stats, tfw_in_tx_pkt);
	EC_STATS_PRINT(&ec_stats, tfw_out_tx_pkt);
	EC_STATS_PRINT(&ec_stats, rfw_in_rx_pkt);
	EC_STATS_PRINT(&ec_stats, rfw_in_vlan_drop);
	EC_STATS_PRINT(&ec_stats, rfw_in_parse_drop);
	EC_STATS_PRINT(&ec_stats, rfw_in_mc);
	EC_STATS_PRINT(&ec_stats, rfw_in_bc);
	EC_STATS_PRINT(&ec_stats, rfw_in_vlan_exist);
	EC_STATS_PRINT(&ec_stats, rfw_in_vlan_nexist);
	EC_STATS_PRINT(&ec_stats, rfw_in_mac_drop);
	EC_STATS_PRINT(&ec_stats, rfw_in_mac_ndet_drop);
	EC_STATS_PRINT(&ec_stats, rfw_in_ctrl_drop);
	EC_STATS_PRINT(&ec_stats, rfw_in_prot_i_drop);
	EC_STATS_PRINT(&ec_stats, eee_in);
}

/**
 * al_mod_eth_free_tx_bufs - Free Tx Buffers per Queue
 * @adapter: network interface device structure
 * @qid: queue index
 **/
static void
al_mod_eth_free_tx_bufs(struct al_mod_eth_adapter *adapter, unsigned int qid)
{
	struct al_mod_eth_ring *tx_ring = &adapter->tx_ring[qid];
	unsigned int i;
	bool udma_debug_printed = 0;

	for (i = 0; i < tx_ring->sw_count; i++) {
		struct al_mod_eth_tx_buffer *tx_info = &tx_ring->tx_buffer_info[i];
		struct al_mod_buf *al_mod_buf;
		struct sk_buff *skb;
		int nr_frags;
		int j;

		skb = tx_info->skb;
		if (skb == NULL)
			continue;

		if (!udma_debug_printed) {
			al_mod_udma_regs_print(tx_ring->dma_q->udma, AL_UDMA_DEBUG_QUEUE(qid));
			al_mod_udma_q_struct_print(tx_ring->dma_q->udma, qid);
			al_mod_udma_ring_descs_around_comp_print(tx_ring->dma_q->udma, qid,
							AL_RING_SUBMISSION,
							AL_ETH_NUM_OF_DESCS_PRE_COMP_HEAD_PRINT);
			al_mod_eth_ints_and_errors_print(adapter);

			al_mod_eth_ec_counters_print(adapter, adapter->udma_num);

			udma_debug_printed = 1;
		}
		netdev_warn(adapter->netdev, "free uncompleted tx skb qid %d idx 0x%x. skb->len:%d, skb->data_len:%d\n",
			    qid,
			    i,
			    skb->len,
			    skb->data_len);

		/* Some of the following fields are valid only if the packets were indeed TCP */
		if (skb_shinfo(skb)->gso_type & (SKB_GSO_TCPV4 | SKB_GSO_TCPV6)) {
			unsigned short mss = skb_shinfo(skb)->gso_size;
			unsigned int header_len = skb_transport_offset(skb) + tcp_hdrlen(skb);
			unsigned int total_payload_len = skb->len - header_len;

			netdev_warn(adapter->netdev, "\tRequested MSS size: %u, headers len (incl. TCP): %u, total payload len: %u\n",
				    mss,
				    header_len,
				    total_payload_len);
		}

		al_mod_buf = tx_info->hal_pkt.bufs;
		dma_unmap_single(&adapter->pdev->dev,
				 dma_unmap_addr(al_mod_buf, addr),
				 dma_unmap_len(al_mod_buf, len), DMA_TO_DEVICE);

		/* unmap remaining mapped pages */
		nr_frags = tx_info->hal_pkt.num_of_bufs - 1;
		for (j = 0; j < nr_frags; j++) {
			al_mod_buf++;
			dma_unmap_page(&adapter->pdev->dev,
				       dma_unmap_addr(al_mod_buf, addr),
				       dma_unmap_len(al_mod_buf, len),
				       DMA_TO_DEVICE);
		}

		dev_kfree_skb_any(skb);
	}
	netdev_tx_reset_queue(netdev_get_tx_queue(adapter->netdev, qid));
}

static void
al_mod_eth_free_all_tx_bufs(struct al_mod_eth_adapter *adapter)
{
	int i;

	for (i = 0; i < adapter->num_rx_queues; i++)
		al_mod_eth_free_tx_bufs(adapter, i);
}

static int al_mod_eth_request_irq(struct al_mod_eth_adapter *adapter)
{
	unsigned long flags;
	struct al_mod_eth_irq *irq;
	int rc = 0, i;

	if (adapter->flags & AL_ETH_FLAG_MSIX_ENABLED)
		flags = 0;
	else
		flags = IRQF_SHARED;

	for (i = 0; i < adapter->irq_vecs; i++) {
		irq = &adapter->irq_tbl[i];
		rc = request_irq(irq->vector, irq->handler, flags, irq->name, irq->data);
		if (rc) {
			netdev_err(adapter->netdev,
				"failed to request irq. index %d rc %d\n", i, rc);
			return rc;
		}
		irq->requested = 1;

		netdev_dbg(adapter->netdev,
			"set affinity hint of irq. index %d to 0x%lx (irq vector: %d)\n",
			i, irq->affinity_hint_mask.bits[0], irq->vector);

#ifdef HAVE_IRQ_AFFINITY_HINT
		irq_set_affinity_hint(irq->vector, &irq->affinity_hint_mask);
#endif
	}

	if (adapter->ua_irq.vector) {
		irq = &adapter->ua_irq;
		rc = request_irq(irq->vector, irq->handler, 0, irq->name, irq->data);
		if (rc) {
			netdev_err(adapter->netdev,
				"failed to request unit adapter irq. rc %d\n",
				rc);
			return rc;
		}
		irq->requested = 1;
#ifdef HAVE_IRQ_AFFINITY_HINT
		irq_set_affinity_hint(irq->vector, &irq->affinity_hint_mask);
#endif
	}

	return 0;
}

static void __al_eth_free_irq(struct al_mod_eth_adapter *adapter)
{
	struct al_mod_eth_irq *irq;
	int i;

	if (adapter->ua_irq.requested) {
#ifdef HAVE_IRQ_AFFINITY_HINT
		irq_set_affinity_hint(adapter->ua_irq.vector, NULL);
#endif
		free_irq(adapter->ua_irq.vector, adapter);
		adapter->ua_irq.requested = 0;
	}

	for (i = 0; i < adapter->irq_vecs; i++) {
		irq = &adapter->irq_tbl[i];
		if (irq->requested) {
#ifdef HAVE_IRQ_AFFINITY_HINT
			irq_set_affinity_hint(irq->vector, NULL);
#endif
			free_irq(irq->vector, irq->data);
		}
		irq->requested = 0;
	}
}

static void
al_mod_eth_free_irq(struct al_mod_eth_adapter *adapter)
{
#ifdef CONFIG_RFS_ACCEL
	if (adapter->msix_vecs >= 1) {
		free_irq_cpu_rmap(adapter->netdev->rx_cpu_rmap);
		adapter->netdev->rx_cpu_rmap = NULL;
	}
#endif

	__al_eth_free_irq(adapter);
	if (adapter->flags & AL_ETH_FLAG_MSIX_ENABLED)
		pci_disable_msix(adapter->pdev);

	adapter->flags &= ~AL_ETH_FLAG_MSIX_ENABLED;

	kfree(adapter->msix_entries);
	adapter->msix_entries = NULL;
}

/**
 * al_mod_eth_tx_poll - NAPI Tx polling callback
 * @napi: structure for representing this polling device
 * @budget: how many packets driver is allowed to clean
 *
 * This function is used for legacy and MSI, NAPI mode
 **/
static int
al_mod_eth_tx_poll(struct napi_struct *napi, int budget)
{
	struct al_mod_eth_napi *al_mod_napi = container_of(napi, struct al_mod_eth_napi, napi);
	struct al_mod_eth_adapter *adapter = al_mod_napi->adapter;
	unsigned int qid = al_mod_napi->qid;
	struct al_mod_eth_ring *tx_ring = &adapter->tx_ring[qid];
	struct netdev_queue *txq;
	unsigned int tx_bytes = 0;
	unsigned int total_done;
	u16 next_to_clean;
	int tx_pkt = 0;
#ifdef CONFIG_AL_ETH_ALLOC_SKB
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 19, 0)
	struct sk_buff_head *rx_rc = this_cpu_ptr(&rx_recycle_cache);
#else
	struct sk_buff_head *rx_rc = &__get_cpu_var(rx_recycle_cache);
#endif
#endif
	total_done = al_mod_eth_comp_tx_get(tx_ring->dma_q);
	dev_dbg(&adapter->pdev->dev, "tx_poll: q %d total completed descs %x\n",
		qid, total_done);
	next_to_clean = tx_ring->next_to_clean;
	txq = netdev_get_tx_queue(adapter->netdev, qid);

	while (total_done) {
		struct al_mod_eth_tx_buffer *tx_info;
		struct sk_buff *skb;
		struct al_mod_buf *al_mod_buf;
		int i, nr_frags;

		tx_info = &tx_ring->tx_buffer_info[next_to_clean];
		/* stop if not all descriptors of the packet are completed */
		if (tx_info->tx_descs > total_done)
			break;

		skb = tx_info->skb;

		/* prefetch skb_end_pointer() to speedup skb_shinfo(skb) */
		prefetch(&skb->end);

		tx_info->skb = NULL;
		al_mod_buf = tx_info->hal_pkt.bufs;
		dma_unmap_single(tx_ring->dev, dma_unmap_addr(al_mod_buf, addr),
				 dma_unmap_len(al_mod_buf, len), DMA_TO_DEVICE);

		/* unmap remaining mapped pages */
		nr_frags = tx_info->hal_pkt.num_of_bufs - 1;
		for (i = 0; i < nr_frags; i++) {
			al_mod_buf++;
			dma_unmap_page(tx_ring->dev, dma_unmap_addr(al_mod_buf, addr),
				       dma_unmap_len(al_mod_buf, len), DMA_TO_DEVICE);
		}

		tx_bytes += skb->len;
		dev_dbg(&adapter->pdev->dev, "tx_poll: q %d skb %p completed\n",
				qid, skb);
#ifdef CONFIG_AL_ETH_ALLOC_SKB
		if ((skb_queue_len(rx_rc) < adapter->rx_ring_count) &&
		    skb_recycle_check(skb,
				      tx_ring->netdev->mtu + ETH_HLEN + ETH_FCS_LEN + VLAN_HLEN))
			__skb_queue_head(rx_rc, skb);
               else
#endif
		       dev_kfree_skb(skb);
		tx_pkt++;

		/** Increase counters, relevent in adaptive mode only */
		tx_ring->packets += tx_pkt;
		tx_ring->bytes += tx_bytes;

		total_done -= tx_info->tx_descs;
		next_to_clean = AL_ETH_TX_RING_IDX_NEXT(tx_ring, next_to_clean);
	}

	netdev_tx_completed_queue(txq, tx_pkt, tx_bytes);

	tx_ring->next_to_clean = next_to_clean;

	dev_dbg(&adapter->pdev->dev, "tx_poll: q %d done next to clean %x\n",
		qid, next_to_clean);

	/* need to make the rings circular update visible to
	 * al_mod_eth_start_xmit() before checking for netif_queue_stopped().
	 */
	smp_mb();

	if (unlikely(netif_tx_queue_stopped(txq) &&
		     (al_mod_udma_available_get(tx_ring->dma_q) > AL_ETH_TX_WAKEUP_THRESH))) {
		__netif_tx_lock(txq, smp_processor_id());
		if (netif_tx_queue_stopped(txq) &&
		    (al_mod_udma_available_get(tx_ring->dma_q) > AL_ETH_TX_WAKEUP_THRESH))
			netif_tx_wake_queue(txq);
		__netif_tx_unlock(txq);
	}

	/* all work done, exit the polling mode */
	napi_complete(napi);
	al_mod_reg_write32_relaxed(tx_ring->unmask_reg_offset, tx_ring->unmask_val);
	return 0;
}

/**
 * al_mod_eth_rx_checksum - indicate in skb if hw indicated a good cksum
 * @adapter: structure containing adapter specific data
 * @hal_pkt: HAL structure for the packet
 * @skb: skb currently being received and modified
 **/
static inline void al_mod_eth_rx_checksum(struct al_mod_eth_adapter *adapter,
				      struct al_mod_eth_pkt *hal_pkt,
				      struct sk_buff *skb)
{
	skb_checksum_none_assert(skb);

	/* Rx csum disabled */
	if (unlikely(!(adapter->netdev->features & NETIF_F_RXCSUM))) {
		netdev_dbg(adapter->netdev, "hw checksum offloading disabled\n");
		return;
	}

	/* if IP and error */
	if (unlikely(
		((hal_pkt->l3_proto_idx == AL_ETH_PROTO_ID_IPv4) ||
		AL_ETH_HAL_PKT_PROTO_NUM_IPV4(hal_pkt))
		&&
		(hal_pkt->flags & AL_ETH_RX_FLAGS_L3_CSUM_ERR))) {
		/* ipv4 checksum error */
		netdev_dbg(adapter->netdev, "rx ipv4 header checksum error\n");
		return;
	}

	/* if TCP/UDP over IPv4 (not IPv4 frag in which this bit isn't valid) */
	if (likely((hal_pkt->l4_proto_idx == AL_ETH_PROTO_ID_TCP) ||
	   (hal_pkt->l4_proto_idx == AL_ETH_PROTO_ID_UDP) ||
	   (AL_ETH_HAL_PKT_PROTO_NUM_TCP_UDP(hal_pkt)))) {
		/* TODO: check if we need the test above for TCP/UDP */
		if (unlikely(hal_pkt->flags & AL_ETH_RX_FLAGS_L4_CSUM_ERR)) {
			/* TCP/UDP checksum error */
			netdev_dbg(adapter->netdev, "rx L4 checksum error\n");
			return;
		} else {
			netdev_dbg(adapter->netdev, "rx checksum correct\n");
			skb->ip_summed = CHECKSUM_UNNECESSARY;
		}
	}
}

/**
 * al_mod_eth_rx_poll - NAPI Rx polling callback
 * @napi: structure for representing this polling device
 * @budget: how many packets driver is allowed to clean
 *
 * This function is used for legacy and MSI, NAPI mode
 **/
static int
al_mod_eth_rx_poll(struct napi_struct *napi, int budget)
{
	struct al_mod_eth_napi *al_mod_napi = container_of(napi, struct al_mod_eth_napi, napi);
	struct al_mod_eth_adapter *adapter = al_mod_napi->adapter;
	unsigned int qid = al_mod_napi->qid;
	struct al_mod_eth_ring *rx_ring = &adapter->rx_ring[qid];
	struct al_mod_eth_pkt *hal_pkt = &rx_ring->hal_pkt;
	int work_done = 0;
	u16 next_to_clean = rx_ring->next_to_clean;
	int refill_required;
	int refill_actual;
	unsigned int total_len = 0;
	unsigned int small_copy_len_pkt = 0;
	int budget_init = budget;

	netdev_dbg(adapter->netdev, "%s qid %d\n", __func__, qid);

	do {
		struct sk_buff *skb;
		unsigned int descs;

		descs = al_mod_eth_pkt_rx(rx_ring->dma_q, hal_pkt);
		if (unlikely(descs == 0))
			break;

		netdev_dbg(adapter->netdev, "rx_poll: q %d got packet from hal. descs %d\n",
					   qid, descs);
		netdev_dbg(adapter->netdev, "rx_poll: q %d flags %x. l3 proto %d l4 proto %d\n",
					qid, hal_pkt->flags, hal_pkt->l3_proto_idx,
					hal_pkt->l4_proto_idx);
		/**
		 * Increase packets counter
		 * will be used to adjust threshold if using adaptive interrupt moderation
		 **/
		rx_ring->packets++;

		/* ignore if detected dma or eth controller errors */
		if (hal_pkt->flags & (AL_ETH_RX_ERROR | AL_UDMA_CDESC_ERROR)) {
			netdev_dbg(adapter->netdev, "receive packet with error. flags = 0x%x\n", hal_pkt->flags);
			next_to_clean = AL_ETH_RX_RING_IDX_ADD(rx_ring, next_to_clean, descs);
			goto next;
		}

		/* allocate skb and fill it */
		skb = al_mod_eth_rx_skb(adapter, rx_ring, hal_pkt, descs,
				&next_to_clean);

		/* exit if we failed to retrieve a buffer */
		if (unlikely(!skb)) {
			next_to_clean = AL_ETH_RX_RING_IDX_ADD(rx_ring,
					next_to_clean, descs);
			break;
		}

		al_mod_eth_rx_checksum(adapter, hal_pkt, skb);

#if defined(NETIF_F_RXHASH) || defined(CONFIG_ARCH_ALPINE)
		/** Always do in linux kernel driver, on host-driver it is platform related */
		if (likely(adapter->netdev->features & NETIF_F_RXHASH)) {
			uint32_t rxhash;
#if ((LINUX_VERSION_CODE >= KERNEL_VERSION(3, 14, 0) && !(RHEL_RELEASE_CODE)) || (RHEL_RELEASE_CODE && RHEL_RELEASE_CODE > RHEL_RELEASE_VERSION(7, 1)))
			enum pkt_hash_types type;
			/* This is necessary since the kernel expects a 32-bit hash, while our HW
			 * performs a 16-bit hash. This ensures a unique "final" hash for 16-bit
			 * and 32-bit hashes (if we add support in future HW).
			 * NOTICE: This hash isn't equal to the 32-bit hash which would
			 * be calculated by the kernel.
			 *
			 * ETH v4 the HW does pass a 32bit hash, but we keep this rxhash "fixup"
			 * for :
			 * (*) consistency between drivers of different generations
			 * (*) not an expensive operation, better than adding an "if" to check
			 *     adapter rev id
			 *
			 * (*) In the case of IPv4 frag packet, this value is the
			 * intermediate checksum for fragment IP.
			 */
			rxhash = (((uint32_t)hal_pkt->rxhash) << 16) ^ hal_pkt->rxhash;
			if (likely((hal_pkt->l4_proto_idx == AL_ETH_PROTO_ID_TCP) ||
				   (hal_pkt->l4_proto_idx == AL_ETH_PROTO_ID_UDP)) ||
				   (AL_ETH_HAL_PKT_PROTO_NUM_TCP_UDP(hal_pkt)))
				/** Hash on L4 + L3 params : src_IP, dst_IP, src_port, dst_port */
				type = PKT_HASH_TYPE_L4;

			/** set RX hash & L4/L3 hash */
			skb_set_hash(skb, rxhash, type);
#else
			rxhash = (((uint32_t)hal_pkt->rxhash) << 16) ^ hal_pkt->rxhash;
			skb->rxhash = rxhash;
			if (likely((hal_pkt->l4_proto_idx == AL_ETH_PROTO_ID_TCP) ||
				   (hal_pkt->l4_proto_idx == AL_ETH_PROTO_ID_UDP)) ||
				   (AL_ETH_HAL_PKT_PROTO_NUM_TCP_UDP(hal_pkt)))
				skb->l4_rxhash = 1;
#endif
		}
#endif /* defined(NETIF_F_RXHASH) || defined(CONFIG_ARCH_ALPINE) */

		skb_record_rx_queue(skb, qid);

		total_len += skb->len;
#ifdef CONFIG_AL_ETH_ALLOC_SKB
		netif_receive_skb(skb);
#else
		if (hal_pkt->bufs[0].len <= adapter->small_copy_len) {
			small_copy_len_pkt++;
			napi_gro_receive(napi, skb);
		} else
			napi_gro_frags(napi);
#endif

next:
		budget--;
		work_done++;
	} while (likely(budget));

	rx_ring->next_to_clean = next_to_clean;

	u64_stats_update_begin(&rx_ring->syncp);
	rx_ring->rx_stats.bytes += total_len;
	rx_ring->rx_stats.packets += work_done;
	rx_ring->rx_stats.small_copy_len_pkt += small_copy_len_pkt;
	u64_stats_update_end(&rx_ring->syncp);

	refill_required = al_mod_udma_available_get(rx_ring->dma_q);
	refill_actual = al_mod_eth_refill_rx_bufs(adapter, qid, refill_required);

	if (budget > 0) {
		netdev_dbg(adapter->netdev, "rx_poll: q %d done. next to clean %x\n",
			   qid, next_to_clean);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 19, 0)
		napi_complete_done(napi, work_done);
#else
		napi_complete(napi);
#endif

		if(adapter->adaptive_intr_rate)
			al_mod_eth_update_intr_moderation(adapter, qid, AL_ETH_RX);

		if (unlikely(refill_actual < refill_required)) {
			/* No need to reschedule when budget=0, since the napi system does it */
			netdev_warn(adapter->netdev, "%s: rescheduling rx queue %d\n",
				    __func__, qid);
			napi_reschedule(napi);
		}

		al_mod_reg_write32_relaxed(rx_ring->unmask_reg_offset, rx_ring->unmask_val);
	}

	netdev_dbg(adapter->netdev, "queue%d - budget_init=%d, budget=%d, work_done=%d, next_to_clean=%d\n",
		   qid, budget_init, budget, work_done, next_to_clean);

	return work_done;
}

static void
al_mod_eth_del_napi(struct al_mod_eth_adapter *adapter)
{
	int i;
	int napi_num = adapter->num_rx_queues + adapter->num_tx_queues;

	for (i = 0; i < napi_num; i++)
		netif_napi_del(&adapter->al_mod_napi[i].napi);
}

static void
al_mod_eth_init_napi(struct al_mod_eth_adapter *adapter)
{
	int i;
	int napi_num = adapter->num_rx_queues + adapter->num_tx_queues;

	for (i = 0; i < napi_num; i++) {
		struct al_mod_eth_napi *napi = &adapter->al_mod_napi[i];
		int (*poll)(struct napi_struct *, int);

		if (i < adapter->num_rx_queues) {
			poll = al_mod_eth_rx_poll;
			napi->qid = i;
		} else {
			poll = al_mod_eth_tx_poll;
			napi->qid = i - adapter->num_rx_queues;
		}
		netif_napi_add(adapter->netdev, &adapter->al_mod_napi[i].napi, poll, 64);
		napi->adapter = adapter;
	}
}


static void
al_mod_eth_napi_disable_all(struct al_mod_eth_adapter *adapter)
{
	int i;
	int napi_num = adapter->num_rx_queues + adapter->num_tx_queues;

	for (i = 0; i < napi_num; i++)
		napi_disable(&adapter->al_mod_napi[i].napi);
}

static void
al_mod_eth_napi_enable_all(struct al_mod_eth_adapter *adapter)

{
	int i;
	int napi_num = adapter->num_rx_queues + adapter->num_tx_queues;

	for (i = 0; i < napi_num; i++)
		napi_enable(&adapter->al_mod_napi[i].napi);
}

static void al_mod_eth_restore_ethtool_params(struct al_mod_eth_adapter *adapter)
{
	int i;
	unsigned int tx_usecs = adapter->tx_usecs;
	unsigned int rx_usecs = adapter->rx_usecs;

	adapter->tx_usecs = 0;
	adapter->rx_usecs = 0;

	al_mod_eth_set_coalesce(adapter, tx_usecs, rx_usecs);

	if (IS_ETH_V4_ADV(adapter->rev_id, adapter->dev_id))
		for (i = 0; i < adapter->rss_ind_tbl_size; i++)
			al_mod_eth_rfw_lrss_generic_entry_set(
				&adapter->rfw_handle, i, adapter->default_rx_udma_num,
				(uint8_t)adapter->rss_ind_tbl[i]);
	else
		for (i = 0; i < adapter->rss_ind_tbl_size; i++)
			al_mod_eth_thash_table_set(&adapter->hal_adapter,
					       i,
					       adapter->default_rx_udma_num,
					       adapter->rss_ind_tbl[i]);
}

static void __iomem *pbs_regs_base;

#ifdef CONFIG_ARCH_ALPINE
static int __init al_mod_eth_pbs_regs_base_map(void)
{
	struct device_node *node;
	struct of_device_id of_pbs_table[] = {
		{.compatible = "annapurna-labs,al-pbs"},
		{ /* end of list */ },
	};

	pr_info("Initializing Peripheral Bus System (PBS) resources\n");

	node = of_find_matching_node(NULL, of_pbs_table);
	if (!node) {
		pr_err("PBS entry was not found in device-tree\n");
		return -ENODEV;
	}

	pbs_regs_base = of_iomap(node, 0);
	if (!pbs_regs_base) {
		pr_err("Failed to map PBS memory\n");
		return -ENOMEM;
	}

	pr_debug("PBS addr [%p]\n", pbs_regs_base);

	return 0;
}

static void __exit al_mod_eth_pbs_regs_base_unmap(void)
{
	if (pbs_regs_base)
		iounmap(pbs_regs_base);
}
#endif

static void al_mod_eth_error_cb(struct al_mod_err_events_field *f)
{
	struct al_mod_eth_adapter *adapter = container_of(f->parent_module->handle,
						      struct al_mod_eth_adapter,
						      err_events_handle);
	u64 *counter;
	unsigned long flags;

	netdev_err(adapter->netdev, "%s %s: %d\n", f->parent_module->name,
		   f->name, f->counter);

	switch (f->parent_module->secondary_module) {
	case AL_ERR_EVENTS_SEC_MODULE_UNIT_ADAPTER:
		counter = &adapter->dev_stats.ua_errors;
		break;
	case AL_ERR_EVENTS_SEC_MODULE_ETH_EC:
		counter = &adapter->dev_stats.ec_errors;
		break;
	case AL_ERR_EVENTS_SEC_MODULE_ETH_MAC:
		counter = &adapter->dev_stats.mac_errors;
		break;
	case AL_ERR_EVENTS_SEC_MODULE_UDMA:
		counter = &adapter->dev_stats.udma_errors;
		break;
	default:
		netdev_err(adapter->netdev, "Unknown module [%d]\n",
			   f->parent_module->secondary_module);
		return;
	}

	/*
	 * The spinlock is used for two purposes:
	 * - protect the counter (may be written from sysfs as well)
	 * - protect the syncp (two writers using syncp at the same time may
	 *   cause readers to block forever)
	 * irqsave is used since this function can be called from both ISR and
	 * tasklet
	 * The syncp itself protects against reading in the middle of write
	 * operation (on 32bit archs only)
	 */
	spin_lock_irqsave(&adapter->stats_lock, flags);

	u64_stats_update_begin(&adapter->syncp);
	(*counter)++;
	u64_stats_update_end(&adapter->syncp);

	spin_unlock_irqrestore(&adapter->stats_lock, flags);
}

static int err_events_handle_init(struct al_mod_eth_adapter *adapter)
{
	struct al_mod_err_events_handle_init_params handle_init = {
		.pbs_regs_base = adapter->pbs_regfile_base,
		.print_cb = printk, /* Currently not used */
		.error_cb = al_mod_eth_error_cb,
		.error_threshold = 0
	};
	int rc;

	rc = al_mod_err_events_handle_init(&adapter->err_events_handle,
				       &handle_init);
	if (rc) {
		netdev_err(adapter->netdev,
			   "%s: Failed to init al_mod_err_events_handle_init\n",
			   __func__);
		return rc;
	}

	return 0;
}

static int err_events_eth_ua_init(struct al_mod_eth_adapter *adapter)
{
	struct al_mod_err_events_ua_init_params unit_adapter_init = {
		.type = AL_UNIT_ADAPTER_TYPE_ETH,
		.index = adapter->port_num,
		.ua_regs_base = adapter->unit_adapter_base,
		.collect_mode = IS_NIC(adapter->board_type) ?
			AL_ERR_EVENTS_COLLECT_POLLING :
			AL_ERR_EVENTS_COLLECT_DEFAULT,
	};
	int rc;

	/* init Unit-Adapter errors */
	rc = al_mod_err_events_unit_adapter_init(&adapter->err_events_handle,
					     &adapter->ua_data,
					     &unit_adapter_init);
	if (rc) {
		netdev_err(adapter->netdev,
			   "%s: Failed to init ua err events for ETH(%d)\n",
			   __func__, adapter->port_num);
		return rc;
	}

	netdev_dbg(adapter->netdev,
		    "%s: Registered ETH(%d)-Unit-Adapter errors\n", __func__,
		    adapter->port_num);
	return 0;
}

static int err_events_eth_ec_init(struct al_mod_eth_adapter *adapter)
{
	struct al_mod_err_events_eth_ec_init_params ec_init = {
		.eth_index = adapter->port_num,
		.adapter = &adapter->hal_adapter,
		.collect_mode = EC_ERRS_COLLECTED_BY_INTR(adapter) ?
			AL_ERR_EVENTS_COLLECT_DEFAULT :
			AL_ERR_EVENTS_COLLECT_POLLING,
	};
	int rc;

	/* init EC errors */
	rc = al_mod_err_events_eth_ec_init(&adapter->err_events_handle,
				       &adapter->ec_data,
				       &ec_init);
	if (rc) {
		netdev_err(adapter->netdev,
			   "%s: Failed to init ec err events for ETH(%d)\n",
			   __func__, adapter->port_num);
		return rc;
	}

	netdev_dbg(adapter->netdev, "%s: Registered ETH(%d) EC errors\n",
		    __func__, adapter->port_num);
	return 0;
}

static int err_events_eth_udma_init(struct al_mod_eth_adapter *adapter)
{
	int rc;
	struct al_mod_err_events_udma_init_params udma_init = {
		.regs_base = adapter->udma_base,
		.udma_index = adapter->udma_num,
		.module_index = adapter->port_num,
		.primary_module = AL_ERR_EVENTS_MODULE_ETH,
		.collect_mode = AL_ERR_EVENTS_COLLECT_DEFAULT,
	};

	/* init UDMA errors */
	rc = al_mod_err_events_udma_init(&adapter->err_events_handle,
				     &adapter->udma_data,
				     &udma_init);
	if (rc) {
		netdev_err(adapter->netdev,
			   "%s: Failed to init udma err events for ETH(%d),UDMA(%d)\n",
			   __func__, adapter->port_num, adapter->udma_num);
		return rc;
	}

	netdev_dbg(adapter->netdev, "%s: Registered ETH(%d)-UDMA(%d) errors\n",
		    __func__, adapter->port_num, adapter->udma_num);
	return 0;
}

static int al_mod_eth_err_events_init(struct al_mod_eth_adapter *adapter)
{
	int rc;

	err_events_handle_init(adapter);

	/* init Unit Adapter */
	if (adapter->unit_adapter_base) {
		rc = err_events_eth_ua_init(adapter);
		if (rc)
			goto exit;
	}

	/* init EC and its submodules */
	rc = err_events_eth_ec_init(adapter);
	if (rc)
		goto exit;

	/* init UDMA */
	rc = err_events_eth_udma_init(adapter);

exit:
	return rc;
}

#ifdef CONFIG_AL_ETH_ERR_EVENTS_TEST
static void al_mod_eth_test_error_cb(struct al_mod_err_events_field *f)
{
	struct al_mod_eth_adapter *adapter = container_of(f->parent_module->handle,
						      struct al_mod_eth_adapter,
						      err_events_handle);

	netdev_info(adapter->netdev, "TEST: %s %s: %d\n",
		    f->parent_module->name, f->name, f->counter);
}

void al_mod_eth_err_events_self_test(struct al_mod_eth_adapter *adapter)
{
	al_mod_bool ret;

	/* some minimal validation before running the test - if interface is
	 * down interrupts won't be triggered */
	if (!adapter->up) {
		netdev_info(adapter->netdev,
			    "%s: can't run test while interface is down\n",
			    __func__);
		return;
	}

	netdev_info(adapter->netdev, "%s: start\n", __func__);

	/* Since the self test is used for internal verification, we ignore the
	 * unlikely probability of an actual error occuring during the test */
	adapter->err_events_handle.error_cb = al_mod_eth_test_error_cb;
	ret = al_mod_err_events_handle_test(&adapter->err_events_handle);
	adapter->err_events_handle.error_cb = al_mod_eth_error_cb;

	netdev_info(adapter->netdev, "%s: %s\n", __func__,
		    ret ? "success" : "failed");
}
#endif

static int al_mod_eth_up(struct al_mod_eth_adapter *adapter)
{
	int rc;

	netdev_dbg(adapter->netdev, "%s\n", __func__);
	al_mod_eth_mdio_lock(adapter->mdio_bus);

	if (adapter->flags & AL_ETH_FLAG_RESET_REQUESTED) {
		if (IS_ETH_V3_ADV(adapter->rev_id, adapter->dev_id)) {
			rc = al_mod_eth_mac_tx_stop(adapter);
			if (rc)
				goto err_mac_tx_stop;
		}

		al_mod_eth_function_reset(adapter);
		adapter->flags &= ~AL_ETH_FLAG_RESET_REQUESTED;
		adapter->flags &= ~AL_ETH_FLAG_RESET_UDMA_EC;
	}

	rc = al_mod_eth_hw_init(adapter);
	if (rc)
		goto err_hw_init_open;

	if (adapter->err_events_enabled)
		schedule_delayed_work(&adapter->err_events_task,
				      msecs_to_jiffies(AL_ETH_ERR_EVENTS_POLL_IVAL));

	rc = al_mod_eth_setup_int_mode(adapter, disable_msi);
	if (rc) {
		dev_err(&adapter->pdev->dev, "%s failed at setup interrupt mode!\n", __func__);
		goto err_setup_int;
	}

	/* allocate transmit descriptors */
	rc = al_mod_eth_setup_all_tx_resources(adapter);
	if (rc)
		goto err_setup_tx;

	/* allocate receive descriptors */
	rc = al_mod_eth_setup_all_rx_resources(adapter);
	if (rc)
		goto err_setup_rx;

	/*
	 * napi poll functions should be intialized before running request_irq(), to handle
	 * the case where an interrupt is already pending in the GIC (shouldn't happen normally),
	 * causing the ISR to fire immediately while the poll function isn't set yet,causing a
	 * null dereference
	 */
	al_mod_eth_init_napi(adapter);

	al_mod_eth_configure_int_mode(adapter);

	/*config rx fwd*/
	if (adapter->rev_id <= AL_ETH_REV_ID_3)
		al_mod_eth_config_rx_fwd(adapter);
	else
		al_mod_eth_config_rx_fwd_adv(adapter);

	al_mod_eth_change_mtu(adapter->netdev, adapter->netdev->mtu);

	/* enable hw queues */
	al_mod_eth_udma_queues_enable_all(adapter);

	/* TODO: move this before queues_enable_all() */
	al_mod_eth_refill_all_rx_bufs(adapter);

	al_mod_eth_napi_enable_all(adapter);

	rc = al_mod_eth_request_irq(adapter);
	if (rc)
		goto err_req_irq;

	al_mod_eth_interrupts_unmask(adapter);

#ifndef CONFIG_ARCH_ALPINE
	{
		int err;

		/** Linux kernel / Alpine integrated driver */
		/* enable forwarding interrupts from eth through pci end point*/
		err = al_mod_eth_modify_pcie_ep_int(adapter, 1);
		if (err != 0)
			netdev_err(adapter->netdev, "%s : Failed at al_mod_eth_modify_pcie_ep_int\n",
				__func__);
	}
#endif
	/* enable transmits */
	netif_tx_start_all_queues(adapter->netdev);

	/* enable flow control */
	al_mod_eth_flow_ctrl_enable(adapter);

	al_mod_eth_restore_ethtool_params(adapter);

	/* If this is the first call to al_mod_eth_up after the probe (as part of
	 * live update), then we need to set it to false for subsequent
	 * al_mod_eth_up() invocations (so mac_config will be called).
	 */
	if (adapter->mac_started)
		adapter->mac_started = false;

	/* enable the mac tx and rx paths */
	al_mod_eth_mac_start(&adapter->hal_adapter);

	adapter->up = true;

	/* For RGMII with no external phy, this usually indicates that the peer is onboard (e.g. a
	 * switch device), so the physical link should always be up. This mainly handles the case
	 * that reset_task() was invoked, which indirectly invokes netif_carrier_off() without
	 * calling netif_carrier_on() later on.
	 * netif_carrier_on() is being called from :
	 *  (*) Interface initialization - one time via al_mod_eth_open().
	 *  (*) if there is LM the LM status task will invoke it once a link is achieved.
	 */
#ifdef CONFIG_ARCH_ALPINE
	if (!((adapter->board_type == ALPINE_INTEGRATED) && (adapter->use_lm)))
		netif_carrier_on(adapter->netdev);
#else
	if (adapter->board_type == ALPINE_NIC_V2_1G_TEST)
		netif_carrier_on(adapter->netdev);
#endif
	al_mod_eth_mdio_unlock(adapter->mdio_bus);
	return rc;

err_req_irq:
	al_mod_eth_free_irq(adapter);
	al_mod_eth_napi_disable_all(adapter);
	/*
	 * TODO: call the "inverse" of al_mod_eth_udma_queues_enable_all().
	 * Currently there is no HAL API for udma_q_disable(). Perhaps should
	 * be some mix of al_mod_eth_adapter_stop() and al_mod_eth_flr_udma_queues().
	 */
	al_mod_eth_free_all_rx_bufs(adapter);
	al_mod_eth_del_napi(adapter);
	al_mod_eth_free_all_rx_resources(adapter);
err_setup_rx:
	al_mod_eth_free_all_tx_resources(adapter);
err_setup_tx:
err_setup_int:
	al_mod_eth_hw_stop(adapter);
err_hw_init_open:
	al_mod_eth_function_reset(adapter);
err_mac_tx_stop:
	al_mod_eth_mdio_unlock(adapter->mdio_bus);
	return rc;
}

#ifdef CONFIG_RFS_ACCEL
static int
al_mod_eth_flow_steer(struct net_device *netdev, const struct sk_buff *skb,
	       u16 rxq_index, u32 flow_id)
{
	struct al_mod_eth_adapter *adapter = netdev_priv(netdev);
	uint32_t hash_table_size = adapter->rss_ind_tbl_size;
	unsigned int flow_hash_entry;

	if ((skb->protocol != htons(ETH_P_IP)) && (skb->protocol != htons(ETH_P_IPV6)))
		return -EPROTONOSUPPORT;

	if (skb->protocol == htons(ETH_P_IP)) {
		if (ip_is_fragment(ip_hdr(skb)))
			return -EPROTONOSUPPORT;
		if ((ip_hdr(skb)->protocol != IPPROTO_TCP) && (ip_hdr(skb)->protocol != IPPROTO_UDP))
			return -EPROTONOSUPPORT;
	}

	if (skb->protocol == htons(ETH_P_IPV6)) {
		/* ipv6 with extension not supported yet */
		if ((ipv6_hdr(skb)->nexthdr != IPPROTO_TCP) && (ipv6_hdr(skb)->nexthdr != IPPROTO_UDP))
			return -EPROTONOSUPPORT;
	}
	flow_hash_entry = flow_id & (hash_table_size - 1);
	adapter->rss_ind_tbl[flow_hash_entry] = rxq_index;

	if (IS_ETH_V4_ADV(adapter->rev_id, adapter->dev_id))
		al_mod_eth_rfw_lrss_generic_entry_set(&adapter->rfw_handle,
			flow_hash_entry, adapter->default_rx_udma_num, (uint8_t)rxq_index);
	else
		al_mod_eth_thash_table_set(&adapter->hal_adapter, flow_hash_entry,
			adapter->default_rx_udma_num, rxq_index);

	if (skb->protocol == htons(ETH_P_IP)) {
		int nhoff = skb_network_offset(skb);
		const struct iphdr *ip = (const struct iphdr *)(skb->data + nhoff);
		const __be16 *ports = (const __be16 *)(skb->data + nhoff + 4 * ip->ihl);

		netdev_info(adapter->netdev, "steering %s %pI4:%u:%pI4:%u to queue %u [flow %u filter %d]\n",
		   (ip->protocol == IPPROTO_TCP) ? "TCP" : "UDP",
		   &ip->saddr, ntohs(ports[0]), &ip->daddr, ntohs(ports[1]),
		   rxq_index, flow_id, flow_hash_entry);
	} else {
		struct ipv6hdr *ip6h = ipv6_hdr(skb);
		const __be16 *ports = (const __be16 *)skb_transport_header(skb);

		netdev_info(adapter->netdev, "steering %s %pI6c:%u:%pI6c:%u to queue %u [flow %u filter %d]\n",
		   (ipv6_hdr(skb)->nexthdr == IPPROTO_TCP) ? "TCP" : "UDP",
		   &ip6h->saddr, ntohs(ports[0]), &ip6h->daddr, ntohs(ports[1]),
		   rxq_index, flow_id, flow_hash_entry);

	}

	return flow_hash_entry;
}
#endif


#ifdef HAVE_NDO_SET_FEATURES
/** For kernels above KERNEL_VERSION(2,6,39) */
static int al_mod_set_features(struct net_device *dev,
		    netdev_features_t features) {
#if defined(NETIF_F_MQ_TX_LOCK_OPT)
	if (((features ^ dev->features) & NETIF_F_MQ_TX_LOCK_OPT) && netif_running(dev)) {
		netdev_warn(dev,
			    "Can't toggle NETIF_F_MQ_TX_LOCK_OPT : device is running!  \n");
		return -EINVAL;
	}
#endif
	return 0;
}
#endif

/************************ Link management ************************/

#if defined(CONFIG_ARCH_ALPINE)

static int al_mod_eth_i2c_data_read(void *context, uint8_t bus_id, uint8_t i2c_addr, uint8_t reg_addr,
				uint8_t *val, size_t len, al_mod_bool seq)
{
	struct i2c_adapter *i2c_adapter;
	struct al_mod_eth_adapter *adapter = context;
	struct i2c_msg msgs[2] = { 0 };
	size_t i2c_ops_cnt;
	int rc = 0;

	msgs[0].addr = i2c_addr;
	msgs[0].flags = 0;
	msgs[0].len = 1;
	msgs[0].buf = &reg_addr;
	msgs[1].addr = i2c_addr;
	msgs[1].flags = I2C_M_RD;
	msgs[1].buf = val;

	if (likely(seq)) {
		msgs[1].len = len;
		i2c_ops_cnt = 1;
	} else {
		msgs[1].len = 1;
		i2c_ops_cnt = len;
	}

	i2c_adapter = i2c_get_adapter(bus_id);

	if (i2c_adapter == NULL) {
		netdev_err(
			adapter->netdev,
			"Failed to get i2c adapter. "
			"probably caused by wrong i2c bus id in the device tree, "
			"wrong i2c mux implementation, or the port is configured wrongly as SFP+\n");
		return -EINVAL;
	}

	for (; i2c_ops_cnt--; msgs[1].buf++, reg_addr++) {
		if (i2c_transfer(i2c_adapter, msgs, ARRAY_SIZE(msgs)) != ARRAY_SIZE(msgs)) {
			netdev_dbg(adapter->netdev, "Failed to write sfp+ parameters\n");
			rc = -ETIMEDOUT;
			break;
		}
	}

	i2c_put_adapter(i2c_adapter);
	return rc;
}

static int al_mod_eth_i2c_data_write(void *context, uint8_t bus_id, uint8_t i2c_addr, uint8_t reg_addr, uint8_t *val, size_t len, al_mod_bool seq)
{
	struct i2c_adapter *i2c_adapter;
	struct al_mod_eth_adapter *adapter = context;
	struct i2c_msg msgs[1] = { 0 };
	uint8_t data_s[2] = { 0 }, *data_d = NULL;
	size_t i2c_ops_cnt;
	int rc = 0;

	msgs[0].addr = i2c_addr;
	msgs[0].flags = 0;
	if (likely(seq)) {
		data_d = kmalloc(1 + len, GFP_KERNEL);
		if (NULL == data_d) {
			netdev_err(adapter->netdev, "Unable to allocate i2c msg.\n");
			return -ENOMEM;
		}
		msgs[0].len = (1 + len);
		msgs[0].buf = data_d;
		i2c_ops_cnt = 1;
	} else {
		msgs[0].len = (1 + 1);
		msgs[0].buf = &data_s[0];
		i2c_ops_cnt = len;
	}

	i2c_adapter = i2c_get_adapter(bus_id);

	if (i2c_adapter == NULL) {
		kfree(data_d);
		netdev_err(
			adapter->netdev,
			"Failed to get i2c adapter. "
			"probably caused by wrong i2c bus id in the device tree, "
			"wrong i2c mux implementation, or the port is configured wrongly as SFP+\n");
		return -EINVAL;
	}

	while (i2c_ops_cnt--) {
		if (likely(seq)) {
			/* sequential access*/
			msgs[0].buf[0] = reg_addr;
			memcpy(&msgs[0].buf[1], val, len);
		} else {
			/* byte-per-byte access */
			msgs[0].buf[0] = reg_addr++;
			msgs[0].buf[1] = *val++;
		}

		if (i2c_transfer(i2c_adapter, msgs, ARRAY_SIZE(msgs)) != ARRAY_SIZE(msgs)) {
			netdev_dbg(adapter->netdev, "Failed to write sfp+ parameters\n");
			rc = -ETIMEDOUT;
			break;
		}
	}

	i2c_put_adapter(i2c_adapter);
	kfree(data_d);

	return rc;
}

static int al_mod_eth_i2c_byte_read(void *context, uint8_t bus_id, uint8_t i2c_addr, uint8_t reg_addr,
				uint8_t *val)
{

	return al_mod_eth_i2c_data_read(context, bus_id, i2c_addr, reg_addr, val, 1, AL_TRUE);
}

static int al_mod_eth_i2c_byte_write(void *context, uint8_t bus_id, uint8_t i2c_addr, uint8_t reg_addr, uint8_t val)
{
	return al_mod_eth_i2c_data_write(context, bus_id, i2c_addr, reg_addr, &val, 1, AL_TRUE);
}

static uint8_t al_mod_eth_get_rand_byte(void)
{
	uint8_t byte;
	get_random_bytes(&byte, 1);
	return byte;
}

static void al_mod_eth_lm_led_config_init_single(unsigned int gpio, const char *name)
{
	int err;

	if (gpio == GPIO_SPD_NOT_AVAILABLE)
		return;

	err = gpio_request_one(gpio, GPIOF_OUT_INIT_LOW, name);
	if (err)
		pr_err("[%s] LED %s (%u): unable to request GPIO\n", __func__, name, gpio);
}

static void al_mod_eth_lm_mode_apply(struct al_mod_eth_adapter		*adapter,
				 enum al_mod_eth_lm_link_mode	new_mode)
{

	if (new_mode == AL_ETH_LM_MODE_DISCONNECTED)
		return;

	if (new_mode == AL_ETH_LM_MODE_1G || new_mode == AL_ETH_LM_MODE_1G_DA) {
		adapter->mac_mode = AL_ETH_MAC_MODE_SGMII;
		adapter->link_config.active_speed = SPEED_1000;
	} else if (new_mode == AL_ETH_LM_MODE_25G) {
		adapter->mac_mode = AL_ETH_MAC_MODE_KR_LL_25G;
		adapter->link_config.active_speed = SPEED_25000;
	} else {
		/* force 25G MAC mode when using 25G SerDes */
		if (adapter->serdes_obj->type_get() == AL_SRDS_TYPE_25G)
			adapter->mac_mode = AL_ETH_MAC_MODE_KR_LL_25G;
		else
			adapter->mac_mode = AL_ETH_MAC_MODE_10GbE_Serial;

		adapter->link_config.active_speed = SPEED_10000;
	}

	adapter->link_config.active_duplex = DUPLEX_FULL;
}

static void al_mod_eth_lm_led_config_init(struct al_mod_eth_adapter *adapter)
{
	al_mod_eth_lm_led_config_init_single(
		adapter->gpio_spd_1g, "1g");
	al_mod_eth_lm_led_config_init_single(
		adapter->gpio_spd_10g, "10g");
	al_mod_eth_lm_led_config_init_single(
		adapter->gpio_spd_25g, "25g");
}

static void al_mod_eth_lm_led_config_terminate_single(unsigned int gpio)
{
	if (gpio == GPIO_SPD_NOT_AVAILABLE)
		return;
	else
		gpio_free(gpio);
}

static void al_mod_eth_lm_led_config_terminate(struct al_mod_eth_adapter *adapter)
{
	al_mod_eth_lm_led_config_terminate_single(adapter->gpio_spd_1g);
	al_mod_eth_lm_led_config_terminate_single(adapter->gpio_spd_10g);
	al_mod_eth_lm_led_config_terminate_single(adapter->gpio_spd_25g);
}

static void al_mod_eth_lm_led_config_single(unsigned int gpio, unsigned int val, const char *name)
{
	if (gpio == GPIO_SPD_NOT_AVAILABLE) {
		return;
	} else {
		if (gpio_cansleep(gpio))
			gpio_set_value_cansleep(gpio, val);
		else
			gpio_set_value(gpio, val);
		pr_debug("[%s] LED %s (%u) set to %u\n", __func__, name, gpio, val);
	}
}

static void al_mod_eth_lm_led_config(void *context, struct al_mod_eth_lm_led_config_data *data)
{
	struct al_mod_eth_adapter *adapter = context;

	al_mod_eth_lm_led_config_single(
		adapter->gpio_spd_1g, (data->speed == AL_ETH_LM_LED_CONFIG_1G), "1g");
	al_mod_eth_lm_led_config_single(
		adapter->gpio_spd_10g, (data->speed == AL_ETH_LM_LED_CONFIG_10G), "10g");
	al_mod_eth_lm_led_config_single(
		adapter->gpio_spd_25g, (data->speed == AL_ETH_LM_LED_CONFIG_25G), "25g");
}

static void al_mod_eth_serdes_mode_set(struct al_mod_eth_adapter *adapter)
{
	enum alpine_serdes_eth_mode mode =
		(adapter->mac_mode == AL_ETH_MAC_MODE_SGMII) ?
		ALPINE_SERDES_ETH_MODE_SGMII :
		ALPINE_SERDES_ETH_MODE_KR;

	if (alpine_serdes_eth_mode_set(adapter->serdes_grp, mode))
		netdev_err(
			adapter->netdev,
			"%s: alpine_serdes_eth_mode_set(%d, %d) failed!\n",
			__func__,
			adapter->serdes_grp,
			mode);

	al_mod_udelay(1000);
}

static int al_mod_eth_serdes_init(struct al_mod_eth_adapter *adapter)
{
	void __iomem			*serdes_base;

	adapter->serdes_init = false;

	if (adapter->orig_rev_id <= AL_ETH_REV_ID_3) {
		/*
		 * always call with group A to get the base address of
		 * all groups.
		 */
		serdes_base = alpine_serdes_resource_get(adapter->serdes_grp);

		if (!serdes_base) {
			netdev_err(adapter->netdev, "serdes_base get failed!\n");
			return -EIO;
		}

		adapter->serdes_obj = alpine_serdes_grp_obj_get(adapter->serdes_grp);

		if (!adapter->serdes_obj) {
			netdev_err(adapter->netdev, "serdes_obj get failed!\n");
			return -EIO;
		}

		if (adapter->live_update_restore_state &&
		    adapter->group_lm_context->serdes_mode ==
		    AL_SERDES_MODE_UNKNOWN) {
			al_mod_eth_serdes_mode_restore(adapter);
		}
	} else if (adapter->orig_rev_id == AL_ETH_REV_ID_4) {
#ifdef CONFIG_ALPINE_SERDES_AVG
		enum alpine_avg_serdes_cmplx_id type;

		if (adapter->dev_id == AL_ETH_DEV_ID_ADVANCED) {
			type = ALPINE_AVG_SERDES_CMPLX_ID_HS;
		} else if (adapter->mac_mode == AL_ETH_MAC_MODE_SGMII &&
			   adapter->dev_id == AL_ETH_DEV_ID_STANDARD) {
			type = ALPINE_AVG_SERDES_CMPLX_ID_SEC;
		} else {
			netdev_err(adapter->netdev,
				   "%s: unsupported eth configuration\n",
				   __func__);
			return -EIO;
		}

		serdes_base = alpine_avg_serdes_resource_get(type);
		if (!serdes_base) {
			netdev_err(adapter->netdev,
				"%s: Serdes resource get failed! "
				"Serdes & lanes have not been initialized\n", __func__);
			return -EIO;
		}
		/** In eth v4 - each group is a lane */
		adapter->serdes_obj =
			alpine_avg_serdes_obj_get(
				type,
				adapter->serdes_lane);
		if (!adapter->serdes_obj) {
			netdev_err(adapter->netdev,
				"%s: Failed to get serdes group obj (lane = %d)!\n",
				__func__, adapter->serdes_lane);
			return -EIO;
		}
#else
		netdev_err(adapter->netdev,
			"%s: No serdes resource support\n",
			__func__);
		return -EIO;
#endif
	}
	adapter->serdes_init = true;

	return 0;
}
#endif /** CONFIG_ARCH_ALPINE */

static void al_mod_eth_down(struct al_mod_eth_adapter *adapter)
{
	netdev_dbg(adapter->netdev, "%s\n", __func__);

	adapter->up = false;
	adapter->last_link = false;
	adapter->link_config.old_link = 0;
	adapter->link_config.active_duplex = DUPLEX_UNKNOWN;
	adapter->link_config.active_speed = SPEED_UNKNOWN;

	netif_carrier_off(adapter->netdev);
	netif_tx_disable(adapter->netdev);

	al_mod_eth_disable_int_sync(adapter);
	al_mod_eth_napi_disable_all(adapter);

	al_mod_eth_free_irq(adapter);
	if (adapter->err_events_enabled) {
		cancel_delayed_work_sync(&adapter->err_events_task);
		adapter->eth_error_ready = false;
	}

	al_mod_eth_hw_stop(adapter);
	al_mod_eth_del_napi(adapter);

	adapter->last_link = false;

	al_mod_eth_free_all_tx_bufs(adapter);
	al_mod_eth_free_all_rx_bufs(adapter);
	al_mod_eth_free_all_tx_resources(adapter);
	al_mod_eth_free_all_rx_resources(adapter);
}

#if defined(CONFIG_ARCH_ALPINE)
static void al_mod_eth_link_status_task(struct work_struct *work)
{
	struct al_mod_eth_adapter *adapter = container_of(to_delayed_work(work),
				       struct al_mod_eth_adapter, link_status_task);
	int				rc;

	do {
		rc = al_mod_eth_group_lm_link_manage(adapter->group_lm_context,
						 adapter->serdes_lane, (void *)adapter);
		/* sleep so another port can run link_status, to avoid deadlocks */
		usleep_range(1, 10);
	} while (rc == -EINPROGRESS);

	/* setting link status delay to 0 (through sysfs) will stop the task */
	if (adapter->link_poll_interval != 0) {
		unsigned long delay;

		delay = msecs_to_jiffies(adapter->link_poll_interval);

		schedule_delayed_work(&adapter->link_status_task, delay);
	}
}
#elif !defined(AL_ETH_PLAT_EMU) /* defined(CONFIG_ARCH_ALPINE) */
static void al_mod_eth_link_status_task_nic(struct work_struct *work)
{
	struct al_mod_eth_adapter *adapter = container_of(to_delayed_work(work),
			       struct al_mod_eth_adapter, link_status_task);
	struct al_mod_eth_link_status status;

	if (((adapter->board_type == ALPINE_NIC_V2_1G_TEST) ||
	    (adapter->board_type == ALPINE_NIC_V3_1G_TEST)) &&
	    (adapter->last_link == false)) {
		/* In 1g NIC case, there is no external phy and the eth port is connected
		 * directly to an on-board switch. The link between them should always be up,
		 * so no need to periodically check the link status.
		 */
		netdev_info(adapter->netdev, "%s link up\n", __func__);

		netif_carrier_on(adapter->netdev);
		adapter->last_link = true;

		return;
	}

	al_mod_eth_link_status_get(&adapter->hal_adapter, &status);

	if ((adapter->last_link == true) && (status.link_up == false)) {
		netdev_info(adapter->netdev, "%s link down\n", __func__);

		netif_carrier_off(adapter->netdev);
		adapter->last_link = false;
	} else if ((adapter->last_link == false) && (status.link_up == true)) {
		netdev_info(adapter->netdev, "%s link up\n", __func__);

		netif_carrier_on(adapter->netdev);
		adapter->last_link = true;
	}


	if (adapter->link_poll_interval != 0) {
		unsigned long delay;

		delay = msecs_to_jiffies(adapter->link_poll_interval);

		schedule_delayed_work(&adapter->link_status_task, delay);
	}
}
#endif /* defined(CONFIG_ARCH_ALPINE) */

#ifdef CONFIG_ARCH_ALPINE
static unsigned int al_mod_eth_systime_msec_get(void)
{
	struct timespec ts;
	getnstimeofday(&ts);
	return (unsigned int)((ts.tv_sec * 1000) + (ts.tv_nsec / 1000000));
}

#ifdef AL_ETH_V4_LM
static int al_mod_eth_v4_lm_gpio_get(void *gpio_context, unsigned int gpio, al_mod_bool *val)
{
	*val = gpio_get_value(gpio);
	return 0;
}
#endif

static void al_mod_eth_lm_config(struct al_mod_eth_adapter *adapter)
{
	struct al_mod_eth_lm_init_params	params = {0};
	int err;

	params.adapter = &adapter->hal_adapter;
	params.serdes_obj = adapter->serdes_obj;
	params.lane = adapter->serdes_lane;
	params.sfp_detection = adapter->sfp_detection_needed;
	if (adapter->sfp_detection_needed) {
		params.sfp_bus_id = adapter->i2c_adapter_id;
		params.sfp_i2c_addr = SFP_I2C_ADDR;
	}

	params.rx_equal = true;
	params.max_speed = adapter->max_speed;

	switch (adapter->max_speed) {
	case AL_ETH_LM_MAX_SPEED_MAX:
	case AL_ETH_LM_MAX_SPEED_25G:
		params.default_mode = AL_ETH_LM_MODE_25G;
		params.rx_equal = false;
		params.sfp_detect_force_mode = true;
		break;
	case AL_ETH_LM_MAX_SPEED_10G:
		if (adapter->lt_en && adapter->an_en)
			params.default_mode = AL_ETH_LM_MODE_10G_DA;
		else
			params.default_mode = AL_ETH_LM_MODE_10G_OPTIC;
		break;
	case AL_ETH_LM_MAX_SPEED_1G:
		params.default_mode = AL_ETH_LM_MODE_1G;
		break;
	default:
		netdev_warn(adapter->netdev, "Unknown max speed, using default\n");
		params.default_mode = AL_ETH_LM_MODE_10G_DA;
	}

	params.sfp_probe_1g = adapter->sfp_probe_1g;
	params.sfp_probe_10g = adapter->sfp_probe_10g;
	params.link_training = adapter->lt_en;
	params.static_values = !adapter->dont_override_serdes;
	params.i2c_read = &al_mod_eth_i2c_byte_read;
	params.i2c_write = &al_mod_eth_i2c_byte_write;
	params.i2c_read_data = &al_mod_eth_i2c_data_read;
	params.i2c_write_data = &al_mod_eth_i2c_data_write;
	params.i2c_context = adapter;
	params.get_random_byte = &al_mod_eth_get_rand_byte;
	params.kr_fec_enable = adapter->kr_fec_enable;

	params.retimer_exist = adapter->retimer.exist;
	params.retimer_type = adapter->retimer.type;
	params.retimer_bus_id = adapter->retimer.bus_id;
	params.retimer_i2c_addr = adapter->retimer.i2c_addr;
	params.retimer_channel = adapter->retimer.channel;
	params.retimer_tx_channel = adapter->retimer.tx_channel;
	params.speed_detection = adapter->speed_detection;

	params.auto_fec_enable = adapter->auto_fec_enable;
	params.auto_fec_initial_timeout = AUTO_FEC_INITIAL_TIMEOUT;
	params.auto_fec_toggle_timeout = AUTO_FEC_TOGGLE_TIMEOUT;
	params.get_msec = al_mod_eth_systime_msec_get;
	params.led_config = &al_mod_eth_lm_led_config;

	if (adapter->gpio_sfp_present) {
		err = gpio_request_one(adapter->gpio_sfp_present, GPIOF_IN, "sfp_present");
		if (err) {
			netdev_err(adapter->netdev,
				"Unable to request SFP present gpio %d, falling back to i2c polling\n",
				adapter->gpio_sfp_present);
		} else {
			params.gpio_present = adapter->gpio_sfp_present;
			params.gpio_get = gpio_get_value;
		}
	}

	al_mod_eth_lm_led_config_init(adapter);

	al_mod_eth_lm_init(&adapter->lm_context, &params);

}

#ifdef AL_ETH_V4_LM
#define ETH_V4_LM_STEP_MAX_TIME 5

static uint64_t al_mod_eth_lm_v4_systime_msec_get(void *p)
{
	struct timespec ts;
	getnstimeofday(&ts);
	return (uint64_t)((ts.tv_sec * 1000) + (ts.tv_nsec / 1000000));
}
#endif
#endif

static int al_mod_eth_v4_lm_init(struct al_mod_eth_adapter *adapter, bool first_time)
{
#ifdef AL_ETH_V4_LM
	struct al_mod_eth_v4_lm_params eth_v4_lm_params = {0};
	int ret;
	int i;

	/**
	 * ETH v4 in ALPINE_INTEGRATED mode, for EVP there is NO retimer
	 * once we will want to run this al_mod_eth on K2 that runs linux - we will need
	 * to add retimer support
	 */
	al_mod_assert(adapter->board_type == ALPINE_INTEGRATED); /** Support only this for now */

	eth_v4_lm_params.retimers_num = 0;
	eth_v4_lm_params.dev_id = adapter->dev_id;
	eth_v4_lm_params.rev_id = adapter->rev_id;
	eth_v4_lm_params.port_id = adapter->port_num;
	if (adapter->port_num == 0) {
	/** assuming DT holds 6 eth ports - 1 & 3 are STD - should take care of other modes later */
	/** Port 0 shoulh hold handles to all the lanes & mac handles */

		int adv_port_id;
		eth_v4_lm_params.lanes_num = AL_ETH_V4_LM_LANE_MAX;
		for (i = 0, adv_port_id = 0; i < AL_ETH_V4_PORTS_NUM; i++) {
			if (!(IS_ETH_V4_PORT_NUM_IS_ADV(i)))
				continue;
			al_mod_assert(adv_port_id < AL_ETH_V4_LM_PORT_MAX);
			eth_v4_lm_params.mac_objs[adv_port_id] =  &(adapter_handles[i]->mac_obj);
			eth_v4_lm_params.lanes[adv_port_id].serdes_obj =
				alpine_avg_serdes_obj_get(
						ALPINE_AVG_SERDES_CMPLX_ID_HS,
						adv_port_id);
			adv_port_id++;
		}
	} else {
		eth_v4_lm_params.mac_obj = &adapter->hal_adapter.mac_obj;
		eth_v4_lm_params.lanes_num = 1;
		eth_v4_lm_params.lanes[0].serdes_obj = adapter->serdes_obj;
	}

	eth_v4_lm_params.time_get = al_mod_eth_lm_v4_systime_msec_get;
	eth_v4_lm_params.step_max_time = ETH_V4_LM_STEP_MAX_TIME;

	if (adapter->gpio_sfp_present) {
		/** Bringup DT for EVP doesnt  have SFP PRESENCE GPIO - It always assumes SFP
		 * is connected
		 */
		ret = gpio_request_one(adapter->gpio_sfp_present, GPIOF_IN, "sfp_present");
		if (ret) {
			netdev_err(adapter->netdev,
				"Unable to request SFP present gpio %d, falling back to i2c polling\n",
				adapter->gpio_sfp_present);
		} else {
			eth_v4_lm_params.lanes[adapter->serdes_lane].sfp_presence_gpio = adapter->gpio_sfp_present;
			eth_v4_lm_params.lanes[adapter->serdes_lane].sfp_presence_exists = true;
			eth_v4_lm_params.gpio_get = al_mod_eth_v4_lm_gpio_get;
		}
	} else {
		eth_v4_lm_params.lanes[adapter->serdes_lane].sfp_presence_exists = false;
		eth_v4_lm_params.gpio_get = NULL;
		eth_v4_lm_params.lanes[adapter->serdes_lane].sfp_presence_gpio = adapter->gpio_sfp_present;
	}

	ret = al_mod_eth_v4_lm_handle_init(&adapter->eth_v4_lm_handle, &eth_v4_lm_params);
	if (ret) {
		netdev_err(adapter->netdev,
			"%s : Error in al_mod_eth_v4_lm_handle_init (%d)\n", __func__, ret);
		return ret;
	}

	if (first_time) {
		struct al_mod_eth_v4_lm_detected_mode detected_mode;

		detected_mode.detected = (adapter->auto_speed ? AL_FALSE : AL_TRUE);
		detected_mode.mac_mode = adapter->mac_mode;

		ret = al_mod_eth_v4_lm_detected_mode_set(&adapter->eth_v4_lm_handle, &detected_mode);
		if (ret) {
			netdev_err(adapter->netdev,"%s: Failed to set LM detected mode\n", __func__);
			return ret;
		}

		al_mod_eth_v4_lm_status_get(&adapter->eth_v4_lm_handle, &eth_v4_lm_port_0_status);
	} else {
		al_mod_eth_v4_lm_detected_mode_set(&adapter->eth_v4_lm_handle, &eth_v4_lm_port_0_status.detected_mode);
	}

	al_mod_eth_lm_led_config_init(adapter);
#endif
	return 0;
}

#ifdef AL_ETH_V4_LM
static int al_mod_eth_v4_lm_process(struct al_mod_eth_adapter *adapter)
{
	struct al_mod_eth_v4_lm 	*eth_v4_lm_handle = &adapter->eth_v4_lm_handle;
	int ret;
	bool link_up;
	struct al_mod_eth_v4_lm_status prev_status;

	al_mod_eth_v4_lm_status_get(eth_v4_lm_handle, &prev_status);

	/** Trigger LM state machine, step time = ETH_V4_LM_STEP_MAX_TIME */
	ret = al_mod_eth_v4_lm_step(eth_v4_lm_handle);
	if (ret) {
		netdev_err(adapter->netdev, "%s: LM step error\n", __func__);
		return ret;
	}

	link_up = eth_v4_lm_handle->status.link_up;

	if (link_up && !prev_status.link_up) {
		/** First establishment of link - Notify link is UP */
		netdev_info(adapter->netdev, "link UP\n");
		netif_carrier_on(adapter->netdev);
		ret = 0;
	} else if (!link_up && prev_status.link_up) {
		/** Link changed to DOWN  */
		netdev_info(adapter->netdev, "link DOWN\n");
		netif_carrier_off(adapter->netdev);
		ret = -ENETDOWN;
	}

	return ret;
}

static void al_mod_eth_lm_v4_link_status_task_port0(struct work_struct *work)
{
	struct al_mod_eth_adapter *adapter = container_of(to_delayed_work(work),
				       struct al_mod_eth_adapter, link_status_task);
	unsigned long delay;
	struct al_mod_eth_v4_lm 	*eth_v4_lm_handle = &adapter->eth_v4_lm_handle;
	bool prev_mode_detection = eth_v4_lm_handle->status.mode_detection_ongoing;

	al_mod_eth_v4_lm_process(adapter);

	/** End of mode detection mode will be identified on the next iteration */
	if (unlikely(!prev_mode_detection && eth_v4_lm_port_0_status.mode_detection_ongoing)) {
		/** Update the global status of port 0 */
		al_mod_eth_v4_lm_status_get(eth_v4_lm_handle, &eth_v4_lm_port_0_status);
	}

	/* setting link status delay to 0 (through sysfs) will stop the task */
	if (adapter->link_poll_interval != 0) {
		delay = msecs_to_jiffies(adapter->link_poll_interval);
		schedule_delayed_work(&adapter->link_status_task, delay);
	}
}

static void al_mod_eth_lm_v4_link_status_task_slave_port(struct work_struct *work)
{
	struct al_mod_eth_adapter *adapter = container_of(to_delayed_work(work),
				       struct al_mod_eth_adapter, link_status_task);
	unsigned long delay;

	/* sleep untill port 0 publish mode detection (happens when LINK changes UP or DOWN) */
	if (eth_v4_lm_port_0_status.mode_detection_ongoing) {
		delay = msecs_to_jiffies(adapter->link_poll_interval);
		schedule_delayed_work(&adapter->link_status_task, delay);
		return;
	}

	al_mod_eth_v4_lm_process(adapter);

	/* setting link status delay to 0 (through sysfs) will stop the task */
	if (adapter->link_poll_interval != 0) {
		delay = msecs_to_jiffies(adapter->link_poll_interval);
		schedule_delayed_work(&adapter->link_status_task, delay);
	}
}
#endif /* CONFIG_ARCH_ALPINE */

#define AQUANTIA_AQR105_ID			0x3a1b4a2

static int al_mod_eth_aq_phy_fixup(struct phy_device *phydev)
{
	int temp = 0;

	temp = phy_read(phydev, (MII_ADDR_C45 | (7 * 0x10000) | 0x20));
	temp &= ~(1 << 12);

	phy_write(phydev, (MII_ADDR_C45 | (7 * 0x10000) | 0x20), temp);

	temp = phy_read(phydev, (MII_ADDR_C45 | (7 * 0x10000) | 0xc400));
	temp |= ((1 << 15) | (1 << 11) | (1 << 10));
	phy_write(phydev, (MII_ADDR_C45 | (7 * 0x10000) | 0xc400), temp);

	temp = phy_read(phydev, (MII_ADDR_C45 | (7 * 0x10000) | 0));
	temp |= (1 << 9);
	temp &= ~(1 << 15);

	phy_write(phydev, (MII_ADDR_C45 | (7 * 0x10000) | 0), temp);

	return 0;
}

#if defined(CONFIG_ARCH_ALPINE)
/* This function is invoked in group_lm before changing serdes speed
 * (even if there is no need to change speed)
 */
static int al_mod_eth_lm_mode_change(void *handle, enum al_mod_eth_lm_link_mode old_mode,
				 enum al_mod_eth_lm_link_mode new_mode)
{
	struct al_mod_eth_adapter		*adapter = (struct al_mod_eth_adapter *)handle;
	int rc;

	if (old_mode != AL_ETH_LM_MODE_DISCONNECTED) {
		rc = al_mod_eth_rtnl_lock_w_flags_check(adapter);
		if (rc < 0)
			return rc;
		al_mod_eth_down(adapter);
		rtnl_unlock();
		netdev_info(adapter->netdev, "%s link down\n", __func__);
	}

	al_mod_eth_lm_mode_apply(adapter, new_mode);

	return 0;
}

/* This function is invoked in group_lm after serdes speed change (even if speed wasn't changed) */
static int al_mod_eth_group_lm_pre_establish(void *handle, enum al_mod_eth_lm_link_mode old_mode,
					 enum al_mod_eth_lm_link_mode new_mode)
{
	struct al_mod_eth_adapter		*adapter = (struct al_mod_eth_adapter *)handle;
	int rc = 0;

	if (new_mode != AL_ETH_LM_MODE_DISCONNECTED) {
		if (!adapter->up) {
			/* pre_establish can be called several times until link is established
			 * but we must call al_mod_eth_up only once
			 */
			rc = al_mod_eth_rtnl_lock_w_flags_check(adapter);
			if (rc < 0)
				return rc;
			if (!adapter->up)
				rc = al_mod_eth_up(adapter);
			rtnl_unlock();
			if (rc)
				return rc;
		}
	} else {
		return -ENETDOWN;
	}

	return 0;
}

static void al_mod_eth_group_lm_update_port_status(void *handle, int link_up)
{
	struct al_mod_eth_adapter		*adapter = (struct al_mod_eth_adapter *)handle;

	netdev_dbg(adapter->netdev, "%s: adapter->last_link:%d, link_up:%d",
		   __func__, adapter->last_link, link_up);

	if (link_up) {
		if (IS_ETH_V3_ADV(adapter->rev_id, adapter->dev_id))
			al_mod_eth_mac_flush_state_config(adapter, false);

		netif_carrier_on(adapter->netdev);
		adapter->last_link = true;
	} else {
		if (IS_ETH_V3_ADV(adapter->rev_id, adapter->dev_id))
			al_mod_eth_mac_flush_state_config(adapter, true);

		netif_carrier_off(adapter->netdev);
		adapter->last_link = false;
	}
}

static int al_mod_eth_rsrvd_mem_hdr_store(void)
{
	struct al_mod_eth_rsrvd_mem_hdr hdr = {0};

	pr_debug("%s: g_rsrvd_mem: %px\n", __func__, g_rsrvd_mem);

	hdr.hdr.magic = AL_ETH_RSRVD_MEM_HDR_MAGIC;
	hdr.hdr.type = AL_ETH_RSRVD_MEM_TYPE_GEN;
	hdr.hdr.ver = AL_ETH_RSRVD_MEM_VER_CURR;
	hdr.hdr.len = sizeof(hdr) +
		AL_ETH_MAX_PORTS_NUM *
		(sizeof(struct al_mod_eth_rsrvd_mem_section_hdr) +
		AL_ETH_RSRVD_MEM_SECTION_TYPE_PORT_DATA_SIZE);

	*((struct al_mod_eth_rsrvd_mem_hdr *)g_rsrvd_mem) = hdr;

	hdr.hdr.crc = al_mod_eth_calculate_crc(g_rsrvd_mem, hdr.hdr.len);
	((struct al_mod_eth_rsrvd_mem_hdr *)g_rsrvd_mem)->hdr.crc = hdr.hdr.crc;

	return 0;
}

static inline void
al_mod_eth_lm_v3_save_persistent(struct al_mod_eth_adapter *adapter,
			     struct al_mod_hal_eth_lm_v3_packed_container *packed)
{
	struct al_mod_eth_rsrvd_mem_section_hdr hdr = {0};
	void *section_port_data;

	section_port_data = adapter->rsrvd_mem +
		AL_ETH_RSRVD_MEM_SECTION_HDR_SIZE;
	*((struct al_mod_hal_eth_lm_v3_packed_container *)section_port_data) =
		*packed;

	hdr.hdr.magic = AL_ETH_RSRVD_MEM_SECTION_HDR_MAGIC;
	hdr.hdr.type = AL_ETH_RSRVD_MEM_SECTION_TYPE_PORT;
	hdr.hdr.ver = AL_ETH_RSRVD_MEM_SECTION_VER_CURR;

	*((struct al_mod_eth_rsrvd_mem_section_hdr *)adapter->rsrvd_mem) = hdr;

	/*
	 * Technically this func should only be called in al_mod_eth_clenaup(),
	 * instead of being invoked here for each port, but module_exit() funcs
	 * aren't invoked as part of the kexec flow.
	 */
	al_mod_eth_rsrvd_mem_hdr_store();
}

static void al_mod_eth_lm_v3_save(struct al_mod_eth_adapter *adapter)
{
	int rc;
	struct al_mod_hal_eth_lm_v3_packed_container packed_v3;

	netdev_dbg(adapter->netdev, "%s\n", __func__);

	rc = al_mod_eth_lm_v3_handle_pack(&packed_v3,
				      &adapter->lm_context,
				      adapter->port_num,
				      AL_LU_HAL_CURR_VER);
	if (rc) {
		netdev_err(adapter->netdev, "%s: failed to save LM state (%d)\n",
			   __func__, rc);
		return;
	}

	al_mod_eth_lm_v3_save_persistent(adapter, &packed_v3);

	netdev_dbg(adapter->netdev, "%s: done\n", __func__);
}

static inline void
al_mod_eth_lm_v3_load_persistent(struct al_mod_eth_adapter *adapter,
			     struct al_mod_hal_eth_lm_v3_packed_container *packed)
{
	void *section_port_data;

	section_port_data = adapter->rsrvd_mem +
		AL_ETH_RSRVD_MEM_SECTION_HDR_SIZE;

	*packed = *((struct al_mod_hal_eth_lm_v3_packed_container *)
		    section_port_data);
}

static int al_mod_eth_lm_v3_restore(struct al_mod_eth_adapter *adapter)
{
	struct al_mod_hal_eth_lm_v3_packed_container packed_v3;
	struct al_mod_eth_group_lm_link *link =
		&adapter->group_lm_context->link[adapter->serdes_lane];

	netdev_dbg(adapter->netdev, "%s\n", __func__);

	switch (live_update_restore_internal) {
	case AL_ETH_LIVE_UPDATE_RESTORE_ENABLE:
		al_mod_eth_lm_v3_load_persistent(adapter, &packed_v3);

		/* Restore saved lm_context state */
		al_mod_eth_lm_v3_handle_unpack(&adapter->lm_context, &packed_v3);
		break;
	case AL_ETH_LIVE_UPDATE_RESTORE_USE_PRE_DEFINED:
		adapter->lm_context.link_state = AL_ETH_LM_LINK_UP;
		adapter->lm_context.auto_fec_state = adapter->auto_fec_enable ?
			AL_ETH_LM_AUTO_FEC_ENABLED : AL_ETH_LM_AUTO_FEC_INIT;
		adapter->lm_context.last_detected_mode =
			AL_ETH_LM_MODE_DISCONNECTED;
		adapter->lm_context.mode = AL_ETH_LM_MODE_25G;
		break;
	case AL_ETH_LIVE_UPDATE_RESTORE_DISABLE:
		/* fall through */
	default:
		netdev_err(adapter->netdev, "Invalid live_update_restore_internal value (%d)\n",
			   live_update_restore_internal);
		al_mod_assert(0);
	}

	/* Restore group lm's port state */
	link->old_mode = adapter->lm_context.mode;
	link->new_mode = adapter->lm_context.mode;
	link->last_link_state = al_mod_eth_lm_link_state_get(&adapter->lm_context);

	netdev_dbg(adapter->netdev, "%s: lm old_mode [%s] -> lm new_mode [%s]\n",
		   __func__,
		   al_mod_eth_lm_mode_convert_to_str(link->old_mode),
		   al_mod_eth_lm_mode_convert_to_str(link->new_mode));

	if (link->last_link_state == AL_ETH_LM_LINK_UP) {
		int rc;

		/* In case we were already up, in order to avoid a link flap
		 * during the first al_mod_eth_up(), we signal via this flag that
		 * mac_config should be skipped.
		 */
		adapter->mac_started = true;

		rc = al_mod_eth_up(adapter);
		if (rc) {
			netdev_err(adapter->netdev, "%s: al_mod_eth_up failed %d\n",
				   __func__, rc);
			return rc;
		}

		al_mod_eth_lm_mode_apply(adapter, link->new_mode);
		al_mod_eth_group_lm_update_port_status(adapter, 1);
	}

	netdev_dbg(adapter->netdev, "%s: done\n", __func__);

	return 0;
}

static void al_mod_eth_serdes_mode_restore(struct al_mod_eth_adapter *adapter)
{
	enum al_mod_serdes_group_mode serdes_grp_mode;

	adapter->serdes_obj->serdes_mode_get(adapter->serdes_obj,
					     &serdes_grp_mode);

	switch (serdes_grp_mode) {
	case AL_SRDS_CFG_ETH_25G:
		adapter->group_lm_context->serdes_mode = AL_SERDES_MODE_25G;
		break;
	case AL_SRDS_CFG_KR:
		adapter->group_lm_context->serdes_mode = AL_SERDES_MODE_KR;
		break;
	case AL_SRDS_CFG_SGMII:
		adapter->group_lm_context->serdes_mode = AL_SERDES_MODE_SGMII;
		break;
	default:
		adapter->group_lm_context->serdes_mode = AL_SERDES_MODE_UNKNOWN;
		netdev_warn(adapter->netdev, "The serdes is configured to an unknown serdes_group_mode (%d)\n",
			    serdes_grp_mode);
	}

	netdev_dbg(adapter->netdev, "serdes_group_mode = %d\n",
		   serdes_grp_mode);
}
#endif /* defined(CONFIG_ARCH_ALPINE) */

static int al_mod_eth_v3_lm_init(struct al_mod_eth_adapter *adapter)
{
#if defined(CONFIG_ARCH_ALPINE)
	struct al_mod_eth_group_lm_context *group_lm_context = adapter->group_lm_context;
	struct al_mod_eth_group_lm_link_params group_lm_link_params;

	/** Driver is running LM */
	al_mod_eth_lm_config(adapter);

	group_lm_link_params.lm_context = &adapter->lm_context;
	group_lm_link_params.eth_port_num = adapter->port_num;
	group_lm_link_params.auto_speed = adapter->auto_speed;
	/* Since each eth port exists independently in Linux,
	 * we want each to run the group_lm_flow
	 */
	group_lm_link_params.skip_group_flow = AL_FALSE;
	group_lm_link_params.init_cb = NULL;
	group_lm_link_params.lm_mode_change_cb = &al_mod_eth_lm_mode_change;
	group_lm_link_params.update_link_status_cb =
		&al_mod_eth_group_lm_update_port_status;
	group_lm_link_params.pre_establish_cb = &al_mod_eth_group_lm_pre_establish;

	while (!group_lm_context->common_params.try_lock_cb(
				group_lm_context->common_params.serdes_grp))
		usleep_range(1, 10);

	al_mod_eth_group_lm_port_register(group_lm_context, adapter->serdes_lane,
				      &group_lm_link_params);

	group_lm_context->common_params.unlock_cb(
		group_lm_context->common_params.serdes_grp);

#endif
	return 0;
}

static void al_mod_eth_err_events_task(struct work_struct *work)
{
	struct al_mod_eth_adapter *adapter = container_of(to_delayed_work(work),
						      struct al_mod_eth_adapter,
						      err_events_task);
	struct al_mod_err_events_module *m;
	int rc;
	al_mod_bool ret;

	/* Check if we can proceed to polling */
	if (!adapter->eth_error_ready) {
		if (EC_ERRS_COLLECTED_BY_INTR(adapter))
			ret = (al_mod_eth_error_ints_unmask(&adapter->hal_adapter) == 0);
		else
			ret = al_mod_eth_err_polling_is_avail(&adapter->hal_adapter);
		if (!ret) {
			netdev_dbg(adapter->netdev,
				   "%s: FIFOs are not fully initialized\n",
				   __func__);
			goto resched;
		}

		netdev_dbg(adapter->netdev,
			    "%s: FIFOs are fully initialized\n", __func__);

		adapter->eth_error_ready = true;
	}

	if (!IS_V2_NIC(adapter->board_type))
		return;

	/* Collect all polling mode modules */
	for (m = adapter->err_events_handle.first_comp; m; m = m->next_comp) {
		/* We get UDMA errors via interrupts */
		if (m->secondary_module == AL_ERR_EVENTS_SEC_MODULE_UDMA)
			continue;

		/* We get EC/MAC errors via interrupts for STD (UDMA0) */
		if ((m->secondary_module == AL_ERR_EVENTS_SEC_MODULE_ETH_EC) &&
		    EC_ERRS_COLLECTED_BY_INTR(adapter))
			continue;

		rc = al_mod_err_events_module_collect(m, AL_ERR_EVENTS_COLLECT_POLLING);
		if (rc) {
			netdev_err(adapter->netdev,
				   "%s: Failed to collect [%s]\n", __func__,
				   m->name);
		}
	}

resched:
	schedule_delayed_work(&adapter->err_events_task,
			      msecs_to_jiffies(AL_ETH_ERR_EVENTS_POLL_IVAL));
}

#ifdef CONFIG_ARCH_ALPINE
#define AL_ETH_DT_ENTRY_FMT "/soc/eth%d"
static inline int al_mod_eth_parse_dev_node(struct al_mod_eth_adapter *adapter)
{
	char match_str[64];
	struct device_node *node;
	int rc;

	snprintf(match_str, sizeof(match_str), AL_ETH_DT_ENTRY_FMT,
		 adapter->port_num);

	node = of_find_node_by_path(match_str);
	if (!node) {
		netdev_err(adapter->netdev, "can't find node [%s]\n",
			   match_str);
		return -EINVAL;
	}

	adapter->unit_adapter_base = of_iomap(node, 0);
	if (!adapter->unit_adapter_base) {
		netdev_err(adapter->netdev,
			   "failed to map unit adapter base address, node [%s]\n",
			   match_str);
		return -ENOMEM;
	}

	adapter->ua_irq.vector = irq_of_parse_and_map(node, 0);
	if (!adapter->ua_irq.vector) {
		netdev_err(adapter->netdev, "failed to map unit adapter irq, node [%s]\n",
			   match_str);
		rc = -EINVAL;
		goto err_unit_adapter;
	}

	return 0;

err_unit_adapter:
	iounmap(adapter->unit_adapter_base);
	adapter->unit_adapter_base = NULL;
	return rc;
}
#endif

/**
 * al_mod_eth_open - Called when a network interface is made active
 * @netdev: network interface device structure
 *
 * Returns 0 on success, negative value on failure
 *
 * The open entry point is called when a network interface is made
 * active by the system (IFF_UP).  At this point all resources needed
 * for transmit and receive operations are allocated, the interrupt
 * handler is registered with the OS, the watchdog timer is started,
 * and the stack is notified that the interface is ready.
 **/
static int al_mod_eth_open(struct net_device *netdev)
{
	struct al_mod_eth_adapter		*adapter = netdev_priv(netdev);
	int				rc;
	unsigned long delay;
	netdev_dbg(adapter->netdev, "%s\n", __func__);
	netif_carrier_off(netdev);

	/* Notify the stack of the actual queue counts. */
	rc = netif_set_real_num_tx_queues(netdev, adapter->num_tx_queues);
	if (rc)
		return rc;

	rc = netif_set_real_num_rx_queues(netdev, adapter->num_rx_queues);
	if (rc)
		return rc;

#ifdef CONFIG_ARCH_ALPINE
		al_mod_eth_serdes_init(adapter);
#endif

	adapter->last_establish_failed = false;

	if (adapter->phy_exist == false) {
		if (adapter->use_lm) {
			if (adapter->rev_id <= AL_ETH_REV_ID_3) {
				al_mod_eth_v3_lm_init(adapter);
#ifdef CONFIG_ARCH_ALPINE
				if (adapter->live_update_restore_state) {
					al_mod_eth_lm_v3_restore(adapter);
					adapter->live_update_restore_state = false;
				}
#endif
			}
			else {
				al_mod_eth_v4_lm_init(adapter, (adapter->port_num == 0) ? true : false);
				rc = al_mod_eth_up(adapter);
				if (rc)
					return rc;
			}
		} else {
			/** Someone else is running LM */
			rc = al_mod_eth_up(adapter);
			if (rc)
				return rc;
		}
	} else {
		rc = al_mod_eth_up(adapter);
		if (rc)
			return rc;

		if (adapter->phy_fixup_needed) {
			rc = phy_register_fixup_for_uid(AQUANTIA_AQR105_ID, 0xffffffff,
							al_mod_eth_aq_phy_fixup);
			if (rc)
				netdev_warn(adapter->netdev, "failed to register PHY fixup\n");
		}
	}

#ifdef CONFIG_PHYLIB
	if (adapter->mdio_bus) {
		/* Bring the PHY up */
		phy_start(adapter->phydev);
		return 0;
	}
#endif

	u64_stats_update_begin(&adapter->syncp);
	adapter->dev_stats.interface_up++;
	u64_stats_update_end(&adapter->syncp);

	delay = msecs_to_jiffies(AL_ETH_FIRST_LINK_POLL_INTERVAL);
#ifdef CONFIG_ARCH_ALPINE
	if ((adapter->board_type == ALPINE_INTEGRATED) && (adapter->use_lm)) {
		if (adapter->rev_id <= AL_ETH_REV_ID_3)
			INIT_DELAYED_WORK(&adapter->link_status_task, al_mod_eth_link_status_task);
		else {
#ifdef AL_ETH_V4_LM
			if (adapter->port_num == 0)
				INIT_DELAYED_WORK(&adapter->link_status_task, al_mod_eth_lm_v4_link_status_task_port0);
			else
				INIT_DELAYED_WORK(&adapter->link_status_task, al_mod_eth_lm_v4_link_status_task_slave_port);
#else
			netdev_err(adapter->netdev, "%s: No LMv4 support\n", __func__);
			return -EINVAL;
#endif
		}
		schedule_delayed_work(&adapter->link_status_task, delay);
	} else
		netif_carrier_on(adapter->netdev);
#else
#ifdef AL_ETH_PLAT_EMU
	netif_carrier_on(adapter->netdev);
#else
	if (IS_NIC(adapter->board_type)) {
		INIT_DELAYED_WORK(&adapter->link_status_task,
				al_mod_eth_link_status_task_nic);
		schedule_delayed_work(&adapter->link_status_task, delay);
	}
#endif /* AL_ETH_PLAT_EMU */
#endif

	return rc;
}

static int al_mod_eth_close_lm(struct al_mod_eth_adapter *adapter)
{
#if defined(CONFIG_ARCH_ALPINE)
	if (adapter->use_lm && adapter->rev_id == AL_ETH_REV_ID_3 &&
	    live_update_save == AL_ETH_LIVE_UPDATE_SAVE_ENABLE &&
	    adapter->rsrvd_mem) {
		al_mod_eth_lm_v3_save(adapter);
	}

	if (!IS_ETH_V4_ADV(adapter->rev_id, adapter->dev_id)) {
		struct al_mod_eth_group_lm_context *group_lm_context = adapter->group_lm_context;
		while (!group_lm_context->common_params.try_lock_cb(
				group_lm_context->common_params.serdes_grp))
			usleep_range(1, 10);

		al_mod_eth_group_lm_port_unregister(group_lm_context,
						adapter->serdes_lane);

		group_lm_context->common_params.unlock_cb(
			group_lm_context->common_params.serdes_grp);
	}
#endif
	return 0;
}

/**
 * al_mod_eth_close - Disables a network interface
 * @netdev: network interface device structure
 *
 * Returns 0, this is not allowed to fail
 *
 * The close entry point is called when an interface is de-activated
 * by the OS.  The hardware is still under the drivers control, but
 * needs to be disabled.  A global MAC reset is issued to stop the
 * hardware, and all transmit and receive resources are freed.
 */
static int al_mod_eth_close(struct net_device *netdev)
{
	struct al_mod_eth_adapter *adapter = netdev_priv(netdev);

	netdev_dbg(adapter->netdev, "%s\n", __func__);

	adapter->flags |= AL_ETH_FLAG_CLOSE_ONGOING;

	if (adapter->use_lm)
		cancel_delayed_work_sync(&adapter->link_status_task);

	cancel_work_sync(&adapter->reset_task);

	if (adapter->use_lm) {
		al_mod_eth_close_lm(adapter);
	}

#ifdef CONFIG_PHYLIB
	/** Stop PHY & MDIO BUS */
	if (adapter->phydev) {
		/* Bring the PHY down */
		phy_stop(adapter->phydev);
	}
#endif

	if (adapter->up)
		al_mod_eth_down(adapter);

	u64_stats_update_begin(&adapter->syncp);
	adapter->dev_stats.interface_down++;
	u64_stats_update_end(&adapter->syncp);

#if defined(CONFIG_ARCH_ALPINE)
	if (adapter->use_lm) {
		al_mod_eth_lm_led_config_terminate(adapter);
		if (adapter->gpio_sfp_present)
			gpio_free(adapter->gpio_sfp_present);
	}
#endif

	adapter->flags &= ~AL_ETH_FLAG_CLOSE_ONGOING;

	return 0;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 6, 0)
static int
al_mod_eth_get_link_ksettings(struct net_device *netdev,
			  struct ethtool_link_ksettings *ecmd)
{
	struct al_mod_eth_adapter *adapter = netdev_priv(netdev);
	struct al_mod_eth_board_params params;
	int rc;

#ifdef CONFIG_PHYLIB
	if (adapter->phydev) {
		phy_ethtool_ksettings_get(adapter->phydev, ecmd);
		return 0;
	}
#endif

	rc = al_mod_eth_board_params_get_ex(&adapter->hal_adapter, &params);
	if (rc) {
		netdev_err(adapter->netdev, "Board info not available\n");
		return rc;
	}

	/* If we are in 25g_10g_autodetect, need to check the speed on the fly */
	if (params.media_type == AL_ETH_BOARD_MEDIA_TYPE_25G_10G_AUTO) {
		rc = al_mod_eth_get_serdes_25g_speed(adapter, &adapter->link_config.active_speed);
		if (rc) {
			netdev_err(adapter->netdev, "Invalid speed read from 25G serdes\n");
			return rc;
		}
	}

	ecmd->base.speed = adapter->link_config.active_speed;
	ecmd->base.duplex = adapter->link_config.active_duplex;
	ecmd->base.autoneg = adapter->link_config.autoneg;

	return 0;
}

static int
al_mod_eth_set_link_ksettings(struct net_device *netdev,
			  const struct ethtool_link_ksettings *ecmd)
{
	struct al_mod_eth_adapter *adapter = netdev_priv(netdev);
	struct al_mod_eth_group_lm_context *group_lm_context = adapter->group_lm_context;
	struct al_mod_eth_lm_link_config lm_link_config = {0};

#if defined(CONFIG_PHYLIB)
	if (adapter->phydev)
		return phy_ethtool_ksettings_set(adapter->phydev, ecmd);
#endif
	adapter->link_config.active_speed = ecmd->base.speed;
	adapter->link_config.active_duplex = ecmd->base.duplex;
	adapter->link_config.autoneg = ecmd->base.autoneg;

	if (adapter->use_lm) {
		lm_link_config.autoneg = adapter->link_config.autoneg;
		lm_link_config.duplex = adapter->link_config.active_duplex;
		lm_link_config.speed = (adapter->link_config.autoneg) ? 0 : adapter->link_config.active_speed;
		/* re-apply configuration on the fly */
		return al_mod_eth_group_lm_link_conf_apply(group_lm_context, &lm_link_config);
	} else {
		if (adapter->up)
			dev_warn(&adapter->pdev->dev,
				"%s this action will take place in the next activation (up)\n",
				__func__);
	}

	return 0;
}
#else
static int
al_mod_eth_get_settings(struct net_device *netdev, struct ethtool_cmd *ecmd)
{
	struct al_mod_eth_adapter *adapter = netdev_priv(netdev);
	struct al_mod_eth_board_params params;
	int rc;

#ifdef CONFIG_PHYLIB
	struct phy_device *phydev = adapter->phydev;

	if (phydev)
		return phy_ethtool_gset(phydev, ecmd);
#endif

	rc = al_mod_eth_board_params_get_ex(&adapter->hal_adapter, &params);
	if (rc) {
		netdev_err(adapter->netdev, "Board info not available\n");
		return rc;
	}

	/* If we are in 25g_10g_autodetect, need to check the speed on the fly */
	if (params.media_type == AL_ETH_BOARD_MEDIA_TYPE_25G_10G_AUTO) {
		rc = al_mod_eth_get_serdes_25g_speed(adapter, &adapter->link_config.active_speed);
		if (rc) {
			netdev_err(adapter->netdev, "Invalid speed read from 25G serdes\n");
			return rc;
		}
	}

	ecmd->speed = adapter->link_config.active_speed;
	ecmd->duplex = adapter->link_config.active_duplex;
	ecmd->autoneg = adapter->link_config.autoneg;

	return 0;
}

static int
al_mod_eth_set_settings(struct net_device *netdev, struct ethtool_cmd *ecmd)
{
	struct al_mod_eth_adapter *adapter = netdev_priv(netdev);

	int rc = 0;
#if defined(CONFIG_PHYLIB)
	struct phy_device *phydev = adapter->phydev;

	if (phydev)
		return phy_ethtool_sset(phydev, ecmd);
#endif

	/* in case no phy exist set only mac parameters */
	adapter->link_config.active_speed = ecmd->speed;
	adapter->link_config.active_duplex = ecmd->duplex;
	adapter->link_config.autoneg = ecmd->autoneg;

	if (adapter->use_lm) {
		struct al_mod_eth_group_lm_context *group_lm_context = adapter->group_lm_context;
		struct al_mod_eth_lm_link_config lm_link_config = {0};
		lm_link_config.autoneg = adapter->link_config.autoneg;
		lm_link_config.duplex = adapter->link_config.active_duplex;
		lm_link_config.speed = (adapter->link_config.autoneg) ? 0 : adapter->link_config.active_speed;
		/* re-apply configuration on the fly */
		return al_mod_eth_group_lm_link_conf_apply(group_lm_context, &lm_link_config);
	} else {
		if (adapter->up)
			dev_warn(&adapter->pdev->dev,
				"%s this action will take place in the next activation (up)\n",
				__func__);
	}

	return rc;
}
#endif

static int
al_mod_eth_module_info(struct net_device *netdev,
				struct ethtool_modinfo *modinfo)
{
	struct al_mod_eth_adapter *adapter = netdev_priv(netdev);

	if (!adapter || !adapter->use_lm || !adapter->up) {
		/* interface is down || lm is not used, no available info */
		return -EINVAL;
	}

	return al_mod_eth_lm_get_module_info(&adapter->lm_context, modinfo);
}

static int
al_mod_eth_module_eeprom(struct net_device *netdev,
				  struct ethtool_eeprom *eeprom, u8 *data)
{
	struct al_mod_eth_adapter *adapter = netdev_priv(netdev);

	if (!adapter || !adapter->use_lm || !adapter->up) {
		/* interface is down || lm is not used, no available info */
		return -EINVAL;
	}

	return al_mod_eth_lm_get_module_eeprom(&adapter->lm_context, eeprom, data);
}

static int al_mod_eth_ethtool_set_coalesce(
			struct net_device *net_dev,
			struct ethtool_coalesce *coalesce)
{
	struct al_mod_eth_adapter *adapter = netdev_priv(net_dev);
	unsigned int tx_usecs = adapter->tx_usecs;
	unsigned int rx_usecs = adapter->rx_usecs;
	uint sb_clk_freq = AL_ETH_REF_CLK_FREQ_TO_HZ(adapter->ref_clk_freq) / 1000;

	/* allow setting coalescing parameters only if adaptive coalescing  isn't enabled*/
	if (adapter->adaptive_intr_rate)
		return -EINVAL;

	if (coalesce->use_adaptive_tx_coalesce)
		return -EINVAL;

	if (coalesce->rx_coalesce_usecs != rx_usecs)
		rx_usecs = coalesce->rx_coalesce_usecs;
	else
		rx_usecs = coalesce->rx_coalesce_usecs_irq;

	if (coalesce->tx_coalesce_usecs != tx_usecs)
		tx_usecs = coalesce->tx_coalesce_usecs;
	else
		tx_usecs = coalesce->tx_coalesce_usecs_irq;

	if (tx_usecs > (uint)(255 * AL_ETH_INTR_MODERATION_RESOLUTION_NSECS(sb_clk_freq) / 1000))
		return -EINVAL;
	if (rx_usecs > (uint)(255 * AL_ETH_INTR_MODERATION_RESOLUTION_NSECS(sb_clk_freq) / 1000))
		return -EINVAL;

	al_mod_eth_set_coalesce(adapter, tx_usecs, rx_usecs);

	return 0;
}

static int al_mod_eth_nway_reset(struct net_device *netdev)
{
#if defined(CONFIG_PHYLIB)
	struct al_mod_eth_adapter *adapter = netdev_priv(netdev);
	struct phy_device *phydev = adapter->phydev;

	if (!phydev)
		return -ENODEV;

	return phy_start_aneg(phydev);
#else
	return -ENODEV;
#endif
}

static u32 al_mod_eth_get_msglevel(struct net_device *netdev)
{
	struct al_mod_eth_adapter *adapter = netdev_priv(netdev);
	return adapter->msg_enable;
}

static void al_mod_eth_set_msglevel(struct net_device *netdev, u32 value)
{
	struct al_mod_eth_adapter *adapter = netdev_priv(netdev);
	adapter->msg_enable = value;
}


#ifdef HAVE_NDO_GET_STATS64
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 11, 0) || \
	(RHEL_RELEASE_CODE && (RHEL_RELEASE_CODE >= RHEL_RELEASE_VERSION(7, 5))))
static void al_mod_eth_get_stats64(struct net_device *netdev,
			       struct rtnl_link_stats64 *stats)
#else
static struct rtnl_link_stats64 *al_mod_eth_get_stats64(struct net_device *netdev,
						    struct rtnl_link_stats64 *stats)
#endif
#else
static struct net_device_stats *al_mod_eth_get_stats(struct net_device *netdev)
#endif
{
	struct al_mod_eth_adapter *adapter = netdev_priv(netdev);
	struct al_mod_eth_mac_stats *mac_stats = &adapter->mac_stats;
#ifdef HAVE_NDO_GET_STATS64
	if (!adapter->up)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 11, 0) || \
	(RHEL_RELEASE_CODE && (RHEL_RELEASE_CODE >= RHEL_RELEASE_VERSION(7, 5))))
		return;
#else
		return NULL;
#endif
#else
	struct net_device_stats *stats;
#ifdef HAVE_NETDEV_STATS_IN_NETDEV
	stats = &netdev->net_stats;
#else
	stats = &adapter->net_stats;
#endif /* HAVE_NETDEV_STATS_IN_NETDEV */
	if (!adapter->up) {
		memset(stats, 0, sizeof(*stats));
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 11, 0) || \
	(RHEL_RELEASE_CODE && (RHEL_RELEASE_CODE >= RHEL_RELEASE_VERSION(7, 5))))
		return;
#else
		return stats;
#endif
	}
#endif /* HAVE_NDO_GET_STATS64 */

	al_mod_eth_mac_stats_get(&adapter->hal_adapter, mac_stats);

	stats->rx_packets = mac_stats->aFramesReceivedOK; /* including pause frames */
	stats->tx_packets = mac_stats->aFramesTransmittedOK; /* including pause frames */
	stats->rx_bytes = mac_stats->aOctetsReceivedOK;
	stats->tx_bytes = mac_stats->aOctetsTransmittedOK;
	stats->rx_dropped = 0;
	stats->multicast = mac_stats->ifInMulticastPkts;
	stats->collisions = 0;

	stats->rx_length_errors = (mac_stats->etherStatsUndersizePkts + /* good but short */
				   mac_stats->etherStatsFragments + /* short and bad*/
				   mac_stats->etherStatsJabbers + /* with crc errors */
				   mac_stats->etherStatsOversizePkts);
	stats->rx_crc_errors = mac_stats->aFrameCheckSequenceErrors;
	stats->rx_frame_errors = mac_stats->aAlignmentErrors;
	stats->rx_fifo_errors = mac_stats->etherStatsDropEvents;
	stats->rx_missed_errors = 0;
	stats->tx_window_errors = 0;

	stats->rx_errors = mac_stats->ifInErrors;
	stats->tx_errors = mac_stats->ifOutErrors;

#if !(LINUX_VERSION_CODE >= KERNEL_VERSION(4, 11, 0) || \
	(RHEL_RELEASE_CODE && (RHEL_RELEASE_CODE >= RHEL_RELEASE_VERSION(7, 5))))
	return stats;
#endif
}

static void
al_mod_eth_get_drvinfo(struct net_device *dev, struct ethtool_drvinfo *info)
{
	struct al_mod_eth_adapter *adapter = netdev_priv(dev);

	strlcpy(info->driver, DRV_MODULE_NAME, sizeof(info->driver));
	strlcpy(info->version, DRV_MODULE_VERSION, sizeof(info->version));
	strlcpy(info->bus_info, pci_name(adapter->pdev), sizeof(info->bus_info));
}

static void
al_mod_eth_get_ringparam(struct net_device *netdev, struct ethtool_ringparam *ring)
{
	struct al_mod_eth_adapter *adapter = netdev_priv(netdev);
	struct al_mod_eth_ring *tx_ring = &adapter->tx_ring[0];
	struct al_mod_eth_ring *rx_ring = &adapter->rx_ring[0];

	ring->rx_max_pending = AL_ETH_MAX_RX_DESCS;
	ring->tx_max_pending = AL_ETH_MAX_TX_SW_DESCS;
	ring->rx_pending = rx_ring->sw_count;
	ring->tx_pending = tx_ring->sw_count;
}

static int al_mod_eth_set_ringparam(struct net_device *netdev, struct ethtool_ringparam *ring)
{
	struct al_mod_eth_adapter *adapter = netdev_priv(netdev);
	unsigned int prev_rx_ring_count = adapter->rx_ring_count;
	unsigned int prev_tx_ring_count = adapter->tx_ring_count;
	bool adapter_was_up = adapter->up;

	if (ring->rx_jumbo_pending) {
		netdev_info(netdev, "%s: rx_jumbo_pending not supported\n",
			    __func__);
		return -EINVAL;
	}
	if (ring->rx_mini_pending) {
		netdev_info(netdev, "%s: rx_mini_pending not supported\n",
			    __func__);
		return -EINVAL;
	}

	if ((ring->rx_pending > AL_ETH_MAX_RX_DESCS) ||
	    (ring->rx_pending < AL_ETH_MIN_RX_DESCS) ||
	    (ring->tx_pending > AL_ETH_MAX_TX_SW_DESCS) ||
	    (ring->tx_pending < AL_ETH_MIN_TX_SW_DESCS) ||
	    !is_power_of_2(ring->rx_pending) || !is_power_of_2(ring->tx_pending)) {
		netdev_info(netdev, "%s: supported ring size range is [%u-%u] and must be a power of 2!\n",
			__func__, AL_ETH_MIN_DESCS_NUM, AL_ETH_MAX_DESCS_NUM);
		return -EINVAL;
	}

	if ((ring->rx_pending == adapter->rx_ring_count) &&
	    (ring->tx_pending == adapter->tx_ring_count))
		return 0;

	/* To support modifying the ring size while the interface is up in a simple manner
	 * (as opposed to allocating new rings with a smooth handoff), we call al_mod_eth_down().
	 * Notice that this will cause a momentary stop of traffic.
	*/
	if (adapter->up)
		al_mod_eth_down(adapter);

	adapter->rx_ring_count = ring->rx_pending;
	adapter->rx_descs_count = ring->rx_pending;
	adapter->tx_ring_count = ring->tx_pending;
	adapter->tx_descs_count = ring->tx_pending;

	/*
	 * TODO: perhaps add a helper which just sets the sw_count/hw_count fields of the
	 * adapter's al_mod_eth_ring structs, instead of re-initializing all fields
	 */
	al_mod_eth_init_rings(adapter);

	/* as a side effect of bringing back the interface, some hw configurations will experience
	 * a momentary reset, e.g. rss indirection table, coalescing, flow contorl
	 */
	if (adapter_was_up)
		al_mod_eth_up(adapter);

	if (ring->rx_pending != prev_rx_ring_count)
		netdev_info(netdev, "RX ring size changed from %u to %u\n",
			    prev_rx_ring_count, ring->rx_pending);
	if (ring->tx_pending != prev_tx_ring_count)
		netdev_info(netdev, "TX ring size changed from %u to %u\n",
			    prev_tx_ring_count, ring->tx_pending);

	return 0;
}

static void
al_mod_eth_get_pauseparam(struct net_device *netdev,
			 struct ethtool_pauseparam *pause)
{
	struct al_mod_eth_adapter *adapter = netdev_priv(netdev);
	struct al_mod_eth_link_config *link_config = &adapter->link_config;

	pause->autoneg = ((link_config->flow_ctrl_active &
					AL_ETH_FLOW_CTRL_AUTONEG) != 0);
	pause->rx_pause = ((link_config->flow_ctrl_active &
					AL_ETH_FLOW_CTRL_RX_PAUSE) != 0);
	pause->tx_pause = ((link_config->flow_ctrl_active &
					AL_ETH_FLOW_CTRL_TX_PAUSE) != 0);
}

static int
al_mod_eth_set_pauseparam(struct net_device *netdev,
			 struct ethtool_pauseparam *pause)
{
	struct al_mod_eth_adapter *adapter = netdev_priv(netdev);
	struct al_mod_eth_link_config *link_config = &adapter->link_config;
#ifdef CONFIG_PHYLIB
	uint32_t newadv;
#endif

	/* auto negotiation and receive pause are currently not supported */
	if (pause->autoneg == AUTONEG_ENABLE)
		return -EINVAL;

	link_config->flow_ctrl_supported = 0;

	if (pause->rx_pause)
		link_config->flow_ctrl_supported |= AL_ETH_FLOW_CTRL_RX_PAUSE;

	if (pause->tx_pause)
		link_config->flow_ctrl_supported |= AL_ETH_FLOW_CTRL_TX_PAUSE;

#ifdef CONFIG_PHYLIB
	if (pause->tx_pause & pause->rx_pause)
		newadv = ADVERTISED_Pause;
	else if (pause->rx_pause)
		newadv = ADVERTISED_Pause | ADVERTISED_Asym_Pause;
	else if (pause->tx_pause)
		newadv = ADVERTISED_Asym_Pause;
	else
		newadv = 0;

	if (pause->autoneg) {
		struct phy_device *phydev;
		uint32_t oldadv;

		phydev = adapter->phydev;
		oldadv = phydev->advertising &
				     (ADVERTISED_Pause | ADVERTISED_Asym_Pause);
		link_config->flow_ctrl_supported |= AL_ETH_FLOW_CTRL_AUTONEG;

		if (oldadv != newadv) {
			phydev->advertising &= ~(ADVERTISED_Pause |
							ADVERTISED_Asym_Pause);
			phydev->advertising |= newadv;

			if (phydev->autoneg)
				return phy_start_aneg(phydev);
		}
	} else {
		link_config->flow_ctrl_active = link_config->flow_ctrl_supported;
		al_mod_eth_flow_ctrl_config(adapter);
	}
#else
	link_config->flow_ctrl_active = link_config->flow_ctrl_supported;
	al_mod_eth_flow_ctrl_config(adapter);
#endif
	return 0;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30)
static int
al_mod_eth_get_rxnfc(struct net_device *netdev, struct ethtool_rxnfc *info,
		 u32 *rules __always_unused)
{
	struct al_mod_eth_adapter *adapter = netdev_priv(netdev);

	switch (info->cmd) {
	case ETHTOOL_GRXRINGS:
		info->data = adapter->num_rx_queues;
		return 0;
/*	case ETHTOOL_GRXFH:
		return bnx2x_get_rss_flags(bp, info);
*/
	default:
		netdev_err(netdev, "Command parameters not supported\n");
		return -EOPNOTSUPP;
	}
}
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,8,0)
static u32 al_mod_eth_get_rxfh_indir_size(struct net_device *netdev)
{
	struct al_mod_eth_adapter *adapter = netdev_priv(netdev);

	return adapter->rss_ind_tbl_size;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,19,0)
static int al_mod_eth_get_rxfh_indir(struct net_device *netdev, u32 *indir, u8 *key, u8 *hfunc)
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(3,16,0)
static int al_mod_eth_get_rxfh_indir(struct net_device *netdev, u32 *indir, u8 *key)
#else
static int al_mod_eth_get_rxfh_indir(struct net_device *netdev, u32 *indir)
#endif
{
	struct al_mod_eth_adapter *adapter = netdev_priv(netdev);
	int i;

	if (indir) {
		for (i = 0; i < adapter->rss_ind_tbl_size; i++)
			indir[i] = adapter->rss_ind_tbl[i];
	} else {
		return -EOPNOTSUPP;
	}

	return 0;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,19,0)
static int al_mod_eth_set_rxfh_indir(struct net_device *netdev, const u32 *indir, const u8 *key,
	const u8 hfunc)
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(3,16,0)
static int al_mod_eth_set_rxfh_indir(struct net_device *netdev, const u32 *indir, const u8 *key)
#else
static int al_mod_eth_set_rxfh_indir(struct net_device *netdev, const u32 *indir)
#endif
{
	struct al_mod_eth_adapter *adapter = netdev_priv(netdev);
	size_t i;

	if (indir) {
		if (IS_ETH_V4_ADV(adapter->rev_id, adapter->dev_id)) {
			int rc;
			struct al_mod_eth_rfw_lrss_params params = {
				.indir_table = adapter->rss_ind_tbl,
				.table_size = adapter->rss_ind_tbl_size,
				.udma_num = adapter->default_rx_udma_num,
			};

			for (i = 0; i < adapter->rss_ind_tbl_size; i++)
				adapter->rss_ind_tbl[i] = indir[i];

			rc = al_mod_eth_rfw_lrss_generic_config(&adapter->rfw_handle, &params);
			if (rc != 0)
				netdev_err(adapter->netdev,
					   "%s: Error on al_mod_eth_rfw_lrss_generic_config (rc = %d)",
					   __func__, rc);

			return rc;
		} else {
			for (i = 0; i < adapter->rss_ind_tbl_size; i++) {
				adapter->rss_ind_tbl[i] = indir[i];
				al_mod_eth_thash_table_set(&adapter->hal_adapter,
						       i,
						       adapter->default_rx_udma_num,
						       indir[i]);
			}
		}
	} else {
		return -EOPNOTSUPP;
	}

	return 0;
}
#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(3,8,0) */

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,0,0)
static void al_mod_eth_get_channels(struct net_device *netdev,
				struct ethtool_channels *channels)
{
	struct al_mod_eth_adapter *adapter = netdev_priv(netdev);

	channels->max_rx = AL_ETH_UDMA_HW_QUEUES_BY_REV(adapter->rev_id);
	channels->max_tx = AL_ETH_UDMA_HW_QUEUES_BY_REV(adapter->rev_id);
	channels->max_other = 0;
	channels->max_combined = 0;
	channels->rx_count = adapter->num_rx_queues;
	channels->tx_count = adapter->num_tx_queues;
	channels->other_count = 0;
	channels->combined_count = 0;
}
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0)
#if defined(CONFIG_PHYLIB) || defined(CONFIG_ARCH_ALPINE)
static int al_mod_eth_get_eee(struct net_device *netdev,
			  struct ethtool_eee *edata)
{
	struct al_mod_eth_adapter *adapter = netdev_priv(netdev);
	struct al_mod_eth_eee_params params;


	if (!adapter->phy_exist)
		return -EOPNOTSUPP;

	al_mod_eth_eee_get(&adapter->hal_adapter, &params);

	edata->eee_enabled = params.enable;
	edata->tx_lpi_timer = params.tx_eee_timer;

	return phy_ethtool_get_eee(adapter->phydev, edata);
}


static int al_mod_eth_set_eee(struct net_device *netdev,
			  struct ethtool_eee *edata)
{
	struct al_mod_eth_adapter *adapter = netdev_priv(netdev);
	struct al_mod_eth_eee_params params;

	struct phy_device *phydev;

	if (!adapter->phy_exist)
		return -EOPNOTSUPP;

	phydev = adapter->phydev;

	phy_init_eee(phydev, 1);

	params.enable = edata->eee_enabled;
	params.tx_eee_timer = edata->tx_lpi_timer;
	params.min_interval = 10;

	al_mod_eth_eee_config(&adapter->hal_adapter, &params);

	return phy_ethtool_set_eee(phydev, edata);
}
#else /* defined(CONFIG_PHYLIB) || defined(CONFIG_ARCH_ALPINE) */
static int al_mod_eth_get_eee(struct net_device *netdev,
			  struct ethtool_eee *edata)
{
	return -EOPNOTSUPP;
}

static int al_mod_eth_set_eee(struct net_device *netdev,
			  struct ethtool_eee *edata)
{
	return -EOPNOTSUPP;
}
#endif /* defined(CONFIG_PHYLIB) || defined(CONFIG_ARCH_ALPINE) */
#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0) */

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0))
#if defined(CONFIG_PHYLIB) || defined(CONFIG_ARCH_ALPINE)
static void al_mod_eth_get_wol(struct net_device *netdev,
			   struct ethtool_wolinfo *wol)
{
	struct al_mod_eth_adapter *adapter = netdev_priv(netdev);
	struct phy_device *phydev;

	wol->wolopts = adapter->wol;

	if ((adapter) && (adapter->phy_exist) && (adapter->mdio_bus)) {
		phydev = adapter->phydev;
		if (phydev) {
			phy_ethtool_get_wol(phydev, wol);
			wol->supported |= WAKE_PHY;
			return;
		}
	}

	wol->supported |= WAKE_UCAST | WAKE_MCAST | WAKE_BCAST;
}

static int al_mod_eth_set_wol(struct net_device *netdev, struct ethtool_wolinfo *wol)
{
	struct al_mod_eth_adapter *adapter = netdev_priv(netdev);
	struct phy_device *phydev;

	if (wol->wolopts & (WAKE_ARP | WAKE_MAGICSECURE))
		return -EOPNOTSUPP;

	adapter->wol = wol->wolopts;

	if ((adapter) && (adapter->phy_exist) && (adapter->mdio_bus)) {
		phydev = adapter->phydev;
		if (phydev)
			return phy_ethtool_set_wol(phydev, wol);
	}

	device_set_wakeup_enable(&adapter->pdev->dev, adapter->wol);

	return 0;
}
#else /* defined(CONFIG_PHYLIB) || defined(CONFIG_ARCH_ALPINE) */
static void al_mod_eth_get_wol(struct net_device *netdev,
			   struct ethtool_wolinfo *wol)
{
	struct al_mod_eth_adapter *adapter = netdev_priv(netdev);

	wol->wolopts = adapter->wol;

	wol->supported |= WAKE_UCAST | WAKE_MCAST | WAKE_BCAST;
}

static int al_mod_eth_set_wol(struct net_device *netdev, struct ethtool_wolinfo *wol)
{
	struct al_mod_eth_adapter *adapter = netdev_priv(netdev);

	if (wol->wolopts & (WAKE_ARP | WAKE_MAGICSECURE))
		return -EOPNOTSUPP;

	adapter->wol = wol->wolopts;

	device_set_wakeup_enable(&adapter->pdev->dev, adapter->wol);

	return 0;
}
#endif /* defined(CONFIG_PHYLIB) || defined(CONFIG_ARCH_ALPINE) */
#endif /* (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0)) */


/****************** ETHTOOL_STATS BEGIN ******************/

struct al_mod_eth_ethtool_stats {
	char name[ETH_GSTRING_LEN];
	int stat_offset;
};

#define AL_ETH_ETHTOOL_STAT_ENTRY(stat, stat_type) { \
	.name = #stat, \
	.stat_offset = offsetof(struct al_mod_eth_stats_##stat_type, stat) \
}

#define AL_ETH_ETHTOOL_STAT_RX_ENTRY(stat) \
	AL_ETH_ETHTOOL_STAT_ENTRY(stat, rx)

#define AL_ETH_ETHTOOL_STAT_TX_ENTRY(stat) \
	AL_ETH_ETHTOOL_STAT_ENTRY(stat, tx)

#define AL_ETH_ETHTOOL_STAT_GLOBAL_ENTRY(stat) \
	AL_ETH_ETHTOOL_STAT_ENTRY(stat, dev)

static const struct al_mod_eth_ethtool_stats al_mod_eth_ethtool_stats_global_strings[] = {
	AL_ETH_ETHTOOL_STAT_GLOBAL_ENTRY(tx_timeout),
	AL_ETH_ETHTOOL_STAT_GLOBAL_ENTRY(interface_up),
	AL_ETH_ETHTOOL_STAT_GLOBAL_ENTRY(interface_down),
	AL_ETH_ETHTOOL_STAT_GLOBAL_ENTRY(fec_ce),
	AL_ETH_ETHTOOL_STAT_GLOBAL_ENTRY(fec_ue),
	AL_ETH_ETHTOOL_STAT_GLOBAL_ENTRY(ua_errors),
	AL_ETH_ETHTOOL_STAT_GLOBAL_ENTRY(ec_errors),
	AL_ETH_ETHTOOL_STAT_GLOBAL_ENTRY(mac_errors),
	AL_ETH_ETHTOOL_STAT_GLOBAL_ENTRY(udma_errors),
};

static const struct al_mod_eth_ethtool_stats al_mod_eth_ethtool_stats_tx_strings[] = {
	AL_ETH_ETHTOOL_STAT_TX_ENTRY(packets),
	AL_ETH_ETHTOOL_STAT_TX_ENTRY(bytes),
	AL_ETH_ETHTOOL_STAT_TX_ENTRY(tso_small_mss),
	AL_ETH_ETHTOOL_STAT_TX_ENTRY(tso_mss_seg_pad_bad),
};

static const struct al_mod_eth_ethtool_stats al_mod_eth_ethtool_stats_rx_strings[] = {
	AL_ETH_ETHTOOL_STAT_RX_ENTRY(packets),
	AL_ETH_ETHTOOL_STAT_RX_ENTRY(bytes),
	AL_ETH_ETHTOOL_STAT_RX_ENTRY(skb_alloc_fail),
	AL_ETH_ETHTOOL_STAT_RX_ENTRY(dma_mapping_err),
	AL_ETH_ETHTOOL_STAT_RX_ENTRY(small_copy_len_pkt),
	AL_ETH_ETHTOOL_STAT_RX_ENTRY(buf_alloc_fail),
	AL_ETH_ETHTOOL_STAT_RX_ENTRY(partial_rx_ring_refill),
};

#define AL_ETH_ETHTOOL_STATS_ARRAY_GLOBAL	ARRAY_SIZE(al_mod_eth_ethtool_stats_global_strings)
#define AL_ETH_ETHTOOL_STATS_ARRAY_TX	ARRAY_SIZE(al_mod_eth_ethtool_stats_tx_strings)
#define AL_ETH_ETHTOOL_STATS_ARRAY_RX	ARRAY_SIZE(al_mod_eth_ethtool_stats_rx_strings)

static void al_mod_eth_safe_update_stat(u64 *src, u64 *dst,
				 struct u64_stats_sync *syncp)
{
	unsigned int start;

	do {
		start = u64_stats_fetch_begin_irq(syncp);
		*(dst) = *src;
	} while (u64_stats_fetch_retry_irq(syncp, start));
}

static void al_mod_eth_ethtool_queue_stats(struct al_mod_eth_adapter *adapter, u64 **data)
{
	const struct al_mod_eth_ethtool_stats *al_mod_eth_ethtool_stats;
	struct al_mod_eth_ring *ring;

	u64 *ptr;
	int i, j;

	for (i = 0; i < adapter->num_tx_queues; i++) {
		/* Tx stats */
		ring = &adapter->tx_ring[i];

		for (j = 0; j < AL_ETH_ETHTOOL_STATS_ARRAY_TX; j++) {
			al_mod_eth_ethtool_stats = &al_mod_eth_ethtool_stats_tx_strings[j];

			ptr = (u64 *)((uintptr_t)&ring->tx_stats +
				(uintptr_t)al_mod_eth_ethtool_stats->stat_offset);

			al_mod_eth_safe_update_stat(ptr, (*data)++, &ring->syncp);
		}
	}

	for (i = 0; i < adapter->num_rx_queues; i++) {
		/* Rx stats */
		ring = &adapter->rx_ring[i];

		for (j = 0; j < AL_ETH_ETHTOOL_STATS_ARRAY_RX; j++) {
			al_mod_eth_ethtool_stats = &al_mod_eth_ethtool_stats_rx_strings[j];

			ptr = (u64 *)((uintptr_t)&ring->rx_stats +
				(uintptr_t)al_mod_eth_ethtool_stats->stat_offset);

			al_mod_eth_safe_update_stat(ptr, (*data)++, &ring->syncp);
		}
	}
}

static void al_mod_eth_get_ethtool_stats(struct net_device *netdev,
				  struct ethtool_stats *stats,
				  u64 *data)
{
	struct al_mod_eth_adapter *adapter = netdev_priv(netdev);
	const struct al_mod_eth_ethtool_stats *al_mod_eth_ethtool_stats;
	u64 *ptr;
	int i;
	u32 ce = 0, ue = 0;
	int rc;

	if (adapter->dev_id == AL_ETH_DEV_ID_ADVANCED) {
		/*
		 * Note: fec_stats_get may return some errors (~50) generated
		 * during link state changes.
		 * error return code will mean the feature is not supported,
		 * no need for special handling
		 */
		rc = al_mod_eth_fec_stats_get(&adapter->hal_adapter, &ce, &ue);
		if (!rc) {
			adapter->dev_stats.fec_ce += ce;
			adapter->dev_stats.fec_ue += ue;
		}
	}

	for (i = 0; i < AL_ETH_ETHTOOL_STATS_ARRAY_GLOBAL; i++) {
		al_mod_eth_ethtool_stats = &al_mod_eth_ethtool_stats_global_strings[i];

		ptr = (u64 *)((uintptr_t)&adapter->dev_stats +
			(uintptr_t)al_mod_eth_ethtool_stats->stat_offset);

		al_mod_eth_safe_update_stat(ptr, data++, &adapter->syncp);
	}

	al_mod_eth_ethtool_queue_stats(adapter, &data);
}

int al_mod_eth_get_sset_count(struct net_device *netdev, int sset)
{
	struct al_mod_eth_adapter *adapter = netdev_priv(netdev);

	if (sset != ETH_SS_STATS)
		return -EOPNOTSUPP;

	return  (adapter->num_tx_queues * AL_ETH_ETHTOOL_STATS_ARRAY_TX)
		+ (adapter->num_rx_queues * AL_ETH_ETHTOOL_STATS_ARRAY_RX)
		+ AL_ETH_ETHTOOL_STATS_ARRAY_GLOBAL;
}

static void al_mod_eth_queue_strings(struct al_mod_eth_adapter *adapter, u8 **data)
{
	const struct al_mod_eth_ethtool_stats *al_mod_eth_ethtool_stats;
	int i, j;

	for (i = 0; i < adapter->num_tx_queues; i++) {
		/* Tx stats */
		for (j = 0; j < AL_ETH_ETHTOOL_STATS_ARRAY_TX; j++) {
			al_mod_eth_ethtool_stats = &al_mod_eth_ethtool_stats_tx_strings[j];

			snprintf(*data, ETH_GSTRING_LEN,
				 "queue_%u_tx_%s", i, al_mod_eth_ethtool_stats->name);
			 (*data) += ETH_GSTRING_LEN;
		}
	}

	for (i = 0; i < adapter->num_rx_queues; i++) {
		/* Rx stats */
		for (j = 0; j < AL_ETH_ETHTOOL_STATS_ARRAY_RX; j++) {
			al_mod_eth_ethtool_stats = &al_mod_eth_ethtool_stats_rx_strings[j];

			snprintf(*data, ETH_GSTRING_LEN,
				 "queue_%u_rx_%s", i, al_mod_eth_ethtool_stats->name);
			(*data) += ETH_GSTRING_LEN;
		}
	}
}

static void al_mod_eth_get_strings(struct net_device *netdev, u32 sset, u8 *data)
{
	struct al_mod_eth_adapter *adapter = netdev_priv(netdev);
	const struct al_mod_eth_ethtool_stats *al_mod_eth_ethtool_stats;
	int i;

	if (sset != ETH_SS_STATS)
		return;

	for (i = 0; i < AL_ETH_ETHTOOL_STATS_ARRAY_GLOBAL; i++) {
		al_mod_eth_ethtool_stats = &al_mod_eth_ethtool_stats_global_strings[i];

		memcpy(data, al_mod_eth_ethtool_stats->name, ETH_GSTRING_LEN);
		data += ETH_GSTRING_LEN;
	}

	al_mod_eth_queue_strings(adapter, &data);
}

/****************** ETHTOOL_STATS END ******************/

static const struct ethtool_ops al_mod_eth_ethtool_ops = {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 6, 0))
	.get_link_ksettings	= al_mod_eth_get_link_ksettings,
	.set_link_ksettings	= al_mod_eth_set_link_ksettings,
#else
	.get_settings		= al_mod_eth_get_settings,
	.set_settings		= al_mod_eth_set_settings,
#endif
	.get_drvinfo		= al_mod_eth_get_drvinfo,
/*	.get_regs_len		= al_mod_eth_get_regs_len,*/
/*	.get_regs		= al_mod_eth_get_regs,*/
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0))
	.get_wol		= al_mod_eth_get_wol,
	.set_wol		= al_mod_eth_set_wol,
#endif
	.get_msglevel		= al_mod_eth_get_msglevel,
	.set_msglevel		= al_mod_eth_set_msglevel,

	.nway_reset		= al_mod_eth_nway_reset,
	.get_link		= ethtool_op_get_link,
	.get_coalesce		= al_mod_eth_get_coalesce,
	.set_coalesce		= al_mod_eth_ethtool_set_coalesce,
	.get_ringparam		= al_mod_eth_get_ringparam,
	.set_ringparam		= al_mod_eth_set_ringparam,
	.get_pauseparam		= al_mod_eth_get_pauseparam,
	.set_pauseparam		= al_mod_eth_set_pauseparam,
/*	.self_test		= al_mod_eth_self_test,*/
	.get_strings		= al_mod_eth_get_strings,
/*	.set_phys_id		= al_mod_eth_set_phys_id,*/
	.get_ethtool_stats	= al_mod_eth_get_ethtool_stats,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30)
	.get_rxnfc		= al_mod_eth_get_rxnfc,
#endif
	.get_sset_count		= al_mod_eth_get_sset_count,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,8,0)
	.get_rxfh_indir_size    = al_mod_eth_get_rxfh_indir_size,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,16,0)
	.get_rxfh			= al_mod_eth_get_rxfh_indir,
	.set_rxfh			= al_mod_eth_set_rxfh_indir,
#else
	.get_rxfh_indir		= al_mod_eth_get_rxfh_indir,
	.set_rxfh_indir		= al_mod_eth_set_rxfh_indir,
#endif
#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(3,8,0) */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,0,0)
	.get_channels		= al_mod_eth_get_channels,
/*	.set_channels		= al_mod_eth_set_channels,*/
#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(3,0,0) */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0)
	.get_eee		= al_mod_eth_get_eee,
	.set_eee		= al_mod_eth_set_eee,
#endif
	.get_module_info	= al_mod_eth_module_info,
	.get_module_eeprom	= al_mod_eth_module_eeprom,
};

static void al_mod_eth_ipv4_tcp_header_dump(struct al_mod_eth_adapter *adapter, struct sk_buff *skb)
{
	struct iphdr *ip_header;
	struct tcphdr *tcp_header;
	unsigned char *ip_header_start;
	unsigned char *tcp_header_start;
	unsigned int ip_header_len;
	unsigned int tcp_header_len;
	int i;

	ip_header = ip_hdr(skb);
	tcp_header = tcp_hdr(skb);
	netdev_info(adapter->netdev, "SIP:%pI4, DIP:%pI4, SrcPort:%hu, DstPort:%hu\n",
		&ip_header->saddr,
		&ip_header->daddr,
		htons(tcp_header->source),
		htons(tcp_header->dest));

	ip_header_start = skb_network_header(skb);
	ip_header_len = ip_hdrlen(skb);
	netdev_info(adapter->netdev, "Raw dump of IP header:");
	/* ip header length is always a multiple of 4 bytes */
	for (i = 0; i < ip_header_len / 4; i++)
		netdev_warn(adapter->netdev, "0x%08x", htonl(((u32 *)ip_header_start)[i]));

	tcp_header_start = skb_transport_header(skb);
	tcp_header_len = tcp_hdrlen(skb);
	netdev_info(adapter->netdev, "Raw dump of TCP header:");
	/* tcp header length is always a multiple of 4 bytes */
	for (i = 0; i < tcp_header_len / 4; i++)
		netdev_warn(adapter->netdev, "0x%08x", htonl(((u32 *)tcp_header_start)[i]));
}

#define AL_ETH_TSO_MIN_MSS 64

static int
al_mod_eth_tx_csum(struct al_mod_eth_adapter *adapter,
	       struct al_mod_eth_ring *tx_ring,
	       struct al_mod_eth_tx_buffer *tx_info,
	       struct al_mod_eth_pkt *hal_pkt, struct sk_buff *skb)
{
	u32 mss = skb_shinfo(skb)->gso_size;
	u8 l4_protocol = 0;

	if ((skb->ip_summed == CHECKSUM_PARTIAL) || mss) {
		struct al_mod_eth_meta_data *meta = &tx_ring->hal_meta;
		if (mss)
			hal_pkt->flags |= AL_ETH_TX_FLAGS_TSO|AL_ETH_TX_FLAGS_L4_CSUM;
		else
			hal_pkt->flags |= AL_ETH_TX_FLAGS_L4_CSUM|AL_ETH_TX_FLAGS_L4_PARTIAL_CSUM;

		switch (ip_hdr(skb)->version) {
		case IPVERSION:
			hal_pkt->l3_proto_idx = AL_ETH_PROTO_ID_IPv4;
			if (mss)
				hal_pkt->flags |= AL_ETH_TX_FLAGS_IPV4_L3_CSUM;
			l4_protocol = ip_hdr(skb)->protocol;
			break;
		case 6:
			hal_pkt->l3_proto_idx = AL_ETH_PROTO_ID_IPv6;
			/* TODO: add support for csum offloading for ipv6 with options */
			l4_protocol = ipv6_hdr(skb)->nexthdr;
			break;
		default:
			break;
		}

		if (l4_protocol == IPPROTO_TCP) {
			hal_pkt->l4_proto_idx = AL_ETH_PROTO_ID_TCP;

			if (mss) {
				unsigned int tcp_header_len, header_len, total_payload_len;

				tcp_header_len = tcp_hdrlen(skb);
				header_len = skb_transport_offset(skb) + tcp_header_len;
				total_payload_len = skb->len - header_len;

				if ((mss < AL_ETH_TSO_MIN_MSS) || (mss >= total_payload_len)) {
					/* We should never reach here, due to the checks in
					 * al_mod_eth_validate_skb_tso_features(), but better be
					 * safe than sorry.
					 */

					netdev_err(adapter->netdev, "Dropping skb due to invalid MSS size (%dB) for TSO. skb_len:%d, header_len:%d, tcp_header_len:%u\n",
						mss,
						skb->len,
						header_len,
						tcp_header_len);
					al_mod_eth_ipv4_tcp_header_dump(adapter, skb);

					return -EINVAL;
				}
			}
		} else if (l4_protocol == IPPROTO_UDP) {
			hal_pkt->l4_proto_idx = AL_ETH_PROTO_ID_UDP;
		} else {
			/* TODO: Need to add support for ipv6 when nexthdr is an ipv6 option */
			netdev_warn(adapter->netdev, "%s: l4 protocol isn't TCP(6) or UDP(17). proto:%d\n",
				    __func__, l4_protocol);
		}

		if (skb->protocol == __constant_htons(ETH_P_8021Q))
			hal_pkt->source_vlan_count = 1;
		else
			hal_pkt->source_vlan_count = 0;

		meta->words_valid = 4;
		meta->l3_header_len = skb_network_header_len(skb);
		meta->l3_header_offset = skb_network_offset(skb) -
			(VLAN_HLEN * hal_pkt->source_vlan_count);
		meta->l4_header_len = tcp_hdr(skb)->doff; /* this param needed only for TSO */
		meta->mss_idx_sel = 0; /* TODO: check how to select MSS */
		meta->mss_val = mss;
		hal_pkt->meta = meta;
	} else {
		hal_pkt->meta = NULL;
	}

	return 0;
}

/* Called with netif_tx_lock.
 */
static netdev_tx_t
al_mod_eth_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct al_mod_eth_adapter *adapter = netdev_priv(dev);
	dma_addr_t dma;
	struct al_mod_eth_tx_buffer *tx_info;
	struct al_mod_eth_pkt *hal_pkt;
	struct al_mod_buf *al_mod_buf;
	u32 len, last_frag;
	u16 next_to_use;
	int i = 0, qid;
	struct al_mod_eth_ring *tx_ring;
	struct netdev_queue *txq;
	int rc;

	netdev_dbg(adapter->netdev, "%s skb %p\n", __func__, skb);
	/*  Determine which tx ring we will be placed on */
	qid = skb_get_queue_mapping(skb);
	tx_ring = &adapter->tx_ring[qid];
	txq = netdev_get_tx_queue(dev, qid);

	/* Need to pad the frame to ETH minimal, since MAC padding isn't zeroed
	 * when packet size is <60 and not 4-byte aligned
	 */
	if (eth_skb_pad(skb)) {
		netdev_dbg(adapter->netdev, "failed to pad skb to minimum ETH size (60B). skb->len = %d, skb->data_len= %d\n",
			   skb->len, skb->data_len);
		/* eth_skb_pad frees skb on error */
		return NETDEV_TX_OK;
	}

	next_to_use = tx_ring->next_to_use;
	tx_info = &tx_ring->tx_buffer_info[next_to_use];
	tx_info->skb = skb;
	hal_pkt = &tx_info->hal_pkt;

	/* set flags and meta data */
	hal_pkt->flags = AL_ETH_TX_FLAGS_INT;
	rc = al_mod_eth_tx_csum(adapter, tx_ring, tx_info, hal_pkt, skb);
	if (rc != 0)
		goto tx_csum_error;

	al_mod_buf = hal_pkt->bufs;

	len = skb_headlen(skb);
	dma = dma_map_single(tx_ring->dev, skb->data, len, DMA_TO_DEVICE);
	if (dma_mapping_error(tx_ring->dev, dma)) {
		dev_kfree_skb(skb);
		return NETDEV_TX_OK;
	}
	dma_unmap_addr_set(al_mod_buf, addr, dma);
	dma_unmap_len_set(al_mod_buf, len, len);

	last_frag = skb_shinfo(skb)->nr_frags;

	for (i = 0; i < last_frag; i++) {
		const skb_frag_t *frag = &skb_shinfo(skb)->frags[i];

		al_mod_buf++;

		len = skb_frag_size(frag);
		dma = skb_frag_dma_map(tx_ring->dev, frag, 0, len,
				       DMA_TO_DEVICE);
		if (dma_mapping_error(tx_ring->dev, dma))
			goto dma_error;
		dma_unmap_addr_set(al_mod_buf, addr, dma);
		dma_unmap_len_set(al_mod_buf, len, len);
	}

	hal_pkt->num_of_bufs = 1 + last_frag;
	if (unlikely(last_frag > (AL_ETH_PKT_MAX_BUFS - 2))) {
		int i;
		netdev_err(adapter->netdev, "too much descriptors. last_frag %d!\n", last_frag);
		for (i = 0; i <= last_frag; i++)
			netdev_err(adapter->netdev, "frag[%d]: addr:0x%llx, len 0x%x\n",
					i, (unsigned long long)hal_pkt->bufs[i].addr, hal_pkt->bufs[i].len);
		BUG();
	}
	netdev_tx_sent_queue(txq, skb->len);

	u64_stats_update_begin(&tx_ring->syncp);
	tx_ring->tx_stats.packets++;
	tx_ring->tx_stats.bytes += skb->len;
	u64_stats_update_end(&tx_ring->syncp);

	/*smp_wmb();*/ /* commit the item before incrementing the head */
	tx_ring->next_to_use = AL_ETH_TX_RING_IDX_NEXT(tx_ring, next_to_use);

	/* prepare the packet's descriptors to dma engine */
	tx_info->tx_descs = al_mod_eth_tx_pkt_prepare(tx_ring->dma_q, hal_pkt);

	/* stop the queue when no more space available, the packet can have up
	 * to MAX_SKB_FRAGS + 1 buffers and a meta descriptor */
	if (unlikely(al_mod_udma_available_get(tx_ring->dma_q) <
				(MAX_SKB_FRAGS + 2))) {
		dev_dbg(&adapter->pdev->dev, "%s stop queue %d\n", __func__, qid);
		netif_tx_stop_queue(txq);
	}

	/* trigger the dma engine */
	al_mod_eth_tx_dma_action(tx_ring->dma_q, tx_info->tx_descs);

	return NETDEV_TX_OK;

tx_csum_error:
dma_error:
	/* save value of frag that failed */
	last_frag = i;

	/* start back at beginning and unmap skb */
	tx_info->skb = NULL;
	al_mod_buf = hal_pkt->bufs;
	dma_unmap_single(tx_ring->dev, dma_unmap_addr(al_mod_buf, addr),
			 dma_unmap_len(al_mod_buf, len), DMA_TO_DEVICE);

	/* unmap remaining mapped pages */
	for (i = 0; i < last_frag; i++) {
		al_mod_buf++;
		dma_unmap_page(tx_ring->dev, dma_unmap_addr(al_mod_buf, addr),
			       dma_unmap_len(al_mod_buf, len), DMA_TO_DEVICE);
	}

	dev_kfree_skb(skb);
	return NETDEV_TX_OK;
}

#if (defined HAVE_NETDEV_SELECT_QUEUE) || (defined HAVE_NET_DEVICE_OPS)
/* Return subqueue id on this core (one per core). */
static u16 al_mod_eth_select_queue(struct net_device *dev, struct sk_buff *skb
#if (defined HAVE_NDO_SELECT_QUEUE_ACCEL) || (defined HAVE_NDO_SELECT_QUEUE_ACCEL_FALLBACK)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 19, 0)
	, struct net_device *sb_dev
#else
	, void *accel_priv
#endif
#endif
#if (defined HAVE_NDO_SELECT_QUEUE_ACCEL_FALLBACK)
	, select_queue_fallback_t fallback
#endif
	)
{
	struct al_mod_eth_adapter *adapter = netdev_priv(dev);

	u16 qid;
	/* we suspect that this is good for in--kernel network services that want to loop incoming skb rx to tx
	 * in normal user generated traffic, most probably we will not get to this */
	if(skb_rx_queue_recorded(skb)) {
		qid = skb_get_rx_queue(skb);
		pr_debug("sel_rec_qid=%d\n", qid);
	}
	else {
#ifdef CONFIG_ARCH_ALPINE
		/** Is an integrated networking driver */
		qid = smp_processor_id();
#else
		/** Is a host/NIC mode driver */
#if (defined HAVE_NDO_SELECT_QUEUE_ACCEL_FALLBACK)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 19, 0)
		qid = fallback(dev, skb, NULL);
#else
		qid = fallback(dev, skb);
#endif
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(3,9,0)
		qid = __netdev_pick_tx(dev, skb);
#else
		qid = skb_tx_hash(dev, skb);
#endif /* HAVE_NDO_SELECT_QUEUE_ACCEL_FALLBACK */
#endif /* CONFIG_ARCH_ALPINE */
		pr_debug("sel_smp_qid=%d\n", qid);
	}

	return (qid % adapter->num_tx_queues);
}
#endif /* (defined HAVE_NETDEV_SELECT_QUEUE) || (defined HAVE_NET_DEVICE_OPS) */


static int al_mod_eth_set_mac_addr(struct net_device *dev, void *p)
{
	struct al_mod_eth_adapter *adapter = netdev_priv(dev);
	struct sockaddr *addr = p;
	int err = 0;

	if (!is_valid_ether_addr(addr->sa_data))
		return -EADDRNOTAVAIL;

	memcpy(dev->dev_addr, addr->sa_data, dev->addr_len);
	memcpy(adapter->mac_addr, addr->sa_data, dev->addr_len);
	al_mod_eth_mac_table_unicast_add(adapter, AL_ETH_MAC_TABLE_UNICAST_IDX_BASE,
				     adapter->mac_addr, 1 << adapter->default_rx_udma_num);

	if (!netif_running(dev))
		return 0;

	return err;
}

#ifdef HAVE_SET_RX_MODE
/**
 *  Unicast, Multicast and Promiscuous mode set
 *  @netdev: network interface device structure
 *
 *  The set_rx_mode entry point is called whenever the unicast or multicast
 *  address lists or the network interface flags are updated.  This routine is
 *  responsible for configuring the hardware for proper unicast, multicast,
 *  promiscuous mode, and all-multi behavior.
 **/
static void al_mod_eth_set_rx_mode(struct net_device *netdev)
{
	struct al_mod_eth_adapter *adapter = netdev_priv(netdev);

	if (adapter->rev_id >= AL_ETH_REV_ID_4) {
		/**
		 * al_mod_eth_mac_table_* uses legacy logic not in use in eth v4 adv
		 * (due to DDP datapath)
		 * In addition - promiscuous mode is always on at eth v4 adv 25/50G MAC
		 */
		return;
	}

	if (netdev->flags & IFF_PROMISC) {
		al_mod_eth_mac_table_promiscuous_set(adapter, true, 1 << adapter->default_rx_udma_num);
	} else {

		if (netdev->flags & IFF_ALLMULTI) {
			al_mod_eth_mac_table_all_multicast_add(adapter,
							   AL_ETH_MAC_TABLE_ALL_MULTICAST_IDX,
							   1 << adapter->default_rx_udma_num);
		} else {
			if (netdev_mc_empty(netdev))
				al_mod_eth_mac_table_entry_clear(adapter,
					AL_ETH_MAC_TABLE_ALL_MULTICAST_IDX);
			else
				al_mod_eth_mac_table_all_multicast_add(adapter,
								AL_ETH_MAC_TABLE_ALL_MULTICAST_IDX,
								1 << adapter->default_rx_udma_num);
		}

		if (!netdev_uc_empty(netdev)) {
			struct netdev_hw_addr *ha;
			uint8_t i = AL_ETH_MAC_TABLE_UNICAST_IDX_BASE + 1;

			if (netdev_uc_count(netdev) >
			    (AL_ETH_MAC_TABLE_UNICAST_MAX_COUNT - 1)) {
				/* In this case there are more addresses then
				 * entries in the mac table - set promiscuous */
				al_mod_eth_mac_table_promiscuous_set(adapter, true,
								 1 << adapter->default_rx_udma_num);
				return;
			}

			/* clear the last configuration */
			while (i < (AL_ETH_MAC_TABLE_UNICAST_IDX_BASE +
				    AL_ETH_MAC_TABLE_UNICAST_MAX_COUNT)) {
				al_mod_eth_mac_table_entry_clear(adapter, i);
				i++;
			}

			/* set new addresses */
			i = AL_ETH_MAC_TABLE_UNICAST_IDX_BASE + 1;
			netdev_for_each_uc_addr(ha, netdev) {
				al_mod_eth_mac_table_unicast_add(adapter, i,
							     ha->addr,
							     1 << adapter->default_rx_udma_num);
				i++;
			}
		}

		al_mod_eth_mac_table_promiscuous_set(adapter, false, 1 << adapter->default_rx_udma_num);
	}
}
#else /* HAVE_SET_RX_MODE */
static void al_mod_eth_set_rx_mode(struct net_device *netdev)
{
}
#endif /* HAVE_SET_RX_MODE */

#if defined(HAVE_NDO_FEATURES_CHECK) || defined(HAVE_NDO_GSO_CHECK)
static int al_mod_eth_mss_segment_pad_ok(u32 mss, u32 total_payload_len, u32 header_len)
{
	int res_payload_len;

	res_payload_len = total_payload_len % mss;

	/* check that the last packet which will be semgented by TSO, is larger than the
	 * ETH minimum length
	 */
	if (res_payload_len != 0)
		return ((header_len + res_payload_len) >= ETH_ZLEN);
	else
		return ((header_len + mss) >= ETH_ZLEN);
}

static netdev_features_t al_mod_eth_skb_tso_features_check(struct sk_buff *skb,
							  struct net_device *dev,
							  netdev_features_t features)
{
	int mss;
	unsigned int header_len, total_payload_len;
	struct al_mod_eth_ring *tx_ring;
	struct al_mod_eth_adapter *adapter;

	if (skb_shinfo(skb)->gso_type & (SKB_GSO_TCPV4 | SKB_GSO_TCPV6)) {
		adapter = netdev_priv(dev);
		mss = skb_shinfo(skb)->gso_size;

		/* Alpine v2 can't TSO packets with MSS < 16, so we disable support for such SKBs
		 * (since Alpine v3 has the same issue for MSS < 32, we future-proof and even harden
		 *  the check to 64 to protect against any other possible underlying HW issues)
		 */
		if (mss < AL_ETH_TSO_MIN_MSS) {
			features &= ~NETIF_F_GSO_MASK;

			tx_ring = &adapter->tx_ring[skb_get_queue_mapping(skb)];
			u64_stats_update_begin(&tx_ring->syncp);
			tx_ring->tx_stats.tso_small_mss++;
			u64_stats_update_end(&tx_ring->syncp);

			goto out_tso_disable;
		}

		header_len = skb_transport_offset(skb) + tcp_hdrlen(skb);
		total_payload_len = skb->len - header_len;

		/* HW assumes that the packet requires segmentation
		 * (e.g. requested MSS < TCP payload len).
		 */
		if (mss >= total_payload_len) {
			features &= ~NETIF_F_GSO_MASK;
			goto out_tso_disable;
		}

		/* MIN_IPV6_HEADER(40) + MIN_TCP_HEADER(20) always yields a frame larger than 60,
		 * so the following is relevant only for IPv4
		 */
		if (skb_shinfo(skb)->gso_type & SKB_GSO_TCPV4) {
			/* Due to HW issue, we want to make sure that the residual packet of the
			 * HW segmentation yields a packet which is at least 60B
			 */
			if (unlikely(!al_mod_eth_mss_segment_pad_ok(mss,
								total_payload_len,
								header_len))) {
				features &= ~NETIF_F_GSO_MASK;

				tx_ring = &adapter->tx_ring[skb_get_queue_mapping(skb)];
				u64_stats_update_begin(&tx_ring->syncp);
				tx_ring->tx_stats.tso_mss_seg_pad_bad++;
				u64_stats_update_end(&tx_ring->syncp);

				goto out_tso_disable;
			}
		}
	}

out_tso_disable:
	return features;
}
#endif

#ifdef HAVE_NDO_FEATURES_CHECK
static netdev_features_t al_mod_features_check(struct sk_buff *skb,
					   struct net_device *dev,
					   netdev_features_t features)
{
	if (skb_is_gso(skb))
		features = al_mod_eth_skb_tso_features_check(skb, dev, features);

	return features;
}
#elif defined(HAVE_NDO_GSO_CHECK)
static netdev_features_t al_mod_gso_check(struct sk_buff *skb,
					   struct net_device *dev,
					   netdev_features_t features)
{
	if (skb_is_gso(skb))
		features = al_mod_eth_skb_tso_features_check(skb, dev, features);

	return features;
}
#endif

/** HAVE_NET_DEVICE_OPS is defined above KERNEL_VERSION(2,6,29) */
/** CONFIG_ARCH_ALPINE assumes that we are on a kernel above that! */
#ifdef HAVE_NET_DEVICE_OPS
static const struct net_device_ops al_mod_eth_netdev_ops = {
	.ndo_open		= al_mod_eth_open,
	.ndo_stop		= al_mod_eth_close,
	.ndo_start_xmit		= al_mod_eth_start_xmit,
	.ndo_select_queue	= al_mod_eth_select_queue,
#ifdef HAVE_NDO_GET_STATS64
	.ndo_get_stats64	= al_mod_eth_get_stats64,
#else
	.ndo_get_stats		= al_mod_eth_get_stats,
#endif
	.ndo_do_ioctl		= al_mod_eth_ioctl,
	.ndo_tx_timeout		= al_mod_eth_tx_timeout,
	.ndo_change_mtu		= al_mod_eth_change_mtu,
	.ndo_set_mac_address	= al_mod_eth_set_mac_addr,
#ifdef HAVE_SET_RX_MODE
	.ndo_set_rx_mode	= al_mod_eth_set_rx_mode,
#endif
#if 0
	.ndo_validate_addr	= eth_validate_addr,

#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller	= al_mod_eth_netpoll,
#endif
#endif
#ifdef CONFIG_RFS_ACCEL
	.ndo_rx_flow_steer      = al_mod_eth_flow_steer,
#endif
	.ndo_set_features       = al_mod_set_features,
#ifdef HAVE_NDO_FEATURES_CHECK
	.ndo_features_check	= al_mod_features_check,
#elif defined(HAVE_NDO_GSO_CHECK)
	.ndo_gso_check		= al_mod_gso_check,
#endif
};
#endif /* HAVE_NET_DEVICE_OPS */

static inline u8 al_mod_eth_rev_id_to_chip_id(u8 rev_id)
{
	switch (rev_id) {
	case AL_ETH_REV_ID_1:
		return PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V1;
	case AL_ETH_REV_ID_2:
	case AL_ETH_REV_ID_3:
		return PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V2;
	case AL_ETH_REV_ID_4:
		return PBS_UNIT_CHIP_ID_DEV_ID_ALPINE_V3;
	default:
		return 0xff;
	}
}

/**
 * al_mod_eth_probe - Device Initialization Routine
 * @pdev: PCI device information struct
 * @ent: entry in al_mod_eth_pci_tbl
 *
 * Returns 0 on success, negative on failure
 *
 * al_mod_eth_probe initializes an adapter identified by a pci_dev structure.
 * The OS initialization, configuring of the adapter private structure,
 * and a hardware reset occur.
 **/
static int
al_mod_eth_probe(struct pci_dev *pdev, const struct pci_device_id *ent)
{
	static int version_printed;
	struct net_device *netdev;
	struct al_mod_eth_adapter *adapter;
	void __iomem * const *iomap;
	struct al_mod_hal_eth_adapter *hal_adapter;
	u16 dev_id;
	u8 rev_id, orig_rev_id;
	int i;
	unsigned int num_queues; /** Per direction (TX/RX) */
	int rc;
	struct al_mod_eth_adapter_handle_init_params hal_adapter_params = {0};
	int al_mod_chip_id = 0;
	void __iomem *nic_internal_pcie_addr = NULL;
	int bar = 0;
	unsigned int port_index = 0;
	u8 pf_num = 0;
	struct al_mod_eth_common_handle_init_params hal_eth_common_handle_params = {0};

	dev_dbg(&pdev->dev, "%s\n", __func__);

	if (version_printed++ == 0)
		dev_info(&pdev->dev, "%s\n", version);

	rc = pcim_enable_device(pdev);
	if (rc) {
		dev_err(&pdev->dev, "pcim_enable_device failed!\n");
		return rc;
	}
	dev_info(&pdev->dev, "driver_data is 0x%lx", ent->driver_data);

	if (ent->driver_data == ALPINE_INTEGRATED)
		rc = pcim_iomap_regions(pdev, (1 << 0) | (1 << 2) | (1 << 4), DRV_MODULE_NAME);
	else {
		dev_dbg(&pdev->dev, "external pcie rev_id is 0x%x",
			pdev->revision);
#if defined(CONFIG_AL_ETH_SRIOV) && !defined(CONFIG_ARCH_ALPINE)
		if (!pdev->is_physfn) {
			bar = 0;
		} else {
			bar = (pdev->revision >= PCI_REV_ID_64B_BAR) ?
				board_info[ent->driver_data].bar_64 :
				board_info[ent->driver_data].bar;
		}
		dev_dbg(&pdev->dev, "bar is %d phys dev is %d\n", bar, pdev->is_physfn);
#else
		bar = (pdev->revision >= PCI_REV_ID_64B_BAR) ?
			board_info[ent->driver_data].bar_64 :
			board_info[ent->driver_data].bar;
		dev_dbg(&pdev->dev, "bar is %d\n", bar);
#endif
		rc = pcim_iomap_regions(pdev, (1 << bar), DRV_MODULE_NAME);
	}
	if (rc) {
		dev_err(&pdev->dev, "pcim_iomap_regions failed %d\n", rc);
		return rc;
	}

	iomap = pcim_iomap_table(pdev);
	if (!iomap) {
		dev_err(&pdev->dev, "pcim_iomap_table failed\n");
		return -ENOMEM;
	}

	rc = pci_set_dma_mask(pdev, DMA_BIT_MASK(40));
	if (rc) {
		dev_err(&pdev->dev, "pci_set_dma_mask failed 0x%x\n", rc);
		return rc;
	}

	rc = pci_set_consistent_dma_mask(pdev, DMA_BIT_MASK(40));
	if (rc) {
		dev_err(&pdev->dev,
			"err_pci_set_consistent_dma_mask failed 0x%x\n", rc);
		return rc;
	}

	pci_set_master(pdev);
	pci_save_state(pdev);

	if (ent->driver_data == ALPINE_INTEGRATED) {
		pci_read_config_byte(pdev, PCI_REVISION_ID, &rev_id);
		pci_read_config_word(pdev, PCI_DEVICE_ID, &dev_id);

		port_index = PCI_SLOT(pdev->devfn);
	} else {
#ifndef CONFIG_ARCH_ALPINE
		void __iomem *pbs_regs = iomap[bar] + AL_ETH_PBS_REGFILE_OFFSET_SB;

		/** Read alpine chip ID */
		al_mod_chip_id = al_mod_pbs_dev_id_get(pbs_regs);
		dev_info(&pdev->dev, "alpine chip id %d\n", al_mod_chip_id);

		switch (ent->driver_data) {
		case ALPINE_NIC_V2_25_DUAL:
			if (PCI_FUNC(pdev->devfn) == 1)
				port_index = 2;
			else
				port_index = 0;
			break;
		case ALPINE_NIC_V3_4x25:
		case ALPINE_NIC_V3_4x25_EX:
			pf_num = PCI_FUNC(pdev->devfn);
			port_index = eth_v4_adv_port_map[pf_num];
			break;
		case ALPINE_NIC_V2_1G_TEST:
		case ALPINE_NIC_V3_1G_TEST:
			pf_num = PCI_FUNC(pdev->devfn);
			port_index = 1;
			dev_info(&pdev->dev, "1G test support, eth port %d\n",
				 port_index);
			break;
		default:
			port_index = 0;
			break;
		}

		dev_info(&pdev->dev, "eth port %d pf num %d\n", port_index, pf_num);
		nic_internal_pcie_addr = iomap[bar] +
			al_mod_iomap_offset_idx_sub_idx_get(
				al_mod_chip_id, AL_IOMAP_ETH_OFFSET, port_index, 0);
#endif
		rev_id = readb(nic_internal_pcie_addr + PCI_REVISION_ID);
		dev_id = readw(nic_internal_pcie_addr + PCI_DEVICE_ID);

	}

	/*
	 * Treat ETHv4 STD as REV_2
	 * TODO: This is sort of a hack, and should be removed once all paths
	 * in the HAL for STD v4 are fixed (including
	 * AL_ETH_UDMA_HW_QUEUES_BY_REV() macro)
	 */
	orig_rev_id = rev_id;
	if ((rev_id == AL_ETH_REV_ID_4) && (dev_id == AL_ETH_DEV_ID_STANDARD))
		rev_id = AL_ETH_REV_ID_2;

	dev_info(&pdev->dev, "eth rev_id %d (orig_rev_id %d) dev_id %d\n",
		 rev_id,
		 orig_rev_id,
		 dev_id);

	num_queues = AL_ETH_UDMA_HW_QUEUES_BY_REV(rev_id);

	/* dev zeroed in init_etherdev */
	netdev = alloc_etherdev_mq(sizeof(struct al_mod_eth_adapter), num_queues);
	if (!netdev) {
		dev_err(&pdev->dev, "alloc_etherdev_mq failed\n");
		return -ENOMEM;
	}

	SET_NETDEV_DEV(netdev, &pdev->dev);

	adapter = netdev_priv(netdev);
	pci_set_drvdata(pdev, adapter);

	adapter->up = false;
	adapter->netdev = netdev;
	adapter->pdev = pdev;
	adapter->board_type = ent->driver_data;
	hal_adapter = &adapter->hal_adapter;
	adapter->msg_enable = netif_msg_init(debug, DEFAULT_MSG_ENABLE);
	adapter->rev_id = rev_id;
	adapter->orig_rev_id = orig_rev_id;
	adapter->dev_id = dev_id;
	adapter->port_num = port_index;

	if (adapter->board_type == ALPINE_INTEGRATED) {
		/** For integrated networking always use udma 0 in host & kernel driver */
		adapter->udma_num = 0;
		adapter->udma_base = iomap[AL_ETH_UDMA_BAR];
		adapter->ec_base = iomap[AL_ETH_EC_BAR];
		adapter->mac_base = iomap[AL_ETH_MAC_BAR];
		adapter->pbs_regfile_base = pbs_regs_base;
		adapter->al_mod_chip_id = pbs_regs_base ?
			(u8)al_mod_pbs_dev_id_get(pbs_regs_base) :
			al_mod_eth_rev_id_to_chip_id(orig_rev_id);
		if (adapter->al_mod_chip_id == 0xff) {
			dev_err(&pdev->dev, "Couldn't get valid chip id\n");
			rc = -EINVAL;
			goto err_hw_init;
		}
	} else {
		u16 adapter_pci_cmd;

#ifdef AL_ETH_PLAT_EMU
		adapter->udma_num = AL_ETH_DEFAULT_UDMA;
#else
		/*
		 * V2 advanced units may use udma_num supplied by module param.
		 * All other units use udma number 0
		 */
		adapter->udma_num =
			(IS_V2_NIC(adapter->board_type) &&
			 (adapter->dev_id == AL_ETH_DEV_ID_ADVANCED)) ?
			udma_num : 0;
#endif
		dev_info(&pdev->dev, "udma_num %d\n", adapter->udma_num);

		/*
		 * pci adapter configuration space: 0-4K
		 * BAR0-ETH_CTL: 20K-36K (start offset 0x5000)
		 * BAR1-MAC_CTL: 36K-40K (start offset 0x9000)
		 * BAR2-UDMA: 128K-256K
		 */
		adapter->al_mod_chip_id = al_mod_chip_id;
		adapter->pcie_ep_num = IS_V3_NIC(adapter->board_type) ? 2 : 0;
		adapter->pf_num = pf_num;
		adapter->internal_sb_base = iomap[bar];
		adapter->internal_pcie_base = nic_internal_pcie_addr;
		adapter->unit_adapter_base = nic_internal_pcie_addr;
#ifndef CONFIG_ARCH_ALPINE
		adapter->udma_base = adapter->internal_pcie_base +
			al_mod_iomap_offset_idx_sub_idx_get(
				al_mod_chip_id, AL_IOMAP_ETH_UDMA_OFFSET, port_index, adapter->udma_num);
		adapter->ec_base = adapter->internal_pcie_base +
			al_mod_iomap_offset_idx_sub_idx_get(
				al_mod_chip_id, AL_IOMAP_ETH_EC_OFFSET, port_index, 0);

		adapter->serdes_base = iomap[bar] +
			al_mod_iomap_offset_idx_sub_idx_get(
				al_mod_chip_id, AL_IOMAP_SB_SERDES_OFFSET, port_index, 0);

		adapter->mac_base = adapter->internal_pcie_base +
			al_mod_iomap_offset_idx_sub_idx_get(
				al_mod_chip_id, AL_IOMAP_ETH_MAC_OFFSET, port_index, 0);

		adapter->pbs_regfile_base = adapter->internal_sb_base +
			al_mod_iomap_offset_idx_sub_idx_get(adapter->al_mod_chip_id,
							AL_IOMAP_SB_PBS_OFFSET,
							0, 0) +
			al_mod_iomap_offset_idx_sub_idx_get(adapter->al_mod_chip_id,
							AL_IOMAP_PBS_REGFILE_OFFSET,
							0, 0);
		adapter->eth_common_base = adapter->internal_sb_base +
			al_mod_iomap_offset_idx_sub_idx_get(al_mod_chip_id,
							AL_IOMAP_ETH_COMMON_OFFSET,
							0,
							0);
		adapter->ec_0_base = adapter->internal_sb_base +
			al_mod_iomap_offset_idx_sub_idx_get(al_mod_chip_id,
							AL_IOMAP_ETH_EC_OFFSET,
							0,
							0);
#endif
		dev_info(&pdev->dev,
			 "Mapped addresses - port base %p udma %p ec %p mac %p serdes %p eth_common %p ec_0_base %p\n",
			 adapter->internal_pcie_base,
			 adapter->udma_base,
			 adapter->ec_base,
			 adapter->mac_base,
			 adapter->serdes_base,
			 adapter->eth_common_base,
			 adapter->ec_0_base);

		/* enable sriov*/
		if (adapter->udma_num > 0) {
			/* enable all 3 VF's */
			writel(3, adapter->internal_pcie_base + 0x300 + PCI_SRIOV_NUM_VF);
			writel(PCI_SRIOV_CTRL_VFE | PCI_SRIOV_CTRL_MSE,
				adapter->internal_pcie_base + 0x300 + PCI_SRIOV_CTRL);
			/* enable master/slave in the adapter conf */
			adapter_pci_cmd =
				readw(adapter->internal_pcie_base + 0x1000 * (adapter->udma_num) +\
				PCI_COMMAND);
			adapter_pci_cmd |= PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER;
			writew(adapter_pci_cmd,
				adapter->internal_pcie_base + 0x1000 * (adapter->udma_num) +\
				PCI_COMMAND);
		} else {
			/* enable master/slave in the adapter conf */
			adapter_pci_cmd = readw(adapter->internal_pcie_base + PCI_COMMAND);
			adapter_pci_cmd |= PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER;

			/*
			 * RMN 2871: MSI-X & Legacy interrupts
			 *
			 * When the Interrupt controller in ETH UDMA is
			 * configured to MSI-X interrupts for groups A, B and C
			 * while group D sends its summary to group A's bit2,
			 * the Interrupt Disable bit in the PCIe command reg
			 * gets set. This masks the interrupt summary of groupD,
			 * causing bit2 of group A never to be set.
			 * As a WA, we clear the Interrupt Disable bit in the
			 * PCIe command reg.
			 *
			 * The RMN above also states that:
			 * The MSI-X interrupt enable bit still masks the Legacy
			 * interrupts from the UDMA towards the GIC. Therefore,
			 * in practice legacy interrupts & MSI-X interrupts
			 * don't work together (although theoretically was
			 * supposed to be supported).
			 */
			adapter_pci_cmd &= ~PCI_COMMAND_INTX_DISABLE;
			writew(adapter_pci_cmd, adapter->internal_pcie_base + PCI_COMMAND);
		}
	}

	adapter->default_rx_udma_num = adapter->udma_num;
	/* set default ring sizes */
	adapter->tx_ring_count = AL_ETH_DEFAULT_TX_SW_DESCS;
	adapter->tx_descs_count = AL_ETH_DEFAULT_TX_HW_DESCS;
	adapter->rx_ring_count = AL_ETH_DEFAULT_RX_DESCS;
	adapter->rx_descs_count = AL_ETH_DEFAULT_RX_DESCS;

	adapter->num_tx_queues = num_queues;
	adapter->num_rx_queues = num_queues;

	adapter->small_copy_len = AL_ETH_DEFAULT_SMALL_PACKET_LEN;
	adapter->link_poll_interval = AL_ETH_DEFAULT_LINK_POLL_INTERVAL;
	adapter->max_rx_buff_alloc_size = AL_ETH_DEFAULT_MAX_RX_BUFF_ALLOC_SIZE;
	adapter->link_config.force_1000_base_x = AL_ETH_DEFAULT_FORCE_1000_BASEX;

	adapter->queue_steer_id = -1;

	if (pcie_int_lock_inited == 0) {
		spin_lock_init(&pcie_int_lock);
		pcie_int_lock_inited = 1;
	}
	snprintf(adapter->name, AL_ETH_NAME_MAX_LEN, "al_mod_eth_%d", adapter->port_num);

#ifndef CONFIG_ARCH_ALPINE
	/** Init serdes handle if board_params need to determine speed on the fly */
	switch (adapter->board_type) {
	case ALPINE_NIC:
	case ALPINE_NIC_V2_10:
		adapter->serdes_obj = kzalloc(sizeof(struct al_mod_serdes_grp_obj), GFP_KERNEL);
		al_mod_serdes_hssp_handle_init(adapter->serdes_base, adapter->serdes_obj);
		break;
	case ALPINE_NIC_V2_25:
	case ALPINE_NIC_V2_25_DUAL:
		adapter->serdes_obj = kzalloc(sizeof(struct al_mod_serdes_grp_obj), GFP_KERNEL);
		al_mod_serdes_25g_handle_init(adapter->serdes_base + AL_ETH_SERDES_25G_OFFSET,
			adapter->serdes_obj);
		break;
	default:
		break;
	}
#endif

#ifndef CONFIG_ARCH_ALPINE
	/** Disable interrupts - restore when opening interface */
	al_mod_eth_modify_pcie_ep_int(adapter, 0);
#endif

	if (IS_V3_NIC(adapter->board_type) &&
	    IS_ETH_V4_ADV(adapter->rev_id, adapter->dev_id)) {
		hal_eth_common_handle_params.eth_common_regs_base =
			adapter->eth_common_base;
		hal_eth_common_handle_params.eth_ec_0_regs_base =
			adapter->ec_0_base;
		al_mod_eth_common_handle_init(&adapter->hal_eth_common_handle,
					  &hal_eth_common_handle_params);
	}

	/**
	 * Init eth adapter handle for board params get ex
	 * This will be done in full later when setting up the HW, so for now
	 * we will configure only whats required
	 */
	hal_adapter_params.name = adapter->name;
	hal_adapter_params.rev_id = orig_rev_id;
	hal_adapter_params.dev_id = adapter->dev_id;
	hal_adapter_params.udma_id = adapter->udma_num;
	hal_adapter_params.udma_regs_base = adapter->udma_base;
	hal_adapter_params.ec_regs_base = adapter->ec_base;
	hal_adapter_params.mac_regs_base = adapter->mac_base;
	hal_adapter_params.eth_common_regs_base = adapter->eth_common_base;
	al_mod_eth_adapter_handle_init(&adapter->hal_adapter, &hal_adapter_params);
	rc = al_mod_eth_board_params_init(adapter);
	if (rc)
		goto err_hw_init;
	if (adapter->rev_id >= AL_ETH_REV_ID_4) {
		/** update HAL adapter with common mode as read from board params */
		hal_adapter_params.common_mode = adapter->common_mode;
		al_mod_eth_adapter_handle_init(&adapter->hal_adapter, &hal_adapter_params);
	}

#ifndef CONFIG_ARCH_ALPINE
	if (IS_V3_NIC(adapter->board_type) &&
	    IS_ETH_V4_ADV(adapter->rev_id, adapter->dev_id)) {
		rc = al_mod_eth_reset_workaround_handling(adapter, true);
		if (rc)
			goto err_hw_init;
	}
#endif /* CONFIG_ARCH_ALPINE */

	adapter->user_def_flow_steering_restored = AL_TRUE;
	adapter->user_def_rx_icrc_restored = AL_TRUE;

#ifdef CONFIG_ARCH_ALPINE
	/*
	 * adapter->live_update_restore_state is a flag which indicates whether
	 * we are still before the restore (which occurs only during the first
	 * open after probe) and is set to false once the restore is done.
	 * Currently supported for eth rev id 3 only.
	 */
	if (adapter->use_lm && adapter->rev_id == AL_ETH_REV_ID_3) {
		if (live_update_restore_internal ==
		    AL_ETH_LIVE_UPDATE_RESTORE_USE_PRE_DEFINED) {
			adapter->live_update_restore_state = true;
		}

		if (g_rsrvd_mem) {
			adapter->rsrvd_mem = g_rsrvd_mem +
				AL_ETH_RSRVD_MEM_SECTION_TYPE_PORT_OFFSET(port_index);

			if (live_update_restore_internal ==
			    AL_ETH_LIVE_UPDATE_RESTORE_ENABLE) {
				struct al_mod_eth_rsrvd_mem_section_hdr *hdr;

				hdr = adapter->rsrvd_mem;
				if (hdr->hdr.magic == AL_ETH_RSRVD_MEM_SECTION_HDR_MAGIC) {
					adapter->live_update_restore_state = true;
				} else {
					adapter->live_update_restore_state = false;
					dev_warn(&pdev->dev, "magic validation failed - actual: 0x%x, expected:0x%x\n",
						 hdr->hdr.magic,
						 AL_ETH_RSRVD_MEM_SECTION_HDR_MAGIC);
				}
			}
		}
	}

	dev_dbg(&pdev->dev, "%s: live_update_restore:%d, live_update_restore_internal:%d, adapter->live_update_restore_state:%d\n",
		__func__, live_update_restore,
		live_update_restore_internal,
		adapter->live_update_restore_state);

	dev_dbg(&pdev->dev, "%s: adapter->rsrvd_mem: %px\n",
		__func__, adapter->rsrvd_mem);
#endif /* CONFIG_ARCH_ALPINE */

	/*
	 * adapter->lm_context.lock must be initialized only once.
	 */
	mutex_init(&adapter->lm_context.lock);

	/*
	 * During live update do not reset the whole unit, just the EC and UDMA
	 */
	if (adapter->live_update_restore_state) {
		al_mod_assert(IS_ETH_V3_ADV(adapter->rev_id, adapter->dev_id));
		adapter->flags |= AL_ETH_FLAG_RESET_UDMA_EC;
	}
	al_mod_eth_function_reset(adapter);
	adapter->flags &= ~AL_ETH_FLAG_RESET_UDMA_EC;

	/** Perform HW stop to clean garbage left-overs from PXE / Uboot driver */
#ifndef AL_ETH_PLAT_EMU
	rc = al_mod_eth_init_rings_handle(adapter);
	if (rc) {
		dev_err(&pdev->dev, "Failed at al_mod_eth_hal_adapter_init (rc = %d)\n", rc);
		goto err_hw_init;
	}
	al_mod_eth_mac_mode_set(&adapter->hal_adapter, adapter->mac_mode);
	al_mod_eth_hw_stop(adapter);
#endif

#if defined(CONFIG_ARCH_ALPINE)
	if (IS_ETH_V4_ADV(adapter->rev_id, adapter->dev_id)) {
		adapter_handles[adapter->port_num] = &adapter->hal_adapter;
		if (adapter->port_num == 0) {
			eth_v4_lm_port_0_status.mode_detection_ongoing = true;
		}
	} else {
		adapter->group_lm_context = alpine_group_lm_get(adapter->serdes_grp);
	}
#endif

	rc = al_mod_eth_hw_init_adapter(adapter);
	if (rc)
		goto err_hw_init;

	al_mod_eth_init_rings(adapter);
	INIT_WORK(&adapter->reset_task, al_mod_eth_reset_task);

#ifdef HAVE_NET_DEVICE_OPS
	netdev->netdev_ops = &al_mod_eth_netdev_ops;
#else
	netdev->open = &al_mod_eth_open;
	netdev->stop = &al_mod_eth_close;
	netdev->hard_start_xmit = &al_mod_eth_start_xmit;
	netdev->get_stats = &al_mod_eth_get_stats;
#ifdef HAVE_SET_RX_MODE
	netdev->set_rx_mode = &al_mod_eth_set_rx_mode;
#endif
	netdev->do_ioctl = &al_mod_eth_ioctl;
	netdev->set_mac_address = &al_mod_eth_set_mac_addr;
	netdev->change_mtu = &al_mod_eth_change_mtu;
	netdev->tx_timeout = &al_mod_eth_tx_timeout;
#ifdef HAVE_NETDEV_SELECT_QUEUE
	netdev->select_queue = &al_mod_eth_select_queue;
#endif
#endif /* HAVE_NET_DEVICE_OPS */

#ifndef AL_ETH_PLAT_EMU
	netdev->watchdog_timeo = TX_TIMEOUT;
#else
	netdev->watchdog_timeo = TX_TIMEOUT*10;
#endif
	netdev->ethtool_ops = &al_mod_eth_ethtool_ops;

	if (!is_valid_ether_addr(adapter->mac_addr)) {
		eth_hw_addr_random(netdev);
		memcpy(adapter->mac_addr, netdev->dev_addr, ETH_ALEN);
	} else {
		memcpy(netdev->dev_addr, adapter->mac_addr, ETH_ALEN);
	}

	memcpy(adapter->netdev->perm_addr, adapter->mac_addr, netdev->addr_len);

	netdev->features =	NETIF_F_SG |
				NETIF_F_IP_CSUM |
#if defined(NETIF_F_IPV6_CSUM) || defined(CONFIG_ARCH_ALPINE)
				NETIF_F_IPV6_CSUM |
#endif
#if defined(HAVE_NDO_FEATURES_CHECK) || defined(HAVE_NDO_GSO_CHECK)
				NETIF_F_TSO |
				NETIF_F_TSO_ECN |
				NETIF_F_TSO6 |
#endif
				NETIF_F_RXCSUM |
				NETIF_F_NTUPLE |
#if defined(NETIF_F_RXHASH) || defined(CONFIG_ARCH_ALPINE)
				NETIF_F_RXHASH |
#endif
#ifdef CONFIG_NET_MQ_TX_LOCK_OPT
				NETIF_F_MQ_TX_LOCK_OPT |
#endif
				NETIF_F_HIGHDMA;

/** HAVE_NDO_SET_FEATURES defined for kernels above KERNEL_VERSION(2,6,39) */
#ifdef HAVE_NDO_SET_FEATURES
	netdev->hw_features |= netdev->features;
#endif

	netdev->vlan_features |= netdev->features;

/* set this after vlan_features since it cannot be part of vlan_features */
#if defined(CONFIG_AL_ETH_ALLOC_PAGE) || defined(CONFIG_AL_ETH_ALLOC_FRAG)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0))
	netdev->features |= NETIF_F_HW_VLAN_CTAG_RX;
#else
	netdev->features |=  NETIF_F_HW_VLAN_RX;
#endif
#endif

#if defined(NETIF_F_MQ_TX_LOCK_OPT)
	netdev->features &= ~NETIF_F_MQ_TX_LOCK_OPT;
#endif
#if defined(IFF_UNICAST_FLT) || defined(CONFIG_ARCH_ALPINE)
	if (!(IS_ETH_V4_ADV(adapter->rev_id, adapter->dev_id)))
		netdev->priv_flags |= IFF_UNICAST_FLT;
#endif

	adapter->rss_ind_tbl_size = (adapter->rev_id >= AL_ETH_REV_ID_4) ?
		AL_ETH_V4_ADV_RX_FWD_RSS_TABLE_SIZE : AL_ETH_V3_ADV_RX_FWD_RSS_TABLE_SIZE;

	for (i = 0; i < adapter->rss_ind_tbl_size; i++)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 8, 0)
		adapter->rss_ind_tbl[i] =
			ethtool_rxfh_indir_default(i, adapter->num_rx_queues);
#else
		adapter->rss_ind_tbl[i] = i % adapter->num_rx_queues;
#endif
	u64_stats_init(&adapter->syncp);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 10, 0)
	netdev->min_mtu = AL_ETH_MIN_MTU;
	netdev->max_mtu = AL_ETH_MAX_MTU;
#endif

	adapter->adaptive_intr_rate = adaptive_int_moderation;
	al_mod_eth_init_intr_default_moderation_table_intervals(adapter);
	/* assign initial_moderation_table the accurate interval values,according to SB frequency */
	al_mod_eth_initial_moderation_table_restore_default();

	if (AL_ETH_ERR_EVENTS_SUPPORTED(adapter)) {
		spin_lock_init(&adapter->stats_lock);

		INIT_DELAYED_WORK(&adapter->err_events_task,
				  al_mod_eth_err_events_task);

#ifdef CONFIG_ARCH_ALPINE
		if (!IS_NIC(adapter->board_type))
			al_mod_eth_parse_dev_node(adapter);
#endif

		rc = al_mod_eth_err_events_init(adapter);
		if (rc) {
			netdev_err(adapter->netdev,
				   "failed to initialize err_events\n");
			goto err_events;
		}

		/* NOTE: must be set before calling al_mod_eth_sysfs_init */
		adapter->err_events_enabled = true;
	}

	rc = register_netdev(netdev);
	if (rc) {
		dev_err(&pdev->dev, "Cannot register net device\n");
		goto err_register;
	}

	rc = al_mod_eth_sysfs_init(&adapter->pdev->dev);
	if (rc)
		goto err_sysfs;

	netdev_info(netdev, "%s found at mem %lx, mac addr %pM\n",
		board_info[ent->driver_data].name,
		(long)pci_resource_start(pdev, 0), netdev->dev_addr);

#if defined(CONFIG_AL_ETH_SRIOV) && !defined(CONFIG_ARCH_ALPINE)
	if (pdev->is_physfn) {
		int err = 0;
		dev_info(&pdev->dev, "disable sriov");
		pci_disable_sriov(pdev);

		dev_info(&pdev->dev, "enable sriov, vfs: %d", 1);
		err = pci_enable_sriov(pdev, 1);
		if (err)
			dev_info(&pdev->dev, "pci_enable_sriov() failed. err=%d\n", err);
	}
#endif

	/* Init MAC MDIO - needed by al_mod_eth_mdiobus_setup */
	rc = al_mod_eth_mdiobus_hw_init(adapter);
	if(rc) {
		netdev_err(adapter->netdev, "%s: al_mod_eth_mdiobus_hw_init failed\n", __func__);
		goto err_sysfs;
	}

	rc = al_mod_eth_mdiobus_setup(adapter);
	if (rc) {
		netdev_err(netdev, "failed at mdiobus setup!\n");
		goto err_sysfs;
	}

	return 0;

err_sysfs:
	unregister_netdev(netdev);
err_register:
err_events:
#ifdef CONFIG_ARCH_ALPINE
	if (adapter->ua_irq.vector)
		irq_dispose_mapping(adapter->ua_irq.vector);

	if (adapter->unit_adapter_base)
		iounmap(adapter->unit_adapter_base);
#endif
err_hw_init:
	free_netdev(netdev);
	return rc;
}

/**
 * al_mod_eth_remove - Device Removal Routine
 * @pdev: PCI device information struct
 *
 * al_mod_eth_remove is called by the PCI subsystem to alert the driver
 * that it should release a PCI device.
 **/
static void
al_mod_eth_remove(struct pci_dev *pdev)
{
	struct al_mod_eth_adapter *adapter = pci_get_drvdata(pdev);
	struct net_device *dev = adapter->netdev;

#ifdef CONFIG_ARCH_ALPINE
	/*
	 * adapter->lm_context.lock was initialized in al_mod_eth_probe
	 */
	mutex_destroy(&adapter->lm_context.lock);
#endif

#ifdef CONFIG_PHYLIB
	/** Stop PHY */
	if (adapter->phydev) {
		/* Disconnect the PHY */
		phy_disconnect(adapter->phydev);
	}
#endif
	al_mod_eth_mdiobus_teardown(adapter);

	unregister_netdev(dev);

#ifndef CONFIG_ARCH_ALPINE
	if (IS_V3_NIC(adapter->board_type) &&
	    IS_ETH_V4_ADV(adapter->rev_id, adapter->dev_id))
		al_mod_eth_reset_workaround_handling(adapter, false);
#endif /* CONFIG_ARCH_ALPINE */

#ifdef CONFIG_ARCH_ALPINE
	if (adapter->ua_irq.vector)
		irq_dispose_mapping(adapter->ua_irq.vector);

	if (adapter->unit_adapter_base)
		iounmap(adapter->unit_adapter_base);
#endif

	al_mod_eth_sysfs_terminate(&pdev->dev);
#ifndef CONFIG_ARCH_ALPINE
	if (IS_NIC(adapter->board_type))
		kzfree(adapter->serdes_obj);
#endif
	free_netdev(dev);

#if defined(CONFIG_AL_ETH_SRIOV) && !defined(CONFIG_ARCH_ALPINE)
	if (pdev->is_physfn) {
		dev_info(&pdev->dev, "disable sriov");
		pci_disable_sriov(pdev);
	}
#endif

	pci_set_drvdata(pdev, NULL);
	pci_disable_device(pdev);
}

#ifdef CONFIG_PM
static int al_mod_eth_resume(struct pci_dev *pdev)
{
	struct al_mod_eth_adapter *adapter = pci_get_drvdata(pdev);
	struct net_device *netdev = adapter->netdev;
	u32 err;

	pci_set_power_state(pdev, PCI_D0);
	pci_restore_state(pdev);
	/*
	 * pci_restore_state clears dev->state_saved so call
	 * pci_save_state to restore it.
	 */
	pci_save_state(pdev);

	err = pci_enable_device_mem(pdev);
	if (err) {
		pr_err("Cannot enable PCI device from suspend\n");
		return err;
	}
	pci_set_master(pdev);

	pci_wake_from_d3(pdev, false);

#if defined(CONFIG_AL_ETH_SRIOV) && !defined(CONFIG_ARCH_ALPINE)
	if (pdev->is_physfn) {
	   int err = 0;
	   dev_info(&pdev->dev, "disable sriov");
	   pci_disable_sriov(pdev);

	   dev_info(&pdev->dev, "enable sriov, vfs: %d", 1);
	   err = pci_enable_sriov(pdev, 1);
	   if (err) {
		   dev_info(&pdev->dev, "pci_enable_sriov() failed. err=%d\n", err);
	   }
	}
#endif

	al_mod_eth_wol_disable(&adapter->hal_adapter);

	netif_device_attach(netdev);

	return 0;
}

static int al_mod_eth_wol_config(struct al_mod_eth_adapter *adapter)
{
	struct al_mod_eth_wol_params wol = {0};

	if (adapter->wol & WAKE_UCAST) {
		wol.int_mask = AL_ETH_WOL_INT_UNICAST;
		wol.forward_mask = AL_ETH_WOL_FWRD_UNICAST;
	}

	if (adapter->wol & WAKE_MCAST) {
		wol.int_mask = AL_ETH_WOL_INT_MULTICAST;
		wol.forward_mask = AL_ETH_WOL_FWRD_MULTICAST;
	}

	if (adapter->wol & WAKE_BCAST) {
		wol.int_mask = AL_ETH_WOL_INT_BROADCAST;
		wol.forward_mask = AL_ETH_WOL_FWRD_BROADCAST;
	}

	if (wol.int_mask != 0) {
		al_mod_eth_wol_enable(&adapter->hal_adapter, &wol);
		return 1;
	}

	return 0;
}

static int al_mod_eth_suspend(struct pci_dev *pdev, pm_message_t state)
{
	struct al_mod_eth_adapter *adapter = pci_get_drvdata(pdev);

	if (al_mod_eth_wol_config(adapter)) {
		pci_prepare_to_sleep(pdev);
	} else {
		pci_wake_from_d3(pdev, false);
		pci_set_power_state(pdev, PCI_D3hot);
	}

	return 0;
}
#endif /* CONFIG_PM */

int al_mod_eth_udma_steer_override(struct al_mod_eth_adapter *adapter, uint8_t udma_mask)
{
	struct al_mod_eth_fwd_ctrl_table_entry entry;
	bool use_table = AL_FALSE;

	if (adapter->rev_id >= AL_ETH_REV_ID_4) {
		netdev_err(adapter->netdev, "%s : Doesnt not support eth v%d adv\n",
			__func__, adapter->rev_id);
		return -EOPNOTSUPP;
	}

	entry.prio_sel = AL_ETH_CTRL_TABLE_PRIO_SEL_VAL_0;
	entry.queue_sel_1 = AL_ETH_CTRL_TABLE_QUEUE_SEL_1_THASH_TABLE;
	entry.queue_sel_2 = AL_ETH_CTRL_TABLE_QUEUE_SEL_2_NO_PRIO;
	entry.udma_sel = AL_ETH_CTRL_TABLE_UDMA_SEL_REG1;
	entry.filter = AL_FALSE;

	if (udma_mask > 0xf) {
		netdev_warn(adapter->netdev, "Invalid UDMA mask given for UDMA steering (0x%x)\n",
			udma_mask);
		return -EINVAL;
	}

	if (udma_mask == 0x0) {
		/* udma mask equal to zero is treated as restoring the default setting
		 * which was configured in al_mod_eth_config_rx_fwd() */
		entry.udma_sel = AL_ETH_CTRL_TABLE_UDMA_SEL_MAC_TABLE;
		/* if queue_steer_override isn't configured then we go back to using
		 * the entire control table */
		if (adapter->queue_steer_id == -1)
			use_table = AL_TRUE;
	} else {
		entry.udma_sel = AL_ETH_CTRL_TABLE_UDMA_SEL_REG1;
		/* setup default udma_sel (reg1) to udma#1 */
		al_mod_eth_fwd_default_udma_config(&adapter->hal_adapter, 0, udma_mask);
	}

	if (adapter->queue_steer_id != -1)
		entry.queue_sel_1 = AL_ETH_CTRL_TABLE_QUEUE_SEL_1_REG1;

	adapter->udma_steer_mask = udma_mask;

	/* set default entry (and not control table) */
	al_mod_eth_ctrl_table_def_set(&adapter->hal_adapter, use_table, &entry);

	if (adapter->udma_steer_mask || (adapter->queue_steer_id != -1))
		netdev_info(adapter->netdev, "RX flow steering override configured with udma_mask=%u, qid=%d\n",
			    adapter->udma_steer_mask,
			    adapter->queue_steer_id);

	return 0;
}

int al_mod_eth_queue_steer_override(struct al_mod_eth_adapter *adapter, int qid)
{
	struct al_mod_eth_fwd_ctrl_table_entry entry;
	bool use_table = AL_FALSE;

	if (adapter->rev_id >= AL_ETH_REV_ID_4) {
		netdev_err(adapter->netdev, "%s : Doesnt not support eth v%d adv\n",
			__func__, adapter->rev_id);
		return -EOPNOTSUPP;
	}

	entry.prio_sel = AL_ETH_CTRL_TABLE_PRIO_SEL_VAL_0;
	entry.queue_sel_2 = AL_ETH_CTRL_TABLE_QUEUE_SEL_2_NO_PRIO;
	entry.udma_sel = AL_ETH_CTRL_TABLE_UDMA_SEL_REG1;
	entry.filter = AL_FALSE;

	if ((qid >= AL_ETH_MAX_HW_QUEUES) || (qid < -1)) {
		netdev_warn(adapter->netdev,
			    "The given qid (%d) is out of range. -1=restore default, "
			    "0-%d=valid queue id\n", qid, AL_ETH_MAX_HW_QUEUES-1);
		return -EINVAL;
	}

	if (qid == -1) {
		/* a value of -1 is interpreted as restoring the default configuration (i.e. RSS) */
		entry.queue_sel_1 = AL_ETH_CTRL_TABLE_QUEUE_SEL_1_THASH_TABLE;
		/* if udma_steer_mask isn't configured then we go back to using
		 * the entire control table */
		if (adapter->udma_steer_mask == 0x0)
			use_table = AL_TRUE;
	} else {
		entry.queue_sel_1 = AL_ETH_CTRL_TABLE_QUEUE_SEL_1_REG1;
		al_mod_eth_fwd_default_queue_config(&adapter->hal_adapter, 0, qid);
	}

	entry.udma_sel = adapter->udma_steer_mask ? AL_ETH_CTRL_TABLE_UDMA_SEL_REG1 :
						    AL_ETH_CTRL_TABLE_UDMA_SEL_MAC_TABLE;

	adapter->queue_steer_id = qid;

	/* set default entry (and not control table) */
	al_mod_eth_ctrl_table_def_set(&adapter->hal_adapter, use_table, &entry);

	if (adapter->udma_steer_mask || (adapter->queue_steer_id != -1))
		netdev_info(adapter->netdev, "RX flow steering override configured with udma_mask=%u, qid=%d\n",
			    adapter->udma_steer_mask,
			    adapter->queue_steer_id);

	return 0;
}

static void al_mod_eth_flow_steer_rx_icrc_hw_offload_enable(struct al_mod_eth_adapter *adapter,
							unsigned int entry_idx,
							enum AL_ETH_PROTO_ID protocol_idx,
							uint8_t protocol_idx_mask,
							bool icrc_hw_enable)
{
	/* Default rx_gpd_cam_entry for iCRC. The first field will be overwritten with
	 * protocol_idx */
	struct al_mod_eth_rx_gpd_cam_entry icrc_rx_gpd_cam_entry = {
		0,		13,		0,		0,
		0,		0,		0,		0,		1,
		0x1f,		0x1f,		0x0,		0x0,
		0x4,		0x0,		0x0,		0x0
	};
	struct al_mod_eth_rx_gcp_table_entry icrc_rx_gcp_table_entry = {
		0,		1,		1,		0,		1,
		0,		4,		0,		0,		1,
		0,		0,		0,		0,		0,
		0,		0,		{0x3000cf00,	0x00000000,	0x00000000,
		0x00000000,	0x00000000,	0x00000000},	0xffffffff,	0x03000001,
		0
	};
	struct al_mod_eth_rx_gpd_cam_entry disabled_rx_gpd_cam_entry = {
		0,		0,		0,		0,
		0,		0,		0,		0,		0,
		0x0,		0x0,		0x0,		0x0,
		0x0,		0x0,		0x0,		0x0
	};
	struct al_mod_eth_rx_gcp_table_entry disabled_rx_gcp_table_entry = {
		0,		0,		0,		0,		0,
		0,		0,		0,		0,		0,
		0,		0,		0,		0,		0,
		0,		0,		{0x00000000,	0x00000000,	0x00000000,
		0x00000000,	0x00000000,	0x00000000},	0x00000000,	0x00000000,
		0
	};

	icrc_rx_gpd_cam_entry.outer_l3_proto_idx = protocol_idx;
	icrc_rx_gpd_cam_entry.outer_l3_proto_idx_mask = protocol_idx_mask;

	if (icrc_hw_enable == AL_TRUE) {
		al_mod_eth_rx_generic_crc_table_entry_set(&adapter->hal_adapter,
						      0, entry_idx,
						      &icrc_rx_gcp_table_entry);
		al_mod_eth_rx_protocol_detect_table_entry_set(&adapter->hal_adapter,
							  entry_idx,
							  &icrc_rx_gpd_cam_entry);

		netdev_info(adapter->netdev, "Configured iCRC RX HW offload in entry_idx %u (protocol_idx:0x%x, mask:0x%x)\n",
			    entry_idx, protocol_idx, protocol_idx_mask);
	} else {
		al_mod_eth_rx_protocol_detect_table_entry_set(&adapter->hal_adapter,
							  entry_idx,
							  &disabled_rx_gpd_cam_entry);
		al_mod_eth_rx_generic_crc_table_entry_set(&adapter->hal_adapter,
						      0, entry_idx,
						      &disabled_rx_gcp_table_entry);
		netdev_info(adapter->netdev, "Disabled iCRC RX HW offload in entry_idx %u\n",
			    entry_idx);
	}
}

static int al_mod_eth_rx_steering_action_config(struct al_mod_eth_adapter *adapter,
					     struct al_mod_eth_flow_steer_action *action,
					     unsigned int protocol_idx)
{
	struct al_mod_eth_fwd_ctrl_table_index steering_ctrl_table_index;
	struct al_mod_eth_fwd_ctrl_table_entry steering_ctrl_table_entry;
	unsigned int protocol_idx_offset;

	al_mod_assert((protocol_idx >= (AL_ETH_EPE_ENTRIES_NUM - 1)) &&
		  (protocol_idx < AL_ETH_PROTOCOLS_NUM));

	protocol_idx_offset = protocol_idx - (AL_ETH_EPE_ENTRIES_NUM - 1);
	netdev_dbg(adapter->netdev, "%s: protocol_idx_offset=%d, action->udma_mask=%d, action->queue_num=%d\n",
		   __func__, protocol_idx_offset, action->udma_mask, action->queue_num);

	/* setup all steering control table entries */
	steering_ctrl_table_index.vlan_table_out = AL_ETH_FWD_CTRL_IDX_VLAN_TABLE_OUT_ANY;
	steering_ctrl_table_index.tunnel_exist = AL_ETH_FWD_CTRL_IDX_TUNNEL_ANY;
	steering_ctrl_table_index.vlan_exist = AL_ETH_FWD_CTRL_IDX_VLAN_ANY;
	steering_ctrl_table_index.mac_table_match = AL_ETH_FWD_CTRL_IDX_MAC_TABLE_ANY;
	steering_ctrl_table_index.protocol_id = protocol_idx;
	steering_ctrl_table_index.mac_type = AL_ETH_FWD_CTRL_IDX_MAC_DA_TYPE_ANY;

	steering_ctrl_table_entry.prio_sel = AL_ETH_CTRL_TABLE_PRIO_SEL_VAL_0;
	steering_ctrl_table_entry.queue_sel_1 = AL_ETH_CTRL_TABLE_QUEUE_SEL_1_THASH_TABLE;
	steering_ctrl_table_entry.queue_sel_2 = AL_ETH_CTRL_TABLE_QUEUE_SEL_2_NO_PRIO;
	steering_ctrl_table_entry.udma_sel = AL_ETH_CTRL_TABLE_UDMA_SEL_REG2 + protocol_idx_offset;
	steering_ctrl_table_entry.hdr_split_len_sel = AL_ETH_CTRL_TABLE_HDR_SPLIT_LEN_SEL_0;
	steering_ctrl_table_entry.filter = AL_FALSE;

	if ((action->queue_num >= 0) && (action->queue_num < AL_ETH_MAX_HW_QUEUES)) {
		/* We use the offset of the new protocol_idx from the default end_of_table entry
		 * idx, to decide which deafult_queue_sel reg to use.
		 * At the time of writing this function, the AL_ETH_EPE_ENTRIES_NUM==26, while the
		 * max size of the table is 32 so the maximal number of new protocol indexes is 6,
		 * while the number of queue_sel regs is 8, so we're good.
		 */
		steering_ctrl_table_entry.queue_sel_1 = AL_ETH_CTRL_TABLE_QUEUE_SEL_1_REG2 +
							protocol_idx_offset;
		al_mod_eth_fwd_default_queue_config(&adapter->hal_adapter,
						1 + protocol_idx_offset,
						action->queue_num);
	} else if (action->queue_num != -1) {
		netdev_err(adapter->netdev,
			   "Invalid queue_num parameter for flow_steering (%d). Valid range is 0-3 (-1 to keep default)\n",
			   action->queue_num);
		return -EINVAL;
	}

	/* setup default udma for this entry*/
	if ((action->udma_mask > 0x0) && (action->udma_mask <= 0xf)) {
		al_mod_eth_fwd_default_udma_config(&adapter->hal_adapter, 1 + protocol_idx_offset,
					       action->udma_mask);
	} else {
		netdev_err(adapter->netdev,
			   "Invalid udma_mask val parameter for flow_steering (%d). Valid range is 0x1-0xf\n",
			   action->udma_mask);
		return -EINVAL;
	}

	al_mod_eth_ctrl_table_set(&adapter->hal_adapter,
			      &steering_ctrl_table_index,
			      &steering_ctrl_table_entry);

	return 0;
}

static int al_mod_eth_flow_steer_first_user_free_entry_find(struct al_mod_eth_adapter *adapter)
{
	int i;

	struct al_mod_eth_epe_p_reg_entry parser_reg_entry;
	struct al_mod_eth_epe_control_entry parser_control_entry;

	for (i = 0; i < AL_ETH_EPE_ENTRIES_FREE_NUM; i++) {
		al_mod_eth_epe_entry_get(&adapter->hal_adapter,
				     i + (AL_ETH_EPE_ENTRIES_NUM - 1),
				     &parser_reg_entry,
				     &parser_control_entry,
				     0);
		if ((parser_reg_entry.ctrl & EC_EPE_P_COMP_CTRL_VALID) == AL_FALSE)
			return i;
	}

	return -ENOMEM;
}


static int al_mod_eth_flow_steer_add_entry(struct al_mod_eth_adapter *adapter,
				struct al_mod_eth_ip_flow_char *ip_flow_char,
				struct al_mod_eth_flow_steer_action *action)
{
	int rc;
	int first_free_entry_offset;
	int first_free_entry_idx;
	struct al_mod_eth_epe_p_reg_entry parser_reg_udp_entry;
	struct al_mod_eth_epe_control_entry parser_control_udp_entry;
	struct al_mod_eth_epe_p_reg_entry parser_reg_new_entry = {0x50000, 0x70000, 0x80000000};
	struct al_mod_eth_epe_control_entry parser_control_new_entry = {
		{0x280B000, 0x1F43028, 0x2168000, 0xC00000, 0x10100001, 0x40B800} };
	struct al_mod_eth_epe_p_reg_entry parser_reg_clear_entry = {0};
	struct al_mod_eth_epe_control_entry parser_control_clear_entry = {{0} };

	if (adapter->rev_id >= AL_ETH_REV_ID_4) {
		netdev_err(adapter->netdev, "%s : Doesnt not support eth v%d adv\n",
			__func__, adapter->rev_id);
		return -EOPNOTSUPP;
	}

	first_free_entry_offset = al_mod_eth_flow_steer_first_user_free_entry_find(adapter);
	if (first_free_entry_offset < 0)
		return first_free_entry_offset;
	first_free_entry_idx = first_free_entry_offset + (AL_ETH_EPE_ENTRIES_NUM - 1);

	/* Configure icrc hw offload for this flow. For simplicity we use the same entry_idx which
	   is used for the EPE tables */
	if (action->icrc_hw_enable == AL_TRUE) {
		al_mod_eth_flow_steer_rx_icrc_hw_offload_enable(adapter,
							(first_free_entry_offset +
							 AL_ETH_RX_PROTOCOL_DETECT_ENTRIES_NUM),
							first_free_entry_idx,
							0x1f,
							AL_TRUE);
	}

	/* Configure EPE with new protocol_idx */
	parser_reg_new_entry.ctrl = 0x80000000 | first_free_entry_idx;
	parser_reg_new_entry.data |= ip_flow_char->dst_port;
	parser_reg_new_entry.mask |= ip_flow_char->dst_port_mask;
	al_mod_eth_epe_entry_set(&adapter->hal_adapter,
			     first_free_entry_idx,
			     &parser_reg_new_entry,
			     &parser_control_new_entry);

	netdev_dbg(adapter->netdev, "EPE table user def first_free_entry_idx=%u\n",
		   first_free_entry_idx);

	/* Configure RX forwarding */
	rc = al_mod_eth_rx_steering_action_config(adapter, action, first_free_entry_idx);
	if (rc) {
		netdev_err(adapter->netdev, "failed to configure RX forwarding request, rc=%d\n",
			   rc);
		goto rx_fwd_err;
	}

	/* TODO: add support for TCP entry */
	/* disable end of parsing for UDP entry */
	al_mod_eth_epe_entry_get(&adapter->hal_adapter,
			     AL_ETH_PROTO_ID_UDP,
			     &parser_reg_udp_entry,
			     &parser_control_udp_entry,
			     0);

	if (AL_REG_FIELD_GET(parser_control_udp_entry.data[4], 1 << 0, 0) == AL_TRUE) {
		netdev_err(adapter->netdev, "%s: disable udp end of parse\n", __func__);
		al_mod_eth_epe_entry_set(&adapter->hal_adapter,
					      AL_ETH_PROTO_ID_UDP,
					      &parser_reg_udp_dis_end_of_parse_entry,
					      &parser_control_udp_dis_end_of_parse_entry);
	}

	adapter->sw_flow_steer_rule[first_free_entry_offset].action = *action;
	adapter->sw_flow_steer_rule[first_free_entry_offset].valid = AL_TRUE;

	netdev_info(adapter->netdev, "RX Flow steering entry added: protocol_idx=%d, ip_proto_type=%u, dst_port=%u, dst_port_mask=%u",
		    first_free_entry_idx,
		    ip_flow_char->protocol_type,
		    ip_flow_char->dst_port,
		    ip_flow_char->dst_port_mask);

	return 0;

rx_fwd_err:
	/* clear the new entry */
	al_mod_eth_epe_entry_set(&adapter->hal_adapter,
			     first_free_entry_idx,
			     &parser_reg_clear_entry,
			     &parser_control_clear_entry);

	al_mod_eth_flow_steer_rx_icrc_hw_offload_enable(adapter,
						    (first_free_entry_offset +
						     AL_ETH_RX_PROTOCOL_DETECT_ENTRIES_NUM),
						    first_free_entry_idx,
						    0x1f,
						    AL_FALSE);

	return rc;
}

static int al_mod_eth_flow_steer_vf_reset_to_default(struct al_mod_eth_adapter *adapter,
						 unsigned int udma_num)
{
	int i;
	struct al_mod_eth_epe_p_reg_entry parser_reg_clear_entry = {0};
	struct al_mod_eth_epe_control_entry parser_control_clear_entry = {{0} };
	struct al_mod_eth_fwd_ctrl_table_index ctrl_table_index = {0};
	struct al_mod_eth_fwd_ctrl_table_entry entry = {0};

	if (adapter->rev_id >= AL_ETH_REV_ID_4) {
		netdev_err(adapter->netdev, "%s : Doesnt not support eth v%d adv\n",
			__func__, adapter->rev_id);
		return -EOPNOTSUPP;
	}

	/* TODO: use macro for MAX NUM OF VFs */
	if ((udma_num == 0) || (udma_num >= 4)) {
		netdev_warn(adapter->netdev, "Invalid VF num (%u) to reset flow_steering rules. Only 1-3 are permitted\n",
			    udma_num);
		return -EINVAL;
	}

	for (i = 0; i < AL_ETH_EPE_ENTRIES_FREE_NUM; i++) {
		netdev_dbg(adapter->netdev, "%s: i=%d, adapter->sw_flow_steer_rule[i].valid=%d, adapter->sw_flow_steer_rule[i].action.udma_mask=%d\n",
			   __func__,
			   i,
			   adapter->sw_flow_steer_rule[i].valid,
			   adapter->sw_flow_steer_rule[i].action.udma_mask);
		if (adapter->sw_flow_steer_rule[i].valid &&
		    (adapter->sw_flow_steer_rule[i].action.udma_mask & (AL_BIT(udma_num)))) {
			/* invalidate entry in EPE tables */
			al_mod_eth_epe_entry_set(&adapter->hal_adapter,
					     (i + (AL_ETH_EPE_ENTRIES_NUM - 1)),
					     &parser_reg_clear_entry,
					     &parser_control_clear_entry);

			/* reset all entries in the RX control table corresponding to this
			 * protocol_id */
			ctrl_table_index.vlan_table_out = AL_ETH_FWD_CTRL_IDX_VLAN_TABLE_OUT_ANY;
			ctrl_table_index.tunnel_exist = AL_ETH_FWD_CTRL_IDX_TUNNEL_ANY;
			ctrl_table_index.vlan_exist = AL_ETH_FWD_CTRL_IDX_VLAN_ANY;
			ctrl_table_index.mac_table_match = AL_ETH_FWD_CTRL_IDX_MAC_TABLE_ANY;
			ctrl_table_index.protocol_id = i + (AL_ETH_EPE_ENTRIES_NUM - 1);
			ctrl_table_index.mac_type = AL_ETH_FWD_CTRL_IDX_MAC_DA_TYPE_ANY;

			entry.prio_sel = AL_ETH_CTRL_TABLE_PRIO_SEL_VAL_0;
			entry.queue_sel_1 = AL_ETH_CTRL_TABLE_QUEUE_SEL_1_THASH_TABLE;
			entry.queue_sel_2 = AL_ETH_CTRL_TABLE_QUEUE_SEL_2_NO_PRIO;
			entry.udma_sel = AL_ETH_CTRL_TABLE_UDMA_SEL_MAC_TABLE;
			entry.filter = AL_FALSE;

			al_mod_eth_ctrl_table_set(&adapter->hal_adapter,
					      &ctrl_table_index,
					      &entry);

			/* clear corresponding RX gpd and gcp entries */
			al_mod_eth_flow_steer_rx_icrc_hw_offload_enable(
							adapter,
							(i + AL_ETH_RX_PROTOCOL_DETECT_ENTRIES_NUM),
							0,
							0,
							false);

			adapter->sw_flow_steer_rule[i].valid = AL_FALSE;
			adapter->sw_flow_steer_rule[i].action =
				(struct al_mod_eth_flow_steer_action){0};
		}
	}

	return 0;
}

int al_mod_eth_flow_steer_config(struct al_mod_eth_adapter *adapter,
			     struct al_mod_eth_flow_steer_command *command)
{
	int rc = 0;

	switch (command->opcode) {
	case AL_ETH_UDMA_OVERRIDE:
		rc = al_mod_eth_udma_steer_override(adapter, command->udma_mask);
		break;
	case AL_ETH_QUEUE_OVERRIDE:
		rc = al_mod_eth_queue_steer_override(adapter, command->qid);
		break;
	case AL_ETH_FLOW_STEER_ADD:
		if (command->ip_flow_char.protocol_type != IPPROTO_UDP) {
			netdev_err(adapter->netdev, "Currently Only UDP is supported for flow steering\n");
		return -EPROTONOSUPPORT;
		}

		rc = al_mod_eth_flow_steer_add_entry(adapter, &command->ip_flow_char, &command->action);
		break;
	case AL_ETH_FLOW_STEER_RESET_VF:
		rc = al_mod_eth_flow_steer_vf_reset_to_default(adapter, command->udma_num);
		break;
	default:
		netdev_err(adapter->netdev, "Unsupported opcode for flow steering!\n");
		return -EOPNOTSUPP;
	}

	return rc;
}
#ifdef CONFIG_ARCH_ALPINE
int al_mod_eth_default_rx_udma_num_set(struct al_mod_eth_adapter *adapter, unsigned int rx_udma_num)
{
	unsigned int udma_num;

	/* Only Advanced ETH unit has multiple UDMAs */
	if (adapter->dev_id != AL_ETH_DEV_ID_ADVANCED)
		return -EINVAL;

	if (adapter->rev_id >= AL_ETH_REV_ID_4) {
		netdev_err(adapter->netdev, "%s: No support on eth rev id %d\n",
			   __func__, adapter->rev_id);
		return -EOPNOTSUPP;
	}

	/* For future support */
	if (IS_ETH_V4_ADV(adapter->rev_id, adapter->dev_id))
		/* For future support - we don't arrive here do to above ifs */
		udma_num = al_mod_eth_common_mode_num_adapters_get(adapter->hal_adapter.common_mode);
	else
		udma_num = AL_ETH_ADV_REV_ID_3_UDMA_NUM;

	if (rx_udma_num >= udma_num)
		return -EINVAL;

	adapter->default_rx_udma_num = rx_udma_num;

	/* By default, the UDMA to which packets are forwarded is determined by the MAC table match.
	 * see al_mod_eth_config_rx_fwd()
	 *
	 * A reference to adapter->udma_num is still present at :
	 * (*) al_mod_eth_fsm_table_init() - udma selection is through MAC table
	 *
	 * (*) FLR - FLR will be performed only for the udma which is controlled by al_mod_eth
	 * (i.e. adapter->udma_num). This means that the driver won't perform FLR
	 * for adapter->default_rx_udma_num (when it has been configured to be
	 * different than adapter->udma_num).
	 *
	 * (*) Flow control - If active, When setting a different rx_udma_num flow control packets
	 * will reach the al_mod_eth port MAC and will effect the adapter->udma_num udma.
	 * That means that if the adapter->default_rx_udma is starting to backpressure due to
	 * traffic directed to it from al_mod_eth it will send flow control packet (Xoff)
	 * via its paired tx udma and not adapter->tx_udma.
	 */

	al_mod_eth_mac_table_unicast_add(adapter, AL_ETH_MAC_TABLE_UNICAST_IDX_BASE,
				     adapter->mac_addr, 1 << rx_udma_num);
	al_mod_eth_mac_table_broadcast_add(adapter, AL_ETH_MAC_TABLE_BROADCAST_IDX, 1 << rx_udma_num);
	al_mod_eth_mac_table_promiscuous_set(adapter, false, 1 << rx_udma_num);

	netdev_info(adapter->netdev, "Set Default RX UDMA to %u\n", rx_udma_num);

	return 0;
}
#endif /* CONFIG_ARCH_ALPINE */

#ifdef HAVE_SRIOV_CONFIGURE
static int al_mod_eth_sriov_configure(struct pci_dev *dev, int numvfs)
{
#ifdef CONFIG_PCI_IOV
	int rc;

	if (numvfs > 0) {
		rc = pci_enable_sriov(dev, numvfs);
		if (rc != 0) {
			dev_err(&dev->dev,
				"pci_enable_sriov failed - %d vfs with the error: %d\n",
				numvfs, rc);
			return rc;
		}

		return numvfs;
	}

	if (numvfs == 0) {
		pci_disable_sriov(dev);
		return 0;
	}

	return -EINVAL;
#else /* CONFIG_PCI_IOV */
	return 0;
#endif /* CONFIG_PCI_IOV */
}
#endif /* HAVE_SRIOV_CONFIGURE */


static struct pci_driver al_mod_eth_pci_driver = {
	.name		= DRV_MODULE_NAME,
	.id_table	= al_mod_eth_pci_tbl,
	.probe		= al_mod_eth_probe,
	.remove		= al_mod_eth_remove,
	.shutdown	= al_mod_eth_remove,
#ifdef CONFIG_PM
	.suspend	= al_mod_eth_suspend,
	.resume		= al_mod_eth_resume,
#endif
#ifdef HAVE_SRIOV_CONFIGURE
	.sriov_configure = al_mod_eth_sriov_configure,
#endif /* HAVE_SRIOV_CONFIGURE */
};

#ifdef CONFIG_ARCH_ALPINE
static int al_mod_eth_rsrvd_mem_hdr_validate(void)
{
	struct al_mod_eth_rsrvd_mem_hdr *hdr;
	uint32_t crc;
	int rc;

	al_mod_assert(g_rsrvd_mem);

	hdr = (struct al_mod_eth_rsrvd_mem_hdr *)g_rsrvd_mem;

	if (hdr->hdr.magic != AL_ETH_RSRVD_MEM_HDR_MAGIC) {
		pr_warn("incompatible magic found in al_eth reserved_memory (0x%x)\n",
			hdr->hdr.magic);
		return -EINVAL;
	}

	if (hdr->hdr.type != AL_ETH_RSRVD_MEM_TYPE_GEN) {
		pr_warn("unsupported type found in al_eth reserved_memory (0x%x)\n",
			hdr->hdr.type);
		return -EINVAL;
	}

	if (hdr->hdr.ver > AL_ETH_RSRVD_MEM_VER_CURR) {
		pr_warn("unsupported version found in al_eth reserved_memory (%u). max ver supported by drv version - %u\n",
			hdr->hdr.type, AL_ETH_RSRVD_MEM_VER_CURR);
		return -EINVAL;
	}

	crc = hdr->hdr.crc;
	hdr->hdr.crc = 0;
	rc = al_mod_eth_validate_crc(g_rsrvd_mem, hdr->hdr.len, crc);
	if (rc) {
		pr_err("Failed to validte CRC of al_eth reserved_memory\n");
		hdr->hdr.crc = crc;
		return rc;
	}

	hdr->hdr.crc = crc;

	return 0;
}

#define AL_ETH_MEM_ENTRY_FMT "/reserved-memory/al_eth"
static int __init al_mod_eth_rsrvd_mem_map(void)
{
	struct device_node *node;
	struct resource res;
	resource_size_t size;
	int rc;

	node = of_find_node_by_path(AL_ETH_MEM_ENTRY_FMT);
	if (!node) {
		pr_debug("can't find node [%s]\n", AL_ETH_MEM_ENTRY_FMT);
		return -EINVAL;
	}

	rc = of_address_to_resource(node, 0, &res);
	if (rc) {
		pr_err("failed to parse reg property of %s (%d)\n",
			AL_ETH_MEM_ENTRY_FMT, rc);
		return rc;
	}

	size = resource_size(&res);
	if (size < AL_ETH_RSRVD_MEM_MIN_SIZE) {
		pr_err("reserved-memory for al_eth is to small (0x%llxp). Should be at least (0x%x)\n",
		       size, AL_ETH_RSRVD_MEM_MIN_SIZE);
		return -ENOMEM;
	}

	g_rsrvd_mem = memremap(res.start, size, MEMREMAP_WB);

	if (!g_rsrvd_mem) {
		pr_err("failed to map reserved base address, node [%s], res %pR\n",
			AL_ETH_MEM_ENTRY_FMT, &res);
		return -ENOMEM;
	}

	pr_debug("g_rsrvd_mem: %px\n", g_rsrvd_mem);

	return 0;
}

static int __init al_mod_eth_rsrvd_mem_init(void)
{
	int rc;

	live_update_restore_internal = live_update_restore;

	if (live_update_restore != AL_ETH_LIVE_UPDATE_RESTORE_ENABLE &&
	    live_update_save != AL_ETH_LIVE_UPDATE_SAVE_ENABLE) {
		pr_debug("Not using reserved memory for live update\n");
		return 0;
	}

	if (al_mod_eth_rsrvd_mem_map())
		goto err_internal_disable;

	rc = al_mod_eth_rsrvd_mem_hdr_validate();
	if (rc)
		goto err_internal_disable;

	return 0;

err_internal_disable:
	/* If the user requested to restore the previous state, but we either
	 * failed to map the rsrvd_mem or succeeded but failed in validation,
	 * then we signal that restore shouldn't be attempted.
	 * Notice: If mapping was successful, saving state is still permitted.
	 */
	if (live_update_restore == AL_ETH_LIVE_UPDATE_RESTORE_ENABLE)
		live_update_restore_internal =
			AL_ETH_LIVE_UPDATE_RESTORE_DISABLE;

	return 0;
}

static void __exit al_mod_eth_rsrvd_mem_unmap(void)
{
	if (g_rsrvd_mem)
		memunmap(g_rsrvd_mem);
}

static void __exit al_mod_eth_rsrvd_mem_exit(void)
{
	if (!g_rsrvd_mem)
		return;

	if (live_update_save == AL_ETH_LIVE_UPDATE_SAVE_ENABLE)
		al_mod_eth_rsrvd_mem_hdr_store();

	al_mod_eth_rsrvd_mem_unmap();
}
#endif /* CONFIG_ARCH_ALPINE */

static int __init al_mod_eth_init(void)
{
#ifdef CONFIG_AL_ETH_ALLOC_SKB
	struct sk_buff_head *rx_rc;
	int cpu;

	for_each_possible_cpu(cpu) {
		rx_rc =  &per_cpu(rx_recycle_cache, cpu);
		skb_queue_head_init(rx_rc);
	}
#endif

#ifdef CONFIG_ARCH_ALPINE
	/** Init shared resources */
	al_mod_eth_common_resource_init();

	al_mod_eth_pbs_regs_base_map();

	al_mod_eth_rsrvd_mem_init();
#endif

	return pci_register_driver(&al_mod_eth_pci_driver);
}

static void __exit al_mod_eth_cleanup(void)
{
#ifdef CONFIG_ARCH_ALPINE
	al_mod_eth_rsrvd_mem_exit();
	al_mod_eth_pbs_regs_base_unmap();
#endif

#ifdef CONFIG_AL_ETH_ALLOC_SKB
	struct sk_buff_head *rx_rc;
	int cpu;

	for_each_possible_cpu(cpu) {
		rx_rc =  &per_cpu(rx_recycle_cache, cpu);
		skb_queue_purge(rx_rc);
	}
#endif
	pci_unregister_driver(&al_mod_eth_pci_driver);
}

module_init(al_mod_eth_init);
module_exit(al_mod_eth_cleanup);
