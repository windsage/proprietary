/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef __RFRPE_RILREQUESTGETPROVTABLEREVMESSAGE_H_
#define __RFRPE_RILREQUESTGETPROVTABLEREVMESSAGE_H_
#include "framework/add_message_id.h"
#include "interfaces/QcRilRequestMessage.h"
#include "services/qtuner_v01.h"

namespace qcril {
namespace interfaces {

struct GetProvTableRevResult : public qcril::interfaces::BasePayload {
  Qtuner_get_provisioned_table_revision_resp_v01 response;
};

}  // namespace interfaces
}  // namespace qcril

/**
 * Request to set rfm secnarios
 * @Receiver: NasModule
 * Response:
 *   errorCode    : Valid error codes
 *   responseData : std::shared_ptr<qcril::interfaces::GetProvTableRevResult>
 */
class RilRequestGetProvTableRevMessage : public QcRilRequestMessage,
                                         public add_message_id<RilRequestGetProvTableRevMessage> {
 public:
  static constexpr const char* MESSAGE_NAME = "RilRequestGetProvTableRevMessage";

  RilRequestGetProvTableRevMessage() = delete;

  explicit inline RilRequestGetProvTableRevMessage(std::shared_ptr<MessageContext> context)
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
