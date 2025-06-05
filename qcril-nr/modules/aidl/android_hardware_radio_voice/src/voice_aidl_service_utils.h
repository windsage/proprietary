/******************************************************************************
#  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
#  All rights reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include "BnRadioVoiceDefault.h"
#include "interfaces/nas/nas_types.h"
#include <interfaces/voice/QcRilUnsolCdmaCallWaitingMessage.h>
#include <interfaces/voice/QcRilUnsolSupplementaryServiceMessage.h>
#include <interfaces/voice/QcRilUnsolCdmaInfoRecordMessage.h>

namespace aidlvoice {
  using namespace aidl::android::hardware::radio::voice;
}

void convertRilCdmaSignalInfoRecord(aidlvoice::CdmaSignalInfoRecord& out,
    std::shared_ptr<RIL_CDMA_SignalInfoRecord> in);
bool makeCdmaCallWaiting(aidlvoice::CdmaCallWaiting& callWaitingRecord,
    std::shared_ptr<QcRilUnsolCdmaCallWaitingMessage> msg);
void convertRilCdmaSignalInfoRecordToHal(aidlvoice::CdmaSignalInfoRecord& out,
    std::shared_ptr<RIL_CDMA_SignalInfoRecord> in);
bool makeCdmaInformationRecords(vector<aidlvoice::CdmaInformationRecord>& records,
    std::shared_ptr<QcRilUnsolCdmaInfoRecordMessage> msg);
int convertRilEccListToAidl(const qcril::interfaces::RIL_EccList& ril_ecc_list,
    vector<aidlvoice::EmergencyNumber>& ecc_list);
bool makeStkCcUnsolSsResult(aidlvoice::StkCcUnsolSsResult& ss,
    std::shared_ptr<QcRilUnsolSupplementaryServiceMessage> msg);
uint32_t toSaidlSources(uint32_t sources);
uint32_t toSaidlCategories(uint32_t categories);
bool isServiceTypeCf(RIL_SsServiceType serType, RIL_SsRequestType reqType);
int convertCallForwardInfoStatus(int in);
int convertCallForwardInfoStatusToAidl(int in);
aidlvoice::AudioQuality convertAudioQualityToAidl(const RIL_AudioQuality);
void convertToAidl(aidlvoice::Call& out, const qcril::interfaces::CallInfo& in);
int convertCallStateToAidl(qcril::interfaces::CallState in);
int convertCallPresentationToAidl(uint32_t in);
uint64_t toRilCategories(const int32_t categories);
qcril::interfaces::EmergencyCallRouting toRilRouting(
    const aidlvoice::EmergencyCallRouting& routing);
bool isUssdOverImsSupported();
aidlvoice::UssdModeType convertUssdModeType(qcril::interfaces::UssdModeType in);
RIL_Errno convertInternalErrorCode(RIL_Errno error);
