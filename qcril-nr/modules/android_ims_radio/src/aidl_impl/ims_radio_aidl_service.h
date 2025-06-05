/*===========================================================================
   Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

#pragma once

#include <framework/legacy.h>
#include <interfaces/ims/QcRilUnsolImsRegStateMessage.h>
#include <interfaces/ims/QcRilUnsolImsRttMessage.h>
#include <interfaces/ims/QcRilUnsolImsVowifiCallQuality.h>
#include <interfaces/ims/QcRilUnsolImsVopsIndication.h>
#include <interfaces/ims/QcRilUnsolImsSubConfigIndication.h>
#include <interfaces/ims/QcRilUnsolImsGeoLocationInfo.h>
#include <interfaces/ims/QcRilUnsolImsPreAlertingCallInfo.h>
#include <interfaces/ims/QcRilUnsolImsSmsCallbackModeMessage.h>
#include <interfaces/ims/QcRilUnsolImsSrvStatusIndication.h>
#include <interfaces/ims/QcRilUnsolImsSsacInfoIndication.h>
#include <interfaces/ims/QcRilUnsolImsRegBlockStatusMessage.h>
#include <interfaces/ims/QcRilUnsolImsPendingMultiLineStatus.h>
#include <interfaces/ims/QcRilUnsolImsMultiIdentityStatusMessage.h>
#include <interfaces/ims/QcRilUnsolImsMultiSimVoiceCapabilityChanged.h>
#include <interfaces/ims/QcRilUnsolImsVoiceInfo.h>
#include <interfaces/ims/QcRilUnsolImsRadioStateIndication.h>
#include <interfaces/ims/QcRilUnsolImsWfcRoamingConfigIndication.h>
#include <interfaces/ims/QcRilUnsolImsGeoLocationDataStatus.h>
#include <interfaces/ims/QcRilUnsolImsCIwlanNotification.h>
#include <interfaces/nas/RilUnsolNasSysInfo.h>
#include <interfaces/nas/RilUnsolEmergencyCallbackModeMessage.h>

#include <QtiMutex.h>
#include <ImsRadioContext.h>
#include "telephony/ril.h"

#include <ImsRadioContext.h>
#include <framework/legacy.h>
#include <interfaces/ims/QcRilUnsolImsRegStateMessage.h>
#include <interfaces/voice/QcRilUnsolAutoCallRejectionMessage.h>
#include <interfaces/voice/QcRilUnsolSupplementaryServiceMessage.h>
#include <interfaces/voice/QcRilUnsolConfParticipantStatusInfoMessage.h>
#include <interfaces/voice/QcRilUnsolCallRingingMessage.h>
#include <interfaces/voice/QcRilUnsolDtmfMessage.h>
#include <interfaces/voice/QcRilUnsolCallStateChangeMessage.h>
#include <interfaces/voice/QcRilUnsolImsConferenceInfoMessage.h>
#include <interfaces/voice/QcRilUnsolImsConferenceCallStateCompletedMessage.h>
#include <interfaces/voice/QcRilUnsolImsHandoverMessage.h>
#include <interfaces/voice/QcRilUnsolImsModifyCallMessage.h>
#include <interfaces/voice/QcRilUnsolImsSrtpEncryptionStatus.h>
#include <interfaces/voice/QcRilUnsolRingbackToneMessage.h>
#include <interfaces/voice/QcRilUnsolImsTtyNotificationMessage.h>
#include <interfaces/voice/QcRilUnsolImsViceInfoMessage.h>
#include <interfaces/voice/QcRilUnsolSuppSvcNotificationMessage.h>
#include <interfaces/sms/QcRilUnsolMessageWaitingInfoMessage.h>
#include <interfaces/voice/QcRilUnsolOnUssdMessage.h>
#include <interfaces/voice/QcRilUnsolOnSipDtmfMessage.h>
#include <interfaces/sms/qcril_qmi_sms_types.h>
#include <interfaces/sms/RilUnsolIncomingImsSMSMessage.h>
#include <interfaces/sms/RilUnsolNewImsSmsStatusReportMessage.h>
#include <interfaces/QcRilRequestMessage.h>

#include <binder/IBinder.h>
#include <aidl/vendor/qti/hardware/radio/ims/AcknowledgeSmsInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/AcknowledgeSmsReportInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/AnswerRequest.h>
#include <aidl/vendor/qti/hardware/radio/ims/BnImsRadio.h>
#include <aidl/vendor/qti/hardware/radio/ims/ClirInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/ColrInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/ConfigInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/CallForwardInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/CallModifyInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/CallWaitingInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/DeflectRequestInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/DialRequest.h>
#include <aidl/vendor/qti/hardware/radio/ims/DtmfInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/EmergencyDialRequest.h>
#include <aidl/vendor/qti/hardware/radio/ims/ErrorCode.h>
#include <aidl/vendor/qti/hardware/radio/ims/ExplicitCallTransferInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/GeoLocationInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/HangupRequestInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/MediaConfig.h>
#include <aidl/vendor/qti/hardware/radio/ims/MultiIdentityLineInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/MultiSimVoiceCapability.h>
#include <aidl/vendor/qti/hardware/radio/ims/RegState.h>
#include <aidl/vendor/qti/hardware/radio/ims/ServiceClassStatus.h>
#include <aidl/vendor/qti/hardware/radio/ims/ServiceStatusInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/SipErrorInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/SmsSendFailureReason.h>
#include <aidl/vendor/qti/hardware/radio/ims/SmsSendRequest.h>
#include <aidl/vendor/qti/hardware/radio/ims/SmsSendStatus.h>
#include <aidl/vendor/qti/hardware/radio/ims/SuppServiceStatusRequest.h>
#include <aidl/vendor/qti/hardware/radio/ims/TtyInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/VerificationStatus.h>

namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace ims {
namespace implementation {

namespace aidlimports {
using namespace aidl::vendor::qti::hardware::radio::ims;
}

/** Class implementing the IImsRadio server implementation for all requests coming in
 * from Ims to RIL. Also registers the callback of type IImsRadioResponse to send the
 * solicited response back and IImsRadioIndication to send unsolicited responses.
 */
