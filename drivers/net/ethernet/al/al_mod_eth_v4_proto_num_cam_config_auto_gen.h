/*
 * Copyright 2016, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#ifndef __AL_ETH_V4_PROTO_NUM_CAM_CONFIG_AUTO_GEN_H__
#define __AL_ETH_V4_PROTO_NUM_CAM_CONFIG_AUTO_GEN_H__

/** Protocol numbers */
enum al_mod_eth_proto_num {
	/** IP v4 */
	PROTO_NUM_IP_V4 = 14,
	/** IP v4 / TCP or UDP */
	PROTO_NUM_IP_V4_TCP_OR_UDP = 15,
	/** IP v6 */
	PROTO_NUM_IP_V6 = 16,
	/** IP v6 / TCP or UDP */
	PROTO_NUM_IP_V6_TCP_OR_UDP = 17,
	/** No protocol detected */
	PROTO_NUM_NOT_DETECTED = 63,
};

#endif /* __AL_ETH_V4_PROTO_NUM_CAM_CONFIG_AUTO_GEN_H__ */
