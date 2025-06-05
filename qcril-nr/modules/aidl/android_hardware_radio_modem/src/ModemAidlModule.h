/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include <framework/Module.h>
#include <framework/QcrilInitMessage.h>
#include "modem_aidl_service.h"

#include "interfaces/RilUnsolRilConnectedMessage.h"
#include "interfaces/nas/RilUnsolModemRestartMessage.h"
#include "interfaces/nas/RilUnsolRadioCapabilityMessage.h"
#include "interfaces/nas/RilUnsolRadioStateChangedMessage.h"

class ModemAidlModule : public Module {
 public:
  ModemAidlModule();
  ~ModemAidlModule();
  void init();

 private:
  void handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg);
  void registerAidlService(qcril_instance_id_e_type instance_id);

 private:
  std::shared_ptr<IRadioModemImpl> mIRadioModemAidlImpl;

  void handleModemRestartMessage(
    std::shared_ptr<RilUnsolModemRestartMessage> msg);
  void handlelRadioCapabilityMessage(
    std::shared_ptr<RilUnsolRadioCapabilityMessage> msg);
  void handleRadioStateChangedMessage(
    std::shared_ptr<RilUnsolRadioStateChangedMessage> msg);
  void handleRilUnsolRilConnectedMessage(
    std::shared_ptr<RilUnsolRilConnectedMessage> msg);
};

