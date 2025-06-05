/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef __SAR_RILREQUESTGETSARREVKEYMESSAGE_H_
#define __SAR_RILREQUESTGETSARREVKEYMESSAGE_H_
#include "framework/add_message_id.h"
#include "interfaces/QcRilRequestMessage.h"
#include "interfaces/nas/nas_types.h"

namespace qcril {
namespace interfaces {

struct GetSarRevKeyResult : public qcril::interfaces::BasePayload {
  uint32_t key;
};

}  // namespace interfaces
}  // namespace qcril

/**
 * Request to SAR Rev key
 * @Receiver: NasModule
 *
 * Response:
 *   errorCode    : Valid error codes
 *   responseData : std::shared_ptr<qcril::interfaces::GetSarRevKeyResult>
 **/
class RilRequestGetSarRevKeyMessage : public QcRilRequestMessage,
                                      public add_message_id<RilRequestGetSarRevKeyMessage> {
 public:
  static constexpr const char* MESSAGE_NAME = "RilRequestGetSarRevKeyMessage";

  RilRequestGetSarRevKeyMessage() = delete;

  explicit inline RilRequestGetSarRevKeyMessage(std::shared_ptr<MessageContext> context)
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
#endif
