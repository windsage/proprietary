#ifndef DATA_COMMON_SERVICE_01_H
#define DATA_COMMON_SERVICE_01_H
/**
  @file data_common_v01.h

  @brief This is the public header file which defines the data_common service Data structures.

  This header file defines the types and structures that were defined in
  data_common. It contains the constant values defined, enums, structures,
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
  Copyright (c) 2006-2021 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //components/rel/qmimsgs.mpss/6.0/data_common/api/data_common_v01.h#4 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.9
   It was generated on: Tue Oct  5 2021 (Spin 0)
   From IDL File: data_common_v01.idl */

/** @defgroup data_common_qmi_consts Constant values defined in the IDL */
/** @defgroup data_common_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup data_common_qmi_enums Enumerated types used in QMI messages */
/** @defgroup data_common_qmi_messages Structures sent as QMI messages */
/** @defgroup data_common_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup data_common_qmi_accessor Accessor for QMI service object */
/** @defgroup data_common_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup data_common_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define DATA_COMMON_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define DATA_COMMON_V01_IDL_MINOR_VERS 0x07
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define DATA_COMMON_V01_IDL_TOOL_VERS 0x06

/**
    @}
  */


/** @addtogroup data_common_qmi_consts
    @{
  */

/**  */
#define QMI_DATA_IPV6_ADDR_LEN_V01 16
#define QMI_DATA_MAX_BEARERS_V01 64
#define QMI_DATA_MAC_ADDR_LEN_V01 6
/**
    @}
  */

/** @addtogroup data_common_qmi_enums
    @{
  */
typedef enum {
  DATA_EP_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DATA_EP_TYPE_RESERVED_V01 = 0x00, /**<  Reserved \n  */
  DATA_EP_TYPE_HSIC_V01 = 0x01, /**<  High-speed inter-chip interface \n  */
  DATA_EP_TYPE_HSUSB_V01 = 0x02, /**<  High-speed universal serial bus \n  */
  DATA_EP_TYPE_PCIE_V01 = 0x03, /**<  Peripheral component interconnect express \n  */
  DATA_EP_TYPE_EMBEDDED_V01 = 0x04, /**<  Embedded \n  */
  DATA_EP_TYPE_BAM_DMUX_V01 = 0x05, /**<  BAM demux \n  */
  DATA_EP_TYPE_ETHERNET_V01 = 0x06, /**<  Ethernet \n  */
  DATA_EP_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}data_ep_type_enum_v01;
/**
    @}
  */

/** @addtogroup data_common_qmi_aggregates
    @{
  */
