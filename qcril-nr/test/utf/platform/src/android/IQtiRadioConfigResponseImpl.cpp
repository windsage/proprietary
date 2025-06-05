/*===========================================================================
 *    Copyright (c) 2023 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *===========================================================================*/
#include "platform/android/IQtiRadioConfigResponseImpl.h"

::ndk::ScopedAStatus aidlqtiradioconfig::IQtiRadioConfigResponseImpl::setMsimPreferenceResponse(
    int32_t in_serial, int32_t in_errorCode)
{
  int slot = acquire_expectation_table();
  enqueue_ril_expect(nullptr,  // payload
                     0,
                     RIL_REQUEST_SET_MSIM_PREFERENCE,  // msg_id
                     in_serial,                        // serial_id
                     (RIL_Errno)in_errorCode,          // RIL error
                     ril_utf_ril_response);            // type

  release_expectation_table(slot);

  return ndk::ScopedAStatus::ok();
}