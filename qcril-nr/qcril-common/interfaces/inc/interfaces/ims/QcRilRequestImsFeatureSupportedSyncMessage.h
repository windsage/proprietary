/******************************************************************************
#  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
#  All rights reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include <framework/add_message_id.h>
#include <framework/SolicitedSyncMessage.h>
#include <interfaces/common.h>

/*
 * Sync request to check if IMS feature is supported by RIL/Modem
 */
class QcRilRequestImsFeatureSupportedSyncMessage
    : public SolicitedSyncMessage<bool>,
      public add_message_id<QcRilRequestImsFeatureSupportedSyncMessage> {
 public:
  static constexpr const char *MESSAGE_NAME = "QcRilRequestImsFeatureSupportedSyncMessage";

  QcRilRequestImsFeatureSupportedSyncMessage() = delete;

  ~QcRilRequestImsFeatureSupportedSyncMessage() {};

  QcRilRequestImsFeatureSupportedSyncMessage(int feature) : SolicitedSyncMessage<bool>(
      get_class_message_id()), mFeature(feature) {
    mName = MESSAGE_NAME;
  }

  int getFeature() {
    return mFeature;
  }

  inline string dump() {
    return mName + ":" + std::to_string(mFeature);
  }
 private:
  int mFeature;
};

