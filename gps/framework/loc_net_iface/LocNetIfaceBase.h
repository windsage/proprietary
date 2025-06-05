/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2020-2021, 2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  Copyright (c) 2017, 2020 The Linux Foundation. All rights reserved
=============================================================================*/
#ifndef LOC_NET_IFACE_BASE_H
#define LOC_NET_IFACE_BASE_H

#include <functional>
#include <IDataItemCore.h>
#include <loc_gps.h>
#include <loc_cfg.h>
#include <algorithm>
#include <vector>
#include <list>
#include <unordered_set>
#include "loc_pla.h"
#include <string>
#include <iostream>
#include <log_util.h>
#include <LocTimer.h>
#include <mutex>

using namespace loc_core;
using namespace loc_util;

#define LOCNETIFACE_AGPS_CLIENT    "agps-client"
#define LOCNETIFACE_SUPL_CLIENT    "supl-client"
#define LOCNETIFACE_SUPL_ES_CLIENT "supl-es-client"

// Callback functions for timer expiry
typedef function<void()> TimerCallback;

/* Connectivity Type Enum
 *
 * These values are same as we define in case of LA,
 * except for emergency type WWAN which is not defined there. */

typedef enum {
    LOC_NET_CONN_TYPE_INVALID = 0,
    LOC_NET_CONN_TYPE_WLAN = 100,
    LOC_NET_CONN_TYPE_ETHERNET = 101,
    LOC_NET_CONN_TYPE_BLUETOOTH = 102,
    LOC_NET_CONN_TYPE_USB_CRADLE = 103,
    LOC_NET_CONN_TYPE_WWAN_INTERNET = 201,
    LOC_NET_CONN_TYPE_WWAN_SUPL = 205,
    LOC_NET_CONN_TYPE_WWAN_EMERGENCY = 206,
    LOC_NET_CONN_TYPE_MAX
} LocNetConnType;

/* WWAN call event */
typedef enum {
    LOC_NET_WWAN_CALL_EVT_INVALID = 0,
    LOC_NET_WWAN_CALL_EVT_OPEN_SUCCESS,
    LOC_NET_WWAN_CALL_EVT_OPEN_FAILED,
    LOC_NET_WWAN_CALL_EVT_CLOSE_SUCCESS,
    LOC_NET_WWAN_CALL_EVT_CLOSE_FAILED,
    LOC_NET_WWAN_CALL_EVT_MAX
} LocNetWwanCallEvent;

/* Connectivity State */
typedef enum {
    LOC_NET_CONN_STATE_INVALID = 0,
    LOC_NET_CONN_STATE_ENABLED,
    LOC_NET_CONN_STATE_DISABLED,
    LOC_NET_CONN_STATE_CONNECTED,
    LOC_NET_CONN_STATE_DISCONNECTED,
    LOC_NET_CONN_STATE_MAX
} LocNetConnState;

/* WWAN call status callback
 * apn and apnIpType values are valid based on event. */
typedef void (*LocWwanCallStatusCb)(
        LocNetWwanCallEvent event, LocNetConnType locNetConnType,
        const char* apn, LocApnIpType apnIpType);

/* DataItem Notification callback */
typedef void (*LocNetStatusChangeCb)(
        void* userDataPtr, const std::unordered_set<IDataItemCore*>& itemList);

/*--------------------------------------------------------------------
 * CLASS LocNetIfaceTimer
 *
 * Functionality:
 * Timer functionality
 *-------------------------------------------------------------------*/

class LocNetIfaceTimer : public LocTimer {
public:
    LocNetIfaceTimer(const string& name) :
            LocTimer(), mName(name), mStarted(false), mWaitTimeInMs(100) {
        LOC_LOGd("LocNetIfaceTimer %s created.", mName.c_str());
    }

    inline void set(const time_t waitTimeSec, const TimerCallback& runable) {
        mWaitTimeInMs = (int64_t)waitTimeSec * 1000;
        mRunable = runable;
    }

    void start();
    inline void start(const time_t waitTimeSec) {
        mWaitTimeInMs = (int64_t)waitTimeSec * 1000;
        start();
    }
    void stop();

    inline void restart() { stop(); start(); }
    inline void restart(const time_t waitTimeSec) { stop(); start(waitTimeSec); }

    inline bool isStarted() {
        return mStarted;
    }

private:
    // Override
    virtual void timeOutCallback() override;

    string mName;
    bool mStarted;
    int64_t mWaitTimeInMs;
    TimerCallback mRunable;
};

/*--------------------------------------------------------------------
 * CLASS LocNetIfaceBase
 *
 * Functionality:
 * Interface to OS specific network connection functionality.
 * Currently supported actions:
 * - Register for WLAN/WWAN connectivity indications
 * - Setup / Teardown WWAN data call
 *-------------------------------------------------------------------*/
class LocNetIfaceBase {

public:
    // Get LocnetImpl object singleton instance
    static LocNetIfaceBase* getLocNetIfaceImpl();

