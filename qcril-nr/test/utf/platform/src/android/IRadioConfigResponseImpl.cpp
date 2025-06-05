/*===========================================================================
 *    Copyright (c) 2022 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *===========================================================================*/

#include "platform/android/IRadioConfigResponseImpl.h"
#include "ril_utf_rild_sim.h"

::ndk::ScopedAStatus aidl::android::hardware::radio::config::IRadioConfigResponseImpl::
  getPhoneCapabilityResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info,
    const ::aidl::android::hardware::radio::config::PhoneCapability& in_phoneCapability)
{
    size_t payload_len = sizeof(RIL_PhoneCapability);
    RIL_PhoneCapability* payload =
            (RIL_PhoneCapability*)malloc(payload_len);

    payload->maxActiveData = in_phoneCapability.maxActiveData;
    payload->maxActiveInternetData = in_phoneCapability.maxActiveInternetData;
    payload->isInternetLingeringSupported = in_phoneCapability.isInternetLingeringSupported;

    for(size_t i = 0; i <in_phoneCapability.logicalModemIds.size();i++)
    {
       payload->logicalModemList[i].modemId = in_phoneCapability.logicalModemIds[i];
    }
    int slot = acquire_expectation_table();
    enqueue_ril_expect(
                        payload,                        // payload
                        payload_len,
                        RIL_REQUEST_CONFIG_GET_PHONE_CAPABILITY,   // msg_id
                        in_info.serial,                 // serial_id
                        (RIL_Errno)in_info.error,       // RIL error
                        ril_utf_ril_response);          // type
    release_expectation_table(slot);
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlconfig::IRadioConfigResponseImpl::getHalDeviceCapabilitiesResponse(
    const ::aidlradio::RadioResponseInfo& in_info, bool in_modemReducedFeatureSet1)
{
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlconfig::IRadioConfigResponseImpl::getNumOfLiveModemsResponse(
    const ::aidlradio::RadioResponseInfo& in_info, int8_t in_numOfLiveModems)
{
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlconfig::IRadioConfigResponseImpl::getSimSlotsStatusResponse(
    const ::aidlradio::RadioResponseInfo& in_info,
    const std::vector<::aidlconfig::SimSlotStatus>& in_slotStatus)
{
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlconfig::IRadioConfigResponseImpl::setNumOfLiveModemsResponse(
    const ::aidlradio::RadioResponseInfo& in_info)
{
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlconfig::IRadioConfigResponseImpl::setPreferredDataModemResponse(
    const ::aidlradio::RadioResponseInfo& in_info)
{
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlconfig::IRadioConfigResponseImpl::setSimSlotsMappingResponse(
    const ::aidlradio::RadioResponseInfo& in_info)
{
  return ndk::ScopedAStatus::ok();
}
