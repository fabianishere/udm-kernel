/*
 * linux/drivers/soc/alpine/alpine_serdes.h
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

#ifndef __ALPINE_SERDES_H__
#define __ALPINE_SERDES_H__

/* Get SerDes group regs base, to be used in relevant Alpine drivers.
 * Valid group is 0..3.
 * Returns virtual base address of the group regs base. */
void __iomem *alpine_serdes_resource_get(u32 group);

/* SerDes ETH mode */
enum alpine_serdes_eth_mode {
	ALPINE_SERDES_ETH_MODE_SGMII,
	ALPINE_SERDES_ETH_MODE_KR,
	ALPINE_SERDES_ETH_MODE_25G
};

/*
 * Set SerDes ETH mode for an entire group, unless already set
 * Valid group is 0..3.
 * Returns 0 upon success.
 */
int alpine_serdes_eth_mode_set(
	u32				group,
	enum alpine_serdes_eth_mode	mode);

/* Lock the all serdes group for using common registers */
void alpine_serdes_eth_group_lock(u32 group);

/* Try to acquire lock of serdes group for using common registers */
int alpine_serdes_eth_group_trylock(u32 group);

/* Unlock the all serdes group for using common registers */
void alpine_serdes_eth_group_unlock(u32 group);

/* Get pointer to serdes_grp_obj */
struct al_mod_serdes_grp_obj *alpine_serdes_grp_obj_get(u32 group);

#endif /* __ALPINE_SERDES_H__ */
