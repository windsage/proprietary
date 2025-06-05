/*===========================================================================
 *    Copyright (c) 2022 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *===========================================================================*/

#include "platform/android/IRadioMessagingIndicationImpl.h"

::ndk::ScopedAStatus aidlmsg::IRadioMessagingIndicationImpl::cdmaNewSms(
    ::aidlradio::RadioIndicationType in_type, const ::aidlmsg::CdmaSmsMessage& in_msg)
{
  size_t payload_len = sizeof(RIL_CDMA_SMS_Message);
  RIL_CDMA_SMS_Message* payload = (RIL_CDMA_SMS_Message*)malloc(payload_len);
  if (payload != nullptr) {
    aidlToRilCdmaSmsMessage(*payload, in_msg);

    int my_expect_slot = acquire_expectation_table();
    enqueue_ril_expect(payload,  // payload
                       payload_len,
                       RIL_UNSOL_RESPONSE_CDMA_NEW_SMS,  // msg_id
                       -1,                               // serial_id
                       RIL_E_SUCCESS,                    // RIL error
                       ril_utf_ril_unsol_response);      // type
    release_expectation_table(my_expect_slot);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlmsg::IRadioMessagingIndicationImpl::cdmaRuimSmsStorageFull(
    ::aidlradio::RadioIndicationType in_type)
{
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlmsg::IRadioMessagingIndicationImpl::newBroadcastSms(
    ::aidlradio::RadioIndicationType in_type, const std::vector<uint8_t>& in_data)
{
  size_t payload_len = in_data.size();
  uint8_t* payload = (uint8_t*)malloc(payload_len);
  if (payload != nullptr) {
    memcpy(payload, in_data.data(), payload_len);
    int my_expect_slot = acquire_expectation_table();
    enqueue_ril_expect(payload,  // payload
                       payload_len,
                       RIL_UNSOL_RESPONSE_NEW_BROADCAST_SMS,  // msg_id
                       -1,                                    // serial_id
                       RIL_E_SUCCESS,                         // RIL error
                       ril_utf_ril_unsol_response);           // type
    release_expectation_table(my_expect_slot);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlmsg::IRadioMessagingIndicationImpl::newSms(
    ::aidlradio::RadioIndicationType in_type, const std::vector<uint8_t>& in_pdu)
{
  size_t payload_len = in_pdu.size();
  uint8_t* payload = (uint8_t*)malloc(payload_len);
  if (payload != nullptr) {
    memcpy(payload, in_pdu.data(), payload_len);
    int my_expect_slot = acquire_expectation_table();
    enqueue_ril_expect(payload,  // payload
                       payload_len,
                       RIL_UNSOL_RESPONSE_NEW_SMS,   // msg_id
                       -1,                           // serial_id
                       RIL_E_SUCCESS,                // RIL error
                       ril_utf_ril_unsol_response);  // type
    release_expectation_table(my_expect_slot);
  }

  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlmsg::IRadioMessagingIndicationImpl::newSmsOnSim(
    ::aidlradio::RadioIndicationType in_type, int32_t in_recordNumber)
{
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlmsg::IRadioMessagingIndicationImpl::newSmsStatusReport(
    ::aidlradio::RadioIndicationType in_type, const std::vector<uint8_t>& in_pdu)
{
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlmsg::IRadioMessagingIndicationImpl::simSmsStorageFull(
    ::aidlradio::RadioIndicationType in_type)
{
  return ndk::ScopedAStatus::ok();
}