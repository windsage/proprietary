/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        I P A _ D A T A P A T H _ E 2 E _ D E B U G _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the ipa_dped service Data structures.

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

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "ipa_datapath_e2e_debug_v01.h"
#include "common_v01.h"


/*Type Definitions*/
static const uint8_t ipa_dped_data_eth_pkt_stats_inst_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_data_eth_pkt_stats_inst_type_v01, pkts),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_data_eth_pkt_stats_inst_type_v01, bytes),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_data_eth_pkt_stats_inst_type_v01, errors),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_data_eth_pkt_stats_inst_type_v01, drops),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_data_eth_pkt_stats_inst_type_v01, reserved_1),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_data_eth_pkt_stats_inst_type_v01, reserved_2),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_dped_data_eth_pkt_stats_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_data_eth_pkt_stats_type_v01, stats_type),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_dped_data_eth_pkt_stats_type_v01, tx_stats),
  QMI_IDL_TYPE88(0, 0),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_dped_data_eth_pkt_stats_type_v01, rx_stats),
  QMI_IDL_TYPE88(0, 0),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_dped_data_eth_emac_addl_inst_stats_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_data_eth_emac_addl_inst_stats_type_v01, pause_frames),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_data_eth_emac_addl_inst_stats_type_v01, desc_unavail),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_dped_data_eth_emac_addl_stats_type_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_dped_data_eth_emac_addl_stats_type_v01, tx_stats),
  QMI_IDL_TYPE88(0, 2),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_dped_data_eth_emac_addl_stats_type_v01, rx_stats),
  QMI_IDL_TYPE88(0, 2),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_dped_data_eth_ring_inst_stats_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_data_eth_ring_inst_stats_type_v01, dma_channel_status),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_data_eth_ring_inst_stats_type_v01, head_ptr),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_data_eth_ring_inst_stats_type_v01, tail_ptr),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_data_eth_ring_inst_stats_type_v01, overflow_error_cnt),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_data_eth_ring_inst_stats_type_v01, underflow_error_cnt),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_dped_data_eth_ring_stats_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_data_eth_ring_stats_type_v01, ring_index),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_data_eth_ring_stats_type_v01, offload),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_dped_data_eth_ring_stats_type_v01, tx_ring_stats),
  QMI_IDL_TYPE88(0, 4),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_dped_data_eth_ring_stats_type_v01, rx_ring_stats),
  QMI_IDL_TYPE88(0, 4),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_dped_data_eth_stats_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_data_eth_stats_type_v01, iface_idx),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_dped_data_eth_stats_type_v01, pkt_stats),
  QMI_IPA_DPED_DATA_ETH_PKT_STATS_MAX_V01,
  QMI_IDL_OFFSET8(ipa_dped_data_eth_stats_type_v01, pkt_stats) - QMI_IDL_OFFSET8(ipa_dped_data_eth_stats_type_v01, pkt_stats_len),
  QMI_IDL_TYPE88(0, 1),
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(ipa_dped_data_eth_stats_type_v01, emac_addl_stats),
  QMI_IDL_TYPE88(0, 3),
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(ipa_dped_data_eth_stats_type_v01, ring_stats),
  QMI_IPA_DPED_DATA_ETH_RING_STATS_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(ipa_dped_data_eth_stats_type_v01, ring_stats) - QMI_IDL_OFFSET16RELATIVE(ipa_dped_data_eth_stats_type_v01, ring_stats_len),
  QMI_IDL_TYPE88(0, 5),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_dped_data_lnx_ip_stats_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_data_lnx_ip_stats_type_v01, inHdrErrors),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_data_lnx_ip_stats_type_v01, inTooBigErrors),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_data_lnx_ip_stats_type_v01, inNoRoutes),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_data_lnx_ip_stats_type_v01, inAddrErrors),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_data_lnx_ip_stats_type_v01, inDiscards),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_data_lnx_ip_stats_type_v01, outDiscards),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_dped_data_lnx_icmp_stats_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_data_lnx_icmp_stats_type_v01, inErrors),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_data_lnx_icmp_stats_type_v01, outErrors),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_data_lnx_icmp_stats_type_v01, inCsumErrors),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_dped_data_lnx_tcp_stats_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_data_lnx_tcp_stats_type_v01, inErrs),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_data_lnx_tcp_stats_type_v01, inCsumErrors),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_data_lnx_tcp_stats_type_v01, TCPOFOQueue),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_data_lnx_tcp_stats_type_v01, TCPOFODrop),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_data_lnx_tcp_stats_type_v01, NumDupAcks),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_data_lnx_tcp_stats_type_v01, NumRetrans),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_dped_data_lnx_udp_stats_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_data_lnx_udp_stats_type_v01, inErrors),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_data_lnx_udp_stats_type_v01, RcvbufErrors),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_data_lnx_udp_stats_type_v01, SndbufErrors),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_data_lnx_udp_stats_type_v01, inCsumErrors),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_dped_data_lnx_rmnet_data_stats_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_data_lnx_rmnet_data_stats_type_v01, ipv4_addr),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_data_lnx_rmnet_data_stats_type_v01, ipv6_addr),
  QMI_IPA_DPED_DATA_LNX_IPV6_SIZE_V01,

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_data_lnx_rmnet_data_stats_type_v01, coal_csum_errors),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_data_lnx_rmnet_data_stats_type_v01, coal_buff_util),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_data_lnx_rmnet_data_stats_type_v01, rsc_buff_util),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_data_lnx_rmnet_data_stats_type_v01, rsb_buff_util),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_dped_data_lnx_stats_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_dped_data_lnx_stats_type_v01, ip_stats),
  QMI_IPA_DPED_DATA_LNX_STATS_MAX_V01,
  QMI_IDL_OFFSET8(ipa_dped_data_lnx_stats_type_v01, ip_stats) - QMI_IDL_OFFSET8(ipa_dped_data_lnx_stats_type_v01, ip_stats_len),
  QMI_IDL_TYPE88(0, 7),
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_dped_data_lnx_stats_type_v01, icmp_stats),
  QMI_IPA_DPED_DATA_LNX_STATS_MAX_V01,
  QMI_IDL_OFFSET8(ipa_dped_data_lnx_stats_type_v01, icmp_stats) - QMI_IDL_OFFSET8(ipa_dped_data_lnx_stats_type_v01, icmp_stats_len),
  QMI_IDL_TYPE88(0, 8),
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_dped_data_lnx_stats_type_v01, tcp_stats),
  QMI_IPA_DPED_DATA_LNX_TCP_STATS_MAX_V01,
  QMI_IDL_OFFSET8(ipa_dped_data_lnx_stats_type_v01, tcp_stats) - QMI_IDL_OFFSET8(ipa_dped_data_lnx_stats_type_v01, tcp_stats_len),
  QMI_IDL_TYPE88(0, 9),
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(ipa_dped_data_lnx_stats_type_v01, udp_stats),
  QMI_IPA_DPED_DATA_LNX_STATS_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(ipa_dped_data_lnx_stats_type_v01, udp_stats) - QMI_IDL_OFFSET16RELATIVE(ipa_dped_data_lnx_stats_type_v01, udp_stats_len),
  QMI_IDL_TYPE88(0, 10),
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(ipa_dped_data_lnx_stats_type_v01, rmnet_data_stats),
  QMI_IPA_DPED_DATA_LNX_RMNET_STATS_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(ipa_dped_data_lnx_stats_type_v01, rmnet_data_stats) - QMI_IDL_OFFSET16RELATIVE(ipa_dped_data_lnx_stats_type_v01, rmnet_data_stats_len),
  QMI_IDL_TYPE88(0, 11),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_dped_ipa_lnx_page_recyc_stats_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_ipa_lnx_page_recyc_stats_type_v01, coal_total_repl_buff),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_ipa_lnx_page_recyc_stats_type_v01, coal_temp_repl_buff),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_ipa_lnx_page_recyc_stats_type_v01, def_total_repl_buff),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_ipa_lnx_page_recyc_stats_type_v01, def_temp_repl_buff),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_dped_ipa_lnx_excep_stats_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_ipa_lnx_excep_stats_type_v01, excep_type_none),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_ipa_lnx_excep_stats_type_v01, excep_type_deaggr),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_ipa_lnx_excep_stats_type_v01, excep_type_iptype),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_ipa_lnx_excep_stats_type_v01, excep_type_pkt_len),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_ipa_lnx_excep_stats_type_v01, excep_type_pkt_thresh),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_ipa_lnx_excep_stats_type_v01, excep_type_fra_rule_miss),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_ipa_lnx_excep_stats_type_v01, excep_type_sw_flt),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_ipa_lnx_excep_stats_type_v01, excep_type_nat),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_ipa_lnx_excep_stats_type_v01, excep_type_ipv6ct),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_ipa_lnx_excep_stats_type_v01, excep_type_chksum),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_dped_ipa_lnx_holb_discard_stats_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_ipa_lnx_holb_discard_stats_type_v01, client_type),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_ipa_lnx_holb_discard_stats_type_v01, num_drop_cnt),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_ipa_lnx_holb_discard_stats_type_v01, num_drop_bytes),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_ipa_lnx_holb_discard_stats_type_v01, reserved),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_dped_ipa_lnx_holb_monitor_stats_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_ipa_lnx_holb_monitor_stats_type_v01, client_type),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_ipa_lnx_holb_monitor_stats_type_v01, curr_indx),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_ipa_lnx_holb_monitor_stats_type_v01, num_enable_cnt),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_ipa_lnx_holb_monitor_stats_type_v01, num_disable_cnt),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_dped_ipa_lnx_stats_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_ipa_lnx_stats_type_v01, wan_rx_empty),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_ipa_lnx_stats_type_v01, wan_repl_rx_empty),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_ipa_lnx_stats_type_v01, lan_rx_empty),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_ipa_lnx_stats_type_v01, curr_clk_vote),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_dped_ipa_lnx_stats_type_v01, page_recyc_stats),
  QMI_IDL_TYPE88(0, 13),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_dped_ipa_lnx_stats_type_v01, excep_stats),
  QMI_IDL_TYPE88(0, 14),
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_dped_ipa_lnx_stats_type_v01, holb_discard_stats),
  QMI_IPA_DPED_IPA_LNX_HOLB_NUM_CLIENTS_MAX_V01,
  QMI_IDL_OFFSET8(ipa_dped_ipa_lnx_stats_type_v01, holb_discard_stats) - QMI_IDL_OFFSET8(ipa_dped_ipa_lnx_stats_type_v01, holb_discard_stats_len),
  QMI_IDL_TYPE88(0, 15),
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_dped_ipa_lnx_stats_type_v01, holb_monitor_stats),
  QMI_IPA_DPED_IPA_LNX_HOLB_NUM_MON_CLIENTS_MAX_V01,
  QMI_IDL_OFFSET8(ipa_dped_ipa_lnx_stats_type_v01, holb_monitor_stats) - QMI_IDL_OFFSET8(ipa_dped_ipa_lnx_stats_type_v01, holb_monitor_stats_len),
  QMI_IDL_TYPE88(0, 16),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_dped_gsi_tx_dbg_stats_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_gsi_tx_dbg_stats_type_v01, tx_client),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_gsi_tx_dbg_stats_type_v01, num_tx_ring_100_perc_with_cred),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_gsi_tx_dbg_stats_type_v01, num_tx_ring_0_perc_with_cred),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_gsi_tx_dbg_stats_type_v01, num_tx_ring_above_75_perc_cred),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_gsi_tx_dbg_stats_type_v01, num_tx_ring_above_25_perc_cred),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_gsi_tx_dbg_stats_type_v01, num_tx_ring_stats_polled),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_gsi_tx_dbg_stats_type_v01, num_tx_oob),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_gsi_tx_dbg_stats_type_v01, num_tx_oob_time),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_gsi_tx_dbg_stats_type_v01, gsi_debug1),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_gsi_tx_dbg_stats_type_v01, gsi_debug2),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_gsi_tx_dbg_stats_type_v01, gsi_debug3),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_gsi_tx_dbg_stats_type_v01, gsi_debug4),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_gsi_tx_dbg_stats_type_v01, tx_summary),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_gsi_tx_dbg_stats_type_v01, reserved),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_dped_gsi_rx_dbg_stats_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_gsi_rx_dbg_stats_type_v01, rx_client),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_gsi_rx_dbg_stats_type_v01, num_rx_ring_100_perc_with_pack),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_gsi_rx_dbg_stats_type_v01, num_rx_ring_0_perc_with_pack),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_gsi_rx_dbg_stats_type_v01, num_rx_ring_above_75_perc_pack),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_gsi_rx_dbg_stats_type_v01, num_rx_ring_above_25_perc_pack),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_gsi_rx_dbg_stats_type_v01, num_rx_ring_stats_polled),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_gsi_rx_dbg_stats_type_v01, num_rx_drop_stats),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_gsi_rx_dbg_stats_type_v01, gsi_debug1),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_gsi_rx_dbg_stats_type_v01, gsi_debug2),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_gsi_rx_dbg_stats_type_v01, gsi_debug3),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_gsi_rx_dbg_stats_type_v01, gsi_debug4),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_gsi_rx_dbg_stats_type_v01, rx_summary),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_dped_gsi_dbg_stats_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_gsi_dbg_stats_type_v01, instance_id),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_dped_gsi_dbg_stats_type_v01, gsi_tx_dbg_stats),
  QMI_IPA_DPED_GSI_DBG_NUM_TX_INST_MAX_V01,
  QMI_IDL_OFFSET8(ipa_dped_gsi_dbg_stats_type_v01, gsi_tx_dbg_stats) - QMI_IDL_OFFSET8(ipa_dped_gsi_dbg_stats_type_v01, gsi_tx_dbg_stats_len),
  QMI_IDL_TYPE88(0, 18),
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_dped_gsi_dbg_stats_type_v01, gsi_rx_dbg_stats),
  QMI_IPA_DPED_GSI_DBG_NUM_RX_INST_MAX_V01,
  QMI_IDL_OFFSET8(ipa_dped_gsi_dbg_stats_type_v01, gsi_rx_dbg_stats) - QMI_IDL_OFFSET8(ipa_dped_gsi_dbg_stats_type_v01, gsi_rx_dbg_stats_len),
  QMI_IDL_TYPE88(0, 19),
  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
