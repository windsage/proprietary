/**
* Copyright (c) 2021 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#pragma once
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"
#include <string.h>

namespace rildata {

/********************** Class Definitions *************************/
class UpdateMtuMessage: public UnSolicitedMessage,
                           public add_message_id<UpdateMtuMessage> {
private:
  unsigned int mtu;

public:
  static constexpr const char *MESSAGE_NAME = "UpdateMtuMessage";
  UpdateMtuMessage() = delete;
  inline UpdateMtuMessage( unsigned int Mtu ):
    UnSolicitedMessage(get_class_message_id()), mtu(Mtu) {}
  ~UpdateMtuMessage() {}

  std::shared_ptr<UnSolicitedMessage> clone() {
    return std::make_shared<UpdateMtuMessage>(mtu);
  }

  unsigned int getMtu() {
    return mtu;
  }

  string dump() {
    return get_message_name();
  }
};
} //namespace
