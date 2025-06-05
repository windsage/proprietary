/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2019-2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/
#ifndef GNSS_MULTI_CLIENT_AIDL_ILOCAIDLGNSS_CB_H
#define GNSS_MULTI_CLIENT_AIDL_ILOCAIDLGNSS_CB_H
#define NAVIC 7

#include <aidl/vendor/qti/gnss/BnLocAidlGnss.h>
#include <aidl/vendor/qti/gnss/BnLocAidlIzatConfigCallback.h>
#include <aidl/vendor/qti/gnss/BnLocAidlGnssConfigServiceCallback.h>
#include <aidl/vendor/qti/gnss/BnLocAidlIzatConfig.h>
#include <aidl/vendor/qti/gnss/BnLocAidlIzatSubscriptionCallback.h>
#include "GnssMultiClientCase.h"
#include "GnssCbBase.h"
#include <stdint.h>

/*using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::hardware::hidl_vec;
*/
using ::aidl::vendor::qti::gnss::BnLocAidlIzatConfigCallback;
using ::aidl::vendor::qti::gnss::BnLocAidlIzatSubscriptionCallback;
using ::aidl::vendor::qti::gnss::BnLocAidlGnssConfigServiceCallback;
using ::aidl::vendor::qti::gnss::LocAidlSubscriptionDataItemId;
using ::aidl::vendor::qti::gnss::LocAidlBoolDataItem;
using LocAidlGnssConstellationType = ::aidl::vendor::qti::gnss::LocAidlGnssConstellationType;
using LocAidlRobustLocationInfo = ::aidl::vendor::qti::gnss::LocAidlRobustLocationInfo;

namespace garden {

class GnssMultiClientAidlILocAidlGnss;

class GnssMultiClientAidlILocAidlGnssCb :
        public BnLocAidlIzatConfigCallback,
        public BnLocAidlIzatSubscriptionCallback,
        public BnLocAidlGnssConfigServiceCallback
{
public:
    GnssMultiClientAidlILocAidlGnssCb() {}
    GnssMultiClientAidlILocAidlGnssCb(GnssMultiClientAidlILocAidlGnss* aidlILocAidlGnss);
    ~GnssMultiClientAidlILocAidlGnssCb();

    //IzatConfigCb
    ::ndk::ScopedAStatus izatConfigCallback(const std::string& izatConfigContent) override;

    //Izatsubscriptioncb
    ::ndk::ScopedAStatus requestData(
            const std::vector<LocAidlSubscriptionDataItemId>& list) override {
        return ndk::ScopedAStatus::ok();
    }
    ::ndk::ScopedAStatus updateSubscribe(const std::vector<LocAidlSubscriptionDataItemId>& list,
        bool subscribe) override {
        return ndk::ScopedAStatus::ok();
    }
    ::ndk::ScopedAStatus unsubscribeAll() override {return ndk::ScopedAStatus::ok();}
    ::ndk::ScopedAStatus turnOnModule(LocAidlSubscriptionDataItemId di, int32_t timeout) override {
        return ndk::ScopedAStatus::ok();
    }
    ::ndk::ScopedAStatus turnOffModule(LocAidlSubscriptionDataItemId di) override {
        return ndk::ScopedAStatus::ok();
    }

    ::ndk::ScopedAStatus boolDataItemUpdate(const std::vector<LocAidlBoolDataItem>& diVec)
            override {
        return ndk::ScopedAStatus::ok();
    }
    //ILocAidlGnssConfigServiceCb
    template <class T>
    static void convertGnssTypeMaskToConstellationVec(GnssSvTypesMask enableMask, GnssSvTypesMask
        disableMask, std::vector<T>& disableVec);
    template <class T>
    static int convertConstellationToint(T in) {
        switch (in) {
            case T::GPS:
                return 1;
            case T::SBAS:
                return 2;
            case T::GLONASS:
                return 3;
            case T::QZSS:
                return 4;
            case T::BEIDOU:
                return 5;
            case T::GALILEO:
                return 6;
            default:
                return 0;
        }
    }
    ::ndk::ScopedAStatus getGnssSvTypeConfigCb(
        const std::vector<LocAidlGnssConstellationType>& disabledSvTypeList) override ;
    ::ndk::ScopedAStatus getRobustLocationConfigCb(const LocAidlRobustLocationInfo& info) override {
        return ndk::ScopedAStatus::ok();
    }
private:
    GnssMultiClientAidlILocAidlGnss* mLocAidlGnss = nullptr;
};

} //namespace garden

#endif //GNSS_MULTI_CLIENT_AIDL_ILOCAIDLGNSS_CB_H
