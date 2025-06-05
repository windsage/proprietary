/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A _ C O M M O N _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the data_common service Data structures.

  Copyright (c) 2006-2021 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //components/rel/qmimsgs.mpss/6.0/data_common/src/data_common_v01.c#4 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.9 
   It was generated on: Tue Oct  5 2021 (Spin 0)
   From IDL File: data_common_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "data_common_v01.h"


/*Type Definitions*/
static const uint8_t data_ep_id_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(data_ep_id_type_v01, ep_type),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(data_ep_id_type_v01, iface_id),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t data_coalescing_info_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(data_coalescing_info_type_v01, tcp_coalescing),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(data_coalescing_info_type_v01, udp_coalescing),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t data_mac_addr_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(data_mac_addr_type_v01, mac_addr),
  QMI_DATA_MAC_ADDR_LEN_V01,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t data_ipv4_addr_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(data_ipv4_addr_type_v01, ipv4_addr),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(data_ipv4_addr_type_v01, subnet_mask),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t data_ipv4_tos_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(data_ipv4_tos_type_v01, val),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(data_ipv4_tos_type_v01, mask),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t data_ipv4_info_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(data_ipv4_info_type_v01, valid_params),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(data_ipv4_info_type_v01, src_addr),
  QMI_IDL_TYPE88(0, 3),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(data_ipv4_info_type_v01, dest_addr),
  QMI_IDL_TYPE88(0, 3),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(data_ipv4_info_type_v01, tos),
  QMI_IDL_TYPE88(0, 4),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t data_ipv6_trf_cls_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(data_ipv6_trf_cls_type_v01, val),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(data_ipv6_trf_cls_type_v01, mask),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t data_ipv6_addr_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(data_ipv6_addr_type_v01, ipv6_address),
  QMI_DATA_IPV6_ADDR_LEN_V01,

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(data_ipv6_addr_type_v01, prefix_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t data_ipv6_info_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(data_ipv6_info_type_v01, valid_params),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(data_ipv6_info_type_v01, src_addr),
  QMI_IDL_TYPE88(0, 7),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(data_ipv6_info_type_v01, dest_addr),
  QMI_IDL_TYPE88(0, 7),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(data_ipv6_info_type_v01, trf_cls),
  QMI_IDL_TYPE88(0, 6),
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(data_ipv6_info_type_v01, flow_label),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t data_ip_header_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(data_ip_header_type_v01, ip_version),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(data_ip_header_type_v01, v4_info),
  QMI_IDL_TYPE88(0, 5),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(data_ip_header_type_v01, v6_info),
  QMI_IDL_TYPE88(0, 8),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t data_port_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(data_port_type_v01, port),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(data_port_type_v01, range),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t data_port_info_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(data_port_info_type_v01, valid_params),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(data_port_info_type_v01, src_port_info),
  QMI_IDL_TYPE88(0, 10),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(data_port_info_type_v01, dest_port_info),
  QMI_IDL_TYPE88(0, 10),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t data_icmp_info_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(data_icmp_info_type_v01, valid_params),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(data_icmp_info_type_v01, type),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(data_icmp_info_type_v01, code),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t data_ipsec_info_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(data_ipsec_info_type_v01, valid_params),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(data_ipsec_info_type_v01, spi),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t data_xport_header_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(data_xport_header_type_v01, xport_protocol),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(data_xport_header_type_v01, tcp_info),
  QMI_IDL_TYPE88(0, 11),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(data_xport_header_type_v01, udp_info),
  QMI_IDL_TYPE88(0, 11),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(data_xport_header_type_v01, icmp_info),
  QMI_IDL_TYPE88(0, 12),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(data_xport_header_type_v01, esp_info),
  QMI_IDL_TYPE88(0, 13),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(data_xport_header_type_v01, ah_info),
  QMI_IDL_TYPE88(0, 13),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t data_filter_rule_type_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(data_filter_rule_type_v01, ip_info),
  QMI_IDL_TYPE88(0, 9),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(data_filter_rule_type_v01, xport_info),
  QMI_IDL_TYPE88(0, 14),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t data_ip_family_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(data_ip_family_type_v01, filter_id),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(data_ip_family_type_v01, ip_version),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t data_ipv4_addr_ex_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(data_ipv4_addr_ex_type_v01, filter_id),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(data_ipv4_addr_ex_type_v01, ip_addr),
  QMI_IDL_TYPE88(0, 3),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t data_ipv4_tos_ex_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(data_ipv4_tos_ex_type_v01, filter_id),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(data_ipv4_tos_ex_type_v01, tos),
  QMI_IDL_TYPE88(0, 4),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t data_ipv6_addr_ex_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(data_ipv6_addr_ex_type_v01, filter_id),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(data_ipv6_addr_ex_type_v01, ip_addr),
  QMI_IDL_TYPE88(0, 7),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t data_ipv6_trf_cls_ex_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(data_ipv6_trf_cls_ex_type_v01, filter_id),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(data_ipv6_trf_cls_ex_type_v01, trf_cls),
  QMI_IDL_TYPE88(0, 6),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t data_ipv6_flow_label_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(data_ipv6_flow_label_type_v01, filter_id),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(data_ipv6_flow_label_type_v01, flow_label),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t data_port_ex_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(data_port_ex_type_v01, filter_id),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(data_port_ex_type_v01, port_info),
  QMI_IDL_TYPE88(0, 10),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t data_icmp_type_info_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(data_icmp_type_info_type_v01, filter_id),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(data_icmp_type_info_type_v01, icmp_type),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(data_icmp_type_info_type_v01, icmp_type_range),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t data_icmp_code_info_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(data_icmp_code_info_type_v01, filter_id),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(data_icmp_code_info_type_v01, icmp_code),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t data_spi_info_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(data_spi_info_type_v01, filter_id),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(data_spi_info_type_v01, spi),

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
/* Type Table */
static const qmi_idl_type_table_entry  data_common_type_table_v01[] = {
  {sizeof(data_ep_id_type_v01), data_ep_id_type_data_v01},
  {sizeof(data_coalescing_info_type_v01), data_coalescing_info_type_data_v01},
  {sizeof(data_mac_addr_type_v01), data_mac_addr_type_data_v01},
  {sizeof(data_ipv4_addr_type_v01), data_ipv4_addr_type_data_v01},
  {sizeof(data_ipv4_tos_type_v01), data_ipv4_tos_type_data_v01},
  {sizeof(data_ipv4_info_type_v01), data_ipv4_info_type_data_v01},
  {sizeof(data_ipv6_trf_cls_type_v01), data_ipv6_trf_cls_type_data_v01},
  {sizeof(data_ipv6_addr_type_v01), data_ipv6_addr_type_data_v01},
  {sizeof(data_ipv6_info_type_v01), data_ipv6_info_type_data_v01},
  {sizeof(data_ip_header_type_v01), data_ip_header_type_data_v01},
  {sizeof(data_port_type_v01), data_port_type_data_v01},
  {sizeof(data_port_info_type_v01), data_port_info_type_data_v01},
  {sizeof(data_icmp_info_type_v01), data_icmp_info_type_data_v01},
  {sizeof(data_ipsec_info_type_v01), data_ipsec_info_type_data_v01},
  {sizeof(data_xport_header_type_v01), data_xport_header_type_data_v01},
  {sizeof(data_filter_rule_type_v01), data_filter_rule_type_data_v01},
  {sizeof(data_ip_family_type_v01), data_ip_family_type_data_v01},
  {sizeof(data_ipv4_addr_ex_type_v01), data_ipv4_addr_ex_type_data_v01},
  {sizeof(data_ipv4_tos_ex_type_v01), data_ipv4_tos_ex_type_data_v01},
  {sizeof(data_ipv6_addr_ex_type_v01), data_ipv6_addr_ex_type_data_v01},
  {sizeof(data_ipv6_trf_cls_ex_type_v01), data_ipv6_trf_cls_ex_type_data_v01},
  {sizeof(data_ipv6_flow_label_type_v01), data_ipv6_flow_label_type_data_v01},
  {sizeof(data_port_ex_type_v01), data_port_ex_type_data_v01},
  {sizeof(data_icmp_type_info_type_v01), data_icmp_type_info_type_data_v01},
  {sizeof(data_icmp_code_info_type_v01), data_icmp_code_info_type_data_v01},
  {sizeof(data_spi_info_type_v01), data_spi_info_type_data_v01}
};

/* Message Table */
/* No Messages Defined in IDL */

/* Range Table */
/* Predefine the Type Table Object */
const qmi_idl_type_table_object data_common_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *data_common_qmi_idl_type_table_object_referenced_tables_v01[] =
{&data_common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
const qmi_idl_type_table_object data_common_qmi_idl_type_table_object_v01 = {
  sizeof(data_common_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  0,
  1,
  data_common_type_table_v01,
  NULL,
  data_common_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

