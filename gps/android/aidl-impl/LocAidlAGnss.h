/*
 * Copyright (c) 2021-2022, 2024 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef VENDOR_QTI_GNSS_LOCAIDLAGNSS_H
#define VENDOR_QTI_GNSS_LOCAIDLAGNSS_H

#include "LocAidlUtils.h"

#include <aidl/vendor/qti/gnss/BnLocAidlAGnss.h>
#include <gps_extended_c.h>
#include <location_interface.h>
#include <IIzatManager.h>

namespace aidl {
namespace vendor {
namespace qti {
namespace gnss {
namespace implementation {

using ::aidl::vendor::qti::gnss::ILocAidlAGnssCallback;
using ::aidl::vendor::qti::gnss::LocAidlAGnssType;
using ::aidl::vendor::qti::gnss::LocAidlApnType;
using ::aidl::vendor::qti::gnss::LocAidlAGnssStatusIpV4;
using ::aidl::vendor::qti::gnss::LocAidlAGnssType;
using ::aidl::vendor::qti::gnss::LocAidlApnTypeMask;
using ::aidl::vendor::qti::gnss::LocAidlAGnssStatusValue;

class NetworkConnListener;
struct LocAidlAGnss : public BnLocAidlAGnss {
    LocAidlAGnss();
    virtual ~LocAidlAGnss() = default;

    // Methods from ::vendor::qti::gnss::ILocAidlAGnss follow.
    ::ndk::ScopedAStatus dataConnOpenExt(const string& apn,
            LocAidlApnType apnIpType, LocAidlAGnssType agnssType, bool* _aidl_return) override;
    ::ndk::ScopedAStatus dataConnClosedExt(LocAidlAGnssType agnssType, bool* _aidl_return) override;
    ::ndk::ScopedAStatus dataConnFailedExt(LocAidlAGnssType agnssType, bool* _aidl_return) override;
    // Method to fetch GNSS Interface
    const GnssInterface* getGnssInterface();

    /* Data call setup callback passed down to GNSS HAL implementation */
    static void locAidlAgnssStatusIpV4Cb(AGnssExtStatusIpV4 status);

    // Methods from ::vendor::qti::gnss::ILocAidlAGnss follow.
    ::ndk::ScopedAStatus setCallbackExt(
            const std::shared_ptr<ILocAidlAGnssCallback>& callback) override;

    // Methods from ::android::hidl::base::IBase follow.

private:
    const GnssInterface* mGnssInterface = nullptr;
    static std::shared_ptr<LocAidlDeathRecipient> mDeathRecipient;
    static std::shared_ptr<ILocAidlAGnssCallback> sLocAidlAGnssCbIface;

    IIzatManager* mIzatManager;
    NetworkConnListener* mListener;
};

}  // namespace implementation
}  // namespace aidl
}  // namespace gnss
}  // namespace qti
}  // namespace vendor
#endif  // VENDOR_QTI_GNSS__LOCHIDLAGNSS_H
