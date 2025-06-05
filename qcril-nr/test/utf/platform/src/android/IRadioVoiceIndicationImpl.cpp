/*===========================================================================
 *    Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *===========================================================================*/

#include "platform/android/IRadioVoiceIndicationImpl.h"
#include "platform/android/VoiceAidlUtil.h"

#include "ril_utf_rild_sim.h"

::ndk::ScopedAStatus aidl::android::hardware::radio::voice::IRadioVoiceIndicationImpl::callRing(
    aidl::android::hardware::radio::RadioIndicationType type,
    bool isGsm,
    const aidl::android::hardware::radio::voice::CdmaSignalInfoRecord& record)
{
  int my_expect_slot = acquire_expectation_table();
  enqueue_ril_expect(nullptr,  // payload
                     0,
                     RIL_UNSOL_CALL_RING,          // msg_id
                     -1,                           // serial_id
                     RIL_E_SUCCESS,                // RIL error
                     ril_utf_ril_unsol_response);  // type
  release_expectation_table(my_expect_slot);

  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidl::android::hardware::radio::voice::IRadioVoiceIndicationImpl::callStateChanged(
    aidl::android::hardware::radio::RadioIndicationType type)
{
  int my_expect_slot = acquire_expectation_table();
  enqueue_ril_expect(nullptr,  // payload
                     0,
                     RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED,  // msg_id
                     -1,                                     // serial_id
                     RIL_E_SUCCESS,                          // RIL error
                     ril_utf_ril_unsol_response);            // type
  release_expectation_table(my_expect_slot);

  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidl::android::hardware::radio::voice::IRadioVoiceIndicationImpl::cdmaCallWaiting(
    aidl::android::hardware::radio::RadioIndicationType type,
    const aidl::android::hardware::radio::voice::CdmaCallWaiting& callWaitingRecord)
{
  RIL_CDMA_CallWaiting_v6* payload =
      (RIL_CDMA_CallWaiting_v6*)malloc(sizeof(RIL_CDMA_CallWaiting_v6));
  if (payload != nullptr) {
    if (callWaitingRecord.number.empty()) {
      payload->number =
          strndup(callWaitingRecord.number.c_str(), callWaitingRecord.number.size() + 1);
    }
    payload->numberPresentation = (int)callWaitingRecord.numberPresentation;
    if (callWaitingRecord.name.empty()) {
      payload->name = strndup(callWaitingRecord.name.c_str(), callWaitingRecord.name.size() + 1);
    }
    payload->signalInfoRecord.isPresent =
        callWaitingRecord.signalInfoRecord.isPresent ? TRUE : FALSE;
    payload->signalInfoRecord.signalType = callWaitingRecord.signalInfoRecord.signalType;
    payload->signalInfoRecord.alertPitch = callWaitingRecord.signalInfoRecord.alertPitch;
    payload->signalInfoRecord.signal = callWaitingRecord.signalInfoRecord.signal;
    payload->number_type = (int)callWaitingRecord.numberType;
    payload->number_plan = (int)callWaitingRecord.numberPlan;

    int my_expect_slot = acquire_expectation_table();
    enqueue_ril_expect(payload,  // payload
                       sizeof(RIL_CDMA_CallWaiting_v6),
                       RIL_UNSOL_CDMA_CALL_WAITING,  // msg_id
                       -1,                           // serial_id
                       RIL_E_SUCCESS,                // RIL error
                       ril_utf_ril_unsol_response);  // type
    release_expectation_table(my_expect_slot);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidl::android::hardware::radio::voice::IRadioVoiceIndicationImpl::cdmaInfoRec(
    aidl::android::hardware::radio::RadioIndicationType type,
    const std::vector<::aidl::android::hardware::radio::voice::CdmaInformationRecord>& records)
{
    RIL_CDMA_InformationRecords* payload =
            (RIL_CDMA_InformationRecords*)malloc(sizeof(RIL_CDMA_InformationRecords));
    if (payload != nullptr) {
        payload->numberOfInfoRecs = records.size();
        // TODO copy payload and add verification in framework
        int my_expect_slot = acquire_expectation_table();
        enqueue_ril_expect(payload,  // payload
                        sizeof(RIL_CDMA_InformationRecords),
                        RIL_UNSOL_CDMA_INFO_REC,      // msg_id
                        -1,                           // serial_id
                        RIL_E_SUCCESS,                // RIL error
                        ril_utf_ril_unsol_response);  // type
        release_expectation_table(my_expect_slot);
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus
aidl::android::hardware::radio::voice::IRadioVoiceIndicationImpl::cdmaOtaProvisionStatus(
    aidl::android::hardware::radio::RadioIndicationType type,
    aidl::android::hardware::radio::voice::CdmaOtaProvisionStatus status)
{
  RIL_CDMA_OTA_ProvisionStatus* payload =
      (RIL_CDMA_OTA_ProvisionStatus*)malloc(sizeof(RIL_CDMA_OTA_ProvisionStatus));
  if (payload != nullptr) {
    *payload = (RIL_CDMA_OTA_ProvisionStatus)status;
    int my_expect_slot = acquire_expectation_table();
    enqueue_ril_expect(payload,  // payload
                       sizeof(RIL_CDMA_OTA_ProvisionStatus),
                       RIL_UNSOL_CDMA_OTA_PROVISION_STATUS,  // msg_id
                       -1,                                   // serial_id
                       RIL_E_SUCCESS,                        // RIL error
                       ril_utf_ril_unsol_response);          // type
    release_expectation_table(my_expect_slot);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus
aidl::android::hardware::radio::voice::IRadioVoiceIndicationImpl::indicateRingbackTone(
    aidl::android::hardware::radio::RadioIndicationType type, bool start)
{
  ril_unsol_ringback_tone_t* payload =
      (ril_unsol_ringback_tone_t*)malloc(sizeof(ril_unsol_ringback_tone_t));
  if (payload != nullptr) {
    payload->start = start;
    int my_expect_slot = acquire_expectation_table();
    enqueue_ril_expect(payload,  // payload
                       sizeof(ril_unsol_ringback_tone_t),
                       RIL_UNSOL_RINGBACK_TONE,      // msg_id
                       -1,                           // serial_id
                       RIL_E_SUCCESS,                // RIL error
                       ril_utf_ril_unsol_response);  // type
    release_expectation_table(my_expect_slot);
  }

  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidl::android::hardware::radio::voice::IRadioVoiceIndicationImpl::
        onSupplementaryServiceIndication(aidl::android::hardware::radio::RadioIndicationType type,
        const aidl::android::hardware::radio::voice::StkCcUnsolSsResult& ss) {
    RIL_StkCcUnsolSsResponse* payload =
            (RIL_StkCcUnsolSsResponse*)malloc(sizeof(RIL_StkCcUnsolSsResponse));
    if (payload != nullptr) {
        // TODO copy payload and fix verification in framework
        int my_expect_slot = acquire_expectation_table();
        enqueue_ril_expect(
                    payload,                                        // payload
                    sizeof(RIL_StkCcUnsolSsResponse),
                    RIL_UNSOL_ON_SS,                                // msg_id
                    -1,                                             // serial_id
                    RIL_E_SUCCESS,                                  // RIL error
                    ril_utf_ril_unsol_response);                    // type
        release_expectation_table(my_expect_slot);
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidl::android::hardware::radio::voice::IRadioVoiceIndicationImpl::srvccStateNotify(
    aidl::android::hardware::radio::RadioIndicationType type,
    aidl::android::hardware::radio::voice::SrvccState state)
{
  RIL_SrvccState* payload = (RIL_SrvccState*)malloc(sizeof(RIL_SrvccState));
  if (payload != nullptr) {
    *payload = (RIL_SrvccState)state;
    int my_expect_slot = acquire_expectation_table();
    enqueue_ril_expect(payload,  // payload
                       sizeof(RIL_SrvccState),
                       RIL_UNSOL_SRVCC_STATE_NOTIFY,  // msg_id
                       -1,                            // serial_id
                       RIL_E_SUCCESS,                 // RIL error
                       ril_utf_ril_unsol_response);   // type
    release_expectation_table(my_expect_slot);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus
aidl::android::hardware::radio::voice::IRadioVoiceIndicationImpl::stkCallControlAlphaNotify(
    aidl::android::hardware::radio::RadioIndicationType type, const std::string& alpha)
{
  auto payload = (ril_unsol_stk_cc_alpha_notify_t*)malloc(sizeof(ril_unsol_stk_cc_alpha_notify_t));
  if (payload != nullptr) {
    size_t payload_len = sizeof(ril_unsol_stk_cc_alpha_notify_t);
    memset(payload, 0, payload_len);
    strlcpy(payload->alpha, alpha.c_str(), sizeof(payload->alpha));
    int my_expect_slot = acquire_expectation_table();
    enqueue_ril_expect(payload,  // payload
                       sizeof(ril_unsol_stk_cc_alpha_notify_t),
                       RIL_UNSOL_STK_CC_ALPHA_NOTIFY,  // msg_id
                       -1,                             // serial_id
                       RIL_E_SUCCESS,                  // RIL error
                       ril_utf_ril_unsol_response);    // type
    release_expectation_table(my_expect_slot);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus
aidl::android::hardware::radio::voice::IRadioVoiceIndicationImpl::currentEmergencyNumberList(
    aidl::android::hardware::radio::RadioIndicationType type,
    const std::vector<aidl::android::hardware::radio::voice::EmergencyNumber>& emergencyNumberList)
{
  auto payload = (ril_unsol_emergency_list_t*)malloc(sizeof(ril_unsol_emergency_list_t));
  size_t payload_len = sizeof(ril_unsol_emergency_list_t);
  memset(payload, 0, payload_len);
  convertToRil(emergencyNumberList, payload);
  if (payload != nullptr) {
    int my_expect_slot = acquire_expectation_table();
    enqueue_ril_expect(payload,  // payload
                       payload_len,
                       RIL_UNSOL_EMERGENCY_NUMBERS_LIST,  // msg_id
                       -1,                                // serial_id
                       RIL_E_SUCCESS,                     // RIL error
                       ril_utf_ril_unsol_response);       // type
    release_expectation_table(my_expect_slot);
  }
  return ndk::ScopedAStatus::ok();
}
