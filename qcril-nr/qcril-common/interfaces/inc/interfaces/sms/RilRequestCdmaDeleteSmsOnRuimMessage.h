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
#include "interfaces/sms/qcril_qmi_sms_types.h"
#include <telephony/ril.h>
#include <interfaces/QcRilRequestMessage.h>

/* Request to delete a CDMA SMS message on SIM card
   @Receiver: SmsModule

   Response:
    errorCode    : Valid error codes
    responseData : nullptr
*/

class RilRequestCdmaDeleteSmsOnRuimMessage
    : public QcRilRequestMessage,
      public add_message_id<RilRequestCdmaDeleteSmsOnRuimMessage> {
public:
  static constexpr const char *MESSAGE_NAME = "RIL_REQUEST_CDMA_DELETE_SMS_ON_RUIM";

  RilRequestCdmaDeleteSmsOnRuimMessage() = delete;
  ~RilRequestCdmaDeleteSmsOnRuimMessage() {};

  inline RilRequestCdmaDeleteSmsOnRuimMessage(std::shared_ptr<MessageContext> context, int index)
      : QcRilRequestMessage(get_class_message_id(), context), mIndex(index) {
    mName = MESSAGE_NAME;
  }

  int getIndex() {
    return mIndex;
  };

  string dump() {
    return mName + std::string(" { index = ")  +
          std::to_string(mIndex) + std::string(" }");
  };

private:
  int mIndex;
};
