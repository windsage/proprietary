/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
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
#include "interfaces/QcRilRequestMessage.h"
#include "interfaces/securemode/securemode_types.h"

class SecureModeStateChangeIndMessage : public UnSolicitedMessage,
    public add_message_id<SecureModeStateChangeIndMessage> {

    private:
    qcril::interfaces::RilSecureModeStatus_t mState;

    public:
      static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.secure_mode_status_indication";
      ~SecureModeStateChangeIndMessage() {};

      SecureModeStateChangeIndMessage(const uint8_t state)
          : UnSolicitedMessage(get_class_message_id()) {
        mName = MESSAGE_NAME;
        mState = qcril::interfaces::RilSecureModeStatus_t(state);
      }

      bool isSecure() {
        return mState.mState == qcril::interfaces::PeripheralStatus::SECURE;
      }

      std::shared_ptr<UnSolicitedMessage> clone() {
        return std::make_shared<SecureModeStateChangeIndMessage>(
        static_cast<uint8_t>(mState.mState));
      }

      inline string dump() {
        return mName;
      }
};
