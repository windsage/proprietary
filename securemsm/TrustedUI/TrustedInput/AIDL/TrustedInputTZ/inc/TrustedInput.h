/*===================================================================================
  Copyright (c) 2019-2021, 2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===================================================================================*/

#ifndef TRUSTEDINPUT_H
#define TRUSTEDINPUT_H

#include <fcntl.h>
#include <memory>
#include <string>
#include <thread>
#include <aidl/vendor/qti/hardware/trustedui/BnTrustedInput.h>

namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace trustedui {

using ::aidl::vendor::qti::hardware::trustedui::ITrustedInput;
using ::aidl::vendor::qti::hardware::trustedui::ITrustedInputCallback;
using ::aidl::vendor::qti::hardware::trustedui::TUIResponse;
using ::ndk::ScopedAStatus;
using namespace std;

class TouchInput : public BnTrustedInput {
  public:
    /* Methods from ::aidl::vendor::qti::hardware::trustedui::ITrustedInput
     * follow. */
    ScopedAStatus init(const std::shared_ptr<ITrustedInputCallback>& cb,
                       const std::string& displayType, TUIOutputID* outParam,
                       TUIResponse* _aidl_return) override;

    ScopedAStatus terminate(TUIResponse* _aidl_return) override;

    ScopedAStatus getInput(int32_t in_timeout, TUIResponse* _aidl_return) override;

  private:
    std::shared_ptr<std::thread> mThread = nullptr;
    int32_t mAbortFd = -1;
    std::shared_ptr<ITrustedInputCallback> mCB = nullptr;
    uint8_t stSession = false;
    int mControlFd = -1;
    int mIrqFd = -1;
    int32_t mTouchControllerId = -1;
    std::string mControlFile;
    std::string mIrqFile;

    void _waitForInputThreadFunc(int32_t _timeout);
    int32_t _stStartSession(void);
    int32_t _stTerminateSession(void);
};

}  // namespace trustedui
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
}  // namespace aidl

#endif
