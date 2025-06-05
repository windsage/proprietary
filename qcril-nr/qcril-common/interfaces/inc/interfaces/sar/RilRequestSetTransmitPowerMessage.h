/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef __SAR_RILREQUESTSETTRANSMITPOWERMESSAGE_H_
#define __SAR_RILREQUESTSETTRANSMITPOWERMESSAGE_H_
#include "framework/add_message_id.h"
#include "interfaces/QcRilRequestMessage.h"
#include "interfaces/nas/nas_types.h"

/**
 * Request to set SAR transmit power
 * @Receiver: NasModule
 *
 * Response:
 *   errorCode    : Valid error codes
 *   responseData : nullptr
 **/
class RilRequestSetTransmitPowerMessage : public QcRilRequestMessage,
                                          public add_message_id<RilRequestSetTransmitPowerMessage> {
 public:
  static constexpr const char* MESSAGE_NAME = "RilRequestSetTransmitPowerMessage";

  RilRequestSetTransmitPowerMessage() = delete;

  explicit inline RilRequestSetTransmitPowerMessage(std::shared_ptr<MessageContext> context)
      : QcRilRequestMessage(get_class_message_id(), context) {
    mName = MESSAGE_NAME;
  }

  bool isSameMessage(std::shared_ptr<Message> msg) override {
    if (msg) {
      return (get_message_id() == msg->get_message_id());
    }
    return false;
  }

  uint32_t getRfState() const {
    return mRfState;
  }

  bool hasCompatibilityKey() const {
    return mHasCompatibilityKey;
  }

  uint32_t getCompatibilityKey() const {
    return mCompatibilityKey;
  }

  RIL_Errno setData(const uint8_t* in, uint32_t inLen) {
    if (!in || !inLen) {
      return RIL_E_INVALID_ARGUMENTS;
    }
    if (inLen < sizeof(mRfState)) {
      return RIL_E_INVALID_ARGUMENTS;
    }
    memcpy(&mRfState, in, sizeof(mRfState));
    inLen -= sizeof(mRfState);
    in += sizeof(mRfState);

    if (inLen < sizeof(mCompatibilityKey)) {
      return RIL_E_INVALID_ARGUMENTS;
    }
    memcpy(&mCompatibilityKey, in, sizeof(mCompatibilityKey));
    inLen -= sizeof(mCompatibilityKey);
    in += sizeof(mCompatibilityKey);

    // By default the second param (compatibility_key) is valid.
    // Application may pass an optional third param to specify compatibility_key valid or not.
    uint8_t compatibility_key_valid = TRUE;
    if (inLen >= sizeof(compatibility_key_valid)) {
      memcpy(&compatibility_key_valid, in, sizeof(compatibility_key_valid));
    }
    mHasCompatibilityKey = compatibility_key_valid;
    return RIL_E_SUCCESS;
  }

 private:
  uint32_t mRfState;
  bool mHasCompatibilityKey;
  uint32_t mCompatibilityKey;
};
#endif
