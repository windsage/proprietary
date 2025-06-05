/*===========================================================================
 *  Copyright (c) 2022 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 *===========================================================================*/

#pragma once

#include "aidl/android/hardware/radio/messaging/IRadioMessagingIndication.h"
#include "SmsAidlUtil.h"

namespace aidl {
namespace android {
namespace hardware {
namespace radio {
namespace messaging {
class IRadioMessagingIndicationImpl : public IRadioMessagingIndication {
public:
  IRadioMessagingIndicationImpl() {}

  virtual ~IRadioMessagingIndicationImpl() {}

  ::ndk::ScopedAStatus cdmaNewSms(::aidlradio::RadioIndicationType in_type,
                                  const ::aidlmsg::CdmaSmsMessage& in_msg) override;

  ::ndk::ScopedAStatus cdmaRuimSmsStorageFull(::aidlradio::RadioIndicationType in_type) override;

  ::ndk::ScopedAStatus newBroadcastSms(::aidlradio::RadioIndicationType in_type,
                                       const std::vector<uint8_t>& in_data) override;

  ::ndk::ScopedAStatus newSms(::aidlradio::RadioIndicationType in_type,
                              const std::vector<uint8_t>& in_pdu) override;

  ::ndk::ScopedAStatus newSmsOnSim(::aidlradio::RadioIndicationType in_type,
                                   int32_t in_recordNumber) override;

  ::ndk::ScopedAStatus newSmsStatusReport(::aidlradio::RadioIndicationType in_type,
                                          const std::vector<uint8_t>& in_pdu) override;

  ::ndk::ScopedAStatus simSmsStorageFull(::aidlradio::RadioIndicationType in_type) override;

  ::ndk::SpAIBinder asBinder() override {
    return ::ndk::SpAIBinder();
  }

  bool isRemote() override {
    return false;
  }

  ::ndk::ScopedAStatus getInterfaceVersion(int32_t* _aidl_return) override {
    return ndk::ScopedAStatus::ok();
  }

  ::ndk::ScopedAStatus getInterfaceHash(std::string* _aidl_return) override {
    return ndk::ScopedAStatus::ok();
  }
};
}  // namespace messaging
}  // namespace radio
}  // namespace hardware
}  // namespace android
}  // namespace aidl
