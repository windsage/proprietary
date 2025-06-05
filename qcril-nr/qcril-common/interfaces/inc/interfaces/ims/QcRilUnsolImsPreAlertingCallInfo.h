/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>
#include <interfaces/ims/ims.h>

/*
 * Unsol message to notify pre alerting information like call composer and ecnam.
 *
 */
class QcRilUnsolImsPreAlertingCallInfo : public UnSolicitedMessage,
                                         public add_message_id<QcRilUnsolImsPreAlertingCallInfo> {
 private:
  qcril::interfaces::PreAlertingCallInfo mPreAlertingInfo;

 public:
  static constexpr const char* MESSAGE_NAME = "QcRilUnsolImsPreAlertingCallInfo";
  ~QcRilUnsolImsPreAlertingCallInfo() {
  }

  QcRilUnsolImsPreAlertingCallInfo(const qcril::interfaces::PreAlertingCallInfo& preAlertingInfo)
      : UnSolicitedMessage(get_class_message_id()), mPreAlertingInfo(preAlertingInfo) {
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    auto msg = std::make_shared<QcRilUnsolImsPreAlertingCallInfo>(mPreAlertingInfo);
    return msg;
  }

  const qcril::interfaces::PreAlertingCallInfo& getPreAlertingInfo() {
    return mPreAlertingInfo;
  }

  virtual string dump() {
    std::string os;
    os += QcRilUnsolImsPreAlertingCallInfo::MESSAGE_NAME;
    os += "{";
    os += ".PreAlertingCallInfo = ";
    os += qcril::interfaces::toString(mPreAlertingInfo);
    os += "}";
    return os;
  }
};
