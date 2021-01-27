/*
 * Copyright 2016, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#ifndef _KCOMPAT_H_
#define _KCOMPAT_H_

#ifndef LINUX_VERSION_CODE
#include <linux/version.h>
#else
#define KERNEL_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))
#endif
#include <linux/init.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/ioport.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <linux/mii.h>
#include <linux/vmalloc.h>
#include <asm/io.h>
#include <linux/ethtool.h>
#include <linux/phy.h>
#include <linux/if_vlan.h>
#include <linux/u64_stats_sync.h>

#define adapter_struct al_mod_eth_adapter
#define adapter_q_vector al_mod_eth_napi

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30)
#include "workqueue.h"
#endif

/* UTS_RELEASE is in a different header starting in kernel 2.6.18 */
#ifndef UTS_RELEASE
/* utsrelease.h changed locations in 2.6.33 */
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33) )
#include <linux/utsrelease.h>
#else
#include <generated/utsrelease.h>
#endif
#endif

/* NAPI enable/disable flags here */
#define NAPI

/* MSI compatibility code for all kernels and drivers */
#ifdef DISABLE_PCI_MSI
#undef CONFIG_PCI_MSI
#endif
#ifndef CONFIG_PCI_MSI
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,8) )
struct msix_entry {
	u16 vector; /* kernel uses to write allocated vector */
	u16 entry;  /* driver uses to specify entry, OS writes */
};
#endif
#undef pci_enable_msi
#define pci_enable_msi(a) -ENOTSUPP
#undef pci_disable_msi
#define pci_disable_msi(a) do {} while (0)
#undef pci_enable_msix
#define pci_enable_msix(a, b, c) -ENOTSUPP
#undef pci_disable_msix
#define pci_disable_msix(a) do {} while (0)
#define msi_remove_pci_irq_vectors(a) do {} while (0)
#endif /* CONFIG_PCI_MSI */
#ifdef DISABLE_PM
#undef CONFIG_PM
#endif

#ifdef DISABLE_NET_POLL_CONTROLLER
#undef CONFIG_NET_POLL_CONTROLLER
#endif

#ifndef PMSG_SUSPEND
#define PMSG_SUSPEND 3
#endif

#ifndef SET_NETDEV_DEV
#define SET_NETDEV_DEV(net, pdev)
#endif

#if !defined(HAVE_FREE_NETDEV) && ( LINUX_VERSION_CODE < KERNEL_VERSION(3,1,0) )
#define free_netdev(x)	kfree(x)
#endif

#ifdef HAVE_POLL_CONTROLLER
#define CONFIG_NET_POLL_CONTROLLER
#endif

#ifndef SKB_DATAREF_SHIFT
/* if we do not have the infrastructure to detect if skb_header is cloned
   just return false in all cases */
#define skb_header_cloned(x) 0
#endif

#ifndef NETIF_F_GSO
#define gso_size tso_size
#define gso_segs tso_segs
#endif

#ifndef NETIF_F_GRO
#define vlan_gro_receive(_napi, _vlgrp, _vlan, _skb) \
		vlan_hwaccel_receive_skb(_skb, _vlgrp, _vlan)
#define napi_gro_receive(_napi, _skb) netif_receive_skb(_skb)
#endif

#ifndef NETIF_F_SCTP_CSUM
#define NETIF_F_SCTP_CSUM 0
#endif

#ifndef NETIF_F_LRO
#define NETIF_F_LRO (1 << 15)
#endif

#ifndef NETIF_F_NTUPLE
#define NETIF_F_NTUPLE (1 << 27)
#endif

#ifndef IPPROTO_SCTP
#define IPPROTO_SCTP 132
#endif

#ifndef CHECKSUM_PARTIAL
#define CHECKSUM_PARTIAL CHECKSUM_HW
#define CHECKSUM_COMPLETE CHECKSUM_HW
#endif

#ifndef PCI_DEVICE
#define PCI_DEVICE(vend,dev) \
	.vendor = (vend), .device = (dev), \
	.subvendor = PCI_ANY_ID, .subdevice = PCI_ANY_ID
#endif

#ifndef DECLARE_BITMAP
#ifndef BITS_TO_LONGS
#define BITS_TO_LONGS(bits) (((bits)+BITS_PER_LONG-1)/BITS_PER_LONG)
#endif
#define DECLARE_BITMAP(name,bits) long name[BITS_TO_LONGS(bits)]
#endif

#ifndef VLAN_HLEN
#define VLAN_HLEN 4
#endif

#ifndef VLAN_ETH_HLEN
#define VLAN_ETH_HLEN 18
#endif

#ifndef VLAN_ETH_FRAME_LEN
#define VLAN_ETH_FRAME_LEN 1518
#endif

#ifndef DCA_GET_TAG_TWO_ARGS
#define dca3_get_tag(a,b) dca_get_tag(b)
#endif

#ifndef CONFIG_HAVE_EFFICIENT_UNALIGNED_ACCESS
#if defined(__i386__) || defined(__x86_64__)
#define CONFIG_HAVE_EFFICIENT_UNALIGNED_ACCESS
#endif
#endif

/* taken from 2.6.24 definition in linux/kernel.h */
#ifndef IS_ALIGNED
#define IS_ALIGNED(x,a)         (((x) % ((typeof(x))(a))) == 0)
#endif

#ifdef IS_ENABLED
#undef IS_ENABLED
#undef __ARG_PLACEHOLDER_1
#undef config_enabled
#undef _config_enabled
#undef __config_enabled
#undef ___config_enabled
#endif

#define __ARG_PLACEHOLDER_1 0,
#define config_enabled(cfg) _config_enabled(cfg)
#define _config_enabled(value) __config_enabled(__ARG_PLACEHOLDER_##value)
#define __config_enabled(arg1_or_junk) ___config_enabled(arg1_or_junk 1, 0)
#define ___config_enabled(__ignored, val, ...) val

