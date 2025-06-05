/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include <framework/legacy.h>
#include <framework/Module.h>
#include <framework/QcrilInitMessage.h>
#include "modules/uim_remote/UimRmtQmiUimRemoteIndMsg.h"
#include "modules/uim_remote/UimRmtCardAppInitStatusIndMsg.h"
#include "modules/qmi/ModemEndPointFactory.h"
#include "modules/qmi/UimRemoteModemEndPoint.h"
#include "modules/qmi/QmiUimRemoteSetupRequestCallback.h"
#include "UimRmtQmiUimRemoteRespMsg.h"
#include "qcril_uim_remote.h"
#include "uimremoteclient_stable_aidl_service.h"

namespace aidlimplimports {
using namespace aidl::vendor::qti::hardware::radio::uim_remote_client::implementation;
}

class UimRemoteClientAidlModule : public Module {
 public:
  UimRemoteClientAidlModule();
  ~UimRemoteClientAidlModule();
  void init();
  bool isReady();

  void qcril_uim_remote_client_request_event(int32_t token,
                                             qcril_uim_remote_event_req_type* user_req_ptr);

  void qcril_uim_remote_client_request_apdu(int32_t token,
                                            aidlimports::UimRemoteClientApduStatus apdu_status,
                                            uint32_t apdu_data_len,
                                            const uint8_t* apdu_ptr);

 private:
  bool mReady;
  uint32_t mApduId;

  void handleQcrilInitMessage(std::shared_ptr<QcrilInitMessage> msg);
  void handleUimRemoteRespMessage(std::shared_ptr<UimRmtQmiUimRemoteRespMsg> msg);
  void handleUimRemoteIndMessage(std::shared_ptr<UimRmtQmiUimRemoteIndMsg> msg);
  void handleUimRemoteCardAppInitStatusIndMessage(std::shared_ptr<UimRmtCardAppInitStatusIndMsg> msg);
  std::shared_ptr<aidlimplimports::IUimRemoteServiceClientImpl> mUimRemoteServiceClientImpl =
      nullptr;
  qmi_uim_remote_slot_type qcril_uim_remote_convert_instance_to_slot_type(uint32_t instance);
};
