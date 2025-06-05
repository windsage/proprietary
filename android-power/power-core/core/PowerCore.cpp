/******************************************************************************
  @file    PowerCore.cpp
  @brief   Power Core library

  DESCRIPTION

  Copyright (c) 2020-2024 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#include <dirent.h>
#include <dlfcn.h>
#include <fstream>
#include <errno.h>
#include <cutils/properties.h>
#include <cutils/trace.h>
#include "PowerCore.h"
#include "PowerOptXMLParser.h"

#define PM_THREAD_NAME "pmCoreThread"
#define XML_FILE_PATH "/vendor/etc/pwr/"
#define FEATURE_CONFIG_XML XML_FILE_PATH "PowerFeatureConfig.xml"
#define WAIT_RETRIES_FOR_DATA_DIR 20

using namespace std;
featureMap* PowerOptFeature::mFeatureMapInstanceList = nullptr;
PowerCore *PowerCore::pmInstance = 0;

PowerCore::PowerCore() : mTriggerQueue("PMTriggerQ", 10) {
    char value[MAX_STR_LEN];
    pmThreadReady = false;
    runPeriodicThreadInterrupt = false;
    pmThread = thread(&PowerCore::PwrMain, this);

    property_get("vendor.poweropt.debug", value, "0");
    debugProperty = atoi(value);

}

PowerCore::~PowerCore() {
    if(pmThread.joinable()) {
        pmThread.join();
    }
}

void PowerCore::sendEvent(uint32_t hint, string userDataStr, int32_t userData1, int32_t userData2, vector<int32_t> reserved) {
    uint32_t retVal = -1;
    powerctl_msg_t pMsg;
    (void) reserved;

    memset(&pMsg, 0, sizeof(powerctl_msg_t));

    pMsg.hintID = hint;
    pMsg.duration = userData1;
    pMsg.hintType = userData2;

    if(!userDataStr.empty()){
        strlcpy(pMsg.appName, userDataStr.c_str(), MAX_STR_LEN);
    }

    if(debugProperty == 1){
        DEBUGE(LOG_TAG_POWER,"Power hint received %d %x\n", hint, hint);
    }

    AsyncTriggerPayload payload(&pMsg);
    payload.timeStamp = 0;
    retVal = trigger(payload);

    if(retVal!=0){
        DEBUGV(LOG_TAG_POWER,"Power core not ready\n");
    }

    return;
}

int PowerCore::trigger(AsyncTriggerPayload payload) {
    /* Don't start delivering triggers until PowerCore Thread is ready */
    if(!pmThreadReady) {
        return -1;
    }

    /* Push the trigger to the TriggerQueue */
    TriggerInfo trigger;
    trigger.payload = payload;
    trigger.exitPMThread = false;
    mTriggerQueue.push(trigger);
    return 0;
}

void PowerCore::PwrMain() {
    int ret_code = 0;
    char boot_completed[PROPERTY_VALUE_MAX];
    int value = 0;

    DEBUGV(LOG_TAG_POWER, "PwrMain starting\n");

    ret_code = pthread_setname_np(pthread_self(),PM_THREAD_NAME);
    if (ret_code != 0) {
        DEBUGE(LOG_TAG_POWER, "Failed to setname ThreadName: %s\n", PM_THREAD_NAME);
        return;
    }

    /* Parse XML */
    DEBUGV(LOG_TAG_POWER, "Parsing xml's \n");

    DIR* pwrDir;
    if ((pwrDir = opendir(XML_FILE_PATH)) == NULL) {
        DEBUGE(LOG_TAG_POWER, "Fatal: can't open feature xml dir: %s, Error: %s", XML_FILE_PATH,strerror(errno));
        return;
    }

    string fileName = FEATURE_CONFIG_XML;
    featureConfigs = featureList.parseFeatureConfig(fileName);

    if(featureConfigs.size() > 0){
       for(auto i:featureConfigs) {
           fileName = XML_FILE_PATH + i + ".xml";
           DEBUGV(LOG_TAG_POWER, "Calling get PowerOptFeatureLibName for xml: %s",fileName.c_str());
           PowerOptXMLParser currFeatureXML(fileName);
           libName = currFeatureXML.getFeatureLibName();
           PowerOptFeatureInfo currFeatureInfo = currFeatureXML.getXML();
           featureName = currFeatureInfo.name;
           mfeatureEnabled = TargetDifferentiation::IsFeatureEnabled(featureName);
           if(!libName.empty() && !featureName.empty() && mfeatureEnabled) {
              void* handle = dlopen(libName.c_str(), RTLD_NOW | RTLD_LOCAL);
              if(!handle) {
                 DEBUGE(LOG_TAG_POWER,"Unable to load library %s: %s", libName.c_str(),dlerror());
              } else {
                 dlLibHandles.push_back(handle);
                 DEBUGV(LOG_TAG_POWER, "Calling get PowerOptFeature for xml: %s", fileName.c_str());
                 PowerOptFeature currFeature = PowerOptFeature(currFeatureInfo);
                 DEBUGV(LOG_TAG_POWER, "Pushing the feature: %s", currFeatureInfo.name.c_str());
                 if(currFeature.isEnabled()) {
                     mFeatureList.push_back(currFeature.getInstance(currFeatureInfo.name,currFeatureInfo));
                     mHintMap[currFeatureInfo.name] = currFeatureInfo.hintList;
                 } else {
                     //feature was disabled dynamically by feature itself, delete the feature object as
                     DEBUGV(LOG_TAG_POWER, "Disabling %s, feature was dynamically disabled", libName.c_str());
                 }
              } /*if (!handle)*/
           } /*if(!libName.empty())*/
        } /*end of loop feature config*/
     }/*if(featureConfigs.size() > 0) */
     closedir(pwrDir);

    if (mFeatureList.empty()) {
        DEBUGE(LOG_TAG_POWER, "No features enabled, exiting..");
        return;
    }

    pmThreadReady = true;
    mFeatureRunner.registerFeatureList(mFeatureList);
    for (PowerOptFeature* currFeature : mFeatureList) {
        currFeature->registerPeriodicRunner(&mFeatureRunner);
    }
    bool shouldExit = false;
    do {
        DEBUGV(LOG_TAG_POWER, "Waiting on trigger Queue");
        TriggerInfo trigger = mTriggerQueue.pop();
        if(debugProperty == 1){
            DEBUGE(LOG_TAG_POWER, "hintID:%d, appName:%s", trigger.payload.hintID, trigger.payload.appName.c_str());
        }
        shouldExit = trigger.exitPMThread;

        for (PowerOptFeature* currFeature : mFeatureList) {
            bool ret = hintFilter(trigger.payload.hintID, currFeature->getFeatureName().c_str());
            if (ret){
                 currFeature->sendEvents(trigger.payload);
                 DEBUGV(LOG_TAG_POWER,"sendEvents for %s returned, hintID:%x", currFeature->getFeatureName().c_str(), trigger.payload.hintID);
            }
        }
    } while (!shouldExit);
    DEBUGV(LOG_TAG_POWER,"Exiting PWRCore Thread");

    return;
}

bool PowerCore::hintFilter(int hintID, std::string featurename){
    auto it = find(mHintMap[featurename].begin(), mHintMap[featurename].end(), hintID);
    return it != mHintMap[featurename].end() ? true : false;
}