#define IS_ENABLED(option) \
	(config_enabled(option) || config_enabled(option##_MODULE))

#if !defined(NETIF_F_HW_VLAN_TX) && !defined(NETIF_F_HW_VLAN_CTAG_TX)
struct _kc_vlan_ethhdr {
	unsigned char	h_dest[ETH_ALEN];
	unsigned char	h_source[ETH_ALEN];
	__be16		h_vlan_proto;
	__be16		h_vlan_TCI;
	__be16		h_vlan_encapsulated_proto;
};
#define vlan_ethhdr _kc_vlan_ethhdr
struct _kc_vlan_hdr {
	__be16		h_vlan_TCI;
	__be16		h_vlan_encapsulated_proto;
};
#define vlan_hdr _kc_vlan_hdr
#define vlan_tx_tag_present(_skb) 0
#define vlan_tx_tag_get(_skb) 0
#endif /* NETIF_F_HW_VLAN_TX && NETIF_F_HW_VLAN_CTAG_TX */

#ifndef VLAN_PRIO_SHIFT
#define VLAN_PRIO_SHIFT 13
#endif

#ifndef __GFP_COLD
#define __GFP_COLD 0
#endif

#ifndef __GFP_COMP
#define __GFP_COMP 0
#endif

/*****************************************************************************/
/* Installations with ethtool version without eeprom, adapter id, or statistics
 * support */

#ifndef ETH_GSTRING_LEN
#define ETH_GSTRING_LEN 32
#endif

#ifndef ETHTOOL_GSTATS
#define ETHTOOL_GSTATS 0x1d
#undef ethtool_drvinfo
#define ethtool_drvinfo k_ethtool_drvinfo
struct k_ethtool_drvinfo {
	u32 cmd;
	char driver[32];
	char version[32];
	char fw_version[32];
	char bus_info[32];
	char reserved1[32];
	char reserved2[16];
	u32 n_stats;
	u32 testinfo_len;
	u32 eedump_len;
	u32 regdump_len;
};

struct ethtool_stats {
	u32 cmd;
	u32 n_stats;
	u64 data[0];
};
#endif /* ETHTOOL_GSTATS */

#ifndef ETHTOOL_PHYS_ID
#define ETHTOOL_PHYS_ID 0x1c
#endif /* ETHTOOL_PHYS_ID */

#ifndef ETHTOOL_GSTRINGS
#define ETHTOOL_GSTRINGS 0x1b
enum ethtool_stringset {
	ETH_SS_TEST             = 0,
	ETH_SS_STATS,
};
struct ethtool_gstrings {
	u32 cmd;            /* ETHTOOL_GSTRINGS */
	u32 string_set;     /* string set id e.c. ETH_SS_TEST, etc*/
	u32 len;            /* number of strings in the string set */
	u8 data[0];
};
#endif /* ETHTOOL_GSTRINGS */

#ifndef ETHTOOL_TEST
#define ETHTOOL_TEST 0x1a
enum ethtool_test_flags {
	ETH_TEST_FL_OFFLINE	= (1 << 0),
	ETH_TEST_FL_FAILED	= (1 << 1),
};
struct ethtool_test {
	u32 cmd;
	u32 flags;
	u32 reserved;
	u32 len;
	u64 data[0];
};
#endif /* ETHTOOL_TEST */

#ifndef ETHTOOL_GEEPROM
#define ETHTOOL_GEEPROM 0xb
#undef ETHTOOL_GREGS
struct ethtool_eeprom {
	u32 cmd;
	u32 magic;
	u32 offset;
	u32 len;
	u8 data[0];
};

struct ethtool_value {
	u32 cmd;
	u32 data;
};
#endif /* ETHTOOL_GEEPROM */

#ifndef ETHTOOL_GLINK
#define ETHTOOL_GLINK 0xa
#endif /* ETHTOOL_GLINK */

#ifndef ETHTOOL_GWOL
#define ETHTOOL_GWOL 0x5
#define ETHTOOL_SWOL 0x6
#define SOPASS_MAX      6
struct ethtool_wolinfo {
	u32 cmd;
	u32 supported;
	u32 wolopts;
	u8 sopass[SOPASS_MAX]; /* SecureOn(tm) password */
};
#endif /* ETHTOOL_GWOL */

#ifndef ETHTOOL_GREGS
#define ETHTOOL_GREGS		0x00000004 /* Get NIC registers */
#define ethtool_regs _kc_ethtool_regs
/* for passing big chunks of data */
struct _kc_ethtool_regs {
	u32 cmd;
	u32 version; /* driver-specific, indicates different chips/revs */
	u32 len; /* bytes */
	u8 data[0];
};
#endif /* ETHTOOL_GREGS */

#ifndef ETHTOOL_GMSGLVL
#define ETHTOOL_GMSGLVL		0x00000007 /* Get driver message level */
#endif
#ifndef ETHTOOL_SMSGLVL
#define ETHTOOL_SMSGLVL		0x00000008 /* Set driver msg level, priv. */
#endif
#ifndef ETHTOOL_NWAY_RST
#define ETHTOOL_NWAY_RST	0x00000009 /* Restart autonegotiation, priv */
#endif
#ifndef ETHTOOL_GLINK
#define ETHTOOL_GLINK		0x0000000a /* Get link status */
#endif
#ifndef ETHTOOL_GEEPROM
#define ETHTOOL_GEEPROM		0x0000000b /* Get EEPROM data */
#endif
#ifndef ETHTOOL_SEEPROM
#define ETHTOOL_SEEPROM		0x0000000c /* Set EEPROM data */
#endif
#ifndef ETHTOOL_GCOALESCE
#define ETHTOOL_GCOALESCE	0x0000000e /* Get coalesce config */
/* for configuring coalescing parameters of chip */
#define ethtool_coalesce _kc_ethtool_coalesce
struct _kc_ethtool_coalesce {
	u32	cmd;	/* ETHTOOL_{G,S}COALESCE */

	/* How many usecs to delay an RX interrupt after
	 * a packet arrives.  If 0, only rx_max_coalesced_frames
	 * is used.
	 */
	u32	rx_coalesce_usecs;

	/* How many packets to delay an RX interrupt after
	 * a packet arrives.  If 0, only rx_coalesce_usecs is
	 * used.  It is illegal to set both usecs and max frames
	 * to zero as this would cause RX interrupts to never be
	 * generated.
	 */
	u32	rx_max_coalesced_frames;

	/* Same as above two parameters, except that these values
	 * apply while an IRQ is being serviced by the host.  Not
	 * all cards support this feature and the values are ignored
	 * in that case.
	 */
	u32	rx_coalesce_usecs_irq;
	u32	rx_max_coalesced_frames_irq;

	/* How many usecs to delay a TX interrupt after
	 * a packet is sent.  If 0, only tx_max_coalesced_frames
	 * is used.
	 */
	u32	tx_coalesce_usecs;

	/* How many packets to delay a TX interrupt after
	 * a packet is sent.  If 0, only tx_coalesce_usecs is
	 * used.  It is illegal to set both usecs and max frames
	 * to zero as this would cause TX interrupts to never be
	 * generated.
	 */
	u32	tx_max_coalesced_frames;

	/* Same as above two parameters, except that these values
	 * apply while an IRQ is being serviced by the host.  Not
	 * all cards support this feature and the values are ignored
	 * in that case.
	 */
	u32	tx_coalesce_usecs_irq;
	u32	tx_max_coalesced_frames_irq;

	/* How many usecs to delay in-memory statistics
	 * block updates.  Some drivers do not have an in-memory
	 * statistic block, and in such cases this value is ignored.
	 * This value must not be zero.
	 */
	u32	stats_block_coalesce_usecs;

	/* Adaptive RX/TX coalescing is an algorithm implemented by
	 * some drivers to improve latency under low packet rates and
	 * improve throughput under high packet rates.  Some drivers
	 * only implement one of RX or TX adaptive coalescing.  Anything
	 * not implemented by the driver causes these values to be
	 * silently ignored.
	 */
	u32	use_adaptive_rx_coalesce;
	u32	use_adaptive_tx_coalesce;

	/* When the packet rate (measured in packets per second)
	 * is below pkt_rate_low, the {rx,tx}_*_low parameters are
	 * used.
	 */
	u32	pkt_rate_low;
	u32	rx_coalesce_usecs_low;
	u32	rx_max_coalesced_frames_low;
	u32	tx_coalesce_usecs_low;
	u32	tx_max_coalesced_frames_low;

	/* When the packet rate is below pkt_rate_high but above
	 * pkt_rate_low (both measured in packets per second) the
	 * normal {rx,tx}_* coalescing parameters are used.
	 */

	/* When the packet rate is (measured in packets per second)
	 * is above pkt_rate_high, the {rx,tx}_*_high parameters are
	 * used.
	 */
	u32	pkt_rate_high;
	u32	rx_coalesce_usecs_high;
	u32	rx_max_coalesced_frames_high;
	u32	tx_coalesce_usecs_high;
	u32	tx_max_coalesced_frames_high;

	/* How often to do adaptive coalescing packet rate sampling,
	 * measured in seconds.  Must not be zero.
	 */
	u32	rate_sample_interval;
};
#endif /* ETHTOOL_GCOALESCE */

#ifndef ETHTOOL_SCOALESCE
#define ETHTOOL_SCOALESCE	0x0000000f /* Set coalesce config. */
#endif
#ifndef ETHTOOL_GRINGPARAM
#define ETHTOOL_GRINGPARAM	0x00000010 /* Get ring parameters */
/* for configuring RX/TX ring parameters */
#define ethtool_ringparam _kc_ethtool_ringparam
struct _kc_ethtool_ringparam {
	u32	cmd;	/* ETHTOOL_{G,S}RINGPARAM */

	/* Read only attributes.  These indicate the maximum number
	 * of pending RX/TX ring entries the driver will allow the
	 * user to set.
	 */
	u32	rx_max_pending;
	u32	rx_mini_max_pending;
	u32	rx_jumbo_max_pending;
	u32	tx_max_pending;

	/* Values changeable by the user.  The valid values are
	 * in the range 1 to the "*_max_pending" counterpart above.
	 */
	u32	rx_pending;
	u32	rx_mini_pending;
	u32	rx_jumbo_pending;
	u32	tx_pending;
};
#endif /* ETHTOOL_GRINGPARAM */

#ifndef ETHTOOL_SRINGPARAM
#define ETHTOOL_SRINGPARAM	0x00000011 /* Set ring parameters, priv. */
#endif
#ifndef ETHTOOL_GPAUSEPARAM
#define ETHTOOL_GPAUSEPARAM	0x00000012 /* Get pause parameters */
/* for configuring link flow control parameters */
#define ethtool_pauseparam _kc_ethtool_pauseparam
struct _kc_ethtool_pauseparam {
	u32	cmd;	/* ETHTOOL_{G,S}PAUSEPARAM */

	/* If the link is being auto-negotiated (via ethtool_cmd.autoneg
	 * being true) the user may set 'autoneg' here non-zero to have the
	 * pause parameters be auto-negotiated too.  In such a case, the
	 * {rx,tx}_pause values below determine what capabilities are
	 * advertised.
	 *
	 * If 'autoneg' is zero or the link is not being auto-negotiated,
	 * then {rx,tx}_pause force the driver to use/not-use pause
	 * flow control.
	 */
	u32	autoneg;
	u32	rx_pause;
	u32	tx_pause;
};
#endif /* ETHTOOL_GPAUSEPARAM */

#ifndef ETHTOOL_SPAUSEPARAM
#define ETHTOOL_SPAUSEPARAM	0x00000013 /* Set pause parameters. */
#endif
#ifndef ETHTOOL_GRXCSUM
#define ETHTOOL_GRXCSUM		0x00000014 /* Get RX hw csum enable (ethtool_value) */
#endif
#ifndef ETHTOOL_SRXCSUM
#define ETHTOOL_SRXCSUM		0x00000015 /* Set RX hw csum enable (ethtool_value) */
#endif
#ifndef ETHTOOL_GTXCSUM
#define ETHTOOL_GTXCSUM		0x00000016 /* Get TX hw csum enable (ethtool_value) */
#endif
#ifndef ETHTOOL_STXCSUM
#define ETHTOOL_STXCSUM		0x00000017 /* Set TX hw csum enable (ethtool_value) */
#endif
#ifndef ETHTOOL_GSG
#define ETHTOOL_GSG		0x00000018 /* Get scatter-gather enable
					    * (ethtool_value) */
#endif
#ifndef ETHTOOL_SSG
#define ETHTOOL_SSG		0x00000019 /* Set scatter-gather enable
					    * (ethtool_value). */
#endif
#ifndef ETHTOOL_TEST
#define ETHTOOL_TEST		0x0000001a /* execute NIC self-test, priv. */
#endif
#ifndef ETHTOOL_GSTRINGS
#define ETHTOOL_GSTRINGS	0x0000001b /* get specified string set */
#endif
#ifndef ETHTOOL_PHYS_ID
#define ETHTOOL_PHYS_ID		0x0000001c /* identify the NIC */
#endif
#ifndef ETHTOOL_GSTATS
#define ETHTOOL_GSTATS		0x0000001d /* get NIC-specific statistics */
#endif
#ifndef ETHTOOL_GTSO
#define ETHTOOL_GTSO		0x0000001e /* Get TSO enable (ethtool_value) */
#endif
#ifndef ETHTOOL_STSO
#define ETHTOOL_STSO		0x0000001f /* Set TSO enable (ethtool_value) */
#endif

#ifndef ETHTOOL_BUSINFO_LEN
#define ETHTOOL_BUSINFO_LEN	32
#endif

#ifndef RHEL_RELEASE_VERSION
#define RHEL_RELEASE_VERSION(a,b) (((a) << 8) + (b))
#endif
#ifndef AX_RELEASE_VERSION
#define AX_RELEASE_VERSION(a,b) (((a) << 8) + (b))
#endif

#ifndef AX_RELEASE_CODE
#define AX_RELEASE_CODE 0
#endif

#if (AX_RELEASE_CODE && AX_RELEASE_CODE == AX_RELEASE_VERSION(3,0))
#define RHEL_RELEASE_CODE RHEL_RELEASE_VERSION(5,0)
#elif (AX_RELEASE_CODE && AX_RELEASE_CODE == AX_RELEASE_VERSION(3,1))
#define RHEL_RELEASE_CODE RHEL_RELEASE_VERSION(5,1)
#elif (AX_RELEASE_CODE && AX_RELEASE_CODE == AX_RELEASE_VERSION(3,2))
#define RHEL_RELEASE_CODE RHEL_RELEASE_VERSION(5,3)
#endif

#ifndef RHEL_RELEASE_CODE
/* NOTE: RHEL_RELEASE_* introduced in RHEL4.5 */
#define RHEL_RELEASE_CODE 0
#endif

/* Ubuntu Release ABI is the 4th digit of their kernel version. You can find
 * it in /usr/src/linux/$(uname -r)/include/generated/utsrelease.h for new
 * enough versions of Ubuntu. Otherwise you can simply see it in the output of
 * uname as the 4th digit of the kernel. The UTS_UBUNTU_RELEASE_ABI is not in
 * the linux-source package, but in the linux-headers package. It begins to
 * appear in later releases of 14.04 and 14.10.
 *
 * Ex:
 * <Ubuntu 14.04.1>
 *  $uname -r
 *  3.13.0-45-generic
 * ABI is 45
 *
 * <Ubuntu 14.10>
 *  $uname -r
 *  3.16.0-23-generic
 * ABI is 23
 */
#ifndef UTS_UBUNTU_RELEASE_ABI
#define UTS_UBUNTU_RELEASE_ABI 0
#define UBUNTU_VERSION_CODE 0
#else
/* Ubuntu does not provide actual release version macro, so we use the kernel
 * version plus the ABI to generate a unique version code specific to Ubuntu.
 * In addition, we mask the lower 8 bits of LINUX_VERSION_CODE in order to
 * ignore differences in sublevel which are not important since we have the
 * ABI value. Otherwise, it becomes impossible to correlate ABI to version for
 * ordering checks.
 */
#define UBUNTU_VERSION_CODE (((LINUX_VERSION_CODE & ~0xFF) << 8) + (UTS_UBUNTU_RELEASE_ABI))

#if UTS_UBUNTU_RELEASE_ABI > 255
#error UTS_UBUNTU_RELEASE_ABI is too large...
#endif /* UTS_UBUNTU_RELEASE_ABI > 255 */

#if ( LINUX_VERSION_CODE <= KERNEL_VERSION(3,0,0) )
/* Our version code scheme does not make sense for non 3.x or newer kernels,
 * and we have no support in kcompat for this scenario. Thus, treat this as a
 * non-Ubuntu kernel. Possibly might be better to error here.
 */
#define UTS_UBUNTU_RELEASE_ABI 0
#define UBUNTU_VERSION_CODE 0
#endif

#endif

/* Note that the 3rd digit is always zero, and will be ignored. This is
 * because Ubuntu kernels are based on x.y.0-ABI values, and while their linux
 * version codes are 3 digit, this 3rd digit is superseded by the ABI value.
 */
#define UBUNTU_VERSION(a,b,c,d) ((KERNEL_VERSION(a,b,0) << 8) + (d))

/**
 * RHEL / CentOS specific
 */
#ifndef RHEL_RELEASE_VERSION
#define RHEL_RELEASE_VERSION(a,b) (((a) << 8) + (b))
#endif
#ifndef AX_RELEASE_VERSION
#define AX_RELEASE_VERSION(a,b) (((a) << 8) + (b))
#endif

#ifndef AX_RELEASE_CODE
#define AX_RELEASE_CODE 0
#endif

#if (AX_RELEASE_CODE && AX_RELEASE_CODE == AX_RELEASE_VERSION(3,0))
#define RHEL_RELEASE_CODE RHEL_RELEASE_VERSION(5,0)
#elif (AX_RELEASE_CODE && AX_RELEASE_CODE == AX_RELEASE_VERSION(3,1))
#define RHEL_RELEASE_CODE RHEL_RELEASE_VERSION(5,1)
#elif (AX_RELEASE_CODE && AX_RELEASE_CODE == AX_RELEASE_VERSION(3,2))
#define RHEL_RELEASE_CODE RHEL_RELEASE_VERSION(5,3)
#endif

#ifndef RHEL_RELEASE_CODE
/* NOTE: RHEL_RELEASE_* introduced in RHEL4.5 */
#define RHEL_RELEASE_CODE 0
#endif

/* SuSE version macro is the same as Linux kernel version */
#ifndef SLE_VERSION
#define SLE_VERSION(a,b,c) KERNEL_VERSION(a,b,c)
#endif
#ifdef CONFIG_SUSE_KERNEL
#if ( LINUX_VERSION_CODE == KERNEL_VERSION(2,6,27) )
/* SLES11 GA is 2.6.27 based */
#define SLE_VERSION_CODE SLE_VERSION(11,0,0)
#elif ( LINUX_VERSION_CODE == KERNEL_VERSION(2,6,32) )
/* SLES11 SP1 is 2.6.32 based */
#define SLE_VERSION_CODE SLE_VERSION(11,1,0)
#elif ( LINUX_VERSION_CODE == KERNEL_VERSION(3,0,13) )
/* SLES11 SP2 is 3.0.13 based */
#define SLE_VERSION_CODE SLE_VERSION(11,2,0)
#elif ((LINUX_VERSION_CODE == KERNEL_VERSION(3,0,76)))
/* SLES11 SP3 is 3.0.76 based */
#define SLE_VERSION_CODE SLE_VERSION(11,3,0)
#endif /* LINUX_VERSION_CODE == KERNEL_VERSION(x,y,z) */
#endif /* CONFIG_SUSE_KERNEL */
#ifndef SLE_VERSION_CODE
#define SLE_VERSION_CODE 0
#endif /* SLE_VERSION_CODE */

#ifdef __KLOCWORK__
#ifdef ARRAY_SIZE
#undef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif
#endif /* __KLOCWORK__ */

#ifndef HAVE_PCI_SET_MWI
#define pci_set_mwi(X) pci_write_config_word(X, \
			       PCI_COMMAND, adapter->hw.bus.pci_cmd_word | \
			       PCI_COMMAND_INVALIDATE);
#define pci_clear_mwi(X) pci_write_config_word(X, \
			       PCI_COMMAND, adapter->hw.bus.pci_cmd_word & \
			       ~PCI_COMMAND_INVALIDATE);
