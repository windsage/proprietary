/******************************************************************************
  @file    voice_aidl_service.cpp
  @brief   voice_aidl_service

  DESCRIPTION
    Implements the server side of the IRadioVoice interface. Handles RIL
    requests and responses to be received and sent to client respectively

  ---------------------------------------------------------------------------
  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#define TAG "RILQ"

#include "voice_aidl_service.h"
#include <framework/Log.h>
#include "wake_lock_utils.h"
#include "voice_aidl_service_utils.h"

#include "interfaces/gstk/GstkSendSetupCallResponseRequestMsg.h"
#include <interfaces/nas/RilRequestGetVonrCapRequest.h>
#include <interfaces/nas/RilRequestSetVonrCapRequest.h>
#include <interfaces/nas/RilRequestExitEmergencyCallbackMessage.h>
#include <interfaces/voice/QcRilRequestCancelUssdMessage.h>
#include <interfaces/voice/QcRilRequestCdmaBurstDtmfMessage.h>
#include <interfaces/voice/QcRilRequestCdmaFlashMessage.h>
#include <interfaces/voice/QcRilRequestConferenceMessage.h>
#include <interfaces/voice/QcRilRequestAnswerMessage.h>
#include <interfaces/voice/QcRilRequestDialMessage.h>
#include <interfaces/voice/QcRilRequestHangupMessage.h>
#include <interfaces/voice/QcRilRequestExplicitCallTransferMessage.h>
#include <interfaces/voice/QcRilRequestGetCallWaitingMessage.h>
#include <interfaces/voice/QcRilRequestGetClipMessage.h>
#include <interfaces/voice/QcRilRequestGetClirMessage.h>
#include <interfaces/voice/QcRilRequestGetCurrentCallsMessage.h>
#include <interfaces/voice/QcRilRequestGetMuteMessage.h>
#include <interfaces/voice/QcRilRequestGetPreferredVoicePrivacyMessage.h>
#include <interfaces/voice/QcRilRequestGetTtyModeMessage.h>
#include <interfaces/voice/QcRilRequestHangupForegroundResumeBackgroundMessage.h>
#include <interfaces/voice/QcRilRequestHangupWaitingOrBackgroundMessage.h>
#include <interfaces/voice/QcRilRequestLastCallFailCauseMessage.h>
#include <interfaces/voice/QcRilRequestQueryCallForwardMessage.h>
#include <interfaces/voice/QcRilRequestSendDtmfMessage.h>
#include <interfaces/voice/QcRilRequestSendUssdMessage.h>
#include <interfaces/voice/QcRilRequestSeparateConnectionMessage.h>
#include <interfaces/voice/QcRilRequestSetCallForwardMessage.h>
#include <interfaces/voice/QcRilRequestSetCallWaitingMessage.h>
#include <interfaces/voice/QcRilRequestSetClirMessage.h>
#include <interfaces/voice/QcRilRequestSetMuteMessage.h>
#include <interfaces/voice/QcRilRequestSetPreferredVoicePrivacyMessage.h>
#include <interfaces/voice/QcRilRequestSetTtyModeMessage.h>
#include <interfaces/voice/QcRilRequestStartDtmfMessage.h>
#include <interfaces/voice/QcRilRequestStopDtmfMessage.h>
#include <interfaces/voice/QcRilRequestSwitchWaitingOrHoldingAndActiveMessage.h>
#include <interfaces/voice/QcRilRequestUdubMessage.h>
#include <interfaces/voice/QcrilVoiceClientConnected.h>

std::shared_ptr<aidlvoice::IRadioVoiceResponse> IRadioVoiceImpl::getResponseCallback() {
  std::shared_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
  return mIRadioVoiceResponse;
}

std::shared_ptr<aidlvoice::IRadioVoiceIndication> IRadioVoiceImpl::getIndicationCallback() {
  std::shared_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
  return mIRadioVoiceIndication;
}

void IRadioVoiceImpl::notifyVoiceClientConnected() {
  auto msg = std::make_shared<QcrilVoiceClientConnected>();
  if (msg != nullptr) {
    msg->broadcast();
  }
}

void IRadioVoiceImpl::setResponseFunctions_nolock(
    const std::shared_ptr<aidlvoice::IRadioVoiceResponse>& radioVoiceResponse,
    const std::shared_ptr<aidlvoice::IRadioVoiceIndication>& radioVoiceIndication) {
  mIRadioVoiceResponse = radioVoiceResponse;
  mIRadioVoiceIndication = radioVoiceIndication;
}

void IRadioVoiceImpl::deathNotifier() {
  QCRIL_LOG_DEBUG("IRadioVoiceImpl: Client died, cleaning up callbacks");
  clearCallbacks();
}

static void deathRecpCallback(void* cookie) {
  IRadioVoiceImpl* impl = static_cast<IRadioVoiceImpl*>(cookie);
  if (impl != nullptr) {
    impl->deathNotifier();
  }
}

/*
 *   @brief
 *   Registers the callback for IRadioVoice using the
 *   IRadioVoiceResponse and IRadioVoiceIndication objects
 *   being passed in by the client as parameters
 *
 */
::ndk::ScopedAStatus IRadioVoiceImpl::setResponseFunctions(
    const std::shared_ptr<aidlvoice::IRadioVoiceResponse>& in_radioVoiceResponse,
    const std::shared_ptr<aidlvoice::IRadioVoiceIndication>& in_radioVoiceIndication) {
  QCRIL_LOG_INFO("IRadioVoiceImpl::setResponseFunctions: Set client callback");

  {
    std::unique_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);

    if (mIRadioVoiceResponse != nullptr) {
      AIBinder_unlinkToDeath(mIRadioVoiceResponse->asBinder().get(), mDeathRecipient,
                             reinterpret_cast<void*>(this));
    }

    setResponseFunctions_nolock(in_radioVoiceResponse, in_radioVoiceIndication);

    if (mIRadioVoiceResponse != nullptr) {
      AIBinder_DeathRecipient_delete(mDeathRecipient);
      mDeathRecipient = AIBinder_DeathRecipient_new(&deathRecpCallback);
      if (mDeathRecipient != nullptr) {
        AIBinder_linkToDeath(mIRadioVoiceResponse->asBinder().get(), mDeathRecipient,
                             reinterpret_cast<void*>(this));
      }
    }
  }

  if(in_radioVoiceResponse != nullptr && in_radioVoiceIndication != nullptr) {
    notifyVoiceClientConnected();
  }

  return ndk::ScopedAStatus::ok();
}

IRadioVoiceImpl::IRadioVoiceImpl(qcril_instance_id_e_type instance) : mInstanceId(instance) {
}

IRadioVoiceImpl::~IRadioVoiceImpl() {
}

void IRadioVoiceImpl::clearCallbacks() {
  QCRIL_LOG_FUNC_ENTRY("enter");
  {
    std::unique_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
    mIRadioVoiceResponse = nullptr;
    mIRadioVoiceIndication = nullptr;
    AIBinder_DeathRecipient_delete(mDeathRecipient);
    mDeathRecipient = nullptr;
  }
  QCRIL_LOG_FUNC_ENTRY("exit");
}

int IRadioVoiceImpl::callRing(std::shared_ptr<QcRilUnsolCallRingingMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY();
  auto ind = this->getIndicationCallback();
  if (ind && msg) {
    bool isGsm;
    aidlvoice::CdmaSignalInfoRecord record{};
    if (!msg->hasCdmaSignalInfoRecord()) {
    isGsm = true;
    } else {
      isGsm = false;
      if (msg->getCdmaSignalInfoRecord() == nullptr) {
        QCRIL_LOG_ERROR("callRing: invalid indication");
        return 1;
      }
      convertRilCdmaSignalInfoRecord(record, msg->getCdmaSignalInfoRecord());
    }
    // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
    qti::ril::utils::grabPartialWakeLock();
    QCRIL_LOG_DEBUG("UNSOL: callRing");
    auto ret = ind->callRing(
        aidl::android::hardware::radio::RadioIndicationType::UNSOLICITED_ACK_EXP, isGsm, record);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
    }
  } else {
    QCRIL_LOG_INFO("ind null: %s, msg null: %s", ind ? "false" : "true", msg ? "false" : "true");
  }
  QCRIL_LOG_FUNC_RETURN();
  return 0;
}

int IRadioVoiceImpl::callStateChanged(std::shared_ptr<QcRilUnsolCallStateChangeMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY();
  auto ind = this->getIndicationCallback();
  if (ind && msg) {
    // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
    qti::ril::utils::grabPartialWakeLock();
    QCRIL_LOG_DEBUG("UNSOL: callStateChanged");
    auto ret = ind->callStateChanged(
        aidl::android::hardware::radio::RadioIndicationType::UNSOLICITED_ACK_EXP);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
    }
  } else {
    QCRIL_LOG_INFO("ind null: %s, msg null: %s", ind ? "false" : "true", msg ? "false" : "true");
  }
  QCRIL_LOG_FUNC_RETURN();
  return 0;
}

