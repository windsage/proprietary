/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>

/*
 * Unsol message to notify that the Power On Optimization UI READY
 */
class NasPowerOnOptUiReadyIndMessage : public UnSolicitedMessage,
                                       public add_message_id<NasPowerOnOptUiReadyIndMessage> {
 public:
  static constexpr const char* MESSAGE_NAME = "NasPowerOnOptUiReadyIndMessage";
  ~NasPowerOnOptUiReadyIndMessage() {
  }

  NasPowerOnOptUiReadyIndMessage() : UnSolicitedMessage(get_class_message_id()) {
    mName = MESSAGE_NAME;
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    auto msg = std::make_shared<NasPowerOnOptUiReadyIndMessage>();
    return msg;
  }

  std::string dump() {
    return mName + "{}";
  }
};
