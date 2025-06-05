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
 * Send Vos Action Info message
 *
 * Response:
 *   errorCode    : Valid error codes
 *   responseData : nullptr
 */
class QcRilRequestSendVosActionInfoMessage
    : public QcRilRequestMessage,
      public add_message_id<QcRilRequestSendVosActionInfoMessage> {
 private:
  std::optional<qcril::interfaces::VosActionInfo> mVosActionInfo;

 public:
  static constexpr const char* MESSAGE_NAME = "QcRilRequestSendVosActionInfoMessage";

  QcRilRequestSendVosActionInfoMessage() = delete;

  ~QcRilRequestSendVosActionInfoMessage() {
  }

  inline QcRilRequestSendVosActionInfoMessage(std::shared_ptr<MessageContext> context)
      : QcRilRequestMessage(get_class_message_id(), context) {
    mName = MESSAGE_NAME;
  }

  bool hasVosActionInfo() {
    return mVosActionInfo ? true : false;
  }
  const qcril::interfaces::VosActionInfo& getVosActionInfo() {
    return *mVosActionInfo;
  }
  void setVosActionInfo(const qcril::interfaces::VosActionInfo& val) {
    mVosActionInfo = val;
  }

  virtual std::string dump() {
    std::string os;
    os += QcRilRequestMessage::dump();
    os += "{";
    os += ".mVosActionInfo=" + (hasVosActionInfo()
        ? qcril::interfaces::toString(*mVosActionInfo) : "<invalid>");
    os += "}";
    return os;
  }
};
