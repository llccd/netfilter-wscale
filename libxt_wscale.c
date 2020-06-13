#include <stdio.h>
#include <xtables.h>
#include "xt_wscale.h"

enum {
	O_WSCALE_EQ = 0,
	O_WSCALE_LT,
	O_WSCALE_GT,
	F_WSCALE_EQ = 1 << O_WSCALE_EQ,
	F_WSCALE_LT = 1 << O_WSCALE_LT,
	F_WSCALE_GT = 1 << O_WSCALE_GT,
	F_ANY       = F_WSCALE_EQ | F_WSCALE_LT | F_WSCALE_GT,
};

static void wscale_help(void)
{
	printf(
"wscale match options:\n"
"[!] --eq value	Match wscale value\n"
"  --lt value	Match wscale < value\n"
"  --gt value	Match wscale > value\n");
}

static void wscale_parse(struct xt_option_call *cb)
{
	struct ipt_wscale_info *info = cb->data;

	xtables_option_parse(cb);
	switch (cb->entry->id) {
	case O_WSCALE_EQ:
		info->mode = cb->invert ? IPT_WSCALE_NE : IPT_WSCALE_EQ;
		break;
	case O_WSCALE_LT:
		info->mode = IPT_WSCALE_LT;
		break;
	case O_WSCALE_GT:
		info->mode = IPT_WSCALE_GT;
		break;
	}
}

static void wscale_check(struct xt_fcheck_call *cb)
{
	if (!(cb->xflags & F_ANY))
		xtables_error(PARAMETER_PROBLEM,
			"wscale match: You must specify one of "
			"`--eq', `--lt', `--gt");
}

static void wscale_print(const void *ip, const struct xt_entry_match *match, int numeric)
{
	const struct ipt_wscale_info *info = (struct ipt_wscale_info *) match->data;

	printf(" wscale match ");
	switch (info->mode) {
		case IPT_WSCALE_EQ:
			printf(" ==");
			break;
		case IPT_WSCALE_NE:
			printf(" !=");
			break;
		case IPT_WSCALE_LT:
			printf(" <");
			break;
		case IPT_WSCALE_GT:
			printf(" >");
			break;
	}
	printf(" %u", info->wscale);
}

static void wscale_save(const void *ip, const struct xt_entry_match *match)
{
	const struct ipt_wscale_info *info = (struct ipt_wscale_info *) match->data;

	switch (info->mode) {
		case IPT_WSCALE_EQ:
			printf(" --eq");
			break;
		case IPT_WSCALE_NE:
			printf(" ! --eq");
			break;
		case IPT_WSCALE_LT:
			printf(" --lt");
			break;
		case IPT_WSCALE_GT:
			printf(" --gt");
			break;
		default:
			/* error */
			break;
	}
	printf(" %u", info->wscale);
}

static int wscale_xlate(struct xt_xlate *xl,
		     const struct xt_xlate_mt_params *params)
{
	const struct ipt_wscale_info *info = (struct ipt_wscale_info *) params->match->data;

		switch (info->mode) {
		case IPT_WSCALE_EQ:
			xt_xlate_add(xl, "wscale");
			break;
		case IPT_WSCALE_NE:
			xt_xlate_add(xl, "wscale !=");
			break;
		case IPT_WSCALE_LT:
			xt_xlate_add(xl, "wscale lt");
			break;
		case IPT_WSCALE_GT:
			xt_xlate_add(xl, "wscale gt");
			break;
		default:
			/* Should not happen. */
			break;
	}

	xt_xlate_add(xl, " %u", info->wscale);

	return 1;
}

static const struct xt_option_entry wscale_opts[] = {
	{.name = "lt", .id = O_WSCALE_LT, .excl = F_ANY, .type = XTTYPE_UINT8,
	 .flags = XTOPT_PUT, XTOPT_POINTER(struct ipt_wscale_info, wscale)},
	{.name = "gt", .id = O_WSCALE_GT, .excl = F_ANY, .type = XTTYPE_UINT8,
	 .flags = XTOPT_PUT, XTOPT_POINTER(struct ipt_wscale_info, wscale)},
	{.name = "eq", .id = O_WSCALE_EQ, .excl = F_ANY, .type = XTTYPE_UINT8,
	 .flags = XTOPT_INVERT | XTOPT_PUT, XTOPT_POINTER(struct ipt_wscale_info, wscale)},
	XTOPT_TABLEEND,
};

static struct xtables_match wscale_mt_reg = {
	.name		= "wscale",
	.version	= XTABLES_VERSION,
	.family		= NFPROTO_UNSPEC,
	.size		= XT_ALIGN(sizeof(struct ipt_wscale_info)),
	.userspacesize	= XT_ALIGN(sizeof(struct ipt_wscale_info)),
	.help		= wscale_help,
	.print		= wscale_print,
	.save		= wscale_save,
	.x6_parse	= wscale_parse,
	.x6_fcheck	= wscale_check,
	.x6_options	= wscale_opts,
	.xlate		= wscale_xlate,
};


void _init(void)
{
	xtables_register_match(&wscale_mt_reg);
}
