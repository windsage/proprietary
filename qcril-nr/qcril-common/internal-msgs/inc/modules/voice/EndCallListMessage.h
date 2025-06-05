/*
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include <interfaces/QcRilRequestMessage.h>
#include "framework/add_message_id.h"
#include "qcril_qmi_voice.h"
#include <vector>

class EndCallListMessage : public QcRilRequestMessage, public add_message_id<EndCallListMessage>
{
    public:
        static constexpr const char* MESSAGE_NAME = "EndCallListMessage";
    private:
        // List of calls to be ended.
        std::vector<uint8_t> mCallList;
    public:
        /* Default constructor with empty list of calls to end */
        EndCallListMessage() = delete;
        inline ~EndCallListMessage() {}
        inline EndCallListMessage(std::vector<uint8_t> calls)
            : QcRilRequestMessage(get_class_message_id()) {
            mName = MESSAGE_NAME;
            mCallList = calls;
        }

        std::shared_ptr<SolicitedMessage> clone() {
            auto msg = std::make_shared<EndCallListMessage>(mCallList);
            return msg;
        }

        std::vector<uint8_t> getCallList() {
            return mCallList;
        }

        void setCallList(std::vector<uint8_t> calls) {
            mCallList = calls;
        }

        virtual std::string dump() {
            std::string os;
            os += mName;
            os += "{ call IDs: ";
            vector<uint8_t> it;
            for (auto id : mCallList) {
                os += std::to_string(id).c_str();
                os += ", ";
            }
            os += "}";
            return os;
        }
    private:

};
