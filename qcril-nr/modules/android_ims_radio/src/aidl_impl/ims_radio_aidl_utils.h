/*===========================================================================
   Copyright (c) 2017,2019-2023 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

#pragma once

#include <ImsRadioContext.h>

#include <interfaces/ims/ims.h>
#include <interfaces/sms/sms.h>
#include <interfaces/sms/QcRilUnsolMessageWaitingInfoMessage.h>
#include <interfaces/voice/voice.h>
#include <interfaces/voice/QcRilRequestDialMessage.h>
#include <interfaces/voice/QcRilUnsolDtmfMessage.h>
#include <interfaces/voice/QcRilUnsolImsSrtpEncryptionStatus.h>

#include <aidl/vendor/qti/hardware/radio/ims/BlockReasonDetails.h>
#include <aidl/vendor/qti/hardware/radio/ims/BlockReasonType.h>
#include <aidl/vendor/qti/hardware/radio/ims/BlockStatus.h>
#include <aidl/vendor/qti/hardware/radio/ims/CallComposerInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/CallDomain.h>
#include <aidl/vendor/qti/hardware/radio/ims/CallFailCause.h>
#include <aidl/vendor/qti/hardware/radio/ims/CallFailCauseResponse.h>
#include <aidl/vendor/qti/hardware/radio/ims/CallInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/CallLocation.h>
#include <aidl/vendor/qti/hardware/radio/ims/CallModifyFailCause.h>
#include <aidl/vendor/qti/hardware/radio/ims/CallPriority.h>
#include <aidl/vendor/qti/hardware/radio/ims/CallProgressInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/CallProgressInfoType.h>
#include <aidl/vendor/qti/hardware/radio/ims/CallForwardInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/CallForwardStatusInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/CallFwdTimerInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/CallState.h>
#include <aidl/vendor/qti/hardware/radio/ims/CallType.h>
#include <aidl/vendor/qti/hardware/radio/ims/CbNumInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/CbNumListInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/CiWlanNotificationInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/ClipProvisionStatus.h>
#include <aidl/vendor/qti/hardware/radio/ims/ClipStatus.h>
#include <aidl/vendor/qti/hardware/radio/ims/ClirInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/ClirMode.h>
#include <aidl/vendor/qti/hardware/radio/ims/Codec.h>
#include <aidl/vendor/qti/hardware/radio/ims/ColrInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/ComputedAudioQuality.h>
#include <aidl/vendor/qti/hardware/radio/ims/ConferenceCallState.h>
#include <aidl/vendor/qti/hardware/radio/ims/ConfigInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/ConfParticipantOperation.h>
#include <aidl/vendor/qti/hardware/radio/ims/Coordinate2D.h>
#include <aidl/vendor/qti/hardware/radio/ims/CrsType.h>
#include <aidl/vendor/qti/hardware/radio/ims/DialRequest.h>
#include <aidl/vendor/qti/hardware/radio/ims/DtmfInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/EmergencyCallRoute.h>
#include <aidl/vendor/qti/hardware/radio/ims/EmergencyServiceCategory.h>
#include <aidl/vendor/qti/hardware/radio/ims/EctType.h>
#include <aidl/vendor/qti/hardware/radio/ims/EcnamInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/ErrorCode.h>
#include <aidl/vendor/qti/hardware/radio/ims/FacilityType.h>
#include <aidl/vendor/qti/hardware/radio/ims/GeoLocationDataStatus.h>
#include <aidl/vendor/qti/hardware/radio/ims/HandoverType.h>
#include <aidl/vendor/qti/hardware/radio/ims/ImsSubConfigInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/IpPresentation.h>
#include <aidl/vendor/qti/hardware/radio/ims/MessageWaitingIndication.h>
#include <aidl/vendor/qti/hardware/radio/ims/MultiIdentityLineInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/MultiIdentityLineType.h>
#include <aidl/vendor/qti/hardware/radio/ims/MultiIdentityRegistrationStatus.h>
#include <aidl/vendor/qti/hardware/radio/ims/MultiSimVoiceCapability.h>
#include <aidl/vendor/qti/hardware/radio/ims/MwiMessagePriority.h>
#include <aidl/vendor/qti/hardware/radio/ims/MwiMessageType.h>
#include <aidl/vendor/qti/hardware/radio/ims/PreAlertingCallInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/RadioState.h>
#include <aidl/vendor/qti/hardware/radio/ims/RadioTechType.h>
#include <aidl/vendor/qti/hardware/radio/ims/RegFailureReasonType.h>
#include <aidl/vendor/qti/hardware/radio/ims/RegistrationInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/RegState.h>
#include <aidl/vendor/qti/hardware/radio/ims/RttMode.h>
#include <aidl/vendor/qti/hardware/radio/ims/NotificationType.h>
#include <aidl/vendor/qti/hardware/radio/ims/ServiceClassStatus.h>
#include <aidl/vendor/qti/hardware/radio/ims/ServiceClassProvisionStatus.h>
#include <aidl/vendor/qti/hardware/radio/ims/ServiceStatusInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/SipErrorInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/SmsCallBackMode.h>
#include <aidl/vendor/qti/hardware/radio/ims/SmsDeliverStatus.h>
#include <aidl/vendor/qti/hardware/radio/ims/SmsSendFailureReason.h>
#include <aidl/vendor/qti/hardware/radio/ims/SmsSendStatus.h>
#include <aidl/vendor/qti/hardware/radio/ims/SuppServiceStatus.h>
#include <aidl/vendor/qti/hardware/radio/ims/SuppSvcOperationType.h>
#include <aidl/vendor/qti/hardware/radio/ims/SsRequestType.h>
#include <aidl/vendor/qti/hardware/radio/ims/SsServiceType.h>
#include <aidl/vendor/qti/hardware/radio/ims/SsTeleserviceType.h>
#include <aidl/vendor/qti/hardware/radio/ims/StatusType.h>
#include <aidl/vendor/qti/hardware/radio/ims/StatusForAccessTech.h>
#include <aidl/vendor/qti/hardware/radio/ims/SystemServiceDomain.h>
#include <aidl/vendor/qti/hardware/radio/ims/SrtpEncryptionInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/TirMode.h>
#include <aidl/vendor/qti/hardware/radio/ims/ToneOperation.h>
#include <aidl/vendor/qti/hardware/radio/ims/TtyMode.h>
#include <aidl/vendor/qti/hardware/radio/ims/UssdModeType.h>
#include <aidl/vendor/qti/hardware/radio/ims/VerificationStatus.h>
#include <aidl/vendor/qti/hardware/radio/ims/VerstatInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/VoiceInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/VosActionInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/VosMoveInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/VosTouchInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/VoWiFiCallQuality.h>

namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace ims {
namespace utils {

namespace aidlimports {
using namespace ::aidl::vendor::qti::hardware::radio::ims;
}
bool convertBytesToHexString(const uint8_t* bytePayload, size_t size, std::string& str);
bool convertPayloadToCdmaFormat(const uint8_t* payload, size_t size, RIL_CDMA_SMS_Message& cdmaMsg);
void convertRilImsResponseToAidlResponse(RIL_Errno rilErr, RIL_RadioTechnologyFamily tech,
                                         std::optional<bool> bLteOnlyReg,
                                         aidlimports::SmsSendStatus& statusReport,
                                         aidlimports::SmsSendFailureReason& failureReason);
aidlimports::VerificationStatus convertToAidlVerificationStatus(
    const qcril::interfaces::VerificationStatus& in);
bool convertCdmaFormatToPseudoPdu(RIL_CDMA_SMS_Message& cdmaMsg, std::vector<uint8_t>& payload);
void convertRilImsSubConfigInfoToAidlResponse(aidlimports::ImsSubConfigInfo& subConfigInfo,
                                              qcril::interfaces::ImsSubConfigInfo& in);
aidlimports::ErrorCode convertRilErrorToAidlImsError(RIL_Errno rilError);
aidlimports::ErrorCode convertDispatchStatusToAidlErrorCode(Message::Callback::Status status);
aidlimports::RadioState convertToAidlRadioState(qcril::interfaces::RadioState inState);
bool convertRilRegistrationToAidlResponse(aidlimports::RegistrationInfo& out,
                                          const qcril::interfaces::Registration& in);
qcril::interfaces::RegState convertToRilRegState(const aidlimports::RegState& in);
bool convertRilServiceStatusInfoListToAidlResponse(
    std::vector<aidlimports::ServiceStatusInfo>& out,
    const qcril::interfaces::ServiceStatusInfoList& in);
bool convertRilServiceStatusInfoListToAidlResponse(
    std::vector<aidlimports::ServiceStatusInfo>& out,
    const std::vector<qcril::interfaces::ServiceStatusInfo>& in);
aidlimports::ToneOperation convertToAidlToneOperation(
    const qcril::interfaces::RingBackToneOperation& in);
/*
 * Modem is updating the SIP error what ever they received as part of supp service request. Due to
 * this we are getting sip_error code as 200 even in case of supp service request is success.
 * When we discussed with modem team they confirmed that its expected behavior to update SIP error
 * even request is success hence legacy behavior is to drop the sip error in QMIRIL if there is no
 * sip error message and same fallowing now.
 * Introduced considerErrorString to control when we need to consider error string.
 * By default considerErrorString will be true so convertToAidlSipErrorInfo will return false if
 * there is no sip error string.
 */
