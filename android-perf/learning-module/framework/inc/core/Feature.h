/******************************************************************************
 *   @file    Feature.h
 *   @brief   Feature abstract base class
 *
 *   DESCRIPTION
 *      "Feature"s take measurements, "learn" at runtime and apply actions
 *   based on such learning. Feature Class provides the abstract base class
 *   for Feature implementers to derive from.
 *
 *---------------------------------------------------------------------------
 *    Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *---------------------------------------------------------------------------
 *******************************************************************************/

#ifndef FEATURE_H
#define FEATURE_H

/*
* This file Shouldn't be modified. Otherwise no-ship
* code has be recompiled because of the change.
*/

#include <string>
#include <atomic>
#include <thread>
#include "FeatureInfo.h"
#include "FeatureState.h"
#include "MetaMeter.h"
#include "Algorithm.h"
#include "Action.h"
#include "FeatureLocator.h"
#include "WaitingQueue.h"

#define TRIGGERQ_FLUSH_THRESHOLD 25

class VendorIPerfDataStore;
class VendorIPerf;

struct FeaturePayload {
    AsyncTriggerPayload payload;
    bool exitThread;
};

class PeriodicFeatureRunner {
public:
    std::condition_variable mRunPeriodic;
    std::mutex mWaitForThreads;

    void notify() {
        mRunPeriodic.notify_all();
    }

    //support for interrupted wakeups
    std::condition_variable mInterruptPeriodic;
    std::mutex mWaitForInterrupt;

    void interruptNotifier() {
        mInterruptPeriodic.notify_all();
    }
};

class Feature {
private:
    std::string mFeatureName;
protected:
    /*
     * Features should implement allocation, maintenance and destruction of
     * resources for MetaMeter, Algorithm, Action and InputDb
     * Features should assign the respective pointer to base members
     * LM checks for null and calls the pointer whenever it wants to use
     * a Feature component.
     */
    MetaMeter* mMetaMeter;
    Algorithm* mAlgorithm;
    Action* mAction;
    FeatureInfo mfeatureInfo;
    FeatureState mFeatureState;
    FeatureLocator *mFeatureLocator;
    PeriodicFeatureRunner mFeatureRunner;
    WaitingQueue<FeaturePayload> mPayloadQueue;
    bool mIsDisabled;

public:
    std::atomic_bool mRunFlag = false;
    std::atomic_int mSleepDurationInMs = -1;
    std::thread periodicAlgoThread;
    std::thread asyncDeliveryThread;
    void runPeriodic();
    std::atomic_bool runPeriodicThreadInterrupt = false;
    bool enablePeriodic = false;
    bool mEnableTracing = false;
    int mMajorVersion = 1;
    int mMinorVersion = 0;
    std::string mDependsOn = "";
    VendorIPerf* mCurrVendor;
    VendorIPerfDataStore* mStore;
    Feature(FeatureInfo featureInfo);
    virtual ~Feature() {}
    std::string&  getFeatureName() {
        return mFeatureName;
    }

    std::string& getDependentFeature() {
        return mDependsOn;
    }

    bool isEnabled() {
        return !mIsDisabled;
    }

    void Disable() {
        mIsDisabled = true;
    }

    void runAsyncDelivery();
    void storeAsync(FeaturePayload& featurePayload);
    /*
     * Features should implement the logic for Async Triggers in runAsync()
     * LM calls runAsync() for each AsyncTrigger the Feature is interested in
     */
    virtual void runAsync(AsyncTriggerPayload payload);


    /*
     * Features should implement the logic for Idle Triggers in runAsyncIdle()
     * This is generally used to call algorithms when the system is idle
     * LM calls runAsyncIdle() of the Feature when it decides the system is idle
     */
    virtual void runAsyncIdle(const std::atomic_bool& interrupt);
    /*
     * Features should implement the logic for Idle Triggers in runAsyncIdle()
     * This is generally used to call algorithms when the system is idle
     * LM calls runAsyncIdle() of the Feature when it decides the system is idle
     */
    virtual void runAsyncPeriodic(const std::atomic_bool& interrupt);

    void registerServiceLocator(FeatureLocator* FeatureLocatorService) {
        mFeatureLocator = FeatureLocatorService;
    }
    virtual int getData(int key __attribute__((unused)), char** ptr __attribute__((unused))) {
        return 0;
    }
    virtual int getData(std::string& key __attribute__((unused)), featureData &data, int type __attribute__((unused))) {
        data.data = {-1};
        return 0;
    }
    virtual int getData(std::string& key __attribute__((unused)), featureData &data, int type __attribute__((unused)), int duration __attribute__((unused))) {
        data.data = {-1};
        return 0;
    }
    virtual int setData(std::string& appName __attribute__((unused)), std::vector<int> data __attribute__((unused))) {
        return 0;
    }
    virtual int setData(int setDataType __attribute__((unused)), std::vector<int> &data __attribute__((unused))) {
        return 0;
    }

};
#endif /* FEATURE_H */

