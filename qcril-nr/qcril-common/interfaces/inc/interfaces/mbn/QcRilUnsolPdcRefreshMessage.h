/**
* Copyright (c) 2022 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#pragma once
#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"
#include <interfaces/mbn/mbn.h>

/********************** Class Definitions *************************/
class QcRilUnsolPdcRefreshMessage: public UnSolicitedMessage,
                           public add_message_id<QcRilUnsolPdcRefreshMessage> {

private:
  qcril::interfaces::PdcRefreshEvent mEvent;
  uint32_t subscriptionId;

public:
  static constexpr const char *MESSAGE_NAME = "QcRilUnsolPdcRefreshMessage";

  QcRilUnsolPdcRefreshMessage()
  : UnSolicitedMessage(get_class_message_id()) {
    mName = MESSAGE_NAME;
  }
  ~QcRilUnsolPdcRefreshMessage(){};

  std::shared_ptr<UnSolicitedMessage> clone() {
    return std::make_shared<QcRilUnsolPdcRefreshMessage>();
  }

  string dump() {
    return mName + "{mEvent=" + qcril::interfaces::toString(mEvent) + "}" +
        " subscriptionId=" + std::to_string(subscriptionId) + "}";
  }

  void setEvent(qcril::interfaces::PdcRefreshEvent event) {
    mEvent = event;
  }
  qcril::interfaces::PdcRefreshEvent getEvent() {
    return mEvent;
  }

  void setSubscriptionId(int subId) {
    subscriptionId = subId;
  }
  int getSubscriptionId() {
    return subscriptionId;
  }
};


