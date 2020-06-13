#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt
#include <linux/version.h>
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <net/ip.h>
#include <net/tcp.h>

#include <linux/netfilter/x_tables.h>
#include "xt_wscale.h"

MODULE_AUTHOR("llccd <me@llccd.eu.org>");
MODULE_DESCRIPTION("Xtables: TCP wscale match");
MODULE_LICENSE("GPL");

static bool wscale_mt(const struct sk_buff *skb, struct xt_action_param *par)
{
	const struct ipt_wscale_info *info = par->matchinfo;
	const struct tcphdr *th;
	struct tcphdr _tcph;
	/* tcp.doff is only 4 bits, ie. max 15 * 4 bytes */
	const u_int8_t *op;
	u8 _opt[15 * 4 - sizeof(_tcph)], wscale = 0;
	unsigned int i, optlen;

	th = skb_header_pointer(skb, par->thoff, sizeof(_tcph), &_tcph);

	/* Malformed. */
	if (th->doff*4 < sizeof(*th))
		goto dropit;

	optlen = th->doff*4 - sizeof(*th);
	if (!optlen)
		goto out;

	/* Truncated options. */
	op = skb_header_pointer(skb, par->thoff + sizeof(*th), optlen, _opt);
	if (op == NULL)
		goto dropit;

	for (i = 0; i < optlen; ) {
		if (op[i] == TCPOPT_WINDOW
		    && (optlen - i) >= TCPOLEN_WINDOW
		    && op[i+1] == TCPOLEN_WINDOW) {
			wscale = op[i+2];
			break;
		}
		if (op[i] < 2)
			i++;
		else
			i += op[i+1] ? : 1;
	}
out:
	switch (info->mode) {
	case IPT_WSCALE_EQ:
		return wscale == info->wscale;
	case IPT_WSCALE_NE:
		return wscale != info->wscale;
	case IPT_WSCALE_LT:
		return wscale < info->wscale;
	case IPT_WSCALE_GT:
		return wscale > info->wscale;
	}

dropit:
	par->hotdrop = true;
	return false;
}

static struct xt_match wscale_mt_regs[] __read_mostly = {
	{
		.name       = "wscale",
		.revision   = 0,
		.family     = NFPROTO_IPV4,
		.match      = wscale_mt,
		.matchsize  = sizeof(struct ipt_wscale_info),
		.proto      = IPPROTO_TCP,
		.me         = THIS_MODULE,
	},
#if IS_ENABLED(CONFIG_IP6_NF_IPTABLES)
	{
		.name       = "wscale",
		.revision   = 0,
		.family     = NFPROTO_IPV6,
		.match      = wscale_mt,
		.matchsize  = sizeof(struct ipt_wscale_info),
		.proto      = IPPROTO_TCP,
		.me         = THIS_MODULE,
	},
#endif
};

static int __init wscale_init(void)
{
	return xt_register_matches(wscale_mt_regs, ARRAY_SIZE(wscale_mt_regs));
}

static void __exit wscale_exit(void)
{
	xt_unregister_matches(wscale_mt_regs, ARRAY_SIZE(wscale_mt_regs));
}

module_init(wscale_init);
module_exit(wscale_exit);
MODULE_ALIAS("ipt_wscale");
#if IS_ENABLED(CONFIG_IP6_NF_IPTABLES)
MODULE_ALIAS("ip6t_wscale");
#endif
