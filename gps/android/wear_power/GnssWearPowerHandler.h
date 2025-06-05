/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
/*
* Copyright (c) 2021 The Linux Foundation. All rights reserved.

* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
*    * Redistributions of source code must retain the above copyright
*      notice, this list of conditions and the following disclaimer.
*    * Redistributions in binary form must reproduce the above
*      copyright notice, this list of conditions and the following
*      disclaimer in the documentation and/or other materials provided
*      with the distribution.
*    * Neither the name of The Linux Foundation nor the names of its
*      contributors may be used to endorse or promote products derived
*      from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
* ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
* BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
* IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef GNSS_WEAR_POWER_HANDLER_H
#define GNSS_WEAR_POWER_HANDLER_H

#include <utils/StrongPointer.h>
#include <utils/RefBase.h>
#include <vendor/qti/hardware/powerstateservice/1.0/IPowerStateService.h>
#include <vendor/qti/hardware/powerstateservice/1.0/IPowerStateCallback.h>
#include <unordered_map>
#include <dlfcn.h>
#include <ILocationAPI.h>


using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::hardware::hidl_death_recipient;
using ::android::sp;
using ::android::wp;
using ::android::hidl::base::V1_0::IBase;
using ::android::hardware::hidl_vec;
using ::vendor::qti::hardware::powerstateservice::V1_0::state;
using ::vendor::qti::hardware::powerstateservice::V1_0::DeviceStates;
using ::vendor::qti::hardware::powerstateservice::V1_0::IPowerStateService;
using ::vendor::qti::hardware::powerstateservice::V1_0::IPowerStateCallback;

class GnssWearPowerHandler : public IPowerStateCallback {

public:
    GnssWearPowerHandler();
    ~GnssWearPowerHandler() = default;

    /* Overrides */
    Return<bool> notifyDeepSleepEvent(state parameter) override;
    Return<bool> notifyHibernateEvent(state parameter) override;

    void handlePowerServiceDeath(uint64_t cookie, const wp<IBase>& who);

    static GnssWearPowerHandler* getGnssWearPowerHandler();
    void sendPowerEventToLocCtrlApi(PowerStateType intPowerState);
    bool connectToPowerHal();
    void initializeGnssWearPower();
    void handleGnssWearPowerEvent(int32_t powerState);
    bool subscribeToPowerHal();

private:
    struct PowerServiceDeathRecipient : public hidl_death_recipient {
        public:
            explicit PowerServiceDeathRecipient(
                const android::sp<GnssWearPowerHandler>& handler)
                : mHandler(handler) {
            }
            ~PowerServiceDeathRecipient() = default;
            void serviceDied(uint64_t cookie,
                               const wp<IBase>& who) override;
        private:
            sp<GnssWearPowerHandler> mHandler;
    };

    android::Mutex mMutex;
    std::unordered_map<int32_t, bool> mSupportedProperties;
    sp<PowerServiceDeathRecipient> mPowerServiceDeathRecipient;
    sp<IPowerStateService> mPowerHalService GUARDED_BY(mMutex);
    ILocationControlAPI *mLocationControlApi;
};

#endif // GNSS_WEAR_POWER_HANDLER_H
