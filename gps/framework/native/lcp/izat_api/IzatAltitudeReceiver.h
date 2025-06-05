/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  GENERAL DESCRIPTION
  Altitude Receiver Types

  Copyright (c) 2021, 2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef IZAT_MANAGER_ALTITUDERECV_H
#define IZAT_MANAGER_ALTITUDERECV_H

#include <vector>
#include "IzatRemoteApi.h"

namespace izat_remote_api {

class AltitudeReceiverUpdater : public IzatNotifier {
protected:
    inline AltitudeReceiverUpdater() : IzatNotifier(sName, nullptr) {}
    virtual inline ~AltitudeReceiverUpdater() = default;
public:
    static const char sName[];
    struct NlpLocation;
    virtual void handleMsg(qc_loc_fw::InPostcard *const in_card) final;
    void reportZAxisApiConnected();
    void pushAltitude(const NlpLocation location);
    virtual void onAltitudeLookUp(const NlpLocation location, bool isEmergency) = 0;
};

struct AltitudeReceiverUpdater::NlpLocation {
    uint8_t  positionSource;
    uint16_t  locationFlagsMask;
    uint64_t  timestamp;
    double  latitude;
    double  longitude;
    double  altitude;
    float   speed;
    float   bearing;
    float   accuracy;
    float   verticalAccuracy;
    float   speedAccuracy;
    float   bearingAccuracy;
    uint64_t elapsedRealTimeInMs;

    static const uint16_t ALTITUDERECEIVER_NLPLOC_LAT_LONG = 0x01;
    static const uint16_t ALTITUDERECEIVER_NLPLOC_ALTITUDE = 0x02;
    static const uint16_t ALTITUDERECEIVER_NLPLOC_SPEED = 0x04;
    static const uint16_t ALTITUDERECEIVER_NLPLOC_BEARING = 0x08;
    static const uint16_t ALTITUDERECEIVER_NLPLOC_ACCURACY = 0x10;
    static const uint16_t ALTITUDERECEIVER_NLPLOC_VERTICAL_ACCURACY = 0x20;
    static const uint16_t ALTITUDERECEIVER_NLPLOC_SPEED_ACCURACY = 0x40;
    static const uint16_t ALTITUDERECEIVER_NLPLOC_BEARING_ACCURACY = 0x80;
};

} //izat_remote_api

#endif // #ifndef __IZAT_MANAGER_ALTITUDERECV_H__
