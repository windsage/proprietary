/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include <framework/Module.h>
#include <framework/QcrilInitMessage.h>
#include "config_aidl_service.h"

class ConfigAidlModule : public Module {
 public:
  ConfigAidlModule();
  ~ConfigAidlModule();
  void init();

 private:
  void handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg);
  void registerAidlService(qcril_instance_id_e_type instance_id);
  void handleSlotStatusIndication(std::shared_ptr<UimSlotStatusInd> msg);
  std::shared_ptr<IRadioConfigImpl> mIRadioConfigAidlImpl{nullptr};
};
