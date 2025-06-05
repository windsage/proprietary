/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include <qtibus/IPCMessage.h>

class IpcReleaseAudioMessage : public IPCMessage, public add_message_id<IpcReleaseAudioMessage> {
 public:
  static constexpr const char* MESSAGE_NAME = "IpcReleaseAudioMessage";

 private:
  uint8_t mInstanceId;

 public:
  IpcReleaseAudioMessage()
      : IPCMessage(get_class_message_id()), mInstanceId(QCRIL_DEFAULT_INSTANCE_ID) {
    mName = MESSAGE_NAME;
  }

  IpcReleaseAudioMessage(uint8_t instanceId)
      : IPCMessage(get_class_message_id()), mInstanceId(instanceId) {
    mName = MESSAGE_NAME;
  }

  inline ~IpcReleaseAudioMessage() {
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    auto ret = std::make_shared<IpcReleaseAudioMessage>(mInstanceId);
    if (ret) {
      ret->setIsRemote(getIsRemote());
    }
    return ret;
  }

  void serialize(IPCOStream& os) {
    os << mInstanceId;
  }

  void deserialize(IPCIStream& is) {
    is >> mInstanceId;
  }

  string dump() {
    std::string os = mName;
    os += "{";
    os += " .isRemote=";
    os += (getIsRemote() ? "true" : "false");
    os += " .mInstanceId=" + std::to_string(mInstanceId);
    os += "}";
    return os;
  }
};
