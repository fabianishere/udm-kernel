/* al_eth.h: AnnapurnaLabs Unified 1GbE and 10GbE ethernet driver header.
 *
 * Copyright (c) 2013 AnnapurnaLabs
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation.
 */

#ifndef AL_ETH_H
#define AL_ETH_H

#include <linux/interrupt.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/inetdevice.h>

#include "kcompat.h"

#include "al_mod_hal_eth.h"
#ifdef CONFIG_ARCH_ALPINE
#include <al_mod_hal_udma_iofic.h>
#include <al_mod_hal_udma_debug.h>
#include <al_mod_serdes.h>
#include "al_mod_eth_v4_lm.h"
#else
#include "al_mod_hal_udma_iofic.h"
#include "al_mod_hal_udma_debug.h"
#include "al_mod_serdes.h"
#endif
#include "al_mod_init_eth_lm.h"
#include "al_mod_hal_eth_rfw.h"
#include "al_mod_hal_unit_adapter.h"

#include "al_mod_err_events.h"
#include "al_mod_err_events_udma.h"
#include "al_mod_err_events_eth.h"
#include "al_mod_err_events_io_fabric.h"

#ifndef BIT
#define BIT(nr)                 (1UL << (nr))
#endif

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

/* TODO: Need use a different naming convention since the version numbers
 * are a bit ambiguous (ETH unit revision vs. SoC/NIC version)
 */
#define IS_ETH_V4_ADV(rev_id, dev_id) \
	(((rev_id) == AL_ETH_REV_ID_4) && ((dev_id) == AL_ETH_DEV_ID_ADVANCED))

#define IS_ETH_V3_ADV(rev_id, dev_id) \
	(((rev_id) == AL_ETH_REV_ID_3) && ((dev_id) == AL_ETH_DEV_ID_ADVANCED))

#define IS_ETH_V4_PORT_NUM_IS_ADV(port_num) \
	(!((port_num == 1) || (port_num == 3)))

enum board_t {
	ALPINE_INTEGRATED = 0,
	ALPINE_NIC = 1,
	ALPINE_NIC_V2_10 = 2,
	ALPINE_NIC_V2_25 = 3,
	ALPINE_NIC_V2_25_DUAL = 4,
	ALPINE_NIC_V2_1G_TEST = 5,
	ALPINE_NIC_V3_100 = 6,
	ALPINE_NIC_V3_4x25 = 7,
	ALPINE_NIC_V3_1G_TEST = 8,
	ALPINE_NIC_V3_4x25_EX = 9,
	ALPINE_BOARD_TYPE_MAX
};

enum al_mod_eth_direction {
	AL_ETH_TX,
	AL_ETH_RX
};

/** HW related defines */

/** IOMAPS */
#define AL_ETH_SB_PBS_OFFSET		0x01880000
#define AL_ETH_PBS_REGFILE_OFFSET	0x00028000
/** Offset from start of SB */
#define AL_ETH_PBS_REGFILE_OFFSET_SB	(AL_ETH_SB_PBS_OFFSET + AL_ETH_PBS_REGFILE_OFFSET)

#ifndef AL_ETH_REV_ID_4
#define AL_ETH_REV_ID_4 4 /* Alpine V3 advanced */
#endif

/* TODO - Add support of this into iomap_dynamic */
/* Num of UDMAs per adapter in Alpine v2 ETH_ADV unit */
#define AL_ETH_ADV_REV_ID_3_UDMA_NUM	4
/* Num of UDMAs per adapter in Alpine v3 ETH_ADV unit */
#define AL_ETH_ADV_REV_ID_4_UDMA_NUM	1

#ifndef DMA_MAX_Q_V4
#define DMA_MAX_Q_V4 16
#endif

#ifndef AL_ETH_UDMA_HW_QUEUES_V4
#define AL_ETH_UDMA_HW_QUEUES_V4 DMA_MAX_Q_V4
#endif

