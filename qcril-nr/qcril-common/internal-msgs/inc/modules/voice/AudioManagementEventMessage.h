/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>
#include <optional>
#include "ip_multimedia_subsystem_application_v01.h"

/**
 * Audio Management event
 */
class AudioManagementEventMessage : public UnSolicitedMessage,
                                    public add_message_id<AudioManagementEventMessage> {
 public:
  enum class Event {
    IMS_HANDOVER,
    UNKNOWN,
  };

  static constexpr const char *MESSAGE_NAME = "AudioManagementEventMessage";
  ~AudioManagementEventMessage() = default;

  AudioManagementEventMessage(Event e)
      : UnSolicitedMessage(get_class_message_id()), mEvent(e) {
    mName = MESSAGE_NAME;
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    auto msg = std::make_shared<AudioManagementEventMessage>(mEvent);
    if (msg) {
      if (mTargetRat) {
        msg->setTargetRat(*mTargetRat);
      }
    }
    return msg;
  }

  Event getEvent() {
    return mEvent;
  }

  bool hasTargetRat() {
    return mTargetRat ? true : false;
  }

  imsa_service_rat_enum_v01 getTargetRat() {
    return *mTargetRat;
  }

  void setTargetRat(imsa_service_rat_enum_v01 val) {
    mTargetRat = val;
  }

  virtual std::string dump() {
    std::string os;
    os += mName;
    os += "{";
    os += ".mEvent=" + std::to_string(static_cast<uint32_t>(mEvent));
    os += "}";
    return os;
  }

 private:
  Event mEvent;
  std::optional<imsa_service_rat_enum_v01> mTargetRat;
};
