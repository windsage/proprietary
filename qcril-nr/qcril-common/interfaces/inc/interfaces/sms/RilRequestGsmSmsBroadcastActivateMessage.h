/******************************************************************************
#  Copyright (c) 2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/GenericCallback.h"
#include "framework/SolicitedMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"
#include "framework/message_translator.h"
#include "framework/legacy.h"
#include <telephony/ril.h>
#include "interfaces/sms/qcril_qmi_sms_types.h"
#include <interfaces/QcRilRequestMessage.h>

/* Request to activate GSM SMS broadcast config Message
   @Receiver: SmsModule

   Response:
    errorCode    : Valid error codes
    responseData : nullptr
*/

class RilRequestGsmSmsBroadcastActivateMessage
    : public QcRilRequestMessage,
      public add_message_id<RilRequestGsmSmsBroadcastActivateMessage> {
public:
  static constexpr const char *MESSAGE_NAME = "RIL_REQUEST_GSM_SMS_BROADCAST_ACTIVATION";

  RilRequestGsmSmsBroadcastActivateMessage() = delete;
  ~RilRequestGsmSmsBroadcastActivateMessage() {};

  inline RilRequestGsmSmsBroadcastActivateMessage(std::shared_ptr<MessageContext> context,
                                                  bool activate)
      : QcRilRequestMessage(get_class_message_id(), context), mActivate(activate) {
    mName = MESSAGE_NAME;
  }

  bool getActivateStatus() {
    return mActivate;
  };

  string dump() {
    return mName + std::string(" { activate = ")  +
        (mActivate ? "true" : "false") + std::string(" }");
  };

private:
  bool mActivate;
};