bool convertToAidlSipErrorInfo(aidlimports::SipErrorInfo& out,
                               const std::shared_ptr<qcril::interfaces::SipErrorInfo>& in,
                               bool considerErrorString = true);
qcril::interfaces::CallType convertToRilCallType(const aidlimports::CallType& in);
qcril::interfaces::Presentation convertToRilIpPresentation(const aidlimports::IpPresentation& in);
qcril::interfaces::RttMode convertToRilRttMode(const aidlimports::RttMode& in);
qcril::interfaces::CallFailCause convertToRilCallFailCause(const aidlimports::CallFailCause& in);
bool convertToAidlCallInfoList(std::vector<aidlimports::CallInfo>& out,
                               const std::vector<qcril::interfaces::CallInfo>& in);
std::shared_ptr<QcRilRequestDialMessage> makeQcRilRequestDialMessage(
    std::shared_ptr<ImsRadioContext> ctx, const aidlimports::DialRequest& dialRequest);
void convertToRilMultiIdentityInfo(const aidlimports::MultiIdentityLineInfo& in_line,
                                   qcril::interfaces::MultiIdentityInfo& out_line);
void convertToRilCallComposerInfo(qcril::interfaces::CallComposerInfo& out,
                                  const aidlimports::CallComposerInfo& in);
