/**
* Copyright (c) 2023 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#ifndef DDSSWITCHRECOMMENDINTERNALMESSAGE
#define DDSSWITCHRECOMMENDINTERNALMESSAGE
#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include <framework/add_message_id.h>

#include "MessageCommon.h"

namespace rildata {

enum class DdsSwitchType_t : int8_t {
  Permanent = 0,
  Temporary = 1,
};

enum class DdsSwitchLevel_t : int8_t {
  None   = 0,
  Revoke = 1,
  Low    = 2,
  High   = 3,
};

class DdsSwitchRecommendInternalMessage : public UnSolicitedMessage,
                             public add_message_id<DdsSwitchRecommendInternalMessage> {
private:
  SubscriptionId_t mSubId;
  DdsSwitchType_t mSwitchType;
  DdsSwitchLevel_t mLevel;

public:
  static constexpr const char *MESSAGE_NAME = "DdsSwitchRecommendInternalMessage";
  ~DdsSwitchRecommendInternalMessage() = default;

  inline DdsSwitchRecommendInternalMessage(SubscriptionId_t subId, DdsSwitchType_t switchType, DdsSwitchLevel_t level)
        : UnSolicitedMessage(get_class_message_id())
  {
    mSubId = subId;
    mSwitchType = switchType;
    mLevel = level;
    mName = MESSAGE_NAME;
  }

  inline std::shared_ptr<UnSolicitedMessage> clone()
  {
    return std::static_pointer_cast<UnSolicitedMessage>
          (std::make_shared<DdsSwitchRecommendInternalMessage>(mSubId, mSwitchType, mLevel));
  }

  inline SubscriptionId_t getSubId() {
    return mSubId;
  }

  inline DdsSwitchType_t getSwitchType() {
    return mSwitchType;
  }

  inline DdsSwitchLevel_t getLevel() {
    return mLevel;
  }

  inline string dump()
  {
    std::stringstream ss;
    ss << mName << " subId=" << (int)mSubId;
    ss << " switchtype=" << (int)mSwitchType;
    ss << " level=" << (int)mLevel;
    return ss.str();
  }
};

} //namespace

#endif
