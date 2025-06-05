/******************************************************************************
 *   @file    Feature.cpp
 *   @brief   Feature default implementation
 *
 *   DESCRIPTION
 *      "Feature"s take measurements, "learn" at runtime and apply actions
 *   based on such learning. Feature Class provides the Base implementation
 *   for Feature implementers to derive.
 *
 *---------------------------------------------------------------------------
 *    Copyright (c) 2017 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *---------------------------------------------------------------------------
 *******************************************************************************/

#define ATRACE_TAG ATRACE_TAG_ALWAYS
#include <cutils/trace.h>
#include "Feature.h"
#include "DebugLog.h"
#include "internal/WaitingQueue.h"
#include "DynamicLoader.h"

#undef LOG_TAG_FEATURE
#define LOG_TAG_FEATURE "Feature"
#define THREAD_NAME_SZ_LIMIT 16
#define SUCCESS 0

using namespace std;
Feature::Feature(FeatureInfo featureInfo) :
    mFeatureName(featureInfo.name),
    mDependsOn(featureInfo.dependsOn),
    mMetaMeter(nullptr),
    mAlgorithm(nullptr),
    mAction(nullptr),
    mfeatureInfo(featureInfo),
    mFeatureState(featureInfo.name),
    mFeatureLocator(nullptr),
    mPayloadQueue(featureInfo.name,TRIGGERQ_FLUSH_THRESHOLD),
    mIsDisabled(false) {
    //property to enable tracing
    mEnableTracing = (property_get_bool("vendor.debug.trace.perf", 0) == 1);

    mCurrVendor = DynamicLoader::getInstance().mCurrVendor;
    if (mCurrVendor == NULL) {
        DEBUGE(LOG_TAG_FEATURE, "mCurrVendor is Empty!!");
    }

    mStore = DynamicLoader::getInstance().mStore;
    if (mStore == NULL) {
       DEBUGE(LOG_TAG_FEATURE, "mStore is Empty!!");
    }

}

void Feature::runAsync(AsyncTriggerPayload payload) {
    DEBUGV(LOG_TAG_FEATURE, "runAsync %s", mFeatureName.c_str());
    if (mMetaMeter) {
        /*
         * Check if the metameter is interested in this trigger:
         * We need to send the trigger if DEFAULT_HINTTYPE is present
         * for this hintID (metameter is interested in all hintTypes)
         * or if current hintType is present for this hintID in the
         * FeatureInfo
         *  */
        auto currHintIDTypes = mfeatureInfo.asyncIdList[payload.hintID];
        if (currHintIDTypes.count(DEFAULT_HINTTYPE) == 1
                || currHintIDTypes.count(payload.hintType) == 1) {
            DEBUGV(LOG_TAG_FEATURE, "calling handleAsync() %s", mFeatureName.c_str());
            mMetaMeter->handleAsync(payload);
        }
    }
}

void Feature::storeAsync(FeaturePayload& featurePayload) {
    DEBUGV(LOG_TAG_FEATURE, "%s Exit[%d] storeAsync Id-%d, _t-%d, appName %s ",
            mFeatureName.c_str(),featurePayload.exitThread,featurePayload.payload.hintID,
            featurePayload.payload.hintType, featurePayload.payload.appName.c_str());
    try {
        mPayloadQueue.push(featurePayload);
    } catch(exception &e) {
         DEBUGE(LOG_TAG_FEATURE, "Exception caught: %s", e.what());
    }
}

void Feature::runAsyncDelivery() {
    // Thread Setup Start
    int ret_code = 0;
    char thread_name[THREAD_NAME_SZ_LIMIT] = {0};
    snprintf(thread_name, THREAD_NAME_SZ_LIMIT-1, "adt_%s", mFeatureName.c_str());
    ret_code = pthread_setname_np(pthread_self(), thread_name);
    if (ret_code != SUCCESS) {
        DEBUGE(LOG_TAG_FEATURE, "Failed to name Thread:  %s", thread_name);
    }
    // Thread Setup Ends

    try {
        bool exit = false;
        do {
            auto trigger = mPayloadQueue.pop();
            exit = trigger.exitThread;
            if (!exit) {
                runAsync(trigger.payload);
                DEBUGD(LOG_TAG_FEATURE, "[%s] Delivered trigger for: %s[%d]", mFeatureName.c_str(),
                    trigger.payload.appName.c_str(), trigger.payload.hintID);
            }
        } while(!exit);

    } catch(exception &e) {
        DEBUGE(LOG_TAG_FEATURE, "Exception caught: %s", e.what());
    }
}

