/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <interfaces/QcRilRequestMessage.h>
#include "framework/add_message_id.h"

/*
 * Request to query sim phone book records message on SIM card
 * @Receiver: PbmModule
 *
 * Response:
 *   errorCode    : Valid error codes
 *                   SUCCESS
 *                   GENERIC_FAILURE
 */
class QcRilRequestGetPhonebookRecordsMessage: public QcRilRequestMessage,
                                        public add_message_id<QcRilRequestGetPhonebookRecordsMessage>
{
  public:
    static constexpr const char *MESSAGE_NAME = "QCRIL_REQUEST_GET_ADN_RECORDS";

    ~QcRilRequestGetPhonebookRecordsMessage() {}

    inline QcRilRequestGetPhonebookRecordsMessage(std::shared_ptr<MessageContext> context):
        QcRilRequestMessage(get_class_message_id(), context) {
      mName = MESSAGE_NAME;
    }

    virtual string dump() {
        return mName;
    }
};