typedef struct {

  data_ep_type_enum_v01 ep_type;
  /**<   Peripheral endpoint type. Values: \n
      - DATA_EP_TYPE_RESERVED (0x00) --  Reserved \n
      - DATA_EP_TYPE_HSIC (0x01) --  High-speed inter-chip interface \n
      - DATA_EP_TYPE_HSUSB (0x02) --  High-speed universal serial bus \n
      - DATA_EP_TYPE_PCIE (0x03) --  Peripheral component interconnect express \n
      - DATA_EP_TYPE_EMBEDDED (0x04) --  Embedded \n
      - DATA_EP_TYPE_BAM_DMUX (0x05) --  BAM demux \n
      - DATA_EP_TYPE_ETHERNET (0x06) --  Ethernet \n
 All other values are reserved and are ignored.
 */

  uint32_t iface_id;
  /**<   Peripheral interface number. \n
  */
}data_ep_id_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup data_common_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t tcp_coalescing;
  /**<   Specifies if TCP coalescing is enabled or disabled on the modem.
       Values:\n
       - FALSE: Disable TCP coalecsing. (Default)
       - TRUE : Enable TCP coalescing
  */

  uint8_t udp_coalescing;
  /**<   Specifies if UDP coalescing is enabled or disabled on the modem.
       Values:\n
       - FALSE: Disable UDP coalecsing. (Default)
       - TRUE : Enable UDP coalescing
  */
}data_coalescing_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup data_common_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t mac_addr[QMI_DATA_MAC_ADDR_LEN_V01];
}data_mac_addr_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup data_common_qmi_enums
    @{
  */
typedef enum {
  DATA_IP_FAMILY_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DATA_IP_FAMILY_IPV4_V01 = 0x04, /**<  IPv4\n  */
  DATA_IP_FAMILY_IPV6_V01 = 0x06, /**<  IPv6  */
  DATA_IP_FAMILY_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}data_ip_family_enum_v01;
/**
    @}
  */

typedef uint64_t data_ipv4_filter_mask_v01;
#define QMI_DATA_IPV4_FILTER_MASK_NONE_V01 ((data_ipv4_filter_mask_v01)0x0000000000000000ull) /**<  No parameters  */
#define QMI_DATA_IPV4_FILTER_MASK_SRC_ADDR_V01 ((data_ipv4_filter_mask_v01)0x0000000000000001ull) /**<  IPv4 source address  */
#define QMI_DATA_IPV4_FILTER_MASK_DEST_ADDR_V01 ((data_ipv4_filter_mask_v01)0x0000000000000002ull) /**<  IPv4 destination address  */
#define QMI_DATA_IPV4_FILTER_MASK_TOS_V01 ((data_ipv4_filter_mask_v01)0x0000000000000004ull) /**<  IPv4 type of service  */
typedef uint64_t data_ipv6_filter_mask_v01;
#define QMI_DATA_IPV6_FILTER_MASK_NONE_V01 ((data_ipv6_filter_mask_v01)0x0000000000000000ull) /**<  No parameters  */
#define QMI_DATA_IPV6_FILTER_MASK_SRC_ADDR_V01 ((data_ipv6_filter_mask_v01)0x0000000000000001ull) /**<  IPv6 source address  */
#define QMI_DATA_IPV6_FILTER_MASK_DEST_ADDR_V01 ((data_ipv6_filter_mask_v01)0x0000000000000002ull) /**<  IPv6 destination address  */
#define QMI_DATA_IPV6_FILTER_MASK_TRAFFIC_CLASS_V01 ((data_ipv6_filter_mask_v01)0x0000000000000004ull) /**<  IPv6 traffic class  */
#define QMI_DATA_IPV6_FILTER_MASK_FLOW_LABEL_V01 ((data_ipv6_filter_mask_v01)0x0000000000000008ull) /**<  IPv6 flow label  */
/** @addtogroup data_common_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t ipv4_addr;
  /**<   IPv4 address in host order.
   */

  uint32_t subnet_mask;
  /**<   IPv4 subnet mask in host order.
   */
}data_ipv4_addr_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup data_common_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t val;
  /**<   Type of service value. */

  uint8_t mask;
  /**<   Type of service mask. */
}data_ipv4_tos_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup data_common_qmi_aggregates
    @{
  */
typedef struct {

  data_ipv4_filter_mask_v01 valid_params;
  /**<   The bits set in this mask denote which parameters contain valid values. Values: \n
      - QMI_DATA_IPV4_FILTER_MASK_NONE (0x0000000000000000) --  No parameters
      - QMI_DATA_IPV4_FILTER_MASK_SRC_ADDR (0x0000000000000001) --  IPv4 source address
      - QMI_DATA_IPV4_FILTER_MASK_DEST_ADDR (0x0000000000000002) --  IPv4 destination address
      - QMI_DATA_IPV4_FILTER_MASK_TOS (0x0000000000000004) --  IPv4 type of service
 */

  data_ipv4_addr_type_v01 src_addr;
  /**<   IPv4 source address in host order.
   */

  data_ipv4_addr_type_v01 dest_addr;
  /**<   IPv4 destination address in host order.
   */

  data_ipv4_tos_type_v01 tos;
  /**<   IPv4 type of service.
   */
}data_ipv4_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup data_common_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t val;
  /**<   Traffic class value. */

  uint8_t mask;
  /**<   Traffic class mask. */
}data_ipv6_trf_cls_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup data_common_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t ipv6_address[QMI_DATA_IPV6_ADDR_LEN_V01];
  /**<   IPv6 address in network byte
       order; an 8-element array of 16-bit
       numbers, each of which is in big-endian
       format.
   */

  uint8_t prefix_len;
  /**<   IPv6 address prefix length.
   */
}data_ipv6_addr_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup data_common_qmi_aggregates
    @{
  */
typedef struct {

  data_ipv6_filter_mask_v01 valid_params;
  /**<   The bits set in this mask denote which parameters contain valid values. Values: \n
      - QMI_DATA_IPV6_FILTER_MASK_NONE (0x0000000000000000) --  No parameters
      - QMI_DATA_IPV6_FILTER_MASK_SRC_ADDR (0x0000000000000001) --  IPv6 source address
      - QMI_DATA_IPV6_FILTER_MASK_DEST_ADDR (0x0000000000000002) --  IPv6 destination address
      - QMI_DATA_IPV6_FILTER_MASK_TRAFFIC_CLASS (0x0000000000000004) --  IPv6 traffic class
      - QMI_DATA_IPV6_FILTER_MASK_FLOW_LABEL (0x0000000000000008) --  IPv6 flow label
 */

  data_ipv6_addr_type_v01 src_addr;
  /**<   IPv6 source address.
   */

  data_ipv6_addr_type_v01 dest_addr;
  /**<   IPv6 destination address.
   */

  data_ipv6_trf_cls_type_v01 trf_cls;
  /**<   IPv6 traffic class.
   */

  uint32_t flow_label;
  /**<   IPv6 flow label.
   */
}data_ipv6_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup data_common_qmi_aggregates
    @{
  */
typedef struct {

  data_ip_family_enum_v01 ip_version;
  /**<   Depending on the IP version set, either the IPv4 or the IPv6 information is valid. Values:\n
      - DATA_IP_FAMILY_IPV4 (0x04) --  IPv4\n
      - DATA_IP_FAMILY_IPV6 (0x06) --  IPv6
 */

  data_ipv4_info_type_v01 v4_info;
  /**<   Filter parameters for IPv4.
   */

  data_ipv6_info_type_v01 v6_info;
  /**<   Filter parameters for IPv6.
   */
}data_ip_header_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup data_common_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t port;
  /**<   Port in host order.
  */

  uint16_t range;
  /**<   Range.
   */
}data_port_type_v01;  /* Type */
/**
    @}
  */

typedef uint64_t data_port_info_filter_mask_v01;
#define QMI_DATA_PORT_INFO_FILTER_MASK_NONE_V01 ((data_port_info_filter_mask_v01)0x0000000000000000ull) /**<  No parameters  */
#define QMI_DATA_PORT_INFO_FILTER_MASK_SRC_PORT_V01 ((data_port_info_filter_mask_v01)0x0000000000000001ull) /**<  Source port  */
#define QMI_DATA_PORT_INFO_FILTER_MASK_DEST_PORT_V01 ((data_port_info_filter_mask_v01)0x0000000000000002ull) /**<  Destination port  */
/** @addtogroup data_common_qmi_aggregates
    @{
  */
typedef struct {

  data_port_info_filter_mask_v01 valid_params;
  /**<   The bits set in this mask denote which parameters contain valid values. Values: \n
      - QMI_DATA_PORT_INFO_FILTER_MASK_NONE (0x0000000000000000) --  No parameters
      - QMI_DATA_PORT_INFO_FILTER_MASK_SRC_PORT (0x0000000000000001) --  Source port
      - QMI_DATA_PORT_INFO_FILTER_MASK_DEST_PORT (0x0000000000000002) --  Destination port
 */

  data_port_type_v01 src_port_info;
  /**<   Source port information.
  */

  data_port_type_v01 dest_port_info;
  /**<   Destination port information.
   */
}data_port_info_type_v01;  /* Type */
/**
    @}
  */

typedef uint64_t data_icmp_filter_mask_v01;
#define QMI_DATA_ICMP_FILTER_MASK_NONE_V01 ((data_icmp_filter_mask_v01)0x0000000000000000ull) /**<  No parameters  */
#define QMI_DATA_ICMP_FILTER_MASK_MSG_TYPE_V01 ((data_icmp_filter_mask_v01)0x0000000000000001ull) /**<  Message type  */
#define QMI_DATA_ICMP_FILTER_MASK_MSG_CODE_V01 ((data_icmp_filter_mask_v01)0x0000000000000002ull) /**<  Message code  */
/** @addtogroup data_common_qmi_aggregates
    @{
  */
typedef struct {

  data_icmp_filter_mask_v01 valid_params;
  /**<   The bits set in this mask denote which parameters contain valid values. Values: \n
      - QMI_DATA_ICMP_FILTER_MASK_NONE (0x0000000000000000) --  No parameters
      - QMI_DATA_ICMP_FILTER_MASK_MSG_TYPE (0x0000000000000001) --  Message type
      - QMI_DATA_ICMP_FILTER_MASK_MSG_CODE (0x0000000000000002) --  Message code
 */

  uint8_t type;
  /**<   ICMP type.
  */

  uint8_t code;
  /**<   ICMP code.
   */
}data_icmp_info_type_v01;  /* Type */
/**
    @}
  */

typedef uint64_t data_ipsec_filter_mask_v01;
#define QMI_DATA_IPSEC_FILTER_MASK_NONE_V01 ((data_ipsec_filter_mask_v01)0x0000000000000000ull) /**<  No parameters  */
#define QMI_DATA_IPSEC_FILTER_MASK_SPI_V01 ((data_ipsec_filter_mask_v01)0x0000000000000001ull) /**<  Security parameter index  */
/** @addtogroup data_common_qmi_aggregates
    @{
  */
typedef struct {

  data_ipsec_filter_mask_v01 valid_params;
  /**<   The bits set in this mask denote which parameters contain valid values. Values: \n
      - QMI_DATA_IPSEC_FILTER_MASK_NONE (0x0000000000000000) --  No parameters
      - QMI_DATA_IPSEC_FILTER_MASK_SPI (0x0000000000000001) --  Security parameter index
 */

  uint32_t spi;
  /**<   Security parameter index for IPSec.
   */
}data_ipsec_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup data_common_qmi_enums
    @{
  */
typedef enum {
  DATA_XPORT_PROTOCOL_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DATA_PROTO_NONE_V01 = 0x00, /**<  No transport protocol \n  */
  DATA_PROTO_ICMP_V01 = 0x01, /**<  Internet Control Messaging Protocol \n  */
  DATA_PROTO_TCP_V01 = 0x06, /**<  Transmission Control Protocol \n  */
  DATA_PROTO_UDP_V01 = 0x11, /**<  User Datagram Protocol \n  */
  DATA_PROTO_ESP_V01 = 0x32, /**<  Encapsulating Security Payload protocol \n  */
  DATA_PROTO_AH_V01 = 0x33, /**<  Authentication Header protocol \n  */
  DATA_PROTO_ICMP6_V01 = 0x3A, /**<  ICMPv6 protocol  */
  DATA_PROTO_TCPUDP_V01 = 0xFD, /**<  TCP and UDP protocol  */
  DATA_XPORT_PROTOCOL_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}data_xport_protocol_enum_v01;
/**
    @}
  */

/** @addtogroup data_common_qmi_aggregates
    @{
  */
typedef struct {

  data_xport_protocol_enum_v01 xport_protocol;
  /**<   Depending on the value in xport_protocol, only one field of icmp_info,
 tcp_info, udp_info, esp_info, or ah_info is valid. DATA_PROTO_NONE
 implies that no transport level protocol parameters are valid. Values:\n
      - DATA_PROTO_NONE (0x00) --  No transport protocol \n
      - DATA_PROTO_ICMP (0x01) --  Internet Control Messaging Protocol \n
      - DATA_PROTO_TCP (0x06) --  Transmission Control Protocol \n
      - DATA_PROTO_UDP (0x11) --  User Datagram Protocol \n
      - DATA_PROTO_ESP (0x32) --  Encapsulating Security Payload protocol \n
      - DATA_PROTO_AH (0x33) --  Authentication Header protocol \n
      - DATA_PROTO_ICMP6 (0x3A) --  ICMPv6 protocol
      - DATA_PROTO_TCPUDP (0xFD) --  TCP and UDP protocol
 */

  data_port_info_type_v01 tcp_info;
  /**<   Filter parameters for TCP.
   */

  data_port_info_type_v01 udp_info;
  /**<   Filter parameters for UDP.
   */

  data_icmp_info_type_v01 icmp_info;
  /**<   Filter parameters for ICMP.
   */

  data_ipsec_info_type_v01 esp_info;
  /**<   Filter parameters for ESP.
   */

  data_ipsec_info_type_v01 ah_info;
  /**<   Filter parameters for AH.
    */
}data_xport_header_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup data_common_qmi_aggregates
    @{
  */
typedef struct {

  data_ip_header_type_v01 ip_info;
  /**<   Internet protocol filter parameters.
  */

  data_xport_header_type_v01 xport_info;
  /**<   Transport level protocol filter parameters.
   */
}data_filter_rule_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup data_common_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t filter_id;

  data_ip_family_enum_v01 ip_version;
  /**<   Depending on the IP version set, either the IPv4 or the IPv6 information
 is valid for filter_id. Values:\n
      - DATA_IP_FAMILY_IPV4 (0x04) --  IPv4\n
      - DATA_IP_FAMILY_IPV6 (0x06) --  IPv6
 */
}data_ip_family_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup data_common_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t filter_id;

  data_ipv4_addr_type_v01 ip_addr;
  /**<     IPv4 address.
   */
}data_ipv4_addr_ex_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup data_common_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t filter_id;

  data_ipv4_tos_type_v01 tos;
  /**<     IPv4 type of service.
   */
}data_ipv4_tos_ex_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup data_common_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t filter_id;

  data_ipv6_addr_type_v01 ip_addr;
  /**<     IPv6 address.
   */
}data_ipv6_addr_ex_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup data_common_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t filter_id;

  data_ipv6_trf_cls_type_v01 trf_cls;
  /**<     IPv6 traffic class.
   */
}data_ipv6_trf_cls_ex_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup data_common_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t filter_id;

  uint32_t flow_label;
  /**<     IPv6 flow label.
   */
}data_ipv6_flow_label_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup data_common_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t filter_id;

  data_port_type_v01 port_info;
  /**<     Port information.
  */
}data_port_ex_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup data_common_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t filter_id;

  uint8_t icmp_type;

  uint8_t icmp_type_range;
  /**<   ICMP Type range to select more than one ICMP types.
       For all ICMP types (set icmp_type to 0 and icmp_type_range to 255).
  */
}data_icmp_type_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup data_common_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t filter_id;

  uint8_t icmp_code;
  /**<     ICMP code.
  */
}data_icmp_code_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup data_common_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t filter_id;

  uint32_t spi;
  /**<   SPI for IPSec e.g. ESP, AH.
   */
}data_spi_info_type_v01;  /* Type */
/**
    @}
  */

/* Conditional compilation tags for message removal */

/*Extern Definition of Type Table Object*/
/*THIS IS AN INTERNAL OBJECT AND SHOULD ONLY*/
/*BE ACCESSED BY AUTOGENERATED FILES*/
extern const qmi_idl_type_table_object data_common_qmi_idl_type_table_object_v01;


#ifdef __cplusplus
}
#endif
#endif

