/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <framework/add_message_id.h>
#include <interfaces/QcRilRequestMessage.h>

/*
 * Request to get the ims multi sim voice capability from lower layers
 *
 * Response:
 *   errorCode    : Valid error codes
 *                   SUCCESS
 *                   GENERIC_FAILURE
 *   responseData : std::shared_ptr<qcril::interfaces::MultiSimVoiceCapabilityRespData>
 */

class QcRilRequestImsQueryMultiSimVoiceCapability
    : public QcRilRequestMessage,
      public add_message_id<QcRilRequestImsQueryMultiSimVoiceCapability> {
 public:
  static constexpr const char* MESSAGE_NAME = "QcRilRequestImsQueryMultiSimVoiceCapability";

  QcRilRequestImsQueryMultiSimVoiceCapability() = delete;

  ~QcRilRequestImsQueryMultiSimVoiceCapability(){};

  inline QcRilRequestImsQueryMultiSimVoiceCapability(std::shared_ptr<MessageContext> context)
      : QcRilRequestMessage(get_class_message_id(), context) {
    mName = MESSAGE_NAME;
  }
};
