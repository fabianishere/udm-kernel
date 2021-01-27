#include "samples.h"
#include "al_hal_eth.h"

/* example function that fills the FSM table according to the following:
 * for non-tunnel packets:
 * if the packet is ipv4/ipv6 and tcp/udp, set the thash to be on 4 tuple
 * if the packet is ipv4/ipv6 but not tcp/udp, set the thash to be on 2 tuple
 * otherwise, skip thash and send the packet to queue 0,UDMA 0
 * for tunneled packets:
 * if the inner packet is ipv4/ipv6 and tcp/udp, set the thash to be on 4 tuple of the inner header
 * if the inner packet is ipv4/ipv6 but not tcp/udp, set the thash to be on 2 tuple of the inner header
 * otherwise, use the same configuration as non-tunneled packets.
 */

static int
al_eth_fsm_table_fill_example(struct al_hal_eth_adapter *eth_adapter)
{
        uint32_t val;
        int i;

        for (i = 0; i < AL_ETH_RX_FSM_TABLE_SIZE; i++) {
                uint8_t outer_type = AL_ETH_FSM_ENTRY_OUTER(i);
                /* for tcp/udp packets, select 4 tuple on outer*/
                switch (outer_type) {
                case AL_ETH_FSM_ENTRY_IPV4_TCP:
                case AL_ETH_FSM_ENTRY_IPV4_UDP:
                case AL_ETH_FSM_ENTRY_IPV6_TCP:
                case AL_ETH_FSM_ENTRY_IPV6_UDP:
                        val = AL_ETH_FSM_DATA_OUTER_4_TUPLE | AL_ETH_FSM_DATA_HASH_SEL;
                        break;
                /* if not tcp/udp, select 2 tuple */
                case AL_ETH_FSM_ENTRY_IPV6_NO_UDP_TCP:
                case AL_ETH_FSM_ENTRY_IPV4_NO_UDP_TCP:
                        val = AL_ETH_FSM_DATA_OUTER_2_TUPLE | AL_ETH_FSM_DATA_HASH_SEL;
                        break;
                default:
                        /* if packet is not ipv4/ipv6, then set default queue to 0, and default
                           udma to 1 in order to select udma 0 (default_udma is bitmask) */
                        val = 0 << AL_ETH_FSM_DATA_DEFAULT_Q_SHIFT | ((1 << 0) << AL_ETH_FSM_DATA_DEFAULT_UDMA_SHIFT);
                }
                /* for tunneled packets that are ipv4/ipv6, override the above selection */
                if (AL_ETH_FSM_ENTRY_TUNNELED(i)) {
                        uint8_t inner_type = AL_ETH_FSM_ENTRY_INNER(i);
                        switch (inner_type) {
                                /* for tunneled packets, select 4 tuple on inner */
                                case AL_ETH_FSM_ENTRY_IPV4_TCP:
                                case AL_ETH_FSM_ENTRY_IPV4_UDP:
                                case AL_ETH_FSM_ENTRY_IPV6_TCP:
                                case AL_ETH_FSM_ENTRY_IPV6_UDP:
                                        val = AL_ETH_FSM_DATA_INNER_4_TUPLE | AL_ETH_FSM_DATA_HASH_SEL;
                                        break;
                                /* if inner is not tcp/udp, select 2 tuple on inner*/
                                case AL_ETH_FSM_ENTRY_IPV6_NO_UDP_TCP:
                                case AL_ETH_FSM_ENTRY_IPV4_NO_UDP_TCP:
                                        val = AL_ETH_FSM_DATA_INNER_2_TUPLE | AL_ETH_FSM_DATA_HASH_SEL;
                                        break;
                        }
                }
                al_eth_fsm_table_set(eth_adapter, i, val);
        }
        return 0;
}

/*
 * example function that configures the flow steering.
 * selected udma is alway udma 0.
 * queue selected from thash table, where entry i selects queue i % 4
 * the index of the thash table is hash on the packet according to
 * al_eth_fsm_table_fill_example().
 *
 */
int eth_init_rx_flow_steering(struct al_hal_eth_adapter *eth_adapter) {
	struct al_eth_fwd_ctrl_table_entry ctrl;
	int i;

	memset(&ctrl, 0, sizeof(ctrl));

	ctrl.prio_sel = AL_ETH_CTRL_TABLE_PRIO_SEL_VAL_0;
	/* select queue according to thash */
        ctrl.queue_sel_1 = AL_ETH_CTRL_TABLE_QUEUE_SEL_1_THASH_TABLE;
        ctrl.queue_sel_2 = AL_ETH_CTRL_TABLE_QUEUE_SEL_2_NO_PRIO;
	/* always select UDMA according to register, the value of the register
	 * is set later to 1 (UDMA 0) by al_eth_fwd_default_udma_config() */
	ctrl.udma_sel = AL_ETH_CTRL_TABLE_UDMA_SEL_REG1;
        ctrl.filter = AL_FALSE;

	al_eth_ctrl_table_def_set(eth_adapter, AL_FALSE, &ctrl);

	/* set the register that contains the selected UDMAs (bit mask value) */
	al_eth_fwd_default_udma_config(eth_adapter,  0, 1);
	/* the queue is selected by thash, so no need to set the 
	 * the default queue, but this won't harm */
	al_eth_fwd_default_queue_config(eth_adapter, 0, 0);


       /* set toeplitz hash keys */
        for (i = 0; i < AL_ETH_RX_HASH_KEY_NUM; i++)
                al_eth_hash_key_set(eth_adapter, i, rand()); 

	/* set the requested queue in the tash redirection table
	 * here we spread the packets in round robin fashion to
	 * 4 queues
	 */ 
        for (i = 0; i < AL_ETH_RX_THASH_TABLE_SIZE; i++) {
                al_eth_thash_table_set(eth_adapter, i, 0, i % 4);
        }

	/* configure the fsm table so thash will be done on inner packet if
	 * found, and on 4 tuple if found.
	 */
	al_eth_fsm_table_fill_example(eth_adapter);

	return 0;
}

