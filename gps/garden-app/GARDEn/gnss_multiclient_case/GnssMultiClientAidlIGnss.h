/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2021-2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/
#ifndef GNSS_MULTI_CLIENT_AIDL_IGNSS_H
#define GNSS_MULTI_CLIENT_AIDL_IGNSS_H

#include <LocationAPI.h>
#include <GnssMultiClientAidlIGnssCb.h>

#include <aidl/android/hardware/gnss/BnGnss.h>
#include <aidl/android/hardware/gnss/BnGnssConfiguration.h>
#include <aidl/android/hardware/gnss/BnGnssMeasurementInterface.h>
#include <aidl/android/hardware/gnss/BnGnssPowerIndicationCallback.h>
#include <aidl/android/hardware/gnss/BnGnssPsdsCallback.h>

#include <android/binder_auto_utils.h>
#include <android/binder_ibinder_platform.h>
#include <android/binder_libbinder.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>

#include "IGardenCase.h"

using ::ndk::SharedRefBase;
using ::std::shared_ptr;
using aidl::android::hardware::gnss::BlocklistedSource;
using aidl::android::hardware::gnss::GnssPowerStats;
using aidl::android::hardware::gnss::IGnssPowerIndication;
using aidl::android::hardware::gnss::IGnssPowerIndicationCallback;
using aidl::android::hardware::gnss::PsdsType;
using aidl::android::hardware::gnss::IGnssDebug;
using aidl::android::hardware::gnss::IAGnss;
using aidl::android::hardware::gnss::IAGnssCallback;
using aidl::android::hardware::gnss::IGnss;
using aidl::android::hardware::gnss::IAGnssRil;
using aidl::android::hardware::gnss::IGnssAntennaInfo;
using aidl::android::hardware::gnss::IGnssAntennaInfoCallback;
using aidl::android::hardware::gnss::IGnssBatching;
using aidl::android::hardware::gnss::IGnssBatchingCallback;
using ::aidl::android::hardware::gnss::visibility_control::IGnssVisibilityControl;
using ::aidl::android::hardware::gnss::visibility_control::IGnssVisibilityControlCallback;
using ::aidl::android::hardware::gnss::measurement_corrections::IMeasurementCorrectionsInterface;
using ::aidl::android::hardware::gnss::measurement_corrections::IMeasurementCorrectionsCallback;

using aidl::android::hardware::gnss::ElapsedRealtime;
using aidl::android::hardware::gnss::GnssClock;
using aidl::android::hardware::gnss::GnssData;
using aidl::android::hardware::gnss::GnssMeasurement;
using aidl::android::hardware::gnss::GnssSignalType;
using ::aidl::android::hardware::gnss::GnssLocation;

using IGnssAidl = aidl::android::hardware::gnss::IGnss;
using IGnssMeasurementInterfaceAidl = ::aidl::android::hardware::gnss::IGnssMeasurementInterface;
using IGnssPowerIndicationAidl = aidl::android::hardware::gnss::IGnssPowerIndication;
using IGnssConfigurationAidl = aidl::android::hardware::gnss::IGnssConfiguration;
using PositionModeOptions = aidl::android::hardware::gnss::IGnss::PositionModeOptions;

