#ifndef IPA_DPED_SERVICE_01_H
#define IPA_DPED_SERVICE_01_H
/**
  @file ipa_datapath_e2e_debug_v01.h

  @brief This is the public header file which defines the ipa_dped service Data structures.

  This header file defines the types and structures that were defined in
  ipa_dped. It contains the constant values defined, enums, structures,
  messages, and service message IDs (in that order) Structures that were
  defined in the IDL as messages contain mandatory elements, optional
  elements, a combination of mandatory and optional elements (mandatory
  always come before optionals in the structure), or nothing (null message)

  An optional element in a message is preceded by a uint8_t value that must be
  set to true if the element is going to be included. When decoding a received
  message, the uint8_t values will be set to true or false by the decode
  routine, and should be checked before accessing the values that they
  correspond to.

  Variable sized arrays are defined as static sized arrays with an unsigned
  integer (32 bit) preceding it that must be set to the number of elements
  in the array that are valid. For Example:

  uint32_t test_opaque_len;
  uint8_t test_opaque[16];

  If only 4 elements are added to test_opaque[] then test_opaque_len must be
  set to 4 before sending the message.  When decoding, the _len value is set
  by the decode routine and should be checked so that the correct number of
  elements in the array will be accessed.

*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2021 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.



  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7
   It was generated on: Fri Jul 30 2021 (Spin 0)
   From IDL File: ipa_datapath_e2e_debug_v01.idl */

/** @defgroup ipa_dped_qmi_consts Constant values defined in the IDL */
/** @defgroup ipa_dped_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup ipa_dped_qmi_enums Enumerated types used in QMI messages */
/** @defgroup ipa_dped_qmi_messages Structures sent as QMI messages */
/** @defgroup ipa_dped_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup ipa_dped_qmi_accessor Accessor for QMI service object */
/** @defgroup ipa_dped_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup ipa_dped_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define IPA_DPED_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define IPA_DPED_V01_IDL_MINOR_VERS 0x00
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define IPA_DPED_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define IPA_DPED_V01_MAX_MESSAGE_ID 0x0021
/**
    @}
  */


/** @addtogroup ipa_dped_qmi_consts
    @{
  */
#define QMI_IPA_DPED_IPA_LNX_HOLB_NUM_CLIENTS_MAX_V01 5
#define QMI_IPA_DPED_IPA_LNX_HOLB_NUM_MON_CLIENTS_MAX_V01 5
#define QMI_IPA_DPED_GSI_DBG_NUM_TX_INST_MAX_V01 3
#define QMI_IPA_DPED_GSI_DBG_NUM_RX_INST_MAX_V01 3
#define QMI_IPA_DPED_GSI_NUM_INST_MAX_V01 3
#define QMI_IPA_DPED_DATA_LNX_STATS_MAX_V01 2
#define QMI_IPA_DPED_DATA_LNX_TCP_STATS_MAX_V01 1
#define QMI_IPA_DPED_DATA_LNX_RMNET_STATS_MAX_V01 10
#define QMI_IPA_DPED_DATA_LNX_IPV6_SIZE_V01 16
#define QMI_IPA_DPED_DATA_ETH_RING_STATS_MAX_V01 1
#define QMI_IPA_DPED_DATA_ETH_PKT_STATS_MAX_V01 4
/**
    @}
  */

/** @addtogroup ipa_dped_qmi_messages
    @{
  */
