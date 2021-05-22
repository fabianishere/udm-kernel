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

/**
 * @brief  dummy platform services implementation
 *
 */

#include <al_hal_common.h>
#include <al_hal_iomap.h>

#include <stdio.h>
#include "al_hal_udma_regs.h"
#include "al_hal_ssm_raid_regs.h"
#include "al_hal_eth_ec_regs.h"

/*struct udma_m2s_regs regs; */
#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

static uint32_t base = 0x0;

#define print_offset_generic(STRUCT, MEMBER)\
	printf("offset of %s\t :\t0x%04zx\n", #MEMBER,	\
		offsetof(struct STRUCT, MEMBER));

#define print_offset_generic_l2(STRUCT, MEMBER)\
	printf("\t");\
	print_offset_generic(STRUCT, MEMBER);

#define udma_print_offset(MEMBER)\
	printf("offset of %s\t :\t0x%04zx\n", #MEMBER, 	\
	       (size_t)(offsetof(struct udma_m2s_regs_v3, MEMBER) + base));

#define udma_q_print_offset(MEMBER)\
	printf("offset of %s\t :\t0x%04zx\n", #MEMBER,		\
		(size_t)(offsetof(struct udma_m2s_regs_v3, m2s_q) + base +\
	       offsetof(struct udma_m2s_q, MEMBER)));

#define udma_s2m_print_offset(MEMBER)\
	printf("offset of %s\t :\t0x%04zx\n", #MEMBER, 	\
	       (size_t)(offsetof(struct udma_s2m_regs_v3, MEMBER) + base));

#define udma_s2m_q_print_offset(MEMBER)\
	printf("offset of %s\t :\t0x%04zx\n", #MEMBER,		\
	       (size_t)(offsetof(struct udma_s2m_regs_v3, s2m_q) + base +	\
	       offsetof(struct udma_s2m_q, MEMBER)));

#define udma_group_print_offset(MEMBER)\
	printf("offset of %s\t :\t0x%04zx\n", #MEMBER, 	\
	       offsetof(struct unit_regs_v3, MEMBER));

static void al_udma_print_reg_offs(void)
{
	printf("sizeof unit_regs 0x%04zx\n", sizeof(struct unit_regs_v3));
	udma_group_print_offset(m2s);
	udma_group_print_offset(s2m);
	udma_group_print_offset(gen);
	print_offset_generic(udma_iofic_regs,  secondary_iofic_ctrl[0]);
	udma_print_offset(axi_m2s);

	udma_print_offset(m2s);
	udma_print_offset(m2s_rd);
	udma_print_offset(m2s_dwrr);
	udma_print_offset(m2s_rate_limiter);
	udma_print_offset(m2s_stream_rate_limiter);
	udma_print_offset(m2s_comp);
	udma_print_offset(m2s_stat);
	udma_print_offset(m2s_feature);
	udma_print_offset(m2s_q);
	printf("size of m2s q:\t0x%04zx\n", sizeof(struct udma_m2s_q));
	udma_q_print_offset(cfg);
	udma_q_print_offset(tcrhp_internal);
	udma_q_print_offset(q_tx_pkt);



	udma_s2m_print_offset(axi_s2m);
	udma_s2m_print_offset(s2m);
	udma_s2m_print_offset(s2m_rd);
	udma_s2m_print_offset(s2m_wr);
	udma_s2m_print_offset(s2m_comp);
	udma_s2m_print_offset(s2m_stat);
	udma_s2m_print_offset(s2m_feature);
	udma_s2m_print_offset(s2m_q);
	printf("size of s2m q:\t0x%04zx\n", sizeof(struct udma_s2m_q));
	udma_s2m_q_print_offset(cfg);
	udma_s2m_q_print_offset(rcrhp_internal);
	udma_s2m_q_print_offset(q_rx_pkt);

}

#define raid_print_offset(MEMBER)\
	printf("offset of %s\t :\t0x%04zx\n", #MEMBER, 	\
	       offsetof(struct raid_accelerator_regs, MEMBER));

static void al_raid_print_reg_offs(void)
{
	raid_print_offset(configuration);
	raid_print_offset(log);
	raid_print_offset(raid_perf_counter);
	raid_print_offset(perfm_cnt_cntl);
	raid_print_offset(gflog_table);
	raid_print_offset(gfilog_table);
	raid_print_offset(raid_status);
}

#define eth_ec_print_offset(MEMBER)\
	printf("offset of %s\t :\t0x%04zx\n", #MEMBER, 	\
	       offsetof(struct al_ec_regs, MEMBER));

static void al_eth_ec_print_reg_offs(void)
{
	eth_ec_print_offset(gen);
	eth_ec_print_offset(rxf);
	eth_ec_print_offset(epe[0]);
	eth_ec_print_offset(epe_res);
	eth_ec_print_offset(rfw);

	eth_ec_print_offset(tso);
	eth_ec_print_offset(tpm_sel[0]);
	eth_ec_print_offset(tfw_udma[0]);

	eth_ec_print_offset(tmi);
	eth_ec_print_offset(fc_udma[0]);
	eth_ec_print_offset(eee);
	eth_ec_print_offset(stat);
	eth_ec_print_offset(stat_udma[0]);
	eth_ec_print_offset(msp);
	eth_ec_print_offset(msp_p);
	eth_ec_print_offset(msp_c);
}

#include "al_hal_iofic_regs.h"
#include "al_hal_iofic.h"

static void iofic(void)
{
}

static void al_iofic_print_regs_offs(void)
{
	printf("interrupt controller\n");
	print_offset_generic(al_iofic_regs,  ctrl[0]);
	print_offset_generic(al_iofic_regs,  ctrl[3]);
	print_offset_generic(al_iofic_regs, grp_int_mod[0][1]);
	print_offset_generic(al_iofic_regs, grp_int_mod[3][1]);
}

int main(void)
{
	printf("Basic Types\n");
	printf("size of uint32_t:\t0x%04zx\n", sizeof(uint32_t));

	al_iofic_print_regs_offs();
	iofic();

	printf("UDMA\n");
	al_udma_print_reg_offs();

	printf("RAID ACCELERATOR\n");
	al_raid_print_reg_offs();

	printf("ETH EC\n");
	al_eth_ec_print_reg_offs();

	return 0;
}
