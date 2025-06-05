/*===========================================================================
 *    Copyright (c) 2022 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *===========================================================================*/

#include "platform/android/IRadioConfigIndicationImpl.h"

::ndk::ScopedAStatus aidlconfig::IRadioConfigIndicationImpl::simSlotsStatusChanged(
    ::aidlradio::RadioIndicationType in_type,
    const std::vector<::aidlconfig::SimSlotStatus>& in_slotStatus)
{
  return ndk::ScopedAStatus::ok();
}