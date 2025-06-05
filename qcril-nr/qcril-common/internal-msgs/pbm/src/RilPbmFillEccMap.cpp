/**
 *  Copyright (c) 2020 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "modules/pbm/RilPbmFillEccMap.h"

RilPbmFillEccMap::~RilPbmFillEccMap() {
    if(mCallback){
        delete mCallback;
        mCallback = nullptr;
    }
}

void RilPbmFillEccMap::setEmergencyNums(char* number) { ecc_numbers = number; }

void RilPbmFillEccMap::setEccMap(std::shared_ptr<qcril::interfaces::RIL_EccMapType> ecc_map_arg) {
    ecc_map = ecc_map_arg;
}

void RilPbmFillEccMap::setSources(qcril::interfaces::RIL_EccNumberSourceMask sources_arg) {
    sources = sources_arg;
}

void RilPbmFillEccMap::setMcc(char* mccVal) { mcc = mccVal; }

void RilPbmFillEccMap::setMnc(char* mncVal) { mnc = mncVal; }

char* RilPbmFillEccMap::getEmergencyNums() { return ecc_numbers; }

std::shared_ptr<qcril::interfaces::RIL_EccMapType> RilPbmFillEccMap::getEccMap() { return ecc_map; }

qcril::interfaces::RIL_EccNumberSourceMask RilPbmFillEccMap::getSources() { return sources; }

char* RilPbmFillEccMap::getMcc() { return mcc; }

char* RilPbmFillEccMap::getMnc() { return mnc; }

string RilPbmFillEccMap::dump() { return mName; }
