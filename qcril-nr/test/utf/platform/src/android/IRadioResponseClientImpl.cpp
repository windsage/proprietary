/*===========================================================================
 *
 *    Copyright (c) 2018-2019,2022-2023 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 *===========================================================================*/

#include "platform/android/IRadioResponseClientImpl.h"
#include "ril_utf_rild_sim.h"
#include "platform/android/NasHidlUtil.h"
#include "platform/android/VoiceHidlUtil.h"
#include "platform/android/SimHidlUtil.h"

Return<void> IRadioResponseClientImpl::sendSmsResponse(const V1_0::RadioResponseInfo& info, const V1_0::SendSmsResult& sms) {
    return Void();
}

Return<void> IRadioResponseClientImpl::sendCdmaSmsResponse(const V1_0::RadioResponseInfo& info, const V1_0::SendSmsResult& sms) {
    return Void();
}

Return<void> IRadioResponseClientImpl::sendCdmaSmsExpectMoreResponse(const V1_0::RadioResponseInfo& info, const V1_0::SendSmsResult& sms) {
    return Void();
}

Return<void> IRadioResponseClientImpl::acknowledgeLastIncomingGsmSmsResponse(const V1_0::RadioResponseInfo& info) {
    return Void();
}

Return<void> IRadioResponseClientImpl::getImsRegistrationStateResponse(const V1_0::RadioResponseInfo& info, bool isRegistered, V1_0::RadioTechnologyFamily ratFamily) {
    auto payload = (int*)malloc(sizeof(int)*2);
    if (payload != nullptr) {
        payload[0] = isRegistered ? 1 : 0;
        payload[1] = (ratFamily == V1_0::RadioTechnologyFamily::THREE_GPP) ?
            RADIO_TECH_3GPP: RADIO_TECH_3GPP2;

        int slot = acquire_expectation_table();
        enqueue_ril_expect(
                payload,                            // payload
                sizeof(int)*2,
                RIL_REQUEST_IMS_REGISTRATION_STATE, // msg_id
                info.serial,                        // serial_id
                (RIL_Errno)info.error,              // RIL error
                ril_utf_ril_response);              // type
        release_expectation_table(slot);
    }
    return Void();
}
Return<void> IRadioResponseClientImpl::setSmscAddressResponse(const V1_0::RadioResponseInfo& info) {
    return Void();
}
Return<void> IRadioResponseClientImpl::getSmscAddressResponse(
                                const V1_0::RadioResponseInfo& info, const hidl_string& smsc) {
    return Void();
}
Return<void> IRadioResponseClientImpl::deleteSmsOnSimResponse(const V1_0::RadioResponseInfo& info) {
    return Void();
}

Return<void> IRadioResponseClientImpl::deleteSmsOnRuimResponse(const V1_0::RadioResponseInfo& info) {
    return Void();
}

Return<void> IRadioResponseClientImpl::getVoiceRegistrationStateResponse(
        const V1_0::RadioResponseInfo& info, const V1_0::VoiceRegStateResult& voiceRegResponse) {
    // use malloc, since framework will use free() to release
    auto data = (RIL_VoiceRegistrationStateResponse*)malloc(sizeof(RIL_VoiceRegistrationStateResponse));
    if (data != nullptr) {
        convertHidlVoiceRegistrationStateResponseToRil(voiceRegResponse, *data);

        int slot = acquire_expectation_table();
        enqueue_ril_expect(
                data,                                 // payload
                sizeof(*data),
                RIL_REQUEST_VOICE_REGISTRATION_STATE, // msg_id
                info.serial,                          // serial_id
                (RIL_Errno)info.error,                // RIL error
                ril_utf_ril_response);                // type

        release_expectation_table(slot);
    }
    return Void();
}

Return<void> IRadioResponseClientImpl::getVoiceRegistrationStateResponse_1_2(
        const V1_0::RadioResponseInfo& info, const V1_2::VoiceRegStateResult& voiceRegResponse) {
    // use malloc, since framework will use free() to release
    auto data = (RIL_VoiceRegistrationStateResponse*)malloc(sizeof(RIL_VoiceRegistrationStateResponse));
    if (data != nullptr) {
        convertHidlVoiceRegistrationStateResponseToRil(voiceRegResponse, *data);

        int slot = acquire_expectation_table();
        enqueue_ril_expect(
                data,                                 // payload
                sizeof(*data),
                RIL_REQUEST_VOICE_REGISTRATION_STATE, // msg_id
                info.serial,                          // serial_id
                (RIL_Errno)info.error,                // RIL error
                ril_utf_ril_response);                // type

        release_expectation_table(slot);
    }
    return Void();
}

Return<void> IRadioResponseClientImpl::getVoiceRegistrationStateResponse_1_5(
        const V1_0::RadioResponseInfo& info, const V1_5::RegStateResult& voiceRegResponse) {
    // use malloc, since framework will use free() to release
    auto data = (RIL_VoiceRegistrationStateResponse*)malloc(sizeof(RIL_VoiceRegistrationStateResponse));
    if (data != nullptr) {
        convertHidlVoiceRegistrationStateResponseToRil(voiceRegResponse, *data);

        int slot = acquire_expectation_table();
        enqueue_ril_expect(
                data,                                 // payload
                sizeof(*data),
                RIL_REQUEST_VOICE_REGISTRATION_STATE, // msg_id
                info.serial,                          // serial_id
                (RIL_Errno)info.error,                // RIL error
                ril_utf_ril_response);                // type

        release_expectation_table(slot);
    }
    return Void();
}

