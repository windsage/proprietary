/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2013-2018, 2021, 2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef __LBS_PROXY_H__
#define __LBS_PROXY_H__
#include <unordered_set>
#include <LBSProxyBase.h>
#include <LocAdapterBase.h>
#include <pthread.h>
#include <LocationDataTypes.h>
#define IzatProviderLib "libizatprovider.so"
using namespace loc_core;

namespace lbs_core {
class IzatProviderBase {
    public:
        inline virtual ~IzatProviderBase() {}
        inline virtual void connectToZProvider() { }
        inline virtual void registerIzatOdcpiCallback() { }
        inline virtual void setIzatContext(void* izatManager, void* context) { }
        inline virtual void getLocationWithZ(Location location, trackingCallback broadcaster) {
            broadcaster(location);
        }
        inline virtual void setNlpRequestStatus(bool isReqActive) {}
};
typedef IzatProviderBase* (getIzatProviderInst)();

class LBSProxy : public LBSProxyBase {
    static pthread_mutex_t mLock;
    static LocAdapterBase* mAdapter;
    static unsigned long mCapabilities;
    static const char * COM_QUALCOMM_LOCATION_APK_FILE_PATH;
    static const char * FFOS_LOCATION_EXTENDED_CLIENT;
    static const bool mLocationExtendedClientExists;
    static const bool mNativeXtraClientExists;
    bool mIzatFusedProviderOverride;
    mutable std::unordered_set<PowerStateLitener*> mPowerStateListeners;
public:
    inline LBSProxy() : LBSProxyBase(), mIzatProvider(NULL), mIzatFusedProviderOverride(false) {
        getIzatProvider();
    }
    inline ~LBSProxy() {}
    virtual LocApiBase* getLocApi(LOC_API_ADAPTER_EVENT_MASK_T exMask,
                                  ContextBase *context) const;
    inline virtual bool hasAgpsExtendedCapabilities() const {
        return mLocationExtendedClientExists;
    }
    inline virtual bool hasNativeXtraClient() const {
        return mNativeXtraClientExists;
    }
    inline virtual void notifyPowerState(PowerStateType pwoerState) const override {
        for (auto listener : mPowerStateListeners) {
            listener->notifyPowerState(pwoerState);
        }
    }
    inline virtual void registerPowerStateListener(PowerStateLitener* listener) const override {
        if (nullptr != listener) mPowerStateListeners.insert(listener);
    }
    inline virtual void unregisterPowerStateListener(PowerStateLitener* listener) const override {
        if (nullptr != listener) mPowerStateListeners.erase(listener);
    }
#ifdef MODEM_POWER_VOTE
#ifdef PDMAPPER_AVAILABLE
    void parseModemRegistryForGpsService(int* pModemType) const;
#endif /*PDMAPPER_AVAILABLE*/
#endif /*MODEM_POWER_VOTE*/
    inline virtual void modemPowerVote(bool power) const;
    virtual void injectFeatureConfig(ContextBase* context) const;
    void setIzatFusedProviderOverride(bool izatFused) override {
        mIzatFusedProviderOverride = izatFused;
    }
    bool getIzatFusedProviderOverride() const  override { return mIzatFusedProviderOverride; }
    virtual void populateAltitudeAndBroadCast(Location location,
            trackingCallback broadcaster) const;
private:
    inline static bool checkIfLocationExtendedClientExists();
    inline static bool checkIfNativeXtraClientExists();
    IzatProviderBase* mIzatProvider;
    void getIzatProvider();
};

}  // namespace lbs_core
#endif //__LBS_PROXY_H__
