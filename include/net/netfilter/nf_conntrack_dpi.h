#ifndef _NF_CONNTRACK_DPI_H
#define _NF_CONNTRACK_DPI_H
#include <net/net_namespace.h>
#include <linux/netfilter/nf_conntrack_common.h>
#include <linux/netfilter/nf_conntrack_tuple_common.h>
#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_extend.h>
#include <linux/types.h>

struct nf_conn_dpi {
	uint8_t dpi_calls;
	uint8_t ubnt_proto;
	uint8_t dns_len;
	uint8_t domain_name_len;
	uint16_t flags;
	char *dns; // domain name set by dns
	char *domain_name; // tls/http host header
};

static inline struct nf_conn_dpi *nfct_dpi(const struct nf_conn *ct)
{
#ifdef CONFIG_NF_CONNTRACK_DPI
	return nf_ct_ext_find(ct, NF_CT_EXT_DPI);
#else
	return NULL;
#endif
}

static inline struct nf_conn_dpi *nf_ct_dpi_ext_add(struct nf_conn *ct)
{
#ifdef CONFIG_NF_CONNTRACK_DPI
	return nf_ct_ext_add(ct, NF_CT_EXT_DPI, GFP_ATOMIC);
#else
	return NULL;
#endif
}

#ifdef CONFIG_NF_CONNTRACK_DPI
int nf_conntrack_dpi_init(void);
void nf_conntrack_dpi_fini(void);
#else
static inline int nf_conntrack_dpi_init(void) { return 0; }
static inline void nf_conntrack_dpi_fini(void) {}
#endif

#endif /* _NF_CONNTRACK_DPI_H */
