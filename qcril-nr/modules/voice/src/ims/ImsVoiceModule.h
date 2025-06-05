/******************************************************************************
#  Copyright (c) 2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include <framework/Module.h>
#include <framework/PendingMessageList.h>
#include <framework/QcrilInitMessage.h>
#include <interfaces/voice/QcRilRequestImsAddParticipantMessage.h>
#include <interfaces/voice/QcRilRequestImsCancelModifyCallMessage.h>
#include <interfaces/voice/QcRilRequestImsDeflectCallMessage.h>
#include <interfaces/voice/QcRilRequestImsHoldMessage.h>
#include <interfaces/voice/QcRilRequestImsModifyCallConfirmMessage.h>
#include <interfaces/voice/QcRilRequestImsModifyCallInitiateMessage.h>
#include <interfaces/voice/QcRilRequestImsResumeMessage.h>
#include <interfaces/voice/QcRilRequestImsSendDtmfMessage.h>
#include <interfaces/voice/QcRilRequestImsSetUiTtyModeMessage.h>
#include <interfaces/voice/QcRilRequestImsStartDtmfMessage.h>
#include <interfaces/voice/QcRilRequestImsStopDtmfMessage.h>
#include <interfaces/voice/QcRilRequestQueryCallForwardMessage.h>
#include <interfaces/voice/QcRilRequestSetCallForwardMessage.h>
#include <interfaces/ims/QcRilUnsolImsMultiSimVoiceCapabilityChanged.h>
#include "modules/voice/IpcMultiSimCapabilityMessage.h"
#include "VoiceModule.h"
#include "qcril_qmi_voice.h"

class ImsVoiceModule : public VoiceModule {
 private:
  // Time gap between DTMF START and STOP
  int mDtmfInterval = QCRIL_QMI_VOICE_DTMF_INTERVAL_VAL;

 public:
  ImsVoiceModule();
  ~ImsVoiceModule();
  void init();

 private:
  void handleQcRilRequestImsHoldMessage(std::shared_ptr<QcRilRequestImsHoldMessage> msg);
  void handleQcRilRequestImsResumeMessageWrapper(std::shared_ptr<QcRilRequestImsResumeMessage> msg);
  void handleQcRilRequestImsAddParticipantMessage(
      std::shared_ptr<QcRilRequestImsAddParticipantMessage> msg);
  void handleQcRilRequestImsModifyCallInitiateMessage(
      std::shared_ptr<QcRilRequestImsModifyCallInitiateMessage> msg);
  void handleQcRilRequestImsCancelModifyCallMessage(
      std::shared_ptr<QcRilRequestImsCancelModifyCallMessage> msg);
  void handleQcRilRequestImsModifyCallConfirmMessage(
      std::shared_ptr<QcRilRequestImsModifyCallConfirmMessage> msg);
  void handleQcRilRequestImsDeflectCallMessage(
      std::shared_ptr<QcRilRequestImsDeflectCallMessage> msg);
  void handleQcRilRequestImsSetUiTtyModeMessage(
      std::shared_ptr<QcRilRequestImsSetUiTtyModeMessage> msg);
  void handleQcRilRequestImsSendDtmfMessage(std::shared_ptr<QcRilRequestImsSendDtmfMessage> msg);
  void handleQcRilRequestImsStartDtmfMessage(std::shared_ptr<QcRilRequestImsStartDtmfMessage> msg);
  void handleQcRilRequestImsStopDtmfMessage(std::shared_ptr<QcRilRequestImsStopDtmfMessage> msg);
  void handleMultiSimCapabilityMessage(std::shared_ptr<QcRilUnsolImsMultiSimVoiceCapabilityChanged> msg);
  void handleIpcMultiSimCapabilityMessage(std::shared_ptr<IpcMultiSimCapabilityMessage> msg);

  void processHoldResponse(CommonVoiceResponseData *data);
  void processAddParticipantResponse(CommonVoiceResponseData *data);
  void processModifyCallInitiateResponse(CommonVoiceResponseData *data);
  void processCancelModifyCallResponse(CommonVoiceResponseData *data);
  void processModifyCallConfirmResponse(CommonVoiceResponseData *data);
  void processDeflectCallResponse(CommonVoiceResponseData *data);
  void processSetUiTtyModeResponse(CommonVoiceResponseData *data);
  void processStartContDtmfResponse(CommonVoiceResponseData *data);
  void processStopContDtmfResponse(CommonVoiceResponseData *data);
};
