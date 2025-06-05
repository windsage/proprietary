/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/add_message_id.h"
#include "interfaces/QcRilRequestMessage.h"
#include "interfaces/nas/nas_types.h"

/**
 * Request to set UI ready status
 * @Receiver: NasModule
 *
 * Response:
 *   errorCode    : Valid error codes
 *   responseData : nullpotr
 **/
class RilRequestSetUiStatusMessage
    : public QcRilRequestMessage,
      public add_message_id<RilRequestSetUiStatusMessage> {
 private:
  bool mUiReady;

 public:
  static constexpr const char* MESSAGE_NAME = "RilRequestSetUiStatusMessage";

  RilRequestSetUiStatusMessage() = delete;
  ~RilRequestSetUiStatusMessage() {
  }

  inline RilRequestSetUiStatusMessage(std::shared_ptr<MessageContext> context, bool uiReady)
      : QcRilRequestMessage(get_class_message_id(), context) {
    mName = MESSAGE_NAME;
    mUiReady = uiReady;
  }

  bool isUiReady() {
    return mUiReady;
  }

  virtual string dump() {
    std::stringstream os;
    os << QcRilRequestMessage::dump();
    os << "{";
    os << ".mUiReady=" << std::boolalpha << mUiReady;
    os << "}";
    return os.str();
  }
};
