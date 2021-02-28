/*
 * Annapurna labs resume address.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef AL_CPU_RESUME_H_
#define AL_CPU_RESUME_H_

struct al_cpu_resume_regs_per_cpu {
	/* Flags */
	uint32_t	flags;

	/* Resume address */
	uint32_t	resume_addr;
};

struct al_cpu_resume_regs {
	/* Watermark for validating the CPU resume struct */
	uint32_t watermark;

	/* Various flags to control the resume behavior */
	uint32_t flags;

	/* Per cpu regs */
	struct al_cpu_resume_regs_per_cpu per_cpu[];
};

/* The expected magic number for validating the resume addresses */
#define AL_CPU_RESUME_MAGIC_NUM		0xf0e1d200
#define AL_CPU_RESUME_MAGIC_NUM_MASK	0xffffff00

/* The expected minimal version number for validating the capabilities */
#define AL_CPU_RESUME_MIN_VER_MASK	0x000000ff
/*minimal version supporting resume*/
#define AL_CPU_RESUME_MIN_VER		0x000000c3
/*minimal version supporting hyp*/
#define AL_CPU_RESUME_MIN_VER_HYP	0x000000c4
/*minimal version supporting aarch64*/
#define AL_CPU_RESUME_MIN_VER_AARCH64	0x000000c5

/* General resume flags*/
#define AL_CPU_RESUME_FLG_SWITCH_TO_NS_DIS     (1 << 0)
#define AL_CPU_RESUME_FLG_SWITCH_TO_HYP		(1 << 1)
#define AL_CPU_RESUME_FLG_CLUSTER_INIT		(1 << 2)

/* Per-cpu resume flags */
/* Don't init anything outside the cluster */
#define AL_CPU_RESUME_FLG_PERCPU_EXTERNAL_SKIP	(1 << 0)
/* Don't init anything outside the core */
#define AL_CPU_RESUME_FLG_PERCPU_CLUSTER_SKIP	(1 << 1)
/* Don't resume */
#define AL_CPU_RESUME_FLG_PERCPU_DONT_RESUME	(1 << 2)
/* use aarch64 */
#define AL_CPU_RESUME_FLG_PERCPU_AARCH64	(1 << 3)

/* App-specific flags */
#define AL_CPU_RESUME_FLG_PERCPU_APP_RSVD_MASK	(0xf << 28)

#endif /* AL_CPU_RESUME_H_ */
