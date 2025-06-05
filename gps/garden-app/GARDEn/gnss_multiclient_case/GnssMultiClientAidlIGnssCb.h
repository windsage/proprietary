/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2021-2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/
#ifndef GNSS_MULTI_CLIENT_AIDL_IGNSS_CB_H
#define GNSS_MULTI_CLIENT_AIDL_IGNSS_CB_H
#include "GnssMultiClientCase.h"
#include "GnssCbBase.h"

#include <aidl/android/hardware/gnss/BnGnss.h>
#include <aidl/android/hardware/gnss/BnGnssPowerIndicationCallback.h>
#include <aidl/android/hardware/gnss/BnGnssCallback.h>
#include <aidl/android/hardware/gnss/BnGnssMeasurementCallback.h>
#include <aidl/android/hardware/gnss/BnAGnss.h>
#include <aidl/android/hardware/gnss/BnAGnssCallback.h>
#include <aidl/android/hardware/gnss/BnGnssBatching.h>
#include <aidl/android/hardware/gnss/BnGnssBatchingCallback.h>
#include <aidl/android/hardware/gnss/BnGnssAntennaInfo.h>
#include <aidl/android/hardware/gnss/BnGnssAntennaInfoCallback.h>
#include <aidl/android/hardware/gnss/visibility_control/BnGnssVisibilityControlCallback.h>
#include <aidl/android/hardware/gnss/measurement_corrections/BnMeasurementCorrectionsCallback.h>
using ::aidl::android::hardware::gnss::visibility_control::BnGnssVisibilityControlCallback;
using ::aidl::android::hardware::gnss::measurement_corrections::BnMeasurementCorrectionsCallback;
using ::aidl::android::hardware::gnss::BnAGnssCallback;
using ::aidl::android::hardware::gnss::BnGnssBatchingCallback;
using ::aidl::android::hardware::gnss::BnGnssAntennaInfoCallback;

using ::aidl::android::hardware::gnss::GnssLocation;
using GnssDataPowerStatsAidl = aidl::android::hardware::gnss::GnssPowerStats;
using IGnssAidl = aidl::android::hardware::gnss::IGnss;
using GnssDataAidl = aidl::android::hardware::gnss::GnssData;

using BnGnssCallbackAidl = aidl::android::hardware::gnss::BnGnssCallback;
using BnGnssMeasurementCallbackAidl = aidl::android::hardware::gnss::BnGnssMeasurementCallback;
using BnGnssPowerIndicationCallbackAidl =
        aidl::android::hardware::gnss::BnGnssPowerIndicationCallback;

using ::aidl::android::hardware::gnss::GnssSignalType;

namespace garden {

class GnssMultiClientAidlIGnssCb : public BnGnssCallbackAidl
{
public:
    GnssMultiClientAidlIGnssCb();
    ~GnssMultiClientAidlIGnssCb();

  ::ndk::ScopedAStatus gnssSetCapabilitiesCb(int capabilities) override;
  ::ndk::ScopedAStatus gnssStatusCb(
        ::aidl::android::hardware::gnss::IGnssCallback::GnssStatusValue in_status)
                { return ndk::ScopedAStatus::ok(); }
  ::ndk::ScopedAStatus gnssSvStatusCb(
        const std::vector<
                ::aidl::android::hardware::gnss::IGnssCallback::GnssSvInfo>& in_svInfoList)
                        { return ndk::ScopedAStatus::ok(); }
  ::ndk::ScopedAStatus gnssLocationCb(
        const ::aidl::android::hardware::gnss::GnssLocation& in_location)
                { return ndk::ScopedAStatus::ok(); }
  ::ndk::ScopedAStatus gnssNmeaCb(int64_t in_timestamp, const std::string& in_nmea)
        { return ndk::ScopedAStatus::ok(); }
  ::ndk::ScopedAStatus gnssAcquireWakelockCb() { return ndk::ScopedAStatus::ok(); }
  ::ndk::ScopedAStatus gnssReleaseWakelockCb() { return ndk::ScopedAStatus::ok(); }
  ::ndk::ScopedAStatus gnssSetSystemInfoCb(
            const ::aidl::android::hardware::gnss::IGnssCallback::GnssSystemInfo& in_info)
                    { return ndk::ScopedAStatus::ok(); }
  ::ndk::ScopedAStatus gnssRequestTimeCb() { return ndk::ScopedAStatus::ok(); }
  ::ndk::ScopedAStatus gnssRequestLocationCb(bool in_independentFromGnss, bool in_isUserEmergency)
        { return ndk::ScopedAStatus::ok(); }

