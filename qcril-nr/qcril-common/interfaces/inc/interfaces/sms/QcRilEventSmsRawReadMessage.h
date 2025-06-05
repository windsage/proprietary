/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>
#include "telephony/ril.h"

/*
 * Message to trigger reading RAW messages from UIM or NV
 */

class QcRilEventSmsRawReadMessage : public UnSolicitedMessage,
            public add_message_id<QcRilEventSmsRawReadMessage> {
 public:
  enum class MessageMode {
    INVALID,
    CDMA,
    GW,
  };
  enum class StorageType {
    NONE,
    UIM,
    NV,
  };
  static constexpr const char *MESSAGE_NAME = "QCRIL_EVT_SMS_RAW_READ";
  ~QcRilEventSmsRawReadMessage() { }

  explicit QcRilEventSmsRawReadMessage(MessageMode mode, StorageType type,
        uint32_t index)
      : UnSolicitedMessage(get_class_message_id()), mMessageMode(mode),
        mStorageType(type), mStorageIndex(index) {
    mName = MESSAGE_NAME;
  }

  void setSmsOnIms(bool val) {
    mSmsOnIms = val;
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    auto msg = std::make_shared<QcRilEventSmsRawReadMessage>(mMessageMode,
            mStorageType, mStorageIndex);
    if (msg && mSmsOnIms) {
        msg->setSmsOnIms(*mSmsOnIms);
    }
    return msg;
  }

  MessageMode getMessageMode() { return mMessageMode; }
  StorageType getStorageType() { return mStorageType; }
  uint32_t getStorageIndex() { return mStorageIndex; }
  const std::optional<bool>& getSmsOnIms() { return mSmsOnIms; }

  string dump() {
    return QcRilEventSmsRawReadMessage::MESSAGE_NAME;
  }

 private:
  MessageMode mMessageMode;
  StorageType mStorageType;
  uint32_t mStorageIndex;
  std::optional<bool> mSmsOnIms;
};
