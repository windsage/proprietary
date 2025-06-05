/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef SLICECONFIGCHANGEDINDMESSAGE
#define SLICECONFIGCHANGEDINDMESSAGE
#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"
#include "MessageCommon.h"

namespace rildata {

/********************** Class Definitions *************************/
class SlicingConfigChangedIndMessage: public UnSolicitedMessage,
                                     public add_message_id<SlicingConfigChangedIndMessage> {
private:
  std::optional<SlicingConfig_t> mSlicingConfig;

public:
  static constexpr const char *MESSAGE_NAME = "SlicingConfigChangedIndMessage";

  ~SlicingConfigChangedIndMessage() {};
  SlicingConfigChangedIndMessage() : UnSolicitedMessage(get_class_message_id()) {
    mName = MESSAGE_NAME;
  }

  void setSliceConfig(SlicingConfig_t config) {
    mSlicingConfig = config;
  }

  std::optional<SlicingConfig_t> getSlicingConfig() {
    return mSlicingConfig;
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    if (mSlicingConfig.has_value()) {
      auto msg = std::make_shared<SlicingConfigChangedIndMessage>();
      msg->setSliceConfig(mSlicingConfig.value());
      return msg;
    }
    return std::make_shared<SlicingConfigChangedIndMessage>();
  }
  string dump(){
    return MESSAGE_NAME;
  }
};

} //namespace

#endif