#ifndef DMA_MAX_Q_V3
#define DMA_MAX_Q_V3 4
#endif

#ifndef AL_ETH_UDMA_HW_QUEUES_V3
#define AL_ETH_UDMA_HW_QUEUES_V3 DMA_MAX_Q_V3
#endif

#ifndef AL_ETH_UDMA_HW_QUEUES_BY_REV
#define AL_ETH_UDMA_HW_QUEUES_BY_REV(rev_id) \
	(((rev_id) >= AL_ETH_REV_ID_4) ? AL_ETH_UDMA_HW_QUEUES_V4 : AL_ETH_UDMA_HW_QUEUES_V3)
#endif

#define AL_ETH_MAX_HW_QUEUES	16
#define AL_ETH_NUM_QUEUES	4
#define AL_ETH_MAX_MSIX_VEC	(1 + 2 * AL_ETH_MAX_HW_QUEUES)

#define AL_ETH_MAX_DESCS_NUM	(1 << 13)
#define AL_ETH_MAX_TX_SW_DESCS	AL_ETH_MAX_DESCS_NUM
#define AL_ETH_MAX_TX_HW_DESCS	AL_ETH_MAX_DESCS_NUM
#define AL_ETH_MAX_RX_DESCS	AL_ETH_MAX_DESCS_NUM

#define AL_ETH_MIN_DESCS_NUM	(1 << 9)
#define AL_ETH_MIN_TX_SW_DESCS	AL_ETH_MIN_DESCS_NUM
#define AL_ETH_MIN_TX_HW_DESCS	AL_ETH_MIN_DESCS_NUM
#define AL_ETH_MIN_RX_DESCS	AL_ETH_MIN_DESCS_NUM

#define AL_ETH_DEFAULT_DESCS_NUM	(2*512)
#define AL_ETH_DEFAULT_TX_SW_DESCS	AL_ETH_DEFAULT_DESCS_NUM
#define AL_ETH_DEFAULT_TX_HW_DESCS	AL_ETH_DEFAULT_DESCS_NUM
#define AL_ETH_DEFAULT_RX_DESCS		(8*512)

#if ((AL_ETH_MIN_TX_SW_DESCS / 4) < (MAX_SKB_FRAGS + 2))
#define AL_ETH_TX_WAKEUP_THRESH		(AL_ETH_MIN_TX_SW_DESCS / 4)
#else
#define AL_ETH_TX_WAKEUP_THRESH		(MAX_SKB_FRAGS + 2)
#endif
#define AL_ETH_DEFAULT_SMALL_PACKET_LEN		(128 - NET_IP_ALIGN)
#define AL_ETH_HEADER_COPY_SIZE			(128 - NET_IP_ALIGN)

#define AL_ETH_DEFAULT_MAX_RX_BUFF_ALLOC_SIZE 1536
/*
 * minimum the buffer size to 600 to avoid situation the mtu will be changed
 * from too little buffer to very big one and then the number of buffer per
 * packet could reach the maximum AL_ETH_PKT_MAX_BUFS
 */
#define AL_ETH_DEFAULT_MIN_RX_BUFF_ALLOC_SIZE 600
#define AL_ETH_DEFAULT_FORCE_1000_BASEX AL_FALSE

#define AL_ETH_DEFAULT_LINK_POLL_INTERVAL 100
#define AL_ETH_FIRST_LINK_POLL_INTERVAL 1

#define AL_ETH_NAME_MAX_LEN	20
#define AL_ETH_IRQNAME_SIZE	40

#define AL_ETH_DEFAULT_MDIO_FREQ_KHZ	2500

#define AL_ETH_MIN_MTU			68
#define AL_ETH_MAX_MTU			9216

#define AL_ETH_ERR_EVENTS_POLL_IVAL	1000

#define AL_ETH_INTR_MODERATION_RESOLUTION   1 /* equivalent to ~1us in Alpine v2 */

