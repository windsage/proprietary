                /*!
 *
 * Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#include <chrono>

#include "SPUTestFw.h"
#include "log.h"
#include "SpcomSSREvent.h"

using namespace std::chrono_literals;

SpcomSSREvent::SpcomSSREvent() :  mCvSSROccurred(), mCvSSRHandled(), mMtx() {}

// SSR callback to be called by SPU service when SPU SSR event occurs
Return<void> SpcomSSREvent::callback() {

    ALOGD("SSR Callback was called");
    unique_lock<mutex> lk(mMtx);
    mCvSSROccurred.notify_one();
    return Void();
}

void SpcomSSREvent::waitForSSREvent() {

    ALOGD("Waiting for SSR occurred notification");
    unique_lock<mutex> lk(mMtx);
    mCvSSROccurred.wait(lk);
}

void SpcomSSREvent::notifySSRHandled() {

    ALOGD("Notify SSR was handled");
    unique_lock<mutex> lk(mMtx);
    mCvSSRHandled.notify_one();
}

void SpcomSSREvent::waitForSSRHandled() {

    ALOGD("Waiting for SSR handled notification");
    unique_lock<mutex> lk(mMtx);
    mCvSSRHandled.wait(lk);
}

Return<void> PrintMessageOnSSREvent::callback() {
    ALOGI("%s", mMsg.c_str());
    return Void();
}