Return<void> IRadioResponseClientImpl::getDataRegistrationStateResponse(
        const V1_0::RadioResponseInfo& info, const V1_0::DataRegStateResult& dataRegResponse) {
    // use malloc, since framework will use free() to release
    auto data = (RIL_DataRegistrationStateResponse*)malloc(sizeof(RIL_DataRegistrationStateResponse));
    if (data != nullptr) {
        convertHidlDataRegistrationStateResponseToRil(dataRegResponse, *data);

        int slot = acquire_expectation_table();
        enqueue_ril_expect(
                data,                                 // payload
                sizeof(*data),
                RIL_REQUEST_DATA_REGISTRATION_STATE,  // msg_id
                info.serial,                          // serial_id
                (RIL_Errno)info.error,                // RIL error
                ril_utf_ril_response);                // type

        release_expectation_table(slot);
    }
    return Void();
}

Return<void> IRadioResponseClientImpl::getDataRegistrationStateResponse_1_2(
        const V1_0::RadioResponseInfo& info, const V1_2::DataRegStateResult& dataRegResponse) {
    // use malloc, since framework will use free() to release
    auto data = (RIL_DataRegistrationStateResponse*)malloc(sizeof(RIL_DataRegistrationStateResponse));
    if (data != nullptr) {
        convertHidlDataRegistrationStateResponseToRil(dataRegResponse, *data);

        int slot = acquire_expectation_table();
        enqueue_ril_expect(
                data,                                 // payload
                sizeof(*data),
                RIL_REQUEST_DATA_REGISTRATION_STATE,  // msg_id
                info.serial,                          // serial_id
                (RIL_Errno)info.error,                // RIL error
                ril_utf_ril_response);                // type

        release_expectation_table(slot);
    }
    return Void();
}

Return<void> IRadioResponseClientImpl::getDataRegistrationStateResponse_1_4(
        const V1_0::RadioResponseInfo& info, const V1_4::DataRegStateResult& dataRegResponse) {
    // use malloc, since framework will use free() to release
    auto data = (RIL_DataRegistrationStateResponse*)malloc(sizeof(RIL_DataRegistrationStateResponse));
    if (data != nullptr) {
        convertHidlDataRegistrationStateResponseToRil_1_4(dataRegResponse, *data);

        int slot = acquire_expectation_table();
        enqueue_ril_expect(
                data,                                 // payload
                sizeof(*data),
                RIL_REQUEST_DATA_REGISTRATION_STATE,  // msg_id
                info.serial,                          // serial_id
                (RIL_Errno)info.error,                // RIL error
                ril_utf_ril_response);                // type

        release_expectation_table(slot);
    }
    return Void();
}

Return<void> IRadioResponseClientImpl::getDataRegistrationStateResponse_1_5(
    const V1_0::RadioResponseInfo& info, const V1_5::RegStateResult& dataRegResponse) {
    // use malloc, since framework will use free() to release
    auto data = (RIL_DataRegistrationStateResponse*)malloc(sizeof(RIL_DataRegistrationStateResponse));
    if (data != nullptr) {
        convertHidlDataRegistrationStateResponseToRil_1_5(dataRegResponse, *data);

        int slot = acquire_expectation_table();
        enqueue_ril_expect(
                data,                                 // payload
                sizeof(*data),
                RIL_REQUEST_DATA_REGISTRATION_STATE,  // msg_id
                info.serial,                          // serial_id
                (RIL_Errno)info.error,                // RIL error
                ril_utf_ril_response);                // type

        release_expectation_table(slot);
    }
    return Void();
}

Return<void> IRadioResponseClientImpl::getOperatorResponse(
        const V1_0::RadioResponseInfo& info, const hidl_string& longName,
        const hidl_string& shortName, const hidl_string& numeric) {
    auto data = convertHidlOperatorReponseToRil(longName, shortName, numeric);
    if (data != nullptr) {
        int slot = acquire_expectation_table();
        enqueue_ril_expect(
                data,                                 // payload
                sizeof(*data),
                RIL_REQUEST_OPERATOR,                 // msg_id
                info.serial,                          // serial_id
                (RIL_Errno)info.error,                // RIL error
                ril_utf_ril_response);                // type

        release_expectation_table(slot);
    }
    return Void();
}

Return<void> IRadioResponseClientImpl::getAvailableNetworksResponse(
        const V1_0::RadioResponseInfo& info, const hidl_vec<V1_0::OperatorInfo>& networkInfos) {
    auto data = convertHidlLegacyNetworkscanResponseToRil(networkInfos);
    if (data != nullptr) {
        int slot = acquire_expectation_table();
        enqueue_ril_expect(
                data,                                 // payload
                sizeof(*data),
                RIL_REQUEST_QUERY_AVAILABLE_NETWORKS, // msg_id
                info.serial,                          // serial_id
                (RIL_Errno)info.error,                // RIL error
                ril_utf_ril_response);                // type

        release_expectation_table(slot);
    }
    return Void();
}

