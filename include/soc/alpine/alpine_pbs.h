#ifndef __ALPINE_PBS_H__
#define __ALPINE_PBS_H__

/**
 * Get the Peripheral Bus System base register address
 *
 * @returns	Peripheral Bus System register base address
 */
void __iomem *alpine_pbs_regs_base_get(void);

#endif /* __ALPINE_PBS_H__ */
