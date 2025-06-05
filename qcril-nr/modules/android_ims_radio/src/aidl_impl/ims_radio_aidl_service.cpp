/******************************************************************************
  @file    ims_radio_aidl_service.cpp
  @brief   ims_radio_aidl_service

  DESCRIPTION
    Implements the server side of the IImsRadio interface. Handles RIL
    requests and responses to be received and sent to client respectively

  ---------------------------------------------------------------------------

  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/
#define TAG "RILQ"

#include <regex>
#include <framework/Log.h>
#include "modules/sms/qcril_qmi_sms.h"
#include <interfaces/ims/QcRilRequestImsGetRegStateMessage.h>
#include <interfaces/ims/QcRilRequestImsQueryServiceStatusMessage.h>
#include <interfaces/ims/QcRilRequestImsQueryVirtualLineInfo.h>
#include <interfaces/ims/QcRilRequestImsRegistrationChangeMessage.h>
#include <interfaces/ims/QcRilRequestImsRegisterMultiIdentityMessage.h>
#include <interfaces/ims/QcRilRequestImsQueryMultiSimVoiceCapability.h>
#include <interfaces/ims/QcRilRequestImsGetRtpStatsMessage.h>
#include <interfaces/ims/QcRilRequestImsGetRtpErrorStatsMessage.h>
#include <interfaces/ims/QcRilRequestImsGetSubConfigMessage.h>
#include <interfaces/ims/QcRilRequestImsGeoLocationInfoMessage.h>
#include <interfaces/ims/QcRilRequestImsSetMediaInfoMessage.h>
#include <interfaces/ims/QcRilRequestImsSetServiceStatusMessage.h>
#include <interfaces/ims/QcRilRequestImsSendRttMessage.h>
#include <interfaces/ims/QcRilRequestImsSetConfigMessage.h>
#include <interfaces/ims/QcRilRequestImsGetConfigMessage.h>
#include <interfaces/ims/QcRilRequestImsExitSmsCallbackModeMessage.h>

#include <interfaces/ims/ims.h>
#include <interfaces/voice/QcRilRequestImsAddParticipantMessage.h>
#include <interfaces/voice/QcRilRequestAnswerMessage.h>
#include <interfaces/voice/QcRilRequestImsCancelModifyCallMessage.h>
#include <interfaces/voice/QcRilRequestConferenceMessage.h>
#include <interfaces/voice/QcRilUnsolImsConferenceCallStateCompletedMessage.h>
#include <interfaces/voice/QcRilRequestImsDeflectCallMessage.h>
#include <interfaces/voice/QcRilRequestDialMessage.h>
#include <interfaces/voice/QcRilRequestExplicitCallTransferMessage.h>
#include <interfaces/voice/QcRilRequestGetCallWaitingMessage.h>
#include <interfaces/voice/QcRilRequestGetClipMessage.h>
#include <interfaces/voice/QcRilRequestGetClirMessage.h>
#include <interfaces/voice/QcRilRequestGetColrMessage.h>
#include <interfaces/voice/QcRilRequestHangupMessage.h>
#include <interfaces/voice/QcRilRequestImsHoldMessage.h>
#include <interfaces/voice/QcRilRequestImsModifyCallConfirmMessage.h>
#include <interfaces/voice/QcRilRequestImsModifyCallInitiateMessage.h>
#include <interfaces/voice/QcRilRequestQueryCallForwardMessage.h>
#include <interfaces/voice/QcRilRequestImsResumeMessage.h>
#include <interfaces/voice/QcRilRequestSendDtmfMessage.h>
#include <interfaces/voice/QcRilRequestSetCallForwardMessage.h>
#include <interfaces/voice/QcRilRequestSetCallWaitingMessage.h>
#include <interfaces/voice/QcRilRequestSetSuppSvcNotificationMessage.h>
#include <interfaces/voice/QcRilRequestSetSupsServiceMessage.h>
#include <interfaces/voice/QcRilRequestQueryColpMessage.h>
#include <interfaces/voice/QcRilRequestGetCallBarringMessage.h>
#include <interfaces/voice/QcRilRequestSetClirMessage.h>
#include <interfaces/voice/QcRilRequestSetColrMessage.h>
#include <interfaces/voice/QcRilRequestImsSetUiTtyModeMessage.h>
#include <interfaces/voice/QcRilRequestStartDtmfMessage.h>
#include <interfaces/voice/QcRilRequestStopDtmfMessage.h>
#include <interfaces/voice/QcRilRequestCancelUssdMessage.h>
#include <interfaces/voice/QcRilRequestSendUssdMessage.h>
#include <interfaces/voice/QcRilRequestSendSipDtmfMessage.h>
#include <interfaces/voice/QcRilRequestSendVosActionInfoMessage.h>
#include <interfaces/voice/QcRilRequestSendVosSupportStatusMessage.h>
#include <interfaces/nas/RilRequestExitEmergencyCallbackMessage.h>
#include <string.h>
#include <interfaces/sms/RilRequestImsSendSmsMessage.h>
#include <interfaces/sms/QcRilSyncQuerySmsFormatMessage.h>
#include <framework/QcrilImsClientConnected.h>
#include <cutils/properties.h>

#ifndef QMI_RIL_UTF
#include "request/UpdateUIOptionMessage.h"
#endif

#include "ims_radio_aidl_service.h"
#include "ims_radio_aidl_utils.h"
#include <binder/IInterface.h>

#include <aidl/vendor/qti/hardware/radio/ims/AutoCallRejectionInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/BlockReasonType.h>
#include <aidl/vendor/qti/hardware/radio/ims/CallComposerAutoRejectionInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/CallComposerInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/CallDomain.h>
#include <aidl/vendor/qti/hardware/radio/ims/CallFailCause.h>
#include <aidl/vendor/qti/hardware/radio/ims/CallInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/CallType.h>
#include <aidl/vendor/qti/hardware/radio/ims/CallModifyFailCause.h>
#include <aidl/vendor/qti/hardware/radio/ims/CallModifyInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/ClirInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/ColrInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/ConferenceCallState.h>
#include <aidl/vendor/qti/hardware/radio/ims/ConferenceInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/ConfParticipantOperation.h>
#include <aidl/vendor/qti/hardware/radio/ims/EmergencyCallBackMode.h>
#include <aidl/vendor/qti/hardware/radio/ims/ErrorCode.h>
#include <aidl/vendor/qti/hardware/radio/ims/ExtraType.h>
#include <aidl/vendor/qti/hardware/radio/ims/HandoverInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/HandoverType.h>
#include <aidl/vendor/qti/hardware/radio/ims/ImsSubConfigInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/IpPresentation.h>
#include <aidl/vendor/qti/hardware/radio/ims/MessageWaitingIndication.h>
#include <aidl/vendor/qti/hardware/radio/ims/MwiMessagePriority.h>
#include <aidl/vendor/qti/hardware/radio/ims/MwiMessageType.h>
#include <aidl/vendor/qti/hardware/radio/ims/ParticipantStatusInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/RadioState.h>
#include <aidl/vendor/qti/hardware/radio/ims/RadioTechType.h>
#include <aidl/vendor/qti/hardware/radio/ims/RegistrationBlockStatusInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/RegistrationInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/RegState.h>
#include <aidl/vendor/qti/hardware/radio/ims/RttMode.h>
#include <aidl/vendor/qti/hardware/radio/ims/SipErrorInfo.h>
#include <aidl/vendor/qti/hardware/radio/ims/SmsDeliverStatus.h>
#include <aidl/vendor/qti/hardware/radio/ims/SmsCallBackMode.h>
#include <aidl/vendor/qti/hardware/radio/ims/SmsSendStatus.h>
#include <aidl/vendor/qti/hardware/radio/ims/SmsSendFailureReason.h>
#include <aidl/vendor/qti/hardware/radio/ims/StatusType.h>
#include <aidl/vendor/qti/hardware/radio/ims/SystemServiceDomain.h>
#include <aidl/vendor/qti/hardware/radio/ims/ToneOperation.h>
#include <aidl/vendor/qti/hardware/radio/ims/VoiceInfo.h>

#define MAX_DIGITS_TO_SHOW 4
#define _S(x) #x
#define STRINGIZE(x) _S(x)
#define MAX_DIGITS_TO_SHOW_S STRINGIZE(MAX_DIGITS_TO_SHOW)

namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace ims {
namespace implementation {

std::string ImsRadioAidlImpl::hidePhoneNumber(std::string logStr) {
  std::smatch results;
  std::regex regNum("(.*number: )(\\+)?(\\d+)(\\d{" MAX_DIGITS_TO_SHOW_S "})(,.*)");
  std::regex regAddr("(.*address: )(\\+)?(\\d+)(\\d{" MAX_DIGITS_TO_SHOW_S "})(,.*)");
  std::regex regConfAddr("(.*address: )(.*)(, clirMode: .*)");
  std::regex regSipTelUris("(.*sip:)(\\+)?(\\d+)(\\d{" MAX_DIGITS_TO_SHOW_S "})(.*tel:)(\\+)?(\\d+)(\\d{" MAX_DIGITS_TO_SHOW_S "})(.*)");
  std::regex regSipSipUris("(.*sip:)(\\+)?(\\d+)(\\d{" MAX_DIGITS_TO_SHOW_S "})(.*sip:)(\\+)?(\\d+)(\\d{" MAX_DIGITS_TO_SHOW_S "})(.*)");
  std::regex regTelSipUris("(.*tel:)(\\+)?(\\d+)(\\d{" MAX_DIGITS_TO_SHOW_S "})(.*sip:)(\\+)?(\\d+)(\\d{" MAX_DIGITS_TO_SHOW_S "})(.*)");
  if(std::regex_match(logStr, results, regNum)) {
    return std::regex_replace(logStr, regNum, "$1*******$4$5");
  } else if(std::regex_match(logStr, results, regAddr)) {
    return std::regex_replace(logStr, regAddr, "$1*******$4$5");
  } else if(std::regex_match(logStr, results, regConfAddr)) {
    return std::regex_replace(logStr, regConfAddr, "$1*******$3");
  } else if(std::regex_match(logStr, results, regSipTelUris)) {
    return std::regex_replace(logStr, regSipTelUris, "$1*******$4$5*******$8$9");
  } else if(std::regex_match(logStr, results, regSipSipUris)) {
    return std::regex_replace(logStr, regSipSipUris, "$1*******$4$5*******$8$9");
  } else if(std::regex_match(logStr, results, regTelSipUris)) {
      return std::regex_replace(logStr, regTelSipUris, "$1*******$4$5*******$8$9");
  }
  return logStr;
}

static inline std::string boolToString(bool value) {
  return value ? "true" : "false";
}

void ImsRadioAidlImpl::imsRadiolog(std::string dir, std::string logStr) {
  for (unsigned li = 0; li < logStr.length(); li += 150) {
    std::string subStr = logStr.substr(li, 150);
    QCRIL_LOG_INFO("%s [%zu/%zu] %s", dir.c_str(), logStr.length(), li + subStr.length(),
                   subStr.c_str());
  }
}

void ImsRadioAidlImpl::notifyImsClientConnected() {
  auto msg = std::make_shared<QcrilImsClientConnected>();
  if (msg != nullptr) {
    msg->broadcast();
  }
}

void ImsRadioAidlImpl::setCallback_nolock(
    const std::shared_ptr<aidlimports::IImsRadioResponse>& imsRadioResponse,
    const std::shared_ptr<aidlimports::IImsRadioIndication>& imsRadioIndication) {
  QCRIL_LOG_DEBUG("ImsRadioAidlImpl::setCallback_nolock");

  mImsRadioResponseCb = imsRadioResponse;
  mImsRadioIndicationCb = imsRadioIndication;
}

void ImsRadioAidlImpl::deathNotifier() {
  QCRIL_LOG_DEBUG("ImsRadioAidlImpl::serviceDied: Client died. Cleaning up callbacks");
  clearCallbacks();
}

static void deathRecpCallback(void* cookie) {
  ImsRadioAidlImpl* impl = static_cast<ImsRadioAidlImpl*>(cookie);
  if (impl != nullptr) {
    impl->deathNotifier();
  }
}

/*
 *   @brief
 *   Registers the callback for IImsRadio using the IImsRadioCallback object
 *   being passed in by the client as a parameter
 *
 */
::ndk::ScopedAStatus ImsRadioAidlImpl::setCallback(
    const std::shared_ptr<aidlimports::IImsRadioResponse>& imsRadioResponse,
    const std::shared_ptr<aidlimports::IImsRadioIndication>& imsRadioIndication) {
  QCRIL_LOG_INFO("ImsRadioAidlImpl::setCallback: Set client response and indication callbacks");
  {
    std::unique_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
    #ifndef QMI_RIL_UTF
    if (mImsRadioResponseCb != nullptr) {
      // android:: IInterface::asBinder(mImsRadioResponseCb).unlinkToDeath(this);
      AIBinder_unlinkToDeath(mImsRadioResponseCb->asBinder().get(), mDeathRecipient,
                             reinterpret_cast<void*>(this));
    }
    #endif
    setCallback_nolock(imsRadioResponse, imsRadioIndication);
    #ifndef QMI_RIL_UTF
    if (mImsRadioResponseCb != nullptr) {
      AIBinder_DeathRecipient_delete(mDeathRecipient);
      mDeathRecipient = AIBinder_DeathRecipient_new(&deathRecpCallback);
      if (mDeathRecipient != nullptr) {
        AIBinder_linkToDeath(mImsRadioResponseCb->asBinder().get(), mDeathRecipient,
                             reinterpret_cast<void*>(this));
      }
    }
    #endif
  }
  if(imsRadioResponse != nullptr && imsRadioIndication != nullptr) {
    notifyImsClientConnected();
  }
  return ndk::ScopedAStatus::ok();
}

std::shared_ptr<aidlimports::IImsRadioResponse> ImsRadioAidlImpl::getResponseCallback() {
  std::shared_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
  return mImsRadioResponseCb;
}

std::shared_ptr<aidlimports::IImsRadioIndication> ImsRadioAidlImpl::getIndicationCallback() {
  std::shared_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
  return mImsRadioIndicationCb;
}

ImsRadioAidlImpl::ImsRadioAidlImpl(qcril_instance_id_e_type instance) : mInstanceId(instance) {
}

ImsRadioAidlImpl::~ImsRadioAidlImpl() {
}

void ImsRadioAidlImpl::clearCallbacks() {
  QCRIL_LOG_FUNC_ENTRY("enter");
  {
    std::unique_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
    mImsRadioResponseCb = nullptr;
    mImsRadioIndicationCb = nullptr;
    AIBinder_DeathRecipient_delete(mDeathRecipient);
    mDeathRecipient = nullptr;
  }
  QCRIL_LOG_FUNC_ENTRY("exit");
}

void ImsRadioAidlImpl::notifyOnRegistrationChanged(std::shared_ptr<QcRilUnsolImsRegStateMessage> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is null");
    return;
  }

  std::shared_ptr<qcril::interfaces::Registration> reg = msg->getRegistration();
  if (!reg) {
    QCRIL_LOG_ERROR("data is null");
    return;
  }

  aidlimports::RegistrationInfo regInfo{ .state = aidlimports::RegState::INVALID,
                                         .errorCode = INT32_MAX,
                                         .radioTech = aidlimports::RadioTechType::INVALID };
  ims::utils::convertRilRegistrationToAidlResponse(regInfo, *reg);
  notifyOnRegistrationChanged(regInfo);
}

void ImsRadioAidlImpl::notifyOnCallStateChanged(std::shared_ptr<QcRilUnsolCallStateChangeMessage> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is nullptr");
    return;
  }

  auto indCb = getIndicationCallback();
  if (!indCb) {
    QCRIL_LOG_ERROR("indCb is null");
    return;
  }

  std::vector<qcril::interfaces::CallInfo> callInfo = msg->getCallInfo();
  if (callInfo.empty()) {
    QCRIL_LOG_ERROR("empty callInfo");
    return;
  }

  std::vector<aidlimports::CallInfo> callList{};
  bool result = ims::utils::convertToAidlCallInfoList(callList, callInfo);
  if (!result) {
    QCRIL_LOG_ERROR("CallInfo convertion failed");
    return;
  }

  std::string logStr;
  for (unsigned i = 0; i < callList.size(); i++) {
      logStr += hidePhoneNumber(::android::internal::ToString(callList[i])) + "; ";
  }
  imsRadiolog("<", "onCallStateChanged: callList = " + logStr);
  auto ret = indCb->onCallStateChanged(callList);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void ImsRadioAidlImpl::notifyOnModifyCall(std::shared_ptr<QcRilUnsolImsModifyCallMessage> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is null");
    return;
  }

  auto indCb = getIndicationCallback();
  if (!indCb) {
    QCRIL_LOG_ERROR("indCb is null");
    return;
  }

  aidlimports::CallModifyInfo modifyInfo{ .callIndex = INT32_MAX,
                                          .callDetails.callType = aidlimports::CallType::UNKNOWN,
                                          .callDetails.callDomain = aidlimports::CallDomain::INVALID,
                                          .callDetails.rttMode = aidlimports::RttMode::INVALID,
                                          .failCause = aidlimports::CallModifyFailCause::E_INVALID };
  if (msg->hasCallIndex()) {
    modifyInfo.callIndex = msg->getCallIndex();
  }

  if (msg->hasCallType()) {
    modifyInfo.callDetails.callType = ims::utils::convertToAidlCallType(msg->getCallType());
  }

  if (msg->hasCallDomain()) {
    modifyInfo.callDetails.callDomain = ims::utils::convertToAidlCallDomain(msg->getCallDomain());
  }

  if (msg->hasRttMode()) {
    modifyInfo.callDetails.rttMode = ims::utils::convertToAidlRttMode(msg->getRttMode());
  }

  if (msg->hasCallModifyFailCause()) {
    modifyInfo.failCause =
        ims::utils::convertToAidlCallModifyFailCause(msg->getCallModifyFailCause());
  }

  if (msg->hasVosSupport()) {
    modifyInfo.callDetails.isVosSupported = msg->getVosSupport();
  }

  imsRadiolog("<", "onModifyCall: modifyInfo = " + ::android::internal::ToString(modifyInfo));
  auto ret = indCb->onModifyCall(modifyInfo);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void ImsRadioAidlImpl::notifyOnRing(std::shared_ptr<QcRilUnsolCallRingingMessage> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is null");
    return;
  }

  auto indCb = getIndicationCallback();
  if (!indCb) {
    QCRIL_LOG_ERROR("indCb is null");
    return;
  }

  imsRadiolog("<", "onRing");
  auto ret = indCb->onRing();
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void ImsRadioAidlImpl::notifyOnRingbackTone(std::shared_ptr<QcRilUnsolRingbackToneMessage> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is null");
    return;
  }

  auto indCb = getIndicationCallback();
  if (!indCb) {
    QCRIL_LOG_ERROR("indCb is null");
    return;
  }

  aidlimports::ToneOperation tone =
      ims::utils::convertToAidlToneOperation(msg->getRingBackToneOperation());
  imsRadiolog("<", "onRingbackTone: tone = " + toString(tone));
  auto ret = indCb->onRingbackTone(tone);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

#define WLAN_HANDOVER_NO_LTE_FAILURE_CODE_STRING "CD-04"

