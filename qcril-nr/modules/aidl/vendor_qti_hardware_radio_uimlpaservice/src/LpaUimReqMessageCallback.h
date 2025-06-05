/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/GenericCallback.h"
#include "interfaces/uim/qcril_uim_lpa.h"

namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace lpa {
namespace implementation {

class LpaUimReqMessageCallback : public GenericCallback<qcril_uim_lpa_response_type> {
 public:
  inline LpaUimReqMessageCallback(string clientToken) : GenericCallback(clientToken){};

  inline ~LpaUimReqMessageCallback(){};

  Message::Callback* clone() const override;

  void onResponse(std::shared_ptr<Message> solicitedMsg,
                  Status status,
                  std::shared_ptr<qcril_uim_lpa_response_type> responseDataPtr) override;
};

}  // namespace implementation
}  // namespace lpa
}  // namespace radio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
}  // namespace aidl
