/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <aidl/vendor/qti/gnss/BnLocAidlGeocoder.h>
#include <aidl/vendor/qti/gnss/LocAidlAddress.h>
#include <aidl/vendor/qti/gnss/LocAidlLocation.h>
#include "LocAidlUtils.h"
#include <location_interface.h>

namespace aidl {
namespace vendor {
namespace qti {
namespace gnss {
namespace implementation {
using ::aidl::vendor::qti::gnss::ILocAidlGeocoder;
using ::aidl::vendor::qti::gnss::ILocAidlGeocoderCallback;
using ::aidl::vendor::qti::gnss::LocAidlAddress;
using ::aidl::vendor::qti::gnss::LocAidlLocation;

struct LocAidlGeocoder : public BnLocAidlGeocoder {
    LocAidlGeocoder() = default;
    virtual ~LocAidlGeocoder() = default;
    ::ndk::ScopedAStatus setCallback(
            const std::shared_ptr<ILocAidlGeocoderCallback>& callback) override;
    ::ndk::ScopedAStatus injectLocationAndAddr(const LocAidlLocation& loc,
            const LocAidlAddress& addr) override;

private:
    const GnssInterface* getGnssInterface();

    std::shared_ptr<ILocAidlGeocoderCallback> mCallbackIface = nullptr;
    GnssInterface* mGnssInterface = nullptr;
    bool mGetGnssInterfaceFailed = false;
};

}
}
}
}
}
