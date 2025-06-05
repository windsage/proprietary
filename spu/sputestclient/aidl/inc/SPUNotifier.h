/*!
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include <aidl/vendor/qti/hardware/spu/BnSPUNotifier.h>
#include <condition_variable>
#include <mutex>
#include <string>

using namespace std;
using namespace aidl::vendor::qti::hardware::spu;
using aidl::vendor::qti::hardware::spu::BnSPUNotifier;
using ::ndk::ScopedAStatus;

struct SPUNotifier : public BnSPUNotifier {

public:
    explicit SPUNotifier();
    virtual ~SPUNotifier() {}

    void waitForSSREvent();
    void waitForSSRHandled();
    void notifySSRHandled();

    ScopedAStatus callback(int32_t eventId);

private:
    condition_variable cvSSROccurred;
    condition_variable cvSSRHandled;
    mutex mtx;
};

struct PrintMessageOnSSREvent : public BnSPUNotifier {

public:
    explicit PrintMessageOnSSREvent(string msg) : message(msg) {};
    virtual ~PrintMessageOnSSREvent() {};
    ScopedAStatus callback(int32_t);

private:
    string message;
};
