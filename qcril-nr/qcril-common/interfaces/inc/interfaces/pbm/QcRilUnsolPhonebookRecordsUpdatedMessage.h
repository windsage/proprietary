/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/UnSolicitedMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"
#include "interfaces/pbm/pbm.h"

/*
 * Request to query sim phone book capacity message on SIM card
 * @Receiver: PbmModule
 *
 * Response:
 *   errorCode    : Valid error codes
 *                   SUCCESS
 *                   GENERIC_FAILURE
 *   responseData : std::shared_ptr<qcril::interfaces::AdnCountInfoResp>
 */
class QcRilUnsolPhonebookRecordsUpdatedMessage: public UnSolicitedMessage,
                                        public add_message_id<QcRilUnsolPhonebookRecordsUpdatedMessage>
{
  public:
    static constexpr const char *MESSAGE_NAME = "UNSOL_PHONE_BOOK_RECORDS_UPDATED";

    ~QcRilUnsolPhonebookRecordsUpdatedMessage() {}

    explicit inline QcRilUnsolPhonebookRecordsUpdatedMessage()
          : UnSolicitedMessage(get_class_message_id()) {
      mName = MESSAGE_NAME;
    }

  std::shared_ptr<UnSolicitedMessage> clone() {
    return std::make_shared<QcRilUnsolPhonebookRecordsUpdatedMessage>();
  }

  virtual string dump() {
    return mName;
  }
};
