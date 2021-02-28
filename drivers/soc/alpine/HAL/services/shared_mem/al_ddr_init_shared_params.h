/*******************************************************************************
Copyright (C) 2015 Annapurna Labs Ltd.

This file is licensed under the terms of the Annapurna Labs' Commercial License
Agreement distributed with the file or available on the software download site.
Recipient shall use the content of this file only on semiconductor devices or
systems developed by or for Annapurna Labs.

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
/**
 * This file includes shared parameters related to stage 2 DDR init
 */
#ifndef __AL_DDR_INIT_SHARED_PARAMS_H__
#define __AL_DDR_INIT_SHARED_PARAMS_H__

#include "al_hal_plat_types.h"

/* The expected magic number for validating the shared params */
#define AL_DDR_INIT_SHARED_PARAMS_MN	0x31415926

struct al_ddr_init_shared_params {
	uint32_t magic_num; /* Magic number for validating the shared params */
	uint8_t	ddr_num_init_retries; /* Number of DDR init retries */
	uint8_t reserved[3];
	uint64_t ddr_size; /* DDR total size in bytes */
};

#endif
