#ifndef _XT_WSCALE_H
#define _XT_WSCALE_H

#include <linux/types.h>

enum {
	IPT_WSCALE_EQ = 0,		/* equals */
	IPT_WSCALE_NE,		/* not equals */
	IPT_WSCALE_LT,		/* less than */
	IPT_WSCALE_GT,		/* greater than */
};


struct ipt_wscale_info {
	__u8	mode;
	__u8	wscale;
};

#endif
