/******************************************************************************
  @file    PowerCore.h
  @brief   Power Core header

  DESCRIPTION

  Copyright (c) 2020-2024 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef POWERCORE_H
#define POWERCORE_H

#include <vector>
#include <thread>
#include <unordered_map>
#include <atomic>
#include "PowerOptXMLParser.h"
#include "PowerOptDebugLog.h"
#include "PowerOptAsyncData.h"
#include "PowerOptWaitingQueue.h"
#include "PowerOptFeature.h"
#include "TargetDifferentiation.h"

#define LOG_TAG_POWER "PowerCore"

struct TriggerInfo{
    AsyncTriggerPayload payload;
    bool exitPMThread;
};

class PowerCore {
private:
    PowerCore();
    static PowerCore* pmInstance;
    std::string libName;
    std::string fileName;
    std::string featureName;
    bool mfeatureEnabled;
    std::vector<PowerOptFeature*> mFeatureList;
    PowerOptPeriodicFeatureRunner mFeatureRunner;
    std::atomic_bool pmThreadReady;
    std::thread pmThread;
    std::atomic_bool runPeriodicThreadInterrupt;
    std::map<std::string, std::vector<int>> mHintMap;

    WaitingQueue<TriggerInfo> mTriggerQueue;
    PowerCore(PowerCore const&);
    PowerCore& operator=(PowerCore const&);
    ~PowerCore();

    void PwrMain();
    std::vector<void*> dlLibHandles;
    std::vector<std::string> featureConfigs;
    PowerOptXMLParser featureList;
    int debugProperty;

public:
    static PowerCore* getInstance() {
        if(!pmInstance){
            pmInstance = new PowerCore();
        }
        return pmInstance;
    }
    int trigger(AsyncTriggerPayload payload);
    void sendEvent(uint32_t hint, std::string userDataStr, int32_t userData1, int32_t userData2, std::vector<int32_t> reserved);
    bool hintFilter(int hintID, std::string featurename);
};


#endif /* POWERCORE_H */
