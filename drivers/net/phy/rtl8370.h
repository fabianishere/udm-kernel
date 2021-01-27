/*
 * rtl8370.h: RTL8370 switch driver header
 * Copyright 2018 Ubiquiti Networks, Inc.
 */
#ifndef _RTL8370_H_
#define _RTL8370_H_

#include "ubnt_acl.h"
#include "rtl83xx_api/acl.h"

/**
 * @brief List of available MIB Stats
 */
#define MIB_LIST_OF_STATS(X)                              \
	X(ifInOctets, rtk_uint64)                         \
	X(dot3StatsFCSErrors, rtk_uint32)                 \
	X(dot3StatsSymbolErrors, rtk_uint32)              \
	X(dot3InPauseFrames, rtk_uint32)                  \
	X(dot3ControlInUnknownOpcodes, rtk_uint32)        \
	X(etherStatsFragments, rtk_uint32)                \
	X(etherStatsJabbers, rtk_uint32)                  \
	X(ifInUcastPkts, rtk_uint32)                      \
	X(etherStatsDropEvents, rtk_uint32)               \
	X(etherStatsOctets, rtk_uint64)                   \
	X(etherStatsUndersizePkts, rtk_uint32)            \
	X(etherStatsOversizePkts, rtk_uint32)             \
	X(etherStatsPkts64Octets, rtk_uint32)             \
	X(etherStatsPkts65to127Octets, rtk_uint32)        \
	X(etherStatsPkts128to255Octets, rtk_uint32)       \
	X(etherStatsPkts256to511Octets, rtk_uint32)       \
	X(etherStatsPkts512to1023Octets, rtk_uint32)      \
	X(etherStatsPkts1024toMaxOctets, rtk_uint32)      \
	X(etherStatsMcastPkts, rtk_uint32)                \
	X(etherStatsBcastPkts, rtk_uint32)                \
	X(ifOutOctets, rtk_uint64)                        \
	X(dot3StatsSingleCollisionFrames, rtk_uint32)     \
	X(dot3StatsMultipleCollisionFrames, rtk_uint32)   \
	X(dot3StatsDeferredTransmissions, rtk_uint32)     \
	X(dot3StatsLateCollisions, rtk_uint32)            \
	X(etherStatsCollisions, rtk_uint32)               \
	X(dot3StatsExcessiveCollisions, rtk_uint32)       \
	X(dot3OutPauseFrames, rtk_uint32)                 \
	X(dot1dBasePortDelayExceededDiscards, rtk_uint32) \
	X(dot1dTpPortInDiscards, rtk_uint32)              \
	X(ifOutUcastPkts, rtk_uint32)                     \
	X(ifOutMulticastPkts, rtk_uint32)                 \
	X(ifOutBrocastPkts, rtk_uint32)                   \
	X(outOampduPkts, rtk_uint32)                      \
	X(inOampduPkts, rtk_uint32)                       \
	X(pktgenPkts, rtk_uint32)                         \
	X(inMldChecksumError, rtk_uint32)                 \
	X(inIgmpChecksumError, rtk_uint32)                \
	X(inMldSpecificQuery, rtk_uint32)                 \
	X(inMldGeneralQuery, rtk_uint32)                  \
	X(inIgmpSpecificQuery, rtk_uint32)                \
	X(inIgmpGeneralQuery, rtk_uint32)                 \
	X(inMldLeaves, rtk_uint32)                        \
	X(inIgmpLeaves, rtk_uint32)                       \
	X(inIgmpJoinsSuccess, rtk_uint32)                 \
	X(inIgmpJoinsFail, rtk_uint32)                    \
	X(inMldJoinsSuccess, rtk_uint32)                  \
	X(inMldJoinsFail, rtk_uint32)                     \
	X(inReportSuppressionDrop, rtk_uint32)            \
	X(inLeaveSuppressionDrop, rtk_uint32)             \
	X(outIgmpReports, rtk_uint32)                     \
	X(outIgmpLeaves, rtk_uint32)                      \
	X(outIgmpGeneralQuery, rtk_uint32)                \
	X(outIgmpSpecificQuery, rtk_uint32)               \
	X(outMldReports, rtk_uint32)                      \
	X(outMldLeaves, rtk_uint32)                       \
	X(outMldGeneralQuery, rtk_uint32)                 \
	X(outMldSpecificQuery, rtk_uint32)                \
	X(inKnownMulticastPkts, rtk_uint32)               \
	X(ifInMulticastPkts, rtk_uint32)                  \
	X(ifInBroadcastPkts, rtk_uint32)                  \
	X(ifOutDiscards, rtk_uint32)

