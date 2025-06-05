/*
 * Copyright (c) 2021-2022 Qualcomm Technologies, Inc.
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
#define LOG_TAG "LocSvc_GnssWearPowerHandler"

#include <log_util.h>
#include <gps_extended_c.h>
#include <utils/Mutex.h>
#include "GnssWearPowerHandler.h"
#include "android-base/macros.h"
#include <chrono>
#include <thread>

using android::Mutex;

// Power state report
constexpr int32_t POWER_EVENT_DEEP_SLEEP_ENTRY = static_cast<int32_t>(state::DEEP_SLEEP_ENTER);
constexpr int32_t POWER_EVENT_DEEP_SLEEP_EXIT = static_cast<int32_t>(state::DEEP_SLEEP_EXIT);
constexpr int32_t POWER_STATE_DEEPSLEEP = static_cast<int32_t>(DeviceStates::POWER_STATE_DEEPSLEEP);
constexpr int32_t POWER_STATE_HIBERNATE = static_cast<int32_t>(DeviceStates::POWER_STATE_HIBERNATE);

#define GNSS_WEAR_POWER_HAL_CLIENT_NAME "vendor.qti.gnss"

static GnssWearPowerHandler* sGnssWearPowerHandler = nullptr;

/* PowerServiceDeathRecipient Implementation */
void GnssWearPowerHandler::PowerServiceDeathRecipient::serviceDied(
                                    uint64_t cookie, const wp<IBase>& who) {
   mHandler->handlePowerServiceDeath(cookie, who);
}

void GnssWearPowerHandler::sendPowerEventToLocCtrlApi(PowerStateType intPowerState) {

    if (NULL == mLocationControlApi)
        mLocationControlApi = LocationControlAPI::getInstance();

    if (NULL != mLocationControlApi)
        mLocationControlApi->powerStateEvent(intPowerState);
}

void GnssWearPowerHandler::initializeGnssWearPower() {

    bool retVal = false;
    do {
        retVal = connectToPowerHal();

        if (true != retVal) {
            LOC_LOGw("Could not connect to POWER_HAL");

            // Sleep for POWER_HAL service to come-up before trying again.
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    } while (false == retVal);
}

GnssWearPowerHandler::GnssWearPowerHandler():
        mPowerServiceDeathRecipient(new PowerServiceDeathRecipient(this)),
        mLocationControlApi(LocationControlAPI::getInstance()) {

    sGnssWearPowerHandler = this;

    std::thread subscriptionThread ( [this]() {
                                        initializeGnssWearPower();
                                    });
    subscriptionThread.detach();
}

bool GnssWearPowerHandler::subscribeToPowerHal() {

    bool retVal = true;

    sp<IPowerStateService> powerHalService;
    {
        Mutex::Autolock lock(mMutex);
        powerHalService = mPowerHalService;
    }

    auto resDeepSleep = powerHalService->registerDeepSleepClient(
        sGnssWearPowerHandler, GNSS_WEAR_POWER_HAL_CLIENT_NAME);
    if (resDeepSleep.isOk() && resDeepSleep) {
        LOC_LOGi("Registered for Deep Sleep events.");
    } else {
        LOC_LOGe("Failed to register for Deep Sleep events.");
        retVal = false;
    }

    auto resHibernate = powerHalService->registerHibernateClient(
        sGnssWearPowerHandler, GNSS_WEAR_POWER_HAL_CLIENT_NAME);
    if (resHibernate.isOk() && resHibernate) {
        LOC_LOGi("Registered for Hibernate events.");
    } else {
        LOC_LOGe("Failed to register for Hibernate events.");
        retVal = false;
    }

    return retVal;
}

bool GnssWearPowerHandler::connectToPowerHal(void) {

    {
        Mutex::Autolock lock(mMutex);
        if (mPowerHalService.get() != nullptr) {
            return true;
        }
    }

    // Get a vehicle HAL instance.
    sp<IPowerStateService> powerHalService = IPowerStateService::tryGetService();
    if (powerHalService.get() == nullptr) {
        LOC_LOGe("Unable to connect to POWER_HAL Service.");
        return false;
    }

    auto ret = powerHalService->linkToDeath(mPowerServiceDeathRecipient, /*cookie=*/ 0);
    if (!ret.isOk() || ret == false) {
        LOC_LOGw("Failed to link to POWER_HAL death");
        return false;
    }
    {
        Mutex::Autolock lock(mMutex);
        mPowerHalService = powerHalService;
    }
    LOC_LOGi("Connected to POWER_HAL");
    return subscribeToPowerHal();
}

void GnssWearPowerHandler::handleGnssWearPowerEvent(int32_t powerEvent) {
    switch (powerEvent) {
        case POWER_EVENT_DEEP_SLEEP_ENTRY:
            LOC_LOGi("Suspend GNSS sessions.");
            sendPowerEventToLocCtrlApi(POWER_STATE_SUSPEND);
            break;
        case POWER_EVENT_DEEP_SLEEP_EXIT:
            LOC_LOGi("Resume GNSS Sessions.");
            sendPowerEventToLocCtrlApi(POWER_STATE_RESUME);
            break;
        default:
            break;
    }
}

void GnssWearPowerHandler::handlePowerServiceDeath(uint64_t cookie, const wp<IBase>& who) {

    LOC_LOGe("POWER_HAL service died. cookie: %llu, who: %p",
                static_cast<unsigned long long>(cookie), &who);

    Mutex::Autolock lock(mMutex);
    if (mPowerHalService.get() != nullptr) {
        mPowerHalService->unlinkToDeath(mPowerServiceDeathRecipient);
        mPowerHalService = nullptr;
    }
}

/* Overrides */

Return<bool> GnssWearPowerHandler::notifyDeepSleepEvent(state parameter) {

    LOC_LOGi("Power Service Deep Sleep Event. Parameter: %d", parameter);

    int32_t powerEvent = 0;
    switch (parameter) {
        case state::DEEP_SLEEP_ENTER:
            powerEvent = POWER_EVENT_DEEP_SLEEP_ENTRY;
            break;
        case state::DEEP_SLEEP_EXIT:
            powerEvent = POWER_EVENT_DEEP_SLEEP_EXIT;
            break;
        default:
            LOC_LOGe("Invalid event.");
    }
    handleGnssWearPowerEvent(powerEvent);
    return true;
}

Return<bool> GnssWearPowerHandler::notifyHibernateEvent(state parameter) {

    LOC_LOGi("Power Service Hibernate Event. Parameter: %d", parameter);

    // Same handling for hibernate as for deep sleep.

    int32_t powerEvent = 0;
    switch (parameter) {
        case state::HIBERNATE_ENTER:
            powerEvent = POWER_EVENT_DEEP_SLEEP_ENTRY;
            break;
        case state::HIBERNATE_EXIT:
            powerEvent = POWER_EVENT_DEEP_SLEEP_EXIT;
            break;
        default:
            LOC_LOGe("Invalid event.");
    }
    handleGnssWearPowerEvent(powerEvent);
    return true;
}

GnssWearPowerHandler* GnssWearPowerHandler::getGnssWearPowerHandler() {

    if (nullptr == sGnssWearPowerHandler) {
        sGnssWearPowerHandler = new GnssWearPowerHandler();
    }
    return sGnssWearPowerHandler;
}

extern "C" void initGnssWearPowerHandler(void){

    GnssWearPowerHandler::getGnssWearPowerHandler();
}
