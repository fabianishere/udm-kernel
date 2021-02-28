/*
 * Copyright 2016, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */
#ifndef __AL_HAL_NB_REGS_GLUE_H__
#define __AL_HAL_NB_REGS_GLUE_H__

#define NB_NB_VERSION_VERSION_RELEASE_NUM_MAJOR_VAL_ALPINE_V1	2
#define NB_NB_VERSION_VERSION_RELEASE_NUM_MAJOR_VAL_ALPINE_V2	3
#define NB_NB_VERSION_VERSION_RELEASE_NUM_MAJOR_VAL_ALPINE_V3	4

#if ((defined(AL_DEV_ID) && (AL_DEV_ID <= AL_DEV_ID_ALPINE_V2)) ||\
	(defined(AL_DEV_ID) && defined(AL_DEV_REV_ID)))

#if (AL_DEV_ID <= AL_DEV_ID_ALPINE_V2)
#include "al_hal_nb_regs_v1_v2.h"
#elif (AL_DEV_ID == AL_DEV_ID_ALPINE_V3) && (AL_DEV_REV_ID == 0)
#include "al_hal_nb_regs_v3_tc.h"
#else
#include "al_hal_nb_regs_v3.h"
#endif

#endif
#endif
