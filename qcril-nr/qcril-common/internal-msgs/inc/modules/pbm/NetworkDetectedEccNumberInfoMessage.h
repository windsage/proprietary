/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include <qtibus/IPCMessage.h>

class NetworkDetectedEccNumberInfoMessage
    : public IPCMessage,
      public add_message_id<NetworkDetectedEccNumberInfoMessage> {
 public:
  static constexpr const char* MESSAGE_NAME = "NetworkDetectedEccNumberInfoMessage";

  enum class Action { RESET, SET };

 private:
  Action mAction;
  std::string mNumber;
  uint32_t mEmergencyCategory = 0;
  std::vector<uint8_t> mEmergencyServiceUrn;

 public:
  NetworkDetectedEccNumberInfoMessage(Action action = Action::RESET)
      : IPCMessage(get_class_message_id()), mAction(action) {
    mName = MESSAGE_NAME;
  }

  inline ~NetworkDetectedEccNumberInfoMessage() {
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    auto ret = std::make_shared<NetworkDetectedEccNumberInfoMessage>(mAction);
    if (ret) {
      ret->mAction = mAction;
      ret->mNumber = mNumber;
      ret->mEmergencyCategory = mEmergencyCategory;
      ret->mEmergencyServiceUrn = mEmergencyServiceUrn;
      ret->setIsRemote(getIsRemote());
    }
    return ret;
  }

  void serialize(IPCOStream& os) {
    uint8_t action = (mAction == Action::SET ? 1 : 0);
    os.write(reinterpret_cast<char *>(&action), sizeof(action));
    uint32_t strSize = mNumber.size();
    os.write(reinterpret_cast<char *>(&strSize), sizeof(strSize));
    if (strSize) {
      os.width(strSize);
      os << mNumber;
    }
    os.write(reinterpret_cast<char *>(&mEmergencyCategory), sizeof(mEmergencyCategory));
    uint32_t urnSize = mEmergencyServiceUrn.size();
    os.write(reinterpret_cast<char *>(&urnSize), sizeof(urnSize));
    if (urnSize) {
      for (auto urn : mEmergencyServiceUrn) {
        os.write(reinterpret_cast<char *>(&urn), sizeof(urn));
      }
    }
  }

  void deserialize(IPCIStream& is) {
    uint8_t action;
    is.read(reinterpret_cast<char *>(&action), sizeof(action));
    mAction = (action ? Action::SET : Action::RESET);
    uint32_t strSize = 0;
    is.read(reinterpret_cast<char *>(&strSize), sizeof(strSize));
    if (strSize) {
      is.width(strSize);
      is >> mNumber;
    }
    is.read(reinterpret_cast<char *>(&mEmergencyCategory), sizeof(mEmergencyCategory));
    uint32_t urnSize = 0;
    is.read(reinterpret_cast<char *>(&urnSize), sizeof(urnSize));
    if (urnSize) {
      uint8_t urn;
      for (uint32_t i = 0; i < urnSize; i++) {
        is.read(reinterpret_cast<char *>(&urn), sizeof(urn));
        mEmergencyServiceUrn.push_back(urn);
      }
    }
  }

  void setAction(Action action) {
    mAction = action;
  }

  Action getAction() {
    return mAction;
  }

  void setNumber(std::string number) {
    mNumber = number;
  }

  std::string getNumber() {
    return mNumber;
  }

  void setEmergencyCategory(uint32_t emergencyCategory) {
    mEmergencyCategory = emergencyCategory;
  }

  uint32_t getEmergencyCategory() {
    return mEmergencyCategory;
  }

  void setEmergencyServiceUrn(std::vector<uint8_t> urn) {
    mEmergencyServiceUrn = urn;
  }

  std::vector<uint8_t> getEmergencyServiceUrn() {
    return mEmergencyServiceUrn;
  }

  std::string dump() {
    std::string os = mName;
    os += "{";
    os += " .isRemote=";
    os += (getIsRemote() ? "true" : "false");
    os += " .mAction=";
    os += (mAction == Action::SET ? "SET" : "RESET");
    os += " .mNumber=" + mNumber;
    os += " .mEmergencyCategory=" + std::to_string(mEmergencyCategory);
    os += " .mEmergencyServiceUrn.size()=" + std::to_string(mEmergencyServiceUrn.size());
    os += "}";
    return os;
  }
};