namespace garden {
/* GNSS MULTI CLIENT AIDL CLIENT FOR IGNSS INTERFACE */
class GnssMultiClientAidlIGnss {
    friend class GnssMultiClientAidlIGnssCb;

private:
    static GnssMultiClientAidlIGnss* sInstance;
    GnssMultiClientAidlIGnss() {
    };
    ~GnssMultiClientAidlIGnss() {
    };

public:
    static GnssMultiClientAidlIGnss& get() {
        if (nullptr == sInstance) {
            sInstance = new GnssMultiClientAidlIGnss();
        }
        return *sInstance;
    }

public:
    GARDEN_RESULT menuTest();

private:
    /* Individual test cases triggered from menu test */
    GARDEN_RESULT createAidlClient();
    GARDEN_RESULT IGnssMeasurement_setCallback();
    GARDEN_RESULT IGnssMeasurement_close();
    GARDEN_RESULT IPowerIndication_setCallback();
    GARDEN_RESULT IPowerIndication_requestGnssPowerStats();
    GARDEN_RESULT IGnss_setCallback();
    GARDEN_RESULT IGnss_close();
    GARDEN_RESULT IGnss_startSvStatus();
    GARDEN_RESULT IGnss_stopSvStatus();
    GARDEN_RESULT IGnss_startNmea();
    GARDEN_RESULT IGnss_stopNmea();
    GARDEN_RESULT IGnssConfiguration_setSuplVersion();
    GARDEN_RESULT IGnssConfiguration_setSuplMode();
    GARDEN_RESULT IGnssConfiguration_setLppProfile();
    GARDEN_RESULT IGnssConfiguration_setGlonassPositioningProtocol();
    GARDEN_RESULT IGnssConfiguration_setEmergencySuplPdn();
    GARDEN_RESULT IGnssConfiguration_setEsExtensionSec();
    GARDEN_RESULT IGnssConfiguration_setBlocklist();

    GARDEN_RESULT IGnss_start();
    GARDEN_RESULT IGnss_stop();
    GARDEN_RESULT IGnss_setPositionMode();
    GARDEN_RESULT IGnss_injectBestLocation();

    GARDEN_RESULT IGnssVisibilityControl_setCallback();
    GARDEN_RESULT IGnssVisibilityControl_controlNfwLocationAccess();

    GARDEN_RESULT IGnssBatching_init();
    GARDEN_RESULT IGnssDebug_getDebugData();
    GARDEN_RESULT IAGnss_setCallback();
    GARDEN_RESULT IAGnss_dataConnOpen();
    GARDEN_RESULT IAGnss_setServer();
    GARDEN_RESULT IAGnss_dataConnClosed();
    GARDEN_RESULT IGnssMeasurementCorrections_setCorrections(const GnssLocation& location);
    GARDEN_RESULT IGnss_setMeasurementCorrectionsCallback();
    GARDEN_RESULT IGnss_setAntennaInfoCallback();
    GARDEN_RESULT IGnss_setAntennaInfoClose();
public:
    shared_ptr<IGnssAidl> mGnssHalAidl;
    shared_ptr<IGnssConfigurationAidl> mGnssConfigIface = nullptr;
    shared_ptr<IGnssMeasurementInterfaceAidl> mGnssMeasurementIface = nullptr;
    shared_ptr<IGnssPowerIndicationAidl> mGnssPowerIndication = nullptr;
    shared_ptr<GnssMultiClientAidlIGnssCb> mAidlIGnssCb = nullptr;
    shared_ptr<GnssMultiClientAidlIGnssMeasCb> mAidlIGnssMeasCb = nullptr;
    shared_ptr<GnssMultiClientAidlIGnssPowerIndCb> mAidlIGnssPowerCb = nullptr;
    bool mMeasurementCorrectionsAllowed;

    shared_ptr<IGnssVisibilityControl> mGnssVisibilityControl = nullptr;
    shared_ptr<IGnssVisibilityControlCallback> mGnssVisibCtrlCb = nullptr;
    shared_ptr<IAGnss> mAGnss = nullptr;
    shared_ptr<IAGnssCallback> mAGnssCb = nullptr;
    shared_ptr<IAGnssRil> mAGnssRil = nullptr;
    shared_ptr<IGnssDebug> mGnssDebug = nullptr;
    shared_ptr<IGnssBatching> mGnssBatching = nullptr;
    shared_ptr<IGnssBatchingCallback> mGnssBatchingCb = nullptr;
    shared_ptr<IMeasurementCorrectionsInterface> mMeasurementCorrections = nullptr;
    shared_ptr<IMeasurementCorrectionsCallback> mGnssMeasCorrCb = nullptr;
    shared_ptr<IGnssAntennaInfo> mGnssAntennaInfo = nullptr;
    shared_ptr<IGnssAntennaInfoCallback> mGnssAntennaInfoCb = nullptr;
};

} //namespace garden

#endif //GNSS_MULTI_CLIENT_AIDL_IGNSS_H
