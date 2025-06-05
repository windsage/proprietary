/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <framework/add_message_id.h>
#include <interfaces/QcRilRequestMessage.h>
#include <interfaces/voice/voice.h>
#include <optional>

/*
 * Send Vos Support Status message
 *
 * Response:
 *   errorCode    : Valid error codes
 *   responseData : nullptr
 */
class QcRilRequestSendVosSupportStatusMessage
    : public QcRilRequestMessage,
      public add_message_id<QcRilRequestSendVosSupportStatusMessage> {
 private:
  bool mIsVosSupported = false;

 public:
  static constexpr const char* MESSAGE_NAME = "QcRilRequestSendVosSupportStatusMessage";

  QcRilRequestSendVosSupportStatusMessage() = delete;

  ~QcRilRequestSendVosSupportStatusMessage() {
  }

  inline QcRilRequestSendVosSupportStatusMessage(std::shared_ptr<MessageContext> context)
      : QcRilRequestMessage(get_class_message_id(), context) {
    mName = MESSAGE_NAME;
  }

  bool getVosSupportStatus() {
    return mIsVosSupported;
  }
  void setVosSupportStatus(bool val) {
    mIsVosSupported = val;
  }

  virtual std::string dump() {
    std::string os;
    os += QcRilRequestMessage::dump();
    os += "{";
    os += std::string(".mIsVosSupported=") + (mIsVosSupported ? "true" : "false");
    os += "}";
    return os;
  }
};
