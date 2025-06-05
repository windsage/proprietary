/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define LOG_TAG "LocSvc_AIDL_DebugReportService"
#define LOG_NDEBUG 0

#include <log_util.h>
#include "loc_misc_utils.h"
#include "LocAidlUtils.h"
#include "LocAidlDebugReportService.h"
#include <vector>

namespace aidl {
namespace vendor {
namespace qti {
namespace gnss {
namespace implementation {

using namespace loc_core;
typedef const GnssInterface* (getLocationInterface)();
std::shared_ptr<LocAidlDeathRecipient> LocAidlDebugReportService::mDeathRecipient = nullptr;
std::shared_ptr<ILocAidlDebugReportServiceCallback> LocAidlDebugReportService::sCallbackIface =
        nullptr;
void DebugReportServiceDied(void* cookie) {
    LOC_LOGe("ILocAidlDegbugReportServiceCallback died.");
    auto thiz = static_cast<LocAidlDebugReportService*>(cookie);
    thiz->handleAidlClientSsr();
}
const GnssInterface* LocAidlDebugReportService::getGnssInterface() {
    if (nullptr == mGnssInterface && !mGetGnssInterfaceFailed) {
        LOC_LOGd("loading libgnss.so::getGnssInterface ...");
        void * handle = nullptr;
        getLocationInterface* getter =
                (getLocationInterface*)dlGetSymFromLib(handle, "libgnss.so", "getGnssInterface");

        if (NULL == getter) {
            mGetGnssInterfaceFailed = true;
        } else {
            mGnssInterface = (GnssInterface*)(*getter)();
        }
    }
    return mGnssInterface;
}

void LocAidlDebugReportService::handleAidlClientSsr() {
    if (sCallbackIface != nullptr && mDeathRecipient != nullptr) {
        mDeathRecipient->unregisterFromPeer(sCallbackIface->asBinder().get(), this);
    }
    mDeathRecipient = nullptr;
    sCallbackIface = nullptr;
}
// Methods from ::vendor::qti::gnss::ILocAidlDebugReportService follow.
::ndk::ScopedAStatus LocAidlDebugReportService::init(bool* _aidl_return) {
    if (mSystemStatus == NULL) {
        mSystemStatus = SystemStatus::getInstance(NULL);
        if (mSystemStatus == NULL) {
            LOC_LOGE("Failed to get SystemStatus");
            *_aidl_return = false;
            return ndk::ScopedAStatus::ok();
        }
        LOC_LOGE("SystemStatus instance: %p", mSystemStatus);
    } else {
        LOC_LOGE("system status already available !");
    }
    // Register for LocationControlCallbacks to get the gnss config info
    mControlCallbacks.responseCb = [](LocationError error, uint32_t id) {};
    mControlCallbacks.collectiveResponseCb =
            [](size_t count, LocationError* errors, uint32_t* ids) {};
    mControlCallbacks.xtraStatusCb = [](uint32_t session_id, const XtraStatus& xtraStatus) {
        LOC_LOGd("xtraStatusCb, session id: %d", session_id);
        LocAidlXtraStatus xtraStatusAidl = {};
        xtraStatusAidl.mEnabled = xtraStatus.featureEnabled;
        xtraStatusAidl.mStatus =(LocAidlXtraDataStatus)xtraStatus.xtraDataStatus;
        xtraStatusAidl.mValidityHrs = xtraStatus.xtraValidForHours;
        xtraStatusAidl.mLastDownloadStatus = xtraStatus.lastDownloadReasonCode;
        if (sCallbackIface != nullptr) {
            sCallbackIface->onXtraStatusChanged(xtraStatusAidl);
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

::ndk::ScopedAStatus LocAidlDebugReportService::deinit(bool* _aidl_return) {
    /* NOOP */
    *_aidl_return = true;
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus LocAidlDebugReportService::registerXtraStatusListener(
        const std::shared_ptr<ILocAidlDebugReportServiceCallback>& callback) {
    ENTRY_LOG();
    const GnssInterface* gnssInterface = getGnssInterface();
    // Register death recipient
    if (mDeathRecipient == nullptr) {
        mDeathRecipient = make_shared<LocAidlDeathRecipient>(DebugReportServiceDied);
    } else if (sCallbackIface != nullptr) {
        mDeathRecipient->unregisterFromPeer(sCallbackIface->asBinder().get(), this);
    }
    sCallbackIface = callback;
    if (callback != nullptr) {
        mDeathRecipient->registerToPeer(callback->asBinder().get(), this);
    }
    if (nullptr == gnssInterface) {
        LOC_LOGe("Failed to get GNSS Interface");
        return ndk::ScopedAStatus::ok();
    }
    gnssInterface->gnssRegisterXtraStatusUpdate(true);
    return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus LocAidlDebugReportService::unregisterXtraStatusListener() {
    ENTRY_LOG();
    const GnssInterface* gnssInterface = getGnssInterface();
    if (nullptr == gnssInterface) {
        LOC_LOGe("Failed to get GNSS Interface");
        return ndk::ScopedAStatus::ok();
    }
    gnssInterface->gnssRegisterXtraStatusUpdate(false);
    return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus LocAidlDebugReportService::getXtraStatus() {
    ENTRY_LOG();
    const GnssInterface* gnssInterface = getGnssInterface();
    if (nullptr == gnssInterface) {
        LOC_LOGe("Failed to get GNSS Interface");
        return ndk::ScopedAStatus::ok();
    }
    if (nullptr == sCallbackIface) {
        LOC_LOGe("LocAidlDebugReportServiceCallback not registered yet");
        return ndk::ScopedAStatus::ok();
    }
    gnssInterface->gnssGetXtraStatus();
    return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus LocAidlDebugReportService::getReport(int32_t maxReports,
        ::aidl::vendor::qti::gnss::LocAidlSystemStatusReports* _aidl_return) {
    LocAidlSystemStatusReports hidlReports = {};
    hidlReports.mSuccess = false;

    if (maxReports <= 0) {
        LOC_LOGE("Invalid maxReports %d", maxReports);
        *_aidl_return = hidlReports;
        return ndk::ScopedAStatus::ok();
    }
    if (mSystemStatus == NULL) {
        LOC_LOGE("mSystemStatus NULL");
        *_aidl_return = hidlReports;
        return ndk::ScopedAStatus::ok();
    }

    SystemStatusReports systemReports;
    mSystemStatus->getReport(systemReports, maxReports == 1);

    populateLocationReport(hidlReports, systemReports, maxReports);
    populateTimeAndClockReport(hidlReports, systemReports, maxReports);
    populateXoStateReport(hidlReports, systemReports, maxReports);
    populateRfParamsReport(hidlReports, systemReports, maxReports);
    populateErrRecoveryReport(hidlReports, systemReports, maxReports);
    populateInjectedPositionReport(hidlReports, systemReports, maxReports);
    populateBestPositionReport(hidlReports, systemReports, maxReports);
    populateXtraReport(hidlReports, systemReports, maxReports);
    populateEphemerisReport(hidlReports, systemReports, maxReports);
    populateSvHealthReport(hidlReports, systemReports, maxReports);
    populatePdrReport(hidlReports, systemReports, maxReports);
    populateNavDataReport(hidlReports, systemReports, maxReports);
    populatePositionFailureReport(hidlReports, systemReports, maxReports);

    hidlReports.mSuccess = true;
    *_aidl_return = hidlReports;
    return ndk::ScopedAStatus::ok();
}

void LocAidlDebugReportService::populateLocationReport(
        LocAidlSystemStatusReports& /*hidlReports*/,
        SystemStatusReports& /*systemReports*/, int32_t /*maxReports*/) {

    /* Not yet supported */
}

void LocAidlDebugReportService::populateTimeAndClockReport(
        LocAidlSystemStatusReports& hidlReports,
        SystemStatusReports& systemReports, int32_t maxReports) {

    int count = systemReports.mTimeAndClock.size();
    count = count < maxReports ? count : maxReports;
    if (count <= 0) {
        return;
    }
    hidlReports.mTimeAndClockVec.resize(count);

    for (auto p = make_pair(0, systemReports.mTimeAndClock.begin());
        (p.first < maxReports) && (p.second != systemReports.mTimeAndClock.end());
        p.first++, ++p.second) {

        LocAidlSystemStatusTimeAndClock& status = hidlReports.mTimeAndClockVec[p.first];

        status.mUtcTime.tv_sec = (*p.second).mUtcTime.tv_sec;
        status.mUtcTime.tv_nsec = (*p.second).mUtcTime.tv_nsec;

        status.mUtcReported.tv_sec = (*p.second).mUtcReported.tv_sec;
        status.mUtcReported.tv_nsec = (*p.second).mUtcReported.tv_nsec;

        status.mGpsWeek = (*p.second).mGpsWeek;
        status.mGpsTowMs = (*p.second).mGpsTowMs;
        status.mTimeValid = (*p.second).mTimeValid;
        status.mTimeSource = (*p.second).mTimeSource;
        status.mTimeUnc = (*p.second).mTimeUnc;
        status.mClockFreqBias =  (*p.second).mClockFreqBias;
        status.mClockFreqBiasUnc = (*p.second).mClockFreqBiasUnc;
        status.mLeapSeconds = (*p.second).mLeapSeconds;
        status.mLeapSecUnc = (*p.second).mLeapSecUnc;
    }
}

void LocAidlDebugReportService::populateXoStateReport(
        LocAidlSystemStatusReports& hidlReports,
        SystemStatusReports& systemReports, int32_t maxReports) {

    int count = systemReports.mXoState.size();
    count = count < maxReports ? count : maxReports;
    if (count <= 0) {
        return;
    }
    hidlReports.mXoStateVec.resize(count);

    for (auto p = make_pair(0, systemReports.mXoState.begin());
         (p.first < maxReports) && (p.second != systemReports.mXoState.end());
         p.first++, ++p.second) {

        LocAidlSystemStatusXoState& status = hidlReports.mXoStateVec[p.first];

        status.mUtcTime.tv_sec = (*p.second).mUtcTime.tv_sec;
        status.mUtcTime.tv_nsec = (*p.second).mUtcTime.tv_nsec;

        status.mUtcReported.tv_sec = (*p.second).mUtcReported.tv_sec;
        status.mUtcReported.tv_nsec = (*p.second).mUtcReported.tv_nsec;

        status.mXoState = (*p.second).mXoState;
    }
}

void LocAidlDebugReportService::populateRfParamsReport(
        LocAidlSystemStatusReports& hidlReports,
        SystemStatusReports& systemReports, int32_t maxReports) {

    int count = systemReports.mRfAndParams.size();
    std::vector<int64_t> jammerInd(GNSS_LOC_MAX_NUMBER_OF_SIGNAL_TYPES);
    std::vector<int64_t> agc(GNSS_LOC_MAX_NUMBER_OF_SIGNAL_TYPES);
    count = count < maxReports ? count : maxReports;
    if (count <= 0) {
        return;
    }
    hidlReports.mRfAndParamsVec.resize(count);
    hidlReports.mRfAndParamsVec.resize(count);

    for (auto p = make_pair(0, systemReports.mRfAndParams.begin());
         (p.first < maxReports) && (p.second != systemReports.mRfAndParams.end());
         p.first++, ++p.second) {
        LocAidlSystemStatusRfAndParams& status = hidlReports.mRfAndParamsVec[p.first];

        status.mUtcTime.tv_sec = (*p.second).mUtcTime.tv_sec;
        status.mUtcTime.tv_nsec = (*p.second).mUtcTime.tv_nsec;

        status.mUtcReported.tv_sec = (*p.second).mUtcReported.tv_sec;
        status.mUtcReported.tv_nsec = (*p.second).mUtcReported.tv_nsec;

        status.mPgaGain = (*p.second).mPgaGain;
        status.mAdcI = (*p.second).mAdcI;
        status.mAdcQ = (*p.second).mAdcQ;
        status.mJammerGps =(*p.second).mJammerGps;
        status.mJammerGlo = (*p.second).mJammerGlo;
        status.mJammerBds = (*p.second).mJammerBds;
        status.mJammerGal = (*p.second).mJammerGal;
        status.mGpsBpAmpI = (*p.second).mGpsBpAmpI;
        status.mGpsBpAmpQ = (*p.second).mGpsBpAmpQ;
        status.mGloBpAmpI = (*p.second).mGloBpAmpI;
        status.mGloBpAmpQ = (*p.second).mGloBpAmpQ;
        status.mBdsBpAmpI = (*p.second).mBdsBpAmpI;
        status.mBdsBpAmpQ = (*p.second).mBdsBpAmpQ;
        status.mGalBpAmpI = (*p.second).mGalBpAmpI;
        status.mGalBpAmpQ = (*p.second).mGalBpAmpQ;
        status.mJammedSignalsMask = (*p.second).mJammedSignalsMask;

        for (int i = 0; i < GNSS_LOC_MAX_NUMBER_OF_SIGNAL_TYPES &&
                i < ((*p.second).mJammerInd.size()); i++) {
            jammerInd[i] = (*p.second).mJammerInd[i];
            agc[i] = -((*p.second).mJammerInd[i]);
        }
        status.mJammerInd = jammerInd;
        status.mAgc = agc;
    }
}

void LocAidlDebugReportService::populateErrRecoveryReport(
        LocAidlSystemStatusReports& hidlReports,
        SystemStatusReports& systemReports, int32_t maxReports) {

    int count = systemReports.mErrRecovery.size();
    count = count < maxReports ? count : maxReports;
    if (count <= 0) {
        return;
    }
    hidlReports.mErrRecoveryVec.resize(count);

    for (auto p = make_pair(0, systemReports.mErrRecovery.begin());
         (p.first < maxReports) && (p.second != systemReports.mErrRecovery.end());
         p.first++, ++p.second) {

        LocAidlSystemStatusErrRecovery& status = hidlReports.mErrRecoveryVec[p.first];

        status.mUtcTime.tv_sec = (*p.second).mUtcTime.tv_sec;
        status.mUtcTime.tv_nsec = (*p.second).mUtcTime.tv_nsec;

        status.mUtcReported.tv_sec = (*p.second).mUtcReported.tv_sec;
        status.mUtcReported.tv_nsec = (*p.second).mUtcReported.tv_nsec;
    }
}

void LocAidlDebugReportService::populateInjectedPositionReport(
        LocAidlSystemStatusReports& hidlReports,
        SystemStatusReports& systemReports, int32_t maxReports) {

    int count = systemReports.mInjectedPosition.size();
    count = count < maxReports ? count : maxReports;
    if (count <= 0) {
        return;
    }
    hidlReports.mInjectedPositionVec.resize(count);

    for (auto p = make_pair(0, systemReports.mInjectedPosition.begin());
         (p.first < maxReports) && (p.second != systemReports.mInjectedPosition.end());
         p.first++, ++p.second) {

        LocAidlSystemStatusInjectedPosition& status =
                hidlReports.mInjectedPositionVec[p.first];

        status.mUtcTime.tv_sec = (*p.second).mUtcTime.tv_sec;
        status.mUtcTime.tv_nsec = (*p.second).mUtcTime.tv_nsec;

        status.mUtcReported.tv_sec = (*p.second).mUtcReported.tv_sec;
        status.mUtcReported.tv_nsec = (*p.second).mUtcReported.tv_nsec;

        status.mEpiValidity = (*p.second).mEpiValidity;
        status.mEpiLat = (*p.second).mEpiLat;
        status.mEpiLon = (*p.second).mEpiLon;
        status.mEpiHepe = (*p.second).mEpiHepe;
        status.mEpiAlt = (*p.second).mEpiAlt;
        status.mEpiAltUnc = (*p.second).mEpiAltUnc;
        status.mEpiSrc = (*p.second).mEpiSrc;
    }
}

void LocAidlDebugReportService::populateBestPositionReport(
        LocAidlSystemStatusReports& hidlReports,
        SystemStatusReports& systemReports, int32_t maxReports) {

    int count = systemReports.mBestPosition.size();
    count = count < maxReports ? count : maxReports;
    if (count <= 0) {
        return;
    }
    hidlReports.mBestPositionVec.resize(count);

    for (auto p = make_pair(0, systemReports.mBestPosition.begin());
         (p.first < maxReports) && (p.second != systemReports.mBestPosition.end());
         p.first++, ++p.second) {

        LocAidlSystemStatusBestPosition& status = hidlReports.mBestPositionVec[p.first];

        status.mUtcTime.tv_sec = (*p.second).mUtcTime.tv_sec;
        status.mUtcTime.tv_nsec = (*p.second).mUtcTime.tv_nsec;

        status.mUtcReported.tv_sec = (*p.second).mUtcReported.tv_sec;
        status.mUtcReported.tv_nsec = (*p.second).mUtcReported.tv_nsec;

        status.mBestLat = (*p.second).mBestLat;
        status.mBestLon = (*p.second).mBestLon;
        status.mBestHepe = (*p.second).mBestHepe;
        status.mBestAlt = (*p.second).mBestAlt;
        status.mBestAltUnc = (*p.second).mBestAltUnc;
    }
}

void LocAidlDebugReportService::populateXtraReport(
        LocAidlSystemStatusReports& hidlReports,
        SystemStatusReports& systemReports, int32_t maxReports) {

    int count = systemReports.mXtra.size();
    count = count < maxReports ? count : maxReports;
    if (count <= 0) {
        return;
    }
    hidlReports.mXtraVec.resize(count);

    for (auto p = make_pair(0, systemReports.mXtra.begin());
         (p.first < maxReports) && (p.second != systemReports.mXtra.end());
         p.first++, ++p.second) {

        LocAidlSystemStatusXtra& status = hidlReports.mXtraVec[p.first];

        status.mUtcTime.tv_sec = (*p.second).mUtcTime.tv_sec;
        status.mUtcTime.tv_nsec = (*p.second).mUtcTime.tv_nsec;

        status.mUtcReported.tv_sec = (*p.second).mUtcReported.tv_sec;
        status.mUtcReported.tv_nsec = (*p.second).mUtcReported.tv_nsec;

        status.mXtraValidMask = (*p.second).mXtraValidMask;
        status.mGpsXtraAge = (*p.second).mGpsXtraAge;
        status.mGloXtraAge = (*p.second).mGloXtraAge;
        status.mBdsXtraAge = (*p.second).mBdsXtraAge;
        status.mGalXtraAge = (*p.second).mGalXtraAge;
        status.mQzssXtraAge = (*p.second).mQzssXtraAge;
        status.mGpsXtraValid = (*p.second).mGpsXtraValid;
        status.mGloXtraValid = (*p.second).mGloXtraValid;
        status.mBdsXtraValid = (*p.second).mBdsXtraValid;
        status.mGalXtraValid =(*p.second).mGalXtraValid;
        status.mQzssXtraValid = (*p.second).mQzssXtraValid;
    }
}

void LocAidlDebugReportService::populateEphemerisReport(
        LocAidlSystemStatusReports& hidlReports,
        SystemStatusReports& systemReports, int32_t maxReports) {

    int count = systemReports.mEphemeris.size();
    count = count < maxReports ? count : maxReports;
    if (count <= 0) {
        return;
    }
    hidlReports.mEphemerisVec.resize(count);

    for (auto p = make_pair(0, systemReports.mEphemeris.begin());
         (p.first < maxReports) && (p.second != systemReports.mEphemeris.end());
         p.first++, ++p.second) {

        LocAidlSystemStatusEphemeris& status = hidlReports.mEphemerisVec[p.first];

        status.mUtcTime.tv_sec = (*p.second).mUtcTime.tv_sec;
        status.mUtcTime.tv_nsec = (*p.second).mUtcTime.tv_nsec;

        status.mUtcReported.tv_sec = (*p.second).mUtcReported.tv_sec;
        status.mUtcReported.tv_nsec = (*p.second).mUtcReported.tv_nsec;

        status.mGpsEpheValid = (*p.second).mGpsEpheValid;
        status.mGloEpheValid = (*p.second).mGloEpheValid;
        status.mBdsEpheValid = (*p.second).mBdsEpheValid;
        status.mGalEpheValid = (*p.second).mGalEpheValid;
        status.mQzssEpheValid = (*p.second).mQzssEpheValid;
    }
}

void LocAidlDebugReportService::populateSvHealthReport(
        LocAidlSystemStatusReports& hidlReports,
        SystemStatusReports& systemReports, int32_t maxReports) {

    int count = systemReports.mSvHealth.size();
    count = count < maxReports ? count : maxReports;
    if (count <= 0) {
        return;
    }
    hidlReports.mSvHealthVec.resize(count);

    for (auto p = make_pair(0, systemReports.mSvHealth.begin());
         (p.first < maxReports) && (p.second != systemReports.mSvHealth.end());
         p.first++, ++p.second) {

        LocAidlSystemStatusSvHealth& status = hidlReports.mSvHealthVec[p.first];

        status.mUtcTime.tv_sec = (*p.second).mUtcTime.tv_sec;
        status.mUtcTime.tv_nsec = (*p.second).mUtcTime.tv_nsec;

        status.mUtcReported.tv_sec = (*p.second).mUtcReported.tv_sec;
        status.mUtcReported.tv_nsec = (*p.second).mUtcReported.tv_nsec;

        status.mGpsGoodMask = (*p.second).mGpsGoodMask;
        status.mGloGoodMask = (*p.second).mGloGoodMask;
        status.mBdsGoodMask = (*p.second).mBdsGoodMask;
        status.mGalGoodMask = (*p.second).mGalGoodMask;
        status.mQzssGoodMask = (*p.second).mQzssGoodMask;

        status.mGpsBadMask = (*p.second).mGpsBadMask;
        status.mGloBadMask = (*p.second).mGloBadMask;
        status.mBdsBadMask = (*p.second).mBdsBadMask;
        status.mGalBadMask = (*p.second).mGalBadMask;
        status.mQzssBadMask = (*p.second).mQzssBadMask;

        status.mGpsUnknownMask = (*p.second).mGpsUnknownMask;
        status.mGloUnknownMask = (*p.second).mGloUnknownMask;
        status.mBdsUnknownMask = (*p.second).mBdsUnknownMask;
        status.mGalUnknownMask = (*p.second).mGalUnknownMask;
        status.mQzssUnknownMask = (*p.second).mQzssUnknownMask;
    }
}

void LocAidlDebugReportService::populatePdrReport(
        LocAidlSystemStatusReports& hidlReports,
        SystemStatusReports& systemReports, int32_t maxReports) {

    int count = systemReports.mPdr.size();
    count = count < maxReports ? count : maxReports;
    if (count <= 0) {
        return;
    }
    hidlReports.mPdrVec.resize(count);

    for (auto p = make_pair(0, systemReports.mPdr.begin());
         (p.first < maxReports) && (p.second != systemReports.mPdr.end());
         p.first++, ++p.second) {

        LocAidlSystemStatusPdr& status = hidlReports.mPdrVec[p.first];

        status.mUtcTime.tv_sec = (*p.second).mUtcTime.tv_sec;
        status.mUtcTime.tv_nsec = (*p.second).mUtcTime.tv_nsec;

        status.mUtcReported.tv_sec = (*p.second).mUtcReported.tv_sec;
        status.mUtcReported.tv_nsec = (*p.second).mUtcReported.tv_nsec;

        status.mFixInfoMask = (*p.second).mFixInfoMask;
    }
}

void LocAidlDebugReportService::populateNavDataReport(
        LocAidlSystemStatusReports& /*hidlReports*/,
        SystemStatusReports& /*systemReports*/, int32_t /*maxReports*/) {

    /* Not yet supported */
}

void LocAidlDebugReportService::populatePositionFailureReport(
        LocAidlSystemStatusReports& hidlReports,
        SystemStatusReports& systemReports, int32_t maxReports) {

    int count = systemReports.mPositionFailure.size();
    count = count < maxReports ? count : maxReports;
    if (count <= 0) {
        return;
    }
    hidlReports.mPositionFailureVec.resize(count);

    for (auto p = make_pair(0, systemReports.mPositionFailure.begin());
         (p.first < maxReports) && (p.second != systemReports.mPositionFailure.end());
         p.first++, ++p.second) {

        LocAidlSystemStatusPositionFailure& status =
                hidlReports.mPositionFailureVec[p.first];

        status.mUtcTime.tv_sec = (*p.second).mUtcTime.tv_sec;
        status.mUtcTime.tv_nsec = (*p.second).mUtcTime.tv_nsec;

        status.mUtcReported.tv_sec = (*p.second).mUtcReported.tv_sec;
        status.mUtcReported.tv_nsec = (*p.second).mUtcReported.tv_nsec;

        status.mFixInfoMask = (*p.second).mFixInfoMask;
        status.mHepeLimit = (*p.second).mHepeLimit;
    }
}

}  // namespace implementation
}  // namespace aidl
}  // namespace gnss
}  // namespace qti
}  // namespace vendor
