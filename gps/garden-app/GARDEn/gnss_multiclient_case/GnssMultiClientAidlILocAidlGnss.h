/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2019-2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/
#ifndef GNSS_MULTI_CLIENT_AIDL_ILOCAIDLGNSS_H
#define GNSS_MULTI_CLIENT_AIDL_ILOCAIDLGNSS_H

#include <GnssMultiClientAidlILocAidlGnssCb.h>
#include "IGnssAPI.h"
#include <LocationAPI.h>
#include <vector>
#include "DataItemId.h"
#include "IGardenCase.h"

using ::ndk::SharedRefBase;
using android::OK;
using android::sp;
using android::wp;
using android::status_t;
using ::aidl::vendor::qti::gnss::ILocAidlGnssConfigService;
using ::aidl::vendor::qti::gnss::ILocAidlGnss;
using ::aidl::vendor::qti::gnss::ILocAidlIzatConfig;
using ::aidl::vendor::qti::gnss::ILocAidlIzatSubscription;
namespace garden {

#define GNSS_VENDOR_SERVICE_INSTANCE "gnss_vendor"


/* GNSS MULTI CLIENT AIDL CLIENT FOR IGNSS INTERFACE */
class GnssMultiClientAidlILocAidlGnss {

    friend class GnssMultiClientAidlILocAidlGnssCb;

private:
    static GnssMultiClientAidlILocAidlGnss* sInstance;
    GnssMultiClientAidlILocAidlGnss() {
        mLocAidlGnssCb.reset(::new GnssMultiClientAidlILocAidlGnssCb(this));
    };

public:
    static GnssMultiClientAidlILocAidlGnss& get() {
        if (nullptr == sInstance) {
            sInstance = new GnssMultiClientAidlILocAidlGnss();
        }
        return *sInstance;
    }

public:
    GARDEN_RESULT menuTest();
private:
    /* utils */
    template <class T>
    GARDEN_RESULT checkResultOk(T& Result, const std::string printMsg);

    template <class T>
    ::ndk::ScopedAStatus ILocAidlExtinit(T& ext);

    template <class T>
    void convertGnssTypeMaskToConstellationVec(GnssSvTypesMask enableMask, GnssSvTypesMask
        disableMask, std::vector<T>& disableVec);

    /* Individual test cases triggered from menu test */
    GARDEN_RESULT ILocAidlIzatConfig_init();
    GARDEN_RESULT ILocAidlIzatConfig_readConfig();
    GARDEN_RESULT ILocAidlIzatSubscription_init();
    GARDEN_RESULT ILocAidlIzatSubscription_boolDataItemUpdate(int result);
    GARDEN_RESULT ILocAidlGnssConfigService_init();
    GARDEN_RESULT ILocAidlGnssConfigService_reset();
    GARDEN_RESULT ILocAidlGnssConfigService_setGnssSvTypeConfig(GnssSvTypesMask enableMask,
        GnssSvTypesMask disableMask);
    GARDEN_RESULT ILocAidlGnssConfigService_getGnssSvTypeConfig();
    void initVendorHal();

public:
    GARDEN_RESULT createAidlClient();
    std::shared_ptr<ILocAidlGnss> mLocAidlGnssIface;
    std::shared_ptr<ILocAidlIzatConfig> mLocAidlGnssExtensionIzatConfig;
    std::shared_ptr<ILocAidlIzatSubscription> mLocAidlIzatSubscription;
    std::shared_ptr<ILocAidlGnssConfigService> mLocAidlGnssConfigService;
    std::shared_ptr<GnssMultiClientAidlILocAidlGnssCb> mLocAidlGnssCb;
};

} //namespace garden


#endif //GNSS_MULTI_CLIENT_AIDL_ILOCAIDLGNSS_H
