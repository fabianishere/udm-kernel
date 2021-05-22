#include <net/netfilter/nf_conntrack_extend.h>
#include <net/netfilter/nf_conntrack_dpi.h>

static void nf_ct_ext_dpi_destroy(struct nf_conn *ct)
{
	struct nf_conn_dpi *conn_dpi = nfct_dpi(ct);
	if (conn_dpi) {
		if (conn_dpi->dns) {
			kfree(conn_dpi->dns);
		}
		if (conn_dpi->domain_name) {
			kfree(conn_dpi->domain_name);
		}
	}
}

static struct nf_ct_ext_type dpi_extend __read_mostly = {
	.len    = sizeof(struct nf_conn_dpi),
	.align  = __alignof__(struct nf_conn_dpi),
	.destroy = nf_ct_ext_dpi_destroy,
	.id     = NF_CT_EXT_DPI,
};

int nf_conntrack_dpi_init(void)
{
	return nf_ct_extend_register(&dpi_extend);
}

void nf_conntrack_dpi_fini(void)
{
	nf_ct_extend_unregister(&dpi_extend);
}
