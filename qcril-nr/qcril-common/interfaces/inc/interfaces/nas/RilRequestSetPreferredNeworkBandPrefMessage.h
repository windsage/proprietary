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

/**
 * Request to get the preferred network band pref.
 * @Receiver: NasModule
 *
 * Response:
 *   errorCode    : Valid error codes
 *   responseData : nullpotr
 **/
class RilRequestSetPreferredNeworkBandPrefMessage
    : public QcRilRequestMessage,
      public add_message_id<RilRequestSetPreferredNeworkBandPrefMessage> {
 private:
  qcril::interfaces::RilBandPrefType mBandPrefMap;

 public:
  static constexpr const char* MESSAGE_NAME = "RilRequestSetPreferredNeworkBandPrefMessage";

  RilRequestSetPreferredNeworkBandPrefMessage() = delete;
  ~RilRequestSetPreferredNeworkBandPrefMessage() {
  }

  inline RilRequestSetPreferredNeworkBandPrefMessage(std::shared_ptr<MessageContext> context,
                                                     qcril::interfaces::RilBandPrefType bandPrefMap)
      : QcRilRequestMessage(get_class_message_id(), context) {
    mName = MESSAGE_NAME;
    mBandPrefMap = bandPrefMap;
  }

  qcril::interfaces::RilBandPrefType getBandPrefMap() {
    return mBandPrefMap;
  }
};
