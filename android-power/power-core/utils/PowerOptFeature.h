/******************************************************************************
 *   @file    PowerOptFeature.h
 *   @brief   Feature abstract base class
 *
 *   DESCRIPTION
 *      "Feature"s take measurements, "learn" at runtime and apply actions
 *   based on such learning. Feature Class provides the abstract base class
 *   for Feature implementers to derive from.
 *
 *---------------------------------------------------------------------------
 *    Copyright (c) 2020 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *---------------------------------------------------------------------------
 *******************************************************************************/

#ifndef POWEROPT_FEATURE_H
#define POWEROPT_FEATURE_H

#include <string>
#include <atomic>
#include <mutex>
#include <map>
#include <condition_variable>
#include "PowerOptFeatureInfo.h"
#include "PowerOptAsyncData.h"
#include "PowerOptDebugLog.h"
#define LOG_TAG_FEATURE "PowerOptFeature"

struct featureData {
    int data;
};

class PowerOptPeriodicFeatureRunner;
class PowerOptFeature;

template<typename T> PowerOptFeature* createInstance(PowerOptFeatureInfo featureInfo) { return new T(featureInfo); }

typedef std::map<std::string, PowerOptFeature*(*)(PowerOptFeatureInfo featureInfo)> featureMap;

class PowerOptFeature {
private:
    std::string mFeatureName;
    static featureMap* mFeatureMapInstanceList;
protected:
    PowerOptFeatureInfo mfeatureInfo;
    PowerOptPeriodicFeatureRunner *mFeatureRunner;
    bool mIsDisabled;

public:
    unsigned long int mSleepDurationInMs = -1;
    PowerOptFeature(PowerOptFeatureInfo featureInfo) :
        mFeatureName(featureInfo.name),
        mfeatureInfo(featureInfo),
        mIsDisabled(false) {
            DEBUGV(LOG_TAG_FEATURE, "is disabled %d feature name %s", mIsDisabled,  mFeatureName.c_str());
    }

    PowerOptFeature(){
    }

    virtual ~PowerOptFeature() {}

    std::string getFeatureName() {
        return mFeatureName;
    }

    bool isEnabled() {
        return !mIsDisabled;
    }

    void Disable() {
        mIsDisabled = true;
    }

    virtual void sendEvents(AsyncTriggerPayload payload);

    static PowerOptFeature* getInstance(std::string featureName,PowerOptFeatureInfo mfeatureInfo ){
        auto mFeatureMapInstance = getMap();
        auto currentInstance =  mFeatureMapInstance->find(featureName);
        DEBUGV(LOG_TAG_FEATURE, "Feature Name %s Feature object %p", featureName.c_str(), mFeatureMapInstance->find(featureName)->second);
        if(currentInstance == mFeatureMapInstance->end()) {
            return nullptr;
        }
        return mFeatureMapInstance->find(featureName)->second(mfeatureInfo);
    }

    static featureMap* getMap() {
        if(!mFeatureMapInstanceList) {
            mFeatureMapInstanceList = new featureMap;
        }
        return mFeatureMapInstanceList;
    }

    void registerPeriodicRunner(PowerOptPeriodicFeatureRunner* featureRunner) {
        mFeatureRunner = featureRunner;
    }

    virtual int getData(std::string key __attribute__((unused)), featureData &data, int type __attribute__((unused))) {
        data.data = {-1};
        return 0;
    }
};

class PowerOptPeriodicFeatureRunner {
private:
    std::vector<PowerOptFeature*> mFeatureList;

public:
    std::condition_variable mRunPeriodic;
    std::mutex mWaitForThreads;

    void notify() {
        mRunPeriodic.notify_all();
    }

    void registerFeatureList(std::vector<PowerOptFeature*> featureList) {
        mFeatureList = featureList;
    }
};


template <typename FType>
class FeatureInstance : PowerOptFeature {
    public:
        FeatureInstance(std::string featureName){
            auto mFeatureMapInstance = PowerOptFeature::getMap();
            mFeatureMapInstance->insert(std::make_pair(featureName, &createInstance<FType>));
    }
};

#endif /* POWEROPT_FEATURE_H */