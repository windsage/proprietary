/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include <qtibus/IPCMessage.h>

class IpcReleaseAudioCnfMessage : public IPCMessage, public add_message_id<IpcReleaseAudioCnfMessage> {
 public:
  static constexpr const char* MESSAGE_NAME = "IpcReleaseAudioCnfMessage";

 private:
  uint8_t mInstanceId;
  RIL_Errno mStatus;

 public:
  IpcReleaseAudioCnfMessage()
      : IPCMessage(get_class_message_id()), mInstanceId(QCRIL_DEFAULT_INSTANCE_ID) {
    mName = MESSAGE_NAME;
  }

  IpcReleaseAudioCnfMessage(uint8_t instanceId)
      : IPCMessage(get_class_message_id()), mInstanceId(instanceId) {
    mName = MESSAGE_NAME;
  }

  inline ~IpcReleaseAudioCnfMessage() {
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    auto ret = std::make_shared<IpcReleaseAudioCnfMessage>(mInstanceId);
    if (ret) {
      ret->mStatus = mStatus;
      ret->setIsRemote(getIsRemote());
    }
    return ret;
  }

  void serialize(IPCOStream& os) {
    os << mInstanceId;
    os << static_cast<uint32_t>(mStatus);
  }

  void deserialize(IPCIStream& is) {
    is >> mInstanceId;
    uint32_t err;
    is >> err;
    mStatus = static_cast<RIL_Errno>(err);
  }

  void setStatus(RIL_Errno status) {
    mStatus = status;
  }

  RIL_Errno getStatus() {
    return mStatus;
  }

  string dump() {
    std::string os = mName;
    os += "{";
    os += " .isRemote=";
    os += (getIsRemote() ? "true" : "false");
    os += " .mInstanceId=" + std::to_string(mInstanceId);
    os += " .mStatus=" + std::to_string(mStatus);
    os += "}";
    return os;
  }
};
