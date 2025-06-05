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

struct GetCsgIdResult_t : public qcril::interfaces::BasePayload {
  uint32_t mCsgId;
  inline GetCsgIdResult_t(uint32_t csgId): mCsgId(csgId) {}
};

}  // namespace interfaces
}  // namespace qcril


/**
 * Request to get Maximum Active Data Subscriptions
 * @Receiver: NasModule
 * Response:
 *   errorCode    : Valid error codes
 *   responseData : std::shared_ptr<qcril::interfaces::GetCsgIdResult_t>
 */

class RilRequestGetCsgIdMessage : public QcRilRequestMessage,
                                  public add_message_id<RilRequestGetCsgIdMessage> {
 public:
  static constexpr const char* MESSAGE_NAME = "RilRequestGetCsgIdMessage";

  RilRequestGetCsgIdMessage() = delete;

  explicit inline RilRequestGetCsgIdMessage(std::shared_ptr<MessageContext> context)
      : QcRilRequestMessage(get_class_message_id(), context) {
    mName = MESSAGE_NAME;
  }
  bool isSameMessage(std::shared_ptr<Message> msg) override {
    if (msg) {
      return (get_message_id() == msg->get_message_id());
    }
    return false;
  }
};
