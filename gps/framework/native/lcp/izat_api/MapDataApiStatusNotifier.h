/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef MAP_DATA_NOTIFIER_H
#define MAP_DATA_NOTIFIER_H

#include <vector>
#include "IzatRemoteApi.h"
#include "gps_extended.h"
#include "MapDataApi.h"

#define MCC_CHINA1      "460"
#define MCC_CHINA2      "461"

using namespace std;

namespace izat_remote_api {

class MapDataApiStatusNotifier : public IzatNotifier {
protected:
    MapDataApiStatusNotifier();
    virtual inline ~MapDataApiStatusNotifier() { }

public:
    static const char sName[];

    virtual void handleMsg(qc_loc_fw::InPostcard *const in_card) final;
    virtual void onLocationChange(UlpLocation& ulpLocation, GpsLocationExtended& locExt) = 0;
    virtual void onTrackingStateChange(bool trackingState) = 0;
protected:
    int subOrUnsubOSAgentUpdate(bool unsubscribe = false);
    int subOrUnsubLocationUpdate(bool unsubscribe = false);
    int sendMapData(const string maString);
    TriStatus isInChina;

private:
    void handleLocationChange(qc_loc_fw::InPostcard * const in_msg);
    void handleMccMncChange(qc_loc_fw::InPostcard * const in_msg);
};

} //izat_remote_api

#endif // #ifndef MAP_DATA_NOTIFIER_H
