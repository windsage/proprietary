/*===========================================================================
   Copyright (c) 2022 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

#pragma once

#include "BnRadioVoiceDefault.h"
#include <framework/legacy.h>
#include <QtiMutex.h>
#include <binder/IBinder.h>
#include "IRadioVoiceContext.h"

#include "telephony/ril.h"
#include "interfaces/RilAcknowledgeRequestMessage.h"

#include <interfaces/voice/QcRilUnsolCallRingingMessage.h>
#include <interfaces/voice/QcRilUnsolCallStateChangeMessage.h>
#include <interfaces/voice/QcRilUnsolCdmaCallWaitingMessage.h>
#include <interfaces/voice/QcRilUnsolCdmaOtaProvisionStatusMessage.h>
#include "interfaces/nas/RilUnsolEmergencyCallbackModeMessage.h"
#include <interfaces/voice/QcRilUnsolRingbackToneMessage.h>
#include <interfaces/voice/QcRilUnsolSupplementaryServiceMessage.h>
#include <interfaces/voice/QcRilUnsolSrvccStatusMessage.h>
#include "interfaces/sms/RilUnsolStkCCAlphaNotifyMessage.h"
#include <interfaces/voice/QcRilUnsolStkCcAlphaNotifyMessage.h>
#include "interfaces/nas/RilUnsolEmergencyListIndMessage.h"
#include <interfaces/voice/QcRilUnsolCdmaInfoRecordMessage.h>
#include "interfaces/gstk/GstkUnsolIndMsg.h"
#include <interfaces/voice/QcRilUnsolOnUssdMessage.h>

#include <interfaces/voice/voice.h>

namespace aidlvoice {
  using namespace aidl::android::hardware::radio::voice;
}

class IRadioVoiceImpl : public aidlvoice::BnRadioVoiceDefault {
 private:
  std::shared_ptr<aidlvoice::IRadioVoiceResponse> mIRadioVoiceResponse;
  std::shared_ptr<aidlvoice::IRadioVoiceIndication> mIRadioVoiceIndication;
  AIBinder_DeathRecipient* mDeathRecipient = nullptr;
  qcril_instance_id_e_type mInstanceId;
  qtimutex::QtiSharedMutex mCallbackLock;

 private:
  std::shared_ptr<aidlvoice::IRadioVoiceResponse> getResponseCallback();
  std::shared_ptr<aidlvoice::IRadioVoiceIndication> getIndicationCallback();

 public:
  IRadioVoiceImpl(qcril_instance_id_e_type instance);
  ~IRadioVoiceImpl();

  void setResponseFunctions_nolock(
      const std::shared_ptr<aidlvoice::IRadioVoiceResponse>& radioVoiceResponse,
      const std::shared_ptr<aidlvoice::IRadioVoiceIndication>& radioVoiceIndication);

  void clearCallbacks();

  qcril_instance_id_e_type getInstanceId() {
    return mInstanceId;
  }

  std::shared_ptr<IRadioVoiceContext> getContext(uint32_t serial) {
    std::shared_ptr<IRadioVoiceContext> ctx = std::make_shared<IRadioVoiceContext>(mInstanceId, serial);
    return ctx;
  }

  void notifyVoiceClientConnected();
  void deathNotifier();

  // AIDL APIs
  ::ndk::ScopedAStatus setResponseFunctions(
      const std::shared_ptr<aidlvoice::IRadioVoiceResponse>& in_radioVoiceResponse,
      const std::shared_ptr<aidlvoice::IRadioVoiceIndication>& in_radioVoiceIndication
  ) override;

  ::ndk::ScopedAStatus acceptCall(int32_t in_serial) override;
  ::ndk::ScopedAStatus cancelPendingUssd(int32_t in_serial) override;
  ::ndk::ScopedAStatus conference(int32_t in_serial) override;
  ::ndk::ScopedAStatus dial(int32_t in_serial, const aidlvoice::Dial& in_dialInfo) override;
  ::ndk::ScopedAStatus emergencyDial(int32_t in_serial, const aidlvoice::Dial& in_dialInfo, int32_t in_categories, const std::vector<std::string>& in_urns, aidlvoice::EmergencyCallRouting in_routing, bool in_hasKnownUserIntentEmergency, bool in_isTesting) override;
  ::ndk::ScopedAStatus exitEmergencyCallbackMode(int32_t in_serial) override;
  ::ndk::ScopedAStatus explicitCallTransfer(int32_t in_serial) override;
  ::ndk::ScopedAStatus getCallForwardStatus(int32_t in_serial, const aidlvoice::CallForwardInfo& in_callInfo) override;
  ::ndk::ScopedAStatus getCallWaiting(int32_t in_serial, int32_t in_serviceClass) override;
  ::ndk::ScopedAStatus getClip(int32_t in_serial) override;
  ::ndk::ScopedAStatus getClir(int32_t in_serial) override;
  ::ndk::ScopedAStatus getCurrentCalls(int32_t in_serial) override;
  ::ndk::ScopedAStatus getLastCallFailCause(int32_t in_serial) override;
  ::ndk::ScopedAStatus getMute(int32_t in_serial) override;
  ::ndk::ScopedAStatus getPreferredVoicePrivacy(int32_t in_serial) override;
  ::ndk::ScopedAStatus getTtyMode(int32_t in_serial) override;
  ::ndk::ScopedAStatus handleStkCallSetupRequestFromSim(int32_t in_serial, bool in_accept) override;
  ::ndk::ScopedAStatus hangup(int32_t in_serial, int32_t in_gsmIndex) override;
  ::ndk::ScopedAStatus hangupForegroundResumeBackground(int32_t in_serial) override;
  ::ndk::ScopedAStatus hangupWaitingOrBackground(int32_t in_serial) override;
  ::ndk::ScopedAStatus rejectCall(int32_t in_serial) override;
  ::ndk::ScopedAStatus responseAcknowledgement() override;
  ::ndk::ScopedAStatus sendBurstDtmf(int32_t in_serial, const std::string& in_dtmf, int32_t in_on, int32_t in_off) override;
  ::ndk::ScopedAStatus sendCdmaFeatureCode(int32_t in_serial, const std::string& in_featureCode) override;
  ::ndk::ScopedAStatus sendDtmf(int32_t in_serial, const std::string& in_s) override;
  ::ndk::ScopedAStatus sendUssd(int32_t in_serial, const std::string& in_ussd) override;
  ::ndk::ScopedAStatus separateConnection(int32_t in_serial, int32_t in_gsmIndex) override;
  ::ndk::ScopedAStatus setCallForward(int32_t in_serial, const aidlvoice::CallForwardInfo& in_callInfo) override;
  ::ndk::ScopedAStatus setCallWaiting(int32_t in_serial, bool in_enable, int32_t in_serviceClass) override;
  ::ndk::ScopedAStatus setClir(int32_t in_serial, int32_t in_status) override;
  ::ndk::ScopedAStatus setMute(int32_t in_serial, bool in_enable) override;
  ::ndk::ScopedAStatus setPreferredVoicePrivacy(int32_t in_serial, bool in_enable) override;
  ::ndk::ScopedAStatus setTtyMode(int32_t in_serial, aidlvoice::TtyMode in_mode) override;
  ::ndk::ScopedAStatus startDtmf(int32_t in_serial, const std::string& in_s) override;
  ::ndk::ScopedAStatus stopDtmf(int32_t in_serial) override;
  ::ndk::ScopedAStatus switchWaitingOrHoldingAndActive(int32_t in_serial) override;
  ::ndk::ScopedAStatus isVoNrEnabled(int32_t in_serial) override;
  ::ndk::ScopedAStatus setVoNrEnabled(int32_t in_serial, bool in_enable) override;

  // Unsol message handlers
  int callRing(std::shared_ptr<QcRilUnsolCallRingingMessage> msg);
  int callStateChanged(std::shared_ptr<QcRilUnsolCallStateChangeMessage> msg);
  int sendCdmaCallWaiting(std::shared_ptr<QcRilUnsolCdmaCallWaitingMessage> msg);
  int sendCdmaInfoRec(std::shared_ptr<QcRilUnsolCdmaInfoRecordMessage> msg);
  int sendCdmaOtaProvisionStatus(std::shared_ptr<QcRilUnsolCdmaOtaProvisionStatusMessage> msg);
  int sendEmergencyCallbackMode(std::shared_ptr<RilUnsolEmergencyCallbackModeMessage> msg);
  int sendIndicateRingbackTone(std::shared_ptr<QcRilUnsolRingbackToneMessage> msg);
  int sendOnSupplementaryServiceIndication(
      std::shared_ptr<QcRilUnsolSupplementaryServiceMessage> msg);
  int sendSrvccStateNotify(std::shared_ptr<QcRilUnsolSrvccStatusMessage> msg);
  int sendStkCCAlphaNotify(std::shared_ptr<RilUnsolStkCCAlphaNotifyMessage> msg);
  int sendCurrentEmergencyNumberList(std::shared_ptr<RilUnsolEmergencyListIndMessage> msg);
  int sendGstkIndication(std::shared_ptr<GstkUnsolIndMsg> msg);
  int sendOnUssd(std::shared_ptr<QcRilUnsolOnUssdMessage> msg);

 private:
  // Send response on IRadioVoiceResponse
  void sendResponseForAcceptCall(int32_t serial, RIL_Errno errorCode);
  int sendAcknowledgeRequest(std::shared_ptr<RilAcknowledgeRequestMessage> msg);
  void sendResponseForCancelPendingUssd(int32_t serial, RIL_Errno errorCode);
  void sendResponseForConference(int32_t serial, RIL_Errno errorCode);
  void sendResponseForDial(int32_t serial, RIL_Errno errorCode);
  void sendResponseForEmergencyDial(int32_t serial, RIL_Errno errorCode);
  void sendResponseForExitEmergencyCallbackMode(int32_t serial, RIL_Errno errorCode);
  void sendResponseForExplicitCallTransfer(int32_t serial, RIL_Errno errorCode);
  void sendResponseForGetCallForwardStatus(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::GetCallForwardRespData> data);
  void sendResponseForGetCallWaiting(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::CallWaitingInfo> cwInfo);
  void sendResponseForGetClip(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::ClipInfo> clipInfo);
  void sendResponseForGetClir(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::ClirInfo> clirInfo);
  void sendResponseForGetCurrentCalls(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::GetCurrentCallsRespData> currentCalls);
  void sendResponseForGetLastCallFailCause(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::LastCallFailCauseInfo> failCause);
  void sendResponseForGetMute(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::MuteInfo> muteInfo);
  void sendResponseForGetPreferredVoicePrivacy(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::PrivacyModeResp> privacyModeResp);
  void sendResponseForGetTTYMode(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::TtyModeResp> ttyModeResp);
  void sendResponseForHandleStkCallSetupRequestFromSim(int32_t serial,
    aidl::android::hardware::radio::RadioError errorCode);
  void sendResponseForHangup(int32_t serial, RIL_Errno errorCode);
  void sendResponseForHangupForegroundResumeBackground(int32_t serial, RIL_Errno errorCode);
  void sendResponseForHangupWaitingOrBackground(int32_t serial, RIL_Errno errorCode);
  void sendResponseForRejectCall(int32_t serial, RIL_Errno errorCode);
  void sendResponseForSendBurstDtmf(int32_t serial, RIL_Errno errorCode);
  void sendResponseForSendCDMAFeatureCode(int32_t serial, RIL_Errno errorCode);
  void sendResponseForSendDtmf(int32_t serial, RIL_Errno errorCode);
  void sendResponseForSendUssd(int32_t serial, RIL_Errno errorCode);
  void sendResponseForSeparateConnection(int32_t serial, RIL_Errno errorCode);
  void sendResponseForSetCallForward(int32_t serial, RIL_Errno errorCode);
  void sendResponseForSetCallWaiting(int32_t serial, RIL_Errno errorCode);
  void sendResponseForSetClir(int32_t serial, RIL_Errno errorCode);
  void sendResponseForSetMute(int32_t serial, RIL_Errno errorCode);
  void sendResponseForSetPreferredVoicePrivacy(int32_t serial, RIL_Errno errorCode);
  void sendResponseForSetTTYMode(int32_t serial, RIL_Errno errorCode);
  void sendResponseForStartDtmf(int32_t serial, RIL_Errno errorCode);
  void sendResponseForStopDtmf(int32_t serial, RIL_Errno errorCode);
  void sendResponseForSwitchWaitingOrHoldingAndActive(int32_t serial, RIL_Errno errorCode);
  void sendResponseForIsVoNrEnabled(int32_t serial, RIL_Errno errorCode, boolean isVonrEnabled);
  void sendResponseForSetVoNrEnabled(int32_t serial, RIL_Errno errorCode);
};