#define AL_ETH_INTR_NO_INTERVAL            0
#define AL_ETH_INTR_NO_INTERVAL_USECS      0
#define AL_ETH_INTR_NO_INTERVAL_PKTS       4
#define AL_ETH_INTR_NO_INTERVAL_BYTES      4*1024

#define AL_ETH_INTR_LOWEST_USECS           0

#define AL_ETH_INTR_LOWEST_PKTS            3
#define AL_ETH_INTR_LOWEST_BYTES           2*1524

#define AL_ETH_INTR_LOW_USECS              32

#define AL_ETH_INTR_LOW_PKTS               12
#define AL_ETH_INTR_LOW_BYTES              16*1024

#define AL_ETH_INTR_MID_USECS              80

#define AL_ETH_INTR_MID_PKTS               48
#define AL_ETH_INTR_MID_BYTES              64*1024

#define AL_ETH_INTR_HIGH_USECS             128

#define AL_ETH_INTR_HIGH_PKTS              96
#define AL_ETH_INTR_HIGH_BYTES             128*1024

#define AL_ETH_INTR_HIGHEST_USECS          192

#define AL_ETH_INTR_HIGHEST_PKTS           128
#define AL_ETH_INTR_HIGHEST_BYTES          192*1024

#define AL_ETH_INTR_INITIAL_TX_INTERVAL_USECS		196
#define AL_ETH_INTR_INITIAL_RX_INTERVAL_USECS		0

struct al_mod_eth_irq {
	irq_handler_t	handler;
	void		*data;
	unsigned int	vector;
	u8		requested;
	cpumask_t	affinity_hint_mask;
	char		name[AL_ETH_IRQNAME_SIZE];
};

struct al_mod_eth_napi {
	struct napi_struct	napi		____cacheline_aligned;
	struct al_mod_eth_adapter	*adapter;
#ifndef HAVE_NETDEV_NAPI_LIST
        struct net_device poll_dev;
#endif
	unsigned int qid;
};

enum al_mod_eth_intr_moderation_level {
	AL_ETH_INTR_MODERATION_LOWEST = 0,
	AL_ETH_INTR_MODERATION_LOW,
	AL_ETH_INTR_MODERATION_MID,
	AL_ETH_INTR_MODERATION_HIGH,
	AL_ETH_INTR_MODERATION_HIGHEST,
	AL_ETH_INTR_MAX_NUM_OF_LEVELS,
};

struct al_mod_eth_intr_moderation_entry {
	unsigned int intr_moderation_interval;
	unsigned int packets_per_interval;
	unsigned int bytes_per_interval;
};

struct al_mod_eth_tx_buffer {
	struct sk_buff *skb;
	struct al_mod_eth_pkt hal_pkt;
	unsigned int	tx_descs;
};

struct al_mod_eth_rx_buffer {
	struct sk_buff *skb;
	struct page *page;
	unsigned int page_offset;
#if (defined(CONFIG_AL_ETH_ALLOC_FRAG) || defined(CONFIG_AL_ETH_ALLOC_SKB))
	u8 *data;
	unsigned int data_size;
	unsigned int frag_size; /* used in rx skb allocation */
#endif
	DEFINE_DMA_UNMAP_ADDR(dma);
	struct al_mod_buf	al_mod_buf;
};

#define AL_ETH_RX_OFFSET	NET_SKB_PAD + NET_IP_ALIGN

struct al_mod_eth_stats_tx {
	u64 packets;
	u64 bytes;
	u64 tso_small_mss;
	u64 tso_mss_seg_pad_bad;
};

struct al_mod_eth_stats_rx {
	u64 packets;
	u64 bytes;
	u64 skb_alloc_fail;
	u64 dma_mapping_err;
	u64 small_copy_len_pkt;
	u64 buf_alloc_fail;
	u64 partial_rx_ring_refill;
};

