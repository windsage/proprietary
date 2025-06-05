/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>

class NasVoiceSystemIdInd : public UnSolicitedMessage, public add_message_id<NasVoiceSystemIdInd> {
 public:
  static constexpr const char* MESSAGE_NAME = "NasVoiceSystemIdInd";

  NasVoiceSystemIdInd() : UnSolicitedMessage(get_class_message_id()) {
    mName = MESSAGE_NAME;
  }

  ~NasVoiceSystemIdInd() = default;

  std::shared_ptr<UnSolicitedMessage> clone() {
    auto msg = std::make_shared<NasVoiceSystemIdInd>();
    return msg;
  }

  void setVoiceSystemId(uint32_t id) {
    mVoiceSystemId = id;
  }

  bool hasVoiceSystemId() {
    return bool(mVoiceSystemId);
  }

  uint32_t getVoiceSystemId() const {
    return *mVoiceSystemId;
  }

  void setLteVoiceSystemId(uint32_t id) {
    mLteVoiceSystemId = id;
  }

  bool hasLteVoiceSystemId() {
    return bool(mLteVoiceSystemId);
  }

  uint32_t getLteVoiceSystemId() const {
    return *mLteVoiceSystemId;
  }

  void setWlanVoiceSystemId(uint32_t id) {
    mWlanVoiceSystemId = id;
  }

  bool hasWlanVoiceSystemId() {
    return bool(mWlanVoiceSystemId);
  }

  uint32_t getWlanVoiceSystemId() const {
    return *mWlanVoiceSystemId;
  }

  string dump() {
    std::string os;
    os += mName;
    os += "{";
    os += "mVoiceSystemId=" + (mVoiceSystemId ? std::to_string(*mVoiceSystemId) : "<invalid>");
    os += "mLteVoiceSystemId=" +
          (mLteVoiceSystemId ? std::to_string(*mLteVoiceSystemId) : "<invalid>");
    os += "mWlanVoiceSystemId=" +
          (mWlanVoiceSystemId ? std::to_string(*mWlanVoiceSystemId) : "<invalid>");
    os += "}";
    return os;
  }

 private:
  std::optional<uint32_t> mVoiceSystemId;
  std::optional<uint32_t> mLteVoiceSystemId;
  std::optional<uint32_t> mWlanVoiceSystemId;
};
