/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2020-2021, 2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  Copyright (c) 2017, 2020 The Linux Foundation. All rights reserved
=============================================================================*/
#define LOG_TAG "LocSvc_LocNetIfaceBase"

#include <LocNetIfaceBase.h>
#include <loc_pla.h>
#include <log_util.h>
#include <loc_cfg.h>
#include "DataItemConcreteTypes.h"

#ifdef USE_WPACLIENT
#include "LocNetIfaceWpaClient.h"
#elif defined(USE_QCMAP)
#include <LocNetIface.h>
#endif

// LocNetIface implementation object
LocNetIfaceBase* LocNetIfaceBase::sLocNetIfaceImpl = nullptr;
std::mutex LocNetIfaceBase::sLocNetIfaceImplMutex;

LocNetIfaceBase* LocNetIfaceBase::getLocNetIfaceImpl() {
    sLocNetIfaceImplMutex.lock();
    if (nullptr == sLocNetIfaceImpl) {
#ifdef USE_WPACLIENT
        sLocNetIfaceImpl = new LocNetIfaceWpaClient();
#elif defined(USE_QCMAP)
        sLocNetIfaceImpl = new LocNetIface();
#endif
    }
    sLocNetIfaceImplMutex.unlock();
    return sLocNetIfaceImpl;
}

void LocNetIfaceBase::registerWwanCallStatusCallback(LocWwanCallStatusCb wwanCallStatusCb) {
    ENTRY_LOG();
    mWwanCallStatusCb = wwanCallStatusCb;
}

void LocNetIfaceBase::registerDataItemNotifyCallback(
        LocNetStatusChangeCb callback, void* userDataPtr) {
    ENTRY_LOG();
    if (mNotifyCb != NULL) {
        LOC_LOGE("Notify cb already registered !");
        return;
    }
    mNotifyCb = callback;
    mNotifyCbUserDataPtr = userDataPtr;
}

void LocNetIfaceBase::notifyObserverForNetworkInfo(bool isConnected, bool isRoaming,
        LocNetConnType connType) {
    ENTRY_LOG();
    // Check if observer is registered
    if (NULL == mNotifyCb) {
        LOC_LOGe("Notify callback NULL !");
        return;
    }
    // For disconnection case, send LOC_NET_CONN_TYPE_INVALID
    int32_t niConnType = (isConnected) ? (int32_t)connType:LOC_NET_CONN_TYPE_INVALID;
    // Create a network data item
    NetworkInfoDataItem networkInfoDataItem(niConnType, "", "", isConnected, isConnected,
            isRoaming, NETWORK_HANDLE_UNKNOWN, "");
    /* Notify back to client */
    mNotifyCb(mNotifyCbUserDataPtr, {&networkInfoDataItem});
}

void LocNetIfaceBase::notifyObserverForWlanStatus(bool isWlanEnabled) {
    ENTRY_LOG();
    /* Validate subscription object */
    if (NULL == mNotifyCb) {
        LOC_LOGe("Notify callback NULL !");
        return;
    }
    /* Create a wifi hardware status item */
    WifiHardwareStateDataItem wifiStateDataItem;
    wifiStateDataItem.mEnabled = isWlanEnabled;
    /* Notify back to client */
    mNotifyCb(mNotifyCbUserDataPtr, {&wifiStateDataItem});
}

bool LocNetIfaceBase::updateSubscribedItemSet(
        const std::unordered_set<DataItemId>& itemSet, bool addOrDelete){
    ENTRY_LOG();
    bool anyUpdatesToSet = false;

    /* Scroll through specified item unordered_set */
    std::unordered_set<DataItemId>::const_iterator it = itemSet.begin();
    for (; it != itemSet.end(); it++) {
        DataItemId itemId = *it;
        bool itemInSubscribedList = isItemSubscribed(itemId);
        /* Request to add */
        if (addOrDelete == true && !itemInSubscribedList) {
            mSubscribedItemList.push_back(itemId);
            anyUpdatesToSet = true;
        } else if (addOrDelete == false && itemInSubscribedList) {
            /* Request to delete */
            mSubscribedItemList.erase(
                    std::remove(
                            mSubscribedItemList.begin(),
                            mSubscribedItemList.end(), itemId),
                            mSubscribedItemList.end());
            anyUpdatesToSet = true;
        }
    }
    return anyUpdatesToSet;
}

// LocNetIfaceTimer implementation
void LocNetIfaceTimer::start()
{
    LOC_LOGd("start timer %s, waiting %" PRId64 " ms...", mName.c_str(), mWaitTimeInMs);
    mStarted = true;
    LocTimer::start((unsigned int) mWaitTimeInMs, false);
}

void LocNetIfaceTimer::stop()
{
    if (!mStarted) {
        LOC_LOGd("time %s not started yet.", mName.c_str());
        return;
    }
    LOC_LOGd("stop timer %s", mName.c_str());
    LocTimer::stop();
    mStarted = false;
}

void LocNetIfaceTimer::timeOutCallback() {
    LOC_LOGd("timer %s timeout", mName.c_str());
    mStarted = false;
    mRunable();
}