struct al_mod_eth_ring {
	struct device *dev;
	struct napi_struct	*napi;
	struct al_mod_eth_pkt hal_pkt; /* used to get rx packets from hal */
	struct al_mod_udma_q *dma_q; /* udma queue handler */
	u16 next_to_use;
	u16 next_to_clean;
	u32 __iomem *unmask_reg_offset; /* the offset of the interrupt unmask register */
	u32	unmask_val; /* the value to write to the above register to
			     * unmask the interrupt of this ring
			     */
	/* need to use union here */
	struct al_mod_eth_meta_data hal_meta;
	struct al_mod_eth_tx_buffer *tx_buffer_info; /* contex of tx packet */
	struct al_mod_eth_rx_buffer *rx_buffer_info; /* contex of rx packet */
	int	sw_count; /* number of tx/rx_buffer_info's entries */
	int	hw_count; /* number of hw descriptors */
	size_t	descs_size; /* size (in bytes) of hw descriptors */
	size_t	cdescs_size; /* size (in bytes) of hw completion descriptors, used
			 for rx */
	enum al_mod_eth_intr_moderation_level moderation_table_indx; /* interrupt coalescing */
	unsigned int smoothed_interval;
	/** Adaptive packets & bytes thresholds for interrupt moderation */
	unsigned int packets;
	unsigned int bytes;

	struct net_device *netdev;
	struct al_mod_udma_q_params	q_params;

	struct u64_stats_sync syncp;
	union {
		struct al_mod_eth_stats_tx tx_stats;
		struct al_mod_eth_stats_rx rx_stats;
	};
};

#define MDIO_BUS_SHARED_NAME "alpine_mdio_shared"

struct al_mod_mdio_bus_adapter {
	struct al_mod_eth_adapter *adapter;
	struct list_head list; /* kernel's list structure */
};

struct al_mod_mdio_bus_shared_data {
	struct list_head adapter_list;
	struct al_mod_eth_adapter *mac_active;
	struct mutex mac_lock;
	uint8_t use_counter;
};

static inline void al_mod_eth_mdio_lock_init(struct mii_bus *mdio_bus)
{
	if (likely(mdio_bus)) {
		BUG_ON(!mdio_bus->priv);
		mutex_init(&((struct al_mod_mdio_bus_shared_data *)mdio_bus->priv)->mac_lock);
	}
}

static inline void al_mod_eth_mdio_lock(struct mii_bus *mdio_bus)
{
	if (likely(mdio_bus)) {
		BUG_ON(!mdio_bus->priv);
		mutex_lock(&((struct al_mod_mdio_bus_shared_data *)mdio_bus->priv)->mac_lock);
	}
}

static inline void al_mod_eth_mdio_unlock(struct mii_bus *mdio_bus)
{
	if (likely(mdio_bus)) {
		BUG_ON(!mdio_bus->priv);
		mutex_unlock(&((struct al_mod_mdio_bus_shared_data *)mdio_bus->priv)->mac_lock);
	}
}

#define AL_ETH_TX_RING_IDX_NEXT(tx_ring, idx)	(((idx) + 1) & ((tx_ring)->sw_count - 1))

#define AL_ETH_RX_RING_IDX_NEXT(rx_ring, idx)	(((idx) + 1) & ((rx_ring)->sw_count - 1))
#define AL_ETH_RX_RING_IDX_ADD(rx_ring, idx, n)	(((idx) + (n)) & ((rx_ring)->sw_count - 1))

/* flow control configuration */
#define AL_ETH_FLOW_CTRL_RX_FIFO_TH_HIGH	0x160
#define AL_ETH_FLOW_CTRL_RX_FIFO_TH_LOW		0x90
#define AL_ETH_FLOW_CTRL_QUANTA			0xffff
#define AL_ETH_FLOW_CTRL_QUANTA_TH		0x8000

#define AL_ETH_FLOW_CTRL_AUTONEG  BIT(0)
#define AL_ETH_FLOW_CTRL_RX_PAUSE BIT(1)
#define AL_ETH_FLOW_CTRL_TX_PAUSE BIT(2)

