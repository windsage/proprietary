/**
 *  Copyright (c) 2020 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "modules/pbm/RilPbmProcessAltEmerCall.h"

RilPbmProcessAltEmerCall::~RilPbmProcessAltEmerCall() {
    if(mCallback){
        delete mCallback;
        mCallback = nullptr;
    }
}

char* RilPbmProcessAltEmerCall::getNumber() { return number;}
void RilPbmProcessAltEmerCall::setNumber(char* num) {number = num;}
void RilPbmProcessAltEmerCall::setCallCategory(uint32_t categ) {callCateg = categ;}
uint32_t RilPbmProcessAltEmerCall::getCallCategory() { return callCateg;}
void RilPbmProcessAltEmerCall::setNonStd(bool status) { is_non_std_otasp = status; }
bool RilPbmProcessAltEmerCall::getNonStd() { return is_non_std_otasp; }
void RilPbmProcessAltEmerCall::resetNonStd() { is_non_std_otasp = false;}
string RilPbmProcessAltEmerCall::dump() { return mName; }
