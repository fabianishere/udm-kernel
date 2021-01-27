#ifndef AL_HAL_ETH_RX_FWD_LRSS_TABLES_H_
#define AL_HAL_ETH_RX_FWD_LRSS_TABLES_H_

#include "al_hal_eth_fwd_tables.h"

#define LRSS_TABLE_FIELD_QUEUE_CMD_DIDX                                         0
#define LRSS_TABLE_FIELD_UDMA_CMD_DIDX                                          0
#define LRSS_TABLE_FIELD_TID_CMD_DIDX                                           0
#define LRSS_TABLE_FIELD_QUEUE_VALUE_DIDX                                       0
#define LRSS_TABLE_FIELD_UDMA_VALUE_DIDX                                        0
#define LRSS_TABLE_FIELD_TID_VALUE_DIDX                                         0

#define LRSS_TABLE_FIELD_QUEUE_CMD_FTYPE                                        BIT
#define LRSS_TABLE_FIELD_UDMA_CMD_FTYPE                                         BIT
#define LRSS_TABLE_FIELD_TID_CMD_FTYPE                                          BIT
#define LRSS_TABLE_FIELD_QUEUE_VALUE_FTYPE                                      MULTI
#define LRSS_TABLE_FIELD_UDMA_VALUE_FTYPE                                       MULTI
#define LRSS_TABLE_FIELD_TID_VALUE_FTYPE                                        MULTI

#define LRSS_TABLE_ENTRY_DWORD_COUNT                                            1

#define LRSS_TABLE_FIELD_SET(entry, fname, value) \
	FWD_TABLE_FIELD_SET(LRSS_TABLE, ETH_RFW_CPU_LRSS_INDIRECTION_TABLE_DATA, \
			entry, fname, value)

#define LRSS_TABLE_FIELD_GET(entry, fname) \
	FWD_TABLE_FIELD_GET(LRSS_TABLE, ETH_RFW_CPU_LRSS_INDIRECTION_TABLE_DATA, \
			entry, fname)

#endif /* AL_HAL_ETH_RX_FWD_LRSS_TABLES_H_ */
