#!/bin/bash -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

HAL_TOP=$DIR/../

cd $HAL_TOP
$DIR/checkpatch.pl -g HEAD
