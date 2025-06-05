/*===========================================================================

  Copyright (c) 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#include "Encoder.h"

using namespace rildata;

#define NO_CORRESPONIDNG_SLICE_DIFFERENTIATOR -1

/*
  uint8_t precedence;
  std::vector<TrafficDescriptor_t> trafficDescriptors;
  std::vector<RouteSelectionDescriptor_t> routeSelectionDescriptors;

  Based on 3GPP TS 24.526 Chapter 5.2 Encoding of UE policy part type URSP,
  the encoded URSP data is only UE policy part contents from 3GPP TS 24.501
  Figure D.6.2.7: UE policy part. UE policy part contents could includes one
  or more URSP rules as described in 3GPP TS 24.526 Figure 5.2.1: UE policy
  part contents including one or more URSP rules. This API is to encode each
  URSP rule.

  Each URSP rule is described in 3GPP TS 24.526 Figure 5.2.2: URSP rule.
  Which contains:
    2 bytes - Length of URSP rule
    1 byte - Precedence value of URSP rule
    2 bytes - Length of traffic descriptor
    vairable bytes - Traffic descriptors from 3GPP TS 24.526 Table 5.2.1
    2 bytes - Length of route selection descriptor list
    variable bytes - Route selection descriptor list from 3GPP TS 24.526
                     Figure 5.2.3
                   - Each Route selection descriptor is in 3GPP TS 24.526
                     Figure 5.2.4

*/
std::vector<uint8_t> encodeUrspRule(UrspRule_t urspRule)
{
  std::vector<uint8_t> result;
  std::vector<uint8_t> encodedUrspRules;
  std::vector<uint8_t> encodedTrafficDescriptors;
  std::vector<uint8_t> encodedRouteSelectionDescriptorList;

  // traffic descriptor
  for (auto it=urspRule.trafficDescriptors.begin() ; it!=urspRule.trafficDescriptors.end() ; ++it) {
    std::vector<uint8_t> td = encodeTrafficDescriptor(*it);
    encodedTrafficDescriptors.insert(encodedTrafficDescriptors.end(), td.begin(), td.end());
  }

  // route selection descriptor list
  for (auto it=urspRule.routeSelectionDescriptors.begin() ; it!=urspRule.routeSelectionDescriptors.end() ; ++it) {
    std::vector<uint8_t> rsd = encodeRouteSelectionDescriptor(*it);
    encodedRouteSelectionDescriptorList.insert(encodedRouteSelectionDescriptorList.end(), rsd.begin(), rsd.end());
  }

  uint16_t tdLength = encodedTrafficDescriptors.size();
  uint16_t rsdLength = encodedRouteSelectionDescriptorList.size();
  // length of URSP rule
  uint16_t urspRuleLength = 1 + sizeof(tdLength) + tdLength + sizeof(rsdLength) + rsdLength;
  result.push_back((uint8_t)((urspRuleLength & 0xFF00) >> 8));  // upper byte
  result.push_back((uint8_t)(urspRuleLength & 0x00FF));         // lower byte

  // precedence value of URSP rule
  result.push_back(urspRule.precedence);

  // length of Traffic Descriptor
  result.push_back((uint8_t)((tdLength & 0xFF00) >> 8));        // upper byte
  result.push_back((uint8_t)(tdLength & 0x00FF));               // lower byte

  // traffic descriptor
  result.insert(result.end(), encodedTrafficDescriptors.begin(), encodedTrafficDescriptors.end());

  // length of route selection descriptor list
  result.push_back((uint8_t)((rsdLength & 0xFF00) >> 8));       // upper byte
  result.push_back((uint8_t)(rsdLength & 0x00FF));              // lower byte

  // route selection descriptor list
  result.insert(result.end(), encodedRouteSelectionDescriptorList.begin(), encodedRouteSelectionDescriptorList.end());

  return result;
}

/*
  std::optional<string> dnn;
  std::optional<std::vector<uint8_t>> osAppId;
*/
std::vector<uint8_t> encodeTrafficDescriptor(TrafficDescriptor_t td)
{
  std::vector<uint8_t> result;

  if (td.dnn.has_value()) {
    result.push_back(TD_TYPE_ID_DNN);
    std::vector<uint8_t> dnn(td.dnn.value().begin(), td.dnn.value().end());
    result.push_back(dnn.size()+1);   // outer length from 3GPP TS 24.526
    result.push_back(dnn.size());     // inner length from 3GPP TS 23.003
    result.insert(result.end(), dnn.begin(), dnn.end());
  }
  if (td.osAppId.has_value()) {
    result.push_back(TD_TYPE_ID_OS_ID_PLUS_OS_APP_ID);
    result.push_back(td.osAppId.value().size());
    result.insert(result.end(), td.osAppId.value().begin(), td.osAppId.value().end());
  }
  // TODO: other traffic descriptor component type identifiers defined in 3GPP TS 24.526 Table 5.2.1

  return result;
}

