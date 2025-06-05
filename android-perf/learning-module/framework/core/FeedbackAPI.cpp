/******************************************************************************
 *   @file    FeedbackAPI.cpp
 *   @brief   feedback apis
 *
 *   DESCRIPTION
 *   Feedback apis
 *
 *---------------------------------------------------------------------------
 *    Copyright (c) 2022 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *---------------------------------------------------------------------------
 *******************************************************************************/

 #include "VendorIPerf.h"
 #include "FeedbackAPI.h"

PerfOutputFeedback::PerfOutputFeedback() {
    mFeatureLocator = nullptr;
}

int PerfOutputFeedback::getFeedback(int hintID, std::string& appName, int hintType) {
    featureData appType = {0};
    std::string featureName = "";

    switch (hintID) {
    case VENDOR_FEEDBACK_WORKLOAD_TYPE:
        featureName = APP_CLASSIFIER;
        break;
    case VENDOR_FEEDBACK_LAUNCH_END_POINT:
        if (hintType > OBSOLETE_LAUNCH_FEEDBCK_TYP) {
            featureName = LL_FEATURE_NAME;
        } else {
            featureName = LAUNCH_END_POINT;
        }
        break;
    }

    if (mFeatureLocator != nullptr)
            mFeatureLocator->getData(featureName, appName, appType, hintType);

    DEBUGV(FEEDBACK_API_TAG, "app:%s feature:%s feedback = %d", appName.c_str(), featureName.c_str(), appType.data);
    return appType.data;
}

int PerfOutputFeedback::getFeedback(int hintID, std::string& appName, int hintType, int duration) {
    featureData appType = {0};
    std::string featureName = "";

    switch (hintID) {
        case VENDOR_FEEDBACK_GPU_HEADROOM:
        case VENDOR_FEEDBACK_MAX_PIPELINE_NUM:
            featureName = QAPE_FEATURE_NAME;
            break;
    }

    if (mFeatureLocator != nullptr)
            mFeatureLocator->getData(featureName, appName, appType, hintType, duration);

    DEBUGV(FEEDBACK_API_TAG, "app:%s feature:%s feedback = %d duration: %d", appName.c_str(), featureName.c_str(), appType.data, duration);
    return appType.data;
}

int PerfOutputFeedback::getFeedback(int hintID, char** ptr) {
    int ret = -1;
    std::string featureName = "";
    switch(hintID) {
        case VENDOR_FEEDBACK_PA_FW:
        case VENDOR_FEEDBACK_PA_SPEED:
        case VENDOR_FEEDBACK_PA_PREKILL:
        case VENDOR_FEEDBACK_PA_ULMK:
            featureName = PA_FEATURE_NAME;
            break;
    }
    if (mFeatureLocator != nullptr) {
        ret = mFeatureLocator->getData(featureName, hintID, ptr);
    }
    if (ptr != nullptr)
        DEBUGV(FEEDBACK_API_TAG, "feature:%s hintID:0x%x feedback = %s", featureName.c_str(), hintID, *ptr);
    return ret;
}

void PerfOutputFeedback::setFeatureLocator(FeatureLocator *fl) {
    mFeatureLocator = fl;
}
