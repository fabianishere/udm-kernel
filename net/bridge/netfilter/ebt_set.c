/* Copyright (C) 2018-2019 Keh-Ming Luoh <kmluoh@ubnt.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

/* Kernel module which implements the set match for ebtables.
 */

#include <linux/module.h>
#include <linux/netfilter/x_tables.h>
#include <linux/netfilter_bridge/ebtables.h>
#include <linux/netfilter/ipset/ip_set.h>

struct ebt_set_info {
	ip_set_id_t index;
	__u8 dim;
	__u8 flags;
	__u8 family;
};

static inline int
match_set(ip_set_id_t index, const struct sk_buff *skb,
	  const struct xt_action_param *par,
	  struct ip_set_adt_opt *opt, int inv)
{
	if (ip_set_test(index, skb, par, opt))
		inv = !inv;
	return inv;
}

#define ADT_OPT(n, f, d, fs, cfs, t)	\
struct ip_set_adt_opt n = {		\
	.family	= f,			\
	.dim = d,			\
	.flags = fs,			\
	.cmdflags = cfs,		\
	.ext.timeout = t,		\
}

static bool
ebt_set_mt(const struct sk_buff *skb, struct xt_action_param *par)
{
	const struct ebt_set_info *setinfo = par->matchinfo;

	pr_debug("ebt_set_mt: par->family = 0x%02hhx\n", xt_family(par));
	ADT_OPT(opt, setinfo->family, setinfo->dim,
		setinfo->flags, 0, UINT_MAX);

	return match_set(setinfo->index, skb, par, &opt,
			 setinfo->flags & IPSET_INV_MATCH);
}

static int
ebt_set_mt_check(const struct xt_mtchk_param *par)
{
	struct ebt_set_info *setinfo = par->matchinfo;
	ip_set_id_t index;

	index = ip_set_nfnl_get_byindex(par->net, setinfo->index);

	if (index == IPSET_INVALID_ID) {
		pr_warn("Cannot find set identified by id %u to match\n",
			setinfo->index);
		return -ENOENT;
	}
	if (setinfo->dim > IPSET_DIM_MAX) {
		pr_warn("Protocol error: set match dimension is over the limit!\n");
		ip_set_nfnl_put(par->net, setinfo->index);
		return -ERANGE;
	}

	return 0;
}

static void
ebt_set_mt_destroy(const struct xt_mtdtor_param *par)
{
       struct ebt_set_info *setinfo = par->matchinfo;

       ip_set_nfnl_put(par->net, setinfo->index);
}

static struct xt_match ebt_set_mt_reg __read_mostly = {
        .name = "set",
        .revision = 0,
        .family = NFPROTO_BRIDGE,
        .match = ebt_set_mt,
        .checkentry = ebt_set_mt_check,
        .matchsize = sizeof(struct ebt_set_info),
		.destroy = ebt_set_mt_destroy,
        .me = THIS_MODULE,
};

static int __init ebt_set_init(void)
{
        return xt_register_match(&ebt_set_mt_reg);
}

static void __exit ebt_set_fini(void)
{
        xt_unregister_match(&ebt_set_mt_reg);
}

module_init(ebt_set_init);
module_exit(ebt_set_fini);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Keh-Ming Luoh <kmluoh@ubnt.com>");
MODULE_DESCRIPTION("ebtables: ipset match module");
MODULE_ALIAS("ebt_set");
