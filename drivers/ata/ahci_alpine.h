#ifndef _AHCI_ALPINE_H
#define _AHCI_ALPINE_H

#ifdef CONFIG_AHCI_ALPINE

extern int al_init_msix_interrupts(struct pci_dev *pdev,
		unsigned int n_ports, struct ahci_host_priv *hpriv);

extern void alpine_clean_cause(struct ata_port *ap_this);

extern int al_port_irq(struct ata_host *host, int port);

extern bool al_ahci_sss_wa_needed(struct device *dev);

extern int al_sata_link_hardreset(struct ata_link *link,
		const unsigned long *timing, unsigned long deadline);

#else

static inline int al_init_msix_interrupts(struct pci_dev *pdev,
	unsigned int n_ports, struct ahci_host_priv *hpriv)
{ return 0; }

static inline void alpine_clean_cause(struct ata_port *ap_this)
{};

static inline int al_port_irq(struct ata_host *host, int port)
{ return -EINVAL; };

static inline bool al_ahci_sss_wa_needed(struct device *dev)
{ return FALSE; };

static inline int al_sata_link_hardreset(struct ata_link *link,
		const unsigned long *timing, unsigned long deadline)
{ return -EINVAL; };
#endif

#endif /* _AHCI_ALPINE_H */
