/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef VENDOR_QTI_GNSS_LOCHIDLFLPCLIENT_H
#define VENDOR_QTI_GNSS_LOCHIDLFLPCLIENT_H

#include <aidl/vendor/qti/gnss/BnLocAidlFlpService.h>
#include <aidl/vendor/qti/gnss/BnLocAidlFlpServiceCallback.h>
#include <LocationAPI.h>
#include <location_interface.h>
#include <LocationAPIClientBase.h>
#include "LocAidlUtils.h"

namespace aidl {
namespace vendor {
namespace qti {
namespace gnss {
namespace implementation {

class LocAidlFlpClient : public LocationAPIClientBase
{
public:
    static LocAidlFlpClient* getInstance();
    void setCallback(
            const std::shared_ptr<::aidl::vendor::qti::gnss::ILocAidlFlpServiceCallback>& callback);
    virtual ~LocAidlFlpClient() = default;

    void onCapabilitiesCb(LocationCapabilitiesMask capabilitiesMask) final;
    void onEngineLocationsInfoCb(uint32_t count,
            GnssLocationInfoNotification* engineLocationInfoNotification);
    void onTrackingCb(const Location& location) final;
    void onBatchingCb(size_t count, Location* locations,
            const BatchingOptions& BatchingOptions) final;
    void onBatchingStatusCb(const BatchingStatusInfo& batchingStatus,
            std::list<uint32_t> &listOfCompletedTrips) final;

    uint32_t locAPIGnssDeleteAidingData(GnssAidingData& data);
    uint32_t updateXtraThrottle(const bool enabled);

private:
    LocAidlFlpClient();
    const GnssInterface* getGnssInterface();

public:
    LocationCapabilitiesMask mCapabilitiesMask;

private:
    std::shared_ptr<::aidl::vendor::qti::gnss::ILocAidlFlpServiceCallback> mGnssCbIface;
    const GnssInterface* mGnssInterface;
    static LocAidlFlpClient* sFlpClient;
};

}  // namespace implementation
}  // namespace AIDL
}  // namespace gnss
}  // namespace qti
}  // namespace vendor
#endif
