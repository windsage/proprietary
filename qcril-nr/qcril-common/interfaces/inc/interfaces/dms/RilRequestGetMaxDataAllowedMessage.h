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

struct MaxDataAllowedResult_t : public qcril::interfaces::BasePayload {
  uint8_t maxDataSubscriptions;
  inline MaxDataAllowedResult_t(uint8_t max) : maxDataSubscriptions(max) {
  }
};

}  // namespace interfaces
}  // namespace qcril


/**
 * Request to get Maximum Active Data Subscriptions
 * @Receiver: DmsModule
 * Response:
 *   errorCode    : Valid error codes
 *   responseData : std::shared_ptr<qcril::interfaces::MaxDataAllowedResult_t>
 */

class RilRequestGetMaxDataAllowedMessage
    : public QcRilRequestMessage,
      public add_message_id<RilRequestGetMaxDataAllowedMessage> {
 public:
  static constexpr const char* MESSAGE_NAME = "RilRequestGetMaxDataAllowedMessage";

  RilRequestGetMaxDataAllowedMessage() = delete;

  explicit inline RilRequestGetMaxDataAllowedMessage(std::shared_ptr<MessageContext> context)
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
