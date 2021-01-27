/*
 *  linux/arch/arm/kernel/sys_arm.c
 *
 *  Copyright (C) People who wrote linux/arch/i386/kernel/sys_i386.c
 *  Copyright (C) 1995, 1996 Russell King.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *  This file contains various random system calls that
 *  have a non-standard calling sequence on the Linux/arm
 *  platform.
 */
#include <linux/export.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/sem.h>
#include <linux/msg.h>
#include <linux/shm.h>
#include <linux/stat.h>
#include <linux/syscalls.h>
#include <linux/mman.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/ipc.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/printk.h>

/*
 * Since loff_t is a 64 bit type we avoid a lot of ABI hassle
 * with a different argument ordering.
 */
asmlinkage long sys_arm_fadvise64_64(int fd, int advice,
				     loff_t offset, loff_t len)
{
	return sys_fadvise64_64(fd, offset, len, advice);
}

#if (PAGE_SHIFT > 12)
	/*
	 * the "offeset" input variable is in always in 4k units for mmap2.
	 * If PAGE_SIZE is different, we need shift it to present real pages,
	 * and to make sure that the actual address is PAGE_SIZE aligned
	 */
asmlinkage unsigned long sys_arm_mmap_4koff(unsigned long addr,
		unsigned long len, unsigned long prot, unsigned long flags,
		unsigned long fd, unsigned long offset)
{
	unsigned long pgoff;
	if (offset & ((PAGE_SIZE-1)>>12)) {
		printk(KERN_WARNING
				"mmap received unaligned request offset: %x.",
				(unsigned int)offset);
		return -EINVAL;
	}
	pgoff = offset >> (PAGE_SHIFT - 12);

	return sys_mmap_pgoff(addr, len, prot, flags, fd, pgoff);
}
#endif
