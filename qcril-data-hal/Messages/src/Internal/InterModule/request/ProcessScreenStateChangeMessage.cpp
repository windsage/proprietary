/**
* Copyright (c) 2017, 2021 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#include <string.h>
#include "framework/legacy.h"
#include "request/ProcessScreenStateChangeMessage.h"

namespace rildata {

ProcessScreenStateChangeMessage::~ProcessScreenStateChangeMessage() {
    if (mCallback) {
        delete mCallback;
        mCallback = nullptr;
    }
}

string ProcessScreenStateChangeMessage::dump() {
    std::stringstream ss;
    ss << mName << " {";
    ss << "screenEnabled: " << screenState;
    ss << "}";
    return ss.str();
}

}// namespace
