/*===========================================================================
 *    Copyright (c) 2022 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *===========================================================================*/

#include "platform/android/IRadioSimIndicationImpl.h"

::ndk::ScopedAStatus aidlsim::IRadioSimIndicationImpl::simStatusChanged(::aidl::android::hardware::radio::RadioIndicationType in_type) {
    int my_expect_slot = acquire_expectation_table();
    enqueue_ril_expect(
                nullptr,                                        // payload
                0,
                RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED,          // msg_id
                -1,                                             // serial_id
                RIL_E_SUCCESS,                                  // RIL error
                ril_utf_ril_unsol_response);                    // type
    release_expectation_table(my_expect_slot);

    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlsim::IRadioSimIndicationImpl::cdmaSubscriptionSourceChanged(::aidl::android::hardware::radio::RadioIndicationType in_type,
                        ::aidl::android::hardware::radio::sim::CdmaSubscriptionSource cdmaSource) {
    RIL_CdmaSubscriptionSource *payload =
      (RIL_CdmaSubscriptionSource*)malloc(sizeof(RIL_CdmaSubscriptionSource));
    if (payload != nullptr) {
        *payload = static_cast<RIL_CdmaSubscriptionSource>(cdmaSource);
        int my_expect_slot = acquire_expectation_table();
        enqueue_ril_expect(
                payload,                            // payload
                sizeof(*payload),
                RIL_UNSOL_CDMA_SUBSCRIPTION_SOURCE_CHANGED, // msg_id
                -1,                                 // serial_id
                RIL_E_SUCCESS,                      // RIL error
                ril_utf_ril_unsol_response);        // type
        release_expectation_table(my_expect_slot);
    }

    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlsim::IRadioSimIndicationImpl::subscriptionStatusChanged(::aidl::android::hardware::radio::RadioIndicationType in_type, bool activate) {
    auto payload = (int32_t*)malloc(sizeof(int32_t));
    if (payload != nullptr) {
        *payload = activate ? 1:0;
        int my_expect_slot = acquire_expectation_table();
        enqueue_ril_expect(
                payload,                            // payload
                sizeof(*payload),
                RIL_UNSOL_UICC_SUBSCRIPTION_STATUS_CHANGED, // msg_id
                -1,                                 // serial_id
                RIL_E_SUCCESS,                      // RIL error
                ril_utf_ril_unsol_response);        // type
        release_expectation_table(my_expect_slot);
    }
    return ndk::ScopedAStatus::ok();
}