void convertToAidlCallComposerInfo(aidlimports::CallComposerInfo& out,
                                   const qcril::interfaces::CallComposerInfo& in);
uint64_t convertToRilEmergencyServiceCategories(const int32_t categories);
qcril::interfaces::EmergencyCallRouting convertToRilEmergencyCallRoute(
    aidlimports::EmergencyCallRoute route);
qcril::interfaces::CallDomain convertToRilCallDomain(const aidlimports::CallDomain& in);
aidlimports::CallDomain convertToAidlCallDomain(const qcril::interfaces::CallDomain& in);
bool convertToAidlColrInfo(aidlimports::ColrInfo& out,
                           const std::shared_ptr<qcril::interfaces::ColrInfo>& in);
void convertToAidlVirtualLines(std::vector<std::string>& out, const std::vector<std::string>& in);
aidlimports::CallModifyFailCause convertToAidlCallModifyFailCause(
    const qcril::interfaces::CallModifyFailCause& in);
void convertToAidlMultiIdentityInfo(const std::vector<qcril::interfaces::MultiIdentityInfo>& in_lines,
                                    std::vector<aidlimports::MultiIdentityLineInfo>& out_lines);
aidlimports::RttMode convertToAidlRttMode(const qcril::interfaces::RttMode& in);
aidlimports::CallType convertToAidlCallType(const qcril::interfaces::CallType& in);
aidlimports::HandoverType convertToAidlHandoverType(const qcril::interfaces::HandoverType& in);
aidlimports::RadioTechType convertToAidlRadioTech(const RIL_RadioTechnology& in);
aidlimports::CallFailCause convertToAidlCallFailCause(const qcril::interfaces::CallFailCause& in);
void convertToAidlRegBlockStatus(aidlimports::BlockStatus& outStatus,
                                 const qcril::interfaces::BlockStatus& in);
aidlimports::ConferenceCallState convertToAidlConferenceCallState(
    const qcril::interfaces::ConferenceCallState& in);
aidlimports::ConfParticipantOperation convertToAidlConfParticipantOperation(
    const qcril::interfaces::ConfParticipantOperation& in);
bool isUssdOverImsSupported();
void convertToRilConfigInfo(const aidlimports::ConfigInfo& inConfig,
                            qcril::interfaces::ConfigInfo& outConfig);
void convertToAidlConfigInfo(aidlimports::ConfigInfo& outConfig,
                             const qcril::interfaces::ConfigInfo& inConfig);
bool convertToAidlClipInfo(aidlimports::ClipProvisionStatus& out,
                           const std::shared_ptr<qcril::interfaces::ClipInfo>& in);
bool convertToAidlClirInfo(aidlimports::ClirInfo& out,
                           const std::shared_ptr<qcril::interfaces::ClirInfo>& in);
qcril::interfaces::TtyMode convertToRilTtyMode(const aidlimports::TtyMode& in);
bool convertToAidlCallForwardInfoList(std::vector<aidlimports::CallForwardInfo>& out,
                                      const std::vector<qcril::interfaces::CallForwardInfo>& in);
bool convertToRilCallFwdTimerInfo(std::shared_ptr<qcril::interfaces::CallFwdTimerInfo>& out,
                                  const aidlimports::CallFwdTimerInfo& in);
