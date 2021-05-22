/* SPDX-License-Identifier: GPL-2.0 */
/*
 *
 */

#ifndef _LINUX_RESVMEM_REBOOT_REASON_H
#define _LINUX_RESVMEM_REBOOT_REASON_H

#include <linux/kernel.h>
#include <linux/platform_device.h>

#define RESVMEM_REBOOT_TYPES(TYPE)      \
    TYPE(cold boot, COLD_BOOT)          \
    TYPE(kernel panic, KERNEL_PANIC)    \
    TYPE(watchdog, WATCHDOG)            \
    TYPE(user manual, USER_MANUAL)

enum {
#define TYPE(_str, _enum) RESVMEM_REBOOT_##_enum,
    RESVMEM_REBOOT_TYPES(TYPE)
#undef TYPE
    RESVMEM_REBOOT_COUNT
};

struct resvmem_reboot {
	u32 type;
};
    
struct resvmem_dev {
	struct device *dev;
	struct resvmem_reboot *reason;
	u32	mem_base;
	u32	mem_size;
};

extern void resvmem_set_reboot_reason(u32 reboot_reason);
#endif
