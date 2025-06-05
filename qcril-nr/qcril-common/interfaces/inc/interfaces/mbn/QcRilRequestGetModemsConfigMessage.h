/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <interfaces/QcRilRequestMessage.h>
#include "framework/add_message_id.h"

/*
 * Request to get num of live modems
 * @Receiver: MbnModule
 *
 * Response:
 *   errorCode    : Valid error codes
 *                   SUCCESS
 *                   GENERIC_FAILURE
 *   responseData : num of live modems
 */
class QcRilRequestGetModemsConfigMessage : public QcRilRequestMessage,
                                        public add_message_id<QcRilRequestGetModemsConfigMessage>
{
  public:
    static constexpr const char *MESSAGE_NAME = "RIL_REQUEST_GET_MODEMS_CONFIG";

    QcRilRequestGetModemsConfigMessage() = delete;
    ~QcRilRequestGetModemsConfigMessage() {};

    explicit QcRilRequestGetModemsConfigMessage(
            std::shared_ptr<MessageContext> context):
            QcRilRequestMessage(get_class_message_id(), context) {
        mName = MESSAGE_NAME;
    }
};