Return<void> IRadioResponseClientImpl::getNetworkSelectionModeResponse(
        const V1_0::RadioResponseInfo& info, bool manual) {
    int* data = nullptr;
    int  data_len = 0;

    if (info.error == V1_0::RadioError::NONE) {
        data = (int*)malloc(sizeof(int));
        if (data == nullptr) return Void();
        *data = manual ? 1 : 0;
        data_len = sizeof(*data);
    }

    int slot = acquire_expectation_table();
    enqueue_ril_expect(
                data,                           // payload
                data_len,
                RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE,
                info.serial,                    // serial_id
                (RIL_Errno)info.error,          // RIL error
                ril_utf_ril_response);          // type
    release_expectation_table(slot);
    return Void();
}

Return<void> IRadioResponseClientImpl::setBandModeResponse(
        const V1_0::RadioResponseInfo& info) {

    int slot = acquire_expectation_table();
    enqueue_ril_expect(
                nullptr,                        // payload
                0,
                RIL_REQUEST_SET_BAND_MODE,
                info.serial,                    // serial_id
                (RIL_Errno)info.error,          // RIL error
                ril_utf_ril_response);          // type
    release_expectation_table(slot);
    return Void();
}

Return<void> IRadioResponseClientImpl::getAvailableBandModesResponse(
        const V1_0::RadioResponseInfo& info, const hidl_vec<V1_0::RadioBandMode>& bandModes) {

    int num_bands = bandModes.size();
    size_t payload_len = 0;
    int *payload = nullptr;
    if (num_bands > 0) {
        payload = (int*)malloc(sizeof(int) * (num_bands + 1));
        payload_len = sizeof(int) * (num_bands + 1);
        memset(payload, 0, payload_len);
        payload[0] = num_bands;

        for(int i=0; i < num_bands; ++i) {
            payload[i+1] = static_cast<int>(bandModes[i]);
        }
    }

    int slot = acquire_expectation_table();
    enqueue_ril_expect(
                payload,                           // payload
                payload_len,
                RIL_REQUEST_QUERY_AVAILABLE_BAND_MODE,
                info.serial,                    // serial_id
                (RIL_Errno)info.error,          // RIL error
                ril_utf_ril_response);          // type
    release_expectation_table(slot);
    return Void();
}

Return<void> IRadioResponseClientImpl::setPreferredNetworkTypeResponse(const V1_0::RadioResponseInfo& info) {
    int slot = acquire_expectation_table();
    enqueue_ril_expect(
                nullptr,                        // payload
                0,
                RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE,    // msg_id
                info.serial,                    // serial_id
                (RIL_Errno)info.error,          // RIL error
                ril_utf_ril_response);          // type
    release_expectation_table(slot);
    return Void();
}

Return<void> IRadioResponseClientImpl::getPreferredNetworkTypeResponse(const V1_0::RadioResponseInfo& info,
        V1_0::PreferredNetworkType nwType) {
    int* data = nullptr;
    int  data_len = 0;

    // No matter failure or success, the HIDL interface will give the nw_type.
    // But the legacy interface UTF is using expects null payload in case of failure
    if (info.error == V1_0::RadioError::NONE) {
        data = (int*)malloc(sizeof(int));
        if (data == nullptr) return Void();
        *data = (int)nwType;
        data_len = sizeof(*data);
    }

    int slot = acquire_expectation_table();
    enqueue_ril_expect(
                data,                           // payload
                data_len,
                RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE,    // msg_id
                info.serial,                    // serial_id
                (RIL_Errno)info.error,          // RIL error
                ril_utf_ril_response);          // type
    release_expectation_table(slot);
    return Void();
}

Return<void> IRadioResponseClientImpl::getSignalStrengthResponse(const V1_0::RadioResponseInfo& info,
        const V1_0::SignalStrength& sigStrength) {
  // use malloc, since framework will use free() to release
  // auto data = (RIL_SignalStrength_v10*)malloc(sizeof(RIL_SignalStrength_v10));
  // if (data != nullptr) {
  //     convertHidlSignalStrengthToRil(sigStrength, *data);

  //     int slot = acquire_expectation_table();
  //     enqueue_ril_expect(
  //                 data,                           // payload
  //                 sizeof(*data),
  //                 RIL_REQUEST_SIGNAL_STRENGTH,
  //                 info.serial,                    // serial_id
  //                 (RIL_Errno)info.error,          // RIL error
  //                 ril_utf_ril_response);          // type
  //     release_expectation_table(slot);
  // }
  return Void();
}

Return<void> IRadioResponseClientImpl::getSignalStrengthResponse_1_2(const V1_0::RadioResponseInfo& info,
        const V1_2::SignalStrength& sigStrength) {
    // use malloc, since framework will use free() to release
    auto data = (RIL_SignalStrength_v10*)malloc(sizeof(RIL_SignalStrength_v10));
    if (data != nullptr) {
        convertHidlSignalStrengthToRil(sigStrength, *data);

        int slot = acquire_expectation_table();
        enqueue_ril_expect(
                    data,                           // payload
                    sizeof(*data),
                    RIL_REQUEST_SIGNAL_STRENGTH,
                    info.serial,                    // serial_id
                    (RIL_Errno)info.error,          // RIL error
                    ril_utf_ril_response);          // type
        release_expectation_table(slot);
    }
    return Void();
}

