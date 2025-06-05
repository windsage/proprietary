/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef GNSS_LE_POWER_HANDLER
#define GNSS_LE_POWER_HANDLER

#include <stdlib.h>
#include <functional>
#include <loc_pla.h>
#include <log_util.h>
#include <LocationAPI.h>
#include <gps_extended_c.h>

#ifdef FEATURE_AUTOMOTIVE
// Power handler for LE Automotive systems - Telux Power Manager Lib
#include <power_state.h>
#include <syslog.h>

#define ACK_TIMEOUT_US 300000 // 300 msec

#elif OPENWRT_BUILD
// Deep Sleep client handler for OpenWrt
#include "PowerStateClient.h"

#define GNSS_DEEPSLEEP_CLIENT_PRIORITY (100)
#define GNSS_DEEPSLEEP_CLIENT_NAME  "Gnss_loc"
#endif

using namespace std;

class GnssPowerHandler {
public:
    // singleton instance
    static GnssPowerHandler* getInstance();
#ifdef FEATURE_AUTOMOTIVE
    int sendPowerState(const power_state_t pwr_state);
#elif OPENWRT_BUILD
    int sendPowerState(const int dsEvent);
#endif
protected:
    ILocationControlAPI     *mLocationControlApi;
private:
    static GnssPowerHandler *mInstance;
    GnssPowerHandler();
    virtual inline ~GnssPowerHandler();
};

#endif // GNSS_LE_POWER_HANDLER
