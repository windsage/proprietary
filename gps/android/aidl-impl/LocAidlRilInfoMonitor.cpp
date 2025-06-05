/*
 * Copyright (c) 2021-2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#define LOG_TAG "LocSvc_AIDL_RilInfoMonitor"
#define LOG_NDEBUG 0

#include "LocAidlRilInfoMonitor.h"
#include "LocAidlUtils.h"

namespace aidl {
namespace vendor {
namespace qti {
namespace gnss {
namespace implementation {

// Methods from ::vendor::qti::gnss::IGnssRilInfoMonitor follow.
::ndk::ScopedAStatus LocAidlRilInfoMonitor::init() {
    FROM_AIDL_CLIENT();

    if (NULL == mLBSAdapter) {
        mLBSAdapter = LBSAdapter::get(0);
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlRilInfoMonitor::cinfoInject(int32_t cid, int32_t lac,
        int32_t mnc, int32_t mcc, bool roaming) {

    FROM_AIDL_CLIENT();

    if (mLBSAdapter != NULL) {
        mLBSAdapter->cinfoInject(cid, lac, mnc, mcc, roaming);
    } else {
        LOC_LOGE("mLBSAdapter NULL");
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlRilInfoMonitor::oosInform() {
    FROM_AIDL_CLIENT();

    if (mLBSAdapter) {
        mLBSAdapter->oosInform();
    } else {
        LOC_LOGE("mLBSAdapter NULL");
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlRilInfoMonitor::niSuplInit(const string& str) {
    FROM_AIDL_CLIENT();

    if (mLBSAdapter) {
        mLBSAdapter->niSuplInit(str.c_str(), str.size());
    } else {
        LOC_LOGE("mLBSAdapter NULL");
    }

    return ndk::ScopedAStatus::ok();
}

// deprecated
::ndk::ScopedAStatus LocAidlRilInfoMonitor::chargerStatusInject(int32_t status) {

    FROM_AIDL_CLIENT();

    return ndk::ScopedAStatus::ok();
}

}  // namespace implementation
}  // namespace aidl
}  // namespace gnss
}  // namespace qti
}  // namespace vendor