void ImsRadioAidlImpl::notifyOnHandover(std::shared_ptr<QcRilUnsolImsHandoverMessage> msg) {
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is null");
    return;
  }

  auto indCb = getIndicationCallback();
  if (!indCb) {
    QCRIL_LOG_ERROR("indcb is null");
    return;
  }

  aidlimports::HandoverInfo handoverInfo = { .type = aidlimports::HandoverType::INVALID,
                                             .srcTech = aidlimports::RadioTechType::INVALID,
                                             .targetTech = aidlimports::RadioTechType::INVALID };
  if (msg->hasHandoverType()) {
    handoverInfo.type = ims::utils::convertToAidlHandoverType(msg->getHandoverType());
  }
  if (msg->hasSourceTech()) {
    handoverInfo.srcTech = ims::utils::convertToAidlRadioTech(msg->getSourceTech());
  }
  if (msg->hasTargetTech()) {
    handoverInfo.targetTech = ims::utils::convertToAidlRadioTech(msg->getTargetTech());
  }
  if (msg->hasErrorCode()) {
    handoverInfo.errorCode = msg->getErrorCode();
  }
  if (msg->hasErrorMsg()) {
    handoverInfo.errorMessage = msg->getErrorMsg();
  }
  if (!msg->hasErrorCode() && !msg->hasErrorMsg() && msg->hasCauseCode() &&
      !msg->getCauseCode().empty()) {
    handoverInfo.hoExtra.type = aidlimports::ExtraType::LTE_TO_IWLAN_HO_FAIL;
    for (size_t idx = 0; idx < msg->getCauseCode().length(); idx++) {
      handoverInfo.hoExtra.extraInfo.push_back(msg->getCauseCode()[idx]);
    }
  } else {
    handoverInfo.hoExtra.type = aidlimports::ExtraType::INVALID;
  }

  imsRadiolog("<", "onHandover: handoverInfo = " + ::android::internal::ToString(handoverInfo));
  auto ret = indCb->onHandover(handoverInfo);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void ImsRadioAidlImpl::notifyOnTtyNotification(
    std::shared_ptr<QcRilUnsolImsTtyNotificationMessage> msg) {
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is null");
    return;
  }

  auto indCb = getIndicationCallback();
  if (!indCb) {
    QCRIL_LOG_ERROR("indcb is null");
    return;
  }

  aidlimports::TtyInfo ttyInfo{.mode = aidlimports::TtyMode::INVALID };
  if (msg->hasTtyMode()) {
    ttyInfo.mode = ims::utils::convertToAidlTtyMode(msg->getTtyMode());
  }
  imsRadiolog("<", "onTtyNotification: ttyInfo = " + android::internal::ToString(ttyInfo));
  auto ret = indCb->onTtyNotification(ttyInfo);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void ImsRadioAidlImpl::notifyOnRefreshConferenceInfo(
    std::shared_ptr<QcRilUnsolImsConferenceInfoMessage> msg) {
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is null");
    return;
  }

  auto indCb = getIndicationCallback();
  if (!indCb) {
    QCRIL_LOG_ERROR("indcb is null");
    return;
  }

  aidlimports::ConferenceInfo conferenceInfo{ .conferenceCallState =
                                                  aidlimports::ConferenceCallState::INVALID };
  if (msg->hasConferenceCallState()) {
    conferenceInfo.conferenceCallState =
        ims::utils::convertToAidlConferenceCallState(msg->getConferenceCallState());
  }
  if (msg->hasConferenceInfoUri() && msg->getConferenceInfoUri().size()) {
    conferenceInfo.confInfoUri = msg->getConferenceInfoUri();
  }

  imsRadiolog("<", "onRefreshConferenceInfo: conferenceInfo = " +
                       ::android::internal::ToString(conferenceInfo));
  auto ret = indCb->onRefreshConferenceInfo(conferenceInfo);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void ImsRadioAidlImpl::notifyOnRefreshViceInfo(std::shared_ptr<QcRilUnsolImsViceInfoMessage> msg) {
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is null");
    return;
  }

  auto indCb = getIndicationCallback();
  if (!indCb) {
    QCRIL_LOG_ERROR("indcb is null");
    return;
  }

  aidlimports::ViceInfo viceInfo{};
  if (msg->hasViceInfoUri() && msg->getViceInfoUri().size()) {
    viceInfo.viceInfoUri = msg->getViceInfoUri();
  }
  imsRadiolog("<", "onRefreshViceInfo: viceInfo = " + hidePhoneNumber(android::internal::ToString(viceInfo)));
  auto ret = indCb->onRefreshViceInfo(viceInfo);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void ImsRadioAidlImpl::notifyOnSuppServiceNotification(
    std::shared_ptr<QcRilUnsolSuppSvcNotificationMessage> msg) {
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is null");
    return;
  }

  auto indCb = getIndicationCallback();
  if (!indCb) {
    QCRIL_LOG_ERROR("indcb is null");
    return;
  }
  aidlimports::SuppServiceNotification data{.notificationType =
                                                aidlimports::NotificationType::INVALID,
                                            .code = INT32_MAX,
                                            .index = INT32_MAX,
                                            .type = INT32_MAX,
                                            .connId = INT32_MAX,
                                            .hasHoldTone = false };

  if (msg->hasNotificationType()) {
    data.notificationType = ims::utils::convertToAidlNotificationType(msg->getNotificationType());
  }
  if (msg->hasCode()) {
    data.code = msg->getCode();
  }
  if (msg->hasIndex()) {
    data.index = msg->getIndex();
  }
  if (msg->hasType()) {
    data.type = msg->getType();
  }
  if (msg->hasNumber() && !msg->getNumber().empty()) {
    data.number = msg->getNumber();
  }
  if (msg->hasConnectionId()) {
    data.connId = msg->getConnectionId();
  }
  if (msg->hasHistoryInfo() && !msg->getHistoryInfo().empty()) {
    data.historyInfo = msg->getHistoryInfo();
  }
  if (msg->hasHoldTone()) {
    data.hasHoldTone = true;
    data.holdTone = msg->getHoldTone();
  }
  imsRadiolog("<", "onSuppServiceNotification: suppServiceNotification = " +
                        ::android::internal::ToString(data));
  auto ret = indCb->onSuppServiceNotification(data);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void ImsRadioAidlImpl::notifyOnParticipantStatusInfo(
    std::shared_ptr<QcRilUnsolConfParticipantStatusInfoMessage> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is null");
    return;
  }

  auto indCb = getIndicationCallback();
  if (!indCb) {
    QCRIL_LOG_ERROR("indCb is null");
    return;
  }

  aidlimports::ParticipantStatusInfo participantStatusInfo{
    .callId = INT32_MAX,
    .operation = aidlimports::ConfParticipantOperation::INVALID,
    .sipStatus = INT32_MAX
  };
  if (msg->hasCallId()) {
    participantStatusInfo.callId = msg->getCallId();
  }
  if (msg->hasOperation()) {
    participantStatusInfo.operation =
        ims::utils::convertToAidlConfParticipantOperation(msg->getOperation());
  }
  if (msg->hasSipStatus()) {
    participantStatusInfo.sipStatus = msg->getSipStatus();
  }
  if (msg->hasParticipantUri() && !msg->getParticipantUri().empty()) {
    participantStatusInfo.participantUri = msg->getParticipantUri();
  }
  if (msg->hasIsEct()) {
    participantStatusInfo.isEct = msg->getIsEct();
  }

  imsRadiolog("<", "onParticipantStatusInfo: participantStatusInfo = " +
                       ::android::internal::ToString(participantStatusInfo));
  auto ret = indCb->onParticipantStatusInfo(participantStatusInfo);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void ImsRadioAidlImpl::notifyOnIncomingCallAutoRejected(
    std::shared_ptr<QcRilUnsolAutoCallRejectionMessage> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is nullptr");
    return;
  }

  auto indCb = getIndicationCallback();
  if (!indCb) {
    QCRIL_LOG_ERROR("indCb is null");
    return;
  }

  // get auto call reject info
  aidlimports::AutoCallRejectionInfo rejInfo = {
    .callType = aidlimports::CallType::UNKNOWN,
    .autoRejectionCause = aidlimports::CallFailCause::INVALID,
    .verificationStatus = aidlimports::VerificationStatus::VALIDATION_NONE
  };

  if (msg->hasCallType()) {
    rejInfo.callType = ims::utils::convertToAidlCallType(msg->getCallType());
  }
  if (msg->hasCallFailCause()) {
    rejInfo.autoRejectionCause = ims::utils::convertToAidlCallFailCause(msg->getCallFailCause());
  }
  if (msg->hasSipErrorCode()) {
    rejInfo.sipErrorCode = msg->getSipErrorCode();
  }
  if (msg->hasNumber()) {
    rejInfo.number = msg->getNumber();
  }
  if (msg->hasVerificationStatus()) {
    rejInfo.verificationStatus =
        ims::utils::convertToAidlVerificationStatus(msg->getVerificationStatus());
  }

  // get call composer ,eCNAM info, data channel info if its available
  aidlimports::AutoCallRejectionInfo2 callRejInfo{ .autoCallRejectionInfo = rejInfo };
  if (msg->hasComposerInfo()) {
    aidlimports::CallComposerInfo ccInfo;
    ims::utils::convertToAidlCallComposerInfo(ccInfo, msg->getComposerInfo());
    callRejInfo.callComposerInfo = std::move(ccInfo);
  }

  if (msg->hasEcnamInfo()) {
    aidlimports::EcnamInfo ecnamInfo;
    ims::utils::convertToAidlEcnamInfo(ecnamInfo, msg->getEcnamInfo());
    callRejInfo.ecnamInfo = std::move(ecnamInfo);
  }

  if (msg->hasIsDcCall()) {
    callRejInfo.isDcCall = msg->getIsDcCall();
  }

  if (msg->hasCallReason()) {
    callRejInfo.callReason = msg->getCallReason();
  }

  imsRadiolog("<", "onIncomingCallAutoRejected2: auto call rejection info = " +
                       hidePhoneNumber(::android::internal::ToString(callRejInfo)));
  auto ret = indCb->onIncomingCallAutoRejected2(callRejInfo);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    return;
  }
}

void ImsRadioAidlImpl::notifyOnSupplementaryServiceIndication(
    std::shared_ptr<QcRilUnsolSupplementaryServiceMessage> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is nullptr");
    return;
  }

  auto indCb = getIndicationCallback();
  if (!indCb) {
    QCRIL_LOG_ERROR("indCb is null");
    return;
  }

  aidlimports::StkCcUnsolSsResult ss{.serviceType = aidlimports::SsServiceType::INVALID,
                                      .requestType = aidlimports::SsRequestType::INVALID,
                                      .teleserviceType = aidlimports::SsTeleserviceType::INVALID,
                                      .serviceClass = INT32_MAX,
                                      .result = INT32_MAX };
  if (msg->hasServiceType()) {
    ims::utils::convertToAidlServiceType(ss.serviceType, msg->getServiceType());
  }
  if (msg->hasRequestType()) {
    ims::utils::convertToAidlRequestType(ss.requestType, msg->getRequestType());
  }
  if (msg->hasTeleserviceType()) {
    ims::utils::convertToAidlTeleserviceType(ss.teleserviceType, msg->getTeleserviceType());
  }
  if (msg->hasServiceClass()) {
    ss.serviceClass = msg->getServiceClass();
  }
  if (msg->hasResult()) {
    ss.result = msg->getResult();
  }

  if (ims::utils::isServiceTypeCfQuery(ss.serviceType, ss.requestType) &&
      msg->hasCallForwardInfoList() && !msg->getCallForwardInfoList().empty()) {
    std::vector<qcril::interfaces::CallForwardInfo> rilCfInfo = msg->getCallForwardInfoList();
    ss.cfData.resize(1);
    ss.ssInfoData.resize(0);

    /* number of call info's */
    ss.cfData[0].cfInfo.resize(rilCfInfo.size());

    for (int i = 0; i < rilCfInfo.size() && i < NUM_SERVICE_CLASSES; i++) {
      qcril::interfaces::CallForwardInfo cf = rilCfInfo[i];
      aidlimports::CallForwardInfo* cfInfo = &ss.cfData[0].cfInfo[i];

      if (cf.hasStatus()) {
        cfInfo->status = cf.getStatus();
      }
      if (cf.hasReason()) {
        cfInfo->reason = cf.getReason();
      }
      if (cf.hasServiceClass()) {
        cfInfo->serviceClass = cf.getServiceClass();
      }
      if (cf.hasToa()) {
        cfInfo->toa = cf.getToa();
      }
      if (!cf.getNumber().empty()) {
        cfInfo->number = cf.getNumber();
      }
      if (cf.hasTimeSeconds()) {
        cfInfo->timeSeconds = cf.getTimeSeconds();
      }
    }
  } else {
    ss.ssInfoData.resize(1);
    ss.cfData.resize(0);

    if (msg->hasSuppSrvInfoList() && !msg->getSuppSrvInfoList().empty()) {
      /* each int */
      ss.ssInfoData[0].ssInfo.resize(SS_INFO_MAX);
      for (int i = 0; i < msg->getSuppSrvInfoList().size() && i < SS_INFO_MAX; i++) {
        ss.ssInfoData[0].ssInfo[i] = msg->getSuppSrvInfoList()[i];
      }
    }
  }

  if (msg->hasBarredNumberList()) {
    std::vector<std::string> cbNumList = msg->getBarredNumberList();
    if (ims::utils::isServiceTypeIcbQueryWithNumber(ss.serviceType, ss.requestType) &&
        cbNumList.size()) {
      ss.cbNumInfo.resize(cbNumList.size());
      for (int i = 0; i < cbNumList.size(); i++) {
        ss.cbNumInfo[i].status = aidlimports::ServiceClassStatus::ENABLED;
        if (!cbNumList[i].empty()) {
          ss.cbNumInfo[i].number = cbNumList[i];
        }
      }
    }
  }
  imsRadiolog("<", "onSupplementaryServiceIndication: ss = " + android::internal::ToString(ss));
  auto ret = indCb->onSupplementaryServiceIndication(ss);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void ImsRadioAidlImpl::notifyOnRTTMessage(std::shared_ptr<QcRilUnsolImsRttMessage> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is null");
    return;
  }

  auto indCb = this->getIndicationCallback();
  if (!indCb) {
    QCRIL_LOG_ERROR("indCb is null");
    return;
  }

  std::string rttMsg = msg->getRttMessage();
  if (rttMsg.empty()) {
    QCRIL_LOG_ERROR("Invalid rtt message");
    return;
  }

  imsRadiolog("<", "onRttMessageReceived: message=" + ::android::internal::ToString(rttMsg));
  auto ret = indCb->onRttMessageReceived(rttMsg);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void ImsRadioAidlImpl::notifyOnVowifiCallQuality(std::shared_ptr<QcRilUnsolImsVowifiCallQuality> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is nullptr");
    return;
  }

  auto indCb = getIndicationCallback();
  if (!indCb) {
    QCRIL_LOG_ERROR("indCb is null");
    return;
  }

  aidlimports::VoWiFiCallQuality voWiFiCallQuality{};
  ims::utils::convertToAidlVowifiCallQuality(voWiFiCallQuality, msg->getCallQuality());
  imsRadiolog("<",
              "onVoWiFiCallQuality: quality=" + android::internal::ToString(voWiFiCallQuality));
  auto ret = indCb->onVoWiFiCallQuality(voWiFiCallQuality);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void ImsRadioAidlImpl::notifyOnVopsChanged(std::shared_ptr<QcRilUnsolImsVopsIndication> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is null");
    return;
  }

  auto indCb = this->getIndicationCallback();
  if (!indCb) {
    QCRIL_LOG_ERROR("indCb is null");
    return;
  }

  imsRadiolog("<", "onVopsChanged: status=" + std::to_string(msg->getVopsStatus()));
  auto ret = indCb->onVopsChanged(msg->getVopsStatus());
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void ImsRadioAidlImpl::notifyOnSubConfigChanged(
    std::shared_ptr<QcRilUnsolImsSubConfigIndication> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is null");
    return;
  }

  aidl::vendor::qti::hardware::radio::ims::ImsSubConfigInfo config{};
  std::shared_ptr<qcril::interfaces::ImsSubConfigInfo> data = msg->getImsSubConfig();
  if (data) {
    ims::utils::convertRilImsSubConfigInfoToAidlResponse(config, *data);
  }
  notifyOnImsSubConfigChanged(config);
}

void ImsRadioAidlImpl::notifyOnGeoLocationChange(std::shared_ptr<QcRilUnsolImsGeoLocationInfo> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is null");
    return;
  }

  auto indCb = this->getIndicationCallback();
  if (!indCb) {
    QCRIL_LOG_ERROR("indCb is null");
    return;
  }

  imsRadiolog("<", "onGeolocationInfoRequested: lat = " + PII(std::to_string(msg->getLatitude()),"<hidden>") +
                             "lon = " + PII(std::to_string(msg->getLongitude()),"<hidden>"));
  auto ret = indCb->onGeolocationInfoRequested(msg->getLatitude(), msg->getLongitude());
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void ImsRadioAidlImpl::notifyOnServiceStatusChange(
    std::shared_ptr<QcRilUnsolImsSrvStatusIndication> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is nullptr");
    return;
  }

  std::shared_ptr<qcril::interfaces::ServiceStatusInfoList> data = msg->getServiceStatus();
  if (!data) {
    QCRIL_LOG_ERROR("data is nullptr");
    return;
  }

  std::vector<aidlimports::ServiceStatusInfo> srvStatusList;
  ims::utils::convertRilServiceStatusInfoListToAidlResponse(srvStatusList, *data);
  notifyOnServiceStatusChanged(srvStatusList);
}

void ImsRadioAidlImpl::notifyOnRegBlockStatusChange(
    std::shared_ptr<QcRilUnsolImsRegBlockStatusMessage> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is nullptr");
    return;
  }

  auto indCb = getIndicationCallback();
  if (!indCb) {
    QCRIL_LOG_ERROR("indCb is null");
    return;
  }

  std::shared_ptr<qcril::interfaces::RegistrationBlockStatus> data = msg->getBlockStatus();
  if (data == nullptr) {
    QCRIL_LOG_ERROR("data is null.");
    return;
  }

  aidlimports::RegistrationBlockStatusInfo regBlockStatus{};

  if (data->hasBlockStatusOnWwan()) {
    ims::utils::convertToAidlRegBlockStatus(regBlockStatus.blockStatusOnWwan,
                                            *(data->getBlockStatusOnWwan()));
  } else {
    regBlockStatus.blockStatusOnWwan.blockReason = aidlimports::BlockReasonType::INVALID;
  }

  if (data->hasBlockStatusOnWlan()) {
    ims::utils::convertToAidlRegBlockStatus(regBlockStatus.blockStatusOnWlan,
                                            *(data->getBlockStatusOnWlan()));
  } else {
    regBlockStatus.blockStatusOnWlan.blockReason = aidlimports::BlockReasonType::INVALID;
  }

  imsRadiolog("<", "onRegistrationBlockStatus: StatusInfo : " +
                       ::android::internal::ToString(regBlockStatus));
  auto ret = indCb->onRegistrationBlockStatus(regBlockStatus);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void ImsRadioAidlImpl::notifyOnSsacInfoChange(std::shared_ptr<QcRilUnsolImsSsacInfoIndication> msg) {
  (void)msg;
}

