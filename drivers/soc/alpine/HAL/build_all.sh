#!/bin/bash

set -e

echo "Building without specific architecture..."
make clean
make $*

echo "Building Alpine V1..."
make AL_DEV_ID=AL_DEV_ID_ALPINE_V1 clean
make AL_DEV_ID=AL_DEV_ID_ALPINE_V1 AL_HAL_EX=1 $*

echo "Building Alpine V2..."
make AL_DEV_ID=AL_DEV_ID_ALPINE_V2 clean
make AL_DEV_ID=AL_DEV_ID_ALPINE_V2 $*

echo "Building Alpine V3..."
make AL_DEV_ID=AL_DEV_ID_ALPINE_V3 AL_DEV_REV_ID=1 clean
make AL_DEV_ID=AL_DEV_ID_ALPINE_V3 AL_DEV_REV_ID=1 $*

echo "Building Alpine V2 proprietary..."
make AL_DEV_ID=AL_DEV_ID_ALPINE_V2 clean
make AL_DEV_ID=AL_DEV_ID_ALPINE_V2 AL_HAL_EX=1 AL_HAL_FP_EX=1 $*

echo "Building Alpine V3 proprietary..."
make AL_DEV_ID=AL_DEV_ID_ALPINE_V3 AL_DEV_REV_ID=1 clean
make AL_DEV_ID=AL_DEV_ID_ALPINE_V3 AL_DEV_REV_ID=1 AL_HAL_EX=1 $*
