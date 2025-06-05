/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef VENDOR_QTI_GNSS_LOCAIDLGNSSCONFIGSERVICE_H
#define VENDOR_QTI_GNSS_LOCAIDLGNSSCONFIGSERVICE_H

#include "LocAidlUtils.h"
#include <aidl/vendor/qti/gnss/BnLocAidlGnssConfigService.h>
#include <location_interface.h>

namespace aidl {
namespace vendor {
namespace qti {
namespace gnss {
namespace implementation {


using ::aidl::vendor::qti::gnss::ILocAidlGnssConfigService;
using ::aidl::vendor::qti::gnss::ILocAidlGnssConfigServiceCallback;
using ::aidl::vendor::qti::gnss::LocAidlGnssConstellationType;
using ::aidl::vendor::qti::gnss::LocAidlRobustLocationInfo;
using ::aidl::vendor::qti::gnss::LocAidlNtripConnectionParams;

struct LocAidlGnssConfigService : public BnLocAidlGnssConfigService {
    static std::shared_ptr<ILocAidlGnssConfigServiceCallback> sCallbackIface;
    LocAidlGnssConfigService();
    virtual ~LocAidlGnssConfigService();
    ::ndk::ScopedAStatus getGnssSvTypeConfig() override;
    ::ndk::ScopedAStatus resetGnssSvTypeConfig() override;
    // Methods from ::vendor::qti::gnss::ILocAidlGnssConfigService follow.
    ::ndk::ScopedAStatus init(const std::shared_ptr<ILocAidlGnssConfigServiceCallback>& callback,
            bool* _aidl_return) override;
    ::ndk::ScopedAStatus setGnssSvTypeConfig(
            const std::vector<LocAidlGnssConstellationType>& disabledSvTypeList) override;
    ::ndk::ScopedAStatus getRobustLocationConfig() override;
    ::ndk::ScopedAStatus setRobustLocationConfig(bool enable, bool enableForE911) override;
    // Precise location
    ::ndk::ScopedAStatus updateNTRIPGGAConsent(bool consentAccepted) override;
    ::ndk::ScopedAStatus enablePPENtripStream(const LocAidlNtripConnectionParams& params,
            bool enableRTKEngine) override;
    ::ndk::ScopedAStatus disablePPENtripStream() override;
    ::ndk::ScopedAStatus configMerkleTree(const std::string& in_merkleTreeXml, int32_t in_xmlSize,
            bool* _aidl_return) {return ndk::ScopedAStatus::ok();}
    ::ndk::ScopedAStatus configOsnmaEnablement(bool in_isEnabled, bool* _aidl_return)
            {return ndk::ScopedAStatus::ok();}

    void handleAidlClientSsr();
private:
    const GnssInterface* getGnssInterface();
    static void getGnssSvTypeConfigCallback(const GnssSvTypeConfig& config);
    template <typename T>
    static bool svTypeToConfigCommon(T svType, GnssSvTypeConfig& config);
    static void svTypeVecToConfig(
            const std::vector<LocAidlGnssConstellationType>& disabledSvTypeVec,
            GnssSvTypeConfig& config);
    static void svTypeConfigToVec(const GnssSvTypeConfig& config,
                                      std::vector<LocAidlGnssConstellationType>& disabledSvTypeVec);
    template <typename T>
    static void svTypeConfigToVecCommon(const GnssSvTypeConfig& config, std::vector<T>& svTypeVec);
    template <typename T>
    static bool svTypeToMaskUpdate(T svType, GnssSvTypesMask& mask);
    static LocAidlGnssConstellationType intToGnssConstellation(int32_t svTypeInt);
    static int32_t gnssConstellationToInt(LocAidlGnssConstellationType svType);
    template <typename T>
    static int32_t gnssConstellationToIntCommon(T svType);
private:
    const GnssInterface* mGnssInterface = nullptr;
    bool mGetGnssInterfaceFailed = false;
    LocationControlCallbacks mControlCallbacks;
    static std::shared_ptr<LocAidlDeathRecipient> mDeathRecipient;
};

}  // namespace implementation
}  // namespace gnss
}  // namespace qti
}  // namespace vendor
}  // namespace aidl

#endif  // VENDOR_QTI_GNSS__LOCHIDLGNSSCONFIGSERVICE_H
