/******************************************************************************
 #  Copyright (c) 2023 Qualcomm Technologies, Inc.
 #  All Rights Reserved.
 #  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>

/*
 * QcrilDataClientDisconnected: Sent by the Radio data interface module (e.g.
 * RadioData_aidl_service) to notify the data client is disconnected.
 */

class QcrilDataClientDisconnectedMessage : public UnSolicitedMessage,
                                 public add_message_id<QcrilDataClientDisconnectedMessage> {
public:
  static constexpr const char *MESSAGE_NAME = "QcrilDataClientDisonnectedMessage";

  QcrilDataClientDisconnectedMessage() : UnSolicitedMessage(get_class_message_id()) {
    mName = MESSAGE_NAME;
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    return (std::make_shared<QcrilDataClientDisconnectedMessage>());
  }

  inline ~QcrilDataClientDisconnectedMessage() {}

  string dump() { return mName + "{}"; }
};