Return<void> IRadioResponseClientImpl::getSignalStrengthResponse_1_4(const V1_0::RadioResponseInfo& info,
        const V1_4::SignalStrength& sigStrength) {
    // use malloc, since framework will use free() to release
    auto data = (RIL_SignalStrength_v10*)malloc(sizeof(RIL_SignalStrength_v10));
    if (data != nullptr) {
        convertHidlSignalStrengthToRil(sigStrength, *data);

        int slot = acquire_expectation_table();
        enqueue_ril_expect(
                    data,                           // payload
                    sizeof(*data),
                    RIL_REQUEST_SIGNAL_STRENGTH,
                    info.serial,                    // serial_id
                    (RIL_Errno)info.error,          // RIL error
                    ril_utf_ril_response);          // type
        release_expectation_table(slot);
    }
    return Void();
}

Return<void> IRadioResponseClientImpl::getVoiceRadioTechnologyResponse(const V1_0::RadioResponseInfo& info,
        V1_0::RadioTechnology rat) {
  // auto data = (int*)malloc(sizeof(int));
  // if (data != nullptr) {
  //     *data = static_cast<int>(rat);

  //     int slot = acquire_expectation_table();
  //     enqueue_ril_expect(
  //                 data,                           // payload
  //                 sizeof(*data),
  //                 RIL_REQUEST_SIGNAL_STRENGTH,
  //                 info.serial,                    // serial_id
  //                 (RIL_Errno)info.error,          // RIL error
  //                 ril_utf_ril_response);          // type
  //     release_expectation_table(slot);
  // }
  return Void();
}

Return<void> IRadioResponseClientImpl::setCdmaSubscriptionSourceResponse(const V1_0::RadioResponseInfo& info) {
    int slot = acquire_expectation_table();
    enqueue_ril_expect(
                nullptr,                        // payload
                0,
                RIL_REQUEST_CDMA_SET_SUBSCRIPTION_SOURCE, // msg_id
                info.serial,                    // serial_id
                (RIL_Errno)info.error,          // RIL error
                ril_utf_ril_response);          // type
    release_expectation_table(slot);
    return Void();
}

Return<void> IRadioResponseClientImpl::setCdmaRoamingPreferenceResponse(const V1_0::RadioResponseInfo& info) {
    int slot = acquire_expectation_table();
    enqueue_ril_expect(
                nullptr,                        // payload
                0,
                RIL_REQUEST_CDMA_SET_ROAMING_PREFERENCE, // msg_id
                info.serial,                    // serial_id
                (RIL_Errno)info.error,          // RIL error
                ril_utf_ril_response);          // type
    release_expectation_table(slot);
    return Void();
}

Return<void> IRadioResponseClientImpl::getCdmaRoamingPreferenceResponse(const V1_0::RadioResponseInfo& info, V1_0::CdmaRoamingType type) {
    auto data = (ril_request_cdma_query_roaming_preference_resp_t*)malloc(sizeof(ril_request_cdma_query_roaming_preference_resp_t));
    if (data != nullptr) {
        data->roaming_preference = static_cast<ril_cdma_roaming_preference_enum_type>(type);

        int slot = acquire_expectation_table();
        enqueue_ril_expect(
                    data,                           // payload
                    sizeof(*data),
                    RIL_REQUEST_CDMA_QUERY_ROAMING_PREFERENCE,
                    info.serial,                    // serial_id
                    (RIL_Errno)info.error,          // RIL error
                    ril_utf_ril_response);          // type
        release_expectation_table(slot);
    }
    return Void();
}

Return<void> IRadioResponseClientImpl::getCdmaSubscriptionSourceResponse(const V1_0::RadioResponseInfo& info, V1_0::CdmaSubscriptionSource source) {
    auto data = (RIL_CdmaSubscriptionSource*)malloc(sizeof(RIL_CdmaSubscriptionSource));
    if (data != nullptr) {
        *data = static_cast<RIL_CdmaSubscriptionSource>(source);

        int slot = acquire_expectation_table();
        enqueue_ril_expect(
                    data,                           // payload
                    sizeof(*data),
                    RIL_REQUEST_CDMA_GET_SUBSCRIPTION_SOURCE,
                    info.serial,                    // serial_id
                    (RIL_Errno)info.error,          // RIL error
                    ril_utf_ril_response);          // type
        release_expectation_table(slot);
    }
    return Void();
}

Return<void> IRadioResponseClientImpl::requestShutdownResponse(const V1_0::RadioResponseInfo& info) {
    return Void();
}

Return<void> IRadioResponseClientImpl::dialResponse(const V1_0::RadioResponseInfo& info) {
    return Void();
}

Return<void> IRadioResponseClientImpl::hangupConnectionResponse(const V1_0::RadioResponseInfo& info) {
    return Void();
}

Return<void> IRadioResponseClientImpl::acceptCallResponse(const V1_0::RadioResponseInfo& info) {
    return Void();
}
Return<void> IRadioResponseClientImpl::hangupWaitingOrBackgroundResponse(const V1_0::RadioResponseInfo& info) {
    return Void();
}
Return<void> IRadioResponseClientImpl::hangupForegroundResumeBackgroundResponse(const V1_0::RadioResponseInfo& info) {
    return Void();
}
Return<void> IRadioResponseClientImpl::rejectCallResponse(const V1_0::RadioResponseInfo& info) {
    return Void();
}
Return<void> IRadioResponseClientImpl::switchWaitingOrHoldingAndActiveResponse(const V1_0::RadioResponseInfo& info) {
    return Void();
}
Return<void> IRadioResponseClientImpl::conferenceResponse(const V1_0::RadioResponseInfo& info) {
    return Void();
}
Return<void> IRadioResponseClientImpl::sendDtmfResponse(const V1_0::RadioResponseInfo& info) {
    return Void();
}
Return<void> IRadioResponseClientImpl::startDtmfResponse(const V1_0::RadioResponseInfo& info) {
    return Void();
}