class ImsRadioAidlImpl : public aidlimports::BnImsRadio {
 private:
  std::shared_ptr<aidlimports::IImsRadioResponse> mImsRadioResponseCb;
  std::shared_ptr<aidlimports::IImsRadioIndication> mImsRadioIndicationCb;
  qcril_instance_id_e_type mInstanceId;
  AIBinder_DeathRecipient* mDeathRecipient = nullptr;
  qtimutex::QtiSharedMutex mCallbackLock;

 protected:
  std::shared_ptr<aidlimports::IImsRadioIndication> getIndicationCallback();
  std::shared_ptr<aidlimports::IImsRadioResponse> getResponseCallback();

 public:
  ImsRadioAidlImpl(qcril_instance_id_e_type instance);
  ~ImsRadioAidlImpl();

  android::status_t registerService();
  void setCallback_nolock(
      const std::shared_ptr<aidlimports::IImsRadioResponse>& imsRadioResponse,
      const std::shared_ptr<aidlimports::IImsRadioIndication>& imsRadioIndication);
  void clearCallbacks();
  qcril_instance_id_e_type getInstanceId() {
    return mInstanceId;
  }
  std::shared_ptr<ImsRadioContext> getContext(uint32_t serial) {
    std::shared_ptr<ImsRadioContext> ctx = std::make_shared<ImsRadioContext>(mInstanceId, serial);
    return ctx;
  }
  std::string hidePhoneNumber(std::string logStr);
  void imsRadiolog(std::string dir, std::string logStr);
  void notifyImsClientConnected();
  void deathNotifier();

