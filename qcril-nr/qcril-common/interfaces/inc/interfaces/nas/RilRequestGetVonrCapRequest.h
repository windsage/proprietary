/******************************************************************************
#  Copyright (c) 2021 - 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <framework/add_message_id.h>
#include <interfaces/QcRilRequestMessage.h>

class RilRequestGetVonrCapRequest: public QcRilRequestMessage,
    public add_message_id<RilRequestGetVonrCapRequest> {
 public:
  static constexpr const char *MESSAGE_NAME = "RilRequestGetVonrCapRequest";

  RilRequestGetVonrCapRequest() = delete;

  ~RilRequestGetVonrCapRequest() {}

  inline RilRequestGetVonrCapRequest(std::shared_ptr<MessageContext> context)
      : QcRilRequestMessage(get_class_message_id(), context) {
    mName = MESSAGE_NAME;
  }

  virtual string dump() {
    return QcRilRequestMessage::dump() + "{}";
  }
};
