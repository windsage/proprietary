/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>

/*
 * Unsol message related to Primary IMEI mapping change
 */
class QcRilUnsolImeiMessage : public UnSolicitedMessage,
                              public add_message_id<QcRilUnsolImeiMessage> {
 private:
  std::shared_ptr<qcril::interfaces::RilGetImeiInfoResult_t> mImeiInfo;

 public:
  static constexpr const char *MESSAGE_NAME = "QcRilUnsolImeiMessage";
  ~QcRilUnsolImeiMessage() {}

  QcRilUnsolImeiMessage(std::shared_ptr<qcril::interfaces::RilGetImeiInfoResult_t> &imeiInfo)
      : UnSolicitedMessage(get_class_message_id()), mImeiInfo(imeiInfo) {
    mName = MESSAGE_NAME;
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    std::shared_ptr<QcRilUnsolImeiMessage> msg =
        std::make_shared<QcRilUnsolImeiMessage>(mImeiInfo);
    return msg;
  }

  const std::shared_ptr<qcril::interfaces::RilGetImeiInfoResult_t> &getImeiInfo() const {
    return mImeiInfo;
  }

  std::string dump() {
    return mName;
  }
};
