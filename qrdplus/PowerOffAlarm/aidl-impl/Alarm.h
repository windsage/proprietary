/*
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#pragma once

#include <aidl/vendor/qti/hardware/alarm/BnAlarm.h>
#include <log/log.h>
#include <stdlib.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "vendor.qti.hardware.alarm-impl"

namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace alarm {

class Alarm : public BnAlarm {
public:
    virtual ::ndk::ScopedAStatus setAlarm(int64_t time, int32_t* _aidl_return) override;
    virtual ::ndk::ScopedAStatus getAlarm(int64_t* _aidl_return) override;
    virtual ::ndk::ScopedAStatus cancelAlarm(int32_t* _aidl_return) override;
    virtual ::ndk::ScopedAStatus getRtcTime(int64_t* _aidl_return) override;
};

}  // namespace alarm
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
}  // namespace aidl
