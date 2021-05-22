#ifndef _XT_DYN_RANDOM_H
#define _XT_DYN_RANDOM_H

enum {
	MAX_DYN_RAND_NAME_LEN = 31,
};

struct xt_dyn_rand_info {
	char name[MAX_DYN_RAND_NAME_LEN + 1];
	__u8 invert;
	void *dyn_rand __attribute__((aligned(8)));
};

#endif /* _XT_DYN_RANDOM_H */
