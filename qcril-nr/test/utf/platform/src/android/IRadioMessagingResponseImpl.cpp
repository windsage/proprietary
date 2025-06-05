/*===========================================================================
 *    Copyright (c) 2022 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *===========================================================================*/

#include "platform/android/IRadioMessagingResponseImpl.h"

::ndk::ScopedAStatus aidlmsg::IRadioMessagingResponseImpl::acknowledgeIncomingGsmSmsWithPduResponse(
    const ::aidlradio::RadioResponseInfo& in_info)
{
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlmsg::IRadioMessagingResponseImpl::acknowledgeLastIncomingCdmaSmsResponse(
    const ::aidlradio::RadioResponseInfo& in_info)
{
  int slot = acquire_expectation_table();
  enqueue_ril_expect(nullptr,  // payload
                     0,
                     RIL_REQUEST_CDMA_SMS_ACKNOWLEDGE,  // msg_id
                     in_info.serial,                    // serial_id
                     (RIL_Errno)in_info.error,          // RIL error
                     ril_utf_ril_response);             // type
  release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlmsg::IRadioMessagingResponseImpl::acknowledgeLastIncomingGsmSmsResponse(
    const ::aidlradio::RadioResponseInfo& in_info)
{
  int slot = acquire_expectation_table();
  enqueue_ril_expect(nullptr,  // payload
                     0,
                     RIL_REQUEST_SMS_ACKNOWLEDGE,  // msg_id
                     in_info.serial,               // serial_id
                     (RIL_Errno)in_info.error,     // RIL error
                     ril_utf_ril_response);        // type
  release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlmsg::IRadioMessagingResponseImpl::acknowledgeRequest(int32_t in_serial)
{
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlmsg::IRadioMessagingResponseImpl::deleteSmsOnRuimResponse(
    const ::aidlradio::RadioResponseInfo& in_info)
{
  int slot = acquire_expectation_table();
  enqueue_ril_expect(nullptr,  // payload
                     0,
                     RIL_REQUEST_CDMA_DELETE_SMS_ON_RUIM,  // msg_id
                     in_info.serial,                       // serial_id
                     (RIL_Errno)in_info.error,             // RIL error
                     ril_utf_ril_response);                // type
  release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlmsg::IRadioMessagingResponseImpl::deleteSmsOnSimResponse(
    const ::aidlradio::RadioResponseInfo& in_info)
{
  int slot = acquire_expectation_table();
  enqueue_ril_expect(nullptr,  // payload
                     0,
                     RIL_REQUEST_DELETE_SMS_ON_SIM,  // msg_id
                     in_info.serial,                 // serial_id
                     (RIL_Errno)in_info.error,       // RIL error
                     ril_utf_ril_response);          // type
  release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlmsg::IRadioMessagingResponseImpl::getCdmaBroadcastConfigResponse(
    const ::aidlradio::RadioResponseInfo& in_info,
    const std::vector<::aidlmsg::CdmaBroadcastSmsConfigInfo>& in_configs)
{
  int slot = acquire_expectation_table();
  int num_configs = in_configs.size();
  RIL_CDMA_BroadcastSmsConfigInfo** payload = nullptr;
  size_t payload_len = 0;
  if (num_configs > 0) {
    payload = (RIL_CDMA_BroadcastSmsConfigInfo**)malloc(sizeof(RIL_CDMA_BroadcastSmsConfigInfo*) *
                                                        num_configs);
    memset(payload, 0x0, num_configs * sizeof(RIL_GSM_BroadcastSmsConfigInfo**));
    payload_len = sizeof(RIL_CDMA_BroadcastSmsConfigInfo*) * num_configs;

    for (int i = 0; i < num_configs; i++) {
      payload[i] = (RIL_CDMA_BroadcastSmsConfigInfo*)malloc(sizeof(RIL_CDMA_BroadcastSmsConfigInfo));
      memset(payload[i], 0x0, sizeof(RIL_CDMA_BroadcastSmsConfigInfo));
      payload[i]->service_category = (int)(in_configs[i].serviceCategory);
      payload[i]->language = (int)(in_configs[i].language);
      payload[i]->selected = (unsigned char)(in_configs[i].selected);
    }
  }
  enqueue_ril_expect(payload,  // payload
                     payload_len,
                     RIL_REQUEST_CDMA_GET_BROADCAST_SMS_CONFIG,  // msg_id
                     in_info.serial,                             // serial_id
                     (RIL_Errno)in_info.error,                   // RIL error
                     ril_utf_ril_response);                      // type
  release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlmsg::IRadioMessagingResponseImpl::getGsmBroadcastConfigResponse(
    const ::aidlradio::RadioResponseInfo& in_info,
    const std::vector<::aidlmsg::GsmBroadcastSmsConfigInfo>& in_configs)
{
  int slot = acquire_expectation_table();
  int num_configs = in_configs.size();
  bool allocation_failure = false;
  RIL_GSM_BroadcastSmsConfigInfo** payload = nullptr;
  size_t payload_len = 0;
  if (num_configs > 0) {
    payload = (RIL_GSM_BroadcastSmsConfigInfo**)malloc(sizeof(RIL_GSM_BroadcastSmsConfigInfo*) *
                                                       num_configs);
    memset(payload, 0x0, sizeof(RIL_GSM_BroadcastSmsConfigInfo**));
    payload_len = sizeof(RIL_GSM_BroadcastSmsConfigInfo*) * num_configs;

    for (int i = 0; i < num_configs; i++) {
      payload[i] = (RIL_GSM_BroadcastSmsConfigInfo*)malloc(sizeof(RIL_GSM_BroadcastSmsConfigInfo));
      memset(payload[i], 0x0, sizeof(RIL_GSM_BroadcastSmsConfigInfo));
      payload[i]->fromServiceId = (int)(in_configs[i].fromServiceId);
      payload[i]->toServiceId = (int)(in_configs[i].toServiceId);
      payload[i]->fromCodeScheme = (int)(in_configs[i].fromCodeScheme);
      payload[i]->toCodeScheme = (int)(in_configs[i].toCodeScheme);
      payload[i]->selected = (unsigned char)(in_configs[i].selected);
    }
  }

  enqueue_ril_expect(payload,  // payload
                     payload_len,
                     RIL_REQUEST_GSM_GET_BROADCAST_SMS_CONFIG,  // msg_id
                     in_info.serial,                            // serial_id
                     (RIL_Errno)in_info.error,                  // RIL error
                     ril_utf_ril_response);                     // type
  release_expectation_table(slot);

  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlmsg::IRadioMessagingResponseImpl::getSmscAddressResponse(
    const ::aidlradio::RadioResponseInfo& in_info, const std::string& in_smsc)
{
  size_t payload_len = sizeof(ril_request_get_smsc_address_resp_t);
  ril_request_get_smsc_address_resp_t* payload =
      (ril_request_get_smsc_address_resp_t*)malloc(payload_len);
  memset(payload, 0x0, payload_len);
  if (in_smsc.size() > 0) {
    strlcpy(payload->SMSC_address,
            in_smsc.c_str(),
            MIN(sizeof(payload->SMSC_address), in_smsc.size() + 1));
  }
  int slot = acquire_expectation_table();
  enqueue_ril_expect(payload,  // payload
                     payload_len,
                     RIL_REQUEST_GET_SMSC_ADDRESS,  // msg_id
                     in_info.serial,                // serial_id
                     (RIL_Errno)in_info.error,      // RIL error
                     ril_utf_ril_response);         // type
  release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlmsg::IRadioMessagingResponseImpl::reportSmsMemoryStatusResponse(
    const ::aidlradio::RadioResponseInfo& in_info)
{
  int slot = acquire_expectation_table();
  enqueue_ril_expect(nullptr,  // payload
                     0,
                     RIL_REQUEST_REPORT_SMS_MEMORY_STATUS,  // msg_id
                     in_info.serial,                        // serial_id
                     (RIL_Errno)in_info.error,              // RIL error
                     ril_utf_ril_response);                 // type
  release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlmsg::IRadioMessagingResponseImpl::sendCdmaSmsExpectMoreResponse(
    const ::aidlradio::RadioResponseInfo& in_info, const ::aidlmsg::SendSmsResult& in_sms)
{
  size_t payload_len = sizeof(RIL_SMS_Response);
  RIL_SMS_Response* payload = (RIL_SMS_Response*)malloc(payload_len);
  convertGsmSmsAidlToRilPayload(in_sms, *payload);
  memset(payload, 0x0, payload_len);
  int slot = acquire_expectation_table();
  enqueue_ril_expect(payload,  // payload
                     payload_len,
                     RIL_REQUEST_CDMA_SEND_SMS_EXPECT_MORE,  // msg_id
                     in_info.serial,                         // serial_id
                     (RIL_Errno)in_info.error,               // RIL error
                     ril_utf_ril_response);                  // type
  release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlmsg::IRadioMessagingResponseImpl::sendCdmaSmsResponse(
    const ::aidlradio::RadioResponseInfo& in_info, const ::aidlmsg::SendSmsResult& in_sms)
{
  size_t payload_len = sizeof(RIL_SMS_Response);
  RIL_SMS_Response* payload = (RIL_SMS_Response*)malloc(payload_len);
  memset(payload, 0x0, payload_len);
  convertGsmSmsAidlToRilPayload(in_sms, *payload);
  int slot = acquire_expectation_table();
  enqueue_ril_expect(payload,  // payload
                     payload_len,
                     RIL_REQUEST_CDMA_SEND_SMS,  // msg_id
                     in_info.serial,             // serial_id
                     (RIL_Errno)in_info.error,   // RIL error
                     ril_utf_ril_response);      // type
  release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlmsg::IRadioMessagingResponseImpl::sendImsSmsResponse(
    const ::aidlradio::RadioResponseInfo& in_info, const ::aidlmsg::SendSmsResult& in_sms)
{
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlmsg::IRadioMessagingResponseImpl::sendSmsExpectMoreResponse(
    const ::aidlradio::RadioResponseInfo& in_info, const ::aidlmsg::SendSmsResult& in_sms)
{
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlmsg::IRadioMessagingResponseImpl::sendSmsResponse(
    const ::aidlradio::RadioResponseInfo& in_info, const ::aidlmsg::SendSmsResult& in_sms)
{
  size_t payload_len = sizeof(RIL_SMS_Response);
  RIL_SMS_Response* payload = (RIL_SMS_Response*)malloc(payload_len);
  memset(payload, 0x0, payload_len);
  convertGsmSmsAidlToRilPayload(in_sms, *payload);
  int slot = acquire_expectation_table();
  enqueue_ril_expect(payload,  // payload
                     payload_len,
                     RIL_REQUEST_SEND_SMS,      // msg_id
                     in_info.serial,            // serial_id
                     (RIL_Errno)in_info.error,  // RIL error
                     ril_utf_ril_response);     // type

  release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlmsg::IRadioMessagingResponseImpl::setCdmaBroadcastActivationResponse(
    const ::aidlradio::RadioResponseInfo& in_info)
{
  int slot = acquire_expectation_table();
  enqueue_ril_expect(nullptr,  // payload
                     0,
                     RIL_REQUEST_CDMA_SMS_BROADCAST_ACTIVATION,  // msg_id
                     in_info.serial,                             // serial_id
                     (RIL_Errno)in_info.error,                   // RIL error
                     ril_utf_ril_response);                      // type
  release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlmsg::IRadioMessagingResponseImpl::setCdmaBroadcastConfigResponse(
    const ::aidlradio::RadioResponseInfo& in_info)
{
  int slot = acquire_expectation_table();
  enqueue_ril_expect(nullptr,  // payload
                     0,
                     RIL_REQUEST_CDMA_SET_BROADCAST_SMS_CONFIG,  // msg_id
                     in_info.serial,                             // serial_id
                     (RIL_Errno)in_info.error,                   // RIL error
                     ril_utf_ril_response);                      // type
  release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlmsg::IRadioMessagingResponseImpl::setGsmBroadcastActivationResponse(
    const ::aidlradio::RadioResponseInfo& in_info)
{
  int slot = acquire_expectation_table();
  enqueue_ril_expect(nullptr,  // payload
                     0,
                     RIL_REQUEST_GSM_SMS_BROADCAST_ACTIVATION,  // msg_id
                     in_info.serial,                            // serial_id
                     (RIL_Errno)in_info.error,                  // RIL error
                     ril_utf_ril_response);                     // type
  release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlmsg::IRadioMessagingResponseImpl::setGsmBroadcastConfigResponse(
    const ::aidlradio::RadioResponseInfo& in_info)
{
  int slot = acquire_expectation_table();
  enqueue_ril_expect(nullptr,  // payload
                     0,
                     RIL_REQUEST_GSM_SET_BROADCAST_SMS_CONFIG,  // msg_id
                     in_info.serial,                            // serial_id
                     (RIL_Errno)in_info.error,                  // RIL error
                     ril_utf_ril_response);                     // type
  release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlmsg::IRadioMessagingResponseImpl::setSmscAddressResponse(
    const ::aidlradio::RadioResponseInfo& in_info)
{
  int slot = acquire_expectation_table();
  enqueue_ril_expect(nullptr,  // payload
                     0,
                     RIL_REQUEST_SET_SMSC_ADDRESS,  // msg_id
                     in_info.serial,                // serial_id
                     (RIL_Errno)in_info.error,      // RIL error
                     ril_utf_ril_response);         // type
  release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlmsg::IRadioMessagingResponseImpl::writeSmsToRuimResponse(
    const ::aidlradio::RadioResponseInfo& in_info, int32_t in_index)
{
  int slot = acquire_expectation_table();
  size_t payload_len = sizeof(int);
  int* payload = (int*)malloc(payload_len);
  memset(payload, 0x0, payload_len);
  *payload = in_index;
  enqueue_ril_expect(payload,  // payload
                     payload_len,
                     RIL_REQUEST_CDMA_WRITE_SMS_TO_RUIM,  // msg_id
                     in_info.serial,                      // serial_id
                     (RIL_Errno)in_info.error,            // RIL error
                     ril_utf_ril_response);               // type
  release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus aidlmsg::IRadioMessagingResponseImpl::writeSmsToSimResponse(
    const ::aidlradio::RadioResponseInfo& in_info, int32_t in_index)
{
  size_t payload_len = sizeof(ril_request_write_sms_to_sim_resp_t);
  ril_request_write_sms_to_sim_resp_t* payload =
      (ril_request_write_sms_to_sim_resp_t*)malloc(payload_len);
  memset(payload, 0x0, payload_len);
  payload->response = in_index;
  int slot = acquire_expectation_table();
  enqueue_ril_expect(payload,  // payload
                     sizeof(ril_request_write_sms_to_sim_resp_t),
                     RIL_REQUEST_WRITE_SMS_TO_SIM,  // msg_id
                     in_info.serial,                // serial_id
                     (RIL_Errno)in_info.error,      // RIL error
                     ril_utf_ril_response);         // type
  release_expectation_table(slot);
  return ndk::ScopedAStatus::ok();
}
