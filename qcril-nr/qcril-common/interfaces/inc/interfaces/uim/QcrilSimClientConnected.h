/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>

/**
 * QcrilSimClientConnected: Sent by the SIM interface module (e.g.
 * sim_adil_service) to notify the SIM client is connected.
 */

class QcrilSimClientConnected : public UnSolicitedMessage,
                                public add_message_id<QcrilSimClientConnected> {
public:
  static constexpr const char *MESSAGE_NAME = "QcrilSimClientConnected";

public:
  QcrilSimClientConnected() : UnSolicitedMessage(get_class_message_id()) {
    mName = MESSAGE_NAME;
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    return (std::make_shared<QcrilSimClientConnected>());
  }

  inline ~QcrilSimClientConnected() {}

  string dump() { return mName + "{}"; }
};