int IRadioVoiceImpl::sendCdmaCallWaiting(std::shared_ptr<QcRilUnsolCdmaCallWaitingMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY();
  auto ind = this->getIndicationCallback();
  if (ind && msg) {
    aidlvoice::CdmaCallWaiting callWaitingRecord{};
    makeCdmaCallWaiting(callWaitingRecord, msg);
    // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
    qti::ril::utils::grabPartialWakeLock();
    QCRIL_LOG_DEBUG("UNSOL: cdmaCallWaiting");
    auto ret = ind->cdmaCallWaiting(
        aidl::android::hardware::radio::RadioIndicationType::UNSOLICITED_ACK_EXP,
        callWaitingRecord);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
    }
  } else {
    QCRIL_LOG_INFO("ind null: %s, msg null: %s", ind ? "false" : "true", msg ? "false" : "true");
  }
  QCRIL_LOG_FUNC_RETURN();
  return 0;
}

int IRadioVoiceImpl::sendCdmaInfoRec(std::shared_ptr<QcRilUnsolCdmaInfoRecordMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY();
  auto ind = this->getIndicationCallback();
  if (ind && msg) {
    vector<aidlvoice::CdmaInformationRecord> records;
    auto valid = makeCdmaInformationRecords(records, msg);
    if (valid) {
      // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
      qti::ril::utils::grabPartialWakeLock();
      QCRIL_LOG_DEBUG("UNSOL: cdmaInfoRec");
      auto ret = ind->cdmaInfoRec(
          aidl::android::hardware::radio::RadioIndicationType::UNSOLICITED_ACK_EXP, records);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s",
            ret.getDescription().c_str());
      }
    }
  } else {
    QCRIL_LOG_INFO("ind null: %s, msg null: %s", ind ? "false" : "true", msg ? "false" : "true");
  }
  QCRIL_LOG_FUNC_RETURN();
  return 0;
}

int IRadioVoiceImpl::sendCdmaOtaProvisionStatus(
  std::shared_ptr<QcRilUnsolCdmaOtaProvisionStatusMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY();
  auto ind = this->getIndicationCallback();
  if (ind && msg) {
    if (msg && msg->hasStatus()) {
      aidlvoice::CdmaOtaProvisionStatus status =
          static_cast<aidlvoice::CdmaOtaProvisionStatus>(msg->getStatus());
      // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
      qti::ril::utils::grabPartialWakeLock();
      QCRIL_LOG_DEBUG("UNSOL: cdmaOtaProvisionStatus");
      auto ret = ind->cdmaOtaProvisionStatus(
          aidl::android::hardware::radio::RadioIndicationType::UNSOLICITED_ACK_EXP, status);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s",
            ret.getDescription().c_str());
      }
    }
  } else {
    QCRIL_LOG_INFO("ind null: %s, msg null: %s", ind ? "false" : "true", msg ? "false" : "true");
  }
  QCRIL_LOG_FUNC_RETURN();
  return 0;
}

int IRadioVoiceImpl::sendEmergencyCallbackMode(
  std::shared_ptr<RilUnsolEmergencyCallbackModeMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY();
  auto ind = this->getIndicationCallback();
  if (ind && msg) {
    auto mode = msg->getEmergencyCallbackMode();
    // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
    qti::ril::utils::grabPartialWakeLock();
    if (mode == RilUnsolEmergencyCallbackModeMessage::EmergencyCallbackMode::ENTER) {
      QCRIL_LOG_DEBUG("UNSOL: enterEmergencyCallbackMode");
      auto ret = ind->enterEmergencyCallbackMode(
          aidl::android::hardware::radio::RadioIndicationType::UNSOLICITED_ACK_EXP);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s",
            ret.getDescription().c_str());
      }
    } else {
      QCRIL_LOG_DEBUG("UNSOL: exitEmergencyCallbackMode");
      auto ret = ind->exitEmergencyCallbackMode(
      aidl::android::hardware::radio::RadioIndicationType::UNSOLICITED_ACK_EXP);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s",
            ret.getDescription().c_str());
      }
    }
  } else {
    QCRIL_LOG_INFO("ind null: %s, msg null: %s", ind ? "false" : "true", msg ? "false" : "true");
  }
  QCRIL_LOG_FUNC_RETURN();
  return 0;
}

int IRadioVoiceImpl::sendIndicateRingbackTone(std::shared_ptr<QcRilUnsolRingbackToneMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY();
  auto ind = this->getIndicationCallback();
  if (ind && msg) {
    if (msg->hasRingBackToneOperation()) {
      bool start =
          (msg->getRingBackToneOperation() == qcril::interfaces::RingBackToneOperation::START)
          ? true : false;
      // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
      qti::ril::utils::grabPartialWakeLock();
      QCRIL_LOG_DEBUG("UNSOL: indicateRingbackTone");
      auto ret = ind->indicateRingbackTone(
          aidl::android::hardware::radio::RadioIndicationType::UNSOLICITED_ACK_EXP, start);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s",
            ret.getDescription().c_str());
      }
    }
  } else {
    QCRIL_LOG_INFO("ind null: %s, msg null: %s", ind ? "false" : "true", msg ? "false" : "true");
  }
  QCRIL_LOG_FUNC_RETURN();
  return 0;
}

int IRadioVoiceImpl::sendOnSupplementaryServiceIndication(
  std::shared_ptr<QcRilUnsolSupplementaryServiceMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY();
  auto ind = this->getIndicationCallback();
  if (ind && msg) {
    aidlvoice::StkCcUnsolSsResult ss{};
    makeStkCcUnsolSsResult(ss, msg);
    // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
    qti::ril::utils::grabPartialWakeLock();
    QCRIL_LOG_DEBUG("UNSOL: onSupplementaryServiceIndication");
    auto ret = ind->onSupplementaryServiceIndication(
        aidl::android::hardware::radio::RadioIndicationType::UNSOLICITED_ACK_EXP, ss);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
    }
  } else {
    QCRIL_LOG_INFO("ind null: %s, msg null: %s", ind ? "false" : "true", msg ? "false" : "true");
  }
  QCRIL_LOG_FUNC_RETURN();
  return 0;
}

int IRadioVoiceImpl::sendSrvccStateNotify(std::shared_ptr<QcRilUnsolSrvccStatusMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY();
  auto ind = this->getIndicationCallback();
  if (ind && msg) {
    if (msg->hasState()) {
      aidlvoice::SrvccState state =
          static_cast<aidlvoice::SrvccState>(msg->getState());  // TODO map this?
      // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
      qti::ril::utils::grabPartialWakeLock();
      QCRIL_LOG_DEBUG("UNSOL: srvccStateNotify");
      auto ret = ind->srvccStateNotify(
          aidl::android::hardware::radio::RadioIndicationType::UNSOLICITED_ACK_EXP, state);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s",
            ret.getDescription().c_str());
      }
    }
  } else {
    QCRIL_LOG_INFO("ind null: %s, msg null: %s", ind ? "false" : "true", msg ? "false" : "true");
  }
  QCRIL_LOG_FUNC_RETURN();
  return 0;
}

int IRadioVoiceImpl::sendStkCCAlphaNotify(std::shared_ptr<RilUnsolStkCCAlphaNotifyMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY();
  auto ind = this->getIndicationCallback();
  if (ind && msg) {
    // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
    qti::ril::utils::grabPartialWakeLock();
    QCRIL_LOG_DEBUG("UNSOL: stkCallControlAlphaNotify");
    auto ret = ind->stkCallControlAlphaNotify(
        aidl::android::hardware::radio::RadioIndicationType::UNSOLICITED_ACK_EXP,
        msg->getPayload());
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
    }
  } else {
    QCRIL_LOG_INFO("ind null: %s, msg null: %s", ind ? "false" : "true", msg ? "false" : "true");
  }
  QCRIL_LOG_FUNC_RETURN();
  return 0;
}

int IRadioVoiceImpl::sendCurrentEmergencyNumberList(
    std::shared_ptr<RilUnsolEmergencyListIndMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY();
  auto ind = this->getIndicationCallback();

  if (ind && msg) {
    vector<aidlvoice::EmergencyNumber> ecc_list;
    convertRilEccListToAidl(msg->getEmergencyList(), ecc_list);
    QCRIL_LOG_DEBUG("UNSOL: currentEmergencyNumberList");
    auto ret = ind->currentEmergencyNumberList(
        aidl::android::hardware::radio::RadioIndicationType::UNSOLICITED, ecc_list);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
    }
  } else {
    QCRIL_LOG_INFO("ind null: %s, msg null: %s", ind ? "false" : "true", msg ? "false" : "true");
  }
  QCRIL_LOG_FUNC_RETURN();
  return 0;
}

