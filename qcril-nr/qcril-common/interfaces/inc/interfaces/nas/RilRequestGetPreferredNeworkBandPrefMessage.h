/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/add_message_id.h"
#include "interfaces/QcRilRequestMessage.h"
#include "interfaces/nas/nas_types.h"
//#include "telephony/ril.h"
//#include "network_access_service_v01.h"

namespace qcril {
namespace interfaces {

struct GetPreferredNeworkBandPrefResult_t : public qcril::interfaces::BasePayload {
  qcril::interfaces::RilBandPrefType bandPrefMap;
};

}  // namespace interfaces
}  // namespace qcril

/**
 * Request to get preferred network band pref.
 * @Receiver: NasModule
 *
 * Response:
 *   errorCode    : Valid error codes
 *   responseData : std::shared_ptr<qcril::interfaces::GetPreferredNeworkBandPrefResult_t>
 **/
class RilRequestGetPreferredNeworkBandPrefMessage
    : public QcRilRequestMessage,
      public add_message_id<RilRequestGetPreferredNeworkBandPrefMessage> {
 private:
  qcril::interfaces::RilRatBandType mRatBandType;

 public:
  static constexpr const char* MESSAGE_NAME = "RilRequestGetPreferredNeworkBandPrefMessage";

  RilRequestGetPreferredNeworkBandPrefMessage() = delete;
  ~RilRequestGetPreferredNeworkBandPrefMessage() {
  }

  inline RilRequestGetPreferredNeworkBandPrefMessage(std::shared_ptr<MessageContext> context,
                                                     qcril::interfaces::RilRatBandType ratBandType)
      : QcRilRequestMessage(get_class_message_id(), context) {
    mName = MESSAGE_NAME;
    mRatBandType = ratBandType;
  }

  qcril::interfaces::RilRatBandType getRatBandType() {
    return mRatBandType;
  }
};
