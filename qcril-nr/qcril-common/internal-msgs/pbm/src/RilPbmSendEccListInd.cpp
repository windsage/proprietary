/*
 *  Copyright (c) 2020 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "modules/pbm/RilPbmSendEccListInd.h"

RilPbmSendEccListInd::~RilPbmSendEccListInd() {
    if(mCallback){
        delete mCallback;
        mCallback = nullptr;
    }
}

string RilPbmSendEccListInd::dump() { return mName; }
