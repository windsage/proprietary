/******************************************************************************
#  Copyright (c) 2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>
#include <interfaces/voice/voice.h>
#include <optional>

/*
 * Unsol message to notify clients about a MT call being automatically rejected by modem
 */
class QcRilUnsolAutoCallRejectionMessage
    : public UnSolicitedMessage,
      public add_message_id<QcRilUnsolAutoCallRejectionMessage> {
 private:
  std::optional<qcril::interfaces::CallType> mCallType;
  std::optional<qcril::interfaces::CallFailCause> mCallFailCause;
  std::optional<uint16_t> mSipErrorCode;
  std::optional<std::string> mNumber;
  std::optional<qcril::interfaces::CallComposerInfo> mComposerInfo;
  std::optional<qcril::interfaces::VerificationStatus> mVerificationStatus;
  std::optional<qcril::interfaces::EcnamInfo> mEcnamInfo;
  std::optional<bool> mIsDcCall;
  std::optional<std::string> mCallReason;

 public:
  static constexpr const char *MESSAGE_NAME = "QcRilUnsolAutoCallRejectionMessage";
  ~QcRilUnsolAutoCallRejectionMessage() {}

  QcRilUnsolAutoCallRejectionMessage() : UnSolicitedMessage(get_class_message_id()) {
    mName = MESSAGE_NAME;
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    std::shared_ptr<QcRilUnsolAutoCallRejectionMessage> msg =
        std::make_shared<QcRilUnsolAutoCallRejectionMessage>();
    if (msg) {
      if (mCallType) {
        msg->setCallType(*mCallType);
      }
      if (mCallFailCause) {
        msg->setCallFailCause(*mCallFailCause);
      }
      if (mSipErrorCode) {
        msg->setSipErrorCode(*mSipErrorCode);
      }
      if (mNumber) {
        msg->setNumber(*mNumber);
      }
      if (mVerificationStatus) {
        msg->setVerificationStatus(*mVerificationStatus);
      }
    }
    return msg;
  }

  bool hasCallType() { return mCallType ? true : false; }
  qcril::interfaces::CallType getCallType() { return *mCallType; }
  void setCallType(qcril::interfaces::CallType val) { mCallType = val; }
  inline string dumpCallType() {
    return "mCallType = " + (mCallType ? toString(*mCallType) : "<invalid>");
  }

  bool hasCallFailCause() { return mCallFailCause ? true : false; }
  qcril::interfaces::CallFailCause getCallFailCause() { return *mCallFailCause; }
  void setCallFailCause(qcril::interfaces::CallFailCause val) { mCallFailCause = val; }
  inline string dumpCallFailCause() {
    return "mCallFailCause = " + (mCallFailCause ? toString(*mCallFailCause) : "<invalid>");
  }

  bool hasSipErrorCode() { return mSipErrorCode ? true : false; }
  uint16_t getSipErrorCode() { return *mSipErrorCode; }
  void setSipErrorCode(uint16_t val) { mSipErrorCode = val; }
  inline string dumpSipErrorCode() {
    return "mSipErrorCode = " + (mSipErrorCode ? std::to_string(*mSipErrorCode) : "<invalid>");
  }

  bool hasNumber() { return mNumber ? true : false; }
  const std::string &getNumber() { return *mNumber; }
  void setNumber(const std::string &val) { mNumber = val; }
  inline std::string loggablePhoneNumber() {
      std::string ret = mNumber ?
          ((*mNumber).length() > QCRIL_SHOW_PHONE_NUMBER_MAX ?
           (QCRIL_HIDE_PHONE_NUMBER_STR + (*mNumber).substr((*mNumber).length() - QCRIL_SHOW_PHONE_NUMBER_MAX)).c_str() :
           *mNumber) : "<invalid>";
      return ret;
  }
  inline std::string dumpNumber() { return "mNumber = " + PII(loggablePhoneNumber()); }

  // TODO: try to generate generic dump function in objs defined under qcril::interfaces
  bool hasComposerInfo() { return mComposerInfo ? true : false; }
  const qcril::interfaces::CallComposerInfo& getComposerInfo() { return *mComposerInfo; }
  void setComposerInfo(const qcril::interfaces::CallComposerInfo& info) { mComposerInfo = info; }

  bool hasVerificationStatus() { return mVerificationStatus.has_value(); }
  qcril::interfaces::VerificationStatus getVerificationStatus() { return *mVerificationStatus; }
  void setVerificationStatus(qcril::interfaces::VerificationStatus status) {
    mVerificationStatus = status;
  }
  inline string dumpVerificationStatus() {
    return "verificationStatus = " + (mVerificationStatus ? toString(*mVerificationStatus):
          "<invalid>");
  }

  bool hasEcnamInfo() {
    return mEcnamInfo ? true : false;
  }
  const qcril::interfaces::EcnamInfo& getEcnamInfo() {
    return *mEcnamInfo;
  }
  void setEcnamInfo(const qcril::interfaces::EcnamInfo& ecnamInfo) {
    mEcnamInfo = ecnamInfo;
  }

  bool hasIsDcCall() {
    return mIsDcCall ? true : false;
  }

  bool getIsDcCall() {
    return *mIsDcCall;
  }
  void setIsDcCall(bool isDcCall) {
    mIsDcCall = isDcCall;
  }

  bool hasCallReason() { return mCallReason ? true : false; }
  const std::string &getCallReason() { return *mCallReason; }
  void setCallReason(const std::string &val) {
    mCallReason = val;
  }

  virtual string dump() {
    return mName + "{" + dumpCallType() + " " + dumpCallFailCause() + " " + dumpSipErrorCode() + " " + dumpNumber() + " " +
           dumpVerificationStatus() + " " +
           (hasComposerInfo() ? qcril::interfaces::toString(*mComposerInfo) : "<invalid>") + " " +
           (hasEcnamInfo() ? qcril::interfaces::toString(*mEcnamInfo) : "<invalid>") + " " +
           "}";
  }
};
