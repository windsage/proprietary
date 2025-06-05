/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <framework/add_message_id.h>
#include <interfaces/QcRilRequestMessage.h>

/*
 * Request to set the Audio Service status
 *
 * Response:
 *   errorCode    : Valid error codes
 *                   SUCCESS
 *                   GENERIC_FAILURE
 *   responseData : nullptr
 */
class QcRilRequestSetAudioServiceStatusMessage
    : public QcRilRequestMessage,
      public add_message_id<QcRilRequestSetAudioServiceStatusMessage> {
 private:
  bool mIsReady = false;

 public:
  static constexpr const char* MESSAGE_NAME = "QcRilRequestSetAudioServiceStatusMessage";

  QcRilRequestSetAudioServiceStatusMessage() = delete;

  ~QcRilRequestSetAudioServiceStatusMessage() {
  }

  inline QcRilRequestSetAudioServiceStatusMessage(std::shared_ptr<MessageContext> context)
      : QcRilRequestMessage(get_class_message_id(), context) {
    mName = MESSAGE_NAME;
  }

  bool isReady() {
    return mIsReady;
  }

  void setIsReady(bool val) {
    mIsReady = val;
  }

  virtual string dump() {
    std::string os = QcRilRequestMessage::dump();
    os += "{";
    os += std::string(".isReady=") + (mIsReady ? "true" : "false");
    os += "}";
    return os;
  }
};