aidlimports::ServiceClassStatus convertToAidlServiceClassStatus(
    const qcril::interfaces::ServiceClassStatus& in);
qcril::interfaces::ServiceClassStatus convertToRilServiceClassStatus(
    const aidlimports::ServiceClassStatus& in);
void convertToAidlSuppServiceStatus(
    aidlimports::SuppServiceStatus& out,
    const qcril::interfaces::SuppServiceStatusInfo& in);
qcril::interfaces::EctType convertToRilEctType(const aidlimports::EctType& in);
qcril::interfaces::FacilityType convertToRilFacilityType(const aidlimports::FacilityType& in);
qcril::interfaces::DeliverStatus convertAidlToRilSmsAckResult(
    aidlimports::SmsDeliverStatus smsdeliverResult);
aidlimports::TtyMode convertToAidlTtyMode(const qcril::interfaces::TtyMode& in);
aidlimports::NotificationType convertToAidlNotificationType(
    const qcril::interfaces::NotificationType& in);
void convertToAidlVowifiCallQuality(aidlimports::VoWiFiCallQuality& out,
                                    qcril::interfaces::VowifiQuality in);
bool isServiceTypeCfQuery(aidlimports::SsServiceType serType, aidlimports::SsRequestType reqType);
bool isServiceTypeIcbQueryWithNumber(aidlimports::SsServiceType serType,
                                     aidlimports::SsRequestType reqType);
aidlimports::UssdModeType convertToAidlUssdModeType(const qcril::interfaces::UssdModeType& in);
aidlimports::GeoLocationDataStatus convertToAidlGeoLocationDataStatus(
    qcril::interfaces::GeoLocationDataStatus in);
bool convertToAidlCallForwardStatusInfo(
    aidlimports::CallForwardStatusInfo& out,
    const std::shared_ptr<qcril::interfaces::SetCallForwardRespData>& in);
void convertToAidlServiceType(aidlimports::SsServiceType& out, RIL_SsServiceType in);
void convertToAidlRequestType(aidlimports::SsRequestType& out, RIL_SsRequestType in);
void convertToAidlTeleserviceType(aidlimports::SsTeleserviceType& out, RIL_SsTeleserviceType in);
int convertToRilOperationType(const aidlimports::SuppSvcOperationType& in);
::aidl::vendor::qti::hardware::radio::ims::VoiceInfo convertToAidlVoiceInfo(
    const qcril::interfaces::VoiceInfo& in);
::aidl::vendor::qti::hardware::radio::ims::SystemServiceDomain convertToAidlSystemServiceDomain(
    const qcril::interfaces::SrvDomain& in);
::aidl::vendor::qti::hardware::radio::ims::SmsCallBackMode convertToAidlSmsCallBackMode(
    const qcril::interfaces::SmsCallbackModeStatus& in);
aidlimports::MessageWaitingIndication convertToAidlMessageWaitingIndication(
    std::shared_ptr<QcRilUnsolMessageWaitingInfoMessage> msg);
::aidl::vendor::qti::hardware::radio::ims::MwiMessageType convertToAidlMessageType(
    const qcril::interfaces::MessageType& in);
::aidl::vendor::qti::hardware::radio::ims::MwiMessagePriority convertToAidlMessagePriority(
    const qcril::interfaces::MessagePriority& in);
aidlimports::DtmfInfo convertToAidlDtmfInfo(QcRilUnsolDtmfMessage& msg);
aidlimports::MultiSimVoiceCapability convertToAidlMultiSimVoiceCapability(
    const qcril::interfaces::MultiSimVoiceCapability& in);
void convertToAidlPreAlertingCallInfo(aidlimports::PreAlertingCallInfo& out,
                                      const qcril::interfaces::PreAlertingCallInfo& in);
aidlimports::CiWlanNotificationInfo convertToAidlCIWlanNotification(
    const qcril::interfaces::CiWlanNotificationInfo& in);
aidlimports::SrtpEncryptionInfo convertToAidlSrtpEncryptionInfo(
    QcRilUnsolImsSrtpEncryptionStatus& in);
void convertToAidlEcnamInfo(aidlimports::EcnamInfo& out, const qcril::interfaces::EcnamInfo& in);
void convertToRilVosActionInfo(qcril::interfaces::VosActionInfo& out,
                               const aidlimports::VosActionInfo& in);
qcril::interfaces::VosMoveInfo convertToRilVosMoveInfo(const aidlimports::VosMoveInfo& in);
qcril::interfaces::VosTouchInfo convertToRilVosTouchInfo(const aidlimports::VosTouchInfo& in);
qcril::interfaces::Coordinate2D convertToRilCoordinate(const aidlimports::Coordinate2D& in);
}  // namespace utils
}  // namespace ims
}  // namespace radio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
