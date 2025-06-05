/******************************************************************************
#  Copyright (c) 2018, 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <framework/add_message_id.h>
#include <interfaces/QcRilRequestMessage.h>
#include <interfaces/voice/voice.h>
#include <optional>

/*
 * Connects the two calls and disconnects the subscriber from both calls.
 *
 * Response:
 *   errorCode    : Valid error codes
 *   responseData : nullptr
 */
class QcRilRequestExplicitCallTransferMessage
    : public QcRilRequestMessage,
      public add_message_id<QcRilRequestExplicitCallTransferMessage> {
 private:
  std::optional<uint32_t> mCallId;
  std::optional<qcril::interfaces::EctType> mEctType;
  std::optional<std::string> mTargetAddress;
  std::optional<uint32_t> mTargetCallId;

 public:
  static constexpr const char *MESSAGE_NAME = "QcRilRequestExplicitCallTransferMessage";

  QcRilRequestExplicitCallTransferMessage() = delete;

  ~QcRilRequestExplicitCallTransferMessage() {}

  inline QcRilRequestExplicitCallTransferMessage(std::shared_ptr<MessageContext> context)
      : QcRilRequestMessage(get_class_message_id(), context) {
    mName = MESSAGE_NAME;
  }

  bool hasCallId()
  {
    return mCallId ? true : false;
  }
  uint32_t getCallId()
  {
    return *mCallId;
  }
  void setCallId(uint32_t val)
  {
    mCallId = val;
  }

  bool hasEctType()
  {
    return mEctType ? true : false;
  }
  qcril::interfaces::EctType getEctType()
  {
    return *mEctType;
  }
  void setEctType(qcril::interfaces::EctType val)
  {
    mEctType = val;
  }

  bool hasTargetAddress()
  {
    return mTargetAddress ? true : false;
  }
  const std::string& getTargetAddress()
  {
    return *mTargetAddress;
  }
  void setTargetAddress(const std::string& val)
  {
    mTargetAddress = val;
  }

  bool hasTargetCallId()
  {
    return mTargetCallId ? true : false;
  }
  uint32_t getTargetCallId()
  {
    return *mTargetCallId;
  }
  void setTargetCallId(uint32_t val)
  {
    mTargetCallId = val;
  }

  virtual std::string dump()
  {
    std::string os;
    os += QcRilRequestMessage::dump();
    os += "{";
    os += ".mCallId=" + (mCallId ? std::to_string(*mCallId) : "<invalid>");
    os += ".mEctType=" + (mEctType ? toString(*mEctType) : "<invalid>");
    os += ".mTargetAddress=" + (mTargetAddress ? (*mTargetAddress) : "<invalid>");
    os += ".mTargetCallId=" + (mTargetCallId ? std::to_string(*mTargetCallId) : "<invalid>");
    os += "}";
    return os;
  }
};