Return<void> IRadioResponseClientImpl::stopDtmfResponse(const V1_0::RadioResponseInfo& info) {
    return Void();
}

Return<void> IRadioResponseClientImpl::getClirResponse(const V1_0::RadioResponseInfo& info, int32_t n, int32_t m) {
    return Void();
}

Return<void> IRadioResponseClientImpl::setClirResponse(const V1_0::RadioResponseInfo& info) {
    return Void();
}

Return<void> IRadioResponseClientImpl::getMuteResponse(const V1_0::RadioResponseInfo& info, bool enable) {
    return Void();
}

Return<void> IRadioResponseClientImpl::setMuteResponse(const V1_0::RadioResponseInfo& info) {
    return Void();
}

Return<void> IRadioResponseClientImpl::setCellInfoListRateResponse(const V1_0::RadioResponseInfo& info) {
    int slot = acquire_expectation_table();
    enqueue_ril_expect(
                nullptr,                        // payload
                0,
                RIL_REQUEST_SET_UNSOL_CELL_INFO_LIST_RATE, // msg_id
                info.serial,                    // serial_id
                (RIL_Errno)info.error,          // RIL error
                ril_utf_ril_response);          // type
    release_expectation_table(slot);
    return Void();
}

Return<void> IRadioResponseClientImpl::getClipResponse(const V1_0::RadioResponseInfo& info, V1_0::ClipStatus status) {
    return Void();
}

Return<void> IRadioResponseClientImpl::getCellInfoListResponse(
    const V1_0::RadioResponseInfo& info, const hidl_vec<V1_0::CellInfo>& cellInfo) {
  size_t payload_len = cellInfo.size() * sizeof(RIL_CellInfo_v12);
  auto payload = (RIL_CellInfo_v12*)malloc(payload_len);
  if (payload != nullptr) {
    memset(payload, 0x0, payload_len);
    // TODO copy payload
    int slot = acquire_expectation_table();
    enqueue_ril_expect(payload,  // payload
                       sizeof(RIL_CellInfo_v12),
                       RIL_REQUEST_GET_CELL_INFO_LIST,  // msg_id
                       info.serial,                     // serial_id
                       (RIL_Errno)info.error,           // RIL error
                       ril_utf_ril_response);           // type
    release_expectation_table(slot);
  }
  return Void();
}

Return<void> IRadioResponseClientImpl::getCellInfoListResponse_1_2(
    const V1_0::RadioResponseInfo& info, const hidl_vec<V1_2::CellInfo>& cellInfo) {
  size_t payload_len = cellInfo.size() * sizeof(RIL_CellInfo_v12);
  auto payload = (RIL_CellInfo_v12*)malloc(payload_len);
  if (payload != nullptr) {
    memset(payload, 0x0, payload_len);
    // TODO copy payload
    int slot = acquire_expectation_table();
    enqueue_ril_expect(payload,  // payload
                       sizeof(RIL_CellInfo_v12),
                       RIL_REQUEST_GET_CELL_INFO_LIST,  // msg_id
                       info.serial,                     // serial_id
                       (RIL_Errno)info.error,           // RIL error
                       ril_utf_ril_response);           // type
    release_expectation_table(slot);
  }
  return Void();
}

Return<void> IRadioResponseClientImpl::getCellInfoListResponse_1_4(
    const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
    const ::android::hardware::hidl_vec<::android::hardware::radio::V1_4::CellInfo>& cellInfo) {
  size_t payload_len = cellInfo.size() * sizeof(RIL_CellInfo_v12);
  auto payload = (RIL_CellInfo_v12*)malloc(payload_len);
  if (payload != nullptr) {
    memset(payload, 0x0, payload_len);
//    for (size_t i = 0; i < cellInfo.size(); i++) {
//      convertRILCellInfoToUtf(payload[i], cellInfo[i]);
//    }
  }
  int slot = acquire_expectation_table();
  enqueue_ril_expect(payload,  // payload
                     payload_len,
                     RIL_REQUEST_GET_CELL_INFO_LIST,  // msg_id
                     info.serial,                     // serial_id
                     (RIL_Errno)info.error,           // RIL error
                     ril_utf_ril_response);           // type
  release_expectation_table(slot);
  return Void();
}

Return<void> IRadioResponseClientImpl::getCellInfoListResponse_1_5(
    const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
    const ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::CellInfo>& cellInfo) {
  size_t payload_len = cellInfo.size() * sizeof(RIL_CellInfo_v12);
  auto payload = (RIL_CellInfo_v12*)malloc(payload_len);
  if (payload != nullptr) {
    memset(payload, 0x0, payload_len);
    for (size_t i = 0; i < cellInfo.size(); i++) {
      convertRILCellInfoToUtf(payload[i], cellInfo[i]);
    }
  }
  int slot = acquire_expectation_table();
  enqueue_ril_expect(payload,  // payload
                     payload_len,
                     RIL_REQUEST_GET_CELL_INFO_LIST,  // msg_id
                     info.serial,                     // serial_id
                     (RIL_Errno)info.error,           // RIL error
                     ril_utf_ril_response);           // type
  release_expectation_table(slot);
  return Void();
}

