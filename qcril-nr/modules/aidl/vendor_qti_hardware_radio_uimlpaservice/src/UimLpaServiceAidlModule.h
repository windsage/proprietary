/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include "framework/Module.h"
#include <framework/QcrilInitMessage.h>
#include "modules/qmi/LpaQmiUimHttpIndicationMsg.h"
#include "LpaQmiUimHttpResponseMsg.h"
#include "interfaces/uim/UimLpaIndicationMsg.h"
#include "interfaces/lpa/lpa_service_types.h"
#include "uimlpaservice_stable_aidl_service.h"
#include "UimLpaRespMessage.h"

namespace aidlimplimports {
using namespace aidl::vendor::qti::hardware::radio::lpa::implementation;
}

class UimLpaServiceAidlModule : public Module {
 public:
  UimLpaServiceAidlModule();
  ~UimLpaServiceAidlModule();
  void init();
#ifdef QMI_RIL_UTF
  void cleanup();
#endif

 private:
  void handleQcrilInit(std::shared_ptr<QcrilInitMessage>);
  void registerAidlService(qcril_instance_id_e_type);
  void handleQmiUimHttpResponse(std::shared_ptr<aidlimplimports::LpaQmiUimHttpResponseMsg> msg_ptr);
  void handleQmiUimHttpIndication(std::shared_ptr<LpaQmiUimHttpIndicationMsg> msg_ptr);
  void handleUimLpaIndicationMessage(std::shared_ptr<UimLpaIndicationMsg> msg_ptr);
  void handleUimLpaResponseMessage(std::shared_ptr<aidlimplimports::UimLpaRespMessage> msg_ptr);
  std::shared_ptr<aidlimplimports::IUimLpaImpl> mIUimLpaImpl = nullptr;
};
