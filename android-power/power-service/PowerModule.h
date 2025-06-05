/**************************************************************
 * @file PowerModule.h
 * @brief Power module HAL impl
 * DESCRIPTION
 *
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * ************************************************************/

#ifndef VENDOR_QTI_HARDWARE_IPOWERMODULE_V1_0_H
#define VENDOR_QTI_HARDWARE_IPOWERMODULE_V1_0_H

#include <aidl/vendor/qti/hardware/power/powermodule/BnPowerModule.h>
#include "PowerCore.h"


namespace aidl{
namespace vendor{
namespace qti{
namespace hardware{
namespace power{
namespace powermodule{

using ::ndk::ScopedAStatus;

class PowerModule : public BnPowerModule{
    private:
        void Init();
        PowerCore *mPowerCore = mPowerCore->getInstance();
    public:
        PowerModule();
        ~PowerModule();
        ScopedAStatus powerSendEvents(int32_t hintId, const std::string& userData, int32_t userData_1, int32_t userData_2, const std::vector<int32_t>& spaceholder) override;
};

}//namespace powermodule
}//namespace power
}//namespace hardware
}//namespace qti
}//namespace vendor
}//namespace aidl

#endif //VENDOR_QTI_HARDWARE_IPOWERMODULE_V1_0_H
