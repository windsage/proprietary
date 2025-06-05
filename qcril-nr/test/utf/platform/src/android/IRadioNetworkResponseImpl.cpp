/*===========================================================================
 *    Copyright (c) 2022 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *===========================================================================*/

#include "platform/android/IRadioNetworkResponseImpl.h"
#include "ril_utf_rild_sim.h"

::ndk::ScopedAStatus aidlnetwork::IRadioNetworkResponseImpl::getAvailableBandModesResponse(
    const ::aidlradio::RadioResponseInfo& info,
    const std::vector<::aidlnetwork::RadioBandMode>& bandModes)
{
  int num_bands = bandModes.size();
  size_t payload_len = 0;
  int* payload = nullptr;
  if (num_bands > 0) {
    payload = (int*)malloc(sizeof(int) * (num_bands + 1));
    payload_len = sizeof(int) * (num_bands + 1);
    memset(payload, 0, payload_len);
    payload[0] = num_bands;

    for (int i = 0; i < num_bands; ++i) {
      payload[i + 1] = static_cast<int>(bandModes[i]);
    }
  }

  int slot = acquire_expectation_table();
  enqueue_ril_expect(payload,  // payload
                     payload_len,
                     RIL_REQUEST_QUERY_AVAILABLE_BAND_MODE,
                     info.serial,            // serial_id
                     (RIL_Errno)info.error,  // RIL error
                     ril_utf_ril_response);  // type
  release_expectation_table(slot);

  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlnetwork::IRadioNetworkResponseImpl::getAvailableNetworksResponse(
    const ::aidlradio::RadioResponseInfo& info,
    const std::vector<::aidlnetwork::OperatorInfo>& networkInfos)
{
  auto data = convertAidlNetworkscanResponseToRil(networkInfos);
  if (data != nullptr) {
    int slot = acquire_expectation_table();
    enqueue_ril_expect(data,  // payload
                       sizeof(*data),
                       RIL_REQUEST_QUERY_AVAILABLE_NETWORKS,  // msg_id
                       info.serial,                           // serial_id
                       (RIL_Errno)info.error,                 // RIL error
                       ril_utf_ril_response);                 // type

    release_expectation_table(slot);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlnetwork::IRadioNetworkResponseImpl::setBarringPasswordResponse(
    const ::aidlradio::RadioResponseInfo& info)
{
  int slot = acquire_expectation_table();
  enqueue_ril_expect(nullptr,  // payload
                     0,
                     RIL_REQUEST_CHANGE_BARRING_PASSWORD,  // msg_id
                     info.serial,                          // serial_id
                     (RIL_Errno)info.error,                // RIL error
                     ril_utf_ril_response);                // type
  release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlnetwork::IRadioNetworkResponseImpl::setSuppServiceNotificationsResponse(
    const ::aidlradio::RadioResponseInfo& info)
{
  int slot = acquire_expectation_table();
    enqueue_ril_expect(
            nullptr,                            // payload
            0,
            RIL_REQUEST_SET_SUPP_SVC_NOTIFICATION, // msg_id
            info.serial,                        // serial_id
            (RIL_Errno)info.error,              // RIL error
            ril_utf_ril_response);              // type
    release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlnetwork::IRadioNetworkResponseImpl::setBandModeResponse(
    const ::aidlradio::RadioResponseInfo& info)
{
  int slot = acquire_expectation_table();
    enqueue_ril_expect(
                nullptr,                        // payload
                0,
                RIL_REQUEST_SET_BAND_MODE,
                info.serial,                    // serial_id
                (RIL_Errno)info.error,          // RIL error
                ril_utf_ril_response);          // type
    release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlnetwork::IRadioNetworkResponseImpl::setSystemSelectionChannelsResponse(
    const ::aidlradio::RadioResponseInfo& info)
{
  int slot = acquire_expectation_table();
    enqueue_ril_expect(
            nullptr,                        // payload
            0,
            RIL_REQUEST_SET_SYSTEM_SELECTION_CHANNELS, // msg_id
            info.serial,                    // serial_id
            (RIL_Errno)info.error,          // RIL error
            ril_utf_ril_response);          // type
    release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlnetwork::IRadioNetworkResponseImpl::startNetworkScanResponse(
    const ::aidlradio::RadioResponseInfo& info)
{
  int slot = acquire_expectation_table();
  enqueue_ril_expect(
            nullptr,                        // payload
            0,
            RIL_REQUEST_START_NETWORK_SCAN, // msg_id
            info.serial,                    // serial_id
            (RIL_Errno)info.error,          // RIL error
            ril_utf_ril_response);          // type
    release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlnetwork::IRadioNetworkResponseImpl::stopNetworkScanResponse(
    const ::aidlradio::RadioResponseInfo& info)
{
  int slot = acquire_expectation_table();
  enqueue_ril_expect(
            nullptr,                        // payload
            0,
            RIL_REQUEST_STOP_NETWORK_SCAN,  // msg_id
            info.serial,                    // serial_id
            (RIL_Errno)info.error,          // RIL error
            ril_utf_ril_response);          // type
    release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlnetwork::IRadioNetworkResponseImpl::setLocationUpdatesResponse(
    const ::aidlradio::RadioResponseInfo& in_info)
{
  int slot = acquire_expectation_table();
  enqueue_ril_expect(NULL,
                     0,
                     RIL_REQUEST_SET_LOCATION_UPDATES,
                     in_info.serial,
                     (RIL_Errno)in_info.error,
                     ril_utf_ril_response);
  release_expectation_table(slot);

  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlnetwork::IRadioNetworkResponseImpl::setUsageSettingResponse(
    const ::aidlradio::RadioResponseInfo& in_info)
{
  int slot = acquire_expectation_table();
  enqueue_ril_expect(nullptr,
                     0,
                     RIL_REQUEST_SET_USAGE_SETTING,
                     in_info.serial,
                     (RIL_Errno)in_info.error,
                     ril_utf_ril_response);
  release_expectation_table(slot);

  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlnetwork::IRadioNetworkResponseImpl::getUsageSettingResponse(
    const ::aidlradio::RadioResponseInfo& in_info, ::aidlnetwork::UsageSetting in_usageSetting)
{
  size_t payload_len = sizeof(RIL_UsageSettingMode);
  RIL_UsageSettingMode* payload = (RIL_UsageSettingMode*)malloc(payload_len);

  if (convertAidlUsageSettingtoRil(in_usageSetting, *payload) == RIL_E_SUCCESS) {
    int slot = acquire_expectation_table();
    enqueue_ril_expect((void*)payload,
                       payload_len,
                       RIL_REQUEST_GET_USAGE_SETTING,
                       in_info.serial,
                       (RIL_Errno)in_info.error,
                       ril_utf_ril_response);
    release_expectation_table(slot);
  }

  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlnetwork::IRadioNetworkResponseImpl::getSignalStrengthResponse(
    const ::aidlradio::RadioResponseInfo& in_info,
    const ::aidlnetwork::SignalStrength& in_signalStrength)
{
  size_t payload_len = sizeof(RIL_SignalStrength);
  RIL_SignalStrength* payload = (RIL_SignalStrength*)malloc(payload_len);
  memset(payload, 0x0, payload_len);
  convertAidlStrengthToRil(in_signalStrength, payload);

  int slot = acquire_expectation_table();
  enqueue_ril_expect(payload,  // payload
                     payload_len,
                     RIL_REQUEST_SIGNAL_STRENGTH,  // msg_id
                     in_info.serial,               // serial_id
                     (RIL_Errno)in_info.error,     // RIL error
                     ril_utf_ril_response);        // type
  release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlnetwork::IRadioNetworkResponseImpl::getVoiceRadioTechnologyResponse(
    const ::aidlradio::RadioResponseInfo& in_info, ::aidlradio::RadioTechnology in_rat)
{
  size_t payload_len = sizeof(RIL_RadioTechnology);
  RIL_RadioTechnology* payload = (RIL_RadioTechnology*)malloc(payload_len);
  memset(payload, 0x0, payload_len);

  *payload = static_cast<RIL_RadioTechnology>(in_rat);

  int slot = acquire_expectation_table();
  enqueue_ril_expect(payload,  // payload
                     payload_len,
                     RIL_REQUEST_VOICE_RADIO_TECH,  // msg_id
                     in_info.serial,                // serial_id
                     (RIL_Errno)in_info.error,      // RIL error
                     ril_utf_ril_response);         // type

  release_expectation_table(slot);

  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidl::android::hardware::radio::network::IRadioNetworkResponseImpl::setIndicationFilterResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) {
    int slot = acquire_expectation_table();
    enqueue_ril_expect(
          nullptr,
          0,
          RIL_REQUEST_SET_UNSOLICITED_RESPONSE_FILTER,
          in_info.serial,
          (RIL_Errno)in_info.error,
          ril_utf_ril_response);
    release_expectation_table(slot);

    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlnetwork::IRadioNetworkResponseImpl::getBarringInfoResponse(
    const ::aidlradio::RadioResponseInfo& in_info,
    const ::aidlnetwork::CellIdentity& cellIdentity,
    const std::vector<::aidlnetwork::BarringInfo>& barringInfo) {

    size_t payload_len = sizeof(RIL_CellBarringInfo);
    RIL_CellBarringInfo* payload =
            (RIL_CellBarringInfo*)malloc(payload_len);
    assert(payload != nullptr);
    memset(payload, 0x00, payload_len);
    convertCellBarringInfoNr(barringInfo, payload);
    int slot = acquire_expectation_table();
    enqueue_ril_expect(
          payload,
          payload_len,
          RIL_REQUEST_GET_CELL_BARRING_INFO,
          in_info.serial,
          (RIL_Errno)in_info.error,
          ril_utf_ril_response);
    release_expectation_table(slot);

    return ndk::ScopedAStatus::ok();
}
