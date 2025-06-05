/*===========================================================================
 *    Copyright (c) 2022 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *===========================================================================*/

#include "platform/android/IRadioNetworkIndicationImpl.h"
#include "ril_utf_rild_sim.h"
#include "platform/android/NasAidlUtil.h"

::ndk::ScopedAStatus aidlnetwork::IRadioNetworkIndicationImpl::imsNetworkStateChanged(::aidl::android::hardware::radio::RadioIndicationType type) {
  int my_expect_slot = acquire_expectation_table();
    enqueue_ril_expect(
                nullptr,                                        // payload
                0,
                RIL_UNSOL_RESPONSE_IMS_NETWORK_STATE_CHANGED,   // msg_id
                -1,                                             // serial_id
                RIL_E_SUCCESS,                                  // RIL error
                ril_utf_ril_unsol_response);                    // type
    release_expectation_table(my_expect_slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlnetwork::IRadioNetworkIndicationImpl::networkStateChanged(::aidl::android::hardware::radio::RadioIndicationType in_type) {
  int my_expect_slot = acquire_expectation_table();
    enqueue_ril_expect(
                nullptr,                                        // payload
                0,
                RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED, // msg_id
                -1,                                             // serial_id
                RIL_E_SUCCESS,                                  // RIL error
                ril_utf_ril_unsol_response);                    // type
    release_expectation_table(my_expect_slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlnetwork::IRadioNetworkIndicationImpl::networkScanResult(::aidl::android::hardware::radio::RadioIndicationType in_type,
  const ::aidl::android::hardware::radio::network::NetworkScanResult& result) {
    auto payload = (RIL_NetworkScanResult*)malloc(sizeof(RIL_NetworkScanResult));
    size_t payload_len = sizeof(RIL_NetworkScanResult);
    memset(payload, 0, payload_len);
    convertNetworkScanResultToRil(result, *payload);
    if (payload != nullptr) {
        int my_expect_slot = acquire_expectation_table();
        enqueue_ril_expect(
                    payload,                                        // payload
                    payload_len,
                    RIL_UNSOL_NETWORK_SCAN_RESULT,                  // msg_id
                    -1,                                             // serial_id
                    RIL_E_SUCCESS,                                  // RIL error
                    ril_utf_ril_unsol_response);                    // type
        release_expectation_table(my_expect_slot);
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlnetwork::IRadioNetworkIndicationImpl::suppSvcNotify(::aidl::android::hardware::radio::RadioIndicationType type,
  const ::aidl::android::hardware::radio::network::SuppSvcNotification& suppSvc) {
  RIL_SuppSvcNotification *payload = (RIL_SuppSvcNotification *)malloc(sizeof(RIL_SuppSvcNotification));
    if (payload != nullptr) {
        payload->notificationType = (suppSvc.isMT ? 1 : 0);  //MO=0, MT=1
        payload->code = suppSvc.code;
        payload->index = suppSvc.index;
        payload->type = suppSvc.type;
        payload->number = nullptr;
        if (!suppSvc.number.empty()) {
          payload->number = strdup(suppSvc.number.c_str());
        }
        int my_expect_slot = acquire_expectation_table();
        enqueue_ril_expect(
                    payload,                                        // payload
                    sizeof(RIL_SuppSvcNotification),
                    RIL_UNSOL_SUPP_SVC_NOTIFICATION,                // msg_id
                    -1,                                             // serial_id
                    RIL_E_SUCCESS,                                  // RIL error
                    ril_utf_ril_unsol_response);                    // type
        release_expectation_table(my_expect_slot);
    }
  return ndk::ScopedAStatus::ok();
}

