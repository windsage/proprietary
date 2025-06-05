/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * All rights reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <aidl/vendor/qti/gnss/BnLocAidlIzatProvider.h>
#include <aidl/vendor/qti/gnss/ILocAidlIzatProviderCallback.h>
#include <aidl/vendor/qti/gnss/LocAidlIzatStreamType.h>
#include <aidl/vendor/qti/gnss/LocAidlIzatLocation.h>
#include <aidl/vendor/qti/gnss/LocAidlIzatProviderStatus.h>
#include <aidl/vendor/qti/gnss/LocAidlIzatRequest.h>
#include <IzatTypes.h>
#include <IIzatManager.h>
#include <IOSListener.h>
#include <IzatRequest.h>
#include <memory>
#include "LocAidlUtils.h"

namespace aidl {
namespace vendor {
namespace qti {
namespace gnss {
namespace implementation {


using ::aidl::vendor::qti::gnss::ILocAidlIzatProvider;
using ::aidl::vendor::qti::gnss::ILocAidlIzatProviderCallback;
using namespace izat_manager;

class Listener;
struct LocAidlIzatProvider : public BnLocAidlIzatProvider {
    LocAidlIzatProvider(::aidl::vendor::qti::gnss::LocAidlIzatStreamType privderType);
    virtual inline ~LocAidlIzatProvider() { mDeathRecipient = nullptr; }
    // Methods from ::aidl::vendor::qti::gnss::ILocAidlIzatProvider follow.
    ::ndk::ScopedAStatus init(
            const std::shared_ptr<ILocAidlIzatProviderCallback>& callback,
            bool* _aidl_return) override;
    ::ndk::ScopedAStatus deinit() override;
    ::ndk::ScopedAStatus onEnable(bool* _aidl_return) override;
    ::ndk::ScopedAStatus onDisable(bool* _aidl_return) override;
    ::ndk::ScopedAStatus onAddRequest(
            const ::aidl::vendor::qti::gnss::LocAidlIzatRequest& request,
            bool* _aidl_return) override;
    ::ndk::ScopedAStatus onRemoveRequest(
            const ::aidl::vendor::qti::gnss::LocAidlIzatRequest& request,
            bool* _aidl_return) override;

    void createIzatRequest(
            IzatRequest& izatRequest,
            const ::aidl::vendor::qti::gnss::LocAidlIzatRequest& gnssIzatRequest);

    static std::shared_ptr<LocAidlDeathRecipient> mDeathRecipient;
    std::shared_ptr<ILocAidlIzatProviderCallback> mCallbackIface;
    Listener* mListener;
    IIzatManager* mIzatManager;
    const IzatStreamType mProviderType;
};

}  // namespace implementation
}  // namespace AIDL
}  // namespace gnss
}  // namespace qti
}  // namespace vendor
