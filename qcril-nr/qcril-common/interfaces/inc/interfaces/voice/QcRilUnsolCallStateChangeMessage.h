/******************************************************************************
#  Copyright (c) 2018, 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>
#include <interfaces/voice/voice.h>

/*
 * Unsol message to notify call state change
 */
class QcRilUnsolCallStateChangeMessage : public UnSolicitedMessage,
                                         public add_message_id<QcRilUnsolCallStateChangeMessage> {
 public:
  static constexpr const char* MESSAGE_NAME = "QcRilUnsolCallStateChangeMessage";

  ~QcRilUnsolCallStateChangeMessage()
  {
  }

  QcRilUnsolCallStateChangeMessage() : UnSolicitedMessage(get_class_message_id())
  {
    mName = MESSAGE_NAME;
  }

  std::shared_ptr<UnSolicitedMessage> clone()
  {
    auto msg = std::make_shared<QcRilUnsolCallStateChangeMessage>();
    if (msg) {
      msg->setCallInfo(mCallInfo);
    }
    return msg;
  }

  void addToCallInfo(qcril::interfaces::CallInfo callInfo)
  {
    mCallInfo.push_back(callInfo);
  }

  void setCallInfo(const std::vector<qcril::interfaces::CallInfo>& callInfo)
  {
    mCallInfo = callInfo;
  }

  const std::vector<qcril::interfaces::CallInfo>& getCallInfo()
  {
    return mCallInfo;
  };

  bool hasCsCalls()
  {
    for (auto& call : mCallInfo) {
      if (call.hasCallDomain() && call.getCallDomain() == qcril::interfaces::CallDomain::CS) {
        return true;
      }
    }
    return false;
  }

  bool hasImsCalls()
  {
    for (auto& call : mCallInfo) {
      if (call.hasCallDomain() &&
          (call.getCallDomain() == qcril::interfaces::CallDomain::PS ||
           call.getCallDomain() == qcril::interfaces::CallDomain::AUTOMATIC)) {
        return true;
      }
    }
    return false;
  }

  std::string dump()
  {
    return mName + "{}";
  }

 private:
  std::vector<qcril::interfaces::CallInfo> mCallInfo;
};
