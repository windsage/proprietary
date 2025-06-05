/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>
#include <interfaces/voice/voice.h>

/*
 * Unsol message to notify conference call state complted
 */
class QcRilUnsolImsConferenceCallStateCompletedMessage
    : public UnSolicitedMessage,
      public add_message_id<QcRilUnsolImsConferenceCallStateCompletedMessage> {
 public:
  static constexpr const char* MESSAGE_NAME = "QcRilUnsolImsConferenceCallStateCompletedMessage";

  ~QcRilUnsolImsConferenceCallStateCompletedMessage() {
  }

  QcRilUnsolImsConferenceCallStateCompletedMessage() : UnSolicitedMessage(get_class_message_id()) {
    mName = MESSAGE_NAME;
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    std::shared_ptr<QcRilUnsolImsConferenceCallStateCompletedMessage> msg =
        std::make_shared<QcRilUnsolImsConferenceCallStateCompletedMessage>();
    return msg;
  }

  std::string dump() {
    return mName + "{}";
  }
};