  ::ndk::ScopedAStatus gnssSetSignalTypeCapabilitiesCb(
      const std::vector<GnssSignalType>& in_gnssSignalTypes) override;
};

class GnssMultiClientAidlIGnssMeasCb : public BnGnssMeasurementCallbackAidl {
public:
    GnssMultiClientAidlIGnssMeasCb();
    ~GnssMultiClientAidlIGnssMeasCb();

    ::ndk::ScopedAStatus gnssMeasurementCb(const GnssDataAidl& data) override;
};

class GnssMultiClientAidlIGnssPowerIndCb : public BnGnssPowerIndicationCallbackAidl {
public:
    GnssMultiClientAidlIGnssPowerIndCb();
    ~GnssMultiClientAidlIGnssPowerIndCb();

    ::ndk::ScopedAStatus gnssPowerStatsCb(const GnssDataPowerStatsAidl& gnssPowerStats) override;
    ::ndk::ScopedAStatus setCapabilitiesCb(int32_t in_capabilities) override;
};

class GnssMultiClientAidlIGnssVisibCtrlCb : public BnGnssVisibilityControlCallback {
public:
    GnssMultiClientAidlIGnssVisibCtrlCb() {}
    ~GnssMultiClientAidlIGnssVisibCtrlCb() {}

  ::ndk::ScopedAStatus nfwNotifyCb(
          const IGnssVisibilityControlCallback::NfwNotification& in_notification) override;
  ::ndk::ScopedAStatus isInEmergencySession(bool* _aidl_return) override;
};

class GnssMultiClientAidlIGnssMeasCorrCb : public BnMeasurementCorrectionsCallback {
public:
    GnssMultiClientAidlIGnssMeasCorrCb() {}
    ~GnssMultiClientAidlIGnssMeasCorrCb() {}

  ::ndk::ScopedAStatus setCapabilitiesCb(int32_t in_capabilities) override;
};
class GnssMultiClientAidlIGnssAntennaInfoCb : public BnGnssAntennaInfoCallback {
public:
    GnssMultiClientAidlIGnssAntennaInfoCb() {}
    ~GnssMultiClientAidlIGnssAntennaInfoCb() {}

  ::ndk::ScopedAStatus gnssAntennaInfoCb(const std::vector<
          IGnssAntennaInfoCallback::GnssAntennaInfo>& in_gnssAntennaInfos) override;
};
class GnssMultiClientAidlIAGnssCb : public BnAGnssCallback {
public:
    GnssMultiClientAidlIAGnssCb() {}
    ~GnssMultiClientAidlIAGnssCb() {}

  ::ndk::ScopedAStatus agnssStatusCb(
          IAGnssCallback::AGnssType in_type, IAGnssCallback::AGnssStatusValue in_status) override;
};
class GnssMultiClientAidlIGnssBatchingCb : public BnGnssBatchingCallback {
public:
    GnssMultiClientAidlIGnssBatchingCb() {}
    ~GnssMultiClientAidlIGnssBatchingCb() {}

  ::ndk::ScopedAStatus gnssLocationBatchCb(const std::vector<GnssLocation>& in_locations) override;
};

} //namespace garden

#endif //GNSS_MULTI_CLIENT_AIDL_IGNSS_CB_H
