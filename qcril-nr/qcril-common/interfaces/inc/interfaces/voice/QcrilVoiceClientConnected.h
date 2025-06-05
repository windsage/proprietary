/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>

/**
 * QcrilVoiceClientConnected: Sent by the voice interface module (e.g.
 * voice_aidl_service) to notify the voice client is connected.
 */

class QcrilVoiceClientConnected : public UnSolicitedMessage,
                                  public add_message_id<QcrilVoiceClientConnected> {
public:
  static constexpr const char *MESSAGE_NAME = "QcrilVoiceClientConnected";

public:
  QcrilVoiceClientConnected() : UnSolicitedMessage(get_class_message_id()) {
    mName = MESSAGE_NAME;
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    return (std::make_shared<QcrilVoiceClientConnected>());
  }

  inline ~QcrilVoiceClientConnected() {}

  string dump() { return mName + "{}"; }
};