/* link configuration for 1G port */
struct al_mod_eth_link_config {
	int old_link;
	/* Describes what we actually have. */
	int	active_duplex;
	int	active_speed;

	/* current flow control status */
	uint8_t flow_ctrl_active;
	/* supported configuration (can be changed from ethtool) */
	uint8_t flow_ctrl_supported;

	/* the following are not relevant to RGMII */
	bool	force_1000_base_x;
	bool	autoneg;
};

/* SFP detection event */
enum al_mod_eth_sfp_detect_evt {
	/* No change (no connect, disconnect, or new SFP module */
	AL_ETH_SFP_DETECT_EVT_NO_CHANGE,
	/* SFP module connected */
	AL_ETH_SFP_DETECT_EVT_CONNECTED,
	/* SFP module disconnected */
	AL_ETH_SFP_DETECT_EVT_DISCONNECTED,
	/* SFP module replaced */
	AL_ETH_SFP_DETECT_EVT_CHANGED,
};

/* SFP detection status */
struct al_mod_eth_sfp_detect_stat {
	/* Status is valid (i.e. rest of fields are valid) */
	bool			valid;
	bool			connected;
	uint8_t			sfp_10g;
	uint8_t			sfp_1g;
	uint8_t			sfp_cable_tech;

	bool			lt_en;
	bool			an_en;
	enum al_mod_eth_mac_mode	mac_mode;
};

/* Retimer parameters */
struct al_mod_eth_retimer_params {
	al_mod_bool				exist;
	enum al_mod_eth_retimer_type	type;
	uint8_t				bus_id;
	uint8_t				i2c_addr;
	enum al_mod_eth_retimer_channel	channel;
	enum al_mod_eth_retimer_channel	tx_channel;
};

struct al_mod_eth_stats_dev {
	u64 tx_timeout;
	u64 interface_up;
	u64 interface_down;
	u64 fec_ce;
	u64 fec_ue;
	u64 ua_errors;
	u64 ec_errors;
	u64 mac_errors;
	u64 udma_errors;
};

enum al_mod_eth_flow_steer_opcode {
	AL_ETH_UDMA_OVERRIDE,
	AL_ETH_QUEUE_OVERRIDE,
	AL_ETH_FLOW_STEER_ADD,
	AL_ETH_FLOW_STEER_RESET_VF,
};


struct al_mod_eth_ip_flow_char {
	u8 protocol_type;
	u16 dst_port;
	u16 dst_port_mask;
};

struct al_mod_eth_flow_steer_action {
	u8 udma_mask;
	int8_t queue_num;
	al_mod_bool icrc_hw_enable;

};

struct al_mod_eth_flow_steer_command {
	enum al_mod_eth_flow_steer_opcode opcode;
	union {
		struct {
			struct al_mod_eth_ip_flow_char ip_flow_char;
			struct al_mod_eth_flow_steer_action action;
		};
		unsigned int udma_num; /* used for AL_ETH_FLOW_STEER_RESET_VF */
		uint8_t udma_mask; /* used for AL_ETH_UDMA_OVERRIDE */
		int qid; /* used for AL_ETH_QUEUE_OVERRIDE */
	};
};

struct al_mod_eth_flow_steer_rule_sw {
	bool valid;
	struct al_mod_eth_flow_steer_action action;
};

#define AL_ETH_EPE_OFFSET_FROM_LAST_DEFAULT_ENTRY(abs_idx) (abs_idx - (AL_ETH_EPE_ENTRIES_NUM - 1))
/* This macro is defined to be min(7, number of usable EPE entries incl. AL_ETH_PROTO_ID_USR_DEF) */
#if (AL_ETH_PROTOCOLS_NUM - 1 - (AL_ETH_EPE_ENTRIES_NUM - 1)) > 7
#define AL_ETH_EPE_ENTRIES_FREE_NUM 7
#else
#define AL_ETH_EPE_ENTRIES_FREE_NUM (AL_ETH_PROTOCOLS_NUM - AL_ETH_EPE_ENTRIES_NUM)
#endif

