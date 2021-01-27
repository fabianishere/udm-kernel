/*******************************************************************************

  Intel 10 Gigabit PCI Express Linux driver
  Copyright(c) 1999 - 2013 Intel Corporation.

  This program is free software; you can redistribute it and/or modify it
  under the terms and conditions of the GNU General Public License,
  version 2, as published by the Free Software Foundation.

  This program is distributed in the hope it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

  The full GNU General Public License is included in this distribution in
  the file called "COPYING".

  Contact Information:
  e1000-devel Mailing List <e1000-devel@lists.sourceforge.net>
  Intel Corporation, 5200 N.E. Elam Young Parkway, Hillsboro, OR 97124-6497

*******************************************************************************/

#include "kcompat.h"
#include "al_eth.h"

/*****************************************************************************/
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,4,13) )

/**************************************/
/* PCI DMA MAPPING */

#if defined(CONFIG_HIGHMEM)

#ifndef PCI_DRAM_OFFSET
#define PCI_DRAM_OFFSET 0
#endif

u64
_kc_pci_map_page(struct pci_dev *dev, struct page *page, unsigned long offset,
                 size_t size, int direction)
{
	return (((u64) (page - mem_map) << PAGE_SHIFT) + offset +
		PCI_DRAM_OFFSET);
}

#else /* CONFIG_HIGHMEM */

u64
_kc_pci_map_page(struct pci_dev *dev, struct page *page, unsigned long offset,
                 size_t size, int direction)
{
	return pci_map_single(dev, (void *)page_address(page) + offset, size,
			      direction);
}

#endif /* CONFIG_HIGHMEM */

void
_kc_pci_unmap_page(struct pci_dev *dev, u64 dma_addr, size_t size,
                   int direction)
{
	return pci_unmap_single(dev, dma_addr, size, direction);
}

#endif /* 2.4.13 => 2.4.3 */

/*****************************************************************************/
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,4,6) )

int
_kc_pci_set_power_state(struct pci_dev *dev, int state)
{
	return 0;
}

int
_kc_pci_enable_wake(struct pci_dev *pdev, u32 state, int enable)
{
	return 0;
}

#endif /* 2.4.6 => 2.4.3 */

/*****************************************************************************/
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19) )
int _kc_skb_pad(struct sk_buff *skb, int pad)
{
	int ntail;

        /* If the skbuff is non linear tailroom is always zero.. */
        if(!skb_cloned(skb) && skb_tailroom(skb) >= pad) {
		memset(skb->data+skb->len, 0, pad);
		return 0;
        }

	ntail = skb->data_len + pad - (skb->end - skb->tail);
	if (likely(skb_cloned(skb) || ntail > 0)) {
		if (pskb_expand_head(skb, 0, ntail, GFP_ATOMIC));
			goto free_skb;
	}

#ifdef MAX_SKB_FRAGS
	if (skb_is_nonlinear(skb) &&
	    !__pskb_pull_tail(skb, skb->data_len))
		goto free_skb;

#endif
	memset(skb->data + skb->len, 0, pad);
        return 0;

free_skb:
	kfree_skb(skb);
	return -ENOMEM;
}

#if (!(RHEL_RELEASE_CODE && RHEL_RELEASE_CODE >= RHEL_RELEASE_VERSION(5,4)))
int _kc_pci_save_state(struct pci_dev *pdev)
{
	struct adapter_struct *adapter = pci_get_drvdata(pdev);
	int size = PCI_CONFIG_SPACE_LEN, i;
	u16 pcie_cap_offset, pcie_link_status;

#if ( LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0) )
	/* no ->dev for 2.4 kernels */
	WARN_ON(pdev->dev.driver_data == NULL);
#endif
	pcie_cap_offset = pci_find_capability(pdev, PCI_CAP_ID_EXP);
	if (pcie_cap_offset) {
		if (!pci_read_config_word(pdev,
		                          pcie_cap_offset + PCIE_LINK_STATUS,
		                          &pcie_link_status))
		size = PCIE_CONFIG_SPACE_LEN;
	}
	pci_config_space_ich8lan();
#ifdef HAVE_PCI_ERS
	if (adapter->config_space == NULL)
#else
	WARN_ON(adapter->config_space != NULL);
#endif
		adapter->config_space = kmalloc(size, GFP_KERNEL);
	if (!adapter->config_space) {
		printk(KERN_ERR "Out of memory in pci_save_state\n");
		return -ENOMEM;
	}
	for (i = 0; i < (size / 4); i++)
		pci_read_config_dword(pdev, i * 4, &adapter->config_space[i]);
	return 0;
}

void _kc_pci_restore_state(struct pci_dev *pdev)
{
	struct adapter_struct *adapter = pci_get_drvdata(pdev);
	int size = PCI_CONFIG_SPACE_LEN, i;
	u16 pcie_cap_offset;
	u16 pcie_link_status;

	if (adapter->config_space != NULL) {
		pcie_cap_offset = pci_find_capability(pdev, PCI_CAP_ID_EXP);
		if (pcie_cap_offset &&
		    !pci_read_config_word(pdev,
		                          pcie_cap_offset + PCIE_LINK_STATUS,
		                          &pcie_link_status))
			size = PCIE_CONFIG_SPACE_LEN;

		pci_config_space_ich8lan();
		for (i = 0; i < (size / 4); i++)
		pci_write_config_dword(pdev, i * 4, adapter->config_space[i]);
#ifndef HAVE_PCI_ERS
		kfree(adapter->config_space);
		adapter->config_space = NULL;
#endif
	}
}
#endif /* !(RHEL_RELEASE_CODE >= RHEL 5.4) */

