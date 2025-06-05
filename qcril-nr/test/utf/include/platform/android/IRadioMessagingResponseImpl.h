/*===========================================================================
 *  Copyright (c) 2022 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 *===========================================================================*/

#pragma once

#include "aidl/android/hardware/radio/messaging/IRadioMessagingResponse.h"
#include "SmsAidlUtil.h"

namespace aidl {
namespace android {
namespace hardware {
namespace radio {
namespace messaging {
class IRadioMessagingResponseImpl : public IRadioMessagingResponse {
public:
  IRadioMessagingResponseImpl() {}

  virtual ~IRadioMessagingResponseImpl() {}

  ::ndk::ScopedAStatus acknowledgeIncomingGsmSmsWithPduResponse(
      const ::aidlradio::RadioResponseInfo& in_info) override;

  ::ndk::ScopedAStatus acknowledgeLastIncomingCdmaSmsResponse(
      const ::aidlradio::RadioResponseInfo& in_info) override;

  ::ndk::ScopedAStatus acknowledgeLastIncomingGsmSmsResponse(
      const ::aidlradio::RadioResponseInfo& in_info) override;

  ::ndk::ScopedAStatus acknowledgeRequest(int32_t in_serial) override;

  ::ndk::ScopedAStatus deleteSmsOnRuimResponse(
      const ::aidlradio::RadioResponseInfo& in_info) override;

  ::ndk::ScopedAStatus deleteSmsOnSimResponse(const ::aidlradio::RadioResponseInfo& in_info) override;

  ::ndk::ScopedAStatus getCdmaBroadcastConfigResponse(
      const ::aidlradio::RadioResponseInfo& in_info,
      const std::vector<::aidlmsg::CdmaBroadcastSmsConfigInfo>& in_configs) override;

  ::ndk::ScopedAStatus getGsmBroadcastConfigResponse(
      const ::aidlradio::RadioResponseInfo& in_info,
      const std::vector<::aidlmsg::GsmBroadcastSmsConfigInfo>& in_configs) override;

  ::ndk::ScopedAStatus getSmscAddressResponse(const ::aidlradio::RadioResponseInfo& in_info,
                                              const std::string& in_smsc) override;

  ::ndk::ScopedAStatus reportSmsMemoryStatusResponse(
      const ::aidlradio::RadioResponseInfo& in_info) override;

  ::ndk::ScopedAStatus sendCdmaSmsExpectMoreResponse(const ::aidlradio::RadioResponseInfo& in_info,
                                                     const ::aidlmsg::SendSmsResult& in_sms) override;

  ::ndk::ScopedAStatus sendCdmaSmsResponse(const ::aidlradio::RadioResponseInfo& in_info,
                                           const ::aidlmsg::SendSmsResult& in_sms) override;

  ::ndk::ScopedAStatus sendImsSmsResponse(const ::aidlradio::RadioResponseInfo& in_info,
                                          const ::aidlmsg::SendSmsResult& in_sms) override;

  ::ndk::ScopedAStatus sendSmsExpectMoreResponse(const ::aidlradio::RadioResponseInfo& in_info,
                                                 const ::aidlmsg::SendSmsResult& in_sms) override;

  ::ndk::ScopedAStatus sendSmsResponse(const ::aidlradio::RadioResponseInfo& in_info,
                                       const ::aidlmsg::SendSmsResult& in_sms) override;

  ::ndk::ScopedAStatus setCdmaBroadcastActivationResponse(
      const ::aidlradio::RadioResponseInfo& in_info) override;

  ::ndk::ScopedAStatus setCdmaBroadcastConfigResponse(
      const ::aidlradio::RadioResponseInfo& in_info) override;

  ::ndk::ScopedAStatus setGsmBroadcastActivationResponse(
      const ::aidlradio::RadioResponseInfo& in_info) override;

  ::ndk::ScopedAStatus setGsmBroadcastConfigResponse(
      const ::aidlradio::RadioResponseInfo& in_info) override;

  ::ndk::ScopedAStatus setSmscAddressResponse(const ::aidlradio::RadioResponseInfo& in_info) override;

  ::ndk::ScopedAStatus writeSmsToRuimResponse(const ::aidlradio::RadioResponseInfo& in_info,
                                              int32_t in_index) override;

  ::ndk::ScopedAStatus writeSmsToSimResponse(const ::aidlradio::RadioResponseInfo& in_info,
                                             int32_t in_index) override;

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
