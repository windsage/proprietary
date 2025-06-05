/*
 * Copyright (c) 2021-2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <aidl/vendor/qti/gnss/BnLocAidlGnss.h>
#include <log_util.h>

namespace aidl {
namespace vendor {
namespace qti {
namespace gnss {
namespace implementation {

using ::aidl::vendor::qti::gnss::BnLocAidlGnss;
using ::aidl::vendor::qti::gnss::ILocAidlFlpService;
using ::aidl::vendor::qti::gnss::ILocAidlIzatProvider;
using ::aidl::vendor::qti::gnss::ILocAidlIzatProvider;
using ::aidl::vendor::qti::gnss::ILocAidlDebugReportService;
using ::aidl::vendor::qti::gnss::ILocAidlWiFiDBReceiver;
using ::aidl::vendor::qti::gnss::ILocAidlWWANDBProvider;
using ::aidl::vendor::qti::gnss::ILocAidlWiFiDBProvider;
using ::aidl::vendor::qti::gnss::ILocAidlWWANDBReceiver;
using ::aidl::vendor::qti::gnss::ILocAidlGnssConfigService;
using ::aidl::vendor::qti::gnss::ILocAidlRilInfoMonitor;
using ::aidl::vendor::qti::gnss::ILocAidlGeofenceService;
using ::aidl::vendor::qti::gnss::ILocAidlIzatConfig;
using ::aidl::vendor::qti::gnss::ILocAidlIzatSubscription;
using ::aidl::vendor::qti::gnss::ILocAidlAGnss;
using ::aidl::vendor::qti::gnss::ILocAidlGnssNi;
using ::aidl::vendor::qti::gnss::ILocAidlGeocoder;
using ::aidl::vendor::qti::gnss::ILocAidlEsStatusReceiver;
using ::aidl::vendor::qti::gnss::ILocAidlQesdkTracking;

class LocAidlGnss : public BnLocAidlGnss {
    ::ndk::ScopedAStatus getExtensionLocAidlAGnss(
            std::shared_ptr<ILocAidlAGnss>* _aidl_return) override;
    ::ndk::ScopedAStatus getExtensionLocAidlDebugReportService(
            std::shared_ptr<ILocAidlDebugReportService>* _aidl_return) override;
    ::ndk::ScopedAStatus getExtensionLocAidlFlpService(
            std::shared_ptr<ILocAidlFlpService>* _aidl_return) override;
    ::ndk::ScopedAStatus getExtensionLocAidlGeofenceService(
            std::shared_ptr<ILocAidlGeofenceService>* _aidl_return) override;
    ::ndk::ScopedAStatus getExtensionLocAidlGnssConfigService(
            std::shared_ptr<ILocAidlGnssConfigService>* _aidl_return) override;
    ::ndk::ScopedAStatus getExtensionLocAidlGnssNi(
            std::shared_ptr<ILocAidlGnssNi>* _aidl_return) override;
    ::ndk::ScopedAStatus getExtensionLocAidlIzatConfig(
            std::shared_ptr<ILocAidlIzatConfig>* _aidl_return) override;
    ::ndk::ScopedAStatus getExtensionLocAidlIzatFusedProvider(
            std::shared_ptr<ILocAidlIzatProvider>* _aidl_return) override;
    ::ndk::ScopedAStatus getExtensionLocAidlIzatNetworkProvider(
            std::shared_ptr<ILocAidlIzatProvider>* _aidl_return) override;
    ::ndk::ScopedAStatus getExtensionLocAidlIzatSubscription(
            std::shared_ptr<ILocAidlIzatSubscription>* _aidl_return) override;
    ::ndk::ScopedAStatus getExtensionLocAidlRilInfoMonitor(
            std::shared_ptr<ILocAidlRilInfoMonitor>* _aidl_return) override;
    ::ndk::ScopedAStatus getExtensionLocAidlWWANDBProvider(
            std::shared_ptr<ILocAidlWWANDBProvider>* _aidl_return) override;
    ::ndk::ScopedAStatus getExtensionLocAidlWWANDBReceiver(
            std::shared_ptr<ILocAidlWWANDBReceiver>* _aidl_return) override;
    ::ndk::ScopedAStatus getExtensionLocAidlWiFiDBProvider(
            std::shared_ptr<ILocAidlWiFiDBProvider>* _aidl_return) override;
    ::ndk::ScopedAStatus getExtensionLocAidlWiFiDBReceiver(
            std::shared_ptr<ILocAidlWiFiDBReceiver>* _aidl_return) override;
    ::ndk::ScopedAStatus getExtensionLocAidlGeocoder(
            std::shared_ptr<ILocAidlGeocoder>* _aidl_return) override;
    ::ndk::ScopedAStatus getExtensionLocAidlEsStatusReceiver(
            std::shared_ptr<ILocAidlEsStatusReceiver>* _aidl_return) override;
    ::ndk::ScopedAStatus getExtensionLocAidlQesdkTracking(
            std::shared_ptr<ILocAidlQesdkTracking>* _aidl_return) override;

private:
    std::shared_ptr<ILocAidlFlpService> mGnssFlpServiceProvider = nullptr;
    std::shared_ptr<ILocAidlIzatProvider> mIzatFusedProvider = nullptr;
    std::shared_ptr<ILocAidlIzatProvider> mIzatNetworkProvider = nullptr;
    std::shared_ptr<ILocAidlDebugReportService> mDebugReportService = nullptr;
    std::shared_ptr<ILocAidlWiFiDBReceiver> mIzatWiFiDBReceiver = nullptr;
    std::shared_ptr<ILocAidlWWANDBProvider> mIzatWWANDBProvider = nullptr;
    std::shared_ptr<ILocAidlWiFiDBProvider> mIzatWiFiDBProvider = nullptr;
    std::shared_ptr<ILocAidlWWANDBReceiver> mIzatWWANDBReceiver = nullptr;
    std::shared_ptr<ILocAidlGnssConfigService> mGnssConfigService = nullptr;
    std::shared_ptr<ILocAidlRilInfoMonitor> mGnssRilInfoMonitor = nullptr;
    std::shared_ptr<ILocAidlGeofenceService> mGnssGeofenceServiceProvider = nullptr;
    std::shared_ptr<ILocAidlIzatConfig> mIzatConfig = nullptr;
    std::shared_ptr<ILocAidlIzatSubscription> mIzatSubscription = nullptr;
    std::shared_ptr<ILocAidlAGnss> mAGnssVendor = nullptr;
    std::shared_ptr<ILocAidlGnssNi> mGnssNiVendor = nullptr;
    std::shared_ptr<ILocAidlGeocoder> mGeocoder = nullptr;
    std::shared_ptr<ILocAidlEsStatusReceiver> mEsStatusReceiver = nullptr;
    std::shared_ptr<ILocAidlQesdkTracking> mQesdkIface = nullptr;
};
}
}
}
}
}
