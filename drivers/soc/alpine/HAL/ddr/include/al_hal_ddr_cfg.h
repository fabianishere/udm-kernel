/*******************************************************************************
Copyright (C) 2015 Annapurna Labs Ltd.

This file may be licensed under the terms of the Annapurna Labs Commercial
License Agreement.

Alternatively, this file can be distributed under the terms of the GNU General
Public License V2 as published by the Free Software Foundation and can be
found at http://www.gnu.org/licenses/gpl-2.0.html

Alternatively, redistribution and use in source and binary forms, with or
without modification, are permitted provided that the following conditions are
met:

    *     Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

    *     Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in
the documentation and/or other materials provided with the
distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/
#ifndef __AL_HAL_DDR_CFG_H__
#define __AL_HAL_DDR_CFG_H__

#ifdef __cplusplus
extern "C" {
#endif

/* The number of byte lanes (including ECC) */
#define AL_DDR_PHY_NUM_BYTE_LANES		9

/* The number of ranks (max) */
#define AL_DDR_NUM_RANKS			4

/*Invalid limit value for vref*/
#define AL_DDR_HV_INVALID_VAL 0xFFFFFFFF

/* The number of ZQ segments */
#define AL_DDR_PHY_NUM_ZQ_SEGMENTS_ALPINE_V1		3
#define AL_DDR_PHY_NUM_ZQ_SEGMENTS_ALPINE_V2		2
#define AL_DDR_PHY_NUM_ZQ_SEGMENTS_ALPINE_V3		2

#if (defined(AL_DEV_ID) && ((AL_DEV_ID >= AL_DEV_ID_ALPINE_V3)))
#define AL_DDR_PHY_NUM_ZQ_SEGMENTS     AL_DDR_PHY_NUM_ZQ_SEGMENTS_ALPINE_V3
#elif (defined(AL_DEV_ID) && ((AL_DEV_ID >= AL_DEV_ID_ALPINE_V2)))
#define AL_DDR_PHY_NUM_ZQ_SEGMENTS     AL_DDR_PHY_NUM_ZQ_SEGMENTS_ALPINE_V2
#elif (!defined(AL_DDR_PHY_NUM_ZQ_SEGMENTS))
#define AL_DDR_PHY_NUM_ZQ_SEGMENTS     AL_DDR_PHY_NUM_ZQ_SEGMENTS_ALPINE_V1
#endif

#ifdef __cplusplus
}
#endif

#endif

