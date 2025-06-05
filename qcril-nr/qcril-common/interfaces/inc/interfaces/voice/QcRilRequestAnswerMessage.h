/******************************************************************************
#  Copyright (c) 2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <framework/add_message_id.h>
#include <interfaces/QcRilRequestMessage.h>
#include <interfaces/voice/voice.h>
#include <optional>

/*
 * Request to answer an incoming call
 *
 * Response:
 *   errorCode    : Valid error codes
 *   responseData : nullptr
 */
class QcRilRequestAnswerMessage : public QcRilRequestMessage,
                                  public add_message_id<QcRilRequestAnswerMessage> {
 private:
  std::optional<qcril::interfaces::CallType> mCallType;
  std::optional<qcril::interfaces::Presentation> mPresentation;
  std::optional<qcril::interfaces::RttMode> mRttMode;

 public:
  static constexpr const char* MESSAGE_NAME = "QcRilRequestAnswerMessage";
  QcRilRequestAnswerMessage() = delete;

  ~QcRilRequestAnswerMessage() {
  }

  inline QcRilRequestAnswerMessage(std::shared_ptr<MessageContext> context)
      : QcRilRequestMessage(get_class_message_id(), context)
  {
    mName = MESSAGE_NAME;
  }

  bool hasCallType() {
    return mCallType ? true : false;
  }
  const qcril::interfaces::CallType& getCallType() {
    return *mCallType;
  }
  void setCallType(const qcril::interfaces::CallType& val) {
    mCallType = val;
  }

  bool hasPresentation() {
    return mPresentation ? true : false;
  }
  const qcril::interfaces::Presentation& getPresentation() {
    return *mPresentation;
  }
  void setPresentation(qcril::interfaces::Presentation val) {
    mPresentation = val;
  }

  bool hasRttMode() {
    return mRttMode ? true : false;
  }
  const qcril::interfaces::RttMode& getRttMode() {
    return *mRttMode;
  }
  void setRttMode(const qcril::interfaces::RttMode& val) {
    mRttMode = val;
  }

  virtual string dump() {
    std::string os;
    os += QcRilRequestMessage::dump();
    os += "{";
    os += ".mCallType=" + (mCallType ? toString(*mCallType) : "<invalid>");
    os += ".mPresentation=" + (mPresentation ? toString(*mPresentation) : "<invalid>");
    os += ".mRttMode=" + (mRttMode ? toString(*mRttMode) : "<invalid>");
    os += "}";
    return os;
  }
};