int IRadioVoiceImpl::sendGstkIndication(std::shared_ptr<GstkUnsolIndMsg> msg) {
  QCRIL_LOG_FUNC_ENTRY();
  auto ind = this->getIndicationCallback();
  RIL_GSTK_UnsolData stk_ind = msg->get_unsol_data();
  if (ind && msg && stk_ind.type == RIL_GSTK_UNSOL_STK_CALL_SETUP) {
    // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
    qti::ril::utils::grabPartialWakeLock();
    QCRIL_LOG_DEBUG("UNSOL: stkCallSetup");
    auto ret = ind->stkCallSetup(
        aidl::android::hardware::radio::RadioIndicationType::UNSOLICITED_ACK_EXP,
        stk_ind.setupCallTimeout);
  } else {
    QCRIL_LOG_INFO("ind null: %s, msg null: %s", ind ? "false" : "true", msg ? "false" : "true");
  }
  QCRIL_LOG_FUNC_RETURN();
  return 0;
}

int IRadioVoiceImpl::sendOnUssd(std::shared_ptr<QcRilUnsolOnUssdMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY();
  auto ind = this->getIndicationCallback();
  // TODO: change to msg->isCsDomain() when AOSP feature is enabled.
  if (ind && msg && !msg->isUnknownDomain()) {
    if (msg->hasMode()) {
      aidlvoice::UssdModeType modeType = convertUssdModeType(msg->getMode());
      std::string ussdMsg;
      if (msg->hasMessage() && !msg->getMessage().empty()) {
        ussdMsg = msg->getMessage();
      }
      // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
      qti::ril::utils::grabPartialWakeLock();
      QCRIL_LOG_DEBUG("UNSOL: onUssd");
      auto ret = ind->onUssd(
          aidl::android::hardware::radio::RadioIndicationType::UNSOLICITED_ACK_EXP,
          modeType, ussdMsg);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
      }
    }
  } else {
    QCRIL_LOG_INFO("ind null: %s, msg null: %s", ind ? "false" : "true", msg ? "false" : "true");
  }
  QCRIL_LOG_FUNC_RETURN();
  return 0;
}