Return<void> IRadioResponseClientImpl::separateConnectionResponse(const V1_0::RadioResponseInfo& info) {
    return Void();
}

Return<void> IRadioResponseClientImpl::explicitCallTransferResponse(const V1_0::RadioResponseInfo& info) {
    return Void();
}

Return<void> IRadioResponseClientImpl::getLastCallFailCauseResponse(const V1_0::RadioResponseInfo& info, const V1_0::LastCallFailCauseInfo& failCauseinfo) {
    return Void();
}

Return<void> IRadioResponseClientImpl::getCurrentCallsResponse(const V1_0::RadioResponseInfo& info, const hidl_vec<V1_0::Call>& calls) {
    return Void();
}

Return<void> IRadioResponseClientImpl::getCurrentCallsResponse_1_2(const V1_0::RadioResponseInfo& info, const hidl_vec<V1_2::Call>& calls) {
    return Void();
}

Return<void> IRadioResponseClientImpl::getCurrentCallsResponse_1_6(const V1_6::RadioResponseInfo& info, const hidl_vec<V1_6::Call>& calls) {
    return Void();
}

Return<void> IRadioResponseClientImpl::sendUssdResponse(const V1_0::RadioResponseInfo& info) {
    int slot = acquire_expectation_table();
    enqueue_ril_expect(
                nullptr,                        // payload
                0,
                RIL_REQUEST_SEND_USSD,          // msg_id
                info.serial,                    // serial_id
                (RIL_Errno)info.error,          // RIL error
                ril_utf_ril_response);          // type
    release_expectation_table(slot);
    return Void();
}

Return<void> IRadioResponseClientImpl::cancelPendingUssdResponse(const V1_0::RadioResponseInfo& info) {
    int slot = acquire_expectation_table();
    enqueue_ril_expect(
                nullptr,                        // payload
                0,
                RIL_REQUEST_CANCEL_USSD,        // msg_id
                info.serial,                    // serial_id
                (RIL_Errno)info.error,          // RIL error
                ril_utf_ril_response);          // type
    release_expectation_table(slot);
    return Void();
}

Return<void> IRadioResponseClientImpl::getCallForwardStatusResponse(const V1_0::RadioResponseInfo& info, const hidl_vec<V1_0::CallForwardInfo>& callForwardInfos) {
  return Void();
}

Return<void> IRadioResponseClientImpl::setCallForwardResponse(const V1_0::RadioResponseInfo& info) {
    return Void();
}

Return<void> IRadioResponseClientImpl::getCallWaitingResponse(const V1_0::RadioResponseInfo& info, bool enable, int32_t serviceClass) {
    return Void();
}

Return<void> IRadioResponseClientImpl::setCallWaitingResponse(const V1_0::RadioResponseInfo& info) {
    return Void();
}
Return<void> IRadioResponseClientImpl::getFacilityLockForAppResponse(const V1_0::RadioResponseInfo& info, int32_t response) {
    auto payload = (int *) malloc(sizeof(int));
    if (payload != nullptr) {
        *payload = response;
        int slot = acquire_expectation_table();
        enqueue_ril_expect(
                payload,                            // payload
                sizeof(int),
                RIL_REQUEST_QUERY_FACILITY_LOCK,    // msg_id
                info.serial,                        // serial_id
                (RIL_Errno)info.error,              // RIL error
                ril_utf_ril_response);              // type
        release_expectation_table(slot);
    }
    return Void();
}
Return<void> IRadioResponseClientImpl::setFacilityLockForAppResponse(const V1_0::RadioResponseInfo& info, int32_t retry) {
    (void)retry;
    int slot = acquire_expectation_table();
    enqueue_ril_expect(
            nullptr,                            // payload
            0,
            RIL_REQUEST_QUERY_FACILITY_LOCK,    // msg_id
            info.serial,                        // serial_id
            (RIL_Errno)info.error,              // RIL error
            ril_utf_ril_response);              // type
    release_expectation_table(slot);
    return Void();
}
Return<void> IRadioResponseClientImpl::setBarringPasswordResponse(const V1_0::RadioResponseInfo& info) {
    int slot = acquire_expectation_table();
    enqueue_ril_expect(
            nullptr,                            // payload
            0,
            RIL_REQUEST_CHANGE_BARRING_PASSWORD,// msg_id
            info.serial,                        // serial_id
            (RIL_Errno)info.error,              // RIL error
            ril_utf_ril_response);              // type
    release_expectation_table(slot);
    return Void();
}
Return<void> IRadioResponseClientImpl::setSuppServiceNotificationsResponse(const V1_0::RadioResponseInfo& info) {
    int slot = acquire_expectation_table();
    enqueue_ril_expect(
            nullptr,                            // payload
            0,
            RIL_REQUEST_SET_SUPP_SVC_NOTIFICATION, // msg_id
            info.serial,                        // serial_id
            (RIL_Errno)info.error,              // RIL error
            ril_utf_ril_response);              // type
    release_expectation_table(slot);
    return Void();
}
Return<void> IRadioResponseClientImpl::writeSmsToSimResponse(const V1_0::RadioResponseInfo& info, int32_t index) {
    return Void();
}
Return<void> IRadioResponseClientImpl::setTTYModeResponse(const V1_0::RadioResponseInfo& info) {
    return Void();
}
Return<void> IRadioResponseClientImpl::getTTYModeResponse(const V1_0::RadioResponseInfo& info, V1_0::TtyMode mode) {
    return Void();
}
Return<void> IRadioResponseClientImpl::setPreferredVoicePrivacyResponse(const V1_0::RadioResponseInfo& info) {
    return Void();
}
Return<void> IRadioResponseClientImpl::getPreferredVoicePrivacyResponse(const V1_0::RadioResponseInfo& info, bool enable) {
    return Void();
}
Return<void> IRadioResponseClientImpl::sendCDMAFeatureCodeResponse(const V1_0::RadioResponseInfo& info) {
    return Void();
}
Return<void> IRadioResponseClientImpl::sendBurstDtmfResponse(const V1_0::RadioResponseInfo& info) {
    return Void();
}
Return<void> IRadioResponseClientImpl::acknowledgeLastIncomingCdmaSmsResponse(const V1_0::RadioResponseInfo& info) {
    return Void();
}
Return<void> IRadioResponseClientImpl::getGsmBroadcastConfigResponse(
      const V1_0::RadioResponseInfo& info, const hidl_vec<V1_0::GsmBroadcastSmsConfigInfo>& configs) {
    return Void();
}

