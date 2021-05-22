/*
 * Copyright (c) 2013, Ubiquiti Networks, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/spinlock.h>

#include <linux/netfilter/x_tables.h>
#include <linux/netfilter/xt_dyn_random.h>

#include <net/tcp.h>
#include <net/udp.h>

MODULE_LICENSE("GPL");

struct dyn_rand {
	struct list_head 	list;
	struct proc_dir_entry 	*proc;
	unsigned int 		refcnt;
	uint32_t 		prob;
	char 			name[MAX_DYN_RAND_NAME_LEN + 1];
};

static struct proc_dir_entry *proc_root;
static LIST_HEAD(proc_list);
static DEFINE_MUTEX(proc_lock);
static u32 sticky_iv = 0;

static ssize_t _read_proc(struct file *file, char __user *buf,
			  size_t size, loff_t *loff)
{
	struct dyn_rand *e = PDE_DATA(file_inode(file));
	char tmp[16];
	int len;

	if (size < 16 || *loff != 0)
		return 0;

	len = sprintf(tmp, "%u\n", e->prob);
	return simple_read_from_buffer(buf, size, loff, tmp, len);
}

static ssize_t _write_proc(struct file *file, const char __user *buf,
			   size_t count, loff_t *loff)
{
	struct dyn_rand *e = PDE_DATA(file_inode(file));
	char tmp[16];
	uint32_t p;

	if (count >= 16)
		return -EACCES;
	if (*loff != 0)
		return -EACCES;

	memset(tmp, 0, count + 1);
	if (copy_from_user(tmp, buf, count))
		return -EFAULT;

	sticky_iv = prandom_u32();

	p = simple_strtoul(tmp, NULL, 10);

	e->prob = p;
	return count;
}

static const struct file_operations fops = {
	.read    = _read_proc,
	.write   = _write_proc,
	.owner   = THIS_MODULE,
	.llseek  = default_llseek,
};

enum {
	STICKY_SADDR = (1 << 0),
	STICKY_DADDR = (1 << 1),
	STICKY_PROTO = (1 << 2),
	STICKY_SPORT = (1 << 3),
	STICKY_DPORT = (1 << 4),
};

static bool dyn_rand_mt(const struct sk_buff *skb, struct xt_action_param *par)
{
	const struct xt_dyn_rand_info *info = par->matchinfo;
	const struct dyn_rand *e = info->dyn_rand;
	uint32_t prob = (e->prob & 0xff);
	uint32_t sticky = ((e->prob >> 8) & 0xff);
	u32 hv = sticky_iv;
	u8 tproto;
	const struct iphdr *iph;
	const struct ipv6hdr *ip6h;

	if (xt_family(par) == NFPROTO_IPV4) {
		iph = ip_hdr(skb);
		tproto = iph->protocol;
	} else {
		ip6h = ipv6_hdr(skb);
		tproto = ip6h->nexthdr;
	}

	if (!sticky) {
		hv = prandom_u32();
		goto done;
	}

	if (sticky & STICKY_SADDR) {
		if (xt_family(par) == NFPROTO_IPV4)
			hv = jhash(&(iph->saddr), sizeof(iph->saddr), hv);
		else
			hv = jhash(&(ip6h->saddr), sizeof(ip6h->saddr), hv);
	}

	if (sticky & STICKY_DADDR) {
		if (xt_family(par) == NFPROTO_IPV4)
			hv = jhash(&(iph->daddr), sizeof(iph->daddr), hv);
		else
			hv = jhash(&(ip6h->daddr), sizeof(ip6h->daddr), hv);
	}

	if (sticky & STICKY_PROTO)
		hv = jhash(&(tproto), sizeof(tproto), hv);

	if (!(sticky & (STICKY_SPORT | STICKY_DPORT)))
		goto done;

	if (tproto == IPPROTO_TCP) {
		const struct tcphdr *th;
		struct tcphdr _th;

		th = skb_header_pointer(skb, par->thoff, sizeof(_th), &_th);
		if (!th)
			goto done;

		if (sticky & STICKY_SPORT)
			hv = jhash(&(th->source), sizeof(th->source), hv);

		if (sticky & STICKY_DPORT)
			hv = jhash(&(th->dest), sizeof(th->dest), hv);
	} else if (tproto == IPPROTO_UDP) {
		const struct udphdr *uh;
		struct udphdr _uh;

		uh = skb_header_pointer(skb, par->thoff, sizeof(_uh), &_uh);
		if (!uh)
			goto done;

		if (sticky & STICKY_SPORT)
			hv = jhash(&(uh->source), sizeof(uh->source), hv);

		if (sticky & STICKY_DPORT)
			hv = jhash(&(uh->dest), sizeof(uh->dest), hv);
	}

done:

	return ((prob > 0 && (hv & 0xff) <= prob)
		^ info->invert);
}

static int dyn_rand_mt_check(const struct xt_mtchk_param *par)
{
	struct xt_dyn_rand_info *info = par->matchinfo;
	struct dyn_rand *e;

	if (*(info->name) == 0 || *(info->name) == '.'
		|| memchr(info->name, 0, sizeof(info->name)) == NULL
		|| memchr(info->name, '/', sizeof(info->name)) != NULL)
		return -EINVAL;

	mutex_lock(&proc_lock);
	list_for_each_entry(e, &proc_list, list) {
		if (strcmp(info->name, e->name) == 0) {
			e->refcnt++;
			mutex_unlock(&proc_lock);
			info->dyn_rand = e;
			return 0;
		}
	}

	if ((e = kzalloc(sizeof(struct dyn_rand), GFP_KERNEL)) == NULL) {
		mutex_unlock(&proc_lock);
		return -EINVAL;
	}

	e->proc = proc_create_data(info->name, S_IRUSR | S_IWUSR, proc_root,
				   &fops, e);
	if (e->proc == NULL) {
		kfree(e);
		mutex_unlock(&proc_lock);
		return -EINVAL;
	}

	strcpy(e->name, info->name);
	e->refcnt = 1;
	e->prob = 0;
	list_add(&(e->list), &proc_list);
	mutex_unlock(&proc_lock);
	info->dyn_rand = e;
	return 0;
}

static void dyn_rand_mt_destroy(const struct xt_mtdtor_param *par)
{
	const struct xt_dyn_rand_info *info = par->matchinfo;
	struct dyn_rand *e = info->dyn_rand;

	mutex_lock(&proc_lock);
	if (--e->refcnt == 0) {
		list_del(&(e->list));
		remove_proc_entry(e->name, proc_root);
		mutex_unlock(&proc_lock);
		kfree(e);
		return;
	}
	mutex_unlock(&proc_lock);
}

static struct xt_match xt_dyn_rand_reg __read_mostly = {
	.name       = "dyn_random",
	.revision   = 0,
	.family     = NFPROTO_UNSPEC,
	.match      = dyn_rand_mt,
	.checkentry = dyn_rand_mt_check,
	.destroy    = dyn_rand_mt_destroy,
	.matchsize  = sizeof(struct xt_dyn_rand_info),
	.me         = THIS_MODULE,
};

static const char * const proc_root_name = "xt_dyn_random";

static int __init dyn_rand_init(void)
{
	int ret;

	mutex_init(&proc_lock);
	proc_root = proc_mkdir(proc_root_name, init_net.proc_net);
	if (proc_root == NULL)
		return -ENOMEM;

	if ((ret = xt_register_match(&xt_dyn_rand_reg)) < 0) {
		remove_proc_entry(proc_root_name, init_net.proc_net);
		return ret;
	}

	return 0;
}

static void __exit dyn_rand_exit(void)
{
	xt_unregister_match(&xt_dyn_rand_reg);
	remove_proc_entry(proc_root_name, init_net.proc_net);
}

module_init(dyn_rand_init);
module_exit(dyn_rand_exit);
