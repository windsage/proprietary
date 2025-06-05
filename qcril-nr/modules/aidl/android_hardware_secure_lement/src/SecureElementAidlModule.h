/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include "interfaces/uim/UimQmiUimRequestMsg.h"
#include "modules/qmi/UimQmiUimIndicationMsg.h"
#include "modules/secure_element/SecureElementOnStateChangeIndMsg.h"
#include "modules/uim/qcril_uim_srvc.h"
#include "secure_element_service.h"
#include <cstring>
#include <framework/Module.h>
#include <framework/QcrilInitMessage.h>

namespace aidlimports {
using namespace aidl::android::hardware::secure_element;
}

class SecureElementAidlModule : public Module {
public:
  SecureElementAidlModule();
  ~SecureElementAidlModule();
  void init();
  int secure_element_send_qmi_sync_msg(uint32_t msg_id, const void *params,
                                       qmi_uim_rsp_data_type *rsp_data_ptr);
  void cleanUpLongApduInfo(void);
  bool mReady;
  bool mServiceUp;
  bool card_accessible;
  bool mQmiUimUp;
  void handleSecureElementOnStateChangeInd(
      std::shared_ptr<SecureElementOnStateChangeIndMsg> msg);

private:
  std::shared_ptr<ISecureElementImpl> mISecureElementImpl = nullptr;
  void handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg);
  void handleQmiUimIndiaction(
      std::shared_ptr<UimQmiUimIndicationMsg> uim_qmi_ind_msg_ptr);
  void secureElementClearChannelIdList(std::vector<uint8_t> &ch_id_list);
};