void ImsRadioAidlImpl::notifyIncomingSms(std::shared_ptr<RilUnsolIncomingImsSMSMessage> msg) {
  if (msg) {
    auto indicationCb = getIndicationCallback();
    if (indicationCb) {
      aidlimports::IncomingSms sms;
      auto tech = msg->getTech();
      if (tech == RADIO_TECH_3GPP) {
        sms.format = "3gpp";
        sms.pdu = msg->getGsmPayload();
        sms.verstat = ims::utils::convertToAidlVerificationStatus(msg->getVerificationStatus());
      } else {
        (void)ims::utils::convertCdmaFormatToPseudoPdu(msg->getCdmaPayload(), sms.pdu);
        sms.format = "3gpp2";
        sms.verstat = aidlimports::VerificationStatus::VALIDATION_NONE;
      }
      imsRadiolog("<", "onIncomingSms: sms = " + ::android::internal::ToString(sms));
      auto ret = indicationCb->onIncomingSms(sms);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
      }
    } else {
      QCRIL_LOG_ERROR("Indication callback is null");
    }
  }
}

void ImsRadioAidlImpl::notifyNewSmsStatusReport(
    std::shared_ptr<RilUnsolNewImsSmsStatusReportMessage> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is nullptr");
    return;
  }

  auto indCb = getIndicationCallback();
  if (!indCb) {
    QCRIL_LOG_ERROR("indCb is null");
    return;
  }

  aidlimports::SmsSendStatusReport report;
  auto tech = msg->getRadioTech();
  if (tech == RADIO_TECH_3GPP) {
    auto& payload = msg->getGsmPayload();
    report = { (int)msg->getMessageRef(), "3gpp", payload };
  } else {
    std::vector<uint8_t> payload;
    (void)ims::utils::convertCdmaFormatToPseudoPdu(msg->getCdmaPayload(), payload);
    report = { (int)msg->getMessageRef(), "3gpp2", payload };
  }
  imsRadiolog("<", "onImsSmsStatusReport: report = " + android::internal::ToString(report));
  auto ret = indCb->onSmsSendStatusReport(report);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void ImsRadioAidlImpl::notifyOnPendingMultiIdentityStatus(
    std::shared_ptr<QcRilUnsolImsPendingMultiLineStatus> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is null");
    return;
  }

  auto indCb = getIndicationCallback();
  if (!indCb) {
    QCRIL_LOG_ERROR("indCb is null");
    return;
  }

  imsRadiolog("<", "onMultiIdentityInfoPending");
  auto ret = indCb->onMultiIdentityInfoPending();
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void ImsRadioAidlImpl::notifyOnMultiIdentityLineStatus(
    std::shared_ptr<QcRilUnsolImsMultiIdentityStatusMessage> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is null");
    return;
  }

  auto indCb = getIndicationCallback();
  if (!indCb) {
    QCRIL_LOG_ERROR("indCb is null");
    return;
  }

  std::vector<aidlimports::MultiIdentityLineInfo> hal_info;
  ims::utils::convertToAidlMultiIdentityInfo(msg->getLineInfo(), hal_info);
  imsRadiolog("<", "onMultiIdentityRegistrationStatusChange: info=" +
                       ::android::internal::ToString(hal_info));
  auto ret = indCb->onMultiIdentityRegistrationStatusChange(hal_info);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void ImsRadioAidlImpl::notifyOnVoiceInfoStatusChange(std::shared_ptr<QcRilUnsolImsVoiceInfo> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is null");
    return;
  }

  auto indCb = this->getIndicationCallback();
  if (!indCb) {
    QCRIL_LOG_ERROR("indCb is null");
    return;
  }

  aidlimports::VoiceInfo voiceInfo = ims::utils::convertToAidlVoiceInfo(msg->getVoiceInfo());
  imsRadiolog("<", "onVoiceInfoChanged: voiceInfo" + toString(voiceInfo));
  auto ret = indCb->onVoiceInfoChanged(voiceInfo);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s" , ret.getDescription().c_str());
  }
}

