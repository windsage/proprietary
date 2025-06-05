/*===========================================================================
 *
 *    Copyright (c) 2018-2019,2022-2023 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 *===========================================================================*/

#include "platform/android/IRadioIndicationClientImpl.h"
#include "ril_utf_rild_sim.h"
#include "platform/android/NasHidlUtil.h"

Return<void> IRadioIndicationClientImpl::newSms(V1_0::RadioIndicationType type, const hidl_vec<uint8_t>& pdu) {
    return Void();
}
Return<void> IRadioIndicationClientImpl::cdmaNewSms(V1_0::RadioIndicationType type, const V1_0::CdmaSmsMessage& msg) {
    return Void();
}

Return<void> IRadioIndicationClientImpl::newBroadcastSms(V1_0::RadioIndicationType type,
        const hidl_vec<uint8_t>& pdu) {
    return Void();
}

Return<void> IRadioIndicationClientImpl::imsNetworkStateChanged(V1_0::RadioIndicationType type) {
    return Void();
}

Return<void> IRadioIndicationClientImpl::networkStateChanged(V1_0::RadioIndicationType type) {
    return Void();
}

Return<void> IRadioIndicationClientImpl::simStatusChanged(V1_0::RadioIndicationType type) {
    return Void();
}


Return<void> IRadioIndicationClientImpl::voiceRadioTechChanged(V1_0::RadioIndicationType type,
        V1_0::RadioTechnology rat) {
    auto payload = (uint32_t*)malloc(sizeof(uint32_t));
    if (payload != nullptr) {
        *payload = static_cast<int32_t>(rat);
        int my_expect_slot = acquire_expectation_table();
        enqueue_ril_expect(
                payload,                            // payload
                sizeof(*payload),
                RIL_UNSOL_VOICE_RADIO_TECH_CHANGED, // msg_id
                -1,                                 // serial_id
                RIL_E_SUCCESS,                      // RIL error
                ril_utf_ril_unsol_response);        // type
        release_expectation_table(my_expect_slot);
    }
    return Void();
}

Return<void> IRadioIndicationClientImpl::currentSignalStrength(V1_0::RadioIndicationType type,
        const V1_0::SignalStrength& signalStrength) {
    auto payload = (RIL_SignalStrength_v10*)malloc(sizeof(RIL_SignalStrength_v10));
    if (payload != nullptr) {
        convertHidlSignalStrengthToRil(signalStrength, *payload);

        int my_expect_slot = acquire_expectation_table();
        enqueue_ril_expect(
                payload,                            // payload
                sizeof(*payload),
                RIL_UNSOL_SIGNAL_STRENGTH,          // msg_id
                -1,                                 // serial_id
                RIL_E_SUCCESS,                      // RIL error
                ril_utf_ril_unsol_response);        // type
        release_expectation_table(my_expect_slot);
    }
    return Void();
}

Return<void> IRadioIndicationClientImpl::currentSignalStrength_1_2(V1_0::RadioIndicationType type,
        const V1_2::SignalStrength& signalStrength) {
    auto payload = (RIL_SignalStrength_v10*)malloc(sizeof(RIL_SignalStrength_v10));
    if (payload != nullptr) {
        convertHidlSignalStrengthToRil(signalStrength, *payload);

        int my_expect_slot = acquire_expectation_table();
        enqueue_ril_expect(
                payload,                            // payload
                sizeof(*payload),
                RIL_UNSOL_SIGNAL_STRENGTH,          // msg_id
                -1,                                 // serial_id
                RIL_E_SUCCESS,                      // RIL error
                ril_utf_ril_unsol_response);        // type
        release_expectation_table(my_expect_slot);
    }
    return Void();
}

