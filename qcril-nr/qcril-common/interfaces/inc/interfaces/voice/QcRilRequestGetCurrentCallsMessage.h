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
 * Get current call list
 *
 * Response:
 *   errorCode    : Valid error codes
 *   responseData : std::shared_ptr<GetCurrentCallsRespData>
 */
class QcRilRequestGetCurrentCallsMessage : public QcRilRequestMessage,
                                  public add_message_id<QcRilRequestGetCurrentCallsMessage> {
 public:
  static constexpr const char *MESSAGE_NAME = "QcRilRequestGetCurrentCallsMessage";

  QcRilRequestGetCurrentCallsMessage() = delete;

  ~QcRilRequestGetCurrentCallsMessage() {}

  inline QcRilRequestGetCurrentCallsMessage(std::shared_ptr<MessageContext> context)
      : QcRilRequestMessage(get_class_message_id(), context) {
    mName = MESSAGE_NAME;
  }

  void setDomain(qcril::interfaces::CallDomain val) {
    mDomain = val;
  }

  qcril::interfaces::CallDomain getDomain() {
    return mDomain;
  }

  virtual string dump() {
    return QcRilRequestMessage::dump() + "{}";
  }
 private:
  qcril::interfaces::CallDomain mDomain = qcril::interfaces::CallDomain::CS;
};