void ImsRadioAidlImpl::notifyOnExitEcbmIndication(
    std::shared_ptr<RilUnsolEmergencyCallbackModeMessage> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is null");
    return;
  }

  auto indCb = getIndicationCallback();
  if (!indCb) {
    QCRIL_LOG_ERROR("indCb is null");
    return;
  }

  imsRadiolog("<", "onEmergencyCallBackModeChanged: EXIT");
  auto ret = indCb->onEmergencyCallBackModeChanged(aidlimports::EmergencyCallBackMode::EXIT);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void ImsRadioAidlImpl::notifyOnEnterEcbmIndication(
    std::shared_ptr<RilUnsolEmergencyCallbackModeMessage> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is null");
    return;
  }

  auto indCb = getIndicationCallback();
  if (!indCb) {
    QCRIL_LOG_ERROR("indCb is null");
    return;
  }

  imsRadiolog("<", "onEmergencyCallBackModeChanged: ENTER");
  auto ret = indCb->onEmergencyCallBackModeChanged(aidlimports::EmergencyCallBackMode::ENTER);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void ImsRadioAidlImpl::notifyOnRadioStateChanged(
    std::shared_ptr<QcRilUnsolImsRadioStateIndication> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is null");
    return;
  }

  auto indCb = getIndicationCallback();
  if (!indCb) {
    QCRIL_LOG_ERROR("indCb is null");
    return;
  }

  aidlimports::RadioState state = ims::utils::convertToAidlRadioState(msg->getRadioState());
  imsRadiolog("<", "onRadioStateChanged: state=" + toString(state));
  auto ret = indCb->onRadioStateChanged(state);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void ImsRadioAidlImpl::notifyOnMessageWaiting(
    std::shared_ptr<QcRilUnsolMessageWaitingInfoMessage> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is null");
    return;
  }

  auto indCb = getIndicationCallback();
  if (!indCb) {
    QCRIL_LOG_ERROR("indCb is null");
    return;
  }

  aidlimports::MessageWaitingIndication mwi =
      ims::utils::convertToAidlMessageWaitingIndication(msg);

  imsRadiolog("<", "onMessageWaiting: mwi = " + ::android::internal::ToString(mwi));
  auto ret = indCb->onMessageWaiting(mwi);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void ImsRadioAidlImpl::notifyOnModemSupportsWfcRoamingModeConfiguration(
    std::shared_ptr<QcRilUnsolImsWfcRoamingConfigIndication> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is null");
    return;
  }

  auto indCb = this->getIndicationCallback();
  if (!indCb) {
    QCRIL_LOG_ERROR("indCb is null");
    return;
  }

  imsRadiolog("<", "onModemSupportsWfcRoamingModeConfiguration: status=" +
      std::to_string(msg->getWfcRoamingConfigSupport()));
  auto ret = indCb->onModemSupportsWfcRoamingModeConfiguration(msg->getWfcRoamingConfigSupport());
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void ImsRadioAidlImpl::notifyOnUssdMessageFailed(std::shared_ptr<QcRilUnsolOnUssdMessage> msg) {
  if (!msg || !msg->hasErrorDetails()) {
    QCRIL_LOG_ERROR("msg is nullptr or invalid");
    return;
  }

  auto indCb = getIndicationCallback();
  if (!indCb) {
    QCRIL_LOG_ERROR("indCb is null");
    return;
  }

  aidlimports::UssdModeType modeType = ims::utils::convertToAidlUssdModeType(msg->getMode());
  aidlimports::SipErrorInfo errorDetails;
  ims::utils::convertToAidlSipErrorInfo(errorDetails, msg->getErrorDetails(), false);
  imsRadiolog("<", "onUssdMessageFailed: type=" + android::internal::ToString(modeType) +
                        android::internal::ToString(errorDetails));
  auto ret = indCb->onUssdMessageFailed(modeType, errorDetails);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void ImsRadioAidlImpl::notifyOnRetrievingGeoLocationDataStatus(
    std::shared_ptr<QcRilUnsolImsGeoLocationDataStatus> msg) {
  if (!msg || !msg->hasGeoLocationDataStatus()) {
    QCRIL_LOG_ERROR("msg is nullptr or invalid");
    return;
  }
  auto indCb = getIndicationCallback();
  if (!indCb) {
    return;
  }
  aidlimports::GeoLocationDataStatus geoLocationDataStatus =
      ims::utils::convertToAidlGeoLocationDataStatus(msg->getGeoLocationDataStatus());
  imsRadiolog("<", "onRetrievingGeoLocationDataStatus: geoLocationDataStatus=" +
                       android::internal::ToString(geoLocationDataStatus));
  auto ret = indCb->onRetrievingGeoLocationDataStatus(geoLocationDataStatus);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void ImsRadioAidlImpl::notifyOnUssdMessage(std::shared_ptr<QcRilUnsolOnUssdMessage> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is null");
    return;
  }

  auto indCb = getIndicationCallback();
  if (!indCb) {
    QCRIL_LOG_ERROR("indCb is null");
    return;
  }

  aidlimports::UssdModeType modeType = ims::utils::convertToAidlUssdModeType(msg->getMode());
  string ussdMsg = "";
  if (msg->hasMessage()) {
    ussdMsg = msg->getMessage();
  }
  aidlimports::SipErrorInfo errorDetails;
  ims::utils::convertToAidlSipErrorInfo(errorDetails, msg->getErrorDetails(), false);
  imsRadiolog("<", "onUssdMessage: type= " + android::internal::ToString(modeType) +
                        " ussdMsg=" + android::internal::ToString(ussdMsg) + " errorDetails=" +
                        android::internal::ToString(errorDetails));
  auto ret = indCb->onUssdReceived(modeType, ussdMsg, errorDetails);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void ImsRadioAidlImpl::notifyOnSipDtmfReceived(std::shared_ptr<QcRilUnsolOnSipDtmfMessage> msg) {
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    return;
  }
  auto indCb = getIndicationCallback();
  if (!indCb) {
    QCRIL_LOG_DEBUG("cannot fallback");
    return;
  }
  string sipMsg = "";
  if (msg->hasMessage()) {
    sipMsg = msg->getMessage();
  }
  imsRadiolog("<", "OnSipDtmf: sipMsg= " + android::internal::ToString(sipMsg));
  auto ret = indCb->onSipDtmfReceived(sipMsg);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void ImsRadioAidlImpl::notifyOnServiceDomainChanged(std::shared_ptr<RilUnsolNasSysInfo> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is nullptr");
    return;
  }
  auto indCb = getIndicationCallback();
  if (!indCb) {
    QCRIL_LOG_ERROR("indCb is null");
    return;
  }

  aidlimports::SystemServiceDomain systemServiceDomain =
      ims::utils::convertToAidlSystemServiceDomain(msg->getImsDomain());
  imsRadiolog("<", "onServiceDomainChanged: systemServiceDomain=" +
                         toString(systemServiceDomain));
  auto ret = indCb->onServiceDomainChanged(systemServiceDomain);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void ImsRadioAidlImpl::notifyOnConferenceCallStateCompleted(
    std::shared_ptr<QcRilUnsolImsConferenceCallStateCompletedMessage> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is nullptr");
    return;
  }
  auto indCb = getIndicationCallback();
  if (!indCb) {
    QCRIL_LOG_ERROR("indCb is null");
    return;
  }
  imsRadiolog("<", "onConferenceCallStateCompleted");
  auto ret = indCb->onConferenceCallStateCompleted();
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void ImsRadioAidlImpl::notifyOnSmsCallBackModeChanged(
    std::shared_ptr<QcRilUnsolImsSmsCallbackModeMessage> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is nullptr");
    return;
  }
  auto indCb = getIndicationCallback();
  if (!indCb) {
    QCRIL_LOG_ERROR("indCb is null");
    return;
  }

  aidlimports::SmsCallBackMode mode =
      ims::utils::convertToAidlSmsCallBackMode(msg->getSmsCallbackModeStatus());
  imsRadiolog("<", "onSmsCallBackModeChanged: mode=" + toString(mode));
  auto ret = indCb->onSmsCallBackModeChanged(mode);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void ImsRadioAidlImpl::notifyOnIncomingDtmfStart(std::shared_ptr<QcRilUnsolDtmfMessage> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is nullptr");
    return;
  }
  auto indCb = getIndicationCallback();
  if (!indCb) {
    QCRIL_LOG_ERROR("indCb is null");
    return;
  }

  aidlimports::DtmfInfo dtmfInfo = ims::utils::convertToAidlDtmfInfo(*msg);
  imsRadiolog("<", "onIncomingDtmfStart: dtmf info = " + android::internal::ToString(dtmfInfo));
  auto ret = indCb->onIncomingDtmfStart(dtmfInfo);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void ImsRadioAidlImpl::notifyOnIncomingDtmfStop(std::shared_ptr<QcRilUnsolDtmfMessage> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is nullptr");
    return;
  }
  auto indCb = getIndicationCallback();
  if (!indCb) {
    QCRIL_LOG_ERROR("indCb is null");
    return;
  }

  aidlimports::DtmfInfo dtmfInfo = ims::utils::convertToAidlDtmfInfo(*msg);
  imsRadiolog("<", "onIncomingDtmfStop: dtmf info = " + android::internal::ToString(dtmfInfo));
  auto ret = indCb->onIncomingDtmfStop(dtmfInfo);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void ImsRadioAidlImpl::notifyOnMultiSimVoiceCapabilityChanged(
    std::shared_ptr<QcRilUnsolImsMultiSimVoiceCapabilityChanged> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is null");
    return;
  }

  aidlimports::MultiSimVoiceCapability voiceCapability =
        ims::utils::convertToAidlMultiSimVoiceCapability(msg->getMultiSimVoiceCapability());
  notifyOnMultiSimVoiceCapabilityChanged(voiceCapability);
}

void ImsRadioAidlImpl::notifyOnPreAlertingCallInfo(
    std::shared_ptr<QcRilUnsolImsPreAlertingCallInfo> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is null");
    return;
  }

  auto indCb = getIndicationCallback();
  if (!indCb) {
    QCRIL_LOG_ERROR("indCb is null");
    return;
  }

  aidlimports::PreAlertingCallInfo preAlertingInfo{};
  ims::utils::convertToAidlPreAlertingCallInfo(preAlertingInfo, msg->getPreAlertingInfo());
  imsRadiolog("<", "onPreAlertingCallInfoAvailable: info = " +
                       ::android::internal::ToString(preAlertingInfo));
  auto ret = indCb->onPreAlertingCallInfoAvailable(preAlertingInfo);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void ImsRadioAidlImpl::notifyOnCIWlanNotification(
    std::shared_ptr<QcRilUnsolImsCIwlanNotification> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is null");
    return;
  }

  auto indCb = getIndicationCallback();
  if (!indCb) {
    QCRIL_LOG_ERROR("indCb is null");
    return;
  }

  aidlimports::CiWlanNotificationInfo info{};
  info = ims::utils::convertToAidlCIWlanNotification(msg->getNotificationStatus());
  imsRadiolog("<", "onCiWlanNotification: status = " + android::internal::ToString(info));
  auto ret = indCb->onCiWlanNotification(info);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void ImsRadioAidlImpl::notifySrtpEnscryptionStatus(
    std::shared_ptr<QcRilUnsolImsSrtpEncryptionStatus> msg)
{
  if (!msg) {
    QCRIL_LOG_ERROR("msg is null");
    return;
  }

  auto indCb = getIndicationCallback();
  if (!indCb) {
    QCRIL_LOG_ERROR("indCb is null");
    return;
  }

  aidlimports::SrtpEncryptionInfo info = ims::utils::convertToAidlSrtpEncryptionInfo(*msg);
  imsRadiolog("<", "onSrtpEncryptionStatusChanged: status = " + android::internal::ToString(info));
  auto ret = indCb->onSrtpEncryptionStatusChanged(info);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

// Requests
::ndk::ScopedAStatus ImsRadioAidlImpl::dial(int32_t in_token,
                                            const aidlimports::DialRequest& in_dialRequest) {
  imsRadiolog(">", "dial: token = " + std::to_string(in_token) +
                       " dialRequest: " + hidePhoneNumber(::android::internal::ToString(in_dialRequest)));
  auto msg = ims::utils::makeQcRilRequestDialMessage(getContext(in_token), in_dialRequest);
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendDialResponse(in_token, aidlimports::ErrorCode::RIL_INTERNAL_NO_MEMORY);
    return ndk::ScopedAStatus::ok();
  }

  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
          } else {
            errorCode = aidlimports::ErrorCode::RIL_INTERNAL_GENERIC_FAILURE;
          }
        }
        sendDialResponse(in_token, errorCode);
      });
  msg->setCallback(&cb);
  msg->dispatch();

  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus ImsRadioAidlImpl::addParticipant(
    int32_t in_token, const aidlimports::DialRequest& in_dialRequest) {
  imsRadiolog(">", "addParticipant: token = " + std::to_string(in_token) +
                       " dialRequest = " + hidePhoneNumber(::android::internal::ToString(in_dialRequest)));
  auto msg = std::make_shared<QcRilRequestImsAddParticipantMessage>(getContext(in_token));
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendAddParticipantResponse(in_token, aidlimports::ErrorCode::NO_MEMORY);
    return ndk::ScopedAStatus::ok();
  }

  // Set parameters
  if (!in_dialRequest.address.empty()) {
    msg->setAddress(in_dialRequest.address);
  }

  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        sendAddParticipantResponse(in_token, errorCode);
      });
  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus ImsRadioAidlImpl::getImsRegistrationState(int32_t in_token) {
  imsRadiolog(">", "getImsRegistrationState: token = " + std::to_string(in_token));
  auto msg = std::make_shared<QcRilRequestImsGetRegStateMessage>(getContext(in_token));
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendGetRegistrationResponse(in_token, aidlimports::ErrorCode::NO_MEMORY, nullptr);
    return ndk::ScopedAStatus::ok();
  }

  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        std::shared_ptr<qcril::interfaces::Registration> respData = nullptr;
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
            respData = std::static_pointer_cast<qcril::interfaces::Registration>(resp->data);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        sendGetRegistrationResponse(in_token, errorCode, respData);
      });
  msg->setCallback(&cb);
  msg->dispatch();

  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus ImsRadioAidlImpl::answer(int32_t in_token,
                                              const aidlimports::AnswerRequest& in_answerRequest) {
  imsRadiolog(">", "answer: token = " + std::to_string(in_token) +
                       " answerRequest = " + ::android::internal::ToString(in_answerRequest));
  auto msg = std::make_shared<QcRilRequestAnswerMessage>(getContext(in_token));
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendAnswerResponse(in_token, aidlimports::ErrorCode::NO_MEMORY);
    return ndk::ScopedAStatus::ok();
  }

  // Set parameters
  msg->setCallType(ims::utils::convertToRilCallType(in_answerRequest.callType));

  if (in_answerRequest.presentation != aidlimports::IpPresentation::INVALID) {
    msg->setPresentation(ims::utils::convertToRilIpPresentation(in_answerRequest.presentation));
  }
  if (in_answerRequest.mode != aidlimports::RttMode::INVALID) {
    msg->setRttMode(ims::utils::convertToRilRttMode(in_answerRequest.mode));
  }

  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        sendAnswerResponse(in_token, errorCode);
      });
  msg->setCallback(&cb);
  msg->dispatch();

  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus ImsRadioAidlImpl::hangup(int32_t in_token,
                                              const aidlimports::HangupRequestInfo& in_hangup) {
  imsRadiolog(">", "hangup: token = " + std::to_string(in_token) +
                       " hangup = " + ::android::internal::ToString(in_hangup));
  auto msg = std::make_shared<QcRilRequestHangupMessage>(getContext(in_token));
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendHangupResponse(in_token, aidlimports::ErrorCode::NO_MEMORY);
    return ndk::ScopedAStatus::ok();
  }

  // Set parameters
  if (in_hangup.connIndex != INT32_MAX) {
    msg->setCallIndex(in_hangup.connIndex);
  }

  msg->setIsMultiParty(in_hangup.multiParty);

  if (in_hangup.connUri.size()) {
    msg->setConnectionUri(in_hangup.connUri);
  }
  if (in_hangup.conf_id != INT32_MAX) {
    msg->setConferenceId(in_hangup.conf_id);
  }
  if (in_hangup.failCauseResponse.failCause != aidlimports::CallFailCause::INVALID) {
    msg->setRejectCause(
        ims::utils::convertToRilCallFailCause(in_hangup.failCauseResponse.failCause));
  }
  const uint32_t errorInfoLen = in_hangup.failCauseResponse.errorInfo.size();
  if (errorInfoLen > 0) {
    std::string errorInfo((const char*)in_hangup.failCauseResponse.errorInfo.data(), errorInfoLen);
    msg->setRejectCauseRaw(std::stoul(errorInfo));
  }

  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        sendHangupResponse(in_token, errorCode);
      });
  msg->setCallback(&cb);
  msg->dispatch();

  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus ImsRadioAidlImpl::requestRegistrationChange(int32_t in_token,
                                                                 aidlimports::RegState in_state) {
  imsRadiolog(">", "requestRegistrationChange: token = " + std::to_string(in_token) +
                       " state = " + toString(in_state));
  qcril::interfaces::RegState regState = ims::utils::convertToRilRegState(in_state);
  if (regState == qcril::interfaces::RegState::UNKNOWN) {
    QCRIL_LOG_ERROR("Invalid parameters");
    sendRequestRegistrationChangeResponse(in_token, aidlimports::ErrorCode::INVALID_PARAMETER,
                                          nullptr);
    return ndk::ScopedAStatus::ok();
  }

  std::shared_ptr<QcRilRequestImsRegistrationChangeMessage> msg =
      std::make_shared<QcRilRequestImsRegistrationChangeMessage>(getContext(in_token), regState);
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendRequestRegistrationChangeResponse(in_token, aidlimports::ErrorCode::NO_MEMORY, nullptr);
    return ndk::ScopedAStatus::ok();
  }

  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        sendRequestRegistrationChangeResponse(in_token, errorCode, nullptr);
      });
  msg->setCallback(&cb);
  msg->dispatch();

  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus ImsRadioAidlImpl::queryServiceStatus(int32_t in_token) {
  imsRadiolog(">", "queryServiceStatus: token = " + std::to_string(in_token));
  std::shared_ptr<QcRilRequestImsQueryServiceStatusMessage> msg =
      std::make_shared<QcRilRequestImsQueryServiceStatusMessage>(getContext(in_token));
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendQueryServiceStatusResponse(in_token, aidlimports::ErrorCode::NO_MEMORY, nullptr);
    return ndk::ScopedAStatus::ok();
  }

  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        std::shared_ptr<qcril::interfaces::ServiceStatusInfoList> respData = nullptr;
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
            respData =
                std::static_pointer_cast<qcril::interfaces::ServiceStatusInfoList>(resp->data);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        sendQueryServiceStatusResponse(in_token, errorCode, respData);
      });
  msg->setCallback(&cb);
  msg->dispatch();

  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus ImsRadioAidlImpl::setServiceStatus(
    int32_t in_token, const std::vector<aidlimports::ServiceStatusInfo>& in_srvStatusInfoList) {
  imsRadiolog(">", "setServiceStatus: token = " + std::to_string(in_token) +
                       " srvStatusInfoList=" + ::android::internal::ToString(in_srvStatusInfoList));
  auto msg = std::make_shared<QcRilRequestImsSetServiceStatusMessage>(getContext(in_token));
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendSetServiceStatusMessageResponse(in_token, aidlimports::ErrorCode::NO_MEMORY, nullptr);
    return ndk::ScopedAStatus::ok();
  }

  for (const auto& srvStatusInfo : in_srvStatusInfoList) {
    auto networkMode = aidlimports::RadioTechType::INVALID;
    auto status = aidlimports::StatusType::INVALID;
    if (srvStatusInfo.accTechStatus.size()) {
      networkMode = srvStatusInfo.accTechStatus[0].networkMode;
      status = srvStatusInfo.accTechStatus[0].status;
    }

    if ((srvStatusInfo.callType == aidlimports::CallType::VOICE) &&
        (networkMode == aidlimports::RadioTechType::LTE)) {
      msg->setVolteEnabled(status != aidlimports::StatusType::DISABLED);
    }
    if ((srvStatusInfo.callType == aidlimports::CallType::VOICE) &&
        ((networkMode == aidlimports::RadioTechType::IWLAN) ||
         (networkMode == aidlimports::RadioTechType::WIFI))) {
      msg->setWifiCallingEnabled(status != aidlimports::StatusType::DISABLED);
    }
    if (srvStatusInfo.callType == aidlimports::CallType::VT) {
      msg->setVideoTelephonyEnabled(status != aidlimports::StatusType::DISABLED);
    }
    if (srvStatusInfo.callType == aidlimports::CallType::UT) {
      msg->setUTEnabled(status != aidlimports::StatusType::DISABLED);
    }
    #ifndef QMI_RIL_UTF
    // If C_IWLAN capability received from telephony, send request to data module for
    // enable/disable C_IWLAN
    if ((srvStatusInfo.callType == aidlimports::CallType::VOICE) &&
            (networkMode == aidlimports::RadioTechType::C_IWLAN)) {
       auto msg_data = std::make_shared<rildata::UpdateUIOptionMessage>();
       if (msg_data != nullptr) {
         msg_data->setCIWlanUIOption(status != aidlimports::StatusType::DISABLED);
         msg_data->dispatch();
      }
    }
    #endif
  }

  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        sendSetServiceStatusMessageResponse(in_token, errorCode, nullptr);
      });
  msg->setCallback(&cb);
  msg->dispatch();

  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus ImsRadioAidlImpl::hold(int32_t in_token, int32_t in_callId) {
  imsRadiolog(
      ">", "hold: token = " + std::to_string(in_token) + " callId = " + std::to_string(in_callId));
  auto msg = std::make_shared<QcRilRequestImsHoldMessage>(getContext(in_token));
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendHoldResponse(in_token, aidlimports::ErrorCode::NO_MEMORY, nullptr);
    return ndk::ScopedAStatus::ok();
  }

  msg->setCallId(in_callId);
  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        std::shared_ptr<qcril::interfaces::SipErrorInfo> errorDetails = nullptr;
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
            errorDetails = std::static_pointer_cast<qcril::interfaces::SipErrorInfo>(resp->data);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        sendHoldResponse(in_token, errorCode, errorDetails);
      });

  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus ImsRadioAidlImpl::resume(int32_t in_token, int32_t in_callId) {
  imsRadiolog(">", "resume: token = " + std::to_string(in_token) +
                       " callId = " + std::to_string(in_callId));
  auto msg = std::make_shared<QcRilRequestImsResumeMessage>(getContext(in_token));
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendResumeResponse(in_token, aidlimports::ErrorCode::NO_MEMORY, nullptr);
    return ndk::ScopedAStatus::ok();
  }

  msg->setCallId(in_callId);
  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        std::shared_ptr<qcril::interfaces::SipErrorInfo> errorDetails = nullptr;
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
            errorDetails = std::static_pointer_cast<qcril::interfaces::SipErrorInfo>(resp->data);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        sendResumeResponse(in_token, errorCode, errorDetails);
      });

  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus ImsRadioAidlImpl::setConfig(int32_t in_token,
                                                 const aidlimports::ConfigInfo& in_config) {
  imsRadiolog(">", "setConfig: token = " + std::to_string(in_token) + " config = " +
                       ::android::internal::ToString(in_config));
  std::shared_ptr<QcRilRequestImsSetConfigMessage> msg =
      std::make_shared<QcRilRequestImsSetConfigMessage>(getContext(in_token));
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendSetConfigMessageResponse(in_token, aidlimports::ErrorCode::NO_MEMORY, nullptr);
    return ndk::ScopedAStatus::ok();
  }

  ims::utils::convertToRilConfigInfo(in_config, msg->getConfigInfo());
  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        std::shared_ptr<qcril::interfaces::ConfigInfo> respData = nullptr;
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
            respData = std::static_pointer_cast<qcril::interfaces::ConfigInfo>(resp->data);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        sendSetConfigMessageResponse(in_token, errorCode, respData);
      });
  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus ImsRadioAidlImpl::getConfig(int32_t in_token,
                                                 const aidlimports::ConfigInfo& in_config) {
  imsRadiolog(">", "getConfig: token=" + std::to_string(in_token) + "config=" +
                       ::android::internal::ToString(in_config));
  std::shared_ptr<QcRilRequestImsGetConfigMessage> msg =
      std::make_shared<QcRilRequestImsGetConfigMessage>(getContext(in_token));
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendGetConfigMessageResponse(in_token, aidlimports::ErrorCode::NO_MEMORY, nullptr);
    return ndk::ScopedAStatus::ok();
  }

  ims::utils::convertToRilConfigInfo(in_config, msg->getConfigInfo());
  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        std::shared_ptr<qcril::interfaces::ConfigInfo> respData = nullptr;
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if(resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
            respData = std::static_pointer_cast<qcril::interfaces::ConfigInfo>(resp->data);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        sendGetConfigMessageResponse(in_token, errorCode, respData);
      });
  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus ImsRadioAidlImpl::conference(int32_t in_token) {
  imsRadiolog(">", "conference: token = " + std::to_string(in_token));
  auto msg = std::make_shared<QcRilRequestConferenceMessage>(getContext(in_token));
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendConferenceResponse(in_token, aidlimports::ErrorCode::NO_MEMORY, nullptr);
    return ndk::ScopedAStatus::ok();
  }

  msg->setIsImsRequest(true);
  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        std::shared_ptr<qcril::interfaces::SipErrorInfo> rilErrorInfo = nullptr;
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
            rilErrorInfo = std::static_pointer_cast<qcril::interfaces::SipErrorInfo>(resp->data);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        sendConferenceResponse(in_token, errorCode, rilErrorInfo);
      });
  msg->setCallback(&cb);
  msg->dispatch();

  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus ImsRadioAidlImpl::getClip(int32_t in_token) {
  imsRadiolog(">", "getClip: token = " + std::to_string(in_token));
  auto msg = std::make_shared<QcRilRequestGetClipMessage>(getContext(in_token));
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendGetClipResponse(in_token, aidlimports::ErrorCode::NO_MEMORY, nullptr);
    return ndk::ScopedAStatus::ok();
  }
  msg->setIsImsRequest(true);
  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        std::shared_ptr<qcril::interfaces::ClipInfo> clipInfo = nullptr;
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
            clipInfo = std::static_pointer_cast<qcril::interfaces::ClipInfo>(resp->data);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        sendGetClipResponse(in_token, errorCode, clipInfo);
      });
  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus ImsRadioAidlImpl::getClir(int32_t in_token) {
  imsRadiolog(">", "getClir: token = " + std::to_string(in_token));
  auto msg = std::make_shared<QcRilRequestGetClirMessage>(getContext(in_token));
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendGetClirResponse(in_token, aidlimports::ErrorCode::NO_MEMORY, nullptr);
    return ndk::ScopedAStatus::ok();
  }

  msg->setIsImsRequest(true);
  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        std::shared_ptr<qcril::interfaces::ClirInfo> clirInfo = nullptr;
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
            clirInfo = std::static_pointer_cast<qcril::interfaces::ClirInfo>(resp->data);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        sendGetClirResponse(in_token, errorCode, clirInfo);
      });
  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus ImsRadioAidlImpl::setClir(int32_t in_token,
                                               const aidlimports::ClirInfo& in_clirInfo) {
  imsRadiolog(">", "setClir: token = " + std::to_string(in_token) + " clirInfo = " +
                       ::android::internal::ToString(in_clirInfo));
  auto msg = std::make_shared<QcRilRequestSetClirMessage>(getContext(in_token));
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendSetClirResponse(in_token, aidlimports::ErrorCode::NO_MEMORY);
    return ndk::ScopedAStatus::ok();
  }

  msg->setIsImsRequest(true);
  msg->setParamN(in_clirInfo.paramN);
  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        sendSetClirResponse(in_token, errorCode);
      });
  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus ImsRadioAidlImpl::getColr(int32_t in_token) {
  imsRadiolog(">", "getColr: token = " + std::to_string(in_token));
  auto msg = std::make_shared<QcRilRequestGetColrMessage>(getContext(in_token));
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendGetColrResponse(in_token, aidlimports::ErrorCode::NO_MEMORY, nullptr);
    return ndk::ScopedAStatus::ok();
  }

  msg->setIsImsRequest(true);
  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        std::shared_ptr<qcril::interfaces::ColrInfo> colrInfo = nullptr;
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
            colrInfo = std::static_pointer_cast<qcril::interfaces::ColrInfo>(resp->data);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        sendGetColrResponse(in_token, errorCode, colrInfo);
      });
  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus ImsRadioAidlImpl::setColr(int32_t in_token,
                                               const aidlimports::ColrInfo& in_colrInfo) {
  imsRadiolog(">", "setColr: token = " + std::to_string(in_token) +
                       " colrInfo = " + ::android::internal::ToString(in_colrInfo));
  auto msg = std::make_shared<QcRilRequestSetColrMessage>(getContext(in_token));
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendSuppServiceStatusResponse(in_token, aidlimports::ErrorCode::NO_MEMORY, nullptr);
    return ndk::ScopedAStatus::ok();
  }

  msg->setIsImsRequest(true);
  // Set parameters
  if (in_colrInfo.presentation != aidlimports::IpPresentation::INVALID) {
    msg->setPresentation(ims::utils::convertToRilIpPresentation(in_colrInfo.presentation));
  }
  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        std::shared_ptr<qcril::interfaces::SipErrorInfo> errorDetails = nullptr;
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
            errorDetails = std::static_pointer_cast<qcril::interfaces::SipErrorInfo>(resp->data);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        auto data = std::make_shared<qcril::interfaces::SuppServiceStatusInfo>();
        if (data) {
          data->setErrorDetails(errorDetails);
        }
        sendSuppServiceStatusResponse(in_token, errorCode, data);
      });
  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus ImsRadioAidlImpl::exitEmergencyCallbackMode(int32_t in_token) {
  imsRadiolog(">", "exitEmergencyCallbackMode: token = " + std::to_string(in_token));
  auto msg = std::make_shared<RilRequestExitEmergencyCallbackMessage>(getContext(in_token));
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendExitEmergencyCallbackModeResponse(in_token, aidlimports::ErrorCode::NO_MEMORY);
    return ndk::ScopedAStatus::ok();
  }

  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        sendExitEmergencyCallbackModeResponse(in_token, errorCode);
      });
  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus ImsRadioAidlImpl::exitSmsCallBackMode(int32_t in_token) {
  imsRadiolog(">", "exitSmsCallBackMode: token = " + std::to_string(in_token));
  auto msg = std::make_shared<QcRilRequestImsExitSmsCallbackModeMessage>(getContext(in_token));
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendExitSmsCallBackModeResponse(in_token, aidlimports::ErrorCode::NO_MEMORY);
    return ndk::ScopedAStatus::ok();
  }

  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        sendExitSmsCallBackModeResponse(in_token, errorCode);
      });
  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus ImsRadioAidlImpl::sendDtmf(int32_t in_token,
                                                const aidlimports::DtmfInfo& in_dtmfInfo) {
  imsRadiolog(">", "sendDtmf: token = " + std::to_string(in_token) + " dtmfInfo = " +
                       ::android::internal::ToString(in_dtmfInfo));
  auto msg = std::make_shared<QcRilRequestSendDtmfMessage>(getContext(in_token));
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendDtmfResponse(in_token, aidlimports::ErrorCode::NO_MEMORY);
    return ndk::ScopedAStatus::ok();
  }

  // Set parameters
  if (!in_dtmfInfo.dtmf.empty()) {
    // Only one digit expected.
    msg->setDigit(in_dtmfInfo.dtmf[0]);
  }
  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        sendDtmfResponse(in_token, errorCode);
      });
  msg->setCallback(&cb);
  msg->dispatch();

  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus ImsRadioAidlImpl::startDtmf(int32_t in_token,
                                                 const aidlimports::DtmfInfo& in_dtmfInfo) {
  imsRadiolog(">", "startDtmf: token = " + std::to_string(in_token) + " dtmfInfo = " +
                       ::android::internal::ToString(in_dtmfInfo));
  auto msg = std::make_shared<QcRilRequestStartDtmfMessage>(getContext(in_token));
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendStartDtmfResponse(in_token, aidlimports::ErrorCode::NO_MEMORY);
    return ndk::ScopedAStatus::ok();
  }

  // Set parameters
  if (!in_dtmfInfo.dtmf.empty()) {
    // Only one digit expected.
    msg->setDigit(in_dtmfInfo.dtmf[0]);
  }
  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        sendStartDtmfResponse(in_token, errorCode);
      });
  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus ImsRadioAidlImpl::stopDtmf(int32_t in_token) {
  imsRadiolog(">", "stopDtmf: token = " + std::to_string(in_token));
  auto msg = std::make_shared<QcRilRequestStopDtmfMessage>(getContext(in_token));
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendStopDtmfResponse(in_token, aidlimports::ErrorCode::NO_MEMORY);
    return ndk::ScopedAStatus::ok();
  }

  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        sendStopDtmfResponse(in_token, errorCode);
      });
  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus ImsRadioAidlImpl::setUiTtyMode(int32_t in_token,
                                                    const aidlimports::TtyInfo& in_ttyInfo) {
  imsRadiolog(">", "setUiTtyMode: token = " + std::to_string(in_token) + " ttyInfo = " +
                       ::android::internal::ToString(in_ttyInfo));
  auto msg = std::make_shared<QcRilRequestImsSetUiTtyModeMessage>(getContext(in_token));
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendSetUiTTYModeResponse(in_token, aidlimports::ErrorCode::NO_MEMORY);
    return ndk::ScopedAStatus::ok();
  }

  // Set parameters
  if (in_ttyInfo.mode != aidlimports::TtyMode::INVALID) {
    msg->setTtyMode(ims::utils::convertToRilTtyMode(in_ttyInfo.mode));
  }
  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        sendSetUiTTYModeResponse(in_token, errorCode);
      });
  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus ImsRadioAidlImpl::modifyCallInitiate(
    int32_t in_token, const aidlimports::CallModifyInfo& in_callModifyInfo) {
  imsRadiolog(">", "modifyCallInitiate: token = " + std::to_string(in_token) +
                       " callModifyInfo = " + ::android::internal::ToString(in_callModifyInfo));
  auto msg = std::make_shared<QcRilRequestImsModifyCallInitiateMessage>(getContext(in_token));
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendModifyCallInitiateResponse(in_token, aidlimports::ErrorCode::NO_MEMORY);
    return ndk::ScopedAStatus::ok();
  }

  // Set parameters
  msg->setCallId(in_callModifyInfo.callIndex);
  if (in_callModifyInfo.callDetails.callType != aidlimports::CallType::UNKNOWN) {
    msg->setCallType(ims::utils::convertToRilCallType(in_callModifyInfo.callDetails.callType));
  }
  if (in_callModifyInfo.callDetails.callDomain != aidlimports::CallDomain::INVALID) {
    msg->setCallDomain(ims::utils::convertToRilCallDomain(
        in_callModifyInfo.callDetails.callDomain));
  }
  if (in_callModifyInfo.callDetails.rttMode != aidlimports::RttMode::INVALID) {
    msg->setRttMode(ims::utils::convertToRilRttMode(in_callModifyInfo.callDetails.rttMode));
  }

  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        sendModifyCallInitiateResponse(in_token, errorCode);
      });
  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus ImsRadioAidlImpl::modifyCallConfirm(
    int32_t in_token, const aidlimports::CallModifyInfo& in_callModifyInfo) {
  imsRadiolog(">", "modifyCallConfirm: token = " + std::to_string(in_token) + " callModifyInfo = " +
                       ::android::internal::ToString(in_callModifyInfo));
  auto msg = std::make_shared<QcRilRequestImsModifyCallConfirmMessage>(getContext(in_token));
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendModifyCallConfirmResponse(in_token, aidlimports::ErrorCode::NO_MEMORY);
    return ndk::ScopedAStatus::ok();
  }

  // Set parameters
  msg->setCallId(in_callModifyInfo.callIndex);
  if (in_callModifyInfo.callDetails.callType != aidlimports::CallType::UNKNOWN) {
    msg->setCallType(ims::utils::convertToRilCallType(in_callModifyInfo.callDetails.callType));
  }
  if (in_callModifyInfo.callDetails.callDomain != aidlimports::CallDomain::INVALID) {
    msg->setCallDomain(ims::utils::convertToRilCallDomain(
        in_callModifyInfo.callDetails.callDomain));
  }
  if (in_callModifyInfo.callDetails.rttMode != aidlimports::RttMode::INVALID) {
    msg->setRttMode(ims::utils::convertToRilRttMode(in_callModifyInfo.callDetails.rttMode));
  }

  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        sendModifyCallConfirmResponse(in_token, errorCode);
      });
  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus ImsRadioAidlImpl::queryCallForwardStatus(
    int32_t in_token, const aidlimports::CallForwardInfo& in_callForwardInfo) {
  imsRadiolog(">", "queryCallForwardStatus: token = " + std::to_string(in_token) +
                       " callForwardInfo = " + hidePhoneNumber(::android::internal::ToString(in_callForwardInfo)));
  auto msg = std::make_shared<QcRilRequestQueryCallForwardMessage>(getContext(in_token));
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendQueryCallForwardStatusResponse(in_token, aidlimports::ErrorCode::NO_MEMORY, nullptr);
    return ndk::ScopedAStatus::ok();
  }
  msg->setIsImsRequest(true);
  // Set parameters
  if (in_callForwardInfo.reason != INT32_MAX) {
    msg->setReason(in_callForwardInfo.reason);
  }
  if (in_callForwardInfo.serviceClass != INT32_MAX) {
    msg->setServiceClass(in_callForwardInfo.serviceClass);
  }
  msg->setExpectMore(in_callForwardInfo.expectMore);

  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        std::shared_ptr<qcril::interfaces::GetCallForwardRespData> data = nullptr;
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
            data = std::static_pointer_cast<qcril::interfaces::GetCallForwardRespData>(resp->data);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        sendQueryCallForwardStatusResponse(in_token, errorCode, data);
      });
  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus ImsRadioAidlImpl::setCallForwardStatus(
    int32_t in_token, const aidlimports::CallForwardInfo& in_callForwardInfo) {
  imsRadiolog(">", "setCallForwardStatus: token = " + std::to_string(in_token) +
                       " callForwardInfo = " + hidePhoneNumber(::android::internal::ToString(in_callForwardInfo)));
  auto msg = std::make_shared<QcRilRequestSetCallForwardMessage>(getContext(in_token));
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendSetCallForwardStatusResponse(in_token, aidlimports::ErrorCode::NO_MEMORY, nullptr);
    return ndk::ScopedAStatus::ok();
  }

  msg->setIsImsRequest(true);
  // Set parameters
  if (in_callForwardInfo.status != INT32_MAX) {
    msg->setStatus(in_callForwardInfo.status);
  }
  if (in_callForwardInfo.reason != INT32_MAX) {
    msg->setReason(in_callForwardInfo.reason);
  }
  if (in_callForwardInfo.serviceClass != INT32_MAX) {
    msg->setServiceClass(in_callForwardInfo.serviceClass);
  }
  if (in_callForwardInfo.toa != INT32_MAX) {
    msg->setToa(in_callForwardInfo.toa);
  }
  if (!in_callForwardInfo.number.empty()) {
    msg->setNumber(in_callForwardInfo.number);
  }
  if (in_callForwardInfo.timeSeconds != INT32_MAX) {
    msg->setTimeSeconds(in_callForwardInfo.timeSeconds);
  }
  if (in_callForwardInfo.callFwdTimerStart.hour != INT32_MAX &&
      in_callForwardInfo.callFwdTimerStart.minute != INT32_MAX) {
    std::shared_ptr<qcril::interfaces::CallFwdTimerInfo> callFwdTimerStart =
        std::make_shared<qcril::interfaces::CallFwdTimerInfo>();
    if (callFwdTimerStart) {
      ims::utils::convertToRilCallFwdTimerInfo(callFwdTimerStart,
                                               in_callForwardInfo.callFwdTimerStart);
      msg->setCallFwdTimerStart(callFwdTimerStart);
    }
  }

  if (in_callForwardInfo.callFwdTimerEnd.hour != INT32_MAX &&
      in_callForwardInfo.callFwdTimerEnd.minute != INT32_MAX) {
    std::shared_ptr<qcril::interfaces::CallFwdTimerInfo> callFwdTimerEnd =
        std::make_shared<qcril::interfaces::CallFwdTimerInfo>();
    if (callFwdTimerEnd) {
      ims::utils::convertToRilCallFwdTimerInfo(callFwdTimerEnd, in_callForwardInfo.callFwdTimerEnd);
      msg->setCallFwdTimerEnd(callFwdTimerEnd);
    }
  }

  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        std::shared_ptr<qcril::interfaces::SetCallForwardRespData> data = nullptr;
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
            data = std::static_pointer_cast<qcril::interfaces::SetCallForwardRespData>(resp->data);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        sendSetCallForwardStatusResponse(in_token, errorCode, data);
      });
  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus ImsRadioAidlImpl::getCallWaiting(int32_t in_token, int32_t in_serviceClass) {
  imsRadiolog(">", "getCallWaiting: token = " + std::to_string(in_token) + " serviceClass = " +
                       std::to_string(in_serviceClass));
  auto msg = std::make_shared<QcRilRequestGetCallWaitingMessage>(getContext(in_token));
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendGetCallWaitingResponse(in_token, aidlimports::ErrorCode::NO_MEMORY, nullptr);
    return ndk::ScopedAStatus::ok();
  }

  msg->setIsImsRequest(true);
  // Set parameters
  if (in_serviceClass != INT32_MAX) {
    msg->setServiceClass(in_serviceClass);
  }
  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        std::shared_ptr<qcril::interfaces::CallWaitingInfo> cwInfo = nullptr;
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
            cwInfo = std::static_pointer_cast<qcril::interfaces::CallWaitingInfo>(resp->data);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        sendGetCallWaitingResponse(in_token, errorCode, cwInfo);
      });
  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus ImsRadioAidlImpl::setCallWaiting(
    int32_t in_token, const aidlimports::CallWaitingInfo& in_callWaitingInfo) {
  imsRadiolog(">", "setCallWaiting: token = " + std::to_string(in_token) + " serviceClass = " +
                       std::to_string(in_callWaitingInfo.serviceClass));
  auto msg = std::make_shared<QcRilRequestSetCallWaitingMessage>(getContext(in_token));
  if (msg == nullptr) {
    sendSuppServiceStatusResponse(in_token, aidlimports::ErrorCode::NO_MEMORY, nullptr);
    return ndk::ScopedAStatus::ok();
  }

  msg->setIsImsRequest(true);
  // Set parameters
  if (in_callWaitingInfo.serviceStatus != aidlimports::ServiceClassStatus::INVALID) {
    msg->setServiceStatus(
        ims::utils::convertToRilServiceClassStatus(in_callWaitingInfo.serviceStatus));
  }
  if (in_callWaitingInfo.serviceClass != INT32_MAX) {
    msg->setServiceClass(in_callWaitingInfo.serviceClass);
  }
  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        std::shared_ptr<qcril::interfaces::SipErrorInfo> errorDetails = nullptr;
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
            errorDetails = std::static_pointer_cast<qcril::interfaces::SipErrorInfo>(resp->data);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        auto data = std::make_shared<qcril::interfaces::SuppServiceStatusInfo>();
        if (data) {
          data->setErrorDetails(errorDetails);
        }
        sendSuppServiceStatusResponse(in_token, errorCode, data);
      });
  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus ImsRadioAidlImpl::setSuppServiceNotification(
    int32_t in_token, aidlimports::ServiceClassStatus in_status) {
  imsRadiolog(">", "setSuppServiceNotification: token = " + std::to_string(in_token) +
                       " status = " + toString(in_status));

  auto msg = std::make_shared<QcRilRequestSetSuppSvcNotificationMessage>(getContext(in_token));
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendSetSuppServiceNotificationResponse(in_token, aidlimports::ErrorCode::NO_MEMORY);
    return ndk::ScopedAStatus::ok();
  }
  // Set parameters
  if (in_status != aidlimports::ServiceClassStatus::INVALID) {
    msg->setStatus((in_status == aidlimports::ServiceClassStatus::ENABLED) ? true : false);
  } else {
    QCRIL_LOG_ERROR("Invalid parameter");
    sendSetSuppServiceNotificationResponse(in_token, aidlimports::ErrorCode::INVALID_PARAMETER);
    return ndk::ScopedAStatus::ok();
  }
  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        sendSetSuppServiceNotificationResponse(in_token, errorCode);
      });
  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus ImsRadioAidlImpl::explicitCallTransfer(
    int32_t in_token, const aidlimports::ExplicitCallTransferInfo& in_ectInfo) {
  imsRadiolog(">", "explicitCallTransfer: token = " + std::to_string(in_token) + " ectInfo = " +
                       ::android::internal::ToString(in_ectInfo));
  auto msg = std::make_shared<QcRilRequestExplicitCallTransferMessage>(getContext(in_token));
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendExplicitCallTransferResponse(in_token, aidlimports::ErrorCode::NO_MEMORY);
    return ndk::ScopedAStatus::ok();
  }

  // Set parameters
  if (in_ectInfo.callId != INT32_MAX) {
    msg->setCallId(in_ectInfo.callId);
  }
  msg->setEctType(ims::utils::convertToRilEctType(in_ectInfo.ectType));

  if (!in_ectInfo.targetAddress.empty()) {
    msg->setTargetAddress(in_ectInfo.targetAddress);
  }
  if (in_ectInfo.targetCallId != INT32_MAX) {
    msg->setTargetCallId(in_ectInfo.targetCallId);
  }
  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        sendExplicitCallTransferResponse(in_token, errorCode);
      });
  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}
