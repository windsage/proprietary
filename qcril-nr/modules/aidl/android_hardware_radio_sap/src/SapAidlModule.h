/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include "interfaces/uim/UimSapStatusIndMsg.h"
#include "sap_aidl_service.h"
#include <cstring>
#include <framework/Module.h>
#include <framework/QcrilInitMessage.h>


class SapAidlModule : public Module {
public:
  SapAidlModule();
  ~SapAidlModule();
  void init();

private:
  std::shared_ptr<ISapImpl> mISapImpl = nullptr;
  void handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg);
  void handleSAPInd(std::shared_ptr<UimSapStatusIndMsg> msg);
};
