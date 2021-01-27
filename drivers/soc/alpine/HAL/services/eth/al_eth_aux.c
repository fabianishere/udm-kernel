/*
 * Copyright 2018, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#include "al_hal_common.h"
#include "al_hal_eth.h"
#include "al_eth_aux.h"

int al_eth_aux_eth_adv_idx_get(unsigned int devno, unsigned int rev_id)
{
	if (rev_id >= AL_ETH_REV_ID_4) {
		switch (devno) {
		case 0:
			return 0;
		case 2:
			return 1;
		case 4:
			return 2;
		case 5:
			return 3;
		default:
			al_err("%s: Unknown devno %d, eth rev %d\n", __func__, devno, rev_id);
		}
	} else {
		al_err("%s: unsupported rev_id %d\n", __func__, rev_id);
	}

	return -EINVAL;
}

int al_eth_aux_eth_devno_get(unsigned int eth_adv_idx, unsigned int rev_id)
{
	if (rev_id >= AL_ETH_REV_ID_4) {
		switch (eth_adv_idx) {
		case 0:
			return 0;
		case 1:
			return 2;
		case 2:
			return 4;
		case 3:
			return 5;
		default:
			al_err("%s: Unknown eth_idx %d, eth rev %d\n", __func__, eth_adv_idx, rev_id);
		}
	} else {
		al_err("%s: unsupported rev_id %d\n", __func__, rev_id);
	}

	return -EINVAL;
}
