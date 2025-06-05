/*
 * Copyright (c) 2021-2022, 2024 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#define LOG_TAG "LocSvc_AIDL_GnssConfigService"
#define LOG_NDEBUG 0

#include "LocAidlGnssConfigService.h"
#include <log_util.h>
#include <gps_extended_c.h>
#include <dlfcn.h>

namespace aidl {
namespace vendor {
namespace qti {
namespace gnss {
namespace implementation {

typedef const GnssInterface* (getLocationInterface)();

#define GNSS_CONFIG_SERVICE_ERROR 1
#define GNSS_CONFIG_SERVICE_SUCCESS 0

std::shared_ptr<LocAidlDeathRecipient> LocAidlGnssConfigService::mDeathRecipient = nullptr;
std::shared_ptr<ILocAidlGnssConfigServiceCallback> LocAidlGnssConfigService::sCallbackIface
    = nullptr;

void GnssConfigServiceDied(void* cookie) {
    LOC_LOGe("ILocAidlGnssConfigServiceCallback died.");
    auto thiz = static_cast<LocAidlGnssConfigService*>(cookie);
    thiz->handleAidlClientSsr();
}

void
LocAidlGnssConfigService::getGnssSvTypeConfigCallback(
        const GnssSvTypeConfig& config)
{
    ENTRY_LOG();
    if (mDeathRecipient != nullptr && mDeathRecipient->peerDied()) {
        LOC_LOGe("Peer Died.");
        sCallbackIface = nullptr;
        return;
    }
    if (sCallbackIface == nullptr) {
        LOC_LOGE("sCallbackIface NULL");
        return;
    }
    if (sCallbackIface != nullptr) {
        // convert config to vector
        std::vector<LocAidlGnssConstellationType> disabledSvTypeVec;
        svTypeConfigToVec(config, disabledSvTypeVec);

        TO_AIDL_CLIENT();
        sCallbackIface->getGnssSvTypeConfigCb(disabledSvTypeVec);
    } else {
        // convert config to vector
        std::vector<LocAidlGnssConstellationType> disabledSvTypeVec;
        svTypeConfigToVec(config, disabledSvTypeVec);

        TO_AIDL_CLIENT();
        sCallbackIface->getGnssSvTypeConfigCb(disabledSvTypeVec);
    }
}

const GnssInterface* LocAidlGnssConfigService::getGnssInterface()
{
    ENTRY_LOG();

    if (nullptr == mGnssInterface && !mGetGnssInterfaceFailed) {
        LOC_LOGd("loading libgnss.so::getGnssInterface ...");
        getLocationInterface* getter = NULL;
        const char *error = NULL;
        dlerror();
        void *handle = dlopen("libgnss.so", RTLD_NOW);
        if (NULL == handle)  {
            LOC_LOGE("dlopen for libgnss.so failed");
        } else if (NULL != (error = dlerror()))  {
            LOC_LOGE("dlopen for libgnss.so failed, error = %s", error);
        } else {
            getter = (getLocationInterface*)dlsym(handle, "getGnssInterface");
            if ((error = dlerror()) != NULL)  {
                LOC_LOGE("dlsym for libgnss.so::getGnssInterface failed, error = %s", error);
                getter = NULL;
            }
        }

        if (NULL == getter) {
            mGetGnssInterfaceFailed = true;
        } else {
            mGnssInterface = (const GnssInterface*)(*getter)();
        }
    }
    return mGnssInterface;
}

LocAidlGnssConfigService::LocAidlGnssConfigService() :
        mGnssInterface(nullptr), mGetGnssInterfaceFailed(false)
{
    ENTRY_LOG();
}

LocAidlGnssConfigService::~LocAidlGnssConfigService()
{
    ENTRY_LOG();
}

void LocAidlGnssConfigService::handleAidlClientSsr() {
    if (sCallbackIface != nullptr && mDeathRecipient != nullptr) {
        mDeathRecipient->unregisterFromPeer(sCallbackIface->asBinder().get(), this);
    }
    mDeathRecipient = nullptr;
    sCallbackIface = nullptr;
}
::ndk::ScopedAStatus LocAidlGnssConfigService::getGnssSvTypeConfig()
{
    FROM_AIDL_CLIENT();

    const GnssInterface* gnssInterface = getGnssInterface();
    if (nullptr == gnssInterface) {
        LOC_LOGE("Failed to get GNSS Interface");
        return ndk::ScopedAStatus::ok();
    }
    GnssSvTypeConfigCallback callback = [](const GnssSvTypeConfig& config) {
        LocAidlGnssConfigService::getGnssSvTypeConfigCallback(config);
    };
    gnssInterface->gnssGetSvTypeConfig(callback);

    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlGnssConfigService::resetGnssSvTypeConfig()
{
    FROM_AIDL_CLIENT();

    const GnssInterface* gnssInterface = getGnssInterface();
    if (nullptr == gnssInterface) {
        LOC_LOGE("Failed to get GNSS Interface");
        return ndk::ScopedAStatus::ok();
    }
    gnssInterface->gnssResetSvTypeConfig();

    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlGnssConfigService::init(
        const std::shared_ptr<ILocAidlGnssConfigServiceCallback>& callback, bool* _aidl_return) {
    FROM_AIDL_CLIENT();
    // Register death recipient
    if (mDeathRecipient == nullptr) {
        mDeathRecipient = make_shared<LocAidlDeathRecipient>(GnssConfigServiceDied);
    } else if (sCallbackIface != nullptr) {
        mDeathRecipient->unregisterFromPeer(sCallbackIface->asBinder().get(), this);
    }
    sCallbackIface = callback;
    if (callback != nullptr) {
        mDeathRecipient->registerToPeer(callback->asBinder().get(), this);
    } else {
        LOC_LOGe("callback is nullptr!");
    }
    // Register for LocationControlCallbacks to get the gnss config info
    mControlCallbacks.responseCb = [](LocationError error, uint32_t id) {};
    mControlCallbacks.collectiveResponseCb =
            [](size_t count, LocationError* errors, uint32_t* ids) {};
    mControlCallbacks.gnssConfigCb = [](uint32_t session_id, const GnssConfig& config) {
        if (config.flags & GNSS_CONFIG_FLAGS_ROBUST_LOCATION_BIT) {
            LocAidlRobustLocationInfo info;
            info.validMask = (uint16_t)config.robustLocationConfig.validMask;
            info.enable = config.robustLocationConfig.enabled;
            info.enableForE911 = config.robustLocationConfig.enabledForE911;
            info.major = config.robustLocationConfig.version.major;
            info.minor = config.robustLocationConfig.version.minor;
            sCallbackIface->getRobustLocationConfigCb(info);
        }
    };
    const GnssInterface* gnssInterface = getGnssInterface();
    if (nullptr == gnssInterface) {
        LOC_LOGE("Failed to get GNSS Interface");
        *_aidl_return = false;;
        return ndk::ScopedAStatus::ok();
    }
    gnssInterface->setControlCallbacks(mControlCallbacks);
    *_aidl_return = true;
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlGnssConfigService::setGnssSvTypeConfig(
        const std::vector<LocAidlGnssConstellationType>& disabledSvTypeList) {

    FROM_AIDL_CLIENT();

    const GnssInterface* gnssInterface = getGnssInterface();
    if (nullptr == gnssInterface) {
        LOC_LOGE("Failed to get GNSS Interface");
        return ndk::ScopedAStatus::ok();
    }

    GnssSvTypeConfig config = {};
    config.size = sizeof(config);
    svTypeVecToConfig(disabledSvTypeList, config);

    gnssInterface->gnssUpdateSvTypeConfig(config);

    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlGnssConfigService::getRobustLocationConfig() {
    FROM_AIDL_CLIENT();

    const GnssInterface* gnssInterface = getGnssInterface();
    if (nullptr == gnssInterface) {
        LOC_LOGE("Failed to get GNSS Interface");
        return ndk::ScopedAStatus::ok();
    }
    GnssConfigFlagsMask mask = GNSS_CONFIG_FLAGS_ROBUST_LOCATION_BIT;
    // need to free the memory used to hold sessionIds
    uint32_t* sessionIds = gnssInterface->gnssGetConfig(mask);
    delete [] sessionIds;

    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlGnssConfigService::setRobustLocationConfig(bool enable,
        bool enableForE911) {
    FROM_AIDL_CLIENT();

    const GnssInterface* gnssInterface = getGnssInterface();
    if (nullptr == gnssInterface) {
        LOC_LOGE("Failed to get GNSS Interface");
        return ndk::ScopedAStatus::ok();
    }
    gnssInterface->configRobustLocation(enable, enableForE911);

    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlGnssConfigService::updateNTRIPGGAConsent(bool consentAccepted) {
    FROM_AIDL_CLIENT();
#ifdef QESDK_ENABLED
    LOC_LOGE("Not Supported in Location SDK");
#else
    const GnssInterface* gnssInterface = getGnssInterface();
    if (nullptr == gnssInterface) {
        LOC_LOGE("Failed to get GNSS Interface");
        return ndk::ScopedAStatus::ok();
    }
    gnssInterface->updateNTRIPGGAConsent(consentAccepted);
#endif

    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlGnssConfigService::enablePPENtripStream(
        const LocAidlNtripConnectionParams& params,
        bool enableRTKEngine) {
    FROM_AIDL_CLIENT();
#ifdef QESDK_ENABLED
    LOC_LOGE("Not Supported in Location SDK");
#else
    const GnssInterface* gnssInterface = getGnssInterface();
    if (nullptr == gnssInterface) {
        LOC_LOGE("Failed to get GNSS Interface");
        return ndk::ScopedAStatus::ok();
    }

    GnssNtripConnectionParams config = {};
    config.size = sizeof(config);
    config.requiresNmeaLocation = params.requiresNmeaLocation;
    config.hostNameOrIp = std::move(params.hostNameOrIp);
    config.mountPoint = std::move(params.mountPoint);
    config.username = std::move(params.username);
    config.password = std::move(params.password);
    config.port = params.port;
    config.useSSL = params.useSSL;
    gnssInterface->enablePPENtripStream(config, enableRTKEngine);
#endif
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlGnssConfigService::disablePPENtripStream() {
    FROM_AIDL_CLIENT();
#ifdef QESDK_ENABLED
    LOC_LOGE("Not Supported in Location SDK");
#else
    const GnssInterface* gnssInterface = getGnssInterface();
    if (nullptr == gnssInterface) {
        LOC_LOGE("Failed to get GNSS Interface");
        return ndk::ScopedAStatus::ok();
    }
    gnssInterface->disablePPENtripStream();
#endif
    return ndk::ScopedAStatus::ok();
}

void LocAidlGnssConfigService::svTypeVecToConfig(
        const std::vector<LocAidlGnssConstellationType>& disabledSvTypeVec,
        GnssSvTypeConfig& config)
{
    for (LocAidlGnssConstellationType svType: disabledSvTypeVec) {

        switch (svType) {
            case LocAidlGnssConstellationType::IRNSS:
                config.blacklistedSvTypesMask |= GNSS_SV_TYPES_MASK_NAVIC_BIT;
                break;
            default:
                // Check if enabled sv type
                if (!svTypeToConfigCommon<LocAidlGnssConstellationType>(svType, config)) {
                    svType = intToGnssConstellation((uint8_t)(~(int32_t)svType));
                    if (LocAidlGnssConstellationType::IRNSS == svType) {
                        config.enabledSvTypesMask |= GNSS_SV_TYPES_MASK_NAVIC_BIT;
                    }
                }
        }
    }
}


template <typename T>
bool LocAidlGnssConfigService::svTypeToMaskUpdate(T svType, GnssSvTypesMask& mask) {
     switch (svType) {
     case T::GPS:
         mask |= GNSS_SV_TYPES_MASK_GPS_BIT;
         return true;
     case T::GLONASS:
         mask |= GNSS_SV_TYPES_MASK_GLO_BIT;
         return true;
     case T::QZSS:
         mask |= GNSS_SV_TYPES_MASK_QZSS_BIT;
         return true;
     case T::BEIDOU:
         mask |= GNSS_SV_TYPES_MASK_BDS_BIT;
         return true;
     case T::GALILEO:
         mask |= GNSS_SV_TYPES_MASK_GAL_BIT;
         return true;
     default:
         return false;
     }
 }

template <typename T>
bool LocAidlGnssConfigService::svTypeToConfigCommon(
        T svType, GnssSvTypeConfig& config) {
    return svTypeToMaskUpdate(svType, config.blacklistedSvTypesMask) ||
            svTypeToMaskUpdate(intToGnssConstellation((uint8_t)(~(int32_t)svType)),
                               config.enabledSvTypesMask);
}

template <typename T>
void LocAidlGnssConfigService::svTypeConfigToVecCommon(const GnssSvTypeConfig& config,
                                                           std::vector<T>& svTypeVec) {
    if (config.blacklistedSvTypesMask & GNSS_SV_TYPES_MASK_GPS_BIT) {
        svTypeVec.push_back(T::GPS);
    }
    if (config.blacklistedSvTypesMask & GNSS_SV_TYPES_MASK_GLO_BIT) {
        svTypeVec.push_back(T::GLONASS);
    }
    if (config.blacklistedSvTypesMask & GNSS_SV_TYPES_MASK_QZSS_BIT) {
        svTypeVec.push_back(T::QZSS);
    }
    if (config.blacklistedSvTypesMask & GNSS_SV_TYPES_MASK_BDS_BIT) {
        svTypeVec.push_back(T::BEIDOU);
    }
    if (config.blacklistedSvTypesMask & GNSS_SV_TYPES_MASK_GAL_BIT) {
        svTypeVec.push_back(T::GALILEO);
    }

    // Set enabled values
    if (config.enabledSvTypesMask & GNSS_SV_TYPES_MASK_GPS_BIT) {
        svTypeVec.push_back((T)
                (~gnssConstellationToIntCommon(T::GPS)));
    }
    if (config.enabledSvTypesMask & GNSS_SV_TYPES_MASK_GLO_BIT) {
        svTypeVec.push_back((T)
                (~gnssConstellationToIntCommon(T::GLONASS)));
    }
    if (config.enabledSvTypesMask & GNSS_SV_TYPES_MASK_QZSS_BIT) {
        svTypeVec.push_back((T)
                (~gnssConstellationToIntCommon(T::QZSS)));
    }
    if (config.enabledSvTypesMask & GNSS_SV_TYPES_MASK_BDS_BIT) {
        svTypeVec.push_back((T)
                (~gnssConstellationToIntCommon(T::BEIDOU)));
    }
    if (config.enabledSvTypesMask & GNSS_SV_TYPES_MASK_GAL_BIT) {
        svTypeVec.push_back((T)
                (~gnssConstellationToIntCommon(T::GALILEO)));
    }
}

void LocAidlGnssConfigService::svTypeConfigToVec(
        const GnssSvTypeConfig& config, std::vector<LocAidlGnssConstellationType>& svTypeVec) {
    if (config.blacklistedSvTypesMask & GNSS_SV_TYPES_MASK_NAVIC_BIT) {
        svTypeVec.push_back(LocAidlGnssConstellationType::IRNSS);
    }
    svTypeConfigToVecCommon<LocAidlGnssConstellationType>(config, svTypeVec);
    // Set enabled values
    if (config.enabledSvTypesMask & GNSS_SV_TYPES_MASK_NAVIC_BIT) {
        svTypeVec.push_back((LocAidlGnssConstellationType)
                (~gnssConstellationToInt(LocAidlGnssConstellationType::IRNSS)));
    }
}

// Helper method for constellation 1.0 and 2.0 combined
LocAidlGnssConstellationType LocAidlGnssConfigService::intToGnssConstellation(int32_t svTypeInt) {

    switch (svTypeInt) {
    case 1:
        return LocAidlGnssConstellationType::GPS;
    case 2:
        return LocAidlGnssConstellationType::SBAS;
    case 3:
        return LocAidlGnssConstellationType::GLONASS;
    case 4:
        return LocAidlGnssConstellationType::QZSS;
    case 5:
        return LocAidlGnssConstellationType::BEIDOU;
    case 6:
        return LocAidlGnssConstellationType::GALILEO;
    case 7:
        return LocAidlGnssConstellationType::IRNSS;
    default:
        return LocAidlGnssConstellationType::UNKNOWN;
    }
}

template <typename T>
int32_t LocAidlGnssConfigService::gnssConstellationToIntCommon(T svType) {
    switch (svType) {
        case T::GPS:
            return 1;
        case T::SBAS:
            return 2;
        case T::GLONASS:
            return 3;
        case T::QZSS:
            return 4;
        case T::BEIDOU:
            return 5;
        case T::GALILEO:
            return 6;
        default:
            return 0;
    }
}

int32_t LocAidlGnssConfigService::gnssConstellationToInt(LocAidlGnssConstellationType svType) {
    switch (svType) {
        case LocAidlGnssConstellationType::IRNSS:
            return 7;
        default:
            return gnssConstellationToIntCommon(svType);
    }
}

}  // namespace implementation
}  // namespace gnss
}  // namespace qti
}  // namespace vendor
}  // namespace aidl