#endif

/*****************************************************************************/
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18) )

#ifndef IRQ_HANDLED
#define irqreturn_t void
#define IRQ_HANDLED
#define IRQ_NONE
#endif

#ifndef IRQF_PROBE_SHARED
#ifdef SA_PROBEIRQ
#define IRQF_PROBE_SHARED SA_PROBEIRQ
#else
#define IRQF_PROBE_SHARED 0
#endif
#endif

#ifndef IRQF_SHARED
#define IRQF_SHARED SA_SHIRQ
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

#ifndef FIELD_SIZEOF
#define FIELD_SIZEOF(t, f) (sizeof(((t*)0)->f))
#endif

#ifndef skb_is_gso
#ifdef NETIF_F_TSO
#define skb_is_gso _kc_skb_is_gso
static inline int _kc_skb_is_gso(const struct sk_buff *skb)
{
	return skb_shinfo(skb)->gso_size;
}
#else
#define skb_is_gso(a) 0
#endif
#endif

#ifndef resource_size_t
#define resource_size_t unsigned long
#endif

#ifdef skb_pad
#undef skb_pad
#endif
#define skb_pad(x,y) _kc_skb_pad(x, y)
int _kc_skb_pad(struct sk_buff *skb, int pad);
#ifdef skb_padto
#undef skb_padto
#endif
#define skb_padto(x,y) _kc_skb_padto(x, y)
static inline int _kc_skb_padto(struct sk_buff *skb, unsigned int len)
{
	unsigned int size = skb->len;
	if(likely(size >= len))
		return 0;
	return _kc_skb_pad(skb, len - size);
}

