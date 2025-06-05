/*
 *  Copyright (c) 2020 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "modules/pbm/RilPbmEnabEmerNumInd.h"

RilPbmEnabEmerNumInd::~RilPbmEnabEmerNumInd() {
    if(mCallback){
        delete mCallback;
        mCallback = nullptr;
    }
}

void RilPbmEnabEmerNumInd::setStatus(bool val) {status = val;}
bool RilPbmEnabEmerNumInd::getStatus() {return status;}
string RilPbmEnabEmerNumInd::dump() { return mName; }
