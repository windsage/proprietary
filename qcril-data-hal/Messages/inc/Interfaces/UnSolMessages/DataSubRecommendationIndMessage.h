/**
* Copyright (c) 2023 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#ifndef DATASUBRECOMMENDATIONINDMESSAGE
#define DATASUBRECOMMENDATIONINDMESSAGE
#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"
#include "MessageCommon.h"

namespace rildata {

enum SubscriptionType {
  DDS = 1,
  NDDS = 2
};

enum Action {
  DataAllowed = 1,
  DataNotAllowed = 2
};

struct Recommendation_t {
  SubscriptionType subs;
  Action action;
};

/********************** Class Definitions *************************/
class DataSubRecommendationIndMessage : public UnSolicitedMessage,
                           public add_message_id<DataSubRecommendationIndMessage> {

private:
  Recommendation_t mRecommendation;

public:
  static constexpr const char *MESSAGE_NAME = "DataSubRecommendationIndMessage";

  ~DataSubRecommendationIndMessage() = default;
  DataSubRecommendationIndMessage():
  UnSolicitedMessage(get_class_message_id())
  {
    mName = MESSAGE_NAME;
  }
  DataSubRecommendationIndMessage(Recommendation_t reco ):
  UnSolicitedMessage(get_class_message_id()), mRecommendation(reco)
  {
    mName = MESSAGE_NAME;
  }

  void setDataSubRecommendation(Recommendation_t reco) {mRecommendation = reco;}
  Recommendation_t getDataSubRecommendation() {return mRecommendation;}

  std::shared_ptr<UnSolicitedMessage> clone()
  {
    return std::make_shared<DataSubRecommendationIndMessage>(mRecommendation);
  }

  string dump()
  {
    std::stringstream ss;
    ss << MESSAGE_NAME;
    ss << "subs=" << (int)mRecommendation.subs;
    ss << "action=" << (int)mRecommendation.action;
    return ss.str();
  }
};

} //namespace

#endif