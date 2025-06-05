/**
 *  Copyright (c) 2020 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "modules/pbm/RilPbmSupportEcc.h"

RilPbmSupportEcc::~RilPbmSupportEcc() {
    if(mCallback){
        delete mCallback;
        mCallback = nullptr;
    }
}

string RilPbmSupportEcc::dump() { return mName; }