/* In the current implementation, the number of free entries in the RX gpd/gcp tables must be at
 * least equal to the number of free entries in the EPE tables.
 */
#if (AL_ETH_EPE_ENTRIES_FREE_NUM >					\
	((AL_ETH_RX_GPD_TABLE_SIZE - 1) - AL_ETH_RX_PROTOCOL_DETECT_ENTRIES_NUM))
#error "AL_ETH_EPE_ENTRIES_FREE_NUM is larger than AL_ETH_RX_GPD_ENTRIES_FREE_NUM"
#define AL_ETH_RX_GPD_ENTRIES_FREE_NUM ((AL_ETH_RX_GPD_TABLE_SIZE - 1) - \
					AL_ETH_RX_PROTOCOL_DETECT_ENTRIES_NUM)
#else
#define AL_ETH_RX_GPD_ENTRIES_FREE_NUM AL_ETH_EPE_ENTRIES_FREE_NUM
#endif

/**
 * ETHv4 uses a different LRSS table that can be segmented to various sizes.
 * This is done by early stages of the boot FW. The current size is defined
 * by AL_ETH_V4_ADV_RX_FWD_RSS_TABLE_SIZE.
 *
 * For pre ETHv4, the size of the RSS table is AL_ETH_RX_THASH_TABLE_SIZE.
 *
 * AL_ETH_RX_RSS_TABLE_SIZE holds the maximum supported table size (across ETH
 * unit revisions), while the adapter->rss_ind_tbl_size field holds the
 * effective table size according to the ETH revision
 */
#define AL_ETH_V4_ADV_RX_FWD_RSS_TABLE_SIZE AL_ETH_RFW_LRSS_SUBTABLE_ENTRIES	/* 256 */
#define AL_ETH_V3_ADV_RX_FWD_RSS_TABLE_SIZE AL_ETH_RX_THASH_TABLE_SIZE		/* 256 */
#define AL_ETH_RX_RSS_TABLE_SIZE \
	MAX(AL_ETH_V3_ADV_RX_FWD_RSS_TABLE_SIZE, AL_ETH_V4_ADV_RX_FWD_RSS_TABLE_SIZE)

/* board specific private data structure */
struct al_mod_eth_adapter {
	/* OS defined structs */
	struct net_device *netdev;
	struct pci_dev *pdev;
	enum board_t	board_type;
	u16 dev_id;
	u8 rev_id;
	u8 orig_rev_id;
	u8 port_num;
	u8 al_mod_chip_id;
	u8 pf_num;
	u8 pcie_ep_num;
	/* Some features need tri-state capability,
	 * thus the additional *_CAPABLE flags.
	 */
	u32 flags;
#define AL_ETH_FLAG_MSIX_CAPABLE		(u32)(1 << 1)
#define AL_ETH_FLAG_MSIX_ENABLED		(u32)(1 << 2)
#define AL_ETH_FLAG_IN_NETPOLL			(u32)(1 << 3)
#define AL_ETH_FLAG_MQ_CAPABLE			(u32)(1 << 4)
#define AL_ETH_FLAG_SRIOV_CAPABLE		(u32)(1 << 5)
#define AL_ETH_FLAG_SRIOV_ENABLED		(u32)(1 << 6)
#define AL_ETH_FLAG_RESET_REQUESTED		(u32)(1 << 7)
#define AL_ETH_FLAG_CLOSE_ONGOING		(u32)(1 << 8)
#define AL_ETH_FLAG_RESET_UDMA_EC		(u32)(1 << 9)

