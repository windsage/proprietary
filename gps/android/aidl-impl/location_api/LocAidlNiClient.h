/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <aidl/vendor/qti/gnss/ILocAidlGnssNi.h>
#include <aidl/vendor/qti/gnss/ILocAidlGnssNiCallback.h>
#include <LocationAPI.h>
#include <LocationAPIClientBase.h>
#include "LocAidlUtils.h"

namespace aidl {
namespace vendor {
namespace qti {
namespace gnss {
namespace implementation {

using ::aidl::vendor::qti::gnss::ILocAidlGnssNiCallback;
using ::aidl::vendor::qti::gnss::LocAidlGnssUserResponseType;
using ::aidl::vendor::qti::gnss::LocAidlGnssNiType;
using ::aidl::vendor::qti::gnss::LocAidlGnssNiNotification;
using ::aidl::vendor::qti::gnss::LocAidlGnssNiEncodingType;

class LocAidlNiClient : public LocationAPIClientBase
{
public:
    static LocAidlNiClient* getInstance();
    void setCallback(const std::shared_ptr<ILocAidlGnssNiCallback>& callback);
    virtual ~LocAidlNiClient() = default;

    void onCapabilitiesCb(LocationCapabilitiesMask capabilitiesMask) final;
    void onGnssNiCb(uint32_t id, const GnssNiNotification& gnssNiNotification) final;

    void gnssNiRespond(int32_t notifId,
            LocAidlGnssUserResponseType userResponse);

private:
    LocAidlNiClient();
    std::shared_ptr<::aidl::vendor::qti::gnss::ILocAidlGnssNiCallback> mGnssCbIface;
    static LocAidlNiClient* sNiClient;
};

}  // namespace implementation
}  // namespace aidl
}  // namespace gnss
}  // namespace qti
}  // namespace vendor