#ifndef DECLARE_PCI_UNMAP_ADDR
#define DECLARE_PCI_UNMAP_ADDR(ADDR_NAME) \
	dma_addr_t ADDR_NAME
#define DECLARE_PCI_UNMAP_LEN(LEN_NAME) \
	u32 LEN_NAME
#define pci_unmap_addr(PTR, ADDR_NAME) \
	((PTR)->ADDR_NAME)
#define pci_unmap_addr_set(PTR, ADDR_NAME, VAL) \
	(((PTR)->ADDR_NAME) = (VAL))
#define pci_unmap_len(PTR, LEN_NAME) \
	((PTR)->LEN_NAME)
#define pci_unmap_len_set(PTR, LEN_NAME, VAL) \
	(((PTR)->LEN_NAME) = (VAL))
#endif /* DECLARE_PCI_UNMAP_ADDR */
#endif /* < 2.6.18 */

/*****************************************************************************/
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19) )

#if (!(RHEL_RELEASE_CODE && RHEL_RELEASE_CODE >= RHEL_RELEASE_VERSION(5,0)))
#define i_private u.generic_ip
#endif /* >= RHEL 5.0 */

#ifndef DIV_ROUND_UP
#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))
#endif
#ifndef __ALIGN_MASK
#define __ALIGN_MASK(x, mask) (((x) + (mask)) & ~(mask))
#endif
#if ( LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,0) )
#if (!((RHEL_RELEASE_CODE && \
        ((RHEL_RELEASE_CODE > RHEL_RELEASE_VERSION(4,4) && \
          RHEL_RELEASE_CODE < RHEL_RELEASE_VERSION(5,0)) || \
         (RHEL_RELEASE_CODE > RHEL_RELEASE_VERSION(5,0))))))
typedef irqreturn_t (*irq_handler_t)(int, void*, struct pt_regs *);
#endif
#if (RHEL_RELEASE_CODE && RHEL_RELEASE_CODE < RHEL_RELEASE_VERSION(6,0))
#undef CONFIG_INET_LRO
#undef CONFIG_INET_LRO_MODULE
#undef CONFIG_FCOE
#undef CONFIG_FCOE_MODULE
#endif
typedef irqreturn_t (*new_handler_t)(int, void*);
static inline irqreturn_t _kc_request_irq(unsigned int irq, new_handler_t handler, unsigned long flags, const char *devname, void *dev_id)
#else /* 2.4.x */
typedef void (*irq_handler_t)(int, void*, struct pt_regs *);
typedef void (*new_handler_t)(int, void*);
static inline int _kc_request_irq(unsigned int irq, new_handler_t handler, unsigned long flags, const char *devname, void *dev_id)
#endif /* >= 2.5.x */
{
	irq_handler_t new_handler = (irq_handler_t) handler;
	return request_irq(irq, new_handler, flags, devname, dev_id);
}

#undef request_irq
#define request_irq(irq, handler, flags, devname, dev_id) _kc_request_irq((irq), (handler), (flags), (devname), (dev_id))

#define irq_handler_t new_handler_t
/* pci_restore_state and pci_save_state handles MSI/PCIE from 2.6.19 */
#if (!(RHEL_RELEASE_CODE && RHEL_RELEASE_CODE >= RHEL_RELEASE_VERSION(5,4)))
#define PCIE_CONFIG_SPACE_LEN 256
#define PCI_CONFIG_SPACE_LEN 64
#define PCIE_LINK_STATUS 0x12
#define pci_config_space_ich8lan() do {} while(0)
#undef pci_save_state
extern int _kc_pci_save_state(struct pci_dev *);
#define pci_save_state(pdev) _kc_pci_save_state(pdev)
#undef pci_restore_state
extern void _kc_pci_restore_state(struct pci_dev *);
#define pci_restore_state(pdev) _kc_pci_restore_state(pdev)
#endif /* !(RHEL_RELEASE_CODE >= RHEL 5.4) */

#ifdef HAVE_PCI_ERS
#undef free_netdev
extern void _kc_free_netdev(struct net_device *);
#define free_netdev(netdev) _kc_free_netdev(netdev)
#endif
static inline int pci_enable_pcie_error_reporting(struct pci_dev *dev)
{
	return 0;
}
#define pci_disable_pcie_error_reporting(dev) do {} while (0)
#define pci_cleanup_aer_uncorrect_error_status(dev) do {} while (0)

#else /* 2.6.19 */
#include <linux/aer.h>
#include <linux/string.h>
#endif /* < 2.6.19 */

/*****************************************************************************/
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,20) )
#define HAVE_EARLY_VMALLOC_NODE
#define dev_to_node(dev) -1
#undef set_dev_node
/* remove compiler warning with b=b, for unused variable */
#define set_dev_node(a, b) do { (b) = (b); } while(0)

#if (!(RHEL_RELEASE_CODE && \
       (((RHEL_RELEASE_CODE >= RHEL_RELEASE_VERSION(4,7)) && \
         (RHEL_RELEASE_CODE < RHEL_RELEASE_VERSION(5,0))) || \
        (RHEL_RELEASE_CODE >= RHEL_RELEASE_VERSION(5,6)))) && \
     !(SLE_VERSION_CODE && SLE_VERSION_CODE >= SLE_VERSION(10,2,0)))
typedef __u16 __bitwise __sum16;
typedef __u32 __bitwise __wsum;
#endif

#else /* < 2.6.20 */
#define HAVE_DEVICE_NUMA_NODE
#endif /* < 2.6.20 */

/*****************************************************************************/
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,21) )
#define to_net_dev(class) container_of(class, struct net_device, class_dev)
#define NETDEV_CLASS_DEV
#if (!(RHEL_RELEASE_CODE && RHEL_RELEASE_CODE > RHEL_RELEASE_VERSION(5,5)))
#define vlan_group_get_device(vg, id) (vg->vlan_devices[id])
#define vlan_group_set_device(vg, id, dev)		\
	do {						\
		if (vg) vg->vlan_devices[id] = dev;	\
	} while (0)
#endif /* !(RHEL_RELEASE_CODE > RHEL_RELEASE_VERSION(5,5)) */
#define pci_channel_offline(pdev) (pdev->error_state && \
	pdev->error_state != pci_channel_io_normal)
#define pci_request_selected_regions(pdev, bars, name) \
        pci_request_regions(pdev, name)
#define pci_release_selected_regions(pdev, bars) pci_release_regions(pdev);

#ifndef __aligned
#define __aligned(x)			__attribute__((aligned(x)))
#endif

extern struct pci_dev *_kc_netdev_to_pdev(struct net_device *netdev);
#define netdev_to_dev(netdev)	\
	pci_dev_to_dev(_kc_netdev_to_pdev(netdev))
#else
static inline struct device *netdev_to_dev(struct net_device *netdev)
{
	return &netdev->dev;
}

#endif /* < 2.6.21 */

/*****************************************************************************/
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,22) )
#ifndef alloc_etherdev_mq
#define alloc_etherdev_mq(_a, _b) alloc_etherdev(_a)
#endif
#else /* 2.6.22 */
#define ETH_TYPE_TRANS_SETS_DEV
#define HAVE_NETDEV_STATS_IN_NETDEV
#endif /* < 2.6.22 */

/*****************************************************************************/
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,23) )

#ifndef CONFIG_PM_SLEEP
#define CONFIG_PM_SLEEP	CONFIG_PM
#endif

#if ( LINUX_VERSION_CODE > KERNEL_VERSION(2,6,13) )
#define HAVE_ETHTOOL_GET_PERM_ADDR
#endif /* 2.6.14 through 2.6.22 */
#endif /* < 2.6.23 */

/*****************************************************************************/
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24) )
#ifndef ETH_FLAG_LRO
#define ETH_FLAG_LRO NETIF_F_LRO
#endif

/* if GRO is supported then the napi struct must already exist */
#ifndef NETIF_F_GRO
/* NAPI API changes in 2.6.24 break everything */
struct napi_struct {
	/* used to look up the real NAPI polling routine */
	int (*poll)(struct napi_struct *, int);
	struct net_device *dev;
	int weight;
};
#endif

#ifdef NAPI
extern int __kc_adapter_clean(struct net_device *, int *);
extern struct net_device *napi_to_poll_dev(const struct napi_struct *napi);
#define netif_napi_add(_netdev, _napi, _poll, _weight) \
	__kc_netif_napi_add(_netdev, _napi, _poll, _weight)
static inline void __kc_netif_napi_add(struct net_device *dev, struct napi_struct *napi,
				       int (*poll)(struct napi_struct *, int), int weight)
{
	struct net_device *poll_dev = napi_to_poll_dev(napi);
	poll_dev->poll = &(__kc_adapter_clean);
	poll_dev->priv = napi;
	poll_dev->weight = weight;
	set_bit(__LINK_STATE_RX_SCHED, &poll_dev->state);
	set_bit(__LINK_STATE_START, &poll_dev->state);
	dev_hold(poll_dev);
	napi->poll = poll;
	printk(KERN_ERR "napi_add napi=%p poll=%p\n", napi, poll);
	napi->weight = weight;
	napi->dev = dev;
}

