/*!
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "spu_definitions.h"
#include "SPUNotifier.h"
#include "SPUTestFw.h"
#include "log.h"

using ::ndk::ScopedAStatus;

SPUNotifier::SPUNotifier() :  cvSSROccurred(), cvSSRHandled(), mtx() {}

ScopedAStatus SPUNotifier::callback(int32_t eventId)
{
    switch (eventId) {
    case SPU_HAL_EVENT_SSR:
    {
        ALOGD("Event ID[%u] notified", eventId);
        unique_lock<mutex> lk(mtx);
        cvSSROccurred.notify_one();
    } break;

    default:
        ALOGE("Invalid eventId [%d]", eventId);
    }

    return ScopedAStatus::ok();
}

void SPUNotifier::waitForSSREvent()
{
    ALOGD("Waiting for SSR occurred notification");
    unique_lock<mutex> lk(mtx);
    cvSSROccurred.wait(lk);
}

void SPUNotifier::notifySSRHandled()
{
    ALOGD("Notify SSR was handled");
    unique_lock<mutex> lk(mtx);
    cvSSRHandled.notify_one();
}

void SPUNotifier::waitForSSRHandled()
{
    ALOGD("Waiting for SSR handled notification");
    unique_lock<mutex> lk(mtx);
    cvSSRHandled.wait(lk);
}

ScopedAStatus PrintMessageOnSSREvent::callback(int32_t eventId)
{
    (void) eventId;
    ALOGI("%s", message.c_str());

    return ScopedAStatus::ok();
}
