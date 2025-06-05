/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#include "framework/Module.h"
#include "framework/QcrilInitMessage.h"
#include "RadioDataServiceImpl.h"

#include "UnSolMessages/SlicingConfigChangedIndMessage.h"


class RadioDataModule : public Module {
 public:
  string mName = "RadioDataModule";
  RadioDataModule();
  ~RadioDataModule();
  void init();

 private:
  void handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg);
  void registerAidlService(qcril_instance_id_e_type instance_id);
  void handleSlicingConfigChangedIndMessage(std::shared_ptr<rildata::SlicingConfigChangedIndMessage> msg);
  void handleSetupDataCallRadioResponseIndMessage(std::shared_ptr<rildata::SetupDataCallRadioResponseIndMessage> msg);
  void handleDeactivateDataCallRadioResponseIndMessage(std::shared_ptr<rildata::DeactivateDataCallRadioResponseIndMessage> msg);
  void handleRadioDataCallListChangeIndMessage(std::shared_ptr<rildata::RadioDataCallListChangeIndMessage> msg);
  void handleUnthrottleApnMessage(std::shared_ptr<rildata::ThrottledApnTimerExpirationMessage> msg);
  void handleRadioKeepAliveStatusIndMessage(std::shared_ptr<rildata::RadioKeepAliveStatusIndMessage> msg);
  void handlePCODataMessage(std::shared_ptr<rildata::RilPCODataMessage> msg);
  std::shared_ptr<RadioDataServiceImpl> mRadioDataServiceImpl;
};