  // AIDL APIs
  ::ndk::ScopedAStatus setCallback(
      const std::shared_ptr<aidlimports::IImsRadioResponse>& in_imsRadioResponse,
      const std::shared_ptr<aidlimports::IImsRadioIndication>& in_imsRadioIndication) override;
  ::ndk::ScopedAStatus dial(int32_t in_token,
                            const aidlimports::DialRequest& in_dialRequest) override;
  ::ndk::ScopedAStatus addParticipant(int32_t in_token,
                                      const aidlimports::DialRequest& in_dialRequest) override;
  ::ndk::ScopedAStatus getImsRegistrationState(int32_t in_token) override;
  ::ndk::ScopedAStatus answer(int32_t in_token,
                              const aidlimports::AnswerRequest& in_answerRequest) override;
  ::ndk::ScopedAStatus hangup(int32_t in_token,
                              const aidlimports::HangupRequestInfo& in_hangup) override;
  ::ndk::ScopedAStatus requestRegistrationChange(int32_t in_token,
                                                 aidlimports::RegState in_state) override;
  ::ndk::ScopedAStatus queryServiceStatus(int32_t in_token) override;
  ::ndk::ScopedAStatus setServiceStatus(
      int32_t in_token,
      const std::vector<aidlimports::ServiceStatusInfo>& in_srvStatusInfoList) override;
  ::ndk::ScopedAStatus hold(int32_t in_token, int32_t in_callId) override;
  ::ndk::ScopedAStatus resume(int32_t in_token, int32_t in_callId) override;
  ::ndk::ScopedAStatus setConfig(int32_t in_token,
                                 const aidlimports::ConfigInfo& in_config) override;
  ::ndk::ScopedAStatus getConfig(int32_t in_token,
                                 const aidlimports::ConfigInfo& in_config) override;
  ::ndk::ScopedAStatus conference(int32_t in_token) override;
  ::ndk::ScopedAStatus getClip(int32_t in_token) override;
  ::ndk::ScopedAStatus getClir(int32_t in_token) override;
  ::ndk::ScopedAStatus setClir(int32_t in_token, const aidlimports::ClirInfo& in_clirInfo) override;
  ::ndk::ScopedAStatus getColr(int32_t in_token) override;
  ::ndk::ScopedAStatus setColr(int32_t in_token, const aidlimports::ColrInfo& in_colrInfo) override;
  ::ndk::ScopedAStatus exitEmergencyCallbackMode(int32_t in_token) override;
  ::ndk::ScopedAStatus sendDtmf(int32_t in_token, const aidlimports::DtmfInfo& in_dtmfInfo) override;
  ::ndk::ScopedAStatus startDtmf(int32_t in_token,
                                 const aidlimports::DtmfInfo& in_dtmfInfo) override;
  ::ndk::ScopedAStatus stopDtmf(int32_t in_token) override;
  ::ndk::ScopedAStatus setUiTtyMode(int32_t in_token,
                                    const aidlimports::TtyInfo& in_ttyInfo) override;
  ::ndk::ScopedAStatus modifyCallInitiate(
      int32_t in_token, const aidlimports::CallModifyInfo& in_callModifyInfo) override;
  ::ndk::ScopedAStatus modifyCallConfirm(
      int32_t in_token, const aidlimports::CallModifyInfo& in_callModifyInfo) override;
  ::ndk::ScopedAStatus queryCallForwardStatus(
      int32_t in_token, const aidlimports::CallForwardInfo& in_callForwardInfo) override;
  ::ndk::ScopedAStatus setCallForwardStatus(
      int32_t in_token, const aidlimports::CallForwardInfo& in_callForwardInfo) override;
  ::ndk::ScopedAStatus getCallWaiting(int32_t in_token, int32_t in_serviceClass) override;
  ::ndk::ScopedAStatus setCallWaiting(
      int32_t in_token, const aidlimports::CallWaitingInfo& in_callWaitingInfo) override;
  ::ndk::ScopedAStatus setSuppServiceNotification(int32_t in_token,
                                                  aidlimports::ServiceClassStatus in_status) override;
  ::ndk::ScopedAStatus explicitCallTransfer(
      int32_t in_token, const aidlimports::ExplicitCallTransferInfo& in_ectInfo) override;
  ::ndk::ScopedAStatus suppServiceStatus(
      int32_t in_token,
      const aidlimports::SuppServiceStatusRequest& in_suppServiceStatusRequest) override;
  ::ndk::ScopedAStatus getRtpStatistics(int32_t in_token) override;
  ::ndk::ScopedAStatus getRtpErrorStatistics(int32_t in_token) override;
  ::ndk::ScopedAStatus deflectCall(
      int32_t in_token, const aidlimports::DeflectRequestInfo& in_deflectRequestInfo) override;
  ::ndk::ScopedAStatus sendGeolocationInfo(
      int32_t in_token, const aidlimports::GeoLocationInfo& in_geoLocationInfo) override;
  ::ndk::ScopedAStatus getImsSubConfig(int32_t in_token) override;
  ::ndk::ScopedAStatus sendRttMessage(int32_t in_token, const std::string& in_message) override;
  ::ndk::ScopedAStatus cancelModifyCall(int32_t in_token, int32_t in_callId) override;
  ::ndk::ScopedAStatus sendSms(int32_t in_token,
                               const aidlimports::SmsSendRequest& in_smsRequest) override;
  ::ndk::ScopedAStatus acknowledgeSms(int32_t in_token,
                                      const aidlimports::AcknowledgeSmsInfo& in_smsInfo) override;
  ::ndk::ScopedAStatus acknowledgeSmsReport(
      int32_t in_token, const aidlimports::AcknowledgeSmsReportInfo& in_smsReportInfo) override;
  ::ndk::ScopedAStatus getSmsFormat(std::string* _aidl_return) override;
  ::ndk::ScopedAStatus registerMultiIdentityLines(
      int32_t in_token, const std::vector<aidlimports::MultiIdentityLineInfo>& in_info) override;
  ::ndk::ScopedAStatus queryVirtualLineInfo(int32_t in_token, const std::string& in_msisdn) override;
  ::ndk::ScopedAStatus emergencyDial(
      int32_t in_token, const aidlimports::EmergencyDialRequest& in_dialRequest) override;
  ::ndk::ScopedAStatus sendSipDtmf(int32_t in_token, const std::string& in_requestCode) override;
  ::ndk::ScopedAStatus sendUssd(int32_t in_token, const std::string& in_ussd) override;
  ::ndk::ScopedAStatus cancelPendingUssd(int32_t in_token) override;
  ::ndk::ScopedAStatus callComposerDial(
      int32_t in_token, const aidlimports::CallComposerDialRequest& in_dialRequest) override;
  ::ndk::ScopedAStatus setMediaConfiguration(int32_t in_token,
      const aidlimports::MediaConfig& in_config) override;
  ::ndk::ScopedAStatus queryMultiSimVoiceCapability(int32_t in_token) override;
  ::ndk::ScopedAStatus exitSmsCallBackMode(int32_t in_token) override;
  ::ndk::ScopedAStatus sendVosSupportStatus(int32_t in_token, bool in_isVosSupport) override;
  ::ndk::ScopedAStatus sendVosActionInfo(int32_t in_token,
        const aidlimports::VosActionInfo& in_vosActionInfo) override;

