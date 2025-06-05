/**
 *  Copyright (c) 2020 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "modules/pbm/RilNumberIsEmergency.h"

RilNumberIsEmergency::~RilNumberIsEmergency() {
    if(mCallback){
        delete mCallback;
        mCallback = nullptr;
    }
}

char* RilNumberIsEmergency::getNumber() { return number;}
void RilNumberIsEmergency::setNumber(char* num) {number = num;}
string RilNumberIsEmergency::dump() { return mName; }
