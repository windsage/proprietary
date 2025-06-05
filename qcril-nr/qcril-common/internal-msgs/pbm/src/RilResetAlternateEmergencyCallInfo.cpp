/*
 *  Copyright (c) 2020 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "modules/pbm/RilResetAlternateEmergencyCallInfo.h"

RilResetAlternateEmergencyCallInfo::~RilResetAlternateEmergencyCallInfo() {
    if(mCallback){
        delete mCallback;
        mCallback = nullptr;
    }
}

string RilResetAlternateEmergencyCallInfo::dump() { return mName; }
