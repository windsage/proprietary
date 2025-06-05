/*===========================================================================
 *    Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *===========================================================================*/

#include "platform/android/IRadioVoiceResponseImpl.h"
#include "platform/android/VoiceAidlUtil.h"

#include "ril_utf_rild_sim.h"

::ndk::ScopedAStatus aidlvoice::IRadioVoiceResponseImpl::acceptCallResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& in_info)
{
  int slot = acquire_expectation_table();
  enqueue_ril_expect(nullptr,  // payload
                     0,
                     RIL_REQUEST_ANSWER,        // msg_id
                     in_info.serial,            // serial_id
                     (RIL_Errno)in_info.error,  // RIL error
                     ril_utf_ril_response);     // type
  release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlvoice::IRadioVoiceResponseImpl::conferenceResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& in_info)
{
  int slot = acquire_expectation_table();
  enqueue_ril_expect(nullptr,  // payload
                     0,
                     RIL_REQUEST_CONFERENCE,    // msg_id
                     in_info.serial,            // serial_id
                     (RIL_Errno)in_info.error,  // RIL error
                     ril_utf_ril_response);     // type
  release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlvoice::IRadioVoiceResponseImpl::dialResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& in_info)
{
  int slot = acquire_expectation_table();
  enqueue_ril_expect(nullptr,  // payload
                     0,
                     RIL_REQUEST_DIAL,          // msg_id
                     in_info.serial,            // serial_id
                     (RIL_Errno)in_info.error,  // RIL error
                     ril_utf_ril_response);     // type
  release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlvoice::IRadioVoiceResponseImpl::explicitCallTransferResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& in_info)
{
  int slot = acquire_expectation_table();
  enqueue_ril_expect(nullptr,  // payload
                     0,
                     RIL_REQUEST_EXPLICIT_CALL_TRANSFER,  // msg_id
                     in_info.serial,                      // serial_id
                     (RIL_Errno)in_info.error,            // RIL error
                     ril_utf_ril_response);               // type
  release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlvoice::IRadioVoiceResponseImpl::getCallForwardStatusResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& in_info,
    const std::vector<::aidl::android::hardware::radio::voice::CallForwardInfo>& in_callForwardInfos)
{
  RIL_CallForwardInfo** payload = nullptr;
  int num_info = in_callForwardInfos.size();
  int size = num_info * sizeof(RIL_CallForwardInfo*);
  if (num_info) {
    payload = (RIL_CallForwardInfo**)malloc(sizeof(RIL_CallForwardInfo*) * num_info);
    for (int i = 0; i < num_info; ++i) {
      payload[i] = (RIL_CallForwardInfo*)malloc(sizeof(RIL_CallForwardInfo));
      convertCallForwardInfotoUtf(*(payload[i]), in_callForwardInfos[i]);
    }
  }

  int slot = acquire_expectation_table();
  enqueue_ril_expect(payload,  // payload
                     size,
                     RIL_REQUEST_QUERY_CALL_FORWARD_STATUS,  // msg_id
                     in_info.serial,                         // serial_id
                     (RIL_Errno)in_info.error,               // RIL error
                     ril_utf_ril_response);                  // type
  release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlvoice::IRadioVoiceResponseImpl::getCallWaitingResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& in_info,
    bool in_enable,
    int32_t in_serviceClass)
{
  auto payload =
      (ril_request_query_call_waiting_resp_t*)malloc(sizeof(ril_request_query_call_waiting_resp_t));
  if (payload != nullptr) {
    payload->status = (ril_call_waiting_status_enum_type)in_enable;
    int slot = acquire_expectation_table();
    enqueue_ril_expect(payload,  // payload
                       sizeof(ril_request_query_call_waiting_resp_t),
                       RIL_REQUEST_QUERY_CALL_WAITING,  // msg_id
                       in_info.serial,                  // serial_id
                       (RIL_Errno)in_info.error,        // RIL error
                       ril_utf_ril_response);           // type
    release_expectation_table(slot);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlvoice::IRadioVoiceResponseImpl::getClipResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& in_info,
    ::aidl::android::hardware::radio::voice::ClipStatus in_status)
{
  auto payload = (int*)malloc(sizeof(int));
  if (payload != nullptr) {
    if (in_status == ::aidl::android::hardware::radio::voice::ClipStatus::CLIP_PROVISIONED) {
      *payload = 0;
    } else if (in_status == ::aidl::android::hardware::radio::voice::ClipStatus::CLIP_UNPROVISIONED) {
      *payload = 1;
    } else {
      *payload = 2;
    }
    int slot = acquire_expectation_table();
    enqueue_ril_expect(payload,  // payload
                       sizeof(int),
                       RIL_REQUEST_QUERY_CLIP,    // msg_id
                       in_info.serial,            // serial_id
                       (RIL_Errno)in_info.error,  // RIL error
                       ril_utf_ril_response);     // type
    release_expectation_table(slot);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlvoice::IRadioVoiceResponseImpl::getClirResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& in_info, int32_t in_n, int32_t in_m)
{
  auto payload = (ril_request_get_clir_resp_t*)malloc(sizeof(ril_request_get_clir_resp_t));
  if (payload != nullptr) {
    payload->n_parameter = in_n;
    payload->m_parameter = in_m;
    int slot = acquire_expectation_table();
    enqueue_ril_expect(payload,  // payload
                       sizeof(ril_request_get_clir_resp_t),
                       RIL_REQUEST_GET_CLIR,      // msg_id
                       in_info.serial,            // serial_id
                       (RIL_Errno)in_info.error,  // RIL error
                       ril_utf_ril_response);     // type
    release_expectation_table(slot);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlvoice::IRadioVoiceResponseImpl::getCurrentCallsResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& in_info,
    const std::vector<::aidl::android::hardware::radio::voice::Call>& in_calls)
{
  int num_calls = in_calls.size();
  RIL_Call** payload = nullptr;
  size_t payload_len = 0;
  if (num_calls > 0) {
    payload = (RIL_Call**)malloc(sizeof(RIL_Call*) * num_calls);
    payload_len = sizeof(RIL_Call*) * num_calls;

    for (int i = 0; i < num_calls; ++i) {
      payload[i] = (RIL_Call*)malloc(sizeof(RIL_Call));
      memset(payload[i], 0, sizeof(RIL_Call));
      payload[i]->state = (RIL_CallState)(in_calls[i].state);
      payload[i]->index = in_calls[i].index;
      payload[i]->toa = in_calls[i].toa;
      payload[i]->isMpty = in_calls[i].isMpty;
      payload[i]->isMT = in_calls[i].isMT;
      payload[i]->als = in_calls[i].als;
      payload[i]->isVoice = in_calls[i].isVoice;
      payload[i]->isVoicePrivacy = in_calls[i].isVoicePrivacy;
      payload[i]->number = strndup(in_calls[i].number.c_str(), in_calls[i].number.size() + 1);
      payload[i]->numberPresentation = (int)in_calls[i].numberPresentation;
      payload[i]->name = strndup(in_calls[i].name.c_str(), in_calls[i].name.size() + 1);
      payload[i]->namePresentation = (int)in_calls[i].namePresentation;
      // TODO copy UusInfo into payload and add verification in framework
      payload[i]->audioQuality = (RIL_AudioQuality)in_calls[i].audioQuality;
      payload[i]->redirNum =
          strndup(in_calls[i].forwardedNumber.c_str(), in_calls[i].forwardedNumber.size() + 1);
    }
  }
  int slot = acquire_expectation_table();
  enqueue_ril_expect(payload,  // payload
                     payload_len,
                     RIL_REQUEST_GET_CURRENT_CALLS,  // msg_id
                     in_info.serial,                 // serial_id
                     (RIL_Errno)in_info.error,       // RIL error
                     ril_utf_ril_response);          // type
  release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlvoice::IRadioVoiceResponseImpl::getLastCallFailCauseResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& in_info,
    const ::aidl::android::hardware::radio::voice::LastCallFailCauseInfo& in_failCauseinfo)
{
  auto payload = (RIL_LastCallFailCauseInfo*)malloc(sizeof(RIL_LastCallFailCauseInfo));
  if (payload != nullptr) {
    payload->cause_code = (RIL_LastCallFailCause)in_failCauseinfo.causeCode;
    if (!in_failCauseinfo.vendorCause.empty()) {
      payload->vendor_cause =
          strndup(in_failCauseinfo.vendorCause.c_str(), in_failCauseinfo.vendorCause.size() + 1);
    }

    int slot = acquire_expectation_table();
    enqueue_ril_expect(payload,  // payload
                       sizeof(RIL_LastCallFailCauseInfo),
                       RIL_REQUEST_LAST_CALL_FAIL_CAUSE,  // msg_id
                       in_info.serial,                    // serial_id
                       (RIL_Errno)in_info.error,          // RIL error
                       ril_utf_ril_response);             // type
    release_expectation_table(slot);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlvoice::IRadioVoiceResponseImpl::getMuteResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& in_info, bool in_enable)
{
  auto payload = (ril_request_mute_t*)malloc(sizeof(ril_request_mute_t));
  if (payload != nullptr) {
    payload->enable = in_enable;
    int slot = acquire_expectation_table();
    enqueue_ril_expect(payload,  // payload
                       sizeof(ril_request_mute_t),
                       RIL_REQUEST_GET_MUTE,      // msg_id
                       in_info.serial,            // serial_id
                       (RIL_Errno)in_info.error,  // RIL error
                       ril_utf_ril_response);     // type
    release_expectation_table(slot);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlvoice::IRadioVoiceResponseImpl::getPreferredVoicePrivacyResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& in_info, bool in_enable)
{
  auto payload = (ril_request_cdma_query_preferred_voice_privacy_mode_t*)malloc(
      sizeof(ril_request_cdma_query_preferred_voice_privacy_mode_t));
  if (payload != nullptr) {
    payload->voice_privacy_mode = STANDARD_PRIVACY_MODE;
    if (in_enable) {
      payload->voice_privacy_mode = ENHANCED_PRIVACY_MODE;
    }
    int slot = acquire_expectation_table();
    enqueue_ril_expect(payload,  // payload
                       sizeof(ril_request_cdma_query_preferred_voice_privacy_mode_t),
                       RIL_REQUEST_CDMA_QUERY_PREFERRED_VOICE_PRIVACY_MODE,  // msg_id
                       in_info.serial,                                       // serial_id
                       (RIL_Errno)in_info.error,                             // RIL error
                       ril_utf_ril_response);                                // type
    release_expectation_table(slot);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlvoice::IRadioVoiceResponseImpl::getTtyModeResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& in_info,
    ::aidl::android::hardware::radio::voice::TtyMode in_mode)
{
  auto payload = (ril_request_get_tty_mode_t*)malloc(sizeof(ril_request_get_tty_mode_t));
  if (payload != nullptr) {
    payload->tty_mode = (ril_tty_mode_enum_type)in_mode;
    int slot = acquire_expectation_table();
    enqueue_ril_expect(payload,  // payload
                       sizeof(ril_request_get_tty_mode_t),
                       RIL_REQUEST_QUERY_TTY_MODE,  // msg_id
                       in_info.serial,              // serial_id
                       (RIL_Errno)in_info.error,    // RIL error
                       ril_utf_ril_response);       // type
    release_expectation_table(slot);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlvoice::IRadioVoiceResponseImpl::hangupConnectionResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& in_info)
{
  int slot = acquire_expectation_table();
  enqueue_ril_expect(nullptr,  // payload
                     0,
                     RIL_REQUEST_HANGUP,        // msg_id
                     in_info.serial,            // serial_id
                     (RIL_Errno)in_info.error,  // RIL error
                     ril_utf_ril_response);     // type
  release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlvoice::IRadioVoiceResponseImpl::hangupForegroundResumeBackgroundResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& in_info)
{
  int slot = acquire_expectation_table();
  enqueue_ril_expect(nullptr,  // payload
                     0,
                     RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND,  // msg_id
                     in_info.serial,                                   // serial_id
                     (RIL_Errno)in_info.error,                         // RIL error
                     ril_utf_ril_response);                            // type
  release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlvoice::IRadioVoiceResponseImpl::hangupWaitingOrBackgroundResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& in_info)
{
  int slot = acquire_expectation_table();
  enqueue_ril_expect(nullptr,  // payload
                     0,
                     RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND,  // msg_id
                     in_info.serial,                            // serial_id
                     (RIL_Errno)in_info.error,                  // RIL error
                     ril_utf_ril_response);                     // type
  release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlvoice::IRadioVoiceResponseImpl::rejectCallResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& in_info)
{
  int slot = acquire_expectation_table();
  enqueue_ril_expect(nullptr,  // payload
                     0,
                     RIL_REQUEST_UDUB,          // msg_id
                     in_info.serial,            // serial_id
                     (RIL_Errno)in_info.error,  // RIL error
                     ril_utf_ril_response);     // type
  release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlvoice::IRadioVoiceResponseImpl::sendBurstDtmfResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& in_info)
{
  int slot = acquire_expectation_table();
  enqueue_ril_expect(nullptr,  // payload
                     0,
                     RIL_REQUEST_CDMA_BURST_DTMF,  // msg_id
                     in_info.serial,               // serial_id
                     (RIL_Errno)in_info.error,     // RIL error
                     ril_utf_ril_response);        // type
  release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlvoice::IRadioVoiceResponseImpl::sendCdmaFeatureCodeResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& in_info)
{
  int slot = acquire_expectation_table();
  enqueue_ril_expect(nullptr,  // payload
                     0,
                     RIL_REQUEST_CDMA_FLASH,    // msg_id
                     in_info.serial,            // serial_id
                     (RIL_Errno)in_info.error,  // RIL error
                     ril_utf_ril_response);     // type
  release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlvoice::IRadioVoiceResponseImpl::sendDtmfResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& in_info)
{
  int slot = acquire_expectation_table();
  enqueue_ril_expect(nullptr,  // payload
                     0,
                     RIL_REQUEST_DTMF,          // msg_id
                     in_info.serial,            // serial_id
                     (RIL_Errno)in_info.error,  // RIL error
                     ril_utf_ril_response);     // type
  release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlvoice::IRadioVoiceResponseImpl::separateConnectionResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& in_info)
{
  int slot = acquire_expectation_table();
  enqueue_ril_expect(nullptr,  // payload
                     0,
                     RIL_REQUEST_SEPARATE_CONNECTION,  // msg_id
                     in_info.serial,                   // serial_id
                     (RIL_Errno)in_info.error,         // RIL error
                     ril_utf_ril_response);            // type
  release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlvoice::IRadioVoiceResponseImpl::setCallForwardResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& in_info)
{
  int slot = acquire_expectation_table();
  enqueue_ril_expect(nullptr,  // payload
                     0,
                     RIL_REQUEST_SET_CALL_FORWARD,  // msg_id
                     in_info.serial,                // serial_id
                     (RIL_Errno)in_info.error,      // RIL error
                     ril_utf_ril_response);         // type
  release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlvoice::IRadioVoiceResponseImpl::setCallWaitingResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& in_info)
{
  int slot = acquire_expectation_table();
  enqueue_ril_expect(nullptr,  // payload
                     0,
                     RIL_REQUEST_SET_CALL_WAITING,  // msg_id
                     in_info.serial,                // serial_id
                     (RIL_Errno)in_info.error,      // RIL error
                     ril_utf_ril_response);         // type
  release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlvoice::IRadioVoiceResponseImpl::setClirResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& in_info)
{
  int slot = acquire_expectation_table();
  enqueue_ril_expect(nullptr,  // payload
                     0,
                     RIL_REQUEST_SET_CLIR,      // msg_id
                     in_info.serial,            // serial_id
                     (RIL_Errno)in_info.error,  // RIL error
                     ril_utf_ril_response);     // type
  release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlvoice::IRadioVoiceResponseImpl::setMuteResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& in_info)
{
  int slot = acquire_expectation_table();
  enqueue_ril_expect(nullptr,  // payload
                     0,
                     RIL_REQUEST_SET_MUTE,      // msg_id
                     in_info.serial,            // serial_id
                     (RIL_Errno)in_info.error,  // RIL error
                     ril_utf_ril_response);     // type
  release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlvoice::IRadioVoiceResponseImpl::setPreferredVoicePrivacyResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& in_info)
{
  int slot = acquire_expectation_table();
  enqueue_ril_expect(nullptr,  // payload
                     0,
                     RIL_REQUEST_CDMA_SET_PREFERRED_VOICE_PRIVACY_MODE,  // msg_id
                     in_info.serial,                                     // serial_id
                     (RIL_Errno)in_info.error,                           // RIL error
                     ril_utf_ril_response);                              // type
  release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlvoice::IRadioVoiceResponseImpl::setTtyModeResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& in_info)
{
  int slot = acquire_expectation_table();
  enqueue_ril_expect(nullptr,  // payload
                     0,
                     RIL_REQUEST_SET_TTY_MODE,  // msg_id
                     in_info.serial,            // serial_id
                     (RIL_Errno)in_info.error,  // RIL error
                     ril_utf_ril_response);     // type
  release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlvoice::IRadioVoiceResponseImpl::startDtmfResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& in_info)
{
  int slot = acquire_expectation_table();
  enqueue_ril_expect(nullptr,  // payload
                     0,
                     RIL_REQUEST_DTMF_START,    // msg_id
                     in_info.serial,            // serial_id
                     (RIL_Errno)in_info.error,  // RIL error
                     ril_utf_ril_response);     // type
  release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlvoice::IRadioVoiceResponseImpl::stopDtmfResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& in_info)
{
  int slot = acquire_expectation_table();
  enqueue_ril_expect(nullptr,  // payload
                     0,
                     RIL_REQUEST_DTMF_STOP,     // msg_id
                     in_info.serial,            // serial_id
                     (RIL_Errno)in_info.error,  // RIL error
                     ril_utf_ril_response);     // type
  release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus aidlvoice::IRadioVoiceResponseImpl::switchWaitingOrHoldingAndActiveResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& in_info)
{
  int slot = acquire_expectation_table();
  enqueue_ril_expect(nullptr,  // payload
                     0,
                     RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE,  // msg_id
                     in_info.serial,                                    // serial_id
                     (RIL_Errno)in_info.error,                          // RIL error
                     ril_utf_ril_response);                             // type
  release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidl::android::hardware::radio::voice::IRadioVoiceResponseImpl::
  setVoNrEnabledResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) {
    int slot = acquire_expectation_table();
    enqueue_ril_expect(
            NULL,
            0,
            RIL_REQUEST_ENABLE_VONR,
            in_info.serial,
            (RIL_Errno)in_info.error,
            ril_utf_ril_response);
    release_expectation_table(slot);
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidl::android::hardware::radio::voice::IRadioVoiceResponseImpl::
  isVoNrEnabledResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info,
                        bool in_enable) {
    size_t payload_len = sizeof(ril_request_set_vo_nr_enabled_t);
    ril_request_set_vo_nr_enabled_t* payload =
        (ril_request_set_vo_nr_enabled_t*)malloc(payload_len);
    payload->mEnable = in_enable;
    int slot = acquire_expectation_table();
    enqueue_ril_expect(
            payload,
            payload_len,
            RIL_REQUEST_IS_VONR_ENABLED,
            in_info.serial,
            (RIL_Errno)in_info.error,
            ril_utf_ril_response);
    release_expectation_table(slot);
    return ndk::ScopedAStatus::ok();
}
