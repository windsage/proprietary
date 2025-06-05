/**************************************************************
 * @file PowerModule.cpp
 * @brief Power module HAL impl
 * DESCRIPTION
 *
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * ************************************************************/

#define LOG_TAG "Powermodule"
#include "PowerModule.h"

namespace aidl{
namespace vendor{
namespace qti{
namespace hardware{
namespace power{
namespace powermodule{

    PowerModule::PowerModule(){

    }

    PowerModule::~PowerModule(){

    }

    ScopedAStatus PowerModule::powerSendEvents(int32_t hintId, const std::string& userData, int32_t userData_1, int32_t userData_2, const std::vector<int32_t>& spaceholder){
        if(mPowerCore != nullptr){
            mPowerCore->sendEvent(hintId, userData, userData_1, userData_2, spaceholder);
        }
        return ndk::ScopedAStatus::ok();
    }

}//namespace powermodule
}//namespace power
}//namespace hardware
}//namespace qti
}//namespace vendor
}//namespace aidl
