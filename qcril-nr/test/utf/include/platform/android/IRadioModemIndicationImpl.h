/*===========================================================================
 *  Copyright (c) 2022 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 *===========================================================================*/

#pragma once

#include "aidl/android/hardware/radio/modem/IRadioModemIndication.h"
#include "ril_utf_rild_sim.h"

namespace aidlradio {
  using namespace aidl::android::hardware::radio;
}

namespace aidlmodem {
  using namespace aidl::android::hardware::radio::modem;
}

namespace aidl {
namespace android {
namespace hardware {
namespace radio {
namespace modem {
class IRadioModemIndicationImpl : public IRadioModemIndication {
public:
  IRadioModemIndicationImpl() {}

  virtual ~IRadioModemIndicationImpl() {}

  ::ndk::ScopedAStatus hardwareConfigChanged(::aidlradio::RadioIndicationType in_type,
    const std::vector<::aidlmodem::HardwareConfig>& in_configs) override;

  ::ndk::ScopedAStatus modemReset(::aidlradio::RadioIndicationType in_type,
    const std::string& in_reason) override;

  ::ndk::ScopedAStatus radioCapabilityIndication(::aidlradio::RadioIndicationType in_type,
    const ::aidlmodem::RadioCapability& in_rc) override;

  ::ndk::ScopedAStatus radioStateChanged(::aidlradio::RadioIndicationType in_type,
    ::aidlmodem::RadioState in_radioState) override;

  ::ndk::ScopedAStatus rilConnected(::aidlradio::RadioIndicationType in_type) override;

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
}  // namespace modem
}  // namespace radio
}  // namespace hardware
}  // namespace android
}  // namespace aidl
