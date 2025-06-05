/******************************************************************************
 *   @file    PowerOptFeature.cpp
 *   @brief   PowerOptFeature default implementation
 *
 *   DESCRIPTION
 *      "Feature"s take measurements, "learn" at runtime and apply actions
 *   based on such learning. Feature Class provides the Base implementation
 *   for Feature implementers to derive.
 *
 *---------------------------------------------------------------------------
 *    Copyright (c) 2020 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *---------------------------------------------------------------------------
 *******************************************************************************/

#include "PowerOptFeature.h"
#include "PowerOptDebugLog.h"

#define LOG_TAG_FEATURE "PowerOptFeature"

void PowerOptFeature::sendEvents(AsyncTriggerPayload payload) {
    DEBUGV(LOG_TAG_FEATURE, "runAsync %s", mFeatureName.c_str());
}
