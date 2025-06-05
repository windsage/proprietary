/*===========================================================================
 *    Copyright (c) 2022 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *===========================================================================*/

#include "platform/android/IRadioModemResponseImpl.h"
#include "ril_utf_rild_sim.h"

#if defined(FEATURE_TARGET_GLIBC_x86) || defined(QMI_RIL_UTF)
extern "C" size_t strlcpy(char*, const char*, size_t);
#endif

::ndk::ScopedAStatus aidlmodem::IRadioModemResponseImpl::getImeiResponse(
    const aidlradio::RadioResponseInfo& info, const std::optional<aidlmodem::ImeiInfo>& imeiInfo)
{
  size_t payload_len = 0;
  RIL_ImeiInfo* payload = nullptr;
  if (imeiInfo) {
    payload_len = sizeof(RIL_ImeiInfo);
    payload = (RIL_ImeiInfo*)malloc(payload_len);
    assert(payload != nullptr);

    RIL_ImeiInfo* imei_payload = (RIL_ImeiInfo*)payload;
    imei_payload->imeiType = static_cast<RIL_ImeiType>(imeiInfo->type);
    imei_payload->imeiLen = imeiInfo->imei.size();
    strlcpy(&imei_payload->imei[0], imeiInfo->imei.c_str(), imei_payload->imeiLen);
  }

  int slot = acquire_expectation_table();
  enqueue_ril_expect(payload,  // payload
                     payload_len,
                     RIL_REQUEST_GET_IMEI,
                     info.serial,            // serial_id
                     (RIL_Errno)info.error,  // RIL error
                     ril_utf_ril_response);  // type
  release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlmodem::IRadioModemResponseImpl::nvResetConfigResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& in_info)
{
  int slot = acquire_expectation_table();
  enqueue_ril_expect(nullptr,
                     0,
                     RIL_REQUEST_NV_RESET_CONFIG,
                     in_info.serial,
                     (RIL_Errno)in_info.error,
                     ril_utf_ril_response);
  release_expectation_table(slot);

  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlmodem::IRadioModemResponseImpl::setRadioCapabilityResponse(
    const aidlradio::RadioResponseInfo& in_info, const aidlmodem::RadioCapability& in_rc)
{
  RIL_RadioCapability* payload = (RIL_RadioCapability*)malloc(sizeof(RIL_RadioCapability));
  memset(payload, 0x0, sizeof(RIL_RadioCapability));
  int32_t payload_len = 0;
  if (convertRadioCapabilityToRil(in_rc, *payload)) {
    payload_len = sizeof(*payload);
  } else {
    free(payload);
    payload = nullptr;
  }

  int slot = acquire_expectation_table();
  enqueue_ril_expect((void*)payload,
                     payload_len,
                     RIL_REQUEST_SET_RADIO_CAPABILITY,
                     in_info.serial,
                     (RIL_Errno)in_info.error,
                     ril_utf_ril_response);
  release_expectation_table(slot);

  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlmodem::IRadioModemResponseImpl::getRadioCapabilityResponse(
    const aidlradio::RadioResponseInfo& in_info, const aidlmodem::RadioCapability& in_rc)
{
  size_t payload_len = 0;
  RIL_RadioCapability* payload = (RIL_RadioCapability*)malloc(sizeof(RIL_RadioCapability));
  memset(payload, 0x0, sizeof(RIL_RadioCapability));

  if (convertRadioCapabilityToRil(in_rc, *payload)) {
    payload_len = sizeof(*payload);
  } else {
    free(payload);
    payload = NULL;
  }

  int slot = acquire_expectation_table();
  enqueue_ril_expect((void*)payload,
                     payload_len,
                     RIL_REQUEST_GET_RADIO_CAPABILITY,
                     in_info.serial,
                     (RIL_Errno)in_info.error,
                     ril_utf_ril_response);
  release_expectation_table(slot);

  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidl::android::hardware::radio::modem::IRadioModemResponseImpl::
  sendDeviceStateResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) {
    int slot = acquire_expectation_table();
    enqueue_ril_expect(
        NULL,
        0,
        RIL_REQUEST_SEND_DEVICE_STATE,
        in_info.serial,
        (RIL_Errno)in_info.error,
        ril_utf_ril_response);
    release_expectation_table(slot);
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlmodem::IRadioModemResponseImpl::acknowledgeRequest(
    int32_t in_serial) {
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlmodem::IRadioModemResponseImpl::enableModemResponse(
    const ::aidlradio::RadioResponseInfo& in_info) {
    int slot = acquire_expectation_table();

    enqueue_ril_expect(
            nullptr,                      // payload
            0,
            RIL_REQUEST_ENABLE_MODEM,     // msg_id
            in_info.serial,                  // serial_id
            (RIL_Errno)in_info.error,        // RIL error
            ril_utf_ril_response);        // type

    release_expectation_table(slot);
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlmodem::IRadioModemResponseImpl::getBasebandVersionResponse(
    const ::aidlradio::RadioResponseInfo& in_info,
    const std::string& in_version) {

    size_t payload_len = sizeof(ril_request_get_baseband_version_type_t);
    ril_request_get_baseband_version_type_t* payload =
        (ril_request_get_baseband_version_type_t*)malloc(payload_len);
    memset(payload, 0x0, payload_len);
    strlcpy(payload->swver, in_version.c_str(), SW_VERSION_MAX);

    int slot = acquire_expectation_table();
    enqueue_ril_expect(
            payload,                            // payload
            payload_len,
            RIL_REQUEST_BASEBAND_VERSION,       // msg_id
            in_info.serial,                        // serial_id
            (RIL_Errno)in_info.error,              // RIL error
            ril_utf_ril_response);              // type
    release_expectation_table(slot);
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlmodem::IRadioModemResponseImpl::getDeviceIdentityResponse(
    const ::aidlradio::RadioResponseInfo& in_info,
    const std::string& in_imei, const std::string& in_imeisv,
    const std::string& in_esn, const std::string& in_meid) {
    size_t payload_len = sizeof(ril_request_device_identity_type_t);
    ril_request_device_identity_type_t* payload =
        (ril_request_device_identity_type_t *)malloc(payload_len);
    memset(payload, 0x0, payload_len);
    convertToRil(in_imei, in_imeisv, in_esn, in_meid, payload);

    int slot = acquire_expectation_table();
    enqueue_ril_expect(
            payload,                            // payload
            payload_len,
            RIL_REQUEST_DEVICE_IDENTITY,        // msg_id
            in_info.serial,                        // serial_id
            (RIL_Errno)in_info.error,              // RIL error
            ril_utf_ril_response);              // type
    release_expectation_table(slot);
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlmodem::IRadioModemResponseImpl::getHardwareConfigResponse(
    const ::aidlradio::RadioResponseInfo& in_info,
    const std::vector<::aidlmodem::HardwareConfig>& in_config) {
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlmodem::IRadioModemResponseImpl::getModemActivityInfoResponse(
    const ::aidlradio::RadioResponseInfo& in_info,
    const ::aidlmodem::ActivityStatsInfo& in_activityInfo) {
    size_t payload_len = sizeof(RIL_ActivityStatsInfo);
    RIL_ActivityStatsInfo* payload = (RIL_ActivityStatsInfo*)malloc(payload_len);
    convertAidlActivityStatsToRil(in_activityInfo, *payload);

    memset(payload, 0x0, payload_len);
    int slot = acquire_expectation_table();
    enqueue_ril_expect(
            payload,                            // payload
            payload_len,
            RIL_REQUEST_GET_ACTIVITY_INFO,        // msg_id
            in_info.serial,                        // serial_id
            (RIL_Errno)in_info.error,              // RIL error
            ril_utf_ril_response);              // type
    release_expectation_table(slot);
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlmodem::IRadioModemResponseImpl::getModemStackStatusResponse(
    const ::aidlradio::RadioResponseInfo& in_info, bool in_isEnabled) {
    int* payload = (int*)malloc(sizeof(int));
    *payload = in_isEnabled ? 1 : 0;

    int slot = acquire_expectation_table();
    enqueue_ril_expect(
            payload,                            // payload
            sizeof(int),
            RIL_REQUEST_GET_MODEM_STACK_STATUS, // msg_id
            in_info.serial,                        // serial_id
            (RIL_Errno)in_info.error,              // RIL error
            ril_utf_ril_response);              // type

    release_expectation_table(slot);
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlmodem::IRadioModemResponseImpl::nvReadItemResponse(
    const ::aidlradio::RadioResponseInfo& in_info,
    const std::string& in_result) {
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlmodem::IRadioModemResponseImpl::nvWriteCdmaPrlResponse(
    const ::aidlradio::RadioResponseInfo& in_info) {
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlmodem::IRadioModemResponseImpl::nvWriteItemResponse(
    const ::aidlradio::RadioResponseInfo& in_info) {
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlmodem::IRadioModemResponseImpl::requestShutdownResponse(
    const ::aidlradio::RadioResponseInfo& in_info) {
    int slot = acquire_expectation_table();
    enqueue_ril_expect(
                nullptr,                        // payload
                0,
                RIL_REQUEST_SHUTDOWN,           // msg_id
                in_info.serial,                    // serial_id
                (RIL_Errno)in_info.error,          // RIL error
                ril_utf_ril_response);          // type
    release_expectation_table(slot);
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlmodem::IRadioModemResponseImpl::setRadioPowerResponse(
    const ::aidlradio::RadioResponseInfo& in_info) {
    int slot = acquire_expectation_table();
    enqueue_ril_expect(
            nullptr,                            // payload
            0,
            RIL_REQUEST_RADIO_POWER,            // msg_id
            in_info.serial,                        // serial_id
            (RIL_Errno)in_info.error,              // RIL error
            ril_utf_ril_response);              // type
    release_expectation_table(slot);
    return ndk::ScopedAStatus::ok();
}
