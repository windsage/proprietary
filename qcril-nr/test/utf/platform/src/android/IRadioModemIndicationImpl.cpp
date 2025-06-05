/*===========================================================================
 *    Copyright (c) 2022 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *===========================================================================*/

#include "platform/android/IRadioModemIndicationImpl.h"

::ndk::ScopedAStatus aidlmodem::IRadioModemIndicationImpl::hardwareConfigChanged(
    ::aidlradio::RadioIndicationType in_type,
    const std::vector<::aidlmodem::HardwareConfig>& in_configs) {
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlmodem::IRadioModemIndicationImpl::modemReset(
    ::aidlradio::RadioIndicationType in_type,
    const std::string& in_reason) {
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlmodem::IRadioModemIndicationImpl::radioCapabilityIndication(
    ::aidlradio::RadioIndicationType in_type,
    const ::aidlmodem::RadioCapability& in_rc) {
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlmodem::IRadioModemIndicationImpl::radioStateChanged(
    ::aidlradio::RadioIndicationType in_type,
    ::aidlmodem::RadioState in_radioState) {
    size_t payload_len = sizeof(int32_t);
    int32_t* payload = (int32_t*)malloc(payload_len);
    *payload = static_cast<int32_t>(in_radioState);

    int my_expect_slot = acquire_expectation_table();
    enqueue_ril_expect(
            payload,                            // payload
            payload_len,
            RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED, // msg_id
            -1,                                 // serial_id
            RIL_E_SUCCESS,                      // RIL error
            ril_utf_ril_unsol_response);        // type
    release_expectation_table(my_expect_slot);

    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlmodem::IRadioModemIndicationImpl::rilConnected(
    ::aidlradio::RadioIndicationType in_type) {
    return ndk::ScopedAStatus::ok();
}