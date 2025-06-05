/*===========================================================================
 *    Copyright (c) 2022 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *===========================================================================*/

#include "platform/android/IRadioSimResponseImpl.h"

namespace aidlradio {
    using namespace aidl::android::hardware::radio;
}

::ndk::ScopedAStatus aidlradio::sim::IRadioSimResponseImpl::getCdmaSubscriptionResponse(
    const ::aidlradio::RadioResponseInfo& in_info,
    const std::string& in_mdn,
    const std::string& in_hSid,
    const std::string& in_hNid,
    const std::string& in_min,
    const std::string& in_prl)
{
  size_t payload_len = sizeof(qcril::interfaces::RilGetCdmaSubscriptionResult_t);
  qcril::interfaces::RilGetCdmaSubscriptionResult_t* payload =
      (qcril::interfaces::RilGetCdmaSubscriptionResult_t*)malloc(payload_len);
  memset(payload, 0x0, payload_len);

  payload->mdn = in_mdn;
  payload->hSid = in_hSid;
  payload->hNid = in_hNid;
  payload->min = in_min;
  payload->prl = in_prl;

  int slot = acquire_expectation_table();
  enqueue_ril_expect(payload,  // payload
                     payload_len,
                     RIL_REQUEST_CDMA_SUBSCRIPTION,
                     in_info.serial,            // serial_id
                     (RIL_Errno)in_info.error,  // RIL error
                     ril_utf_ril_response);     // type
  release_expectation_table(slot);

  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlradio::sim::IRadioSimResponseImpl::areUiccApplicationsEnabledResponse(
    const ::aidlradio::RadioResponseInfo& in_info, bool in_enabled)
{
  size_t payload_len = sizeof(qcril::interfaces::RilGetUiccAppStatusResult_t);
  qcril::interfaces::RilGetUiccAppStatusResult_t* payload =
      (qcril::interfaces::RilGetUiccAppStatusResult_t*)malloc(payload_len);
  payload->state = in_enabled;

  int slot = acquire_expectation_table();
  enqueue_ril_expect(payload,  // payload
                     payload_len,
                     RIL_REQUEST_GET_UICC_APPLICATIONS_ENABLEMENT,  // msg_id
                     in_info.serial,                                // serial_id
                     (RIL_Errno)in_info.error,                      // RIL error
                     ril_utf_ril_response);                         // type

  release_expectation_table(slot);

  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlradio::sim::IRadioSimResponseImpl::setUiccSubscriptionResponse(
    const ::aidlradio::RadioResponseInfo& in_info)
{
  int slot = acquire_expectation_table();
  enqueue_ril_expect(NULL,  // payload
                     0,
                     RIL_REQUEST_SET_UICC_SUBSCRIPTION,  // msg_id
                     in_info.serial,                     // serial_id
                     (RIL_Errno)in_info.error,           // RIL error
                     ril_utf_ril_response);              // type

  release_expectation_table(slot);

  return ndk::ScopedAStatus::ok();
}