/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef LOC_NET_IFACE_WPACLIENT_H
#define LOC_NET_IFACE_WPACLIENT_H

#include <future>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <thread>
#include <loc_pla.h>
#include <LocNetIfaceBase.h>
#include <mutex>
#include <gps_extended.h>
#include <functional>

#include <LocTimer.h>

#include "common.h"
#include "stdlib.h"
#include "wpa_ctrl.h"

using namespace std;

#define CONFIG_CTRL_IFACE_UNIX
#define WPA_CONFIG_CTRL_IFACE "/var/run/wpa_supplicant/wlan0"

class LocNetIfaceWpaClient : public LocNetIfaceBase {

public:
    /* Constructor */
    LocNetIfaceWpaClient();
    LocNetIfaceWpaClient(LocNetIfaceWpaClient const&) = delete;
    void operator=(LocNetIfaceWpaClient const&) = delete;

    /* Override base class pure virtual methods */
    void subscribe(const std::unordered_set<DataItemId>& itemSetToSubscribe);
    void unsubscribe(const std::unordered_set<DataItemId>& itemSetToUnsubscribe);
    void unsubscribeAll();
    void requestData(const std::unordered_set<DataItemId>& itemSetToRequestData);

    inline bool connectBackhaul(const string& clientName,
                         bool async = true,
                         const LocSubId subId = LOC_DEFAULT_SUB,
                         const string& apn = string(),
                         const LocApnIpType ipType = LOC_APN_IP_IPV4) {
        // not supported
        return false;
    }

    inline bool disconnectBackhaul(const string& clientName,
                            bool async = true,
                            const LocSubId subId = LOC_DEFAULT_SUB,
                            const string& apn = string(),
                            const LocApnIpType ipType = LOC_APN_IP_IPV4) {
        // not supported
        return false;
    }

    recursive_mutex& getMutex(){ return mMutex; }

private:
    void subscribeWithWpaClient();
    void unsubscribeWithWpaClient();
    void startAsyncMonitorConnThread();

    bool getWpaStatus(const char *field, char *outBuf, size_t outBufSize);
    bool checkIfWlanAlreadyConnected();

    inline int stringMatch(const char *a, const char *b) {
        return strncmp(a, b, strlen(b)) == 0;
    }

    inline bool isInterestedWpaEvent(const char *evt) {
        bool ret = false;
        if (stringMatch(evt, WPA_EVENT_TERMINATING) || stringMatch(evt, WPA_EVENT_STATE_CHANGE) \
                || stringMatch(evt, AP_EVENT_ENABLED) || stringMatch(evt, AP_EVENT_DISABLED) \
                || stringMatch(evt, INTERFACE_ENABLED) || stringMatch(evt, INTERFACE_DISABLED)) {
            ret = true;
        }
        return ret;
    }

    struct wpa_ctrl *mCtrlConn;
    struct wpa_ctrl *mMonitorConn;

    string strCtrlIface;

    LocNetIfaceTimer mCtrlConnOpenTimer;

    /* Mutex for synchronization */
    recursive_mutex mMutex;
};

#endif /* #ifndef LOC_NET_IFACE_WPACLIENT_H */
