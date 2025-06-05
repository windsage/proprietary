/******************************************************************************
#  Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef RADIOSERVICEUTILS
#define RADIOSERVICEUTILS

#include "BnRadioDataBase.h"
#include "MessageCommon.h"
#include "SetDataThrottlingRequestMessage.h"

using namespace aidl::android::hardware::radio::data;
using namespace aidl::android::hardware::radio;

namespace utils {

DataCallFailCause convertDcFailCauseToAidlDcFailCause(const rildata::DataCallFailCause_t &cause);
PdpProtocolType convertStringToPdpProtocolType(const string &type);
vector<LinkAddress> convertLinkAddressToAidlVector(vector<rildata::LinkAddress_t> in_addresses);
vector<string> convertAddrStringToAidlStringVector(const string &addr);
void convertQosFilters(QosFilter& dcQosFilter, const rildata::QosFilter_t& result);
void convertQosSession(QosSession& dcQosSession, const rildata::QosSession_t& result);
void convertFromAidlTrafficDescriptor(std::optional<rildata::TrafficDescriptor_t>& out_td, const std::optional<::aidl::android::hardware::radio::data::TrafficDescriptor>& in_td);
void convertFromAidlTrafficDescriptor(std::optional<rildata::TrafficDescriptor_t>& out_td, const ::aidl::android::hardware::radio::data::TrafficDescriptor& in_td);
void convertTrafficDescriptor(TrafficDescriptor& trafficDescriptor, const rildata::TrafficDescriptor_t& result);
void convertFromAidlTrafficDescriptor(rildata::TrafficDescriptor_t& out_td, const ::aidl::android::hardware::radio::data::TrafficDescriptor& in_td);
SetupDataCallResult convertDcResultToAidlDcResult(const rildata::DataCallResult_t& result);
rildata::AccessNetwork_t convertAidlAccessNetworkToDataAccessNetwork(AccessNetwork ran);
string convertPdpProtocolTypeToString(const PdpProtocolType protocol);
rildata::DataProfileInfo_t convertAidlDataProfileInfoToRil(const DataProfileInfo& profile);
RadioError convertMsgToRadioError(Message::Callback::Status status, RIL_Errno e);
rildata::DataThrottleAction_t convertAidlDataThrottleActionToRil (const DataThrottlingAction& dataThrottlingAction);
SlicingConfig convertToAidlSlicingConfig(rildata::SlicingConfig_t);
void convertRouteSelectionDescriptor(RouteSelectionDescriptor out, rildata::RouteSelectionDescriptor_t in);
PdpProtocolType convertToPdpProtocolType(const string protocol);
DataProfileInfo convertToAidlDataProfileInfo(rildata::DataProfileInfo_t in_profile);
RadioError convertMsgToRadioError(Message::Callback::Status &status, rildata::ResponseError_t &respErr);
}

#endif
