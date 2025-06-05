/******************************************************************************
#  Copyright (c) 2021, 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <signal.h>

#include <hardware_legacy/power.h>
#include "MessageCommon.h"
#include "QtiMutex.h"
#include "BnIwlanServiceBase.h"

using namespace rildata;

namespace aidl::vendor::qti::hardware::data::iwlandata {

class IWlanServiceBase: public BnIwlanServiceBase {
public:
    virtual ~IWlanServiceBase() {}

    void onDataRegistrationStateChange();
    void onDataCallListChange(std::vector<DataCallResult_t> );
    void onQualifiedNetworksChange(std::vector<QualifiedNetwork_t> );
    void onSetupDataCallIWlanResponseIndMessage(SetupDataCallResponse_t , int32_t , Message::Callback::Status );
    void onDeactivateDataCallIWlanResponseIndMessage(ResponseError_t , int32_t , Message::Callback::Status );
    void onUnthrottleApn(rildata::DataProfileInfo_t profile);
    void acknowledgeRequest(int32_t );
    void clearResponseFunctions();
    void deathNotifier();

protected:
    static std::mutex wakelockMutex;
    static uint8_t wakelockRefCount;
    static timer_t wakelockTimerId;
    static std::string iwlanSvcWakelock;
    static AIBinder_DeathRecipient* mDeathRecipient;

    qtimutex::QtiSharedMutex mCallbackLock;
    static void acquireWakeLock();
    static void releaseWakeLock();
    static void wakeLockTimeoutHdlr(union sigval sval);
    static void resetWakeLock();

    std::shared_ptr<IIWlanResponse> mIWlanResponse;
    std::shared_ptr<IIWlanIndication> mIWlanIndication;

    std::shared_ptr<IIWlanResponse> getIWlanResponse();
    std::shared_ptr<IIWlanIndication> getIWlanIndication();

    /*NDK APIS*/
    ::ndk::ScopedAStatus deactivateDataCall(int32_t /*in_serial*/,
                                            int32_t /*in_cid*/,
                                            DataRequestReason /*in_reason*/) override;
    ::ndk::ScopedAStatus getAllQualifiedNetworks(int32_t /*in_serial*/) override;
    ::ndk::ScopedAStatus getDataCallList(int32_t /*in_serial*/) override;
    ::ndk::ScopedAStatus getDataRegistrationState(int32_t /*in_serial*/) override;
    ::ndk::ScopedAStatus iwlanDisabled() override;
    ::ndk::ScopedAStatus responseAcknowledgement() override;
    ::ndk::ScopedAStatus setResponseFunctions(const std::shared_ptr<IIWlanResponse>& /*in_dataResponse*/,
                                              const std::shared_ptr<IIWlanIndication>& /*in_dataIndication*/) override;
    ::ndk::ScopedAStatus setupDataCall(int32_t /*in_serial*/,
                                       AccessNetwork /*in_accessNetwork*/,
                                       const DataProfileInfo& /*in_dataProfileInfo*/,
                                       bool /*in_roamingAllowed*/,
                                       DataRequestReason /*in_reason*/,
                                       const std::vector<LinkAddress>& /*in_addresses*/,
                                       const std::vector<std::string>& /*in_dnses*/,
                                       int32_t /*in_pduSessionId*/,
                                       const std::optional<SliceInfo>& /*in_sliceInfo*/,
                                       bool /*in_matchAllRuleAllowed*/) override;
};

}