#ifndef RTL8370_CUSTOM_PORT_MAPPING

/* ETH driver compatibility */
#define RTL8370_PHY_ADDR_CPU_0 (0x10)
#define RTL8370_PHY_ADDR_CPU_1 (0x11)

/**
 * @brief Map physical port to logical port
 */
#define PORT_MAPPING_DEFAULT(X)                   \
	X(PHY_PORT0, UTP_PORT0, "EDGE PORT 0")    \
	X(PHY_PORT1, UTP_PORT1, "EDGE PORT 1")    \
	X(PHY_PORT2, UTP_PORT2, "EDGE PORT 2")    \
	X(PHY_PORT3, UTP_PORT3, "EDGE PORT 3")    \
	X(PHY_PORT4, UTP_PORT4, "EDGE PORT 4")    \
	X(PHY_PORT5, UTP_PORT5, "EDGE PORT 5")    \
	X(PHY_PORT6, UTP_PORT6, "EDGE PORT 6")    \
	X(PHY_PORT7, UTP_PORT7, "EDGE PORT 7")    \
	X(PHY_PORT8, EXT_PORT0, "CPU PORT 0") \
	X(PHY_PORT9, EXT_PORT1, "CPU PORT 1")

/**
 * @brief Logical port as swconfig will see it
 */
typedef enum _port_mapping {
#define X(phy, log, desc) phy,
	PORT_MAPPING_DEFAULT(X)
#undef X
	PHY_PORT_COUNT,
	PHY_PORT_UNKNOWN
} port_mapping_e;

#define RTL8370_CPU_PORT_0 (PHY_PORT8)
#define RTL8370_CPU_PORT_1 (PHY_PORT9)
#define RTL8370_NUM_PORTS_CPU (2)
#define RTL8370_NUM_PORTS_PHY (PHY_PORT_COUNT)
#define RTL8370_NUM_PORTS_LOG (RTK_PORT_MAX)

#endif /* ! RTL8370_CUSTOM_PORT_MAPPING */

#define RTL8370_NUM_VIDS (4096)
/* max SVLAN vid */
#define RTL8370_SVLAN_MAX_VID (4094)
/* Reserve 4095 for the default VID */
#define RTL8370_DEFAULT_SVLAN_VID (4095)
/* Reserve one entry for default SVLAN */
#define RTL8370_NUM_SVLAN (64-1)

#define RTL8370_DEFAULT_PRIORITY (0)
#define RTL8370_NUM_ACL_ENTRIES	(96)
#define RTL8370_ARL_AGE_TIME_MAX_S (458)
#define RTL8370_ARL_AGE_TIME_DEFAULT_S (300)
#define RTL8370_HAS_NO_PHY(port) (RTL8370_CPU_PORT_0 == (port) || RTL8370_CPU_PORT_1 == (port))

typedef rtk_uint32 rtl_arl_iter_t;

typedef union {
	struct {
		rtk_filter_field_t fld;
		rtk_filter_action_t act;
		rtk_filter_cfg_t cfg;
	} in;

	struct {
		rtk_filter_action_t act;
		rtk_filter_cfg_raw_t cfg;
	} out;
} rt8370_acl_entry_hw_t;

struct rtl8370_mac_mode {
	rtk_uint32 speed;
	rtk_mode_ext_t mode;
	rtk_data_t delay_tx;
	rtk_data_t delay_rx;
};

struct rtl8370_svlan_entry {
	uint16_t member;
	uint16_t id;
	uint16_t trunk;
	uint8_t efid;
};

struct rtl8370_svlan_table {
	struct rtl8370_svlan_entry lut[RTL8370_NUM_SVLAN];
	ssize_t lut_cnt;
	uint16_t default_svid[RTL8370_NUM_PORTS_PHY];
};

