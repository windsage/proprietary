/******************************************************************************
  @file    ril_utf_i_ims_aidl_radio_sim.cpp
  @brief   RIL UTF IMS AIDL implementation

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#include "ibinder_internal.h"
#include <binder/IBinder.h>
#include <android/binder_status.h>
#include <android/binder_ibinder.h>
#include <android/binder_manager.h>
#include "aidl/vendor/qti/hardware/radio/qtiradio/NrIconType.h"

#include "ril_utf_core.h"
#include "ril_utf_rild_sim.h"

#include "platform/android/IQtiRadioAidlResponseClientImpl.h"
#include "platform/android/IQtiRadioAidlIndicationClientImpl.h"
#include "platform/android/NasAidlUtil.h"
#include "platform/android/ril_qti_radio.h"

#include "platform/android/NasAidlUtil.h"

EnhancedRadioAccessFamily convertEnhancedRafToRil(qtiradioaidl::RadioAccessFamily aidl_raf)
{
  switch (aidl_raf) {
    case qtiradioaidl::RadioAccessFamily::GPRS:
      return ENHANCED_RAF_GPRS;
    case qtiradioaidl::RadioAccessFamily::EDGE:
      return ENHANCED_RAF_EDGE;
    case qtiradioaidl::RadioAccessFamily::UMTS:
      return ENHANCED_RAF_UMTS;
    case qtiradioaidl::RadioAccessFamily::IS95A:
      return ENHANCED_RAF_IS95A;
    case qtiradioaidl::RadioAccessFamily::IS95B:
      return ENHANCED_RAF_IS95B;
    case qtiradioaidl::RadioAccessFamily::ONE_X_RTT:
      return ENHANCED_RAF_1xRTT;
    case qtiradioaidl::RadioAccessFamily::EVDO_0:
      return ENHANCED_RAF_EVDO_0;
    case qtiradioaidl::RadioAccessFamily::EVDO_A:
      return ENHANCED_RAF_EVDO_A;
    case qtiradioaidl::RadioAccessFamily::HSDPA:
      return ENHANCED_RAF_HSDPA;
    case qtiradioaidl::RadioAccessFamily::HSUPA:
      return ENHANCED_RAF_HSUPA;
    case qtiradioaidl::RadioAccessFamily::HSPA:
      return ENHANCED_RAF_HSPA;
    case qtiradioaidl::RadioAccessFamily::EVDO_B:
      return ENHANCED_RAF_EVDO_B;
    case qtiradioaidl::RadioAccessFamily::EHRPD:
      return ENHANCED_RAF_EHRPD;
    case qtiradioaidl::RadioAccessFamily::LTE:
      return ENHANCED_RAF_LTE;
    case qtiradioaidl::RadioAccessFamily::HSPAP:
      return ENHANCED_RAF_HSPAP;
    case qtiradioaidl::RadioAccessFamily::GSM:
      return ENHANCED_RAF_GSM;
    case qtiradioaidl::RadioAccessFamily::TD_SCDMA:
      return ENHANCED_RAF_TD_SCDMA;
    case qtiradioaidl::RadioAccessFamily::LTE_CA:
      return ENHANCED_RAF_LTE_CA;
    case qtiradioaidl::RadioAccessFamily::NR_NSA:
      return ENHANCED_RAF_NR_NSA;
    case qtiradioaidl::RadioAccessFamily::NR_SA:
      return ENHANCED_RAF_NR_SA;
    case qtiradioaidl::RadioAccessFamily::UNKNOWN:
    default:  // Fallthrough
      return ENHANCED_RAF_UNKNOWN;
  }
}

::ndk::ScopedAStatus IQtiRadioAidlResponseClientImpl::onNrIconTypeResponse(
    int32_t in_serial, int32_t in_errorCode, qtiradioaidl::NrIconType in_iconType) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: onNrIconTypeResponse\n");
    int slot = acquire_expectation_table();
    size_t payload_len = sizeof(five_g_icon_type);
    auto payload = (five_g_icon_type *)malloc(payload_len);
    if (payload) {
        switch(in_iconType) {
            case qtiradioaidl::NrIconType::TYPE_NONE:
                *payload = FIVE_G_ICON_TYPE_NONE;
                break;
            case qtiradioaidl::NrIconType::TYPE_5G_BASIC:
                *payload = FIVE_G_ICON_TYPE_BASIC;
                break;
            case qtiradioaidl::NrIconType::TYPE_5G_UWB:
                *payload = FIVE_G_ICON_TYPE_UWB;
                break;
            default:
                *payload = FIVE_G_ICON_TYPE_INVALID;
                break;
        }
    }

    enqueue_ril_expect(
            payload,                            // payload
            payload_len,                        // payload_len
            RIL_REQUEST_GET_DATA_NR_ICON_TYPE,  // msg_id
            in_serial,                          // serial_id
            (RIL_Errno)in_errorCode,            // RIL error
            ril_utf_ril_response);              // type

    release_expectation_table(slot);
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IQtiRadioAidlResponseClientImpl::onEnableEndcResponse(int32_t in_serial,
        int32_t in_errorCode, qtiradioaidl::Status in_status) {
  int slot = acquire_expectation_table();
  enqueue_ril_expect(NULL,                     // payload
                     0,                        // payload_len
                     RIL_REQUEST_ENABLE_ENDC,  // msg_id
                     in_serial,                // serial_id
                     (RIL_Errno)in_errorCode,  // RIL error
                     ril_utf_ril_response);    // type

  release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IQtiRadioAidlResponseClientImpl::onEndcStatusResponse(int32_t in_serial,
        int32_t in_errorCode, qtiradioaidl::EndcStatus in_endcStatus) {
  size_t payload_len = sizeof(endc_status);
  endc_status* payload = (endc_status*)malloc(payload_len);
  if (qtiradioaidl::EndcStatus::INVALID == in_endcStatus) {
    // WARNING: This should not be a possibility. Log it and enqueue a disabled response
    RIL_UTF_DEBUG("!!! Recieved Invalid Status (-1) !!! Enqueing disabled instead");
    *payload = ENDC_STATUS_DISABLED;
  } else {
    *payload = (qtiradioaidl::EndcStatus::ENABLED == in_endcStatus) ? ENDC_STATUS_ENABLED
                                                                    : ENDC_STATUS_DISABLED;
  }

  int slot = acquire_expectation_table();
  enqueue_ril_expect(payload,                        // payload
                     payload_len,                    // payload_len
                     RIL_REQUEST_QUERY_ENDC_STATUS,  // msg_id
                     in_serial,                      // serial_id
                     (RIL_Errno)in_errorCode,        // RIL error
                     ril_utf_ril_response);          // type

  release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IQtiRadioAidlResponseClientImpl::setNrConfigResponse(int32_t in_serial,
        int32_t in_errorCode, qtiradioaidl::Status in_status) {
  size_t payload_len = sizeof(uint32_t);
  uint32_t* payload = (uint32_t*)malloc(payload_len);
  *payload = static_cast<uint32_t>(in_status);

  int slot = acquire_expectation_table();
  enqueue_ril_expect(payload,                          // payload
                     payload_len,                      // payload_len
                     RIL_REQUEST_SET_NR_DISABLE_MODE,  // msg_id
                     in_serial,                        // serial_id
                     (RIL_Errno)in_errorCode,          // RIL error
                     ril_utf_ril_response);            // type

  release_expectation_table(slot);

  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IQtiRadioAidlResponseClientImpl::onNrConfigResponse(int32_t in_serial,
        int32_t in_errorCode, qtiradioaidl::NrConfig in_config) {
  size_t payload_len = sizeof(RIL_NR_DISABLE_MODE);
  RIL_NR_DISABLE_MODE* payload = (RIL_NR_DISABLE_MODE*)malloc(payload_len);
  *payload = convert_nr_config_to_ril(in_config);

  int slot = acquire_expectation_table();
  enqueue_ril_expect(payload,                            // payload
                     payload_len,                        // payload_len
                     RIL_REQUEST_QUERY_NR_DISABLE_MODE,  // msg_id
                     in_serial,                          // serial_id
                     (RIL_Errno)in_errorCode,            // RIL error
                     ril_utf_ril_response);              // type

  release_expectation_table(slot);

  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IQtiRadioAidlResponseClientImpl::getQtiRadioCapabilityResponse(
        int32_t in_serial, int32_t in_errorCode, qtiradioaidl::RadioAccessFamily in_raf) {
  size_t payload_len = sizeof(EnhancedRadioAccessFamily);
  EnhancedRadioAccessFamily* payload = (EnhancedRadioAccessFamily*)malloc(payload_len);
  *payload = convertEnhancedRafToRil(in_raf);

  int slot = acquire_expectation_table();
  enqueue_ril_expect(payload,                                    // payload
                     payload_len,                                // payload_len
                     RIL_REQUEST_GET_ENHANCED_RADIO_CAPABILITY,  // msg_id
                     in_serial,                                  // serial_id
                     (RIL_Errno)in_errorCode,                    // RIL error
                     ril_utf_ril_response);                      // type

  release_expectation_table(slot);

  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IQtiRadioAidlResponseClientImpl::getCallForwardStatusResponse(
        int32_t in_serial, int32_t in_errorCode,
        const std::vector<qtiradioaidl::CallForwardInfo> & in_callForwardInfoList) {
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IQtiRadioAidlResponseClientImpl::getFacilityLockForAppResponse(
        int32_t in_serial, int32_t in_errorCode, int32_t in_response) {
    auto payload = (int *) malloc(sizeof(int));
    if (payload != nullptr) {
        *payload = in_response;
        int slot = acquire_expectation_table();
        enqueue_ril_expect(
                payload,                                      // payload
                sizeof(int),
                RIL_QTI_RADIO_REQUEST_QUERY_FACILITY_LOCK,    // msg_id
                in_serial,                                    // serial_id
                (RIL_Errno)in_errorCode,                      // RIL error
                ril_utf_ril_response);                        // type
        release_expectation_table(slot);
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IQtiRadioAidlResponseClientImpl::getImeiResponse(int32_t in_serial,
        int32_t in_errorCode, const qtiradioaidl::ImeiInfo & in_info) {
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IQtiRadioAidlResponseClientImpl::getDdsSwitchCapabilityResponse(
        int32_t in_serial, int32_t in_errorCode, bool in_support) {
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IQtiRadioAidlResponseClientImpl::sendUserPreferenceForDataDuringVoiceCallResponse(
        int32_t in_serial, int32_t in_errorCode) {
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IQtiRadioAidlResponseClientImpl::setNrUltraWidebandIconConfigResponse(
        int32_t in_serial, int32_t in_errorCode) {
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IQtiRadioAidlResponseClientImpl::getNetworkSelectionModeResponse(
        int32_t in_serial, int32_t in_errorCode,
        const qtiradioaidl::NetworkSelectionMode & in_mode) {
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IQtiRadioAidlResponseClientImpl::setNetworkSelectionModeAutomaticResponse(
        int32_t in_serial, int32_t in_errorCode) {
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IQtiRadioAidlResponseClientImpl::setNetworkSelectionModeManualResponse(
        int32_t in_serial, int32_t in_errorCode) {
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IQtiRadioAidlResponseClientImpl::startNetworkScanResponse(int32_t in_serial,
        int32_t in_errorCode) {
    int slot = acquire_expectation_table();
    enqueue_ril_expect(nullptr,  // payload
                       0,
                       RIL_QTI_RADIO_REQUEST_START_NETWORK_SCAN,  // msg_id
                       in_serial,                                 // serial_id
                       (RIL_Errno)in_errorCode,                   // RIL error
                       ril_utf_ril_response);                     // type

    release_expectation_table(slot);
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IQtiRadioAidlResponseClientImpl::stopNetworkScanResponse(int32_t in_serial,
        int32_t in_errorCode) {
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IQtiRadioAidlResponseClientImpl::getInterfaceVersion(int32_t* _aidl_return) {
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IQtiRadioAidlResponseClientImpl::getInterfaceHash(std::string* _aidl_return) {
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IQtiRadioAidlIndicationClientImpl::onNrIconTypeChange(
        qtiradioaidl::NrIconType in_iconType) {
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IQtiRadioAidlIndicationClientImpl::onNrConfigChange(
        qtiradioaidl::NrConfig in_config) {
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IQtiRadioAidlIndicationClientImpl::onImeiChange(
        const qtiradioaidl::ImeiInfo & in_info) {
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IQtiRadioAidlIndicationClientImpl::onDdsSwitchCapabilityChange() {
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IQtiRadioAidlIndicationClientImpl::onDdsSwitchCriteriaChange(
        bool in_telephonyDdsSwitch) {
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IQtiRadioAidlIndicationClientImpl::onDdsSwitchRecommendation(
        int32_t in_recommendedSlotId) {
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IQtiRadioAidlIndicationClientImpl::onDataDeactivateDelayTime(
        int64_t in_delayTimeMilliSecs) {
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IQtiRadioAidlIndicationClientImpl::onEpdgOverCellularDataSupported(
        bool in_support) {
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IQtiRadioAidlIndicationClientImpl::onMcfgRefresh(
        qtiradioaidl::McfgRefreshState in_refreshState, int32_t in_slotId) {
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IQtiRadioAidlIndicationClientImpl::networkScanResult(
        const qtiradioaidl::QtiNetworkScanResult & in_result) {
    auto payload = (RIL_NetworkScanResult*)malloc(sizeof(RIL_NetworkScanResult));
    size_t payload_len = sizeof(RIL_NetworkScanResult);
    memset(payload, 0, payload_len);
    convertNetworkScanResultToRil(in_result, *payload);
    if (payload != nullptr) {
        int my_expect_slot = acquire_expectation_table();
        enqueue_ril_expect(payload,  // payload
                           payload_len,
                           RIL_QTI_RADIO_UNSOL_NETWORK_SCAN_RESULT,  // msg_id
                           -1,                                       // serial_id
                           RIL_E_SUCCESS,                            // RIL error
                           ril_utf_ril_unsol_response);              // type
        release_expectation_table(my_expect_slot);
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IQtiRadioAidlIndicationClientImpl::getInterfaceVersion(int32_t* _aidl_return) {
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IQtiRadioAidlIndicationClientImpl::getInterfaceHash(
        std::string* _aidl_return) {
    return ndk::ScopedAStatus::ok();
}
