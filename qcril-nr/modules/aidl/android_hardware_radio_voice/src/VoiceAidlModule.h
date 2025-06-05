/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include "voice_aidl_service.h"
#include <framework/Module.h>
#include <framework/QcrilInitMessage.h>

class VoiceAidlModule : public Module {
 public:
  VoiceAidlModule();
  ~VoiceAidlModule();
  void init();

 private:
  void handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg);
  void registerAidlService(qcril_instance_id_e_type instance_id);

  void handleQcRilUnsolCallRingingMessage(
      std::shared_ptr<QcRilUnsolCallRingingMessage> msg);
  void handleQcRilUnsolCallStateChangeMessage(
      std::shared_ptr<QcRilUnsolCallStateChangeMessage> msg);
  void handleQcRilUnsolCdmaCallWaitingMessage(
      std::shared_ptr<QcRilUnsolCdmaCallWaitingMessage> msg);
  void handleQcRilUnsolCdmaInfoRecordMessage(
      std::shared_ptr<QcRilUnsolCdmaInfoRecordMessage> msg);
  void handleQcRilUnsolCdmaOtaProvisionStatusMessage(
      std::shared_ptr<QcRilUnsolCdmaOtaProvisionStatusMessage> msg);
  void handleRilUnsolEmergencyCallbackModeMessage(
      std::shared_ptr<RilUnsolEmergencyCallbackModeMessage> msg);
  void handleQcRilUnsolRingbackToneMessage(
      std::shared_ptr<QcRilUnsolRingbackToneMessage> msg);
  void handleQcRilUnsolSupplementaryServiceMessage(
      std::shared_ptr<QcRilUnsolSupplementaryServiceMessage> msg);
  void handleQcRilUnsolSrvccStatusMessage(
      std::shared_ptr<QcRilUnsolSrvccStatusMessage> msg);
  void handleRilUnsolStkCCAlphaNotifyMessage(
      std::shared_ptr<RilUnsolStkCCAlphaNotifyMessage> msg);
  void handleRilUnsolEmergencyListIndMessage(
      std::shared_ptr<RilUnsolEmergencyListIndMessage> msg);
  void handleGstkUnsolIndMsg(
      std::shared_ptr<GstkUnsolIndMsg> msg);
  void handleQcRilUnsolOnUssdMessage(
      std::shared_ptr<QcRilUnsolOnUssdMessage> msg);

 private:
  std::shared_ptr<IRadioVoiceImpl> mIRadioVoiceAidlImpl;
};

