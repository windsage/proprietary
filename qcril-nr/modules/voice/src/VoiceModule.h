/******************************************************************************
#  Copyright (c) 2017,2020, 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/Module.h"
#include "framework/legacy.h"
#include "framework/QcrilInitMessage.h"
#include "framework/QcrilImsClientConnected.h"
#include <interfaces/voice/QcrilVoiceClientConnected.h>
#include <framework/AddPendingMessageList.h>
#include <qtibus/Messenger.h>

#include <interfaces/audio/QcRilAudioManagerReadyMessage.h>
#include <interfaces/voice/QcRilRequestSendDtmfMessage.h>
#include <interfaces/voice/QcRilRequestStartDtmfMessage.h>
#include <interfaces/voice/QcRilRequestStopDtmfMessage.h>
#include <interfaces/voice/QcRilRequestCancelUssdMessage.h>
#include <interfaces/voice/QcRilRequestDialMessage.h>
#include <interfaces/voice/QcRilRequestSetTtyModeMessage.h>
#include <interfaces/voice/QcRilRequestSetPreferredVoicePrivacyMessage.h>
#include <interfaces/voice/QcRilRequestSendSipDtmfMessage.h>
#include <interfaces/voice/QcRilRequestSendUssdMessage.h>
#include <interfaces/voice/QcRilRequestGetTtyModeMessage.h>
#include <interfaces/voice/QcRilRequestGetPreferredVoicePrivacyMessage.h>
#include <interfaces/voice/QcRilRequestGetCurrentCallsMessage.h>
#include <interfaces/voice/QcRilRequestConferenceMessage.h>
#include <interfaces/voice/QcRilRequestAnswerMessage.h>
#include <interfaces/voice/QcRilRequestLastCallFailCauseMessage.h>
#include <interfaces/voice/QcRilRequestCdmaFlashMessage.h>
#include <interfaces/voice/QcRilRequestExplicitCallTransferMessage.h>
#include <interfaces/voice/QcRilRequestCdmaBurstDtmfMessage.h>
#include <interfaces/voice/QcRilRequestGetCallWaitingMessage.h>
#include <interfaces/voice/QcRilRequestGetClipMessage.h>
#include <interfaces/voice/QcRilRequestGetClirMessage.h>
#include <interfaces/voice/QcRilRequestGetColrMessage.h>
#include <interfaces/voice/QcRilRequestHangupForegroundResumeBackgroundMessage.h>
#include <interfaces/voice/QcRilRequestHangupMessage.h>
#include <interfaces/voice/QcRilRequestHangupWaitingOrBackgroundMessage.h>
#include <interfaces/voice/QcRilRequestQueryCallForwardMessage.h>
#include <interfaces/voice/QcRilRequestQueryColpMessage.h>
#include <interfaces/voice/QcRilRequestGetCallBarringMessage.h>
#include <interfaces/voice/QcRilRequestSeparateConnectionMessage.h>
#include <interfaces/voice/QcRilRequestSetCallBarringPasswordMessage.h>
#include <interfaces/voice/QcRilRequestSetCallForwardMessage.h>
#include <interfaces/voice/QcRilRequestSetCallWaitingMessage.h>
#include <interfaces/voice/QcRilRequestSetClirMessage.h>
#include <interfaces/voice/QcRilRequestSetColrMessage.h>
#include <interfaces/voice/QcRilRequestSetSuppSvcNotificationMessage.h>
#include <interfaces/voice/QcRilRequestSetSupsServiceMessage.h>
#include <interfaces/voice/QcRilRequestSwitchWaitingOrHoldingAndActiveMessage.h>
#include <interfaces/voice/QcRilRequestUdubMessage.h>
#include <interfaces/voice/QcRilRequestRejectIncomingCallMessage.h>
#include <interfaces/voice/QcRilRequestSetupAnswerMessage.h>
#include <interfaces/voice/QcRilRequestGetCurrentSetupCallsMessage.h>
#include <interfaces/voice/QcRilRequestSetMuteMessage.h>
#include <interfaces/voice/QcRilRequestGetMuteMessage.h>
#include <interfaces/voice/QcRilRequestSetAudioServiceStatusMessage.h>
#include <interfaces/voice/QcRilRequestSendVosActionInfoMessage.h>
#include <interfaces/voice/QcRilRequestSendVosSupportStatusMessage.h>
#include <interfaces/uim/UimSimStatusChangedInd.h>
#include "interfaces/common/QcRilUnsolOperationalStatusMessage.h"
#include <interfaces/voice/QcRilRequestImsSendDtmfMessage.h>
#include <interfaces/voice/QcRilRequestImsStartDtmfMessage.h>
#include <interfaces/voice/QcRilRequestImsStopDtmfMessage.h>
#include <interfaces/voice/QcRilRequestImsResumeMessage.h>
#include <interfaces/voice/QcRilUnsolImsConferenceCallStateCompletedMessage.h>

#include "modules/voice/VoiceLimitIndMessage.h"
#include "modules/voice/VoiceCheckSpecificCallMessage.h"
#include "modules/voice/VoiceRteProcessPendingCallMessage.h"
#include "modules/voice/VoiceCheckCallPresentMessage.h"
#include "modules/voice/VoiceImsWaveRadioStateMessage.h"
#include "modules/voice/VoiceGetConfigMessage.h"
#include "modules/voice/VoiceSetConfigMessage.h"
#include "modules/voice/IpcCallStatusMessage.h"
#include "modules/voice/IpcDisconnectCallsMessage.h"
#include "modules/voice/EndCallListMessage.h"
#include "modules/voice/AudioManagementEventMessage.h"
#include "modules/voice/TriggerPendingEmergencyCallMessage.h"
#include "modules/voice/VoiceMakeIncomingCallRingMessage.h"
#include "modules/qmi/VoiceModemEndPoint.h"
#include "modules/qmi/QmiAsyncResponseMessage.h"
#include "modules/voice/IpcMultiSimCapabilityMessage.h"
#include "modules/voice/IpcCallStatusMessage.h"
#include "modules/voice/IpcReleaseAudioMessage.h"
#include "modules/voice/IpcReleaseAudioCnfMessage.h"
#include "modules/voice/ReleaseAudioRequestMessage.h"
#include "modules/nas/NasVoiceSystemIdInd.h"
#include "modules/nas/NasPowerOnOptUiReadyIndMessage.h"
#include "modules/android/ClientConnectedMessage.h"

#include "VoiceRequestPendingDialMessage.h"
#include "FlashActivateHoldAnswerMessage.h"
#include "qcril_qmi_voice.h"
#include <string.h>

#define MAX_ICCID_CLIR_ENTRIES 10
// default_clir is defined here to avoid long lines
#define DEFAULT_CLIR QCRIL_QMI_VOICE_SS_CLIR_PRESENTATION_INDICATOR

enum class VoiceFeatureType {
  REPORT_SPEECH_CODEC = 0,
  SUBADDRESS_SUPPORT = 1,
  SUBADDRESS_SUPPORT_AMP = 2,
  REJECT_CAUSE_21_SUPPORTED = 3,
  SUBADDRESS_IA5_ID_SUPPORT = 4,
  REDIR_PARTY_NUM_SUPPORT = 5,
  FAC_NOT_SUPP_AS_NO_NW = 6,
  ENCRYPTED_CALLS_SUPPORT = 7,
  FAKE_AUDIO_UP = 8,
  SET_ALL_CALL_FORWARD_SUPPORTED = 9,
  PROCESS_SUPS_IND = 10,  // For STK CC cases
  IS_0x9E_NOT_TREAT_AS_NAME = 11,
  IS_ORIGINAL_NUMBER_DISPLAY = 12,
  UE_BASED_CLIR = 13,
};

class VoiceModule : public Module, public AddPendingMessageList {
 private:
  bool mInitialized;
  uint32_t mVoiceFeaturesMask = 0;

  // Uplink (microphone) mute settings
  bool mUplinkMute = false;

  bool mIsAudioReleased = false;
  qtimutex::QtiSharedMutex mIsAudioReleasedLock;
  std::string mIccid; //cached iccid
  qtimutex::QtiSharedMutex mIccidLock;

 public:
  VoiceModule();
  VoiceModule(string name);
  ~VoiceModule();
  void init();
#ifdef QMI_RIL_UTF
  virtual void cleanup();
#endif

  void initFeatureMask();
  bool getFeature(VoiceFeatureType feature);
  void setFeature(VoiceFeatureType feature, bool enable);
  bool setAudioStateBeforeCallStatusReport() {
    // Return true if any calls present in the other RILD
    return mCallsPresentOnOtherRild;
  }
  bool setAudioStateAfterCallStatusReport() {
    return !setAudioStateBeforeCallStatusReport();
  }
  bool isReleaseAudioOnOtherSubRequired();
  void setIsAudioReleased(bool value);
  bool getIsAudioReleased();
  void setIccid(std::string iccid);
  std::string getIccid();
  void processAnswerResponse(CommonVoiceResponseData *data);
  void processAnswerCommandOversightCompletion(CommonVoiceResponseData *data);
  bool isDsdsTransitionMode();
  bool callOnBothSubs();

 protected:
  bool mCallsPresentOnOtherRild = false;
  qcril::interfaces::MultiSimVoiceCapability mMultiSimVoiceCap =
    qcril::interfaces::MultiSimVoiceCapability::UNKNOWN;
   void handleEndCallListMessage(std::shared_ptr<EndCallListMessage> msg);
   //handle IpcDisconnectCallsMessage
  void handleIpcDisconnectCallsMessage(std::shared_ptr<IpcDisconnectCallsMessage> msg);

  bool isCallPresentOnOtherSub()
  {
    return mCallsPresentOnOtherRild;
  }
  bool isDeviceInDsdaMode()
  {
    return (mMultiSimVoiceCap == qcril::interfaces::MultiSimVoiceCapability::DSDA);
  }
  bool isDeviceInDsdsMode()
  {
    return (mMultiSimVoiceCap == qcril::interfaces::MultiSimVoiceCapability::DSDS);
  }
  RIL_Errno dispatchReleaseAudio(std::shared_ptr<QcRilRequestMessage> msg);
  void handleQcRilRequestResumeMessage(std::shared_ptr<QcRilRequestImsResumeMessage> msg);

 private:
  // handle QMI IND
  void handleVoiceQmiIndMessage(std::shared_ptr<Message> msg);
  // handle QmiVoiceNotifyClirModeIndMessage
  void handleQmiVoiceNotifyClirModeIndMessage(std::shared_ptr<QmiVoiceNotifyClirModeIndMessage> msg);
  // handle QmiVoiceNetworkDetectedEccNumberIndMessage
  void handleQmiVoiceNetworkDetectedEccNumberIndMessage(
      std::shared_ptr<QmiVoiceNetworkDetectedEccNumberIndMessage> msg);
  // handle QMI ASYNC RESP
  void handleQmiAsyncRespMessage(std::shared_ptr<QmiAsyncResponseMessage> msg);
  // handle End Point Status Indication
  void handleVoiceEndpointStatusIndMessage(std::shared_ptr<Message> msg);
  // handle Qcril Init Message
  void handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg);
  // handle QcrilVoiceClientConnected
  void handleQcrilVoiceClientConnected(std::shared_ptr<QcrilVoiceClientConnected> msg);
  // handle QcrilImsClientConnected
  void handleQcrilImsClientConnected(std::shared_ptr<QcrilImsClientConnected> msg);
  // handle IpcCallStatusMessage
  void handleIpcCallStatusMessage(std::shared_ptr<IpcCallStatusMessage> msg);
  // handle AudioManagementEventMessage
  void handleAudioManagementEventMessage(std::shared_ptr<AudioManagementEventMessage> msg);
  // handle NasVoiceSystemIdInd
  void handleNasVoiceSystemIdInd(std::shared_ptr<NasVoiceSystemIdInd> msg);
  // handle NasPowerOnOptUiReadyIndMessage
  void handleNasPowerOnOptUiReadyIndMessage(std::shared_ptr<NasPowerOnOptUiReadyIndMessage> msg);
  // handle QcRilUnsolOperationalStatusMessage
  void handleUnsolOperationalStatusMessage(std::shared_ptr<QcRilUnsolOperationalStatusMessage> msg);


  // RIL requests
  void handleQcRilRequestDialMessageWrapper(std::shared_ptr<QcRilRequestDialMessage> msg);
  void handleQcRilRequestGetCurrentCallsMessage(
      std::shared_ptr<QcRilRequestGetCurrentCallsMessage> msg);
  void handleQcRilRequestLastCallFailCauseMessage(
      std::shared_ptr<QcRilRequestLastCallFailCauseMessage> msg);
  void handleQcRilRequestAnswerMessageWrapper(std::shared_ptr<QcRilRequestAnswerMessage> msg);
  void handleQcRilRequestHangupMessage(std::shared_ptr<QcRilRequestHangupMessage> msg);
  void handleQcRilRequestCdmaFlashMessage(std::shared_ptr<QcRilRequestCdmaFlashMessage> msg);
  void handleQcRilRequestGetPreferredVoicePrivacyMessage(
      std::shared_ptr<QcRilRequestGetPreferredVoicePrivacyMessage> msg);
  void handleQcRilRequestSetPreferredVoicePrivacyMessage(
      std::shared_ptr<QcRilRequestSetPreferredVoicePrivacyMessage> msg);
  void handleQcRilRequestSendDtmfMessage(std::shared_ptr<QcRilRequestSendDtmfMessage> msg);
  void handleQcRilRequestStartDtmfMessage(std::shared_ptr<QcRilRequestStartDtmfMessage> msg);
  void handleQcRilRequestStopDtmfMessage(std::shared_ptr<QcRilRequestStopDtmfMessage> msg);
  void handleQcRilRequestCancelUssdMessage(std::shared_ptr<QcRilRequestCancelUssdMessage> msg);
  void handleQcRilRequestSendUssdMessage(std::shared_ptr<QcRilRequestSendUssdMessage> msg);
  void handleQcRilRequestSetTtyModeMessage(std::shared_ptr<QcRilRequestSetTtyModeMessage> msg);
  void handleQcRilRequestGetTtyModeMessage(std::shared_ptr<QcRilRequestGetTtyModeMessage> msg);
  void handleQcRilRequestSetSuppSvcNotificationMessage(
      std::shared_ptr<QcRilRequestSetSuppSvcNotificationMessage> msg);
  void handleQcRilRequestGetClipMessage(std::shared_ptr<QcRilRequestGetClipMessage> msg);
  void handleQcRilRequestGetClirMessage(std::shared_ptr<QcRilRequestGetClirMessage> msg);
  void handleQcRilRequestSetClirMessage(std::shared_ptr<QcRilRequestSetClirMessage> msg);
  void handleQcRilRequestGetColrMessage(std::shared_ptr<QcRilRequestGetColrMessage> msg);
  void handleQcRilRequestSetColrMessage(std::shared_ptr<QcRilRequestSetColrMessage> msg);
  void handleQcRilRequestQueryCallForwardMessage(
      std::shared_ptr<QcRilRequestQueryCallForwardMessage> msg);
  void handleQcRilRequestSetCallForwardMessage(
      std::shared_ptr<QcRilRequestSetCallForwardMessage> msg);
  void handleQcRilRequestGetCallWaitingMessage(
      std::shared_ptr<QcRilRequestGetCallWaitingMessage> msg);
  void handleQcRilRequestSetCallWaitingMessage(
      std::shared_ptr<QcRilRequestSetCallWaitingMessage> msg);
  void handleQcRilRequestSetSupsServiceMessage(
      std::shared_ptr<QcRilRequestSetSupsServiceMessage> msg);
  void handleQcRilRequestQueryColpMessage(
      std::shared_ptr<QcRilRequestQueryColpMessage> msg);
  void handleQcRilRequestGetCallBarringMessage(
      std::shared_ptr<QcRilRequestGetCallBarringMessage> msg);
  void handleQcRilRequestSetCallBarringPasswordMessage(
      std::shared_ptr<QcRilRequestSetCallBarringPasswordMessage> msg);
  void handleQcRilRequestConferenceMessage(
      std::shared_ptr<QcRilRequestConferenceMessage> msg);
  void handleQcRilRequestHangupForegroundResumeBackgroundMessage(
      std::shared_ptr<QcRilRequestHangupForegroundResumeBackgroundMessage> msg);
  void handleQcRilRequestHangupWaitingOrBackgroundMessage(
      std::shared_ptr<QcRilRequestHangupWaitingOrBackgroundMessage> msg);
  void handleQcRilRequestSwitchWaitingOrHoldingAndActiveMessage(
      std::shared_ptr<QcRilRequestSwitchWaitingOrHoldingAndActiveMessage> msg);
  void handleQcRilRequestUdubMessage(
      std::shared_ptr<QcRilRequestUdubMessage> msg);
  void handleQcRilRequestSeparateConnectionMessage(
      std::shared_ptr<QcRilRequestSeparateConnectionMessage> msg);
  void handleQcRilRequestExplicitCallTransferMessage(
      std::shared_ptr<QcRilRequestExplicitCallTransferMessage> msg);
  void handleQcRilRequestCdmaBurstDtmfMessage(
      std::shared_ptr<QcRilRequestCdmaBurstDtmfMessage> msg);
  void handleQcRilRequestRejectIncomingCallMessage(
      std::shared_ptr<QcRilRequestRejectIncomingCallMessage> msg);
  void handleQcRilRequestGetCurrentSetupCallsMessage(
      std::shared_ptr<QcRilRequestGetCurrentSetupCallsMessage> msg);
  void handleQcRilRequestSetupAnswerMessage(
      std::shared_ptr<QcRilRequestSetupAnswerMessage> msg);
  void handleQcRilRequestSetMuteMessage(
      std::shared_ptr<QcRilRequestSetMuteMessage> msg);
  void handleQcRilRequestGetMuteMessage(
      std::shared_ptr<QcRilRequestGetMuteMessage> msg);
  void handleQcRilRequestSetAudioServiceStatusMessage(
      std::shared_ptr<QcRilRequestSetAudioServiceStatusMessage> msg);
  void handleQcRilRequestSendSipDtmfMessage(
      std::shared_ptr<QcRilRequestSendSipDtmfMessage> msg);
  void handleUimSimStatusChangedInd (
      std::shared_ptr<UimSimStatusChangedInd> msg);
  void handleQcRilRequestSendVosSupportStatusMessage(
      std::shared_ptr<QcRilRequestSendVosSupportStatusMessage> msg);
  void handleQcRilRequestSendVosActionInfoMessage(
      std::shared_ptr<QcRilRequestSendVosActionInfoMessage> msg);
  void handleQcRilAudioManagerReadyMessage(
      std::shared_ptr<QcRilAudioManagerReadyMessage> msg);

  // public APIs
  void handleLimitedIndMessage(std::shared_ptr<VoiceLimitIndMessage> msg);
  void handleCheckSpecificCallMessage(std::shared_ptr<VoiceCheckSpecificCallMessage> msg);
  void handleRteProcessPendingCall(std::shared_ptr<VoiceRteProcessPendingCallMessage> msg);
  void handleCheckCallPresentMessage(std::shared_ptr<VoiceCheckCallPresentMessage> msg);
  void handleVoiceRequestPendingDialMessage(std::shared_ptr<VoiceRequestPendingDialMessage> msg);
  void handleVoiceImsWaveRadioStateMessage(std::shared_ptr<VoiceImsWaveRadioStateMessage> msg);
  void handleGetConfigMessage(std::shared_ptr<VoiceGetConfigMessage> msg);
  void handleSetConfigMessage(std::shared_ptr<VoiceSetConfigMessage> msg);
  void handleFlashActivateHoldAnswerMessage(std::shared_ptr<FlashActivateHoldAnswerMessage> msg);
  void handleTriggerPendingEmergencyCallMessage(
      std::shared_ptr<TriggerPendingEmergencyCallMessage> msg);

  // Voice internal event
  void handleVoiceMakeIncomingCallRingMessage(
      std::shared_ptr<VoiceMakeIncomingCallRingMessage> msg);

  void processDialCallResponse(CommonVoiceResponseData *data);
  void processHangupResponse(CommonVoiceResponseData *data);
  void processReleaseParticipantResponse(CommonVoiceResponseData *data);
  void processStartContDtmfResponse(CommonVoiceResponseData *data);
  void processStopContDtmfResponse(CommonVoiceResponseData *data);
  void processReleaseHeldOrWaitingResponse(CommonVoiceResponseData *data);
  void processReleaseActiveAcceptHeldOrWaitingResponse(CommonVoiceResponseData *data);
  void processHoldActiveAcceptWaitingOrHeldResponse(CommonVoiceResponseData *data);
  void processVoipHoldActiveAcceptWaitingOrHeldResponse(CommonVoiceResponseData *data);
  void processVoipHangupWaitingOrBackgroundResponse(CommonVoiceResponseData *data);
  void processVoipHangupForegroundResumeBackgroundResponse(CommonVoiceResponseData *data);
  void processCommonManageCallsResponse(CommonVoiceResponseData *data);
  void processSetupAnswerResponse(CommonVoiceResponseData *data);
  void processQueryCallForwardResponse(CommonVoiceResponseData *data);
  void processGetCallWaitingResponse(CommonVoiceResponseData *data);
  void processGetClipResponse(CommonVoiceResponseData *data);
  void processGetClirResponse(CommonVoiceResponseData *data);
  void processSetClirResponse(CommonVoiceResponseData* data);
  void processGetColrResponse(CommonVoiceResponseData *data);
  void processGetColpResponse(CommonVoiceResponseData *data);
  void processGetCallBarringResponse(CommonVoiceResponseData *data);
  void processSetSupsServiceResponse(CommonVoiceResponseData *data);
  void processSetCallFwdSupsReqResponse(CommonVoiceResponseData *data);
  void processSetAllCallFwdSupsReqResponse(CommonVoiceResponseData *data);
  void processOrigUssdResponse(CommonVoiceResponseData *data);
  void processAnswerUssdResponse(CommonVoiceResponseData *data);
  void processCancelUssdResponse(CommonVoiceResponseData *data);
  void processSetSuppSvcNotificationResponse(CommonVoiceResponseData *data);
  void processSetCallBarringPasswordResponse(CommonVoiceResponseData *data);
  void processGetConfigResponse(CommonVoiceResponseData *data);
  void processSetConfigResponse(CommonVoiceResponseData *data);
  void processBurstDtmfResponse(CommonVoiceResponseData *data);
  void processSendFlashResponse(CommonVoiceResponseData *data);
  void processGetPreferredVoicePrivacyResponse(CommonVoiceResponseData *data);
  void processSetPreferredVoicePrivacyResponse(CommonVoiceResponseData *data);
  void processSetLocalCallHoldResponse(CommonVoiceResponseData *data);

  void processDialCommandOversightCompletion(CommonVoiceResponseData *data);
  void processHangupCommandOversightCompletion(CommonVoiceResponseData *data);
  void processReleaseHeldOrWaitingCommandOversightCompletion(CommonVoiceResponseData *data);
  void processSetupAnswerCommandOversightCompletion(CommonVoiceResponseData *data);

  void processSipDtmfResponse(CommonVoiceResponseData *data);
  void processExplicitCallTransferResponse(CommonVoiceResponseData* data);

  void processConferenceResponse(CommonVoiceResponseData* data);
  void notifyConferenceCallStateCompleted();
  void handleQcRilRequestDialMessage(std::shared_ptr<QcRilRequestDialMessage> msg);
  void handleQcRilRequestAnswerMessage(std::shared_ptr<QcRilRequestAnswerMessage> msg);
  void handleIpcReleaseAudioMessage(std::shared_ptr<IpcReleaseAudioMessage> msg);
  void handleIpcReleaseAudioCnfMessage(std::shared_ptr<IpcReleaseAudioCnfMessage> msg);
  void handleReleaseAudioRequestMessage(std::shared_ptr<ReleaseAudioRequestMessage> msg);

  void processReleaseAudioResponse(CommonVoiceResponseData* data);
  void processResumeResponse(CommonVoiceResponseData* data);
  int lookupClirPref(std::string iccid, uint32_t *clir, bool *found);
  int addIccidTableEntry (std::string iccid);
  int addIccidTableEntry (std::string iccid, uint32_t clir);
  int removeIccidTableEntry(std::string iccid);
  int updateIccidTableEntry(std::string iccid, uint32_t clir);
  void processSendVosSupportStatusResponse(CommonVoiceResponseData *data);
  void processSendVosActionInfoResponse(CommonVoiceResponseData *data);
};

std::shared_ptr<VoiceModule> getVoiceModule();