#ifdef HAVE_PCI_ERS
void _kc_free_netdev(struct net_device *netdev)
{
	struct adapter_struct *adapter = netdev_priv(netdev);

	if (adapter->config_space != NULL)
		kfree(adapter->config_space);
#ifdef CONFIG_SYSFS
	if (netdev->reg_state == NETREG_UNINITIALIZED) {
		kfree((char *)netdev - netdev->padded);
	} else {
		BUG_ON(netdev->reg_state != NETREG_UNREGISTERED);
		netdev->reg_state = NETREG_RELEASED;
		class_device_put(&netdev->class_dev);
	}
#else
	kfree((char *)netdev - netdev->padded);
#endif
}
#endif

#endif /* <= 2.6.19 */

/*****************************************************************************/
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,21) )
struct pci_dev *_kc_netdev_to_pdev(struct net_device *netdev)
{
	return ((struct adapter_struct *)netdev_priv(netdev))->pdev;
}
#endif /* < 2.6.21 */

/*****************************************************************************/
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24) )
#ifdef NAPI
#ifdef adapter_q_vector
struct net_device *napi_to_poll_dev(const struct napi_struct *napi)
{
	struct adapter_q_vector *q_vector = container_of(napi,
							 struct adapter_q_vector,
							 napi);
	return &q_vector->poll_dev;
}
#endif

/*****************************************************************************/
int __kc_adapter_clean(struct net_device *netdev, int *budget)
{
	int work_done;
	int work_to_do = min(*budget, netdev->quota);
	struct napi_struct *napi = netdev->priv;
	work_done = napi->poll(napi, work_to_do);
	*budget -= work_done;
	netdev->quota -= work_done;
	return (work_done >= work_to_do) ? 1 : 0;
}
#endif /* NAPI */
#endif /* <= 2.6.24 */

/*****************************************************************************/
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26) )
void _kc_pci_disable_link_state(struct pci_dev *pdev, int state)
{
	struct pci_dev *parent = pdev->bus->self;
	u16 link_state;
	int pos;

	if (!parent)
		return;

	pos = pci_find_capability(parent, PCI_CAP_ID_EXP);
	if (pos) {
		pci_read_config_word(parent, pos + PCI_EXP_LNKCTL, &link_state);
		link_state &= ~state;
		pci_write_config_word(parent, pos + PCI_EXP_LNKCTL, link_state);
	}
}
#endif /* < 2.6.26 */

/*****************************************************************************/
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27) )
#ifdef HAVE_TX_MQ
void _kc_netif_tx_stop_all_queues(struct net_device *netdev)
{
	struct adapter_struct *adapter = netdev_priv(netdev);
	int i;

	netif_stop_queue(netdev);
	if (netif_is_multiqueue(netdev))
		for (i = 0; i < adapter->num_tx_queues; i++)
			netif_stop_subqueue(netdev, i);
}
void _kc_netif_tx_wake_all_queues(struct net_device *netdev)
{
	struct adapter_struct *adapter = netdev_priv(netdev);
	int i;

	netif_wake_queue(netdev);
	if (netif_is_multiqueue(netdev))
		for (i = 0; i < adapter->num_tx_queues; i++)
			netif_wake_subqueue(netdev, i);
}
void _kc_netif_tx_start_all_queues(struct net_device *netdev)
{
	struct adapter_struct *adapter = netdev_priv(netdev);
	int i;

	netif_start_queue(netdev);
	if (netif_is_multiqueue(netdev))
		for (i = 0; i < adapter->num_tx_queues; i++)
			netif_start_subqueue(netdev, i);
}
#endif /* HAVE_TX_MQ */
#endif /* < 2.6.27 */

/*****************************************************************************/
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,28) )

int
_kc_pci_prepare_to_sleep(struct pci_dev *dev)
{
	pci_power_t target_state;
	int error;

	target_state = pci_choose_state(dev, PMSG_SUSPEND);

	pci_enable_wake(dev, target_state, true);

	error = pci_set_power_state(dev, target_state);

	if (error)
		pci_enable_wake(dev, target_state, false);

	return error;
}

int
_kc_pci_wake_from_d3(struct pci_dev *dev, bool enable)
{
	int err;

	err = pci_enable_wake(dev, PCI_D3cold, enable);
	if (err)
		goto out;

	err = pci_enable_wake(dev, PCI_D3hot, enable);

out:
	return err;
}
#endif /* < 2.6.28 */

/******************************************************************************/
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(3,4,0) )
void _kc_skb_add_rx_frag(struct sk_buff *skb, int i, struct page *page,
			 int off, int size, unsigned int truesize)
{
	skb_fill_page_desc(skb, i, page, off, size);
	skb->len += size;
	skb->data_len += size;
	skb->truesize += truesize;
}

#endif /* < 3.4.0 */