void Feature::runPeriodic() {
     int sleepDurationInMs = -1;
     while (runPeriodicThreadInterrupt == false) {
         sleepDurationInMs = -1;
         DEBUGV(LOG_TAG_FEATURE, "Starting threads, feature name = %s", this->getFeatureName().c_str());

         unique_lock<mutex> lk(this->mFeatureRunner.mWaitForThreads);
         DEBUGV(LOG_TAG_FEATURE, "Waiting for threads to signal");
         this->mFeatureRunner.mRunPeriodic.wait(lk, [this]{return ((mSleepDurationInMs != -1) && mRunFlag);});
         sleepDurationInMs = this->mSleepDurationInMs;
         DEBUGV(LOG_TAG_FEATURE, "Signal received from thread");
         lk.unlock();

         unique_lock<mutex> lck(this->mFeatureRunner.mWaitForInterrupt);
         this->mFeatureRunner.mInterruptPeriodic.wait_for(lck, chrono::milliseconds(sleepDurationInMs));
         lck.unlock();

         if(this->mRunFlag) {
            DEBUGV(LOG_TAG_FEATURE,"Calling runAsyncPeriodic for %s", this->getFeatureName().c_str());
            if(mEnableTracing) {
                string traceLog = "LM-runPeriodic() " + this->getFeatureName();
                ATRACE_ASYNC_BEGIN(traceLog.c_str(), 0);
            }
            this->runAsyncPeriodic(false);
            if(mEnableTracing) {
                string traceLog = "LM-runPeriodic() " + this->getFeatureName();
                ATRACE_ASYNC_END(traceLog.c_str(), 0);
            }
            DEBUGV(LOG_TAG_FEATURE,"runPeriodic for %s returned", this->getFeatureName().c_str());
         }
     }
 }

void Feature::runAsyncPeriodic(const std::atomic_bool& interrupt) {
    /* Each feature should implement it's own */
}

void Feature::runAsyncIdle(const atomic_bool& interrupt) {
    DEBUGV(LOG_TAG_FEATURE, "runAsyncIdle %s", mFeatureName.c_str());
    if(mAlgorithm) {
        DEBUGV(LOG_TAG_FEATURE, "calling runAlgo() %s", mFeatureName.c_str());
        mAlgorithm->runAlgo(interrupt);
    }
}


Feature* FeatureLocator::lookup(std::string& featureName) {
    Feature *currFeature = nullptr;
    int featureFound = 0;
    for (std::vector <Feature *>::size_type i = 0; i < mFeatureList.size(); i++) {
        if (mFeatureList[i]->getFeatureName() == featureName) {
            currFeature = mFeatureList[i];
            featureFound = 1;
            break;
        }
    }
    DEBUGD(LOG_TAG_FEATURE, "Feature Name = %s, total featuers = %lu, featureFound = %d",
           featureName.c_str(), mFeatureList.size(), featureFound);
    return currFeature;
}

int FeatureLocator::getData(std::string& featureName, int hintID, char** ptr) {
    Feature *currFeature;
    int ret = -1;
    currFeature = lookup(featureName);
    if (currFeature) {
        ret = currFeature->getData(hintID, ptr);
    }
    if (ptr != nullptr)
        DEBUGD(LOG_TAG_FEATURE, "feedback = %s", *ptr);
    return ret;
}

int FeatureLocator::getData(std::string& featureName, std::string& appName, featureData &data, int type) {
    Feature *currFeature;
    int ret = -1;
    currFeature = lookup(featureName);
    if (currFeature) {
        ret = currFeature->getData(appName, data, type);
    }
    DEBUGD(LOG_TAG_FEATURE, "data.data = %d", data.data);
    return ret;
}

int FeatureLocator::getData(std::string& featureName, std::string& appName, featureData &data, int type, int duration) {
    DEBUGD(LOG_TAG_FEATURE, "Feature.cpp FeatureLocator::getData featureName: %s", featureName.c_str());
    Feature *currFeature;
    int ret = -1;
    currFeature = lookup(featureName);
    if (currFeature) {
        ret = currFeature->getData(appName, data, type, duration);
    }
    DEBUGD(LOG_TAG_FEATURE, "data.data = %d", data.data);
    return ret;
}

int FeatureLocator::setData(std::string& featureName, std::string& appName, std::vector<int> data) {
    Feature *currFeature;
    int ret = -1;
    currFeature = lookup(featureName);
    if (currFeature) {
        ret = currFeature->setData(appName, data);
    }
    return ret;
}

int FeatureLocator::setData(std::string& featureName , int setDataType , std::vector<int> &data) {
    Feature *currFeature;
    int ret = -1;
    currFeature = lookup(featureName);
    if (currFeature) {
        ret = currFeature->setData(setDataType ,data);
    }
    return ret;
}

void FeatureLocator::registerFeatureList(std::vector<Feature*> featureList) {
    mFeatureList = featureList;
}