#define netif_napi_del(_napi) \
	do { \
		struct net_device *poll_dev = napi_to_poll_dev(_napi); \
		WARN_ON(!test_bit(__LINK_STATE_RX_SCHED, &poll_dev->state)); \
		dev_put(poll_dev); \
		memset(poll_dev, 0, sizeof(struct net_device));\
	} while (0)
#define napi_schedule_prep(_napi) \
	(netif_running((_napi)->dev) && netif_rx_schedule_prep(napi_to_poll_dev(_napi)))
#define napi_schedule(_napi) \
	do { \
		if (napi_schedule_prep(_napi)) \
			__netif_rx_schedule(napi_to_poll_dev(_napi)); \
	} while (0)
#define napi_enable(_napi) netif_poll_enable(napi_to_poll_dev(_napi))
#define napi_disable(_napi) netif_poll_disable(napi_to_poll_dev(_napi))
#ifdef CONFIG_SMP
static inline void napi_synchronize(const struct napi_struct *n)
{
	struct net_device *dev = napi_to_poll_dev(n);

	while (test_bit(__LINK_STATE_RX_SCHED, &dev->state)) {
		/* No hurry. */
		msleep(1);
	}
}
#else
#define napi_synchronize(n)	barrier()
#endif /* CONFIG_SMP */
#define __napi_schedule(_napi) __netif_rx_schedule(napi_to_poll_dev(_napi))
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24) )
/* Try to reschedule poll. Called by dev->poll() after napi_complete().  */
static inline int napi_reschedule(struct napi_struct *napi)
{
	if (napi_schedule_prep(napi)) {
		       __napi_schedule(napi);
		       return 1;
	}
	return 0;
}
#endif
#ifndef NETIF_F_GRO
#define napi_complete(_napi) netif_rx_complete(napi_to_poll_dev(_napi))
#else
#define napi_complete(_napi) \
	do { \
		napi_gro_flush(_napi); \
		netif_rx_complete(napi_to_poll_dev(_napi)); \
	} while (0)
#endif /* NETIF_F_GRO */
#else /* NAPI */
#define netif_napi_add(_netdev, _napi, _poll, _weight) \
	do { \
		struct napi_struct *__napi = _napi; \
		_netdev->poll = &(_poll); \
		_netdev->weight = (_weight); \
		__napi->poll = &(_poll); \
		__napi->weight = (_weight); \
		__napi->dev = (_netdev); \
	} while (0)
#define netif_napi_del(_a) do {} while (0)
#endif /* NAPI */

#undef dev_get_by_name
#define dev_get_by_name(_a, _b) dev_get_by_name(_b)
#define __netif_subqueue_stopped(_a, _b) netif_subqueue_stopped(_a, _b)
#ifndef DMA_BIT_MASK
#define DMA_BIT_MASK(n)	(((n) == 64) ? DMA_64BIT_MASK : ((1ULL<<(n))-1))
#endif

#else /* < 2.6.24 */
#define HAVE_ETHTOOL_GET_SSET_COUNT
#define HAVE_NETDEV_NAPI_LIST
#endif /* < 2.6.24 */

/*****************************************************************************/
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27) )
#if ( LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,15) )
#if ((LINUX_VERSION_CODE < KERNEL_VERSION(2,6,23)) && defined(CONFIG_PM))
#define ANCIENT_PM 1
#elif ((LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,23)) && \
       (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)) && \
       defined(CONFIG_PM_SLEEP))
#define NEWER_PM 1
#endif
#if defined(ANCIENT_PM) || defined(NEWER_PM)
#undef device_set_wakeup_enable
#define device_set_wakeup_enable(dev, val) \
	do { \
		u16 pmc = 0; \
		int pm = pci_find_capability(adapter->pdev, PCI_CAP_ID_PM); \
		if (pm) { \
			pci_read_config_word(adapter->pdev, pm + PCI_PM_PMC, \
				&pmc); \
		} \
		(dev)->power.can_wakeup = !!(pmc >> 11); \
		(dev)->power.should_wakeup = (val && (pmc >> 11)); \
	} while (0)
#endif /* 2.6.15-2.6.22 and CONFIG_PM or 2.6.23-2.6.25 and CONFIG_PM_SLEEP */
#endif /* 2.6.15 through 2.6.27 */
#ifndef netif_napi_del
#define netif_napi_del(_a) do {} while (0)
#ifdef NAPI
#ifdef CONFIG_NETPOLL
#undef netif_napi_del
#define netif_napi_del(_a) list_del(&(_a)->dev_list);
#endif
#endif
#endif /* netif_napi_del */
#ifdef dma_mapping_error
#undef dma_mapping_error
#endif
#define dma_mapping_error(dev, dma_addr) pci_dma_mapping_error(dma_addr)

#ifdef CONFIG_NETDEVICES_MULTIQUEUE
#define HAVE_TX_MQ
#endif

#ifdef HAVE_TX_MQ
extern void _kc_netif_tx_stop_all_queues(struct net_device *);
extern void _kc_netif_tx_wake_all_queues(struct net_device *);
extern void _kc_netif_tx_start_all_queues(struct net_device *);
#define netif_tx_stop_all_queues(a) _kc_netif_tx_stop_all_queues(a)
#define netif_tx_wake_all_queues(a) _kc_netif_tx_wake_all_queues(a)
#define netif_tx_start_all_queues(a) _kc_netif_tx_start_all_queues(a)
#undef netif_stop_subqueue
#define netif_stop_subqueue(_ndev,_qi) do { \
	if (netif_is_multiqueue((_ndev))) \
		netif_stop_subqueue((_ndev), (_qi)); \
	else \
		netif_stop_queue((_ndev)); \
	} while (0)
#undef netif_start_subqueue
#define netif_start_subqueue(_ndev,_qi) do { \
	if (netif_is_multiqueue((_ndev))) \
		netif_start_subqueue((_ndev), (_qi)); \
	else \
		netif_start_queue((_ndev)); \
	} while (0)
#else /* HAVE_TX_MQ */
#define netif_tx_stop_all_queues(a) netif_stop_queue(a)
#define netif_tx_wake_all_queues(a) netif_wake_queue(a)
#if ( LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,12) )
#define netif_tx_start_all_queues(a) netif_start_queue(a)
#else
#define netif_tx_start_all_queues(a) do {} while (0)
#endif
#define netif_stop_subqueue(_ndev,_qi) netif_stop_queue((_ndev))
#define netif_start_subqueue(_ndev,_qi) netif_start_queue((_ndev))
#endif /* HAVE_TX_MQ */
#ifndef NETIF_F_MULTI_QUEUE
#define NETIF_F_MULTI_QUEUE 0
#define netif_is_multiqueue(a) 0
#define netif_wake_subqueue(a, b)
#endif /* NETIF_F_MULTI_QUEUE */

#else /* < 2.6.27 */
#define HAVE_TX_MQ
#define HAVE_NETDEV_SELECT_QUEUE
#endif /* < 2.6.27 */

/*****************************************************************************/
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,28) )
#define pci_ioremap_bar(pdev, bar)	ioremap(pci_resource_start(pdev, bar), \
					        pci_resource_len(pdev, bar))
#define pci_wake_from_d3 _kc_pci_wake_from_d3
#define pci_prepare_to_sleep _kc_pci_prepare_to_sleep
extern int _kc_pci_wake_from_d3(struct pci_dev *dev, bool enable);
extern int _kc_pci_prepare_to_sleep(struct pci_dev *dev);
#define netdev_alloc_page(a) alloc_page(GFP_ATOMIC)
#ifndef __skb_queue_head_init
static inline void __kc_skb_queue_head_init(struct sk_buff_head *list)
{
	list->prev = list->next = (struct sk_buff *)list;
	list->qlen = 0;
}
#define __skb_queue_head_init(_q) __kc_skb_queue_head_init(_q)
#endif

#define PCI_EXP_DEVCAP2		36	/* Device Capabilities 2 */
#define PCI_EXP_DEVCTL2		40	/* Device Control 2 */

#endif /* < 2.6.28 */

/*****************************************************************************/
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,29) )

#ifndef PCI_EXP_LNKCTL_ASPMC
#define  PCI_EXP_LNKCTL_ASPMC	0x0003	/* ASPM Control */
#endif
#else /* < 2.6.29 */
#ifndef HAVE_NET_DEVICE_OPS
#define HAVE_NET_DEVICE_OPS
#endif
#endif /* < 2.6.29 */

/*****************************************************************************/
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30)
#ifndef cpumask_bits
#define nr_cpu_ids NR_CPUS
#define nr_cpumask_bits nr_cpu_ids

typedef cpumask_t cpumask_var_t[1];
#define cpumask_bits(maskp) ((unsigned long*)(maskp))
#define cpu_online_mask &(cpu_online_map)

static inline void cpumask_clear(cpumask_t *dstp)
{
        bitmap_zero(cpumask_bits(dstp), NR_CPUS);
}

