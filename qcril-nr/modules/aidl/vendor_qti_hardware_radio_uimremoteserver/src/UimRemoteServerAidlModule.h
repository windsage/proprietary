/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include <framework/Module.h>
#include <framework/QcrilInitMessage.h>
#include "uimremoteserver_stable_aidl_service.h"
#include "interfaces/uim/UimSapStatusIndMsg.h"

namespace aidlimplimports {
using namespace aidl::vendor::qti::hardware::radio::uim_remote_server::implementation;
}

class UimRemoteServerAidlModule : public Module {
 public:
  UimRemoteServerAidlModule();
  ~UimRemoteServerAidlModule();
  void init();

 private:
  void handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg);
  void handleSAPInd(std::shared_ptr<UimSapStatusIndMsg> msg);
  std::shared_ptr<aidlimplimports::IUimRemoteServiceServerImpl> mIUimRemoteServiceServerImpl =
      nullptr;
};
