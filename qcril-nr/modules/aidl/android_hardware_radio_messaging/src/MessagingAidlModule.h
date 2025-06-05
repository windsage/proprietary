/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include <framework/Module.h>
#include <framework/QcrilInitMessage.h>
#include "messaging_aidl_service.h"

class MessagingAidlModule : public Module {
 private:
  std::shared_ptr<IRadioMessagingImpl> mIRadioMessagingAidlImpl;
  void handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg);
  void registerAidlService(qcril_instance_id_e_type instance_id);

 public:
  MessagingAidlModule();
  ~MessagingAidlModule();
  void init();
  void handleIncoming3GppSMSMessage(std::shared_ptr<RilUnsolIncoming3GppSMSMessage> msg);
  void handleIncoming3Gpp2SMSMessage(std::shared_ptr<RilUnsolIncoming3Gpp2SMSMessage> msg);
  void handleNewSmsOnSimMessage(std::shared_ptr<RilUnsolNewSmsOnSimMessage> msg);
  void handleNewBroadcastSmsMessage(std::shared_ptr<RilUnsolNewBroadcastSmsMessage> msg);
  void handleNewSmsStatusReportMessage(std::shared_ptr<RilUnsolNewSmsStatusReportMessage> msg);
  void handleCdmaRuimSmsStorageFullMessage(
    std::shared_ptr<RilUnsolCdmaRuimSmsStorageFullMessage> msg);
  void handleSimSmsStorageFullMessage(
    std::shared_ptr<RilUnsolSimSmsStorageFullMessage> msg);

};