// Requests
::ndk::ScopedAStatus IRadioVoiceImpl::acceptCall(int32_t in_serial) {
  QCRIL_LOG_DEBUG("acceptCall: serial=%d", in_serial);
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestAnswerMessage>(this->getContext(in_serial));
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          }
          this->sendResponseForAcceptCall(in_serial, errorCode);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);
  if (sendFailure) {
    this->sendResponseForAcceptCall(in_serial, RIL_E_NO_MEMORY);
  }
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus IRadioVoiceImpl::cancelPendingUssd(int32_t in_serial) {
  QCRIL_LOG_DEBUG("cancelPendingUssd: serial=%d", in_serial);
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestCancelUssdMessage>(this->getContext(in_serial));
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          }
          this->sendResponseForCancelPendingUssd(in_serial, errorCode);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);
  if (sendFailure) {
    this->sendResponseForCancelPendingUssd(in_serial, RIL_E_NO_MEMORY);
  }
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus IRadioVoiceImpl::conference(int32_t in_serial) {
  QCRIL_LOG_DEBUG("conference: serial=%d", in_serial);
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestConferenceMessage>(this->getContext(in_serial));
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
          }
          this->sendResponseForConference(in_serial, errorCode);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);
  if (sendFailure) {
    this->sendResponseForConference(in_serial, RIL_E_NO_MEMORY);
  }
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus IRadioVoiceImpl::dial(int32_t in_serial,
    const aidlvoice::Dial& in_dialInfo) {
  QCRIL_LOG_DEBUG("dial: serial=%d", in_serial);
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestDialMessage>(this->getContext(in_serial));
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    if (!in_dialInfo.address.empty()) {
      msg->setAddress(in_dialInfo.address);
    }
    msg->setClir(in_dialInfo.clir);
    msg->setCallDomain(qcril::interfaces::CallDomain::CS);
    if (in_dialInfo.uusInfo.size() != 0) {
      auto uusInfo = std::make_shared<qcril::interfaces::UusInfo>();
      if (uusInfo) {
        uusInfo->setType(static_cast<RIL_UUS_Type>(in_dialInfo.uusInfo[0].uusType));
        uusInfo->setDcs(static_cast<RIL_UUS_DCS>(in_dialInfo.uusInfo[0].uusDcs));
        if (in_dialInfo.uusInfo[0].uusData.size() > 0) {
          uusInfo->setData(in_dialInfo.uusInfo[0].uusData);
        }
        msg->setUusInfo(uusInfo);
      }
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
          }
          this->sendResponseForDial(in_serial, errorCode);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);
  if (sendFailure) {
    this->sendResponseForDial(in_serial, RIL_E_NO_MEMORY);
  }
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus IRadioVoiceImpl::emergencyDial(int32_t in_serial,
    const aidlvoice::Dial& in_dialInfo, int32_t in_categories,
    const std::vector<std::string>& in_urns, aidlvoice::EmergencyCallRouting in_routing,
    bool in_hasKnownUserIntentEmergency, bool in_isTesting) {
  QCRIL_LOG_DEBUG("emergencyDial: serial=%d", in_serial);
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestDialMessage>(this->getContext(in_serial));
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    if (!in_dialInfo.address.empty()) {
      msg->setAddress(in_dialInfo.address);
    }
    msg->setClir(static_cast<int>(in_dialInfo.clir));
    msg->setCallDomain(qcril::interfaces::CallDomain::CS);
    if (in_dialInfo.uusInfo.size() != 0) {
      auto uusInfo = std::make_shared<qcril::interfaces::UusInfo>();
      if (uusInfo) {
        uusInfo->setType(static_cast<RIL_UUS_Type>(in_dialInfo.uusInfo[0].uusType));
        uusInfo->setDcs(static_cast<RIL_UUS_DCS>(in_dialInfo.uusInfo[0].uusDcs));
        if (in_dialInfo.uusInfo[0].uusData.size() > 0) {
          uusInfo->setData(in_dialInfo.uusInfo[0].uusData);
        }
        msg->setUusInfo(uusInfo);
      }
    }
    msg->setIsEmergency(true);
    msg->setCategories(toRilCategories(in_categories));
    msg->setRouting(toRilRouting(in_routing));
    msg->setIsForEccTesting(in_isTesting);
    msg->setIsIntentionEcc(false);
    msg->setIsIntentionEcc(in_hasKnownUserIntentEmergency);
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
          }
          this->sendResponseForEmergencyDial(in_serial, errorCode);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);
  if (sendFailure) {
    this->sendResponseForEmergencyDial(in_serial, RIL_E_NO_MEMORY);
  }
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus IRadioVoiceImpl::exitEmergencyCallbackMode(int32_t in_serial) {
  QCRIL_LOG_DEBUG("exitEmergencyCallbackMode: serial=%d", in_serial);
  auto msg = std::make_shared<RilRequestExitEmergencyCallbackMessage>(this->getContext(in_serial));
  if (msg != nullptr) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          }
          this->sendResponseForExitEmergencyCallbackMode(in_serial, errorCode);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForExitEmergencyCallbackMode(in_serial, RIL_E_NO_MEMORY);
  }
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus IRadioVoiceImpl::explicitCallTransfer(int32_t in_serial) {
  QCRIL_LOG_DEBUG("explicitCallTransfer: serial=%d", in_serial);
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestExplicitCallTransferMessage>(this->getContext(in_serial));
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          }
          this->sendResponseForExplicitCallTransfer(in_serial, errorCode);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);
  if (sendFailure) {
    this->sendResponseForExplicitCallTransfer(in_serial, RIL_E_NO_MEMORY);
  }
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus IRadioVoiceImpl::getCallForwardStatus(int32_t in_serial,
    const aidlvoice::CallForwardInfo& in_callInfo) {
  QCRIL_LOG_DEBUG("getCallForwardStatus: serial=%d", in_serial);
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestQueryCallForwardMessage>(this->getContext(in_serial));
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    // Set parameters
    if (in_callInfo.reason != INT32_MAX) {
      msg->setReason(in_callInfo.reason);
    }
    if (in_callInfo.serviceClass != INT32_MAX) {
      msg->setServiceClass(in_callInfo.serviceClass);
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          std::shared_ptr<qcril::interfaces::GetCallForwardRespData> callFwdData{};
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
            callFwdData =
                std::static_pointer_cast<qcril::interfaces::GetCallForwardRespData>(resp->data);
          }
          this->sendResponseForGetCallForwardStatus(in_serial, errorCode, callFwdData);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);
  if (sendFailure) {
    this->sendResponseForGetCallForwardStatus(in_serial, RIL_E_NO_MEMORY, nullptr);
  }
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus IRadioVoiceImpl::getCallWaiting(int32_t in_serial,
    int32_t in_serviceClass) {
  QCRIL_LOG_DEBUG("getCallWaiting: serial=%d", in_serial);
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestGetCallWaitingMessage>(this->getContext(in_serial));
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    // Set parameters
    msg->setServiceClass(in_serviceClass);
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          std::shared_ptr<qcril::interfaces::CallWaitingInfo> cwInfo{};
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
            cwInfo = std::static_pointer_cast<qcril::interfaces::CallWaitingInfo>(resp->data);
          }
          this->sendResponseForGetCallWaiting(in_serial, errorCode, cwInfo);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);
  if (sendFailure) {
    this->sendResponseForGetCallWaiting(in_serial, RIL_E_NO_MEMORY, nullptr);
  }
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus IRadioVoiceImpl::getClip(int32_t in_serial) {
  QCRIL_LOG_DEBUG("getClip: serial=%d", in_serial);
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestGetClipMessage>(this->getContext(in_serial));
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          std::shared_ptr<qcril::interfaces::ClipInfo> clipInfo{};
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
            clipInfo = std::static_pointer_cast<qcril::interfaces::ClipInfo>(resp->data);
          }
          this->sendResponseForGetClip(in_serial, errorCode, clipInfo);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);
  if (sendFailure) {
    this->sendResponseForGetClip(in_serial, RIL_E_NO_MEMORY, nullptr);
  }
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus IRadioVoiceImpl::getClir(int32_t in_serial) {
  QCRIL_LOG_DEBUG("getClir: serial=%d", in_serial);
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestGetClirMessage>(this->getContext(in_serial));
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          std::shared_ptr<qcril::interfaces::ClirInfo> clirInfo{};
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
            clirInfo = std::static_pointer_cast<qcril::interfaces::ClirInfo>(resp->data);
          }
          this->sendResponseForGetClir(in_serial, errorCode, clirInfo);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);
  if (sendFailure) {
    this->sendResponseForGetClir(in_serial, RIL_E_NO_MEMORY, nullptr);
  }
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus IRadioVoiceImpl::getCurrentCalls(int32_t in_serial) {
  QCRIL_LOG_DEBUG("getCurrentCalls: serial=%d", in_serial);
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestGetCurrentCallsMessage>(this->getContext(in_serial));
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          std::shared_ptr<qcril::interfaces::GetCurrentCallsRespData> currentCalls{};
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
            currentCalls =
                std::static_pointer_cast<qcril::interfaces::GetCurrentCallsRespData>(resp->data);
          }
          this->sendResponseForGetCurrentCalls(in_serial, errorCode, currentCalls);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);
  if (sendFailure) {
    this->sendResponseForGetCurrentCalls(in_serial, RIL_E_NO_MEMORY, nullptr);
  }
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus IRadioVoiceImpl::getLastCallFailCause(int32_t in_serial) {
  QCRIL_LOG_DEBUG("getLastCallFailCause: serial=%d", in_serial);
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestLastCallFailCauseMessage>(this->getContext(in_serial));
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          std::shared_ptr<qcril::interfaces::LastCallFailCauseInfo> failCause{};
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
            failCause =
                std::static_pointer_cast<qcril::interfaces::LastCallFailCauseInfo>(resp->data);
          }
          this->sendResponseForGetLastCallFailCause(in_serial, errorCode, failCause);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);
  if (sendFailure) {
    this->sendResponseForGetLastCallFailCause(in_serial, RIL_E_NO_MEMORY, nullptr);
  }
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus IRadioVoiceImpl::getMute(int32_t in_serial) {
  QCRIL_LOG_DEBUG("getMute: serial=%d", in_serial);
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestGetMuteMessage>(this->getContext(in_serial));
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          std::shared_ptr<qcril::interfaces::MuteInfo> muteInfo{};
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
            muteInfo = std::static_pointer_cast<qcril::interfaces::MuteInfo>(resp->data);
          }
          this->sendResponseForGetMute(in_serial, errorCode, muteInfo);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);
  if (sendFailure) {
    this->sendResponseForGetMute(in_serial, RIL_E_NO_MEMORY, nullptr);
  }
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus IRadioVoiceImpl::getPreferredVoicePrivacy(int32_t in_serial) {
  QCRIL_LOG_DEBUG("getPreferredVoicePrivacy: serial=%d", in_serial);
  bool sendFailure = false;
  do {
    auto msg =
        std::make_shared<QcRilRequestGetPreferredVoicePrivacyMessage>(this->getContext(in_serial));
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          std::shared_ptr<qcril::interfaces::PrivacyModeResp> privacyModeResp{};
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
            privacyModeResp =
                std::static_pointer_cast<qcril::interfaces::PrivacyModeResp>(resp->data);
          }
          this->sendResponseForGetPreferredVoicePrivacy(in_serial, errorCode, privacyModeResp);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);
  if (sendFailure) {
    this->sendResponseForGetPreferredVoicePrivacy(in_serial, RIL_E_NO_MEMORY, nullptr);
  }
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus IRadioVoiceImpl::getTtyMode(int32_t in_serial) {
  QCRIL_LOG_DEBUG("getTTYMode: serial=%d", in_serial);
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestGetTtyModeMessage>(this->getContext(in_serial));
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          std::shared_ptr<qcril::interfaces::TtyModeResp> ttyModeResp{};
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
            ttyModeResp = std::static_pointer_cast<qcril::interfaces::TtyModeResp>(resp->data);
          }
          this->sendResponseForGetTTYMode(in_serial, errorCode, ttyModeResp);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);
  if (sendFailure) {
    this->sendResponseForGetTTYMode(in_serial, RIL_E_NO_MEMORY, nullptr);
  }
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus IRadioVoiceImpl::handleStkCallSetupRequestFromSim(int32_t in_serial,
    bool in_accept) {
  QCRIL_LOG_DEBUG("handleStkCallSetupRequestFromSim: serial=%d, in_accept=%s", in_serial,
      in_accept ? "true" : "false");
  auto msg = std::make_shared<GstkSendSetupCallResponseRequestMsg>(in_accept);
  if (msg) {
    GenericCallback<RIL_GSTK_Errno> cb(
        ([this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                        std::shared_ptr<RIL_GSTK_Errno> responseDataPtr) -> void {
          aidl::android::hardware::radio::RadioError errorCode{
              aidl::android::hardware::radio::RadioError::INTERNAL_ERR };
          if (status == Message::Callback::Status::SUCCESS && responseDataPtr) {
            errorCode = static_cast<aidl::android::hardware::radio::RadioError>(*responseDataPtr);
          }
          this->sendResponseForHandleStkCallSetupRequestFromSim(in_serial, errorCode);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForHandleStkCallSetupRequestFromSim(in_serial,
        aidl::android::hardware::radio::RadioError::NO_MEMORY);
  }
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus IRadioVoiceImpl::hangup(int32_t in_serial, int32_t in_gsmIndex) {
  QCRIL_LOG_DEBUG("hangup: serial=%d", in_serial);
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestHangupMessage>(this->getContext(in_serial));
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    msg->setCallIndex(in_gsmIndex);
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
          }
          this->sendResponseForHangup(in_serial, errorCode);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);
  if (sendFailure) {
    this->sendResponseForHangup(in_serial, RIL_E_NO_MEMORY);
  }
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus IRadioVoiceImpl::hangupForegroundResumeBackground(int32_t in_serial) {
  QCRIL_LOG_DEBUG("hangupForegroundResumeBackground: serial=%d", in_serial);
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestHangupForegroundResumeBackgroundMessage>(
        this->getContext(in_serial));
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
          }
          this->sendResponseForHangupForegroundResumeBackground(in_serial, errorCode);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);
  if (sendFailure) {
    this->sendResponseForHangupForegroundResumeBackground(in_serial, RIL_E_NO_MEMORY);
  }
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus IRadioVoiceImpl::hangupWaitingOrBackground(int32_t in_serial) {
  QCRIL_LOG_DEBUG("hangupWaitingOrBackground: serial=%d", in_serial);
  bool sendFailure = false;
  do {
    auto msg =
        std::make_shared<QcRilRequestHangupWaitingOrBackgroundMessage>(this->getContext(in_serial));
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
          }
          this->sendResponseForHangupWaitingOrBackground(in_serial, errorCode);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);
  if (sendFailure) {
    this->sendResponseForHangupWaitingOrBackground(in_serial, RIL_E_NO_MEMORY);
  }
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus IRadioVoiceImpl::rejectCall(int32_t in_serial) {
  QCRIL_LOG_DEBUG("rejectCall: serial=%d", in_serial);
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestUdubMessage>(this->getContext(in_serial));
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
          }
          this->sendResponseForRejectCall(in_serial, errorCode);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);
  if (sendFailure) {
    this->sendResponseForRejectCall(in_serial, RIL_E_NO_MEMORY);
  }
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus IRadioVoiceImpl::responseAcknowledgement() {
  QCRIL_LOG_DEBUG("responseAcknowledgement");
  qti::ril::utils::releaseWakeLock();
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus IRadioVoiceImpl::sendBurstDtmf(int32_t in_serial,
    const std::string& in_dtmf, int32_t in_on, int32_t in_off) {
  QCRIL_LOG_DEBUG("sendBurstDtmf: serial=%d", in_serial);
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestCdmaBurstDtmfMessage>(this->getContext(in_serial));
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    if (!in_dtmf.empty()) {
      msg->setDigitBuffer(in_dtmf);
    }
    msg->setDtmfOnLength(in_on);
    msg->setDtmfOffLength(in_off);
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          }
          this->sendResponseForSendBurstDtmf(in_serial, errorCode);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);
  if (sendFailure) {
    this->sendResponseForSendBurstDtmf(in_serial, RIL_E_NO_MEMORY);
  }
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus IRadioVoiceImpl::sendCdmaFeatureCode(int32_t in_serial,
    const std::string& in_featureCode) {
  QCRIL_LOG_DEBUG("sendCDMAFeatureCode: serial=%d", in_serial);
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestCdmaFlashMessage>(this->getContext(in_serial));
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    if (!in_featureCode.empty()) {
      msg->setFeatureCode(in_featureCode);
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          }
          this->sendResponseForSendCDMAFeatureCode(in_serial, errorCode);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);
  if (sendFailure) {
    this->sendResponseForSendCDMAFeatureCode(in_serial, RIL_E_NO_MEMORY);
  }
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus IRadioVoiceImpl::sendDtmf(int32_t in_serial, const std::string& in_s) {
  QCRIL_LOG_DEBUG("sendDtmf: serial=%d", in_serial);
  RIL_Errno errResp = RIL_E_SUCCESS;
  do {
    auto msg = std::make_shared<QcRilRequestSendDtmfMessage>(this->getContext(in_serial));
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      errResp = RIL_E_NO_MEMORY;
      break;
    }
    if (in_s.empty()) {
      QCRIL_LOG_ERROR("Invalid parameter: s");
      errResp = RIL_E_INVALID_ARGUMENTS;
      break;
    }
    msg->setDigit(in_s[0]);
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          }
          this->sendResponseForSendDtmf(in_serial, errorCode);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);
  if (errResp != RIL_E_SUCCESS) {
    this->sendResponseForSendDtmf(in_serial, errResp);
  }
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus IRadioVoiceImpl::sendUssd(int32_t in_serial, const std::string& in_ussd) {
  QCRIL_LOG_DEBUG("sendUssd: serial=%d", in_serial);
  RIL_Errno errResp = RIL_E_SUCCESS;
  do {
    auto msg = std::make_shared<QcRilRequestSendUssdMessage>(this->getContext(in_serial));
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      errResp = RIL_E_NO_MEMORY;
      break;
    }
    if (in_ussd.empty()) {
      QCRIL_LOG_ERROR("Invalid parameter: ussd");
      errResp = RIL_E_INVALID_ARGUMENTS;
      break;
    }
    if (isUssdOverImsSupported()) {
      msg->setIsDomainAuto(false);
    } else {
      msg->setIsDomainAuto(true);
    }
    msg->setUssd(in_ussd);
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          }
          this->sendResponseForSendUssd(in_serial, errorCode);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);
  if (errResp != RIL_E_SUCCESS) {
    this->sendResponseForSendUssd(in_serial, errResp);
  }
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus IRadioVoiceImpl::separateConnection(int32_t in_serial, int32_t in_gsmIndex) {
  QCRIL_LOG_DEBUG("separateConnection: serial=%d", in_serial);
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestSeparateConnectionMessage>(this->getContext(in_serial));
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    msg->setCallId(in_gsmIndex);
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          }
          this->sendResponseForSeparateConnection(in_serial, errorCode);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);
  if (sendFailure) {
    this->sendResponseForSeparateConnection(in_serial, RIL_E_NO_MEMORY);
  }
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus IRadioVoiceImpl::setCallForward(int32_t in_serial,
    const aidlvoice::CallForwardInfo& in_callInfo) {
  QCRIL_LOG_DEBUG("setCallForward: serial=%d", in_serial);
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestSetCallForwardMessage>(this->getContext(in_serial));
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    // Set parameters
    msg->setStatus(convertCallForwardInfoStatus(in_callInfo.status));
    msg->setReason(in_callInfo.reason);
    msg->setServiceClass(in_callInfo.serviceClass);
    msg->setToa(in_callInfo.toa);
    if (!in_callInfo.number.empty()) {
      msg->setNumber(in_callInfo.number);
    }
    msg->setTimeSeconds(in_callInfo.timeSeconds);
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          }
          this->sendResponseForSetCallForward(in_serial, errorCode);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);
  if (sendFailure) {
    this->sendResponseForSetCallForward(in_serial, RIL_E_NO_MEMORY);
  }
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus IRadioVoiceImpl::setCallWaiting(int32_t in_serial,
    bool in_enable, int32_t in_serviceClass) {
  QCRIL_LOG_DEBUG("setCallWaiting: serial=%d", in_serial);
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestSetCallWaitingMessage>(this->getContext(in_serial));
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    // Set parameters
    if (in_enable) {
      msg->setServiceStatus(qcril::interfaces::ServiceClassStatus::ENABLED);
    } else {
      msg->setServiceStatus(qcril::interfaces::ServiceClassStatus::DISABLED);
    }
    msg->setServiceClass(in_serviceClass);
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          }
          this->sendResponseForSetCallWaiting(in_serial, errorCode);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);
  if (sendFailure) {
    this->sendResponseForSetCallWaiting(in_serial, RIL_E_NO_MEMORY);
  }
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus IRadioVoiceImpl::setClir(int32_t in_serial, int32_t in_status) {
  QCRIL_LOG_DEBUG("setClir: serial=%d", in_serial);
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestSetClirMessage>(this->getContext(in_serial));
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    msg->setParamN(in_status);
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          }
          this->sendResponseForSetClir(in_serial, errorCode);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);
  if (sendFailure) {
    this->sendResponseForSetClir(in_serial, RIL_E_NO_MEMORY);
  }
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus IRadioVoiceImpl::setMute(int32_t in_serial, bool in_enable) {
  QCRIL_LOG_DEBUG("setMute: serial=%d, enable=%d", in_serial, in_enable);
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestSetMuteMessage>(this->getContext(in_serial));
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    msg->setEnable(in_enable);
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          }
          this->sendResponseForSetMute(in_serial, errorCode);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);
  if (sendFailure) {
    this->sendResponseForSetMute(in_serial, RIL_E_NO_MEMORY);
  }
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus IRadioVoiceImpl::setPreferredVoicePrivacy(int32_t in_serial,
    bool in_enable) {
  QCRIL_LOG_DEBUG("setPreferredVoicePrivacy: serial=%d", in_serial);
  bool sendFailure = false;
  do {
    auto msg =
        std::make_shared<QcRilRequestSetPreferredVoicePrivacyMessage>(this->getContext(in_serial));
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    if (in_enable) {
      msg->setPrivacyMode(qcril::interfaces::PrivacyMode::ENHANCED);
    } else {
      msg->setPrivacyMode(qcril::interfaces::PrivacyMode::STANDARD);
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          }
          this->sendResponseForSetPreferredVoicePrivacy(in_serial, errorCode);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);
  if (sendFailure) {
    this->sendResponseForSetPreferredVoicePrivacy(in_serial, RIL_E_NO_MEMORY);
  }
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus IRadioVoiceImpl::setTtyMode(
    int32_t in_serial, aidlvoice::TtyMode in_mode) {
  QCRIL_LOG_DEBUG("setTTYMode: serial=%d", in_serial);
  RIL_Errno errResp = RIL_E_SUCCESS;
  do {
    auto msg = std::make_shared<QcRilRequestSetTtyModeMessage>(this->getContext(in_serial));
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      errResp = RIL_E_NO_MEMORY;
      break;
    }
    qcril::interfaces::TtyMode ttyMode = qcril::interfaces::TtyMode::UNKNOWN;
    switch (in_mode) {
      case aidlvoice::TtyMode::OFF:
        ttyMode = qcril::interfaces::TtyMode::MODE_OFF;
        break;
      case aidlvoice::TtyMode::FULL:
        ttyMode = qcril::interfaces::TtyMode::FULL;
        break;
      case aidlvoice::TtyMode::HCO:
        ttyMode = qcril::interfaces::TtyMode::HCO;
        break;
      case aidlvoice::TtyMode::VCO:
        ttyMode = qcril::interfaces::TtyMode::VCO;
        break;
      default:
        QCRIL_LOG_ERROR("Invlid TtyMode");
        errResp = RIL_E_INVALID_ARGUMENTS;
        break;
    }
    if (errResp != RIL_E_SUCCESS) {
      break;
    }
    msg->setTtyMode(ttyMode);
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          }
          this->sendResponseForSetTTYMode(in_serial, errorCode);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);
  if (errResp != RIL_E_SUCCESS) {
    this->sendResponseForSetTTYMode(in_serial, errResp);
  }
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus IRadioVoiceImpl::startDtmf(int32_t in_serial, const std::string& in_s) {
  QCRIL_LOG_DEBUG("startDtmf: serial=%d", in_serial);
  RIL_Errno errResp = RIL_E_SUCCESS;
  do {
    auto msg = std::make_shared<QcRilRequestStartDtmfMessage>(this->getContext(in_serial));
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      errResp = RIL_E_NO_MEMORY;
      break;
    }
    if (in_s.empty()) {
      QCRIL_LOG_ERROR("Invalid parameter: in_s");
      errResp = RIL_E_INVALID_ARGUMENTS;
      break;
    }
    msg->setDigit(in_s[0]);
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          } else if (status == Message::Callback::Status::CANCELLED) {
            errorCode = RIL_E_CANCELLED;
          }
          this->sendResponseForStartDtmf(in_serial, errorCode);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);
  if (errResp != RIL_E_SUCCESS) {
    this->sendResponseForStartDtmf(in_serial, errResp);
  }
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus IRadioVoiceImpl::stopDtmf(int32_t in_serial) {
  QCRIL_LOG_DEBUG("stopDtmf: serial=%d", in_serial);
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestStopDtmfMessage>(this->getContext(in_serial));
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          } else if (status == Message::Callback::Status::CANCELLED) {
            errorCode = RIL_E_CANCELLED;
          }
          this->sendResponseForStopDtmf(in_serial, errorCode);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);
  if (sendFailure) {
    this->sendResponseForStopDtmf(in_serial, RIL_E_NO_MEMORY);
  }
  return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus IRadioVoiceImpl::switchWaitingOrHoldingAndActive(int32_t in_serial) {
  QCRIL_LOG_DEBUG("switchWaitingOrHoldingAndActive: serial=%d", in_serial);
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestSwitchWaitingOrHoldingAndActiveMessage>(
        this->getContext(in_serial));
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
          }
          this->sendResponseForSwitchWaitingOrHoldingAndActive(in_serial, errorCode);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);
  if (sendFailure) {
    this->sendResponseForSwitchWaitingOrHoldingAndActive(in_serial, RIL_E_NO_MEMORY);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioVoiceImpl::isVoNrEnabled(int32_t in_serial) {
  QCRIL_LOG_DEBUG("isVoNrEnabled: serial=%d", in_serial);
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<RilRequestGetVonrCapRequest>(this->getContext(in_serial));
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno res = RIL_E_INTERNAL_ERR;
          std::shared_ptr<qcril::interfaces::RilQueryVonrStatusResult_t> payload;
          bool cap = false;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            res = resp->errorCode;
            payload = std::static_pointer_cast<qcril::interfaces::RilQueryVonrStatusResult_t>(
                    resp->data);
          }
          if (payload != nullptr) {
            cap = payload->status;
          }
          this->sendResponseForIsVoNrEnabled(in_serial, res, cap);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);

  if (sendFailure) {
    this->sendResponseForIsVoNrEnabled(in_serial, RIL_E_NO_MEMORY, false);
  }
  return ::ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioVoiceImpl::setVoNrEnabled(int32_t in_serial, bool in_enable) {
  QCRIL_LOG_DEBUG("setVoNrEnabled: serial=%d", in_serial);
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<RilRequestSetVonrCapRequest>(this->getContext(in_serial), in_enable);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno res = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            res = resp->errorCode;
          }
          this->sendResponseForSetVoNrEnabled(in_serial, res);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);

  if (sendFailure) {
    this->sendResponseForSetVoNrEnabled(in_serial, RIL_E_NO_MEMORY);
  }
  return ::ndk::ScopedAStatus::ok();
}

