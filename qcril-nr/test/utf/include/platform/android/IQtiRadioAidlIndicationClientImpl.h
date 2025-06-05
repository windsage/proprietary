/*===========================================================================
 *
 *    Copyright (c) 2022 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 *===========================================================================*/

#pragma once

#include <aidl/vendor/qti/hardware/radio/qtiradio/IQtiRadioIndication.h>

namespace qtiradioaidl {
  using namespace aidl::vendor::qti::hardware::radio::qtiradio;
}

class IQtiRadioAidlIndicationClientImpl : public qtiradioaidl::IQtiRadioIndication {
  virtual ::ndk::ScopedAStatus onNrIconTypeChange(::aidl::vendor::qti::hardware::radio::qtiradio::NrIconType in_iconType) override;
  virtual ::ndk::ScopedAStatus onNrConfigChange(::aidl::vendor::qti::hardware::radio::qtiradio::NrConfig in_config) override;
  virtual ::ndk::ScopedAStatus onImeiChange(const ::aidl::vendor::qti::hardware::radio::qtiradio::ImeiInfo & in_info) override;
  virtual ::ndk::ScopedAStatus onDdsSwitchCapabilityChange() override;
  virtual ::ndk::ScopedAStatus onDdsSwitchCriteriaChange(bool in_telephonyDdsSwitch) override;
  virtual ::ndk::ScopedAStatus onDdsSwitchRecommendation(int32_t in_recommendedSlotId) override;
  virtual ::ndk::ScopedAStatus onDataDeactivateDelayTime(int64_t in_delayTimeMilliSecs) override;
  virtual ::ndk::ScopedAStatus onEpdgOverCellularDataSupported(bool in_support) override;
  virtual ::ndk::ScopedAStatus onMcfgRefresh(::aidl::vendor::qti::hardware::radio::qtiradio::McfgRefreshState in_refreshState, int32_t in_slotId) override;
  virtual ::ndk::ScopedAStatus networkScanResult(const ::aidl::vendor::qti::hardware::radio::qtiradio::QtiNetworkScanResult & in_result) override;
  virtual ::ndk::ScopedAStatus getInterfaceVersion(int32_t* _aidl_return) override;
  virtual ::ndk::ScopedAStatus getInterfaceHash(std::string* _aidl_return) override;
  ::ndk::SpAIBinder asBinder() {
    return ::ndk::SpAIBinder();
  }
  bool isRemote() {
    return false;
  }
};
