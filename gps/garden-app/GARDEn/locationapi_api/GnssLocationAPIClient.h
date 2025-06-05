/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017-2020, 2022,2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef GNSSLOCATIONAPICLIENT_H
#define GNSSLOCATIONAPICLIENT_H

#include <LocationAPIClientBase.h>
#include <loc_gps.h>
#include <location_interface.h>
#include <LocationAPI.h>
#include "GnssCbBase.h"

namespace garden {

class GnssLocationAPIClient: public LocationAPIClientBase {
public:
    GnssLocationAPIClient(LocationAPIControlClient* ControlClient);
    int setCallbacks(GnssCbBase* callbacks);

    int gnssStart(void);
    int gnssInjectLocation(double latitude, double longitude, float accuracy);
    void gnssDeleteAidingData(LocGpsAidingData flags);
    int gnssSetPositionMode(LocGpsPositionMode mode, LocGpsPositionRecurrence recurrence,
            uint32_t min_interval, uint32_t preferred_accuracy, uint32_t preferred_time);

    void configurationUpdate(const char* config_data, int32_t length);
    void updateNetworkAvailability(int available, const char* apn);

    //callbacks
    void onTrackingCb(const Location& location) final;
    void onStartTrackingCb(LocationError error) final;
    void onStopTrackingCb(LocationError error) final;
    void onGnssSvCb(const GnssSvNotification& gnssSvNotification) final;
    void onGnssNmeaCb(const GnssNmeaNotification& gnssNmeaNotification) final;
    void onGnssMeasurementsCb(
            const GnssMeasurementsNotification &gnssMeasurementsNotification) final;
    void onGnssDataCb(const GnssDataNotification& gnssDataNotification) final;
    void onCapabilitiesCb(LocationCapabilitiesMask capabilitiesMask) final;
    void onGnssNiCb(uint32_t id, const GnssNiNotification& gnssNiNotification) final;
    void onGnssLocationInfoCb(const GnssLocationInfoNotification& gnssLocInfoNotif) final;

    LocationAPIControlClient* mControlClient;
private:
    ~GnssLocationAPIClient();
    GnssInterface* getGnssInterface();
private:
    mutex mGnssLock;
    TrackingOptions mLocationOptions;
    GnssInterface* mGnssInterface;
    LocationCapabilitiesMask mLocationCapabilitiesMask;
    GnssCbBase* mGnssCallbacks;
};

} // namespace garden

#endif //GNSSLOCATIONAPICLIENT_H