static inline int cpumask_equal(const cpumask_t *src1p,
                                const cpumask_t *src2p)
{
        return bitmap_equal(cpumask_bits(src1p), cpumask_bits(src2p),
			    nr_cpumask_bits);
}

static inline void cpumask_copy(cpumask_t *dstp,
                                cpumask_t *srcp)
{
        bitmap_copy(cpumask_bits(dstp), cpumask_bits(srcp), nr_cpumask_bits);
}

static inline unsigned int cpumask_weight(const cpumask_t *srcp)
{
        return bitmap_weight(cpumask_bits(srcp), nr_cpumask_bits);
}

static inline void cpumask_set_cpu(unsigned int cpu, cpumask_t *dstp)
{
        set_bit(cpu, cpumask_bits(dstp));
}

static inline void cpumask_setall(cpumask_t *dstp)
{
        bitmap_fill(cpumask_bits(dstp), nr_cpumask_bits);
}

static inline void free_cpumask_var(cpumask_var_t mask)
{
}
#endif /* cpumask_bits */
#endif /* LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30) */

#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30) )
#define skb_record_rx_queue(a, b) do {} while (0)
#else /* < 2.6.30 */
#define HAVE_ASPM_QUIRKS
#endif /* < 2.6.30 */

/*****************************************************************************/
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24) )
static inline void phy_device_free(struct phy_device *phydev)
{
	kfree(phydev);
}

static inline void phy_device_release(struct device *dev)
{
	phy_device_free(to_phy_device(dev));
}
#endif

#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,28) )
/**
 * mdiobus_alloc - allocate a mii_bus structure
 *
 * Description: called by a bus driver to allocate an mii_bus
 * structure to fill in.
 */
static inline struct mii_bus *mdiobus_alloc(void)
{
	return kzalloc(sizeof(struct mii_bus), GFP_KERNEL);
}

/**
 * mdiobus_free - free a struct mii_bus
 * @bus: mii_bus to free
 *
 * This function frees the mii_bus.
 */
static inline void mdiobus_free(struct mii_bus *bus)
{
	kfree(bus);
}
#endif /* < 2.6.28 */

#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31) )
#define ETH_P_1588 0x88F7
#define ETH_P_FIP  0x8914
#ifndef netdev_uc_count
#define netdev_uc_count(dev) ((dev)->uc_count)
#endif
#ifndef netdev_for_each_uc_addr
#define netdev_for_each_uc_addr(uclist, dev) \
	for (uclist = dev->uc_list; uclist; uclist = uclist->next)
#endif

#else /* < 2.6.31 */
#ifndef HAVE_NETDEV_STORAGE_ADDRESS
#define HAVE_NETDEV_STORAGE_ADDRESS
#endif
#ifndef HAVE_NETDEV_HW_ADDR
#define HAVE_NETDEV_HW_ADDR
#endif
#ifndef HAVE_TRANS_START_IN_QUEUE
#define HAVE_TRANS_START_IN_QUEUE
#endif
#ifndef HAVE_INCLUDE_LINUX_MDIO_H
#define HAVE_INCLUDE_LINUX_MDIO_H
#endif
#include <linux/mdio.h>
#endif /* < 2.6.31 */

/*****************************************************************************/
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32) )
#undef netdev_tx_t
#define netdev_tx_t int
#else /* < 2.6.32 */
#include <linux/pm_runtime.h>
/* IOV bad DMA target work arounds require at least this kernel rev support */
#define HAVE_PCIE_TYPE
#endif /* < 2.6.32 */

/*****************************************************************************/
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33) )
#ifndef pci_pcie_cap
#define pci_pcie_cap(pdev) pci_find_capability(pdev, PCI_CAP_ID_EXP)
#endif
#ifndef IPV4_FLOW
#define IPV4_FLOW 0x10
#endif /* IPV4_FLOW */
#ifndef IPV6_FLOW
#define IPV6_FLOW 0x11
#endif /* IPV6_FLOW */
/* Features back-ported to RHEL6 or SLES11 SP1 after 2.6.32 */
#if ( (RHEL_RELEASE_CODE && RHEL_RELEASE_CODE >= RHEL_RELEASE_VERSION(6,0)) || \
      (SLE_VERSION_CODE && SLE_VERSION_CODE >= SLE_VERSION(11,1,0)) )
#if defined(CONFIG_FCOE) || defined(CONFIG_FCOE_MODULE)
#ifndef HAVE_NETDEV_OPS_FCOE_GETWWN
#define HAVE_NETDEV_OPS_FCOE_GETWWN
#endif
#endif /* CONFIG_FCOE || CONFIG_FCOE_MODULE */
#endif /* RHEL6 or SLES11 SP1 */
#ifndef __percpu
#define __percpu
#endif /* __percpu */
#ifndef PORT_DA
#define PORT_DA PORT_OTHER
#endif
#ifndef PORT_NONE
#define PORT_NONE PORT_OTHER
#endif

#if ((RHEL_RELEASE_CODE && \
     (RHEL_RELEASE_CODE >= RHEL_RELEASE_VERSION(6,3)) && \
     (RHEL_RELEASE_CODE < RHEL_RELEASE_VERSION(7,0))))
#if !defined(CONFIG_X86_32) && !defined(CONFIG_NEED_DMA_MAP_STATE)
#undef DEFINE_DMA_UNMAP_ADDR
#define DEFINE_DMA_UNMAP_ADDR(ADDR_NAME)	dma_addr_t ADDR_NAME
#undef DEFINE_DMA_UNMAP_LEN
#define DEFINE_DMA_UNMAP_LEN(LEN_NAME)		__u32 LEN_NAME
#undef dma_unmap_addr
#define dma_unmap_addr(PTR, ADDR_NAME)		((PTR)->ADDR_NAME)
#undef dma_unmap_addr_set
#define dma_unmap_addr_set(PTR, ADDR_NAME, VAL)	(((PTR)->ADDR_NAME) = (VAL))
#undef dma_unmap_len
#define dma_unmap_len(PTR, LEN_NAME)		((PTR)->LEN_NAME)
#undef dma_unmap_len_set
#define dma_unmap_len_set(PTR, LEN_NAME, VAL)	(((PTR)->LEN_NAME) = (VAL))
#endif /* CONFIG_X86_64 && !CONFIG_NEED_DMA_MAP_STATE */
#endif /* RHEL_RELEASE_CODE */

#ifndef __always_unused
#define __always_unused __attribute__((__unused__))
#endif
#ifndef __maybe_unused
#define __maybe_unused __attribute__((__unused__))
#endif

#if (!(RHEL_RELEASE_CODE && \
      (RHEL_RELEASE_CODE >= RHEL_RELEASE_VERSION(6,2))))
#define sk_tx_queue_get(_sk) (-1)
#define sk_tx_queue_set(_sk, _tx_queue) do {} while(0)
#endif /* !(RHEL >= 6.2) */

#if (RHEL_RELEASE_CODE && \
     (RHEL_RELEASE_CODE >= RHEL_RELEASE_VERSION(6,4)) && \
     (RHEL_RELEASE_CODE < RHEL_RELEASE_VERSION(7,0)))
#define HAVE_RHEL6_ETHTOOL_OPS_EXT_STRUCT
#define HAVE_ETHTOOL_SET_PHYS_ID
#define HAVE_ETHTOOL_GET_TS_INFO
#endif /* RHEL >= 6.4 && RHEL < 7.0 */
#endif /* < 2.6.33 */

/*****************************************************************************/
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,34) )
#ifndef ETH_FLAG_NTUPLE
#define ETH_FLAG_NTUPLE NETIF_F_NTUPLE
#endif

#ifndef netdev_mc_count
#define netdev_mc_count(dev) ((dev)->mc_count)
#endif
#ifndef netdev_mc_empty
#define netdev_mc_empty(dev) (netdev_mc_count(dev) == 0)
#endif
#ifndef netdev_for_each_mc_addr
#define netdev_for_each_mc_addr(mclist, dev) \
	for (mclist = dev->mc_list; mclist; mclist = mclist->next)
#endif
#ifndef netdev_uc_count
#define netdev_uc_count(dev) ((dev)->uc.count)
#endif
#ifndef netdev_uc_empty
#define netdev_uc_empty(dev) (netdev_uc_count(dev) == 0)
#endif
#ifndef netdev_for_each_uc_addr
#define netdev_for_each_uc_addr(ha, dev) \
	list_for_each_entry(ha, &dev->uc.list, list)
#endif
#ifndef dma_set_coherent_mask
#define dma_set_coherent_mask(dev,mask) \
	pci_set_consistent_dma_mask(to_pci_dev(dev),(mask))
#endif

#ifndef DEFINE_DMA_UNMAP_ADDR
#define DEFINE_DMA_UNMAP_ADDR DECLARE_PCI_UNMAP_ADDR
#define DEFINE_DMA_UNMAP_LEN DECLARE_PCI_UNMAP_LEN
#define dma_unmap_addr pci_unmap_addr
#define dma_unmap_addr_set pci_unmap_addr_set
#define dma_unmap_len pci_unmap_len
#define dma_unmap_len_set pci_unmap_len_set
#endif /* DEFINE_DMA_UNMAP_ADDR */

#else /* < 2.6.34 */
#define HAVE_SYSTEM_SLEEP_PM_OPS
#ifndef HAVE_SET_RX_MODE
#define HAVE_SET_RX_MODE
#endif

