/*!
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include <vendor/qti/spu/2.0/ISPUNotifier.h>
#include <hidl/Status.h>
#include <condition_variable>
#include <mutex>
#include <string>

using namespace std;

using android::sp;
using ::android::hardware::Void;
using ::android::hardware::Return;
using vendor::qti::spu::V2_0::ISPUNotifier;

struct SPUNotifier : public ISPUNotifier {

public:
    explicit SPUNotifier();
    virtual ~SPUNotifier() {}

    void waitForSSREvent();
    void waitForSSRHandled();
    void notifySSRHandled();
    void resetSSRHandled();

    Return<void> callback(uint32_t eventId) override;

private:
    condition_variable cvSSROccurred;
    condition_variable cvSSRHandled;
    mutex mtx;
    bool was_ssr_handled;
};

struct PrintMessageOnSSREvent : public ISPUNotifier {

public:
    explicit PrintMessageOnSSREvent(string msg) : message(msg) {};
    virtual ~PrintMessageOnSSREvent() {};
    Return<void> callback(uint32_t) override;

private:
    string message;
};
