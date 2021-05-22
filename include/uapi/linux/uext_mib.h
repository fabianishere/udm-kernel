/*
 * UBNT extension (uext) mib counter definition
 * Copyright 2020 Ubiquiti Networks, Inc.
 */
#define SWITCH_MIB_COUNTERS(COUNTER) \
		COUNTER(RX_BYTE, rx_byte) \
		COUNTER(TX_BYTE, tx_byte) \
		COUNTER(RX_TOTAL, rx_total) \
		COUNTER(TX_TOTAL, tx_total) \
		COUNTER(RX_BCAST, rx_bcast) \
		COUNTER(TX_BCAST, tx_bcast) \
		COUNTER(RX_MCAST, rx_mcast) \
		COUNTER(TX_MCAST, tx_mcast) \
		COUNTER(RX_PAUSE, rx_pause) \
		COUNTER(TX_PAUSE, tx_pause) \
		COUNTER(RX_JUMBO, rx_jumbo) \
		COUNTER(TX_JUMBO, tx_jumbo) \
		COUNTER(RX_DROPPED, rx_dropped) \
		COUNTER(TX_DROPPED, tx_dropped) \
		COUNTER(RX_ERROR, rx_error) \
		COUNTER(TX_ERROR, tx_error) \
		COUNTER(RX_PPS, rx_pps) \
		COUNTER(TX_PPS, tx_pps) \
		COUNTER(RX_RATE, rx_rate) \
		COUNTER(TX_RATE, tx_rate)