	struct al_mod_hal_eth_adapter hal_adapter;
	struct al_mod_eth_rfw_handle rfw_handle;
#ifdef AL_ETH_HAS_COMMON_MODE
	enum al_mod_eth_common_mode common_mode;
#endif
	/*
	 * rx packets that shorter that this len will be copied to the skb
	 * header
	 */
	unsigned int small_copy_len;

	/* Maximum size for rx buffer */
	unsigned int max_rx_buff_alloc_size;

	/* Tx fast path data */
	int num_tx_queues;

	/* Rx fast path data */
	int num_rx_queues;

	/* TX */
	struct al_mod_eth_ring tx_ring[AL_ETH_MAX_HW_QUEUES] ____cacheline_aligned_in_smp;

	/* RX */
	struct al_mod_eth_ring rx_ring[AL_ETH_MAX_HW_QUEUES];

#define AL_ETH_RXQ_NAPI_IDX(adapter, q)	(q)
#define AL_ETH_TXQ_NAPI_IDX(adapter, q)	((adapter)->num_rx_queues + (q))
	struct al_mod_eth_napi al_mod_napi[2 * AL_ETH_MAX_HW_QUEUES];

	enum al_mod_iofic_mode int_mode;
	unsigned int adaptive_intr_rate;

#define AL_ETH_MGMT_IRQ_IDX		0
#define AL_ETH_RXQ_IRQ_IDX(adapter, q)	(1 + (q))
#define AL_ETH_TXQ_IRQ_IDX(adapter, q)	(1 + AL_ETH_UDMA_HW_QUEUES_BY_REV((adapter)->rev_id) + (q))
	struct al_mod_eth_irq irq_tbl[AL_ETH_MAX_MSIX_VEC];
	struct msix_entry *msix_entries;
	int	msix_vecs;
	int	irq_vecs;
	struct al_mod_eth_intr_moderation_entry intr_moderation_table[AL_ETH_INTR_MAX_NUM_OF_LEVELS];
	unsigned int tx_usecs, rx_usecs; /* interrupt coalescing */

	unsigned int tx_ring_count;
	unsigned int tx_descs_count;
	unsigned int rx_ring_count;
	unsigned int rx_descs_count;

	/* RSS*/
	uint32_t toeplitz_hash_key[AL_ETH_RX_HASH_KEY_NUM];
	uint8_t	 rss_ind_tbl[AL_ETH_RX_RSS_TABLE_SIZE];
	unsigned int rss_ind_tbl_size;
	uint32_t msg_enable;
	struct al_mod_eth_mac_stats mac_stats;

	enum al_mod_eth_mac_mode	mac_mode;
	bool			mac_mode_set; /* Relevant only when 'auto_speed' is set */
	u8 mac_addr[ETH_ALEN];
	/* mdio and phy*/
	bool			phy_exist;
	struct mii_bus		*mdio_bus;
	struct phy_device	*phydev;
	uint8_t			phy_addr;
	struct al_mod_eth_link_config	link_config;

	/* HAL layer data */
	char	name[AL_ETH_NAME_MAX_LEN];
	void __iomem	*internal_sb_base;
	void __iomem	*internal_pcie_base; /* use for ALPINE_NIC devices */
	void __iomem	*udma_base;
	void __iomem	*ec_base;
	void __iomem	*mac_base;
	void __iomem	*serdes_base;

	unsigned int	udma_num;

	struct al_mod_eth_flow_control_params flow_ctrl_params;

	struct al_mod_eth_adapter_params eth_hal_params;

	struct delayed_work	link_status_task;
	uint32_t		link_poll_interval; /* task interval in mSec */

	bool			serdes_init;
	struct al_mod_serdes_grp_obj	*serdes_obj;
	uint8_t			serdes_grp;
	uint8_t			serdes_lane;

	bool			an_en;	/* run kr auto-negotiation */
	bool			lt_en;	/* run kr link-training */
	bool			sfp_probe_1g;  /* on true probe 1G SFP modules */
	bool			sfp_probe_10g; /* on true probe 10G SFP modules */

