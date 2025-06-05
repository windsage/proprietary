/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#define LOG_TAG "LocSvc_AIDL_Geocoder"
#define LOG_NDEBUG 0

#include "LocAidlGeocoder.h"
#include <gps_extended_c.h>
#include <log_util.h>
#include <dlfcn.h>
#include "loc_misc_utils.h"

namespace aidl {
namespace vendor {
namespace qti {
namespace gnss {
namespace implementation {

typedef const GnssInterface* (getLocationInterface)();

const GnssInterface* LocAidlGeocoder::getGnssInterface() {

    ENTRY_LOG();

    if (nullptr == mGnssInterface && !mGetGnssInterfaceFailed) {
        LOC_LOGd("loading libgnss.so::getGnssInterface ...");
        void * handle = nullptr;
        getLocationInterface* getter =
                (getLocationInterface*)dlGetSymFromLib(handle, "libgnss.so", "getGnssInterface");

        if (NULL == getter) {
            mGetGnssInterfaceFailed = true;
        } else {
            mGnssInterface = (GnssInterface*)(*getter)();
        }
    }
    return mGnssInterface;
}

::ndk::ScopedAStatus LocAidlGeocoder::setCallback(
        const std::shared_ptr<ILocAidlGeocoderCallback>& callback) {
    FROM_AIDL_CLIENT();

    mCallbackIface = callback;

    const GnssInterface* gnssInterface = getGnssInterface();
    if (nullptr == gnssInterface) {
        LOC_LOGE("Failed to get GNSS Interface");
        return ndk::ScopedAStatus::ok();
    }
    gnssInterface->setAddressRequestCb([cbIface = mCallbackIface](const Location& loc){
                if (cbIface != nullptr) {
                    LOC_LOGD("getAddrFromLocationCb");
                    LocAidlLocation aidlLoc;
                    LocAidlUtils::locationToLocAidlLocation(loc, aidlLoc);
                    cbIface->getAddrFromLocationCb(aidlLoc);
                }
            });
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlGeocoder::injectLocationAndAddr(const LocAidlLocation& loc,
            const LocAidlAddress& addr) {
    FROM_AIDL_CLIENT();
    const GnssInterface* gnssInterface = getGnssInterface();
    if (nullptr == gnssInterface) {
        LOC_LOGE("Failed to get GNSS Interface");
        return ndk::ScopedAStatus::ok();
    }
    Location location;
    GnssCivicAddress civicAddress;
    LocAidlUtils::LocAidlLocationToLocation(loc, location);
    LocAidlUtils::LocAidlAddressToCivicAddress(addr, civicAddress);
    gnssInterface->injectLocationAndAddr(location, civicAddress);
    return ndk::ScopedAStatus::ok();
}

}
}
}
}
}
