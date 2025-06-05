/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include <framework/Module.h>
#include <framework/QcrilInitMessage.h>
#include <cstring>
#include "uimservice_stable_aidl_service.h"
#include "interfaces/uim_service/UimGetRilPropertySyncMsg.h"
#include "interfaces/uim_service/UimSetRilPropertySyncMsg.h"
namespace aidlimplimports {
using namespace aidl::vendor::qti::hardware::radio::uim::implementation;
}

class UimAidlModule : public Module {
 public:
  UimAidlModule();
  ~UimAidlModule();
  void init();

 private:
  std::shared_ptr<aidlimplimports::IUimImpl> mIUimImpl = nullptr;
  void handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg);
  void handleQcrilUimGetProperty(std::shared_ptr<UimGetRilPropertySyncMsg> req_ptr);
  void handleQcrilUimSetProperty(std::shared_ptr<UimSetRilPropertySyncMsg> req_ptr);

};
