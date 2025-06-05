/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <interfaces/QcRilRequestMessage.h>
#include "framework/add_message_id.h"

/*
 * Request to query adn capacity from sim phone booke on SIM card
 * @Receiver: PbmModule
 *
 * Response:
 *   errorCode    : Valid error codes
 *                   SUCCESS
 *                   GENERIC_FAILURE
 *   responseData : std::shared_ptr<qcril::interfaces::AdnCountInfoResp>
 */
class QcRilRequestGetAdnCapacityMessage: public QcRilRequestMessage,
                                        public add_message_id<QcRilRequestGetAdnCapacityMessage>
{
  public:
    static constexpr const char *MESSAGE_NAME = "QCRIL_REQUEST_GET_ADN_CAPACITY";

    ~QcRilRequestGetAdnCapacityMessage() {}

    inline QcRilRequestGetAdnCapacityMessage(std::shared_ptr<MessageContext> context):
            QcRilRequestMessage(get_class_message_id(), context) {
      mName = MESSAGE_NAME;
    }

    virtual string dump() {
        return mName;
    }
};
