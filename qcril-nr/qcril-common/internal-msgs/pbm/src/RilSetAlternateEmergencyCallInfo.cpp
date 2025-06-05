/**
 *  Copyright (c) 2020 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "modules/pbm/RilSetAlternateEmergencyCallInfo.h"

RilSetAlternateEmergencyCallInfo::~RilSetAlternateEmergencyCallInfo() {
    if(mCallback){
        delete mCallback;
        mCallback = nullptr;
    }
}

char* RilSetAlternateEmergencyCallInfo::getNumber() { return number;}
uint32_t RilSetAlternateEmergencyCallInfo::getEmerCateg() {return emerCateg;}
void RilSetAlternateEmergencyCallInfo::setNumber(char* num) {number = num;}
void RilSetAlternateEmergencyCallInfo::setEmerCateg(uint32_t emerCat) {emerCateg = emerCat;}


string RilSetAlternateEmergencyCallInfo::dump() { return mName; }
