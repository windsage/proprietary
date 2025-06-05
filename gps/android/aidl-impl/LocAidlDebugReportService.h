/*
 * Copyright (c) 2021-2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <aidl/vendor/qti/gnss/BnLocAidlDebugReportService.h>
#include "SystemStatus.h"
#include <aidl/vendor/qti/gnss/LocAidlSystemStatusBestPosition.h>
#include <aidl/vendor/qti/gnss/LocAidlSystemStatusEphemeris.h>
#include <aidl/vendor/qti/gnss/LocAidlSystemStatusErrRecovery.h>
#include <aidl/vendor/qti/gnss/LocAidlSystemStatusInjectedPosition.h>
#include <aidl/vendor/qti/gnss/LocAidlSystemStatusLocation.h>
#include <aidl/vendor/qti/gnss/LocAidlSystemStatusNav.h>
#include <aidl/vendor/qti/gnss/LocAidlSystemStatusNavData.h>
#include <aidl/vendor/qti/gnss/LocAidlSystemStatusPdr.h>
#include <aidl/vendor/qti/gnss/LocAidlSystemStatusPositionFailure.h>
#include <aidl/vendor/qti/gnss/LocAidlSystemStatusReports.h>
#include <aidl/vendor/qti/gnss/LocAidlSystemStatusRfAndParams.h>
#include <aidl/vendor/qti/gnss/LocAidlSystemStatusSvHealth.h>
#include <aidl/vendor/qti/gnss/LocAidlSystemStatusTimeAndClock.h>
#include <aidl/vendor/qti/gnss/LocAidlSystemStatusXoState.h>
#include <aidl/vendor/qti/gnss/LocAidlSystemStatusXtra.h>
#include <location_interface.h>

namespace aidl {
namespace vendor {
namespace qti {
namespace gnss {
namespace implementation {

using ::aidl::vendor::qti::gnss::LocAidlSystemStatusBestPosition;
using ::aidl::vendor::qti::gnss::LocAidlSystemStatusEphemeris;
using ::aidl::vendor::qti::gnss::LocAidlSystemStatusErrRecovery;
using ::aidl::vendor::qti::gnss::LocAidlSystemStatusInjectedPosition;
using ::aidl::vendor::qti::gnss::LocAidlSystemStatusLocation;
using ::aidl::vendor::qti::gnss::LocAidlSystemStatusNav;
using ::aidl::vendor::qti::gnss::LocAidlSystemStatusNavData;
using ::aidl::vendor::qti::gnss::LocAidlSystemStatusPdr;
using ::aidl::vendor::qti::gnss::LocAidlSystemStatusPositionFailure;
using ::aidl::vendor::qti::gnss::LocAidlSystemStatusReports;
using ::aidl::vendor::qti::gnss::LocAidlSystemStatusRfAndParams;
using ::aidl::vendor::qti::gnss::LocAidlSystemStatusSvHealth;
using ::aidl::vendor::qti::gnss::LocAidlSystemStatusTimeAndClock;
using ::aidl::vendor::qti::gnss::LocAidlSystemStatusXoState;
using ::aidl::vendor::qti::gnss::LocAidlSystemStatusXtra;
using ::aidl::vendor::qti::gnss::ILocAidlDebugReportServiceCallback;
using namespace loc_core;
struct LocAidlDebugReportService : public BnLocAidlDebugReportService {
    static std::shared_ptr<ILocAidlDebugReportServiceCallback> sCallbackIface;
    LocAidlDebugReportService() = default;
    virtual ~LocAidlDebugReportService() = default;
    // Methods from ::vendor::qti::gnss::ILocAidlDebugReportService follow.
    ::ndk::ScopedAStatus init(bool* _aidl_return) override;
    ::ndk::ScopedAStatus deinit(bool* _aidl_return) override;
    ::ndk::ScopedAStatus getReport(int32_t maxReports,
            LocAidlSystemStatusReports* _aidl_return) override;
    ::ndk::ScopedAStatus registerXtraStatusListener(
            const std::shared_ptr<ILocAidlDebugReportServiceCallback>& callback) override;
    ::ndk::ScopedAStatus unregisterXtraStatusListener() override;
    ::ndk::ScopedAStatus getXtraStatus() override;

    void handleAidlClientSsr();
private:
    SystemStatus* mSystemStatus;
    GnssInterface* mGnssInterface = nullptr;
    bool mGetGnssInterfaceFailed = false;
    LocationControlCallbacks mControlCallbacks;
    static std::shared_ptr<LocAidlDeathRecipient> mDeathRecipient;

private:
    const GnssInterface* getGnssInterface();
    void getReport(LocAidlSystemStatusReports& hidlReports,
            int32_t maxReports);
    void populateLocationReport(
            LocAidlSystemStatusReports& hidlReport,
            SystemStatusReports& report, int32_t maxReports);
    void populateTimeAndClockReport(
            LocAidlSystemStatusReports& hidlReport,
            SystemStatusReports& report, int32_t maxReports);
    void populateXoStateReport(
            LocAidlSystemStatusReports& hidlReport,
            SystemStatusReports& report, int32_t maxReports);
    void populateRfParamsReport(
            LocAidlSystemStatusReports& hidlReport,
            SystemStatusReports& report, int32_t maxReports);
    void populateErrRecoveryReport(
            LocAidlSystemStatusReports& hidlReport,
            SystemStatusReports& report, int32_t maxReports);
    void populateInjectedPositionReport(
            LocAidlSystemStatusReports& hidlReport,
            SystemStatusReports& report, int32_t maxReports);
    void populateBestPositionReport(
            LocAidlSystemStatusReports& hidlReport,
            SystemStatusReports& report, int32_t maxReports);
    void populateXtraReport(
            LocAidlSystemStatusReports& hidlReport,
            SystemStatusReports& report, int32_t maxReports);
    void populateEphemerisReport(
            LocAidlSystemStatusReports& hidlReport,
            SystemStatusReports& report, int32_t maxReports);
    void populateSvHealthReport(
            LocAidlSystemStatusReports& hidlReport,
            SystemStatusReports& report, int32_t maxReports);
    void populatePdrReport(
            LocAidlSystemStatusReports& hidlReport,
            SystemStatusReports& report, int32_t maxReports);
    void populateNavDataReport(
            LocAidlSystemStatusReports& hidlReport,
            SystemStatusReports& report, int32_t maxReports);
    void populatePositionFailureReport(
            LocAidlSystemStatusReports& hidlReport,
            SystemStatusReports& report, int32_t maxReports);
};

}  // namespace implementation
}  // namespace aidl
}  // namespace gnss
}  // namespace qti
}  // namespace vendor
