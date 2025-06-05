/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>
#include <interfaces/ims/ims.h>

/*
 * Unsol message to notify change in NasSysInfo
 */
class RilUnsolNasSysInfo : public UnSolicitedMessage,
                             public add_message_id<RilUnsolNasSysInfo> {
 private:
  qcril::interfaces::SrvDomain mImsDomain;

 public:
  static constexpr const char *MESSAGE_NAME = "RilUnsolNasSysInfo";
  ~RilUnsolNasSysInfo() {}

  RilUnsolNasSysInfo(qcril::interfaces::SrvDomain domain)
      : UnSolicitedMessage(get_class_message_id()), mImsDomain(domain) {}

  std::shared_ptr<UnSolicitedMessage> clone() {
    std::shared_ptr<RilUnsolNasSysInfo> msg =
        std::make_shared<RilUnsolNasSysInfo>(mImsDomain);
      return msg;
  }

  qcril::interfaces::SrvDomain getImsDomain() { return mImsDomain; }

  string dump() {
    return RilUnsolNasSysInfo::MESSAGE_NAME;
  }
};