static const uint8_t ipa_dped_indication_reg_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_dped_indication_reg_req_msg_v01, trigger_e2e_stats) - QMI_IDL_OFFSET8(ipa_dped_indication_reg_req_msg_v01, trigger_e2e_stats_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_indication_reg_req_msg_v01, trigger_e2e_stats)
};

static const uint8_t ipa_dped_indication_reg_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_dped_indication_reg_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t ipa_dped_trigger_e2e_stats_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_dped_trigger_e2e_stats_ind_msg_v01, log_seq_id) - QMI_IDL_OFFSET8(ipa_dped_trigger_e2e_stats_ind_msg_v01, log_seq_id_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_trigger_e2e_stats_ind_msg_v01, log_seq_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_dped_trigger_e2e_stats_ind_msg_v01, dl_burst_size) - QMI_IDL_OFFSET8(ipa_dped_trigger_e2e_stats_ind_msg_v01, dl_burst_size_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_trigger_e2e_stats_ind_msg_v01, dl_burst_size),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_dped_trigger_e2e_stats_ind_msg_v01, vstmr_ticks_xo) - QMI_IDL_OFFSET8(ipa_dped_trigger_e2e_stats_ind_msg_v01, vstmr_ticks_xo_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_trigger_e2e_stats_ind_msg_v01, vstmr_ticks_xo)
};

