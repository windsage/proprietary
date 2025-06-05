/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * All rights reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#define LOG_TAG "LocSvc_AIDL_IzatConfig"
#define LOG_NDEBUG 0

#include <log_util.h>
#include "loc_cfg.h"
#include <gps_extended_c.h>
#include <LocationAPI.h>
#include <izat_remote_api.h>
#include <IzatRemoteApi.h>
#include <IzatTypes.h>
#include "izat_wifi_db_provider.h"
#include "IzatWiFiDBProvider.h"
#include "LocAidlIzatConfig.h"
#include <stdio.h>

using namespace izat_remote_api;
using namespace izat_manager;

namespace aidl {
namespace vendor {
namespace qti {
namespace gnss {
namespace implementation {


LocAidlIzatConfig::LocAidlIzatConfig() : mCallbackIface(nullptr) {

    ENTRY_LOG();
}

LocAidlIzatConfig::~LocAidlIzatConfig() {

    ENTRY_LOG();
}

// Methods from ILocAidlIzatConfig follow.
std::shared_ptr<LocAidlDeathRecipient> LocAidlIzatConfig::mDeathRecipient = nullptr;

void IzatConfigServiceDied(void* cookie) {
    LOC_LOGe("ILocAidlIzatConfigCallback died.");
    auto thiz = static_cast<LocAidlIzatConfig*>(cookie);
    if (nullptr != thiz) {
        thiz->handleAidlClientSsr();
        thiz = nullptr;
    }
}

void LocAidlIzatConfig::handleAidlClientSsr() {
    if (mCallbackIface != nullptr && mDeathRecipient != nullptr) {
        mDeathRecipient->unregisterFromPeer(mCallbackIface->asBinder().get(), this);
    }
    mDeathRecipient = nullptr;
    mCallbackIface = nullptr;
}

::ndk::ScopedAStatus LocAidlIzatConfig::init(
        const std::shared_ptr<ILocAidlIzatConfigCallback>& callback, bool* _aidl_return) {

    FROM_AIDL_CLIENT();

    // Register death recipient
    if (mDeathRecipient == nullptr) {
        mDeathRecipient = make_shared<LocAidlDeathRecipient>(IzatConfigServiceDied);
    } else if (mCallbackIface != nullptr) {
        mDeathRecipient->unregisterFromPeer(mCallbackIface->asBinder().get(), this);
    }
    if (callback != nullptr) {
        mDeathRecipient->registerToPeer(callback->asBinder().get(), this);
    } else {
        LOC_LOGe("callback is nullptr!");
    }
    mCallbackIface = callback;
    *_aidl_return = true;
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlIzatConfig::readConfig(bool* _aidl_return) {

    FROM_AIDL_CLIENT();

    // Sanity check
    if (mCallbackIface == nullptr) {
        LOC_LOGE("mCallbackIface NULL");
        *_aidl_return = false;
        return ndk::ScopedAStatus::ok();
    }

    // Read izat.conf file
    string izatConfContent;
    bool success = readIzatConf(izatConfContent);
    if (!success) {
        LOC_LOGE("Failed to read izat.conf");
        *_aidl_return = false;
        return ndk::ScopedAStatus::ok();
    }

    // Invoke callback
    TO_AIDL_CLIENT();
    mCallbackIface->izatConfigCallback(izatConfContent);

    *_aidl_return = true;
    return ndk::ScopedAStatus::ok();
}

bool LocAidlIzatConfig::readIzatConf(std::string& izatConfContent) {

    std::FILE *fp = fopen(LOC_PATH_IZAT_CONF, "r");

    if (NULL != fp) {
        std::fseek(fp, 0, SEEK_END);
        izatConfContent.resize(std::ftell(fp));
        std::rewind(fp);
        std::fread(&izatConfContent[0], 1, izatConfContent.size(), fp);
        std::fclose(fp);
        return true;
    } else {
        LOC_LOGE("Failed to open izat.conf");
    }

    return false;
}


}  // namespace implementation
}  // namespace gnss
}  // namespace qti
}  // namespace vendor
}  // namespace aidl
