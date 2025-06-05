/*===========================================================================
 *  Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 *===========================================================================*/

#pragma once

#include "aidl/android/hardware/radio/voice/Dial.h"
#include "aidl/android/hardware/radio/voice/CallForwardInfo.h"
#include "aidl/android/hardware/radio/voice/IRadioVoiceIndication.h"

#include "ril_utf_rild_sim.h"

void convertToRil(const std::vector<::aidl::android::hardware::radio::voice::EmergencyNumber>& in,
                  ril_unsol_emergency_list_t* out);
int convertDialInfotoAidl(void* data,
                          size_t datalen,
                          ::aidl::android::hardware::radio::voice::Dial& dialInfo);
int convertCallForwardInfotoUtf(RIL_CallForwardInfo& out,
                                const ::aidl::android::hardware::radio::voice::CallForwardInfo& in);
int convertCallForwardInfoStatustoUtf(int in);
int convertCallForwardInfotoAidl(void* data,
                                 size_t datalen,
                                 ::aidl::android::hardware::radio::voice::CallForwardInfo& info);
int convertCallForwardInfoStatustoAidl(int in);