#endif /* < 2.6.34 */

/*****************************************************************************/
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35) )

#ifdef HAVE_TX_MQ
#include <net/sch_generic.h>
#ifndef CONFIG_NETDEVICES_MULTIQUEUE
#if (!(RHEL_RELEASE_CODE && RHEL_RELEASE_CODE >= RHEL_RELEASE_VERSION(6,0)))
void _kc_netif_set_real_num_tx_queues(struct net_device *, unsigned int);
#define netif_set_real_num_tx_queues  _kc_netif_set_real_num_tx_queues
#endif /* !(RHEL_RELEASE_CODE >= RHEL_RELEASE_VERSION(6,0)) */
#else /* CONFIG_NETDEVICES_MULTI_QUEUE */
#define netif_set_real_num_tx_queues(_netdev, _count) \
	do { \
		(_netdev)->egress_subqueue_count = _count; \
	} while (0)
#endif /* CONFIG_NETDEVICES_MULTI_QUEUE */
#else /* HAVE_TX_MQ */
static inline int netif_set_real_num_tx_queues(struct net_device *dev, unsigned int txq)
{
	return 0;
}
#define netdev_queue    net_device
#define netdev_get_tx_queue(dev, j)     (dev)
#define netif_tx_queue_stopped(dev)	netif_queue_stopped(dev)
#define netif_tx_wake_queue(dev)	netif_wake_queue(dev)
#define netif_tx_stop_queue(txq)        netif_stop_queue(txq)
#define skb_get_queue_mapping(skb)      0       
#define __netif_tx_lock(_dev, _cpu)	netif_tx_lock(_dev)
#define __netif_tx_unlock(_dev)		netif_tx_unlock(_dev)
#endif /* HAVE_TX_MQ */

#ifndef ETH_FLAG_RXHASH
#define ETH_FLAG_RXHASH (1<<28)
#endif /* ETH_FLAG_RXHASH */
#if (RHEL_RELEASE_CODE && RHEL_RELEASE_CODE >= RHEL_RELEASE_VERSION(6,0))
#define HAVE_IRQ_AFFINITY_HINT
#endif
#else /* < 2.6.35 */
#define HAVE_PM_QOS_REQUEST_LIST
#define HAVE_IRQ_AFFINITY_HINT
#endif /* < 2.6.35 */

/*****************************************************************************/
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36) )
#ifdef CONFIG_HAVE_EFFICIENT_UNALIGNED_ACCESS
#ifdef NET_IP_ALIGN
#undef NET_IP_ALIGN
#endif
#define NET_IP_ALIGN 0
#endif /* CONFIG_HAVE_EFFICIENT_UNALIGNED_ACCESS */

#ifdef NET_SKB_PAD
#undef NET_SKB_PAD
#endif

#if (L1_CACHE_BYTES > 32)
#define NET_SKB_PAD L1_CACHE_BYTES
#else
#define NET_SKB_PAD 32
#endif

static inline struct sk_buff *_kc_netdev_alloc_skb_ip_align(struct net_device *dev,
							    unsigned int length)
{
	struct sk_buff *skb;

	skb = alloc_skb(length + NET_SKB_PAD + NET_IP_ALIGN, GFP_ATOMIC);
	if (skb) {
#if (NET_IP_ALIGN + NET_SKB_PAD)
		skb_reserve(skb, NET_IP_ALIGN + NET_SKB_PAD);
#endif
		skb->dev = dev;
	}
	return skb;
}

#ifdef netdev_alloc_skb_ip_align
#undef netdev_alloc_skb_ip_align
#endif
#define netdev_alloc_skb_ip_align(n, l) _kc_netdev_alloc_skb_ip_align(n, l)

#else /* < 2.6.36 */

#define HAVE_PM_QOS_REQUEST_ACTIVE
#define HAVE_8021P_SUPPORT
#define HAVE_NDO_GET_STATS64
#endif /* < 2.6.36 */

/*****************************************************************************/
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,37) )
#ifndef netif_set_real_num_rx_queues
static inline int _kc_netif_set_real_num_rx_queues(struct net_device *dev,
						    unsigned int rxq)
{
	return 0;
}
#define netif_set_real_num_rx_queues(dev, rxq) \
	_kc_netif_set_real_num_rx_queues((dev), (rxq))
#endif

#endif /* < 2.6.37 */

/*****************************************************************************/
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,39) )
#ifndef NETIF_F_RXCSUM
#define NETIF_F_RXCSUM		(1 << 29)
#endif
#if (!(RHEL_RELEASE_CODE && RHEL_RELEASE_CODE > RHEL_RELEASE_VERSION(6,0)))
#else /* RHEL6.1 or greater */
#ifndef HAVE_MQPRIO
#define HAVE_MQPRIO
#endif /* HAVE_MQPRIO */
#endif /* !(RHEL_RELEASE_CODE > RHEL_RELEASE_VERSION(6,0)) */
#else /* < 2.6.39 */
#ifndef HAVE_MQPRIO
#define HAVE_MQPRIO
#endif
#ifndef HAVE_SETUP_TC
#define HAVE_SETUP_TC
#endif
#ifndef HAVE_NDO_SET_FEATURES
#define HAVE_NDO_SET_FEATURES
#endif
#endif /* < 2.6.39 */

/*****************************************************************************/
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(3,0,0) )
#define USE_LEGACY_PM_SUPPORT
#ifndef kfree_rcu
#define kfree_rcu(_ptr, _rcu_head) kfree(_ptr)
#endif
#endif /* < 3.0.0 */

/*****************************************************************************/
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(3,1,0) )
#ifndef __netdev_alloc_skb_ip_align
#define __netdev_alloc_skb_ip_align(d,l,_g) netdev_alloc_skb_ip_align(d,l)
#endif /* __netdev_alloc_skb_ip_align */
#endif /* < 3.1.0 */

/*****************************************************************************/
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(3,2,0) )
#ifndef skb_frag_size
#define skb_frag_size(frag)	_kc_skb_frag_size(frag)
static inline unsigned int _kc_skb_frag_size(const skb_frag_t *frag)
{
	return frag->size;
}
#endif /* skb_frag_size */

#ifndef skb_frag_size_sub
#define skb_frag_size_sub(frag, delta)	_kc_skb_frag_size_sub(frag, delta)
static inline void _kc_skb_frag_size_sub(skb_frag_t *frag, int delta)
{
	frag->size -= delta;
}
#endif /* skb_frag_size_sub */

#ifndef skb_frag_page
#define skb_frag_page(frag)	_kc_skb_frag_page(frag)
static inline struct page *_kc_skb_frag_page(const skb_frag_t *frag)
{
	return frag->page;
}
#endif /* skb_frag_page */

#ifndef skb_frag_address
#define skb_frag_address(frag)	_kc_skb_frag_address(frag)
static inline void *_kc_skb_frag_address(const skb_frag_t *frag)
{
	return page_address(skb_frag_page(frag)) + frag->page_offset;
}
#endif /* skb_frag_address */

#ifndef skb_frag_dma_map
#if ( LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0) )
#include <linux/dma-mapping.h>
#endif
#define skb_frag_dma_map(dev,frag,offset,size,dir) \
		_kc_skb_frag_dma_map(dev,frag,offset,size,dir)
static inline dma_addr_t _kc_skb_frag_dma_map(struct device *dev,
					      const skb_frag_t *frag,
					      size_t offset, size_t size,
					      enum dma_data_direction dir)
{
	return dma_map_page(dev, skb_frag_page(frag),
			    frag->page_offset + offset, size, dir);
}
#endif /* skb_frag_dma_map */

#ifndef __skb_frag_unref
#define __skb_frag_unref(frag) __kc_skb_frag_unref(frag)
static inline void __kc_skb_frag_unref(skb_frag_t *frag)
{
	put_page(skb_frag_page(frag));
}
#endif /* __skb_frag_unref */

#ifndef SPEED_UNKNOWN
#define SPEED_UNKNOWN	-1
#endif
#ifndef DUPLEX_UNKNOWN
#define DUPLEX_UNKNOWN	0xff
#endif
#if (RHEL_RELEASE_CODE >= RHEL_RELEASE_VERSION(6,3))
#ifndef HAVE_PCI_DEV_FLAGS_ASSIGNED
#define HAVE_PCI_DEV_FLAGS_ASSIGNED
#endif
#endif
#else /* < 3.2.0 */
#ifndef HAVE_PCI_DEV_FLAGS_ASSIGNED
#define HAVE_PCI_DEV_FLAGS_ASSIGNED
#define HAVE_VF_SPOOFCHK_CONFIGURE
#endif
#ifndef HAVE_SKB_L4_RXHASH
#define HAVE_SKB_L4_RXHASH
#endif
#endif /* < 3.2.0 */