  // Unsol message handlers
  void notifyOnRegistrationChanged(std::shared_ptr<QcRilUnsolImsRegStateMessage> msg);
  void notifyOnRTTMessage(std::shared_ptr<QcRilUnsolImsRttMessage> msg);
  void notifyOnVowifiCallQuality(std::shared_ptr<QcRilUnsolImsVowifiCallQuality> msg);
  void notifyOnVopsChanged(std::shared_ptr<QcRilUnsolImsVopsIndication> msg);
  void notifyOnSubConfigChanged(std::shared_ptr<QcRilUnsolImsSubConfigIndication> msg);
  void notifyOnGeoLocationChange(std::shared_ptr<QcRilUnsolImsGeoLocationInfo> msg);
  void notifyOnServiceStatusChange(std::shared_ptr<QcRilUnsolImsSrvStatusIndication> msg);
  void notifyOnSsacInfoChange(
      std::shared_ptr<QcRilUnsolImsSsacInfoIndication> msg);  // remove this as it's not bein used?
  void notifyOnRegBlockStatusChange(std::shared_ptr<QcRilUnsolImsRegBlockStatusMessage> msg);
  void notifyOnCallStateChanged(std::shared_ptr<QcRilUnsolCallStateChangeMessage> msg);
  void notifyOnModifyCall(std::shared_ptr<QcRilUnsolImsModifyCallMessage> msg);
  void notifyOnRing(std::shared_ptr<QcRilUnsolCallRingingMessage> msg);
  void notifyOnRingbackTone(std::shared_ptr<QcRilUnsolRingbackToneMessage> msg);
  void notifyOnHandover(std::shared_ptr<QcRilUnsolImsHandoverMessage> msg);
  void notifyOnTtyNotification(std::shared_ptr<QcRilUnsolImsTtyNotificationMessage> msg);
  void notifyOnRefreshConferenceInfo(std::shared_ptr<QcRilUnsolImsConferenceInfoMessage> msg);
  void notifyOnRefreshViceInfo(std::shared_ptr<QcRilUnsolImsViceInfoMessage> msg);
  void notifyOnSuppServiceNotification(std::shared_ptr<QcRilUnsolSuppSvcNotificationMessage> msg);
  void notifyOnParticipantStatusInfo(std::shared_ptr<QcRilUnsolConfParticipantStatusInfoMessage> msg);
  void notifyOnIncomingCallAutoRejected(std::shared_ptr<QcRilUnsolAutoCallRejectionMessage> msg);
  void notifyOnSupplementaryServiceIndication(
      std::shared_ptr<QcRilUnsolSupplementaryServiceMessage> msg);
  void notifyIncomingSms(std::shared_ptr<RilUnsolIncomingImsSMSMessage> msg);
  void notifyNewSmsStatusReport(std::shared_ptr<RilUnsolNewImsSmsStatusReportMessage> msg);
  void notifyOnPendingMultiIdentityStatus(std::shared_ptr<QcRilUnsolImsPendingMultiLineStatus> msg);
  void notifyOnMultiIdentityLineStatus(std::shared_ptr<QcRilUnsolImsMultiIdentityStatusMessage> msg);
  void notifyOnVoiceInfoStatusChange(std::shared_ptr<QcRilUnsolImsVoiceInfo> msg);
  void notifyOnExitEcbmIndication(std::shared_ptr<RilUnsolEmergencyCallbackModeMessage> msg);
  void notifyOnEnterEcbmIndication(std::shared_ptr<RilUnsolEmergencyCallbackModeMessage> msg);
  void notifyOnRadioStateChanged(std::shared_ptr<QcRilUnsolImsRadioStateIndication> msg);
  void notifyOnMessageWaiting(std::shared_ptr<QcRilUnsolMessageWaitingInfoMessage> msg);
  void notifyOnModemSupportsWfcRoamingModeConfiguration(
      std::shared_ptr<QcRilUnsolImsWfcRoamingConfigIndication> msg);
  void notifyOnUssdMessageFailed(std::shared_ptr<QcRilUnsolOnUssdMessage> msg);
  void notifyOnRetrievingGeoLocationDataStatus(
      std::shared_ptr<QcRilUnsolImsGeoLocationDataStatus> msg);
  void notifyOnUssdMessage(std::shared_ptr<QcRilUnsolOnUssdMessage> msg);
  void notifyOnSipDtmfReceived(std::shared_ptr<QcRilUnsolOnSipDtmfMessage> msg);
  void notifyOnServiceDomainChanged(std::shared_ptr<RilUnsolNasSysInfo> msg);
  void notifyOnConferenceCallStateCompleted(
      std::shared_ptr<QcRilUnsolImsConferenceCallStateCompletedMessage> msg);
  void notifyOnSmsCallBackModeChanged(
      std::shared_ptr<QcRilUnsolImsSmsCallbackModeMessage> msg);
  void notifyOnIncomingDtmfStart(std::shared_ptr<QcRilUnsolDtmfMessage> msg);
  void notifyOnIncomingDtmfStop(std::shared_ptr<QcRilUnsolDtmfMessage> msg);
  void notifyOnMultiSimVoiceCapabilityChanged(
      std::shared_ptr<QcRilUnsolImsMultiSimVoiceCapabilityChanged> msg);
  void notifyOnPreAlertingCallInfo(std::shared_ptr<QcRilUnsolImsPreAlertingCallInfo> msg);
  void notifyOnCIWlanNotification(std::shared_ptr<QcRilUnsolImsCIwlanNotification> msg);
  void notifySrtpEnscryptionStatus(std::shared_ptr<QcRilUnsolImsSrtpEncryptionStatus> msg);