static const uint8_t ipa_dped_send_e2e_stats_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_send_e2e_stats_req_msg_v01, abm_log_seq_id),

  0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_dped_send_e2e_stats_req_msg_v01, is_final_stats),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_dped_send_e2e_stats_req_msg_v01, data_eth_stats) - QMI_IDL_OFFSET8(ipa_dped_send_e2e_stats_req_msg_v01, data_eth_stats_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_dped_send_e2e_stats_req_msg_v01, data_eth_stats),
  QMI_IDL_TYPE88(0, 6),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_dped_send_e2e_stats_req_msg_v01, data_lnx_stats) - QMI_IDL_OFFSET16RELATIVE(ipa_dped_send_e2e_stats_req_msg_v01, data_lnx_stats_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(ipa_dped_send_e2e_stats_req_msg_v01, data_lnx_stats),
  QMI_IDL_TYPE88(0, 12),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_dped_send_e2e_stats_req_msg_v01, ipa_lnx_stats) - QMI_IDL_OFFSET16RELATIVE(ipa_dped_send_e2e_stats_req_msg_v01, ipa_lnx_stats_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(ipa_dped_send_e2e_stats_req_msg_v01, ipa_lnx_stats),
  QMI_IDL_TYPE88(0, 17),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_dped_send_e2e_stats_req_msg_v01, wlan_gsi_dbg_stats) - QMI_IDL_OFFSET16RELATIVE(ipa_dped_send_e2e_stats_req_msg_v01, wlan_gsi_dbg_stats_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(ipa_dped_send_e2e_stats_req_msg_v01, wlan_gsi_dbg_stats),
  QMI_IPA_DPED_GSI_NUM_INST_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(ipa_dped_send_e2e_stats_req_msg_v01, wlan_gsi_dbg_stats) - QMI_IDL_OFFSET16RELATIVE(ipa_dped_send_e2e_stats_req_msg_v01, wlan_gsi_dbg_stats_len),
  QMI_IDL_TYPE88(0, 20),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_dped_send_e2e_stats_req_msg_v01, eth_gsi_dbg_stats) - QMI_IDL_OFFSET16RELATIVE(ipa_dped_send_e2e_stats_req_msg_v01, eth_gsi_dbg_stats_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(ipa_dped_send_e2e_stats_req_msg_v01, eth_gsi_dbg_stats),
  QMI_IPA_DPED_GSI_NUM_INST_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(ipa_dped_send_e2e_stats_req_msg_v01, eth_gsi_dbg_stats) - QMI_IDL_OFFSET16RELATIVE(ipa_dped_send_e2e_stats_req_msg_v01, eth_gsi_dbg_stats_len),
  QMI_IDL_TYPE88(0, 20),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_dped_send_e2e_stats_req_msg_v01, usb_gsi_dbg_stats) - QMI_IDL_OFFSET16RELATIVE(ipa_dped_send_e2e_stats_req_msg_v01, usb_gsi_dbg_stats_valid)),
  0x15,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(ipa_dped_send_e2e_stats_req_msg_v01, usb_gsi_dbg_stats),
  QMI_IPA_DPED_GSI_NUM_INST_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(ipa_dped_send_e2e_stats_req_msg_v01, usb_gsi_dbg_stats) - QMI_IDL_OFFSET16RELATIVE(ipa_dped_send_e2e_stats_req_msg_v01, usb_gsi_dbg_stats_len),
  QMI_IDL_TYPE88(0, 20),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_dped_send_e2e_stats_req_msg_v01, mhip_gsi_dbg_stats) - QMI_IDL_OFFSET16RELATIVE(ipa_dped_send_e2e_stats_req_msg_v01, mhip_gsi_dbg_stats_valid)),
  0x16,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(ipa_dped_send_e2e_stats_req_msg_v01, mhip_gsi_dbg_stats),
  QMI_IPA_DPED_GSI_NUM_INST_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(ipa_dped_send_e2e_stats_req_msg_v01, mhip_gsi_dbg_stats) - QMI_IDL_OFFSET16RELATIVE(ipa_dped_send_e2e_stats_req_msg_v01, mhip_gsi_dbg_stats_len),
  QMI_IDL_TYPE88(0, 20)
};

