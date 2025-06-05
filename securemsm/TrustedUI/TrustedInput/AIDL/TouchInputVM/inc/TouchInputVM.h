/*===================================================================================
  Copyright (c) 2020-2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===================================================================================*/

#ifndef __TOUCHINPUTVM_H__
#define __TOUCHINPUTVM_H__

/******************************************************************************
 *                         Header Inclusions
 *****************************************************************************/

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

class TouchInputVM : public BnTrustedInput {
private:
  static std::mutex sLock;
  static std::shared_ptr<TouchInputVM> sInstance;

public:
  /* Methods from ::aidl::vendor::qti::hardware::trustedui::ITrustedInput
   * follow. */
  /* Description :  This API starts the TouchInput session in Android
   *
   * Return :  Response::TUI_SUCCESS along with mTouchControllerId
   *           Response::TUI_FAILURE along with mTouchControllerId
   *
   */
   ::ndk::ScopedAStatus init(const std::shared_ptr<ITrustedInputCallback>& in_cb,
                             const std::string& in_displayType, TUIOutputID* out_outParam,
                             TUIResponse* _aidl_return) override;


  /* Description :  This API terminates the TouchInput session in Android
   *
   * Return :  Response::TUI_SUCCESS,
   *           Response::TUI_FAILURE
   *
   */
   ::ndk::ScopedAStatus terminate(TUIResponse* _aidl_return) override;

  /* Description :  This API always returns "Response::TUI_FAILURE",
   * as this is mainly executed by LEVM TouchInput.
   *
   * Return :  Response::TUI_FAILURE
   *
   *
   */
   ::ndk::ScopedAStatus getInput(int32_t in_timeout, TUIResponse* _aidl_return) override;

};


}  // namespace trustedui
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
}  // namespace aidl


#endif /* __TOUCHINPUTVM_H__ */