std::shared_ptr<QcRilRequestMessage> ImsRadioAidlImpl::createSupsServiceMessage(int32_t in_token,
    const aidlimports::SuppServiceStatusRequest& in_suppServiceStatusRequest) {
  auto setSupsMsg = std::make_shared<QcRilRequestSetSupsServiceMessage>(getContext(in_token));
  if (setSupsMsg == nullptr) {
    QCRIL_LOG_ERROR("setSupsMsg is nullptr");
    return nullptr;
  }
  // Set parameters
  if (in_suppServiceStatusRequest.operationType != aidlimports::SuppSvcOperationType::INVALID) {
    setSupsMsg->setOperationType(
        ims::utils::convertToRilOperationType(in_suppServiceStatusRequest.operationType));
  }
  if (in_suppServiceStatusRequest.facilityType != aidlimports::FacilityType::INVALID) {
    setSupsMsg->setFacilityType(
        ims::utils::convertToRilFacilityType(in_suppServiceStatusRequest.facilityType));
  }
  if (in_suppServiceStatusRequest.cbNumListInfo.serviceClass != INT32_MAX) {
    setSupsMsg->setServiceClass(in_suppServiceStatusRequest.cbNumListInfo.serviceClass);
  }
  if (in_suppServiceStatusRequest.facilityType == aidlimports::FacilityType::BS_MT) {
    if (in_suppServiceStatusRequest.cbNumListInfo.cbNumInfo.size() > 0) {
      std::vector<std::string> cbNumList;
      for (uint32_t i = 0; i < in_suppServiceStatusRequest.cbNumListInfo.cbNumInfo.size();
            ++i) {
        cbNumList.push_back(in_suppServiceStatusRequest.cbNumListInfo.cbNumInfo[i].number);
      }
      setSupsMsg->setCallBarringNumberList(cbNumList);
    }
  }
  if (in_suppServiceStatusRequest.password.size()) {
    setSupsMsg->setPassword(in_suppServiceStatusRequest.password);
  }
  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        std::shared_ptr<qcril::interfaces::SipErrorInfo> errorDetails = nullptr;
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
            errorDetails = std::static_pointer_cast<qcril::interfaces::SipErrorInfo>(resp->data);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        auto data = std::make_shared<qcril::interfaces::SuppServiceStatusInfo>();
        if (data) {
          data->setErrorDetails(errorDetails);
        }
        sendSuppServiceStatusResponse(in_token, errorCode, data);
      });
  setSupsMsg->setCallback(&cb);
  return setSupsMsg;
}
std::shared_ptr<QcRilRequestMessage> ImsRadioAidlImpl::createColpMessage(int32_t in_token) {
  auto queryColp = std::make_shared<QcRilRequestQueryColpMessage>(getContext(in_token));
  if (queryColp == nullptr) {
    QCRIL_LOG_ERROR("queryColp is nullptr");
    return nullptr;
  }
  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        std::shared_ptr<qcril::interfaces::SuppServiceStatusInfo> data = nullptr;
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
            data =
                std::static_pointer_cast<qcril::interfaces::SuppServiceStatusInfo>(resp->data);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        sendSuppServiceStatusResponse(in_token, errorCode, data);
      });
  queryColp->setCallback(&cb);
  return queryColp;
}
std::shared_ptr<QcRilRequestMessage> ImsRadioAidlImpl::createCallBarringMessage(int32_t in_token,
    const aidlimports::SuppServiceStatusRequest& in_suppServiceStatusRequest) {
  auto queryMsg = std::make_shared<QcRilRequestGetCallBarringMessage>(getContext(in_token));
  if (queryMsg == nullptr) {
    QCRIL_LOG_ERROR("queryMsg is nullptr");
    return nullptr;
  }
  // Set parameters
  if (in_suppServiceStatusRequest.facilityType != aidlimports::FacilityType::INVALID) {
    queryMsg->setFacilityType(
        ims::utils::convertToRilFacilityType(in_suppServiceStatusRequest.facilityType));
  }
  if (in_suppServiceStatusRequest.cbNumListInfo.serviceClass != INT32_MAX) {
    queryMsg->setServiceClass(in_suppServiceStatusRequest.cbNumListInfo.serviceClass);
  }
  queryMsg->setExpectMore(in_suppServiceStatusRequest.expectMore);

  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        std::shared_ptr<qcril::interfaces::SuppServiceStatusInfo> data = nullptr;
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
            data =
                std::static_pointer_cast<qcril::interfaces::SuppServiceStatusInfo>(resp->data);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        sendSuppServiceStatusResponse(in_token, errorCode, data);
      });
  queryMsg->setCallback(&cb);
  return queryMsg;
}
::ndk::ScopedAStatus ImsRadioAidlImpl::suppServiceStatus(
    int32_t in_token, const aidlimports::SuppServiceStatusRequest& in_suppServiceStatusRequest) {
  imsRadiolog(">", "suppServiceStatus: token = " + std::to_string(in_token) + " operationType = " +
                       ::android::internal::ToString(in_suppServiceStatusRequest.operationType) +
                       " facilityType = " +
                       ::android::internal::ToString(in_suppServiceStatusRequest.facilityType) +
                       " cbNumListInfo = " +
                       ::android::internal::ToString(in_suppServiceStatusRequest.cbNumListInfo));
  std::shared_ptr<QcRilRequestMessage> msg = nullptr;
  switch (in_suppServiceStatusRequest.operationType) {
    case aidlimports::SuppSvcOperationType::REGISTER:
    case aidlimports::SuppSvcOperationType::ERASURE:
      if (in_suppServiceStatusRequest.facilityType != aidlimports::FacilityType::BS_MT) {
        QCRIL_LOG_INFO("Unsupported facility type %d for reg or erase",
                        in_suppServiceStatusRequest.facilityType);
        break;
      }
      // fallthrough; use QcRilRequestSetSupsServiceMessage for REGISTER/ERASURE of BS_MT
      [[fallthrough]];
    case aidlimports::SuppSvcOperationType::ACTIVATE:
    case aidlimports::SuppSvcOperationType::DEACTIVATE: {
      msg = createSupsServiceMessage(in_token, in_suppServiceStatusRequest);
    } break;
    case aidlimports::SuppSvcOperationType::QUERY:
      if (in_suppServiceStatusRequest.facilityType == aidlimports::FacilityType::COLP) {
        msg = createColpMessage(in_token);
      } else if (in_suppServiceStatusRequest.facilityType != aidlimports::FacilityType::CLIP) {
        msg = createCallBarringMessage(in_token, in_suppServiceStatusRequest);
      }
      break;
    default:
      break;
  }
  if (msg) {
    msg->setIsImsRequest(true);
    msg->dispatch();
  } else {
    sendSuppServiceStatusResponse(in_token, aidlimports::ErrorCode::GENERIC_FAILURE, nullptr);
  }
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus ImsRadioAidlImpl::getRtpStatistics(int32_t in_token) {
  imsRadiolog(">", "getRtpStatistics: token = " + std::to_string(in_token));
  std::shared_ptr<QcRilRequestImsGetRtpStatsMessage> msg =
      std::make_shared<QcRilRequestImsGetRtpStatsMessage>(getContext(in_token));
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendGetRtpStatsMessageResponse(in_token, aidlimports::ErrorCode::NO_MEMORY, nullptr);
    return ndk::ScopedAStatus::ok();
  }

  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        std::shared_ptr<qcril::interfaces::RtpStatisticsData> respData = nullptr;
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            respData = std::static_pointer_cast<qcril::interfaces::RtpStatisticsData>(resp->data);
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        sendGetRtpStatsMessageResponse(in_token, errorCode, respData);
      });
  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus ImsRadioAidlImpl::getRtpErrorStatistics(int32_t in_token) {
  imsRadiolog(">", "getRtpErrorStatistics: token = " + std::to_string(in_token));
  std::shared_ptr<QcRilRequestImsGetRtpErrorStatsMessage> msg =
      std::make_shared<QcRilRequestImsGetRtpErrorStatsMessage>(getContext(in_token));
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendGetRtpErrorStatsMessageResponse(in_token, aidlimports::ErrorCode::NO_MEMORY, nullptr);
    return ndk::ScopedAStatus::ok();
  }
  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        std::shared_ptr<qcril::interfaces::RtpStatisticsData> respData = nullptr;
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            respData = std::static_pointer_cast<qcril::interfaces::RtpStatisticsData>(resp->data);
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        sendGetRtpErrorStatsMessageResponse(in_token, errorCode, respData);
      });
  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus ImsRadioAidlImpl::sendUssd(int32_t in_token, const std::string& in_ussd) {
  imsRadiolog(">", "sendUssd: token = " + std::to_string(in_token) +
                       " ussd = " + ::android::internal::ToString(in_ussd));
  if (!ims::utils::isUssdOverImsSupported()) {
    QCRIL_LOG_DEBUG("USSD cs fallback");
    sendUssdResponse(in_token, aidlimports::ErrorCode::USSD_CS_FALLBACK, nullptr);
    return ndk::ScopedAStatus::ok();
  }

  auto msg = std::make_shared<QcRilRequestSendUssdMessage>(getContext(in_token));
  if (!msg) {
    QCRIL_LOG_ERROR("msg is null");
    sendUssdResponse(in_token, aidlimports::ErrorCode::NO_MEMORY, nullptr);
    return ndk::ScopedAStatus::ok();
  }

  if (in_ussd.empty()) {
    QCRIL_LOG_ERROR("Invalid parameter: ussd");
    sendUssdResponse(in_token, aidlimports::ErrorCode::INVALID_PARAMETER, nullptr);
    return ndk::ScopedAStatus::ok();
  }

  msg->setIsDomainAuto(true);
  msg->setUssd(in_ussd);
  msg->setIsImsRequest(true);
  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        std::shared_ptr<qcril::interfaces::SipErrorInfo> errorDetails = nullptr;
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
            errorDetails = std::static_pointer_cast<qcril::interfaces::SipErrorInfo>(resp->data);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        sendUssdResponse(in_token, errorCode, errorDetails);
      });
  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus ImsRadioAidlImpl::cancelPendingUssd(int32_t in_token) {
  imsRadiolog(">", "cancelPendingUssd: token=" + std::to_string(in_token));
  auto msg = std::make_shared<QcRilRequestCancelUssdMessage>(getContext(in_token));
  if (!msg) {
    QCRIL_LOG_ERROR("msg is nullptr");
    cancelPendingUssdResponse(in_token, aidlimports::ErrorCode::NO_MEMORY, nullptr);
    return ndk::ScopedAStatus::ok();
  }

  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        std::shared_ptr<qcril::interfaces::SipErrorInfo> errorDetails = nullptr;
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
            errorDetails = std::static_pointer_cast<qcril::interfaces::SipErrorInfo>(resp->data);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        cancelPendingUssdResponse(in_token, errorCode, errorDetails);
      });
  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus ImsRadioAidlImpl::deflectCall(
    int32_t in_token, const aidlimports::DeflectRequestInfo& in_deflectRequestInfo) {
  imsRadiolog(">", "deflectCall: token = " + std::to_string(in_token) + " deflectRequestInfo = " +
                       ::android::internal::ToString(in_deflectRequestInfo));
  auto msg = std::make_shared<QcRilRequestImsDeflectCallMessage>(getContext(in_token));
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendDeflectCallResponse(in_token, aidlimports::ErrorCode::NO_MEMORY);
    return ndk::ScopedAStatus::ok();
  }
  // Set parameters
  if (in_deflectRequestInfo.connIndex != INT32_MAX) {
    msg->setCallId(in_deflectRequestInfo.connIndex);
  }
  if (!in_deflectRequestInfo.number.empty()) {
    msg->setNumber(in_deflectRequestInfo.number);
  }
  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        sendDeflectCallResponse(in_token, errorCode);
      });
  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus ImsRadioAidlImpl::sendGeolocationInfo(
    int32_t in_token, const aidlimports::GeoLocationInfo& in_geoLocationInfo) {
  imsRadiolog(">", "sendGeolocationInfo: token=" + std::to_string(in_token) + " lat=" +
                       PII(std::to_string(in_geoLocationInfo.lat),"<hidden>") + " lon=" +
                       PII(std::to_string(in_geoLocationInfo.lon),"<hidden>") + " addressInfo=" +
                       PII(::android::internal::ToString(in_geoLocationInfo.addressInfo),"<hidden>"));
  std::shared_ptr<QcRilRequestImsGeoLocationInfoMessage> msg =
      std::make_shared<QcRilRequestImsGeoLocationInfoMessage>(getContext(in_token));
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendGeoLocationResponse(in_token, aidlimports::ErrorCode::NO_MEMORY, nullptr);
    return ndk::ScopedAStatus::ok();
  }

  msg->setLatitude(in_geoLocationInfo.lat);
  msg->setLongitude(in_geoLocationInfo.lon);
  if (!in_geoLocationInfo.addressInfo.city.empty()) {
    msg->setCity(in_geoLocationInfo.addressInfo.city);
  }
  if (!in_geoLocationInfo.addressInfo.state.empty()) {
    msg->setState(in_geoLocationInfo.addressInfo.state);
  }
  if (!in_geoLocationInfo.addressInfo.country.empty()) {
    msg->setCountry(in_geoLocationInfo.addressInfo.country);
  }
  if (!in_geoLocationInfo.addressInfo.postalCode.empty()) {
    msg->setPostalCode(in_geoLocationInfo.addressInfo.postalCode);
  }
  if (!in_geoLocationInfo.addressInfo.countryCode.empty()) {
    msg->setCountryCode(in_geoLocationInfo.addressInfo.countryCode);
  }
  if (!in_geoLocationInfo.addressInfo.street.empty()) {
    msg->setStreet(in_geoLocationInfo.addressInfo.street);
  }
  if (!in_geoLocationInfo.addressInfo.houseNumber.empty()) {
    msg->setHouseNumber(in_geoLocationInfo.addressInfo.houseNumber);
  }
  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        sendGeoLocationResponse(in_token, errorCode, nullptr);
      });
  msg->setCallback(&cb);
  msg->dispatch();

  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus ImsRadioAidlImpl::getImsSubConfig(int32_t in_token) {
  imsRadiolog(">", "getImsSubConfig: token = " + std::to_string(in_token));

  std::shared_ptr<QcRilRequestImsGetSubConfigMessage> msg =
      std::make_shared<QcRilRequestImsGetSubConfigMessage>(getContext(in_token));
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendGetImsSubConfigMessageResponse(in_token, aidlimports::ErrorCode::NO_MEMORY, nullptr);
    return ndk::ScopedAStatus::ok();
  }

  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        std::shared_ptr<qcril::interfaces::ImsSubConfigInfo> respData = nullptr;
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            respData = std::static_pointer_cast<qcril::interfaces::ImsSubConfigInfo>(resp->data);
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        sendGetImsSubConfigMessageResponse(in_token, errorCode, respData);
      });
  msg->setCallback(&cb);
  msg->dispatch();

  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus ImsRadioAidlImpl::sendRttMessage(int32_t in_token,
                                                      const std::string& in_message) {
  imsRadiolog(">", "sendRttMessage: token=" + std::to_string(in_token) + " rttmsg=" +
                       ::android::internal::ToString(in_message));

  string rttStr;
  if (!in_message.empty()) {
    rttStr = in_message;
  } else {
    sendRttMessageResponse(in_token, aidlimports::ErrorCode::INVALID_PARAMETER, nullptr);
    return ndk::ScopedAStatus::ok();
  }
  std::shared_ptr<QcRilRequestImsSendRttMessage> msg =
      std::make_shared<QcRilRequestImsSendRttMessage>(getContext(in_token), rttStr);
  if (msg == nullptr) {
    sendRttMessageResponse(in_token, aidlimports::ErrorCode::NO_MEMORY, nullptr);
    return ndk::ScopedAStatus::ok();
  }
  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        sendRttMessageResponse(in_token, errorCode, nullptr);
      });
  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus ImsRadioAidlImpl::cancelModifyCall(int32_t in_token, int32_t in_callId) {
  imsRadiolog(">", "cancelModifyCall: token = " + std::to_string(in_token) + " callId = " +
                       std::to_string(in_callId));
  auto msg = std::make_shared<QcRilRequestImsCancelModifyCallMessage>(getContext(in_token));
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendCancelModifyCallResponse(in_token, aidlimports::ErrorCode::NO_MEMORY);
    return ndk::ScopedAStatus::ok();
  }

  // Set parameters
  msg->setCallId(in_callId);
  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        sendCancelModifyCallResponse(in_token, errorCode);
      });
  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus ImsRadioAidlImpl::sendSms(int32_t in_token,
                                               const aidlimports::SmsSendRequest& in_smsRequest) {
  imsRadiolog(">", "sendSms: token=" + std::to_string(in_token) + " in_smsRequest=" +
                       in_smsRequest.format + " " + ::android::internal::ToString(in_smsRequest));
  RIL_Errno ret = RIL_E_SUCCESS;
  RIL_RadioTechnologyFamily tech =
      (in_smsRequest.format == "3gpp") ? RADIO_TECH_3GPP : RADIO_TECH_3GPP2;
  auto msg = std::make_shared<RilRequestImsSendSmsMessage>(
      getContext(in_token), in_smsRequest.messageRef, tech, in_smsRequest.shallRetry);
  if (msg) {
    QCRIL_LOG_INFO("sendSms: pdu size = %d", in_smsRequest.pdu.size());
    if (tech == RADIO_TECH_3GPP) {
      std::string payload;
      if (ims::utils::convertBytesToHexString(in_smsRequest.pdu.data(), in_smsRequest.pdu.size(), payload)) {
        msg->setGsmPayload(in_smsRequest.smsc, payload);
      } else {
        ret = RIL_E_INVALID_ARGUMENTS;
      }
    } else {
      RIL_CDMA_SMS_Message cdmaMsg;
      if (ims::utils::convertPayloadToCdmaFormat(in_smsRequest.pdu.data(), in_smsRequest.pdu.size(), cdmaMsg)) {
        msg->setCdmaPayload(cdmaMsg);
      } else {
        ret = RIL_E_INVALID_ARGUMENTS;
      }
    }
  } else {
    ret = RIL_E_NO_MEMORY;
  }

  if (ret == RIL_E_SUCCESS) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        ([this, in_token, tech]([[maybe_unused]] std::shared_ptr<Message> msg,
                                Message::Callback::Status status,
                                std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          std::shared_ptr<RilSendSmsResult_t> respData = nullptr;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            respData = std::static_pointer_cast<RilSendSmsResult_t>(resp->data);
          }
          sendSmsResponse(in_token, resp->errorCode, tech, respData);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    sendSmsResponse(in_token, ret, tech, nullptr);
  }
  QCRIL_LOG_INFO("sendSms: ret = %d", ret);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus ImsRadioAidlImpl::acknowledgeSms(
    int32_t in_token, const aidlimports::AcknowledgeSmsInfo& in_smsInfo) {
  imsRadiolog(">", "acknowledgeSms: token=" + std::to_string(in_token) + " smsResult=" +
                       ::android::internal::ToString(in_smsInfo));
  if (in_smsInfo.smsDeliverStatus == aidlimports::SmsDeliverStatus::INVALID) {
    QCRIL_LOG_ERROR("Sms deliver status is invalid");
    return ndk::ScopedAStatus::ok();
  }
  // currently no respCb defined for ims sms ack
  auto msg = std::make_shared<RilRequestAckImsSmsMessage>(
      getContext(in_token), in_smsInfo.messageRef,
      ims::utils::convertAidlToRilSmsAckResult(in_smsInfo.smsDeliverStatus));
  if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        ([]([[maybe_unused]] std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
            std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          if (status == Message::Callback::Status::SUCCESS && resp) {
            QCRIL_LOG_INFO("acknowledgeSms cb, result: %d", resp->errorCode);
          }
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  }

  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus ImsRadioAidlImpl::acknowledgeSmsReport(
    int32_t in_token, const aidlimports::AcknowledgeSmsReportInfo& in_smsReportInfo) {
  imsRadiolog(">", "acknowledgeSmsReport: token=" + std::to_string(in_token) + " messageRef=" +
                       std::to_string(in_smsReportInfo.messageRef) + " smsReport=" +
                       ::android::internal::ToString(in_smsReportInfo.smsReportStatus));
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus ImsRadioAidlImpl::getSmsFormat(std::string* _aidl_return) {
  imsRadiolog(">", "getSmsFormat");
  qmi_ril_sms_format_type sms_format = QMI_RIL_SMS_FORMAT_UNKNOWN;
  auto msg = std::make_shared<QcRilSyncQuerySmsFormatMessage>();
  if (msg) {
    std::shared_ptr<qmi_ril_sms_format_type> resp;
    auto status = msg->dispatchSync(resp);
    if (status == Message::Callback::Status::SUCCESS && resp) {
      sms_format = *resp;
    }
  }

  std::string format{"unknown"};
  switch (sms_format) {
    case QMI_RIL_SMS_FORMAT_3GPP:
      format = "3gpp";
      break;
    case QMI_RIL_SMS_FORMAT_3GPP2:
      format = "3gpp2";
      break;
    default:
      format = "unknown";
      break;
  }
  imsRadiolog("<", std::string("getSmsFormat: format=") + format);
  if(_aidl_return) {
      *_aidl_return = format;
  }
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus ImsRadioAidlImpl::registerMultiIdentityLines(
    int32_t in_token, const std::vector<aidlimports::MultiIdentityLineInfo>& in_info) {
  imsRadiolog(">", "registerMultiIdentityLines: token=" + std::to_string(in_token) +
                       " info=" + ::android::internal::ToString(in_info));
  auto msg = std::make_shared<QcRilRequestImsRegisterMultiIdentityMessage>(getContext(in_token));
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendRegisterMultiIdentityLinesResponse(in_token, aidlimports::ErrorCode::NO_MEMORY, nullptr);
    return ndk::ScopedAStatus::ok();
  }

  std::vector<qcril::interfaces::MultiIdentityInfo>& out_lines = msg->getLineInfo();
  for (const auto& in_line : in_info) {
    qcril::interfaces::MultiIdentityInfo out_line;
    // from here only activation requests will be sent to modem
    ims::utils::convertToRilMultiIdentityInfo(in_line, out_line);
    out_lines.push_back(out_line);
  }

  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        sendRegisterMultiIdentityLinesResponse(in_token, errorCode, nullptr);
      });
  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus ImsRadioAidlImpl::queryVirtualLineInfo(int32_t in_token,
                                                            const std::string& in_msisdn) {
  imsRadiolog(">", "queryVirtualLineInfo: token=" + std::to_string(in_token) +
                       " msisdn=" + PII(::android::internal::ToString(in_msisdn)));
  if (in_msisdn.empty()) {
    QCRIL_LOG_ERROR("invalid input");
    sendQueryVirtualLinesResponse(in_token, aidlimports::ErrorCode::INVALID_PARAMETER, nullptr);
    return ndk::ScopedAStatus::ok();
  }
  auto msg = std::make_shared<QcRilRequestImsQueryVirtualLineInfo>(getContext(in_token), in_msisdn);
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendQueryVirtualLinesResponse(in_token, aidlimports::ErrorCode::NO_MEMORY, nullptr);
    return ndk::ScopedAStatus::ok();
  }

  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        std::shared_ptr<qcril::interfaces::VirtualLineInfo> respData = nullptr;
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
            respData = std::static_pointer_cast<qcril::interfaces::VirtualLineInfo>(resp->data);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        sendQueryVirtualLinesResponse(in_token, errorCode, respData);
      });
  msg->setCallback(&cb);
  msg->dispatch();

  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus ImsRadioAidlImpl::emergencyDial(
    int32_t in_token, const aidlimports::EmergencyDialRequest& in_dialRequest) {
  imsRadiolog(">", "emergencyDial: token = " + std::to_string(in_token) +
                       " Emergency dialRequest = " + ::android::internal::ToString(in_dialRequest));
  auto msg =
      ims::utils::makeQcRilRequestDialMessage(getContext(in_token), in_dialRequest.dialRequest);
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendDialResponse(in_token, aidlimports::ErrorCode::RIL_INTERNAL_NO_MEMORY);
    return ndk::ScopedAStatus::ok();
  }

  msg->setIsEmergency(true);
  msg->setCategories(
      ims::utils::convertToRilEmergencyServiceCategories((int32_t)in_dialRequest.categories));
  msg->setRouting(ims::utils::convertToRilEmergencyCallRoute(in_dialRequest.route));
  msg->setIsForEccTesting(in_dialRequest.isTesting);
  msg->setIsIntentionEcc(in_dialRequest.hasKnownUserIntentEmergency);

  std::vector<std::string> urns;
  for (size_t j = 0; j < in_dialRequest.urns.size(); j++) {
    urns.push_back(in_dialRequest.urns[j]);
  }

  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
          } else {
            errorCode = aidlimports::ErrorCode::RIL_INTERNAL_GENERIC_FAILURE;
          }
        }
        sendDialResponse(in_token, errorCode);
      });
  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus ImsRadioAidlImpl::callComposerDial(
    int32_t in_token, const aidlimports::CallComposerDialRequest& in_dialRequest) {
  imsRadiolog(">", "callComposerDial: token = " + std::to_string(in_token) +
                       " callComposerInfo: " + ::android::internal::ToString(in_dialRequest));
  auto msg =
      ims::utils::makeQcRilRequestDialMessage(getContext(in_token), in_dialRequest.dialRequest);
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendDialResponse(in_token, aidlimports::ErrorCode::RIL_INTERNAL_NO_MEMORY);
    return ndk::ScopedAStatus::ok();
  }

  // set call composer info
  qcril::interfaces::CallComposerInfo out_composer_info = {};
  ims::utils::convertToRilCallComposerInfo(out_composer_info, in_dialRequest.callComposerInfo);
  msg->setCallComposerInfo(out_composer_info);

  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
          } else {
            errorCode = aidlimports::ErrorCode::RIL_INTERNAL_GENERIC_FAILURE;
          }
        }
        sendDialResponse(in_token, errorCode);
      });

  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus ImsRadioAidlImpl::sendSipDtmf(int32_t in_token,
                                                   const std::string& in_requestCode) {
  imsRadiolog(">", "sendSipDtmf: token = " + std::to_string(in_token) + " requestCode = " +
                       ::android::internal::ToString(in_requestCode));

  auto msg = std::make_shared<QcRilRequestSendSipDtmfMessage>(getContext(in_token));
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendSipDtmfResponse(in_token, aidlimports::ErrorCode::NO_MEMORY);
    return ndk::ScopedAStatus::ok();
  }
  if (in_requestCode.empty()) {
    QCRIL_LOG_ERROR("Invalid parameter: requestCode");
    sendSipDtmfResponse(in_token, aidlimports::ErrorCode::INVALID_PARAMETER);
    return ndk::ScopedAStatus::ok();
  }
  msg->setSipInfo(in_requestCode);
  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        sendSipDtmfResponse(in_token, errorCode);
      });
  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus ImsRadioAidlImpl::setMediaConfiguration(int32_t in_token,
  const aidlimports::MediaConfig& in_config) {
  imsRadiolog(">", "setMediaConfiguration: token = " + std::to_string(in_token) + " config = " +
          ::android::internal::ToString(in_config));
  std::shared_ptr<QcRilRequestImsSetMediaInfoMessage> msg =
      std::make_shared<QcRilRequestImsSetMediaInfoMessage>(getContext(in_token));
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendSetMediaConfigurationResponse(in_token, aidlimports::ErrorCode::NO_MEMORY);
    return ndk::ScopedAStatus::ok();
  }
  msg->setMaxAvcCodecWidth(in_config.maxAvcCodecResolution.width);
  msg->setMaxAvcCodecHeight(in_config.maxAvcCodecResolution.height);
  msg->setMaxHevcCodecWidth(in_config.maxHevcCodecResolution.width);
  msg->setMaxHevcCodecHeight(in_config.maxHevcCodecResolution.height);
  msg->setScreenWidth(in_config.screenSize.width);
  msg->setScreenHeight(in_config.screenSize.height);
  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                      std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
          sendSetMediaConfigurationResponse(in_token, errorCode);
      });
  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus ImsRadioAidlImpl::queryMultiSimVoiceCapability(int32_t in_token) {
  imsRadiolog(">", "queryMultiSimVoiceCapability: token = " + std::to_string(in_token));

  auto msg = std::make_shared<QcRilRequestImsQueryMultiSimVoiceCapability>(getContext(in_token));
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendQueryMultiSimVoiceCapabilityResponse(in_token, aidlimports::ErrorCode::NO_MEMORY, nullptr);
    return ndk::ScopedAStatus::ok();
  }

  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode = ims::utils::convertDispatchStatusToAidlErrorCode(status);
        std::shared_ptr<qcril::interfaces::MultiSimVoiceCapabilityRespData> respData = nullptr;
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            respData = std::static_pointer_cast<qcril::interfaces::MultiSimVoiceCapabilityRespData>(
                resp->data);
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        sendQueryMultiSimVoiceCapabilityResponse(in_token, errorCode, respData);
      });
  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus ImsRadioAidlImpl::sendVosSupportStatus(int32_t in_token,
                                                            bool in_isVosSupported) {
  imsRadiolog(">", "sendVosSupportStatus: token = " + std::to_string(in_token) +
      " requestCode = " + boolToString(in_isVosSupported));

  auto msg = std::make_shared<QcRilRequestSendVosSupportStatusMessage>(getContext(in_token));
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendVosSupportStatusResponse(in_token, aidlimports::ErrorCode::NO_MEMORY);
    return ndk::ScopedAStatus::ok();
  }

  msg->setVosSupportStatus(in_isVosSupported);
  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode =
            ims::utils::convertDispatchStatusToAidlErrorCode(status);
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        sendVosSupportStatusResponse(in_token, errorCode);
      });
  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus ImsRadioAidlImpl::sendVosActionInfo(int32_t in_token,
    const aidlimports::VosActionInfo& in_vosActionInfo) {
  imsRadiolog(">", "sendVosActionInfo: token = " + std::to_string(in_token) + " requestCode = " +
      ::android::internal::ToString(in_vosActionInfo));

  auto msg = std::make_shared<QcRilRequestSendVosActionInfoMessage>(getContext(in_token));
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    sendVosActionInfoResponse(in_token, aidlimports::ErrorCode::NO_MEMORY);
    return ndk::ScopedAStatus::ok();
  }

  // set vos action info
  qcril::interfaces::VosActionInfo out_vos_action_info{};
  ims::utils::convertToRilVosActionInfo(out_vos_action_info, in_vosActionInfo);
  msg->setVosActionInfo(out_vos_action_info);
  GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, in_token](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
        aidlimports::ErrorCode errorCode =
            ims::utils::convertDispatchStatusToAidlErrorCode(status);
        if (errorCode == aidlimports::ErrorCode::SUCCESS) {
          if (resp) {
            errorCode = ims::utils::convertRilErrorToAidlImsError(resp->errorCode);
          } else {
            errorCode = aidlimports::ErrorCode::GENERIC_FAILURE;
          }
        }
        sendVosActionInfoResponse(in_token, errorCode);
      });
  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}

