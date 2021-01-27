/*
 * Copyright 2018, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#ifndef __AL_CRC_H__
#define __AL_CRC_H__

#include "al_hal_types.h"

enum al_crc8_polynom {

	/* CRC8N = (0 1 2 5 6 7 8) */
	AL_CRC8_POL__8N
};

/**
 * Generic 8 bit CRC calculation function.
 *
 * @param       initial_crc
 *              the crc value of previous block
 * @param       data
 *              the data to calculate its crc
 *              if set to NULL, the 'initial_crc' is returned
 * @param       size
 *              number of bytes in 'data' buffer
 *              if set to 0, the 'initial_crc' is returned
 * @param       polynom
 *              the polynom to use for CRC calculation
 * @returns     crc result value
 */
uint8_t al_crc8(uint8_t initial_crc,
		const uint8_t *data,
		uint32_t size,
		enum al_crc8_polynom polynom);

#endif /* __AL_CRC_H__ */
