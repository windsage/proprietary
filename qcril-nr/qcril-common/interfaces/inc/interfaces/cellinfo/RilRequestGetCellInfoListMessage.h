/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include "framework/add_message_id.h"
#include "interfaces/QcRilRequestMessage.h"
#include "interfaces/cellinfo/cellinfo.h"

/*
 * Request all of the current cell information known to the radio. The radio
 * must a list of all current cells, including the neighboring cells. If for a particular
 * cell information isn't known then the appropriate unknown value will be returned.
 *
 * Response:
 *   errorCode    : Valid error codes
 *   responseData : qcril::interfaces::RilGetCellInfoListResult_t
 *
 * @Receiver: cellinfo
 */

class RilRequestGetCellInfoListMessage : public QcRilRequestMessage,
                                     public add_message_id<RilRequestGetCellInfoListMessage> {
 public:
  static constexpr const char* MESSAGE_NAME = "RIL_REQUEST_GET_CELL_INFO_LIST";

  RilRequestGetCellInfoListMessage() = delete;
  ~RilRequestGetCellInfoListMessage() {
  }

  inline RilRequestGetCellInfoListMessage(std::shared_ptr<MessageContext> context)
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
