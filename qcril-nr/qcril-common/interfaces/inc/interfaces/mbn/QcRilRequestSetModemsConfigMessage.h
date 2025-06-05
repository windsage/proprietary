/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <interfaces/QcRilRequestMessage.h>
#include "framework/add_message_id.h"

/*
 * Request to set num of live modems
 * @Receiver: MbnModule
 *
 * Response:
 *   errorCode    : Valid error codes
 *                   SUCCESS
 *                   GENERIC_FAILURE
 *   responseData : nullptr
 */
class QcRilRequestSetModemsConfigMessage : public QcRilRequestMessage,
                                        public add_message_id<QcRilRequestSetModemsConfigMessage>
{
  public:
    static constexpr const char *MESSAGE_NAME = "RIL_REQUEST_SET_MODEMS_CONFIG";
    enum class Status { NOT_ACTIVATED, ACTIVATING, ACTIVATED };

    QcRilRequestSetModemsConfigMessage() = delete;
    ~QcRilRequestSetModemsConfigMessage() {};

    explicit QcRilRequestSetModemsConfigMessage(
            std::shared_ptr<MessageContext> context, uint8_t numOfModems):
            QcRilRequestMessage(get_class_message_id(), context), mNumOfModems(numOfModems) {
        mName = MESSAGE_NAME;
        mState = Status::NOT_ACTIVATED;
    }

    uint8_t getNumOfModems() {
        return mNumOfModems;
    }

    Status getState() {
        return mState;
    }

    void setState(Status state) {
        mState = state;
    }

    virtual string dump() {
      return QcRilRequestMessage::dump() +
            "{" +  "number of modems = " +  std::to_string(mNumOfModems)
            + " state = " + convertToString(mState) + "}";
    }

  private:
    const std::string convertToString(Status state) {
        switch (state) {
        case Status::ACTIVATING:
            return "ACTIVATING";
        case Status::ACTIVATED:
            return "ACTIVATED";
        case Status::NOT_ACTIVATED:
        default:
            return "NOT_ACTIVATED";
        }
    }

    uint8_t mNumOfModems;
    Status mState;
};