/*****************************************************************************/
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(3,3,0) )
#if !(RHEL_RELEASE_CODE >= RHEL_RELEASE_VERSION(6,5))
typedef u32 netdev_features_t;
#endif
#undef PCI_EXP_TYPE_RC_EC
#define  PCI_EXP_TYPE_RC_EC	0xa	/* Root Complex Event Collector */
#ifndef CONFIG_BQL
#define netdev_tx_completed_queue(_q, _p, _b) do {} while (0)
#define netdev_completed_queue(_n, _p, _b) do {} while (0)
#define netdev_tx_sent_queue(_q, _b) do {} while (0)
#define netdev_sent_queue(_n, _b) do {} while (0)
#define netdev_tx_reset_queue(_q) do {} while (0)
#define netdev_reset_queue(_n) do {} while (0)
#endif
#else /* ! < 3.3.0 */
#define HAVE_INT_NDO_VLAN_RX_ADD_VID
#ifdef ETHTOOL_SRXNTUPLE
#undef ETHTOOL_SRXNTUPLE
#endif
#endif /* < 3.3.0 */

/*****************************************************************************/
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(3,4,0) )
#ifndef NETIF_F_RXFCS
#define NETIF_F_RXFCS	0
#endif /* NETIF_F_RXFCS */
#ifndef NETIF_F_RXALL
#define NETIF_F_RXALL	0
#endif /* NETIF_F_RXALL */

#ifndef skb_add_rx_frag
#define skb_add_rx_frag _kc_skb_add_rx_frag
extern void _kc_skb_add_rx_frag(struct sk_buff *, int, struct page *,
				int, int, unsigned int);
#endif
#ifdef NET_ADDR_RANDOM
#define eth_hw_addr_random(N) do { \
	eth_random_addr(N->dev_addr); \
	N->addr_assign_type |= NET_ADDR_RANDOM; \
	} while (0)
#else /* NET_ADDR_RANDOM */
#define eth_hw_addr_random(N) eth_random_addr(N->dev_addr)
#endif /* NET_ADDR_RANDOM */
#else /* < 3.4.0 */
#include <linux/kconfig.h>
#endif /* >= 3.4.0 */

/*****************************************************************************/
#if ( LINUX_VERSION_CODE >= KERNEL_VERSION(3,5,0) )
#define HAVE_FDB_OPS
#define HAVE_ETHTOOL_GET_TS_INFO
#undef CONFIG_AL_ETH_ALLOC_PAGE
#ifndef CONFIG_AL_ETH_ALLOC_FRAG
#define CONFIG_AL_ETH_ALLOC_FRAG
#endif
#endif /* >= 3.5.0 */

/*****************************************************************************/
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(3,6,0) )
#ifndef eth_random_addr
#define eth_random_addr _kc_eth_random_addr
static inline void _kc_eth_random_addr(u8 *addr)
{
        get_random_bytes(addr, ETH_ALEN);
        addr[0] &= 0xfe; /* clear multicast */
        addr[0] |= 0x02; /* set local assignment */
}
#endif
#endif /* < 3.6.0 */

/*****************************************************************************/
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(3,8,0) )
#ifndef PCI_EXP_LNKCTL_ASPM_L0S
#define  PCI_EXP_LNKCTL_ASPM_L0S  0x01	/* L0s Enable */
#endif
#ifndef PCI_EXP_LNKCTL_ASPM_L1
#define  PCI_EXP_LNKCTL_ASPM_L1   0x02	/* L1 Enable */
#endif
#define HAVE_CONFIG_HOTPLUG
#else /* >= 3.8.0 */

#ifndef HAVE_ENCAP_CSUM_OFFLOAD
#define HAVE_ENCAP_CSUM_OFFLOAD
#endif

#ifndef HAVE_SRIOV_CONFIGURE
#define HAVE_SRIOV_CONFIGURE
#endif
#endif /* >= 3.8.0 */

/*****************************************************************************/
#if ( LINUX_VERSION_CODE >= KERNEL_VERSION(3,9,0) )
#define HAVE_BRIDGE_FILTER
#define USE_DEFAULT_FDB_DEL_DUMP
#endif /* >= 3.9.0 */

/*****************************************************************************/
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0))
static inline int al_mod_eth_vlan_hwaccel_check_and_put(netdev_features_t *features,
						struct sk_buff *skb,
						struct vlan_ethhdr *veh) {
	if ((*features & NETIF_F_HW_VLAN_CTAG_RX) == NETIF_F_HW_VLAN_CTAG_RX &&
		veh->h_vlan_proto == htons(ETH_P_8021Q)) {
		/* copy vlan header to coresponding skb fields */
		__vlan_hwaccel_put_tag(skb, htons(ETH_P_8021Q), ntohs(veh->h_vlan_TCI));

		return 0;
	}

	return -EOPNOTSUPP;
}
#else
static inline int al_mod_eth_vlan_hwaccel_check_and_put(netdev_features_t *features,
						struct sk_buff *skb,
						struct vlan_ethhdr *veh) {
	if ((*features & NETIF_F_HW_VLAN_RX) == NETIF_F_HW_VLAN_RX &&
		veh->h_vlan_proto == htons(ETH_P_8021Q)) {
		/* copy vlan header to coresponding skb fields */
		__vlan_hwaccel_put_tag(skb, ntohs(veh->h_vlan_TCI));

		return 0;
	}

	return -EOPNOTSUPP;
}
#endif /* >= 3.10.0 */

/*****************************************************************************/
#if ((LINUX_VERSION_CODE == KERNEL_VERSION(3, 10, 0)) && \
	RHEL_RELEASE_CODE && (RHEL_RELEASE_CODE >= RHEL_RELEASE_VERSION(7, 3)))
#define HAVE_NDO_SELECT_QUEUE_ACCEL_FALLBACK
#endif /* == 3.10.0 */

/*****************************************************************************/
#if ( LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0) )
#define HAVE_VXLAN_RX_OFFLOAD
#define HAVE_NDO_GET_PHYS_PORT_ID
#endif /* >= 3.12.0 */

/*****************************************************************************/
#if ( LINUX_VERSION_CODE >= KERNEL_VERSION(3,13,0) )
#define HAVE_VXLAN_CHECKS
#if (UBUNTU_VERSION_CODE && UBUNTU_VERSION_CODE >= UBUNTU_VERSION(3,13,0,24))
#define HAVE_NDO_SELECT_QUEUE_ACCEL_FALLBACK
#else
#define HAVE_NDO_SELECT_QUEUE_ACCEL
#endif
#else
#if BITS_PER_LONG == 32 && defined(CONFIG_SMP)
# define u64_stats_init(syncp)  seqcount_init(syncp.seq)
#else
# define u64_stats_init(syncp)  do { } while (0)
#endif
#endif /* >= 3.13.0 */

/*****************************************************************************/
#if ( LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0) )
/* for ndo_dfwd_ ops add_station, del_station and _start_xmit */
#ifndef HAVE_NDO_DFWD_OPS
#define HAVE_NDO_DFWD_OPS
#endif
#define HAVE_NDO_SELECT_QUEUE_ACCEL_FALLBACK
#endif

/*****************************************************************************/
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 15, 0)) || \
	(UBUNTU_VERSION_CODE && (UBUNTU_VERSION_CODE > UBUNTU_VERSION(3, 13, 0, 24))) || \
	(SLE_VERSION_CODE && (SLE_VERSION_CODE >= SLE_VERSION(12, 0, 0))) || \
	(RHEL_RELEASE_CODE && (RHEL_RELEASE_CODE >= RHEL_RELEASE_VERSION(7, 2)))
#else
static inline bool u64_stats_fetch_retry_irq(const struct u64_stats_sync *syncp,
					     unsigned int start)
{
	return u64_stats_fetch_retry(syncp, start);
}

static inline unsigned int u64_stats_fetch_begin_irq(const struct u64_stats_sync *syncp)
{
	return u64_stats_fetch_begin(syncp);
}
#endif /* >= 3.15.0 */

/*****************************************************************************/
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(3,8,0) )
/* cpu_rmap is buggy on older version and causes dead lock */
#ifdef CONFIG_RFS_ACCEL
#undef CONFIG_RFS_ACCEL
#endif
#endif

/*****************************************************************************/
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 18, 0))
#define HAVE_NDO_GSO_CHECK
#endif

/*****************************************************************************/
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 18, 4))
/* In 3.18.4 ndo_gso_check was changed to ndo_features_check to generalize the
 * checks performed on a per-SKB basis
 */
#undef HAVE_NDO_GSO_CHECK
#define HAVE_NDO_FEATURES_CHECK
#endif

/*****************************************************************************/
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 19, 0)) || \
	(RHEL_RELEASE_CODE && (RHEL_RELEASE_CODE >= RHEL_RELEASE_VERSION(7, 2)))
#else
static inline int skb_put_padto(struct sk_buff *skb, unsigned int len)
{
	unsigned int size = skb->len;

	if (unlikely(size < len)) {
		len -= size;
		if (skb_pad(skb, len))
			return -ENOMEM;
		__skb_put(skb, len);
	}
	return 0;
}

static inline int eth_skb_pad(struct sk_buff *skb)
{
	return skb_put_padto(skb, ETH_ZLEN);
}
#endif

#ifndef DEFINE_DMA_UNMAP_ADDR
#define DEFINE_DMA_UNMAP_ADDR(ADDR_NAME) dma_addr_t ADDR_NAME
#endif

#if (RHEL_RELEASE_CODE && (RHEL_RELEASE_CODE >= RHEL_RELEASE_VERSION(7, 5)))
#define ndo_change_mtu ndo_change_mtu_rh74
#endif /* RHEL7.5 */

#endif /* _KCOMPAT_H_ */
