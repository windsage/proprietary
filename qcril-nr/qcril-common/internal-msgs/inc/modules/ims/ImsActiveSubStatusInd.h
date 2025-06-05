/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>
#include <framework/legacy.h>

class ImsActiveSubStatusInd : public UnSolicitedMessage,
                        public add_message_id<ImsActiveSubStatusInd> {
public:
  static constexpr const char *MESSAGE_NAME =
      "com.qualcomm.qti.qcril.ims.active_sub_status_ind";

  ImsActiveSubStatusInd(bool isActive)
      : UnSolicitedMessage(get_class_message_id()) {
    mIsSubActive = isActive;
  }

  inline ~ImsActiveSubStatusInd() {}

  std::shared_ptr<UnSolicitedMessage> clone() {
    return (std::make_shared<ImsActiveSubStatusInd>(mIsSubActive));
  }

  string dump() { return ImsActiveSubStatusInd::MESSAGE_NAME; }

  inline bool isSubActive() { return mIsSubActive; }

private:
  bool mIsSubActive;
};
