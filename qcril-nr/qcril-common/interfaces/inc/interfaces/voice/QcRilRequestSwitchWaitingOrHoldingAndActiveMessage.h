/******************************************************************************
#  Copyright (c) 2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <framework/add_message_id.h>
#include <interfaces/QcRilRequestMessage.h>
#include <interfaces/voice/voice.h>
#include <optional>

/*
 * Switch waiting or holding call and active call
 *
 * Response:
 *   errorCode    : Valid error codes
 *   responseData : nullptr
 */
class QcRilRequestSwitchWaitingOrHoldingAndActiveMessage
    : public QcRilRequestMessage,
      public add_message_id<QcRilRequestSwitchWaitingOrHoldingAndActiveMessage> {
 public:
  static constexpr const char *MESSAGE_NAME = "QcRilRequestSwitchWaitingOrHoldingAndActiveMessage";

  QcRilRequestSwitchWaitingOrHoldingAndActiveMessage() = delete;

  ~QcRilRequestSwitchWaitingOrHoldingAndActiveMessage() {}

  inline QcRilRequestSwitchWaitingOrHoldingAndActiveMessage(std::shared_ptr<MessageContext> context)
      : QcRilRequestMessage(get_class_message_id(), context) {
    mName = MESSAGE_NAME;
  }

  bool hasCallType() {
    return mCallType ? true : false;
  }
  const qcril::interfaces::CallType& getCallType() {
    return *mCallType;
  }
  void setCallType(const qcril::interfaces::CallType& val) {
    mCallType = val;
  }

  virtual string dump() {
    return QcRilRequestMessage::dump() + "{}";
  }
 private:
  std::optional<qcril::interfaces::CallType> mCallType;
};