Return<void> IRadioIndicationClientImpl::currentSignalStrength_1_4(V1_0::RadioIndicationType type,
        const V1_4::SignalStrength& signalStrength) {
    auto payload = (RIL_SignalStrength_v10*)malloc(sizeof(RIL_SignalStrength_v10));
    if (payload != nullptr) {
        convertHidlSignalStrengthToRil(signalStrength, *payload);

        int my_expect_slot = acquire_expectation_table();
        enqueue_ril_expect(
                payload,                            // payload
                sizeof(*payload),
                RIL_UNSOL_SIGNAL_STRENGTH,          // msg_id
                -1,                                 // serial_id
                RIL_E_SUCCESS,                      // RIL error
                ril_utf_ril_unsol_response);        // type
        release_expectation_table(my_expect_slot);
    }
    return Void();
}

Return<void> IRadioIndicationClientImpl::currentSignalStrength_1_6(V1_0::RadioIndicationType type,
        const V1_6::SignalStrength& signalStrength) {
    auto payload = (RIL_SignalStrength_v10*)malloc(sizeof(RIL_SignalStrength_v10));
    if (payload != nullptr) {
        convertHidlSignalStrengthToRil(signalStrength, *payload);

        int my_expect_slot = acquire_expectation_table();
        enqueue_ril_expect(
                payload,                            // payload
                sizeof(*payload),
                RIL_UNSOL_SIGNAL_STRENGTH,          // msg_id
                -1,                                 // serial_id
                RIL_E_SUCCESS,                      // RIL error
                ril_utf_ril_unsol_response);        // type
        release_expectation_table(my_expect_slot);
    }
    return Void();
}

Return<void> IRadioIndicationClientImpl::radioStateChanged(V1_0::RadioIndicationType type,
        V1_0::RadioState radioState) {
    return Void();
}

Return<void> IRadioIndicationClientImpl::subscriptionStatusChanged(V1_0::RadioIndicationType type,
        bool activate) {
    return Void();
}

Return<void> IRadioIndicationClientImpl::restrictedStateChanged(V1_0::RadioIndicationType type,
        V1_0::PhoneRestrictedState state) {
    auto payload = (int32_t*)malloc(sizeof(int32_t));
    if (payload != nullptr) {
        *payload = static_cast<int32_t>(state);
        int my_expect_slot = acquire_expectation_table();
        enqueue_ril_expect(
                payload,                            // payload
                sizeof(*payload),
                RIL_UNSOL_RESTRICTED_STATE_CHANGED, // msg_id
                -1,                                 // serial_id
                RIL_E_SUCCESS,                      // RIL error
                ril_utf_ril_unsol_response);        // type
        release_expectation_table(my_expect_slot);
    }
    return Void();
}

Return<void> IRadioIndicationClientImpl::cdmaSubscriptionSourceChanged(V1_0::RadioIndicationType type,
        V1_0::CdmaSubscriptionSource cdmaSource) {
    return Void();
}

Return<void> IRadioIndicationClientImpl::callStateChanged(V1_0::RadioIndicationType type) {
    return Void();
}

Return<void> IRadioIndicationClientImpl::indicateRingbackTone(V1_0::RadioIndicationType type, bool start) {
    return Void();
}

Return<void> IRadioIndicationClientImpl::callRing(V1_0::RadioIndicationType type, bool isGsm, const V1_0::CdmaSignalInfoRecord& record) {
    return Void();
}
Return<void> IRadioIndicationClientImpl::srvccStateNotify(V1_0::RadioIndicationType type, V1_0::SrvccState state) {
    return Void();
}
Return<void> IRadioIndicationClientImpl::cdmaCallWaiting(V1_0::RadioIndicationType type, const V1_0::CdmaCallWaiting& callWaitingRecord) {
    return Void();
}
Return<void> IRadioIndicationClientImpl::cdmaOtaProvisionStatus(V1_0::RadioIndicationType type, V1_0::CdmaOtaProvisionStatus status) {
    return Void();
}

