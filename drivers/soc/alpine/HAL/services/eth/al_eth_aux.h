/*
 * Copyright 2018, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#ifndef __AL_ETH_AUX_H__
#define __AL_ETH_AUX_H__

/**
 * Convert eth device number to eth adv index
 *
 * @param devno device number
 * @param rev_id revision ID
 *
 * @return
 *	eth index on success, errno otherwise
 */
int al_eth_aux_eth_adv_idx_get(unsigned int devno, unsigned int rev_id);

/**
 * Convert eth adv idx to eth adv device number
 *
 * @param eth_adv_idx eth port index
 * @param rev_id revision ID
 *
 * @return
 *	eth index on success, errno otherwise
 */
int al_eth_aux_eth_devno_get(unsigned int eth_adv_idx, unsigned int rev_id);

#endif /* __AL_ETH_AUX_H__ */