 private:
  // Send response on IImsRadioResponse
  void sendRequestRegistrationChangeResponse(int32_t token, aidlimports::ErrorCode errorCode,
                                             std::shared_ptr<qcril::interfaces::BasePayload> data);
  void sendGetRegistrationResponse(int32_t token, aidlimports::ErrorCode errorCode,
                                   std::shared_ptr<qcril::interfaces::Registration> data);
  void sendQueryServiceStatusResponse(int32_t token, aidlimports::ErrorCode errorCode,
                                      std::shared_ptr<qcril::interfaces::ServiceStatusInfoList> data);
  void sendDialResponse(int32_t token, aidlimports::ErrorCode errorCode);
  void sendAnswerResponse(int32_t token, aidlimports::ErrorCode errorCode);
  void sendHangupResponse(int32_t token, aidlimports::ErrorCode errorCode);
  void sendHoldResponse(int32_t token, aidlimports::ErrorCode errorCode,
                        std::shared_ptr<qcril::interfaces::SipErrorInfo> data);
  void sendResumeResponse(int32_t token, aidlimports::ErrorCode errorCode,
                          std::shared_ptr<qcril::interfaces::SipErrorInfo> data);
  void sendQueryCallForwardStatusResponse(
      int32_t token, aidlimports::ErrorCode errorCode,
      const std::shared_ptr<qcril::interfaces::GetCallForwardRespData> data);
  void sendGetCallWaitingResponse(int32_t token, aidlimports::ErrorCode errorCode,
                                  const std::shared_ptr<qcril::interfaces::CallWaitingInfo> data);
  void sendConferenceResponse(int32_t token, aidlimports::ErrorCode errorCode,
                              const std::shared_ptr<qcril::interfaces::SipErrorInfo> data);
  void sendGetClipResponse(int32_t token, aidlimports::ErrorCode errorCode,
                           const std::shared_ptr<qcril::interfaces::ClipInfo> data);
  void sendGetClirResponse(int32_t token, aidlimports::ErrorCode errorCode,
                           const std::shared_ptr<qcril::interfaces::ClirInfo> data);
  void sendSetClirResponse(int32_t token, aidlimports::ErrorCode errorCode);
  void sendGetColrResponse(int32_t token, aidlimports::ErrorCode errorCode,
                           const std::shared_ptr<qcril::interfaces::ColrInfo> data);
  void sendDtmfResponse(int32_t token, aidlimports::ErrorCode errorCode);
  void sendStartDtmfResponse(int32_t token, aidlimports::ErrorCode errorCode);
  void sendStopDtmfResponse(int32_t token, aidlimports::ErrorCode errorCode);
  void sendExitEmergencyCallbackModeResponse(int32_t token, aidlimports::ErrorCode errorCode);
  void sendSetUiTTYModeResponse(int32_t token, aidlimports::ErrorCode errorCode);
  void sendExplicitCallTransferResponse(int32_t token, aidlimports::ErrorCode errorCode);
  void sendModifyCallInitiateResponse(int32_t token, aidlimports::ErrorCode errorCode);
  void sendCancelModifyCallResponse(int32_t token, aidlimports::ErrorCode errorCode);
  void sendModifyCallConfirmResponse(int32_t token, aidlimports::ErrorCode errorCode);
  void sendAddParticipantResponse(int32_t token, aidlimports::ErrorCode errorCode);
  void sendDeflectCallResponse(int32_t token, aidlimports::ErrorCode errorCode);
  void sendSetSuppServiceNotificationResponse(int32_t token, aidlimports::ErrorCode errorCode);
  void sendGetRtpStatsMessageResponse(int32_t token, aidlimports::ErrorCode errorCode,
                                      std::shared_ptr<qcril::interfaces::RtpStatisticsData> data);
  void sendGetRtpErrorStatsMessageResponse(
      int32_t token, aidlimports::ErrorCode errorCode,
      std::shared_ptr<qcril::interfaces::RtpStatisticsData> data);
  void sendGetImsSubConfigMessageResponse(int32_t token, aidlimports::ErrorCode errorCode,
                                          std::shared_ptr<qcril::interfaces::ImsSubConfigInfo> data);
  void sendSmsResponse(int32_t token, RIL_Errno errorCode, RIL_RadioTechnologyFamily tech,
                          std::shared_ptr<RilSendSmsResult_t> data);
  void sendRttMessageResponse(int32_t token, aidlimports::ErrorCode errorCode,
                              std::shared_ptr<qcril::interfaces::BasePayload> data);
  void sendSetServiceStatusMessageResponse(int32_t token, aidlimports::ErrorCode errorCode,
                                           std::shared_ptr<qcril::interfaces::BasePayload> data);
  void sendGeoLocationResponse(int32_t token, aidlimports::ErrorCode errorCode,
                               std::shared_ptr<qcril::interfaces::BasePayload> data);
  void sendGetConfigMessageResponse(int32_t token, aidlimports::ErrorCode errorCode,
                                    std::shared_ptr<qcril::interfaces::ConfigInfo> data);
  void sendSetConfigMessageResponse(int32_t token, aidlimports::ErrorCode errorCode,
                                    std::shared_ptr<qcril::interfaces::ConfigInfo> data);
  void sendSuppServiceStatusResponse(
      int32_t token, aidlimports::ErrorCode errorCode,
      const std::shared_ptr<qcril::interfaces::SuppServiceStatusInfo> data);
  void sendRegisterMultiIdentityLinesResponse(int32_t token, aidlimports::ErrorCode errorCode,
                                              std::shared_ptr<qcril::interfaces::BasePayload> data);
  void sendQueryVirtualLinesResponse(int32_t token, aidlimports::ErrorCode errorCode,
                                     std::shared_ptr<qcril::interfaces::VirtualLineInfo> data);
  void sendUssdResponse(int32_t token, aidlimports::ErrorCode errorCode,
                        std::shared_ptr<qcril::interfaces::SipErrorInfo> data);
  void cancelPendingUssdResponse(int32_t token, aidlimports::ErrorCode errorCode,
                                 std::shared_ptr<qcril::interfaces::SipErrorInfo> data);
  void sendSipDtmfResponse(int32_t token, aidlimports::ErrorCode errorCode);
  void sendSetCallForwardStatusResponse(
      int32_t token, aidlimports::ErrorCode errorCode,
      const std::shared_ptr<qcril::interfaces::SetCallForwardRespData> data);
  void sendSetMediaConfigurationResponse(int32_t token, aidlimports::ErrorCode errorCode);
  void sendQueryMultiSimVoiceCapabilityResponse(
      int32_t token, aidlimports::ErrorCode errorCode,
      std::shared_ptr<qcril::interfaces::MultiSimVoiceCapabilityRespData> data);
  void sendExitSmsCallBackModeResponse(int32_t token, aidlimports::ErrorCode errorCode);
  void sendVosSupportStatusResponse(int32_t token, aidlimports::ErrorCode errorCode);
  void sendVosActionInfoResponse(int32_t token, aidlimports::ErrorCode errorCode);

  // Utility functions to create message.
  std::shared_ptr<QcRilRequestMessage> createSupsServiceMessage(int32_t in_token,
    const aidlimports::SuppServiceStatusRequest& in_suppServiceStatusRequest);
  std::shared_ptr<QcRilRequestMessage> createColpMessage(int32_t in_token);
  std::shared_ptr<QcRilRequestMessage> createCallBarringMessage(int32_t in_token,
    const aidlimports::SuppServiceStatusRequest& in_suppServiceStatusRequest);

  // Utility functions to trigger Stable AIDL indication API
  void notifyOnMultiSimVoiceCapabilityChanged(
    const aidlimports::MultiSimVoiceCapability capability);
  void notifyOnServiceStatusChanged(
    const std::vector<aidlimports::ServiceStatusInfo> srvStatusList);
  void notifyOnRegistrationChanged(
    const aidlimports::RegistrationInfo);
  void notifyOnImsSubConfigChanged(
    const aidlimports::ImsSubConfigInfo config);
};
}  // namespace implementation
}  // namespace ims
}  // namespace radio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