// Responses
void ImsRadioAidlImpl::sendRequestRegistrationChangeResponse(
    int32_t token, aidlimports::ErrorCode errorCode,
    std::shared_ptr<qcril::interfaces::BasePayload> /*data*/) {
  auto respCb = getResponseCallback();
  if (respCb) {
    imsRadiolog("<", "requestRegistrationChangeResponse: token = " + std::to_string(token) +
                         " errorCode = " + ::android::internal::ToString(errorCode));
    auto ret = respCb->requestRegistrationChangeResponse(token, errorCode);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void ImsRadioAidlImpl::sendGetRegistrationResponse(
    int32_t token, aidlimports::ErrorCode errorCode,
    std::shared_ptr<qcril::interfaces::Registration> data) {
  aidlimports::RegistrationInfo reg{ .state = aidlimports::RegState::INVALID,
                                     .errorCode = INT32_MAX,
                                     .radioTech = aidlimports::RadioTechType::INVALID };
  if (data) {
    ims::utils::convertRilRegistrationToAidlResponse(reg, *data);
  } else {
    QCRIL_LOG_INFO("data is nullptr");
  }
  auto respCb = getResponseCallback();
  if (respCb) {
    imsRadiolog("<", "getImsRegistrationStateResponse: token = " + std::to_string(token) +
                         " errorCode = " + ::android::internal::ToString(errorCode) +
                         " reg = " + hidePhoneNumber(::android::internal::ToString(reg)));
    auto ret = respCb->getImsRegistrationStateResponse(token, errorCode, reg);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
  notifyOnRegistrationChanged(reg);
}
void ImsRadioAidlImpl::sendQueryServiceStatusResponse(
    int32_t token, aidlimports::ErrorCode errorCode,
    std::shared_ptr<qcril::interfaces::ServiceStatusInfoList> data) {
  std::vector<aidlimports::ServiceStatusInfo> srvStatusList;
  if (data) {
    ims::utils::convertRilServiceStatusInfoListToAidlResponse(srvStatusList, *data);
  } else {
    QCRIL_LOG_ERROR("data is nullptr");
  }
  auto respCb = getResponseCallback();
  if (respCb) {
    imsRadiolog("<", "queryServiceStatusResponse: token = " + std::to_string(token) +
                         " errorCode = " + ::android::internal::ToString(errorCode) +
                         " srvStatusList = " + ::android::internal::ToString(srvStatusList));
    auto ret = respCb->queryServiceStatusResponse(token, errorCode, srvStatusList);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
  notifyOnServiceStatusChanged(srvStatusList);
}
void ImsRadioAidlImpl::sendDialResponse(int32_t token, aidlimports::ErrorCode errorCode) {
  auto respCb = this->getResponseCallback();
  if (respCb) {
    imsRadiolog("<", "dialResponse: token = " + std::to_string(token) +
                         " errorCode = " + ::android::internal::ToString(errorCode));
    auto ret = respCb->dialResponse(token, errorCode);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void ImsRadioAidlImpl::sendAnswerResponse(int32_t token, aidlimports::ErrorCode errorCode) {
  auto respCb = getResponseCallback();
  if (respCb) {
    imsRadiolog("<", "answerResponse: token = " + std::to_string(token) +
                         " errorCode = " + ::android::internal::ToString(errorCode));
    auto ret = respCb->answerResponse(token, errorCode);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void ImsRadioAidlImpl::sendHangupResponse(int32_t token, aidlimports::ErrorCode errorCode) {
  auto respCb = getResponseCallback();
  if (respCb) {
    imsRadiolog("<", "hangupResponse: token = " + std::to_string(token) +
                         " errorCode = " + ::android::internal::ToString(errorCode));
    auto ret = respCb->hangupResponse(token, errorCode);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void ImsRadioAidlImpl::sendHoldResponse(int32_t token, aidlimports::ErrorCode errorCode,
                                        std::shared_ptr<qcril::interfaces::SipErrorInfo> data) {
  aidlimports::SipErrorInfo errorDetails{ .errorCode = INT32_MAX };
  if (data) {
    ims::utils::convertToAidlSipErrorInfo(errorDetails, data);
  }
  auto respCb = getResponseCallback();
  if (respCb) {
    imsRadiolog("<", "holdResponse: token = " + std::to_string(token) +
                         " errorCode = " + ::android::internal::ToString(errorCode) +
                         " errorDetails = " + ::android::internal::ToString(errorDetails));
    auto ret = respCb->holdResponse(token, errorCode, errorDetails);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void ImsRadioAidlImpl::sendResumeResponse(int32_t token, aidlimports::ErrorCode errorCode,
                                          std::shared_ptr<qcril::interfaces::SipErrorInfo> data) {
  aidlimports::SipErrorInfo errorDetails{ .errorCode = INT32_MAX };
  if (data) {
    ims::utils::convertToAidlSipErrorInfo(errorDetails, data);
  }
  auto respCb = getResponseCallback();
  if (respCb) {
    imsRadiolog("<", "resumeResponse: token = " + std::to_string(token) +
                         " errorCode = " + ::android::internal::ToString(errorCode) +
                         " errorDetails = " + ::android::internal::ToString(errorDetails));
    auto ret = respCb->resumeResponse(token, errorCode, errorDetails);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void ImsRadioAidlImpl::sendQueryCallForwardStatusResponse(
    int32_t token, aidlimports::ErrorCode errorCode,
    const std::shared_ptr<qcril::interfaces::GetCallForwardRespData> data) {
  std::vector<aidlimports::CallForwardInfo> outCfInfoList;
  aidlimports::SipErrorInfo errorDetails{.errorCode = INT32_MAX };

  if (data) {
    if (!data->getCallForwardInfo().empty()) {
      ims::utils::convertToAidlCallForwardInfoList(outCfInfoList, data->getCallForwardInfo());
    }
    if (data->hasErrorDetails()) {
      ims::utils::convertToAidlSipErrorInfo(errorDetails, data->getErrorDetails());
    }
  }

  auto respCb = getResponseCallback();
  if (respCb) {
    imsRadiolog("<", "queryCallForwardStatusResponse: token = " + std::to_string(token) +
                         " errorCode = " + ::android::internal::ToString(errorCode) +
                         " outCfInfoList = " + hidePhoneNumber(android::internal::ToString(outCfInfoList)) +
                         " errorDetails = " + android::internal::ToString(errorDetails));
    auto ret = respCb->queryCallForwardStatusResponse(token, errorCode, outCfInfoList, errorDetails);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void ImsRadioAidlImpl::sendGetCallWaitingResponse(
    int32_t token, aidlimports::ErrorCode errorCode,
    const std::shared_ptr<qcril::interfaces::CallWaitingInfo> data) {
  aidlimports::SipErrorInfo errorDetails{.errorCode = INT32_MAX };
  aidlimports::CallWaitingInfo callWaitingInfo{
    .serviceStatus = aidlimports::ServiceClassStatus::INVALID, .serviceClass = INT32_MAX
  };

  if (data) {
    if (data->hasStatus()) {
      callWaitingInfo.serviceStatus = ims::utils::convertToAidlServiceClassStatus(data->getStatus());
    }
    if (data->hasErrorDetails()) {
      ims::utils::convertToAidlSipErrorInfo(errorDetails, data->getErrorDetails());
    }
    if (data->hasServiceClass()) {
      callWaitingInfo.serviceClass = data->getServiceClass();
    }
  }

  auto respCb = getResponseCallback();
  if (respCb) {
    imsRadiolog("<", "getCallWaitingResponse: token = " + std::to_string(token) + " errorCode = " +
                         ::android::internal::ToString(errorCode) + " status = " +
                         android::internal::ToString(callWaitingInfo.serviceStatus) +
                         " serviceClass = " + std::to_string(callWaitingInfo.serviceClass) +
                         " errorDetails = " + android::internal::ToString(errorDetails));
    auto ret = respCb->getCallWaitingResponse(token, errorCode, callWaitingInfo, errorDetails);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void ImsRadioAidlImpl::sendConferenceResponse(
    int32_t token, aidlimports::ErrorCode errorCode,
    const std::shared_ptr<qcril::interfaces::SipErrorInfo> data) {
  aidlimports::SipErrorInfo errorInfo{ .errorCode = INT32_MAX };
  if (data) {
    ims::utils::convertToAidlSipErrorInfo(errorInfo, data);
  }
  auto respCb = this->getResponseCallback();
  if (respCb) {
    imsRadiolog("<", "conferenceResponse: token = " + std::to_string(token) +
                         " errorCode = " + ::android::internal::ToString(errorCode) +
                         " errorInfo = " + ::android::internal::ToString(errorInfo));
    auto ret = respCb->conferenceResponse(token, errorCode, errorInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void ImsRadioAidlImpl::sendGetClipResponse(int32_t token, aidlimports::ErrorCode errorCode,
                                           const std::shared_ptr<qcril::interfaces::ClipInfo> data) {
  aidlimports::ClipProvisionStatus clipProvisionStatus{};
  bool hasClipInfo = false;
  if (data) {
    hasClipInfo = ims::utils::convertToAidlClipInfo(clipProvisionStatus, data);
  }
  auto respCb = getResponseCallback();
  if (respCb) {
    imsRadiolog("<", "getClipResponse: token = " + std::to_string(token) + " errorCode = " +
                         ::android::internal::ToString(errorCode) + " clipProvisionStatus = " +
                         android::internal::ToString(clipProvisionStatus));
    auto ret = respCb->getClipResponse(token, errorCode, clipProvisionStatus);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void ImsRadioAidlImpl::sendGetClirResponse(int32_t token, aidlimports::ErrorCode errorCode,
                                           const std::shared_ptr<qcril::interfaces::ClirInfo> data) {
  aidlimports::ClirInfo clirInfo{};
  bool hasClirInfo = false;
  if (data) {
    hasClirInfo = ims::utils::convertToAidlClirInfo(clirInfo, data);
  }
  auto respCb = getResponseCallback();
  if (respCb) {
    imsRadiolog("<", "getClirResponse: token = " + std::to_string(token) + " errorCode = " +
                         ::android::internal::ToString(errorCode) + " clirInfo = " +
                         android::internal::ToString(clirInfo) + " hasClirInfo = " +
                         std::to_string(hasClirInfo));
    auto ret = respCb->getClirResponse(token, errorCode, clirInfo, hasClirInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void ImsRadioAidlImpl::sendSetClirResponse(int32_t token, aidlimports::ErrorCode errorCode) {
  auto respCb = getResponseCallback();
  if (respCb) {
    imsRadiolog("<", "setClirResponse: token = " + std::to_string(token) + " errorCode = " +
                         ::android::internal::ToString(errorCode));
    auto ret = respCb->setClirResponse(token, errorCode);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void ImsRadioAidlImpl::sendGetColrResponse(int32_t token, aidlimports::ErrorCode errorCode,
                                           const std::shared_ptr<qcril::interfaces::ColrInfo> data) {
  auto respCb = getResponseCallback();
  if (respCb) {
    aidlimports::ColrInfo colrInfo{};
    if (data) {
      ims::utils::convertToAidlColrInfo(colrInfo, data);
    }
    imsRadiolog("<", "getColrResponse: token = " + std::to_string(token) +
                         " errorCode = " + ::android::internal::ToString(errorCode) +
                         " colrInfo = " + ::android::internal::ToString(colrInfo));
    auto ret = respCb->getColrResponse(token, errorCode, colrInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void ImsRadioAidlImpl::sendDtmfResponse(int32_t token, aidlimports::ErrorCode errorCode) {
  auto respCb = getResponseCallback();
  if (respCb) {
    imsRadiolog("<", "sendDtmfResponse: token = " + std::to_string(token) + " errorCode = " +
                         ::android::internal::ToString(errorCode));
    auto ret = respCb->sendDtmfResponse(token, errorCode);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void ImsRadioAidlImpl::sendStartDtmfResponse(int32_t token, aidlimports::ErrorCode errorCode) {
  auto respCb = getResponseCallback();
  if (respCb) {
    imsRadiolog("<", "startDtmfResponse: token = " + std::to_string(token) + " errorCode = " +
                         ::android::internal::ToString(errorCode));
    auto ret = respCb->startDtmfResponse(token, errorCode);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void ImsRadioAidlImpl::sendStopDtmfResponse(int32_t token, aidlimports::ErrorCode errorCode) {
  auto respCb = getResponseCallback();
  if (respCb) {
    imsRadiolog("<", "stopDtmfResponse: token = " + std::to_string(token) + " errorCode = " +
                         ::android::internal::ToString(errorCode));
    auto ret = respCb->stopDtmfResponse(token, errorCode);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void ImsRadioAidlImpl::sendExitEmergencyCallbackModeResponse(int32_t token,
                                                             aidlimports::ErrorCode errorCode) {
  auto respCb = getResponseCallback();
  if (respCb) {
    imsRadiolog("<", "exitEmergencyCallbackModeResponse: token = " + std::to_string(token) +
                         " errorCode = " + ::android::internal::ToString(errorCode));
    auto ret = respCb->exitEmergencyCallbackModeResponse(token, errorCode);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void ImsRadioAidlImpl::sendSetUiTTYModeResponse(int32_t token, aidlimports::ErrorCode errorCode) {
  auto respCb = getResponseCallback();
  if (respCb) {
    imsRadiolog("<", "setUiTTYModeResponse: token = " + std::to_string(token) + " errorCode = " +
                         ::android::internal::ToString(errorCode));
    auto ret = respCb->setUiTTYModeResponse(token, errorCode);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void ImsRadioAidlImpl::sendExplicitCallTransferResponse(int32_t token,
                                                        aidlimports::ErrorCode errorCode) {
  aidlimports::SipErrorInfo errorDetails{.errorCode = INT32_MAX };
  auto respCb = getResponseCallback();
  if (respCb) {
    imsRadiolog("<", "explicitCallTransferResponse: token = " + std::to_string(token) +
                         " errorCode = " + ::android::internal::ToString(errorCode));
    auto ret = respCb->explicitCallTransferResponse(token, errorCode, errorDetails);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void ImsRadioAidlImpl::sendModifyCallInitiateResponse(int32_t token,
                                                      aidlimports::ErrorCode errorCode) {
  auto respCb = getResponseCallback();
  if (respCb) {
    imsRadiolog("<", "modifyCallInitiateResponse: token = " + std::to_string(token) +
                         " errorCode = " + ::android::internal::ToString(errorCode));
    auto ret = respCb->modifyCallInitiateResponse(token, errorCode);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void ImsRadioAidlImpl::sendCancelModifyCallResponse(int32_t token,
                                                    aidlimports::ErrorCode errorCode) {
  auto respCb = getResponseCallback();
  if (respCb) {
    imsRadiolog("<", "cancelModifyCallResponse: token = " + std::to_string(token) +
                         " errorCode = " + ::android::internal::ToString(errorCode));
    auto ret = respCb->cancelModifyCallResponse(token, errorCode);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void ImsRadioAidlImpl::sendModifyCallConfirmResponse(int32_t token,
                                                     aidlimports::ErrorCode errorCode) {
  auto respCb = getResponseCallback();
  if (respCb) {
    imsRadiolog("<", "modifyCallConfirmResponse: token = " + std::to_string(token) +
                         " errorCode = " + ::android::internal::ToString(errorCode));
    auto ret = respCb->modifyCallConfirmResponse(token, errorCode);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void ImsRadioAidlImpl::sendAddParticipantResponse(int32_t token, aidlimports::ErrorCode errorCode) {
  auto respCb = getResponseCallback();
  if (respCb) {
    imsRadiolog("<", "addParticipantResponse: token = " + std::to_string(token) +
                         " errorCode = " + ::android::internal::ToString(errorCode));
    auto ret = respCb->addParticipantResponse(token, errorCode);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void ImsRadioAidlImpl::sendDeflectCallResponse(int32_t token, aidlimports::ErrorCode errorCode) {
  auto respCb = getResponseCallback();
  if (respCb) {
    imsRadiolog("<", "deflectCallResponse: token = " + std::to_string(token) + " errorCode = " +
                         ::android::internal::ToString(errorCode));
    auto ret = respCb->deflectCallResponse(token, errorCode);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void ImsRadioAidlImpl::sendSetSuppServiceNotificationResponse(int32_t token,
                                                              aidlimports::ErrorCode errorCode) {
  aidlimports::ServiceClassStatus status = aidlimports::ServiceClassStatus::INVALID;
  auto respCb = getResponseCallback();
  if (respCb) {
    imsRadiolog("<", "setSuppServiceNotificationResponse: token = " + std::to_string(token) +
                         " errorCode = " + ::android::internal::ToString(errorCode) + " status = " +
                         android::internal::ToString(status));
    auto ret = respCb->setSuppServiceNotificationResponse(token, errorCode, status);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void ImsRadioAidlImpl::sendGetRtpStatsMessageResponse(
    int32_t token, aidlimports::ErrorCode errorCode,
    std::shared_ptr<qcril::interfaces::RtpStatisticsData> data) {
  uint64_t count = 0;
  if (data) {
    count = data->getCount();
  }
  auto respCb = getResponseCallback();
  if (respCb) {
    imsRadiolog("<", "getRtpStatisticsResponse: token = " + std::to_string(token) +
                         " errorCode = " + ::android::internal::ToString(errorCode) + " count = " +
                         std::to_string(count));
    auto ret = respCb->getRtpStatisticsResponse(token, errorCode, count);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void ImsRadioAidlImpl::sendGetRtpErrorStatsMessageResponse(
    int32_t token, aidlimports::ErrorCode errorCode,
    std::shared_ptr<qcril::interfaces::RtpStatisticsData> data) {
  uint64_t count = 0;
  if (data) {
    count = data->getCount();
  }
  auto respCb = getResponseCallback();
  if (respCb) {
    imsRadiolog("<", "getRtpErrorStatisticsResponse: token = " + std::to_string(token) +
                         " errorCode = " + ::android::internal::ToString(errorCode) + " count = " +
                         std::to_string(count));
    auto ret = respCb->getRtpErrorStatisticsResponse(token, errorCode, count);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void ImsRadioAidlImpl::sendGetImsSubConfigMessageResponse(
    int32_t token, aidlimports::ErrorCode errorCode,
    std::shared_ptr<qcril::interfaces::ImsSubConfigInfo> data) {
  auto respCb = getResponseCallback();
  aidlimports::ImsSubConfigInfo subConfigInfo{};
  if (data) {
    ims::utils::convertRilImsSubConfigInfoToAidlResponse(subConfigInfo, *data);
  }
  if (respCb != nullptr) {
    imsRadiolog("<", "getImsSubConfigResponse: token=" + std::to_string(token) +
                         " errorCode=" + ::android::internal::ToString(errorCode) +
                         " subConfigInfo=" + ::android::internal::ToString(subConfigInfo));
    auto ret = respCb->getImsSubConfigResponse(token, errorCode, subConfigInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
  notifyOnImsSubConfigChanged(subConfigInfo);
}

void ImsRadioAidlImpl::sendSmsResponse(int32_t token, RIL_Errno errorCode,
                                       RIL_RadioTechnologyFamily tech,
                                       std::shared_ptr<RilSendSmsResult_t> data) {
  auto respCb = getResponseCallback();
  if (respCb != nullptr) {
    int32 messageRef = -1;
    int32 networkErrorCode = -1;
    int32 transportErrorCode = -1;
    std::optional<bool> bLteOnlyReg;
    aidlimports::SmsSendFailureReason failureReason =
        aidlimports::SmsSendFailureReason::GENERIC_FAILURE;
    aidlimports::SmsSendStatus status = aidlimports::SmsSendStatus::ERROR;
    aidlimports::RadioTechType radioTech = aidlimports::RadioTechType::UNKNOWN;

    if (data) {
      messageRef = data->messageRef;
      networkErrorCode = data->rpCause;
      transportErrorCode = data->causeCode;
      bLteOnlyReg = data->bLteOnlyReg;
      radioTech = ims::utils::convertToAidlRadioTech(data->rat);
    }
    ims::utils::convertRilImsResponseToAidlResponse(errorCode, tech, bLteOnlyReg, status,
                                                    failureReason);
    imsRadiolog("<", "sendSmsResponse: token=" + std::to_string(token) + " msgeRef=" +
                         std::to_string(messageRef) + " status=" + toString(status) + " reason=" +
                         toString(failureReason) + " networkErrorCode=" +
                         std::to_string(networkErrorCode) + " transportErrorCode=" +
                         std::to_string(transportErrorCode) + " radioTech=" + toString(radioTech));
    aidlimports::SmsSendResponse response =
        aidlimports::SmsSendResponse{.msgRef = messageRef,
                                     .smsStatus = status,
                                     .reason = failureReason,
                                     .networkErrorCode = networkErrorCode,
                                     .transportErrorCode = transportErrorCode,
                                     .radioTech = radioTech };
    auto retStatus = respCb->sendSmsResponse(token, response);
    if (!retStatus.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", retStatus.getDescription().c_str());
    }
  }
}

void ImsRadioAidlImpl::sendRttMessageResponse(int32_t token, aidlimports::ErrorCode errorCode,
                                              std::shared_ptr<qcril::interfaces::BasePayload> /*data*/) {
  auto respCb = getResponseCallback();
  if (respCb) {
    imsRadiolog("<", "sendRttMessageResponse: token = " + std::to_string(token) + " errorCode = " +
                         ::android::internal::ToString(errorCode));
    auto retStatus = respCb->sendRttMessageResponse(token, errorCode);
    if (!retStatus.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", retStatus.getDescription().c_str());
    }
  }
}
void ImsRadioAidlImpl::sendSetServiceStatusMessageResponse(
    int32_t token, aidlimports::ErrorCode errorCode,
    std::shared_ptr<qcril::interfaces::BasePayload> /*data*/) {
  auto respCb = getResponseCallback();
  if (respCb) {
    imsRadiolog("<", "setServiceStatusResponse: token = " + std::to_string(token) +
                         " errorCode = " + ::android::internal::ToString(errorCode));
    auto retStatus = respCb->setServiceStatusResponse(token, errorCode);
    if (!retStatus.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", retStatus.getDescription().c_str());
    }
  }
}
void ImsRadioAidlImpl::sendGeoLocationResponse(int32_t token, aidlimports::ErrorCode errorCode,
                                               std::shared_ptr<qcril::interfaces::BasePayload> /*data*/) {
  auto respCb = getResponseCallback();
  if (respCb) {
    imsRadiolog("<", "sendGeolocationInfoResponse: token = " + std::to_string(token) +
                         " errorCode = " + ::android::internal::ToString(errorCode));
    auto retStatus = respCb->sendGeolocationInfoResponse(token, errorCode);
    if (!retStatus.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", retStatus.getDescription().c_str());
    }
  }
}
void ImsRadioAidlImpl::sendGetConfigMessageResponse(
    int32_t token, aidlimports::ErrorCode errorCode,
    std::shared_ptr<qcril::interfaces::ConfigInfo> data) {
  aidlimports::ConfigInfo config{.item = aidlimports::ConfigItem::INVALID,
                                 .intValue = INT32_MAX,
                                 .errorCause = aidlimports::ConfigFailureCause::INVALID };
  if (data) {
    ims::utils::convertToAidlConfigInfo(config, (*data));
  }
  auto respCb = getResponseCallback();
  if (respCb) {
    imsRadiolog("<", "getConfigResponse: token = " + std::to_string(token) + " errorCode = " +
                         ::android::internal::ToString(errorCode) + " config = " +
                         android::internal::ToString(config));
    auto retStatus = respCb->getConfigResponse(token, errorCode, config);
    if (!retStatus.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", retStatus.getDescription().c_str());
    }
  }
}
void ImsRadioAidlImpl::sendSetConfigMessageResponse(
    int32_t token, aidlimports::ErrorCode errorCode,
    std::shared_ptr<qcril::interfaces::ConfigInfo> data) {
  aidlimports::ConfigInfo config{.item = aidlimports::ConfigItem::INVALID,
                                 .intValue = INT32_MAX,
                                 .errorCause = aidlimports::ConfigFailureCause::INVALID };
  if (data) {
    ims::utils::convertToAidlConfigInfo(config, *(data));
  }
  auto respCb = getResponseCallback();
  if (respCb) {
    imsRadiolog("<", "setConfigResponse: token = " + std::to_string(token) + " errorCode = " +
                         ::android::internal::ToString(errorCode) + " config = " +
                         android::internal::ToString(config));
    auto retStatus = respCb->setConfigResponse(token, errorCode, config);
    if (!retStatus.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", retStatus.getDescription().c_str());
    }
  }
}
void ImsRadioAidlImpl::sendSuppServiceStatusResponse(
    int32_t token, aidlimports::ErrorCode errorCode,
    const std::shared_ptr<qcril::interfaces::SuppServiceStatusInfo> data) {
  aidlimports::SuppServiceStatus suppServiceStatus{
    .status = aidlimports::ServiceClassStatus::INVALID,
    .provisionStatus = aidlimports::ServiceClassProvisionStatus::INVALID,
    .facilityType = aidlimports::FacilityType::INVALID,
    .hasErrorDetails = false,
    .isPasswordRequired = false
  };

  if (data) {
    ims::utils::convertToAidlSuppServiceStatus(suppServiceStatus, *data);
  }
  auto respCb = getResponseCallback();
  if (respCb) {
    imsRadiolog("<", "suppServiceStatusResponse: token = " + std::to_string(token) +
                         " errorCode = " + ::android::internal::ToString(errorCode) +
                         " suppServiceStatus = " + android::internal::ToString(suppServiceStatus) +
                         " isPasswordRequired = " + (suppServiceStatus.isPasswordRequired ?
                                                    "true":"false"));
    auto retStatus = respCb->suppServiceStatusResponse(token, errorCode, suppServiceStatus);
    if (!retStatus.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", retStatus.getDescription().c_str());
    }
  }
}
void ImsRadioAidlImpl::sendRegisterMultiIdentityLinesResponse(
    int32_t token, aidlimports::ErrorCode errorCode,
    std::shared_ptr<qcril::interfaces::BasePayload> /*data*/) {
  auto respCb = getResponseCallback();
  if (respCb) {
    imsRadiolog("<", "registerMultiIdentityLinesResponse: token=" + std::to_string(token) +
                         " errorCode=" + ::android::internal::ToString(errorCode));
    auto retStatus = respCb->registerMultiIdentityLinesResponse(token, errorCode);
    if (!retStatus.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", retStatus.getDescription().c_str());
    }
  }
}
void ImsRadioAidlImpl::sendQueryVirtualLinesResponse(
    int32_t token, aidlimports::ErrorCode /*errorCode*/,
    std::shared_ptr<qcril::interfaces::VirtualLineInfo> data) {
  std::string msisdn{};
  std::vector<std::string> lines{};
  if (data) {
    msisdn = data->getMsisdn();
    ims::utils::convertToAidlVirtualLines(lines, data->getVirtualLines());
  }

  auto respCb = this->getResponseCallback();
  if (respCb) {
    imsRadiolog("<", "sendQueryVirtualLinesResponse: token=" + std::to_string(token) +
                         " msisdn=" + PII(::android::internal::ToString(msisdn)) +
                         " lines= " + ::android::internal::ToString(lines));
    auto retStatus = respCb->queryVirtualLineInfoResponse(token, msisdn, lines);
    if (!retStatus.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", retStatus.getDescription().c_str());
    }
  }
}
void ImsRadioAidlImpl::sendUssdResponse(int32_t token, aidlimports::ErrorCode errorCode,
                                        std::shared_ptr<qcril::interfaces::SipErrorInfo> data) {
  aidlimports::SipErrorInfo errorDetails = { .errorCode = INT32_MAX };
  if (data) {
    ims::utils::convertToAidlSipErrorInfo(errorDetails, data, false);
  }

  auto respCb = this->getResponseCallback();
  if (respCb) {
    imsRadiolog("<", "sendUssdResponse: token = " + std::to_string(token) +
                         " errorCode = " + ::android::internal::ToString(errorCode) +
                         " errorDetails = " + ::android::internal::ToString(errorDetails));
    auto ret = respCb->sendUssdResponse(token, errorCode, errorDetails);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void ImsRadioAidlImpl::cancelPendingUssdResponse(
    int32_t token, aidlimports::ErrorCode errorCode,
    std::shared_ptr<qcril::interfaces::SipErrorInfo> data) {
  aidlimports::SipErrorInfo errorDetails = { .errorCode = INT32_MAX };
  if (data) {
    ims::utils::convertToAidlSipErrorInfo(errorDetails, data, false);
  }

  auto respCb = this->getResponseCallback();
  if (respCb) {
    imsRadiolog("<", "cancelPendingUssdResponse: token = " + std::to_string(token) +
                         " errorCode = " + ::android::internal::ToString(errorCode) +
                         " errorDetails = " + ::android::internal::ToString(errorDetails));
    auto ret = respCb->cancelPendingUssdResponse(token, errorCode, errorDetails);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void ImsRadioAidlImpl::sendSipDtmfResponse(int32_t token, aidlimports::ErrorCode errorCode) {
  auto respCb = getResponseCallback();
  if (!respCb) {
    QCRIL_LOG_ERROR("respCb is null");
    return;
  }
  imsRadiolog("<", "sendSipDtmfResponse: token = " + std::to_string(token) + " errorCode = " +
                       ::android::internal::ToString(errorCode));
  auto ret = respCb->sendSipDtmfResponse(token, errorCode);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}
void ImsRadioAidlImpl::sendSetCallForwardStatusResponse(
    int32_t token, aidlimports::ErrorCode errorCode,
    const std::shared_ptr<qcril::interfaces::SetCallForwardRespData> data) {
  aidlimports::CallForwardStatusInfo callForwardStatus = {};
  ims::utils::convertToAidlCallForwardStatusInfo(callForwardStatus, data);
  auto respCb = getResponseCallback();
  if (respCb) {
    imsRadiolog("<", "setCallForwardStatusResponse: token = " + std::to_string(token) +
                         " errorCode = " + ::android::internal::ToString(errorCode) +
                         " callForwardStatus = " + android::internal::ToString(callForwardStatus));
    auto ret = respCb->setCallForwardStatusResponse(token, errorCode, callForwardStatus);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void ImsRadioAidlImpl::sendSetMediaConfigurationResponse(
    int32_t token, aidlimports::ErrorCode errorCode) {
  auto respCb = getResponseCallback();
  if (respCb) {
    imsRadiolog("<", "setMediaConfigurationResponse: token = " + std::to_string(token) +
                         " errorCode = " + ::android::internal::ToString(errorCode));
    auto ret = respCb->setMediaConfigurationResponse(token, errorCode);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void ImsRadioAidlImpl::sendQueryMultiSimVoiceCapabilityResponse(
    int32_t token, aidlimports::ErrorCode errorCode,
    std::shared_ptr<qcril::interfaces::MultiSimVoiceCapabilityRespData> data) {
  auto respCb = getResponseCallback();
  aidlimports::MultiSimVoiceCapability voiceCapability =
      aidlimports::MultiSimVoiceCapability::NONE;
  if (data) {
    voiceCapability = ims::utils::convertToAidlMultiSimVoiceCapability(data->getVoiceCapability());
  }
  if (respCb) {
    imsRadiolog("<", "queryMultiSimVoiceCapabilityResponse: token=" + std::to_string(token) +
                         " errorCode=" + ::android::internal::ToString(errorCode) +
                         " voiceCapability=" + ::android::internal::ToString(voiceCapability));
    auto ret = respCb->queryMultiSimVoiceCapabilityResponse(token, errorCode, voiceCapability);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
  notifyOnMultiSimVoiceCapabilityChanged(voiceCapability);
}

void ImsRadioAidlImpl::sendExitSmsCallBackModeResponse(int32_t token,
                                                             aidlimports::ErrorCode errorCode) {
  auto respCb = getResponseCallback();
  if (respCb) {
    imsRadiolog("<", "exitSmsCallBackModeResponse: token = " + std::to_string(token) +
                         " errorCode = " + ::android::internal::ToString(errorCode));
    auto ret = respCb->exitSmsCallBackModeResponse(token, errorCode);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void ImsRadioAidlImpl::sendVosSupportStatusResponse(int32_t token,
                                                    aidlimports::ErrorCode errorCode) {
  auto respCb = getResponseCallback();
  if (!respCb) {
    QCRIL_LOG_ERROR("respCb is null");
    return;
  }
  imsRadiolog("<", "sendVosSupportStatusResponse: token = " + std::to_string(token) +
      " errorCode = " + ::android::internal::ToString(errorCode));
  auto ret = respCb->sendVosSupportStatusResponse(token, errorCode);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}
void ImsRadioAidlImpl::sendVosActionInfoResponse(int32_t token, aidlimports::ErrorCode errorCode) {
  auto respCb = getResponseCallback();
  if (!respCb) {
    QCRIL_LOG_ERROR("respCb is null");
    return;
  }
  imsRadiolog("<", "sendVosActionInfoResponse: token = " + std::to_string(token) +
      " errorCode = " + ::android::internal::ToString(errorCode));
  auto ret = respCb->sendVosActionInfoResponse(token, errorCode);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

// Utility functions
void ImsRadioAidlImpl::notifyOnMultiSimVoiceCapabilityChanged(
    const aidlimports::MultiSimVoiceCapability capability) {
  auto indCb = getIndicationCallback();
  if (!indCb) {
    QCRIL_LOG_ERROR("indCb is null");
    return;
  }
  imsRadiolog("<", "notifyOnMultiSimVoiceCapabilityChanged: capability = " +
                       ::android::internal::ToString(capability));
  auto ret = indCb->onMultiSimVoiceCapabilityChanged(capability);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
  }
}
void ImsRadioAidlImpl::notifyOnServiceStatusChanged(
    const std::vector<aidlimports::ServiceStatusInfo> srvStatusList) {
  auto indCb = getIndicationCallback();
  if (!indCb) {
    QCRIL_LOG_ERROR("indCb is null");
    return;
  }
  imsRadiolog("<", "notifyOnServiceStatusChanged: srvStatusList= " +
                       ::android::internal::ToString(srvStatusList));
  auto ret = indCb->onServiceStatusChanged(srvStatusList);

  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
  }
}
void ImsRadioAidlImpl::notifyOnRegistrationChanged(const aidlimports::RegistrationInfo regInfo) {
  auto indCb = getIndicationCallback();
  if (!indCb) {
    QCRIL_LOG_ERROR("indCb is null");
    return;
  }

  imsRadiolog("<", "notifyOnRegistrationChanged: reg = " +
      hidePhoneNumber(::android::internal::ToString(regInfo)));
  auto ret = indCb->onRegistrationChanged(regInfo);

  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
  }
}
void ImsRadioAidlImpl::notifyOnImsSubConfigChanged(const aidlimports::ImsSubConfigInfo config) {
  auto indCb = getIndicationCallback();
  if (!indCb) {
    QCRIL_LOG_ERROR("indCb is null");
    return;
  }
  imsRadiolog("<", "notifyOnImsSubConfigChanged: status= " + ::android::internal::ToString(config));
  auto ret = indCb->onImsSubConfigChanged(config);

  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
  }
}
}  // namespace implementation
}  // namespace ims
}  // namespace radio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
