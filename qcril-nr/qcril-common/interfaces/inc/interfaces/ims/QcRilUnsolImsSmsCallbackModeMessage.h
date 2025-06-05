/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>
#include <interfaces/ims/ims.h>

/*
 * Unsol message to notify the geo location date retrieval status
 */
class QcRilUnsolImsSmsCallbackModeMessage
    : public UnSolicitedMessage,
      public add_message_id<QcRilUnsolImsSmsCallbackModeMessage> {
 private:
  std::optional<qcril::interfaces::SmsCallbackModeStatus> mScbmStatus;

 public:
  static constexpr const char* MESSAGE_NAME = "QcRilUnsolImsSmsCallbackModeMessage";
  ~QcRilUnsolImsSmsCallbackModeMessage() {
  }

  QcRilUnsolImsSmsCallbackModeMessage() : UnSolicitedMessage(get_class_message_id()) {
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    std::shared_ptr<QcRilUnsolImsSmsCallbackModeMessage> msg =
        std::make_shared<QcRilUnsolImsSmsCallbackModeMessage>();
    return msg;
  }

  bool hasSmsCallbackModeStatus() {
    return mScbmStatus.has_value();
  }
  qcril::interfaces::SmsCallbackModeStatus getSmsCallbackModeStatus() {
    return *mScbmStatus;
  }
  void setSmsCallbackModeStatus(qcril::interfaces::SmsCallbackModeStatus val) {
    mScbmStatus = val;
  }

  virtual string dump() {
    std::string os;
    os += QcRilUnsolImsSmsCallbackModeMessage::MESSAGE_NAME;
    os += "{";
    os += ".mScbmStatus=";
    os += qcril::interfaces::toString(*mScbmStatus);
    os += "}";
    return os;
  }
};
