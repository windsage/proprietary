/******************************************************************************
 *   @file    FeedbackAPI.h
 *   @brief   feedback apis
 *
 *   DESCRIPTION
 *   Feedback apis
 *
 *---------------------------------------------------------------------------
 *    Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *---------------------------------------------------------------------------
 *******************************************************************************/

#ifndef _FEEDBACK_API_H_
#define _FEEDBACK_API_H_

/*
* This file Shouldn't be modified. Otherwise no-ship
* code has be recompiled because of the change.
*/

#include "FeatureLocator.h"
#include <vector>
#include <pthread.h>
#include <DebugLog.h>

#define FEEDBACK_API_TAG "FeedbackAPI"

#define APP_CLASSIFIER "AppClassifier"
#define LAUNCH_END_POINT "AdaptLaunch"
#define QGPE_FEATURE_NAME "APEngine"
#define LL_FEATURE_NAME "LightningLaunches"
#define PA_FEATURE_NAME "PreferredApps"
#define QAPE_FEATURE_NAME "QAPE"
#define OBSOLETE_LAUNCH_FEEDBCK_TYP 2

class PerfOutputFeedback {
private:
    FeatureLocator *mFeatureLocator;

private:
    static PerfOutputFeedback *mInstance;
    static pthread_mutex_t mMutex;

    //ctor, copy ctor, assignment overloading
    PerfOutputFeedback();
    PerfOutputFeedback(PerfOutputFeedback const& copy);               //Not to be implemented
    PerfOutputFeedback& operator=(PerfOutputFeedback const& copy);    //Not to be implemented

public:

    virtual ~PerfOutputFeedback() {}

    int getFeedback(int hintID, std::string& appName, int hintType);

    int getFeedback(int hintID, std::string& appName, int hintType, int duration);

    int getFeedback(int hintID, char** ptr);

    void setFeatureLocator(FeatureLocator *fl);

    static PerfOutputFeedback* getInstance() {
        pthread_mutex_lock(&mMutex);
        if (!mInstance) {
            mInstance = new(std::nothrow) PerfOutputFeedback();
            if (!mInstance) {
                DEBUGE(FEEDBACK_API_TAG, "AdaptLaunchAction::getInstance failure");
                pthread_mutex_unlock(&mMutex);
                return NULL;
            }
        }
        pthread_mutex_unlock(&mMutex);
        return mInstance;
    }
};

#endif /* _FEEDBACK_API_H_ */
