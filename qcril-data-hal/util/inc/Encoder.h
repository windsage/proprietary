/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QCRIL_DATA_ENCODER_H_
#define _QCRIL_DATA_ENCODER_H_

#include <map>
#include <vector>
#include <framework/Log.h>
#include "MessageCommon.h"

// TRAFFIC DESCRIPTOR COMPONENT TYPE IDENTIFIER - 3GPP TS 24.526 Table 5.2.1
#define TD_TYPE_ID_MATCH_ALL                    0x01
#define TD_TYPE_ID_OS_ID_PLUS_OS_APP_ID         0x08
#define TD_TYPE_ID_IPV4_REMOTE_ADDRESS          0x10
#define TD_TYPE_ID_IPV6_REMOTE_ADDRESS          0x21
#define TD_TYPE_ID_PROTOCOL_IDENTIFIER          0x30
#define TD_TYPE_ID_SINGLE_REMOTE_PORT           0x50
#define TD_TYPE_ID_REMOTE_PORT_RANGE            0x51
#define TD_TYPE_ID_IP_TUPLE                     0x52
#define TD_TYPE_ID_SECURITY_PARAM_INDEX         0x60
#define TD_TYPE_ID_TOS_TRAFFIC_CLASS            0x70
#define TD_TYPE_ID_FLOW_LABEL                   0x80
#define TD_TYPE_ID_DESTINATION_MAC_ADDRESS      0x81
#define TD_TYPE_ID_CTAG_VID                     0x83
#define TD_TYPE_ID_STAG_VID                     0x84
#define TD_TYPE_ID_CTAG_PCP_DEI                 0x85
#define TD_TYPE_ID_STAG_PCP_DEI                 0x86
#define TD_TYPE_ID_ETHERTYPE                    0x87
#define TD_TYPE_ID_DNN                          0x88
#define TD_TYPE_ID_CONNECTION_CAPABILITY        0x90
#define TD_TYPE_ID_DESTINATION_FQDN             0x91
#define TD_TYPE_ID_REGULAR_EXPRESSION           0x92
#define TD_TYPE_ID_OS_APP_ID                    0xA0

// ROUTE SELECTION DESCRIPTOR COMPONENT TYPE IDENTIFIER - 3GPP TS 24.526 Table 5.2.1
#define RSD_TYPE_ID_SSC_MODE_TYPE               0x01
#define RSD_TYPE_ID_SNSSAI_TYPE                 0x02
#define RSD_TYPE_ID_DNN_TYPE                    0x04
#define RSD_TYPE_ID_PDU_SESSION_TYPE            0x08
#define RSD_TYPE_ID_PREFERRED_ACCESS_TYPE       0x10
#define RSD_TYPE_ID_MULTIACCESS_TYPE            0x11
#define RSD_TYPE_ID_NONSEAMLESS_OFFLOAD_TYPE    0x20
#define RSD_TYPE_ID_LOCATION_CRITERIA_TYPE      0x40
#define RSD_TYPE_ID_TIME_WINDOW_TYPE            0x80

// S-NSSAI CONTENTS FILEDS - 3GPP TS 24.501 9.11.2.8
#define SNSSAI_SST                              0x01
#define SNSSAI_SST_HPLMNSST                     0x02
#define SNSSAI_SST_SD                           0x04
#define SNSSAI_SST_SD_HPLMNSST                  0x05
#define SNSSAI_SST_SD_HPLMNSST_HPLMNSD          0x08

// SSC MODE VALUE - 3GPP TS 24.501 Table 9.11.4.16.1
#define SSC_MODE_1                              0x01
#define SSC_MODE_2                              0x02
#define SSC_MODE_3                              0x03

// PDU SESSION TYPE VALUE - 3GPP TS 24.501 Table 9.11.4.11.1
#define PDU_SESSION_TYPE_IPV4                   0x01
#define PDU_SESSION_TYPE_IPV6                   0x02
#define PDU_SESSION_TYPE_IPV4V6                 0x03
#define PDU_SESSION_TYPE_UNSTRUCTURED           0x04
#define PDU_SESSION_TYPE_ETHERNET               0x05
#define PDU_SESSION_TYPE_RESERVED               0x07

using namespace rildata;

std::vector<uint8_t> encodeUrspRule(UrspRule_t urspRule);
std::vector<uint8_t> encodeTrafficDescriptor(TrafficDescriptor_t td);
std::vector<uint8_t> encodeRouteSelectionDescriptor(RouteSelectionDescriptor_t rsd);
std::vector<uint8_t> encodeSliceInfo(SliceInfo_t si);

#endif /* _QCRIL_DATA_ENCODER_H_ */
