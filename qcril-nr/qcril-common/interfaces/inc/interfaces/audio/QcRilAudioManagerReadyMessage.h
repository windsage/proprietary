/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "interfaces/QcRilRequestMessage.h"
#include "framework/add_message_id.h"

/*===========================================================================*/
/*!
    @brief
    Provides the message when  Audio Manager is ready,
    that is, connected to the IQcRilAudio aidl service
*/
/*==========================================================================*/


class QcRilAudioManagerReadyMessage : public QcRilRequestMessage,
                                      public add_message_id<QcRilAudioManagerReadyMessage> {
private:
public:
    static constexpr const char *MESSAGE_NAME = "QcRilAudioManagerReadyMessage";

    QcRilAudioManagerReadyMessage() = delete;
    ~QcRilAudioManagerReadyMessage() = default;

    inline QcRilAudioManagerReadyMessage(std::shared_ptr<MessageContext> context)
        : QcRilRequestMessage(get_class_message_id(), context) {
      mName = MESSAGE_NAME;
    }

    inline string dump() {
        return QcRilRequestMessage::dump() + "{}";
    }
};