struct rtl8370_priv {
	struct mii_bus *mii_bus;
	struct switch_dev swdev;
	struct rtlglue_ctl api_ctl;
	char buffer[4096];
	bool reset_on_init;
	unsigned int use_count;
	uint8_t led_profile;
	struct list_head list;
	uint32_t vlan_enabled;
	uint16_t arl_age_time;
	struct acl_hw hw_acl;
	uint8_t  fid_table[RTL8370_NUM_VIDS];
	uint8_t  efid_table[RTL8370_NUM_PORTS_PHY];

	uint16_t vlan_table[RTL8370_NUM_VIDS];
	struct rtl8370_svlan_table svlan_table;
	uint16_t pvid_tagged;
	uint16_t pvid_table[RTL8370_NUM_PORTS_PHY];
	/* array of port isolation masks */
	rtk_portmask_t port_isolation[RTL8370_NUM_PORTS_PHY];
	struct rtl8370_mac_mode mac_mode[RTL8370_NUM_PORTS_CPU];
};

#define RTL8370_INTERPRET_API_ERRORS

#ifdef RTL8370_INTERPRET_API_ERRORS

typedef struct rtl_api_err {
	const uint32_t code;
	const char *desc;
} rtl_api_err_t;

#define RTL_API_ERRORS_DEFS(X)                                                                \
	X(RT_ERR_FAILED, "General Error")                                                     \
	X(RT_ERR_OK, "OK")                                                                    \
	X(RT_ERR_INPUT, "invalid input parameter")                                            \
	X(RT_ERR_UNIT_ID, "invalid unit id")                                                  \
	X(RT_ERR_PORT_ID, "invalid port id")                                                  \
	X(RT_ERR_PORT_MASK, "invalid port mask")                                              \
	X(RT_ERR_PORT_LINKDOWN, "link down port status")                                      \
	X(RT_ERR_ENTRY_INDEX, "invalid entry index")                                          \
	X(RT_ERR_NULL_POINTER, "input parameter is null pointer")                             \
	X(RT_ERR_QUEUE_ID, "invalid queue id")                                                \
	X(RT_ERR_QUEUE_NUM, "invalid queue number")                                           \
	X(RT_ERR_BUSYWAIT_TIMEOUT, "busy watting time out")                                   \
	X(RT_ERR_MAC, "invalid mac address")                                                  \
	X(RT_ERR_OUT_OF_RANGE, "input parameter out of range")                                \
	X(RT_ERR_CHIP_NOT_SUPPORTED, "functions not supported by this chip model")            \
	X(RT_ERR_SMI, "SMI error")                                                            \
	X(RT_ERR_NOT_INIT, "The module is not initial")                                       \
	X(RT_ERR_CHIP_NOT_FOUND, "The chip can not found")                                    \
	X(RT_ERR_NOT_ALLOWED, "actions not allowed by the function")                          \
	X(RT_ERR_DRIVER_NOT_FOUND, "The driver can not found")                                \
	X(RT_ERR_SEM_LOCK_FAILED, "Failed to lock semaphore")                                 \
	X(RT_ERR_SEM_UNLOCK_FAILED, "Failed to unlock semaphore")                             \
	X(RT_ERR_ENABLE, "invalid enable parameter")                                          \
	X(RT_ERR_TBL_FULL, "input table full")                                                \
	X(RT_ERR_VLAN_VID, "invalid vid")                                                     \
	X(RT_ERR_VLAN_PRIORITY, "invalid 1p priority")                                        \
	X(RT_ERR_VLAN_EMPTY_ENTRY, "emtpy entry of vlan table")                               \
	X(RT_ERR_VLAN_ACCEPT_FRAME_TYPE, "invalid accept frame type")                         \
	X(RT_ERR_VLAN_EXIST, "vlan is exist")                                                 \
	X(RT_ERR_VLAN_ENTRY_NOT_FOUND, "specified vlan entry not found")                      \
	X(RT_ERR_VLAN_PORT_MBR_EXIST, "member port exist in the specified vlan")              \
	X(RT_ERR_VLAN_PROTO_AND_PORT, "invalid protocol and port based vlan")                 \
	X(RT_ERR_SVLAN_ENTRY_INDEX, "invalid svid entry no")                                  \
	X(RT_ERR_SVLAN_ETHER_TYPE, "invalid SVLAN ether type")                                \
	X(RT_ERR_SVLAN_TABLE_FULL, "no empty entry in SVLAN table")                           \
	X(RT_ERR_SVLAN_ENTRY_NOT_FOUND, "specified svlan entry not found")                    \
	X(RT_ERR_SVLAN_EXIST, "SVLAN entry is exist")                                         \
	X(RT_ERR_SVLAN_VID, "invalid svid")                                                   \
	X(RT_ERR_MSTI, "invalid msti")                                                        \
	X(RT_ERR_MSTP_STATE, "invalid spanning tree status")                                  \
	X(RT_ERR_MSTI_EXIST, "MSTI exist")                                                    \
	X(RT_ERR_MSTI_NOT_EXIST, "MSTI not exist")                                            \
	X(RT_ERR_TIMESLOT, "invalid time slot")                                               \
	X(RT_ERR_TOKEN, "invalid token amount")                                               \
	X(RT_ERR_RATE, "invalid rate")                                                        \
	X(RT_ERR_TICK, "invalid tick")                                                        \
	X(RT_ERR_RMA_ADDR, "invalid rma mac address")                                         \
	X(RT_ERR_RMA_ACTION, "invalid rma action")                                            \
	X(RT_ERR_L2_HASH_KEY, "invalid L2 Hash key")                                          \
	X(RT_ERR_L2_HASH_INDEX, "invalid L2 Hash index")                                      \
	X(RT_ERR_L2_CAM_INDEX, "invalid L2 CAM index")                                        \
	X(RT_ERR_L2_ENRTYSEL, "invalid EntrySel")                                             \
	X(RT_ERR_L2_INDEXTABLE_INDEX, "invalid L2 index table(=portMask table) index")        \
	X(RT_ERR_LIMITED_L2ENTRY_NUM, "invalid limited L2 entry number")                      \
	X(RT_ERR_L2_AGGREG_PORT, "this aggregated port is not the lowest physical")           \
	X(RT_ERR_L2_FID, "invalid fid")                                                       \
	X(RT_ERR_L2_VID, "invalid cvid")                                                      \
	X(RT_ERR_L2_NO_EMPTY_ENTRY, "no empty entry in L2 table")                             \
	X(RT_ERR_L2_ENTRY_NOTFOUND, "specified entry not found")                              \
	X(RT_ERR_L2_INDEXTBL_FULL, "the L2 index table is full")                              \
	X(RT_ERR_L2_INVALID_FLOWTYPE, "invalid L2 flow type")                                 \
	X(RT_ERR_L2_L2UNI_PARAM, "invalid L2 unicast parameter")                              \
	X(RT_ERR_L2_L2MULTI_PARAM, "invalid L2 multicast parameter")                          \
	X(RT_ERR_L2_IPMULTI_PARAM, "invalid L2 ip multicast parameter")                       \
	X(RT_ERR_L2_PARTIAL_HASH_KEY, "invalid L2 partial Hash key")                          \
	X(RT_ERR_L2_EMPTY_ENTRY, "the entry is empty(invalid)")                               \
	X(RT_ERR_L2_FLUSH_TYPE, "the flush type is invalid")                                  \
	X(RT_ERR_L2_NO_CPU_PORT, "CPU port not exist")                                        \
	X(RT_ERR_FILTER_BLOCKNUM, "invalid block number")                                     \
	X(RT_ERR_FILTER_ENTRYIDX, "invalid entry index")                                      \
	X(RT_ERR_FILTER_CUTLINE, "invalid cutline value")                                     \
	X(RT_ERR_FILTER_FLOWTBLBLOCK, "block belongs to flow table")                          \
	X(RT_ERR_FILTER_INACLBLOCK, "block belongs to ingress ACL")                           \
	X(RT_ERR_FILTER_ACTION, "action doesn't consist to entry type")                       \
	X(RT_ERR_FILTER_INACL_RULENUM, "invalid ACL rulenum")                                 \
	X(RT_ERR_FILTER_INACL_TYPE, "entry type isn't an ingress ACL rule")                   \
	X(RT_ERR_FILTER_INACL_EXIST, "ACL entry is already exit")                             \
	X(RT_ERR_FILTER_INACL_EMPTY, "ACL entry is empty")                                    \
	X(RT_ERR_FILTER_FLOWTBL_TYPE, "entry type isn't an flow table rule")                  \
	X(RT_ERR_FILTER_FLOWTBL_RULENUM, "invalid flow table rulenum")                        \
	X(RT_ERR_FILTER_FLOWTBL_EMPTY, "flow table entry is empty")                           \
	X(RT_ERR_FILTER_FLOWTBL_EXIST, "flow table entry is already exist")                   \
	X(RT_ERR_FILTER_METER_ID, "invalid metering id")                                      \
	X(RT_ERR_FILTER_LOG_ID, "invalid log id")                                             \
	X(RT_ERR_FILTER_INACL_NONE_BEGIN_IDX,                                                 \
	  "entry index is not starting index of a group of rules")                            \
	X(RT_ERR_FILTER_INACL_ACT_NOT_SUPPORT, "action not support")                          \
	X(RT_ERR_FILTER_INACL_RULE_NOT_SUPPORT, "rule not support")                           \
	X(RT_ERR_ACLRL_HTHR, "invalid high threshold")                                        \
	X(RT_ERR_ACLRL_TIMESLOT, "invalid time slot")                                         \
	X(RT_ERR_ACLRL_TOKEN, "invalid token amount")                                         \
	X(RT_ERR_ACLRL_RATE, "invalid rate")                                                  \
	X(RT_ERR_LA_CPUPORT, "CPU port can not be aggregated port")                           \
	X(RT_ERR_LA_TRUNK_ID, "invalid trunk id")                                             \
	X(RT_ERR_LA_PORTMASK, "invalid port mask")                                            \
	X(RT_ERR_LA_HASHMASK, "invalid hash mask")                                            \
	X(RT_ERR_LA_DUMB, "this API should be used in 802.1ad dumb mode")                     \
	X(RT_ERR_LA_PORTNUM_DUMB,                                                             \
	  "it can only aggregate at most four ports when 802.1ad dumb mode")                  \
	X(RT_ERR_LA_PORTNUM_NORMAL,                                                           \
	  "it can only aggregate at most eight ports when 802.1ad normal mode")               \
	X(RT_ERR_LA_MEMBER_OVERLAP, "the specified port mask is overlapped with other group") \
	X(RT_ERR_LA_NOT_MEMBER_PORT, "the port is not a member port of the trunk")            \
	X(RT_ERR_LA_TRUNK_NOT_EXIST, "the trunk doesn't exist")                               \
	X(RT_ERR_SFC_TICK_PERIOD, "invalid SFC tick period")                                  \
	X(RT_ERR_SFC_UNKNOWN_GROUP, "Unknown Storm filter group")                             \
	X(RT_ERR_PM_MASK, "invalid pattern length. Pattern length should be 8")               \
	X(RT_ERR_PM_LENGTH, "first byte must care")                                           \
	X(RT_ERR_PM_MODE, "invalid pattern match mode")                                       \
	X(RT_ERR_INBW_TICK_PERIOD, "invalid tick period for input bandwidth control")         \
	X(RT_ERR_INBW_TOKEN_AMOUNT, "invalid amount of token for input bandwidth control")    \
	X(RT_ERR_INBW_FCON_VALUE,                                                             \
	  "invalid flow control ON threshold value for input bandwidth control")              \
	X(RT_ERR_INBW_FCOFF_VALUE,                                                            \
	  "invalid flow control OFF threshold value for input bandwidth control")             \
	X(RT_ERR_INBW_FC_ALLOWANCE,                                                           \
	  "invalid allowance of incomming packet for input bandwidth control")                \
	X(RT_ERR_INBW_RATE, "invalid input bandwidth")                                        \
	X(RT_ERR_QOS_1P_PRIORITY, "invalid 802.1P priority")                                  \
	X(RT_ERR_QOS_DSCP_VALUE, "invalid DSCP value")                                        \
	X(RT_ERR_QOS_INT_PRIORITY, "invalid internal priority")                               \
	X(RT_ERR_QOS_SEL_DSCP_PRI, "invalid DSCP selection priority")                         \
	X(RT_ERR_QOS_SEL_PORT_PRI, "invalid port selection priority")                         \
	X(RT_ERR_QOS_SEL_IN_ACL_PRI, "invalid ingress ACL selection priority")                \
	X(RT_ERR_QOS_SEL_CLASS_PRI, "invalid classifier selection priority")                  \
	X(RT_ERR_QOS_EBW_RATE, "invalid egress bandwidth rate")                               \
	X(RT_ERR_QOS_SCHE_TYPE, "invalid QoS scheduling type")                                \
	X(RT_ERR_QOS_QUEUE_WEIGHT, "invalid Queue weight")                                    \
	X(RT_ERR_QOS_SEL_PRI_SOURCE, "invalid selection of priority source")                  \
	X(RT_ERR_PHY_PAGE_ID, "invalid PHY page id")                                          \
	X(RT_ERR_PHY_REG_ID, "invalid PHY reg id")                                            \
	X(RT_ERR_PHY_DATAMASK, "invalid PHY data mask")                                       \
	X(RT_ERR_PHY_AUTO_NEGO_MODE, "invalid PHY auto-negotiation mode")                     \
	X(RT_ERR_PHY_SPEED, "invalid PHY speed setting")                                      \
	X(RT_ERR_PHY_DUPLEX, "invalid PHY duplex setting")                                    \
	X(RT_ERR_PHY_FORCE_ABILITY, "invalid PHY force mode ability parameter")               \
	X(RT_ERR_PHY_FORCE_1000, "invalid PHY force mode 1G speed setting")                   \
	X(RT_ERR_PHY_TXRX, "invalid PHY tx/rx")                                               \
	X(RT_ERR_PHY_ID, "invalid PHY id")                                                    \
	X(RT_ERR_PHY_RTCT_NOT_FINISH, "PHY RTCT in progress")                                 \
	X(RT_ERR_MIRROR_DIRECTION, "invalid error mirror direction")                          \
	X(RT_ERR_MIRROR_SESSION_FULL, "mirroring session is full")                            \
	X(RT_ERR_MIRROR_SESSION_NOEXIST, "mirroring session not exist")                       \
	X(RT_ERR_MIRROR_PORT_EXIST, "mirroring port already exists")                          \
	X(RT_ERR_MIRROR_PORT_NOT_EXIST, "mirroring port does not exists")                     \
	X(RT_ERR_MIRROR_PORT_FULL, "Exceeds maximum number of supported mirroring port")      \
	X(RT_ERR_STAT_INVALID_GLOBAL_CNTR, "Invalid Global Counter")                          \
	X(RT_ERR_STAT_INVALID_PORT_CNTR, "Invalid Port Counter")                              \
	X(RT_ERR_STAT_GLOBAL_CNTR_FAIL, "Could not retrieve/reset Global Counter")            \
	X(RT_ERR_STAT_PORT_CNTR_FAIL, "Could not retrieve/reset Port Counter")                \
	X(RT_ERR_STAT_INVALID_CNTR, "Invalid Counter")                                        \
	X(RT_ERR_STAT_CNTR_FAIL, "Could not retrieve/reset Counter")                          \
	X(RT_ERR_DOT1X_INVALID_DIRECTION, "Invalid Authentication Direction")                 \
	X(RT_ERR_DOT1X_PORTBASEDPNEN, "Port-based enable port error")                         \
	X(RT_ERR_DOT1X_PORTBASEDAUTH, "Port-based auth port error")                           \
	X(RT_ERR_DOT1X_PORTBASEDOPDIR, "Port-based opdir error")                              \
	X(RT_ERR_DOT1X_MACBASEDPNEN, "MAC-based enable port error")                           \
	X(RT_ERR_DOT1X_MACBASEDOPDIR, "MAC-based opdir error")                                \
	X(RT_ERR_DOT1X_PROC, "unauthorized behavior error")                                   \
	X(RT_ERR_DOT1X_GVLANIDX, "guest vlan index error")                                    \
	X(RT_ERR_DOT1X_GVLANTALK, "guest vlan OPDIR error")                                   \
	X(RT_ERR_DOT1X_MAC_PORT_MISMATCH, "Auth MAC and port mismatch eror")

typedef enum _rtl_api_err {
#define X(code, desc) code##_E,
	RTL_API_ERRORS_DEFS(X)
#undef X
		RTL_API_ERR_COUNT
} rtl_api_err_e;

#endif /* RTL8370_INTERPRET_API_ERRORS */

#endif /* _RTL8370_H_ */
