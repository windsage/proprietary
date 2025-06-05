/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <interfaces/QcRilRequestMessage.h>
#include "framework/add_message_id.h"

/*
 * Deactivate current active SW MBNs
 * @Receiver: MbnModule
 *
 * Response:
 *   errorCode    : Valid error codes
 *                   SUCCESS
 *                   GENERIC_FAILURE
 *   responseData : nullptr
 */
class QcRilDeactivateSwMbnsRequestMessage :  public SolicitedMessage<RIL_Errno>,
                                        public add_message_id<QcRilDeactivateSwMbnsRequestMessage>
{
  public:
    static constexpr const char *MESSAGE_NAME = "MBN_DEACTIVATE_CURRENT_SW_MBNS";

    ~QcRilDeactivateSwMbnsRequestMessage() {};

    QcRilDeactivateSwMbnsRequestMessage():
            SolicitedMessage<RIL_Errno>(get_class_message_id()) {
        mName = MESSAGE_NAME;
    }


    string dump() {
      return mName;
    }

};
