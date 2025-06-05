/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef LOC_AIDL_GEOFENCE_CLIENT_H
#define LOC_AIDL_GEOFENCE_CLIENT_H

#include <aidl/vendor/qti/gnss/ILocAidlGeofenceService.h>
#include <aidl/vendor/qti/gnss/ILocAidlGeofenceServiceCallback.h>
#include <LocationAPI.h>
#include <LocationAPIClientBase.h>

namespace aidl {
namespace vendor {
namespace qti {
namespace gnss {
namespace implementation {

using ::aidl::vendor::qti::gnss::ILocAidlGeofenceServiceCallback;

class LocAidlGeofenceClient : public LocationAPIClientBase
{
public:
    static LocAidlGeofenceClient* getInstance();
    void setCallback(const std::shared_ptr<ILocAidlGeofenceServiceCallback>& callback);
    virtual ~LocAidlGeofenceClient() = default;

    void onGeofenceBreachCb(const GeofenceBreachNotification& geofenceBreachNotification) final;
    void onGeofenceStatusCb(const GeofenceStatusNotification& geofenceStatusNotification) final;
    void onAddGeofencesCb(size_t count, LocationError* errors, uint32_t* ids) final;
    void onRemoveGeofencesCb(size_t count, LocationError* errors, uint32_t* ids) final;
    void onPauseGeofencesCb(size_t count, LocationError* errors, uint32_t* ids) final;
    void onResumeGeofencesCb(size_t count, LocationError* errors, uint32_t* ids) final;

private:
    LocAidlGeofenceClient();
    std::shared_ptr<ILocAidlGeofenceServiceCallback> mGnssCbIface;
    static LocAidlGeofenceClient* sGeofenceClient;
};

}  // namespace implementation
}  // namespace gnss
}  // namespace qti
}  // namespace vendor
}  // namespace aidl

#endif  // LOC_AIDL_GEOFENCE_CLIENT_H