	al_mod_bool		sfp_detection_needed; /**< true if need to run sfp detection */
	bool		auto_speed; /**< true if allowed to change SerDes speed configuration */
	uint8_t		i2c_adapter_id; /**< identifier for the i2c adapter to use to access SFP+ module */
	enum al_mod_eth_ref_clk_freq	ref_clk_freq; /**< reference clock frequency */
	unsigned int	mdio_freq; /**< MDIO frequency [Khz] */
	enum al_mod_eth_board_ext_phy_if	phy_if;

	bool up;
#ifdef CONFIG_ARCH_ALPINE
	struct al_mod_eth_v4_lm 	eth_v4_lm_handle;
#endif
	bool				last_link;
	bool				last_establish_failed;
	struct al_mod_eth_lm_context	lm_context;
	bool				use_lm;

	struct al_mod_eth_group_lm_context	*group_lm_context;

	bool				dont_override_serdes; /**< avoid overriding serdes parameters
								   to preset static values */
	uint32_t wol;

	struct al_mod_eth_retimer_params	retimer;

	struct u64_stats_sync syncp;
	struct al_mod_eth_stats_dev dev_stats;

	bool				phy_fixup_needed;

	enum al_mod_eth_lm_max_speed	max_speed;
	bool				speed_detection;
	al_mod_bool				auto_fec_enable;
	unsigned int			gpio_spd_1g;
	unsigned int			gpio_spd_10g;
	unsigned int			gpio_spd_25g;
	unsigned int			gpio_sfp_present;
	al_mod_bool				kr_fec_enable;

	struct 	work_struct 		reset_task;

	uint8_t				udma_steer_mask;
	int				queue_steer_id;

	bool				user_def_flow_steering_restored;
	struct al_mod_eth_flow_steer_rule_sw sw_flow_steer_rule[AL_ETH_EPE_ENTRIES_FREE_NUM + 1];
	uint32_t			rx_ctrl_table_def_backup;
	uint32_t			rx_ctrl_table_backup[AL_ETH_RX_CTRL_TABLE_SIZE];
	uint32_t			rfw_default_backup[8];
	struct al_mod_eth_epe_p_reg_entry	user_def_epe_reg_entry[AL_ETH_EPE_ENTRIES_FREE_NUM];
	struct al_mod_eth_epe_control_entry	user_def_epe_control_entry[AL_ETH_EPE_ENTRIES_FREE_NUM];
#ifdef AL_ETH_ADAPTER_PARAMS_HAS_UNIT_ADAPTER
	struct al_mod_unit_adapter unit_adapter;
#endif
	bool				user_def_rx_icrc_restored;
	struct al_mod_eth_rx_gpd_cam_entry	user_def_rx_icrc_gpd_entry[AL_ETH_RX_GPD_ENTRIES_FREE_NUM];
	struct al_mod_eth_rx_gcp_table_entry user_def_rx_icrc_gcp_entry[AL_ETH_RX_GPD_ENTRIES_FREE_NUM];
	unsigned int			default_rx_udma_num;

	void __iomem			*pbs_regfile_base;

	bool				err_events_enabled;
	struct al_mod_err_events_handle	err_events_handle;
	struct al_mod_err_events_udma_data	udma_data;
	struct al_mod_err_events_eth_ec_data	ec_data;
	struct al_mod_err_events_unit_adapter_data	ua_data;

	struct delayed_work		err_events_task;
	bool				eth_error_ready;
	spinlock_t			stats_lock;

	void __iomem			*unit_adapter_base;
	struct al_mod_eth_irq		ua_irq;

	void __iomem			*eth_common_base;
	void __iomem			*ec_0_base;
	struct al_mod_eth_common_handle	hal_eth_common_handle;

	bool				mac_started;
	void __iomem			*rsrvd_mem;
	bool				live_update_restore_state;
};

#endif /* !(AL_ETH_H) */
