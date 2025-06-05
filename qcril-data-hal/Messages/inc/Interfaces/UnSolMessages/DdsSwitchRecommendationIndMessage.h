/**
* Copyright (c) 2021 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#ifndef DDSSWITCHRECOMMENDATIONINDMESSAGE
#define DDSSWITCHRECOMMENDATIONINDMESSAGE
#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"
#include "MessageCommon.h"

namespace rildata {

/********************** Class Definitions *************************/
class DdsSwitchRecommendationIndMessage : public UnSolicitedMessage,
                           public add_message_id<DdsSwitchRecommendationIndMessage> {

private:
    SubscriptionId_t mSubscriptionId;
public:
    static constexpr const char *MESSAGE_NAME = "DdsSwitchRecommendationIndMessage";

    DdsSwitchRecommendationIndMessage() = delete;
   ~DdsSwitchRecommendationIndMessage() = default;
    DdsSwitchRecommendationIndMessage(SubscriptionId_t subId):
    UnSolicitedMessage(get_class_message_id()), mSubscriptionId(subId)
    {
      mName = MESSAGE_NAME;
    }
    SubscriptionId_t getSubscriptionId() {return mSubscriptionId;}

    std::shared_ptr<UnSolicitedMessage> clone()
    {
      return std::make_shared<DdsSwitchRecommendationIndMessage>(mSubscriptionId);
    }

    string dump()
    {
      std::stringstream ss;
      ss << MESSAGE_NAME << " " << (int)getSubscriptionId();
      return ss.str();
    }

};

} //namespace

#endif