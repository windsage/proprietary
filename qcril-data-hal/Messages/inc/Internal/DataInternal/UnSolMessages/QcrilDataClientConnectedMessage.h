/******************************************************************************
 #  Copyright (c) 2023 Qualcomm Technologies, Inc.
 #  All Rights Reserved.
 #  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>

/*
 * QcrilDataClientConnected: Sent by the Radio data interface module (e.g.
 * RadioData_aidl_service) to notify the data client is connected.
 */

class QcrilDataClientConnectedMessage : public UnSolicitedMessage,
                                 public add_message_id<QcrilDataClientConnectedMessage> {
public:
  static constexpr const char *MESSAGE_NAME = "QcrilDataClientConnectedMessage";

  QcrilDataClientConnectedMessage() : UnSolicitedMessage(get_class_message_id()) {
    mName = MESSAGE_NAME;
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    return (std::make_shared<QcrilDataClientConnectedMessage>());
  }

  inline ~QcrilDataClientConnectedMessage() {}

  string dump() { return mName + "{}"; }
};