    /* Setup WWAN backhaul via QCMAP
     * This sets up IP routes as well for any AP socket */
    virtual bool connectBackhaul(const string& clientName,
                                 bool async = true,
                                 const LocSubId subId = LOC_DEFAULT_SUB,
                                 const string& apn = string(),
                                 const LocApnIpType ipType = LOC_APN_IP_IPV4) = 0;

    /* Disconnects the WWANbackhaul, only if it was setup by us */
    virtual bool disconnectBackhaul(const string& clientName,
                                    bool async = true,
                                    const LocSubId subId = LOC_DEFAULT_SUB,
                                    const string& apn = string(),
                                    const LocApnIpType ipType = LOC_APN_IP_IPV4) = 0;

    /* Register data call setup callback
     * If callback is registered, we notify back data call status with it. */
    void registerWwanCallStatusCallback(LocWwanCallStatusCb wwanCallStatusCb);

    /* Register for data items */
    virtual void subscribe(
            const std::unordered_set<DataItemId>& itemListToSubscribe) = 0;

    /* Unregister for data items */
    virtual void unsubscribe(
            const std::unordered_set<DataItemId>& itemListToUnsubscribe) = 0;

    /* Unregister all data items */
    virtual void unsubscribeAll() = 0;

    /* Request data items current value */
    virtual void requestData(
            const std::unordered_set<DataItemId>& itemListToRequestData) = 0;

    /* Register Notification callback  */
    void registerDataItemNotifyCallback(
            LocNetStatusChangeCb callback, void* userDataPtr);

    /* Virtual destructor since we have other virtual methods */
    virtual ~LocNetIfaceBase() {};

    void notifyObserverForWlanStatus(bool isWlanEnabled);
    void notifyObserverForNetworkInfo(bool isConnected, bool isRoaming, LocNetConnType connType);

private:
    // LocNetIface implementation object
    static LocNetIfaceBase* sLocNetIfaceImpl;
    static std::mutex sLocNetIfaceImplMutex;

protected:
    /* List of data items subscribed at any instant */
    std::vector<DataItemId> mSubscribedItemList;

    /* Data Item notification callback registered.
     * This information is not instance specific, supports only single
     * client. */
    LocNetStatusChangeCb mNotifyCb;
    void* mNotifyCbUserDataPtr;

    /* WWAN data call setup callback */
    LocWwanCallStatusCb mWwanCallStatusCb;

    /* Config items */
    /* WWAN Call type supported by this instance */
    LocNetConnType mLocNetConnType;
    /* APN name and IP type for internet call */
    char mInternetApnName[LOC_MAX_PARAM_STRING];
    LocApnIpType  mInternetIpType;
    /* APN name and IP type for supl data call */
    char mSuplApnName[LOC_MAX_PARAM_STRING];
    LocApnIpType  mSuplIpType;

    LocNetIfaceBase() :
        mSubscribedItemList(), mWwanCallStatusCb(NULL), mSuplIpType(LOC_APN_IP_INVALID),
        mNotifyCb(NULL), mNotifyCbUserDataPtr(NULL),
        mInternetIpType(LOC_APN_IP_INVALID), mLocNetConnType(LOC_NET_CONN_TYPE_INVALID) {
        memset(mInternetApnName, 0, LOC_MAX_PARAM_STRING);
        memset(mSuplApnName, 0, LOC_MAX_PARAM_STRING);
    }

    // Get/Set Internet APN and IP type
    inline void setInternetApnName(char* apn) {
        strlcpy(mInternetApnName, apn, LOC_MAX_PARAM_STRING);
    }
    inline char* getInternetApnName() {
        return mInternetApnName;
    }
    inline void setInternetIpType(LocApnIpType ipType) {
        mInternetIpType = ipType;
    }
    inline LocApnIpType getInternetIpType() {
        return mInternetIpType;
    }

    // Get/Set SUPL APN and IP type
    inline void setSuplApnName(char* apn) {
        strlcpy(mSuplApnName, apn, LOC_MAX_PARAM_STRING);
    }
    inline char* getSuplApnName() {
        return mSuplApnName;
    }
    inline void setSuplIpType(LocApnIpType ipType) {
        mSuplIpType = ipType;
    }
    inline LocApnIpType getSuplIpType() {
        return mSuplIpType;
    }

    inline void setLocNetConnType(LocNetConnType locNetConnType) {
        mLocNetConnType = locNetConnType;
    }

    inline LocNetConnType getLocNetConnType() {
        return mLocNetConnType;
    }

    /* Update the subscribed item unordered_set
     * addOrDelete: true = append items to subscribed unordered_set
     *              false = delete items from subscribed unordered_set
     * Just a utility method to be used from platform specific sub-classes
     * Returns true if any updates are made to the subscription unordered_set,
     * or else false. */
    bool updateSubscribedItemSet(
            const std::unordered_set<DataItemId>& itemSet, bool addOrDelete);

    /* Utility method */
    inline bool isItemSubscribed(DataItemId itemId){
        return ( mSubscribedItemList.end() !=
                    std::find( mSubscribedItemList.begin(),
                            mSubscribedItemList.end(), itemId));
    }
};
#endif /* #ifndef LOC_NET_IFACE_BASE_H */
