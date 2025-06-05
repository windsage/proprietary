/*!
 *
 * Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#pragma once

#include <vendor/qti/spu/1.0/ISpcomSSREvent.h>
#include <hidl/Status.h>
#include <condition_variable>
#include <mutex>
#include <string>

using namespace std;

using android::sp;
using ::android::hardware::Void;
using ::android::hardware::Return;
using vendor::qti::spu::V1_0::ISpcomSSREvent;

struct SpcomSSREvent : public ISpcomSSREvent {

public:
    explicit SpcomSSREvent();
    virtual ~SpcomSSREvent() {}

    void waitForSSREvent();
    void waitForSSRHandled();
    void notifySSRHandled();

    Return<void> callback() override;

private:
    condition_variable mCvSSROccurred;
    condition_variable mCvSSRHandled;
    mutex mMtx;
};

struct PrintMessageOnSSREvent : public ISpcomSSREvent {

public:
    explicit PrintMessageOnSSREvent(string msg) : mMsg(msg) {};
    virtual ~PrintMessageOnSSREvent() {};
    Return<void> callback() override;

private:
    string mMsg;
};
