/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include <qtibus/IPCMessage.h>

class IpcMultiSimCapabilityMessage : public IPCMessage, public add_message_id<IpcMultiSimCapabilityMessage> {
 public:
  static constexpr const char* MESSAGE_NAME = "IpcMultiSimCapabilityMessage";

 private:
  uint8_t mInstanceId;
  qcril::interfaces::MultiSimVoiceCapability mMultiSimVoiceCapability;

 public:
  IpcMultiSimCapabilityMessage()
      : IPCMessage(get_class_message_id()), mInstanceId(QCRIL_DEFAULT_INSTANCE_ID) {
    mName = MESSAGE_NAME;
  }

  IpcMultiSimCapabilityMessage(uint8_t instanceId)
      : IPCMessage(get_class_message_id()), mInstanceId(instanceId) {
    mName = MESSAGE_NAME;
  }

  inline ~IpcMultiSimCapabilityMessage() {
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    auto ret = std::make_shared<IpcMultiSimCapabilityMessage>(mInstanceId);
    if (ret) {
      ret->mMultiSimVoiceCapability = mMultiSimVoiceCapability;
      ret->setIsRemote(getIsRemote());
    }
    return ret;
  }

  void serialize(IPCOStream& os) {
    os << mInstanceId;
    os << static_cast<uint8_t>(mMultiSimVoiceCapability);
  }

  void deserialize(IPCIStream& is) {
    is >> mInstanceId;
    uint8_t cap;
    is >> cap;
    mMultiSimVoiceCapability = static_cast<qcril::interfaces::MultiSimVoiceCapability>(cap);
  }

  void setMultiSimVoiceCapability(qcril::interfaces::MultiSimVoiceCapability capability) {
    mMultiSimVoiceCapability = capability;
  }

  qcril::interfaces::MultiSimVoiceCapability getMultiSimVoiceCapability() {
    return mMultiSimVoiceCapability;
  }

  string dump() {
    std::string os = mName;
    os += "{";
    os += " .isRemote=";
    os += (getIsRemote() ? "true" : "false");
    os += " .mInstanceId=" + std::to_string(mInstanceId);
    os += " .mMultiSimVoiceCapability=" + std::to_string(static_cast<uint8_t>(mMultiSimVoiceCapability));
    os += "}";
    return os;
  }
};
