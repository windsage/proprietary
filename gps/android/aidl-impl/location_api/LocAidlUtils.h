/*
 * Copyright (c) 2021, 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef LOC_AIDL_UTILS
#define LOC_AIDL_UTILS

#include <aidl/vendor/qti/gnss/LocAidlNetworkPositionSourceType.h>
#include <aidl/vendor/qti/gnss/LocAidlIzatHorizontalAccuracy.h>
#include <aidl/vendor/qti/gnss/LocAidlIzatOtherAccuracy.h>
#include <aidl/vendor/qti/gnss/LocAidlLocation.h>
#include <aidl/vendor/qti/gnss/LocAidlAddress.h>
#include <aidl/vendor/qti/gnss/LocAidlIzatLocation.h>
#include <aidl/vendor/qti/gnss/LocAidlIzatStreamType.h>
#include <aidl/vendor/qti/gnss/LocAidlIzatRequest.h>
#include <aidl/vendor/qti/gnss/LocAidlSubscriptionDataItemId.h>
#include <aidl/vendor/qti/gnss/LocAidlIzatProviderStatus.h>
#include <android/binder_auto_utils.h>
#include <LocationAPI.h>
#include <IzatLocation.h>
#include <IzatRequest.h>
#include <DataItemId.h>
#include <log_util.h>

namespace aidl {
namespace vendor {
namespace qti {
namespace gnss {
namespace implementation {

#define TO_AIDL_CLIENT()   LOC_LOGv("[%d] [AS] =>>>> [AC]", __LINE__)
#define FROM_AIDL_CLIENT() LOC_LOGv("[%d] [AS] <<<<= [AC]", __LINE__)

using namespace izat_manager;

struct LocAidlDeathRecipient {
    LocAidlDeathRecipient(AIBinder_DeathRecipient_onBinderDied fun):
            mFun(fun), mPeerDied(false) {
        mDeathRecipient = AIBinder_DeathRecipient_new(mFun);
    }
    LocAidlDeathRecipient() = delete;
    ~LocAidlDeathRecipient() = default;
    void registerToPeer(AIBinder* binder, void* cookie);
    void unregisterFromPeer(AIBinder* binder, void* cookie);
    inline bool peerDied() { return mPeerDied; }
    AIBinder_DeathRecipient_onBinderDied mFun;
    bool mPeerDied;
    AIBinder_DeathRecipient* mDeathRecipient;
};

class LocAidlUtils {

public:
    static void locationToLocAidlLocation(
            const Location& location, LocAidlLocation& gnssLocation);

    static void LocAidlLocationToLocation(
            const LocAidlLocation& location, Location& gnssLocation);

    static void LocAidlAddressToCivicAddress(
            const LocAidlAddress& addr, GnssCivicAddress& gnssAddr);

    static void izatLocationToGnssIzatLocation(
            const IzatLocation& izatLocation,
            LocAidlIzatLocation& gnssIzatLocation);

    inline static LocAidlIzatStreamType izatStreamToGnssIzatStream(
            IzatStreamType ist) {
        return static_cast<LocAidlIzatStreamType>(ist);
    }

    static IzatStreamType gnssIzatStreamToIzatStreamType(
            LocAidlIzatStreamType gist);

    inline static LocAidlIzatProviderStatus izatStatusToGnssIzatStatus(
            izat_manager::IzatProviderStatus ips) {
        return static_cast<LocAidlIzatProviderStatus>(ips);
    }

    static DataItemId translateToDataItemId(LocAidlSubscriptionDataItemId id);

    static void izatRequestToGnssIzatRequest(
            const IzatRequest& ir, LocAidlIzatRequest& gir);

    static LocAidlSubscriptionDataItemId
    translateToSubscriptionDataItemId(DataItemId id);

};

}  // namespace implementation
}  // namespace AIDL
}  // namespace gnss
}  // namespace qti
}  // namespace vendor
#endif