/** Request Message; Registers for indications from a service. */
typedef struct {

  /* Optional */
  /*  Trigger E2E Stats */
  uint8_t trigger_e2e_stats_valid;  /**< Must be set to true if trigger_e2e_stats is being passed */
  uint8_t trigger_e2e_stats;
  /**<   If set to TRUE, this field indicates that the client wants to receive 
         QMI_IPA_DPED_TRIGGER_E2E_STATS_IND indications to trigger polling for E2E stats.
    */
}ipa_dped_indication_reg_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_dped_qmi_messages
    @{
  */
/** Response Message; Registers for indications from a service. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.
 Standard response type. Contains the following data members: \n
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
     - qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message definition.
  */
}ipa_dped_indication_reg_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_dped_qmi_messages
    @{
  */
/** Indication Message; Requests the AP to trigger the E2E statistics collection */
typedef struct {

  /* Optional */
  /*  log_seq_id */
  uint8_t log_seq_id_valid;  /**< Must be set to true if log_seq_id is being passed */
  uint32_t log_seq_id;
  /**<   Log Sequence ID
  */

  /* Optional */
  /*  dl_burst_size */
  uint8_t dl_burst_size_valid;  /**< Must be set to true if dl_burst_size is being passed */
  uint32_t dl_burst_size;
  /**<   DL Burst Size in bytes
  */

  /* Optional */
  /*  vstmr_ticks_xo */
  uint8_t vstmr_ticks_xo_valid;  /**< Must be set to true if vstmr_ticks_xo is being passed */
  uint32_t vstmr_ticks_xo;
  /**<   VSTMR Ticks in XO units
  */
}ipa_dped_trigger_e2e_stats_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_dped_qmi_enums
    @{
  */
typedef enum {
  IPA_DPED_ETH_PKT_STATS_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QMI_IPA_DPED_ETH_PKT_STATS_EMAC_V01 = 0, /**<  EMAC
   */
  QMI_IPA_DPED_ETH_PKT_STATS_HWP_V01 = 1, /**<  Hardware Path
   */
  QMI_IPA_DPED_ETH_PKT_STATS_SWP_V01 = 2, /**<  Software Path
   */
  QMI_IPA_DPED_ETH_PKT_STATS_EXCEP_V01 = 3, /**<  Exception
   */
  IPA_DPED_ETH_PKT_STATS_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ipa_dped_eth_pkt_stats_enum_v01;
/**
    @}
  */

/** @addtogroup ipa_dped_qmi_aggregates
    @{
  */
typedef struct {

  uint64_t pkts;
  /**<   Total number of packets
  */

  uint64_t bytes;
  /**<   Total number of bytes
  */

  uint64_t errors;
  /**<   Total number of errors
  */

  uint64_t drops;
  /**<   Total number of drops
  */

  uint32_t reserved_1;
  /**<   Reserved 1 field
  */

  uint32_t reserved_2;
  /**<   Reserved 2 field
  */
}ipa_dped_data_eth_pkt_stats_inst_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_dped_qmi_aggregates
    @{
  */
typedef struct {

  ipa_dped_eth_pkt_stats_enum_v01 stats_type;
  /**<   Statistics type
  */

  ipa_dped_data_eth_pkt_stats_inst_type_v01 tx_stats;
  /**<   TX Statistics
  */

  ipa_dped_data_eth_pkt_stats_inst_type_v01 rx_stats;
  /**<   RX Statistics
  */
}ipa_dped_data_eth_pkt_stats_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_dped_qmi_aggregates
    @{
  */
typedef struct {

  uint64_t pause_frames;
  /**<   Total number of pause frames
  */

  uint64_t desc_unavail;
  /**<   Total number of descriptor unavailable
  */
}ipa_dped_data_eth_emac_addl_inst_stats_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_dped_qmi_aggregates
    @{
  */
typedef struct {

  ipa_dped_data_eth_emac_addl_inst_stats_type_v01 tx_stats;
  /**<   EMAC TX Statistics
  */

  ipa_dped_data_eth_emac_addl_inst_stats_type_v01 rx_stats;
  /**<   EMAC RX Statistics
  */
}ipa_dped_data_eth_emac_addl_stats_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_dped_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t dma_channel_status;
  /**<   RX DMA channel status 
       0 - Channel Disabled
       1 - Channel Enabled
  */

  uint64_t head_ptr;
  /**<   head pointer index
  */

  uint64_t tail_ptr;
  /**<   tail pointer index
  */

  uint64_t overflow_error_cnt;
  /**<   overflow error count
  */

  uint64_t underflow_error_cnt;
  /**<   underflow error count
  */
}ipa_dped_data_eth_ring_inst_stats_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_dped_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t ring_index;
  /**<   Ring index
  */

  uint8_t offload;
  /**<   offload 
       0 - IPA offloaded
       1 - IPA not offloaded
  */

  ipa_dped_data_eth_ring_inst_stats_type_v01 tx_ring_stats;
  /**<   TX Ring statistics
  */

  ipa_dped_data_eth_ring_inst_stats_type_v01 rx_ring_stats;
  /**<   RX Ring statistics
  */
}ipa_dped_data_eth_ring_stats_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_dped_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t iface_idx;
  /**<   Ethernet interface index
  */

  uint32_t pkt_stats_len;  /**< Must be set to # of elements in pkt_stats */
  ipa_dped_data_eth_pkt_stats_type_v01 pkt_stats[QMI_IPA_DPED_DATA_ETH_PKT_STATS_MAX_V01];
  /**<   Packet statistics
  */

  ipa_dped_data_eth_emac_addl_stats_type_v01 emac_addl_stats;
  /**<   EMAC additional statistics
  */

  uint32_t ring_stats_len;  /**< Must be set to # of elements in ring_stats */
  ipa_dped_data_eth_ring_stats_type_v01 ring_stats[QMI_IPA_DPED_DATA_ETH_RING_STATS_MAX_V01];
  /**<   Ring statistics
  */
}ipa_dped_data_eth_stats_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_dped_qmi_aggregates
    @{
  */
typedef struct {

  uint64_t inHdrErrors;
  /**<   IP RX packets dropped because of IP header problems
  */

  uint64_t inTooBigErrors;
  /**<   IP RX packets dropped because of size
  */

  uint64_t inNoRoutes;
  /**<   IP RX packets dropped because of no route
  */

  uint64_t inAddrErrors;
  /**<   IP RX packets dropped because of invalid address
  */

  uint64_t inDiscards;
  /**<   IP RX packets dropped
  */

  uint64_t outDiscards;
  /**<   IP RX packets dropped
  */
}ipa_dped_data_lnx_ip_stats_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_dped_qmi_aggregates
    @{
  */
typedef struct {

  uint64_t inErrors;
  /**<   ICMP RX messages with format errors
  */

  uint64_t outErrors;
  /**<   ICMP TX messages with format errors
  */

  uint64_t inCsumErrors;
  /**<   ICMP RX messages with bad ICMP checksums
  */
}ipa_dped_data_lnx_icmp_stats_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_dped_qmi_aggregates
    @{
  */
typedef struct {

  uint64_t inErrs;
  /**<   TCP RX packets with errors
  */

  uint64_t inCsumErrors;
  /**<   TCP RX packets with checksum errors
  */

  uint64_t TCPOFOQueue;
  /**<   TCP RX packets arriving out of order and queued
  */

  uint64_t TCPOFODrop;
  /**<   TCP RX packets arriving out of order and dropped
  */

  uint64_t NumDupAcks;
  /**<   Number of Dup Acks transmitted
  */

  uint64_t NumRetrans;
  /**<   Number of Retransmissions
  */
}ipa_dped_data_lnx_tcp_stats_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_dped_qmi_aggregates
    @{
  */
typedef struct {

  uint64_t inErrors;
  /**<   UDP RX packets with format errors
  */

  uint64_t RcvbufErrors;
  /**<   UDP RX packets dropped because no receive buffer space
  */

  uint64_t SndbufErrors;
  /**<   UDP TC packets dropped because no send buffer space
  */

  uint64_t inCsumErrors;
  /**<   UDP RX packets with bad UDP checksums
  */
}ipa_dped_data_lnx_udp_stats_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_dped_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t ipv4_addr;
  /**<   IPv4 address of the RMNET Data interface
  */

  uint8_t ipv6_addr[QMI_IPA_DPED_DATA_LNX_IPV6_SIZE_V01];
  /**<   IPv6 address of the RMNET Data interface
  */

  uint64_t coal_csum_errors;
  /**<   Number of packets with checksum errors in QMAP coalescing frames
  */

  uint64_t coal_buff_util;
  /**<   Total coalescing channel buffer utilization (number of buffers utilized)
  */

  uint64_t rsc_buff_util;
  /**<   RSC buffer utilization
  */

  uint64_t rsb_buff_util;
  /**<   RSB buffer utilization
  */
}ipa_dped_data_lnx_rmnet_data_stats_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_dped_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t ip_stats_len;  /**< Must be set to # of elements in ip_stats */
  ipa_dped_data_lnx_ip_stats_type_v01 ip_stats[QMI_IPA_DPED_DATA_LNX_STATS_MAX_V01];
  /**<   IP stats
  */

  uint32_t icmp_stats_len;  /**< Must be set to # of elements in icmp_stats */
  ipa_dped_data_lnx_icmp_stats_type_v01 icmp_stats[QMI_IPA_DPED_DATA_LNX_STATS_MAX_V01];
  /**<   ICMP stats
  */

  uint32_t tcp_stats_len;  /**< Must be set to # of elements in tcp_stats */
  ipa_dped_data_lnx_tcp_stats_type_v01 tcp_stats[QMI_IPA_DPED_DATA_LNX_TCP_STATS_MAX_V01];
  /**<   TCP stats
  */

  uint32_t udp_stats_len;  /**< Must be set to # of elements in udp_stats */
  ipa_dped_data_lnx_udp_stats_type_v01 udp_stats[QMI_IPA_DPED_DATA_LNX_STATS_MAX_V01];
  /**<   UDP stats
  */

  uint32_t rmnet_data_stats_len;  /**< Must be set to # of elements in rmnet_data_stats */
  ipa_dped_data_lnx_rmnet_data_stats_type_v01 rmnet_data_stats[QMI_IPA_DPED_DATA_LNX_RMNET_STATS_MAX_V01];
  /**<   RMNET Data stats
  */
}ipa_dped_data_lnx_stats_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_dped_qmi_aggregates
    @{
  */
typedef struct {

  uint64_t coal_total_repl_buff;
  /**<   Total number of buffers replenished for coalescing pipe
  */

  uint64_t coal_temp_repl_buff;
  /**<   Total number buffers replenished from temporary cache for coalescing pipe
  */

  uint64_t def_total_repl_buff;
  /**<   Total number of buffers replenished for default pipe
  */

  uint64_t def_temp_repl_buff;
  /**<   Total number of buffers replenished from temporary cache for default pipe
  */
}ipa_dped_ipa_lnx_page_recyc_stats_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_dped_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t excep_type_none;
  /**<   Number of packets with exception type as None
  */

  uint32_t excep_type_deaggr;
  /**<   Number of packets with exception type as Deaggregation
  */

  uint32_t excep_type_iptype;
  /**<   Number of packets with exception type as IP Type
  */

  uint32_t excep_type_pkt_len;
  /**<   Number of packets with exception type as Packet Length
  */

  uint32_t excep_type_pkt_thresh;
  /**<   Number of packets with exception type as Packet Threshold
  */

  uint32_t excep_type_fra_rule_miss;
  /**<   Number of packets with exception type as Fragmentation Rule Miss
  */

  uint32_t excep_type_sw_flt;
  /**<   Number of packets with exception type as SW filter
  */

  uint32_t excep_type_nat;
  /**<   Number of packets with exception type as NAT
  */

  uint32_t excep_type_ipv6ct;
  /**<   Number of packets with exception type as Ipv6 Connection tracks
  */

  uint32_t excep_type_chksum;
  /**<   Number of packets with exception type as checksum
  */
}ipa_dped_ipa_lnx_excep_stats_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_dped_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t client_type;
  /**<   IPA client type
  */

  uint32_t num_drop_cnt;
  /**<   Total number of dropped packets
  */

  uint32_t num_drop_bytes;
  /**<   Total number of dropped bytes
  */

  uint32_t reserved;
  /**<   Reserved for debugging purposes
  */
}ipa_dped_ipa_lnx_holb_discard_stats_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_dped_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t client_type;
  /**<   IPA client type
  */

  uint32_t curr_indx;
  /**<   Current HOLB monitoring index
  */

  uint32_t num_enable_cnt;
  /**<   Number of times peripheral went to bad state
  */

  uint32_t num_disable_cnt;
  /**<   Number of times peripheral went to recovered
  */
}ipa_dped_ipa_lnx_holb_monitor_stats_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_dped_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t wan_rx_empty;
  /**<   Number of times WAN Consumer/Coalescing pipes have buffers less than threshold of 32
  */

  uint32_t wan_repl_rx_empty;
  /**<   Number of times there are no pages in temporary cache for WAN pipe
  */

  uint32_t lan_rx_empty;
  /**<   Number of times LAN Consumer pipe has buffers less than threshold of 32
  */

  uint32_t curr_clk_vote;
  /**<   Current active clock vote
  */

  ipa_dped_ipa_lnx_page_recyc_stats_type_v01 page_recyc_stats;
  /**<   Page Recycling Stats
  */

  ipa_dped_ipa_lnx_excep_stats_type_v01 excep_stats;
  /**<   Exception Stats
  */

  uint32_t holb_discard_stats_len;  /**< Must be set to # of elements in holb_discard_stats */
  ipa_dped_ipa_lnx_holb_discard_stats_type_v01 holb_discard_stats[QMI_IPA_DPED_IPA_LNX_HOLB_NUM_CLIENTS_MAX_V01];
  /**<   HOLB Discard statistics
  */

  uint32_t holb_monitor_stats_len;  /**< Must be set to # of elements in holb_monitor_stats */
  ipa_dped_ipa_lnx_holb_monitor_stats_type_v01 holb_monitor_stats[QMI_IPA_DPED_IPA_LNX_HOLB_NUM_MON_CLIENTS_MAX_V01];
  /**<   HOLB Monitoring Stats
  */
}ipa_dped_ipa_lnx_stats_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_dped_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t tx_client;
  /**<   Client type of the TX instance
  */

  uint32_t num_tx_ring_100_perc_with_cred;
  /**<   Total number of times the ring is full of free credits.
  */

  uint32_t num_tx_ring_0_perc_with_cred;
  /**<   Total number of times the ring has empty credits
  */

  uint32_t num_tx_ring_above_75_perc_cred;
  /**<   Total number of times ring has > 75% free credits.
  */

  uint32_t num_tx_ring_above_25_perc_cred;
  /**<   Total number of times ring has < 25% of free credits.
  */

  uint32_t num_tx_ring_stats_polled;
  /**<   Total number of times TX ring stats are counted.
  */

  uint32_t num_tx_oob;
  /**<   Number of times GSI encountered OOB.
  */

  uint32_t num_tx_oob_time;
  /**<   Total time GSI was in OOB state i.e no credits available.
  */

  uint32_t gsi_debug1;
  /**<   Additional GSI Debug information
  */

  uint32_t gsi_debug2;
  /**<   Additional GSI Debug information
  */

  uint32_t gsi_debug3;
  /**<   Additional GSI Debug information
  */

  uint32_t gsi_debug4;
  /**<   Additional GSI Debug information
  */

  uint32_t tx_summary;
  /**<   1 - Peripheral is bad in replenishing credits

       2 - IPA is not giving packets fast enough
  */

  uint32_t reserved;
  /**<   Reserved for debugging purposes
  */
}ipa_dped_gsi_tx_dbg_stats_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_dped_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t rx_client;
  /**<   Client type of the RX instance
  */

  uint32_t num_rx_ring_100_perc_with_pack;
  /**<   Total number of times the ring is full of packets.
  */

  uint32_t num_rx_ring_0_perc_with_pack;
  /**<   Total number of times the ring has 0 packets.
  */

  uint32_t num_rx_ring_above_75_perc_pack;
  /**<   Total number of times ring has > 75% packets.
  */

  uint32_t num_rx_ring_above_25_perc_pack;
  /**<   Total number of times ring has < 25% of packets.
  */

  uint32_t num_rx_ring_stats_polled;
  /**<   Total number of times RX ring stats are counted.
  */

  uint32_t num_rx_drop_stats;
  /**<   Total number of times GSI dropped packets in RX
  */

  uint32_t gsi_debug1;
  /**<   Additional GSI Debug information
  */

  uint32_t gsi_debug2;
  /**<   Additional GSI Debug information
  */

  uint32_t gsi_debug3;
  /**<   Additional GSI Debug information
  */

  uint32_t gsi_debug4;
  /**<   Additional GSI Debug information
  */

  uint32_t rx_summary;
  /**<   1 - Peripheral is bad in providing packets

       2 - IPA is not processing packets fast enough
  */
}ipa_dped_gsi_rx_dbg_stats_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_dped_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t instance_id;
  /**<   Instance ID of the peripheral
  */

  uint32_t gsi_tx_dbg_stats_len;  /**< Must be set to # of elements in gsi_tx_dbg_stats */
  ipa_dped_gsi_tx_dbg_stats_type_v01 gsi_tx_dbg_stats[QMI_IPA_DPED_GSI_DBG_NUM_TX_INST_MAX_V01];
  /**<   GSI TX Debug Statistics Info
  */

  uint32_t gsi_rx_dbg_stats_len;  /**< Must be set to # of elements in gsi_rx_dbg_stats */
  ipa_dped_gsi_rx_dbg_stats_type_v01 gsi_rx_dbg_stats[QMI_IPA_DPED_GSI_DBG_NUM_RX_INST_MAX_V01];
  /**<   GSI RX Debug Statistics Info
  */
}ipa_dped_gsi_dbg_stats_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_dped_qmi_messages
    @{
  */
/** Request Message; AP sends current E2E statistics to modem using this request message */
typedef struct {

  /* Mandatory */
  /*  abm_log_seq_id */
  uint32_t abm_log_seq_id;
  /**<   Active Burst Management (ABM) Log Sequence ID
  */

  /* Mandatory */
  /*  is_final_stats */
  uint8_t is_final_stats;
  /**<    Indicates if this is the final statistics for current ABM log seq id transaction 
        0  -  This is the not the final statistics message and more messages to follow
        1 -  This is the final statistics message and no more messages follows
  */

  /* Optional */
  /*  data_eth_stats */
  uint8_t data_eth_stats_valid;  /**< Must be set to true if data_eth_stats is being passed */
  ipa_dped_data_eth_stats_type_v01 data_eth_stats;
  /**<   Data Ethernet statistics
  */

  /* Optional */
  /*  data_lnx_stats */
  uint8_t data_lnx_stats_valid;  /**< Must be set to true if data_lnx_stats is being passed */
  ipa_dped_data_lnx_stats_type_v01 data_lnx_stats;
  /**<   Data Linux statistics
  */

  /* Optional */
  /*  ipa_lnx_stats */
  uint8_t ipa_lnx_stats_valid;  /**< Must be set to true if ipa_lnx_stats is being passed */
  ipa_dped_ipa_lnx_stats_type_v01 ipa_lnx_stats;
  /**<   IPA Linux statistics
  */

  /* Optional */
  /*  wlan_gsi_dbg_stats */
  uint8_t wlan_gsi_dbg_stats_valid;  /**< Must be set to true if wlan_gsi_dbg_stats is being passed */
  uint32_t wlan_gsi_dbg_stats_len;  /**< Must be set to # of elements in wlan_gsi_dbg_stats */
  ipa_dped_gsi_dbg_stats_type_v01 wlan_gsi_dbg_stats[QMI_IPA_DPED_GSI_NUM_INST_MAX_V01];
  /**<   WLAN GSI Debug statistics
  */

  /* Optional */
  /*  eth_gsi_dbg_stats */
  uint8_t eth_gsi_dbg_stats_valid;  /**< Must be set to true if eth_gsi_dbg_stats is being passed */
  uint32_t eth_gsi_dbg_stats_len;  /**< Must be set to # of elements in eth_gsi_dbg_stats */
  ipa_dped_gsi_dbg_stats_type_v01 eth_gsi_dbg_stats[QMI_IPA_DPED_GSI_NUM_INST_MAX_V01];
  /**<   Ethernet GSI Debug statistics
  */

  /* Optional */
  /*  usb_gsi_dbg_stats */
  uint8_t usb_gsi_dbg_stats_valid;  /**< Must be set to true if usb_gsi_dbg_stats is being passed */
  uint32_t usb_gsi_dbg_stats_len;  /**< Must be set to # of elements in usb_gsi_dbg_stats */
  ipa_dped_gsi_dbg_stats_type_v01 usb_gsi_dbg_stats[QMI_IPA_DPED_GSI_NUM_INST_MAX_V01];
  /**<   USB GSI Debug statistics
  */

  /* Optional */
  /*  mhip_gsi_dbg_stats */
  uint8_t mhip_gsi_dbg_stats_valid;  /**< Must be set to true if mhip_gsi_dbg_stats is being passed */
  uint32_t mhip_gsi_dbg_stats_len;  /**< Must be set to # of elements in mhip_gsi_dbg_stats */
  ipa_dped_gsi_dbg_stats_type_v01 mhip_gsi_dbg_stats[QMI_IPA_DPED_GSI_NUM_INST_MAX_V01];
  /**<   MHIP GSI Debug statistics
  */
}ipa_dped_send_e2e_stats_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_dped_qmi_messages
    @{
  */
/** Response Message; AP sends current E2E statistics to modem using this request message */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.
 Standard response type. Contains the following data members: \n
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
     - qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message definition.
  */
}ipa_dped_send_e2e_stats_resp_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 
//#define REMOVE_QMI_IPA_DPED_INDICATION_REGISTER_V01 
//#define REMOVE_QMI_IPA_DPED_SEND_E2E_STATS_V01 
//#define REMOVE_QMI_IPA_DPED_TRIGGER_E2E_STATS_IND_V01 

/*Service Message Definition*/
/** @addtogroup ipa_dped_qmi_msg_ids
    @{
  */
#define QMI_IPA_DPED_INDICATION_REGISTER_REQ_V01 0x0001
#define QMI_IPA_DPED_INDICATION_REGISTER_RESP_V01 0x0001
#define QMI_IPA_DPED_TRIGGER_E2E_STATS_IND_V01 0x0020
#define QMI_IPA_DPED_SEND_E2E_STATS_REQ_MSG_V01 0x0021
#define QMI_IPA_DPED_SEND_E2E_STATS_RESP_MSG_V01 0x0021
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro ipa_dped_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type ipa_dped_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define ipa_dped_get_service_object_v01( ) \
          ipa_dped_get_service_object_internal_v01( \
            IPA_DPED_V01_IDL_MAJOR_VERS, IPA_DPED_V01_IDL_MINOR_VERS, \
            IPA_DPED_V01_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif

