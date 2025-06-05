/******************************************************************************
#  Copyright (c) 2020-2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#ifndef __RIL_UTILS_H_
#define __RIL_UTILS_H_

#include <string>
#include <telephony/ril.h>
#include <telephony/ril_ims.h>
#include <telephony/ril_call.h>
#include <qcril_config.h>
#include <interfaces/voice/voice.h>
#include <interfaces/ims/ims.h>
#include <interfaces/sms/sms.h>
#include <interfaces/dms/dms_types.h>
#include <interfaces/voice/QcRilUnsolImsHandoverMessage.h>
#include <interfaces/voice/QcRilUnsolImsConferenceInfoMessage.h>
#include <interfaces/voice/QcRilUnsolImsSrtpEncryptionStatus.h>
#include <interfaces/ims/QcRilUnsolImsPendingMultiLineStatus.h>
#include <interfaces/ims/QcRilUnsolImsMultiIdentityStatusMessage.h>
#include "MessageCommon.h"

bool isServiceTypeCfQuery(RIL_SsServiceType serType, RIL_SsRequestType reqType);

namespace ril {
namespace socket {
namespace utils {

template <class T, class U, std::enable_if_t<!std::is_same<T, U>::value, int> = 0>
void convertToSocket(T& out, const U& in);
template <class T, class U, std::enable_if_t<std::is_same<T, U>::value, int> = 0>
void convertToSocket(T& out, const U& in) {
  out = in;
}

template <class T, class U, std::enable_if_t<!std::is_pointer<T>::value, int> = 0>
void convertToSocket(T& out, const std::shared_ptr<U> in) {
  if (in != nullptr) {
    convertToSocket(out, *in);
  }
}
template <class T, class U>
void convertToSocket(T*& out, const std::shared_ptr<U> in) {
  if (in != nullptr) {
    T* tmp = new T{};
    if (tmp) {
      convertToSocket(*tmp, *in);
      out = tmp;
    }
  }
}

template <class T, class U>
void convertToSocket(T*& out, size_t& outlen, const U& in);

template <class T, class U>
void convertToSocket(T*& out, size_t& outlen, const std::vector<U>& in) {
  size_t sz = in.size();
  T* tmp = nullptr;
  if (sz > 0) {
    tmp = new T[sz];
    std::transform(in.begin(), in.end(), tmp, [](const U& in) -> T {
      T ret{};
      convertToSocket(ret, in);
      return ret;
    });
  }
  out = tmp;
  outlen = sz;
}

template <class T, class U>
void convertToSocket(T*& out, const U& in) {
  T* tmp = new T{};
  if (tmp) {
    convertToSocket(*tmp, in);
    out = tmp;
  }
}

template <>
void convertToSocket(char*& out, size_t& outlen, const std::string& in);

template <>
void convertToSocket<char, std::string>(char*& out, const std::string& in);

template <>
void convertToSocket(RIL_IMS_SipErrorInfo& out, const qcril::interfaces::SipErrorInfo& in);

template <>
void convertToSocket(RIL_IMS_CallType& out, const qcril::interfaces::CallType& in);

template <>
void convertToSocket(RIL_CallType& out, const qcril::interfaces::CallType& in);

template <>
void convertToSocket(RIL_IMS_RegistrationState& out, const qcril::interfaces::RegState& in);

template <>
void convertToSocket(uint8_t& out, const bool& in);

template <>
void convertToSocket(RIL_IMS_CallState& out, const qcril::interfaces::CallState& in);

template <>
void convertToSocket(RIL_IMS_CallDomain& out, const qcril::interfaces::CallDomain& in);

template <>
void convertToSocket(RIL_CallDomain& out, const qcril::interfaces::CallDomain& in);

template <>
void convertToSocket(RIL_IMS_CallModifiedCause& out, const qcril::interfaces::CallModifiedCause& in);

template <>
void convertToSocket(RIL_IMS_RttMode& out, const qcril::interfaces::RttMode& in);

template <>
void convertToSocket(RIL_RttMode& out, const qcril::interfaces::RttMode& in);

template <>
void convertToSocket(RIL_IMS_StatusType& out, const qcril::interfaces::StatusType& in);

template <>
void convertToSocket(RIL_IMS_Registration& out, const qcril::interfaces::Registration& in);

template <>
void convertToSocket(RIL_IMS_AccessTechnologyStatus& out,
                     const qcril::interfaces::AccessTechnologyStatus& in);

template <>
void convertToSocket(RIL_IMS_ServiceStatusInfo& out, const qcril::interfaces::ServiceStatusInfo& in);

template <>
void convertToSocket(RIL_IMS_CallFailCause& out, const qcril::interfaces::CallFailCause& in);

template <>
void convertToSocket(RIL_IMS_CallFailCauseResponse& out,
                     const qcril::interfaces::CallFailCauseResponse& in);

template <>
void convertToSocket(RIL_IMS_VerificationStatus& out,
                     qcril::interfaces::VerificationStatus const& in);

template <>
void convertToSocket(RIL_IMS_VerstatInfo& out, const qcril::interfaces::VerstatInfo& in);

template <>
void convertToSocket(RIL_UUS_Info& out, const qcril::interfaces::UusInfo& in);

template <>
void convertToSocket(RIL_IMS_TirMode& out, const qcril::interfaces::TirMode& in);

template <>
void convertToSocket(RIL_IMS_CallInfo& out, const qcril::interfaces::CallInfo& in);

template <>
void convertToSocket(RIL_IMS_CallInfo*& out, size_t& outlen,
                     const std::vector<qcril::interfaces::CallInfo>& callInfoList);

template <>
void convertToSocket(RIL_IMS_MessageType& out, const qcril::interfaces::MessageType& in);

template <>
void convertToSocket(RIL_IMS_MessagePriority& out, const qcril::interfaces::MessagePriority& in);

template <>
void convertToSocket(RIL_IMS_CallModifyFailCause& out,
                     const qcril::interfaces::CallModifyFailCause& in);
template <>
void convertToSocket(RIL_CallModifyFailCause& out,
                     const qcril::interfaces::CallModifyFailCause& in);

template <>
void convertToSocket(RIL_CallInfo& out, const qcril::interfaces::CallInfo& in);



#if 0
template <>
void convertToSocket(RIL_IMS_MessageDetails &out, const qcril::interfaces::MessageDetails &in);

template <>
void convertToSocket(RIL_IMS_MessageDetails *&out, size_t &outlen, const std::vector<qcril::interfaces::MessageDetails> &in);

template <>
void convertToSocket(RIL_IMS_MessageSummary &out, const qcril::interfaces::MessageSummary &in);
#endif

template <>
void convertToSocket(RIL_IMS_CallForwardInfo& out, const qcril::interfaces::CallForwardInfo& in);

template <>
void convertToSocket(RIL_IMS_QueryCallForwardStatusInfo& out,
                     const qcril::interfaces::GetCallForwardRespData& in);
template <>
void convertToSocket(RIL_QueryCallForwardStatusInfo& out,
                     const qcril::interfaces::GetCallForwardRespData& in);

template <>
void convertToSocket(std::vector<RIL_IMS_ServiceStatusInfo>& out,
                     const std::vector<qcril::interfaces::ServiceStatusInfo>& in);

template <>
void convertToSocket(RIL_IMS_SubConfigInfo& out, const qcril::interfaces::ImsSubConfigInfo& in);

template <>
void convertToSocket(RIL_IMS_ClirInfo& out, const qcril::interfaces::ClirInfo& in);

template <>
void convertToSocket(RIL_IMS_ClipInfo& out, const qcril::interfaces::ClipInfo& in);

template <>
void convertToSocket(RIL_IMS_ConfParticipantOperation& out,
                     const qcril::interfaces::ConfParticipantOperation& in);

template <>
void convertToSocket(RIL_IMS_BlockStatus& out, const qcril::interfaces::BlockStatus& in);

template <>
void convertToSocket(RIL_IMS_VoiceInfoType& out, const qcril::interfaces::VoiceInfo& in);

template <>
void convertToSocket(RIL_VoiceInfoType& out, const qcril::interfaces::VoiceInfo& in);

template <>
void convertToSocket(RIL_IMS_SuppSvcResponse& out,
                     const qcril::interfaces::SuppServiceStatusInfo& in);

template <>
void convertToSocket(RIL_IMS_ColrInfo& out, const qcril::interfaces::ColrInfo& in);

template <>
void convertToSocket(RIL_ColrInfo& out, const qcril::interfaces::ColrInfo& in);

template <>
void convertToSocket(qcril::interfaces::MultiIdentityInfo& out,
                     const RIL_IMS_MultiIdentityLineInfo& in);

template <>
void convertToSocket(RIL_IMS_QueryVirtualLineInfoResponse& out,
                     const qcril::interfaces::VirtualLineInfo& in);

template <>
void convertToSocket(RIL_IMS_CallProgressInfo& out, const qcril::interfaces::CallProgressInfo& in);

template <>
void convertToSocket(RIL_IMS_MsimAdditionalCallInfo& out,
                     const qcril::interfaces::MsimAdditionalCallInfo& in);
template <>
void convertToSocket(RIL_IMS_Codec& out, const qcril::interfaces::AudioCodec& in);
template <>
void convertToSocket(RIL_IMS_ComputedAudioQuality& out,
                     const qcril::interfaces::ComputedAudioQuality& in);
template <>
void convertToSocket(RIL_IMS_CallComposerInfo& ccInfo,
                     const qcril::interfaces::CallComposerInfo& entry);
template <>
void convertToSocket(RIL_IMS_EcnamInfo& ecnamInfo, const qcril::interfaces::EcnamInfo& entry);

template <>
void convertToSocket(RIL_CallComposerInfo& ccInfo,
                     const qcril::interfaces::CallComposerInfo& entry);
template <>
void convertToSocket(RIL_EcnamInfo& ecnamInfo, const qcril::interfaces::EcnamInfo& entry);

template <>
void convertToSocket(RIL_ConfParticipantOperation& out,
                     const qcril::interfaces::ConfParticipantOperation& in);
template <>
void convertToSocket(RIL_ClipInfo& out, const qcril::interfaces::ClipInfo& in);

template <>
void convertToSocket(RIL_ClirInfo& out, const qcril::interfaces::ClirInfo& in);

template <>
void convertToSocket(RIL_CallForwardParams& out, const qcril::interfaces::CallForwardInfo& in);

bool convertBytesToHexString(const uint8_t* bytePayload, size_t size, std::string& str);
void smsConvertCdmaFormatToPseudoPdu(const RIL_CDMA_SMS_Message& cdmaMsg, std::vector<uint8_t>& payload);
bool smsConvertPayloadToCdmaFormat(const uint8_t* payload, size_t size, RIL_CDMA_SMS_Message& cdmaMsg);
RIL_IMS_VerificationStatus convertVerificationStatus(const qcril::interfaces::VerificationStatus& verificationStatus);
RIL_IMS_TtyModeType convertTtyMode(const qcril::interfaces::TtyMode& ttyMode);
RIL_TtyModeType convertCallTtyMode(const qcril::interfaces::TtyMode& ttyMode);
RIL_IMS_SmsSendStatus getImsSmsSendStatus(RIL_Errno err, RIL_RadioTechnologyFamily tech, bool registeredOnLte);
RIL_IMS_SrtpEncryptionStatus convertToSocket(const QcRilUnsolImsSrtpEncryptionStatus& in);
qcril::interfaces::DeliverStatus convertImsSmsDeliveryStatus(RIL_IMS_SmsDeliveryStatus smsDeliveryStatus);
void convertToSocket(RIL_IMS_HandoverType &type, const qcril::interfaces::HandoverType &&handoverType);
void convertToSocket(RIL_IMS_HandoverInfo &handoverInfo, std::shared_ptr<QcRilUnsolImsHandoverMessage> msg);

void convertToSocket(RIL_IMS_ConferenceCallState &callState, qcril::interfaces::ConferenceCallState src);
void convertToSocket(RIL_ConferenceCallState &callState, qcril::interfaces::ConferenceCallState src);

void convertToSocket(RIL_IMS_RefreshConferenceInfo &conferenceInfo,
        std::shared_ptr<QcRilUnsolImsConferenceInfoMessage> msg);

void convertToSocket(RIL_RefreshConferenceInfo &conferenceInfo,
        std::shared_ptr<QcRilUnsolImsConferenceInfoMessage> msg);

void convertToSocket(RIL_IMS_MultiIdentityRegistrationStatus &out, int in);
void convertToSocket(RIL_IMS_MultiIdentityLineType &out, int in);

void convertToSocket(RIL_IMS_MultiIdentityLineInfo &out,
        const qcril::interfaces::MultiIdentityInfo &in);

void convertToSocket(std::vector<RIL_IMS_MultiIdentityLineInfo> &out,
        const std::vector<qcril::interfaces::MultiIdentityInfo> &in);

void convertToSocket(RIL_IMS_GeoLocationDataStatus& out,
                     qcril::interfaces::GeoLocationDataStatus in);

void convertToSocket(RIL_Qos& out, const rildata::Qos_t& in);
void convertToSocket(RIL_QosFilter& out, const rildata::QosFilter_t& in);
void convertToSocket(RIL_QosSession& out, const rildata::QosSession_t& in);
void convertToSocket(RIL_TrafficDescriptor& out, const rildata::TrafficDescriptor_t& in);
void convertAddressesToSocket(RIL_Data_Call_Response_v11& dataCall, const rildata::DataCallResult_t& entry);
void convertToSocket(RIL_Data_Call_Response_v11& dataCall, const rildata::DataCallResult_t& entry);
void convertToSocket(RIL_IMS_VerstatVerificationStatus& verStatus,
                     const qcril::interfaces::VerificationStatus& entry);
void convertToSocket(RIL_VerstatVerificationStatus& verStatus,
                     const qcril::interfaces::VerificationStatus& entry);
void convertToSocket(rilSlicingConfig &out, rildata::SlicingConfig_t &in);
void convertToSocket(rilUrspRule &out, rildata::UrspRule_t &in);
void convertToSocket(RIL_SliceInfo &out, rildata::SliceInfo_t &in);
void convertToSocket(rilRouteSelectorDescriptor &out, rildata::RouteSelectionDescriptor_t &in);
void convertToSocket(RIL_DataProfileInfo &out, rildata::DataProfileInfo_t &in);

void convertToSocket(RIL_HandoverInfo& handoverInfo,
                     std::shared_ptr<QcRilUnsolImsHandoverMessage> msg);

template <>
void convertToSocket(RIL_ImeiInfo& out, const qcril::interfaces::RilGetImeiInfoResult_t& in);

void freeSocketResource(RIL_QosSession& qosSession);
void freeSocketResource(RIL_TrafficDescriptor& td);
void freeSocketResource(RIL_Data_Call_Response_v11& dataCall);
bool checkThresholdAndHysteresis(int32_t hysteresisDb,
        const int32_t* const thresholds, const uint32_t thresholdsLen);
RIL_IMS_MultiSimVoiceCapability convertMultiSimVoiceCapability(
    const qcril::interfaces::MultiSimVoiceCapability& voiceCapability);
bool isUssdOverImsSupported();

void release(RIL_IMS_SipErrorInfo &arg);
void release(RIL_IMS_Registration &arg);
void release(RIL_IMS_UssdInfo &arg);
void release(RIL_IMS_MessageWaitingInfo &arg);
void release(RIL_IMS_HandoverInfo &arg);
void release(RIL_IMS_RefreshConferenceInfo &arg);
void release(RIL_RefreshConferenceInfo &arg);
void release(RIL_IMS_SubConfigInfo &arg);
void release(RIL_IMS_ServiceStatusInfo &arg);
void release(std::vector<RIL_IMS_ServiceStatusInfo> &arg);
void release(RIL_IMS_ClipInfo &arg);
void release(RIL_ClipInfo& arg);
void release(RIL_ClirInfo& arg);
void release(RIL_HandoverInfo& arg);
void release(RIL_IMS_ColrInfo &arg);
void release(RIL_ColrInfo &arg);
void release(std::vector<RIL_IMS_MultiIdentityLineInfo> &arg);
void release(RIL_IMS_CallInfo *&arg, size_t sz);
void release(RIL_IMS_ConfigInfo& arg);
void release(RIL_IMS_PreAlertingCallInfo& preAlertInfo);
void release(RIL_IMS_AutoCallRejectionInfo& autoRej);
void release(RIL_IMS_CallComposerInfo& ccInfo);
void release(RIL_IMS_EcnamInfo& ecnamInfo);
void release(RIL_PreAlertingCallInfo& preAlertInfo);
void release(RIL_AutoCallRejectionInfo& autoRej);
void release(RIL_CallComposerInfo& ccInfo);
void release(RIL_EcnamInfo& ecnamInfo);
void release(RIL_SuppSvcResponse& sppSvcResp);
void release(RIL_AutoCallRejectionInfo& autoRej);
void release(RIL_CallInfo &arg);
}  // namespace utils
}  // namespace socket
}  // namespace ril

#endif  //__RIL_UTILS_H_