Return<void> IRadioIndicationClientImpl::cdmaInfoRec(V1_0::RadioIndicationType type, const V1_0::CdmaInformationRecords& records) {
    return Void();
}

Return<void> IRadioIndicationClientImpl::onSupplementaryServiceIndication(V1_0::RadioIndicationType type, const V1_0::StkCcUnsolSsResult& ss) {
    return Void();
}
Return<void> IRadioIndicationClientImpl::stkCallControlAlphaNotify(V1_0::RadioIndicationType type, const hidl_string& alpha) {
    return Void();
}

Return<void> IRadioIndicationClientImpl::onUssd(V1_0::RadioIndicationType type, V1_0::UssdModeType modeType, const hidl_string& msg) {
    char **payload = (char **)malloc(2*sizeof(char *));
    if (payload != nullptr) {
      const char *mode = nullptr;
      switch(modeType) {
        case V1_0::UssdModeType::NOTIFY:
          mode = "0";
          break;
        case V1_0::UssdModeType::REQUEST:
          mode = "1";
          break;
        case V1_0::UssdModeType::NW_RELEASE:
          mode = "2";
          break;
        case V1_0::UssdModeType::LOCAL_CLIENT:
          mode = "3";
          break;
        case V1_0::UssdModeType::NOT_SUPPORTED:
          mode = "4";
          break;
        case V1_0::UssdModeType::NW_TIMEOUT:
          mode = "5";
          break;
      }
        if (mode) {
          int size = strlen(mode)+1;
          payload[0] = (char *)malloc(sizeof(char) * size);
          memcpy(payload[0], mode, size);
        }
        if (!msg.empty()) {
          int size = msg.size()+1;
          payload[1] = (char *)malloc(sizeof(char) * size);
          memcpy(payload[1], msg.c_str(), size);
        }
        int my_expect_slot = acquire_expectation_table();
        enqueue_ril_expect(
                    payload,                                        // payload
                    2*sizeof(char),
                    RIL_UNSOL_ON_USSD,                              // msg_id
                    -1,                                             // serial_id
                    RIL_E_SUCCESS,                                  // RIL error
                    ril_utf_ril_unsol_response);                    // type
        release_expectation_table(my_expect_slot);
    }
    return Void();
}

Return<void> IRadioIndicationClientImpl::nitzTimeReceived(V1_0::RadioIndicationType type, const hidl_string& nitzTime, uint64_t receivedTime) {
    int size = nitzTime.size()+1;
    char *payload = (char *)malloc(sizeof(char) * size);

    if (payload != nullptr) {
        memcpy(payload, nitzTime.c_str(), size);
    }

    int my_expect_slot = acquire_expectation_table();
    enqueue_ril_expect(
                payload,                                        // payload
                size,
                RIL_UNSOL_NITZ_TIME_RECEIVED,                              // msg_id
                -1,                                             // serial_id
                RIL_E_SUCCESS,                                  // RIL error
                ril_utf_ril_unsol_response);                    // type
    release_expectation_table(my_expect_slot);
    return Void();
}

Return<void> IRadioIndicationClientImpl::suppSvcNotify(V1_0::RadioIndicationType type, const V1_0::SuppSvcNotification& suppSvc) {
    return Void();
}

Return<void> IRadioIndicationClientImpl::currentEmergencyNumberList(
        V1_0::RadioIndicationType type,
        const hidl_vec<V1_4::EmergencyNumber>& emergencyNumberList) {
    return Void();
}

Return<void> IRadioIndicationClientImpl::networkScanResult_1_5(
    ::android::hardware::radio::V1_0::RadioIndicationType type,
    const ::android::hardware::radio::V1_5::NetworkScanResult& result) {
    return Void();
}

Return<void> IRadioIndicationClientImpl::networkScanResult_1_6(
    ::android::hardware::radio::V1_0::RadioIndicationType type,
    const ::android::hardware::radio::V1_6::NetworkScanResult& result) {
    return Void();
}