/*
  uint8_t precedence;
  PdpProtocolType_t sessionType;
  uint8_t sscMode;
  std::vector<SliceInfo_t> sliceInfo;
  std::vector<std::string> dnn;
*/
std::vector<uint8_t> encodeRouteSelectionDescriptor(RouteSelectionDescriptor_t rsd)
{
  std::vector<uint8_t> result;
  std::vector<uint8_t> encodedRouteSelectionDescriptor;

  // route selection descriptor component type identifier - ssc mode
  encodedRouteSelectionDescriptor.push_back(RSD_TYPE_ID_SSC_MODE_TYPE);
  switch (rsd.sscMode) {
    case SSC_MODE_1:
      encodedRouteSelectionDescriptor.push_back(SSC_MODE_1);
      break;
    case SSC_MODE_2:
      encodedRouteSelectionDescriptor.push_back(SSC_MODE_2);
      break;
    case SSC_MODE_3:
      encodedRouteSelectionDescriptor.push_back(SSC_MODE_3);
      break;
    default:
      encodedRouteSelectionDescriptor.pop_back();
      break;
  }

  // route selection descriptor component type identifier - S-NSSAPI
  if (!rsd.sliceInfo.empty()) {
    encodedRouteSelectionDescriptor.push_back(RSD_TYPE_ID_SNSSAI_TYPE);
    for (auto it=rsd.sliceInfo.begin() ; it!=rsd.sliceInfo.end() ; ++it) {
      std::vector<uint8_t> si = encodeSliceInfo(*it);
      encodedRouteSelectionDescriptor.insert(encodedRouteSelectionDescriptor.end(), si.begin(), si.end());
    }
  }

  // route selection descriptor component type identifier - DNN type
  if (!rsd.dnn.empty()) {
    encodedRouteSelectionDescriptor.push_back(RSD_TYPE_ID_DNN_TYPE);
    std::vector<uint8_t> dnnlist;
    for (auto it=rsd.dnn.begin() ; it!=rsd.dnn.end() ; ++it) {
      std::vector<uint8_t> dnn(it->begin(), it->end());
      dnnlist.push_back(dnn.size());
      dnnlist.insert(dnnlist.end(), dnn.begin(), dnn.end());
    }
    encodedRouteSelectionDescriptor.push_back(dnnlist.size());
    encodedRouteSelectionDescriptor.insert(encodedRouteSelectionDescriptor.end(), dnnlist.begin(), dnnlist.end());
  }

  // route selection descriptor component type identifier - PDU session type
  encodedRouteSelectionDescriptor.push_back(RSD_TYPE_ID_PDU_SESSION_TYPE);
  switch(rsd.sessionType) {
    case PdpProtocolType_t::IP:
      encodedRouteSelectionDescriptor.push_back(PDU_SESSION_TYPE_IPV4);
      break;
    case PdpProtocolType_t::IPV6:
      encodedRouteSelectionDescriptor.push_back(PDU_SESSION_TYPE_IPV6);
      break;
    case PdpProtocolType_t::UNSTRUCTURED:
      encodedRouteSelectionDescriptor.push_back(PDU_SESSION_TYPE_UNSTRUCTURED);
      break;
    case PdpProtocolType_t::IPV4V6:
    default:
      encodedRouteSelectionDescriptor.push_back(PDU_SESSION_TYPE_IPV4V6);
      break;
  }

  result.push_back(encodedRouteSelectionDescriptor.size());
  result.insert(result.end(), encodedRouteSelectionDescriptor.begin(), encodedRouteSelectionDescriptor.end());

  return result;
}

/*
  SliceInfoServiceType_t sliceServiceType;
  int32_t sliceDifferentiator;  // -1 indicates that there is no corresponding SliceInfo of the HPLMN
  SliceInfoServiceType_t mappedHplmnSst;
  int32_t mappedHplmnSd;        // -1 indicates that there is no corresponding SliceInfo of the HPLMN
  SliceInfoStatusType_t status;
*/
std::vector<uint8_t> encodeSliceInfo(SliceInfo_t si)
{
  std::vector<uint8_t> encodedSliceInfo;

  // Length of S-NSSAI contents
  if (si.sliceDifferentiator != NO_CORRESPONIDNG_SLICE_DIFFERENTIATOR &&
      si.mappedHplmnSd != NO_CORRESPONIDNG_SLICE_DIFFERENTIATOR) {
    encodedSliceInfo.push_back(SNSSAI_SST_SD_HPLMNSST_HPLMNSD);
    encodedSliceInfo.push_back((uint8_t)si.sliceServiceType);
    encodedSliceInfo.push_back((uint8_t)((si.sliceDifferentiator & 0x00FF0000) >> 16));
    encodedSliceInfo.push_back((uint8_t)((si.sliceDifferentiator & 0x0000FF00) >> 8));
    encodedSliceInfo.push_back((uint8_t)(si.sliceDifferentiator & 0x000000FF));
    encodedSliceInfo.push_back((uint8_t)si.mappedHplmnSst);
    encodedSliceInfo.push_back((uint8_t)((si.mappedHplmnSd & 0x00FF0000) >> 16));
    encodedSliceInfo.push_back((uint8_t)((si.mappedHplmnSd & 0x0000FF00) >> 8));
    encodedSliceInfo.push_back((uint8_t)(si.mappedHplmnSd & 0x000000FF));
  }
  else if (si.sliceDifferentiator != NO_CORRESPONIDNG_SLICE_DIFFERENTIATOR &&
    si.mappedHplmnSd == NO_CORRESPONIDNG_SLICE_DIFFERENTIATOR) {
    encodedSliceInfo.push_back(SNSSAI_SST_SD_HPLMNSST);
    encodedSliceInfo.push_back((uint8_t)si.sliceServiceType);
    encodedSliceInfo.push_back((uint8_t)((si.sliceDifferentiator & 0x00FF0000) >> 16));
    encodedSliceInfo.push_back((uint8_t)((si.sliceDifferentiator & 0x0000FF00) >> 8));
    encodedSliceInfo.push_back((uint8_t)(si.sliceDifferentiator & 0x000000FF));
    encodedSliceInfo.push_back((uint8_t)si.mappedHplmnSst);
  }
  else {
    encodedSliceInfo.push_back(SNSSAI_SST_HPLMNSST);
    encodedSliceInfo.push_back((uint8_t)si.sliceServiceType);
    encodedSliceInfo.push_back((uint8_t)si.mappedHplmnSst);
  }

  return encodedSliceInfo;
}
