/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#include <framework/SolicitedMessage.h>
#include <framework/legacy.h>
#include <framework/add_message_id.h>
#include "interfaces/uim/UimLpaReqMessage.h"

namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace lpa {
namespace implementation {

class UimLpaRespMessage : public SolicitedMessage<void>, public add_message_id<UimLpaRespMessage> {
 private:
  uimLpaResponseType mRespId;
  void* mDataPtr;
  int mToken;

 public:
  static constexpr const char* MESSAGE_NAME = "com.qualcomm.qti.qcril.uim.lpa_reponse_aidl";

  UimLpaRespMessage() = delete;

  UimLpaRespMessage(uimLpaResponseType resp_id, void* data_ptr, int token);

  int get_token()
  {
    return mToken;
  }

  uimLpaResponseType get_response_id()
  {
    return mRespId;
  }

  void* get_message_ptr()
  {
    return mDataPtr;
  }

  ~UimLpaRespMessage();

  string dump();
};

}  // namespace implementation
}  // namespace lpa
}  // namespace radio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
}  // namespace aidl
