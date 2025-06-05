/*
 * Copyright (c) 2021-2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#define LOG_TAG "LocSvc_AIDL_vendor.LocAidlGnss"
#define LOG_NDEBUG 0

#include "LocAidlGnss.h"
#include "LocAidlFlpService.h"
#include "LocAidlIzatProvider.h"
#include "LocAidlAGnss.h"
#include "LocAidlGnssNi.h"
#include "LocAidlWiFiDBProvider.h"
#include "LocAidlWiFiDBReceiver.h"
#include "LocAidlWWANDBProvider.h"
#include "LocAidlWWANDBReceiver.h"
#include "LocAidlDebugReportService.h"
#include "LocAidlGnssConfigService.h"
#include "LocAidlRilInfoMonitor.h"
#include "LocAidlIzatConfig.h"
#include "LocAidlGeofenceService.h"
#include "LocAidlIzatSubscription.h"
#include "LocAidlGeocoder.h"
#include "LocAidlEsStatusReceiver.h"
#include "LocAidlQesdkTracking.h"
#include <OSFramework.h>
#include "IzatManager.h"

namespace aidl {
namespace vendor {
namespace qti {
namespace gnss {
namespace implementation {

::ndk::ScopedAStatus LocAidlGnss::getExtensionLocAidlAGnss(
        std::shared_ptr<ILocAidlAGnss>* _aidl_return) {
    FROM_AIDL_CLIENT();
    if (mAGnssVendor == nullptr) {
        mAGnssVendor = SharedRefBase::make<LocAidlAGnss>();
    }
    *_aidl_return = mAGnssVendor;
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlGnss::getExtensionLocAidlDebugReportService(
        std::shared_ptr<ILocAidlDebugReportService>* _aidl_return) {
    FROM_AIDL_CLIENT();
    if (mDebugReportService == nullptr) {
        mDebugReportService = SharedRefBase::make<LocAidlDebugReportService>();
    }
    *_aidl_return = mDebugReportService;
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlGnss::getExtensionLocAidlFlpService(
        std::shared_ptr<ILocAidlFlpService>* _aidl_return) {
    FROM_AIDL_CLIENT();
    if (mGnssFlpServiceProvider == nullptr) {
        mGnssFlpServiceProvider = SharedRefBase::make<LocAidlFlpService>();
    }
    *_aidl_return = mGnssFlpServiceProvider;
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlGnss::getExtensionLocAidlGeofenceService(
        std::shared_ptr<ILocAidlGeofenceService>* _aidl_return) {
    FROM_AIDL_CLIENT();
    if (mGnssGeofenceServiceProvider == nullptr) {
        mGnssGeofenceServiceProvider = SharedRefBase::make<LocAidlGeofenceService>();
    }
    *_aidl_return = mGnssGeofenceServiceProvider;

    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlGnss::getExtensionLocAidlGnssConfigService(
        std::shared_ptr<ILocAidlGnssConfigService>* _aidl_return) {
    FROM_AIDL_CLIENT();
    if (mGnssConfigService == nullptr) {
        mGnssConfigService = SharedRefBase::make<LocAidlGnssConfigService>();
    }
    *_aidl_return = mGnssConfigService;

    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlGnss::getExtensionLocAidlGnssNi(
        std::shared_ptr<ILocAidlGnssNi>* _aidl_return) {
    FROM_AIDL_CLIENT();
    if (mGnssNiVendor == nullptr) {
        mGnssNiVendor = SharedRefBase::make<LocAidlGnssNi>();
    }
    *_aidl_return = mGnssNiVendor;
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlGnss::getExtensionLocAidlIzatConfig(
        std::shared_ptr<ILocAidlIzatConfig>* _aidl_return) {
    FROM_AIDL_CLIENT();
    if (mIzatConfig == nullptr) {
        mIzatConfig = SharedRefBase::make<LocAidlIzatConfig>();
    }
    *_aidl_return = mIzatConfig;

    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlGnss::getExtensionLocAidlIzatFusedProvider(
        std::shared_ptr<ILocAidlIzatProvider>* _aidl_return) {
    FROM_AIDL_CLIENT();
    IIzatManager *pIzatManager = getIzatManager(OSFramework::getOSFramework());
    if (NULL != pIzatManager) {
        IzatManager* izatMgr = static_cast<IzatManager*>(pIzatManager);
        izatMgr->setIzatFusedProviderOverride(true);
    }
    //We don't have Fused Location Provider in LocAidl layer, return nullptr directly
    *_aidl_return = nullptr;
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlGnss::getExtensionLocAidlIzatNetworkProvider(
        std::shared_ptr<ILocAidlIzatProvider>* _aidl_return) {
    FROM_AIDL_CLIENT();
    if (mIzatNetworkProvider == nullptr) {
        mIzatNetworkProvider =
                SharedRefBase::make<LocAidlIzatProvider>(LocAidlIzatStreamType::NETWORK);
    }
    *_aidl_return = mIzatNetworkProvider;
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlGnss::getExtensionLocAidlIzatSubscription(
        std::shared_ptr<ILocAidlIzatSubscription>* _aidl_return) {
    FROM_AIDL_CLIENT();
    if (mIzatSubscription == nullptr) {
        mIzatSubscription = SharedRefBase::make<LocAidlIzatSubscription>();
    }
    *_aidl_return = mIzatSubscription;


    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlGnss::getExtensionLocAidlRilInfoMonitor(
        std::shared_ptr<ILocAidlRilInfoMonitor>* _aidl_return) {
    FROM_AIDL_CLIENT();
    if (mGnssRilInfoMonitor == nullptr) {
        mGnssRilInfoMonitor = SharedRefBase::make<LocAidlRilInfoMonitor>();
    }
    *_aidl_return = mGnssRilInfoMonitor;

    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlGnss::getExtensionLocAidlWWANDBProvider(
        std::shared_ptr<ILocAidlWWANDBProvider>* _aidl_return) {
    FROM_AIDL_CLIENT();
    if (mIzatWWANDBProvider == nullptr) {
        mIzatWWANDBProvider = SharedRefBase::make<LocAidlWWANDBProvider>();
    }
    *_aidl_return = mIzatWWANDBProvider;
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlGnss::getExtensionLocAidlWWANDBReceiver(
        std::shared_ptr<ILocAidlWWANDBReceiver>* _aidl_return) {
    FROM_AIDL_CLIENT();
    if (mIzatWWANDBReceiver == nullptr) {
        mIzatWWANDBReceiver = SharedRefBase::make<LocAidlWWANDBReceiver>();
    }
    *_aidl_return = mIzatWWANDBReceiver;
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlGnss::getExtensionLocAidlWiFiDBProvider(
        std::shared_ptr<ILocAidlWiFiDBProvider>* _aidl_return) {
    FROM_AIDL_CLIENT();
    if (mIzatWiFiDBProvider == nullptr) {
        mIzatWiFiDBProvider = SharedRefBase::make<LocAidlWiFiDBProvider>();
    }
    *_aidl_return = mIzatWiFiDBProvider;
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlGnss::getExtensionLocAidlWiFiDBReceiver(
        std::shared_ptr<ILocAidlWiFiDBReceiver>* _aidl_return) {
    FROM_AIDL_CLIENT();
    if (mIzatWiFiDBReceiver == nullptr) {
        mIzatWiFiDBReceiver = SharedRefBase::make<LocAidlWiFiDBReceiver>();
    }
    *_aidl_return = mIzatWiFiDBReceiver;
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlGnss::getExtensionLocAidlGeocoder(
        std::shared_ptr<ILocAidlGeocoder>* _aidl_return) {
    FROM_AIDL_CLIENT();
    if (mGeocoder == nullptr) {
        mGeocoder = SharedRefBase::make<LocAidlGeocoder>();
    }
    *_aidl_return = mGeocoder;
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlGnss::getExtensionLocAidlEsStatusReceiver(
        std::shared_ptr<ILocAidlEsStatusReceiver>* _aidl_return) {
    FROM_AIDL_CLIENT();
    if (mEsStatusReceiver == nullptr) {
        mEsStatusReceiver = SharedRefBase::make<LocAidlEsStatusReceiver>();
    }
    *_aidl_return = mEsStatusReceiver;

    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlGnss::getExtensionLocAidlQesdkTracking(
        std::shared_ptr<ILocAidlQesdkTracking>* _aidl_return) {
    FROM_AIDL_CLIENT();
    if (mQesdkIface == nullptr) {
        mQesdkIface = SharedRefBase::make<LocAidlQesdkTracking>();
    }
    *_aidl_return = mQesdkIface;

    return ndk::ScopedAStatus::ok();
}
}
}
}
}
}
