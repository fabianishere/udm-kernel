#ifndef __LINUX_BRIDGE_EBT_MARK_T_H
#define __LINUX_BRIDGE_EBT_MARK_T_H

/* The target member is reused for adding new actions, the
 * value of the real target is -1 to -NUM_STANDARD_TARGETS.
 * For backward compatibility, the 4 lsb (2 would be enough,
 * but let's play it safe) are kept to designate this target.
 * The remaining bits designate the action. By making the set
 * action 0xfffffff0, the result will look ok for older
 * versions. [September 2006] */
#define MARK_SET_VALUE (0xfffffff0)
#define MARK_OR_VALUE  (0xffffffe0)
#define MARK_AND_VALUE (0xffffffd0)
#define MARK_XOR_VALUE (0xffffffc0)
#define MARK_UBNT_SET_VALUE (0xffffffb0)
#define MARK_UBNT_OR_VALUE (0xffffffa0)
#define MARK_UBNT_AND_VALUE (0xffffff90)
#define MARK_UBNT_XOR_VALUE (0xffffff80)

struct ebt_mark_t_info {
	unsigned long mark;
	/* EBT_ACCEPT, EBT_DROP, EBT_CONTINUE or EBT_RETURN */
	int target;
};
#define EBT_MARK_TARGET "mark"

#endif
