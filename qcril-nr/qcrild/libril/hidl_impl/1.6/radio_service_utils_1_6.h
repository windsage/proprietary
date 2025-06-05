/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#ifndef __RADIO_SERVICE_UTILS_1_6_H__
#define __RADIO_SERVICE_UTILS_1_6_H__

#include "android/hardware/radio/1.6/types.h"
#include "telephony/ril.h"
#include "MessageCommon.h"
#include "interfaces/voice/voice.h"
#include "interfaces/nas/nas_types.h"
#include "interfaces/uim/qcril_uim_types.h"
#include "interfaces/pbm/pbm.h"

#include "request/SetDataThrottlingRequestMessage.h"

namespace android {
namespace hardware {
namespace radio {
namespace utils {

void convertRilCellInfoList(const std::vector<RIL_CellInfo_v12>& rilCellInfo,
                            hidl_vec<V1_6::CellInfo>& records);
int convertToHidl(V1_6::Call& out, const qcril::interfaces::CallInfo& in);
int convertToHal(V1_6::NrSignalStrength& out, const RIL_NR_SignalStrength& in);
int convertToHal(V1_6::LteSignalStrength& out, RIL_LTE_SignalStrength_v8 in);
void convertRilSignalStrengthToHal(const RIL_SignalStrength& rilSignalStrength,
        V1_6::SignalStrength& signalStrength);
void fillVoiceRegistrationStateResponse(V1_6::RegStateResult& out,
                                        const RIL_VoiceRegistrationStateResponse& in);
void fillDataRegistrationStateResponse(V1_6::RegStateResult& out,
                                       const RIL_DataRegistrationStateResponse& in);
V1_6::SetupDataCallResult convertDcResultToHidlDcResult_1_6(const rildata::DataCallResult_t& result);
void convertQosFilters(V1_6::QosFilter& dcQosFilter, const rildata::QosFilter_t& result);
void convertQosSession(V1_6::QosSession& dcQosSession, const rildata::QosSession_t& result);
void convertTrafficDescriptor(V1_6::TrafficDescriptor& trafficDescriptor, const rildata::TrafficDescriptor_t& result);
rildata::DataThrottleAction_t convertHidlDataThrottleActionToRil(
                                const V1_6::DataThrottlingAction& dataThrottlingAction);



void convertRilPbCapacity(const std::shared_ptr<qcril::interfaces::qcril_pbm_adn_count_info> &in, V1_6::PhonebookCapacity &out);

void convertRilPhonebookRecords(const qcril::interfaces::AdnRecords *in, hidl_vec<V1_6::PhonebookRecordInfo> &out);

V1_6::PbReceivedStatus convertRilPbReceivedStatus(int seq_num);

void convertHidlPhonebookRecords(const V1_6::PhonebookRecordInfo &in, qcril::interfaces::AdnRecordInfo &out);
}  // namespace utils
}  // namespace radio
}  // namespace hardware
}  // namespace android

#endif  // __RADIO_SERVICE_UTILS_1_6_H__
