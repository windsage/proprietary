/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/add_message_id.h"
#include "interfaces/QcRilRequestMessage.h"

namespace qcril {
namespace interfaces {

struct RilLPlusLFeatureSupportStatus_t : public qcril::interfaces::BasePayload {
  bool mSupported;
  inline RilLPlusLFeatureSupportStatus_t(bool supported): mSupported(supported) {}
};

}  // namespace interfaces
}  // namespace qcril


/**
 * Request to query L+L feature support status
 * @Receiver: NasModule
 *
 * Response:
 *   errorCode    : Valid error codes
 *   responseData : std::shared_ptr<qcril::interfaces::RilLPlusLFeatureSupportStatus_t>
 */
class RilRequestGetLPlusLFeatureSupportStatusMessage
    : public QcRilRequestMessage,
      public add_message_id<RilRequestGetLPlusLFeatureSupportStatusMessage> {
 public:
  static constexpr const char* MESSAGE_NAME = "RilRequestGetLPlusLFeatureSupportStatusMessage";

  RilRequestGetLPlusLFeatureSupportStatusMessage() = delete;

  inline RilRequestGetLPlusLFeatureSupportStatusMessage(std::shared_ptr<MessageContext> context)
      : QcRilRequestMessage(get_class_message_id(), context) {
    mName = MESSAGE_NAME;
  }
};
