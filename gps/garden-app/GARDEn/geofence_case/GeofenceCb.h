/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017, 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef GEOFENCECB_H
#define GEOFENCECB_H
#include "GeofenceCase.h"

namespace garden {

class GeofenceCb : public GnssCbBase {
public:
    GeofenceCb();
    ~GeofenceCb() {}

    void geofenceBreachCb(uint32_t id, Location location,
            GeofenceBreachType transition, uint64_t timeStamp) override;
    void geofenceStatusCb(GeofenceStatusAvailable status) override;
    void addGeofencesCb(uint32_t id, LocationError error) override;
    void removeGeofencesCb(uint32_t id, LocationError error) override;
    void pauseGeofencesCb(uint32_t id, LocationError error) override;
    void resumeGeofencesCb(uint32_t id, LocationError error) override;
    void modifyGeofencesCb(uint32_t id, LocationError error) override;
};


} //namespace garden


#endif //GEOFENCECB_H
