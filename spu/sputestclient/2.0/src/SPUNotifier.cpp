/*!
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "spu_definitions.h"
#include "SPUNotifier.h"
#include "SPUTestFw.h"
#include "log.h"

SPUNotifier::SPUNotifier() :  cvSSROccurred(), cvSSRHandled(), mtx(), was_ssr_handled(false) {}

Return<void> SPUNotifier::callback(uint32_t eventId)
{
    switch (eventId) {
    case SPU_HAL_EVENT_SSR:
    {
        ALOGD("Event ID[%u] notified", eventId);
        unique_lock<mutex> lk(mtx);
        cvSSROccurred.notify_one();
        return Void();
    }
    default:
        ALOGE("Invalid eventId [%d], eventId");
    }

   return Void();
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
    was_ssr_handled = true;
    cvSSRHandled.notify_one();
}

void SPUNotifier::waitForSSRHandled()
{
    ALOGD("Waiting for SSR handled notification");
    unique_lock<mutex> lk(mtx);
    if (!was_ssr_handled)
        cvSSRHandled.wait(lk);
}

void SPUNotifier::resetSSRHandled()
{
    ALOGD("Reseting SSR handled flag");
    unique_lock<mutex> lk(mtx);
    was_ssr_handled = false;
}

Return<void> PrintMessageOnSSREvent::callback(uint32_t eventId)
{
    (void) eventId;
    ALOGI("%s", message.c_str());
    return Void();
}
