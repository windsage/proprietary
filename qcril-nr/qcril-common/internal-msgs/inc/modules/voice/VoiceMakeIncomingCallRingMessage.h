/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"
#include "telephony/ril.h"

class VoiceMakeIncomingCallRingMessage : public UnSolicitedMessage,
                                         public add_message_id<VoiceMakeIncomingCallRingMessage> {
 public:
  static constexpr const char* MESSAGE_NAME = "VoiceMakeIncomingCallRingMessage";

  ~VoiceMakeIncomingCallRingMessage() = default;

  inline VoiceMakeIncomingCallRingMessage(const RIL_CDMA_SignalInfoRecord& info)
      : UnSolicitedMessage(get_class_message_id()) {
    mName = MESSAGE_NAME;
    mCdmaInfo = info;
  }

  inline VoiceMakeIncomingCallRingMessage() : UnSolicitedMessage(get_class_message_id()) {
    mName = MESSAGE_NAME;
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    if (mCdmaInfo)
      return std::make_shared<VoiceMakeIncomingCallRingMessage>(*mCdmaInfo);
    else
      return std::make_shared<VoiceMakeIncomingCallRingMessage>();
  }

  std::optional<RIL_CDMA_SignalInfoRecord> getCdmaSignalInfoRecord() {
    return mCdmaInfo;
  }

  string dump() {
    std::string os = mName + "CDMA Signal Info Record = {";
    if (mCdmaInfo) {
      const auto& info = *mCdmaInfo;
      os += ".isPresent=" + std::to_string(info.isPresent);
      os += ".signalType=" + std::to_string(info.signalType);
      os += ".alertPitch=" + std::to_string(info.alertPitch);
      os += ".signal=" + std::to_string(info.signal);
    }
    os += "}";
    return os;
  }

 private:
  std::optional<RIL_CDMA_SignalInfoRecord> mCdmaInfo;
};