// Responses
void IRadioVoiceImpl::sendResponseForAcceptCall(int32_t serial, RIL_Errno errorCode) {
  aidl::android::hardware::radio::RadioResponseInfo responseInfo{
      aidl::android::hardware::radio::RadioResponseType::SOLICITED, serial,
      static_cast<aidl::android::hardware::radio::RadioError>(errorCode) };
  auto respCb = this->getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("acceptCallResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->acceptCallResponse(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
int IRadioVoiceImpl::sendAcknowledgeRequest(std::shared_ptr<RilAcknowledgeRequestMessage> msg) {
  if (msg) {
    auto ctx = msg->getContext();
    auto respCb = this->getResponseCallback();
    if (respCb && ctx) {
      QCRIL_LOG_DEBUG("acknowledgeRequest: serial=%d", ctx->serial);
      auto ret = respCb->acknowledgeRequest(ctx->serial);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
      }
    }
  }
  return 0;
}
void IRadioVoiceImpl::sendResponseForCancelPendingUssd(int32_t serial, RIL_Errno errorCode) {
   aidl::android::hardware::radio::RadioResponseInfo responseInfo{
      aidl::android::hardware::radio::RadioResponseType::SOLICITED, serial,
      static_cast<aidl::android::hardware::radio::RadioError>(errorCode) };
  auto respCb = this->getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("cancelPendingUssdResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->cancelPendingUssdResponse(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void IRadioVoiceImpl::sendResponseForConference(int32_t serial, RIL_Errno errorCode) {
  aidl::android::hardware::radio::RadioResponseInfo responseInfo{
      aidl::android::hardware::radio::RadioResponseType::SOLICITED, serial,
      static_cast<aidl::android::hardware::radio::RadioError>(errorCode) };
  auto respCb = this->getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("conferenceResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->conferenceResponse(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void IRadioVoiceImpl::sendResponseForDial(int32_t serial, RIL_Errno errorCode) {
  errorCode = convertInternalErrorCode(errorCode);
  aidl::android::hardware::radio::RadioResponseInfo responseInfo{
      aidl::android::hardware::radio::RadioResponseType::SOLICITED, serial,
      static_cast<aidl::android::hardware::radio::RadioError>(errorCode) };
  auto respCb = this->getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("dialResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->dialResponse(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void IRadioVoiceImpl::sendResponseForEmergencyDial(int32_t serial, RIL_Errno errorCode) {
  errorCode = convertInternalErrorCode(errorCode);
  aidl::android::hardware::radio::RadioResponseInfo responseInfo{
      aidl::android::hardware::radio::RadioResponseType::SOLICITED, serial,
      static_cast<aidl::android::hardware::radio::RadioError>(errorCode) };
  auto respCb = this->getResponseCallback();
  if (!respCb) {
    return;
  }
  QCRIL_LOG_DEBUG("emergencyDialResponse: serial=%d, error=%d", serial, errorCode);
  auto ret = respCb->emergencyDialResponse(responseInfo);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}
void IRadioVoiceImpl::sendResponseForExitEmergencyCallbackMode(
    int32_t serial, RIL_Errno errorCode) {
  aidl::android::hardware::radio::RadioResponseInfo responseInfo{
      aidl::android::hardware::radio::RadioResponseType::SOLICITED, serial,
      static_cast<aidl::android::hardware::radio::RadioError>(errorCode) };
  auto respCb = this->getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("exitEmergencyCallbackModeResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->exitEmergencyCallbackModeResponse(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void IRadioVoiceImpl::sendResponseForExplicitCallTransfer(int32_t serial, RIL_Errno errorCode) {
  aidl::android::hardware::radio::RadioResponseInfo responseInfo{
      aidl::android::hardware::radio::RadioResponseType::SOLICITED, serial,
      static_cast<aidl::android::hardware::radio::RadioError>(errorCode) };
  auto respCb = this->getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("explicitCallTransferResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->explicitCallTransferResponse(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void IRadioVoiceImpl::sendResponseForGetCallForwardStatus(
    int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::GetCallForwardRespData> data
) {
  aidl::android::hardware::radio::RadioResponseInfo responseInfo{
      aidl::android::hardware::radio::RadioResponseType::SOLICITED, serial,
      static_cast<aidl::android::hardware::radio::RadioError>(errorCode) };
  std::vector<aidlvoice::CallForwardInfo> callForwardInfos;
  if (errorCode == RIL_E_SUCCESS && data) {
    auto cfInfoList = data->getCallForwardInfo();
    if (!cfInfoList.empty()) {
      uint32_t count = cfInfoList.size();
      callForwardInfos.resize(count);
      for (uint32_t i = 0; i < count; i++) {
        auto& resp = cfInfoList[i];
        if (resp.hasStatus()) {
          callForwardInfos[i].status = convertCallForwardInfoStatusToAidl(resp.getStatus());
        }
        if (resp.hasReason()) {
          callForwardInfos[i].reason = resp.getReason();
        }
        if (resp.hasServiceClass()) {
          callForwardInfos[i].serviceClass = resp.getServiceClass();
        }
        if (resp.hasToa()) {
          callForwardInfos[i].toa = resp.getToa();
        }
        if (resp.hasNumber() && !resp.getNumber().empty()) {
          callForwardInfos[i].number = resp.getNumber();
        }
        if (resp.hasTimeSeconds()) {
          callForwardInfos[i].timeSeconds = resp.getTimeSeconds();
        }
      }
    }
  }
  auto respCb = this->getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("getCallForwardStatusResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->getCallForwardStatusResponse(responseInfo, callForwardInfos);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void IRadioVoiceImpl::sendResponseForGetCallWaiting(
    int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::CallWaitingInfo> cwInfo
) {
  aidl::android::hardware::radio::RadioResponseInfo responseInfo{
      aidl::android::hardware::radio::RadioResponseType::SOLICITED, serial,
      static_cast<aidl::android::hardware::radio::RadioError>(errorCode) };
  bool enable = false;
  int serviceClass = -1;
  if (errorCode == RIL_E_SUCCESS && cwInfo) {
    if (cwInfo->hasStatus()) {
      enable = ((cwInfo->getStatus() == qcril::interfaces::ServiceClassStatus::ENABLED) ? true
                                                                                        : false);
    }
    if (cwInfo->hasServiceClass()) {
      serviceClass = cwInfo->getServiceClass();
    }
  }

  auto respCb = this->getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("getCallWaitingResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->getCallWaitingResponse(responseInfo, enable, serviceClass);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void IRadioVoiceImpl::sendResponseForGetClip(
    int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::ClipInfo> clipInfo
) {
  aidl::android::hardware::radio::RadioResponseInfo responseInfo{
      aidl::android::hardware::radio::RadioResponseType::SOLICITED, serial,
      static_cast<aidl::android::hardware::radio::RadioError>(errorCode) };
  aidlvoice::ClipStatus clip = aidlvoice::ClipStatus::UNKNOWN;
  if (errorCode == RIL_E_SUCCESS && clipInfo) {
    if (clipInfo->hasClipStatus()) {
      if (clipInfo->getClipStatus() == qcril::interfaces::ClipStatus::NOT_PROVISIONED) {
        clip = static_cast<aidlvoice::ClipStatus>(0);  // 0 : CLIP not provisioned
      } else if (clipInfo->getClipStatus() == qcril::interfaces::ClipStatus::PROVISIONED) {
        clip = static_cast<aidlvoice::ClipStatus>(1);  // 1 : Provisioned
      }
    }
  }
  auto respCb = this->getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("getClipResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->getClipResponse(responseInfo, clip);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void IRadioVoiceImpl::sendResponseForGetClir(
    int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::ClirInfo> clirInfo
) {
  aidl::android::hardware::radio::RadioResponseInfo responseInfo{
      aidl::android::hardware::radio::RadioResponseType::SOLICITED, serial,
      static_cast<aidl::android::hardware::radio::RadioError>(errorCode) };
  int n = -1, m = -1;
  if (errorCode == RIL_E_SUCCESS && clirInfo) {
    if (clirInfo->hasAction()) {
      n = clirInfo->getAction();
    }
    if (clirInfo->hasPresentation()) {
      m = clirInfo->getPresentation();
    }
  }
  auto respCb = this->getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("getClirResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->getClirResponse(responseInfo, n, m);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

static std::string hide_number(std::string tmp)
{
  if (tmp.empty()) return "";
  if (tmp.length() > QCRIL_SHOW_PHONE_NUMBER_MAX) {
    std::string lastDigits = tmp.substr(tmp.length() - QCRIL_SHOW_PHONE_NUMBER_MAX);
    std::string numStr = QCRIL_HIDE_PHONE_NUMBER_STR + lastDigits;
      return numStr;
  } else {
    return "****";
  }
}

void IRadioVoiceImpl::sendResponseForGetCurrentCalls(
    int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::GetCurrentCallsRespData> currentCalls
) {
  aidl::android::hardware::radio::RadioResponseInfo responseInfo{
      aidl::android::hardware::radio::RadioResponseType::SOLICITED, serial,
      static_cast<aidl::android::hardware::radio::RadioError>(errorCode) };
  std::vector<aidlvoice::Call> calls{};
  if (errorCode == RIL_E_SUCCESS && currentCalls) {
    auto callList = currentCalls->getCallInfoList();
    if (!callList.empty()) {
      calls.reserve(callList.size());
      for (uint32_t i = 0; i < callList.size(); i++) {
        if (callList[i].getCallState() != qcril::interfaces::CallState::END) {
          aidlvoice::Call call{};
          convertToAidl(call, callList[i]);
          calls.push_back(std::move(call));
          QCRIL_LOG_DEBUG("Forwarded_number %d: %s",i,
              PII(call.forwardedNumber,hide_number(call.forwardedNumber)).c_str());
        }
      }
    }
  }
  auto respCb = this->getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("getCurrentCallsResponse: serial=%d, errorCode=%d", serial, errorCode);
    auto ret = respCb->getCurrentCallsResponse(responseInfo, calls);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void IRadioVoiceImpl::sendResponseForGetLastCallFailCause(
    int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::LastCallFailCauseInfo> failCause
) {
  aidl::android::hardware::radio::RadioResponseInfo responseInfo{
      aidl::android::hardware::radio::RadioResponseType::SOLICITED, serial,
      static_cast<aidl::android::hardware::radio::RadioError>(errorCode) };
  aidlvoice::LastCallFailCauseInfo info{};

  if (errorCode == RIL_E_SUCCESS && failCause) {
    if (failCause->hasCallFailCause()) {
      info.causeCode = static_cast<aidlvoice::LastCallFailCause>(failCause->getCallFailCause());
    }
    if (failCause->hasCallFailCauseDescription()) {
      info.vendorCause = failCause->getCallFailCauseDescription();
    }
  }
  auto respCb = this->getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("getLastCallFailCauseResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->getLastCallFailCauseResponse(responseInfo, info);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void IRadioVoiceImpl::sendResponseForGetMute(
    int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::MuteInfo> muteInfo
) {
  aidl::android::hardware::radio::RadioResponseInfo responseInfo{
      aidl::android::hardware::radio::RadioResponseType::SOLICITED, serial,
      static_cast<aidl::android::hardware::radio::RadioError>(errorCode) };
  bool enable = false;
  if (errorCode == RIL_E_SUCCESS && muteInfo) {
    if (muteInfo->hasEnable()) {
      enable = muteInfo->getEnable();
    }
  }
  auto respCb = this->getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("getMuteResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->getMuteResponse(responseInfo, enable);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void IRadioVoiceImpl::sendResponseForGetPreferredVoicePrivacy(
    int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::PrivacyModeResp> privacyModeResp
) {
  aidl::android::hardware::radio::RadioResponseInfo responseInfo{
      aidl::android::hardware::radio::RadioResponseType::SOLICITED, serial,
      static_cast<aidl::android::hardware::radio::RadioError>(errorCode) };
  bool enable = false;
  if (errorCode == RIL_E_SUCCESS && privacyModeResp) {
    if (privacyModeResp->hasPrivacyMode()) {
      if (privacyModeResp->getPrivacyMode() == qcril::interfaces::PrivacyMode::ENHANCED) {
        enable = true;
      }
    }
  }
  auto respCb = this->getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("getPreferredVoicePrivacyResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->getPreferredVoicePrivacyResponse(responseInfo, enable);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void IRadioVoiceImpl::sendResponseForGetTTYMode(
    int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::TtyModeResp> ttyModeResp
) {
  aidl::android::hardware::radio::RadioResponseInfo responseInfo{
      aidl::android::hardware::radio::RadioResponseType::SOLICITED, serial,
      static_cast<aidl::android::hardware::radio::RadioError>(errorCode) };
  aidlvoice::TtyMode mode = aidlvoice::TtyMode::OFF;
    if (errorCode == RIL_E_SUCCESS && ttyModeResp) {
      if (ttyModeResp->hasTtyMode()) {
        switch (ttyModeResp->getTtyMode()) {
          case qcril::interfaces::TtyMode::MODE_OFF:
            mode = aidlvoice::TtyMode::OFF;
            break;
          case qcril::interfaces::TtyMode::FULL:
            mode = aidlvoice::TtyMode::FULL;
            break;
          case qcril::interfaces::TtyMode::HCO:
            mode = aidlvoice::TtyMode::HCO;
            break;
          case qcril::interfaces::TtyMode::VCO:
            mode = aidlvoice::TtyMode::VCO;
            break;
          case qcril::interfaces::TtyMode::UNKNOWN:
            mode = aidlvoice::TtyMode::OFF;
            break;
        }
      }
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("getTtyModeResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->getTtyModeResponse(responseInfo, mode);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
      }
    }
}
void IRadioVoiceImpl::sendResponseForHandleStkCallSetupRequestFromSim(
    int32_t serial, aidl::android::hardware::radio::RadioError errorCode) {
  aidl::android::hardware::radio::RadioResponseInfo responseInfo{
      aidl::android::hardware::radio::RadioResponseType::SOLICITED, serial,
      static_cast<aidl::android::hardware::radio::RadioError>(errorCode) };
  auto respCb = this->getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("handleStkCallSetupRequestFromSimResponse: serial=%d, error=%d", serial,
                    errorCode);
    auto ret = respCb->handleStkCallSetupRequestFromSimResponse(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void IRadioVoiceImpl::sendResponseForHangup(int32_t serial, RIL_Errno errorCode) {
  aidl::android::hardware::radio::RadioResponseInfo responseInfo{
      aidl::android::hardware::radio::RadioResponseType::SOLICITED, serial,
      static_cast<aidl::android::hardware::radio::RadioError>(errorCode) };
  auto respCb = this->getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("hangupConnectionResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->hangupConnectionResponse(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void IRadioVoiceImpl::sendResponseForHangupForegroundResumeBackground(
    int32_t serial, RIL_Errno errorCode) {
  aidl::android::hardware::radio::RadioResponseInfo responseInfo{
      aidl::android::hardware::radio::RadioResponseType::SOLICITED, serial,
      static_cast<aidl::android::hardware::radio::RadioError>(errorCode) };
  auto respCb = this->getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("hangupForegroundResumeBackgroundResponse: serial=%d, error=%d", serial,
                    errorCode);
    auto ret = respCb->hangupForegroundResumeBackgroundResponse(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void IRadioVoiceImpl::sendResponseForHangupWaitingOrBackground(
    int32_t serial, RIL_Errno errorCode) {
  aidl::android::hardware::radio::RadioResponseInfo responseInfo{
      aidl::android::hardware::radio::RadioResponseType::SOLICITED, serial,
      static_cast<aidl::android::hardware::radio::RadioError>(errorCode) };
  auto respCb = this->getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("hangupWaitingOrBackgroundResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->hangupWaitingOrBackgroundResponse(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void IRadioVoiceImpl::sendResponseForRejectCall(int32_t serial, RIL_Errno errorCode) {
  aidl::android::hardware::radio::RadioResponseInfo responseInfo{
      aidl::android::hardware::radio::RadioResponseType::SOLICITED, serial,
      static_cast<aidl::android::hardware::radio::RadioError>(errorCode) };
  auto respCb = this->getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("rejectCallResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->rejectCallResponse(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void IRadioVoiceImpl::sendResponseForSendBurstDtmf(int32_t serial, RIL_Errno errorCode) {
  aidl::android::hardware::radio::RadioResponseInfo responseInfo{
      aidl::android::hardware::radio::RadioResponseType::SOLICITED, serial,
      static_cast<aidl::android::hardware::radio::RadioError>(errorCode) };
  auto respCb = this->getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("sendBurstDtmfResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->sendBurstDtmfResponse(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void IRadioVoiceImpl::sendResponseForSendCDMAFeatureCode(int32_t serial, RIL_Errno errorCode) {
  aidl::android::hardware::radio::RadioResponseInfo responseInfo{
      aidl::android::hardware::radio::RadioResponseType::SOLICITED, serial,
      static_cast<aidl::android::hardware::radio::RadioError>(errorCode) };
  auto respCb = this->getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("sendCdmaFeatureCodeResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->sendCdmaFeatureCodeResponse(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void IRadioVoiceImpl::sendResponseForSendDtmf(int32_t serial, RIL_Errno errorCode) {
  aidl::android::hardware::radio::RadioResponseInfo responseInfo{
      aidl::android::hardware::radio::RadioResponseType::SOLICITED, serial,
      static_cast<aidl::android::hardware::radio::RadioError>(errorCode) };
  auto respCb = this->getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("sendDtmfResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->sendDtmfResponse(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void IRadioVoiceImpl::sendResponseForSendUssd(int32_t serial, RIL_Errno errorCode) {
  aidl::android::hardware::radio::RadioResponseInfo responseInfo{
      aidl::android::hardware::radio::RadioResponseType::SOLICITED, serial,
      static_cast<aidl::android::hardware::radio::RadioError>(errorCode) };
  auto respCb = this->getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("sendUssdResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->sendUssdResponse(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void IRadioVoiceImpl::sendResponseForSeparateConnection(int32_t serial, RIL_Errno errorCode) {
  aidl::android::hardware::radio::RadioResponseInfo responseInfo{
      aidl::android::hardware::radio::RadioResponseType::SOLICITED, serial,
      static_cast<aidl::android::hardware::radio::RadioError>(errorCode) };
  auto respCb = this->getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("separateConnectionResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->separateConnectionResponse(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void IRadioVoiceImpl::sendResponseForSetCallForward(int32_t serial, RIL_Errno errorCode) {
  aidl::android::hardware::radio::RadioResponseInfo responseInfo{
      aidl::android::hardware::radio::RadioResponseType::SOLICITED, serial,
      static_cast<aidl::android::hardware::radio::RadioError>(errorCode) };
  auto respCb = this->getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("setCallForwardResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->setCallForwardResponse(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void IRadioVoiceImpl::sendResponseForSetCallWaiting(int32_t serial, RIL_Errno errorCode) {
  aidl::android::hardware::radio::RadioResponseInfo responseInfo{
      aidl::android::hardware::radio::RadioResponseType::SOLICITED, serial,
      static_cast<aidl::android::hardware::radio::RadioError>(errorCode) };
  auto respCb = this->getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("setCallWaitingResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->setCallWaitingResponse(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void IRadioVoiceImpl::sendResponseForSetClir(int32_t serial, RIL_Errno errorCode) {
  aidl::android::hardware::radio::RadioResponseInfo responseInfo{
      aidl::android::hardware::radio::RadioResponseType::SOLICITED, serial,
      static_cast<aidl::android::hardware::radio::RadioError>(errorCode) };
  auto respCb = this->getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("setClirResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->setClirResponse(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void IRadioVoiceImpl::sendResponseForSetMute(int32_t serial, RIL_Errno errorCode) {
  aidl::android::hardware::radio::RadioResponseInfo responseInfo{
      aidl::android::hardware::radio::RadioResponseType::SOLICITED, serial,
      static_cast<aidl::android::hardware::radio::RadioError>(errorCode) };
  auto respCb = this->getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("setMuteResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->setMuteResponse(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void IRadioVoiceImpl::sendResponseForSetPreferredVoicePrivacy(
    int32_t serial, RIL_Errno errorCode) {
  aidl::android::hardware::radio::RadioResponseInfo responseInfo{
      aidl::android::hardware::radio::RadioResponseType::SOLICITED, serial,
      static_cast<aidl::android::hardware::radio::RadioError>(errorCode) };
  auto respCb = this->getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("setPreferredVoicePrivacyResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->setPreferredVoicePrivacyResponse(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void IRadioVoiceImpl::sendResponseForSetTTYMode(int32_t serial, RIL_Errno errorCode) {
  aidl::android::hardware::radio::RadioResponseInfo responseInfo{
      aidl::android::hardware::radio::RadioResponseType::SOLICITED, serial,
      static_cast<aidl::android::hardware::radio::RadioError>(errorCode) };
  auto respCb = this->getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("setTtyModeResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->setTtyModeResponse(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void IRadioVoiceImpl::sendResponseForStartDtmf(int32_t serial, RIL_Errno errorCode) {
  aidl::android::hardware::radio::RadioResponseInfo responseInfo{
      aidl::android::hardware::radio::RadioResponseType::SOLICITED, serial,
      static_cast<aidl::android::hardware::radio::RadioError>(errorCode) };
  auto respCb = this->getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("startDtmfResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->startDtmfResponse(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void IRadioVoiceImpl::sendResponseForStopDtmf(int32_t serial, RIL_Errno errorCode) {
  aidl::android::hardware::radio::RadioResponseInfo responseInfo{
      aidl::android::hardware::radio::RadioResponseType::SOLICITED, serial,
      static_cast<aidl::android::hardware::radio::RadioError>(errorCode) };
  auto respCb = this->getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("stopDtmfResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->stopDtmfResponse(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void IRadioVoiceImpl::sendResponseForSwitchWaitingOrHoldingAndActive(
    int32_t serial, RIL_Errno errorCode) {
  aidl::android::hardware::radio::RadioResponseInfo responseInfo{
      aidl::android::hardware::radio::RadioResponseType::SOLICITED, serial,
      static_cast<aidl::android::hardware::radio::RadioError>(errorCode) };
  auto respCb = this->getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("switchWaitingOrHoldingAndActiveResponse: serial=%d, error=%d", serial,
                    errorCode);
    auto ret = respCb->switchWaitingOrHoldingAndActiveResponse(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void IRadioVoiceImpl::sendResponseForIsVoNrEnabled(
    int32_t serial, RIL_Errno errorCode, boolean isVonrEnabled) {
  aidl::android::hardware::radio::RadioResponseInfo responseInfo{
      aidl::android::hardware::radio::RadioResponseType::SOLICITED, serial,
      static_cast<aidl::android::hardware::radio::RadioError>(errorCode) };
  auto respCb = this->getResponseCallback();
  if (!respCb) {
    return;
  }
  QCRIL_LOG_DEBUG("isVoNrEnabledResponse: serial=%d, error=%d", serial, errorCode);
  auto ret = respCb->isVoNrEnabledResponse(responseInfo, isVonrEnabled);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void IRadioVoiceImpl::sendResponseForSetVoNrEnabled(int32_t serial, RIL_Errno errorCode) {
  aidl::android::hardware::radio::RadioResponseInfo responseInfo{
      aidl::android::hardware::radio::RadioResponseType::SOLICITED, serial,
      static_cast<aidl::android::hardware::radio::RadioError>(errorCode) };
  auto respCb = this->getResponseCallback();
  if (!respCb) {
    return;
  }
  QCRIL_LOG_DEBUG("setVoNrEnabledResponse: serial=%d, error=%d", serial, errorCode);
  auto ret = respCb->setVoNrEnabledResponse(responseInfo);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}
