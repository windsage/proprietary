/*
 * Copyright (c) 2021-2022, 2024 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define LOG_TAG "LocSvc_AIDL_AGnss"
#define LOG_NDEBUG 0

#include "LocAidlUtils.h"
#include "LocAidlAGnss.h"

#include <dlfcn.h>
#include <log_util.h>
#include <inttypes.h>
#include <OSFramework.h>

 //Fix the re definition comile error
#undef LOC_DEFAULT_SUB
#undef LOC_PRIMARY_SUB
#undef LOC_SECONDARY_SUB
#undef LOC_TERTIARY_SUB

namespace aidl {
namespace vendor {
namespace qti {
namespace gnss {
namespace implementation {

class NetworkConnListener : public IOSListener {
    std::shared_ptr<ILocAidlAGnssCallback> mAgnssCbIface;
public:
    inline NetworkConnListener(const std::shared_ptr<ILocAidlAGnssCallback>& cb) :
        IOSListener(), mAgnssCbIface(cb) {}

    inline virtual IzatListenerMask listensTo() const override {
        return IZAT_STREAM_REQUEST_NETWORK;
    }

    virtual void onLocationChanged (const IzatLocation * location, int locCnt,
            const IzatLocationStatus status) override {}

    virtual void onStatusChanged (const IzatProviderStatus status) override {}

    virtual void onNmeaChanged (const IzatNmea * nmea) override {}

    virtual void onRequestNetwork (bool isStart)  override {
        LocAidlAGnssStatusIpV4 st = {};
        //Set AGNSS type tp WWAN_ANY to indicate it is triggered by WWAN positioning session
        st.type = LocAidlAGnssType::TYPE_WWAN_ANY;
        st.status = isStart ? LocAidlAGnssStatusValue::REQUEST_AGNSS_DATA_CONN :
            LocAidlAGnssStatusValue::RELEASE_AGNSS_DATA_CONN;
        st.apnTypeMask = LocAidlApnTypeMask::APN_TYPE_MASK_DEFAULT;
        st.subId = LocAidlAGnssSubId::LOC_DEFAULT_SUB;
        mAgnssCbIface->locAidlAgnssStatusIpV4Cb(st);
    }
};

typedef const GnssInterface* (getLocationInterface)();

std::shared_ptr<ILocAidlAGnssCallback> LocAidlAGnss::sLocAidlAGnssCbIface = nullptr;
std::shared_ptr<LocAidlDeathRecipient> LocAidlAGnss::mDeathRecipient = nullptr;

void AGnssServiceDied(void* cookie) {
    LOC_LOGe("IAGnssCallback died.");
    LocAidlAGnss* iface = static_cast<LocAidlAGnss*>(cookie);
    if (iface != nullptr) {
        iface ->setCallbackExt(nullptr);
    };
}

const GnssInterface* LocAidlAGnss::getGnssInterface() {

    ENTRY_LOG();
    static bool getGnssInterfaceFailed = false;
    if (nullptr == mGnssInterface && !getGnssInterfaceFailed) {
        LOC_LOGd("loading libgnss.so::getGnssInterface ...");
        getLocationInterface* getter = NULL;
        const char *error = NULL;
        dlerror();
        void *handle = dlopen("libgnss.so", RTLD_NOW);
        if (NULL == handle)  {
            LOC_LOGD("dlopen for libgnss.so failed");
        } else if (NULL != (error = dlerror()))  {
            LOC_LOGD("dlopen for libgnss.so failed, error = %s", error);
        } else {
            getter = (getLocationInterface*)dlsym(handle, "getGnssInterface");
            if ((error = dlerror()) != NULL)  {
                LOC_LOGD("dlsym for libgnss.so::getGnssInterface failed, error = %s", error);
                getter = NULL;
            }
        }

        if (NULL == getter) {
            getGnssInterfaceFailed = true;
        } else {
            mGnssInterface = (const GnssInterface*)(*getter)();
        }
    }
    return mGnssInterface;
}

static AGpsType gType = AGPS_TYPE_INVALID;

LocAidlAGnss::LocAidlAGnss() : mIzatManager(getIzatManager(OSFramework::getOSFramework())),
    mListener(nullptr) {}

void LocAidlAGnss::locAidlAgnssStatusIpV4Cb(AGnssExtStatusIpV4 status) {

    ENTRY_LOG();
    if (mDeathRecipient != nullptr && mDeathRecipient->peerDied()) {
        LOC_LOGe("Peer has gone.");
        return;
    }

    LocAidlAGnssStatusIpV4 st = {};

    // cache the AGps Type
    gType = status.type;

    switch (status.type) {
    case LOC_AGPS_TYPE_SUPL:
        st.type = LocAidlAGnssType::TYPE_SUPL;
        break;
    case LOC_AGPS_TYPE_C2K:
        st.type = LocAidlAGnssType::TYPE_C2K;
        break;
    case LOC_AGPS_TYPE_WWAN_ANY:
        st.type = LocAidlAGnssType::TYPE_WWAN_ANY;
        break;
    case LOC_AGPS_TYPE_WIFI:
        st.type = LocAidlAGnssType::TYPE_WIFI;
        break;
    case LOC_AGPS_TYPE_SUPL_ES:
        st.type = LocAidlAGnssType::TYPE_SUPL_ES;
        break;
    default:
        st.type = (LocAidlAGnssType)status.type;
        break;
    }

    switch (status.status) {
    case LOC_GPS_REQUEST_AGPS_DATA_CONN:
        st.status = LocAidlAGnssStatusValue::REQUEST_AGNSS_DATA_CONN;
        break;
    case LOC_GPS_RELEASE_AGPS_DATA_CONN:
        st.status = LocAidlAGnssStatusValue::RELEASE_AGNSS_DATA_CONN;
        break;
    case LOC_GPS_AGPS_DATA_CONNECTED:
        st.status = LocAidlAGnssStatusValue::AGNSS_DATA_CONNECTED;
        break;
    case LOC_GPS_AGPS_DATA_CONN_DONE:
        st.status = LocAidlAGnssStatusValue::AGNSS_DATA_CONN_DONE;
        break;
    case LOC_GPS_AGPS_DATA_CONN_FAILED:
        st.status = LocAidlAGnssStatusValue::AGNSS_DATA_CONN_FAILED;
        break;
    default:
        LOC_LOGE("invalid status: %d", status.status);
        return;
    }

    switch (status.apnTypeMask) {
    case LOC_APN_TYPE_MASK_DEFAULT:
        st.apnTypeMask = LocAidlApnTypeMask::APN_TYPE_MASK_DEFAULT;
        break;
    case LOC_APN_TYPE_MASK_IMS:
        st.apnTypeMask = LocAidlApnTypeMask::APN_TYPE_MASK_IMS;
        break;
    case LOC_APN_TYPE_MASK_MMS:
        st.apnTypeMask = LocAidlApnTypeMask::APN_TYPE_MASK_MMS;
        break;
    case LOC_APN_TYPE_MASK_DUN:
        st.apnTypeMask = LocAidlApnTypeMask::APN_TYPE_MASK_DUN;
        break;
    case LOC_APN_TYPE_MASK_SUPL:
        st.apnTypeMask = LocAidlApnTypeMask::APN_TYPE_MASK_SUPL;
        break;
    case LOC_APN_TYPE_MASK_HIPRI:
        st.apnTypeMask = LocAidlApnTypeMask::APN_TYPE_MASK_HIPRI;
        break;
    case LOC_APN_TYPE_MASK_FOTA:
        st.apnTypeMask = LocAidlApnTypeMask::APN_TYPE_MASK_FOTA;
        break;
    case LOC_APN_TYPE_MASK_CBS:
        st.apnTypeMask = LocAidlApnTypeMask::APN_TYPE_MASK_CBS;
        break;
    case LOC_APN_TYPE_MASK_IA:
        st.apnTypeMask = LocAidlApnTypeMask::APN_TYPE_MASK_IA;
        break;
    case LOC_APN_TYPE_MASK_EMERGENCY:
        st.apnTypeMask = LocAidlApnTypeMask::APN_TYPE_MASK_EMERGENCY;
        break;
    default:
        st.apnTypeMask = LocAidlApnTypeMask::APN_TYPE_MASK_DEFAULT;
        break;
    }
    st.ipV4Addr = status.ipV4Addr;
    switch (status.subId) {
    case PRIMARY_SUB:
        st.subId = LocAidlAGnssSubId::LOC_PRIMARY_SUB;
        break;
    case SECONDARY_SUB:
        st.subId = LocAidlAGnssSubId::LOC_SECONDARY_SUB;
        break;
    case TERTIARY_SUB:
        st.subId = LocAidlAGnssSubId::LOC_TERTIARY_SUB;
        break;
    case DEFAULT_SUB:
    default:
        st.subId = LocAidlAGnssSubId::LOC_DEFAULT_SUB;
        break;
    }

    auto aGnssCbIface = sLocAidlAGnssCbIface;
    if (aGnssCbIface != nullptr) {
        LOC_LOGi("Calling Java locAidlAgnssStatusIpV4Cb with: type=%hhd" \
                 "status=%hhd mask=0x%" PRIx32 " subId=%hhd",
                 st.type, st.status, st.apnTypeMask, st.subId);
        aGnssCbIface->locAidlAgnssStatusIpV4Cb(st);
    } else {
        LOC_LOGe("AGNSS CB Iface NULL, setCallbackExt has not been called yet");
    }
}

::ndk::ScopedAStatus LocAidlAGnss::dataConnOpenExt(const string& apn,
        LocAidlApnType apnIpType, LocAidlAGnssType agnssType, bool* _aidl_return) {
    FROM_AIDL_CLIENT();

    /* Validate */
    if (apn.empty()) {
        LOC_LOGe("Invalid APN");
        *_aidl_return = false;
        return ndk::ScopedAStatus::ok();
    }

    LOC_LOGd("dataConnOpen APN name = [%s]", apn.c_str());

    std::string apnString(apn.c_str());
    // During Emergency SUPL, an apn name of "sos" means that no
    // apn was found, like in the simless case, so apn is cleared
    if (LOC_AGPS_TYPE_SUPL_ES == gType && "sos" == apnString) {
        LOC_LOGd("dataConnOpen APN name = [sos] cleared");
        apnString.clear();
    }

    LOC_LOGd("dataConnOpen APN name = [%s]", apnString.c_str());

    LOC_LOGd("dataConnOpen LocAidlApnType = %hhd", apnIpType);
    AGpsBearerType bearerType;
    switch (apnIpType) {
    case LocAidlApnType::IPV4:
            bearerType = AGPS_APN_BEARER_IPV4;
            break;
    case LocAidlApnType::IPV6:
            bearerType = AGPS_APN_BEARER_IPV6;
            break;
    case LocAidlApnType::IPV4V6:
            bearerType = AGPS_APN_BEARER_IPV4V6;
            break;
    case LocAidlApnType::INVALID:
        default:
        bearerType = AGPS_APN_BEARER_IPV4V6;
            break;
    }

    if (agnssType == LocAidlAGnssType::TYPE_WWAN_ANY) {
        //This is wwan positioning network connection case
        if (mIzatManager != nullptr) {
            IzatNetworkStatus networkStatus = {};
            networkStatus.status = IZAT_DATA_CONN_OPEN;
            networkStatus.type = static_cast<IzatAGpsType>(agnssType);
            networkStatus.bearerType = static_cast<IzatAGpsBearerType>(bearerType);
            networkStatus.apn = apnString;
            mIzatManager->notifyNetworkStatus(networkStatus);
        }
    } else {
        //This is AGPS SUPL case
        if (getGnssInterface() != nullptr &&
                getGnssInterface()->agpsDataConnOpen != NULL) {
            getGnssInterface()->agpsDataConnOpen(
                    (AGpsExtType)agnssType, apnString.c_str(), apnString.size(), (int)bearerType);
        } else {
            LOC_LOGe("Null GNSS interface");
            *_aidl_return = false;
            return ndk::ScopedAStatus::ok();
        }
    }

    *_aidl_return = true;
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlAGnss::dataConnClosedExt(LocAidlAGnssType agnssType,
        bool* _aidl_return) {
    FROM_AIDL_CLIENT();
    if (agnssType == LocAidlAGnssType::TYPE_WWAN_ANY) {
        //This is wwan positioning network connection case
        if (mIzatManager != nullptr) {
            IzatNetworkStatus networkStatus = {};
            networkStatus.status = IZAT_DATA_CONN_CLOSE;
            mIzatManager->notifyNetworkStatus(networkStatus);
        }
    } else {
        //This is AGPS SUPL case
        if (getGnssInterface() == nullptr ||
                getGnssInterface()->agpsDataConnClosed == NULL) {
            LOC_LOGE("Null GNSS interface");
            *_aidl_return = false;
            return ndk::ScopedAStatus::ok();
        }

        getGnssInterface()->agpsDataConnClosed((AGpsExtType)agnssType);
    }
    *_aidl_return = true;
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlAGnss::dataConnFailedExt(LocAidlAGnssType agnssType,
        bool* _aidl_return) {
    FROM_AIDL_CLIENT();
    if (agnssType == LocAidlAGnssType::TYPE_WWAN_ANY) {
        //This is wwan positioning network connection case
        if (mIzatManager != nullptr) {
            IzatNetworkStatus networkStatus = {};
            networkStatus.status = IZAT_DATA_CONN_FAILED;
            mIzatManager->notifyNetworkStatus(networkStatus);
        }
    } else {
        //This is AGPS SUPL case
        if (getGnssInterface() == nullptr ||
                getGnssInterface()->agpsDataConnFailed == NULL) {
            LOC_LOGE("Null GNSS interface");
            *_aidl_return = false;
            return ndk::ScopedAStatus::ok();
        }

        getGnssInterface()->agpsDataConnFailed((AGpsExtType)agnssType);
    }
    *_aidl_return = true;
    return ndk::ScopedAStatus::ok();
}

// Methods from ::vendor::qti::gnss::ILocAidlAGnss follow.
::ndk::ScopedAStatus LocAidlAGnss::setCallbackExt(
        const std::shared_ptr<::aidl::vendor::qti::gnss::ILocAidlAGnssCallback>& callback) {
    FROM_AIDL_CLIENT();

    if (getGnssInterface() == nullptr ||
        getGnssInterface()->agpsDataConnFailed == NULL) {
        LOC_LOGE("Null GNSS interface");
        return ndk::ScopedAStatus::ok();
    }

    // Register death recipient
    if (mDeathRecipient == nullptr) {
        mDeathRecipient = make_shared<LocAidlDeathRecipient>(AGnssServiceDied);
    }
    if (sLocAidlAGnssCbIface != nullptr && mDeathRecipient != nullptr) {
        mDeathRecipient->unregisterFromPeer(sLocAidlAGnssCbIface->asBinder().get(), this);
    }
    // Save the interface
    sLocAidlAGnssCbIface = callback;
    if (callback != nullptr) {
        mDeathRecipient->registerToPeer(callback->asBinder().get(), this);
    } else {
        LOC_LOGe("callback is NULL");
    }

    AgpsCbInfo cbInfo = {};
    cbInfo.statusV4Cb = [this](AGnssExtStatusIpV4 status) {
        locAidlAgnssStatusIpV4Cb(status);
    };
    cbInfo.atlType = AGPS_ATL_TYPE_SUPL | AGPS_ATL_TYPE_SUPL_ES;
    cbInfo.cbPriority = AGPS_CB_PRIORITY_HIGH;
    getGnssInterface()->agpsInit(cbInfo);

    if (callback != nullptr) {
        // Set callback for WWAN positioning to bring up network connection
        mListener = new NetworkConnListener(callback);
        mIzatManager->subscribeListener(mListener);
    }
    return ndk::ScopedAStatus::ok();
}

}  // namespace implementation
}  // namespace aidl
}  // namespace gnss
}  // namespace qti
}  // namespace vendor
