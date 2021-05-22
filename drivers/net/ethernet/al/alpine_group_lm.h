/*
 * linux/drivers/soc/alpine/alpine_group_lm.h
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

#ifndef __ALPINE_GROUP_LM_H__
#define __ALPINE_GROUP_LM_H__

/* Return a pointer to group_lm_context of the serdes group*/
struct al_mod_eth_group_lm_context *alpine_group_lm_get(u32 group);

#endif /* __ALPINE_GROUP_LM_H__ */
