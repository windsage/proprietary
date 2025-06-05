/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  loc service module

  Copyright  (c) 2015-2017, 2020-2023 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/


#define LOG_TAG "Subscription"
#define LOG_NDEBUG 0
#include <loc_pla.h>
#include <log_util.h>
#include "Subscription.h"
#include "IzatDefines.h"
#include <mutex>

Subscription* Subscription::mSubscriptionObj = NULL;
#ifdef USE_LOCNETIFACE
LocNetIfaceBase* Subscription::mLocNetIfaceObj = NULL;
#endif
IDataItemObserver* Subscription::mObserverObj = NULL;
std::unordered_set<DataItemId> Subscription::mCachedDi = {};
std::unordered_map<SubscriptionCallbackIface*, uint64_t> Subscription::sSubscriptionCbMap = {};
std::mutex mMutex;
using namespace std;

// Subscription class implementation
Subscription* Subscription::getSubscriptionObj()
{
    int result = 0;

    ENTRY_LOG();
    std::lock_guard<std::mutex> lock(mMutex);
    do {
          // already initialized
          BREAK_IF_NON_ZERO(0, mSubscriptionObj);

          mSubscriptionObj = new (std::nothrow) Subscription();
          BREAK_IF_ZERO(1, mSubscriptionObj);
#ifdef USE_LOCNETIFACE
          int use_locnetiface_manager = 1;
          static loc_param_s_type gps_conf_param_table[] = {
              {"USE_LOCNETIFACE_MANAGER", &use_locnetiface_manager, NULL, 'n'}
          };
          UTIL_READ_CONF(LOC_PATH_GPS_CONF, gps_conf_param_table);
          LOC_LOGd("USE_LOCNETIFACE_MANAGER %d", use_locnetiface_manager);
          if (use_locnetiface_manager != 0) {
              mLocNetIfaceObj = LocNetIfaceBase::getLocNetIfaceImpl();
              BREAK_IF_ZERO(2, mLocNetIfaceObj);
              mLocNetIfaceObj->registerDataItemNotifyCallback(
                    Subscription::locNetIfaceCallback, NULL);
          }
#endif
          result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
    return mSubscriptionObj;
}

//IDataItemSubscription overrides
void Subscription::subscribe(const std::unordered_set<DataItemId> & dataItemList,
        IDataItemObserver * observerObj)
{
    // Assign the observer object if required
    if ((Subscription::mObserverObj == NULL) && (observerObj)) {
        Subscription::mObserverObj = observerObj;
    }
    std::unordered_set<DataItemId> dataItemSet;
    // POWER_CONNECTED_STATE_DATA_ITEM_ID, NETWORKINFO_DATA_ITEM_ID, GPSSTATE_DATA_ITEM_ID
    // either computed internally in Gnss Hal process OR other external hal (like Health hal)
    // don't send out the data subscription for these dataitems to OSAgent.
    // Also don't send out TAC_DATA_ITEM_ID to OsAgent because OsAgent only supports
    // request data for this data item.
    for (auto it = dataItemList.begin(); it != dataItemList.end(); ++it) {
        switch (*it) {
            case POWER_CONNECTED_STATE_DATA_ITEM_ID:
            case NETWORKINFO_DATA_ITEM_ID:
            case GPSSTATE_DATA_ITEM_ID:
            case TAC_DATA_ITEM_ID:
                break;
            default:
                dataItemSet.insert(*it);
        }
    }

    if (dataItemSet.empty()) {
        LOC_LOGe("No dataItem subscribed");
        return;
    }

#if defined(USE_LOCNETIFACE)
    if (mLocNetIfaceObj) {
        mLocNetIfaceObj->subscribe(dataItemSet);
    }
#else

    uint64_t dataItemSetMask = 0;
    for (auto item: dataItemSet)  {
        dataItemSetMask |= (1 << item);
    }
    {
        std::lock_guard<std::mutex> lock(mMutex);
        if (dataItemSetMask != 0) {
            for (auto subscriber : sSubscriptionCbMap) {
                uint64_t outMask = 0;
                std::unordered_set<DataItemId> subDataItemSet = {};
                outMask = getDataItemSetPerMask(dataItemSet, subDataItemSet, subscriber.second);
                if (!subDataItemSet.empty()) {
                    (subscriber.first)->updateSubscribe(subDataItemSet, true);
                    if (!mCachedDi.empty()) {
                        for (auto di: subDataItemSet) {
                            mCachedDi.erase(di);
                        }
                    }
                    dataItemSetMask &= ~(outMask);
                }
            }
        }
    }

    // cache the data items
    if (dataItemSetMask > 0) {
        std::unordered_set<DataItemId> dataItemsToCache = {};
        getDataItemSetPerMask(dataItemSet, dataItemsToCache, dataItemSetMask);
        mCachedDi.insert(dataItemsToCache.begin(), dataItemsToCache.end());
    }
#endif
}

uint64_t Subscription::getDataItemSetPerMask(const std::unordered_set<DataItemId>& in,
        std::unordered_set<DataItemId>& out, uint64_t dataItemIdMask) {
    uint64_t outDataItemIdMask = 0;
    for (auto item : in) {
        if (dataItemIdMask & (1 << item)) {
            out.insert(item);
            outDataItemIdMask |= (1 << item);
        }
    }
    return outDataItemIdMask;
}

void Subscription::updateSubscription(const std::unordered_set<DataItemId> & /*l*/, IDataItemObserver * /*observerObj*/)
{
}

void Subscription::requestData(const std::unordered_set<DataItemId> & dataItemSet,
                               IDataItemObserver * observerObj)
{
    LOC_LOGd("Subscription::requestData");
    // Assign the observer object if required
    if ((Subscription::mObserverObj == NULL) && (observerObj)) {
        Subscription::mObserverObj = observerObj;
    }

#if defined(USE_LOCNETIFACE)
    if (mLocNetIfaceObj) {
        mLocNetIfaceObj->requestData(dataItemSet);
    }
#else
    // It is possible that requestData is called before setSubscriptionCallback
    // Cache data item and requestData again when setSubscriptionCallback
    // if there is any data item is not handled
    std::unordered_set<DataItemId> remainingDataItemSet = dataItemSet;
    {
        std::lock_guard<std::mutex> lock(mMutex);
        for (auto item : sSubscriptionCbMap) {
            std::unordered_set<DataItemId> subDataItemSet = {};
            getDataItemSetPerMask(dataItemSet, subDataItemSet, item.second);
            if (!subDataItemSet.empty()) {
                (item.first)->requestData(subDataItemSet);
                for (auto subItem : subDataItemSet) {
                    remainingDataItemSet.erase(subItem);
                }
            }
        }
    }

    if (!remainingDataItemSet.empty()) {
        mCachedDi.insert(remainingDataItemSet.begin(), remainingDataItemSet.end());
    }
#endif
}

void Subscription::unsubscribe(const std::unordered_set<DataItemId> & dataItemList,
                               IDataItemObserver * observerObj)
{
     // Assign the observer object if required
    if ((Subscription::mObserverObj == NULL) && (observerObj)) {
        Subscription::mObserverObj = observerObj;
    }

#if defined(USE_LOCNETIFACE)
    if (mLocNetIfaceObj) {
        mLocNetIfaceObj->unsubscribe(dataItemList);
    }
#else
    {
        std::lock_guard<std::mutex> lock(mMutex);
        for (auto item : sSubscriptionCbMap) {
            std::unordered_set<DataItemId> subDataItemSet = {};
            getDataItemSetPerMask(dataItemList, subDataItemSet, item.second);
            if (!subDataItemSet.empty()) {
                (item.first)->updateSubscribe(subDataItemSet, false);
            }
        }
    }
#endif
}

void Subscription::unsubscribeAll(IDataItemObserver * observerObj)
{
    // Assign the observer object if required
    if ((Subscription::mObserverObj == NULL) && (observerObj)) {
        Subscription::mObserverObj = observerObj;
    }

#if defined(USE_LOCNETIFACE)
    if (mLocNetIfaceObj) {
       mLocNetIfaceObj->unsubscribeAll();
    }
#else
    {
        std::lock_guard<std::mutex> lock(mMutex);
        if (sSubscriptionCbMap.empty()) {
            LOC_LOGE("sSubscriptionCbMap NULL !");
            return;
        }
        for (auto item : sSubscriptionCbMap) {
            item.first->unsubscribeAll();
        }
    }

#endif
}

void Subscription::setSubscriptionCallback(SubscriptionCallbackIface* cb, uint64_t dataItemIdMask) {

    ENTRY_LOG();
    if (nullptr == cb) {
        LOC_LOGe("mSubscriptionCb is nullptr.");
        return;
    }

    {
        std::lock_guard<std::mutex> lock(mMutex);
        // erase the Subscription callback when dataItem Mask is 0
        if (dataItemIdMask == 0) {
            auto itr = sSubscriptionCbMap.find(cb);
            sSubscriptionCbMap.erase(itr);
            return;
        } else {
            sSubscriptionCbMap[cb] = dataItemIdMask;
        }
    }
    LOC_LOGd("Client subscription mask [%p] = 0x%" PRIx64 "", cb, sSubscriptionCbMap[cb]);

    if ((NULL != Subscription::mObserverObj) && !mCachedDi.empty()) {
        // Subscribe request came before we received SubscriptionCallbackIface
        // object. Subscribe to these data items and request for data item value.
        LOC_LOGD("Subscribing to items in cache..");
        mSubscriptionObj->subscribe(mCachedDi, Subscription::mObserverObj);
        mSubscriptionObj->requestData(mCachedDi, Subscription::mObserverObj);
    }
}

SubscriptionCallbackIface* Subscription::getSubscriptionCallback(DataItemId id) {
    SubscriptionCallbackIface* cb = NULL;
    {
        std::lock_guard<std::mutex> lock(mMutex);
        for (auto item : sSubscriptionCbMap) {
            if ((item.second) & (1 << id)) {
                cb = item.first;
                break;
            }
        }
    }

    return cb;
}

#ifdef USE_LOCNETIFACE
void Subscription::locNetIfaceCallback(void* userDataPtr,
        const std::unordered_set<IDataItemCore*>& itemSet) {
    LOC_LOGV("Subscription::locNetIfaceCallback");
    if (Subscription::mObserverObj == NULL) {
        LOC_LOGE("NULL observer object");
        return;
    }
    Subscription::mObserverObj->notify(itemSet);
}
#endif

extern "C" void notifyToSubscriberExt(std::unordered_set<IDataItemCore*> itemSet){
    if (NULL != Subscription::mObserverObj) {
        LOC_LOGd("updating opt-in");
        Subscription::mObserverObj->notify(itemSet);
    } else {
        LOC_LOGd("Subscription::mObserverObj is null!!!");
    }
}
