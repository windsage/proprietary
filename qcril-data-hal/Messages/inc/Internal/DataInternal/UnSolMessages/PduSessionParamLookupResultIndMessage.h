/**
* Copyright (c) 2021, 2023 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#pragma once

#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"
#include "MessageCommon.h"

namespace rildata {

enum class PduSessionParamLookupResultStatus_t : int32_t {
  SUCCESS                       = 0,
  INTERNAL_ERROR                = 1,
  NO_MEMORY                     = 2,
  INVALID_APN                   = 3,
  SLICE_NOT_ALLOWED             = 4,
  ROUTING_FAIL                  = 5,
  LADN_DNN_NOT_AVAIL            = 6,
  APN_TYPE_MISMATCH             = 7,
  NON_DEFAULT_RULE_NOT_MATCHED  = 8,
  NON_SEAMLESS_OFFLOAD          = 9,
  ROUTE_NOT_ALLOWED             = 10,
};

struct PduSessionParamLookupResult_t {
  uint16_t txId;
  PduSessionParamLookupResultStatus_t status;
  std::string dnnName;
};

/********************** Class Definitions *************************/
class PduSessionParamLookupResultIndMessage: public UnSolicitedMessage,
                           public add_message_id<PduSessionParamLookupResultIndMessage> {

private:
  PduSessionParamLookupResult_t mResult;

public:
  static constexpr const char *MESSAGE_NAME = "PduSessionParamLookupResultIndMessage";

  PduSessionParamLookupResultIndMessage() = delete;
  PduSessionParamLookupResultIndMessage(PduSessionParamLookupResult_t result)
  : UnSolicitedMessage(get_class_message_id()), mResult(result) {
    mName = MESSAGE_NAME;
    std::transform(mResult.dnnName.begin(), mResult.dnnName.end(), mResult.dnnName.begin(), ::tolower);
  }
  ~PduSessionParamLookupResultIndMessage(){};

  std::shared_ptr<UnSolicitedMessage> clone() {
    return std::make_shared<PduSessionParamLookupResultIndMessage>(mResult);
  }

  PduSessionParamLookupResult_t getParams() {
    return mResult;
  }

  std::string dump() {
    std::stringstream ss;
    ss << mName << " ";
    ss << mResult.txId << ",";
    ss << (int)mResult.status << ",";
    ss << mResult.dnnName;
    return ss.str();
  }
};

} //namespace