Return<void> IRadioResponseClientImpl::setRadioPowerResponse(const V1_0::RadioResponseInfo& info) {
    return Void();
}

Return<void> IRadioResponseClientImpl::setRadioPowerResponse_1_5(const V1_0::RadioResponseInfo& info) {
    return Void();
}
Return<void> IRadioResponseClientImpl::getBasebandVersionResponse(const V1_0::RadioResponseInfo& info,
        const hidl_string& version) {
    return Void();
}
Return<void> IRadioResponseClientImpl::setGsmBroadcastActivationResponse(const V1_0::RadioResponseInfo& info) {
    return Void();
}
Return<void> IRadioResponseClientImpl::getCdmaBroadcastConfigResponse(
    const V1_0::RadioResponseInfo& info, const hidl_vec<V1_0::CdmaBroadcastSmsConfigInfo>& configs) {
    return Void();
}
Return<void> IRadioResponseClientImpl::setCdmaBroadcastActivationResponse(
                                                     const V1_0::RadioResponseInfo& info) {
    return Void();
}
Return<void> IRadioResponseClientImpl::writeSmsToRuimResponse(const V1_0::RadioResponseInfo& info, uint32_t index) {
    return Void();
}
Return<void> IRadioResponseClientImpl::getDeviceIdentityResponse(const V1_0::RadioResponseInfo& info,
        const hidl_string& imei, const hidl_string& imeisv, const hidl_string& esn, const hidl_string& meid) {
    return Void();
}
Return<void> IRadioResponseClientImpl::setGsmBroadcastConfigResponse(
                                                        const V1_0::RadioResponseInfo& info) {
    return Void();
}
Return<void> IRadioResponseClientImpl::setCdmaBroadcastConfigResponse(
                                                        const V1_0::RadioResponseInfo& info) {
    return Void();
}
Return<void> IRadioResponseClientImpl::reportSmsMemoryStatusResponse(
                                                        const V1_0::RadioResponseInfo& info) {
    return Void();
}
Return<void> IRadioResponseClientImpl::getModemActivityInfoResponse(const V1_0::RadioResponseInfo& info,
        const V1_0::ActivityStatsInfo& activityInfo) {
    return Void();
}

Return<void> IRadioResponseClientImpl::iccIOForAppResponse(const V1_0::RadioResponseInfo& info,
                 const V1_0::IccIoResult& iccIo) {
  auto payload = (RIL_SIM_IO_Response*) malloc(sizeof(RIL_SIM_IO_Response));
    if (payload != nullptr) {
        memset(payload, 0, sizeof(RIL_SIM_IO_Response));
        convertSimIoRespHidltoRilPayload(iccIo, *payload);

        int slot = acquire_expectation_table();

        enqueue_ril_expect(
                payload,                 // payload
                sizeof(RIL_SIM_IO_Response),
                RIL_REQUEST_SIM_IO,      // msg_id
                info.serial,             // serial_id
                (RIL_Errno)info.error,   // RIL error
                ril_utf_ril_response);   // type

        release_expectation_table(slot);
    }

  return Void();
}

Return<void> IRadioResponseClientImpl::getIccCardStatusResponse_1_2
                 (const V1_0::RadioResponseInfo& info, const V1_2::CardStatus& cardStatus) {

  auto payload = (RIL_CardStatus_v6*) malloc(sizeof(RIL_CardStatus_v6));
    if (payload != nullptr) {
        memset(payload, 0, sizeof(RIL_CardStatus_v6));
        convertGetIccCardStatusRespHidltoRilPayload(cardStatus, *payload);

        int slot = acquire_expectation_table();

        enqueue_ril_expect(
                payload,                      // payload
                sizeof(RIL_CardStatus_v6),
                RIL_REQUEST_GET_SIM_STATUS,   // msg_id
                info.serial,                  // serial_id
                (RIL_Errno)info.error,        // RIL error
                ril_utf_ril_response);        // type

        release_expectation_table(slot);
    }

  return Void();
}

