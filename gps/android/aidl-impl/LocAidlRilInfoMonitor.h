/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <aidl/vendor/qti/gnss/BnLocAidlRilInfoMonitor.h>
#include <LBSAdapter.h>

namespace aidl {
namespace vendor {
namespace qti {
namespace gnss {
namespace implementation {

using namespace std;
using ::aidl::vendor::qti::gnss::ILocAidlRilInfoMonitor;

struct LocAidlRilInfoMonitor : public BnLocAidlRilInfoMonitor {
    LocAidlRilInfoMonitor() = default;
    virtual ~LocAidlRilInfoMonitor() = default;
    // Methods from ::vendor::qti::gnss::ILocAidlRilInfoMonitor follow.
    ::ndk::ScopedAStatus init() override;
    ::ndk::ScopedAStatus cinfoInject(int32_t cid, int32_t lac,
            int32_t mnc, int32_t mcc, bool roaming) override;
    ::ndk::ScopedAStatus oosInform() override;
    ::ndk::ScopedAStatus niSuplInit(const string& str) override;
    ::ndk::ScopedAStatus chargerStatusInject(int32_t status) override;

private:
    LBSAdapter* mLBSAdapter = NULL;

};

}  // namespace implementation
}  // namespace aidl
}  // namespace gnss
}  // namespace qti
}  // namespace vendor
