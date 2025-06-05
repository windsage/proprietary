/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  loc service module

  Copyright (c) 2015-2017, 2020-2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef __SUBSCRIPTION_H__
#define __SUBSCRIPTION_H__

#include <IDataItemSubscription.h>
#include <IDataItemObserver.h>
#include <DataItemId.h>
#include <loc_pla.h>
#include "IDataItemCore.h"
#include <unordered_map>
#ifdef USE_LOCNETIFACE
#include "LocNetIfaceBase.h"
#endif

using loc_core::IDataItemObserver;
using loc_core::IDataItemSubscription;
using loc_core::IDataItemCore;

struct SubscriptionCallbackIface {

    virtual ~SubscriptionCallbackIface() = default;
    virtual void updateSubscribe(const std::unordered_set<DataItemId> & list, bool subscribe) = 0;
    virtual void requestData(const std::unordered_set<DataItemId> & list) = 0;
    virtual void unsubscribeAll() = 0;
};

class Subscription : public IDataItemSubscription {

public:
    static IDataItemObserver *mObserverObj;

    static Subscription* getSubscriptionObj();
    static void destroyInstance();
#ifdef USE_LOCNETIFACE
    inline LocNetIfaceBase* getLocNetIfaceObj() {
        return mLocNetIfaceObj;
    }
#endif

    // IDataItemSubscription overrides
    void subscribe(const std::unordered_set<DataItemId> & l, IDataItemObserver * observerObj = NULL);
    void updateSubscription(const std::unordered_set<DataItemId> & l, IDataItemObserver * observerObj = NULL);
    void requestData(const std::unordered_set <DataItemId> & l, IDataItemObserver * client = NULL);
    void unsubscribe(const std::unordered_set<DataItemId> & l, IDataItemObserver * observerObj = NULL);
    void unsubscribeAll(IDataItemObserver * observerObj = NULL);
    uint64_t getDataItemSetPerMask(const std::unordered_set <DataItemId>& in,
        std::unordered_set<DataItemId>& out, uint64_t dataItemIdMask);
    static void setSubscriptionCallback(SubscriptionCallbackIface* cb, uint64_t dataItemIdMask);
    static SubscriptionCallbackIface* getSubscriptionCallback(DataItemId id);

protected:

private:
    static Subscription *mSubscriptionObj;
    static std::unordered_set<DataItemId> mCachedDi;
#ifdef USE_LOCNETIFACE
    static LocNetIfaceBase* mLocNetIfaceObj;
#endif

    Subscription() {
      mObserverObj = NULL;
    }
    ~Subscription() {}

    //subscriptionCb and subscribed item mask
    static std::unordered_map<SubscriptionCallbackIface*, uint64_t> sSubscriptionCbMap;

#ifdef USE_LOCNETIFACE
    static void locNetIfaceCallback(
            void* userDataPtr, const std::unordered_set<IDataItemCore*>& itemSet);
#endif

};

#endif // #ifndef __SUBSCRIPTION_H__