Return<void> IRadioResponseClientImpl::getIccCardStatusResponse_1_5
                 (const V1_0::RadioResponseInfo& info, const V1_5::CardStatus& cardStatus) {

  auto payload = (RIL_CardStatus_v6*) malloc(sizeof(RIL_CardStatus_v6));
    if (payload != nullptr) {
        memset(payload, 0, sizeof(RIL_CardStatus_v6));
        convertGetIccCardStatusRespHidltoRilPayload_1_5(cardStatus, *payload);

        int slot = acquire_expectation_table();

        enqueue_ril_expect(
                payload,                      // payload
                sizeof(RIL_CardStatus_v6),
                RIL_REQUEST_GET_SIM_STATUS,   // msg_id
                info.serial,                  // serial_id
                (RIL_Errno)info.error,        // RIL error
                ril_utf_ril_response);        // type

        release_expectation_table(slot);
    }

  return Void();
}

Return<void> IRadioResponseClientImpl::enableModemResponse
                 (const V1_0::RadioResponseInfo& info) {
    return Void();
}

Return<void> IRadioResponseClientImpl::getModemStackStatusResponse
                 (const V1_0::RadioResponseInfo& info, bool isEnabled) {
    return Void();
}

Return<void> IRadioResponseClientImpl::startNetworkScanResponse_1_5(const V1_0::RadioResponseInfo& info) {
    int slot = acquire_expectation_table();
    enqueue_ril_expect(
            nullptr,                        // payload
            0,
            RIL_REQUEST_START_NETWORK_SCAN, // msg_id
            info.serial,                    // serial_id
            (RIL_Errno)info.error,          // RIL error
            ril_utf_ril_response);          // type
    release_expectation_table(slot);
    return Void();
}

Return<void> IRadioResponseClientImpl::stopNetworkScanResponse(const V1_0::RadioResponseInfo& info) {
    int slot = acquire_expectation_table();
    enqueue_ril_expect(
            nullptr,                        // payload
            0,
            RIL_REQUEST_STOP_NETWORK_SCAN,  // msg_id
            info.serial,                    // serial_id
            (RIL_Errno)info.error,          // RIL error
            ril_utf_ril_response);          // type
    release_expectation_table(slot);
    return Void();
}
Return<void> IRadioResponseClientImpl::getNeighboringCidsResponse(const V1_0::RadioResponseInfo& info, const hidl_vec<V1_0::NeighboringCell>& cells) {
    int slot = acquire_expectation_table();
    int num_cells = cells.size();
    RIL_NeighboringCell **payload = nullptr;
    size_t payload_len = 0;
    if(num_cells > 0) {
        payload = (RIL_NeighboringCell **)malloc(sizeof(RIL_NeighboringCell*) * num_cells);
        payload_len = sizeof(RIL_NeighboringCell*)*num_cells;

        for(int i=0; i<num_cells; i++) {
            payload[i] = (RIL_NeighboringCell*)malloc(sizeof(RIL_NeighboringCell));
            memset(payload[i], 0, sizeof(RIL_NeighboringCell));
            payload[i]->cid = strdup(cells[i].cid.c_str());
            payload[i]->rssi = (int)(cells[i].rssi);
        }
    }
    enqueue_ril_expect(
            payload,                             // payload
            payload_len,
            RIL_REQUEST_GET_NEIGHBORING_CELL_IDS, // msg_id
            info.serial,                         // serial_id
            (RIL_Errno)info.error,               // RIL error
            ril_utf_ril_response);               // type
    release_expectation_table(slot);
    return Void();
}

Return<void> IRadioResponseClientImpl::setSystemSelectionChannelsResponse_1_5(const V1_0::RadioResponseInfo& info) {
    int slot = acquire_expectation_table();
    enqueue_ril_expect(
            nullptr,                        // payload
            0,
            RIL_REQUEST_SET_SYSTEM_SELECTION_CHANNELS, // msg_id
            info.serial,                    // serial_id
            (RIL_Errno)info.error,          // RIL error
            ril_utf_ril_response);          // type
    release_expectation_table(slot);
    return Void();
}

Return<void> IRadioResponseClientImpl::getSystemSelectionChannelsResponse(const V1_6::RadioResponseInfo& info,
        const hidl_vec<V1_5::RadioAccessSpecifier>& specifiers) {
    RIL_Errno error = RIL_E_SUCCESS;
    int slot = acquire_expectation_table();
    auto payload = (RIL_SysSelChannels*) malloc(sizeof(RIL_SysSelChannels));
    if (payload != nullptr) {
        memset(payload, 0, sizeof(RIL_SysSelChannels));
        payload->specifiers_length = specifiers.size();
        for (size_t i = 0; i < specifiers.size(); ++i) {
            error = android::hardware::radio::utils::convertHidlRasToRilRas(payload->specifiers[i], specifiers[i]);
            if (error != RIL_E_SUCCESS) {
                QCRIL_LOG_ERROR("failed to convert hidl fields to ril fields - %d", error);
            }
        }
        enqueue_ril_expect(
                payload,                        // payload
                sizeof(RIL_SysSelChannels),
                RIL_REQUEST_GET_SYSTEM_SELECTION_CHANNELS, // msg_id
                info.serial,                    // serial_id
                (RIL_Errno)info.error,          // RIL error
                ril_utf_ril_response);          // type
        release_expectation_table(slot);
    }
    return Void();
}