static const uint8_t ipa_dped_send_e2e_stats_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_dped_send_e2e_stats_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/* Type Table */
static const qmi_idl_type_table_entry  ipa_dped_type_table_v01[] = {
  {sizeof(ipa_dped_data_eth_pkt_stats_inst_type_v01), ipa_dped_data_eth_pkt_stats_inst_type_data_v01},
  {sizeof(ipa_dped_data_eth_pkt_stats_type_v01), ipa_dped_data_eth_pkt_stats_type_data_v01},
  {sizeof(ipa_dped_data_eth_emac_addl_inst_stats_type_v01), ipa_dped_data_eth_emac_addl_inst_stats_type_data_v01},
  {sizeof(ipa_dped_data_eth_emac_addl_stats_type_v01), ipa_dped_data_eth_emac_addl_stats_type_data_v01},
  {sizeof(ipa_dped_data_eth_ring_inst_stats_type_v01), ipa_dped_data_eth_ring_inst_stats_type_data_v01},
  {sizeof(ipa_dped_data_eth_ring_stats_type_v01), ipa_dped_data_eth_ring_stats_type_data_v01},
  {sizeof(ipa_dped_data_eth_stats_type_v01), ipa_dped_data_eth_stats_type_data_v01},
  {sizeof(ipa_dped_data_lnx_ip_stats_type_v01), ipa_dped_data_lnx_ip_stats_type_data_v01},
  {sizeof(ipa_dped_data_lnx_icmp_stats_type_v01), ipa_dped_data_lnx_icmp_stats_type_data_v01},
  {sizeof(ipa_dped_data_lnx_tcp_stats_type_v01), ipa_dped_data_lnx_tcp_stats_type_data_v01},
  {sizeof(ipa_dped_data_lnx_udp_stats_type_v01), ipa_dped_data_lnx_udp_stats_type_data_v01},
  {sizeof(ipa_dped_data_lnx_rmnet_data_stats_type_v01), ipa_dped_data_lnx_rmnet_data_stats_type_data_v01},
  {sizeof(ipa_dped_data_lnx_stats_type_v01), ipa_dped_data_lnx_stats_type_data_v01},
  {sizeof(ipa_dped_ipa_lnx_page_recyc_stats_type_v01), ipa_dped_ipa_lnx_page_recyc_stats_type_data_v01},
  {sizeof(ipa_dped_ipa_lnx_excep_stats_type_v01), ipa_dped_ipa_lnx_excep_stats_type_data_v01},
  {sizeof(ipa_dped_ipa_lnx_holb_discard_stats_type_v01), ipa_dped_ipa_lnx_holb_discard_stats_type_data_v01},
  {sizeof(ipa_dped_ipa_lnx_holb_monitor_stats_type_v01), ipa_dped_ipa_lnx_holb_monitor_stats_type_data_v01},
  {sizeof(ipa_dped_ipa_lnx_stats_type_v01), ipa_dped_ipa_lnx_stats_type_data_v01},
  {sizeof(ipa_dped_gsi_tx_dbg_stats_type_v01), ipa_dped_gsi_tx_dbg_stats_type_data_v01},
  {sizeof(ipa_dped_gsi_rx_dbg_stats_type_v01), ipa_dped_gsi_rx_dbg_stats_type_data_v01},
  {sizeof(ipa_dped_gsi_dbg_stats_type_v01), ipa_dped_gsi_dbg_stats_type_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry ipa_dped_message_table_v01[] = {
  {sizeof(ipa_dped_indication_reg_req_msg_v01), ipa_dped_indication_reg_req_msg_data_v01},
  {sizeof(ipa_dped_indication_reg_resp_msg_v01), ipa_dped_indication_reg_resp_msg_data_v01},
  {sizeof(ipa_dped_trigger_e2e_stats_ind_msg_v01), ipa_dped_trigger_e2e_stats_ind_msg_data_v01},
  {sizeof(ipa_dped_send_e2e_stats_req_msg_v01), ipa_dped_send_e2e_stats_req_msg_data_v01},
  {sizeof(ipa_dped_send_e2e_stats_resp_msg_v01), ipa_dped_send_e2e_stats_resp_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object ipa_dped_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *ipa_dped_qmi_idl_type_table_object_referenced_tables_v01[] =
{&ipa_dped_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object ipa_dped_qmi_idl_type_table_object_v01 = {
  sizeof(ipa_dped_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(ipa_dped_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  ipa_dped_type_table_v01,
  ipa_dped_message_table_v01,
  ipa_dped_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry ipa_dped_service_command_messages_v01[] = {
  {QMI_IPA_DPED_INDICATION_REGISTER_REQ_V01, QMI_IDL_TYPE16(0, 0), 4},
  {QMI_IPA_DPED_SEND_E2E_STATS_REQ_MSG_V01, QMI_IDL_TYPE16(0, 3), 5298}
};

static const qmi_idl_service_message_table_entry ipa_dped_service_response_messages_v01[] = {
  {QMI_IPA_DPED_INDICATION_REGISTER_RESP_V01, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_IPA_DPED_SEND_E2E_STATS_RESP_MSG_V01, QMI_IDL_TYPE16(0, 4), 7}
};

static const qmi_idl_service_message_table_entry ipa_dped_service_indication_messages_v01[] = {
  {QMI_IPA_DPED_TRIGGER_E2E_STATS_IND_V01, QMI_IDL_TYPE16(0, 2), 21}
};

/*Service Object*/
struct qmi_idl_service_object ipa_dped_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x42F,
  5298,
  { sizeof(ipa_dped_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(ipa_dped_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(ipa_dped_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { ipa_dped_service_command_messages_v01, ipa_dped_service_response_messages_v01, ipa_dped_service_indication_messages_v01},
  &ipa_dped_qmi_idl_type_table_object_v01,
  0x00,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type ipa_dped_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( IPA_DPED_V01_IDL_MAJOR_VERS != idl_maj_version || IPA_DPED_V01_IDL_MINOR_VERS != idl_min_version
       || IPA_DPED_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&ipa_dped_qmi_idl_service_object_v01;
}

